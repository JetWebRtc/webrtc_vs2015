﻿/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/pacing/bitrate_prober.h"

#include <algorithm>

#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"
#include "webrtc/modules/pacing/paced_sender.h"

namespace webrtc
{

namespace
{

// A minimum interval between probes to allow scheduling to be feasible.
constexpr int kMinProbeDeltaMs = 1;

// The minimum number probing packets used.
constexpr int kMinProbePacketsSent = 5;

// The minimum probing duration in ms.
constexpr int kMinProbeDurationMs = 15;

// Maximum amount of time each probe can be delayed. Probe cluster is reset and
// retried from the start when this limit is reached.
constexpr int kMaxProbeDelayMs = 3;

// Number of times probing is retried before the cluster is dropped.
constexpr int kMaxRetryAttempts = 3;

// The min probe packet size is scaled with the bitrate we're probing at.
// This defines the max min probe packet size, meaning that on high bitrates
// we have a min probe packet size of 200 bytes.
constexpr size_t kMinProbePacketSize = 200;

constexpr int64_t kProbeClusterTimeoutMs = 5000;

}  // namespace

BitrateProber::BitrateProber()
    : probing_state_(ProbingState::kDisabled),
      next_probe_time_ms_(-1),
      next_cluster_id_(0)
{
    SetEnabled(true);
}

void BitrateProber::SetEnabled(bool enable)
{
    if (enable)
    {
        if (probing_state_ == ProbingState::kDisabled)
        {
            probing_state_ = ProbingState::kInactive;
            LOG(LS_INFO) << "Bandwidth probing enabled, set to inactive";
        }
    }
    else
    {
        probing_state_ = ProbingState::kDisabled;
        LOG(LS_INFO) << "Bandwidth probing disabled";
    }
}

bool BitrateProber::IsProbing() const
{
    return probing_state_ == ProbingState::kActive;
}

void BitrateProber::OnIncomingPacket(size_t packet_size)
{
    // Don't initialize probing unless we have something large enough to start
    // probing.
    if (probing_state_ == ProbingState::kInactive && !clusters_.empty() &&
            packet_size >=
            std::min<size_t>(RecommendedMinProbeSize(), kMinProbePacketSize))
    {
        // Send next probe right away.
        next_probe_time_ms_ = -1;
        probing_state_ = ProbingState::kActive;
    }
}

void BitrateProber::CreateProbeCluster(int bitrate_bps, int64_t now_ms)
{
    RTC_DCHECK(probing_state_ != ProbingState::kDisabled);
    while (!clusters_.empty() &&
            now_ms - clusters_.front().time_created_ms > kProbeClusterTimeoutMs)
    {
        clusters_.pop();
    }

    ProbeCluster cluster;
    cluster.min_probes = kMinProbePacketsSent;
    cluster.min_bytes = bitrate_bps * kMinProbeDurationMs / 8000;
    cluster.bitrate_bps = bitrate_bps;
    cluster.time_created_ms = now_ms;
    cluster.id = next_cluster_id_++;
    clusters_.push(cluster);

    LOG(LS_INFO) << "Probe cluster (bitrate:min bytes:min packets): ("
                 << cluster.bitrate_bps << ":" << cluster.min_bytes << ":"
                 << cluster.min_probes << ")";
    // If we are already probing, continue to do so. Otherwise set it to
    // kInactive and wait for OnIncomingPacket to start the probing.
    if (probing_state_ != ProbingState::kActive)
        probing_state_ = ProbingState::kInactive;
}

void BitrateProber::ResetState(int64_t now_ms)
{
    RTC_DCHECK(probing_state_ == ProbingState::kActive);

    // Recreate all probing clusters.
    std::queue<ProbeCluster> clusters;
    clusters.swap(clusters_);
    while (!clusters.empty())
    {
        if (clusters.front().retries < kMaxRetryAttempts)
        {
            CreateProbeCluster(clusters.front().bitrate_bps, now_ms);
            clusters_.back().retries = clusters.front().retries + 1;
        }
        clusters.pop();
    }

    probing_state_ = ProbingState::kInactive;
}

int BitrateProber::TimeUntilNextProbe(int64_t now_ms)
{
    // Probing is not active or probing is already complete.
    if (probing_state_ != ProbingState::kActive || clusters_.empty())
        return -1;

    int time_until_probe_ms = 0;
    if (next_probe_time_ms_ >= 0)
    {
        time_until_probe_ms = next_probe_time_ms_ - now_ms;
        if (time_until_probe_ms < -kMaxProbeDelayMs)
        {
            ResetState(now_ms);
            return -1;
        }
    }

    return std::max(time_until_probe_ms, 0);
}

int BitrateProber::CurrentClusterId() const
{
    RTC_DCHECK(!clusters_.empty());
    RTC_DCHECK(ProbingState::kActive == probing_state_);
    return clusters_.front().id;
}

// Probe size is recommended based on the probe bitrate required. We choose
// a minimum of twice |kMinProbeDeltaMs| interval to allow scheduling to be
// feasible.
size_t BitrateProber::RecommendedMinProbeSize() const
{
    RTC_DCHECK(!clusters_.empty());
    return clusters_.front().bitrate_bps * 2 * kMinProbeDeltaMs / (8 * 1000);
}

void BitrateProber::ProbeSent(int64_t now_ms, size_t bytes)
{
    RTC_DCHECK(probing_state_ == ProbingState::kActive);
    RTC_DCHECK_GT(bytes, 0);

    if (!clusters_.empty())
    {
        ProbeCluster* cluster = &clusters_.front();
        if (cluster->sent_probes == 0)
        {
            RTC_DCHECK_EQ(cluster->time_started_ms, -1);
            cluster->time_started_ms = now_ms;
        }
        cluster->sent_bytes += static_cast<int>(bytes);
        cluster->sent_probes += 1;
        next_probe_time_ms_ = GetNextProbeTime(clusters_.front());
        if (cluster->sent_bytes >= cluster->min_bytes &&
                cluster->sent_probes >= cluster->min_probes)
        {
            clusters_.pop();
        }
        if (clusters_.empty())
            probing_state_ = ProbingState::kSuspended;
    }
}

int64_t BitrateProber::GetNextProbeTime(const ProbeCluster& cluster)
{
    RTC_CHECK_GT(cluster.bitrate_bps, 0);
    RTC_CHECK_GE(cluster.time_started_ms, 0);

    // Compute the time delta from the cluster start to ensure probe bitrate stays
    // close to the target bitrate. Result is in milliseconds.
    int64_t delta_ms = (8000ll * cluster.sent_bytes + cluster.bitrate_bps / 2) /
                       cluster.bitrate_bps;
    return cluster.time_started_ms + delta_ms;
}


}  // namespace webrtc

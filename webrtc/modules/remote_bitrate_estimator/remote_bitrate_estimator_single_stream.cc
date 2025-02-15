﻿/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/remote_bitrate_estimator/remote_bitrate_estimator_single_stream.h"

#include <utility>

#include "webrtc/base/constructormagic.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/thread_annotations.h"
#include "webrtc/modules/remote_bitrate_estimator/aimd_rate_control.h"
#include "webrtc/modules/remote_bitrate_estimator/inter_arrival.h"
#include "webrtc/modules/remote_bitrate_estimator/overuse_detector.h"
#include "webrtc/modules/remote_bitrate_estimator/overuse_estimator.h"
#include "webrtc/system_wrappers/include/clock.h"
#include "webrtc/system_wrappers/include/critical_section_wrapper.h"
#include "webrtc/system_wrappers/include/metrics.h"
#include "webrtc/typedefs.h"

namespace webrtc
{

enum { kTimestampGroupLengthMs = 5 };
static const double kTimestampToMs = 1.0 / 90.0;

struct RemoteBitrateEstimatorSingleStream::Detector
{
    explicit Detector(int64_t last_packet_time_ms,
                      const OverUseDetectorOptions& options,
                      bool enable_burst_grouping)
        : last_packet_time_ms(last_packet_time_ms),
          inter_arrival(90 * kTimestampGroupLengthMs,
                        kTimestampToMs,
                        enable_burst_grouping),
          estimator(options),
          detector() {}
    int64_t last_packet_time_ms;
    InterArrival inter_arrival;
    OveruseEstimator estimator;
    OveruseDetector detector;
};

RemoteBitrateEstimatorSingleStream::RemoteBitrateEstimatorSingleStream(
    RemoteBitrateObserver* observer,
    Clock* clock)
    : clock_(clock),
      incoming_bitrate_(kBitrateWindowMs, 8000),
      last_valid_incoming_bitrate_(0),
      remote_rate_(new AimdRateControl()),
      observer_(observer),
      crit_sect_(CriticalSectionWrapper::CreateCriticalSection()),
      last_process_time_(-1),
      process_interval_ms_(kProcessIntervalMs),
      uma_recorded_(false)
{
    assert(observer_);
    LOG(LS_INFO) << "RemoteBitrateEstimatorSingleStream: Instantiating.";
}

RemoteBitrateEstimatorSingleStream::~RemoteBitrateEstimatorSingleStream()
{
    while (!overuse_detectors_.empty())
    {
        SsrcOveruseEstimatorMap::iterator it = overuse_detectors_.begin();
        delete it->second;
        overuse_detectors_.erase(it);
    }
}

void RemoteBitrateEstimatorSingleStream::IncomingPacket(
    int64_t arrival_time_ms,
    size_t payload_size,
    const RTPHeader& header)
{
    if (!uma_recorded_)
    {
        BweNames type = BweNames::kReceiverTOffset;
        if (!header.extension.hasTransmissionTimeOffset)
            type = BweNames::kReceiverNoExtension;
        RTC_HISTOGRAM_ENUMERATION(kBweTypeHistogram, type, BweNames::kBweNamesMax);
        uma_recorded_ = true;
    }
    uint32_t ssrc = header.ssrc;
    uint32_t rtp_timestamp = header.timestamp +
                             header.extension.transmissionTimeOffset;
    int64_t now_ms = clock_->TimeInMilliseconds();
    CriticalSectionScoped cs(crit_sect_.get());
    SsrcOveruseEstimatorMap::iterator it = overuse_detectors_.find(ssrc);
    if (it == overuse_detectors_.end())
    {
        // This is a new SSRC. Adding to map.
        // TODO(holmer): If the channel changes SSRC the old SSRC will still be
        // around in this map until the channel is deleted. This is OK since the
        // callback will no longer be called for the old SSRC. This will be
        // automatically cleaned up when we have one RemoteBitrateEstimator per REMB
        // group.
        std::pair<SsrcOveruseEstimatorMap::iterator, bool> insert_result =
            overuse_detectors_.insert(std::make_pair(
                                          ssrc, new Detector(now_ms, OverUseDetectorOptions(), true)));
        it = insert_result.first;
    }
    Detector* estimator = it->second;
    estimator->last_packet_time_ms = now_ms;

    // Check if incoming bitrate estimate is valid, and if it needs to be reset.
    rtc::Optional<uint32_t> incoming_bitrate = incoming_bitrate_.Rate(now_ms);
    if (incoming_bitrate)
    {
        last_valid_incoming_bitrate_ = *incoming_bitrate;
    }
    else if (last_valid_incoming_bitrate_ > 0)
    {
        // Incoming bitrate had a previous valid value, but now not enough data
        // point are left within the current window. Reset incoming bitrate
        // estimator so that the window size will only contain new data points.
        incoming_bitrate_.Reset();
        last_valid_incoming_bitrate_ = 0;
    }
    incoming_bitrate_.Update(payload_size, now_ms);

    const BandwidthUsage prior_state = estimator->detector.State();
    uint32_t timestamp_delta = 0;
    int64_t time_delta = 0;
    int size_delta = 0;
    if (estimator->inter_arrival.ComputeDeltas(
                rtp_timestamp, arrival_time_ms, now_ms, payload_size,
                &timestamp_delta, &time_delta, &size_delta))
    {
        double timestamp_delta_ms = timestamp_delta * kTimestampToMs;
        estimator->estimator.Update(time_delta, timestamp_delta_ms, size_delta,
                                    estimator->detector.State(), now_ms);
        estimator->detector.Detect(estimator->estimator.offset(),
                                   timestamp_delta_ms,
                                   estimator->estimator.num_of_deltas(), now_ms);
    }
    if (estimator->detector.State() == kBwOverusing)
    {
        rtc::Optional<uint32_t> incoming_bitrate_bps =
            incoming_bitrate_.Rate(now_ms);
        if (incoming_bitrate_bps &&
                (prior_state != kBwOverusing ||
                 GetRemoteRate()->TimeToReduceFurther(now_ms, *incoming_bitrate_bps)))
        {
            // The first overuse should immediately trigger a new estimate.
            // We also have to update the estimate immediately if we are overusing
            // and the target bitrate is too high compared to what we are receiving.
            UpdateEstimate(now_ms);
        }
    }
}

void RemoteBitrateEstimatorSingleStream::Process()
{
    if (TimeUntilNextProcess() > 0)
    {
        return;
    }
    {
        CriticalSectionScoped cs(crit_sect_.get());
        UpdateEstimate(clock_->TimeInMilliseconds());
    }
    last_process_time_ = clock_->TimeInMilliseconds();
}

int64_t RemoteBitrateEstimatorSingleStream::TimeUntilNextProcess()
{
    if (last_process_time_ < 0)
    {
        return 0;
    }
    {
        CriticalSectionScoped cs_(crit_sect_.get());
        return last_process_time_ + process_interval_ms_ -
               clock_->TimeInMilliseconds();
    }
}

void RemoteBitrateEstimatorSingleStream::UpdateEstimate(int64_t now_ms)
{
    BandwidthUsage bw_state = kBwNormal;
    double sum_var_noise = 0.0;
    SsrcOveruseEstimatorMap::iterator it = overuse_detectors_.begin();
    while (it != overuse_detectors_.end())
    {
        const int64_t time_of_last_received_packet =
            it->second->last_packet_time_ms;
        if (time_of_last_received_packet >= 0 &&
                now_ms - time_of_last_received_packet > kStreamTimeOutMs)
        {
            // This over-use detector hasn't received packets for |kStreamTimeOutMs|
            // milliseconds and is considered stale.
            delete it->second;
            overuse_detectors_.erase(it++);
        }
        else
        {
            sum_var_noise += it->second->estimator.var_noise();
            // Make sure that we trigger an over-use if any of the over-use detectors
            // is detecting over-use.
            if (it->second->detector.State() > bw_state)
            {
                bw_state = it->second->detector.State();
            }
            ++it;
        }
    }
    // We can't update the estimate if we don't have any active streams.
    if (overuse_detectors_.empty())
    {
        return;
    }
    AimdRateControl* remote_rate = GetRemoteRate();

    double mean_noise_var = sum_var_noise /
                            static_cast<double>(overuse_detectors_.size());
    const RateControlInput input(bw_state,
                                 incoming_bitrate_.Rate(now_ms),
                                 mean_noise_var);
    remote_rate->Update(&input, now_ms);
    uint32_t target_bitrate = remote_rate->UpdateBandwidthEstimate(now_ms);
    if (remote_rate->ValidEstimate())
    {
        process_interval_ms_ = remote_rate->GetFeedbackInterval();
        std::vector<uint32_t> ssrcs;
        GetSsrcs(&ssrcs);
        observer_->OnReceiveBitrateChanged(ssrcs, target_bitrate);
    }
}

void RemoteBitrateEstimatorSingleStream::OnRttUpdate(int64_t avg_rtt_ms,
        int64_t max_rtt_ms)
{
    CriticalSectionScoped cs(crit_sect_.get());
    GetRemoteRate()->SetRtt(avg_rtt_ms);
}

void RemoteBitrateEstimatorSingleStream::RemoveStream(unsigned int ssrc)
{
    CriticalSectionScoped cs(crit_sect_.get());
    SsrcOveruseEstimatorMap::iterator it = overuse_detectors_.find(ssrc);
    if (it != overuse_detectors_.end())
    {
        delete it->second;
        overuse_detectors_.erase(it);
    }
}

bool RemoteBitrateEstimatorSingleStream::LatestEstimate(
    std::vector<uint32_t>* ssrcs,
    uint32_t* bitrate_bps) const
{
    CriticalSectionScoped cs(crit_sect_.get());
    assert(bitrate_bps);
    if (!remote_rate_->ValidEstimate())
    {
        return false;
    }
    GetSsrcs(ssrcs);
    if (ssrcs->empty())
        *bitrate_bps = 0;
    else
        *bitrate_bps = remote_rate_->LatestEstimate();
    return true;
}

void RemoteBitrateEstimatorSingleStream::GetSsrcs(
    std::vector<uint32_t>* ssrcs) const
{
    assert(ssrcs);
    ssrcs->resize(overuse_detectors_.size());
    int i = 0;
    for (SsrcOveruseEstimatorMap::const_iterator it = overuse_detectors_.begin();
            it != overuse_detectors_.end(); ++it, ++i)
    {
        (*ssrcs)[i] = it->first;
    }
}

AimdRateControl* RemoteBitrateEstimatorSingleStream::GetRemoteRate()
{
    if (!remote_rate_)
        remote_rate_.reset(new AimdRateControl());
    return remote_rate_.get();
}

void RemoteBitrateEstimatorSingleStream::SetMinBitrate(int min_bitrate_bps)
{
    CriticalSectionScoped cs(crit_sect_.get());
    remote_rate_->SetMinBitrate(min_bitrate_bps);
}

}  // namespace webrtc

﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <memory>

#include "webrtc/base/logging.h"
#include "webrtc/modules/congestion_controller/probe_controller.h"
#include "webrtc/modules/pacing/mock/mock_paced_sender.h"
#include "webrtc/system_wrappers/include/clock.h"
#include "webrtc/test/gmock.h"
#include "webrtc/test/gtest.h"

using testing::_;
using testing::AtLeast;
using testing::NiceMock;
using testing::Return;

namespace webrtc
{
namespace test
{

namespace
{

constexpr int kMinBitrateBps = 100;
constexpr int kStartBitrateBps = 300;
constexpr int kMaxBitrateBps = 10000;

constexpr int kExponentialProbingTimeoutMs = 5000;

constexpr int kAlrProbeInterval = 5000;

}  // namespace

class ProbeControllerTest : public ::testing::Test
{
protected:
    ProbeControllerTest() : clock_(100000000L)
    {
        probe_controller_.reset(new ProbeController(&pacer_, &clock_));
    }
    ~ProbeControllerTest() override {}

    SimulatedClock clock_;
    NiceMock<MockPacedSender> pacer_;
    std::unique_ptr<ProbeController> probe_controller_;
};

TEST_F(ProbeControllerTest, InitiatesProbingAtStart)
{
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(AtLeast(2));
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);
}

TEST_F(ProbeControllerTest, ProbeOnlyWhenNetworkIsUp)
{
    probe_controller_->OnNetworkStateChanged(kNetworkDown);
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(0);
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);

    testing::Mock::VerifyAndClearExpectations(&pacer_);
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(AtLeast(2));
    probe_controller_->OnNetworkStateChanged(kNetworkUp);
}

TEST_F(ProbeControllerTest, InitiatesProbingOnMaxBitrateIncrease)
{
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(AtLeast(2));
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);
    // Long enough to time out exponential probing.
    clock_.AdvanceTimeMilliseconds(kExponentialProbingTimeoutMs);
    probe_controller_->SetEstimatedBitrate(kStartBitrateBps);
    probe_controller_->Process();

    EXPECT_CALL(pacer_, CreateProbeCluster(kMaxBitrateBps + 100));
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps + 100);
}

TEST_F(ProbeControllerTest, InitiatesProbingOnMaxBitrateIncreaseAtMaxBitrate)
{
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(AtLeast(2));
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);
    // Long enough to time out exponential probing.
    clock_.AdvanceTimeMilliseconds(kExponentialProbingTimeoutMs);
    probe_controller_->SetEstimatedBitrate(kStartBitrateBps);
    probe_controller_->Process();

    probe_controller_->SetEstimatedBitrate(kMaxBitrateBps);
    EXPECT_CALL(pacer_, CreateProbeCluster(kMaxBitrateBps + 100));
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps + 100);
}

TEST_F(ProbeControllerTest, TestExponentialProbing)
{
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);

    // Repeated probe should only be sent when estimated bitrate climbs above
    // 0.7 * 6 * kStartBitrateBps = 1260.
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(0);
    probe_controller_->SetEstimatedBitrate(1000);
    testing::Mock::VerifyAndClearExpectations(&pacer_);

    EXPECT_CALL(pacer_, CreateProbeCluster(2 * 1800));
    probe_controller_->SetEstimatedBitrate(1800);
}

TEST_F(ProbeControllerTest, TestExponentialProbingTimeout)
{
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);

    // Advance far enough to cause a time out in waiting for probing result.
    clock_.AdvanceTimeMilliseconds(kExponentialProbingTimeoutMs);
    probe_controller_->Process();

    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(0);
    probe_controller_->SetEstimatedBitrate(1800);
}

TEST_F(ProbeControllerTest, ProbeAfterEstimateDropInAlr)
{
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(2);
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);
    probe_controller_->SetEstimatedBitrate(500);
    testing::Mock::VerifyAndClearExpectations(&pacer_);

    // When bandwidth estimate drops the controller should send a probe at the
    // previous bitrate.
    EXPECT_CALL(pacer_, CreateProbeCluster(500)).Times(1);
    EXPECT_CALL(pacer_, GetApplicationLimitedRegionStartTime())
    .WillRepeatedly(
        Return(rtc::Optional<int64_t>(clock_.TimeInMilliseconds())));
    clock_.AdvanceTimeMilliseconds(kAlrProbeInterval + 1);
    probe_controller_->Process();
    probe_controller_->SetEstimatedBitrate(50);
}

TEST_F(ProbeControllerTest, PeriodicProbing)
{
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(2);
    probe_controller_->EnablePeriodicAlrProbing(true);
    probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                   kMaxBitrateBps);
    probe_controller_->SetEstimatedBitrate(500);
    testing::Mock::VerifyAndClearExpectations(&pacer_);

    int64_t start_time = clock_.TimeInMilliseconds();

    // Expect the controller to send a new probe after 5s has passed.
    EXPECT_CALL(pacer_, CreateProbeCluster(1000)).Times(1);
    EXPECT_CALL(pacer_, GetApplicationLimitedRegionStartTime())
    .WillRepeatedly(Return(rtc::Optional<int64_t>(start_time)));
    clock_.AdvanceTimeMilliseconds(5000);
    probe_controller_->Process();
    probe_controller_->SetEstimatedBitrate(500);
    testing::Mock::VerifyAndClearExpectations(&pacer_);

    // The following probe should be sent at 10s into ALR.
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(0);
    EXPECT_CALL(pacer_, GetApplicationLimitedRegionStartTime())
    .WillRepeatedly(Return(rtc::Optional<int64_t>(start_time)));
    clock_.AdvanceTimeMilliseconds(4000);
    probe_controller_->Process();
    probe_controller_->SetEstimatedBitrate(500);
    testing::Mock::VerifyAndClearExpectations(&pacer_);

    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(1);
    EXPECT_CALL(pacer_, GetApplicationLimitedRegionStartTime())
    .WillRepeatedly(Return(rtc::Optional<int64_t>(start_time)));
    clock_.AdvanceTimeMilliseconds(1000);
    probe_controller_->Process();
    probe_controller_->SetEstimatedBitrate(500);
    testing::Mock::VerifyAndClearExpectations(&pacer_);
}

TEST_F(ProbeControllerTest, TestExponentialProbingOverflow)
{
    const int64_t kMbpsMultiplier = 1000000;
    probe_controller_->SetBitrates(kMinBitrateBps, 10 * kMbpsMultiplier,
                                   100 * kMbpsMultiplier);

    // Verify that probe bitrate is capped at the specified max bitrate
    EXPECT_CALL(pacer_, CreateProbeCluster(100 * kMbpsMultiplier));
    probe_controller_->SetEstimatedBitrate(60 * kMbpsMultiplier);
    testing::Mock::VerifyAndClearExpectations(&pacer_);

    // Verify that repeated probes aren't sent.
    EXPECT_CALL(pacer_, CreateProbeCluster(_)).Times(0);
    probe_controller_->SetEstimatedBitrate(100 * kMbpsMultiplier);
}

}  // namespace test
}  // namespace webrtc

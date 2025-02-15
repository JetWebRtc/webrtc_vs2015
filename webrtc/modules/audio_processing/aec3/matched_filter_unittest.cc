﻿/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
  */

#include "webrtc/modules/audio_processing/aec3/matched_filter.h"

#include <algorithm>
#include <sstream>
#include <string>

#include "webrtc/modules/audio_processing/aec3/aec3_constants.h"
#include "webrtc/modules/audio_processing/logging/apm_data_dumper.h"
#include "webrtc/modules/audio_processing/test/echo_canceller_test_tools.h"
#include "webrtc/test/gtest.h"

namespace webrtc
{
namespace
{

std::string ProduceDebugText(size_t delay)
{
    std::ostringstream ss;
    ss << "Delay: " << delay;
    return ss.str();
}

constexpr size_t kWindowSizeSubBlocks = 32;
constexpr size_t kAlignmentShiftSubBlocks = kWindowSizeSubBlocks * 3 / 4;
constexpr size_t kNumMatchedFilters = 4;

}  // namespace

// Verifies that the matched filter produces proper lag estimates for
// artificially
// delayed signals.
TEST(MatchedFilter, LagEstimation)
{
    Random random_generator(42U);
    std::array<float, kSubBlockSize> render;
    std::array<float, kSubBlockSize> capture;
    render.fill(0.f);
    capture.fill(0.f);
    ApmDataDumper data_dumper(0);
    for (size_t delay_samples :
            {
                0, 64, 150, 200, 800, 1000
            })
    {
        SCOPED_TRACE(ProduceDebugText(delay_samples));
        DelayBuffer<float> signal_delay_buffer(delay_samples);
        MatchedFilter filter(&data_dumper, kWindowSizeSubBlocks, kNumMatchedFilters,
                             kAlignmentShiftSubBlocks);

        // Analyze the correlation between render and capture.
        for (size_t k = 0; k < (100 + delay_samples / kSubBlockSize); ++k)
        {
            RandomizeSampleVector(&random_generator, render);
            signal_delay_buffer.Delay(render, capture);
            filter.Update(render, capture);
        }

        // Obtain the lag estimates.
        auto lag_estimates = filter.GetLagEstimates();

        // Find which lag estimate should be the most accurate.
        rtc::Optional<size_t> expected_most_accurate_lag_estimate;
        size_t alignment_shift_sub_blocks = 0;
        for (size_t k = 0; k < kNumMatchedFilters; ++k)
        {
            if ((alignment_shift_sub_blocks + kWindowSizeSubBlocks / 2) *
                    kSubBlockSize >
                    delay_samples)
            {
                expected_most_accurate_lag_estimate = rtc::Optional<size_t>(k);
                break;
            }
            alignment_shift_sub_blocks += kAlignmentShiftSubBlocks;
        }
        ASSERT_TRUE(expected_most_accurate_lag_estimate);

        // Verify that the expected most accurate lag estimate is the most accurate
        // estimate.
        for (size_t k = 0; k < kNumMatchedFilters; ++k)
        {
            if (k != *expected_most_accurate_lag_estimate)
            {
                EXPECT_GT(lag_estimates[*expected_most_accurate_lag_estimate].accuracy,
                          lag_estimates[k].accuracy);
            }
        }

        // Verify that all lag estimates are updated as expected for signals
        // containing strong noise.
        for (auto& le : lag_estimates)
        {
            EXPECT_TRUE(le.updated);
        }

        // Verify that the expected most accurate lag estimate is reliable.
        EXPECT_TRUE(lag_estimates[*expected_most_accurate_lag_estimate].reliable);

        // Verify that the expected most accurate lag estimate is correct.
        EXPECT_EQ(delay_samples,
                  lag_estimates[*expected_most_accurate_lag_estimate].lag);
    }
}

// Verifies that the matched filter does not produce reliable and accurate
// estimates for uncorrelated render and capture signals.
TEST(MatchedFilter, LagNotReliableForUncorrelatedRenderAndCapture)
{
    Random random_generator(42U);
    std::array<float, kSubBlockSize> render;
    std::array<float, kSubBlockSize> capture;
    render.fill(0.f);
    capture.fill(0.f);
    ApmDataDumper data_dumper(0);
    MatchedFilter filter(&data_dumper, kWindowSizeSubBlocks, kNumMatchedFilters,
                         kAlignmentShiftSubBlocks);

    // Analyze the correlation between render and capture.
    for (size_t k = 0; k < 100; ++k)
    {
        RandomizeSampleVector(&random_generator, render);
        RandomizeSampleVector(&random_generator, capture);
        filter.Update(render, capture);
    }

    // Obtain the lag estimates.
    auto lag_estimates = filter.GetLagEstimates();
    EXPECT_EQ(kNumMatchedFilters, lag_estimates.size());

    // Verify that no lag estimates are reliable.
    for (auto& le : lag_estimates)
    {
        EXPECT_FALSE(le.reliable);
    }
}

// Verifies that the matched filter does not produce updated lag estimates for
// render signals of low level.
TEST(MatchedFilter, LagNotUpdatedForLowLevelRender)
{
    Random random_generator(42U);
    std::array<float, kSubBlockSize> render;
    std::array<float, kSubBlockSize> capture;
    render.fill(0.f);
    capture.fill(0.f);
    ApmDataDumper data_dumper(0);
    MatchedFilter filter(&data_dumper, kWindowSizeSubBlocks, kNumMatchedFilters,
                         kAlignmentShiftSubBlocks);

    // Analyze the correlation between render and capture.
    for (size_t k = 0; k < 100; ++k)
    {
        RandomizeSampleVector(&random_generator, render);
        for (auto& render_k : render)
        {
            render_k *= 149.f / 32767.f;
        }
        std::copy(render.begin(), render.end(), capture.begin());
        filter.Update(render, capture);
    }

    // Obtain the lag estimates.
    auto lag_estimates = filter.GetLagEstimates();
    EXPECT_EQ(kNumMatchedFilters, lag_estimates.size());

    // Verify that no lag estimates are updated and that no lag estimates are
    // reliable.
    for (auto& le : lag_estimates)
    {
        EXPECT_FALSE(le.updated);
        EXPECT_FALSE(le.reliable);
    }
}

// Verifies that the correct number of lag estimates are produced for a certain
// number of alignment shifts.
TEST(MatchedFilter, NumberOfLagEstimates)
{
    ApmDataDumper data_dumper(0);
    for (size_t num_matched_filters = 0; num_matched_filters < 10;
            ++num_matched_filters)
    {
        MatchedFilter filter(&data_dumper, 32, num_matched_filters, 1);
        EXPECT_EQ(num_matched_filters, filter.GetLagEstimates().size());
    }
}

#if RTC_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(WEBRTC_ANDROID)

// Verifies the check for non-zero windows size.
TEST(MatchedFilter, ZeroWindowSize)
{
    ApmDataDumper data_dumper(0);
    EXPECT_DEATH(MatchedFilter(&data_dumper, 0, 1, 1), "");
}

// Verifies the check for non-null data dumper.
TEST(MatchedFilter, NullDataDumper)
{
    EXPECT_DEATH(MatchedFilter(nullptr, 1, 1, 1), "");
}

#endif

}  // namespace webrtc

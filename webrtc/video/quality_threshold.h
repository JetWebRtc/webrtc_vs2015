﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VIDEO_QUALITY_THRESHOLD_H_
#define WEBRTC_VIDEO_QUALITY_THRESHOLD_H_

#include <memory>

#include "webrtc/base/optional.h"

namespace webrtc
{

class QualityThreshold
{
public:
    // Both thresholds are inclusive, i.e. measurement >= high signifies a high
    // state, while measurement <= low signifies a low state.
    QualityThreshold(int low_threshold,
                     int high_threshold,
                     float fraction,
                     int max_measurements);

    void AddMeasurement(int measurement);
    rtc::Optional<bool> IsHigh() const;
    rtc::Optional<double> CalculateVariance() const;
    rtc::Optional<double> FractionHigh(int min_required_samples) const;

private:
    const std::unique_ptr<int[]> buffer_;
    const int max_measurements_;
    const float fraction_;
    const int low_threshold_;
    const int high_threshold_;
    int until_full_;
    int next_index_;
    rtc::Optional<bool> is_high_;
    int sum_;
    int count_low_;
    int count_high_;
    int num_high_states_;
    int num_certain_states_;
};

}  // namespace webrtc

#endif  // WEBRTC_VIDEO_QUALITY_THRESHOLD_H_

﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_MIXER_DEFAULT_OUTPUT_RATE_CALCULATOR_H_
#define WEBRTC_MODULES_AUDIO_MIXER_DEFAULT_OUTPUT_RATE_CALCULATOR_H_

#include <vector>

#include "webrtc/modules/audio_mixer/output_rate_calculator.h"

namespace webrtc
{

class DefaultOutputRateCalculator : public OutputRateCalculator
{
public:
    static const int kDefaultFrequency = 48000;

    // Produces the least native rate greater or equal to the preferred
    // sample rates. A native rate is one in
    // AudioProcessing::NativeRate. If |preferred_sample_rates| is
    // empty, returns |kDefaultFrequency|.
    int CalculateOutputRate(
        const std::vector<int>& preferred_sample_rates) override;
    ~DefaultOutputRateCalculator() override {}
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_MIXER_DEFAULT_OUTPUT_RATE_CALCULATOR_H_

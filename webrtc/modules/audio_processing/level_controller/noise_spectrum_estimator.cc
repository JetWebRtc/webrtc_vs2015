﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_processing/level_controller/noise_spectrum_estimator.h"

#include <string.h>
#include <algorithm>

#include "webrtc/base/array_view.h"
#include "webrtc/base/arraysize.h"
#include "webrtc/modules/audio_processing/logging/apm_data_dumper.h"

namespace webrtc
{
namespace
{
float kMinNoisePower = 100.f;
}  // namespace

NoiseSpectrumEstimator::NoiseSpectrumEstimator(ApmDataDumper* data_dumper)
    : data_dumper_(data_dumper)
{
    Initialize();
}

void NoiseSpectrumEstimator::Initialize()
{
    std::fill(noise_spectrum_, noise_spectrum_ + arraysize(noise_spectrum_),
              kMinNoisePower);
}

void NoiseSpectrumEstimator::Update(rtc::ArrayView<const float> spectrum,
                                    bool first_update)
{
    RTC_DCHECK_EQ(65, spectrum.size());

    if (first_update)
    {
        // Initialize the noise spectral estimate with the signal spectrum.
        std::copy(spectrum.data(), spectrum.data() + spectrum.size(),
                  noise_spectrum_);
    }
    else
    {
        // Smoothly update the noise spectral estimate towards the signal spectrum
        // such that the magnitude of the updates are limited.
        for (size_t k = 0; k < spectrum.size(); ++k)
        {
            if (noise_spectrum_[k] < spectrum[k])
            {
                noise_spectrum_[k] = std::min(
                                         1.01f * noise_spectrum_[k],
                                         noise_spectrum_[k] + 0.05f * (spectrum[k] - noise_spectrum_[k]));
            }
            else
            {
                noise_spectrum_[k] = std::max(
                                         0.99f * noise_spectrum_[k],
                                         noise_spectrum_[k] + 0.05f * (spectrum[k] - noise_spectrum_[k]));
            }
        }
    }

    // Ensure that the noise spectal estimate does not become too low.
    for (auto& v : noise_spectrum_)
    {
        v = std::max(v, kMinNoisePower);
    }

    data_dumper_->DumpRaw("lc_noise_spectrum", 65, noise_spectrum_);
    data_dumper_->DumpRaw("lc_signal_spectrum", spectrum);
}

}  // namespace webrtc

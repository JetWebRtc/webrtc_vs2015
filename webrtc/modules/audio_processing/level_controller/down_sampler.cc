﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_processing/level_controller/down_sampler.h"

#include <string.h>
#include <algorithm>
#include <vector>

#include "webrtc/base/checks.h"
#include "webrtc/modules/audio_processing/include/audio_processing.h"
#include "webrtc/modules/audio_processing/level_controller/biquad_filter.h"
#include "webrtc/modules/audio_processing/logging/apm_data_dumper.h"

namespace webrtc
{
namespace
{

// Bandlimiter coefficients computed based on that only
// the first 40 bins of the spectrum for the downsampled
// signal are used.
// [B,A] = butter(2,(41/64*4000)/8000)
const BiQuadFilter::BiQuadCoefficients kLowPassFilterCoefficients_16kHz =
{
    {0.1455f, 0.2911f, 0.1455f},
    {-0.6698f, 0.2520f}
};

// [B,A] = butter(2,(41/64*4000)/16000)
const BiQuadFilter::BiQuadCoefficients kLowPassFilterCoefficients_32kHz =
{
    {0.0462f, 0.0924f, 0.0462f},
    {-1.3066f, 0.4915f}
};

// [B,A] = butter(2,(41/64*4000)/24000)
const BiQuadFilter::BiQuadCoefficients kLowPassFilterCoefficients_48kHz =
{
    {0.0226f, 0.0452f, 0.0226f},
    {-1.5320f, 0.6224f}
};

}  // namespace

DownSampler::DownSampler(ApmDataDumper* data_dumper)
    : data_dumper_(data_dumper)
{
    Initialize(48000);
}
void DownSampler::Initialize(int sample_rate_hz)
{
    RTC_DCHECK(sample_rate_hz == AudioProcessing::kSampleRate8kHz ||
               sample_rate_hz == AudioProcessing::kSampleRate16kHz ||
               sample_rate_hz == AudioProcessing::kSampleRate32kHz ||
               sample_rate_hz == AudioProcessing::kSampleRate48kHz);

    sample_rate_hz_ = sample_rate_hz;
    down_sampling_factor_ = rtc::CheckedDivExact(sample_rate_hz_, 8000);

    /// Note that the down sampling filter is not used if the sample rate is 8
    /// kHz.
    if (sample_rate_hz_ == AudioProcessing::kSampleRate16kHz)
    {
        low_pass_filter_.Initialize(kLowPassFilterCoefficients_16kHz);
    }
    else if (sample_rate_hz_ == AudioProcessing::kSampleRate32kHz)
    {
        low_pass_filter_.Initialize(kLowPassFilterCoefficients_32kHz);
    }
    else if (sample_rate_hz_ == AudioProcessing::kSampleRate48kHz)
    {
        low_pass_filter_.Initialize(kLowPassFilterCoefficients_48kHz);
    }
}

void DownSampler::DownSample(rtc::ArrayView<const float> in,
                             rtc::ArrayView<float> out)
{
    data_dumper_->DumpWav("lc_down_sampler_input", in, sample_rate_hz_, 1);
    RTC_DCHECK_EQ(sample_rate_hz_ * AudioProcessing::kChunkSizeMs / 1000,
                  in.size());
    RTC_DCHECK_EQ(
        AudioProcessing::kSampleRate8kHz * AudioProcessing::kChunkSizeMs / 1000,
        out.size());
    const size_t kMaxNumFrames =
        AudioProcessing::kSampleRate48kHz * AudioProcessing::kChunkSizeMs / 1000;
    float x[kMaxNumFrames];

    // Band-limit the signal to 4 kHz.
    if (sample_rate_hz_ != AudioProcessing::kSampleRate8kHz)
    {
        low_pass_filter_.Process(in, rtc::ArrayView<float>(x, in.size()));

        // Downsample the signal.
        size_t k = 0;
        for (size_t j = 0; j < out.size(); ++j)
        {
            RTC_DCHECK_GT(kMaxNumFrames, k);
            out[j] = x[k];
            k += down_sampling_factor_;
        }
    }
    else
    {
        std::copy(in.data(), in.data() + in.size(), out.data());
    }

    data_dumper_->DumpWav("lc_down_sampler_output", out,
                          AudioProcessing::kSampleRate8kHz, 1);
}

}  // namespace webrtc

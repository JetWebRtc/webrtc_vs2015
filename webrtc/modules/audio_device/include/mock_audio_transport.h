﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_DEVICE_INCLUDE_MOCK_AUDIO_TRANSPORT_H_
#define WEBRTC_MODULES_AUDIO_DEVICE_INCLUDE_MOCK_AUDIO_TRANSPORT_H_

#include "webrtc/modules/audio_device/include/audio_device_defines.h"
#include "webrtc/test/gmock.h"

namespace webrtc
{
namespace test
{

class MockAudioTransport : public AudioTransport
{
public:
    MockAudioTransport() {}
    ~MockAudioTransport() {}

    MOCK_METHOD10(RecordedDataIsAvailable,
                  int32_t(const void* audioSamples,
                          const size_t nSamples,
                          const size_t nBytesPerSample,
                          const size_t nChannels,
                          const uint32_t samplesPerSec,
                          const uint32_t totalDelayMS,
                          const int32_t clockDrift,
                          const uint32_t currentMicLevel,
                          const bool keyPressed,
                          uint32_t& newMicLevel));

    MOCK_METHOD8(NeedMorePlayData,
                 int32_t(const size_t nSamples,
                         const size_t nBytesPerSample,
                         const size_t nChannels,
                         const uint32_t samplesPerSec,
                         void* audioSamples,
                         size_t& nSamplesOut,
                         int64_t* elapsed_time_ms,
                         int64_t* ntp_time_ms));

    MOCK_METHOD6(PushCaptureData,
                 void(int voe_channel,
                      const void* audio_data,
                      int bits_per_sample,
                      int sample_rate,
                      size_t number_of_channels,
                      size_t number_of_frames));

    MOCK_METHOD7(PullRenderData,
                 void(int bits_per_sample,
                      int sample_rate,
                      size_t number_of_channels,
                      size_t number_of_frames,
                      void* audio_data,
                      int64_t* elapsed_time_ms,
                      int64_t* ntp_time_ms));
};

}  // namespace test
}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_DEVICE_INCLUDE_MOCK_AUDIO_TRANSPORT_H_

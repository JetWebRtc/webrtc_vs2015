﻿/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_API_CALL_AUDIO_SINK_H_
#define WEBRTC_API_CALL_AUDIO_SINK_H_

#if defined(WEBRTC_POSIX) && !defined(__STDC_FORMAT_MACROS)
// Avoid conflict with format_macros.h.
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include <stddef.h>

namespace webrtc
{

// Represents a simple push audio sink.
class AudioSinkInterface
{
public:
    virtual ~AudioSinkInterface() {}

    struct Data
    {
        Data(int16_t* data,
             size_t samples_per_channel,
             int sample_rate,
             size_t channels,
             uint32_t timestamp)
            : data(data),
              samples_per_channel(samples_per_channel),
              sample_rate(sample_rate),
              channels(channels),
              timestamp(timestamp) {}

        int16_t* data;               // The actual 16bit audio data.
        size_t samples_per_channel;  // Number of frames in the buffer.
        int sample_rate;             // Sample rate in Hz.
        size_t channels;             // Number of channels in the audio data.
        uint32_t timestamp;          // The RTP timestamp of the first sample.
    };

    virtual void OnData(const Data& audio) = 0;
};

}  // namespace webrtc

#endif  // WEBRTC_API_CALL_AUDIO_SINK_H_

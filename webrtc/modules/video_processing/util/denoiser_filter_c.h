﻿/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_VIDEO_PROCESSING_UTIL_DENOISER_FILTER_C_H_
#define WEBRTC_MODULES_VIDEO_PROCESSING_UTIL_DENOISER_FILTER_C_H_

#include "webrtc/modules/video_processing/util/denoiser_filter.h"

namespace webrtc
{

class DenoiserFilterC : public DenoiserFilter
{
public:
    DenoiserFilterC() {}
    void CopyMem16x16(const uint8_t* src,
                      int src_stride,
                      uint8_t* dst,
                      int dst_stride) override;
    uint32_t Variance16x8(const uint8_t* a,
                          int a_stride,
                          const uint8_t* b,
                          int b_stride,
                          unsigned int* sse) override;
    DenoiserDecision MbDenoise(const uint8_t* mc_running_avg_y,
                               int mc_avg_y_stride,
                               uint8_t* running_avg_y,
                               int avg_y_stride,
                               const uint8_t* sig,
                               int sig_stride,
                               uint8_t motion_magnitude,
                               int increase_denoising) override;
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_VIDEO_PROCESSING_UTIL_DENOISER_FILTER_C_H_

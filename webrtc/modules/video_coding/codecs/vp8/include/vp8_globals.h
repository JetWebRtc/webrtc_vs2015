﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains codec dependent definitions that are needed in
// order to compile the WebRTC codebase, even if this codec is not used.

#ifndef WEBRTC_MODULES_VIDEO_CODING_CODECS_VP8_INCLUDE_VP8_GLOBALS_H_
#define WEBRTC_MODULES_VIDEO_CODING_CODECS_VP8_INCLUDE_VP8_GLOBALS_H_

#include "webrtc/modules/video_coding/codecs/interface/common_constants.h"

namespace webrtc
{

struct RTPVideoHeaderVP8
{
    void InitRTPVideoHeaderVP8()
    {
        nonReference = false;
        pictureId = kNoPictureId;
        tl0PicIdx = kNoTl0PicIdx;
        temporalIdx = kNoTemporalIdx;
        layerSync = false;
        keyIdx = kNoKeyIdx;
        partitionId = 0;
        beginningOfPartition = false;
    }

    bool nonReference;          // Frame is discardable.
    int16_t pictureId;          // Picture ID index, 15 bits;
    // kNoPictureId if PictureID does not exist.
    int16_t tl0PicIdx;          // TL0PIC_IDX, 8 bits;
    // kNoTl0PicIdx means no value provided.
    uint8_t temporalIdx;        // Temporal layer index, or kNoTemporalIdx.
    bool layerSync;             // This frame is a layer sync frame.
    // Disabled if temporalIdx == kNoTemporalIdx.
    int keyIdx;                 // 5 bits; kNoKeyIdx means not used.
    int partitionId;            // VP8 partition ID
    bool beginningOfPartition;  // True if this packet is the first
    // in a VP8 partition. Otherwise false
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_VIDEO_CODING_CODECS_VP8_INCLUDE_VP8_GLOBALS_H_

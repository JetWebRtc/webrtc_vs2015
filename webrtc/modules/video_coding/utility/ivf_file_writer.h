﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_VIDEO_CODING_UTILITY_IVF_FILE_WRITER_H_
#define WEBRTC_MODULES_VIDEO_CODING_UTILITY_IVF_FILE_WRITER_H_

#include <memory>
#include <string>

#include "webrtc/base/constructormagic.h"
#include "webrtc/base/file.h"
#include "webrtc/base/timeutils.h"
#include "webrtc/modules/include/module_common_types.h"
#include "webrtc/video_frame.h"

namespace webrtc
{

class IvfFileWriter
{
public:
    // Takes ownership of the file, which will be closed either through
    // Close or ~IvfFileWriter. If writing a frame would take the file above the
    // |byte_limit| the file will be closed, the write (and all future writes)
    // will fail. A |byte_limit| of 0 is equivalent to no limit.
    static std::unique_ptr<IvfFileWriter> Wrap(rtc::File file, size_t byte_limit);
    ~IvfFileWriter();

    bool WriteFrame(const EncodedImage& encoded_image, VideoCodecType codec_type);
    bool Close();

private:
    explicit IvfFileWriter(rtc::File file, size_t byte_limit);

    bool WriteHeader();
    bool InitFromFirstFrame(const EncodedImage& encoded_image,
                            VideoCodecType codec_type);

    VideoCodecType codec_type_;
    size_t bytes_written_;
    size_t byte_limit_;
    size_t num_frames_;
    uint16_t width_;
    uint16_t height_;
    int64_t last_timestamp_;
    bool using_capture_timestamps_;
    rtc::TimestampWrapAroundHandler wrap_handler_;
    rtc::File file_;

    RTC_DISALLOW_COPY_AND_ASSIGN(IvfFileWriter);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_VIDEO_CODING_UTILITY_IVF_FILE_WRITER_H_

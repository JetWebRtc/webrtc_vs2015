﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_RAPID_RESYNC_REQUEST_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_RAPID_RESYNC_REQUEST_H_

#include "webrtc/base/basictypes.h"
#include "webrtc/modules/rtp_rtcp/source/rtcp_packet/rtpfb.h"

namespace webrtc
{
namespace rtcp
{
class CommonHeader;

// draft-perkins-avt-rapid-rtp-sync-03
class RapidResyncRequest : public Rtpfb
{
public:
    static constexpr uint8_t kFeedbackMessageType = 5;

    RapidResyncRequest() {}
    ~RapidResyncRequest() override {}

    // Parse assumes header is already parsed and validated.
    bool Parse(const CommonHeader& header);

protected:
    bool Create(uint8_t* packet,
                size_t* index,
                size_t max_length,
                RtcpPacket::PacketReadyCallback* callback) const override;

private:
    size_t BlockLength() const override
    {
        return kHeaderLength + kCommonFeedbackLength;
    }
};
}  // namespace rtcp
}  // namespace webrtc
#endif  // WEBRTC_MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_RAPID_RESYNC_REQUEST_H_

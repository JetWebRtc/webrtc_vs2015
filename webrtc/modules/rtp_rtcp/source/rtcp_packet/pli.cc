﻿/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/rtp_rtcp/source/rtcp_packet/pli.h"

#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"
#include "webrtc/modules/rtp_rtcp/source/rtcp_packet/common_header.h"

namespace webrtc
{
namespace rtcp
{
constexpr uint8_t Pli::kFeedbackMessageType;
// RFC 4585: Feedback format.
//
// Common packet format:
//
//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |V=2|P|   FMT   |       PT      |          length               |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                  SSRC of packet sender                        |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                  SSRC of media source                         |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  :            Feedback Control Information (FCI)                 :
//  :                                                               :

//
// Picture loss indication (PLI) (RFC 4585).
// FCI: no feedback control information.
bool Pli::Parse(const CommonHeader& packet)
{
    RTC_DCHECK_EQ(packet.type(), kPacketType);
    RTC_DCHECK_EQ(packet.fmt(), kFeedbackMessageType);

    if (packet.payload_size_bytes() < kCommonFeedbackLength)
    {
        LOG(LS_WARNING) << "Packet is too small to be a valid PLI packet";
        return false;
    }

    ParseCommonFeedback(packet.payload());
    return true;
}

bool Pli::Create(uint8_t* packet,
                 size_t* index,
                 size_t max_length,
                 RtcpPacket::PacketReadyCallback* callback) const
{
    while (*index + BlockLength() > max_length)
    {
        if (!OnBufferFull(packet, index, callback))
            return false;
    }

    CreateHeader(kFeedbackMessageType, kPacketType, HeaderLength(), packet,
                 index);
    CreateCommonFeedback(packet + *index);
    *index += kCommonFeedbackLength;
    return true;
}

}  // namespace rtcp
}  // namespace webrtc

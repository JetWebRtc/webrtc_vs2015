﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/rtp_rtcp/source/rtcp_packet/tmmbn.h"

#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"
#include "webrtc/modules/rtp_rtcp/source/byte_io.h"
#include "webrtc/modules/rtp_rtcp/source/rtcp_packet/common_header.h"

namespace webrtc
{
namespace rtcp
{
constexpr uint8_t Tmmbn::kFeedbackMessageType;
// RFC 4585: Feedback format.
// Common packet format:
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |V=2|P|   FMT   |       PT      |          length               |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                  SSRC of packet sender                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |             SSRC of media source (unused) = 0                 |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   :            Feedback Control Information (FCI)                 :
//   :                                                               :
// Temporary Maximum Media Stream Bit Rate Notification (TMMBN) (RFC 5104).
// The Feedback Control Information (FCI) consists of zero, one, or more
// TMMBN FCI entries.
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                              SSRC                             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   | MxTBR Exp |  MxTBR Mantissa                 |Measured Overhead|
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool Tmmbn::Parse(const CommonHeader& packet)
{
    RTC_DCHECK_EQ(packet.type(), kPacketType);
    RTC_DCHECK_EQ(packet.fmt(), kFeedbackMessageType);

    if (packet.payload_size_bytes() < kCommonFeedbackLength)
    {
        LOG(LS_WARNING) << "Payload length " << packet.payload_size_bytes()
                        << " is too small for TMMBN.";
        return false;
    }
    size_t items_size_bytes = packet.payload_size_bytes() - kCommonFeedbackLength;
    if (items_size_bytes % TmmbItem::kLength != 0)
    {
        LOG(LS_WARNING) << "Payload length " << packet.payload_size_bytes()
                        << " is not valid for TMMBN.";
        return false;
    }
    ParseCommonFeedback(packet.payload());
    const uint8_t* next_item = packet.payload() + kCommonFeedbackLength;

    size_t number_of_items = items_size_bytes / TmmbItem::kLength;
    items_.resize(number_of_items);
    for (TmmbItem& item : items_)
    {
        if (!item.Parse(next_item))
            return false;
        next_item += TmmbItem::kLength;
    }
    return true;
}

void Tmmbn::AddTmmbr(const TmmbItem& item)
{
    items_.push_back(item);
}

bool Tmmbn::Create(uint8_t* packet,
                   size_t* index,
                   size_t max_length,
                   RtcpPacket::PacketReadyCallback* callback) const
{
    while (*index + BlockLength() > max_length)
    {
        if (!OnBufferFull(packet, index, callback))
            return false;
    }
    const size_t index_end = *index + BlockLength();

    CreateHeader(kFeedbackMessageType, kPacketType, HeaderLength(), packet,
                 index);
    RTC_DCHECK_EQ(0, Rtpfb::media_ssrc());
    CreateCommonFeedback(packet + *index);
    *index += kCommonFeedbackLength;
    for (const TmmbItem& item : items_)
    {
        item.Create(packet + *index);
        *index += TmmbItem::kLength;
    }
    RTC_CHECK_EQ(index_end, *index);
    return true;
}
}  // namespace rtcp
}  // namespace webrtc

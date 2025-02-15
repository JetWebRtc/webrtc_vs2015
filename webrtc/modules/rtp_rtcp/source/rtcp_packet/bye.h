﻿/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_BYE_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_BYE_H_

#include <string>
#include <vector>

#include "webrtc/modules/rtp_rtcp/source/rtcp_packet.h"

namespace webrtc
{
namespace rtcp
{
class CommonHeader;

class Bye : public RtcpPacket
{
public:
    static constexpr uint8_t kPacketType = 203;

    Bye();
    ~Bye() override {}

    // Parse assumes header is already parsed and validated.
    bool Parse(const CommonHeader& packet);

    void SetSenderSsrc(uint32_t ssrc)
    {
        sender_ssrc_ = ssrc;
    }
    bool SetCsrcs(std::vector<uint32_t> csrcs);
    void SetReason(std::string reason);

    uint32_t sender_ssrc() const
    {
        return sender_ssrc_;
    }
    const std::vector<uint32_t>& csrcs() const
    {
        return csrcs_;
    }
    const std::string& reason() const
    {
        return reason_;
    }

protected:
    bool Create(uint8_t* packet,
                size_t* index,
                size_t max_length,
                RtcpPacket::PacketReadyCallback* callback) const override;

private:
    static const int kMaxNumberOfCsrcs = 0x1f - 1;  // First item is sender SSRC.

    size_t BlockLength() const override;

    uint32_t sender_ssrc_;
    std::vector<uint32_t> csrcs_;
    std::string reason_;
};

}  // namespace rtcp
}  // namespace webrtc
#endif  // WEBRTC_MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_BYE_H_

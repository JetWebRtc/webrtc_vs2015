﻿/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/rtp_rtcp/source/rtcp_packet/nack.h"

#include "webrtc/test/gmock.h"
#include "webrtc/test/gtest.h"
#include "webrtc/test/rtcp_packet_parser.h"

namespace webrtc
{
namespace
{

using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Invoke;
using ::testing::make_tuple;
using ::testing::UnorderedElementsAreArray;
using ::webrtc::rtcp::Nack;

constexpr uint32_t kSenderSsrc = 0x12345678;
constexpr uint32_t kRemoteSsrc = 0x23456789;

constexpr uint16_t kList[] = {0, 1, 3, 8, 16};
constexpr size_t kListLength = sizeof(kList) / sizeof(kList[0]);
constexpr uint8_t kVersionBits = 2 << 6;
// clang-format off
constexpr uint8_t kPacket[] =
{
    kVersionBits | Nack::kFeedbackMessageType, Nack::kPacketType, 0, 3,
    0x12, 0x34, 0x56, 0x78,
    0x23, 0x45, 0x67, 0x89,
    0x00, 0x00, 0x80, 0x85
};

constexpr uint16_t kWrapList[] = {0xffdc, 0xffec, 0xfffe, 0xffff, 0x0000,
                                  0x0001, 0x0003, 0x0014, 0x0064
                                 };
constexpr size_t kWrapListLength = sizeof(kWrapList) / sizeof(kWrapList[0]);
constexpr uint8_t kWrapPacket[] =
{
    kVersionBits | Nack::kFeedbackMessageType, Nack::kPacketType, 0, 6,
    0x12, 0x34, 0x56, 0x78,
    0x23, 0x45, 0x67, 0x89,
    0xff, 0xdc, 0x80, 0x00,
    0xff, 0xfe, 0x00, 0x17,
    0x00, 0x14, 0x00, 0x00,
    0x00, 0x64, 0x00, 0x00
};
constexpr uint8_t kTooSmallPacket[] =
{
    kVersionBits | Nack::kFeedbackMessageType, Nack::kPacketType, 0, 2,
    0x12, 0x34, 0x56, 0x78,
    0x23, 0x45, 0x67, 0x89
};
// clang-format on
}  // namespace

TEST(RtcpPacketNackTest, Create)
{
    Nack nack;
    nack.SetSenderSsrc(kSenderSsrc);
    nack.SetMediaSsrc(kRemoteSsrc);
    nack.SetPacketIds(kList, kListLength);

    rtc::Buffer packet = nack.Build();

    EXPECT_THAT(make_tuple(packet.data(), packet.size()),
                ElementsAreArray(kPacket));
}

TEST(RtcpPacketNackTest, Parse)
{
    Nack parsed;
    EXPECT_TRUE(test::ParseSinglePacket(kPacket, &parsed));
    const Nack& const_parsed = parsed;

    EXPECT_EQ(kSenderSsrc, const_parsed.sender_ssrc());
    EXPECT_EQ(kRemoteSsrc, const_parsed.media_ssrc());
    EXPECT_THAT(const_parsed.packet_ids(), ElementsAreArray(kList));
}

TEST(RtcpPacketNackTest, CreateWrap)
{
    Nack nack;
    nack.SetSenderSsrc(kSenderSsrc);
    nack.SetMediaSsrc(kRemoteSsrc);
    nack.SetPacketIds(kWrapList, kWrapListLength);

    rtc::Buffer packet = nack.Build();

    EXPECT_THAT(make_tuple(packet.data(), packet.size()),
                ElementsAreArray(kWrapPacket));
}

TEST(RtcpPacketNackTest, ParseWrap)
{
    Nack parsed;
    EXPECT_TRUE(test::ParseSinglePacket(kWrapPacket, &parsed));

    EXPECT_EQ(kSenderSsrc, parsed.sender_ssrc());
    EXPECT_EQ(kRemoteSsrc, parsed.media_ssrc());
    EXPECT_THAT(parsed.packet_ids(), ElementsAreArray(kWrapList));
}

TEST(RtcpPacketNackTest, BadOrder)
{
    // Does not guarantee optimal packing, but should guarantee correctness.
    const uint16_t kUnorderedList[] = {1, 25, 13, 12, 9, 27, 29};
    const size_t kUnorderedListLength =
        sizeof(kUnorderedList) / sizeof(kUnorderedList[0]);
    Nack nack;
    nack.SetSenderSsrc(kSenderSsrc);
    nack.SetMediaSsrc(kRemoteSsrc);
    nack.SetPacketIds(kUnorderedList, kUnorderedListLength);

    rtc::Buffer packet = nack.Build();

    Nack parsed;
    EXPECT_TRUE(test::ParseSinglePacket(packet, &parsed));

    EXPECT_EQ(kSenderSsrc, parsed.sender_ssrc());
    EXPECT_EQ(kRemoteSsrc, parsed.media_ssrc());
    EXPECT_THAT(parsed.packet_ids(), UnorderedElementsAreArray(kUnorderedList));
}

TEST(RtcpPacketNackTest, CreateFragmented)
{
    Nack nack;
    const uint16_t kList[] = {1, 100, 200, 300, 400};
    const uint16_t kListLength = sizeof(kList) / sizeof(kList[0]);
    nack.SetSenderSsrc(kSenderSsrc);
    nack.SetMediaSsrc(kRemoteSsrc);
    nack.SetPacketIds(kList, kListLength);

    class MockPacketReadyCallback : public rtcp::RtcpPacket::PacketReadyCallback
    {
    public:
        MOCK_METHOD2(OnPacketReady, void(uint8_t*, size_t));
    } verifier;

    class NackVerifier
    {
    public:
        explicit NackVerifier(std::vector<uint16_t> ids) : ids_(ids) {}
        void operator()(uint8_t* data, size_t length)
        {
            Nack nack;
            EXPECT_TRUE(test::ParseSinglePacket(data, length, &nack));
            EXPECT_EQ(kSenderSsrc, nack.sender_ssrc());
            EXPECT_EQ(kRemoteSsrc, nack.media_ssrc());
            EXPECT_THAT(nack.packet_ids(), ElementsAreArray(ids_));
        }
        std::vector<uint16_t> ids_;
    } packet1({1, 100, 200}), packet2({300, 400});

    EXPECT_CALL(verifier, OnPacketReady(_, _))
    .WillOnce(Invoke(packet1))
    .WillOnce(Invoke(packet2));
    const size_t kBufferSize = 12 + (3 * 4);  // Fits common header + 3 nack items
    uint8_t buffer[kBufferSize];
    EXPECT_TRUE(nack.BuildExternalBuffer(buffer, kBufferSize, &verifier));
}

TEST(RtcpPacketNackTest, CreateFailsWithTooSmallBuffer)
{
    const uint16_t kList[] = {1};
    const size_t kMinNackBlockSize = 16;
    Nack nack;
    nack.SetSenderSsrc(kSenderSsrc);
    nack.SetMediaSsrc(kRemoteSsrc);
    nack.SetPacketIds(kList, 1);
    class Verifier : public rtcp::RtcpPacket::PacketReadyCallback
    {
    public:
        void OnPacketReady(uint8_t* data, size_t length) override
        {
            ADD_FAILURE() << "Buffer should be too small.";
        }
    } verifier;
    uint8_t buffer[kMinNackBlockSize - 1];
    EXPECT_FALSE(
        nack.BuildExternalBuffer(buffer, kMinNackBlockSize - 1, &verifier));
}

TEST(RtcpPacketNackTest, ParseFailsWithTooSmallBuffer)
{
    Nack parsed;
    EXPECT_FALSE(test::ParseSinglePacket(kTooSmallPacket, &parsed));
}

}  // namespace webrtc

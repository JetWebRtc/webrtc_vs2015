﻿/*
 *  Copyright 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/base/bytebuffer.h"
#include "webrtc/base/copyonwritebuffer.h"
#include "webrtc/base/gunit.h"
#include "webrtc/pc/sctputils.h"

class SctpUtilsTest : public testing::Test
{
public:
    void VerifyOpenMessageFormat(const rtc::CopyOnWriteBuffer& packet,
                                 const std::string& label,
                                 const webrtc::DataChannelInit& config)
    {
        uint8_t message_type;
        uint8_t channel_type;
        uint32_t reliability;
        uint16_t priority;
        uint16_t label_length;
        uint16_t protocol_length;

        rtc::ByteBufferReader buffer(packet.data<char>(), packet.size());
        ASSERT_TRUE(buffer.ReadUInt8(&message_type));
        EXPECT_EQ(0x03, message_type);

        ASSERT_TRUE(buffer.ReadUInt8(&channel_type));
        if (config.ordered)
        {
            EXPECT_EQ(config.maxRetransmits > -1 ?
                      0x01 : (config.maxRetransmitTime > -1 ? 0x02 : 0),
                      channel_type);
        }
        else
        {
            EXPECT_EQ(config.maxRetransmits > -1 ?
                      0x81 : (config.maxRetransmitTime > -1 ? 0x82 : 0x80),
                      channel_type);
        }

        ASSERT_TRUE(buffer.ReadUInt16(&priority));

        ASSERT_TRUE(buffer.ReadUInt32(&reliability));
        if (config.maxRetransmits > -1 || config.maxRetransmitTime > -1)
        {
            EXPECT_EQ(config.maxRetransmits > -1 ?
                      config.maxRetransmits : config.maxRetransmitTime,
                      static_cast<int>(reliability));
        }

        ASSERT_TRUE(buffer.ReadUInt16(&label_length));
        ASSERT_TRUE(buffer.ReadUInt16(&protocol_length));
        EXPECT_EQ(label.size(), label_length);
        EXPECT_EQ(config.protocol.size(), protocol_length);

        std::string label_output;
        ASSERT_TRUE(buffer.ReadString(&label_output, label_length));
        EXPECT_EQ(label, label_output);
        std::string protocol_output;
        ASSERT_TRUE(buffer.ReadString(&protocol_output, protocol_length));
        EXPECT_EQ(config.protocol, protocol_output);
    }
};

TEST_F(SctpUtilsTest, WriteParseOpenMessageWithOrderedReliable)
{
    webrtc::DataChannelInit config;
    std::string label = "abc";
    config.protocol = "y";

    rtc::CopyOnWriteBuffer packet;
    ASSERT_TRUE(webrtc::WriteDataChannelOpenMessage(label, config, &packet));

    VerifyOpenMessageFormat(packet, label, config);

    std::string output_label;
    webrtc::DataChannelInit output_config;
    ASSERT_TRUE(webrtc::ParseDataChannelOpenMessage(
                    packet, &output_label, &output_config));

    EXPECT_EQ(label, output_label);
    EXPECT_EQ(config.protocol, output_config.protocol);
    EXPECT_EQ(config.ordered, output_config.ordered);
    EXPECT_EQ(config.maxRetransmitTime, output_config.maxRetransmitTime);
    EXPECT_EQ(config.maxRetransmits, output_config.maxRetransmits);
}

TEST_F(SctpUtilsTest, WriteParseOpenMessageWithMaxRetransmitTime)
{
    webrtc::DataChannelInit config;
    std::string label = "abc";
    config.ordered = false;
    config.maxRetransmitTime = 10;
    config.protocol = "y";

    rtc::CopyOnWriteBuffer packet;
    ASSERT_TRUE(webrtc::WriteDataChannelOpenMessage(label, config, &packet));

    VerifyOpenMessageFormat(packet, label, config);

    std::string output_label;
    webrtc::DataChannelInit output_config;
    ASSERT_TRUE(webrtc::ParseDataChannelOpenMessage(
                    packet, &output_label, &output_config));

    EXPECT_EQ(label, output_label);
    EXPECT_EQ(config.protocol, output_config.protocol);
    EXPECT_EQ(config.ordered, output_config.ordered);
    EXPECT_EQ(config.maxRetransmitTime, output_config.maxRetransmitTime);
    EXPECT_EQ(-1, output_config.maxRetransmits);
}

TEST_F(SctpUtilsTest, WriteParseOpenMessageWithMaxRetransmits)
{
    webrtc::DataChannelInit config;
    std::string label = "abc";
    config.maxRetransmits = 10;
    config.protocol = "y";

    rtc::CopyOnWriteBuffer packet;
    ASSERT_TRUE(webrtc::WriteDataChannelOpenMessage(label, config, &packet));

    VerifyOpenMessageFormat(packet, label, config);

    std::string output_label;
    webrtc::DataChannelInit output_config;
    ASSERT_TRUE(webrtc::ParseDataChannelOpenMessage(
                    packet, &output_label, &output_config));

    EXPECT_EQ(label, output_label);
    EXPECT_EQ(config.protocol, output_config.protocol);
    EXPECT_EQ(config.ordered, output_config.ordered);
    EXPECT_EQ(config.maxRetransmits, output_config.maxRetransmits);
    EXPECT_EQ(-1, output_config.maxRetransmitTime);
}

TEST_F(SctpUtilsTest, WriteParseAckMessage)
{
    rtc::CopyOnWriteBuffer packet;
    webrtc::WriteDataChannelOpenAckMessage(&packet);

    uint8_t message_type;
    rtc::ByteBufferReader buffer(packet.data<char>(), packet.size());
    ASSERT_TRUE(buffer.ReadUInt8(&message_type));
    EXPECT_EQ(0x02, message_type);

    EXPECT_TRUE(webrtc::ParseDataChannelOpenAckMessage(packet));
}

TEST_F(SctpUtilsTest, TestIsOpenMessage)
{
    rtc::CopyOnWriteBuffer open(1);
    open[0] = 0x03;
    EXPECT_TRUE(webrtc::IsOpenMessage(open));

    rtc::CopyOnWriteBuffer openAck(1);
    openAck[0] = 0x02;
    EXPECT_FALSE(webrtc::IsOpenMessage(openAck));

    rtc::CopyOnWriteBuffer invalid(1);
    invalid[0] = 0x01;
    EXPECT_FALSE(webrtc::IsOpenMessage(invalid));

    rtc::CopyOnWriteBuffer empty;
    EXPECT_FALSE(webrtc::IsOpenMessage(empty));
}

﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_CHANNEL_CONTROLLER_H_
#define WEBRTC_MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_CHANNEL_CONTROLLER_H_

#include "webrtc/base/constructormagic.h"
#include "webrtc/modules/audio_coding/audio_network_adaptor/controller.h"

namespace webrtc
{

class ChannelController final : public Controller
{
public:
    struct Config
    {
        Config(size_t num_encoder_channels,
               size_t intial_channels_to_encode,
               int channel_1_to_2_bandwidth_bps,
               int channel_2_to_1_bandwidth_bps);
        size_t num_encoder_channels;
        size_t intial_channels_to_encode;
        // Uplink bandwidth above which the number of encoded channels should switch
        // from 1 to 2.
        int channel_1_to_2_bandwidth_bps;
        // Uplink bandwidth below which the number of encoded channels should switch
        // from 2 to 1.
        int channel_2_to_1_bandwidth_bps;
    };

    explicit ChannelController(const Config& config);

    ~ChannelController() override;

    void UpdateNetworkMetrics(const NetworkMetrics& network_metrics) override;

    void MakeDecision(AudioNetworkAdaptor::EncoderRuntimeConfig* config) override;

private:
    const Config config_;
    size_t channels_to_encode_;
    rtc::Optional<int> uplink_bandwidth_bps_;
    RTC_DISALLOW_COPY_AND_ASSIGN(ChannelController);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_CHANNEL_CONTROLLER_H_

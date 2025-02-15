﻿/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/video_coding/codecs/test/videoprocessor_integrationtest.h"

namespace webrtc
{
namespace test
{

#if defined(WEBRTC_VIDEOPROCESSOR_H264_TESTS)

// H264: Run with no packet loss and fixed bitrate. Quality should be very high.
// Note(hbos): The PacketManipulatorImpl code used to simulate packet loss in
// these unittests appears to drop "packets" in a way that is not compatible
// with H264. Therefore ProcessXPercentPacketLossH264, X != 0, unittests have
// not been added.
TEST_F(VideoProcessorIntegrationTest, Process0PercentPacketLossH264)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecH264, 0.0f, -1, 1, false,
                       false, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 35.0, 25.0, 0.93, 0.70);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 2, 60, 20, 10, 20, 0, 1);
    ProcessFramesAndVerify(quality_metrics,
                           rate_profile,
                           process_settings,
                           rc_metrics);
}

#endif  // defined(WEBRTC_VIDEOPROCESSOR_H264_TESTS)

// Fails on iOS. See webrtc:4755.
#if !defined(WEBRTC_IOS)

#if !defined(RTC_DISABLE_VP9)
// VP9: Run with no packet loss and fixed bitrate. Quality should be very high.
// One key frame (first frame only) in sequence. Setting |key_frame_interval|
// to -1 below means no periodic key frames in test.
TEST_F(VideoProcessorIntegrationTest, Process0PercentPacketLossVP9)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP9, 0.0f, -1, 1, false,
                       false, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 37.0, 36.0, 0.93, 0.92);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 0, 40, 20, 10, 20, 0, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP9: Run with 5% packet loss and fixed bitrate. Quality should be a bit
// lower. One key frame (first frame only) in sequence.
TEST_F(VideoProcessorIntegrationTest, Process5PercentPacketLossVP9)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP9, 0.05f, -1, 1, false,
                       false, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 17.0, 14.0, 0.45, 0.36);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 0, 40, 20, 10, 20, 0, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP9: Run with no packet loss, with varying bitrate (3 rate updates):
// low to high to medium. Check that quality and encoder response to the new
// target rate/per-frame bandwidth (for each rate update) is within limits.
// One key frame (first frame only) in sequence.
TEST_F(VideoProcessorIntegrationTest, ProcessNoLossChangeBitRateVP9)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 200, 30, 0);
    SetRateProfilePars(&rate_profile, 1, 700, 30, 100);
    SetRateProfilePars(&rate_profile, 2, 500, 30, 200);
    rate_profile.frame_index_rate_update[3] = kNumFramesLong + 1;
    rate_profile.num_frames = kNumFramesLong;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP9, 0.0f, -1, 1, false,
                       false, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 35.5, 30.0, 0.90, 0.85);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[3];
    SetRateControlMetrics(rc_metrics, 0, 0, 30, 20, 20, 30, 0, 1);
    SetRateControlMetrics(rc_metrics, 1, 2, 0, 20, 20, 60, 0, 0);
    SetRateControlMetrics(rc_metrics, 2, 0, 0, 25, 20, 40, 0, 0);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP9: Run with no packet loss, with an update (decrease) in frame rate.
// Lower frame rate means higher per-frame-bandwidth, so easier to encode.
// At the low bitrate in this test, this means better rate control after the
// update(s) to lower frame rate. So expect less frame drops, and max values
// for the rate control metrics can be lower. One key frame (first frame only).
// Note: quality after update should be higher but we currently compute quality
// metrics averaged over whole sequence run.
TEST_F(VideoProcessorIntegrationTest,
       ProcessNoLossChangeFrameRateFrameDropVP9)
{
    config_.networking_config.packet_loss_probability = 0;
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 100, 24, 0);
    SetRateProfilePars(&rate_profile, 1, 100, 15, 100);
    SetRateProfilePars(&rate_profile, 2, 100, 10, 200);
    rate_profile.frame_index_rate_update[3] = kNumFramesLong + 1;
    rate_profile.num_frames = kNumFramesLong;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP9, 0.0f, -1, 1, false,
                       false, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 31.5, 18.0, 0.80, 0.43);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[3];
    SetRateControlMetrics(rc_metrics, 0, 38, 50, 75, 15, 45, 0, 1);
    SetRateControlMetrics(rc_metrics, 1, 10, 0, 40, 10, 30, 0, 0);
    SetRateControlMetrics(rc_metrics, 2, 5, 0, 30, 5, 20, 0, 0);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP9: Run with no packet loss and denoiser on. One key frame (first frame).
TEST_F(VideoProcessorIntegrationTest, ProcessNoLossDenoiserOnVP9)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP9, 0.0f, -1, 1, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 36.8, 35.8, 0.92, 0.91);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 0, 40, 20, 10, 20, 0, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// Run with no packet loss, at low bitrate.
// spatial_resize is on, for this low bitrate expect one resize in sequence.
// Resize happens on delta frame. Expect only one key frame (first frame).
TEST_F(VideoProcessorIntegrationTest,
       DISABLED_ProcessNoLossSpatialResizeFrameDropVP9)
{
    config_.networking_config.packet_loss_probability = 0;
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 50, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesLong + 1;
    rate_profile.num_frames = kNumFramesLong;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP9, 0.0f, -1, 1, false,
                       false, true, true);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 24.0, 13.0, 0.65, 0.37);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 228, 70, 160, 15, 80, 1, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// TODO(marpan): Add temporal layer test for VP9, once changes are in
// vp9 wrapper for this.

#endif  // !defined(RTC_DISABLE_VP9)

// VP8: Run with no packet loss and fixed bitrate. Quality should be very high.
// One key frame (first frame only) in sequence. Setting |key_frame_interval|
// to -1 below means no periodic key frames in test.
TEST_F(VideoProcessorIntegrationTest, ProcessZeroPacketLoss)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP8, 0.0f, -1, 1, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 34.95, 33.0, 0.90, 0.89);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 0, 40, 20, 10, 15, 0, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP8: Run with 5% packet loss and fixed bitrate. Quality should be a bit
// lower. One key frame (first frame only) in sequence.
TEST_F(VideoProcessorIntegrationTest, Process5PercentPacketLoss)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP8, 0.05f, -1, 1, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 20.0, 16.0, 0.60, 0.40);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 0, 40, 20, 10, 15, 0, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP8: Run with 10% packet loss and fixed bitrate. Quality should be lower.
// One key frame (first frame only) in sequence.
TEST_F(VideoProcessorIntegrationTest, Process10PercentPacketLoss)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 500, 30, 0);
    rate_profile.frame_index_rate_update[1] = kNumFramesShort + 1;
    rate_profile.num_frames = kNumFramesShort;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP8, 0.1f, -1, 1, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 19.0, 16.0, 0.50, 0.35);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[1];
    SetRateControlMetrics(rc_metrics, 0, 0, 40, 20, 10, 15, 0, 1);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

#endif  // !defined(WEBRTC_IOS)

// The tests below are currently disabled for Android. For ARM, the encoder
// uses |cpu_speed| = 12, as opposed to default |cpu_speed| <= 6 for x86,
// which leads to significantly different quality. The quality and rate control
// settings in the tests below are defined for encoder speed setting
// |cpu_speed| <= ~6. A number of settings would need to be significantly
// modified for the |cpu_speed| = 12 case. For now, keep the tests below
// disabled on Android. Some quality parameter in the above test has been
// adjusted to also pass for |cpu_speed| <= 12.

// VP8: Run with no packet loss, with varying bitrate (3 rate updates):
// low to high to medium. Check that quality and encoder response to the new
// target rate/per-frame bandwidth (for each rate update) is within limits.
// One key frame (first frame only) in sequence.
// Too slow to finish before timeout on iOS. See webrtc:4755.
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
#define MAYBE_ProcessNoLossChangeBitRateVP8 \
  DISABLED_ProcessNoLossChangeBitRateVP8
#else
#define MAYBE_ProcessNoLossChangeBitRateVP8 ProcessNoLossChangeBitRateVP8
#endif
TEST_F(VideoProcessorIntegrationTest, MAYBE_ProcessNoLossChangeBitRateVP8)
{
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 200, 30, 0);
    SetRateProfilePars(&rate_profile, 1, 800, 30, 100);
    SetRateProfilePars(&rate_profile, 2, 500, 30, 200);
    rate_profile.frame_index_rate_update[3] = kNumFramesLong + 1;
    rate_profile.num_frames = kNumFramesLong;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP8, 0.0f, -1, 1, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 34.0, 32.0, 0.85, 0.80);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[3];
    SetRateControlMetrics(rc_metrics, 0, 0, 45, 20, 10, 15, 0, 1);
    SetRateControlMetrics(rc_metrics, 1, 0, 0, 25, 20, 10, 0, 0);
    SetRateControlMetrics(rc_metrics, 2, 0, 0, 25, 15, 10, 0, 0);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP8: Run with no packet loss, with an update (decrease) in frame rate.
// Lower frame rate means higher per-frame-bandwidth, so easier to encode.
// At the bitrate in this test, this means better rate control after the
// update(s) to lower frame rate. So expect less frame drops, and max values
// for the rate control metrics can be lower. One key frame (first frame only).
// Note: quality after update should be higher but we currently compute quality
// metrics averaged over whole sequence run.
// Too slow to finish before timeout on iOS. See webrtc:4755.
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
#define MAYBE_ProcessNoLossChangeFrameRateFrameDropVP8 \
  DISABLED_ProcessNoLossChangeFrameRateFrameDropVP8
#else
#define MAYBE_ProcessNoLossChangeFrameRateFrameDropVP8 \
  ProcessNoLossChangeFrameRateFrameDropVP8
#endif
TEST_F(VideoProcessorIntegrationTest,
       MAYBE_ProcessNoLossChangeFrameRateFrameDropVP8)
{
    config_.networking_config.packet_loss_probability = 0;
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 80, 24, 0);
    SetRateProfilePars(&rate_profile, 1, 80, 15, 100);
    SetRateProfilePars(&rate_profile, 2, 80, 10, 200);
    rate_profile.frame_index_rate_update[3] = kNumFramesLong + 1;
    rate_profile.num_frames = kNumFramesLong;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP8, 0.0f, -1, 1, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 31.0, 22.0, 0.80, 0.65);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[3];
    SetRateControlMetrics(rc_metrics, 0, 40, 20, 75, 15, 60, 0, 1);
    SetRateControlMetrics(rc_metrics, 1, 10, 0, 25, 10, 35, 0, 0);
    SetRateControlMetrics(rc_metrics, 2, 0, 0, 20, 10, 15, 0, 0);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}

// VP8: Run with no packet loss, with 3 temporal layers, with a rate update in
// the middle of the sequence. The max values for the frame size mismatch and
// encoding rate mismatch are applied to each layer.
// No dropped frames in this test, and internal spatial resizer is off.
// One key frame (first frame only) in sequence, so no spatial resizing.
// Too slow to finish before timeout on iOS. See webrtc:4755.
#if defined(WEBRTC_ANDROID) || defined(WEBRTC_IOS)
#define MAYBE_ProcessNoLossTemporalLayersVP8 \
  DISABLED_ProcessNoLossTemporalLayersVP8
#else
#define MAYBE_ProcessNoLossTemporalLayersVP8 ProcessNoLossTemporalLayersVP8
#endif
TEST_F(VideoProcessorIntegrationTest, MAYBE_ProcessNoLossTemporalLayersVP8)
{
    config_.networking_config.packet_loss_probability = 0;
    // Bitrate and frame rate profile.
    RateProfile rate_profile;
    SetRateProfilePars(&rate_profile, 0, 200, 30, 0);
    SetRateProfilePars(&rate_profile, 1, 400, 30, 150);
    rate_profile.frame_index_rate_update[2] = kNumFramesLong + 1;
    rate_profile.num_frames = kNumFramesLong;
    // Codec/network settings.
    CodecConfigPars process_settings;
    SetCodecParameters(&process_settings, kVideoCodecVP8, 0.0f, -1, 3, false,
                       true, true, false);
    // Metrics for expected quality.
    QualityMetrics quality_metrics;
    SetQualityMetrics(&quality_metrics, 32.5, 30.0, 0.85, 0.80);
    // Metrics for rate control.
    RateControlMetrics rc_metrics[2];
    SetRateControlMetrics(rc_metrics, 0, 0, 20, 30, 10, 10, 0, 1);
    SetRateControlMetrics(rc_metrics, 1, 0, 0, 30, 15, 10, 0, 0);
    ProcessFramesAndVerify(quality_metrics, rate_profile, process_settings,
                           rc_metrics);
}
}  // namespace test
}  // namespace webrtc

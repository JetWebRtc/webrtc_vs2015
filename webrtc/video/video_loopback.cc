﻿/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdio.h>

#include "gflags/gflags.h"
#include "webrtc/test/field_trial.h"
#include "webrtc/test/gtest.h"
#include "webrtc/test/run_test.h"
#include "webrtc/video/video_quality_test.h"

namespace webrtc
{
namespace flags
{

// Flags common with screenshare loopback, with different default values.
DEFINE_int32(width, 640, "Video width.");
size_t Width()
{
    return static_cast<size_t>(FLAGS_width);
}

DEFINE_int32(height, 480, "Video height.");
size_t Height()
{
    return static_cast<size_t>(FLAGS_height);
}

DEFINE_int32(fps, 30, "Frames per second.");
int Fps()
{
    return static_cast<int>(FLAGS_fps);
}

DEFINE_int32(min_bitrate, 50, "Call and stream min bitrate in kbps.");
int MinBitrateKbps()
{
    return static_cast<int>(FLAGS_min_bitrate);
}

DEFINE_int32(start_bitrate, 300, "Call start bitrate in kbps.");
int StartBitrateKbps()
{
    return static_cast<int>(FLAGS_start_bitrate);
}

DEFINE_int32(target_bitrate, 800, "Stream target bitrate in kbps.");
int TargetBitrateKbps()
{
    return static_cast<int>(FLAGS_target_bitrate);
}

DEFINE_int32(max_bitrate, 800, "Call and stream max bitrate in kbps.");
int MaxBitrateKbps()
{
    return static_cast<int>(FLAGS_max_bitrate);
}

DEFINE_bool(suspend_below_min_bitrate,
            false,
            "Suspends video below the configured min bitrate.");

DEFINE_int32(num_temporal_layers,
             1,
             "Number of temporal layers. Set to 1-4 to override.");
int NumTemporalLayers()
{
    return static_cast<int>(FLAGS_num_temporal_layers);
}

// Flags common with screenshare loopback, with equal default values.
DEFINE_string(codec, "VP8", "Video codec to use.");
std::string Codec()
{
    return static_cast<std::string>(FLAGS_codec);
}

DEFINE_int32(selected_tl,
             -1,
             "Temporal layer to show or analyze. -1 to disable filtering.");
int SelectedTL()
{
    return static_cast<int>(FLAGS_selected_tl);
}

DEFINE_int32(
    duration,
    0,
    "Duration of the test in seconds. If 0, rendered will be shown instead.");
int DurationSecs()
{
    return static_cast<int>(FLAGS_duration);
}

DEFINE_string(output_filename, "", "Target graph data filename.");
std::string OutputFilename()
{
    return static_cast<std::string>(FLAGS_output_filename);
}

DEFINE_string(graph_title,
              "",
              "If empty, title will be generated automatically.");
std::string GraphTitle()
{
    return static_cast<std::string>(FLAGS_graph_title);
}

DEFINE_int32(loss_percent, 0, "Percentage of packets randomly lost.");
int LossPercent()
{
    return static_cast<int>(FLAGS_loss_percent);
}

DEFINE_int32(avg_burst_loss_length,
             -1,
             "Average burst length of lost packets.");
int AvgBurstLossLength()
{
    return static_cast<int>(FLAGS_avg_burst_loss_length);
}

DEFINE_int32(link_capacity,
             0,
             "Capacity (kbps) of the fake link. 0 means infinite.");
int LinkCapacityKbps()
{
    return static_cast<int>(FLAGS_link_capacity);
}

DEFINE_int32(queue_size, 0, "Size of the bottleneck link queue in packets.");
int QueueSize()
{
    return static_cast<int>(FLAGS_queue_size);
}

DEFINE_int32(avg_propagation_delay_ms,
             0,
             "Average link propagation delay in ms.");
int AvgPropagationDelayMs()
{
    return static_cast<int>(FLAGS_avg_propagation_delay_ms);
}

DEFINE_int32(std_propagation_delay_ms,
             0,
             "Link propagation delay standard deviation in ms.");
int StdPropagationDelayMs()
{
    return static_cast<int>(FLAGS_std_propagation_delay_ms);
}

DEFINE_int32(selected_stream, 0, "ID of the stream to show or analyze.");
int SelectedStream()
{
    return static_cast<int>(FLAGS_selected_stream);
}

DEFINE_int32(num_spatial_layers, 1, "Number of spatial layers to use.");
int NumSpatialLayers()
{
    return static_cast<int>(FLAGS_num_spatial_layers);
}

DEFINE_int32(selected_sl,
             -1,
             "Spatial layer to show or analyze. -1 to disable filtering.");
int SelectedSL()
{
    return static_cast<int>(FLAGS_selected_sl);
}

DEFINE_string(stream0,
              "",
              "Comma separated values describing VideoStream for stream #0.");
std::string Stream0()
{
    return static_cast<std::string>(FLAGS_stream0);
}

DEFINE_string(stream1,
              "",
              "Comma separated values describing VideoStream for stream #1.");
std::string Stream1()
{
    return static_cast<std::string>(FLAGS_stream1);
}

DEFINE_string(sl0,
              "",
              "Comma separated values describing SpatialLayer for layer #0.");
std::string SL0()
{
    return static_cast<std::string>(FLAGS_sl0);
}

DEFINE_string(sl1,
              "",
              "Comma separated values describing SpatialLayer for layer #1.");
std::string SL1()
{
    return static_cast<std::string>(FLAGS_sl1);
}

DEFINE_string(encoded_frame_path,
              "",
              "The base path for encoded frame logs. Created files will have "
              "the form <encoded_frame_path>.<n>.(recv|send.<m>).ivf");
std::string EncodedFramePath()
{
    return static_cast<std::string>(FLAGS_encoded_frame_path);
}

DEFINE_bool(logs, false, "print logs to stderr");

DEFINE_bool(send_side_bwe, true, "Use send-side bandwidth estimation");

DEFINE_bool(allow_reordering, false, "Allow packet reordering to occur");

DEFINE_bool(use_ulpfec, false, "Use RED+ULPFEC forward error correction.");

DEFINE_bool(use_flexfec, false, "Use FlexFEC forward error correction.");

DEFINE_bool(audio, false, "Add audio stream");

DEFINE_bool(audio_video_sync, false, "Sync audio and video stream (no effect if"
            " audio is false)");

DEFINE_bool(video, true, "Add video stream");

DEFINE_string(
    force_fieldtrials,
    "",
    "Field trials control experimental feature code which can be forced. "
    "E.g. running with --force_fieldtrials=WebRTC-FooFeature/Enable/"
    " will assign the group Enable to field trial WebRTC-FooFeature. Multiple "
    "trials are separated by \"/\"");

// Video-specific flags.
DEFINE_string(clip,
              "",
              "Name of the clip to show. If empty, using chroma generator.");
std::string Clip()
{
    return static_cast<std::string>(FLAGS_clip);
}

}  // namespace flags

void Loopback()
{
    FakeNetworkPipe::Config pipe_config;
    pipe_config.loss_percent = flags::LossPercent();
    pipe_config.avg_burst_loss_length = flags::AvgBurstLossLength();
    pipe_config.link_capacity_kbps = flags::LinkCapacityKbps();
    pipe_config.queue_length_packets = flags::QueueSize();
    pipe_config.queue_delay_ms = flags::AvgPropagationDelayMs();
    pipe_config.delay_standard_deviation_ms = flags::StdPropagationDelayMs();
    pipe_config.allow_reordering = flags::FLAGS_allow_reordering;

    Call::Config::BitrateConfig call_bitrate_config;
    call_bitrate_config.min_bitrate_bps = flags::MinBitrateKbps() * 1000;
    call_bitrate_config.start_bitrate_bps = flags::StartBitrateKbps() * 1000;
    call_bitrate_config.max_bitrate_bps = flags::MaxBitrateKbps() * 1000;

    VideoQualityTest::Params params;
    params.call = {flags::FLAGS_send_side_bwe, call_bitrate_config};
    params.video = {flags::FLAGS_video,
                    flags::Width(),
                    flags::Height(),
                    flags::Fps(),
                    flags::MinBitrateKbps() * 1000,
                    flags::TargetBitrateKbps() * 1000,
                    flags::MaxBitrateKbps() * 1000,
                    flags::FLAGS_suspend_below_min_bitrate,
                    flags::Codec(),
                    flags::NumTemporalLayers(),
                    flags::SelectedTL(),
                    0,  // No min transmit bitrate.
                    flags::FLAGS_use_ulpfec,
                    flags::FLAGS_use_flexfec,
                    flags::EncodedFramePath(),
                    flags::Clip()
                   };
    params.audio = {flags::FLAGS_audio, flags::FLAGS_audio_video_sync};
    params.screenshare.enabled = false;
    params.analyzer = {"video", 0.0, 0.0, flags::DurationSecs(),
                       flags::OutputFilename(), flags::GraphTitle()
                      };
    params.pipe = pipe_config;
    params.logs = flags::FLAGS_logs;

    std::vector<std::string> stream_descriptors;
    stream_descriptors.push_back(flags::Stream0());
    stream_descriptors.push_back(flags::Stream1());
    std::vector<std::string> SL_descriptors;
    SL_descriptors.push_back(flags::SL0());
    SL_descriptors.push_back(flags::SL1());
    VideoQualityTest::FillScalabilitySettings(
        &params, stream_descriptors, flags::SelectedStream(),
        flags::NumSpatialLayers(), flags::SelectedSL(), SL_descriptors);

    VideoQualityTest test;
    if (flags::DurationSecs())
    {
        test.RunWithAnalyzer(params);
    }
    else
    {
        test.RunWithRenderers(params);
    }
}
}  // namespace webrtc

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);
    webrtc::test::InitFieldTrialsFromString(
        webrtc::flags::FLAGS_force_fieldtrials);
    webrtc::test::RunTest(webrtc::Loopback);
    return 0;
}

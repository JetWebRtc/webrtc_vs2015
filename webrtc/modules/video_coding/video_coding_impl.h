﻿/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_VIDEO_CODING_VIDEO_CODING_IMPL_H_
#define WEBRTC_MODULES_VIDEO_CODING_VIDEO_CODING_IMPL_H_

#include "webrtc/modules/video_coding/include/video_coding.h"

#include <memory>
#include <string>
#include <vector>

#include "webrtc/base/onetimeevent.h"
#include "webrtc/base/thread_annotations.h"
#include "webrtc/base/sequenced_task_checker.h"
#include "webrtc/common_video/include/frame_callback.h"
#include "webrtc/modules/video_coding/codec_database.h"
#include "webrtc/modules/video_coding/frame_buffer.h"
#include "webrtc/modules/video_coding/generic_decoder.h"
#include "webrtc/modules/video_coding/generic_encoder.h"
#include "webrtc/modules/video_coding/jitter_buffer.h"
#include "webrtc/modules/video_coding/media_optimization.h"
#include "webrtc/modules/video_coding/receiver.h"
#include "webrtc/modules/video_coding/timing.h"
#include "webrtc/modules/video_coding/utility/qp_parser.h"
#include "webrtc/system_wrappers/include/clock.h"

namespace webrtc
{

class VideoBitrateAllocator;
class VideoBitrateAllocationObserver;

namespace vcm
{

class VCMProcessTimer
{
public:
    static const int64_t kDefaultProcessIntervalMs = 1000;

    VCMProcessTimer(int64_t periodMs, Clock* clock)
        : _clock(clock),
          _periodMs(periodMs),
          _latestMs(_clock->TimeInMilliseconds()) {}
    int64_t Period() const;
    int64_t TimeUntilProcess() const;
    void Processed();

private:
    Clock* _clock;
    int64_t _periodMs;
    int64_t _latestMs;
};

class VideoSender : public Module
{
public:
    typedef VideoCodingModule::SenderNackMode SenderNackMode;

    VideoSender(Clock* clock,
                EncodedImageCallback* post_encode_callback,
                VCMSendStatisticsCallback* send_stats_callback);

    ~VideoSender();

    // Register the send codec to be used.
    // This method must be called on the construction thread.
    int32_t RegisterSendCodec(const VideoCodec* sendCodec,
                              uint32_t numberOfCores,
                              uint32_t maxPayloadSize);

    void RegisterExternalEncoder(VideoEncoder* externalEncoder,
                                 uint8_t payloadType,
                                 bool internalSource);

    int Bitrate(unsigned int* bitrate) const;
    int FrameRate(unsigned int* framerate) const;

    // Update the channel parameters based on new rates and rtt. This will also
    // cause an immediate call to VideoEncoder::SetRateAllocation().
    int32_t SetChannelParameters(
        uint32_t target_bitrate_bps,
        uint8_t loss_rate,
        int64_t rtt,
        VideoBitrateAllocator* bitrate_allocator,
        VideoBitrateAllocationObserver* bitrate_updated_callback);

    // Updates the channel parameters with a new bitrate allocation, but using the
    // current targit_bitrate, loss rate and rtt. That is, the distribution or
    // caps may be updated to a change to a new VideoCodec or allocation mode.
    // The new parameters will be stored as pending EncoderParameters, and the
    // encoder will only be updated on the next frame.
    void UpdateChannelParemeters(
        VideoBitrateAllocator* bitrate_allocator,
        VideoBitrateAllocationObserver* bitrate_updated_callback);

    // Deprecated:
    // TODO(perkj): Remove once no projects use it.
    int32_t RegisterProtectionCallback(VCMProtectionCallback* protection);

    int32_t AddVideoFrame(const VideoFrame& videoFrame,
                          const CodecSpecificInfo* codecSpecificInfo);

    int32_t IntraFrameRequest(size_t stream_index);
    int32_t EnableFrameDropper(bool enable);

    int64_t TimeUntilNextProcess() override;
    void Process() override;

private:
    EncoderParameters UpdateEncoderParameters(
        const EncoderParameters& params,
        VideoBitrateAllocator* bitrate_allocator,
        uint32_t target_bitrate_bps);
    void SetEncoderParameters(EncoderParameters params, bool has_internal_source)
    EXCLUSIVE_LOCKS_REQUIRED(encoder_crit_);

    Clock* const clock_;

    rtc::CriticalSection encoder_crit_;
    VCMGenericEncoder* _encoder;
    media_optimization::MediaOptimization _mediaOpt;
    VCMEncodedFrameCallback _encodedFrameCallback GUARDED_BY(encoder_crit_);
    EncodedImageCallback* const post_encode_callback_;
    VCMSendStatisticsCallback* const send_stats_callback_;
    VCMCodecDataBase _codecDataBase GUARDED_BY(encoder_crit_);
    bool frame_dropper_enabled_ GUARDED_BY(encoder_crit_);
    VCMProcessTimer _sendStatsTimer;

    // Must be accessed on the construction thread of VideoSender.
    VideoCodec current_codec_;
    rtc::SequencedTaskChecker sequenced_checker_;

    rtc::CriticalSection params_crit_;
    EncoderParameters encoder_params_ GUARDED_BY(params_crit_);
    bool encoder_has_internal_source_ GUARDED_BY(params_crit_);
    std::vector<FrameType> next_frame_types_ GUARDED_BY(params_crit_);
};

class VideoReceiver : public Module
{
public:
    typedef VideoCodingModule::ReceiverRobustness ReceiverRobustness;

    VideoReceiver(Clock* clock,
                  EventFactory* event_factory,
                  EncodedImageCallback* pre_decode_image_callback,
                  VCMTiming* timing,
                  NackSender* nack_sender = nullptr,
                  KeyFrameRequestSender* keyframe_request_sender = nullptr);
    ~VideoReceiver();

    int32_t RegisterReceiveCodec(const VideoCodec* receiveCodec,
                                 int32_t numberOfCores,
                                 bool requireKeyFrame);

    void RegisterExternalDecoder(VideoDecoder* externalDecoder,
                                 uint8_t payloadType);
    int32_t RegisterReceiveCallback(VCMReceiveCallback* receiveCallback);
    int32_t RegisterReceiveStatisticsCallback(
        VCMReceiveStatisticsCallback* receiveStats);
    int32_t RegisterDecoderTimingCallback(
        VCMDecoderTimingCallback* decoderTiming);
    int32_t RegisterFrameTypeCallback(VCMFrameTypeCallback* frameTypeCallback);
    int32_t RegisterPacketRequestCallback(VCMPacketRequestCallback* callback);

    int32_t Decode(uint16_t maxWaitTimeMs);

    int32_t Decode(const webrtc::VCMEncodedFrame* frame);

    int32_t ReceiveCodec(VideoCodec* currentReceiveCodec) const;
    VideoCodecType ReceiveCodec() const;

    int32_t IncomingPacket(const uint8_t* incomingPayload,
                           size_t payloadLength,
                           const WebRtcRTPHeader& rtpInfo);
    int32_t SetMinimumPlayoutDelay(uint32_t minPlayoutDelayMs);
    int32_t SetRenderDelay(uint32_t timeMS);
    int32_t Delay() const;
    uint32_t DiscardedPackets() const;

    int SetReceiverRobustnessMode(ReceiverRobustness robustnessMode,
                                  VCMDecodeErrorMode errorMode);
    void SetNackSettings(size_t max_nack_list_size,
                         int max_packet_age_to_nack,
                         int max_incomplete_time_ms);

    void SetDecodeErrorMode(VCMDecodeErrorMode decode_error_mode);
    int SetMinReceiverDelay(int desired_delay_ms);

    int32_t SetReceiveChannelParameters(int64_t rtt);
    int32_t SetVideoProtection(VCMVideoProtection videoProtection, bool enable);

    int64_t TimeUntilNextProcess() override;
    void Process() override;

    void TriggerDecoderShutdown();

protected:
    int32_t Decode(const webrtc::VCMEncodedFrame& frame)
    EXCLUSIVE_LOCKS_REQUIRED(receive_crit_);
    int32_t RequestKeyFrame();
    int32_t RequestSliceLossIndication(const uint64_t pictureID) const;

private:
    Clock* const clock_;
    rtc::CriticalSection process_crit_;
    rtc::CriticalSection receive_crit_;
    VCMTiming* _timing;
    VCMReceiver _receiver;
    VCMDecodedFrameCallback _decodedFrameCallback;
    VCMFrameTypeCallback* _frameTypeCallback GUARDED_BY(process_crit_);
    VCMReceiveStatisticsCallback* _receiveStatsCallback GUARDED_BY(process_crit_);
    VCMDecoderTimingCallback* _decoderTimingCallback GUARDED_BY(process_crit_);
    VCMPacketRequestCallback* _packetRequestCallback GUARDED_BY(process_crit_);
    VCMGenericDecoder* _decoder;

    VCMFrameBuffer _frameFromFile;
    bool _scheduleKeyRequest GUARDED_BY(process_crit_);
    bool drop_frames_until_keyframe_ GUARDED_BY(process_crit_);
    size_t max_nack_list_size_ GUARDED_BY(process_crit_);

    VCMCodecDataBase _codecDataBase GUARDED_BY(receive_crit_);
    EncodedImageCallback* pre_decode_image_callback_;

    VCMProcessTimer _receiveStatsTimer;
    VCMProcessTimer _retransmissionTimer;
    VCMProcessTimer _keyRequestTimer;
    QpParser qp_parser_;
    ThreadUnsafeOneTimeEvent first_frame_received_;
};

}  // namespace vcm
}  // namespace webrtc
#endif  // WEBRTC_MODULES_VIDEO_CODING_VIDEO_CODING_IMPL_H_

﻿/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// TODO(pbos): Move Config from common.h to here.

#ifndef WEBRTC_CONFIG_H_
#define WEBRTC_CONFIG_H_

#include <string>
#include <vector>

#include "webrtc/base/basictypes.h"
#include "webrtc/base/optional.h"
#include "webrtc/base/refcount.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/common_types.h"
#include "webrtc/typedefs.h"

namespace webrtc
{

// Settings for NACK, see RFC 4585 for details.
struct NackConfig
{
    NackConfig() : rtp_history_ms(0) {}
    std::string ToString() const;
    // Send side: the time RTP packets are stored for retransmissions.
    // Receive side: the time the receiver is prepared to wait for
    // retransmissions.
    // Set to '0' to disable.
    int rtp_history_ms;
};

// Settings for ULPFEC forward error correction.
// Set the payload types to '-1' to disable.
struct UlpfecConfig
{
    UlpfecConfig()
        : ulpfec_payload_type(-1),
          red_payload_type(-1),
          red_rtx_payload_type(-1) {}
    std::string ToString() const;
    bool operator==(const UlpfecConfig& other) const;

    // Payload type used for ULPFEC packets.
    int ulpfec_payload_type;

    // Payload type used for RED packets.
    int red_payload_type;

    // RTX payload type for RED payload.
    int red_rtx_payload_type;
};

// RTP header extension, see RFC 5285.
struct RtpExtension
{
    RtpExtension() : id(0) {}
    RtpExtension(const std::string& uri, int id) : uri(uri), id(id) {}
    std::string ToString() const;
    bool operator==(const RtpExtension& rhs) const
    {
        return uri == rhs.uri && id == rhs.id;
    }
    static bool IsSupportedForAudio(const std::string& uri);
    static bool IsSupportedForVideo(const std::string& uri);

    // Header extension for audio levels, as defined in:
    // http://tools.ietf.org/html/draft-ietf-avtext-client-to-mixer-audio-level-03
    static const char* kAudioLevelUri;
    static const int kAudioLevelDefaultId;

    // Header extension for RTP timestamp offset, see RFC 5450 for details:
    // http://tools.ietf.org/html/rfc5450
    static const char* kTimestampOffsetUri;
    static const int kTimestampOffsetDefaultId;

    // Header extension for absolute send time, see url for details:
    // http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
    static const char* kAbsSendTimeUri;
    static const int kAbsSendTimeDefaultId;

    // Header extension for coordination of video orientation, see url for
    // details:
    // http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/ts_126114v120700p.pdf
    static const char* kVideoRotationUri;
    static const int kVideoRotationDefaultId;

    // Header extension for transport sequence number, see url for details:
    // http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions
    static const char* kTransportSequenceNumberUri;
    static const int kTransportSequenceNumberDefaultId;

    static const char* kPlayoutDelayUri;
    static const int kPlayoutDelayDefaultId;

    std::string uri;
    int id;
};

struct VideoStream
{
    VideoStream();
    ~VideoStream();
    std::string ToString() const;

    size_t width;
    size_t height;
    int max_framerate;

    int min_bitrate_bps;
    int target_bitrate_bps;
    int max_bitrate_bps;

    int max_qp;

    // Bitrate thresholds for enabling additional temporal layers. Since these are
    // thresholds in between layers, we have one additional layer. One threshold
    // gives two temporal layers, one below the threshold and one above, two give
    // three, and so on.
    // The VideoEncoder may redistribute bitrates over the temporal layers so a
    // bitrate threshold of 100k and an estimate of 105k does not imply that we
    // get 100k in one temporal layer and 5k in the other, just that the bitrate
    // in the first temporal layer should not exceed 100k.
    // TODO(kthelgason): Apart from a special case for two-layer screencast these
    // thresholds are not propagated to the VideoEncoder. To be implemented.
    std::vector<int> temporal_layer_thresholds_bps;
};

class VideoEncoderConfig
{
public:
    // These are reference counted to permit copying VideoEncoderConfig and be
    // kept alive until all encoder_specific_settings go out of scope.
    // TODO(kthelgason): Consider removing the need for copying VideoEncoderConfig
    // and use rtc::Optional for encoder_specific_settings instead.
    class EncoderSpecificSettings : public rtc::RefCountInterface
    {
    public:
        // TODO(pbos): Remove FillEncoderSpecificSettings as soon as VideoCodec is
        // not in use and encoder implementations ask for codec-specific structs
        // directly.
        void FillEncoderSpecificSettings(VideoCodec* codec_struct) const;

        virtual void FillVideoCodecVp8(VideoCodecVP8* vp8_settings) const;
        virtual void FillVideoCodecVp9(VideoCodecVP9* vp9_settings) const;
        virtual void FillVideoCodecH264(VideoCodecH264* h264_settings) const;
    private:
        ~EncoderSpecificSettings() override {}
        friend class VideoEncoderConfig;
    };

    class H264EncoderSpecificSettings : public EncoderSpecificSettings
    {
    public:
        explicit H264EncoderSpecificSettings(const VideoCodecH264& specifics);
        void FillVideoCodecH264(VideoCodecH264* h264_settings) const override;

    private:
        VideoCodecH264 specifics_;
    };

    class Vp8EncoderSpecificSettings : public EncoderSpecificSettings
    {
    public:
        explicit Vp8EncoderSpecificSettings(const VideoCodecVP8& specifics);
        void FillVideoCodecVp8(VideoCodecVP8* vp8_settings) const override;

    private:
        VideoCodecVP8 specifics_;
    };

    class Vp9EncoderSpecificSettings : public EncoderSpecificSettings
    {
    public:
        explicit Vp9EncoderSpecificSettings(const VideoCodecVP9& specifics);
        void FillVideoCodecVp9(VideoCodecVP9* vp9_settings) const override;

    private:
        VideoCodecVP9 specifics_;
    };

    enum class ContentType
    {
        kRealtimeVideo,
        kScreen,
    };

    class VideoStreamFactoryInterface : public rtc::RefCountInterface
    {
    public:
        // An implementation should return a std::vector<VideoStream> with the
        // wanted VideoStream settings for the given video resolution.
        // The size of the vector may not be larger than
        // |encoder_config.number_of_streams|.
        virtual std::vector<VideoStream> CreateEncoderStreams(
            int width,
            int height,
            const VideoEncoderConfig& encoder_config) = 0;

    protected:
        ~VideoStreamFactoryInterface() override {}
    };

    VideoEncoderConfig& operator=(VideoEncoderConfig&&) = default;
    VideoEncoderConfig& operator=(const VideoEncoderConfig&) = delete;

    // Mostly used by tests.  Avoid creating copies if you can.
    VideoEncoderConfig Copy() const
    {
        return VideoEncoderConfig(*this);
    }

    VideoEncoderConfig();
    VideoEncoderConfig(VideoEncoderConfig&&);
    ~VideoEncoderConfig();
    std::string ToString() const;

    rtc::scoped_refptr<VideoStreamFactoryInterface> video_stream_factory;
    std::vector<SpatialLayer> spatial_layers;
    ContentType content_type;
    rtc::scoped_refptr<const EncoderSpecificSettings> encoder_specific_settings;

    // Padding will be used up to this bitrate regardless of the bitrate produced
    // by the encoder. Padding above what's actually produced by the encoder helps
    // maintaining a higher bitrate estimate. Padding will however not be sent
    // unless the estimated bandwidth indicates that the link can handle it.
    int min_transmit_bitrate_bps;
    int max_bitrate_bps;

    // Max number of encoded VideoStreams to produce.
    size_t number_of_streams;

private:
    // Access to the copy constructor is private to force use of the Copy()
    // method for those exceptional cases where we do use it.
    VideoEncoderConfig(const VideoEncoderConfig&);
};

}  // namespace webrtc

#endif  // WEBRTC_CONFIG_H_

﻿/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_API_RTPPARAMETERS_H_
#define WEBRTC_API_RTPPARAMETERS_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "webrtc/api/mediatypes.h"
#include "webrtc/base/optional.h"

namespace webrtc
{

// These structures are intended to mirror those defined by:
// http://draft.ortc.org/#rtcrtpdictionaries*
// Contains everything specified as of 2017 Jan 24.
//
// They are used when retrieving or modifying the parameters of an
// RtpSender/RtpReceiver, or retrieving capabilities.
//
// Note on conventions: Where ORTC may use "octet", "short" and "unsigned"
// types, we typically use "int", in keeping with our style guidelines. The
// parameter's actual valid range will be enforced when the parameters are set,
// rather than when the parameters struct is built. An exception is made for
// SSRCs, since they use the full unsigned 32-bit range, and aren't expected to
// be used for any numeric comparisons/operations.
//
// Additionally, where ORTC uses strings, we may use enums for things that have
// a fixed number of supported values. However, for things that can be extended
// (such as codecs, by providing an external encoder factory), a string
// identifier is used.

enum class FecMechanism
{
    RED,
    RED_AND_ULPFEC,
    FLEXFEC,
};

// Used in RtcpFeedback struct.
enum class RtcpFeedbackType
{
    ACK,
    CCM,
    NACK,
    REMB,  // "goog-remb"
    TRANSPORT_CC,
};

// Used in RtcpFeedback struct when type is ACK, NACK or CCM.
enum class RtcpFeedbackMessageType
{
    // Equivalent to {type: "nack", parameter: undefined} in ORTC.
    GENERIC_NACK,
    PLI,  // Usable with NACK.
    FIR,  // Usable with CCM.
};

enum class DtxStatus
{
    DISABLED,
    ENABLED,
};

enum class DegradationPreference
{
    MAINTAIN_FRAMERATE,
    MAINTAIN_RESOLUTION,
    BALANCED,
};

enum class PriorityType { VERY_LOW, LOW, MEDIUM, HIGH };

struct RtcpFeedback
{
    RtcpFeedbackType type = RtcpFeedbackType::ACK;

    // Equivalent to ORTC "parameter" field with slight differences:
    // 1. It's an enum instead of a string.
    // 2. Generic NACK feedback is represented by a GENERIC_NACK message type,
    //    rather than an unset "parameter" value.
    rtc::Optional<RtcpFeedbackMessageType> message_type;

    bool operator==(const RtcpFeedback& o) const
    {
        return type == o.type && message_type == o.message_type;
    }
    bool operator!=(const RtcpFeedback& o) const
    {
        return !(*this == o);
    }
};

// RtpCodecCapability is to RtpCodecParameters as RtpCapabilities is to
// RtpParameters. This represents the static capabilities of an endpoint's
// implementation of a codec.
struct RtpCodecCapability
{
    // Build MIME "type/subtype" string from |name| and |kind|.
    std::string mime_type() const
    {
        return MediaTypeToString(kind) + "/" + name;
    }

    // Used to identify the codec. Equivalent to MIME subtype.
    std::string name;

    // The media type of this codec. Equivalent to MIME top-level type.
    cricket::MediaType kind = cricket::MEDIA_TYPE_AUDIO;

    // Clock rate in Hertz. If unset, the codec is applicable to any clock rate.
    rtc::Optional<int> clock_rate;

    // Default payload type for this codec. Mainly needed for codecs that use
    // that have statically assigned payload types.
    rtc::Optional<int> preferred_payload_type;

    // Maximum packetization time supported by an RtpReceiver for this codec.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<int> max_ptime;

    // Preferred packetization time for an RtpReceiver or RtpSender of this
    // codec.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<int> ptime;

    // The number of audio channels supported. Unused for video codecs.
    rtc::Optional<int> num_channels;

    // Feedback mechanisms supported for this codec.
    std::vector<RtcpFeedback> rtcp_feedback;

    // Codec-specific parameters that must be signaled to the remote party.
    // Corresponds to "a=fmtp" parameters in SDP.
    std::unordered_map<std::string, std::string> parameters;

    // Codec-specific parameters that may optionally be signaled to the remote
    // party.
    // TODO(deadbeef): Not implemented.
    std::unordered_map<std::string, std::string> options;

    // Maximum number of temporal layer extensions supported by this codec.
    // For example, a value of 1 indicates that 2 total layers are supported.
    // TODO(deadbeef): Not implemented.
    int max_temporal_layer_extensions = 0;

    // Maximum number of spatial layer extensions supported by this codec.
    // For example, a value of 1 indicates that 2 total layers are supported.
    // TODO(deadbeef): Not implemented.
    int max_spatial_layer_extensions = 0;

    // Whether the implementation can send/receive SVC layers with distinct
    // SSRCs. Always false for audio codecs. True for video codecs that support
    // scalable video coding with MRST.
    // TODO(deadbeef): Not implemented.
    bool svc_multi_stream_support = false;

    bool operator==(const RtpCodecCapability& o) const
    {
        return name == o.name && kind == o.kind && clock_rate == o.clock_rate &&
               preferred_payload_type == o.preferred_payload_type &&
               max_ptime == o.max_ptime && ptime == o.ptime &&
               num_channels == o.num_channels && rtcp_feedback == o.rtcp_feedback &&
               parameters == o.parameters && options == o.options &&
               max_temporal_layer_extensions == o.max_temporal_layer_extensions &&
               max_spatial_layer_extensions == o.max_spatial_layer_extensions &&
               svc_multi_stream_support == o.svc_multi_stream_support;
    }
    bool operator!=(const RtpCodecCapability& o) const
    {
        return !(*this == o);
    }
};

// Used in RtpCapabilities; represents the capabilities/preferences of an
// implementation for a header extension.
//
// Just called "RtpHeaderExtension" in ORTC, but the "Capability" suffix was
// added here for consistency and to avoid confusion with
// RtpHeaderExtensionParameters.
//
// Note that ORTC includes a "kind" field, but we omit this because it's
// redundant; if you call "RtpReceiver::GetCapabilities(MEDIA_TYPE_AUDIO)",
// you know you're getting audio capabilities.
struct RtpHeaderExtensionCapability
{
    // URI of this extension, as defined in RFC5285.
    std::string uri;

    // Preferred value of ID that goes in the packet.
    rtc::Optional<int> preferred_id;

    // If true, it's preferred that the value in the header is encrypted.
    // TODO(deadbeef): Not implemented.
    bool preferred_encrypt = false;

    bool operator==(const RtpHeaderExtensionCapability& o) const
    {
        return uri == o.uri && preferred_id == o.preferred_id &&
               preferred_encrypt == o.preferred_encrypt;
    }
    bool operator!=(const RtpHeaderExtensionCapability& o) const
    {
        return !(*this == o);
    }
};

// Used in RtpParameters; represents a specific configuration of a header
// extension.
struct RtpHeaderExtensionParameters
{
    // URI of this extension, as defined in RFC5285.
    std::string uri;

    // ID value that goes in the packet.
    int id = 0;

    // If true, the value in the header is encrypted.
    // TODO(deadbeef): Not implemented.
    bool encrypt = false;

    bool operator==(const RtpHeaderExtensionParameters& o) const
    {
        return uri == o.uri && id == o.id && encrypt == o.encrypt;
    }
    bool operator!=(const RtpHeaderExtensionParameters& o) const
    {
        return !(*this == o);
    }
};

struct RtpFecParameters
{
    // If unset, a value is chosen by the implementation.
    rtc::Optional<uint32_t> ssrc;

    FecMechanism mechanism = FecMechanism::RED;

    bool operator==(const RtpFecParameters& o) const
    {
        return ssrc == o.ssrc && mechanism == o.mechanism;
    }
    bool operator!=(const RtpFecParameters& o) const
    {
        return !(*this == o);
    }
};

struct RtpRtxParameters
{
    // If unset, a value is chosen by the implementation.
    rtc::Optional<uint32_t> ssrc;

    bool operator==(const RtpRtxParameters& o) const
    {
        return ssrc == o.ssrc;
    }
    bool operator!=(const RtpRtxParameters& o) const
    {
        return !(*this == o);
    }
};

struct RtpEncodingParameters
{
    // If unset, a value is chosen by the implementation.
    rtc::Optional<uint32_t> ssrc;

    // Can be used to reference a codec in the |codecs| member of the
    // RtpParameters that contains this RtpEncodingParameters. If unset, the
    // implementation will choose the first possible codec.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<int> codec_payload_type;

    // Specifies the FEC mechanism, if set.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<RtpFecParameters> fec;

    // Specifies the RTX parameters, if set.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<RtpRtxParameters> rtx;

    // Only used for audio. If set, determines whether or not discontinuous
    // transmission will be used, if an available codec supports it. If not
    // set, the implementation default setting will be used.
    rtc::Optional<DtxStatus> dtx;

    // The relative priority of this encoding.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<PriorityType> priority;

    // If set, this represents the Transport Independent Application Specific
    // maximum bandwidth defined in RFC3890. If unset, there is no maximum
    // bitrate.
    // Just called "maxBitrate" in ORTC spec.
    rtc::Optional<int> max_bitrate_bps;

    // TODO(deadbeef): Not implemented.
    rtc::Optional<int> max_framerate;

    // For video, scale the resolution down by this factor.
    // TODO(deadbeef): Not implemented.
    double scale_resolution_down_by = 1.0;

    // Scale the framerate down by this factor.
    // TODO(deadbeef): Not implemented.
    double scale_framerate_down_by = 1.0;

    // For an RtpSender, set to true to cause this encoding to be sent, and false
    // for it not to be sent. For an RtpReceiver, set to true to cause the
    // encoding to be decoded, and false for it to be ignored.
    // TODO(deadbeef): RtpReceiver part is not implemented.
    bool active = true;

    // Value to use for RID RTP header extension.
    // Called "encodingId" in ORTC.
    // TODO(deadbeef): Not implemented.
    std::string rid;

    // RIDs of encodings on which this layer depends.
    // Called "dependencyEncodingIds" in ORTC spec.
    // TODO(deadbeef): Not implemented.
    std::vector<std::string> dependency_rids;

    bool operator==(const RtpEncodingParameters& o) const
    {
        return ssrc == o.ssrc && codec_payload_type == o.codec_payload_type &&
               fec == o.fec && rtx == o.rtx && dtx == o.dtx &&
               priority == o.priority && max_bitrate_bps == o.max_bitrate_bps &&
               max_framerate == o.max_framerate &&
               scale_resolution_down_by == o.scale_resolution_down_by &&
               scale_framerate_down_by == o.scale_framerate_down_by &&
               active == o.active && rid == o.rid &&
               dependency_rids == o.dependency_rids;
    }
    bool operator!=(const RtpEncodingParameters& o) const
    {
        return !(*this == o);
    }
};

struct RtpCodecParameters
{
    // Build MIME "type/subtype" string from |name| and |kind|.
    std::string mime_type() const
    {
        return MediaTypeToString(kind) + "/" + name;
    }

    // Used to identify the codec. Equivalent to MIME subtype.
    std::string name;

    // The media type of this codec. Equivalent to MIME top-level type.
    cricket::MediaType kind = cricket::MEDIA_TYPE_AUDIO;

    // Payload type used to identify this codec in RTP packets.
    // This MUST always be present, and must be unique across all codecs using
    // the same transport.
    int payload_type = 0;

    // If unset, the implementation default is used.
    rtc::Optional<int> clock_rate;

    // The number of audio channels used. Unset for video codecs. If unset for
    // audio, the implementation default is used.
    // TODO(deadbeef): The "implementation default" part is unimplemented.
    rtc::Optional<int> num_channels;

    // The maximum packetization time to be used by an RtpSender.
    // If |ptime| is also set, this will be ignored.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<int> max_ptime;

    // The packetization time to be used by an RtpSender.
    // If unset, will use any time up to max_ptime.
    // TODO(deadbeef): Not implemented.
    rtc::Optional<int> ptime;

    // Feedback mechanisms to be used for this codec.
    // TODO(deadbeef): Not implemented.
    std::vector<RtcpFeedback> rtcp_feedback;

    // Codec-specific parameters that must be signaled to the remote party.
    // Corresponds to "a=fmtp" parameters in SDP.
    // TODO(deadbeef): Not implemented.
    std::unordered_map<std::string, std::string> parameters;

    bool operator==(const RtpCodecParameters& o) const
    {
        return name == o.name && kind == o.kind && payload_type == o.payload_type &&
               clock_rate == o.clock_rate && num_channels == o.num_channels &&
               max_ptime == o.max_ptime && ptime == o.ptime &&
               rtcp_feedback == o.rtcp_feedback && parameters == o.parameters;
    }
    bool operator!=(const RtpCodecParameters& o) const
    {
        return !(*this == o);
    }
};

// RtpCapabilities is used to represent the static capabilities of an
// endpoint. An application can use these capabilities to construct an
// RtpParameters.
struct RtpCapabilities
{
    // Supported codecs.
    std::vector<RtpCodecCapability> codecs;

    // Supported RTP header extensions.
    std::vector<RtpHeaderExtensionCapability> header_extensions;

    // Supported Forward Error Correction (FEC) mechanisms.
    std::vector<FecMechanism> fec;

    bool operator==(const RtpCapabilities& o) const
    {
        return codecs == o.codecs && header_extensions == o.header_extensions &&
               fec == o.fec;
    }
    bool operator!=(const RtpCapabilities& o) const
    {
        return !(*this == o);
    }
};

// Note that unlike in ORTC, an RtcpParameters is not included in
// RtpParameters, because our API will include an additional "RtpTransport"
// abstraction on which RTCP parameters are set.
struct RtpParameters
{
    // Used when calling getParameters/setParameters with a PeerConnection
    // RtpSender, to ensure that outdated parameters are not unintentionally
    // applied successfully.
    // TODO(deadbeef): Not implemented.
    std::string transaction_id;

    // Value to use for MID RTP header extension.
    // Called "muxId" in ORTC.
    // TODO(deadbeef): Not implemented.
    std::string mid;

    std::vector<RtpCodecParameters> codecs;

    // TODO(deadbeef): Not implemented.
    std::vector<RtpHeaderExtensionParameters> header_extensions;

    std::vector<RtpEncodingParameters> encodings;

    // TODO(deadbeef): Not implemented.
    DegradationPreference degradation_preference =
        DegradationPreference::BALANCED;

    bool operator==(const RtpParameters& o) const
    {
        return mid == o.mid && codecs == o.codecs &&
               header_extensions == o.header_extensions &&
               encodings == o.encodings &&
               degradation_preference == o.degradation_preference;
    }
    bool operator!=(const RtpParameters& o) const
    {
        return !(*this == o);
    }
};

}  // namespace webrtc

#endif  // WEBRTC_API_RTPPARAMETERS_H_

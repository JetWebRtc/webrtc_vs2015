﻿/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/media/base/mediaconstants.h"

#include <string>

namespace cricket
{

const int kVideoCodecClockrate = 90000;
const int kDataCodecClockrate = 90000;
const int kDataMaxBandwidth = 30720;  // bps

const float kHighSystemCpuThreshold = 0.85f;
const float kLowSystemCpuThreshold = 0.65f;
const float kProcessCpuThreshold = 0.10f;

const char kRtxCodecName[] = "rtx";
const char kRedCodecName[] = "red";
const char kUlpfecCodecName[] = "ulpfec";

// TODO(brandtr): Change this to 'flexfec' when we are confident that the
// header format is not changing anymore.
const char kFlexfecCodecName[] = "flexfec-03";

// draft-ietf-payload-flexible-fec-scheme-02.txt
const char kFlexfecFmtpRepairWindow[] = "repair-window";

const char kCodecParamAssociatedPayloadType[] = "apt";

const char kOpusCodecName[] = "opus";
const char kIsacCodecName[] = "isac";
const char kL16CodecName[]  = "l16";
const char kG722CodecName[] = "g722";
const char kIlbcCodecName[] = "ilbc";
const char kPcmuCodecName[] = "pcmu";
const char kPcmaCodecName[] = "pcma";
const char kCnCodecName[]   = "cn";
const char kDtmfCodecName[] = "telephone-event";

// draft-spittka-payload-rtp-opus-03.txt
const char kCodecParamPTime[] = "ptime";
const char kCodecParamMaxPTime[] = "maxptime";
const char kCodecParamMinPTime[] = "minptime";
const char kCodecParamSPropStereo[] = "sprop-stereo";
const char kCodecParamStereo[] = "stereo";
const char kCodecParamUseInbandFec[] = "useinbandfec";
const char kCodecParamUseDtx[] = "usedtx";
const char kCodecParamMaxAverageBitrate[] = "maxaveragebitrate";
const char kCodecParamMaxPlaybackRate[] = "maxplaybackrate";

const char kCodecParamSctpProtocol[] = "protocol";
const char kCodecParamSctpStreams[] = "streams";

const char kParamValueTrue[] = "1";
const char kParamValueEmpty[] = "";

const int kOpusDefaultMaxPTime = 120;
const int kOpusDefaultPTime = 20;
const int kOpusDefaultMinPTime = 3;
const int kOpusDefaultSPropStereo = 0;
const int kOpusDefaultStereo = 0;
const int kOpusDefaultUseInbandFec = 0;
const int kOpusDefaultUseDtx = 0;
const int kOpusDefaultMaxPlaybackRate = 48000;

const int kPreferredMaxPTime = 120;
const int kPreferredMinPTime = 10;
const int kPreferredSPropStereo = 0;
const int kPreferredStereo = 0;
const int kPreferredUseInbandFec = 0;

const char kRtcpFbParamNack[] = "nack";
const char kRtcpFbNackParamPli[] = "pli";
const char kRtcpFbParamRemb[] = "goog-remb";
const char kRtcpFbParamTransportCc[] = "transport-cc";

const char kRtcpFbParamCcm[] = "ccm";
const char kRtcpFbCcmParamFir[] = "fir";
const char kCodecParamMaxBitrate[] = "x-google-max-bitrate";
const char kCodecParamMinBitrate[] = "x-google-min-bitrate";
const char kCodecParamStartBitrate[] = "x-google-start-bitrate";
const char kCodecParamMaxQuantization[] = "x-google-max-quantization";
const char kCodecParamPort[] = "x-google-port";

const int kGoogleRtpDataCodecPlType = 109;
const char kGoogleRtpDataCodecName[] = "google-data";

const int kGoogleSctpDataCodecPlType = 108;
const char kGoogleSctpDataCodecName[] = "google-sctp-data";

const char kComfortNoiseCodecName[] = "CN";

const char kVp8CodecName[] = "VP8";
const char kVp9CodecName[] = "VP9";
const char kH264CodecName[] = "H264";

// RFC 6184 RTP Payload Format for H.264 video
const char kH264FmtpProfileLevelId[] = "profile-level-id";
const char kH264FmtpLevelAsymmetryAllowed[] = "level-asymmetry-allowed";
const char kH264FmtpPacketizationMode[] = "packetization-mode";
const char kH264FmtpSpropParameterSets[] = "sprop-parameter-sets";
const char kH264ProfileLevelConstrainedBaseline[] = "42e01f";

const int kDefaultVideoMaxFramerate = 60;
}  // namespace cricket

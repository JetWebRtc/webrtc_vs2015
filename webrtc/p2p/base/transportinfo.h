﻿/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_P2P_BASE_TRANSPORTINFO_H_
#define WEBRTC_P2P_BASE_TRANSPORTINFO_H_

#include <string>
#include <vector>

#include "webrtc/p2p/base/candidate.h"
#include "webrtc/p2p/base/p2pconstants.h"
#include "webrtc/p2p/base/transportdescription.h"
#include "webrtc/base/helpers.h"

namespace cricket
{

// A TransportInfo is NOT a transport-info message.  It is comparable
// to a "ContentInfo". A transport-infos message is basically just a
// collection of TransportInfos.
struct TransportInfo
{
    TransportInfo() {}

    TransportInfo(const std::string& content_name,
                  const TransportDescription& description)
        : content_name(content_name),
          description(description) {}

    std::string content_name;
    TransportDescription description;
};

typedef std::vector<TransportInfo> TransportInfos;

}  // namespace cricket

#endif  // WEBRTC_P2P_BASE_TRANSPORTINFO_H_

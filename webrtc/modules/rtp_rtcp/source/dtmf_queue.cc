﻿/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/rtp_rtcp/source/dtmf_queue.h"

namespace
{
constexpr size_t kDtmfOutbandMax = 20;
}

namespace webrtc
{
DtmfQueue::DtmfQueue() {}

DtmfQueue::~DtmfQueue() {}

bool DtmfQueue::AddDtmf(const Event& event)
{
    rtc::CritScope lock(&dtmf_critsect_);
    if (queue_.size() >= kDtmfOutbandMax)
    {
        return false;
    }
    queue_.push_back(event);
    return true;
}

bool DtmfQueue::NextDtmf(Event* event)
{
    RTC_DCHECK(event);
    rtc::CritScope lock(&dtmf_critsect_);
    if (queue_.empty())
    {
        return false;
    }

    *event = queue_.front();
    queue_.pop_front();
    return true;
}

bool DtmfQueue::PendingDtmf() const
{
    rtc::CritScope lock(&dtmf_critsect_);
    return !queue_.empty();
}
}  // namespace webrtc

﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// TODO(zijiehe): Implement ScreenDrawerMac

#include "webrtc/modules/desktop_capture/screen_drawer.h"

namespace webrtc
{

// static
std::unique_ptr<ScreenDrawerLock> ScreenDrawerLock::Create()
{
    return nullptr;
}

// static
std::unique_ptr<ScreenDrawer> ScreenDrawer::Create()
{
    return nullptr;
}

}  // namespace webrtc

﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_DESKTOP_CAPTURE_WIN_DXGI_ADAPTER_DUPLICATOR_H_
#define WEBRTC_MODULES_DESKTOP_CAPTURE_WIN_DXGI_ADAPTER_DUPLICATOR_H_

#include <wrl/client.h>

#include <vector>

#include "webrtc/modules/desktop_capture/desktop_geometry.h"
#include "webrtc/modules/desktop_capture/desktop_region.h"
#include "webrtc/modules/desktop_capture/shared_desktop_frame.h"
#include "webrtc/modules/desktop_capture/win/d3d_device.h"
#include "webrtc/modules/desktop_capture/win/dxgi_output_duplicator.h"

namespace webrtc
{

// A container of DxgiOutputDuplicators to duplicate monitors attached to a
// single video card.
class DxgiAdapterDuplicator
{
public:
    struct Context
    {
        Context();
        Context(const Context& other);
        ~Context();

        // Child DxgiOutputDuplicator::Context belongs to this
        // DxgiAdapterDuplicator::Context.
        std::vector<DxgiOutputDuplicator::Context> contexts;
    };

    // Creates an instance of DxgiAdapterDuplicator from a D3dDevice. Only
    // DxgiDuplicatorController can create an instance.
    explicit DxgiAdapterDuplicator(const D3dDevice& device);

    // Move constructor, to make it possible to store instances of
    // DxgiAdapterDuplicator in std::vector<>.
    DxgiAdapterDuplicator(DxgiAdapterDuplicator&& other);

    ~DxgiAdapterDuplicator();

    // Initializes the DxgiAdapterDuplicator from a D3dDevice.
    bool Initialize();

    // Sequentially calls Duplicate function of all the DxgiOutputDuplicator
    // instances owned by this instance, and writes into |target|.
    bool Duplicate(Context* context, SharedDesktopFrame* target);

    // Captures one monitor and writes into |target|. |monitor_id| should be
    // between [0, screen_count()).
    bool DuplicateMonitor(Context* context,
                          int monitor_id,
                          SharedDesktopFrame* target);

    // Returns desktop rect covered by this DxgiAdapterDuplicator.
    DesktopRect desktop_rect() const
    {
        return desktop_rect_;
    }

    // Returns the size of one screen owned by this DxgiAdapterDuplicator. |id|
    // should be between [0, screen_count()).
    DesktopRect ScreenRect(int id) const;

    // Returns the count of screens owned by this DxgiAdapterDuplicator. These
    // screens can be retrieved by an interger in the range of
    // [0, screen_count()).
    int screen_count() const
    {
        return static_cast<int>(duplicators_.size());
    }

private:
    friend class DxgiDuplicatorController;

    bool DoInitialize();

    void Setup(Context* context);

    void Unregister(const Context* const context);

    const D3dDevice device_;
    std::vector<DxgiOutputDuplicator> duplicators_;
    DesktopRect desktop_rect_;
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_DESKTOP_CAPTURE_WIN_DXGI_ADAPTER_DUPLICATOR_H_

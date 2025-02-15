﻿/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include "webrtc/modules/desktop_capture/mouse_cursor_monitor.h"
#include "webrtc/modules/desktop_capture/desktop_capturer.h"
#include "webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "webrtc/modules/desktop_capture/desktop_frame.h"
#include "webrtc/modules/desktop_capture/mouse_cursor.h"
#include "webrtc/system_wrappers/include/logging.h"
#include "webrtc/test/gtest.h"

namespace webrtc
{

class MouseCursorMonitorTest : public testing::Test,
    public MouseCursorMonitor::Callback
{
public:
    MouseCursorMonitorTest()
        : position_received_(false)
    {
    }

    // MouseCursorMonitor::Callback interface
    void OnMouseCursor(MouseCursor* cursor_image) override
    {
        cursor_image_.reset(cursor_image);
    }

    void OnMouseCursorPosition(MouseCursorMonitor::CursorState state,
                               const DesktopVector& position) override
    {
        state_ = state;
        position_ = position;
        position_received_ = true;
    }

protected:
    std::unique_ptr<MouseCursor> cursor_image_;
    MouseCursorMonitor::CursorState state_;
    DesktopVector position_;
    bool position_received_;
};

// TODO(sergeyu): On Mac we need to initialize NSApplication before running the
// tests. Figure out how to do that without breaking other tests in
// modules_unittests and enable these tests on Mac.
// https://code.google.com/p/webrtc/issues/detail?id=2532
//
// Disabled on Windows due to flake, see:
// https://code.google.com/p/webrtc/issues/detail?id=3408
// Disabled on Linux due to flake, see:
// https://code.google.com/p/webrtc/issues/detail?id=3245
#if !defined(WEBRTC_MAC) && !defined(WEBRTC_WIN) && !defined(WEBRTC_LINUX)
#define MAYBE(x) x
#else
#define MAYBE(x) DISABLED_##x
#endif

TEST_F(MouseCursorMonitorTest, MAYBE(FromScreen))
{
    std::unique_ptr<MouseCursorMonitor> capturer(
        MouseCursorMonitor::CreateForScreen(
            DesktopCaptureOptions::CreateDefault(),
            webrtc::kFullDesktopScreenId));
    assert(capturer.get());
    capturer->Init(this, MouseCursorMonitor::SHAPE_AND_POSITION);
    capturer->Capture();

    EXPECT_TRUE(cursor_image_.get());
    EXPECT_GE(cursor_image_->hotspot().x(), 0);
    EXPECT_LE(cursor_image_->hotspot().x(),
              cursor_image_->image()->size().width());
    EXPECT_GE(cursor_image_->hotspot().y(), 0);
    EXPECT_LE(cursor_image_->hotspot().y(),
              cursor_image_->image()->size().height());

    EXPECT_TRUE(position_received_);
    EXPECT_EQ(MouseCursorMonitor::INSIDE, state_);
}

TEST_F(MouseCursorMonitorTest, MAYBE(FromWindow))
{
    DesktopCaptureOptions options = DesktopCaptureOptions::CreateDefault();

    // First get list of windows.
    std::unique_ptr<DesktopCapturer> window_capturer(
        DesktopCapturer::CreateWindowCapturer(options));

    // If window capturing is not supported then skip this test.
    if (!window_capturer.get())
        return;

    DesktopCapturer::SourceList sources;
    EXPECT_TRUE(window_capturer->GetSourceList(&sources));

    // Iterate over all windows and try capturing mouse cursor for each of them.
    for (size_t i = 0; i < sources.size(); ++i)
    {
        cursor_image_.reset();
        position_received_ = false;

        std::unique_ptr<MouseCursorMonitor> capturer(
            MouseCursorMonitor::CreateForWindow(
                DesktopCaptureOptions::CreateDefault(), sources[i].id));
        assert(capturer.get());

        capturer->Init(this, MouseCursorMonitor::SHAPE_AND_POSITION);
        capturer->Capture();

        EXPECT_TRUE(cursor_image_.get());
        EXPECT_TRUE(position_received_);
    }
}

// Make sure that OnMouseCursorPosition() is not called in the SHAPE_ONLY mode.
TEST_F(MouseCursorMonitorTest, MAYBE(ShapeOnly))
{
    std::unique_ptr<MouseCursorMonitor> capturer(
        MouseCursorMonitor::CreateForScreen(
            DesktopCaptureOptions::CreateDefault(),
            webrtc::kFullDesktopScreenId));
    assert(capturer.get());
    capturer->Init(this, MouseCursorMonitor::SHAPE_ONLY);
    capturer->Capture();

    EXPECT_TRUE(cursor_image_.get());
    EXPECT_FALSE(position_received_);
}

}  // namespace webrtc

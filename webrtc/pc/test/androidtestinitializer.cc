﻿/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/pc/test/androidtestinitializer.h"

#include <pthread.h>

#include "webrtc/base/ignore_wundef.h"

// Note: this dependency is dangerous since it reaches into Chromium's base.
// There's a risk of e.g. macro clashes. This file may only be used in tests.
// Since we use Chromes build system for creating the gtest binary, this should
// be fine.
RTC_PUSH_IGNORING_WUNDEF()
#include "base/android/context_utils.h"
#include "base/android/jni_android.h"
RTC_POP_IGNORING_WUNDEF()

#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/voice_engine/include/voe_base.h"

namespace webrtc
{

namespace
{

static pthread_once_t g_initialize_once = PTHREAD_ONCE_INIT;

// There can only be one JNI_OnLoad in each binary. So since this is a GTEST
// C++ runner binary, we want to initialize the same global objects we normally
// do if this had been a Java binary.
void EnsureInitializedOnce()
{
    RTC_CHECK(::base::android::IsVMInitialized());
    JNIEnv* jni = ::base::android::AttachCurrentThread();
    JavaVM* jvm = NULL;
    RTC_CHECK_EQ(0, jni->GetJavaVM(&jvm));
    jobject context = ::base::android::GetApplicationContext().obj();

    RTC_CHECK(rtc::InitializeSSL()) << "Failed to InitializeSSL()";

    webrtc::VoiceEngine::SetAndroidObjects(jvm, context);
}

}  // anonymous namespace

void InitializeAndroidObjects()
{
    RTC_CHECK_EQ(0, pthread_once(&g_initialize_once, &EnsureInitializedOnce));
}

}  // namespace webrtc

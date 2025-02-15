﻿/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VPX_MEM_VPX_MEM_H_
#define VPX_MEM_VPX_MEM_H_

#include "vpx_config.h"
#if defined(__uClinux__)
#include <lddk.h>
#endif

#include <stdlib.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

void *vpx_memalign(size_t align, size_t size);
void *vpx_malloc(size_t size);
void *vpx_calloc(size_t num, size_t size);
void vpx_free(void *memblk);

#if CONFIG_VP9_HIGHBITDEPTH
void *vpx_memset16(void *dest, int val, size_t length);
#endif

#include <string.h>

#ifdef VPX_MEM_PLTFRM
#include VPX_MEM_PLTFRM
#endif

#if defined(__cplusplus)
}
#endif

#endif  // VPX_MEM_VPX_MEM_H_

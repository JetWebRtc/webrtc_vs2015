﻿/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef IVFENC_H_
#define IVFENC_H_

#include "./tools_common.h"

struct vpx_codec_enc_cfg;
struct vpx_codec_cx_pkt;

#ifdef __cplusplus
extern "C" {
#endif

void ivf_write_file_header(FILE *outfile, const struct vpx_codec_enc_cfg *cfg,
                           uint32_t fourcc, int frame_cnt);

void ivf_write_frame_header(FILE *outfile, int64_t pts, size_t frame_size);

void ivf_write_frame_size(FILE *outfile, size_t frame_size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // IVFENC_H_

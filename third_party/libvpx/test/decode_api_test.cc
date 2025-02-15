﻿/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "third_party/googletest/src/include/gtest/gtest.h"

#include "./vpx_config.h"
#include "test/ivf_video_source.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_decoder.h"

namespace
{

#define NELEMENTS(x) static_cast<int>(sizeof(x) / sizeof(x[0]))

TEST(DecodeAPI, InvalidParams)
{
    static const vpx_codec_iface_t *kCodecs[] =
    {
#if CONFIG_VP8_DECODER
        &vpx_codec_vp8_dx_algo,
#endif
#if CONFIG_VP9_DECODER
        &vpx_codec_vp9_dx_algo,
#endif
    };
    uint8_t buf[1] = { 0 };
    vpx_codec_ctx_t dec;

    EXPECT_EQ(VPX_CODEC_INVALID_PARAM, vpx_codec_dec_init(NULL, NULL, NULL, 0));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM, vpx_codec_dec_init(&dec, NULL, NULL, 0));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM, vpx_codec_decode(NULL, NULL, 0, NULL, 0));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM, vpx_codec_decode(NULL, buf, 0, NULL, 0));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
              vpx_codec_decode(NULL, buf, NELEMENTS(buf), NULL, 0));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
              vpx_codec_decode(NULL, NULL, NELEMENTS(buf), NULL, 0));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM, vpx_codec_destroy(NULL));
    EXPECT_TRUE(vpx_codec_error(NULL) != NULL);

    for (int i = 0; i < NELEMENTS(kCodecs); ++i)
    {
        EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
                  vpx_codec_dec_init(NULL, kCodecs[i], NULL, 0));

        EXPECT_EQ(VPX_CODEC_OK, vpx_codec_dec_init(&dec, kCodecs[i], NULL, 0));
        EXPECT_EQ(VPX_CODEC_UNSUP_BITSTREAM,
                  vpx_codec_decode(&dec, buf, NELEMENTS(buf), NULL, 0));
        EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
                  vpx_codec_decode(&dec, NULL, NELEMENTS(buf), NULL, 0));
        EXPECT_EQ(VPX_CODEC_INVALID_PARAM, vpx_codec_decode(&dec, buf, 0, NULL, 0));

        EXPECT_EQ(VPX_CODEC_OK, vpx_codec_destroy(&dec));
    }
}

#if CONFIG_VP8_DECODER
TEST(DecodeAPI, OptionalParams)
{
    vpx_codec_ctx_t dec;

#if CONFIG_ERROR_CONCEALMENT
    EXPECT_EQ(VPX_CODEC_OK, vpx_codec_dec_init(&dec, &vpx_codec_vp8_dx_algo, NULL,
              VPX_CODEC_USE_ERROR_CONCEALMENT));
#else
    EXPECT_EQ(VPX_CODEC_INCAPABLE,
              vpx_codec_dec_init(&dec, &vpx_codec_vp8_dx_algo, NULL,
                                 VPX_CODEC_USE_ERROR_CONCEALMENT));
#endif  // CONFIG_ERROR_CONCEALMENT
}
#endif  // CONFIG_VP8_DECODER

#if CONFIG_VP9_DECODER
// Test VP9 codec controls after a decode error to ensure the code doesn't
// misbehave.
void TestVp9Controls(vpx_codec_ctx_t *dec)
{
    static const int kControls[] = { VP8D_GET_LAST_REF_UPDATES,
                                     VP8D_GET_FRAME_CORRUPTED,
                                     VP9D_GET_DISPLAY_SIZE, VP9D_GET_FRAME_SIZE
                                   };
    int val[2];

    for (int i = 0; i < NELEMENTS(kControls); ++i)
    {
        const vpx_codec_err_t res = vpx_codec_control_(dec, kControls[i], val);
        switch (kControls[i])
        {
        case VP8D_GET_FRAME_CORRUPTED:
            EXPECT_EQ(VPX_CODEC_ERROR, res) << kControls[i];
            break;
        default:
            EXPECT_EQ(VPX_CODEC_OK, res) << kControls[i];
            break;
        }
        EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
                  vpx_codec_control_(dec, kControls[i], NULL));
    }

    vp9_ref_frame_t ref;
    ref.idx = 0;
    EXPECT_EQ(VPX_CODEC_ERROR, vpx_codec_control(dec, VP9_GET_REFERENCE, &ref));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
              vpx_codec_control(dec, VP9_GET_REFERENCE, NULL));

    vpx_ref_frame_t ref_copy;
    const int width = 352;
    const int height = 288;
    ASSERT_TRUE(
        vpx_img_alloc(&ref_copy.img, VPX_IMG_FMT_I420, width, height, 1) != NULL);
    ref_copy.frame_type = VP8_LAST_FRAME;
    EXPECT_EQ(VPX_CODEC_ERROR,
              vpx_codec_control(dec, VP8_COPY_REFERENCE, &ref_copy));
    EXPECT_EQ(VPX_CODEC_INVALID_PARAM,
              vpx_codec_control(dec, VP8_COPY_REFERENCE, NULL));
    vpx_img_free(&ref_copy.img);
}

TEST(DecodeAPI, Vp9InvalidDecode)
{
    const vpx_codec_iface_t *const codec = &vpx_codec_vp9_dx_algo;
    const char filename[] =
        "invalid-vp90-2-00-quantizer-00.webm.ivf.s5861_r01-05_b6-.v2.ivf";
    libvpx_test::IVFVideoSource video(filename);
    video.Init();
    video.Begin();
    ASSERT_TRUE(!HasFailure());

    vpx_codec_ctx_t dec;
    EXPECT_EQ(VPX_CODEC_OK, vpx_codec_dec_init(&dec, codec, NULL, 0));
    const uint32_t frame_size = static_cast<uint32_t>(video.frame_size());
#if CONFIG_VP9_HIGHBITDEPTH
    EXPECT_EQ(VPX_CODEC_MEM_ERROR,
              vpx_codec_decode(&dec, video.cxdata(), frame_size, NULL, 0));
#else
    EXPECT_EQ(VPX_CODEC_UNSUP_BITSTREAM,
              vpx_codec_decode(&dec, video.cxdata(), frame_size, NULL, 0));
#endif
    vpx_codec_iter_t iter = NULL;
    EXPECT_EQ(NULL, vpx_codec_get_frame(&dec, &iter));

    TestVp9Controls(&dec);
    EXPECT_EQ(VPX_CODEC_OK, vpx_codec_destroy(&dec));
}

TEST(DecodeAPI, Vp9PeekSI)
{
    const vpx_codec_iface_t *const codec = &vpx_codec_vp9_dx_algo;
    // The first 9 bytes are valid and the rest of the bytes are made up. Until
    // size 10, this should return VPX_CODEC_UNSUP_BITSTREAM and after that it
    // should return VPX_CODEC_CORRUPT_FRAME.
    const uint8_t data[32] =
    {
        0x85, 0xa4, 0xc1, 0xa1, 0x38, 0x81, 0xa3, 0x49, 0x83, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    for (uint32_t data_sz = 1; data_sz <= 32; ++data_sz)
    {
        // Verify behavior of vpx_codec_decode. vpx_codec_decode doesn't even get
        // to decoder_peek_si_internal on frames of size < 8.
        if (data_sz >= 8)
        {
            vpx_codec_ctx_t dec;
            EXPECT_EQ(VPX_CODEC_OK, vpx_codec_dec_init(&dec, codec, NULL, 0));
            EXPECT_EQ(
                (data_sz < 10) ? VPX_CODEC_UNSUP_BITSTREAM : VPX_CODEC_CORRUPT_FRAME,
                vpx_codec_decode(&dec, data, data_sz, NULL, 0));
            vpx_codec_iter_t iter = NULL;
            EXPECT_EQ(NULL, vpx_codec_get_frame(&dec, &iter));
            EXPECT_EQ(VPX_CODEC_OK, vpx_codec_destroy(&dec));
        }

        // Verify behavior of vpx_codec_peek_stream_info.
        vpx_codec_stream_info_t si;
        si.sz = sizeof(si);
        EXPECT_EQ((data_sz < 10) ? VPX_CODEC_UNSUP_BITSTREAM : VPX_CODEC_OK,
                  vpx_codec_peek_stream_info(codec, data, data_sz, &si));
    }
}
#endif  // CONFIG_VP9_DECODER

}  // namespace

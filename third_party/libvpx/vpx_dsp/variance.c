﻿/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./vpx_config.h"
#include "./vpx_dsp_rtcd.h"

#include "vpx_ports/mem.h"
#include "vpx/vpx_integer.h"

#include "vpx_dsp/variance.h"

static const uint8_t bilinear_filters[8][2] =
{
    { 128, 0 }, { 112, 16 }, { 96, 32 }, { 80, 48 },
    { 64, 64 }, { 48, 80 },  { 32, 96 }, { 16, 112 },
};

uint32_t vpx_get4x4sse_cs_c(const uint8_t *a, int a_stride, const uint8_t *b,
                            int b_stride)
{
    int distortion = 0;
    int r, c;

    for (r = 0; r < 4; ++r)
    {
        for (c = 0; c < 4; ++c)
        {
            int diff = a[c] - b[c];
            distortion += diff * diff;
        }

        a += a_stride;
        b += b_stride;
    }

    return distortion;
}

uint32_t vpx_get_mb_ss_c(const int16_t *a)
{
    unsigned int i, sum = 0;

    for (i = 0; i < 256; ++i)
    {
        sum += a[i] * a[i];
    }

    return sum;
}

static void variance(const uint8_t *a, int a_stride, const uint8_t *b,
                     int b_stride, int w, int h, uint32_t *sse, int *sum)
{
    int i, j;

    *sum = 0;
    *sse = 0;

    for (i = 0; i < h; ++i)
    {
        for (j = 0; j < w; ++j)
        {
            const int diff = a[j] - b[j];
            *sum += diff;
            *sse += diff * diff;
        }

        a += a_stride;
        b += b_stride;
    }
}

// Applies a 1-D 2-tap bilinear filter to the source block in either horizontal
// or vertical direction to produce the filtered output block. Used to implement
// the first-pass of 2-D separable filter.
//
// Produces int16_t output to retain precision for the next pass. Two filter
// taps should sum to FILTER_WEIGHT. pixel_step defines whether the filter is
// applied horizontally (pixel_step = 1) or vertically (pixel_step = stride).
// It defines the offset required to move from one input to the next.
static void var_filter_block2d_bil_first_pass(const uint8_t *a, uint16_t *b,
        unsigned int src_pixels_per_line,
        int pixel_step,
        unsigned int output_height,
        unsigned int output_width,
        const uint8_t *filter)
{
    unsigned int i, j;

    for (i = 0; i < output_height; ++i)
    {
        for (j = 0; j < output_width; ++j)
        {
            b[j] = ROUND_POWER_OF_TWO(
                       (int)a[0] * filter[0] + (int)a[pixel_step] * filter[1], FILTER_BITS);

            ++a;
        }

        a += src_pixels_per_line - output_width;
        b += output_width;
    }
}

// Applies a 1-D 2-tap bilinear filter to the source block in either horizontal
// or vertical direction to produce the filtered output block. Used to implement
// the second-pass of 2-D separable filter.
//
// Requires 16-bit input as produced by filter_block2d_bil_first_pass. Two
// filter taps should sum to FILTER_WEIGHT. pixel_step defines whether the
// filter is applied horizontally (pixel_step = 1) or vertically
// (pixel_step = stride). It defines the offset required to move from one input
// to the next. Output is 8-bit.
static void var_filter_block2d_bil_second_pass(const uint16_t *a, uint8_t *b,
        unsigned int src_pixels_per_line,
        unsigned int pixel_step,
        unsigned int output_height,
        unsigned int output_width,
        const uint8_t *filter)
{
    unsigned int i, j;

    for (i = 0; i < output_height; ++i)
    {
        for (j = 0; j < output_width; ++j)
        {
            b[j] = ROUND_POWER_OF_TWO(
                       (int)a[0] * filter[0] + (int)a[pixel_step] * filter[1], FILTER_BITS);
            ++a;
        }

        a += src_pixels_per_line - output_width;
        b += output_width;
    }
}

#define VAR(W, H)                                                    \
  uint32_t vpx_variance##W##x##H##_c(const uint8_t *a, int a_stride, \
                                     const uint8_t *b, int b_stride, \
                                     uint32_t *sse) {                \
    int sum;                                                         \
    variance(a, a_stride, b, b_stride, W, H, sse, &sum);             \
    return *sse - (uint32_t)(((int64_t)sum * sum) / (W * H));        \
  }

#define SUBPIX_VAR(W, H)                                                \
  uint32_t vpx_sub_pixel_variance##W##x##H##_c(                         \
      const uint8_t *a, int a_stride, int xoffset, int yoffset,         \
      const uint8_t *b, int b_stride, uint32_t *sse) {                  \
    uint16_t fdata3[(H + 1) * W];                                       \
    uint8_t temp2[H * W];                                               \
                                                                        \
    var_filter_block2d_bil_first_pass(a, fdata3, a_stride, 1, H + 1, W, \
                                      bilinear_filters[xoffset]);       \
    var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,       \
                                       bilinear_filters[yoffset]);      \
                                                                        \
    return vpx_variance##W##x##H##_c(temp2, W, b, b_stride, sse);       \
  }

#define SUBPIX_AVG_VAR(W, H)                                            \
  uint32_t vpx_sub_pixel_avg_variance##W##x##H##_c(                     \
      const uint8_t *a, int a_stride, int xoffset, int yoffset,         \
      const uint8_t *b, int b_stride, uint32_t *sse,                    \
      const uint8_t *second_pred) {                                     \
    uint16_t fdata3[(H + 1) * W];                                       \
    uint8_t temp2[H * W];                                               \
    DECLARE_ALIGNED(16, uint8_t, temp3[H * W]);                         \
                                                                        \
    var_filter_block2d_bil_first_pass(a, fdata3, a_stride, 1, H + 1, W, \
                                      bilinear_filters[xoffset]);       \
    var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,       \
                                       bilinear_filters[yoffset]);      \
                                                                        \
    vpx_comp_avg_pred(temp3, second_pred, W, H, temp2, W);              \
                                                                        \
    return vpx_variance##W##x##H##_c(temp3, W, b, b_stride, sse);       \
  }

/* Identical to the variance call except it takes an additional parameter, sum,
 * and returns that value using pass-by-reference instead of returning
 * sse - sum^2 / w*h
 */
#define GET_VAR(W, H)                                                         \
  void vpx_get##W##x##H##var_c(const uint8_t *a, int a_stride,                \
                               const uint8_t *b, int b_stride, uint32_t *sse, \
                               int *sum) {                                    \
    variance(a, a_stride, b, b_stride, W, H, sse, sum);                       \
  }

/* Identical to the variance call except it does not calculate the
 * sse - sum^2 / w*h and returns sse in addtion to modifying the passed in
 * variable.
 */
#define MSE(W, H)                                               \
  uint32_t vpx_mse##W##x##H##_c(const uint8_t *a, int a_stride, \
                                const uint8_t *b, int b_stride, \
                                uint32_t *sse) {                \
    int sum;                                                    \
    variance(a, a_stride, b, b_stride, W, H, sse, &sum);        \
    return *sse;                                                \
  }

/* All three forms of the variance are available in the same sizes. */
#define VARIANCES(W, H) \
  VAR(W, H)             \
  SUBPIX_VAR(W, H)      \
  SUBPIX_AVG_VAR(W, H)

VARIANCES(64, 64)
VARIANCES(64, 32)
VARIANCES(32, 64)
VARIANCES(32, 32)
VARIANCES(32, 16)
VARIANCES(16, 32)
VARIANCES(16, 16)
VARIANCES(16, 8)
VARIANCES(8, 16)
VARIANCES(8, 8)
VARIANCES(8, 4)
VARIANCES(4, 8)
VARIANCES(4, 4)

GET_VAR(16, 16)
GET_VAR(8, 8)

MSE(16, 16)
MSE(16, 8)
MSE(8, 16)
MSE(8, 8)

void vpx_comp_avg_pred_c(uint8_t *comp_pred, const uint8_t *pred, int width,
                         int height, const uint8_t *ref, int ref_stride)
{
    int i, j;

    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            const int tmp = pred[j] + ref[j];
            comp_pred[j] = ROUND_POWER_OF_TWO(tmp, 1);
        }
        comp_pred += width;
        pred += width;
        ref += ref_stride;
    }
}

#if CONFIG_VP9_HIGHBITDEPTH
static void highbd_variance64(const uint8_t *a8, int a_stride,
                              const uint8_t *b8, int b_stride, int w, int h,
                              uint64_t *sse, int64_t *sum)
{
    int i, j;

    uint16_t *a = CONVERT_TO_SHORTPTR(a8);
    uint16_t *b = CONVERT_TO_SHORTPTR(b8);
    *sum = 0;
    *sse = 0;

    for (i = 0; i < h; ++i)
    {
        for (j = 0; j < w; ++j)
        {
            const int diff = a[j] - b[j];
            *sum += diff;
            *sse += diff * diff;
        }
        a += a_stride;
        b += b_stride;
    }
}

static void highbd_8_variance(const uint8_t *a8, int a_stride,
                              const uint8_t *b8, int b_stride, int w, int h,
                              uint32_t *sse, int *sum)
{
    uint64_t sse_long = 0;
    int64_t sum_long = 0;
    highbd_variance64(a8, a_stride, b8, b_stride, w, h, &sse_long, &sum_long);
    *sse = (uint32_t)sse_long;
    *sum = (int)sum_long;
}

static void highbd_10_variance(const uint8_t *a8, int a_stride,
                               const uint8_t *b8, int b_stride, int w, int h,
                               uint32_t *sse, int *sum)
{
    uint64_t sse_long = 0;
    int64_t sum_long = 0;
    highbd_variance64(a8, a_stride, b8, b_stride, w, h, &sse_long, &sum_long);
    *sse = (uint32_t)ROUND_POWER_OF_TWO(sse_long, 4);
    *sum = (int)ROUND_POWER_OF_TWO(sum_long, 2);
}

static void highbd_12_variance(const uint8_t *a8, int a_stride,
                               const uint8_t *b8, int b_stride, int w, int h,
                               uint32_t *sse, int *sum)
{
    uint64_t sse_long = 0;
    int64_t sum_long = 0;
    highbd_variance64(a8, a_stride, b8, b_stride, w, h, &sse_long, &sum_long);
    *sse = (uint32_t)ROUND_POWER_OF_TWO(sse_long, 8);
    *sum = (int)ROUND_POWER_OF_TWO(sum_long, 4);
}

#define HIGHBD_VAR(W, H)                                                       \
  uint32_t vpx_highbd_8_variance##W##x##H##_c(const uint8_t *a, int a_stride,  \
                                              const uint8_t *b, int b_stride,  \
                                              uint32_t *sse) {                 \
    int sum;                                                                   \
    highbd_8_variance(a, a_stride, b, b_stride, W, H, sse, &sum);              \
    return *sse - (((int64_t)sum * sum) / (W * H));                            \
  }                                                                            \
                                                                               \
  uint32_t vpx_highbd_10_variance##W##x##H##_c(const uint8_t *a, int a_stride, \
                                               const uint8_t *b, int b_stride, \
                                               uint32_t *sse) {                \
    int sum;                                                                   \
    int64_t var;                                                               \
    highbd_10_variance(a, a_stride, b, b_stride, W, H, sse, &sum);             \
    var = (int64_t)(*sse) - (((int64_t)sum * sum) / (W * H));                  \
    return (var >= 0) ? (uint32_t)var : 0;                                     \
  }                                                                            \
                                                                               \
  uint32_t vpx_highbd_12_variance##W##x##H##_c(const uint8_t *a, int a_stride, \
                                               const uint8_t *b, int b_stride, \
                                               uint32_t *sse) {                \
    int sum;                                                                   \
    int64_t var;                                                               \
    highbd_12_variance(a, a_stride, b, b_stride, W, H, sse, &sum);             \
    var = (int64_t)(*sse) - (((int64_t)sum * sum) / (W * H));                  \
    return (var >= 0) ? (uint32_t)var : 0;                                     \
  }

#define HIGHBD_GET_VAR(S)                                                    \
  void vpx_highbd_8_get##S##x##S##var_c(const uint8_t *src, int src_stride,  \
                                        const uint8_t *ref, int ref_stride,  \
                                        uint32_t *sse, int *sum) {           \
    highbd_8_variance(src, src_stride, ref, ref_stride, S, S, sse, sum);     \
  }                                                                          \
                                                                             \
  void vpx_highbd_10_get##S##x##S##var_c(const uint8_t *src, int src_stride, \
                                         const uint8_t *ref, int ref_stride, \
                                         uint32_t *sse, int *sum) {          \
    highbd_10_variance(src, src_stride, ref, ref_stride, S, S, sse, sum);    \
  }                                                                          \
                                                                             \
  void vpx_highbd_12_get##S##x##S##var_c(const uint8_t *src, int src_stride, \
                                         const uint8_t *ref, int ref_stride, \
                                         uint32_t *sse, int *sum) {          \
    highbd_12_variance(src, src_stride, ref, ref_stride, S, S, sse, sum);    \
  }

#define HIGHBD_MSE(W, H)                                                      \
  uint32_t vpx_highbd_8_mse##W##x##H##_c(const uint8_t *src, int src_stride,  \
                                         const uint8_t *ref, int ref_stride,  \
                                         uint32_t *sse) {                     \
    int sum;                                                                  \
    highbd_8_variance(src, src_stride, ref, ref_stride, W, H, sse, &sum);     \
    return *sse;                                                              \
  }                                                                           \
                                                                              \
  uint32_t vpx_highbd_10_mse##W##x##H##_c(const uint8_t *src, int src_stride, \
                                          const uint8_t *ref, int ref_stride, \
                                          uint32_t *sse) {                    \
    int sum;                                                                  \
    highbd_10_variance(src, src_stride, ref, ref_stride, W, H, sse, &sum);    \
    return *sse;                                                              \
  }                                                                           \
                                                                              \
  uint32_t vpx_highbd_12_mse##W##x##H##_c(const uint8_t *src, int src_stride, \
                                          const uint8_t *ref, int ref_stride, \
                                          uint32_t *sse) {                    \
    int sum;                                                                  \
    highbd_12_variance(src, src_stride, ref, ref_stride, W, H, sse, &sum);    \
    return *sse;                                                              \
  }

static void highbd_var_filter_block2d_bil_first_pass(
    const uint8_t *src_ptr8, uint16_t *output_ptr,
    unsigned int src_pixels_per_line, int pixel_step,
    unsigned int output_height, unsigned int output_width,
    const uint8_t *filter)
{
    unsigned int i, j;
    uint16_t *src_ptr = CONVERT_TO_SHORTPTR(src_ptr8);
    for (i = 0; i < output_height; ++i)
    {
        for (j = 0; j < output_width; ++j)
        {
            output_ptr[j] = ROUND_POWER_OF_TWO(
                                (int)src_ptr[0] * filter[0] + (int)src_ptr[pixel_step] * filter[1],
                                FILTER_BITS);

            ++src_ptr;
        }

        // Next row...
        src_ptr += src_pixels_per_line - output_width;
        output_ptr += output_width;
    }
}

static void highbd_var_filter_block2d_bil_second_pass(
    const uint16_t *src_ptr, uint16_t *output_ptr,
    unsigned int src_pixels_per_line, unsigned int pixel_step,
    unsigned int output_height, unsigned int output_width,
    const uint8_t *filter)
{
    unsigned int i, j;

    for (i = 0; i < output_height; ++i)
    {
        for (j = 0; j < output_width; ++j)
        {
            output_ptr[j] = ROUND_POWER_OF_TWO(
                                (int)src_ptr[0] * filter[0] + (int)src_ptr[pixel_step] * filter[1],
                                FILTER_BITS);
            ++src_ptr;
        }

        src_ptr += src_pixels_per_line - output_width;
        output_ptr += output_width;
    }
}

#define HIGHBD_SUBPIX_VAR(W, H)                                              \
  uint32_t vpx_highbd_8_sub_pixel_variance##W##x##H##_c(                     \
      const uint8_t *src, int src_stride, int xoffset, int yoffset,          \
      const uint8_t *dst, int dst_stride, uint32_t *sse) {                   \
    uint16_t fdata3[(H + 1) * W];                                            \
    uint16_t temp2[H * W];                                                   \
                                                                             \
    highbd_var_filter_block2d_bil_first_pass(                                \
        src, fdata3, src_stride, 1, H + 1, W, bilinear_filters[xoffset]);    \
    highbd_var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,     \
                                              bilinear_filters[yoffset]);    \
                                                                             \
    return vpx_highbd_8_variance##W##x##H##_c(CONVERT_TO_BYTEPTR(temp2), W,  \
                                              dst, dst_stride, sse);         \
  }                                                                          \
                                                                             \
  uint32_t vpx_highbd_10_sub_pixel_variance##W##x##H##_c(                    \
      const uint8_t *src, int src_stride, int xoffset, int yoffset,          \
      const uint8_t *dst, int dst_stride, uint32_t *sse) {                   \
    uint16_t fdata3[(H + 1) * W];                                            \
    uint16_t temp2[H * W];                                                   \
                                                                             \
    highbd_var_filter_block2d_bil_first_pass(                                \
        src, fdata3, src_stride, 1, H + 1, W, bilinear_filters[xoffset]);    \
    highbd_var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,     \
                                              bilinear_filters[yoffset]);    \
                                                                             \
    return vpx_highbd_10_variance##W##x##H##_c(CONVERT_TO_BYTEPTR(temp2), W, \
                                               dst, dst_stride, sse);        \
  }                                                                          \
                                                                             \
  uint32_t vpx_highbd_12_sub_pixel_variance##W##x##H##_c(                    \
      const uint8_t *src, int src_stride, int xoffset, int yoffset,          \
      const uint8_t *dst, int dst_stride, uint32_t *sse) {                   \
    uint16_t fdata3[(H + 1) * W];                                            \
    uint16_t temp2[H * W];                                                   \
                                                                             \
    highbd_var_filter_block2d_bil_first_pass(                                \
        src, fdata3, src_stride, 1, H + 1, W, bilinear_filters[xoffset]);    \
    highbd_var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,     \
                                              bilinear_filters[yoffset]);    \
                                                                             \
    return vpx_highbd_12_variance##W##x##H##_c(CONVERT_TO_BYTEPTR(temp2), W, \
                                               dst, dst_stride, sse);        \
  }

#define HIGHBD_SUBPIX_AVG_VAR(W, H)                                          \
  uint32_t vpx_highbd_8_sub_pixel_avg_variance##W##x##H##_c(                 \
      const uint8_t *src, int src_stride, int xoffset, int yoffset,          \
      const uint8_t *dst, int dst_stride, uint32_t *sse,                     \
      const uint8_t *second_pred) {                                          \
    uint16_t fdata3[(H + 1) * W];                                            \
    uint16_t temp2[H * W];                                                   \
    DECLARE_ALIGNED(16, uint16_t, temp3[H * W]);                             \
                                                                             \
    highbd_var_filter_block2d_bil_first_pass(                                \
        src, fdata3, src_stride, 1, H + 1, W, bilinear_filters[xoffset]);    \
    highbd_var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,     \
                                              bilinear_filters[yoffset]);    \
                                                                             \
    vpx_highbd_comp_avg_pred(temp3, second_pred, W, H,                       \
                             CONVERT_TO_BYTEPTR(temp2), W);                  \
                                                                             \
    return vpx_highbd_8_variance##W##x##H##_c(CONVERT_TO_BYTEPTR(temp3), W,  \
                                              dst, dst_stride, sse);         \
  }                                                                          \
                                                                             \
  uint32_t vpx_highbd_10_sub_pixel_avg_variance##W##x##H##_c(                \
      const uint8_t *src, int src_stride, int xoffset, int yoffset,          \
      const uint8_t *dst, int dst_stride, uint32_t *sse,                     \
      const uint8_t *second_pred) {                                          \
    uint16_t fdata3[(H + 1) * W];                                            \
    uint16_t temp2[H * W];                                                   \
    DECLARE_ALIGNED(16, uint16_t, temp3[H * W]);                             \
                                                                             \
    highbd_var_filter_block2d_bil_first_pass(                                \
        src, fdata3, src_stride, 1, H + 1, W, bilinear_filters[xoffset]);    \
    highbd_var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,     \
                                              bilinear_filters[yoffset]);    \
                                                                             \
    vpx_highbd_comp_avg_pred(temp3, second_pred, W, H,                       \
                             CONVERT_TO_BYTEPTR(temp2), W);                  \
                                                                             \
    return vpx_highbd_10_variance##W##x##H##_c(CONVERT_TO_BYTEPTR(temp3), W, \
                                               dst, dst_stride, sse);        \
  }                                                                          \
                                                                             \
  uint32_t vpx_highbd_12_sub_pixel_avg_variance##W##x##H##_c(                \
      const uint8_t *src, int src_stride, int xoffset, int yoffset,          \
      const uint8_t *dst, int dst_stride, uint32_t *sse,                     \
      const uint8_t *second_pred) {                                          \
    uint16_t fdata3[(H + 1) * W];                                            \
    uint16_t temp2[H * W];                                                   \
    DECLARE_ALIGNED(16, uint16_t, temp3[H * W]);                             \
                                                                             \
    highbd_var_filter_block2d_bil_first_pass(                                \
        src, fdata3, src_stride, 1, H + 1, W, bilinear_filters[xoffset]);    \
    highbd_var_filter_block2d_bil_second_pass(fdata3, temp2, W, W, H, W,     \
                                              bilinear_filters[yoffset]);    \
                                                                             \
    vpx_highbd_comp_avg_pred(temp3, second_pred, W, H,                       \
                             CONVERT_TO_BYTEPTR(temp2), W);                  \
                                                                             \
    return vpx_highbd_12_variance##W##x##H##_c(CONVERT_TO_BYTEPTR(temp3), W, \
                                               dst, dst_stride, sse);        \
  }

/* All three forms of the variance are available in the same sizes. */
#define HIGHBD_VARIANCES(W, H) \
  HIGHBD_VAR(W, H)             \
  HIGHBD_SUBPIX_VAR(W, H)      \
  HIGHBD_SUBPIX_AVG_VAR(W, H)

HIGHBD_VARIANCES(64, 64)
HIGHBD_VARIANCES(64, 32)
HIGHBD_VARIANCES(32, 64)
HIGHBD_VARIANCES(32, 32)
HIGHBD_VARIANCES(32, 16)
HIGHBD_VARIANCES(16, 32)
HIGHBD_VARIANCES(16, 16)
HIGHBD_VARIANCES(16, 8)
HIGHBD_VARIANCES(8, 16)
HIGHBD_VARIANCES(8, 8)
HIGHBD_VARIANCES(8, 4)
HIGHBD_VARIANCES(4, 8)
HIGHBD_VARIANCES(4, 4)

HIGHBD_GET_VAR(8)
HIGHBD_GET_VAR(16)

HIGHBD_MSE(16, 16)
HIGHBD_MSE(16, 8)
HIGHBD_MSE(8, 16)
HIGHBD_MSE(8, 8)

void vpx_highbd_comp_avg_pred(uint16_t *comp_pred, const uint8_t *pred8,
                              int width, int height, const uint8_t *ref8,
                              int ref_stride)
{
    int i, j;
    uint16_t *pred = CONVERT_TO_SHORTPTR(pred8);
    uint16_t *ref = CONVERT_TO_SHORTPTR(ref8);
    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            const int tmp = pred[j] + ref[j];
            comp_pred[j] = ROUND_POWER_OF_TWO(tmp, 1);
        }
        comp_pred += width;
        pred += width;
        ref += ref_stride;
    }
}
#endif  // CONFIG_VP9_HIGHBITDEPTH

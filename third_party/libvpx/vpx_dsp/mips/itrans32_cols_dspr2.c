﻿/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./vpx_config.h"
#include "vpx_dsp/mips/inv_txfm_dspr2.h"
#include "vpx_dsp/txfm_common.h"

#if HAVE_DSPR2
void vpx_idct32_cols_add_blk_dspr2(int16_t *input, uint8_t *dest, int stride)
{
    int step1_0, step1_1, step1_2, step1_3, step1_4, step1_5, step1_6;
    int step1_7, step1_8, step1_9, step1_10, step1_11, step1_12, step1_13;
    int step1_14, step1_15, step1_16, step1_17, step1_18, step1_19, step1_20;
    int step1_21, step1_22, step1_23, step1_24, step1_25, step1_26, step1_27;
    int step1_28, step1_29, step1_30, step1_31;
    int step2_0, step2_1, step2_2, step2_3, step2_4, step2_5, step2_6;
    int step2_7, step2_8, step2_9, step2_10, step2_11, step2_12, step2_13;
    int step2_14, step2_15, step2_16, step2_17, step2_18, step2_19, step2_20;
    int step2_21, step2_22, step2_23, step2_24, step2_25, step2_26, step2_27;
    int step2_28, step2_29, step2_30, step2_31;
    int step3_8, step3_9, step3_10, step3_11, step3_12, step3_13, step3_14;
    int step3_15, step3_16, step3_17, step3_18, step3_19, step3_20, step3_21;
    int step3_22, step3_23, step3_24, step3_25, step3_26, step3_27, step3_28;
    int step3_29, step3_30, step3_31;
    int temp0, temp1, temp2, temp3;
    int load1, load2, load3, load4;
    int result1, result2;
    int i;
    uint8_t *dest_pix, *dest_pix1;
    const int const_2_power_13 = 8192;
    uint8_t *cm = vpx_ff_cropTbl;

    /* prefetch vpx_ff_cropTbl */
    prefetch_load(vpx_ff_cropTbl);
    prefetch_load(vpx_ff_cropTbl + 32);
    prefetch_load(vpx_ff_cropTbl + 64);
    prefetch_load(vpx_ff_cropTbl + 96);
    prefetch_load(vpx_ff_cropTbl + 128);
    prefetch_load(vpx_ff_cropTbl + 160);
    prefetch_load(vpx_ff_cropTbl + 192);
    prefetch_load(vpx_ff_cropTbl + 224);

    for (i = 0; i < 32; ++i)
    {
        dest_pix = dest + i;
        dest_pix1 = dest + i + 31 * stride;

        __asm__ __volatile__(
            "lh       %[load1],             2(%[input])                     \n\t"
            "lh       %[load2],             62(%[input])                    \n\t"
            "lh       %[load3],             34(%[input])                    \n\t"
            "lh       %[load4],             30(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_31_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_1_64]   \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"

            "madd     $ac3,                 %[load1],       %[cospi_1_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_31_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_15_64]  \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_17_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"

            "madd     $ac1,                 %[load3],       %[cospi_17_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_15_64]  \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp3],       %[temp2]        \n\t"
            "sub      %[load2],             %[temp0],       %[temp1]        \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_28_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_4_64]   \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_4_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_28_64]  \n\t"

            "extp     %[step1_17],          $ac1,           31              \n\t"
            "extp     %[step1_30],          $ac3,           31              \n\t"
            "add      %[step1_16],          %[temp0],       %[temp1]        \n\t"
            "add      %[step1_31],          %[temp2],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3),
            [step1_16] "=&r"(step1_16), [step1_17] "=&r"(step1_17),
            [step1_30] "=&r"(step1_30), [step1_31] "=&r"(step1_31)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_31_64] "r"(cospi_31_64), [cospi_1_64] "r"(cospi_1_64),
            [cospi_4_64] "r"(cospi_4_64), [cospi_17_64] "r"(cospi_17_64),
            [cospi_15_64] "r"(cospi_15_64), [cospi_28_64] "r"(cospi_28_64));

        __asm__ __volatile__(
            "lh       %[load1],             18(%[input])                    \n\t"
            "lh       %[load2],             46(%[input])                    \n\t"
            "lh       %[load3],             50(%[input])                    \n\t"
            "lh       %[load4],             14(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_23_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_9_64]   \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"

            "madd     $ac3,                 %[load1],       %[cospi_9_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_23_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_7_64]   \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_25_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"

            "madd     $ac1,                 %[load3],       %[cospi_25_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_7_64]   \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp1],       %[temp0]        \n\t"
            "sub      %[load2],             %[temp2],       %[temp3]        \n\t"

            "msub     $ac1,                 %[load1],       %[cospi_28_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_4_64]   \n\t"
            "msub     $ac3,                 %[load1],       %[cospi_4_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_28_64]  \n\t"

            "extp     %[step1_18],          $ac1,           31              \n\t"
            "extp     %[step1_29],          $ac3,           31              \n\t"
            "add      %[step1_19],          %[temp0],       %[temp1]        \n\t"
            "add      %[step1_28],          %[temp2],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3),
            [step1_18] "=&r"(step1_18), [step1_19] "=&r"(step1_19),
            [step1_28] "=&r"(step1_28), [step1_29] "=&r"(step1_29)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_23_64] "r"(cospi_23_64), [cospi_9_64] "r"(cospi_9_64),
            [cospi_4_64] "r"(cospi_4_64), [cospi_7_64] "r"(cospi_7_64),
            [cospi_25_64] "r"(cospi_25_64), [cospi_28_64] "r"(cospi_28_64));

        __asm__ __volatile__(
            "lh       %[load1],             10(%[input])                    \n\t"
            "lh       %[load2],             54(%[input])                    \n\t"
            "lh       %[load3],             42(%[input])                    \n\t"
            "lh       %[load4],             22(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_27_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_5_64]   \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"

            "madd     $ac3,                 %[load1],       %[cospi_5_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_27_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_11_64]  \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_21_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"

            "madd     $ac1,                 %[load3],       %[cospi_21_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_11_64]  \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp0],       %[temp1]        \n\t"
            "sub      %[load2],             %[temp3],       %[temp2]        \n\t"

            "madd     $ac1,                 %[load2],       %[cospi_12_64]  \n\t"
            "msub     $ac1,                 %[load1],       %[cospi_20_64]  \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_12_64]  \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_20_64]  \n\t"

            "extp     %[step1_21],          $ac1,           31              \n\t"
            "extp     %[step1_26],          $ac3,           31              \n\t"
            "add      %[step1_20],          %[temp0],       %[temp1]        \n\t"
            "add      %[step1_27],          %[temp2],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3),
            [step1_20] "=&r"(step1_20), [step1_21] "=&r"(step1_21),
            [step1_26] "=&r"(step1_26), [step1_27] "=&r"(step1_27)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_27_64] "r"(cospi_27_64), [cospi_5_64] "r"(cospi_5_64),
            [cospi_11_64] "r"(cospi_11_64), [cospi_21_64] "r"(cospi_21_64),
            [cospi_12_64] "r"(cospi_12_64), [cospi_20_64] "r"(cospi_20_64));

        __asm__ __volatile__(
            "lh       %[load1],             26(%[input])                    \n\t"
            "lh       %[load2],             38(%[input])                    \n\t"
            "lh       %[load3],             58(%[input])                    \n\t"
            "lh       %[load4],              6(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_19_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_13_64]  \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_13_64]  \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_19_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_3_64]   \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_29_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"
            "madd     $ac1,                 %[load3],       %[cospi_29_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_3_64]   \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp1],       %[temp0]        \n\t"
            "sub      %[load2],             %[temp2],       %[temp3]        \n\t"
            "msub     $ac1,                 %[load1],       %[cospi_12_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_20_64]  \n\t"
            "msub     $ac3,                 %[load1],       %[cospi_20_64]  \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_12_64]  \n\t"
            "extp     %[step1_22],          $ac1,           31              \n\t"
            "extp     %[step1_25],          $ac3,           31              \n\t"
            "add      %[step1_23],          %[temp0],       %[temp1]        \n\t"
            "add      %[step1_24],          %[temp2],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3),
            [step1_22] "=&r"(step1_22), [step1_23] "=&r"(step1_23),
            [step1_24] "=&r"(step1_24), [step1_25] "=&r"(step1_25)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_19_64] "r"(cospi_19_64), [cospi_13_64] "r"(cospi_13_64),
            [cospi_3_64] "r"(cospi_3_64), [cospi_29_64] "r"(cospi_29_64),
            [cospi_12_64] "r"(cospi_12_64), [cospi_20_64] "r"(cospi_20_64));

        __asm__ __volatile__(
            "lh       %[load1],              4(%[input])                    \n\t"
            "lh       %[load2],             60(%[input])                    \n\t"
            "lh       %[load3],             36(%[input])                    \n\t"
            "lh       %[load4],             28(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_30_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_2_64]   \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_2_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_30_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_14_64]  \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_18_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"
            "madd     $ac1,                 %[load3],       %[cospi_18_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_14_64]  \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp0],       %[temp1]        \n\t"
            "sub      %[load2],             %[temp3],       %[temp2]        \n\t"
            "msub     $ac1,                 %[load1],       %[cospi_8_64]   \n\t"
            "madd     $ac1,                 %[load2],       %[cospi_24_64]  \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_24_64]  \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_8_64]   \n\t"
            "extp     %[step2_9],           $ac1,           31              \n\t"
            "extp     %[step2_14],          $ac3,           31              \n\t"
            "add      %[step2_8],           %[temp0],       %[temp1]        \n\t"
            "add      %[step2_15],          %[temp2],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3), [step2_8] "=&r"(step2_8),
            [step2_9] "=&r"(step2_9), [step2_14] "=&r"(step2_14),
            [step2_15] "=&r"(step2_15)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_30_64] "r"(cospi_30_64), [cospi_2_64] "r"(cospi_2_64),
            [cospi_14_64] "r"(cospi_14_64), [cospi_18_64] "r"(cospi_18_64),
            [cospi_8_64] "r"(cospi_8_64), [cospi_24_64] "r"(cospi_24_64));

        __asm__ __volatile__(
            "lh       %[load1],             20(%[input])                    \n\t"
            "lh       %[load2],             44(%[input])                    \n\t"
            "lh       %[load3],             52(%[input])                    \n\t"
            "lh       %[load4],             12(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_22_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_10_64]  \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_10_64]  \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_22_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_6_64]   \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_26_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"
            "madd     $ac1,                 %[load3],       %[cospi_26_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_6_64]   \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp1],       %[temp0]        \n\t"
            "sub      %[load2],             %[temp2],       %[temp3]        \n\t"
            "msub     $ac1,                 %[load1],       %[cospi_24_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_8_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_24_64]  \n\t"
            "msub     $ac3,                 %[load1],       %[cospi_8_64]   \n\t"
            "extp     %[step2_10],          $ac1,           31              \n\t"
            "extp     %[step2_13],          $ac3,           31              \n\t"
            "add      %[step2_11],          %[temp0],       %[temp1]        \n\t"
            "add      %[step2_12],          %[temp2],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3),
            [step2_10] "=&r"(step2_10), [step2_11] "=&r"(step2_11),
            [step2_12] "=&r"(step2_12), [step2_13] "=&r"(step2_13)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_22_64] "r"(cospi_22_64), [cospi_10_64] "r"(cospi_10_64),
            [cospi_6_64] "r"(cospi_6_64), [cospi_26_64] "r"(cospi_26_64),
            [cospi_8_64] "r"(cospi_8_64), [cospi_24_64] "r"(cospi_24_64));

        __asm__ __volatile__(
            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "sub      %[temp0],             %[step2_14],    %[step2_13]     \n\t"
            "sub      %[temp0],             %[temp0],       %[step2_9]      \n\t"
            "add      %[temp0],             %[temp0],       %[step2_10]     \n\t"
            "madd     $ac0,                 %[temp0],       %[cospi_16_64]  \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "sub      %[temp1],             %[step2_14],    %[step2_13]     \n\t"
            "add      %[temp1],             %[temp1],       %[step2_9]      \n\t"
            "sub      %[temp1],             %[temp1],       %[step2_10]     \n\t"
            "madd     $ac1,                 %[temp1],       %[cospi_16_64]  \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"
            "sub      %[temp0],             %[step2_15],    %[step2_12]     \n\t"
            "sub      %[temp0],             %[temp0],       %[step2_8]      \n\t"
            "add      %[temp0],             %[temp0],       %[step2_11]     \n\t"
            "madd     $ac2,                 %[temp0],       %[cospi_16_64]  \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"
            "sub      %[temp1],             %[step2_15],    %[step2_12]     \n\t"
            "add      %[temp1],             %[temp1],       %[step2_8]      \n\t"
            "sub      %[temp1],             %[temp1],       %[step2_11]     \n\t"
            "madd     $ac3,                 %[temp1],       %[cospi_16_64]  \n\t"

            "add      %[step3_8],           %[step2_8],     %[step2_11]     \n\t"
            "add      %[step3_9],           %[step2_9],     %[step2_10]     \n\t"
            "add      %[step3_14],          %[step2_13],    %[step2_14]     \n\t"
            "add      %[step3_15],          %[step2_12],    %[step2_15]     \n\t"
            "extp     %[step3_10],          $ac0,           31              \n\t"
            "extp     %[step3_13],          $ac1,           31              \n\t"
            "extp     %[step3_11],          $ac2,           31              \n\t"
            "extp     %[step3_12],          $ac3,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [step3_8] "=&r"(step3_8),
            [step3_9] "=&r"(step3_9), [step3_10] "=&r"(step3_10),
            [step3_11] "=&r"(step3_11), [step3_12] "=&r"(step3_12),
            [step3_13] "=&r"(step3_13), [step3_14] "=&r"(step3_14),
            [step3_15] "=&r"(step3_15)
            : [const_2_power_13] "r"(const_2_power_13), [step2_8] "r"(step2_8),
            [step2_9] "r"(step2_9), [step2_10] "r"(step2_10),
            [step2_11] "r"(step2_11), [step2_12] "r"(step2_12),
            [step2_13] "r"(step2_13), [step2_14] "r"(step2_14),
            [step2_15] "r"(step2_15), [cospi_16_64] "r"(cospi_16_64));

        __asm__ __volatile__(
            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "sub      %[temp0],             %[step1_17],    %[step1_18]     \n\t"
            "sub      %[temp1],             %[step1_30],    %[step1_29]     \n\t"
            "add      %[step3_17],          %[step1_17],    %[step1_18]     \n\t"
            "add      %[step3_30],          %[step1_30],    %[step1_29]     \n\t"

            "msub     $ac0,                 %[temp0],       %[cospi_8_64]   \n\t"
            "madd     $ac0,                 %[temp1],       %[cospi_24_64]  \n\t"
            "extp     %[step3_18],          $ac0,           31              \n\t"
            "madd     $ac1,                 %[temp0],       %[cospi_24_64]  \n\t"
            "madd     $ac1,                 %[temp1],       %[cospi_8_64]   \n\t"
            "extp     %[step3_29],          $ac1,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [step3_18] "=&r"(step3_18), [step3_29] "=&r"(step3_29),
            [step3_17] "=&r"(step3_17), [step3_30] "=&r"(step3_30)
            : [const_2_power_13] "r"(const_2_power_13), [step1_17] "r"(step1_17),
            [step1_18] "r"(step1_18), [step1_30] "r"(step1_30),
            [step1_29] "r"(step1_29), [cospi_24_64] "r"(cospi_24_64),
            [cospi_8_64] "r"(cospi_8_64));

        __asm__ __volatile__(
            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "sub      %[temp0],             %[step1_16],    %[step1_19]     \n\t"
            "sub      %[temp1],             %[step1_31],    %[step1_28]     \n\t"
            "add      %[step3_16],          %[step1_16],    %[step1_19]     \n\t"
            "add      %[step3_31],          %[step1_31],    %[step1_28]     \n\t"

            "msub     $ac0,                 %[temp0],       %[cospi_8_64]   \n\t"
            "madd     $ac0,                 %[temp1],       %[cospi_24_64]  \n\t"
            "extp     %[step3_19],          $ac0,           31              \n\t"
            "madd     $ac1,                 %[temp0],       %[cospi_24_64]  \n\t"
            "madd     $ac1,                 %[temp1],       %[cospi_8_64]   \n\t"
            "extp     %[step3_28],          $ac1,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [step3_16] "=&r"(step3_16), [step3_31] "=&r"(step3_31),
            [step3_19] "=&r"(step3_19), [step3_28] "=&r"(step3_28)
            : [const_2_power_13] "r"(const_2_power_13), [step1_16] "r"(step1_16),
            [step1_19] "r"(step1_19), [step1_31] "r"(step1_31),
            [step1_28] "r"(step1_28), [cospi_24_64] "r"(cospi_24_64),
            [cospi_8_64] "r"(cospi_8_64));

        __asm__ __volatile__(
            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "sub      %[temp0],             %[step1_23],    %[step1_20]     \n\t"
            "sub      %[temp1],             %[step1_24],    %[step1_27]     \n\t"
            "add      %[step3_23],          %[step1_23],    %[step1_20]     \n\t"
            "add      %[step3_24],          %[step1_24],    %[step1_27]     \n\t"

            "msub     $ac0,                 %[temp0],       %[cospi_8_64]   \n\t"
            "madd     $ac0,                 %[temp1],       %[cospi_24_64]  \n\t"
            "extp     %[step3_27],          $ac0,           31              \n\t"
            "msub     $ac1,                 %[temp0],       %[cospi_24_64]  \n\t"
            "msub     $ac1,                 %[temp1],       %[cospi_8_64]   \n\t"
            "extp     %[step3_20],          $ac1,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [step3_23] "=&r"(step3_23), [step3_24] "=&r"(step3_24),
            [step3_20] "=&r"(step3_20), [step3_27] "=&r"(step3_27)
            : [const_2_power_13] "r"(const_2_power_13), [step1_23] "r"(step1_23),
            [step1_20] "r"(step1_20), [step1_24] "r"(step1_24),
            [step1_27] "r"(step1_27), [cospi_24_64] "r"(cospi_24_64),
            [cospi_8_64] "r"(cospi_8_64));

        __asm__ __volatile__(
            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "sub      %[temp0],             %[step1_22],    %[step1_21]     \n\t"
            "sub      %[temp1],             %[step1_25],    %[step1_26]     \n\t"
            "add      %[step3_22],          %[step1_22],    %[step1_21]     \n\t"
            "add      %[step3_25],          %[step1_25],    %[step1_26]     \n\t"

            "msub     $ac0,                 %[temp0],       %[cospi_24_64]  \n\t"
            "msub     $ac0,                 %[temp1],       %[cospi_8_64]   \n\t"
            "extp     %[step3_21],          $ac0,           31              \n\t"
            "msub     $ac1,                 %[temp0],       %[cospi_8_64]   \n\t"
            "madd     $ac1,                 %[temp1],       %[cospi_24_64]  \n\t"
            "extp     %[step3_26],          $ac1,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [step3_22] "=&r"(step3_22), [step3_25] "=&r"(step3_25),
            [step3_21] "=&r"(step3_21), [step3_26] "=&r"(step3_26)
            : [const_2_power_13] "r"(const_2_power_13), [step1_22] "r"(step1_22),
            [step1_21] "r"(step1_21), [step1_25] "r"(step1_25),
            [step1_26] "r"(step1_26), [cospi_24_64] "r"(cospi_24_64),
            [cospi_8_64] "r"(cospi_8_64));

        __asm__ __volatile__(
            "add      %[step2_16],          %[step3_16],    %[step3_23]     \n\t"
            "add      %[step2_17],          %[step3_17],    %[step3_22]     \n\t"
            "add      %[step2_18],          %[step3_18],    %[step3_21]     \n\t"
            "add      %[step2_19],          %[step3_19],    %[step3_20]     \n\t"
            "sub      %[step2_20],          %[step3_19],    %[step3_20]     \n\t"
            "sub      %[step2_21],          %[step3_18],    %[step3_21]     \n\t"
            "sub      %[step2_22],          %[step3_17],    %[step3_22]     \n\t"
            "sub      %[step2_23],          %[step3_16],    %[step3_23]     \n\t"

            : [step2_16] "=&r"(step2_16), [step2_17] "=&r"(step2_17),
            [step2_18] "=&r"(step2_18), [step2_19] "=&r"(step2_19),
            [step2_20] "=&r"(step2_20), [step2_21] "=&r"(step2_21),
            [step2_22] "=&r"(step2_22), [step2_23] "=&r"(step2_23)
            : [step3_16] "r"(step3_16), [step3_23] "r"(step3_23),
            [step3_17] "r"(step3_17), [step3_22] "r"(step3_22),
            [step3_18] "r"(step3_18), [step3_21] "r"(step3_21),
            [step3_19] "r"(step3_19), [step3_20] "r"(step3_20));

        __asm__ __volatile__(
            "sub      %[step2_24],          %[step3_31],    %[step3_24]     \n\t"
            "sub      %[step2_25],          %[step3_30],    %[step3_25]     \n\t"
            "sub      %[step2_26],          %[step3_29],    %[step3_26]     \n\t"
            "sub      %[step2_27],          %[step3_28],    %[step3_27]     \n\t"
            "add      %[step2_28],          %[step3_28],    %[step3_27]     \n\t"
            "add      %[step2_29],          %[step3_29],    %[step3_26]     \n\t"
            "add      %[step2_30],          %[step3_30],    %[step3_25]     \n\t"
            "add      %[step2_31],          %[step3_31],    %[step3_24]     \n\t"

            : [step2_24] "=&r"(step2_24), [step2_28] "=&r"(step2_28),
            [step2_25] "=&r"(step2_25), [step2_29] "=&r"(step2_29),
            [step2_26] "=&r"(step2_26), [step2_30] "=&r"(step2_30),
            [step2_27] "=&r"(step2_27), [step2_31] "=&r"(step2_31)
            : [step3_31] "r"(step3_31), [step3_24] "r"(step3_24),
            [step3_30] "r"(step3_30), [step3_25] "r"(step3_25),
            [step3_29] "r"(step3_29), [step3_26] "r"(step3_26),
            [step3_28] "r"(step3_28), [step3_27] "r"(step3_27));

        __asm__ __volatile__(
            "lh       %[load1],             0(%[input])                     \n\t"
            "lh       %[load2],             32(%[input])                    \n\t"
            "lh       %[load3],             16(%[input])                    \n\t"
            "lh       %[load4],             48(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"
            "add      %[result1],           %[load1],       %[load2]        \n\t"
            "sub      %[result2],           %[load1],       %[load2]        \n\t"
            "madd     $ac1,                 %[result1],     %[cospi_16_64]  \n\t"
            "madd     $ac2,                 %[result2],     %[cospi_16_64]  \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"
            "madd     $ac3,                 %[load3],       %[cospi_24_64]  \n\t"
            "msub     $ac3,                 %[load4],       %[cospi_8_64]   \n\t"
            "extp     %[temp2],             $ac3,           31              \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "madd     $ac1,                 %[load3],       %[cospi_8_64]   \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_24_64]  \n\t"
            "extp     %[temp3],             $ac1,           31              \n\t"
            "add      %[step1_0],           %[temp0],       %[temp3]        \n\t"
            "add      %[step1_1],           %[temp1],       %[temp2]        \n\t"
            "sub      %[step1_2],           %[temp1],       %[temp2]        \n\t"
            "sub      %[step1_3],           %[temp0],       %[temp3]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [result1] "=&r"(result1),
            [result2] "=&r"(result2), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3), [step1_0] "=&r"(step1_0),
            [step1_1] "=&r"(step1_1), [step1_2] "=&r"(step1_2),
            [step1_3] "=&r"(step1_3)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_24_64] "r"(cospi_24_64), [cospi_8_64] "r"(cospi_8_64),
            [cospi_16_64] "r"(cospi_16_64));

        __asm__ __volatile__(
            "lh       %[load1],             8(%[input])                     \n\t"
            "lh       %[load2],             56(%[input])                    \n\t"
            "lh       %[load3],             40(%[input])                    \n\t"
            "lh       %[load4],             24(%[input])                    \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac1,                 %[load1],       %[cospi_28_64]  \n\t"
            "msub     $ac1,                 %[load2],       %[cospi_4_64]   \n\t"
            "extp     %[temp0],             $ac1,           31              \n\t"
            "madd     $ac3,                 %[load1],       %[cospi_4_64]   \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_28_64]  \n\t"
            "extp     %[temp3],             $ac3,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"

            "madd     $ac2,                 %[load3],       %[cospi_12_64]  \n\t"
            "msub     $ac2,                 %[load4],       %[cospi_20_64]  \n\t"
            "extp     %[temp1],             $ac2,           31              \n\t"
            "madd     $ac1,                 %[load3],       %[cospi_20_64]  \n\t"
            "madd     $ac1,                 %[load4],       %[cospi_12_64]  \n\t"
            "extp     %[temp2],             $ac1,           31              \n\t"

            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "sub      %[load1],             %[temp3],       %[temp2]        \n\t"
            "sub      %[load1],             %[load1],       %[temp0]        \n\t"
            "add      %[load1],             %[load1],       %[temp1]        \n\t"
            "sub      %[load2],             %[temp0],       %[temp1]        \n\t"
            "sub      %[load2],             %[load2],       %[temp2]        \n\t"
            "add      %[load2],             %[load2],       %[temp3]        \n\t"
            "madd     $ac1,                 %[load1],       %[cospi_16_64]  \n\t"
            "madd     $ac3,                 %[load2],       %[cospi_16_64]  \n\t"

            "extp     %[step1_5],           $ac1,           31              \n\t"
            "extp     %[step1_6],           $ac3,           31              \n\t"
            "add      %[step1_4],           %[temp0],       %[temp1]        \n\t"
            "add      %[step1_7],           %[temp3],       %[temp2]        \n\t"

            : [load1] "=&r"(load1), [load2] "=&r"(load2), [load3] "=&r"(load3),
            [load4] "=&r"(load4), [temp0] "=&r"(temp0), [temp1] "=&r"(temp1),
            [temp2] "=&r"(temp2), [temp3] "=&r"(temp3), [step1_4] "=&r"(step1_4),
            [step1_5] "=&r"(step1_5), [step1_6] "=&r"(step1_6),
            [step1_7] "=&r"(step1_7)
            : [const_2_power_13] "r"(const_2_power_13), [input] "r"(input),
            [cospi_20_64] "r"(cospi_20_64), [cospi_12_64] "r"(cospi_12_64),
            [cospi_4_64] "r"(cospi_4_64), [cospi_28_64] "r"(cospi_28_64),
            [cospi_16_64] "r"(cospi_16_64));

        __asm__ __volatile__(
            "add      %[step2_0],          %[step1_0],    %[step1_7]     \n\t"
            "add      %[step2_1],          %[step1_1],    %[step1_6]     \n\t"
            "add      %[step2_2],          %[step1_2],    %[step1_5]     \n\t"
            "add      %[step2_3],          %[step1_3],    %[step1_4]     \n\t"
            "sub      %[step2_4],          %[step1_3],    %[step1_4]     \n\t"
            "sub      %[step2_5],          %[step1_2],    %[step1_5]     \n\t"
            "sub      %[step2_6],          %[step1_1],    %[step1_6]     \n\t"
            "sub      %[step2_7],          %[step1_0],    %[step1_7]     \n\t"

            : [step2_0] "=&r"(step2_0), [step2_4] "=&r"(step2_4),
            [step2_1] "=&r"(step2_1), [step2_5] "=&r"(step2_5),
            [step2_2] "=&r"(step2_2), [step2_6] "=&r"(step2_6),
            [step2_3] "=&r"(step2_3), [step2_7] "=&r"(step2_7)
            : [step1_0] "r"(step1_0), [step1_7] "r"(step1_7),
            [step1_1] "r"(step1_1), [step1_6] "r"(step1_6),
            [step1_2] "r"(step1_2), [step1_5] "r"(step1_5),
            [step1_3] "r"(step1_3), [step1_4] "r"(step1_4));

        // stage 7
        __asm__ __volatile__(
            "add      %[step1_0],          %[step2_0],    %[step3_15]     \n\t"
            "add      %[step1_1],          %[step2_1],    %[step3_14]     \n\t"
            "add      %[step1_2],          %[step2_2],    %[step3_13]     \n\t"
            "add      %[step1_3],          %[step2_3],    %[step3_12]     \n\t"
            "sub      %[step1_12],         %[step2_3],    %[step3_12]     \n\t"
            "sub      %[step1_13],         %[step2_2],    %[step3_13]     \n\t"
            "sub      %[step1_14],         %[step2_1],    %[step3_14]     \n\t"
            "sub      %[step1_15],         %[step2_0],    %[step3_15]     \n\t"

            : [step1_0] "=&r"(step1_0), [step1_12] "=&r"(step1_12),
            [step1_1] "=&r"(step1_1), [step1_13] "=&r"(step1_13),
            [step1_2] "=&r"(step1_2), [step1_14] "=&r"(step1_14),
            [step1_3] "=&r"(step1_3), [step1_15] "=&r"(step1_15)
            : [step2_0] "r"(step2_0), [step3_15] "r"(step3_15),
            [step2_1] "r"(step2_1), [step3_14] "r"(step3_14),
            [step2_2] "r"(step2_2), [step3_13] "r"(step3_13),
            [step2_3] "r"(step2_3), [step3_12] "r"(step3_12));

        __asm__ __volatile__(
            "add      %[step1_4],          %[step2_4],    %[step3_11]     \n\t"
            "add      %[step1_5],          %[step2_5],    %[step3_10]     \n\t"
            "add      %[step1_6],          %[step2_6],    %[step3_9]      \n\t"
            "add      %[step1_7],          %[step2_7],    %[step3_8]      \n\t"
            "sub      %[step1_8],          %[step2_7],    %[step3_8]      \n\t"
            "sub      %[step1_9],          %[step2_6],    %[step3_9]      \n\t"
            "sub      %[step1_10],         %[step2_5],    %[step3_10]     \n\t"
            "sub      %[step1_11],         %[step2_4],    %[step3_11]     \n\t"

            : [step1_4] "=&r"(step1_4), [step1_8] "=&r"(step1_8),
            [step1_5] "=&r"(step1_5), [step1_9] "=&r"(step1_9),
            [step1_6] "=&r"(step1_6), [step1_10] "=&r"(step1_10),
            [step1_7] "=&r"(step1_7), [step1_11] "=&r"(step1_11)
            : [step2_4] "r"(step2_4), [step3_11] "r"(step3_11),
            [step2_5] "r"(step2_5), [step3_10] "r"(step3_10),
            [step2_6] "r"(step2_6), [step3_9] "r"(step3_9),
            [step2_7] "r"(step2_7), [step3_8] "r"(step3_8));

        __asm__ __volatile__(
            "sub      %[temp0],             %[step2_27],    %[step2_20]     \n\t"
            "add      %[temp1],             %[step2_27],    %[step2_20]     \n\t"
            "sub      %[temp2],             %[step2_26],    %[step2_21]     \n\t"
            "add      %[temp3],             %[step2_26],    %[step2_21]     \n\t"

            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac0,                 %[temp0],       %[cospi_16_64]  \n\t"
            "madd     $ac1,                 %[temp1],       %[cospi_16_64]  \n\t"
            "madd     $ac2,                 %[temp2],       %[cospi_16_64]  \n\t"
            "madd     $ac3,                 %[temp3],       %[cospi_16_64]  \n\t"

            "extp     %[step1_20],          $ac0,           31              \n\t"
            "extp     %[step1_27],          $ac1,           31              \n\t"
            "extp     %[step1_21],          $ac2,           31              \n\t"
            "extp     %[step1_26],          $ac3,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [step1_20] "=&r"(step1_20),
            [step1_27] "=&r"(step1_27), [step1_21] "=&r"(step1_21),
            [step1_26] "=&r"(step1_26)
            : [const_2_power_13] "r"(const_2_power_13), [step2_20] "r"(step2_20),
            [step2_27] "r"(step2_27), [step2_21] "r"(step2_21),
            [step2_26] "r"(step2_26), [cospi_16_64] "r"(cospi_16_64));

        __asm__ __volatile__(
            "sub      %[temp0],             %[step2_25],    %[step2_22]     \n\t"
            "add      %[temp1],             %[step2_25],    %[step2_22]     \n\t"
            "sub      %[temp2],             %[step2_24],    %[step2_23]     \n\t"
            "add      %[temp3],             %[step2_24],    %[step2_23]     \n\t"

            "mtlo     %[const_2_power_13],  $ac0                            \n\t"
            "mthi     $zero,                $ac0                            \n\t"
            "mtlo     %[const_2_power_13],  $ac1                            \n\t"
            "mthi     $zero,                $ac1                            \n\t"
            "mtlo     %[const_2_power_13],  $ac2                            \n\t"
            "mthi     $zero,                $ac2                            \n\t"
            "mtlo     %[const_2_power_13],  $ac3                            \n\t"
            "mthi     $zero,                $ac3                            \n\t"

            "madd     $ac0,                 %[temp0],       %[cospi_16_64]  \n\t"
            "madd     $ac1,                 %[temp1],       %[cospi_16_64]  \n\t"
            "madd     $ac2,                 %[temp2],       %[cospi_16_64]  \n\t"
            "madd     $ac3,                 %[temp3],       %[cospi_16_64]  \n\t"

            "extp     %[step1_22],          $ac0,           31              \n\t"
            "extp     %[step1_25],          $ac1,           31              \n\t"
            "extp     %[step1_23],          $ac2,           31              \n\t"
            "extp     %[step1_24],          $ac3,           31              \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [step1_22] "=&r"(step1_22),
            [step1_25] "=&r"(step1_25), [step1_23] "=&r"(step1_23),
            [step1_24] "=&r"(step1_24)
            : [const_2_power_13] "r"(const_2_power_13), [step2_22] "r"(step2_22),
            [step2_25] "r"(step2_25), [step2_23] "r"(step2_23),
            [step2_24] "r"(step2_24), [cospi_16_64] "r"(cospi_16_64));

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_0],         %[step2_31]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_1],         %[step2_30]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_2],         %[step2_29]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_3],         %[step2_28]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix] "+r"(dest_pix)
            : [cm] "r"(cm), [stride] "r"(stride), [step1_0] "r"(step1_0),
            [step1_1] "r"(step1_1), [step1_2] "r"(step1_2),
            [step1_3] "r"(step1_3), [step2_28] "r"(step2_28),
            [step2_29] "r"(step2_29), [step2_30] "r"(step2_30),
            [step2_31] "r"(step2_31));

        step3_12 = ROUND_POWER_OF_TWO((step1_3 - step2_28), 6);
        step3_13 = ROUND_POWER_OF_TWO((step1_2 - step2_29), 6);
        step3_14 = ROUND_POWER_OF_TWO((step1_1 - step2_30), 6);
        step3_15 = ROUND_POWER_OF_TWO((step1_0 - step2_31), 6);

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_15]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_14]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_13]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_12]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix1] "+r"(dest_pix1)
            : [cm] "r"(cm), [stride] "r"(stride), [step3_12] "r"(step3_12),
            [step3_13] "r"(step3_13), [step3_14] "r"(step3_14),
            [step3_15] "r"(step3_15));

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_4],         %[step1_27]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_5],         %[step1_26]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_6],         %[step1_25]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_7],         %[step1_24]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix] "+r"(dest_pix)
            : [cm] "r"(cm), [stride] "r"(stride), [step1_4] "r"(step1_4),
            [step1_5] "r"(step1_5), [step1_6] "r"(step1_6),
            [step1_7] "r"(step1_7), [step1_24] "r"(step1_24),
            [step1_25] "r"(step1_25), [step1_26] "r"(step1_26),
            [step1_27] "r"(step1_27));

        step3_12 = ROUND_POWER_OF_TWO((step1_7 - step1_24), 6);
        step3_13 = ROUND_POWER_OF_TWO((step1_6 - step1_25), 6);
        step3_14 = ROUND_POWER_OF_TWO((step1_5 - step1_26), 6);
        step3_15 = ROUND_POWER_OF_TWO((step1_4 - step1_27), 6);

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_15]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_14]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_13]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_12]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix1] "+r"(dest_pix1)
            : [cm] "r"(cm), [stride] "r"(stride), [step3_12] "r"(step3_12),
            [step3_13] "r"(step3_13), [step3_14] "r"(step3_14),
            [step3_15] "r"(step3_15));

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_8],         %[step1_23]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_9],         %[step1_22]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_10],        %[step1_21]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_11],        %[step1_20]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix] "+r"(dest_pix)
            : [cm] "r"(cm), [stride] "r"(stride), [step1_8] "r"(step1_8),
            [step1_9] "r"(step1_9), [step1_10] "r"(step1_10),
            [step1_11] "r"(step1_11), [step1_20] "r"(step1_20),
            [step1_21] "r"(step1_21), [step1_22] "r"(step1_22),
            [step1_23] "r"(step1_23));

        step3_12 = ROUND_POWER_OF_TWO((step1_11 - step1_20), 6);
        step3_13 = ROUND_POWER_OF_TWO((step1_10 - step1_21), 6);
        step3_14 = ROUND_POWER_OF_TWO((step1_9 - step1_22), 6);
        step3_15 = ROUND_POWER_OF_TWO((step1_8 - step1_23), 6);

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_15]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_14]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_13]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_12]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix1] "+r"(dest_pix1)
            : [cm] "r"(cm), [stride] "r"(stride), [step3_12] "r"(step3_12),
            [step3_13] "r"(step3_13), [step3_14] "r"(step3_14),
            [step3_15] "r"(step3_15));

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_12],        %[step2_19]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_13],        %[step2_18]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix])                      \n\t"
            "add      %[temp0],         %[step1_14],        %[step2_17]     \n\t"
            "addi     %[temp0],         %[temp0],           32              \n\t"
            "sra      %[temp0],         %[temp0],           6               \n\t"
            "add      %[temp2],         %[temp2],           %[temp0]        \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "add      %[temp1],         %[step1_15],        %[step2_16]     \n\t"
            "sb       %[temp0],         0(%[dest_pix])                      \n\t"
            "addu     %[dest_pix],      %[dest_pix],        %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix])                      \n\t"
            "addi     %[temp1],         %[temp1],           32              \n\t"
            "sra      %[temp1],         %[temp1],           6               \n\t"
            "add      %[temp3],         %[temp3],           %[temp1]        \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix])                      \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix] "+r"(dest_pix)
            : [cm] "r"(cm), [stride] "r"(stride), [step1_12] "r"(step1_12),
            [step1_13] "r"(step1_13), [step1_14] "r"(step1_14),
            [step1_15] "r"(step1_15), [step2_16] "r"(step2_16),
            [step2_17] "r"(step2_17), [step2_18] "r"(step2_18),
            [step2_19] "r"(step2_19));

        step3_12 = ROUND_POWER_OF_TWO((step1_15 - step2_16), 6);
        step3_13 = ROUND_POWER_OF_TWO((step1_14 - step2_17), 6);
        step3_14 = ROUND_POWER_OF_TWO((step1_13 - step2_18), 6);
        step3_15 = ROUND_POWER_OF_TWO((step1_12 - step2_19), 6);

        __asm__ __volatile__(
            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_15]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_14]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"

            "lbu      %[temp2],         0(%[dest_pix1])                     \n\t"
            "add      %[temp2],         %[temp2],           %[step3_13]     \n\t"
            "lbux     %[temp0],         %[temp2](%[cm])                     \n\t"
            "sb       %[temp0],         0(%[dest_pix1])                     \n\t"
            "subu     %[dest_pix1],     %[dest_pix1],       %[stride]       \n\t"
            "lbu      %[temp3],         0(%[dest_pix1])                     \n\t"
            "add      %[temp3],         %[temp3],           %[step3_12]     \n\t"
            "lbux     %[temp1],         %[temp3](%[cm])                     \n\t"
            "sb       %[temp1],         0(%[dest_pix1])                     \n\t"

            : [temp0] "=&r"(temp0), [temp1] "=&r"(temp1), [temp2] "=&r"(temp2),
            [temp3] "=&r"(temp3), [dest_pix1] "+r"(dest_pix1)
            : [cm] "r"(cm), [stride] "r"(stride), [step3_12] "r"(step3_12),
            [step3_13] "r"(step3_13), [step3_14] "r"(step3_14),
            [step3_15] "r"(step3_15));

        input += 32;
    }
}
#endif  // #if HAVE_DSPR2

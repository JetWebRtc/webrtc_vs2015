﻿/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"
#include "common.h"
#include "pixelutils.h"
#include "internal.h"

#if CONFIG_PIXELUTILS

#include "x86/pixelutils.h"

static av_always_inline int sad_wxh(const uint8_t *src1, ptrdiff_t stride1,
                                    const uint8_t *src2, ptrdiff_t stride2,
                                    int w, int h)
{
    int x, y, sum = 0;

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
            sum += abs(src1[x] - src2[x]);
        src1 += stride1;
        src2 += stride2;
    }
    return sum;
}

#define DECLARE_BLOCK_FUNCTIONS(size)                                               \
static int block_sad_##size##x##size##_c(const uint8_t *src1, ptrdiff_t stride1,    \
                                         const uint8_t *src2, ptrdiff_t stride2)    \
{                                                                                   \
    return sad_wxh(src1, stride1, src2, stride2, size, size);                       \
}

DECLARE_BLOCK_FUNCTIONS(2)
DECLARE_BLOCK_FUNCTIONS(4)
DECLARE_BLOCK_FUNCTIONS(8)
DECLARE_BLOCK_FUNCTIONS(16)

static const av_pixelutils_sad_fn sad_c[] =
{
    block_sad_2x2_c,
    block_sad_4x4_c,
    block_sad_8x8_c,
    block_sad_16x16_c,
};

#endif /* CONFIG_PIXELUTILS */

av_pixelutils_sad_fn av_pixelutils_get_sad_fn(int w_bits, int h_bits, int aligned, void *log_ctx)
{
#if !CONFIG_PIXELUTILS
    av_log(log_ctx, AV_LOG_ERROR, "pixelutils support is required "
           "but libavutil is not compiled with it\n");
    return NULL;
#else
    av_pixelutils_sad_fn sad[FF_ARRAY_ELEMS(sad_c)];

    memcpy(sad, sad_c, sizeof(sad));

    if (w_bits < 1 || w_bits > FF_ARRAY_ELEMS(sad) ||
            h_bits < 1 || h_bits > FF_ARRAY_ELEMS(sad))
        return NULL;
    if (w_bits != h_bits) // only squared sad for now
        return NULL;

#if ARCH_X86
    ff_pixelutils_sad_init_x86(sad, aligned);
#endif

    return sad[w_bits - 1];
#endif
}

#ifdef TEST
#define W1 320
#define H1 240
#define W2 640
#define H2 480

static int run_single_test(const char *test,
                           const uint8_t *block1, ptrdiff_t stride1,
                           const uint8_t *block2, ptrdiff_t stride2,
                           int align, int n)
{
    int out, ref;
    av_pixelutils_sad_fn f_ref = sad_c[n - 1];
    av_pixelutils_sad_fn f_out = av_pixelutils_get_sad_fn(n, n, align, NULL);

    switch (align)
    {
    case 0:
        block1++;
        block2++;
        break;
    case 1:
        block2++;
        break;
    case 2:
        break;
    }

    out = f_out(block1, stride1, block2, stride2);
    ref = f_ref(block1, stride1, block2, stride2);
    printf("[%s] [%c%c] SAD [%s] %dx%d=%d ref=%d\n",
           out == ref ? "OK" : "FAIL",
           align ? 'A' : 'U', align == 2 ? 'A' : 'U',
           test, 1<<n, 1<<n, out, ref);
    return out != ref;
}

static int run_test(const char *test,
                    const uint8_t *b1, const uint8_t *b2)
{
    int i, a, ret = 0;

    for (a = 0; a < 3; a++)
    {
        const uint8_t *block1 = b1;
        const uint8_t *block2 = b2;

        switch (a)
        {
        case 0:
            block1++;
            block2++;
            break;
        case 1:
            block2++;
            break;
        case 2:
            break;
        }
        for (i = 1; i <= FF_ARRAY_ELEMS(sad_c); i++)
        {
            int r = run_single_test(test, b1, W1, b2, W2, a, i);
            if (r)
                ret = r;
        }
    }
    return ret;
}

int main(void)
{
    int i, align, ret;
    uint8_t *buf1 = av_malloc(W1*H1);
    uint8_t *buf2 = av_malloc(W2*H2);
    uint32_t state = 0;

    if (!buf1 || !buf2)
    {
        fprintf(stderr, "malloc failure\n");
        ret = 1;
        goto end;
    }

    ff_check_pixfmt_descriptors();

#define RANDOM_INIT(buf, size) do {             \
    int k;                                      \
    for (k = 0; k < size; k++) {                \
        state = state * 1664525 + 1013904223;   \
        buf[k] = state>>24;                     \
    }                                           \
} while (0)

    /* Normal test with different strides */
    RANDOM_INIT(buf1, W1*H1);
    RANDOM_INIT(buf2, W2*H2);
    ret = run_test("random", buf1, buf2);
    if (ret < 0)
        goto end;

    /* Check for maximum SAD */
    memset(buf1, 0xff, W1*H1);
    memset(buf2, 0x00, W2*H2);
    ret = run_test("max", buf1, buf2);
    if (ret < 0)
        goto end;

    /* Check for minimum SAD */
    memset(buf1, 0x90, W1*H1);
    memset(buf2, 0x90, W2*H2);
    ret = run_test("min", buf1, buf2);
    if (ret < 0)
        goto end;

    /* Exact buffer sizes, to check for overreads */
    for (i = 1; i <= 4; i++)
    {
        for (align = 0; align < 3; align++)
        {
            int size1, size2;

            av_freep(&buf1);
            av_freep(&buf2);

            size1 = size2 = 1 << (i << 1);

            switch (align)
            {
            case 0:
                size1++;
                size2++;
                break;
            case 1:
                size2++;
                break;
            case 2:
                break;
            }

            buf1 = av_malloc(size1);
            buf2 = av_malloc(size2);
            if (!buf1 || !buf2)
            {
                fprintf(stderr, "malloc failure\n");
                ret = 1;
                goto end;
            }
            RANDOM_INIT(buf1, size1);
            RANDOM_INIT(buf2, size2);
            ret = run_single_test("small", buf1, 1<<i, buf2, 1<<i, align, i);
            if (ret < 0)
                goto end;
        }
    }

end:
    av_free(buf1);
    av_free(buf2);
    return ret;
}
#endif /* TEST */

﻿/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./tools_common.h"

#if CONFIG_VP8_ENCODER || CONFIG_VP9_ENCODER
#include "vpx/vp8cx.h"
#endif

#if CONFIG_VP8_DECODER || CONFIG_VP9_DECODER
#include "vpx/vp8dx.h"
#endif

#if defined(_WIN32) || defined(__OS2__)
#include <io.h>
#include <fcntl.h>

#ifdef __OS2__
#define _setmode setmode
#define _fileno fileno
#define _O_BINARY O_BINARY
#endif
#endif

#define LOG_ERROR(label)               \
  do {                                 \
    const char *l = label;             \
    va_list ap;                        \
    va_start(ap, fmt);                 \
    if (l) fprintf(stderr, "%s: ", l); \
    vfprintf(stderr, fmt, ap);         \
    fprintf(stderr, "\n");             \
    va_end(ap);                        \
  } while (0)

FILE *set_binary_mode(FILE *stream)
{
    (void)stream;
#if defined(_WIN32) || defined(__OS2__)
    _setmode(_fileno(stream), _O_BINARY);
#endif
    return stream;
}

void die(const char *fmt, ...)
{
    LOG_ERROR(NULL);
    usage_exit();
}

void fatal(const char *fmt, ...)
{
    LOG_ERROR("Fatal");
    exit(EXIT_FAILURE);
}

void warn(const char *fmt, ...)
{
    LOG_ERROR("Warning");
}

void die_codec(vpx_codec_ctx_t *ctx, const char *s)
{
    const char *detail = vpx_codec_error_detail(ctx);

    printf("%s: %s\n", s, vpx_codec_error(ctx));
    if (detail) printf("    %s\n", detail);
    exit(EXIT_FAILURE);
}

int read_yuv_frame(struct VpxInputContext *input_ctx, vpx_image_t *yuv_frame)
{
    FILE *f = input_ctx->file;
    struct FileTypeDetectionBuffer *detect = &input_ctx->detect;
    int plane = 0;
    int shortread = 0;
    const int bytespp = (yuv_frame->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1;

    for (plane = 0; plane < 3; ++plane)
    {
        uint8_t *ptr;
        const int w = vpx_img_plane_width(yuv_frame, plane);
        const int h = vpx_img_plane_height(yuv_frame, plane);
        int r;

        /* Determine the correct plane based on the image format. The for-loop
         * always counts in Y,U,V order, but this may not match the order of
         * the data on disk.
         */
        switch (plane)
        {
        case 1:
            ptr =
                yuv_frame->planes[yuv_frame->fmt == VPX_IMG_FMT_YV12 ? VPX_PLANE_V
                                  : VPX_PLANE_U];
            break;
        case 2:
            ptr =
                yuv_frame->planes[yuv_frame->fmt == VPX_IMG_FMT_YV12 ? VPX_PLANE_U
                                  : VPX_PLANE_V];
            break;
        default:
            ptr = yuv_frame->planes[plane];
        }

        for (r = 0; r < h; ++r)
        {
            size_t needed = w * bytespp;
            size_t buf_position = 0;
            const size_t left = detect->buf_read - detect->position;
            if (left > 0)
            {
                const size_t more = (left < needed) ? left : needed;
                memcpy(ptr, detect->buf + detect->position, more);
                buf_position = more;
                needed -= more;
                detect->position += more;
            }
            if (needed > 0)
            {
                shortread |= (fread(ptr + buf_position, 1, needed, f) < needed);
            }

            ptr += yuv_frame->stride[plane];
        }
    }

    return shortread;
}

#if CONFIG_ENCODERS

static const VpxInterface vpx_encoders[] =
{
#if CONFIG_VP8_ENCODER
    { "vp8", VP8_FOURCC, &vpx_codec_vp8_cx },
#endif

#if CONFIG_VP9_ENCODER
    { "vp9", VP9_FOURCC, &vpx_codec_vp9_cx },
#endif
};

int get_vpx_encoder_count(void)
{
    return sizeof(vpx_encoders) / sizeof(vpx_encoders[0]);
}

const VpxInterface *get_vpx_encoder_by_index(int i)
{
    return &vpx_encoders[i];
}

const VpxInterface *get_vpx_encoder_by_name(const char *name)
{
    int i;

    for (i = 0; i < get_vpx_encoder_count(); ++i)
    {
        const VpxInterface *encoder = get_vpx_encoder_by_index(i);
        if (strcmp(encoder->name, name) == 0) return encoder;
    }

    return NULL;
}

#endif  // CONFIG_ENCODERS

#if CONFIG_DECODERS

static const VpxInterface vpx_decoders[] =
{
#if CONFIG_VP8_DECODER
    { "vp8", VP8_FOURCC, &vpx_codec_vp8_dx },
#endif

#if CONFIG_VP9_DECODER
    { "vp9", VP9_FOURCC, &vpx_codec_vp9_dx },
#endif
};

int get_vpx_decoder_count(void)
{
    return sizeof(vpx_decoders) / sizeof(vpx_decoders[0]);
}

const VpxInterface *get_vpx_decoder_by_index(int i)
{
    return &vpx_decoders[i];
}

const VpxInterface *get_vpx_decoder_by_name(const char *name)
{
    int i;

    for (i = 0; i < get_vpx_decoder_count(); ++i)
    {
        const VpxInterface *const decoder = get_vpx_decoder_by_index(i);
        if (strcmp(decoder->name, name) == 0) return decoder;
    }

    return NULL;
}

const VpxInterface *get_vpx_decoder_by_fourcc(uint32_t fourcc)
{
    int i;

    for (i = 0; i < get_vpx_decoder_count(); ++i)
    {
        const VpxInterface *const decoder = get_vpx_decoder_by_index(i);
        if (decoder->fourcc == fourcc) return decoder;
    }

    return NULL;
}

#endif  // CONFIG_DECODERS

// TODO(dkovalev): move this function to vpx_image.{c, h}, so it will be part
// of vpx_image_t support
int vpx_img_plane_width(const vpx_image_t *img, int plane)
{
    if (plane > 0 && img->x_chroma_shift > 0)
        return (img->d_w + 1) >> img->x_chroma_shift;
    else
        return img->d_w;
}

int vpx_img_plane_height(const vpx_image_t *img, int plane)
{
    if (plane > 0 && img->y_chroma_shift > 0)
        return (img->d_h + 1) >> img->y_chroma_shift;
    else
        return img->d_h;
}

void vpx_img_write(const vpx_image_t *img, FILE *file)
{
    int plane;

    for (plane = 0; plane < 3; ++plane)
    {
        const unsigned char *buf = img->planes[plane];
        const int stride = img->stride[plane];
        const int w = vpx_img_plane_width(img, plane) *
                      ((img->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
        const int h = vpx_img_plane_height(img, plane);
        int y;

        for (y = 0; y < h; ++y)
        {
            fwrite(buf, 1, w, file);
            buf += stride;
        }
    }
}

int vpx_img_read(vpx_image_t *img, FILE *file)
{
    int plane;

    for (plane = 0; plane < 3; ++plane)
    {
        unsigned char *buf = img->planes[plane];
        const int stride = img->stride[plane];
        const int w = vpx_img_plane_width(img, plane) *
                      ((img->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
        const int h = vpx_img_plane_height(img, plane);
        int y;

        for (y = 0; y < h; ++y)
        {
            if (fread(buf, 1, w, file) != (size_t)w) return 0;
            buf += stride;
        }
    }

    return 1;
}

// TODO(dkovalev) change sse_to_psnr signature: double -> int64_t
double sse_to_psnr(double samples, double peak, double sse)
{
    static const double kMaxPSNR = 100.0;

    if (sse > 0.0)
    {
        const double psnr = 10.0 * log10(samples * peak * peak / sse);
        return psnr > kMaxPSNR ? kMaxPSNR : psnr;
    }
    else
    {
        return kMaxPSNR;
    }
}

// TODO(debargha): Consolidate the functions below into a separate file.
#if CONFIG_VP9_HIGHBITDEPTH
static void highbd_img_upshift(vpx_image_t *dst, vpx_image_t *src,
                               int input_shift)
{
    // Note the offset is 1 less than half.
    const int offset = input_shift > 0 ? (1 << (input_shift - 1)) - 1 : 0;
    int plane;
    if (dst->d_w != src->d_w || dst->d_h != src->d_h ||
            dst->x_chroma_shift != src->x_chroma_shift ||
            dst->y_chroma_shift != src->y_chroma_shift || dst->fmt != src->fmt ||
            input_shift < 0)
    {
        fatal("Unsupported image conversion");
    }
    switch (src->fmt)
    {
    case VPX_IMG_FMT_I42016:
    case VPX_IMG_FMT_I42216:
    case VPX_IMG_FMT_I44416:
    case VPX_IMG_FMT_I44016:
        break;
    default:
        fatal("Unsupported image conversion");
        break;
    }
    for (plane = 0; plane < 3; plane++)
    {
        int w = src->d_w;
        int h = src->d_h;
        int x, y;
        if (plane)
        {
            w = (w + src->x_chroma_shift) >> src->x_chroma_shift;
            h = (h + src->y_chroma_shift) >> src->y_chroma_shift;
        }
        for (y = 0; y < h; y++)
        {
            uint16_t *p_src =
                (uint16_t *)(src->planes[plane] + y * src->stride[plane]);
            uint16_t *p_dst =
                (uint16_t *)(dst->planes[plane] + y * dst->stride[plane]);
            for (x = 0; x < w; x++) *p_dst++ = (*p_src++ << input_shift) + offset;
        }
    }
}

static void lowbd_img_upshift(vpx_image_t *dst, vpx_image_t *src,
                              int input_shift)
{
    // Note the offset is 1 less than half.
    const int offset = input_shift > 0 ? (1 << (input_shift - 1)) - 1 : 0;
    int plane;
    if (dst->d_w != src->d_w || dst->d_h != src->d_h ||
            dst->x_chroma_shift != src->x_chroma_shift ||
            dst->y_chroma_shift != src->y_chroma_shift ||
            dst->fmt != src->fmt + VPX_IMG_FMT_HIGHBITDEPTH || input_shift < 0)
    {
        fatal("Unsupported image conversion");
    }
    switch (src->fmt)
    {
    case VPX_IMG_FMT_I420:
    case VPX_IMG_FMT_I422:
    case VPX_IMG_FMT_I444:
    case VPX_IMG_FMT_I440:
        break;
    default:
        fatal("Unsupported image conversion");
        break;
    }
    for (plane = 0; plane < 3; plane++)
    {
        int w = src->d_w;
        int h = src->d_h;
        int x, y;
        if (plane)
        {
            w = (w + src->x_chroma_shift) >> src->x_chroma_shift;
            h = (h + src->y_chroma_shift) >> src->y_chroma_shift;
        }
        for (y = 0; y < h; y++)
        {
            uint8_t *p_src = src->planes[plane] + y * src->stride[plane];
            uint16_t *p_dst =
                (uint16_t *)(dst->planes[plane] + y * dst->stride[plane]);
            for (x = 0; x < w; x++)
            {
                *p_dst++ = (*p_src++ << input_shift) + offset;
            }
        }
    }
}

void vpx_img_upshift(vpx_image_t *dst, vpx_image_t *src, int input_shift)
{
    if (src->fmt & VPX_IMG_FMT_HIGHBITDEPTH)
    {
        highbd_img_upshift(dst, src, input_shift);
    }
    else
    {
        lowbd_img_upshift(dst, src, input_shift);
    }
}

void vpx_img_truncate_16_to_8(vpx_image_t *dst, vpx_image_t *src)
{
    int plane;
    if (dst->fmt + VPX_IMG_FMT_HIGHBITDEPTH != src->fmt || dst->d_w != src->d_w ||
            dst->d_h != src->d_h || dst->x_chroma_shift != src->x_chroma_shift ||
            dst->y_chroma_shift != src->y_chroma_shift)
    {
        fatal("Unsupported image conversion");
    }
    switch (dst->fmt)
    {
    case VPX_IMG_FMT_I420:
    case VPX_IMG_FMT_I422:
    case VPX_IMG_FMT_I444:
    case VPX_IMG_FMT_I440:
        break;
    default:
        fatal("Unsupported image conversion");
        break;
    }
    for (plane = 0; plane < 3; plane++)
    {
        int w = src->d_w;
        int h = src->d_h;
        int x, y;
        if (plane)
        {
            w = (w + src->x_chroma_shift) >> src->x_chroma_shift;
            h = (h + src->y_chroma_shift) >> src->y_chroma_shift;
        }
        for (y = 0; y < h; y++)
        {
            uint16_t *p_src =
                (uint16_t *)(src->planes[plane] + y * src->stride[plane]);
            uint8_t *p_dst = dst->planes[plane] + y * dst->stride[plane];
            for (x = 0; x < w; x++)
            {
                *p_dst++ = (uint8_t)(*p_src++);
            }
        }
    }
}

static void highbd_img_downshift(vpx_image_t *dst, vpx_image_t *src,
                                 int down_shift)
{
    int plane;
    if (dst->d_w != src->d_w || dst->d_h != src->d_h ||
            dst->x_chroma_shift != src->x_chroma_shift ||
            dst->y_chroma_shift != src->y_chroma_shift || dst->fmt != src->fmt ||
            down_shift < 0)
    {
        fatal("Unsupported image conversion");
    }
    switch (src->fmt)
    {
    case VPX_IMG_FMT_I42016:
    case VPX_IMG_FMT_I42216:
    case VPX_IMG_FMT_I44416:
    case VPX_IMG_FMT_I44016:
        break;
    default:
        fatal("Unsupported image conversion");
        break;
    }
    for (plane = 0; plane < 3; plane++)
    {
        int w = src->d_w;
        int h = src->d_h;
        int x, y;
        if (plane)
        {
            w = (w + src->x_chroma_shift) >> src->x_chroma_shift;
            h = (h + src->y_chroma_shift) >> src->y_chroma_shift;
        }
        for (y = 0; y < h; y++)
        {
            uint16_t *p_src =
                (uint16_t *)(src->planes[plane] + y * src->stride[plane]);
            uint16_t *p_dst =
                (uint16_t *)(dst->planes[plane] + y * dst->stride[plane]);
            for (x = 0; x < w; x++) *p_dst++ = *p_src++ >> down_shift;
        }
    }
}

static void lowbd_img_downshift(vpx_image_t *dst, vpx_image_t *src,
                                int down_shift)
{
    int plane;
    if (dst->d_w != src->d_w || dst->d_h != src->d_h ||
            dst->x_chroma_shift != src->x_chroma_shift ||
            dst->y_chroma_shift != src->y_chroma_shift ||
            src->fmt != dst->fmt + VPX_IMG_FMT_HIGHBITDEPTH || down_shift < 0)
    {
        fatal("Unsupported image conversion");
    }
    switch (dst->fmt)
    {
    case VPX_IMG_FMT_I420:
    case VPX_IMG_FMT_I422:
    case VPX_IMG_FMT_I444:
    case VPX_IMG_FMT_I440:
        break;
    default:
        fatal("Unsupported image conversion");
        break;
    }
    for (plane = 0; plane < 3; plane++)
    {
        int w = src->d_w;
        int h = src->d_h;
        int x, y;
        if (plane)
        {
            w = (w + src->x_chroma_shift) >> src->x_chroma_shift;
            h = (h + src->y_chroma_shift) >> src->y_chroma_shift;
        }
        for (y = 0; y < h; y++)
        {
            uint16_t *p_src =
                (uint16_t *)(src->planes[plane] + y * src->stride[plane]);
            uint8_t *p_dst = dst->planes[plane] + y * dst->stride[plane];
            for (x = 0; x < w; x++)
            {
                *p_dst++ = *p_src++ >> down_shift;
            }
        }
    }
}

void vpx_img_downshift(vpx_image_t *dst, vpx_image_t *src, int down_shift)
{
    if (dst->fmt & VPX_IMG_FMT_HIGHBITDEPTH)
    {
        highbd_img_downshift(dst, src, down_shift);
    }
    else
    {
        lowbd_img_downshift(dst, src, down_shift);
    }
}
#endif  // CONFIG_VP9_HIGHBITDEPTH

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

#include <CoreServices/CoreServices.h>

#include "config.h"
#include "libavcodec/avcodec.h"
#if CONFIG_VDA
#  include "libavcodec/vda.h"
#endif
#if CONFIG_VIDEOTOOLBOX
#  include "libavcodec/videotoolbox.h"
#endif
#include "libavutil/imgutils.h"
#include "ffmpeg.h"

typedef struct VTContext
{
    AVFrame *tmp_frame;
} VTContext;

char *videotoolbox_pixfmt;

static int videotoolbox_retrieve_data(AVCodecContext *s, AVFrame *frame)
{
    InputStream *ist = s->opaque;
    VTContext  *vt = ist->hwaccel_ctx;
    CVPixelBufferRef pixbuf = (CVPixelBufferRef)frame->data[3];
    OSType pixel_format = CVPixelBufferGetPixelFormatType(pixbuf);
    CVReturn err;
    uint8_t *data[4] = { 0 };
    int linesize[4] = { 0 };
    int planes, ret, i;
    char codec_str[32];

    av_frame_unref(vt->tmp_frame);

    switch (pixel_format)
    {
    case kCVPixelFormatType_420YpCbCr8Planar:
        vt->tmp_frame->format = AV_PIX_FMT_YUV420P;
        break;
    case kCVPixelFormatType_422YpCbCr8:
        vt->tmp_frame->format = AV_PIX_FMT_UYVY422;
        break;
    case kCVPixelFormatType_32BGRA:
        vt->tmp_frame->format = AV_PIX_FMT_BGRA;
        break;
#ifdef kCFCoreFoundationVersionNumber10_7
    case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange:
        vt->tmp_frame->format = AV_PIX_FMT_NV12;
        break;
#endif
    default:
        av_get_codec_tag_string(codec_str, sizeof(codec_str), s->codec_tag);
        av_log(NULL, AV_LOG_ERROR,
               "%s: Unsupported pixel format: %s\n", codec_str, videotoolbox_pixfmt);
        return AVERROR(ENOSYS);
    }

    vt->tmp_frame->width  = frame->width;
    vt->tmp_frame->height = frame->height;
    ret = av_frame_get_buffer(vt->tmp_frame, 32);
    if (ret < 0)
        return ret;

    err = CVPixelBufferLockBaseAddress(pixbuf, kCVPixelBufferLock_ReadOnly);
    if (err != kCVReturnSuccess)
    {
        av_log(NULL, AV_LOG_ERROR, "Error locking the pixel buffer.\n");
        return AVERROR_UNKNOWN;
    }

    if (CVPixelBufferIsPlanar(pixbuf))
    {

        planes = CVPixelBufferGetPlaneCount(pixbuf);
        for (i = 0; i < planes; i++)
        {
            data[i]     = CVPixelBufferGetBaseAddressOfPlane(pixbuf, i);
            linesize[i] = CVPixelBufferGetBytesPerRowOfPlane(pixbuf, i);
        }
    }
    else
    {
        data[0] = CVPixelBufferGetBaseAddress(pixbuf);
        linesize[0] = CVPixelBufferGetBytesPerRow(pixbuf);
    }

    av_image_copy(vt->tmp_frame->data, vt->tmp_frame->linesize,
                  (const uint8_t **)data, linesize, vt->tmp_frame->format,
                  frame->width, frame->height);

    ret = av_frame_copy_props(vt->tmp_frame, frame);
    CVPixelBufferUnlockBaseAddress(pixbuf, kCVPixelBufferLock_ReadOnly);
    if (ret < 0)
        return ret;

    av_frame_unref(frame);
    av_frame_move_ref(frame, vt->tmp_frame);

    return 0;
}

static void videotoolbox_uninit(AVCodecContext *s)
{
    InputStream *ist = s->opaque;
    VTContext  *vt = ist->hwaccel_ctx;

    ist->hwaccel_uninit        = NULL;
    ist->hwaccel_retrieve_data = NULL;

    av_frame_free(&vt->tmp_frame);

    if (ist->hwaccel_id == HWACCEL_VIDEOTOOLBOX)
    {
#if CONFIG_VIDEOTOOLBOX
        av_videotoolbox_default_free(s);
#endif
    }
    else
    {
#if CONFIG_VDA
        av_vda_default_free(s);
#endif
    }
    av_freep(&ist->hwaccel_ctx);
}

int videotoolbox_init(AVCodecContext *s)
{
    InputStream *ist = s->opaque;
    int loglevel = (ist->hwaccel_id == HWACCEL_AUTO) ? AV_LOG_VERBOSE : AV_LOG_ERROR;
    int ret = 0;
    VTContext *vt;

    vt = av_mallocz(sizeof(*vt));
    if (!vt)
        return AVERROR(ENOMEM);

    ist->hwaccel_ctx           = vt;
    ist->hwaccel_uninit        = videotoolbox_uninit;
    ist->hwaccel_retrieve_data = videotoolbox_retrieve_data;

    vt->tmp_frame = av_frame_alloc();
    if (!vt->tmp_frame)
    {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    if (ist->hwaccel_id == HWACCEL_VIDEOTOOLBOX)
    {
#if CONFIG_VIDEOTOOLBOX
        if (!videotoolbox_pixfmt)
        {
            ret = av_videotoolbox_default_init(s);
        }
        else
        {
            AVVideotoolboxContext *vtctx = av_videotoolbox_alloc_context();
            CFStringRef pixfmt_str = CFStringCreateWithCString(kCFAllocatorDefault,
                                     videotoolbox_pixfmt,
                                     kCFStringEncodingUTF8);
            vtctx->cv_pix_fmt_type = UTGetOSTypeFromString(pixfmt_str);
            ret = av_videotoolbox_default_init2(s, vtctx);
            CFRelease(pixfmt_str);
        }
#endif
    }
    else
    {
#if CONFIG_VDA
        if (!videotoolbox_pixfmt)
        {
            ret = av_vda_default_init(s);
        }
        else
        {
            AVVDAContext *vdactx = av_vda_alloc_context();
            CFStringRef pixfmt_str = CFStringCreateWithCString(kCFAllocatorDefault,
                                     videotoolbox_pixfmt,
                                     kCFStringEncodingUTF8);
            vdactx->cv_pix_fmt_type = UTGetOSTypeFromString(pixfmt_str);
            ret = av_vda_default_init2(s, vdactx);
            CFRelease(pixfmt_str);
        }
#endif
    }
    if (ret < 0)
    {
        av_log(NULL, loglevel,
               "Error creating %s decoder.\n", ist->hwaccel_id == HWACCEL_VIDEOTOOLBOX ? "Videotoolbox" : "VDA");
        goto fail;
    }

    return 0;
fail:
    videotoolbox_uninit(s);
    return ret;
}

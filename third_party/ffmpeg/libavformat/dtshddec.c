﻿/*
 * Raw DTS-HD demuxer
 * Copyright (c) 2012 Paul B Mahol
 *
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

#include "libavutil/intreadwrite.h"
#include "libavutil/dict.h"
#include "avformat.h"

#define AUPR_HDR 0x415550522D484452
#define AUPRINFO 0x41555052494E464F
#define BITSHVTB 0x4249545348565442
#define BLACKOUT 0x424C41434B4F5554
#define BRANCHPT 0x4252414E43485054
#define BUILDVER 0x4255494C44564552
#define CORESSMD 0x434F524553534D44
#define DTSHDHDR 0x4454534844484452
#define EXTSS_MD 0x45585453535f4d44
#define FILEINFO 0x46494C45494E464F
#define NAVI_TBL 0x4E4156492D54424C
#define STRMDATA 0x5354524D44415441
#define TIMECODE 0x54494D45434F4445

typedef struct DTSHDDemuxContext
{
    uint64_t    data_end;
} DTSHDDemuxContext;

static int dtshd_probe(AVProbeData *p)
{
    if (AV_RB64(p->buf) == DTSHDHDR)
        return AVPROBE_SCORE_MAX;
    return 0;
}

static int dtshd_read_header(AVFormatContext *s)
{
    DTSHDDemuxContext *dtshd = s->priv_data;
    AVIOContext *pb = s->pb;
    uint64_t chunk_type, chunk_size;
    AVStream *st;
    int ret;
    char *value;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);
    st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id   = AV_CODEC_ID_DTS;
    st->need_parsing      = AVSTREAM_PARSE_FULL_RAW;

    while (!avio_feof(pb))
    {
        chunk_type = avio_rb64(pb);
        chunk_size = avio_rb64(pb);

        if (chunk_size < 4)
        {
            av_log(s, AV_LOG_ERROR, "chunk size too small\n");
            return AVERROR_INVALIDDATA;
        }
        if (chunk_size > ((uint64_t)1 << 61))
        {
            av_log(s, AV_LOG_ERROR, "chunk size too big\n");
            return AVERROR_INVALIDDATA;
        }

        switch (chunk_type)
        {
        case STRMDATA:
            dtshd->data_end = chunk_size + avio_tell(pb);
            if (dtshd->data_end <= chunk_size)
                return AVERROR_INVALIDDATA;
            return 0;
            break;
        case FILEINFO:
            if (chunk_size > INT_MAX)
                goto skip;
            value = av_malloc(chunk_size);
            if (!value)
                goto skip;
            avio_read(pb, value, chunk_size);
            value[chunk_size - 1] = 0;
            av_dict_set(&s->metadata, "fileinfo", value,
                        AV_DICT_DONT_STRDUP_VAL);
            break;
        default:
skip:
            ret = avio_skip(pb, chunk_size);
            if (ret < 0)
                return ret;
        };
    }

    return AVERROR_EOF;
}

static int raw_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    DTSHDDemuxContext *dtshd = s->priv_data;
    int64_t size, left;
    int ret;

    left = dtshd->data_end - avio_tell(s->pb);
    size = FFMIN(left, 1024);
    if (size <= 0)
        return AVERROR_EOF;

    ret = av_get_packet(s->pb, pkt, size);
    if (ret < 0)
        return ret;

    pkt->stream_index = 0;

    return ret;
}

AVInputFormat ff_dtshd_demuxer =
{
    .name           = "dtshd",
    .long_name      = NULL_IF_CONFIG_SMALL("raw DTS-HD"),
    .priv_data_size = sizeof(DTSHDDemuxContext),
    .read_probe     = dtshd_probe,
    .read_header    = dtshd_read_header,
    .read_packet    = raw_read_packet,
    .flags          = AVFMT_GENERIC_INDEX,
    .extensions     = "dtshd",
    .raw_codec_id   = AV_CODEC_ID_DTS,
};

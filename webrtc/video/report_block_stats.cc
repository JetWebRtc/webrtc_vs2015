﻿/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/video/report_block_stats.h"

namespace webrtc
{

namespace
{
int FractionLost(uint32_t num_lost_sequence_numbers,
                 uint32_t num_sequence_numbers)
{
    if (num_sequence_numbers == 0)
    {
        return 0;
    }
    return ((num_lost_sequence_numbers * 255) + (num_sequence_numbers / 2)) /
           num_sequence_numbers;
}
}  // namespace


// Helper class for rtcp statistics.
ReportBlockStats::ReportBlockStats()
    : num_sequence_numbers_(0),
      num_lost_sequence_numbers_(0)
{
}

void ReportBlockStats::Store(const RtcpStatistics& rtcp_stats,
                             uint32_t remote_ssrc,
                             uint32_t source_ssrc)
{
    RTCPReportBlock block;
    block.cumulativeLost = rtcp_stats.cumulative_lost;
    block.fractionLost = rtcp_stats.fraction_lost;
    block.extendedHighSeqNum = rtcp_stats.extended_max_sequence_number;
    block.jitter = rtcp_stats.jitter;
    block.remoteSSRC = remote_ssrc;
    block.sourceSSRC = source_ssrc;
    uint32_t num_sequence_numbers = 0;
    uint32_t num_lost_sequence_numbers = 0;
    StoreAndAddPacketIncrement(
        block, &num_sequence_numbers, &num_lost_sequence_numbers);
}

RTCPReportBlock ReportBlockStats::AggregateAndStore(
    const ReportBlockVector& report_blocks)
{
    RTCPReportBlock aggregate;
    if (report_blocks.empty())
    {
        return aggregate;
    }
    uint32_t num_sequence_numbers = 0;
    uint32_t num_lost_sequence_numbers = 0;
    ReportBlockVector::const_iterator report_block = report_blocks.begin();
    for (; report_block != report_blocks.end(); ++report_block)
    {
        aggregate.cumulativeLost += report_block->cumulativeLost;
        aggregate.jitter += report_block->jitter;
        StoreAndAddPacketIncrement(*report_block,
                                   &num_sequence_numbers,
                                   &num_lost_sequence_numbers);
    }

    if (report_blocks.size() == 1)
    {
        // No aggregation needed.
        return report_blocks[0];
    }
    // Fraction lost since previous report block.
    aggregate.fractionLost =
        FractionLost(num_lost_sequence_numbers, num_sequence_numbers);
    aggregate.jitter = static_cast<uint32_t>(
                           (aggregate.jitter + report_blocks.size() / 2) / report_blocks.size());
    return aggregate;
}

void ReportBlockStats::StoreAndAddPacketIncrement(
    const RTCPReportBlock& report_block,
    uint32_t* num_sequence_numbers,
    uint32_t* num_lost_sequence_numbers)
{
    // Get diff with previous report block.
    ReportBlockMap::iterator prev_report_block = prev_report_blocks_.find(
                report_block.sourceSSRC);
    if (prev_report_block != prev_report_blocks_.end())
    {
        int seq_num_diff = report_block.extendedHighSeqNum -
                           prev_report_block->second.extendedHighSeqNum;
        int cum_loss_diff = report_block.cumulativeLost -
                            prev_report_block->second.cumulativeLost;
        if (seq_num_diff >= 0 && cum_loss_diff >= 0)
        {
            *num_sequence_numbers += seq_num_diff;
            *num_lost_sequence_numbers += cum_loss_diff;
            // Update total number of packets/lost packets.
            num_sequence_numbers_ += seq_num_diff;
            num_lost_sequence_numbers_ += cum_loss_diff;
        }
    }
    // Store current report block.
    prev_report_blocks_[report_block.sourceSSRC] = report_block;
}

int ReportBlockStats::FractionLostInPercent() const
{
    if (num_sequence_numbers_ == 0)
    {
        return -1;
    }
    return FractionLost(
               num_lost_sequence_numbers_, num_sequence_numbers_) * 100 / 255;
}

}  // namespace webrtc


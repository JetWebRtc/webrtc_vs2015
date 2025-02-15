﻿/*
 * DCA compatible decoder data
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

#ifndef AVCODEC_DCADATA_H
#define AVCODEC_DCADATA_H

#include <stdint.h>

extern const uint32_t ff_dca_bit_rates[32];

extern const uint8_t ff_dca_channels[16];

extern const uint8_t ff_dca_bits_per_sample[7];

extern const int16_t ff_dca_adpcm_vb[4096][4];

extern const uint32_t ff_dca_scale_factor_quant6[64];
extern const uint32_t ff_dca_scale_factor_quant7[128];

extern const uint32_t ff_dca_lossy_quant[32];
extern const float ff_dca_lossy_quant_d[32];

extern const uint32_t ff_dca_lossless_quant[32];
extern const float ff_dca_lossless_quant_d[32];

extern const int8_t ff_dca_high_freq_vq[1024][32];

extern const float ff_dca_fir_32bands_perfect[512];
extern const float ff_dca_fir_32bands_nonperfect[512];

extern const float ff_dca_lfe_fir_64[256];
extern const float ff_dca_lfe_fir_128[256];
extern const float ff_dca_lfe_xll_fir_64[256];
extern const float ff_dca_fir_64bands[1024];

#define FF_DCA_DMIXTABLE_SIZE      242
#define FF_DCA_INV_DMIXTABLE_SIZE  201

extern const uint16_t ff_dca_dmixtable[FF_DCA_DMIXTABLE_SIZE];
extern const uint32_t ff_dca_inv_dmixtable[FF_DCA_INV_DMIXTABLE_SIZE];

extern const float ff_dca_default_coeffs[10][6][2];

extern const uint32_t ff_dca_map_xxch_to_native[28];
extern const int ff_dca_ext_audio_descr_mask[8];

extern const uint64_t ff_dca_core_channel_layout[16];

extern const int32_t ff_dca_sampling_freqs[16];

extern const int8_t ff_dca_lfe_index[16];

extern const int8_t ff_dca_channel_reorder_lfe[16][9];
extern const int8_t ff_dca_channel_reorder_lfe_xch[16][9];
extern const int8_t ff_dca_channel_reorder_nolfe[16][9];
extern const int8_t ff_dca_channel_reorder_nolfe_xch[16][9];

extern const uint16_t ff_dca_vlc_offs[63];

#endif /* AVCODEC_DCADATA_H */

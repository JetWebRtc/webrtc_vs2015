﻿
/* -----------------------------------------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2013 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
  All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software that implements
the MPEG Advanced Audio Coding ("AAC") encoding and decoding scheme for digital audio.
This FDK AAC Codec software is intended to be used on a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient general perceptual
audio codecs. AAC-ELD is considered the best-performing full-bandwidth communications codec by
independent studies and is widely deployed. AAC has been standardized by ISO and IEC as part
of the MPEG specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including those of Fraunhofer)
may be obtained through Via Licensing (www.vialicensing.com) or through the respective patent owners
individually for the purpose of encoding or decoding bit streams in products that are compliant with
the ISO/IEC MPEG audio standards. Please note that most manufacturers of Android devices already license
these patent claims through Via Licensing or directly from the patent owners, and therefore FDK AAC Codec
software may already be covered under those patent licenses when it is used for those licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions with enhanced sound quality,
are also available from Fraunhofer. Users are encouraged to check the Fraunhofer website for additional
applications information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification, are permitted without
payment of copyright license fees provided that you satisfy the following conditions:

You must retain the complete text of this software license in redistributions of the FDK AAC Codec or
your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation and/or other materials
provided with redistributions of the FDK AAC Codec or your modifications thereto in binary form.
You must make available free of charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived from this library without
prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute the FDK AAC Codec
software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating that you changed the software
and the date of any change. For modified versions of the FDK AAC Codec, the term
"Fraunhofer FDK AAC Codec Library for Android" must be replaced by the term
"Third-Party Modified Version of the Fraunhofer FDK AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without limitation the patents of Fraunhofer,
ARE GRANTED BY THIS SOFTWARE LICENSE. Fraunhofer provides no warranty of patent non-infringement with
respect to this software.

You may use this FDK AAC Codec software or modifications thereto only for purposes that are authorized
by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright holders and contributors
"AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, including but not limited to the implied warranties
of merchantability and fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary, or consequential damages,
including but not limited to procurement of substitute goods or services; loss of use, data, or profits,
or business interruption, however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of this software, even if
advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------------------------------------- */

/***************************  Fraunhofer IIS FDK Tools  **********************

   Author(s):   Manuel Jander, Josef Hoepfl
   Description: MDCT routines

******************************************************************************/

#ifndef __MDCT_H__
#define __MDCT_H__



#include "common_fix.h"

#define MDCT_OUT_HEADROOM     2 /* Output additional headroom */
#define MDCT_OUTPUT_SCALE    (DFRACT_BITS-SAMPLE_BITS-MDCT_OUT_HEADROOM)
/* Refer to "Output word length" in ISO/IEC 14496-3:2008(E) 23.2.3.6 */
#define MDCT_OUTPUT_GAIN      16

#if (SAMPLE_BITS == DFRACT_BITS)
#define IMDCT_SCALE(x) (INT_PCM)SATURATE_LEFT_SHIFT(x, -MDCT_OUTPUT_SCALE, SAMPLE_BITS)
#else
#define IMDCT_SCALE(x) (INT_PCM)SATURATE_RIGHT_SHIFT(x, MDCT_OUTPUT_SCALE, SAMPLE_BITS)
#endif
#define IMDCT_SCALE_DBL(x) (FIXP_DBL)(x)

/**
 * \brief MDCT persistent data
 */
typedef struct
{
    union
    {
        FIXP_DBL *freq;
        FIXP_DBL *time;
    } overlap;                       /**< Pointer to overlap memory */

    const FIXP_WTP *prev_wrs;        /**< pointer to previous right window slope  */
    int prev_tl;                     /**< previous tranform length */
    int prev_nr;                     /**< previous right window offset */
    int prev_fr;                     /**< previous right window slope length */
    int ov_offset;                   /**< overlap time data fill level */
    int ov_size;                     /**< Overlap buffer size in words */

} mdct_t;

typedef mdct_t* H_MDCT;

/**
 * \brief Initialize as valid MDCT handle
 *
 * \param hMdct handle of an allocated MDCT handle.
 * \param overlap pointer to FIXP_DBL overlap buffer.
 * \param overlapBufferSize size in FIXP_DBLs of the given overlap buffer.
 * \return void
 */
void mdct_init( H_MDCT hMdct,
                FIXP_DBL *overlap,
                INT overlapBufferSize );

/**
 * \brief perform MDCT transform (time domain to frequency domain) with given parameters.
 *
 * \param hMdct handle of an allocated MDCT handle.
 * \param spectrum pointer to where the resulting MDCT spectrum will be stored into.
 * \param scalefactor pointer to the input scale shift value. Updated accordingly on return.
 * \param input pointer to input time domain signal
 * \param tl transformation length.
 * \param nr right window slope offset (amount of window coefficients assumed to be 1.0)
 * \param fr right overlap window slope length
 * \param wrs pointer to the right side overlap window coefficients.
 * \return number of input samples processed.
 */
INT  mdct_block(
    H_MDCT hMdct,
    FIXP_DBL *spectrum,
    INT *scalefactor,
    INT_PCM *input,
    INT tl,
    INT nr,
    INT fr,
    const FIXP_WTB *wrs );


/**
 * \brief add/multiply 2/N transform gain and MPEG4 part 3 defined output gain (see definition
 *        of MDCT_OUTPUT_GAIN) to given mantissa factor and exponent.
 * \param pGain pointer to the mantissa of a gain factor to be applied to IMDCT data.
 * \param pExponent pointer to the exponent of a gain factor to be applied to IMDCT data.
 * \param tl length of the IMDCT where the gain *pGain * (2 ^ *pExponent) will be applied to.
 */
void imdct_gain(
    FIXP_DBL *pGain,
    int *pExponent,
    int tl
);

/**
 * \brief drain buffered output samples into given buffer. Changes the MDCT state.
 */
INT imdct_drain(
    H_MDCT hMdct,
    FIXP_DBL * pTimeData,
    INT nrSamplesRoom
);


/**
 * \brief Copy overlap time domain data to given buffer. Does not change the MDCT state.
 * \return number of actually copied samples (ov + nr).
 */
INT imdct_copy_ov_and_nr(
    H_MDCT hMdct,
    FIXP_DBL * pTimeData,
    INT nrSamples
);

/**
 * \brief Adapt MDCT parameters for non-matching window slopes.
 * \param hMdct handle of an allocated MDCT handle.
 * \param pfl pointer to left overlap window side length.
 * \param pnl pointer to length of the left n part of the window.
 * \param tl transform length.
 * \param wls pointer to the left side overlap window coefficients.
 * \param noOutSamples desired number of output samples.
 */
void imdct_adapt_parameters(H_MDCT hMdct, int *pfl, int *pnl, int tl, const FIXP_WTP *wls, int noOutSamples);

/**
 * \brief perform several inverse MDCT transforms (frequency domain to time domain) with given parameters.
 *
 * \param hMdct handle of an allocated MDCT handle.
 * \param output pointer to where the output time domain signal will be stored into.
 * \param stride the stride factor for accessing time domain samples in output.
 * \param spectrum pointer to the input MDCT spectra.
 * \param scalefactors scale shift values of the input spectrum.
 * \param nSpec number of MDCT spectrums.
 * \param noOutSamples desired number of output samples.
 * \param tl transform length.
 * \param wls pointer to the left side overlap window coefficients.
 * \param fl left overlap window side length.
 * \param wrs pointer to the right side overlap window coefficients of all individual IMDCTs.
 * \param fr right overlap window side length of all individual IMDCTs.
 * \param gain factor to apply to output samples (if != 0).
 * \return number of output samples returned.
 */
INT  imdct_block(
    H_MDCT hMdct,
    FIXP_DBL *output,
    FIXP_DBL *spectrum,
    const SHORT scalefactor[],
    const INT nSpec,
    const INT noOutSamples,
    const INT tl,
    const FIXP_WTP *wls,
    INT fl,
    const FIXP_WTP *wrs,
    const INT fr,
    FIXP_DBL gain );


#endif /* __MDCT_H__ */

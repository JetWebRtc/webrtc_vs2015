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

/************************  FDK PCM postprocessor module  *********************

   Author(s):   Matthias Neusinger
   Description: Hard limiter for clipping prevention

*******************************************************************************/

#include "limiter.h"


struct TDLimiter
{
    unsigned int  attack;
    FIXP_DBL      attackConst, releaseConst;
    unsigned int  attackMs, releaseMs, maxAttackMs;
    FIXP_PCM      threshold;
    unsigned int  channels, maxChannels;
    unsigned int  sampleRate, maxSampleRate;
    FIXP_DBL      cor, max;
    FIXP_DBL*     maxBuf;
    FIXP_DBL*     delayBuf;
    unsigned int  maxBufIdx, delayBufIdx;
    FIXP_DBL      smoothState0;
    FIXP_DBL      minGain;

    FIXP_DBL      additionalGainPrev;
    FIXP_DBL      additionalGainFilterState;
    FIXP_DBL      additionalGainFilterState1;
};

/* create limiter */
TDLimiterPtr createLimiter(
    unsigned int  maxAttackMs,
    unsigned int  releaseMs,
    INT_PCM       threshold,
    unsigned int  maxChannels,
    unsigned int  maxSampleRate
)
{
    TDLimiterPtr limiter = NULL;
    unsigned int attack, release;
    FIXP_DBL attackConst, releaseConst, exponent;
    INT e_ans;

    /* calc attack and release time in samples */
    attack = (unsigned int)(maxAttackMs * maxSampleRate / 1000);
    release = (unsigned int)(releaseMs * maxSampleRate / 1000);

    /* alloc limiter struct */
    limiter = (TDLimiterPtr)FDKcalloc(1, sizeof(struct TDLimiter));
    if (!limiter) return NULL;

    /* alloc max and delay buffers */
    limiter->maxBuf   = (FIXP_DBL*)FDKcalloc(attack + 1, sizeof(FIXP_DBL));
    limiter->delayBuf = (FIXP_DBL*)FDKcalloc(attack * maxChannels, sizeof(FIXP_DBL));

    if (!limiter->maxBuf || !limiter->delayBuf)
    {
        destroyLimiter(limiter);
        return NULL;
    }

    /* attackConst = pow(0.1, 1.0 / (attack + 1)) */
    exponent = invFixp(attack+1);
    attackConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
    attackConst = scaleValue(attackConst, e_ans);

    /* releaseConst  = (float)pow(0.1, 1.0 / (release + 1)) */
    exponent = invFixp(release + 1);
    releaseConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
    releaseConst = scaleValue(releaseConst, e_ans);

    /* init parameters */
    limiter->attackMs      = maxAttackMs;
    limiter->maxAttackMs   = maxAttackMs;
    limiter->releaseMs     = releaseMs;
    limiter->attack        = attack;
    limiter->attackConst   = attackConst;
    limiter->releaseConst  = releaseConst;
    limiter->threshold     = (FIXP_PCM)threshold;
    limiter->channels      = maxChannels;
    limiter->maxChannels   = maxChannels;
    limiter->sampleRate    = maxSampleRate;
    limiter->maxSampleRate = maxSampleRate;

    resetLimiter(limiter);

    return limiter;
}


/* reset limiter */
TDLIMITER_ERROR resetLimiter(TDLimiterPtr limiter)
{
    if (limiter != NULL)
    {

        limiter->maxBufIdx = 0;
        limiter->delayBufIdx = 0;
        limiter->max = (FIXP_DBL)0;
        limiter->cor = FL2FXCONST_DBL(1.0f/(1<<1));
        limiter->smoothState0 = FL2FXCONST_DBL(1.0f/(1<<1));
        limiter->minGain = FL2FXCONST_DBL(1.0f/(1<<1));

        limiter->additionalGainPrev = FL2FXCONST_DBL(1.0f/(1<<TDL_GAIN_SCALING));
        limiter->additionalGainFilterState = FL2FXCONST_DBL(1.0f/(1<<TDL_GAIN_SCALING));
        limiter->additionalGainFilterState1 = FL2FXCONST_DBL(1.0f/(1<<TDL_GAIN_SCALING));

        FDKmemset(limiter->maxBuf,   0, (limiter->attack + 1) * sizeof(FIXP_DBL) );
        FDKmemset(limiter->delayBuf, 0, limiter->attack * limiter->channels * sizeof(FIXP_DBL) );
    }
    else
    {
        return TDLIMIT_INVALID_HANDLE;
    }

    return TDLIMIT_OK;
}


/* destroy limiter */
TDLIMITER_ERROR destroyLimiter(TDLimiterPtr limiter)
{
    if (limiter != NULL)
    {
        FDKfree(limiter->maxBuf);
        FDKfree(limiter->delayBuf);

        FDKfree(limiter);
    }
    else
    {
        return TDLIMIT_INVALID_HANDLE;
    }
    return TDLIMIT_OK;
}

/* apply limiter */
TDLIMITER_ERROR applyLimiter(TDLimiterPtr limiter,
                             INT_PCM*    samples,
                             FIXP_DBL*    pGain,
                             const INT*   gain_scale,
                             const UINT   gain_size,
                             const UINT   gain_delay,
                             const UINT   nSamples)
{
    unsigned int i, j;
    FIXP_PCM tmp1, tmp2;
    FIXP_DBL tmp, old, gain, additionalGain, additionalGainUnfiltered;
    FIXP_DBL minGain = FL2FXCONST_DBL(1.0f/(1<<1));

    FDK_ASSERT(gain_size == 1);
    FDK_ASSERT(gain_delay <= nSamples);

    if ( limiter == NULL ) return TDLIMIT_INVALID_HANDLE;

    {
        unsigned int channels       = limiter->channels;
        unsigned int attack         = limiter->attack;
        FIXP_DBL     attackConst    = limiter->attackConst;
        FIXP_DBL     releaseConst   = limiter->releaseConst;
        FIXP_DBL     threshold      = FX_PCM2FX_DBL(limiter->threshold)>>TDL_GAIN_SCALING;

        FIXP_DBL     max            = limiter->max;
        FIXP_DBL*    maxBuf         = limiter->maxBuf;
        unsigned int maxBufIdx      = limiter->maxBufIdx;
        FIXP_DBL     cor            = limiter->cor;
        FIXP_DBL*    delayBuf       = limiter->delayBuf;
        unsigned int delayBufIdx    = limiter->delayBufIdx;

        FIXP_DBL     smoothState0   = limiter->smoothState0;
        FIXP_DBL     additionalGainSmoothState = limiter->additionalGainFilterState;
        FIXP_DBL     additionalGainSmoothState1 = limiter->additionalGainFilterState1;

        for (i = 0; i < nSamples; i++)
        {

            if (i < gain_delay)
            {
                additionalGainUnfiltered = limiter->additionalGainPrev;
            }
            else
            {
                additionalGainUnfiltered = pGain[0];
            }

            /* Smooth additionalGain */
            /* [b,a] = butter(1, 0.01) */
            static const FIXP_SGL b[] = { FL2FXCONST_SGL(0.015466*2.0), FL2FXCONST_SGL( 0.015466*2.0) };
            static const FIXP_SGL a[] = { FL2FXCONST_SGL(1.000000), FL2FXCONST_SGL(-0.96907) };
            /* [b,a] = butter(1, 0.001) */
            //static const FIXP_SGL b[] = { FL2FXCONST_SGL(0.0015683*2.0), FL2FXCONST_SGL( 0.0015683*2.0) };
            //static const FIXP_SGL a[] = { FL2FXCONST_SGL(1.0000000), FL2FXCONST_SGL(-0.99686) };
            additionalGain = - fMult(additionalGainSmoothState, a[1]) + fMultDiv2( additionalGainUnfiltered, b[0]) + fMultDiv2(additionalGainSmoothState1, b[1]);
            additionalGainSmoothState1 = additionalGainUnfiltered;
            additionalGainSmoothState = additionalGain;

            /* Apply the additional scaling that has no delay and no smoothing */
            if (gain_scale[0] > 0)
            {
                additionalGain <<= gain_scale[0];
            }
            else
            {
                additionalGain >>= gain_scale[0];
            }

            /* get maximum absolute sample value of all channels, including the additional gain. */
            tmp1 = (FIXP_PCM)0;
            for (j = 0; j < channels; j++)
            {
                tmp2 = (FIXP_PCM)samples[i * channels + j];
                if (tmp2 == (FIXP_PCM)SAMPLE_MIN) /* protect fAbs from -1.0 value */
                    tmp2 = (FIXP_PCM)(SAMPLE_MIN+1);
                tmp1 = fMax(tmp1, fAbs(tmp2));
            }
            tmp = SATURATE_LEFT_SHIFT(fMultDiv2(tmp1, additionalGain), 1, DFRACT_BITS);

            /* set threshold as lower border to save calculations in running maximum algorithm */
            tmp = fMax(tmp, threshold);

            /* running maximum */
            old = maxBuf[maxBufIdx];
            maxBuf[maxBufIdx] = tmp;

            if (tmp >= max)
            {
                /* new sample is greater than old maximum, so it is the new maximum */
                max = tmp;
            }
            else if (old < max)
            {
                /* maximum does not change, as the sample, which has left the window was
                   not the maximum */
            }
            else
            {
                /* the old maximum has left the window, we have to search the complete
                   buffer for the new max */
                max = maxBuf[0];
                for (j = 1; j <= attack; j++)
                {
                    if (maxBuf[j] > max) max = maxBuf[j];
                }
            }
            maxBufIdx++;
            if (maxBufIdx >= attack+1) maxBufIdx = 0;

            /* calc gain */
            /* gain is downscaled by one, so that gain = 1.0 can be represented */
            if (max > threshold)
            {
                gain = fDivNorm(threshold, max)>>1;
            }
            else
            {
                gain = FL2FXCONST_DBL(1.0f/(1<<1));
            }

            /* gain smoothing, method: TDL_EXPONENTIAL */
            /* first order IIR filter with attack correction to avoid overshoots */

            /* correct the 'aiming' value of the exponential attack to avoid the remaining overshoot */
            if (gain < smoothState0)
            {
                cor = fMin(cor, fMultDiv2((gain - fMultDiv2(FL2FXCONST_SGL(0.1f*(1<<1)),smoothState0)), FL2FXCONST_SGL(1.11111111f/(1<<1)))<<2);
            }
            else
            {
                cor = gain;
            }

            /* smoothing filter */
            if (cor < smoothState0)
            {
                smoothState0 = fMult(attackConst,(smoothState0 - cor)) + cor;  /* attack */
                smoothState0 = fMax(smoothState0, gain); /* avoid overshooting target */
            }
            else
            {
                /* sign inversion twice to round towards +infinity,
                   so that gain can converge to 1.0 again,
                   for bit-identical output when limiter is not active */
                smoothState0 = -fMult(releaseConst,-(smoothState0 - cor)) + cor; /* release */
            }

            gain = smoothState0;

            /* lookahead delay, apply gain */
            for (j = 0; j < channels; j++)
            {

                tmp = delayBuf[delayBufIdx * channels + j];
                delayBuf[delayBufIdx * channels + j] = fMult((FIXP_PCM)samples[i * channels + j], additionalGain);

                /* Apply gain to delayed signal */
                if (gain < FL2FXCONST_DBL(1.0f/(1<<1)))
                    tmp = fMult(tmp,gain<<1);

                samples[i * channels + j] = FX_DBL2FX_PCM((FIXP_DBL)SATURATE_LEFT_SHIFT(tmp,TDL_GAIN_SCALING,DFRACT_BITS));
            }
            delayBufIdx++;
            if (delayBufIdx >= attack) delayBufIdx = 0;

            /* save minimum gain factor */
            if (gain < minGain) minGain = gain;
        }


        limiter->max = max;
        limiter->maxBufIdx = maxBufIdx;
        limiter->cor = cor;
        limiter->delayBufIdx = delayBufIdx;

        limiter->smoothState0 = smoothState0;
        limiter->additionalGainFilterState = additionalGainSmoothState;
        limiter->additionalGainFilterState1 = additionalGainSmoothState1;

        limiter->minGain = minGain;

        limiter->additionalGainPrev = pGain[0];

        return TDLIMIT_OK;
    }
}

/* get delay in samples */
unsigned int getLimiterDelay(TDLimiterPtr limiter)
{
    FDK_ASSERT(limiter != NULL);
    return limiter->attack;
}

/* set number of channels */
TDLIMITER_ERROR setLimiterNChannels(TDLimiterPtr limiter, unsigned int nChannels)
{
    if ( limiter == NULL ) return TDLIMIT_INVALID_HANDLE;

    if (nChannels > limiter->maxChannels) return TDLIMIT_INVALID_PARAMETER;

    limiter->channels = nChannels;
    //resetLimiter(limiter);

    return TDLIMIT_OK;
}

/* set sampling rate */
TDLIMITER_ERROR setLimiterSampleRate(TDLimiterPtr limiter, unsigned int sampleRate)
{
    unsigned int attack, release;
    FIXP_DBL attackConst, releaseConst, exponent;
    INT e_ans;

    if ( limiter == NULL ) return TDLIMIT_INVALID_HANDLE;

    if (sampleRate > limiter->maxSampleRate) return TDLIMIT_INVALID_PARAMETER;

    /* update attack and release time in samples */
    attack = (unsigned int)(limiter->attackMs * sampleRate / 1000);
    release = (unsigned int)(limiter->releaseMs * sampleRate / 1000);

    /* attackConst = pow(0.1, 1.0 / (attack + 1)) */
    exponent = invFixp(attack+1);
    attackConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
    attackConst = scaleValue(attackConst, e_ans);

    /* releaseConst  = (float)pow(0.1, 1.0 / (release + 1)) */
    exponent = invFixp(release + 1);
    releaseConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
    releaseConst = scaleValue(releaseConst, e_ans);

    limiter->attack        = attack;
    limiter->attackConst   = attackConst;
    limiter->releaseConst  = releaseConst;
    limiter->sampleRate    = sampleRate;

    /* reset */
    //resetLimiter(limiter);

    return TDLIMIT_OK;
}

/* set attack time */
TDLIMITER_ERROR setLimiterAttack(TDLimiterPtr limiter, unsigned int attackMs)
{
    unsigned int attack;
    FIXP_DBL attackConst, exponent;
    INT e_ans;

    if ( limiter == NULL ) return TDLIMIT_INVALID_HANDLE;

    if (attackMs > limiter->maxAttackMs) return TDLIMIT_INVALID_PARAMETER;

    /* calculate attack time in samples */
    attack = (unsigned int)(attackMs * limiter->sampleRate / 1000);

    /* attackConst = pow(0.1, 1.0 / (attack + 1)) */
    exponent = invFixp(attack+1);
    attackConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
    attackConst = scaleValue(attackConst, e_ans);

    limiter->attack        = attack;
    limiter->attackConst   = attackConst;
    limiter->attackMs      = attackMs;

    return TDLIMIT_OK;
}

/* set release time */
TDLIMITER_ERROR setLimiterRelease(TDLimiterPtr limiter, unsigned int releaseMs)
{
    unsigned int release;
    FIXP_DBL releaseConst, exponent;
    INT e_ans;

    if ( limiter == NULL ) return TDLIMIT_INVALID_HANDLE;

    /* calculate  release time in samples */
    release = (unsigned int)(releaseMs * limiter->sampleRate / 1000);

    /* releaseConst  = (float)pow(0.1, 1.0 / (release + 1)) */
    exponent = invFixp(release + 1);
    releaseConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
    releaseConst = scaleValue(releaseConst, e_ans);

    limiter->releaseConst  = releaseConst;
    limiter->releaseMs     = releaseMs;

    return TDLIMIT_OK;
}

/* set limiter threshold */
TDLIMITER_ERROR setLimiterThreshold(TDLimiterPtr limiter, INT_PCM threshold)
{
    if ( limiter == NULL ) return TDLIMIT_INVALID_HANDLE;

    limiter->threshold = (FIXP_PCM)threshold;

    return TDLIMIT_OK;
}

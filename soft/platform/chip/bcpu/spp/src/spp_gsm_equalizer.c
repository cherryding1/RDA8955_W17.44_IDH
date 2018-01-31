/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#include "cs_types.h"

#include "chip_id.h"

// common/include
#include "gsm.h"
#include "baseband_defs.h"
// SPAL public headers
#include "spal_vitac.h"
#include "spal_cap.h"
#include "spal_xcor.h"
#include "spal_itlv.h"
#ifdef CHIP_DIE_8955
#include "spal_mem.h"
#include "spal_cholk.h"
#include "spal_excor.h"
#include "sppp_equalizer_private.h"
#endif

// SPP public headers
#include "spp_profile_codes.h"
// SPP private headers
#include "sppp_cfg.h"
#include "sppp_gsm_private.h"
#include "sppp_debug.h"

// SPC public headers
#include "spc_mailbox.h"
#include "spc_ctx.h"

#ifdef __MBOX_DEBUG__ // DEBUG STUB
#include "spc_mailbox.h"
#include "spal_comregs.h"
#include "global_macros.h"
#endif

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


#define MAXU_NBIT(n) ((1<<n)-1)
#define SV_MAX MAXU_NBIT(7)

// spc context structure
extern SPC_CONTEXT_T g_spcCtxStruct;



#ifdef CHIP_DIE_8955
//=============================================================================
// spp_DigitalAgc
//-----------------------------------------------------------------------------
/// Performs digital automatic gain control.
/// This function try to adjust amplitude of samples closest to a given target value.
///
/// @param pwrValue UINT32. Power of samples before go through the function spp_DigitalAgc.
/// @param RxBuffer UINT32*. Address of the received samples.
/// @param pwrAfterDgc UINT32*. Power of samples after go through the function spp_DigitalAgc.
/// @return dgcShift INT8. Bit shift value, positive for right shift and negatvie for left shift.
//=============================================================================
PUBLIC INT8 spp_DigitalAgc(UINT32 pwrValue, UINT32* RxBuffer, UINT32* pwrAfterDgc)
{
// Target value for digtial agc control.
#define DIG_AGC_TARGET    (1<< g_sppGlobalParams.DigitalAgcTarget)

    INT8 dgcShift = 0;

    if (pwrValue > 0)
    {
        while (pwrValue < DIG_AGC_TARGET)
        {
            pwrValue <<= 2;
            dgcShift--;
        }
        while (pwrValue > ((DIG_AGC_TARGET << 4) / 9))
        {
            pwrValue >>= 2;
            dgcShift++;
        }
        if (dgcShift)
        {
            spal_ExcorIQShift(RxBuffer, BB_BURST_TOTAL_SIZE, dgcShift);
        }
    }

    *pwrAfterDgc = pwrValue/BB_BURST_TOTAL_SIZE;
    return dgcShift;
}

//=============================================================================
// spp_QrDecomposition
//-----------------------------------------------------------------------------
/// This function performs the QR Decomposition
///
/// @param noise UINT32. Noise figure.
/// @param tapInfo INT32*. Input 6 taps pointer.
/// @param prefltLen UINT8. Input prefilter length
/// @param rMat[PREFILTER_LEN][H_TARGET_LEN-1] INT32. R matrix derived from QR decomposition.
/// @param normalizedCoef UINT32*. Normolized coefficient of normolized R matrix.
//============================================================================
PUBLIC VOID spp_QrDecomposition(UINT32 noise,
                                INT32* tapInfo,
                                UINT8 prefltLen,
                                INT32 rMat[PREFILTER_LEN][H_TARGET_LEN-1],
                                UINT32* normalizedCoef)
{
    UINT32 dNew = 0;
    UINT32 delta = 1 << DELTA_SHIFT;
    INT32 sReal = 0;
    INT32 sImag = 0;
    UINT8 chanLen = H_TARGET_LEN;
    INT32 rTmp[PREFILTER_LEN + 1][H_TARGET_LEN - 1];
    INT16 tapFirstReal = 0;
    INT16 tapFirstImag = 0;
    UINT32 coef = 0;
    UINT32 dNewTmp = 0;

    prefltLen += 1;
    for (int n = 1; n <= prefltLen; n++)
    {
        tapFirstReal = (INT16) tapInfo[0];
        tapFirstImag = (INT16) (tapInfo[0]>>16);
        coef = 0;

        dNewTmp = (UINT32)(tapFirstReal * tapFirstReal + tapFirstImag * tapFirstImag);
        dNewTmp >>= 4;
        dNew = noise + (delta * dNewTmp >> 11);

        if (dNew == 0)
        {
            dNew = 1;
        }

        coef = (noise << 12) / dNew;
        noise = dNew;
        sReal = ((INT32)(delta << 6) * tapFirstReal / (INT32)dNew);
        sImag = ((INT32)(delta << 6) * tapFirstImag / (INT32)dNew);

        delta = (delta * coef) >> 12;
        for (int k = 1; k <= chanLen - 1; k++)
        {
            rTmp[n][k - 1] = ((((sReal * ((INT16)tapInfo[k]) + sImag * ((INT16)(tapInfo[k]>>16))) >> 10) + (((INT32)coef) * ((INT16)rTmp[n - 1][k - 1]) >> 12)) & 0xffff) |
                             ((((sReal * ((INT16)(tapInfo[k] >> 16)) - sImag * ((INT16)tapInfo[k])) >> 10) + (((INT32)coef) * ((INT16)(rTmp[n - 1][k - 1] >> 16)) >> 12)) << 16);
            tapInfo[k - 1] = (((INT16)tapInfo[k] + (((-(INT16)rTmp[n - 1][k - 1]) * tapFirstReal + ((INT16)(rTmp[n - 1][k - 1] >> 16)) * tapFirstImag) >> R_TMP_RIGHT_SHIFT)) & 0xffff) |
                             (((INT16)(tapInfo[k] >> 16) + (((-(INT16)rTmp[n - 1][k - 1]) * tapFirstImag - ((INT16)(rTmp[n - 1][k - 1] >> 16)) * tapFirstReal) >> R_TMP_RIGHT_SHIFT)) << 16);
        }
        tapInfo[chanLen - 1] = 0;
    }

    for (int i = 0; i < prefltLen; i++)
    {
        for (int j = 0; j < H_TARGET_LEN - 1; j++)
        {
            rMat[i][j] = rTmp[i + 1][j];     //2
        }
    }

    *normalizedCoef = noise;
}

//=============================================================================
// spp_ComputePrefilter
//-----------------------------------------------------------------------------
/// This function computes the coefficient of the MMSE-Prefilter and gets the equivalent channel info.
///
/// @param noise UINT32. Noise figure.
/// @param hInput INT32*. Input 6 taps pointer.
/// @param prefltLen. Input prefilter length.
/// @param chanLen. Input channel length.
/// @param pCoef. Output coefficient of the prefilter.
/// @param gEqu. The equivalent channel info.
//============================================================================
PUBLIC VOID spp_ComputePrefilter(UINT32 noise,
                                 INT32* hInput,
                                 UINT8 prefltLen,
                                 UINT8 chanLen,
                                 INT32* pCoef,
                                 INT32* gEqu)
{
    UINT32 normalizedCoef = 0;
    INT32 hTmp[H_TARGET_LEN];
    INT32 hMedium[H_TARGET_LEN];
    INT32 rMat[PREFILTER_LEN][H_TARGET_LEN-1];
    INT32 invQ[PREFILTER_LEN];
    INT32 tmp1 = 0;
    INT32 tmp2 = 0;
    UINT8 min = 0;
    UINT32 invNormalizedCoef = 0;
    UINT8 i = 0;
    UINT8 j = 0;

    for (i = 0; i < H_TARGET_LEN; i++)
    {
        hTmp[i] = hInput[i];
        hMedium[i] = hInput[i];
    }

    spp_QrDecomposition(noise, hTmp, prefltLen, rMat, &normalizedCoef);

    gEqu[0] = 1 << 11;
    for (i = 1; i < H_TARGET_LEN; i++)
    {
        gEqu[i] = rMat[prefltLen][i - 1];
    }
    invQ[prefltLen] = 1 << 8;

    for (i = prefltLen; i >= 1; i--)
    {
        tmp1 = 0;
        tmp2 = 0;
        min = (chanLen - 2) > (prefltLen - i) ? (prefltLen - i) : (chanLen - 2);
        for (j = 0; j <= min; j++)
        {
            tmp1 -=  ((INT16)invQ[i - 1 + j + 1]) * ((INT16)rMat[i - 1][j]) - ((INT16)(invQ[i - 1 + j + 1] >> 16)) * ((INT16)(rMat[i - 1][j] >> 16));
            tmp2 -=  (((INT16)invQ[i - 1 + j + 1]) * ((INT16)(rMat[i - 1][j] >> 16)) + ((INT16)(invQ[i - 1 + j + 1] >> 16)) * ((INT16)rMat[i - 1][j]));
        }
        invQ[i - 1] = ((tmp1 >> 11) & 0xffff) | ((tmp2 >> 11) << 16);

    }

    invNormalizedCoef = (1 << 20) / normalizedCoef;

    for (i = 1; i <= prefltLen + 1; i++)
    {
        tmp1 = 0;
        tmp2 = 0;
        min = (chanLen - 1) > (prefltLen - i + 1) ? (prefltLen - i + 1) : (chanLen - 1);
        for (j = 0; j <= min; j++)
        {
            tmp1 += ((INT16)hMedium[j]) * ((INT16)invQ[i + j - 1]) - ((INT16)( hMedium[j] >> 16)) * ((INT16)(invQ[i + j - 1] >> 16));
            tmp2 -= ((INT16)hMedium[j]) * ((INT16)(invQ[i + j - 1] >> 16)) + ((INT16)( hMedium[j] >> 16)) * ((INT16)invQ[i + j - 1]);
        }
        pCoef[i - 1] = ((tmp1 * invNormalizedCoef >> 10) & 0xffff) | ((tmp2 * invNormalizedCoef >> 10) << 16);
    }
}

//=============================================================================
// spp_HardDecisionToSym
//-----------------------------------------------------------------------------
/// Transform hard-decision and training sequence to GMSK symbols.
/// The hard-decision is output by function spal_XcorHvextract.
///
/// @param hardDecision UINT32*. Address of hard-decisions.
/// @param tsc UINT32. Index of training seqence.
/// @return symbol SPAL_COMPLEX_T*. Output GMSK symbols.
//=============================================================================
PUBLIC VOID spp_HardDecisionToSym(UINT32* hardDecision, UINT32 tsc, SPAL_COMPLEX_T* symbol)
{
    UINT8 m = 0;
    // 1st to 57th symbols
    for (m = 0; m < 57; m++)
    {
        if (!!(hardDecision[m / 32] & (1 << (m % 32))))
        {
            symbol[m].i = -SVAL_NEGONE_GPRS;
            symbol[m].q = 0;
        }
        else
        {
            symbol[m].i = SVAL_ONE_GPRS;
            symbol[m].q = 0;
        }
    }
    // the 58th symbols
    if (!!(hardDecision[114 / 32] & (1 << (114 % 32))))
    {
        symbol[57].i = -SVAL_NEGONE_GPRS;
        symbol[57].q = 0;
    }
    else
    {
        symbol[57].i = SVAL_ONE_GPRS;
        symbol[57].q = 0;
    }
    // the 59th to 84th symbols; training sequence;
    for (m = 0; m < 26; m++) // 59 to 84
    {
        if ((!!(tsc & (1 << m))) == 1)
        {
            symbol[m + 58].i = -SVAL_NEGONE_GPRS;
            symbol[m + 58].q = 0;
        }
        else
        {
            symbol[m + 58].i = SVAL_ONE_GPRS;
            symbol[m + 58].q = 0;
        }
    }
    // the 85th symbols
    if (!!(hardDecision[115 / 32] & (1 << (115 % 32))))
    {
        symbol[84].i = -SVAL_NEGONE_GPRS;
        symbol[84].q = 0;
    }
    else
    {
        symbol[84].i = SVAL_ONE_GPRS;
        symbol[84].q = 0;
    }
    // the 86th to 142nd symbols
    for (m = 0; m < 57; m++)
    {
        if (!!(hardDecision[(m + 57) / 32] & (1 << ((m + 57) % 32))))
        {
            symbol[m + 58 + 26 + 1].i = -SVAL_NEGONE_GPRS;
            symbol[m + 58 + 26 + 1].q = 0;
        }
        else
        {
            symbol[m + 58 + 26 + 1].i = SVAL_ONE_GPRS;
            symbol[m + 58 + 26 + 1].q = 0;
        }
    }
}

//=============================================================================
// spp_GaussSeidel
//-----------------------------------------------------------------------------
/// Performs Gauss-Seidel iteration. H = inverse(R) * B;
/// R is a 5x5 Matrix, B is a 5x1 vector, H is what we want.
///
/// @param gaussSeidel SPP_UNCACHED_GAUSS_GMSK_T*. Address of Gauss-Seidel struct.
/// @param hVec SPAL_COMPLEX_T*. Output 5x1 vector.
//=============================================================================
PUBLIC VOID spp_GaussSeidel(SPP_UNCACHED_GAUSS_GMSK_T* gaussSeidel, SPAL_COMPLEX_T* hVec)
{
#define MAX_ITR_NUM_GAUSS 3
#define LK 9

    SPAL_COMPLEX_T z[MAX_ITR_NUM_GAUSS + 1][5];
    UINT8 i;
    UINT8 j;
    UINT8 it;
    // initialization
    for (i = 0; i < 5; i++)
    {
        z[0][i].i = hVec[i].i;
        z[0][i].q = hVec[i].q;
        gaussSeidel->k_matrix[i][i].i = 0;
        gaussSeidel->k_matrix[i][i].q = 0;
    }
    // iteration
    for (it = 0; it < MAX_ITR_NUM_GAUSS; it++)
    {

        for (i = 0; i < 5; i++)
        {
            for (j = 0; j < 5; j++)
            {
                if (j > i)
                {
                    (gaussSeidel->temp)[j].i = z[it][j].i;
                    (gaussSeidel->temp)[j].q = z[it][j].q;
                }
                else if (j < i)
                {
                    (gaussSeidel->temp)[j].i = z[it + 1][j].i;
                    (gaussSeidel->temp)[j].q = z[it + 1][j].q;
                }
                else
                {
                    *(PUINT32)(gaussSeidel->temp + j) = 0;
                }
            }
            spal_XcorComplexMatrixMultiply((PINT16)(gaussSeidel->k_matrix[i]), (PINT16)(gaussSeidel->temp), (PINT16)&(gaussSeidel->q), 1, 5, LK);
            z[it + 1][i].i = ((gaussSeidel->x_matrix[i].i - gaussSeidel->q.i) >> 6);
            z[it + 1][i].q = ((gaussSeidel->x_matrix[i].q - gaussSeidel->q.q) >> 6);
        }
    }
    // output result
    for (i = 0; i<5; i++)
    {
        hVec[i].i = z[it][i].i;
        hVec[i].q = z[it][i].q;
    }
}

//=============================================================================
// spp_ChannelTracking
//-----------------------------------------------------------------------------
/// This function reestimates channel using result of first equalize.
/// Use 64 (19 + 26 + 19) hard-decisions, include training sequence, to update the channel.
///
/// @param hardDec SPAL_COMPLEX_T*. Input, address of 64 hard-decisions.
/// @param dataRecv SPAL_COMPLEX_T*. Input, address of received symbols.
/// @param tapsUpdate SPAL_COMPLEX_T*. Output, address of 5 taps.
//=============================================================================
PUBLIC VOID spp_ChannelTracking(SPAL_COMPLEX_T* hardDec, SPAL_COMPLEX_T* dataRecv, SPAL_COMPLEX_T* tapsUpdate)
{
#define SPAL_CMOV(address_a,address_b) *(PUINT32)(address_a)=*(PUINT32)(address_b);

    SPP_UNCACHED_GAUSS_GMSK_T gaussData;
    SPP_UNCACHED_GAUSS_GMSK_T* uncachedGaussData;
    UINT8 i;
    UINT8 j;

    uncachedGaussData = (SPP_UNCACHED_GAUSS_GMSK_T*)MEM_ACCESS_UNCACHED(&gaussData);

    for (i = 0; i < GAUSS_TOTAL_LENGTH; i++)
    {
        SPAL_CMOV(uncachedGaussData->sym_value + i, hardDec + i);
        SPAL_CMOV(uncachedGaussData->sym_value_conj + i, hardDec + (GAUSS_TOTAL_LENGTH - 1 - i));
    }

    for (i = 0, j = 4; i < 4; i++, j--)
    {
        spal_XcorComplexConvolution((INT16 *)(uncachedGaussData->sym_value + j), GAUSS_UPDATE_LENGTH,//Q(11)
                                    (INT16 *)uncachedGaussData->sym_value_conj, GAUSS_TOTAL_LENGTH,//Q(11)
                                    (INT16 *)(uncachedGaussData->k_matrix[i] + 1 + i), GAUSS_UPDATE_LENGTH + i, j, 13);//Q(6.9)
    }

    for (j = 0; j < 5; j++)
    {
        for (i = j + 1; i < 5; i++)
        {
            uncachedGaussData->k_matrix[i][j].i = uncachedGaussData->k_matrix[j][i].i;
            uncachedGaussData->k_matrix[i][j].q = uncachedGaussData->k_matrix[j][i].q;

        }
    }

    for (j = 0; j < 5; j++)
    {
        for (i = 0; i < j; i++)
        {
            uncachedGaussData->k_matrix[i][j].i = uncachedGaussData->k_matrix[j][i].i;
            uncachedGaussData->k_matrix[i][j].q = -uncachedGaussData->k_matrix[j][i].q;
        }
    }

    spal_XcorComplexConvolution((INT16 *)(dataRecv), GAUSS_UPDATE_LENGTH,//Q(9)
                                (INT16 *)(uncachedGaussData->sym_value_conj), GAUSS_TOTAL_LENGTH,//Q(11)
                                (INT16 *)(uncachedGaussData->x_matrix), GAUSS_UPDATE_LENGTH-1, 5, 11);//Q(6.5)

    spp_GaussSeidel(uncachedGaussData, tapsUpdate);
}

//=============================================================================
// spp_TapsShorten
//-----------------------------------------------------------------------------
/// This function selects 3 continuous taps out of 5 and sets others to zero.
/// Input is 5 taps and output is also 5, but 2 of output taps is zero.
///
/// @param taps SPAL_COMPLEX_T*. Input & Output, address of both input and output taps.
//=============================================================================
PUBLIC VOID spp_TapsShorten(SPAL_COMPLEX_T* taps)
{
    SPAL_COMPLEX_T tapsConj[5];
    SPAL_COMPLEX_T tapsLength;
    UINT8 m = 0;
    UINT8 index = 0;
    INT16 maxPwr = 0;

    for (m = 0; m < 5; m++)
    {
        tapsConj[m].i = taps[m].i;
        tapsConj[m].q = -taps[m].q;
    }

    for (m = 0; m < 3; m++)
    {
        spal_XcorComplexMatrixMultiply((PINT16)(taps + m), (PINT16)(tapsConj + m), (PINT16)(&tapsLength), 1, 3, g_sppGlobalParams.TapsShortFac);
        spal_InvalidateDcache();
        if (tapsLength.i > maxPwr)
        {
            maxPwr = tapsLength.i;
            index = m;
        }
    }

    switch (index)
    {
        case 0:
            taps[3].i = 0;
            taps[3].q = 0;
            taps[4].i = 0;
            taps[4].q = 0;
            break;
        case 1:
            taps[0].i = 0;
            taps[0].q = 0;
            taps[4].i = 0;
            taps[4].q = 0;
            break;
        case 2:
            taps[0].i = 0;
            taps[0].q = 0;
            taps[1].i = 0;
            taps[1].q = 0;
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//=============================================================================
// spp_ChannelEstimation8taps
//-----------------------------------------------------------------------------
/// This function includes two scheme of 5 tap info access, MMSE-Prefilter and select 5 max taps from 8 taps
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param tap8 UINT32*. Input 8 taps pointer
/// @param RxBuffer_HT. Input RxBuffer pointer from RxBuffer[4]
//=============================================================================
PUBLIC VOID spp_ChannelEstimation8taps(SPP_EQU_BURST_T* burst,
                                       UINT32 *tap8,
                                       SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    UINT32 Xcorr_Energy[SCH_TSC_XCOR_NBR]; // Sized for SCH
    UINT8 search_size;
    UINT8 TOf = 0;
    UINT16 tapWindow [3] = {6,7,8};
    INT16 *taps = (INT16*)burst->taps;
    UINT32 chPwr[8];
    UINT8 chPos = 0;
    UINT8 i;
    UINT32 pwrSum1 = 0;
    UINT32 pwrSum2 = 0;
    UINT32 pwrSum3 = 0;
    INT32 tapsPfIn[H_TARGET_LEN];
    INT32 samplesConvOut[161] ;
    INT32 gEquMin[H_TARGET_LEN];
    INT32 prefltMin[PREFILTER_LEN];
    INT32 pCoef[PREFILTER_LEN] ;
    INT32 gEqu[H_TARGET_LEN] ;
    UINT32 tapsPfInPwr =0, gEquMinPwr =0;
    UINT16 pfGain = 0;

    // ******************************
    // Search Time Synchronization
    // ******************************
    // Use XCOR to compute Xcorrelation with Training Sequence
    volatile INT16 *Xcorr= UncachedBuffers->NbVitac.Xcorr;
    // The first five bits of the training sequence are not
    // used to perform the following correlation, as they are
    // only a repetition of the last 5 bits of the training
    // sequence.
    // Therefore, the received pilots of interest start at earliest
    // on bit 65.
    // The correlation is performed using 16 consecutive bits of the
    // training sequence and it is performed on 8+SPAL_ISI=13
    // consecutive time slot.
    // We can detect a ToF of +/- 4 samples.
    spal_XcorTscxcNb(
        g_sppGsmTscNburst[burst->tsc],
        (UINT32*)(burst->samples + 60 + 4), // 4 samples margin
        (INT16*)Xcorr);

    // Calculate I^2 + Q^2 for every coeffs
    for(i=0; i<NB_TSC_XCOR_NBR; i++)
    {
        Xcorr_Energy[i] =   Xcorr[2*i]     * Xcorr[2*i] +
                            Xcorr[2*i + 1] * Xcorr[2*i + 1];
    }
    search_size = NB_TSC_XCOR_NBR - SPAL_ISI + 1;

    // Now that we have the correlation on 13 consecutive time
    // slots, we find the maximum power profile considering that
    // the channel is 5 taps long with the main tap in the
    // middle (this assumptions corresponds to a GMSK on an AWGN
    // channel).

    // the correlation window could be flatter at the expense
    // of a little snr decrease (~1dB) in static conditions
    spp_TofSearch( Xcorr_Energy,
                   tapWindow,
                   search_size,
                   &(burst->tof),
                   &TOf);

    // TODO : see if this saturation is usefull
    // TODO : if yes -> do it for SCH also
    // if(burst->tof>32) burst->tof=32;


    // Shift Symbol Buffer Origin for remaining of equalization process
    // After origin shift we have the following bits:
    //
    // NORMAL BURST
    // 0:2     -> guard bits
    // 3:59    -> data bits
    // 60      -> steal bit
    // 61:86   -> pilot bits
    // 87      -> steal bit
    // 88:144  -> data bits
    // 145:147 -> guard bits

    // ******************************
    // Channel taps Estimation
    // ******************************

    // CE Output shape:
    // tap idx: 0,1,2...7
    // peak value at idx==2
    burst->samples = burst->samples + 4;
    for (i = 0; i < 8; i++)
    {
        INT16 x;
        INT16 y;
        x = (INT16)(tap8[i] & 0xFFFF);
        y = (INT16)(tap8[i] >> 16);
        chPwr[i] = ((x * x) >> 2) + ((y * y) >> 2);
    }

    pwrSum1 = chPwr[0] + chPwr[1];
    pwrSum2 = chPwr[6] + chPwr[7];
    pwrSum3 = chPwr[0] + chPwr[7];

    if ((pwrSum1 < pwrSum2) && (pwrSum1 < pwrSum3))
    {
        chPos = 2;
    }
    else if ((pwrSum2 < pwrSum1) && (pwrSum2 < pwrSum3))
    {
        chPos = 0;
    }
    else
    {
        chPos = 1;
    }

    for(i = 0; i < 6; i++)
    {
        tap8[i] = tap8[i + chPos];
    }

    for(i = 0; i < 6; i++)
    {
        tapsPfIn[i] = tap8[i];
    }

    spp_ComputePrefilter(g_sppGlobalParams.Ht100QrNoise, tapsPfIn, PREFILTER_LEN-1, H_TARGET_LEN, pCoef, gEqu);

    for(i = 0; i < 6; i++)
    {
        tapsPfInPwr += ((INT16)tapsPfIn[i]) * ((INT16)tapsPfIn[i]) + ((INT16)(tapsPfIn[i] >> 16)) * ((INT16)(tapsPfIn[i] >> 16));
        gEquMinPwr += ((INT16)gEqu[i]) * ((INT16)gEqu[i]) + ((INT16)(gEqu[i] >> 16)) * ((INT16)(gEqu[i] >> 16));
    }

    gEquMinPwr =  gEquMinPwr >> 16;  //2

    pfGain = spp_Sqrt(tapsPfInPwr / gEquMinPwr);

    for(i = 0; i < 10; i++)
    {
        prefltMin[i] = (((INT32)((INT16)pCoef[i]) * pfGain >> 7) & 0xffff) |
                       (((INT32)((INT16)(pCoef[i] >> 16)) * pfGain >> 7) << 16);  //2
    }
    for(i = 0; i < 6; i++)
    {
        gEquMin[i] = (((INT32)((INT16)gEqu[i]) * pfGain >> 8) & 0xffff) |
                     (((INT32)((INT16)(gEqu[i] >> 16)) * pfGain >> 8) << 16);
    }

    spal_XcorComplexConvolution((INT16*)prefltMin, 10, (INT16*)burst->samples, 152, (INT16*)samplesConvOut, 0, 161, 8);
    spal_InvalidateDcache(); // clear cache

    for(i = 0; i < 152; i++)
    {
        burst->samples[i] = (UINT32)samplesConvOut[i + 9];
    }

    for(i = 0; i < 5; i++)
    {
        taps[2 * i] = (INT16)gEquMin[i] ;
        taps[2 * i + 1] = (INT16)(gEquMin[i] >> 16);
    }

    burst->samples += chPos;
    burst->samples_offset = (chPos + 4);
}


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
//=============================================================================
// spp_Chest
//-----------------------------------------------------------------------------
/// This function performs part of channel estimation and tap filtering.
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param taps INT16*. Output taps pointer
///
//=============================================================================

PROTECTED VOID spp_Chest(SPP_EQU_BURST_T* burst, INT16 *taps)
{

    // We skip 2 symbols on both sides of the NB TSC
    // NOTE : the Matrix for SCH is calculated for the same
    //        burst position as NB (it doesn't correspond to
    //        SCH Tsc border)
    spal_XcorChest((INT16*)&burst->samples[63 + CHIP_CHEST_LEN - SPAL_ISI], (INT16*)&g_sppChestTab[burst->tsc][0][0], taps, SPAL_ISI, 15);

    // approximative weightening
    taps[0]=(taps[0]*25231)>>15;
    taps[1]=(taps[1]*25231)>>15;
    taps[2]=(taps[2]*30749)>>15;
    taps[3]=(taps[3]*30749)>>15;
    taps[4]= taps[4];
    taps[5]= taps[5];
    taps[6]=(taps[6]*30749)>>15;
    taps[7]=(taps[7]*30749)>>15;
    taps[8]=(taps[8]*25231)>>15;
    taps[9]=(taps[9]*25231)>>15;
}

//=============================================================================
// spp_8tapDetectGsm()
//-----------------------------------------------------------------------------
/// This function is used to detect where last 3 tap has enough power
/// output global:g_fadingchannel_8tap_state(long term)
/// 0: 5tap channel
/// 1: 8tap channel
/// return value: current channel :
/// 0: 5tap channel
/// 1: 8tap channel
/// TapPwr: 8 tap pwr sum
//=============================================================================
PUBLIC UINT8 spp_8tapDetectGsm(SPP_COMPLEX_T* RLS_W, UINT32*TapPwr)
{
#define SPP_8TAP_DETECT_PWR_THD             3
#define SPP_8TAP_DETECT_CNT_LIMIT           127
#define SPP_8TAP_DETECT_8TAP_NUM            32
#define SPP_8TAP_DETECT_5TAP_NUM            16



    UINT8 Valid = 0;
    UINT32 prepwr=0, postpwr=0;
#if 0
    prepwr = spal_XcorComplexPower16((INT16*) RLS_W, NULL, 5, 0);
    postpwr = spal_XcorComplexPower16((INT16*) (&RLS_W[5]), NULL, 3, 0);
#else
    for(UINT8 i=0; i<8; i++)
    {
        UINT32 Pwr = ((RLS_W[i].i*RLS_W[i].i + RLS_W[i].q*RLS_W[i].q) >> 4);
        if (i<5)
            prepwr += Pwr;
        else
            postpwr += Pwr;
    }
#endif
    *TapPwr = ((prepwr + postpwr)>>(LPCH-4));// RLS_W.i and RLS_W.q : Q(LPCH)
    if ( (prepwr>>SPP_8TAP_DETECT_PWR_THD) < postpwr )
    {
        g_sppGlobalParams.FadingChannel8TapNum ++;
    }

    g_sppGlobalParams.FadingChannelCnt++;

    if (g_sppGlobalParams.FadingChannelCnt > SPP_8TAP_DETECT_CNT_LIMIT)
    {
        if (g_sppGlobalParams.FadingChannel8TapNum > SPP_8TAP_DETECT_8TAP_NUM)
            g_sppGlobalParams.FadingChannel8TapState = TRUE;
        else if (g_sppGlobalParams.FadingChannel8TapNum < SPP_8TAP_DETECT_5TAP_NUM)
            g_sppGlobalParams.FadingChannel8TapState = FALSE;

        g_sppGlobalParams.FadingChannelCnt = 0;
        g_sppGlobalParams.FadingChannel8TapNum  = 0;
    }

    if (g_sppGlobalParams.FadingChannel8TapState == FALSE)
        Valid = 0;
    else if ((prepwr >> 5) < postpwr)
        Valid = 1;

    return Valid;

}

//=============================================================================
// spal_InterfereDetect
//-----------------------------------------------------------------------------
// This function is used to detect interference
//
// @param samples      UINT32*. Pointer to the received sequence samples.
// @param ref           UINT32*. Pointer to the ideal sequence samples.
// @param power        UINT32.  Power of the sequence.
// @param noise         UINT32.  Noise of the sequence.
//
// @return               BOOL. Return the possibility of the existence of interference
//=============================================================================
PUBLIC BOOL spp_InterfereDetect(UINT32* samples, UINT32* ref, UINT32 power, UINT32 noise)
{
    UINT8 softDetect = (UINT8)(g_sppGlobalParams.NoiseRatio >> 24);
    if (softDetect > 128)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//=============================================================================
// spp_ChannelEstimation
//-----------------------------------------------------------------------------
/// This function performs Least Square Error (LSE) channel estimation.
///
/// @param WinType UINT8. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
PUBLIC VOID spp_ChannelEstimation(UINT8 WinType,
                                  SPP_EQU_BURST_T* burst,
                                  SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    UINT32 Xcorr_Energy[SCH_TSC_XCOR_NBR]; // Sized for SCH
    UINT8 search_size;

    int i;
    UINT8 TOf = 0;
    UINT32 tap8[8]= {0};
    UINT32 TapsPwr;
    UINT32 chPwr[8];
    UINT8 chPos=0;
    UINT32 maxPwr=0, sumPwr=0;
    UINT8  HT100Flag=0;
    INT16 *taps = (INT16*)burst->taps;
    UINT16 tapWindow [3] = {6,7,8};
    volatile INT32 *Xcorr_I= UncachedBuffers->SchVitac.Xcorr_I;
    volatile INT32 *Xcorr_Q= UncachedBuffers->SchVitac.Xcorr_Q;

    SPP_PROFILE_FUNCTION_ENTRY(spp_ChannelEstimation);
    // ******************************
    // Search Time Synchronization
    // ******************************
    // Use XCOR to compute Xcorrelation with Training Sequence
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        volatile INT16 *Xcorr= UncachedBuffers->NbVitac.Xcorr;
        // The first five bits of the training sequence are not
        // used to perform the following correlation, as they are
        // only a repetition of the last 5 bits of the training
        // sequence.
        // Therefore, the received pilots of interest start at earliest
        // on bit 65.
        // The correlation is performed using 16 consecutive bits of the
        // training sequence and it is performed on 8+SPAL_ISI=13
        // consecutive time slot.
        // We can detect a ToF of +/- 4 samples.
        spal_XcorTscxcNb(
            g_sppGsmTscNburst[burst->tsc],
            (UINT32*)(burst->samples + 60 + 4), // 4 samples margin
            (INT16*)Xcorr);

        // Calculate I^2 + Q^2 for every coeffs
        for(i=0; i<NB_TSC_XCOR_NBR; i++)
        {
            Xcorr_Energy[i] =   Xcorr[2*i]     * Xcorr[2*i] +
                                Xcorr[2*i + 1] * Xcorr[2*i + 1];
        }
        search_size = NB_TSC_XCOR_NBR - SPAL_ISI + 1;
    }
    else // SCH
    {

        spal_XcorTscxcSch(
            (UINT32*)(burst->samples + 28 + 12), // 12 samples margin
            (UINT32*)(Xcorr_I),
            (UINT32*)(Xcorr_Q));

        // Calculate I^2 + Q^2 for every coeffs
        // Xcorr results are 18bits -> rescale to avoid ovflw
        for(i=0; i<SCH_TSC_XCOR_NBR; i++)
        {
            Xcorr_Energy[i] =   (((Xcorr_I[i]>>2) * (Xcorr_I[i])>>2)) +
                                (((Xcorr_Q[i]>>2) * (Xcorr_Q[i])>>2));
        }
        search_size = SCH_TSC_XCOR_NBR - SPAL_ISI + 1;
    }

    // Now that we have the correlation on 13 consecutive time
    // slots, we find the maximum power profile considering that
    // the channel is 5 taps long with the main tap in the
    // middle (this assumptions corresponds to a GMSK on an AWGN
    // channel).

    // the correlation window could be flatter at the expense
    // of a little snr decrease (~1dB) in static conditions
    spp_TofSearch( Xcorr_Energy,
                   tapWindow,
                   search_size,
                   &(burst->tof),
                   &TOf);

    // TODO : see if this saturation is usefull
    // TODO : if yes -> do it for SCH also
    // if(burst->tof>32) burst->tof=32;


    // Shift Symbol Buffer Origin for remaining of equalization process
    // After origin shift we have the following bits:
    //
    // NORMAL BURST
    // 0:2     -> guard bits
    // 3:59    -> data bits
    // 60      -> steal bit
    // 61:86   -> pilot bits
    // 87      -> steal bit
    // 88:144  -> data bits
    // 145:147 -> guard bits


    // ******************************
    // Channel taps Estimation
    // ******************************

    if ((g_sppGlobalParams.TofFixedEnable == TRUE)&&(WinType == GSM_WIN_TYPE_NBURST))
    {
        TOf = 4;
    }
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        burst->samples += (TOf);
        burst->samples_offset = TOf;

        spal_XcorChest8taps(
            (INT16 *) (&burst->samples[63 + 8 - SPAL_ISI]), // format 3.9
            (INT16 *) (&g_spp_8TapChestTab[burst->tsc][0][0]),//format: Q(15)
            (INT16 *) tap8,// format: Q(LPCH)(i.e. 0.LPCH)
            8,
            15);
        spal_InvalidateDcache(); // clear cache

        if (g_sppGlobalParams.GcfMode)
        {
            HT100Flag = spp_8tapDetectGsm((SPP_COMPLEX_T*)tap8, &TapsPwr);
        }
        else
        {
            HT100Flag = g_sppGlobalParams.Ht100Mode;
        }

        if (HT100Flag)
        {
            for (i=0; i<8; i++)
            {
                INT16 x,y;
                x = (INT16)(tap8[i]&0xFFFF);
                y = (INT16)(tap8[i]>>16);
                chPwr[i] = ((x*x)>>2) + ((y*y)>>2);
            }
            maxPwr = 0;
            for (i=0; i<4; i++)
            {
                sumPwr = 0;
                for(UINT8 k=i; k<i+5; k++)
                    sumPwr += chPwr[k];
                if (maxPwr < sumPwr)
                {
                    maxPwr = sumPwr;
                    chPos = i;
                }
            }

            for (i=0; i<5; i++)
                burst->taps[i] = tap8[i+chPos];

            burst->samples += chPos;
            burst->samples_offset = (TOf + chPos);
        }
        else
        {
            spp_Chest(burst, taps);
        }
    }
    else
    {
        burst->samples += (TOf);
        burst->samples_offset = TOf; // Keep track of symbol offset for HBURST mode
        spp_Chest(burst, taps);
    }

    SPP_PROFILE_FUNCTION_EXIT(spp_ChannelEstimation);
}
#else
//=============================================================================
// spp_ChannelEstimation
//-----------------------------------------------------------------------------
/// This function performs Least Square Error (LSE) channel estimation.
///
/// @param WinType UINT8. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
PUBLIC VOID spp_ChannelEstimation(UINT8 WinType,
                                  SPP_EQU_BURST_T* burst,
                                  SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    UINT32 Xcorr_Energy[SCH_TSC_XCOR_NBR]; // Sized for SCH
    UINT8 search_size;
    int i;
    UINT8 TOf = 0;
    INT16 *taps = (INT16*)burst->taps;
    UINT16 tapWindow [3] = {6,7,8};
    volatile INT32 *Xcorr_I= UncachedBuffers->SchVitac.Xcorr_I;
    volatile INT32 *Xcorr_Q= UncachedBuffers->SchVitac.Xcorr_Q;

    SPP_PROFILE_FUNCTION_ENTRY(spp_ChannelEstimation);
    // ******************************
    // Search Time Synchronization
    // ******************************
    // Use XCOR to compute Xcorrelation with Training Sequence
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        volatile INT16 *Xcorr= UncachedBuffers->NbVitac.Xcorr;
        // The first five bits of the training sequence are not
        // used to perform the following correlation, as they are
        // only a repetition of the last 5 bits of the training
        // sequence.
        // Therefore, the received pilots of interest start at earliest
        // on bit 65.
        // The correlation is performed using 16 consecutive bits of the
        // training sequence and it is performed on 8+SPAL_ISI=13
        // consecutive time slot.
        // We can detect a ToF of +/- 4 samples.
        spal_XcorTscxcNb(
            g_sppGsmTscNburst[burst->tsc],
            (UINT32*)(burst->samples + 60 + 4), // 4 samples margin
            (INT16*)Xcorr);

        // Calculate I^2 + Q^2 for every coeffs
        for(i=0; i<NB_TSC_XCOR_NBR; i++)
        {
            Xcorr_Energy[i] =   Xcorr[2*i]     * Xcorr[2*i] +
                                Xcorr[2*i + 1] * Xcorr[2*i + 1];
        }
        search_size = NB_TSC_XCOR_NBR - SPAL_ISI + 1;
    }
    else // SCH
    {

        spal_XcorTscxcSch(
            (UINT32*)(burst->samples + 28 + 12), // 12 samples margin
            (UINT32*)(Xcorr_I),
            (UINT32*)(Xcorr_Q));

        // Calculate I^2 + Q^2 for every coeffs
        // Xcorr results are 18bits -> rescale to avoid ovflw
        for(i=0; i<SCH_TSC_XCOR_NBR; i++)
        {
            Xcorr_Energy[i] =   (((Xcorr_I[i]>>2) * (Xcorr_I[i])>>2)) +
                                (((Xcorr_Q[i]>>2) * (Xcorr_Q[i])>>2));
        }
        search_size = SCH_TSC_XCOR_NBR - SPAL_ISI + 1;
    }

    // Now that we have the correlation on 13 consecutive time
    // slots, we find the maximum power profile considering that
    // the channel is 5 taps long with the main tap in the
    // middle (this assumptions corresponds to a GMSK on an AWGN
    // channel).

    // the correlation window could be flatter at the expense
    // of a little snr decrease (~1dB) in static conditions
    spp_TofSearch( Xcorr_Energy,
                   tapWindow,
                   search_size,
                   &(burst->tof),
                   &TOf);

    // TODO : see if this saturation is usefull
    // TODO : if yes -> do it for SCH also
    // if(burst->tof>32) burst->tof=32;


    // Shift Symbol Buffer Origin for remaining of equalization process
    // After origin shift we have the following bits:
    //
    // NORMAL BURST
    // 0:2     -> guard bits
    // 3:59    -> data bits
    // 60      -> steal bit
    // 61:86   -> pilot bits
    // 87      -> steal bit
    // 88:144  -> data bits
    // 145:147 -> guard bits
    burst->samples += (TOf);
    burst->samples_offset = TOf; // Keep track of symbol offset for HBURST mode

    // ******************************
    // Channel taps Estimation
    // ******************************

    // We skip 2 symbols on both sides of the NB TSC
    // NOTE : the Matrix for SCH is calculated for the same
    //        burst position as NB (it doesn't correspond to
    //        SCH Tsc border)
    spal_XcorChest((INT16*)&burst->samples[63 + CHIP_CHEST_LEN - SPAL_ISI], (INT16*)&g_sppChestTab[burst->tsc][0][0], taps, SPAL_ISI, 15);



    // approximative weightening
    taps[0]=(taps[0]*25231)>>15;
    taps[1]=(taps[1]*25231)>>15;
    taps[2]=(taps[2]*30749)>>15;
    taps[3]=(taps[3]*30749)>>15;
    taps[4]= taps[4];
    taps[5]= taps[5];
    taps[6]=(taps[6]*30749)>>15;
    taps[7]=(taps[7]*30749)>>15;
    taps[8]=(taps[8]*25231)>>15;
    taps[9]=(taps[9]*25231)>>15;
    SPP_PROFILE_FUNCTION_EXIT(spp_ChannelEstimation);
}
#endif


#ifdef CHIP_DIE_8955
//=============================================================================
// spp_EqShiftParam
//-----------------------------------------------------------------------------
/// This function calculates eq param's shift_sb and shift_bm
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param offset UINT8. 0: original way. else: change for better gprs performance
///
//=============================================================================

PROTECTED VOID spp_EqShiftParam(SPAL_VITAC_EQU_PARAM_T* equ_param, UINT8 offset)
{
#define BM_SHIFT_MAX    17

    if (equ_param->shift_bm>SHIFT_BM_OFFSET) equ_param->shift_bm -= SHIFT_BM_OFFSET;

    equ_param->shift_sb = SHIFT_SB_OFFSET;

    if (equ_param->shift_bm>BM_SHIFT_MAX)
    {
        equ_param->shift_sb += (equ_param->shift_bm-BM_SHIFT_MAX);
        equ_param->shift_bm=BM_SHIFT_MAX;
    }
    else if (equ_param->shift_bm<4) equ_param->shift_bm = 4;
    if (equ_param->shift_sb>9) equ_param->shift_sb = 9;
    if (offset)
        equ_param->shift_bm -= 3;

}

//=============================================================================
// spp_Equalize
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param offset UINT8. 0: original way. else: change for better gprs performance
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================
PUBLIC VOID spp_Equalize(UINT8 WinType,
                         UINT8 offset,
                         SPP_EQU_BURST_T* burst,
                         SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    INT16 H[4];
    int i,survivor;
    SPAL_VITAC_EQU_PARAM_T equ_param;
    UINT32 SumTable[2*2*8/2];
    volatile UINT8  *path_diff_bkw;
    volatile UINT8  *path_diff_fwd;
    volatile UINT16 *PathMetric;
    UINT8* tb_wr_ptr = &(burst->sv[1]);
    UINT8   tb_start,tb_end,tb_idx,tb_size;
    INT16* taps = (INT16*) burst->taps;
    SPAL_DC_OFFSET_T Dco;
    BOOL fwd_vitac_done = FALSE;


    SPP_PROFILE_FUNCTION_ENTRY(spp_Equalize);
    // ***********************************
    // Derive the VITAC shifters settings
    // ***********************************
    equ_param.shift_bm = spp_Log2(burst->noise);
    spp_EqShiftParam(&equ_param, offset);

    // ***********************************
    // Precalculation of Sum terms for BMs
    // ***********************************
    // The branch metric for a given node can be precalculated
    // since the register state is directly linked to the node.
    spal_XcorBmsum(taps, (INT16*)SumTable);
    if (offset)
    {
        // change SumTable's Content:
        // SumTable:
        // 0->7: for forward bm
        // 8->15:for backward bm. conjugate of 0->7
        for (i=0; i<8; i++)
        {
            *(UINT16*)(&SumTable[i+8]) = *(UINT16*)(&SumTable[i]); // real
            *((UINT16*)(&SumTable[i+8])+1) = -*((UINT16*)(&SumTable[i])+1); // image
        }
    }
    // Compute H0 and HL for Viterbi Accelerator
    H[0] = taps[0];
    H[1] = taps[1];
    H[2] = taps[SPAL_DELAY*2];
    H[3] = taps[SPAL_DELAY*2+1];

    // Saturation on SYMB_SIZE bits signed
    for (i=0; i<4; i++)
    {
        H[i] = (H[i] > SYMB_MAX) ? SYMB_MAX : H[i];
        H[i] = (H[i] < SYMB_MIN) ? SYMB_MIN : H[i];
    }

    if (offset)
    {
        // backward H0 and HL
        // backward CIR: conj(h4,h3,h2,h1,h0)
        // tap value: h0,h1,h2,h3,h4
        equ_param.H0 = (UINT32)(((-H[3])<<16)|((0xFFFF)&H[2]));
        equ_param.HL = (UINT32)(((-H[1])<<16)|((0xFFFF)&H[0]));
    }
    else
    {
        equ_param.H0 = (UINT32)((H[1]<<16)|((0xFFFF)&H[0]));
        equ_param.HL = (UINT32)((H[3]<<16)|((0xFFFF)&H[2]));
    }
    equ_param.bmSum     = (INT16*)SumTable;

    // Setup for first half-burst equalization (backward)
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        equ_param.length = 3 + NB_HPAYLD_SIZE + 1;
        equ_param.in     = (burst->samples +3 +NB_TS_START +SPAL_DELAY -1 -2 - offset);
        equ_param.out    =
            (UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->NbVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->NbVitac.PathMetric;
        equ_param.first_state = g_sppNbBwequStartSt[burst->tsc];
    }
    else // SCH
    {
        equ_param.length = 3 + SCH_HPAYLD_SIZE;
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SPAL_DELAY -1 -2);
        equ_param.out    =
            (UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->SchVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->SchVitac.PathMetric;
        equ_param.first_state = SCH_BWEQU_START_ST;
    }

    // Run the viterbi for the backward part of the burst (1st half)
    spal_VitacEqualize(BACKWARD_TRELLIS, &equ_param);

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of backward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get the first payload bit
    // Due to Vitac architecture (PM double buffering), the actual
    // PM position depends on nb_symbol parity.
    PathMetric = (equ_param.length & 0x1) ? PathMetric + VIT_NSTATES:
                 PathMetric;
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;
        diff = (PathMetric[8]-PathMetric[0]);
        if (offset)
        {

            if (equ_param.shift_sb<=4)
                diff <<= (4-equ_param.shift_sb);
            else
                diff >>= (equ_param.shift_sb-4);
        }
        else
        {
            diff >>= equ_param.shift_sb;
        }
        burst->sv[0]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;
        diff = (PathMetric[0]-PathMetric[8]);
        if (offset)
        {
            if (equ_param.shift_sb<=4)
                diff <<= (4-equ_param.shift_sb);
            else
                diff >>= (equ_param.shift_sb-4);
        }
        else
        {
            diff >>= equ_param.shift_sb;
        }
        burst->sv[0]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // ***************************************************
    // Launching HW Viterbi Trellis processing of the 2nd
    // burst during the Trace-back of the first half-burst
    // if we are not in EQU_HBURST_MODE
    // ***************************************************
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        // Setup for second half-burst equalization (forward)
        // No change needed for the other parameters
        equ_param.in     = (burst->samples +3 +NB_TS_START +NB_TS_SIZE -2);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = g_sppNbFwequStartSt[burst->tsc];
        if (offset)
        {
            equ_param.H0 = (UINT32)((H[1]<<16)|((0xFFFF)&H[0]));
            equ_param.HL = (UINT32)((H[3]<<16)|((0xFFFF)&H[2]));
        }
        if (!EQU_HBURST_MODE)
        {
            // Run the viterbi now only in non-HalfBurst mode
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
        }
        // Start now even in EQU_BURST_MODE if the full Nburst
        // has been received
        else if (spp_FullBurstReady() && (WinType == GSM_WIN_TYPE_NBURST))
        {
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);
            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flags
            spp_OneFullBurstDone();
            fwd_vitac_done = TRUE;
        }
    }
    else // SCH
    {
        // SCH doesn't support HalfBurst mode
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SCH_TS_SIZE -2);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = SCH_FWEQU_START_ST;
        // Run the viterbi
        spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
    }


    // ****************************************
    // Back to the trace-back of 1st half-burst
    // ****************************************
    //  Setup the traceback loop bounds
    //  Note that for NB stealing bits are handled separately
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        tb_start = (56 + 1) + SPAL_DELAY - 1; // +1 for steal bit shift
        tb_end   = ( 0 + 1) + SPAL_DELAY - 1;
        tb_size  = 2*57; // steal bits not accounted for
    }
    else //SCH
    {
        tb_start = 38 + SPAL_DELAY - 1;
        tb_end   =  0 + SPAL_DELAY - 1;
        tb_size  = 2*39;
    }

    // Move traceback pointer to beginning of first part of payload
    tb_wr_ptr = &(burst->sv[1]);

    // Get the remaining payload bits
    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_bkw[VIT_NSTATES*(tb_idx) + survivor]);

        if (hard8(*tb_wr_ptr++))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the first stealing bit (hl) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[114] = (path_diff_bkw[VIT_NSTATES*SPAL_DELAY+survivor]);
    //-------------------------------------------------------------------

    // In case of EQU_HBURST_MODE we need to wait for the samples of
    // the second half burst to be ready
    if (EQU_HBURST_MODE && (!fwd_vitac_done))
    {
        if (WinType == GSM_WIN_TYPE_NBURST)
        {
            // Wait for EndOfBurst Interrupt
            while(!spp_FullBurstReady());
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);

            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flag
            spp_OneFullBurstDone();
        }
    }

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of forward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get first bit of payload
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;
        diff = (PathMetric[8]-PathMetric[0]);
        if (offset)
        {
            if (equ_param.shift_sb<=4)
                diff <<= (4-equ_param.shift_sb);
            else
                diff >>= (equ_param.shift_sb-4);
        }
        else
        {
            diff = diff >>  equ_param.shift_sb;
        }

        burst->sv[tb_size-1]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;
        diff = (PathMetric[0]-PathMetric[8]);
        if (offset)
        {
            if (equ_param.shift_sb<=4)
                diff <<= (4-equ_param.shift_sb);
            else
                diff >>= (equ_param.shift_sb-4);
        }
        else
        {
            diff >>= equ_param.shift_sb;
        }

        burst->sv[tb_size-1]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // Move traceback pointer to the end of second half of payload
    tb_wr_ptr = &(burst->sv[tb_size - 2]);

    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_fwd[VIT_NSTATES*tb_idx + survivor]);

        if (hard8(*tb_wr_ptr--))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the second stealing bit (hu) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[115] = (path_diff_fwd[VIT_NSTATES*SPAL_DELAY+survivor]);

    SPP_PROFILE_FUNCTION_EXIT(spp_Equalize);
}



//=============================================================================
// spp_Equalize_saic
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param RxTscAddr.UINT32*.INPUT Pointer the start address of Training sequence.
/// @param temsv UINT8. tempbuffer,is used to save the softvalues of whole burst.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================
PUBLIC VOID  spp_Equalize_saic(UINT8 WinType,
                               SPP_EQU_BURST_T* burst,
                               UINT8 temsv[],
                               SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    INT16 H[4];
    int i,survivor;
    SPAL_VITAC_EQU_PARAM_T equ_param;
    UINT32 SumTable[2*2*8/2];
    volatile UINT8  *path_diff_bkw;
    volatile UINT8  *path_diff_fwd;
    volatile UINT16 *PathMetric;
    UINT8* tb_wr_ptr = &(temsv[1]);
    UINT8   tb_start,tb_end,tb_idx,tb_size;
    INT16* taps = (INT16*) burst->taps;


    // SPP_PROFILE_FUNCTION_ENTRY(spp_Equalize_saic);
    // ***********************************
    // Derive the VITAC shifters settings
    // ***********************************
    equ_param.shift_bm = spp_Log2(burst->noise);
    spp_EqShiftParam(&equ_param, 0);


    // ***********************************
    // Precalculation of Sum terms for BMs
    // ***********************************
    // The branch metric for a given node can be precalculated
    // since the register state is directly linked to the node.
    spal_XcorBmsum(taps, (INT16*)SumTable);


    // Compute H0 and HL for Viterbi Accelerator
    H[0] = taps[0];
    H[1] = taps[1];
    H[2] = taps[SPAL_DELAY*2];
    H[3] = taps[SPAL_DELAY*2+1];

    // Saturation on SYMB_SIZE bits signed
    for (i=0; i<4; i++)
    {
        H[i] = (H[i] > SYMB_MAX) ? SYMB_MAX : H[i];
        H[i] = (H[i] < SYMB_MIN) ? SYMB_MIN : H[i];
    }

    equ_param.H0 = (UINT32)((H[1]<<16)|((0xFFFF)&H[0]));
    equ_param.HL = (UINT32)((H[3]<<16)|((0xFFFF)&H[2]));
    equ_param.bmSum     = (INT16*)SumTable;

    // Setup for whole-burst equalization (backward)

    equ_param.out    =
        (UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
    path_diff_bkw    =
        (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
    path_diff_fwd    =
        (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_fwd;
    equ_param.pm     =
        (UINT16*)UncachedBuffers->NbVitac.PathMetric;
    PathMetric       =
        (volatile UINT16*)UncachedBuffers->NbVitac.PathMetric;


    // Setup for whole-burst equalization (forward)
    // No change needed for the other parameters
    equ_param.length = 152;

    equ_param.in     = (burst->samples + 0);
    equ_param.out    = (UINT8 *)path_diff_fwd;

    equ_param.first_state = 0;

    // Run the viterbi now only in non-HalfBurst mode
    spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);


    // ****************************************
    // Back to the trace-back
    // ****************************************
    //  Setup the traceback loop bounds
    //  Note that for NB stealing bits are handled separately
    tb_start = 152 - 2; // +1 for steal bit shift
    tb_end   = SPAL_DELAY - 1;
    tb_size  = 152; // steal bits not accounted for


    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of forward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get first bit of payload
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;

        diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        // burst->sv[tb_size-1-SPAL_DELAY]=((diff>SV_MAX)? SV_MAX: diff);
        temsv[tb_size-1-SPAL_DELAY]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;

        diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        //  burst->sv[tb_size-1-SPAL_DELAY]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        temsv[tb_size-1-SPAL_DELAY]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // Move traceback pointer to the end of second half of payload
    // tb_wr_ptr = &(burst->sv[tb_size - 2 - SPAL_DELAY]);
    tb_wr_ptr = &(temsv[tb_size - 2 - SPAL_DELAY]);

    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_fwd[VIT_NSTATES*tb_idx + survivor]);

        if (hard8(*tb_wr_ptr--))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }

    // Store the second stealing bit (hu) at the end of the burst

//   SPP_PROFILE_FUNCTION_EXIT(spp_Equalize_saic);
}

//=============================================================================
// spp_DcocProcess
//-----------------------------------------------------------------------------
/// Dc canceliation and derotation.
///
/// @param RxBuffer UINT32*, INPUT.The start address of 1x data ,down sampling from 2x data.
/// @param size UINT8, INPUT.length of one burst data.
/// @param derotate BOOL. INPUT.derotation setting.
/// @param bypassDcoc BOOL.INPUT.remove dc value setting.
/// @param Dc_I INT16*,OUTPUT. real part of DC
/// @param  Dc_Q INT16*.OUTPUT.imag part of DC
/// @param Is2xMode (BOOL):INPUT.It is used to indicate 1x rate mode or 2x rate mode,1: 2x mode. 0: default 1x mode.
/// @return UINT32. Return the normalized power of one burst data.
//=============================================================================

PUBLIC  UINT32 spp_DcocProcess(UINT32* RxBuffer,
                               UINT8 size,
                               BOOL derotate,
                               BOOL bypassDcoc,
                               INT16* Dc_I,
                               INT16* Dc_Q,
                               BOOL Is2xMode)

{
    //Derotation
    UINT32 *pointer,tvalue,power;
    INT16 tempI,tempQ;
    SPAL_DC_OFFSET_T Dco;

    if(Is2xMode)
    {
        pointer = (UINT32*)&RxBuffer[1];
        power = spal_XcorDcoc((UINT32*) pointer,
                              size,
                              derotate, // Derotation
                              bypassDcoc,
                              &Dco);
        //  spal_InvalidateDcache();
        tempI = Dco.I;
        tempQ = Dco.Q;

        pointer = (UINT32*)&RxBuffer[159];
        tvalue = spal_XcorDcoc((UINT32*) pointer,
                               size,
                               derotate, // Derotation
                               bypassDcoc,
                               &Dco);
        //  spal_InvalidateDcache();
        *Dc_I = (Dco.I + tempI)/2;
        *Dc_Q = (Dco.Q + tempQ)/2;
        power = (power + tvalue)/2;

    }
    else
    {
        pointer = RxBuffer;
        power = spal_XcorDcoc((UINT32*) pointer,
                              size,
                              derotate, // Derotation
                              bypassDcoc,
                              &Dco);
        *Dc_I = Dco.I;
        *Dc_Q = Dco.Q;
        power /= size; // Normalised power
    }
    return (power);
}


//=============================================================================
// spp_EqualizeNBurstSaic
//-----------------------------------------------------------------------------
// Performs normal burst equalization including:
// - DC offset estimation and correction,
// - channel estimation,
// - SNR estimation,
// - Viterbi equalization and traceback,
// - Frequency Offset estimation.
// .
//
// @param RxBuffer UINT32*. Address of the received samples.
// @param Tsc UINT8. Training sequence number.
// @param EqBufferOut UINT32*.
// @param EqBufferOutSaic UINT32*.
// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*.Pointer to the  buffers used for vitac.
// @param Res SPP_N_BURST_RESULT_T*. Pointer to the burst equalization structure.
//
//=============================================================================
PUBLIC VOID spp_EqualizeNBurstSaic(UINT32* RxBuffer,
                                   UINT8 Tsc,
                                   UINT32* EqBufferOut,
                                   UINT32* EqBufferOutSaic,
                                   SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
                                   SPP_N_BURST_RESULT_T* Res)
{

    UINT32 refBurst[152]; // stores reconstructed burst
    UINT32 R_imag_vec[316];
    INT16 M_vec[300];
    UINT8  temsv[152];
    UINT16 i;
    INT16* Rx_ptr;
    UINT8 *svSaic;
    UINT8  Y_SHIFT;
    UINT32 pwr = 0;
    INT8 dgcShift = 0;

    // TODO : see if this structure can be used one level higher
    // to avoid passing too much parameters and keep structured datas.
    SPP_EQU_BURST_T burst; // equalisation structure
    //SPAL_DC_OFFSET_T Dco;

    SPP_PROFILE_FUNCTION_ENTRY(spp_EqualizeNBurst);
    burst.tsc       = Tsc;
    burst.samples   = (UINT32 *)&RxBuffer[1];
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)EqBufferOut;
    svSaic = (UINT8*)EqBufferOutSaic;
    Rx_ptr = (INT16*)RxBuffer;

    // DC Offset Correction
    INT16 DC_I,DC_Q;
    DC_I = 0;
    DC_Q = 0;


    burst.power = spp_DcocProcess((UINT32*)(RxBuffer),
                                  BB_BURST_TOTAL_SIZE,
                                  TRUE,
                                  BYPASS_DCOC,
                                  &DC_I,
                                  &DC_Q,
                                  1);
    burst.dco.i=DC_I;
    burst.dco.q=DC_Q;

    if (g_sppGlobalParams.RxBufferIndex)
    {
        burst.samples   = (UINT32 *)&RxBuffer[159];
    }
    else
    {
        burst.samples = (UINT32 *)&RxBuffer[1];
    }

    burst.power /= BB_BURST_TOTAL_SIZE; // Normalised power


    spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, &burst, UncachedBuffers);
    // SNR Estimation
    // --------------
    // First reconstruct TrainingSequence (minus 2 tail symbols on each side)
    spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                   (NB_TS_SIZE-4), // Reconstruction length
                   (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                   &(burst.ref[NB_TS_START+2]), // reconstructed output
                   SPAL_ISI); // channel tap number

    // Run power of the error calculation upon Training Sequence
    // The three guard bits are not included in the reference (reconstructed) burst
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[NB_TS_START+3+2]), // original
                      (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                      (NB_TS_SIZE-4)); // Number of samples
    if (g_sppGlobalParams.InterfernceDetect)
    {
        g_spcCtxStruct.IsSaicEnable = spp_InterfereDetect(burst.samples, burst.ref, burst.power, burst.noise);
    }

    // Normalise NoisePower and calculate Snr
    burst.noise /= (NB_TS_SIZE-4);

    // Equalize : Viterbi + traceback
    // ------------------------------
    spp_Equalize(GSM_WIN_TYPE_NBURST,0,&burst, UncachedBuffers);

    // Frequency error estimation
    // --------------------------
    spp_FofEstimation(GSM_WIN_TYPE_NBURST,&burst);

    // use the whole burst for snr re-calculation. burst.ref has been reconstruct in
    // spal_XcorSrec and spp_FofEstimation
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[3]), // original, 3 tail bits removed
                      (UINT32*)&(burst.ref[0]), // reference
                      142); // Number of samples. 58+26+58

    // Normalise NoisePower and calculate Snr
    burst.noise /= 142;

    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);

    // Eq output shift
    spp_EqSoftShift(Res->Snr, EqBufferOut);

    // Fill result structure
    Res->Pwr = spp_10log10(burst.power);
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;
    Res->Dco.i = burst.dco.i;
    Res->Dco.q = burst.dco.q;


    // SPP_PROFILE_FUNCTION_ENTRY(spp_saicstep1);

    pwr = burst.power *BB_BURST_TOTAL_SIZE;
    if ( pwr>0 )
    {
        while(pwr < DIG_AMP_TARGET_PWR_SAIC)
        {
            pwr <<= 2;
            dgcShift--;
        }
        while ( pwr > (16*DIG_AMP_TARGET_PWR_SAIC/9) )
        {
            pwr >>= 2;
            dgcShift++;
        }
        if (dgcShift)
        {
            spal_ExcorIQShift((UINT32 *)&RxBuffer[1],BB_BURST_TOTAL_SIZE,dgcShift);
            spal_ExcorIQShift((UINT32 *)&RxBuffer[159],BB_BURST_TOTAL_SIZE,dgcShift);
        }
    }
    pwr = pwr/BB_BURST_TOTAL_SIZE;
    for(i = 0; i < 4; i += 2)
    {
        RxBuffer[i] = 0;
        RxBuffer[i + 1] = 0;
        RxBuffer[i + 153] = 0;
        RxBuffer[i + 154] = 0;
        RxBuffer[i + 158] = 0;
        RxBuffer[i + 159] = 0;
        RxBuffer[i + 311] = 0;
        RxBuffer[i + 312] = 0;
    }
    RxBuffer[4] = 0;
    RxBuffer[157] = 0;
    RxBuffer[162] = 0;
    RxBuffer[315] = 0;

    for(i = 0; i < 316; i += 4)
    {
        R_imag_vec[i]    = (unsigned int)(Rx_ptr[(i<<1) + 1]);
        R_imag_vec[i+1] = (unsigned int)(Rx_ptr[(i<<1) + 3]);
        R_imag_vec[i+2] = (unsigned int)(Rx_ptr[(i<<1) + 5]);
        R_imag_vec[i+3] = (unsigned int)(Rx_ptr[(i<<1) + 7]);
    }

    burst.samples   = refBurst;

    Y_SHIFT = (spp_Log2(pwr) >> 1)  - 0;

    spp_SaicPro((INT16*) RxBuffer, R_imag_vec, M_vec, burst.tsc, Y_SHIFT , burst.samples, burst.taps);


    spp_Equalize_saic(GSM_WIN_TYPE_NBURST,&burst, temsv,UncachedBuffers);


    if(g_sppGlobalParams.IsSaicStep2Enable == FALSE)
    {
        svSaic = (UINT8*)EqBufferOutSaic;
        for(i = 0; i < 56; i+= 4)
        {
            svSaic[i] = temsv[i + 3];
            svSaic[i + 1] = temsv[i + 4];
            svSaic[i + 2] = temsv[i + 5];
            svSaic[i + 3] = temsv[i + 6];
            svSaic[i + 57] = temsv[i + 88];
            svSaic[i + 58] = temsv[i + 89];
            svSaic[i + 59] = temsv[i + 90];
            svSaic[i + 60] = temsv[i + 91];
        }
        svSaic[56] = temsv[59];
        svSaic[113] = temsv[144];
        svSaic[114] = temsv[60];
        svSaic[115] = temsv[87];
        return;
        //  SPP_PROFILE_FUNCTION_EXIT(spp_saicstep1);
    }
    else
    {
        //SPP_PROFILE_FUNCTION_EXIT(spp_saicstep1);

        //SPP_PROFILE_FUNCTION_ENTRY(spp_saicstep2);
        spp_SaicProstep2((INT16*) RxBuffer, R_imag_vec, M_vec, burst.tsc, Y_SHIFT , temsv ,svSaic);

        // SPP_PROFILE_FUNCTION_EXIT(spp_saicstep2);

    }

    SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeNBurst);
}
//=============================================================================
// spp_ExcorActive
//-----------------------------------------------------------------------------
/// This function is used to check if excor is busy.
//=============================================================================

INLINE BOOL spp_ExcorActive()
{
    if (((hwp_excor->status) & EXCOR_STATUS_MASK))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


//=============================================================================
// spp_ExcorActive
//-----------------------------------------------------------------------------
/// This function is used to check if excor is busy.
/// @param value UINT32. it is used to set ctrl_fast register of excor.
//=============================================================================

INLINE VOID  spp_CtrlFastRegSet(UINT32 value)
{
    hwp_excor->ctrl_fast = value;
    return;
}
//=============================================================================
// spp_SaicPro
//-----------------------------------------------------------------------------
// Performs saic process step1
// @param RxBuffer UINT16*. Address of the 2x received samples.
// @param R_imag_vec UINT32*. Address of the 2x received samples,imaginary parts.
// @param M_vecstep2 INT16*. M matrix(S(H)*S),it is used for saic process2.
// @param Tsc UINT8,Training Sequence index.
// @param Y_SHIFT UINT8, Shift parameter.
// @param Out UINT32*. Samples filterd out of saic process1.
// @param Tap UINT32*.equavalent taps.
//=============================================================================
PUBLIC VOID spp_SaicPro(INT16* RxBuffer, UINT32* R_imag_vec,  INT16* M_vecstep2, UINT8 Tsc, UINT8 Y_SHIFT,UINT32 * Out, UINT32* Tap)
{
    UINT16 i, j, k;
    INT8 s_fac;
    UINT32 P_vec[26];
    UINT32 P_vec_neg[26];
    UINT32 Z_vec[36];
    UINT32 Z_vec1[36];
    INT16 Y_out_ref[152];
    INT16 P_xcor[L_saic1];
    INT16 M_vec[304];
    INT16 M_cov[304];
    UINT32 *Z_out;
    UINT32 *Z_out0;
    INT16 *M_out;
    INT16 *M_vec1;
    INT16 *Y_out;
    UINT32 Ctrl_FastP1;
    UINT32 Ctrl_P1;
    UINT32 Ctrl_FastM1;
    UINT32 Ctrl_FastM2;
    UINT32 Ctrl_M1;
    UINT32 Ctrl_FastConv1;
    UINT32 Ctrl_Conv1;

    Z_out = MEM_ACCESS_UNCACHED(Z_vec);
    Z_out0 = MEM_ACCESS_UNCACHED(Z_vec1);
    M_out = (INT16 *)MEM_ACCESS_UNCACHED(M_cov);
    M_vec1 = (INT16 *)MEM_ACCESS_UNCACHED(M_vec);
    Y_out = (INT16 *)MEM_ACCESS_UNCACHED(Y_out_ref);

    Ctrl_FastP1 = EXCOR_LOOP_NUM_A(Lf_saic1)|EXCOR_LOOP_NUM_B(1)|EXCOR_LOOP_NUM_AB(L_saic1)|((0&0xFF)<< 16);
    Ctrl_P1 = EXCOR_CMD(EXCOR_FASTMATRIMUL) |EXCOR_SHIFT_BIT(Y_SHIFT+1) ;
    Ctrl_FastM1 = EXCOR_LOOP_NUM_A(Lf_saic1)|EXCOR_LOOP_NUM_B(Lf_saic1)|EXCOR_LOOP_NUM_AB(L_saic1)|((0&0xFF)<< 16);
    Ctrl_FastM2 = EXCOR_LOOP_NUM_A(Lf_saic1)|EXCOR_LOOP_NUM_B(Lb_saic1)|EXCOR_LOOP_NUM_AB(L_saic1)|((0&0xFF)<< 16);
    Ctrl_M1 = EXCOR_CMD(EXCOR_FASTMATRIMUL) |EXCOR_SHIFT_BIT(Y_SHIFT - 1) ;
    Ctrl_FastConv1 =    (Conv_shift1&0xFF)<< 16 ;
    Ctrl_Conv1 =  EXCOR_CMD(EXCOR_FASTCONV) |EXCOR_NB_ILOOP(Lf_saic1) | EXCOR_NB_OLOOP(152) |EXCOR_SHIFT_BIT(Conv_shift0) ;

    for(i = 0; i < 26; i++)
    {
        P_vec[i] = (UINT32)(((1 - 2*((INT16)((g_sppGsmTscNburst[Tsc]>> i) & 0x00000001))) << Y_SHIFT)& 0x0000FFFF);
        P_vec_neg[i] = (-P_vec[i]) & 0x0000FFFF;
    }

    ////////////////////////////////////////////////
    // hwp_excor->ctrl_fast = Ctrl_FastP1;
    spp_CtrlFastRegSet(Ctrl_FastP1);
    // S(H)*x05
    spal_XcorFastMatrixMultiplyS((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1], (INT16*)&P_vec[Lb_saic1], (INT16 *)(Z_out + (Lf_saic1*3) + Lb_saic1), Ctrl_P1);
    spal_XcorFastMatrixMultiplyS((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&P_vec[Lb_saic1], (INT16 *)(Z_out + (Lf_saic1<<1) + Lb_saic1), Ctrl_P1);
    spal_XcorFastMatrixMultiplyS((INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic)], (INT16*)&P_vec[Lb_saic1], (INT16 *)(Z_out + Lf_saic1 + Lb_saic1), Ctrl_P1);
    spal_XcorFastMatrixMultiplyS((INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)], (INT16*)&P_vec[Lb_saic1], (INT16 *)(Z_out+Lb_saic1), Ctrl_P1);

    for(i = 0; i < 20 ; i ++)
    {
        P_xcor[i] = (INT16)(Z_out[22-i]);
        P_xcor[i] = (P_xcor[i]  > 4095) ? 4095 : P_xcor[i];
        P_xcor[i] = (P_xcor[i]  < -4096) ? -4096 : P_xcor[i];
    }

    P_xcor[20] = ((INT16)g_sppSaicPMatrixTable[Tsc][0]) << (Y_SHIFT - 1);
    P_xcor[21] = ((INT16)g_sppSaicPMatrixTable[Tsc][1]) << (Y_SHIFT - 1);
    P_xcor[22] = ((INT16)g_sppSaicPMatrixTable[Tsc][2]) << (Y_SHIFT - 1);

    ///////////////////////////////////////////////////
    //S(H)*S
    //hwp_excor->ctrl_fast = Ctrl_FastM1;
    spp_CtrlFastRegSet(Ctrl_FastM1);
    spal_XcorFastMatrixMultiplyS((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1], (INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1] , (INT16 *)Z_out, Ctrl_M1);
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1],(INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)<<1] ,(INT16 *)Z_out0, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[0 + k] = (INT16)(Z_out[24 - k]);
        M_vec[22 + k] = (INT16)(Z_out[19 - k]);
        M_vec[43 + k] = (INT16)(Z_out[14 - k]);
        M_vec[63 + k] = (INT16)(Z_out[9 - k]);
        M_vec[82 + k] = (INT16)(Z_out[4 - k]);
        k++;
    }

    while (spp_ExcorActive());

    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1],(INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic)],(INT16 *)Z_out, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[5 + k] = (INT16)(Z_out0[24 - k]);
        M_vec[27 + k] = (INT16)(Z_out0[19 - k]);
        M_vec[48 + k] = (INT16)(Z_out0[14 - k]);
        M_vec[68 + k] = (INT16)(Z_out0[9 - k]);
        M_vec[87 + k] = (INT16)(Z_out0[4 - k]);
        k++;
    }


    while (spp_ExcorActive());
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1],(INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)],(INT16 *)Z_out0, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[10 + k] = (INT16)(Z_out[24 - k]);
        M_vec[32 + k] = (INT16)(Z_out[19 - k]);
        M_vec[53 + k] = (INT16)(Z_out[14 - k]);
        M_vec[73 + k] = (INT16)(Z_out[9 - k]);
        M_vec[92 + k] = (INT16)(Z_out[4 - k]);
        k++;
    }


    while (spp_ExcorActive());
    // hwp_excor->ctrl_fast = Ctrl_FastM2;
    spp_CtrlFastRegSet(Ctrl_FastM2);
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic)<<1],(INT16*)&P_vec_neg[0],(INT16 *)Z_out, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[15 + k] = (INT16)(Z_out0[24 - k]);
        M_vec[37 + k] = (INT16)(Z_out0[19 - k]);
        M_vec[58 + k] = (INT16)(Z_out0[14 - k]);
        M_vec[78 + k] = (INT16)(Z_out0[9 - k]);
        M_vec[97 + k] = (INT16)(Z_out0[4 - k]);
        k++;
    }


    while (spp_ExcorActive());
    // hwp_excor->ctrl_fast = Ctrl_FastM1;
    spp_CtrlFastRegSet(Ctrl_FastM1);
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic+ Y1_Start_Pos)<<1] ,(INT16 *)Z_out0, Ctrl_M1);
    k = 0;
    for(j = 0; j < 3; j++)
    {
        M_vec[20 + k] = (INT16)(Z_out[14 - k]);
        M_vec[42 + k] = (INT16)(Z_out[11 - k]);
        M_vec[63 + k] = (INT16)(Z_out[8 - k]);
        M_vec[83 + k] = (INT16)(Z_out[5 - k]);
        M_vec[102 + k] = (INT16)(Z_out[2 - k]);
        k++;
    }


    ////////////////////////////////////////////////////////////////////
    while (spp_ExcorActive());
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic) ] ,(INT16 *)Z_out, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[105 + k] = (INT16)(Z_out0[24-k]);
        M_vec[122 + k] = (INT16)(Z_out0[19-k]);
        M_vec[138 + k] = (INT16)(Z_out0[14-k]);
        M_vec[153 + k] = (INT16)(Z_out0[9-k]);
        M_vec[167 + k] = (INT16)(Z_out0[4-k]);
        k++;
    }

    while (spp_ExcorActive());
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)],(INT16 *)Z_out0, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[110 + k] = (INT16)(Z_out[24-k]);
        M_vec[127 + k] = (INT16)(Z_out[19-k]);
        M_vec[143 + k] = (INT16)(Z_out[14-k]);
        M_vec[158 + k] = (INT16)(Z_out[9-k]);
        M_vec[172 + k] = (INT16)(Z_out[4-k]);
        k++;
    }

    while (spp_ExcorActive());
    // hwp_excor->ctrl_fast = Ctrl_FastM2;
    spp_CtrlFastRegSet(Ctrl_FastM2);
    spal_XcorFastMatrixMultiplyInit((INT16*)&RxBuffer[(K1-N1-Lf + Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&P_vec_neg[0],(INT16 *)Z_out, Ctrl_M1);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[115 + k] = (INT16)(Z_out0[24-k]);
        M_vec[132 + k] = (INT16)(Z_out0[19-k]);
        M_vec[148 + k] = (INT16)(Z_out0[14-k]);
        M_vec[163 + k] = (INT16)(Z_out0[9-k]);
        M_vec[177 + k] = (INT16)(Z_out0[4-k]);
        k++;
    }

    while (spp_ExcorActive());
    // hwp_excor->ctrl_fast = Ctrl_FastM1;
    spp_CtrlFastRegSet(Ctrl_FastM1);
    spal_XcorFastMatrixMultiplyInit((INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic)], (INT16*)&R_imag_vec[(K1-N1-Lf + Pos0_saic)] ,(INT16 *)Z_out0, Ctrl_M1);

    k = 0;
    for(j = 0; j < 3; j++)
    {
        M_vec[120 + k] = (INT16)(Z_out[14-k]);
        M_vec[137 + k] = (INT16)(Z_out[11-k]);
        M_vec[153 + k] = (INT16)(Z_out[8-k]);
        M_vec[168 + k] = (INT16)(Z_out[5-k]);
        M_vec[182 + k] = (INT16)(Z_out[2-k]);
        k++;
    }

    ////////////////////////////////////////////////////////////////
    while (spp_ExcorActive());

    spal_XcorFastMatrixMultiplyInit((INT16*)&R_imag_vec[K1-N1-Lf + Pos0_saic ], (INT16*)&R_imag_vec[K1-N1-Lf + Pos0_saic + Y1_Start_Pos ] ,(INT16 *)Z_out, Ctrl_M1);


    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[185 + k] = (INT16)(Z_out0[24-k]);
        M_vec[197 + k] = (INT16)(Z_out0[19-k]);
        M_vec[208 + k] = (INT16)(Z_out0[14-k]);
        M_vec[218 + k] = (INT16)(Z_out0[9-k]);
        M_vec[227 + k] = (INT16)(Z_out0[4-k]);
        k++;
    }

    while (spp_ExcorActive());
    spal_XcorFastMatrixMultiplySet((INT16*)&R_imag_vec[K1-N1-Lf + Pos0_saic ], (INT16*)&P_vec_neg[0],(INT16 *)Z_out0, Lf_saic1 , Lb_saic1,  L_saic1, Y_SHIFT -1, 0);


    k = 0;
    for(j = 0; j < 5; j++)
    {
        M_vec[190 + k] = (INT16)(Z_out[24-k]);
        M_vec[202 + k] = (INT16)(Z_out[19-k]);
        M_vec[213 + k] = (INT16)(Z_out[14-k]);
        M_vec[223 + k] = (INT16)(Z_out[9-k]);
        M_vec[232 + k] = (INT16)(Z_out[4-k]);
        k++;
    }

    while (spp_ExcorActive());

    spal_XcorFastMatrixMultiplySet((INT16*)&R_imag_vec[K1-N1-Lf + Pos0_saic + Y1_Start_Pos], (INT16*)&R_imag_vec[K1-N1-Lf + Pos0_saic + Y1_Start_Pos] ,(INT16 *)Z_out, Lf_saic1 , Lf_saic1,  L_saic1, Y_SHIFT -1, 0);

    k = 0;
    for(j = 0; j < 3; j++)
    {
        M_vec[195 + k] = (INT16)(Z_out0[14-k]);
        M_vec[207 + k] = (INT16)(Z_out0[11-k]);
        M_vec[218 + k] = (INT16)(Z_out0[8-k]);
        M_vec[228 + k] = (INT16)(Z_out0[5-k]);
        M_vec[237 + k] = (INT16)(Z_out0[2-k]);
        k++;
    }

    ///////////////////////////////////////////////////////////////
    while (spp_ExcorActive());

    spal_XcorFastMatrixMultiplySet((INT16*)&R_imag_vec[K1-N1-Lf + Pos0_saic + Y1_Start_Pos], (INT16*)&P_vec_neg[0],(INT16 *)Z_out0, Lf_saic1 , Lb_saic1,    L_saic1, Y_SHIFT -1, 0);

    k = 0;
    for(j = 0; j < 5; j++)
    {
        // different from others to make sure the former data will be covered by latter data which is correct.
        M_vec[240 + k] = (INT16)(Z_out[24 - k]);
        M_vec[247 + k] = (INT16)(Z_out[19 - k]);
        M_vec[253 + k] = (INT16)(Z_out[14 - k]);
        M_vec[258 + k] = (INT16)(Z_out[9 - k]);
        M_vec[262 + k] = (INT16)(Z_out[4 - k]);
        k++;
    }

    while (spp_ExcorActive());

    k = 0;
    for(j = 0; j < 3; j++)
    {
        M_vec[245 + k] = (INT16)(Z_out0[14-k]);
        M_vec[252 + k] = (INT16)(Z_out0[11-k]);
        M_vec[258 + k] = (INT16)(Z_out0[8-k]);
        M_vec[263 + k] = (INT16)(Z_out0[5-k]);
        M_vec[267 + k] = (INT16)(Z_out0[2-k]);
        M_vec[270 + k] = (((INT16)g_sppSaicTable[Tsc][k]) << (Y_SHIFT + 1));
        M_vec[272 + k] = (((INT16)g_sppSaicTable[Tsc][3 + k]) << (Y_SHIFT + 1));
        M_vec[273 + k] = (((INT16)g_sppSaicTable[Tsc][6 + k]) << (Y_SHIFT + 1));

        k++;
    }

    ///////////////////////////////////////////////////////////////////////

    k = 0;
    for( i = L_saic1; i > 0; i--)
    {
        M_vec[k] = (M_vec[k]  > (32767 - DIAG_FAC1)) ? (32767 - DIAG_FAC1) : M_vec[k];
        M_vec[k] = M_vec[k] + DIAG_FAC1;
        k = k + i;
    }


    //////////////////////////////////////////////////////////////////////////////
    spal_CholkCgStep1((UINT32*) M_vec , (UINT32 *)P_xcor,  L_saic1,  138,   g_sppGlobalParams.CgIternum, (UINT32*) Y_out);
    /////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    //S(H)*S
    M_vecstep2 = M_vecstep2 + 0;
    spal_XcorFastMatrixMultiply((INT16*)&RxBuffer[Pos0_saic<<1], (INT16*)&RxBuffer[Pos0_saic<<1] , (INT16 *)Z_out, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT+1, 0);
    spal_XcorFastMatrixMultiplySet((INT16*)&RxBuffer[Pos0_saic<<1],(INT16*)&RxBuffer[(Pos0_saic + Y1_Start_Pos)<<1] ,(INT16 *)Z_out0, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT+1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[k] = (INT16)(Z_out[35-k]);
        M_vecstep2[23 + k] = (INT16)(Z_out[29-k]);
        M_vecstep2[45 + k] = (INT16)(Z_out[23-k]);
        M_vecstep2[66 + k] = (INT16)(Z_out[17-k]);
        M_vecstep2[86 + k] = (INT16)(Z_out[11-k]);
        M_vecstep2[105 + k] = (INT16)(Z_out[5-k]);
        k++;
    }
    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&RxBuffer[Pos0_saic<<1],(INT16*)&R_imag_vec[ Pos0_saic ],(INT16 *)Z_out, Lf_saic2 , Lf_saic2,    N2, Y_SHIFT +1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[6 + k] = (INT16)(Z_out0[35-k]);
        M_vecstep2[29 + k] = (INT16)(Z_out0[29-k]);
        M_vecstep2[51 + k] = (INT16)(Z_out0[23-k]);
        M_vecstep2[72 + k] = (INT16)(Z_out0[17-k]);
        M_vecstep2[92 + k] = (INT16)(Z_out0[11-k]);
        M_vecstep2[111 + k] = (INT16)(Z_out0[5-k]);
        k++;
    }
    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&RxBuffer[Pos0_saic<<1],(INT16*)&R_imag_vec[ Pos0_saic + Y1_Start_Pos],(INT16 *)Z_out0, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT+1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[12 + k] = (INT16)(Z_out[35-k]);
        M_vecstep2[35 + k] = (INT16)(Z_out[29-k]);
        M_vecstep2[57 + k] = (INT16)(Z_out[23-k]);
        M_vecstep2[78 + k] = (INT16)(Z_out[17-k]);
        M_vecstep2[98 + k] = (INT16)(Z_out[11-k]);
        M_vecstep2[117 + k] = (INT16)(Z_out[5-k]);
        k++;
    }
    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&RxBuffer[(Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&RxBuffer[(Pos0_saic+ Y1_Start_Pos)<<1] ,(INT16 *)Z_out, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT + 1, 0);

    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[18 + k] = (INT16)(Z_out0[35-k]);
        M_vecstep2[41 + k] = (INT16)(Z_out0[29-k]);
        M_vecstep2[63 + k] = (INT16)(Z_out0[23-k]);
        M_vecstep2[84 + k] = (INT16)(Z_out0[17-k]);
        M_vecstep2[104 + k] = (INT16)(Z_out0[11-k]);
        M_vecstep2[123 + k] = (INT16)(Z_out0[5-k]);
        k++;
    }

    while (spp_ExcorActive()) ;
    ////////////////////////////////////////////////////////////////////
    spal_XcorFastMatrixMultiplySet((INT16*)&RxBuffer[ (Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&R_imag_vec[ Pos0_saic ] ,(INT16 *)Z_out0, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT +1, 0);

    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[129 + k] = (INT16)(Z_out[35-k]);
        M_vecstep2[146 + k] = (INT16)(Z_out[29-k]);
        M_vecstep2[162 + k] = (INT16)(Z_out[23-k]);
        M_vecstep2[177 + k] = (INT16)(Z_out[17-k]);
        M_vecstep2[191 + k] = (INT16)(Z_out[11-k]);
        M_vecstep2[204 + k] = (INT16)(Z_out[5-k]);
        k++;
    }

    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&RxBuffer[(Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&R_imag_vec[ Pos0_saic + Y1_Start_Pos],(INT16 *)Z_out, Lf_saic2 , Lf_saic2,    N2, Y_SHIFT +1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[135 + k] = (INT16)(Z_out0[35-k]);
        M_vecstep2[152 + k] = (INT16)(Z_out0[29-k]);
        M_vecstep2[168 + k] = (INT16)(Z_out0[23-k]);
        M_vecstep2[183 + k] = (INT16)(Z_out0[17-k]);
        M_vecstep2[197 + k] = (INT16)(Z_out0[11-k]);
        M_vecstep2[210 + k] = (INT16)(Z_out0[5-k]);
        k++;
    }
    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&R_imag_vec[ Pos0_saic ], (INT16*)&R_imag_vec[ Pos0_saic] ,(INT16 *)Z_out0, Lf_saic2 , Lf_saic2, N2, Y_SHIFT +1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[141 + k] = (INT16)(Z_out[35-k]);
        M_vecstep2[158 + k] = (INT16)(Z_out[29-k]);
        M_vecstep2[174 + k] = (INT16)(Z_out[23-k]);
        M_vecstep2[189 + k] = (INT16)(Z_out[17-k]);
        M_vecstep2[203 + k] = (INT16)(Z_out[11-k]);
        M_vecstep2[216 + k] = (INT16)(Z_out[5-k]);
        k++;
    }

    ////////////////////////////////////////////////////////////////
    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&R_imag_vec[ Pos0_saic ], (INT16*)&R_imag_vec[ Pos0_saic + Y1_Start_Pos ] ,(INT16 *)Z_out, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT+1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[222 + k] = (INT16)(Z_out0[35-k]);
        M_vecstep2[233 + k] = (INT16)(Z_out0[29-k]);
        M_vecstep2[243 + k] = (INT16)(Z_out0[23-k]);
        M_vecstep2[252 + k] = (INT16)(Z_out0[17-k]);
        M_vecstep2[260 + k] = (INT16)(Z_out0[11-k]);
        M_vecstep2[267 + k] = (INT16)(Z_out0[5-k]);
        k++;
    }
    while (spp_ExcorActive()) ;

    spal_XcorFastMatrixMultiplySet((INT16*)&R_imag_vec[ Pos0_saic + Y1_Start_Pos ], (INT16*)&R_imag_vec[ Pos0_saic + Y1_Start_Pos] ,(INT16 *)Z_out0, Lf_saic2 , Lf_saic2,  N2, Y_SHIFT + 1, 0);
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[228 + k] = (INT16)(Z_out[35-k]);
        M_vecstep2[239 + k] = (INT16)(Z_out[29-k]);
        M_vecstep2[249 + k] = (INT16)(Z_out[23-k]);
        M_vecstep2[258 + k] = (INT16)(Z_out[17-k]);
        M_vecstep2[266 + k] = (INT16)(Z_out[11-k]);
        M_vecstep2[273 + k] = (INT16)(Z_out[5-k]);
        k++;
    }

    ///////////////////////////////////////////////////////////////

    while (spp_ExcorActive()) ;
    k = 0;
    for (j = 0; j < 6; j++)
    {
        M_vecstep2[279 + k] = (INT16)(Z_out0[35-k]);
        M_vecstep2[284 + k] = (INT16)(Z_out0[29-k]);
        M_vecstep2[288 + k] = (INT16)(Z_out0[23-k]);
        M_vecstep2[291 + k] = (INT16)(Z_out0[17-k]);
        M_vecstep2[293 + k] = (INT16)(Z_out0[11-k]);
        M_vecstep2[294 + k] = (INT16)(Z_out0[5-k]);
        k++;
    }

    ///////////////////////////////////////////////////////////////////////
    k = 0;
    for(i = L_saic2; i > 0; i--)
    {
        M_vecstep2[k] = (M_vecstep2[k]  > (32767 - DIAG_FAC2)) ? (32767 - DIAG_FAC2) : M_vecstep2[k];
        M_vecstep2[k] = M_vecstep2[k] + DIAG_FAC2;
        k = k + i;
    }

    /////////////////////////////////////////////////////////////////////////
    spal_CholkCgStep2((UINT32*) M_vec , (UINT32 *)P_xcor,  L_saic1,  138,   g_sppGlobalParams.CgIternum,(UINT32*) Y_out);

    for(i = 0; i < 20; i++)
    {
        Z_vec[i] = (UINT32)((INT16)(Y_out[((19 -i)<<1) + 1])& 0x0000FFFF);
    }

    Z_vec[20] = (UINT32)(Y_out[41]& 0x0000FFFF);
    Z_vec[21] = (UINT32)(Y_out[43]& 0x0000FFFF);
    Z_vec[22] = (UINT32)(Y_out[45]& 0x0000FFFF);




    //  hwp_excor->ctrl_fast = Ctrl_FastConv1;
    spp_CtrlFastRegSet(Ctrl_FastConv1);

    spal_XcorFastConvolutionSet ( (INT16*)&RxBuffer[1<<1], (INT16*) &Z_vec[Lf_saic1*3],  (INT16*) M_out,  Ctrl_Conv1);

    spal_XcorFastConvolutionSet ( (INT16*)&RxBuffer[(1 + Y1_Start_Pos)<<1], (INT16*) &Z_vec[Lf_saic1<<1],  (INT16*) M_vec1,  Ctrl_Conv1);

    for(i = 0; i < 148; i += 4)
    {
        Y_out[i]   = M_vec1[2*i + 4] + (M_out[2*i + 4]);
        Y_out[i+1]    = M_vec1[2*i+6] + (M_out[2*i+6]);
        Y_out[i+2]    = M_vec1[2*i+8] + (M_out[2*i+8]);
        Y_out[i+3]    = M_vec1[2*i+10] + (M_out[2*i+10]);
    }

    spal_XcorFastConvolutionInit( (INT16*)&R_imag_vec[1], (INT16*) &Z_vec[Lf_saic1],    (INT16*) M_out, Ctrl_Conv1);

    Out[148] = 0;
    Out[149] = 0;
    Out[150] = 0;
    Out[151] = 0;
    //////////////////////////////////////////////////////////////
    Tap[0]  = (1 <<Y_SHIFT);
    s_fac = Y_SHIFT - (NCE_SHIFT_DIV);
    if(s_fac > 0)
    {
        Tap[1]  =  (UINT32)((((INT16)(Z_vec[20])) << s_fac)& 0x0000FFFF);
        Tap[2]  =  (UINT32)((((INT16)(Z_vec[21])) << s_fac)& 0x0000FFFF);
        Tap[3]  =  (UINT32)((((INT16)(Z_vec[22])) << s_fac)& 0x0000FFFF);
    }
    else
    {
        s_fac = -s_fac;
        Tap[1]  =  (UINT32)((((INT16)(Z_vec[20])) >> s_fac)& 0x0000FFFF);
        Tap[2]  =  (UINT32)((((INT16)(Z_vec[21])) >> s_fac)& 0x0000FFFF);
        Tap[3]  =  (UINT32)((((INT16)(Z_vec[22])) >> s_fac)& 0x0000FFFF);
    }
    Tap[Lb] = 0;
    while (spp_ExcorActive()) ;

    spal_XcorFastConvolutionSet ( (INT16*)&R_imag_vec[1 + Y1_Start_Pos], (INT16*) &Z_vec[0], (INT16*) M_vec1,  Ctrl_Conv1);

    for(i = 0; i < 148; i+=4)
    {
        Out[i]     = ((Y_out[i] + M_vec1[2*(i+ 2)] + M_out[2*(i+ 2)])& 0x0000FFFF );
        Out[i+1]  = ((Y_out[i + 1] + M_vec1[2*(i+ 3)] + M_out[2*(i+ 3)])& 0x0000FFFF );
        Out[i+2]  = ((Y_out[i + 2] + M_vec1[2*(i+ 4)] + M_out[2*(i+ 4)])& 0x0000FFFF );
        Out[i+3]  = ((Y_out[i + 3] + M_vec1[2*(i+ 5)] + M_out[2*(i+ 5)])& 0x0000FFFF );
    }

}
//=============================================================================
// spp_SaicProstep2
//-----------------------------------------------------------------------------
// Performs saic process step2
// @param RxBuffer UINT16*. Address of the 2x received samples.
// @param R_imag_vec UINT32*. Address of the 2x received samples,imag parts.
// @param M_vec INT16*. M matrix(S(H)*S),it is used for saic process2.
// @param Tsc UINT8,Training Sequence index.
// @param Y_SHIFT UINT8, Shift parameter.
// @param sv UINT8*. soft value of vitac output.
// @param sv_out UINT8*.soft value of saic process2.
//=============================================================================
PUBLIC VOID  spp_SaicProstep2(INT16* RxBuffer, UINT32* R_imag_vec, INT16* M_vec, UINT8 Tsc, UINT8 Y_SHIFT, UINT8* sv, UINT8* sv_out)
{
    UINT16 i;
    UINT32 P_vec[148];
    UINT32 Z_vec[36];
    UINT32 *Z_out;
    INT16  P_xcor[L_saic2];
    INT16  M_out_cache[306];
    INT16  M_vec1_cache[306];
    INT16  Y_out_ref[153];
    UINT8 tempsv[60];
    UINT8 tmp;
    UINT16 softtmp;
    INT16* M_out;
    INT16* M_vec1;
    INT16* Y_out;
    UINT32 Fac1;
    UINT32 Fac2;
    UINT32 Ctrl_FastP1;
    UINT32 Ctrl_P1;
    UINT32 Ctrl_FastConv2;
    UINT32 Ctrl_Conv2;
    Z_out =  MEM_ACCESS_UNCACHED(Z_vec);
    Y_out =  (INT16*)MEM_ACCESS_UNCACHED(Y_out_ref);
    M_out = (INT16 *)MEM_ACCESS_UNCACHED(M_out_cache);
    M_vec1 = (INT16 *)MEM_ACCESS_UNCACHED(M_vec1_cache);


    Ctrl_FastP1 = EXCOR_LOOP_NUM_A(Lf_saic2)|EXCOR_LOOP_NUM_B(1)|EXCOR_LOOP_NUM_AB(N2)|((0&0xFF)<< 16);
    Ctrl_P1 = EXCOR_CMD(EXCOR_FASTMATRIMUL) |EXCOR_SHIFT_BIT(Y_SHIFT+3) ;
    Ctrl_FastConv2 =   (3&0xFF)<< 16 ;
    Ctrl_Conv2 =  EXCOR_CMD(EXCOR_FASTCONV) |EXCOR_NB_ILOOP(Lf_saic2) | EXCOR_NB_OLOOP(153) |EXCOR_SHIFT_BIT(Y_SHIFT+1) ;

    Fac1 = (UINT32)(((-1)<< Y_SHIFT)& 0x0000FFFF);
    Fac2 = (UINT32)(((1)<< Y_SHIFT)& 0x0000FFFF);

    for(i = 0; i < 56; i+=8)
    {
        P_vec[i] = (sv[i + 3] >128) ? (Fac1) : (Fac2);
        P_vec[i+1] = (sv[i + 4] >128) ? (Fac1) : (Fac2);
        P_vec[i+2] = (sv[i + 5] >128) ? (Fac1) : (Fac2);
        P_vec[i+3] = (sv[i + 6] >128) ? (Fac1) : (Fac2);
        P_vec[i+4] = (sv[i + 7] >128) ? (Fac1) : (Fac2);
        P_vec[i+5] = (sv[i + 8] >128) ? (Fac1) : (Fac2);
        P_vec[i+6] = (sv[i + 9] >128) ? (Fac1) : (Fac2);
        P_vec[i+7] = (sv[i + 10] >128) ? (Fac1) : (Fac2);

        P_vec[i + 85] = (sv[i + 88] >128) ? (Fac1) : (Fac2);
        P_vec[i + 86] = (sv[i + 89] >128) ? (Fac1) : (Fac2);
        P_vec[i + 87] = (sv[i + 90] >128) ? (Fac1) : (Fac2);
        P_vec[i + 88] = (sv[i + 91] >128) ? (Fac1) : (Fac2);
        P_vec[i + 89] = (sv[i + 92] >128) ? (Fac1) : (Fac2);
        P_vec[i + 90] = (sv[i + 93] >128) ? (Fac1) : (Fac2);
        P_vec[i + 91] = (sv[i + 94] >128) ? (Fac1) : (Fac2);
        P_vec[i + 92] = (sv[i + 95] >128) ? (Fac1) : (Fac2);
    }
    P_vec[56] = (sv[59] >128) ? (Fac1) : (Fac2);
    P_vec[141] = (sv[144] >128) ? (Fac1) : (Fac2);
    P_vec[57] = (sv[60] >128) ? (Fac1) : (Fac2);
    P_vec[84] = (sv[87] >128) ? (Fac1) : (Fac2);

    for(i = 0; i < 26; i++)
    {
        P_vec[i + 58] = ((1- (((INT16)((g_sppGsmTscNburst[Tsc]>> i) & 0x00000001)) << 1)) << Y_SHIFT) & (0x0000FFFF);
    }

    ////////////////////////////////////////////////
    // S(H)*x0
    //hwp_excor->ctrl_fast = Ctrl_FastP1;
    spp_CtrlFastRegSet(Ctrl_FastP1);
    spal_XcorFastMatrixMultiplyS((INT16*)&RxBuffer[Pos0_saic<<1], (INT16*)&P_vec[0] ,(INT16 *)&Z_out[(Lf_2x + 1) * 3], Ctrl_P1);

    spal_XcorFastMatrixMultiplyS((INT16*)&RxBuffer[(Pos0_saic + Y1_Start_Pos)<<1], (INT16*)&P_vec[0] ,(INT16 *)&Z_out[(Lf_2x + 1) << 1], Ctrl_P1);

    spal_XcorFastMatrixMultiplyS((INT16*)&R_imag_vec[(Pos0_saic)], (INT16*)&P_vec[0] ,(INT16 *)&Z_out[(Lf_2x + 1)], Ctrl_P1);

    spal_XcorFastMatrixMultiplyS((INT16*)&R_imag_vec[(Pos0_saic + Y1_Start_Pos)], (INT16*)&P_vec[0] ,(INT16 *)&Z_out[0], Ctrl_P1);

    for(i = 0; i < L_saic2; i ++)
    {
        P_xcor[i] = (INT16)(Z_out[23 -i]);
        P_xcor[i] = (P_xcor[i]  > 4095) ? 4095 : P_xcor[i];
        P_xcor[i] = (P_xcor[i]  < -4096) ? -4096 : P_xcor[i];
    }


    spal_CholkCg((UINT32*)&M_vec[0] , (UINT32 *)P_xcor,  L_saic2,  150,  g_sppGlobalParams.CgIternum, (UINT32*)Y_out);

    for(i = 0; i < L_saic2; i++)
    {
        Z_out[i] = (((INT16)(Y_out[((23 -i)<<1) + 1]))&(0x0000FFFF));
    }
    /////////////////
    //  hwp_excor->ctrl_fast = Ctrl_FastConv2;
    spp_CtrlFastRegSet(Ctrl_FastConv2);

    spal_XcorFastConvolutionSet ( (INT16*)&RxBuffer[0], (INT16*) &Z_out[Lf_saic2*3],  (INT16*) M_out,  Ctrl_Conv2);

    spal_XcorFastConvolutionSet ( (INT16*)&RxBuffer[( Y1_Start_Pos)<<1], (INT16*) &Z_out[Lf_saic2<<1],  (INT16*) M_vec1,  Ctrl_Conv2);

    M_vec1 = M_vec1 + 10;
    M_out = M_out + 10;
    for(i = 0; i < 148; i += 4)
    {
        Y_out[i]   = M_vec1[i<<1] + (M_out[i<<1]);
        Y_out[i+1]   = M_vec1[(i<<1)+2] + (M_out[(i<<1)+2]);
        Y_out[i+2]   = M_vec1[(i<<1)+4] + (M_out[(i<<1)+4]);
        Y_out[i+3]   = M_vec1[(i<<1)+6] + (M_out[(i<<1)+6]);
    }
    M_vec1 = M_vec1 - 10;
    M_out = M_out - 10;

    spal_XcorFastConvolutionSet ( (INT16*)&R_imag_vec[0], (INT16*) &Z_out[Lf_saic2],  (INT16*) M_out,  Ctrl_Conv2);

    spal_XcorFastConvolutionSet ( (INT16*)&R_imag_vec[0 + Y1_Start_Pos], (INT16*) &Z_out[0], (INT16*) M_vec1,  Ctrl_Conv2);

    M_vec1 = M_vec1 + 10;
    M_out = M_out + 10;

    for(i = 0; i < 148; i += 4)
    {
        Y_out[i]   = (Y_out[i] + M_vec1[i<<1] + M_out[i<<1]);
        Y_out[i+1]   = (Y_out[i+1] + M_vec1[(i<<1)+2] + M_out[(i<<1)+2]);
        Y_out[i+2]   = (Y_out[i+2] + M_vec1[(i<<1)+4] + M_out[(i<<1)+4]);
        Y_out[i+3]   = (Y_out[i+3] + M_vec1[(i<<1)+6] + M_out[(i<<1)+6]);
    }
    M_vec1 = M_vec1 - 10;
    M_out = M_out - 10;
    //////////////////////////////////////////
    //For bpsk,llr can be represented as y(i)
    M_vec1 = (INT16*)&Y_out[0];

    for(i = 0; i < 116; i++)
    {
        M_vec1[150 + i] = (i < 58) ? (M_vec1[i]) : (M_vec1[i + 26]);
        if(M_vec1[150 + i] >= 0)
        {
            sv_out[i] = (M_vec1[150 + i] > 127) ? (127) : (M_vec1[150 + i]);
        }
        else
        {
            M_vec1[150 + i] = (M_vec1[150 + i]  < -127) ? (-127) : (M_vec1[150 + i]);
            softtmp = (UINT16)M_vec1[150 + i];
            softtmp &= 0xFF;
            tmp =(UINT8)(~softtmp);
            tmp += 0x81;
            sv_out[i] = tmp;
        }
    }

    for( i = 0; i < 58; i +=2 )
    {
        tempsv[i] = sv_out[i + 57];
        tempsv[i + 1] = sv_out[i + 58];
    }
    tempsv[58] = sv_out[115];

    for(i = 0; i < 56; i +=4)
    {
        sv_out[i + 57] = tempsv[i + 2];
        sv_out[i + 58] = tempsv[i + 3];
        sv_out[i + 59] = tempsv[i + 4];
        sv_out[i + 60] = tempsv[i + 5];
    }
    sv_out[113] = tempsv[58];
    sv_out[114] = tempsv[0];
    sv_out[115] = tempsv[1];
}
#else
//=============================================================================
// spp_Equalize
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================
PUBLIC VOID spp_Equalize(UINT8 WinType,
                         SPP_EQU_BURST_T* burst,
                         SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    INT16 H[4];
    int i,survivor;
    SPAL_VITAC_EQU_PARAM_T equ_param;
    UINT32 SumTable[2*2*8/2];
    volatile UINT8  *path_diff_bkw;
    volatile UINT8  *path_diff_fwd;
    volatile UINT16 *PathMetric;
    UINT8* tb_wr_ptr = &(burst->sv[1]);
    UINT8   tb_start,tb_end,tb_idx,tb_size;
    INT16* taps = (INT16*) burst->taps;
    SPAL_DC_OFFSET_T Dco;
    BOOL fwd_vitac_done = FALSE;

#define BM_SHIFT_MAX    17

    SPP_PROFILE_FUNCTION_ENTRY(spp_Equalize);
    // ***********************************
    // Derive the VITAC shifters settings
    // ***********************************
    equ_param.shift_bm = spp_Log2(burst->noise);
    if (equ_param.shift_bm>SHIFT_BM_OFFSET) equ_param.shift_bm -= SHIFT_BM_OFFSET;

    equ_param.shift_sb = SHIFT_SB_OFFSET;

    if (equ_param.shift_bm>BM_SHIFT_MAX)
    {
        equ_param.shift_sb += (equ_param.shift_bm-BM_SHIFT_MAX);
        equ_param.shift_bm=BM_SHIFT_MAX;
    }
    else if (equ_param.shift_bm<4) equ_param.shift_bm = 4;
    if (equ_param.shift_sb>9) equ_param.shift_sb = 9;


    // ***********************************
    // Precalculation of Sum terms for BMs
    // ***********************************
    // The branch metric for a given node can be precalculated
    // since the register state is directly linked to the node.
    spal_XcorBmsum(taps, (INT16*)SumTable);

    // Compute H0 and HL for Viterbi Accelerator
    H[0] = taps[0];
    H[1] = taps[1];
    H[2] = taps[SPAL_DELAY*2];
    H[3] = taps[SPAL_DELAY*2+1];

    // Saturation on SYMB_SIZE bits signed
    for (i=0; i<4; i++)
    {
        H[i] = (H[i] > SYMB_MAX) ? SYMB_MAX : H[i];
        H[i] = (H[i] < SYMB_MIN) ? SYMB_MIN : H[i];
    }

    equ_param.H0 = (UINT32)((H[1]<<16)|((0xFFFF)&H[0]));
    equ_param.HL = (UINT32)((H[3]<<16)|((0xFFFF)&H[2]));
    equ_param.bmSum     = (INT16*)SumTable;

    // Setup for first half-burst equalization (backward)
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        equ_param.length = 3 + NB_HPAYLD_SIZE + 1;
        equ_param.in     = (burst->samples +3 +NB_TS_START +SPAL_DELAY -1 -2);
        equ_param.out    =
            (UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->NbVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->NbVitac.PathMetric;
        equ_param.first_state = g_sppNbBwequStartSt[burst->tsc];
    }
    else // SCH
    {
        equ_param.length = 3 + SCH_HPAYLD_SIZE;
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SPAL_DELAY -1 -2);
        equ_param.out    =
            (UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->SchVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->SchVitac.PathMetric;
        equ_param.first_state = SCH_BWEQU_START_ST;
    }

    // Run the viterbi for the backward part of the burst (1st half)
    spal_VitacEqualize(BACKWARD_TRELLIS, &equ_param);

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of backward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get the first payload bit
    // Due to Vitac architecture (PM double buffering), the actual
    // PM position depends on nb_symbol parity.
    PathMetric = (equ_param.length & 0x1) ? PathMetric + VIT_NSTATES:
                 PathMetric;
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;

        diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        burst->sv[0]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;

        diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        burst->sv[0]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // ***************************************************
    // Launching HW Viterbi Trellis processing of the 2nd
    // burst during the Trace-back of the first half-burst
    // if we are not in EQU_HBURST_MODE
    // ***************************************************
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        // Setup for second half-burst equalization (forward)
        // No change needed for the other parameters
        equ_param.in     = (burst->samples +3 +NB_TS_START +NB_TS_SIZE -2);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = g_sppNbFwequStartSt[burst->tsc];

        if (!EQU_HBURST_MODE)
        {
            // Run the viterbi now only in non-HalfBurst mode
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
        }
        // Start now even in EQU_BURST_MODE if the full Nburst
        // has been received
        else if (spp_FullBurstReady() && (WinType == GSM_WIN_TYPE_NBURST))
        {
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);
            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flags
            spp_OneFullBurstDone();
            fwd_vitac_done = TRUE;
        }
    }
    else // SCH
    {
        // SCH doesn't support HalfBurst mode
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SCH_TS_SIZE -2);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = SCH_FWEQU_START_ST;
        // Run the viterbi
        spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
    }


    // ****************************************
    // Back to the trace-back of 1st half-burst
    // ****************************************
    //  Setup the traceback loop bounds
    //  Note that for NB stealing bits are handled separately
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        tb_start = (56 + 1) + SPAL_DELAY - 1; // +1 for steal bit shift
        tb_end   = ( 0 + 1) + SPAL_DELAY - 1;
        tb_size  = 2*57; // steal bits not accounted for
    }
    else //SCH
    {
        tb_start = 38 + SPAL_DELAY - 1;
        tb_end   =  0 + SPAL_DELAY - 1;
        tb_size  = 2*39;
    }

    // Move traceback pointer to beginning of first part of payload
    tb_wr_ptr = &(burst->sv[1]);

    // Get the remaining payload bits
    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_bkw[VIT_NSTATES*(tb_idx) + survivor]);

        if (hard8(*tb_wr_ptr++))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the first stealing bit (hl) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[114] = (path_diff_bkw[VIT_NSTATES*SPAL_DELAY+survivor]);
    //-------------------------------------------------------------------

    // In case of EQU_HBURST_MODE we need to wait for the samples of
    // the second half burst to be ready
    if (EQU_HBURST_MODE && (!fwd_vitac_done))
    {
        if (WinType == GSM_WIN_TYPE_NBURST)
        {
            // Wait for EndOfBurst Interrupt
            while(!spp_FullBurstReady());
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);

            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flag
            spp_OneFullBurstDone();
        }
    }

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of forward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get first bit of payload
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;

        diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        burst->sv[tb_size-1]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;

        diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        burst->sv[tb_size-1]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // Move traceback pointer to the end of second half of payload
    tb_wr_ptr = &(burst->sv[tb_size - 2]);

    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_fwd[VIT_NSTATES*tb_idx + survivor]);

        if (hard8(*tb_wr_ptr--))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the second stealing bit (hu) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[115] = (path_diff_fwd[VIT_NSTATES*SPAL_DELAY+survivor]);

    SPP_PROFILE_FUNCTION_EXIT(spp_Equalize);
}


#endif
//=============================================================================
// spp_BuildRefNBurst
//-----------------------------------------------------------------------------
/// Rebuilds the theoretical received normal burst
/// from the hard bits decisions and the estimated channel taps.
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
INLINE VOID spp_BuildRefNBurst(SPP_EQU_BURST_T* burst)
{
    UINT32 hard_refBurst_tab[4];
    UINT32* hard_refBurst;
    UINT32 tmp_hardRef[2];

    hard_refBurst = MEM_ACCESS_UNCACHED(hard_refBurst_tab);

    // First extract hard values from equalized output
    // 116 SoftValues => 29 words
    spal_XcorHvextract( burst->sv,
                        (UINT8) 29,
                        hard_refBurst);

    // Build the first part of the burst
    // add the 2 needed tail bits ...
    tmp_hardRef[0] = hard_refBurst[0] << 2;
    // ...to the first 57bits of payload...
    tmp_hardRef[1] = (hard_refBurst[1] << 2)|(hard_refBurst[0] >> 30);
    // ...mask...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32-(57+2-32));
    // ...and add the first stealing flag...
    tmp_hardRef[1] |= (hard8(burst->sv[114]) >> 7 ) << (57+2-32);
    // ...and the beginning of the TS
    tmp_hardRef[1] |= (g_sppGsmTscNburst[burst->tsc]) << (57+2-32+1);

    // Reconstruct reference symbols of first part of the burst:
    // 57bits payload + 1 stealing flag + 2 TS bits not done during snr calc.
    spal_XcorSrec((INT16*)burst->taps, // channel taps
                  (NB_HPAYLD_SIZE+1+2), // number of symbols
                  tmp_hardRef, // hardbit reference
                  &(burst->ref[0]),// output
                  SPAL_ISI); // channel tap number

    // Build the second part of the burst
    // add the 4 last TS bits
    tmp_hardRef[0] = (g_sppGsmTscNburst[burst->tsc]) >> (NB_TS_SIZE-4);
    // add the second stealing flag
    tmp_hardRef[0] |= (hard8(burst->sv[115])>>7) << 4;
    // ...to the second 57bits of payload...
    tmp_hardRef[0] |= (hard_refBurst[1] >> (NB_HPAYLD_SIZE-32-5)) & 0xFFFFFFE0;
    tmp_hardRef[0] |= hard_refBurst[2] << (64-NB_HPAYLD_SIZE+5);
    tmp_hardRef[1] = (hard_refBurst[2] >> (NB_HPAYLD_SIZE-32-5));
    tmp_hardRef[1] |= (hard_refBurst[3] << (64-NB_HPAYLD_SIZE+5));
    // add mask for the 2 needed tail bits ...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32-(4+1+57-32));

    // Reconstruct reference symbols of second half part of the burst:
    spal_XcorSrec(   (INT16*)burst->taps, // channel taps
                     (NB_HPAYLD_SIZE+1+2), // number of symbols
                     tmp_hardRef, // hardbit reference
                     &(burst->ref[142-(NB_HPAYLD_SIZE+1+2)]),// output
                     SPAL_ISI); // channel tap number

}

//=============================================================================
// spp_BuildRefSCHburst
//-----------------------------------------------------------------------------
/// Rebuilds the theoretical received SCH burst
/// from the hard bits decisions and the estimated channel taps.
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
INLINE VOID spp_BuildRefSCHburst(SPP_EQU_BURST_T* burst)
{
    UINT32 hard_refBurst_tab[3];
    UINT32* hard_refBurst;
    UINT32 tmp_hardRef[2];

    hard_refBurst = MEM_ACCESS_UNCACHED(hard_refBurst_tab);

    // First extract hard values from equalized output
    // 78 SoftValues => 20 words
    spal_XcorHvextract( burst->sv,
                        (UINT8)20,
                        hard_refBurst);

    // Build the first part of the burst
    // add the 2 needed tail bits ...
    tmp_hardRef[0] = hard_refBurst[0] << 2;
    // ...to the first 39bits of payload...
    tmp_hardRef[1] = (hard_refBurst[1] << 2)|(hard_refBurst[0] >> 30);
    // ...mask...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32-(39+2-32));
    // ...and the beginning of the TS
    tmp_hardRef[1] |= (GSM_TSC_SCH0) << (39+2-32);// changed by tcj @20100818

    // Reconstruct reference symbols of first part of the burst:
    // 39bits payload + 2 TS bits not done during snr calc.
    spal_XcorSrec(   (INT16*)burst->taps, // channel taps
                     (SCH_HPAYLD_SIZE+2), // number of symbols
                     tmp_hardRef,  // hardbit reference
                     &(burst->ref[0]), // output
                     SPAL_ISI); // channel tap number

    // Build the second part of the burst
    // add the 4 last TS bits    // changed by tcj @20100818
    tmp_hardRef[0] = (GSM_TSC_SCH1) >> ((SCH_TS_SIZE-32)-4);
    // ...to the second 39bits of payload...
    tmp_hardRef[0] |= (hard_refBurst[1] >> (SCH_HPAYLD_SIZE-32-4)) & 0xFFFFFFF0;
    tmp_hardRef[0] |= hard_refBurst[2] << (64-SCH_HPAYLD_SIZE+4);
    tmp_hardRef[1] = (hard_refBurst[2] >> (SCH_HPAYLD_SIZE-32-4));
    // add mask for the 2 needed tail bits ...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32*2-SCH_HPAYLD_SIZE-4);

    // Reconstruct reference symbols of second half part of the burst:
    spal_XcorSrec(   (INT16*)burst->taps, // channel taps
                     (SCH_HPAYLD_SIZE+2), // number of symbols
                     tmp_hardRef, // hardbit reference
                     &(burst->ref[142-(SCH_HPAYLD_SIZE+2)]), // output
                     SPAL_ISI); // channel tap number
}

//=============================================================================
// spp_FofEstimation
//-----------------------------------------------------------------------------
/// Reconstruct the reference burst (softvalues), part of it
/// (training sequence) has already been done during SNR estimation.
///
/// Once the reference burst has been rebuilt, determines the Fof by
/// comparing the first FOF_NB_SYMB symbols of reference burst and
/// received burst, as well as the last FOF_NB_SYMB symbols of reference
/// burst and received burst.
///
/// The Fof is returned in Hertz.
///
/// @param WinType UINT8. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
PUBLIC VOID spp_FofEstimation(UINT8 WinType, SPP_EQU_BURST_T* burst)
{
    INT32 Icorr,divider;

    SPP_PROFILE_FUNCTION_ENTRY(spp_FofEstimation);
    // Reconstruct the reference burst (softvalues), part of it
    // (training sequence) has already been done during SNR estimation.
    // NOTE: It is not necessary to rebuild the full burst as we only
    // need FOF_NB_SYMB on each side to estimate FOF, but this is more
    // flexible and doesn't cost much thanks to the accelerators.
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        spp_BuildRefNBurst(burst);
    }
    else // SCH
    {
        spp_BuildRefSCHburst(burst);
    }


    // Calculate Imaginary part of correlation on start of burst
    // '+3' because tail bits are not present in refBurst
    Icorr = - spal_XcorIcorCalc(&(burst->samples[FOF_START_1+3]),
                                &(burst->ref[FOF_START_1]),
                                FOF_NB_SYMB);

    // Calculate Imaginary part of correlation on end of burst
    // '+3' because tail bits are not present in refBurst
    Icorr += spal_XcorIcorCalc( &(burst->samples[FOF_START_2+3]),
                                &(burst->ref[FOF_START_2]),
                                FOF_NB_SYMB);

    // FOf is given by:
    //
    //                           CorrI x Fs
    // FOF = _______________________________________________________
    //
    //       corr_gap x 2 PI x Norm(Power-NoisePower) x nb_symb_corr
    divider= 43104 / (FOF_CORR_GAP*FOF_NB_SYMB);// Fs/(2*PI)=43104
    divider=(INT32)((burst->power-burst->noise)/divider);
    if(divider)
        burst->fof = (INT16)(Icorr / divider);
    else
        burst->fof = 0;
    SPP_PROFILE_FUNCTION_EXIT(spp_FofEstimation);
}

//=============================================================================
// spp_EqSoftShift
//-----------------------------------------------------------------------------
/// Shift Equalizer's Output Based on SNR, if SNR<=0,clear softbits except sign bits
///
/// @param Snr INT8. input SNR
/// @param EqBufferOut UINT32*. equalizer's output ptr
///
//=============================================================================
PUBLIC VOID spp_EqSoftShift(INT8 Snr, UINT32* EqBufferOut)
{
    UINT8 Shft;
    UINT32 SignBits;
    UINT32 Msk;
    UINT32 i;
#if (SPC_IF_VER >= 5 && CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
    CONST UINT8 *eqShiftTable = (CONST UINT8*)g_sppGlobalParams.eqShiftTable;
#else
    CONST UINT8 eqShiftTable[EQ_SHIFT_MAX_NUM] =
    {
        // shift bits         snr
        5,              // 0
        3,              // 1
        3,              // 2
        2,              // 3
        1,              // 4
        1,              // 5
        0,              // 6
        0,              // 7
    };
#endif

    if (Snr < EQ_SHIFT_MAX_NUM)
    {
        Shft = eqShiftTable[Snr < 0 ? 0 : Snr];
        switch(Shft)
        {
            case 1:
                Msk = 0x7E7E7E7E;
                break;
            case 2:
                Msk = 0x7C7C7C7C;
                break;
            case 3:
                Msk = 0x78787878;
                break;
            case 4:
                Msk = 0x70707070;
                break;
            case 5:
                Msk = 0x60606060;
                break;
            case 6:
                Msk = 0x40404040;
                break;
            default:
                Msk = 0x0;
                break;
        }
        if (Shft)
        {
            for (i=0; i<29; i++)
            {
                SignBits = (EqBufferOut[i] & 0x80808080);
                EqBufferOut[i] &= Msk;
                EqBufferOut[i] >>= Shft;
                EqBufferOut[i] |= SignBits;
            }
        }
    }

    if ((Snr <= 0) && (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx == 4))
    {
        for (i=0; i<29; i++)
        {
            EqBufferOut[i] &= 0x80808080;
        }
    }
}

//=============================================================================
// spp_EqualizeNBurst
//-----------------------------------------------------------------------------
// Performs normal burst equalization including:
// - DC offset estimation and correction,
// - channel estimation,
// - SNR estimation,
// - Viterbi equalization and traceback,
// - Frequency Offset estimation.
// .
//
// @param RxBuffer UINT32*. Address of the received samples.
// @param Tsc UINT8. Training sequence number.
// @param EqBufferOut UINT32*.
// @param Res SPP_N_BURST_RESULT_T*. Pointer to the burst equalization structure.
//
//=============================================================================
PUBLIC VOID spp_EqualizeNBurst(UINT32* RxBuffer,
                               UINT8 Tsc,
                               UINT32* EqBufferOut,
                               SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
                               SPP_N_BURST_RESULT_T* Res)
{
    //UINT32 TapPwr=0;
    UINT32 refBurst[142]; // stores reconstructed burst

    // TODO : see if this structure can be used one level higher
    // to avoid passing too much parameters and keep structured datas.
    SPP_EQU_BURST_T burst; // equalisation structure
    SPAL_DC_OFFSET_T Dco;

    SPP_PROFILE_FUNCTION_ENTRY(spp_EqualizeNBurst);
    burst.tsc       = Tsc;
    burst.samples   = RxBuffer;
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)EqBufferOut;

#if ((defined CHIP_DIE_8955) && (SPC_IF_VER == 5))
    UINT32 dgcPwr = 0;
    UINT32 pwr = 0;
    UINT32 tap8[8] = {0};
    INT8 dgcShift = 0;
#endif

    // DC Offset Correction
    // --------------------
    if (EQU_HBURST_MODE)
    {
        // remove dc and derotation
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_HALF_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);


        burst.dco.i = Dco.I;
        burst.dco.q = Dco.Q;
        burst.power /= BB_BURST_HALF_SIZE; // Normalised power

    }
    else
    {
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_TOTAL_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);



        burst.dco.i = Dco.I;
        burst.dco.q = Dco.Q;

        burst.power /= BB_BURST_TOTAL_SIZE; // Normalised power
    }

#if ((defined CHIP_DIE_8955) && (SPC_IF_VER == 5))

    if(g_sppGlobalParams.DigitalAgc)
    {
        pwr = burst.power * BB_BURST_TOTAL_SIZE;
        dgcShift = spp_DigitalAgc(pwr, RxBuffer, &dgcPwr);
    }

    if (g_sppGlobalParams.EnhanceMode) // Enter the performance enhancement mode in lab condition.
    {
        if(g_sppGlobalParams.Ht100QrPrefilter)
        {
            burst.samples = burst.samples + 4;
            spal_XcorChest8taps(
                (INT16 *) (&burst.samples[63 + 8 - SPAL_ISI]), // format 3.9
                (INT16 *) (&g_spp_8TapChestTab[burst.tsc][0][0]),//format: Q(15)
                (INT16 *) tap8,// format: Q(LPCH)(i.e. 0.LPCH)
                8,
                15);
            spal_InvalidateDcache(); // clear cache
            burst.samples = burst.samples - 4;
            spp_ChannelEstimation8taps(&burst,tap8,UncachedBuffers);
            spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                           (NB_TS_SIZE-4), // Reconstruction length
                           (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                           &(burst.ref[NB_TS_START+2]), // reconstructed output
                           SPAL_ISI); // channel tap number

            // Run power of the error calculation upon Training Sequence
            // The three guard bits are not included in the reference (reconstructed) burst
            // if(g_spc_HT100_Procs_SchemeQR)
            burst.noise = spal_XcorErrCalc(
                              (UINT32*)&(burst.samples[NB_TS_START+3+2]), // original
                              (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                              (NB_TS_SIZE-4)); // Number of samples

            // Normalise NoisePower and calculate Snr
            burst.noise /= (NB_TS_SIZE-4);

            // Equalize : Viterbi + traceback
            // ------------------------------
            spp_Equalize(GSM_WIN_TYPE_NBURST,0,&burst, UncachedBuffers);
        }
        else
        {
            spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, &burst, UncachedBuffers);
            if (g_sppGlobalParams.TapsShorten)
            {
                spp_TapsShorten((SPAL_COMPLEX_T*)burst.taps);
            }
            // SNR Estimation
            // --------------
            // First reconstruct TrainingSequence (minus 2 tail symbols on each side)
            spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                           (NB_TS_SIZE-4), // Reconstruction length
                           (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                           &(burst.ref[NB_TS_START+2]), // reconstructed output
                           SPAL_ISI); // channel tap number

            // Run power of the error calculation upon Training Sequence
            // The three guard bits are not included in the reference (reconstructed) burst
            burst.noise = spal_XcorErrCalc(
                              (UINT32*)&(burst.samples[NB_TS_START+3+2]), // original
                              (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                              (NB_TS_SIZE-4)); // Number of samples

            // Normalise NoisePower and calculate Snr
            burst.noise /= (NB_TS_SIZE-4);

            // Equalize : Viterbi + traceback
            // ------------------------------
            spp_Equalize(GSM_WIN_TYPE_NBURST,0,&burst, UncachedBuffers);

            if (g_sppGlobalParams.ChannelTracking)
            {
                UINT32 hv[4];
                SPAL_COMPLEX_T data_orginal_all[58*2+26];
                SPAL_COMPLEX_T* data_orginal_xcor;

                spal_XcorHvextract(burst.sv, 29, hv);
                spal_InvalidateDcache(); // clear cache
                spp_HardDecisionToSym(hv, g_sppGsmTscNburst[burst.tsc], data_orginal_all);
                data_orginal_xcor = data_orginal_all + 39;
                spp_ChannelTracking(data_orginal_xcor, (SPAL_COMPLEX_T*)(burst.samples + 7 + 39 -2), (SPAL_COMPLEX_T*)burst.taps);
                if(g_sppGlobalParams.TapsShorten)
                {
                    spp_TapsShorten((SPAL_COMPLEX_T*)burst.taps);
                }

                spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                               (NB_TS_SIZE-4), // Reconstruction length
                               (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                               &(burst.ref[NB_TS_START+2]), // reconstructed output
                               SPAL_ISI); // channel tap number
                burst.noise = spal_XcorErrCalc(
                                  (UINT32*)&(burst.samples[NB_TS_START+3+2]), // original
                                  (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                                  (NB_TS_SIZE-4)); // Number of samples
                burst.noise /= (NB_TS_SIZE-4);
                spp_Equalize(GSM_WIN_TYPE_NBURST,0,&burst, UncachedBuffers);
            }
            if (g_sppGlobalParams.InterfernceDetect)
            {
                if (g_sppGlobalParams.DigitalAgc == FALSE)
                {
                    dgcPwr = burst.power;
                }
                g_spcCtxStruct.IsSaicEnable = spp_InterfereDetect(burst.samples, burst.ref, dgcPwr, burst.noise);
            }
        }
    }
    else
    {
        spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, &burst, UncachedBuffers);
        // SNR Estimation
        // --------------
        // First reconstruct TrainingSequence (minus 2 tail symbols on each side)
        spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                       (NB_TS_SIZE-4), // Reconstruction length
                       (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                       &(burst.ref[NB_TS_START+2]), // reconstructed output
                       SPAL_ISI); // channel tap number

        // Run power of the error calculation upon Training Sequence
        // The three guard bits are not included in the reference (reconstructed) burst
        burst.noise = spal_XcorErrCalc(
                          (UINT32*)&(burst.samples[NB_TS_START+3+2]), // original
                          (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                          (NB_TS_SIZE-4)); // Number of samples

        // Normalise NoisePower and calculate Snr
        burst.noise /= (NB_TS_SIZE-4);

        if (g_sppGlobalParams.InterfernceDetect)
        {
            if (g_sppGlobalParams.DigitalAgc == FALSE)
            {
                dgcPwr = burst.power;
            }
            g_spcCtxStruct.IsSaicEnable = spp_InterfereDetect(burst.samples, burst.ref, dgcPwr, burst.noise);
        }

        // Equalize : Viterbi + traceback
        // ------------------------------
        spp_Equalize(GSM_WIN_TYPE_NBURST,0,&burst, UncachedBuffers);

    }
#else
    spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, &burst, UncachedBuffers);
    // SNR Estimation
    // --------------
    // First reconstruct TrainingSequence (minus 2 tail symbols on each side)
    spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                   (NB_TS_SIZE-4), // Reconstruction length
                   (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                   &(burst.ref[NB_TS_START+2]), // reconstructed output
                   SPAL_ISI); // channel tap number

    // Run power of the error calculation upon Training Sequence
    // The three guard bits are not included in the reference (reconstructed) burst
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[NB_TS_START+3+2]), // original
                      (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                      (NB_TS_SIZE-4)); // Number of samples

    // Normalise NoisePower and calculate Snr
    burst.noise /= (NB_TS_SIZE-4);

    // Equalize : Viterbi + traceback
    // ------------------------------

    spp_Equalize(GSM_WIN_TYPE_NBURST,&burst, UncachedBuffers);

#endif

    // Frequency error estimation
    // --------------------------
    spp_FofEstimation(GSM_WIN_TYPE_NBURST,&burst);

    // use the whole burst for snr re-calculation. burst.ref has been reconstruct in
    // spal_XcorSrec and spp_FofEstimation
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[3]), // original, 3 tail bits removed
                      (UINT32*)&(burst.ref[0]), // reference
                      142); // Number of samples. 58+26+58

    // Normalise NoisePower and calculate Snr
    burst.noise /= 142;
#if ((defined CHIP_DIE_8955) && (SPC_IF_VER == 5))
    if (g_sppGlobalParams.DigitalAgc == FALSE)
    {
        dgcPwr = burst.power;
    }

    if (dgcPwr > burst.noise)
    {
        Res->Snr =  spp_10log10(dgcPwr-burst.noise)-spp_10log10(burst.noise);
    }
    else
    {
        Res->Snr =- spp_10log10(burst.noise);
    }
#else
    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);
#endif

    // Eq output shift
    spp_EqSoftShift(Res->Snr, EqBufferOut);

    // Fill result structure
    Res->Pwr = spp_10log10(burst.power);
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;
    Res->Dco.i = burst.dco.i;
    Res->Dco.q = burst.dco.q;

    SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeNBurst);
}

//=============================================================================
// spp_EqualizeDecodeSch
//-----------------------------------------------------------------------------
// Performs SCH burst equalization including:
// - DC offset estimation and correction,
// - channel estimation,
// - SNR estimation,
// - Viterbi equalization and traceback,
// - Frequency Offset estimation.
// .
//
// @param RxBuffer UINT32*. Address of the received samples.
// @param Res SPP_SCH_RESULT_T*. Pointer to the burst equalization structure.
//
//=============================================================================
PUBLIC BOOL spp_EqualizeDecodeSch(UINT32* RxBuffer,
                                  SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
                                  SPP_SCH_RESULT_T* Res)
{
    UINT32 refBurst[142]; // stores reconstructed burst
    UINT32 equalized_out[20]; // stores 78 equalized softvalues
    UINT32 crccode[2];
    int i;
    UINT8 decoded_out_tab[5];
    UINT8 *decoded_out = (UINT8*)MEM_ACCESS_UNCACHED(decoded_out_tab);

    // TODO : see if this structure can be used one level higher
    // to avoid passing too much parameters and keep structured datas.
    SPP_EQU_BURST_T burst; // equalisation structure
    SPAL_DC_OFFSET_T Dco;
    SPAL_VITAC_DEC_CFG_T vitacCfg;

    SPP_PROFILE_FUNCTION_ENTRY(spp_EqualizeDecodeSch);

    burst.samples   = RxBuffer;
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)equalized_out;
    burst.tsc       = SCH_TSC_IDX;

    // DC Offset Correction
    // --------------------
    burst.power = spal_XcorDcoc((UINT32*)burst.samples,
                                BB_SCH_TOTAL_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                &Dco);
    burst.power /= BB_SCH_TOTAL_SIZE; // Normalised power

    // Channel Estimation Process
    // --------------------------
    spp_ChannelEstimation(GSM_WIN_TYPE_SCH, &burst, UncachedBuffers);

    // SNR Estimation
    // --------------
    // First reconstruct TrainingSequence (the 2 tail symbols on each side of the
    // TSC are not used as surrounding symbols are unknown)
    spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                   (SCH_TS_SIZE-4), // Reconstruction length
                   (UINT32*)g_sppGsmTscSch, // ref. hardbit sequence
                   &(burst.ref[SCH_TS_START+2]), // reconstructed output
                   SPAL_ISI); // channel tap number

    // Run Error Calculation upon Training Sequence
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[SCH_TS_START+3+2]), // original
                      (UINT32*)&(burst.ref[SCH_TS_START + 2]), // reference
                      (SCH_TS_SIZE-4)); // Number of samples

    // Normalise NoisePower and calculate Snr
    burst.noise /= (SCH_TS_SIZE-4);

    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise) -
                    spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);

    // Equalize : Viterbi + traceback
    // ------------------------------
#ifdef CHIP_DIE_8955
    spp_Equalize(GSM_WIN_TYPE_SCH,0,&burst, UncachedBuffers);
#else
    spp_Equalize(GSM_WIN_TYPE_SCH,&burst, UncachedBuffers);
#endif

    // Frequency error estimation
    // --------------------------
    spp_FofEstimation(GSM_WIN_TYPE_SCH,&burst);

    // Fill result structure
    Res->Pwr = spp_10log10(burst.power);
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;

    //-------------------------------------------------------------------

    // ******************************
    // Channel Decoding of SCH
    // ******************************

    vitacCfg.inSoftBit     = (UINT32*) burst.sv;
    vitacCfg.survivorMem   = (UINT32*) UncachedBuffers->DecodeVitac.SurvivorMEM;
    vitacCfg.punctTable    = (UINT32*) g_sppCsPuncturingTable[CS_SCH];
    vitacCfg.convPoly0     = g_sppVitacPolyres[g_sppVitacDecParam[CS_SCH].resPoly0];
    vitacCfg.convPoly1     = g_sppVitacPolyres[g_sppVitacDecParam[CS_SCH].resPoly1];
    vitacCfg.convPoly2     = g_sppVitacPolyres[g_sppVitacDecParam[CS_SCH].resPoly2];
    vitacCfg.vitacDecParam = g_sppVitacDecParam[CS_SCH];

    spal_VitacFullDecoding(&vitacCfg, (UINT32*)decoded_out);

    // Verify CRC
    spal_CapDecodeCrc(g_sppCsCrcParam[CS_SCH], (UINT32*)decoded_out, crccode);

    // Fill Result structure
    for(i=0; i<4; i++)
    {
        Res->Data[i] = decoded_out[i];
    }

    SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeDecodeSch);
    if (crccode[0]!=0x3FF)  // Failed
    {
        return FALSE;
    }
    return TRUE;
}

//======================================================================
/// spp_TofSearch
//----------------------------------------------------------------------
/// This function is used in order to calculate from the 13 correlations
/// of the ouput of spal_XcorCorrTsc(), the maximum power profile.
/// The channel is consider to be 5 taps long with the main tap in the middle.
///
/// @param in          UINT32*  INPUT.  Pointer to the input matrix.
/// @param pWindow     UINT16*  INPUT.  Pointer to a table containing the taps of the channel window.
/// @param search_size UINT8    INPUT.  Size to search
/// @param BurstTOF    UINT8*   OUTPUT. Address containing the calculated TOf in Quarter bits.
/// @param SampleTOF   UINT8*   OUTPUT. Address containing the calculated TOf per sample.
///
/// @author Nadia Touliou based on the code on the spp_ChannelEstimation().
/// @date 30/11/07
//======================================================================
PROTECTED VOID spp_TofSearch(     UINT32* in,
                                  UINT16* pWindow,
                                  UINT8   search_size,
                                  UINT8*  BurstTOF,
                                  UINT8*  SampleTOF)
{

    UINT8 i;
    UINT32 CorrAccu [SCH_TSC_XCOR_NBR - SPAL_ISI + 1]= {0};

    UINT8  TOf = 0;
    UINT32 max_accu = 0;

    for(i=0; i<search_size; i++)
    {
        register UINT32 accu;

        accu  = pWindow[0]*in[i] >>3;
        accu += pWindow[1]*in[i+1] >>3;
        accu += pWindow[2]*in[i+2] >>3;
        accu += pWindow[1]*in[i+3] >>3;
        accu += pWindow[0]*in[i+4] >>3;
        CorrAccu[i]=accu;

        if (accu>=max_accu)
        {
            max_accu = accu;
            TOf = i;
        }
    }

    *SampleTOF = TOf;

    // Besides recentering the received buffer, we report the
    // TOF to the stack to recenter the next receive window.
    // The stack requires the TOF in quarter of a sample.
    //
    // parabolic interpolation for half-symbol precision:
    //                 (Corr[TOf+1]-Corr[TOf-1])
    // dTOf = ________________________________________________
    //        (2*(Corr[TOf]-Corr[TOf+1]+Corr[TOf]-Corr[TOf-1]))
    if( (TOf != 0) && (TOf < (search_size-1)))
    {
        INT8 delta=0;
        INT32 diff1=( CorrAccu[TOf+1]-CorrAccu[TOf-1])<<1;
        INT32 diff2=(max_accu-CorrAccu[TOf+1])+(max_accu-CorrAccu[TOf-1]);
        // changed by tcj @20110519
        //diff1 += diff2 >> 1; // +0.5 for rounding
        if (diff1>0)
        {
            diff1 += diff2 >> 1; // +0.5 for rounding
        }
        else
        {
            diff1 -= diff2 >> 1; // neg +0.5 for rounding
        }
        // end by tcj

        if (diff2>0)
            delta=(INT8)(diff1/diff2);
        // Store result (in Quarterbit)
        *BurstTOF = (UINT8)((INT8)(4*TOf)+delta);
    }
    else
    {
        // Store result (in Quarterbit)
        *BurstTOF = 4*TOf;
    }
}


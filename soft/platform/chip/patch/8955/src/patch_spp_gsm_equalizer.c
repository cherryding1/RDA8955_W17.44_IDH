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
#include "spal_mem.h"

#ifdef CHIP_DIE_8955
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
#include "sppp_equalizer_private.h"

#include "spc_ctx.h"


#ifdef ENABLE_PATCH_SPP_EQUALIZENBURST
extern SPC_CONTEXT_T g_spcCtxStruct;
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
PUBLIC VOID SPAL_BBSRAM_PATCH_TEXT patch_spp_EqualizeNBurst(UINT32* RxBuffer,
                               UINT8 Tsc,
                               UINT32* EqBufferOut,
                               SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
                               SPP_N_BURST_RESULT_T* Res)
{
    //UINT32 TapPwr=0;
#define DC_UP_THRED (1900)
#define DC_DOWN_THRED (-1900)   
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
    UINT32 pwrOrg = 0;
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
        pwrOrg = burst.power;
        burst.power = dgcPwr;
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
                g_spcCtxStruct.IsSaicEnable = spp_InterfereDetect(burst.samples, burst.ref, burst.power, burst.noise);
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
            g_spcCtxStruct.IsSaicEnable = spp_InterfereDetect(burst.samples, burst.ref, burst.power, burst.noise);
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

    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);

    // Eq output shift
    spp_EqSoftShift(Res->Snr, EqBufferOut);

    // Fill result structure
    if(g_sppGlobalParams.DigitalAgc)
    {
        Res->Pwr = spp_10log10(pwrOrg);
    }
    else
    {
        Res->Pwr = spp_10log10(burst.power);
    }
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;
    Res->Dco.i = burst.dco.i;
    Res->Dco.q = burst.dco.q;

    if (Res->Pwr > 60)
    {
        Res->TOf = 16;
    }	
    if ((Res->Dco.i >= DC_UP_THRED) || (Res->Dco.i <= DC_DOWN_THRED) || (Res->Dco.q >= DC_UP_THRED) || (Res->Dco.q <= DC_DOWN_THRED))
    {
        Res->Snr = 0;
    }
    SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeNBurst);
}


//=============================================================================
// patch_spp_EqualizeNBurst
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
PUBLIC VOID SPAL_BBSRAM_PATCH_TEXT patch_spp_EqualizeNBurstSaic(UINT32* RxBuffer,
        UINT8 Tsc,
        UINT32* EqBufferOut,
        UINT32* EqBufferOutSaic,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
        SPP_N_BURST_RESULT_T* Res)
{
#define DC_UP_THRED (1900)
#define DC_DOWN_THRED (-1900)
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
#if 0
    if (g_sppGlobalParams.InterfernceDetect)
    {
        g_spcCtxStruct.IsSaicEnable = spp_InterfereDetect(burst.samples, burst.ref, burst.power, burst.noise);
    }
#endif
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

    if (Res->Pwr > 60)
    {
        Res->TOf = 16;
    }
	

    if ((Res->Dco.i >= DC_UP_THRED) || (Res->Dco.i <= DC_DOWN_THRED) || (Res->Dco.q >= DC_UP_THRED) || (Res->Dco.q <= DC_DOWN_THRED))
    {
        Res->Snr = 0;
    }

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
    for(i = 0; i < BB_BURST_ACTIVE_SIZE; i++)
    {
        R_imag_vec[i] = RxBuffer[1 + burst.samples_offset + i];
        R_imag_vec[i+156] = RxBuffer[159 + burst.samples_offset + i];
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
    for(i = 0; i < BB_BURST_ACTIVE_SIZE; i ++)
    {
        RxBuffer[i+5] = R_imag_vec[i];
        RxBuffer[i+163] = R_imag_vec[i + 156];
    }

    for(i = 0; i < 316; i += 4)
    {
        R_imag_vec[i]    = (unsigned int)(Rx_ptr[(i<<1) + 1]);
        R_imag_vec[i+1] = (unsigned int)(Rx_ptr[(i<<1) + 3]);
        R_imag_vec[i+2] = (unsigned int)(Rx_ptr[(i<<1) + 5]);
        R_imag_vec[i+3] = (unsigned int)(Rx_ptr[(i<<1) + 7]);
    }

    burst.samples   = refBurst;

    // Y_SHIFT = (spp_Log2(pwr) >> 1)  - 0;
    Y_SHIFT = (spp_Log2(pwr) >> 1)  + 1;

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

#endif
#if    (defined(ENABLE_PATCH_SPC_SCHRXPROCESS)&&defined(ENABLE_PATCH_SPC_IRQHANDLER))
UINT8 SPAL_INTSRAM_PATCH_DATA g_sppSaicPMatrixTableSch[3] = {-1,-5,-1};
UINT8 SPAL_INTSRAM_PATCH_DATA g_sppSaicTableSch[9] = {23,-1,3,-1,23,-1,3,-1,23};
//=============================================================================
// patch_spp_EqualizeDecodeSch
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
PUBLIC BOOL SPAL_INTSRAM_PATCH_TEXT patch_spp_EqualizeDecodeSch(UINT32* RxBuffer,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
        SPP_SCH_RESULT_T* Res)
{
    UINT32 refBurst[142]; // stores reconstructed burst
    UINT32 equalized_out[20]; // stores 78 equalized softvalues
    UINT32 crccode[2];
    UINT32 power_tem;
    INT16 I_tem;
    INT16 Q_tem;
    INT8 dgcShift = 0;
    UINT32 R_imag_vec[316];
    INT16* Rx_ptr;
    UINT8  Y_SHIFT;
    UINT8  temsv[152];
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
    //burst.samples   = (UINT32*)&RxBuffer[2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE];
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)equalized_out;
    burst.tsc       = SCH_TSC_IDX;
    Rx_ptr = (INT16*)g_spcStaticBufTabSaic;

    // DC Offset Correction
    // --------------------
    power_tem = spal_XcorDcoc((UINT32*)burst.samples,
                              BB_SCH_TOTAL_SIZE,
                              TRUE, // Derotation
                              BYPASS_DCOC,
                              &Dco);
    I_tem = Dco.I;
    Q_tem = Dco.Q;
    burst.power = spal_XcorDcoc((UINT32*)&RxBuffer[2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE],
                                BB_SCH_TOTAL_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                &Dco);

    burst.power = (burst.power + power_tem)/2;
    Dco.I = (Dco.I + I_tem)/2;
    Dco.Q = (Dco.Q + Q_tem)/2;

    if (g_sppGlobalParams.RxBufferIndex)
    {
        burst.samples   = (UINT32 *)&RxBuffer[2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE];
    }
    else
    {
        burst.samples = (UINT32 *)&RxBuffer[0];
    }


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

    if (crccode[0]==0x3FF)
    {
        SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeDecodeSch);
        return TRUE;
    }
    else
    {
        power_tem = burst.power *BB_SCH_TOTAL_SIZE;
        if ( power_tem>0 )
        {
            while(power_tem < DIG_AMP_TARGET_PWR_SAIC)
            {
                power_tem <<= 2;
                dgcShift--;
            }
            while ( power_tem > (16*DIG_AMP_TARGET_PWR_SAIC/9) )
            {
                power_tem >>= 2;
                dgcShift++;
            }
            if (dgcShift)
            {
                spal_ExcorIQShift((UINT32 *)&RxBuffer[0],BB_SCH_TOTAL_SIZE,dgcShift);
                spal_ExcorIQShift((UINT32 *)&RxBuffer[2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE],BB_SCH_TOTAL_SIZE,dgcShift);
            }
        }

        power_tem = power_tem/BB_SCH_TOTAL_SIZE;

        for(i = 0; i < BB_BURST_ACTIVE_SIZE; i ++)
        {
            g_spcStaticBufTabSaic[i + 5] = RxBuffer[i + burst.samples_offset];
            g_spcStaticBufTabSaic[i + 163] = RxBuffer[2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE + i + burst.samples_offset];

            // g_spcStaticBufTabSaic[i + 5] = RxBuffer[i + 12];
            //g_spcStaticBufTabSaic[i + 163] = RxBuffer[2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE + i + 12];

        }

        for(i = 0; i < 4; i += 2)
        {
            g_spcStaticBufTabSaic[i] = 0;
            g_spcStaticBufTabSaic[i + 1] = 0;
            g_spcStaticBufTabSaic[i + 153] = 0;
            g_spcStaticBufTabSaic[i + 154] = 0;
            g_spcStaticBufTabSaic[i + 158] = 0;
            g_spcStaticBufTabSaic[i + 159] = 0;
            g_spcStaticBufTabSaic[i + 311] = 0;
            g_spcStaticBufTabSaic[i + 312] = 0;
        }
        g_spcStaticBufTabSaic[4] = 0;
        g_spcStaticBufTabSaic[157] = 0;
        g_spcStaticBufTabSaic[162] = 0;
        g_spcStaticBufTabSaic[315] = 0;

        for(i = 0; i < 316; i += 4)
        {
            R_imag_vec[i]    = (unsigned int)(Rx_ptr[(i<<1) + 1]);
            R_imag_vec[i+1] = (unsigned int)(Rx_ptr[(i<<1) + 3]);
            R_imag_vec[i+2] = (unsigned int)(Rx_ptr[(i<<1) + 5]);
            R_imag_vec[i+3] = (unsigned int)(Rx_ptr[(i<<1) + 7]);
        }


        Y_SHIFT = (spp_Log2(power_tem) >> 1)  + 1;

        patch_spp_SaicProSch((INT16*) g_spcStaticBufTabSaic, R_imag_vec, Y_SHIFT , burst.samples, burst.taps);

        spp_Equalize_saic(GSM_WIN_TYPE_NBURST,&burst, temsv,UncachedBuffers);

        for(i = 0; i < 39; i ++)
        {
            burst.sv[i] = temsv[i + 3];
            burst.sv[i + 39] = temsv[i + 3 +39 + 64];
        }

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
PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spp_SaicProSch(INT16* RxBuffer, UINT32* R_imag_vec, UINT8 Y_SHIFT,UINT32 * Out, UINT32* Tap)
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
    UINT8  GSM_SCH_TSC[64] = {1,0,1,1,1,0,0,1,0,1,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,0,1,0,1,1,0,1,0,1,0,0,0,1,0,1,0,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1};


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
        P_vec[i] = (UINT32)(((1 - 2*((INT16)(GSM_SCH_TSC[i + 19]))) << Y_SHIFT)& 0x0000FFFF);
        P_vec_neg[i] = (-P_vec[i]) & 0x0000FFFF;
    }

    ////////////////////////////////////////////////
    // hwp_excor->ctrl_fast = Ctrl_FastP1;
    spp_CtrlFastRegSet(Ctrl_FastP1);
    // S(H)*x0
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

    P_xcor[20] = ((INT16)g_sppSaicPMatrixTableSch[0]) << (Y_SHIFT - 1);
    P_xcor[21] = ((INT16)g_sppSaicPMatrixTableSch[1]) << (Y_SHIFT - 1);
    P_xcor[22] = ((INT16)g_sppSaicPMatrixTableSch[2]) << (Y_SHIFT - 1);

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
        M_vec[270 + k] = (((INT16)g_sppSaicTableSch[k]) << (Y_SHIFT + 1));
        M_vec[272 + k] = (((INT16)g_sppSaicTableSch[3 + k]) << (Y_SHIFT + 1));
        M_vec[273 + k] = (((INT16)g_sppSaicTableSch[6 + k]) << (Y_SHIFT + 1));
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


#endif


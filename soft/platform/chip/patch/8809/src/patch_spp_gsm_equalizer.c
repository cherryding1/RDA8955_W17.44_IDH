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

#ifdef ENABLE_PATCH_SPP_EQUALIZENBURST




//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

// include
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

// SPP public headers
#include "spp_profile_codes.h"
// SPP private headers
#include "sppp_cfg.h"
#include "sppp_gsm_private.h"
#include "sppp_debug.h"
#include "spc_ctx.h"
#include "xcor.h"

extern VOID spp_Equalize(UINT8 WinType, SPP_EQU_BURST_T* burst, SPP_UNCACHED_BUFFERS_T* UncachedBuffers);
extern VOID spp_FofEstimation(UINT8 WinType, SPP_EQU_BURST_T* burst);

#define     EQ_SHIFT_MAX_NUM        8

//#define SPAL_BBSRAM_PATCH_DATA   __attribute__((section(".bbsram_patch_data")))


UINT8 SPAL_BBSRAM_PATCH_DATA g_mailbox_spc2pal_win_rx_burstRes_pwr[MBOX_CTX_SNAP_QTY][4];
UINT8 SPAL_BBSRAM_PATCH_DATA g_mailbox_spc2pal_win_rx_burstRes_snr[MBOX_CTX_SNAP_QTY][4];
INT16 SPAL_INTSRAM_PATCH_UCDATA g_mailbox_spc2pal_win_rx_burstRes_dcI[MBOX_CTX_SNAP_QTY];
INT16 SPAL_INTSRAM_PATCH_UCDATA g_mailbox_spc2pal_win_rx_burstRes_dcQ[MBOX_CTX_SNAP_QTY];
UINT8 SPAL_BBSRAM_PATCH_DATA g_EqShiftTable[EQ_SHIFT_MAX_NUM] =
{
//   shift bits     snr
    5,              // 0
    3,              // 1
    3,              // 2
    2,              // 3
    1,              // 4
    1,              // 5
    0,              // 6
    0               // 7
};


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
PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spp_EqualizeNBurst(UINT32* RxBuffer,
        UINT8 Tsc,
        UINT32* EqBufferOut,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
        SPP_N_BURST_RESULT_T* Res)
{
#define DC_UP_THRED (1900)
#define DC_DOWN_THRED (-1900)
    UINT32 refBurst[142]; // stores reconstructed burst

    // TODO : see if this structure can be used one level higher
    // to avoid passing too much parameters and keep structured datas.
    SPP_EQU_BURST_T burst; // equalisation structure
    SPAL_DC_OFFSET_T Dco;

    //SPP_PROFILE_FUNCTION_ENTRY(spp_EqualizeNBurst);
    burst.tsc       = Tsc;
    burst.samples   = RxBuffer;
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)EqBufferOut;

#ifdef __MBOX_DEBUG__
    UINT8 snap = spal_ComregsGetSnap();
    // DEBUG STUB : use external samples
    if (g_mailbox.dbg[snap].req & MBOX_DBG_EXT_SAMPLES)
    {
        int i;
        for (i=0; i<BB_BURST_TOTAL_SIZE; i++)
            RxBuffer[i] = g_mailbox.dbg[snap].ext_samples[i];
        g_mailbox.dbg[snap].req &= ~MBOX_DBG_EXT_SAMPLES;
    }
    // DEBUG STUB : record incoming samples
    if (g_mailbox.dbg[snap].req & MBOX_DBG_REC_SAMPLES)
    {
        int i;
        UINT32 *tmp = MEM_ACCESS_UNCACHED(RxBuffer);
        for (i=0; i<BB_BURST_TOTAL_SIZE; i++)
            g_mailbox.dbg[snap].samples[i] = tmp[i];
        g_mailbox.dbg[snap].req &= ~MBOX_DBG_REC_SAMPLES;
    }
#endif

    // DC Offset Correction
    // --------------------
    if (EQU_HBURST_MODE)
    {
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_HALF_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);
        burst.dco.i=Dco.I;
        burst.dco.q=Dco.Q;
        burst.power /= BB_BURST_HALF_SIZE; // Normalised power
    }
    else
    {
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_TOTAL_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);
        burst.dco.i=Dco.I;
        burst.dco.q=Dco.Q;
        burst.power /= BB_BURST_TOTAL_SIZE; // Normalised power
    }

    // Channel Estimation Process
    // --------------------------
    patch_spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, &burst, UncachedBuffers);

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
    /*
        if (burst.power > burst.noise)
            Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
        else
            Res->Snr =- spp_10log10(burst.noise);
    */
    // Equalize : Viterbi + traceback
    // ------------------------------
    spp_Equalize(GSM_WIN_TYPE_NBURST,&burst, UncachedBuffers);

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
    {
        UINT8 Shft;
        UINT32 SignBits;
        UINT32 Msk;
        INT16 i;

        if (Res->Snr < EQ_SHIFT_MAX_NUM)
        {
            Shft = g_EqShiftTable[Res->Snr < 0 ? 0: Res->Snr];
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
    }




    // Fill result structure
    Res->Pwr = spp_10log10(burst.power);
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;
    Res->Dco.i = burst.dco.i;
    Res->Dco.q = burst.dco.q;

    if ((Res->Dco.i >= DC_UP_THRED) || (Res->Dco.i <= DC_DOWN_THRED) || (Res->Dco.q >= DC_UP_THRED) || (Res->Dco.q <= DC_DOWN_THRED))
    {
        Res->Snr = 0;
    }

    if ((Res->Snr <= 0) && (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx == 4))
    {
        UINT32 i;
        for (i=0; i<29; i++)
        {
            EqBufferOut[i] &= 0x80808080;
        }
    }

    if(g_spcCtx->sched_slot_count>0)
    {
        g_mailbox_spc2pal_win_rx_burstRes_pwr[g_spcCtx->currentSnap][g_spcCtx->sched_slot_count-1]= Res->Pwr;
        g_mailbox_spc2pal_win_rx_burstRes_snr[g_spcCtx->currentSnap][g_spcCtx->sched_slot_count-1]= (Res->Snr>0)?Res->Snr:0;
    }
    if (g_spcCtx->sched_slot_count == 1)
    {
        g_mailbox_spc2pal_win_rx_burstRes_dcI[g_spcCtx->currentSnap] = Res->Dco.i;
        g_mailbox_spc2pal_win_rx_burstRes_dcQ[g_spcCtx->currentSnap] = Res->Dco.q;
    }

    ///

#ifdef __MBOX_DEBUG__ // DEBUG STUB : record part
    if (g_mailbox.dbg[snap].req & MBOX_DBG_REC_REF)
    {
        int i;
        for (i=0; i<BB_BURST_TOTAL_SIZE; i++)
            g_mailbox.dbg[snap].ref[i] = burst.ref[i];
        g_mailbox.dbg[snap].req &= ~MBOX_DBG_REC_REF;
    }
    if (g_mailbox.dbg[snap].req & MBOX_DBG_REC_TAPS)
    {
        int i;
        for (i=0; i<5; i++)
            g_mailbox.dbg[snap].taps[i] = burst.taps[i];
        g_mailbox.dbg[snap].req &= ~MBOX_DBG_REC_TAPS;
    }
#endif
    //SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeNBurst);

}

PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spp_ChannelEstimation(UINT8 WinType,
        SPP_EQU_BURST_T* burst,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    UINT8 spp2stack_tof_sel = 0;
    UINT32 Xcorr_Energy[SCH_TSC_XCOR_NBR]; // Sized for SCH
    UINT8 search_size;
    int i;
    UINT8 TOf = 0;
    UINT32 tap8[8]= {0};
    UINT32 TapsPwr;
    UINT32 chPwr[8];
    UINT8 chPos=0;
    UINT32 maxPwr=0, sumPwr=0;
    UINT8  spp_HT100_entry=0;
    INT16 *taps = (INT16*)burst->taps;
    UINT16 tapWindow [3] = {6,7,8};
    volatile INT32 *Xcorr_I= UncachedBuffers->SchVitac.Xcorr_I;
    volatile INT32 *Xcorr_Q= UncachedBuffers->SchVitac.Xcorr_Q;
    EXPORT INT16  gc_sppChestTabEgprs_Ext_8809[8][8][19];

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

    burst->samples += TOf;


    burst->samples_offset = TOf; // Keep track of symbol offset for HBURST mode

    spal_XcorChest_8taps_Ext_8809(
        (INT16 *) (&burst->samples[63 + 8 - SPAL_ISI]), // format 3.9
        (INT16 *) (&gc_sppChestTabEgprs_Ext_8809[burst->tsc][0][0]),//format: Q(15)
        (INT16 *) tap8,// format: Q(LPCH)(i.e. 0.LPCH)
        8,
        15);
#ifdef GCF_TEST
    spp_HT100_entry = spp_8tapDetect_Ext_8809(tap8, &TapsPwr);
#else
    spp_HT100_entry = 1;
#endif
    if(spp_HT100_entry)
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


}
#endif



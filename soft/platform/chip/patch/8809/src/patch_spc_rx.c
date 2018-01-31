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
#include "gsm.h"
#include "baseband_defs.h"
#include "chip_id.h"
// SPC
#include "spc_ctx.h"
#include "spc_private.h"
#include "spc_mailbox.h"
#include "spcp_debug.h"
#include "spc_profile_codes.h"
// SPAL
#include "spal_comregs.h"
#include "spal_tcu.h"
#include "spal_ifc.h"
// CHIP
#include "global_macros.h"
#include "tcu.h"
#include "hal_mem_map.h"

#include "spal_mem.h"
#include "spal_irq.h"
#include "spal_rfif.h"


#ifdef ENABLE_PATCH_SPC_BFIFILTER

extern VOID spc_VocBfiFilter(SPC_RX_LOCALS_T *spRx);

//=============================================================================
// spc_BfiFilter()
//-----------------------------------------------------------------------------
/// This function adds a BFI to the speech frame when needed
//=============================================================================
PUBLIC VOID SPAL_FUNC_INTERNAL patch_spc_BfiFilter(VOID)
{
#define LOOPBACK_FSBFIBER2THRESH  0
#define LOOPBACK_FSBFIBERTHRESH 40
#define LOOPBACK_FSBFISNRTHRESH -8

#define LOOPBACK_HSBFIBER2THRESH  0
#define LOOPBACK_HSBFIBERTHRESH 30
#define LOOPBACK_HSBFISNRTHRESH 1
    register SPC_RX_LOCALS_T* spRx asm("$17");

    if (!((spRx->StealingResult)&&(spRx->burstIdx == 3)))
    {
        UINT8 LoopBackMode = 0;

        SPC_PROFILE_FUNCTION_ENTER(spc_BfiFilter);
        if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_D) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_E) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_F) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) )
        {
            LoopBackMode = 1;
        }
        // BFi Filter relevant for FS, HS and AMR
        // FS and HS
        if ((MODE(g_spcCtx->ChMode)==TCH_SPEECH)&&
                (SPEECH(g_spcCtx->ChMode)==FR_CODEC))
        {
            INT16 TotalBlockSnr = g_spcCtx->BlockSnR;
            INT8 SnrThreshold  = (LoopBackMode==0)?g_spcCtx->FsBfiSnrThresh:LOOPBACK_FSBFISNRTHRESH;
            UINT8 BerThreshold  = (LoopBackMode==0)?g_spcCtx->FsBfiBerThresh:LOOPBACK_FSBFIBERTHRESH;
            UINT8 Ber2Threshold  = (LoopBackMode==0)?g_spcCtx->FsBfiBer2Thresh:LOOPBACK_FSBFIBER2THRESH;

            if (RATE(g_spcCtx->ChMode)==FR_RATE)
            {
                TotalBlockSnr += g_spcCtx->LastBlockSnR;
            }
            else if (RATE(g_spcCtx->ChMode)==HR_RATE)
            {
                SnrThreshold  = (LoopBackMode==0)?g_spcCtx->HsBfiSnrThresh:LOOPBACK_HSBFISNRTHRESH;
                BerThreshold  = (LoopBackMode==0)?g_spcCtx->HsBfiBerThresh:LOOPBACK_HSBFIBERTHRESH;
                Ber2Threshold  = (LoopBackMode==0)?g_spcCtx->HsBfiBer2Thresh:LOOPBACK_HSBFIBER2THRESH;
            }

            if (((TotalBlockSnr < SnrThreshold)&&
                    (g_spcCtx->NBlock_result.BitError > Ber2Threshold)) ||
                    (g_spcCtx->NBlock_result.BitError > BerThreshold))
            {
                g_spcCtx->NBlock_result.Bfi = 1;
            }
        }
        // AMR
        else if ((MODE(g_spcCtx->ChMode)==TCH_SPEECH)&&
                 (SPEECH(g_spcCtx->ChMode)==AMR_CODEC))
        {
            INT16 TotalBlockSnr = g_spcCtx->BlockSnR;
            const UINT8 AFSBerThresh[8] = {61,60,60,60,59,59,59,50};
            const UINT8 AHSBerThresh[6] = {28,27,22,16,13,9};
            const UINT8 AFSBerThreshLoopback[8] = {77,76,76,76,75,75,75,70};
            const UINT8 AHSBerThreshLoopback[6] = {30,29,24,18,15,11};
            UINT8  *AMRBerThresh = (LoopBackMode==0)?(UINT8 *)AFSBerThresh:(UINT8 *)AFSBerThreshLoopback;

            INT8 SnrThreshold  = g_spcCtx->FsBfiSnrThresh;

            if (RATE(g_spcCtx->ChMode)==FR_RATE)
            {
                TotalBlockSnr += g_spcCtx->LastBlockSnR;
            }
            else if (RATE(g_spcCtx->ChMode)==HR_RATE)
            {
                AMRBerThresh = (LoopBackMode==0)?(UINT8 *)AHSBerThresh:(UINT8 *)AHSBerThreshLoopback;
                SnrThreshold  = g_spcCtx->HsBfiSnrThresh;
            }

            if ((TotalBlockSnr < SnrThreshold) &&
                    (g_spcCtx->NBlock_result.BitError>AMRBerThresh[g_spcCtx->RxCodecMode]))
            {
                g_spcCtx->NBlock_result.Bfi = 1;
            }

        }
        SPC_PROFILE_FUNCTION_EXIT(spc_BfiFilter);
    }

    g_spcCtx->LastBlockSnR = g_spcCtx->BlockSnR;


}

#endif // ENABLE_PATCH_SPC_BFIFILTER

#ifdef ENABLE_PATCH_CHECK_LOOP_MODE
//=============================================================================
// spc_CheckLoopMode()
//-----------------------------------------------------------------------------
/// This function close and open the TCH test loop mode
//=============================================================================
PUBLIC VOID SPAL_FUNC_INTERNAL patch_spc_CheckLoopMode(SPC_RX_LOCALS_T *spRx)
{
    UINT8 i;
    UINT32* BlockDecodedAdd=g_spcCtx->NBlockDataOut[0];
    SPC_PROFILE_FUNCTION_ENTER(spc_CheckLoopMode);
    // test for loop A, D, E and F
    if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_D) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_E) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_F) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) )
    {
        UINT8 zeros = 0;
        switch (g_mailbox.pal2spc.statWin.loopMode)
        {
            case MBOX_LOOP_CLOSE_A:
                if (g_spcCtx->NBlock_result.Bfi)
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_D:
                if ((spRx->voc_bfi) || (spRx->voc_ufi) || (spRx->StealingResult))
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_E:
                if (spRx->voc_sid == 0)
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_F:
                if ((spRx->voc_bfi) || (spRx->voc_ufi) || (spRx->StealingResult) || (spRx->voc_sid != 2))
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_I:
                if (spRx->StealingResult)
                {
                    zeros = 1;
                }
                else
                {
                    for (i = 0; i < 14; i++)
                    {
                        BlockDecodedAdd[i] = 0x55555555;
                    }
                }
                break;
            default:
                break;
        }
        if (zeros)
        {
            for (i = 0; i < 14; i++)
            {
                BlockDecodedAdd[i] = 0;
            }
        }
        else
        {
            if(g_mailbox.pal2spc.statWin.loopMode != MBOX_LOOP_CLOSE_I)
            {
                for (i = 0; i < 14; i++)
                {
                    BlockDecodedAdd[i] = spRx->BlockDecodedAdd[i];
                }
            }
        }
        g_spcCtx->loop_bufIdx = 0 ;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_CheckLoopMode);
}

#endif

#ifdef PATCH_SPC_CCHRXPROCESS_ENABLE

#define DUMMY_SNR_THRESHOLD (10)
#define rxslotNb sched_rx_slot_nb
#ifdef GCF_TEST
extern UINT8 g_mailbox_pal2spc_statWin_rx_Usf[4];
UINT8 SPAL_INTSRAM_PATCH_UCDATA rx_Usf_Decode[4] = {0xFF,0xFF,0xFF,0xFF};
#endif
UINT8 SPAL_INTSRAM_PATCH_UCDATA g_mailbox_pal2spc_statWin_rx_Usf_temp = 0xFF;
UINT8 SPAL_INTSRAM_PATCH_UCDATA g_rx_Usf_Decode_temp = 0xFF;
//=============================================================================
// spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
{
    // default value set for SACCH
    UINT32 *ItlvOutput=(UINT32*)(g_spcCtx->Malloc) ;
    // default value set for SACCH
    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT32 *EqualizerInput;
    UINT32 *EqualizerOutput;
    UINT8 BuffIdx = spRx->buffIdx+spRx->slotIdx;
    UINT8 i=0;

    SPC_PROFILE_FUNCTION_ENTER(spc_CCHRxProcess);

    if (g_spcCtx->DedicatedActive==FALSE)
    {
        SoftBitAddress=g_spcCtx->ItlvBufRxCtrl+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;
    }
    //----------------------------- EQUALIZE  ---------------------------------
    EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
    ////////////////////////////////////////////////////////
    // dump the input samples if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_EQU_IN)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerInput,
            312,
            SPC_DBG_DUMP_EQU_IN_MARKER);
    }
    ////////////////////////////////////////////////////////
    EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 32);

    spp_EqualizeNBurst(EqualizerInput,
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);


    g_spcCtx->Rx_q_CCH[spRx->slotIdx][ spRx->burstIdx << 1     ] =
        ((EqualizerOutput[28]) >> 16) & 0x0FF;
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
        ( EqualizerOutput[28]) >> 24;
    ////////////////////////////////////////////////////////
    // dump the output softbits if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_EQU_OUT)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerOutput,
            58,
            SPC_DBG_DUMP_EQU_OUT_MARKER);
    }
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    // check the dummy PCH burst detection is enabled
    if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_PCH_READY)
    {
        UINT8 result=0;
        result = spp_DetectDummyPCH(EqualizerOutput,
                                    spRx->burstIdx,
                                    ((g_mailbox.dummyPchThresh)&0xFF00)>>8,
                                    (g_mailbox.dummyPchThresh)&0xFF);

        if (g_spcCtx->NBurst_result.Snr <= DUMMY_SNR_THRESHOLD)
            result = SPP_DUMMY_PCH_NOT_DETECTED;

        g_mailbox.dummyPchResult = (UINT32) result;
        spal_ComregsSetIrq(MBOX_DSP_PCH_READY);
    }
    ////////////////////////////////////////////////////////

    //----------------------------- EQUALIZE  ---------------------------------
    //----------------------------  RESULTS  ----------------------------------
    if (spRx->slotIdx==0)
    {
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
            (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
            g_spcCtx->NBurst_result.FOf;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
            (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
            g_spcCtx->NBurst_result.Pwr;
    }
    //----------------------------  RESULTS  ----------------------------------
    //---------------------------- CIPHERING ----------------------------------
    if (g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);
    }
    //---------------------------- CIPHERING ----------------------------------
    //----------------------------  DECODE  -----------------------------------
    if (spRx->burstIdx == 3)
    {
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
            (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[1] = NULL;

        // initialize the output buffer to avoid bit error due to tail bits
        for(i=0; i<14; i++)
        {
            g_spcCtx->NBlockDataOut[BuffIdx][i]=0;
        }
        // initialize the block decoding output address
        spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[BuffIdx];

        spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxCtrl;
        // initialize the usfs
        if (spRx->slotIdx == 0)
        {
            g_mailbox.spc2pal.statWin.rx.decodedUsf[0]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[1]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[2]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[3]=0xFF;
        }
        // decode Coding Scheme for the current slot
        spp_DecodeStealingBits((UINT32 *) (g_spcCtx->Rx_q_CCH[spRx->slotIdx]),
                               1,
                               &(g_spcCtx->Rx_CS[spRx->slotIdx]),
                               0);
        // default value for SACCH
        spRx->Copy_ChMode = CCH_mode + (GSM_CS1<<8);

        //--------------------------  DEINTERLEAVE  ---------------------------
        if (g_spcCtx->DedicatedActive==FALSE)
        {
            if (spRx->slotIdx == 0)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     SPC_TMP_ITLV_RX_BUFF_OFFSET);
            }
            else if(spRx->slotIdx == g_spcCtx->rxslotNb-1)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->Malloc);
            }
            else
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     (spRx->slotIdx-1)*SPC_ITLV_RX_BUFF_OFFSET);
            }
        }
        spp_DeinterleaveBlock(SoftBitAddress,
                              ITLV_TYPE_1B,
                              0,
                              (UINT32*)ItlvOutput);
        //--------------------------  DEINTERLEAVE  ---------------------------
#ifdef GCF_TEST
        g_mailbox_pal2spc_statWin_rx_Usf_temp = g_mailbox_pal2spc_statWin_rx_Usf[spRx->slotIdx];
        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            spp_GetUSF((UINT32*)ItlvOutput,
                       g_spcCtx->Rx_CS[spRx->slotIdx]);
        rx_Usf_Decode[spRx->slotIdx] = g_rx_Usf_Decode_temp;
#else
        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            spp_GetUSF((UINT32*)ItlvOutput,
                       g_spcCtx->Rx_CS[spRx->slotIdx]);

#endif

        if(g_spcCtx->sched_slot_count==g_spcCtx->rxslotNb)
        {
            UINT8 k=0;

            // send an it to the XCPU if the last slot has been received
            if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_USF_READY)
            {
                spal_ComregsSetIrq(MBOX_DSP_USF_READY);
            }
            for (k=0; k<g_spcCtx->rxslotNb; k++)
            {
                BuffIdx = spRx->buffIdx+k;
                if (g_spcCtx->DedicatedActive==FALSE)
                {
                    if (k == 0)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             SPC_TMP_ITLV_RX_BUFF_OFFSET);
                    }
                    else if(k == g_spcCtx->rxslotNb-1)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->Malloc);
                    }
                    else
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             (k-1)*SPC_ITLV_RX_BUFF_OFFSET);
                    }
                    spRx->Copy_ChMode=CCH_mode +
                                      ((g_spcCtx->Rx_CS[k])<<8);
                    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
                        (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
                    spRx->BlockDecodedAdd=g_spcCtx->NBlockDataOut[BuffIdx];
                }

                ////////////////////////////////////////////////////////
                // dump the decoder input softbits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_IN)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)ItlvOutput,
                        228,
                        SPC_DBG_DUMP_DEC_IN_MARKER);
                }
                ////////////////////////////////////////////////////////

#ifdef GCF_TEST
                spp_DecodeBlock((UINT32*)ItlvOutput,
                                spRx->Copy_ChMode,
                                0,
                                0,
                                spRx->BlockDecodedAdd,
                                (rx_Usf_Decode[k])&0x7,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);

#else
                spp_DecodeBlock((UINT32*)ItlvOutput,
                                spRx->Copy_ChMode,
                                0,
                                0,
                                spRx->BlockDecodedAdd,
                                (g_mailbox.spc2pal.statWin.rx.decodedUsf[k])&0x7,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);

#endif
                ////////////////////////////////////////////////////////
                // dump the decoder output bits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_OUT)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)spRx->BlockDecodedAdd,
                        28,
                        SPC_DBG_DUMP_DEC_OUT_MARKER);
                }
                ////////////////////////////////////////////////////////

                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].meanBep =
                    g_spcCtx->NBlock_result.MeanBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cvBep =
                    g_spcCtx->NBlock_result.CvBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitError =
                    g_spcCtx->NBlock_result.BitError;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitTotal =
                    g_spcCtx->NBlock_result.BitTotal;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bfi =
                    g_spcCtx->NBlock_result.Bfi & 0x01;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                    g_spcCtx->Rx_CS[k];
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].sid =
                    g_spcCtx->NBlock_result.Voc_Sid |
                    (spRx->RxFrameType==SPP_RX_SID_UPDATE);


            }
            // reset the Rx Ctrl buffer to allow decoding with only two bursts
            for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrl[i]=0;
        }
    }
    //----------------------------  DECODE  -----------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_CCHRxProcess);
}
#endif




#ifdef ENABLE_PATCH_SPC_RXPROCESS

#if (CHIP_EDGE_SUPPORTED==1)
extern PROTECTED SPP_EGPRS_IR_CTX_T g_Egprs_Ir_Ctx; // __attribute__((section("BB_SPC_CTX_SECTION")));
extern PROTECTED UINT32 g_Egprs_Softbit_Buff[87*2]; // __attribute__((section("BB_SPC_CTX_SECTION")));
#endif

extern INT32 HAL_UNCACHED_DATA_INTERNAL g_stopBcpu[3];
extern UINT32 HAL_UNCACHED_DATA_INTERNAL g_winIdx;

VOID SPAL_FUNC_INTERNAL spc_HungBcpu(VOID)
{
    g_stopBcpu[g_spcCtx->currentSnap] = -1;
    spal_IrqEnterCriticalSection();
    spal_IfcDisableRf();
    while(1);
}

//=============================================================================
// spc_FcchProcess()
//-----------------------------------------------------------------------------
/// This function processes the FCCH samples for each interrupt
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_FcchProcess(VOID)
{
    SPC_PROFILE_FUNCTION_ENTER(spc_FcchProcess);

    if (g_spcCtx->FcchInit == 1)
    {
        g_spcCtx->Init_FCCH = TRUE;
        g_spcCtx->FcchFound = 0;
        g_spcCtx->FcchInit = 0;
    }


    if (g_spcCtx->FcchFound == 0)
    {
        SPP_FCCH_RETURN_T Res;
        // Search for FCCH
        Res = spp_SearchFcch( (UINT32*) &(g_spcStaticBufTab.RxIfcBuffer[0]),
                              g_spcCtx->Init_FCCH,
                              (g_spcCtx->DedicatedActive == TRUE) ? FCCH_SLICED
                              : FCCH_FULL,
                              &g_spcCtx->FCCH_Result);

        // An FCCH has been found, we stop the process
        if ( Res != FCCH_NOT_FOUND)
        {
            g_spcCtx->FcchFound = 1;
            // This could be risky: we might disable the IFC after it was started under interrupt
            // Disable IFC as we are done
            UINT32 critical_sec = spal_IrqEnterCriticalSection();
            // Disable IFC2 only if there is no other window programmed
            if ( (g_spcCtx->bcpuIrqCause&BB_IRQ_BB_IRQ_FRAME) == 0 ||
                    g_mailbox.pal2spc.win[spal_ComregsGetSnap()].rx.qty == 0)
            {
                spal_IfcDisableRf();
            }
            spal_IrqExitCriticalSection(critical_sec);

            // Send results only if FCCH detection is valid
            if (Res != FCCH_INVALID)
            {
                g_mailbox.spc2pal.statWin.rx.fcch.valid = TRUE;
                g_mailbox.spc2pal.statWin.rx.fcch.fof =
                    g_spcCtx->FCCH_Result.FOf;
                // We remove FcchPreOpening as this offset is hidden
                // from L1
                // TODO : more logical to do it in PAL ?
                g_mailbox.spc2pal.statWin.rx.fcch.tof =
                    (INT16) g_spcCtx->FCCH_Result.TOf // Found offset
                    - g_spcCtx->FcchPreOpening; // Window "pre-opening"
            }
        }
        g_spcCtx->Init_FCCH = FALSE;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_FcchProcess);
}

//=============================================================================
// spc_MonitoringProcess()
//-----------------------------------------------------------------------------
/// This function processes the monitoring results
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_MonitoringProcess(UINT8 rxCount, UINT8 winIdx)
{
    UINT32 Power;

    spp_GetMonPower(rxCount,(UINT32 *) &(g_spcStaticBufTab.MonIfcBuffer[rxCount*(g_spcCtx->MonWinSize)]),&Power);

    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.monRes[winIdx].power = Power;
}

//=============================================================================
// spc_SchProcess()
//-----------------------------------------------------------------------------
/// This function processes the Synchro Burst reception
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_SchProcess()
{
    if (spp_EqualizeDecodeSch((UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]),
                              (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                              &g_spcCtx->SCH_Result))
    {
        g_mailbox.spc2pal.statWin.rx.sch[0].data[0] = g_spcCtx->SCH_Result.Data[0];
        g_mailbox.spc2pal.statWin.rx.sch[0].data[1] = g_spcCtx->SCH_Result.Data[1];
        g_mailbox.spc2pal.statWin.rx.sch[0].data[2] = g_spcCtx->SCH_Result.Data[2];
        g_mailbox.spc2pal.statWin.rx.sch[0].data[3] = g_spcCtx->SCH_Result.Data[3];
        g_mailbox.spc2pal.statWin.rx.sch[0].valid = TRUE;
    }
    else
    {
        g_mailbox.spc2pal.statWin.rx.sch[0].valid = FALSE;
    }
    g_mailbox.spc2pal.statWin.rx.sch[0].tof  =
        (INT8) ((int) g_spcCtx->SCH_Result.TOf - 4*BB_RX_SCH_MARGIN);
    g_mailbox.spc2pal.statWin.rx.sch[0].fof  =        g_spcCtx->SCH_Result.FOf;
    g_mailbox.spc2pal.statWin.rx.sch[0].snr  =
        (UINT8) ((g_spcCtx->SCH_Result.Snr>=0)?g_spcCtx->SCH_Result.Snr:0);
    g_mailbox.spc2pal.statWin.rx.sch[0].power=(UINT8) g_spcCtx->SCH_Result.Pwr;
    spal_ComregsSetIrq(MBOX_DSP_SCH_DECODING_COMPLETE);
}

//=============================================================================
// spc_SetFacch()
//-----------------------------------------------------------------------------
/// This function is used to deinterleave FACCH block in CSD buffer
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_SetFacch(SPC_RX_LOCALS_T *spRx)
{
    UINT8 i,j,k,l,itlv;
    itlv=22;

    SPC_PROFILE_FUNCTION_ENTER(spc_SetFacch);

    if (g_spcCtx->RxBlockOffset >= 8)
    {
        k = g_spcCtx->RxBlockOffset - 8;
        l = 8;
    }
    else
    {
        k = g_spcCtx->RxBlockOffset + itlv - 8;
        l = itlv - k;
    }
    for (i=0; i<l; i++)
    {
        for (j=0; j<32; j++)
        {
            g_spcCtx->ItlvBufRxFacch[32*i+j] =
                g_spcCtx->ItlvBufRxDedicated[32*(k+i)+j];
        }
    }
    k = l;
    l = (l==8)? 0 : 8-l;
    for (i=0; i<l; i++)
    {
        for (j=0; j<32; j++)
        {
            g_spcCtx->ItlvBufRxFacch[32*(k+i)+j] =
                g_spcCtx->ItlvBufRxDedicated[32*i+j];
        }
    }
    spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxFacch;

    spRx->RxBuffOffset = 0;

    SPC_PROFILE_FUNCTION_EXIT(spc_SetFacch);
}

//=============================================================================
// spc_BfiFilter()
//-----------------------------------------------------------------------------
/// This function adds a BFI to the speech frame when needed
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_BfiFilter(VOID)
{
    SPC_PROFILE_FUNCTION_ENTER(spc_BfiFilter);
    // BFi Filter relevant for FS, HS and AMR
    // FS and HS
    if ((MODE(g_spcCtx->ChMode)==TCH_SPEECH)&&
            (SPEECH(g_spcCtx->ChMode)==FR_CODEC))
    {
        INT16 TotalBlockSnr = g_spcCtx->BlockSnR;
        INT8 SnrThreshold  = g_spcCtx->FsBfiSnrThresh;
        UINT8 BerThreshold  = g_spcCtx->FsBfiBerThresh;
        UINT8 Ber2Threshold  = g_spcCtx->FsBfiBer2Thresh;

        if (RATE(g_spcCtx->ChMode)==FR_RATE)
        {
            TotalBlockSnr += g_spcCtx->LastBlockSnR;
        }
        else if (RATE(g_spcCtx->ChMode)==HR_RATE)
        {
            SnrThreshold  = g_spcCtx->HsBfiSnrThresh;
            BerThreshold  = g_spcCtx->HsBfiBerThresh;
            Ber2Threshold  = g_spcCtx->HsBfiBer2Thresh;
        }

        if (((TotalBlockSnr < SnrThreshold)&&
                (g_spcCtx->NBlock_result.BitError > Ber2Threshold)) ||
                (g_spcCtx->NBlock_result.BitError > BerThreshold))
        {
            g_spcCtx->NBlock_result.Bfi = 1;
        }
    }
    // AMR
    else if ((MODE(g_spcCtx->ChMode)==TCH_SPEECH)&&
             (SPEECH(g_spcCtx->ChMode)==AMR_CODEC))
    {
        INT16 TotalBlockSnr = g_spcCtx->BlockSnR;
        const UINT8 AFSBerThresh[8] = {61,60,60,60,59,59,59,50};
        const UINT8 AHSBerThresh[6] = {28,27,22,16,13,9};
        UINT8  *AMRBerThresh = (UINT8 *)AFSBerThresh;
        INT8 SnrThreshold  = g_spcCtx->FsBfiSnrThresh;

        if (RATE(g_spcCtx->ChMode)==FR_RATE)
        {
            TotalBlockSnr += g_spcCtx->LastBlockSnR;
        }
        else if (RATE(g_spcCtx->ChMode)==HR_RATE)
        {
            AMRBerThresh = (UINT8 *)AHSBerThresh;
            SnrThreshold  = g_spcCtx->HsBfiSnrThresh;
        }
        if ((TotalBlockSnr < SnrThreshold)&&
                (g_spcCtx->NBlock_result.BitError>AMRBerThresh[g_spcCtx->RxCodecMode]))
        {
            g_spcCtx->NBlock_result.Bfi = 1;
        }
    }

    g_spcCtx->LastBlockSnR = g_spcCtx->BlockSnR;
    SPC_PROFILE_FUNCTION_EXIT(spc_BfiFilter);
}

//=============================================================================
// spc_VocBfiFilter()
//-----------------------------------------------------------------------------
/// This function implements a state machine for DTX downlink
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_VocBfiFilter(SPC_RX_LOCALS_T *spRx)
{
    UINT32 StealingSoftBits[2]= {0,0};
    UINT8 Facch_detected=0;
    UINT8 bfi_threshold;

    // set the bfi threshold corresponding to the codec
    // don't care about AMR since the bfi is not managed
    // the same way (done in AMR decode)
    if (RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        if (SPEECH(g_spcCtx->ChMode)==FR_CODEC)
            bfi_threshold = g_spcCtx->FR_BfiThreshold;
        else
            bfi_threshold = g_spcCtx->EFR_BfiThreshold;
    }
    else
    {
        bfi_threshold = g_spcCtx->HR_BfiThreshold;
    }

    // derive the new bfi for voc from NBlock_result.Bfi
    spRx->voc_bfi |= g_spcCtx->NBlock_result.Bfi |
                     spRx->StealingResult     |
                     (g_spcCtx->NBlock_result.BitError > bfi_threshold);

    // discard non aligned sidupdate frame in DTX downlink
    if ((spRx->voc_taf ==0)&&
            (spRx->voc_sid)&&(g_spcCtx->DTX_dwnlk_flag))
    {
        spRx->voc_bfi=1;
    }

    // update the BFI counter
    if ((g_spcCtx->NBlock_result.Bfi==1)&&
            (g_spcCtx->BFI_count<2))
    {
        g_spcCtx->BFI_count++;
    }
    else if ((g_spcCtx->NBlock_result.Bfi==0)&&
             (g_spcCtx->BFI_count>0))
    {
        g_spcCtx->BFI_count--;
    }

    //update the current state: speech or DTX downlink
    // if SIDUPDATE: =>DTX_dwnlk_flag DTX_dwnlk_count=2
    if((spRx->voc_sid)||(g_spcCtx->BFI_count==2))
    {
        g_spcCtx->DTX_dwnlk_flag=1;
        g_spcCtx->DTX_dwnlk_count=2;
    }
    // if no SIDUPDATE && no BFI && DTX downlink: DTX_dwnlk_count--
    else if(g_spcCtx->NBlock_result.Bfi==0)
    {
        if (g_spcCtx->DTX_dwnlk_count)
        {
            g_spcCtx->DTX_dwnlk_count--;
        }
    }
    // if BFI && DTX downlink: DTX_dwnlk_count=2;
    else
    {
        if (g_spcCtx->DTX_dwnlk_flag)
        {
            g_spcCtx->DTX_dwnlk_count=2;
        }
    }
    // if DTX_dwnlk_count==0 => speech
    if (g_spcCtx->DTX_dwnlk_count==0)
    {
        g_spcCtx->DTX_dwnlk_flag=0;
    }

    if (g_spcCtx->DTX_dwnlk_flag)
    {
        if ((spRx->voc_sid==0))
        {
            spRx->voc_bfi = 1;
        }
    }
    // detection of FACCH peempted frame for HR
    if (RATE(g_spcCtx->ChMode) == HR_RATE)
    {
        StealingSoftBits[0] = g_spcCtx->Rx_Hu_TCH_N;
        StealingSoftBits[1] = g_spcCtx->Rx_Hl_TCH_N;
        spp_DecodeStealingBits(StealingSoftBits,
                               0,
                               &Facch_detected,
                               g_spcCtx->FacchThreshold);
        if (Facch_detected)
        {
            spRx->voc_bfi=1;
        }
    }
}

//=============================================================================
// spc_SppToMboxSpeechMode function
//-----------------------------------------------------------------------------
/// Convert from SPP speech mode to VPP speech mode
/// @param enc : SPP speech mode for encoding
/// @param dec : SPP speech mode for decoding
/// @return UINT16 codecMode formed by logical and of two MBOX_SPEECH_MODE_T
/// encMode & decMode
//=============================================================================
PRIVATE UINT16 SPAL_FUNC_INTERNAL spc_SppToMboxSpeechMode(SPP_SPEECH_MODE_T enc, SPP_SPEECH_MODE_T dec)
{

    MBOX_SPEECH_MODE_T encMode;
    MBOX_SPEECH_MODE_T decMode;

    switch (dec)
    {
        case SPP_MR475_MODE:        decMode = MBOX_AMR475_DEC_MODE; break;
        case SPP_MR515_MODE:        decMode = MBOX_AMR515_DEC_MODE; break;
        case SPP_MR59_MODE:         decMode = MBOX_AMR59_DEC_MODE;  break;
        case SPP_MR67_MODE:         decMode = MBOX_AMR67_DEC_MODE;  break;
        case SPP_MR74_MODE:         decMode = MBOX_AMR74_DEC_MODE;  break;
        case SPP_MR795_MODE:        decMode = MBOX_AMR795_DEC_MODE; break;
        case SPP_MR102_MODE:        decMode = MBOX_AMR102_DEC_MODE; break;
        case SPP_MR122_MODE:        decMode = MBOX_AMR122_DEC_MODE; break;
        case SPP_FR_MODE:           decMode = MBOX_FR_MODE;         break;
        case SPP_HR_MODE:           decMode = MBOX_HR_MODE;         break;
        case SPP_EFR_MODE:          decMode = MBOX_EFR_MODE;        break;
        default:                    decMode = MBOX_INVALID_MODE;    break;
    }

    if ((decMode | MBOX_AMR122_DEC_MODE) == MBOX_AMR122_DEC_MODE)
    {
        // if the decoder mode is AMR
        switch (enc)
        {
            case SPP_MR475_MODE:        encMode = MBOX_AMR475_ENC_MODE; break;
            case SPP_MR515_MODE:        encMode = MBOX_AMR515_ENC_MODE; break;
            case SPP_MR59_MODE:         encMode = MBOX_AMR59_ENC_MODE;  break;
            case SPP_MR67_MODE:         encMode = MBOX_AMR67_ENC_MODE;  break;
            case SPP_MR74_MODE:         encMode = MBOX_AMR74_ENC_MODE;  break;
            case SPP_MR795_MODE:        encMode = MBOX_AMR795_ENC_MODE; break;
            case SPP_MR102_MODE:        encMode = MBOX_AMR102_ENC_MODE; break;
            case SPP_MR122_MODE:
            default:                    encMode = MBOX_AMR122_ENC_MODE; break;
        }
    }
    else
    {
        // else the encoder and decoder modes are the same
        encMode = MBOX_INVALID_MODE;
    }

    return (encMode & decMode);

}

//=============================================================================
// spc_UpdateSpeechDec()
//-----------------------------------------------------------------------------
/// This function updates the context speech variables for VOC
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_UpdateSpeechDec(SPC_RX_LOCALS_T *spRx)
{
    UINT16 *Addr16;
    UINT8 voc_bfi;
    UINT8 i;

    SPC_PROFILE_FUNCTION_ENTER(spc_UpdateSpeechDec);
    // update parameters for VOC : voc_sid
    spRx->voc_sid = g_spcCtx->NBlock_result.Voc_Sid;
    spRx->voc_taf=0;
    spRx->voc_bfi=0;

    if (RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        if (g_spcCtx->AFNmod104==59)
        {
            spRx->voc_taf = 1;
        }
    }
    else
    {
        if (SUBCHANNEL(g_spcCtx->ChMode) == 0)
        {
            if ((g_spcCtx->AFNmod104==6)||(g_spcCtx->AFNmod104==58))
            {
                spRx->voc_taf = 1;
            }
        }
        else
        {
            if ((g_spcCtx->AFNmod104==20)||(g_spcCtx->AFNmod104==72))
            {
                spRx->voc_taf = 1;
            }
        }
        if( g_spcCtx->NBlock_result.BitError > g_spcCtx->UfiThreshold)
        {
            spRx->voc_ufi=0x1;
            if(g_spcCtx->NBlock_result.Voc_Sid==1)
            {
                spRx->voc_bfi=1;
            }
        }
    }
    // update parameters for VOC : voc_bfi
    spc_VocBfiFilter(spRx);

    if(SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
    {
        voc_bfi=g_spcCtx->NBlock_result.Bfi;
    }
    else
    {
        voc_bfi=spRx->voc_bfi;
    }

    // copy the results in speech structure
    g_mailbox.spc2pal.speechDecIn.dtxOn = g_spcCtx->voc_dtx_en;
    g_mailbox.spc2pal.speechDecIn.codecMode =
        spc_SppToMboxSpeechMode(g_spcCtx->TxCodecMode,g_spcCtx->RxCodecMode);
    g_mailbox.spc2pal.speechDecIn.decFrameType = spRx->RxFrameType;
    g_mailbox.spc2pal.speechDecIn.bfi = voc_bfi;
    g_mailbox.spc2pal.speechDecIn.sid = spRx->voc_sid;
    g_mailbox.spc2pal.speechDecIn.taf = spRx->voc_taf;
    g_mailbox.spc2pal.speechDecIn.ufi = spRx->voc_ufi;
    Addr16 = (UINT16*) spRx->BlockDecodedAdd;
    for (i=0; i<MBOX_MAX_SPEECH_FRAME_SIZE; i++)
    {
        g_mailbox.spc2pal.speechDecIn.decInBuf[i] = Addr16[i];
    }
    // copy the Tx struct in BB context for the next speech encoding
    g_spcCtx->speechEncOut = g_mailbox.pal2spc.speechEncOut;

    SPC_PROFILE_FUNCTION_EXIT(spc_UpdateSpeechDec);
}

//=============================================================================
// spc_CheckLoopMode()
//-----------------------------------------------------------------------------
/// This function close and open the TCH test loop mode
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_CheckLoopMode(SPC_RX_LOCALS_T *spRx)
{
    UINT8 i;
    SPC_PROFILE_FUNCTION_ENTER(spc_CheckLoopMode);
    // test for loop A, D, E and F
    if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_D) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_E) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_F) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) )
    {
        UINT8 zeros = 0;
        switch (g_mailbox.pal2spc.statWin.loopMode)
        {
            case MBOX_LOOP_CLOSE_A:
                if (g_spcCtx->NBlock_result.Bfi)
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_D:
                if ((spRx->voc_bfi) || (spRx->voc_ufi) || (spRx->StealingResult))
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_E:
                if (spRx->voc_sid == 0)
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_F:
                if ((spRx->voc_bfi) || (spRx->voc_ufi) || (spRx->StealingResult) || (spRx->voc_sid != 2))
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_I:
                if (spRx->StealingResult)
                {
                    zeros = 1;
                }
                else
                {
                    for (i = 0; i < 14; i++)
                    {
                        spRx->BlockDecodedAdd[i] = 0x55555555;
                    }
                }
                break;
            default:
                break;
        }
        if (zeros)
        {
            for (i = 0; i < 14; i++)
            {
                spRx->BlockDecodedAdd[i] = 0;
            }
        }
        g_spcCtx->loop_bufIdx = spRx->buffIdx ;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_CheckLoopMode);
}

//=============================================================================
// spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
{
    // default value set for SACCH
    UINT32 *ItlvOutput=(UINT32*)(g_spcCtx->Malloc) ;
    // default value set for SACCH
    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT32 *EqualizerInput;
    UINT32 *EqualizerOutput;
    UINT8 BuffIdx = spRx->buffIdx+spRx->slotIdx;
    UINT8 i=0;

    SPC_PROFILE_FUNCTION_ENTER(spc_CCHRxProcess);

    if (g_spcCtx->DedicatedActive==FALSE)
    {
        SoftBitAddress=g_spcCtx->ItlvBufRxCtrl+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;
    }
    //----------------------------- EQUALIZE  ---------------------------------
    EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
    ////////////////////////////////////////////////////////
    // dump the input samples if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_EQU_IN)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerInput,
            312,
            SPC_DBG_DUMP_EQU_IN_MARKER);
    }
    ////////////////////////////////////////////////////////
    EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 32);

    spp_EqualizeNBurst(EqualizerInput,
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);


    g_spcCtx->Rx_q_CCH[spRx->slotIdx][ spRx->burstIdx << 1     ] =
        ((EqualizerOutput[28]) >> 16) & 0x0FF;
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
        ( EqualizerOutput[28]) >> 24;
    ////////////////////////////////////////////////////////
    // dump the output softbits if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_EQU_OUT)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerOutput,
            58,
            SPC_DBG_DUMP_EQU_OUT_MARKER);
    }
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    // check the dummy PCH burst detection is enabled
    if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_PCH_READY)
    {
        UINT8 result=0;
        result = spp_DetectDummyPCH(EqualizerOutput,
                                    spRx->burstIdx,
                                    ((g_mailbox.dummyPchThresh)&0xFF00)>>8,
                                    (g_mailbox.dummyPchThresh)&0xFF);
        g_mailbox.dummyPchResult = (UINT32) result;
        spal_ComregsSetIrq(MBOX_DSP_PCH_READY);
    }
    ////////////////////////////////////////////////////////

    //----------------------------- EQUALIZE  ---------------------------------
    //----------------------------  RESULTS  ----------------------------------
    if (spRx->slotIdx==0)
    {
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
            (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
            g_spcCtx->NBurst_result.FOf;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
            (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
            g_spcCtx->NBurst_result.Pwr;
    }
    //----------------------------  RESULTS  ----------------------------------
    //---------------------------- CIPHERING ----------------------------------
    if (g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);
    }
    //---------------------------- CIPHERING ----------------------------------
    //----------------------------  DECODE  -----------------------------------
    if (spRx->burstIdx == 3)
    {
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
            (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[1] = NULL;

        // initialize the output buffer to avoid bit error due to tail bits
        for(i=0; i<14; i++)
        {
            g_spcCtx->NBlockDataOut[BuffIdx][i]=0;
        }
        // initialize the block decoding output address
        spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[BuffIdx];

        spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxCtrl;
        // initialize the usfs
        if (spRx->slotIdx == 0)
        {
            g_mailbox.spc2pal.statWin.rx.decodedUsf[0]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[1]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[2]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[3]=0xFF;
        }
        // decode Coding Scheme for the current slot
        spp_DecodeStealingBits((UINT32 *) (g_spcCtx->Rx_q_CCH[spRx->slotIdx]),
                               1,
                               &(g_spcCtx->Rx_CS[spRx->slotIdx]),
                               0);
        // default value for SACCH
        spRx->Copy_ChMode = CCH_mode + (GSM_CS1<<8);

        //--------------------------  DEINTERLEAVE  ---------------------------
        if (g_spcCtx->DedicatedActive==FALSE)
        {
            if (spRx->slotIdx == 0)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     SPC_TMP_ITLV_RX_BUFF_OFFSET);
            }
            else if(spRx->slotIdx == g_spcCtx->rxslotNb-1)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->Malloc);
            }
            else
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     (spRx->slotIdx-1)*SPC_ITLV_RX_BUFF_OFFSET);
            }
        }
        spp_DeinterleaveBlock(SoftBitAddress,
                              ITLV_TYPE_1B,
                              0,
                              (UINT32*)ItlvOutput);
        //--------------------------  DEINTERLEAVE  ---------------------------

        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            spp_GetUSF((UINT32*)ItlvOutput,
                       g_spcCtx->Rx_CS[spRx->slotIdx]);

        if(g_spcCtx->sched_slot_count==g_spcCtx->rxslotNb)
        {
            UINT8 k=0;

            // send an it to the XCPU if the last slot has been received
            if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_USF_READY)
            {
                spal_ComregsSetIrq(MBOX_DSP_USF_READY);
            }
            for (k=0; k<g_spcCtx->rxslotNb; k++)
            {
                BuffIdx = spRx->buffIdx+k;
                if (g_spcCtx->DedicatedActive==FALSE)
                {
                    if (k == 0)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             SPC_TMP_ITLV_RX_BUFF_OFFSET);
                    }
                    else if(k == g_spcCtx->rxslotNb-1)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->Malloc);
                    }
                    else
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             (k-1)*SPC_ITLV_RX_BUFF_OFFSET);
                    }
                    spRx->Copy_ChMode=CCH_mode +
                                      ((g_spcCtx->Rx_CS[k])<<8);
                    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
                        (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
                    spRx->BlockDecodedAdd=g_spcCtx->NBlockDataOut[BuffIdx];
                }

                ////////////////////////////////////////////////////////
                // dump the decoder input softbits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_IN)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)ItlvOutput,
                        228,
                        SPC_DBG_DUMP_DEC_IN_MARKER);
                }
                ////////////////////////////////////////////////////////

                spp_DecodeBlock((UINT32*)ItlvOutput,
                                spRx->Copy_ChMode,
                                0,
                                0,
                                spRx->BlockDecodedAdd,
                                (g_mailbox.spc2pal.statWin.rx.decodedUsf[k])&0x7,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);

                ////////////////////////////////////////////////////////
                // dump the decoder output bits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_OUT)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)spRx->BlockDecodedAdd,
                        28,
                        SPC_DBG_DUMP_DEC_OUT_MARKER);
                }
                ////////////////////////////////////////////////////////

                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].meanBep =
                    g_spcCtx->NBlock_result.MeanBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cvBep =
                    g_spcCtx->NBlock_result.CvBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitError =
                    g_spcCtx->NBlock_result.BitError;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitTotal =
                    g_spcCtx->NBlock_result.BitTotal;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bfi =
                    g_spcCtx->NBlock_result.Bfi;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                    g_spcCtx->Rx_CS[k];
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].sid =
                    g_spcCtx->NBlock_result.Voc_Sid |
                    (spRx->RxFrameType==SPP_RX_SID_UPDATE);


            }
            // reset the Rx Ctrl buffer to allow decoding with only two bursts
            for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrl[i]=0;
        }
    }
    //----------------------------  DECODE  -----------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_CCHRxProcess);
}

#if (CHIP_EDGE_SUPPORTED == 1)
//=============================================================================
// spc_EgprsBlockDecode(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes a block decoding for Egprs MCS1-9
//=============================================================================
PROTECTED VOID patch_spc_EgprsBlockDecode(UINT8 bufferindex, UINT8 slot)
{
    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT8 BuffIdx=bufferindex+slot;
    UINT8 size=0;
    UINT8 i=0;
    SPP_HEADER_TYPE_T header_type;
    SPP_HEADER_RESULT_T header_result;

    SoftBitAddress=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                             BuffIdx*SPC_ITLV_RX_BUFF_OFFSET);

    // for IR only we implement the header filtering
    if (g_spcCtx->statWinRxCfg[BuffIdx].egprs==MBOX_EGPRS_WITH_IR)
    {
        // save the previous header result
        header_result=g_spcCtx->Header_result;
        // filter the header type
        if(g_spcCtx->prev_rx_headertype[slot]==
                g_spcCtx->rx_headertype[slot])
        {
            header_type=g_spcCtx->rx_headertype[slot];
        }
        else
        {
            header_type=g_spcCtx->curr_rx_headertype[slot];
        }
    }
    else
    {
        header_type=g_spcCtx->rx_headertype[slot];
    }

    if (header_type == SPP_HEADER_TYPE3)
    {
        // Demapping of the block
        spp_EgprsBurstDemapping((UINT32*)(SoftBitAddress+128),
                                (UINT32*)(SoftBitAddress),
                                NULL,
                                header_type);

        // decode the header
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
            (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];

        spp_EgprsDecodeHeader((UINT8*)(SoftBitAddress),
                              header_type,
                              g_mailbox.spc2pal.statWin.rx.decodedUsf[BuffIdx]&0x7,
                              (UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx]),
                              (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                              &header_result);

        if (!header_result.bfi)
        {
            size=spcSizeFromMCS[header_result.mcs];

            spp_EgprsDecodeData((UINT8*)(SoftBitAddress)+68,
                                header_result,
                                (UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx])+6,
                                NULL,
                                &g_Egprs_Ir_Ctx,
                                g_spcCtx->egprsIrBuffer,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);
        }
    }
    else
    {
        SPP_ITLV_TYPE_T itlv_type;
        if (header_type == SPP_HEADER_TYPE2)
        {
            itlv_type=ITLV_TYPE_H1;
        }
        else
        {
            itlv_type=ITLV_TYPE_H3;
        }
        // Demapping of the block
        spp_EgprsBurstDemapping((UINT32*)(SoftBitAddress),
                                (UINT32*)(g_spcCtx->Malloc),
                                (UINT32*)(g_spcCtx->Malloc+32),
                                header_type);

        spp_EgprsDeinterleaveBlock((UINT32*)(g_spcCtx->Malloc),
                                   itlv_type,
                                   (UINT32*)(SoftBitAddress));

        // decode the header
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
            (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];

        spp_EgprsDecodeHeader((UINT8*)(SoftBitAddress),
                              header_type,
                              g_mailbox.spc2pal.statWin.rx.decodedUsf[BuffIdx]&0x7,
                              (UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx]),
                              (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                              &header_result);

        if (!header_result.bfi)
        {
            size=spcSizeFromMCS[header_result.mcs];
            itlv_type=ITLV_TYPE_D1;
            if (header_result.mcs == EGPRS_MCS7)
            {
                itlv_type=ITLV_TYPE_D2;
            }
            else if ((header_result.mcs == EGPRS_MCS8)||
                     (header_result.mcs == EGPRS_MCS9))
            {
                itlv_type=ITLV_TYPE_D3;
            }
            spp_EgprsDeinterleaveBlock(
                (UINT32*)(g_spcCtx->Malloc+32),
                itlv_type,
                (UINT32*)(SoftBitAddress+32)
            );

            spp_EgprsDecodeData((UINT8*)(SoftBitAddress)+128,
                                header_result,
                                (UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx])+6,
                                (UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx])+6
                                +size+1,
                                &g_Egprs_Ir_Ctx,
                                g_spcCtx->egprsIrBuffer,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);
        }
    }

    // rewrite the RLC block at the format defined in PAL Emerald
    // interface and update the current header type for filtering
    // in case of IR.
    if (!header_result.bfi)
    {
        // update the header result with the new value
        g_spcCtx->Header_result=header_result;
        // for IR only we implement the header filtering
        if (g_spcCtx->statWinRxCfg[BuffIdx].egprs==MBOX_EGPRS_WITH_IR)
        {
            // clean the IR Ctx if the type has changed
            if( (g_spcCtx->prev_rx_headertype[slot]==header_type)&&
                    (g_spcCtx->curr_rx_headertype[slot]!=header_type) )
            {
                // update the current type with the new one
                g_spcCtx->curr_rx_headertype[slot]=header_type;
                // clean the IR Ctx
                spp_EgprsIrInitCtx(&g_Egprs_Ir_Ctx,header_result.mcs);
            }
            else // the previous type was the rigth one
            {
                g_spcCtx->rx_headertype[slot]=
                    g_spcCtx->curr_rx_headertype[slot];
            }
        }

        if (!(g_spcCtx->NBlock_result.Bfi & 0x1))
        {
            UINT8* ptr=(UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx]);
            UINT8 offset=size+6;
            for (i=0; i<size; i++)
            {
                ptr[offset-i]=(ptr[offset-i]<<6)||
                              (ptr[offset-i-1]>>2);
            }
            ptr[6] &= 0x03;
        }
        if ((g_spcCtx->rx_headertype[slot] == SPP_HEADER_TYPE1)&&
                (!(g_spcCtx->NBlock_result.Bfi & 0x2)))
        {
            UINT8* ptr=(UINT8*) (g_spcCtx->NBlockDataOut[BuffIdx]);
            UINT8 offset=size+6+1+size;
            for (i=0; i<size; i++)
            {
                ptr[offset-i]=(ptr[offset-i]<<6)||
                              (ptr[offset-i-1]>>2);
            }
            ptr[6+1+size] &= 0x03;
        }
    }

    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].meanBep =
        g_spcCtx->NBlock_result.MeanBEP;
    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cvBep =
        g_spcCtx->NBlock_result.CvBEP;
    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitError =
        g_spcCtx->NBlock_result.BitError;
    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitTotal =
        g_spcCtx->NBlock_result.BitTotal;
    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bfi =
        g_spcCtx->NBlock_result.Bfi||
        header_result.bfi<<2;
    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
        header_result.mcs;
}

//=============================================================================
// spc_ECCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_EgprsCCHRxProcess(SPC_RX_LOCALS_T *spRx)
{

    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT32 *EqualizerInput;
    UINT32 *EqualizerOutput;
    UINT32 *loopCBuffAdd;
    VOID (*EqualizerPtr)(UINT32*,
                         UINT8,
                         UINT32*,
                         SPP_UNCACHED_BUFFERS_T*,
                         SPP_N_BURST_RESULT_T*);
    UINT8 BuffIdx = spRx->buffIdx+spRx->slotIdx;
    UINT8 burst_offset;
    UINT8 usf_offset;
    UINT8 tof=0;
    UINT8 i=0;
    UINT8 j=0;
    UINT8 tsc=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc;

    SPC_PROFILE_FUNCTION_ENTER(spc_EgprsCCHRxProcess);

    //----------------------------- EQUALIZE  ---------------------------------
    if (spRx->burstIdx==0)
    {
        g_spcCtx->rx_modulation=SPP_UNKNOWN_MOD;
    }
    if (g_spcCtx->rx_modulation!=SPP_GMSK_MOD)
    {
        spp_EgprsBlindDetection(
            (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                         (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]),
            spRx->burstIdx,
            tsc,
            &g_spcCtx->rx_modulation,
            &g_spcCtx->NBurst_result.TOf,
            &tof
        );
    }

    SoftBitAddress = g_spcCtx->ItlvBufRxCtrl +
                     BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;
    if (g_spcCtx->rx_modulation==SPP_GMSK_MOD)
    {
        burst_offset=4;
        usf_offset=28;
        EqualizerInput = (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx
                                      *(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
        EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 32);
        EqualizerPtr = spp_EqualizeNBurst;
    }
    else // 8PSK MODULATION
    {
        burst_offset=11;
        usf_offset=43;
        EqualizerInput = (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx
                                      *(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
        EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 87);
        EqualizerPtr = spp_EgprsEqualizeNBurst;
    }

    ////////////////////////////////////////////////////////
    // dump the input samples if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_EGPRS_EQU_IN)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerInput,
            312,
            SPC_DBG_DUMP_EQU_IN_MARKER);
    }
    ////////////////////////////////////////////////////////

    // process the burst equalization
    (*EqualizerPtr)(
        (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                     (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)+tof]),
        tsc,
        EqualizerOutput,
        (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
        &g_spcCtx->NBurst_result
    );

    g_spcCtx->Rx_q_CCH[spRx->slotIdx][ spRx->burstIdx << 1     ] =
        ((EqualizerOutput[usf_offset]) >> 16) & 0x0FF;
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
        ( EqualizerOutput[usf_offset]) >> 24;

    ////////////////////////////////////////////////////////
    // dump the output softbits if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_EGPRS_EQU_OUT)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerOutput,
            (burst_offset==4)?58:174,
            SPC_DBG_DUMP_EQU_OUT_MARKER);
    }
    ////////////////////////////////////////////////////////


    //------------------ test if EGPRS loopback is closed ---------------------
    if(g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_EGPRS)
    {
        // we store the hard decisions of the equalized softbit in one of the
        // softbit buffer left assuming the multislot case will be limited
        // to two Rx slots.
        loopCBuffAdd=g_spcCtx->ItlvBufRxCtrl + (BuffIdx+3)*SPC_ITLV_RX_BUFF_OFFSET +
                     spRx->slotIdx*SPC_ITLV_TX_BUFF_OFFSET +
                     spRx->burstIdx * burst_offset;
        g_spcCtx->loopC_buffer=(UINT8*)EqualizerOutput;
        for (i=0; i<burst_offset; i++)
        {
            for (j = 0; j < 32; j++)
            {
                loopCBuffAdd[i]|=((g_spcCtx->loopC_buffer[32*i+j]>>7)&0x01)<<j;
            }
        }
        g_spcCtx->loopC_buffer=(UINT8*)loopCBuffAdd;
    }
    //----------------------------- EQUALIZE  ---------------------------------
    //----------------------------  RESULTS  ----------------------------------
    if (spRx->slotIdx==0)
    {
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
            (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
            g_spcCtx->NBurst_result.FOf;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
            (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
            g_spcCtx->NBurst_result.Pwr;
    }
    //----------------------------  RESULTS  ----------------------------------
    //------------------------  USF DECODE  -----------------------------------
    if (spRx->burstIdx == 3)
    {
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
            (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[1] = NULL;
        // reset the bit map representing the buffers to decode
        g_spcCtx->egprs_rx_buffer_bmp=0;

        // initialize the output buffer to avoid bit error due to tail bits
        for(i=0; i<39; i++)
        {
            g_spcCtx->NBlockDataOut[BuffIdx][i]=0;
        }
        // initialize the block decoding output address
        spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[BuffIdx];

        // initialize the usfs
        if (spRx->slotIdx == 0)
        {
            g_mailbox.spc2pal.statWin.rx.decodedUsf[0]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[1]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[2]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[3]=0xFF;
        }
        // update the previous header type for the filter
        g_spcCtx->prev_rx_headertype[spRx->slotIdx]=
            g_spcCtx->rx_headertype[spRx->slotIdx];

        if (g_spcCtx->rx_modulation==SPP_GMSK_MOD) // header of type 3
        {
            g_spcCtx->rx_headertype[spRx->slotIdx] = SPP_HEADER_TYPE3;
            //--------------------------  DEINTERLEAVE  -----------------------
            spp_DeinterleaveBlock((UINT32*)(SoftBitAddress),
                                  ITLV_TYPE_1B,
                                  0,
                                  (UINT32*)(SoftBitAddress+128));
            //--------------------------  DEINTERLEAVE  -----------------------

            g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
                spp_GetUSF((UINT32*)(SoftBitAddress),GSM_CS4);
        }
        else // header of type 1 or 2 (8PSK modulation)
        {
            UINT8 USF_Tab[36];

            for (i=0; i<48; i++)
            {
                if (((i%12)!=6)||((i%12)!=7)||((i%12)!=11))
                {
                    USF_Tab[j]=((SoftBitAddress[(87*(i/12))+42+i/4])>>8*(i%4))
                               &0x0FF;
                    j++;
                }
            }
            // decode Coding Scheme for the current slot
            spp_EgprsDecodeStealingBits(
                (UINT32 *) (g_spcCtx->Rx_q_CCH[spRx->slotIdx]),
                &(g_spcCtx->rx_headertype[spRx->slotIdx])
            );
            // header of type SPC_HEADER_TYPE1
            // 124 header bits and 2x612 data bits
            // header of type SPP_HEADER_TYPE2
            // 100 header bits and 1248 data bits

            // decode the USF
            g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
                spp_EgprsGetUSF((UINT32*) &USF_Tab[0]);
        }

        if(g_spcCtx->sched_slot_count==g_spcCtx->rxslotNb)
        {
            // send an it to the XCPU if the last slot has been received
            if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_USF_READY)
            {
                spal_ComregsSetIrq(MBOX_DSP_USF_READY);
            }
            //---------------  FORM THE BLOCK TO DECODE BITMAP ----------------
            for (i=0; i<g_spcCtx->rxslotNb; i++)
            {
                g_spcCtx->egprs_rx_buffer_bmp |= (1<<(spRx->buffIdx+i));
            }
            // save the burstIdx0 third and fourth slot equalization results
            if(g_spcCtx->rxslotNb>2)
                for(i=0; i<2; i++)
                {
                    UINT32* tmp1=&(g_Egprs_Softbit_Buff[87*i]);
                    UINT32* tmp2=g_spcCtx->ItlvBufRxCtrl+(i+2)*SPC_ITLV_RX_BUFF_OFFSET;
                    for(j=0; j<87; j++)
                    {
                        tmp1[j]=tmp2[j];
                    }
                }
        }
    }
    //------------------------  USF DECODE  -----------------------------------
    //----------------------------  ACTUAL BLOCK DECODE -----------------------
    if((g_spcCtx->egprs_rx_buffer_bmp)&&
            (g_spcCtx->sched_slot_count==g_spcCtx->rxslotNb))
    {
        if(spRx->burstIdx == 0)
        {
            // if we decode the third or the fourth block we need to recopy
            // the fourth burst softbits previously saved at N+1
            for(i=0; i<2; i++)
            {
                UINT32* tmp1=&(g_Egprs_Softbit_Buff[87*i]);
                UINT32* tmp2=g_spcCtx->ItlvBufRxCtrl+(i+2)*SPC_ITLV_RX_BUFF_OFFSET;
                for(j=0; j<87; j++)
                {
                    tmp2[j]=tmp1[j];
                }
            }
        }
        for(i=0; i<2; i++)
        {
            for (j=0; j<g_spcCtx->rxslotNb; j++)
            {
                if ((g_spcCtx->egprs_rx_buffer_bmp)&(0x1<<j))
                {
                    break;
                }
            }

            patch_spc_EgprsBlockDecode(j,i);
            // reset the decoded buffer in the bitmap
            g_spcCtx->egprs_rx_buffer_bmp &= ~(0x1<<j);
            // send an it to the XCPU for the end of Block Decoding if the last
            // buffer has been decoded
            if (g_spcCtx->egprs_rx_buffer_bmp==0)
            {
#ifdef __USE_NB_DECODE_IRQ__
                if(g_spcCtx->statWinRxCfg[j].itr &
                        MBOX_DSP_NB_DECODING_COMPLETE)
                {
                    spal_ComregsSetIrq(MBOX_DSP_NB_DECODING_COMPLETE);
                }
#endif
                break;
            }
        }
    }
    //----------------------------  ACTUAL BLOCK DECODE -----------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_EgprsCCHRxProcess);
}
#endif // CHIP_EDGE_SUPPORTED == 1

//=============================================================================
// spc_TCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_TCHRxProcess(SPC_RX_LOCALS_T *spRx)
{
#define DECODE_SPEECH   1
#define DECODE_DATA     2
#define WRITE_DATA      3

    UINT32 StealingSoftBits[2]= {0,0};
    UINT32 *EqualizerOutput;
    UINT8 RxBurstOffset;
    UINT8 i;
    UINT8 ItlvBufLength=8;
    UINT8 Mask=0x3;

    SPC_PROFILE_FUNCTION_ENTER(spc_TCHRxProcess);

    // DTX is actually done here, except for HBURST equalization
    // because this would clear the PDN and the burst would not
    // be entirely received.
    if ((g_spcCtx->Tx_off==TRUE) && (!g_spcCtx->equ_hburst_mode))
    {
        spal_TcuDisableEvent(g_mailbox.pal2spc.rf.dtxTcoSettings);
    }

    if (RATE(g_spcCtx->ChMode) == HR_RATE)
    {
        ItlvBufLength=6;
        Mask=0x1;

    }
    else if(MODE(g_spcCtx->ChMode) == TCH_DATA)
    {
        ItlvBufLength=22;
    }

    //------------------------------ EQUALIZE ----------------------------------
    RxBurstOffset = g_spcCtx->RxBlockOffset + (spRx->burstIdx & Mask);
    if (RxBurstOffset >= ItlvBufLength)
    {
        RxBurstOffset -= ItlvBufLength;    //test for data interleaving
    }
    EqualizerOutput = (UINT32 *) (g_spcCtx->ItlvBufRxDedicated +
                                  RxBurstOffset * 32);

    ////////////////////////////////////////////////////////
    // dump the input samples if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_TCH_EQU_IN)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)&(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]),
            312,
            SPC_DBG_DUMP_EQU_IN_MARKER);
    }
    ////////////////////////////////////////////////////////

    spp_EqualizeNBurst((UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]),
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);


    // Store Stealing Flag
    g_spcCtx->Rx_Hu_TCH_N <<= 8;
    g_spcCtx->Rx_Hl_TCH_N <<= 8;
    g_spcCtx->Rx_Hu_TCH_N |= ( EqualizerOutput[28]) >> 24;
    g_spcCtx->Rx_Hl_TCH_N |= ((EqualizerOutput[28]) >> 16) & 0x0FF;

    ////////////////////////////////////////////////////////
    // dump the output softbits if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_TCH_EQU_OUT)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerOutput,
            58,
            SPC_DBG_DUMP_EQU_OUT_MARKER);
    }
    ////////////////////////////////////////////////////////

    if (spRx->burstIdx == 0)
    {
        g_spcCtx->BlockSnR  = g_spcCtx->NBurst_result.Snr;
    }
    else
    {
        g_spcCtx->BlockSnR += g_spcCtx->NBurst_result.Snr;
    }

    if (SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
    {
        INT32 QI=0;
        INT16 *filter=(RATE(g_spcCtx->ChMode)==FR_RATE)?
                      (INT16*)AFSfilterCI:(INT16*)AHSfilterCI;
        UINT8 length=(RATE(g_spcCtx->ChMode)==FR_RATE)?101:51;
        for(i=1; i<=length-1; i++)
        {
            g_spcCtx->CIest[length-i]=g_spcCtx->CIest[length-i-1];
        }
        g_spcCtx->CIest[0]=g_spcCtx->NBurst_result.Snr;
        for(i=0; i<length; i++)
        {
            QI+=filter[i]*g_spcCtx->CIest[i];
        }
        if (QI<0)
        {
            QI=0;
        }
        spc_UpdateCMR(g_spcCtx->AMR_Cfg_Rx,
                      (UINT16)(QI>>15),
                      &g_spcCtx->CodecModeRequest);
    }

    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.sid =
        (g_spcCtx->sidupdate_ctr==2)?1:0;

    //------------------------------ EQUALIZE ----------------------------------
    //----------------------------- LOOPBACK C ---------------------------------
    g_spcCtx->loopC_buffer = (UINT8 *) EqualizerOutput;
    //----------------------------- LOOPBACK C ---------------------------------
    //-----------------------------  RESULTS  ----------------------------------
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
        (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
        g_spcCtx->NBurst_result.FOf;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
        (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
        g_spcCtx->NBurst_result.Pwr;
    //-----------------------------  RESULTS  ----------------------------------
    //----------------------------- CIPHERING ----------------------------------
    if(g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);
    }
    //----------------------------- CIPHERING ----------------------------------
    //------------------------------ DECODE ------------------------------------
    if (RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        if (spRx->burstIdx == 3)
        {
            spRx->RX_decode=DECODE_SPEECH;
            spRx->itlv_mode=ITLV_TYPE_1A;
            g_spcCtx->RxBlockOffset += 4;
            spRx->RxBuffOffset = g_spcCtx->RxBlockOffset;
            if (g_spcCtx->RxBlockOffset >= ItlvBufLength)
            {
                g_spcCtx->RxBlockOffset -= ItlvBufLength;
            }
        }
        if(MODE(g_spcCtx->ChMode) == TCH_DATA)
        {
            if(spRx->burstIdx == 1)
            {
                spRx->RX_decode=DECODE_DATA;
                spRx->itlv_mode=ITLV_TYPE_3;
                spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxDedicated;
                spRx->RxBuffOffset = g_spcCtx->RxBlockOffset + 2;
                if (spRx->RxBuffOffset == ItlvBufLength)
                {
                    spRx->RxBuffOffset = 0;
                }
                spRx->BlockDecodedAdd = g_spcCtx->CSD_NBlockDataOut;
                for(i=0; i<14; i++)
                {
                    g_spcCtx->CSD_NBlockDataOut[i]=0;
                }
            }
        }
    }
    else
    {
        if ((spRx->burstIdx & Mask) == 1)
        {
            spRx->RX_decode=DECODE_SPEECH;
            g_spcCtx->RxBlockOffset += 2;
            if (g_spcCtx->RxBlockOffset == ItlvBufLength)
            {
                g_spcCtx->RxBlockOffset = 0;
            }
            spRx->RxBuffOffset = g_spcCtx->RxBlockOffset + 2;
            if (spRx->RxBuffOffset == ItlvBufLength)
            {
                spRx->RxBuffOffset = 0;
            }
            spRx->itlv_mode=ITLV_TYPE_2A;
        }
    }

    if (spRx->RX_decode==DECODE_SPEECH)
    {
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[0] = NULL;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[1] = NULL;

        // initialize the output buffer to avoid bit error due to tail bits
        for(i=0; i<14; i++)
        {
            g_spcCtx->NBlockDataOut[spRx->buffIdx][i]=0;
        }
        // initialize the block decoding output address
        spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[spRx->buffIdx];

        if(MODE(g_spcCtx->ChMode) == TCH_DATA)
        {
            spRx->RX_decode=WRITE_DATA;
            g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[1] =
                (UINT8 *) g_spcCtx->CSD_NBlockDataOut;
        }
        spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxDedicated;
        StealingSoftBits[0] = g_spcCtx->Rx_Hu_TCH_N_1;
        StealingSoftBits[1] = g_spcCtx->Rx_Hl_TCH_N;
        g_spcCtx->Rx_Hu_TCH_N_1 = g_spcCtx->Rx_Hu_TCH_N;
        spp_DecodeStealingBits(StealingSoftBits,
                               0,
                               &(spRx->StealingResult),
                               g_spcCtx->FacchThreshold);
        if ((spRx->StealingResult)&&(spRx->burstIdx == 3))
        {
            if(RATE(g_spcCtx->ChMode) == HR_RATE)
            {
                spRx->RxBuffOffset = g_spcCtx->RxBlockOffset;
                spRx->itlv_mode=ITLV_TYPE_1C;
            }
            else if(MODE(g_spcCtx->ChMode) == TCH_DATA)
            {
                spRx->RxBuffOffset = g_spcCtx->RxBlockOffset;
                spc_SetFacch(spRx);
            }

            spRx->Copy_ChMode = CCH_mode + (GSM_CS1<<8);
            // force CS to GSM_CS1 in case of FACCH
            g_spcCtx->Rx_CS[0]=GSM_CS1;
            g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[0] =
                (UINT8 *) g_spcCtx->NBlockDataOut[spRx->buffIdx];
            spRx->RX_decode=DECODE_SPEECH;
        }
    }
    if(spRx->RX_decode)
    {

        if (SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
        {
            spc_AmrRatscch();
            if ((SPEECH(g_spcCtx->ChMode)==AMR_CODEC)&&
                    (spRx->StealingResult==0))
            {
                spc_AmrDecode(spRx);
                spRx->RX_decode=0;
            }
        }

        if((spRx->RX_decode==DECODE_SPEECH) || (spRx->RX_decode==DECODE_DATA))
        {
            // Deinterleave the current block
            spp_DeinterleaveBlock(spRx->ItlvBuffInput,
                                  spRx->itlv_mode,
                                  spRx->RxBuffOffset,
                                  (UINT32*)g_spcCtx->Malloc);

            spp_DecodeBlock((UINT32*)g_spcCtx->Malloc,
                            spRx->Copy_ChMode,
                            0,
                            0,
                            spRx->BlockDecodedAdd,
                            (g_mailbox.spc2pal.statWin.rx.decodedUsf[0])&0x7,
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            &g_spcCtx->NBlock_result);
        }
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].meanBep =
            g_spcCtx->NBlock_result.MeanBEP;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].cvBep =
            g_spcCtx->NBlock_result.CvBEP;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bitError =
            g_spcCtx->NBlock_result.BitError;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bitTotal =
            g_spcCtx->NBlock_result.BitTotal;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bfi =
            g_spcCtx->NBlock_result.Bfi;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].cs =
            g_spcCtx->Rx_CS[0];
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].sid =
            g_spcCtx->NBlock_result.Voc_Sid|(spRx->RxFrameType==SPP_RX_SID_UPDATE);


        if(MODE(g_spcCtx->ChMode) == TCH_SPEECH)
        {
            spc_BfiFilter();
            // Update Mbox Bfi result as it may be modified by the filter
            g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bfi =
                g_spcCtx->NBlock_result.Bfi;
            spc_UpdateSpeechDec(spRx);
        }

        spc_CheckLoopMode(spRx);
    }
    //------------------------------ DECODE ------------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_TCHRxProcess);
}

//=============================================================================
// spc_NBurstProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst reception
//=============================================================================
PRIVATE VOID SPAL_FUNC_INTERNAL spc_NBurstProcess(SPC_RX_LOCALS_T *spRx)
{

    g_spcCtx->NBlock_result.Bfi = 0;
    g_spcCtx->NBlock_result.BitError = 0;
    g_spcCtx->NBlock_result.Voc_Sid = 0;
    g_spcCtx->NBlock_result.Cs = GSM_CS1;    // to avoid pal assert


    switch(g_spcCtx->statWinRxCfg[spRx->buffIdx].type)
    {
        case MBOX_CCH_CHN :
        case MBOX_PDCH_CHN:
#if (CHIP_EDGE_SUPPORTED==1)
            if(g_spcCtx->statWinRxCfg[spRx->buffIdx].egprs==MBOX_NO_EGPRS)
            {
                spc_CCHRxProcess(spRx);
            }
            else
            {
                spc_EgprsCCHRxProcess(spRx);
            }
#else
            spc_CCHRxProcess(spRx);
#endif
            break;
        case MBOX_TCHF_CHN :
        case MBOX_TCHH_CHN :
        case MBOX_FACCH_CHN:
        case MBOX_CDATA_CHN:
            spc_TCHRxProcess(spRx);
            break;
        default:
            break;
    }
    //send an interrupt to XCPu at next FINT
    g_spcCtx->sendEqItr = 1;

}


//=============================================================================
// spc_RxProcess()
//-----------------------------------------------------------------------------
/// This function executes the whole Rx for the current frame
//=============================================================================
PROTECTED VOID SPAL_FUNC_INTERNAL patch_spc_RxProcess(VOID)
{
    UINT8 current_type;
    UINT8 winIdx;

    SPC_RX_LOCALS_T spRx =
    {
        .ItlvBuffInput=NULL,
        .BlockDecodedAdd=g_spcCtx->NBlockDataOut[0],
        .burstIdx=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bstIdx,
        .buffIdx=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx,
        // duplicate the sched_slot_count for timing reason
        .slotIdx=g_spcCtx->sched_slot_count,
        .RxBuffOffset=0,
        .itlv_mode=ITLV_TYPE_1B,
        .StealingResult=0,
        .Copy_ChMode=g_spcCtx->ChMode,
        .voc_bfi=0,
        .voc_sid=0,
        .voc_taf=0,
        .voc_ufi=0,
        .RxFrameType=SPP_RX_NO_DATA,
        .RX_decode=0
    };

    SPC_PROFILE_FUNCTION_ENTER(spc_RxProcess);
    //read the rxtype
    if (g_spcCtx->FcchActive == 0)
    {
        current_type = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.
                       rxTypeWin[g_spcCtx->sched_rx_count];
        if (g_spcCtx->FcchSet==1)
        {
            g_spcCtx->FcchActive=1;
        }
        g_spcCtx->FcchSet=0;
    }
    else
    {
        current_type = GSM_WIN_TYPE_FCCH;
    }

// hzeng: stop BCPU to check rx buffer
#if 1
    if (g_stopBcpu[g_spcCtx->currentSnap] == 1)
    {
        if (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.
                monWinIdx[g_spcCtx->sched_rx_count] == g_winIdx)
            spc_HungBcpu();
    }
#endif

    switch (current_type)
    {
        //--------------------------------------------------------------------
        //           FREQUENCY BURST
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_FCCH:
            //HAL_W2SET(W2_FCCH);
            SPC_PROFILE_WINDOW_ENTER(FCCH);
            spc_FcchProcess();
            //HAL_W2CLR(W2_FCCH);
            SPC_PROFILE_WINDOW_EXIT(FCCH);
            break;
        //--------------------------------------------------------------------
        //           MONITORING
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_MONIT:
            //HAL_W2SET(W2_MONITORING);
            SPC_PROFILE_WINDOW_ENTER(MONITORING);
            winIdx = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.
                     monWinIdx[g_spcCtx->sched_rx_count];
            spc_MonitoringProcess(g_spcCtx->sched_rx_count,winIdx);
            g_spcCtx->sched_rx_count++;
            //HAL_W2CLR(W2_MONITORING);
            SPC_PROFILE_WINDOW_EXIT(MONITORING);
            break;
        //--------------------------------------------------------------------
        //           MONITORING
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_INTRF:
            // XXX: This will work only if INTRF and MONIT window are not programmed
            // XXX: within the same frame
            //HAL_W2SET(W2_INTRF);
            SPC_PROFILE_WINDOW_ENTER(INTERF);
            // TODO: to save space we only have 3 MonIfcBuffer.
            //       this implies that INTRF process for slot 0 MUST
            //       be finished before slot 3 reception
            spc_MonitoringProcess((g_spcCtx->sched_slot_count % 3),
                                  g_spcCtx->sched_slot_count);
            g_spcCtx->sched_slot_count++;
            if (g_spcCtx->sched_slot_count==g_spcCtx->rxslotNb)
            {
                g_spcCtx->sched_rx_count++;
            }
            //HAL_W2CLR(W2_INTRF);
            SPC_PROFILE_WINDOW_EXIT(INTERF);
            break;

        //--------------------------------------------------------------------
        //           SYNCHRO BURST
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_SCH:
            //HAL_W2SET(W2_SCH);
            SPC_PROFILE_WINDOW_ENTER(SCH);
            spc_SchProcess();
            g_spcCtx->sched_rx_count++;
            //HAL_W2CLR(W2_SCH);
            SPC_PROFILE_WINDOW_EXIT(SCH);
            break;

        //--------------------------------------------------------------------
        //             NORMAL BURST
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_NBURST:
            //HAL_W2SET(W2_NORMAL_BURST);
            SPC_PROFILE_WINDOW_ENTER(NBURST);
            // increment it rigth now for timming reasons
            g_spcCtx->sched_slot_count++;

            if(g_spcCtx->sched_slot_count==g_spcCtx->rxslotNb)
            {
                g_spcCtx->sched_rx_count++;
            }
            spc_NBurstProcess(&spRx);
            //HAL_W2CLR(W2_NORMAL_BURST);
            SPC_PROFILE_WINDOW_EXIT(NBURST);
            break;

        default:
            break;
    }                         //end swtich type

    if (g_spcCtx->sched_rx_count==g_spcCtx->rxQty)
    {
        g_spcCtx->frame_over=1; // end of the current frame process
        if (g_spcCtx->FcchActive == 0)
            spal_ComregsSetIrq(MBOX_DSP_TASK_COMPLETE);
    }

    SPC_PROFILE_FUNCTION_EXIT(spc_RxProcess);
}

#endif // ENABLE_PATCH_SPC_RXPROCESS

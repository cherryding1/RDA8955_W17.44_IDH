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
#include "spal_mem.h"
// CHIP
#include "global_macros.h"
#include "tcu.h"
#ifdef ENABLE_PATCH_SPC_SCHRXPROCESS

//=============================================================================
// spc_SchProcess()
//-----------------------------------------------------------------------------
/// This function processes the Synchro Burst reception
//=============================================================================
PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spc_SchProcess()
{
    BOOL  EqualizeDecodeSchFlag = FALSE;
    UINT16 i;

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if((g_spcCtx->IsSaicEnable))
    {

        for(i = 0; i <(2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE) ; i++)
        {
            g_spcStaticBufTabSaic[i] = g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)*2 + 2*i];
            g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_SCH_MARGIN)*4 + i] = g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)*2 + 2*i +1];
        }

        for(i = 0; i <(2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE) ; i++)
        {
            g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)*2 + i] = g_spcStaticBufTabSaic[i];
            g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)*2 + (2*BB_RX_SCH_MARGIN+BB_BURST_ACTIVE_SIZE) + i] =
                g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_SCH_MARGIN)*4 + i];
        }

        EqualizeDecodeSchFlag = patch_spp_EqualizeDecodeSch((UINT32 *)&(g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)*2]),
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->SCH_Result);

    }
    else
    {
        EqualizeDecodeSchFlag = spp_EqualizeDecodeSch((UINT32 *)&(g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]),
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->SCH_Result);
    }
#else
    EqualizeDecodeSchFlag = spp_EqualizeDecodeSch((UINT32 *)&(g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]),
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            &g_spcCtx->SCH_Result);
#endif




    if (EqualizeDecodeSchFlag)
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
#endif

#ifdef ENABLE_PATCH_SPC_CCHRXPROCESS

UINT16 SPAL_BBSRAM_PATCH_DATA Copy_ChModeSaic;
UINT8  SPAL_BBSRAM_PATCH_DATA Rx_q_CCHSaic[4][8];
UINT8  SPAL_BBSRAM_PATCH_DATA Rx_CS_Saic[4];
UINT8  SPAL_BBSRAM_PATCH_DATA decodedUsfSaic[4];
UINT32  SPAL_BBSRAM_PATCH_UCDATA SACCHSaic[128];

#if ((defined GCF_TEST)&&(defined ENABLE_PATCH_SPP_GETUSF))
extern UINT8 g_mailbox_pal2spc_statWin_rx_Usf[4];
UINT8 SPAL_INTSRAM_PATCH_UCDATA rx_Usf_Decode[4] = {0xFF,0xFF,0xFF,0xFF};
UINT8 SPAL_INTSRAM_PATCH_UCDATA rx_Usf_Decode_Saic[4] = {0xFF,0xFF,0xFF,0xFF};
UINT8 SPAL_INTSRAM_PATCH_UCDATA g_mailbox_pal2spc_statWin_rx_Usf_ind = 0xFF;
UINT8 SPAL_INTSRAM_PATCH_UCDATA g_rx_Usf_Decode_ind = 0xFF;
//UINT8 SPAL_INTSRAM_PATCH_UCDATA g_rx_Snr_ind = 0;
#endif
#if ((defined CHIP_DIE_8955) && (SPC_IF_VER == 5))
UINT8 SPAL_INTSRAM_PATCH_UCDATA g_mailbox_pal2spc_RxBufInd = 0xff;
#endif

//=============================================================================
// spc_CchDecDoneIrq()
//-----------------------------------------------------------------------------
/// This function generate CCH dec done irq
//=============================================================================
PRIVATE VOID spc_CchDecDoneIrq()
{
    if(g_spcCtx->statWinRxCfg[0].itr &
            MBOX_DSP_CCH_DECODING_COMPLETE)
    {
        spal_ComregsSetIrq(MBOX_DSP_CCH_DECODING_COMPLETE);
    }
}

//=============================================================================
// spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
INT16 SPAL_INTSRAM_PATCH_UCDATA  UsfDistanceNormal= 0x0;
INT16 SPAL_INTSRAM_PATCH_UCDATA  DistanceMaxNormal = 0x0;
INT16 SPAL_INTSRAM_PATCH_UCDATA  UsfDistanceSaic= 0x0;
INT16 SPAL_INTSRAM_PATCH_UCDATA  DistanceMaxSaic = 0x0;
extern  INT16 gUsfDistance;
extern  INT16 gDistance_max;
PUBLIC VOID SPAL_BBSRAM_PATCH_TEXT patch_spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
{
    // default value set for SACCH
    UINT32 *ItlvOutput=(UINT32*)(g_spcCtx->Malloc) ;
    // default value set for SACCH
    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT32 *EqualizerInput;
    UINT32 *EqualizerOutput;
    UINT8 BuffIdx = spRx->buffIdx+spRx->slotIdx;
    UINT8 i=0;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    UINT32* RestAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT8 decodeFlagTmp = 0;
    UINT32 *ItlvOutputSaic = ((UINT32 *) &SACCHSaic[0]);
    UINT32* SoftBitAddressSaic = g_spcCtx->ItlvBufRxCtrlSaic;
    UINT32* EqualizerOutputSaic  = g_spcCtx->ItlvBufRxCtrlSaic;
#endif


    SPC_PROFILE_FUNCTION_ENTER(spc_CCHRxProcess);

    if (g_spcCtx->DedicatedActive==FALSE)
    {
        SoftBitAddress = g_spcCtx->ItlvBufRxCtrl+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((g_spcCtx->IsSaicEnable))
        {
            SoftBitAddressSaic = g_spcCtx->ItlvBufRxCtrlSaic+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;
        }
#endif
    }

    g_spcCtx->NBurst_result.Dco.i = 0;
    g_spcCtx->NBurst_result.Dco.q = 0;



    //Adjacent Channel Interference detection

    //----------------------------- EQUALIZE  ---------------------------------


#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)

    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerInput = (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                      2*(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);

        for(i = 0; i < BB_BURST_TOTAL_SIZE; i += 4)
        {
            g_spcStaticBufTabSaic[i+1] = EqualizerInput[2*i];
            g_spcStaticBufTabSaic[i + 159] = EqualizerInput[2*i + 1];
            g_spcStaticBufTabSaic[i+2] = EqualizerInput[2*i+2];
            g_spcStaticBufTabSaic[i + 160] = EqualizerInput[2*i + 3];
            g_spcStaticBufTabSaic[i+3] = EqualizerInput[2*i + 4];
            g_spcStaticBufTabSaic[i + 161] = EqualizerInput[2*i + 5];
            g_spcStaticBufTabSaic[i+4] = EqualizerInput[2*i + 6];
            g_spcStaticBufTabSaic[i + 162] = EqualizerInput[2*i + 7];
        }
        EqualizerInput = (UINT32 *) &g_spcStaticBufTabSaic[0];
    }
    else
    {

        EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*1*
                                    (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
    }

#else

    EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);


#endif

    EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 32);

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerOutputSaic = (UINT32 *) (SoftBitAddressSaic + spRx->burstIdx * 32);
    }
#endif


#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if (g_spcCtx->IsSaicEnable)
    {
        spp_EqualizeNBurstSaic(EqualizerInput,
                               g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                               EqualizerOutput,
                               EqualizerOutputSaic,
                               (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                               &g_spcCtx->NBurst_result);

        Rx_q_CCHSaic[spRx->slotIdx][ spRx->burstIdx << 1     ] =
            ((EqualizerOutputSaic[28]) >> 16) & 0x0FF;
        Rx_q_CCHSaic[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
            ( EqualizerOutputSaic[28]) >> 24;
    }
    else
    {
        spp_EqualizeNBurst(EqualizerInput,
                           g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                           EqualizerOutput,
                           (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                           &g_spcCtx->NBurst_result);
    }


#else
    spp_EqualizeNBurst(EqualizerInput,
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);
#endif
#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
if (g_spcCtx->statWinRxCfg[spRx->buffIdx].type == 1)//pdch type
{
	if (g_mailbox_pal2spc_RxBufInd == 1)
	{
	    RestAddress = (UINT32 *) SoftBitAddress;
	    if((spRx->burstIdx) == 1)
	    {
	        for(i = 0; i < 32; i++)
	        {
	            RestAddress[i] = 0;
	        }
	    }
	 if(g_spcCtx->sched_slot_count==g_spcCtx->sched_rx_slot_nb)
	     g_mailbox_pal2spc_RxBufInd=0x2;
	}
}
#endif
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][ spRx->burstIdx << 1     ] =
        ((EqualizerOutput[28]) >> 16) & 0x0FF;
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
        ( EqualizerOutput[28]) >> 24;

    ////////////////////////////////////////////////////////
    // check the dummy PCH burst detection is enabled
    if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_PCH_READY)
    {
        UINT8 result=0;
        result = spp_DetectDummyPCH(EqualizerOutput,
                                    spRx->burstIdx,
                                    ((g_mailbox.dummyPchThresh)&0xFF00)>>8,
                                    (g_mailbox.dummyPchThresh)&0xFF);

        if (g_spcCtx->NBurst_result.Snr <= (g_spcCtx->DummySnrThreshold))
            result = SPP_DUMMY_PCH_NOT_DETECTED;
        g_mailbox.dummyPchResult = (UINT32) result;
        spal_ComregsSetIrq(MBOX_DSP_PCH_READY);
    }
    ////////////////////////////////////////////////////////

    //----------------------------- EQUALIZE  ---------------------------------
    //----------------------------  RESULTS  ----------------------------------

    UINT8 bstSnr = (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
    if (spRx->slotIdx==0)
    {
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
            (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
            g_spcCtx->NBurst_result.FOf;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=bstSnr;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
            g_spcCtx->NBurst_result.Pwr;
#ifdef CHIP_DIE_8955
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_I = g_spcCtx->NBurst_result.Dco.i;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_Q = g_spcCtx->NBurst_result.Dco.q;
#endif
    }
    else// save multislot pwr and snr
    {
#ifdef CHIP_DIE_8955
        g_mailbox.spc2pal.bstSnr[4*g_spcCtx->currentSnap + spRx->slotIdx] = bstSnr;
        g_mailbox.spc2pal.pwrs[4*g_spcCtx->currentSnap + spRx->slotIdx] =
            g_spcCtx->NBurst_result.Pwr;
        // Only report the first three dc value.
        if (spRx->slotIdx <= 2)
        {
            g_mailbox.spc2pal.multiDcI[2*g_spcCtx->currentSnap + spRx->slotIdx-1] =
                g_spcCtx->NBurst_result.Dco.i;
            g_mailbox.spc2pal.multiDcQ[2*g_spcCtx->currentSnap + spRx->slotIdx-1] =
                g_spcCtx->NBurst_result.Dco.q;
        }
#endif

    }
    //----------------------------  RESULTS  ----------------------------------
    //---------------------------- CIPHERING ----------------------------------
    if (g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((g_spcCtx->IsSaicEnable))
        {
            spp_CipherDecipherNb(EqualizerOutputSaic,EqualizerOutputSaic);
        }
#endif

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
#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((g_spcCtx->IsSaicEnable))
        {
            spp_DecodeStealingBits((UINT32 *) (Rx_q_CCHSaic[spRx->slotIdx]),
                                   1,
                                   &(Rx_CS_Saic[spRx->slotIdx]),
                                   0);
            // default value for SACCH
            Copy_ChModeSaic = CCH_mode + (GSM_CS1<<8);
        }
#endif

        //--------------------------  DEINTERLEAVE  ---------------------------
        if (g_spcCtx->DedicatedActive==FALSE)
        {
            if (spRx->slotIdx == 0)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     SPC_TMP_ITLV_RX_BUFF_OFFSET);
            }
            else if(spRx->slotIdx == g_spcCtx->sched_rx_slot_nb-1)
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


#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            if (g_spcCtx->DedicatedActive==FALSE)
            {
                if (spRx->slotIdx == 0)
                {
                    ItlvOutputSaic = ((UINT32 *) &SACCHSaic[0]);
                }
                else
                {
                    ItlvOutputSaic = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic +
                                               (spRx->slotIdx-1)*SPC_ITLV_RX_BUFF_OFFSET);
                }
            }
            spp_DeinterleaveBlock(SoftBitAddressSaic,
                                  ITLV_TYPE_1B,
                                  0,
                                  (UINT32*)ItlvOutputSaic);
        }
#endif

        //--------------------------  DEINTERLEAVE  ---------------------------
#if  ((defined GCF_TEST)&&(defined ENABLE_PATCH_SPP_GETUSF))
        //g_rx_Snr_ind = bstSnr;
        g_mailbox_pal2spc_statWin_rx_Usf_ind = g_mailbox_pal2spc_statWin_rx_Usf[spRx->slotIdx];
        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            patch_spp_GetUSF((UINT32*)ItlvOutput,
                             g_spcCtx->Rx_CS[spRx->slotIdx]);
        rx_Usf_Decode[spRx->slotIdx] = g_rx_Usf_Decode_ind;
#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((g_spcCtx->IsSaicEnable))
        {
            g_mailbox_pal2spc_statWin_rx_Usf_ind = g_mailbox_pal2spc_statWin_rx_Usf[spRx->slotIdx];
            decodedUsfSaic[spRx->slotIdx] =
                patch_spp_GetUSF((UINT32*)ItlvOutputSaic,
                                 Rx_CS_Saic[spRx->slotIdx]);
            rx_Usf_Decode_Saic[spRx->slotIdx] = g_rx_Usf_Decode_ind;
        }
#endif

#else
#if 0
        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            spp_GetUSF((UINT32*)ItlvOutput,
                       g_spcCtx->Rx_CS[spRx->slotIdx]);
#endif
        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            patch_spp_GetUSF1((UINT32*)ItlvOutput,
                       g_spcCtx->Rx_CS[spRx->slotIdx]);

        UsfDistanceNormal = gUsfDistance;
        DistanceMaxNormal = gDistance_max;	
#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((g_spcCtx->IsSaicEnable))
        {
#if 0        
            decodedUsfSaic[spRx->slotIdx] =
                spp_GetUSF((UINT32*)ItlvOutputSaic,
                           Rx_CS_Saic[spRx->slotIdx]);
#endif

            decodedUsfSaic[spRx->slotIdx] =
                patch_spp_GetUSF1((UINT32*)ItlvOutputSaic,
                           Rx_CS_Saic[spRx->slotIdx]);
            UsfDistanceSaic= gUsfDistance;
            DistanceMaxSaic = gDistance_max;
            if ((g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] == 0xFF) && (decodedUsfSaic[spRx->slotIdx] != 0xFF))
            {
                g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] = decodedUsfSaic[spRx->slotIdx];
            }
            else if ((g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] != 0xFF) && (decodedUsfSaic[spRx->slotIdx] != 0xFF))
            {
                INT32 temp0;
                INT32 temp1;
                temp0 = (INT32)UsfDistanceNormal * DistanceMaxSaic;
                temp1 = (INT32)DistanceMaxNormal * UsfDistanceSaic;
                if ((temp0 != 0) && (temp1 != 0))
                {
                    if (temp0 < temp1) 
                    {
                        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] = decodedUsfSaic[spRx->slotIdx];
                    }
                }
            }
            else
            {
            }
        }
#endif
#endif

        if(g_spcCtx->sched_slot_count==g_spcCtx->sched_rx_slot_nb)
        {
            UINT8 k=0;

            // send an it to the XCPU if the last slot has been received
            if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_USF_READY)
            {
                spal_ComregsSetIrq(MBOX_DSP_USF_READY);
            }
            for (k=0; k<g_spcCtx->sched_rx_slot_nb; k++)
            {
                BuffIdx = spRx->buffIdx+k;
                if (g_spcCtx->DedicatedActive==FALSE)
                {
                    if (k == 0)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             SPC_TMP_ITLV_RX_BUFF_OFFSET);
                    }
                    else if(k == g_spcCtx->sched_rx_slot_nb-1)
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

#if ((defined GCF_TEST)&&(defined ENABLE_PATCH_SPP_GETUSF))
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


#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
                if( (g_spcCtx->NBlock_result.Bfi&0x1) != 0)
                {
                    if(g_spcCtx->IsSaicEnable)
                    {
                        if (g_spcCtx->DedicatedActive==FALSE)
                        {
                            if (k == 0)
                            {
                                ItlvOutputSaic = ((UINT32 *) &SACCHSaic[0]);
                            }
                            else
                            {
                                ItlvOutputSaic = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic +
                                                           (k-1)*SPC_ITLV_RX_BUFF_OFFSET);
                            }
                            Copy_ChModeSaic=CCH_mode +
                                            ((Rx_CS_Saic[k])<<8);
                            g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
                                (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
                            spRx->BlockDecodedAdd=g_spcCtx->NBlockDataOut[BuffIdx];
                        }
#if  ((defined GCF_TEST)&&(defined ENABLE_PATCH_SPP_GETUSF))
                        spp_DecodeBlock((UINT32*)ItlvOutputSaic,
                                        Copy_ChModeSaic,
                                        0,
                                        0,
                                        spRx->BlockDecodedAdd,
                                        (rx_Usf_Decode_Saic[k])&0x7,
                                        (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                        &g_spcCtx->NBlock_result);
#else
                        spp_DecodeBlock((UINT32*)ItlvOutputSaic,
                                        Copy_ChModeSaic,
                                        0,
                                        0,
                                        spRx->BlockDecodedAdd,
                                        (decodedUsfSaic[k])&0x7,
                                        (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                        &g_spcCtx->NBlock_result);
#endif
                        if ((g_spcCtx->NBlock_result.Bfi&0x1) == 0)
                        {
                            decodeFlagTmp = 1;
                            g_mailbox.spc2pal.statWin.rx.decodedUsf[k] = decodedUsfSaic[k];
                            spRx->Copy_ChMode = Copy_ChModeSaic;

                        }
                    }
                    else
                    {
                        decodeFlagTmp = 0;
                    }
                }
#endif
#if 0
                // dump the decoder output bits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_OUT)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)spRx->BlockDecodedAdd,
                        28,
                        SPC_DBG_DUMP_DEC_OUT_MARKER);
                }

                spc_CchDecDoneIrq();
#endif

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)

                if(g_spcCtx->IsSaicEnable)
                {
                    if (decodeFlagTmp)
                    {
                        g_mailbox.spc2pal.decodeFlag |= (1 << BuffIdx);
                    }
                    else
                    {
                        g_mailbox.spc2pal.decodeFlag &= (~(1 << BuffIdx));
                    }
                }
                else
                {
                    g_mailbox.spc2pal.decodeFlag = 0;
                }
#endif
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

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
                if(decodeFlagTmp)
                {
                    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                        Rx_CS_Saic[k];
                }
                else
                {
                    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                        g_spcCtx->Rx_CS[k];
                }
#else
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                    g_spcCtx->Rx_CS[k];
#endif
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].sid =
                    g_spcCtx->NBlock_result.Voc_Sid |
                    (spRx->RxFrameType==SPP_RX_SID_UPDATE);

            }
            // reset the Rx Ctrl buffer to allow decoding with only two bursts
            for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrl[i]=0;

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
            if(g_spcCtx->IsSaicEnable)
            {
                for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrlSaic[i]=0;
            }
			g_mailbox_pal2spc_RxBufInd = 0xff;
			spal_ComregsSetIrq( MBOX_DSP_NB_DECODING_COMPLETE);

#endif

        }
    }

    //----------------------------  DECODE  -----------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_CCHRxProcess);
}
#endif

#ifdef ENABLE_PATCH_SPC_TCHRXPROCESS
//=============================================================================
// patch_spc_BfiFilter()
//-----------------------------------------------------------------------------
/// This function adds a BFI to the speech frame when needed
//=============================================================================
PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spc_BfiFilter(SPC_RX_LOCALS_T *spRx)
{
#define LOOPBACK_FSBFIBER2THRESH  0
#define LOOPBACK_FSBFIBERTHRESH 40
#define LOOPBACK_FSBFISNRTHRESH -8
#define LOOPBACK_HSBFIBER2THRESH  0
#define LOOPBACK_HSBFIBERTHRESH 30
#define LOOPBACK_HSBFISNRTHRESH 1
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
            goto LABEL_BLOCKSNR;
            LoopBackMode = 1;
        }
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
LABEL_BLOCKSNR:
    g_spcCtx->LastBlockSnR = g_spcCtx->BlockSnR;
}
UINT32 SPAL_BBSRAM_PATCH_UCDATA NBlockDataOutSaic[5][14] = {0};

UINT8 SPAL_BBSRAM_PATCH_UCDATA g_rx_Saic_Bfi_Last_ind = 0xff;
//=============================================================================
// spc_TCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT patch_spc_TCHRxProcess(SPC_RX_LOCALS_T *spRx)
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

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    SPP_N_BLOCK_RESULT_T NBlock_result_saic;
    UINT32* BlockDecodedAddSaic = NULL;
    UINT8 decodeFlagTmp = 0;
    UINT32* EqualizerOutputSaic = (UINT32*) (g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL);
    UINT32* ItlvBuffInputSaic  = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL);
    UINT32 *EqualizerInput = NULL;
#endif


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
    EqualizerOutput = (UINT32*) (g_spcCtx->ItlvBufRxDedicated +
                                 RxBurstOffset * 32);

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerOutputSaic = (UINT32*) (g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL +
                                         RxBurstOffset * 32);
    }
#endif

    g_spcCtx->NBurst_result.Dco.i = 0;
    g_spcCtx->NBurst_result.Dco.q = 0;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerInput = (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]);

        for(i = 0; i < BB_BURST_TOTAL_SIZE; i += 4)
        {
            g_spcStaticBufTabSaic[i+1] = EqualizerInput[2*i];
            g_spcStaticBufTabSaic[i + 159] = EqualizerInput[2*i + 1];
            g_spcStaticBufTabSaic[i+2] = EqualizerInput[2*i+2];
            g_spcStaticBufTabSaic[i + 160] = EqualizerInput[2*i + 3];
            g_spcStaticBufTabSaic[i+3] = EqualizerInput[2*i + 4];
            g_spcStaticBufTabSaic[i + 161] = EqualizerInput[2*i + 5];
            g_spcStaticBufTabSaic[i+4] = EqualizerInput[2*i + 6];
            g_spcStaticBufTabSaic[i + 162] = EqualizerInput[2*i + 7];
        }
        EqualizerInput = (UINT32 *) &g_spcStaticBufTabSaic[0];

        spp_EqualizeNBurstSaic(EqualizerInput,
                               g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                               EqualizerOutput,
                               EqualizerOutputSaic,
                               (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                               &g_spcCtx->NBurst_result);


    }
    else
    {
        spp_EqualizeNBurst((UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]),
                           g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                           EqualizerOutput,
                           (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                           &g_spcCtx->NBurst_result);
    }

#else
    spp_EqualizeNBurst((UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]),
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);
#endif


    // Store Stealing Flag
    g_spcCtx->Rx_Hu_TCH_N <<= 8;
    g_spcCtx->Rx_Hl_TCH_N <<= 8;
    g_spcCtx->Rx_Hu_TCH_N |= ( EqualizerOutput[28]) >> 24;
    g_spcCtx->Rx_Hl_TCH_N |= ((EqualizerOutput[28]) >> 16) & 0x0FF;


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
#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if(g_spcCtx->IsSaicEnable)
    {	
        if(g_rx_Saic_Bfi_Last_ind == 0)
		{
			if (g_spcCtx->NBurst_result.Snr <= 1)
			{
				g_spcCtx->NBurst_result.Snr = 1;
				if (g_spcCtx->NBurst_result.FOf > 15)
					g_spcCtx->NBurst_result.FOf = 15;
				if (g_spcCtx->NBurst_result.FOf <-15)
					g_spcCtx->NBurst_result.FOf = -15;
				if(g_spcCtx->NBurst_result.TOf > 18)
					g_spcCtx->NBurst_result.TOf = 18;
				if(g_spcCtx->NBurst_result.TOf < 14)
					g_spcCtx->NBurst_result.TOf = 14;		
			}
		}
    }
	
#endif	
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
        (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
        g_spcCtx->NBurst_result.FOf;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
        (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
        g_spcCtx->NBurst_result.Pwr;
#ifdef CHIP_DIE_8955
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_I = g_spcCtx->NBurst_result.Dco.i;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_Q = g_spcCtx->NBurst_result.Dco.q;
#endif

    //-----------------------------  RESULTS  ----------------------------------
    //----------------------------- CIPHERING ----------------------------------
    if(g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            spp_CipherDecipherNb(EqualizerOutputSaic,EqualizerOutputSaic);
        }
#endif

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

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            ItlvBuffInputSaic = g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL;
        }
#endif

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
#ifdef CHIP_DIE_8955
            if ((g_spcCtx->NBlock_result.Bfi&0x1) == 0)
            {
                decodeFlagTmp = 0;
            }
#endif
        }
#ifdef CHIP_DIE_8955
        else
        {
            decodeFlagTmp = 0;
        }
#endif

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(spRx->RX_decode==DECODE_SPEECH)
        {
            //g_spcCtx->NBlock_result.Bfi = 1;
            if(g_spcCtx->IsSaicEnable)
            {
                // initialize the output buffer to avoid bit error due to tail bits
                for(i=0; i<14; i++)
                {
                    NBlockDataOutSaic[spRx->buffIdx][i] = 0;
                }
// initialize the block decoding output address
                //spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[spRx->buffIdx];
                BlockDecodedAddSaic = NBlockDataOutSaic[spRx->buffIdx];
                // Deinterleave the current block
                spp_DeinterleaveBlock(ItlvBuffInputSaic,
                                      spRx->itlv_mode,
                                      spRx->RxBuffOffset,
                                      (UINT32*)g_spcCtx->Malloc);

                spp_DecodeBlock((UINT32*)g_spcCtx->Malloc,
                                spRx->Copy_ChMode,
                                0,
                                0,
                                BlockDecodedAddSaic,
                                (g_mailbox.spc2pal.statWin.rx.decodedUsf[0])&0x7,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &NBlock_result_saic);

                if ((NBlock_result_saic.Bfi&0x1) == 0)
                {
                    decodeFlagTmp = 1;
                    if (g_spcCtx->BlockSnR == 0) 
                    g_spcCtx->BlockSnR = g_spcCtx->BlockSnR + 1;
                }
                else
                {
                    decodeFlagTmp = 0;
                }

            }

        }
#endif

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            if (decodeFlagTmp)
            {
                g_mailbox.spc2pal.decodeFlag |= (1 << spRx->buffIdx);
            }
            else
            {
                g_mailbox.spc2pal.decodeFlag &= (~(1 << spRx->buffIdx));
            }
        }
        else
        {
            g_mailbox.spc2pal.decodeFlag = 0;
        }
#endif

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if ((g_spcCtx->IsSaicEnable)&&(spRx->RX_decode==DECODE_SPEECH))
        {
            if ((((NBlock_result_saic.Bfi&0x1) == (g_spcCtx->NBlock_result.Bfi&0x1))&&
                    ((NBlock_result_saic.BitError) < (g_spcCtx->NBlock_result.BitError))) || (((NBlock_result_saic.Bfi&0x1) == 0)&&((g_spcCtx->NBlock_result.Bfi&0x1) == 1)))
            {
                for(i=0; i<14; i++)
                {
                    g_spcCtx->NBlockDataOut[spRx->buffIdx][i] = NBlockDataOutSaic[spRx->buffIdx][i];
                }
                g_spcCtx->NBlock_result.MeanBEP = NBlock_result_saic.MeanBEP;
                g_spcCtx->NBlock_result.CvBEP = NBlock_result_saic.CvBEP;
                //g_spcCtx->NBlock_result.BitError = NBlock_result_saic.BitError;
               // g_spcCtx->NBlock_result.BitTotal = NBlock_result_saic.BitTotal;
                g_spcCtx->NBlock_result.Bfi = NBlock_result_saic.Bfi;
				g_rx_Saic_Bfi_Last_ind = NBlock_result_saic.Bfi;
                g_spcCtx->NBlock_result.Voc_Sid = NBlock_result_saic.Voc_Sid;
            }
            else
            {
                g_rx_Saic_Bfi_Last_ind = 1;
            }			
            if (((NBlock_result_saic.Bfi&0x1) == (g_spcCtx->NBlock_result.Bfi&0x1)) || (((NBlock_result_saic.Bfi&0x1) == 0)&&((g_spcCtx->NBlock_result.Bfi&0x1) == 1)))
            {
                if ((NBlock_result_saic.BitError) > (g_spcCtx->NBlock_result.BitError))
                {
                    g_spcCtx->NBlock_result.BitError = NBlock_result_saic.BitError;
                    g_spcCtx->NBlock_result.BitTotal = NBlock_result_saic.BitTotal;
                    g_spcCtx->loopC_buffer = (UINT8 *) EqualizerOutputSaic;
                }
            }
        }
#endif
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
            patch_spc_BfiFilter(spRx);
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
#endif


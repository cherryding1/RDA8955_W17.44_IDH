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










#include "spc_ctx.h"
#include "spc_private.h"
#include "spcp_debug.h"
#include "spcp_amr.h"
#include "spc_profile_codes.h"
#include "spp_gsm.h"

#include "spal_mem.h"


#ifdef ENABLE_PATCH_SPC_AMRDECODE
// ****************************************************************************
//  spc_AmrDecode():
//
//      this function decodes one AMR frame, updates Tx & Rx codec mode
//      and analyses the RATSCCH message if one was detected
//
//
// ****************************************************************************
VOID SPAL_INTSRAM_PATCH_TEXT  patch_spc_AmrDecode(SPC_RX_LOCALS_T *spRx)
{
    UINT8 cmi=g_spcCtx->RxCodecMode;
    UINT8 cmc=g_spcCtx->TxCodecMode;
    UINT8 T2=g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2;
    UINT8 i,j;
    SPP_N_BLOCK_RESULT_T NBlock_result_saic;
    UINT8 RxFrameTypeSaic;
    UINT8 ratscch_detected_saic;
    UINT32  BlockDecodedAddSaic[14] = {0};

    SPC_PROFILE_FUNCTION_ENTER(spc_AmrDecode);
    if (RATE(g_spcCtx->ChMode)==FR_RATE)
    {
        spc_AFSdecoding( g_spcCtx->AMR_Cfg_Rx,
                         spRx->ItlvBuffInput,
                         (UINT32*) g_spcCtx->Malloc,
                         spRx->RxBuffOffset,
                         spRx->BlockDecodedAdd,
                         T2,
                         &g_spcCtx->NBlock_result,
                         &cmc,
                         &cmi,
                         &spRx->RxFrameType,
                         &g_spcCtx->ratscch_detected);

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if (g_spcCtx->IsSaicEnable)
        {
            spc_AFSdecoding( g_spcCtx->AMR_Cfg_Rx,
                             (UINT32*) (g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL),
                             (UINT32*) g_spcCtx->Malloc,
                             spRx->RxBuffOffset,
                             BlockDecodedAddSaic,
                             T2,
                             &NBlock_result_saic,
                             &cmc,
                             &cmi,
                             &RxFrameTypeSaic,
                             &ratscch_detected_saic);
            if ((((NBlock_result_saic.Bfi&0x1) == (g_spcCtx->NBlock_result.Bfi&0x1))&&
                    ((NBlock_result_saic.BitError) < (g_spcCtx->NBlock_result.BitError))) || (((NBlock_result_saic.Bfi&0x1) == 0)&&((g_spcCtx->NBlock_result.Bfi&0x1) == 1)))
            {
                for(i=0; i<14; i++)
                {
                    spRx->BlockDecodedAdd[i] = BlockDecodedAddSaic[i];
                }
                g_spcCtx->NBlock_result.MeanBEP = NBlock_result_saic.MeanBEP;
                g_spcCtx->NBlock_result.CvBEP = NBlock_result_saic.CvBEP;
                //g_spcCtx->NBlock_result.BitError = NBlock_result_saic.BitError;
                //g_spcCtx->NBlock_result.BitTotal = NBlock_result_saic.BitTotal;
                g_spcCtx->NBlock_result.Bfi = NBlock_result_saic.Bfi;
                g_spcCtx->NBlock_result.Voc_Sid = NBlock_result_saic.Voc_Sid;
                spRx->RxFrameType = RxFrameTypeSaic;
                g_spcCtx->ratscch_detected = ratscch_detected_saic;
            }
        }
#endif

    }

    else
    {
        spc_AHSdecoding( g_spcCtx->AMR_Cfg_Rx,
                         spRx->ItlvBuffInput,
                         (UINT32*) g_spcCtx->Malloc,
                         spRx->RxBuffOffset,
                         spRx->BlockDecodedAdd,
                         T2,
                         &g_spcCtx->NBlock_result,
                         &cmc,
                         &cmi,
                         &spRx->RxFrameType,
                         &g_spcCtx->ratscch_detected);



#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if (g_spcCtx->IsSaicEnable)
        {

            spc_AHSdecoding( g_spcCtx->AMR_Cfg_Rx,
                             (UINT32*) (g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL),
                             (UINT32*) g_spcCtx->Malloc,
                             spRx->RxBuffOffset,
                             BlockDecodedAddSaic,
                             T2,
                             &NBlock_result_saic,
                             &cmc,
                             &cmi,
                             &RxFrameTypeSaic,
                             &ratscch_detected_saic);
            if ((((NBlock_result_saic.Bfi&0x1) == (g_spcCtx->NBlock_result.Bfi&0x1))&&
                    ((NBlock_result_saic.BitError) < (g_spcCtx->NBlock_result.BitError))) || (((NBlock_result_saic.Bfi&0x1) == 0)&&((g_spcCtx->NBlock_result.Bfi&0x1) == 1)))
            {
                for(i=0; i<14; i++)
                {
                    spRx->BlockDecodedAdd[i] = BlockDecodedAddSaic[i];
                }
                g_spcCtx->NBlock_result.MeanBEP = NBlock_result_saic.MeanBEP;
                g_spcCtx->NBlock_result.CvBEP = NBlock_result_saic.CvBEP;
                //g_spcCtx->NBlock_result.BitError = NBlock_result_saic.BitError;
                //g_spcCtx->NBlock_result.BitTotal = NBlock_result_saic.BitTotal;
                g_spcCtx->NBlock_result.Bfi = NBlock_result_saic.Bfi;
                g_spcCtx->NBlock_result.Voc_Sid = NBlock_result_saic.Voc_Sid;
                spRx->RxFrameType = RxFrameTypeSaic;
                g_spcCtx->ratscch_detected = ratscch_detected_saic;
            }
        }
#endif       
    }

    // update TxCodecMode and RxCodecMode
    if(cmc!=g_spcCtx->TxCodecMode)
    {
        for (i=0; i<g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet; i++)
        {
            if (cmc==g_spcCtx->AMR_Cfg_Tx.activeCodecSet[i]) break;
        }
        for (j=0; j<g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet; j++)
        {
            if (g_spcCtx->TxCodecMode==g_spcCtx->AMR_Cfg_Tx.activeCodecSet[j]) break;
        }
        if (i<g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet)
        {
            if (i<j) g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[j-1];
            else     g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[j+1];
        }
    }

#if 1
    g_spcCtx->RxCodecMode=cmi;
#else
    if(cmi!=g_spcCtx->RxCodecMode)
    {
        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; i++)
        {
            if (cmi==g_spcCtx->AMR_Cfg_Rx.activeCodecSet[i]) break;
        }
        for (j=0; j<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; j++)
        {
            if (g_spcCtx->RxCodecMode==g_spcCtx->AMR_Cfg_Rx.activeCodecSet[j]) break;
        }
        if (i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet)
        {
            if (i<j) g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[j-1];
            else     g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[j+1];
        }
        if(ABS(i-j)>1)
        {
            if(spRx->RxFrameType != SPP_RX_SPEECH_BAD)
            {
                g_spcCtx->RxCodecMode=cmi;
            }
            else
            {
                g_spcCtx->NBlock_result.Bfi=1;
                spRx->RxFrameType=SPP_RX_SPEECH_BAD;
            }
        }
    }
#endif

    if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I)  )
    {
        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; i++)
        {
            if (cmi==g_spcCtx->AMR_Cfg_Rx.activeCodecSet[i]) break;
        }
        g_spcCtx->CodecModeRequest=i;
        if (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) g_spcCtx->TxCodecMode=cmc;
        else g_spcCtx->TxCodecMode=cmi;
    }

    // processing of RATSCCH request
    if (g_spcCtx->ratscch_detected==1)
    {
        g_spcCtx->ratscch_detected=0;
        if (g_spcCtx->REQ_Activation==0)   //if no ratscch request is already pending
        {
            if(g_spcCtx->NBlock_result.Bfi)
                g_spcCtx->ratscch_to_encode=ACK_ERR;
            else
            {
                UINT8 thresh[3]= {0x3F,0x3F,0x3F};
                UINT8 hyst[3]= {0xF,0xF,0xF};
                UINT8 acs=((spRx->BlockDecodedAdd[0])>>20)&0xFF;
                UINT8 icm=((spRx->BlockDecodedAdd[0])>>28)&0x3;
                UINT8 nb_actv=0;
                UINT8 actv_codec_set[4]= {0,0,0,0};
                g_spcCtx->ratscch_to_encode=ACK_OK;
                // decoding of the ratscch request
                // switch regarding bits 34 33 32 31 30
                switch((((spRx->BlockDecodedAdd[1])&0x7)<<2)|((spRx->BlockDecodedAdd[0])>>30))
                {
                    case 0:     // CMI_PHASE_REQ
                        if (((spRx->BlockDecodedAdd[0])>>1)==2)
                        {
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase=(spRx->BlockDecodedAdd[0])&1;
                            g_spcCtx->REQ_Activation=1;
                            g_spcCtx->RATSCCH_REQ=CMI_PHASE_REQ;
                        }
                        //if BlockDecodedAdd[0]==ACK_OK / ACK_ERR / ACK_UNKNOWN do nothing
                        else if (((spRx->BlockDecodedAdd[0])==1)||
                                 ((spRx->BlockDecodedAdd[0])==2)||
                                 ((spRx->BlockDecodedAdd[0])==3)) g_spcCtx->ratscch_to_encode=0;
                        else g_spcCtx->ratscch_to_encode=ACK_UNKNOWN;
                        break;

                    case 1:     // THRESH_REQ
                        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet-1; i++)
                        {
                            thresh[i] =((spRx->BlockDecodedAdd[0])>>(i*10))&0x3F;
                            hyst[i]=((spRx->BlockDecodedAdd[0])>>(i*10+6))&0xF;
                        }
                        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet-1; i++)
                        {
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i] =thresh[i];
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i]=hyst[i];
                        }
                        g_spcCtx->REQ_Activation=1;
                        g_spcCtx->RATSCCH_REQ=THRESH_REQ;
                        break;

                    case 7:     // AMR_CONFIG_REQ
                        for (i=0; i<8; i++) if ((acs>>i)&1) {actv_codec_set[nb_actv]=i; nb_actv++;}
                        if (nb_actv>4) g_spcCtx->ratscch_to_encode=ACK_UNKNOWN;
                        else
                        {
                            if (((spRx->BlockDecodedAdd[0])&0xFFFFF)!=0xFFFFF)
                            {
                                if (nb_actv==4)
                                {
                                    thresh[0]=(spRx->BlockDecodedAdd[0])&0x3F;
                                    thresh[1]=((spRx->BlockDecodedAdd[0])>>6)&0x3F;
                                    thresh[2]=((spRx->BlockDecodedAdd[0])>>12)&0x3F;
                                    hyst[0]=(((spRx->BlockDecodedAdd[0])>>18)&0x3)<<1;
                                    hyst[1]=hyst[0];
                                    hyst[2]=hyst[0];
                                }
                                else  //normal format
                                {
                                    thresh[0]=(spRx->BlockDecodedAdd[0])&0x3F;
                                    hyst[0]=((spRx->BlockDecodedAdd[0])>>6)&0xF;
                                    thresh[1]=((spRx->BlockDecodedAdd[0])>>10)&0x3F;
                                    hyst[1]=((spRx->BlockDecodedAdd[0])>>16)&0xF;
                                }
                            }
                            // thresh & hyst are updated even if there's no value available
                            for (i=1; i<4; i++)
                            {
                                g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i-1]=thresh[i-1];
                                g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i-1]=hyst[i-1];
                            }
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx=icm;
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet=nb_actv;
                            for (i=0; i<4; i++)
                                g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[i]=actv_codec_set[i];
                            g_spcCtx->REQ_Activation=1;
                            g_spcCtx->RATSCCH_REQ=AMR_CONFIG_REQ;
                        }
                        break;

                    default:    // UNKNOWN
                        g_spcCtx->ratscch_to_encode=ACK_UNKNOWN;
                        break;
                } //endif switch
            } //endif bfi
        } //endif Req_Act==0
    } //endif ratscch

    // in case of dtx downlink the sidupdate_ctr is incremented from 1 to 4 or 11 to 18
    switch (spRx->RxFrameType)
    {
        case SPP_RX_NO_DATA:
            if (((g_spcCtx->sidupdate_ctr>0) && (g_spcCtx->sidupdate_ctr < 4))
                    ||((g_spcCtx->sidupdate_ctr>10) && (g_spcCtx->sidupdate_ctr < 18)))
            {
                g_spcCtx->sidupdate_ctr++;
            }
            break;
        case SPP_RX_SID_FIRST:
            g_spcCtx->sidupdate_ctr=1;
            break;
        case SPP_RX_SID_UPDATE:
            g_spcCtx->sidupdate_ctr=11;
            break;
        default:
            g_spcCtx->sidupdate_ctr=0;
            break;
    }

    SPC_PROFILE_FUNCTION_EXIT(spc_AmrDecode);
}
#endif


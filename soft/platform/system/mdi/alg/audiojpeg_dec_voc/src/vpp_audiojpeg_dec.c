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

#include "hal_error.h"
#include "hal_voc.h"

#include "vpp_audiojpeg_dec_asm.h"
#include "mcip_debug.h"
#include "vpp_audiojpeg_dec.h"

#include "calib_m.h"

#include "aud_m.h"

#include "analog_audio_iir.h"

CONST INT16 f_sbc_persition[30]=
{
    2, 2, 2, 4, 4,
    4, 4, 4, 4, 8, 8, 8, 8,
    8, 8, 16, 16, 16, 16, 16, 32,
    32, 32, 32, 32, 64, 64, 64, 63,
    0
};

BOOL vpp_AudioJpegDecVocOpen_status=FALSE;

extern AUD_LEVEL_T audio_cfg;
//   extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

static bool reCalcMp3EQ = 0;
extern SND_ITF_T musicItf;
static AUD_ITF_T lastAudioItf = AUD_ITF_NONE;

//=============================================================================
// vpp_AudioJpegDecOpen function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_AudioJpegDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler)
{
    HAL_VOC_CFG_T cfg;
    vpp_AudioJpeg_DEC_IN_T * pDecIn;
    vpp_AudioJpeg_DEC_OUT_T *pDecStatus;
    INT32 * *pDMA;

    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    diag_printf("[vpp_AudioJpeg_DEC]Opening VPP AUDIO_DEC\n");


    cfg.vocCode              = G_VppCommonDecCode;
    cfg.vocCodeSize        = vpp_AudioJpeg_DEC_CODE_SIZE;
    cfg.pcVal                   = vpp_AudioJpeg_DEC_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = vpp_AudioJpeg_DEC_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = vpp_AudioJpeg_DEC_CRITICAL_SECTION_MAX;
    cfg.needOpenDoneIrq      = FALSE;
    cfg.irqMask.voc          = (vocIrqHandler) ? 1 : 0;
    cfg.irqMask.dmaVoc       = 0;
    cfg.vocIrqHandler        = vocIrqHandler;

    cfg.eventMask.wakeupIfc0 = 0;
    cfg.eventMask.wakeupIfc1 = 0;
    cfg.eventMask.wakeupDmae = 0;
    cfg.eventMask.wakeupDmai = 0;
    cfg.eventMask.wakeupSof0 = 0;
    cfg.eventMask.wakeupSof1 = 0;
    cfg.enableFlashAccess = false;

    // load the VPP AUDIO_DEC  code and configure the VoC resource
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:
            diag_printf("[vpp_AudioJpeg_DEC]##WARNING##Error opening VoC resource\n");
            return HAL_ERR_RESOURCE_BUSY;

        // first open, load the constant tables
        case HAL_ERR_RESOURCE_RESET:
            diag_printf("[vpp_AudioJpeg_DEC]First open.\n");
            break;

        default:
            diag_printf("[vpp_AudioJpeg_DEC]No first open.\n");
            break;
    }

    diag_printf("[vpp_AudioJpeg_DEC]Initializing the DMA addr.\n");
    //mp3
    pDMA = hal_VocGetPointer(VPP_MP3_Code_ExternalAddr_addr);
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3DecConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3DecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    //aac
    pDMA = hal_VocGetPointer(VPP_AAC_Code_ExternalAddr_addr);

    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3Layer12Dec_DMA_ConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    //amr
    pDMA = hal_VocGetPointer(VPP_AMR_Code_ExternalAddr_addr);

    //*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAmrDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMp3Dec_EQ_DMA_ConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    //pDMA=pDMA+1;
    //*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAmrDecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);


#ifdef VIDEO_PLAYER_SUPPORT
    //jpeg
#if 0 //replace by G_VppJpegDec2Code or software.sheen
    pDMA = hal_VocGetPointer(VPP_JPEG_Code_ExternalAddr_addr);

    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJpegDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJpegDecConstY,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif


    pDMA = hal_VocGetPointer(VPP_AAC_Tab_huffTabSpec_START_addr);
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecHuffTabSpec,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecCos4sin4tab,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecTwidTabOdd,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppAacDecSinWindow,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    //h263 zoom
#if 0 //replace by gouda.sheen
    pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif
#endif

    //sbc encode
    pDMA = hal_VocGetPointer(VPP_SBC_ENC_Code_ExternalAddr_addr);
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppSBCCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    //drc mode
    //note: reuse jpeg addr and h263_zoom addr
    pDMA = hal_VocGetPointer(VPP_DRC_MODE_Code_ExternalAddr_addr);
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppDRCCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppDRCConst,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_Mp3DRCHistoryBuf,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

#if 0 //do not use the algorithm
    pDMA = hal_VocGetPointer(VPP_Speak_AntiDistortion_Filter_Coef_addr);
    *pDMA=hal_VocGetDmaiPointer((INT32*)(&(calibPtr->audio_voc->voc[musicItf].mp3.gains[0])),  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif

#if 0 //MP3 VoC decoder not use NonClip, replace by drc mode
    pDMA = hal_VocGetPointer(VPP_GLOBAL_NON_CLIP_HISTORYDATA_L_MP3_ADDR);

#ifdef SMALL_BSS_RAM_SIZE

    G_Mp3NonCliPLBuf_alloc                     = (UINT8*)mmc_MemMalloc_BssRam(G_Mp3NonCliPLBuf_Len+3);
    G_Mp3NonCliPRBuf_alloc                     = (UINT8*)mmc_MemMalloc_BssRam(G_Mp3NonCliPRBuf_Len+3);
    G_Mp3NonCliPLBuf =(UINT32*) ((UINT32)(G_Mp3NonCliPLBuf_alloc +3) & ~0x3);
    G_Mp3NonCliPRBuf =(UINT32*) ((UINT32)(G_Mp3NonCliPRBuf_alloc+3) & ~0x3);

    hal_HstSendEvent(0xaccccccc);
    hal_HstSendEvent(G_Mp3NonCliPLBuf_alloc);
    hal_HstSendEvent(G_Mp3NonCliPLBuf);
    hal_HstSendEvent(G_Mp3NonCliPRBuf_alloc);
    hal_HstSendEvent(G_Mp3NonCliPRBuf);

#endif

    *pDMA=hal_VocGetDmaiPointer((INT32*)G_Mp3NonCliPLBuf,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pDMA = hal_VocGetPointer(VPP_GLOBAL_NON_CLIP_HISTORYDATA_R_MP3_ADDR);
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_Mp3NonCliPRBuf,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
#endif

    pDMA = hal_VocGetPointer(VPP_GLOBAL_DIGITAL_GAIN_MP3_ADDR);

    //*pDMA=(INT32 *)(((32767)<<16)|(0x10));
    *pDMA = (INT32 *)((32767<<16)| 0X4000);

#if 0 //do not use the algorithm
    for(INT32 i=0; i<30; i++)
    {
        calibPtr->audio_voc->voc[musicItf].mp3.gains[32+i]=f_sbc_persition[i];
    }
#endif

#if 0 //MP3 VoC decoder not use NonClip, replace by drc mode
    for(INT32 i=0; i<576; i++)
    {
        G_Mp3NonCliPLBuf[i]=0;
        G_Mp3NonCliPRBuf[i]=0;
    }
#endif

    for(INT32 i=0; i<1157; i++)
    {
        G_Mp3DRCHistoryBuf[i] =0;
    }
    G_Mp3DRCHistoryBuf[4] = (INT32)0x7fff7fff;

    pDecIn = hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
    pDecIn->reset=1;
    pDecIn->EQ_Type = -1;



    pDecStatus = (vpp_AudioJpeg_DEC_OUT_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_OUT_STRUCT);
    pDecStatus->ErrorStatus=0;
    pDecStatus->mode=-1;//not 0~9;


    // move to STALL location (VoC irq generated)

    vpp_AudioJpegDecVocOpen_status=TRUE;

    return  hal_VocWakeup(HAL_VOC_START);

}


//=============================================================================
// vpp_AudioJpegDecClose function
//-----------------------------------------------------------------------------
void vpp_AudioJpegDecClose(void)
{
    if(vpp_AudioJpegDecVocOpen_status==TRUE)
        hal_VocClose();
#if 0 //MP3 VoC decoder not use NonClip, replace by drc mode
#ifdef SMALL_BSS_RAM_SIZE
    if(G_Mp3NonCliPLBuf_alloc)
    {
        mmc_MemFreeAll_BssRam(G_Mp3NonCliPLBuf_alloc);
        G_Mp3NonCliPLBuf_alloc=0;
    }
    if(G_Mp3NonCliPRBuf_alloc)
    {
        mmc_MemFreeAll_BssRam(G_Mp3NonCliPRBuf_alloc);
        G_Mp3NonCliPRBuf_alloc=0;
    }
#endif
#endif
    vpp_AudioJpegDecVocOpen_status=FALSE;
    diag_printf("[vpp_AudioJpeg_DEC]Closing VPP\n");

}
//=============================================================================
// vpp_AudioJpegDecStatus function
//-----------------------------------------------------------------------------
void vpp_AudioJpegDecStatus(vpp_AudioJpeg_DEC_OUT_T * pDecStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pDecStatus = *((vpp_AudioJpeg_DEC_OUT_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_OUT_STRUCT));
}

PUBLIC UINT32 vpp_AudioJpegDecOutAlgGainDb2Val(INT32 db)
{
    if (db < 0)
        db = 0;
    else if (db > 9)
        db = 9;
    return db/3;
}

void vpp_AudioMP3DecSetDrcModeParam(CALIB_AUDIO_MUSIC_DRC_T drc_control, INT16 alg_gain)
{

    drc_control.thres = (drc_control.thres + alg_gain + drc_control.mm_gain)* 256;
    drc_control.width = drc_control.width *256;
    drc_control.limit = (drc_control.limit + alg_gain + drc_control.mm_gain) * 256;
    drc_control.M   = drc_control.M * 256;
    drc_control.noise_gate = (drc_control.noise_gate + alg_gain + drc_control.mm_gain) * 256;

    *((CALIB_AUDIO_MUSIC_DRC_T*)hal_VocGetPointer(DRC_GLOBAL_STRUCT_DRC_PARAM)) = drc_control;
    *((INT32*)(hal_VocGetPointer(DRC_G_Y_alg_gain)))    = (INT32)(alg_gain| (512<<16)); //alg_gain, crosszero_window=512

    //diag_printf("set mp3 drc mode,alc_enable=%d,thres= %d,limit= %d,noise_gate= %d,alg_gain= %d.\n",
    //  drc_control.alc_enable,drc_control.thres,drc_control.limit,drc_control.noise_gate,alg_gain);

}

void vpp_AudioMP3DecSetEqParam(CALIB_AUDIO_IIR_EQ_T *eq)
{
    int i;
    INT16 *IIR_Init = (INT16 *)hal_VocGetPointer(m_IIRProcPara_ADDR);

    *((INT16 *)hal_VocGetPointer(IIR_ENABLE_FLAG_ADDR)) = eq->flag;

    // each band has 6 coeffs(num[3], den[3])
    for (i = 0; i < CALIB_AUDIO_IIR_BANDS; i++)
    {
        CALIB_AUDIO_IIR_EQ_BAND_T band = eq->band[i];

        IIR_Init[0] = band.num[0];
        IIR_Init[1] = band.num[1];
        IIR_Init[2] = band.num[2];
        IIR_Init[3] = band.den[0];
        IIR_Init[4] = band.den[1];
        IIR_Init[5] = band.den[2];

        IIR_Init += 6;
    }

    return;

}

void vpp_AudioMP3DecSetReloadFlag(void)
{
    reCalcMp3EQ = 1;
}

//=============================================================================
// vpp_AudioJpegDecScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_AudioJpegDecScheduleOneFrame(vpp_AudioJpeg_DEC_IN_T *pDecIn)
{
    vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
    INT32 * *pDMA;
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    INT16 alg_gain = 0x4000;

    *pDecInVoC=*pDecIn;

    pDecInVoC->inStreamBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->inStreamUBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamUBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->inStreamVBufAddr   = hal_VocGetDmaiPointer(pDecIn->inStreamVBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecInVoC->outStreamBufAddr = hal_VocGetDmaiPointer(pDecIn->outStreamBufAddr,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->reset=0;
    // diag_printf("[vpp_AudioJpeg_DEC]Codecmode:%d\n",pDecInVoC->mode);

    pDMA = hal_VocGetPointer(VPP_GLOBAL_DIGITAL_GAIN_MP3_ADDR);//MP3 and AAC use the same addr

    if (lastAudioItf != audioItf)
    {
        vpp_AudioMP3DecSetReloadFlag();
        lastAudioItf = audioItf;
    }

    alg_gain = aud_GetOutAlgGainDb2Val();
    //diag_printf("[vpp_AudioJpeg_DEC] alg_gain= %d\n",alg_gain);

    *pDMA=(INT32 *)(((32767)<<16)|alg_gain);

    /*
        switch (vpp_AudioJpegDecOutAlgGainDb2Val(pOutGains->musicOrLsAlg))
        {
        case 0:
        {
           *pDMA=(INT32 *)(((32767)<<16)|(0x10));
        }
        break;

        case 1:
        {
            *pDMA=(INT32 *)(((32767*0x10/0x16)<<16)|(0x16));
        }
        break;

        case 2:
        {
            *pDMA=(INT32 *)(((32767*0x10/0x20)<<16)|0x20);
        }
        break;

        case 3:
        {
            *pDMA=(INT32 *)(((32767*0x10/0x2d)<<16)|0x2d);
        }
        break;

        default:
        {
            *pDMA=(INT32 *)(((32767)<<16)|(0x10));
        }
        break;
        }
    */

    /*
        if(audio_cfg.spkLevel%2==0)
        {
            *pDMA=(INT32 *)(((32767*0x10/0x20)<<16)|0x20);

        }
        else
        {
            *pDMA=(INT32 *)(((32767)<<16)|(0x10));
        }

    */

    //only support mp3 drc
    if(pDecInVoC->mode==VOC_MP3_DEC_MODE)
    {
        // set drc
        CALIB_AUDIO_MUSIC_DRC_T  drc_in = calibPtr->audio_music->music[audioItf].drc;
        INT16 drc_alg_gain = 0;

        vpp_AudioMP3DecSetDrcModeParam(drc_in,drc_alg_gain);

        // set mp3eq
        CALIB_AUDIO_IIR_EQ_T *eq =  &(calibPtr->audio_music->music[audioItf].eq);
        if (reCalcMp3EQ == 1)
        {
            vpp_AudioJpeg_DEC_OUT_T *pDecOutVoC = (vpp_AudioJpeg_DEC_OUT_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_OUT_STRUCT);
            calc_coeffs_for_voc(eq, pDecOutVoC->SampleRate, audioItf);
            reCalcMp3EQ = 0;
        }
        vpp_AudioMP3DecSetEqParam(eq);
    }

    // schedule next frame on sof1 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
}

#ifdef VIDEO_PLAYER_SUPPORT
HAL_ERR_T  vpp_AudioJpegDecSetCurMode(INT16 pVoc_Video_Mode, INT16 pVoc_Audio_Mode)
{
    INT16 * pointVoC = hal_VocGetPointer(VPP_Current_Audio_Mode_addr);
    vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);

    *pointVoC=pVoc_Audio_Mode;
    pointVoC=pointVoC+1;
    *pointVoC=pVoc_Video_Mode;

    pDecInVoC->reset=0;//not reload code
    pDecInVoC->BsbcEnable=0;//bluetooth

    return HAL_ERR_NO;
}


HAL_ERR_T  vpp_AudioJpegDecSetInputMode(INT16 pVoc_Input_Mode)
{
    vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);

    pDecInVoC->mode=pVoc_Input_Mode;
    pDecInVoC->BsbcEnable=0;

    return HAL_ERR_NO;
}
/*
set voc audio/video decode mode
DecMode:    Codecmode enum
AVMode:     0= video, 1= audio.
Reset:      0=keep current mode,1=reset current mode in voc.
NOTE:       set Reset=1 for the first time.
sheen
2011.09.11
*/
HAL_ERR_T  vpp_AVDecSetMode(INT16 DecMode, INT16 AVMode, INT16 Reset)
{
    INT16 * pCurInVoC = hal_VocGetPointer(VPP_Current_Audio_Mode_addr);
    vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);

    pDecInVoC->mode=DecMode;
    pDecInVoC->reset=Reset;// 1 = set both current audio & video mode in valide and reload code
    pDecInVoC->BsbcEnable=0;//bluetooth

    if(AVMode==0)
    {
        //set next audio mode invalide in voc
        *pCurInVoC= -1;
    }
    else
    {
        //set next video mode invalide in voc
        pCurInVoC=pCurInVoC+1;
        *pCurInVoC= -1;
    }

    return HAL_ERR_NO;
}

INT16  vpp_AudioJpegDecGetMode(void)
{
    vpp_AudioJpeg_DEC_IN_T * pDecInVoC = (vpp_AudioJpeg_DEC_IN_T *)hal_VocGetPointer(vpp_AudioJpeg_DEC_IN_STRUCT);
    return pDecInVoC->mode;
}

//=============================================================================
// vpp_AudioJpegSetReuseCode function
//reuse with VPP_H263_Zoom_Code_ExternalAddr_addr
//code_id: 0=zoom, 1=rmvb, 2=264, 3=mpeg4 ...
//set after vpp_AudioJpegDecOpen
//-----------------------------------------------------------------------------
void vpp_AudioJpegSetReuseCode(INT16 code_id)
{
    INT32 * *pDMA;

    switch(code_id)
    {
#if 0 //replace by gouda.sheen
        case 0:
            //h263 zoom
            pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            pDMA=pDMA+1;
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263ZoomConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            break;
#endif
#ifdef MEDIA_VOCVID_SUPPORT
#ifdef MEDIA_RM_SUPPORT
        case 1:
            //rmvb
            pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppRMDecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            pDMA=pDMA+1;
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppRMDecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            break;
#endif //MEDIA_RM_SUPPORT
#ifdef MEDIA_H264_SUPPORT
        case 2:
            //h264
            pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH264DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            pDMA=pDMA+1;
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH264DecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            break;
#endif //MEDIA_H264_SUPPORT
        case 3:
            //mpeg4
            pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMpeg4DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            pDMA=pDMA+1;
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMpeg4DecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            break;
        case 4:
            //h263
            pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263DecCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            pDMA=pDMA+1;
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppH263DecConsty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            break;
        case 5:
            //jpeg
            pDMA = hal_VocGetPointer(VPP_H263_Zoom_Code_ExternalAddr_addr);
            *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJpegDec2Code,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            //pDMA=pDMA+1;
            //*pDMA=hal_VocGetDmaiPointer((INT32*)G_VppJPEGDec2Consty,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
            break;
#endif //MEDIA_VOCVID_SUPPORT
        default:
            break;
    }
}
#endif


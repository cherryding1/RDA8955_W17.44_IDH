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
#include "hal_speech.h"
#include "hal_dma.h"

#include "vppp_speech.h"
#include "vppp_speech_asm.h"

#include "vpp_speech.h"

// Global variable storing the voc irq handler
PRIVATE VPP_SPEECH_WAKEUP_MODE_T g_vppSpeechWakeupMode  __attribute__((section (".vpp.globalvars.c"))) = VPP_SPEECH_WAKEUP_NO;

//=============================================================================
// vpp_SpeechOpen function
//-----------------------------------------------------------------------------
PUBLIC HAL_ERR_T vpp_SpeechOpen(    HAL_VOC_IRQ_HANDLER_T       vocIrqHandler,
                                    VPP_SPEECH_WAKEUP_MODE_T    wakeupMode)
{

    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;
    VPP_SPEECH_AUDIO_CFG_T audioCfg;
    VPP_SPEECH_INIT_T *pInit = hal_VocGetPointer(VPP_SPEECH_INIT_STRUCT);

    cfg.vocCode              = g_vppSpeechMainCode;
    cfg.vocCodeSize          = g_vppSpeechMainCodeSize;
    cfg.pcVal                = VPP_SPEECH_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_SPEECH_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_SPEECH_CRITICAL_SECTION_MAX;
    cfg.needOpenDoneIrq      = FALSE;
    cfg.irqMask.voc         = (vocIrqHandler) ? 1 : 0;
    cfg.irqMask.dmaVoc      = 0;
    cfg.vocIrqHandler       = vocIrqHandler;

    cfg.eventMask.wakeupIfc0 = 0;
    cfg.eventMask.wakeupIfc1 = 0;
    cfg.eventMask.wakeupDmae = 0;
    cfg.eventMask.wakeupDmai = 1;
    cfg.eventMask.wakeupSof0 = 0;
    cfg.eventMask.wakeupSof1 = 0;

    switch (wakeupMode)
    {
        case VPP_SPEECH_WAKEUP_SW_DECENC :
        case VPP_SPEECH_WAKEUP_SW_ENC    : cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_SPEECH_WAKEUP_SW_DEC    : cfg.eventMask.wakeupSof1 = 1; break;
        case VPP_SPEECH_WAKEUP_HW_DECENC :
        case VPP_SPEECH_WAKEUP_HW_ENC    : cfg.eventMask.wakeupIfc0 = 1; break;
        case VPP_SPEECH_WAKEUP_HW_DEC    : cfg.eventMask.wakeupIfc1 = 1; break;
        default                          : break;
    }

    g_vppSpeechWakeupMode = wakeupMode;
    cfg.enableFlashAccess = false;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return status;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        UINT16 i;

        // Init code state structure
        for (i = 0; i < 4; i++)
        {
            pInit->codeStateTab[i] = -1;
        }

        pInit->codePtrTab[VPP_SPEECH_EFR_AMR_IDX]          = hal_VocGetDmaiPointer(g_vppEfrAmrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_FR_IDX]               = hal_VocGetDmaiPointer(g_vppFrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_HR_IDX]               = hal_VocGetDmaiPointer(g_vppHrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_IDX]              = hal_VocGetDmaiPointer(g_vppAmrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_EFR_IDX]              = hal_VocGetDmaiPointer(g_vppEfrCode,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_475_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode475,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_515_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode515,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_59_IDX]       = hal_VocGetDmaiPointer(g_vppAmrEncCode59,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_67_IDX]       = hal_VocGetDmaiPointer(g_vppAmrEncCode67,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_74_IDX]       = hal_VocGetDmaiPointer(g_vppAmrEncCode74,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_795_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode795,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_102_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode102,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_ENC_122_IDX]      = hal_VocGetDmaiPointer(g_vppAmrEncCode122,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_475_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode475,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_515_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode515,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_59_IDX]       = hal_VocGetDmaiPointer(g_vppAmrDecCode59,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_67_IDX]       = hal_VocGetDmaiPointer(g_vppAmrDecCode67,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_74_IDX]       = hal_VocGetDmaiPointer(g_vppAmrDecCode74,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_795_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode795,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_102_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode102,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pInit->codePtrTab[VPP_SPEECH_AMR_DEC_122_IDX]      = hal_VocGetDmaiPointer(g_vppAmrDecCode122,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    }

    // irq generation
    switch (wakeupMode)
    {
        case VPP_SPEECH_WAKEUP_SW_ENC    :
        case VPP_SPEECH_WAKEUP_HW_ENC    : pInit->irqgen = VPP_SPEECH_IRQGEN_ENC; break;
        case VPP_SPEECH_WAKEUP_SW_DECENC :
        case VPP_SPEECH_WAKEUP_HW_DECENC :
        case VPP_SPEECH_WAKEUP_SW_DEC    :
        case VPP_SPEECH_WAKEUP_HW_DEC    : pInit->irqgen = VPP_SPEECH_IRQGEN_DEC; break;
        default                          : pInit->irqgen = VPP_SPEECH_IRQGEN_NONE; break;
    }

    // reset VPP speech
    vpp_SpeechReset();

    // default audio configuration
    audioCfg.echoEsOn   = 0;
    audioCfg.echoEsVad  = 0;
    audioCfg.echoEsDtd  = 0;
    audioCfg.echoExpRel = 0;
    audioCfg.echoExpMu  = 0;
    audioCfg.echoExpMin = 0;
    audioCfg.encMute    = VPP_SPEECH_UNMUTE;
    audioCfg.decMute    = VPP_SPEECH_UNMUTE;
    audioCfg.sdf        = NULL;
    audioCfg.mdf        = NULL;

    vpp_SpeechAudioCfg(&audioCfg);

    // invalidate the encoder out buffer
    ((HAL_SPEECH_ENC_OUT_T *)hal_VocGetPointer(VPP_SPEECH_ENC_OUT_STRUCT))->encMode = HAL_INVALID;

    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;

}

//=============================================================================
// vpp_SpeechClose function
//-----------------------------------------------------------------------------
/// Close VPP Speech, clear VoC wakeup masks. This function is
/// called each time a voice call is stopped.
//=============================================================================
PUBLIC VOID vpp_SpeechClose(VOID)
{

    hal_VocClose();
    g_vppSpeechWakeupMode = VPP_SPEECH_WAKEUP_NO;

}

//=============================================================================
// vpp_SpeechReset function
//-----------------------------------------------------------------------------
/// Reset VPP (including swap buffer initialization).
//=============================================================================
PUBLIC VOID vpp_SpeechReset(VOID)
{
    ((VPP_SPEECH_INIT_T *)hal_VocGetPointer(VPP_SPEECH_INIT_STRUCT))->reset = 1;
}

//=============================================================================
// vpp_SpeechAudioCfg function
//-----------------------------------------------------------------------------
/// Configure the audio parameters of VPP Speech.
/// @param pAudioCfg : pointer to the structure containing the audio configuration.
//=============================================================================
PUBLIC VOID vpp_SpeechAudioCfg(VPP_SPEECH_AUDIO_CFG_T *pAudioCfg)
{

    VPP_SPEECH_AUDIO_CFG_T *pDest = hal_VocGetPointer(VPP_SPEECH_AUDIO_CFG_STRUCT);

    // copy structure
    *pDest = *pAudioCfg;

    // convert to DMAI pointers
    pDest->sdf = hal_VocGetDmaiPointer(pDest->sdf,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDest->mdf = hal_VocGetDmaiPointer(pDest->mdf,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

}

//=============================================================================
// vpp_SpeechScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule one decoding and/or one encoding frame. Convert to Vpp codec mode.
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_SpeechScheduleOneFrame(VOID)
{

    HAL_ERR_T status = HAL_ERR_NO;

    if ((g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_DECENC) || (g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_DEC))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_1);

        if (HAL_ERR_NO != status)
        {
            return status;
        }
    }

    if ((g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_DECENC) || (g_vppSpeechWakeupMode == VPP_SPEECH_WAKEUP_SW_ENC))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
    }

    return status;

}

//=============================================================================
// vpp_SpeechGetRxPcmBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the output (Rx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechGetRxPcmBuffer(VOID)
{

    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_DEC_OUT_STRUCT);

}

//=============================================================================
// vpp_SpeechGetTxPcmBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Tx) PCM buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_PCM_BUF_T * vpp_SpeechGetTxPcmBuffer(VOID)
{

    return (CONST HAL_SPEECH_PCM_BUF_T *) hal_VocGetPointer(VPP_SPEECH_ENC_IN_STRUCT);

}

//=============================================================================
// vpp_SpeechGetRxCodBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the input (Rx) coded buffer
//=============================================================================
PUBLIC HAL_SPEECH_DEC_IN_T * vpp_SpeechGetRxCodBuffer(VOID)
{

    return (HAL_SPEECH_DEC_IN_T *) hal_VocGetPointer(VPP_SPEECH_DEC_IN_STRUCT);

}

//=============================================================================
// vpp_SpeechGetTxCodBuffer function
//-----------------------------------------------------------------------------
/// @return pointer to the output (Tx) coded buffer
//=============================================================================
PUBLIC CONST HAL_SPEECH_ENC_OUT_T * vpp_SpeechGetTxCodBuffer(VOID)
{

    return (CONST HAL_SPEECH_ENC_OUT_T *) hal_VocGetPointer(VPP_SPEECH_ENC_OUT_STRUCT);

}



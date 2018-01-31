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

#include "vppp_debug.h"
#include "vpp_amjr.h"
#include "vppp_amjr.h"
#include "vppp_amjr_asm.h"
#include "vppp_amjr_map.h"

// Global variable storing the voc irq handler
PRIVATE VPP_AMJR_WAKEUP_MODE_T g_vppAmjrWakeupMode  __attribute__((section (".vpp.globalvars.c"))) = VPP_AMJR_WAKEUP_NO;

//=============================================================================
// vpp_AmjrOpen function
//-----------------------------------------------------------------------------

PUBLIC HAL_ERR_T vpp_AmjrOpen(HAL_VOC_IRQ_HANDLER_T  vocIrqHandler,
                              VPP_AMJR_WAKEUP_MODE_T wakeupMode)
{
    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;

    VPP_AMJR_STATUS_T* pStatus = hal_VocGetPointer(VPP_AMJR_STATUS_STRUCT);

    cfg.vocCode              = g_vppAmjrCommonCode;
    cfg.vocCodeSize          = VPP_AMJR_MAIN_SIZE;
    cfg.pcVal                = VPP_AMJR_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_AMJR_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_AMJR_CRITICAL_SECTION_MAX;
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


    switch(wakeupMode)
    {
        case VPP_AMJR_WAKEUP_SW_AUDIO_ONLY     : cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_AMJR_WAKEUP_HW_AUDIO_ONLY     : cfg.eventMask.wakeupIfc0 = 1; break;
        case VPP_AMJR_WAKEUP_SW_AUDIO_SW_VIDEO : cfg.eventMask.wakeupSof0 = 1;
            cfg.eventMask.wakeupSof1 = 1; break;
        case VPP_AMJR_WAKEUP_HW_AUDIO_SW_VIDEO : cfg.eventMask.wakeupIfc0 = 1;
            cfg.eventMask.wakeupSof1 = 1; break;
        case VPP_AMJR_WAKEUP_SW_VIDEO_ONLY     : cfg.eventMask.wakeupSof1 = 1; break;
        case VPP_AMJR_WAKEUP_NO                : return HAL_ERR_BAD_PARAMETER;
    }

    g_vppAmjrWakeupMode = wakeupMode;
    cfg.enableFlashAccess = false;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        VPP_AMJR_CODE_CFG_T *pIn  = hal_VocGetPointer(VPP_AMJR_CODE_CFG_STRUCT);

        // pointers to the jpeg tables
        pIn->jpegPtrs[0] = hal_VocGetDmaiPointer((INT32*)g_vppAmjrJpegCode,        HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->jpegPtrs[1] = hal_VocGetDmaiPointer((INT32*)g_vppAmjrJpegConstY,      HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        // pointers to the amr tables
        pIn->amrPtrs[0]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjrAmrCode,         HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->amrPtrs[1]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjrAmrConstX,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->amrPtrs[2]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjrAmrConstY,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        // pointers to the mp3 tables
        pIn->mp3Ptrs[0]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjrMp3Code,         HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->mp3Ptrs[1]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjrMp3ConstX,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->mp3Ptrs[2]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjrMp3ConstY,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }

    pStatus->audioMode      = VPP_AMJR_AUDIO_MODE_NO;
    pStatus->videoMode      = VPP_AMJR_VIDEO_MODE_NO;
    pStatus->strmStatus     = VPP_AMJR_STRM_ID_INIT;
    pStatus->errorStatus    = VPP_AMJR_ERROR_NONE;

    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;
}

//=============================================================================
// vpp_AmjrClose function
//-----------------------------------------------------------------------------
/// Close VPP Amjr, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_AmjrClose(VOID)
{
    hal_VocClose();
    g_vppAmjrWakeupMode = VPP_AMJR_WAKEUP_NO;
}


//=============================================================================
// vpp_AmjrStatus function
//-----------------------------------------------------------------------------
/// Return the VPP AMJR status structure.
/// @param pStatus : result status structure of type VPP_AMJR_STATUS_T
//=============================================================================
PUBLIC VOID vpp_AmjrStatus(VPP_AMJR_STATUS_T * pStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pStatus = *((VPP_AMJR_STATUS_T *)hal_VocGetPointer(VPP_AMJR_STATUS_STRUCT));
}


//=============================================================================
// vpp_AmjrAudioCfg function
//-----------------------------------------------------------------------------
/// @param cfg : audio configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_AmjrAudioCfg(VPP_AMJR_AUDIO_CFG_T* pCfg)
{
    VPP_AMJR_CFG_T* pVppCfg    = hal_VocGetPointer(VPP_AMJR_CFG_STRUCT);

    pVppCfg->audioMode         = pCfg->mode;
    pVppCfg->reset             = pCfg->reset;
    pVppCfg->inStreamBufAddr   = hal_VocGetDmaiPointer((INT32*)pCfg->inBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->outStreamBufAddr  = hal_VocGetDmaiPointer((INT32*)pCfg->outBufAddr, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->sampleRate        = pCfg->sampleRate;
    pVppCfg->bitRate           = pCfg->bitRate;
    pVppCfg->channelNum        = pCfg->channelNum;
    pVppCfg->inStreamBufIfcPtr = pCfg->ifcStatusPtr;
}

//=============================================================================
// vpp_AmjrVideoCfg function
//-----------------------------------------------------------------------------
/// @param cfg : video configuration (codec mode, ...)
//=============================================================================
PUBLIC VOID vpp_AmjrVideoCfg(VPP_AMJR_VIDEO_CFG_T* pCfg)
{
    VPP_AMJR_CFG_T* pVppCfg     = hal_VocGetPointer(VPP_AMJR_CFG_STRUCT);

    pVppCfg->videoMode          = pCfg->mode;
    pVppCfg->imagInBufAddr      = hal_VocGetDmaiPointer((INT32*)pCfg->inBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->imagOutBufAddr     = hal_VocGetDmaiPointer((INT32*)pCfg->outBufAddr, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->imagWidth          = pCfg->width;
    pVppCfg->imagHeight         = pCfg->height;
    pVppCfg->imagQuality        = pCfg->quality;
    pVppCfg->imagFormat         = pCfg->format;
    pVppCfg->imagPxlSwap        = pCfg->pxlSwap;
    pVppCfg->previewOutBufAddr  = hal_VocGetDmaiPointer((INT32*)pCfg->previewBufAddr, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->previewWidth       = pCfg->previewWidth;
    pVppCfg->previewHeight      = pCfg->previewHeight;
    pVppCfg->previewFormat      = pCfg->previewFormat;
    pVppCfg->previewScaleFactor = pCfg->previewScaleFactor;
}

//=============================================================================
// vpp_AmjrScheduleOneAudioFrame function
//-----------------------------------------------------------------------------
/// @param pIn : pointer to the input buffer
/// @param pOut : pointer to the output buffer
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_AmjrScheduleOneAudioFrame(UINT32* pIn, UINT32* pOut)
{
    HAL_ERR_T status = HAL_ERR_NO;

    VPP_AMJR_CFG_T * pVppCfg = hal_VocGetPointer(VPP_AMJR_CFG_STRUCT);

    // set the buffer pointers
    pVppCfg->inStreamBufAddr   = hal_VocGetDmaiPointer((INT32*)pIn,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->outStreamBufAddr  = hal_VocGetDmaiPointer((INT32*)pOut,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    // schedule one audio frame if the SW wakeup event is used
    if ((VPP_AMJR_WAKEUP_SW_AUDIO_ONLY == g_vppAmjrWakeupMode) || (VPP_AMJR_WAKEUP_SW_AUDIO_SW_VIDEO == g_vppAmjrWakeupMode))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
    }
    else if ((VPP_AMJR_WAKEUP_SW_VIDEO_ONLY == g_vppAmjrWakeupMode) || (VPP_AMJR_WAKEUP_NO == g_vppAmjrWakeupMode))
    {
        status = HAL_ERR_RESOURCE_NOT_ENABLED;
    }

    return status;
}

//=============================================================================
// vpp_AmjrScheduleOneVideoFrame function
//-----------------------------------------------------------------------------
/// @param pIn : pointer to the input buffer
/// @param pOut : pointer to the coded output buffer
/// @param pPreview : pointer to the preview output buffer
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_AmjrScheduleOneVideoFrame(UINT32* pIn, UINT32* pOut, UINT32* pPreview)
{
    HAL_ERR_T status = HAL_ERR_NO;
    VPP_TRACE(TSTDOUT, 0, "VPP video frame: in:%#x, out:%#x, preview:%#x",
              pIn, pOut, pPreview);

    VPP_AMJR_CFG_T * pVppCfg = hal_VocGetPointer(VPP_AMJR_CFG_STRUCT);

    // set the buffer pointers
    pVppCfg->imagInBufAddr     = hal_VocGetDmaiPointer((INT32*)pIn,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->imagOutBufAddr    = hal_VocGetDmaiPointer((INT32*)pOut,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->previewOutBufAddr = hal_VocGetDmaiPointer((INT32*)pPreview,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    // schedule one audio frame if the SW wakeup event is used
    if ((VPP_AMJR_WAKEUP_SW_VIDEO_ONLY == g_vppAmjrWakeupMode) || (VPP_AMJR_WAKEUP_SW_AUDIO_SW_VIDEO == g_vppAmjrWakeupMode) || (VPP_AMJR_WAKEUP_HW_AUDIO_SW_VIDEO == g_vppAmjrWakeupMode))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_1);
    }
    else
    {
        status = HAL_ERR_RESOURCE_NOT_ENABLED;
    }

    return status;
}


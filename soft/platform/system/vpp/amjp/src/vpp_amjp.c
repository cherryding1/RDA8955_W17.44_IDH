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
#include "hal_aif.h"

#include "vpp_amjp.h"
#include "vppp_amjp.h"
#include "vppp_amjp_asm.h"
#include "vppp_amjp_map.h"

// Global variable storing the voc irq handler
PRIVATE VPP_AMJP_WAKEUP_MODE_T g_vppAmjpWakeupMode= VPP_AMJP_WAKEUP_NO;

//=============================================================================
// vpp_AmjpOpen function
//-----------------------------------------------------------------------------

PUBLIC HAL_ERR_T vpp_AmjpOpen(HAL_VOC_IRQ_HANDLER_T  vocIrqHandler,
                              VPP_AMJP_WAKEUP_MODE_T wakeupMode)
{
    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;

    VPP_AMJP_CFG_T* pCfg       = hal_VocGetPointer(VPP_AMJP_CFG_STRUCT);
    VPP_AMJP_STATUS_T* pStatus = hal_VocGetPointer(VPP_AMJP_STATUS_STRUCT);

    cfg.vocCode              = g_vppAmjpCommonCode;
    cfg.vocCodeSize          = VPP_AMJP_MAIN_SIZE;
    cfg.pcVal                = VPP_AMJP_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_AMJP_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_AMJP_CRITICAL_SECTION_MAX;
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
        case VPP_AMJP_WAKEUP_SW_AUDIO_ONLY     : cfg.eventMask.wakeupSof1 = 1; break;
        case VPP_AMJP_WAKEUP_HW_AUDIO_ONLY     : cfg.eventMask.wakeupIfc1 = 1; break;
        case VPP_AMJP_WAKEUP_SW_AUDIO_SW_VIDEO : cfg.eventMask.wakeupSof1 = 1; cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_AMJP_WAKEUP_HW_AUDIO_SW_VIDEO : cfg.eventMask.wakeupIfc1 = 1; cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_AMJP_WAKEUP_SW_VIDEO_ONLY     : cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_AMJP_WAKEUP_NO                : return HAL_ERR_BAD_PARAMETER;
    }

    g_vppAmjpWakeupMode = wakeupMode;
    cfg.enableFlashAccess =false;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        VPP_AMJP_CODE_CFG_T *pIn  = hal_VocGetPointer(VPP_AMJP_CODE_CFG_STRUCT);

        // pointers to the jpeg tables
        pIn->jpegPtrs[0] = hal_VocGetDmaiPointer((INT32*)g_vppAmjpJpegCode,        HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->jpegPtrs[1] = hal_VocGetDmaiPointer((INT32*)g_vppAmjpJpegConstY,      HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        // pointers to the mp12 tables
        pIn->mp12Ptrs[0] = hal_VocGetDmaiPointer((INT32*)g_vppAmjpMp12Code,        HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->mp12Ptrs[1] = hal_VocGetDmaiPointer((INT32*)g_vppAmjpMp12ConstY,      HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        // pointers to the mp3 tables
        pIn->mp3Ptrs[0]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpMp3Code,         HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->mp3Ptrs[1]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpMp3ConstX,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->mp3Ptrs[2]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpMp3ConstY,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        // pointers to the amr tables
        pIn->amrPtrs[0]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpAmrCode,         HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->amrPtrs[1]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpAmrConstY,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        // pointers to the aac tables
        pIn->aacPtrs[0]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpAacCode,         HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->aacPtrs[1]  = hal_VocGetDmaiPointer((INT32*)g_vppAmjpAacConstY,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

        pIn->eqTable     = hal_VocGetDmaiPointer((INT32*)g_vppAmjpConstAudioEq,    HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    }

    // clear structures
    pCfg->reset = 1;
    pCfg->eqType = VPP_AMJP_EQUALIZER_OFF;
    pCfg->rotateMode = VPP_AMJP_ROTATE_NO;
    pCfg->resizeToWidth = -1;
    pCfg->vidBufMode = VPP_AMJP_BUF_MODE_SWAP;
    pCfg->outStreamIfcStatusRegAddr = NULL;

    pStatus->audioMode = VPP_AMJP_AUDIO_MODE_NO;
    pStatus->videoMode = VPP_AMJP_VIDEO_MODE_NO;
    pStatus->errorStatus = 0;
    pStatus->strmStatus = VPP_AMJP_STRM_ID_INIT;

    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;
}

//=============================================================================
// vpp_AmjpClose function
//-----------------------------------------------------------------------------
/// Close VPP Amjp, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_AmjpClose(VOID)
{
    hal_VocClose();
    g_vppAmjpWakeupMode = VPP_AMJP_WAKEUP_NO;
}


//=============================================================================
// vpp_AmjpStatus function
//-----------------------------------------------------------------------------
PUBLIC VOID vpp_AmjpStatus(VPP_AMJP_STATUS_T * pStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pStatus = *((VPP_AMJP_STATUS_T *)hal_VocGetPointer(VPP_AMJP_STATUS_STRUCT));
}


//=============================================================================
// vpp_AmjpAudioCfg function
//-----------------------------------------------------------------------------
PUBLIC VOID vpp_AmjpAudioCfg(VPP_AMJP_AUDIO_CFG_T* pCfg)
{
    VPP_AMJP_CFG_T * pVppCfg = hal_VocGetPointer(VPP_AMJP_CFG_STRUCT);

    pVppCfg->audioMode                    = pCfg->mode;
    pVppCfg->reset                        = pCfg->reset;
    pVppCfg->eqType                       = pCfg->eqType;
    pVppCfg->inStreamBufAddrStart         = hal_VocGetDmaiPointer(pCfg->inStreamBufAddrStart,  HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->inStreamBufAddrEnd           = hal_VocGetDmaiPointer(pCfg->inStreamBufAddrEnd,    HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->outStreamBufAddr             = hal_VocGetDmaiPointer(pCfg->outStreamBufAddr,      HAL_VOC_DMA_WRITE, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->outStreamIfcStatusRegAddr    = hal_VocGetDmaiPointer(hal_AifGetIfcStatusRegPtr(), HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    return;
}


//=============================================================================
// vpp_AmjpVideoCfg function
//-----------------------------------------------------------------------------
PUBLIC VOID vpp_AmjpVideoCfg(VPP_AMJP_VIDEO_CFG_T* pCfg)
{
    VPP_AMJP_CFG_T * pVppCfg = hal_VocGetPointer(VPP_AMJP_CFG_STRUCT);

    pVppCfg->videoMode            = pCfg->mode;
    pVppCfg->zoomMode             = pCfg->zoomMode;
    pVppCfg->rotateMode           = pCfg->rotateMode;
    pVppCfg->resizeToWidth        = pCfg->resizeToWidth;
    pVppCfg->vidBufMode           = pCfg->vidBufMode;
    pVppCfg->sourceWidth          = pCfg->sourceWidth;
    pVppCfg->sourceHeight         = pCfg->sourceHeight;
    pVppCfg->croppedWidth         = pCfg->croppedWidth;
    pVppCfg->croppedHeight        = pCfg->croppedHeight;
    pVppCfg->zoomedWidth          = pCfg->zoomedWidth;
    pVppCfg->zoomedHeight         = pCfg->zoomedHeight;
    pVppCfg->inVidBufAddrStart    = hal_VocGetDmaiPointer(pCfg->inVidBufAddrStart, HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->inVidBufAddrEnd      = hal_VocGetDmaiPointer(pCfg->inVidBufAddrEnd,   HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->outVidBufAddr        = hal_VocGetDmaiPointer(pCfg->outVidBufAddr,     HAL_VOC_DMA_WRITE, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->inYBufAddrStart      = hal_VocGetDmaiPointer(pCfg->inYBufAddrStart,   HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->inVBufAddrStart      = hal_VocGetDmaiPointer(pCfg->inVBufAddrStart,   HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    return;
}

//=============================================================================
// vpp_AmjpScheduleOneAudioFrame function
//-----------------------------------------------------------------------------
PUBLIC HAL_ERR_T vpp_AmjpScheduleOneAudioFrame(INT32* pOut)
{
    HAL_ERR_T status = HAL_ERR_NO;

    VPP_AMJP_CFG_T * pVppCfg = hal_VocGetPointer(VPP_AMJP_CFG_STRUCT);

    // configure the output PCM buffer pointer
    pVppCfg->outStreamBufAddr = hal_VocGetDmaiPointer(pOut, HAL_VOC_DMA_WRITE, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    // schedule one audio frame if the SW wakeup event is used
    // (no need to schedule if the IFC event is used)
    if ((VPP_AMJP_WAKEUP_HW_AUDIO_ONLY != g_vppAmjpWakeupMode) || (VPP_AMJP_WAKEUP_HW_AUDIO_SW_VIDEO != g_vppAmjpWakeupMode))
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_1);
    }

    return status;
}

//=============================================================================
// vpp_AmjpScheduleOneVideoFrame function
//-----------------------------------------------------------------------------
PUBLIC HAL_ERR_T vpp_AmjpScheduleOneVideoFrame(INT32* pInStart, INT32* pOut)
{
    HAL_ERR_T status = HAL_ERR_NO;

    VPP_AMJP_CFG_T * pVppCfg = hal_VocGetPointer(VPP_AMJP_CFG_STRUCT);

    // configure the input and output video buffer pointers
    pVppCfg->inVidBufAddrStart = hal_VocGetDmaiPointer(pInStart, HAL_VOC_DMA_READ,  HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pVppCfg->outVidBufAddr     = hal_VocGetDmaiPointer(pOut,     HAL_VOC_DMA_WRITE, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    // schedule one video frame
    status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

    return status;
}


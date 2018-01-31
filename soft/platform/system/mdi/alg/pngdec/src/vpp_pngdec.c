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

#include "vpp_pngdec.h"
#include "vppp_pngdec.h"
#include "vppp_pngdec_asm.h"
#include "vppp_pngdec_map.h"

// Global variable storing the voc irq handler
PRIVATE VPP_PNGDEC_WAKEUP_MODE_T g_vppPngDecWakeupMode  __attribute__((section (".vpp.globalvars.c"))) = VPP_PNGDEC_WAKEUP_NO;

//=============================================================================
// vpp_PngDecOpen function
//-----------------------------------------------------------------------------

PUBLIC HAL_ERR_T vpp_PngDecOpen(HAL_VOC_IRQ_HANDLER_T  vocIrqHandler,
                                VPP_PNGDEC_WAKEUP_MODE_T wakeupMode)
{
    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;

    VPP_PNGDEC_STATUS_T* pStatus = hal_VocGetPointer(VPP_PNGDEC_STATUS_STRUCT);

    cfg.vocCode              = g_vppPngDecCommonCode;
    cfg.vocCodeSize          = VPP_PNGDEC_MAIN_SIZE;
    cfg.pcVal                = VPP_PNGDEC_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_PNGDEC_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_PNGDEC_CRITICAL_SECTION_MAX;
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
        case VPP_PNGDEC_WAKEUP_SW0               : cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_PNGDEC_WAKEUP_HW0               : cfg.eventMask.wakeupIfc0 = 1; break;
        case VPP_PNGDEC_WAKEUP_NO                :
        default                                 : return HAL_ERR_BAD_PARAMETER;
    }

    g_vppPngDecWakeupMode = wakeupMode;
    cfg.enableFlashAccess = false;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        VPP_PNGDEC_CODE_CFG_T *pIn  = hal_VocGetPointer(VPP_PNGDEC_CODE_CFG_STRUCT);

        pIn->constPtr    = hal_VocGetDmaiPointer((INT32*)g_vppPngDecConst,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->hiPtr   = hal_VocGetDmaiPointer((INT32*)g_vppPngDecLenfixDistFix,      HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }

    pStatus->mode         = VPP_PNGDEC_MODE_NO;
    pStatus->errorStatus  = VPP_PNGDEC_ERROR_NO;
    pStatus->PngdecFinished= 0;

    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;
}

//=============================================================================
// vpp_PngDecClose function
//-----------------------------------------------------------------------------
/// Close VPP PngDec, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_PngDecClose(VOID)
{
    hal_VocClose();
    g_vppPngDecWakeupMode = VPP_PNGDEC_WAKEUP_NO;
}


//=============================================================================
// vpp_PngDecStatus function
//-----------------------------------------------------------------------------
/// Return the VPP PNGDEC status structure.
/// @param pStatus : result status structure of type VPP_PNGDEC_STATUS_T
//=============================================================================
PUBLIC VOID vpp_PngDecStatus(VPP_PNGDEC_STATUS_T * pStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pStatus = *((VPP_PNGDEC_STATUS_T *)hal_VocGetPointer(VPP_PNGDEC_STATUS_STRUCT));
}

//=============================================================================
// vpp_PngDecCfg function
//-----------------------------------------------------------------------------
/// @param cfg : configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_PngDecCfg(VPP_PNGDEC_CFG_T* pCfg)
{
    // copy cfg structure from source to VoC RAM.

    // loading the images in RAM_E (simulation only)
    VPP_PNGDEC_CFG_T      *pDecIn  = (VPP_PNGDEC_CFG_T*)     hal_VocGetPointer(VPP_PNGDEC_CFG_STRUCT);
    *pDecIn=*pCfg;


    pDecIn->FileContent    = hal_VocGetDmaiPointer(pCfg->FileContent, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pDecIn->FileContent=(UINT32 *)((UINT32)pDecIn->FileContent|((UINT32)pCfg->FileContent&0x3));



    pDecIn->PNGOutBuffer   = hal_VocGetDmaiPointer(pCfg->PNGOutBuffer, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pDecIn->AlphaOutBuffer = hal_VocGetDmaiPointer(pCfg->AlphaOutBuffer, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->BkgrdBuffer = hal_VocGetDmaiPointer(pCfg->BkgrdBuffer, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->Window = hal_VocGetDmaiPointer(pCfg->Window, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->ImgWidth = hal_VocGetDmaiPointer(pCfg->ImgWidth, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_SINGLE,HAL_VOC_DMA_B2S_NO);
    pDecIn->ImgHeigh = hal_VocGetDmaiPointer(pCfg->ImgHeigh, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_SINGLE,HAL_VOC_DMA_B2S_NO);


}

//=============================================================================
// vpp_PngDecSchedule function
//-----------------------------------------------------------------------------
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_PngDecSchedule(VOID)
{
    HAL_ERR_T status = HAL_ERR_NO;

    // schedule VoC execution only if the SW event is used
    if (VPP_PNGDEC_WAKEUP_SW0 == g_vppPngDecWakeupMode)
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
    }

    return status;
}

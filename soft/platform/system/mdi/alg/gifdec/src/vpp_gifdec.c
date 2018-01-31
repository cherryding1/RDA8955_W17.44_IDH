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

#include "vpp_gifdec.h"
#include "vpp_gifdec_asm.h"
#include "vppp_gifdec_asm_map.h"


PRIVATE VPP_GIFDEC_WAKEUP_MODE_T g_vppGifDecWakeupMode  __attribute__((section (".vpp.globalvars.c"))) = VPP_GIFDEC_WAKEUP_NO;


PUBLIC HAL_ERR_T vpp_GifDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler,
                                VPP_GIFDEC_WAKEUP_MODE_T wakeupMode)
{

    int Status;
    HAL_VOC_CFG_T cfg;

//    VPP_GIFDEC_CFG_T      *pDecIn  = (VPP_GIFDEC_CFG_T*)     hal_VocGetPointer(GIF_PARA_STRUCT);
//    VPP_GIFDEC_STATUS_T   *pDecOut = (VPP_GIFDEC_STATUS_T*)  hal_VocGetPointer(GIF_INFO_STRUCT);
    VPP_GIFDEC_STATUS_T* pStatus = (VPP_GIFDEC_STATUS_T*) hal_VocGetPointer(GIF_ST_STRUCT);

    cfg.vocCode              = (INT32 *)g_vppGifDecCommonCode;
    cfg.vocCodeSize          = VPP_GIFDEC_MAIN_SIZE;
    cfg.pcVal                = VPP_GIFDEC_MAIN_ENTRY;
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
        case VPP_GIFDEC_WAKEUP_SW0               : cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_GIFDEC_WAKEUP_HW0               : cfg.eventMask.wakeupIfc0 = 1; break;
        case VPP_GIFDEC_WAKEUP_NO                :
        default                                 : return HAL_ERR_BAD_PARAMETER;
    }

    g_vppGifDecWakeupMode = wakeupMode;
    cfg.enableFlashAccess =false;
    Status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == Status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

#if 0
    else if (HAL_ERR_RESOURCE_RESET == Status)
    {
        VPP_GIFDEC_CODE_CFG_T *codePtr = (VPP_GIFDEC_CODE_CFG_T*)hal_VocGetPointer(VPP_GIFDEC_CODE_CFG_STRUCT);

        codePtr->hiPtr    = hal_VocGetDmaiPointer(ptrs[0], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        codePtr->byePtr   = hal_VocGetDmaiPointer(ptrs[1], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        codePtr->constPtr = hal_VocGetDmaiPointer(ptrs[2], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }
#endif

    pStatus->error_code = 0xbeef;
    pStatus->dec_finished  = 0;

    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return (HAL_ERR_T)Status;
}

//=============================================================================
// vpp_GifDecClose function
//-----------------------------------------------------------------------------
/// Close VPP GifDec, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_GifDecClose(VOID)
{
    hal_VocClose();
    g_vppGifDecWakeupMode = VPP_GIFDEC_WAKEUP_NO;
}

//=============================================================================
// vpp_GifDecStatus function
//-----------------------------------------------------------------------------
/// Return the VPP PNGDEC status structure.
/// @param pStatus : result status structure of type VPP_PNGDEC_STATUS_T
//=============================================================================
PUBLIC VOID vpp_GifDecStatus(VPP_GIFDEC_STATUS_T * pStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pStatus = *((VPP_GIFDEC_STATUS_T *)hal_VocGetPointer(GIF_ST_STRUCT));
}

PUBLIC VOID vpp_GifDecProfile(CACHE_PROFILE_T *profile)
{
    // copy status structure from VoC RAM to the destination.
    *profile = *((CACHE_PROFILE_T *)hal_VocGetPointer(CACHE_PROFILE));
}
//=============================================================================
// vpp_GifDecCfg function
//-----------------------------------------------------------------------------
/// @param cfg : configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_GifDecCfg(VPP_GIFDEC_CFG_T* pCfg)
{
    // loading the images in RAM_E (simulation only)
    VPP_GIFDEC_CFG_T      *pDecIn  = (VPP_GIFDEC_CFG_T*)     hal_VocGetPointer(GIF_CFG_STRUCT);

    gdi_resizer_struct_voc *pResizer = (gdi_resizer_struct_voc*) pCfg->resizer;

    gdi_resizer_struct_voc *pDecInResizer  = (gdi_resizer_struct_voc*)     hal_VocGetPointer(GDI_RESIZER_STRUCT);

    // copy cfg structure from source to VoC RAM.
    *pDecIn=*pCfg;

    pDecIn->img_adrs    = (UINT32)hal_VocGetDmaiPointer((UINT32*)pCfg->img_adrs, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pDecIn->img_adrs = ((UINT32)pDecIn->img_adrs|((UINT32)pCfg->img_adrs&0x3));

    pDecIn->shadow_adrs   = (UINT32)hal_VocGetDmaiPointer((UINT32*)pCfg->shadow_adrs, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    *pDecInResizer = *pResizer;

    if(pResizer->want_sx_table)
    {
        pDecInResizer->want_sx_table = (S16*)hal_VocGetDmaiPointer(pResizer->want_sx_table, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }

    if(pResizer->want_sy_table)
    {
        pDecInResizer->want_sy_table = (S16*)hal_VocGetDmaiPointer(pResizer->want_sy_table, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }

}


//=============================================================================
// vpp_GifDecSchedule function
//-----------------------------------------------------------------------------
/// @return error of type HAL_ERR_T
//=============================================================================

PUBLIC HAL_ERR_T vpp_GifDecSchedule(VOID)
{
    HAL_ERR_T status = HAL_ERR_NO;

    if (g_vppGifDecWakeupMode == VPP_GIFDEC_WAKEUP_SW0)
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
    }

    return status;
}




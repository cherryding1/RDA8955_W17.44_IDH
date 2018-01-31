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

#include <stdio.h>
#include <string.h>

#include "vpp_pngdec.h"
#include "vppp_pngdec.h"
#include "vppp_pngdec_asm.h"
#include "vppp_pngdec_asm_common.h"
#include "vppp_pngdec_asm_map.h"
#include "vppp_pngdec_asm_sections.h"

#include "voc2_library.h"
#include "voc2_simulator.h"
#include "voc2_define.h"

extern VPP_PNGDEC_CFG_T g_vppPngDecCfg;

INT32 g_vppPngDecWakeupMode;

INT32* ptrs[7];

// loading the images in RAM_E (simulation only)
INT32 ext_ptr = 0;

PUBLIC HAL_ERR_T vpp_PngDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler,
                                VPP_PNGDEC_WAKEUP_MODE_T wakeupMode)
{

    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;

    VPP_PNGDEC_CFG_T      *pDecIn  = (VPP_PNGDEC_CFG_T*)     hal_VocGetPointer(VPP_PNGDEC_CFG_STRUCT);
    VPP_PNGDEC_STATUS_T   *pDecOut = (VPP_PNGDEC_STATUS_T*)  hal_VocGetPointer(VPP_PNGDEC_STATUS_STRUCT);

    ptrs[2] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)g_vppPngDecConst,      GLOBAL_CONST_SIZE,  ext_ptr);

    ptrs[0] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)g_vppPngDecLenfixDistFix,      GLOBAL_CONST_LENFIX_DISTFIX_SIZE,  ext_ptr);


    cfg.vocCode              = g_vppPngDecCommonCode;
    cfg.vocCodeSize          = VPP_PNGDEC_MAIN_SIZE;
    cfg.pcVal                = VPP_PNGDEC_MAIN_ENTRY;
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
    cfg.enableFlashAccess =false;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        VPP_PNGDEC_CODE_CFG_T *codePtr = (VPP_PNGDEC_CODE_CFG_T*)hal_VocGetPointer(VPP_PNGDEC_CODE_CFG_STRUCT);

        codePtr->hiPtr    = hal_VocGetDmaiPointer(ptrs[0], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        codePtr->byePtr   = hal_VocGetDmaiPointer(ptrs[1], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        codePtr->constPtr = hal_VocGetDmaiPointer(ptrs[2], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }

    pDecOut->mode        = VPP_PNGDEC_MODE_NO;
    pDecOut->errorStatus = VPP_PNGDEC_ERROR_NO;

    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;

}

extern INT32 g_vppPngDecOutbuf[] ;

// ******************************
//            Outpout data
// ******************************

extern INT32 g_vppALphaDecOutbuf[];
// ******************************
//            inpout data
// ******************************

extern INT32 g_vppBkgrdInbuf[];

extern INT32 g_vppWindowbuf[];

//=============================================================================
// vpp_PngDecClose function
//-----------------------------------------------------------------------------
/// Close VPP PngDec, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_PngDecClose(VOID)
{

    VoC_load_internal((int*)g_vppPngDecOutbuf,    1*1024*1024,  ((INT32)ptrs[1])>>2);
    VoC_load_internal((int*)g_vppALphaDecOutbuf,    1*1024*1024,  ((INT32)ptrs[2])>>2);
    VoC_load_internal((int*)g_vppBkgrdInbuf,    1*1024*1024,  ((INT32)ptrs[3])>>2);
    VoC_load_internal((int*)g_vppWindowbuf,    64*1024,  ((INT32)ptrs[4])>>2);
    VoC_load_internal((int*)g_vppPngDecCfg.ImgWidth,    1,  ((INT32)ptrs[5])>>2);
    VoC_load_internal((int*)g_vppPngDecCfg.ImgHeigh,  1,  ((INT32)ptrs[6])>>2);


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
    // loading the images in RAM_E (simulation only)
    VPP_PNGDEC_CFG_T      *pDecIn  = (VPP_PNGDEC_CFG_T*)     hal_VocGetPointer(VPP_PNGDEC_CFG_STRUCT);


    // copy cfg structure from source to VoC RAM.
    //*((VPP_PNGDEC_CFG_T*)hal_VocGetPointer(VPP_PNGDEC_CFG_STRUCT)) = *pCfg;
    *pDecIn=*pCfg;

    ptrs[0] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->FileContent,      1*1024*1024,  ext_ptr);

    ptrs[1] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->PNGOutBuffer,      1*1024*1024,  ext_ptr);

    ptrs[2] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->AlphaOutBuffer,      1*1024*1024,  ext_ptr);

    ptrs[3] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->BkgrdBuffer,      1*1024*1024,  ext_ptr);

    ptrs[4] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->Window,      64*1024,  ext_ptr);

    ptrs[5] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->ImgWidth,      1,  ext_ptr);

    ptrs[6] = (INT32*)(ext_ptr<<2);
    ext_ptr = VoC_load_extern((int*)pCfg->ImgHeigh,      1,  ext_ptr);


    pDecIn->FileContent    = hal_VocGetDmaiPointer(ptrs[0], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pDecIn->FileContent=(UINT32 *)((INT8 *)pDecIn->FileContent+3);

    pDecIn->PNGOutBuffer   = hal_VocGetDmaiPointer(ptrs[1], HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pDecIn->AlphaOutBuffer = hal_VocGetDmaiPointer(ptrs[2], HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->BkgrdBuffer = hal_VocGetDmaiPointer(ptrs[3], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->Window = hal_VocGetDmaiPointer(ptrs[4], HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDecIn->ImgWidth = hal_VocGetDmaiPointer(ptrs[5], HAL_VOC_DMA_WRITE,HAL_VOC_DMA_SINGLE,HAL_VOC_DMA_B2S_NO);
    pDecIn->ImgHeigh = hal_VocGetDmaiPointer(ptrs[6], HAL_VOC_DMA_WRITE,HAL_VOC_DMA_SINGLE,HAL_VOC_DMA_B2S_NO);

}


//=============================================================================
// vpp_PngDecSchedule function
//-----------------------------------------------------------------------------
/// @return error of type HAL_ERR_T
//=============================================================================

PUBLIC HAL_ERR_T vpp_PngDecSchedule(VOID)
{
    HAL_ERR_T status = HAL_ERR_NO;

    if (g_vppPngDecWakeupMode == VPP_PNGDEC_WAKEUP_SW0)
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
    }

    return status;
}


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

#include "vpp_aec.h"
#include "vppp_aec.h"
#include "vppp_aec_asm.h"
#include "vppp_aec_map.h"

// Global variable storing the voc irq handler
PRIVATE VPP_HELLO_WAKEUP_MODE_T g_vppHelloWakeupMode  __attribute__((section (".vpp.globalvars.c"))) = VPP_HELLO_WAKEUP_NO;

//=============================================================================
// vpp_HelloOpen function
//-----------------------------------------------------------------------------

PUBLIC HAL_ERR_T vpp_HelloOpen(HAL_VOC_IRQ_HANDLER_T  vocIrqHandler,
                               VPP_HELLO_WAKEUP_MODE_T wakeupMode)
{
    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;

    VPP_HELLO_STATUS_T* pStatus = hal_VocGetPointer(VPP_HELLO_STATUS_STRUCT);

    cfg.vocCode              = g_vppHelloCommonCode;
    cfg.vocCodeSize          = VPP_HELLO_MAIN_SIZE;
    cfg.pcVal                = VPP_HELLO_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_HELLO_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_HELLO_CRITICAL_SECTION_MAX;
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
        case VPP_HELLO_WAKEUP_SW0               : cfg.eventMask.wakeupSof0 = 1; break;
        case VPP_HELLO_WAKEUP_HW0               : cfg.eventMask.wakeupIfc0 = 1; break;
        case VPP_HELLO_WAKEUP_NO                :
        default                                 : return HAL_ERR_BAD_PARAMETER;
    }

    g_vppHelloWakeupMode = wakeupMode;
    cfg.enableFlashAccess =false;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        VPP_HELLO_CODE_CFG_T *pIn  = hal_VocGetPointer(VPP_HELLO_CODE_CFG_STRUCT);

        pIn->hiPtr    = hal_VocGetDmaiPointer((INT32*)g_vppHelloHiCode,       HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->byePtr   = hal_VocGetDmaiPointer((INT32*)g_vppHelloByeCode,      HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pIn->constPtr = hal_VocGetDmaiPointer((INT32*)g_vppHelloConst,        HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    }

    pStatus->mode         = VPP_HELLO_MODE_NO;
    pStatus->errorStatus  = VPP_HELLO_ERROR_NO;

    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;
}

//=============================================================================
// vpp_HelloClose function
//-----------------------------------------------------------------------------
/// Close VPP Hello, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_HelloClose(VOID)
{
    hal_VocClose();
    g_vppHelloWakeupMode = VPP_HELLO_WAKEUP_NO;
}


//=============================================================================
// vpp_HelloStatus function
//-----------------------------------------------------------------------------
/// Return the VPP HELLO status structure.
/// @param pStatus : result status structure of type VPP_HELLO_STATUS_T
//=============================================================================
PUBLIC VOID vpp_HelloStatus(VPP_HELLO_STATUS_T * pStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pStatus = *((VPP_HELLO_STATUS_T *)hal_VocGetPointer(VPP_HELLO_STATUS_STRUCT));
}


//=============================================================================
// vpp_HelloCfg function
//-----------------------------------------------------------------------------
/// @param cfg : configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_HelloCfg(VPP_HELLO_CFG_T* pCfg)
{
    // copy cfg structure from source to VoC RAM.
    *((VPP_HELLO_CFG_T*)hal_VocGetPointer(VPP_HELLO_CFG_STRUCT)) = *pCfg;
}

//=============================================================================
// vpp_HelloSchedule function
//-----------------------------------------------------------------------------
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_HelloSchedule(VOID)
{
    HAL_ERR_T status = HAL_ERR_NO;

    // schedule VoC execution only if the SW event is used
    if (VPP_HELLO_WAKEUP_SW0 == g_vppHelloWakeupMode)
    {
        status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
    }

    return status;
}


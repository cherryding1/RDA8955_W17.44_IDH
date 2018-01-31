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

#include "vpp_lzmadec.h"
#include "vppp_lzmadec.h"
#include "vppp_lzmadec_asm.h"
#include "vppp_lzmadec_map.h"

volatile VPP_LZMADEC_STATUS_T g_VppLzmaStatus;

//=============================================================================
// vpp_LzmaDecOpen function
//-----------------------------------------------------------------------------

PUBLIC HAL_ERR_T vpp_LzmaDecOpen(UINT32 *InputBuf,INT32 InputLen,UINT32 *OutputBuf,HAL_VOC_IRQ_HANDLER_T  vocIrqHandler)
{
    HAL_ERR_T status;
    HAL_VOC_CFG_T cfg;

    VPP_LZMADEC_CFG_T      *pDecIn  = (VPP_LZMADEC_CFG_T*)     hal_VocGetPointer(VPP_LZMADEC_CFG_STRUCT);
    VPP_LZMADEC_STATUS_T   *pDecOut = (VPP_LZMADEC_STATUS_T*)  hal_VocGetPointer(VPP_LZMADEC_STATUS_STRUCT);


    cfg.vocCode              = g_vppLzmaDecCommonCode;
    cfg.vocCodeSize          = VPP_LZMADEC_MAIN_SIZE;
    cfg.pcVal                = VPP_LZMADEC_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_LZMADEC_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_LZMADEC_CRITICAL_SECTION_MAX;
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
    cfg.enableFlashAccess = true;
    status = hal_VocOpen(&cfg);

    if (HAL_ERR_RESOURCE_BUSY == status)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else if (HAL_ERR_RESOURCE_RESET == status)
    {
        pDecIn->InPtr = hal_VocGetDmaiPointer(InputBuf, HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pDecIn->OutPtr = hal_VocGetDmaiPointer(OutputBuf, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
        pDecIn->OutPara = hal_VocGetDmaiPointer((INT32 *)&g_VppLzmaStatus, HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);


    }
    pDecOut->mode         = 0;
    pDecOut->ErrorStatus  = VPP_LZMADEC_ERROR_NO;
    pDecOut->OutSize= 0;

    g_VppLzmaStatus.mode        = 0;
    g_VppLzmaStatus.ErrorStatus = VPP_LZMADEC_ERROR_NO;
    g_VppLzmaStatus.OutSize= 0;
    // move to STALL location (VoC irq generated)
    if (HAL_ERR_RESOURCE_BUSY == hal_VocWakeup(HAL_VOC_START))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    return status;
}

//=============================================================================
// vpp_LzmaDecClose function
//-----------------------------------------------------------------------------
/// Close VPP LzmaDec, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_LzmaDecClose(VOID)
{
    hal_VocClose();
}


//=============================================================================
// vpp_LzmaDecStatus function
//-----------------------------------------------------------------------------
/// Return the VPP LZMADEC status structure.
/// @param pStatus : result status structure of type VPP_LZMADEC_STATUS_T
//=============================================================================
PUBLIC VOID vpp_LzmaDecStatus(VPP_LZMADEC_STATUS_T * pStatus)
{
    // copy status structure from VoC RAM to the destination.
//    *pStatus = *((VPP_LZMADEC_STATUS_T *)hal_VocGetPointer(VPP_LZMADEC_STATUS_STRUCT));


    *pStatus =*((VPP_LZMADEC_STATUS_T *)((UINT32)( &g_VppLzmaStatus)|0x20000000));

//  pStatus->mode=g_VppLzmaStatus.mode;
//  pStatus->ErrorStatus=g_VppLzmaStatus.ErrorStatus;
//  pStatus->OutSize=g_VppLzmaStatus.OutSize;
}


//=============================================================================
// vpp_LzmaDecCfg function
//-----------------------------------------------------------------------------
/// @param cfg : configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_LzmaDecCfg(VPP_LZMADEC_CFG_T* pCfg)
{
    // copy cfg structure from source to VoC RAM.
    *((VPP_LZMADEC_CFG_T*)hal_VocGetPointer(VPP_LZMADEC_CFG_STRUCT)) = *pCfg;
}

//=============================================================================
// vpp_LzmaDecSchedule function
//-----------------------------------------------------------------------------
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_LzmaDecSchedule(VOID)
{
    HAL_ERR_T status = HAL_ERR_NO;

    // schedule VoC execution only if the SW event is used
    status = hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

    return status;
}


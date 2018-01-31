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

#include "vppp_analogtv_asm.h"
#include "vppp_analogtv_map.h"

#include "vpp_analogtv.h"
#include "mcip_debug.h"



//=============================================================================
// vpp_AnalogTVOpen function
//-----------------------------------------------------------------------------
/// Open VPP Analog TV.
//=============================================================================
PUBLIC HAL_ERR_T vpp_AnalogTVOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler)
{
    HAL_VOC_CFG_T cfg;
    //uint32 * *pDMA;
    HAL_VOC_DMA_CFG_T dmaCfg;
    //CutZoomInStruct  *pstruct;

    diag_printf("[vpp_AnalogTV]Opening VPP Analog TV\n");


    cfg.vocCode                = g_vppAnalogTVMainCode;
    cfg.vocCodeSize          = g_vppAnalogTVMainCodeSize;
    cfg.pcVal                     = VPP_ANALOGTV_MAIN_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_ANALOGTV_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_ANALOGTV_CRITICAL_SECTION_MAX;
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
    // load the VPP AUDIO_ENC  code and configure the VoC resource
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:
            diag_printf("[vpp_AnalogTV]##WARNING##Error opening VoC resource\n");
            return HAL_ERR_RESOURCE_BUSY;

        // first open, load the constant tables
        case HAL_ERR_RESOURCE_RESET:
            diag_printf("[vpp_AnalogTV]First open.\n");

            ////////DMA the const data in ramx
            dmaCfg.extAddr              = (INT32*)g_vppAnalogTVConstX;
            dmaCfg.vocLocalAddr      = (INT32*)hal_VocGetPointer(GLOBAL_CONST_X_STRUCT);
            dmaCfg.size                   = g_vppAnalogTVConstXSize;
            dmaCfg.wr1Rd0             = HAL_VOC_DMA_READ;
            dmaCfg.needIrq             = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[vpp_AnalogTV]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);
            diag_printf("[vpp_AnalogTV]DMA has finished.\n");

            ////////DMA the const data in ramy
            dmaCfg.extAddr              = (INT32*)g_vppAnalogTVConstY;
            dmaCfg.vocLocalAddr      = (INT32*)hal_VocGetPointer(GLOBAL_CONST_Y_STRUCT);
            dmaCfg.size                   = g_vppAnalogTVConstYSize;
            dmaCfg.wr1Rd0             = HAL_VOC_DMA_READ;
            dmaCfg.needIrq             = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[vpp_AnalogTV]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);
            diag_printf("[vpp_AnalogTV]DMA has finished.\n");

            break;


        default:
            diag_printf("[vpp_AnalogTV]No first open.\n");
            break;
    }


    return  hal_VocWakeup(HAL_VOC_START);

}


//=============================================================================
// vpp_AnalogTVClose function
//-----------------------------------------------------------------------------
/// Close VPP Analog TV, clear VoC wakeup masks. This function is
/// called each time a voice call is stopped.
//=============================================================================
PUBLIC VOID vpp_AnalogTVClose(VOID)
{
    hal_VocClose();
    diag_printf("[vpp_AnalogTV]Closing VPP\n");

}


//=============================================================================
// vpp_AnalogTVScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule one picture frame.
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_AnalogTVScheduleOneFrame(VPP_ANALOGTV_INPUT_CFG_T *pEncIn)
{
    //diag_printf("[vpp_PreviewScheduleOneFrame]\n");
    VPP_ANALOGTV_INPUT_CFG_T * pEncInVoC = (VPP_ANALOGTV_INPUT_CFG_T *)hal_VocGetPointer(GLOBAL_INPUT_STRUCT);

    *pEncInVoC = *pEncIn;

    pEncInVoC->inStreamBufAddr   = hal_VocGetDmaiPointer(pEncIn->inStreamBufAddr,   HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pEncInVoC->outStreamBufAddr = hal_VocGetDmaiPointer(pEncIn->outStreamBufAddr,   HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pEncInVoC->blendBufAddr        = hal_VocGetDmaiPointer(pEncIn->blendBufAddr,        HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    // diag_printf("[vpp_AudioJpeg_ENC]Coencmode:%d\n",pEncInVoC->mode);

    // schedule next frame on sof0 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

}







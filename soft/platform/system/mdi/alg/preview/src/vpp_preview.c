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

#include "vpp_preview_asm.h"
#include "vpp_preview.h"
#include "mcip_debug.h"

//=============================================================================
// vpp_AudioJpegEncOpen function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_PreviewOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler)
{
    HAL_VOC_CFG_T cfg;
    //uint32 * *pDMA;
    //HAL_VOC_DMA_CFG_T dmaCfg;
    //CutZoomInStruct  *pstruct;

    diag_printf("[vpp_Preview]Opening VPP Preview\n");


    cfg.vocCode                = G_VppPreviewCode;
    cfg.vocCodeSize          = vpp_Preview_CODE_SIZE;
    cfg.pcVal                     = vpp__Preview_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = vpp__Preview_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = vpp__Preview_CRITICAL_SECTION_MAX;
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
    cfg.enableFlashAccess =false;
    // load the VPP AUDIO_ENC  code and configure the VoC resource
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:
            diag_printf("[vpp_Preview##WARNING##Error opening VoC resource\n");
            return HAL_ERR_RESOURCE_BUSY;

        // first open, load the constant tables
        case HAL_ERR_RESOURCE_RESET:
            diag_printf("[vpp_Preview]First open.\n");
#if 0
            dmaCfg.extAddr              = G_VppPreviewData;
            dmaCfg.vocLocalAddr      = (INT32*)hal_VocGetPointer(CONST_RGB_SAT_TAB_ADDR);
            dmaCfg.size                   = vpp__Preview_CONST_DATA_SIZE;
            dmaCfg.wr1Rd0             = HAL_VOC_DMA_READ;
            dmaCfg.needIrq             = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VPP_AAC]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);
            diag_printf("[vpp_Preview]DMA has finished.\n");
#endif
            break;


        default:
            diag_printf("[vpp_Preview]No first open.\n");
            break;
    }


    // pDMA = hal_VocGetPointer(VPP_Preview_Code_ExternalAddr_addr);
    //*pDMA=hal_VocGetDmaiPointer(G_VppPreviewData,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    //pstruct = hal_VocGetPointer(vpp__Preview_IN_STRUCT);
    // pstruct->in_mode = 0;
    // pstruct->out_mode = -1;

    // move to STALL location (VoC irq generated)

    return  hal_VocWakeup(HAL_VOC_START);

}


HAL_ERR_T vpp_PreviewYUVOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler)
{
    HAL_VOC_CFG_T cfg;
    //uint32 * *pDMA;
    HAL_VOC_DMA_CFG_T dmaCfg;
    //CutZoomInStruct  *pstruct;

    diag_printf("[vpp_Preview]Opening VPP Preview\n");


    cfg.vocCode                = G_VppCutzzomCode;
    cfg.vocCodeSize          = vpp_Preview_CODE_SIZE;
    cfg.pcVal                     = vpp__Preview_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = vpp__Preview_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = vpp__Preview_CRITICAL_SECTION_MAX;
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
    cfg.enableFlashAccess =false;

    // load the VPP AUDIO_ENC  code and configure the VoC resource
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:
            diag_printf("[vpp_Preview##WARNING##Error opening VoC resource\n");
            return HAL_ERR_RESOURCE_BUSY;

        // first open, load the constant tables
        case HAL_ERR_RESOURCE_RESET:
            diag_printf("[vpp_Preview]First open.\n");
#if 1
            ////////DMA the const data in ramx
            dmaCfg.extAddr              = (INT32*)G_CUTZOOMDATA_TABX_ADDR;
            dmaCfg.vocLocalAddr      = (INT32*)hal_VocGetPointer(CONST_TabV2R_ADDR);
            dmaCfg.size                   = vpp__Preview_CONST_DATA_SIZE;
            dmaCfg.wr1Rd0             = HAL_VOC_DMA_READ;
            dmaCfg.needIrq             = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VPP_AAC]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);
            diag_printf("[vpp_Preview]DMA has finished.\n");

            ////////DMA the const data in ramy
            dmaCfg.extAddr              = (INT32*)G_CUTZOOMDATA_TABY_ADDR;
            dmaCfg.vocLocalAddr      = (INT32*)hal_VocGetPointer(CONST_TabU2G_ADDR);
            dmaCfg.size                   = vpp__Preview_CONST_DATA_SIZE;
            dmaCfg.wr1Rd0             = HAL_VOC_DMA_READ;
            dmaCfg.needIrq             = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VPP_AAC]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);
            diag_printf("[vpp_Preview]DMA has finished.\n");
#endif
            break;


        default:
            diag_printf("[vpp_Preview]No first open.\n");
            break;
    }


    // pDMA = hal_VocGetPointer(VPP_Preview_Code_ExternalAddr_addr);
    //*pDMA=hal_VocGetDmaiPointer(G_VppPreviewData,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    //pstruct = hal_VocGetPointer(vpp__Preview_IN_STRUCT);
    // pstruct->in_mode = 0;
    // pstruct->out_mode = -1;

    // move to STALL location (VoC irq generated)

    return  hal_VocWakeup(HAL_VOC_START);

}
//=============================================================================
// vpp_AudioJpegEncClose function
//-----------------------------------------------------------------------------
void vpp_PreviewClose(void)
{
    hal_VocClose();
    diag_printf("[vpp_PreviewClose]Closing VPP\n");

}


//=============================================================================
// vpp_AudioJpegEncScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_PreviewScheduleOneFrame(CutZoomInStruct *pEncIn)
{
    //diag_printf("[vpp_PreviewScheduleOneFrame]\n");
    CutZoomInStruct * pEncInVoC = (CutZoomInStruct *)hal_VocGetPointer(vpp__Preview_IN_STRUCT);


    *pEncInVoC = *pEncIn;

    pEncInVoC->inStreamBufAddr   = hal_VocGetDmaiPointer(pEncIn->inStreamBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pEncInVoC->outStreamBufAddr  = hal_VocGetDmaiPointer(pEncIn->outStreamBufAddr,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pEncInVoC->blendBufAddr= hal_VocGetDmaiPointer(pEncIn->blendBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    // diag_printf("[vpp_AudioJpeg_ENC]Coencmode:%d\n",pEncInVoC->mode);

    // schedule next frame on sof0 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

}






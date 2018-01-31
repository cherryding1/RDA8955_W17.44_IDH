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

#include "mcip_debug.h"
#include "vpp_meitu_filter.h"

#define vpp_MEITU_IN_STRUCT           (0  + 0x4000)
#define vpp_MEITU_OUT_STRUCT        (12  + 0x4000)
#define vpp_MEITU_OUT_STATUS        (16  + 0x4000)

#define VPP_MEITU_Code_ExternalAddr_addr             (18 + 0x4000)


//meitu
const INT32 G_VppMeituCode[]  MICRON_ALIGN(32)=
{
#include "meitu.tab"
};

const INT32 G_VppMeituConstX[]  MICRON_ALIGN(32)=
{
#include "meitu_constx.tab"
};

const INT32 G_VppMEITU_COMMON[]  MICRON_ALIGN(32)=
{
#include "imageview_meitu_common.tab"
};

#define vpp_MeituCOMMON_CODE_ENTRY 0
#define vpp_Meitu_CRITICAL_SECTION_MIN 0
#define vpp_Meitu_CRITICAL_SECTION_MAX 0

const INT32 vpp_MeituCOMMON_CODE_SIZE = sizeof(G_VppMEITU_COMMON);

//=============================================================================
// vpp_MeituOpen function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_MeituOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler)
{
    HAL_VOC_CFG_T cfg;
    vpp_MEITU_IN_T * pMeituIn;
    vpp_MEITU_OUT_T *pMeituStatus;
    INT32 * *pDMA;
    // dbg_TraceOutputText(0,"[vpp_Meitu]Opening VPP AUDIO_DEC\n");

    //hal_HstSendEvent(SYS_EVENT,0x88100000);

    cfg.vocCode              = G_VppMEITU_COMMON;
    cfg.vocCodeSize        = vpp_MeituCOMMON_CODE_SIZE;
    cfg.pcVal                   = vpp_MeituCOMMON_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = vpp_Meitu_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = vpp_Meitu_CRITICAL_SECTION_MAX;
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

    // load the VPP AUDIO_DEC  code and configure the VoC resource
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:
            dbg_TraceOutputText(0,"[vpp_Meitu]##WARNING##Error opening VoC resource\n");
            return HAL_ERR_RESOURCE_BUSY;

        // first open, load the constant tables
        case HAL_ERR_RESOURCE_RESET:
            dbg_TraceOutputText(0,"[vpp_Meitu]First open.\n");
            break;

        default:
            dbg_TraceOutputText(0,"[vpp_Meitu]No first open.\n");
            break;
    }

    dbg_TraceOutputText(0,"[vpp_Meitu]Initializing the DMA addr.\n");

    // meitu
    pDMA = hal_VocGetPointer(VPP_MEITU_Code_ExternalAddr_addr);

    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMeituCode,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pDMA=pDMA+1;
    *pDMA=hal_VocGetDmaiPointer((INT32*)G_VppMeituConstX,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pMeituIn = hal_VocGetPointer(vpp_MEITU_IN_STRUCT);
    pMeituIn->reset=1;
    pMeituStatus = (vpp_MEITU_OUT_T *)hal_VocGetPointer(vpp_MEITU_OUT_STRUCT);
    pMeituStatus->consumedLen=0;
    pMeituStatus->output_len=0;
    pMeituStatus->ErrorStatus=0;
    pMeituStatus->framecount=0;

    // move to STALL location (VoC irq generated)
    return  hal_VocWakeup(HAL_VOC_START);
}


//=============================================================================
// vpp_MeituClose function
//-----------------------------------------------------------------------------
void vpp_MeituClose(void)
{
    hal_VocClose();
    diag_printf("[vpp_Meitu]Closing VPP\n");

}
//=============================================================================
// vpp_MeituStatus function
//-----------------------------------------------------------------------------
void vpp_MeituStatus(vpp_MEITU_OUT_T * pMeituStatus)
{
    // copy status structure from VoC RAM to the destination.
    *pMeituStatus = *((vpp_MEITU_OUT_T *)hal_VocGetPointer(vpp_MEITU_OUT_STRUCT));
}

//=============================================================================
// vpp_MeituScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_MeituScheduleOneFrame(vpp_MEITU_IN_T *pMeituIn)
{
    vpp_MEITU_OUT_T *pMeituStatus = (vpp_MEITU_OUT_T *)hal_VocGetPointer(vpp_MEITU_OUT_STRUCT);
    vpp_MEITU_IN_T * pMeituInVoC = (vpp_MEITU_IN_T *)hal_VocGetPointer(vpp_MEITU_IN_STRUCT);
    *pMeituInVoC=*pMeituIn;

    pMeituInVoC->inStreamBufAddr   = hal_VocGetDmaiPointer(pMeituIn->inStreamBufAddr,  HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pMeituInVoC->outStreamBufAddr = hal_VocGetDmaiPointer(pMeituIn->outStreamBufAddr,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    pMeituInVoC->brightness = pMeituIn->brightness;
    pMeituInVoC->contrast = pMeituIn->contrast;
    pMeituIn->reset = 0;

    /*
        dbg_TraceOutputText(0,"vpp_MEITUScheduleOneFrame 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x"
            ,pMeituIn->inStreamBufAddr
            ,*pMeituIn->inStreamBufAddr
            ,pMeituIn->outStreamBufAddr
            ,pMeituIn->image_width
            ,pMeituIn->image_height
            ,pMeituInVoC->mode
            ,pMeituInVoC->reset
            ,pMeituStatus->framecount
            //,init
            );
    */

    // schedule next frame on sof1 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);
}


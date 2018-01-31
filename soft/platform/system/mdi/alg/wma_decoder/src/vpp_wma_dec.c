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
//#include "lp_ct810reg.h"
//#include "global_macros.h"
#include "hal_voc.h"
//#include "voc_cfg.h"
//#include "hal_private.h"
#include "hal_error.h"

#include "vpp_wma_dec.h"
#include "wmaudio.h"
#include "wmaudio_type.h"
#include "map_addr.h"

#include "mcip_debug.h"

#include "calib_m.h"

#include "aud_m.h"


const UINT32 TabinX[]=
{
#include "tabinx.tab"
};
const UINT32 TabinY[]=
{
#include "tabiny.tab"
};

const UINT32 TabinXEND[]=
{
#include "TabinXEND.tab"
};



#include "huffmantbl.tab"
#include "g_sintableforreconstruction.tab"
#include "costab.tab"

/*
const UINT32 wma_codeM[] = {
    #include "wma_common.tab"
    #include "wma_highmid.tab"
    #include "wma_midlow.tab"
    #include "wma_mid.tab"
};
const UINT32 wma_codeH[] = {
    #include "wma_common.tab"
    #include "wma_highmid.tab"
    #include "wma_high.tab"
};

const UINT32 wma_codeL[] = {
    #include "wma_commonforl.tab"
};
*/

const int wma_code_high[] =
{
#include "audiojpeg_dec_common.tab"
#include "wmadecoderio_voc_high.tab"
};

const int wma_code_mid[] =
{
#include "audiojpeg_dec_common.tab"
#include "wmadecoderio_voc_mid.tab"
};

const int wma_code_low[] =
{
#include "audiojpeg_dec_common.tab"
#include "wmadecoderio_voc_low.tab"
};


/*
const UINT32 wma_codeL_LPC[] = {
    #include "rlcodelpc.tab"
};
const UINT32 wma_codeL_IDCT[] = {
    #include "rlcodeinvedct4.tab"
};
*/
extern AUD_LEVEL_T audio_cfg;
extern AUD_ITF_T audioItf;



//unsigned short WMA_Decoder[684];
HAL_ERR_T vpp_WmaGetInfo( int32 *pStreamIn ,VPP_WMA_DEC_CONTENT_T * pWmaProperty )
{
    int i;
    tWMAFileStatus          iResult;                // api result
    tWMAFileStateInternal WMAFileState;
    uint8 *pchar;



    pchar = (int8 *)(&WMAFileState);
    for(i=0; i<sizeof(tWMAFileStateInternal); i++)
        *pchar++=0;

    WMAFileState.hdr_parse.pInput = pStreamIn;
    // initialize decoder
    iResult = WMAFileGetInfo((tHWMAFileState)(&WMAFileState));
    if(cWMA_NoErr != iResult)   return HAL_ERR_RESOURCE_NOT_ENABLED;

    pWmaProperty->nVersion = WMAFileState.hdr_parse.nVersion;
    pWmaProperty->nChannels = WMAFileState.hdr_parse.nChannels;
    pWmaProperty->nSamplesPerSec= WMAFileState.hdr_parse.nSamplesPerSec;
    pWmaProperty->nAvgBitsPerSec= WMAFileState.hdr_parse.nAvgBytesPerSec*8;
    pWmaProperty->msDuration= WMAFileState.hdr_parse.msDuration;

    diag_printf("msDuration = %d\n",pWmaProperty->msDuration);


    return HAL_ERR_NO;
}



void vpp_Wma_voc_open(void)
{
    HAL_VOC_CFG_T cfg;

    cfg.vocCode              = 0;
    cfg.vocCodeSize          = 0;
    cfg.pcVal                = VPP_WMA_DEC_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_WMA_DEC_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_WMA_DEC_CRITICAL_SECTION_MAX;
    cfg.needOpenDoneIrq      = FALSE;
    cfg.irqMask.voc          =  0;
    cfg.irqMask.dmaVoc       = 0;
    cfg.vocIrqHandler        = NULL;

    cfg.eventMask.wakeupIfc0 = 0;
    cfg.eventMask.wakeupIfc1 = 0;
    cfg.eventMask.wakeupDmae = 0;
    cfg.eventMask.wakeupDmai = 0;
    cfg.eventMask.wakeupSof0 = 0;
    cfg.eventMask.wakeupSof1 = 0;
    cfg.enableFlashAccess =false;
    hal_VocOpen(&cfg);

}

//=============================================================================
// vpp_WmaDecOpen function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_WmaDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler, int32 * pStreamIn,int32 *output, int16 * PrevOutput)
{
    HAL_VOC_CFG_T cfg;
    HAL_VOC_DMA_CFG_T dmaCfg;
    //int * pIntA,*pIntB;
    tWMAFileStatus          iResult;                // api result
    int WeightingMode,fNoiseSub;
    tWMAFileStateInternal WMAFileState;
    uint8 *pchar;

    diag_printf("[VPP_WMA]##WARNING##Opening VPP WMA Decoder\n");


// pls try to remove following  code! i havnt found the why error occurs when switch to 16K wma songs without the ram clr.
/////////////////////////////

////////////////////////////////////

//  pchar = (int8 *)(&WMAFileState);
//  for(i=0;i<sizeof(tWMAFileStateInternal);i++)
//      *pchar++=0;

    // initialize decoder
//  iResult = WMAFileDecodeInit((tHWMAFileState)(&WMAFileState));
//      diag_printf("init iResult =%d  \n",iResult); // added by liyongjian for debug
//  if(cWMA_NoErr != iResult)   return HAL_ERR_RESOURCE_NOT_ENABLED;

//  pWmaProperty->nVersion = WMAFileState.hdr_parse.nVersion;
//  pWmaProperty->nChannels = WMAFileState.hdr_parse.nChannels;
//  pWmaProperty->nSamplesPerSec= WMAFileState.hdr_parse.nSamplesPerSec;
//  pWmaProperty->nAvgBitsPerSec= WMAFileState.hdr_parse.nAvgBytesPerSec*8;
//  pWmaProperty->msDuration= WMAFileState.hdr_parse.msDuration;

// if (WMA_OPEN_VOC_NO == ifOpenVoc)       return HAL_ERR_NO;

    WeightingMode = *(short *)hal_VocGetPointer(GLOBAL_m_iWeightingMode_ADDR);// LPC =0 BARD =1
    fNoiseSub = *(short *)hal_VocGetPointer(GLOBAL_m_fNoiseSub_ADDR);// LPC =0 BARD =1
    diag_printf("[VPP_WMA]##WARNING##WeightingMode =%d fNoiseSub =%d \n",WeightingMode,fNoiseSub); // added by liyongjian for debug
    hal_HstSendEvent(0x88899001);
    hal_HstSendEvent(WeightingMode);
    hal_HstSendEvent(fNoiseSub);
    hal_HstSendEvent((*(INT32*)hal_VocGetPointer(GLOBAL_currPacketOffset_ADDR)));

    hal_HstSendEvent(0x88899002);

    if(!WeightingMode||fNoiseSub)
    {
        if(WeightingMode)
        {
            if(fNoiseSub)
            {
                cfg.vocCode              = wma_code_mid;
                cfg.vocCodeSize          = VPP_WMA_DEC_CODE_M_SIZE;
            }
            else
                return HAL_ERR_RESOURCE_NOT_ENABLED;
        }
        else
        {
            cfg.vocCode              = wma_code_low;
            cfg.vocCodeSize          = VPP_WMA_DEC_CODE_L_SIZE;
        }
    }
    else
    {
        cfg.vocCode              = wma_code_high;
        cfg.vocCodeSize          = VPP_WMA_DEC_CODE_H_SIZE;
    }

    cfg.pcVal                = VPP_WMA_DEC_CODE_ENTRY;
    cfg.pcValCriticalSecMin  = VPP_WMA_DEC_CRITICAL_SECTION_MIN;
    cfg.pcValCriticalSecMax  = VPP_WMA_DEC_CRITICAL_SECTION_MAX;
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

    // load the VPP Wma Decoder code and configure the VoC resource
    switch (hal_VocOpen(&cfg))
    {
        // error opening the resource
        case HAL_ERR_RESOURCE_BUSY:

            diag_printf("[VPP_WMA]##ERROR##Error opening VoC resource\n");
            return HAL_ERR_RESOURCE_BUSY;

        // first open, load the constant tables
        case HAL_ERR_RESOURCE_RESET:
            hal_HstSendEvent(0x88899003);

            // load X constant tables
            dmaCfg.extAddr           = (UINT32*)TabinX;
            dmaCfg.vocLocalAddr      = (UINT32*)hal_VocGetPointer(GLOBAL_ROM_BUFFER_X_ADDR);
            dmaCfg.size              = VPP_WMA_DEC_ROM_BUFFER_X_SIZE;
            dmaCfg.wr1Rd0            = HAL_VOC_DMA_READ;
            dmaCfg.needIrq           = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VPP_WMA]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);

            // load X constant tables
            dmaCfg.extAddr           = (UINT32*)TabinXEND;
            dmaCfg.vocLocalAddr      = (UINT32*)hal_VocGetPointer(TABLE_BandWeightToEQtype_ADDR);
            dmaCfg.size              = 280;
            dmaCfg.wr1Rd0            = HAL_VOC_DMA_READ;
            dmaCfg.needIrq           = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VPP_WMA]##WARNING##Error opening VoC DMA resource\n");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);


            // load Y constant tables
            dmaCfg.extAddr           = (UINT32*)TabinY;
            dmaCfg.vocLocalAddr      = (UINT32*)hal_VocGetPointer(GLOBAL_ROM_BUFFER_Y_ADDR);
            dmaCfg.size              = VPP_WMA_DEC_ROM_BUFFER_Y_SIZE;
            dmaCfg.wr1Rd0            = HAL_VOC_DMA_READ;
            dmaCfg.needIrq           = FALSE;

            if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
            {
                diag_printf("[VPP_WMA]##WARNING##Error opening VoC DMA resource");
                return HAL_ERR_RESOURCE_BUSY;
            }

            while (hal_VocDmaDone() != TRUE);
            break;

        default:

            diag_printf("[VPP_WMA]##WARNING##No first open, no code and constants reload\n");
            break;
    }



    *((short*)hal_VocGetPointer(GLOBAL_wmaResult_ADDR))= cWMA_NoErr;
    *((short*)hal_VocGetPointer(LOCAL_pcSampleGet_ADDR))= 0x0000;
    //fft table
    *(int *)hal_VocGetPointer(GLOBAL_FFTcostabExAddr_ADDR)=  (INT32)hal_VocGetDmaiPointer((INT32 *)costab,0,0,HAL_VOC_DMA_B2S_NO);
    //save history and reconstruction table
    *(int *)hal_VocGetPointer(GLOBAL_m_piSinForRecon_EXT_ADDR)=  (INT32)hal_VocGetDmaiPointer((INT32 *)g_SinTableForReconstruction,0,0,HAL_VOC_DMA_B2S_NO);
    //save history table
    *(int *)hal_VocGetPointer(GLOBAL_m_piSinForSaveHistory_ADDR)=(INT32) hal_VocGetDmaiPointer((INT32 *)(g_SinTableForReconstruction+2048),0,0,HAL_VOC_DMA_B2S_NO);
    //history data buffer: short [2048*2]
    *(int *)hal_VocGetPointer(GLOBAL_PrevOutputDMAExAddr_ADDR)=(INT32) hal_VocGetDmaiPointer((INT32 *)PrevOutput,0,0,HAL_VOC_DMA_B2S_NO);
    //output pcm data buffer: short [2048*2] & VOC_CFG_DMA_EADDR_MASK;
//  *(int *)hal_VocGetPointer(GLOBAL_OutPcmDMAExAddr_ADDR)= VOC_DMA_EXTERN_ADDR((int)iPCMBuffer);
    // huffman tables
    *(int *)hal_VocGetPointer(GLOBAL_HuffDecTblDMAExAddr_ADDR)= (INT32)hal_VocGetDmaiPointer((INT32 *)HuffmanTbl,0,0,HAL_VOC_DMA_B2S_NO);
//  *(int32*)hal_VocGetPointer(GLOBAL_ReLoadLPC_EADDR)=(INT32) hal_VocGetDmaiPointer((INT32 *)wma_codeL_LPC,0,0,HAL_VOC_DMA_B2S_NO);
//  *(int32*)hal_VocGetPointer(GLOBAL_ReLoadIDCT_EADDR)=(INT32) hal_VocGetDmaiPointer((INT32 *)wma_codeL_IDCT,0,0,HAL_VOC_DMA_B2S_NO);

    *(int32*)hal_VocGetPointer(GLOBAL_INPUT_OFFSET_ADDR)= (*(INT32*)hal_VocGetPointer(GLOBAL_currPacketOffset_ADDR))&0xfffffffc;
    *(int32*)hal_VocGetPointer(GLOBAL_INPUT_DATALEN_ADDR)= 0x800;//4096;
    *(INT32*)hal_VocGetPointer(GLOBAL_OutPcmDMAExAddr_ADDR) =(INT32)hal_VocGetDmaiPointer((INT32 *)output,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    *((INT16*)hal_VocGetPointer(GLOBAL_EQtype_ADDR)) = 0;
    *((INT16*)hal_VocGetPointer(GLOBAL_VOC_CFG_DMA_LADDR)) = 0x400;
    *((INT16*)hal_VocGetPointer(GLOBAL_Digital_gain_ADDR)) = 0x4000;
// init stream buffer
    dmaCfg.extAddr           = (INT32*)pStreamIn;// +((*(int32*)hal_VocGetPointer(GLOBAL_currPacketOffset_ADDR))>>2);
    dmaCfg.vocLocalAddr      = (INT32*)hal_VocGetPointer(GLOBAL_INPUT_DATA_ADDR);
    dmaCfg.size              = 0x800;//4096;
    dmaCfg.wr1Rd0            = HAL_VOC_DMA_READ;
    dmaCfg.needIrq           = FALSE;

    if (hal_VocDmaStart(&dmaCfg) == HAL_ERR_RESOURCE_BUSY)
    {
        diag_printf("[VPP_WMA]##ERROR##Error opening VoC DMA resource");
        return HAL_ERR_RESOURCE_BUSY;
    }

//  diag_printf("[VPP_WMA]start wait voc dam done\n");
    while (hal_VocDmaDone() != TRUE);
    diag_printf("[VPP_WMA]voc wakeup!\n");
    // move to STALL location (VoC irq generated)

    return HAL_ERR_NO;

}

void vpp_WmaStart_Voc(void)
{
    hal_VocWakeup(HAL_VOC_START);

}
//=============================================================================
// vpp_WmaDecClose function
//-----------------------------------------------------------------------------
void vpp_WmaDecClose(void)
{
    hal_VocClose();

    diag_printf("[VPP_WMA]##WARNING##Closing VPP");
}

//=============================================================================
// vpp_WmaDecStatus function
//-----------------------------------------------------------------------------
void vpp_WmaInitStatus(VPP_WMA_DEC_OUT_T * pDecStatus)
{
    // copy status structure from VoC RAM to the destination.
    pDecStatus->consumedLen = ((*(int*)hal_VocGetPointer(GLOBAL_INPUT_OFFSET_ADDR))+4096)>>2;
    pDecStatus->inputOffset = *(int*)hal_VocGetPointer(GLOBAL_INPUT_OFFSET_ADDR);
    pDecStatus->iSamplingRate = *(int*)hal_VocGetPointer(GLOBAL_m_iSamplingRate_ADDR);

    pDecStatus->WmaFileStatus = (VPP_WMA_DEC_STREAM_STATUS)*((short*)hal_VocGetPointer(GLOBAL_wmaResult_ADDR));
    pDecStatus->pcmFrameLen =(*(short*)hal_VocGetPointer(LOCAL_pcSampleGet_ADDR))>>2;
    pDecStatus->nbChannel = *((short*)hal_VocGetPointer(GLOBAL_m_cChannel_ADDR));

    diag_printf("[VPP_WMA]##WARNING##consumedLen : %x  iSamplingRate :%d \n",pDecStatus->consumedLen,pDecStatus->iSamplingRate );
}

//=============================================================================
// vpp_WmaDecStatus function
//-----------------------------------------------------------------------------
void vpp_WmaDecStatus(VPP_WMA_DEC_OUT_T * pDecStatus)
{
    // copy status structure from VoC RAM to the destination.
    //  int  tempInputOffset;
//  tempInputOffset = *(int32*)hal_VocGetPointer(GLOBAL_INPUT_OFFSET_ADDR);

//  pDecStatus->consumedLen = *(int32*)hal_VocGetPointer(GLOBAL_INPUT_OFFSET_ADDR);
    pDecStatus->inputOffset = *(int32*)hal_VocGetPointer(GLOBAL_INPUT_OFFSET_ADDR);
    pDecStatus->WmaFileStatus = (VPP_WMA_DEC_STREAM_STATUS)*((short*)hal_VocGetPointer(GLOBAL_wmaResult_ADDR));
    pDecStatus->pcmFrameLen =*((short*)hal_VocGetPointer(LOCAL_pcSampleGet_ADDR));
    pDecStatus->nbChannel = *((short*)hal_VocGetPointer(GLOBAL_m_cChannel_ADDR));
    pDecStatus->EQtype = *((short*)hal_VocGetPointer(GLOBAL_EQtype_ADDR));

}


//=============================================================================
// vpp_WmaDecScheduleOneFrame function
//-----------------------------------------------------------------------------
HAL_ERR_T vpp_WmaDecScheduleOneFrame(/*INT32 * pStreamIn,*/ INT32 * pStreamOut, UINT8 EQMode)
{
    INT16 alg_gain = 0x4000;

    alg_gain = aud_GetOutAlgGainDb2Val();
    //diag_printf("[vpp_WmaDecScheduleOneFrame] alg_gain:%d\n",alg_gain);

    // fill in the input structure
//  *(INT32*)hal_VocGetPointer(GLOBAL_FILE_POINTER_ADDR) = (INT32)hal_VocGetDmaiPointer(pStreamIn,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    *(INT32*)hal_VocGetPointer(GLOBAL_OutPcmDMAExAddr_ADDR) = (INT32)hal_VocGetDmaiPointer(pStreamOut,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    *((INT16*)hal_VocGetPointer(GLOBAL_EQtype_ADDR)) = (INT16)EQMode;

    *((INT16*)hal_VocGetPointer(GLOBAL_Mixer_flag_ADDR)) = 0;
    *((INT16*)hal_VocGetPointer(GLOBAL_adjust_eq_curve_enable_ADDR)) = 0;
    *((INT16*)hal_VocGetPointer(GLOBAL_Digital_gain_ADDR)) = alg_gain;

    // schedule next frame on sof1 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

}

HAL_ERR_T vpp_WmaDecScheduleOneFrame_For_Datafill( UINT8 EQMode)
{
    // fill in the input structure
    *((INT16*)hal_VocGetPointer(GLOBAL_EQtype_ADDR)) = (INT16)EQMode;
    // schedule next frame on sof1 event
    return hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

}



HAL_ERR_T vpp_WmaDecScheduleSetFile_Point_Addr(INT32 * pStreamIn)
{
    // fill in the input structure
    *(INT32*)hal_VocGetPointer(GLOBAL_FILE_POINTER_ADDR) = (INT32)hal_VocGetDmaiPointer(pStreamIn,HAL_VOC_DMA_READ,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

}


UINT32 vpp_WmaDecScheduleGetFile_Point_Addr(void)
{

    return  *(UINT32*)hal_VocGetPointer(GLOBAL_FILE_POINTER_ADDR);

}


HAL_ERR_T vpp_WmaDecScheduleSetOutput_Pcm_Addr(INT32 * pStreamout)
{
    // fill in the input structure
    *(INT32*)hal_VocGetPointer(GLOBAL_OutPcmDMAExAddr_ADDR) = (INT32)hal_VocGetDmaiPointer(pStreamout,HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

}


UINT32 vpp_WmaDecScheduleGetOutput_Pcm_Addr(void)
{
    // fill in the input structure
    return  *(INT32*)hal_VocGetPointer(GLOBAL_OutPcmDMAExAddr_ADDR);

}

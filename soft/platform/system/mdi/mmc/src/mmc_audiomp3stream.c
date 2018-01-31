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

#include "cswtype.h"
#include "mcip_debug.h"
#include "fs.h"
#include "mmc_audiomp3stream.h"
#include "hal_error.h"


//#include "cpu_comm.h"
//#include "lp_audio_codec.h"
#include "cpu_share.h"
#include "assert.h"
#include "hal_voc.h"

#include "global.h"
#include "audio_api.h"
//#include "mmi_trace.h"
//#include"stack_config.h"
#include "aud_m.h"
//#include "aud_maxim.h"
//#include "frameworkstruct.h"
//#include "filemgr.h"
#include "fs_asyn.h"
//#include "med_main.h"
#include "mci.h"
#include "cos.h"

#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"
#include "string.h"
#include "hal_overlay.h"

#define MP3STREAM_PCM_OUTPUT_BUFFER_SIZE (1152*4*4)

extern UINT32 gnMmcMode;
extern Audio_Selector gAudio;

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;


MP3_STREAM_PLAY Mp3StreamPlayer;
static volatile INT32 MMC_Mp3Stream_Pcm_Half;
static volatile UINT32 Mmc_Mp3Stream_PcmDataCopyCount = 0;
static volatile UINT32 ContinueFlag = 0;
static UINT32 ErrorFrameCount=0;
extern UINT32 pcmbuf_overlay[4608];


/*
* NAME: MMC_Mp3StreamDecClose()
* DESCRIPTION: Close aac decoder.
*/
MCI_ERR_T MMC_Mp3StreamDecClose (MP3_STREAM_PLAY *pMp3StreamPlayer)
{
    aud_StreamStop(audioItf);
    vpp_AudioJpegDecClose();
    mmc_MemFreeAll();
    // Free the buffer that was allocated in internal SRAM
    //  if(pMp3StreamPlayer->Mp3StreamOutput.pcm.Buffer!=NULL)
//   {
    //      sxr_Free(pMp3StreamPlayer->Mp3StreamOutput.pcm.Buffer);
    //      pMp3StreamPlayer->Mp3StreamOutput.pcm.Buffer=NULL;
    // }


    return  MCI_ERR_NO;
}

/*
* NAME: MMC_Mp3StreamVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_Mp3StreamVocISR(void)
{
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MP3STREAMDEC_VOC);
}

/*
* NAME: MMC_Mp3StreamHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/
void MMC_Mp3StreamHalfPcmISR(void)
{
    MMC_Mp3Stream_Pcm_Half = 0;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MP3STREAMDEC_PCM);
}

/*
* NAME: MMC_Mp3StreamEndPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/
void MMC_Mp3StreamEndPcmISR(void)
{
    MMC_Mp3Stream_Pcm_Half = 1;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_MP3STREAMDEC_PCM);
}

void MMC_Mp3StreamPcmOutStart(MP3_STREAM_PLAY *pMp3StreamPlayer)
{
    MCI_TRACE (MCI_AUDIO_TRC,0, "*******************MMC_Mp3StreamPcmOutStart begin**********************");
    //Stream out
    audioStream.startAddress  = (UINT32*) pMp3StreamPlayer->Mp3StreamOutput.pcm.Buffer;
    audioStream.length        = pMp3StreamPlayer->Mp3StreamOutput.pcm.Size*4;//MP3STREAM_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
    audioStream.channelNb     = pMp3StreamPlayer->Voc_Mp3StreamDecStatus.nbChannel;//HAL_AIF_STEREO;//HAL_AIF_MONO;
    audioStream.voiceQuality  = !TRUE;    // To adjust with a key
    audioStream.playSyncWithRecord = FALSE;
    audioStream.halfHandler   = MMC_Mp3StreamHalfPcmISR;
    audioStream.endHandler    = MMC_Mp3StreamEndPcmISR;
    // Initial cfg

    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER&&audioItf!=AUD_ITF_BLUETOOTH)
    {
        // Initial cfg
        audioItf = AUD_ITF_EAR_PIECE;
    }



    MCI_TRACE (MCI_AUDIO_TRC,0, "*******************pMp3StreamPlayer->Voc_Mp3StreamDecStatus.SampleRate:%d**********************",pMp3StreamPlayer->Voc_Mp3StreamDecStatus.SampleRate);

    switch (pMp3StreamPlayer->Voc_Mp3StreamDecStatus.SampleRate)
    {
        case 48000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_48K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_48000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 44100:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_44_1K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 32000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_32K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_32000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 24000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_24K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_24000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 22050:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_22_05K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_22050HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;

        case 16000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_16K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_16000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 12000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_12K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_12000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 11025:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_11_025K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_11025HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 8000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_8K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        default:
            diag_printf("[MMC_MP3_STRAEM]##WARNING## Sample rate error:%d\n",pMp3StreamPlayer->Voc_Mp3StreamDecStatus.SampleRate);
            break;
    }
    UINT32 errStatus =  aud_StreamStart(audioItf, &audioStream, &audio_cfg);
    MCI_TRACE (MCI_AUDIO_TRC,0, "*******************MMC_Mp3StreamPcmOutStart end; errStatus = %d", errStatus);
}

/*
* NAME: Mp3StreamDecVoc()
* DESCRIPTION: Decode mp3 stream ......
*/
//static    HANDLE testfileHandle;
MCI_ERR_T MMC_Mp3StreamDecVoc (MP3_STREAM_PLAY *pMp3StreamPlayer)
{
    MP3_STREAM_INPUT *input = &pMp3StreamPlayer->Mp3StreamInput;
    MP3_STREAM_OUTPUT *output = &pMp3StreamPlayer->Mp3StreamOutput;
    vpp_AudioJpeg_DEC_OUT_T *pVoc_Mp3StreamDecStatus = &pMp3StreamPlayer->Voc_Mp3StreamDecStatus;
    vpp_AudioJpeg_DEC_IN_T *pVoc_Mp3StreamDecIN = &pMp3StreamPlayer->Voc_Mp3StreamDecIN;

    if(!input->is_open_intr)
    {
        vpp_AudioJpegDecStatus(pVoc_Mp3StreamDecStatus);
        input->data += pVoc_Mp3StreamDecStatus->consumedLen;

        if(input->data >= input->bof + input->length * 4)
        {
            return MCI_ERR_END_OF_FILE;
        }

        if(pVoc_Mp3StreamDecStatus->ErrorStatus == 0)
        {
            ErrorFrameCount = 0;

            if(MMC_Mp3Stream_Pcm_Half == 0)
            {
                output->pcm.Buffer_head = output->pcm.Buffer;
            }
            else if(MMC_Mp3Stream_Pcm_Half == 1)
            {
                output->pcm.Buffer_head = output->pcm.Buffer + (output->pcm.Size>>1);
            }

            Mmc_Mp3Stream_PcmDataCopyCount += (pVoc_Mp3StreamDecStatus->output_len>>2);
            output->data = (int16 *)&output->pcm.Buffer_head[Mmc_Mp3Stream_PcmDataCopyCount];
        }
        else
        {
            ErrorFrameCount++;
            MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_MP3_STRAEM]##WARNING##Error Frame!:%d\n",ErrorFrameCount);
            if(ErrorFrameCount>20)
            {
                MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_MP3_STRAEM]##WARNING##ErrorFrameCount>20!\n");
                return MCI_ERR_ERROR;
            }
        }
    }
    else
    {
        MCI_TRACE (MCI_AUDIO_TRC,0,"first interrupt");
        input->is_open_intr = 0;
    }

    pVoc_Mp3StreamDecIN->inStreamBufAddr = (INT32 *)input->data;
    pVoc_Mp3StreamDecIN->outStreamBufAddr = (INT32 *)output->data;
    pVoc_Mp3StreamDecIN->mode = pMp3StreamPlayer->mode;
    pVoc_Mp3StreamDecIN->BsbcEnable = 0;

    if(input->is_first_fill_pcm)
    {
        if(Mmc_Mp3Stream_PcmDataCopyCount >= (output->pcm.Size))
        {
            Mmc_Mp3Stream_PcmDataCopyCount = 0;
            input->is_first_fill_pcm = 0;
            output->data = (int16 *)output->pcm.Buffer;
            MMC_Mp3StreamPcmOutStart(pMp3StreamPlayer);
        }
        else
        {
            vpp_AudioJpegDecScheduleOneFrame(pVoc_Mp3StreamDecIN);
        }
    }
    else
    {
        if(Mmc_Mp3Stream_PcmDataCopyCount >= (output->pcm.Size>>1))
        {
            Mmc_Mp3Stream_PcmDataCopyCount = 0;
            if(MMC_Mp3Stream_Pcm_Half == 1)
            {
                output->data = (int16 *)output->pcm.Buffer;
            }
        }
        else
        {
            vpp_AudioJpegDecScheduleOneFrame(pVoc_Mp3StreamDecIN);
        }
    }

    return MCI_ERR_NO;
}

MCI_ERR_T MMC_Mp3StreamDecPcm(MP3_STREAM_PLAY *pMp3StreamPlayer)
{
    MP3_STREAM_INPUT *input = &pMp3StreamPlayer->Mp3StreamInput;
    MP3_STREAM_OUTPUT *output = &pMp3StreamPlayer->Mp3StreamOutput;
    vpp_AudioJpeg_DEC_IN_T *pVoc_Mp3StreamDecIN = &pMp3StreamPlayer->Voc_Mp3StreamDecIN;

    pVoc_Mp3StreamDecIN->inStreamBufAddr = (INT32 *)input->data;
    pVoc_Mp3StreamDecIN->outStreamBufAddr = (INT32 *)output->data;
    pVoc_Mp3StreamDecIN->mode = pMp3StreamPlayer->mode;
    pVoc_Mp3StreamDecIN->BsbcEnable = 0;

    if(ContinueFlag)
    {
        //Drv_PcmOutJudgeWhetherPlayEnd(&(output->pcm.Buffer_head[Mmc_Mp3Stream_PcmDataCopyCount>>1])-576);
        diag_printf("!!!!!play all left data in the pcm buffer\n");
        MMC_Mp3StreamDecClose(pMp3StreamPlayer);
        //EVENT_Send2Jade(CHAN_MMC_ID, MCI_MEDIA_PLAY_FINISH_IND, 5, 0, 0);
        //mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
        ContinueFlag = 0;
        gnMmcMode = MMC_MODE_IDLE;
        gAudio.UserMsg = 0;
        if(pMp3StreamPlayer->loop == 1)
        {
            AudioMp3StartStream((uint32 *)pMp3StreamPlayer->Mp3StreamInput.bof, pMp3StreamPlayer->Mp3StreamInput.length, pMp3StreamPlayer->loop);
        }
        else
        {
            //MCI_RingFinished();
        }
    }
    else
    {
        if(Mmc_Mp3Stream_PcmDataCopyCount==0)
        {
            vpp_AudioJpegDecScheduleOneFrame(pVoc_Mp3StreamDecIN);
        }
        else
        {
            MCI_TRACE (MCI_AUDIO_TRC,0, "*******************X*****************");
            //EVENT_Send2Task(MBOX_ID_MMC,MSG_MMC_AUDIODEC_PCM);
        }
    }

    return MCI_ERR_NO;
}


int32 AudioMp3StartStream(uint32 *pBuffer, uint32 len, uint8 loop)
{
    HAL_ERR_T voc_ret = 0;
    gnMmcMode = MMC_MODE_AUDIO;
    gAudio.UserMsg = Audio_Mp3StreamUserMsg;

    Mp3StreamPlayer.mode = MMC_MP3_DECODE;
    Mp3StreamPlayer.loop = loop;
    Mp3StreamPlayer.Mp3StreamInput.bof = (uint8 *)pBuffer;
    Mp3StreamPlayer.Mp3StreamInput.data = (uint8 *)pBuffer;
    Mp3StreamPlayer.Mp3StreamInput.length = len;

    //Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer = (uint32 *)mmc_MemMalloc(MP3STREAM_PCM_OUTPUT_BUFFER_SIZE);
    // allocate the buffer in internal SRAM
    // Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer = (uint32 *)sxr_IntMalloc(MP3STREAM_PCM_OUTPUT_BUFFER_SIZE);
    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);
    Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer = (uint32 *)pcmbuf_overlay;

    memset(Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer,0,MP3STREAM_PCM_OUTPUT_BUFFER_SIZE);
    Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer_head = Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer;
    Mp3StreamPlayer.Mp3StreamOutput.pcm.Size = MP3STREAM_PCM_OUTPUT_BUFFER_SIZE>>2;

    Mp3StreamPlayer.Mp3StreamOutput.data = (int16 *)Mp3StreamPlayer.Mp3StreamOutput.pcm.Buffer;

    Mp3StreamPlayer.Voc_Mp3StreamDecIN.EQ_Type = -1;
    Mp3StreamPlayer.Voc_Mp3StreamDecIN.reset = 1;

    Mp3StreamPlayer.Mp3StreamInput.is_first_fill_pcm = 1;
    Mp3StreamPlayer.Mp3StreamInput.is_open_intr = 1;

    //initialize global variable
    MMC_Mp3Stream_Pcm_Half = 0;
    Mmc_Mp3Stream_PcmDataCopyCount = 0;
    ContinueFlag = 0;
    ErrorFrameCount = 0;

    /*open voc decoder*/
    voc_ret = vpp_AudioJpegDecOpen((HAL_VOC_IRQ_HANDLER_T)MMC_Mp3StreamVocISR);
    switch (voc_ret)
    {
        case HAL_ERR_NO:
            //diag_printf("[MMC_MP3_STRAEM] success to call vpp_AudioJpegDecOpen(), result: %d \n", voc_ret);
            break;

        case HAL_ERR_RESOURCE_RESET:
        case HAL_ERR_RESOURCE_BUSY:
        case HAL_ERR_RESOURCE_TIMEOUT:
        case HAL_ERR_RESOURCE_NOT_ENABLED:
        case HAL_ERR_BAD_PARAMETER:
        case HAL_ERR_UART_RX_OVERFLOW:
        case HAL_ERR_UART_TX_OVERFLOW:
        case HAL_ERR_UART_PARITY:
        case HAL_ERR_UART_FRAMING:
        case HAL_ERR_UART_BREAK_INT:
        case HAL_ERR_TIM_RTC_NOT_VALID:
        case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
        case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
        case HAL_ERR_COMMUNICATION_FAILED:
        case HAL_ERR_QTY:
            diag_printf("[MMC_MP3_STRAEM]##WARNING## fail to call vpp_AudioJpegDecOpen(), result: %d \n", voc_ret);
            return MCI_ERR_ERROR;
        default:
            break;
    }
    return MCI_ERR_NO;

}

int32 AudioMp3StopStream(void)
{
    MMC_Mp3StreamDecClose(&Mp3StreamPlayer);

    gnMmcMode = MMC_MODE_IDLE;
    gAudio.UserMsg = 0;
    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
    return 0;
}

int32 Audio_Mp3StreamUserMsg(int32 nMsg)
{
    switch (nMsg)
    {
        case MSG_MMC_MP3STREAMDEC_VOC:
            switch (MMC_Mp3StreamDecVoc(&Mp3StreamPlayer))
            {
                case MCI_ERR_END_OF_FILE:
                    diag_printf("[MMC_MP3_STRAEM] return FINISH!\n");
                    ContinueFlag = 1;
                    break;
                case MCI_ERR_ERROR:
                    diag_printf("[MMC_MP3_STRAEM]##WARNING## return FALSE!\n");
                    MMC_Mp3StreamDecClose(&Mp3StreamPlayer);
                    break;
                default:
                    break;
            }
            break;

        case MSG_MMC_MP3STREAMDEC_PCM:
            MMC_Mp3StreamDecPcm(&Mp3StreamPlayer);
            break;

        default:
            diag_printf("[MMC_MP3_STRAEM]##WARNING##**************Error message******************!\n");
            break;

    }
    return 0;
}


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

#include "mmc_audiosco.h"

#include "mmc.h"
#include "cpu_share.h"

#include "audio_api.h"

#include "aud_m.h"
#include "fs_asyn.h"

#include "cos.h"
#include "mci.h"
#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"

#include "string.h"
#include "hal_overlay.h"

#include "mmc_adpcm.h"
#include "resample.h"
#include "ars_m.h"
#include "event.h"

#define MMC_SCO_BUFF_SIZE              (480)  //(480) // in pcm sample, total use 480*4 buffs, two for aif play, two for sco receive = 480*4*2bytes

static INT8 Mmc_SCO_Pcm_Half;
static INT8 Mmc_SCO_MIC_Half;

static INT8 g_continue_play_flag = 0;

SCO_PLAY SCOPlayer;

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

HAL_AIF_STREAM_T micStream;

int rdabt_uart_tx_sco_data(int16 *pdu, uint16 handle, uint16 length);

/*
* NAME: MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioSCOVocISR(void)
{
//  //dbg_TraceOutputText(0, "[MCI_SCOSBC]VoC INT!");
//      mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SCO_SBC_VOC);
}

/*
* NAME: MMC_AudioSCOHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

void MMC_AudioSCOHalfPcmISR(void)
{
//  //dbg_TraceOutputText(0, "[MMC_SCO]HALF!");
    Mmc_SCO_Pcm_Half = 0;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SCO_PCM_INT);
}

/*
* NAME: MMC_AudioSCOEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioSCOEndPcmISR(void)
{
    ////dbg_TraceOutputText(0, "[MMC_SCO]END!");
    Mmc_SCO_Pcm_Half = 1;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SCO_PCM_INT);

}


/*
* NAME: MMC_AudioSCOHalfPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/

void MMC_AudioSCOHalfMicISR(void)
{
    ////dbg_TraceOutputText(0, "[MMC_SCO]HALF!");
    Mmc_SCO_MIC_Half = 0;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SCO_MIC_INT);
}

/*
* NAME: MMC_AudioSCOEndPcmISR()
* DESCRIPTION: ISR for pcm end buffer exhausted.
*/
void MMC_AudioSCOEndMicISR(void)
{
    ////dbg_TraceOutputText(0, "[MMC_SCO]END!");
    Mmc_SCO_MIC_Half = 1;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_SCO_MIC_INT);

}

void MMC_SCOPcmOutStart(SCO_PLAY *pSCOPlayer)
{
    MCI_ERR_T result;
    //dbg_TraceOutputText(0,"MMC_SCOPcmOutStart ");

    //hal_HstSendEvent(SYS_EVENT,0x10280010);
    // Stream out
    audioStream.startAddress    = (UINT32 *)pSCOPlayer->SCOOutput.Buffer;
    audioStream.length          = pSCOPlayer->SCOOutput.Size;
    audioStream.channelNb       = HAL_AIF_MONO;
    //audioStream.bits = 16;

    audioStream.voiceQuality   = !TRUE;
    audioStream.halfHandler     = MMC_AudioSCOHalfPcmISR;
    audioStream.endHandler    = MMC_AudioSCOEndPcmISR;

    // Initial cfg
    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER&&audioItf!=AUD_ITF_BLUETOOTH)
    {
        // Initial cfg
        //  if(gpio_detect_earpiece())
        //audioItf = AUD_ITF_EAR_PIECE;
        // else
        audioItf = AUD_ITF_RECEIVER;
        //    audioItf = AUD_ITF_LOUD_SPEAKER;
    }

    audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;

    result = aud_StreamStart(audioItf, &audioStream, &audio_cfg);

    // steam in
    micStream.startAddress      = (UINT32 *)(pSCOPlayer->SCOInput.data+MMC_SCO_BUFF_SIZE);
    micStream.length            = pSCOPlayer->SCOInput.size;//MP3DEC_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
    micStream.channelNb         = 1;//pSCOPlayer->SCOOutput.Channel;//HAL_AIF_STEREO;//HAL_AIF_MONO;
    //micStream.bits                   = 16;
    micStream.sampleRate    = HAL_AIF_FREQ_8000HZ;

    micStream.voiceQuality   = TRUE;
    micStream.halfHandler    = MMC_AudioSCOHalfMicISR;
    micStream.endHandler    = MMC_AudioSCOEndMicISR;

    result = aud_StreamRecord(audioItf, &micStream, &audio_cfg);
}



/*
* NAME: MMC_AudioSCODecOpen()
* DESCRIPTION: Initialize SCO decoder.
*/
UINT32 sco_packet_send_tx = 0x0;

extern uint32 pcmbuf_overlay[4608];
MCI_ERR_T MMC_AudioSCODecOpen (SCO_PLAY *pSCOPlayer, HANDLE sco_handle)
{
    SCO_INPUT *input = &pSCOPlayer->SCOInput;
    SCO_OUTPUT *output = &pSCOPlayer->SCOOutput;
    dbg_TraceOutputText(0,"MMC_AudioSCODecOpen sco_handle %d",sco_handle);

    pSCOPlayer->in_flag = 0;
    input->sco_handle = sco_handle;

    output->Buffer= (uint16 *)pcmbuf_overlay;
    output->length = 0;
    output->Size = MMC_SCO_BUFF_SIZE*4;
    memset(output->Buffer, 0, output->Size*2);

    input->data = pcmbuf_overlay+MMC_SCO_BUFF_SIZE*2;
    input->length = MMC_SCO_BUFF_SIZE;
    input->size = MMC_SCO_BUFF_SIZE*2;//(MCI_INPUT_BUF_SIZE/pSCOPlayer->packet_size-1)*pSCOPlayer->packet_size*2;
    memset(input->data, 0, input->size*2);

    g_continue_play_flag = 1;
    sco_packet_send_tx = 0;

    //hal_HstSendEvent(SYS_EVENT,0x10280020);

    //hal_HstSendEvent(SYS_EVENT,output->Buffer);
    //hal_HstSendEvent(SYS_EVENT,output->Size);
    //hal_HstSendEvent(SYS_EVENT,input->data);
    //hal_HstSendEvent(SYS_EVENT,input->size);

    return MCI_ERR_NO;
}

int16 *MMC_SCOGetRXBuffer(void)
{
    SCO_OUTPUT *output = &SCOPlayer.SCOOutput;
    //  dbg_TraceOutputText(0,"MMC_SCOGetRXBuffer ");


    if(!g_continue_play_flag)
        return NULL;
#if 0
    if(!rdabt_sco_player_states()||!mmi_bt_phone_getCallFlag())
    {
        if(g_continue_play_flag)
            aud_StreamStop(audioItf);
        g_continue_play_flag = 0;
        return NULL;
    }
#endif
    if(output->length == MMC_SCO_BUFF_SIZE*4)
    {
        //hal_HstSendEvent(SYS_EVENT,0x84400002);
        output->length -= SCOPlayer.packet_size/2;
    }
    return output->Buffer + output->length;
}

int16 MMC_SCOReceiveData(int32 length)
{
    SCO_PLAY *pSCOPlayer = &SCOPlayer;
    SCO_OUTPUT *output = &pSCOPlayer->SCOOutput;
    SCO_INPUT *input = &pSCOPlayer->SCOInput;
    //   dbg_TraceOutputText(0,"MMC_SCOReceiveData %d ,length %d ",g_continue_play_flag,length);

    if(!g_continue_play_flag)
        return 0;
#if 0
    if(!rdabt_sco_player_states()||!mmi_bt_phone_getCallFlag())
    {
        if(g_continue_play_flag)
            aud_StreamStop(audioItf);
        g_continue_play_flag = 0;
        return ;
    }
#endif
    output->length += length/2;
    pSCOPlayer->packet_size = length;

//   dbg_TraceOutputText(0,"MMC_SCOReceiveData output->length %d half buff? %d ",output->length , Mmc_SCO_MIC_Half);
    //dbg_TraceOutputText(0,"MMC_SCOReceiveData pSCOPlayer->in_flag? %d ", pSCOPlayer->in_flag);

    if(pSCOPlayer->in_flag == 1)
    {
        ////hal_HstSendEvent(SYS_EVENT,0x10280060);
        ////hal_HstSendEvent(SYS_EVENT,input->length);

        if(input->length+length/2 > MMC_SCO_BUFF_SIZE)
        {
            //hal_HstSendEvent(SYS_EVENT,0x84400004);
        }
        else
        {
            //  uint8 pkt_head[4];
            uint16 bytesWritten;

            //  pkt_head[0] = 0x03;
            //  pkt_head[1] = (input->sco_handle)&0xff;
            //  pkt_head[2] = ((input->sco_handle)>>8)&0xff;
            //  pkt_head[3] = length-4;

            //   memcpy(input->data+input->length,pkt_head,4 );
            //    sco_packet_send_tx++;
            MMC_AudioSCO_AsynSendReq(input->data+input->length, input->sco_handle, length);
            // rdabt_uart_tx_sco_data(input->data+input->length, input->sco_handle, length);
            input->length += length/2;
        }
    }
    else if(output->length >= MMC_SCO_BUFF_SIZE*3)
    {
        //hal_HstSendEvent(SYS_EVENT,0x10270030);
        //hal_HstSendEvent(SYS_EVENT,output->length);
        pSCOPlayer->in_flag = 1;
        MMC_SCOPcmOutStart(pSCOPlayer);
    }

    return 1;

}

MCI_ERR_T  MMC_AudioSCOMicData(SCO_PLAY *pSCOPlayer)
{
    SCO_INPUT *input = &pSCOPlayer->SCOInput;
    UINT32 sco_length;
    INT16 *PCM_q=(INT16 *)input->data;
    sco_length = pSCOPlayer->packet_size/2;
#if 0
    if(!rdabt_sco_player_states()||!mmi_bt_phone_getCallFlag())
    {
        if(g_continue_play_flag)
            aud_StreamStop(audioItf);
        g_continue_play_flag = 0;
        return MCI_ERR_NO;
    }
#endif
    while(input->length < MMC_SCO_BUFF_SIZE/2)
    {
        //uint8 pkt_head[4];
        uint16 bytesWritten;
        ////hal_HstSendEvent(SYS_EVENT,0x84400003);]


        //pkt_head[0] = 0x03;
        //pkt_head[1] = (input->sco_handle)&0xff;
        //pkt_head[2] = ((input->sco_handle)>>8)&0xff;
        //pkt_head[3] = sco_length - 4;
        //memcpy(input->data+input->length,pkt_head,4);
        // rdabt_uart_tx_sco_data(input->data+input->length, input->sco_handle, sco_length);
        //MMC_AudioSCO_AsynSendReq(input->data+input->length, input->sco_handle, sco_length);
        input->length += sco_length/2;
    }
    if(Mmc_SCO_MIC_Half == 0) // half
    {
        memcpy(PCM_q+input->length-MMC_SCO_BUFF_SIZE/2, PCM_q+input->length, MMC_SCO_BUFF_SIZE*3-input->length*2);
    }
    else // end
    {
        memcpy(PCM_q+input->length-MMC_SCO_BUFF_SIZE/2, PCM_q+input->length, (MMC_SCO_BUFF_SIZE-input->length)*2);
        memcpy(PCM_q+MMC_SCO_BUFF_SIZE/2, PCM_q+MMC_SCO_BUFF_SIZE*3/2, MMC_SCO_BUFF_SIZE);
    }
    input->length -= MMC_SCO_BUFF_SIZE/2;
    return MCI_ERR_NO;
}
/*
* NAME: MMC_AudioSCODecPcm()
* DESCRIPTION: Initialize SCO decoder.
*/
MCI_ERR_T MMC_AudioSCODecPcm(SCO_PLAY *pSCOPlayer)
{
    SCO_OUTPUT *output = &pSCOPlayer->SCOOutput;

    if(output->length < MMC_SCO_BUFF_SIZE*3)
    {
        //hal_HstSendEvent(SYS_EVENT,0x84400001);
        return MCI_ERR_NO;
    }

    if(Mmc_SCO_Pcm_Half==0) // half
    {
        memcpy(output->Buffer, output->Buffer+MMC_SCO_BUFF_SIZE*2, MMC_SCO_BUFF_SIZE*2);
        memcpy(output->Buffer+MMC_SCO_BUFF_SIZE*2, output->Buffer+MMC_SCO_BUFF_SIZE*3, MMC_SCO_BUFF_SIZE*2);
    }
    else if(Mmc_SCO_Pcm_Half==1) // end
    {
        memcpy(output->Buffer+MMC_SCO_BUFF_SIZE, output->Buffer+MMC_SCO_BUFF_SIZE*2, MMC_SCO_BUFF_SIZE*4);
    }
    output->length -= MMC_SCO_BUFF_SIZE;

    ////hal_HstSendEvent(SYS_EVENT,0x10270035);
    ////hal_HstSendEvent(SYS_EVENT,output->length);

    return MCI_ERR_NO;
}

/*
* NAME: MMC_AudioSCODecClose()
* DESCRIPTION: Close SCO decoder.
*/
MCI_ERR_T MMC_AudioSCODecClose (SCO_PLAY *pSCOPlayer)
{
    dbg_TraceOutputText(0,"MMC_AudioSCODecClose ");
    if(g_continue_play_flag == 0) return MCI_ERR_NO;

    pSCOPlayer->in_flag = 0;

    g_continue_play_flag = 0;
    aud_StreamStop(audioItf);
    mmc_MemFreeAll();

//tianwq        hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    return  MCI_ERR_NO;

}

int32 Audio_SCOPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{

    dbg_TraceOutputText(0,"[MCI_SCO]g_AMRConsumedLength: %d ",fhd);

    MMC_AudioSCODecOpen(&SCOPlayer, fhd);

    return  MCI_ERR_NO;
}

int32 Audio_SCOStop (void)
{
    dbg_TraceOutputText(0,"Audio_SCOStop ");

    MMC_AudioSCODecClose(&SCOPlayer);
    return  MCI_ERR_NO;
}


int32 Audio_SCOPause (void)
{
    //dbg_TraceOutputText(0,"[MMC_SCO] Audio_SCOPause!!!!");
    //MMC_AudioSCODecClose(&SCOPlayer);
    return  MCI_ERR_NO;
}


int32 Audio_SCOResume ( HANDLE fhd)
{
    //dbg_TraceOutputText(0,"[MMC_SCO]Audio_SCOResume!");
    //dbg_TraceOutputText(0,"[MMC_SCO] Audio_SCOResume!!!!");
    return  MCI_ERR_NO;
}


int32 Audio_SCOGetID3 (char * pFileName)
{
    return 0;
}

int32 Audio_SCOGetPlayInformation (MCI_PlayInf * MCI_PlayInfSCO)
{
    MCI_PlayInfSCO->PlayProgress=0;

    //dbg_TraceOutputText(0,"[MCI_SCO]PlayProgress:%d",MCI_PlayInfSCO->PlayProgress);

    return MCI_ERR_NO;
}
//(int16 *pdu, uint16 handle, uint16 length)



int32 Audio_SCOUserMsg(int32 nMsg)
{
    switch (nMsg)
    {
        case MSG_MMC_SCO_PCM_INT:
            if(MMC_AudioSCODecPcm(&SCOPlayer)==MCI_ERR_END_OF_FILE)
            {
                ////dbg_TraceOutputText(0,"[MMC_SCO]END OF FILE!");
                g_continue_play_flag = 2;
            }
            break;


        case MSG_MMC_SCO_PCM_END:

            ////dbg_TraceOutputText(0,"[MMC_SCO]MSG_MMC_SCO_PCM_END\n");
            MMC_AudioSCODecClose(&SCOPlayer);
            MCI_AudioFinished(MCI_ERR_END_OF_FILE);
            mmc_SetCurrMode(MMC_MODE_IDLE);

            break;

        case MSG_MMC_SCO_DATA_IND:
            //  //dbg_TraceOutputText(0,"[MMC_SCO]MSG_MMC_SCO_DATA_IND \n");
            //MMC_AudioReceiveSCOData(&SCOPlayer);
            break;

        case MSG_MMC_SCO_MIC_INT:
            MMC_AudioSCOMicData(&SCOPlayer);
            break;

        default:

            //dbg_TraceOutputText(0,"[MMC_SCO_ERR]ERROR MESSAGE!");

            break;
    }
    return 0;
}



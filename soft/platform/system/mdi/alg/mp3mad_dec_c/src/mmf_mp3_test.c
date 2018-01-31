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


#if 0

#include "fs.h"
#include "os_api.h"
#include <cyg/kernel/diag.h>
#include "lp_pcm_driver.h"
//#include "lp_audio_codec.h"

#include "global_macros.h"
#include "lp_ct810reg.h"

#include <cyg/kernel/kapi.h>



#define PCM_PLAY_SIZE   (8*1024)


char pcmDataBuffer[]=
{
//  #include "MP88K.tab"
#include "teleno8k16b.tab"
//  #include "MP88K.tab"
//  #include "t_16K.tab"
//  #include "horse.tab"
};
uint32 pcmDataBufferSize=sizeof(pcmDataBuffer);

static uint32 pcmConsumeSize;

int32 pcm_play_buffer[PCM_PLAY_SIZE];
int16 *pOutBufData;
int32 *pFeedData;


//unsigned char dongwlOut[20*1024];

//jiashuo+++,for there is no external D/A in the new lily board
void pcm_audio_dac_cfg11(void)
{
    LILY_WR_REG(REG_RST_SET,    0x00004000);    //AU_RSTB->0 reset
    cyg_thread_delay(1);
    LILY_WR_REG(REG_RST_CLEAR,  0x00004000);    //AU_RSTB->1; reset clear
    cyg_thread_delay(10);

    LILY_WR_REG(AU_MIXER_ST_L,  0x50505001);
    cyg_thread_delay(3);
    LILY_WR_REG(AU_MIXER_ST_R,  0x50505010);
    cyg_thread_delay(3);
    LILY_WR_REG(AU_HP_CTRL,     LILY_RD_REG(AU_HP_CTRL) | 0x00010001);
    cyg_thread_delay(3);
    LILY_WR_REG(AU_LS_OUT3_CTRL,    LILY_RD_REG(AU_LS_OUT3_CTRL) | 0x00010001);
    cyg_thread_delay(3);
    LILY_WR_REG(AU_REF_GEN,     LILY_RD_REG(AU_REF_GEN) | 0x00000030);
    cyg_thread_delay(6);
}
//jiashuo---,for there is no external D/A in the new lily board

void DRV_PCMOutStart11(PCMSETTING *pPCMSet)
{
    pcm_out_t pcm_tx;
    int rtn;


    printf("jiashuo++++++++++++++++++++++++++++++++++++++++++++++++++DRV_PCMOutStart\n");
    pcm_audio_dac_cfg11();//jiashuo+,for there is no external D/A in the new lily board

    pcm_tx.format.bits.enable = PCM_OUT_ENABLE;
    pcm_tx.format.bits.resol = PCM_OUT_RESO_24BIT;//jiashuo_change,there is no external D/A in the new lily board

    pcm_tx.format.bits.align = PCM_OUT_ALIGN_RIGHT;
    pcm_tx.format.bits.endian = PCM_OUT_B_ENDIAN;
    pcm_tx.format.bits.time_inc = PCM_OUT_TIME_INC_0;
    pcm_tx.format.bits.lrck_pol = PCM_OUT_LRCK_H_LEFT;
    pcm_tx.format.bits.sclk_lrck = PCM_OUT_LRCK_48_SCLK;//jiashuo_change
    pcm_tx.format.bits.pack = PCM_OUT_NONPACK_MODE;//jiashuo_change

    pcm_tx.format.bits.mono_out = pPCMSet->Channel >1 ? PCM_OUT_OUTSIDE_ST: PCM_OUT_OUTSIDE_MO_MO;
    pcm_tx.format.bits.mono_in = pPCMSet->Channel >1 ?PCM_OUT_INSIDE_ST: PCM_OUT_INSIDE_MO;
    pcm_tx.format.bits.mono_r = PCM_OUT_INSIDE_MO_FROM_L;

    pcm_tx.buf = pPCMSet->Buffer;
    pcm_tx.length = pPCMSet->Size;
    pcm_tx.callback = pPCMSet->OnPCM;

    printf("pcm_tx buffer is: 0x%x\n", pcm_tx.buf);
    //pcm_tx.format.reg = g_pcm_format_default.reg;
    trace(" in %s, %d\n",__FILE__,__LINE__);

//  pcm_init(PCM_SAMPLE_RATE_44_1K, PCM_SAMPLE_RATE_44_1K);

    pcm_init(PCM_SAMPLE_RATE_16K, PCM_SAMPLE_RATE_16K);

    pcm_out_setgain(PCM_OUT_USER_N_20);

    rtn = pcm_tx_start(&pcm_tx);
    printf("tx start: %d\n", rtn);
    trace(" in %s, %d\n",__FILE__,__LINE__);
}

static int times;

void MMF_MP3_ISR(int half);


void MP3_PCMIsr(int half)
{
    int i;

    if(times>8)
    {
        times=0;
        pOutBufData=(int16 *)pcmDataBuffer;
    }

//  if(pOutBufData>pcmDataBufferSize)

//  diag_printf("MP3_PCMIsr: %d\n",half);

    pFeedData= (half == PCM_FIRST_HALF ) ? pcm_play_buffer :\
               (pcm_play_buffer + PCM_PLAY_SIZE/2);

    for(i=0; i< PCM_PLAY_SIZE/2; i++,pFeedData++, pOutBufData++)
        *pFeedData=(int32)(*pOutBufData);

    times++;

}

void MP3_PCMOUT_Start()
{
    PCMSETTING aPCMSet;
    PCMSETTING *pPCMSet=&aPCMSet;

    int k;



    pPCMSet->Channel=2;
    pPCMSet->Buffer=pcm_play_buffer;
    pPCMSet->Size=PCM_PLAY_SIZE;
    pPCMSet->OnPCM=MMF_MP3_ISR;

    DRV_PCMOutStart11(pPCMSet);

    printf(" MP3_out_start:---------- finish\n");
}

static int isrFlag;

static int ls=0;
int init_flag=0;
int pinpanflag=0;
int pcm_offset=0;

void MMF_MP3_ISR(int half)
{
    isrFlag=half;
}

int  MP3_feed_buffer(unsigned char * pData, unsigned long len)
{
    int pcmbuf_remain;
    int feed_num;
    int16 *pFeed;

    int i;

    feed_num=(len+1)/2;
    pFeed=(int16 *)pData;

//  pFeedData= (isrFlag == PCM_FIRST_HALF ) ? pcm_play_buffer :(pcm_play_buffer + PCM_PLAY_SIZE/2);

    if( ! init_flag)
    {

        printf(" MP3_feed_buffer: init_flag\n");

        pcmbuf_remain = PCM_PLAY_SIZE -pcm_offset;

        if(feed_num <= pcmbuf_remain)
        {
            for( i=0; i<(len/2); i++)
            {
                pcm_play_buffer[pcm_offset]=(int32)(*pFeed);
                pFeed++;
                pcm_offset++;
//              pcm_play_buffer[pcm_offset]= pData[2*i+1] << 8 | pData[2*i] ;
            }
            if (len%2)
            {
                pcm_play_buffer[pcm_offset]=(int32)pData[len-1];
                pcm_offset++;
            }
            return 0;
        }
        else
        {
            for( i=0; i<pcmbuf_remain; i++)
            {
                pcm_play_buffer[pcm_offset]=(int32)(*pFeed);
                pFeed++;
                pcm_offset++;
            }
            pcm_offset=0;
            init_flag=1;
            isrFlag=pinpanflag=1;
            feed_num-=pcmbuf_remain;
            MP3_PCMOUT_Start();
            goto waitevent;
        }
    }

    pcmbuf_remain = PCM_PLAY_SIZE/2 -pcm_offset;
    if(feed_num <= pcmbuf_remain)
    {
        printf(" MP3_feed_buffer: after init_flag, pinpan is %d\n", pinpanflag);

        for( i=0; i<(len/2); i++)
        {
            pFeedData[pcm_offset]=(int32)(*pFeed);
            pFeed++;
            pcm_offset++;
        }
        if(len%2)
        {
            pFeedData[pcm_offset]=(int32)pData[len-1];
            pcm_offset++;
        }
        return 0;
    }
    else
    {
        for( i=0; i<pcmbuf_remain; i++)
        {
            pFeedData[pcm_offset]=(int32)(*pFeed);
            pFeed++;
            pcm_offset++;
        }
        feed_num -= pcmbuf_remain;

waitevent:

        do
        {
            printf(" MP3_feed_buffer: looping buffer is %d\n", isrFlag);
            COS_Sleep( 10 );
        }
        while(isrFlag == pinpanflag);

        pinpanflag=isrFlag;

        pcm_offset=0;

        pFeedData= (isrFlag == PCM_FIRST_HALF ) ? pcm_play_buffer :\
                   (pcm_play_buffer + PCM_PLAY_SIZE/2);

        for( i=0; i<(len-2*pcmbuf_remain)/2; i++)
        {
            pFeedData[pcm_offset]=(int32)(*pFeed);
            pFeed++;
            pcm_offset++;
        }
        if(len%2)
        {
            pFeedData[pcm_offset]= (int32) pData[len-1];
            pcm_offset++;
        }

    }

    return 0;
}

void MMF_MP3_FILEPlay()
{
    uint32 cnt;
    int ret=0;
    int k;
    unsigned long outNum, iret;
    MMF_Mp3_OpenDecoder();

    for(k=0; k<10; k++)
    {
        if(k==0)
            iret=MMF_Mp3_DecodeFrame( (pcmDataBuffer)+k*504,504,pcm_play_buffer, 1);
        else
        {
            iret=MMF_Mp3_DecodeFrame( (pcmDataBuffer)+k*504,504,pcm_play_buffer, 0);
        }
        printf(" mmf_mp3_fileplay finished! %d ********************* 0x%x\n",k,iret);
//      TS_OutputText(MMI_TS_ID, " mmf_mp3_fileplay finished! %d ********************* 0x%x\n",k,iret);
    }

    for(k=0; k<10; k++)
    {
        if(k==0)
            iret=MMF_Mp3_DecodeFrame( (pcmDataBuffer)+k*504,504,pcm_play_buffer, 1);
        else
        {
            iret=MMF_Mp3_DecodeFrame( (pcmDataBuffer)+k*504,504,pcm_play_buffer, 0);
        }
        printf(" mmf_mp3_fileplay finished! %d ********************* 0x%x\n",k,iret);
//      TS_OutputText(MMI_TS_ID, " mmf_mp3_fileplay finished! %d ********************* 0x%x\n",k,iret);
    }

    printf(" mmf_mp3_fileplay finished! ************************* 0x%x\n",ret);

}

#endif

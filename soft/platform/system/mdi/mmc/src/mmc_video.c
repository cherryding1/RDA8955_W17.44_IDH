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
#include "cswtype.h"
#include "cs_types.h"
#include "cpu_share.h"
#include "mmc_video.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"
#include "wrappergprot.h"

#include "hal_aif.h"
#include "aud_m.h"
#include "di.h"
#include "mci.h"
#include "protocolevents.h"
#include "mmi_trace.h"
#include "timerevents.h"
#include "sxs_type.h"
#include "sxr_sbx.h"

#include "mcip_debug.h"

//#define MP3_1152_SUPPORT //for short pcm buf

/* video state */
#define VID_IDLE                 0
#define VID_FILE_READY        1
#define VID_PLAY               2
#define VID_PAUSE               3

/*video limit*/
#define MAX_QVGA_WIDTH 320
#define MAX_QVGA_HEIGHT 240
#define MAX_QVGA_FRAME_RATE 10
#define MAX_QCIF_WIDTH 176
#define MAX_QCIF_HEIGHT 144
#define MAX_SCREEN_WIDTH 128
#define MAX_SCREEN_HEIGHT 96
#define MAX_QCIF_FRAME_RATE 15
#define MJPG_FRAME_BUFFER_SIZE   (15*1024) /* jpeg frame buffer for MJPEG */
/*audio parameter*/
#define AmrFrameNum 50  //(for full pingpang buf)
#ifdef MP3_1152_SUPPORT
#define Mp3FrameNum 6  //(for full pingpang buf) 
#else
#define Mp3FrameNum 12  //(for full pingpang buf) 
#endif
#define AMR_FFRAME_SAMPLE 160
#define MP3_FFRAME_SAMPLE_1152 1152
#define MP3_FFRAME_SAMPLE_576 576
#define AUD_RING_BUFFER_LEN (AMR_FFRAME_SAMPLE*2*AmrFrameNum) //for 16bit pcm data ,shenh
#define MP3_FRAME_BUFFER_LEN  1024*12

#define MaxAmrNum 9 // 3 times decode=AmrFrameNum/2

int32 aud_temp_frame_num=0;
int32 aud_read_num=0;
int32 pcm_frame_play_num=0;

avi_struct *avi_p=NULL;
vid_context_struct    *v_context=NULL;
aud_context_struct    *a_context=NULL;
play_control* playctl=NULL ;


extern HAL_AIF_STREAM_T audioStream;
extern AUD_LEVEL_T audio_cfg;
extern AUD_ITF_T    audioItf;

//static volatile uint16 timer_mode=0;
static volatile uint16 videoDecReq=0;
static volatile uint16 videoDecInsert=1;
static volatile uint16 DISPLAY_INTERFACE;

// for voc
#define VOC_STATE_CONT 2300000//total time=200ms (11180 per ms, 96Mhz)
static volatile uint8 VOC_OPEN;
static volatile int32 voc_state=0;// 1=in work 0= no work
static volatile boolean VOC_WAIT;// 1=in wait 0= no wait
static int32       TotalNum=0;
vpp_AudioJpeg_DEC_IN_T DecIn;
vpp_AudioJpeg_DEC_OUT_T  DecStatus;
//

#define VID_ENTER_STATE(_s_) (v_context->state = _s_)
#define VID_AUD_ENTER_STATE(_s_) (v_context->aud_state = _s_)

#define VID_IN_STATE(_s_) (v_context->state == _s_)
#define VID_AUD_IN_STATE(_s_) (v_context->aud_state == _s_)

extern VOID mmc_MemCheck(BOOL bprintinfo);
/******************************************************************
 * FUNCTION
 *    vid_init
 * DESCRIPTION
 *    This function is used to init video module of media task.
 * PARAMETERS
 *
 *
 * RETURNS
 *
 * GLOBALS AFFECTED
 *
 *
 ******************************************************************/
static int32 vid_init(void)
{
    v_context=NULL;
    v_context=(vid_context_struct *)mmc_MemMalloc(sizeof(vid_context_struct));
    if(v_context )
    {
        memset(v_context, 0, sizeof(vid_context_struct));
        v_context->state = VID_IDLE;
    }
    else
        return FALSE;

    a_context=NULL;
    a_context=(aud_context_struct *)mmc_MemMalloc(sizeof(aud_context_struct));
    if( a_context )
        memset(a_context, 0, sizeof(aud_context_struct));
    else
        return FALSE;

    playctl=NULL;
    playctl=(play_control *)mmc_MemMalloc(sizeof(play_control));
    if(  playctl )
        memset(playctl, 0, sizeof(play_control));
    else
        return FALSE;

    return TRUE ;
}

void vid_mem_free(void)
{
#if 0
    if(v_context)
    {
        memset(v_context, 0, sizeof(vid_context_struct));
        v_context = NULL;
    }
    if(a_context)
    {
        memset(a_context, 0, sizeof(aud_context_struct));
        a_context = NULL;
    }
    if(playctl)
    {
        memset(playctl, 0, sizeof(play_control));
        playctl = NULL;
    }
    /*
    if(image_para)
        {
        memset(image_para, 0, sizeof(image_para_t));
        image_para = NULL;
        }
        */
    if(pcm_para)
    {
        memset(pcm_para, 0, sizeof(HAL_AIF_STREAM_T));
        pcm_para = NULL;
    }
    mmc_MemFreeAll();
#endif
}

uint16 vid_mp4_initBuffer(void)
{
    uint32 ring_buf_size;
    uint32 aud_buf_size;

    if(v_context->a_fmt==0xfe)
    {
        ring_buf_size=AUD_RING_BUFFER_LEN;
        aud_buf_size=6+v_context->aud_frame_size*AmrFrameNum+2;
    }
    else if(v_context->a_fmt==0x55)
    {
        if(v_context->a_chans==2)
            ring_buf_size=v_context->aud_frame_sample*Mp3FrameNum*2;
        else//avoid fake 1 channel but real 2 channel, decode buffer overflow
            ring_buf_size=v_context->aud_frame_sample*Mp3FrameNum*2*2;
        aud_buf_size=MP3_FRAME_BUFFER_LEN;
    }
    else
        return MED_RES_BAD_FORMAT;

    if(ring_buf_size>mmc_MemGetFree()-32)
        return MED_RES_MEM_INSUFFICIENT;

    a_context->ring_buf=NULL;
    a_context->ring_buf=(int32 *)mmc_MemMalloc(ring_buf_size);

    if(a_context->ring_buf)
        memset(a_context->ring_buf, 0, ring_buf_size);
    else
    {
        mmi_trace(TRUE,  "[MMC_VID]##WARNING##  malloc error\n");
        return MED_RES_MEM_INSUFFICIENT;
    }

    if(aud_buf_size>mmc_MemGetFree()-32)
        return MED_RES_MEM_INSUFFICIENT;

    a_context->aud_buf=NULL;
    a_context->aud_buf=(uint8 *)mmc_MemMalloc(aud_buf_size);

    if(a_context->aud_buf)
        memset(a_context->aud_buf, 0, aud_buf_size);
    else
    {
        mmi_trace(TRUE,  "[MMC_VID]##WARNING##  malloc error\n");
        return MED_RES_MEM_INSUFFICIENT;
    }


    v_context->decode_buffer_size=(v_context->display_width)*(v_context->display_height)*2;

    if(v_context->decode_buffer_size>mmc_MemGetFree()-32)
        return MED_RES_MEM_INSUFFICIENT;

    v_context->decode_buffer_address=NULL;
    v_context->decode_buffer_address=(uint8 *)mmc_MemMalloc(v_context->decode_buffer_size);

    if(v_context->decode_buffer_address)
        memset(v_context->decode_buffer_address, 0, v_context->decode_buffer_size);
    else
    {
        mmi_trace(TRUE,  "[MMC_VID]##WARNING##  malloc error\n");
        return MED_RES_MEM_INSUFFICIENT;
    }

    if((v_context->image_width*v_context->image_height)<=(MAX_QCIF_WIDTH*MAX_QCIF_HEIGHT))
    {
        v_context->file_size=MJPG_FRAME_BUFFER_SIZE;
    }
    else
    {
        v_context->file_size=(MJPG_FRAME_BUFFER_SIZE<<1);
    }

    if(v_context->file_size*2>mmc_MemGetFree()-32)
        return MED_RES_MEM_INSUFFICIENT;

    v_context->file_buffer_address=NULL;
    v_context->file_buffer_address=(uint8 *)mmc_MemMalloc(v_context->file_size*2);//pingpong buf

    if(v_context->file_buffer_address)
        memset(v_context->file_buffer_address, 0, v_context->file_size*2);
    else
    {
        mmi_trace(TRUE,  "[MMC_VID]##WARNING##  malloc error\n");
        return MED_RES_MEM_INSUFFICIENT;
    }

    mmc_MemCheck(TRUE);
    return MED_RES_OK;
}

void vid_end()
{
    pcm_frame_play_num=0;
    v_context->current_frame_num=0;
    v_context->display_frame_num=0;
    v_context->current_aud_frame_num=0;
}


int32 vid_get_res_from_avi_cause(int32 cause)
{
    switch(cause)
    {
        case AVI_ERR_SIZELIM:
            return MED_RES_VIDEO_FILE_TOO_LARGE;
        case AVI_ERR_OPEN:
        case AVI_ERR_READ:
        case AVI_ERR_WRITE:
        case AVI_ERR_WRITE_INDEX:
        case AVI_ERR_CLOSE:
        case AVI_ERR_NOT_PERM:
            return MED_RES_OPEN_FILE_FAIL;
        case AVI_ERR_NO_MEM:
            return MED_RES_MEM_INSUFFICIENT;
        case AVI_ERR_NO_AVI:
        case AVI_ERR_NO_HDRL:
        case AVI_ERR_NO_MOVI:
        case AVI_ERR_NO_VIDS:
        case AVI_ERR_NO_IDX:
        case AVI_ERR_FORTMAT:
            return MED_RES_BAD_FORMAT;
        default:
            return MED_RES_ERROR;
    }
}





/*************************************************************************
* FUNCTION
*   vid_open_avi_play_file
*
* DESCRIPTION
*   This function is to open image file for recording.
*
* PARAMETERS
*   char* file_name
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 vid_open_avi_play_file(HANDLE fhd)
{

    avi_p=avi_open_input_file(fhd, 1);
    if(avi_p==0)
    {
        mmi_trace(TRUE, "[MMC_VID]##WARNING## vid_open_avi_play_file error %d \n",avi_get_error() );
        return vid_get_res_from_avi_cause(avi_get_error());
    }

    return MED_RES_OK;
}

/*************************************************************************
* FUNCTION
*   vid_close_avi_play_file
*
* DESCRIPTION
*   This function is to close the video clip file.
*
* PARAMETERS
*   void
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 vid_close_avi_play_file(void)
{
    int32  res=MED_RES_OK;

    if(avi_p)
        avi_close_input_file(avi_p);
    return res;

}

/*************************************************************************
* FUNCTION
*   vid_measure_update_period
*
* DESCRIPTION
*   This function is to measure the update period for playback
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
int vid_measure_update_period(void)
{
    if(!avi_p->fps)return -1;
    v_context->period=(uint32)((10000/avi_p->fps+5)/10);

    //v_context->period_fraction=(v_context->period+2)/5;
    v_context->period_fraction=16384/avi_p->fps;
    return 0;
}

#if 0
void timer_isr_process(HANDLE HndTimer, uint32 data)
{
    timer_mode=1;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
}
#endif


void pcmHalf_isr_process ()
{
    int32 adjast_time;
    int32 disNum;

    playctl->fist_buffer_flag=0;

    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);

#if 1
    if(v_context->a_fmt==0xfe)
    {
        pcm_frame_play_num+=(AmrFrameNum>>1);
        adjast_time=pcm_frame_play_num*v_context->aud_frame_period;
        if(adjast_time>=(v_context->current_time+500))
        {
            disNum=(adjast_time*v_context->frame_rate+500)/1000;
            if(disNum!=v_context->display_frame_num)
            {
                v_context->current_frame_num=disNum+1;
                mmi_trace(TRUE, "V=A:%d -->%d\n",v_context->display_frame_num,disNum);
            }
            v_context->current_time=adjast_time;
        }
    }
    else if(v_context->a_fmt==0x55)
    {
        pcm_frame_play_num+=(Mp3FrameNum>>1);
        adjast_time=pcm_frame_play_num*v_context->aud_frame_period;
        if(adjast_time>=(v_context->current_time+500))
        {
            disNum=(adjast_time*v_context->frame_rate+500)/1000;
            if(disNum!=v_context->display_frame_num)
            {
                v_context->current_frame_num=disNum+1;
                mmi_trace(TRUE, "V=A:%d -->%d\n",v_context->display_frame_num,disNum);
            }
            v_context->current_time=adjast_time;
        }
    }
#endif

}

void pcmEnd_isr_process ()
{
    int32 adjast_time;
    int32 disNum;

    playctl->fist_buffer_flag=1;
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);

#if 1
    if(v_context->a_fmt==0xfe)
    {
        pcm_frame_play_num+=(AmrFrameNum>>1);
        adjast_time=pcm_frame_play_num*v_context->aud_frame_period;
        if(adjast_time>=(v_context->current_time+500))
        {
            disNum=(adjast_time*v_context->frame_rate+500)/1000;
            if(disNum!=v_context->display_frame_num)
            {
                v_context->current_frame_num=disNum+1;
                mmi_trace(TRUE, "V=A:%d -->%d\n",v_context->display_frame_num,disNum);
            }
            v_context->current_time=adjast_time;
        }
    }
    else if(v_context->a_fmt==0x55)
    {
        pcm_frame_play_num+=(Mp3FrameNum>>1);
        adjast_time=pcm_frame_play_num*v_context->aud_frame_period;
        if(adjast_time>=(v_context->current_time+500))
        {
            disNum=(adjast_time*v_context->frame_rate+500)/1000;
            if(disNum!=v_context->display_frame_num)
            {
                v_context->current_frame_num=disNum+1;
                mmi_trace(TRUE, "V=A:%d -->%d\n",v_context->display_frame_num,disNum);
            }
            v_context->current_time=adjast_time;
        }
    }
#endif
}

//voc isr
void voc_isr_process(HAL_VOC_IRQ_STATUS_T * status )
{
    if( DecIn.mode==MJPEG_DECODE)
    {

        voc_state=0;
        if(VOC_WAIT)return;

        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_DISPLAY_REQ);
    }
    else if(DecIn.mode==AMR_DECODE)
    {

        if(TotalNum>1)
        {
            aud_temp_frame_num+=1;
            a_context->pcm_temp_buf=(int8*)(((uint16*)a_context->ring_buf)+playctl->fist_buffer_flag*(v_context->aud_frame_sample*AmrFrameNum>>1)+aud_temp_frame_num*v_context->aud_frame_sample);
            voc_state=0;
            if(aud_temp_frame_num==MaxAmrNum)
            {

                if(videoDecReq)
                {
                    videoDecReq=0;
                    videoDecInsert=1;

                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                }

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
            }
            else if(aud_temp_frame_num==(MaxAmrNum*2))
            {
                if(videoDecReq)
                {
                    videoDecReq=0;
                    videoDecInsert=1;

                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                }

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
            }
            else if(aud_temp_frame_num==(AmrFrameNum>>1))
            {
                aud_temp_frame_num=0;
                videoDecInsert=1;
                if(videoDecReq)
                {
                    videoDecReq=0;

                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                }
            }
            else
            {
                if(v_context->current_aud_frame_num<v_context->total_aud_frame_num)
                {
                    DecIn.inStreamBufAddr= (INT32 *)(a_context->aud_buf+aud_temp_frame_num*v_context->aud_frame_size);
                    DecIn.outStreamBufAddr=(INT32*)a_context->pcm_temp_buf;
                    DecIn.EQ_Type=-1;
                    voc_state=1;
                    vpp_AudioJpegDecScheduleOneFrame(&DecIn);
                    v_context->current_aud_frame_num+=1;
                }
                else
                {
                    videoDecInsert=1;
                    aud_temp_frame_num=0;
                }

            }

        }
        else
        {
            aud_temp_frame_num+=1;
            a_context->pcm_temp_buf=(int8*)(((uint16*)a_context->ring_buf)+playctl->fist_buffer_flag*(v_context->aud_frame_sample*AmrFrameNum>>1)+aud_temp_frame_num*v_context->aud_frame_sample);
            voc_state=0;
        }
    }
    else if(DecIn.mode==MP3_DECODE)
    {
        if(TotalNum>1)
        {
            vpp_AudioJpegDecStatus(&DecStatus);
            if(DecStatus.output_len==0)
                memset(a_context->pcm_temp_buf, 0, (v_context->aud_frame_sample<<1));

            aud_temp_frame_num+=1;
            a_context->pcm_temp_buf=(int8*)(((uint16*)a_context->ring_buf)+playctl->fist_buffer_flag*(v_context->aud_frame_sample*Mp3FrameNum>>1)+aud_temp_frame_num*v_context->aud_frame_sample);
            voc_state=0;

#ifdef MP3_1152_SUPPORT
            if( aud_temp_frame_num==(Mp3FrameNum>>1))
#else
            if(aud_temp_frame_num==(Mp3FrameNum>>2))
            {
                a_context->p_aud_buf+=DecStatus.consumedLen;
                if(videoDecReq)
                {

                    videoDecReq=0;
                    videoDecInsert=1;
                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                }

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
            }
            else if( aud_temp_frame_num==(Mp3FrameNum>>1))
#endif
            {
                a_context->p_aud_buf+=DecStatus.consumedLen;
                aud_temp_frame_num=0;
                videoDecInsert=1;

                if(videoDecReq)
                {
                    videoDecReq=0;

                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_IMG_DEC_REQ);
                }
            }
            else
            {
                if((pcm_frame_play_num+(Mp3FrameNum>>1)+aud_temp_frame_num)<v_context->total_aud_frame_num)
                {
                    a_context->p_aud_buf+=DecStatus.consumedLen;
                    if(a_context->p_aud_buf<a_context->aud_buf+a_context->aud_buf_remain_bytes)
                    {
                        DecIn.inStreamBufAddr= (INT32 *)(a_context->p_aud_buf);
                        DecIn.outStreamBufAddr=(INT32*)a_context->pcm_temp_buf;
                        DecIn.EQ_Type=-1;
                        voc_state=1;
                        vpp_AudioJpegDecScheduleOneFrame(&DecIn);
                    }
                    else
                    {
                        videoDecInsert=1;
                        aud_temp_frame_num=0;
                    }
                }
                else
                {
                    videoDecInsert=1;
                    aud_temp_frame_num=0;
                }

            }

        }
        else
        {
            aud_temp_frame_num+=1;
            a_context->pcm_temp_buf=(int8*)(((uint16*)a_context->ring_buf)+playctl->fist_buffer_flag*(v_context->aud_frame_sample*Mp3FrameNum>>1)+aud_temp_frame_num*v_context->aud_frame_sample);
            voc_state=0;
        }
    }
}

/*
将565rgb数据拷贝("嵌入")到LCD Buffer中,以便调用LCD的显示函数进行bitmap的显示.
**/
static void EmbedRgbDataIntoLcdBuffer(
    uint16 LcdFrameBuffer_x, uint16 LcdFrameBuffer_y,
    uint16 bmp_width, uint16 bmp_height,
    uint16 *pbmp, uint16 *pLcdShadowBuffer)
{
    uint16 *p1 = NULL;
    uint16 *p2 = NULL;
    uint16 i;
    uint16 rowBytes;

    if(!pbmp || !pLcdShadowBuffer)
    {
        mmi_trace(TRUE, "EmbedRgbDataIntoLcdBuffer ERR!\n");
        return;
    }

    if(LcdFrameBuffer_x+bmp_width> v_context->lcd_size_x || LcdFrameBuffer_y+bmp_height> v_context->lcd_size_y )
    {
        mmi_trace(TRUE, "EmbedRgbDataIntoLcdBuffer ERR!display size>lcd size\n");
        return;
    }

    p1=pbmp;
    p2 = pLcdShadowBuffer + LcdFrameBuffer_y * v_context->lcd_size_x + LcdFrameBuffer_x;
    rowBytes=bmp_width*2;

    for (i= 0; i < bmp_height; i++)
    {
        memcpy(p2, p1, rowBytes);
        p1+=bmp_width;
        p2+=v_context->lcd_size_x;
    }
}

static int32 vid_display(void)
{
    uint32 lcdbuf;

    lcdbuf=get_lcd_frame_buffer_address();
    EmbedRgbDataIntoLcdBuffer(v_context->lcd_offset_x, v_context->lcd_offset_y,
                              v_context->display_width, v_context->display_height,
                              (uint16*)v_context->decode_buffer_address, (uint16*)lcdbuf);

    if(!DISPLAY_INTERFACE)
    {
        mci_LcdBlockWrite((UINT16*)get_lcd_frame_buffer_address(),v_context->lcd_offset_x, v_context->lcd_offset_y,
                          v_context->lcd_offset_x+v_context->display_width-1, v_context->lcd_offset_y+v_context->display_height-1);
    }
    else
    {
        DISPLAY_INTERFACE=0;
        mci_LcdBlockWrite((UINT16*)get_lcd_frame_buffer_address(),0, 0, v_context->lcd_size_x-1, v_context->lcd_size_y -1);
    }

    playctl->rgb_buf_flag=0;
    v_context->display_frame_num=v_context->current_frame_num;
    v_context->current_frame_num+=1;
    if( v_context->open_audio==0 )
        v_context->current_time=v_context->display_frame_num*v_context->period;
    return MED_RES_OK;
}

int32 vid_display_interface(void)
{
    mci_LcdBlockWrite((UINT16*)get_lcd_frame_buffer_address(),0, 0, v_context->lcd_size_x-1, v_context->lcd_size_y -1);
    return MED_RES_OK;
}


int32 vid_dec_video_frame(void)
{
    int32 key_frame;
    int32 read_len=0;

    if(VID_IN_STATE(VID_PLAY))
    {
        if(v_context->current_frame_num > (v_context->total_frame_num-1) )
        {
            aud_StreamStop(audioItf);
            //StopMyTimer(EV_VIDEO_TIMER_ID);
            COS_KillTimer(GetMCITaskHandle(MBOX_ID_MMC), 0);
            mmi_trace(TRUE, "[MMC_VID] video frame over current_frame_num=%d  \n",v_context->current_frame_num);

            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
            vid_end();
            VID_ENTER_STATE(VID_FILE_READY);
            return MED_RES_END_OF_FILE;
        }

        /*
        if(timer_mode==1)
        {
            timer_mode=0;
            StopMyTimer(EV_VIDEO_TIMER_ID);
            StartMyTimer(EV_VIDEO_TIMER_ID,v_context->period_fraction,(oslTimerFuncPtr)timer_isr_process);
        }
        */

        if(playctl->rgb_buf_flag==0)
        {
            if(videoDecInsert==0)
            {
                videoDecReq=1;
                return MED_RES_OK;
            }

            DecIn.inStreamBufAddr=(INT32 *)(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size);
            DecIn.outStreamBufAddr= (INT32 *)v_context->decode_buffer_address;
            DecIn.mode=MJPEG_DECODE;
            DecIn.reset=0;
            DecIn.zoom_mode=ORIGINAL;
            voc_state=1;
            vpp_AudioJpegDecScheduleOneFrame(&DecIn);
            playctl->rgb_buf_flag=1;
        }
        else
        {
            mmi_trace(TRUE, "[MMC_VID]##WARNING## lose one frame !! \n");
        }

        if(playctl->jpeg_buf_flag==0)
            playctl->jpeg_buf_flag=1;
        else
            playctl->jpeg_buf_flag=0;
        if((v_context->current_frame_num +1)< v_context->total_frame_num )
        {
            avi_set_video_position(avi_p, (uint32)(v_context->current_frame_num+1));

            /* read the frame */
            if((read_len = avi_read_frame(avi_p, (int8*)(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size), v_context->file_size, &key_frame))<=0)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## jpeg err read  read_len=%d\n",read_len);
                if(playctl->jpeg_buf_flag==0)
                    playctl->jpeg_buf_flag=1;
                else
                    playctl->jpeg_buf_flag=0;
            }

        }

        return MED_RES_OK;
    }
    else if(VID_IN_STATE(VID_PAUSE))
    {
        return MED_RES_OK;
    }
    else
        return MED_RES_FAIL;
}



int32 vid_dec_audio_frame(void)
{
    int32 result;
    uint32 i;

    if(VID_IN_STATE(VID_PLAY))
    {
        if( v_context->total_aud_frame_num<=(pcm_frame_play_num))
        {
            aud_StreamStop(audioItf);
            mmi_trace(TRUE, "[MMC_VID] pcm over, pcm_frame_play_num=%d\n",pcm_frame_play_num);
            pcm_frame_play_num=0;
            v_context->current_aud_frame_num=0;
            videoDecInsert=1;
            return MED_RES_OK;
        }

        if(v_context->a_fmt==0xfe)
        {

            if(v_context->current_aud_frame_num>=v_context->total_aud_frame_num)
            {
                videoDecInsert=1;
                mmi_trace(TRUE, "[MMC_VID] total aud dec over, current_aud_frame_num=%d\n",v_context->current_aud_frame_num);
                return MED_RES_OK;
            }

            i=0;
            while(voc_state )
            {
                i++;
                if(i>=VOC_STATE_CONT)
                {
                    mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                    return MED_RES_OK;
                }
            }

            if((aud_temp_frame_num==0) ||(aud_temp_frame_num==MaxAmrNum) ||(aud_temp_frame_num==(MaxAmrNum*2)))
                TotalNum =aud_read_num;

            a_context->pcm_temp_buf=(int8*)(((uint16*)a_context->ring_buf)+playctl->fist_buffer_flag*(v_context->aud_frame_sample*AmrFrameNum>>1)+aud_temp_frame_num*v_context->aud_frame_sample);
            DecIn.inStreamBufAddr= (INT32 *)(a_context->aud_buf+aud_temp_frame_num*v_context->aud_frame_size);
            DecIn.outStreamBufAddr=(INT32 *)(a_context->pcm_temp_buf);
            DecIn.mode=AMR_DECODE;
            DecIn.reset=0;
            DecIn.EQ_Type=-1;

            voc_state=1;
            videoDecInsert=0;
            vpp_AudioJpegDecScheduleOneFrame(&DecIn);
            v_context->current_aud_frame_num+=1;


            if(aud_temp_frame_num==0)
            {
                if((v_context->current_aud_frame_num+MaxAmrNum-1)>=v_context->total_aud_frame_num)
                    return MED_RES_OK;

                if((result=avi_read_audio(avi_p, (int8*)(a_context->aud_buf+MaxAmrNum*v_context->aud_frame_size), MaxAmrNum*v_context->aud_frame_size))<=0)
                {
                    mmi_trace(TRUE, "[MMC_VID]##WARNING## read audio  err read_len=%d  \n", result);
                }
                aud_read_num=result/v_context->aud_frame_size;

            }
            else if(aud_temp_frame_num==MaxAmrNum)
            {
                if((v_context->current_aud_frame_num+MaxAmrNum-1)>=v_context->total_aud_frame_num)
                    return MED_RES_OK;
                if((result=avi_read_audio(avi_p, (int8*)(a_context->aud_buf+MaxAmrNum*2*v_context->aud_frame_size), ((AmrFrameNum>>1)-MaxAmrNum*2)*v_context->aud_frame_size))<=0)
                {
                    mmi_trace(TRUE, "[MMC_VID]##WARNING## read audio  err read_len=%d  \n", result);
                }
                aud_read_num=result/v_context->aud_frame_size;
            }
            else if(aud_temp_frame_num==(MaxAmrNum*2))
            {
                if((v_context->current_aud_frame_num+((AmrFrameNum>>1)-MaxAmrNum*2)-1)>=v_context->total_aud_frame_num)
                    return MED_RES_OK;
                if((result=avi_read_audio(avi_p, (int8*)a_context->aud_buf, MaxAmrNum*v_context->aud_frame_size))<=0)
                {
                    mmi_trace(TRUE, "[MMC_VID]##WARNING## read audio  err read_len=%d  \n", result);
                }
                aud_read_num=result/v_context->aud_frame_size;
            }
        }
        else if(v_context->a_fmt==0x55)
        {
            a_context->aud_buf_remain_bytes-=a_context->p_aud_buf-a_context->aud_buf;
            if(a_context->p_aud_buf>a_context->aud_buf )
            {
                if(a_context->aud_buf_remain_bytes>0)
                {
                    memcpy(a_context->aud_buf, a_context->p_aud_buf, a_context->aud_buf_remain_bytes);
                }
                else
                    a_context->aud_buf_remain_bytes=0;
            }
            a_context->p_aud_buf=a_context->aud_buf;

            if(v_context->current_aud_frame_num>=v_context->total_aud_frame_num)
            {
                memset(a_context->aud_buf+a_context->aud_buf_remain_bytes, 0, MP3_FRAME_BUFFER_LEN-a_context->aud_buf_remain_bytes);
                if((pcm_frame_play_num+(Mp3FrameNum>>1)+aud_temp_frame_num)>=v_context->total_aud_frame_num)
                {
                    videoDecInsert=1;
                    mmi_trace(TRUE, "[MMC_VID] total audio decode over !output_len=%d \n", DecStatus.output_len);
                    return MED_RES_OK;
                }
            }
            DecStatus.consumedLen=0;
            DecStatus.output_len=0;

            i=0;
            while(voc_state )
            {
                i++;
                if(i>=VOC_STATE_CONT)
                {
                    mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                    return MED_RES_OK;
                }
            }
#ifdef MP3_1152_SUPPORT
            TotalNum =(Mp3FrameNum>>1);
#else
            TotalNum =(Mp3FrameNum>>2);
#endif

            a_context->pcm_temp_buf=(int8*)(((uint16*)a_context->ring_buf)+playctl->fist_buffer_flag*(v_context->aud_frame_sample*Mp3FrameNum>>1)+aud_temp_frame_num*v_context->aud_frame_sample);
            DecIn.inStreamBufAddr=(INT32 *)(a_context->p_aud_buf);
            DecIn.outStreamBufAddr=(INT32 *)(a_context->pcm_temp_buf);
            DecIn.mode=MP3_DECODE;
            DecIn.reset=0;
            DecIn.EQ_Type=-1;
            voc_state=1;
            videoDecInsert=0;
            vpp_AudioJpegDecScheduleOneFrame(&DecIn);

            while((a_context->aud_buf_remain_bytes+v_context->aud_frame_size)<=MP3_FRAME_BUFFER_LEN)
            {
                if(v_context->current_aud_frame_num>=v_context->total_aud_frame_num)
                {
                    memset(a_context->aud_buf+a_context->aud_buf_remain_bytes, 0, MP3_FRAME_BUFFER_LEN-a_context->aud_buf_remain_bytes);
                    mmi_trace(TRUE, "[MMC_VID] total audio data read over !\n");
                    return MED_RES_OK;
                }

                if((result=avi_read_audio(avi_p, (int8*)a_context->aud_buf+a_context->aud_buf_remain_bytes, v_context->aud_frame_size))!=v_context->aud_frame_size)
                {
                    if(result>0)
                        a_context->aud_buf_remain_bytes+=result;
                    v_context->current_aud_frame_num+=1;
                    mmi_trace(TRUE,  "[MMC_VID]##WARNING## err audio read_size =  %d\n", result);
                    break;
                }
                a_context->aud_buf_remain_bytes+=v_context->aud_frame_size;
                v_context->current_aud_frame_num+=1;
            }

        }
        return MED_RES_OK;
    }
    else if(VID_IN_STATE(VID_PAUSE))
    {
        v_context->a_Pause_DecReq=1;
        return MED_RES_OK;
    }
    else
        return MED_RES_FAIL;
}


/*************************************************************************
* FUNCTION
*   mmc_vid_open_file_req_hdlr
*
* DESCRIPTION
*   This function is to handle open file request
*
* PARAMETERS
*  mp4_open_struct
*
* RETURNS
*   MED_RES_OK
*
* GLOBALS AFFECTED
*
*************************************************************************/

int32 mmc_vid_open_file_req_hdlr(mp4_open_struct *pMp4Open)
{

    uint32 result;

    if(vid_init()!=TRUE)
    {
        result= MED_RES_MEM_INSUFFICIENT;
        goto open_error;
    }
    if( VID_IN_STATE(VID_IDLE))
    {
        v_context->open_audio=pMp4Open->open_audio;

        if((result=vid_open_avi_play_file( pMp4Open->file_handle))==MED_RES_OK)
        {
            if(vid_measure_update_period()!=0)
            {
                result= MED_RES_PARAM_ERROR;
                goto open_error;
            }
            //for audio
            v_context->a_fmt=avi_p->a_fmt;
            v_context->a_chans=avi_p->a_chans;
            v_context->a_sample=avi_p->a_rate;
            v_context->a_bits=avi_p->a_bits;
            v_context->a_bitrate=avi_p->mp3rate;
            v_context->audio_bytes=avi_p->audio_bytes;
            //
            v_context->total_frame_num=avi_p->video_frames;
            v_context->frame_rate=avi_p->fps;
            v_context->total_time=avi_p->video_frames*v_context->period;
            v_context->image_width=avi_p->width;
            v_context->image_height=avi_p->height;
            v_context->display_width=v_context->image_width;
            v_context->display_height=v_context->image_height;
            v_context->lcd_size_x=MMC_LcdWidth;
            v_context->lcd_size_y=MMC_LcdHeight;
            //lcd_get_size(MAIN_LCD, v_context->lcd_size_x, v_context->lcd_size_y);

            mmi_trace(TRUE, "[MMC_VID] total_frame_num %d \n",v_context->total_frame_num );
            mmi_trace(TRUE, "[MMC_VID] image_width %d \n",v_context->image_width );
            mmi_trace(TRUE, "[MMC_VID] image_height %d \n",v_context->image_height );
            mmi_trace(TRUE, "[MMC_VID] fps %d \n",v_context->frame_rate );
            mmi_trace(TRUE, "[MMC_VID] period %d \n",v_context->period );
            mmi_trace(TRUE, "[MMC_VID] period_fraction %d \n",v_context->period_fraction);
            mmi_trace(TRUE, "[MMC_VID] a_fmt =0x%x \n",v_context->a_fmt );
            mmi_trace(TRUE, "[MMC_VID] a_chans =%d \n",v_context->a_chans );
            mmi_trace(TRUE, "[MMC_VID] a_sample =%d \n",v_context->a_sample );
            mmi_trace(TRUE, "[MMC_VID] a_bits =%d \n",v_context->a_bits );
            mmi_trace(TRUE, "[MMC_VID] a_bitrate =%d \n",v_context->a_bitrate);
            mmi_trace(TRUE, "[MMC_VID] audio_bytes =%d \n",v_context->audio_bytes);
            mmi_trace(TRUE, "[MMC_VID] lcd_size_x =%d \n",v_context->lcd_size_x);
            mmi_trace(TRUE, "[MMC_VID] lcd_size_y =%d \n",v_context->lcd_size_y);

            if(v_context->image_width>MAX_SCREEN_WIDTH ||v_context->image_height>MAX_SCREEN_HEIGHT)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## image too large \n");
                result= MED_RES_VIDEO_IMAGE_TOO_LARGE;
                goto open_error;
            }

            if(avi_p->fps>MAX_QCIF_FRAME_RATE)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## fps too large \n");
                result= MED_RES_VIDEO_FRAME_RATE_TOO_HIGH;
                goto open_error;
            }

#ifndef MP3_1152_SUPPORT
            if(v_context->a_sample>24000)//for pcm buf too short
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## a_sample too large \n");
                result= MED_RES_INVALID_FORMAT;
                goto open_error;
            }
#endif

            if(v_context->a_fmt==0xfe)
            {
                if(v_context->a_chans!=1)
                {
                    mmi_trace(TRUE, "[MMC_VID]##WARNING## real a_chans =1 \n");
                    v_context->a_chans=1;
                }
                v_context->aud_frame_sample=AMR_FFRAME_SAMPLE*v_context->a_chans;
                v_context->aud_frame_size=32;//AMR_FFRAME_SAMPLE*v_context->a_bitrate/(8*v_context->a_sample);//32bytes
                if(!v_context->a_sample)
                {
                    result= MED_RES_PARAM_ERROR;
                    goto open_error;
                }
                v_context->aud_frame_period=1000*AMR_FFRAME_SAMPLE/v_context->a_sample;//20ms
                if(!v_context->aud_frame_size)
                {
                    result= MED_RES_PARAM_ERROR;
                    goto open_error;
                }
                v_context->total_aud_frame_num=v_context->audio_bytes/v_context->aud_frame_size;
            }
            else if(v_context->a_fmt==0x55)
            {
                if((v_context->a_sample==48000) || (v_context->a_sample==44100) ||(v_context->a_sample==32000))//only for layer3
                {
                    v_context->aud_frame_sample=MP3_FFRAME_SAMPLE_1152*v_context->a_chans;
                    if(!v_context->a_sample)
                    {
                        result= MED_RES_PARAM_ERROR;
                        goto open_error;
                    }
                    v_context->aud_frame_size=MP3_FFRAME_SAMPLE_1152*v_context->a_bitrate/(8*v_context->a_sample);
                    v_context->aud_frame_period=1000*MP3_FFRAME_SAMPLE_1152/v_context->a_sample;
                }
                else
                {
                    v_context->aud_frame_sample=MP3_FFRAME_SAMPLE_576*v_context->a_chans;
                    if(!v_context->a_sample)
                    {
                        result= MED_RES_PARAM_ERROR;
                        goto open_error;
                    }
                    v_context->aud_frame_size=MP3_FFRAME_SAMPLE_576*v_context->a_bitrate/(8*v_context->a_sample);//252bytes
                    v_context->aud_frame_period=1000*MP3_FFRAME_SAMPLE_576/v_context->a_sample;//36ms 72ms
                }
                v_context->total_aud_frame_num=v_context->audio_bytes/v_context->aud_frame_size;
            }
            else
            {
                result= MED_RES_BAD_FORMAT;
                goto open_error;
            }

            mmi_trace(TRUE, "[MMC_VID] total_aud_frame_num =%d \n",v_context->total_aud_frame_num);
            mmi_trace(TRUE, "[MMC_VID] aud_frame_sample =%d \n",v_context->aud_frame_sample);
            mmi_trace(TRUE, "[MMC_VID] aud_frame_size =%d \n",v_context->aud_frame_size);
            mmi_trace(TRUE, "[MMC_VID] aud_frame_period =%d \n",v_context->aud_frame_period);

            if(vid_mp4_initBuffer()!=MED_RES_OK)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## vid_mp4_initBuffer malloc err!!!\n");
                result= MED_RES_MEM_INSUFFICIENT;
                goto open_error;
            }

            /*open voc*/
            VOC_OPEN=0;
            result=vpp_AudioJpegDecOpen(voc_isr_process);
            if(result!=HAL_ERR_NO)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## vpp_JpegAmrDecOpen err result=%d \n" ,result);

                result= MED_RES_FAIL;
                goto open_error;
            }
            VOC_OPEN=1;

            a_context->a_outPutPath=pMp4Open->OutputPath;

            v_context->current_frame_num=0;
            v_context->current_aud_frame_num=0;

            DISPLAY_INTERFACE=0;

            VID_ENTER_STATE(VID_FILE_READY);
        }
        else
        {
            mmi_trace(TRUE, "[MMC_VID] open file fail!  \n");
            result= MED_RES_OPEN_FILE_FAIL;
            goto open_error;
        }
    }
    else
    {
        result= MED_RES_BUSY;
        goto open_error;
    }
    mmi_trace(TRUE, "[MMC_VID] mmc_vid_open_file_req_hdlr ok!  \n");
    return MED_RES_OK;

open_error:

    mmi_trace(TRUE, "[MMC_VID_ISOM] ##WARNING## mmc_vid_open_file_req_hdlr err res=%d\n",result);
    return result;
}

/*************************************************************************
* FUNCTION
*   mmc_vid_seek_req_hdlr
*
* DESCRIPTION
*   This function is to handle video seek request
*
* PARAMETERS
*  mp4_seek_struct
*
* RETURNS
*   MED_RES_OK
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 mmc_vid_seek_req_hdlr(mp4_seek_struct *pSeek)
{
    uint32 result;

    if(VID_IN_STATE(VID_FILE_READY) || VID_IN_STATE(VID_PAUSE) )
    {
        v_context->lcd_offset_x=pSeek->lcd_start_x;
        v_context->lcd_offset_y=pSeek->lcd_start_y;
        if(((v_context->lcd_offset_x+v_context->display_width)>v_context->lcd_size_x)||
                ((v_context->lcd_offset_y+v_context->display_height)>v_context->lcd_size_y))
        {
            mmi_trace(TRUE, "[MMC_VID]##WARNING## offset_x=%d offset_y=%d \n", v_context->lcd_offset_x,v_context->lcd_offset_y);
            v_context->lcd_offset_x=0;
            v_context->lcd_offset_y=0;
            return MED_RES_PARAM_ERROR;
        }

        if(pSeek->time_mode==1)
        {
            v_context->current_time+=pSeek->time;
        }
        else //default
        {
            v_context->current_time=pSeek->time;
        }

        if(v_context->current_time<0)
        {
            v_context->current_time=0;
        }
        else if((uint32)v_context->current_time>v_context->total_time)
        {
            v_context->current_time=(int32)v_context->total_time;
        }
        v_context->current_frame_num=(int32)(v_context->current_time*v_context->frame_rate/1000);

        /* limit to the length of video clip */
        if(v_context->current_frame_num>=v_context->total_frame_num)
            v_context->current_frame_num=v_context->total_frame_num-1;
        mmi_trace(TRUE, "[MMC_VID]  seek current_frame_num=%d lcd_offset_x =%d y =%d\n",v_context->current_frame_num,
                  v_context->lcd_offset_x,v_context->lcd_offset_y);
        result=avi_set_video_position(avi_p, (uint32)v_context->current_frame_num);
        if(pSeek->get_frame)
        {
            int32 key_frame;
            uint32 i;

            if(playctl->rgb_buf_flag==1)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING##display busy!\n");
                return MED_RES_OK;
            }

            if((result=avi_read_frame(avi_p, (int8*)(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size), v_context->file_size,  &key_frame))<=0)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## frame read err result=%d \n", result );
                return MED_RES_ERROR;
            }

            result=0;
            result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+159);
            result<<=8;
            result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+160);
            if(result!=v_context->image_height)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## avi file image_height error! %d \n", result );
                return MED_RES_BAD_FORMAT;
            }
            result=0;
            result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+161);
            result<<=8;
            result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+162);
            if(result!=v_context->image_width)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## avi file image_width error! %d \n", result );
                return MED_RES_BAD_FORMAT;
            }

            i=0;
            while(voc_state )
            {
                i++;
                if(i>=VOC_STATE_CONT)
                {
                    mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                    return MED_RES_OK;
                }
            }
            DecIn.inStreamBufAddr=(INT32*)(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size);
            DecIn.outStreamBufAddr= (INT32 *)v_context->decode_buffer_address;
            DecIn.mode=MJPEG_DECODE;
            DecIn.reset=0;
            DecIn.zoom_mode=ORIGINAL;
            voc_state=1;
            VOC_WAIT=1;
            vpp_AudioJpegDecScheduleOneFrame(&DecIn);
            playctl->rgb_buf_flag=1;

            if(playctl->jpeg_buf_flag==0)
                playctl->jpeg_buf_flag=1;
            else
                playctl->jpeg_buf_flag=0;

            i=0;
            while(voc_state )
            {
                i++;
                if(i>=VOC_STATE_CONT)
                {
                    mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                    VOC_WAIT=0;
                    return MED_RES_OK;
                }
            }
            VOC_WAIT=0;
            vid_display();
        }
        return MED_RES_OK;
    }
    else
    {
        mmi_trace(TRUE, "[MMC_VID]##WARNING## seek MED_RES_BUSY\n");
        return MED_RES_BUSY;
    }
}

/*************************************************************************
* FUNCTION
*   mmc_vid_play_req_hdlr
*
* DESCRIPTION
*   This function is to handle video play request
*
* PARAMETERS
*   void
*
* RETURNS
*   MED_RES_OK
*
* GLOBALS AFFECTED
*
*************************************************************************/

int32 mmc_vid_play_req_hdlr(uint16 lcd_start_x, uint16 lcd_start_y)
{
    int32 key_frame;
    int read_size;
    int i,j;
    int32 result;

    if(VID_IN_STATE(VID_FILE_READY) || VID_IN_STATE(VID_PAUSE))
    {
        v_context->lcd_offset_x=lcd_start_x;
        v_context->lcd_offset_y=lcd_start_y;
        if(((v_context->lcd_offset_x+v_context->display_width)>v_context->lcd_size_x)||
                ((v_context->lcd_offset_y+v_context->display_height)>v_context->lcd_size_y))
        {
            mmi_trace(TRUE, "[MMC_VID]##WARNING## offset_x=%d offset_y=%d \n", v_context->lcd_offset_x,v_context->lcd_offset_y);
            v_context->lcd_offset_x=0;
            v_context->lcd_offset_y=0;

            result=MED_RES_PARAM_ERROR;
            goto play_error;
        }

        if(v_context->a_fmt==0xfe)
        {
            v_context->current_aud_frame_num=v_context->current_frame_num*v_context->period/v_context->aud_frame_period;
            avi_set_audio_position(avi_p, v_context->current_aud_frame_num * v_context->aud_frame_size);

            if((read_size=avi_read_audio(avi_p, (int8*)a_context->aud_buf, 6+v_context->aud_frame_size*AmrFrameNum+2))<=0)
            {
                mmi_trace(TRUE,  "[MMC_VID]##WARNING## audio read_size =  %d\n", read_size);
            }

            if((memcmp(a_context->aud_buf,"#!AMR\n",6)==0) && (*(a_context->aud_buf+6)==0))
                v_context->play_audio=0;
            else
            {
                i=0;
                v_context->play_audio=1;
                while(*(a_context->aud_buf+i)==0)
                {
                    i+=1;
                    if(i>=7)
                    {
                        v_context->play_audio=0;
                        break;
                    }
                }
                i=0;
            }

            if(playctl->rgb_buf_flag!=0)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## voc dec jpeg no return  \n");
            }

            i=0;
            while(voc_state )
            {
                i++;
                if(i>=VOC_STATE_CONT)
                {
                    mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                    result=MED_RES_FAIL;
                    goto play_error;
                }
            }

            DecIn.reset=1;
            DecIn.EQ_Type=-1;
            aud_temp_frame_num=0;
            playctl->fist_buffer_flag=0;
            a_context->pcm_temp_buf=(int8*)a_context->ring_buf;
            for(i=0; i<AmrFrameNum; i++)
            {
                TotalNum =1;
                if(i==0)
                {
                    DecIn.inStreamBufAddr=(INT32 *)a_context->aud_buf;
                }
                else
                    DecIn.inStreamBufAddr=(INT32 *)(a_context->aud_buf+6+v_context->aud_frame_size*i+2);
                DecIn.outStreamBufAddr=(INT32 *)a_context->pcm_temp_buf;
                DecIn.mode=AMR_DECODE;
                voc_state=1;
                vpp_AudioJpegDecScheduleOneFrame(&DecIn);
                j=0;
                while(voc_state )
                {
                    j++;
                    if(j>=VOC_STATE_CONT)
                    {
                        mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                        result=MED_RES_FAIL;
                        goto play_error;
                    }
                }
                DecIn.reset=0;
            }
            aud_temp_frame_num=0;

            pcm_frame_play_num=0;
            if(v_context->current_aud_frame_num!=0)
            {
                pcm_frame_play_num=v_context->current_aud_frame_num+1;
            }
            v_context->current_aud_frame_num+=AmrFrameNum;

            //for next aud
            if((result=avi_read_audio(avi_p, (int8*)a_context->aud_buf, MaxAmrNum*v_context->aud_frame_size))<=0)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## read audio  read_len=%d  \n", result);
            }
            aud_read_num=result/v_context->aud_frame_size;

        }
        else if(v_context->a_fmt==0x55)
        {
            if(!v_context->aud_frame_period)
            {
                result=MED_RES_PARAM_ERROR;
                goto play_error;
            }
            v_context->current_aud_frame_num=v_context->current_frame_num*v_context->period/v_context->aud_frame_period;

            pcm_frame_play_num=0;
            if(v_context->current_aud_frame_num!=0)
            {
                pcm_frame_play_num=v_context->current_aud_frame_num+1;
            }

            avi_set_audio_position(avi_p, v_context->current_aud_frame_num * v_context->aud_frame_size);

            a_context->aud_buf_remain_bytes=0;
            while((a_context->aud_buf_remain_bytes+v_context->aud_frame_size)<=MP3_FRAME_BUFFER_LEN)
            {
                if((read_size=avi_read_audio(avi_p, (int8*)a_context->aud_buf+a_context->aud_buf_remain_bytes, v_context->aud_frame_size))!=v_context->aud_frame_size)
                {
                    mmi_trace(TRUE,  "[MMC_VID]##WARNING## audio read_size =  %d\n", read_size);
                    if(read_size>0)
                        a_context->aud_buf_remain_bytes+=read_size;
                    v_context->current_aud_frame_num+=1;
                    break;
                }
                a_context->aud_buf_remain_bytes+=v_context->aud_frame_size;
                v_context->current_aud_frame_num+=1;
            }

            a_context->p_aud_buf=a_context->aud_buf;
            v_context->play_audio=1;

            if(playctl->rgb_buf_flag!=0)
            {
                mmi_trace(TRUE, "[MMC_VID]##WARNING## voc dec jpeg no return  \n");
            }

            j=0;
            while(voc_state )
            {
                j++;
                if(j>=VOC_STATE_CONT)
                {
                    mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                    result=MED_RES_FAIL;
                    goto play_error;
                }
            }
            DecIn.reset=1;
            DecIn.EQ_Type=-1;
            aud_temp_frame_num=0;
            playctl->fist_buffer_flag=0;
            a_context->pcm_temp_buf=(int8*)a_context->ring_buf;
            for(i=0; i<Mp3FrameNum; i++)
            {
                if(a_context->aud_buf_remain_bytes<=0)
                    break;

                TotalNum =1;
                DecIn.inStreamBufAddr=(INT32 *)a_context->p_aud_buf;
                DecIn.outStreamBufAddr=(INT32 *)a_context->pcm_temp_buf;
                DecIn.mode=MP3_DECODE;
                voc_state=1;
                vpp_AudioJpegDecScheduleOneFrame(&DecIn);
                j=0;
                while(voc_state )
                {
                    j++;
                    if(j>=VOC_STATE_CONT)
                    {
                        mmi_trace(TRUE, "VOC_WORK_CONT end! \n");
                        result=MED_RES_FAIL;
                        goto play_error;
                    }
                }
                vpp_AudioJpegDecStatus(&DecStatus);
                mmi_trace(TRUE, "ErrorStatus=%d  \n",DecStatus.ErrorStatus);

                a_context->p_aud_buf+=DecStatus.consumedLen;
                a_context->aud_buf_remain_bytes-=DecStatus.consumedLen;

                if(DecStatus.consumedLen>0 )
                {
                    if(a_context->aud_buf_remain_bytes>0)
                        memcpy(a_context->aud_buf, a_context->p_aud_buf, a_context->aud_buf_remain_bytes);
                    else
                        a_context->aud_buf_remain_bytes=0;
                }

                a_context->p_aud_buf=a_context->aud_buf;
                DecStatus.consumedLen=0;
                DecStatus.output_len=0;

                while((a_context->aud_buf_remain_bytes+v_context->aud_frame_size)<=MP3_FRAME_BUFFER_LEN)
                {
                    if((read_size=avi_read_audio(avi_p, (int8*)a_context->aud_buf+a_context->aud_buf_remain_bytes, v_context->aud_frame_size))!=v_context->aud_frame_size)
                    {
                        mmi_trace(TRUE,  "[MMC_VID]##WARNING## audio read_size =  %d\n", read_size);
                        if(read_size>0)
                            a_context->aud_buf_remain_bytes+=read_size;
                        v_context->current_aud_frame_num+=1;
                        break;
                    }
                    a_context->aud_buf_remain_bytes+=v_context->aud_frame_size;
                    v_context->current_aud_frame_num+=1;
                }
                DecIn.reset=0;
            }
            aud_temp_frame_num=0;
        }

        //decode one jpeg
        if((v_context->current_frame_num+1) > (v_context->total_frame_num-1) )
        {
            vid_end();
            mmi_trace(TRUE, "[MMC_VID] video frame over current_frame_num=%d  \n",v_context->current_frame_num);
            result=MED_RES_END_OF_FILE;
            goto play_error;
        }
        avi_set_video_position(avi_p, (uint32)(v_context->current_frame_num+1));

        /* read frame */
        if((result = avi_read_frame(avi_p, (int8*)(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size), v_context->file_size, &key_frame))<=0)
        {
            mmi_trace(TRUE, "[MMC_VID]##WARNING## jpeg read  read_len=%d \n",result);
            result=MED_RES_BAD_FORMAT;
            goto play_error;
        }

        result=0;
        result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+159);
        result<<=8;
        result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+160);
        if(result!=v_context->image_height)
        {
            mmi_trace(TRUE, "[MMC_VID]##WARNING## avi file image_height error! %d \n", result );
            result=MED_RES_BAD_FORMAT;
            goto play_error;
        }
        result=0;
        result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+161);
        result<<=8;
        result|=*(v_context->file_buffer_address+playctl->jpeg_buf_flag*v_context->file_size+162);
        if(result!=v_context->image_width)
        {
            mmi_trace(TRUE, "[MMC_VID]##WARNING## avi file image_width error! %d \n", result );
            result=MED_RES_BAD_FORMAT;
            goto play_error;
        }
        VID_ENTER_STATE(VID_PLAY);

#if 1
        if( v_context->open_audio==1 )
        {
            if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER)
            {
                // Initial cfg
                audioItf = AUD_ITF_EAR_PIECE;
            }
            audioStream.startAddress =(UINT32*) a_context->ring_buf;
            if(v_context->a_fmt==0xfe)
                audioStream.length=AUD_RING_BUFFER_LEN;
            else if(v_context->a_fmt==0x55)
                audioStream.length=v_context->aud_frame_sample*Mp3FrameNum*2;
            audioStream.channelNb = v_context->a_chans;
            audioStream.voiceQuality = FALSE;
            audioStream.playSyncWithRecord = FALSE;
            audioStream.halfHandler = pcmHalf_isr_process;
            audioStream.endHandler = pcmEnd_isr_process;
            switch (v_context->a_sample)
            {
                case 48000:
                    audioStream.sampleRate = HAL_AIF_FREQ_48000HZ;
                    break;
                case 44100:
                    audioStream.sampleRate = HAL_AIF_FREQ_44100HZ;
                    break;
                case 32000:
                    audioStream.sampleRate = HAL_AIF_FREQ_32000HZ;
                    break;
                case 24000:
                    audioStream.sampleRate = HAL_AIF_FREQ_24000HZ;
                    break;
                case 22050:
                    audioStream.sampleRate = HAL_AIF_FREQ_22050HZ;
                    break;
                case 16000:
                    audioStream.sampleRate = HAL_AIF_FREQ_16000HZ;
                    break;
                case 12000:
                    audioStream.sampleRate = HAL_AIF_FREQ_12000HZ;
                    break;
                case 11025:
                    audioStream.sampleRate = HAL_AIF_FREQ_11025HZ;
                    break;
                case 8000:
                    audioStream.sampleRate = HAL_AIF_FREQ_8000HZ;
                    break;
                default:
                    mmi_trace(TRUE, "[MMC_VID]##WARNING## invalid a_sample=%d\n",v_context->a_sample);
                    result= MED_RES_PARAM_ERROR;
                    goto play_error;
            }

            if( v_context->play_audio==1)
            {
                if((result=aud_StreamStart(audioItf, &audioStream, &audio_cfg))!=AUD_ERR_NO)
                {
                    mmi_trace(TRUE, "[MMC_VID]##WARNING## pcm start err result= %d \n",result);
                    //camInPreviewState = FALSE;
                    result=MED_RES_FAIL;
                    goto play_error;
                }
            }
        }
#endif

#if 1 //display video or not 
        //StartMyTimer(EV_VIDEO_TIMER_ID,v_context->period_fraction,(oslTimerFuncPtr)timer_isr_process);
        COS_SetTimer(GetMCITaskHandle(MBOX_ID_MMC), 0, COS_TIMER_MODE_PERIODIC, v_context->period_fraction);

        v_context->current_time=v_context->current_frame_num*v_context->period;
#endif
        mmi_trace(TRUE, "[MMC_VID] mmc_vid_play_req_hdlr \n");
        return MED_RES_OK;
    }
    else
    {
        mmi_trace(TRUE, "[MMC_VID]##WARNING## mmc_vid_play_req_hdlr MED_RES_BUSY\n");
        result=MED_RES_BUSY;
        goto play_error;
    }

play_error:
    mmi_trace(TRUE, "[MMC_VID] ##WARNING## mmc_vid_play_req_hdlr err res= %d \n", result);
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
    return result;
}

/*************************************************************************
* FUNCTION
*   mmc_vid_stop_req_hdlr
*
* DESCRIPTION
*   This function is to handle video stop request
*
* PARAMETERS
*
*
* RETURNS
*   MED_RES_OK
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 mmc_vid_stop_req_hdlr(void)
{
    if(VID_IN_STATE(VID_PLAY) || VID_IN_STATE(VID_FILE_READY) || VID_IN_STATE(VID_PAUSE))
    {
        if( v_context->open_audio==1)
            aud_StreamStop(audioItf);
        //StopMyTimer(EV_VIDEO_TIMER_ID);
        COS_KillTimer(GetMCITaskHandle(MBOX_ID_MMC), 0);
        videoDecInsert=1;
        videoDecReq=0;
        VID_ENTER_STATE(VID_FILE_READY);
        mmi_trace(TRUE, "[MMC_VID] mmc_vid_stop_req_hdlr\n");
    }

    return MED_RES_OK;
}



/*************************************************************************
* FUNCTION
*   mmc_vid_close_file_req_hdlr
*
* DESCRIPTION
*   This function is to handle close file request
*
* PARAMETERS
*
*
* RETURNS
*   MED_RES_OK
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 mmc_vid_close_file_req_hdlr(void)
{
    aud_temp_frame_num=0;
    aud_read_num=0;
    pcm_frame_play_num=0;
    //timer_mode=0;
    videoDecReq=0;
    videoDecInsert=1;
    voc_state=0;
    TotalNum=0;

    if(VOC_OPEN)
    {
        vpp_AudioJpegDecClose();
        VOC_OPEN=0;
    }
    vid_close_avi_play_file();
    mmc_MemFreeAll();
    mmi_trace(TRUE, "[MMC_VID] mmc_vid_close_file_req_hdlr  \n");
    return MED_RES_OK;
}

int32 mmc_vid_pause_req_hdlr (void)
{
    if(VID_IN_STATE(VID_PLAY))
    {
        if( v_context->open_audio==1)
        {
            int32 res;
            if((res=aud_StreamPause(audioItf, TRUE))!=AUD_ERR_NO)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
                mmi_trace(TRUE, "[MMC_VID] aud_StreamPause ERROR res=%d\n", res);
            }
        }

        //StopMyTimer(EV_VIDEO_TIMER_ID);
        COS_KillTimer(GetMCITaskHandle(MBOX_ID_MMC), 0);
        VID_ENTER_STATE(VID_PAUSE);
        mmi_trace(TRUE, "[MMC_VID] mmc_vid_pause_req_hdlr\n");
    }
    else
        return MED_RES_FAIL;
    return MED_RES_OK;
}

int32 mmc_vid_resume_req_hdlr (void)
{
    int32 result;

    if(VID_IN_STATE(VID_PAUSE))
    {
        if(v_context->a_Pause_DecReq==1)
        {
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_AUD_DEC_REQ);
            v_context->a_Pause_DecReq=0;
        }

        //StopMyTimer(EV_VIDEO_TIMER_ID);
        //StartMyTimer(EV_VIDEO_TIMER_ID,v_context->period_fraction,(oslTimerFuncPtr)timer_isr_process);
        COS_SetTimer(GetMCITaskHandle(MBOX_ID_MMC), 0, COS_TIMER_MODE_PERIODIC, v_context->period_fraction);

        if( v_context->open_audio==1)
        {
            if(aud_StreamPause(audioItf, FALSE)!=AUD_ERR_NO)
            {
                result=MED_RES_FAIL;
                goto resume_error;
            }
        }

        VID_ENTER_STATE(VID_PLAY);
        mmi_trace(TRUE, "[MMC_VID] mmc_vid_resume_req_hdlr\n");
    }
    else
    {
        result=MED_RES_BUSY;
        goto resume_error;
    }
    return MED_RES_OK;

resume_error:

    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_STOP_REQ);
    diag_printf("[MMC_VID_ISOM] mmc_vid_resume err res=%d\n", result);
    return result;

}

/*************************************************************************
* FUNCTION
*   mmc_vid_get_current_state
*
* DESCRIPTION
*   This function is to get current time/total_time/image_height/image_width when video playing.
*
* PARAMETERS
*
*  mp4_play_info* pState
*
* RETURNS
*   pState.
*
* GLOBALS AFFECTED
*
*************************************************************************/
int32 mmc_vid_get_info( mp4_play_info* pState)
{
    //mmi_trace(TRUE, "mmc_vid_get_info  \n");
    if(!v_context) return MED_RES_FAIL;
    /*get info*/
    pState->current_time= v_context->current_time;
    pState->total_time=v_context->total_time-500;
    pState->total_frame_num=v_context->total_frame_num;
    pState->image_height=v_context->image_height;
    pState->image_width=v_context->image_width;
    //no using now
    /*
    pState->pada=0;
    pState->padb=0;
    pState->result=0;
    pState->seq_num=0;
    */
    return MED_RES_OK;
}

int32 mmc_vid_set_info(mp4_set_info *pSetInfo)
{
#if 0
    /*set info*/
    v_context->lcd_offset_x=pSetInfo->lcd_start_x;
    v_context->lcd_offset_y=pSetInfo->lcd_start_y;

    if(pSetInfo->zoom_mode==1)
    {
        if(v_context->rotate_on==1)
        {
            if((v_context->image_width==MAX_QCIF_WIDTH)&&(v_context->image_height==MAX_QCIF_HEIGHT))
            {
                if(v_context->frame_rate<=MAX_QVGA_FRAME_RATE)
                {
                    lcd_set_entry_mode(LCD_CTRL_ENTRY_MODE_ID_10, LCD_CTRL_ENTRY_MODE_AM_V);
                    //JpgParam->zoom_mode=ZOOMIN;
                    image_para->image_width =MAX_QVGA_HEIGHT;
                    image_para->image_height =MAX_QVGA_WIDTH;
                    image_para->image_offset_x =0;
                    image_para->image_offset_y =0;
                }
                else
                {
                    //JpgParam->zoom_mode=ORIGINAL;
                    image_para->image_width =v_context->image_width;
                    image_para->image_height =v_context->image_height;
                    image_para->image_offset_x =(MAX_QVGA_HEIGHT-v_context->display_width)/2;
                    image_para->image_offset_y =(MAX_QVGA_WIDTH-v_context->display_height)/2;
                }
            }
            else if((v_context->image_width==MAX_QVGA_WIDTH)&&(v_context->image_height==MAX_QVGA_HEIGHT))
            {
                lcd_set_entry_mode(LCD_CTRL_ENTRY_MODE_ID_10, LCD_CTRL_ENTRY_MODE_AM_V);
                //JpgParam->zoom_mode=ORIGINAL;
                image_para->image_width =MAX_QVGA_HEIGHT;
                image_para->image_height =MAX_QVGA_WIDTH;
                image_para->image_offset_x =0;
                image_para->image_offset_y =0;
            }
            else
            {
                lcd_set_entry_mode(LCD_CTRL_ENTRY_MODE_ID_10, LCD_CTRL_ENTRY_MODE_AM_V);
                //JpgParam->zoom_mode=ORIGINAL;
                image_para->image_width =v_context->image_height;
                image_para->image_height =v_context->image_width;
                image_para->image_offset_x = (MAX_QVGA_HEIGHT-v_context->display_width)/2;
                image_para->image_offset_y = (MAX_QVGA_WIDTH-v_context->display_height)/2;
            }
        }
        else
        {
            //JpgParam->zoom_mode=ORIGINAL;
            image_para->image_width =v_context->image_width;
            image_para->image_height =v_context->image_height;
            image_para->image_offset_x = (MAX_QVGA_HEIGHT-v_context->display_width)/2;
            image_para->image_offset_y = (MAX_QVGA_WIDTH-v_context->display_height)/2;
        }
        v_context->full_size_mode=1;
    }
    else//default mode
    {
        if(v_context->rotate_on==1)
        {
            lcd_set_entry_mode(LCD_CTRL_ENTRY_MODE_ID_DEFAULT, LCD_CTRL_ENTRY_MODE_AM_DEFAULT);
            if((v_context->image_width==MAX_QCIF_WIDTH)&&(v_context->image_height==MAX_QCIF_HEIGHT))
            {
                //JpgParam->zoom_mode=ORIGINAL;
            }
            else if((v_context->image_width==MAX_QVGA_WIDTH)&&(v_context->image_height==MAX_QVGA_HEIGHT))
            {
                //JpgParam->zoom_mode=ZOOMOUT;
            }
            else
                //JpgParam->zoom_mode=ORIGINAL;
                image_para->image_width = v_context->display_width;
            image_para->image_height = v_context->display_height;
        }
        image_para->image_offset_x = v_context->lcd_offset_x;
        image_para->image_offset_y = v_context->lcd_offset_y;
        v_context->full_size_mode=0;
    }
#endif
    return MED_RES_OK;
}


/* video play msg process */
//mp4_open_struct Mp4Play;
//mp4_seek_struct SeekInfo;
//mp4_play_info  StateInfo;
//int32 try_seek_times=3;
#if 1
int32 Video_MjpegOpen(uint16 lcd_start_x, uint16 lcd_start_y, char* file_name)
{
#if 0
    int32 result;
    Mp4Play.file_name=file_name;
    Mp4Play.lcd_start_x=lcd_start_x;
    Mp4Play.lcd_start_y=lcd_start_y;

    if((result=mmc_vid_open_file_req_hdlr(&Mp4Play, &StateInfo))==MED_RES_OK)
    {
        //for test
        StateInfo.full_size_mode=1;
        mmc_vid_set_and_get_info(&StateInfo);
        //
        Mp4Play.seek_pra.get_frame=1;
        Mp4Play.seek_pra.time=0;
        Mp4Play.seek_pra.absolute=1;
        result=mmc_vid_seek_req_hdlr(&Mp4Play);
    }
    return result;
#else
    return 0;
#endif
}
int32 Video_MjpegSeek(int32 time, int32 absolute)
{
#if 0
    int32 result;
    Mp4Play.seek_pra.get_frame=1;
    Mp4Play.seek_pra.time=time;
    Mp4Play.seek_pra.absolute=absolute;
    if((result=mmc_vid_stop_req_hdlr())==MED_RES_OK)
    {
        result=mmc_vid_seek_req_hdlr(&Mp4Play);
        if((result=mmc_vid_play_req_hdlr(&Mp4Play))==MED_RES_OK)
            EVENT_Send2Jade(CHAN_MMC_ID, MCI_VID_SEEK_DONE_IND, v_context->current_time, 0, 0);
    }
    else
    {
        if((result=mmc_vid_seek_req_hdlr(&Mp4Play))==MED_RES_OK)
            EVENT_Send2Jade(CHAN_MMC_ID, MCI_VID_SEEK_DONE_IND, v_context->current_time, 0, 0);
    }
    return result;
#else
    return 0;
#endif
}
int32 Video_MjpegPlay()
{
#if 0
    int32 result;
    //for test
    //StateInfo.full_size_mode=1;
    //mmc_vid_set_and_get_info(&StateInfo);
    //
    result=mmc_vid_play_req_hdlr(&Mp4Play);
    return result;
#else
    return 0;
#endif
}
int32 Video_MjpegStop()
{
#if 0
    int32 result;
    result=mmc_vid_stop_req_hdlr();

    Mp4Play.seek_pra.get_frame=1;
    Mp4Play.seek_pra.time=0;
    Mp4Play.seek_pra.absolute=1;
    if(mmc_vid_seek_req_hdlr(&Mp4Play)!=MED_RES_OK)
    {
        EVENT_Send2Task(MBOX_ID_MMC, MSG_VID_STOP_REQ);
    }
    return result;
#else
    return 0;
#endif
}
int32 Video_MjpegClose()
{
#if 0
    int32 result;
    result=mmc_vid_close_file_req_hdlr();
    return result;
#else
    return 0;
#endif
}
int32 Video_MjpegPause()
{
#if 0
    int32 result;
    result=mmc_vid_stop_req_hdlr();
    return result;
#else
    return 0;
#endif
}
int32 Video_MjpegResume()
{
#if 0
    int32 result;
    result=mmc_vid_play_req_hdlr(&Mp4Play);
    return result;
#else
    return 0;
#endif
}
#endif
int32 Video_MjpegUserMsg(int32 nCmdHandle)
{
    int32 result;
    //mci_vid_play_finish_ind_struct *msg;
    //mmi_trace(TRUE, "[mmc_video] Video_MjpegUserMsg nCmdHandle=%d \n", nCmdHandle);
    switch(nCmdHandle)
    {
        case MSG_VID_IMG_DEC_REQ:
        case EV_TIMER:
            vid_dec_video_frame();
            break;
        case MSG_VID_AUD_DEC_REQ:
            vid_dec_audio_frame();
            break;
        case MSG_VID_DISPLAY_REQ:
            vid_display();
            break;
        case MSG_VID_DISPLAY_INTERFACE_REQ:
            if(VID_IN_STATE(VID_PLAY))
                DISPLAY_INTERFACE=1;
            else
                vid_display_interface();
            break;
        case MSG_VID_STOP_REQ:
        {
            media_vid_play_finish_ind_struct *ind_p;
            ilm_struct       *local_ilm_ptr = NULL;
            int32 i;

            result = mmc_vid_stop_req_hdlr();
            v_context->current_time=0;
            COS_Sleep(200);//ms

            ind_p = (media_vid_play_finish_ind_struct*)
                    construct_local_para(sizeof(media_vid_play_finish_ind_struct), TD_CTRL);

            ind_p->result = result;
            ind_p->seq_num = 0;

            local_ilm_ptr = allocate_ilm(MOD_MED);
            local_ilm_ptr->src_mod_id = MOD_MED;
            local_ilm_ptr->dest_mod_id = MOD_MMI;
            local_ilm_ptr->sap_id = MED_SAP;

            local_ilm_ptr->msg_id = (msg_type_t)MSG_ID_MEDIA_VID_PLAY_FINISH_IND;
            local_ilm_ptr->local_para_ptr = (local_para_struct*)ind_p;
            local_ilm_ptr->peer_buff_ptr = NULL;

            i=0;
            while(!mmc_SendMsg(GetMmiTaskHandle(local_ilm_ptr->dest_mod_id), local_ilm_ptr->msg_id, (uint32)local_ilm_ptr, 0, 0))
            {
                i++;
                mmi_trace(TRUE, "COS_SendEvent 2 MMI fail i=%d",i);
                COS_Sleep(20);//ms
                if(i>=3)
                {
                    result=1;
                    break;
                }
            }

        }
        break;
        default:
            break;
    }
    return MED_RES_OK;
}
#endif


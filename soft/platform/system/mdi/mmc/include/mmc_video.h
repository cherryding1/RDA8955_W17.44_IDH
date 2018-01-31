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
#ifndef _MMC_VIDEO_H_
#define _MMC_VIDEO_H_

#include "mmf_vid_avi.h"
//#include "med_main.h"
//#define LCD_DMA_TEST
//#define ZOOM_ON
/*
#define VID_ENTER_STATE(_s_) (vid_context_p->state = _s_)
#define VID_AUD_ENTER_STATE(_s_) (vid_context_p->aud_state = _s_)

#define VID_IN_STATE(_s_) (vid_context_p->state == _s_)
#define VID_AUD_IN_STATE(_s_) (vid_context_p->aud_state == _s_)
*/
//mmc return err enum  shenh2007.9.14
#if 0
typedef enum
{
    MED_RES_OK=0,
    MED_RES_FAIL=1,
    MED_RES_BUSY=2,
    MED_RES_DISC_FULL =3,
    MED_RES_OPEN_FILE_FAIL=4,
    MED_RES_END_OF_FILE=5,
    MED_RES_BAD_FORMAT=6,
    MED_RES_INVALID_FORMAT=7,
    MED_RES_ERROR=8,
    MED_RES_RESUME=9,
    MED_RES_MEM_INSUFFICIENT=10,
    MED_RES_FILE_EXIST=11,
    MED_RES_PARAM_ERROR=12,

    MED_RES_VIDEO_IMAGE_TOO_LARGE=13,
    MED_RES_VIDEO_FRAME_RATE_TOO_HIGH=14,
    MED_RES_VIDEO_FILE_TOO_LARGE=15,

    MED_MALLOC_ERR=16,
    MED_AUDIO_READ_ERR=17,
    MED_VIDEO_READ_ERR=18,
    MED_AMR_DEC_ERR=19,
    MED_MP3_DEC_ERR=20,
    MED_JPEG_DEC_ERR=21,
    MED_DISPLAY_ERR=22,
    MED_VOC_OPEN_ERR=23,
    MED_PCM_OPEN_ERR=24,
    MED_FILE_READY=25,
    MED_MP3_OPEN_ERR=26,
    MED_IMAGE_BIG_THAN_LCD_ERR=27,
    MED_VIDEO_IMAGE_SIZE_INVALID=28

}
med_result_enum;
#endif
typedef struct
{
    uint8   state;
    uint8   aud_state;
    uint8   a_Pause_DecReq;

    uint8   media_type;                    /* 3GP, ... */
    int32   file_handle;                     /* the file handle of the file */
    int32   current_frame_num;              /* the current frame number of video file */
    int32   display_frame_num;              /* the current displayed frame number of video file */
    uint32   total_frame_num;              /* the total frame number of video file */
    uint32   current_time;                  /* the current time duration of the video play */
    uint32   total_time;                     /* the total time duration of the video file */

    uint8   open_audio;                   /* open audio, 1: yes, 0: no */
    uint8   play_audio;                   /* play audio, 1: yes, 0: no */
    uint8   get_frame;                     /* get frame when seek */
    uint32   file_size_limit;                /* the limit of the video file size */

    uint32   period;                      /* the period(ms) of the video clip */
    uint32   period_fraction;                 /* the period fraction of the video clip for timer create*/
    uint32   time_elapsed;                 /* the time elapsed during a period of the video clip */
    uint32   frame_rate;                      /* the frame rate fps of the video clip */

    uint8   *file_buffer_address;           /* image buffer address  */
    uint32   file_size;                      /* image buffer size */
    uint8   *decode_buffer_address;         /* intermediate image buffer address for jpeg decode result */
    uint32   decode_buffer_size;            /* intermediate image buffer size for jpeg decode result */
    uint16   image_width;                  /* the image width after decoder and resizer */
    uint16   image_height;                  /* the image height after decoder and resizer */
    uint16   display_width;                  /* the display width after decoder and resizer */
    uint16   display_height;                  /* the display height after decoder and resizer */
    uint16       lcd_offset_x;
    uint16       lcd_offset_y;
    uint16   lcd_size_x;
    uint16   lcd_size_y;
    int16     full_size_mode;// 1=yes 0=no
    int16     rotate_on;// 1=yes 0=no (rotate 90 degree clockwise)
    ///for audio
    int32 current_aud_frame_num;
    int32 total_aud_frame_num;
    int32 aud_frame_period;//audio frame period(ms)
    int32 aud_frame_size;//bytes
    int32 aud_frame_sample;//8k mp3 =576*ch ,8k amr =160*ch
    uint32   a_fmt;             /* Audio format, see #defines below */
    uint32   a_chans;           /* Audio channels, 0 for no audio */
    uint32   a_sample;            /* Rate in Hz */
    uint32   a_bits;            /* bits per audio sample */
    uint32   a_bitrate;           /* mp3 bitrate kbs*/
    uint32   audio_bytes;       /* Total number of bytes of audio data */

}
vid_context_struct;

/* Audio Context Struct */
typedef struct
{
    uint8   state;
    int16   current_format;

    int8*   pcm_temp_buf;
    int32*  ring_buf;//for pcm
    uint8*      aud_buf;
    uint8*      p_aud_buf;
    int32 aud_buf_remain_bytes;
    //int32 aud_buf_used_bytes;

    int32     a_outPutPath;    /*out put from headphone or speaker*/
}
aud_context_struct;

typedef  struct
{
    //int audio_buffer_remain_len;
    //int audio_buffer_fill_len;
    int fist_buffer_flag;//  0 first buf can write, 1 sec buf can write
    int jpeg_buf_flag;//  0 first buf can write, 1 sec buf can write
    int rgb_buf_flag;//  0  buf can write, 1  buf can not write
} play_control;


//////////////////////////////////////////////////////////
//        for user
//////////////////////////////////////////////////////////

#if 1
typedef enum
{
    MSG_VID_STOP_REQ,
    MSG_VID_IMG_DEC_REQ,
    MSG_VID_AUD_DEC_REQ,
    MSG_VID_DISPLAY_REQ,
    MSG_VID_DISPLAY_INTERFACE_REQ,
    MSG_VID_GET_STATE_REQ
}
msg_id_enum;
#endif

typedef struct
{
    /*get info*/
    int32     result;
    uint32   total_time;
    uint32   total_frame_num;
    uint32   current_time;
    uint16   image_width;
    uint16   image_height;
    uint8     pada;
    uint8     padb;
    uint16   seq_num;
} mp4_play_info;

typedef struct
{
    /*set info*/
    int16   zoom_mode; //0=default, 1=zoom
    uint16  lcd_start_x;//image_width+lcd_start_x<lcd_x
    uint16  lcd_start_y;
} mp4_set_info;

typedef struct
{
    uint8       get_frame;//need display? 1=y 0=no
    int32   time;//ms
    int32       time_mode;// 0=default absolute time  1= time offset
    uint16  lcd_start_x;//image_width+lcd_start_x<lcd_x
    uint16  lcd_start_y;
}
mp4_seek_struct;

typedef struct
{
    HANDLE           file_handle;
    int32           OutputPath;
    uint16          lcd_start_x;//image_width+lcd_start_x<lcd_x
    uint16          lcd_start_y;
    uint8       open_audio;// open=1,off=0
} mp4_open_struct;

#define  ORIGINAL  0
#define  ZOOMIN     1
#define  ZOOMOUT  2

int32 mmc_vid_open_file_req_hdlr(mp4_open_struct *pMp4Play);
int32 mmc_vid_seek_req_hdlr(mp4_seek_struct *pSeek);
int32 mmc_vid_play_req_hdlr(uint16 lcd_start_x, uint16 lcd_start_y);
int32 mmc_vid_pause_req_hdlr (void);
int32 mmc_vid_resume_req_hdlr (void);
int32 mmc_vid_stop_req_hdlr(void);
int32 mmc_vid_close_file_req_hdlr(void);
//int32  mmc_vid_dec_audio_frame(void);
//int32  mmc_vid_dec_video_frame(void);
//int32  mmc_vid_display(void);
int32 mmc_vid_get_info( mp4_play_info* pState);
int32 mmc_vid_set_info(mp4_set_info *pSetInfo);

int32 Video_MjpegOpen(uint16 lcd_start_x, uint16 lcd_start_y, char* file_name);
int32 Video_MjpegPlay();
int32 Video_MjpegStop();
int32 Video_MjpegClose();
int32 Video_MjpegPause();
int32 Video_MjpegResume();
int32 Video_MjpegSeek(int32 time, int32 absolute);
int32 Video_MjpegUserMsg(int32 nCmdHandle);


#endif
#endif


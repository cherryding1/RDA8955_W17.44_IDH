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


#include "global.h"
#ifdef MP4_3GP_SUPPORT


#ifndef _MMC_ISOM_VIDEO_H_
#define _MMC_ISOM_VIDEO_H_

#define H263_SUPPORT
#define MPEG4_SUPPORT
#ifdef MEDIA_RM_SUPPORT //define in target.def
#define RM_SUPPORT
#endif


#ifdef MEDIA_VOCVID_SUPPORT //define in target.def
#define VOCVID_CAUD_SUPPORT
#ifdef MEDIA_H264_SUPPORT //define in target.def
#define H264_SUPPORT
#endif
#endif
#ifdef MEDIA_DRA_SUPPORT //define in target.def
#define DRA_SUPPORT
#endif
#define VID_XCPU_FREQ_SET //control xcpu frequence
#define VID_VOC_FREQ_SET //control voc frequence

#define VDO_RSLUT_QVGA ( 320*240) //memory need about 360KB for MP4
#define VDO_RSLUT_WQVGA (400*240)
#define VDO_RSLUT_CIF (352*288)
#define VDO_RSLUT_HVGA (480*320)//memory need about 700KB for MP4
#define VDO_RSLUT_D1 (720*480)
#define VDO_RSLUT_4CIF (704*576)

#ifdef MEDIA_VOCVID_SUPPORT //define in target.def
#define VDO_RSLUT_LIMIT_SIZE VDO_RSLUT_HVGA
//#define VDO_RSLUT_LIMIT_SIZE_LOWRES VDO_RSLUT_4CIF
#define VDO_RSLUT_LIMIT_SIZE_LOWRES 0 //close lowres.
#else
#define VDO_RSLUT_LIMIT_SIZE VDO_RSLUT_QVGA
#endif


#include "isomedia_dev.h"
#include "mmf_vid_avi.h"
#include "mci.h"
#ifdef VOCVID_CAUD_SUPPORT
#ifdef RM_SUPPORT
#include "gecko2codec.h"
#endif
#endif
#ifdef H264_SUPPORT
#include "avcodec.h"
#endif
#ifdef VOCVID_CAUD_SUPPORT
#include "aacdec.h"
//#include "mp3dec.h"
#include "mmf_mp3_api.h"
#include "amrfile.h"
#include "sbc.h"
#endif
//return err
typedef enum
{
    ISOM_ERR_OK=0,
    ISOM_ERR_OPEN_FILE_FAIL=1,
    ISOM_ERR_END_OF_FILE=2,
    ISOM_ERR_UNSUPPORT=3,
    ISOM_ERR_INVALID_FORMAT=4,
    ISOM_ERR_MEM_INSUFFICIENT=5,
    ISOM_ERR_PARAM_ERROR=6,

    ISOM_ERR_VIDEO_UNSUPPORT_IMAGE_SIZE=7,
    ISOM_ERR_VIDEO_UNSUPPORT_FPS=8,
    ISOM_ERR_VIDEO_UNSUPPORT_FILE_SIZE=9,

    ISOM_ERR_MALLOC_FAIL=10,
    ISOM_ERR_AUDIO_READ_FAIL=11,
    ISOM_ERR_VIDEO_READ_FAIL=12,
    ISOM_ERR_VIDEO_DEC_FAIL=13,
    ISOM_ERR_AUDIO_DEC_FAIL=14,
    ISOM_ERR_DISPLAY_FAIL=15,
    ISOM_ERR_VOC_OPEN_FAIL=16,
    ISOM_ERR_PCM_OPEN_FAIL=17,
    ISOM_ERR_OPEN_PARSE_FILE_FAIL=18,
    ISOM_ERR_PCM_START_FAIL=19,
    ISOM_ERR_DATA_NONE=20,
    ISOM_ERR_VIDEO_INIT_FAIL=21,
    ISOM_ERR_PLAY_STATE_FAIL=22,
    ISOM_ERR_COOK_INIT_FAIL=23

}
isom_err_enum;

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

typedef struct
{
    unsigned int is_aac;
    unsigned int aac_type;
} aacTypeStru;

typedef struct
{
    int dec_mode;//0=c video voc audio;1=c audio voc video;
    int med_type;
    unsigned int filehdl;
    GF_ISOFile *file;
    unsigned char* v_SampleBuf;
    unsigned int v_SampleBufLen;
    unsigned char* v_SampleBuf_Pos;
    int v_SampleBuf_BytesRemain;
    int v_SampleBuf_BytesUsed;

    //unsigned char* a_SampleBuf;
    //unsigned int a_SampleBufLen;
    //unsigned char* a_SampleBuf_Pos;
    //int a_SampleBuf_BytesRemain;//for decode
    //int a_SampleBuf_BytesUsed;

    unsigned char* a_SampBuf;//a_SampBufMirrorLen + a_SampBufPipeLen + a_SampBufMirrorLen
    unsigned char* a_SampStart;//get data pos. a_SampStart-a_SampEnd>=4bytes(a_SampStart right, a_SampEnd left) or a_SampStart-a_SampEnd<0(a_SampStart left, a_SampEnd right)
    unsigned char* a_SampEnd;//fill data pos
    unsigned int a_SampBufPipeLen;// ****=========****  = is a_SampBufPipe  * isa_SampBufMirror(left same as right)
    unsigned int a_SampBufMirrorLen;
    unsigned char* a_SampTmpBuf;
    unsigned char* a_SampTmpBufp;
    unsigned int a_SampTmpRemain;

    unsigned int v_YuvInfo[5];//get information from decoder. 0=y address, 1=u address, 2=v address, 3=extend width, 4=extend height
#if  ( CHIP_HAS_GOUDA ==1)
    //unsigned int v_YuvInfo2[5];// used for pingpang buffer
#endif
    unsigned short* v_ImageBuf;
    unsigned short* v_ImageBuf2;
#if ( CHIP_HAS_GOUDA !=1)
    unsigned short* v_ImageBufPos;
#endif
    unsigned char   v_ImageBuf_HalfFlag;//for pingpang buf
    unsigned char* v_ExtraRotateBuf;//for soft rotate
    uint32 v_LastSkipRotateTk;//record last tk that skip rotate and display
    BOOL bSkipFrame;
    BOOL v_LastFrmStates;//1=dec err ,0= ok

    int16 a_vocDecType;
    uint8* a_PcmBuf;
    uint8* a_PcmBufHalfPos;
    uint32* a_PcmBufCache;
    int32 a_PcmBuf_HalfFlag;
    uint32 a_HalfPcmBufFrame;
    unsigned char* a_PcmFrameTempBuf;
    uint32* a_Pause_Pos; //for pcm out pause
    uint16 a_Pause_DecReq; //pause state decode 0=none 1=yes
    uint8* a_ExtraPcmBuf;


    unsigned int total_time;
    unsigned int current_time;

    uint32 v_type;
    uint16 v_trackID;
    GF_TrackBox *v_trak;
    GF_MediaInformationBox *v_info;
    unsigned int v_total_sample_num;//for read
    unsigned int v_current_sample_num;//for read
    unsigned int v_total_frame_num;//for play
    unsigned int v_total_keyframe_num;//for sync
    int32 v_current_frame_num;//for play
    uint16 v_lowres;
    uint16 v_width;//mp4 real width
    uint16 v_height;
    uint16 v_display_width;//will display width(without rotate)
    uint16 v_display_height;
    uint16 v_rotate;//0=0,1=90,2=270
    uint16 v_rotate_way;//0=by gouda,1=by soft,2=by lcd
    uint16 v_fps;
    uint32 v_period;
    uint32 v_timerPeriod;
    uint32 v_timeScale;
    int16 v_lcd_start_x;//lcd x.from tl to br if no rotate,from tr to bl if 90 clockwise.
    int16 v_lcd_start_y;//lcd y. same as x.
    uint16 v_lcd_end_x;
    uint16 v_lcd_end_y;
    uint16 v_lcd_size_x;//lcd width.horizontal side if no rotate,vertical side if 90 clockwise.
    uint16 v_lcd_size_y;//lcd height.vertical side if no rotate,horizontal side if 90 clockwise.

    int16 v_cutX;//cut area x from scale image(without rotate)
    int16 v_cutY;//cut area y from scale image(without rotate)
    int16 v_cutW;//cut area width from scale image(without rotate)
    int16 v_cutH;//cut area height from scale image(without rotate)
    int16 v_cutX_origin;//cut area x from origin image(without rotate)
    int16 v_cutY_origin;//cut area y from origin image(without rotate)
    int16 v_cutW_origin;//cut area width from origin image(without rotate)
    int16 v_cutH_origin;//cut area height from origin image(without rotate)

    GF_DefaultDescriptor *v_decSpeInfo;//for m4v

    uint32 a_type;
    uint16 a_trackID;
    GF_TrackBox *a_trak;
    GF_MediaInformationBox *a_info;
    unsigned int a_total_sample_num;//chunk num, some frame maybe in 1 chunk.
    unsigned int a_current_sample_num;
    unsigned int a_total_frame_num;//frame num
    unsigned int a_current_frame_num;//reuse in rmvb for record pcm sample num.
    unsigned int a_sync_distance_frame_num;//once sync every a_sync_distance_frame_num audio frame
    unsigned int a_frame_Period;
    unsigned int a_sample_rate;
    unsigned int a_frame_sample;//sample
    unsigned int a_frameBytes;
    unsigned int a_codeframeBytes;
    unsigned int a_amr_mode;
    uint16 a_bit;
    unsigned int a_bitrate;
    uint16 a_channels;
    int a_audio_path;
    uint8 open_audio;//1=play video with audio, 0=play video without audio, 2=only play audio, 3=audio pause

    GF_M4ADecSpecInfo a_cfg;
    aacTypeStru aacPara;
    h263DecSpecStrc *h263Sp;//for h.263
    amrDecSpecStrc *amrSp;//for amr
    GF_AVCConfig *h264Cfg;//for h.264
    avi_struct *avi_p;//for AVI
#ifdef VOCVID_CAUD_SUPPORT
    HAACDecoder *a_AacDecHdl;//AAC c decoder
    //HMP3Decoder a_Mp3DecHdl;// mp3 c decoder
    AMRStruct*  a_AmrStruct;
#ifdef RM_SUPPORT
    HGecko2Decoder a_Gecko2DecHdl;//for ra cook
#endif
#ifdef H264_SUPPORT
    h264handle *pH264Hdl;//for h.264 decoder
#endif
    sbc_t sbc;
#endif
#ifdef DRA_SUPPORT
    void *a_DraDecHdl;
#endif
    void(*vid_play_finish_callback)(int32);// result input
    void (*vid_draw_panel_callback)(uint32); //draw the interface for playing, current time input
} IsomPlayGlobal;

/* Video track type enum */
typedef enum
{
    MDI_VIDEO_TRACK_NONE,   /* Video has no track in it */
    MDI_VIDEO_TRACK_AV,     /* Video has both audio and video tracks */
    MDI_VIDEO_TRACK_A_ONLY, /* Video has audio track only */
    MDI_VIDEO_TRACK_V_ONLY, /* Video has video track only */
    MDI_VIDEO_TRACK_COUNT   /* Count of video track enum */
} mdi_video_track_enum;

typedef struct
{
    /*get info*/
    uint32   total_time;
    uint32   current_time;
    uint32   total_frame_num;
    uint16   image_width;
    uint16   image_height;
    uint16   aud_channel_no;
    uint16   aud_sample_rate;
    mdi_video_track_enum track;
    uint32   a_type;
} IsomPlayInfo;

typedef struct
{
    /*set mode*/
    uint16   zoom_width;//image width to scale.(without rotate)
    uint16   zoom_height;//image height to scale.(without rotate)
    int16   lcd_start_x;//lcd x.from topleft if no rotate,from topright if 90 clockwise.
    int16   lcd_start_y;//lcd y. same as x.
    int16 cutX;//cut area x from scale image.(without rotate)
    int16 cutY;//cut area y from scale image.(without rotate)
    int16 cutW;//cut area width from scale image.(without rotate)
    int16 cutH;//cut area height from scale image.(without rotate)
    uint16   rotate;// bit01:0=no, 1=90clockwise 2=270clockwise,
    /*
    bit23 for gouda layer.
    3 set HAL_GOUDA_VID_LAYER_BEHIND_ALL ,
    2 set HAL_GOUDA_VID_LAYER_BETWEEN_1_0 ,
    1set HAL_GOUDA_VID_LAYER_BETWEEN_2_1 ,
    0 set HAL_GOUDA_VID_LAYER_OVER_ALL
    bit4 for display area control
    1= control display clip to set area.
    0= whole lcd area.
    */
} IsomPlayMode;


//#define MAX_TRACK_NUM 2
//#define MAX_TOPBOX_SLOT_NUM 10

#define SEEK_TIME_MODE_ABSOLUTE 0 //0=default absolute time
#define SEEK_TIME_MODE_OFFSET 1  // 1= time offset

//IsomPlayGlobal *pIsomGlob;
//GF_BitStream bs_glob;

//extern unsigned short zoom_width;// for zoom
//extern unsigned short zoom_height;// for zoom
//extern unsigned short zoom_mode;//0=no zoom 1=zoom in 2=zoom out
//extern unsigned short rotate_mode;//0=no rotate 1=image rotate 2=LCD rotate
//extern unsigned char* tempRotateBuf;//for rotate YUV

/*
MCI_VideoOpenFile

audio_path: audio path
open_audio: default 1=play video with audio, 0=play video without audio, 2=only play audio
file_handle: file handle or buffer address.
file_mode: 0 file mode, 1 buffer mode, 2 http temp file mode, 3 av stream ringbuf mode
data_Len: data length in fhd buffer when file_mode==1
type: media type

sheen
*/
int32 mmc_vid_isom_open_file( HANDLE file_handle , uint8 file_mode, uint32 data_Len, int32 audio_path, uint8 open_audio, mci_type_enum type, void(*vid_play_finish_callback)(int32), void (*vid_draw_panel_callback)(uint32));
int32 mmc_vid_isom_play (uint16 lcd_start_x,  uint16 lcd_start_y);
int32 mmc_vid_isom_seek ( uint32    time, int16 time_mode, int16 lcd_start_x, int16 lcd_start_y);
int32 mmc_vid_isom_pause (void);
int32 mmc_vid_isom_resume (void);
int32 mmc_vid_isom_stop(void);
int32 mmc_vid_isom_close_file (void);
int32 mmc_vid_isom_get_info( IsomPlayInfo* pInfo);
int32 mmc_vid_isom_set_mode( IsomPlayMode* pMode);
int32 Video_IsomUserMsg(COS_EVENT *pVidEv);

#ifdef MEDIA_VOCVID_SUPPORT
int32 mmc_audio_isom_open_file( HANDLE file_handle , uint8 file_mode, uint32 data_Len, int32 audio_path, uint8 open_audio, mci_type_enum type, void(*vid_play_finish_callback)(int32), void (*vid_draw_panel_callback)(uint32));
int32 mmc_audio_isom_play (uint16 lcd_start_x,  uint16 lcd_start_y);
int32 mmc_audio_isom_seek ( uint32	time, int16 time_mode, int16 lcd_start_x, int16	lcd_start_y);
int32 mmc_audio_isom_pause (void);
int32 mmc_audio_isom_resume (void);
int32 mmc_audio_isom_stop(void);
int32 mmc_audio_isom_close_file (void);
int32 mmc_audio_isom_get_info( IsomPlayInfo* pInfo);
int32 Audio_IsomUserMsg(COS_EVENT *pVidEv);
#endif

#endif
#endif


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


#ifndef _MCI_H__
#define _MCI_H__
////////////////////////////////////////////////////////////////////////////////
// DECLARE HEAD FILES
////////////////////////////////////////////////////////////////////////////////
#include "cs_types.h"
#include "cpu_share.h"

#ifndef __BT_NO_A2DP_PROFILE__
#define __BT_A2DP_PROFILE__
#endif


// ============================================================================
// Defines
// ============================================================================

// ============================================================================
// Types
// ============================================================================

#if (CSW_EXTENDED_API_AUDIO_VIDEO==1)

#else

typedef enum
{
    MCI_TYPE_NONE=-1,
    MCI_TYPE_GSM_FR,             /* 0 */
    MCI_TYPE_GSM_HR,              /* 1 */
    MCI_TYPE_GSM_EFR,             /* 2 */
    MCI_TYPE_AMR,                 /* 3 */
    MCI_TYPE_AMR_WB,              /* 4 */
    MCI_TYPE_DAF,                 /* 5 */
    MCI_TYPE_AAC,                 /* 6 */
    MCI_TYPE_PCM_8K,              /* 7 */
    MCI_TYPE_PCM_16K,             /* 8 */
    MCI_TYPE_G711_ALAW,           /* 9 */
    MCI_TYPE_G711_ULAW,           /* 10 */
    MCI_TYPE_DVI_ADPCM,           /* 11 */
    MCI_TYPE_VR,                  /* 12 */
    MCI_TYPE_WAV,                 /* 13 */
    MCI_TYPE_WAV_ALAW,            /* 14 */
    MCI_TYPE_WAV_ULAW,            /* 15 */
    MCI_TYPE_WAV_DVI_ADPCM,       /* 16 */
    MCI_TYPE_SMF,                    /* 17 */
    MCI_TYPE_IMELODY,                /* 18 */
    MCI_TYPE_SMF_SND,             /* 19 */
    MCI_TYPE_MMF,                /* 20 */
    MCI_TYPE_AU,                 /*  21 */
    MCI_TYPE_AIFF,                 /*  22 */
    MCI_TYPE_M4A,                  /* 23  */
    MCI_TYPE_3GP,                  /*  24 */
    MCI_TYPE_MP4,                      /* 25 */
    MCI_TYPE_JPG,                      /* 26 */
    MCI_TYPE_GIF,                      /* 27 */
    MCI_TYPE_MJPG,                     /* 28 */
    MCI_TYPE_WMA,                  /* 29 */
    MCI_TYPE_MIDI,                  /* 30 */
    MCI_TYPE_RM,              /*31*/
    MCI_TYPE_AVSTRM,         /*32*/
    MCI_TYPE_SCO,                 /*33*/

    MCI_NO_OF_TYPE
}
mci_type_enum;
// ============================================================================
// AUDIO_EQ
// ----------------------------------------------------------------------------
/// Enum describing the various equalizer modes
// ============================================================================
typedef enum
{
    NORMAL,   /*EQ=0*/
    BASS,     /*EQ=1*/
    DANCE,    /*EQ=2*/
    CLASSICAL,/*EQ=3*/
    TREBLE,   /*EQ=4*/
    PARTY,    /*EQ=5*/
    POP,      /*EQ=6*/
    ROCK,     /*EQ=7*/
    AUDIO_EQ_NUM
} AUDIO_EQ;

// ============================================================================
// MCI_AUDIO_PATH_T
// ----------------------------------------------------------------------------
/// Enum describing the various audio paths
// ============================================================================
typedef enum
{
    MCI_PATH_NORMAL    = 0,      /* normal  */
    MCI_PATH_HP        = 1,      /* earphone, carkit */
    MCI_PATH_LSP       = 2,      /* loudspeaker  */
    MCI_PATH_FM_HP     = 3,
    MCI_PATH_FM_LSP_HP = 4
} MCI_AUDIO_PATH_T;

// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
///
///
// =============================================================================

typedef enum
{
    STREAM_STATUS_REQUEST_DATA,
    STREAM_STATUS_NO_MORE_DATA,
    STREAM_STATUS_END,
    STREAM_STATUS_ERR,

} APBS_STREAM_STATUS_T;


typedef enum
{
    MSG_MMC_AUDIODEC_VOC,
    MSG_MMC_AUDIODEC_PCM,
//  MSG_MMC_AUDIODEC_A2DP,
    MSG_MMC_AUDIODEC_SCO,

} AUDIODEC_USER_MSG;


typedef enum
{
    MCI_ERR_NO,
    MCI_ERR_UNKNOWN_FORMAT,
    MCI_ERR_BUSY,
    MCI_ERR_INVALID_PARAMETER,
    MCI_ERR_ACTION_NOT_ALLOWED,
    MCI_ERR_OUT_OF_MEMORY,
    MCI_ERR_CANNOT_OPEN_FILE,
    MCI_ERR_END_OF_FILE,
    MCI_ERR_TERMINATED,
    MCI_ERR_BAD_FORMAT,
    MCI_ERR_INVALID_FORMAT,
    MCI_ERR_ERROR,
    MCI_ERR_DISK_FULL,


} MCI_ERR_T;


typedef struct _PlayInfromation
{
    INT32 PlayProgress;
    INT32 reserve;
} MCI_PlayInf;

typedef struct _ProgressInfromation
{
    INT32 DurationTime;//ms
    INT32 BitRate;
    INT32 ID3Offset;
    INT32 reserve;
} MCI_ProgressInf;


// =============================================================================
// APBS_PLAY_MODE_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a stream to play.
// =============================================================================

typedef enum
{
    MCI_PLAY_MODE_AMR475,
    MCI_PLAY_MODE_AMR515,
    MCI_PLAY_MODE_AMR59,
    MCI_PLAY_MODE_AMR67,
    MCI_PLAY_MODE_AMR74,
    MCI_PLAY_MODE_AMR795,
    MCI_PLAY_MODE_AMR102,
    MCI_PLAY_MODE_AMR122,
    MCI_PLAY_MODE_FR,
    MCI_PLAY_MODE_HR,
    MCI_PLAY_MODE_EFR,
    MCI_PLAY_MODE_PCM,
    // TODO Implement that mode
    MCI_PLAY_MODE_AMR_RING,
    MCI_PLAY_MODE_MP3,
    MCI_PLAY_MODE_AAC,
    MCI_PLAY_MODE_WAV,
    MCI_PLAY_MODE_MID,
    MCI_PLAY_MODE_STREAM_PCM, //for TTS stream play
    MCI_PLAY_MODE_QTY

} MCI_PLAY_MODE_T;







// ============================================================================
// MCI_AUDIO_PLAY_CALLBACK_T
// ----------------------------------------------------------------------------
/// Callback function called when the service changes state
// ============================================================================

typedef VOID (*MCI_AUDIO_PLAY_CALLBACK_T)(MCI_ERR_T result);
// ============================================================================
// Functions
// ============================================================================
void MCI_AudioSetFinishCallbask(MCI_AUDIO_PLAY_CALLBACK_T callback);
// ============================================================================
// MCI_AudioPlay
// ----------------------------------------------------------------------------
/// Play the selected file on selected output
/// @param OutputPath: Selects the audio path to use
/// @param filename: Unix absolute filename
/// @param callback: Callback function called when the state changes (e.g EOF)
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_UNKNOWN_FORMAT, MCI_ERR_NO
// ============================================================================

// MCI_MEDIA_PLAY_REQ,
UINT32 MCI_AudioPlay(INT32 OutputPath,HANDLE fileHandle,mci_type_enum fielType,MCI_AUDIO_PLAY_CALLBACK_T callback,INT32 PlayProgress);
// ============================================================================
// MCI_AudioPause
// ----------------------------------------------------------------------------
/// Pause the current stream
/// @param none
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioPause(VOID);

// ============================================================================
// MCI_AudioResume
// ----------------------------------------------------------------------------
/// Resume previoulsy paused stream
/// @param none
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_NO
// ============================================================================
// MCI_MEDIA_PLAY_REQ,
UINT32 MCI_AudioResume (HANDLE fileHandle);

// ============================================================================
// MCI_AudioStop
// ----------------------------------------------------------------------------
/// Stop current stream
/// @param none
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioStop (VOID);

// ============================================================================
// MCI_AudioSetEQ
// ----------------------------------------------------------------------------
/// select equalizer mode
/// @param EQMode : Equalizer mode
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioSetEQ(AUDIO_EQ EQMode);

// ============================================================================
// MCI_AudioSetOutputPath
// ----------------------------------------------------------------------------
/// Allows to change the audio path while playing
/// @param OutputPath : Selects the audio path to use
/// @param Mute : If Mute=1 the sound will be muted
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioSetOutputPath(UINT16 OutputPath,UINT16 Mute);

// ============================================================================
// MCI_AudioSetVolume
// ----------------------------------------------------------------------------
/// Allows to change the audio volume
/// If no audio is currently played, the value will be saved and used for
/// the next stream played
/// @param volume : Selects the audio volume
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioSetVolume(UINT16 volume);

UINT32 MCI_AudioGetPlayInformation(MCI_PlayInf* PlayInformation);

VOID MCI_AudioFinished(MCI_ERR_T result);
VOID MCI_RingFinished(MCI_ERR_T result);
VOID MCI_AudioRecordFinished(MCI_ERR_T result);

#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */















typedef struct
{
    S8 *file_name;//[255];
    UINT16 display_width;
    UINT16 display_height;
    UINT16 image_clip_x1; /* clip window start x for clipping decoded image */
    UINT16 image_clip_x2; /* clip window end x for clipping decoded image */
    UINT16 image_clip_y1; /* clip window start y for clipping decoded image */
    UINT16 image_clip_y2; /* clip window end y for clipping decoded image */
    VOID*  image_buffer_p;
    UINT32 image_buffer_size;
    UINT8  media_mode;
    UINT8  media_type;
    UINT32 file_size;
    UINT16 seq_num;
    UINT8  blocking;
    UINT16 decoded_image_width; // decoded image width
} img_decode_req_struct;

typedef struct
{
    UINT8   padA;
    UINT16  padB;
    INT16   result;
    UINT16  image_width;
    UINT16  image_height;
    UINT32  total_frame_num;
    UINT32  total_time;
    UINT32  current_time;
    UINT16  seq_num;
    UINT16  media_type;
    UINT16  aud_channel;
    UINT16  aud_sample_rate;
    UINT16  track;
    UINT16  audio_type;
} mci_vid_file_ready_ind_struct; //chenhe add for file ready to get total duration,it's equal to media_vid_file_ready_ind_struct

typedef struct
{
    UINT8    padA;
    UINT16   padB;
    UINT16   result;
    UINT16   seq_num;
}
mci_vid_play_finish_ind_struct;

typedef struct
{
    UINT8    padA;
    UINT16   padB;
    UINT16   result;
    UINT16   seq_num;
}
mci_vid_seek_done_ind_struct;

typedef struct
{
    UINT8   padA;
    UINT16  padB;
    INT16   result;
    UINT16  seq_num;
}
mci_vid_record_finish_ind_struct;

typedef struct
{
    UINT8   padA;
    UINT16  padB;
    UINT8   src_id;
    UINT8   result;
}
mci_aud_record_finish_ind_struct;

typedef enum
{
    MBOX_ID_SERVER,
    MBOX_ID_APP,
    MBOX_ID_COCEC,
//add by longnb
    //MBOX_ID_MAINCONTROL,
    MBOX_ID_AGENT,
    MBOX_ID_FSAPP,
    MBOX_ID_MMC,
    MBOX_ID_DI,
    MBOX_ID_IMAGE,
//end
    MBOX_ID_NUM
} COSMBOXID;




#define MAX_AUD_DESCRIPTION     80
typedef struct tAudInfoDescription
{
    UINT32 time; // duration in sec
    UINT32 bitRate;
    UINT32 sampleRate;
    BOOL stereo;
    INT16 title[MAX_AUD_DESCRIPTION+1];
    INT16 artist[MAX_AUD_DESCRIPTION+1];
    INT16 album[MAX_AUD_DESCRIPTION+1];
    INT16 author[MAX_AUD_DESCRIPTION+1];
    INT16 copyright[MAX_AUD_DESCRIPTION+1];
    INT16 date[MAX_AUD_DESCRIPTION+1];

    union
    {
        struct
        {
            UINT8 CRC;
            UINT8 channel_mode;
            UINT8 bit_rate_index;
        } DAF;
    } details;
} AudDesInfoStruct;


typedef VOID (*MCI_AUDIO_BUFFER_PLAY_CALLBACK_T)(MCI_ERR_T result);
typedef VOID (*MCI_AUDIO_FILE_RECORD_CALLBACK_T)(MCI_ERR_T result);

typedef VOID (*APBS_STREAM_USER_HANDLER_T)(APBS_STREAM_STATUS_T);
typedef VOID (*MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T)(unsigned char* buf_p,unsigned int len, unsigned int *record_len) ;


// Video
UINT32  MCI_VideoClose (VOID) ;  //MCI_VID_CLOSE_REQ
#if (CSW_EXTENDED_API_AUDIO_VIDEO==1)
UINT32  MCI_VideoOpenFile (INT32 OutputPath,UINT8* filename);
#else
/*
MCI_VideoOpenFile

OutputPath: audio path
open_audio: 1=play audio, 0=no
fhd: file handle or buffer address.
file_mode: 0 file mode, 1 buffer mode
data_Len: data length in fhd buffer when file_mode==1
type: media type

sheen
*/
UINT32  MCI_VideoOpenFile (INT32 OutputPath,UINT8 open_audio, HANDLE fhd, UINT8 file_mode, UINT32 data_Len, mci_type_enum type, VOID(*vid_play_finish_callback)(int32), VOID (*vid_draw_panel_callback)(uint32));   //MCI_VID_OPEN_FILE_REQ
#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */
VOID    MCI_VideoOpenFileInd (mci_vid_file_ready_ind_struct *vdoOpen)  ;   //MCI_VID_FILE_READY_IND;
UINT32  MCI_VideoPlay (UINT16 startX, UINT16 startY);   //MCI_VID_PLAY_REQ
VOID    MCI_VideoPlayInd (VOID) ;  // MCI_VID_PLAY_FINISH_IND
UINT32  MCI_VideoStop (VOID) ;   // MCI_VID_STOP_REQ
UINT32  MCI_VideoPause (VOID) ; //  MCI_VID_PAUSE_REQ
UINT32  MCI_VideoResume (VOID)  ;// MCI_VID_RESUME_REQ
UINT32  MCI_VideoSeek (long long playtime, INT32 time_mode, INT16 startX, INT16 startY)  ;// MCI_VID_SEEK_REQ
VOID    MCI_VideoSeekInd (VOID) ; // MCI_VID_SEEK_DONE_IND
UINT32  MCI_VideoSet (UINT16 ZoomWidth, UINT16 ZoomHeight, INT16 startX, INT16 startY, INT16 cutX, INT16 cutY, INT16 cutW, INT16 cutH, UINT16 Rotate)  ;// MCI_VID_SET_MODE_REQ, Rotate=0(no), 1(90clockwise), 2(270clockwise)
UINT16  MCI_VideoGetInfo ( UINT16  *image_width, UINT16  *image_height, UINT32  *total_time, UINT16  *aud_channel, UINT16  *aud_sample_rate,UINT16  *track);
UINT32  MCI_VideoGetPlayTime (VOID) ;

// Camera
UINT32 MCI_CamPowerUp (INT32  vediomode, void (*cb)(int32));   // MCI_CAM_POWER_UP_REQ
UINT32  MCI_CamPowerDown (VOID) ; //MCI_CAM_POWER_DOWN_REQ;
UINT32  MCI_CamPreviewOpen(CAM_PREVIEW_STRUCT* data);//MCI_CAM_PREVIEW_REQ
UINT32  MCI_CamPreviewClose (VOID) ; //MCI_CAM_STOP_REQ
UINT32  MCI_CamCapture (CAM_CAPTURE_STRUCT *data);    //MCI_CAM_CAPTURE_REQ
UINT32  MCI_CamSetPara(INT32 effectCode,INT32 value) ; // MCI_CAM_SET_PARAM_REQ
INT32 MCI_CamMdOpen(INT32 sensLevel, VOID(* md_callback)(int32));
INT32 MCI_CamMdClose( VOID);
VOID MCI_CamDoMd( VOID);



// Image
UINT32  MCI_ImgDecode (img_decode_req_struct *imgPara);  // MCI_IMG_DECODE_REQ
UINT32  MCI_ImgDecodeEvenInd (VOID) ; // MCI_IMG_DECODE_EVENT_IND
UINT32  MCI_ImgDecodeFinishInd (VOID) ; //MCI_IMG_DECODE_FINISH_IND
UINT32  MCI_ImgResize (img_decode_req_struct *imgPara)  ;// MCI_IMG_RESIZE_REQ
UINT32  MCI_ImgStop (VOID) ; //MCI_IMG_STOP_REQ

// Init
BOOL    MCI_TaskInit(VOID);     //  MCI_MEDIA_PLAY_REQ,
BOOL    MCI_LcdSizeSet(INT32 LcdWidth, INT32 LcdHeight);
#ifdef DUAL_LCD
BOOL    MCI_SubLcdSizeSet(INT32 LcdWidth, INT32 LcdHeight);
#endif
INT32  MMC_GetBVGA(INT32 bvga);
// Video Record
UINT32  MCI_VideoRecordAdjustSetting(INT32 adjustItem, INT32 value);
UINT32  MCI_VideoRecordPause(VOID);
UINT32  MCI_VideoRecordPreviewStart(MMC_VDOREC_SETTING_STRUCT *previewPara, VOID(*vid_rec_finish_ind)(UINT16 msg_result));
UINT32  MCI_VideoRecordPreviewStop(VOID);
UINT32  MCI_VideoRecordResume(VOID);
UINT32  MCI_VideoRecordStart(HANDLE filehandle);
UINT32  MCI_VideoRecordStop(VOID);
UINT32  MCI_DisplayVideoInterface (VOID);

// MCI Task
HANDLE  GetMCITaskHandle(COSMBOXID mod);
BOOL    SetMCITaskHandle(COSMBOXID mod, HANDLE hTask);


INT32  MCI_AudioPlayBuffer(INT32 *pBuffer, UINT32 len, UINT8 loop,MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback,MCI_PLAY_MODE_T  format,INT32 startPosition);
INT32  MCI_AUD_StreamPlay( INT32 *pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback);
INT32  MCI_AUD_StreamPlayPCM( INT32 *pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback,UINT32 sampleRate, UINT32  bitPerSample);

INT32  MCI_AudioStopBuffer(VOID);
UINT32 MCI_AudioPlayPause(VOID);
UINT32 MCI_AudioPlayResume(VOID);
UINT32* MCI_GetBufPosition(VOID);

void MCI_GetWriteBuffer( UINT32 **buffer, UINT32 *buf_len );
UINT32 MCI_GetRemain(VOID);
void MCI_DataFinished( void );
INT32 MCI_AddedData(UINT32 addedDataBytes);

void  MCI_SetBuffer(UINT32 *buffer, uint32 buf_len);


extern UINT32 MCI_AudioRecordStart (HANDLE fhd,mci_type_enum format,U8 quality,MCI_AUDIO_FILE_RECORD_CALLBACK_T callback,  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback);
extern UINT32 MCI_AudioRecordPause(VOID);
extern UINT32 MCI_AudioRecordResume(VOID);
extern UINT32 MCI_AudioRecordStop(VOID);



typedef struct
{

    /// X coordinate of the top left corner of the region.
    UINT16 StartX;

    /// Y coordinate of the top left corner of the region.
    UINT16 StartY;

    /// Width of the region.
    UINT16 ImageWidth;

    /// Height of the region.
    UINT16 ImageHeight;

    /// flag of rotate.  1 stand for rotating, o stand for no rotating.
    UINT16 Rotate;

} MCI_ANALOG_TV_PREVIEW_CFG_T;

// Audio Streaming Sender

typedef struct sender_stream_handle SSHdl;
struct sender_stream_handle
{
    INT32 (*PutData)( SSHdl *hdl, UINT8 *, UINT32 );
    VOID (*Flush)( SSHdl *hdl );

    UINT8 *rb_base;
    INT32 rb_size;
    INT32 rb_read;
    INT32 rb_write;
    INT32 rb_threshold;
    VOID (*callback)( VOID * );
    BOOL wait;
    INT32 rb_read_total;
    /*
    BOOL      overflow;
    INT32      overflow_size;
    UINT8 *overflow_buf; */
} ;

typedef struct
{
    UINT32 Emphasis: 2;
    UINT32 Original: 1;
    UINT32 Copyright: 1;
    UINT32 ModeExt: 2;
    UINT32 ChannelMode: 2; // fixed
    UINT32 Private: 1;
    UINT32 Padding: 1;
    UINT32 SampleRateIndex: 2; // fixed
    UINT32 BitRateIndex: 4;
    UINT32 Protection: 1;
    UINT32 Layer: 2; // fixed
    UINT32 VersionID: 2; // fixed
    UINT32 Sync: 11; // fixed
} dafHeaderStruct;

typedef struct
{
    SSHdl ss_handle;
    dafHeaderStruct config_header;
    UINT32 last_frame_size;
    UINT32 last_bit_rate;
    UINT32 fragment_start;
} dafRTPSSHdlInternal;





/* SBC event */
typedef enum
{
    A2DP_DATA_NOTIFY
} A2DP_Event;



typedef VOID (*A2DP_Callback)(A2DP_Event , VOID*);

typedef struct
{
    UINT32 (*GetPayload)(UINT8 *, UINT32, UINT32 *);
    VOID (*GetPayloadDone)(VOID);
    VOID (*QueryPayloadSize)(UINT32 *, UINT32 *);
    UINT32 (*AdjustBitRateFromQos)(UINT8); // return adjusted bit rate
    UINT32 (*SetBitRate)(UINT32); // return adjusted bit rate
    UINT8 state;
} A2DP_codec_struct;





#ifndef __BT_A2DP_CODEC_TYPES__
#define __BT_A2DP_CODEC_TYPES__

typedef struct
{
    UINT8 min_bit_pool;
    UINT8 max_bit_pool;
    UINT8 block_len; // b0: 16, b1: 12, b2: 8, b3: 4
    UINT8 subband_num; // b0: 8, b1: 4
    UINT8 alloc_method; // b0: loudness, b1: SNR
    UINT8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 16000
    UINT8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
} bt_a2dp_sbc_codec_cap_struct;



typedef struct
{
    UINT8 layer; // b0: layerIII, b1: layerII, b0: layerI
    BOOL CRC;
    UINT8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
    BOOL MPF; // is support MPF-2
    UINT8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 24000, b4: 22050, b5: 16000
    BOOL VBR; // is support VBR
    UINT16 bit_rate; // bit-rate index in ISO 11172-3 , b0:0000 ~ b14: 1110
} bt_a2dp_mp3_codec_cap_struct; /* all MPEG-1,2 Audio */




typedef struct
{
    UINT8 object_type; // b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC
    UINT16 sample_rate; // b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000
    UINT8 channels; // b0: 2, b1: 1
    BOOL VBR; // is supported VBR
    UINT32 bit_rate; // constant/peak bits per second in 23 bit UiMsbf, 0:unknown
} bt_a2dp_aac_codec_cap_struct; /* all MPEG-2,4 AAC */

typedef struct
{
    UINT8 version; // 1:ATRAC, 2:ATRAC2, 3:ATRAC3
    UINT8 channel_mode; // b0: joint stereo, b1: dual, b2: single
    UINT8 sample_rate; // b0: 48000, b1: 44100
    BOOL VBR; // is supported VBR
    UINT32 bit_rate; // bit-rate index in ATRAC, b0: 0x0012 ~ b18: 0x0000
    UINT16 max_sul; // sound unit length in 16 bits UiMsbf
} bt_a2dp_atrac_codec_cap_struct; /* all ATRAC family */

typedef union
{
    bt_a2dp_sbc_codec_cap_struct sbc;
    bt_a2dp_mp3_codec_cap_struct mp3;
    bt_a2dp_aac_codec_cap_struct aac;
    bt_a2dp_atrac_codec_cap_struct atrac;
} bt_a2dp_audio_codec_cap_struct;

typedef struct
{
    UINT8 codec_type; // SBC, MP3
    bt_a2dp_audio_codec_cap_struct codec_cap;
} bt_a2dp_audio_cap_struct;



#endif /* _MED_STRUCT_H */

BOOL is_SSHdl_valid(SSHdl *hdl);


MCI_ERR_T MCI_AudioGetFileInformation (CONST HANDLE FileHander,
                                       AudDesInfoStruct  * CONST DecInfo,
                                       CONST mci_type_enum FileType   );


UINT32 MCI_AudioGetDurationTime(HANDLE fileHandle,
                                mci_type_enum fielType,
                                INT32 BeginPlayProgress,
                                INT32 OffsetPlayProgress,
                                MCI_ProgressInf* PlayInformation);

A2DP_codec_struct *MCI_A2dpDafOpen(  A2DP_Callback pHandler,
                                     bt_a2dp_audio_cap_struct *daf_config_data,
                                     UINT8 *buf, UINT32 buf_len);


VOID MCI_A2dpDafClose( void);



A2DP_codec_struct *MCI_SBCOpen(   A2DP_Callback pHandler,
                                  bt_a2dp_audio_cap_struct *daf_config_data,
                                  UINT8 *buf, UINT32 buf_len);


VOID MCI_SBCClose( void);

VOID MCI_FmSetOutputDevice(MCI_AUDIO_PATH_T device);

UINT32 MCI_FmPlay(VOID);

UINT32 MCI_FmStop(VOID);

UINT32 MCI_FmSetup(UINT8 volume );


#ifdef AUD_3_IN_1_SPK


extern volatile INT16 g_MCIAudioVibrator;


VOID MCI_AudioVibratorStart(VOID);

VOID MCI_AudioVibratorStop(VOID);

#endif


#define SBC_Out_put_addr                                         (7916 + 0x0000) //256 short  sbc_output_addr
#define OUTPUT_PCM_BUFFER1_ADDR                          (3244 + 0)



typedef VOID (*PNGDEC_USER_HANDLER_T)(INT32 value);

//struct of output parameters
typedef struct
{
    // ----------------
    // common status
    // ----------------
    INT16 mode;
    INT16 errorStatus;

    // ----------------
    // pngdec status
    // ----------------
    INT32 PngdecFinished;



} MCI_PNGDEC_STATUS_T;


INT32 MCI_PngdecStart(UINT32 *FileContent,
                      UINT32 FileSize,
                      UINT32 *PNGOutBuffer,
                      UINT32 OutBufferSize,
                      UINT32 *AlphaOutBuffer,
                      UINT32 AlphaBufferSize,
                      UINT32 *BkgrdBuffer,
                      UINT16 BkgrdClipX1,
                      UINT16 BkgrdClipX2,
                      UINT16 BkgrdClipY1,
                      UINT16 BkgrdClipY2,
                      INT16 BkgrdOffsetX,
                      INT16 BkgrdOffsetY,
                      UINT16 BkgrdWidth,
                      UINT16 BkgrdHeight,
                      UINT16 ZoomWidth,
                      UINT16 ZoomHeight,
                      UINT16 DecMode,
                      UINT16 alpha_blending_to_transparentColor,
                      UINT32 *ImgWidth,
                      UINT32 *ImgHeigh ,
                      UINT32*WindowBuf,
                      UINT32 *rgb_temp_ptr,
                      UINT32 PngFileHandle,
                      PNGDEC_USER_HANDLER_T handle

                     );

BOOL MCI_PngdecGetDecodingStatus(MCI_PNGDEC_STATUS_T *status_png);
#endif // __JADE_LILY_MCI_H__



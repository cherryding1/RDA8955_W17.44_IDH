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



#ifndef __MMC_VIDEORECORD_H__
#define __MMC_VIDEORECORD_H__


//#if ((defined __MEDIACHIP__)&&(LILY== __MEDIACHIP__))

////////////////////////////////////////////////////////////////////////////////
// DECLARE HEAD FILES
////////////////////////////////////////////////////////////////////////////////
// TODO...
#include "cs_types.h"
#include "mmc.h"
#include "cpu_share.h"
#include "mmc_sndrec.h"


#if (CHIP_HAS_GOUDA == 1)

#include "cos.h"
//#include "aud_m.h"

#ifdef VIDREC_USE_QVGA_SCREEN
#define VIDEO_RECORD_SENSOR_SAMPLE_RATE  CA_FRAME_10
#define VIDEO_RECORD_DISP_WIDTH   176
#define VIDEO_RECORD_DISP_HEIGHT  144
//#define VIDREC_AVI_VIDEO_FRAME_RATE   10
#else
#define VIDEO_RECORD_SENSOR_SAMPLE_RATE  CA_FRAME_13
#define VIDEO_RECORD_DISP_WIDTH   128//(g_VidRecCntx.settingInfo.previewWidth/16*16)//176
#define VIDEO_RECORD_DISP_HEIGHT  96//(g_VidRecCntx.settingInfo.previewHeight/16*16)//144
//#define VIDREC_AVI_VIDEO_FRAME_RATE   10
#endif

#define JPEG_BUF_NUM (2)
#define AVI_BUF_NUM (3)

#ifdef VDOREC_MODE_VGA
// QVGA Recording
#define JPEG_BUF_SIZE (80)
#define AVI_BUF_SIZE (90)
#elif defined(VDOREC_MODE_QVGA)
// QVGA Recording
#define JPEG_BUF_SIZE (60)
#define AVI_BUF_SIZE (70)
#elif defined(VDOREC_MODE_QCIF)
// QCIF Recording
#define JPEG_BUF_SIZE (30)
#define AVI_BUF_SIZE (35)
#else
// QQVGA Recording by default
#define JPEG_BUF_SIZE (40)  
#define AVI_BUF_SIZE (45)
#endif

#if defined(VDOREC_MODE_VGA)
#define VIDREC_AVI_VIDEO_FRAME_RATE 8
#elif defined(VDOREC_FAST_RECORDING)
#define VIDREC_AVI_VIDEO_FRAME_RATE 15
#else
#define VIDREC_AVI_VIDEO_FRAME_RATE 10
#endif

#define VIDREC_AVI_AUDIO_FRAME_INTERVAL   (1000/VIDREC_AVI_VIDEO_FRAME_RATE)
#define VIDREC_AMR_FRAME_NUM_IN_A_PACK   (VIDREC_AVI_AUDIO_FRAME_INTERVAL/20)
#define VIDREC_CHECK_AVI_FILE_SIZE  (10<<10)        //10k
#define VIDREC_USER__MAX_SECOND         32*60//max 32 minutes //660*3   // max 30 minutes
#define VIDREC_AVI_FRAME_MAX_COUNT (VIDREC_USER__MAX_SECOND*VIDREC_AVI_VIDEO_FRAME_RATE)  //7200*3
#define VIDREC_AVI_SINGLE_INDEX_LEN     32
#define VIDREC_AVI_INDEX_TABLE_HEAD_LEN   8
#define VIDREC_DEFAULT_FREQ_TICK_COUNT_ONE_SECOND 16384//200

#define MDI_RES_VDOREC_SUCCEED                      0
#define MDI_RES_VDOREC_DISK_FULL                        2001
#define MDI_RES_VDOREC_REACH_SIZE_LIMIT             2002
#define MDI_RES_VDOREC_RECORD_ALREADY_STOP          2003
#define MDI_RES_VDOREC_REACH_DISK_LIMIT             2004
#define MDI_RES_VDOREC_ERR_FAILED                       -2001
#define MDI_RES_VDOREC_ERR_DISK_FULL                    -2002
#define MDI_RES_VDOREC_ERR_WRITE_PROTECTION         -2003
#define MDI_RES_VDOREC_ERR_NO_DISK                  -2004
#define MDI_RES_VDOREC_ERR_HW_NOT_READY         -2005
#define MDI_RES_VDOREC_ERR_PREVIEW_FAILED           -2006
#define MDI_RES_VDOREC_ERR_RESUME_FAILED                -2007
#define MDI_RES_VDOREC_ERR_PAUSE_FAILED             -2008
#define MDI_RES_VDOREC_ERR_SAVE_FILE_FAILED         -2009
#define MDI_RES_VDOREC_ERR_POWER_ON_FAILED          -2010
#define MDI_RES_VDOREC_ERR_RECORD_FAILED                -2011
#define MDI_RES_VDOREC_ERR_FILE_SIZE                    -2012
#define MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH        -2013

#define AVI_TF_MIN 650 //for 32minutes video index

/* Video encoding quality */
enum
{
    VID_REC_QTY_LOW = 0,
    VID_REC_QTY_NORMAL,
    VID_REC_QTY_HIGH,
    VID_REC_QTY_FINE,
    VID_NO_OF_REC_QTY
};

typedef enum
{
    VIDREC_VOC_IDLE = 0,
    VIDREC_AMR_ENC_BUSY,
    VIDREC_JPEG_ENC_BUSY
} VIDREC_VOC_STATE;

typedef enum
{
    RGB_BUF_NULL = 0,
    RGB_BUF_BUSY,
    RGB_BUF_READY,
    RGB_BUF_INVALID_STATUS
} RGB_BUF_STATUS;

typedef enum
{
    NO_ERRO=0,
    ERROR_OPENFAIL=-1,
    ERROR_WRITEFAIL=-2,
    ERROR_AMROPENFAIL=-3,
    ERROR_RESUMEFAIL=-4,
    ERROR_CloseFAIL=-5
} error_type;

typedef enum
{
    //  MSG_VIDREC_SENSOR_CALLBACK=0,
    MSG_VIDREC_SENSOR_IMAGE_READY=0,
    //  MSG_VIDREC_MIC_CALLBACK,
    MSG_VIDREC_AVI_OUTPUT_REQ,
    MSG_VIDREC_BLT_PARTIAL,
    MSG_VIDREC_BLT_FULL
} msg_id_vidrec;

typedef struct
{
    HANDLE fileHandle;
    int32  pingInDataBlockNum;
    int32  AMREncodeUnitCnt;
    int32  AVIFrameCnt;
    int32  JPEGEncodeFrameCnt;
    Codecmode  VppCodecMode;
    VIDREC_VOC_STATE  VocState;
    uint16  curJPEGLen;
    uint32  lastJPEGLen;
    uint8 RGBBufStatus[2];
    MMC_VDOREC_SETTING_STRUCT settingInfo;
    uint32 recordStartTime;     // point to tick
    uint32 lastRecordPeriod;    //reocord period before pause
    uint32 curFileSize;
    uint16 zoomWidthValue;
    uint16 zoomHeightValue;
    void (*vidrec_callback)(uint16 msg_result);
} VIDREC_CONTEXT;

int32 VidRec_PreviewStart(MMC_VDOREC_SETTING_STRUCT* previewPara,void(*vid_rec_finish_ind)(uint16 msg_result));
int32 VidRec_PreviewStop(void);
int32 VidRec_RecordStart(HANDLE filehandle);
int32 VidRec_RecordPause(void);
int32 VidRec_RecordResume(void);
int32 VidRec_RecordStop(void);

void VidRec_UserMsg(COS_EVENT *pnMsg);
void VidRec_CallbackFun(UINT8 id,  UINT8 ActiveBufNum);
int32 vidrec_VideoSampleStart(MMC_VDOREC_SETTING_STRUCT* previewPara);
void    VidRec_JPEGEncode(int32* buf_in,short width,short height);
void VidRec_FS_AsynWrite_CB(void* pParam);
void VidRec_PreCap(void);
void VidRec_FrameOutput(void);

void VidRec_Pcm2Amr(void);
error_type VidRec_AVIFileClose(VIDREC_CONTEXT* amr_in);


#else // (CHIP_HAS_GOUDA != 1)


//#define VIDREC_USE_640X480
////////////////////////////////////////////////////////////////////////////////
// DEFINE LOCAL CONSTANTS
////////////////////////////////////////////////////////////////////////////////
// TODO..
#define VIDREC_USE_QVGA_SCREEN
//#define VIDREC_USE_640X480
#ifdef VIDREC_USE_640X480
#define VIDEO_RECORD_MJPEG_WIDTH 640//240
#define VIDEO_RECORD_MJPEG_HEIGHT 480//320
#else
#ifdef VIDREC_USE_320X240
#define VIDEO_RECORD_MJPEG_WIDTH 320//240
#define VIDEO_RECORD_MJPEG_HEIGHT 240//320
#else
#define VIDEO_RECORD_MJPEG_WIDTH 128//128
#define VIDEO_RECORD_MJPEG_HEIGHT 96//96    
#endif
#endif
#define VIDEO_RECORD_SENSOR_BUF_MIN_SIZE  320*240*2

#define VIDREC_AMR_FRAME_MAX_CNT_IN_A_PACK  10

#define VIDREC_PCM_INBUF_SIZE       80*4*VIDREC_AMR_FRAME_MAX_CNT_IN_A_PACK
#define VIDREC_AMRENC_OUTBUF_SIZE      8*4*VIDREC_AMR_FRAME_MAX_CNT_IN_A_PACK

#ifdef VIDREC_USE_QVGA_SCREEN
#define VIDEO_RECORD_SENSOR_SAMPLE_RATE  CA_FRAME_10
#define VIDEO_RECORD_DISP_WIDTH   176
#define VIDEO_RECORD_DISP_HEIGHT  144
#define VIDREC_AVI_VIDEO_FRAME_RATE   10
#else
#define VIDEO_RECORD_SENSOR_SAMPLE_RATE  CA_FRAME_13
#define VIDEO_RECORD_DISP_WIDTH   128//(g_VidRecCntx.settingInfo.previewWidth/16*16)//176
#define VIDEO_RECORD_DISP_HEIGHT  96//(g_VidRecCntx.settingInfo.previewHeight/16*16)//144
#define VIDREC_AVI_VIDEO_FRAME_RATE   10
#endif
#define VIDREC_AVI_AUDIO_FRAME_INTERVAL   (1000/VIDREC_AVI_VIDEO_FRAME_RATE)
#define VIDREC_AMR_FRAME_NUM_IN_A_PACK   (VIDREC_AVI_AUDIO_FRAME_INTERVAL/20)

#define VIDREC_USER__MAX_SECOND         32*60//max 32 minutes //660*3   // max 30 minutes
//#define   VIDREC_AVI_FRAME_MAX_COUNT    (VIDREC_USER__MAX_SECOND*VIDREC_AVI_VIDEO_FRAME_RATE*6/5)  //7200*3
#define VIDREC_AVI_FRAME_MAX_COUNT    (VIDREC_USER__MAX_SECOND*VIDREC_AVI_VIDEO_FRAME_RATE)  //7200*3

#define MMC_VIDREC_USER_MSG_BASE    2
#define MMC_VIDREC_VOC_CONTINUE     MMC_VIDREC_USER_MSG_BASE
#define MMC_VIDREC_INVALID_SENSOR_CONFIG_ITEM   0xFF
#define MMC_VIDREC_INVALID_SENSOR_EV_PARAM   0xFF

#define VIDREC_DEFAULT_FREQ_TICK_COUNT_ONE_SECOND 16384//200

//#define   VIDREC_AVI_HEAD_SIZE    328
#define VIDREC_AVI_INDEX_TABLE_HEAD_LEN   8
#define VIDREC_AVI_SINGLE_INDEX_LEN     32
#define VIDREC_CHECK_AVI_FILE_SIZE  (10<<10)        //10k

#define MSG_VID_REC_START   123
#define MSG_VID_REC_END     124
#define JPEG_BUF_INTERVAL 20*1024
////////////////////////////////////////////////////////////////////////////////
// DEFINE LOCAL MACROS
////////////////////////////////////////////////////////////////////////////////
// TODO...


#define MDI_RES_VDOREC_SUCCEED                  0
#define MDI_RES_VDOREC_DISK_FULL                    2001
#define MDI_RES_VDOREC_REACH_SIZE_LIMIT         2002
#define MDI_RES_VDOREC_RECORD_ALREADY_STOP      2003
#define MDI_RES_VDOREC_REACH_DISK_LIMIT         2004
#define MDI_RES_VDOREC_ERR_FAILED                   -2001
#define MDI_RES_VDOREC_ERR_DISK_FULL                -2002
#define MDI_RES_VDOREC_ERR_WRITE_PROTECTION     -2003
#define MDI_RES_VDOREC_ERR_NO_DISK              -2004
#define MDI_RES_VDOREC_ERR_HW_NOT_READY     -2005
#define MDI_RES_VDOREC_ERR_PREVIEW_FAILED       -2006
#define MDI_RES_VDOREC_ERR_RESUME_FAILED            -2007
#define MDI_RES_VDOREC_ERR_PAUSE_FAILED         -2008
#define MDI_RES_VDOREC_ERR_SAVE_FILE_FAILED     -2009
#define MDI_RES_VDOREC_ERR_POWER_ON_FAILED      -2010
#define MDI_RES_VDOREC_ERR_RECORD_FAILED            -2011
#define MDI_RES_VDOREC_ERR_FILE_SIZE                -2012
#define MDI_RES_VDOREC_ERR_MEMORY_NOT_ENOUGH    -2013
////////////////////////////////////////////////////////////////////////////////
// DECLARE GLOBAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
// TODO...

////////////////////////////////////////////////////////////////////////////////
// DEFINE VARIABLES TYPE
////////////////////////////////////////////////////////////////////////////////
// TODO...
typedef enum
{
    VIDREC_VOC_IDLE = 0,
    VIDREC_AMR_ENC_BUSY,
    VIDREC_JPEG_ENC_BUSY
} VIDREC_VOC_STATE;

typedef enum
{
    AMR_ENCODE_OUTBUF_NULL = 0,
    AMR_ENCODE_OUTBUF_BUSY,
    AMR_ENCODE_OUTBUF_READY,
    AMR_ENCODE_OUTBUF_INVALID_STATUS
} AMR_ENCODE_OUTBUF_STATUS;

typedef enum
{
    RGB_BUF_NULL = 0,
    RGB_BUF_BUSY,
    RGB_BUF_READY,
    RGB_BUF_INVALID_STATUS
} RGB_BUF_STATUS;

typedef struct
{
    HANDLE fileHandle;
    int32  pingInDataBlockNum;
//  int32 *pingOutData;
    int32  AMREncodeUnitCnt;
    int32  AVIFrameCnt;
    int32  JPEGEncodeFrameCnt;
    Codecmode  VppCodecMode;
    VIDREC_VOC_STATE  VocState;
    uint16  curJPEGLen;
    uint32  lastJPEGLen;
//  uint8 AMREncOutBufStatus[2];
    uint8 RGBBufStatus[2];
    MMC_VDOREC_SETTING_STRUCT settingInfo;
    uint32 recordStartTime;     // point to tick
    uint32 lastRecordPeriod;    //reocord period before pause
    uint32 curFileSize;
    uint16 zoomWidthValue;
    uint16 zoomHeightValue;
// add callback function
    void (*vidrec_callback)(uint16 msg_result);
} VIDREC_CONTEXT;

typedef enum
{
//      MSG_VIDREC_SENSOR_CALLBACK=0,
    MSG_VIDREC_SENSOR_IMAGE_READY=0,
//  MSG_VIDREC_MIC_CALLBACK,
    MSG_VIDREC_AVI_OUTPUT_REQ,
    MSG_VIDREC_BLT_PARTIAL,
    MSG_VIDREC_BLT_FULL
} msg_id_vidrec;

typedef enum
{
    NO_ERRO=0,
    ERROR_OPENFAIL=-1,
    ERROR_WRITEFAIL=-2,
    ERROR_AMROPENFAIL=-3,
    ERROR_RESUMEFAIL=-4,
    ERROR_CloseFAIL=-5

} error_type;

////////////////////////////////////////////////////////////////
//    Jade side camera sensor param constant define
////////////////////////////////////////////////////////////////

/* White balance enum */
/*
enum {
    CAM_WB_AUTO = 0,
    CAM_WB_CLOUD,
    CAM_WB_DAYLIGHT,
    CAM_WB_INCANDESCENCE,
    CAM_WB_FLUORESCENT,
    CAM_WB_TUNGSTEN,
    CAM_WB_MANUAL,
    CAM_NO_OF_WB
};
*/
/* exposure compensation value enum */
enum
{
    CAM_EV_NEG_4_3 = 0,
    CAM_EV_NEG_3_3,
    CAM_EV_NEG_2_3,
    CAM_EV_NEG_1_3,
    CAM_EV_ZERO,
    CAM_EV_POS_1_3,
    CAM_EV_POS_2_3,
    CAM_EV_POS_3_3,
    CAM_EV_POS_4_3,
    CAM_EV_NIGHT_SHOT,
    CAM_NO_OF_EV
};

/* Special Effect for Preview, Capture and MPEG4 Encode */
/*
enum {
    CAM_EFFECT_ENC_NORMAL=0,
    CAM_EFFECT_ENC_GRAYSCALE,
    CAM_EFFECT_ENC_SEPIA,
    CAM_EFFECT_ENC_SEPIAGREEN,
    CAM_EFFECT_ENC_SEPIABLUE,
    CAM_EFFECT_ENC_COLORINV,
    CAM_EFFECT_ENC_GRAYINV,
    CAM_EFFECT_ENC_BLACKBOARD,
    CAM_EFFECT_ENC_WHITEBOARD,
    CAM_EFFECT_ENC_COPPERCARVING,
    CAM_EFFECT_ENC_EMBOSSMENT,
    CAM_EFFECT_ENC_BLUECARVING,
    CAM_EFFECT_ENC_CONTRAST,
    CAM_EFFECT_ENC_JEAN,
    CAM_EFFECT_ENC_SKETCH,
    CAM_EFFECT_ENC_OIL,
    CAM_NO_OF_EFFECT_ENC,
      //add by gugogi
      CAM_EFFECT_ENC_ANTIQUE,
      CAM_EFFECT_ENC_REDRISH,
      CAM_EFFECT_ENC_GREENRISH,
      CAM_EFFECT_ENC_BLUERISH,
      CAM_EFFECT_ENC_BLACKWHITE,
      CAM_EFFECT_ENC_NEGATIVENESS

};
*/
/* Camera- Jpeg encode quality enum */
enum
{
    CAM_JPG_QTY_LOW = 20,  //modified by WeiD
    CAM_JPG_QTY_NORMAL = 14,
    CAM_JPG_QTY_HIGH = 10,
    CAM_JPG_QTY_FINE = 7,
    CAM_NO_OF_JPG_QTY
};

/* Zoom */
#define VDOREC_FEATURE_ZOOM_COUNT           (16)

#define VDOREC_FEATURE_ZOOM_0               (10)
#define VDOREC_FEATURE_ZOOM_1               (12)
#define VDOREC_FEATURE_ZOOM_2               (14)
#define VDOREC_FEATURE_ZOOM_3               (16)
#define VDOREC_FEATURE_ZOOM_4               (18)

/* Video encoding quality */
enum
{
    VID_REC_QTY_LOW = 0,
    VID_REC_QTY_NORMAL,
    VID_REC_QTY_HIGH,
    VID_REC_QTY_FINE,
    VID_NO_OF_REC_QTY
};

typedef struct xxxx
{
    int32 fileHandle;
    uint32 dataSize;
    uint8 *writeBuffer;
    struct xxxx *next;
} BUFFER_UNIT_T;

typedef struct
{
    uint8 unitCount;
    BUFFER_UNIT_T *first;
    BUFFER_UNIT_T *end;
} QUEUE_HEAD_T;

////////////////////////////////////////////////////////////////////////////////
// DECLARE GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////////////
// TODO...
extern VIDREC_CONTEXT g_VidRecCntx;
extern const MMC_VDOREC_SETTING_STRUCT g_VidrecDefaultSetting;

#if 0
#define VIDEO_RECORD_DISP_WIDTH (g_VidRecCntx.settingInfo.previewWidth/16*16)//176
#define VIDEO_RECORD_DISP_HEIGHT (g_VidRecCntx.settingInfo.previewHeight/16*16)//144
#endif


int32   VidRec_PreviewStart(MMC_VDOREC_SETTING_STRUCT* previewPara,void(*vid_rec_finish_ind)(uint16 msg_result));
int32   VidRec_PreviewStop(void);
int32   VidRec_RecordStart(HANDLE filehandle);
int32   VidRec_RecordPause(void);
int32   VidRec_RecordResume(void);
int32   VidRec_RecordStop(void);
void    VidRec_Pcm2Amr(void);
void    VidRec_UserMsg(COS_EVENT *pnMsg);
void    VidRec_PreCap(void);
void        VidRec_FrameOutput(void);
//void  VidRec_BltFull(void);
//void  VidRec_BltPartial(void);
void    VidRec_CallbackFun(UINT8 id,  UINT8 ActiveBufNum);
//void  VidRec_JpegEnc(uint16 img_width,uint16 img_height);
int32   vidrec_VideoSampleStart(MMC_VDOREC_SETTING_STRUCT* previewPara);
void        VidRec_JPEGEncode(int32* buf_in,short width,short height);
void   VidRec_FS_AsynWrite_CB(void* pParam);
////////////////////////////////////////////////////////////////////////////////

//#endif // ((defined __MEDIACHIP__)&&(LILY== __MEDIACHIP__))


#endif // (CHIP_HAS_GOUDA != 1)


#endif // __MMC_VIDEORECORD_H__ 


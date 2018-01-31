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

#ifndef _MMC_CAMERA_H_
#define _MMC_CAMERA_H_

#if (CHIP_HAS_GOUDA == 1)

#include "cpu_share.h"
#include "cos.h"
#include "camera_m.h"

//a switch for camera trace
#define CAM_TRACE 1

#define CameraSetContrast     3
#define CameraSetBrightness  4

#define USER_TRACE(_flag,_format,...)   do{ \
                                                                    if( (_flag)>0) \
                                                                    { \
                                                                        diag_printf((_format),##__VA_ARGS__);\
                                                                        diag_printf("\n");\
                                                                    } \
                                    }while(0)


typedef enum
{
    MSG_CAM_CALLBACK=0,
#ifdef _CAM_SPECIAL_GC6113_
    MSG_CAM_PART_DATA,
    MSG_CAM_FULL_DATA
#endif
} msg_id_camera;

typedef enum
{
    CAM_PARAM_NONE=0,
    CAM_PARAM_ZOOM_FACTOR,
    CAM_PARAM_CONTRAST,
    CAM_PARAM_BRIGHTNESS,
    CAM_PARAM_HUE,
    CAM_PARAM_GAMMA,
    CAM_PARAM_WB,
    CAM_PARAM_EXPOSURE,
    CAM_PARAM_EFFECT,
    CAM_PARAM_BANDING,
    CAM_PARAM_SATURATION,
    CAM_PARAM_NIGHT_MODE,
    CAM_PARAM_EV_VALUE,
    CAM_PARAM_FLASH,
    CAM_PARAM_FLASH_MODE=CAM_PARAM_FLASH,
    CAM_PARAM_AE_METERING,
    CAM_PARAM_AF_KEY,
    CAM_PARAM_AF_METERING,
    CAM_PARAM_AF_MODE,
    CAM_PARAM_MANUAL_FOCUS_DIR,
    CAM_PARAM_SHUTTER_PRIORITY,
    CAM_PARAM_APERTURE_PRIORITY,
    CAM_PARAM_ISO,
    CAM_PARAM_SCENE_MODE,
    CAM_PARAM_QUALITY,
    CAM_PARAM_IMAGESIZE,
    CAM_PARAM_ADD_FRAME,//25,chenhe add

    CAM_NO_OF_PARAM
} CAM_SET_PARA;

enum
{
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

typedef enum
{
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
    ROTATE_MAX
} IMG_ROTATE_E;
//monitor camera
typedef enum
{
    MONITOR_PHOTO,
    MONITOR_VIDEO
}MONITOR_CATEGORY;


typedef enum
{
    MONITOR_IDLE,
    MONITOR_POWERON,
    MONITOR_POWEROFF,
    MONITOR_CAPTURE,
    MONITOR_CAPTURE_FINISH,
    MONITOR_SAVE_FINISH
}MONITOR_STATUS;

#define MDI_CAMERA_EFFECT_NORMAL                        CAM_EFFECT_ENC_NORMAL
#define MDI_CAMERA_EFFECT_ANTIQUE                                  CAM_EFFECT_ENC_ANTIQUE
#define  MDI_CAMERA_EFFECT_REDRISH                                 CAM_EFFECT_ENC_REDRISH
#define MDI_CAMERA_EFFECT_GREENRISH                              CAM_EFFECT_ENC_GREENRISH
#define MDI_CAMERA_EFFECT_BLUERISH                                 CAM_EFFECT_ENC_BLUERISH
#define MDI_CAMERA_EFFECT_BLACKWHITE                            CAM_EFFECT_ENC_BLACKWHITE
#define MDI_CAMERA_EFFECT_NEGATIVENESS                        CAM_EFFECT_ENC_NEGATIVENESS

#define MDI_RES_CAMERA_SUCCEED                               0
#define MDI_RES_CAMERA_ERR_FAILED                           -1001
#define MDI_RES_CAMERA_ERR_DISK_FULL                               -1002
#define MDI_RES_CAMERA_ERR_WRITE_PROTECTION             -1003
#define MDI_RES_CAMERA_ERR_NO_DISK                          -1004
#define MDI_RES_CAMERA_ERR_HW_NOT_READY                 -1005
#define MDI_RES_CAMERA_ERR_MEMORY_NOT_ENOUGH                -1006
#define MDI_RES_CAMERA_ERR_PREVIEW_FAILED                   -1007
#define MDI_RES_CAMERA_ERR_CAPTURE_FAILED                   -1008

#if defined(__CAMERA_FULL_OSD__)
UINT8 vidRec_rotate(UINT16* inBuf, UINT16* outBuf, IMG_ROTATE_E rotate, UINT16 width, UINT16 height);
#endif

#if defined(__CAMERA_FULL_OSD__) || defined(__IMAGE_ROTATE_EN__)
typedef struct
{
    UINT32 sidx;
    UINT32 midx;
    UINT8* idxTable;
} ROTATE_TABLE_T;

UINT8 rotate_table_init(UINT16 imgw, UINT16 imgh);
void rotate_table_deinit();
void rotate_table_reset();
UINT8 rotate_table_set_idx(UINT32 idx);
UINT32 rotate_table_get_idx();

UINT32 image_get_dst_idx(UINT32 sidx,
                         IMG_ROTATE_E rotate,
                         UINT16 imgw,
                         UINT16 imgh);
UINT8 image_rotate(UINT16* buf,
                   IMG_ROTATE_E rotate,
                   UINT16 width,
                   UINT16 height);
#endif

typedef struct
{
    // preview window setting
    struct
    {
        UINT16 start_x;
        UINT16 start_y;
        UINT16 end_x;
        UINT16 end_y;
        UINT16 width;
        UINT16 height;
    } prev_win;

    // other preview setting
    BOOL  addFrame;
    UINT8 flash_en;
    UINT16 nightmode;
    UINT16 previewZoom;
    UINT16 preview_rotate;

    // previewing information
    UINT16 prev_width;
    UINT16 prev_height;
    CAM_SIZE_T prev_size;

    // photo setting
    UINT16 image_width;
    UINT16 image_height;
    INT32 image_quality;

    // sensor information
    UINT16 sensor_width;
    UINT16 sensor_height;
    CAM_SIZE_T sensor_size;
#if defined(__CAMERA_FULL_OSD__)
    IMG_ROTATE_E img_rotate;
#endif
} CAM_INFO_STRUCT;

typedef void(*MMC_CAMERA_CB)(int32 result);

void set_multicaputure(int i);

int Lily_Camera_PowerOn(INT32  videomode,void(*cb)(int32));
int Lily_Camera_PowerOff(void);
int Lily_Camera_Preview(CAM_PREVIEW_STRUCT* data);
int Lily_Camera_PreviewOff(void);

void Cam_CallbackFun(UINT8 id,  UINT8 ActiveBufNum);
#ifdef _CAM_SPECIAL_GC6113_
void Cam_CallbackFun_gc6113(UINT8 id,  UINT8 ActiveBufNum);
void mmc_camResize(UINT8* in, UINT32 inSize, UINT8* out);
#endif
void mmc_CamBlendDsp();

int Lily_Camera_Capture(CAM_CAPTURE_STRUCT *data) ;
int Lily_Capture_Preview();
int Lily_camera_savePhoto(int  file_handle);

void Camera_UserMsg(COS_EVENT *pnMsg);
int Lily_Camera_SetEffect(CAM_SET_PARA effectCode,uint32 value);
void camera_cut_image( UINT16* inbuf, UINT16* outbuf, UINT16 srcw, UINT16 srch, UINT16 cutw, UINT16 cuth);
//monitor camera
int lily_Camera_Monitor_PowerOn(MONITOR_CATEGORY mode);
int Lily_Camera_Monitor_PowerOff(void);
int lily_Camera_Monitor_Capture(CAM_PREVIEW_STRUCT* data);
MONITOR_STATUS lily_Camera_Monitor_Get_Status(void);
int lily_Camera_Monitor_Save(int file_handle,char *save_buf);

#else // (CHIP_HAS_GOUDA != 1)

//#include "lp_camera_driver.h"
#include "cpu_share.h"
#include "vpp_preview.h"
#include "cos.h"
//a switch for camera trace
#define CAM_TRACE 1
//#include "mmc_videorecord.h"


//chenhe for get a new file name for all app+
typedef enum
{
    CAM_APP,
    VDOREC_APP,
    DIR_END
} APP_TYPE;
#define CAM_DIR_NAME "photos"
#define VDOREC_DIR_NAME "video"
#define CAM_EXT ".jpg"
#define VIDREC_EXT ".avi"

#define  CameraGetBrightness                1
#define  CameraGetContrast              2

#define  CameraSetContrast              3
#define  CameraSetBrightness                4
#define  CameraSetRedGAIN               5
#define  CameraSetBlueGAIN              6
#define  CameraSetGAIN                  7

#define  CameraXmirror                  8
#define  CameraYmirror                  9
#define  CameraNightModeEn              10
#define  CameraNightModeDis             11

#define  CameraAWB_Day                  12 /*晴天太阳光*/
#define  CameraAWB_Auto             13
#define  CameraAWB_Office               14 /*日光灯*/
#define  CameraAWB_Cloudy               15 /*阴天*/
#define  CameraAWB_Alight               16 /*钨丝灯*/

#define CameraSetExposure_Levl          17


enum
{
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

#define MDI_CAMERA_EFFECT_NORMAL                        CAM_EFFECT_ENC_NORMAL
#define MDI_CAMERA_EFFECT_ANTIQUE                                  CAM_EFFECT_ENC_ANTIQUE
#define  MDI_CAMERA_EFFECT_REDRISH                                 CAM_EFFECT_ENC_REDRISH
#define MDI_CAMERA_EFFECT_GREENRISH                              CAM_EFFECT_ENC_GREENRISH
#define MDI_CAMERA_EFFECT_BLUERISH                                 CAM_EFFECT_ENC_BLUERISH
#define MDI_CAMERA_EFFECT_BLACKWHITE                            CAM_EFFECT_ENC_BLACKWHITE
#define MDI_CAMERA_EFFECT_NEGATIVENESS                        CAM_EFFECT_ENC_NEGATIVENESS


#define MDI_RES_CAMERA_SUCCEED                  0
#define MDI_RES_CAMERA_ERR_FAILED                   -1001
#define MDI_RES_CAMERA_ERR_DISK_FULL                -1002
#define MDI_RES_CAMERA_ERR_WRITE_PROTECTION     -1003
#define MDI_RES_CAMERA_ERR_NO_DISK              -1004
#define MDI_RES_CAMERA_ERR_HW_NOT_READY     -1005
#define MDI_RES_CAMERA_ERR_MEMORY_NOT_ENOUGH    -1006
#define MDI_RES_CAMERA_ERR_PREVIEW_FAILED       -1007
#define MDI_RES_CAMERA_ERR_CAPTURE_FAILED       -1008

enum Exposure_Levl
{
    Exposure_Levl_negative_2=-2,
    Exposure_Levl_negative_1=-1,
    Exposure_Levl_default_0,
    Exposure_Levl_positive_1,
    Exposure_Levl_positive_2
};

enum Brightness_Levl
{
    Brightness_Levl_negative_2=-2,
    Brightness_Levl_negative_1=-1,
    Brightness_Levl_default_0,
    Brightness_Levl_positive_1,
    Brightness_Levl_positive_2
};
enum Contrast_Levl
{
    Contrast_Levl_negative_3=-3,
    Contrast_Levl_negative_2=-2,
    Contrast_Levl_negative_1=-1,
    Contrast_Levl_default_0,
    Contrast_Levl_positive_1,
    Contrast_Levl_positive_2,
    Contrast_Levl_positive_3
};


#define CameraEffect_Normal             18
#define CameraEffect_Antique                19  /*复古*/
#define CameraEffect_Redish             20  /*偏红*/
#define CameraEffect_Greenish           21  /*偏绿*/
#define CameraEffect_Blueish                22  /*偏蓝*/
#define CameraEffect_BlackWhite         23  /*黑白*/
#define CameraEffect_Negative           24  /*负片*/
#define CameraEffect_BlackWhite_Negative 25 /*黑白负片*/
//#define CAM_ENCODE_BY_VOC  //use voc encode jpg
//chenhe for get a new file name for all app-


#define   YUV2RGBLINE  20
/******************function feature swich++++++*************/
//#define CAM_TEST_WIDTH_HEIGHT_ENABLE
#define CAM_BLEND_BY_ASIC
/******************function feature swich--------*************/


#define CAM_ENCODE_BUFFER_LEN (40*1024)
#define CAM_TEST_WIDTH (480)
#define CAM_TEST_HEIGHT  (640)
#define CAM_MAX_FILENAME_LENGTH (30)
#define CAM_FILENAME_PREFIX "cam"

#define CAM_TRANSFERCOLOR (0x0) /*it's transfer color*/
#define USER_TRACE(_flag,_format,...)   do{ \
                                            if( (_flag)>0) \
                                            { \
                                                diag_printf((_format),##__VA_ARGS__);\
                                                diag_printf("\n");\
                                            } \
                                    }while(0)



typedef struct
{
    //uint16 sensor_width;
    //uint16 sensor_height;
    uint16 start_x;
    uint16 start_y;
    uint16 end_x;
    uint16 end_y;
    uint16 image_width;
    uint16 image_height;
    uint16 preview_width;
    uint16 preview_height;
    uint32 previewZoom;
    int32 image_quality;
    boolean addFrame;
} CAM_INFO_STRUCT;

typedef struct
{
    uint32 contrast;
    uint32 bright;
    uint32 expose;
    uint32 effect;
    uint32 nightmode;
    uint32 wb;

} CAM_EFFECT_STRUCT;


typedef enum
{
    CAM_PARAM_NONE=0,
    CAM_PARAM_ZOOM_FACTOR,
    CAM_PARAM_CONTRAST,
    CAM_PARAM_BRIGHTNESS,
    CAM_PARAM_HUE,
    CAM_PARAM_GAMMA,
    CAM_PARAM_WB,
    CAM_PARAM_EXPOSURE,
    CAM_PARAM_EFFECT,
    CAM_PARAM_BANDING,
    CAM_PARAM_SATURATION,
    CAM_PARAM_NIGHT_MODE,
    CAM_PARAM_EV_VALUE,
    CAM_PARAM_FLASH,
    CAM_PARAM_FLASH_MODE = CAM_PARAM_FLASH,
    CAM_PARAM_AE_METERING,
    CAM_PARAM_AF_KEY,
    CAM_PARAM_AF_METERING,
    CAM_PARAM_AF_MODE,
    CAM_PARAM_MANUAL_FOCUS_DIR,
    CAM_PARAM_SHUTTER_PRIORITY,
    CAM_PARAM_APERTURE_PRIORITY,
    CAM_PARAM_ISO,
    CAM_PARAM_SCENE_MODE,
    CAM_PARAM_QUALITY,
    CAM_PARAM_IMAGESIZE,
    CAM_PARAM_ADD_FRAME,//25,chenhe add

    CAM_NO_OF_PARAM
} CAM_SET_PARA;


typedef enum
{
    MSG_CAM_CALLBACK=0,
    MSG_CAM_CALLBACK_VGA=1,
    //  MSG_ATV_CALLBACK=2,
} msg_id_camera;

typedef void(*MMC_CAMERA_CB)(int32 result);

void set_multicaputure(int i);
int mmc_camera_getstatus();
void set_mmc_camera_preview();
void setwb_isr_process(void);
void  zoom_line(uint16* srcbuf,uint16* dec, uint16 dec_w,uint16 scalex);
void Coolsand_bilinerz_zoom_op(uint16* srcbuf,uint16 src_w,uint16 src_h,uint16 cut_w,uint16 cut_h,uint16* decbuf,uint16 dec_w,uint16 dec_h);
void  zoom_line_yuv(uint16* srcbuf,uint16* dec, uint16 dec_w,int scalex);
int Coolsand_bilinerz_zoom_op_yuv(uint16* srcbuf,uint16 src_w,uint16 src_h,uint16 cut_w,uint16 cut_h,uint16* decbuf,uint16 dec_w,uint16 dec_h);
int  Coolsand_bilinerz_zoom_op_yuv2rgb(uint16* srcbuf,uint16 src_w,uint16 src_h,uint16 cut_w,uint16 cut_h,uint16* decbuf,uint16 dec_w,uint16 dec_h);
//void  SimCutZoom(uint16* srcbuf,uint16 src_w,uint16 src_h,uint16 cut_w,uint16 cut_h,uint16* decbuf,uint16 dec_w,uint16 dec_h);
int CoolSand_Cut_Zoom(uint16* srcbuf,uint16 src_w,uint16 src_h,uint16 cut_w,uint16 cut_h,uint16* decbuf,uint16 dec_w,uint16 dec_h);
void  MMC_Preview(CutZoomInStruct *pZoomStruct);
int Mmc_blendPartPure(uint16 *baseBuffer,uint16 baseBufW,uint16 baseBufH,uint16 *relBuffer,uint16 relBufW,uint16 relBufH,uint16 transfCol);
void EmbedBmpDataIntoLcdShadowBuffer(
    uint16 LcdFrameBuffer_x, uint16 LcdFrameBuffer_y,
    uint16 bmp_width, uint16 bmp_height,
    uint16 *pbmp, uint16 *pLcdShadowBuffer);
void mmc_camSwitchDataEnds(uint16 *buffer,uint32 bufferSize);
//void convert_rgb5652yuv422(uint16 *src, uint16 width, uint16 height, uint16 *dec);
int Lily_Camera_PowerOn(INT32  videomode,void(*cb)(int32));
int Lily_Camera_PowerOff(void);
int Lily_Camera_Preview(CAM_PREVIEW_STRUCT* data);
int Lily_Camera_PreviewOff(void);
int lily_transfer_data(void);
void Cam_CallbackFun(UINT8 id,  UINT8 ActiveBufNum);
//void zoomyuv422_SIM(uint16* yuv_buf,uint16 src_w,uint16 src_h,uint16 cut_w,uint16 cut_h, uint16*rgb_buf,uint16 dec_w,uint16 dec_h);
void yuv4112rgb565(uint16* yuv_buf,uint16 src_w,uint16 src_h, uint16*rgb_buf);
void mmc_CamBlendDsp();
void Cam_CallbackFun_VGA(UINT8 id,  UINT8 ActiveBufNum);
void mmc_CamBlendDsp_VGA();
int Lily_Camera_Capture(CAM_CAPTURE_STRUCT *data) ;
int Lily_Capture_Preview();
int Lily_camera_savePhoto(int filehandle);
void Camera_UserMsg(COS_EVENT *pnMsg);
int Lily_Camera_SetEffect(CAM_SET_PARA effectCode,uint32 value);
void ATV_CallbackFun(UINT8 id,  UINT8 ActiveBufNum);
void  ATV_BlendDisplay(void);

#endif // (CHIP_HAS_GOUDA != 1)

#endif // _MMC_CAMERA_H_


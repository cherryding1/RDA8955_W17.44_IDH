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

#ifndef __AP_CAMARA_H__
#define __AP_CAMARA_H__

#include "cs_types.h"
#include "../driver/include/lp_camera_driver.h"

typedef enum _PICTURE_SIZE
{
    PIC_SIZE_1,//640*480
    PIC_SIZE_2,
    PIC_SIZE_3,
    PIC_SIZE_4,
    PIC_SIZE_5
} PICTURE_SIZE;

enum
{
    CAMARASETCONTRAST,
    CAMARASETBTIGHTNESS,
    CAMARASETZOOMIN,
    CAMARASETZOOMOUT,
    CAMARASETREDGAIN,
    CAMARASETGREENGAIN,
    CAMARASETBLUEGAIN,
    CAMARASETGAIN
};
#if 0
typedef struct
{
    uint16  preview_offset_x;
    uint16  preview_offset_y;
    uint16  preview_width;
    uint16  preview_height;
    uint16  image_width;
    uint16  image_height;
    uint8   frame_rate;
} Camera_para_struct;
#endif

void MMC_CameraInit(void);

void MMC_CameraClose(void);

//int32 MMC_CameraCapture(Camera_para_struct *data, uint32 *pOutBuf, uint32 *returnlenth);

int32 MMC_CameraCapture(uint16 image_width, uint16 image_height);

//int32 MMC_CameraPreviewOpen(Camera_para_struct *data);

int32 MMC_CameraPreviewOpen(uint16 preview_offset_x,uint16 preview_offset_y,uint16 preview_width,uint16 preview_height,uint16 frameon);

void MMC_CameraPreviewClose(void);

int32 MMC_SetBrightness(uint32 BrightnessValue);

int32 MMC_SetContrast(uint32 ContrastValue);

int32 MMC_SetColorR(uint32 ColorRValue);

int32 MMC_SetColorG(uint32 ColorGValue);

int32 MMC_SetColorB(uint32 ColorBValue);

int32 MMC_SaveJPGFile(void);

int32 MMC_MultiShotCapture(uint32 Delaytime,uint32 FrameCount);

int32 MMC_MultiShotStop(void);

#endif


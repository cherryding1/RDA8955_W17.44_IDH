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

#ifndef __AP_COMM_FUNCLIST_H__
#define __AP_COMM_FUNCLIST_H__

#include"mcip_debug.h"
#include"cs_types.h"
#include"ap_camera.h"

//test func
int32 MMC_Jade_DataWrite(uint8 *Buffer,int32 Size);
int32 MMC_Jade_DataRead(uint8* Buffer,int32 Size);

//start camera api*************************************************************************
#if 0
void MMC_CameraInit(void);
void MMC_CameraClose(void);
int32 MMC_CameraCapture(void);
int32 MMC_CameraPreviewOpen(uint32 Quality, PICTURE_SIZE Size);
int32 MMC_CameraPreviewClose(void);
int32 MMC_SaveJPGFile(void);
int32 MMC_MultiShotCapture(uint32 Interval,uint32 FrameCount);
int32 MMC_MultiShotStop(void);
int32 MMC_SetBrightness(uint32 BrightnessValue);
int32 MMC_SetContrast(uint32 ContrastValue);
int32 MMC_SetGainR(uint32 ColorRValue);
int32 MMC_SetGainG(uint32 ColorGValue);
int32 MMC_SetGainB(uint32 ColorBValue);
#endif


/*
void cam_module_set_para(uint8 cmd,uint8 para);
void img_open(uint32 length,    uint16 image_offset_x,
        uint16 image_offset_y,  uint16 image_width,     uint16 image_height);
void img_close(void);
uint16 img_displaystill(uint32 length,  uint16 image_offset_x,
        uint16 image_offset_y,  uint16 image_width,     uint16 image_height);

void chipinit (void);
void lcd_draw_image(uint16 startx, uint16 starty,uint16 Width,uint16 Height);
void lcd_update_image(uint16 startx, uint16 starty,uint16 Width,uint16 Height);
*/

//end camera api*************************************************************************


//start image playback func******************************************************************

int32 MMC_PlayImage(uint32 FileIndex,uint32 startX,uint32 startY,uint32 ImageWidth,uint32 ImageHeidth);
//int32 MMC_AddImageSpeEffect(enum SFFECT_FLAG  effect_name);

//end image playback func********************************************************************

#endif//__AP_COMM_FUNCLIST_H__


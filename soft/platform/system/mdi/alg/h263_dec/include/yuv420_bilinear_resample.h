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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _YUV420_BILINEAR_RESAMPLE_H_
#define _YUV420_BILINEAR_RESAMPLE_H_

/*
yuv420_bilinear_resample.h

shenh
2008.4.25
*/

//#include <stdio.h>
//#include "cs_type.h"
//#include "mpeg4_global.h"
#ifdef SHEEN_VC_DEBUG
#include "h263_config.h"
#else
#include "cs_types.h"
#endif

typedef struct
{
    uint16 zoom_width;//shenh for zoom
    uint16 zoom_height;//shenh for zoom
    uint16 zoom_mode;//0=no zoom 1=zoom in 2=zoom out
    uint16 rotate_mode;//0=no rotate 1=image rotate 2=LCD rotate
    uint8* tempRotateBuf;// temp buffer for rotate
} zoomPar;

typedef struct
{
    uint8* y;
    uint8* u;
    uint8* v;
} yuvImage;


//any size resample
int init_pixeltab();
int close_pixeltab();
int Img_Resample(uint8 *InBuf, unsigned short ImgInw, unsigned short ImgInh, uint8 *outBuf,unsigned short ImgOutw,unsigned short ImgOuth);
//any size resample
int Img_Resample2(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, uint8 *outBuf,uint8* tempRotateBuf,unsigned int ImgOutw,unsigned int ImgOuth);
//any size resample and rotate 90degree
int Img_Resample2_rotate_closewise(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, uint8 *outBuf,uint8* tempRotateBuf, unsigned int ImgOutw,unsigned int ImgOuth);

void initResample(short lcd_width);

int Img_Resample_Yuv2Rgb_Embed(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, char* LcdBuf, short LcdBufWidth, short StartX, short StartY,  unsigned int ImgOutw,unsigned int ImgOuth);


//only double size
int  zoom_twice(uint8* inbuf, unsigned short width, unsigned short extr_width,unsigned short height,uint8* outbuf,unsigned short des_w,unsigned short des_h);
//double size and rotate
int  zoom_twice_rotate_clockwise(uint8* inbuf, unsigned short width, unsigned short extr_width,unsigned short height,uint8* outbuf,uint8* tempRotateBuf, unsigned short des_w,unsigned short des_h);
//only rotate
int  rotate_clockwise(unsigned char* inbuf, unsigned short width, unsigned short extr_width,unsigned short height,uint8* outbuf);

// for yuv 2 rgb565
int InitColorDither ();

void Color16DitherImage (char *InY, char *InU, char *InV, char* Out, short width, short height);

void Color16DitherImageIntoLcdBuf (char *InY, char *InU, char *InV, short InExtrWidth, short* LcdBuf, short LcdBufWidth, short StartX, short StartY, short width, short height);

#endif
#endif


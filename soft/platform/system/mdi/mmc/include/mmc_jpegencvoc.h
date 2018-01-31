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


#ifndef  MMC_JPEGENCVOC
#define  MMC_JPEGENCVOC

void JPEGENCIsr(void);
boolean getJpegEncodeFinish(void);
INT32 initVocJpegEncode(void);
void quitVocJpegEncode(void);
uint32 getJpegEncodeLen(void);
void  MMC_jpgEn (UINT16 srcw,  // width of source
                 UINT16 srch,        // height of source
                 UINT16 imgw,      // width of the jpeg image
                 UINT16 imgh,       // height of the jpeg image
                 UINT16 quality,    // quality of the jpeg image
                 char *bufin,         // buffer of the source
                 char *bufout,       // buffer of the jpeg image
                 UINT16 yuvmode  // 1 - YUV, 0 - RGB565
                );

#endif


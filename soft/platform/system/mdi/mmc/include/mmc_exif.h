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

#ifndef AP_EXIF_H
#define AP_EXIF_H

#include "cs_types.h"
#define STRIPCOUNTMAX           1  //only support thumbnail nocompression
#define EXIFHEADLENGTH          10

/*DEFINE TAG VALUE*/
#define IMAGEWIDTH                  0x0100
#define IMAGELENGTH                 0x0101
#define MAKE                        0x010F
#define MODEL                       0x0110
#define ORIENTATION                 0x0112
#define DATETIME                    0x0132
#define EXIFIFDPOINTER              0x8769
#define EXIFDATETIME                0x9003
#define EXIFVERSION                 0x9000
#define APERTURE                    0x9202
#define EXPOSUREBIAS                0x9204

typedef struct
{
    uint8   Make[26];
    uint8   Model[30];
    uint8   DateTime[20];
    uint32  ImageOffset;
    uint16  ImageWidth;
    uint16  ImageHeight;
    uint16  Orientation;
    uint16  ExifVersion;
    uint32  Aperture_1;
    uint32  Aperture_2;
    uint32  ExposureBiasValue1;
    uint32  ExposureBiasValue2;
    uint32  FileSize;
} EXIFINFO;

/* FUNCTION  */
int32  Ap_GetImageInfo(uint8* FileBuf);//(uint32 FileIndex)
int Ap_ParseJPEGFile(uint8 *buf);
int Ap_ParseExifMarker(uint32 Offset);
int Ap_ParseSofMarker(uint32 Offset);
int Ap_ParseImageExif(uint32 APPOffset);
int Ap_ParseExifIFD(uint32 IFDOffset, uint32 *NextOffset);
int Ap_ParseTagInfo(uint32 TagOffset);
int Ap_CheckSOFInfo(uint8* StartOfFrame);
uint16 GetWord( uint8 *buf, uint8 ucBytesOrder);
uint32 GetDword( uint8 *buf, uint8 ucBytesOrder);
void Ap_ParseFileInit(void);
void Ap_ParseFileEnd(void);

#endif//AP_EXIF_H

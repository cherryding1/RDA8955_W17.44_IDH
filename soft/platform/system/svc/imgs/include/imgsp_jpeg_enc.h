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



#ifndef _IMGSP_JPEG_ENC_H_
#define _IMGSP_JPEG_ENC_H_

//#define SHEEN_VC_DEBUG

#ifndef SHEEN_VC_DEBUG
#include "imgs_m.h"
#endif
//#include "imgsp_jpeg_dec.h"
#ifdef SHEEN_VC_DEBUG
#define PRIVATE static
#define PROTECTED
#define CONST const
#define INT16 short
#define UINT16 unsigned short
#define INT32 int
#define UINT32 unsigned int
#define INT8 char
#define UINT8 unsigned char
#define CHAR char
#define VOID void

typedef enum
{
    IMGS_ERR_NO,
    IMGS_ERR_UNSUPPORTED_FORMAT,
    IMGS_ERR_OUT_OF_MEMORY,
    IMGS_ERR_FILE,
    IMGS_ERR_FILE_OPEN,
    IMGS_ERR_FILE_CREATE,
    IMGS_ERR_FILE_CLOSE,
    IMGS_ERR_FUNC_FORMAT,
    IMGS_ERR_WRONG_DECOMPOSE ,
    IMGS_ERR_WRONG_SIZE,
    IMGS_ERR_BIG_SIZE,
    IMGS_ERR_WRONG_CASE,
    IMGS_ERR_WRONG_HEAD,
    IMGS_ERR_MALLOC,
    IMGS_ERR_INPUT_PARAMETERS,
    /// ?
    IMGS_ERR_ISP_DATA,
    IMGS_ERR_ISP_STATE,

    /// Legacy error ?
    IMGS_ERR_VLC,

    IMGS_ERR_QTY
} IMGS_ERR_T;

#define IMGS_TRACE()
#define sxr_Malloc malloc
#define FS_Write(a,b,c) fwrite(b,1,c,a)
#define FS_Close fclose

#endif


#define CHUNK_SOI       0xffd8
#define CHUNK_EOI       0xffd9

#define CHUNK_JPG0      0xf0
#define CHUNK_JPG13     0xfd
#define CHUNK_DHP       0xde
#define CHUNK_EXP       0xdf
#define CHUNK_DNL       0xdc
#define CHUNK_DAC       0xcc
#define CHUNK_SOF3      0xc3
#define CHUNK_SOF5      0xc5
#define CHUNK_SOF6      0xc6
#define CHUNK_SOF7      0xc7
#define CHUNK_SOF9      0xc9
#define CHUNK_SOF10     0xca
#define CHUNK_SOF11     0xcb
#define CHUNK_SOF13     0xcd
#define CHUNK_SOF14     0xce
#define CHUNK_SOF15     0xcf
#define CHUNK_APP0      0xe0
#define CHUNK_APP1      0xe1
#define CHUNK_APP2      0xe2
#define CHUNK_APP3      0xe3
#define CHUNK_APP4      0xe4
#define CHUNK_APP5      0xe5
#define CHUNK_APP6      0xe6
#define CHUNK_APP7      0xe7
#define CHUNK_APP8      0xe8
#define CHUNK_APP9      0xe9
#define CHUNK_APP10     0xea
#define CHUNK_APP11     0xeb
#define CHUNK_APP12     0xec
#define CHUNK_APP13     0xed
#define CHUNK_APP14     0xee
#define CHUNK_APP15     0xef

#define CHUNK_TEM       0x01
#define CHUNK_RST0      0xffd0
#define CHUNK_RST1      0xffd1
#define CHUNK_RST2      0xffd2
#define CHUNK_RST3      0xffd3
#define CHUNK_RST4      0xffd4
#define CHUNK_RST5      0xffd5
#define CHUNK_RST6      0xffd6
#define CHUNK_RST7      0xffd7

#define CHUNK_JPG       0xc8

#define CHUNK_SOF0      0xc0
#define CHUNK_SOF1      0xc1
#define CHUNK_SOF2      0xc2
#define CHUNK_DHT       0xc4
#define CHUNK_SOS       0xda
#define CHUNK_DQT       0xdb
#define CHUNK_DRI       0xdd
#define CHUNK_COM       0xfe

#define SOF0_DEFINED        0x01
#define DHT_DEFINED     0x02
#define SOS_DEFINED     0x04
#define DQT_DEFINED     0x08
#define APP0_DEFINED        0x10
#define APP1_DEFINED        0x20
#define DRI_DEFINED     0x40
#define IS_PROGRESSIVE      0x80

#define JFIF_OK         0x1F
#define EXIF_OK         0x2F

#define LUMINANCE       0
#define CHROMINANCE     1

#define PASS_WRITE      0
#define PASS_COMPUTE_HUFFMAN    1


#define  TEMPOUTBUFSIZE      512*20


#define FIX_1_082392200  ((INT32)  277)     // FIX(1.082392200) 
#define FIX_1_414213562  ((INT32)  362)     // FIX(1.414213562) 
#define FIX_2_613125930  ((INT32)  669)     // FIX(2.613125930) 
#define FIX_1_84775906555  ((INT32)  473)   // FIX(1.847759065) 
//low idct
#define FIX_0_298631336  ((INT32)  2446)    // FIX(0.298631336) 
#define FIX_0_390180644  ((INT32)  3196)    // FIX(0.390180644) 
#define FIX_0_541196100  ((INT32)  4433)    // FIX(0.541196100) 
#define FIX_0_765366865  ((INT32)  6270)    // FIX(0.765366865) 
#define FIX_0_899976223  ((INT32)  7373)    // FIX(0.899976223) 
#define FIX_1_175875602  ((INT32)  9633)    // FIX(1.175875602) 
#define FIX_1_501321110  ((INT32)  12299)   // FIX(1.501321110) 
#define FIX_1_961570560  ((INT32)  16069)   // FIX(1.961570560) 
#define FIX_2_053119869  ((INT32)  16819)   // FIX(2.053119869) 
#define FIX_2_562915447  ((INT32)  20995)   // FIX(2.562915447) 
#define FIX_3_072711026  ((INT32)  25172)   // FIX(3.072711026) 
#define FIX_0_211164243  ((INT32)  1730)    // FIX(0.211164243) 
#define FIX_0_509795579  ((INT32)  4176)    // FIX(0.509795579) 
#define FIX_0_601344887  ((INT32)  4926)    // FIX(0.601344887) 
#define FIX_0_720959822  ((INT32)  5906)    // FIX(0.720959822) 
#define FIX_0_765366865  ((INT32)  6270)    // FIX(0.765366865) 
#define FIX_0_850430095  ((INT32)  6967)    // FIX(0.850430095) 
#define FIX_0_899976223  ((INT32)  7373)    // FIX(0.899976223) 
#define FIX_1_061594337  ((INT32)  8697)    // FIX(1.061594337) 
#define FIX_1_272758580  ((INT32)  10426)   // FIX(1.272758580) 
#define FIX_1_451774981  ((INT32)  11893)   // FIX(1.451774981) 
#define FIX_1_847759065  ((INT32)  15137)   // FIX(1.847759065) 
#define FIX_2_172734803  ((INT32)  17799)   // FIX(2.172734803) 
#define FIX_2_562915447  ((INT32)  20995)   // FIX(2.562915447) 
#define FIX_3_624509785  ((INT32)  29692)   // FIX(3.624509785) 



#ifndef SHEEN_VC_DEBUG
PROTECTED IMGS_ERR_T  imgs_JpegEncode(IMGS_FBW_T* inFbw, PCSTR filename,
                                      IMGS_ENCODING_PARAMS_T* encParams,
                                      IMGS_ENC_CALLBACK_T     callback);
#endif

IMGS_ERR_T  imgs_JpegEncodeBufYUV422(UINT16* bufIn, UINT16 quality, UINT16 imgWidth, UINT16 imgHeight, UINT8* bufOut, UINT32* outSize);



#endif // _IMGSP_JPEG_ENC_H_


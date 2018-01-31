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



#ifndef _IMGSP_JPEG_DEC_H_
#define _IMGSP_JPEG_DEC_H_

#include "imgs_m.h"

////////////jpeg header marker
#define   M_SOF0   0xc0
#define   M_SOF1   0xc1
#define   M_SOF2   0xc2
#define   M_SOF3   0xc3
#define   M_DHT    0xc4
#define   M_EOI    0xd9
#define   M_SOS    0xda
#define   M_DQT    0xdb
#define   M_DRI    0xdd
#define   M_APP0   0xe0
#define   Q15      16384//32768
#define   Q14      8192//16384
#define   S15      14// 15

#define DCTELEM         INT32

#define   TEMPBUFSIZE    50000
#define HUFF_LOOKAHEAD  8
#define DCTSIZE         8
#define DCTSIZE2        64

#define CONST_BITS  13
#define PASS1_BITS  2
#define MAXJSAMPLE  255
#define CENTERJSAMPLE   128
#define RANGE_MASK  (MAXJSAMPLE * 4 + 3)
//fast idct
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


#define RIGHT_SHIFT(x,shft) ((x) >> (shft))
#define IRIGHT_SHIFT(x,shft)    ((x) >> (shft))
#define IDESCALE(x,n)  ((INT32) IRIGHT_SHIFT(x, n))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (1 << ((n)-1)), n)
#define MULTIPLY(var,const)  ((var) * (const))
#define MULTIPLY8(var,const)  ((DCTELEM) DESCALE((var) * (const), 8))
#define DEQUANTIZE(coef,quantval)  (( (coef)) * (quantval))



#define MAKEAUINT16(a,b) (a)|((b) << 8)
#define DRV_RGB2PIXEL565(r,g,b) \
((( (r) & 0xf8) << 8) | (( (g) & 0xfc) << 3) | (( (b) & 0xf8) >> 3))

typedef struct
{
    INT16  width;
    INT16  height;
    INT32    len;
    INT32    headlen;
} DEC_UNIT;
EXPORT DEC_UNIT  decUnit;



// =============================================================================
// imgs_JpegDecode
// -----------------------------------------------------------------------------
/// Decode an encoded image, previously dumped into a buffer by #imgs_OpenImage,
/// into a frame buffer window passed as a parameter. The frame buffer window
/// data buffer is allocated by the #imgs_JpegDecode function, and contains
/// the full resolution image. Its region of interest is the full frame buffer
/// window.
/// @param fileName Encoded image. Can be either a file (kind=1) or a buffer
/// (kind = 0);
/// @param fbw Output frame buffer window.
/// @param callback User handler called at the end of the conversion.
/// @param kind of image: If 0, fileName is a pointer to a buffer holding the
/// image data. If kind = 1,  fileName is the name of a file to decode.
/// @return #IMGS_ERR_WRONG_HEAD, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_WRONG_CASE
/// or #IMGS_ERR_NO.
// =============================================================================
PROTECTED IMGS_ERR_T imgs_JpegDecode(PCSTR fileName, IMGS_FBW_T* fbw,
                                     IMGS_DEC_CALLBACK_T callback,UINT32 kind);



#endif // _IMGSP_JPEG_DEC_H_


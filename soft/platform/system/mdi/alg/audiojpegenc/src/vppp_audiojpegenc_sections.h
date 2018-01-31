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

#ifndef VPPP_AUDIOJPEGENC_SECTIONS_H
#define VPPP_AUDIOJPEGENC_SECTIONS_H


// all size and laddr defines are given in word size

#define RAM_I_START               (RAM_I_MIN/2)
#define RAM_I_END                 (RAM_I_MAX/2)

#define SECTION_1_START ( 0x100/2 + RAM_I_START)



#define SECTION_AAC_MP3_COMMON_SIZE         (SECTION_1_START       - RAM_I_START)

#define SECTION_MP3_ENC_SIZE                2000//(RAM_I_END       - SECTION_1_START) 
#define SECTION_AMR_ENC_SIZE                4400//(RAM_I_END       - SECTION_1_START) 

#define SECTION_RGBYUV_ROTATE_SIZE  2000


// start of different image sections in extern RAM


#define AAC_MP3_COMMON_START          RAM_I_START
#define MP3_DEC_START                (RAM_I_START+SECTION_AAC_MP3_COMMON_SIZE)
#define AMR_DEC_START                (RAM_I_START+SECTION_AAC_MP3_COMMON_SIZE)
#define RGBYUV_ROTATE_START          (RAM_I_START+SECTION_AAC_MP3_COMMON_SIZE)

#endif//VPPP_AUDIOJPEGENC_SECTIONS_H
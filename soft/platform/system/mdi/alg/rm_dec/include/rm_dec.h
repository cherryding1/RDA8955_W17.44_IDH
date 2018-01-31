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



#ifndef RM_DEC_H
#define RM_DEC_H

#include "helix_types.h"
#include "helix_result.h"
#include "codec_defines.h"

typedef struct
{
    UINT32 rm_Duration;
    UINT16 rm_NumStream;

    UINT32 rv_Format;
    UINT32 rv_Width;
    UINT32 rv_Height;
    UINT32 rv_Fps;

    UINT32 ra_Format;
    UINT32 ra_SampleRate;
    UINT16 ra_BitsPerSample;
    UINT16 ra_NumChannels;

    UINT32 ra_BitsPerFrame;//bits per code frame. for cook.
    UINT16 ra_SamplePerFrame;//samples per code frame(include each channel).for cook. 1ch(256,512,1024),2ch(512,1024,2048)
    UINT16 ra_Regions;//for cook
    UINT16 ra_cplStart;//for cook
    UINT16 ra_cplQBits;//for cook
} rmInfo;//add sheen


HX_RESULT rm_init(FILE_HDL fpIn, rmInfo *pRmInfo);
HX_RESULT rm_destroy(FILE_HDL fpIn);
HX_RESULT rm_seek( UINT32 ulTime);
HX_RESULT rm_seek_video( UINT32 ulTime);
HX_RESULT rm_getOneFrameVideo(INT8* pOut, /*VocWorkStruct *pVocState,*/ INT8 skipBFlag);
UINT32 rm_getAudioData(char *pOutBuf, UINT32 readLen);
UINT32 rm_getKeyTimeStamp( VOID );
UINT32 rm_getNextKeyTimeStamp(UINT32 ulTime);
UINT32 rm_getLastFrameTimeStamp(VOID);
UINT32 rm_getAudioTimeStampStart(VOID);











#endif


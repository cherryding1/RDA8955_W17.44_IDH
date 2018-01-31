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

#ifndef __RECORDFMR_H__
#define __RECORDFMR_H__

#include "cs_types.h"
#include "vpp_audiojpegenc.h"

#define FM_MP3ENC_PCM_INPUT_BUFSIZE  (22*1152*2)//16 frames (mono)
#define FM_MP3ENC_RESAMPLE_OUTSIZE   (1152*4*2)//pingpong buffer
#define FM_MP3ENC_SAMPLE_RATE_N 32000
#define FM_MP3ENC_OUTPUT_FRAME_SIZE 1000//byte

#define FM_WAVENC_PCM_INPUT_BUFSIZE   (22*1024*2)
#define FM_WAVENC_RESAMPLE_OUTSIZE    (1152*4)

typedef enum
{
    MSG_MMC_FMRENC_VOC_INT,
    MSG_MMC_FMRENC_PCM_INT,
} FMRENC_USER_MSG;


extern int32 FmrRecordStart(int32 volume, int32 play_style, char * filename);

extern int32 FmrRecordStop (void) ;

extern int32 FmrRecordPause (void)  ;

extern int32 FmrRecordResume (void) ;

extern int32 FmrRecordUserMsg(int32 nMsg);

#endif//__RECORDFMR_H__


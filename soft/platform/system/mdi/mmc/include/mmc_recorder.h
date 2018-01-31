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

#ifndef __RECORDMP3_H__
#define __RECORDMP3_H__

#include "cs_types.h"
#include "vpp_audiojpegenc.h"

#define FRAME_NUM 11
#define MP3ENC_INPUT_PCMSIZE        2*288*FRAME_NUM//6336 byte
#define MP3ENC_INPUT_BUFSIZE        2*1152*FRAME_NUM //25344 byte
#define MP3ENC_INPUT_FRAMESIZE      2*1152

#define MP3ENC_INPUT_ONEFRAME_SIZE  2*1152//2304 BYTE
#define MP3ENC_SAMPLE_RATE_N 32000
#define ENCODER_OUTPUT_FRAME_BUF_MAX_SIZE 1000//byte

//define local msg
#define MSG_MMC_AUDIOENC_VOC_INT 1

extern int32 RecordStart(int32 volume, int32 play_style, char * filename);

extern int32 RecordStop (void) ;

extern int32 RecordPause (void) ;

extern int32 RecordResume (void) ;

extern int32 RecordSample (char *pSample, int32 nSample);

extern int32 RecordUserMsg(int32 nMsg);

#endif//__RECORDMP3_H__



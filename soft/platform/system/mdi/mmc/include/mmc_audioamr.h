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

#ifndef MMC_AUDIOAMR_H
#define MMC_AUDIOAMR_H


#include "mmc.h"


typedef enum
{
    MSG_MMC_AMRDEC_VOC,
    MSG_MMC_AMRDEC_PCM,
} AMRDEC_USER_MSG;

typedef struct AMR_DECODER_INPUT
{
    int32 fileHandle;
    uint32 fileOffset;
    uint8 *data;
    uint8 *bof;
    uint8 *eof;
    uint32 length;
    uint32 readOffset;
    uint8 inFlag;
    uint8 is_first_frame;
} AMR_INPUT;

typedef struct AMR_DECODER_OUTPUT
{
    int16 *data;
    uint32 length;
    uint8 channels;
    uint8 rPingpFlag;
    uint8 wPingpFlag;
    uint8 outFlag;
    PCMSETTING pcm;
    int32 OutputPath;
} AMR_OUTPUT;

typedef struct AMR_DECODER
{
    AMR_INPUT  AmrInput;
    AMR_OUTPUT  AmrOutput;
    Codecmode mode;

} AMR_PLAY;






int32 Audio_AmrPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_AmrStop (void);

int32 Audio_AmrPause (void);

int32 Audio_AmrResume (HANDLE fhd);

int32 Audio_AmrGetID3 (char * pFileName);

int32 Audio_AmrUserMsg(int32 nMsg);

int32 Audio_AmrGetPlayInformation (MCI_PlayInf * MCI_PlayInfAMR);

#endif


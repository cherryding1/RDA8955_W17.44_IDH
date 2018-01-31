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

#ifndef AUDIO_API_H
#define AUDIO_API_H

#include "cs_types.h"
#include "mmc_audiompeg.h"
#include "mci.h"



uint32 LILY_AudioPlay (int32 OutputPath, HANDLE fileHandle, int16 fielType,INT32 PlayProgress);     //  MCI_MEDIA_PLAY_REQ,

uint32 LILY_AudioPause(void);

uint32 LILY_AudioResume(HANDLE fileHandle);

uint32 LILY_AudioSeek(int32 time);

uint32 LILY_AudioStop(void);

//uint32 LILY_AudioGetID3(int8 *pfilename,ID3INFO *id3info) ;   //  MCI_MEDIA_PLAY_REQ,

uint32 LILY_AudioSetEQ(AUDIO_EQ     EQMode);

uint32 LILY_AudioGetPlayInformation(MCI_PlayInf* PlayInformation);


#endif


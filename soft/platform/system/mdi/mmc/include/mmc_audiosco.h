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

#include "mci.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc_adpcm.h"

#ifndef MMC_AUDIOSCO_H
#define MMC_AUDIOSCO_H

#define MSG_MMC_SCO_PCM_INT 10
#define MSG_MMC_SCO_PCM_END 20
#define MSG_MMC_SCO_MIC_INT 30
#define MSG_MMC_SCO_DATA_IND MSG_MMC_AUDIODEC_SCO

typedef struct SCO_INPUT  // struct for record data to send by sco
{
    uint16 sco_handle;
    uint16 *data;          // pcm record buff
    uint16 length;       // valid data in pcm buff, in pcm sample
    uint16 size;          // size of data, in pcm sample
} SCO_INPUT;

typedef struct SCO_OUTPUT  // struct for play received sco data
{
    uint16 *Buffer;     //Pointer for PCM buffer
    uint16 length;       // valid data in pcm buff, in pcm sample
    uint16 Size;        //Size of PCM buffer, in pcm sample
} SCO_OUTPUT;

typedef struct SCO_PLAY
{
    uint8 in_flag;
    uint8 packet_size;
    SCO_INPUT  SCOInput;
    SCO_OUTPUT  SCOOutput;
//  vpp_AudioJpeg_DEC_OUT_T Voc_AudioDecStatus;
//  vpp_AudioJpeg_DEC_IN_T Voc_AudioDecIN;
} SCO_PLAY;

int32 Audio_SCOPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_SCOStop (void);

int32 Audio_SCOPause (void);

int32 Audio_SCOResume (HANDLE fhd);

int32 Audio_SCOGetID3 (char * pFileName);

int32 Audio_SCOUserMsg(int32 nMsg);

int32 Audio_SCOGetPlayInformation (MCI_PlayInf * MCI_PlayInfSCO);


#endif



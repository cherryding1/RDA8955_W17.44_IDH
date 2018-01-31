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

#ifndef MMC_AUDIO_STREAM_H
#define MMC_AUDIO_STREAM_H
//#include "lily_lcd_api.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"

typedef enum
{
    MSG_MMC_MP3STREAMDEC_VOC,
    MSG_MMC_MP3STREAMDEC_PCM,
} MP3STREAMDEC_USER_MSG;

typedef struct MP3_DECODER_INPUT
{
    uint8 *data;
    uint8 *bof;
    uint32 length;
    uint8 is_first_fill_pcm;
    uint8 is_open_intr;
} MP3_STREAM_INPUT;

typedef struct MP3_DECODER_OUTPUT
{
    int16 *data;
    uint32 length;
    uint8 channels;
    uint8 rPingpFlag;
    uint8 wPingpFlag;
    uint8 outFlag;
    PCMSETTING pcm;
    int32 OutputPath;
} MP3_STREAM_OUTPUT;



typedef struct MP3_STREAM_DECODER
{
    MP3_STREAM_INPUT  Mp3StreamInput;
    MP3_STREAM_OUTPUT  Mp3StreamOutput;
    Codecmode mode;
    uint8 loop;
    vpp_AudioJpeg_DEC_OUT_T Voc_Mp3StreamDecStatus;
    vpp_AudioJpeg_DEC_IN_T Voc_Mp3StreamDecIN;
} MP3_STREAM_PLAY;

int32 AudioMp3StartStream(uint32 *pBuffer, uint32 len, uint8 loop);
int32 AudioMp3StopStream(void);
int32 Audio_Mp3StreamUserMsg(int32 nMsg);

#endif


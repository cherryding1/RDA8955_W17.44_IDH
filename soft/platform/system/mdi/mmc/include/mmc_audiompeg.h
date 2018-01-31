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

#ifndef MMC_AUDIO_H
#define MMC_AUDIO_H

//#include "lily_lcd_api.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"
#include "mci.h"




typedef struct ID3INFO
{


} ID3INFO;
typedef struct MPEG_FILE{
    bool EndErrorFlag;
    bool buffer_flag;
    bool EndofFileFlag;
    bool ReadFileFlag;
    bool ReadFileSize;
    bool MemMoveFlag;
    bool ReadFileError;
    UINT32 ReadFileCounter;
    
}MPEG_FILE_OPTION;

typedef struct MPEG_DECODER_INPUT
{
    int32 fileHandle;
    uint32 fileOffset;
    uint8 *data;
    uint8 *bof;
    uint8 *eof;
    uint32 length;
    uint32 readOffset;
    uint8 is_first_frame;
    SSHdl *ss_handle; // for A2dp
    bt_a2dp_audio_cap_struct *audio_config_handle;
    bool  bt_sco;      //for audio over bt sco
    bool  pcmflag;
    MPEG_FILE_OPTION *MpegStreamRead;
} MPEG_INPUT;

typedef struct MPEG_DECODER_OUTPUT
{
    int16 *data;
    uint32 length;
    uint8 channels;
    uint8 rPingpFlag;
    uint8 wPingpFlag;
    uint8 StartAif;
    PCMSETTING pcm;
    int32 OutputPath;
  
} MPEG_OUTPUT;

typedef struct MPEG_DECODER
{
    MPEG_INPUT  MpegInput;
    MPEG_OUTPUT  MpegOutput;
    Codecmode mode;
    vpp_AudioJpeg_DEC_OUT_T Voc_AudioDecStatus;
    vpp_AudioJpeg_DEC_IN_T Voc_AudioDecIN;
} MPEG_PLAY;

//for sbc encoder  zouying
#define SBC_FREQ_16000      0x00
#define SBC_FREQ_32000      0x01
#define SBC_FREQ_44100      0x02
#define SBC_FREQ_48000      0x03

/* channel mode */
#define SBC_MODE_MONO       0x00
#define SBC_MODE_DUAL_CHANNEL   0x01
#define SBC_MODE_STEREO     0x02
#define SBC_MODE_JOINT_STEREO   0x03

/* allocation method */
#define SBC_AM_LOUDNESS     0x00
#define SBC_AM_SNR          0x01

#define SBC_BIT_POOL        32
///////////////////////////////////////
int32 Audio_MpegPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_MpegStop (void);

int32 Audio_MpegPause (void);

int32 Audio_MpegResume (HANDLE fhd);

int32 Audio_MpegGetID3 (char * pFileName);

int32 Audio_MpegUserMsg(int32 nMsg);

int32 Audio_MpegGetPlayInformation (MCI_PlayInf * MCI_PlayInfMPEG) ;
#endif


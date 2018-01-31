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

#ifndef MMC_AUDIOWAV_H
#define MMC_AUDIOWAV_H

#define MSG_MMC_WAV_PCM_INT 1
#define MSG_MMC_WAV_PCM_END 2
#define MSG_MMC_WAV_SBC_VOC 3

#define WAV_DATA_DEPTH_8BIT  8
#define WAV_DATA_DEPTH_16BIT 16



#define WAV_INPUT_BUFFER_PADDING_SIZE 8

#define WAV_ADPCM_TMP_BUFSIZE     (2*1024)

typedef struct WAV_INPUT
{
    HANDLE fileHandle;
    int32 fileTotalLen;
    uint32 fileOffset;
    uint8 *data;
    uint8 *bof;
    uint8 *eof;

    uint32 length;
    uint8 is_first_frame;
    uint16 codec;
    enum AdpcmCodecID codec_id; //The follow members are defined for ADPCM decoder
    uint8   channels;
    uint32 block_align; /*  number of bytes per packet if constant and known or 0  Used by some WAV based audio codecs. */
    uint8 *extradata;
    uint32 extradata_size;
    void   *priv_data;
} WAV_INPUT;

typedef struct WAV_OUTPUT
{
    uint16 SampleRate;  //48k,44.1k,32k,22.05k,24k,16k,12k,11.025k,8k
    uint8  Bits;        //16bit or 8bit
    uint8  Channel;     //stero or mono
    uint32 *Buffer;     //Pointer for PCM buffer
    uint32 *Buffer_head;
    uint8 *write;        // used for ADPCM
    uint8 *tmpBuffer;
    uint8 *tmpBuf_read;
    int32  Remain_Size;
    uint32 Size;        //Size of PCM buffer  len-by-int
    void (*OnPCM)(int32 half);//PCM1 handler
    int32 OutputPath;
} WAV_OUTPUT;

typedef struct WAV_PLAY
{
    WAV_INPUT  WavInput;
    WAV_OUTPUT  WavOutput;
    vpp_AudioJpeg_DEC_OUT_T Voc_AudioDecStatus;
    vpp_AudioJpeg_DEC_IN_T Voc_AudioDecIN;
} WAV_PLAY;

typedef struct
{
    uint8       szRiff[4];          /* "RIFF" */
    uint32      dwFileSize;         /* file size */
    uint8       szWaveFmt[8];       /* "WAVEfmt " */
    uint32      dwFmtSize;          /* 16 */
    uint16        wFormatTag;         /* format type */
    uint16        nChannels;          /* number of channels (i.e. mono, stereo...) */
    uint32       nSamplesPerSec;     /* sample rate */
    uint32       nAvgBytesPerSec;    /* for buffer estimation */
    uint16        nBlockAlign;        /* block size of data */
    uint16        wBitsPerSample;     /* number of bits per sample of mono data */
} WAVE_HEADER;
#define SIZE_WAVE_HEADER sizeof(WAVE_HEADER)

//for adpcm.sheen
typedef struct
{
    uint8       szRiff[4];          /* "RIFF" */
    uint32      dwFileSize;         /* file size */
    uint8       szWaveFmt[8];       /* "WAVEfmt " */
    uint32      dwFmtSize;          /* 20 before 'fact' */
    uint16        wFormatTag;         /* format type */
    uint16        nChannels;          /* number of channels (i.e. mono, stereo...) */
    uint32       nSamplesPerSec;     /* sample rate */
    uint32       nAvgBytesPerSec;    /* for buffer estimation */
    uint16        nBlockAlign;        /* block size of data */
    uint16        wBitsPerSample;     /* number of bits per sample of mono data */
    uint16        cbSize;
    uint16	  samplesPerBlock;
    uint8         fact[4]; //="fact";
    uint16        factsize_low; //=4;
    uint16        factsize_hi; //=0;
    uint16        sample_low;
    uint16        sample_hi;
} WAVE_HEADER_EX;
#define SIZE_WAVE_HEADER_EX sizeof(WAVE_HEADER_EX)

typedef struct
{
    uint8       szData[4];          /* "data" */
    uint32       dwDataSize;         /*pcm data size*/
} WAVE_DATA_HEADER;
#define SIZE_WAVE_DATA_HEADER sizeof(WAVE_DATA_HEADER)

typedef struct
{
    uint16        cbSize;
    uint8         fact[4]; //="fact";
    uint16        factsize_low; //=4;
    uint16        factsize_hi; //=0;
    uint16        sample_low;
    uint16        sample_hi;
    uint8         szData[4];            /* "data" */
    uint16        dwDataSize_low;         /*pcm data size*/
    uint16        dwDataSize_hi;         /*pcm data size*/
} WAVE_DATA_HEADER_EX;

#define SIZE_WAVE_DATA_HEADER_EX sizeof(WAVE_DATA_HEADER_EX)

int32 Audio_WavPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress);

int32 Audio_WavStop (void);

int32 Audio_WavPause (void);

int32 Audio_WavResume (HANDLE fhd);

int32 Audio_WavGetID3 (char * pFileName);

int32 Audio_WavUserMsg(int32 nMsg);

int32 Audio_WavGetPlayInformation (MCI_PlayInf * MCI_PlayInfWAV);
PUBLIC MCI_ERR_T MMC_WavGetFileInformation ( CONST INT32 FileHander,  AudDesInfoStruct  * CONST DecInfo  );

int adpcm_decode_init(WAV_INPUT * input );

int adpcm_decode_frame(WAV_INPUT *avctx,
                       void *data, int *data_size,
                       const uint8 *buf, int buf_size);

#endif



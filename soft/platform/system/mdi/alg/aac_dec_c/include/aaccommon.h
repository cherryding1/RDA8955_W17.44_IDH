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





#ifndef _AACCOMMON_H
#define _AACCOMMON_H

#include "aacdec.h"
// #include "statname.h"

/* 12-bit syncword */
#define SYNCWORDH           0xff
#define SYNCWORDL           0xf0

#define MAX_NCHANS_ELEM     2   /* max number of channels in any single bitstream element (SCE,CPE,CCE,LFE) */

#define ADTS_HEADER_BYTES   7
#define NUM_SAMPLE_RATES    12
#define NUM_DEF_CHAN_MAPS   8
#define NUM_ELEMENTS        8
#define MAX_NUM_PCE_ADIF    16

#define MAX_WIN_GROUPS      8
#define MAX_SFB_SHORT       15
#define MAX_SF_BANDS        (MAX_SFB_SHORT*MAX_WIN_GROUPS)  /* worst case = 15 sfb's * 8 windows for short block */
#define MAX_MS_MASK_BYTES   ((MAX_SF_BANDS + 7) >> 3)
#define MAX_PRED_SFB        41
#define MAX_TNS_FILTERS     8
#define MAX_TNS_COEFS       60
#define MAX_TNS_ORDER       20
#define MAX_PULSES          4
#define MAX_GAIN_BANDS      3
#define MAX_GAIN_WIN        8
#define MAX_GAIN_ADJUST     7

#define NSAMPS_LONG         1024
#define NSAMPS_SHORT        128

#define NUM_SYN_ID_BITS     3
#define NUM_INST_TAG_BITS   4

#define EXT_SBR_DATA        0x0d
#define EXT_SBR_DATA_CRC    0x0e

#define IS_ADIF(p)      ((p)[0] == 'A' && (p)[1] == 'D' && (p)[2] == 'I' && (p)[3] == 'F')
#define GET_ELE_ID(p)   ((AACElementID)(*(p) >> (8-NUM_SYN_ID_BITS)))

/* AAC file format */
enum
{
    AAC_FF_Unknown = 0,     /* should be 0 on init */

    AAC_FF_ADTS = 1,
    AAC_FF_ADIF = 2,
    AAC_FF_RAW =  3

};

/* syntactic element type */
enum
{
    AAC_ID_INVALID = -1,

    AAC_ID_SCE =  0,
    AAC_ID_CPE =  1,
    AAC_ID_CCE =  2,
    AAC_ID_LFE =  3,
    AAC_ID_DSE =  4,
    AAC_ID_PCE =  5,
    AAC_ID_FIL =  6,
    AAC_ID_END =  7
};

typedef struct _AACDecInfo
{
    /* pointers to platform-specific state information */
    void *psInfoBase;   /* baseline MPEG-4 LC decoding */
    void *psInfoSBR;    /* MPEG-4 SBR decoding */

    /* raw decoded data, before rounding to 16-bit PCM (for postprocessing such as SBR) */
    void *rawSampleBuf[AAC_MAX_NCHANS];
    int rawSampleBytes;
    int rawSampleFBits;

    /* fill data (can be used for processing SBR or other extensions) */
    unsigned char *fillBuf;
    int fillCount;
    int fillExtType;

    /* block information */
    int prevBlockID;
    int currBlockID;
    int currInstTag;
    int sbDeinterleaveReqd[MAX_NCHANS_ELEM];
    int adtsBlocksLeft;

    /* user-accessible info */
    int bitRate;
    int nChans;
    int sampRate;
    int profile;
    int format;
    int sbrEnabled;
    int tnsUsed;
    int pnsUsed;
    int frameCount;

} AACDecInfo;

/* decoder functions which must be implemented for each platform */
AACDecInfo *AllocateBuffers(void);
void FreeBuffers(AACDecInfo *aacDecInfo);
void ClearBuffer(void *buf, int nBytes);

int UnpackADTSHeader(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);
int GetADTSChannelMapping(AACDecInfo *aacDecInfo, unsigned char *buf, int bitOffset, int bitsAvail);
int UnpackADIFHeader(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);
int SetRawBlockParams(AACDecInfo *aacDecInfo, int copyLast, int nChans, int sampRate, int profile);
int PrepareRawBlock(AACDecInfo *aacDecInfo);
int FlushCodec(AACDecInfo *aacDecInfo);

int DecodeNextElement(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);
int DecodeNoiselessData(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail, int ch);

int Dequantize(AACDecInfo *aacDecInfo, int ch);
int StereoProcess(AACDecInfo *aacDecInfo);
int DeinterleaveShortBlocks(AACDecInfo *aacDecInfo, int ch);
int PNS(AACDecInfo *aacDecInfo, int ch);
int TNSFilter(AACDecInfo *aacDecInfo, int ch);
int aac_IMDCT(AACDecInfo *aacDecInfo, int ch, int chBase, short *outbuf);

/* SBR specific functions */
int InitSBR(AACDecInfo *aacDecInfo);
void FreeSBR(AACDecInfo *aacDecInfo);
int DecodeSBRBitstream(AACDecInfo *aacDecInfo, int chBase);
int DecodeSBRData(AACDecInfo *aacDecInfo, int chBase, short *outbuf);
int FlushCodecSBR(AACDecInfo *aacDecInfo);

/* aactabs.c - global ROM tables */
extern const int aacSampRateTab[NUM_SAMPLE_RATES];
extern const int predSFBMax[NUM_SAMPLE_RATES];
extern const int channelMapTab[NUM_DEF_CHAN_MAPS];
extern const int elementNumChans[NUM_ELEMENTS];
extern const unsigned char sfBandTotalShort[NUM_SAMPLE_RATES];
extern const unsigned char sfBandTotalLong[NUM_SAMPLE_RATES];
extern const int sfBandTabShortOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabShort[76];
extern const int sfBandTabLongOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabLong[325];
extern const int tnsMaxBandsShortOffset[AAC_NUM_PROFILES];
extern const unsigned char tnsMaxBandsShort[2*NUM_SAMPLE_RATES];
extern const unsigned char tnsMaxOrderShort[AAC_NUM_PROFILES];
extern const int tnsMaxBandsLongOffset[AAC_NUM_PROFILES];
extern const unsigned char tnsMaxBandsLong[2*NUM_SAMPLE_RATES];
extern const unsigned char tnsMaxOrderLong[AAC_NUM_PROFILES];

#endif  /* _AACCOMMON_H */

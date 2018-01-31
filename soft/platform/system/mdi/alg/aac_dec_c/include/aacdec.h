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





#ifndef _AACDEC_H
#define _AACDEC_H



#ifdef __cplusplus
extern "C" {
#endif

/* according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3)
 * max size of input buffer =
 *    6144 bits =  768 bytes per SCE or CCE-I
 *   12288 bits = 1536 bytes per CPE
 *       0 bits =    0 bytes per CCE-D (uses bits from the SCE/CPE/CCE-I it is coupled to)
 */
#ifndef AAC_MAX_NCHANS              /* if max channels isn't set in makefile, */
#define AAC_MAX_NCHANS      2       /* set to default max number of channels  */
#endif
#define AAC_MAX_NSAMPS      1024
#define AAC_MAINBUF_SIZE    (768 * AAC_MAX_NCHANS)

#define AAC_NUM_PROFILES    3
#define AAC_PROFILE_MP      0
#define AAC_PROFILE_LC      1
#define AAC_PROFILE_SSR     2

// #define  HELIX_FEATURE_AUDIO_CODEC_AAC_SBR
/* define these to enable decoder features */
#if defined(HELIX_FEATURE_AUDIO_CODEC_AAC_SBR)
#define AAC_ENABLE_SBR
#endif //  HELIX_FEATURE_AUDIO_CODEC_AAC_SBR.
#define AAC_ENABLE_MPEG4

enum
{
    ERR_AAC_NONE                          =   0,
    ERR_AAC_INDATA_UNDERFLOW              =  -1,
    ERR_AAC_NULL_POINTER                  =  -2,
    ERR_AAC_INVALID_ADTS_HEADER           =  -3,
    ERR_AAC_INVALID_ADIF_HEADER           =  -4,
    ERR_AAC_INVALID_FRAME                 =  -5,
    ERR_AAC_MPEG4_UNSUPPORTED             =  -6,
    ERR_AAC_CHANNEL_MAP                   =  -7,
    ERR_AAC_SYNTAX_ELEMENT                =  -8,

    ERR_AAC_DEQUANT                       =  -9,
    ERR_AAC_STEREO_PROCESS                = -10,
    ERR_AAC_PNS                           = -11,
    ERR_AAC_SHORT_BLOCK_DEINT             = -12,
    ERR_AAC_TNS                           = -13,
    ERR_AAC_IMDCT                         = -14,
    ERR_AAC_NCHANS_TOO_HIGH               = -15,

    ERR_AAC_SBR_INIT                      = -16,
    ERR_AAC_SBR_BITSTREAM                 = -17,
    ERR_AAC_SBR_DATA                      = -18,
    ERR_AAC_SBR_PCM_FORMAT                = -19,
    ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,
    ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,

    ERR_AAC_RAWBLOCK_PARAMS               = -22,

    ERR_AAC_UNKNOWN               = -9999
};

typedef struct _AACFrameInfo
{
    int bitRate;
    int nChans;
    int sampRateCore;
    int sampRateOut;
    int bitsPerSample;
    int outputSamps;
    int profile;
    int tnsUsed;
    int pnsUsed;
} AACFrameInfo;

typedef void *HAACDecoder;

/* public C API */
HAACDecoder AACInitDecoder(void);
void AACFreeDecoder(HAACDecoder hAACDecoder);
int AACDecode(HAACDecoder hAACDecoder, unsigned char **inbuf, int *bytesLeft, short *outbuf);

int AACFindSyncWord(unsigned char *buf, int nBytes);
void AACGetLastFrameInfo(HAACDecoder hAACDecoder, AACFrameInfo *aacFrameInfo);
int AACSetRawBlockParams(HAACDecoder hAACDecoder, int copyLast, AACFrameInfo *aacFrameInfo);
int AACFlushCodec(HAACDecoder hAACDecoder);

#ifdef HELIX_CONFIG_AAC_GENERATE_TRIGTABS_FLOAT
int AACInitTrigtabsFloat(void);
void AACFreeTrigtabsFloat(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _AACDEC_H */

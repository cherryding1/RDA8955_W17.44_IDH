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



#ifndef __SBC_H
#define __SBC_H

#define __BYTE_ORDER 0
#define __LITTLE_ENDIAN 0
#define __BIG_ENDIAN 1

#ifndef _VC_DEBUG_
#include "cs_types.h"
#endif
#ifdef _VC_DEBUG_
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef short int16_t;
#endif
/* sampling frequency */
#define SBC_FREQ_16000      0x00
#define SBC_FREQ_32000      0x01
#define SBC_FREQ_44100      0x02
#define SBC_FREQ_48000      0x03

/* blocks */
#define SBC_BLK_4       0x00
#define SBC_BLK_8       0x01
#define SBC_BLK_12      0x02
#define SBC_BLK_16      0x03

/* channel mode */
#define SBC_MODE_MONO       0x00
#define SBC_MODE_DUAL_CHANNEL   0x01
#define SBC_MODE_STEREO     0x02
#define SBC_MODE_JOINT_STEREO   0x03

/* allocation method */
#define SBC_AM_LOUDNESS     0x00
#define SBC_AM_SNR      0x01

/* subbands */
#define SBC_SB_4        0x00
#define SBC_SB_8        0x01

/* Data endianess */
#define SBC_LE          0x00
#define SBC_BE          0x01

#define   ENCODE_SBC   0
#define   DECODE_SBC   1

#define   MONO2DUAL    1
#define   DUAL2MONO    2

struct resample_struct
{
    double factor;
    unsigned short Nmult;
    unsigned short LpScl;
    unsigned short Nwing;
    unsigned short interpFilt;
    unsigned short Xoff;
    unsigned short NX;
    unsigned short Xp;
    unsigned short Ncreep;
    unsigned int   time;
    unsigned int   flag;
    unsigned int   readlen;
    unsigned int   initreadlen;
    unsigned int   inbuffersize;
    short *imp;
    short *impD;
    short *input;
};
typedef struct resample_struct resample_t;

struct sbc_struct
{
    unsigned long flags;
    unsigned char restart;
    unsigned char frequency;
    unsigned char blocks;
    unsigned char subbands;
    unsigned char mode;
    unsigned char allocation;
    unsigned char bitpool;
    unsigned char endian;
    unsigned char Resample;
#ifndef _VC_DEBUG_
    unsigned char *OutputBuffer;
    unsigned char *extraBuf;
    unsigned int mono2dual; // 0:unused,1:mono->dual,2:dual->mono
    int          ResidualData;
    resample_t   resampleup;
    unsigned int inputlen;
    unsigned int samplerate;
    unsigned int channels;
#endif
    void *priv;
};

typedef struct sbc_struct sbc_t;



#ifdef _VC_DEBUG_
FILE *pcm_file,*scale_factor_file, *bits_file,*enc_sbc_file,*sb_sample_f_file ;
#endif
int sbc_init(sbc_t *sbc, unsigned long flags);
int sbc_reinit(sbc_t *sbc, unsigned long flags);
int sbc_parse(sbc_t *sbc, void *input, int input_len);
int sbc_decode(sbc_t *sbc, void *input, int input_len, void *output,
               int output_len, int *len);
int sbc_encode(sbc_t *sbc, short *input, int *input_len, void *output,
               int output_len, int *written,int *consumelen);
int sbc_get_frame_length(sbc_t *sbc);
int sbc_get_frame_duration(sbc_t *sbc);
int sbc_get_codesize(sbc_t *sbc);
void sbc_finish(sbc_t *sbc);
void  decode(char *infilename, char* outfilename);
unsigned int sbc_enc_cfg (unsigned short channel,sbc_t *sbc);
unsigned int sbc_resample_cfg(INT32 InSampleRate,unsigned char *OutSampleRate,resample_t *resample);

#endif /* __SBC_H */

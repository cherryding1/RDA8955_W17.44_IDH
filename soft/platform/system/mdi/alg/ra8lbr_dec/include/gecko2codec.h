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





#ifndef _GECKO2CODEC_H
#define _GECKO2CODEC_H

#ifdef SHEEN_VC_DEBUG

#if defined(_WIN32) && !defined(_WIN32_WCE)
#
#elif defined(_WIN32) && defined(_WIN32_WCE) && defined(ARM)
#
#elif defined(_WIN32) && defined(WINCE_EMULATOR)
#
#elif defined(ARM_ADS)
#
#elif defined(_SYMBIAN) && defined(__WINS__)
#
#elif defined(__GNUC__) && defined(ARM)
#
#elif defined(__GNUC__) && defined(__i386__)
#
#elif defined(_OPENWAVE)
#
#else
#error No platform defined. See valid options in gecko2codec.h.
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void *HGecko2Decoder;

enum
{
    ERR_GECKO2_NONE =                  0,
    ERR_GECKO2_INVALID_SIDEINFO =     -1,

    ERR_GECKO2_UNKNOWN =                  -9999
};

/* public API */
HGecko2Decoder Gecko2InitDecoder(int nSamples, int nChannels, int nRegions, int nFrameBits, int sampRate, int cplStart, int cplQbits, int *codingDelay);
void Gecko2FreeDecoder(HGecko2Decoder hGecko2Decoder);
int Gecko2Decode(HGecko2Decoder hGecko2Decoder, unsigned char *codebuf, int lostflag, short *outbuf);

#ifdef __cplusplus
}
#endif

#endif  /* _GECKO2CODEC_H */


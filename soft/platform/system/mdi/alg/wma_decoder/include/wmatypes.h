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

#ifndef WMATYPES_H
#define WMATYPES_H


/****************************************************************************/

#define UNALIGNED
#include "cs_types.h"
/****************************************************************************/
#ifndef SEEK_SET
#define SEEK_SET         0
#endif /* SEEK_SET */

#ifndef SEEK_CUR
#define SEEK_CUR         1
#endif /* SEEK_CUR */

#ifndef SEEK_END
#define SEEK_END         2
#endif /* SEEK_END */
//#ifndef _BOOL_DEFINED
//#define _BOOL_DEFINED
//typedef long boolean;
//#endif /* _BOOL_DEFINED */

//#ifndef _BYTE_DEFINED
//#define _BYTE_DEFINED
//typedef unsigned char   BYTE;
//#endif /* _BYTE_DEFINED */



//#ifndef _WORD_DEFINED
//#define _WORD_DEFINED
//typedef unsigned short  WORD;
//#endif /* _WORD_DEFINED */

//#ifndef _DWORD_DEFINED
//#define _DWORD_DEFINED
//typedef unsigned long    DWORD;   /* actually long, but this should be okay */
//#endif /* _DWORD_DEFINED */

#ifndef _QWORD_DEFINED
#define _QWORD_DEFINED
typedef  struct tQWORD
{
    DWORD   dwLo;
    DWORD   dwHi;

}  QWORD;

#endif /* _QWORD_DEFINED */


#pragma pack(1)

/****************************************************************************/
#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct
{
    DWORD   Data1;
    WORD    Data2;
    WORD    Data3;
    BYTE    Data4[ 8 ];

} tGUID, GUID;

#endif


/****************************************************************************/
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef  struct tWAVEFORMATEX
{
    WORD    wFormatTag;         /* format type */
    WORD    nChannels;          /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;     /* sample rate */
    DWORD   nAvgBytesPerSec;    /* for buffer estimation */
    WORD    nBlockAlign;        /* block size of data */
    WORD    wBitsPerSample;     /* number of bits per sample of mono data */
    WORD    cbSize;             /* the count in bytes of the size of */

}  WAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;

#endif

#define WAVE_FORMAT_MSAUDIO1  0x0160
typedef  struct
{
    WAVEFORMATEX wfx;
    WORD         wSamplesPerBlock;
    WORD         wEncodeOptions;

} tMSAUDIO1WAVEFORMAT , MSAUDIO1WAVEFORMAT;

#define WAVE_FORMAT_WMAUDIO2  0x0161
typedef  struct
{
    WAVEFORMATEX wfx;
    DWORD        dwSamplesPerBlock;
    WORD         wEncodeOptions;
    DWORD        dwSuperBlockAlign;

} tWMAUDIO2WAVEFORMAT , WMAUDIO2WAVEFORMAT;


/****************************************************************************/
typedef  struct
{
    BYTE    span;
    WORD    virtualPacketLen;
    WORD    virtualChunkLen;
    WORD    silenceLen;
    WORD    silence[1];

} AsfXAcmAudioErrorMaskingData;


/****************************************************************************/
typedef  struct
{
    DWORD maxObjectSize;
    WORD  chunkLen;
    WORD  signatureLen;
    BYTE  signature[1];////should not be modified!!!!

} tAsfXSignatureAudioErrorMaskingData ,  AsfXSignatureAudioErrorMaskingData;

#pragma pack()

#endif  /* WMATYPES_H */


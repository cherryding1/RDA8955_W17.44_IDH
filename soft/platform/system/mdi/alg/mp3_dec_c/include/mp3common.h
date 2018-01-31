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





#ifndef _MP3COMMON_H
#define _MP3COMMON_H

#include "mp3dec.h"
#include "mp3_statname.h"   /* do name-mangling for static linking */

#ifndef SHEEN_VC_DEBUG
#include "hal_debug.h"
#include "mmc.h"
#endif


#define MAX_SCFBD       4       /* max scalefactor bands per channel */
#define NGRANS_MPEG1    2
#define NGRANS_MPEG2    1

/* 11-bit syncword if MPEG 2.5 extensions are enabled */
#define SYNCWORDH       0xff
#define SYNCWORDL       0xe0

/* 12-bit syncword if MPEG 1,2 only are supported
 * #define  SYNCWORDH       0xff
 * #define  SYNCWORDL       0xf0
 */

typedef struct _MP3DecInfo
{
    /* pointers to platform-specific data structures */
    void *FrameHeaderPS;
    void *SideInfoPS;
    void *ScaleFactorInfoPS;
    void *HuffmanInfoPS;
    void *DequantInfoPS;
    void *IMDCTInfoPS;
    void *SubbandInfoPS;

    /* buffer which must be large enough to hold largest possible main_data section */
    unsigned char mainBuf[MAINBUF_SIZE];

    /* special info for "free" bitrate files */
    int freeBitrateFlag;
    int freeBitrateSlots;

    /* user-accessible info */
    int bitrate;
    int nChans;
    int samprate;
    int nGrans;             /* granules per frame */
    int nGranSamps;         /* samples per granule */
    int nSlots;
    int layer;
    MPEGVersion version;

    int mainDataBegin;
    int mainDataBytes;

    int part23Length[MAX_NGRAN][MAX_NCHAN];

} MP3DecInfo;

typedef struct _SFBandTable
{
    short l[23];
    short s[14];
} SFBandTable;

/* decoder functions which must be implemented for each platform */
MP3DecInfo *AllocateBuffers(void);
void FreeBuffers(MP3DecInfo *mp3DecInfo);
int CheckPadBit(MP3DecInfo *mp3DecInfo);
int UnpackFrameHeader(MP3DecInfo *mp3DecInfo, unsigned char *buf);
int UnpackSideInfo(MP3DecInfo *mp3DecInfo, unsigned char *buf);
int DecodeHuffman(MP3DecInfo *mp3DecInfo, unsigned char *buf, int *bitOffset, int huffBlockBits, int gr, int ch);
int Dequantize(MP3DecInfo *mp3DecInfo, int gr);
int IMDCT(MP3DecInfo *mp3DecInfo, int gr, int ch);
int UnpackScaleFactors(MP3DecInfo *mp3DecInfo, unsigned char *buf, int *bitOffset, int bitsAvail, int gr, int ch);
int Subband(MP3DecInfo *mp3DecInfo, short *pcmBuf);

/* mp3tabs.c - global ROM tables */
extern const int samplerateTab[3][3];
extern const short bitrateTab[3][3][15];
extern const short samplesPerFrameTab[3][3];
extern const short bitsPerSlotTab[3];
extern const short sideBytesTab[3][2];
extern const short slotTab[3][3][15];
extern const SFBandTable sfBandTable[3][3];

#endif  /* _MP3COMMON_H */

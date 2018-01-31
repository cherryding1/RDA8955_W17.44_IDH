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





#ifndef _MP3DEC_H
#define _MP3DEC_H


#ifdef __cplusplus
extern "C" {
#endif

/* determining MAINBUF_SIZE:
 *   max mainDataBegin = (2^9 - 1) bytes (since 9-bit offset) = 511
 *   max nSlots (concatenated with mainDataBegin bytes from before) = 1440 - 9 - 4 + 1 = 1428
 *   511 + 1428 = 1939, round up to 1940 (4-byte align)
 */
#define MAINBUF_SIZE    1940

#define MAX_NGRAN       2       /* max granules */
#define MAX_NCHAN       2       /* max channels */
#define MAX_NSAMP       576     /* max samples per channel, per granule */

/* map to 0,1,2 to make table indexing easier */
typedef enum
{
    MPEG1 =  0,
    MPEG2 =  1,
    MPEG25 = 2
} MPEGVersion;

typedef void *HMP3Decoder;

enum
{
    ERR_MP3_NONE =                  0,
    ERR_MP3_INDATA_UNDERFLOW =     -1,
    ERR_MP3_MAINDATA_UNDERFLOW =   -2,
    ERR_MP3_FREE_BITRATE_SYNC =    -3,
    ERR_MP3_OUT_OF_MEMORY =        -4,
    ERR_MP3_NULL_POINTER =         -5,
    ERR_MP3_INVALID_FRAMEHEADER =  -6,
    ERR_MP3_INVALID_SIDEINFO =     -7,
    ERR_MP3_INVALID_SCALEFACT =    -8,
    ERR_MP3_INVALID_HUFFCODES =    -9,
    ERR_MP3_INVALID_DEQUANTIZE =   -10,
    ERR_MP3_INVALID_IMDCT =        -11,
    ERR_MP3_INVALID_SUBBAND =      -12,

    ERR_MP3_UNKNOWN =                  -9999
};

typedef struct _MP3FrameInfo
{
    int bitrate;
    int nChans;
    int samprate;
    int bitsPerSample;
    int outputSamps;
    int layer;
    int version;
} MP3FrameInfo;

/* public API */
HMP3Decoder MP3InitDecoder(void);
void MP3FreeDecoder(HMP3Decoder hMP3Decoder);
int MP3Decode(HMP3Decoder hMP3Decoder, unsigned char **inbuf, int *bytesLeft, short *outbuf, int useSize);

void MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo);
int MP3GetNextFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo, unsigned char *buf);
int MP3FindSyncWord(unsigned char *buf, int nBytes);

#ifdef __cplusplus
}
#endif

#endif  /* _MP3DEC_H */

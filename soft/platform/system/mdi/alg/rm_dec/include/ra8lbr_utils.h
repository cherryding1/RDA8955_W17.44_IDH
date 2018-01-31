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



#ifndef RA8LBR_UTILS_H
#define RA8LBR_UTILS_H

/* Includes */
#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"

/* Defines */
#define GECKO_VERSION                 ((1L<<24)|(0L<<16)|(0L<<8)|(3L))
#define GECKO_MC1_VERSION             ((2L<<24)|(0L<<16)|(0L<<8)|(0L))
#define HX_MC_MAX_NUM_CODECS          5
#define HX_MC_MAX_NUM_CHANNELS        6
#define HX_MC_CHANNEL_FRONT_LEFT      1
#define HX_MC_CHANNEL_FRONT_RIGHT     2
#define HX_MC_CHANNEL_CENTER          4
#define HX_MC_CHANNEL_LOWFREQ         8
#define HX_MC_CHANNEL_SURROUND_LEFT  16
#define HX_MC_CHANNEL_SURROUND_RIGHT 32
#define HX_MC_CHANNEL_CENTER_BACK   256

#define HX_MC_CHANNEL_MONO            4 /* center channel only */
#define HX_MC_CHANNEL_STEREO          3 /* front-left + front-right */

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

/* ra8lbr opaque data struct */
typedef struct ra8lbr_data_struct
{
    INT32   version;
    INT16   nSamples;
    INT16   nRegions;
    INT32   delay;
    UINT16  cplStart;
    UINT16  cplQBits;
    UINT32  channelMask;
} ra8lbr_data;

/* ra8lbr multi-channel per-codec info struct */
typedef struct ra8lbr_mc_codec_struct
{
    UINT32      ulNumChannels;
    UINT32      ulSamplesPerFrameOneChannel;
    UINT32      ulFrameSize;
    UINT32      ulDelayFrames;
    UINT32      ulDelayRemaining;
    ra8lbr_data codecData;
    void*       pDecoder;
    UINT32*     pulChannelMap;
} ra8lbr_mc_codec;

/* ra8lbr multi-channel struct */
typedef struct ra8lbr_mc_struct
{
    UINT32           ulNumCodecs;
    ra8lbr_mc_codec* pCodec;
    UINT16*          pusInterleaveBuffer;
} ra8lbr_mc;

/* ra8lbr decode struct */
typedef struct ra8lbr_decode_struct
{
    void*     pDecoder;
    UINT32    ulNumChannels;
    UINT32    ulChannelMask;
    UINT32    ulFrameSize;
    UINT32    ulFramesPerBlock;
    UINT32    ulSamplesPerFrame;
    UINT32    ulSampleRate;
    UINT32    ulDelayFrames;
    UINT32    ulDelayRemaining;
    UINT32    ulFramesToConceal;
    UCHAR*    pFlushData;
    HXBOOL    bMultiChannel;
    ra8lbr_mc multiChannel;
} ra8lbr_decode;

/*
 * ra8lbr_unpack_opaque_data
 *
 * This unpacks the opaque data for a single ra8lbr codec
 */
HX_RESULT ra8lbr_unpack_opaque_data(ra8lbr_data* pData,
                                    BYTE**       ppBuf,
                                    UINT32*      pulLen);


/* Utility to count the number of bits set */
UINT32 hx_get_num_bits_set(UINT32 ulFlags);

/*
 * ra8lbr_get_channel_map
 *
 * Given the channel mask for all the multi-channel codecs,
 * and the channel mask for a single codec, this routine
 * calculates a "channel map", which is an interleaving
 * offset to map between the output of a single codec
 * into the interleaved output from all codecs.
 */
HX_RESULT ra8lbr_get_channel_map(UINT32  ulChannelMaskAllCodecs,
                                 UINT32  ulChannelMaskThisCodec,
                                 UINT32  ulNumChannelsAllCodecs,
                                 UINT32  ulNumChannelsThisCodec,
                                 UINT32* pulChannelMap);

/*
 * ra8lbr_cleanup_decode_struct
 *
 * This routine cleans up a ra8lbr_decode struct
 */
void ra8lbr_cleanup_decode_struct(ra8lbr_decode*   pDecode,
                                  void*            pUserMem,
                                  rm_free_func_ptr fpFree);


#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef RA8LBR_UTILS_H */

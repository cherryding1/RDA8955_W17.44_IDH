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
























#ifndef __RV30DEC_H__
#define __RV30DEC_H__

#include "rv_backend_types.h"

#ifdef REAL_FORMAT_SDK
#include "rv30backend.h"
#endif /* REAL_FORMAT_SDK */
#include "hxtypes.h"
#include "hxcodec.h"
#include "rvmsg.h"

/*#include "drvxapi.h" */


#if defined(_MSC_VER)
/*///////////////////////////////// */
/* */
/* Begin 1-byte structure alignment */
/* */
/*///////////////////////////////// */
/* RealVideo front end files use 1-byte structure alignment under MSVC, */
/* but the backend is compiled with 8-byte alignment. */
/* This pragma keeps the structure alignment independent of compiler options, */
/* for all structures declared in this file. */
/* 1-byte alignment is now the frontent project default. --karl */
#pragma pack(push, 1)
#endif

/* Byte alignment settings for Mac */
#if defined( _MACINTOSH )
#pragma options align=mac68k
#endif


#ifdef __cplusplus
extern "C" {    /* Assume C declarations for C++ */
#endif /* __cplusplus */

typedef struct tag_H263DecoderInParams
{
    ULONG32 dataLength;
    LONG32  bInterpolateImage;
    ULONG32 numDataSegments;
    HXCODEC_SEGMENTINFO *pDataSegments;
    ULONG32 flags;
    /* 'flags' should be initialized by the front-end before each */
    /* invocation to decompress a frame.  It is not updated by the decoder. */
    /* */
    /* If it contains RV_DECODE_MORE_FRAMES, it informs the decoder */
    /* that it is being called to extract the second or subsequent */
    /* frame that the decoder is emitting for a given input frame. */
    /* The front-end should set this only in response to seeing */
    /* an RV_DECODE_MORE_FRAMES indication in H263DecoderOutParams. */
    /* */
    /* If it contains RV_DECODE_DONT_DRAW, it informs the decoder */
    /* that it should decode the image (in order to produce a valid */
    /* reference frame for subsequent decoding), but that no image */
    /* should be returned.  This provides a "hurry-up" mechanism. */
    ULONG32 timestamp;
} H263DecoderInParams;

typedef struct tag_H263DecoderOutParams
{
    ULONG32 numFrames;
    ULONG32 notes;
    /* 'notes' is assigned by the transform function during each call to */
    /* decompress a frame.  If upon return the notes parameter contains */
    /* the indication RV_DECODE_MORE_FRAMES, then the front-end */
    /* should invoke the decoder again to decompress the same image. */
    /* For this additional invocation, the front-end should first set */
    /* the RV_DECODE_MORE_FRAMES bit in the 'H263DecoderInParams.flags' */
    /* member, to indicate to the decoder that it is being invoked to */
    /* extract the next frame. */
    /* The front-end should continue invoking the decoder until the */
    /* RV_DECODE_MORE_FRAMES bit is not set in the 'notes' member. */
    /* For each invocation to decompress a frame in the same "MORE_FRAMES" */
    /* loop, the front-end should send in the same input image. */
    /* */
    /* If the decoder has no frames to return for display, 'numFrames' will */
    /* be set to zero.  To avoid redundancy, the decoder does *not* set */
    /* the RV_DECODE_DONT_DRAW bit in 'notes' in this case. */


    ULONG32 timestamp;
    /* The 'temporal_offset' parameter is used in conjunction with the */
    /* RV_DECODE_MORE_FRAMES note, to assist the front-end in */
    /* determining when to display each returned frame. */
    /* If the decoder sets this to T upon return, the front-end should */
    /* attempt to display the returned image T milliseconds relative to */
    /* the front-end's idea of the presentation time corresponding to */
    /* the input image. */
    /* Be aware that this is a signed value, and will typically be */
    /* negative. */

    ULONG32 width;
    ULONG32 height;
    /* Width and height of the returned frame. */
    /* This is the width and the height as signalled in the bitstream. */


} H263DecoderOutParams;



typedef struct tag_RV10_DITHERPARAMS
{
    UCHAR* pColors;  /* Array of colors used for dithering. */
} RV10_DITHERPARAMS;

#define RV10_POSTFILTER_PARAMS 0x00001002
#define RV10_ADVANCED_MP_PARAMS 0x0001003
#define RV10_TEMPORALINTERP_PARAMS  0x00001004

typedef struct tagRV10_INIT
{
    UINT16 outtype;
    UINT16 pels;
    UINT16 lines;
    UINT16 nPadWidth;   /* number of columns of padding on right to get 16 x 16 block*/
    UINT16 nPadHeight;  /* number of rows of padding on bottom to get 16 x 16 block*/

    UINT16 pad_to_32;   /* to keep struct member alignment independent of */
    /* compiler options */
    ULONG32 ulInvariants;
    /* ulInvariants specifies the invariant picture header bits */
    LONG32 packetization;
    ULONG32 ulStreamVersion;
} RV10_INIT;

typedef HX_RESULT (HXEXPORT_PTR FPGETGUID)(UCHAR*);

typedef HX_RESULT (HXEXPORT_PTR FPTRANSFORMINIT)(void * prv10Init,void **global);
typedef HX_RESULT (HXEXPORT_PTR FPTRANSFORMFREE)(void *global);

typedef HX_RESULT (HXEXPORT_PTR FPCUSTOMMESSAGE)(RV_Custom_Message_ID *msg_id, void *global);
typedef HX_RESULT (HXEXPORT_PTR FPHIVEMESSAGE)(ULONG32 *msg, void *global);
typedef HX_RESULT (HXEXPORT_PTR FPTRANSFORMRV10TOYUV)(UCHAR *pRV10Packets,
        UCHAR *pDecodedFrameBuffer, void *pInputParams,
        void *pOutputParams,void *global);

#ifdef __cplusplus
}       /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#if defined(_MSC_VER)
#pragma pack(pop)
/*///////////////////////////////// */
/* */
/* End 1-byte structure alignment */
/* */
/*///////////////////////////////// */
#endif

/* Byte alignment settings for Mac */
#if defined( _MACINTOSH )
#pragma options align=reset
#endif

#endif /* __RV20DEC_H__ */


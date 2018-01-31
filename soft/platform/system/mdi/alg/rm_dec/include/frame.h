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














#ifndef DECTEMP_H__
#define DECTEMP_H__

#include "rvtypes.h"
#include "rvstatus.h"
#include "rvdebug.h"
#include "rvstruct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* */
/* Define classes that encapsulate pitched YUV image data. */
/* */




/* The PaddedYUVBuffer class represents a YUV image laid out in memory such */
/* that there are padding bytes surrounding the three planes.  This extra */
/* padding allows pixels at the edge of the planes to be replicated, as */
/* well as facilitating algorithms that need to read or write a few bytes */
/* beyond the end of a line of pixels.  The frame is laid out as shown below. */
/* */
/*          +---------------------+ */
/*          |                     | */
/*          |    +-----------+    |     The amount of padding around the */
/*          |    |           |    |     U and V planes is half the amount */
/*          |    |     Y     |    |     around the Y plane.  The dotted line */
/*          |    |           |    |     separating the planes is there just */
/*          |    +-----------+    |     to emphasize that the padding around */
/*          |                     |     each plane does not overlap the */
/*          | .  .  .  .  .  .  . |     padding around the other planes. */
/*          |          .          | */
/*          |  +----+  .  +----+  |     Note that all three planes have */
/*          |  | U  |  .  |  V |  |     the same pitch. */
/*          |  +----+  .  +----+  | */
/*          +---------------------+ */
/* */
/* Note that the class is designed to allow reusing existing buffer space */
/* for a smaller image size.  For example, if the frame buffer was originally */
/* allocated to hold a CIF image, and is then adjusted to hold a QCIF image, */
/* then the QCIF Y, U and V planes will reside in or about the upper left */
/* quadrant of the CIF-size frame buffer.  Such reuse can result in much */
/* wasted space.  If this is undesireable, then the existing large buffer */
/* should be explicitly deallocated prior to allocating the smaller size. */
/* */

/* To be fully general, this class should allow the amount of padding to be */
/* specified via a constructor parameter.  However, we don't need such */
/* generality currently.  To simplify the class, we will use the hard coded */
/* padding amounts defined below. */

#define YUV_Y_PADDING       16
/* The Y plane has this many bytes (a.k.a. pels) of padding around each */
/* of its 4 sides */

#define YUV_UV_PADDING      8
/* The U and V planes have this many bytes (a.k.a. pels) of padding */
/* around each of their 4 sides */

#define YUV_ALIGNMENT       32
/* The beginning of the padded buffer is aligned thusly. */
/* (Hmm?  These numbers imply that the entire buffer is 32-byte aligned, */
/* while the Y plane is only 16-byte aligned.  Is 16-byte alignment */
/* sufficient for the overall buffer?) */


struct DecodedFrame;

void DecodedFrame_Init(struct DecodedFrame *t);

void DecodedFrame_Delete(struct DecodedFrame *t);

RV_Status
DecodedFrame_allocate(struct DecodedFrame *t, const struct RV_Dimensions *lumaSize);

void
DecodedFrame_conditionalDeallocate(struct DecodedFrame *t, const struct RV_Dimensions *dim);


void
DecodedFrame_addref(struct DecodedFrame *t);

void
DecodedFrame_release(struct DecodedFrame *t);

I32
DecodedFrame_getrefcount(struct DecodedFrame *t);

void
DecodedFrame_deallocate(struct DecodedFrame *t);

void
DecodedFrame_clearState(struct DecodedFrame *t);

void
DecodedFrame_copyState(struct DecodedFrame *t, struct DecodedFrame *src);

void
DecodedFrame_Propagate_Defaults(struct RV_Image_Format *fmt);

void
DecodedFrame_Set_Buffer_Pointers(struct RV_Image *t, void *buffer);






void
DecodedFrame_CopyFormat(struct RV_Image_Format *t, const struct RV_Image_Format *fmt);

void
DecodedFrame_convertToRV_Image
(
    struct DecodedFrame *t,
    struct RV_Image *image,
    const struct RV_Dimensions *size
);

void
DecodedFrame_expand(struct DecodedFrame *t);

void
DecodedFrame_swap(struct DecodedFrame *t, struct DecodedFrame *pOther);

extern void RV_CDECL ExpandPlane(
    U8 *StartPtr,
    U32 uFrameWidth,
    U32 uFrameHeight,
    U32 uPitch,
    U32 uPels) GNUCDECL;             /* size of expansion (8 or 16) */

#ifdef INTERLACED_CODE
extern void RV_CDECL ExpandPlanei(
    U8 *StartPtr,
    U32 uFrameWidth,
    U32 uFrameHeight,
    U32 uPitch,
    U32 uPels) GNUCDECL;             /* size of expansion (8 or 16) */
#endif


struct DecodedFrame /*: public DecoderYUVWorkSpace */
{

    /* YUVPointers */
    U8     *m_pYPlane;
    U8     *m_pUPlane;
    U8     *m_pVPlane;

    /* PaddedYUVBuffer */
    U8                 *m_pAllocatedBuffer;
    /* m_pAllocatedBuffer contains the pointer returned when */
    /* we allocated space for the data. */

    U32                 m_allocatedSize;
    /* This is the size with which m_pAllocatedBuffer was allocated. */

    U8                 *m_pBuffer;
    /* m_pBuffer is a "YUV_ALIGNMENT"-byte aligned address, pointing */
    /* to the beginning of the padded YUV data within */
    /* m_pAllocatedBuffer. */

    struct RV_Dimensions      m_lumaSize;
    /* m_lumaSize specifies the dimensions of the Y plane, as */
    /* specified when allocate() was most recently called. */
    /* */
    /* For clarity, it should be noted that in the context of our */
    /* codec, these dimensions have typically already been rounded */
    /* up to a multiple of 16.  However, such rounding is performed */
    /* outside of this class.  We use whatever dimensions come into */
    /* allocate(). */

    U32                 m_pitch;
    /* m_pitch is 0 if the buffer hasn't been allocated. */
    /* Otherwise it is the current pitch in effect (typically */
    /* 2*Y_PADDING + m_lumaSize.width). */


    /* DecoderYUVWorkspace */
    RV_Boolean            m_isDeblocked;
    RV_Boolean            m_isSmoothed;
    /* These indicate whether a deblocking or smoothing filter */
    /* have been applied. */

    RV_Boolean            m_isExpanded;
    /* This indicates whether pixels around the edges of the planes */
    /* have been replicated into the surrounding padding area. */
    /* The expand() method performs this operation. */

    struct RV_Dimensions  m_macroBlockSize;
    struct RV_Dimensions  m_subBlockSize;
    /* The image's width and height, in units of macroblocks and */
    /* 4x4 subblocks.  For example, a QCIF image is 11 MBs wide and */
    /* 9 MBs high; or 44 subblocks wide and 36 subblocks high. */


    /* DecodedFrame */
    struct DecodedFrame       *m_pPreviousFrame;
    struct DecodedFrame       *m_pFutureFrame;
    /* These point to the previous and future reference frames */
    /* used to decode this frame. */
    /* These are also used to maintain a doubly-linked list of */
    /* reference frames in the DecodedFrameList class.  So, these */
    /* may be non-NULL even if the current frame is an I frame, */
    /* for example.  m_pPreviousFrame will always be valid when */
    /* decoding a P or B frame, and m_pFutureFrame will always */
    /* be valid when decoding a B frame. */

    struct DecodedFrame *m_pPreviousFrame2;
    struct DecodedFrame *m_pFutureFrame2;
    /* These pointers are needed to implement DecodedFrameList2 */
    /* which does not depend on pointers to future and  */
    /* previous referenceframes. */

    I32             m_nRefCount;
    /* reference counter */

    U32             m_dstWidth;
    U32             m_dstHeight;
    /* destination width and height (not rounded to  */
    /* an even number of macroblocks).  Copied from */
    /* dst.format.  Needed for new hardware video */
    /* mode when the size changes. Otherwise we  */
    /* could have used the decoder's dst.format */
    /* variable */

    struct PictureHeader       m_pictureHeader;

    F64                 m_cumulativeTR;
    /* This is the TR of the picture, relative to the very first */
    /* frame decoded after a call to Start_Sequence.  The F64 type */
    /* is used to avoid wrap around problems. */

    U32                 m_timeStamp;
    /* m_timeStamp provides an alternate time stamping mechanism, */
    /* used by RealVideo bitstreams. */
    /* For reference frames (i.e., non-B frames) m_timeStamp */
    /* is the sequence_number of the RV_Image with which this */
    /* image came into Decode().  For B frames, it is calculated */
    /* using the previous reference frame's frameTimestamp() plus */
    /* the TR difference between that frame and the B frame. */
    /* It assumes that the TR has millisecond granularity. */

    RV_Boolean                m_wasDisplayed;
    /* m_wasDisplayed indicates whether this decoded frame has */
    /* already been displayed. */

    U32                 m_uFrameNumber;
    /* allocated frame number */

    struct DecoderMBInfo        *m_pMBInfo;
    /* copy of the decoded frame MB info.  Needed for post */
    /* processing the frame. */
    U32                 m_allocatedMBInfoSize;
    /* size of allocated MB Info structure array */

    U32                 m_adjusted_smoothing_strength;
    /* adjusted smoothing strength based on number of active */
    /* macroblocks ( and distance of key frame from previous  */
    /* key frame) */

};

#ifdef __cplusplus
}
#endif

#endif

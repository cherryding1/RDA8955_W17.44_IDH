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












#ifdef SHEEN_VC_DEBUG
#include "memory.h"
#include "string.h"
#endif
#include "beginhdr.h"
#include "rvtypes.h"
#include "rvstatus.h"
#include "rvfid.h"
#include "rvdebug.h"
#include "rvstruct.h"
#include "frame.h"



void DecodedFrame_Init(struct DecodedFrame *t)
{
    t->m_pAllocatedBuffer = 0;
    t->m_allocatedSize = 0;
    t->m_pBuffer = 0;
    t->m_lumaSize.height = 0;
    t->m_lumaSize.width = 0;
    t->m_pitch = 0;

    RVAssert(YUV_Y_PADDING == (YUV_UV_PADDING << 1));

    t->m_pYPlane = 0;
    t->m_pUPlane = 0;
    t->m_pVPlane = 0;
    t->m_dstWidth = 0;
    t->m_dstHeight = 0;
    t->m_uFrameNumber = 0;


    DecodedFrame_clearState(t);

    t->m_pPreviousFrame = 0;
    t->m_pFutureFrame = 0;
    t->m_pPreviousFrame2 = 0;
    t->m_pFutureFrame2 = 0;
    t->m_cumulativeTR = 0.0;
    t->m_timeStamp = 0;
    t->m_wasDisplayed = FALSE;
    t->m_pMBInfo = 0;
    t->m_allocatedMBInfoSize = 0;
    t->m_adjusted_smoothing_strength = 2 /*MEDIUM_SMOOTHING*/;
    (void) memset(&t->m_pictureHeader, 0, sizeof(t->m_pictureHeader));
    t->m_nRefCount = 0;

}



void DecodedFrame_Delete(struct DecodedFrame *t)
{
    /* Just to be safe. */
    t->m_pPreviousFrame = 0;
    t->m_pFutureFrame = 0;

#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
    if (g_framelist)
        fprintf(g_framelist, "Deleting: %d\n", t->m_uFrameNumber);
#endif

    DecodedFrame_deallocate(t);
}

RV_Status
DecodedFrame_allocate(struct DecodedFrame *t, const struct RV_Dimensions *lumaSize)
{
    /* Clear our state, since allocate is called when we are about */
    /* to decode into this frame buffer. */

    RV_Status ps = RV_S_OK;
    struct RV_Dimensions paddedSize;
    U32 newSize;
    U32 extraBytes;
    U32 newMBInfoSize;

    t->m_wasDisplayed = FALSE;
    t->m_cumulativeTR = 0.0;
    t->m_timeStamp = 0;
    (void) memset(&t->m_pictureHeader, 0, sizeof(t->m_pictureHeader));

    /*ps = DecoderYUVWorkSpace_allocate(t, lumaSize); */
    /* rounded up to an integral number of macroblocks */

    paddedSize.width  = (lumaSize->width  + 15) & ~15;
    paddedSize.height = (lumaSize->height + 15) & ~15;

    DecodedFrame_clearState(t);

    /*RV_Status ps = PaddedYUVBuffer::allocate(paddedSize); */

    /* Y plane dimensions better be even, so width/2 correctly gives U,V size */
    RVAssert((lumaSize->width & 1) == 0 && (lumaSize->height & 1) == 0);

    /* YUV_ALIGNMENT must be a power of 2.  Since this is unlikely to change, */
    /* and since checking for a power of 2 is painful, let's just put a simple */
    /* assert here, that can be updated as needed for other powers of 2. */
    RVAssert(YUV_ALIGNMENT == 32);

    newSize =
        ((YUV_Y_PADDING << 1) + paddedSize.width)
        *
        (3*YUV_Y_PADDING + ((3*paddedSize.height) >> 1))
        + YUV_ALIGNMENT;

    /* Reallocate only if needed */

    if (newSize > t->m_allocatedSize)
    {
        if (t->m_pAllocatedBuffer)
            RV_Free(t->m_pAllocatedBuffer);

        t->m_pAllocatedBuffer = (U8*)RV_Allocate(newSize, FALSE);
        if (!t->m_pAllocatedBuffer)
        {
            RVDebug((RV_TL_ALWAYS,
                     "ERROR:  PaddedYUVBuffer::allocate(%ld) -- allocation failed"));

            /* Reset all our state variables */
            DecodedFrame_deallocate(t);
            return RV_S_OUT_OF_MEMORY;
        }

        t->m_allocatedSize = newSize;
        t->m_pBuffer = t->m_pAllocatedBuffer;

        extraBytes = ((t->m_pBuffer - (U8*)0) & (YUV_ALIGNMENT - 1));
        if (extraBytes)
            t->m_pBuffer += YUV_ALIGNMENT - extraBytes;
    }

    t->m_lumaSize.width = paddedSize.width;
    t->m_lumaSize.height = paddedSize.height;
    t->m_pitch = t->m_lumaSize.width + (YUV_Y_PADDING << 1);

    t->m_pYPlane = t->m_pBuffer + YUV_Y_PADDING * (t->m_pitch + 1);

    //t->m_pUPlane = t->m_pBuffer + YUV_UV_PADDING +
    //            t->m_pitch * (2*YUV_Y_PADDING + t->m_lumaSize.height + YUV_UV_PADDING); sheen
    t->m_pUPlane = t->m_pBuffer + YUV_UV_PADDING +
                   t->m_pitch * (2*YUV_Y_PADDING + t->m_lumaSize.height) + (t->m_pitch>>1)*YUV_UV_PADDING;

    //t->m_pVPlane = t->m_pUPlane + (t->m_lumaSize.width >> 1) + YUV_Y_PADDING; for yuv420. sheen
    t->m_pVPlane = t->m_pUPlane + (t->m_pitch>>1)*((t->m_lumaSize.height >> 1)+(YUV_UV_PADDING<<1));

    t->m_macroBlockSize.width  = paddedSize.width  >> 4;
    t->m_macroBlockSize.height = paddedSize.height >> 4;

    t->m_subBlockSize.width    = t->m_macroBlockSize.width  << 2;
    t->m_subBlockSize.height   = t->m_macroBlockSize.height << 2;



    newMBInfoSize  = sizeof(struct DecoderMBInfo) *
                     t->m_macroBlockSize.height   * t->m_macroBlockSize.width;

    if (newMBInfoSize > t->m_allocatedMBInfoSize)
    {
        if (t->m_pMBInfo)
            RV_Free(t->m_pMBInfo);

        t->m_pMBInfo = (struct DecoderMBInfo *)RV_Allocate(newMBInfoSize, TRUE);
        if (!t->m_pMBInfo)
        {
            RVDebug((RV_TL_ALWAYS,
                     "ERROR: DecodedFrame::allocate(%ld) -- allocation failed"));

            /* Reset all our state variables */
            DecodedFrame_deallocate(t);
            return RV_S_OUT_OF_MEMORY;
        }

        t->m_allocatedMBInfoSize = newMBInfoSize;
    }

    return ps;
}


void
DecodedFrame_conditionalDeallocate(struct DecodedFrame *t, const struct RV_Dimensions *dim)
{
    struct RV_Dimensions paddedSize;
    /* rounded up to an integral number of macroblocks */

    paddedSize.width  = (dim->width  + 15) & ~15;
    paddedSize.height = (dim->height + 15) & ~15;

    if ((dim->width != t->m_lumaSize.width)
            ||
            (dim->height != t->m_lumaSize.height))
        DecodedFrame_deallocate(t);


    /*PaddedYUVBuffer::conditionalDeallocate(paddedSize); */
}



void
DecodedFrame_deallocate(struct DecodedFrame *t)
{
    if (t->m_pMBInfo)
    {
        RV_Free(t->m_pMBInfo);
        t->m_pMBInfo = 0;
        t->m_allocatedMBInfoSize = 0;
    }

    t->m_allocatedSize = 0;
    t->m_pBuffer = 0;

    t->m_pYPlane = 0;
    t->m_pUPlane = 0;
    t->m_pVPlane = 0;

    t->m_lumaSize.width = 0;
    t->m_lumaSize.height = 0;
    t->m_pitch = 0;

    if (t->m_pAllocatedBuffer)
    {
        RV_Free(t->m_pAllocatedBuffer);
        t->m_pAllocatedBuffer = 0;
    }

    /*DecoderYUVWorkSpace_deallocate(t); */
}


void
DecodedFrame_Propagate_Defaults(struct RV_Image_Format *fmt)
{
    fmt->rectangle.width = fmt->dimensions.width;
    fmt->rectangle.height = fmt->dimensions.height;

    /*    if (RV_FID_Uses_YUV_Info(fid)) */
    {
        fmt->yuv_info.y_pitch = fmt->dimensions.width;
        fmt->yuv_info.u_pitch = fmt->yuv_info.v_pitch = fmt->dimensions.width / 2;
    }
}


void
DecodedFrame_CopyFormat(struct RV_Image_Format *t, const struct RV_Image_Format *fmt)
{
    /* fid, dimensions, rectangle */
    memcpy (t, fmt, sizeof (struct RV_Image_Format)); /* Flawfinder: ignore */
}

void
DecodedFrame_Set_Buffer_Pointers(struct RV_Image *t, void *buffer)
{
    t->yuv_info.y_plane = (U8*)buffer;
    if (!buffer)
    {
        t->yuv_info.u_plane = t->yuv_info.v_plane = 0;
    }
    else if (t->format.fid == RV_FID_YUV12 || t->format.fid == RV_FID_IYUV)
    {
        t->yuv_info.u_plane = t->yuv_info.y_plane +
                              (t->format.yuv_info.y_pitch * t->format.rectangle.height);
        t->yuv_info.v_plane = t->yuv_info.u_plane +
                              (t->format.yuv_info.u_pitch * t->format.rectangle.height / 2);
    }
    else if (t->format.fid == RV_FID_YV12)
    {
        /* For YV12, the v plane comes before the u plane */
        t->yuv_info.v_plane = t->yuv_info.y_plane +
                              (t->format.yuv_info.y_pitch * t->format.rectangle.height);
        t->yuv_info.u_plane = t->yuv_info.v_plane +
                              (t->format.yuv_info.v_pitch * t->format.rectangle.height / 2);
    }
    else if (t->format.fid == RV_FID_YVU9)
    {
        /* For YVU9, the v plane comes before the u plane */
        t->yuv_info.v_plane = t->yuv_info.y_plane +
                              (t->format.yuv_info.y_pitch * t->format.rectangle.height);
        t->yuv_info.u_plane = t->yuv_info.v_plane +
                              (t->format.yuv_info.v_pitch * t->format.rectangle.height / 4);
    }
    else
        t->yuv_info.y_plane = (U8*)buffer;
}



void
DecodedFrame_addref(struct DecodedFrame *t)
{
    t->m_nRefCount ++;
}

void
DecodedFrame_release(struct DecodedFrame *t)
{
    t->m_nRefCount --;
    if (t->m_nRefCount < 0)
        t->m_nRefCount = 0;
}

I32
DecodedFrame_getrefcount(struct DecodedFrame *t)
{
    return t->m_nRefCount;
}

void
DecodedFrame_clearState(struct DecodedFrame *t)
{
    t->m_isDeblocked = FALSE;
    t->m_isSmoothed = FALSE;
    t->m_isExpanded = FALSE;
    t->m_macroBlockSize.width = 0;
    t->m_macroBlockSize.height = 0;
    t->m_subBlockSize.width = 0;
    t->m_subBlockSize.height = 0;
}

void
DecodedFrame_copyState(struct DecodedFrame *t, struct DecodedFrame *src)
{
    t->m_isDeblocked = src->m_isDeblocked;
    t->m_isSmoothed  = src->m_isSmoothed;
    t->m_isExpanded  = FALSE;
}



void
DecodedFrame_convertToRV_Image
(
    struct DecodedFrame *t,
    struct RV_Image *image,
    const struct RV_Dimensions *size
)
{
    /*image->format.Clear(); */

    image->format.fid              = RV_FID_YUV12;
    image->format.dimensions.width  = size->width;
    image->format.dimensions.height  = size->height;
    image->format.rectangle.width        = image->format.dimensions.width;
    image->format.rectangle.height        = image->format.dimensions.height;
    image->format.yuv_info.y_pitch = t->m_pitch;
    image->format.yuv_info.u_pitch = t->m_pitch>>1;//t->m_pitch; for yuv420 sheen
    image->format.yuv_info.v_pitch = t->m_pitch>>1;//t->m_pitch; for yuv420 sheen
    image->yuv_info.y_plane        = t->m_pYPlane;
    image->yuv_info.u_plane        = t->m_pUPlane;
    image->yuv_info.v_plane        = t->m_pVPlane;
}


void
DecodedFrame_expand(struct DecodedFrame *t)
{
    if (!t->m_isExpanded)
    {
#ifdef INTERLACED_CODE
        if (t->m_pictureHeader.Interlaced)
        {
            ExpandPlanei(
                t->m_pYPlane,
                t->m_lumaSize.width,
                t->m_lumaSize.height,
                t->m_pitch,
                YUV_Y_PADDING);
        }
        else
#endif
        {
            ExpandPlane(t->m_pYPlane,
                        t->m_lumaSize.width,
                        t->m_lumaSize.height,
                        t->m_pitch,
                        YUV_Y_PADDING);
        }

        ExpandPlane(t->m_pVPlane,
                    t->m_lumaSize.width  >> 1,
                    t->m_lumaSize.height >> 1,
                    t->m_pitch>>1,//t->m_pitch, for yuv420 sheen
                    YUV_UV_PADDING);

        ExpandPlane(t->m_pUPlane,
                    t->m_lumaSize.width  >> 1,
                    t->m_lumaSize.height >> 1,
                    t->m_pitch>>1,//t->m_pitch, for yuv420 sheen
                    YUV_UV_PADDING);

        t->m_isExpanded = TRUE;
    }
}

void
DecodedFrame_swap(struct DecodedFrame *t, struct DecodedFrame *pOther)
{
    RV_Boolean bTmp;
    U8 *pTmp;
    U32     uTmp;

    if (pOther)
    {

        pTmp = pOther->m_pYPlane;
        pOther->m_pYPlane = t->m_pYPlane;
        t->m_pYPlane = pTmp;

        pTmp = pOther->m_pUPlane;
        pOther->m_pUPlane = t->m_pUPlane;
        t->m_pUPlane = pTmp;

        pTmp = pOther->m_pVPlane;
        pOther->m_pVPlane = t->m_pVPlane;
        t->m_pVPlane = pTmp;


        pTmp = pOther->m_pAllocatedBuffer;
        pOther->m_pAllocatedBuffer = t->m_pAllocatedBuffer;
        t->m_pAllocatedBuffer = pTmp;

        uTmp = pOther->m_allocatedSize;
        pOther->m_allocatedSize = t->m_allocatedSize;
        t->m_allocatedSize = uTmp;

        pTmp = pOther->m_pBuffer;
        pOther->m_pBuffer = t->m_pBuffer;
        t->m_pBuffer = pTmp;

        uTmp = pOther->m_lumaSize.width;
        pOther->m_lumaSize.width = t->m_lumaSize.width;
        t->m_lumaSize.width = uTmp;

        uTmp = pOther->m_lumaSize.height;
        pOther->m_lumaSize.height = t->m_lumaSize.height;
        t->m_lumaSize.height = uTmp;

        uTmp = pOther->m_pitch;
        pOther->m_pitch = t->m_pitch;
        t->m_pitch = uTmp;

        bTmp = pOther->m_isDeblocked;
        pOther->m_isDeblocked = t->m_isDeblocked;
        t->m_isDeblocked = bTmp;

        bTmp = pOther->m_isSmoothed;
        pOther->m_isSmoothed = t->m_isSmoothed;
        t->m_isSmoothed = bTmp;

        bTmp = pOther->m_isExpanded;
        pOther->m_isExpanded = t->m_isExpanded;
        t->m_isExpanded = bTmp;
    }
}











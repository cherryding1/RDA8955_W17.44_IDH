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


































#include "string.h"
#include "beginhdr.h"
#include "rvdecoder.h"
#include "tables.h"
#include "dirb.h"
#ifdef SHEEN_VC_DEBUG

//added by huangx
#include "vpp_rmvb.h"
#include "vppp_rmvb_asm_map.h"
#include "voc2_define.h"
#include "hal_voc.h"
//#define VOC_CFG_PRED_DMA_EADDR_MASK (0xFFFFFFFF)//(0x3FFFFFF)

#else

#include "vpp_rm_dec_asm.h"
//#define VOC_CFG_PRED_DMA_EADDR_MASK (0x3FFFFFF)

#endif

extern VPP_RMVB_DATA_T *vppDataIn;

#if 0// move to rv_overlay_set.c sheen
/*//////////////////////////////////////////////////////////////// */
/* Based on luma motion vectors, perform frame reconstruction for */
/* one 16x16 luma macroblock. */
RV_Status
Decoder_reconstructLumaMacroblock
(
    struct Decoder *t,
    U32                     mbType,
    /* We must not be called for intra and skipped macroblocks */
    struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlane,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    RV_Status   status = RV_S_OK;
    U16         *pt0;
    U32         *pt1;

    const U8    *pRef;
    U32         subblock, pitch, subBlockPitch;
    I32         xint, yint, xh, yh;

    pitch = t->m_pCurrentFrame->m_pitch;
    subBlockPitch = t->m_pCurrentFrame->m_subBlockSize.width;

    pt0 = vppDataIn->motion_vectors;
    pt1 = (U32*)(vppDataIn->interplate_yblk_addr);

    switch (mbType)
    {
#ifdef SKIPPED_RV10i
        case MBTYPE_SKIPPED:
#endif
        case MBTYPE_INTER:
        case MBTYPE_INTER_16x16:
            *pt0++ = pMV->mvx;
            *pt0++ = pMV->mvy;

            MV_Sub2FullPels(pMV->mvx, &xint, &xh);
            MV_Sub2FullPels(pMV->mvy, &yint, &yh);

            pRef = pRefYPlane + mbXOffset + xint + ((mbYOffset + yint) * pitch);

            if(yh==0 || (xh==3 && yh==3))
                pRef = pRef -2;
            else
                pRef = pRef -2*pitch -2;

            *pt1 = (INT32)(pRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
            /* interpolate 16x16 block */
            //(*t->m_InterpFunc)[yh][xh](pRef, pt1, pitch, 16);
            break;

        case MBTYPE_FORWARD:
        case MBTYPE_BACKWARD:
            /* 1 vector for the MB */
            *pt0++ = pMV->mvx;
            *pt0++ = pMV->mvy;

            MV_Sub2FullPels_B(pMV->mvx, &xint, &xh);
            MV_Sub2FullPels_B(pMV->mvy, &yint, &yh);

            pRef = pRefYPlane + mbXOffset + xint + ((mbYOffset + yint) * pitch);

            if(yh==0 || (xh==3 && yh==3))
                pRef = pRef -2;
            else
                pRef = pRef -2*pitch -2;

            *pt1 = (INT32)(pRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
            //(*t->m_InterpFunc_B)[yh][xh](pRef, pt1, pitch, 16);
            break;

        case MBTYPE_INTER_4V:
        case MBTYPE_INTER_16x8V:
        case MBTYPE_INTER_8x16V:
            /* 4 8x8 blocks */
            for (subblock=0; subblock<4; subblock++)
            {
                *pt0++ = pMV->mvx;
                *pt0++ = pMV->mvy;

                MV_Sub2FullPels(pMV->mvx, &xint, &xh);
                MV_Sub2FullPels(pMV->mvy, &yint, &yh);

                pRef = pRefYPlane + mbXOffset + xoff[chroma_block_address[subblock]] + xint
                       + (mbYOffset + yoff[chroma_block_address[subblock]] + yint) * pitch;

                if(yh==0 || (xh==3 && yh==3))
                    pRef = pRef -2;
                else
                    pRef = pRef -2*pitch -2;

                *pt1++ = (INT32)(pRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
                /* interpolate 8x8 block */
                //(*t->m_InterpFunc)[yh][xh](pRef, pt1, pitch, 8);

                /* advance MV pointer and source ptr to next 8x8 block */
                if (1 == subblock)
                {
                    /* next block is lower left */
                    pMV += subBlockPitch*2 - 2;
                }
                else
                {
                    pMV += 2;
                }
                //pt1 += VPP_LUMABLK_SIZE;
            }   /* for subblock */
            break;

        default:
            /* not supposed to be here */
            RVAssert(0);
            break;

    }
    return status;
}
#endif


RV_Status
Decoder_reconstructLumaMacroblock_RV8
(
    struct Decoder *t,
    U32                     mbType,
    /* We must not be called for intra and skipped macroblocks */
    struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlane,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    U16        *pt0;
    U32        *pt1;
    const U8   *pRef;
    U32         subblock, pitch, subBlockPitch;
    I32         xint, yint, xh, yh;

    pitch = t->m_pCurrentFrame->m_pitch;
    subBlockPitch = t->m_pCurrentFrame->m_subBlockSize.width;

    pt0 = vppDataIn->motion_vectors;
    pt1 = (U32*)(vppDataIn->interplate_yblk_addr);

    switch (mbType)
    {
        case MBTYPE_INTER:
            /* 1 vector for the MB */
            MV_Sub2FullPels_RV8(pMV->mvx, &xint, &xh);
            MV_Sub2FullPels_RV8(pMV->mvy, &yint, &yh);

            *pt0++ = xh;
            *pt0++ = yh;

            pRef = pRefYPlane + mbXOffset + xint + (((I32)mbYOffset + yint) * (I32)pitch);
            *pt1 = (INT32)pRef;

            /* interpolate 16x16 block */
            //(*t->m_InterpFunc_RV8)[yh][xh](pRef, pt1, pitch, 16);
            break;

        case MBTYPE_FORWARD:
        case MBTYPE_BACKWARD:
            /* 1 vector for the MB */
            MV_Sub2FullPels_RV8(pMV->mvx, &xint, &xh);
            MV_Sub2FullPels_RV8(pMV->mvy, &yint, &yh);

            *pt0++ = xh;
            *pt0++ = yh;

            pRef = pRefYPlane + (I32)(mbXOffset + xint + ((mbYOffset + yint) * pitch));
            *pt1 = (INT32)pRef;

            /* interpolate 16x16 block */
            //(*t->m_InterpFunc_RV8)[yh][xh](pRef, pt1, pitch, 16);
            break;

        case MBTYPE_INTER_4V:
            /* 4 8x8 blocks */
            for (subblock=0; subblock<4; subblock++)
            {
                MV_Sub2FullPels_RV8(pMV->mvx, &xint, &xh);
                MV_Sub2FullPels_RV8(pMV->mvy, &yint, &yh);

                *pt0++ = xh;
                *pt0++ = yh;

                pRef = pRefYPlane + (I32)(mbXOffset + xoff[chroma_block_address[subblock]] + xint
                                          + (mbYOffset + yoff[chroma_block_address[subblock]] + yint) * pitch);

                *pt1++ = (INT32)pRef;
                /* interpolate copy the 8x8 block */
                //(*t->m_InterpFunc_RV8)[yh][xh](pRef, pt1, pitch, 8);

                /* advance MV pointer and source ptr to next 8x8 block */
                if (1 == subblock)
                {
                    /* next block is lower left */
                    pMV += subBlockPitch*2 - 2;
                }
                else
                {
                    pMV += 2;
                }

                //pt1 += VPP_LUMABLK_SIZE;
            }   /* for subblock */
            break;

        default:
            /* not supposed to be here */
            RVAssert(0);
            break;

    }   /* switch */

    return RV_S_OK;
}

#if 0//sheen
/*//////////////////////////////////////////////////////////////// */
/* Based on luma motion vectors, perform frame reconstruction for */
/* one direct coded 16x16 luma macroblock. */

void RV_CDECL C_DirectB_InterpolMB_Lu(
    U8 *const       pDirB,      /* pointer to current direct mode MB buffer */
    const U8 *const pPrev,      /* pointer to previous ref plane buffer */
    const U8 *const pFutr,      /* pointer to future ref plane buffer */
    const U32       uPredPitch, /* for pPrev and pFutr */
    const U32       uPitch,     /* buffer pitch for pDirB */
    const Bool32    bSkipInterp,
    const I32       uFwdRatio,
    const I32       uBwdRatio
)
{
    I32 i, j, n, m;

    if (bSkipInterp)
    {
        for (j = 0, n = 0, m = 0; j < 16; j ++, n += uPitch, m += uPredPitch)
            memcpy(pDirB + n, pPrev + m, sizeof(U8) * 16); /* Flawfinder: ignore */
    }
    else if (uFwdRatio != uBwdRatio)
    {
        for (j = 0, n = 0, m = 0; j < 16; j ++, n += uPitch, m += uPredPitch)
            for (i = 0; i < 16; i ++)
            {
#if (TR_SHIFT == 14) && !defined(ARM) && !defined(_ARM_)
                /* matches 16-bit arithmetics in the asm implementation */
                U32 v1 = (U32) pFutr[m + i] << 7;
                U32 v2 = (U32) pPrev[m + i] << 7;
                U32 w = ((v1 * uFwdRatio) >> 16) + ((v2 * uBwdRatio) >> 16);
                pDirB[n + i] = (U8) ((w + 0x10) >> 5);
#else
                pDirB[n + i] = (U8) (((pFutr[m + i] * uFwdRatio +
                                       pPrev[m + i] * uBwdRatio) + TR_RND) >> TR_SHIFT);
#endif  /* (TR_SHIFT == 14) */
            }
    }
    else
    {
        for (j = 0, n = 0, m = 0; j < 16; j ++, n += uPitch, m += uPredPitch)
            for (i = 0; i < 16; i ++)
            {
                pDirB[n + i] = (U8) ((pPrev[m + i] + pFutr[m + i] + 1) >> 1);
            }
    }

} /* C_DirectB_InterpolMB_Lu */
#endif

#if 0 //voc sheen.
void Decoder_reconDirectInterpLuma
(
    struct Decoder *t,
    U8 *const       pDst,
    const U8 *const pPrev,
    const U8 *const pFutr,
    const U32       uPredPitch,
    const U32       pitch,
    const U32       mbType,
    const Bool32    bSkipInterp,
    const I32       ratio0,
    const I32       ratio1,
    const Bool8 *pIsSubBlockEmpty,
    I32 *pQuantBuf
)
{
    I32 b;
    /* interpolate from future and previous reconstruction */
    (*t->m_pDirectB_InterpolMB_Lu)(pDst, pPrev, pFutr, uPredPitch, pitch, bSkipInterp,
                                   ratio0, ratio1
                                  );

    if (mbType != MBTYPE_SKIPPED)
    {
        for (b = 0; b < 16; b ++)
        {
            if (pIsSubBlockEmpty[b])
                continue;

            t->m_pAdd4x4_Direct(
                pDst + yoff[b] * pitch + xoff[b],
                pQuantBuf + 16 * b,
                pitch);
        }
    }

} /* reconDirectInterpLuma */
#endif

#if 0// move to rvdecoder.h sheen

#define TRUNCATE_LO(val, lim, tmp)      \
    (tmp) = (lim);                      \
    if ((tmp) < (val))                  \
        (val) = (tmp);

#define TRUNCATE_HI(val, lim, tmp)      \
    (tmp) = (lim);                      \
    if ((tmp) > (val))                  \
        (val) = (tmp);
#endif

#if 0// move to rv_overlay_set.c sheen

RV_Status Decoder_reconstructLumaMacroblockDirect
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    /* We must not be called for intra and skipped macroblocks */
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch;
    const I32 width = t->m_pCurrentFrame->m_lumaSize.width;
    const I32 height = t->m_pCurrentFrame->m_lumaSize.height;

    I32 mvxP, mvyP, mvxF, mvyF;
    I32 posx, posy;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    I32 lo, hi;     /* temp vars for TRUNCATE macro */
    U16 *pt0;
    U32 *pt1,*pt2;
    I32 i;

    const U8* pRefYFuture;
    const U8* pRefYPrevious;

    pt0   = vppDataIn->motion_vectors;
    for(i=0; i<4; i++)
    {
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
    }

    pt1   = (U32*)(vppDataIn->interplate_yblk_addr);
    pt2   = (U32*)(vppDataIn->interplate_yblk_b_addr);

    pRefYPrevious   = pRefYPlanePrevious + mbXOffset + mbYOffset * pitch;
    pRefYFuture     = pRefYPlaneFuture + mbXOffset + mbYOffset * pitch;


    if (IS_INTRA_MBTYPE(mbTypeRef) || (mbTypeRef == MBTYPE_SKIPPED))
    {
        *pt1 = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
        *pt2 = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
        /* interpolate copy the 16x16 previous block */
        //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);

        /* interpolate copy the 16x16 future block */
        //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
    }
    else if (mbTypeRef == MBTYPE_INTER || mbTypeRef == MBTYPE_INTER_16x16)
    {
        if (!pMV->mvx && !pMV->mvy)
        {
            *pt1 = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2 = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 16x16 previous block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);

            /* interpolate copy the 16x16 future block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
        }
        else
        {
            posx = mbXOffset;
            posy = mbYOffset;

            if (pMV->mvx)
            {
                mvxP = (iRatio0 * pMV->mvx + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - pMV->mvx;

                TRUNCATE_LO(mvxF, (width - 3 - posx) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (-14 - posx) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefYPrevious += xintp;
                pRefYFuture += xintf;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (pMV->mvy)
            {
                mvyP = (iRatio0 * pMV->mvy + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - pMV->mvy;

                TRUNCATE_LO(mvyF, (height - 6 - posy) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (-11 - posy) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefYPrevious += yintp * pitch;
                pRefYFuture += yintf * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt1 = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2 = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 16x16 previous block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);

            /* interpolate copy the 16x16 future block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
        }
    }
    else
    {
        vppDataIn->inter16      = 0;
        /* process four 8x8 blocks */
        for (i = 0; i < 16; i += 4)
        {
            posx = mbXOffset + xoff[block_subblock_mapping[i]];
            posy = mbYOffset + yoff[block_subblock_mapping[i]];

            pRefYPrevious = pRefYPlanePrevious + posx + posy * pitch;
            pRefYFuture = pRefYPlaneFuture + posx + posy * pitch;

            if (pMV->mvx)
            {
                mvxP = (iRatio0 * pMV->mvx + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - pMV->mvx;
                TRUNCATE_LO(mvxF, (width + 5 - posx) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (-14 - posx) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefYPrevious += xintp;
                pRefYFuture += xintf;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (pMV->mvy)
            {
                mvyP = (iRatio0 * pMV->mvy + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - pMV->mvy;

                TRUNCATE_LO(mvyF, (height + 2 - posy) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (-11 - posy) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefYPrevious += yintp * pitch;
                pRefYFuture += yintf * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt1++ = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2++ = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 8x8 previous block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 8);
            /* interpolate copy the 8x8 future block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 8);

            if (i & 4)
            {
                pMV += t->m_pCurrentFrame->m_subBlockSize.width * 2 - 2;
            }
            else
            {
                pMV += 2;
            }

        } /* for subblock */
    }
    return RV_S_OK;

} /* reconstructLumaMacroblockDirect */

#endif

RV_Status Decoder_reconstructLumaMacroblockDirect_RV8
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    /* We must not be called for intra and skipped macroblocks */
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch;
    const I32 width = t->m_pCurrentFrame->m_lumaSize.width;
    const I32 height = t->m_pCurrentFrame->m_lumaSize.height;

    I32 mvxP, mvyP, mvxF, mvyF;
    I32 posx, posy;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    I32 lo, hi;     /* temp vars for TRUNCATE macro */
    U16 *pt0;
    U32 *pt1,*pt2;
    I32 i;

    const U8* pRefYFuture;
    const U8* pRefYPrevious;

    pt0   = vppDataIn->motion_vectors;
    for(i=0; i<4; i++)
    {
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
    }

    pt1   = (U32*)(vppDataIn->interplate_yblk_addr);
    pt2   = (U32*)(vppDataIn->interplate_yblk_b_addr);

    pRefYPrevious   = pRefYPlanePrevious + mbXOffset + mbYOffset * pitch;
    pRefYFuture     = pRefYPlaneFuture + mbXOffset + mbYOffset * pitch;

    if (IS_INTRA_MBTYPE(mbTypeRef) || mbTypeRef == MBTYPE_SKIPPED)
    {
        *pt1 = (INT32)pRefYPrevious;
        *pt2 = (INT32)pRefYFuture;

        /* interpolate copy the 16x16 previous block */
        //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);
        /* interpolate copy the 16x16 future block */
        //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
    }
    else if (mbTypeRef == MBTYPE_INTER || mbTypeRef == MBTYPE_INTER_16x16)
    {
        if (!pMV->mvx && !pMV->mvy)
        {
            *pt1 = (INT32)pRefYPrevious;
            *pt2 = (INT32)pRefYFuture;

            /* interpolate copy the 16x16 previous block */
            //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);
            /* interpolate copy the 16x16 future block */
            //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
        }
        else
        {
            posx = mbXOffset;
            posy = mbYOffset;

            if (pMV->mvx)
            {
                mvxP = (iRatio0 * pMV->mvx + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - pMV->mvx;

                TRUNCATE_LO(mvxF, (width - 3 - posx) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvxF, (-14 - posx) * INTERP_FACTOR_RV8 + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR_RV8; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR_RV8; xhp = 0;

                pRefYPrevious += xintp;
                pRefYFuture += xintf;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (pMV->mvy)
            {
                mvyP = (iRatio0 * pMV->mvy + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - pMV->mvy;

                TRUNCATE_LO(mvyF, (height - 6 - posy) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvyF, (-11 - posy) * INTERP_FACTOR_RV8 + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR_RV8; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR_RV8; yhp = 0;

                pRefYPrevious += yintp * pitch;
                pRefYFuture += yintf * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt1 = (INT32)pRefYPrevious;
            *pt2 = (INT32)pRefYFuture;

            /* interpolate copy the 16x16 previous block */
            //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);
            /* interpolate copy the 16x16 future block */
            //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
        }
    }
    else
    {
        vppDataIn->inter16      = 0;
        /* process four 8x8 blocks */
        for (i = 0; i < 16; i += 4)
        {
            posx = mbXOffset + xoff[block_subblock_mapping[i]];
            posy = mbYOffset + yoff[block_subblock_mapping[i]];

            pRefYPrevious = pRefYPlanePrevious + posx + posy * pitch;
            pRefYFuture = pRefYPlaneFuture + posx + posy * pitch;

            if (pMV->mvx)
            {
                mvxP = (iRatio0 * pMV->mvx + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - pMV->mvx;
                TRUNCATE_LO(mvxF, (width + 5 - posx) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvxF, (-14 - posx) * INTERP_FACTOR_RV8 + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR_RV8; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR_RV8; xhp = 0;

                pRefYPrevious += xintp;
                pRefYFuture += xintf;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (pMV->mvy)
            {
                mvyP = (iRatio0 * pMV->mvy + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - pMV->mvy;

                TRUNCATE_LO(mvyF, (height + 2 - posy) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvyF, (-11 - posy) * INTERP_FACTOR_RV8 + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR_RV8; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR_RV8; yhp = 0;

                pRefYPrevious += yintp * pitch;
                pRefYFuture += yintf * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt1++ = (INT32)pRefYPrevious;
            *pt2++ = (INT32)pRefYFuture;

            /* interpolate copy the 8x8 previous block */
            //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 8);
            /* interpolate copy the 8x8 future block */
            //(*t->m_InterpFunc_RV8)[0][0]((U8 *)pRefYFuture, pt1, pitch, 8);

            if (i & 4)
            {
                pMV += t->m_pCurrentFrame->m_subBlockSize.width * 2 - 2;
            }
            else
            {
                pMV += 2;
            }

            //pt0 +=VPP_LUMABLK_SIZE;
            //pt1 +=VPP_LUMABLK_SIZE;

        } /* for subblock */
    }

    return RV_S_OK;
} /* reconstructLumaMacroblockDirect_RV8 */

#if 0// move to rv_overlay_set.c sheen

RV_Status Decoder_reconstructLumaMacroblockBidir
(
    struct Decoder *t,
    const U32               mbType,
    struct DecoderPackedMotionVector *pMVp,
    struct DecoderPackedMotionVector *pMVf,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{

    const I32 pitch = t->m_pCurrentFrame->m_pitch;
    const U8* in1;
    const U8* in2;
    I32 mvxP, mvyP, mvxF, mvyF;
    I32 posx, posy;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    U16 *pt0;
    U32 *pt1;
    I32 i;

    posx = mbXOffset;
    posy = mbYOffset;

    in1 = pRefYPlanePrevious + posx + posy * pitch;
    in2 = pRefYPlaneFuture + posx + posy * pitch;

    mvxF = pMVf->mvx;
    mvxP = pMVp->mvx;

    xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
    xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

    in1 += xintp;
    in2 += xintf;

    mvyF = pMVf->mvy;
    mvyP = pMVp->mvy;

    yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
    yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

    in1 += yintp * pitch;
    in2 += yintf * pitch;

    pt0   = vppDataIn->motion_vectors;
    for(i=0; i<4; i++)
    {
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
    }

    pt1   = (U32*)(vppDataIn->interplate_yblk_addr);
    *pt1++ = (INT32)(in1-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
    /* interpolate copy the 16x16 previous block */
    //(*t->m_InterpFunc_B)[0][0]((U8 *)in1, pt0, pitch, 16);

    pt1   = (U32*)(vppDataIn->interplate_yblk_b_addr);
    *pt1++ = (INT32)(in2-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
    /* interpolate copy the 16x16 future block */
    //(*t->m_InterpFunc_B)[0][0]((U8 *)in2, pt0, pitch, 16);

    return RV_S_OK;

} /* reconstructLumaMacroblockBidir */

#endif

#ifdef __cplusplus
extern "C"
#endif

#if 0// voc sheen

void RV_CDECL C_DirectB_InterpolMB_Cr(
    U8 *const       pDirB,      /* pointer to current direct mode MB buffer */
    const U8 *const pPrev,      /* pointer to previous ref plane buffer */
    const U8 *const pFutr,      /* pointer to future ref plane buffer */
    const U32       uPredPitch, /* for pPrev and pFutr */
    const U32       uPitch,     /* buffer pitch for pDirB */
    const Bool32    bSkipInterp,
    const I32       uFwdRatio,
    const I32       uBwdRatio
)
{
    I32 i, j, m, n;

    if (bSkipInterp)
    {
        for (j = 0, n = 0, m = 0; j < 8; j ++, n += uPitch, m += uPredPitch)
            memcpy(pDirB + n, pPrev + m, sizeof(U8) * 8); /* Flawfinder: ignore */
    }
    else if (uFwdRatio != uBwdRatio)
    {
        for (j = 0, n = 0, m = 0; j < 8; j ++, n += uPitch, m += uPredPitch)
            for (i = 0; i < 8; i ++)
            {
#if (TR_SHIFT == 14) && !defined(ARM) && !defined(_ARM_)
                /* matches 16-bit arithmetics in the asm implementation */
                U32 v1 = (U32) pFutr[m + i] << 7;
                U32 v2 = (U32) pPrev[m + i] << 7;
                U32 w = ((v1 * uFwdRatio) >> 16) + ((v2 * uBwdRatio) >> 16);
                pDirB[n + i] = (U8) ((w + 0x10) >> 5);
#else
                pDirB[n + i] = (U8) (((pFutr[m + i] * uFwdRatio +
                                       pPrev[m + i] * uBwdRatio) + TR_RND) >> TR_SHIFT);
#endif  /* (TR_SHIFT == 14) */
            }
    }
    else
    {
        for (j = 0, n = 0, m = 0; j < 8; j ++, n += uPitch, m += uPredPitch)
            for (i = 0; i < 8; i ++)
            {
                pDirB[n + i] = (U8) ((pPrev[m + i] + pFutr[m + i] + 1) >> 1);
            }
    }

} /* C_DirectB_InterpolMB_Cr */



void Decoder_reconDirectInterpChroma
(
    struct Decoder *t,
    U8 *const           pDst,
    const I32 *const    pTransmit,
    const U8 *const     pPrev,
    const U8 *const     pFutr,
    const U32           uPredPitch,
    const U32           pitch,
    const U32           mbType,
    const Bool32        bSkipInterp,
    const I32           ratio0,
    const I32           ratio1,
    const U32           init_subblock,
    const Bool8 *pIsSubBlockEmpty
)
{
    /* interpolate from future and previous reconstruction */
    (*t->m_pDirectB_InterpolMB_Cr)(
        pDst, pPrev, pFutr, uPredPitch, pitch, bSkipInterp,
        ratio0, ratio1
    );

    /* add residual to form reconstruction */
    if (mbType != MBTYPE_SKIPPED)
    {
        I32 b;
        const U8 xoff4[4] = {0, 4, 0, 4};
        const U8 yoff4[4] = {0, 0, 4, 4};

        for (b = 0; b < 4; b ++)
        {
            if (pIsSubBlockEmpty[init_subblock+b])
                continue;

            t->m_pAdd4x4_Direct(
                pDst + yoff4[b] * pitch + xoff4[b],
                (I32*) pTransmit + 16 * b,
                pitch);
        }
    }

} /* reconDirectInterpChroma */

#endif

#if 0// move to rv_overlay_set.c sheen

void Decoder_reconstructChromaMacroBlockDirect
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen
    const I32 crWidth = t->m_pCurrentFrame->m_lumaSize.width >> 1;
    const I32 crHeight = t->m_pCurrentFrame->m_lumaSize.height >> 1;
    U32 *pt0,*pt1,*pt2,*pt3;

    I32 xRefPos = (mbXOffset >> 1);
    I32 yRefPos = (mbYOffset >> 1);
    I32 offset = xRefPos + (pitch * yRefPos);

    I32 mvX, mvY;
    I32 mvxP, mvyP, mvxF, mvyF;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    I32 lo, hi;         /* temp vars for TRUNCATE macro */

    const U8* pRefFuture;
    const U8* pRefPrevious;
    const U8* pRefFuture1;
    const U8* pRefPrevious1;

    pRefPlaneFuture   += offset;
    pRefPlanePrevious += offset;
    pRefPlaneFuture1  += offset;
    pRefPlanePrevious1+= offset;

    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_ublk_b_addr);
    pt2   = (U32*)(vppDataIn->interplate_vblk_addr);
    pt3   = (U32*)(vppDataIn->interplate_vblk_b_addr);

    if ((mbTypeRef == MBTYPE_INTER) || (mbTypeRef == MBTYPE_INTER_16x16) ||
            IS_INTRA_MBTYPE(mbTypeRef) || (mbTypeRef == MBTYPE_SKIPPED))
    {
        if(IS_INTRA_MBTYPE(mbTypeRef) || mbTypeRef == MBTYPE_SKIPPED || (!pMV->mvx && !pMV->mvy))
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;
        }
        else
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;

            mvX = pMV->mvx / 2;
            mvY = pMV->mvy / 2;

            if (mvX)
            {
                mvxP = (iRatio0 * mvX + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - mvX;
                TRUNCATE_LO(mvxF, (crWidth - xRefPos - 1) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (- 7 - xRefPos) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefFuture += xintf;
                pRefPrevious += xintp;
                pRefFuture1 += xintf;
                pRefPrevious1+= xintp;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (mvY)
            {
                mvyP = (iRatio0 * mvY + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - mvY;
                TRUNCATE_LO(mvyF, (crHeight - yRefPos - 1) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (- 7 - yRefPos) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefFuture += yintf * pitch;
                pRefPrevious += yintp * pitch;
                pRefFuture1 += yintf * pitch;
                pRefPrevious1 += yintp * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }
        }
        pRefPrevious = (U8 *)((INT32)(pRefPrevious) & VOC_CFG_PRED_DMA_EADDR_MASK);
        pRefFuture    = (U8 *)((INT32)(pRefFuture) & VOC_CFG_PRED_DMA_EADDR_MASK);
        pRefPrevious1= (U8 *)((INT32)(pRefPrevious1) & VOC_CFG_PRED_DMA_EADDR_MASK);
        pRefFuture1   = (U8 *)((INT32)(pRefFuture1) & VOC_CFG_PRED_DMA_EADDR_MASK);

        *pt0++ = (INT32)pRefPrevious;
        *pt1++ = (INT32)pRefFuture;
        *pt2++ = (INT32)pRefPrevious1;
        *pt3++ = (INT32)pRefFuture1;

        //(*t->m_InterpFuncChroma)[0][0](pRefPrevious, pt0, pitch);
        //(*t->m_InterpFuncChroma)[0][0](pRefFuture, pt1, pitch);
        //(*t->m_InterpFuncChroma)[0][0](pRefPrevious1, pt2, pitch);
        //(*t->m_InterpFuncChroma)[0][0](pRefFuture1, pt3, pitch);

        //pt0 +=VPP_CHROMABLK_SIZE;
        //pt1 +=VPP_CHROMABLK_SIZE;
        //pt2 +=VPP_CHROMABLK_SIZE;
        //pt3 +=VPP_CHROMABLK_SIZE;

        *pt0++ = (INT32)(pRefPrevious + 4);
        *pt1++ = (INT32)(pRefFuture + 4);
        *pt2++ = (INT32)(pRefPrevious1 + 4);
        *pt3++ = (INT32)(pRefFuture1 + 4);

        /*
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious + 4, pt0, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture + 4, pt1, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious1 + 4, pt2, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture1 + 4, pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
        *pt1++ = (INT32)(pRefFuture + (pitch << 2));
        *pt2++ = (INT32)(pRefPrevious1 + (pitch << 2));
        *pt3++ = (INT32)(pRefFuture1 + (pitch << 2));

        /*
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2), pt0, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2), pt1, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious1 + (pitch << 2), pt2, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture1 + (pitch << 2), pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
        *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);
        *pt2++ = (INT32)(pRefPrevious1 + (pitch << 2) + 4);
        *pt3++ = (INT32)(pRefFuture1 + (pitch << 2) + 4);

        /*
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious+ (pitch << 2) + 4, pt0, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture+ (pitch << 2) + 4, pt1, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious1+ (pitch << 2) + 4, pt2, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture1+ (pitch << 2) + 4, pt3, pitch);
        */
    }
    else /* MBTYPE_INTER_4V */
    {
        const I32 widthInBlks = t->m_pCurrentFrame->m_subBlockSize.width;
        U32 subblock;

        for (subblock = 0; subblock < 4; subblock ++)
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;

            mvX = pMV->mvx / 2;
            mvY = pMV->mvy / 2;

            if (mvX)
            {
                mvxP = (iRatio0 * mvX + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - mvX;
                TRUNCATE_LO(mvxF, (crWidth - xRefPos - 3) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (- 7 - xRefPos) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefFuture += xintf;
                pRefPrevious += xintp;
                pRefFuture1 += xintf;
                pRefPrevious1 += xintp;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (mvY)
            {
                mvyP = (iRatio0 * mvY + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - mvY;
                TRUNCATE_LO(mvyF, (crHeight - yRefPos - 3) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (- 7 - yRefPos) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefFuture += yintf * pitch;
                pRefPrevious += yintp * pitch;
                pRefFuture1 += yintf * pitch;
                pRefPrevious1 += yintp * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt0++ = (INT32)(pRefPrevious) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt1++ = (INT32)(pRefFuture) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2++ = (INT32)(pRefPrevious1) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt3++ = (INT32)(pRefFuture1) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 4x4 block from previous, then future */
            //(*t->m_InterpFuncChroma)[0][0](pRefPrevious, pt0, pitch);
            //(*t->m_InterpFuncChroma)[0][0](pRefFuture, pt1, pitch);
            //(*t->m_InterpFuncChroma)[0][0](pRefPrevious1, pt2, pitch);
            //(*t->m_InterpFuncChroma)[0][0](pRefFuture1, pt3, pitch);

            /* Advance xRefPos and yRefPos for next iteration */
            /* Move pointers to next subblock. */
            if (subblock != 1)
            {
                xRefPos += 4;
                pRefPlaneFuture += 4;
                pRefPlanePrevious += 4;
                pRefPlaneFuture1 += 4;
                pRefPlanePrevious1 += 4;
                pMV += 2;
            }
            else
            {
                xRefPos -= 4;
                yRefPos += 4;
                pRefPlaneFuture += (pitch << 2) - 4;
                pRefPlanePrevious += (pitch << 2) - 4;
                pRefPlaneFuture1 += (pitch << 2) - 4;
                pRefPlanePrevious1 += (pitch << 2) - 4;

                pMV += (widthInBlks << 1) - 2;
            }

            //pt0 +=VPP_CHROMABLK_SIZE;
            //pt1 +=VPP_CHROMABLK_SIZE;
            //pt2 +=VPP_CHROMABLK_SIZE;
            //pt3 +=VPP_CHROMABLK_SIZE;
        } /* for subblock */
    }

} /* reconstructChromaMacroBlockDirect */
#endif


void Decoder_reconstructChromaMacroBlockDirect_RV8
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen
    const I32 crWidth = t->m_pCurrentFrame->m_lumaSize.width >> 1;
    const I32 crHeight = t->m_pCurrentFrame->m_lumaSize.height >> 1;
    U32 *pt0,*pt1,*pt2,*pt3;

    I32 xRefPos = (mbXOffset >> 1);
    I32 yRefPos = (mbYOffset >> 1);
    I32 offset = xRefPos + (pitch * yRefPos);

    I32 mvX, mvY;
    I32 mvxP, mvyP, mvxF, mvyF;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    I32 lo, hi;         /* temp vars for TRUNCATE macro */

    const U8* pRefFuture;
    const U8* pRefPrevious;
    const U8* pRefFuture1;
    const U8* pRefPrevious1;

    pRefPlaneFuture   += offset;
    pRefPlanePrevious += offset;
    pRefPlaneFuture1  += offset;
    pRefPlanePrevious1+= offset;

    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_ublk_b_addr);
    pt2   = (U32*)(vppDataIn->interplate_vblk_addr);
    pt3   = (U32*)(vppDataIn->interplate_vblk_b_addr);

    if ((mbTypeRef == MBTYPE_INTER) || (mbTypeRef == MBTYPE_INTER_16x16) ||
            IS_INTRA_MBTYPE(mbTypeRef) || (mbTypeRef == MBTYPE_SKIPPED))
    {
        if(IS_INTRA_MBTYPE(mbTypeRef) || mbTypeRef == MBTYPE_SKIPPED || (!pMV->mvx && !pMV->mvy))
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;
        }
        else
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;

            mvX = pMV->mvx / 2;
            mvY = pMV->mvy / 2;

            if (mvX)
            {
                mvxP = (iRatio0 * mvX + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - mvX;
                TRUNCATE_LO(mvxF, (crWidth - xRefPos - 1) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvxF, (- 7 - xRefPos) * INTERP_FACTOR_RV8 + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR_RV8; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR_RV8; xhp = 0;

                pRefFuture += xintf;
                pRefPrevious += xintp;
                pRefFuture1 += xintf;
                pRefPrevious1+= xintp;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (mvY)
            {
                mvyP = (iRatio0 * mvY + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - mvY;
                TRUNCATE_LO(mvyF, (crHeight - yRefPos - 1) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvyF, (- 7 - yRefPos) * INTERP_FACTOR_RV8 + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR_RV8; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR_RV8; yhp = 0;

                pRefFuture += yintf * pitch;
                pRefPrevious += yintp * pitch;
                pRefFuture1 += yintf * pitch;
                pRefPrevious1 += yintp * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }
        }

        *pt0++ = (INT32)pRefPrevious;
        *pt1++ = (INT32)pRefFuture;
        *pt2++ = (INT32)pRefPrevious1;
        *pt3++ = (INT32)pRefFuture1;

        /*
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious, pt0, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture, pt1, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious1, pt2, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture1, pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + 4);
        *pt1++ = (INT32)(pRefFuture + 4);
        *pt2++ = (INT32)(pRefPrevious1 + 4);
        *pt3++ = (INT32)(pRefFuture1 + 4);

        /*
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious + 4, pt0, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture + 4, pt1, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious1 + 4, pt2, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture1 + 4, pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
        *pt1++ = (INT32)(pRefFuture + (pitch << 2));
        *pt2++ = (INT32)(pRefPrevious1 + (pitch << 2));
        *pt3++ = (INT32)(pRefFuture1 + (pitch << 2));

        /*
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious + (pitch << 2), pt0, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture + (pitch << 2), pt1, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious1 + (pitch << 2), pt2, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture1 + (pitch << 2), pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
        *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);
        *pt2++ = (INT32)(pRefPrevious1 + (pitch << 2) + 4);
        *pt3++ = (INT32)(pRefFuture1 + (pitch << 2) + 4);

        /*
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious+ (pitch << 2) + 4, pt0, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture+ (pitch << 2) + 4, pt1, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious1+ (pitch << 2) + 4, pt2, pitch);
        (*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture1+ (pitch << 2) + 4, pt3, pitch);
        */
    }
    else /* MBTYPE_INTER_4V */
    {
        const I32 widthInBlks = t->m_pCurrentFrame->m_subBlockSize.width;
        U32 subblock;

        for (subblock = 0; subblock < 4; subblock ++)
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;

            mvX = pMV->mvx / 2;
            mvY = pMV->mvy / 2;

            if (mvX)
            {
                mvxP = (iRatio0 * mvX + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - mvX;
                TRUNCATE_LO(mvxF, (crWidth - xRefPos - 3) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvxF, (- 7 - xRefPos) * INTERP_FACTOR_RV8 + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR_RV8; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR_RV8; xhp = 0;

                pRefFuture += xintf;
                pRefPrevious += xintp;
                pRefFuture1 += xintf;
                pRefPrevious1 += xintp;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (mvY)
            {
                mvyP = (iRatio0 * mvY + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - mvY;
                TRUNCATE_LO(mvyF, (crHeight - yRefPos - 3) * INTERP_FACTOR_RV8 - 1, lo)
                TRUNCATE_HI(mvyF, (- 7 - yRefPos) * INTERP_FACTOR_RV8 + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR_RV8; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR_RV8; yhp = 0;

                pRefFuture += yintf * pitch;
                pRefPrevious += yintp * pitch;
                pRefFuture1 += yintf * pitch;
                pRefPrevious1 += yintp * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt0++ = (INT32)pRefPrevious;
            *pt1++ = (INT32)pRefFuture;
            *pt2++ = (INT32)pRefPrevious1;
            *pt3++ = (INT32)pRefFuture1;

            /* interpolate copy the 4x4 block from previous, then future */
            //(*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious, pt0, pitch);
            //(*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture, pt1, pitch);
            //(*t->m_InterpFuncChroma_RV8)[0][0](pRefPrevious1, pt2, pitch);
            //(*t->m_InterpFuncChroma_RV8)[0][0](pRefFuture1, pt3, pitch);

            /* Advance xRefPos and yRefPos for next iteration */
            /* Move pointers to next subblock. */
            if (subblock != 1)
            {
                xRefPos += 4;
                pRefPlaneFuture += 4;
                pRefPlanePrevious += 4;
                pRefPlaneFuture1 += 4;
                pRefPlanePrevious1 += 4;
                pMV += 2;
            }
            else
            {
                xRefPos -= 4;
                yRefPos += 4;
                pRefPlaneFuture += (pitch << 2) - 4;
                pRefPlanePrevious += (pitch << 2) - 4;
                pRefPlaneFuture1 += (pitch << 2) - 4;
                pRefPlanePrevious1 += (pitch << 2) - 4;

                pMV += (widthInBlks << 1) - 2;
            }

            //pt0 +=VPP_CHROMABLK_SIZE;
            //pt1 +=VPP_CHROMABLK_SIZE;
            //pt2 +=VPP_CHROMABLK_SIZE;
            //pt3 +=VPP_CHROMABLK_SIZE;
        } /* for subblock */
    }
} /* reconstructChromaMacroBlockDirect_RV8 */

#if 0// move to rv_overlay_set.c sheen

void Decoder_reconstructChromaMacroBlockBidir
(
    struct Decoder *t,
    const struct DecoderPackedMotionVector *pMVp,
    const struct DecoderPackedMotionVector *pMVf,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen

    I32 xRefPos = (mbXOffset >> 1);
    I32 yRefPos = (mbYOffset >> 1);
    I32 offset = xRefPos + (pitch * yRefPos);

    I32 mvxP, mvyP, mvxF, mvyF;

    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;

    U32 *pt0,*pt1;
    const U8* pRefFuture;
    const U8* pRefPrevious;

    pRefPlaneFuture += offset;
    pRefPlanePrevious += offset;
    pRefPlaneFuture1+= offset;
    pRefPlanePrevious1+= offset;

    mvxF = pMVf->mvx / 2;
    mvyF = pMVf->mvy / 2;

    mvxP = pMVp->mvx / 2;
    mvyP = pMVp->mvy / 2;

    pRefFuture = pRefPlaneFuture;
    pRefPrevious = pRefPlanePrevious;

    xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
    xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

    pRefFuture += xintf;
    pRefPrevious += xintp;

    yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
    yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

    pRefFuture += yintf * pitch;
    pRefPrevious += yintp * pitch;

    /* interpolate copy the 4 blocks from previous, then future */
    //U BLOCK
    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_ublk_b_addr);

    pRefPrevious = (U8*)((INT32)pRefPrevious & VOC_CFG_PRED_DMA_EADDR_MASK);
    pRefFuture    = (U8*)((INT32)pRefFuture & VOC_CFG_PRED_DMA_EADDR_MASK);

    *pt0++ = (INT32)pRefPrevious;
    *pt1++ = (INT32)pRefFuture;

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture,  pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious+4);
    *pt1++ = (INT32)(pRefFuture+4);

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture  +4,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
    *pt1++ = (INT32)(pRefFuture + (pitch << 2));
    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2),pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)  ,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
    *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);
    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2)+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)+4  ,pt1, pitch);

    //V BLOCK
    pRefFuture = pRefPlaneFuture1;
    pRefPrevious = pRefPlanePrevious1;

    pRefFuture += xintf;
    pRefPrevious += xintp;

    pRefFuture += yintf * pitch;
    pRefPrevious += yintp * pitch;

    pt0   = (U32*)(vppDataIn->interplate_vblk_addr);
    pt1   = (U32*)(vppDataIn->interplate_vblk_b_addr);

    pRefPrevious = (U8 *)((INT32)(pRefPrevious) & VOC_CFG_PRED_DMA_EADDR_MASK);
    pRefFuture    = (U8 *)((INT32)(pRefFuture) & VOC_CFG_PRED_DMA_EADDR_MASK);

    *pt0++ = (INT32)pRefPrevious;
    *pt1++ = (INT32)pRefFuture;

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture,  pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious+4);
    *pt1++ = (INT32)(pRefFuture+4);

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture  +4,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
    *pt1++ = (INT32)(pRefFuture + (pitch << 2));

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2),pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)  ,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
    *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2)+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)+4  ,pt1, pitch);

} /* reconstructChromaMacroBlockBidir */


void Decoder_reconstructChromaBlock
(
    struct Decoder *t,
    const struct DecoderMBInfo    *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRef,
    const U8               *pRef1,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    I32     pitch;
    U32     subblock;
    I32     mvX, mvY, mvXint, mvYint, mvXsubpel, mvYsubpel;
    U32     xRefPos;
    U32     yRefPos;
    U32     offset;
    U32     *pt0,*pt1;
    const U8 *pMCRef,*pMCRef1;

    pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch sheen

    xRefPos = (mbXOffset >> 1);
    yRefPos = (mbYOffset >> 1);
    offset = xRefPos + (pitch * yRefPos);

    pRef += offset;
    pRef1+= offset;

    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_vblk_addr);

    for (subblock = 0; subblock < 4; subblock++)
    {
        mvX = pMV->mvx / 2;
        mvY = pMV->mvy / 2;

        MV_Sub2FullPels(mvX, &mvXint, &mvXsubpel);
        MV_Sub2FullPels(mvY, &mvYint, &mvYsubpel);

        pMCRef = pRef + mvXint + pitch * mvYint;
        pMCRef1= pRef1+ mvXint + pitch * mvYint;
        /* pMCRef is the motion-compensated reference subblock */

        /* interpolate copy the 4x4 block */
        *pt0++ = (INT32)(pMCRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
        *pt1++ = (INT32)(pMCRef1) & VOC_CFG_PRED_DMA_EADDR_MASK;

        //(*t->m_InterpFuncChroma)[mvYsubpel][mvXsubpel](pMCRef, pt0, pitch);
        //(*t->m_InterpFuncChroma)[mvYsubpel][mvXsubpel](pMCRef1,pt1, pitch);

        /* Move pointers to next subblock. */
        if (subblock != 1)
        {
            pRef += 4;
            pRef1+= 4;
            pMV  += 2;
        }
        else
        {
            /* Move pointers from subblock 1 to subblock 2. */
            I32 offsetToSubblock2 = (pitch << 2) - 4;

            pRef += offsetToSubblock2;
            pRef1+= offsetToSubblock2;
            pMV  += (t->m_pCurrentFrame->m_subBlockSize.width << 1) - 2;
        }
        //pt0 +=VPP_CHROMABLK_SIZE;
        //pt1 +=VPP_CHROMABLK_SIZE;
    }

}   /* reconstructChromaBlock */
#endif

void Decoder_reconstructChromaBlock_RV8
(
    struct Decoder *t,
    const struct DecoderMBInfo    *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRef,
    const U8               *pRef1,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    I32     pitch;
    U32     subblock;
    I32     mvX, mvY, mvXint, mvYint, mvXsubpel, mvYsubpel;
    U32     xRefPos;
    U32     yRefPos;
    U32     offset;
    U16     *pt0;
    U32     *pt1,*pt2;
    U16     data_U16_t;
    const U8 *pMCRef,*pMCRef1;

    pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen

    xRefPos = (mbXOffset >> 1);
    yRefPos = (mbYOffset >> 1);
    offset = xRefPos + (pitch * yRefPos);

    pRef += offset;
    pRef1+= offset;

    pt0 = vppDataIn->motion_vectors;
    pt1  = (U32*)(vppDataIn->interplate_ublk_addr);
    pt2  = (U32*)(vppDataIn->interplate_vblk_addr);

    for (subblock = 0; subblock < 4; subblock++)
    {
        mvX = pMV->mvx / 2;
        mvY = pMV->mvy / 2;

        MV_Sub2FullPels_RV8(mvX, &mvXint, &mvXsubpel);
        MV_Sub2FullPels_RV8(mvY, &mvYint, &mvYsubpel);

        data_U16_t = *pt0;
        data_U16_t = data_U16_t | (mvXsubpel <<2);
        *pt0++     = data_U16_t;

        data_U16_t = *pt0;
        data_U16_t = data_U16_t | (mvYsubpel <<2);
        *pt0++     = data_U16_t;

        pMCRef = pRef + mvXint + pitch * mvYint;
        pMCRef1= pRef1+ mvXint + pitch * mvYint;
        /* pMCRef is the motion-compensated reference subblock */

        /* interpolate copy the 4x4 block */
        *pt1++ = (INT32)pMCRef;
        *pt2++ = (INT32)pMCRef1;

        //(*t->m_InterpFuncChroma_RV8)[mvYsubpel][mvXsubpel](pMCRef, pt1, pitch);
        //(*t->m_InterpFuncChroma_RV8)[mvYsubpel][mvXsubpel](pMCRef1,pt2, pitch);

        /* Move pointers to next subblock. */
        if (subblock != 1)
        {
            pRef += 4;
            pRef1+= 4;
            pMV  += 2;
        }
        else
        {
            /* Move pointers from subblock 1 to subblock 2. */
            I32 offsetToSubblock2 = (pitch << 2) - 4;

            pRef += offsetToSubblock2;
            pRef1+= offsetToSubblock2;
            pMV  += (t->m_pCurrentFrame->m_subBlockSize.width << 1) - 2;
        }

        //pt1 +=VPP_CHROMABLK_SIZE;
        //pt2 +=VPP_CHROMABLK_SIZE;
    }
}   /* reconstructChromaBlock_RV8 */


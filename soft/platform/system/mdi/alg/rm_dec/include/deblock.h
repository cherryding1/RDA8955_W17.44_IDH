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

































#ifndef DCDBLOCK_H__
#define DCDBLOCK_H__

#include "stdlib.h" /* for abs() */
#include "beginhdr.h"

#include "rvtypes.h"
#include "rvstatus.h"
#include "rvdebug.h"

#if defined(DEC_TIMING_DETAIL)
#include "cdrvdefs.h"
#endif

/*#define REFERENCE_FILTER */
#define DITHER
#define QP_LEVELS 32
#define MEABS(a) (x1=(a), x2=x1>>31, (((x1)^x2)-x2))

#ifdef __cplusplus
extern "C" {
#endif


typedef void (RV_CDECL T_EdgeFilter4)(
    U8 *pPels,          /* first of 4 */
    U32 uPitch,
    U32 uMBPos,         /* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
    U32 uClipR,         /* clip for right/above */
    U32 uClipL,         /* clip for left/below */
    I32 alpha,
    I32 betaX,
    I32 beta2,
    RV_Boolean bChroma,
    RV_Boolean bStrong      /* true when strong filter enabled */
);


struct DeblockingFilter
{
    /* Pointer to table for converting QP to filter strength. */
    const I8* m_pQPSTab;

#if defined(DEC_TIMING_DETAIL)
    /* counter made public so decoder core can just read it */
    U32 m_uTimeCount_Deblock_PrePost;   /* before and after actual deblock filter */
#endif

    /* CPU-dependent function pointers */
    T_EdgeFilter4 *m_pEdgeFilter4V;
    T_EdgeFilter4 *m_pEdgeFilter4H;

    /* Pointers to strengths map. */
    PU8 m_pStrengthMapPreAligned;
    PU8 m_pStrengthMapVy, m_pStrengthMapHy;
    PU8 m_pStrengthMapVu, m_pStrengthMapHu;
    PU8 m_pStrengthMapVv, m_pStrengthMapHv;

    /* Strength map dimensions. */
    U32 m_uStrengthMapSize;

    /* Set true if all filter strengths are zero */
    RV_Boolean m_bLumaStrengthMapEmpty;
    RV_Boolean m_bChromaStrengthMapEmpty;
};


/* Inline clip function used in C_EdgeFilter4x4. */
I32 clipd1(I32 x, I32 limit);
I32 clipd2(I32 x, I32 x1, I32 limit);


T_EdgeFilter4 C_EdgeFilter4V;
T_EdgeFilter4 C_EdgeFilter4H;

#ifdef APK
T_EdgeFilter4 KNI_EdgeFilter4V;
T_EdgeFilter4 KNI_EdgeFilter4H;
#endif



RV_Status DeblockingFilter_FilterYUVFrameNew(
    struct DeblockingFilter *t,
    PU8         pYPlane,
    PU8         pVPlane,
    PU8         pUPlane,
    const U32   uWidth,
    const U32   uHeight,
    const U32   uPitch,
    const RV_Boolean    bHasSSE,
    PU8         pQPMap,
    const U32   uQPMapPitch,
    PU32        pCBPMap,
    const U32   uCBPMapPitch,
    PU8         pMBType,
    const U32   uMBTypePitch,
    PU32        pMVDMap,
    const U32   uMVDMapPitch,
    const U32   uFrameType,
    const U32   refQP
#ifdef INTERLACED_CODE
    , const U32   adjustQP
#endif
);


RV_Status DeblockingFilter_SetUpStrengthMapBpicLowCPU(
    struct DeblockingFilter *t,
    const U32   uWidth,
    const U32   uHeight,
    PU8         pQPMap,
    const U32   uQPMapPitch,
    PU32        pCBPMap,
    const U32   uCBPMapPitch,
    PU8         pMBType,
    const U32   uMBTypePitch,
    PU32        pMVDMap,
    const U32   uMVDMapPitch,
    const U32   uFrameType
);

RV_Status DeblockingFilter_FilterYUVFrameLowCPU(
    struct DeblockingFilter *t,
    PU8         pYPlane,
    PU8         pVPlane,
    PU8         pUPlane,
    const U32   uWidth,
    const U32   uHeight,
    const U32   uPitch,
    const RV_Boolean    bHasMMX,
    PU8         pQPMap,
    const U32   uQPMapPitch,
    PU32        pCBPMap,
    const U32   uCBPMapPitch,
    PU8         pMBType,
    const U32   uMBTypePitch,
    PU32        pMVDMap,
    const U32   uMVDMapPitch,
    const U32   uFrameType
);

void DeblockingFilter_C_EdgeFilter4x4StrongOptimized1A4B(
    PU8         pRec,
    const U32   uPitch,
    const U32   uWidth,
    const U32   uHeight,
    PU8         pStrengthV,
    PU8         pStrengthH,
    U8          QP,
    const RV_Boolean  chroma
);
RV_Status DeblockingFilter_EdgeFilter4x4StrongOptimizedNSM(
    struct DeblockingFilter *t,
    const U32   uWidth,         /* for luma */
    const U32   uHeight,        /* for luma */
    PU8         pQPMap,
    const U32   uQPMapPitch,
    PU32        pCBPMap,
    const U32   uCBPMapPitch,
    PU8         pMBType,
    const U32   uMBTypePitch,
    PU32        pMVDMap,
    const U32   uMVDMapPitch,
    const U32   uFrameType,
    const U32   uMB,
    const U32   uMBCol
#ifdef INTERLACED_CODE
    , const U32   adjustQP
#endif
);

void DeblockingFilter_Init(struct DeblockingFilter *t);
void DeblockingFilter_Delete(struct DeblockingFilter *t);


#ifdef __cplusplus
}
#endif

#endif  /* DCDBLOCK_H__ */

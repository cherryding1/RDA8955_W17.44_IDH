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

































#include "beginhdr.h"
#include "deblock.h"
//#include "deblock8.h" sheen
#include "tables.h" /* needed for ClampTbl */
#ifdef SHEEN_VC_DEBUG
#include "memory.h"  /* for memset */
#endif
#include "rvstruct.h"
#include "rvdebug.h"
#include "rvstatus.h"

//added by huangx
#ifdef SHEEN_VC_DEBUG
#include "vpp_rmvb.h"
#else
#include "vpp_rm_dec_asm.h"
#endif

extern VPP_RMVB_DATA_T *vppDataIn;



/* for testing: the filters are independent, finding bugs can be simpler */
/* by using only 1 filter at a time. These should normally be disabled, */
/* to run all filters. */
/*#define NO_VERTICAL_STRONG */
/*#define NO_VERTICAL_WEAK */
/*#define NO_HORIZ_STRONG */
/*#define NO_HORIZ_WEAK */
/*#define NO_VERTICAL_STRONG_CHROMA */
/*#define NO_VERTICAL_WEAK_CHROMA */
/*#define NO_HORIZ_STRONG_CHROMA */
/*#define NO_HORIZ_WEAK_CHROMA */
/* TBD remove after completion of ASM versions */
/*#define TEST_FILTER_BY_COMPARE */

#define USE_OPTIMIZED_DEBLOC
/*#define REFERENCE_FILTER */
#define DEBLOC_REF_DIFF
//#define DEBLOC_REF_DIFF_REFQP
/*#define CASE3 */

/*#define NO_5TAP */
#define STRONG_INTER_16x16

#if 0 //move to table.c sheen
/* New Stronger Deblocking filter */
/* Strength Clip Table, Strength = 0, 1, 2 */
static const
I8 clip_table[3][QP_LEVELS] =
{
    /*   0         5        10        15        20        25        30 */
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,5,5},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,5,5,5,7,8,9},
};
#endif

#if 0 //voc sheen.

/* following timing code are for development purposes */
/*#define GET_DBF_TIMING */
/*#define NO_HORIZ_WEAK */
/*#define NO_HORIZ_STRONG */
/*#define NO_VERTICAL_WEAK */
/*#define NO_VERTICAL_STRONG */

#if defined(GET_DBF_TIMING)
#include "hlxclib/stdio.h"
static U32 timing_sum;
static U32 timing_cnt;
static U32 timing_pels;
#endif

/*#define DBLOCK_TIME */
#ifdef DBLOCK_TIME
#include "hlxclib/stdio.h"
const double RDTSC_SHIFT_322    = 4294967296.0;
I32 RDTSC_CLOCK_FREQ2           = 650*1000000;
static double total_compress_time;
static U32 timing_cnt;
double PENTIUM2_TIMER() /* read time-stamp counter */
{
    unsigned long a, b;
    double temp1, temp2, result;

    __asm
    {
        _emit   0x0f
        _emit   0x31
        mov     a, eax
        mov     b, edx
    }

    temp1 = (double) a;
    temp2 = (double) (b & 0xFFFF);
    result = (temp1 + temp2 * RDTSC_SHIFT_322) / RDTSC_CLOCK_FREQ2;
    return( result * 1000.0 );
}
#endif



#define RV_Maximum_Deblocking_Strength     6
#define RV_Default_Deblocking_Strength     0

extern const
I8 s_deblock_rv89combo[RV_Maximum_Deblocking_Strength+1][QP_LEVELS];
/* defined in dcdblok8.c */


/*---------------------------------------------------------------------------- */
/*  Constructor */
/*---------------------------------------------------------------------------- */
void DeblockingFilter_Init(struct DeblockingFilter *t)
{
    /* internal strength map has not been initialized */
    t->m_pStrengthMapPreAligned = NULL;
    t->m_uStrengthMapSize = 0;
    t->m_pStrengthMapVy = t->m_pStrengthMapHy = NULL;
    t->m_pStrengthMapVu = t->m_pStrengthMapHu = NULL;
    t->m_pStrengthMapVv = t->m_pStrengthMapHv = NULL;

    t->m_bLumaStrengthMapEmpty = TRUE;
    t->m_bChromaStrengthMapEmpty = TRUE;

    /* use default deblocking filter strength */
    t->m_pQPSTab = s_deblock_rv89combo[RV_Default_Deblocking_Strength];


#if defined(GET_DBF_TIMING)
    timing_sum = timing_cnt = timing_pels = 0;
#endif
#if defined(DBLOCK_TIME)
    total_compress_time = timing_cnt = 0;
#endif

#if defined(DEC_TIMING_DETAIL)
    m_uTimeCount_Deblock_PrePost = 0;
    ALLOC_TIME_DETAIL_INST
#endif

} /* Constructor */


/*---------------------------------------------------------------------------- */
/*  Destructor */
/*---------------------------------------------------------------------------- */
void DeblockingFilter_Delete(struct DeblockingFilter *t)
{
    if (t->m_pStrengthMapPreAligned)
    {
        RV_Free(t->m_pStrengthMapPreAligned);
        t->m_pStrengthMapPreAligned = NULL;
    }
    t->m_uStrengthMapSize = 0;
    t->m_pStrengthMapVy = t->m_pStrengthMapHy = NULL;
    t->m_pStrengthMapVu = t->m_pStrengthMapHu = NULL;
    t->m_pStrengthMapVv = t->m_pStrengthMapHv = NULL;

#if defined(GET_DBF_TIMING)
    U32 timing_avg = timing_cnt ? (timing_sum / timing_cnt) : 0;
    printf("\nInloop Deblocking Filter Avg = %d per frm, %d per pel\n",
           timing_avg, timing_pels ? (timing_avg / timing_pels) : 0);
#endif


#if defined(DBLOCK_TIME)
    double timing_avg = timing_cnt ? (total_compress_time / timing_cnt) : 0;
    printf("\nInloop Deblocking Filter Avg = %f per frm\n", total_compress_time,
           timing_avg);
#endif


#if defined(DEC_TIMING_DETAIL)
    FREE_TIME_DETAIL_INST
#endif

} /* Destructor */




/*---------------------------------------------------------------------------- */
/*  Clip magnitude of x to limit.  Used in C_EdgeFilter4x4. */
/*---------------------------------------------------------------------------- */
I32 clipd1(I32 x, I32 limit)
{
    if (x > limit)
        return(limit);

    if (x < -limit)
        return(-limit);

    return(x);

} /* clipd1 */

/*---------------------------------------------------------------------------- */
/*  Clip magnitude of x to limit around X.  Used in C_EdgeFilter4x4. */
/*---------------------------------------------------------------------------- */
I32 clipd2(I32 x, I32 x1, I32 limit)
{
    if (x > (x1+limit))
        return(x1+limit);

    if (x < (x1-limit))
        return(x1-limit);

    return(x);

} /* clipd2 */

#endif


RV_Boolean deblockRefDiff(U32 mbType, U32 adjMbType)
{
    switch(mbType)
    {
        case MBTYPE_FORWARD: /* 5 */
            if(adjMbType != MBTYPE_FORWARD) return TRUE;
            break;
        case MBTYPE_BACKWARD: /* 6 */
            if(adjMbType != MBTYPE_BACKWARD) return TRUE;
            break;
        case MBTYPE_SKIPPED:        /* 7                 */
        case MBTYPE_DIRECT:         /* 8         */
        case MBTYPE_BIDIR:
            if(adjMbType == MBTYPE_FORWARD || adjMbType == MBTYPE_BACKWARD)
                return TRUE;
            break;
        default:
            break;
    }
    return FALSE;
}

#if 0//voc sheen.

/*----------------------------------------------------------------------------
// USES SIMPLE MMX 4x4 EDGE FILTER
// Strength Map structure is different too.
//--------------------------------------------------------------------------*/
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
)
{
    // determine the size of strength map
    U32 mb_per_row = uWidth >> 4;
    U32 b_per_row = uWidth >> 2;
    U32 b_per_col = uHeight >> 2;
    U32 sizeV = b_per_row * uHeight;
    U32 sizeH = b_per_col * uWidth;
    U32 size = (((sizeV + sizeH) * 3) >> 1) + 12;
    U8 str   = 0;
    U8 str_c = 0;
    U8 mbQP = pQPMap[0];
    U8 smbQP = t->m_pQPSTab[mbQP];
    // vars
    U32 mbmvd;
    // set up all maps for vertical and horizontal filtering
    U32 i, j;

    PU8 pSy;

    U32 mb_left, b_left, cbpmask_left;
    U32 mb_top, b_top, cbpmask_top;


    // allocate/re-allocate buffer if necessary
    if (size > t->m_uStrengthMapSize || !t->m_pStrengthMapPreAligned)
    {
        if (t->m_pStrengthMapPreAligned)
        {
            RV_Free (t->m_pStrengthMapPreAligned);
            t->m_pStrengthMapPreAligned = NULL;
        }
        //t->m_pStrengthMapPreAligned = new U8[size + 8];
        t->m_pStrengthMapPreAligned = (U8 *) RV_Allocate(sizeof(U8)*(size + 8), 0);

        if (!t->m_pStrengthMapPreAligned)
        {
            RVDebug((RV_TL_ALWAYS,
                     "ERROR SmoothingFilter::SetUpStrengthMap -- "
                     "Failed to allocate m_pStrengthMap"));
            t->m_pStrengthMapVy = t->m_pStrengthMapHy = NULL;
            t->m_pStrengthMapVu = t->m_pStrengthMapHu = NULL;
            t->m_pStrengthMapVv = t->m_pStrengthMapHv = NULL;
            t->m_uStrengthMapSize = 0;
            return RV_S_OUT_OF_MEMORY;
        }

        t->m_pStrengthMapVy = (PU8) (((U32) t->m_pStrengthMapPreAligned + 7) & ~7);
        t->m_pStrengthMapHy = t->m_pStrengthMapVy + sizeV+8;
        t->m_pStrengthMapVu = t->m_pStrengthMapHy + sizeH;
        t->m_pStrengthMapHu = t->m_pStrengthMapVu + (sizeV >> 2)+4;
        t->m_pStrengthMapVv = t->m_pStrengthMapHu + (sizeH >> 2);
        t->m_pStrengthMapHv = t->m_pStrengthMapVv + (sizeV >> 2)+4;
        t->m_uStrengthMapSize = size;
    }


    // initialize m_pStrengthMapVy[] for vertical edges
    pSy = t->m_pStrengthMapVy;
    for (j = 0; j < uHeight; j += 8)
    {
        for (i = 4; i < uWidth; i += 4, pSy += 8)
        {
            U32 mb = (j >> 4) * mb_per_row + (i >> 4);
            U32 mbcbpcoef = pCBPMap[mb * uCBPMapPitch];
            U8 mbmbtype = pMBType[mb * uMBTypePitch];
            U8 mb_leftmbtype = mbmbtype;
            // luma block
            // check if current luma block has any coefficients
            U8 s = smbQP;
            U32 b = (j & 12) + ((i >> 2) & 3);
            U32 cbpmask = CBP4x4Mask[b];
            if(mb)
            {
                mb_left = (j >> 4) * mb_per_row + ((i-4) >> 4);
                mb_leftmbtype = pMBType[mb_left * uMBTypePitch];
                b_left = (j & 12) + (((i-4) >> 2) & 3);
                cbpmask_left = CBP4x4Mask[b_left];
                if(pCBPMap[mb_left * uCBPMapPitch] & cbpmask_left)
                    mbcbpcoef |= cbpmask;
                if(pCBPMap[mb_left * uCBPMapPitch] & cbpmask_left<<4)
                    mbcbpcoef |= (cbpmask<<4);

            }
            mbmvd = pMVDMap[mb * uMVDMapPitch];
            if(mbmvd & cbpmask)
            {
                mbcbpcoef |= cbpmask;
                mbcbpcoef |= (cbpmask<<4);
            }
            if(deblockRefDiff(mbmbtype, mb_leftmbtype))
            {
                mbcbpcoef |= cbpmask;
                mbcbpcoef |= (cbpmask<<4);
            }
            if (!(mbcbpcoef & cbpmask))
            {
                *((U32*)pSy)=0;
            }
            else
            {
                // 4 pixels per block
                pSy[0] = pSy[1] = pSy[2] = pSy[3] = s;
                str |= s;
            }
            // proceed to the next luma block below
            // check if curent luma block has any coefficients
            s = smbQP;
            cbpmask<<=4;
            if (!(mbcbpcoef & cbpmask))
            {
                *((U32*)&pSy[4])=0;
            }
            else
            {
                // 4 pixels per block
                pSy[4] = pSy[5] = pSy[6] = pSy[7] = s;
                str |= s;
            }
        } // for j
    } // for i

    // initialize m_pStrengthMapHy[] for horizontal edges
    pSy = t->m_pStrengthMapHy;
    for (j = 4; j < uHeight; j += 4)
    {
        for (i = 0; i < uWidth; i += 4)
        {
            U32 mb = (j >> 4) * mb_per_row + (i >> 4);
            U32 b  = (j & 12) + ((i >> 2) & 3);
            U32 mbcbpcoef = pCBPMap[mb * uCBPMapPitch];
            U8 mbmbtype = pMBType[mb * uMBTypePitch];
            U8 mb_topmbtype = mbmbtype;
            U32 cbpmask = CBP4x4Mask[b];
            // luma blocks
            // check if current block has any coefficients
            U8 s = smbQP;
            if(mb>mb_per_row)
            {
                mb_top= ((j-4) >> 4) * mb_per_row + (i >> 4);;
                mb_topmbtype = pMBType[mb_top * uMBTypePitch];
                b_top = ((j-4) & 12) + ((i >> 2) & 3);
                cbpmask_top = CBP4x4Mask[b_top];
                if(pCBPMap[mb_top * uCBPMapPitch] & cbpmask_top)
                    mbcbpcoef |= cbpmask;
            }
            mbmvd = pMVDMap[mb * uMVDMapPitch];
            if(mbmvd & cbpmask)
            {
                mbcbpcoef |= cbpmask;
            }
            if(deblockRefDiff(mbmbtype, mb_topmbtype))
            {
                mbcbpcoef |= cbpmask;
            }
            if (!(mbcbpcoef & cbpmask))
            {
                *((U32*)pSy)=0;
            }
            else
            {
                // 4 pixels per block edge
                pSy[0] = pSy[1] = pSy[2] = pSy[3] = s;
                str |= s;
            }
            pSy += 4;
        } // for j
    } // for i

    t->m_bLumaStrengthMapEmpty = (str == 0)?(TRUE):(FALSE);
    t->m_bChromaStrengthMapEmpty = (str_c == 0)?(TRUE):(FALSE);

    return RV_S_OK;
} /* SetUpStrengthMapLowCPU */


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
)
{

    RV_Status status = RV_S_OK;


    /* only handle width and height that are multiples of 16 */
    RVAssert(!(uWidth & 15) && !(uHeight & 15));

#if defined(GET_DBF_TIMING)
    U32 timing_start;
    __asm
    {
        xor     eax, eax
        xor     edx, edx
        _emit   0x0f
        _emit   0x31
        mov     timing_start, eax
    }
#endif

    /* first prepare strength map based on given witdh, height and QPs */
    /* then filter each color plane separately */

    if(uFrameType == TRUEBPIC)
    {
        status = DeblockingFilter_SetUpStrengthMapBpicLowCPU\
                 (t, uWidth, uHeight, pQPMap, uQPMapPitch,
                  pCBPMap, uCBPMapPitch, pMBType, uMBTypePitch,
                  pMVDMap,uMVDMapPitch,
                  uFrameType);

        /* proceed with filtering only if strength map has been  */
        /* properly initialized */
        if (status != RV_S_OK)
            return status;

        if (!t->m_bLumaStrengthMapEmpty)
        {
            C_EdgeFilter4x4_RV8(pYPlane, uPitch, uWidth, uHeight,
                                t->m_pStrengthMapVy, t->m_pStrengthMapHy);
        }
    }

#if defined(GET_DBF_TIMING)
    U32 timing_end;
    __asm
    {
        xor     eax, eax
        xor     edx, edx
        _emit   0x0f
        _emit   0x31
        mov     timing_end, eax
    }
    if (timing_end > timing_start)
    {
        timing_sum += timing_end - timing_start;
        timing_cnt ++;
    }
    timing_pels = uWidth * uHeight * 3 / 2;
#endif

    return status;

} /* FilterYUVFrameLowCPU() */


void DeblockingFilter_C_EdgeFilter4x4StrongOptimized1A4B(
    PU8         pRec,
    const U32   uPitch,
    const U32   uWidth,
    const U32   uHeight,
    PU8         pStrengthV,
    PU8         pStrengthH,
    U8          QP,
    const RV_Boolean  chroma
)
{
    U32 i, j, d1, d2, n;
    U8 Ar, Al, sp, Cl, Cr, C, c, s, t1, t2;
    PU8 pL1, pL2, pL3, pL4, pR2, pR3, pR4;
    I32 delta;
    I32 alpha = alpha_tab[QP];
    U8 betaX = beta_tab[QP];
    U8 beta = betaX*4;
    U8 beta2;
    U32 u4Pitch;
    U32 u3Pitch;
    PU8 pStrengthHS = pStrengthH;
    I32 deltaL[4], deltaL2[4], deltaR[4], deltaR2[4];
    RV_Boolean b3SmoothLeft, b3SmoothRight;
    PU8 rt,pSV,rv,rh,rhS,r,pSH,pSHS;
    I32 R1, R2, R3, R4, L1, L2, L3, L4, d, z;
    I32  x1, x2;


    RVAssert(!(uWidth & 7) && !(uHeight & 7));
    RVAssert(pStrengthV && pStrengthH);

    if((uWidth*uHeight)<=(176*144) && !chroma)
        beta2 = beta;
    else
        beta2 = betaX*3;

    u4Pitch = uPitch <<2;
    u3Pitch = u4Pitch - uPitch;

    pStrengthH+=uWidth;

    for (j = 0; j < uHeight; j+=4)
    {
        rv = pRec + j * uPitch;
        rh = pRec + (j+4) * uPitch; /* pR1 */
        rhS = pRec + j * uPitch;

        for (i = 0; i < uWidth; i+=4, rv+=4, rh+=4, rhS+=4, pStrengthV+=4, pStrengthH+=4, pStrengthHS+=4)
        {
            /* do 1 Horiz and 1 Vert Edges on a 4x4 basis */
            /* ._ */
            /* |_..:  ->             */
            /* ..:.. */

            r = rh;
            pSH = pStrengthH;
            pSHS = pStrengthHS;
            if((j+4) != uHeight)
            {
                pR2 = r + uPitch;
                pR3 = pR2 + uPitch;
                pR4 = pR3 + uPitch;
                pL1 = r - uPitch;
                pL2 = pL1 - uPitch;
                pL3 = pL2 - uPitch;
                pL4 = pL3 - uPitch;

                sp = pSH[0];
                Cr = sp >> 2;
                Cl = pSHS[0] >> 2;
                C = Cr+Cl;
#ifdef NO_HORIZ_WEAK
                if(0)
#else
                if(sp & 1)
#endif
                {
                    /* weak horizontal */
                    deltaL[0] = pL2[0] - pL1[0];
                    deltaL[1] = pL2[1] - pL1[1];
                    deltaL[2] = pL2[2] - pL1[2];
                    deltaL[3] = pL2[3] - pL1[3];
                    deltaR[0] = pR2[0] - r[0];
                    deltaR[1] = pR2[1] - r[1];
                    deltaR[2] = pR2[2] - r[2];
                    deltaR[3] = pR2[3] - r[3];
                    Al = Ar = 3;
                    delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
                    if (MEABS(delta) >= beta) Al = 1;
                    delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
                    if (MEABS(delta) >= beta) Ar = 1;
                    if((Al+Ar) > 2)
                    {
                        deltaL2[0] = pL2[0] - pL3[0];
                        deltaL2[1] = pL2[1] - pL3[1];
                        deltaL2[2] = pL2[2] - pL3[2];
                        deltaL2[3] = pL2[3] - pL3[3];
                        deltaR2[0] = pR2[0] - pR3[0];
                        deltaR2[1] = pR2[1] - pR3[1];
                        deltaR2[2] = pR2[2] - pR3[2];
                        deltaR2[3] = pR2[3] - pR3[3];
                        c = (C + Al + Ar)>>1;
                        if(Al!=1 && Ar!=1)
                        {
                            for(z=0; z<4; z++)
                            {
                                L1 = pL1[z];
                                R1 = r[z];
                                delta = (R1 -L1);
                                if(delta == 0) continue;
                                if(((MEABS(delta)*alpha)>>7)>2) continue;
                                d = ((I32) pL2[z] - (I32) pR2[z] + (delta << 2) + 4) >> 3;
                                d = clipd1(d, c);
                                pL1[z] = ClampTbl[CLAMP_BIAS + pL1[z] + d];
                                r[z] = ClampTbl[CLAMP_BIAS + r[z] - d];
                                if(MEABS(deltaL2[z])<= betaX)
                                {
                                    d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                    pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - clipd1(d2, Cl)];
                                }
                                if(MEABS(deltaR2[z])<= betaX)
                                {
                                    d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                    pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - clipd1(d2, Cr)];
                                }
                            }
                        }
                        else
                        {
                            for(z=0; z<4; z++)
                            {
                                L1 = pL1[z];
                                R1 = r[z];
                                delta = (R1 -L1);
                                if(delta == 0) continue;
                                if(((MEABS(delta)*alpha)>>7)>3) continue;
                                d = (delta+1)>> 1;
                                s = c>>1;
                                d = clipd1(d, s);
                                pL1[z] = ClampTbl[CLAMP_BIAS + pL1[z] + d];
                                r[z] = ClampTbl[CLAMP_BIAS + r[z] - d];
                                if(MEABS(deltaL2[z])<= betaX && Al!=1)
                                {
                                    d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                    pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - clipd1(d2, Cl>>1)];
                                }
                                if(MEABS(deltaR2[z])<= betaX && Ar!=1)
                                {
                                    d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                    pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - clipd1(d2, Cr>>1)];
                                }
                            } /* z */
                        }
                    } /* >2 */
                } /* ! sp Strong */
            } /* 1 Horiz Edges */

            r = rv;
            rt=r;
            pSV = pStrengthV;

            sp = pSV[0];
            Cr = sp >> 2;
            if(i)Cl = pSV[-4] >> 2;
            else Cl = Cr;
            C = Cr+Cl;
#ifdef NO_VERTICAL_WEAK
            if(0)
#else
            if(sp & 1)
#endif
            {
                /* weak vertical */
                deltaL[0] = rt[-2] - rt[-1];
                deltaL[1] = rt[uPitch-2] - rt[uPitch-1];
                deltaL[2] = rt[(uPitch<<1)-2] - rt[(uPitch<<1)-1];
                deltaL[3] = rt[u3Pitch-2] -rt[u3Pitch-1];

                deltaR[0] = rt[1] - rt[0];
                deltaR[1] = rt[uPitch+1] - rt[uPitch];
                deltaR[2] = rt[(uPitch<<1)+1] - rt[(uPitch<<1)];
                deltaR[3] = rt[u3Pitch+1] - rt[u3Pitch];
                Al = Ar = 3;
                delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
                if (MEABS(delta) >= beta) Al =1;
                delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
                if (MEABS(delta) >= beta) Ar =1;
                if((Al+Ar) > 2)
                {
                    deltaL2[0] = rt[-2] - rt[-3];
                    deltaL2[1] = rt[uPitch-2] - rt[uPitch-3];
                    deltaL2[2] = rt[(uPitch<<1)-2] - rt[(uPitch<<1)-3];
                    deltaL2[3] = rt[u3Pitch-2] - rt[u3Pitch-3];
                    deltaR2[0] = rt[1] - rt[2];
                    deltaR2[1] = rt[uPitch+1] - rt[uPitch+2];
                    deltaR2[2] = rt[(uPitch<<1)+1] - rt[(uPitch<<1)+2];
                    deltaR2[3] = rt[u3Pitch+1] - rt[u3Pitch+2];
                    c = (C + Al + Ar)>>1;
                    if(Al!=1 && Ar!=1)
                    {
                        for(z=0; z<4; z++, rt+=uPitch)
                        {
                            L1=rt[-1];
                            R1=rt[0];
                            delta = (R1 -L1);
                            if(delta == 0) continue;
                            if(((MEABS(delta)*alpha)>>7)>2) continue;
                            d = ((I32) rt[-2] - (I32) rt[1] + (delta << 2) + 4) >> 3;
                            d = clipd1(d, c);
                            rt[-1] = ClampTbl[CLAMP_BIAS + rt[-1] + d];
                            rt[0] = ClampTbl[CLAMP_BIAS + rt[0] - d];
                            if(MEABS(deltaL2[z])<= betaX)
                            {
                                d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - clipd1(d2, Cl)];
                            }
                            if(MEABS(deltaR2[z])<= betaX)
                            {
                                d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - clipd1(d2, Cr)];
                            }
                        }
                    }
                    else
                    {
                        for(z=0; z<4; z++, rt+=uPitch)
                        {
                            L1=rt[-1];
                            R1=rt[0];
                            delta = (R1 -L1);
                            if(delta == 0) continue;
                            if(((MEABS(delta)*alpha)>>7)>3) continue;
                            s = c>>1;
                            d = (delta + 1) >> 1;
                            d = clipd1(d, s);
                            rt[-1] = ClampTbl[CLAMP_BIAS + rt[-1] + d];
                            rt[0] = ClampTbl[CLAMP_BIAS + rt[0] - d];
                            if(MEABS(deltaL2[z])<= betaX && Al!=1)
                            {
                                d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - clipd1(d2, Cl>>1)];
                            }
                            if(MEABS(deltaR2[z])<= betaX && Ar!=1)
                            {
                                d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - clipd1(d2, Cr>>1)];
                            }
                        } /* z                   */
                    }
                } /* >2 */
            } /* if !sp */

            r = rhS;
            pSH=pStrengthHS;
            if(j)pSH-=uWidth;
            pSHS = pStrengthHS;

            pR2 = r + uPitch;
            pR3 = pR2 + uPitch;
            pR4 = pR3 + uPitch;
            pL1 = r - uPitch;
            pL2 = pL1 - uPitch;
            pL3 = pL2 - uPitch;
            pL4 = pL3 - uPitch;

            sp = pSHS[0];
            Cr = sp >> 2;
            Cl = pSH[0] >> 2;
            C = Cr+Cl;
            /* filter is Strong */
#ifdef NO_HORIZ_STRONG
            if(0)
#else
            if(sp & 2)
#endif
            {
                /* strong horizontal */
                deltaL[0] = pL2[0] - pL1[0];
                deltaL[1] = pL2[1] - pL1[1];
                deltaL[2] = pL2[2] - pL1[2];
                deltaL[3] = pL2[3] - pL1[3];
                deltaR[0] = pR2[0] - r[0];
                deltaR[1] = pR2[1] - r[1];
                deltaR[2] = pR2[2] - r[2];
                deltaR[3] = pR2[3] - r[3];
                Al = Ar = 3;
                delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
                if (MEABS(delta) >= beta) Al = 1;
                delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
                if (MEABS(delta) >= beta) Ar = 1;
                if((Al+Ar) > 2)
                {
                    deltaL2[0] = pL2[0] - pL3[0];
                    deltaL2[1] = pL2[1] - pL3[1];
                    deltaL2[2] = pL2[2] - pL3[2];
                    deltaL2[3] = pL2[3] - pL3[3];
                    deltaR2[0] = pR2[0] - pR3[0];
                    deltaR2[1] = pR2[1] - pR3[1];
                    deltaR2[2] = pR2[2] - pR3[2];
                    deltaR2[3] = pR2[3] - pR3[3];
                    b3SmoothLeft = (Al == 3);
                    if (b3SmoothLeft)
                    {
                        delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
                        if (MEABS(delta) >= beta2) b3SmoothLeft = FALSE;
                    }
                    b3SmoothRight = (Ar == 3);
                    if (b3SmoothRight)
                    {
                        delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
                        if (MEABS(delta) >= beta2) b3SmoothRight = FALSE;
                    }
                    c = (C + Al + Ar)>>1;
                    if(b3SmoothLeft && b3SmoothRight)
                    {
                        for(z=0; z<4; z++)
                        {
                            L1 = (I32) pL1[z];
                            R1 = (I32) r[z];
                            delta = (R1 -L1);
                            if(delta == 0) continue;
                            n = (MEABS(delta)*alpha)>>7;
                            if(n>1) continue;
                            L2 = (I32) pL2[z];
                            R2 = (I32) pR2[z];
                            L3 = (I32) pL3[z];
                            R3 = (I32) pR3[z];
                            L4 = (I32) pL4[z];
                            R4 = (I32) pR4[z];
#ifdef DITHER
                            /* Added chroma conditional to enable simpler optimized code */
                            /* Note there is flexibility here if required for optimized versions */
                            if (!chroma)
                            {
                                d1 = ditherL[((i+z)&0xf)];
                                d2 = ditherR[((i+z)&0xf)];
                            }
                            else
                            {
                                d1 = ditherL[((i*2+z)&0xf)];
                                d2 = ditherR[((i*2+z)&0xf)];
                            }
#else
                            d1 = d2 = 64;
#endif
                            t1 = (U8)((25*L3 + 26*L2 + 26*L1 + 26*R1 + 25*R2 + d1)>>7);
                            t2 = (U8)((25*L2 + 26*L1 + 26*R1 + 26*R2 + 25*R3 + d2)>>7);
                            if(n)
                            {
                                pL1[z] = (U8) clipd2(t1, L1, c);
                                r[z] = (U8) clipd2(t2, R1, c);
                            }
                            else
                            {
                                pL1[z] = t1;
                                r[z] = t2;
                            }
                            t1 = (U8)((25*L4 + 26*L3 + 26*L2 + 26*pL1[z] + 25*R1 + d1)>>7);
                            t2 = (U8)((25*L1 + 26*r[z] + 26*R2 + 26*R3 + 25*R4 + d2)>>7);
                            if(n)
                            {
                                pL2[z] = (U8) clipd2(t1, L2, c);
                                pR2[z] = (U8) clipd2(t2, R2, c);
                            }
                            else
                            {
                                pL2[z] = t1;
                                pR2[z] = t2;
                            }
                            if(!chroma)
                            {
                                pL3[z] = (U8)((26*L4 + 51*L3 + 26*pL2[z] + 25*pL1[z] + 64)>>7);
                                pR3[z] = (U8)((25*r[z] + 26*pR2[z] + 51*R3 + 26*R4  + 64)>>7);
                            }
                        }
                    }
                    else
                    {
                        if(Al!=1 && Ar!=1)
                        {
                            for(z=0; z<4; z++)
                            {
                                L1 = (I32) pL1[z];
                                R1 = (I32) r[z];
                                delta = (R1 -L1);
                                if(delta == 0) continue;
                                if(((MEABS(delta)*alpha)>>7)>2) continue;
                                s = c;
                                d = ((I32) pL2[z] - (I32) pR2[z] + ((delta) << 2) + 4) >> 3;
                                d = clipd1(d, s);
                                pL1[z] = ClampTbl[CLAMP_BIAS + pL1[z] + d];
                                r[z] = ClampTbl[CLAMP_BIAS + r[z] - d];
                                if(MEABS(deltaL2[z])<= betaX)
                                {
                                    d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                    pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - clipd1(d2, Cl)];
                                }
                                if(MEABS(deltaR2[z])<= betaX)
                                {
                                    d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                    pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - clipd1(d2, Cr)];
                                }
                            } /* z */
                        }
                        else
                        {
                            for(z=0; z<4; z++)
                            {
                                L1 = (I32) pL1[z];
                                R1 = (I32) r[z];
                                delta = (R1 -L1);
                                if(delta == 0) continue;
                                if(((MEABS(delta)*alpha)>>7)>3) continue;
                                d = (delta + 1)>> 1;
                                s = c>>1;
                                d = clipd1(d, s);
                                pL1[z] = ClampTbl[CLAMP_BIAS + pL1[z] + d];
                                r[z] = ClampTbl[CLAMP_BIAS + r[z] - d];
                                /* if(Al == 3) {  */
                                if(MEABS(deltaL2[z])<= betaX && Al!=1)
                                {
                                    d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                    pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - clipd1(d2, Cl>>1)];
                                }
                                /* if(Ar == 3) {  */
                                if(MEABS(deltaR2[z])<= betaX && Ar!=1)
                                {
                                    d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                    pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - clipd1(d2, Cr>>1)];
                                }
                            } /* z */
                        }
                    } /* if */
                } /* >2 */
            } /* if Strong */

            rt = rv;
            sp = pStrengthV[0];
            Cr = sp >> 2;
            if(i)Cl = pStrengthV[-4] >> 2;
            else continue;
            C = Cr+Cl;
#ifdef NO_VERTICAL_STRONG
            if(0)
#else
            if(sp & 2)
#endif
            {
                /* strong vertical */
                deltaL[0] = rt[-2] - rt[-1];
                deltaL[1] = rt[uPitch-2] - rt[uPitch-1];
                deltaL[2] = rt[(uPitch<<1)-2] - rt[(uPitch<<1)-1];
                deltaL[3] = rt[u3Pitch-2] -rt[u3Pitch-1];
                deltaR[0] = rt[1] - rt[0];
                deltaR[1] = rt[uPitch+1] - rt[uPitch];
                deltaR[2] = rt[(uPitch<<1)+1] - rt[(uPitch<<1)];
                deltaR[3] = rt[u3Pitch+1] - rt[u3Pitch];
                Al = Ar = 3;
                delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
                if (MEABS(delta) >= beta) Al =1;
                delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
                if (MEABS(delta) >= beta) Ar =1;
                if((Al+Ar) > 2)
                {
                    deltaL2[0] = rt[-2] - rt[-3];
                    deltaL2[1] = rt[uPitch-2] - rt[uPitch-3];
                    deltaL2[2] = rt[(uPitch<<1)-2] - rt[(uPitch<<1)-3];
                    deltaL2[3] = rt[u3Pitch-2] - rt[u3Pitch-3];
                    deltaR2[0] = rt[1] - rt[2];
                    deltaR2[1] = rt[uPitch+1] - rt[uPitch+2];
                    deltaR2[2] = rt[(uPitch<<1)+1] - rt[(uPitch<<1)+2];
                    deltaR2[3] = rt[u3Pitch+1] - rt[u3Pitch+2];
                    b3SmoothLeft = (Al == 3);
                    if (b3SmoothLeft)
                    {
                        delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
                        if (MEABS(delta) >= beta2) b3SmoothLeft = FALSE;
                    }
                    b3SmoothRight = (Ar == 3);
                    if (b3SmoothRight)
                    {
                        delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
                        if (MEABS(delta) >= beta2) b3SmoothRight = FALSE;
                    }
                    c = (C + Al + Ar)>>1;
                    if(b3SmoothLeft && b3SmoothRight)
                    {
                        for(z=0; z<4; z++, rt+=uPitch)
                        {
                            L1 = (I32) rt[-1];
                            R1 = (I32) rt[0];
                            delta = (R1 -L1);
                            if(delta == 0) continue;
                            n=(MEABS(delta)*alpha)>>7;
                            if(n>1) continue;
                            L2 = (I32) rt[-2];
                            R2 = (I32) rt[1];
                            L3 = (I32) rt[-3];
                            R3 = (I32) rt[2];
                            L4 = (I32) rt[-4];
                            R4 = (I32) rt[3];
#ifdef DITHER
                            /* Added chroma conditional to enable simpler optimized code */
                            /* Note there is flexibility here if required for optimized versions */
                            if (!chroma)
                            {
                                d1 = ditherL[(j+z)&0xf];
                                d2 = ditherR[(j+z)&0xf];
                            }
                            else
                            {
                                d1 = ditherL[(j*2+z)&0xf];
                                d2 = ditherR[(j*2+z)&0xf];
                            }
#else
                            d1=d2=64;
#endif
                            t1 = (U8)((25*L3 + 26*L2 + 26*L1 + 26*R1 + 25*R2 + d1)>>7);
                            t2 = (U8)((25*L2 + 26*L1 + 26*R1 + 26*R2 + 25*R3 + d2)>>7);
                            if(n)
                            {
                                rt[-1] = (U8) clipd2(t1, L1, c);
                                rt[0] = (U8) clipd2(t2, R1, c);
                            }
                            else
                            {
                                rt[-1] = t1;
                                rt[0] = t2;
                            }
                            t1 = (U8)((25*L4 + 26*L3 + 26*L2 + 26*rt[-1] + 25*R1 + d1)>>7);
                            t2 = (U8)((25*L1 + 26*rt[0] + 26*R2 + 26*R3 + 25*R4 + d2)>>7);
                            if(n)
                            {
                                rt[-2] = (U8) clipd2(t1, L2, c);
                                rt[1] = (U8) clipd2(t2, R2, c);
                            }
                            else
                            {
                                rt[-2] = t1;
                                rt[1] = t2;
                            }
                            if(!chroma)
                            {
                                rt[-3] = (U8)((26*L4 + 51*L3 + 26*rt[-2] + 25*rt[-1] + 64)>>7);
                                rt[2] = (U8)((25*rt[0] + 26*rt[1] + 51*R3 + 26*R4    + 64)>>7);
                            }
                        } /*z */
                    }
                    else
                    {
                        if(Al!=1 && Ar!=1)
                        {
                            for(z=0; z<4; z++, rt+=uPitch)
                            {
                                L1 = (I32) rt[-1];
                                R1 = (I32) rt[0];
                                delta = (R1 -L1);
                                if(delta == 0) continue;
                                if(((MEABS(delta)*alpha)>>7)>2) continue;
                                d = ((I32) rt[-2] - (I32) rt[1] + ((delta) << 2) + 4) >> 3;
                                d = clipd1(d, c);
                                rt[-1] = ClampTbl[CLAMP_BIAS + rt[-1] + d];
                                rt[0] = ClampTbl[CLAMP_BIAS + rt[0] - d];
                                /* if(Al == 3)  */
                                if(MEABS(deltaL2[z])<= betaX)
                                {
                                    d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                    rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - clipd1(d2, Cl)];
                                }
                                /* if(Ar == 3)  */
                                if(MEABS(deltaR2[z])<= betaX)
                                {
                                    d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                    rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - clipd1(d2, Cr)];
                                }
                            }
                        }
                        else
                        {
                            for(z=0; z<4; z++, rt+=uPitch)
                            {
                                L1 = (I32) rt[-1];
                                R1 = (I32) rt[0];
                                delta = (R1 -L1);
                                if(delta == 0) continue;
                                if(((MEABS(delta)*alpha)>>7)>3) continue;
                                s = c>>1;
                                d = (delta + 1) >> 1;
                                d = clipd1(d, s);
                                rt[-1] = ClampTbl[CLAMP_BIAS + rt[-1] + d];
                                rt[0] = ClampTbl[CLAMP_BIAS + rt[0] - d];
                                if(MEABS(deltaL2[z])<= betaX && Al!=1)
                                {
                                    d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                                    rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - clipd1(d2, Cl>>1)];
                                }
                                if(MEABS(deltaR2[z])<= betaX && Ar!=1)
                                {
                                    d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                                    rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - clipd1(d2, Cr>>1)];
                                }
                            } /* z */
                        }
                    } /* if */
                } /* > 2 */
            } /* Vert Strong Edge             */
        } /* i */
    } /* j */
}

#endif

U8 getMBStrength(const U32 mbType)
{
    if(mbType == MBTYPE_INTRA || mbType == MBTYPE_INTRA_16x16 || mbType == MBTYPE_INTER_16x16)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}



/*---------------------------------------------------------------------------- */
/* Implementation of deblocking filter which uses the input qp, cbp, mbtype, */
/* and mvd info directly during filtering rather than through an intermediate */
/* strength map. */
/* */
/* All 4x4 block edges are optionally filtered using a strong or weak filter. */
/* The implementation filters one 16x16 luma macroblock (MB) area at a time as most */
/* of the input information is MB-based. The filtering of the vertical and */
/* horizontal edges of a 4x4 block must be performed in the following order: */
/*  1. Lower H (horizontal) if weak filtered */
/*  2. Left V (vertical) if weak filtered */
/*  3. Upper H if strong filtered */
/*  4. Left V if strong filtered. */
/* Thus 3 of the 4 edges of the 4x4 block are potentially filtered. */
/* Note that the strong or weak filter decision is per edge (not per block). For */
/* each edge there is also a filter/no filter decision. */
/* A simplifying filter definition rule is that only MB edges are strong-filtered. */
/* */
/* Block filtering order: */
/*  Consider the 16x16 luma MB composed of 16 4x4 blocks numbered as follows: */
/*  0  1  2  3 */
/*  4  5  6  7 */
/*  8  9 10 11 */
/* 12 13 14 15 */
/* */
/*  To maintain the required edge order, this implementation filters in order */
/*  0 to 15, using the MB edge strong filter rule to simplify, so: */
/*  Block           Edge/Strength Filter (1-4 from edge order above) */
/*  =====           ==================== */
/*  0               1-4 */
/*  1-3             1,2,3 */
/*  4,8,12          1,2,4 */
/*  5-7,9-11,13-15  1,2 */
/* */
/*  This results in the H edge between adjacent MBs being filtered with the */
/*  weak filter as the bottom edge of a MB, and with the strong filter as */
/*  the top edge of a MB (only one of the 2 filters will actually be used, */
/*  as is the case for all edges, or the edge may not be filtered at all). */
/* */
/*  Another implementation might choose a different filtering order for the */
/*  MB, as long as the edge filter order rules are honored. This particular */
/*  order results in all 16 left V edges and all 20 H edges of the luma MB */
/*  being potentially filtered. */
/* */
/* For each V or H edge there are three properties to be determined for the */
/* filter: */
/* a. boolean, whether or not to filter the edge */
/* b. boolean, weak or strong filter (if edge is filtered) */
/* c. filter clip level, dependent upon clip levels of the blocks on either */
/*    side of the edge as well as pixel activity levels on either side of the edge. */
/* */
/* This implementation uses packed bit variables for booleans (a) and (b), with */
/* one variable for each, and for each of V and H. The contents are readily */
/* initialized from the bit-packed input variables cbp and mvd. Note that */
/* these new boolean variables are *edge* booleans (not 4x4 *block* booleans); */
/* The input cbp and mvd are block-based. Bit order of the edge booleans is: */
/*  V: same as MB block order, bit refers to left edge of block (thus bit 0 */
/*     is the bit for the left edge of 4x4 block 0. */
/*  H: same as MB block order, bit refers to upper edge of block; plus bits */
/*     16-19 refer to the lower edges of blocks 12 to 15. */
/* */
/* Edge boolean determination */
/* -------------------------- */
/* a. whether or not to filter the edge (bit=1 indicates filter edge) */
/* */
/*  NO, unless: */
/*   i. cbp bit for block on either side set */
/*   ii. mvd bit for block below or to right of edge set */
/*   iii. when B frame, MB edge, reference frames differ for adjacent MB */
/*  also, picture edges are never filtered */
/* */
/* b. strong or weak filter (bit=1 indicates strong) */
/*  -- note content only important when var a bit is set, indicating filter edge */
/* */
/*  WEAK, unless: */
/*   i. MB edge and this or adjacent MB filter strength = 2 */
/*   ii. B frame, MB edge, reference frames differ for adjacent MB */
/* */
/*  MB filter strength is dependent upon MB type, is obtained by calling */
/*  getMBStrength(mbtype). */
/* */
/* Clip levels are determined by finding the clip levels for the current */
/* and adjacent MBs (dependent on MB filter strength), then adjusting on a block basis */
/* dependent upon block location, coded coeffs, and in B frames adjusted */
/* when adjacent MBs use different reference frames. The pixel activity */
/* portion of the clip levels is determined within the filters themselves. */
/* */
/* Chroma blocks */
/* ------------- */
/* This implementation filters the chroma block edges in the same MB loop */
/* as the luma edges of the MB. The choice adds complexity to this function */
/* because it needs to set up additional variables for the chroma blocks, */
/* then traverse the edges of those blocks in the filtering part. But it */
/* takes advantage of the MB-level processing which is common for both */
/* luma and chroma (such as cbp, mbtype, and B frame edge cases). An */
/* alternative approach would be to separate chroma into a separate function */
/* which would be very similar to the luma function. */
/* */
/* Bit-packed variables are also used for chroma, separate from the luma */
/* bit vars and separate for V and U chroma. Bit positions are 0-3 for the */
/* 4 chroma blocks of a plane for the MB, plus bits 5-6 for the adjacent */
/* 2 chroma blocks below. Decisions are the same as for luma with the */
/* exception that mvd bits are not used. */
/*  */
/*---------------------------------------------------------------------------- */
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
)
{

    RV_Status ps = RV_S_OK;
    U32 uNumMBPerRow = (uWidth >> 4);
    U32 uQP;

    /*  for the MB being filtered */
    U32 cbp;        /* luma */
    U32 cbpc[2];    /* chroma, U=0,V=1 */
    U32 mbtype;
    U32 mvd;
    U32 s;              /* strength (index to clip_table) */
    U32 bhfilter;       /* bit-packed boolean, filter H edge (1=yes) */
    U32 bhfilterc[2];   /* bit-packed boolean, filter H edge (1=yes) */
    U32 bvfilter;       /* bit-packed boolean, filter V edge (1=yes) */
    U32 bvfilterc[2];   /* bit-packed boolean, filter V edge (1=yes) */
    U32 hfilter;        /* bit-packed H edge filter (0=weak,1=strong) */
    U32 hfilterchroma[2];   /* bit-packed H edge filter (0=weak,1=strong) */
    U32 vfilter;        /* bit-packed V edge filter (0=weak,1=strong) */
    U32 vfilterchroma[2];   /* bit-packed V edge filter (0=weak,1=strong) */
    /* bnzs is a bit-packed block boolean used to determine clip levels. One bit */
    /* for each MB block, plus bits 16-19 for the 4 blocks below this MB (bit 16 */
    /* for block 0 of that MB, etc.). When the bit is 1, the mbtype-dependent */
    /* filter strength determines the clip level for the block; when the bit */
    /* is 0, clip level = clip_table[0][QP]. */
    U32 bnzs;           /* bit-packed block boolean, use MB strength (1=yes, 0=use 0 strength) */
    RV_Boolean bOnTopEdge;
    RV_Boolean bOnLeftEdge;
    RV_Boolean bAboveRefDiff = FALSE;
    RV_Boolean bLeftRefDiff = FALSE;

    /* for adjacent MBs */
    U32 cbp_left;
    U32 cbpc_left[2];
    U32 cbp_above;
    U32 cbpc_above[2];
    U32 mvd_above;
    U32 mvd_left;
    U32 mbtype_left;
    U32 mbtype_above;
    U32 s_left;
    U32 s_above;
    U32 bnzs_left;
    U32 bnzs_above;

    /* for adjacent blocks, formed by combining this MB and adjacent MB bits */
    U32 cbph_adj;
    U32 cbpv_adj;

    U8 beta2;
    U8 beta2chroma;

#define uTopBlocksMask (CBP4x4Mask[0]|CBP4x4Mask[1]|CBP4x4Mask[2]|CBP4x4Mask[3])
#define uLeftBlocksMask  (CBP4x4Mask[0]|CBP4x4Mask[4]|CBP4x4Mask[8]|CBP4x4Mask[12])
#define uBottomBlocksMask (CBP4x4Mask[12]|CBP4x4Mask[13]|CBP4x4Mask[14]|CBP4x4Mask[15])
#define uRightBlocksMask (CBP4x4Mask[3]|CBP4x4Mask[7]|CBP4x4Mask[11]|CBP4x4Mask[15])

    /* chroma masks, using bits 0-3 for chroma */
#define uTopBlocksMaskChroma (CBP4x4Mask[0]|CBP4x4Mask[1])
#define uLeftBlocksMaskChroma (CBP4x4Mask[0]|CBP4x4Mask[2])
#define uBottomBlocksMaskChroma (CBP4x4Mask[2]|CBP4x4Mask[3])
#define uRightBlocksMaskChroma (CBP4x4Mask[1]|CBP4x4Mask[3])

    /* Each loop iteration filters the Vertical (left) and */
    /* Horizontal (all) 4x4 block edges of a 16x16 macroblock. */
    cbp = *pCBPMap;
    mbtype = *pMBType;
    uQP = *pQPMap;

    if ((uWidth*uHeight)<=(176*144))
        beta2 = beta_tab[uQP]*4;
    else
        beta2 = beta_tab[uQP]*3;

    beta2chroma = beta_tab[uQP]*3;

    vppDataIn->defilter_alpha   = alpha_tab[uQP];
    vppDataIn->defilter_beta    = beta_tab[uQP];
    vppDataIn->defilter_beta2   = beta2;
    vppDataIn->defilter_beta2_c = beta2chroma;

    if (uFrameType != INTRAPIC)
        mvd = *pMVDMap;
    else
        mvd = 0;

    if (uMBCol > 0)
    {
        bOnLeftEdge = FALSE;
        mbtype_left = pMBType[-1*(I32)uMBTypePitch];
        cbp_left = pCBPMap[-1*(I32)uCBPMapPitch];
        if (uFrameType != INTRAPIC)
            mvd_left = pMVDMap[-1*(I32)uMVDMapPitch];
        else
            mvd_left = 0;
    }
    else
    {
        bOnLeftEdge = TRUE;
        mbtype_left = mbtype;
        cbp_left = 0;
        mvd_left = 0;
    }
    if (uMB >= uNumMBPerRow)
    {
        bOnTopEdge = FALSE;
        mbtype_above = pMBType[-((I32)uNumMBPerRow * (I32)uMBTypePitch)];
        cbp_above = pCBPMap[-((I32)uNumMBPerRow * (I32)uCBPMapPitch)];
        if (uFrameType != INTRAPIC)
            mvd_above = pMVDMap[-((I32)uNumMBPerRow * (I32)uMVDMapPitch)];
        else
            mvd_above = 0;
    }
    else
    {
        bOnTopEdge = TRUE;
        mbtype_above = mbtype;
        cbp_above = 0;
        mvd_above = 0;
    }

    /* Extract chroma bits from MB cbp vars, U bits are 16-19, V are 20-23 */
    cbpc[0] = (cbp >> 16) & 0xf;
    cbpc[1] = (cbp >> 20) & 0xf;
    cbpc_left[0] = (cbp_left >> 16) & 0xf;
    cbpc_left[1] = (cbp_left >> 20) & 0xf;
    cbpc_above[0] = (cbp_above >> 16) & 0xf;
    cbpc_above[1] = (cbp_above >> 20) & 0xf;

    /* Initialize filter edge bools dependent upon cbp and mvd (block and adjacent) */
    cbp &= 0xffff;                      /* clear chroma bits */

    cbp_above &= uBottomBlocksMask;     /* only want bottom edge blocks */
    cbpc_above[0] &= uBottomBlocksMaskChroma;
    cbpc_above[1] &= uBottomBlocksMaskChroma;
    mvd_above &= uBottomBlocksMask;     /* only want bottom edge blocks */

    cbp_left &= uRightBlocksMask;       /* only want right edge blocks */
    cbpc_left[0] &= uRightBlocksMaskChroma;
    cbpc_left[1] &= uRightBlocksMaskChroma;
    mvd_left &= uRightBlocksMask;       /* only want right edge blocks */

    /* Form cbp bit vars for adjacent blocks vertically (to left) by shifting the */
    /* cbp bits for this MB left 1 bit and replacing the adjacent bits */
    /* for blocks 0,4,8,12 with the cbp bits from the MB to the left, */
    /* bits 3,7,11,15: */
    /*  adj V: 14 13 12 15L 10 9 8 11L 6 5 4 7L 2 1 0 3L */
    cbpv_adj = (cbp<<1) & (~uLeftBlocksMask);
    cbpv_adj |= (cbp_left>>3);

    /* Form the cbp bit vars for adjacent blocks horizontally (above) by shifting */
    /* the cbp bits left 4 bits and replacing the adjacent bits for blocks */
    /* 0-3 with the cbp bits from the MB above, bits 12-15. */
    /*  adj H: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 15A 14A 13A 12A */
    cbph_adj = (cbp<<4) & (~uTopBlocksMask);
    cbph_adj |= (cbp_above>>12);

    /* filter an edge if cbp for the block or the adjacent block is set, or */
    /* if mvd for the block is set */
    bhfilter = cbp | cbph_adj | mvd;
    bvfilter = cbp | cbpv_adj | mvd;

    /* Repeat for chroma, U */
    /*  adj V: 2 3L 0 1L */
    cbpv_adj = (cbpc[0]<<1) & (~uLeftBlocksMaskChroma);
    cbpv_adj |= (cbpc_left[0]>>1);

    /*  adj H: 1 0 3A 2A */
    cbph_adj = (cbpc[0]<<2) & (~uTopBlocksMaskChroma);
    cbph_adj |= (cbpc_above[0]>>2);
    /* filter an edge if cbp for the block or the adjacent block is set */
    bhfilterc[0] = cbpc[0] | cbph_adj;
    bvfilterc[0] = cbpc[0] | cbpv_adj;

    /* Repeat for chroma, V */
    cbpv_adj = (cbpc[1]<<1) & (~uLeftBlocksMaskChroma);
    cbpv_adj |= (cbpc_left[1]>>1);

    cbph_adj = (cbpc[1]<<2) & (~uTopBlocksMaskChroma);
    cbph_adj |= (cbpc_above[1]>>2);
    bhfilterc[1] = cbpc[1] | cbph_adj;
    bvfilterc[1] = cbpc[1] | cbpv_adj;

    /* but don't filter picture edges */
    if (bOnLeftEdge)
    {
        bvfilter &= (~uLeftBlocksMask);
        bvfilterc[0] &= (~uLeftBlocksMaskChroma);
        bvfilterc[1] &= (~uLeftBlocksMaskChroma);
    }
    if (bOnTopEdge)
    {
        bhfilter &= (~uTopBlocksMask);
        bhfilterc[0] &= (~uTopBlocksMaskChroma);
        bhfilterc[1] &= (~uTopBlocksMaskChroma);
    }

    /* MB strength will be used to determine clip level except */
    /* when the block has zero cbp and zero mvd (ie, edge is being filtered */
    /* because the adjacent block cbp is non-zero). In this case, a strength */
    /* of zero should be used. */
    bnzs = cbp | mvd;
    bnzs_left = cbp_left | mvd_left;
    bnzs_above = cbp_above | mvd_above;

    /* select filter (0=weak or 1=strong) for each edge, H and V */
    /* init to weak for all edges */
    hfilter = 0;
    hfilterchroma[0] = hfilterchroma[1] = 0;
    vfilter = 0;
    vfilterchroma[0] = vfilterchroma[1] = 0;

    s = getMBStrength(mbtype);              //MB_strength = 2 or 1
    s_left = getMBStrength(mbtype_left);
    s_above = getMBStrength(mbtype_above);

    /* change to strong on MB edges if this or adj MB strength is 2 */
    if ((s == 2) || (s_above == 2))
    {
        hfilter |= uTopBlocksMask;
        hfilterchroma[0] |= uTopBlocksMaskChroma;
        hfilterchroma[1] |= uTopBlocksMaskChroma;
    }
    if ((s == 2) || (s_left == 2))
    {
        vfilter |= uLeftBlocksMask;
        vfilterchroma[0] |= uLeftBlocksMaskChroma;
        vfilterchroma[1] |= uLeftBlocksMaskChroma;
    }

    if (uFrameType == TRUEBPIC)
    {
        /* Adjust for case when adjacent MBs use differen reference frame. */
        if (deblockRefDiff(mbtype, mbtype_left))
        {
            bLeftRefDiff = TRUE;
            /* force filter of MB left edges with strong filter              */
#ifdef DEBLOC_REF_DIFF_REFQP
            vfilter |= (bvfilter & uLeftBlocksMask);
            vfilterchroma[0] |= (bvfilterc[0] & uLeftBlocksMaskChroma);
            vfilterchroma[1] |= (bvfilterc[1] & uLeftBlocksMaskChroma);
#else
            vfilter |= uLeftBlocksMask;
            vfilterchroma[0] |= uLeftBlocksMaskChroma;
            vfilterchroma[1] |= uLeftBlocksMaskChroma;
#endif
            bvfilter |= uLeftBlocksMask;
            bvfilterc[0] |= uLeftBlocksMaskChroma;
            bvfilterc[1] |= uLeftBlocksMaskChroma;

        }
        else
            bLeftRefDiff = FALSE;
        if (deblockRefDiff(mbtype, mbtype_above))
        {
            bAboveRefDiff = TRUE;
            /* force filter of MB top edges with strong filter */
#ifdef DEBLOC_REF_DIFF_REFQP
            hfilter |= (bhfilter & uTopBlocksMask);
            hfilterchroma[0] |= (bhfilterc[0] & uTopBlocksMaskChroma);
            hfilterchroma[1] |= (bhfilterc[1] & uTopBlocksMaskChroma);
#else
            hfilter |= uTopBlocksMask;
            hfilterchroma[0] |= uTopBlocksMaskChroma;
            hfilterchroma[1] |= uTopBlocksMaskChroma;
#endif
            bhfilter |= uTopBlocksMask;
            bhfilterc[0] |= uTopBlocksMaskChroma;
            bhfilterc[1] |= uTopBlocksMaskChroma;

        }
        else
            bAboveRefDiff = FALSE;
    }

    vppDataIn->defilter_refdiff_left = bLeftRefDiff;
    vppDataIn->defilter_refdiff_up   = bAboveRefDiff;
    // merge bhfilter
    bhfilter &= 0xffff;
    bhfilterc[0] &= 0xf;
    bhfilterc[1] &= 0xf;

    bhfilter |= bhfilterc[0]<<16;
    bhfilter |= bhfilterc[1]<<20;
    vppDataIn->defilter_bh_filter = bhfilter;

    // merge bvfilter
    bvfilter &= 0xffff;
    bvfilterc[0] &= 0xf;
    bvfilterc[1] &= 0xf;

    bvfilter |= bvfilterc[0]<<16;
    bvfilter |= bvfilterc[1]<<20;
    vppDataIn->defilter_bv_filter = bvfilter;

    // merge hfilter
    hfilter &= 0xffff;
    hfilterchroma[0] &= 0xf;
    hfilterchroma[1] &= 0xf;

    hfilter |= hfilterchroma[0]<<16;
    hfilter |= hfilterchroma[1]<<20;
    vppDataIn->defilter_h_filter = hfilter;

    // merge vfilter
    vfilter &= 0xffff;
    vfilterchroma[0] &= 0xf;
    vfilterchroma[1] &= 0xf;

    vfilter |= vfilterchroma[0]<<16;
    vfilter |= vfilterchroma[1]<<20;
    vppDataIn->defilter_v_filter = vfilter;

    //merge bnzs
    bnzs &= 0xffff;
    bnzs |= cbpc[0]<<16;
    bnzs |= cbpc[1]<<20;
    vppDataIn->defilter_bnzs = bnzs;

    //merge bnzs_left
    bnzs_left &= 0xffff;
    bnzs_left |= cbpc_left[0]<<16;
    bnzs_left |= cbpc_left[1]<<20;
    vppDataIn->defilter_bnzs_left = bnzs_left;

    //merge bnzs_above
    bnzs_above &= 0xffff;
    bnzs_above|= cbpc_above[0]<<16;
    bnzs_above|= cbpc_above[1]<<20;
    vppDataIn->defilter_bnzs_up = bnzs_above;

    /* convert strengths to clips */
    vppDataIn->defilter_mbclip0 = clip_table[0][uQP];
    vppDataIn->defilter_mbclip2 = clip_table[2][uQP];

    vppDataIn->defilter_mbclip      = clip_table[s][uQP];       //MB strength,s = 2 or 1
    vppDataIn->defilter_mbclip_left = clip_table[s_left][uQP];  //LEFT MB strength,s = 2 or 1
    vppDataIn->defilter_mbclip_up   = clip_table[s_above][uQP]; //ABOVE MB strength,s = 2 or 1

    /*__asm emms */
    return ps;

}   /* C_EdgeFilter4x4StrongOptimizedNSM */



#if 0 /* used only for MMX version of low CPU usage deblocking filter */

/*---------------------------------------------------------------------------- */
/* USES SIMPLE MMX 4x4 EDGE FILTER */
/* Strength Map structure is different too. */
/*---------------------------------------------------------------------------- */
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
)
{
    /* determine the size of strength map */
    U32 mb_per_row = uWidth >> 4;
    U32 b_per_row = uWidth >> 2;
    U32 b_per_col = uHeight >> 2;
    U32 sizeV = b_per_row * uHeight;
    U32 sizeH = b_per_col * uWidth;
    U32 size = (((sizeV + sizeH) * 3) >> 1) + 12;
    U8 str   = 0;
    U8 str_c = 0;
    U8 mbQP = pQPMap[0];
    U8 smbQP = t->m_pQPSTab[mbQP];
    /* vars  */
    U32 mbmvd;
    U32 i, j;
    U32 mb;
    U32 mbcbpcoef;
    U8 mbmbtype;
    U8 mb_leftmbtype;
    U8 s ;
    U32 b ;
    U32 cbpmask;
    PU8 pSy = t->m_pStrengthMapVy;
    U32 mb_left;
    U32 b_left;
    U32 cbpmask_left;
    U8 mb_topmbtype;
    U32 mb_top, b_top, cbpmask_top;

    /* allocate/re-allocate buffer if necessary */
    if (size > t->m_uStrengthMapSize || !t->m_pStrengthMapPreAligned)
    {
        if (t->m_pStrengthMapPreAligned)
        {
            RV_Free(t->m_pStrengthMapPreAligned);
            t->m_pStrengthMapPreAligned = NULL;
        }
        /*t->m_pStrengthMapPreAligned = new U8[size + 8]; */
        t->m_pStrengthMapPreAligned = (U8 *)RV_Allocate(sizeof(U8) * (size+8), 0);

        if (!t->m_pStrengthMapPreAligned)
        {
            RVDebug((RV_TL_ALWAYS,
                     "ERROR SmoothingFilter::SetUpStrengthMap -- "
                     "Failed to alllocate m_pStrengthMap"));
            t->m_pStrengthMapVy = t->m_pStrengthMapHy = NULL;
            t->m_pStrengthMapVu = t->m_pStrengthMapHu = NULL;
            t->m_pStrengthMapVv = t->m_pStrengthMapHv = NULL;
            t->m_uStrengthMapSize = 0;
            return RV_S_OUT_OF_MEMORY;
        }

        t->m_pStrengthMapVy = (PU8) (((U32) t->m_pStrengthMapPreAligned + 7) & ~7);
        t->m_pStrengthMapHy = t->m_pStrengthMapVy + sizeV+8;
        t->m_pStrengthMapVu = t->m_pStrengthMapHy + sizeH;
        t->m_pStrengthMapHu = t->m_pStrengthMapVu + (sizeV >> 2)+4;
        t->m_pStrengthMapVv = t->m_pStrengthMapHu + (sizeH >> 2);
        t->m_pStrengthMapHv = t->m_pStrengthMapVv + (sizeV >> 2)+4;
        t->m_uStrengthMapSize = size;
    }

    /* set up all maps for vertical and horizontal filtering */

    /* initialize m_pStrengthMapVy[] for vertical edges */
    for (j = 0; j < uHeight; j += 8)
    {
        for (i = 4; i < uWidth; i += 4, pSy += 8)
        {
            mb = (j >> 4) * mb_per_row + (i >> 4);
            mbcbpcoef = pCBPMap[mb * uCBPMapPitch];
            mbmbtype = pMBType[mb * uMBTypePitch];
            mb_leftmbtype = mbmbtype;
            /* luma block */
            /* check if current luma block has any coefficients */
            s = smbQP;
            b = (j & 12) + ((i >> 2) & 3);
            cbpmask = CBP4x4Mask[b];
            if(mb)
            {
                mb_left = (j >> 4) * mb_per_row + ((i-4) >> 4);
                mb_leftmbtype = pMBType[mb_left * uMBTypePitch];
                b_left = (j & 12) + (((i-4) >> 2) & 3);
                cbpmask_left = CBP4x4Mask[b_left];
                if(pCBPMap[mb_left * uCBPMapPitch] & cbpmask_left)
                    mbcbpcoef |= cbpmask;
                if(pCBPMap[mb_left * uCBPMapPitch] & cbpmask_left<<4)
                    mbcbpcoef |= (cbpmask<<4);

            }
            mbmvd = pMVDMap[mb * uMVDMapPitch];
            if(mbmvd & cbpmask)
            {
                mbcbpcoef |= cbpmask;
                mbcbpcoef |= (cbpmask<<4);
            }
            if(deblockRefDiff(mbmbtype, mb_leftmbtype))
            {
                mbcbpcoef |= cbpmask;
                mbcbpcoef |= (cbpmask<<4);
            }
            if (!(mbcbpcoef & cbpmask))
            {
                *((U32*)pSy)=0;
            }
            else
            {
                /* 4 pixels per block */
                pSy[0] = pSy[1] = pSy[2] = pSy[3] = s;
                str |= s;
            }
            /* proceed to the next luma block below */
            /* check if curent luma block has any coefficients */
            s = smbQP;
            cbpmask<<=4;
            if (!(mbcbpcoef & cbpmask))
            {
                *((U32*)&pSy[4])=0;
            }
            else
            {
                /* 4 pixels per block */
                pSy[4] = pSy[5] = pSy[6] = pSy[7] = s;
                str |= s;
            }
        } /* for j */
    } /* for i */

    /* initialize m_pStrengthMapHy[] for horizontal edges */
    pSy = t->m_pStrengthMapHy;
    for (j = 4; j < uHeight; j += 4)
    {
        for (i = 0; i < uWidth; i += 4)
        {
            mb = (j >> 4) * mb_per_row + (i >> 4);
            b  = (j & 12) + ((i >> 2) & 3);
            mbcbpcoef = pCBPMap[mb * uCBPMapPitch];
            mbmbtype = pMBType[mb * uMBTypePitch];
            mb_topmbtype = mbmbtype;
            cbpmask = CBP4x4Mask[b];
            /* luma blocks */
            /* check if current block has any coefficients */
            s = smbQP;
            if(mb>mb_per_row)
            {
                mb_top= ((j-4) >> 4) * mb_per_row + (i >> 4);;
                mb_topmbtype = pMBType[mb_top * uMBTypePitch];
                b_top = ((j-4) & 12) + ((i >> 2) & 3);
                cbpmask_top = CBP4x4Mask[b_top];
                if(pCBPMap[mb_top * uCBPMapPitch] & cbpmask_top)
                    mbcbpcoef |= cbpmask;
            }
            mbmvd = pMVDMap[mb * uMVDMapPitch];
            if(mbmvd & cbpmask)
            {
                mbcbpcoef |= cbpmask;
            }
            if(deblockRefDiff(mbmbtype, mb_topmbtype))
            {
                mbcbpcoef |= cbpmask;
            }
            if (!(mbcbpcoef & cbpmask))
            {
                *((U32*)pSy)=0;
            }
            else
            {
                /* 4 pixels per block edge */
                pSy[0] = pSy[1] = pSy[2] = pSy[3] = s;
                str |= s;
            }
            pSy += 4;
        } /* for j */
    } /* for i */

    t->m_bLumaStrengthMapEmpty = (str == 0)?(TRUE):(FALSE);
    t->m_bChromaStrengthMapEmpty = (str_c == 0)?(TRUE):(FALSE);

    return RV_S_OK;
} /* SetUpStrengthMap */

#endif /* if 0 (not used in porting kit) */

#if 0 //voc sheen.

/* Two filter functions follow, for horizontal and vertical. */

/*---------------------------------------------------------------------------- */
/* filter, horizontal */
/*---------------------------------------------------------------------------- */
void RV_CDECL C_EdgeFilter4H(
    U8 *pPels,          /* first of 4 */
    U32 uPitch,
    U32 uMBPos,         /* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
    U32 Cr,             /* clip for right */
    U32 Cl,             /* clip for left */
    I32 alpha,
    I32 betaX,
    I32 beta2,
    RV_Boolean bChroma,
    RV_Boolean bStrong      /* true when strong filter enabled */
)
{
    U32 d1, d2, n;
    U8 Ar, Al, C, c, s, t1, t2;
    PU8 pL1, pL2, pL3, pL4, pR2, pR3, pR4;
    I32 delta;
    U8 beta = betaX*4;
    //U32 u4Pitch = uPitch <<2;
    //U32 u3Pitch = u4Pitch - uPitch;
    I32 deltaL[4], deltaL2[4], deltaR[4], deltaR2[4];
    RV_Boolean b3SmoothLeft, b3SmoothRight;

    I32 R1, R2, R3, R4, L1, L2, L3, L4, d, z;

    PU8 r = pPels;

    pR2 = r + uPitch;
    pR3 = pR2 + uPitch;
    pR4 = pR3 + uPitch;
    pL1 = r - uPitch;
    pL2 = pL1 - uPitch;
    pL3 = pL2 - uPitch;
    pL4 = pL3 - uPitch;

    C = (U8)(Cr+Cl);
    Al = Ar = 3;
    deltaL[0] = pL2[0] - pL1[0];
    deltaL[1] = pL2[1] - pL1[1];
    deltaL[2] = pL2[2] - pL1[2];
    deltaL[3] = pL2[3] - pL1[3];
    deltaR[0] = pR2[0] - r[0];
    deltaR[1] = pR2[1] - r[1];
    deltaR[2] = pR2[2] - r[2];
    deltaR[3] = pR2[3] - r[3];
    delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
    if (abs(delta) >= beta) Al = 1;
    delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
    if (abs(delta) >= beta) Ar = 1;
    if((Al+Ar) > 2)
    {
        if (bStrong)
        {
            b3SmoothLeft = (Al == 3);
            b3SmoothRight = (Ar == 3);
        }
        else
        {
            /* not strong, force select weak below */
            b3SmoothLeft = FALSE;
            b3SmoothRight = FALSE;
        }
        deltaL2[0] = pL2[0] - pL3[0];
        deltaL2[1] = pL2[1] - pL3[1];
        deltaL2[2] = pL2[2] - pL3[2];
        deltaL2[3] = pL2[3] - pL3[3];
        deltaR2[0] = pR2[0] - pR3[0];
        deltaR2[1] = pR2[1] - pR3[1];
        deltaR2[2] = pR2[2] - pR3[2];
        deltaR2[3] = pR2[3] - pR3[3];
        if (b3SmoothLeft)
        {
            delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
            if (abs(delta) >= beta2) b3SmoothLeft = FALSE;
        }
        if (b3SmoothRight)
        {
            delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
            if (abs(delta) >= beta2) b3SmoothRight = FALSE;
        }
        c = (C + Al + Ar)>>1;
        if(b3SmoothLeft && b3SmoothRight)
        {
            for(z=0; z<4; z++)
            {
                L1 = (I32) pL1[z];
                R1 = (I32) r[z];
                delta = (R1 -L1);
                if(delta == 0) continue;
                n = (abs(delta)*alpha)>>7;
                if(n>1) continue;
                L2 = (I32) pL2[z];
                R2 = (I32) pR2[z];
                L3 = (I32) pL3[z];
                R3 = (I32) pR3[z];
                L4 = (I32) pL4[z];
                R4 = (I32) pR4[z];
#ifdef DITHER
                d1 = ditherL[uMBPos+z];
                d2 = ditherR[uMBPos+z];
#else
                d1 = d2 = 64;
#endif
                t1 = (U8)((25*L3 + 26*L2 + 26*L1 + 26*R1 + 25*R2 + d1)>>7);
                t2 = (U8)((25*L2 + 26*L1 + 26*R1 + 26*R2 + 25*R3 + d2)>>7);
                if(n)
                {
                    pL1[z] = (U8) clipd2(t1, L1, c);
                    r[z] = (U8) clipd2(t2, R1, c);
                }
                else
                {
                    pL1[z] = t1;
                    r[z] = t2;
                }
                t1 = (U8)((25*L4 + 26*L3 + 26*L2 + 26*pL1[z] + 25*R1 + d1)>>7);
                t2 = (U8)((25*L1 + 26*r[z] + 26*R2 + 26*R3 + 25*R4 + d2)>>7);
                if(n)
                {
                    pL2[z] = (U8) clipd2(t1, L2, c);
                    pR2[z] = (U8) clipd2(t2, R2, c);
                }
                else
                {
                    pL2[z] = t1;
                    pR2[z] = t2;
                }
                if(!bChroma)
                {
                    pL3[z] = (U8)((26*L4 + 51*L3 + 26*pL2[z] + 25*pL1[z] + 64)>>7);
                    pR3[z] = (U8)((25*r[z] + 26*pR2[z] + 51*R3 + 26*R4  + 64)>>7);
                }
            }
        }
        else
        {
            if(Al!=1 && Ar!=1)
            {
                for(z=0; z<4; z++)
                {
                    L1 = (I32) pL1[z];
                    R1 = (I32) r[z];
                    delta = (R1 -L1);
                    if(delta == 0) continue;
                    if(((abs(delta)*alpha)>>7)>2) continue;
                    s = c;
                    d = ((I32) pL2[z] - (I32) pR2[z] + ((delta) << 2) + 4) >> 3;
                    d = clipd1(d, s);
                    pL1[z] = ClampTbl[CLAMP_BIAS + pL1[z] + d];
                    r[z] = ClampTbl[CLAMP_BIAS + r[z] - d];
                    if(abs(deltaL2[z])<= betaX)
                    {
                        d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                        pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - clipd1(d2, Cl)];
                    }
                    if(abs(deltaR2[z])<= betaX)
                    {
                        d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                        pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - clipd1(d2, Cr)];
                    }
                } /* z */
            }
            else
            {
                for(z=0; z<4; z++)
                {
                    L1 = (I32) pL1[z];
                    R1 = (I32) r[z];
                    delta = (R1 -L1);
                    if(delta == 0) continue;
                    if(((abs(delta)*alpha)>>7)>3) continue;
                    d = (delta + 1)>> 1;
                    s = c>>1;
                    d = clipd1(d, s);
                    pL1[z] = ClampTbl[CLAMP_BIAS + pL1[z] + d];
                    r[z] = ClampTbl[CLAMP_BIAS + r[z] - d];
                    if(abs(deltaL2[z])<= betaX && Al!=1)
                    {
                        d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                        pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - clipd1(d2, Cl>>1)];
                    }
                    if(abs(deltaR2[z])<= betaX && Ar!=1)
                    {
                        d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                        pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - clipd1(d2, Cr>>1)];
                    }
                } /* z */
            }
        } /* if */
    } /* >2 */

}   /* C_EdgeFilter4H */

/*---------------------------------------------------------------------------- */
/* filter, vertical */
/*---------------------------------------------------------------------------- */
void RV_CDECL C_EdgeFilter4V(
    U8 *pPels,          /* first of 4 */
    U32 uPitch,
    U32 uMBPos,         /* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
    U32 Cr,             /* clip for right */
    U32 Cl,             /* clip for left */
    I32 alpha,
    I32 betaX,
    I32 beta2,
    RV_Boolean bChroma,
    RV_Boolean bStrong      /* true when strong filter enabled */
)
{
    U32 d1, d2, n;
    U8 Ar, Al, C, c, s, t1, t2;
    I32 delta;
    U8 beta = betaX*4;
    U32 u4Pitch = uPitch <<2;
    U32 u3Pitch = u4Pitch - uPitch;
    I32 deltaL[4], deltaL2[4], deltaR[4], deltaR2[4];
    RV_Boolean b3SmoothLeft, b3SmoothRight;

    I32 R1, R2, R3, R4, L1, L2, L3, L4, d, z;

    PU8 rt = pPels;

    C = (U8)(Cr+Cl);
    Al = Ar = 3;
    deltaL[0] = rt[-2] - rt[-1];
    deltaL[1] = rt[uPitch-2] - rt[uPitch-1];
    deltaL[2] = rt[(uPitch<<1)-2] - rt[(uPitch<<1)-1];
    deltaL[3] = rt[u3Pitch-2] -rt[u3Pitch-1];
    deltaR[0] = rt[1] - rt[0];
    deltaR[1] = rt[uPitch+1] - rt[uPitch];
    deltaR[2] = rt[(uPitch<<1)+1] - rt[(uPitch<<1)];
    deltaR[3] = rt[u3Pitch+1] - rt[u3Pitch];
    delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
    if (abs(delta) >= beta) Al =1;
    delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
    if (abs(delta) >= beta) Ar =1;
    if((Al+Ar) > 2)
    {
        if (bStrong)
        {
            b3SmoothLeft = (Al == 3);
            b3SmoothRight = (Ar == 3);
        }
        else
        {
            /* not strong, force select weak below */
            b3SmoothLeft = FALSE;
            b3SmoothRight = FALSE;
        }
        deltaL2[0] = rt[-2] - rt[-3];
        deltaL2[1] = rt[uPitch-2] - rt[uPitch-3];
        deltaL2[2] = rt[(uPitch<<1)-2] - rt[(uPitch<<1)-3];
        deltaL2[3] = rt[u3Pitch-2] - rt[u3Pitch-3];
        deltaR2[0] = rt[1] - rt[2];
        deltaR2[1] = rt[uPitch+1] - rt[uPitch+2];
        deltaR2[2] = rt[(uPitch<<1)+1] - rt[(uPitch<<1)+2];
        deltaR2[3] = rt[u3Pitch+1] - rt[u3Pitch+2];
        if (b3SmoothLeft)
        {
            delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
            if (abs(delta) >= beta2) b3SmoothLeft = FALSE;
        }
        if (b3SmoothRight)
        {
            delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
            if (abs(delta) >= beta2) b3SmoothRight = FALSE;
        }
        c = (C + Al + Ar)>>1;
        if(b3SmoothLeft && b3SmoothRight)
        {
            for(z=0; z<4; z++, rt+=uPitch)
            {
                L1 = (I32) rt[-1];
                R1 = (I32) rt[0];
                delta = (R1 -L1);
                if(delta == 0) continue;
                n=(abs(delta)*alpha)>>7;
                if(n>1) continue;
                L2 = (I32) rt[-2];
                R2 = (I32) rt[1];
                L3 = (I32) rt[-3];
                R3 = (I32) rt[2];
                L4 = (I32) rt[-4];
                R4 = (I32) rt[3];
#ifdef DITHER
                d1 = ditherL[uMBPos+z];
                d2 = ditherR[uMBPos+z];
#else
                d1=d2=64;
#endif
                t1 = (U8)((25*L3 + 26*L2 + 26*L1 + 26*R1 + 25*R2 + d1)>>7);
                t2 = (U8)((25*L2 + 26*L1 + 26*R1 + 26*R2 + 25*R3 + d2)>>7);
                if(n)
                {
                    rt[-1] = (U8) clipd2(t1, L1, c);
                    rt[0] = (U8) clipd2(t2, R1, c);
                }
                else
                {
                    rt[-1] = t1;
                    rt[0] = t2;
                }
                t1 = (U8)((25*L4 + 26*L3 + 26*L2 + 26*rt[-1] + 25*R1 + d1)>>7);
                t2 = (U8)((25*L1 + 26*rt[0] + 26*R2 + 26*R3 + 25*R4 + d2)>>7);
                if(n)
                {
                    rt[-2] = (U8) clipd2(t1, L2, c);
                    rt[1] = (U8) clipd2(t2, R2, c);
                }
                else
                {
                    rt[-2] = t1;
                    rt[1] = t2;
                }
                if(!bChroma)
                {
                    rt[-3] = (U8)((26*L4 + 51*L3 + 26*rt[-2] + 25*rt[-1] + 64)>>7);
                    rt[2] = (U8)((25*rt[0] + 26*rt[1] + 51*R3 + 26*R4    + 64)>>7);
                }
            } /*z */
        }
        else
        {
            if(Al!=1 && Ar!=1)
            {
                for(z=0; z<4; z++, rt+=uPitch)
                {
                    L1 = (I32) rt[-1];
                    R1 = (I32) rt[0];
                    delta = (R1 -L1);
                    if(delta == 0) continue;
                    if(((abs(delta)*alpha)>>7)>2) continue;
                    d = ((I32) rt[-2] - (I32) rt[1] + ((delta) << 2) + 4) >> 3;
                    d = clipd1(d, c);
                    rt[-1] = ClampTbl[CLAMP_BIAS + rt[-1] + d];
                    rt[0] = ClampTbl[CLAMP_BIAS + rt[0] - d];
                    if(abs(deltaL2[z])<= betaX)
                    {
                        d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                        rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - clipd1(d2, Cl)];
                    }
                    if(abs(deltaR2[z])<= betaX)
                    {
                        d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                        rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - clipd1(d2, Cr)];
                    }
                }
            }
            else
            {
                for(z=0; z<4; z++, rt+=uPitch)
                {
                    L1 = (I32) rt[-1];
                    R1 = (I32) rt[0];
                    delta = (R1 -L1);
                    if(delta == 0) continue;
                    if(((abs(delta)*alpha)>>7)>3) continue;
                    s = c>>1;
                    d = (delta + 1) >> 1;
                    d = clipd1(d, s);
                    rt[-1] = ClampTbl[CLAMP_BIAS + rt[-1] + d];
                    rt[0] = ClampTbl[CLAMP_BIAS + rt[0] - d];
                    if(abs(deltaL2[z])<= betaX && Al!=1)
                    {
                        d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
                        rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - clipd1(d2, Cl>>1)];
                    }
                    if(abs(deltaR2[z])<= betaX && Ar!=1)
                    {
                        d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
                        rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - clipd1(d2, Cr>>1)];
                    }
                } /* z */
            }
        } /* if */
    } /* > 2 */

}   /* C_EdgeFilter4V */

#endif
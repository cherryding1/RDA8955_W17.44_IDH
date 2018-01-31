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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif
#ifdef MP4_3GP_SUPPORT

#include <stdlib.h>
#include "mpeg4_global.h"
//#include "../encoder.h"
#include "mbprediction.h"
//#include "../utils/mbfunctions.h"
//#include "../bitstream/cbp.h"

#ifndef MEDIA_VOCVID_SUPPORT
#include "mbcoding.h"
#include "zigzag.h"

//const uint32_t roundtab[16] =
//{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };

/* K = 4 */
const uint16_t roundtab_76[16] =
{ 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 };

/* K = 2 */
//const uint32_t roundtab_78[8] =
//{ 0, 0, 1, 1, 0, 0, 0, 1  };

/* K = 1 */
const uint32_t roundtab_79[4] =
{ 0, 1, 0, 0 };

VECTOR
get_pmv2(uint16_t mb_width,VECTOR **pMVBuffPred,const int block)
{
    VECTOR ret_mv;
    int32_t mv1_x=0,mv2_x=0,mv3_x=0,mv1_y=0,mv2_y=0,mv3_y=0;
    int num_cand = 0;

    switch(block)
    {
        case 0:
            if(pMVBuffPred[0][1].x == 0x80000000)
            {
                mv1_x = mv1_y = 0;
            }
            else
            {
                mv1_x = pMVBuffPred[0][1].x;
                mv1_y = pMVBuffPred[0][1].y;
                num_cand ++;
            }
            if(pMVBuffPred[1][2].x == 0x80000000)
            {
                mv2_x = mv2_y = 0;
            }
            else
            {
                mv2_x = pMVBuffPred[1][2].x;
                mv2_y = pMVBuffPred[1][2].y;
                num_cand ++;
            }
            if(pMVBuffPred[2][2].x == 0x80000000)
            {
                mv3_x = mv3_y = 0;
            }
            else
            {
                mv3_x = pMVBuffPred[2][2].x;
                mv3_y = pMVBuffPred[2][2].y;
                num_cand ++;
            }
            if(num_cand < 2)
            {
                ret_mv.x = mv1_x + mv2_x + mv3_x;
                ret_mv.y = mv1_y + mv2_y + mv3_y;
                return ret_mv;
            }
            break;
        case 1:
            mv1_x = pMVBuffPred[0][0].x;
            mv1_y = pMVBuffPred[0][0].y;
            num_cand ++;
            if(pMVBuffPred[1][3].x == 0x80000000)
            {
                mv2_x = mv2_y = 0;
            }
            else
            {
                mv2_x = pMVBuffPred[1][3].x;
                mv2_y = pMVBuffPred[1][3].y;
                num_cand ++;
            }
            if(pMVBuffPred[2][2].x == 0x80000000)
            {
                mv3_x = mv3_y = 0;
            }
            else
            {
                mv3_x = pMVBuffPred[2][2].x;
                mv3_y = pMVBuffPred[2][2].y;
                num_cand ++;
            }
            if(num_cand < 2)
            {
                ret_mv.x = mv1_x + mv2_x + mv3_x;
                ret_mv.y = mv1_y + mv2_y + mv3_y;
                return ret_mv;
            }
            break;
        case 2:
            if(pMVBuffPred[0][3].x == 0x80000000)
            {
                mv1_x = mv1_y = 0;
            }
            else
            {
                mv1_x = pMVBuffPred[0][3].x;
                mv1_y = pMVBuffPred[0][3].y;
            }
            mv2_x = pMVBuffPred[0][0].x;
            mv2_y = pMVBuffPred[0][0].y;
            mv3_x = pMVBuffPred[0][1].x;
            mv3_y = pMVBuffPred[0][1].y;
            break;
        case 3:
            mv1_x = pMVBuffPred[0][2].x;
            mv1_y = pMVBuffPred[0][2].y;

            mv2_x = pMVBuffPred[0][0].x;
            mv2_y = pMVBuffPred[0][0].y;

            mv3_x = pMVBuffPred[0][1].x;
            mv3_y = pMVBuffPred[0][1].y;
            break;
    }

    ret_mv.x = mv1_x + mv2_x + mv3_x - MAX_32C(mv1_x, MAX_32C(mv2_x, mv3_x)) - MIN_32C(mv1_x, MIN_32C(mv2_x, mv3_x));
    ret_mv.y = mv1_y + mv2_y + mv3_y - MAX_32C(mv1_y, MAX_32C(mv2_y, mv3_y)) - MIN_32C(mv1_y, MIN_32C(mv2_y, mv3_y));

    return ret_mv;
}
/* ************************************************************
 * simplified version for 1 warp point
 */

static
void Predict_1pt_16x16_C(const NEW_GMC_DATA * const This,
                         uint8_t *Dst, const uint8_t *Src,
                         int dststride, int srcstride, int x, int y, int rounding)
{
    const int W  = This->sW;
    const int H  = This->sH;
    const int rho = 3-This->accuracy;
    const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;


    int32_t uo = This->Uo + (x<<8);  /* ((16*x)<<4) */
    int32_t vo = This->Vo + (y<<8);
    uint32_t ri = MTab[uo & 15];
    uint32_t rj = MTab[vo & 15];
    int i, j;

    int32_t Offset;
    if (vo>=(-16<<4) && vo<=H) Offset = (vo>>4)*srcstride;
    else
    {
        if (vo>H) Offset = ( H>>4)*srcstride;
        else Offset =-16*srcstride;
        rj = MTab[0];
    }
    if (uo>=(-16<<4) && uo<=W) Offset += (uo>>4);
    else
    {
        if (uo>W) Offset += (W>>4);
        else Offset -= 16;
        ri = MTab[0];
    }

    Dst += 16;

    for(j=16; j>0; --j, Offset+=srcstride-16)
    {
        for(i=-16; i<0; ++i, ++Offset)
        {
            uint32_t f0, f1;
            f0  = Src[ Offset       +0 ];
            f0 |= Src[ Offset       +1 ] << 16;
            f1  = Src[ Offset+srcstride +0 ];
            f1 |= Src[ Offset+srcstride +1 ] << 16;
            f0 = (ri*f0)>>16;
            f1 = (ri*f1) & 0x0fff0000;
            f0 |= f1;
            f0 = ( rj*f0 + Rounder ) >> 24;
            Dst[i] = (uint8_t)f0;
        }
        Dst += dststride;
    }
}

static
void Predict_1pt_8x8_C(const NEW_GMC_DATA * const This,
                       uint8_t *uDst, const uint8_t *uSrc,
                       uint8_t *vDst, const uint8_t *vSrc,
                       int dststride, int srcstride, int x, int y, int rounding)
{
    const int W  = This->sW >> 1;
    const int H  = This->sH >> 1;
    const int rho = 3-This->accuracy;
    const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;

    int32_t uo = This->Uco + (x<<7);
    int32_t vo = This->Vco + (y<<7);
    uint32_t rri = MTab[uo & 15];
    uint32_t rrj = MTab[vo & 15];
    int i, j;

    int32_t Offset;
    if (vo>=(-8<<4) && vo<=H) Offset = (vo>>4)*srcstride;
    else
    {
        if (vo>H) Offset = ( H>>4)*srcstride;
        else Offset =-8*srcstride;
        rrj = MTab[0];
    }
    if (uo>=(-8<<4) && uo<=W) Offset += (uo>>4);
    else
    {
        if (uo>W) Offset += ( W>>4);
        else Offset -= 8;
        rri = MTab[0];
    }

    uDst += 8;
    vDst += 8;
    for(j=8; j>0; --j, Offset+=srcstride-8)
    {
        for(i=-8; i<0; ++i, Offset++)
        {
            uint32_t f0, f1;
            f0  = uSrc[ Offset + 0 ];
            f0 |= uSrc[ Offset + 1 ] << 16;
            f1  = uSrc[ Offset + srcstride + 0 ];
            f1 |= uSrc[ Offset + srcstride + 1 ] << 16;
            f0 = (rri*f0)>>16;
            f1 = (rri*f1) & 0x0fff0000;
            f0 |= f1;
            f0 = ( rrj*f0 + Rounder ) >> 24;
            uDst[i] = (uint8_t)f0;

            f0  = vSrc[ Offset + 0 ];
            f0 |= vSrc[ Offset + 1 ] << 16;
            f1  = vSrc[ Offset + srcstride + 0 ];
            f1 |= vSrc[ Offset + srcstride + 1 ] << 16;
            f0 = (rri*f0)>>16;
            f1 = (rri*f1) & 0x0fff0000;
            f0 |= f1;
            f0 = ( rrj*f0 + Rounder ) >> 24;
            vDst[i] = (uint8_t)f0;
        }
        uDst += dststride;
        vDst += dststride;
    }
}

static
void get_average_mv_1pt_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
                          int x, int y, int qpel)
{
    mv->x = RSHIFT(Dsp->Uo<<qpel, 3);
    mv->y = RSHIFT(Dsp->Vo<<qpel, 3);
}

static
void get_average_mv_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
                      int x, int y, int qpel)
{
    int i, j;
    int vx = 0, vy = 0;
    int32_t uo = Dsp->Uo + 16*(Dsp->dU[1]*y + Dsp->dU[0]*x);
    int32_t vo = Dsp->Vo + 16*(Dsp->dV[1]*y + Dsp->dV[0]*x);
    for (j=16; j>0; --j)
    {
        int32_t U, V;
        U = uo; uo += Dsp->dU[1];
        V = vo; vo += Dsp->dV[1];
        for (i=16; i>0; --i)
        {
            int32_t u,v;
            u = U >> 16; U += Dsp->dU[0]; vx += u;
            v = V >> 16; V += Dsp->dV[0]; vy += v;
        }
    }
    vx -= (256*x+120) << (5+Dsp->accuracy); /* 120 = 15*16/2 */
    vy -= (256*y+120) << (5+Dsp->accuracy);

    mv->x = RSHIFT( vx, 8+Dsp->accuracy - qpel );
    mv->y = RSHIFT( vy, 8+Dsp->accuracy - qpel );
}

static
void Predict_16x16_C(const NEW_GMC_DATA * const This,
                     uint8_t *dst, const uint8_t *src,
                     int dststride, int srcstride, int x, int y, int rounding)
{
    const int W = This->sW;
    const int H = This->sH;
    const int rho = 3 - This->accuracy;
    const int Rounder = ( (1<<7) - (rounding<<(2*rho)) ) << 16;

    const int dUx = This->dU[0];
    const int dVx = This->dV[0];
    const int dUy = This->dU[1];
    const int dVy = This->dV[1];

    int Uo = This->Uo + 16*(dUy*y + dUx*x);
    int Vo = This->Vo + 16*(dVy*y + dVx*x);

    int i, j;

    dst += 16;
    for (j=16; j>0; --j)
    {
        int U = Uo, V = Vo;
        Uo += dUy; Vo += dVy;
        for (i=-16; i<0; ++i)
        {
            unsigned int f0, f1, ri = 16, rj = 16;
            int Offset;
            int u = ( U >> 16 ) << rho;
            int v = ( V >> 16 ) << rho;

            U += dUx; V += dVx;

            if (u > 0 && u <= W) { ri = MTab[u&15]; Offset = u>>4;  }
            else
            {
                if (u > W) Offset = W>>4;
                else Offset = 0;
                ri = MTab[0];
            }

            if (v > 0 && v <= H) { rj = MTab[v&15]; Offset += (v>>4)*srcstride; }
            else
            {
                if (v > H) Offset += (H>>4)*srcstride;
                rj = MTab[0];
            }

            f0  = src[Offset + 0];
            f0 |= src[Offset + 1] << 16;
            f1  = src[Offset + srcstride + 0];
            f1 |= src[Offset + srcstride + 1] << 16;
            f0 = (ri*f0)>>16;
            f1 = (ri*f1) & 0x0fff0000;
            f0 |= f1;
            f0 = (rj*f0 + Rounder) >> 24;

            dst[i] = (uint8_t)f0;
        }
        dst += dststride;
    }
}

static
void Predict_8x8_C(const NEW_GMC_DATA * const This,
                   uint8_t *uDst, const uint8_t *uSrc,
                   uint8_t *vDst, const uint8_t *vSrc,
                   int dststride, int srcstride, int x, int y, int rounding)
{
    const int W  = This->sW >> 1;
    const int H  = This->sH >> 1;
    const int rho = 3-This->accuracy;
    const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;

    const int32_t dUx = This->dU[0];
    const int32_t dVx = This->dV[0];
    const int32_t dUy = This->dU[1];
    const int32_t dVy = This->dV[1];

    int32_t Uo = This->Uco + 8*(dUy*y + dUx*x);
    int32_t Vo = This->Vco + 8*(dVy*y + dVx*x);

    int i, j;

    uDst += 8;
    vDst += 8;
    for (j=8; j>0; --j)
    {
        int32_t U = Uo, V = Vo;
        Uo += dUy; Vo += dVy;

        for (i=-8; i<0; ++i)
        {
            int Offset;
            uint32_t f0, f1, ri, rj;
            int32_t u, v;

            u = ( U >> 16 ) << rho;
            v = ( V >> 16 ) << rho;
            U += dUx; V += dVx;

            if (u > 0 && u <= W)
            {
                ri = MTab[u&15];
                Offset = u>>4;
            }
            else
            {
                if (u>W) Offset = W>>4;
                else Offset = 0;
                ri = MTab[0];
            }

            if (v > 0 && v <= H)
            {
                rj = MTab[v&15];
                Offset += (v>>4)*srcstride;
            }
            else
            {
                if (v>H) Offset += (H>>4)*srcstride;
                rj = MTab[0];
            }

            f0  = uSrc[Offset + 0];
            f0 |= uSrc[Offset + 1] << 16;
            f1  = uSrc[Offset + srcstride + 0];
            f1 |= uSrc[Offset + srcstride + 1] << 16;
            f0 = (ri*f0)>>16;
            f1 = (ri*f1) & 0x0fff0000;
            f0 |= f1;
            f0 = (rj*f0 + Rounder) >> 24;

            uDst[i] = (uint8_t)f0;

            f0  = vSrc[Offset + 0];
            f0 |= vSrc[Offset + 1] << 16;
            f1  = vSrc[Offset + srcstride + 0];
            f1 |= vSrc[Offset + srcstride + 1] << 16;
            f0 = (ri*f0)>>16;
            f1 = (ri*f1) & 0x0fff0000;
            f0 |= f1;
            f0 = (rj*f0 + Rounder) >> 24;

            vDst[i] = (uint8_t)f0;
        }
        uDst += dststride;
        vDst += dststride;
    }
}

#endif// MEDIA_VOCVID_SUPPORT
unsigned short generate_GMCparameters( int nb_pts, const int accuracy,
                                       const WARPPOINTS *const pts,
                                       const int width, const int height,
                                       NEW_GMC_DATA *const gmc)
{
    unsigned short gGmc1PtFlag = 0;
    gmc->sW = width << 4;
    gmc->sH = height << 4;
    gmc->accuracy = accuracy;
    gmc->num_wp = nb_pts;

    /* reduce the number of points, if possible */
    if (nb_pts<2 || (pts->duv[2].x==0 && pts->duv[2].y==0 && pts->duv[1].x==0 && pts->duv[1].y==0 ))
    {
        if (nb_pts<2 || (pts->duv[1].x==0 && pts->duv[1].y==0))
        {
            if (nb_pts<1 || (pts->duv[0].x==0 && pts->duv[0].y==0))
            {
                nb_pts = 0;
            }
            else nb_pts = 1;
        }
        else nb_pts = 2;
    }

    /* now, nb_pts stores the actual number of points required for interpolation */

    if (nb_pts<=1)
    {
        if (nb_pts==1)
        {
            /* store as 4b fixed point */
            gmc->Uo = pts->duv[0].x << accuracy;
            gmc->Vo = pts->duv[0].y << accuracy;
            gmc->Uco = ((pts->duv[0].x>>1) | (pts->duv[0].x&1)) << accuracy;     /* DIV2RND() */
            gmc->Vco = ((pts->duv[0].y>>1) | (pts->duv[0].y&1)) << accuracy;     /* DIV2RND() */
        }
        else    /* zero points?! */
        {
            gmc->Uo = gmc->Vo   = 0;
            gmc->Uco = gmc->Vco = 0;
        }

#ifndef MEDIA_VOCVID_SUPPORT
        gmc->predict_16x16  = Predict_1pt_16x16_C;
        gmc->predict_8x8    = Predict_1pt_8x8_C;
        gmc->get_average_mv = get_average_mv_1pt_C;
#endif// MEDIA_VOCVID_SUPPORT
        gGmc1PtFlag = 0;
    }
    else        /* 2 or 3 points */
    {
        const int rho    = 3 - accuracy;    /* = {3,2,1,0} for Acc={0,1,2,3} */
        int Alpha = log2bin(width-1);
        int Ws = 1 << Alpha;

        gmc->dU[0] = 16*Ws + RDIV( 8*Ws*pts->duv[1].x, width );  /* dU/dx */
        gmc->dV[0] =         RDIV( 8*Ws*pts->duv[1].y, width );  /* dV/dx */

        if (nb_pts==2)
        {
            gmc->dU[1] = -gmc->dV[0];   /* -Sin */
            gmc->dV[1] =    gmc->dU[0] ;    /* Cos */
        }
        else
        {
            const int Beta = log2bin(height-1);
            const int Hs = 1<<Beta;
            gmc->dU[1] =         RDIV( 8*Hs*pts->duv[2].x, height );     /* dU/dy */
            gmc->dV[1] = 16*Hs + RDIV( 8*Hs*pts->duv[2].y, height );     /* dV/dy */
            if (Beta>Alpha)
            {
                gmc->dU[0] <<= (Beta-Alpha);
                gmc->dV[0] <<= (Beta-Alpha);
                Alpha = Beta;
                Ws = Hs;
            }
            else
            {
                gmc->dU[1] <<= Alpha - Beta;
                gmc->dV[1] <<= Alpha - Beta;
            }
        }
        /* upscale to 16b fixed-point */
        gmc->dU[0] <<= (16-Alpha - rho);
        gmc->dU[1] <<= (16-Alpha - rho);
        gmc->dV[0] <<= (16-Alpha - rho);
        gmc->dV[1] <<= (16-Alpha - rho);

        gmc->Uo = ( pts->duv[0].x    <<(16+ accuracy)) + (1<<15);
        gmc->Vo = ( pts->duv[0].y    <<(16+ accuracy)) + (1<<15);
        gmc->Uco = ((pts->duv[0].x-1)<<(17+ accuracy)) + (1<<17);
        gmc->Vco = ((pts->duv[0].y-1)<<(17+ accuracy)) + (1<<17);
        gmc->Uco = (gmc->Uco + gmc->dU[0] + gmc->dU[1])>>2;
        gmc->Vco = (gmc->Vco + gmc->dV[0] + gmc->dV[1])>>2;

#ifndef MEDIA_VOCVID_SUPPORT
        gmc->predict_16x16  = Predict_16x16_C;
        gmc->predict_8x8    = Predict_8x8_C;
        gmc->get_average_mv = get_average_mv_C;
#endif// MEDIA_VOCVID_SUPPORT
        gGmc1PtFlag = 1;
    }
    return gGmc1PtFlag;
}

#endif


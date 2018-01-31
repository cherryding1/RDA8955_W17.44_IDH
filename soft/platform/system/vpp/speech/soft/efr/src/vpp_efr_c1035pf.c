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



#include "cs_types.h"
#include "vpp_efr_basic_op.h"
#include "vpp_efr_basic_macro.h"
#include "vpp_efr_sig_proc.h"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

#include <stdio.h>

#define L_CODE    40
#define NB_TRACK  5
#define NB_PULSE  10
#define STEP      5

/* local functions */

void cor_h_x (
    INT16 h[],    /* (i)  : impulse response of weighted synthesis filter */
    INT16 x[],    /* (i)  : target                                        */
    INT16 dn[]    /* (o)  : correlation between target and h[]            */
);

void set_sign (
    INT16 dn[],      /* (i/o)  : correlation between target and h[]       */
    INT16 cn[],      /* (i)  : residual after long term prediction        */
    INT16 sign[],    /* (o)  : sign of d[n]                               */
    INT16 pos_max[], /* (o)  : position of maximum of dn[]                */
    INT16 ipos[]     /* (o)  : starting position for each pulse           */
);

void cor_h (
    INT16 h[],         /* (i)  : impulse response of weighted synthesis
                                  filter */
    INT16 sign[],      /* (i)  : sign of d[n]                             */
    INT16 rr[][L_CODE] /* (o)  : matrix of autocorrelation                */
);

void search_10i40 (
    INT16 dn[],         /* (i) : correlation between target and h[]        */
    INT16 rr[][L_CODE], /* (i) : matrix of autocorrelation                 */
    INT16 ipos[],       /* (i) : starting position for each pulse          */
    INT16 pos_max[],    /* (i) : position of maximum of dn[]               */
    INT16 codvec[]      /* (o) : algebraic codebook vector                 */
);

void build_code (
    INT16 codvec[], /* (i)  : algebraic codebook vector                   */
    INT16 sign[],   /* (i)  : sign of dn[]                                */
    INT16 cod[],    /* (o)  : algebraic (fixed) codebook excitation       */
    INT16 h[],      /* (i)  : impulse response of weighted synthesis filter*/
    INT16 y[],      /* (o)  : filtered fixed codebook excitation           */
    INT16 indx[]    /* (o)  : index of 10 pulses (position+sign+ampl)*10   */
);

void q_p (
    INT16 *ind,   /* Pulse position                                        */
    INT16 n       /* Pulse number                                          */
);


/*************************************************************************
 *
 *  FUNCTION:  code_10i40_35bits()
 *
 *  PURPOSE:  Searches a 35 bit algebraic codebook containing 10 pulses
 *            in a frame of 40 samples.
 *
 *  DESCRIPTION:
 *    The code contains 10 nonzero pulses: i0...i9.
 *    All pulses can have two possible amplitudes: +1 or -1.
 *    The 40 positions in a subframe are divided into 5 tracks of
 *    interleaved positions. Each track contains two pulses.
 *    The pulses can have the following possible positions:
 *
 *       i0, i5 :  0, 5, 10, 15, 20, 25, 30, 35.
 *       i1, i6 :  1, 6, 11, 16, 21, 26, 31, 36.
 *       i2, i7 :  2, 7, 12, 17, 22, 27, 32, 37.
 *       i3, i8 :  3, 8, 13, 18, 23, 28, 33, 38.
 *       i4, i9 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *    Each pair of pulses require 1 bit for their signs and 6 bits for their
 *    positions (3 bits + 3 bits). This results in a 35 bit codebook.
 *    The function determines the optimal pulse signs and positions, builds
 *    the codevector, and computes the filtered codevector.
 *
 *************************************************************************/

void code_10i40_35bits (
    INT16 x[],   /* (i)   : target vector                                 */
    INT16 cn[],  /* (i)   : residual after long term prediction           */
    INT16 h[],   /* (i)   : impulse response of weighted synthesis filter
                             h[-L_subfr..-1] must be set to zero           */
    INT16 cod[], /* (o)   : algebraic (fixed) codebook excitation         */
    INT16 y[],   /* (o)   : filtered fixed codebook excitation            */
    INT16 indx[] /* (o)   : index of 10 pulses (sign + position)          */
)
{
    INT16 ipos[NB_PULSE], pos_max[NB_TRACK];
    INT16 dn[L_CODE], sign[L_CODE];
    INT16 rr[L_CODE][L_CODE];
    INT16 i;
    INT16 codvec [NB_PULSE] = {0, 1, 2, 3, 4, 5, 6,7, 8,9};

    VPP_EFR_PROFILE_FUNCTION_ENTER(code_10i40_35bits);

    cor_h_x (h, x, dn);
    set_sign (dn, cn, sign, pos_max, ipos);
    cor_h (h, sign, rr);

    search_10i40 (dn, rr,ipos, pos_max, codvec);
    build_code (codvec, sign, cod, h, y, indx);
    for (i = 0; i < 10; i++)
    {
        q_p (&indx[i], i);
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(code_10i40_35bits);

    return;
}

/*************************************************************************
 *
 *  FUNCTION:  cor_h_x()
 *
 *  PURPOSE:  Computes correlation between target signal "x[]" and
 *            impulse response"h[]".
 *
 *  DESCRIPTION:
 *    The correlation is given by:
 *       d[n] = sum_{i=n}^{L-1} x[i] h[i-n]      n=0,...,L-1
 *
 *    d[n] is normalized such that the sum of 5 maxima of d[n] corresponding
 *    to each position track does not saturate.
 *
 *************************************************************************/
void cor_h_x (
    INT16 h[],     /* (i)   : impulse response of weighted synthesis filter */
    INT16 x[],     /* (i)   : target                                        */
    INT16 dn[]     /* (o)   : correlation between target and h[]            */
)
{
    INT16 i, j, k;
    INT32 s, y32[L_CODE], max, tot;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    /* first keep the result on 32 bits and find absolute maximum */

    tot = 5;
    for (k = 0; k < NB_TRACK; k++)
    {
        max = 0;
        for (i = k; i < L_CODE; i += STEP)
        {
            //s = 0;
            s_lo = 0;
            for (j = i; j < L_CODE; j++)
            {
                //s = L_mac (s, x[j], h[j - i]);
                //s = L_MAC(s, x[j], h[j - i]);
                VPP_MLA16(s_hi,s_lo, x[j], h[j - i]);
            }

            y32[i] = VPP_SCALE64_TO_16( s_hi, s_lo);

            //s = L_abs (s);
            s = L_ABS(y32[i]);

            //if (L_SUB(s, max) > (INT32) 0L)
            if(s>max) max = s;
        }
        //tot = L_add (tot, L_shr (max, 1));
        tot = L_ADD(tot, L_SHR_D(max, 1));

    }

    //j = sub (norm_l (tot), 2);                   /* multiply tot by 4 */
    j = SUB (norm_l (tot), 2);

    for (i = 0; i < L_CODE; i++)
    {
        //dn[i] = ROUND (L_shl (y32[i], j));
        dn[i] = ROUND (L_SHL_SAT(y32[i], j));
    }
}

/*************************************************************************
 *
 *  FUNCTION  set_sign()
 *
 *  PURPOSE: Builds sign[] vector according to "dn[]" and "cn[]", and modifies
 *           dn[] to include the sign information (dn[i]=sign[i]*dn[i]).
 *           Also finds the position of maximum of correlation in each track
 *           and the starting position for each pulse.
 *
 *************************************************************************/

void set_sign (
    INT16 dn[],      /* (i/o): correlation between target and h[]         */
    INT16 cn[],      /* (i)  : residual after long term prediction        */
    INT16 sign[],    /* (o)  : sign of d[n]                               */
    INT16 pos_max[], /* (o)  : position of maximum correlation            */
    INT16 ipos[]     /* (o)  : starting position for each pulse           */
)
{
    INT16 i, j;
    INT16 val, cor,k_cn, k_dn, max, max_of_all;
    INT16 pos=0;
    INT16 en[L_CODE];                  /* correlation vector */
    INT32 s;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    /* calculate energy for normalization of cn[] and dn[] */

    //s = 256;
    s_lo = 0x80; //(256 >>1)
    for (i = 0; i < L_CODE; i++)
    {
        //s = L_mac (s, cn[i], cn[i]);
        //s = L_MAC(s, cn[i], cn[i]);
        VPP_MLA16(s_hi,s_lo, cn[i], cn[i]);
    }
    s = VPP_SCALE64_TO_16( s_hi, s_lo);

    s = Inv_sqrt (s);
    //k_cn = EXTRACT_H (L_shl (s, 5));
    k_cn = EXTRACT_H (L_SHL_SAT(s, 5));

    //s = 256;
    s_lo = 0x80; //(256 >>1)
    for (i = 0; i < L_CODE; i++)
    {
        //s = L_mac (s, dn[i], dn[i]);
        //s = L_MAC(s, dn[i], dn[i]);
        VPP_MLA16(s_hi,s_lo, dn[i], dn[i]);
    }
    s = VPP_SCALE64_TO_16( s_hi, s_lo);

    s = Inv_sqrt (s);
    //k_dn = extract_h (L_shl (s, 5));
    k_dn = EXTRACT_H(L_SHL_SAT(s, 5));

    for (i = 0; i < L_CODE; i++)
    {
        val = dn[i];
        //cor = round (l_shl (l_mac (l_mult (k_cn, cn[i]), k_dn, val), 10));
        //cor = ROUND (L_SHL (L_MAC (L_MULT (k_cn, cn[i]), k_dn, val), 10));
        VPP_MLX16 (s_hi, s_lo, k_cn, cn[i]);
        VPP_MLA16(s_hi,s_lo, k_dn, val);

        //cor = ROUND (L_SHL(VPP_SCALE64_TO_16(s_hi, s_lo), 10));
        cor = L_SHR_D(((INT32)s_lo + 0x10), 5);

        if (cor >= 0)
        {
            sign[i] = MAX_16;                     /* sign = +1 */
        }
        else
        {
            sign[i] = 0X8001;                    /* sign = -1 */
            //cor = negate (cor);
            cor = NEGATE(cor);
            //val = negate (val);
            val = NEGATE(val);
        }
        /* modify dn[] according to the fixed sign */
        dn[i] = val;
        en[i] = cor;
    }

    max_of_all = -1;
    for (i = 0; i < NB_TRACK; i++)
    {
        max = -1;

        for (j = i; j < L_CODE; j += STEP)
        {
            cor = en[j];

            //val = sub (cor, max);
            val = SUB (cor, max);

            if (val > 0)
            {
                max = cor;
                pos = j;
            }
        }
        /* store maximum correlation position */
        pos_max[i] = pos;
        //val = sub (max, max_of_all);
        val = SUB (max, max_of_all);
        if (val > 0)
        {
            max_of_all = max;
            /* starting position for i0 */
            ipos[0] = i;
        }
    }

    /*----------------------------------------------------------------*
     *     Set starting position of each pulse.                       *
     *----------------------------------------------------------------*/

    pos = ipos[0];
    ipos[5] = pos;

    for (i = 1; i < NB_TRACK; i++)
    {
        //pos = add (pos, 1);
        pos = ADD(pos, 1);

        //if (sub (pos, NB_TRACK) >= 0)
        if (SUB (pos, NB_TRACK) >= 0)
        {
            pos = 0;
        }
        ipos[i] = pos;
        ipos[i + 5] = pos;
    }
}

void q_p (
    INT16 *ind,        /* Pulse position */
    INT16 n            /* Pulse number   */
)
{
    static const INT16 gray[8] = {0, 1, 3, 2, 6, 4, 5, 7};
    INT16 tmp;

    tmp = *ind;

    //if (sub (n, 5) < 0)
    if (SUB (n, 5) < 0)
    {
        tmp = (tmp & 0x8) | gray[tmp & 0x7];

    }
    else
    {
        tmp = gray[tmp & 0x7];
    }

    *ind = tmp;
}

/*************************************************************************
 *
 *  FUNCTION:  cor_h()
 *
 *  PURPOSE:  Computes correlations of h[] needed for the codebook search;
 *            and includes the sign information into the correlations.
 *
 *  DESCRIPTION: The correlations are given by
 *         rr[i][j] = sum_{n=i}^{L-1} h[n-i] h[n-j];   i>=j; i,j=0,...,L-1
 *
 *  and the sign information is included by
 *         rr[i][j] = rr[i][j]*sign[i]*sign[j]
 *
 *************************************************************************/

void cor_h (
    INT16 h[],         /* (i) : impulse response of weighted synthesis
                                 filter                                  */
    INT16 sign[],      /* (i) : sign of d[n]                            */
    INT16 rr[][L_CODE] /* (o) : matrix of autocorrelation               */
)
{
    INT16 i, j, k, dec, h2[L_CODE];
    INT32 s;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;

    /* Scaling for maximum precision */

    //s = 2;
    s_lo = 1; //(2>>1)
    for (i = 0; i < L_CODE; i++)
    {
        //s = L_mac (s, h[i], h[i]);
        //s = L_MAC(s, h[i], h[i]);
        VPP_MLA16(s_hi,s_lo,  h[i], h[i]);
    }
    s = VPP_SCALE64_TO_16(s_hi, s_lo);

    //j = sub (extract_h (s), 32767);
    j = SUB (EXTRACT_H(s), 32767);

    if (j == 0)
    {
        for (i = 0; i < L_CODE; i++)
        {
            //h2[i] = shr (h[i], 1);
            h2[i] = SHR_D(h[i], 1);
        }
    }
    else
    {
        //s = L_shr (s, 1);
        s = L_SHR_D(s, 1);
        //k = extract_h (L_shl (Inv_sqrt (s), 7));
        k = EXTRACT_H (L_SHL_SAT(Inv_sqrt (s), 7));
        //k = mult (k, 32440);                     /* k = 0.99*k */
        k = MULT(k, 32440);

        for (i = 0; i < L_CODE; i++)
        {
            //h2[i] = round (L_shl (l_mult (h[i], k), 9));
            h2[i] = ROUND(L_SHL_SAT(L_MULT (h[i], k), 9));

        }
    }

    /* build matrix rr[] */
    //s = 0;
    s_lo = 0;
    //i = L_CODE - 1;

    for (i =39, k = 0; k < L_CODE; k++, i--)
    {
        //s = L_mac (s, h2[k], h2[k]);
        //s = L_MAC(s, h2[k], h2[k]);
        VPP_MLA16(s_hi,s_lo, h2[k], h2[k]);
        //rr[i][i] = round (s);
        rr[i][i] = ROUND(VPP_SCALE64_TO_16(s_hi, s_lo));
    }

    for (dec = 1; dec < L_CODE; dec++)
    {

        //s = 0;
        s_lo = 0;
        //j = L_CODE - 1;
        //j =39;
        //i = sub (j, dec);
        i = SUB (39, dec);
        for (j =39,k = 0; k < (L_CODE - dec); k++, i--, j--)
        {
            //s = L_mac (s, h2[k], h2[k + dec]);
            //s = L_MAC(s, h2[k], h2[k + dec]);
            VPP_MLA16(s_hi,s_lo,  h2[k], h2[k + dec]);
            //rr[j][i] = mult (round (s), mult (sign[i], sign[j]));
            rr[j][i] = MULT(ROUND(VPP_SCALE64_TO_16(s_hi, s_lo)), MULT(sign[i], sign[j]));
            rr[i][j] = rr[j][i] ;
        }
    }
}

/*************************************************************************
 *
 *  FUNCTION  search_10i40()
 *
 *  PURPOSE: Search the best codevector; determine positions of the 10 pulses
 *           in the 40-sample frame.
 *
 *************************************************************************/

/*#define _1_2    (INT16)(0x4000)
#define _1_4    (INT16)(0x2000)
#define _1_8    (INT16)(0x1000)
#define _1_16   (INT16)(0x800)
#define _1_32   (INT16)(0x400)
#define _1_64   (INT16)(0x200)
#define _1_128  (INT16)(0x100)*/

void search_10i40 (
    INT16 dn[],         /* (i) : correlation between target and h[]        */
    INT16 rr[][L_CODE], /* (i) : matrix of autocorrelation                 */
    INT16 ipos[],       /* (i) : starting position for each pulse          */
    INT16 pos_max[],    /* (i) : position of maximum of dn[]               */
    INT16 codvec[]      /* (o) : algebraic codebook vector                 */
)
{
    INT16 i0, i1, i2, i3, i4, i5, i6, i7, i8, i9;
    INT16 i, j, k, pos, ia, ib;
    INT16 psk, ps, ps0, ps1, ps2, sq, sq2;
    INT16 alpk, alp, alp_16;
    INT16 rrv[L_CODE];
    INT32 s;
    INT32 alp0, alp1, alp2;
    register INT32 s_hi=0;
    register UINT32 s_lo=0;
    INT16* p_rr;


    /* fix i0 on maximum of correlation position */
    i0 = pos_max[ipos[0]];

    /*------------------------------------------------------------------*
     * i1 loop:                                                         *
     *------------------------------------------------------------------*/

    /* Default value */
    psk = -1;
    alpk = 1;

    for (i = 1; i < NB_TRACK; i++)
    {
        i1 = pos_max[ipos[1]];

        ps0 = ADD (dn[i0], dn[i1]);

        alp0 =  ((INT32)rr[i0][i1])<<1;
        alp0 += (INT32)rr[i1][i1];
        alp0 += (INT32)rr[i0][i0];

        /*----------------------------------------------------------------*
         * i2 and i3 loop:                                                *
         *----------------------------------------------------------------*/


        for (i3 = ipos[3]; i3 < L_CODE; i3 += STEP)
        {

            //s = (INT32)rr[i0][i3];
            //s += (INT32)rr[i1][i3];
            //s <<=1;
            //s += (INT32)rr[i3][i3];
            //s <<= 13;

            p_rr = (INT16*)rr[i3];

            s = ((UINT32)p_rr[i0] + (UINT32)p_rr[i1]) << 1;
            s += (UINT32)p_rr[i3];
            s <<= 13;

            rrv[i3] = ROUND(s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[2];
        ib = ipos[3];

        /* initialize 4 indices for i2 loop. */

        for (i2 = ipos[2]; i2 < L_CODE; i2 += STEP)
        {
            /* index increment = STEP  */
            //ps1 = add (ps0, dn[i2]);
            ps1 = ADD (ps0, dn[i2]);


            //alp1 = (INT32)rr[i0][i2];
            //alp1 += (INT32)rr[i1][i2];
            //alp1 <<=1;
            //alp1 += (INT32)rr[i2][i2];
            //alp1 += alp0;

            p_rr = (INT16*)rr[i2];

            alp1 = ((UINT32)p_rr[i0] + (UINT32)p_rr[i1]) << 1;
            alp1 += (INT32)p_rr[i2];
            alp1 += alp0;


            /* initialize 3 indices for i3 inner loop */

            for (i3 = ipos[3]; i3 < L_CODE; i3 += STEP)
            {
                /* index increment = STEP */
                //ps2 = add (ps1, dn[i3]);
                ps2 = ADD (ps1, dn[i3]);

                /* index increment = STEP */
                //alp2_hi =alp1_hi;
                //alp2_lo =alp1_lo;
                //VPP_MLA16 (alp2_hi,alp2_lo,rrv[i3], _1_2);
                //VPP_MLA16 (alp2_hi,alp2_lo,rr[i2][i3], _1_8);
                //alp_16 = ROUND(VPP_SCALE64_TO_16(alp2_hi,alp2_lo));

                alp2 = ((INT32)rrv[i3]<<2);
                alp2 += (INT32)rr[i2][i3];
                alp2 <<= 1;
                alp2 += alp1;
                alp2 <<= 12;

                alp_16 = ROUND(alp2);


                sq2 = MULT(ps2, ps2);

                VPP_MLX16(s_hi,s_lo, alp, sq2);
                VPP_MLA16(s_hi ,s_lo, sq, -alp_16);

                if ((s_hi == 0)&&(s_lo>0))
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i2;
                    ib = i3;
                }
            }
        }
        i2 = ia;
        i3 = ib;

        /*----------------------------------------------------------------*
         * i4 and i5 loop:                                                *
         *----------------------------------------------------------------*/

        ps0 = ps;

        //VPP_MLX16 (alp0_hi, alp0_lo, alp, _1_2);
        alp0 = alp<<15;

        /* initialize 6 indices for next loop (see i2-i3 loop) */
        for (i5 = ipos[5]; i5 < L_CODE; i5 += STEP)
        {

            //s  = (INT32)rr[i0][i5];
            //s += (INT32)rr[i1][i5];
            //s += (INT32)rr[i2][i5];
            //s += (INT32)rr[i3][i5];
            //s <<=1;
            //s += (INT32)rr[i5][i5];
            //s <<=13;

            p_rr = (INT16*)rr[i5];
            s = ((INT32)p_rr[i0] + (INT32)p_rr[i1] +
                 (INT32)p_rr[i2] + (INT32)p_rr[i3])<<1;
            s += (INT32)p_rr[i5];
            s <<= 13;

            rrv[i5] = ROUND(s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[4];
        ib = ipos[5];

        /* initialize 6 indices for i4 loop (see i2-i3 loop) */

        for (i4 = ipos[4]; i4 < L_CODE; i4 += STEP)
        {
            ps1 = ADD (ps0, dn[i4]);


            //alp1 = (INT32)rr[i0][i4];
            //alp1 += (INT32)rr[i1][i4];
            //alp1 += (INT32)rr[i2][i4];
            //alp1 += (INT32)rr[i3][i4];
            //alp1 <<= 1;
            //alp1 +=  rr[i4][i4];
            p_rr = (INT16*)rr[i4];

            alp1 = ((INT32)p_rr[i0] + (INT32)p_rr[i1] +
                    (INT32)p_rr[i2] + (INT32)p_rr[i3])<<1;
            alp1 +=  p_rr[i4];


            /* initialize 3 indices for i5 inner loop (see i2-i3 loop) */

            for (i5 = ipos[5]; i5 < L_CODE; i5 += STEP)
            {
                ps2 = ADD (ps1, dn[i5]);

                //alp2_hi =alp1_hi;
                //alp2_lo =alp1_lo;
                //VPP_MLA16(alp2_hi,alp2_lo,rrv[i5], _1_4);
                //VPP_MLA16(alp2_hi,alp2_lo, rr[i4][i5], _1_16);
                //alp_16 = ROUND (VPP_SCALE64_TO_16(alp2_hi,alp2_lo));

                alp2 = ((INT32)rrv[i5]<<2);
                alp2 += (INT32)rr[i4][i5];
                alp2 <<= 1;
                alp2 +=alp1;
                alp2 <<= 11;
                alp2 += alp0;

                alp_16 = ROUND(alp2);

                sq2 = MULT(ps2, ps2);

                VPP_MLX16(s_hi,s_lo, alp, sq2);
                VPP_MLA16(s_hi ,s_lo, sq, -alp_16);

                if ((s_hi == 0)&&(s_lo>0))
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i4;
                    ib = i5;
                }
            }
        }
        i4 = ia;
        i5 = ib;

        /*----------------------------------------------------------------*
         * i6 and i7 loop:                                                *
         *----------------------------------------------------------------*/

        ps0 = ps;

        //VPP_MLX16 (alp0_hi, alp0_lo, alp, _1_2);
        alp0 = alp << 15;

        /* initialize 8 indices for next loop (see i2-i3 loop) */
        for (i7 = ipos[7]; i7 < L_CODE; i7 += STEP)
        {

            //s = (INT32)rr[i0][i7];
            //s += (INT32)rr[i1][i7];
            //s += (INT32)rr[i2][i7];
            //s += (INT32)rr[i3][i7];
            //s += (INT32)rr[i4][i7];
            //s += (INT32)rr[i5][i7];
            //s <<= 1;
            //s += (INT32)rr[i7][i7];
            //s <<=12;

            p_rr = (INT16*) rr[i7];
            s = ((INT32)p_rr[i0] + (INT32)p_rr[i1]
                 + (INT32)p_rr[i2] + (INT32)p_rr[i3]
                 + (INT32)p_rr[i4] + (INT32)p_rr[i5])<< 1;
            s += (INT32)p_rr[i7];
            s <<=12;

            rrv[i7] = ROUND(s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[6];
        ib = ipos[7];

        /* initialize 8 indices for i6 loop (see i2-i3 loop) */

        for (i6 = ipos[6]; i6 < L_CODE; i6 += STEP)
        {
            ps1 = ADD(ps0, dn[i6]);

            //alp1 = (INT32)(rr[i0][i6]);
            //alp1 += (INT32)(rr[i1][i6]);
            //alp1 += (INT32)(rr[i2][i6]);
            //alp1 += (INT32)(rr[i3][i6]);
            //alp1 += (INT32)(rr[i4][i6]);
            //alp1 += (INT32)(rr[i5][i6]);
            //alp1 <<= 1;
            //alp1 += (INT32)rr[i6][i6];

            p_rr = (INT16*) rr[i6];
            alp1 = ((INT32)p_rr[i0] + (INT32)p_rr[i1]
                    +(INT32)p_rr[i2] + (INT32)p_rr[i3]
                    +(INT32)p_rr[i4] + (INT32)p_rr[i5]) << 1;
            alp1 += (INT32)p_rr[i6];


            /* initialize 3 indices for i7 inner loop (see i2-i3 loop) */

            for (i7 = ipos[7]; i7 < L_CODE; i7 += STEP)
            {
                ps2 = ADD (ps1, dn[i7]);

                //alp2_hi =alp1_hi;
                //alp2_lo =alp1_lo;
                //VPP_MLA16(alp2_hi,alp2_lo, rrv[i7], _1_4);
                //VPP_MLA16(alp2_hi,alp2_lo, rr[i6][i7], _1_32);
                //lp_16 = ROUND (VPP_SCALE64_TO_16(alp2_hi,alp2_lo));

                alp2 = ((INT32)rrv[i7]<<3);
                alp2 += (INT32)rr[i6][i7];
                alp2 <<= 1;
                alp2 += alp1;
                alp2 <<= 10;
                alp2 += alp0;

                alp_16 = ROUND(alp2);

                sq2 = MULT(ps2, ps2);

                //s = L_msu (L_mult (alp, sq2), sq, alp_16);
                VPP_MLX16(s_hi,s_lo, alp, sq2);
                VPP_MLA16(s_hi ,s_lo, sq, -alp_16);

                if ((s_hi == 0)&&(s_lo>0))
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i6;
                    ib = i7;
                }
            }
        }
        i6 = ia;
        i7 = ib;

        /*----------------------------------------------------------------*
         * i8 and i9 loop:                                                *
         *----------------------------------------------------------------*/

        ps0 = ps;

        alp0 = alp << 15;

        /* initialize 10 indices for next loop (see i2-i3 loop) */
        for (i9 = ipos[9]; i9 < L_CODE; i9 += STEP)
        {

            //s = (INT32)rr[i0][i9];
            //s +=(INT32) rr[i1][i9];
            //s += (INT32)rr[i2][i9];
            //s += (INT32)rr[i3][i9];
            //s += (INT32)rr[i4][i9];
            //s += (INT32)rr[i5][i9];
            //s += (INT32)rr[i6][i9];
            //s += (INT32)rr[i7][i9];
            //s <<= 1;
            //s += (INT32)rr[i9][i9];
            //s <<= 12;

            p_rr = (INT16*)rr[i9];

            s =  ((INT32)p_rr[i0] + (INT32) p_rr[i1]
                  +(INT32)p_rr[i2] + (INT32) p_rr[i3]
                  +(INT32)p_rr[i4] + (INT32)p_rr[i5]
                  +(INT32)p_rr[i6] + (INT32)p_rr[i7]) << 1;
            s += (INT32)p_rr[i9];
            s <<= 12;

            rrv[i9] = ROUND(s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[8];
        ib = ipos[9];

        /* initialize 10 indices for i8 loop (see i2-i3 loop) */


        for (i8 = ipos[8]; i8 < L_CODE; i8 += STEP)
        {
            ps1 = ADD(ps0, dn[i8]);

            //alp1 = (INT32)rr[i0][i8];
            //alp1 += (INT32)rr[i1][i8];
            //alp1 += (INT32)rr[i2][i8];
            //alp1 += (INT32)rr[i3][i8];
            //alp1 += (INT32)rr[i4][i8];
            //alp1 += (INT32)rr[i5][i8];
            //alp1 += (INT32)rr[i6][i8];
            //alp1 += (INT32)rr[i7][i8];
            //alp1 <<= 1;
            //alp1 += (INT32)rr[i8][i8];
            p_rr = (INT16*)rr[i8];

            alp1 = ((INT32)p_rr[i0] + (INT32)p_rr[i1]
                    + (INT32)p_rr[i2] +(INT32)p_rr[i3]
                    + (INT32)p_rr[i4] +(INT32)p_rr[i5]
                    + (INT32)p_rr[i6] +(INT32)p_rr[i7]) <<1;
            alp1 += (INT32)p_rr[i8];


            /* initialize 3 indices for i9 inner loop (see i2-i3 loop) */


            for (i9 = ipos[9]; i9 < L_CODE; i9 += STEP)
            {
                ps2 = ADD (ps1, dn[i9]);

                alp2 = ((INT32)rrv[i9]<<3);
                alp2 += (INT32)rr[i8][i9];
                alp2 <<=1;
                alp2 +=alp1;
                alp2 <<=9;
                alp2 += alp0;

                alp_16 = ROUND(alp2);

                sq2 = MULT(ps2, ps2);

                VPP_MLX16(s_hi,s_lo, alp, sq2);
                VPP_MLA16(s_hi ,s_lo, sq, -alp_16);

                if ((s_hi == 0)&&(s_lo>0))
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i8;
                    ib = i9;
                }
            }
        }

        /*----------------------------------------------------------------*
         * memorise codevector if this one is better than the last one.   *
         *----------------------------------------------------------------*/

        VPP_MLX16(s_hi,s_lo, alpk, sq);
        VPP_MLA16(s_hi ,s_lo, psk, -alp);

        if ((s_hi == 0)&&(s_lo>0))
        {
            psk = sq;
            alpk = alp;
            codvec[0] = i0;
            codvec[1] = i1;
            codvec[2] = i2;
            codvec[3] = i3;
            codvec[4] = i4;
            codvec[5] = i5;
            codvec[6] = i6;
            codvec[7] = i7;
            codvec[8] = ia;
            codvec[9] = ib;
        }
        /*----------------------------------------------------------------*
         * Cyclic permutation of i1,i2,i3,i4,i5,i6,i7,i8 and i9.          *
         *----------------------------------------------------------------*/

        pos = ipos[1];
        for (j = 1, k = 2; k < NB_PULSE; j++, k++)
        {
            ipos[j] = ipos[k];
        }
        ipos[NB_PULSE - 1] = pos;
    }
}

/*************************************************************************
 *
 *  FUNCTION:  build_code()
 *
 *  PURPOSE: Builds the codeword, the filtered codeword and index of the
 *           codevector, based on the signs and positions of 10 pulses.
 *
 *************************************************************************/

void build_code (
    INT16 codvec[], /* (i)  : position of pulses                           */
    INT16 sign[],   /* (i)  : sign of d[n]                                 */
    INT16 cod[],    /* (o)  : innovative code vector                       */
    INT16 h[],      /* (i)  : impulse response of weighted synthesis filter*/
    INT16 y[],      /* (o)  : filtered innovative code                     */
    INT16 indx[]    /* (o)  : index of 10 pulses (sign+position)           */
)
{
    INT16 i, j, k, track, index, _sign[NB_PULSE];
    INT16 *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;
    INT32 s;
    //register INT32 s_hi=0;
    //register UINT32 s_lo=0;


    for (i = 0; i < L_CODE; i++)
    {
        cod[i] = 0;
    }
    for (i = 0; i < NB_TRACK; i++)
    {
        indx[i] = -1;
    }

    for (k = 0; k < NB_PULSE; k++)
    {
        /* read pulse position */
        i = codvec[k];
        /* read sign           */
        j = sign[i];

        //index = mult (i, 6554);                  /* index = pos/5       */
        index = MULT(i, 6554);
        /* track = pos%5 */
        //track = sub (i, extract_l (L_shr (L_mult (index, 5), 1)));
        track = SUB (i, EXTRACT_L (L_SHR_D (L_MULT (index, 5), 1)));

        if (j > 0)
        {
            //cod[i] = add (cod[i], 4096);
            cod[i] = ADD (cod[i], 4096);
            _sign[k] = 8192;
        }
        else
        {
            //cod[i] = sub (cod[i], 4096);
            cod[i] = SUB (cod[i], 4096);
            _sign[k] = -8192;
            //index = add (index, 8);
            index = ADD (index, 8);
        }


        if (indx[track] < 0)
        {
            indx[track] = index;
        }
        else
        {

            if (((index ^ indx[track]) & 8) == 0)
            {
                /* sign of 1st pulse == sign of 2nd pulse */

                //if (sub (indx[track], index) <= 0)
                if (SUB (indx[track], index) <= 0)
                {
                    indx[track + 5] = index;
                }
                else
                {
                    indx[track + 5] = indx[track];

                    indx[track] = index;
                }
            }
            else
            {
                /* sign of 1st pulse != sign of 2nd pulse */

                //if (sub ((indx[track] & 7), (index & 7)) <= 0)
                if (SUB ((indx[track] & 7), (index & 7)) <= 0)
                {
                    indx[track + 5] = indx[track];

                    indx[track] = index;
                }
                else
                {
                    indx[track + 5] = index;
                }
            }
        }
    }

    p0 = h - codvec[0];
    p1 = h - codvec[1];
    p2 = h - codvec[2];
    p3 = h - codvec[3];
    p4 = h - codvec[4];
    p5 = h - codvec[5];
    p6 = h - codvec[6];
    p7 = h - codvec[7];
    p8 = h - codvec[8];
    p9 = h - codvec[9];

    for (i = 0; i < L_CODE; i++)
    {

        if(_sign[0]<0)
            s = -(*p0);
        else
            s = *p0;
        p0++;

        if(_sign[1]<0)
            s -= *p1;
        else
            s += *p1;
        p1++;

        if(_sign[2]<0)
            s -= *p2;
        else
            s += *p2;
        p2++;

        if(_sign[3]<0)
            s -= *p3;
        else
            s += *p3;
        p3++;

        if(_sign[4]<0)
            s -= *p4;
        else
            s += *p4;
        p4++;

        if(_sign[5]<0)
            s -= *p5;
        else
            s += *p5;
        p5++;

        if(_sign[6]<0)
            s -= *p6;
        else
            s += *p6;
        p6++;

        if(_sign[7]<0)
            s -= *p7;
        else
            s += *p7;
        p7++;

        if(_sign[8]<0)
            s -= *p8;
        else
            s += *p8;
        p8++;

        if(_sign[9]<0)
            s -= *p9;
        else
            s += *p9;
        p9++;

        s = L_SHL_SAT(s,14);

        y[i] = ROUND(s);
    }
}












































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







#include <stdio.h>
#include <assert.h>

#include "vpp_fr_private.h"

#include "vpp_fr_gsm.h"
#include "vpp_fr_proto.h"

#include "vpp_fr_dtx.h"

#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

/*  4.2.13 .. 4.2.17  RPE ENCODING SECTION
 */

/* 4.2.13 */

static void Weighting_filter P2((e, x),
                                register INT16  * e,            /* signal [-5..0.39.44] IN  */
                                INT16           * x             /* signal [0..39]       OUT */
                               )
/*
 *  The coefficients of the weighting filter are stored in a table
 *  (see table 4.4).  The following scaling is used:
 *
 *      H[0..10] = integer( real_H[ 0..10] * 8192 );
 */
{
    /* INT16                wt[ 50 ]; */

    //register INT32        L_result;
    register UINT32     lo=0;
    register INT32      hi=0;
    register int            k;

    VPP_FR_PROFILE_FUNCTION_ENTER(Weighting_filter);

    /*  Initialization of a temporary working array wt[0...49]
     */

    /* for (k =  0; k <=  4; k++) wt[k] = 0;
     * for (k =  5; k <= 44; k++) wt[k] = *e++;
     * for (k = 45; k <= 49; k++) wt[k] = 0;
     *
     *  (e[-5..-1] and e[40..44] are allocated by the caller,
     *  are initially zero and are not written anywhere.)
     */
    e -= 5;

    /*  Compute the signal x[0..39]
     */
    for (k = 0; k <= 39; k++)
    {

        //L_result = 8192 >> 1;
        //L_result = 0x1000;
        lo = 0x1000;

        /* for (i = 0; i <= 10; i++) {
         *      L_temp   = GSM_L_MULT( wt[k+i], gsm_H[i] );
         *      L_result = GSM_L_ADD( L_result, L_temp );
         * }
         */

#undef  STEP
#define STEP( i, H )    (e[ k + i ] * ((long) H))

        /*  Every one of these multiplications is done twice --
         *  but I don't see an elegant way to optimize this.
         *  Do you?
         */


        /*L_result += STEP(     0,      -134 ) ;
        L_result += STEP(       1,      -374 )  ;
        //+ STEP(       2,      0    )
        L_result += STEP(       3,      2054 ) ;
        L_result += STEP(       4,      5741 ) ;
        L_result += STEP(       5,      8192 ) ;
        L_result += STEP(       6,      5741 ) ;
        L_result += STEP(       7,      2054 ) ;
        //+ STEP(       8,      0    )
        L_result += STEP(       9,      -374 ) ;
        L_result += STEP(       10,     -134 ) ;*/



        GSM_MLA16(hi,lo, e[k], -134);
        GSM_MLA16(hi,lo, e[k+10], -134);
        GSM_MLA16(hi,lo, e[k+1], -374);
        GSM_MLA16(hi,lo, e[k+9], -374);
        GSM_MLA16(hi,lo, e[k+3], 2054);
        GSM_MLA16(hi,lo, e[k+7], 2054);
        GSM_MLA16(hi,lo, e[k+4], 5741);
        GSM_MLA16(hi,lo, e[k+6], 5741);
        GSM_MLA16(hi,lo, e[k+5], 8192);


        /* L_result = GSM_L_ADD( L_result, L_result ); (* scaling(x2) *)
         * L_result = GSM_L_ADD( L_result, L_result ); (* scaling(x4) *)
         *
         * x[k] = SASR( L_result, 16 );
         */

        /* 2 adds vs. >>16 => 14, minus one shift to compensate for
         * those we lost when replacing L_MULT by '*'.
         */

        /*L_result = SASR( L_result, 13 );
        x[k] =   (INT16) (  L_result< MIN_WORD ? MIN_WORD
             : (L_result > MAX_WORD ? MAX_WORD : L_result));*/

        lo = SASR( (INT32) lo, 13 );
        x[k] =  (INT16) (  (INT32)lo< MIN_WORD ? MIN_WORD
                           : ((INT32)lo > MAX_WORD ? MAX_WORD : (INT32)lo));
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(Weighting_filter);
}

/* 4.2.14 */

static void RPE_grid_selection P3((x,xM,Mc_out),
                                  INT16           * x,            /* [0..39]              IN  */
                                  INT16           * xM,           /* [0..12]              OUT */
                                  INT16           * Mc_out        /*                      OUT */
                                 )
/*
 *  The signal x[0..39] is used to select the RPE grid which is
 *  represented by Mc.
 */
{
    /* register INT16       temp1;  */
    register int            /* m, */ i;
//      register UINT32     utmp;
    register INT32      L_result, L_temp;
    INT32               EM;     /* xxx should be L_EM? */
    INT16                   Mc;

    INT32               L_common_0_3;

    VPP_FR_PROFILE_FUNCTION_ENTER(RPE_grid_selection);

    EM = 0;
    Mc = 0;

    /* for (m = 0; m <= 3; m++) {
     *      L_result = 0;
     *
     *
     *      for (i = 0; i <= 12; i++) {
     *
     *              temp1    = SASR( x[m + 3*i], 2 );
     *
     *              assert(temp1 != MIN_WORD);
     *
     *              L_temp   = GSM_L_MULT( temp1, temp1 );
     *              L_result = GSM_L_ADD( L_temp, L_result );
     *      }
     *
     *      if (L_result > EM) {
     *              Mc = m;
     *              EM = L_result;
     *      }
     * }
     */

#undef  STEP
#define STEP( m, i )            L_temp = SASR( x[m + 3 * i], 2 );       \
                                L_result += L_temp * L_temp;

    /* common part of 0 and 3 */

    L_result = 0;
    STEP( 0, 1 ); STEP( 0, 2 ); STEP( 0, 3 ); STEP( 0, 4 );
    STEP( 0, 5 ); STEP( 0, 6 ); STEP( 0, 7 ); STEP( 0, 8 );
    STEP( 0, 9 ); STEP( 0, 10); STEP( 0, 11); STEP( 0, 12);
    L_common_0_3 = L_result;

    /* i = 0 */

    STEP( 0, 0 );
    L_result <<= 1; /* implicit in L_MULT */
    EM = L_result;

    /* i = 1 */

    L_result = 0;
    STEP( 1, 0 );
    STEP( 1, 1 ); STEP( 1, 2 ); STEP( 1, 3 ); STEP( 1, 4 );
    STEP( 1, 5 ); STEP( 1, 6 ); STEP( 1, 7 ); STEP( 1, 8 );
    STEP( 1, 9 ); STEP( 1, 10); STEP( 1, 11); STEP( 1, 12);
    L_result <<= 1;
    if (L_result > EM)
    {
        Mc = 1;
        EM = L_result;
    }

    /* i = 2 */

    L_result = 0;
    STEP( 2, 0 );
    STEP( 2, 1 ); STEP( 2, 2 ); STEP( 2, 3 ); STEP( 2, 4 );
    STEP( 2, 5 ); STEP( 2, 6 ); STEP( 2, 7 ); STEP( 2, 8 );
    STEP( 2, 9 ); STEP( 2, 10); STEP( 2, 11); STEP( 2, 12);
    L_result <<= 1;
    if (L_result > EM)
    {
        Mc = 2;
        EM = L_result;
    }

    /* i = 3 */

    L_result = L_common_0_3;
    STEP( 3, 12 );
    L_result <<= 1;
    if (L_result > EM)
    {
        Mc = 3;
        EM = L_result;
    }

    /**/

    /*  Down-sampling by a factor 3 to get the selected xM[0..12]
     *  RPE sequence.
     */
    for (i = 0; i <= 12; i ++) xM[i] = x[Mc + 3*i];
    *Mc_out = Mc;

    VPP_FR_PROFILE_FUNCTION_EXIT(RPE_grid_selection);
}

/* 4.12.15 */

static void APCM_quantization_xmaxc_to_exp_mant P3((xmaxc,exp_out,mant_out),
        INT16           xmaxc,          /* IN   */
        INT16           * exp_out,      /* OUT  */
        INT16           * mant_out )    /* OUT  */
{
    INT16   exp, mant;

    /* Compute exponent and mantissa of the decoded version of xmaxc
     */

    VPP_FR_PROFILE_FUNCTION_ENTER(APCM_quantization_xmaxc_to_exp_mant);

    exp = 0;
    if (xmaxc > 15) exp = SASR(xmaxc, 3) - 1;
    mant = xmaxc - (exp << 3);

    if (mant == 0)
    {
        exp  = -4;
        mant = 7;
    }
    else
    {
        while (mant <= 7)
        {
            mant = mant << 1 | 1;
            exp--;
        }
        mant -= 8;
    }

    //assert( exp  >= -4 && exp <= 6 );
    //assert( mant >= 0 && mant <= 7 );

    *exp_out  = exp;
    *mant_out = mant;

    VPP_FR_PROFILE_FUNCTION_EXIT(APCM_quantization_xmaxc_to_exp_mant);
}

extern INT16 xmax_SID_tx[DTX_HANGOVER][4];
extern INT16 txdtx_ctrl;
//extern INT16 xmax_aver_tx;
extern INT16 subfrm;

static void APCM_quantization P5((xM,xMc,mant_out,exp_out,xmaxc_out),
                                 INT16           * xM,           /* [0..12]              IN      */

                                 INT16           * xMc,          /* [0..12]              OUT     */
                                 INT16           * mant_out,     /*                      OUT     */
                                 INT16           * exp_out,      /*                      OUT     */
                                 INT16           * xmaxc_out     /*                      OUT     */
                                )
{
    int     i, itest;
//      long    L_Temp,L_Temp1;

//      register INT32      ltmp;   /* for GSM_SATADD */
    INT16   xmax, xmaxc, temp, temp1, temp2;
    INT16   exp, mant;


    VPP_FR_PROFILE_FUNCTION_ENTER(APCM_quantization);


    /*  Find the maximum absolute value xmax of xM[0..12].
     */

    xmax = 0;
    for (i = 0; i <= 12; i++)
    {
        temp = xM[i];
        temp = GSM_ABS(temp);
        if (temp > xmax) xmax = temp;
    }


//////////add fts

    //assert(subfrm < 4);
    //assert(subfrm > -1);

    if (((txdtx_ctrl & TX_HANGOVER_ACTIVE) != 0 ) ||
            ((txdtx_ctrl & TX_SID_UPDATE) != 0 ))
    {
        for (i = DTX_HANGOVER - 1; i > 0; i--)
        {
            xmax_SID_tx[i][subfrm] = xmax_SID_tx[i - 1][subfrm];
        }

        xmax_SID_tx[0][subfrm] = xmax;

    }

//////////add fts end


    /*  Qantizing and coding of xmax to get xmaxc.
     */

    exp   = 0;
    temp  = SASR( xmax, 9 );
    itest = 0;

    for (i = 0; i <= 5; i++)
    {

        itest |= (temp <= 0);
        temp = SASR( temp, 1 );

        //assert(exp <= 5);
        if (itest == 0) exp++;          /* exp = add (exp, 1) */
    }

    //assert(exp <= 6 && exp >= 0);
    temp = exp + 5;

    //assert(temp <= 11 && temp >= 0);
    xmaxc = GSM_ADD( (INT16) SASR(xmax, temp), (INT16) (exp << 3) );

    /*   Quantizing and coding of the xM[0..12] RPE sequence
     *   to get the xMc[0..12]
     */


    APCM_quantization_xmaxc_to_exp_mant( xmaxc, &exp, &mant );

    /*  This computation uses the fact that the decoded version of xmaxc
     *  can be calculated by using the exponent and the mantissa part of
     *  xmaxc (logarithmic table).
     *  So, this method avoids any division and uses only a scaling
     *  of the RPE samples by a function of the exponent.  A direct
     *  multiplication by the inverse of the mantissa (NRFAC[0..7]
     *  found in table 4.5) gives the 3 bit coded version xMc[0..12]
     *  of the RPE samples.
     */


    /* Direct computation of xMc[0..12] using table 4.5
     */

    //assert( exp <= 4096 && exp >= -4096);
    //assert( mant >= 0 && mant <= 7 );

    temp1 = 6 - exp;                /* normalization by the exponent */
    temp2 = gsm_NRFAC[ mant ];      /* inverse mantissa              */

    for (i = 0; i <= 12; i++)
    {

        register INT32 hi=0;
        register UINT32 lo=0;

        //assert(temp1 >= 0 && temp1 < 16);

        temp = xM[i] << temp1;
        //temp = (INT16) (GSM_MULT( temp, temp2 ));
        GSM_MLX16(hi,lo ,temp, temp2);

        temp = (INT16) SASR((INT32)lo, 27);
        xMc[i] = temp + 4;              /* see note below */
    }

    /*  NOTE: This equation is used to make all the xMc[i] positive.
     */

    *mant_out  = mant;
    *exp_out   = exp;
    *xmaxc_out = xmaxc;

    VPP_FR_PROFILE_FUNCTION_EXIT(APCM_quantization);

}

/* 4.2.16 */

static void APCM_inverse_quantization P4((xMc,mant,exp,xMp),
        register INT16  * xMc,  /* [0..12]                      IN      */
        INT16           mant,
        INT16           exp,
        register INT16  * xMp)  /* [0..12]                      OUT     */
/*
 *  This part is for decoding the RPE sequence of coded xMc[0..12]
 *  samples to obtain the xMp[0..12] array.  Table 4.6 is used to get
 *  the mantissa of xmaxc (FAC[0..7]).
 */
{
    int     i;
    INT16   temp, temp1, temp2, temp3;
//      UINT32      utmp;
    INT32       ltmp;

    VPP_FR_PROFILE_FUNCTION_ENTER(APCM_inverse_quantization);

    //assert( mant >= 0 && mant <= 7 );

    temp1 = gsm_FAC[ mant ];        /* see 4.2-15 for mant */
    //temp2 = gsm_sub( 6, exp );        /* see 4.2-15 for exp  */
    temp2 = GSM_SUB( 6, exp );
    //temp3 = gsm_asl( 1, (int) (gsm_sub(temp2, 1)));
    temp3 = gsm_asl( 1, (int) (GSM_SUB(temp2, 1)));

    for (i = 13; i--;)
    {

        //assert( *xMc <= 7 && *xMc >= 0 );     /* 3 bit unsigned */

        /* temp = gsm_sub( *xMc++ << 1, 7 ); */
        temp = (*xMc++ << 1) - 7;               /* restore sign   */
        //assert( temp <= 7 && temp >= -7 );        /* 4 bit signed   */

        temp <<= 12;                            /* 16 bit signed  */
        temp = (INT16) (GSM_MULT_R( temp1, temp ));
        temp = (INT16) (GSM_SATADD( temp, temp3 ));
        *xMp++ = gsm_asr( temp, temp2 );
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(APCM_inverse_quantization);
}

/* 4.2.17 */

static void RPE_grid_positioning P3((Mc,xMp,ep),
                                    INT16           Mc,             /* grid position        IN      */
                                    register INT16  * xMp,          /* [0..12]              IN      */
                                    register INT16  * ep            /* [0..39]              OUT     */
                                   )
/*
 *  This procedure computes the reconstructed long term residual signal
 *  ep[0..39] for the LTP analysis filter.  The inputs are the Mc
 *  which is the grid position selection and the xMp[0..12] decoded
 *  RPE samples which are upsampled by a factor of 3 by inserting zero
 *  values.
 */
{
    int     i = 13;

    VPP_FR_PROFILE_FUNCTION_ENTER(RPE_grid_positioning);

    //assert(0 <= Mc && Mc <= 3);

    switch (Mc)
    {
        case 3: *ep++ = 0;
        case 2:  do
            {
                *ep++ = 0;
            case 1:         *ep++ = 0;
            case 0:         *ep++ = *xMp++;
            }
            while (--i);
    }
    while (++Mc < 4) *ep++ = 0;

    /*

    int i, k;
    for (k = 0; k <= 39; k++) ep[k] = 0;
    for (i = 0; i <= 12; i++) {
            ep[ Mc + (3*i) ] = xMp[i];
    }
    */

    VPP_FR_PROFILE_FUNCTION_EXIT(RPE_grid_positioning);
}

/* 4.2.18 */

/*  This procedure adds the reconstructed long term residual signal
 *  ep[0..39] to the estimated signal dpp[0..39] from the long term
 *  analysis filter to compute the reconstructed short term residual
 *  signal dp[-40..-1]; also the reconstructed short term residual
 *  array dp[-120..-41] is updated.
 */

#if 0   /* Has been inlined in code.c */
void Gsm_Update_of_reconstructed_short_time_residual_signal P3((dpp, ep, dp),
        INT16   * dpp,          /* [0...39]     IN      */
        INT16   * ep,           /* [0...39]     IN      */
        INT16   * dp)           /* [-120...-1]  IN/OUT  */
{
    int             k;

    for (k = 0; k <= 79; k++)
        dp[ -120 + k ] = dp[ -80 + k ];

    for (k = 0; k <= 39; k++)
        dp[ -40 + k ] = GSM_ADD( ep[k], dpp[k] );
}
#endif  /* Has been inlined in code.c */

void Gsm_RPE_Encoding P5((S,e,xmaxc,Mc,xMc),

                         struct gsm_state * S,

                         INT16   * e,            /* -5..-1][0..39][40..44        IN/OUT  */
                         INT16   * xmaxc,        /*                              OUT */
                         INT16   * Mc,           /*                              OUT */
                         INT16   * xMc)          /* [0..12]                      OUT */
{
    INT16   x[40];
    INT16   xM[13], xMp[13];
    INT16   mant, exp;

    VPP_FR_PROFILE_FUNCTION_ENTER(Gsm_RPE_Encoding);

    Weighting_filter(e, x);
    RPE_grid_selection(x, xM, Mc);

    APCM_quantization(      xM, xMc, &mant, &exp, xmaxc);
    APCM_inverse_quantization(  xMc,  mant,  exp, xMp);

    RPE_grid_positioning( *Mc, xMp, e );

    VPP_FR_PROFILE_FUNCTION_EXIT(Gsm_RPE_Encoding);

}

void Gsm_RPE_Decoding P5((S, xmaxcr, Mcr, xMcr, erp),
                         struct gsm_state        * S,

                         INT16           xmaxcr,
                         INT16           Mcr,
                         INT16           * xMcr,  /* [0..12], 3 bits             IN      */
                         INT16           * erp    /* [0..39]                     OUT     */
                        )
{

    INT16   exp, mant;
    INT16   xMp[ 13 ];

    VPP_FR_PROFILE_FUNCTION_ENTER(Gsm_RPE_Decoding);

    APCM_quantization_xmaxc_to_exp_mant( xmaxcr, &exp, &mant );
    APCM_inverse_quantization( xMcr, mant, exp, xMp );
    RPE_grid_positioning( Mcr, xMp, erp );

    VPP_FR_PROFILE_FUNCTION_EXIT(Gsm_RPE_Decoding);

}

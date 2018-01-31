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


extern INT16 rxdtx_ctrl;
/*
 *  SHORT TERM ANALYSIS FILTERING SECTION
 */

/* 4.2.8 */


static void Decoding_of_the_coded_Log_Area_Ratios P2((LARc,LARpp),
        INT16   * LARc,         /* coded log area ratio [0..7]  IN      */
        INT16   * LARpp)        /* out: decoded ..                      */
{
    register INT16  temp1;//, temp2;
    register long   ltmp;   /* for GSM_SATADD */

    VPP_FR_PROFILE_FUNCTION_ENTER(Decoding_of_the_coded_Log_Area_Ratios);

    /*  This procedure requires for efficient implementation
     *  two tables.
     *
     *  INVA[1..8] = integer( (32768 * 8) / real_A[1..8])
     *  MIC[1..8]  = minimum value of the LARc[1..8]
     */

    /*  Compute the LARpp[1..8]
     */

    /*      for (i = 1; i <= 8; i++, B++, MIC++, INVA++, LARc++, LARpp++) {
     *
     *              temp1  = GSM_SATADD( *LARc, *MIC ) << 10;
     *              temp2  = *B << 1;
     *              temp1  = GSM_SUB( temp1, temp2 );
     *
     *              assert(*INVA != MIN_WORD);
     *
     *              temp1  = GSM_MULT_R( *INVA, temp1 );
     *              *LARpp = GSM_SATADD( temp1, temp1 );
     *      }
     */
    /* Microsoft's ever-so-carefully-debugged Visual C++ 1.52 loses it
       trying to evaluate the following macro.  For some reason it
       fails to recognise the reference to GSM_SUB in the following
       expansion */
#ifdef INLINE_ADD_H_FUNCTIONS
//NEED TO FIX THIS!!!!
#endif

#undef  STEP
#define STEP( B, MIC, INVA )    \
                temp1    = (INT16) (GSM_SATADD( (long) *LARc++, (long) MIC ) << 10);       \
                temp1    = (INT16) GSM_SATSUB( (long) temp1, (long) B << 1 );            \
                temp1    = (INT16) GSM_MULT_R( (long) INVA, (long) temp1 );           \
                *LARpp++ = (INT16) GSM_SATADD( (long) temp1, (long) temp1 );


    STEP(      0,  -32,  13107 );
    STEP(      0,  -32,  13107 );
    STEP(   2048,  -16,  13107 );
    STEP(  -2560,  -16,  13107 );

    STEP(     94,   -8,  19223 );
    STEP(  -1792,   -8,  17476 );
    STEP(   -341,   -4,  31454 );
    STEP(  -1144,   -4,  29708 );

    /* NOTE: the addition of *MIC is used to restore
     *       the sign of *LARc.
     */

    VPP_FR_PROFILE_FUNCTION_EXIT(Decoding_of_the_coded_Log_Area_Ratios);
}

/* 4.2.9 */
/* Computation of the quantized reflection coefficients
 */

/* 4.2.9.1  Interpolation of the LARpp[1..8] to get the LARp[1..8]
 */

/*
 *  Within each frame of 160 analyzed speech samples the short term
 *  analysis and synthesis filters operate with four different sets of
 *  coefficients, derived from the previous set of decoded LARs(LARpp(j-1))
 *  and the actual set of decoded LARs (LARpp(j))
 *
 * (Initial value: LARpp(j-1)[1..8] = 0.)
 */

static void Coefficients_0_12 P3((LARpp_j_1, LARpp_j, LARp),
                                 register INT16 * LARpp_j_1,
                                 register INT16 * LARpp_j,
                                 register INT16 * LARp)
{
    register int    i;
    register INT32 ltmp;



    for (i = 1; i <= 8; i++, LARp++, LARpp_j_1++, LARpp_j++)
    {
        //*LARp = (INT16) GSM_SATADD( SASR( *LARpp_j_1, 2 ), SASR( *LARpp_j, 2 ));
        *LARp = (INT16) GSM_ADD( SASR( *LARpp_j_1, 2 ), SASR( *LARpp_j, 2 ));
        *LARp = (INT16) GSM_SATADD( *LARp,  SASR( *LARpp_j_1, 1));
    }


}

static void Coefficients_13_26 P3((LARpp_j_1, LARpp_j, LARp),
                                  register INT16 * LARpp_j_1,
                                  register INT16 * LARpp_j,
                                  register INT16 * LARp)
{
    register int i;
    //register INT32 ltmp;


    for (i = 1; i <= 8; i++, LARpp_j_1++, LARpp_j++, LARp++)
    {
        //*LARp = (INT16) GSM_SATADD( SASR( *LARpp_j_1, 1), SASR( *LARpp_j, 1 ));
        *LARp = (INT16) GSM_ADD( SASR( *LARpp_j_1, 1), SASR( *LARpp_j, 1 ));
    }
}

static void Coefficients_27_39 P3((LARpp_j_1, LARpp_j, LARp),
                                  register INT16 * LARpp_j_1,
                                  register INT16 * LARpp_j,
                                  register INT16 * LARp)
{
    register int i;
    register INT32 ltmp;

    for (i = 1; i <= 8; i++, LARpp_j_1++, LARpp_j++, LARp++)
    {
        //*LARp = (INT16) GSM_SATADD( SASR( *LARpp_j_1, 2L ), SASR( *LARpp_j, 2L ));
        *LARp = (INT16) GSM_ADD( SASR( *LARpp_j_1, 2L ), SASR( *LARpp_j, 2L ));
        *LARp = (INT16) GSM_SATADD( *LARp, SASR( *LARpp_j, 1L ));
    }
}


static void Coefficients_40_159 P2((LARpp_j, LARp),
                                   register INT16 * LARpp_j,
                                   register INT16 * LARp)
{
    register int i;

    for (i = 1; i <= 8; i++, LARp++, LARpp_j++)
        *LARp = *LARpp_j;
}

/* 4.2.9.2 */

static void LARp_to_rp P1((LARp),
                          register INT16 * LARp)  /* [0..7] IN/OUT  */
/*
 *  The input of this procedure is the interpolated LARp[0..7] array.
 *  The reflection coefficients, rp[i], are used in the analysis
 *  filter and in the synthesis filter.
 */
{
    register int            i;
    register INT16          temp;
    register INT32      ltmp;

    VPP_FR_PROFILE_FUNCTION_ENTER(LARp_to_rp);

    for (i = 1; i <= 8; i++, LARp++)
    {

        /* temp = GSM_ABS( *LARp );
         *
         * if (temp < 11059) temp <<= 1;
         * else if (temp < 20070) temp += 11059;
         * else temp = GSM_SATADD( temp >> 2, 26112 );
         *
         * *LARp = *LARp < 0 ? -temp : temp;
         */

        if (*LARp < 0)
        {
            temp = *LARp == MIN_WORD ? MAX_WORD : -(*LARp);
            *LARp = - ((temp < 11059) ? temp << 1
                       : ((temp < 20070) ? temp + 11059
                          :  (INT16) GSM_SATADD( temp >> 2, 26112L )));
        }
        else
        {
            temp  = *LARp;
            *LARp =    (temp < 11059) ? temp << 1
                       : ((temp < 20070) ? temp + 11059
                          :  (INT16) GSM_SATADD( temp >> 2, 26112L ));
        }
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(LARp_to_rp);
}


/* 4.2.10 */
static void Short_term_analysis_filtering P4((S,rp,k_n,s),
        struct gsm_state * S,
        register INT16  * rp,   /* [0..7]       IN      */
        register int    k_n,    /*   k_end - k_start    */
        register INT16  * s     /* [0..n-1]     IN/OUT  */
                                            )
/*
 *  This procedure computes the short term residual signal d[..] to be fed
 *  to the RPE-LTP loop from the s[..] signal and from the local rp[..]
 *  array (quantized reflection coefficients).  As the call of this
 *  procedure can be done in many ways (see the interpolation of the LAR
 *  coefficient), it is assumed that the computation begins with index
 *  k_start (for arrays d[..] and s[..]) and stops with index k_end
 *  (k_start and k_end are defined in 4.2.9.1).  This procedure also
 *  needs to keep the array u[0..7] in memory for each call.
 */
{
    register INT16          * u = S->u;
    //register int          i;
    register INT16          di, zzz, ui, sav, rpi;
    register INT32      ltmp;

    VPP_FR_PROFILE_FUNCTION_ENTER(Short_term_analysis_filtering);

    for (; k_n--; s++)
    {

        di = sav = *s;

        //for (i = 0; i < 8; i++) {             /* YYY */

        ui    = u[0];
        rpi   = rp[0];
        u[0]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[1];
        rpi   = rp[1];
        u[1]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[2];
        rpi   = rp[2];
        u[2]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[3];
        rpi   = rp[3];
        u[3]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[4];
        rpi   = rp[4];
        u[4]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[5];
        rpi   = rp[5];
        u[5]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[6];
        rpi   = rp[6];
        u[6]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        ui    = u[7];
        rpi   = rp[7];
        u[7]  = sav;

        zzz   = (INT16) GSM_MULT_R(rpi, di);
        sav   = (INT16) GSM_SATADD(   ui,   zzz);

        zzz   = (INT16) GSM_MULT_R(rpi, ui);
        di    = (INT16) GSM_SATADD(   di,   zzz );

        //}

        *s = di;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(Short_term_analysis_filtering);
}

#if defined(USE_FLOAT_MUL) && defined(FAST)

static void Fast_Short_term_analysis_filtering P4((S,rp,k_n,s),
        struct gsm_state * S,
        register INT16  * rp,   /* [0..7]       IN      */
        register int    k_n,    /*   k_end - k_start    */
        register INT16  * s     /* [0..n-1]     IN/OUT  */
                                                 )
{
    register INT16          * u = S->u;
    register int            i;

    float     uf[8],
              rpf[8];

    register float scalef = 3.0517578125e-5;
    register float          sav, di, temp;

    VPP_FR_PROFILE_FUNCTION_ENTER(Fast_Short_term_analysis);

    for (i = 0; i < 8; ++i)
    {
        uf[i]  = u[i];
        rpf[i] = rp[i] * scalef;
    }
    for (; k_n--; s++)
    {
        sav = di = *s;
        for (i = 0; i < 8; ++i)
        {
            register float rpfi = rpf[i];
            register float ufi  = uf[i];

            uf[i] = sav;
            temp  = rpfi * di + ufi;
            di   += rpfi * ufi;
            sav   = temp;
        }
        *s = di;
    }
    for (i = 0; i < 8; ++i) u[i] = uf[i];

    VPP_FR_PROFILE_FUNCTION_EXIT(Fast_Short_term_analysis);
}
#endif /* ! (defined (USE_FLOAT_MUL) && defined (FAST)) */

static void Short_term_synthesis_filtering P5((S,rrp,k,wt,sr),
        struct gsm_state * S,
        register INT16  * rrp,  /* [0..7]       IN      */
        register int    k,      /* k_end - k_start      */
        register INT16  * wt,   /* [0..k-1]     IN      */
        register INT16  * sr    /* [0..k-1]     OUT     */
                                             )
{
    register INT16          * v = S->v;
    register int            i;
    register INT16          sri, tmp1, tmp2;
    register INT32      ltmp;   /* for GSM_SATADD   & GSM_SUB */

    VPP_FR_PROFILE_FUNCTION_ENTER(Short_term_synthesis_filtering);

    while (k--)
    {
        sri = *wt++;
        for (i = 8; i--;)
        {

            /* sri = GSM_SUB( sri, gsm_mult_r( rrp[i], v[i] ) );
             */
            tmp1 = rrp[i];
            tmp2 = v[i];
            tmp2 =  ( tmp1 == MIN_WORD && tmp2 == MIN_WORD
                      ? MAX_WORD
                      : 0x0FFFF & ((INT16) (( (INT32)tmp1 * (INT32)tmp2
                                              + 16384) >> 15))) ;

            sri  = (INT16) GSM_SATSUB( sri, tmp2 );

            /* v[i+1] = GSM_SATADD( v[i], gsm_mult_r( rrp[i], sri ) );
             */
            tmp1  = ( tmp1 == MIN_WORD && sri == MIN_WORD
                      ? MAX_WORD
                      : 0x0FFFF & ((INT16) (( (INT32)tmp1 * (INT32)sri
                                              + 16384) >> 15))) ;

            v[i+1] = (INT16) GSM_SATADD( v[i], tmp1);
        }
        *sr++ = v[0] = sri;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(Short_term_synthesis_filtering);
}


#if defined(FAST) && defined(USE_FLOAT_MUL)

static void Fast_Short_term_synthesis_filtering P5((S,rrp,k,wt,sr),
        struct gsm_state * S,
        register INT16  * rrp,  /* [0..7]       IN      */
        register int    k,      /* k_end - k_start      */
        register INT16  * wt,   /* [0..k-1]     IN      */
        register INT16  * sr    /* [0..k-1]     OUT     */
                                                  )
{
    register INT16          * v = S->v;
    register int            i;

    float va[9], rrpa[8];
    register float scalef = 3.0517578125e-5, temp;

    VPP_FR_PROFILE_FUNCTION_ENTER(Fast_Short_term_synthesis_filtering);

    for (i = 0; i < 8; ++i)
    {
        va[i]   = v[i];
        rrpa[i] = (float)rrp[i] * scalef;
    }
    while (k--)
    {
        register float sri = *wt++;
        for (i = 8; i--;)
        {
            sri -= rrpa[i] * va[i];
            if     (sri < -32768.) sri = -32768.;
            else if (sri > 32767.) sri =  32767.;

            temp = va[i] + rrpa[i] * sri;
            if     (temp < -32768.) temp = -32768.;
            else if (temp > 32767.) temp =  32767.;
            va[i+1] = temp;
        }
        *sr++ = va[0] = sri;
    }
    for (i = 0; i < 9; ++i) v[i] = va[i];

    VPP_FR_PROFILE_FUNCTION_EXIT(Fast_Short_term_synthesis_filtering);
}

#endif /* defined(FAST) && defined(USE_FLOAT_MUL) */


extern INT16 txdtx_ctrl;
void Gsm_Short_Term_Analysis_Filter P3((S,LARc,s),

                                       struct gsm_state * S,

                                       INT16   * LARc,         /* coded log area ratio [0..7]  IN      */
                                       INT16   * s             /* signal [0..159]              IN/OUT  */
                                      )
{
    INT16           * LARpp_j       = S->LARpp[ S->j      ];
    INT16           * LARpp_j_1     = S->LARpp[ S->j ^= 1 ];

    INT16           LARp[8];

#undef  FILTER
#if     defined(FAST) && defined(USE_FLOAT_MUL)
#       define  FILTER  (* (S->fast                     \
                           ? Fast_Short_term_analysis_filtering \
                           : Short_term_analysis_filtering      ))

#else
#       define  FILTER  Short_term_analysis_filtering
#endif

    VPP_FR_PROFILE_FUNCTION_ENTER(Gsm_Short_Term_Analysis_Filter);

    Decoding_of_the_coded_Log_Area_Ratios( LARc, LARpp_j );

//  if ((txdtx_ctrl & TX_SP_FLAG) != 0)
//  {
    Coefficients_0_12(  LARpp_j_1, LARpp_j, LARp );
    LARp_to_rp( LARp );
    FILTER( S, LARp, 13L, s);

    Coefficients_13_26( LARpp_j_1, LARpp_j, LARp);
    LARp_to_rp( LARp );
    FILTER( S, LARp, 14L, s + 13);

    Coefficients_27_39( LARpp_j_1, LARpp_j, LARp);
    LARp_to_rp( LARp );
    FILTER( S, LARp, 13L, s + 27);

    Coefficients_40_159( LARpp_j, LARp);
    LARp_to_rp( LARp );
    FILTER( S, LARp, 120L, s + 40);
//  }
//  else
//  {
//      Coefficients_40_159( LARpp_j, LARp);
//      LARp_to_rp( LARp );
//      FILTER( S, LARp, 160L, s);
//  }
//
//
    VPP_FR_PROFILE_FUNCTION_EXIT(Gsm_Short_Term_Analysis_Filter);
}

void Gsm_Short_Term_Synthesis_Filter P4((S, LARcr, wt, s),
                                        struct gsm_state * S,

                                        INT16   * LARcr,        /* received log area ratios [0..7] IN  */
                                        INT16   * wt,           /* received d [0..159]             IN  */

                                        INT16   * s             /* signal   s [0..159]            OUT  */
                                       )
{
    INT16           * LARpp_j       = S->LARpp[ S->j     ];
    INT16           * LARpp_j_1     = S->LARpp[ S->j ^=1 ];

    INT16           LARp[8];

    VPP_FR_PROFILE_FUNCTION_ENTER(Gsm_Short_Term_Synthesis_Filter);

#undef  FILTER
#if     defined(FAST) && defined(USE_FLOAT_MUL)

#       define  FILTER  (* (S->fast                     \
                           ? Fast_Short_term_synthesis_filtering        \
                           : Short_term_synthesis_filtering     ))
#else
#       define  FILTER  Short_term_synthesis_filtering
#endif

    Decoding_of_the_coded_Log_Area_Ratios( LARcr, LARpp_j );

    if ((rxdtx_ctrl & TX_SP_FLAG) != 0)
    {
        Coefficients_0_12( LARpp_j_1, LARpp_j, LARp );
        LARp_to_rp( LARp );
        FILTER( S, LARp, 13L, wt, s );

        Coefficients_13_26( LARpp_j_1, LARpp_j, LARp);
        LARp_to_rp( LARp );
        FILTER( S, LARp, 14L, wt + 13, s + 13 );

        Coefficients_27_39( LARpp_j_1, LARpp_j, LARp);
        LARp_to_rp( LARp );
        FILTER( S, LARp, 13L, wt + 27, s + 27 );

        Coefficients_40_159( LARpp_j, LARp );
        LARp_to_rp( LARp );
        FILTER(S, LARp, 120L, wt + 40, s + 40);
    }
    else
    {
        Coefficients_40_159( LARpp_j, LARp );
        LARp_to_rp( LARp );
        FILTER(S, LARp, 160L, wt, s );

    }

    VPP_FR_PROFILE_FUNCTION_EXIT(Gsm_Short_Term_Synthesis_Filter);


}

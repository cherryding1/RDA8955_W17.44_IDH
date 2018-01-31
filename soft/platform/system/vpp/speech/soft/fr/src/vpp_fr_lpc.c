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
#include "vpp_fr_vad.h"
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

#undef  P

//INT16 scal_acf,rc_VAD[8];
INT16 scalvad;
extern INT16 LARc_old_tx[8],xmaxc_old_tx;

/*
 *  4.2.4 .. 4.2.7 LPC ANALYSIS SECTION
 */

/* 4.2.4 */

extern char dtx_mode;
//extern FILE *pfileVAD;


extern  int frame;

static void Autocorrelation P2((s, L_ACF),
                               INT16    * s,           /* [0..159]     IN/OUT  */
                               INT32 * L_ACF)      /* [0..8]       OUT     */
/*
 *  The goal is to compute the array L_ACF[k].  The signal s[i] must
 *  be scaled in order to avoid an overflow situation.
 */
{
    register int    k, i;



    INT16           temp, smax, scalauto;
//      INT32       L_temp;

#ifdef  USE_FLOAT_MUL
    float           float_s[160];
//      INT32       L_temp2;
#else
//      INT32       L_temp2;
#endif

    /*  Dynamic scaling of the array  s[0..159]
     */

    /*  Search for the maximum.
     */
    //  scal_acf = 0;

    VPP_FR_PROFILE_FUNCTION_ENTER(autocorrelation);

    scalvad = 0;
    smax = 0;
    for (k = 0; k <= 159; k++)
    {
        temp = GSM_ABS( s[k] );
        if (temp > smax) smax = temp;
    }

    /*  Computation of the scaling factor.
     */
    if (smax == 0) scalauto = 0;
    else
    {
        //assert(smax > 0);
        scalauto = 4 - gsm_norm( (INT32)smax << 16 );/* sub(4,..) */
    }


    /*  Scaling of the array s[0...159]
     */

    if (scalauto > 0)
    {

# ifdef USE_FLOAT_MUL
#   define SCALE(n)     \
        case n: for (k = 0; k <= 159; k++) \
                        float_s[k] = (float)    \
                                (s[k] = GSM_MULT_R(s[k], 16384 >> (n-1)));\
                break;
# else
#   define SCALE(n)     \
        case n: for (k = 0; k <= 159; k++) \
                        s[k] = GSM_MULT_R( s[k], (16384 >> (n-1) );\
                break;
# endif /* USE_FLOAT_MUL */

        /*switch (scalauto) {
        SCALE(1)
        SCALE(2)
        SCALE(3)
        SCALE(4)
        }*/
        for (k = 0; k <= 159; k++)
        {
            s[k] = ((s[k] <<(15-scalauto ))+0x4000)>>15;
            // s[k] = GSM_MULT_R( s[k], (UINT16) 0x8000 >> scalauto );
        }

# undef SCALE
    }
# ifdef USE_FLOAT_MUL
    else for (k = 0; k <= 159; k++) float_s[k] = (float) s[k];
# endif

    /*  Compute the L_ACF[..].
     */
    {
# ifdef USE_FLOAT_MUL
        register float * sp = float_s;
        register float   sl = *sp;
# else
        INT16  * sp = s;
        INT16   sl = *sp;

# endif

#       define STEP(k)   L_ACF[k] += (INT32)((INT32) sl * sp[ -(k) ]);
#       define NEXTI     sl = *++sp


        for (k = 9; k--; L_ACF[k] = 0) ;



        STEP(0);
        NEXTI;
        STEP(0); STEP(1);
        NEXTI;
        STEP(0); STEP(1); STEP(2);
        NEXTI;
        STEP(0); STEP(1); STEP(2); STEP(3);
        NEXTI;
        STEP(0); STEP(1); STEP(2); STEP(3); STEP(4);
        NEXTI;
        STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5);
        NEXTI;
        STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5); STEP(6);
        NEXTI;
        STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5); STEP(6); STEP(7);



        for (i = 8; i <= 159; i++)
        {

            NEXTI;
            STEP(0);
            STEP(1); STEP(2); STEP(3); STEP(4);
            STEP(5); STEP(6); STEP(7); STEP(8);

        }




        for (k = 9; k--; L_ACF[k] <<= 1) ;

    }
    /*   Rescaling of the array s[0..159]
     */

    if (scalauto > 0)
    {
        //assert(scalauto <= 4);
//                scal_acf = (scalauto+1)*2 -1;//fts add
//                scal_acf = scalauto; //fts
        scalvad = scalauto;
        for (k = 160; k--; *s++ <<= scalauto) ;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(autocorrelation);
}

#if defined(USE_FLOAT_MUL) && defined(FAST)

static void Fast_Autocorrelation P2((s, L_ACF),
                                    INT16 * s,              /* [0..159]     IN/OUT  */
                                    INT32 * L_ACF)      /* [0..8]       OUT     */
{
    register int    k, i;
    float f_L_ACF[9];
    float scale;

    float          s_f[160];
    register float *sf = s_f;

    VPP_FR_PROFILE_FUNCTION_ENTER(fast_autocorrelation);

    for (i = 0; i < 160; ++i) sf[i] = s[i];
    for (k = 0; k <= 8; k++)
    {
        register float L_temp2 = 0;
        register float *sfl = sf - k;
        for (i = k; i < 160; ++i) L_temp2 += sf[i] * sfl[i];
        f_L_ACF[k] = L_temp2;
    }
    scale = MAX_LONGWORD / f_L_ACF[0];

    for (k = 0; k <= 8; k++)
    {
        L_ACF[k] = f_L_ACF[k] * scale;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(fast_autocorrelation);
}
#endif  /* defined (USE_FLOAT_MUL) && defined (FAST) */

/* 4.2.5 */

static void Reflection_coefficients P2( (L_ACF, r),
                                        INT32       * L_ACF,                /* 0...8        IN      */
                                        register INT16  * r                     /* 0...7        OUT     */
                                      )
{
    register int    i, m, n;
    register INT16  temp;
    register INT32 ltmp;
    INT16           ACF[9]; /* 0..8 */
    INT16           P[  9]; /* 0..8 */
    INT16           K[  9]; /* 2..8 */

    /*  Schur recursion with 16 bits arithmetic.
     */

    VPP_FR_PROFILE_FUNCTION_ENTER(reflection_coefficients);

    if (L_ACF[0] == 0)      /* everything is the same. */
    {
        for (i = 8; i--; *r++ = 0) ;
        VPP_FR_PROFILE_FUNCTION_EXIT(reflection_coefficients);
        return;
    }

    //assert( L_ACF[0] != 0 );
    temp = gsm_norm( L_ACF[0] );

    //assert(temp >= 0 && temp < 32);

    /* ? overflow ? */
    for (i = 0; i <= 8; i++) ACF[i] = ((INT16) (SASR( L_ACF[i] << temp, 16 )));

    /*   Initialize array P[..] and K[..] for the recursion.
     */

    for (i = 1; i <= 7; i++) K[ i ] = ACF[ i ];
    for (i = 0; i <= 8; i++) P[ i ] = ACF[ i ];

    /*   Compute reflection coefficients
     */
    for (n = 1; n <= 8; n++, r++)
    {

        temp = P[1];
        temp = GSM_ABS(temp);
        if (P[0] < temp)
        {
            for (i = n; i <= 8; i++) *r++ = 0;
            VPP_FR_PROFILE_FUNCTION_EXIT(reflection_coefficients);
            return;
        }

        *r = gsm_div( temp, P[0] );

        //assert(*r >= 0);
        if (P[1] > 0) *r = -*r;         /* r[n] = sub(0, r[n]) */
        //assert (*r != MIN_WORD);
        if (n == 8)
        {
            VPP_FR_PROFILE_FUNCTION_EXIT(reflection_coefficients);
            return;
        }

        /*  Schur recursion
         */
        temp = GSM_MULT_R( P[1], *r );
        P[0] = (INT16) GSM_SATADD( P[0], temp );

        for (m = 1; m <= 8 - n; m++)
        {
            temp     = GSM_MULT_R( K[ m   ],    *r );
            P[m]     = (INT16) GSM_SATADD(    P[ m+1 ],  temp );

            temp     = GSM_MULT_R( P[ m+1 ],    *r );
            K[m]     = (INT16) GSM_SATADD(    K[ m   ],  temp );
        }
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(reflection_coefficients);
}

/* 4.2.6 */

static void Transformation_to_Log_Area_Ratios P1((r),
        register INT16  * r                     /* 0..7    IN/OUT */
                                                )
/*
 *  The following scaling for r[..] and LAR[..] has been used:
 *
 *  r[..]   = integer( real_r[..]*32768. ); -1 <= real_r < 1.
 *  LAR[..] = integer( real_LAR[..] * 16384 );
 *  with -1.625 <= real_LAR <= 1.625
 */
{
    register INT16  temp;
    register int    i;

    VPP_FR_PROFILE_FUNCTION_ENTER(transformation_to_log_area_ratios);

    /* Computation of the LAR[0..7] from the r[0..7]
     */
    for (i = 1; i <= 8; i++, r++)
    {

        temp = *r;
        temp = GSM_ABS(temp);
        //assert(temp >= 0);

        if (temp < 22118)
        {
            temp >>= 1;
        }
        else if (temp < 31130)
        {
            //assert( temp >= 11059 );
            temp -= 11059;
        }
        else
        {
            //assert( temp >= 26112 );
            temp -= 26112;
            temp <<= 2;
        }

        *r = *r < 0 ? -temp : temp;
        //assert( *r != MIN_WORD );
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(transformation_to_log_area_ratios);
}

/* 4.2.7 */

static void Quantization_and_coding P1((LAR),
                                       register INT16 * LAR    /* [0..7]       IN/OUT  */
                                      )
{
    register INT16  temp;
    INT32       ltmp;

    VPP_FR_PROFILE_FUNCTION_ENTER(quantization_and_coding);

//      UINT32      utmp;


    /*  This procedure needs four tables; the following equations
     *  give the optimum scaling for the constants:
     *
     *  A[0..7] = integer( real_A[0..7] * 1024 )
     *  B[0..7] = integer( real_B[0..7] *  512 )
     *  MAC[0..7] = maximum of the LARc[0..7]
     *  MIC[0..7] = minimum of the LARc[0..7]
     */

#       undef STEP
#       define  STEP( A, B, MAC, MIC )          \
                temp = (INT16) GSM_MULT( A,   *LAR );   \
                temp = (INT16) GSM_SATADD(  temp,   B );   \
                temp = (INT16) GSM_SATADD(  temp, 256 );   \
                temp = SASR(     temp,   9 );   \
                *LAR  =  temp>MAC ? MAC - MIC : (temp<MIC ? 0 : temp - MIC); \
                LAR++;

    STEP(  20480,     0,  31, -32 );
    STEP(  20480,     0,  31, -32 );
    STEP(  20480,  2048,  15, -16 );
    STEP(  20480, -2560,  15, -16 );

    STEP(  13964,    94,   7,  -8 );
    STEP(  15360, -1792,   7,  -8 );
    STEP(   8534,  -341,   3,  -4 );
    STEP(   9036, -1144,   3,  -4 );

#       undef   STEP

    VPP_FR_PROFILE_FUNCTION_EXIT(quantization_and_coding);
}

INT16 VAD_flag;
extern INT16 txdtx_ctrl;
extern INT16 sof[160];
//extern INT16 ptch;
extern INT16 LAR_SID_tx[DTX_HANGOVER][8];
extern INT16 xmax_SID_tx[DTX_HANGOVER][4];

void Gsm_LPC_Analysis P3((S, s,LARc),
                         struct gsm_state *S,
                         INT16            * s,           /* 0..159 signals       IN/OUT  */
                         INT16             * LARc)        /* 0..7   LARc's        OUT     */
{
    //add fts
    int i,j;
    long L_Temp,L_Temp1;
    INT16 * LARc_temp ;
    INT16 LARc_aver[8];
//  INT16 r_h[9],r_l[9];
    //add end

    //long E,ACF[9]; //fts add
    INT32       L_ACF[9];

    VPP_FR_PROFILE_FUNCTION_ENTER(GSM_LPC_Analysis);

#if defined(USE_FLOAT_MUL) && defined(FAST)
    if (S->fast) Fast_Autocorrelation (s,     L_ACF );
    else
#endif
        Autocorrelation                   (s,     L_ACF );

    /*  printf("\n9 autofunction value:\n");
        for(i=0;i<9;i++)
        {
            printf("  %d ",L_ACF[i]);
            ACF[i]=L_ACF[i];
        }
    */



    if (dtx_mode==1)
    {
        /* DTX enabled, make voice activity decision */

        VAD_flag = vad_computation (L_ACF);//, ptch);


        //  if(VAD_flag == 0)
        //      VAD_flag = 0;


        //printf("\n scalvad = %d , vad= %d ", scalvad, VAD_flag);
//      printf("\n vad= %d ", VAD_flag);

        //  fprintf(pfileVAD, "%d\n", VAD_flag);

        //   fprintf(pfileVAD, "frame   %d # ,               %d          \n",      frame,VAD_flag);


        tx_dtx (VAD_flag, &txdtx_ctrl); /* TX DTX handler */
    }
    else
    {
        /* DTX disabled, active speech in every frame */
        VAD_flag = 1;
        txdtx_ctrl = TX_VAD_FLAG | TX_SP_FLAG;
    }

    Reflection_coefficients           (L_ACF, LARc  );
    /*  printf("\n 8 K coefficient:\n");
        for(i=0;i<8;i++)
        printf("LARc[%d] = 0x%x ",i,LARc[i]);
        printf("\n");

        E=LARc[0];

        for(i=0;i<8;i++)
           E  = E - (ACF[i]*LARc[i]);
        printf("E = %d \n",E);
    */

    Transformation_to_Log_Area_Ratios (LARc);

    if (((txdtx_ctrl & TX_HANGOVER_ACTIVE) != 0)||((txdtx_ctrl & TX_SID_UPDATE) != 0))
    {

        for (i = DTX_HANGOVER - 1; i > 0; i--)
        {
            for (j = 0; j < 8; j++)
            {
                LAR_SID_tx[i][j] = LAR_SID_tx[i - 1][j];
            }
        }

        LARc_temp = LARc;
        for (j = 0; j < 8; j++)
        {
            LAR_SID_tx[0][j] = *LARc_temp++;
        }
    }


    if ((txdtx_ctrl & TX_SID_UPDATE) != 0)
    {

        /*  //shift and  restore
            for (i = DTX_HANGOVER - 1; i > 0; i--)
            {
                for (j = 0; j < 8; j++)
                {
                    LAR_SID_tx[i][j] = LAR_SID_tx[i - 1][j];
                }
            }

            //store nowly parm
            LARc_temp = LARc;
            for (j = 0; j < 8; j++)
            {
                LAR_SID_tx[0][j] = *LARc_temp++;
            }
        */

        //avering
        for(i=0; i<8; i++)
        {
            register UINT32 utmp;       /* for L_ADD */
            L_Temp = 2;
            for(j=0; j<DTX_HANGOVER; j++)
            {
                L_Temp1 = (long)LAR_SID_tx[j][i];
                L_Temp = GSM_L_ADD(L_Temp,L_Temp1);
            }
            L_Temp = L_Temp >> 2;
            LARc_aver[i] = (INT16)(L_Temp);
        }

        //SID frame use aver value
        LARc_temp = LARc;
        for (j = 0; j < 8; j++)
        {
            *LARc_temp++ = LARc_aver[j];
        }

    }


    Quantization_and_coding           (LARc);

    VPP_FR_PROFILE_FUNCTION_EXIT(GSM_LPC_Analysis);

    /*  printf("\nQuantization \n");
        for(i=0;i<8;i++)
        printf(" LARc[%d] = %d ", i,LARc[i]);
    */

}

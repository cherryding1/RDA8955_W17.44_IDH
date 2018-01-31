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



#include "vpp_fr_private.h"
#include "vpp_fr_vad.h"
//#include "basic_opera.h"
//
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

#include <stdio.h>

//INT16 ptch;

#define EFR_PARMETER 0  //set 0 as fr parm value

/* Constants of VAD hangover addition */


#define HANGCONST 5
#define BURSTCONST 3

/* Constant of spectral comparison */


#define STAT_THRESH 3277L       /* 0.05  Q16*/


/* Constants of periodicity detection */

#define LTHRESH 2
#define NTHRESH 4

/* Pseudo floating point representations of constants
for threshold adaptation */

#define M_PTH    18750          /*** 300000.0 ***/
#define E_PTH    19
#define M_PLEV   25000          /*** 800 000.0 ***/
#define E_PLEV   20
#define M_MARGIN 19531          /*** 80 000 000.0 ***/
#define E_MARGIN 27


#define FAC 24576               /* 3.0 */

/* Constants of tone detection */

#define FREQTH 3189

#if EFR_PARMETER
#define PREDTH 1464
#else
#define PREDTH 1464//518
#endif

static INT16 rvad[9], normrvad;

static Pfloat thvad;
static INT32 L_sacf[27];
static INT32 L_sav0[36];
static INT16  pt_sacf, pt_sav0;
static INT32 L_lastdm;
static INT16  adaptcount;
static INT16  burstcount, hangcount;
static INT16  oldlagcount, veryoldlagcount, oldlag;
INT16  tone;

//INT16 normrvad;
extern INT16 scalvad;

//INT16 veryoldlagcount,oldlagcount;
void vad_init(void)
{

    INT16  i;

    /* Initialize rvad variables */
    rvad[0] = 24576;//0x6000;
    rvad[1] = -16384;//(short)0xc000; //-4
    rvad[2] = 4096;//0x1000; //1
    for (i = 3; i < 9; i++)
    {
        rvad[i] = 0;
    }

    normrvad = 7;


    /* Initialize ACF averaging variables */
    for (i = 0; i < 27; i++)
    {
        L_sacf[i] = 0L;
    }
    for (i = 0; i < 36; i++)
    {
        L_sav0[i] = 0L;
    }
    pt_sacf = 0;
    pt_sav0 = 0;

    /* Initialize spectral comparison variable */
    L_lastdm = 0L;

    /* Initialize periodicity detection variables */
    oldlagcount = 0;
    veryoldlagcount = 0;

    /* Initialize threshold level */
    thvad.e = 20;               /*** exponent ***///1 000 000
    thvad.m = 31250;            /*** mantissa ***/

    /* Initialize threshold adaptation variable */
    adaptcount = 0;

    /* Initialize VAD hangover addition variables */
    burstcount = 0;
    hangcount = -1;

    oldlag = 40;

    tone  = 0;

    //    ptch = 1;

    return;
}

/****************************************************************************
*
*     FUNCTION:  vad_computation
*
*     PURPOSE:   Returns a decision as to whether the current frame being
*                processed by the speech encoder contains speech or not.
*
*     INPUTS:    L_ACF[0..8]     autocorrelation of input signal frame
*                sof[0..159]     offset compensated signal frame
*                ptch            flag to indicate a periodic signal component
*
*     OUTPUTS:   none
*
*     RETURN VALUE: vad decision
*
***************************************************************************/

INT16 vad_computation (INT32 * L_ACF)//, INT16 ptch)
{
    INT32 L_av0[9], L_av1[9];
    INT16 vad, vvad, rav1[9], normrav1, stat, ptch;//, tone;
    Pfloat acf0, pvad;

    VPP_FR_PROFILE_FUNCTION_ENTER(vad_computation);

    energy_computation (L_ACF, rvad, &acf0, &pvad);

    //  printf(" acf0 = %d ,pvad = %d ",acf0,pvad);

    acf_averaging (L_ACF, L_av0, L_av1);

    predictor_values (L_av1, rav1, &normrav1);

    stat = spectral_comparison (rav1, normrav1, L_av0);

    //  printf("\n stat = %d ", stat);

    //    tone_detection (sof, &tone);
    //  printf(" tone = %d ", tone);

    ptch = Periodicity_detection(oldlagcount, veryoldlagcount);

    //  printf("\n ptch = %d ", ptch);

    threshold_adaptation (stat, ptch, tone, rav1, normrav1, pvad, acf0,
                          rvad, &normrvad, &thvad);
    //  printf(" thvad = %d ",thvad);

    vvad = vad_decision (pvad, thvad);
    //  printf(" vvad = %d ",vvad);

    vad = vad_hangover (vvad);


//  printf("\t%d",vad);



    /*  for(i=0;i<9;i++)
        {
            printf("\n\tL_av1[i]=%d",L_av1[i]);
        }
    */

    VPP_FR_PROFILE_FUNCTION_EXIT(vad_computation);
    return vad;
}


/****************************************************************************
*
*     FUNCTION:  energy_computation
*
*     PURPOSE:   Computes the input and residual energies of the adaptive
*                filter in a floating point representation.
*
*     INPUTS:    L_ACF[0..8]      autocorrelation of input signal frame
*                rvad[0..8]     autocorrelated adaptive filter coefficients
*
*     OUTPUTS:   *acf0          signal frame energy (mantissa+exponent)
*                *pvad          filtered signal energy (mantissa+exponent)
*
*     RETURN VALUE: none
*
***************************************************************************/


//energy_computation (L_ACF, scalvad, rvad, scal_rvad, &acf0, &pvad);


void energy_computation (
    INT32 L_ACF[],
    //    INT16 scalvad,
    INT16 r_vad[],
    Pfloat * acf0,
    Pfloat * pvad
)

{
    INT16 i, normprod, normacf, sacf[9];
    INT32 L_temp;

    register UINT32 utmp;       /* for L_ADD */

    VPP_FR_PROFILE_FUNCTION_ENTER(energy_computation);

    if (L_ACF[0] == 0L )
    {
        pvad->e = -32768;
        pvad->m = 0;
        acf0->e = -32768;
        acf0->m = 0;
        VPP_FR_PROFILE_FUNCTION_EXIT(energy_computation);
        return;
    }

    /*Re_normalization of the L_ACF[0..8]:*/

    normacf = gsm_norm(L_ACF[0]);

    for (i = 0; i < 9; i++)
        sacf[i] = (INT16) (gsm_L_asl(L_ACF[i], normacf) >> 19);

    /* Computation of acf0 (exponent and mantissa) */

    acf0->e = GSM_ADD (32, (INT16)(scalvad << 1));
    //acf0->e = gsm_sub (acf0->e, normacf);
    acf0->e = GSM_SUB(acf0->e, normacf);
    acf0->m = sacf[0] << 3;

    /* Computation of pvad (exponent and mantissa) */

    pvad->e = GSM_ADD (acf0->e, 14);
    //pvad->e = gsm_sub (pvad->e, normrvad);
    pvad->e = GSM_SUB(pvad->e, normrvad);

    L_temp = 0L;

    for (i = 1; i <= 8; i++)
    {


        L_temp = GSM_L_ADD( L_temp, GSM_L_MULT( sacf[i], r_vad[i] ) );
    }

    L_temp = GSM_L_ADD( L_temp, GSM_L_MULT( sacf[0], r_vad[0] ) >> 1 );

    if (L_temp <= 0L)
    {
        L_temp = 1L;
    }

    normprod = gsm_norm (L_temp);

    //pvad->e = gsm_sub (pvad->e, normprod);
    pvad->e = GSM_SUB (pvad->e, normprod);

    pvad->m = (INT16)(gsm_L_asl(L_temp, normprod) >> 16 );

//  printf("#");

    VPP_FR_PROFILE_FUNCTION_EXIT(energy_computation);

    return;
}

void acf_averaging (
    //    INT16  r_h[],
    //    INT16  r_l[],
    //    INT16  scal_acf,
    INT32 L_ACF[],
    INT32 L_av0[],
    INT32 L_av1[]
)
{
    INT32 L_temp;
    INT16  scale;
    INT16  i;

    VPP_FR_PROFILE_FUNCTION_ENTER(acf_averaging);

    //scale = gsm_sub (10, (INT16)(scalvad << 1));
    scale = GSM_SUB (10, (INT16)(scalvad << 1));

    /*Computation of the arrays L_av0[0..8] and L_av1[0..8]:*/

    for(i = 0; i<9; i++)
    {

        register UINT32 utmp;       /* for L_ADD */

        L_temp = L_ACF[i] >> scale;

        L_av0[i] = GSM_L_ADD( L_sacf[i], L_temp );

        L_av0[i] = GSM_L_ADD( L_sacf[i+9], L_av0[i] );

        L_av0[i] = GSM_L_ADD( L_sacf[i+18], L_av0[i] );

        L_sacf[ pt_sacf + i ] = L_temp;

        L_av1[i] = L_sav0[ pt_sav0 + i ];

        L_sav0[ pt_sav0 + i] = L_av0[i];

    }

    /* Update the array pointers */

    //if (gsm_sub (pt_sacf, 18) == 0)
    if (pt_sacf == 18)
    {
        pt_sacf = 0;
    }
    else
    {
        pt_sacf = GSM_ADD (pt_sacf, 9);
    }


    //if (gsm_sub (pt_sav0, 27) == 0)
    if (pt_sav0 == 27)
    {
        pt_sav0 = 0;
    }
    else
    {
        pt_sav0 = GSM_ADD (pt_sav0, 9);
    }


    VPP_FR_PROFILE_FUNCTION_EXIT(acf_averaging);

    return;
}


void predictor_values (
    INT32 L_av1[],
    INT16 rav1[],
    INT16 *normrav1
)
{
    //INT16 vpar[8], aav1[9];
    INT16 vpar[9], aav1[9];

    VPP_FR_PROFILE_FUNCTION_ENTER(predictor_values);

    schur_recursion (L_av1, vpar);
    step_up (8, vpar, aav1);
    compute_rav1 (aav1, rav1, normrav1);

    VPP_FR_PROFILE_FUNCTION_EXIT(predictor_values);

    return;
}

void schur_recursion (
    INT32 L_av1[],
    INT16 vpar[]
)
{
    INT16 sacf[9], pp[9], kk[9], temp;
    INT16 i, k, m, n;

    VPP_FR_PROFILE_FUNCTION_ENTER(schur_recursion);

    /*** Schur recursion with 16-bit arithmetic ***/

    if (L_av1[0] == 0)
    {
        //        for (i = 0; i < 8; i++)
        for (i = 1; i < 9; i++)
        {
            vpar[i] = 0;
        }
        VPP_FR_PROFILE_FUNCTION_EXIT(schur_recursion);
        return;
    }

    temp = gsm_norm (L_av1[0]);

    for (k = 0; k <= 8; k++)
    {
        //acf[k] = extract_h (gsm_L_asl (L_av1[k], temp));
        sacf[k] = (INT16)(gsm_L_asl (L_av1[k], temp) >> 16);
    }

    /*** Initialize arrays pp[..] and kk[..] for the recursion: ***/

    for (i = 1; i <= 7; i++)
    {
        kk[9 - i] = sacf[i];
    }

    for (i = 0; i <= 8; i++)
    {
        pp[i] = sacf[i];
    }

    /*** Compute Parcor coefficients: ***/

    //    for (n = 0; n < 8; n++)
    for (n = 1; n <= 8; n++)
    {

        // if ((pp[0] == 0) ||
        //     (gsm_sub (pp[0], GSM_ABS (pp[1])) < 0))
        if ((pp[0] == 0) || (pp[0] < GSM_ABS (pp[1])))

            //      if (gsm_sub (pp[0], GSM_ABS (pp[1])) < 0)
        {
            //for (i = n; i < 8; i++)
            for (i = n; i <= 8; i++)
            {
                vpar[i] = 0;
            }
            VPP_FR_PROFILE_FUNCTION_EXIT(schur_recursion);
            return;
        }
        vpar[n] = gsm_div (GSM_ABS (pp[1]), pp[0]);

        if (pp[1] > 0)
        {
            //vpar[n] = gsm_sub( 0, vpar[n] ); //negate (vpar[n]);
            vpar[n] = GSM_SUB( 0, vpar[n] );
        }

        //if (gsm_sub (n, 7) == 0)
        //if (gsm_sub (n, 8) == 0)
        if (n == 8)
        {
            VPP_FR_PROFILE_FUNCTION_EXIT(schur_recursion);
            return;
        }

        /*** Schur recursion: ***/

        pp[0] = GSM_ADD (pp[0], GSM_SAT_MULT_R(pp[1], vpar[n]));

        //for (m = 1; m <= 7 - n; m++)
        for (m = 1; m <= 8 - n; m++)
        {
            pp[m] = GSM_ADD (pp[1 + m], GSM_SAT_MULT_R (kk[9 - m], vpar[n]));

            kk[9 - m] = GSM_ADD (kk[9 - m], GSM_SAT_MULT_R (pp[1 + m], vpar[n]));

        }
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(schur_recursion);

    return;
}


void step_up (
    INT16 np,
    INT16 vpar[],
    INT16 aav1[]
)
{
    INT32 L_coef[9];
    INT16 temp;
    INT16 i, m;

    // register INT32 hi=0;
    //register UINT32 lo=0;

    VPP_FR_PROFILE_FUNCTION_ENTER(step_up);

    /*** Initialization of the step-up recursion ***/

    //    L_coef[0] = 0x20000000L;
    //    L_coef[1] = gsm_L_asl (L_deposit_l (vpar[0]), 14);
    L_coef[0] = 16384 << 15;
    L_coef[1] = vpar[1] << 14;

    /*** Loop on the LPC analysis order: ***/

    for (m = 2; m <= np; m++)
    {
        register UINT32 utmp;       /* for L_ADD */

        for (i = 1; i < m; i++)
        {
            //            temp = extract_h (L_coef[m - i]);
            //            L_work[i] = L_mac (L_coef[i], vpar[m - 1], temp);
            temp = (INT16)(L_coef[m - i] >> 16);
            L_coef[i] = GSM_L_ADD (L_coef[i], GSM_L_MULT(vpar[m], temp));
        }

        /*for (i = 1; i < m; i++)
        {
            L_coef[i] = L_work[i];
        }*/

        //L_coef[m] = gsm_L_asl (L_deposit_l (vpar[m - 1]), 14);
        L_coef[m] = (INT32)vpar[m] << 14;
    }

    /*** Keep the aav1[0..np] in 13 (15) bits ***/

    for (i = 0; i <= np; i++)
    {
        //aav1[i] = extract_h (gsm_L_asr (L_coef[i], 3));
        aav1[i] =(INT16)(L_coef[i] >> 19);
    }


    VPP_FR_PROFILE_FUNCTION_EXIT(step_up);
    return;
}

void compute_rav1 (
    INT16 aav1[],
    INT16 rav1[],
    INT16 *normrav1
)
{
    INT32 L_work[9];
    INT16 i, k;
    register INT32 hi=0;
    register UINT32 lo=0;

    VPP_FR_PROFILE_FUNCTION_ENTER(compute_rav1);

    /*** Computation of the rav1[0..8] ***/

    for (i = 0; i <= 8; i++)
    {
        //L_work[i] = 0L;
        lo =0;

        for (k = 0; k <= 8 - i; k++)
        {
            //L_work[i] = L_mac (L_work[i], aav1[k], aav1[k + i]);
            //L_work[i] = GSM_L_ADD( L_work[i], GSM_L_MULT( aav1[k], aav1[k+i] ) );
            GSM_MLA16( hi, lo, aav1[k], aav1[k+i]);
        }
        L_work[i] = lo;

    }


    if (L_work[0] == 0L)
    {
        *normrav1 = 0;
    }
    else
    {
        *normrav1 = gsm_norm (L_work[0]);
    }

    for (i = 0; i <= 8; i++)
    {
        //rav1[i] = extract_h (gsm_L_asl (L_work[i], *normrav1));
        rav1[i] = (INT16)(gsm_L_asl (L_work[i], *normrav1)  >> 16);
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(compute_rav1);

    return;
}

INT16 spectral_comparison (
    INT16 rav1[],
    INT16 normrav1,
    INT32 L_av0[]
)
{
    INT32 L_dm, L_sump, L_temp;
    INT16 stat, sav0[9], shift, divshift, temp;
    INT16 i;

    register UINT32 utmp;       /* for L_ADD */

    VPP_FR_PROFILE_FUNCTION_ENTER(spectral_comparison);

    /*** Re-normalize L_av0[0..8] ***/

    if (L_av0[0] == 0L)
    {
        for (i = 0; i <= 8; i++)
        {
            sav0[i] = 0x0fff;   /* 4095 */
        }
    }
    else
    {
        //shift = gsm_sub  (gsm_norm (L_av0[0]), 3);//sub 3 in advance
        shift = GSM_SUB (gsm_norm (L_av0[0]), 3);//
        for (i = 0; i <= 8; i++)
        {
            sav0[i] = (INT16) (gsm_L_asl (L_av0[i], shift ) >> 16);
        }
    }

    /*** Compute partial sum of dm ***/

    L_sump = 0L;
    for (i = 1; i <= 8; i++)
    {

        //        L_sump = L_mac (L_sump, rav1[i], sav0[i]);
        L_sump = GSM_L_ADD( L_sump, GSM_L_MULT( rav1[i], sav0[i] ) );
    }

    /*** Compute the division of the partial sum by sav0[0] ***/

    if (L_sump < 0L)
    {
        //L_temp = L_negate (L_sump);
        L_temp = gsm_L_sub(0,L_sump);
    }
    else
    {
        L_temp = L_sump;
    }


    if (L_temp == 0L)
    {
        L_dm = 0L;
        shift = 0;
    }
    else
    {

        sav0[0] = gsm_asl (sav0[0], 3);
        shift = gsm_norm (L_temp);
        temp = (INT16) (gsm_L_asl (L_temp, shift) >> 16);

        //if (gsm_sub  (sav0[0], temp) >= 0)
        if (sav0[0] >= temp)
        {
            divshift = 0;
            temp = gsm_div (temp, sav0[0]);
        }
        else
        {
            divshift = 1;
            //temp = gsm_sub  (temp, sav0[0]);
            temp = GSM_SUB  (temp, sav0[0]);
            temp = gsm_div (temp, sav0[0]);
        }


        //if (gsm_sub  (divshift, 1) == 0)
        if (divshift == 1)
        {
            L_dm = 0x8000L;
        }
        else
        {
            L_dm = 0L;
        }

        L_dm = gsm_L_asl (GSM_L_ADD (L_dm, (INT32)(temp)), 1);


        if (L_sump < 0L)
        {
            //L_dm = L_negate (L_dm);
            L_dm = gsm_L_sub (0, L_dm);
        }
    }

    /*** Re-normalization and final computation of L_dm ***/

    L_dm = gsm_L_asl (L_dm, 14);
    L_dm = gsm_L_asr (L_dm, shift);
    L_dm = GSM_L_ADD (L_dm, gsm_L_asl ((INT32)(rav1[0]), 11));
    L_dm = gsm_L_asr (L_dm, normrav1);

    /*** Compute the difference and save L_dm ***/

    L_temp = gsm_L_sub (L_dm, L_lastdm);
    L_lastdm = L_dm;


    if (L_temp < 0L)
    {
        //L_temp = L_negate (L_temp);
        L_temp = gsm_L_sub(0, L_temp);
    }
    /*** Evaluation of the stat flag ***/

    L_temp = gsm_L_sub (L_temp, STAT_THRESH);


    if (L_temp < 0L)
    {
        stat = 1;
    }
    else
    {
        stat = 0;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(spectral_comparison);
    return stat;
}

static const INT16 hann[80] =
{
    0,   12,   51,  114,   204,  318,  458,  622,   811, 1025,
    1262, 1523, 1807, 2114,  2444, 2795, 3167, 3560,  3972, 4405,
    4856, 5325, 5811, 6314,  6832, 7365, 7913, 8473,  9046, 9631,
    10226,10831,11444,12065, 12693,13326,13964,14607, 15251,15898,
    16545,17192,17838,18482, 19122,19758,20389,21014, 21631,22240,
    22840,23430,24009,24575, 25130,25670,26196,26707, 27201,27679,
    28139,28581,29003,29406, 29789,30151,30491,30809, 31105,31377,
    31626,31852,32053,32230, 32382,32509,32611,32688, 32739,32764
};


void tone_detection (
    //    INT16 rc[],
    INT16 sig_of[],
    INT16 *sig_tone
)
{


    INT32 L_num, L_den, L_temp;
    INT16 temp, prederr, a[3];
    INT16 i;
    INT16 sofh[160], smax,scalauto,  rc[5], sacf[5], K[5], P[5];
    INT16 n, m, k;
    INT32 L_acfh[5];

    VPP_FR_PROFILE_FUNCTION_ENTER(tone_detection);

    *sig_tone = 0;

    /***  Multiply signal frame by Hanning window: ***/
    for (i = 0; i < 80; i++ )
    {
        sofh[i]     = GSM_SAT_MULT_R (sig_of[i], hann[i]);
        sofh[159-i] = GSM_SAT_MULT_R (sig_of[159-i], hann[i]);
    }

    /*  Auto_correlation  */

    smax = 0;

    for( k = 0; k<160; k++)
    {
        temp = GSM_ABS( sofh[k] );
        if ( temp > smax ) smax = temp;
    }

    if ( smax == 0 )
    {
        scalauto = 0;
    }
    else
    {
        //scalauto = gsm_sub( 4, gsm_norm( (INT32)smax << 16));
        scalauto = GSM_SUB( 4, gsm_norm( (INT32)smax << 16));
    }


    if ( scalauto > 0 )
    {
        //temp = 16384 >> gsm_sub( scalauto,1);
        temp = 16384 >> GSM_SUB( scalauto,1);

        for (k = 0; k < 160; k++)
        {
            sofh[k] = GSM_SAT_MULT_R( sofh[k], temp);
        }
    }

    /* Compute the L_ACF[..]: */

    for ( k=0;  k<5; k++)
    {
        register INT32 hi=0;
        register UINT32 lo=0;
        L_acfh[k] = 0;

        for(i=k; i<160; i++)
        {
            GSM_MLA( hi, lo ,sofh[i], sofh[i - k]);
            //L_temp = GSM_L_MULT( sofh[i], sofh[i - k] );
            //L_acfh[k] = GSM_L_ADD( L_acfh[k], L_temp );
        }
        L_acfh[k] = lo << 1;
    }

    /*********** Computation of the reflection coefficients  **********/

    if( L_acfh[0] == 0 )
    {
        for(i=1; i<5; i++) rc[i] = 0;
    }
    else
    {
        temp = gsm_norm( L_acfh[0] );

        for(k = 0; k < 5; k++ )
        {
            sacf[k] = (INT16)(gsm_L_asl( L_acfh[k],  temp ) >> 16);
        }

        for(i = 1; i < 4; i++)
        {
            K[5 - i] = sacf[i];
        }

        for(i = 0; i<5; i++)
        {
            P[i] = sacf[i];
        }


        /*Compute reflection coefficients:*/
        for(n = 1; n < 5; n++)
        {
            if( P[0] < GSM_ABS( P[1] ) )
            {
                for( i =n; i<5; i++)
                {
                    rc[i] = 0;
                }
                break;
            }
            else
            {
                rc[n] = gsm_div( GSM_ABS( P[1] ), P[0] );
                if( P[1] > 0 )
                {
                    //rc[n] = gsm_sub( 0, rc[n] );
                    rc[n] = GSM_SUB( 0, rc[n] );
                }
                if (n == 4) break;

                /* Schur recursion:*/

                P[0] = GSM_ADD( P[0], GSM_SAT_MULT_R( P[1], rc[n] ) );

                for(m = 1; m <= 4 - n;  m++)
                {
                    P[m] = GSM_ADD( P[m + 1], GSM_SAT_MULT_R( K[5 - m], rc[n] ) );

                    K[5 - m] = GSM_ADD( K[5 - m], GSM_SAT_MULT_R( P[m+1], rc[n] ) );
                }
            }
        }//for n
    }//else



    /*** Calculate filter coefficients ***/

    //step_up (2, rc, a);
    temp = rc[1] >> 2;
    a[1] = GSM_ADD( temp, GSM_SAT_MULT_R( rc[2], temp ) );
    a[2] = rc[2] >> 2;


    /*** Calculate ( 4*a[2] - a[1]*a[1] ) ***/

    L_den  = GSM_L_MULT (a[1], a[1]);

    L_temp = gsm_L_asl ((INT32)(a[2]), 16);

    L_num  = gsm_L_sub (L_temp, L_den);


    /*** Check if pole frequency is less than 385 Hz ***/

    if (L_num <= 0)
    {
        VPP_FR_PROFILE_FUNCTION_EXIT(tone_detection);
        return;
    }

    if (a[1] < 0)
    {
        temp = (INT16) (L_den >> 16);

        L_den = GSM_L_MULT (temp, FREQTH);

        L_temp = gsm_L_sub (L_num, L_den);

        if (L_temp < 0)
        {
            VPP_FR_PROFILE_FUNCTION_EXIT(tone_detection);
            return;
        }
    }
    /*** Calculate normalised prediction error ***/

    prederr = 0x7fff;

    // for (i = 0; i < 4; i++)
    for (i = 1; i < 5; i++)
    {
        temp = gsm_mult (rc[i], rc[i]);

        //temp = gsm_sub (0x7fff, temp);
        temp = GSM_SUB (0x7fff, temp);

        prederr = gsm_mult (prederr, temp);
    }

    /*** Test if prediction error is smaller than threshold ***/

    //temp = gsm_sub (prederr, PREDTH);
    temp = GSM_SUB (prederr, PREDTH);


    if (temp < 0)
    {
        *sig_tone = 1;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(tone_detection);
    return;
}

void threshold_adaptation (
    INT16 stat,
    INT16 ptch,
    INT16 sig_tone,
    INT16 rav1[],
    INT16 normrav1,
    Pfloat pvad,
    Pfloat acf0,
    INT16 r_vad[],
    INT16 *norm_rvad,
    Pfloat * th_vad
)
{
    INT16 comp, comp2;
    INT32 L_temp;
    INT16 temp;
    Pfloat p_temp;
    INT16 i;

    register UINT32 utmp;       /* for L_ADD */


    VPP_FR_PROFILE_FUNCTION_ENTER(threshold_adaptation);

    comp = 0;

    /*** Test if acf0 < pth; if yes set thvad to plev ***/

    //if (GSM_SATSUB(acf0.e, E_PTH) < 0)
    if (acf0.e < E_PTH)
    {
        comp = 1;
    }

    //if ((GSM_SATSUB  (acf0.e, E_PTH) == 0) && (GSM_SATSUB  (acf0.m, M_PTH) < 0))
    if ((acf0.e == E_PTH) && (acf0.m < M_PTH))
    {
        comp = 1;
    }

    //if (gsm_sub  (comp, 1) == 0)
    if (comp == 1)
    {
        th_vad->e = E_PLEV;
        th_vad->m = M_PLEV;
        VPP_FR_PROFILE_FUNCTION_EXIT(threshold_adaptation);
        return;
    }

    /*** Test if an adaption is required ***/

    //if (gsm_sub  (ptch, 1) == 0)
    if (ptch == 1)
    {
        comp = 1;
    }

    if (stat == 0)
    {
        comp = 1;
    }

    //if (gsm_sub  (sig_tone, 1) == 0)
    if (sig_tone == 1)
    {
        comp = 1;
    }

    //if (gsm_sub  (comp, 1) == 0)
    if (comp == 1)
    {
        adaptcount = 0;
        VPP_FR_PROFILE_FUNCTION_EXIT(threshold_adaptation);
        return;
    }
    /*** Increment adaptcount ***/

    adaptcount = GSM_ADD (adaptcount, 1);

    //if (gsm_sub  (adaptcount, 8) <= 0)
    if (adaptcount <= 8)
    {
        VPP_FR_PROFILE_FUNCTION_EXIT(threshold_adaptation);
        return;
    }

    /*** computation of thvad-(thvad/dec) ***/

    //th_vad->m = gsm_sub  (th_vad->m, gsm_asr (th_vad->m, 5));
    th_vad->m = GSM_SUB(th_vad->m, gsm_asr (th_vad->m, 5));


    //if (gsm_sub  (th_vad->m, 0x4000) < 0)
    if (th_vad->m < 0x4000)
    {
        th_vad->m = gsm_asl (th_vad->m, 1);
        //th_vad->e = gsm_sub  (th_vad->e, 1);
        th_vad->e = GSM_SUB(th_vad->e, 1);
    }
    /*** computation of pvad*fac ***/

    // L_temp = GSM_L_MULT (pvad.m, FAC);
    // L_temp = gsm_L_asr (L_temp, 15);

    L_temp = GSM_L_ADD( pvad.m, pvad.m );
    L_temp = GSM_L_ADD( L_temp, pvad.m );//pvad * 3  not use * FAC=3
    L_temp = L_temp >> 1;

    p_temp.e = GSM_ADD (pvad.e, 1);

    if (L_temp > 0x7fffL)
    {
        L_temp = gsm_L_asr (L_temp, 1);
        p_temp.e = GSM_ADD (p_temp.e, 1);
    }
    p_temp.m = (INT16) (L_temp);

    /*** test if thvad < pvad*fac ***/

    comp = 0;

    //if (gsm_sub  (th_vad->e, p_temp.e) < 0)
    if (th_vad->e < p_temp.e)
    {
        comp = 1;
    }

    //if ((gsm_sub  (th_vad->e, p_temp.e) == 0) &&
    //    (gsm_sub  (th_vad->m, p_temp.m) < 0))
    if ((th_vad->e == p_temp.e)&& (th_vad->m < p_temp.m) )
    {
        comp = 1;
    }

    /*** compute minimum(thvad+(thvad/inc), pvad*fac) when comp = 1 ***/

    //if (gsm_sub  (comp, 1) == 0)
    if (comp == 1)
    {
        /*** compute thvad + (thvad/inc) ***/

        L_temp = GSM_L_ADD ((INT32) (th_vad->m),
                            (INT32) (gsm_asr (th_vad->m, 4)));


        if (gsm_L_sub (L_temp, 0x7fffL) > 0)
        {
            th_vad->m = (INT16) (gsm_L_asr (L_temp, 1));
            th_vad->e = GSM_ADD (th_vad->e, 1);
        }
        else
        {
            th_vad->m = (INT16) (L_temp);
        }

        comp2 = 0;


        //if (gsm_sub  (p_temp.e, th_vad->e) < 0)
        if(p_temp.e <th_vad->e)
        {
            comp2 = 1;
        }

        //if ((gsm_sub  (p_temp.e, th_vad->e) == 0) &&
        //    (gsm_sub  (p_temp.m, th_vad->m) < 0))
        if ((p_temp.e == th_vad->e) && (p_temp.m < th_vad->m))
        {
            comp2 = 1;
        }

        //if (gsm_sub  (comp2, 1) == 0)
        if (comp2 == 1)
        {
            th_vad->e = p_temp.e;
            th_vad->m = p_temp.m;
        }
    }

    /*** compute pvad + margin ***/

    //if (gsm_sub  (pvad.e, E_MARGIN) == 0)
    if (pvad.e == E_MARGIN)
    {
        L_temp = GSM_L_ADD ((INT32) (pvad.m), (INT32) (M_MARGIN));
        p_temp.m = (INT16) (gsm_L_asr (L_temp, 1));
        p_temp.e = GSM_ADD (pvad.e, 1);
    }
    else
    {

        //if (gsm_sub  (pvad.e, E_MARGIN) > 0)
        if (pvad.e > E_MARGIN)
        {
            //temp = gsm_sub  (pvad.e, E_MARGIN);
            temp = GSM_SUB(pvad.e, E_MARGIN);
            temp = gsm_asr (M_MARGIN, temp);
            L_temp = GSM_L_ADD ((INT32) (pvad.m), (INT32) (temp));

            //if (gsm_L_sub (L_temp, 0x7fffL) > 0)
            if (L_temp > 0x7fffL)
            {
                p_temp.e = GSM_ADD (pvad.e, 1);
                p_temp.m = (INT16) (gsm_L_asr (L_temp, 1));
            }
            else
            {
                p_temp.e = pvad.e;
                p_temp.m = (INT16) (L_temp);
            }
        }
        else
        {
            //temp = gsm_sub  (E_MARGIN, pvad.e);
            temp = GSM_SUB(E_MARGIN, pvad.e);
            temp = gsm_asr (pvad.m, temp);
            L_temp = GSM_L_ADD ((INT32) (M_MARGIN), (INT32) (temp));

            //if (gsm_L_sub (L_temp, 0x7fffL) > 0)
            if (L_temp > 0x7fffL)
            {
                p_temp.e = GSM_ADD (E_MARGIN, 1);
                p_temp.m = (INT16) (gsm_L_asr (L_temp, 1));

            }
            else
            {
                p_temp.e = E_MARGIN;
                p_temp.m = (INT16) (L_temp);
            }
        }
    }

    /*** Test if thvad > pvad + margin ***/

    comp = 0;

    //if (gsm_sub  (th_vad->e, p_temp.e) > 0)
    if (th_vad->e > p_temp.e)
    {
        comp = 1;
    }

    //if ((gsm_sub  (th_vad->e, p_temp.e) == 0) &&
    //    (gsm_sub  (th_vad->m, p_temp.m) > 0))
    if ((th_vad->e == p_temp.e) && (th_vad->m > p_temp.m))
    {
        comp = 1;
    }

    //if (gsm_sub  (comp, 1) == 0)
    if (comp == 1)
    {
        th_vad->e = p_temp.e;
        th_vad->m = p_temp.m;
    }

    /*** Normalise and retain rvad[0..8] in memory ***/

    *norm_rvad = normrav1;

    for (i = 0; i <= 8; i++)
    {
        r_vad[i] = rav1[i];
    }

    /*** Set adaptcount to adp + 1 ***/

    adaptcount = 9;  // adp + 1          adp = CONST 8

    VPP_FR_PROFILE_FUNCTION_EXIT(threshold_adaptation);

    return;
}

INT16 vad_decision (
    Pfloat pvad,
    Pfloat th_vad
)
{
    INT16 vvad;

    if (pvad.e > th_vad.e)
    {
        vvad = 1;
    }
    else if ((pvad.e == th_vad.e) &&
             (pvad.m > th_vad.m))
    {
        vvad = 1;
    }
    else
    {
        vvad = 0;
    }

    return vvad;
}

INT16 vad_hangover (
    INT16 vvad
)
{

    //if (gsm_sub  (vvad, 1) == 0)
    if (vvad == 1)
    {
        burstcount = GSM_ADD (burstcount, 1);
    }
    else
    {
        burstcount = 0;
    }


    //if (gsm_sub  (burstcount, BURSTCONST) >= 0)
    if (burstcount >= BURSTCONST)
    {
        hangcount = HANGCONST;
        burstcount = BURSTCONST;
    }

    if (hangcount >= 0)
    {
        //hangcount = gsm_sub  (hangcount, 1);
        hangcount = GSM_SUB  (hangcount, 1);
        return 1;               /* vad = 1 */
    }
    return vvad;                /* vad = vvad */
}


//extern INT16 Nc_old_tx;



void periodicity_update (
    INT16 *lags//Nc
    //    INT16 *ptch
)
{
    INT16 lagcount, minlag, maxlag, smallag, temp;

    int i, j;

    VPP_FR_PROFILE_FUNCTION_ENTER(periodicity_update);

    lagcount = 0;

    for(i = 0; i < 4; i++)
    {
        //if(gsm_sub(oldlag, lags[i]) > 0)
        if(oldlag > lags[i])
        {
            minlag = lags[i];

            maxlag = oldlag;
        }
        else
        {
            minlag = oldlag;

            maxlag = lags[i] ;
        }


        /* Compute smallag (modulo operation not defined ): */

        smallag = maxlag;

        for(j = 0; j < 3; j++)
        {
            //if(gsm_sub(smallag, minlag) >= 0)
            if(smallag >= minlag)
            {

                //smallag =gsm_sub( smallag, minlag);
                smallag =GSM_SUB( smallag, minlag);

            }
        }

        /* Minimum of smallag and minlag - smallag: */

        //temp = gsm_sub( minlag, smallag );
        temp = GSM_SUB( minlag, smallag );

        //if(gsm_sub(temp, smallag) < 0)
        if(temp < smallag)
        {

            smallag = temp;

        }

        if( smallag < 2 )
        {

            lagcount = GSM_ADD( lagcount, 1 );

        }

        oldlag = lags[i];

    }

    veryoldlagcount = oldlagcount;

    oldlagcount = lagcount;

    VPP_FR_PROFILE_FUNCTION_EXIT(periodicity_update);

    return;

}

INT16 Periodicity_detection(INT16 oldlag_count, INT16 veryold_lagcount)
{

    INT16 temp, ptch;

    temp = GSM_ADD( oldlag_count, veryold_lagcount );

    if ( temp >= 4 )  ptch = 1;

    else ptch = 0;

    return ptch;

}

#if 0

void periodicity_update (
    INT16 *Nc,
    INT16 *ptch
)
{



    INT16 lagcount,smalllag;
    INT16 temp1,temp2;
    INT16 Nc_temp[5];

    int j;

    lagcount = 0;

    Nc_temp[0] = Nc_old_tx;

    for(j=1; j<5; j++)
    {
        Nc_temp[j] = *Nc++;
    }
    Nc_old_tx = Nc_temp[4];

    for(j=1; j<5; j++)
    {
        temp1 = GSM_MAX(Nc_temp[j],Nc_temp[j-1]);
        temp2 = GSM_MIN(Nc_temp[j],Nc_temp[j-1]);
        if(temp2) smalllag = temp1 % temp2 ;
        temp1 = temp2 - smalllag;
        temp1 = GSM_MIN(smalllag,temp1);

        if (temp1 < NTHRESH) lagcount++;
    }

    temp1 = oldlagcount + veryoldlagcount;
    if(temp1 >= NTHRESH) *ptch = 1;
    else *ptch = 0;

    veryoldlagcount = oldlagcount;
    oldlagcount = lagcount;

    return;

}
#endif


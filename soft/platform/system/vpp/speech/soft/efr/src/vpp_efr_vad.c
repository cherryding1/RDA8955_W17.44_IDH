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
#include "vpp_efr_cnst.h"
#include "vpp_efr_basic_op.h"
#include "vpp_efr_oper_32b.h"

#include "vpp_efr_vad.h"
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"



#include <stdio.h>

/* Constants of VAD hangover addition */

#define HANGCONST 10
#define BURSTCONST 3

/* Constant of spectral comparison */

#define STAT_THRESH 3670L       /* 0.056 */

/* Constants of periodicity detection */

#define LTHRESH 2
#define NTHRESH 4

/* Pseudo floating point representations of constants
   for threshold adaptation */

#define M_PTH    32500          /*** 130000.0 ***/
#define E_PTH    17
#define M_PLEV   21667          /*** 346666.7 ***/
#define E_PLEV   19
#define M_MARGIN 16927          /*** 69333340 ***/
#define E_MARGIN 27

#define FAC 17203               /* 2.1 */

/* Constants of tone detection */

#define FREQTH 3189
#define PREDTH 1464

/* Static variables of VAD */

static INT16 rvad[9], scal_rvad;
static Pfloat thvad;
static INT32 L_sacf[27];
static INT32 L_sav0[36];
static INT16 pt_sacf, pt_sav0;
static INT32 L_lastdm;
static INT16 adaptcount;
static INT16 burstcount, hangcount;
static INT16 oldlagcount, veryoldlagcount, oldlag;

INT16 ptch;

/*************************************************************************
 *
 *   FUNCTION NAME: vad_reset
 *
 *   PURPOSE:  Resets the static variables of the VAD to their
 *             initial values
 *
 *************************************************************************/

void __attribute__((section(".sramtext"))) vad_reset ()
{
    INT16 i;

    /* Initialize rvad variables */
    rvad[0] = 0x6000;
    for (i = 1; i < 9; i++)
    {
        rvad[i] = 0;
    }
    scal_rvad = 7;

    /* Initialize threshold level */
    thvad.e = 20;               /*** exponent ***/
    thvad.m = 27083;            /*** mantissa ***/

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

    /* Initialize threshold adaptation variable */
    adaptcount = 0;

    /* Initialize VAD hangover addition variables */
    burstcount = 0;
    hangcount = -1;

    /* Initialize periodicity detection variables */
    oldlagcount = 0;
    veryoldlagcount = 0;
    oldlag = 18;

    ptch = 1;

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  vad_computation
 *
 *     PURPOSE:   Returns a decision as to whether the current frame being
 *                processed by the speech encoder contains speech or not.
 *
 *     INPUTS:    r_h[0..8]     autocorrelation of input signal frame (msb)
 *                r_l[0..8]     autocorrelation of input signal frame (lsb)
 *                scal_acf      scaling factor for the autocorrelations
 *                rc[0..3]      speech encoder reflection coefficients
 *                ptch          flag to indicate a periodic signal component
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: vad decision
 *
 ***************************************************************************/

INT16 vad_computation (
    INT16 r_h[],
    INT16 r_l[],
    INT16 scal_acf,
    INT16 rc[],
    INT16 ptch_flag
)
{


    INT32 L_av0[9], L_av1[9];
    INT16 vad, vvad, rav1[9], scal_rav1, stat, tone;
    Pfloat acf0, pvad;

    VPP_EFR_PROFILE_FUNCTION_ENTER(vad_computation);


    energy_computation (r_h, scal_acf, rvad, scal_rvad, &acf0, &pvad);
    acf_averaging (r_h, r_l, scal_acf, L_av0, L_av1);
    predictor_values (L_av1, rav1, &scal_rav1);
    stat = spectral_comparison (rav1, scal_rav1, L_av0);
    tone_detection (rc, &tone);
    threshold_adaptation (stat, ptch_flag, tone, rav1, scal_rav1, pvad, acf0,
                          rvad, &scal_rvad, &thvad);
    vvad = vad_decision (pvad, thvad);
    vad = vad_hangover (vvad);

    VPP_EFR_PROFILE_FUNCTION_EXIT(vad_computation);
    return vad;
}

/****************************************************************************
 *
 *     FUNCTION:  energy_computation
 *
 *     PURPOSE:   Computes the input and residual energies of the adaptive
 *                filter in a floating point representation.
 *
 *     INPUTS:    r_h[0..8]      autocorrelation of input signal frame (msb)
 *                scal_acf       scaling factor for the autocorrelations
 *                rvad[0..8]     autocorrelated adaptive filter coefficients
 *                scal_rvad      scaling factor for rvad[]
 *
 *     OUTPUTS:   *acf0          signal frame energy (mantissa+exponent)
 *                *pvad          filtered signal energy (mantissa+exponent)
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void energy_computation (
    INT16 r_h[],
    INT16 scal_acf,
    INT16 r_vad[],
    INT16 scal_r_vad,
    Pfloat * acf0,
    Pfloat * pvad
)
{
    INT16 i, temp, norm_prod;
    INT32 L_temp;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;

    /* r[0] is always greater than zero (no need to test for r[0] == 0) */

    /* Computation of acf0 (exponent and mantissa) */

    //acf0->e = sub (32, scal_acf);
    acf0->e = SUB (32, scal_acf);

    acf0->m = r_h[0] & 0x7ff8;

    /* Computation of pvad (exponent and mantissa) */

    //pvad->e = add (acf0->e, 14);
    pvad->e = ADD (acf0->e, 14);

    //pvad->e = sub (pvad->e, scal_r_vad);
    pvad->e = SUB (pvad->e, scal_r_vad);


    //L_temp = 0L;
    tmp_lo = 0;
    for (i = 1; i <= 8; i++)
    {
        //temp = shr (r_h[i], 3);
        temp = SHR_D(r_h[i], 3);
        //L_temp = L_mac (L_temp, temp, r_vad[i]);
        //L_temp = L_MAC(L_temp, temp, r_vad[i]);
        VPP_MLA16(tmp_hi,tmp_lo,temp, r_vad[i]);
    }

    L_temp = VPP_SCALE64_TO_16(tmp_hi,tmp_lo);
    //temp = shr (r_h[0], 3);
    temp = SHR_D(r_h[0], 3);
    //L_temp = L_add (L_temp, L_shr (L_mult (temp, r_vad[0]), 1));
    L_temp = L_ADD(L_temp, L_SHR_D(L_MULT (temp, r_vad[0]), 1));

    if (L_temp <= 0L)
    {
        L_temp = 1L;
    }
    norm_prod = norm_l (L_temp);
    //pvad->e = sub (pvad->e, norm_prod);
    pvad->e = SUB (pvad->e, norm_prod);

    //pvad->m = extract_h(L_shl (L_temp, norm_prod));
    pvad->m = EXTRACT_H(L_SHL(L_temp, norm_prod));

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  acf_averaging
 *
 *     PURPOSE:   Computes the arrays L_av0[0..8] and L_av1[0..8].
 *
 *     INPUTS:    r_h[0..8]     autocorrelation of input signal frame (msb)
 *                r_l[0..8]     autocorrelation of input signal frame (lsb)
 *                scal_acf      scaling factor for the autocorrelations
 *
 *     OUTPUTS:   L_av0[0..8]   ACF averaged over last four frames
 *                L_av1[0..8]   ACF averaged over previous four frames
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void acf_averaging (
    INT16 r_h[],
    INT16 r_l[],
    INT16 scal_acf,
    INT32 L_av0[],
    INT32 L_av1[]
)
{
    INT32 L_temp;
    INT16 scale;
    INT16 i;

    //scale = add (9, scal_acf);
    scale = ADD (9, scal_acf);

    for (i = 0; i <= 8; i++)
    {
        //L_temp = L_shr (L_Comp (r_h[i], r_l[i]), scale);
        L_temp = L_SHR_V(L_Comp (r_h[i], r_l[i]), scale);

        //L_av0[i] = L_add (L_sacf[i], L_temp);
        L_av0[i] = L_ADD(L_sacf[i], L_temp);

        //L_av0[i] = L_add (L_sacf[i + 9], L_av0[i]);
        L_av0[i] = L_ADD(L_sacf[i + 9], L_av0[i]);

        //L_av0[i] = L_add (L_sacf[i + 18], L_av0[i]);
        L_av0[i] = L_ADD(L_sacf[i + 18], L_av0[i]);

        L_sacf[pt_sacf + i] = L_temp;
        L_av1[i] = L_sav0[pt_sav0 + i];
        L_sav0[pt_sav0 + i] = L_av0[i];
    }

    /* Update the array pointers */


    //if (sub (pt_sacf, 18) == 0)
    if (SUB (pt_sacf, 18) == 0)
    {
        pt_sacf = 0;
    }
    else
    {
        //pt_sacf = add (pt_sacf, 9);
        pt_sacf = ADD (pt_sacf, 9);
    }


    //if (sub (pt_sav0, 27) == 0)
    if (SUB (pt_sav0, 27) == 0)
    {
        pt_sav0 = 0;
    }
    else
    {
        //pt_sav0 = add (pt_sav0, 9);
        pt_sav0 = ADD (pt_sav0, 9);
    }

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  predictor_values
 *
 *     PURPOSE:   Computes the array rav[0..8] needed for the spectral
 *                comparison and the threshold adaptation.
 *
 *     INPUTS:    L_av1[0..8]   ACF averaged over previous four frames
 *
 *     OUTPUTS:   rav1[0..8]    ACF obtained from L_av1
 *                *scal_rav1    rav1[] scaling factor
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void predictor_values (
    INT32 L_av1[],
    INT16 rav1[],
    INT16 *scal_rav1
)
{
    INT16 vpar[8], aav1[9];

    schur_recursion (L_av1, vpar);
    step_up (8, vpar, aav1);
    compute_rav1 (aav1, rav1, scal_rav1);

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  schur_recursion
 *
 *     PURPOSE:   Uses the Schur recursion to compute adaptive filter
 *                reflection coefficients from an autorrelation function.
 *
 *     INPUTS:    L_av1[0..8]    autocorrelation function
 *
 *     OUTPUTS:   vpar[0..7]     reflection coefficients
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void schur_recursion (
    INT32 L_av1[],
    INT16 vpar[]
)
{
    INT16 acf[9], pp[9], kk[9], temp;
    INT16 i, k, m, n;
    register INT32 pp_hi=0;
    register UINT32 pp_lo=0;


    /*** Schur recursion with 16-bit arithmetic ***/


    if (L_av1[0] == 0)
    {
        for (i = 0; i < 8; i++)
        {
            vpar[i] = 0;
        }
        return;
    }
    temp = norm_l (L_av1[0]);

    for (k = 0; k <= 8; k++)
    {
        //acf[k] = extract_h(L_shl (L_av1[k], temp));
        acf[k] = EXTRACT_H( L_SHL(L_av1[k], temp));

    }

    /*** Initialize arrays pp[..] and kk[..] for the recursion: ***/

    for (i = 1; i <= 7; i++)
    {
        kk[9 - i] = acf[i];
    }

    for (i = 0; i <= 8; i++)
    {
        pp[i] = acf[i];
    }

    /*** Compute Parcor coefficients: ***/

    for (n = 0; n < 8; n++)
    {

        /*if ((pp[0] == 0) ||
            (sub (pp[0], abs_s (pp[1])) < 0))*/
        if ((pp[0] == 0) ||
                (SUB (pp[0], ABS_S(pp[1])) < 0))
        {
            for (i = n; i < 8; i++)
            {
                vpar[i] = 0;
            }
            return;
        }
        vpar[n] = div_s (ABS_S(pp[1]), pp[0]);


        if (pp[1] > 0)
        {
            //vpar[n] = negate (vpar[n]);
            vpar[n] = NEGATE(vpar[n]);
        }

        //if (sub (n, 7) == 0)
        if (SUB (n, 7) == 0)
        {
            return;
        }
        /*** Schur recursion: ***/

        //pp[0] = add (pp[0], mult_r (pp[1], vpar[n]));
        //pp[0] = ADD (pp[0], MULT_R(pp[1], vpar[n]));
        VPP_MULT_R(pp_hi, pp_lo, pp[1], vpar[n]);
        pp[0] = ADD (pp[0], L_SHR_D((INT32)pp_lo, 15));

        for (m = 1; m <= 7 - n; m++)
        {
            //pp[m] = add (pp[1 + m], mult_r (kk[9 - m], vpar[n]));
            //pp[m] = ADD (pp[1 + m], MULT_R(kk[9 - m], vpar[n]));
            pp_hi = 0;
            VPP_MULT_R(pp_hi, pp_lo, kk[9 - m], vpar[n]);
            pp[m] = ADD (pp[1 + m], L_SHR_D((INT32)pp_lo, 15));

            //kk[9 - m] = add (kk[9 - m], mult_r (pp[1 + m], vpar[n]));
            //kk[9 - m] = ADD (kk[9 - m], MULT_R(pp[1 + m], vpar[n]));
            pp_hi = 0;
            VPP_MULT_R(pp_hi, pp_lo, pp[1 + m], vpar[n]);
            kk[9 - m] = ADD (kk[9 - m], L_SHR_D((INT32)pp_lo, 15));

        }
    }

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  step_up
 *
 *     PURPOSE:   Computes the transversal filter coefficients from the
 *                reflection coefficients.
 *
 *     INPUTS:    np               filter order (2..8)
 *                vpar[0..np-1]    reflection coefficients
 *
 *     OUTPUTS:   aav1[0..np]      transversal filter coefficients
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void step_up (
    INT16 np,
    INT16 vpar[],
    INT16 aav1[]
)
{
    INT32 L_coef[9], L_work[9];
    INT16 temp;
    INT16 i, m;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;


    /*** Initialization of the step-up recursion ***/

    L_coef[0] = 0x20000000L;
    //L_coef[1] = L_shl (L_deposit_l (vpar[0]), 14);
    L_coef[1] = L_SHL_SAT(L_DEPOSIT_L (vpar[0]), 14);

    /*** Loop on the LPC analysis order: ***/

    for (m = 2; m <= np; m++)
    {
        for (i = 1; i < m; i++)
        {
            //temp = extract_h (L_coef[m - i]);
            temp = EXTRACT_H (L_coef[m - i]);
            tmp_lo = L_coef[i];
            //L_work[i] = L_mac (L_coef[i], vpar[m - 1], temp);
            //L_work[i] = L_MAC(L_coef[i], vpar[m - 1], temp);
            VPP_MLA16(tmp_hi, tmp_lo, vpar[m - 1], temp);
            L_work[i] = VPP_SCALE64_TO_16(tmp_hi, tmp_lo);
        }

        for (i = 1; i < m; i++)
        {
            L_coef[i] = L_work[i];
        }

        //L_coef[m] = L_shl (L_deposit_l (vpar[m - 1]), 14);
        L_coef[m] = L_SHL_SAT(L_DEPOSIT_L (vpar[m - 1]), 14);

    }

    /*** Keep the aav1[0..np] in 15 bits ***/

    for (i = 0; i <= np; i++)
    {
        //aav1[i] = extract_h (L_shr (L_coef[i], 3));
        aav1[i] = EXTRACT_H (L_SHR_D(L_coef[i], 3));

    }

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  compute_rav1
 *
 *     PURPOSE:   Computes the autocorrelation function of the adaptive
 *                filter coefficients.
 *
 *     INPUTS:    aav1[0..8]     adaptive filter coefficients
 *
 *     OUTPUTS:   rav1[0..8]     ACF of aav1
 *                *scal_rav1     rav1[] scaling factor
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void compute_rav1 (
    INT16 aav1[],
    INT16 rav1[],
    INT16 *scal_rav1
)
{
    INT32 L_work[9];
    INT16 i, k;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;


    /*** Computation of the rav1[0..8] ***/

    for (i = 0; i <= 8; i++)
    {
        //L_work[i] = 0L;
        tmp_lo =0;

        for (k = 0; k <= 8 - i; k++)
        {
            //L_work[i] = L_mac (L_work[i], aav1[k], aav1[k + i]);
            //L_work[i] = L_MAC(L_work[i], aav1[k], aav1[k + i]);
            VPP_MLA16(tmp_hi, tmp_lo, aav1[k], aav1[k + i]);
        }
        L_work[i] = VPP_SCALE64_TO_16(tmp_hi, tmp_lo);
    }


    if (L_work[0] == 0L)
    {
        *scal_rav1 = 0;
    }
    else
    {
        *scal_rav1 = norm_l (L_work[0]);
    }

    for (i = 0; i <= 8; i++)
    {
        //rav1[i] = extract_h (L_shl (L_work[i], *scal_rav1));
        rav1[i] = EXTRACT_H(L_SHL(L_work[i], *scal_rav1));
    }

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  spectral_comparison
 *
 *     PURPOSE:   Computes the stat flag needed for the threshold
 *                adaptation decision.
 *
 *     INPUTS:    rav1[0..8]      ACF obtained from L_av1
 *                *scal_rav1      rav1[] scaling factor
 *                L_av0[0..8]     ACF averaged over last four frames
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: flag to indicate spectral stationarity
 *
 ***************************************************************************/

INT16 spectral_comparison (
    INT16 rav1[],
    INT16 scal_rav1,
    INT32 L_av0[]
)
{
    INT32 L_dm, L_sump, L_temp;
    INT16 stat, sav0[9], shift, divshift, temp;
    INT16 i;
    register INT32 sump_hi=0;
    register UINT32 sump_lo=0;

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
        //shift = sub (norm_l (L_av0[0]), 3);
        shift = SUB (norm_l (L_av0[0]), 3);
        for (i = 0; i <= 8; i++)
        {
            //sav0[i] = extract_h (L_shl (L_av0[i], shift));
            sav0[i] = EXTRACT_H(L_SHL(L_av0[i], shift));
        }
    }

    /*** Compute partial sum of dm ***/

    //L_sump = 0L;
    sump_lo = 0;
    for (i = 1; i <= 8; i++)
    {
        //L_sump = L_mac (L_sump, rav1[i], sav0[i]);
        //L_sump = L_MAC(L_sump, rav1[i], sav0[i]);
        VPP_MLA16(sump_hi,sump_lo, rav1[i], sav0[i]);
    }

    L_sump = VPP_SCALE64_TO_16(sump_hi,sump_lo);

    /*** Compute the division of the partial sum by sav0[0] ***/


    if (L_sump < 0L)
    {
        //L_temp = L_negate (L_sump);
        L_temp = L_NEGATE(L_sump);
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
        //sav0[0] = shl (sav0[0], 3);
        sav0[0] = SHL(sav0[0], 3);
        shift = norm_l (L_temp);

        //temp = extract_h (L_shl (L_temp, shift));
        temp = EXTRACT_H(L_SHL(L_temp, shift));

        //if (sub (sav0[0], temp) >= 0)
        if (SUB (sav0[0], temp) >= 0)
        {
            divshift = 0;
            temp = div_s (temp, sav0[0]);
        }
        else
        {
            divshift = 1;
            //temp = sub (temp, sav0[0]);
            temp = SUB (temp, sav0[0]);
            temp = div_s (temp, sav0[0]);
        }


        //if (sub (divshift, 1) == 0)
        if (SUB (divshift, 1) == 0)
        {
            L_dm = 0x8000L;
        }
        else
        {
            L_dm = 0L;
        }

        //L_dm = L_shl (L_add (L_dm, L_deposit_l (temp)), 1);
        L_dm = L_SHL_SAT(L_ADD (L_dm, L_DEPOSIT_L (temp)), 1);

        if (L_sump < 0L)
        {
            //L_dm = L_negate (L_dm);
            L_dm = L_NEGATE(L_dm);
        }
    }

    /*** Re-normalization and final computation of L_dm ***/

    //L_dm = L_shl (L_dm, 14);
    L_dm = L_SHL_SAT(L_dm, 14);
    //L_dm =L_shr (L_dm, shift);
    L_dm =L_SHR_V(L_dm, shift);
    //L_dm = L_add (L_dm, L_shl (L_deposit_l (rav1[0]), 11));
    L_dm = L_ADD(L_dm, L_SHL_SAT(L_DEPOSIT_L (rav1[0]), 11));
    //L_dm = L_shr (L_dm, scal_rav1);
    L_dm = L_SHR_V(L_dm, scal_rav1);

    /*** Compute the difference and save L_dm ***/

    //L_temp = L_sub (L_dm, L_lastdm);
    L_temp = L_SUB(L_dm, L_lastdm);

    L_lastdm = L_dm;


    if (L_temp < 0L)
    {
        //L_temp = L_negate (L_temp);
        L_temp = L_NEGATE(L_temp);
    }
    /*** Evaluation of the stat flag ***/

    //L_temp = L_sub (L_temp, STAT_THRESH);
    L_temp = L_SUB(L_temp, STAT_THRESH);


    if (L_temp < 0L)
    {
        stat = 1;
    }
    else
    {
        stat = 0;
    }

    return stat;
}

/****************************************************************************
 *
 *     FUNCTION:  threshold_adaptation
 *
 *     PURPOSE:   Evaluates the secondary VAD decision.  If speech is not
 *                present then the noise model rvad and adaptive threshold
 *                thvad are updated.
 *
 *     INPUTS:    stat          flag to indicate spectral stationarity
 *                ptch          flag to indicate a periodic signal component
 *                tone          flag to indicate a tone signal component
 *                rav1[0..8]    ACF obtained from L_av1
 *                scal_rav1     rav1[] scaling factor
 *                pvad          filtered signal energy (mantissa+exponent)
 *                acf0          signal frame energy (mantissa+exponent)
 *
 *     OUTPUTS:   rvad[0..8]    autocorrelated adaptive filter coefficients
 *                *scal_rvad    rvad[] scaling factor
 *                *thvad        decision threshold (mantissa+exponent)
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void threshold_adaptation (
    INT16 stat,
    INT16 ptch_flag,
    INT16 tone,
    INT16 rav1[],
    INT16 scal_rav1,
    Pfloat pvad,
    Pfloat acf0,
    INT16 r_vad[],
    INT16 *scal_r_vad,
    Pfloat * th_vad
)
{
    INT16 comp, comp2;
    INT32 L_temp;
    INT16 temp;
    Pfloat p_temp;
    INT16 i;

    comp = 0;

    /*** Test if acf0 < pth; if yes set thvad to plev ***/


    //if (sub (acf0.e, E_PTH) < 0)
    if (SUB (acf0.e, E_PTH) < 0)
    {
        comp = 1;
    }

    //if ((sub (acf0.e, E_PTH) == 0) && (sub (acf0.m, M_PTH) < 0))
    if ((SUB (acf0.e, E_PTH) == 0) && (SUB (acf0.m, M_PTH) < 0))
    {
        comp = 1;
    }

    //if (sub (comp, 1) == 0)
    if (SUB (comp, 1) == 0)
    {
        th_vad->e = E_PLEV;
        th_vad->m = M_PLEV;

        return;
    }
    /*** Test if an adaption is required ***/


    //if (sub (ptch_flag, 1) == 0)
    if (SUB (ptch_flag, 1) == 0)
    {
        comp = 1;
    }

    if (stat == 0)
    {
        comp = 1;
    }

    //if (sub (tone, 1) == 0)
    if (SUB (tone, 1) == 0)
    {
        comp = 1;
    }

    //if (sub (comp, 1) == 0)
    if (SUB (comp, 1) == 0)
    {
        adaptcount = 0;
        return;
    }
    /*** Increment adaptcount ***/

    //adaptcount = add (adaptcount, 1);
    adaptcount = ADD (adaptcount, 1);


    //if (sub (adaptcount, 8) <= 0)
    if (SUB (adaptcount, 8) <= 0)
    {
        return;
    }
    /*** computation of thvad-(thvad/dec) ***/

    //thvad->m = sub (thvad->m, shr (thvad->m, 5));
    th_vad->m = SUB (th_vad->m, SHR_D(th_vad->m, 5));



    //if (sub (thvad->m, 0x4000) < 0)
    if (SUB (th_vad->m, 0x4000) < 0)
    {
        //th_vad->m = shl (th_vad->m, 1);
        th_vad->m = SHL(th_vad->m, 1);
        //th_vad->e = sub (th_vad->e, 1);
        th_vad->e = SUB (th_vad->e, 1);

    }
    /*** computation of pvad*fac ***/

    //L_temp = L_mult (pvad.m, FAC);
    L_temp = L_MULT(pvad.m, FAC);
    //L_temp = L_shr (L_temp, 15);
    L_temp = L_SHR_D(L_temp, 15);
    //p_temp.e = add (pvad.e, 1);
    p_temp.e = ADD (pvad.e, 1);



    if (L_temp > 0x7fffL)
    {
        //L_temp = L_shr (L_temp, 1);
        L_temp = L_SHR_D(L_temp, 1);
        //p_temp.e = add (p_temp.e, 1);
        p_temp.e = ADD (p_temp.e, 1);

    }

    //p_temp.m = extract_l(L_temp);
    p_temp.m = EXTRACT_L (L_temp);

    /*** test if th_vad < pvad*fac ***/


    //if (sub (th_vad->e, p_temp.e) < 0)
    if (SUB (th_vad->e, p_temp.e) < 0)
    {
        comp = 1;
    }

    /*if ((sub (th_vad->e, p_temp.e) == 0) &&
        (sub (th_vad->m, p_temp.m) < 0))*/
    if ((SUB (th_vad->e, p_temp.e) == 0) &&
            (SUB (th_vad->m, p_temp.m) < 0))
    {
        comp = 1;
    }
    /*** compute minimum(th_vad+(th_vad/inc), pvad*fac) when comp = 1 ***/


    //if (sub (comp, 1) == 0)
    if (SUB (comp, 1) == 0)
    {
        /*** compute th_vad + (th_vad/inc) ***/

        /*L_temp = L_add (L_deposit_l (th_vad->m),
                       L_deposit_l (shr (th_vad->m, 4)));*/

        L_temp = L_ADD(L_DEPOSIT_L(th_vad->m),
                       L_DEPOSIT_L(SHR_D(th_vad->m, 4)));




        //if (L_sub (L_temp, 0x7fffL, out_sub) > 0)
        if ( L_SUB(L_temp, 0x7fffL) >0)
        {
            //th_vad->m = extract_l(L_shr (L_temp, 1));
            th_vad->m = EXTRACT_L(L_SHR_D(L_temp, 1));
            //th_vad->e = add (th_vad->e, 1);
            th_vad->e = ADD (th_vad->e, 1);

        }
        else
        {
            //th_vad->m = extract_l(L_temp);
            th_vad->m = EXTRACT_L (L_temp);
        }

        comp2 = 0;


        //if (sub (p_temp.e, th_vad->e) < 0)
        if (SUB (p_temp.e, th_vad->e) < 0)
        {
            comp2 = 1;
        }

        /*if ((sub (p_temp.e, th_vad->e) == 0) &&
            (sub (p_temp.m, th_vad->m) < 0))*/
        if ((SUB (p_temp.e, th_vad->e) == 0) &&
                (SUB (p_temp.m, th_vad->m) < 0))
        {
            comp2 = 1;
        }

        //if (sub (comp2, 1) == 0)
        if (SUB (comp2, 1) == 0)
        {
            th_vad->e = p_temp.e;
            th_vad->m = p_temp.m;
        }
    }
    /*** compute pvad + margin ***/


    //if (sub (pvad.e, E_MARGIN) == 0)
    if (SUB (pvad.e, E_MARGIN) == 0)
    {
        //L_temp = L_add (L_deposit_l (pvad.m), L_deposit_l (M_MARGIN));
        L_temp = L_ADD(L_DEPOSIT_L (pvad.m), L_DEPOSIT_L(M_MARGIN));

        //p_temp.m = extract_l(L_shr (L_temp, 1));
        p_temp.m = EXTRACT_L(L_SHR_D(L_temp, 1));
        //p_temp.e = add (pvad.e, 1);
        p_temp.e = ADD (pvad.e, 1);

    }
    else
    {

        //if (sub (pvad.e, E_MARGIN) > 0)
        if (SUB (pvad.e, E_MARGIN) > 0)
        {
            //temp = sub (pvad.e, E_MARGIN);
            temp = SUB (pvad.e, E_MARGIN);
            //temp = shr (M_MARGIN, temp);
            temp = SHR_V(M_MARGIN, temp);
            //L_temp = L_add (L_deposit_l (pvad.m), L_deposit_l (temp));
            L_temp = L_ADD(L_DEPOSIT_L(pvad.m), L_DEPOSIT_L (temp));


            //if (L_sub (L_temp, 0x7fffL) > 0)
            if (L_SUB(L_temp, 0x7fffL) > 0)
            {
                //p_temp.e = add (pvad.e, 1);
                p_temp.e = ADD (pvad.e, 1);

                //p_temp.m = extract_l(L_shr (L_temp, 1));
                p_temp.m = EXTRACT_L(L_SHR_D(L_temp, 1));

            }
            else
            {
                p_temp.e = pvad.e;
                //p_temp.m = extract_l(L_temp);ptch
                p_temp.m = EXTRACT_L (L_temp);
            }
        }
        else
        {
            //temp = sub (E_MARGIN, pvad.e);
            temp = SUB (E_MARGIN, pvad.e);
            //temp = shr (pvad.m, temp);
            temp = SHR_V(pvad.m, temp);

            //L_temp = L_add (L_deposit_l (M_MARGIN), L_deposit_l (temp));
            L_temp = L_ADD(L_DEPOSIT_L(M_MARGIN), L_DEPOSIT_L(temp));


            //if (L_sub (L_temp, 0x7fffL) > 0)
            if (L_SUB(L_temp, 0x7fffL)> 0)
            {
                //p_temp.e = add (E_MARGIN, 1);
                p_temp.e = ADD (E_MARGIN, 1);

                //p_temp.m = extract_l(L_shr (L_temp, 1));
                p_temp.m = EXTRACT_L(L_SHR_D(L_temp, 1));
            }
            else
            {
                p_temp.e = E_MARGIN;
                //p_temp.m = extract_l(L_temp);
                p_temp.m = EXTRACT_L (L_temp);
            }
        }
    }

    /*** Test if th_vad > pvad + margin ***/

    comp = 0;


    //if (sub (th_vad->e, p_temp.e) > 0)
    if (SUB (th_vad->e, p_temp.e) > 0)
    {
        comp = 1;
    }

    /*if ((sub (th_vad->e, p_temp.e) == 0) &&
        (sub (th_vad->m, p_temp.m) > 0))*/
    if ((SUB (th_vad->e, p_temp.e) == 0) &&
            (SUB (th_vad->m, p_temp.m) > 0))
    {
        comp = 1;
    }

    //if (sub (comp, 1) == 0)
    if (SUB (comp, 1) == 0)
    {
        th_vad->e = p_temp.e;
        th_vad->m = p_temp.m;
    }
    /*** Normalise and retain rvad[0..8] in memory ***/

    *scal_r_vad = scal_rav1;

    for (i = 0; i <= 8; i++)
    {
        r_vad[i] = rav1[i];
    }

    /*** Set adaptcount to adp + 1 ***/

    adaptcount = 9;

    return;
}

/****************************************************************************
 *
 *     FUNCTION:  tone_detection
 *
 *     PURPOSE:   Computes the tone flag needed for the threshold
 *                adaptation decision.
 *
 *     INPUTS:    rc[0..3]    reflection coefficients calculated in the
 *                            speech encoder short term predictor
 *
 *     OUTPUTS:   *tone       flag to indicate a periodic signal component
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void tone_detection (
    INT16 rc[],
    INT16 *tone
)
{
    INT32 L_num, L_den, L_temp;
    INT16 temp, prederr, a[3];
    INT16 i;

    *tone = 0;

    /*** Calculate filter coefficients ***/

    step_up (2, rc, a);

    /*** Calculate ( a[1] * a[1] ) ***/

    //temp = shl (a[1], 3);
    temp = SHL(a[1], 3);
    //L_den = L_mult (temp, temp);
    L_den = L_MULT(temp, temp);

    /*** Calculate ( 4*a[2] - a[1]*a[1] ) ***/

    //L_temp = L_shl (L_deposit_h (a[2]), 3);
    L_temp = L_SHL_SAT(L_DEPOSIT_H (a[2]), 3);
    //L_num = L_sub (L_temp, L_den);
    L_num = L_SUB(L_temp, L_den);

    /*** Check if pole frequency is less than 385 Hz ***/


    if (L_num <= 0)
    {
        return;
    }

    if (a[1] < 0)
    {
        //temp = extract_h (L_den);
        temp = EXTRACT_H(L_den);
        //L_den = L_mult (temp, FREQTH);
        L_den = L_MULT(temp, FREQTH);

        //L_temp = L_sub (L_num, L_den);
        L_temp = L_SUB(L_num, L_den);


        if (L_temp < 0)
        {
            return;
        }
    }
    /*** Calculate normalised prediction error ***/

    prederr = 0x7fff;

    for (i = 0; i < 4; i++)
    {
        //temp = mult (rc[i], rc[i]);
        temp = MULT(rc[i], rc[i]);
        //temp = sub (0x7fff, temp);
        temp = SUB (0x7fff, temp);
        //prederr = mult (prederr, temp);
        prederr = MULT(prederr, temp);
    }

    /*** Test if prediction error is smaller than threshold ***/

    //temp = sub (prederr, PREDTH);
    temp = SUB (prederr, PREDTH);


    if (temp < 0)
    {
        *tone = 1;
    }
    return;
}

/****************************************************************************
 *
 *     FUNCTION:  vad_decision
 *
 *     PURPOSE:   Computes the VAD decision based on the comparison of the
 *                floating point representations of pvad and thvad.
 *
 *     INPUTS:    pvad          filtered signal energy (mantissa+exponent)
 *                thvad         decision threshold (mantissa+exponent)
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: vad decision before hangover is added
 *
 ***************************************************************************/

INT16 vad_decision (
    Pfloat pvad,
    Pfloat th_vad
)
{
    INT16 vvad;


    //if (sub (pvad.e, th_vad.e) > 0)
    if (SUB (pvad.e, th_vad.e) > 0)
    {
        vvad = 1;
    }
    /*else if ((sub (pvad.e, th_vad.e) == 0) &&
               (sub (pvad.m, th_vad.m) > 0))*/
    else if ((SUB (pvad.e, th_vad.e) == 0) &&
             (SUB (pvad.m, th_vad.m) > 0))
    {
        vvad = 1;
    }
    else
    {
        vvad = 0;
    }

    return vvad;
}

/****************************************************************************
 *
 *     FUNCTION:  vad_hangover
 *
 *     PURPOSE:   Computes the final VAD decision for the current frame
 *                being processed.
 *
 *     INPUTS:    vvad           vad decision before hangover is added
 *
 *     OUTPUTS:   none
 *
 *     RETURN VALUE: vad decision after hangover is added
 *
 ***************************************************************************/

INT16 vad_hangover (
    INT16 vvad
)
{

    //if (sub (vvad, 1) == 0)
    if (SUB (vvad, 1) == 0)
    {
        //burstcount = add (burstcount, 1);
        burstcount = ADD (burstcount, 1);
    }
    else
    {
        burstcount = 0;
    }


    //if (sub (burstcount, BURSTCONST) >= 0)
    if (SUB (burstcount, BURSTCONST) >= 0)
    {
        hangcount = HANGCONST;
        burstcount = BURSTCONST;
    }

    if (hangcount >= 0)
    {
        //hangcount = sub (hangcount, 1);
        hangcount = SUB (hangcount, 1);
        return 1;               /* vad = 1 */
    }
    return vvad;                /* vad = vvad */
}

/****************************************************************************
 *
 *     FUNCTION:  periodicity_update
 *
 *     PURPOSE:   Computes the ptch flag needed for the threshold
 *                adaptation decision for the next frame.
 *
 *     INPUTS:    lags[0..1]       speech encoder long term predictor lags
 *
 *     OUTPUTS:   *ptch            Boolean voiced / unvoiced decision
 *
 *     RETURN VALUE: none
 *
 ***************************************************************************/

void periodicity_update (
    INT16 lags[],
    INT16 *ptch_flag
)
{
    INT16 minlag, maxlag, lagcount, temp;
    INT16 i;

    /*** Run loop for the two halves in the frame ***/

    lagcount = 0;

    for (i = 0; i <= 1; i++)
    {
        /*** Search the maximum and minimum of consecutive lags ***/


        //if (sub (oldlag, lags[i]) > 0)
        if (SUB (oldlag, lags[i]) > 0)
        {
            minlag = lags[i];
            maxlag = oldlag;
        }
        else
        {
            minlag = oldlag;
            maxlag = lags[i];
        }

        //temp = sub (maxlag, minlag);
        temp = SUB (maxlag, minlag);


        //if (sub (temp, LTHRESH) < 0)
        if (SUB (temp, LTHRESH) < 0)
        {
            //lagcount = add (lagcount, 1);
            lagcount = ADD (lagcount, 1);
        }
        /*** Save the current LTP lag ***/

        oldlag = lags[i];
    }

    /*** Update the veryoldlagcount and oldlagcount ***/

    veryoldlagcount = oldlagcount;

    oldlagcount = lagcount;

    /*** Make ptch decision ready for next frame ***/

    //temp = add (oldlagcount, veryoldlagcount);
    temp = ADD (oldlagcount, veryoldlagcount);


    //if (sub (temp, NTHRESH) >= 0)
    if (SUB (temp, NTHRESH) >= 0)
    {
        *ptch_flag = 1;
    }
    else
    {
        *ptch_flag = 0;
    }

    return;
}
































































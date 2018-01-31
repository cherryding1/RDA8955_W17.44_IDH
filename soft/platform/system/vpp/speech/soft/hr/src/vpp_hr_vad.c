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







#include "vpp_hr_typedefs.h"
#include "vpp_hr_mathhalf.h"
#include "vpp_hr_mathdp31.h"
#include "vpp_hr_vad.h"
#include "vpp_hr_mathhalf_macro.h"



//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



/*_________________________________________________________________________
 |                                                                         |
 |                              Local Defines                              |
 |_________________________________________________________________________|
*/

/*** Floating point representations of constants pth, plev and margin ***/

#define M_PTH    26250
#define E_PTH    18
#define M_PLEV   17500
#define E_PLEV   20
#define M_MARGIN 27343
#define E_MARGIN 27

/*_________________________________________________________________________
 |                                                                         |
 |                            Static Variables                             |
 |_________________________________________________________________________|
*/

static INT16
pswRvad[9],
        swNormRvad,
        swPt_sacf,
        swPt_sav0,
        swE_thvad,
        swM_thvad,
        swAdaptCount,
        swBurstCount,
        swHangCount,
        swOldLagCount,
        swVeryOldLagCount,
        swOldLag;

static INT32
pL_sacf[27],
        pL_sav0[36],
        L_lastdm;

/****************************************************************************
 *
 *     FUNCTION:  vad_reset
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Resets VAD static variables to their initial value.
 *
 ***************************************************************************/

void   vad_reset(void)

{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    int    i;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    pswRvad[0] = 24576;
    swNormRvad = 7;
    swPt_sacf = 0;
    swPt_sav0 = 0;
    L_lastdm = 0;
    swE_thvad = 21;
    swM_thvad = 21875;
    swAdaptCount = 0;
    swBurstCount = 0;
    swHangCount = -1;
    swOldLagCount = 0;
    swVeryOldLagCount = 0;
    swOldLag = 21;

    for (i = 1; i < 9; i++)
        pswRvad[i] = 0;
    for (i = 0; i < 27; i++)
        pL_sacf[i] = 0;
    for (i = 0; i < 36; i++)
        pL_sav0[i] = 0;

}

/****************************************************************************
 *
 *     FUNCTION:  vad_algorithm
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Returns a decision as to whether the current frame being
 *                processed by the speech encoder contains speech or not.
 *
 *     INPUTS:    pL_acf[0..8]  autocorrelation of input signal frame
 *                swScaleAcf    L_acf scaling factor
 *                pswRc[0..3]   speech encoder reflection coefficients
 *                swPtch        flag to indicate a periodic signal component
 *
 *     OUTPUTS:   pswVadFlag    vad decision
 *
 ***************************************************************************/

void   vad_algorithm(INT32 pL_acf[9],
                     INT16 swScaleAcf,
                     INT16 pswRc[4],
                     INT16 swPtch,
                     INT16 *pswVadFlag)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    pL_av0[9],
           pL_av1[9];

    INT16
    swM_acf0,
    swE_acf0,
    pswRav1[9],
    swNormRav1,
    swM_pvad,
    swE_pvad,
    swStat,
    swTone,
    swVvad;

    VPP_HR_PROFILE_FUNCTION_ENTER(vad_algorithm );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    energy_computation
    (
        pL_acf, swScaleAcf,
        pswRvad, swNormRvad,
        &swM_pvad, &swE_pvad,
        &swM_acf0, &swE_acf0
    );

    average_acf
    (
        pL_acf, swScaleAcf,
        pL_av0, pL_av1
    );

    predictor_values
    (
        pL_av1,
        pswRav1,
        &swNormRav1
    );

    spectral_comparison
    (
        pswRav1, swNormRav1,
        pL_av0,
        &swStat
    );

    tone_detection
    (
        pswRc,
        &swTone
    );

    threshold_adaptation
    (
        swStat, swPtch, swTone,
        pswRav1, swNormRav1,
        swM_pvad, swE_pvad,
        swM_acf0, swE_acf0,
        pswRvad, &swNormRvad,
        &swM_thvad, &swE_thvad
    );

    vad_decision
    (
        swM_pvad, swE_pvad,
        swM_thvad, swE_thvad,
        &swVvad
    );

    vad_hangover
    (
        swVvad,
        pswVadFlag
    );

    VPP_HR_PROFILE_FUNCTION_EXIT( vad_algorithm);
}

/****************************************************************************
 *
 *     FUNCTION:  energy_computation
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the input and residual energies of the adaptive
 *                filter in a floating point representation.
 *
 *     INPUTS:    pL_acf[0..8]   autocorrelation of input signal frame
 *                swScaleAcf     L_acf scaling factor
 *                pswRvad[0..8]  autocorrelated adaptive filter coefficients
 *                swNormRvad     rvad scaling factor
 *
 *     OUTPUTS:   pswM_pvad      mantissa of filtered signal energy
 *                pswE_pvad      exponent of filtered signal energy
 *                pswM_acf0      mantissa of signal frame energy
 *                pswE_acf0      exponent of signal frame energy
 *
 ***************************************************************************/

void   energy_computation(INT32 pL_acf[],
                          INT16 swScaleAcf,
                          INT16 pswRvadm[],
                          INT16 swNormRvadm,
                          INT16 *pswM_pvad,
                          INT16 *pswE_pvad,
                          INT16 *pswM_acf0,
                          INT16 *pswE_acf0)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    L_temp;

    INT16
    pswSacf[9],
            swNormAcf,
            swNormProd,
            swShift;

    int i;


    register INT32  s_hi=0;
    register UINT32 s_lo=0;
//  INT32 essai0, essai,swShift0;

    // VPP_HR_PROFILE_FUNCTION_ENTER(energy_computation );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** Test if acf[0] is zero ***/

    if (pL_acf[0] == 0)
    {
        *pswE_pvad = -0x8000;
        *pswM_pvad = 0;
        *pswE_acf0 = -0x8000;
        *pswM_acf0 = 0;

        //VPP_HR_PROFILE_FUNCTION_EXIT(energy_computation );
        return;
    }


    /*** Re-normalisation of L_acf[0..8] ***/

    swNormAcf = norm_l(pL_acf[0]);
    swShift = SUB(swNormAcf, 3);

    /*for (i = 0; i <= 8; i++)
    {
      //pswSacf[i] = EXTRACT_H(L_shl(pL_acf[i], swShift));
      pswSacf[i] = EXTRACT_H(L_SHL_SAT(pL_acf[i], swShift));


    }*/

    pswSacf[0] = EXTRACT_H(L_SHL_SAT(pL_acf[0], swShift));
    pswSacf[1] = EXTRACT_H(L_SHL_SAT(pL_acf[1], swShift));
    pswSacf[2] = EXTRACT_H(L_SHL_SAT(pL_acf[2], swShift));
    pswSacf[3] = EXTRACT_H(L_SHL_SAT(pL_acf[3], swShift));
    pswSacf[4] = EXTRACT_H(L_SHL_SAT(pL_acf[4], swShift));
    pswSacf[5] = EXTRACT_H(L_SHL_SAT(pL_acf[5], swShift));
    pswSacf[6] = EXTRACT_H(L_SHL_SAT(pL_acf[6], swShift));
    pswSacf[7] = EXTRACT_H(L_SHL_SAT(pL_acf[7], swShift));
    pswSacf[8] = EXTRACT_H(L_SHL_SAT(pL_acf[8], swShift));

    /*** Computation of e_acf0 and m_acf0 ***/

    //*pswE_acf0 = ADD_SAT16(32, shl(swScaleAcf, 1));
    *pswE_acf0 = ADD(32, SHL(swScaleAcf, 1));
    *pswE_acf0 -=swNormAcf;// sub(*pswE_acf0, swNormAcf);
    *pswM_acf0 = SHL(pswSacf[0], 3);


    /*** Computation of e_pvad and m_pvad ***/

    //*pswE_pvad = ADD_SAT16(*pswE_acf0, 14);
    *pswE_pvad = ADD(*pswE_acf0, 14);
    *pswE_pvad -=swNormRvadm;// sub(*pswE_pvad, swNormRvadm);

    L_temp = 0;
    s_lo=0;
    for (i = 1; i <= 8; i++)
        //L_temp = L_mac(L_temp, pswSacf[i], pswRvadm[i]);
        VPP_MLA16(s_hi,s_lo, pswSacf[i], pswRvadm[i]);

    /*VPP_MLA16(s_hi,s_lo, pswSacf[1], pswRvadm[1]);
    VPP_MLA16(s_hi,s_lo, pswSacf[2], pswRvadm[2]);
    VPP_MLA16(s_hi,s_lo, pswSacf[3], pswRvadm[3]);
    VPP_MLA16(s_hi,s_lo, pswSacf[4], pswRvadm[4]);
    VPP_MLA16(s_hi,s_lo, pswSacf[5], pswRvadm[5]);
    VPP_MLA16(s_hi,s_lo, pswSacf[6], pswRvadm[6]);
    VPP_MLA16(s_hi,s_lo, pswSacf[7], pswRvadm[7]);
    VPP_MLA16(s_hi,s_lo, pswSacf[8], pswRvadm[8]);*/


    L_temp = L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);
    //L_temp = L_ADD(L_temp, L_shr(L_MULT(pswSacf[0], pswRvadm[0]), 1));
    //L_temp = L_ADD(L_temp, SHR(L_MULT(pswSacf[0], pswRvadm[0]), 1));
    L_temp +=   SHR(L_MULT(pswSacf[0], pswRvadm[0]), 1) ;

    if (L_temp <= 0)
        L_temp = 1;

    swNormProd = norm_l(L_temp);
    *pswE_pvad -=swNormProd;// sub(*pswE_pvad, swNormProd);
    //*pswM_pvad = EXTRACT_H(L_shl(L_temp, swNormProd));
    //*pswM_pvad = EXTRACT_H(L_SHL_SAT(L_temp, swNormProd));
    *pswM_pvad = EXTRACT_H( SHL (L_temp, swNormProd));

    // VPP_HR_PROFILE_FUNCTION_EXIT(energy_computation );


}

/****************************************************************************
 *
 *     FUNCTION:  average_acf
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the arrays L_av0 [0..8] and L_av1 [0..8].
 *
 *     INPUTS:    pL_acf[0..8]  autocorrelation of input signal frame
 *                swScaleAcf    L_acf scaling factor
 *
 *     OUTPUTS:   pL_av0[0..8]  ACF averaged over last four frames
 *                pL_av1[0..8]  ACF averaged over previous four frames
 *
 ***************************************************************************/

void   average_acf(INT32 pL_acf[],
                   INT16 swScaleAcf,
                   INT32 pL_av0[],
                   INT32 pL_av1[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_temp;

    INT16 swScale;

    int    i;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** computation of the scaleing factor ***/

    swScale = SUB(10, SHL(swScaleAcf, 1));


    /*** Computation of the arrays L_av0 and L_av1 ***/

    for (i = 0; i <= 8; i++)
    {
        //L_temp = L_shr(pL_acf[i], swScale);
        L_temp = L_SHR_V(pL_acf[i], swScale);
        /*pL_av0[i] = L_ADD(pL_sacf[i], L_temp);
        pL_av0[i] = L_ADD(pL_sacf[i + 9], pL_av0[i]);
        pL_av0[i] = L_ADD(pL_sacf[i + 18], pL_av0[i]);*/
        pL_av0[i] = ADD(pL_sacf[i], L_temp);
        pL_av0[i] = ADD(pL_sacf[i + 9], pL_av0[i]);
        pL_av0[i] = ADD(pL_sacf[i + 18], pL_av0[i]);
        pL_sacf[swPt_sacf + i] = L_temp;
        pL_av1[i] = pL_sav0[swPt_sav0 + i];
        pL_sav0[swPt_sav0 + i] = pL_av0[i];
    }


    /*** Update the array pointers ***/

    if (swPt_sacf == 18)
        swPt_sacf = 0;
    else
        //swPt_sacf = ADD_SAT16(swPt_sacf, 9);
        swPt_sacf += 9;

    if (swPt_sav0 == 27)
        swPt_sav0 = 0;
    else
        //swPt_sav0 = ADD_SAT16(swPt_sav0, 9);
        swPt_sav0 += 9;

}

/****************************************************************************
 *
 *     FUNCTION:  predictor_values
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the array rav [0..8] needed for the spectral
 *                comparison and the threshold adaptation.
 *
 *     INPUTS:    pL_av1 [0..8]  ACF averaged over previous four frames
 *
 *     OUTPUTS:   pswRav1 [0..8] ACF obtained from L_av1
 *                pswNormRav1    r_av1 scaling factor
 *
 ***************************************************************************/

void   predictor_values(INT32 pL_av1[],
                        INT16 pswRav1[],
                        INT16 *pswNormRav1)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16
    pswVpar[8],
            pswAav1[9];

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    schur_recursion(pL_av1, pswVpar);
    step_up(8, pswVpar, pswAav1);
    compute_rav1(pswAav1, pswRav1, pswNormRav1);

}

/****************************************************************************
 *
 *     FUNCTION:  schur_recursion
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Uses the Schur recursion to compute adaptive filter
 *                reflection coefficients from an autorrelation function.
 *
 *     INPUTS:    pL_av1[0..8]   autocorrelation function
 *
 *     OUTPUTS:   pswVpar[0..7]  reflection coefficients
 *
 ***************************************************************************/

void   schur_recursion(INT32 pL_av1[],
                       INT16 pswVpar[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16
    pswAcf[9],
           pswPp[9],
           pswKk[9],
           swTemp;

    int    i,
           //k,
           m,
           n;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** Schur recursion with 16-bit arithmetic ***/

    if (pL_av1[0] == 0)
    {
        for (i = 0; i < 8; i++)
            pswVpar[i] = 0;
        /*pswVpar[0] = 0;
        pswVpar[1] = 0;
        pswVpar[2] = 0;
        pswVpar[3] = 0;
        pswVpar[4] = 0;
        pswVpar[5] = 0;
        pswVpar[6] = 0;
        pswVpar[7] = 0;*/


        return;
    }

    swTemp = norm_l(pL_av1[0]);

    /*for (k = 0; k <= 8; k++)
      //pswAcf[k] = EXTRACT_H(L_shl(pL_av1[k], swTemp));
      //pswAcf[k] = EXTRACT_H(L_SHL_SAT(pL_av1[k], swTemp));
      pswAcf[k] = EXTRACT_H( SHL (pL_av1[k], swTemp));*/

    pswAcf[0] = EXTRACT_H(SHL(pL_av1[0], swTemp));
    pswAcf[1] = EXTRACT_H(SHL(pL_av1[1], swTemp));
    pswAcf[2] = EXTRACT_H(SHL(pL_av1[2], swTemp));
    pswAcf[3] = EXTRACT_H(SHL(pL_av1[3], swTemp));
    pswAcf[4] = EXTRACT_H(SHL(pL_av1[4], swTemp));
    pswAcf[5] = EXTRACT_H(SHL(pL_av1[5], swTemp));
    pswAcf[6] = EXTRACT_H(SHL(pL_av1[6], swTemp));
    pswAcf[7] = EXTRACT_H(SHL(pL_av1[7], swTemp));
    pswAcf[8] = EXTRACT_H(SHL(pL_av1[8], swTemp));

    /*** Initialise array pp[..] and kk[..] for the recursion: ***/

    /*for (i = 1; i <= 7; i++)
       pswKk[9 - i] = pswAcf[i];*/

    pswKk[9 - 1] = pswAcf[1];
    pswKk[9 - 2] = pswAcf[2];
    pswKk[9 - 3] = pswAcf[3];
    pswKk[9 - 4] = pswAcf[4];
    pswKk[9 - 5] = pswAcf[5];
    pswKk[9 - 6] = pswAcf[6];
    pswKk[9 - 7] = pswAcf[7];


    /*for (i = 0; i <= 8; i++)
      pswPp[i] = pswAcf[i];*/

    pswPp[1] = pswAcf[1];
    pswPp[2] = pswAcf[2];
    pswPp[3] = pswAcf[3];
    pswPp[4] = pswAcf[4];
    pswPp[5] = pswAcf[5];
    pswPp[6] = pswAcf[6];
    pswPp[7] = pswAcf[7];


    /*** Compute Parcor coefficients: ***/

    for (n = 0; n < 8; n++)
    {
        //if (pswPp[0] < abs_s(pswPp[1]))
        if (pswPp[0] < ABS_S(pswPp[1]))
        {
            for (i = n; i < 8; i++)
                pswVpar[i] = 0;
            return;
        }
        //pswVpar[n] = divide_s(abs_s(pswPp[1]), pswPp[0]);
        pswVpar[n] = DIVIDE_SAT16(ABS_S(pswPp[1]), pswPp[0]);

        if (pswPp[1] > 0)
            pswVpar[n] = NEGATE(pswVpar[n]);
        if (n == 7)
            return;


        /*** Schur recursion: ***/

        //pswPp[0] = ADD_SAT16(pswPp[0], mult_r(pswPp[1], pswVpar[n]));
        pswPp[0] = ADD(pswPp[0], (MULT_R(pswPp[1], pswVpar[n])));

        for (m = 1; m <= (7 - n); m++)
        {
            //pswPp[m] = ADD_SAT16(pswPp[1 + m], mult_r(pswKk[9 - m], pswVpar[n]));
            //pswKk[9 - m] = ADD_SAT16(pswKk[9 - m], mult_r(pswPp[1 + m], pswVpar[n]));
            pswPp[m]     = ADD(pswPp[1 + m], (MULT_R(pswKk[9 - m], pswVpar[n])));
            pswKk[9 - m] = ADD(pswKk[9 - m], (MULT_R(pswPp[1 + m], pswVpar[n])));

        }
    }

}

/****************************************************************************
 *
 *     FUNCTION:  step_up
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the transversal filter coefficients from the
 *                reflection coefficients.
 *
 *     INPUTS:    swNp             filter order (2..8)
 *                pswVpar[0..np-1] reflection coefficients
 *
 *     OUTPUTS:   pswAav1[0..np]   transversal filter coefficients
 *
 ***************************************************************************/

void   step_up(INT16 swNp,
               INT16 pswVpar[],
               INT16 pswAav1[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    pL_coef[9],
            pL_work[9];

    INT16
    swTemp;

    int
    i,
    m;


    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** Initialisation of the step-up recursion ***/

    //pL_coef[0] = L_shl(0x4000L, 15);
    //pL_coef[1] = L_shl(L_DEPOSIT_L(pswVpar[0]), 14);
    //pL_coef[0] = L_SHL_SAT(0x4000L, 15);
    //pL_coef[1] = L_SHL_SAT(L_DEPOSIT_L(pswVpar[0]), 14);
    pL_coef[0] = SHL(0x4000L, 15);
    pL_coef[1] = SHL(L_DEPOSIT_L(pswVpar[0]), 14);


    /*** Loop on the LPC analysis order: ***/

    for (m = 2; m <= swNp; m++)
    {
        for (i = 1; i < m; i++)
        {
            swTemp = EXTRACT_H(pL_coef[m - i]);
            s_lo=SHR(pL_coef[i] ,1);
            //pL_work[i] = L_mac(pL_coef[i], pswVpar[m - 1], swTemp);
            VPP_MLA16(s_hi,s_lo, pswVpar[m - 1], swTemp);
            pL_work[i] = L_MLA_SAT_CARRY(s_lo, pL_coef[i]&1);// L_MLA_SAT(s_hi,s_lo);
        }
        for (i = 1; i < m; i++)
            pL_coef[i] = pL_work[i];
        //pL_coef[m] = L_shl(L_DEPOSIT_L(pswVpar[m - 1]), 14);
        //pL_coef[m] = L_SHL_SAT(L_DEPOSIT_L(pswVpar[m - 1]), 14);
        pL_coef[m] = SHL(L_DEPOSIT_L(pswVpar[m - 1]), 14);

    }


    /*** Keep the aav1[0..swNp] in 15 bits for the following subclause ***/

    for (i = 0; i <= swNp; i++)
        //pswAav1[i] = EXTRACT_H(L_shr(pL_coef[i], 3));
        pswAav1[i] = EXTRACT_H(SHR(pL_coef[i], 3));

}

/****************************************************************************
 *
 *     FUNCTION:  compute_rav1
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the autocorrelation function of the adaptive
 *                filter coefficients.
 *
 *     INPUTS:    pswAav1[0..8]  adaptive filter coefficients
 *
 *     OUTPUTS:   pswRav1[0..8]  ACF of aav1
 *                pswNormRav1    r_av1 scaling factor
 *
 ***************************************************************************/

void   compute_rav1(INT16 pswAav1[],
                    INT16 pswRav1[],
                    INT16 *pswNormRav1)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    pL_work[9];

    int
    i,
    k;


    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** Computation of the rav1[0..8] ***/

    for (i = 0; i <= 8; i++)
    {
        pL_work[i] = 0;
        s_lo=0;
        for (k = 0; k <= 8 - i; k++)
            //pL_work[i] = L_mac(pL_work[i], pswAav1[k], pswAav1[k + i]);
            VPP_MLA16(s_hi,s_lo, pswAav1[k], pswAav1[k + i]);
        pL_work[i] = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
    }

    if (pL_work[0] == 0)
        *pswNormRav1 = 0;
    else
        *pswNormRav1 = norm_l(pL_work[0]);

    /*for (i = 0; i <= 8; i++)
      pswRav1[i] = EXTRACT_H(L_shl(pL_work[i], *pswNormRav1));
      pswRav1[i] = EXTRACT_H(L_SHL_SAT(pL_work[i], *pswNormRav1));*/
    pswRav1[0] = EXTRACT_H(SHL(pL_work[0], *pswNormRav1));
    pswRav1[1] = EXTRACT_H(SHL(pL_work[1], *pswNormRav1));
    pswRav1[2] = EXTRACT_H(SHL(pL_work[2], *pswNormRav1));
    pswRav1[3] = EXTRACT_H(SHL(pL_work[3], *pswNormRav1));
    pswRav1[4] = EXTRACT_H(SHL(pL_work[4], *pswNormRav1));
    pswRav1[5] = EXTRACT_H(SHL(pL_work[5], *pswNormRav1));
    pswRav1[6] = EXTRACT_H(SHL(pL_work[6], *pswNormRav1));
    pswRav1[7] = EXTRACT_H(SHL(pL_work[7], *pswNormRav1));
    pswRav1[8] = EXTRACT_H(SHL(pL_work[8], *pswNormRav1));

}

/****************************************************************************
 *
 *     FUNCTION:  spectral_comparison
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the stat flag needed for the threshold
 *                adaptation decision.
 *
 *     INPUTS:    pswRav1[0..8]   ACF obtained from L_av1
 *                swNormRav1      rav1 scaling factor
 *                pL_av0[0..8]    ACF averaged over last four frames
 *
 *     OUTPUTS:   pswStat         flag to indicate spectral stationarity
 *
 ***************************************************************************/

void   spectral_comparison(INT16 pswRav1[],
                           INT16 swNormRav1,
                           INT32 pL_av0[],
                           INT16 *pswStat)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    L_dm,
    L_sump,
    L_temp;

    INT16
    pswSav0[9],
            swShift,
            swDivShift,
            swTemp;

//  int i;


    register INT32  s_hi=0;
    register UINT32 s_lo=0;



    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** Re-normalise L_av0[0..8] ***/

    if (pL_av0[0] == 0)
    {
        /*for (i = 0; i <= 8; i++)
          pswSav0[i] = 4095;*/
        pswSav0[0] = 4095;
        pswSav0[1] = 4095;
        pswSav0[2] = 4095;
        pswSav0[3] = 4095;
        pswSav0[4] = 4095;
        pswSav0[5] = 4095;
        pswSav0[6] = 4095;
        pswSav0[7] = 4095;
        pswSav0[8] = 4095;

    }
    else
    {
        swShift =SUB(norm_l(pL_av0[0]), 3);
        /*for (i = 0; i <= 8; i++)
          //pswSav0[i] = EXTRACT_H(L_shl(pL_av0[i], swShift));
          pswSav0[i] = EXTRACT_H( L_SHL_SAT (pL_av0[i], swShift));*/
        pswSav0[0] = EXTRACT_H(SHL(pL_av0[0], swShift));
        pswSav0[1] = EXTRACT_H(SHL(pL_av0[1], swShift));
        pswSav0[2] = EXTRACT_H(SHL(pL_av0[2], swShift));
        pswSav0[3] = EXTRACT_H(SHL(pL_av0[3], swShift));
        pswSav0[4] = EXTRACT_H(SHL(pL_av0[4], swShift));
        pswSav0[5] = EXTRACT_H(SHL(pL_av0[5], swShift));
        pswSav0[6] = EXTRACT_H(SHL(pL_av0[6], swShift));
        pswSav0[7] = EXTRACT_H(SHL(pL_av0[7], swShift));
        pswSav0[8] = EXTRACT_H(SHL(pL_av0[8], swShift));/**/

    }

    /*** compute partial sum of dm ***/

    //L_sump = 0;
    s_lo=0;
    //for (i = 1; i <= 8; i++)
    //L_sump = L_mac(L_sump, pswRav1[i], pswSav0[i]);
    //VPP_MLA16(s_hi,s_lo, pswRav1[i], pswSav0[i]);

    VPP_MLA16(s_hi,s_lo, pswRav1[1], pswSav0[1]);
    VPP_MLA16(s_hi,s_lo, pswRav1[2], pswSav0[2]);
    VPP_MLA16(s_hi,s_lo, pswRav1[3], pswSav0[3]);
    VPP_MLA16(s_hi,s_lo, pswRav1[4], pswSav0[4]);
    VPP_MLA16(s_hi,s_lo, pswRav1[5], pswSav0[5]);
    VPP_MLA16(s_hi,s_lo, pswRav1[6], pswSav0[6]);
    VPP_MLA16(s_hi,s_lo, pswRav1[7], pswSav0[7]);
    VPP_MLA16(s_hi,s_lo, pswRav1[8], pswSav0[8]);/**/


    L_sump = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
    /*** compute the division of the partial sum by sav0[0] ***/

    if (L_sump < 0)
        L_temp = L_NEGATE(L_sump);
    else
        L_temp = L_sump;

    if (L_temp == 0)
    {
        L_dm = 0;
        swShift = 0;
    }
    else
    {
        pswSav0[0] = SHL(pswSav0[0], 3);
        swShift = norm_l(L_temp);
        //swTemp = EXTRACT_H(L_shl(L_temp, swShift));
        //swTemp = EXTRACT_H(L_SHL_SAT(L_temp, swShift));
        swTemp = EXTRACT_H( SHL (L_temp, swShift));

        if (pswSav0[0] >= swTemp)
        {
            swDivShift = 0;
            //swTemp = divide_s(swTemp, pswSav0[0]);
            swTemp = DIVIDE_SAT16(swTemp, pswSav0[0]);

        }
        else
        {
            swDivShift = 1;
            swTemp = SUB(swTemp, pswSav0[0]);
            //swTemp = divide_s(swTemp, pswSav0[0]);
            swTemp = DIVIDE_SAT16(swTemp, pswSav0[0]);
        }

        if (swDivShift == 1)
            L_dm = 0x8000L;
        else
            L_dm = 0;

        //L_dm = L_shl((L_ADD(L_dm, L_DEPOSIT_L(swTemp))), 1);
        //L_dm = L_SHL_SAT((L_ADD(L_dm, L_DEPOSIT_L(swTemp))), 1);
        //L_dm =  SHL ((L_ADD(L_dm, L_DEPOSIT_L(swTemp))), 1);
        L_dm =  SHL ((ADD(L_dm, L_DEPOSIT_L(swTemp))), 1);

        if (L_sump < 0)
            //L_dm = L_SUB(0L, L_dm);
            //L_dm =  SUB(0L, L_dm);
            L_dm =  NEGATE( L_dm);

    }


    /*** Re-normalisation and final computation of L_dm ***/

    //L_dm = L_shl(L_dm, 14);
    //L_dm = L_SHL_SAT(L_dm, 14);
    L_dm <<=  14 ;
    //L_dm = L_shr(L_dm, swShift);
    //L_dm = SHR(L_dm, swShift);
    L_dm >>=  swShift ;
    //L_dm = L_ADD(L_dm, L_shl(L_DEPOSIT_L(pswRav1[0]), 11));
    //L_dm = L_ADD(L_dm, SHL(L_DEPOSIT_L(pswRav1[0]), 11));
    L_dm =  ADD(L_dm, SHL(L_DEPOSIT_L(pswRav1[0]), 11));
    //L_dm = L_shr(L_dm, swNormRav1);
    //L_dm = L_SHR_V(L_dm, swNormRav1);
    L_dm >>=  swNormRav1 ;


    /*** Compute the difference and save L_dm ***/

    //L_temp = L_SUB(L_dm, L_lastdm);
    L_temp =  SUB(L_dm, L_lastdm);
    L_lastdm = L_dm;

    if (L_temp < 0L)
        L_temp = L_NEGATE(L_temp);


    /*** Evaluation of the state flag  ***/

    //L_temp = L_SUB(L_temp, 4456L);
    L_temp -=   4456L ;

    if (L_temp < 0)
        *pswStat = 1;
    else
        *pswStat = 0;

}

/****************************************************************************
 *
 *     FUNCTION:  tone_detection
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the tone flag needed for the threshold
 *                adaptation decision.
 *
 *     INPUTS:    pswRc[0..3] reflection coefficients calculated in the
 *                            speech encoder short term predictor
 *
 *     OUTPUTS:   pswTone     flag to indicate a periodic signal component
 *
 ***************************************************************************/

void   tone_detection(INT16 pswRc[4],
                      INT16 *pswTone)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    L_num,
    L_den,
    L_temp;

    INT16
    swTemp,
    swPredErr,
    pswA[3];

//  int
//         i;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;



    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    *pswTone = 0;


    /*** Calculate filter coefficients ***/

    step_up(2, pswRc, pswA);


    /*** Calculate ( a[1] * a[1] ) ***/

    swTemp = SHL(pswA[1], 3);
    L_den = L_MULT(swTemp, swTemp);


    /*** Calculate ( 4*a[2] - a[1]*a[1] ) ***/

    //L_temp = L_shl(L_DEPOSIT_H(pswA[2]), 3);
    //L_temp = L_SHL_SAT(L_DEPOSIT_H(pswA[2]), 3);
    L_temp =  SHL (L_DEPOSIT_H(pswA[2]), 3);
    //L_num = L_SUB(L_temp, L_den);
    L_num =  SUB(L_temp, L_den);


    /*** Check if pole frequency is less than 385 Hz ***/

    if (L_num <= 0)
        return;

    if (pswA[1] < 0)
    {
        swTemp = EXTRACT_H(L_den);
        s_lo=SHR( L_num,1);
        //L_temp = L_msu(L_num, swTemp, 3189);
        VPP_MLA16(s_hi,s_lo, -swTemp, 3189);
        L_temp = L_MLA_SAT_CARRY(s_lo, L_num&1);// L_MLA_SAT(s_hi,s_lo);

        if (L_temp < 0)
            return;
    }


    /*** Calculate normalised prediction error ***/

    swPredErr = 0x7fff;

    /*for (i = 0; i < 4; i++)
    {
      swTemp = MULT(pswRc[i], pswRc[i]);
      swTemp = SUB(32767, swTemp);
      swPredErr = MULT(swPredErr, swTemp);
    }*/
    swTemp = MULT(pswRc[0], pswRc[0]);
    swTemp = SUB(32767, swTemp);
    swPredErr = MULT(swPredErr, swTemp);
    swTemp = MULT(pswRc[1], pswRc[1]);
    swTemp = SUB(32767, swTemp);
    swPredErr = MULT(swPredErr, swTemp);
    swTemp = MULT(pswRc[2], pswRc[2]);
    swTemp = SUB(32767, swTemp);
    swPredErr = MULT(swPredErr, swTemp);
    swTemp = MULT(pswRc[3], pswRc[3]);
    swTemp = SUB(32767, swTemp);
    swPredErr = MULT(swPredErr, swTemp);




    /*** Test if prediction error is smaller than threshold ***/

    swTemp = SUB(swPredErr, 1464);

    if (swTemp < 0)
        *pswTone = 1;

}

/****************************************************************************
 *
 *     FUNCTION:  threshold_adaptation
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Evaluates the secondary VAD decision.  If speech is not
 *                present then the noise model rvad and adaptive threshold
 *                thvad are updated.
 *
 *     INPUTS:    swStat        flag to indicate spectral stationarity
 *                swPtch        flag to indicate a periodic signal component
 *                swTone        flag to indicate a tone signal component
 *                pswRav1[0..8] ACF obtained from l_av1
 *                swNormRav1    r_av1 scaling factor
 *                swM_pvad      mantissa of filtered signal energy
 *                swE_pvad      exponent of filtered signal energy
 *                swM_acf0      mantissa of signal frame energy
 *                swE_acf0      exponent of signal frame energy
 *
 *     OUTPUTS:   pswRvad[0..8] autocorrelated adaptive filter coefficients
 *                pswNormRvad   rvad scaling factor
 *                pswM_thvad    mantissa of decision threshold
 *                pswE_thvad    exponent of decision threshold
 *
 ***************************************************************************/

void   threshold_adaptation(INT16 swStat,
                            INT16 swPtch,
                            INT16 swTone,
                            INT16 pswRav1[],
                            INT16 swNormRav1,
                            INT16 swM_pvad,
                            INT16 swE_pvad,
                            INT16 swM_acf0,
                            INT16 swE_acf0,
                            INT16 pswRvadm[],
                            INT16 *pswNormRvad,
                            INT16 *pswM_thvad,
                            INT16 *pswE_thvad)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32
    L_temp;

    INT16
    swTemp,
    swComp,
    swComp2,
    swM_temp,
    swE_temp;

    int
    i;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    swComp = 0;

    /*** Test if acf0 < pth; if yes set thvad to plev ***/

    if (swE_acf0 < E_PTH)
        swComp = 1;
    if ((swE_acf0 == E_PTH) && (swM_acf0 < M_PTH))
        swComp = 1;

    if (swComp == 1)
    {
        *pswE_thvad = E_PLEV;
        *pswM_thvad = M_PLEV;

        return;
    }


    /*** Test if an adaption is required ***/

    if (swPtch == 1)
        swComp = 1;
    if (swStat == 0)
        swComp = 1;
    if (swTone == 1)
        swComp = 1;

    if (swComp == 1)
    {
        swAdaptCount = 0;
        return;
    }


    /*** Increment adaptcount ***/

    //swAdaptCount = ADD_SAT16(swAdaptCount, 1);
    swAdaptCount ++;
    if (swAdaptCount <= 8)
        return;


    /*** computation of thvad-(thvad/dec) ***/

    //*pswM_thvad = SUB(*pswM_thvad, shr(*pswM_thvad, 5));
    *pswM_thvad = SUB(*pswM_thvad, L_SHR_V(*pswM_thvad, 5));

    if (*pswM_thvad < 0x4000)
    {
        (*pswM_thvad) <<=1;// SHL(*pswM_thvad, 1);
        (*pswE_thvad)-- ;//= SUB(*pswE_thvad, 1);
    }


    /*** computation of pvad*fac ***/

    L_temp = L_MULT(swM_pvad, 20889);
    //L_temp = L_shr(L_temp, 15);
    L_temp >>=15;// SHR(L_temp, 15);
    //swE_temp = ADD_SAT16(swE_pvad, 1);
    swE_temp = ADD(swE_pvad, 1);

    if (L_temp > 0x7fffL)
    {
        //L_temp = L_shr(L_temp, 1);
        L_temp >>=1;// SHR(L_temp, 1);
        //swE_temp = ADD_SAT16(swE_temp, 1);
        swE_temp++;

    }
    swM_temp = EXTRACT_L(L_temp);


    /*** test if thvad < pavd*fac ***/

    if (*pswE_thvad < swE_temp)
        swComp = 1;

    if ((*pswE_thvad == swE_temp) && (*pswM_thvad < swM_temp))
        swComp = 1;


    /*** compute minimum(thvad+(thvad/inc), pvad*fac) when comp = 1 ***/

    if (swComp == 1)
    {

        /*** compute thvad + (thvad/inc) ***/

        //L_temp = L_ADD(L_DEPOSIT_L(*pswM_thvad),L_DEPOSIT_L(shr(*pswM_thvad, 4)));
        //L_temp = L_ADD(L_DEPOSIT_L(*pswM_thvad),L_DEPOSIT_L(SHR(*pswM_thvad, 4)));
        L_temp =  ADD(L_DEPOSIT_L(*pswM_thvad),L_DEPOSIT_L(SHR(*pswM_thvad, 4)));

        if (L_temp > 0x7fffL)
        {
            //*pswM_thvad = EXTRACT_L(L_shr(L_temp, 1));
            *pswM_thvad = EXTRACT_L(SHR(L_temp, 1));
            //*pswE_thvad = ADD_SAT16(*pswE_thvad, 1);
            (*pswE_thvad)++;

        }
        else
            *pswM_thvad = EXTRACT_L(L_temp);

        swComp2 = 0;

        if (swE_temp < *pswE_thvad)
            swComp2 = 1;

        if ((swE_temp == *pswE_thvad) && (swM_temp < *pswM_thvad))
            swComp2 = 1;

        if (swComp2 == 1)
        {
            *pswE_thvad = swE_temp;
            *pswM_thvad = swM_temp;
        }
    }


    /*** compute pvad + margin ***/

    if (swE_pvad == E_MARGIN)
    {
        //L_temp = L_ADD(L_DEPOSIT_L(swM_pvad), L_DEPOSIT_L(M_MARGIN));
        L_temp =  ADD(L_DEPOSIT_L(swM_pvad), L_DEPOSIT_L(M_MARGIN));
        //swM_temp = EXTRACT_L(L_shr(L_temp, 1));
        swM_temp = EXTRACT_L(SHR(L_temp, 1));
        //swE_temp = ADD_SAT16(swE_pvad, 1);
        swE_temp = ADD(swE_pvad, 1);

    }
    else
    {
        if (swE_pvad > E_MARGIN)
        {
            swTemp = SUB(swE_pvad, E_MARGIN);
            //swTemp = shr(M_MARGIN, swTemp);
            swTemp = L_SHR_V(M_MARGIN, swTemp);
            //L_temp = L_ADD(L_DEPOSIT_L(swM_pvad), L_DEPOSIT_L(swTemp));
            L_temp =  ADD(L_DEPOSIT_L(swM_pvad), L_DEPOSIT_L(swTemp));

            if (L_temp > 0x7fffL)
            {
                //swE_temp = ADD_SAT16(swE_pvad, 1);
                swE_temp = ADD(swE_pvad, 1);
                //swM_temp = EXTRACT_L(L_shr(L_temp, 1));
                swM_temp = EXTRACT_L(SHR(L_temp, 1));

            }
            else
            {
                swE_temp = swE_pvad;
                swM_temp = EXTRACT_L(L_temp);
            }
        }
        else
        {
            swTemp = SUB(E_MARGIN, swE_pvad);
            //swTemp = shr(swM_pvad, swTemp);
            swTemp = L_SHR_V(swM_pvad, swTemp);
            //L_temp = L_ADD(L_DEPOSIT_L(M_MARGIN), L_DEPOSIT_L(swTemp));
            L_temp =  ADD(L_DEPOSIT_L(M_MARGIN), L_DEPOSIT_L(swTemp));

            if (L_temp > 0x7fffL)
            {
                //swE_temp = ADD_SAT16(E_MARGIN, 1);
                swE_temp = ADD(E_MARGIN, 1);
                //swM_temp = EXTRACT_L(L_shr(L_temp, 1));
                swM_temp = EXTRACT_L(SHR(L_temp, 1));

            }
            else
            {
                swE_temp = E_MARGIN;
                swM_temp = EXTRACT_L(L_temp);
            }
        }
    }

    /*** Test if thvad > pvad + margin ***/

    swComp = 0;

    if (*pswE_thvad > swE_temp)
        swComp = 1;

    if ((*pswE_thvad == swE_temp) && (*pswM_thvad > swM_temp))
        swComp = 1;

    if (swComp == 1)
    {
        *pswE_thvad = swE_temp;
        *pswM_thvad = swM_temp;
    }

    /*** Normalise and retain rvad[0..8] in memory ***/

    *pswNormRvad = swNormRav1;

    for (i = 0; i <= 8; i++)
        pswRvadm[i] = pswRav1[i];

    /*** Set adaptcount to adp + 1 ***/

    swAdaptCount = 9;

}

/****************************************************************************
 *
 *     FUNCTION:  vad_decision
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the VAD decision based on the comparison of the
 *                floating point representations of pvad and thvad.
 *
 *     INPUTS:    swM_pvad      mantissa of filtered signal energy
 *                swE_pvad      exponent of filtered signal energy
 *                swM_thvad     mantissa of decision threshold
 *                swE_thvad     exponent of decision threshold
 *
 *     OUTPUTS:   pswVvad       vad decision before hangover is added
 *
 ***************************************************************************/

void   vad_decision(INT16 swM_pvad,
                    INT16 swE_pvad,
                    INT16 swM_thvadm,
                    INT16 swE_thvadm,
                    INT16 *pswVvad)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    *pswVvad = 0;

    if (swE_pvad > swE_thvadm)
        *pswVvad = 1;
    if ((swE_pvad == swE_thvadm) && (swM_pvad > swM_thvadm))
        *pswVvad = 1;

}

/****************************************************************************
 *
 *     FUNCTION:  vad_hangover
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the final VAD decision for the current frame
 *                being processed.
 *
 *     INPUTS:    swVvad        vad decision before hangover is added
 *
 *     OUTPUTS:   pswVadFlag    vad decision after hangover is added
 *
 ***************************************************************************/

void   vad_hangover(INT16 swVvad,
                    INT16 *pswVadFlag)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    if (swVvad == 1)
        //swBurstCount = ADD_SAT16(swBurstCount, 1);
        swBurstCount++;
    else
        swBurstCount = 0;

    if (swBurstCount >= 3)
    {
        swHangCount = 5;
        swBurstCount = 3;
    }

    *pswVadFlag = swVvad;

    if (swHangCount >= 0)
    {
        *pswVadFlag = 1;
        swHangCount--;// = sub(swHangCount, 1);
    }

}

/****************************************************************************
 *
 *     FUNCTION:  periodicity_update
 *
 *     VERSION:   1.2
 *
 *     PURPOSE:   Computes the ptch flag needed for the threshold
 *                adaptation decision for the next frame.
 *
 *     INPUTS:    pswLags[0..3]    speech encoder long term predictor lags
 *
 *     OUTPUTS:   pswPtch          Boolean voiced / unvoiced decision
 *
 ***************************************************************************/

void   periodicity_update(INT16 pswLags[4],
                          INT16 *pswPtch)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16
    swMinLag,
    swMaxLag,
    swSmallLag,
    swLagCount,
    swTemp;

    int
    i,
    j;


//VPP_HR_PROFILE_FUNCTION_ENTER(periodicity_update );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /*** Run loop for No. of sub-segments in the frame ***/

    swLagCount = 0;

    for (i = 0; i <= 3; i++)
    {
        /*** Search the maximum and minimum of consecutive lags ***/

        if (swOldLag > pswLags[i])
        {
            swMinLag = pswLags[i];
            swMaxLag = swOldLag;
        }
        else
        {
            swMinLag = swOldLag;
            swMaxLag = pswLags[i];
        }


        /*** Compute smallag (modulo operation not defined) ***/

        swSmallLag = swMaxLag;

        for (j = 0; j <= 2; j++)
        {
            if (swSmallLag >= swMinLag)
                swSmallLag = SUB(swSmallLag, swMinLag);
        }


        /***  Minimum of smallag and minlag - smallag ***/

        swTemp = SUB(swMinLag, swSmallLag);

        if (swTemp < swSmallLag)
            swSmallLag = swTemp;

        if (swSmallLag < 2)
            //swLagCount= ADD_SAT16(swLagCount, 1);
            swLagCount++;


        /*** Save the current LTP lag ***/

        swOldLag = pswLags[i];
    }


    /*** Update the veryoldlagcount and oldlagcount ***/

    swVeryOldLagCount = swOldLagCount;
    swOldLagCount = swLagCount;


    /*** Make ptch decision ready for next frame ***/

    //swTemp = ADD_SAT16(swOldLagCount, swVeryOldLagCount);
    swTemp = ADD(swOldLagCount, swVeryOldLagCount);

    if (swTemp >= 7)
        *pswPtch = 1;
    else
        *pswPtch = 0;


//VPP_HR_PROFILE_FUNCTION_EXIT(periodicity_update );


}

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
#include "vpp_hr_mathhalf.h"
#include "vpp_hr_sp_rom.h"
#include "vpp_hr_sp_dec.h"
#include "vpp_hr_sp_frm.h"
#include "vpp_hr_sp_sfrm.h"
#include "vpp_hr_mathhalf_macro.h"


//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



/*_________________________________________________________________________
 |                                                                         |
 |                              Local Defines                              |
 |_________________________________________________________________________|
*/

#define CG_INT_MACS     6
#define C_BITS_UV       7
#define C_BITS_UV_1     C_BITS_UV-1
#define C_BITS_V        9
#define C_BITS_V_1      C_BITS_V-1
#define DELTA_LEVELS    16
#define GSP0_NUM        32
#define GSP0_VECTOR_SIZE      5
#define GTWEAKMAX       0x5A82         /* sqrt(2)/2 */
#define LMAX            142
#define LSMAX           (LMAX + CG_INT_MACS/2)
#define HNW_BUFF_LEN    LSMAX
#define LSP_MASK        0xffff
#define LTP_LEN         147            /* maximum ltp lag */
#define MAX_CANDIDATE   6              /* maximum number of lag candidates */


/*_________________________________________________________________________
 |                                                                         |
 |                     State variables (globals)                           |
 |_________________________________________________________________________|
*/

INT16 pswLtpStateBase[LTP_LEN + S_LEN];
INT16 pswHState[NP];
INT16 pswHNWState[HNW_BUFF_LEN];

/***************************************************************************
 *
 *   FUNCTION NAME: closedLoopLagSearch
 *
 *   PURPOSE:
 *
 *     Performs the closed loop search of a list of candidate lags to
 *     determine the best fractional lag.
 *
 *   INPUTS:
 *
 *     pswLagList[0:iNumLags] - list of candidate lags
 *     iNumLags - number of candidate lags in LagList
 *     pswLtpState[0:5] - array of past excitations (LTP state)
 *     pswHCoefs[0:9] - coefficient array of spectral weighting filter
 *     pswPVect[0:39] - speech sub frame data
 *
 *   OUTPUTS:
 *
 *     pswLag - pointer to put best lag from list of candidates
 *     *pswLtpShift - Number of shifts applied to weighted LTP vector.
 *
 *   RETURN VALUE:
 *
 *     siLagCode - code corresponding to the best lag
 *
 *   IMPLEMENTATION:
 *
 *     Generate excitation vectors for all candidate lags. Find the candidate
 *     lag that maximizes C**2/G using the calculated excitation.
 *
 *   DESCRIPTION:
 *
 *     The function closedLoopLagSearch() searches a very small subset of the
 *     available LTP lags.  The lags to be searched are defined by the open
 *     loop lag search.  This information is passed in as a list of
 *     oversampled lag values.  These values are translated into LTP
 *     vectors extracted from the LTP history.
 *
 *     GSM document 06.20's b sub L prime variable is called
 *     ppswTVect[L][n] in the C code.  The document's variable p(n) is
 *     named pswPVect[] in the C code.
 *
 *     The function performs a simple maximization of the cross correlation
 *     of the weighted LTP vector and the weighted speech vector divided
 *     by the autocorrelation of the weighted LTP vector.  The function is
 *     encumbered slightly by the necessity of scaling.
 *
 *   REFERENCE:  Sub-clause 4.1.8.5 of GSM Recommendation 06.20
 *
 *   KEYWORDS:  closed loop, LTP lag search, adaptive codebook search
 *
 **************************************************************************/

int    closedLoopLagSearch(INT16 pswLagList[], int iNumLags,
                           INT16 pswLtpState[], INT16 pswHCoefs[],
                           INT16 pswPVect[],
                           INT16 *pswLag, INT16 *pswLtpShift)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Energy,
          L_ccNorm,
          L_cgNorm,
          L_CrossCorr;
    INT32 pL_CCBuf[MAX_CANDIDATE],
          pL_CGBuf[MAX_CANDIDATE];
    INT16 swCCMax,
          swCCShiftCnt,
          swCGShiftCnt,
          swGMax,
          swLTPEnergy,
          swSampleA,
          pswCCBuf[MAX_CANDIDATE],
          pswCGBuf[MAX_CANDIDATE],
          ppswTVect[N_SUB][S_LEN];
    INT16 i,
          j,
          siLagOffset,
          siLagCode;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    VPP_HR_PROFILE_FUNCTION_ENTER(closedLoopLagSearch );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */


    *pswLtpShift = 0;                    /* Energy in weighted ltp vector =
                                        * [0..0x7ff] */
    for (i = 0; i < iNumLags; i++)
    {

        /* Construct the excitation vector for lag i */
        /* ----------------------------------------- */

        fp_ex(pswLagList[i], &pswLtpState[LTP_LEN]);

        /* Perform all pole filtering */
        /* -------------------------- */

        lpcZsIir(&pswLtpState[LTP_LEN], pswHCoefs, ppswTVect[i]);
    }

    /* scale the pitch vector s.t. its energy is strictly */
    /* less than 1.0                                      */
    /*----------------------------------------------------*/

    swSampleA = SHR(ppswTVect[0][0], 2);
    //L_Energy = L_mac(0x001dff4cL, swSampleA, swSampleA);
    s_lo=0xEFFA6;//SHR( 0x001dff4cL,1);
    VPP_MLA16(s_hi,s_lo, swSampleA, swSampleA);
    for (j = 1; j < S_LEN; j++)
    {
        swSampleA = SHR(ppswTVect[0][j], 2);
        //L_Energy = L_mac(L_Energy, swSampleA, swSampleA);
        VPP_MLA16(s_hi,s_lo, swSampleA, swSampleA);
    }

    /* add in the energy of the first sample of the subsequent lags */
    /*--------------------------------------------------------------*/

    for (i = 1; i < iNumLags; i++)
    {
        swSampleA = SHR(ppswTVect[i][0], 2);
        //L_Energy = L_mac(L_Energy, swSampleA, swSampleA);
        VPP_MLA16(s_hi,s_lo, swSampleA, swSampleA);
    }
    L_Energy = L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo) ;

    /* An upper bound on the weighted pitch vectors energy */
    /*-----------------------------------------------------*/

    swLTPEnergy = ROUNDO(L_Energy);

    if ( swLTPEnergy > 0x07ff )
    {
        /* E = (0x7ff.. 0x7fff] */
        if ( swLTPEnergy > 0x1fff )
        {
            *pswLtpShift = 2;                /* E = (0x1fff..0x7fff] */
        }
        else
        {
            *pswLtpShift = 1;                /* E = (0x7ff.. 0x1fff] */
        }

    }

    for (i = 0; i < iNumLags; i++)
    {

        /* shift all vectors down s.t. the largest is has energy < 1.0 */
        /*-------------------------------------------------------------*/

        for (j = 0; j < S_LEN; j++)
            //ppswTVect[i][j] = shr(ppswTVect[i][j], *pswLtpShift);
            ppswTVect[i][j] = L_SHR_V(ppswTVect[i][j], *pswLtpShift);

        /* Calculate the energy of the subframe */
        /* ------------------------------------ */

        //L_Energy = L_MULT(ppswTVect[i][0], ppswTVect[i][0]);
        //s_lo=SHR( L_Energy,1);
        VPP_MLX16(s_hi,s_lo, ppswTVect[i][0], ppswTVect[i][0]);
        for (j = 1; j < S_LEN; j++)
            //L_Energy = L_mac(L_Energy, ppswTVect[i][j], ppswTVect[i][j]);
            VPP_MLA16(s_hi,s_lo, ppswTVect[i][j], ppswTVect[i][j]);

        L_Energy = L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo) ;
        pL_CGBuf[i] = L_Energy;

        /* Cross correlate the normalized speech frame and the filtered
         * subframe */
        /* ---------------------------------------------------------------------
         * */

        //L_CrossCorr = L_MULT(ppswTVect[i][0], pswPVect[0]);
        //s_lo=SHR( L_CrossCorr,1);
        VPP_MLX16(s_hi,s_lo, ppswTVect[i][0], pswPVect[0]);
        for (j = 1; j < S_LEN; j++)
            //L_CrossCorr = L_mac(L_CrossCorr, ppswTVect[i][j], pswPVect[j]);
            VPP_MLA16(s_hi,s_lo, ppswTVect[i][j], pswPVect[j]);

        L_CrossCorr = L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo) ;
        pL_CCBuf[i] = L_CrossCorr;

    }

    /* find the shift count associated with the largest CC and G */
    /* ---------------------------------------------------------- */
    L_ccNorm = L_ABS(pL_CCBuf[0]);
    L_cgNorm = pL_CGBuf[0];

    for (i = 1; i < iNumLags; i++)
    {
        L_ccNorm |= L_ABS(pL_CCBuf[i]);
        L_cgNorm |= pL_CGBuf[i];
    }

    swCCShiftCnt = norm_l(L_ccNorm);
    swCGShiftCnt = norm_l(L_cgNorm);

    for (i = 0; i < iNumLags; i++)
    {
        //pswCCBuf[i] = ROUNDO(L_shl(pL_CCBuf[i], swCCShiftCnt));
        //pswCGBuf[i] = ROUNDO(L_shl(pL_CGBuf[i], swCGShiftCnt));
        //pswCCBuf[i] = ROUNDO(L_SHL_SAT(pL_CCBuf[i], swCCShiftCnt));
        //pswCGBuf[i] = ROUNDO(L_SHL_SAT(pL_CGBuf[i], swCGShiftCnt));
        pswCCBuf[i] = ROUNDO(SHL(pL_CCBuf[i], swCCShiftCnt));
        pswCGBuf[i] = ROUNDO(SHL(pL_CGBuf[i], swCGShiftCnt));

    }

    /* Maximize C**2/G */
    /* --------------- */
    siLagOffset = maxCCOverGWithSign(pswCCBuf, pswCGBuf,
                                     &swCCMax, &swGMax,
                                     iNumLags);

    /* Determine the offset of the max value into CC buffer */
    /* ---------------------------------------------------- */
    *pswLag = pswLagList[siLagOffset];

    /* Store Lag Code for best lag result */
    /* ---------------------------------- */
    quantLag(*pswLag, &siLagCode);


    VPP_HR_PROFILE_FUNCTION_EXIT(closedLoopLagSearch );


    return (siLagCode);
}

/*****************************************************************************
 *
 *   FUNCTION NAME: decorr
 *
 *   PURPOSE: Decorrelates(orthogonalizes) a set of vectors from a given
 *            vector.
 *
 *
 *   INPUTS: iNumVects - number of vectors to decorrelate
 *           pswGivenVect[0..39] - array of given vectors
 *           pswVects[0..359] (voice) [0..279] (unvoiced) - array of
 *             contiguous vectors to be decorrelated
 *   OUTPUTS: pswVects[0..359] (voice) [0..279] (unvoiced) - output vectors
 *            are written back over input vectors
 *
 *   RETURN VALUE: none
 *
 *   IMPLEMENTATION:
 *
 *   REFERENCE:  Sub-clause 4.1.10.1 of GSM Recommendation 06.20
 *
 *   KEYWORDS: decorrelate, codewords, codevectors, orthogonalize, encoder
 *
 ****************************************************************************/

void   decorr(int iNumVects, INT16 pswGivenVect[],
              INT16 pswVects[])
{

    /*___________________________________________________________________________
     |                                                                           |
     |                            Automatic Variables                            |
     |___________________________________________________________________________|
    */
    int    i,
           iLoopCnt;
    INT16 swNorm_energy,
          swTemp;
    INT16 swEShift,
          swCShift,
          swShiftSum,
          swQShift;
    INT32 L_Energy,
          L_Temp1,
          L_Temp2,
          L_Accum;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*___________________________________________________________________________
     |                                                                           |
     |                              Executable Code                              |
     |___________________________________________________________________________|
     */

    /* Compute normalized energy in given vector */
    /*-------------------------------------------*/
    swEShift = g_corr1(pswGivenVect, &L_Energy);
    swNorm_energy = EXTRACT_H(L_Energy);

    if (swNorm_energy == 0)
    {
        return;
    }

    /* Decorrelate vectors */
    /*---------------------*/

    for (iLoopCnt = 0; iLoopCnt < iNumVects; iLoopCnt++)
    {

        swCShift = g_corr2(pswGivenVect, &pswVects[iLoopCnt * S_LEN],
                           &L_Temp1);
        L_Temp2 = L_Temp1;
        L_Temp1 = L_ABS(L_Temp1);
        swCShift--;// = sub(swCShift, 1);
        swShiftSum = SUB(swCShift, swEShift);
        //L_Temp1 = L_shr(L_Temp1, 1);
        L_Temp1 = SHR(L_Temp1, 1);
        //swTemp = divide_s(ROUNDO(L_Temp1), swNorm_energy);//
        swTemp = DIVIDE_SAT16(ROUNDO(L_Temp1), swNorm_energy);

        if (L_Temp2 > 0)
            swTemp = NEGATE(swTemp);

        swQShift = norm_s(swTemp);
        //swTemp = shl(swTemp, swQShift);
        swTemp = SHL(swTemp, swQShift);
        //swQShift = ADD_SAT16(swShiftSum, swQShift);
        swQShift += swShiftSum;

        if (swQShift > 0)
        {
            swTemp = SHIFT_R(swTemp, NEGATE(swQShift));
            swQShift = 0;
        }
        else
            swQShift = NEGATE(swQShift);

        for (i = 0; i < S_LEN; i++)
        {
            //L_Accum = L_msu(0x00008000L, pswVects[i + iLoopCnt * S_LEN], SW_MIN);
            s_lo=0x00004000L;//SHR(0x00008000L,1);
            VPP_MLA16(s_hi,s_lo, -pswVects[i + iLoopCnt * S_LEN], SW_MIN);

            //temp=L_mac(L_Accum, swTemp, shl(pswGivenVect[i], swQShift));
            //VPP_MLA16(s_hi,s_lo, swTemp, shl(pswGivenVect[i], swQShift));
            VPP_MLA16(s_hi,s_lo, swTemp, SHL(pswGivenVect[i], swQShift));

            L_Accum = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);

            pswVects[iLoopCnt * S_LEN + i] = EXTRACT_H(L_Accum );

            //pswVects[iLoopCnt * S_LEN + i] = EXTRACT_H(L_mac(L_Accum, swTemp, shl(pswGivenVect[i], swQShift)));
        }
    }
}

/***************************************************************************
 *
 *    FUNCTION NAME: g_corr2
 *
 *    PURPOSE: Calculates correlation between subframe vectors.
 *
 *
 *    INPUT:
 *
 *       pswIn[0:39]
 *                     A subframe vector.
 *
 *       pswIn2[0:39]
 *                     A subframe vector.
 *
 *
 *    OUTPUT:
 *
 *       *pL_out
 *                     A INT32 containing the normalized correlation
 *                     between the input vectors.
 *
 *    RETURN:
 *
 *       swOut
 *                     Number of right shifts which the accumulator was
 *                     shifted to normalize it.  Negative number implies
 *                     a left shift, and therefore an energy larger than
 *                     1.0.
 *
 *    REFERENCE:  Sub-clauses 4.1.10.1 and 4.1.11.1 of GSM
 *       Recommendation 06.20
 *
 *    keywords: energy, autocorrelation, correlation, g_corr2
 *
 *
 **************************************************************************/

INT16 g_corr2(INT16 *pswIn, INT16 *pswIn2,
              INT32 *pL_out)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_sum;
    INT16 swEngyLShft;
    int    i;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Calculate energy in subframe vector (40 samples) */
    /*--------------------------------------------------*/

    //L_sum = L_MULT(pswIn[0], pswIn2[0]);
    //s_lo=SHR( L_sum ,1);
    VPP_MLX16(s_hi,s_lo, pswIn[0], pswIn2[0]);
    for (i = 1; i < S_LEN; i++)
    {
        //L_sum = L_mac(L_sum, pswIn[i], pswIn2[i]);
        VPP_MLA16(s_hi,s_lo, pswIn[i], pswIn2[i]);
    }
    L_sum= L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);



    if (L_sum != 0)
    {

        /* Normalize the energy in the output INT32 */
        /*---------------------------------------------*/

        swEngyLShft = norm_l(L_sum);
        //*pL_out = L_shl(L_sum, swEngyLShft);        /* normalize output * INT32 */
        //(*pL_out) = L_SHL_SAT(L_sum, swEngyLShft);        /* normalize output * INT32 */
        (*pL_out) =  SHL (L_sum, swEngyLShft);        /* normalize output * INT32 */

    }
    else
    {

        /* Special case: energy is zero */
        /*------------------------------*/

        *pL_out = L_sum;
        swEngyLShft = 0;
    }

    return (swEngyLShft);
}

/***************************************************************************
 *
 *   FUNCTION NAME: g_quant_vl
 *
 *   PURPOSE:
 *
 *     Joint quantization of excitation gains.
 *     GS represents the subframe energy relative to the frame energy.
 *     P0 represents the relative contribution of the first exctitation
 *     source to the total excitation.
 *
 *   INPUTS:
 *
 *     swUVCode - voicing level (Mode 0-3)
 *     pswWInput[0:39] - weighted input p(n) (used in mode 0-3)
 *     swWIShift - weighted input shift factor (right shift, 0,1, or 2)
 *     pswWLTPVec[0:39] - weighted pitch excitation vector (used in mode 1-3)
 *     pswWVSVec1[0:39] - weighted 1st v-s codevector (used in mode 0-3)
 *     pswWVSVec2[0:39] - weighted 2nd v-s codevector (used in mode 0)
 *     snsRs00 - square root of RS/pitch excitation energy (used in mode 1-3)
 *     snsRs11 - square root of RS/1st v-s codevector energy
 *               (used in mode 0-3)
 *     snsRs22 - square root of RS/2nd v-s codevector energy (used in mode 0)
 *
 *     pppsrGsp0[0:3][0:31][0:4] - lookup table
 *
 *   OUTPUTS:
 *
 *     None
 *
 *   RETURN VALUE:
 *
 *     siCode - output quantized gain code (5 bits)
 *
 *   IMPLEMENTATION:
 *
 *     Calculates first the parameters required for error equation 7.21:
 *
 *     Rcc(k,j)        k = 0,1, j=k,1
 *     Rx(k)           k = 0,1
 *     RS
 *     Rpc(k)          k = 0,1
 *     a,b,c,d,e
 *
 *     The constant terms in equation 7.21 are stored in ROM instead of GS
 *     and P0. There is one vector quantizer for each voicing state.
 *
 *   REFERENCE:  Sub-clause 4.1.11 and 4.1.11.1 of GSM Recommendation 06.20
 *
 *   KEYWORDS: gain quantization, energy domain transforms, p0, gs
 *
 **************************************************************************/

INT16 g_quant_vl(INT16 swUVCode,
                 INT16 pswWInput[], INT16 swWIShift,
                 INT16 pswWLTPVec[],
                 INT16 pswWVSVec1[], INT16 pswWVSVec2[],
                 struct NormSw snsRs00, struct NormSw snsRs11,
                 struct NormSw snsRs22)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Temp,
          L_Temp2;
    INT16 swShift;
    struct NormSw ErrorTerm[6];
    INT16 i,siNormShift,siNormMin,siCode=0;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Test voicing level, mode 0-3 */
    /* ---------------------------- */

    if (swUVCode == 0)
    {

        /* Unvoiced */
        /* -------- */

        /* Compute cross correlation Rpc(0) */
        /* -------------------------------- */


        ErrorTerm[0].sh = g_corr2(pswWInput, pswWVSVec1, &L_Temp);
        ErrorTerm[0].man = ROUNDO(L_Temp);

        /* Compute cross correlation Rpc(1) */
        /* -------------------------------- */

        ErrorTerm[1].sh = g_corr2(pswWInput, pswWVSVec2, &L_Temp);
        ErrorTerm[1].man = ROUNDO(L_Temp);

        /* Compute cross correlation Rcc(0,1) */
        /* ---------------------------------- */


        ErrorTerm[2].sh = g_corr2(pswWVSVec1, pswWVSVec2, &L_Temp);
        ErrorTerm[2].man = ROUNDO(L_Temp);

        /* Compute correlation Rcc(0,0) */
        /* ---------------------------- */

        ErrorTerm[3].sh = g_corr1(pswWVSVec1, &L_Temp);
        ErrorTerm[3].man = ROUNDO(L_Temp);

        /* Compute correlation Rcc(1,1) */
        /* ---------------------------- */

        ErrorTerm[4].sh = g_corr1(pswWVSVec2, &L_Temp);
        ErrorTerm[4].man = ROUNDO(L_Temp);

        /* Compute correlation Rpp */
        /* ----------------------- */

        ErrorTerm[5].sh = g_corr1(pswWInput, &L_Temp);
        ErrorTerm[5].man = ROUNDO(L_Temp);

        /* Compute gain tweak factor, adjusts A and B error coefs */
        /* ------------------------------------------------------ */
        gainTweak(&ErrorTerm[0]);

        /* Compute error coefficient A, equation 5.22 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[0].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[0].sh = ADD_SAT16(ErrorTerm[0].sh, swShift);
        ErrorTerm[0].sh += swShift;
        //ErrorTerm[0].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[0].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[0].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[0].sh = ADD_SAT16(ErrorTerm[0].sh, snsRs11.sh);
        ErrorTerm[0].sh += snsRs11.sh;
        siNormMin = ErrorTerm[0].sh;

        /* Compute error coefficient B, equation 5.23 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[1].man, snsRs22.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[1].sh = ADD_SAT16(ErrorTerm[1].sh, swShift);
        ErrorTerm[1].sh += swShift;
        //ErrorTerm[1].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[1].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[1].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[1].sh = ADD_SAT16(ErrorTerm[1].sh, snsRs22.sh);
        ErrorTerm[1].sh += snsRs22.sh;
        if ( ErrorTerm[1].sh < siNormMin )
            siNormMin = ErrorTerm[1].sh;

        /* Compute error coefficient C, equation 5.24 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[2].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, swShift);
        ErrorTerm[2].sh += swShift;
        //ErrorTerm[2].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[2].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[2].man = ROUNDO( SHL (L_Temp, swShift));
        L_Temp = L_MULT(ErrorTerm[2].man, snsRs22.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, swShift);
        ErrorTerm[2].sh += swShift;
        //ErrorTerm[2].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[2].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[2].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, snsRs11.sh);
        ErrorTerm[2].sh += snsRs11.sh;
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, snsRs22.sh);
        ErrorTerm[2].sh += snsRs22.sh;
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, swWIShift);
        ErrorTerm[2].sh += swWIShift;
        if ( ErrorTerm[2].sh < siNormMin )
            siNormMin = ErrorTerm[2].sh;

        /* Compute error coefficient D, equation 5.25 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[3].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, swShift);
        ErrorTerm[3].sh += swShift ;
        //ErrorTerm[3].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[3].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[3].man = ROUNDO( SHL (L_Temp, swShift));
        L_Temp = L_MULT(ErrorTerm[3].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, swShift);
        ErrorTerm[3].sh += swShift;
        //ErrorTerm[3].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[3].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[3].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, snsRs11.sh);
        ErrorTerm[3].sh += snsRs11.sh;
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, snsRs11.sh);
        ErrorTerm[3].sh += snsRs11.sh;
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, swWIShift);
        ErrorTerm[3].sh += swWIShift;

        if ( ErrorTerm[3].sh < siNormMin )
            siNormMin = ErrorTerm[3].sh;

        /* Compute error coefficient E, equation 5.26 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[4].man, snsRs22.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, swShift);
        ErrorTerm[4].sh += swShift;
        //ErrorTerm[4].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[4].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[4].man = ROUNDO( SHL (L_Temp, swShift));
        L_Temp = L_MULT(ErrorTerm[4].man, snsRs22.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, swShift);
        ErrorTerm[4].sh += swShift;
        //ErrorTerm[4].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[4].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[4].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, snsRs22.sh);
        ErrorTerm[4].sh += snsRs22.sh;
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, snsRs22.sh);
        ErrorTerm[4].sh += snsRs22.sh;
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, swWIShift);
        ErrorTerm[4].sh += swWIShift;

        if ( ErrorTerm[4].sh < siNormMin )
            siNormMin = ErrorTerm[4].sh;

    }

    else
    {
        /* Voicing level */

        /* Voiced */
        /* ------ */

        /* Compute cross correlation Rpc(0) */
        /* -------------------------------- */


        ErrorTerm[0].sh = g_corr2(pswWInput, pswWLTPVec, &L_Temp);
        ErrorTerm[0].man = ROUNDO(L_Temp);

        /* Compute cross correlation Rpc(1) */
        /* -------------------------------- */


        ErrorTerm[1].sh = g_corr2(pswWInput, pswWVSVec1, &L_Temp);
        ErrorTerm[1].man = ROUNDO(L_Temp);

        /* Compute cross correlation Rcc(0,1) */
        /* ---------------------------------- */


        ErrorTerm[2].sh = g_corr2(pswWLTPVec, pswWVSVec1, &L_Temp);
        ErrorTerm[2].man = ROUNDO(L_Temp);

        /* Compute correlation Rcc(0,0) */
        /* ---------------------------- */

        ErrorTerm[3].sh = g_corr1(pswWLTPVec, &L_Temp);
        ErrorTerm[3].man = ROUNDO(L_Temp);

        /* Compute correlation Rcc(1,1) */
        /* ---------------------------- */

        ErrorTerm[4].sh = g_corr1(pswWVSVec1, &L_Temp);
        ErrorTerm[4].man = ROUNDO(L_Temp);

        /* Compute correlation Rpp */
        /* ----------------------- */

        ErrorTerm[5].sh = g_corr1(pswWInput, &L_Temp);
        ErrorTerm[5].man = ROUNDO(L_Temp);

        /* Compute gain tweak factor, adjusts A and B error coefs */
        /* ------------------------------------------------------ */

        gainTweak(&ErrorTerm[0]);

        /* Compute error coefficient A, equation 5.22 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[0].man, snsRs00.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[0].sh = ADD_SAT16(ErrorTerm[0].sh, swShift);
        ErrorTerm[0].sh += swShift;
        //ErrorTerm[0].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[0].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[0].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[0].sh = ADD_SAT16(ErrorTerm[0].sh, snsRs00.sh);
        ErrorTerm[0].sh += snsRs00.sh;
        siNormMin = ErrorTerm[0].sh;

        /* Compute error coefficient B, equation 5.23 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[1].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[1].sh = ADD_SAT16(ErrorTerm[1].sh, swShift);
        ErrorTerm[1].sh += swShift;
        //ErrorTerm[1].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[1].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[1].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[1].sh = ADD_SAT16(ErrorTerm[1].sh, snsRs11.sh);
        ErrorTerm[1].sh += snsRs11.sh;
        if ( ErrorTerm[1].sh < siNormMin )
            siNormMin = ErrorTerm[1].sh;

        /* Compute error coefficient C, equation 5.24 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[2].man, snsRs00.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, swShift);
        ErrorTerm[2].sh += swShift;
        //ErrorTerm[2].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[2].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[2].man = ROUNDO( SHL (L_Temp, swShift));
        L_Temp = L_MULT(ErrorTerm[2].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, swShift);
        ErrorTerm[2].sh += swShift;
        //ErrorTerm[2].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[2].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[2].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, snsRs00.sh);
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, snsRs11.sh);
        //ErrorTerm[2].sh = ADD_SAT16(ErrorTerm[2].sh, swWIShift);
        ErrorTerm[2].sh += snsRs00.sh;
        ErrorTerm[2].sh += snsRs11.sh;
        ErrorTerm[2].sh += swWIShift;
        if ( ErrorTerm[2].sh < siNormMin )
            siNormMin = ErrorTerm[2].sh;

        /* Compute error coefficient D, equation 5.25 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[3].man, snsRs00.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, swShift);
        ErrorTerm[3].sh += swShift;
        //ErrorTerm[3].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[3].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[3].man = ROUNDO( SHL (L_Temp, swShift));
        L_Temp = L_MULT(ErrorTerm[3].man, snsRs00.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, swShift);
        ErrorTerm[3].sh += swShift;
        //ErrorTerm[3].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[3].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[3].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, snsRs00.sh);
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, snsRs00.sh);
        //ErrorTerm[3].sh = ADD_SAT16(ErrorTerm[3].sh, swWIShift);
        ErrorTerm[3].sh += snsRs00.sh;
        ErrorTerm[3].sh += snsRs00.sh;
        ErrorTerm[3].sh += swWIShift;
        if ( ErrorTerm[3].sh < siNormMin )
            siNormMin = ErrorTerm[3].sh;

        /* Compute error coefficient E, equation 5.26 */
        /* ------------------------------------------ */

        L_Temp = L_MULT(ErrorTerm[4].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, swShift);
        ErrorTerm[4].sh += swShift;
        //ErrorTerm[4].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[4].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[4].man = ROUNDO( SHL (L_Temp, swShift));
        L_Temp = L_MULT(ErrorTerm[4].man, snsRs11.man);
        swShift = norm_s(EXTRACT_H(L_Temp));
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, swShift);
        ErrorTerm[4].sh += swShift;
        //ErrorTerm[4].man = ROUNDO(L_shl(L_Temp, swShift));
        //ErrorTerm[4].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
        ErrorTerm[4].man = ROUNDO( SHL (L_Temp, swShift));
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, snsRs11.sh);
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, snsRs11.sh);
        //ErrorTerm[4].sh = ADD_SAT16(ErrorTerm[4].sh, swWIShift);
        ErrorTerm[4].sh += snsRs11.sh;
        ErrorTerm[4].sh += snsRs11.sh;
        ErrorTerm[4].sh += swWIShift;
        if ( ErrorTerm[4].sh < siNormMin )
            siNormMin = ErrorTerm[4].sh;

    }                                    /* Voicing level */



    /* Normalize all error coefficients to same shift count */
    /* ---------------------------------------------------- */

    for (i = 0; i < GSP0_VECTOR_SIZE; i++)
    {
        L_Temp = L_DEPOSIT_H(ErrorTerm[i].man);
        siNormShift = SUB(ErrorTerm[i].sh, siNormMin);
        if (siNormShift > 0)
            //L_Temp = L_shr(L_Temp, siNormShift);
            L_Temp = SHR(L_Temp, siNormShift);
        ErrorTerm[i].man = ROUNDO(L_Temp);
    }


    /* Codebook search, find max of error equation 5.21 */
    /* ------------------------------------------------ */

    L_Temp2 = 0x80000000;

    for (i = 0; i < GSP0_NUM; i++)
    {

        //L_Temp = L_MULT(pppsrGsp0[swUVCode][i][0], ErrorTerm[0].man);
        //s_lo=SHR(L_Temp ,1);
        VPP_MLX16(s_hi,s_lo, pppsrGsp0[swUVCode][i][0], ErrorTerm[0].man);
        //L_Temp = L_mac(L_Temp, pppsrGsp0[swUVCode][i][1], ErrorTerm[1].man);
        VPP_MLA16(s_hi,s_lo, pppsrGsp0[swUVCode][i][1], ErrorTerm[1].man);

        //L_Temp = L_mac(L_Temp, pppsrGsp0[swUVCode][i][2], ErrorTerm[2].man);
        VPP_MLA16(s_hi,s_lo, pppsrGsp0[swUVCode][i][2], ErrorTerm[2].man);

        //L_Temp = L_mac(L_Temp, pppsrGsp0[swUVCode][i][3], ErrorTerm[3].man);
        VPP_MLA16(s_hi,s_lo, pppsrGsp0[swUVCode][i][3], ErrorTerm[3].man);

        //L_Temp = L_mac(L_Temp, pppsrGsp0[swUVCode][i][4], ErrorTerm[4].man);
        VPP_MLA16(s_hi,s_lo, pppsrGsp0[swUVCode][i][4], ErrorTerm[4].man);

        L_Temp= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
        //if (L_sub(L_Temp2, L_Temp) < 0)
        if ( L_Temp2 < L_Temp )
        {
            L_Temp2 = L_Temp;
            siCode = i;                      /* Save best code */
        }
    }
    return (siCode);
}

/***************************************************************************
 *
 *   FUNCTION NAME: gainTweak
 *
 *   PURPOSE:
 *
 *     Calculates gain bias factor, limits it, and
 *     applies it to A and B error coefficients.
 *
 *   INPUTS:
 *
 *     psErrorTerm[0:5] - array (6) of error coefficients in floating
 *                        point format
 *
 *   OUTPUTS:
 *
 *     psErrorTerm[0:5] - array of gain adjusted A and B error coefficients
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   IMPLEMENTATION:
 *
 *     The gain tweak is:
 *
 *                 Rpp*Rcc(0,0)*Rcc(1,1) - Rpp*Rcc(0,1)*Rcc(0,1)
 *sqrt(---------------------------------------------------------------------)
 *     Rcc(0,0)*Rpc(1)*Rpc(1)-2*Rcc(0,1)*Rpc(0)*Rpc(1)+Rcc(1,1)*Rpc(0)*Rpc(0)
 *
 *   REFERENCE:  Sub-clause 4.1.11.1 of GSM Recommendation 06.20
 *
 *   KEYWORDS: gain tweak, g_quant_vl
 *
 **************************************************************************/

void   gainTweak(struct NormSw *psErrorTerm)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Temp;
    INT16 swTemp,
          swNum,
          swDenom,
          swGainTweak,
          swShift;
    struct NormSw terms[5];
    INT16 i,
          siNormShift,
          siNorm;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Calculate third order terms in the gain tweak factor, while
     * maintaining the largest exponent */
    /* ---------------------------------------------------- */

    /* Compute Rpp*Rcc(0,0)*Rcc(1,1) */
    /* ----------------------------- */

    L_Temp = L_MULT(psErrorTerm[3].man, psErrorTerm[5].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[0].sh = ADD_SAT16(psErrorTerm[3].sh, swShift);
    terms[0].sh = ADD(psErrorTerm[3].sh, swShift);
    //terms[0].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[0].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[0].man = ROUNDO( SHL (L_Temp, swShift));
    L_Temp = L_MULT(terms[0].man, psErrorTerm[4].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[0].sh = ADD_SAT16(terms[0].sh, swShift);
    terms[0].sh += swShift;
    //terms[0].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[0].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[0].man = ROUNDO( SHL (L_Temp, swShift));
    //terms[0].sh = ADD_SAT16(terms[0].sh, psErrorTerm[4].sh);
    //terms[0].sh = ADD_SAT16(terms[0].sh, psErrorTerm[5].sh);
    terms[0].sh += psErrorTerm[4].sh;
    terms[0].sh += psErrorTerm[5].sh;
    /* Init. siNorm */
    siNorm = terms[0].sh;

    /* Compute Rpp*Rcc(0,1)*Rcc(0,1) */
    /* ----------------------------- */

    L_Temp = L_MULT(psErrorTerm[2].man, psErrorTerm[2].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[1].sh = ADD_SAT16(psErrorTerm[2].sh, swShift);
    terms[1].sh = ADD(psErrorTerm[2].sh, swShift);
    //terms[1].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[1].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[1].man = ROUNDO( SHL (L_Temp, swShift));
    L_Temp = L_MULT(terms[1].man, psErrorTerm[5].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[1].sh = ADD_SAT16(terms[1].sh, swShift);
    terms[1].sh += swShift;
    //terms[1].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[1].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[1].man = ROUNDO( SHL (L_Temp, swShift));
    //terms[1].sh = ADD_SAT16(terms[1].sh, psErrorTerm[2].sh);
    //terms[1].sh = ADD_SAT16(terms[1].sh, psErrorTerm[5].sh);
    terms[1].sh += psErrorTerm[2].sh;
    terms[1].sh += psErrorTerm[5].sh;
    if ( terms[1].sh < siNorm )
        siNorm = terms[1].sh;

    /* Compute Rcc(0,0)*Rpc(1)*Rpc(1) */
    /* ------------------------------ */

    L_Temp = L_MULT(psErrorTerm[1].man, psErrorTerm[1].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[2].sh = ADD_SAT16(psErrorTerm[1].sh, swShift);
    terms[2].sh = ADD(psErrorTerm[1].sh, swShift);
    //terms[2].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[2].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[2].man = ROUNDO( SHL (L_Temp, swShift));
    L_Temp = L_MULT(terms[2].man, psErrorTerm[3].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[2].sh = ADD_SAT16(terms[2].sh, swShift);
    terms[2].sh += swShift;
    //terms[2].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[2].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[2].man = ROUNDO( SHL (L_Temp, swShift));
    //terms[2].sh = ADD_SAT16(terms[2].sh, psErrorTerm[1].sh);
    //terms[2].sh = ADD_SAT16(terms[2].sh, psErrorTerm[3].sh);
    terms[2].sh += psErrorTerm[1].sh;
    terms[2].sh += psErrorTerm[3].sh;
    if ( terms[2].sh < siNorm )
        siNorm = terms[2].sh;

    /* Compute 2*Rcc(0,1)*Rpc(0)*Rpc(1) */
    /* -------------------------------- */

    L_Temp = L_MULT(psErrorTerm[0].man, psErrorTerm[1].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[3].sh = ADD_SAT16(psErrorTerm[0].sh, swShift);
    terms[3].sh = ADD(psErrorTerm[0].sh, swShift);
    //terms[3].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[3].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[3].man = ROUNDO( SHL (L_Temp, swShift));
    L_Temp = L_MULT(terms[3].man, psErrorTerm[2].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[3].sh = ADD_SAT16(terms[3].sh, swShift);
    terms[3].sh += swShift;
    //terms[3].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[3].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[3].man = ROUNDO( SHL (L_Temp, swShift));
    //terms[3].sh = ADD_SAT16(terms[3].sh, psErrorTerm[1].sh);
    //terms[3].sh = ADD_SAT16(terms[3].sh, psErrorTerm[2].sh);
    terms[3].sh += psErrorTerm[1].sh;
    terms[3].sh += psErrorTerm[2].sh;
    terms[3].sh--;// = sub(terms[3].sh, 1);   /* Multiply by 2 */
    if ( terms[3].sh < siNorm )
        siNorm = terms[3].sh;

    /* Compute Rcc(1,1)*Rpc(0)*Rpc(0) */
    /* ------------------------------ */

    L_Temp = L_MULT(psErrorTerm[0].man, psErrorTerm[4].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[4].sh = ADD_SAT16 (psErrorTerm[0].sh, swShift);
    terms[4].sh = ADD (psErrorTerm[0].sh, swShift);
    //terms[4].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[4].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[4].man = ROUNDO( SHL (L_Temp, swShift));
    L_Temp = L_MULT(terms[4].man, psErrorTerm[0].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //terms[4].sh = ADD_SAT16 (terms[4].sh, swShift);
    terms[4].sh += swShift;
    //terms[4].man = ROUNDO(L_shl(L_Temp, swShift));
    //terms[4].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    terms[4].man = ROUNDO( SHL (L_Temp, swShift));
    //terms[4].sh = ADD_SAT16 (terms[4].sh, psErrorTerm[0].sh);
    //terms[4].sh = ADD_SAT16 (terms[4].sh, psErrorTerm[4].sh);
    terms[4].sh += psErrorTerm[0].sh;
    terms[4].sh += psErrorTerm[4].sh;
    if ( terms[4].sh <siNorm )
        siNorm = terms[4].sh;

    /* Normalize all terms to same shift count */
    /* --------------------------------------- */

    for (i = 0; i < 5; i++)
    {
        L_Temp = L_DEPOSIT_H(terms[i].man);
        siNormShift = SUB(terms[i].sh, siNorm);
        if (siNormShift > 0)
        {
            //L_Temp = L_shr(L_Temp, siNormShift);
            L_Temp = SHR(L_Temp, siNormShift);

        }
        terms[i].man = ROUNDO(L_Temp);
    }

    /* Calculate numerator */
    /* ------------------- */

    /* Rpp*Rcc(0,0)*Rcc(1,1) - Rpp*Rcc(0,1)*Rcc(0,1) */
    /* --------------------------------------------- */

    swNum = SUB(terms[0].man, terms[1].man);

    /* Skip gain tweak if numerator =< 0 */
    /* --------------------------------- */

    if (swNum <= 0)
        return;

    /* Calculate denominator */
    /* --------------------- */

    /* Rcc(0,0)*Rpc(1)*Rpc(1)-2*Rcc(0,1)*Rpc(0)*Rpc(1)+Rcc(1,1)*Rpc(0)*Rpc(0) */
    /*----------------------------------------------------------------------*/

    swDenom = SUB(terms[2].man, terms[3].man);
    swDenom = ADD(swDenom, terms[4].man);

    /* Skip gain tweak if denominator =< 0 */
    /* ----------------------------------- */

    if (swDenom <= 0)
        return;

    /* Compare numerator to denominator, skip if tweak =< 1 */
    /* ---------------------------------------------------- */

    swTemp = SUB(swNum, swDenom);
    if (swTemp <= 0)
        return;

    /* Normalize and do divide */
    /* ----------------------- */

    swShift = norm_s(swNum);
    siNormShift = SUB(swShift, 1);       /* Multiply by 2 */
    swNum = SHL(swNum, swShift);
    swNum = SHR(swNum, 1);
    swShift = norm_s(swDenom);
    siNormShift = SUB(siNormShift, swShift);
    swDenom = SHL(swDenom, swShift);
    //swTemp = divide_s(swNum, swDenom);
    swTemp = DIVIDE_SAT16(swNum, swDenom);
    swShift = norm_s(swTemp);
    //siNormShift = ADD_SAT16(siNormShift, swShift);
    siNormShift += swShift;
    //L_Temp = L_shl(L_DEPOSIT_H(swTemp), swShift);
    //L_Temp = L_SHL_SAT(L_DEPOSIT_H(swTemp), swShift);
    L_Temp =  SHL (L_DEPOSIT_H(swTemp), swShift);

    /* Calculate square root */
    /* --------------------- */

    swTemp = sqroot(L_Temp);

    /* If odd no. of shifts compensate by sqrt(0.5) */
    /* -------------------------------------------- */

    if (siNormShift & 1)
    {
        L_Temp = L_MULT(0x5a82, swTemp);
        siNormShift--;// = sub(siNormShift, 1);
    }
    else
        L_Temp = L_DEPOSIT_H(swTemp);
    siNormShift = SHR(siNormShift, 1);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //siNormShift = ADD_SAT16(siNormShift, swShift);
    siNormShift += swShift;
    //swGainTweak = ROUNDO(L_shl(L_Temp, swShift));
    //swGainTweak = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    swGainTweak = ROUNDO( SHL (L_Temp, swShift));

    /* If exponent > -1, skip gain tweak */
    /* --------------------------------- */

    //if (ADD_SAT16(1, siNormShift) > 0)
    if ((1 + siNormShift) > 0)
        return;

    /* If exponent < -1, limit gain tweak to GTWEAKMAX */
    /* ----------------------------------------------- */

    //if (ADD_SAT16(1, siNormShift) < 0)
    if ((1 + siNormShift) < 0)
        swGainTweak = GTWEAKMAX;
    else
    {

        /* If exponent = -1, compare to GTWEAKMAX */
        /* -------------------------------------- */

        if ( GTWEAKMAX < swGainTweak )
            swGainTweak = GTWEAKMAX;
    }

    /* Multiply gain tweak factor on A and B error terms */
    /* ------------------------------------------------- */

    L_Temp = L_MULT(swGainTweak, psErrorTerm[0].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //psErrorTerm[0].sh = ADD_SAT16(psErrorTerm[0].sh, swShift);
    psErrorTerm[0].sh += swShift;
    psErrorTerm[0].sh--;// = sub(psErrorTerm[0].sh, 1);
    //psErrorTerm[0].man = ROUNDO(L_shl(L_Temp, swShift));
    //psErrorTerm[0].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    psErrorTerm[0].man = ROUNDO( SHL (L_Temp, swShift));

    L_Temp = L_MULT(swGainTweak, psErrorTerm[1].man);
    swShift = norm_s(EXTRACT_H(L_Temp));
    //psErrorTerm[1].sh = ADD_SAT16(psErrorTerm[1].sh, swShift);
    psErrorTerm[1].sh += swShift;
    psErrorTerm[1].sh--;// = sub(psErrorTerm[1].sh, 1);
    //psErrorTerm[1].man = ROUNDO(L_shl(L_Temp, swShift));
    //psErrorTerm[1].man = ROUNDO(L_SHL_SAT(L_Temp, swShift));
    psErrorTerm[1].man = ROUNDO( SHL (L_Temp, swShift));

}

/***************************************************************************
 *
 *   FUNCTION NAME: hnwFilt
 *
 *   PURPOSE:
 *     Performs the filtering operation for harmonic noise weighting.
 *
 *   INPUTS:
 *         pswInSample[0:39] - array of input speech signal,
 *            pswInSample points to the "oldest" sample of the
 *            current subframe to be hnw filtered, S_LEN samples
 *            will be stored in this array, this data is not
 *            explicitly modified.
 *
 *         pswState[0:183] - array of state of samples, the most
 *            recent sample is the tail of the state buffer,
 *            used only for full- state filtering, this data is
 *            not modified
 *         pswInCoef[0:5] - array of unmodified filter coefficients
 *         iStateOffset - address offset from a sample in the subframe back
 *            to the oldest element of the state used in the interpolating
 *            filter for that sample. Although the subframe samples and
 *            state information can come from different buffers, this
 *            offset represents the case in which the state and sample
 *            information are in the same buffer
 *         swZeroState - indicate if the interpolating filter should be
 *            "zero-state" filtering or "full-state" filtering:
 *                       0 ==> zero-state filtering
 *                      !0 ==> full-state filtering
 *         iNumSamples - the number of samples that are to be filtered,
 *            required to be less than or equal to S_LEN in order to
 *            correctly match speech samples with sample states for the
 *            filtering procedure
 *
 *   OUTPUTS:
 *         pswOutSample[0:39] - array of output filtered speech signal,
 *            pswOutSample points to the "oldest" sample location, S_LEN
 *            filtered samples will be stored at the buffer associated with
 *            this array, can implicitly overwrite input samples with
 *            with filtered samples by setting pswOutSample = pswInSample
 *
 *   RETURN VALUE:
 *         none
 *
 *   IMPLEMENTATION:
 *         The harmonic noise weighting filter is implemented in reverse
 *         temporal order, from most recent input sample backwards through
 *         the input sample array. The procedure follows the equation:
 *                   x(n) = x(n) - PW_COEF*x(n - lag)
 *         where the PW_COEF is the pitch weighting for the current
 *         subframe and lag is the full-resolution lag for the current
 *         subframe. x(n - lag) is found by implementing a CG_INT_MACS-
 *         order FIR interpolating filter
 *
 *         Harmonic noise weighting is discussed in secion 5.5.
 *
 *   REFERENCE:  Sub-clause 4.1.9 of GSM Recommendation 06.20
 *
 *   KEYWORDS: T_SUB, LAG, HNW_FILT, PW_COEF, CG_INT_MACS, S_LEN, LSMAX
 *
 **************************************************************************/

void   hnwFilt(INT16 pswInSample[],
               INT16 pswOutSample[],
               INT16 pswState[],
               INT16 pswInCoef[],
               int iStateOffset,
               INT16 swZeroState,
               int iNumSamples)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT32 L_temp;
    int    i,
           j;

    int    iStatIndx = S_LEN - 1 + iStateOffset;
    int    iStatIndx1 = S_LEN + iStateOffset;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    if (swZeroState == 0)
    {

        /* zero state response assumes input and output arrays are the same */
        /*------------------------------------------------------------------*/

        for (i = 0; i < iNumSamples; i++)
        {

            /* get input with rounding */
            /*-------------------------*/
            //L_temp = L_mac((long) 16384, pswInSample[S_LEN - i - 1], 0x4000);
            s_lo=0x00002000L;//SHR( 16384,1);
            VPP_MLA16(s_hi,s_lo, pswInSample[S_LEN - i - 1], 0x4000);


            for (j = 5; (j >= 0) && (iStatIndx - i + j >= 0); j--)
                /* evaluate taps 1 - 6 that point to input */
                /*-----------------------------------------*/
                //L_temp = L_mac(L_temp, pswInSample[iStatIndx - i + j], pswInCoef[j]);
                VPP_MLA16(s_hi,s_lo, pswInSample[iStatIndx - i + j], pswInCoef[j]);


            L_temp = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
            //pswOutSample[S_LEN - 1 - i] = EXTRACT_H(L_shl(L_temp, 1));
            //pswOutSample[S_LEN - 1 - i] = EXTRACT_H(L_SHL_SAT(L_temp, 1));
            pswOutSample[S_LEN - 1 - i] = EXTRACT_H( SHL (L_temp, 1));

        }
    }
    else
    {
        for (i = 0; i < iNumSamples; i++)
        {

            /* get input with rounding */
            /*-------------------------*/
            //L_temp = L_mac((long) 16384, pswInSample[S_LEN - i - 1], 0x4000);
            s_lo=0x00002000L;//SHR( 16384,1);
            VPP_MLA16(s_hi,s_lo, pswInSample[S_LEN - i - 1], 0x4000);

            for (j = 5; (j >= 0) && (iStatIndx - i + j >= 0); j--)
                /* evaluate taps 1 - 6 that point to input */
                /*-----------------------------------------*/
                //L_temp = L_mac(L_temp, pswInSample[iStatIndx - i + j], pswInCoef[j]);
                VPP_MLA16(s_hi,s_lo, pswInSample[iStatIndx - i + j], pswInCoef[j]);

            for (; (j >= 0); j--)
                /* evaluate taps 1 - 6 that point to state */
                /*----------------------------------------*/
                //L_temp = L_mac(L_temp, pswState[iStatIndx1 - i + j], pswInCoef[j]);
                VPP_MLA16(s_hi,s_lo, pswState[iStatIndx1 - i + j], pswInCoef[j]);

            L_temp = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
            //pswOutSample[S_LEN - 1 - i] = EXTRACT_H(L_shl(L_temp, 1));
            //pswOutSample[S_LEN - 1 - i] = EXTRACT_H(L_SHL_SAT(L_temp, 1));
            pswOutSample[S_LEN - 1 - i] = EXTRACT_H( SHL (L_temp, 1));

        }
    }

}

/***************************************************************************
 *
 *   FUNCTION NAME: sfrmAnalysis
 *
 *   PURPOSE:
 *
 *     Determines the synthetic excitation for a subframe.
 *
 *   INPUTS:
 *
 *     pswWSpeech
 *                     Input weighted speech vector to be matched.
 *
 *     swVoicingMode
 *
 *                     Voicing mode 0,1,2 or 3.  0 is unvoiced.  A
 *                     frame parameter.
 *
 *     snsSqrtRs
 *
 *                     Normalized estimate of the excitation energy
 *
 *     pswHCoefs
 *
 *                     Coefficientss used in weighted synthesis filter,
 *                     H(z), (a negated version is used).  pswHCoefs[0]
 *                     is  t=-1 tap, pswHCoefs[9] is t=-10 tap.
 *
 *     pswLagList
 *
 *                     List of lags to be searched in the long-term
 *                     predictor, determined by the open-loop lag search.
 *
 *     siNumLags
 *
 *                     Number of lags in pswLagList.
 *
 *     swPitch
 *
 *                     Fundamental pitch value to be used in harmonic-
 *                     noise-weighting, actualPitch*OS_FCTR.
 *
 *     swHNWCoef
 *                     Coefficient of the harmonic-noise-weighting filter.
 *
 *     ppsrCGIntFilt[0:5][0:5]
 *
 *                     polyphase interpolation filter,
 *                     ppsrCGIntFilt[iTap][iPhase], OS_FCTR phases,
 *                     CG_INT_MACS taps per phase.  Used to construct
 *                     sequences delayed by fractional lags for Harmonic-
 *                     Noise-Weighting.
 *
 *     pppsrUvCodeVec[0:1][0:6][0:39]
 *
 *                     unvoiced codebooks:
 *                     pppsrUvCodeVec[codeBook][vectorNumber][time]
 *
 *     pppsrVcdCodeVec[0][0:8][0:39]
 *
 *                     voiced codebook:
 *                     pppsrVcdCodeVect[codebook(=0)][vectorNumber][time]
 *
 *     swSP
 *                     speech flag (DTX mode)
 *
 *   OUTPUTS:
 *
 *     psiLagCode
 *
 *                     Lag code: frame- or delta-, or zero if unvoiced.
 *
 *     psiVSCode1
 *
 *                     First vector-sum codebook code.
 *
 *     psiVSCode2
 *
 *                     Second vector-sum codebook code, or zero if voiced.
 *
 *     psiGsp0Code
 *
 *                     Gain quantizer code.
 *
 *     DESCRIPTION:
 *
 *     sfrmAnalysis() is the calling function for the subframe analysis
 *     functions.  All subframe based processing is done by it and its
 *     daughter functions.  All functions in this file are called by
 *     sfrmAnalysis() or one of its daughter functions.  As can be seen
 *     above, this routine will select the LTP lag, the VSELP
 *     codevector(s) and the GSP0 codeword.  It is called by
 *     speechEncoder().
 *
 *     The subframe processing can follow one of two paths depending on
 *     whether the frame is voiced or unvoiced.  These two paths are now
 *     described.
 *
 *     First the zero input response of H(z) is calculated (lpcZiIir());
 *     then subtracted from the weighted speech (W(z)).  The outcome, p(n)
 *     or pswWSVec[], will be the vector matched by the first excitation
 *     vector (either adaptive or first VSELP codevector).  The p(n)
 *     vector is scaled to prevent overflow.
 *
 *     If the frame is voiced, the closed loop lag search (closedLoop())
 *     is performed.  An adaptive codevector lag is selected.  Using the
 *     open loop "pitch" value, the harmonic noise weighting
 *     coefficients are obtained.  The adaptive codevector is
 *     reconstructed (fp_ex()), and weighted through the (zero state)
 *     spectral (lpcZsIir()) and harmonic noise weighting filters
 *     (hnwFilt()).
 *
 *     The basis vectors are also filtered through the weighting
 *     filters.  If the frame is unvoiced, there is no spectral noise
 *     weighting.
 *
 *     If voiced the VSELP basis vectors are decorrelated (decorr())
 *     from the selected adaptive (LTP) codevector, and the VSELP
 *     codevector search is initiated (v_srch()).
 *
 *     If unvoiced, the first VSELP codevector search is performed
 *     (without any decorrelation). After a vector from the first VSELP
 *     codebook has been selected, the second set of basis vectors are
 *     decorrelated from the selected vector.
 *
 *     Once all the excitation vectors have been selected, the gain
 *     quantizer is called, g_quant_vl().
 *
 *     Finally, once all subframe parameters have been found, the
 *     selected excitation is scaled according to GSP0 (scaleExcite()),
 *     and the composite excitation is entered into the long term
 *     predictor history.  The final excitation is also used to update
 *     H(z) and C(z).
 *
 *   REFERENCE:  Sub-clauses 4.1.8.5, 4.1.9 - 4.1.12 of GSM
 *     Recommendation 06.20
 *
 *   Keywords: codewords, lag, codevectors, gsp0, decoding, analysis, t_sub
 *
 **************************************************************************/

void   sfrmAnalysis(INT16 *pswWSpeech,
                    INT16 swVoicingMode,
                    struct NormSw snsSqrtRs,
                    INT16 *pswHCoefs,
                    INT16 *pswLagList,
                    short siNumLags,
                    INT16 swPitch,
                    INT16 swHNWCoef,
                    short *psiLagCode,
                    short *psiVSCode1,
                    short *psiVSCode2,
                    short *psiGsp0Code,
                    INT16 swSP)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Static Variables                             |
     |_________________________________________________________________________|
    */

    static short siPrevLagCode;

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short  i,
           j,
           siCode,
           siIntPitch,
           siRemainder;
    short  siHnwOffset=0,
           siHnwNum,
           siNumBasisVecs;

    INT16 swLag,
          swPnEnergy,
          swPnShift,
          swSampleA;
    INT16 swLtpShift;

    INT32 L_PnEnergy;

    struct NormSw snsRs00,
               snsRs11,
               snsRs22;

    INT16 pswWSVec[S_LEN],
          pswTempVec[S_LEN];
    INT16 pswPVec[S_LEN],
          pswWPVec[S_LEN];
    INT16 ppswVselpEx[2][S_LEN],
          ppswWVselpEx[2][S_LEN];
    INT16 pswWBasisVecs[9 * S_LEN],
          pswBitArray[9];
    INT16 pswHNWCoefs[CG_INT_MACS];

    INT16 *pswLtpStateOut;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                            Executable Code                              |
     |_________________________________________________________________________|
    */

    pswLtpStateOut = pswLtpStateBase + LTP_LEN;

    if (swSP == 1)                                             /* DTX mode */
    {
        /* DTX mode */

        /* if not in CNI mode */
        /*--------------------*/

        /* Get the zero-input response of H(z) */
        /*-------------------------------------*/

        lpcZiIir(pswHCoefs, pswHState, pswTempVec);

        /* Subtract the zero-input response of H(z) from W(z)-weighted speech. */
        /* The result is the vector to match for the adaptive codebook (long-  */
        /* term-predictor) search in voiced modes, or the vector to match for  */
        /* all synthetic excitation searches in unvoiced mode.                 */
        /*---------------------------------------------------------------------*/

        for (i = 0; i < S_LEN; i++)
        {

            pswWSVec[i] = SUB(pswWSpeech[i], pswTempVec[i]);
        }

        /* scale the weighted speech vector (p[n]) s.t. its energy is strictly */
        /* less than 1.0                                                       */
        /*---------------------------------------------------------------------*/

        swSampleA = SHR(pswWSVec[0], 2);
        //L_PnEnergy = L_mac(0x001dff4cL, swSampleA, swSampleA);
        s_lo=0xEFFA6;//SHR( 0x001dff4cL,1);
        VPP_MLA16(s_hi,s_lo, swSampleA, swSampleA);
        for (i = 1; i < S_LEN; i++)
        {
            swSampleA = SHR(pswWSVec[i], 2);   /* reduces energy by 16 */
            //L_PnEnergy = L_mac(L_PnEnergy, swSampleA, swSampleA);
            VPP_MLA16(s_hi,s_lo,  swSampleA, swSampleA);
        }
        L_PnEnergy = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
        swPnEnergy = ROUNDO(L_PnEnergy);

        if ( swPnEnergy <= 0x7ff )
        {
            /* E = [0..0x7ff] */

            swPnShift = 0;

        }
        else
        {

            if ( swPnEnergy <= 0x1fff )
            {
                /* E = (0x7ff.. 0x1fff] */
                swPnShift = 1;

            }
            else
            {

                swPnShift = 2;                   /* E = (0x1fff..0x7fff] */

            }
        }

        /* shift pswWSVect down by the shift factor */
        /*------------------------------------------*/

        for (i = 0; i < S_LEN; i++)
            //pswWSVec[i] = shr(pswWSVec[i], swPnShift);
            pswWSVec[i] = L_SHR_V(pswWSVec[i], swPnShift);


        if (swVoicingMode > 0)
        {

            /* Do restricted adaptive codebook (long-term-predictor) search: */
            /* the search is restricted to the lags passed in from the       */
            /* open-loop lag search                                          */
            /*---------------------------------------------------------------*/

            siCode = closedLoopLagSearch(pswLagList, siNumLags,
                                         pswLtpStateBase, pswHCoefs, pswWSVec,
                                         &swLag, &swLtpShift);

            /* Construct frame-lag code if this is the first subframe, */
            /* or delta-lag code if it is not the first subframe       */
            /*---------------------------------------------------------*/

            if (swVoicingMode > 0)
            {

                if (giSfrmCnt == 0)
                {
                    siPrevLagCode = siCode;
                    *psiLagCode = siCode;
                }
                else
                {

                    //*psiLagCode = ADD_SAT16(sub(siCode, siPrevLagCode), DELTA_LEVELS / 2);
                    *psiLagCode = ADD(SUB(siCode, siPrevLagCode), (DELTA_LEVELS / 2));
                    siPrevLagCode = siCode;
                }
            }

            /* From the value of the fundamental pitch obtained in the open-loop */
            /* lag search, get the correct phase of the interpolating filter,    */
            /* and scale the coefficients by the Harmonic-Noise-Weighting        */
            /* coefficient.  The result is the interpolating coefficients scaled */
            /* by the HNW coefficient.  These will be used in all C(z) filtering */
            /*-------------------------------------------------------------------*/

            get_ipjj(swPitch, &siIntPitch, &siRemainder);

            for (i = 0; i < CG_INT_MACS; i++)
            {

                pswHNWCoefs[i] = MULT_R(NEGATE(ppsrCGIntFilt[i][siRemainder]), swHNWCoef);

            }

            /* Calculate a few values which will speed up C(z) filtering:      */
            /* "HnwOffset" is the distance in samples from the input sample of */
            /* the C(z) filter to the first sample tapped by the interpolating */
            /* filter.  "HnwNum" is the number of samples which need to be     */
            /* filtered by C(z) in the zero-state case.                        */
            /*-----------------------------------------------------------------*/

            siHnwOffset = SUB((-CG_INT_MACS / 2), siIntPitch);
            siHnwNum = SUB((S_LEN + CG_INT_MACS / 2 - 1), siIntPitch);

            /* Perform C(z) filter on W(z)-weighted speech, get zero-input */
            /* response of H(z)C(z) combo, subtract zero-input response    */
            /* of H(z)C(z) from W(z)C(z)-weighted speech.  The result is   */
            /* the vector to match for the rest of the synthetic           */
            /* excitation searches in the voiced modes                     */
            /*-------------------------------------------------------------*/

            hnwFilt(pswWSpeech, pswWSVec, &pswWSpeech[-1], pswHNWCoefs,
                    siHnwOffset, 1, S_LEN);

            hnwFilt(pswTempVec, pswTempVec, &pswHNWState[HNW_BUFF_LEN - 1],
                    pswHNWCoefs, siHnwOffset, 1, S_LEN);

            for (i = 0; i < S_LEN; i++)
            {

                //pswWSVec[i] = shr(sub(pswWSVec[i], pswTempVec[i]), swPnShift);
                pswWSVec[i] = L_SHR_V(SUB(pswWSVec[i], pswTempVec[i]), swPnShift);

            }

            /* Recontruct adaptive codebook (long-term-predictor) vector, */
            /* weight it through H(z) and C(z), each with zero state      */
            /*------------------------------------------------------------*/

            fp_ex(swLag, pswLtpStateOut);

            for (i = 0; i < S_LEN; i++)
                pswPVec[i] = pswLtpStateOut[i];

            lpcZsIir(pswPVec, pswHCoefs, pswWPVec);

            if (siHnwNum > 0)
            {
                hnwFilt(pswWPVec, pswWPVec, NULL, pswHNWCoefs, siHnwOffset,
                        0, siHnwNum);
            }
            for (i = 0; i < S_LEN; i++)
            {
                //pswPVec[i] = shr(pswPVec[i], swLtpShift);
                //pswWPVec[i] = shr(pswWPVec[i], swLtpShift);
                pswPVec[i]  = L_SHR_V(pswPVec[i], swLtpShift);
                pswWPVec[i] = L_SHR_V(pswWPVec[i], swLtpShift);

            }

        }
        else
        {

            /* Unvoiced mode: clear all voiced variables */
            /*-------------------------------------------*/

            swLag = 0;
            *psiLagCode = 0;
            siHnwNum = 0;
        }

        /* "NumBasisVecs" will be the number of basis vectors in */
        /* the vector-sum codebook(s)                            */
        /*-------------------------------------------------------*/

        if (swVoicingMode > 0)
            siNumBasisVecs = C_BITS_V;

        else
            siNumBasisVecs = C_BITS_UV;

        /* Filter the basis vectors through H(z) with zero state, and if  */
        /* voiced, through C(z) with zero state                           */
        /*----------------------------------------------------------------*/

        for (i = 0; i < siNumBasisVecs; i++)
        {

            if (swVoicingMode > 0)
            {

                lpcZsIir((INT16 *) pppsrVcdCodeVec[0][i], pswHCoefs,
                         &pswWBasisVecs[i * S_LEN]);
            }
            else
            {

                lpcZsIir((INT16 *) pppsrUvCodeVec[0][i], pswHCoefs,
                         &pswWBasisVecs[i * S_LEN]);
            }

            if (siHnwNum > 0)
            {

                hnwFilt(&pswWBasisVecs[i * S_LEN], &pswWBasisVecs[i * S_LEN],
                        NULL, pswHNWCoefs, siHnwOffset, 0, siHnwNum);
            }
        }

        /* If voiced, make the H(z)C(z)-weighted basis vectors orthogonal to */
        /* the H(z)C(z)-weighted adaptive codebook vector                    */
        /*-------------------------------------------------------------------*/

        if (swVoicingMode > 0)
            decorr(siNumBasisVecs, pswWPVec, pswWBasisVecs);

        /* Do the vector-sum codebook search on the H(z)C(z)-weighted, */
        /* orthogonalized basis vectors                                */
        /*-------------------------------------------------------------*/

        *psiVSCode1 = v_srch(pswWSVec, pswWBasisVecs, siNumBasisVecs);

        /* Construct the chosen vector-sum codebook vector from basis vectors */
        /*--------------------------------------------------------------------*/

        b_con(*psiVSCode1, siNumBasisVecs, pswBitArray);

        if (swVoicingMode > 0)
            v_con((INT16 *) pppsrVcdCodeVec[0][0], ppswVselpEx[0], pswBitArray,
                  siNumBasisVecs);

        else
            v_con((INT16 *) pppsrUvCodeVec[0][0], ppswVselpEx[0], pswBitArray,
                  siNumBasisVecs);

        if (swVoicingMode == 0)
        {

            /* Construct the H(z)-weighted 1st-codebook vector */
            /*-------------------------------------------------*/

            v_con(pswWBasisVecs, ppswWVselpEx[0], pswBitArray, siNumBasisVecs);

            /* Filter the 2nd basis vector set through H(z) with zero state */
            /*--------------------------------------------------------------*/

            for (i = 0; i < siNumBasisVecs; i++)
            {

                lpcZsIir((INT16 *) pppsrUvCodeVec[1][i], pswHCoefs,
                         &pswWBasisVecs[i * S_LEN]);
            }

            /* Make the 2nd set of H(z)-weighted basis vectors orthogonal to the */
            /* H(z)-weighted 1st-codebook vector                                 */
            /*-------------------------------------------------------------------*/

            decorr(siNumBasisVecs, ppswWVselpEx[0], pswWBasisVecs);

            /* Do the vector-sum codebook search on the H(z)-weighted, */
            /* orthogonalized, 2nd basis vector set                    */
            /*---------------------------------------------------------*/

            *psiVSCode2 = v_srch(pswWSVec, pswWBasisVecs, siNumBasisVecs);

            /* Construct the chosen vector-sum codebook vector from the 2nd set */
            /* of basis vectors                                                 */
            /*------------------------------------------------------------------*/

            b_con(*psiVSCode2, siNumBasisVecs, pswBitArray);

            v_con((INT16 *) pppsrUvCodeVec[1][0], ppswVselpEx[1], pswBitArray,
                  siNumBasisVecs);
        }

        else
            *psiVSCode2 = 0;

        /* Filter the 1st-codebook vector through H(z) (also through C(z) */
        /* if appropriate)                                                */
        /*----------------------------------------------------------------*/

        lpcZsIir(ppswVselpEx[0], pswHCoefs, ppswWVselpEx[0]);

        if (siHnwNum > 0)
        {
            hnwFilt(ppswWVselpEx[0], ppswWVselpEx[0], NULL, pswHNWCoefs,
                    siHnwOffset, 0, siHnwNum);
        }

        if (swVoicingMode == 0)
        {

            /* Filter the 2nd-codebook vector through H(z) */
            /*---------------------------------------------*/

            lpcZsIir(ppswVselpEx[1], pswHCoefs, ppswWVselpEx[1]);
        }

        /* Get the square-root of the ratio of residual energy to      */
        /* excitation vector energy for each of the excitation sources */
        /*-------------------------------------------------------------*/

        if (swVoicingMode > 0)
        {

            rs_rrNs(pswPVec, snsSqrtRs, &snsRs00);
        }

        rs_rrNs(ppswVselpEx[0], snsSqrtRs, &snsRs11);

        if (swVoicingMode == 0)
        {

            rs_rrNs(ppswVselpEx[1], snsSqrtRs, &snsRs22);
        }

        /* Determine the vector-quantized gains for each of the excitations */
        /*------------------------------------------------------------------*/

        *psiGsp0Code = g_quant_vl(swVoicingMode, pswWSVec, swPnShift,
                                  pswWPVec,
                                  ppswWVselpEx[0], ppswWVselpEx[1], snsRs00,
                                  snsRs11, snsRs22);

    }                                                            /* DTX mode */
    else                                                         /* DTX mode */
    {
        /* DTX mode */

        /* swSP == 0, currently in comfort noise insertion mode */ /* DTX mode */
        /*------------------------------------------------------*/ /* DTX mode */

        /* generate the random codevector */                       /* DTX mode */
        siNumBasisVecs = C_BITS_UV;                                /* DTX mode */

        /* build codevector 1 */                                   /* DTX mode */

        b_con(*psiVSCode1, siNumBasisVecs, pswBitArray);           /* DTX mode */
        v_con((INT16 *) pppsrUvCodeVec[0][0], ppswVselpEx[0],  /* DTX mode */
              pswBitArray, siNumBasisVecs);                        /* DTX mode */

        /* build codevector 2 */                                   /* DTX mode */

        b_con(*psiVSCode2, siNumBasisVecs, pswBitArray);           /* DTX mode */
        v_con((INT16 *) pppsrUvCodeVec[1][0], ppswVselpEx[1],  /* DTX mode */
              pswBitArray, siNumBasisVecs);                        /* DTX mode */


        /* get rs_rr for the two vectors */                        /* DTX mode */
        rs_rrNs(ppswVselpEx[0], snsSqrtRs, &snsRs11);              /* DTX mode */
        rs_rrNs(ppswVselpEx[1], snsSqrtRs, &snsRs22);              /* DTX mode */

    }                                                            /* DTX mode */


    /* Scale the excitations, each by its gain, and add them. Put the  */
    /* result at the end of the adaptive codebook (long-term-predictor */
    /* state)                                                          */
    /*-----------------------------------------------------------------*/

    if (swVoicingMode == 0)
    {

        /* unvoiced */
        /* -------- */

        scaleExcite(ppswVselpEx[0],
                    pppsrGsp0[swVoicingMode][*psiGsp0Code][0],
                    snsRs11, ppswVselpEx[0]);
        scaleExcite(ppswVselpEx[1],
                    pppsrGsp0[swVoicingMode][*psiGsp0Code][1],
                    snsRs22, ppswVselpEx[1]);

        /* now combine the two scaled excitations */
        /* -------------------------------------- */
        for (i = 0; i < S_LEN; i++)
            //pswTempVec[i] = ADD_SAT16(ppswVselpEx[0][i], ppswVselpEx[1][i]);
            pswTempVec[i] = ADD(ppswVselpEx[0][i], ppswVselpEx[1][i]);

    }

    else
    {

        /* voiced */
        /* ------ */

        scaleExcite(pswPVec,
                    pppsrGsp0[swVoicingMode][*psiGsp0Code][0],
                    snsRs00, pswPVec);
        scaleExcite(ppswVselpEx[0],
                    pppsrGsp0[swVoicingMode][*psiGsp0Code][1],
                    snsRs11, ppswVselpEx[0]);

        /* now combine the two scaled excitations */
        /* -------------------------------------- */
        for (i = 0; i < S_LEN; i++)
            //pswTempVec[i] = ADD_SAT16(pswPVec[i], ppswVselpEx[0][i]);
            pswTempVec[i] = ADD(pswPVec[i], ppswVselpEx[0][i]);

    }

    /* Update the long-term-predictor state using the synthetic excitation */
    /*---------------------------------------------------------------------*/

    for (i = -LTP_LEN; i < -S_LEN; i++)
        pswLtpStateOut[i] = pswLtpStateOut[i + S_LEN];

    for (i = -S_LEN, j = 0; j < S_LEN; i++, j++)
        pswLtpStateOut[i] = pswTempVec[j];

    /* Filter the synthetic excitation through the weighting filters, */
    /* H(z) and C(z), only to update filter states (Note that C(z)    */
    /* state may be updated without filtering, since it is an FIR)    */
    /* */
    /* First, perform one subframe's worth of delay on C(z) state     */
    /*----------------------------------------------------------------*/

    for (i = 0; i < HNW_BUFF_LEN - S_LEN; i++)
        pswHNWState[i] = pswHNWState[i + S_LEN];

    /* Second, perform H(z) filter on excitation, output goes into */
    /* C(z) state                                                  */
    /*-------------------------------------------------------------*/

    lpcIir(pswTempVec, pswHCoefs, pswHState,
           &pswHNWState[HNW_BUFF_LEN - S_LEN]);

}                                      /* end of sfrmAnalysis() */

/***************************************************************************
 *
 *   FUNCTION NAME: v_srch
 *
 *   PURPOSE:
 *     The purpose of this function is search a vector-sum codebook for the
 *     optimal vector
 *
 *   INPUTS:
 *
 *     pswWInput[0:S_LEN]
 *
 *                     the weighted input speech frame, with the zero-input
 *                     response of H(z) subtracted
 *
 *     pswWBasisVecs[0:S_LEN*siNumBasis]
 *
 *                     weighted, decorrelated vector-sum codebook basis
 *                     vectors
 *
 *     siNumBasis
 *
 *                     number of basis vectors
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     sw1
 *
 *                     output code
 *
 *   REFERENCE:  Sub-clause 4.1.10.2 of GSM Recommendation 06.20
 *
 *   KEYWORDS: v_srch, codebook, search
 *
 **************************************************************************/

INT16 v_srch(INT16 pswWInput[], INT16 pswWBasisVecs[],
             short int siNumBasis)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  V_ARRAY_SIZE   ( 1 << (C_BITS_V-1) )  - 1
#define  UN_ARRAY_SIZE  ( 1 << (C_BITS_UV-1) ) - 1
#define  MINUS_HALF     -0x4000

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */
    static INT16
    pswUpdateIndexV[V_ARRAY_SIZE] =
    {
        0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x1b, 0x00, 0x09,
        0x48, 0x5a, 0x00, 0x51, 0x48, 0x24, 0x00, 0x09, 0x48, 0x12,
        0x00, 0x51, 0x48, 0x63, 0x00, 0x09, 0x48, 0x5a, 0x00, 0x51,
        0x48, 0x2d, 0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x1b,
        0x00, 0x09, 0x48, 0x5a, 0x00, 0x51, 0x48, 0x6c, 0x00, 0x09,
        0x48, 0x12, 0x00, 0x51, 0x48, 0x63, 0x00, 0x09, 0x48, 0x5a,
        0x00, 0x51, 0x48, 0x36, 0x00, 0x09, 0x48, 0x12, 0x00, 0x51,
        0x48, 0x1b, 0x00, 0x09, 0x48, 0x5a, 0x00, 0x51, 0x48, 0x24,
        0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x63, 0x00, 0x09,
        0x48, 0x5a, 0x00, 0x51, 0x48, 0x75, 0x00, 0x09, 0x48, 0x12,
        0x00, 0x51, 0x48, 0x1b, 0x00, 0x09, 0x48, 0x5a, 0x00, 0x51,
        0x48, 0x6c, 0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x63,
        0x00, 0x09, 0x48, 0x5a, 0x00, 0x51, 0x48, 0x3f, 0x00, 0x09,
        0x48, 0x12, 0x00, 0x51, 0x48, 0x1b, 0x00, 0x09, 0x48, 0x5a,
        0x00, 0x51, 0x48, 0x24, 0x00, 0x09, 0x48, 0x12, 0x00, 0x51,
        0x48, 0x63, 0x00, 0x09, 0x48, 0x5a, 0x00, 0x51, 0x48, 0x2d,
        0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x1b, 0x00, 0x09,
        0x48, 0x5a, 0x00, 0x51, 0x48, 0x6c, 0x00, 0x09, 0x48, 0x12,
        0x00, 0x51, 0x48, 0x63, 0x00, 0x09, 0x48, 0x5a, 0x00, 0x51,
        0x48, 0x7e, 0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x1b,
        0x00, 0x09, 0x48, 0x5a, 0x00, 0x51, 0x48, 0x24, 0x00, 0x09,
        0x48, 0x12, 0x00, 0x51, 0x48, 0x63, 0x00, 0x09, 0x48, 0x5a,
        0x00, 0x51, 0x48, 0x75, 0x00, 0x09, 0x48, 0x12, 0x00, 0x51,
        0x48, 0x1b, 0x00, 0x09, 0x48, 0x5a, 0x00, 0x51, 0x48, 0x6c,
        0x00, 0x09, 0x48, 0x12, 0x00, 0x51, 0x48, 0x63, 0x00, 0x09,
        0x48, 0x5a, 0x00, 0x51, 0x48,
    },
    pswBitIndexV[V_ARRAY_SIZE] =
    {
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x06, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x07, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x06,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x07, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x06, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01,
    },
    pswModNextBitV[C_BITS_V] =
    {
        1, 2, 3, 4, 5, 6, 7, 0, 1,
    },
    pswUpdateIndexUn[UN_ARRAY_SIZE] =
    {
        0x00, 0x07, 0x2a, 0x0e, 0x00, 0x31, 0x2a, 0x15, 0x00, 0x07,
        0x2a, 0x38, 0x00, 0x31, 0x2a, 0x1c, 0x00, 0x07, 0x2a, 0x0e,
        0x00, 0x31, 0x2a, 0x3f, 0x00, 0x07, 0x2a, 0x38, 0x00, 0x31,
        0x2a, 0x23, 0x00, 0x07, 0x2a, 0x0e, 0x00, 0x31, 0x2a, 0x15,
        0x00, 0x07, 0x2a, 0x38, 0x00, 0x31, 0x2a, 0x46, 0x00, 0x07,
        0x2a, 0x0e, 0x00, 0x31, 0x2a, 0x3f, 0x00, 0x07, 0x2a, 0x38,
        0x00, 0x31, 0x2a,
    },
    pswBitIndexUn[UN_ARRAY_SIZE] =
    {
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02,
        0x01, 0x00, 0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x04,
        0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x05, 0x01, 0x02,
        0x01, 0x03, 0x01, 0x02, 0x01, 0x04, 0x01, 0x02, 0x01, 0x03,
        0x01, 0x02, 0x01,
    },
    pswModNextBitUV[C_BITS_UV] =
    {
        1, 2, 3, 4, 5, 0, 1,
    };

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 **pppswDubD[C_BITS_V - 1],
          *ppswD[C_BITS_V - 1],
          pswCGUpdates[2 * C_BITS_V * (C_BITS_V - 1)],
          pswDSpace[2 * C_BITS_V * (C_BITS_V - 1)],
          *ppswDPSpace[C_BITS_V * (C_BITS_V - 1)],
          pswBits[C_BITS_V - 1],
          *pswUpdatePtr,
          *pswUIndex,
          *pswBIndex,
          *pswModNextBit,
          *psw0,
          *psw1,
          *psw2,
          swC0,
          swG0,
          swCC,
          swG,
          swCCMax,
          swGMax,
          sw1;
    INT32 pL_R[C_BITS_V],
          L_R,
          L_MaxC,
          L_C0,
          L_D,
          L_G0,
          L_C,
          L_G,
          L_1;
    short int siI,
          siJ,
          siK,
          siEBits,
          siShiftCnt,
          siBitIndex,
          siBest,
          siMask;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    register UINT32 s_lo1=0;
    /*INT32 L_temp,temp;
    INT16 tempo,tempoi;*/

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* initialize variables based on voicing mode */
    /* ------------------------------------------ */

    if ( siNumBasis == C_BITS_V )
    {
        siEBits = C_BITS_V_1;
        pswUIndex = pswUpdateIndexV;
        pswBIndex = pswBitIndexV;
        pswModNextBit = pswModNextBitV;
    }
    else
    {
        siEBits = C_BITS_UV_1;
        pswUIndex = pswUpdateIndexUn;
        pswBIndex = pswBitIndexUn;
        pswModNextBit = pswModNextBitUV;
    }

    /* initialize pointers */
    /* ------------------- */

    for (siI = 0; siI < siNumBasis - 1; siI++)
    {

        pppswDubD[siI] = &ppswDPSpace[siI * siNumBasis];

        for (siJ = 0; siJ < siNumBasis; siJ++)
        {
            ppswDPSpace[(siI * siNumBasis) + siJ] =
                &pswDSpace[(siI * siNumBasis * 2) + (siJ * 2)];
        }
        ppswD[siI] = &pswDSpace[siI * siNumBasis];
    }

    /* compute correlations (Rm) between given vector and basis vectors,   */
    /* store in double precision; maintain max C for later scaling of Rm's */
    /* ------------------------------------------------------------------- */

    L_MaxC = 0L;

    for (siI = 0; siI < siNumBasis; siI++)
    {

        //L_R = L_MULT(pswWBasisVecs[siI * S_LEN], pswWInput[0]);
        //s_lo=SHR( L_R,1);
        VPP_MLX16(s_hi,s_lo, pswWBasisVecs[siI * S_LEN], pswWInput[0]);

        for (siJ = 1; siJ < S_LEN; siJ++)
        {
            //L_R = L_mac(L_R, pswWBasisVecs[siJ + (siI * S_LEN)], pswWInput[siJ]);
            VPP_MLA16(s_hi,s_lo, pswWBasisVecs[siJ + (siI * S_LEN)], pswWInput[siJ]);
        }
        L_R= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
        pL_R[siI] = L_R;
        L_R = L_ABS(L_R);
        //L_MaxC = L_ADD(L_R, L_MaxC);
        //L_MaxC =  ADD(L_R, L_MaxC);
        L_MaxC +=   L_R ;

    }

    /* normalize max C to get scaling shift count */
    /* scale Rm's and calculate C(0)              */
    /* ------------------------------------------ */

    /* max abs(C) after scale is <= 0.5 */
    //siShiftCnt = ADD_SAT16(-1, norm_l(L_MaxC));
    siShiftCnt = ADD(-1, norm_l(L_MaxC));

    L_C0 = 0L;

    for (siI = 0; siI < siNumBasis; siI++)
    {

        //L_R = L_shl(pL_R[siI], siShiftCnt);
        //L_R = L_SHL_SAT(pL_R[siI], siShiftCnt);
        L_R =  SHL(pL_R[siI], siShiftCnt);

        //L_C0 = L_SUB(L_C0, L_R);
        L_C0 -=  L_R ;

        //pL_R[siI] = L_shl(L_R, 1);
        //pL_R[siI] = L_SHL_SAT(L_R, 1);
        pL_R[siI] =  SHL (L_R, 1);

    }
    swC0 = EXTRACT_H(L_C0);

    /* compute correlations (Dmj, for m != j) between the basis vectors */
    /* store in double precision                                        */
    /* ---------------------------------------------------------------- */

    for (siI = 0; siI < siNumBasis - 1; siI++)
    {

        for (siJ = siI + 1; siJ < siNumBasis; siJ++)
        {

            //L_D = L_MULT(pswWBasisVecs[siI * S_LEN], pswWBasisVecs[siJ * S_LEN]);
            //s_lo=SHR( L_D,1);
            VPP_MLX16(s_hi,s_lo, pswWBasisVecs[siI * S_LEN], pswWBasisVecs[siJ * S_LEN]);
            for (siK = 1; siK < S_LEN; siK++)
            {
                //L_D = L_mac(L_D, pswWBasisVecs[siK + (siI * S_LEN)], pswWBasisVecs[siK + (siJ * S_LEN)]);
                VPP_MLA16(s_hi,s_lo, pswWBasisVecs[siK + (siI * S_LEN)], pswWBasisVecs[siK + (siJ * S_LEN)]);
            }
            L_D= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
            pppswDubD[siI][siJ][0] = EXTRACT_H(L_D);
            pppswDubD[siI][siJ][1] = EXTRACT_L(L_D);
        }
    }

    /* compute the sum of the Djj's (to be used for scaling the Dmj's and   */
    /* for computing G(0)); normalize it, get shift count for scaling Dmj's */
    /* -------------------------------------------------------------------- */

    psw1 = pswWBasisVecs;

    //L_G0 = L_MULT(psw1[0], psw1[0]);
    //s_lo=SHR( L_G0,1);
    VPP_MLX16(s_hi,s_lo, psw1[0], psw1[0]);
    for (siI = 1; siI < siNumBasis * S_LEN; siI++)
    {
        //L_G0 = L_mac(L_G0, psw1[siI], psw1[siI]);
        VPP_MLA16(s_hi,s_lo,  psw1[siI], psw1[siI]);
    }
    L_G0= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);
    //siShiftCnt = ADD_SAT16(-4, norm_l(L_G0));
    siShiftCnt = ADD(-4, norm_l(L_G0));

    //L_G0 = L_shl(L_G0, siShiftCnt);
    //L_G0 = L_SHL_SAT(L_G0, siShiftCnt);
    L_G0 <<= siShiftCnt ;

    /* scale Dmj's and compute G(0) */
    /* ---------------------------- */

    for (siI = 0; siI < siNumBasis - 1; siI++)
    {

        for (siJ = siI + 1; siJ < siNumBasis; siJ++)
        {

            L_D = L_DEPOSIT_H(pppswDubD[siI][siJ][0]);
            //L_D = L_ADD(L_D, (LSP_MASK & L_DEPOSIT_L(pppswDubD[siI][siJ][1])));
            L_D +=  (LSP_MASK & L_DEPOSIT_L(pppswDubD[siI][siJ][1])) ;

            //L_D = L_shl(L_D, siShiftCnt);

            //L_D = L_shl(L_D, 1);

            //L_D = L_SHL_SAT(L_D, siShiftCnt);

            //L_D = L_SHL_SAT(L_D, 1);

            L_D <<=  (siShiftCnt+1) ;

            //L_D <<=  1 ;

            //L_G0 = L_ADD(L_D, L_G0);
            L_G0 +=  L_D;

            //L_D = L_shl(L_D, 1);
            //L_D = L_SHL_SAT(L_D, 1);
            L_D <<=  1 ;

            ppswD[siI][siJ] = ROUNDO(L_D);
        }
    }

    swG0 = EXTRACT_H(L_G0);

    /* build array of update values for codebook search */
    /* ------------------------------------------------ */

    for (siI = 0; siI < siEBits; siI++)
    {
        pswCGUpdates[siI * (siEBits + 1)] = ROUNDO(pL_R[siI]);
    }
    psw0 = &pswCGUpdates[siEBits];
    psw1 = &pswCGUpdates[1];
    psw2 = &pswCGUpdates[2 * siEBits];

    for (siI = 0; siI < siEBits - 1; siI++)
    {

        for (siJ = siI + 1; siJ < siEBits; siJ++)
        {

            L_1 = L_DEPOSIT_H(ppswD[siI][siJ]);
            //L_1 = L_shl(L_1, 1);
            //L_1 = L_SHL_SAT(L_1, 1);
            L_1 <<=  1 ;
            psw1[siJ - 1 + (siI * siEBits)] = EXTRACT_H(L_1);
            psw2[siI + (siEBits * (siJ - 1))] = EXTRACT_H(L_1);
        }
        psw0[siI * (siEBits + 1)] = NEGATE(ppswD[siI][siEBits]);
    }

    psw0[siI * (siEBits + 1)] = NEGATE(ppswD[siEBits - 1][siEBits]);

    /* copy to negative side of array */
    /* ------------------------------ */

    psw0 = &pswCGUpdates[(siEBits + 1) * siEBits];

    for (siI = 0; siI < (siEBits + 1) * siEBits; siI++)
    {
        psw0[siI] = NEGATE(pswCGUpdates[siI]);
    }

    /* initialize array of bits (magnitude = 0.5) */
    /* ------------------------------------------ */

    for (siI = 0; siI < siEBits; siI++)
    {
        pswBits[siI] = MINUS_HALF;
    }

    /* initialize and do codebook search */
    /* --------------------------------- */

    swGMax = swG0;
    swCCMax = MULT_R(swC0, swC0);
    L_C = L_DEPOSIT_H(swC0);
    L_G = L_DEPOSIT_H(swG0);
    siBest = 0;
    s_lo=SHR( L_C,1);
    s_lo1=SHR( L_G,1);
    for (siI = 0; siI < (1 << siEBits) - 1; siI++)
    {

        pswUpdatePtr = &pswCGUpdates[pswUIndex[siI]];

        siBitIndex = pswBIndex[siI];

        //L_C = L_msu(L_C, pswUpdatePtr[0], 0x8000);
        VPP_MLA16(s_hi,s_lo,  -pswUpdatePtr[0], (INT16) 0x8000);

        for (siJ = 0; siJ < siEBits - 1; siJ++)
        {
            //L_G = L_mac(L_G, pswUpdatePtr[siJ + 1], pswBits[siBitIndex]);
            VPP_MLA16(s_hi,s_lo1, pswUpdatePtr[siJ + 1], pswBits[siBitIndex]);

            siBitIndex = pswModNextBit[siBitIndex];
        }
        //L_G = L_msu(L_G, pswUpdatePtr[siJ + 1], 0x8000);
        VPP_MLA16(s_hi,s_lo1,  -pswUpdatePtr[siJ + 1], (INT16) 0x8000);

        pswBits[siBitIndex] = NEGATE(pswBits[siBitIndex]);

        L_C= L_MLA_SAT_CARRY(s_lo, L_C&1);//L_MLA_SAT(s_hi,s_lo);
        sw1 = EXTRACT_H(L_C);
        swCC = MULT_R(sw1, sw1);

        L_G= L_MLA_SAT_CARRY(s_lo1, L_G&1);//L_MLA_SAT(s_hi,s_lo1);
        swG = EXTRACT_H(L_G);
        L_1 = L_MULT(swG, swCCMax);

        /*s_hi=0;
        s_lo=0;
        s_lo=SHR( L_1,1);*/
        //L_1 = L_msu(L_1, swGMax, swCC);
        L_1 = L_MSU(L_1, swGMax, swCC);
        //L_1 = L_MAC(L_1, -swGMax, swCC);
        //tempo=swCC;
        //tempoi=swGMax;
        //VPP_MLA16(s_hi,s_lo, - tempoi, tempo);
        //L_temp=L_MLA_SAT(s_hi,s_lo);

        /*if (L_temp!=L_1)
        {
            temp=0;
        }*/

        if (L_1 < 0)//(L_temp < 0)//
        {
            swCCMax = swCC;
            swGMax = swG;
            //siBest = ADD_SAT16(siI, 1);
            siBest = ADD(siI, 1);

        }
    }

    /* generate code for positive correlation;       */
    /* compute correlation, if negative, invert code */
    /* --------------------------------------------- */

    sw1 = siBest ^ (SHR(siBest, 1));

    siMask = 0x1;
    L_1 = 0L;

    for (siI = 0; siI < siNumBasis; siI++)
    {

        if ((sw1 & siMask) == 0)
        {
            //L_1 = L_SUB(L_1, pL_R[siI]);
            L_1 -=  pL_R[siI] ;

        }
        else
        {
            //L_1 = L_ADD(L_1, pL_R[siI]);
            L_1 +=   pL_R[siI] ;

        }

        //siMask = shl(siMask, 1);
        siMask = SHL(siMask, 1);

    }

    if (L_1 < 0)
    {
        //sw1 = sw1 ^ (SUB(shl(1, siNumBasis), 1));
        sw1 = sw1 ^ (SUB(SHL(1, siNumBasis), 1));

    }

    /* return code */
    /* ----------- */

    return (sw1);
}

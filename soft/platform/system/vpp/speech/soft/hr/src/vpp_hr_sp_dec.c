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






#include "hal_speech.h"
#include "vpp_hr_host.h"
#include "vpp_hr_typedefs.h"
#include "vpp_hr_mathhalf.h"
#include "vpp_hr_sp_rom.h"
#include "vpp_hr_sp_dec.h"
#include "vpp_hr_err_conc.h"
#include "vpp_hr_dtx.h"
#include "vpp_hr_mathhalf_macro.h"


//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"




/*_________________________________________________________________________
 |                                                                         |
 |            Local Functions (scope is limited to this file)              |
 |_________________________________________________________________________|
*/

static void a_sst(INT16 swAshift, INT16 swAscale,
                  INT16 pswDirectFormCoefIn[],
                  INT16 pswDirectFormCoefOut[]);

static short aToRc(INT16 swAshift, INT16 pswAin[],
                   INT16 pswRc[]);

static INT16 agcGain(INT16 pswStateCurr[],
                     struct NormSw snsInSigEnergy,
                     INT16 swEngyRShft);

static INT16 lagDecode(INT16 swDeltaLag);

static void lookupVq(INT16 pswVqCodeWds[], INT16 pswRCOut[]);

static void pitchPreFilt(INT16 pswExcite[],
                         INT16 swRxGsp0,
                         INT16 swRxLag,
                         INT16 swUvCode,
                         INT16 swSemiBeta,
                         struct NormSw snsSqrtRs,
                         INT16 pswExciteOut[],
                         INT16 pswPPreStatem[]);

static void spectralPostFilter(INT16 pswSPFIn[],
                               INT16 pswNumCoef[], INT16 pswDenomCoef[],
                               INT16 pswSPFOut[]);

/*_________________________________________________________________________
 |                                                                         |
 |                              Local Defines                              |
 |_________________________________________________________________________|
*/

#define  P_INT_MACS   10
#define  ASCALE       0x0800
#define  ASHIFT       4
#define  DELTA_LEVELS 16
#define  GSP0_SCALE   1
#define  C_BITS_V     9                /* number of bits in any voiced VSELP
                                        * codeword */
#define  C_BITS_UV    7                /* number of bits in a unvoiced VSELP
                                        * codeword */
#define  MAXBITS      C_BITS_V         /* max number of bits in any VSELP
                                        * codeword */
#define  LTP_LEN      147              /* 147==0x93 length of LTP history */
#define  SQRT_ONEHALF 0x5a82           /* the 0.5 ** 0.5 */
#define  LPC_ROUND    0x00000800L      /* 0x8000 >> ASHIFT */
#define  AFSHIFT      2                /* number of right shifts to be
                                        * applied to the autocorrelation
                                        * sequence in aFlatRcDp     */

/*_________________________________________________________________________
 |                                                                         |
 |                         State variables (globals)                       |
 |_________________________________________________________________________|
*/

INT16 gswPostFiltAgcGain,
      gpswPostFiltStateNum[NP],
      gpswPostFiltStateDenom[NP],
      swPostEmphasisState,
      pswSynthFiltState[NP],
      pswOldFrmKsDec[NP],
      pswOldFrmAsDec[NP],
      pswOldFrmPFNum[NP],
      pswOldFrmPFDenom[NP],
      swOldR0Dec,
      pswLtpStateBaseDec[LTP_LEN + S_LEN],
      pswPPreState[LTP_LEN + S_LEN];


INT16 swMuteFlagOld;             /* error concealment */


/* DTX state variables */
/* ------------------- */

INT16 swRxDTXState = CNINTPER - 1;        /* DTX State at the rx.
                                                 * Modulo */

/* counter [0,11].             */

INT16 swDecoMode = SPEECH;
INT16 swDtxMuting = 0;
INT16 swDtxBfiCnt = 0;

INT16 swOldR0IndexDec = 0;

INT16 swRxGsHistPtr = 0;
INT32 pL_RxGsHist[(OVERHANG - 1) * N_SUB];


/*_________________________________________________________________________
 |                                                                         |
 |                               Global Data                               |
 |                     (scope is global to this file)                      |
 |_________________________________________________________________________|
*/

INT16 swR0Dec;

INT16 swVoicingMode,             /* MODE */
      pswVq[3],                     /* LPC1, LPC2, LPC3 */
      swSi,                         /* INT_LPC */
      swEngyRShift;                 /* for use by spectral postfilter */


INT16 swR0NewCN;                 /* DTX mode */

extern INT32 ppLr_gsTable[4][32];       /* DTX mode */


/***************************************************************************
 *
 *   FUNCTION NAME: aFlatRcDp
 *
 *   PURPOSE:
 *
 *     Given a INT32 autocorrelation sequence, representing LPC
 *     information, aFlatRcDp converts the vector to one of NP
 *     INT16 reflection coefficients.
 *
 *   INPUT:
 *
 *
 *     pL_R[0:NP]    - An input INT32 autocorrelation sequence, (pL_R[0] =
 *                     not necessarily 0x7fffffffL).  pL_R is altered in the
 *                     call, by being right shifted by global constant
 *                     AFSHIFT bits.
 *
 *                     The input array pL_R[] should be shifted left as much
 *                     as possible to improve precision.
 *
 *     AFSHIFT       - The number of right shifts to be applied to the
 *                     normalized autocorrelation sequence pL_R.
 *
 *   OUTPUT:
 *
 *     pswRc[0:NP-1] - A INT16 output vector of NP reflection
 *                     coefficients.
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   DESCRIPTION:
 *
 *     This routine transforms LPC information from one set of
 *     parameters to another.  It is better suited for fixed point
 *     implementations than the Levinson-Dubin recursion.
 *
 *     The function is called by a_sst(), and getNWCoefs().  In a_sst()
 *     direct form coefficients are converted to autocorrelations,
 *     and smoothed in that domain.  Conversion back to direct form
 *     coefficients is done by calling aFlatRc(), followed by rcToADp().
 *
 *     In getNwCoefs() again the conversion back to direct form
 *     coefficients is done by calling aFlatRc(), followed by rcToADp().
 *     In getNwCoefs() an autocorrelation sequence is generated from the
 *     impulse response of the weighting filters.
 *
 *     The fundamental recursion is derived from AFLAT, which is
 *     described in section 4.1.4.1.
 *
 *     Unlike in AFLAT where the reflection coefficients are known, here
 *     they are the unknowns.  PBar and VBar for j==0 are initially
 *     known, as is rSub1.  From this information the next set of P's
 *     and V's are generated.  At the end of the recursion the next,
 *     reflection coefficient rSubj (pswRc[j]) can be calcluated by
 *     dividing Vsubj by Psubj.
 *
 *     Precision is crucial in this routine.  At each stage, a
 *     normalization is performed prior to the reflection coefficient
 *     calculation.  In addition, to prevent overflow, the
 *     autocorrelation sequence is scaled down by ASHIFT (4) right
 *     shifts.
 *
 *
 *   REFERENCES: Sub_Clause 4.1.9 and 4.2.1  of GSM Recomendation 06.20
 *
 *   KEYWORDS: reflection coefficients, AFLAT, aflat, recursion, LPC
 *   KEYWORDS: autocorrelation
 *
 *************************************************************************/

void   aFlatRcDp(INT32 *pL_R, INT16 *pswRc)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 pL_pjNewSpace[NP];
    INT32 pL_pjOldSpace[NP];
    INT32 pL_vjNewSpace[2 * NP - 1];
    INT32 pL_vjOldSpace[2 * NP - 1];

    INT32 *pL_pjOld;
    INT32 *pL_pjNew;
    INT32 *pL_vjOld;
    INT32 *pL_vjNew;
    INT32 *pL_swap;

    INT32 L_temp;
    INT32 L_sum;
    INT16 swRc,
          swRcSq,
          swTemp,
          swTemp1,
          swAbsTemp1,
          swTemp2;
    int    i,
           j;


    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    VPP_HR_PROFILE_FUNCTION_ENTER( aFlatRcDp);


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    pL_pjOld = pL_pjOldSpace;
    pL_pjNew = pL_pjNewSpace;
    pL_vjOld = pL_vjOldSpace + NP - 1;
    pL_vjNew = pL_vjNewSpace + NP - 1;


    /* Extract the 0-th reflection coefficient */
    /*-----------------------------------------*/

    swTemp1 = ROUNDO(pL_R[1]);
    swTemp2 = ROUNDO(pL_R[0]);
    //swAbsTemp1 = abs_s(swTemp1);
    swAbsTemp1 = ABS_S(swTemp1);
    //if (swTemp2 <= 0 || sub(swAbsTemp1, swTemp2) >= 0)
    if ((swTemp2 <= 0) || (swAbsTemp1>= swTemp2) )
    {
        j = 0;
        for (i = j; i < NP; i++)
        {
            pswRc[i] = 0;
        }
        VPP_HR_PROFILE_FUNCTION_EXIT( aFlatRcDp);
        return;
    }

    //swRc = divide_s(swAbsTemp1, swTemp2);/* return division result */
    swRc = DIVIDE_SAT16(swAbsTemp1, swTemp2);/* return division result */

    //if (sub(swTemp1, swAbsTemp1) == 0)
    if (swTemp1 == swAbsTemp1 )
        swRc = NEGATE(swRc);               /* negate reflection Rc[j] */

    pswRc[0] = swRc;                     /* copy into the output Rc array */

    for (i = 0; i <= NP; i++)
    {
        //pL_R[i] = L_shr(pL_R[i], AFSHIFT);
        pL_R[i] = SHR(pL_R[i], AFSHIFT);
    }

    /* Initialize the pjOld and vjOld recursion arrays */
    /*-------------------------------------------------*/

    for (i = 0; i < NP; i++)
    {
        pL_pjOld[i] = pL_R[i];
        pL_vjOld[i] = pL_R[i + 1];
    }
    for (i = -1; i > -NP; i--)
        pL_vjOld[i] = pL_R[-(i + 1)];


    /* Compute the square of the j=0 reflection coefficient */
    /*------------------------------------------------------*/

    swRcSq = MULT_R(swRc, swRc);

    /* Update pjNew and vjNew arrays for lattice stage j=1 */
    /*-----------------------------------------------------*/

    /* Updating pjNew: */
    /*-------------------*/

    for (i = 0; i <= NP - 2; i++)
    {
        L_temp = L_mpy_ls(pL_vjOld[i], swRc);
        //L_sum = L_ADD(L_temp, pL_pjOld[i]);
        L_sum =  ADD(L_temp, pL_pjOld[i]);
        L_temp = L_mpy_ls(pL_pjOld[i], swRcSq);
        //L_sum = L_ADD(L_temp, L_sum);
        L_sum =  ADD(L_temp, L_sum);
        L_temp = L_mpy_ls(pL_vjOld[-i], swRc);
        //pL_pjNew[i] = L_ADD(L_sum, L_temp);
        pL_pjNew[i] =  ADD(L_sum, L_temp);

    }

    /* Updating vjNew: */
    /*-------------------*/

    for (i = -NP + 2; i <= NP - 2; i++)
    {
        L_temp = L_mpy_ls(pL_vjOld[-i - 1], swRcSq);
        //L_sum = L_ADD(L_temp, pL_vjOld[i + 1]);
        L_sum =  ADD(L_temp, pL_vjOld[i + 1]);
        L_temp = L_mpy_ls(pL_pjOld[(((i + 1) >= 0) ? i + 1 : -(i + 1))], swRc);
        //L_temp = L_shl(L_temp, 1);
        L_temp = SHL(L_temp, 1);
        //pL_vjNew[i] = L_ADD(L_temp, L_sum);
        pL_vjNew[i] =  ADD(L_temp, L_sum);

    }



    j = 0;

    /* Compute reflection coefficients Rc[1],...,Rc[9] */
    /*-------------------------------------------------*/

    for (j = 1; j < NP; j++)
    {

        /* Swap pjNew and pjOld buffers */
        /*------------------------------*/

        pL_swap = pL_pjNew;
        pL_pjNew = pL_pjOld;
        pL_pjOld = pL_swap;

        /* Swap vjNew and vjOld buffers */
        /*------------------------------*/

        pL_swap = pL_vjNew;
        pL_vjNew = pL_vjOld;
        pL_vjOld = pL_swap;

        /* Compute the j-th reflection coefficient */
        /*-----------------------------------------*/

        swTemp = norm_l(pL_pjOld[0]);      /* get shift count */
        //swTemp1 = ROUNDO(L_shl(pL_vjOld[0], swTemp));        /* normalize num.  */
        //swTemp2 = ROUNDO(L_shl(pL_pjOld[0], swTemp));        /* normalize den.  */
        //swTemp1 = ROUNDO(L_SHL_SAT(pL_vjOld[0], swTemp));        /* normalize num.  */
        //swTemp2 = ROUNDO(L_SHL_SAT(pL_pjOld[0], swTemp));        /* normalize den.  */
        swTemp1 = ROUNDO(SHL(pL_vjOld[0], swTemp));        /* normalize num.  */
        swTemp2 = ROUNDO(SHL(pL_pjOld[0], swTemp));        /* normalize den.  */

        /* Test for invalid divide conditions: a) devisor < 0 b) abs(divident) >
         * abs(devisor) If either of these conditions is true, zero out
         * reflection coefficients for i=j,...,NP-1 and return. */

        //swAbsTemp1 = abs_s(swTemp1);
        swAbsTemp1 = ABS_S(swTemp1);
        if (swTemp2 <= 0 ||  (swAbsTemp1 >= swTemp2)  )
        {
            i = j;
            for (i = j; i < NP; i++)
            {
                pswRc[i] = 0;
            }
            VPP_HR_PROFILE_FUNCTION_EXIT( aFlatRcDp);
            return;
        }

        //swRc = divide_s(swAbsTemp1, swTemp2);       /* return division result */
        swRc = DIVIDE_SAT16(swAbsTemp1, swTemp2);       /* return division result */
        if ( swTemp1 == swAbsTemp1 )
            swRc = NEGATE(swRc);             /* negate reflection Rc[j] */
        swRcSq = MULT_R(swRc, swRc);       /* compute Rc^2 */
        /*VPP_MULT_R(m_hi, m_lo, swRc, swRc);
        swRcSq = EXTRACT_H( SHL(m_lo,1) );*/
        pswRc[j] = swRc;                   /* copy Rc[j] to output array */

        /* Update pjNew and vjNew arrays for the next lattice stage if j < NP-1 */
        /*---------------------------------------------------------------------*/

        /* Updating pjNew: */
        /*-----------------*/

        for (i = 0; i <= NP - j - 2; i++)
        {
            L_temp = L_mpy_ls(pL_vjOld[i], swRc);
            //L_sum = L_ADD(L_temp, pL_pjOld[i]);
            L_sum =  ADD(L_temp, pL_pjOld[i]);
            L_temp = L_mpy_ls(pL_pjOld[i], swRcSq);
            //L_sum = L_ADD(L_temp, L_sum);
            L_sum =  ADD(L_temp, L_sum);
            L_temp = L_mpy_ls(pL_vjOld[-i], swRc);
            //pL_pjNew[i] = L_ADD(L_sum, L_temp);
            pL_pjNew[i] =  ADD(L_sum, L_temp);

        }

        /* Updating vjNew: */
        /*-----------------*/

        for (i = -NP + j + 2; i <= NP - j - 2; i++)
        {
            L_temp = L_mpy_ls(pL_vjOld[-i - 1], swRcSq);
            //L_sum = L_ADD(L_temp, pL_vjOld[i + 1]);
            L_sum =  ADD(L_temp, pL_vjOld[i + 1]);
            L_temp = L_mpy_ls(pL_pjOld[(((i + 1) >= 0) ? i + 1 : -(i + 1))], swRc);
            //L_temp = L_shl(L_temp, 1);
            //L_temp = L_SHL_SAT(L_temp, 1);
            L_temp <<=1 ;
            //pL_vjNew[i] = L_ADD(L_temp, L_sum);
            pL_vjNew[i] = ADD(L_temp, L_sum);

        }
    }


    VPP_HR_PROFILE_FUNCTION_EXIT(aFlatRcDp );




    return;
}

/***************************************************************************
 *
 *   FUNCTION NAME: aToRc
 *
 *   PURPOSE:
 *
 *     This subroutine computes a vector of reflection coefficients, given
 *     an input vector of direct form LPC filter coefficients.
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the LPC filter (global constant)
 *
 *     swAshift
 *                     The number of right shifts applied externally
 *                     to the direct form filter coefficients.
 *
 *     pswAin[0:NP-1]
 *                     The input vector of direct form LPC filter
 *                     coefficients.
 *
 *   OUTPUTS:
 *
 *     pswRc[0:NP-1]
 *                     Array containing the reflection coefficients.
 *
 *   RETURN VALUE:
 *
 *     siUnstableFlt
 *                     If stable reflection coefficients 0, 1 if unstable.
 *
 *
 *   DESCRIPTION:
 *
 *     This function performs the conversion from direct form
 *     coefficients to reflection coefficients. It is used in a_sst()
 *     and interpolateCheck().  In a_sst() reflection coefficients used
 *     as a transitional data format.  aToRc() is used for this
 *     conversion.
 *
 *     When performing interpolation, a stability check must be
 *     performed. interpolateCheck() does this by calling aToRc().
 *
 *     First coefficients are shifted down by iAshift. NP, the filter
 *     order is 10. The a's and rc's each have NP elements in them. An
 *     elaborate algorithm description can be found on page 443, of
 *     "Digital Processing of Speech Signals" by L.R. Rabiner and R.W.
 *     Schafer; Prentice-Hall; Englewood Cliffs, NJ (USA).  1978.
 *
 *   REFERENCES: Sub_Clause 4.1.6, and 4.2.3 of GSM Recomendation 06.20
 *
 *   KEYWORDS: reflectioncoefficients, parcors, conversion, atorc, ks, as
 *   KEYWORDS: parcorcoefficients, lpc, flat, vectorquantization
 *
 *************************************************************************/

static short aToRc(INT16 swAshift, INT16 pswAin[],
                   INT16 pswRc[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Constants                                    |
     |_________________________________________________________________________|
    */

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswTmpSpace[NP],
          pswASpace[NP],
          swNormShift,
          swActShift,
          swNormProd,
          swRcOverE,
          swDiv,
          *pswSwap,
          *pswTmp,
          *pswA;

    INT32 L_temp;

    short int siUnstableFlt,
          i,
          j;                            /* Loop control variables */


    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Initialize starting addresses for temporary buffers */
    /*-----------------------------------------------------*/

    pswA = pswASpace;
    pswTmp = pswTmpSpace;

    /* Copy the direct form filter coefficients to a temporary array */
    /*---------------------------------------------------------------*/

    for (i = 0; i < NP; i++)
    {
        pswA[i] = pswAin[i];
    }

    /* Initialize the flag for filter stability check */
    /*------------------------------------------------*/

    siUnstableFlt = 0;

    /* Start computation of the reflection coefficients, Rc[9],...,Rc[1] */
    /*-------------------------------------------------------------------*/

    for (i = NP - 1; i >= 1; i--)
    {

        pswRc[i] = SHL(pswA[i], swAshift); /* write Rc[i] to output array */

        /* Check the stability of i-th reflection coefficient */
        /*----------------------------------------------------*/

        siUnstableFlt = siUnstableFlt | isSwLimit(pswRc[i]);

        /* Precompute intermediate variables for needed for the computation */
        /* of direct form filter of order i-1                               */
        /*------------------------------------------------------------------*/

        if ( pswRc[i] == SW_MIN )
        {
            siUnstableFlt = 1;
            swRcOverE = 0;
            swDiv = 0;
            swActShift = 2;
        }
        else
        {
            //L_temp = LW_MAX;                 /* Load ~1.0 into accum */
            s_lo=0x3FFFFFFF;//SHR(LW_MAX ,1);
            //L_temp = L_msu(L_temp, pswRc[i], pswRc[i]);       /* 1.-Rc[i]*Rc[i]  */
            VPP_MLA16(s_hi,s_lo, -pswRc[i], pswRc[i]);
            L_temp = L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);
            swNormShift = norm_l(L_temp);
            //L_temp = L_shl(L_temp, swNormShift);
            //L_temp = L_SHL_SAT(L_temp, swNormShift);
            L_temp <<=  swNormShift;
            //swNormProd = extract_h(L_temp);
            swNormProd = EXTRACT_H(L_temp);
            //swActShift = ADD_SAT16(2, swNormShift);
            swActShift = ADD(2, swNormShift);
            //swDiv = divide_s(0x2000, swNormProd);
            swDiv = DIVIDE_SAT16(0x2000, swNormProd);
            swRcOverE = MULT_R(pswRc[i], swDiv);
            /*VPP_MULT_R(m_hi, m_lo, pswRc[i], swDiv);
            swRcOverE = EXTRACT_H( SHL(m_lo,1) );*/
        }
        /* Check stability   */
        /*---------------------*/
        siUnstableFlt = siUnstableFlt | isSwLimit(swRcOverE);

        /* Compute direct form filter coefficients corresponding to */
        /* a direct form filter of order i-1                        */
        /*----------------------------------------------------------*/

        for (j = 0; j <= i - 1; j++)
        {
            //L_temp = L_MULT(pswA[j], swDiv);
            //s_lo=SHR(L_temp ,1);
            //L_temp = L_msu(L_temp, pswA[i - j - 1], swRcOverE);
            VPP_MLX16(s_hi, s_lo, pswA[j], swDiv);
            VPP_MLA16(s_hi,s_lo, -pswA[i - j - 1], swRcOverE);
            L_temp =  L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);

            //L_temp = L_shl(L_temp, swActShift);
            //L_temp = L_SHL_SAT(L_temp, swActShift);
            L_temp <<=  swActShift;
            pswTmp[j] = ROUNDO(L_temp);
            siUnstableFlt = siUnstableFlt | isSwLimit(pswTmp[j]);
        }

        /* Swap swA and swTmp buffers */
        /*----------------------------*/

        pswSwap = pswA;
        pswA = pswTmp;
        pswTmp = pswSwap;
    }

    /* Compute reflection coefficient Rc[0] */
    /*--------------------------------------*/

    pswRc[0] = SHL(pswA[0], swAshift);   /* write Rc[0] to output array */

    /* Check the stability of 0-th reflection coefficient */
    /*----------------------------------------------------*/

    siUnstableFlt = siUnstableFlt | isSwLimit(pswRc[0]);

    return (siUnstableFlt);
}

/***************************************************************************
 *
 *   FUNCTION NAME: a_sst
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform spectral smoothing of the
 *     direct form filter coefficients
 *
 *   INPUTS:
 *
 *     swAshift
 *                     number of shift for coefficients
 *
 *     swAscale
 *                     scaling factor for coefficients
 *
 *     pswDirectFormCoefIn[0:NP-1]
 *
 *                     array of input direct form coefficients
 *
 *   OUTPUTS:
 *
 *     pswDirectFormCoefOut[0:NP-1]
 *
 *                     array of output direct form coefficients
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     In a_sst() direct form coefficients are converted to
 *     autocorrelations, and smoothed in that domain.  The function is
 *     used in the spectral postfilter.  A description can be found in
 *     section 3.2.4 as well as in the reference by Y. Tohkura et al.
 *     "Spectral Smoothing Technique in PARCOR Speech
 *     Analysis-Synthesis", IEEE Trans. ASSP, vol. ASSP-26, pp. 591-596,
 *     Dec. 1978.
 *
 *     After smoothing is performed conversion back to direct form
 *     coefficients is done by calling aFlatRc(), followed by rcToADp().
 *
 *     The spectral smoothing filter coefficients with bandwidth set to 300
 *     and a sampling rate of 8000 be :
 *     static INT16 psrSST[NP+1] = { 0x7FFF,
 *         0x7F5C, 0x7D76, 0x7A5B, 0x7622, 0x70EC,
 *         0x6ADD, 0x641F, 0x5CDD, 0x5546, 0x4D86
 *     }
 *
 *   REFERENCES: Sub_Clause 4.2.4 of GSM Recomendation 06.20
 *
 *   KEYWORDS: spectral smoothing, direct form coef, sst, atorc, atocor
 *   KEYWORDS: levinson
 *
 *************************************************************************/

static void a_sst(INT16 swAshift, INT16 swAscale,
                  INT16 pswDirectFormCoefIn[],
                  INT16 pswDirectFormCoefOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */

    static INT16 psrSST[NP + 1] = {0x7FFF,
                                   0x7F5C, 0x7D76, 0x7A5B, 0x7622, 0x70EC,
                                   0x6ADD, 0x641F, 0x5CDD, 0x5546, 0x4D86,
                                  };

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 pL_CorrTemp[NP + 1];

    INT16 pswRCNum[NP],
          pswRCDenom[NP];

    short int siLoopCnt;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* convert direct form coefs to reflection coefs */
    /* --------------------------------------------- */

    aToRc(swAshift, pswDirectFormCoefIn, pswRCDenom);

    /* convert to autocorrelation coefficients */
    /* --------------------------------------- */

    rcToCorrDpL(swAshift, swAscale, pswRCDenom, pL_CorrTemp);

    /* do spectral smoothing technique */
    /* ------------------------------- */

    for (siLoopCnt = 1; siLoopCnt <= NP; siLoopCnt++)
    {
        pL_CorrTemp[siLoopCnt] = L_mpy_ls(pL_CorrTemp[siLoopCnt],
                                          psrSST[siLoopCnt]);
    }

    /* Compute the reflection coefficients via AFLAT */
    /*-----------------------------------------------*/

    aFlatRcDp(pL_CorrTemp, pswRCNum);


    /* Convert reflection coefficients to direct form filter coefficients */
    /*-------------------------------------------------------------------*/

    rcToADp(swAscale, pswRCNum, pswDirectFormCoefOut);
}

/**************************************************************************
 *
 *   FUNCTION NAME: agcGain
 *
 *   PURPOSE:
 *
 *     Figure out what the agc gain should be to make the energy in the
 *     output signal match that of the input signal.  Used in the post
 *     filters.
 *
 *   INPUT:
 *
 *      pswStateCurr[0:39]
 *                     Input signal into agc block whose energy is
 *                     to be modified using the gain returned. Signal is not
 *                     modified in this routine.
 *
 *      snsInSigEnergy
 *                     Normalized number with shift count - the energy in
 *                     the input signal.
 *
 *      swEngyRShft
 *                     Number of right shifts to apply to the vectors energy
 *                     to ensure that it remains less than 1.0
 *                     (swEngyRShft is always positive or zero)
 *
 *   OUTPUT:
 *
 *      none
 *
 *   RETURN:
 *
 *      the agc's gain/2 note DIVIDED by 2
 *
 *
 *   REFERENCES: Sub_Clause 4.2.2 and 4.2.4 of GSM Recomendation 06.20
 *
 *   KEYWORDS: postfilter, agc, automaticgaincontrol, leveladjust
 *
 *************************************************************************/

static INT16 agcGain(INT16 pswStateCurr[],
                     struct NormSw snsInSigEnergy, INT16 swEngyRShft)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_OutEnergy,
          L_AgcGain;

    struct NormSw snsOutEnergy,
               snsAgc;

    INT16 swAgcOut,
          swAgcShftCnt;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Calculate the energy in the output vector divided by 2 */
    /*--------------------------------------------------------*/

    snsOutEnergy.sh = g_corr1s(pswStateCurr, swEngyRShft, &L_OutEnergy);

    /* reduce energy by a factor of 2 */
    snsOutEnergy.sh++;

    /* if waveform has nonzero energy, find AGC gain */
    /*-----------------------------------------------*/

    if (L_OutEnergy == 0)
    {
        swAgcOut = 0;
    }
    else
    {

        snsOutEnergy.man = ROUNDO(L_OutEnergy);

        /* divide input energy by 2 */
        snsInSigEnergy.man = SHR(snsInSigEnergy.man, 1);


        /* Calculate AGC gain squared */
        /*----------------------------*/

        //snsAgc.man = divide_s(snsInSigEnergy.man, snsOutEnergy.man);
        snsAgc.man = DIVIDE_SAT16(snsInSigEnergy.man, snsOutEnergy.man);
        swAgcShftCnt = norm_s(snsAgc.man);
        snsAgc.man = SHL(snsAgc.man, swAgcShftCnt);

        /* find shift count for G^2 */
        /*--------------------------*/

        //snsAgc.sh = ADD_SAT16(sub(snsInSigEnergy.sh, snsOutEnergy.sh), swAgcShftCnt);
        //snsAgc.sh = ADD(sub(snsInSigEnergy.sh, snsOutEnergy.sh), swAgcShftCnt);
        snsAgc.sh = ADD(SUB(snsInSigEnergy.sh, snsOutEnergy.sh), swAgcShftCnt);
        L_AgcGain = L_DEPOSIT_H(snsAgc.man);


        /* Calculate AGC gain */
        /*--------------------*/

        snsAgc.man = sqroot(L_AgcGain);


        /* check if 1/2 sqrt(G^2) >= 1.0                      */
        /* This is equivalent to checking if shiftCnt/2+1 < 0 */
        /*----------------------------------------------------*/

        //if (ADD_SAT16(snsAgc.sh, 2) < 0)
        if ((snsAgc.sh + 2) < 0)
        {
            swAgcOut = SW_MAX;
        }
        else
        {

            if (0x1 & snsAgc.sh)
            {
                snsAgc.man = MULT(snsAgc.man, SQRT_ONEHALF);
            }

            snsAgc.sh = SHR(snsAgc.sh, 1);   /* shiftCnt/2 */
            //snsAgc.sh = ADD_SAT16(snsAgc.sh, 1);   /* shiftCnt/2 + 1 */
            snsAgc.sh++;

            if (snsAgc.sh > 0)
            {
                snsAgc.man = SHR(snsAgc.man, snsAgc.sh);
            }
            swAgcOut = snsAgc.man;
        }
    }

    return (swAgcOut);
}

/***************************************************************************
 *
 *   FUNCTION NAME: b_con
 *
 *   PURPOSE:
 *     Expands codeword into an one dimensional array. The 0/1 input is
 *     changed to an element with magnitude +/- 0.5.
 *
 *     input  output
 *
 *       0    -0.5
 *       1    +0.5
 *
 *   INPUT:
 *
 *      swCodeWord
 *                     Input codeword, information in the LSB's
 *
 *      siNumBits
 *                     number of bits in the input codeword and number
 *                     of elements in output vector
 *
 *      pswVectOut[0:siNumBits]
 *
 *                     pointer to bit array
 *
 *   OUTPUT:
 *
 *      pswVectOut[0:siNumBits]
 *
 *                     signed bit array
 *
 *   RETURN:
 *
 *      none
 *
 *   REFERENCES: Sub_Clause 4.1.10 and 4.2.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: b_con, codeword, expansion
 *
 *************************************************************************/

void   b_con(INT16 swCodeWord, short siNumBits,
             INT16 pswVectOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int siLoopCnt;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    for (siLoopCnt = 0; siLoopCnt < siNumBits; siLoopCnt++)
    {

        if (swCodeWord & 1)                /* temp accumulator get 0.5 */
            pswVectOut[siLoopCnt] = (INT16) 0x4000;
        else                               /* temp accumulator gets -0.5 */
            pswVectOut[siLoopCnt] = (INT16) 0xc000;

        swCodeWord = SHR(swCodeWord, 1);
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: fp_ex
 *
 *   PURPOSE:
 *
 *     Looks up a vector in the adaptive excitation codebook (long-term
 *     predictor).
 *
 *   INPUTS:
 *
 *     swOrigLagIn
 *
 *                     Extended resolution lag (lag * oversampling factor)
 *
 *     pswLTPState[-147:39]
 *
 *                     Adaptive codebook (with space at end for looked up
 *                     vector).  Indicies [-147:-1] are the history, [0:39]
 *                     are for the looked up vector.
 *
 *     psrPitchIntrpFirBase[0:59]
 *     ppsrPVecIntFilt[0:9][0:5] ([tap][phase])
 *
 *                     Interpolating FIR filter coefficients.
 *
 *   OUTPUTS:
 *
 *     pswLTPState[0:39]
 *
 *                     Array containing the contructed output vector
 *
 *   RETURN VALUE:
 *     none
 *
 *   DESCRIPTION:
 *
 *     The adaptive codebook consists of the history of the excitation.
 *     The vector is looked up by going back into this history
 *     by the amount of the input lag.  If the input lag is fractional,
 *     then the samples to be looked up are interpolated from the existing
 *     samples in the history.
 *
 *     If the lag is less than the length of the vector to be generated
 *     (i.e. less than the subframe length), then the lag is doubled
 *     after the first n samples have been looked up (n = input lag).
 *     In this way, the samples being generated are not part of the
 *     codebook.  This is described in section 4.1.8.
 *
 *   REFERENCES: Sub_Clause 4.1.8.5 and 4.2.1  of GSM Recomendation 06.20
 *
 *   Keywords: pitch, excitation vector, long term filter, history,
 *   Keywords: fractional lag, get_ipjj
 *
 *************************************************************************/



void   fp_ex(INT16 swOrigLagIn,
             INT16 pswLTPState[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Temp;//,Lmult
    INT16 swIntLag,
          swRemain,
          swRunningLag;
    short int siSampsSoFar,
          siSampsThisPass,
          i,
          j;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Loop: execute until all samples in the vector have been looked up */
    /*-------------------------------------------------------------------*/

    swRunningLag = swOrigLagIn;
    siSampsSoFar = 0;
    while (siSampsSoFar < S_LEN)
    {

        /* Get integer lag and remainder.  These are used in addressing */
        /* the LTP state and the interpolating filter, respectively     */
        /*--------------------------------------------------------------*/

        get_ipjj(swRunningLag, &swIntLag, &swRemain);


        /* Get the number of samples to look up in this pass */
        /*---------------------------------------------------*/

        if ( swIntLag < S_LEN  )
            siSampsThisPass = swIntLag - siSampsSoFar;
        else
            siSampsThisPass = S_LEN - siSampsSoFar;

        /* Look up samples by interpolating (fractional lag), or copying */
        /* (integer lag).                                                */
        /*---------------------------------------------------------------*/

        if (swRemain == 0)
        {

            /* Integer lag: copy samples from history */
            /*----------------------------------------*/

            for (i = siSampsSoFar; i < siSampsSoFar + siSampsThisPass; i++)
                pswLTPState[i] = pswLTPState[i - swIntLag];
        }
        else
        {

            /* Fractional lag: interpolate to get samples */
            /*--------------------------------------------*/

            for (i = siSampsSoFar; i < siSampsSoFar + siSampsThisPass; i++)
            {

                /* first tap with rounding offset */
                /*--------------------------------*/
                /*L_Temp = L_mac((long) 32768,
                               pswLTPState[i - swIntLag - P_INT_MACS / 2],
                               ppsrPVecIntFilt[0][swRemain]);

                Lmult=L_MULT(pswLTPState[i - swIntLag - P_INT_MACS / 2],
                               ppsrPVecIntFilt[0][swRemain]);
                L_Temp=L_ADD((long) 32768,Lmult);*/

                s_lo=0x4000;//SHR( 32768,1);
                VPP_MLA16(s_hi, s_lo, pswLTPState[i - swIntLag - P_INT_MACS / 2], ppsrPVecIntFilt[0][swRemain]);



                for (j = 1; j < P_INT_MACS - 1; j++)
                {

                    /*L_Temp = L_mac(L_Temp,
                                   pswLTPState[i - swIntLag - P_INT_MACS / 2 + j],
                                   ppsrPVecIntFilt[j][swRemain]);

                    Lmult=L_MULT(pswLTPState[i - swIntLag - P_INT_MACS / 2 + j],
                                   ppsrPVecIntFilt[j][swRemain]);
                    L_Temp=L_ADD(L_Temp,Lmult);*/
                    VPP_MLA16(s_hi, s_lo, pswLTPState[i - swIntLag - P_INT_MACS / 2 + j], ppsrPVecIntFilt[j][swRemain]);

                }

                /*pswLTPState[i] = extract_h(L_mac(L_Temp,
                                     pswLTPState[i - swIntLag + P_INT_MACS / 2 - 1],
                                        ppsrPVecIntFilt[P_INT_MACS - 1][swRemain]));*/

                VPP_MLA16(s_hi, s_lo, pswLTPState[i - swIntLag + P_INT_MACS / 2 - 1], ppsrPVecIntFilt[P_INT_MACS - 1][swRemain]);
                L_Temp=  L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi, s_lo);

                pswLTPState[i] = EXTRACT_H(L_Temp);

                /*pswLTPState[i] = EXTRACT_H(L_mac(L_Temp,
                                     pswLTPState[i - swIntLag + P_INT_MACS / 2 - 1],
                                        ppsrPVecIntFilt[P_INT_MACS - 1][swRemain]));*/


            }
        }

        /* Done with this pass: update loop controls */
        /*-------------------------------------------*/

        siSampsSoFar += siSampsThisPass;
        //swRunningLag = ADD_SAT16(swRunningLag, swOrigLagIn);
        swRunningLag = ADD(swRunningLag, swOrigLagIn);

    }
}

/***************************************************************************
 *
 *    FUNCTION NAME: g_corr1 (no scaling)
 *
 *    PURPOSE:
 *
 *     Calculates energy in subframe vector.  Differs from g_corr1s,
 *     in that there the estimate of the maximum possible
 *     energy is < 1.0
 *
 *
 *    INPUT:
 *
 *       pswIn[0:39]
 *                     A subframe vector.
 *
 *
 *    OUTPUT:
 *
 *       *pL_out
 *                     A INT32 containing the normalized energy
 *                     in the input vector.
 *
 *    RETURN:
 *
 *       swOut
 *                     Number of right shifts which the accumulator was
 *                     shifted to normalize it.  Negative number implies
 *                     a left shift, and therefore an energy larger than
 *                     1.0.
 *
 *    REFERENCES: Sub_Clause 4.1.10.2 and 4.2.1 of GSM Recomendation 06.20
 *
 *    KEYWORDS: energy, autocorrelation, correlation, g_corr1
 *
 *
 *************************************************************************/

INT16 g_corr1(INT16 *pswIn, INT32 *pL_out)
{


    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_sum;//,Lmult
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

    //L_sum = L_MULT(pswIn[0], pswIn[0]);
    //s_lo=SHR( L_sum,1);
    VPP_MLX16(s_hi, s_lo, pswIn[0], pswIn[0]);
    for (i = 1; i < S_LEN; i++)
    {
        //L_sum = L_mac(L_sum, pswIn[i], pswIn[i]);
        /*Lmult=L_MULT(pswIn[i], pswIn[i]);
        L_sum=L_ADD(L_sum,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswIn[i], pswIn[i]);

    }
    L_sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);


    if (L_sum != 0)
    {

        /* Normalize the energy in the output INT32 */
        /*---------------------------------------------*/

        swEngyLShft = norm_l(L_sum);
        //*pL_out = L_shl(L_sum, swEngyLShft);        /* normalize output INT32 */
        //*pL_out = L_SHL_SAT(L_sum, swEngyLShft);        /* normalize output INT32 */
        *pL_out =  SHL (L_sum, swEngyLShft);        /* normalize output INT32 */

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
 *    FUNCTION NAME: g_corr1s (g_corr1 with scaling)
 *
 *    PURPOSE:
 *
 *     Calculates energy in subframe vector.  Differs from g_corr1,
 *     in that there is an estimate of the maximum possible energy in the
 *     vector.
 *
 *    INPUT:
 *
 *       pswIn[0:39]
 *                     A subframe vector.
 *
 *       swEngyRShft
 *
 *                     Number of right shifts to apply to the vectors energy
 *                     to ensure that it remains less than 1.0
 *                     (swEngyRShft is always positive or zero)
 *
 *    OUTPUT:
 *
 *       *pL_out
 *                     A INT32 containing the normalized energy
 *                     in the input vector.
 *
 *    RETURN:
 *
 *       swOut
 *                     Number of right shifts which the accumulator was
 *                     shifted to normalize it.  Negative number implies
 *                     a left shift, and therefore an energy larger than
 *                     1.0.
 *
 *    REFERENCES: Sub-Clause 4.1.8, 4.2.1, 4.2.2, and 4.2.4
 *                of GSM Recomendation 06.20
 *
 *    keywords: energy, autocorrelation, correlation, g_corr1
 *
 *
 *************************************************************************/

INT16 g_corr1s(INT16 pswIn[], INT16 swEngyRShft,
               INT32 *pL_out)
{


    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_sum;//,Lmult
    INT16 swTemp,
          swEngyLShft;
    INT16 swInputRShft;

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

    //if (sub(swEngyRShft, 1) <= 0)
    if ( swEngyRShft <= 1 )
    {

        /* use the energy shift factor, although it is an odd shift count */
        /*----------------------------------------------------------------*/

        //swTemp = shr(pswIn[0], swEngyRShft);
        swTemp = SHR_V(pswIn[0], swEngyRShft);
        //L_sum = L_MULT(pswIn[0], swTemp);
        //s_lo=SHR(L_sum ,1);
        VPP_MLX16(s_hi, s_lo, pswIn[0], swTemp);
        for (i = 1; i < S_LEN; i++)
        {
            //swTemp = shr(pswIn[i], swEngyRShft);
            swTemp = SHR_V(pswIn[i], swEngyRShft);
            //L_sum = L_mac(L_sum, pswIn[i], swTemp);
            /*Lmult=L_MULT(pswIn[i], swTemp);
            L_sum=L_ADD(L_sum,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswIn[i], swTemp);

        }

    }
    else
    {

        /* convert energy shift factor to an input shift factor */
        /*------------------------------------------------------*/

        swInputRShft = SHIFT_R(swEngyRShft, -1);
        //swInputRShft = ADD((SHR(swEngyRShft, 1)), (swEngyRShft & 0x1));
        swEngyRShft = SHL(swInputRShft, 1);

        //swTemp = shr(pswIn[0], swInputRShft);
        swTemp = SHR(pswIn[0], swInputRShft);
        //L_sum = L_MULT(swTemp, swTemp);
        //s_lo=SHR(L_sum ,1);
        VPP_MLX16(s_hi, s_lo, swTemp, swTemp);
        for (i = 1; i < S_LEN; i++)
        {
            //swTemp = shr(pswIn[i], swInputRShft);
            swTemp = SHR(pswIn[i], swInputRShft);
            //L_sum = L_mac(L_sum, swTemp, swTemp);
            /*Lmult=L_MULT(swTemp, swTemp);
            L_sum=L_ADD(L_sum,Lmult);*/
            VPP_MLA16(s_hi, s_lo, swTemp, swTemp);
        }
    }

    L_sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    if (L_sum != 0)
    {

        /* Normalize the energy in the output INT32 */
        /*---------------------------------------------*/

        swTemp = norm_l(L_sum);
        //*pL_out = L_shl(L_sum, swTemp);    /* normalize output INT32 */
        //*pL_out = L_SHL_SAT(L_sum, swTemp);    /* normalize output INT32 */
        *pL_out =  SHL (L_sum, swTemp);    /* normalize output INT32 */
        //swEngyLShft = sub(swTemp, swEngyRShft);
        swEngyLShft = SUB(swTemp, swEngyRShft);

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
 *   FUNCTION NAME: getSfrmLpc
 *
 *   PURPOSE:
 *
 *     Given frame information from past and present frame, interpolate
 *     (or copy) the frame based LPC coefficients into subframe
 *     lpc coeffs, i.e. the ones which will be used by the subframe
 *     as opposed to those coded and transmitted.
 *
 *   INPUTS:
 *
 *     siSoftInterpolation
 *
 *                     interpolate 1/0, a coded parameter.
 *
 *     swPrevR0,swNewR0
 *
 *                     Rq0 for the last frame and for this frame.
 *                     These are the decoded values, not the codewords.
 *
 *     Previous lpc coefficients from the previous frame:
 *       in all filters below array[0] is the t=-1 element array[9]
 *       t=-10 element.
 *
 *     pswPrevFrmKs[0:9]
 *
 *                     decoded version of the rc's tx'd last frame
 *
 *     pswPrevFrmAs[0:9]
 *
 *                     the above K's converted to A's.  i.e. direct
 *                     form coefficients.
 *
 *     pswPrevFrmPFNum[0:9], pswPrevFrmPFDenom[0:9]
 *
 *                     numerator and denominator coefficients used in the
 *                     postfilter
 *
 *     Current lpc coefficients from the current frame:
 *
 *     pswNewFrmKs[0:9], pswNewFrmAs[0:9],
 *     pswNewFrmPFNum[0:9], pswNewFrmPFDenom[0:9] same as above.
 *
 *   OUTPUTS:
 *
 *     psnsSqrtRs[0:3]
 *
 *                      a normalized number (struct NormSw)
 *                      containing an estimate of RS for each subframe.
 *                      (number and a shift)
 *
 *     ppswSynthAs[0:3][0:9]
 *
 *                      filter coefficients used by the synthesis filter.
 *
 *     ppswPFNumAs[0:3][0:9]
 *
 *                      filter coefficients used by the postfilters
 *                      numerator.
 *
 *     ppswPFDenomAs[0:3][0:9]
 *
 *                      filter coefficients used by postfilters denominator.
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   DESCRIPTION:
 *
 *     For interpolated subframes, the direct form coefficients
 *     are converted to reflection coeffiecients to check for
 *     filter stability. If unstable, the uninterpolated coef.
 *     are used for that subframe.
 *
 *     Interpolation is described in section 4.1.6, "Soft Interpolation
 *     of the Spectral Parameters"
 *
 *    REFERENCES: Sub_clause 4.2.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: soft interpolation, int_lpc, interpolate, atorc,res_eng,i_mov
 *
 *************************************************************************/

void   getSfrmLpc(short int siSoftInterpolation,
                  INT16 swPrevR0, INT16 swNewR0,
                  /* last frm */ INT16 pswPrevFrmKs[], INT16 pswPrevFrmAs[],
                  INT16 pswPrevFrmPFNum[],
                  INT16 pswPrevFrmPFDenom[],

                  /* this frm */ INT16 pswNewFrmKs[], INT16 pswNewFrmAs[],
                  INT16 pswNewFrmPFNum[],
                  INT16 pswNewFrmPFDenom[],

                  /* output */ struct NormSw *psnsSqrtRs,
                  INT16 *ppswSynthAs[], INT16 *ppswPFNumAs[],
                  INT16 *ppswPFDenomAs[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */


    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int siSfrm,
          siStable,
          i;

    INT32 L_Temp1,
          L_Temp2;

//INT16 essai;

//VPP_HR_PROFILE_FUNCTION_ENTER( getSfrmLpc);


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    if (siSoftInterpolation)
    {
        /* yes, interpolating */
        /* ------------------ */

        siSfrm = 0;

        siStable = interpolateCheck(pswPrevFrmKs, pswPrevFrmAs,
                                    pswPrevFrmAs, pswNewFrmAs,
                                    psrOldCont[siSfrm], psrNewCont[siSfrm],
                                    swPrevR0,
                                    &psnsSqrtRs[siSfrm],
                                    ppswSynthAs[siSfrm]);
        if (siStable)
        {

            /* interpolate between direct form coefficient sets */
            /* for both numerator and denominator coefficients  */
            /* assume output will be stable                     */
            /* ------------------------------------------------ */

            for (i = 0; i < NP; i++)
            {
                L_Temp1 = L_MULT(pswNewFrmPFNum[i], psrNewCont[siSfrm]);
                ppswPFNumAs[siSfrm][i] = MAC_R(L_Temp1, pswPrevFrmPFNum[i], psrOldCont[siSfrm]);
                /*essai= MAC_R(L_Temp1, pswPrevFrmPFNum[i], psrOldCont[siSfrm]);
                if (essai!=ppswPFNumAs[siSfrm][i] )
                {
                    essai=0;
                }*/
                L_Temp2 = L_MULT(pswNewFrmPFDenom[i], psrNewCont[siSfrm]);
                ppswPFDenomAs[siSfrm][i] = MAC_R(L_Temp2, pswPrevFrmPFDenom[i], psrOldCont[siSfrm]);
            }
        }
        else
        {
            /* this subframe is unstable */
            /* ------------------------- */
            for (i = 0; i < NP; i++)
            {
                ppswPFNumAs[siSfrm][i] = pswPrevFrmPFNum[i];
                ppswPFDenomAs[siSfrm][i] = pswPrevFrmPFDenom[i];
            }
        }
        for (siSfrm = 1; siSfrm < N_SUB - 1; siSfrm++)
        {

            siStable = interpolateCheck(pswNewFrmKs, pswNewFrmAs,
                                        pswPrevFrmAs, pswNewFrmAs,
                                        psrOldCont[siSfrm], psrNewCont[siSfrm],
                                        swNewR0,
                                        &psnsSqrtRs[siSfrm],
                                        ppswSynthAs[siSfrm]);
            if (siStable)
            {

                /* interpolate between direct form coefficient sets */
                /* for both numerator and denominator coefficients  */
                /* assume output will be stable                     */
                /* ------------------------------------------------ */

                for (i = 0; i < NP; i++)
                {
                    L_Temp1 = L_MULT(pswNewFrmPFNum[i], psrNewCont[siSfrm]);
                    ppswPFNumAs[siSfrm][i] = MAC_R(L_Temp1, pswPrevFrmPFNum[i], psrOldCont[siSfrm]);
                    L_Temp2 = L_MULT(pswNewFrmPFDenom[i], psrNewCont[siSfrm]);
                    ppswPFDenomAs[siSfrm][i] = MAC_R(L_Temp2, pswPrevFrmPFDenom[i], psrOldCont[siSfrm]);
                }
            }
            else
            {
                /* this subframe has unstable filter coeffs, would like to
                 * interpolate but can not  */
                /* -------------------------------------- */
                for (i = 0; i < NP; i++)
                {
                    ppswPFNumAs[siSfrm][i] = pswNewFrmPFNum[i];
                    ppswPFDenomAs[siSfrm][i] = pswNewFrmPFDenom[i];
                }
            }
        }
        /* the last subframe never interpolate */
        /* ----------------------------------- */
        siSfrm = 3;
        for (i = 0; i < NP; i++)
        {
            ppswPFNumAs[siSfrm][i] = pswNewFrmPFNum[i];
            ppswPFDenomAs[siSfrm][i] = pswNewFrmPFDenom[i];
            ppswSynthAs[siSfrm][i] = pswNewFrmAs[i];
        }

        res_eng(pswNewFrmKs, swNewR0, &psnsSqrtRs[siSfrm]);

    }
    /* SoftInterpolation == 0  - no interpolation */
    /* ------------------------------------------ */
    else
    {
        siSfrm = 0;
        for (i = 0; i < NP; i++)
        {
            ppswPFNumAs[siSfrm][i] = pswPrevFrmPFNum[i];
            ppswPFDenomAs[siSfrm][i] = pswPrevFrmPFDenom[i];
            ppswSynthAs[siSfrm][i] = pswPrevFrmAs[i];
        }

        res_eng(pswPrevFrmKs, swPrevR0, &psnsSqrtRs[siSfrm]);

        /* for subframe 1 and all subsequent sfrms, use result from new frm */
        /* ---------------------------------------------------------------- */


        res_eng(pswNewFrmKs, swNewR0, &psnsSqrtRs[1]);

        for (siSfrm = 1; siSfrm < N_SUB; siSfrm++)
        {


            psnsSqrtRs[siSfrm].man = psnsSqrtRs[1].man;
            psnsSqrtRs[siSfrm].sh = psnsSqrtRs[1].sh;

            for (i = 0; i < NP; i++)
            {
                ppswPFNumAs[siSfrm][i] = pswNewFrmPFNum[i];
                ppswPFDenomAs[siSfrm][i] = pswNewFrmPFDenom[i];
                ppswSynthAs[siSfrm][i] = pswNewFrmAs[i];
            }
        }
    }

//VPP_HR_PROFILE_FUNCTION_EXIT( getSfrmLpc);


}

/***************************************************************************
 *
 *   FUNCTION NAME: get_ipjj
 *
 *   PURPOSE:
 *
 *     This subroutine calculates IP, the single-resolution lag rounded
 *     down to the nearest integer, and JJ, the remainder when the
 *     extended resolution lag is divided by the oversampling factor
 *
 *   INPUTS:
 *
 *     swLagIn
 *                     extended resolution lag as an integer, i.e.
 *                     fractional lag x oversampling factor
 *
 *   OUTPUTS:
 *
 *     *pswIp
 *                     fractional lag rounded down to nearest integer, IP
 *
 *     *pswJj
 *                     the remainder JJ
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     ip = integer[lag/OS_FCTR]
 *     jj = integer_round[((lag/OS_FCTR)-ip)*(OS_FCTR)]
 *          if the rounding caused an 'overflow'
 *            set remainder jj to 0 and add 'carry' to ip
 *
 *     This routine is involved in the mechanics of fractional and
 *     integer LTP searchs.  The LTP is described in section 5.
 *
 *   REFERENCES: Sub-clause 4.1.8 and 4.2.2 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lag, fractional, remainder, ip, jj, get_ipjj
 *
 *************************************************************************/

void   get_ipjj(INT16 swLagIn,
                INT16 *pswIp, INT16 *pswJj)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  OS_FCTR_INV  (INT16)0x1555/* SW_MAX/OS_FCTR */

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Temp;

    INT16 swTemp,
          swTempIp,
          swTempJj;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* calculate ip */
    /* ------------ */

    L_Temp = L_MULT(OS_FCTR_INV, swLagIn);        /* lag/OS_FCTR */
    //swTempIp = extract_h(L_Temp);
    swTempIp = EXTRACT_H(L_Temp);

    /* calculate jj */
    /* ------------ */

    swTemp = EXTRACT_L(L_Temp);          /* loose ip */
    swTemp = SHR(swTemp, 1);             /* isolate jj fraction */
    swTemp = swTemp & SW_MAX;
    L_Temp = L_MULT(swTemp, OS_FCTR);    /* ((lag/OS_FCTR)-ip))*(OS_FCTR) */
    swTemp = ROUNDO(L_Temp);              /* round and pick-off jj */
    if ( swTemp == OS_FCTR )
    {
        /* if 'overflow ' */
        swTempJj = 0;                      /* set remainder,jj to 0 */
        //swTempIp = ADD_SAT16(swTempIp, 1);
        swTempIp++;      /* 'carry' overflow into ip */
    }
    else
    {
        swTempJj = swTemp;                 /* read-off remainder,jj */
    }

    /* return ip and jj */
    /* ---------------- */

    *pswIp = swTempIp;
    *pswJj = swTempJj;
}

/***************************************************************************
 *
 *   FUNCTION NAME: interpolateCheck
 *
 *   PURPOSE:
 *
 *     Interpolates between direct form coefficient sets.
 *     Before releasing the interpolated coefficients, they are checked.
 *     If unstable, the "old" parameters are used.
 *
 *   INPUTS:
 *
 *     pswRefKs[0:9]
 *                     decoded version of the rc's tx'd last frame
 *
 *     pswRefCoefsA[0:9]
 *                     above K's converted to direct form coefficients
 *
 *     pswOldCoefsA[0:9]
 *                     array of old Coefseters
 *
 *     pswNewCoefsA[0:9]
 *                     array of new Coefseters
 *
 *     swOldPer
 *                     amount old coefs supply to the output
 *
 *     swNewPer
 *                     amount new coefs supply to the output
 *
 *     ASHIFT
 *                     shift for reflection coef. conversion
 *
 *     swRq
 *                     quantized energy to use for subframe
 * *
 *   OUTPUTS:
 *
 *     psnsSqrtRsOut
 *                     output pointer to sqrt(RS) normalized
 *
 *     pswCoefOutA[0:9]
 *                     output coefficients
 *
 *   RETURN VALUE:
 *
 *     siInterp_flg
 *                     temporary subframe interpolation flag
 *                     0 - coef. interpolated, 1 -coef. not interpolated
 *
 *   DESCRIPTION:
 *
 *     For interpolated subframes, the direct form coefficients
 *     are converted to reflection coefficients to check for
 *     filter stability. If unstable, the uninterpolated coef.
 *     are used for that subframe.  Section 4.1.6 describes
 *     interpolation.
 *
 *   REFERENCES: Sub-clause 4.1.6 and 4.2.3 of GSM Recomendation 06.20
 *
 *   KEYWORDS: soft interpolation, int_lpc, interpolate, atorc,res_eng,i_mov
 *
 *************************************************************************/

short int interpolateCheck(INT16 pswRefKs[],
                           INT16 pswRefCoefsA[],
                           INT16 pswOldCoefsA[], INT16 pswNewCoefsA[],
                           INT16 swOldPer, INT16 swNewPer,
                           INT16 swRq,
                           struct NormSw *psnsSqrtRsOut,
                           INT16 pswCoefOutA[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswRcTemp[NP];

    INT32 L_Temp;

    short int siInterp_flg,
          i;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Interpolation loop, NP is order of LPC filter */
    /* --------------------------------------------- */

    for (i = 0; i < NP; i++)
    {
        L_Temp = L_MULT(pswNewCoefsA[i], swNewPer);
        pswCoefOutA[i] = MAC_R(L_Temp, pswOldCoefsA[i], swOldPer);
    }

    /* Convert to reflection coefficients and check stability */
    /* ------------------------------------------------------ */

    if (aToRc(ASHIFT, pswCoefOutA, pswRcTemp) != 0)
    {

        /* Unstable, use uninterpolated parameters and compute RS update the
         * state with the frame data closest to this subfrm */
        /* --------------------------------------------------------- */

        res_eng(pswRefKs, swRq, psnsSqrtRsOut);

        for (i = 0; i < NP; i++)
        {
            pswCoefOutA[i] = pswRefCoefsA[i];
        }
        siInterp_flg = 0;
    }
    else
    {

        /* Stable, compute RS */
        /* ------------------ */
        res_eng(pswRcTemp, swRq, psnsSqrtRsOut);

        /* Set temporary subframe interpolation flag */
        /* ----------------------------------------- */
        siInterp_flg = 1;
    }

    /* Return subframe interpolation flag */
    /* ---------------------------------- */
    return (siInterp_flg);
}

/***************************************************************************
 *
 *   FUNCTION NAME: lagDecode
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to decode the lag received from the
 *     speech encoder into a full resolution lag for the speech decoder
 *
 *   INPUTS:
 *
 *     swDeltaLag
 *
 *                     lag received from channel decoder
 *
 *     giSfrmCnt
 *
 *                     current sub-frame count
 *
 *     swLastLag
 *
 *                     previous lag to un-delta this sub-frame's lag
 *
 *     psrLagTbl[0:255]
 *
 *                     table used to look up full resolution lag
 *
 *   OUTPUTS:
 *
 *     swLastLag
 *
 *                     new previous lag for next sub-frame
 *
 *   RETURN VALUE:
 *
 *     swLag
 *
 *                     decoded full resolution lag
 *
 *   DESCRIPTION:
 *
 *     If first subframe, use lag as index to look up table directly.
 *
 *     If it is one of the other subframes, the codeword represents a
 *     delta offset.  The previously decoded lag is used as a starting
 *     point for decoding the current lag.
 *
 *   REFERENCES: Sub-clause 4.2.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: deltalags, lookup lag
 *
 *************************************************************************/

static INT16 lagDecode(INT16 swDeltaLag)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  DELTA_LEVELS_D2  DELTA_LEVELS/2
#define  MAX_LAG          0x00ff
#define  MIN_LAG          0x0000

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */

    static INT16 swLastLag;

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swLag;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* first sub-frame */
    /* --------------- */

    if (giSfrmCnt == 0)
    {
        swLastLag = swDeltaLag;
    }

    /* remaining sub-frames */
    /* -------------------- */

    else
    {

        /* get lag biased around 0 */
        /* ----------------------- */

        //swLag = sub(swDeltaLag, DELTA_LEVELS_D2);
        //swLag = SUB_s(swDeltaLag, DELTA_LEVELS_D2);
        swLag = SUB(swDeltaLag, DELTA_LEVELS_D2);

        /* get real lag relative to last */
        /* ----------------------------- */

        //swLag = ADD_SAT16(swLag, swLastLag);
        swLag = ADD(swLag, swLastLag);

        /* clip to max or min */
        /* ------------------ */

        if ( swLag > MAX_LAG )
        {
            swLastLag = MAX_LAG;
        }
        else if ( swLag < MIN_LAG )
        {
            swLastLag = MIN_LAG;
        }
        else
        {
            swLastLag = swLag;
        }
    }

    /* return lag after look up */
    /* ------------------------ */

    swLag = psrLagTbl[swLastLag];
    return (swLag);
}

/***************************************************************************
 *
 *   FUNCTION NAME: lookupVq
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to recover the reflection coeffs from
 *     the received LPC codewords.
 *
 *   INPUTS:
 *
 *     pswVqCodeWds[0:2]
 *
 *                         the codewords for each of the segments
 *
 *   OUTPUTS:
 *
 *     pswRCOut[0:NP-1]
 *
 *                        the decoded reflection coefficients
 *
 *   RETURN VALUE:
 *
 *     none.
 *
 *   DESCRIPTION:
 *
 *     For each segment do the following:
 *       setup the retrieval pointers to the correct vector
 *       get that vector
 *
 *   REFERENCES: Sub-clause 4.2.3 of GSM Recomendation 06.20
 *
 *   KEYWORDS: vq, vectorquantizer, lpc
 *
 *************************************************************************/

static void lookupVq(INT16 pswVqCodeWds[], INT16 pswRCOut[])
{
    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  LSP_MASK  0x00ff

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int siSeg,
          siIndex,
          siVector,
          siVector1,
          siVector2,
          siWordPtr;

    INT16 *psrQTable;


    //VPP_HR_PROFILE_FUNCTION_ENTER(lookupVq );


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* for each segment */
    /* ---------------- */

    for (siSeg = 0; siSeg < QUANT_NUM_OF_TABLES; siSeg++)
    {

        siVector = pswVqCodeWds[siSeg];
        siIndex = psvqIndex[siSeg].l;

        if ( siSeg == 2 )
        {
            /* segment 3 */

            /* set table */
            /* --------- */

            psrQTable = psrQuant3;

            /* set offset into table */
            /* ---------------------- */

            //siWordPtr = ADD_SAT16(siVector, siVector);
            siWordPtr = ADD(siVector, siVector);

            /* look up coeffs */
            /* -------------- */

            siVector1 = psrQTable[siWordPtr];
            siVector2 = psrQTable[siWordPtr + 1];

            pswRCOut[siIndex - 1] = psrSQuant[SHR(siVector1, 8) & LSP_MASK];
            pswRCOut[siIndex] = psrSQuant[siVector1 & LSP_MASK];
            pswRCOut[siIndex + 1] = psrSQuant[SHR(siVector2, 8) & LSP_MASK];
            pswRCOut[siIndex + 2] = psrSQuant[siVector2 & LSP_MASK];
        }
        else
        {
            /* segments 1 and 2 */

            /* set tables */
            /* ---------- */

            if (siSeg == 0)
            {
                psrQTable = psrQuant1;
            }
            else
            {
                psrQTable = psrQuant2;

            }

            /* set offset into table */
            /* --------------------- */

            //siWordPtr = ADD_SAT16(siVector, siVector);
            siWordPtr = ADD (siVector, siVector);
            //siWordPtr = ADD_SAT16(siWordPtr, siVector);
            siWordPtr = ADD(siWordPtr, siVector);
            siWordPtr = SHR(siWordPtr, 1);

            /* look up coeffs */
            /* -------------- */

            siVector1 = psrQTable[siWordPtr];
            siVector2 = psrQTable[siWordPtr + 1];

            if ((siVector & 0x0001) == 0)
            {
                pswRCOut[siIndex - 1] = psrSQuant[SHR(siVector1, 8) & LSP_MASK];
                pswRCOut[siIndex] = psrSQuant[siVector1 & LSP_MASK];
                pswRCOut[siIndex + 1] = psrSQuant[SHR(siVector2, 8) & LSP_MASK];
            }
            else
            {
                pswRCOut[siIndex - 1] = psrSQuant[siVector1 & LSP_MASK];
                pswRCOut[siIndex] = psrSQuant[SHR(siVector2, 8) & LSP_MASK];
                pswRCOut[siIndex + 1] = psrSQuant[siVector2 & LSP_MASK];
            }
        }
    }

    //VPP_HR_PROFILE_FUNCTION_EXIT(lookupVq );

}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcFir
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform direct form fir filtering
 *     assuming a NP order filter and given state, coefficients, and input.
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter
 *
 *     pswInput[0:S_LEN-1]
 *
 *                     input array of points to be filtered.
 *                     pswInput[0] is the oldest point (first to be filtered)
 *                     pswInput[siLen-1] is the last point filtered (newest)
 *
 *     pswCoef[0:NP-1]
 *
 *                     array of direct form coefficients
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *     pswState[0:NP-1]
 *
 *                     array of the filter state following form of pswCoef
 *                     pswState[0] = state of filter for delay n = -1
 *                     pswState[NP-1] = state of filter for delay n = -NP
 *
 *   OUTPUTS:
 *
 *     pswState[0:NP-1]
 *
 *                     updated filter state, ready to filter
 *                     pswInput[siLen], i.e. the next point
 *
 *     pswFiltOut[0:S_LEN-1]
 *
 *                     the filtered output
 *                     same format as pswInput, pswFiltOut[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] += state[j]*coef[j] (state is taken from either input
 *                                       state[] or input in[] arrays)
 *        rescale(out[i])
 *        out[i] += in[i]
 *     update final state array using in[]
 *
 *   REFERENCES: Sub-clause 4.1.7 and 4.2.4 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, fir, lpcFir, inversefilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set, i_dir_mod
 *
 *************************************************************************/

void   lpcFir(INT16 pswInput[], INT16 pswCoef[],
              INT16 pswState[], INT16 pswFiltOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;//,Lmult,L_Sum1
    short int siStage,
          siSmp;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* filter 1st sample */
    /* ----------------- */

    /* sum past state outputs */
    /* ---------------------- */
    /* 0th coef, with rounding */
    //L_Sum = L_mac(LPC_ROUND, pswState[0], pswCoef[0]);
    /*Lmult=L_MULT(pswState[0], pswCoef[0]);
    L_Sum=L_ADD(LPC_ROUND,Lmult);*/
    s_lo=0x00000400L;//SHR(LPC_ROUND ,1);
    VPP_MLA16(s_hi, s_lo, pswState[0], pswCoef[0]);



    for (siStage = 1; siStage < NP; siStage++)
    {
        /* remaining coefs */
        //L_Sum = L_mac(L_Sum, pswState[siStage], pswCoef[siStage]);
        /*Lmult=L_MULT(pswState[siStage], pswCoef[siStage]);
        L_Sum=L_ADD(L_Sum,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswState[siStage], pswCoef[siStage]);
    }
    L_Sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    /* add input to partial output */
    /* --------------------------- */

    //L_Sum = L_shl(L_Sum, ASHIFT);
    //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
    L_Sum <<=ASHIFT ;

    s_lo=SHR(L_Sum ,1);
    //L_Sum = L_msu(L_Sum, pswInput[0], 0x8000);
    VPP_MLA16(s_hi, s_lo, -(pswInput[0]),(INT16) 0x8000);

    L_Sum= L_MLA_SAT_CARRY(s_lo, L_Sum&1);// L_MLA_SAT(s_hi, s_lo);

    /* save 1st output sample */
    /* ---------------------- */

    //pswFiltOut[0] = extract_h(L_Sum);
    pswFiltOut[0] = EXTRACT_H(L_Sum);

    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_mac(LPC_ROUND, pswInput[siSmp - 1], pswCoef[0]);
        /*Lmult=L_MULT(pswInput[siSmp - 1], pswCoef[0]);
        L_Sum=L_ADD(LPC_ROUND,Lmult);*/
        s_lo=0x00000400L;//SHR(LPC_ROUND ,1);
        VPP_MLA16(s_hi, s_lo, pswInput[siSmp - 1], pswCoef[0]);

        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            //L_Sum = L_mac(L_Sum, pswInput[siSmp - siStage - 1], pswCoef[siStage]);
            /*Lmult=L_MULT(pswInput[siSmp - siStage - 1], pswCoef[siStage]);
            L_Sum=L_ADD(L_Sum,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswInput[siSmp - siStage - 1], pswCoef[siStage]);
        }

        /* sum past states, if any */
        /* ----------------------- */

        for (siStage = siSmp; siStage < NP; siStage++)
        {
            //L_Sum = L_mac(L_Sum, pswState[siStage - siSmp], pswCoef[siStage]);
            /*Lmult=L_MULT(pswState[siStage - siSmp], pswCoef[siStage]);
            L_Sum=L_ADD(L_Sum,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswState[siStage - siSmp], pswCoef[siStage]);
        }
        L_Sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* add input to partial output */
        /* --------------------------- */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        s_lo=SHR(L_Sum ,1);
        //L_Sum = L_msu(L_Sum, pswInput[siSmp], 0x8000);
        VPP_MLA16(s_hi, s_lo, -(pswInput[siSmp]), (INT16) 0x8000);

        L_Sum=  L_MLA_SAT_CARRY(s_lo, L_Sum&1);//L_MLA_SAT(s_hi, s_lo);

        /* save current output sample */
        /* -------------------------- */

        //pswFiltOut[siSmp] = extract_h(L_Sum);
        pswFiltOut[siSmp] = EXTRACT_H(L_Sum);

    }

    /* save final state */
    /* ---------------- */

    for (siStage = 0; siStage < NP; siStage++)
    {
        pswState[siStage] = pswInput[S_LEN - siStage - 1];
    }

}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcIir
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform direct form IIR filtering
 *     assuming a NP order filter and given state, coefficients, and input
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter
 *
 *     pswInput[0:S_LEN-1]
 *
 *                     input array of points to be filtered
 *                     pswInput[0] is the oldest point (first to be filtered)
 *                     pswInput[siLen-1] is the last point filtered (newest)
 *
 *     pswCoef[0:NP-1]
 *                     array of direct form coefficients
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *     pswState[0:NP-1]
 *
 *                     array of the filter state following form of pswCoef
 *                     pswState[0] = state of filter for delay n = -1
 *                     pswState[NP-1] = state of filter for delay n = -NP
 *
 *   OUTPUTS:
 *
 *     pswState[0:NP-1]
 *
 *                     updated filter state, ready to filter
 *                     pswInput[siLen], i.e. the next point
 *
 *     pswFiltOut[0:S_LEN-1]
 *
 *                     the filtered output
 *                     same format as pswInput, pswFiltOut[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] -= state[j]*coef[j] (state is taken from either input
 *                                       state[] or prior out[] arrays)
 *        rescale(out[i])
 *        out[i] += in[i]
 *     update final state array using out[]
 *
 *   REFERENCES: Sub-clause 4.1.7 and 4.2.4 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, iir, synthesisfilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set
 *
 *************************************************************************/

void   lpcIir(INT16 pswInput[], INT16 pswCoef[],
              INT16 pswState[], INT16 pswFiltOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;
    short int siStage,
          siSmp;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* filter 1st sample */
    /* ----------------- */

    /* sum past state outputs */
    /* ---------------------- */
    /* 0th coef, with rounding */
    //L_Sum = L_msu(LPC_ROUND, pswState[0], pswCoef[0]);
    s_lo=0x00000400L;//SHR(LPC_ROUND ,1);
    VPP_MLA16(s_hi, s_lo, -pswState[0], pswCoef[0]);



    for (siStage = 1; siStage < NP; siStage++)
    {
        /* remaining coefs */
        //L_Sum = L_msu(L_Sum, pswState[siStage], pswCoef[siStage]);
        VPP_MLA16(s_hi, s_lo, -pswState[siStage], pswCoef[siStage]);
    }
    L_Sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    /* add input to partial output */
    /* --------------------------- */

    //L_Sum = L_shl(L_Sum, ASHIFT);
    //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
    L_Sum <<=ASHIFT ;

    s_lo=SHR(L_Sum ,1);
    //L_Sum = L_msu(L_Sum, pswInput[0], 0x8000);
    VPP_MLA16(s_hi, s_lo, -pswInput[0], (INT16)0x8000);

    L_Sum= L_MLA_SAT_CARRY(s_lo, L_Sum&1);// L_MLA_SAT(s_hi, s_lo);

    /* save 1st output sample */
    /* ---------------------- */

    //pswFiltOut[0] = extract_h(L_Sum);
    pswFiltOut[0] = EXTRACT_H(L_Sum);


    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_msu(LPC_ROUND, pswFiltOut[siSmp - 1], pswCoef[0]);
        s_lo=0x00000400L;//SHR(LPC_ROUND ,1);
        VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - 1], pswCoef[0]);

        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            //L_Sum = L_msu(L_Sum, pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
            VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
        }

        /* sum past states, if any */
        /* ----------------------- */

        for (siStage = siSmp; siStage < NP; siStage++)
        {
            //L_Sum = L_msu(L_Sum, pswState[siStage - siSmp], pswCoef[siStage]);
            VPP_MLA16(s_hi, s_lo, -pswState[siStage - siSmp], pswCoef[siStage]);
        }
        L_Sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* add input to partial output */
        /* --------------------------- */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        s_lo=SHR(L_Sum ,1);
        //L_Sum = L_msu(L_Sum, pswInput[siSmp], 0x8000);
        VPP_MLA16(s_hi, s_lo, -pswInput[siSmp], (INT16)0x8000);
        L_Sum= L_MLA_SAT_CARRY(s_lo, L_Sum&1);// L_MLA_SAT(s_hi, s_lo);

        /* save current output sample */
        /* -------------------------- */

        //pswFiltOut[siSmp] = extract_h(L_Sum);
        pswFiltOut[siSmp] = EXTRACT_H(L_Sum);

    }

    /* save final state */
    /* ---------------- */

    for (siStage = 0; siStage < NP; siStage++)
    {
        pswState[siStage] = pswFiltOut[S_LEN - siStage - 1];
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcIrZsIir
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to calculate the impulse response
 *     via direct form IIR filtering with zero state assuming a NP order
 *     filter and given coefficients
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter
 *
 *     pswCoef[0:NP-1]
 *                     array of direct form coefficients
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *   OUTPUTS:
 *
 *     pswFiltOut[0:S_LEN-1]
 *
 *                     the filtered output
 *                     same format as pswInput, pswFiltOut[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     This routine is called by getNWCoefs().
 *
 *     Because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] -= state[j]*coef[j] (state taken from prior output[])
 *        rescale(out[i])
 *
 *   REFERENCES: Sub-clause 4.1.8 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, iir, synthesisfilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set
 *
 *************************************************************************/

void   lpcIrZsIir(INT16 pswCoef[], INT16 pswFiltOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;
    short int siStage,
          siSmp;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* output 1st sample */
    /* ----------------- */

    pswFiltOut[0] = 0x0400;

    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_msu(LPC_ROUND, pswFiltOut[siSmp - 1], pswCoef[0]);
        s_lo=0x00000400L;//SHR( LPC_ROUND,1);
        VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - 1], pswCoef[0]);



        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            //L_Sum = L_msu(L_Sum, pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
            VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
        }
        L_Sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* scale output */
        /* ------------ */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        /* save current output sample */
        /* -------------------------- */

        //pswFiltOut[siSmp] = extract_h(L_Sum);
        pswFiltOut[siSmp] = EXTRACT_H(L_Sum);

    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcZiIir
 *
 *   PURPOSE:
 *     The purpose of this function is to perform direct form iir filtering
 *     with zero input assuming a NP order filter, and given state and
 *     coefficients
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter MUST be <= MAX_ZIS
 *
 *     pswCoef[0:NP-1]
 *
 *                     array of direct form coefficients.
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *     pswState[0:NP-1]
 *
 *                     array of the filter state following form of pswCoef
 *                     pswState[0] = state of filter for delay n = -1
 *                     pswState[NP-1] = state of filter for delay n = -NP
 *
 *   OUTPUTS:
 *
 *     pswFiltOut[0:S_LEN-1]
 *
 *                     the filtered output
 *                     same format as pswIn, pswFiltOut[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     The routine is called from sfrmAnalysis, and is used to let the
 *     LPC filters ring out.
 *
 *     because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] -= state[j]*coef[j] (state is taken from either input
 *                                       state[] or prior output[] arrays)
 *        rescale(out[i])
 *
 *   REFERENCES: Sub-clause 4.1.7 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, iir, synthesisfilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set
 *
 *************************************************************************/

void   lpcZiIir(INT16 pswCoef[], INT16 pswState[],
                INT16 pswFiltOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;
    short int siStage,
          siSmp;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* filter 1st sample */
    /* ----------------- */

    /* sum past state outputs */
    /* ---------------------- */
    /* 0th coef, with rounding */
    //L_Sum = L_msu(LPC_ROUND, pswState[0], pswCoef[0]);
    s_lo=0x00000400L;//SHR( LPC_ROUND,1);
    VPP_MLA16(s_hi, s_lo, -pswState[0], pswCoef[0]);



    for (siStage = 1; siStage < NP; siStage++)
    {
        /* remaining coefs */
        //L_Sum = L_msu(L_Sum, pswState[siStage], pswCoef[siStage]);
        VPP_MLA16(s_hi, s_lo, -pswState[siStage], pswCoef[siStage]);
    }
    L_Sum= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    /* scale output */
    /* ------------ */

    //L_Sum = L_shl(L_Sum, ASHIFT);
    //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
    L_Sum <<=ASHIFT ;

    /* save 1st output sample */
    /* ---------------------- */

    //pswFiltOut[0] = extract_h(L_Sum);
    pswFiltOut[0] = EXTRACT_H(L_Sum);


    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_msu(LPC_ROUND, pswFiltOut[siSmp - 1], pswCoef[0]);
        s_lo=0x00000400L;//SHR( LPC_ROUND,1);
        VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - 1], pswCoef[0]);

        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            //L_Sum = L_msu(L_Sum, pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
            VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
        }

        /* sum past states, if any */
        /* ----------------------- */

        for (siStage = siSmp; siStage < NP; siStage++)
        {
            //L_Sum = L_msu(L_Sum, pswState[siStage - siSmp], pswCoef[siStage]);
            VPP_MLA16(s_hi, s_lo, -pswState[siStage - siSmp], pswCoef[siStage]);
        }
        L_Sum = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* scale output */
        /* ------------ */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        /* save current output sample */
        /* -------------------------- */

        pswFiltOut[siSmp] = EXTRACT_H(L_Sum);
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcZsFir
 *
 *   PURPOSE:
 *     The purpose of this function is to perform direct form fir filtering
 *     with zero state, assuming a NP order filter and given coefficients
 *     and non-zero input.
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter
 *
 *     pswInput[0:S_LEN-1]
 *
 *                     input array of points to be filtered.
 *                     pswInput[0] is the oldest point (first to be filtered)
 *                     pswInput[siLen-1] is the last point filtered (newest)
 *
 *     pswCoef[0:NP-1]
 *
 *                     array of direct form coefficients
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *   OUTPUTS:
 *
 *     pswFiltOut[0:S_LEN-1]
 *
 *                     the filtered output
 *                     same format as pswInput, pswFiltOut[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     This routine is used in getNWCoefs().  See section 4.1.7.
 *
 *     because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] += state[j]*coef[j] (state taken from in[])
 *        rescale(out[i])
 *        out[i] += in[i]
 *
 *   REFERENCES: Sub-clause 4.1.7 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, fir, lpcFir, inversefilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set, i_dir_mod
 *
 *************************************************************************/

void   lpcZsFir(INT16 pswInput[], INT16 pswCoef[],
                INT16 pswFiltOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;//,Lmult
    short int siStage,
          siSmp;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* output 1st sample */
    /* ----------------- */

    pswFiltOut[0] = pswInput[0];

    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_mac(LPC_ROUND, pswInput[siSmp - 1], pswCoef[0]);
        /*Lmult=L_MULT(pswInput[siSmp - 1], pswCoef[0]);
        L_Sum=L_ADD(LPC_ROUND,Lmult);*/
        s_lo=0x00000400L;//SHR( LPC_ROUND,1);
        VPP_MLA16(s_hi, s_lo, pswInput[siSmp - 1], pswCoef[0]);


        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            /*L_Sum = L_mac(L_Sum, pswInput[siSmp - siStage - 1],
                          pswCoef[siStage]);
            Lmult=L_MULT(pswInput[siSmp - siStage - 1],
                          pswCoef[siStage]);
            L_Sum=L_ADD(L_Sum,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswInput[siSmp - siStage - 1], pswCoef[siStage]);
        }
        L_Sum = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* add input to partial output */
        /* --------------------------- */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        //L_Sum = L_msu(L_Sum, pswInput[siSmp], 0x8000);
        s_lo=SHR( L_Sum,1);
        VPP_MLA16(s_hi, s_lo, -pswInput[siSmp], (INT16)0x8000);
        L_Sum = L_MLA_SAT_CARRY(s_lo, L_Sum&1);// L_MLA_SAT(s_hi, s_lo);

        /* save current output sample */
        /* -------------------------- */

        pswFiltOut[siSmp] = EXTRACT_H(L_Sum);
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcZsIir
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform direct form IIR filtering
 *     with zero state, assuming a NP order filter and given coefficients
 *     and non-zero input.
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter
 *
 *     pswInput[0:S_LEN-1]
 *
 *                     input array of points to be filtered
 *                     pswInput[0] is the oldest point (first to be filtered)
 *                     pswInput[siLen-1] is the last point filtered (newest)
 *
 *     pswCoef[0:NP-1]
 *                     array of direct form coefficients
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *   OUTPUTS:
 *
 *     pswFiltOut[0:S_LEN-1]
 *
 *                     the filtered output
 *                     same format as pswInput, pswFiltOut[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     This routine is used in the subframe analysis process.  It is
 *     called by sfrmAnalysis() and fnClosedLoop().  It is this function
 *     which performs the weighting of the excitation vectors.
 *
 *     because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] -= state[j]*coef[j] (state taken from prior out[])
 *        rescale(out[i])
 *        out[i] += in[i]
 *
 *   REFERENCES: Sub-clause 4.1.8.5 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, iir, synthesisfilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set
 *
 *************************************************************************/

void   lpcZsIir(INT16 pswInput[], INT16 pswCoef[],
                INT16 pswFiltOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;
    short int siStage,
          siSmp;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* output 1st sample */
    /* ----------------- */

    pswFiltOut[0] = pswInput[0];

    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_msu(LPC_ROUND, pswFiltOut[siSmp - 1], pswCoef[0]);
        s_lo=0x00000400L;//SHR( LPC_ROUND,1);
        VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - 1], pswCoef[0]);


        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            //L_Sum = L_msu(L_Sum, pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
            VPP_MLA16(s_hi, s_lo, -pswFiltOut[siSmp - siStage - 1], pswCoef[siStage]);
        }
        L_Sum = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* add input to partial output */
        /* --------------------------- */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        s_lo=SHR( L_Sum,1);
        //L_Sum = L_msu(L_Sum, pswInput[siSmp], 0x8000);
        VPP_MLA16(s_hi, s_lo, -pswInput[siSmp], (INT16) 0x8000);
        L_Sum = L_MLA_SAT_CARRY(s_lo, L_Sum&1);// L_MLA_SAT(s_hi, s_lo);

        /* save current output sample */
        /* -------------------------- */

        pswFiltOut[siSmp] = EXTRACT_H(L_Sum);
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: lpcZsIirP
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform direct form iir filtering
 *     with zero state, assuming a NP order filter and given coefficients
 *     and input
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the lpc filter
 *
 *     S_LEN
 *                     number of samples to filter
 *
 *     pswCommonIO[0:S_LEN-1]
 *
 *                     input array of points to be filtered
 *                     pswCommonIO[0] is oldest point (first to be filtered)
 *                     pswCommonIO[siLen-1] is last point filtered (newest)
 *
 *     pswCoef[0:NP-1]
 *                     array of direct form coefficients
 *                     pswCoef[0] = coeff for delay n = -1
 *                     pswCoef[NP-1] = coeff for delay n = -NP
 *
 *     ASHIFT
 *                     number of shifts input A's have been shifted down by
 *
 *     LPC_ROUND
 *                     rounding constant
 *
 *   OUTPUTS:
 *
 *     pswCommonIO[0:S_LEN-1]
 *
 *                     the filtered output
 *                     pswCommonIO[0] is oldest point
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     This function is called by geNWCoefs().  See section 4.1.7.
 *
 *     because of the default sign of the coefficients the
 *     formula for the filter is :
 *     i=0, i < S_LEN
 *        out[i] = rounded(state[i]*coef[0])
 *        j=1, j < NP
 *           out[i] += state[j]*coef[j] (state taken from prior out[])
 *        rescale(out[i])
 *        out[i] += in[i]
 *
 *   REFERENCES: Sub-clause 4.1.7 of GSM Recomendation 06.20
 *
 *   KEYWORDS: lpc, directform, iir, synthesisfilter, lpcFilt
 *   KEYWORDS: dirForm, dir_mod, dir_clr, dir_neg, dir_set
 *
 *************************************************************************/

void   lpcZsIirP(INT16 pswCommonIO[], INT16 pswCoef[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Sum;//,Lmult
    short int siStage,
          siSmp;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* filter remaining samples */
    /* ------------------------ */

    for (siSmp = 1; siSmp < S_LEN; siSmp++)
    {

        /* sum past outputs */
        /* ---------------- */
        /* 0th coef, with rounding */
        //L_Sum = L_mac(LPC_ROUND, pswCommonIO[siSmp - 1], pswCoef[0]);
        /*Lmult=L_MULT(pswCommonIO[siSmp - 1], pswCoef[0]);
        L_Sum=L_ADD(LPC_ROUND,Lmult);*/
        s_lo=0x00000400L;//SHR( LPC_ROUND,1);
        VPP_MLA16(s_hi, s_lo, pswCommonIO[siSmp - 1], pswCoef[0]);



        /* remaining coefs */
        for (siStage = 1; ((0 < (siSmp - siStage)) && siStage < NP); siStage++)
        {
            /*L_Sum = L_mac(L_Sum, pswCommonIO[siSmp - siStage - 1], pswCoef[siStage]);
            Lmult=L_MULT(pswCommonIO[siSmp - siStage - 1], pswCoef[siStage]);
            L_Sum=L_ADD(L_Sum,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswCommonIO[siSmp - siStage - 1], pswCoef[siStage]);
        }
        L_Sum = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* add input to partial output */
        /* --------------------------- */

        //L_Sum = L_shl(L_Sum, ASHIFT);
        //L_Sum = L_SHL_SAT(L_Sum, ASHIFT);
        L_Sum <<=ASHIFT ;

        s_lo=SHR( L_Sum,1);
        //L_Sum = L_msu(L_Sum, pswCommonIO[siSmp], 0x8000);
        VPP_MLA16(s_hi, s_lo, -pswCommonIO[siSmp], (INT16) 0x8000);
        L_Sum = L_MLA_SAT_CARRY(s_lo, L_Sum&1);// L_MLA_SAT(s_hi, s_lo);

        /* save current output sample */
        /* -------------------------- */

        pswCommonIO[siSmp] = EXTRACT_H(L_Sum);
    }
}

/**************************************************************************
 *
 *   FUNCTION NAME: pitchPreFilt
 *
 *   PURPOSE:
 *
 *     Performs pitch pre-filter on excitation in speech decoder.
 *
 *   INPUTS:
 *
 *     pswExcite[0:39]
 *
 *                     Synthetic residual signal to be filtered, a subframe-
 *                     length vector.
 *
 *     ppsrPVecIntFilt[0:9][0:5] ([tap][phase])
 *
 *                     Interpolation filter coefficients.
 *
 *     ppsrSqtrP0[0:2][0:31] ([voicing level-1][gain code])
 *
 *                     Sqrt(P0) look-up table, used to determine pitch
 *                     pre-filtering coefficient.
 *
 *     swRxGsp0
 *
 *                     Coded value from gain quantizer, used to look up
 *                     sqrt(P0).
 *
 *     swRxLag
 *
 *                     Full-resolution lag value (fractional lag *
 *                     oversampling factor), used to index pitch pre-filter
 *                     state.
 *
 *     swUvCode
 *
 *                     Coded voicing level, used to distinguish between
 *                     voiced and unvoiced conditions, and to look up
 *                     sqrt(P0).
 *
 *     swSemiBeta
 *
 *                     The gain applied to the adaptive codebook excitation
 *                     (long-term predictor excitation) limited to a maximum
 *                     of 1.0, used to determine the pitch pre-filter
 *                     coefficient.
 *
 *     snsSqrtRs
 *
 *                     The estimate of the energy in the residual, used only
 *                     for scaling.
 *
 *   OUTPUTS:
 *
 *     pswExciteOut[0:39]
 *
 *                     The output pitch pre-filtered excitation.
 *
 *     pswPPreState[0:44]
 *
 *                     Contains the state of the pitch pre-filter
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     If the voicing mode for the frame is unvoiced, then the pitch pre-
 *     filter state is updated with the input excitation, and the input
 *     excitation is copied to the output.
 *
 *     If voiced: first the energy in the input excitation is calculated.
 *     Then, the coefficient of the pitch pre-filter is obtained:
 *
 *     PpfCoef = POST_EPSILON * min(beta, sqrt(P0)).
 *
 *     Then, the pitch pre-filter is performed:
 *
 *     ex_p(n) = ex(n)  +  PpfCoef * ex_p(n-L)
 *
 *     The ex_p(n-L) sample is interpolated from the surrounding samples,
 *     even for integer values of L.
 *
 *     Note: The coefficients of the interpolating filter are multiplied
 *     by PpfCoef, rather multiplying ex_p(n_L) after interpolation.
 *
 *     Finally, the energy in the output excitation is calculated, and
 *     automatic gain control is applied to the output signal so that
 *     its energy matches the original.
 *
 *     The pitch pre-filter is described in section 4.2.2.
 *
 *   REFERENCES: Sub-clause 4.2.2 of GSM Recomendation 06.20
 *
 *   KEYWORDS: prefilter, pitch, pitchprefilter, excitation, residual
 *
 *************************************************************************/

static void pitchPreFilt(INT16 pswExcite[],
                         INT16 swRxGsp0,
                         INT16 swRxLag, INT16 swUvCode,
                         INT16 swSemiBeta, struct NormSw snsSqrtRs,
                         INT16 pswExciteOut[],
                         INT16 pswPPreStatem[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  POST_EPSILON  0x2666

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */


    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_1,
          L_OrigEnergy;//,Lmult

    INT16 swScale,
          swSqrtP0,
          swIntLag,
          swRemain,
          swEnergy,
          pswInterpCoefs[P_INT_MACS];

    short int i,
          j;

    struct NormSw snsOrigEnergy;

    INT16 *pswPPreCurr = &pswPPreStatem[LTP_LEN];
    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    register INT32  m_hi=0;
    register UINT32 m_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Initialization */
    /*----------------*/

    swEnergy = 0;

    /* Check voicing level */
    /*---------------------*/

    if (swUvCode == 0)
    {

        /* Unvoiced: perform one subframe of delay on state, copy input to */
        /* state, copy input to output (if not same)                       */
        /*-----------------------------------------------------------------*/

        for (i = 0; i < LTP_LEN - S_LEN; i++)
            pswPPreStatem[i] = pswPPreStatem[i + S_LEN];

        for (i = 0; i < S_LEN; i++)
            pswPPreStatem[i + LTP_LEN - S_LEN] = pswExcite[i];

        if (pswExciteOut != pswExcite)
        {

            for (i = 0; i < S_LEN; i++)
                pswExciteOut[i] = pswExcite[i];
        }
    }
    else
    {

        /* Voiced: calculate energy in input, filter, calculate energy in */
        /* output, scale                                                  */
        /*----------------------------------------------------------------*/

        /* Get energy in input excitation vector */
        /*---------------------------------------*/

        //swEnergy = ADD_SAT16(NEGATE(shl(snsSqrtRs.sh, 1)), 3);
        swEnergy = ADD(NEGATE(SHL(snsSqrtRs.sh, 1)), 3);

        if (swEnergy > 0)
        {

            /* High-energy residual: scale input vector during energy */
            /* calculation.  The shift count + 1 of the energy of the */
            /* residual estimate is used as an estimate of the shift  */
            /* count needed for the excitation energy                 */
            /*--------------------------------------------------------*/


            snsOrigEnergy.sh = g_corr1s(pswExcite, swEnergy, &L_OrigEnergy);
            snsOrigEnergy.man = ROUNDO(L_OrigEnergy);

        }
        else
        {

            /* set shift count to zero for AGC later */
            /*---------------------------------------*/

            swEnergy = 0;

            /* Lower-energy residual: no overflow protection needed */
            /*------------------------------------------------------*/

            L_OrigEnergy = 0;
            s_lo=0;
            for (i = 0; i < S_LEN; i++)
            {

                //L_OrigEnergy = L_mac(L_OrigEnergy, pswExcite[i], pswExcite[i]);
                /*Lmult=L_MULT(pswExcite[i], pswExcite[i]);
                L_OrigEnergy=L_ADD(L_OrigEnergy,Lmult);*/
                VPP_MLA16(s_hi, s_lo, pswExcite[i], pswExcite[i]);

            }
            L_OrigEnergy = L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

            snsOrigEnergy.sh = norm_l(L_OrigEnergy);
            //snsOrigEnergy.man = ROUNDO(L_shl(L_OrigEnergy, snsOrigEnergy.sh));
            //snsOrigEnergy.man = ROUNDO(L_SHL_SAT(L_OrigEnergy, snsOrigEnergy.sh));
            snsOrigEnergy.man = ROUNDO( SHL (L_OrigEnergy, snsOrigEnergy.sh));

        }

        /* Determine pitch pre-filter coefficient, and scale the appropriate */
        /* phase of the interpolating filter by it                           */
        /*-------------------------------------------------------------------*/

        swSqrtP0 = ppsrSqrtP0[swUvCode - 1][swRxGsp0];

        if ( swSqrtP0 > swSemiBeta )
            swScale = swSemiBeta;
        else
            swScale = swSqrtP0;

        swScale = MULT_R(POST_EPSILON, swScale);

        get_ipjj(swRxLag, &swIntLag, &swRemain);

        m_hi=0;
        m_lo=0;
        for (i = 0; i < P_INT_MACS; i++)
        {
            pswInterpCoefs[i] = MULT_R(ppsrPVecIntFilt[i][swRemain], swScale);
            /*VPP_MULT_R(m_hi, m_lo, ppsrPVecIntFilt[i][swRemain], swScale);
            pswInterpCoefs[i] = EXTRACT_H( SHL(m_lo,1) );*/
        }

        /* Perform filter */
        /*----------------*/

        for (i = 0; i < S_LEN; i++)
        {

            L_1 = L_DEPOSIT_H(pswExcite[i]);
            s_lo=SHR(L_1,1);
            for (j = 0; j < P_INT_MACS - 1; j++)
            {

                /*L_1 = L_mac(L_1, pswPPreCurr[i - swIntLag - P_INT_MACS / 2 + j], pswInterpCoefs[j]);
                Lmult=L_MULT(pswPPreCurr[i - swIntLag - P_INT_MACS / 2 + j], pswInterpCoefs[j]);
                L_1=L_ADD(L_1,Lmult);*/
                VPP_MLA16(s_hi, s_lo, pswPPreCurr[i - swIntLag - P_INT_MACS / 2 + j], pswInterpCoefs[j]);
            }
            L_1= L_MLA_SAT_CARRY(s_lo, L_1&1);// L_MLA_SAT(s_hi, s_lo);

            pswPPreCurr[i] = MAC_R(L_1, pswPPreCurr[i - swIntLag + P_INT_MACS / 2 - 1], pswInterpCoefs[P_INT_MACS - 1]);
        }

        /* Get energy in filtered vector, determine automatic-gain-control */
        /* scale factor                                                    */
        /*-----------------------------------------------------------------*/

        swScale = agcGain(pswPPreCurr, snsOrigEnergy, swEnergy);

        /* Scale filtered vector by AGC, put out.  NOTE: AGC scale returned */
        /* by routine above is divided by two, hence the shift below        */
        /*------------------------------------------------------------------*/

        for (i = 0; i < S_LEN; i++)
        {

            L_1 = L_MULT(pswPPreCurr[i], swScale);
            //L_1 = L_shl(L_1, 1);
            //L_1 = L_SHL_SAT(L_1, 1);
            L_1 <<=1 ;
            pswExciteOut[i] = ROUNDO(L_1);
        }

        /* Update pitch pre-filter state */
        /*-------------------------------*/

        for (i = 0; i < LTP_LEN; i++)
            pswPPreStatem[i] = pswPPreStatem[i + S_LEN];
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: r0BasedEnergyShft
 *
 *   PURPOSE:
 *
 *     Given an R0 voicing level, find the number of shifts to be
 *     performed on the energy to ensure that the subframe energy does
 *     not overflow.  example if energy can maximally take the value
 *     4.0, then 2 shifts are required.
 *
 *   INPUTS:
 *
 *     swR0Index
 *                     R0 codeword (0-0x1f)
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swShiftDownSignal
 *
 *                    number of right shifts to apply to energy (0..6)
 *
 *   DESCRIPTION:
 *
 *     Based on the R0, the average frame energy, we can get an
 *     upper bound on the energy any one subframe can take on.
 *     Using this upper bound we can calculate what right shift is
 *     needed to ensure an unsaturated output out of a subframe
 *     energy calculation (g_corr).
 *
 *   REFERENCES: Sub-clause 4.1.9 and 4.2.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: spectral postfilter
 *
 *************************************************************************/

INT16 r0BasedEnergyShft(INT16 swR0Index)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swShiftDownSignal;



    //VPP_HR_PROFILE_FUNCTION_ENTER( r0BasedEnergyShft);


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    if ( swR0Index <= 26 )
    {
        if ( swR0Index <= 23 )
        {
            if ( swR0Index <= 21 )
                swShiftDownSignal = 0;         /* r0  [0,  21] */
            else
                swShiftDownSignal = 1;         /* r0  [22, 23] */
        }
        else
        {
            if ( swR0Index <= 24 )
                swShiftDownSignal = 2;         /* r0  [23, 24] */
            else
                swShiftDownSignal = 3;         /* r0  [24, 26] */
        }
    }
    else
    {
        /* r0 index > 26 */
        if ( swR0Index <= 28 )
        {
            swShiftDownSignal = 4;           /* r0  [26, 28] */
        }
        else
        {
            if ( swR0Index <= 29 )
                swShiftDownSignal = 5;         /* r0  [28, 29] */
            else
                swShiftDownSignal = 6;         /* r0  [29, 31] */
        }
    }
    if ( swR0Index > 18 )
        //swShiftDownSignal = ADD_SAT16(swShiftDownSignal, 2);
        swShiftDownSignal = ADD(swShiftDownSignal, 2);

    //VPP_HR_PROFILE_FUNCTION_EXIT( r0BasedEnergyShft);

    return (swShiftDownSignal);
}

/***************************************************************************
 *
 *   FUNCTION NAME: rcToADp
 *
 *   PURPOSE:
 *
 *     This subroutine computes a vector of direct form LPC filter
 *     coefficients, given an input vector of reflection coefficients.
 *     Double precision is used internally, but 16 bit direct form
 *     filter coefficients are returned.
 *
 *   INPUTS:
 *
 *     NP
 *                     order of the LPC filter (global constant)
 *
 *     swAscale
 *                     The multiplier which scales down the direct form
 *                     filter coefficients.
 *
 *     pswRc[0:NP-1]
 *                     The input vector of reflection coefficients.
 *
 *   OUTPUTS:
 *
 *     pswA[0:NP-1]
 *                     Array containing the scaled down direct form LPC
 *                     filter coefficients.
 *
 *   RETURN VALUE:
 *
 *     siLimit
 *                     1 if limiting occured in computation, 0 otherwise.
 *
 *   DESCRIPTION:
 *
 *     This function performs the conversion from reflection coefficients
 *     to direct form LPC filter coefficients. The direct form coefficients
 *     are scaled by multiplication by swAscale. NP, the filter order is 10.
 *     The a's and rc's each have NP elements in them. Double precision
 *     calculations are used internally.
 *
 *        The equations are:
 *        for i = 0 to NP-1{
 *
 *          a(i)(i) = rc(i)              (scaling by swAscale occurs here)
 *
 *          for j = 0 to i-1
 *             a(i)(j) = a(i-1)(j) + rc(i)*a(i-1)(i-j-1)
 *       }
 *
 *     See page 443, of
 *     "Digital Processing of Speech Signals" by L.R. Rabiner and R.W.
 *     Schafer; Prentice-Hall; Englewood Cliffs, NJ (USA).  1978.
 *
 *   REFERENCES: Sub-clause 4.1.7 and 4.2.3 of GSM Recomendation 06.20
 *
 *  KEYWORDS: reflectioncoefficients, parcors, conversion, rctoadp, ks, as
 *  KEYWORDS: parcorcoefficients, lpc, flat, vectorquantization
 *
 *************************************************************************/

short  rcToADp(INT16 swAscale, INT16 pswRc[],
               INT16 pswA[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 pL_ASpace[NP],
          pL_tmpSpace[NP],
          L_temp,
          *pL_A,
          *pL_tmp,
          *pL_swap;

    short int i,
          j,                            /* loop counters */
          siLimit;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Initialize starting addresses for temporary buffers */
    /*-----------------------------------------------------*/

    pL_A = pL_ASpace;
    pL_tmp = pL_tmpSpace;

    /* Initialize the flag for checking if limiting has occured */
    /*----------------------------------------------------------*/

    siLimit = 0;

    /* Compute direct form filter coefficients, pswA[0],...,pswA[9] */
    /*-------------------------------------------------------------------*/

    for (i = 0; i < NP; i++)
    {

        pL_tmp[i] = L_MULT(swAscale, pswRc[i]);
        for (j = 0; j <= i - 1; j++)
        {
            L_temp = L_mpy_ls(pL_A[i - j - 1], pswRc[i]);
            //pL_tmp[j] = L_ADD(L_temp, pL_A[j]);
            pL_tmp[j] =  ADD(L_temp, pL_A[j]);
            siLimit |= isLwLimit(pL_tmp[j]);
        }
        if (i != NP - 1)
        {
            /* Swap swA and swTmp buffers */

            pL_swap = pL_tmp;
            pL_tmp = pL_A;
            pL_A = pL_swap;
        }
    }

    for (i = 0; i < NP; i++)
    {
        pswA[i] = ROUNDO(pL_tmp[i]);
        siLimit |= isSwLimit(pswA[i]);
    }
    return (siLimit);
}

/***************************************************************************
 *
 *   FUNCTION NAME: rcToCorrDpL
 *
 *   PURPOSE:
 *
 *     This subroutine computes an autocorrelation vector, given a vector
 *     of reflection coefficients as an input. Double precision calculations
 *     are used internally, and a double precision (INT32)
 *     autocorrelation sequence is returned.
 *
 *   INPUTS:
 *
 *     NP
 *                     LPC filter order passed in as a global constant.
 *
 *     swAshift
 *                     Number of right shifts to be applied to the
 *                     direct form filter coefficients being computed
 *                     as an intermediate step to generating the
 *                     autocorrelation sequence.
 *
 *     swAscale
 *                     A multiplicative scale factor corresponding to
 *                     swAshift; i.e. swAscale = 2 ^(-swAshift).
 *
 *     pswRc[0:NP-1]
 *                     An input vector of reflection coefficients.
 *
 *   OUTPUTS:
 *
 *     pL_R[0:NP]
 *                     An output INT32 array containing the
 *                     autocorrelation vector where
 *                     pL_R[0] = 0x7fffffff; (i.e., ~1.0).
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     The algorithm used for computing the correlation sequence is
 *     described on page 232 of the book "Linear Prediction of Speech",
 *     by J.D.  Markel and A.H. Gray, Jr.; Springer-Verlag, Berlin,
 *     Heidelberg, New York, 1976.
 *
 *   REFERENCES: Sub_Clause 4.1.4 and 4.2.1  of GSM Recomendation 06.20
 *
 *   KEYWORDS: normalized autocorrelation, reflection coefficients
 *   KEYWORDS: conversion
 *
 **************************************************************************/

void   rcToCorrDpL(INT16 swAshift, INT16 swAscale,
                   INT16 pswRc[], INT32 pL_R[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 pL_ASpace[NP],
          pL_tmpSpace[NP],
          L_temp,
          L_sum,
          *pL_A,
          *pL_tmp,
          *pL_swap;

    short int i,
          j;                            /* loop control variables */

    VPP_HR_PROFILE_FUNCTION_ENTER( rcToCorrDpL);

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Set R[0] = 0x7fffffff, (i.e., R[0] = 1.0) */
    /*-------------------------------------------*/

    pL_R[0] = LW_MAX;

    /* Assign an address onto each of the two temporary buffers */
    /*----------------------------------------------------------*/

    pL_A = pL_ASpace;
    pL_tmp = pL_tmpSpace;

    /* Compute correlations R[1],...,R[10] */
    /*------------------------------------*/

    for (i = 0; i < NP; i++)
    {

        /* Compute, as an intermediate step, the filter coefficients for */
        /* for an i-th order direct form filter (pL_tmp[j],j=0,i)        */
        /*---------------------------------------------------------------*/

        pL_tmp[i] = L_MULT(swAscale, pswRc[i]);
        for (j = 0; j <= i - 1; j++)
        {
            L_temp = L_mpy_ls(pL_A[i - j - 1], pswRc[i]);
            //pL_tmp[j] = L_ADD(L_temp, pL_A[j]);
            pL_tmp[j] =  ADD(L_temp, pL_A[j]);

        }

        /* Swap pL_A and pL_tmp buffers */
        /*------------------------------*/

        pL_swap = pL_A;
        pL_A = pL_tmp;
        pL_tmp = pL_swap;

        /* Given the direct form filter coefficients for an i-th order filter  */
        /* and the autocorrelation vector computed up to and including stage i */
        /* compute the autocorrelation coefficient R[i+1]                      */
        /*---------------------------------------------------------------------*/

        L_temp = L_mpy_ll(pL_A[0], pL_R[i]);
        L_sum = L_NEGATE(L_temp);

        for (j = 1; j <= i; j++)
        {
            L_temp = L_mpy_ll(pL_A[j], pL_R[i - j]);
            //L_sum = L_SUB(L_sum, L_temp);
            L_sum -=   L_temp ;

        }
        //pL_R[i + 1] = L_shl(L_sum, swAshift);
        //pL_R[i + 1] = L_SHL_SAT(L_sum, swAshift);
        pL_R[i + 1] =  SHL (L_sum, swAshift);

    }
    VPP_HR_PROFILE_FUNCTION_EXIT( rcToCorrDpL);
}

/***************************************************************************
 *
 *   FUNCTION NAME: res_eng
 *
 *   PURPOSE:
 *
 *     Calculates square root of subframe residual energy estimate:
 *
 *                     sqrt( R(0)(1-k1**2)...(1-k10**2) )
 *
 *   INPUTS:
 *
 *     pswReflecCoefIn[0:9]
 *
 *                     Array of reflection coeffcients.
 *
 *     swRq
 *
 *                     Subframe energy = sqrt(frame_energy * S_LEN/2**S_SH)
 *                     (quantized).
 *
 *   OUTPUTS:
 *
 *     psnsSqrtRsOut
 *
 *                     (Pointer to) the output residual energy estimate.
 *
 *   RETURN VALUE:
 *
 *     The shift count of the normalized residual energy estimate, as int.
 *
 *   DESCRIPTION:
 *
 *     First, the canonic product of the (1-ki**2) terms is calculated
 *     (normalizations are done to maintain precision).  Also, a factor of
 *     2**S_SH is applied to the product to offset this same factor in the
 *     quantized square root of the subframe energy.
 *
 *     Then the product is square-rooted, and multiplied by the quantized
 *     square root of the subframe energy.  This combined product is put
 *     out as a normalized fraction and shift count (mantissa and exponent).
 *
 *   REFERENCES: Sub-clause 4.1.7 and 4.2.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: residualenergy, res_eng, rs
 *
 *************************************************************************/

void   res_eng(INT16 pswReflecCoefIn[], INT16 swRq,
               struct NormSw *psnsSqrtRsOut)
{
    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  S_SH          6               /* ceiling(log2(S_LEN)) */
#define  MINUS_S_SH    -S_SH


    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Product,
          L_Shift,
          L_SqrtResEng;

    INT16 swPartialProduct,
          swPartialProductShift,
          swTerm,
          swShift,
          swSqrtPP,
          swSqrtPPShift,
          swSqrtResEng,
          swSqrtResEngShift;

    short int i;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Form canonic product, maintain precision and shift count */
    /*----------------------------------------------------------*/

    /* (Start off with unity product (actually -1), and shift offset) */
    /*----------------------------------------------------------------*/
    swPartialProduct = SW_MIN;
    swPartialProductShift = MINUS_S_SH;

    for (i = 0; i < NP; i++)
    {

        /* Get next (-1 + k**2) term, form partial canonic product */
        /*---------------------------------------------------------*/


        swTerm = MAC_R(LW_MIN, pswReflecCoefIn[i], pswReflecCoefIn[i]);

        L_Product = L_MULT(swTerm, swPartialProduct);

        /* Normalize partial product, round */
        /*----------------------------------*/

        swShift = norm_s(EXTRACT_H(L_Product));
        //swPartialProduct = ROUNDO(L_shl(L_Product, swShift));
        //swPartialProduct = ROUNDO(L_SHL_SAT(L_Product, swShift));
        swPartialProduct = ROUNDO( SHL (L_Product, swShift));
        //swPartialProductShift = ADD_SAT16(swPartialProductShift, swShift);
        swPartialProductShift = ADD(swPartialProductShift, swShift);

    }

    /* Correct sign of product, take square root */
    /*-------------------------------------------*/

    //swPartialProduct = abs_s(swPartialProduct);
    swPartialProduct = ABS_S(swPartialProduct);

    swSqrtPP = sqroot(L_DEPOSIT_H(swPartialProduct));

    //L_Shift = L_shr(L_DEPOSIT_H(swPartialProductShift), 1);
    L_Shift = SHR(L_DEPOSIT_H(swPartialProductShift), 1);

    swSqrtPPShift = EXTRACT_H(L_Shift);

    if (EXTRACT_L(L_Shift) != 0)
    {

        /* Odd exponent: shr above needs to be compensated by multiplying */
        /* mantissa by sqrt(0.5)                                          */
        /*----------------------------------------------------------------*/

        swSqrtPP = MULT_R(swSqrtPP, SQRT_ONEHALF);
    }

    /* Form final product, the residual energy estimate, and do final */
    /* normalization                                                  */
    /*----------------------------------------------------------------*/

    L_SqrtResEng = L_MULT(swRq, swSqrtPP);

    swShift = norm_l(L_SqrtResEng);
    //swSqrtResEng = ROUNDO(L_shl(L_SqrtResEng, swShift));
    //swSqrtResEng = ROUNDO(L_SHL_SAT(L_SqrtResEng, swShift));
    swSqrtResEng = ROUNDO( SHL (L_SqrtResEng, swShift));
    //swSqrtResEngShift = ADD_SAT16(swSqrtPPShift, swShift);
    swSqrtResEngShift = ADD(swSqrtPPShift, swShift);

    /* Return */
    /*--------*/
    psnsSqrtRsOut->man = swSqrtResEng;
    psnsSqrtRsOut->sh = swSqrtResEngShift;

    return;
}

/***************************************************************************
 *
 *   FUNCTION NAME: rs_rr
 *
 *   PURPOSE:
 *
 *     Calculates sqrt(RS/R(x,x)) using floating point format,
 *     where RS is the approximate residual energy in a given
 *     subframe and R(x,x) is the power in each long term
 *     predictor vector or in each codevector.
 *     Used in the joint optimization of the gain and the long
 *     term predictor coefficient.
 *
 *   INPUTS:
 *
 *     pswExcitation[0:39] - excitation signal array
 *     snsSqrtRs - structure sqrt(RS) normalized with mantissa and shift
 *
 *   OUTPUTS:
 *
 *     snsSqrtRsRr - structure sqrt(RS/R(x,x)) with mantissa and shift
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   DESCRIPTION:
 *
 *     Implemented as sqrt(RS)/sqrt(R(x,x)) where both sqrts
 *     are stored normalized (0.5<=x<1.0) and the associated
 *     shift.  See section 4.1.11.1 for details
 *
 *   REFERENCES: Sub-clause 4.1.11.1 and 4.2.1 of GSM
 *               Recomendation 06.20
 *
 *   KEYWORDS: rs_rr, sqroot
 *
 *************************************************************************/

void   rs_rr(INT16 pswExcitation[], struct NormSw snsSqrtRs,
             struct NormSw *snsSqrtRsRr)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT32 L_Temp;
    INT16 swTemp,
          swTemp2,
          swEnergy,
          swNormShift,
          swShift;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    swEnergy = SUB(SHL(snsSqrtRs.sh, 1), 3);      /* shift*2 + margin ==
                                                 * energy. */


    if (swEnergy < 0)
    {

        /* High-energy residual: scale input vector during energy */
        /* calculation. The shift count of the energy of the      */
        /* residual estimate is used as an estimate of the shift  */
        /* count needed for the excitation energy                 */
        /*--------------------------------------------------------*/

        swNormShift = g_corr1s(pswExcitation, NEGATE(swEnergy), &L_Temp);

    }
    else
    {

        /* Lower-energy residual: no overflow protection needed */
        /*------------------------------------------------------*/

        swNormShift = g_corr1(pswExcitation, &L_Temp);
    }

    /* Compute single precision square root of energy sqrt(R(x,x)) */
    /* ----------------------------------------------------------- */
    swTemp = sqroot(L_Temp);

    /* If odd no. of shifts compensate by sqrt(0.5) */
    /* -------------------------------------------- */
    if (swNormShift & 1)
    {

        /* Decrement no. of shifts in accordance with sqrt(0.5) */
        /* ---------------------------------------------------- */
        swNormShift--;// = sub(swNormShift, 1);

        /* sqrt(R(x,x) = sqrt(R(x,x)) * sqrt(0.5) */
        /* -------------------------------------- */
        L_Temp = L_MULT(0x5a82, swTemp);
    }
    else
    {
        L_Temp = L_DEPOSIT_H(swTemp);
    }

    /* Normalize again and update shifts */
    /* --------------------------------- */
    swShift = norm_l(L_Temp);
    //swNormShift = ADD_SAT16(shr(swNormShift, 1), swShift);
    swNormShift = ADD((SHR(swNormShift, 1)), swShift);
    //L_Temp = L_shl(L_Temp, swShift);
    //L_Temp = L_SHL_SAT(L_Temp, swShift);
    L_Temp <<= swShift;

    /* Shift sqrt(RS) to make sure less than divisor */
    /* --------------------------------------------- */
    swTemp = SHR(snsSqrtRs.man, 1);

    /* Divide sqrt(RS)/sqrt(R(x,x)) */
    /* ---------------------------- */
    //swTemp2 = divide_s(swTemp, ROUNDO(L_Temp));
    swTemp2 = DIVIDE_SAT16(swTemp, ROUNDO(L_Temp));


    /* Calculate shift for division, compensate for shift before division */
    /* ------------------------------------------------------------------ */
    swNormShift = SUB(snsSqrtRs.sh, swNormShift);
    swNormShift--;// = sub(swNormShift, 1);

    /* Normalize and get no. of shifts */
    /* ------------------------------- */
    swShift = norm_s(swTemp2);
    snsSqrtRsRr->sh = ADD(swNormShift, swShift);
    snsSqrtRsRr->man = SHL(swTemp2, swShift);

}

/***************************************************************************
 *
 *   FUNCTION NAME: rs_rrNs
 *
 *   PURPOSE:
 *
 *     Calculates sqrt(RS/R(x,x)) using floating point format,
 *     where RS is the approximate residual energy in a given
 *     subframe and R(x,x) is the power in each long term
 *     predictor vector or in each codevector.
 *
 *     Used in the joint optimization of the gain and the long
 *     term predictor coefficient.
 *
 *   INPUTS:
 *
 *     pswExcitation[0:39] - excitation signal array
 *     snsSqrtRs - structure sqrt(RS) normalized with mantissa and shift
 *
 *   OUTPUTS:
 *
 *     snsSqrtRsRr - structure sqrt(RS/R(x,x)) with mantissa and shift
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   DESCRIPTION:
 *
 *     Implemented as sqrt(RS)/sqrt(R(x,x)) where both sqrts
 *     are stored normalized (0.5<=x<1.0) and the associated
 *     shift.
 *
 *   REFERENCES: Sub-clause 4.1.11.1 and 4.2.1 of GSM
 *               Recomendation 06.20
 *
 *   KEYWORDS: rs_rr, sqroot
 *
 *************************************************************************/

void   rs_rrNs(INT16 pswExcitation[], struct NormSw snsSqrtRs,
               struct NormSw *snsSqrtRsRr)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT32 L_Temp;
    INT16 swTemp,
          swTemp2,
          swNormShift,
          swShift;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Lower-energy residual: no overflow protection needed */
    /*------------------------------------------------------*/

    swNormShift = g_corr1(pswExcitation, &L_Temp);


    /* Compute single precision square root of energy sqrt(R(x,x)) */
    /* ----------------------------------------------------------- */
    swTemp = sqroot(L_Temp);

    /* If odd no. of shifts compensate by sqrt(0.5) */
    /* -------------------------------------------- */
    if (swNormShift & 1)
    {

        /* Decrement no. of shifts in accordance with sqrt(0.5) */
        /* ---------------------------------------------------- */
        swNormShift--;// = sub(swNormShift, 1);

        /* sqrt(R(x,x) = sqrt(R(x,x)) * sqrt(0.5) */
        /* -------------------------------------- */
        L_Temp = L_MULT(0x5a82, swTemp);
    }
    else
    {
        L_Temp = L_DEPOSIT_H(swTemp);
    }

    /* Normalize again and update shifts */
    /* --------------------------------- */

    swShift = norm_l(L_Temp);
    //swNormShift = ADD_SAT16(shr(swNormShift, 1), swShift);
    swNormShift = ADD((SHR(swNormShift, 1)), swShift);
    //L_Temp = L_shl(L_Temp, swShift);
    //L_Temp = L_SHL_SAT(L_Temp, swShift);
    L_Temp <<= swShift;

    /* Shift sqrt(RS) to make sure less than divisor */
    /* --------------------------------------------- */
    swTemp = SHR(snsSqrtRs.man, 1);

    /* Divide sqrt(RS)/sqrt(R(x,x)) */
    /* ---------------------------- */
    //swTemp2 = divide_s(swTemp, ROUNDO(L_Temp));
    swTemp2 = DIVIDE_SAT16(swTemp, ROUNDO(L_Temp));

    /* Calculate shift for division, compensate for shift before division */
    /* ------------------------------------------------------------------ */
    swNormShift = SUB(snsSqrtRs.sh, swNormShift);
    swNormShift--;// = sub(swNormShift, 1);

    /* Normalize and get no. of shifts */
    /* ------------------------------- */
    swShift = norm_s(swTemp2);
    //snsSqrtRsRr->sh = ADD_SAT16(swNormShift, swShift);
    snsSqrtRsRr->sh = ADD(swNormShift, swShift);
    snsSqrtRsRr->man = SHL(swTemp2, swShift);

}


/***************************************************************************
 *
 *   FUNCTION NAME: scaleExcite
 *
 *   PURPOSE:
 *
 *     Scale an arbitrary excitation vector (codevector or
 *     pitch vector)
 *
 *   INPUTS:
 *
 *     pswVect[0:39] - the unscaled vector.
 *     iGsp0Scale - an positive offset to compensate for the fact
 *                  that GSP0 table is scaled down.
 *     swErrTerm - rather than a gain being passed in, (beta, gamma)
 *                 it is calculated from this error term - either
 *                 Gsp0[][][0] error term A or Gsp0[][][1] error
 *                 term B. Beta is calculated from error term A,
 *                 gamma from error term B.
 *     snsRS - the RS_xx appropriate to pswVect.
 *
 *   OUTPUTS:
 *
 *      pswScldVect[0:39] - the output, scaled excitation vector.
 *
 *   RETURN VALUE:
 *
 *     swGain - One of two things.  Either a clamped value of 0x7fff if the
 *              gain's shift was > 0 or the rounded vector gain otherwise.
 *
 *   DESCRIPTION:
 *
 *     If gain > 1.0 then
 *         (do not shift gain up yet)
 *         partially scale vector element THEN shift and round save
 *      else
 *         shift gain correctly
 *         scale vector element and round save
 *         update state array
 *
 *   REFERENCES: Sub-clause 4.1.10.2 and 4.2.1 of GSM
 *               Recomendation 06.20
 *
 *   KEYWORDS: excite_vl, sc_ex, excitevl, scaleexcite, codevector, p_vec,
 *   KEYWORDS: x_vec, pitchvector, gain, gsp0
 *
 *************************************************************************/

INT16 scaleExcite(INT16 pswVect[],
                  INT16 swErrTerm, struct NormSw snsRS,
                  INT16 pswScldVect[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT32 L_GainUs,
          L_scaled,
          L_Round_off;//,Lmult,essai
    INT16 swGain,
          swGainUs,
          swGainShift,
          i,
          swGainUsShft;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */


    L_GainUs = L_MULT(swErrTerm, snsRS.man);
    swGainUsShft = norm_s(EXTRACT_H(L_GainUs));
    //L_GainUs = L_shl(L_GainUs, swGainUsShft);
    //L_GainUs = L_SHL_SAT(L_GainUs, swGainUsShft);
    L_GainUs <<=  swGainUsShft ;

    swGainShift = ADD(swGainUsShft, snsRS.sh);
    swGainShift -=GSP0_SCALE;// SUB(swGainShift, GSP0_SCALE);


    /* gain > 1.0 */
    /* ---------- */

    if (swGainShift < 0)
    {
        swGainUs = ROUNDO(L_GainUs);

        //L_Round_off = L_shl((long) 32768, swGainShift);
        L_Round_off = SHR((INT32)0x8000, ABS_S(swGainShift));



        for (i = 0; i < S_LEN; i++)
        {
            s_lo=SHR(L_Round_off,1);
            /*L_scaled = L_mac(L_Round_off, swGainUs, pswVect[i]);
            Lmult=L_MULT(swGainUs, pswVect[i]);
            L_scaled=L_ADD(L_Round_off,Lmult);*/
            VPP_MLA16(s_hi, s_lo, swGainUs, pswVect[i]);

            L_scaled= L_MLA_SAT_CARRY(s_lo, L_Round_off&1);//L_MLA_SAT(s_hi, s_lo);

            //L_scaled = L_shr(L_scaled, swGainShift);
            L_scaled = L_SHR_V(L_scaled, swGainShift);
            pswScldVect[i] = EXTRACT_H(L_scaled);
        }

        if (swGainShift == 0)
            swGain = swGainUs;
        else
            swGain = 0x7fff;
    }

    /* gain < 1.0 */
    /* ---------- */

    else
    {

        /* shift down or not at all */
        /* ------------------------ */
        if (swGainShift > 0)
            //L_GainUs = L_shr(L_GainUs, swGainShift);
            L_GainUs = SHR(L_GainUs, swGainShift);

        /* the rounded actual vector gain */
        /* ------------------------------ */
        swGain = ROUNDO(L_GainUs);

        /* now scale the vector (with rounding) */
        /* ------------------------------------ */

        for (i = 0; i < S_LEN; i++)
        {
            /*L_scaled = L_mac((long) 32768, swGain, pswVect[i]);
            Lmult=L_MULT(swGain, pswVect[i]);
            L_scaled=L_ADD((long) 32768,Lmult);*/
            s_lo=0x4000;//SHR(32768,1);
            VPP_MLA16(s_hi, s_lo, swGain, pswVect[i]);

            L_scaled= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

            pswScldVect[i] = EXTRACT_H(L_scaled);
        }
    }
    return (swGain);
}

/***************************************************************************
 *
 *   FUNCTION NAME: spectralPostFilter
 *
 *   PURPOSE:
 *
 *     Perform spectral post filter on the output of the
 *     synthesis filter.
 *
 *
 *   INPUT:
 *
 *      S_LEN         a global constant
 *
 *      pswSPFIn[0:S_LEN-1]
 *
 *                    input to the routine. Unmodified
 *                    pswSPFIn[0] is the oldest point (first to be filtered),
 *                    pswSPFIn[iLen-1] is the last pointer filtered,
 *                    the newest.
 *
 *      pswNumCoef[0:NP-1],pswDenomCoef[0:NP-1]
 *
 *                    numerator and denominator
 *                    direct form coeffs used by postfilter.
 *                    Exactly like lpc coefficients in format.  Shifted down
 *                    by iAShift to ensure that they are < 1.0.
 *
 *      gpswPostFiltStateNum[0:NP-1], gpswPostFiltStateDenom[0:NP-1]
 *
 *                    array of the filter state.
 *                    Same format as coefficients: *praState = state of
 *                    filter for delay n = -1 praState[NP] = state of
 *                    filter for delay n = -NP These numbers are not
 *                    shifted at all.  These states are static to this
 *                    file.
 *
 *   OUTPUT:
 *
 *      gpswPostFiltStateNum[0:NP-1], gpswPostFiltStateDenom[0:NP-1]
 *
 *                      See above for description.  These are updated.
 *
 *      pswSPFOut[0:S_LEN-1]
 *
 *                    same format as pswSPFIn,
 *                    *pswSPFOut is oldest point. The filtered output.
 *                    Note this routine can handle pswSPFOut = pswSPFIn.
 *                    output can be the same as input.  i.e. in place
 *                    calculation.
 *
 *   RETURN:
 *
 *      none
 *
 *   DESCRIPTION:
 *
 *      find energy in input,
 *      perform the numerator fir
 *      perform the denominator iir
 *      perform the post emphasis
 *      find energy in signal,
 *      perform the agc using energy in and energy in signam after
 *      post emphasis signal
 *
 *      The spectral postfilter is described in section 4.2.4.
 *
 *   REFERENCES: Sub-clause 4.2.4 of GSM Recomendation 06.20
 *
 *   KEYWORDS: postfilter, emphasis, postemphasis, brightness,
 *   KEYWORDS: numerator, deminator, filtering, lpc,
 *
 *************************************************************************/

static void spectralPostFilter(INT16 pswSPFIn[],
                               INT16 pswNumCoef[],
                               INT16 pswDenomCoef[], INT16 pswSPFOut[])
{
    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define  AGC_COEF       (INT16)0x19a        /* (1.0 - POST_AGC_COEF)
                                                 * 1.0-.9875 */
#define  POST_EMPHASIS  (INT16)0x199a       /* 0.2 */

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int i;

    INT32 L_OrigEnergy,
          L_runningGain,
          L_Output;//,Lmult

    INT16 swAgcGain,
          swRunningGain,
          swTemp;//,essai,temp;

    struct NormSw snsOrigEnergy;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* calculate the energy in the input and save it */
    /*-----------------------------------------------*/

    snsOrigEnergy.sh = g_corr1s(pswSPFIn, swEngyRShift, &L_OrigEnergy);
    snsOrigEnergy.man = ROUNDO(L_OrigEnergy);

    /* numerator of the postfilter */
    /*-----------------------------*/

    lpcFir(pswSPFIn, pswNumCoef, gpswPostFiltStateNum, pswSPFOut);

    /* denominator of the postfilter */
    /*-------------------------------*/

    lpcIir(pswSPFOut, pswDenomCoef, gpswPostFiltStateDenom, pswSPFOut);

    /* postemphasis section of postfilter */
    /*------------------------------------*/

    for (i = 0; i < S_LEN; i++)
    {
        //s_hi=0;
        //s_lo=SHR(L_DEPOSIT_H(pswSPFOut[i]),1);
        swTemp = MSU_R(L_DEPOSIT_H(pswSPFOut[i]), swPostEmphasisState, POST_EMPHASIS);
        /*VPP_MLA16(s_hi, (INT32)s_lo, -swPostEmphasisState, POST_EMPHASIS);
        swTemp = ROUNDO( L_MLA_SAT(s_hi,s_lo));*/

        swPostEmphasisState = pswSPFOut[i];
        pswSPFOut[i] = swTemp;
    }

    swAgcGain = agcGain(pswSPFOut, snsOrigEnergy, swEngyRShift);

    /* scale the speech vector */
    /*-----------------------------*/

    swRunningGain = gswPostFiltAgcGain;
    L_runningGain = L_DEPOSIT_H(gswPostFiltAgcGain);
    s_lo=SHR( L_runningGain,1);
    for (i = 0; i < S_LEN; i++)
    {
        //L_runningGain = L_msu(L_runningGain, swRunningGain, AGC_COEF);
        VPP_MLA16(s_hi, s_lo, -swRunningGain, AGC_COEF);

        //L_runningGain = L_mac(L_runningGain, swAgcGain, AGC_COEF);
        /*Lmult=L_MULT(swAgcGain, AGC_COEF);
        L_runningGain=L_ADD(L_runningGain,Lmult);*/
        VPP_MLA16(s_hi, s_lo, swAgcGain, AGC_COEF);

        L_runningGain= L_MLA_SAT_CARRY(s_lo, L_runningGain&1);//L_MLA_SAT(s_hi, s_lo);

        swRunningGain = EXTRACT_H(L_runningGain);

        /* now scale input with gain */

        L_Output = L_MULT(swRunningGain, pswSPFOut[i]);
        //pswSPFOut[i] = EXTRACT_H(L_shl(L_Output, 2));
        pswSPFOut[i] = EXTRACT_H(SHL(L_Output, 2));

    }
    gswPostFiltAgcGain = swRunningGain;

}




static void dePackEncDec(UINT16* In, UINT16* Out)
{
    int i;

    for (i=0; i<18; i++)
        Out[i] =0;

    Out[0] = ( In[0] & 0x001f)                                    ; //RO  :  5 bits
    Out[1] = ((In[0] & 0xffe0)>>5)                                ; //LPC1: 11 bits
    Out[2] = ( In[1] & 0x01ff)                                    ; //LPC2:  9 bits
    Out[3] = ((In[1] & 0xfe00)>>9) | ((In[2] & 0x0001)<<7)        ; //LPC3:  8 bits=7+1
    Out[4] = ( In[2] & 0x0002)>>1                                 ; //INT_LPC
    Out[5] = ( In[2] & 0x000c)>>2                                 ; //MODE

    if (Out[5])
    {
        Out[6] = ( In[2] & 0x0ff0)>>4                                 ; //LAG_1
        Out[7] = ((In[2] & 0xf000)>>12) | ((In[3] & 0x001f)<<4)       ; //CODE_1
        Out[8] = ( In[3] & 0x03e0)>>5                                 ; //GSPO_1
        Out[9] = ( In[3] & 0x3c00)>>10                                ; //LAG_2
        Out[10]= ((In[3] & 0xc000)>>14) | ((In[4] & 0x007f)<<2)       ; //CODE_2
        Out[11]= ( In[4] & 0x0f80)>>7                                 ; //GSPO_2
        Out[12]= ( In[4] & 0xf000)>>12                                ; //LAG_3
        Out[13]= ( In[5] & 0x01ff)                                    ; //CODE_3
        Out[14]= ( In[5] & 0x3e00)>>9                                 ; //GSPO_3
        Out[15]= ((In[5] & 0xc000)>>14) | ((In[6] & 0x0003)<<2)       ; //LAG_4
        Out[16]= ( In[6] & 0x07fc)>>2                                 ; //CODE_4
        Out[17]= ( In[6] & 0xf800)>>11                                ; //GSPO_4
    }
    else
    {
        Out[6] = ( In[2] & 0x07f0)>>4                                 ; //CODE1_1
        Out[7] = ((In[2] & 0xf800)>>11) | ((In[3] & 0x0003)<<5)       ; //CODE2_1
        Out[8] = ( In[3] & 0x007c)>>2                                 ; //GSPO_1
        Out[9] = ( In[3] & 0x3f80)>>7                                 ; //CODE1_2
        Out[10]= ((In[3] & 0xc000)>>14) | ((In[4] & 0x001f)<<2)       ; //CODE2_2
        Out[11]= ( In[4] & 0x03e0)>>5                                 ; //GSPO_2
        Out[12]= ((In[4] & 0xfc00)>>10) | ((In[5] & 0x0001)<<6)       ; //CODE1_3
        Out[13]= ( In[5] & 0x00fe)>>1                                 ; //CODE2_3
        Out[14]= ( In[5] & 0x1f00)>>8                                 ; //GSPO_3
        Out[15]= ((In[5] & 0xe000)>>13) | ((In[6] & 0x000f)<<3)       ; //CODE1_4
        Out[16]= ( In[6] & 0x07f0)>>4                                 ; //CODE2_4
        Out[17]= ( In[6] & 0xf800)>>11                                ; //GSPO_4
    }

}





/***************************************************************************
 *
 *   FUNCTION NAME: speechDecoder
 *
 *   PURPOSE:
 *     The purpose of this function is to call all speech decoder
 *     subroutines.  This is the top level routine for the speech
 *     decoder.
 *
 *   INPUTS:
 *
 *     pswParameters[0:21]
 *
 *        pointer to this frame's parameters.  See below for input
 *        data format.
 *
 *   OUTPUTS:
 *
 *     pswDecodedSpeechFrame[0:159]
 *
 *        this frame's decoded 16 bit linear pcm frame
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     The sequence of events in the decoder, and therefore this routine
 *     follow a simple plan.  First, the frame based parameters are
 *     decoded.  Second, on a subframe basis, the subframe based
 *     parameters are decoded and the excitation is generated.  Third,
 *     on a subframe basis, the combined and scaled excitation is
 *     passed through the synthesis filter, and then the pitch and
 *     spectral postfilters.
 *
 *     The in-line comments for the routine speechDecoder, are very
 *     detailed.  Here in a more consolidated form, are the main
 *     points.
 *
 *     The R0 parameter is decoded using the lookup table
 *     psrR0DecTbl[].  The LPC codewords are looked up using lookupVQ().
 *     The decoded parameters are reflection coefficients
 *     (pswFrmKs[]).
 *
 *     The decoder does not use reflection coefficients directly.
 *     Instead it converts them to direct form coeficients.  This is
 *     done using rcToADp().  If this conversion results in invalid
 *     results, the previous frames parameters are used.
 *
 *     The direct form coeficients are used to derive the spectal
 *     postfilter's numerator and denominator coeficients.  The
 *     denominators coefficients are widened, and the numerators
 *     coefficients are a spectrally smoothed version of the
 *     denominator.  The smoothing is done with a_sst().
 *
 *     The frame based LPC coefficients are either used directly as the
 *     subframe coefficients, or are derived through interpolation.
 *     The subframe based coeffiecients are calculated in getSfrmLpc().
 *
 *     Based on voicing mode, the decoder will construct and scale the
 *     excitation in one of two ways.  For the voiced mode the lag is
 *     decoded using lagDecode().  The fractional pitch LTP lookup is
 *     done by the function fp_ex().  In both voiced and unvoiced
 *     mode, the VSELP codewords are decoded into excitation vectors
 *     using b_con() and v_con().
 *
 *     rs_rr(), rs_rrNs(), and scaleExcite() are used to calculate
 *     the gamma's, codevector gains, as well as beta, the LTP vector
 *     gain.  Description of this can be found in section 4.1.11.  Once
 *     the vectors have been scaled and combined, the excitation is
 *     stored in the LTP history.
 *
 *     The excitation, pswExcite[], passes through the pitch pre-filter
 *     (pitchPreFilt()).  Then the harmonically enhanced excitation
 *     passes through the synthesis filter, lpcIir(), and finally the
 *     reconstructed speech passes through the spectral post-filter
 *     (spectalPostFilter()).  The final output speech is passed back in
 *     the array pswDecodedSpeechFrame[].
 *
 *   INPUT DATA FORMAT:
 *
 *      The format/content of the input parameters is the so called
 *      bit alloc format.
 *
 *     voiced mode bit alloc format:
 *     -----------------------------
 *     index    number of bits  parameter name
 *     0        5               R0
 *     1        11              k1Tok3
 *     2        9               k4Tok6
 *     3        8               k7Tok10
 *     4        1               softInterpolation
 *     5        2               voicingDecision
 *     6        8               frameLag
 *     7        9               code_1
 *     8        5               gsp0_1
 *     9        4               deltaLag_2
 *     10       9               code_2
 *     11       5               gsp0_2
 *     12       4               deltaLag_3
 *     13       9               code_3
 *     14       5               gsp0_3
 *     15       4               deltaLag_4
 *     16       9               code_4
 *     17       5               gsp0_4
 *
 *     18       1               BFI
 *     19       1               UFI
 *     20       2               SID
 *     21       1               TAF
 *
 *
 *     unvoiced mode bit alloc format:
 *     -------------------------------
 *
 *     index    number of bits  parameter name
 *     0        5               R0
 *     1        11              k1Tok3
 *     2        9               k4Tok6
 *     3        8               k7Tok10
 *     4        1               softInterpolation
 *     5        2               voicingDecision
 *     6        7               code1_1
 *     7        7               code2_1
 *     8        5               gsp0_1
 *     9        7               code1_2
 *     10       7               code2_2
 *     11       5               gsp0_2
 *     12       7               code1_3
 *     13       7               code2_3
 *     14       5               gsp0_3
 *     15       7               code1_4
 *     16       7               code2_4
 *     17       5               gsp0_4
 *
 *     18       1               BFI
 *     19       1               UFI
 *     20       2               SID
 *     21       1               TAF
 *
 *
 *   REFERENCES: Sub_Clause 4.2 of GSM Recomendation 06.20
 *
 *   KEYWORDS: synthesis, speechdecoder, decoding
 *   KEYWORDS: codewords, lag, codevectors, gsp0
 *
 *************************************************************************/

void   vpp_hr_decode(HAL_SPEECH_DEC_IN_T DecIn, HAL_SPEECH_PCM_HALF_BUF_T DecOut)
//void   speechDecoder(INT16 pswParameters[], INT16 pswDecodedSpeechFrame[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */

    static INT16
    *pswLtpStateOut = &pswLtpStateBaseDec[LTP_LEN],
     pswSythAsSpace[NP * N_SUB],
     pswPFNumAsSpace[NP * N_SUB],
     pswPFDenomAsSpace[NP * N_SUB],
     *ppswSynthAs[N_SUB] =
    {
        &pswSythAsSpace[0],
        &pswSythAsSpace[10],
        &pswSythAsSpace[20],
        &pswSythAsSpace[30],
    },

    *ppswPFNumAs[N_SUB] =
    {
        &pswPFNumAsSpace[0],
        &pswPFNumAsSpace[10],
        &pswPFNumAsSpace[20],
        &pswPFNumAsSpace[30],
    },
    *ppswPFDenomAs[N_SUB] =
    {
        &pswPFDenomAsSpace[0],
        &pswPFDenomAsSpace[10],
        &pswPFDenomAsSpace[20],
        &pswPFDenomAsSpace[30],
    };

    static INT16
    psrSPFDenomWidenCf[NP] =
    {
        0x6000, 0x4800, 0x3600, 0x2880, 0x1E60,
        0x16C8, 0x1116, 0x0CD0, 0x099C, 0x0735,
    };


    static INT32 L_RxPNSeed;          /* DTX mode */
    static INT16 swRxDtxGsIndex;     /* DTX mode */



    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int i,j, siGsp0Code,siVselpCw,siNumBits,siCodeBook;
    short int siLagCode=0;
    short int psiVselpCw[2];

    INT16 pswFrmKs[NP],
          pswFrmAs[NP],
          pswFrmPFNum[NP],
          pswFrmPFDenom[NP],
          pswPVec[S_LEN],
          ppswVselpEx[2][S_LEN],
          pswExcite[S_LEN],
          pswPPFExcit[S_LEN],
          pswSynthFiltOut[S_LEN],
          swR0Index,
          swLag,
          swSemiBeta,
          pswBitArray[MAXBITS];

    struct NormSw psnsSqrtRs[N_SUB],
               snsRs00,
               snsRs11,
               snsRs22;


    INT16 swMutePermit,
          swLevelMean,
          swLevelMax,                   /* error concealment */
          swMuteFlag;                   /* error concealment */


    INT16 swTAF,
          swSID,
          swBfiDtx;                     /* DTX mode */
    INT16 swFrameType;               /* DTX mode */

    INT32 L_RxDTXGs;                  /* DTX mode */

    INT16 pswParameters[22]= {0};
    INT16 pswDecodedSpeechFrame[160]= {0};
    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    VPP_HR_PROFILE_FUNCTION_ENTER( vpp_hr_decode);


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */
    /*for (i = 0; i < 18; i++)
         pswParameters[i]=DecIn.decInBuf[i];*/

    dePackEncDec(&(DecIn.decInBuf[0]), &pswParameters[0]);

    pswParameters[18] = DecIn.bfi;
    pswParameters[19] = DecIn.ufi;
    pswParameters[20] = DecIn.sid;
    pswParameters[21] = DecIn.taf;



    /* -------------------------------------------------------------------- */
    /* do bad frame handling (error concealment) and comfort noise          */
    /* insertion                                                            */
    /* -------------------------------------------------------------------- */


    /* This flag indicates whether muting is performed in the actual frame */
    /* ------------------------------------------------------------------- */
    swMuteFlag = 0;


    /* This flag indicates whether muting is allowed in the actual frame */
    /* ----------------------------------------------------------------- */
    swMutePermit = 0;


    /* frame classification */
    /* -------------------- */

    swSID = pswParameters[20]; //swSID = DecIn.sid;
    swTAF = pswParameters[21]; //swSID = DecIn.taf;

    swBfiDtx = pswParameters[18] | pswParameters[19];// swBfiDtx = DecIn.bfi | DecIn.ufi;    /* BFI | UFI */

    if ((swSID == 2) && (swBfiDtx == 0))
        swFrameType = VALIDSID;
    else if ((swSID == 0) && (swBfiDtx == 0))
        swFrameType = GOODSPEECH;
    else if ((swSID == 0) && (swBfiDtx != 0))
        swFrameType = UNUSABLE;
    else
        swFrameType = INVALIDSID;


    /* update of decoder state */
    /* ----------------------- */

    if (swDecoMode == SPEECH)
    {
        /* speech decoding mode */
        /* -------------------- */

        if (swFrameType == VALIDSID)
            swDecoMode = CNIFIRSTSID;
        else if (swFrameType == INVALIDSID)
            swDecoMode = CNIFIRSTSID;
        else if (swFrameType == UNUSABLE)
            swDecoMode = SPEECH;
        else if (swFrameType == GOODSPEECH)
            swDecoMode = SPEECH;
    }
    else
    {
        /* comfort noise insertion mode */
        /* ---------------------------- */

        if (swFrameType == VALIDSID)
            swDecoMode = CNICONT;
        else if (swFrameType == INVALIDSID)
            swDecoMode = CNICONT;
        else if (swFrameType == UNUSABLE)
            swDecoMode = CNIBFI;
        else if (swFrameType == GOODSPEECH)
            swDecoMode = SPEECH;
    }


    if (swDecoMode == SPEECH)
    {
        /* speech decoding mode */
        /* -------------------- */

        /* Perform parameter concealment, depending on BFI (pswParameters[18]) */
        /* or UFI (pswParameters[19])                                          */
        /* ------------------------------------------------------------------- */
        para_conceal_speech_decoder(&pswParameters[18],
                                    pswParameters, &swMutePermit);


        /* copy the frame rate parameters */
        /* ------------------------------ */

        swR0Index = pswParameters[0];       // swR0Index      = DecIn.decInBuf[0];       /* R0 Index */
        pswVq[0] = pswParameters[1];        // pswVq[0]       = DecIn.decInBuf[1];       /* LPC1 */
        pswVq[1] = pswParameters[2];        // pswVq[1]       = DecIn.decInBuf[2];       /* LPC2 */
        pswVq[2] = pswParameters[3];        // pswVq[2]       = DecIn.decInBuf[3];       /* LPC3 */
        swSi = pswParameters[4];            // swSi           = DecIn.decInBuf[4];       /* INT_LPC */
        swVoicingMode = pswParameters[5];   // swVoicingMode  = DecIn.decInBuf[5];       /* MODE */


        /* lookup R0 and VQ parameters */
        /* --------------------------- */

        swR0Dec = psrR0DecTbl[swR0Index * 2];       /* R0 */
        lookupVq(pswVq, pswFrmKs);


        /* save this frames GS values */
        /* -------------------------- */

        for (i = 0; i < N_SUB; i++)
        {
            pL_RxGsHist[swRxGsHistPtr] =
                ppLr_gsTable[swVoicingMode][pswParameters[(i * 3) + 8]];
            swRxGsHistPtr++;
            if (swRxGsHistPtr > ((OVERHANG - 1) * N_SUB) - 1)
                swRxGsHistPtr = 0;
        }


        /* DTX variables */
        /* ------------- */

        swDtxBfiCnt = 0;
        swDtxMuting = 0;
        swRxDTXState = CNINTPER - 1;

    }
    else
    {
        /* comfort noise insertion mode */
        /*----------------------------- */

        /* copy the frame rate parameters */
        /* ------------------------------ */

        swR0Index = pswParameters[0];      /* R0 Index */
        pswVq[0] = pswParameters[1];       /* LPC1 */
        pswVq[1] = pswParameters[2];       /* LPC2 */
        pswVq[2] = pswParameters[3];       /* LPC3 */
        swSi = 1;                          /* INT_LPC */
        swVoicingMode = 0;                 /* MODE */


        /* bad frame handling in comfort noise insertion mode */
        /* -------------------------------------------------- */

        if (swDecoMode == CNIFIRSTSID)     /* first SID frame */
        {
            swDtxBfiCnt = 0;
            swDtxMuting = 0;
            swRxDTXState = CNINTPER - 1;

            if (swFrameType == VALIDSID)     /* valid SID frame */
            {
                swR0NewCN = psrR0DecTbl[swR0Index * 2];
                lookupVq(pswVq, pswFrmKs);
            }
            else if (swFrameType == INVALIDSID)       /* invalid SID frame */
            {
                swR0NewCN = psrR0DecTbl[swOldR0IndexDec * 2];
                swR0Index = swOldR0IndexDec;
                for (i = 0; i < NP; i++)
                    pswFrmKs[i] = pswOldFrmKsDec[i];
            }

        }
        else if (swDecoMode == CNICONT)    /* SID frame detected, but */
        {
            /* not the first SID       */
            swDtxBfiCnt = 0;
            swDtxMuting = 0;

            if (swFrameType == VALIDSID)     /* valid SID frame */
            {
                swRxDTXState = 0;
                swR0NewCN = psrR0DecTbl[swR0Index * 2];
                lookupVq(pswVq, pswFrmKs);
            }
            else if (swFrameType == INVALIDSID)       /* invalid SID frame */
            {
                if (swRxDTXState < (CNINTPER - 1))
                    //swRxDTXState = ADD_SAT16(swRxDTXState, 1);
                    swRxDTXState++;
                swR0Index = swOldR0IndexDec;
            }

        }
        else if (swDecoMode == CNIBFI)     /* bad frame received in */
        {
            /* CNI mode              */
            if (swRxDTXState < (CNINTPER - 1))
                //swRxDTXState = ADD_SAT16(swRxDTXState, 1);
                swRxDTXState++;
            swR0Index = swOldR0IndexDec;

            if (swDtxMuting == 1)
            {
                //swOldR0IndexDec = sub(swOldR0IndexDec, 2);      /* attenuate by 4 dB */
                swOldR0IndexDec -=2;      /* attenuate by 4 dB */
                if (swOldR0IndexDec < 0)
                    swOldR0IndexDec = 0;

                swR0Index = swOldR0IndexDec;

                swR0NewCN = psrR0DecTbl[swOldR0IndexDec * 2];   /* R0 */

            }

            //swDtxBfiCnt = ADD_SAT16(swDtxBfiCnt, 1);
            swDtxBfiCnt++;
            if ((swTAF == 1) && (swDtxBfiCnt >= (2 * CNINTPER + 1)))  /* 25 */
                swDtxMuting = 1;

        }


        if (swDecoMode == CNIFIRSTSID)
        {

            /* the first SID frame is received */
            /* ------------------------------- */

            /* initialize the decoders pn-generator */
            /* ------------------------------------ */

            L_RxPNSeed = PN_INIT_SEED;


            /* using the stored rx history, generate averaged GS */
            /* ------------------------------------------------- */

            avgGsHistQntz(pL_RxGsHist, &L_RxDTXGs);
            swRxDtxGsIndex = gsQuant(L_RxDTXGs, 0);

        }


        /* Replace the "transmitted" subframe parameters with */
        /* synthetic ones                                     */
        /* -------------------------------------------------- */

        for (i = 0; i < 4; i++)
        {
            /* initialize the GSP0 parameter */
            pswParameters[(i * 3) + 8] = swRxDtxGsIndex;

            /* CODE1 */
            pswParameters[(i * 3) + 6] = getPnBits(7, &L_RxPNSeed);
            /* CODE2 */
            pswParameters[(i * 3) + 7] = getPnBits(7, &L_RxPNSeed);
        }


        /* Interpolation of CN parameters */
        /* ------------------------------ */

        rxInterpR0Lpc(pswOldFrmKsDec, pswFrmKs, swRxDTXState,
                      swDecoMode, swFrameType);

    }


    /* ------------------- */
    /* do frame processing */
    /* ------------------- */

    /* generate the direct form coefs */
    /* ------------------------------ */

    if (!rcToADp(ASCALE, pswFrmKs, pswFrmAs))
    {

        /* widen direct form coefficients using the widening coefs */
        /* ------------------------------------------------------- */

        for (i = 0; i < NP; i++)
        {
            pswFrmPFDenom[i] = MULT_R(pswFrmAs[i], psrSPFDenomWidenCf[i]);
            /*VPP_MULT_R(m_hi, m_lo, pswFrmAs[i], psrSPFDenomWidenCf[i]);
            pswFrmPFDenom[i] = EXTRACT_H( SHL(m_lo,1) );//EXTRACT_H( L_MLA_SAT(m_hi, m_lo) );*/
        }

        a_sst(ASHIFT, ASCALE, pswFrmPFDenom, pswFrmPFNum);
    }
    else
    {


        for (i = 0; i < NP; i++)
        {
            pswFrmKs[i] = pswOldFrmKsDec[i];
            pswFrmAs[i] = pswOldFrmAsDec[i];
            pswFrmPFDenom[i] = pswOldFrmPFDenom[i];
            pswFrmPFNum[i] = pswOldFrmPFNum[i];
        }
    }

    /* interpolate, or otherwise get sfrm reflection coefs */
    /* --------------------------------------------------- */

    getSfrmLpc(swSi, swOldR0Dec, swR0Dec, pswOldFrmKsDec, pswOldFrmAsDec,
               pswOldFrmPFNum, pswOldFrmPFDenom, pswFrmKs, pswFrmAs,
               pswFrmPFNum, pswFrmPFDenom, psnsSqrtRs, ppswSynthAs,
               ppswPFNumAs, ppswPFDenomAs);

    /* calculate shift for spectral postfilter */
    /* --------------------------------------- */

    swEngyRShift = r0BasedEnergyShft(swR0Index);


    /* ----------------------- */
    /* do sub-frame processing */
    /* ----------------------- */

    for (giSfrmCnt = 0; giSfrmCnt < 4; giSfrmCnt++)
    {

        /* copy this sub-frame's parameters */
        /* -------------------------------- */

        //if (sub(swVoicingMode, 0) == 0)
        if ( swVoicingMode== 0 )
        {
            /* unvoiced */
            psiVselpCw[0] = pswParameters[(giSfrmCnt * 3) + 6];       /* CODE_1 */
            psiVselpCw[1] = pswParameters[(giSfrmCnt * 3) + 7];       /* CODE_2 */
            siGsp0Code = pswParameters[(giSfrmCnt * 3) + 8];  /* GSP0 */
        }
        else
        {
            /* voiced */
            siLagCode = pswParameters[(giSfrmCnt * 3) + 6];   /* LAG */
            psiVselpCw[0] = pswParameters[(giSfrmCnt * 3) + 7];       /* CODE */
            siGsp0Code = pswParameters[(giSfrmCnt * 3) + 8];  /* GSP0 */
        }

        /* for voiced mode, reconstruct the pitch vector */
        /* --------------------------------------------- */

        if (swVoicingMode)
        {

            /* convert delta lag to lag and convert to fractional lag */
            /* ------------------------------------------------------ */

            swLag = lagDecode(siLagCode);

            /* state followed by out */
            /* --------------------- */

            fp_ex(swLag, pswLtpStateOut);

            /* extract a piece of pswLtpStateOut into newly named vector pswPVec */
            /* ----------------------------------------------------------------- */

            for (i = 0; i < S_LEN; i++)
            {
                pswPVec[i] = pswLtpStateOut[i];
            }
        }

        /* for unvoiced, do not reconstruct a pitch vector */
        /* ----------------------------------------------- */

        else
        {
            swLag = 0;                       /* indicates invalid lag
                                        * and unvoiced */
        }

        /* now work on the VSELP codebook excitation output */
        /* x_vec, x_a_vec here named ppswVselpEx[0] and [1] */
        /* ------------------------------------------------ */

        if (swVoicingMode)
        {
            /* voiced */

            siNumBits = C_BITS_V;
            siVselpCw = psiVselpCw[0];

            b_con(siVselpCw, siNumBits, pswBitArray);

            v_con(pppsrVcdCodeVec[0][0], ppswVselpEx[0], pswBitArray, siNumBits);
        }

        else
        {
            /* unvoiced */

            siNumBits = C_BITS_UV;

            for (siCodeBook = 0; siCodeBook < 2; siCodeBook++)
            {

                siVselpCw = psiVselpCw[siCodeBook];

                b_con(siVselpCw, siNumBits, (INT16 *) pswBitArray);

                v_con(pppsrUvCodeVec[siCodeBook][0], ppswVselpEx[siCodeBook],
                      pswBitArray, siNumBits);
            }
        }

        /* all excitation vectors have been created: ppswVselpEx and pswPVec  */
        /* if voiced compute rs00 and rs11; if unvoiced cmpute rs11 and rs22  */
        /* ------------------------------------------------------------------ */

        if (swLag)
        {
            rs_rr(pswPVec, psnsSqrtRs[giSfrmCnt], &snsRs00);
        }

        rs_rrNs(ppswVselpEx[0], psnsSqrtRs[giSfrmCnt], &snsRs11);

        if (!swVoicingMode)
        {
            rs_rrNs(ppswVselpEx[1], psnsSqrtRs[giSfrmCnt], &snsRs22);
        }

        /* now implement synthesis - combine the excitations */
        /* ------------------------------------------------- */

        if (swVoicingMode)
        {
            /* voiced */

            /* scale pitch and codebook excitations and get beta */
            /* ------------------------------------------------- */
            swSemiBeta = scaleExcite(pswPVec,
                                     pppsrGsp0[swVoicingMode][siGsp0Code][0],
                                     snsRs00, pswPVec);
            scaleExcite(ppswVselpEx[0],
                        pppsrGsp0[swVoicingMode][siGsp0Code][1],
                        snsRs11, ppswVselpEx[0]);

            /* combine the two scaled excitations */
            /* ---------------------------------- */
            for (i = 0; i < S_LEN; i++)
            {
                //pswExcite[i] = ADD_SAT16(pswPVec[i], ppswVselpEx[0][i]);
                pswExcite[i] = ADD(pswPVec[i], ppswVselpEx[0][i]);

            }
        }
        else
        {
            /* unvoiced */

            /* scale codebook excitations and set beta to 0 as not applicable */
            /* -------------------------------------------------------------- */
            swSemiBeta = 0;
            scaleExcite(ppswVselpEx[0],
                        pppsrGsp0[swVoicingMode][siGsp0Code][0],
                        snsRs11, ppswVselpEx[0]);
            scaleExcite(ppswVselpEx[1],
                        pppsrGsp0[swVoicingMode][siGsp0Code][1],
                        snsRs22, ppswVselpEx[1]);

            /* combine the two scaled excitations */
            /* ---------------------------------- */
            for (i = 0; i < S_LEN; i++)
            {
                //pswExcite[i] = ADD_SAT16(ppswVselpEx[1][i], ppswVselpEx[0][i]);
                pswExcite[i] = ADD(ppswVselpEx[1][i], ppswVselpEx[0][i]);

            }
        }

        /* now update the pitch state using the combined/scaled excitation */
        /* --------------------------------------------------------------- */

        for (i = 0; i < LTP_LEN; i++)
        {
            pswLtpStateBaseDec[i] = pswLtpStateBaseDec[i + S_LEN];
        }

        /* add the current sub-frames data to the state */
        /* -------------------------------------------- */

        for (i = -S_LEN, j = 0; j < S_LEN; i++, j++)
        {
            pswLtpStateOut[i] = pswExcite[j];/* add new frame at t = -S_LEN */
        }

        /* given the excitation perform pitch prefiltering */
        /* ----------------------------------------------- */

        pitchPreFilt(pswExcite, siGsp0Code, swLag,
                     swVoicingMode, swSemiBeta, psnsSqrtRs[giSfrmCnt],
                     pswPPFExcit, pswPPreState);


        /* Concealment on subframe signal level: */
        /* ------------------------------------- */
        level_estimator(0, &swLevelMean, &swLevelMax,
                        &pswDecodedSpeechFrame[giSfrmCnt * S_LEN]);

        signal_conceal_sub(pswPPFExcit, ppswSynthAs[giSfrmCnt], pswSynthFiltState,
                           &pswLtpStateOut[-S_LEN], &pswPPreState[LTP_LEN - S_LEN],
                           swLevelMean, swLevelMax,
                           pswParameters[19], swMuteFlagOld,
                           &swMuteFlag, swMutePermit);


        /* synthesize the speech through the synthesis filter */
        /* -------------------------------------------------- */

        lpcIir(pswPPFExcit, ppswSynthAs[giSfrmCnt], pswSynthFiltState,
               pswSynthFiltOut);

        /* pass reconstructed speech through adaptive spectral postfilter */
        /* -------------------------------------------------------------- */

        spectralPostFilter(pswSynthFiltOut, ppswPFNumAs[giSfrmCnt],
                           ppswPFDenomAs[giSfrmCnt],
                           &pswDecodedSpeechFrame[giSfrmCnt * S_LEN]);

        level_estimator(1, &swLevelMean, &swLevelMax,
                        &pswDecodedSpeechFrame[giSfrmCnt * S_LEN]);

    }

    /*for (i = 0; i < 18; i++)
         DecIn.decInBuf[i]=pswParameters[i];

    DecIn.bfi = pswParameters[18];
    DecIn.ufi = pswParameters[19];
    DecIn.sid = pswParameters[20];
    DecIn.taf = pswParameters[21];*/

    for (i = 0; i < 160; i++)
    {
        DecOut[i] = pswDecodedSpeechFrame[i];//(UINT16)()
    }/**/



    /* Save muting information for next frame */
    /* -------------------------------------- */
    swMuteFlagOld = swMuteFlag;

    /* end of frame processing - save this frame's frame energy,  */
    /* reflection coefs, direct form coefs, and post filter coefs */
    /* ---------------------------------------------------------- */

    swOldR0Dec = swR0Dec;
    swOldR0IndexDec = swR0Index;         /* DTX mode */

    for (i = 0; i < NP; i++)
    {
        pswOldFrmKsDec[i] = pswFrmKs[i];
        pswOldFrmAsDec[i] = pswFrmAs[i];
        pswOldFrmPFNum[i] = pswFrmPFNum[i];
        pswOldFrmPFDenom[i] = pswFrmPFDenom[i];
    }

    VPP_HR_PROFILE_FUNCTION_EXIT( vpp_hr_decode);

}


/***************************************************************************
 *
 *   FUNCTION NAME: sqroot
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform a single precision square
 *     root function on a INT32
 *
 *   INPUTS:
 *
 *     L_SqrtIn
 *                     input to square root function
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swSqrtOut
 *                     output to square root function
 *
 *   DESCRIPTION:
 *
 *      Input assumed to be normalized
 *
 *      The algorithm is based around a six term Taylor expansion :
 *
 *        y^0.5 = (1+x)^0.5
 *             ~= 1 + (x/2) - 0.5*((x/2)^2) + 0.5*((x/2)^3)
 *                - 0.625*((x/2)^4) + 0.875*((x/2)^5)
 *
 *      Max error less than 0.08 % for normalized input ( 0.5 <= x < 1 )
 *
 *   REFERENCES: Sub-clause 4.1.4.1, 4.1.7, 4.1.11.1, 4.2.1,
 *               4.2.2, 4.2.3 and 4.2.4 of GSM Recomendation 06.20
 *
 *   KEYWORDS: sqrt, squareroot, sqrt016
 *
 *************************************************************************/

INT16 sqroot(INT32 L_SqrtIn)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Local Constants                            |
     |_________________________________________________________________________|
    */

#define    PLUS_HALF          0x40000000L       /* 0.5 */
#define    MINUS_ONE          0x80000000L       /* -1 */
#define    TERM5_MULTIPLER    0x5000   /* 0.625 */
#define    TERM6_MULTIPLER    0x7000   /* 0.875 */

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Temp0,
          L_Temp1;

    INT16 swTemp,
          swTemp2,
          swTemp3,
          swTemp4,
          swSqrtOut;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* determine 2nd term x/2 = (y-1)/2 */
    /* -------------------------------- */

    //L_Temp1 = L_shr(L_SqrtIn, 1);        /* L_Temp1 = y/2 */
    L_Temp1 = SHR(L_SqrtIn, 1);        /* L_Temp1 = y/2 */
    //L_Temp1 = L_SUB(L_Temp1, PLUS_HALF); /* L_Temp1 = (y-1)/2 */
    L_Temp1 -=  PLUS_HALF ; /* L_Temp1 = (y-1)/2 */
    swTemp = EXTRACT_H(L_Temp1);         /* swTemp = x/2 */

    /* add contribution of 2nd term */
    /* ---------------------------- */

    //L_Temp1 = L_SUB(L_Temp1, MINUS_ONE); /* L_Temp1 = 1 + x/2 */
    L_Temp1 -=  MINUS_ONE ; /* L_Temp1 = 1 + x/2 */

    /* determine 3rd term */
    /* ------------------ */

    //L_Temp0 = L_msu(0L, swTemp, swTemp); /* L_Temp0 = -(x/2)^2 */
    s_lo=0L;
    VPP_MLA16(s_hi,s_lo, -swTemp, swTemp);
    L_Temp0= L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);



    swTemp2 = EXTRACT_H(L_Temp0);        /* swTemp2 = -(x/2)^2 */
    //L_Temp0 = L_shr(L_Temp0, 1);         /* L_Temp0 = -0.5*(x/2)^2 */
    L_Temp0 = SHR(L_Temp0, 1);         /* L_Temp0 = -0.5*(x/2)^2 */

    /* add contribution of 3rd term */
    /* ---------------------------- */

    //L_Temp0 = L_ADD(L_Temp1, L_Temp0);   /* L_Temp0 = 1 + x/2 - 0.5*(x/2)^2 */
    L_Temp0 =  ADD(L_Temp1, L_Temp0);   /* L_Temp0 = 1 + x/2 - 0.5*(x/2)^2 */

    /* determine 4rd term */
    /* ------------------ */

    //L_Temp1 = L_msu(0L, swTemp, swTemp2);/* L_Temp1 = (x/2)^3 */
    s_lo=0L;
    VPP_MLA16(s_hi,s_lo, -swTemp, swTemp2);
    L_Temp1= L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);

    swTemp3 = EXTRACT_H(L_Temp1);        /* swTemp3 = (x/2)^3 */
    //L_Temp1 = L_shr(L_Temp1, 1);         /* L_Temp1 = 0.5*(x/2)^3 */
    L_Temp1 = SHR(L_Temp1, 1);         /* L_Temp1 = 0.5*(x/2)^3 */

    /* add contribution of 4rd term */
    /* ---------------------------- */

    /* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */

    //L_Temp1 = L_ADD(L_Temp0, L_Temp1);
    L_Temp1 = ADD(L_Temp0, L_Temp1);

    /* determine partial 5th term */
    /* -------------------------- */

    L_Temp0 = L_MULT(swTemp, swTemp3);   /* L_Temp0 = (x/2)^4 */
    swTemp4 = ROUNDO(L_Temp0);            /* swTemp4 = (x/2)^4 */

    /* determine partial 6th term */
    /* -------------------------- */

    //L_Temp0 = L_msu(0L, swTemp2, swTemp3);        /* L_Temp0 = (x/2)^5 */
    s_lo=0L;
    VPP_MLA16(s_hi,s_lo, -swTemp2, swTemp3);
    L_Temp0= L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_SAT(s_hi,s_lo);

    swTemp2 = ROUNDO(L_Temp0);            /* swTemp2 = (x/2)^5 */

    /* determine 5th term and add its contribution */
    /* ------------------------------------------- */

    /* L_Temp0 = -0.625*(x/2)^4 */

    //L_Temp0 = L_msu(0L, TERM5_MULTIPLER, swTemp4);
    s_lo=0L;
    VPP_MLA16(s_hi,s_lo, TERM5_MULTIPLER, - swTemp4);
    L_Temp0= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);

    /* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 - 0.625*(x/2)^4 */

    //L_Temp1 = L_ADD(L_Temp0, L_Temp1);
    L_Temp1 =  ADD(L_Temp0, L_Temp1);

    /* determine 6th term and add its contribution */
    /* ------------------------------------------- */

    /* swSqrtOut = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */
    /* - 0.625*(x/2)^4 + 0.875*(x/2)^5     */

    swSqrtOut = MAC_R(L_Temp1, TERM6_MULTIPLER, swTemp2);

    /* return output */
    /* ------------- */

    return (swSqrtOut);
}

/***************************************************************************
 *
 *   FUNCTION NAME: v_con
 *
 *   PURPOSE:
 *
 *     This subroutine constructs a codebook excitation
 *     vector from basis vectors
 *
 *   INPUTS:
 *
 *     pswBVects[0:siNumBVctrs*S_LEN-1]
 *
 *                     Array containing a set of basis vectors.
 *
 *     pswBitArray[0:siNumBVctrs-1]
 *
 *                     Bit array dictating the polarity of the
 *                     basis vectors in the output vector.
 *                     Each element of the bit array is either -0.5 or +0.5
 *
 *     siNumBVctrs
 *                     Number of bits in codeword
 *
 *   OUTPUTS:
 *
 *     pswOutVect[0:39]
 *
 *                     Array containing the contructed output vector
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *     The array pswBitArray is used to multiply each of the siNumBVctrs
 *     basis vectors.  The input pswBitArray[] is an array whose
 *     elements are +/-0.5.  These multiply the VSELP basis vectors and
 *     when summed produce a VSELP codevector.  b_con() is the function
 *     used to translate a VSELP codeword into pswBitArray[].
 *
 *
 *   REFERENCES: Sub-clause 4.1.10 and 4.2.1 of GSM Recomendation 06.20
 *
 *   KEYWORDS: v_con, codeword, reconstruct, basis vector, excitation
 *
 *************************************************************************/

void   v_con(INT16 pswBVects[], INT16 pswOutVect[],
             INT16 pswBitArray[], short int siNumBVctrs)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_Temp;//,Lmult

    short int siSampleCnt,
          siCVectCnt;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Sample loop  */
    /*--------------*/
    for (siSampleCnt = 0; siSampleCnt < S_LEN; siSampleCnt++)
    {

        /* First element of output vector */
        //L_Temp = L_MULT(pswBitArray[0], pswBVects[0 * S_LEN + siSampleCnt]);

        //s_lo=SHR(L_Temp,1);
        VPP_MLX16(s_hi, s_lo, pswBitArray[0], pswBVects[0 * S_LEN + siSampleCnt]);
        /* Construct output vector */
        /*-------------------------*/
        for (siCVectCnt = 1; siCVectCnt < siNumBVctrs; siCVectCnt++)
        {
            /*L_Temp = L_mac(L_Temp, pswBitArray[siCVectCnt],pswBVects[siCVectCnt * S_LEN + siSampleCnt]);*/
            /*Lmult=L_MULT(pswBitArray[siCVectCnt], pswBVects[siCVectCnt * S_LEN + siSampleCnt]);
            L_Temp=L_ADD(L_Temp,Lmult);*/
            VPP_MLA16(s_hi,s_lo, pswBitArray[siCVectCnt],pswBVects[siCVectCnt * S_LEN + siSampleCnt]);
        }
        L_Temp= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi,s_lo);

        /* store the output vector sample */
        /*--------------------------------*/
        //L_Temp = L_shl(L_Temp, 1);
        //L_Temp = L_SHL_SAT(L_Temp, 1);
        L_Temp <<=1 ;
        pswOutVect[siSampleCnt] = EXTRACT_H(L_Temp);
    }
}

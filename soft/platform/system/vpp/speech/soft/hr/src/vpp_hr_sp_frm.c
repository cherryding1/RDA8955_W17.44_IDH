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







#include "vpp_hr_mathhalf.h"
#include "vpp_hr_mathdp31.h"
#include "vpp_hr_sp_rom.h"
#include "vpp_hr_sp_dec.h"
#include "vpp_hr_sp_frm.h"
#include "vpp_hr_sp_sfrm.h"
#include "vpp_hr_vad.h"
#include "vpp_hr_dtx.h"
#include "vpp_hr_mathhalf_macro.h"


//For Profiling
#include "vpp_hr_debug.h"
#include "vpp_hr_profile_codes.h"



/*_________________________________________________________________________
 |                                                                         |
 |                            Local Constants                              |
 |_________________________________________________________________________|
*/

#define ASCALE  0x0800
#define ASHIFT 4
#define CG_INT_MACS     6
#define CG_TERMS        (LSMAX - LSMIN + 1)
#define CVSHIFT 2                      /* Number of right shifts to be
                                        * applied to the normalized Phi
                                        * array in cov32, also used in flat
                                        * to shift down normalized F, B, C
                                        * matrices.                        */
#define C_FRAME_LEN     (N_SUB * CG_TERMS)
#define DELTA_LEVELS    16
#define G_FRAME_LEN     (LSMAX + (N_SUB-1) * S_LEN - LSMIN  + 1)
#define HIGH 1
#define INV_OS_FCTR     0x1555         /* 1.0/6.0 */
#define LAG_TABLE_LEN   (1 << L_BITS)
#define LMAX            142
#define LMAX_FR         (LMAX * OS_FCTR)
#define LMIN            21
#define LMIN_FR         (LMIN * OS_FCTR)
#define LOW 0
#define LPC_VQ_SEG 3
#define LSMAX           (LMAX + CG_INT_MACS/2)
#define LSMIN           (LMIN - CG_INT_MACS/2)
#define LSP_MASK  0xffff
#define L_BITS          8
#define L_ROUND (INT32)0x8000       /* Preload accumulator value for
                                        * rounding  */
#define NP_AFLAT     4
#define NUM_CLOSED      3
#define NUM_TRAJ_MAX    2
#define ONE_EIGHTH      0x1000
#define ONE_HALF        0x4000
#define ONE_QUARTER     0x2000
#define PEAK_VICINITY   3
#define PGAIN_CLAMP    0x0021          /* 0.001 */
#define PGAIN_SCALE    0x6000          /* 0.75 */
#define PW_FRAC         0x3333         /* 0.4 */
#define R0BITS 5
#define RSHIFT  2
#define S_SH    6                      /* Shift offset for computing frame
                                        * energy */
#define UV_SCALE0       -0x2976
#define UV_SCALE1       -0x46d3
#define UV_SCALE2       -0x6676
#define W_F_BUFF_LEN  (F_LEN + LSMAX)
#define high(x) (SHR(x,8) & 0x00ff)
#define low(x) x & 0x00ff              /* This macro will return the low
                                        * byte of a word */
#define odd(x) (x & 0x0001)            /* This macro will determine if an
                                        * integer is odd */

/*_________________________________________________________________________
 |                                                                         |
 |                         State Variables (globals)                       |
 |_________________________________________________________________________|
*/

INT16 pswAnalysisState[NP];

INT16 pswWStateNum[NP],
      pswWStateDenom[NP];

/*_________________________________________________________________________
 |                                                                         |
 |                         Other External Variables                        |
 |_________________________________________________________________________|
*/

static INT16 *psrTable;         /* points to correct table of
                                        * vectors */
int iLimit;                            /* accessible to all in this file
                                        * and to lpcCorrQntz() in dtx.c */
static int iLow;                       /* the low element in this segment */
static int iThree;                     /* boolean, is this a three element
                                        * vector */
static int iWordHalfPtr;               /* points to the next byte */
static int iWordPtr;                   /* points to the next word to be
                                        * read */

extern INT16 pswCNVSCode1[],       /* comfort noise parameters */
       pswCNVSCode2[],
       pswCNGsp0Code[],
       pswCNLpc[],
       swCNR0;


//INT32 essai0=0,essai=0;
/***************************************************************************
 *
 *    FUNCTION NAME: aflat
 *
 *    PURPOSE:  Given a vector of high-pass filtered input speech samples
 *              (A_LEN samples), function aflat computes the NP unquantized
 *              reflection coefficients using the FLAT algorithm, searches
 *              the three segment Rc-VQ based on the AFLAT recursion, and
 *              outputs a quantized set of NP reflection coefficients, along
 *              with the three indices specifying the selected vectors
 *              from the Rc-VQ. The index of the quantized frame energy R0
 *              is also output.
 *
 *
 *    INPUT:
 *
 *        pswSpeechToLpc[0:A_LEN-1]
 *                     A vector of high-pass filtered input speech, from
 *                     which the unquantized reflection coefficients and
 *                     the index of the quantized frame energy are
 *                     computed.
 *
 *    OUTPUTS:
 *
 *        piR0Index[0:0]
 *                     An index into a 5 bit table of quantized frame
 *                     energies.
 *
 *        pswFinalRc[0:NP-1]
 *                     A quantized set of NP reflection coefficients.
 *
 *        piVQCodewds[0:2]
 *                     An array containing the indices of the 3 reflection
 *                     coefficient vectors selected from the three segment
 *                     Rc-VQ.
 *
 *        swPtch
 *                     Flag to indicate a periodic signal component
 *
 *        pswVadFlag
 *                     Voice activity decision flag
 *                      = 1: voice activity
 *                      = 0: no voice activity
 *
 *        pswSP
 *                     Speech flag
 *                      = 1: encoder generates speech frames
 *                      = 0: encoder generate SID frames
 *
 *
 *    RETURN:
 *        None.
 *
 *    REFERENCE:  Sub-clauses 4.1.3, 4.1.4, and 4.1.4.1
 *        of GSM Recommendation 06.20
 *
 *    KEYWORDS: AFLAT,aflat,flat,vectorquantization, reflectioncoefficients
 *
 *************************************************************************/

void   aflat(INT16 pswSpeechToLPC[],
             int piR0Index[],
             INT16 pswFinalRc[],
             int piVQCodewds[],
             INT16 swPtch,
             INT16 *pswVadFlag,
             INT16 *pswSP)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswPOldSpace[NP_AFLAT],
          pswPNewSpace[NP_AFLAT],
          pswVOldSpace[2 * NP_AFLAT - 1],
          pswVNewSpace[2 * NP_AFLAT - 1],
          *ppswPAddrs[2],
          *ppswVAddrs[2],
          *pswVBar,
          pswPBar[NP_AFLAT],
          pswVBarSpace[2 * NP_AFLAT - 1],
          pswFlatsRc[NP],               /* Unquantized Rc's computed by FLAT */
          pswRc[NP + 1];                /* Temp list for the converted RC's */
    INT32 pL_CorrelSeq[NP + 1],
          *pL_VBarFull,
          pL_PBarFull[NP],
          pL_VBarFullSpace[2 * NP - 1];

    int    i,
           iVec,
           iSeg,
           iCnt;                         /* Loop counter */
    struct QuantList quantList,          /* A list of vectors */
               bestPql[4];                   /* The four best vectors from the
                                        * PreQ */
    struct QuantList bestQl[LPC_VQ_SEG + 1];      /* Best vectors for each of
                                                 * the three segments */
    INT16 swVadScalAuto;
    INT16 pswVadRc[4];
    INT32 pL_VadAcf[9];

    INT32 L_R0;                       /* Normalized R0 (use swRShifts to
                                        * unnormalize). This is done prior
                                        * to r0quant(). After this, its is
                                        * a unnormalized number */


    VPP_HR_PROFILE_FUNCTION_ENTER(aflat);


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Setup pointers temporary space */
    /*--------------------------------*/

    pswVBar = pswVBarSpace + NP_AFLAT - 1;
    pL_VBarFull = pL_VBarFullSpace + NP - 1;
    ppswPAddrs[0] = pswPOldSpace;
    ppswPAddrs[1] = pswPNewSpace;
    ppswVAddrs[0] = pswVOldSpace + NP_AFLAT - 1;
    ppswVAddrs[1] = pswVNewSpace + NP_AFLAT - 1;

    /* Given the input speech, compute the optimal reflection coefficients */
    /* using the FLAT algorithm.                                           */
    /*---------------------------------------------------------------------*/

    L_R0 = flat(pswSpeechToLPC, pswFlatsRc, piR0Index, pL_VadAcf,
                &swVadScalAuto);

    /* Get unquantized reflection coefficients for VAD */      /* DTX mode */
    /* algorithm                                       */      /* DTX mode */
    /* ----------------------------------------------- */      /* DTX mode */

    //for (i = 0; i < 4; i++)                                    /* DTX mode */
    //pswVadRc[i] = pswFlatsRc[i];                             /* DTX mode */
    pswVadRc[0] = pswFlatsRc[0];                             /* DTX mode */
    pswVadRc[1] = pswFlatsRc[1];                             /* DTX mode */
    pswVadRc[2] = pswFlatsRc[2];                             /* DTX mode */
    pswVadRc[3] = pswFlatsRc[3];                             /* DTX mode */


    /* convert reflection coefficients to correlation */       /* DTX mode */
    /* sequence                                       */       /* DTX mode */
    /* ---------------------------------------------- */       /* DTX mode */

    rcToCorrDpL(ASHIFT, ASCALE, pswFlatsRc, pL_CorrelSeq);     /* DTX mode */


    /* Make the voice activity detection. Only swVadFlag is */ /* DTX mode */
    /*  modified.                                           */ /* DTX mode */
    /* ---------------------------------------------------- */ /* DTX mode */

    vad_algorithm(pL_VadAcf, swVadScalAuto, pswVadRc, swPtch,  /* DTX mode */
                  pswVadFlag);


    /* if DTX mode off, then always voice activity */          /* DTX mode */
    /* ------------------------------------------- */          /* DTX mode */
    if (!giDTXon) *pswVadFlag = 1;                             /* DTX mode */


    /* determination of comfort noise parameters */            /* DTX mode */
    /* ----------------------------------------- */            /* DTX mode */

    *pswSP = swComfortNoise(*pswVadFlag,                       /* DTX mode */
                            L_R0,                              /* DTX mode */
                            pL_CorrelSeq);                     /* DTX mode */

    if (*pswSP == 0)                                           /* DTX mode */
    {
        /* SID frame generation */                             /* DTX mode */

        /* use unquantized reflection coefficients in the */     /* DTX mode */
        /* encoder, when SID frames are generated         */     /* DTX mode */
        /* ---------------------------------------------- */     /* DTX mode */

        for (i = 0; i < NP; i++)                                 /* DTX mode */
            pswFinalRc[i] = pswFlatsRc[i];                         /* DTX mode */

    }                                                          /* DTX mode */
    else                                                       /* DTX mode */
    {
        /* speech frame generation */

        /* Set up pL_PBarFull and pL_VBarFull initial conditions, using the   */
        /* autocorrelation sequence derived from the optimal reflection       */
        /* coefficients computed by FLAT. The initial conditions are shifted  */
        /* right by RSHIFT bits. These initial conditions, stored as          */
        /* INT32s, are used to initialize PBar and VBar arrays for the     */
        /* next VQ segment.                                                   */
        /*--------------------------------------------------------------------*/

        initPBarFullVBarFullL(pL_CorrelSeq, pL_PBarFull, pL_VBarFull);

        /* Set up initial PBar and VBar initial conditions, using pL_PBarFull */
        /* and pL_VBarFull arrays initialized above. These are the initial    */
        /* PBar and VBar conditions to be used by the AFLAT recursion at the  */
        /* 1-st Rc-VQ segment.                                                */
        /*--------------------------------------------------------------------*/

        initPBarVBarL(pL_PBarFull, pswPBar, pswVBar);

        for (iSeg = 1; iSeg <= LPC_VQ_SEG; iSeg++)
        {

            /* initialize candidate list */
            /*---------------------------*/

            quantList.iNum = psrPreQSz[iSeg - 1];
            quantList.iRCIndex = 0;

            /* do aflat for all vectors in the list */
            /*--------------------------------------*/

            setupPreQ(iSeg, quantList.iRCIndex);        /* set up vector ptrs */

            for (iCnt = 0; iCnt < quantList.iNum; iCnt++)
            {
                /* get a vector */
                /*--------------*/

                getNextVec(pswRc);

                /* clear the limiter flag */
                /*------------------------*/

                iLimit = 0;

                /* find the error values for each vector */
                /*---------------------------------------*/

                quantList.pswPredErr[iCnt] =
                    aflatRecursion(&pswRc[psvqIndex[iSeg - 1].l],
                                   pswPBar, pswVBar,
                                   ppswPAddrs, ppswVAddrs,
                                   psvqIndex[iSeg - 1].len);

                /* check the limiter flag */
                /*------------------------*/

                if (iLimit)
                {
                    quantList.pswPredErr[iCnt] = 0x7fff;    /* set error to bad value */
                }

            }                                  /* done list loop */

            /* find 4 best prequantizer levels */
            /*---------------------------------*/

            findBestInQuantList(quantList, 4, bestPql);

            for (iVec = 0; iVec < 4; iVec++)
            {

                /* initialize quantizer list */
                /*---------------------------*/

                quantList.iNum = psrQuantSz[iSeg - 1];
                quantList.iRCIndex = bestPql[iVec].iRCIndex * psrQuantSz[iSeg - 1];

                setupQuant(iSeg, quantList.iRCIndex);     /* set up vector ptrs */

                /* do aflat recursion on each element of list */
                /*--------------------------------------------*/

                for (iCnt = 0; iCnt < quantList.iNum; iCnt++)
                {

                    /* get a vector */
                    /*--------------*/

                    getNextVec(pswRc);

                    /* clear the limiter flag */
                    /*------------------------*/

                    iLimit = 0;

                    /* find the error values for each vector */
                    /*---------------------------------------*/

                    quantList.pswPredErr[iCnt] =
                        aflatRecursion(&pswRc[psvqIndex[iSeg - 1].l],
                                       pswPBar, pswVBar,
                                       ppswPAddrs, ppswVAddrs,
                                       psvqIndex[iSeg - 1].len);

                    /* check the limiter flag */
                    /*------------------------*/

                    if (iLimit)
                    {
                        quantList.pswPredErr[iCnt] = 0x7fff;  /* set error to the worst
                                                   * value */
                    }

                }                                /* done list loop */

                /* find best quantizer vector for this segment, and save it */
                /*----------------------------------------------------------*/

                findBestInQuantList(quantList, 1, bestQl);
                if (iVec == 0)
                {
                    bestQl[iSeg] = bestQl[0];
                }
                else
                {
                    if ( bestQl[iSeg].pswPredErr[0] > bestQl[0].pswPredErr[0] )
                    {
                        bestQl[iSeg] = bestQl[0];
                    }
                }
            }

            /* find the quantized reflection coefficients */
            /*--------------------------------------------*/

            setupQuant(iSeg, bestQl[iSeg].iRCIndex);    /* set up vector ptrs */
            getNextVec((INT16 *) (pswFinalRc - 1));


            /* Update pBarFull and vBarFull for the next Rc-VQ segment, and */
            /* update the pswPBar and pswVBar for the next Rc-VQ segment    */
            /*--------------------------------------------------------------*/

            if (iSeg < LPC_VQ_SEG)
            {

                aflatNewBarRecursionL(&pswFinalRc[psvqIndex[iSeg - 1].l - 1], iSeg,
                                      pL_PBarFull, pL_VBarFull, pswPBar, pswVBar);

            }

        }

        /* find the quantizer index (the values */
        /* to be output in the symbol file)     */
        /*--------------------------------------*/

        for (iSeg = 1; iSeg <= LPC_VQ_SEG; iSeg++)
        {
            piVQCodewds[iSeg - 1] = bestQl[iSeg].iRCIndex;
        }

    }

    VPP_HR_PROFILE_FUNCTION_EXIT(aflat );


}

/***************************************************************************
 *
 *    FUNCTION NAME: aflatNewBarRecursionL
 *
 *    PURPOSE:  Given the INT32 initial condition arrays, pL_PBarFull and
 *              pL_VBarFull, a reflection coefficient vector selected from
 *              the Rc-VQ at the current stage, and index of the current
 *              Rc-VQ stage, the AFLAT recursion is evaluated to obtain the
 *              updated initial conditions for the AFLAT recursion at the
 *              next Rc-VQ stage. At each lattice stage the pL_PBarFull and
 *              pL_VBarFull arrays are shifted to be RSHIFT down from full
 *              scale. Two sets of initial conditions are output:
 *
 *              1) pswPBar and pswVBar INT16 arrays are used at the
 *                 next Rc-VQ segment as the AFLAT initial conditions
 *                 for the Rc prequantizer and the Rc quantizer searches.
 *              2) pL_PBarFull and pL_VBarFull arrays are output and serve
 *                 as the initial conditions for the function call to
 *                 aflatNewBarRecursionL at the next lattice stage.
 *
 *
 *              This is an implementation of equations 4.24 through
 *              4.27.
 *    INPUTS:
 *
 *        pswQntRc[0:NP_AFLAT-1]
 *                     An input reflection coefficient vector selected from
 *                     the Rc-VQ quantizer at the current stage.
 *
 *        iSegment
 *                    An input describing the current Vector quantizer
 *                    quantizer segment (1, 2, or 3).
 *
 *        RSHIFT      The number of shifts down from full scale the
 *                     pL_PBarFull and pL_VBarFull arrays are to be shifted
 *                     at each lattice stage. RSHIFT is a global constant.
 *
 *        pL_PBar[0:NP-1]
 *                     A INT32 input array containing the P initial
 *                     conditions for the full 10-th order LPC filter.
 *                     The address of the 0-th element of  pL_PBarFull
 *                     is passed in when function aflatNewBarRecursionL
 *                     is called.
 *
 *        pL_VBar[-NP+1:NP-1]
 *                     A INT32 input array containing the V initial
 *                     conditions for the full 10-th order LPC filter.
 *                     The address of the 0-th element of  pL_VBarFull
 *                     is passed in when function aflatNewBarRecursionL
 *                     is called.
 *
 *    OUTPUTS:
 *
 *        pL_PBar[0:NP-1]
 *                     A INT32 output array containing the updated P
 *                     initial conditions for the full 10-th order LPC
 *                     filter.
 *
 *        pL_VBar[-NP+1:NP-1]
 *                     A INT32 output array containing the updated V
 *                     initial conditions for the full 10-th order LPC
 *                     filter.
 *
 *        pswPBar[0:NP_AFLAT-1]
 *                     An output INT16 array containing the P initial
 *                     conditions for the P-V AFLAT recursion for the next
 *                     Rc-VQ segment. The address of the 0-th element of
 *                     pswVBar is passed in.
 *
 *        pswVBar[-NP_AFLAT+1:NP_AFLAT-1]
 *                     The output INT16 array containing the V initial
 *                     conditions for the P-V AFLAT recursion, for the next
 *                     Rc-VQ segment. The address of the 0-th element of
 *                     pswVBar is passed in.
 *
 *    RETURN:
 *        None.
 *
 *    REFERENCE:  Sub-clause 4.1.4.1 GSM Recommendation 06.20
 *
 *************************************************************************/

void   aflatNewBarRecursionL(INT16 pswQntRc[], int iSegment,
                             INT32 pL_PBar[], INT32 pL_VBar[],
                             INT16 pswPBar[], INT16 pswVBar[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT32 *pL_VOld,
          *pL_VNew,
          *pL_POld,
          *pL_PNew,
          *ppL_PAddrs[2],
          *ppL_VAddrs[2],
          pL_VOldSpace[2 * NP - 1],
          pL_VNewSpace[2 * NP - 1],
          pL_POldSpace[NP],
          pL_PNewSpace[NP],
          L_temp,
          L_sum;
    INT16 swQntRcSq,
          swNShift;
    short int i,
          j,
          bound;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    VPP_HR_PROFILE_FUNCTION_ENTER( aflatNewBarRecursionL);

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */
    /* Copy the addresses of the input PBar and VBar arrays into  */
    /* pL_POld and pL_VOld respectively.                          */
    /*------------------------------------------------------------*/

    pL_POld = pL_PBar;
    pL_VOld = pL_VBar;

    /* Point to PNew and VNew temporary arrays */
    /*-----------------------------------------*/

    pL_PNew = pL_PNewSpace;
    pL_VNew = pL_VNewSpace + NP - 1;

    /* Load the addresses of the temporary buffers into the address arrays. */
    /* The address arrays are used to swap PNew and POld (VNew and VOLd)    */
    /* buffers to avoid copying of the buffer contents at the end of a      */
    /* lattice filter stage.                                                */
    /*----------------------------------------------------------------------*/

    ppL_PAddrs[0] = pL_POldSpace;
    ppL_PAddrs[1] = pL_PNewSpace;
    ppL_VAddrs[0] = pL_VOldSpace + NP - 1;
    ppL_VAddrs[1] = pL_VNewSpace + NP - 1;


    /* Update AFLAT recursion initial conditions for searching the Rc vector */
    /* quantizer at the next VQ segment.                                     */
    /*-------------------------------------------------------------------*/

    for (j = 0; j < psvqIndex[iSegment - 1].len; j++)
    {
        bound = NP - psvqIndex[iSegment - 1].l - j - 1;

        /* Compute rc squared, used by the recursion at the j-th lattice stage. */
        /*---------------------------------------------------------------------*/

        swQntRcSq = MULT_R(pswQntRc[j], pswQntRc[j]);
        /*VPP_MULT_R(m_hi, m_lo, pswQntRc[j], pswQntRc[j]);
        swQntRcSq = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/

        /* Calculate PNew(i) */
        /*-------------------*/

        L_temp = L_mpy_ls(pL_VOld[0], pswQntRc[j]);
        //L_sum = L_ADD(L_temp, pL_POld[0]);
        L_sum =  ADD(L_temp, pL_POld[0]);
        L_temp = L_mpy_ls(pL_POld[0], swQntRcSq);
        //L_sum = L_ADD(L_temp, L_sum);
        L_sum =  ADD(L_temp, L_sum);
        L_temp = L_mpy_ls(pL_VOld[0], pswQntRc[j]);
        //L_temp = L_ADD(L_sum, L_temp);
        L_temp =  ADD(L_sum, L_temp);

        /* Compute the number of bits to shift left by to achieve  */
        /* the nominal value of PNew[0] which is right shifted by  */
        /* RSHIFT bits relative to full scale.                     */
        /*---------------------------------------------------------*/

        swNShift = SUB(norm_s(EXTRACT_H(L_temp)), RSHIFT);

        /* Rescale PNew[0] by shifting left by swNShift bits */
        /*---------------------------------------------------*/

        //pL_PNew[0] = L_shl(L_temp, swNShift);
        //pL_PNew[0] = L_SHL_SAT(L_temp, swNShift);
        pL_PNew[0] = SHL(L_temp, swNShift);

        for (i = 1; i <= bound; i++)
        {
            L_temp = L_mpy_ls(pL_VOld[i], pswQntRc[j]);
            //L_sum = L_ADD(L_temp, pL_POld[i]);
            L_sum =  ADD(L_temp, pL_POld[i]);
            L_temp = L_mpy_ls(pL_POld[i], swQntRcSq);
            //L_sum = L_ADD(L_temp, L_sum);
            L_sum =  ADD(L_temp, L_sum);
            L_temp = L_mpy_ls(pL_VOld[-i], pswQntRc[j]);
            //L_temp = L_ADD(L_sum, L_temp);
            L_temp =  ADD(L_sum, L_temp);
            //pL_PNew[i] = L_shl(L_temp, swNShift);
            //pL_PNew[i] = L_SHL_SAT(L_temp, swNShift);
            pL_PNew[i] =  SHL (L_temp, swNShift);

        }

        /* Calculate VNew(i) */
        /*-------------------*/

        for (i = -bound; i < 0; i++)
        {
            L_temp = L_mpy_ls(pL_VOld[-i - 1], swQntRcSq);
            //L_sum = L_ADD(L_temp, pL_VOld[i + 1]);
            L_sum =  ADD(L_temp, pL_VOld[i + 1]);
            L_temp = L_mpy_ls(pL_POld[-i - 1], pswQntRc[j]);
            //L_temp = L_shl(L_temp, 1);
            //L_temp = L_SHL_SAT(L_temp, 1);
            L_temp <<=  1 ;
            //L_temp = L_ADD(L_temp, L_sum);
            L_temp =  ADD(L_temp, L_sum);
            //pL_VNew[i] = L_shl(L_temp, swNShift);
            //pL_VNew[i] = L_SHL_SAT(L_temp, swNShift);
            pL_VNew[i] =  SHL (L_temp, swNShift);

        }
        for (i = 0; i <= bound; i++)
        {
            L_temp = L_mpy_ls(pL_VOld[-i - 1], swQntRcSq);
            //L_sum = L_ADD(L_temp, pL_VOld[i + 1]);
            L_sum =  ADD(L_temp, pL_VOld[i + 1]);
            L_temp = L_mpy_ls(pL_POld[i + 1], pswQntRc[j]);
            //L_temp = L_shl(L_temp, 1);
            //L_temp = L_SHL_SAT(L_temp, 1);
            L_temp <<=  1 ;
            //L_temp = L_ADD(L_temp, L_sum);
            L_temp =  ADD(L_temp, L_sum);
            //pL_VNew[i] = L_shl(L_temp, swNShift);
            //pL_VNew[i] = L_SHL_SAT(L_temp, swNShift);
            pL_VNew[i] =  SHL (L_temp, swNShift);

        }

        if (j < psvqIndex[iSegment - 1].len - 2)
        {

            /* Swap POld and PNew buffers, using modulo addressing */
            /*-----------------------------------------------------*/

            pL_POld = ppL_PAddrs[(j + 1) % 2];
            pL_PNew = ppL_PAddrs[j % 2];

            /* Swap VOld and VNew buffers, using modulo addressing */
            /*-----------------------------------------------------*/

            pL_VOld = ppL_VAddrs[(j + 1) % 2];
            pL_VNew = ppL_VAddrs[j % 2];
        }
        else
        {
            if (j == psvqIndex[iSegment - 1].len - 2)
            {

                /* Then recursion to be done for one more lattice stage */
                /*------------------------------------------------------*/

                /* Copy address of PNew into POld */
                /*--------------------------------*/
                pL_POld = ppL_PAddrs[(j + 1) % 2];

                /* Copy address of the input pL_PBar array into pswPNew; this will */
                /* cause the PNew array to overwrite the input pL_PBar array, thus */
                /* updating it at the final lattice stage of the current segment   */
                /*-----------------------------------------------------------------*/

                pL_PNew = pL_PBar;

                /* Copy address of VNew into VOld */
                /*--------------------------------*/

                pL_VOld = ppL_VAddrs[(j + 1) % 2];

                /* Copy address of the input pL_VBar array into pswVNew; this will */
                /* cause the VNew array to overwrite the input pL_VBar array, thus */
                /* updating it at the final lattice stage of the current segment   */
                /*-----------------------------------------------------------------*/

                pL_VNew = pL_VBar;

            }
        }
    }

    /* Update the pswPBar and pswVBar initial conditions for the AFLAT      */
    /* Rc-VQ search at the next segment.                                    */
    /*----------------------------------------------------------------------*/

    bound = psvqIndex[iSegment].len - 1;

    for (i = 0; i <= bound; i++)
    {
        pswPBar[i] = ROUNDO(pL_PBar[i]);
        pswVBar[i] = ROUNDO(pL_VBar[i]);
    }
    for (i = -bound; i < 0; i++)
    {
        pswVBar[i] = ROUNDO(pL_VBar[i]);
    }


    VPP_HR_PROFILE_FUNCTION_EXIT(aflatNewBarRecursionL );

    return;
}

/***************************************************************************
 *
 *    FUNCTION NAME: aflatRecursion
 *
 *    PURPOSE:  Given the INT16 initial condition arrays, pswPBar and
 *              pswVBar, a reflection coefficient vector from the quantizer
 *              (or a prequantizer), and the order of the current Rc-VQ
 *              segment, function aflatRecursion computes and returns the
 *              residual error energy by evaluating the AFLAT recursion.
 *
 *              This is an implementation of equations 4.18 to 4.23.
 *    INPUTS:
 *
 *        pswQntRc[0:NP_AFLAT-1]
 *                     An input reflection coefficient vector from the
 *                     Rc-prequantizer or the Rc-VQ codebook.
 *
 *        pswPBar[0:NP_AFLAT-1]
 *                     The input INT16 array containing the P initial
 *                     conditions for the P-V AFLAT recursion at the current
 *                     Rc-VQ segment. The address of the 0-th element of
 *                     pswVBar is passed in.
 *
 *        pswVBar[-NP_AFLAT+1:NP_AFLAT-1]
 *                     The input INT16 array containing the V initial
 *                     conditions for the P-V AFLAT recursion, at the current
 *                     Rc-VQ segment. The address of the 0-th element of
 *                     pswVBar is passed in.
 *
 *        *ppswPAddrs[0:1]
 *                     An input array containing the address of temporary
 *                     space P1 in its 0-th element, and the address of
 *                     temporary space P2 in its 1-st element. Each of
 *                     these addresses is alternately assigned onto
 *                     pswPNew and pswPOld pointers using modulo
 *                     arithmetic, so as to avoid copying the contents of
 *                     pswPNew array into the pswPOld array at the end of
 *                     each lattice stage of the AFLAT recursion.
 *                     Temporary space P1 and P2 is allocated outside
 *                     aflatRecursion by the calling function aflat.
 *
 *        *ppswVAddrs[0:1]
 *                     An input array containing the address of temporary
 *                     space V1 in its 0-th element, and the address of
 *                     temporary space V2 in its 1-st element. Each of
 *                     these addresses is alternately assigned onto
 *                     pswVNew and pswVOld pointers using modulo
 *                     arithmetic, so as to avoid copying the contents of
 *                     pswVNew array into the pswVOld array at the end of
 *                     each lattice stage of the AFLAT recursion.
 *                     Temporary space V1 and V2 is allocated outside
 *                     aflatRecursion by the calling function aflat.
 *
 *        swSegmentOrder
 *                     This input short word describes the number of
 *                     stages needed to compute the vector
 *                     quantization of the given segment.
 *
 *    OUTPUTS:
 *        None.
 *
 *    RETURN:
 *        swRe         The INT16 value of residual energy for the
 *                     Rc vector, given the pswPBar and pswVBar initial
 *                     conditions.
 *
 *    REFERENCE:  Sub-clause 4.1.4.1 GSM Recommendation 06.20
 *
 *************************************************************************/

INT16 aflatRecursion(INT16 pswQntRc[],
                     INT16 pswPBar[],
                     INT16 pswVBar[],
                     INT16 *ppswPAddrs[],
                     INT16 *ppswVAddrs[],
                     INT16 swSegmentOrder)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 *pswPOld,
          *pswPNew,
          *pswVOld,
          *pswVNew,
          pswQntRcSqd[NP_AFLAT],
          swRe;
    INT32  L_sum;//,L_sum1,L_sum2
    short int i,
          j,
          bound;                        /* loop control variables */

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    VPP_HR_PROFILE_FUNCTION_ENTER( aflatRecursion);

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Point to PBar and VBar, the initial condition arrays for the AFLAT  */
    /* recursion.                                                          */
    /*---------------------------------------------------------------------*/

    pswPOld = pswPBar;
    pswVOld = pswVBar;

    /* Point to PNew and VNew, the arrays into which updated values of  P  */
    /* and V functions will be written.                                    */
    /*---------------------------------------------------------------------*/

    pswPNew = ppswPAddrs[1];
    pswVNew = ppswVAddrs[1];

    /* Compute the residual error energy due to the selected Rc vector */
    /* using the AFLAT recursion.                                      */
    /*-----------------------------------------------------------------*/

    /* Compute rc squared, used by the recursion */
    /*-------------------------------------------*/

    for (j = 0; j < swSegmentOrder; j++)
    {
        pswQntRcSqd[j] = MULT_R(pswQntRc[j], pswQntRc[j]);
        /*VPP_MULT_R(m_hi, m_lo, pswQntRc[j], pswQntRc[j]);
        pswQntRcSqd[j] = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
    }





    /* Compute the residual error energy due to the selected Rc vector */
    /* using the AFLAT recursion.                                      */
    /*-----------------------------------------------------------------*/

    for (j = 0; j < swSegmentOrder - 1; j++)
    {
        bound = swSegmentOrder - j - 2;
        //s_lo=0;

        /* Compute Psubj(i), for i = 0, bound  */
        /*-------------------------------------*/

        for (i = 0; i <= bound; i++)
        {
            s_hi=0;
            s_lo=0x4000;//L_ROUND/2;


            /*Lmult=L_MULT(pswVOld[i], pswQntRc[j]);
            L_sum1=L_ADD(L_ROUND,Lmult);*/
            //L_sum = L_mac(L_ROUND , pswVOld[i] , pswQntRc[j]);
            VPP_MLA16(s_hi, s_lo  , pswVOld[i] , pswQntRc[j]);


            /*Lmult=L_MULT(pswVOld[-i], pswQntRc[j]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            //L_sum = L_mac(L_sum   , pswVOld[-i], pswQntRc[j]);
            VPP_MLA16(s_hi, s_lo  , pswVOld[-i], pswQntRc[j]);


            /*Lmult=L_MULT(pswPOld[i], pswQntRcSqd[j]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            //L_sum = L_mac(L_sum   , pswPOld[i] , pswQntRcSqd[j]);
            VPP_MLA16(s_hi, s_lo  , pswPOld[i] , pswQntRcSqd[j]);

            /*Lmult=L_MULT(-(pswPOld[i]) , SW_MIN);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            //L_sum = L_msu(L_sum    , pswPOld[i]    , SW_MIN);
            VPP_MLA16(s_hi, s_lo   , -(pswPOld[i]) , SW_MIN);

            //L_sum1=L_SHL_SAT(s_lo, 1);//L_MLA_SAT(s_hi, s_lo);//
            //L_sum2=L_MLA_SAT(s_hi, s_lo);//
            L_sum= L_MLA_SAT_CARRY(s_lo, 0);//L_MLA_CARRY_SAT(s_hi, s_lo, 0);

            //if (L_sum1!=L_sum)//((L_sum1==0x7fffffff) || (L_sum1==0x80000000) )//
            //L_sum1=0;

            pswPNew[i] = EXTRACT_H(L_sum);
        }

        /* Check if potential for limiting exists. */
        /*-----------------------------------------*/

        if ( pswPNew[0] >= 0x4000 )
            iLimit = 1;

        /* Compute the new Vsubj(i) */
        /*--------------------------*/

        for (i = -bound; i < 0; i++)
        {
            s_hi=0;
            s_lo=0x4000;//L_ROUND/2;
            //L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
            /*Lmult=L_MULT(-(pswVOld[i + 1]), SW_MIN);
            L_sum1=L_ADD(L_ROUND,Lmult);*/
            VPP_MLA16(s_hi, s_lo   , -(pswVOld[i + 1]), SW_MIN);

            //L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
            /*Lmult=L_MULT(pswQntRcSqd[j], pswVOld[-i - 1]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,pswQntRcSqd[j], pswVOld[-i - 1]);

            //L_sum = L_mac(L_sum, pswQntRc[j], pswPOld[-i - 1]);
            /*Lmult=L_MULT(pswQntRc[j], pswPOld[-i - 1]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,pswQntRc[j], pswPOld[-i - 1]);

            //L_sum = L_mac(L_sum, pswQntRc[j], pswPOld[-i - 1]);
            /*Lmult=L_MULT(pswQntRc[j], pswPOld[-i - 1]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,pswQntRc[j], pswPOld[-i - 1]);

            L_sum=L_MLA_CARRY_SAT(s_hi, s_lo, 0);

            pswVNew[i] = EXTRACT_H(L_sum);
        }

        for (i = 0; i <= bound; i++)
        {
            s_hi=0;
            s_lo=0x4000;//L_ROUND/2;
            //L_sum = L_msu(L_ROUND, pswVOld[i + 1], SW_MIN);
            /*Lmult=L_MULT(-(pswVOld[i + 1]), SW_MIN);
            L_sum1=L_ADD(L_ROUND,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,-(pswVOld[i + 1]), SW_MIN);

            //L_sum = L_mac(L_sum, pswQntRcSqd[j], pswVOld[-i - 1]);
            /*Lmult=L_MULT(pswQntRcSqd[j], pswVOld[-i - 1]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,pswQntRcSqd[j], pswVOld[-i - 1]);

            //L_sum = L_mac(L_sum, pswQntRc[j], pswPOld[i + 1]);
            /*Lmult=L_MULT(pswQntRc[j], pswPOld[i + 1]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,pswQntRc[j], pswPOld[i + 1]);

            //L_sum = L_mac(L_sum, pswQntRc[j], pswPOld[i + 1]);
            /*Lmult=L_MULT(pswQntRc[j], pswPOld[i + 1]);
            L_sum1=L_ADD(L_sum1,Lmult);*/
            VPP_MLA16(s_hi, s_lo   ,pswQntRc[j], pswPOld[i + 1]);

            L_sum=L_MLA_CARRY_SAT(s_hi, s_lo, 0);

            pswVNew[i] = EXTRACT_H(L_sum);
        }

        if (j < swSegmentOrder - 2)
        {

            /* Swap POld and PNew buffers, using modulo addressing */
            /*-----------------------------------------------------*/

            pswPOld = ppswPAddrs[(j + 1) % 2];
            pswPNew = ppswPAddrs[j % 2];

            /* Swap VOld and VNew buffers, using modulo addressing */
            /*-----------------------------------------------------*/

            pswVOld = ppswVAddrs[(j + 1) % 2];
            pswVNew = ppswVAddrs[j % 2];

        }
    }

    /* Computing Psubj(0) for the last lattice stage */
    /*-----------------------------------------------*/

    j = swSegmentOrder - 1;

    s_lo=0x4000;//L_ROUND/2;
    s_hi=0;
    //L_sum = L_mac(L_ROUND, pswVNew[0], pswQntRc[j]);
    /*Lmult=L_MULT(pswVNew[0], pswQntRc[j]);
    L_sum1=L_ADD(L_ROUND,Lmult);*/
    VPP_MLA16(s_hi, s_lo   , pswVNew[0], pswQntRc[j]);

    //L_sum = L_mac(L_sum, pswVNew[0], pswQntRc[j]);
    /*Lmult=L_MULT( pswVNew[0], pswQntRc[j]);
    L_sum1=L_ADD(L_sum1,Lmult);*/
    VPP_MLA16(s_hi, s_lo   ,pswVNew[0], pswQntRc[j]);

    //L_sum = L_mac(L_sum, pswPNew[0], pswQntRcSqd[j]);
    /*Lmult=L_MULT( pswPNew[0], pswQntRcSqd[j]);
    L_sum1=L_ADD(L_sum1,Lmult);*/
    VPP_MLA16(s_hi, s_lo   ,pswPNew[0], pswQntRcSqd[j]);

    //L_sum = L_msu(L_sum, pswPNew[0], SW_MIN);
    /*Lmult=L_MULT( -(pswPNew[0]), SW_MIN);
    L_sum1=L_ADD(L_sum1,Lmult);*/
    VPP_MLA16(s_hi, s_lo   ,-(pswPNew[0]), SW_MIN);

    L_sum=L_MLA_CARRY_SAT(s_hi, s_lo, 0);

    swRe = EXTRACT_H(L_sum);

    /* Return the residual energy corresponding to the reflection   */
    /* coefficient vector being evaluated.                          */
    /*--------------------------------------------------------------*/

    VPP_HR_PROFILE_FUNCTION_EXIT(aflatRecursion );

    return (swRe);                       /* residual error is returned */

}

/***************************************************************************
 *
 *   FUNCTION NAME: bestDelta
 *
 *   PURPOSE:
 *
 *     This function finds the delta-codeable lag which maximizes CC/G.
 *
 *   INPUTS:
 *
 *     pswLagList[0:siNumLags-1]
 *
 *                     List of delta-codeable lags over which search is done.
 *
 *     pswCSfrm[0:127]
 *
 *                     C(k) sequence, k integer.
 *
 *     pswGSfrm[0:127]
 *
 *                     G(k) sequence, k integer.
 *
 *     siNumLags
 *
 *                     Number of lags in contention.
 *
 *     siSfrmIndex
 *
 *                     The index of the subframe to which the delta-code
 *                     applies.
 *
 *
 *   OUTPUTS:
 *
 *     pswLTraj[0:3]
 *
 *                     The winning lag is put into this array at
 *                     pswLTraj[siSfrmIndex]
 *
 *     pswCCTraj[0:3]
 *
 *                     The corresponding winning C**2 is put into this
 *                     array at pswCCTraj[siSfrmIndex]
 *
 *     pswGTraj[0:3]
 *
 *                     The corresponding winning G is put into this arrray
 *                     at pswGTraj[siSfrmIndex]
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *   REFERENCE:  Sub-clause 4.1.8.3 of GSM Recommendation 06.20
 *
 *   KEYWORDS:
 *
 *************************************************************************/

void   bestDelta(INT16 pswLagList[],
                 INT16 pswCSfrm[],
                 INT16 pswGSfrm[],
                 short int siNumLags,
                 short int siSfrmIndex,
                 INT16 pswLTraj[],
                 INT16 pswCCTraj[],
                 INT16 pswGTraj[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswCBuf[DELTA_LEVELS + CG_INT_MACS + 2],
          pswGBuf[DELTA_LEVELS + CG_INT_MACS + 2],
          pswCInterp[DELTA_LEVELS + 2],
          pswGInterp[DELTA_LEVELS + 2],
          *psw1,
          *psw2,
          swCmaxSqr,
          swGmax,
          swPeak;
    short int siIPLo,
          siRemLo,
          siIPHi,
          siRemHi,
          siLoLag,
          siHiLag,
          siI;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* get bounds for integer C's and G's needed for interpolation */
    /* get integer and fractional portions of boundary lags        */
    /* ----------------------------------------------------------- */

    get_ipjj(pswLagList[0], &siIPLo, &siRemLo);

    get_ipjj(pswLagList[siNumLags - 1], &siIPHi, &siRemHi);

    /* get lag for first and last C and G required */
    /* ------------------------------------------- */

    siLoLag = SUB(siIPLo, CG_INT_MACS / 2 - 1);

    if (siRemHi != 0)
    {
        //siHiLag = ADD_SAT16(siIPHi, CG_INT_MACS / 2);
        siHiLag = ADD(siIPHi, (CG_INT_MACS / 2));

    }
    else
    {
        //siHiLag = ADD_SAT16(siIPHi, CG_INT_MACS / 2 - 1);
        siHiLag = ADD(siIPHi, ((CG_INT_MACS / 2) - 1));

    }

    /* transfer needed integer C's and G's to temp buffers */
    /* --------------------------------------------------- */

    psw1 = pswCBuf;
    psw2 = pswGBuf;

    if (siRemLo == 0)
    {

        /* first lag in list is integer: don't care about first entries */
        /* (they will be paired with zero tap in interpolating filter)  */
        /* ------------------------------------------------------------ */

        psw1[0] = 0;
        psw2[0] = 0;
        psw1 = &psw1[1];
        psw2 = &psw2[1];
    }

    for (siI = siLoLag; siI <= siHiLag; siI++)
    {
        psw1[siI - siLoLag] = pswCSfrm[siI - LSMIN];
        psw2[siI - siLoLag] = pswGSfrm[siI - LSMIN];
    }

    if (siRemLo == 0)
    {
        /* make siLoLag correspond to first entry in temp buffers */
        /* ------------------------------------------------------ */
        siLoLag--;// = sub(siLoLag, 1);
    }

    /* interpolate to get C's and G's which correspond to lags in list */
    /* --------------------------------------------------------------- */

    CGInterp(pswLagList, siNumLags, pswCBuf, pswGBuf, siLoLag,
             pswCInterp, pswGInterp);

    /* find max C*C*sgn(C)/G */
    /* --------------------- */

    swPeak = maxCCOverGWithSign(pswCInterp, pswGInterp, &swCmaxSqr, &swGmax,
                                siNumLags);

    /* store best lag and corresponding C*C and G */
    /* ------------------------------------------ */

    pswLTraj[siSfrmIndex] = pswLagList[swPeak];
    pswCCTraj[siSfrmIndex] = swCmaxSqr;
    pswGTraj[siSfrmIndex] = swGmax;

}


/***************************************************************************
 *
 *   FUNCTION NAME: CGInterp
 *
 *   PURPOSE:
 *
 *     Given a list of fractional lags, a C(k) array, and a G(k) array
 *     (k integer), this function generates arrays of C's and G's
 *     corresponding to the list of fractional lags by interpolating the
 *     integer C(k) and G(k) arrays.
 *
 *   INPUTS:
 *
 *     pswLIn[0:siNum-1]
 *
 *                     List of valid lags
 *
 *     siNum
 *
 *                     Length of output lists
 *
 *     pswCIn[0:variable]
 *
 *                     C(k) sequence, k integer.  The zero index corresponds
 *                     to k = siLoIntLag.
 *
 *     pswGIn[0:variable]
 *
 *                     G(k) sequence, k integer.  The zero index corresponds
 *                     to k = siLoIntLag.
 *
 *     siLoIntLag
 *
 *                     Integer lag corresponding to the first entry in the
 *                     C(k) and G(k) input arrays.
 *
 *     ppsrCGIntFilt[0:5][0:5]
 *
 *                     The FIR interpolation filter for C's and G's.
 *
 *   OUTPUTS:
 *
 *     pswCOut[0:siNum-1]
 *
 *                     List of interpolated C's corresponding to pswLIn.
 *
 *     pswGOut[0:siNum-1]
 *
 *                     List of interpolated G's corresponding to pswLIn
 *
 *   RETURN VALUE: none
 *
 *   DESCRIPTION:
 *
 *
 *   REFERENCE:  Sub-clause 4.1.8.2, 4.1.8.3 of GSM Recommendation 06.20
 *
 *   KEYWORDS: lag, interpolateCG
 *
 *************************************************************************/

void   CGInterp(INT16 pswLIn[],
                short siNum,
                INT16 pswCIn[],
                INT16 pswGIn[],
                short siLoIntLag,
                INT16 pswCOut[],
                INT16 pswGOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16 i,
          swBig,
          swLoIntLag;
    INT16 swLagInt,
          swTempRem,
          swLagRem;
    INT32 L_Temp,
          L_Temp1,
          L_Temp2;//,Lmult

    register INT32  s_hi1=0;
    register INT32  s_hi2=0;
    register UINT32 s_lo1=0;
    register UINT32 s_lo2=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    //swLoIntLag = ADD_SAT16(siLoIntLag, (CG_INT_MACS / 2) - 1);
    swLoIntLag = ADD(siLoIntLag, ((CG_INT_MACS / 2) - 1));

    for (swBig = 0; swBig < siNum; swBig++)
    {

        /* Separate integer and fractional portions of lag */
        /*-------------------------------------------------*/
        L_Temp = L_MULT(pswLIn[swBig], INV_OS_FCTR);
        swLagInt = EXTRACT_H(L_Temp);

        /* swLagRem = (OS_FCTR - pswLIn[iBig] % OS_FCTR)) */
        /*---------------------------------------------------*/
        swTempRem = EXTRACT_L(L_Temp);
        //swTempRem = shr(swTempRem, 1);
        swTempRem = SHR(swTempRem, 1);
        swLagRem = swTempRem & SW_MAX;
        swLagRem = MULT_R(swLagRem, OS_FCTR);
        /*VPP_MULT_R(m_hi, m_lo, swLagRem, OS_FCTR);
        swLagRem = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
        swLagRem = SUB(OS_FCTR, swLagRem);

        /* Get interpolated C and G values */
        /*--------------------------*/

        /*L_Temp1 = L_mac(32768, pswCIn[swLagInt - swLoIntLag],
                        ppsrCGIntFilt[0][swLagRem]);*/
        s_lo1=0x4000;//32768/2;
        s_hi1=0;
        VPP_MLA16(s_hi1, s_lo1, pswCIn[swLagInt - swLoIntLag], ppsrCGIntFilt[0][swLagRem]);

        /*L_Temp2 = L_mac(32768, pswGIn[swLagInt - swLoIntLag],
                        ppsrCGIntFilt[0][swLagRem]);*/
        s_lo2=0x4000;//32768/2;
        s_hi2=0;
        VPP_MLA16(s_hi2, s_lo2, pswGIn[swLagInt - swLoIntLag],ppsrCGIntFilt[0][swLagRem]);

        for (i = 1; i <= CG_INT_MACS - 1; i++)
        {
            /*L_Temp1 = L_mac(L_Temp1, pswCIn[i + swLagInt - swLoIntLag],
                            ppsrCGIntFilt[i][swLagRem]);*/
            /*Lmult=L_MULT(pswCIn[i + swLagInt - swLoIntLag], ppsrCGIntFilt[i][swLagRem]);
            L_Temp1=L_ADD(L_Temp1,Lmult);*/
            VPP_MLA16(s_hi1, s_lo1, pswCIn[i + swLagInt - swLoIntLag], ppsrCGIntFilt[i][swLagRem]);

            /*L_Temp2 = L_mac(L_Temp2, pswGIn[i + swLagInt - swLoIntLag],
                            ppsrCGIntFilt[i][swLagRem]);*/
            /*Lmult=L_MULT(pswGIn[i + swLagInt - swLoIntLag],
                            ppsrCGIntFilt[i][swLagRem]);
            L_Temp2=L_ADD(L_Temp2,Lmult);*/
            VPP_MLA16(s_hi2, s_lo2, pswGIn[i + swLagInt - swLoIntLag], ppsrCGIntFilt[i][swLagRem]);

        }
        L_Temp1=L_MLA_CARRY_SAT(s_hi1, s_lo1, 0);
        L_Temp2=L_MLA_CARRY_SAT(s_hi2, s_lo2, 0);

        pswCOut[swBig] = EXTRACT_H(L_Temp1);
        pswGOut[swBig] = EXTRACT_H(L_Temp2);
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: CGInterpValid
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to retrieve the valid (codeable) lags
 *     within one (exclusive) integer sample of the given integer lag, and
 *     interpolate the corresponding C's and G's from the integer arrays
 *
 *   INPUTS:
 *
 *     swFullResLag
 *
 *                     integer lag * OS_FCTR
 *
 *     pswCIn[0:127]
 *
 *                     integer C sequence
 *
 *     pswGIn[0:127]
 *
 *                     integer G sequence
 *
 *     psrLagTbl[0:255]
 *
 *                     reference table of valid (codeable) lags
 *
 *
 *   OUTPUTS:
 *
 *     pswLOut[0:*psiNum-1]
 *
 *                     list of valid lags within 1 of swFullResLag
 *
 *     pswCOut[0:*psiNum-1]
 *
 *                     list of interpolated C's corresponding to pswLOut
 *
 *     pswGOut[0:*psiNum-1]
 *
 *                     list of interpolated G's corresponding to pswLOut
 *
 *   RETURN VALUE:
 *
 *     siNum
 *
 *                     length of output lists
 *
 *   DESCRIPTION:
 *
 *   REFERENCE:  Sub-clause 4.1.8.2, 4.1.9 of GSM Recommendation 06.20
 *
 *   KEYWORDS: CGInterpValid, cginterpvalid, CG_INT_VALID
 *
 *************************************************************************/

short  CGInterpValid(INT16 swFullResLag,
                     INT16 pswCIn[],
                     INT16 pswGIn[],
                     INT16 pswLOut[],
                     INT16 pswCOut[],
                     INT16 pswGOut[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int siLowerBound,
          siUpperBound,
          siNum,
          siI;
    INT16 swLag;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Get lower and upper bounds for valid lags     */
    /* within 1 (exclusive) integer lag of input lag */
    /* --------------------------------------------- */

    //swLag = sub(swFullResLag, OS_FCTR);
    swLag = SUB(swFullResLag, OS_FCTR);
    swLag = quantLag(swLag, &siLowerBound);
    if ( swLag != swFullResLag )
    {
        //siLowerBound = ADD_SAT16(siLowerBound, 1);
        siLowerBound++;

    }

    //swLag = ADD_SAT16(swFullResLag, OS_FCTR);
    swLag = ADD(swFullResLag, OS_FCTR);
    swLag = quantLag(swLag, &siUpperBound);
    if ( swLag != swFullResLag )
    {
        siUpperBound--;// = sub(siUpperBound, 1);
    }

    /* Get list of full resolution lags whose */
    /* C's and G's will be interpolated       */
    /* -------------------------------------- */

    siNum = SUB(siUpperBound, siLowerBound);
    //siNum = ADD_SAT16(siNum, 1);
    siNum++;// = ADD(siNum, 1);

    for (siI = 0; siI < siNum; siI++)
    {
        pswLOut[siI] = psrLagTbl[siI + siLowerBound];
    }

    /* Interpolate C's and G's */
    /* ----------------------- */

    CGInterp(pswLOut, siNum, pswCIn, pswGIn, LSMIN, pswCOut,
             pswGOut);

    /* Return the length of the output lists */
    /* ------------------------------------- */

    return (siNum);
}

/***************************************************************************
 *
 *   FUNCTION NAME: compResidEnergy
 *
 *   PURPOSE:
 *
 *     Computes and compares the residual energy from interpolated and
 *     non-interpolated coefficients. From the difference determines
 *     the soft interpolation decision.
 *
 *   INPUTS:
 *
 *     pswSpeech[0:159] ( [0:F_LEN-1] )
 *
 *                     Input speech frame (after high-pass filtering).
 *
 *     ppswInterpCoef[0:3][0:9] ( [0:N_SUB-1][0:NP-1] )
 *
 *                     Set of interpolated LPC direct-form coefficients for
 *                     each subframe.
 *
 *     pswPreviousCoef[0:9} ( [0:NP-1] )
 *
 *                     Set of LPC direct-form coefficients corresponding to
 *                     the previous frame
 *
 *     pswCurrentCoef[0:9} ( [0:NP-1] )
 *
 *                     Set of LPC direct-form coefficients corresponding to
 *                     the current frame
 *
 *     psnsSqrtRs[0:3] ( [0:N_SUB-1] )
 *
 *                     Array of residual energy estimates for each subframe
 *                     based on interpolated coefficients.  Used for scaling.
 *
 *   RETURN:
 *
 *     Returned value indicates the coefficients to use for each subframe:
 *     One indicates interpolated coefficients are to be used, zero indicates
 *     un-interpolated coefficients are to be used.
 *
 *   DESCRIPTION:
 *
 *
 *   REFERENCE:  Sub-clause 4.1.6 of GSM Recommendation 06.20
 *
 *   Keywords: openlooplagsearch, openloop, lag, pitch
 *
 **************************************************************************/



short  compResidEnergy(INT16 pswSpeech[],
                       INT16 ppswInterpCoef[][NP],
                       INT16 pswPreviousCoef[],
                       INT16 pswCurrentCoef[],
                       struct NormSw psnsSqrtRs[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short  i,
           j,
           siOverflowPossible,
           siInterpDecision;
    INT16 swMinShift,swSample, swShiftFactor=0;
    INT16 *pswCoef=NULL;
    INT16 pswTempState[NP];
    INT16 pswResidual[S_LEN];
    INT32 L_ResidualEng;//,Lmult

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    /*_________________________________________________________________________
     |                                                                         |
     |                            Executable Code                              |
     |_________________________________________________________________________|
    */

    /* Find minimum shift count of the square-root of residual energy */
    /* estimates over the four subframes.  According to this minimum, */
    /* find a shift count for the residual signal which will be used  */
    /* to avoid overflow when the actual residual energies are        */
    /* calculated over the frame                                      */
    /*----------------------------------------------------------------*/

    swMinShift = SW_MAX;
    for (i = 0; i < N_SUB; i++)
    {

        if (SUB(psnsSqrtRs[i].sh, swMinShift) < 0 && psnsSqrtRs[i].man > 0)
            swMinShift = psnsSqrtRs[i].sh;
    }

    if ( swMinShift >= 1 )
    {

        siOverflowPossible = 0;
    }

    else if (swMinShift == 0)
    {
        siOverflowPossible = 1;
        swShiftFactor = ONE_HALF;
    }

    else if ( swMinShift == -1 )
    {
        siOverflowPossible = 1;
        swShiftFactor = ONE_QUARTER;
    }

    else
    {
        siOverflowPossible = 1;
        swShiftFactor = ONE_EIGHTH;
    }

    /* Copy analysis filter state into temporary buffer */
    /*--------------------------------------------------*/

    for (i = 0; i < NP; i++)
        pswTempState[i] = pswAnalysisState[i];

    /* Send the speech frame, one subframe at a time, through the analysis */
    /* filter which is based on interpolated coefficients.  After each     */
    /* subframe, accumulate the energy in the residual signal, scaling to  */
    /* avoid overflow if necessary.                                        */
    /*---------------------------------------------------------------------*/

    L_ResidualEng = 0;

    s_lo=0 ;
    s_hi=0 ;

    for (i = 0; i < N_SUB; i++)
    {

        lpcFir(&pswSpeech[i * S_LEN], ppswInterpCoef[i], pswTempState,
               pswResidual);



        if (siOverflowPossible)
        {

            for (j = 0; j < S_LEN; j++)
            {

                swSample = MULT_R(swShiftFactor, pswResidual[j]);
                /*VPP_MULT_R(m_hi, m_lo, swShiftFactor, pswResidual[j]);
                swSample = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
                //L_ResidualEng = L_mac(L_ResidualEng, swSample, swSample);
                /*Lmult=L_MULT(swSample, swSample);
                L_ResidualEng=L_ADD(L_ResidualEng,Lmult);*/
                VPP_MLA16(s_hi, s_lo, swSample, swSample);
            }
        }

        else
        {

            for (j = 0; j < S_LEN; j++)
            {

                //L_ResidualEng = L_mac(L_ResidualEng, pswResidual[j], pswResidual[j]);
                /*Lmult=L_MULT(pswResidual[j], pswResidual[j]);
                L_ResidualEng=L_ADD(L_ResidualEng,Lmult);*/
                VPP_MLA16(s_hi, s_lo, pswResidual[j], pswResidual[j]);
            }
        }
    }

    //L_ResidualEng=L_MLA_SAT(s_hi, s_lo);

    /* Send the speech frame, one subframe at a time, through the analysis */
    /* filter which is based on un-interpolated coefficients.  After each  */
    /* subframe, subtract the energy in the residual signal from the       */
    /* accumulated residual energy due to the interpolated coefficient     */
    /* analysis filter, again scaling to avoid overflow if necessary.      */
    /* Note that the analysis filter state is updated during these         */
    /* filtering operations.                                               */
    /*---------------------------------------------------------------------*/

    for (i = 0; i < N_SUB; i++)
    {

        switch (i)
        {

            case 0:

                pswCoef = pswPreviousCoef;
                break;

            case 1:
            case 2:
            case 3:

                pswCoef = pswCurrentCoef;
                break;
        }

        lpcFir(&pswSpeech[i * S_LEN], pswCoef, pswAnalysisState,
               pswResidual);

        if (siOverflowPossible)
        {

            for (j = 0; j < S_LEN; j++)
            {

                swSample = MULT_R(swShiftFactor, pswResidual[j]);
                /*VPP_MULT_R(m_hi, m_lo, swShiftFactor, pswResidual[j]);
                swSample = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
                //L_ResidualEng = L_msu(L_ResidualEng, swSample, swSample);
                VPP_MLA16(s_hi, s_lo, -swSample, swSample);
            }
        }

        else
        {

            for (j = 0; j < S_LEN; j++)
            {

                //L_ResidualEng = L_msu(L_ResidualEng, pswResidual[j], pswResidual[j]);
                VPP_MLA16(s_hi, s_lo, -(pswResidual[j]), pswResidual[j]);
            }
        }
    }

    L_ResidualEng=L_MLA_CARRY_SAT(s_hi, s_lo, 0);

    /* Make soft-interpolation decision based on the difference in residual */
    /* energies                                                             */
    /*----------------------------------------------------------------------*/

    if (L_ResidualEng < 0)
        siInterpDecision = 1;

    else
        siInterpDecision = 0;

    return siInterpDecision;
}

/***************************************************************************
 *
 *    FUNCTION NAME: cov32
 *
 *    PURPOSE: Calculates B, F, and C correlation matrices from which
 *             the reflection coefficients are computed using the FLAT
 *             algorithm. The Spectral Smoothing Technique (SST) is applied
 *             to the correlations. End point correction is employed
 *             in computing the correlations to minimize computation.
 *
 *    INPUT:
 *
 *       pswIn[0:169]
 *                     A sampled speech vector used to compute
 *                     correlations need for generating the optimal
 *                     reflection coefficients via the FLAT algorithm.
 *
 *       CVSHIFT       The number of right shifts by which the normalized
 *                     correlations are to be shifted down prior to being
 *                     rounded into the INT16 output correlation arrays
 *                     B, F, and C.
 *
 *       pL_rFlatSstCoefs[NP]
 *
 *                     A table stored in Rom containing the spectral
 *                     smoothing function coefficients.
 *
 *    OUTPUTS:
 *
 *       pppL_B[0:NP-1][0:NP-1][0:1]
 *                     An output correlation array containing the backward
 *                     correlations of the input signal. It is a square
 *                     matrix symmetric about the diagonal. Only the upper
 *                     right hand triangular region of this matrix is
 *                     initialized, but two dimensional indexing is retained
 *                     to enhance clarity. The third array dimension is used
 *                     by function flat to swap the current and the past
 *                     values of B array, eliminating the need to copy
 *                     the updated B values onto the old B values at the
 *                     end of a given lattice stage. The third dimension
 *                     is similarily employed in arrays F and C.
 *
 *       pppL_F[0:NP-1][0:NP-1][0:1]
 *                     An output correlation array containing the forward
 *                     correlations of the input signal. It is a square
 *                     matrix symmetric about the diagonal. Only the upper
 *                     right hand triangular region of this matrix is
 *                     initialized.
 *
 *       pppL_C[0:NP-1][0:NP-1][0:1]
 *                     An output correlation array containing the cross
 *                     correlations of the input signal. It is a square
 *                     matrix which is not symmetric. All its elements
 *                     are initialized, for the third dimension index = 0.
 *
 *       pL_R0         Average normalized signal power over F_LEN
 *                     samples, given by 0.5*(Phi(0,0)+Phi(NP,NP)), where
 *                     Phi(0,0) and Phi(NP,NP) are normalized signal
 *                     autocorrelations.  The average unnormalized signal
 *                     power over the frame is given by adjusting L_R0 by
 *                     the shift count which is returned. pL_R0 along
 *                     with the returned shift count are the inputs to
 *                     the frame energy quantizer.
 *
 *        INT32 pL_VadAcf[4]
 *                     An array with the autocorrelation coefficients to be
 *                     used by the VAD.
 *
 *        INT16 *pswVadScalAuto
 *                     Input scaling factor used by the VAD.
 *
 *    RETURN:
 *
 *       swNormPwr     The shift count to be applied to pL_R0 for
 *                     reconstructing the average unnormalized
 *                     signal power over the frame.
 *                     Negative shift count means that a left shift was
 *                     applied to the correlations to achieve a normalized
 *                     value of pL_R0.
 *
 *   DESCRIPTION:
 *
 *
 *      The input energy of the signal is assumed unknown.  It maximum
 *      can be F_LEN*0.5. The 0.5 factor accounts for scaling down of the
 *      input signal in the high-pass filter.  Therefore the signal is
 *      shifted down by 3 shifts producing an energy reduction of 2^(2*3)=64.
 *      The resulting energy is then normalized.  Based on the shift count,
 *      the correlations F, B, and C are computed using as few shifts as
 *      possible, so a precise result is attained.
 *      This is an implementation of equations: 2.1 through 2.11.
 *
 *   REFERENCE:  Sub-clause 4.1.3 of GSM Recommendation 06.20
 *
 *   keywords: energy, autocorrelation, correlation, cross-correlation
 *   keywords: spectral smoothing, SST, LPC, FLAT, flat
 *
 *************************************************************************/

INT16 cov32(INT16 pswIn[],
            INT32 pppL_B[NP][NP][2],
            INT32 pppL_F[NP][NP][2],
            INT32 pppL_C[NP][NP][2],
            INT32 *pL_R0,
            INT32 pL_VadAcf[],
            INT16 *pswVadScalAuto)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_max,
          L_Pwr0,
          L_Pwr,
          L_temp,
          pL_Phi[NP + 1];//,Lmult,essai,essai0,L_Pwr1
    INT16 swTemp,
          swNorm,
          swNormSig,
          swNormPwr,
          pswInScale[A_LEN],
          swPhiNorm;
    short int i,
          k,
          kk,
          n;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    //register INT32  s_hi1=0;
    //register UINT32 s_lo1=0;


    //VPP_HR_PROFILE_FUNCTION_ENTER(cov32);

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Calculate energy in the frame vector (160 samples) for each   */
    /* of NP frame placements. The energy is reduced by 64. This is  */
    /* accomplished by shifting the input right by 3 bits. An offset */
    /* of 0x117f0b is placed into the accumulator to account for     */
    /* the worst case power gain due to the 3 LSB's of the input     */
    /* signal which were right shifted. The worst case is that the   */
    /* 3 LSB's were all set to 1 for each of the samples. Scaling of */
    /* the input by a half is assumed here.                          */
    /*---------------------------------------------------------------*/

    L_max = 0;
    s_lo= SHR(0x117f0b, 1);//0x8BF85;//
    s_hi=0;
    for (i = 0; i < F_LEN; i++)//(L_Pwr = 0x117f0b, i = 0; i < F_LEN; i++)//
    {
        //swTemp = shr(pswIn[i], 3);
        swTemp = SHR(pswIn[i], 3);
        //L_Pwr = L_mac(L_Pwr, swTemp, swTemp);
        /*Lmult=L_MULT(swTemp, swTemp);
        L_Pwr=L_ADD(L_Pwr,Lmult);*/
        VPP_MLA16(s_hi, s_lo, swTemp, swTemp);
    }
    L_max |=  L_MLA_SAT_CARRY(s_lo, 0x117f0b&1);//L_MLA_CARRY_SAT(s_hi, s_lo, 0);//L_Pwr = L_MLA_SAT(s_hi, s_lo);//
    //L_max |= L_Pwr;

    /* L_max tracks the maximum power over NP window placements */
    /*----------------------------------------------------------*/

    for (i = 1; i <= NP; i++)
    {

        /* Subtract the power due to 1-st sample from previous window
         * placement. */
        /*-----------------------------------------------------------*/

        //swTemp = shr(pswIn[i - 1], 3);
        swTemp = SHR(pswIn[i - 1], 3);
        //L_Pwr = L_msu(L_Pwr, swTemp, swTemp);
        VPP_MLA16(s_hi, s_lo, -swTemp, swTemp);

        /* Add the power due to new sample at the current window placement. */
        /*------------------------------------------------------------------*/

        //swTemp = shr(pswIn[F_LEN + i - 1], 3);
        swTemp = SHR(pswIn[F_LEN + i - 1], 3);
        //L_Pwr = L_mac(L_Pwr, swTemp, swTemp);
        /*Lmult=L_MULT(swTemp, swTemp);
        L_Pwr=L_ADD(L_Pwr,Lmult);*/
        VPP_MLA16(s_hi, s_lo, swTemp, swTemp);

        L_Pwr = L_MLA_CARRY_SAT(s_hi, s_lo, 0);

        L_max |= L_Pwr;

    }

    /* Compute the shift count needed to achieve normalized value */
    /* of the correlations.                                       */
    /*------------------------------------------------------------*/

    swTemp = norm_l(L_max);
    swNorm = SUB(6, swTemp);

    if (swNorm >= 0)
    {

        /* The input signal needs to be shifted down, to avoid limiting */
        /* so compute the shift count to be applied to the input.       */
        /*--------------------------------------------------------------*/

        //swTemp = ADD_SAT16(swNorm, 1);
        swTemp = ADD(swNorm, 1);
        //swNormSig = shr(swTemp, 1);
        swNormSig = SHR(swTemp, 1);
        //swNormSig = ADD_SAT16(swNormSig, 0x0001);
        swNormSig = ADD(swNormSig, 0x0001);

    }
    else
    {
        /* No scaling down of the input is necessary */
        /*-------------------------------------------*/

        swNormSig = 0;

    }

    /* Convert the scaling down, if any, which was done to the time signal */
    /* to the power domain, and save.                                      */
    /*---------------------------------------------------------------------*/

    swNormPwr = SHL(swNormSig, 1);

    /* Buffer the input signal, scaling it down if needed. */
    /*-----------------------------------------------------*/

    for (i = 0; i < A_LEN; i++)
    {
        //pswInScale[i] = shr(pswIn[i], swNormSig);
        pswInScale[i] = L_SHR_V(pswIn[i], swNormSig);

    }

    /* Compute from buffered (scaled) input signal the correlations     */
    /* needed for the computing the reflection coefficients.            */
    /*------------------------------------------------------------------*/

    /* Compute correlation Phi(0,0) */
    /*------------------------------*/

    //L_Pwr = L_MULT(pswInScale[NP], pswInScale[NP]);
    //s_lo= SHR(L_Pwr, 1);
    VPP_MLX16(s_hi, s_lo, pswInScale[NP], pswInScale[NP]);
    for (n = 1; n < F_LEN; n++)
    {
        //L_Pwr = L_mac(L_Pwr, pswInScale[NP + n], pswInScale[NP + n]);
        /*Lmult=L_MULT(pswInScale[NP + n], pswInScale[NP + n]);
        L_Pwr=L_ADD(L_Pwr,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswInScale[NP + n], pswInScale[NP + n]);

    }

    L_Pwr = L_MLA_CARRY_SAT(s_hi, s_lo, 0);
    pL_Phi[0] = L_Pwr;

    /* Get ACF[0] and input scaling factor for VAD algorithm */
    *pswVadScalAuto = swNormSig;
    pL_VadAcf[0] = L_Pwr;

    /* Compute the remaining correlations along the diagonal which */
    /* starts at Phi(0,0). End-point correction is employed to     */
    /* limit computation.                                          */
    /*-------------------------------------------------------------*/

    //s_lo= SHR(L_Pwr, 1);
    for (i = 1; i <= NP; i++)
    {

        /* Compute the power in the last sample from the previous         */
        /* window placement, and subtract it from correlation accumulated */
        /* at the previous window placement.                              */
        /*----------------------------------------------------------------*/

        /*L_Pwr = L_msu(L_Pwr, pswInScale[NP + F_LEN - i],
                      pswInScale[NP + F_LEN - i]);*/
        VPP_MLA16(s_hi, s_lo, -(pswInScale[NP + F_LEN - i]), pswInScale[NP + F_LEN - i]);

        /* Compute the power in the new sample for the current window       */
        /* placement, and add it to L_Pwr to obtain the value of Phi(i,i). */
        /*------------------------------------------------------------------*/

        //L_Pwr = L_mac(L_Pwr, pswInScale[NP - i], pswInScale[NP - i]);
        /*Lmult=L_MULT(pswInScale[NP - i], pswInScale[NP - i]);
        L_Pwr=L_ADD(L_Pwr,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswInScale[NP - i], pswInScale[NP - i]);

        L_Pwr = L_MLA_SAT_CARRY(s_lo, L_Pwr&1);// L_MLA_CARRY_SAT(s_hi, s_lo, (L_Pwr&1));//L_MLA_SAT(s_hi, s_lo);

        pL_Phi[i] = L_Pwr;

    }

    /* Compute the shift count necessary to normalize the Phi array  */
    /*---------------------------------------------------------------*/

    L_max = 0;
    for (i = 0; i <= NP; i++)
    {
        L_max |= pL_Phi[i];
    }
    swPhiNorm = norm_l(L_max);

    /* Adjust the shift count to be returned to account for any scaling */
    /* down which might have been done to the input signal prior to     */
    /* computing the correlations.                                      */
    /*------------------------------------------------------------------*/

    //swNormPwr = sub(swNormPwr, swPhiNorm);
    //swNormPwr = SUB_s(swNormPwr, swPhiNorm);
    swNormPwr = SUB(swNormPwr, swPhiNorm);

    /* Compute the average power over the frame; i.e.,                   */
    /* 0.5*(Phi(0,0)+Phi(NP,NP)), given a normalized pL_Phi array.       */
    /*-------------------------------------------------------------------*/

    //swTemp = sub(swPhiNorm, 1);
    swTemp =  (swPhiNorm - 1);
    //L_Pwr0 = L_shl(pL_Phi[0], swTemp);
    //L_Pwr = L_shl(pL_Phi[NP], swTemp);
    //L_Pwr0 = L_SHL_SAT(pL_Phi[0], swTemp);
    //L_Pwr = L_SHL_SAT(pL_Phi[NP], swTemp);
    L_Pwr0 =  SHL(pL_Phi[0], swTemp);
    L_Pwr =   SHL(pL_Phi[NP], swTemp);
    //*pL_R0 = L_ADD(L_Pwr, L_Pwr0);       /* Copy power to output pointer */
    *pL_R0 = ADD(L_Pwr, L_Pwr0);       /* Copy power to output pointer */

    /* Check if the average power is normalized; if not, shift left by 1 bit */
    /*-----------------------------------------------------------------------*/

    if (!(*pL_R0 & 0x40000000))
    {
        //*pL_R0 = L_shl(*pL_R0, 1);         /* normalize the average power    */
        *pL_R0 = SHL(*pL_R0, 1);         /* normalize the average power    */
        //swNormPwr = sub(swNormPwr, 1);     /* adjust the shift count         */
        swNormPwr = SUB(swNormPwr, 1);     /* adjust the shift count         */

    }

    /* Reduce the shift count needed to normalize the correlations   */
    /* by CVSHIFT bits.                                              */
    /*---------------------------------------------------------------*/

    //swNorm = sub(swPhiNorm, CVSHIFT);
    swNorm = SUB(swPhiNorm, CVSHIFT);

    /* Initialize the F, B, and C output correlation arrays, using the */
    /* Phi correlations computed along the diagonal of symmetry.       */
    /*-----------------------------------------------------------------*/

    //L_temp = L_shl(pL_Phi[0], swNorm);   /* Normalize the result     */
    L_temp = SHR_V(pL_Phi[0], swNorm);   /* Normalize the result     */


    pppL_F[0][0][0] = L_temp;            /* Write to output array    */

    for (i = 1; i <= NP - 1; i++)
    {

        //L_temp = L_shl(pL_Phi[i], swNorm); /* Normalize the result     */
        L_temp = SHR_V(pL_Phi[i], swNorm); /* Normalize the result     */



        pppL_F[i][i][0] = L_temp;          /* Write to output array    */
        pppL_B[i - 1][i - 1][0] = L_temp;  /* Write to output array    */
        pppL_C[i][i - 1][0] = L_temp;      /* Write to output array    */

    }

    //L_temp = L_shl(pL_Phi[NP], swNorm);  /* Normalize the result     */
    L_temp = SHR_V(pL_Phi[NP], swNorm);  /* Normalize the result     */

    pppL_B[NP - 1][NP - 1][0] = L_temp;  /* Write to output array    */

    for (k = 1; k <= NP - 1; k++)
    {

        /* Compute correlation Phi(0,k) */
        /*------------------------------*/

        //L_Pwr = L_MULT(pswInScale[NP], pswInScale[NP - k]);
        //s_lo= SHR(L_Pwr, 1);
        VPP_MLX16(s_hi, s_lo, pswInScale[NP], pswInScale[NP - k]);
        for (n = 1; n < F_LEN; n++)
        {
            //L_Pwr = L_mac(L_Pwr, pswInScale[NP + n], pswInScale[NP + n - k]);
            /*Lmult=L_MULT(pswInScale[NP + n], pswInScale[NP + n - k]);
            L_Pwr=L_ADD(L_Pwr,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswInScale[NP + n], pswInScale[NP + n - k]);

        }
        L_Pwr = L_MLA_CARRY_SAT(s_hi, s_lo, 0);

        /* convert covariance values to ACF and store for VAD algorithm */
        if (k < 9)
        {
            pL_VadAcf[k] = L_Pwr;
            //s_lo= SHR(L_Pwr, 1);
            for (kk = 0; kk < k; kk++)
            {
                /*pL_VadAcf[k] = L_msu(pL_VadAcf[k], pswInScale[NP + kk],
                                     pswInScale[NP + kk - k]);*/
                VPP_MLA16(s_hi, s_lo, -(pswInScale[NP + kk]),pswInScale[NP + kk - k]);
                pL_VadAcf[k] = L_MLA_CARRY_SAT(s_hi, s_lo, (L_Pwr&1));//L_MLA_SAT(s_hi, s_lo);
            }
        }

        //L_temp = L_shl(L_Pwr, swNorm);     /* Normalize the result */
        L_temp = SHR_V(L_Pwr, swNorm);     /* Normalize the result */

        L_temp = L_mpy_ll(L_temp, pL_rFlatSstCoefs[k - 1]); /* Apply SST */

        pppL_F[0][k][0] = L_temp;          /* Write to output array    */
        pppL_C[0][k - 1][0] = L_temp;      /* Write to output array    */


        /* Compute the remaining correlations along the diagonal which */
        /* starts at Phi(0,k). End-point correction is employed to     */
        /* limit computation.                                          */
        /*-------------------------------------------------------------*/

        s_lo= SHR(L_Pwr, 1);//
        //s_lo1=0;
        //s_hi1=s_hi;
        for (kk = k + 1, i = 1; kk <= NP - 1; kk++, i++)
        {

            /* Compute the power in the last sample from the previous         */
            /* window placement, and subtract it from correlation accumulated */
            /* at the previous window placement.                              */
            /*----------------------------------------------------------------*/

            /*L_Pwr = L_msu(L_Pwr, pswInScale[NP + F_LEN - i],
                          pswInScale[NP + F_LEN - kk]);*/

            VPP_MLA16(s_hi, s_lo, -(pswInScale[NP + F_LEN - i]), pswInScale[NP + F_LEN - kk]);


            //VPP_MLA16(s_hi1, s_lo1, -(pswInScale[NP + F_LEN - i]), pswInScale[NP + F_LEN - kk]);

            /* Compute the power in the new sample for the current window       */
            /* placement, and add it to L_Pwr to obtain the value of Phi(i,kk). */
            /*------------------------------------------------------------------*/



            //L_Pwr = L_mac(L_Pwr, pswInScale[NP - i], pswInScale[NP - kk]);
            /*Lmult=L_MULT(pswInScale[NP - i], pswInScale[NP - kk]);
            L_Pwr=L_ADD(L_Pwr,Lmult);*/
            VPP_MLA16(s_hi, s_lo, pswInScale[NP - i], pswInScale[NP - kk]);

            //VPP_MLA16(s_hi1, s_lo1, pswInScale[NP - i], pswInScale[NP - kk]);

            //L_Pwr1= L_MLA_CARRY_SAT(s_hi1, s_lo1, (L_Pwr&1));//

            L_Pwr= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);//L_MLA_CARRY_SAT(s_hi, s_lo, (L_Pwr&1));//
            /*L_Pwr1=L_MLA_CARRY_SAT(s_hi, s_lo, (L_Pwr&1));//

            if (L_Pwr1!=L_Pwr)//((L_Pwr==0x7fffffff) || (L_Pwr==0x80000000) )//
              L_Pwr1=0;*/

            //L_temp = L_shl(L_Pwr, swNorm);   /* Normalize */
            L_temp = SHR_V(L_Pwr, swNorm);   /* Normalize */


            L_temp = L_mpy_ll(L_temp, pL_rFlatSstCoefs[k - 1]);     /* Apply SST */

            pppL_F[i][kk][0] = L_temp;       /* Write to output array */
            pppL_B[i - 1][kk - 1][0] = L_temp;        /* Write to output array */
            pppL_C[i][kk - 1][0] = L_temp;   /* Write to output array    */
            pppL_C[kk][i - 1][0] = L_temp;   /* Write to output array    */

        }

        /* Compute the power in the last sample from the previous         */
        /* window placement, and subtract it from correlation accumulated */
        /* at the previous window placement.                              */
        /*----------------------------------------------------------------*/
        s_lo= SHR(L_Pwr, 1);
        //L_Pwr = L_msu(L_Pwr, pswInScale[F_LEN + k], pswInScale[F_LEN]);
        VPP_MLA16(s_hi, s_lo, -(pswInScale[F_LEN + k]), pswInScale[F_LEN]);

        /* Compute the power in the new sample for the current window       */
        /* placement, and add it to L_Pwr to obtain the value of Phi(i,kk). */
        /*------------------------------------------------------------------*/

        //L_Pwr = L_mac(L_Pwr, pswInScale[k], pswInScale[0]);
        /*Lmult=L_MULT(pswInScale[k], pswInScale[0]);
        L_Pwr=L_ADD(L_Pwr,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswInScale[k], pswInScale[0]);

        L_Pwr= L_MLA_SAT_CARRY(s_lo, L_Pwr&1);// L_MLA_SAT(s_hi, s_lo);

        //L_temp = L_shl(L_Pwr, swNorm);     /* Normalize the result */
        L_temp = SHR_V(L_Pwr, swNorm);     /* Normalize the result */


        L_temp = L_mpy_ll(L_temp, pL_rFlatSstCoefs[k - 1]); /* Apply SST */

        pppL_B[NP - k - 1][NP - 1][0] = L_temp;     /* Write to output array */
        pppL_C[NP - k][NP - 1][0] = L_temp;/* Write to output array */

    }

    /* Compute correlation Phi(0,NP) */
    /*-------------------------------*/

    //L_Pwr = L_MULT(pswInScale[NP], pswInScale[0]);
    //s_lo= SHR(L_Pwr, 1);
    VPP_MLX16(s_hi, s_lo, pswInScale[NP], pswInScale[0]);
    for (n = 1; n < F_LEN; n++)
    {
        //L_Pwr = L_mac(L_Pwr, pswInScale[NP + n], pswInScale[n]);
        /*Lmult=L_MULT(pswInScale[NP + n], pswInScale[n]);
        L_Pwr=L_ADD(L_Pwr,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswInScale[NP + n], pswInScale[n]);
    }

    L_Pwr= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    //L_temp = L_shl(L_Pwr, swNorm);       /* Normalize the result */
    L_temp = SHR_V(L_Pwr, swNorm);       /* Normalize the result */


    L_temp = L_mpy_ll(L_temp, pL_rFlatSstCoefs[NP - 1]);  /* Apply SST */

    pppL_C[0][NP - 1][0] = L_temp;       /* Write to output array */

    //VPP_HR_PROFILE_FUNCTION_EXIT( cov32);

    return (swNormPwr);

}

/***************************************************************************
 *
 *    FUNCTION NAME: filt4_2nd
 *
 *    PURPOSE:  Implements a fourth order filter by cascading two second
 *              order sections.
 *
 *    INPUTS:
 *
 *      pswCoef[0:9]   An array of two sets of filter coefficients.
 *
 *      pswIn[0:159]   An array of input samples to be filtered, filtered
 *                     output samples written to the same array.
 *
 *      pswXstate[0:3] An array containing x-state memory for two 2nd order
 *                     filter sections.
 *
 *      pswYstate[0:7] An array containing y-state memory for two 2nd order
 *                     filter sections.
 *
 *      npts           Number of samples to filter (must be even).
 *
 *      shifts         number of shifts to be made on output y(n).
 *
 *    OUTPUTS:
 *
 *       pswIn[0:159]  Output array containing the filtered input samples.
 *
 *    RETURN:
 *
 *       none.
 *
 *    DESCRIPTION:
 *
 *    data structure:
 *
 *    Coeff array order:  (b2,b1,b0,a2,a1)Section 1;(b2,b1,b0,a2,a1)Section 2
 *    Xstate array order: (x(n-2),x(n-1))Section 1; (x(n-2),x(n-1))Section 2
 *    Ystate array order: y(n-2)MSB,y(n-2)LSB,y(n-1)MSB,y(n-1)LSB Section 1
 *                        y(n-2)MSB,y(n-2)LSB,y(n-1)MSB,y(n-1)LSB Section 2
 *
 *    REFERENCE:  Sub-clause 4.1.1 GSM Recommendation 06.20
 *
 *    KEYWORDS: highpass filter, hp, HP, filter
 *
 *************************************************************************/

void   filt4_2nd(INT16 pswCoeff[], INT16 pswIn[],
                 INT16 pswXstate[], INT16 pswYstate[],
                 int npts, int shifts)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */
    VPP_HR_PROFILE_FUNCTION_ENTER( filt4_2nd);
    /* Do first second order section */
    /*-------------------------------*/

    iir_d(&pswCoeff[0],pswIn,&pswXstate[0],&pswYstate[0],npts,shifts,1,0);


    /* Do second second order section */
    /*--------------------------------*/

    iir_d(&pswCoeff[5],pswIn,&pswXstate[2],&pswYstate[4],npts,shifts,0,1);
    VPP_HR_PROFILE_FUNCTION_EXIT( filt4_2nd);

}

/***************************************************************************
 *
 *   FUNCTION NAME: findBestInQuantList
 *
 *   PURPOSE:
 *     Given a list of quantizer vectors and their associated prediction
 *     errors, search the list for the iNumVectOut vectors and output them
 *     as a new list.
 *
 *   INPUTS: psqlInList, iNumVectOut
 *
 *   OUTPUTS: psqlBestOutList
 *
 *   RETURN VALUE: none
 *
 *   DESCRIPTION:
 *
 *     The AFLAT recursion yields prediction errors.  This routine finds
 *     the lowest candidate is the AFLAT recursion outputs.
 *
 *
 *   KEYWORDS: best quantlist find
 *
 *   REFERENCE:  Sub-clause 4.1.4.1 GSM Recommendation 06.20
 *
 *************************************************************************/

void findBestInQuantList(struct QuantList psqlInList,
                         int iNumVectOut,
                         struct QuantList psqlBestOutList[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    int    quantIndex,
           bstIndex,
           i;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* initialize the best list */
    /* invalidate, ensure they will be dropped */
    for (bstIndex = 0; bstIndex < iNumVectOut; bstIndex++)
    {
        psqlBestOutList[bstIndex].iNum = 1;
        psqlBestOutList[bstIndex].iRCIndex = psqlInList.iRCIndex;
        psqlBestOutList[bstIndex].pswPredErr[0] = 0x7fff;
    }

    /* best list elements replaced in the order:  0,1,2,3... challenger must
     * be < (not <= ) current best */
    for (quantIndex = 0; quantIndex < psqlInList.iNum; quantIndex++)
    {
        bstIndex = 0;
        /*while (sub(psqlInList.pswPredErr[quantIndex],
                   psqlBestOutList[bstIndex].pswPredErr[0]) >= 0 &&
               bstIndex < iNumVectOut)*/
        while ((psqlInList.pswPredErr[quantIndex] >= psqlBestOutList[bstIndex].pswPredErr[0])  &&
                (bstIndex < iNumVectOut))
        {
            bstIndex++;                      /* only increments to next upon
                                        * failure to beat "best" */
        }

        if (bstIndex < iNumVectOut)
        {
            /* a new value is found */
            /* now add challenger to best list at index bstIndex */
            for (i = iNumVectOut - 1; i > bstIndex; i--)
            {
                psqlBestOutList[i].pswPredErr[0] =
                    psqlBestOutList[i - 1].pswPredErr[0];
                psqlBestOutList[i].iRCIndex =
                    psqlBestOutList[i - 1].iRCIndex;
            }
            /* get new best value and place in list */
            psqlBestOutList[bstIndex].pswPredErr[0] =
                psqlInList.pswPredErr[quantIndex];
            psqlBestOutList[bstIndex].iRCIndex =
                psqlInList.iRCIndex + quantIndex;
        }
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: findPeak
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to return the lag
 *     that maximizes CC/G within +- PEAK_VICINITY of the
 *     input lag.  The input lag is an integer lag, and
 *     the search for a peak is done on the surrounding
 *     integer lags.
 *
 *   INPUTS:
 *
 *     swSingleResLag
 *
 *                     Input integer lag, expressed as lag * OS_FCTR
 *
 *     pswCIn[0:127]
 *
 *                     C(k) sequence, k an integer
 *
 *     pswGIn[0:127]
 *
 *                     G(k) sequence, k an integer
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     Integer lag where peak was found, or zero if no peak was found.
 *     The lag is expressed as lag * OS_FCTR
 *
 *   DESCRIPTION:
 *
 *     This routine is called from pitchLags(), and is used to do the
 *     interpolating CC/G peak search.  This is used in a number of
 *     places in pitchLags().  See description 5.3.1.
 *
 *   REFERENCE:  Sub-clause 4.1.8.2 of GSM Recommendation 06.20
 *
 *   KEYWORDS:
 *
 *************************************************************************/

INT16 findPeak(INT16 swSingleResLag,
               INT16 pswCIn[],
               INT16 pswGIn[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swCmaxSqr,
          swGmax,
          swFullResPeak;
    short int siUpperBound,
          siLowerBound,
          siRange,
          siPeak;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* get upper and lower bounds for integer lags for peak search */
    /* ----------------------------------------------------------- */

    //siUpperBound = ADD_SAT16(swSingleResLag, PEAK_VICINITY + 1);
    siUpperBound = ADD(swSingleResLag, (PEAK_VICINITY + 1));
    //if (sub(siUpperBound, LMAX + 1) > 0)
    if ( siUpperBound > (LMAX + 1)  )
    {
        siUpperBound = LMAX + 1;
    }

    //siLowerBound = sub(swSingleResLag, PEAK_VICINITY + 1);
    siLowerBound = SUB(swSingleResLag, (PEAK_VICINITY + 1));//(swSingleResLag- (PEAK_VICINITY + 1));//

    if ( siLowerBound < LMIN - 1 )
    {
        siLowerBound = LMIN - 1;
    }

    siRange = SUB(siUpperBound, siLowerBound);
    //siRange = ADD_SAT16(siRange, 1);
    siRange++;

    /* do peak search */
    /* -------------- */

    swCmaxSqr = 0;
    swGmax = 0x3f;

    siPeak = fnBest_CG(&pswCIn[siLowerBound - LSMIN],
                       &pswGIn[siLowerBound - LSMIN], &swCmaxSqr, &swGmax,
                       siRange);

    /* if no max found, flag no peak */
    /* ----------------------------- */

    //if (ADD_SAT16(siPeak, 1) == 0)
    if ((siPeak + 1) == 0)
    {
        swFullResPeak = 0;
    }

    /* determine peak location      */
    /* if at boundary, flag no peak */
    /* else return lag at peak      */
    /* ---------------------------- */

    else
    {
        //siPeak = ADD_SAT16(siPeak, siLowerBound);
        siPeak = ADD(siPeak, siLowerBound);

        if (  (siPeak == siLowerBound)    ||   (siPeak == siUpperBound)   )
        {
            swFullResPeak = 0;
        }
        else
        {
            swFullResPeak = SHR(EXTRACT_L(L_MULT(siPeak, OS_FCTR)), 1);
        }
    }
    return (swFullResPeak);
}

/***************************************************************************
 *
 *    FUNCTION NAME: flat
 *
 *    PURPOSE:  Computes the unquantized reflection coefficients from the
 *              input speech using the FLAT algorithm. Also computes the
 *              frame energy, and the index of the element in the R0
 *              quantization table which best represents the frame energy.
 *              Calls function cov32 which computes the F, B, and C
 *              correlation arrays, required by the FLAT algorithm to
 *              compute the reflection coefficients.
 *
 *    INPUT:
 *
 *       pswSpeechIn[0:169]
 *                     A sampled speech vector used to compute
 *                     correlations need for generating the optimal
 *                     reflection coefficients via the FLAT algorithm.
 *
 *    OUTPUTS:
 *
 *       pswRc[NP]     An array of unquantized reflection coefficients.
 *
 *       *piR0Inx      An index of the quantized frame energy value.
 *
 *       INT32 pL_VadAcf[4]
 *                     An array with the autocorrelation coefficients to be
 *                     used by the VAD.  Generated by cov16(), a daughter
 *                     function of flat().
 *
 *       INT16 *pswVadScalAuto
 *                     Input scaling factor used by the VAD.
 *                     Generated by cov16(), a daughter function of flat().
 *                     function.
 *
 *    RETURN:          L_R0 normalized frame energy value, required in DTX
 *                     mode.
 *
 *   DESCRIPTION:
 *
 *    An efficient Fixed point LAtice Technique (FLAT) is used to compute
 *    the reflection coefficients, given B, F, and C arrays returned by
 *    function cov32. B, F, and C are backward, forward, and cross
 *    correlations computed from the input speech. The correlations
 *    are spectrally smoothed in cov32.
 *
 *
 *   REFERENCE:  Sub-clause 4.1.3 of GSM Recommendation 06.20
 *
 *   keywords: LPC, FLAT, reflection coefficients, covariance, correlation,
 *   keywords: spectrum, energy, R0, spectral smoothing, SST
 *
 *************************************************************************/

INT32   flat(INT16 pswSpeechIn[],
             INT16 pswRc[],
             int *piR0Inx,
             INT32 pL_VadAcf[],
             INT16 *pswVadScalAuto)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16
    swNum,
    swDen,
    swRcSq,
    swSqrtOut,
    swRShifts,
    swShift,
    swShift1;
    INT32
    pppL_F[NP][NP][2],
           pppL_B[NP][NP][2],
           pppL_C[NP][NP][2],
           L_Num,
           L_TmpA,
           L_TmpB,
           L_temp,
           L_sum,
           L_R0,
           L_Fik,
           L_Bik,
           L_Cik,
           L_Cki;
    short int i,
          j,
          k,
          l,
          j_0,
          j_1;


    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/


    VPP_HR_PROFILE_FUNCTION_ENTER(flat );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Compute from the input speech the elements of the B, F, and C     */
    /* arrays, which form the initial conditions for the FLAT algorithm. */
    /*-------------------------------------------------------------------*/

    swRShifts = cov32(pswSpeechIn, pppL_B, pppL_F, pppL_C, &L_R0,
                      pL_VadAcf, pswVadScalAuto);

    /* Compute the intermediate quantities required by the R0 quantizer */
    /*------------------------------------------------------------------*/

    if (L_R0 != 0)
    {
        swSqrtOut = sqroot(L_R0);          /* If L_R0 > 0, compute sqrt */
    }
    else
    {
        swSqrtOut = 0;                     /* L_R0 = 0, initialize sqrt(0) */
    }

    swRShifts = SUB((S_SH + 2), swRShifts);

    /* If odd number of shifts compensate by sqrt(0.5) */
    /*-------------------------------------------------*/

    if (swRShifts & 1)
    {
        L_temp = L_MULT(swSqrtOut, 0x5a82);
    }
    else
    {
        L_temp = L_DEPOSIT_H(swSqrtOut);
    }
    //swRShifts = shr(swRShifts, 1);
    swRShifts = SHR(swRShifts, 1);

    if (swRShifts > 0)
    {
        //L_temp = L_shr(L_temp, swRShifts);
        L_temp = SHR(L_temp, swRShifts);

    }
    else if (swRShifts < 0)
    {
        L_temp = 0;
    }

    /* Given average energy L_temp, find the index in the R0 quantization */
    /* table which best represents it.                                    */
    /*--------------------------------------------------------------------*/

    *piR0Inx = r0Quant(L_temp);

    L_R0 = L_temp; /* save the unquantized R0 */             /* DTX mode */

    /* Zero out the number of left-shifts to be applied to the  */
    /* F, B, and C matrices.                                    */
    /*----------------------------------------------------------*/

    swShift = 0;

    /* Now compute the NP reflection coefficients  */
    /*---------------------------------------------*/

    for (j = 0; j < NP; j++)
    {

        /* Initialize the modulo indices of the third dimension of arrays  */
        /* F, B, and C, where indices j_0 and j_1 point to:                */
        /* */
        /* j_0 = points to F, B, and C matrix values at stage j-0, which   */
        /* is the current  lattice stage.                                  */
        /* j_1 = points to F, B, and C matrix values at stage j-1, which   */
        /* is the previous lattice stage.                                  */
        /* */
        /* Use of modulo address arithmetic permits to swap values of j_0 and */
        /* and j_1 at each lattice stage, thus eliminating the need to copy   */
        /* the current elements of F, B, and C arrays, into the F, B, and C   */
        /* arrays corresponding to the previous lattice stage, prior to       */
        /* incrementing j, the index of the lattice filter stage.             */
        /*--------------------------------------------------------------------*/

        j_0 = (j + 1) % 2;
        j_1 = j % 2;

        /* Get the numerator for computing the j-th reflection coefficient */
        /*-----------------------------------------------------------------*/

        //L_Num = L_ADD(L_shl(pppL_C[0][0][j_1], swShift), L_shl(pppL_C[NP - j - 1][NP - j - 1][j_1], swShift));
        //L_Num = L_ADD(SHR_V(pppL_C[0][0][j_1], swShift), SHR_V(pppL_C[NP - j - 1][NP - j - 1][j_1], swShift));
        L_Num = ADD(SHR_V(pppL_C[0][0][j_1], swShift), SHR_V(pppL_C[NP - j - 1][NP - j - 1][j_1], swShift));



        /* Get the denominator for computing the j-th reflection coefficient */
        /*-------------------------------------------------------------------*/

        //L_temp = L_ADD(L_shl(pppL_F[0][0][j_1], swShift), L_shl(pppL_B[0][0][j_1], swShift));
        //L_TmpA = L_ADD(L_shl(pppL_F[NP - j - 1][NP - j - 1][j_1], swShift), L_shl(pppL_B[NP - j - 1][NP - j - 1][j_1], swShift));
        //L_temp = L_ADD(L_SHL_SAT(pppL_F[0][0][j_1], swShift), L_SHL_SAT(pppL_B[0][0][j_1], swShift));
        //L_TmpA = L_ADD(L_SHL_SAT(pppL_F[NP - j - 1][NP - j - 1][j_1], swShift), L_SHL_SAT(pppL_B[NP - j - 1][NP - j - 1][j_1], swShift));
        /*L_temp = L_ADD(SHL(pppL_F[0][0][j_1], swShift),  SHL (pppL_B[0][0][j_1], swShift));
        L_TmpA = L_ADD(SHL(pppL_F[NP - j - 1][NP - j - 1][j_1], swShift),  SHL (pppL_B[NP - j - 1][NP - j - 1][j_1], swShift));
        L_sum = L_ADD(L_TmpA, L_temp);*/
        L_temp =  ADD(SHL(pppL_F[0][0][j_1], swShift),  SHL (pppL_B[0][0][j_1], swShift));
        L_TmpA =  ADD(SHL(pppL_F[NP - j - 1][NP - j - 1][j_1], swShift),  SHL (pppL_B[NP - j - 1][NP - j - 1][j_1], swShift));
        L_sum  =  ADD(L_TmpA, L_temp);
        //L_sum = L_shr(L_sum, 1);
        L_sum >>= 1;

        /* Normalize the numerator and the denominator terms */
        /*---------------------------------------------------*/

        swShift1 = norm_s(EXTRACT_H(L_sum));

        //L_sum = L_shl(L_sum, swShift1);
        //L_Num = L_shl(L_Num, swShift1);
        //L_sum = L_SHL_SAT(L_sum, swShift1);
        //L_Num = L_SHL_SAT(L_Num, swShift1);
        L_sum <<= swShift1 ;
        L_Num <<= swShift1;

        swNum = ROUNDO(L_Num);
        swDen = ROUNDO(L_sum);

        if (swDen <= 0)
        {

            /* Zero prediction error at the j-th lattice stage, zero */
            /* out remaining reflection coefficients and return.     */
            /*-------------------------------------------------------*/

            for (i = j; i < NP; i++)
            {
                pswRc[i] = 0;
            }

            VPP_HR_PROFILE_FUNCTION_EXIT(flat );

            return (L_R0);
        }
        else
        {

            /* Non-zero prediction error, check if the j-th reflection
             * coefficient */
            /* about to be computed is stable.                           */
            /*-----------------------------------------------------------*/

            //if (sub(abs_s(swNum), swDen) >= 0)
            if ( ABS_S(swNum) >= swDen )
            {

                /* Reflection coefficient at j-th lattice stage unstable, so zero  */
                /* out reflection coefficients for lattice stages i=j,...,NP-1, and */
                /* return.                                                         */
                /*-----------------------------------------------------------------*/

                for (i = j; i < NP; i++)
                {
                    pswRc[i] = 0;
                }

                VPP_HR_PROFILE_FUNCTION_EXIT(flat );

                return (L_R0);
            }
            else
            {

                /* j-th reflection coefficient is stable, compute it. */
                /*----------------------------------------------------*/

                if (swNum < 0)
                {

                    swNum = NEGATE(swNum);
                    //pswRc[j] = divide_s(swNum, swDen);
                    pswRc[j] = DIVIDE_SAT16(swNum, swDen);
                }
                else
                {

                    //pswRc[j] = divide_s(swNum, swDen);
                    pswRc[j] = DIVIDE_SAT16(swNum, swDen);
                    pswRc[j] = NEGATE(pswRc[j]);

                }                              /* j-th reflection coefficient
                                        * sucessfully computed. */
                /*----------------------------------------------------*/


            }                                /* End of reflection coefficient
                                        * stability test (and computation) */
            /*------------------------------------------------------------------*/

        }                                  /* End of non-zero prediction error
                                        * case */
        /*----------------------------------------*/



        /* If not at the last lattice stage, update F, B, and C arrays */
        /*-------------------------------------------------------------*/

        if (j != NP - 1)
        {

            /* Compute squared Rc[j] */
            /*-----------------------*/

            swRcSq = MULT_R(pswRc[j], pswRc[j]);
            /*VPP_MULT_R(m_hi, m_lo, pswRc[j], pswRc[j]);
            swRcSq = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/

            i = 0;
            k = 0;

            /* Compute the common terms used by the FLAT recursion to reduce */
            /* computation.                                                  */
            /*---------------------------------------------------------------*/

            //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
            //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
            L_Cik =  SHL (pppL_C[i][k][j_1], swShift);

            //L_TmpA = L_ADD(L_Cik, L_Cik);
            L_TmpA = ADD(L_Cik, L_Cik);
            L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

            /* Update the F array */
            /*--------------------*/

            //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
            //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
            //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
            //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
            L_Fik = SHL(pppL_F[i][k][j_1], swShift);
            L_Bik = SHL(pppL_B[i][k][j_1], swShift);

            L_temp = L_mpy_ls(L_Bik, swRcSq);
            /*L_temp = L_ADD(L_temp, L_Fik);
            pppL_F[i][k][j_0] = L_ADD(L_temp, L_TmpA);*/
            L_temp =  ADD(L_temp, L_Fik);
            pppL_F[i][k][j_0] =  ADD(L_temp, L_TmpA);

            for (k = i + 1; k <= NP - j - 2; k++)
            {

                /* Compute the common terms used by the FLAT recursion to reduce */
                /* computation.                                                  */
                /*---------------------------------------------------------------*/

                //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
                //L_Cki = L_shl(pppL_C[k][i][j_1], swShift);
                //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
                //L_Cki = L_SHL_SAT(pppL_C[k][i][j_1], swShift);
                L_Cik = SHL(pppL_C[i][k][j_1], swShift);
                L_Cki = SHL(pppL_C[k][i][j_1], swShift);

                //L_TmpA = L_ADD(L_Cik, L_Cki);
                L_TmpA =  ADD(L_Cik, L_Cki);
                L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

                //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
                //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
                L_Bik = SHL(pppL_B[i][k][j_1], swShift);
                L_Fik = SHL(pppL_F[i][k][j_1], swShift);

                //L_TmpB = L_ADD(L_Bik, L_Fik);
                L_TmpB =  ADD(L_Bik, L_Fik);
                L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);

                /* Update the F and C arrays */
                /*---------------------------------*/

                L_temp = L_mpy_ls(L_Bik, swRcSq);
                //L_temp = L_ADD(L_temp, L_Fik);
                L_temp =  ADD(L_temp, L_Fik);
                pppL_F[i][k][j_0] = L_ADD(L_temp, L_TmpA);

                L_temp = L_mpy_ls(L_Cki, swRcSq);
                //L_temp = L_ADD(L_temp, L_Cik);
                L_temp = ADD(L_temp, L_Cik);
                pppL_C[i][k - 1][j_0] = L_ADD(L_temp, L_TmpB);

            }

            k = NP - j - 1;

            /* Compute the common terms used by the FLAT recursion to reduce */
            /* computation.                                                  */
            /*---------------------------------------------------------------*/

            //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
            //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
            //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
            //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
            L_Bik = SHL(pppL_B[i][k][j_1], swShift);
            L_Fik = SHL(pppL_F[i][k][j_1], swShift);

            //L_TmpB = L_ADD(L_Bik, L_Fik);
            L_TmpB =  ADD(L_Bik, L_Fik);
            L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);

            /* Update the C array */
            /*-----------------------*/

            //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
            //L_Cki = L_shl(pppL_C[k][i][j_1], swShift);
            //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
            //L_Cki = L_SHL_SAT(pppL_C[k][i][j_1], swShift);
            L_Cik = SHL(pppL_C[i][k][j_1], swShift);
            L_Cki = SHL(pppL_C[k][i][j_1], swShift);

            L_temp = L_mpy_ls(L_Cki, swRcSq);
            //L_temp = L_ADD(L_temp, L_Cik);
            L_temp =  ADD(L_temp, L_Cik);
            pppL_C[i][k - 1][j_0] = L_ADD(L_temp, L_TmpB);


            for (i = 1; i <= NP - j - 2; i++)
            {

                k = i;

                /* Compute the common terms used by the FLAT recursion to reduce */
                /* computation.                                                  */
                /*---------------------------------------------------------------*/

                //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
                //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
                L_Cik =  SHL (pppL_C[i][k][j_1], swShift);

                //L_TmpA = L_ADD(L_Cik, L_Cik);
                L_TmpA =  ADD(L_Cik, L_Cik);
                L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

                //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
                //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
                L_Bik = SHL(pppL_B[i][k][j_1], swShift);
                L_Fik = SHL(pppL_F[i][k][j_1], swShift);

                //L_TmpB = L_ADD(L_Bik, L_Fik);
                L_TmpB =  ADD(L_Bik, L_Fik);
                L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);

                /* Update F, B and C arrays */
                /*-----------------------------------*/

                L_temp = L_mpy_ls(L_Bik, swRcSq);
                //L_temp = L_ADD(L_temp, L_Fik);
                L_temp =  ADD(L_temp, L_Fik);
                //pppL_F[i][k][j_0] = L_ADD(L_temp, L_TmpA);
                pppL_F[i][k][j_0] = ADD(L_temp, L_TmpA);

                L_temp = L_mpy_ls(L_Fik, swRcSq);
                //L_temp = L_ADD(L_temp, L_Bik);
                L_temp =  ADD(L_temp, L_Bik);
                //pppL_B[i - 1][k - 1][j_0] = L_ADD(L_temp, L_TmpA);
                pppL_B[i - 1][k - 1][j_0] =  ADD(L_temp, L_TmpA);

                L_temp = L_mpy_ls(L_Cik, swRcSq);
                //L_temp = L_ADD(L_temp, L_Cik);
                L_temp =  ADD(L_temp, L_Cik);
                //pppL_C[i][k - 1][j_0] = L_ADD(L_temp, L_TmpB);
                pppL_C[i][k - 1][j_0] = ADD(L_temp, L_TmpB);

                for (k = i + 1; k <= NP - j - 2; k++)
                {

                    /* Compute the common terms used by the FLAT recursion to reduce */
                    /* computation.                                                  */
                    /*---------------------------------------------------------------*/

                    //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
                    //L_Cki = L_shl(pppL_C[k][i][j_1], swShift);
                    //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
                    //L_Cki = L_SHL_SAT(pppL_C[k][i][j_1], swShift);
                    L_Cik = SHL(pppL_C[i][k][j_1], swShift);
                    L_Cki = SHL(pppL_C[k][i][j_1], swShift);

                    //L_TmpA = L_ADD(L_Cik, L_Cki);
                    L_TmpA = ADD(L_Cik, L_Cki);
                    L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

                    //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
                    //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
                    //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
                    //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
                    L_Bik = SHL(pppL_B[i][k][j_1], swShift);
                    L_Fik = SHL(pppL_F[i][k][j_1], swShift);

                    //L_TmpB = L_ADD(L_Bik, L_Fik);
                    L_TmpB = ADD(L_Bik, L_Fik);
                    L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);

                    /* Update F, B and C arrays */
                    /*-----------------------------------*/

                    L_temp = L_mpy_ls(L_Bik, swRcSq);
                    //L_temp = L_ADD(L_temp, L_Fik);
                    L_temp = ADD(L_temp, L_Fik);
                    pppL_F[i][k][j_0] = L_ADD(L_temp, L_TmpA);

                    L_temp = L_mpy_ls(L_Fik, swRcSq);
                    //L_temp = L_ADD(L_temp, L_Bik);
                    L_temp = ADD(L_temp, L_Bik);
                    pppL_B[i - 1][k - 1][j_0] = L_ADD(L_temp, L_TmpA);

                    L_temp = L_mpy_ls(L_Cki, swRcSq);
                    //L_temp = L_ADD(L_temp, L_Cik);
                    L_temp = ADD(L_temp, L_Cik);
                    //pppL_C[i][k - 1][j_0] = L_ADD(L_temp, L_TmpB);
                    pppL_C[i][k - 1][j_0] = ADD(L_temp, L_TmpB);

                    L_temp = L_mpy_ls(L_Cik, swRcSq);
                    //L_temp = L_ADD(L_temp, L_Cki);
                    L_temp = ADD(L_temp, L_Cki);
                    //pppL_C[k][i - 1][j_0] = L_ADD(L_temp, L_TmpB);
                    pppL_C[k][i - 1][j_0] = ADD(L_temp, L_TmpB);

                }                              /* end of loop indexed by k */
                /*---------------------------*/

                k = NP - j - 1;

                /* Compute the common terms used by the FLAT recursion to reduce */
                /* computation.                                                  */
                /*---------------------------------------------------------------*/

                //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
                //L_Cki = L_shl(pppL_C[k][i][j_1], swShift);
                //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
                //L_Cki = L_SHL_SAT(pppL_C[k][i][j_1], swShift);
                L_Cik = SHL(pppL_C[i][k][j_1], swShift);
                L_Cki = SHL(pppL_C[k][i][j_1], swShift);

                //L_TmpA = L_ADD(L_Cik, L_Cki);
                L_TmpA =  ADD(L_Cik, L_Cki);
                L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

                //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
                //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
                L_Bik = SHL(pppL_B[i][k][j_1], swShift);
                L_Fik = SHL(pppL_F[i][k][j_1], swShift);

                //L_TmpB = L_ADD(L_Bik, L_Fik);
                L_TmpB =  ADD(L_Bik, L_Fik);
                L_TmpB = L_mpy_ls(L_TmpB, pswRc[j]);

                /* Update B and C arrays */
                /*-----------------------------------*/

                L_temp = L_mpy_ls(L_Fik, swRcSq);
                //L_temp = L_ADD(L_temp, L_Bik);
                //pppL_B[i - 1][k - 1][j_0] = L_ADD(L_temp, L_TmpA);
                L_temp =  ADD(L_temp, L_Bik);
                pppL_B[i - 1][k - 1][j_0] =  ADD(L_temp, L_TmpA);

                L_temp = L_mpy_ls(L_Cki, swRcSq);
                //L_temp = L_ADD(L_temp, L_Cik);
                //pppL_C[i][k - 1][j_0] = L_ADD(L_temp, L_TmpB);
                L_temp =  ADD(L_temp, L_Cik);
                pppL_C[i][k - 1][j_0] =  ADD(L_temp, L_TmpB);

            }                                /* end of loop indexed by i */
            /*---------------------------*/

            i = NP - j - 1;
            for (k = i; k <= NP - j - 1; k++)
            {

                /* Compute the common terms used by the FLAT recursion to reduce */
                /* computation.                                                  */
                /*---------------------------------------------------------------*/

                //L_Cik = L_shl(pppL_C[i][k][j_1], swShift);
                //L_Cik = L_SHL_SAT(pppL_C[i][k][j_1], swShift);
                L_Cik =  SHL (pppL_C[i][k][j_1], swShift);

                //L_TmpA = L_ADD(L_Cik, L_Cik);
                L_TmpA = ADD(L_Cik, L_Cik);
                L_TmpA = L_mpy_ls(L_TmpA, pswRc[j]);

                /* Update B array */
                /*-----------------------------------*/

                //L_Bik = L_shl(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_shl(pppL_F[i][k][j_1], swShift);
                //L_Bik = L_SHL_SAT(pppL_B[i][k][j_1], swShift);
                //L_Fik = L_SHL_SAT(pppL_F[i][k][j_1], swShift);
                L_Bik = SHL(pppL_B[i][k][j_1], swShift);
                L_Fik = SHL(pppL_F[i][k][j_1], swShift);

                L_temp = L_mpy_ls(L_Fik, swRcSq);
                //L_temp = L_ADD(L_temp, L_Bik);
                L_temp = ADD(L_temp, L_Bik);
                //pppL_B[i - 1][k - 1][j_0] = L_ADD(L_temp, L_TmpA);
                pppL_B[i - 1][k - 1][j_0] = ADD(L_temp, L_TmpA);

            }                                /* end of loop indexed by k */
            /*-----------------------------------------------------------*/

            /* OR the F and B matrix diagonals to find maximum for normalization */
            /*********************************************************************/

            L_TmpA = 0;
            for (l = 0; l <= NP - j - 2; l++)
            {
                L_TmpA |= pppL_F[l][l][j_0];
                L_TmpA |= pppL_B[l][l][j_0];
            }
            /* Compute the shift count to be applied to F, B, and C arrays */
            /* at the next lattice stage.                                  */
            /*-------------------------------------------------------------*/

            if (L_TmpA > 0)
            {
                swShift = norm_l(L_TmpA);
                swShift -=CVSHIFT;// sub(swShift, CVSHIFT);
            }
            else
            {
                swShift = 0;
            }

        }                                  /* End of update of F, B, and C
                                        * arrays for the next lattice stage */
        /*----------------------------------------------------------------*/

    }                                    /* Finished computation of
                                        * reflection coefficients */
    /*--------------------------------------------------------------*/

    VPP_HR_PROFILE_FUNCTION_EXIT(flat );

    return (L_R0);

}

/**************************************************************************
 *
 *   FUNCTION NAME: fnBest_CG
 *
 *   PURPOSE:
 *     The purpose of this function is to determine the C:G pair from the
 *     input arrays which maximize C*C/G
 *
 *   INPUTS:
 *
 *     pswCframe[0:siNumPairs]
 *
 *                     pointer to start of the C frame vector
 *
 *     pswGframe[0:siNumPairs]
 *
 *                     pointer to start of the G frame vector
 *
 *     pswCmaxSqr
 *
 *                     threshold Cmax**2 or 0 if no threshold
 *
 *     pswGmax
 *
 *                     threshold Gmax, must be > 0
 *
 *     siNumPairs
 *
 *                     number of C:G pairs to test
 *
 *   OUTPUTS:
 *
 *     pswCmaxSqr
 *
 *                     final Cmax**2 value
 *
 *     pswGmax
 *
 *                     final Gmax value
 *
 *   RETURN VALUE:
 *
 *     siMaxLoc
 *
 *                     index of Cmax in the input C matrix or -1 if none
 *
 *   DESCRIPTION:
 *
 *     test the result of (C * C * Gmax) - (Cmax**2 * G)
 *     if the result is > 0 then a new max has been found
 *     the Cmax**2, Gmax and MaxLoc parameters are all updated accordingly.
 *     if no new max is found for all NumPairs then MaxLoc will retain its
 *     original value
 *
 *   REFERENCE:  Sub-clause 4.1.8.1, 4.1.8.2, and 4.1.8.3 of GSM
 *     Recommendation 06.20
 *
 *   KEYWORDS: C_Frame, G_Frame, Cmax, Gmax, DELTA_LAGS, PITCH_LAGS
 *

****************************************************************************/

short int fnBest_CG(INT16 pswCframe[], INT16 pswGframe[],
                    INT16 *pswCmaxSqr, INT16 *pswGmax,
                    short int siNumPairs)
{

    /*_________________________________________________________________________
    |                                                                           |
    |                            Automatic Variables                            |
    |___________________________________________________________________________|
    */

    INT32 L_Temp2;
    INT16 swCmaxSqr,
          swGmax,
          swTemp;
    short int siLoopCnt,
          siMaxLoc;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    /*_________________________________________________________________________
    |                                                                           |
    |                              Executable Code                              |
    |___________________________________________________________________________|
    */

    /* initialize */
    /* ---------- */

    swCmaxSqr = *pswCmaxSqr;
    swGmax = *pswGmax;
    siMaxLoc = -1;

    for (siLoopCnt = 0; siLoopCnt < siNumPairs; siLoopCnt++)
    {

        /* make sure both C and energy > 0 */
        /* ------------------------------- */

        if ((pswGframe[siLoopCnt] > 0) && (pswCframe[siLoopCnt] > 0))
        {

            /* calculate (C * C) */
            /* ----------------- */

            swTemp = MULT_R(pswCframe[siLoopCnt], pswCframe[siLoopCnt]);
            /*VPP_MULT_R(m_hi, m_lo, pswCframe[siLoopCnt], pswCframe[siLoopCnt]);
            swTemp = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/

            /* calculate (C * C * Gmax) */
            /* ------------------------ */

            //L_Temp2 = L_MULT(swTemp, swGmax);
            //s_lo=SHR(L_Temp2,1);
            /* calculate (C * C * Gmax) - (Cmax**2 * G) */
            /* ----------------------------------------- */

            VPP_MLX16(s_hi, s_lo, swTemp, swGmax);
            //L_Temp2 = L_msu(L_Temp2, swCmaxSqr, pswGframe[siLoopCnt]);
            VPP_MLA16(s_hi, s_lo, -swCmaxSqr, pswGframe[siLoopCnt]);

            L_Temp2= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

            /* if new max found, update it and its location */
            /* -------------------------------------------- */

            if (L_Temp2 > 0)
            {
                swCmaxSqr = swTemp;            /* Cmax**2 = current C * C */
                swGmax = pswGframe[siLoopCnt]; /* Gmax */
                siMaxLoc = siLoopCnt;          /* max location = current (C)
                                        * location */
            }
        }
    }

    /* set output */
    /* ---------- */

    *pswCmaxSqr = swCmaxSqr;
    *pswGmax = swGmax;
    return (siMaxLoc);
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnExp2
 *
 *   PURPOSE:
 *     The purpose of this function is to implement a base two exponential
 *     2**(32*x) by polynomial approximation
 *
 *
 *   INPUTS:
 *
 *     L_Input
 *
 *                     unnormalized input exponent (input range constrained
 *                     to be < 0; for input < -0.46 output is 0)
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swTemp4
 *
 *                     exponential output
 *
 *   DESCRIPTION:
 *
 *     polynomial approximation is used for the generation of the exponential
 *
 *     2**(32*X) = 0.1713425*X*X + 0.6674432*X + 0.9979554
 *                     c2              c1            c0
 *
 *   REFERENCE:  Sub-clause 4.1.8.2 of GSM Recommendation 06.20, eqn 3.9
 *
 *   KEYWORDS: EXP2, DELTA_LAGS
 *
 *************************************************************************/

INT16 fnExp2(INT32 L_Input)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Local Static Variables                        |
     |_________________________________________________________________________|
    */
    static INT16 pswPCoefE[3] =
    {
        /* c2,   c1,    c0 */
        0x15ef, 0x556f, 0x7fbd
    };

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swTemp1,
          swTemp2,
          swTemp3,
          swTemp4;

    //INT32 Lmult;
    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* initialize */
    /* ---------- */

    swTemp3 = 0x0020;

    /* determine normlization shift count */
    /* ---------------------------------- */

    swTemp1 = EXTRACT_H(L_Input);
    L_Input = L_MULT(swTemp1, swTemp3);
    swTemp2 = EXTRACT_H(L_Input);

    /* determine un-normalized shift count */
    /* ----------------------------------- */

    swTemp3 = -0x0001;
    swTemp4 = SUB(swTemp3, swTemp2);

    /* normalize input */
    /* --------------- */

    L_Input = L_Input & LSP_MASK;
    //L_Input = L_ADD(L_Input, L_DEPOSIT_H(swTemp3));
    L_Input =  ADD(L_Input, L_DEPOSIT_H(swTemp3));

    //L_Input = L_shr(L_Input, 1);
    L_Input = SHR(L_Input, 1);
    swTemp1 = EXTRACT_L(L_Input);

    /* calculate x*x*c2 */
    /* ---------------- */

    swTemp2 = MULT_R(swTemp1, swTemp1);
    /*VPP_MULT_R(m_hi, m_lo, swTemp1, swTemp1);
    swTemp2 = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
    //L_Input = L_MULT(swTemp2, pswPCoefE[0]);

    /* calculate x*x*c2 + x*c1 */
    /* ----------------------- */
    //s_lo=SHR(L_Input,1);
    VPP_MLX16(s_hi, s_lo, swTemp2, pswPCoefE[0]);
    //L_Input = L_mac(L_Input, swTemp1, pswPCoefE[1]);
    /*Lmult=L_MULT(swTemp1, pswPCoefE[1]);
    L_Input=L_ADD(L_Input,Lmult);*/
    VPP_MLA16(s_hi, s_lo, swTemp1, pswPCoefE[1]);

    L_Input= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    /* calculate x*x*c2 + x*c1 + c0 */
    /* --------------------------- */

    //L_Input = L_ADD(L_Input, L_DEPOSIT_H(pswPCoefE[2]));
    L_Input =  ADD(L_Input, L_DEPOSIT_H(pswPCoefE[2]));

    /* un-normalize exponent if its requires it */
    /* ---------------------------------------- */

    if (swTemp4 > 0)
    {
        //L_Input = L_shr(L_Input, swTemp4);
        L_Input = SHR(L_Input, swTemp4);

    }

    /* return result */
    /* ------------- */

    swTemp4 = EXTRACT_H(L_Input);
    return (swTemp4);
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnLog2
 *
 *   PURPOSE:
 *     The purpose of this function is to take the log base 2 of input and
 *     divide by 32 and return; i.e. output = log2(input)/32
 *
 *   INPUTS:
 *
 *     L_Input
 *
 *                     input
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     INT16
 *
 *                     output
 *
 *   DESCRIPTION:
 *
 *     log2(x) = 4.0 * (-.3372223*x*x + .9981958*x -.6626105)
 *                           c0            c1          c2   (includes sign)
 *
 *   REFERENCE:  Sub-clause 4.1.8.2 of GSM Recommendation 06.20, eqn 3.9
 *
 *   KEYWORDS: log, logarithm, logbase2, fnLog2
 *
 *************************************************************************/

INT16 fnLog2(INT32 L_Input)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                           Static Variables                              |
     |_________________________________________________________________________|
    */

    static INT16
    swC0 = -0x2b2a,
    swC1 = 0x7fc5,
    swC2 = -0x54d0;

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    short int siShiftCnt;
    INT16 swInSqrd,
          swIn;


    //INT32 Lmult;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* normalize input and store shifts required */
    /* ----------------------------------------- */

    siShiftCnt = norm_l(L_Input);
    //L_Input = L_shl(L_Input, siShiftCnt);
    //L_Input = L_SHL_SAT(L_Input, siShiftCnt);
    L_Input <<= siShiftCnt ;
    //siShiftCnt = ADD_SAT16(siShiftCnt, 1);
    siShiftCnt++;
    siShiftCnt = NEGATE(siShiftCnt);

    /* calculate x*x*c0 */
    /* ---------------- */

    swIn = EXTRACT_H(L_Input);
    swInSqrd = MULT_R(swIn, swIn);
    //L_Input = L_MULT(swInSqrd, swC0);
    /*VPP_MULT_R(m_hi, m_lo, swIn, swIn);
    swInSqrd = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/

    /* add x*c1 */
    /* --------- */

    //s_lo=SHR(L_Input,1);
    VPP_MLX16(s_hi, s_lo, swInSqrd, swC0);
    //L_Input = L_mac(L_Input, swIn, swC1);
    /*Lmult=L_MULT( swIn, swC1);
    L_Input=L_ADD(L_Input,Lmult);*/
    VPP_MLA16(s_hi, s_lo,  swIn, swC1);

    L_Input= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

    /* add c2 */
    /* ------ */

    //L_Input = L_ADD(L_Input, L_DEPOSIT_H(swC2));
    L_Input =  ADD(L_Input, L_DEPOSIT_H(swC2));

    /* apply *(4/32) */
    /* ------------- */

    //L_Input = L_shr(L_Input, 3);
    L_Input = SHR(L_Input, 3);
    L_Input = L_Input & 0x03ffffff;
    siShiftCnt = SHL(siShiftCnt, 10);
    //L_Input = L_ADD(L_Input, L_DEPOSIT_H(siShiftCnt));
    L_Input =  ADD(L_Input, L_DEPOSIT_H(siShiftCnt));

    /* return log */
    /* ---------- */

    return (ROUNDO(L_Input));
}

/***************************************************************************
 *
 *   FUNCTION NAME: getCCThreshold
 *
 *   PURPOSE:
 *     The purpose of this function is to calculate a threshold for other
 *     correlations (subject to limits), given subframe energy (Rp0),
 *     correlation squared (CC), and energy of delayed sequence (G)
 *
 *   INPUTS:
 *
 *     swRp0
 *
 *                     energy of the subframe
 *
 *     swCC
 *
 *                     correlation (squared) of subframe and delayed sequence
 *
 *     swG
 *
 *                     energy of delayed sequence
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swCCThreshold
 *
 *                     correlation (squared) threshold
 *
 *   DESCRIPTION:
 *
 *     CCt/0.5 = R - R(antilog(SCALE*log(max(CLAMP,(RG-CC)/RG))))
 *
 *     The threshold CCt is then applied with an understood value of Gt = 0.5
 *
 *   REFERENCE:  Sub-clause 4.1.8.2 of GSM Recommendation 06.20, eqn 3.9
 *
 *   KEYWORDS: getCCThreshold, getccthreshold, GET_CSQ_THRES
 *
 *************************************************************************/

INT16 getCCThreshold(INT16 swRp0,
                     INT16 swCC,
                     INT16 swG)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swPGainClamp,
          swPGainScale,
          sw1;
    INT32 L_1;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;
    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* load CLAMP and SCALE */
    /* -------------------- */

    swPGainClamp = PGAIN_CLAMP;
    swPGainScale = PGAIN_SCALE;

    /* calculate RG-CC */
    /* --------------- */

    L_1 = L_MULT(swRp0, swG);
    sw1 = EXTRACT_H(L_1);
    //L_1 = L_SUB(L_1, (L_DEPOSIT_H(swCC)));
    L_1 -=  (L_DEPOSIT_H(swCC)) ;

    /* if RG - CC > 0 do max(CLAMP, (RG-CC)/RG) */
    /* ---------------------------------------- */

    if (L_1 > 0)
    {
        //sw1 = divide_s(EXTRACT_H(L_1), sw1);
        sw1 = DIVIDE_SAT16(EXTRACT_H(L_1), sw1);

        L_1 = L_DEPOSIT_H(sw1);

        if ( sw1 <= swPGainClamp )
        {
            L_1 = L_DEPOSIT_H(swPGainClamp);
        }
    }
    /* else max(CLAMP, (RG-CC)/RG) is CLAMP */
    /* ------------------------------------ */

    else
    {
        L_1 = L_DEPOSIT_H(swPGainClamp);
    }

    /* L_1 holds max(CLAMP, (RG-CC)/RG)   */
    /* do antilog( SCALE * log( max() ) ) */
    /* ---------------------------------- */

    sw1 = fnLog2(L_1);

    L_1 = L_MULT(sw1, swPGainScale);

    sw1 = fnExp2(L_1);


    /* do R - (R * antilog()) */
    /* ---------------------- */

    L_1 = L_DEPOSIT_H(swRp0);

    s_lo=SHR(L_1,1);
    //L_1 = L_msu(L_1, swRp0, sw1);
    VPP_MLA16(s_hi, s_lo,  -swRp0, sw1);

    L_1= L_MLA_SAT_CARRY(s_lo, L_1&1);// L_MLA_SAT(s_hi, s_lo);

    /* apply Gt value */
    /* -------------- */

    //L_1 = L_shr(L_1, 1);
    L_1 = SHR(L_1, 1);

    return (EXTRACT_H(L_1));
}


/***************************************************************************
 *
 *   FUNCTION NAME: getNWCoefs
 *
 *   PURPOSE:
 *
 *     Obtains best all-pole fit to various noise weighting
 *     filter combinations
 *
 *   INPUTS:
 *
 *     pswACoefs[0:9] - A(z) coefficient array
 *     psrNWCoefs[0:9] - filter smoothing coefficients
 *
 *   OUTPUTS:
 *
 *     pswHCoefs[0:9] - H(z) coefficient array
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   DESCRIPTION:
 *
 *     The function getNWCoefs() derives the spectral noise weighting
 *     coefficients W(z)and H(z).  W(z) and H(z) actually consist of
 *     three filters in cascade.  To avoid having such a complicated
 *     filter required for weighting, the filters are reduced to a
 *     single filter.
 *
 *     This is accomplished by passing an impulse through the cascased
 *     filters.  The impulse response of the filters is used to generate
 *     autocorrelation coefficients, which are then are transformed into
 *     a single direct form estimate of W(z) and H(z).  This estimate is
 *     called HHat(z) in the documentation.
 *
 *
 *   REFERENCE:  Sub-clause 4.1.7 of GSM Recommendation 06.20
 *
 *   KEYWORDS: spectral noise weighting, direct form coefficients
 *   KEYWORDS: getNWCoefs
 *
 *************************************************************************/

void   getNWCoefs(INT16 pswACoefs[],
                  INT16 pswHCoefs[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswCoefTmp2[NP],
          pswCoefTmp3[NP],
          pswVecTmp[S_LEN],
          pswVecTmp2[S_LEN],
          pswTempRc[NP];
    INT16 swNormShift,
          iLoopCnt,
          iLoopCnt2;
    INT32 pL_AutoCorTmp[NP + 1],
          L_Temp;//, Lmult
    short int siNum,
          k;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/
    //INT16 essai,temp;

    VPP_HR_PROFILE_FUNCTION_ENTER(getNWCoefs );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Calculate smoothing parameters for all-zero filter */
    /* -------------------------------------------------- */

    for (iLoopCnt = 0; iLoopCnt < NP; iLoopCnt++)
    {
        pswCoefTmp2[iLoopCnt] = MULT_R(psrNWCoefs[iLoopCnt], pswACoefs[iLoopCnt]);
        /*VPP_MULT_R(m_hi, m_lo, psrNWCoefs[iLoopCnt], pswACoefs[iLoopCnt]);
        pswCoefTmp2[iLoopCnt] = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
    }

    /* Calculate smoothing parameters for all-pole filter */
    /* -------------------------------------------------- */

    for (iLoopCnt = 0; iLoopCnt < NP; iLoopCnt++)
    {
        //s_hi=0;
        //s_lo=0;
        pswCoefTmp3[iLoopCnt] = MSU_R(0, psrNWCoefs[iLoopCnt + NP], pswACoefs[iLoopCnt]);
        /*VPP_MLA16(s_hi, s_lo, -psrNWCoefs[iLoopCnt + NP], pswACoefs[iLoopCnt]);
        pswCoefTmp3[iLoopCnt] = ROUNDO( L_MLA_SAT(s_hi,s_lo));*/

    }

    /* Get impulse response of 1st filter                             */
    /* Done by direct form IIR filter of order NP zero input response */
    /* -------------------------------------------------------------- */

    lpcIrZsIir(pswACoefs, pswVecTmp2);

    /* Send impulse response of 1st filter through 2nd filter */
    /* All-zero filter (FIR)                                  */
    /* ------------------------------------------------------ */

    lpcZsFir(pswVecTmp2, pswCoefTmp2, pswVecTmp);

    /* Send impulse response of 2nd filter through 3rd filter */
    /* All-pole filter (IIR)                                  */
    /* ------------------------------------------------------ */

    lpcZsIirP(pswVecTmp, pswCoefTmp3);

    /* Calculate energy in impulse response */
    /* ------------------------------------ */

    swNormShift = g_corr1(pswVecTmp, &L_Temp);

    pL_AutoCorTmp[0] = L_Temp;

    /* Calculate normalized autocorrelation function */
    /* --------------------------------------------- */
    s_hi=0;
    s_lo=0;

    for (k = 1; k <= NP; k++)
    {

        /* Calculate R(k), equation 2.31 */
        /* ----------------------------- */

        //L_Temp = L_MULT(pswVecTmp[0], pswVecTmp[0 + k]);
        //s_lo=SHR(L_Temp, 1);
        VPP_MLX16(s_hi, s_lo, pswVecTmp[0], pswVecTmp[0 + k]);
        for (siNum = S_LEN - k, iLoopCnt2 = 1; iLoopCnt2 < siNum; iLoopCnt2++)
        {
            /*L_Temp = L_mac(L_Temp, pswVecTmp[iLoopCnt2],
                           pswVecTmp[iLoopCnt2 + k]);
            Lmult=L_MULT( pswVecTmp[iLoopCnt2],
                           pswVecTmp[iLoopCnt2 + k]);
            L_Temp=L_ADD(L_Temp,Lmult);*/

            VPP_MLA16(s_hi, s_lo,  pswVecTmp[iLoopCnt2],pswVecTmp[iLoopCnt2 + k]);
        }
        L_Temp= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        /* Normalize R(k) relative to R(0): */
        /* -------------------------------- */

        //pL_AutoCorTmp[k] = L_shl(L_Temp, swNormShift);
        //pL_AutoCorTmp[k] = L_SHL_SAT(L_Temp, swNormShift);
        pL_AutoCorTmp[k] =  SHL (L_Temp, swNormShift);

    }


    /* Convert normalized autocorrelations to direct form coefficients */
    /* --------------------------------------------------------------- */

    aFlatRcDp(pL_AutoCorTmp, pswTempRc);
    rcToADp(ASCALE, pswTempRc, pswHCoefs);

    VPP_HR_PROFILE_FUNCTION_EXIT( getNWCoefs);

}

/***************************************************************************
 *
 *   FUNCTION NAME: getNextVec
 *
 *   PURPOSE:
 *     The purpose of this function is to get the next vector in the list.
 *
 *   INPUTS: none
 *
 *   OUTPUTS: pswRc
 *
 *   RETURN VALUE: none
 *
 *   DESCRIPTION:
 *
 *     Both the quantizer and pre-quantizer are set concatenated 8 bit
 *     words.  Each of these words represents a reflection coefficient.
 *     The 8 bit words, are actually indices into a reflection
 *     coefficient lookup table.  Memory is organized in 16 bit words, so
 *     there are two reflection coefficients per ROM word.
 *
 *
 *     The full quantizer is subdivided into blocks.  Each of the
 *     pre-quantizers vectors "points" to a full quantizer block.  The
 *     vectors in a block, are comprised of either three or four
 *     elements.  These are concatenated, without leaving any space
 *     between them.
 *
 *     A block of full quantizer elements always begins on an even word.
 *     This may or may not leave a space depending on vector quantizer
 *     size.
 *
 *     getNextVec(), serves to abstract this arcane data format.  Its
 *     function is to simply get the next reflection coefficient vector
 *     in the list, be it a pre or full quantizer list.  This involves
 *     figuring out whether to pick the low or the high part of the 16
 *     bit ROM word.  As well as transforming the 8 bit stored value
 *     into a fractional reflection coefficient.  It also requires a
 *     setup routine to initialize iWordPtr and iWordHalfPtr, two
 *     variables global to this file.
 *
 *
 *
 *   REFERENCE:  Sub-clause 4.1.4.1 of GSM Recommendation 06.20
 *
 *   KEYWORDS: Quant quant vector quantizer
 *
 *************************************************************************/

void   getNextVec(INT16 pswRc[])
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
    i = iLow;

    if (iThree)
    {

        if (iWordHalfPtr == HIGH)
        {
            pswRc[i++] = psrSQuant[high(psrTable[iWordPtr])];
            pswRc[i++] = psrSQuant[low(psrTable[iWordPtr++])];
            pswRc[i] = psrSQuant[high(psrTable[iWordPtr])];
            iWordHalfPtr = LOW;
        }
        else
        {
            pswRc[i++] = psrSQuant[low(psrTable[iWordPtr++])];
            pswRc[i++] = psrSQuant[high(psrTable[iWordPtr])];
            pswRc[i] = psrSQuant[low(psrTable[iWordPtr++])];
            iWordHalfPtr = HIGH;
        }

    }
    else
    {
        pswRc[i++] = psrSQuant[high(psrTable[iWordPtr])];
        pswRc[i++] = psrSQuant[low(psrTable[iWordPtr++])];
        pswRc[i++] = psrSQuant[high(psrTable[iWordPtr])];
        pswRc[i] = psrSQuant[low(psrTable[iWordPtr++])];
    }
}

/***************************************************************************
 *
 *    FUNCTION NAME: getSfrmLpcTx
 *
 *    PURPOSE:
 *       Given frame information from past and present frame, interpolate
 *       (or copy) the frame based lpc coefficients into subframe
 *       lpc coeffs, i.e. the ones which will be used by the subframe
 *       as opposed to those coded and transmitted.
 *
 *    INPUT:
 *       swPrevR0,swNewR0 - Rq0 for the last frame and for this frame.
 *          These are the decoded values, not the codewords.
 *
 *       Previous lpc coefficients from the previous FRAME:
 *          in all filters below array[0] is the t=-1 element array[NP-1]
 *        t=-NP element.
 *       pswPrevFrmKs[NP] - decoded version of the rc's tx'd last frame
 *       pswPrevFrmAs[NP] - the above K's converted to A's.  i.e. direct
 *          form coefficients.
 *       pswPrevFrmSNWCoef[NP] - Coefficients for the Spectral Noise
 *          weighting filter from the previous frame
 *
 *       pswHPFSppech - pointer to High Pass Filtered Input speech
 *
 *       pswSoftInterp - a flag containing the soft interpolation
 *          decision.
 *
 *       Current lpc coefficients from the current frame:
 *       pswNewFrmKs[NP],pswNewFrmAs[NP],
 *       pswNewFrmSNWCoef[NP] - Spectral Noise Weighting Coefficients
 *           for the current frame
 *       ppswSNWCoefAs[1][NP] - pointer into a matrix containing
 *           the interpolated and uninterpolated LP Coefficient
 *           values for the Spectral Noise Weighting Filter.
 *
 *    OUTPUT:
 *       psnsSqrtRs[N_SUB] - a normalized number (struct NormSw)
 *          containing an estimate
 *          of RS for each subframe.  (number and a shift)
 *
 *       ppswSynthAs[N_SUM][NP] - filter coefficients used by the
 *          synthesis filter.
 *
 *    DESCRIPTION:
 *        For interpolated subframes, the direct form coefficients
 *        are converted to reflection coeffiecients to check for
 *        filter stability. If unstable, the uninterpolated coef.
 *        are used for that subframe.
 *
 *
 *    REFERENCE: Sub-clause of 4.1.6 and 4.1.7 of GSM Recommendation
 *        06.20
 *
 *    KEYWORDS: soft interpolation, int_lpc, interpolate, atorc, res_eng
 *
 *************************************************************************/


void   getSfrmLpcTx(INT16 swPrevR0, INT16 swNewR0,
                    /* last frm*/
                    INT16 pswPrevFrmKs[], INT16 pswPrevFrmAs[],
                    INT16 pswPrevFrmSNWCoef[],
                    /* this frm*/
                    INT16 pswNewFrmKs[], INT16 pswNewFrmAs[],
                    INT16 pswNewFrmSNWCoef[],
                    INT16 pswHPFSpeech[],
                    /* output */
                    short *pswSoftInterp,
                    struct NormSw *psnsSqrtRs,
                    INT16 ppswSynthAs[][NP], INT16 ppswSNWCoefAs[][NP])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swSi;
    INT32 L_Temp;
    short int siSfrm,
          siStable,
          i;

    VPP_HR_PROFILE_FUNCTION_ENTER( getSfrmLpcTx);

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* perform interpolation - both for synth filter and noise wgt filt */
    /*------------------------------------------------------------------*/
    siSfrm = 0;
    siStable = interpolateCheck(pswPrevFrmKs, pswPrevFrmAs,
                                pswPrevFrmAs, pswNewFrmAs,
                                psrOldCont[siSfrm], psrNewCont[siSfrm],
                                swPrevR0,
                                &psnsSqrtRs[siSfrm],
                                ppswSynthAs[siSfrm]);
    if (siStable)
    {
        for (i = 0; i < NP; i++)
        {
            L_Temp = L_MULT(pswNewFrmSNWCoef[i], psrNewCont[siSfrm]);
            ppswSNWCoefAs[siSfrm][i] = MAC_R(L_Temp, pswPrevFrmSNWCoef[i], psrOldCont[siSfrm]);
        }
    }
    else
    {

        /* this subframe is unstable */
        /*---------------------------*/

        for (i = 0; i < NP; i++)
        {
            ppswSNWCoefAs[siSfrm][i] = pswPrevFrmSNWCoef[i];
        }

    }

    /* interpolate subframes one and two */
    /*-----------------------------------*/

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
            for (i = 0; i < NP; i++)
            {
                L_Temp = L_MULT(pswNewFrmSNWCoef[i], psrNewCont[siSfrm]);
                ppswSNWCoefAs[siSfrm][i] = MAC_R(L_Temp, pswPrevFrmSNWCoef[i], psrOldCont[siSfrm]);
            }
        }
        else
        {
            /* this sfrm has unstable filter coeffs, would like to interp but
             * cant */
            /*--------------------------------------*/

            for (i = 0; i < NP; i++)
            {
                ppswSNWCoefAs[siSfrm][i] = pswNewFrmSNWCoef[i];
            }

        }
    }


    /* the last subframe: never interpolate. */
    /*--------------------------------------*/

    siSfrm = 3;
    for (i = 0; i < NP; i++)
    {
        ppswSNWCoefAs[siSfrm][i] = pswNewFrmSNWCoef[i];
        ppswSynthAs[siSfrm][i] = pswNewFrmAs[i];
    }

    /* calculate the residual energy for the last subframe */
    /*-----------------------------------------------------*/

    res_eng(pswNewFrmKs, swNewR0, &psnsSqrtRs[siSfrm]);



    /* done with interpolation, now compare the two sets of coefs.   */
    /* make the decision whether to interpolate (1) or not (0)       */
    /*---------------------------------------------------------------*/

    swSi = compResidEnergy(pswHPFSpeech,
                           ppswSynthAs, pswPrevFrmAs,
                           pswNewFrmAs, psnsSqrtRs);

    if (swSi == 0)
    {

        /* no interpolation done: copy the frame based data to output
         * coeffiecient arrays */

        siSfrm = 0;
        for (i = 0; i < NP; i++)
        {
            ppswSNWCoefAs[siSfrm][i] = pswPrevFrmSNWCoef[i];
            ppswSynthAs[siSfrm][i] = pswPrevFrmAs[i];
        }

        /* get RS (energy in the residual) for subframe 0 */
        /*------------------------------------------------*/

        res_eng(pswPrevFrmKs, swPrevR0, &psnsSqrtRs[siSfrm]);

        /* for subframe 1 and all subsequent sfrms, use lpc and R0 from new frm */
        /*---------------------------------------------------------------------*/

        res_eng(pswNewFrmKs, swNewR0, &psnsSqrtRs[1]);

        for (siSfrm = 2; siSfrm < N_SUB; siSfrm++)
            psnsSqrtRs[siSfrm] = psnsSqrtRs[1];

        for (siSfrm = 1; siSfrm < N_SUB; siSfrm++)
        {
            for (i = 0; i < NP; i++)
            {
                ppswSNWCoefAs[siSfrm][i] = pswNewFrmSNWCoef[i];
                ppswSynthAs[siSfrm][i] = pswNewFrmAs[i];
            }
        }
    }

    *pswSoftInterp = swSi;

    VPP_HR_PROFILE_FUNCTION_EXIT( getSfrmLpcTx);

}

/***************************************************************************
 *
 *    FUNCTION NAME: iir_d
 *
 *    PURPOSE:  Performs one second order iir section using double-precision.
 *              feedback,single precision xn and filter coefficients
 *
 *    INPUTS:
 *
 *      pswCoef[0:4]   An array of filter coefficients.
 *
 *      pswIn[0:159]   An array of input samples to be filtered, filtered
 *                     output samples written to the same array.
 *
 *      pswXstate[0:1] An array containing x-state memory.
 *
 *      pswYstate[0:3] An array containing y-state memory.
 *
 *      npts           Number of samples to filter (must be even).
 *
 *      shifts         number of shifts to be made on output y(n) before
 *                     storing to y(n) states.
 *
 *      swPreFirDownSh number of shifts apply to signal before the FIR.
 *
 *      swFinalUpShift number of shifts apply to signal before outputting.
 *
 *    OUTPUTS:
 *
 *       pswIn[0:159]  Output array containing the filtered input samples.
 *
 *    RETURN:
 *
 *       none.
 *
 *    DESCRIPTION:
 *
 *       Transfer function implemented:
 *         (b0 + b1*z-1 + b2*z-2)/(a0 - a1*z-1 - a2*z-2+
 *       data structure:
 *         Coeff array order:  b2,b1,b0,a2,a1
 *         Xstate array order: x(n-2),x(n-1)
 *         Ystate array order: y(n-2)MSB,y(n-2)LSB,y(n-1)MSB,y(n-1)LSB
 *
 *       There is no elaborate discussion of the filter, since it is
 *       trivial.
 *
 *       The filter's cutoff frequency is 120 Hz.
 *
 *    REFERENCE:  Sub-clause 4.1.1 GSM Recommendation 06.20
 *
 *    KEYWORDS: highpass filter, hp, HP, filter
 *
 *************************************************************************/

void   iir_d(INT16 pswCoeff[], INT16 pswIn[], INT16 pswXstate[],
             INT16 pswYstate[], int npts, int shifts,
             INT16 swPreFirDownSh, INT16 swFinalUpShift)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    int    loop_cnt;
    INT32 L_sumA,
          L_sumB;//,Lmult
    INT16 swTemp,
          pswYstate_0,
          pswYstate_1,
          pswYstate_2,
          pswYstate_3,
          pswXstate_0,
          pswXstate_1,
          swx0,
          swx1;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* initialize the temporary state variables */
    /*------------------------------------------*/

    pswYstate_0 = pswYstate[0];
    pswYstate_1 = pswYstate[1];
    pswYstate_2 = pswYstate[2];
    pswYstate_3 = pswYstate[3];

    pswXstate_0 = pswXstate[0];
    pswXstate_1 = pswXstate[1];

    for (loop_cnt = 0; loop_cnt < npts; loop_cnt += 2)
    {
        //swx0 = shr(pswIn[loop_cnt], swPreFirDownSh);
        //swx1 = shr(pswIn[loop_cnt + 1], swPreFirDownSh);
        swx0 = L_SHR_V(pswIn[loop_cnt], swPreFirDownSh);
        swx1 = L_SHR_V(pswIn[loop_cnt + 1], swPreFirDownSh);

        //L_sumB = L_MULT(pswYstate_1, pswCoeff[3]);
        //s_lo=SHR(L_sumB ,1);
        VPP_MLX16(s_hi, s_lo, pswYstate_1, pswCoeff[3]);
        //L_sumB = L_mac(L_sumB, pswYstate_3, pswCoeff[4]);
        /*Lmult=L_MULT( pswYstate_3, pswCoeff[4]);
        L_sumB=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo,  pswYstate_3, pswCoeff[4]);
        L_sumB= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

        //L_sumB = L_shr(L_sumB, 14);
        L_sumB = SHR(L_sumB, 14);

        s_lo = SHR(L_sumB ,1);

        //L_sumB = L_mac(L_sumB, pswYstate_0, pswCoeff[3]);
        /*Lmult=L_MULT( pswYstate_0, pswCoeff[3]);
        L_sumB=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo,  pswYstate_0, pswCoeff[3]);

        //L_sumB = L_mac(L_sumB, pswYstate_2, pswCoeff[4]);
        /*Lmult=L_MULT( pswYstate_2, pswCoeff[4]);
        L_sumB=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswYstate_2, pswCoeff[4]);


        //L_sumA = L_mac(L_sumB, pswCoeff[0], pswXstate_0);
        /*Lmult=L_MULT( pswCoeff[0], pswXstate_0);
        L_sumA=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswCoeff[0], pswXstate_0);


        //L_sumA = L_mac(L_sumA, pswCoeff[1], pswXstate_1);
        /*Lmult=L_MULT( pswCoeff[1], pswXstate_1);
        L_sumA=L_ADD(L_sumA,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswCoeff[1], pswXstate_1);

        //L_sumA = L_mac(L_sumA, pswCoeff[2], swx0);
        /*Lmult=L_MULT( pswCoeff[2], swx0);
        L_sumA=L_ADD(L_sumA,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswCoeff[2], swx0);

        L_sumA= L_MLA_SAT_CARRY(s_lo, L_sumB&1);// L_MLA_SAT(s_hi, s_lo);
        //L_sumA= L_MLA_CARRY_SAT(s_hi, s_lo, (L_sumB & 1));

        //L_sumA = L_shl(L_sumA, shifts);
        //L_sumA = L_SHL_SAT(L_sumA, shifts);
        L_sumA <<=  shifts ;

        pswXstate_0 = swx0;                /* Update X state x(n-1) <- x(n) */

        /* Update double precision Y state temporary variables */
        /*-----------------------------------------------------*/

        pswYstate_0 = EXTRACT_H(L_sumA);
        swTemp = EXTRACT_L(L_sumA);
        //swTemp = SHR(swTemp, 2);
        swTemp >>=  2 ;
        pswYstate_1 = 0x3fff & swTemp;

        /* Round, store output sample and increment to next input sample */
        /*---------------------------------------------------------------*/

        //pswIn[loop_cnt] = ROUNDO(L_shl(L_sumA, swFinalUpShift));
        //pswIn[loop_cnt] = ROUNDO(L_SHL_SAT(L_sumA, swFinalUpShift));
        pswIn[loop_cnt] = ROUNDO( SHL (L_sumA, swFinalUpShift));

        //L_sumB = L_MULT(pswYstate_3, pswCoeff[3]);
        //s_lo=SHR(L_sumB ,1);
        VPP_MLX16(s_hi, s_lo, pswYstate_3, pswCoeff[3]);
        //L_sumB = L_mac(L_sumB, pswYstate_1, pswCoeff[4]);
        VPP_MLA16(s_hi, s_lo, pswYstate_1, pswCoeff[4]);
        L_sumB= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
        //L_sumB = L_shr(L_sumB, 14);
        //L_sumB = SHR(L_sumB, 14);
        L_sumB >>=  14 ;

        s_lo=SHR(L_sumB ,1);
        //L_sumB = L_mac(L_sumB, pswYstate_2, pswCoeff[3]);
        /*Lmult=L_MULT( pswYstate_2, pswCoeff[3]);
        L_sumB=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswYstate_2, pswCoeff[3]);

        //L_sumB = L_mac(L_sumB, pswYstate_0, pswCoeff[4]);
        /*Lmult=L_MULT( pswYstate_0, pswCoeff[4]);
        L_sumB=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswYstate_0, pswCoeff[4]);

        //L_sumA = L_mac(L_sumB, pswCoeff[0], pswXstate_1);
        /*Lmult=L_MULT( pswCoeff[0], pswXstate_1);
        L_sumA=L_ADD(L_sumB,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswCoeff[0], pswXstate_1);

        //L_sumA = L_mac(L_sumA, pswCoeff[1], pswXstate_0);
        /*Lmult=L_MULT( pswCoeff[1], pswXstate_0);
        L_sumA=L_ADD(L_sumA,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswCoeff[1], pswXstate_0);

        //L_sumA = L_mac(L_sumA, pswCoeff[2], swx1);
        /*Lmult=L_MULT( pswCoeff[2], swx1);
        L_sumA=L_ADD(L_sumA,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswCoeff[2], swx1);

        L_sumA= L_MLA_SAT_CARRY(s_lo, L_sumB&1);//  L_MLA_SAT_CARRY(s_lo, L_sumB&1);//L_MLA_SAT(s_hi, s_lo);

        //L_sumA = L_shl(L_sumA, shifts);
        //L_sumA = L_SHL_SAT(L_sumA, shifts);
        L_sumA <<=  shifts ;

        pswXstate_1 = swx1;                /* Update X state x(n-1) <- x(n) */

        /* Update double precision Y state temporary variables */
        /*-----------------------------------------------------*/

        pswYstate_2 = EXTRACT_H(L_sumA);

        swTemp = EXTRACT_L(L_sumA);
        //swTemp = SHR(swTemp, 2);
        swTemp >>=  2 ;
        pswYstate_3 = 0x3fff & swTemp;

        /* Round, store output sample and increment to next input sample */
        /*---------------------------------------------------------------*/

        //pswIn[loop_cnt + 1] = ROUNDO(L_shl(L_sumA, swFinalUpShift));
        //pswIn[loop_cnt + 1] = ROUNDO(L_SHL_SAT(L_sumA, swFinalUpShift));
        pswIn[loop_cnt + 1] = ROUNDO( SHL (L_sumA, swFinalUpShift));

    }

    /* update the states for the next frame */
    /*--------------------------------------*/

    pswYstate[0] = pswYstate_0;
    pswYstate[1] = pswYstate_1;
    pswYstate[2] = pswYstate_2;
    pswYstate[3] = pswYstate_3;

    pswXstate[0] = pswXstate_0;
    pswXstate[1] = pswXstate_1;

}

/***************************************************************************
 *
 *    FUNCTION NAME: initPBarVBarFullL
 *
 *    PURPOSE:  Given the INT32 normalized correlation sequence, function
 *              initPBarVBarL initializes the INT32 initial condition
 *              arrays pL_PBarFull and pL_VBarFull for a full 10-th order LPC
 *              filter. It also shifts down the pL_VBarFull and pL_PBarFull
 *              arrays by a global constant RSHIFT bits. The pL_PBarFull and
 *              pL_VBarFull arrays are used to set the initial INT16
 *              P and V conditions which are used in the actual search of the
 *              Rc prequantizer and the Rc quantizer.
 *
 *              This is an implementation of equations 4.14 and
 *              4.15.
 *
 *    INPUTS:
 *
 *        pL_CorrelSeq[0:NP]
 *                     A INT32 normalized autocorrelation array computed
 *                     from unquantized reflection coefficients.
 *
 *       RSHIFT       The number of right shifts to be applied to the
 *                     input correlations prior to initializing the elements
 *                     of pL_PBarFull and pL_VBarFull output arrays. RSHIFT
 *                     is a global constant.
 *
 *    OUTPUTS:
 *
 *        pL_PBarFull[0:NP-1]
 *                     A INT32 output array containing the P initial
 *                     conditions for the full 10-th order LPC filter.
 *                     The address of the 0-th element of  pL_PBarFull
 *                     is passed in when function initPBarVBarFullL is
 *                     called.
 *
 *        pL_VBarFull[-NP+1:NP-1]
 *                     A INT32 output array containing the V initial
 *                     conditions for the full 10-th order LPC filter.
 *                     The address of the 0-th element of pL_VBarFull is
 *                     passed in when function initPBarVBarFullL is called.
 *    RETURN:
 *        none.
 *
 *    REFERENCE:  Sub-clause 4.1.4.1 GSM Recommendation 06.20
 *
 *************************************************************************/

void   initPBarFullVBarFullL(INT32 pL_CorrelSeq[],
                             INT32 pL_PBarFull[],
                             INT32 pL_VBarFull[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    int    i,
           bound;

    //VPP_HR_PROFILE_FUNCTION_ENTER( initPBarFullVBarFullL);

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Initialize the AFLAT recursion PBarFull and VBarFull 32 bit arrays */
    /* for a 10-th order LPC filter.                                      */
    /*--------------------------------------------------------------------*/

    bound = NP - 1;

    for (i = 0; i <= bound; i++)
    {
        //pL_PBarFull[i] = L_shr(pL_CorrelSeq[i], RSHIFT);
        pL_PBarFull[i] = SHR(pL_CorrelSeq[i], RSHIFT);

    }

    for (i = -bound; i < 0; i++)
    {
        pL_VBarFull[i] = pL_PBarFull[-i - 1];
    }

    for (i = 0; i < bound; i++)
    {
        pL_VBarFull[i] = pL_PBarFull[i + 1];
    }

    //pL_VBarFull[bound] = L_shr(pL_CorrelSeq[bound + 1], RSHIFT);
    pL_VBarFull[bound] = SHR(pL_CorrelSeq[bound + 1], RSHIFT);

    //VPP_HR_PROFILE_FUNCTION_EXIT(initPBarFullVBarFullL );
}

/***************************************************************************
 *
 *    FUNCTION NAME: initPBarVBarL
 *
 *    PURPOSE:  Given the INT32 pL_PBarFull array,
 *              function initPBarVBarL initializes the INT16 initial
 *              condition arrays pswPBar and pswVBar for a 3-rd order LPC
 *              filter, since the order of the 1st Rc-VQ segment is 3.
 *              The pswPBar and pswVBar arrays are a INT16 subset
 *              of the initial condition array pL_PBarFull.
 *              pswPBar and pswVBar are the initial conditions for the AFLAT
 *              recursion at a given segment. The AFLAT recursion is used
 *              to evaluate the residual error due to an Rc vector selected
 *              from a prequantizer or a quantizer.
 *
 *              This is an implementation of equation 4.18 and 4.19.
 *
 *    INPUTS:
 *
 *        pL_PBarFull[0:NP-1]
 *                     A INT32 input array containing the P initial
 *                     conditions for the full 10-th order LPC filter.
 *                     The address of the 0-th element of  pL_PBarFull
 *                     is passed in when function initPBarVBarL is called.
 *
 *    OUTPUTS:
 *
 *        pswPBar[0:NP_AFLAT-1]
 *                     The output INT16 array containing the P initial
 *                     conditions for the P-V AFLAT recursion, set here
 *                     for the Rc-VQ search at the 1st Rc-VQ segment.
 *                     The address of the 0-th element of pswPBar is
 *                     passed in when function initPBarVBarL is called.
 *
 *        pswVBar[-NP_AFLAT+1:NP_AFLAT-1]
 *                     The output INT16 array containing the V initial
 *                     conditions for the P-V AFLAT recursion, set here
 *                     for the Rc-VQ search at the 1st Rc-VQ segment.
 *                     The address of the 0-th element of pswVBar is
 *                     passed in when function initPBarVBarL is called.
 *
 *    RETURN:
 *
 *        none.
 *
 *    REFERENCE:  Sub-clause 4.1.4.1 GSM Recommendation 06.20
 *
 *
 *************************************************************************/

void   initPBarVBarL(INT32 pL_PBarFull[],
                     INT16 pswPBar[],
                     INT16 pswVBar[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    int    bound,
           i;

    //VPP_HR_PROFILE_FUNCTION_ENTER(initPBarVBarL );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Initialize the AFLAT recursion P and V 16 bit arrays for a 3-rd    */
    /* order LPC filter corresponding to the 1-st reflection coefficient  */
    /* VQ segment. The PBar and VBar arrays store the initial conditions  */
    /* for the evaluating the residual error due to Rc vectors being      */
    /* evaluated from the Rc-VQ codebook at the 1-st Rc-VQ segment.       */
    /*--------------------------------------------------------------------*/
    bound = 2;

    for (i = 0; i <= bound; i++)
    {
        pswPBar[i] = ROUNDO(pL_PBarFull[i]);
    }
    for (i = -bound; i < 0; i++)
    {
        pswVBar[i] = pswPBar[-i - 1];
    }
    for (i = 0; i < bound; i++)
    {
        pswVBar[i] = pswPBar[i + 1];
    }
    pswVBar[bound] = ROUNDO(pL_PBarFull[bound + 1]);

//VPP_HR_PROFILE_FUNCTION_EXIT( initPBarVBarL);
}

/***************************************************************************
 *
 *   FUNCTION NAME: maxCCOverGWithSign
 *
 *   PURPOSE:
 *
 *     Finds lag which maximizes C^2/G ( C is allowed to be negative ).
 *
 *   INPUTS:
 *
 *     pswCIn[0:swNum-1]
 *
 *                     Array of C values
 *
 *     pswGIn[0:swNum-1]
 *
 *                     Array of G values
 *
 *     pswCCmax
 *
 *                     Initial value of CCmax
 *
 *     pswGmax
 *
 *                     Initial value of Gmax
 *
 *     swNum
 *
 *                     Number of lags to be searched
 *
 *   OUTPUTS:
 *
 *     pswCCmax
 *
 *                     Value of CCmax after search
 *
 *     pswGmax
 *
 *                     Value of Gmax after search
 *
 *   RETURN VALUE:
 *
 *     maxCCGIndex - index for max C^2/G, defaults to zero if all G <= 0
 *
 *   DESCRIPTION:
 *
 *     This routine is called from bestDelta().  The routine is a simple
 *     find the best in a list search.
 *
 *   REFERENCE:  Sub-clause 4.1.8.3 of GSM Recommendation 06.20
 *
 *   KEYWORDS: LTP correlation peak
 *
 *************************************************************************/

INT16 maxCCOverGWithSign(INT16 pswCIn[],
                         INT16 pswGIn[],
                         INT16 *pswCCMax,
                         INT16 *pswGMax,
                         INT16 swNum)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swCC,
          i,
          maxCCGIndex,
          swCCMax,
          swGMax;
    INT32 L_Temp;

    register INT32  s_hi=0;
    register UINT32 s_lo=0;


    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* initialize max c^2/g to be the initial trajectory */
    /*---------------------------------------------------*/

    maxCCGIndex = 0;
    swGMax = pswGIn[0];

    if (pswCIn[0] < 0)
        swCCMax = NEGATE(MULT_R(pswCIn[0], pswCIn[0]));
    else
        swCCMax = MULT_R(pswCIn[0], pswCIn[0]);

    for (i = 1; i < swNum; i++)
    {

        /* Imperfect interpolation can result in negative energies. */
        /* Check for this                                             */
        /*----------------------------------------------------------*/

        if (pswGIn[i] > 0)
        {

            swCC = MULT_R(pswCIn[i], pswCIn[i]);

            if (pswCIn[i] < 0)
                swCC = NEGATE(swCC);

            //L_Temp = L_MULT(swCC, swGMax);
            //s_lo=SHR( L_Temp,1);
            VPP_MLX16(s_hi, s_lo, swCC, swGMax);
            //L_Temp = L_msu(L_Temp, pswGIn[i], swCCMax);
            VPP_MLA16(s_hi, s_lo, -pswGIn[i], swCCMax);

            L_Temp= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);

            /* Check if C^2*Gmax - G*Cmax^2 > 0 */
            /* -------------------------------- */

            if (L_Temp > 0)
            {
                swGMax = pswGIn[i];
                swCCMax = swCC;
                maxCCGIndex = i;
            }
        }
    }
    *pswGMax = swGMax;
    *pswCCMax = swCCMax;

    return (maxCCGIndex);
}                                      /* end of maxCCOverGWithSign */

/***************************************************************************
 *
 *   FUNCTION NAME: openLoopLagSearch
 *
 *   PURPOSE:
 *
 *     Determines voicing level for the frame.  If voiced, obtains list of
 *     lags to be searched in closed-loop lag search; and value of smoothed
 *     pitch and coefficient for harmonic-noise-weighting.
 *
 *   INPUTS:
 *
 *     pswWSpeech[-145:159] ( [-LSMAX:F_LEN-1] )
 *
 *                     W(z) filtered speech frame, with some history.
 *
 *     swPrevR0Index
 *
 *                     Index of R0 from the previous frame.
 *
 *     swCurrR0Index
 *
 *                     Index of R0 for the current frame.
 *
 *     psrLagTbl[0:255]
 *
 *                     Lag quantization table, in global ROM.
 *
 *     ppsrCGIntFilt[0:5][0:5] ( [tap][phase] )
 *
 *                     Interpolation smoothing filter for generating C(k)
 *                     and G(k) terms, where k is fractional.  Global ROM.
 *
 *     swSP
 *                     speech flag, required for DTX mode
 *
 *   OUTPUTS:
 *
 *     psiUVCode
 *
 *                     (Pointer to) the coded voicing level.
 *
 *     pswLagList[0:?]
 *
 *                     Array of lags to use in the search of the adaptive
 *                     codebook (long-term predictor).  Length determined
 *                     by pswNumLagList[].
 *
 *     pswNumLagList[0:3] ( [0:N_SUB-1] )
 *
 *                     Array of number of lags to use in search of adaptive
 *                     codebook (long-term predictor) for each subframe.
 *
 *     pswPitchBuf[0:3] ( [0:N_SUB-1] )
 *
 *                     Array of estimates of pitch, to be used in harmonic-
 *                     noise-weighting.
 *
 *     pswHNWCoefBuf[0:3] ( [0:N_SUB-1] )
 *
 *                     Array of harmonic-noise-weighting coefficients.
 *
 *     psnsWSfrmEng[-4:3] ( [-N_SUB:N_SUB-1] )
 *
 *                     Array of energies of weighted speech (input speech
 *                     sent through W(z) weighting filter), each stored as
 *                     normalized fraction and shift count.  The zero index
 *                     corresponds to the first subframe of the current
 *                     frame, so there is some history represented.  The
 *                     energies are used for scaling purposes only.
 *
 *     pswVadLags[4]
 *
 *                     An array of INT16s containing the best open
 *                     loop LTP lags for the four subframes.

 *
 *   DESCRIPTION:
 *
 *     Scaling is done on the input weighted speech, such that the C(k) and
 *     G(k) terms will all be representable.  The amount of scaling is
 *     determined by the maximum energy of any subframe of weighted speech
 *     from the current frame or last frame.  These energies are maintained
 *     in a buffer, and used for scaling when the excitation is determined
 *     later in the analysis.
 *
 *     This function is the main calling program for the open loop lag
 *     search.
 *
 *   REFERENCE:  Sub-clauses 4.1.8.1-4.1.8.4 of GSM Recommendation 06.20
 *
 *   Keywords: openlooplagsearch, openloop, lag, pitch
 *
 **************************************************************************/



void   openLoopLagSearch(INT16 pswWSpeech[],
                         INT16 swPrevR0Index,
                         INT16 swCurrR0Index,
                         INT16 *psiUVCode,
                         INT16 pswLagList[],
                         INT16 pswNumLagList[],
                         INT16 pswPitchBuf[],
                         INT16 pswHNWCoefBuf[],
                         struct NormSw psnsWSfrmEng[],
                         INT16 pswVadLags[],
                         INT16 swSP)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT32 L_WSfrmEng,
          L_G,
          L_C,
          L_Voicing;//,Lmult
    INT16 swBestPG,
          swCCMax,
          swGMax,
          swCCDivG;
    INT16 swTotalCCDivG,
          swCC,
          swG,
          swRG;
    short  i,
           j,
           k,
           siShift,
           siIndex,
           siTrajIndex,
           siAnchorIndex;
    short  siNumPeaks,
           siNumTrajToDo,
           siPeakIndex,
           siFIndex;
    short  siNumDelta,
           siBIndex,
           siBestTrajIndex = 0;
    short  siLowestSoFar,
           siLagsSoFar,
           si1,
           si2,
           si3;
    struct NormSw snsMax;

    INT16 pswGFrame[G_FRAME_LEN];
    INT16 *ppswGSfrm[N_SUB];
    INT16 pswSfrmEng[N_SUB];
    INT16 pswCFrame[C_FRAME_LEN];
    INT16 *ppswCSfrm[N_SUB];
    INT16 pswLIntBuf[N_SUB];
    INT16 pswCCThresh[N_SUB];
    INT16 pswScaledWSpeechBuffer[W_F_BUFF_LEN];
    INT16 *pswScaledWSpeech;
    INT16 ppswTrajLBuf[N_SUB * NUM_TRAJ_MAX][N_SUB];
    INT16 ppswTrajCCBuf[N_SUB * NUM_TRAJ_MAX][N_SUB];
    INT16 ppswTrajGBuf[N_SUB * NUM_TRAJ_MAX][N_SUB];
    INT16 pswLPeaks[2 * LMAX / LMIN];
    INT16 pswCPeaks[2 * LMAX / LMIN];
    INT16 pswGPeaks[2 * LMAX / LMIN];
    INT16 pswLArray[DELTA_LEVELS];

    register INT32  s_hi=0;
    register UINT32 s_lo=0;

    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/



    VPP_HR_PROFILE_FUNCTION_ENTER(openLoopLagSearch );


    pswScaledWSpeech = pswScaledWSpeechBuffer + LSMAX;

    /*_________________________________________________________________________
     |                                                                         |
     |                            Executable Code                              |
     |_________________________________________________________________________|
    */

    /* Scale the weighted speech so that all correlations and energies */
    /* will be less than 1.0 in magnitude.  The scale factor is        */
    /* determined by the maximum energy of any subframe contained in   */
    /* the weighted speech buffer                                      */
    /*-----------------------------------------------------------------*/

    /* Perform one frame of delay on the subframe energy array */
    /*---------------------------------------------------------*/

    for (i = 0; i < N_SUB; i++)
        psnsWSfrmEng[i - N_SUB] = psnsWSfrmEng[i];

    /* Calculate the subframe energies of the current weighted speech frame. */
    /* Overflow protection is done based on the energy in the LPC analysis  */
    /* window (previous or current) which is closest to the subframe.       */
    /*----------------------------------------------------------------------*/

    psnsWSfrmEng[0].sh = g_corr1s(&pswWSpeech[0],
                                  r0BasedEnergyShft(swPrevR0Index),
                                  &L_WSfrmEng);
    psnsWSfrmEng[0].man = ROUNDO(L_WSfrmEng);

    psnsWSfrmEng[1].sh = g_corr1s(&pswWSpeech[S_LEN],
                                  r0BasedEnergyShft(swCurrR0Index),
                                  &L_WSfrmEng);
    psnsWSfrmEng[1].man = ROUNDO(L_WSfrmEng);

    psnsWSfrmEng[2].sh = g_corr1s(&pswWSpeech[2 * S_LEN],
                                  r0BasedEnergyShft(swCurrR0Index),
                                  &L_WSfrmEng);
    psnsWSfrmEng[2].man = ROUNDO(L_WSfrmEng);

    psnsWSfrmEng[3].sh = g_corr1s(&pswWSpeech[3 * S_LEN],
                                  r0BasedEnergyShft(swCurrR0Index),
                                  &L_WSfrmEng);
    psnsWSfrmEng[3].man = ROUNDO(L_WSfrmEng);

    /* Find the maximum weighted speech subframe energy from all values */
    /* in the array (the array includes the previous frame's subframes, */
    /* and the current frame's subframes)                               */
    /*------------------------------------------------------------------*/

    snsMax.man = 0;
    snsMax.sh = 0;
    for (i = -N_SUB; i < N_SUB; i++)
    {

        if (psnsWSfrmEng[i].man > 0)
        {

            if (snsMax.man == 0)
                snsMax = psnsWSfrmEng[i];

            if ( psnsWSfrmEng[i].sh < snsMax.sh )
                snsMax = psnsWSfrmEng[i];

            if ( (psnsWSfrmEng[i].sh ==  snsMax.sh)  &&  (psnsWSfrmEng[i].man > snsMax.man)  )
                snsMax = psnsWSfrmEng[i];
        }
    }

    /* Now scale speech up or down, such that the maximum subframe */
    /* energy value will be in range [0.125, 0.25).  This gives a  */
    /* little room for other maxima, and interpolation filtering   */
    /*-------------------------------------------------------------*/

    //siShift = sub(shr(snsMax.sh, 1), 1);
    siShift = SUB(SHR(snsMax.sh, 1), 1);

    for (i = 0; i < W_F_BUFF_LEN; i++)
        pswScaledWSpeech[i - LSMAX] = SHL(pswWSpeech[i - LSMAX], siShift);

    /* Calculate the G(k) (k an integer) terms for all subframes.  (A note */
    /* on the organization of the G buffer: G(k) for a given subframe is   */
    /* the energy in the weighted speech sequence of length S_LEN (40)     */
    /* which begins k back from the beginning of the given subframe-- that */
    /* is, it begins at a lag of k.  These sequences overlap from one      */
    /* subframe to the next, so it is only necessary to compute and store  */
    /* the unique energies.  The unique energies are computed and stored   */
    /* in this buffer, and pointers are assigned for each subframe to make */
    /* array indexing for each subframe easier.                            */
    /* */
    /* (Terms in the G buffer are in order of increasing k, so the energy  */
    /* of the first sequence-- that is, the oldest sequence-- in the       */
    /* weighted speech buffer appears at the end of the G buffer.          */
    /* */
    /* (The subframe pointers are assigned so that they point to the first */
    /* k for their respective subframes, k = LSMIN.)                       */
    /*---------------------------------------------------------------------*/

    //L_G = 0;
    s_lo=0;
    for (i = -LSMAX; i < -LSMAX + S_LEN; i++)
    {
        //L_G = L_mac(L_G, pswScaledWSpeech[i], pswScaledWSpeech[i]);
        /*Lmult=L_MULT( pswScaledWSpeech[i], pswScaledWSpeech[i]);
        L_G =L_ADD(L_G ,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswScaledWSpeech[i], pswScaledWSpeech[i]);
    }
    L_G= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
    pswGFrame[G_FRAME_LEN - 1] = EXTRACT_H(L_G);

    s_lo=SHR( L_G,1);
    for (i = -LSMAX; i < G_FRAME_LEN - LSMAX - 1; i++)
    {

        //L_G = L_msu(L_G, pswScaledWSpeech[i], pswScaledWSpeech[i]);
        VPP_MLA16(s_hi, s_lo, -pswScaledWSpeech[i], pswScaledWSpeech[i]);

        /*L_G = L_mac(L_G, pswScaledWSpeech[i + S_LEN],
                    pswScaledWSpeech[i + S_LEN]);
        Lmult=L_MULT( pswScaledWSpeech[i + S_LEN],
                    pswScaledWSpeech[i + S_LEN]);
        L_G =L_ADD(L_G ,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswScaledWSpeech[i + S_LEN], pswScaledWSpeech[i + S_LEN]);

        L_G= L_MLA_SAT_CARRY(s_lo, L_G&1);// L_MLA_SAT(s_hi, s_lo);


        pswGFrame[G_FRAME_LEN - LSMAX - 2 - i] = EXTRACT_H(L_G);
    }

    ppswGSfrm[0] = pswGFrame + 3 * S_LEN;
    ppswGSfrm[1] = pswGFrame + 2 * S_LEN;
    ppswGSfrm[2] = pswGFrame + S_LEN;
    ppswGSfrm[3] = pswGFrame;

    /* Copy the G(k) terms which also happen to be the subframe energies; */
    /* calculate the 4th subframe energy, which is not a G(k)             */
    /*--------------------------------------------------------------------*/

    pswSfrmEng[0] = pswGFrame[G_FRAME_LEN - 1 - LSMAX];
    pswSfrmEng[1] = pswGFrame[G_FRAME_LEN - 1 - LSMAX - S_LEN];
    pswSfrmEng[2] = pswGFrame[G_FRAME_LEN - 1 - LSMAX - 2 * S_LEN];

    //L_WSfrmEng = 0;
    s_lo=0;
    for (i = F_LEN - S_LEN; i < F_LEN; i++)
    {
        //L_WSfrmEng = L_mac(L_WSfrmEng, pswScaledWSpeech[i], pswScaledWSpeech[i]);
        /*Lmult=L_MULT( pswScaledWSpeech[i], pswScaledWSpeech[i]);
        L_WSfrmEng =L_ADD(L_WSfrmEng ,Lmult);*/
        VPP_MLA16(s_hi, s_lo, pswScaledWSpeech[i], pswScaledWSpeech[i]);
    }
    L_WSfrmEng= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
    pswSfrmEng[3] = EXTRACT_H(L_WSfrmEng);

    /* Calculate the C(k) (k an integer) terms for all subframes. */
    /* (The C(k) terms are all unique, so there is no overlapping */
    /* as in the G buffer.)                                       */
    /*------------------------------------------------------------*/

    for (i = 0; i < N_SUB; i++)
    {

        for (j = LSMIN; j <= LSMAX; j++)
        {

            //L_C = 0;
            s_lo=0;
            for (k = 0; k < S_LEN; k++)
            {

                /*L_C = L_mac(L_C, pswScaledWSpeech[i * S_LEN + k],
                            pswScaledWSpeech[i * S_LEN - j + k]);
                Lmult=L_MULT( pswScaledWSpeech[i * S_LEN + k],
                            pswScaledWSpeech[i * S_LEN - j + k]);
                L_C =L_ADD(L_C ,Lmult);*/
                VPP_MLA16(s_hi, s_lo, pswScaledWSpeech[i * S_LEN + k], pswScaledWSpeech[i * S_LEN - j + k]);
            }
            L_C= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
            pswCFrame[i * CG_TERMS + j - LSMIN] = EXTRACT_H(L_C);
        }
    }

    ppswCSfrm[0] = pswCFrame;
    ppswCSfrm[1] = pswCFrame + CG_TERMS;
    ppswCSfrm[2] = pswCFrame + 2 * CG_TERMS;
    ppswCSfrm[3] = pswCFrame + 3 * CG_TERMS;

    /* For each subframe: find the max C(k)*C(k)/G(k) where C(k) > 0 and */
    /* k is integer; save the corresponding k; calculate the             */
    /* threshold against which other peaks in the interpolated CC/G      */
    /* sequence will be checked.  Meanwhile, accumulate max CC/G over    */
    /* the frame for the voiced/unvoiced determination.                  */
    /*-------------------------------------------------------------------*/

    swBestPG = 0;
    for (i = 0; i < N_SUB; i++)
    {

        /* Find max CC/G (C > 0), store corresponding k */
        /*----------------------------------------------*/

        swCCMax = 0;
        swGMax = 0x003f;
        siIndex = fnBest_CG(&ppswCSfrm[i][LMIN - LSMIN],
                            &ppswGSfrm[i][LMIN - LSMIN],
                            &swCCMax, &swGMax,
                            LMAX - LMIN + 1);

        if (siIndex == -1)
        {
            pswLIntBuf[i] = 0;
            pswVadLags[i] = LMIN;            /* store lag value for VAD algorithm */
        }
        else
        {
            //pswLIntBuf[i] = ADD_SAT16(LMIN, (INT16) siIndex);
            pswLIntBuf[i] = ADD(LMIN, (INT16) siIndex);
            pswVadLags[i] = pswLIntBuf[i];   /* store lag value for VAD algorithm */
        }

        if (pswLIntBuf[i] > 0)
        {

            /* C > 0 was found: accumulate CC/G, get threshold */
            /*-------------------------------------------------*/

            if ( swCCMax < swGMax )
                //swCCDivG = divide_s(swCCMax, swGMax);
                swCCDivG = DIVIDE_SAT16(swCCMax, swGMax);
            else
                swCCDivG = SW_MAX;

            //swBestPG = ADD_SAT16(swCCDivG, swBestPG);
            swBestPG = ADD(swCCDivG, swBestPG);

            pswCCThresh[i] = getCCThreshold(pswSfrmEng[i], swCCMax, swGMax);
        }
        else
            pswCCThresh[i] = 0;
    }

    /* Make voiced/unvoiced decision */
    /*-------------------------------*/

    //L_Voicing = 0;
    s_lo=0;
    /*for (i = 0; i < N_SUB; i++)
    {
      //L_Voicing = L_mac(L_Voicing, pswSfrmEng[i], UV_SCALE0);
      //Lmult=L_MULT( pswSfrmEng[i], UV_SCALE0);
      //L_Voicing =L_ADD(L_Voicing ,Lmult);
      VPP_MLA16(s_hi, s_lo, pswSfrmEng[i], UV_SCALE0);
    }*/
    VPP_MLA16(s_hi, s_lo, pswSfrmEng[0], UV_SCALE0);
    VPP_MLA16(s_hi, s_lo, pswSfrmEng[1], UV_SCALE0);
    VPP_MLA16(s_hi, s_lo, pswSfrmEng[2], UV_SCALE0);
    VPP_MLA16(s_hi, s_lo, pswSfrmEng[3], UV_SCALE0);

    L_Voicing= L_MLA_SAT_CARRY(s_lo, 0);// L_MLA_SAT(s_hi, s_lo);
    //L_Voicing = L_ADD(L_Voicing, L_DEPOSIT_H(swBestPG));
    L_Voicing =  ADD(L_Voicing, L_DEPOSIT_H(swBestPG));

    if ( (L_Voicing <= 0) || (swSP == 0) )
    {

        /* Unvoiced: set return values to zero */
        /*-------------------------------------*/

        /*for (i = 0; i < N_SUB; i++)
        {

          pswNumLagList[i] = 0;
          pswLagList[i] = 0;
          pswPitchBuf[i] = 0;
          pswHNWCoefBuf[i] = 0;
        }*/

        pswNumLagList[0] = 0;
        pswLagList[0] = 0;
        pswPitchBuf[0] = 0;
        pswHNWCoefBuf[0] = 0;

        pswNumLagList[1] = 0;
        pswLagList[1] = 0;
        pswPitchBuf[1] = 0;
        pswHNWCoefBuf[1] = 0;

        pswNumLagList[2] = 0;
        pswLagList[2] = 0;
        pswPitchBuf[2] = 0;
        pswHNWCoefBuf[2] = 0;

        pswNumLagList[3] = 0;
        pswLagList[3] = 0;
        pswPitchBuf[3] = 0;
        pswHNWCoefBuf[3] = 0;

        *psiUVCode = 0;
    }
    else
    {

        /* Voiced: get best delta-codeable lag trajectory; find pitch and */
        /* harmonic-noise-weighting coefficients for each subframe        */
        /*----------------------------------------------------------------*/

        siTrajIndex = 0;
        swBestPG = SW_MIN;
        for (siAnchorIndex = 0; siAnchorIndex < N_SUB; siAnchorIndex++)
        {

            /* Call pitchLags: for the current subframe, find peaks in the */
            /* C(k)**2/G(k) (k fractional) function which exceed the       */
            /* threshold set by the maximum C(k)**2/G(k) (k integer)       */
            /* (also get the smoothed pitch and harmonic-noise-weighting   */
            /* coefficient).                                               */
            /* */
            /* If there is no C(k) > 0 (k integer), set the smoothed pitch */
            /* to its minimum value and set the harmonic-noise-weighting   */
            /* coefficient to zero.                                        */
            /*-------------------------------------------------------------*/

            if (pswLIntBuf[siAnchorIndex] != 0)
            {

                pitchLags(pswLIntBuf[siAnchorIndex],
                          ppswCSfrm[siAnchorIndex],
                          ppswGSfrm[siAnchorIndex],
                          pswCCThresh[siAnchorIndex],
                          pswLPeaks,
                          pswCPeaks,
                          pswGPeaks,
                          &siNumPeaks,
                          &pswPitchBuf[siAnchorIndex],
                          &pswHNWCoefBuf[siAnchorIndex]);

                siPeakIndex = 0;
            }
            else
            {

                /* No C(k) > 0 (k integer): set pitch to min, coef to zero, */
                /* go to next subframe.                                     */
                /*----------------------------------------------------------*/

                pswPitchBuf[siAnchorIndex] = LMIN_FR;
                pswHNWCoefBuf[siAnchorIndex] = 0;
                continue;
            }

            /* It is possible that by interpolating, the only positive */
            /* C(k) was made negative.  Check for this here            */
            /*---------------------------------------------------------*/

            if (siNumPeaks == 0)
            {

                pswPitchBuf[siAnchorIndex] = LMIN_FR;
                pswHNWCoefBuf[siAnchorIndex] = 0;
                continue;
            }

            /* Peak(s) found in C**2/G function: find the best delta-codeable */
            /* trajectory through each peak (unless that peak has already     */
            /* paritcipated in a trajectory) up to a total of NUM_TRAJ_MAX    */
            /* peaks.  After each trajectory is found, check to see if that   */
            /* trajectory is the best one so far.                             */
            /*----------------------------------------------------------------*/

            if (siNumPeaks > NUM_TRAJ_MAX)
                siNumTrajToDo = NUM_TRAJ_MAX;
            else
                siNumTrajToDo = siNumPeaks;

            while (siNumTrajToDo)
            {

                /* Check if this peak has already participated in a trajectory. */
                /* If so, skip it, decrement the number of trajectories yet to  */
                /* be evaluated, and go on to the next best peak                */
                /*--------------------------------------------------------------*/

                si1 = 0;
                for (i = 0; i < siTrajIndex; i++)
                {

                    if ( pswLPeaks[siPeakIndex] == ppswTrajLBuf[i][siAnchorIndex] )
                        si1 = 1;
                }

                if (si1)
                {

                    siNumTrajToDo--;
                    if (siNumTrajToDo)
                    {

                        siPeakIndex++;
                        continue;
                    }
                    else
                        break;
                }

                /* The current peak is not in a previous trajectory: find */
                /* the best trajectory through it.                        */
                /* */
                /* First, store the lag, C**2, and G for the peak in the  */
                /* trajectory storage buffer                              */
                /*--------------------------------------------------------*/

                ppswTrajLBuf[siTrajIndex][siAnchorIndex] = pswLPeaks[siPeakIndex];
                ppswTrajGBuf[siTrajIndex][siAnchorIndex] = pswGPeaks[siPeakIndex];
                ppswTrajCCBuf[siTrajIndex][siAnchorIndex] = MULT_R(pswCPeaks[siPeakIndex], pswCPeaks[siPeakIndex]);
                /*VPP_MULT_R(m_hi, m_lo, pswCPeaks[siPeakIndex], pswCPeaks[siPeakIndex]);
                ppswTrajCCBuf[siTrajIndex][siAnchorIndex] = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/

                /* Complete the part of the trajectory that extends forward */
                /* from the anchor subframe                                 */
                /*----------------------------------------------------------*/

                for (siFIndex = siAnchorIndex + 1; siFIndex < N_SUB; siFIndex++)
                {

                    /* Get array of lags which are delta-codeable */
                    /* */
                    /* First, get code for largest lag in array   */
                    /* (limit it)                                 */
                    /*--------------------------------------------*/

                    quantLag(ppswTrajLBuf[siTrajIndex][siFIndex - 1],
                             &si1);
                    //si2 = ADD_SAT16(si1, (DELTA_LEVELS / 2 - 1) - (NUM_CLOSED - 1));
                    si2 = ADD(si1, (((DELTA_LEVELS / 2) - 1) - (NUM_CLOSED - 1)));
                    if (  si2 > ((1 << L_BITS) - 1)  )
                        si2 = (1 << L_BITS) - 1;

                    /* Get code for smallest lag in array (limit it) */
                    /*-----------------------------------------------*/

                    si3 = SUB(si1, ((DELTA_LEVELS / 2) - (NUM_CLOSED - 1)));
                    if (si3 < 0)
                        si3 = 0;

                    /* Generate array of lags */
                    /*------------------------*/

                    for (i = si3, j = 0; i <= si2; i++, j++)
                        pswLArray[j] = psrLagTbl[i];

                    siNumDelta = ADD( SUB(si2, si3), 1);

                    /* Search array of delta-codeable lags for one which maximizes */
                    /* C**2/G                                                      */
                    /*-------------------------------------------------------------*/

                    bestDelta(pswLArray, ppswCSfrm[siFIndex], ppswGSfrm[siFIndex],
                              siNumDelta, siFIndex,
                              ppswTrajLBuf[siTrajIndex], ppswTrajCCBuf[siTrajIndex],
                              ppswTrajGBuf[siTrajIndex]);
                }

                /* Complete the part of the trajectory that extends backward */
                /* from the anchor subframe                                  */
                /*-----------------------------------------------------------*/

                for (siBIndex = siAnchorIndex - 1; siBIndex >= 0; siBIndex--)
                {

                    /* Get array of lags which are delta-codeable */
                    /* */
                    /* First, get code for largest lag in array   */
                    /* (limit it)                                 */
                    /*--------------------------------------------*/

                    quantLag(ppswTrajLBuf[siTrajIndex][siBIndex + 1],
                             &si1);
                    //si2 = ADD_SAT16(si1, (DELTA_LEVELS / 2) - (NUM_CLOSED - 1));
                    si2 = ADD(si1, ((DELTA_LEVELS / 2) - (NUM_CLOSED - 1)));
                    if ( si2 > ((1 << L_BITS) - 1)  )
                        si2 = (1 << L_BITS) - 1;

                    /* Get code for smallest lag in array (limit it) */
                    /*-----------------------------------------------*/

                    si3 = SUB(si1, ((DELTA_LEVELS / 2 - 1) - (NUM_CLOSED - 1)));
                    if (si3 < 0)
                        si3 = 0;

                    /* Generate array of lags */
                    /*------------------------*/

                    for (i = si3, j = 0; i <= si2; i++, j++)
                        pswLArray[j] = psrLagTbl[i];

                    //siNumDelta = ADD_SAT16(sub(si2, si3), 1);
                    siNumDelta = ADD(SUB(si2, si3), 1);

                    /* Search array of delta-codeable lags for one which maximizes */
                    /* C**2/G                                                      */
                    /*-------------------------------------------------------------*/

                    bestDelta(pswLArray, ppswCSfrm[siBIndex], ppswGSfrm[siBIndex],
                              siNumDelta, siBIndex,
                              ppswTrajLBuf[siTrajIndex], ppswTrajCCBuf[siTrajIndex],
                              ppswTrajGBuf[siTrajIndex]);
                }

                /* This trajectory done: check total C**2/G for this trajectory */
                /* against current best trajectory                              */
                /* */
                /* Get total C**2/G for this trajectory                         */
                /*--------------------------------------------------------------*/

                swTotalCCDivG = 0;
                for (i = 0; i < N_SUB; i++)
                {

                    swCC = ppswTrajCCBuf[siTrajIndex][i];
                    swG = ppswTrajGBuf[siTrajIndex][i];

                    if (swG <= 0)
                    {

                        /* Negative G (imperfect interpolation): do not include in */
                        /* total                                                   */
                        /*---------------------------------------------------------*/

                        swCCDivG = 0;
                    }
                    //else if (sub(abs_s(swCC), swG) > 0)
                    else if ( ABS_S(swCC) > swG  )
                    {

                        /* C**2/G > 0: limit quotient, add to total */
                        /*------------------------------------------*/

                        if (swCC > 0)
                            swCCDivG = SW_MAX;
                        else
                            swCCDivG = SW_MIN;

                        //swTotalCCDivG = ADD_SAT16(swTotalCCDivG, swCCDivG);
                        swTotalCCDivG = ADD(swTotalCCDivG, swCCDivG);

                    }
                    else
                    {

                        /* accumulate C**2/G */
                        /*-------------------*/

                        if (swCC < 0)
                        {

                            //swCCDivG = divide_s(negate(swCC), swG);
                            swCCDivG = DIVIDE_SAT16(NEGATE(swCC), swG);
                            swTotalCCDivG = SUB(swTotalCCDivG, swCCDivG);
                        }
                        else
                        {

                            //swCCDivG = divide_s(swCC, swG);
                            swCCDivG = DIVIDE_SAT16(swCC, swG);
                            //swTotalCCDivG = ADD_SAT16(swTotalCCDivG, swCCDivG);
                            swTotalCCDivG = ADD(swTotalCCDivG, swCCDivG);

                        }
                    }
                }

                /* Compare this trajectory with current best, update if better */
                /*-------------------------------------------------------------*/

                if ( swTotalCCDivG > swBestPG )
                {

                    swBestPG = swTotalCCDivG;
                    siBestTrajIndex = siTrajIndex;
                }

                /* Update trajectory index, peak index, decrement the number */
                /* of trajectories left to do.                               */
                /*-----------------------------------------------------------*/

                siTrajIndex++;
                siPeakIndex++;
                siNumTrajToDo--;
            }
        }

        if (siTrajIndex == 0)
        {

            /* No trajectories searched despite voiced designation: change */
            /* designation to unvoiced.                                    */
            /*-------------------------------------------------------------*/

            for (i = 0; i < N_SUB; i++)
            {

                pswNumLagList[i] = 0;
                pswLagList[i] = 0;
                pswPitchBuf[i] = 0;
                pswHNWCoefBuf[i] = 0;
            }

            *psiUVCode = 0;
        }
        else
        {

            /* Best trajectory determined: get voicing level, generate the */
            /* constrained list of lags to search in the adaptive codebook */
            /* for each subframe                                           */
            /* */
            /* First, get voicing level                                    */
            /*-------------------------------------------------------------*/

            *psiUVCode = 3;
            siLowestSoFar = 2;
            for (i = 0; i < N_SUB; i++)
            {

                /* Check this subframe against highest voicing threshold */
                /*-------------------------------------------------------*/

                swCC = ppswTrajCCBuf[siBestTrajIndex][i];
                swG = ppswTrajGBuf[siBestTrajIndex][i];

                swRG = MULT_R(swG, pswSfrmEng[i]);
                /*VPP_MULT_R(m_hi, m_lo, swG, pswSfrmEng[i]);
                swRG = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
                L_Voicing = L_DEPOSIT_H(swCC);
                s_lo=SHR( L_Voicing,1);
                //L_Voicing = L_mac(L_Voicing, swRG, UV_SCALE2);
                /*Lmult=L_MULT( swRG, UV_SCALE2);
                L_Voicing =L_ADD(L_Voicing ,Lmult);*/
                VPP_MLA16(s_hi, s_lo, swRG, UV_SCALE2);
                L_Voicing = L_MLA_SAT_CARRY(s_lo, L_Voicing&1);//L_MLA_SAT(s_hi, s_lo);

                if (L_Voicing < 0)
                {

                    /* Voicing for this subframe failed to meet 2/3 threshold:  */
                    /* therefore, voicing level for entire frame can only be as */
                    /* high as 2                                                */
                    /*----------------------------------------------------------*/

                    *psiUVCode = siLowestSoFar;

                    L_Voicing = L_DEPOSIT_H(swCC);
                    s_lo=SHR( L_Voicing,1);
                    //L_Voicing = L_mac(L_Voicing, swRG, UV_SCALE1);
                    /*Lmult=L_MULT( swRG, UV_SCALE1);
                    L_Voicing =L_ADD(L_Voicing ,Lmult);*/
                    VPP_MLA16(s_hi, s_lo, swRG, UV_SCALE1);
                    L_Voicing = L_MLA_SAT_CARRY(s_lo, L_Voicing&1);//L_MLA_SAT(s_hi, s_lo);

                    if (L_Voicing < 0)
                    {

                        /* Voicing for this subframe failed to meet 1/2 threshold: */
                        /* therefore, voicing level for entire frame can only be   */
                        /* as high as 1                                            */
                        /*---------------------------------------------------------*/

                        *psiUVCode = siLowestSoFar = 1;
                    }
                }
            }

            /* Generate list of lags to be searched in closed-loop */
            /*-----------------------------------------------------*/

            siLagsSoFar = 0;
            for (i = 0; i < N_SUB; i++)
            {

                quantLag(ppswTrajLBuf[siBestTrajIndex][i], &si1);

                //si2 = ADD_SAT16(si1, NUM_CLOSED / 2);
                si2 = ADD(si1, (NUM_CLOSED / 2));
                if ( si2 > ((1 << L_BITS) - 1)  )
                    si2 = (1 << L_BITS) - 1;

                si3 = SUB(si1, (NUM_CLOSED / 2));
                if (si3 < 0)
                    si3 = 0;

                pswNumLagList[i] = ADD ( SUB(si2, si3), 1);

                for (j = siLagsSoFar; j < siLagsSoFar + pswNumLagList[i]; j++)
                    pswLagList[j] = psrLagTbl[si3++];

                siLagsSoFar += pswNumLagList[i];
            }
        }
    }

    VPP_HR_PROFILE_FUNCTION_EXIT( openLoopLagSearch);

}                                      /* end of openLoopLagSearch */

/***************************************************************************
 *
 *   FUNCTION NAME: pitchLags
 *
 *   PURPOSE:
 *
 *     Locates peaks in the interpolated C(k)*C(k)/G(k) sequence for a
 *     subframe which exceed a given threshold. Also determines the
 *     fundamental pitch, and a harmonic-noise-weighting coefficient.
 *
 *   INPUTS:
 *
 *     swBestIntLag
 *
 *                     The integer lag for which CC/G is maximum.
 *
 *     pswIntCs[0:127]
 *
 *                     The C(k) sequence for the subframe, with k an integer.
 *
 *     pswIntGs[0:127]
 *
 *                     The G(k) sequence for the subframe, with k an integer.
 *
 *     swCCThreshold
 *
 *                     The CC/G threshold which peaks must exceed (G is
 *                     understood to be 0.5).
 *
 *     psrLagTbl[0:255]
 *
 *                     The lag quantization table.
 *
 *
 *   OUTPUTS:
 *
 *     pswLPeaksSorted[0:10(max)]
 *
 *                     List of fractional lags where CC/G peaks, highest
 *                     peak first.
 *
 *     pswCPeaksSorted[0:10(max)]
 *
 *                     List of C's where CC/G peaks.
 *
 *     pswGPeaksSorted[0:10(max)]
 *
 *                     List of G's where CC/G peaks.
 *
 *     psiNumSorted
 *
 *                     Number of peaks found.
 *
 *     pswPitch
 *
 *                     The fundamental pitch for current subframe.
 *
 *     pswHNWCoef
 *
 *                     The harmonic-noise-weighting coefficient for the
 *                     current subframe.
 *
 *   RETURN VALUE:
 *
 *     None
 *
 *   DESCRIPTION:
 *
 *
 *   REFERENCE:  Sub-clauses 4.1.9, 4.1.8.2 of GSM Recommendation 06.20
 *
 *   KEYWORDS: pitchLags, pitchlags, PITCH_LAGS
 *
 *************************************************************************/

void   pitchLags(INT16 swBestIntLag,
                 INT16 pswIntCs[],
                 INT16 pswIntGs[],
                 INT16 swCCThreshold,
                 INT16 pswLPeaksSorted[],
                 INT16 pswCPeaksSorted[],
                 INT16 pswGPeaksSorted[],
                 INT16 *psiNumSorted,
                 INT16 *pswPitch,
                 INT16 *pswHNWCoef)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswLBuf[2 * OS_FCTR - 1],
          pswCBuf[2 * OS_FCTR - 1];
    INT16 pswGBuf[2 * OS_FCTR - 1],
          pswLPeaks[2 * LMAX / LMIN];
    INT16 pswCPeaks[2 * LMAX / LMIN],
          pswGPeaks[2 * LMAX / LMIN];
    short  siLPeakIndex,
           siCPeakIndex,
           siGPeakIndex,
           siPeakIndex;
    short  siSortedIndex,
           siLPeaksSortedInit,
           swWorkingLag;
    INT16 swSubMult,
          swFullResPeak,
          swCPitch,
          swGPitch,
          swMult;
    INT16 swMultInt,
          sw1,
          sw2,
          si1,
          si2;
    INT32 L_1;
    short  siNum,
           siUpperBound,
           siLowerBound,
           siSMFIndex;
    short  siNumPeaks,
           siRepeat,
           i,
           j;

    static INT16 psrSubMultFactor[] = {0x0aab,     /* 1.0/12.0 */
                                       0x071c,                            /* 1.0/18.0 */
                                       0x0555,                            /* 1.0/24.0 */
                                       0x0444,                            /* 1.0/30.0 */
                                       0x038e
                                      };                             /* 1.0/36.0 */


    /*register INT32  m_hi=0;
    register UINT32 m_lo=0;*/
    //INT16 essai;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Get array of valid lags within one integer lag of the best open-loop */
    /* integer lag; get the corresponding interpolated C and G arrays;      */
    /* find the best among these; store the info corresponding to this peak */
    /* in the interpolated CC/G sequence                                    */
    /*----------------------------------------------------------------------*/

    //sw1 = shr(EXTRACT_L(L_MULT(swBestIntLag, OS_FCTR)), 1);
    sw1 = SHR(EXTRACT_L(L_MULT(swBestIntLag, OS_FCTR)), 1);

    siNum = CGInterpValid(sw1, pswIntCs, pswIntGs,
                          pswLBuf, pswCBuf, pswGBuf);

    sw1 = 0;
    sw2 = 0x003f;
    siPeakIndex = fnBest_CG(pswCBuf, pswGBuf, &sw1, &sw2, siNum);
    if (siPeakIndex == -1)
    {

        /* It is possible, although rare, that the interpolated sequence */
        /* will not have a peak where the original sequence did.         */
        /* Indicate this on return                                       */
        /*---------------------------------------------------------------*/

        *psiNumSorted = 0;
        return;
    }

    siLPeakIndex = 0;
    siCPeakIndex = 0;
    siGPeakIndex = 0;
    siSortedIndex = 0;
    pswCPeaks[siCPeakIndex] = pswCBuf[siPeakIndex];
    //siCPeakIndex = ADD_SAT16(siCPeakIndex, 1);
    siCPeakIndex++;// = ADD(siCPeakIndex, 1);
    pswLPeaks[siLPeakIndex] = pswLBuf[siPeakIndex];
    //siLPeakIndex = ADD_SAT16(siLPeakIndex, 1);
    siLPeakIndex++;// = ADD(siLPeakIndex, 1);
    pswGPeaks[siGPeakIndex] = pswGBuf[siPeakIndex];
    //siGPeakIndex = ADD_SAT16(siGPeakIndex, 1);
    siGPeakIndex++;// = ADD(siGPeakIndex, 1);

    /* Find all peaks at submultiples of the first peak */
    /*--------------------------------------------------*/

    siSMFIndex = 0;
    swSubMult=MULT_R(pswLPeaks[0], psrSubMultFactor[siSMFIndex]);
    siSMFIndex++;
    while ( (swSubMult>= LMIN)  && (siSMFIndex <= 5))
    {

        /* Check if there is peak in the integer CC/G sequence within */
        /* PEAK_VICINITY of the submultiple                           */
        /*------------------------------------------------------------*/

        swFullResPeak = findPeak(swSubMult, pswIntCs, pswIntGs);

        if (swFullResPeak)
        {

            /* Peak found at submultiple: interpolate to get C's and G's */
            /* corresponding to valid lags within one of the new found   */
            /* peak; get best C**2/G from these;  if it meets threshold, */
            /* store the info corresponding to this peak                 */
            /*-----------------------------------------------------------*/

            siNum = CGInterpValid(swFullResPeak, pswIntCs, pswIntGs,
                                  pswLBuf, pswCBuf, pswGBuf);

            sw1 = swCCThreshold;
            sw2 = 0x4000;
            siPeakIndex = fnBest_CG(pswCBuf, pswGBuf, &sw1, &sw2, siNum);
            if (siPeakIndex != -1)
            {

                pswCPeaks[siCPeakIndex] = pswCBuf[siPeakIndex];
                //siCPeakIndex = ADD_SAT16(siCPeakIndex, 1);
                siCPeakIndex++;
                pswLPeaks[siLPeakIndex] = pswLBuf[siPeakIndex];
                //siLPeakIndex = ADD_SAT16(siLPeakIndex, 1);
                siLPeakIndex++;
                pswGPeaks[siGPeakIndex] = pswGBuf[siPeakIndex];
                //siGPeakIndex = ADD_SAT16(siGPeakIndex, 1);
                siGPeakIndex++;

            }
        }


        if (siSMFIndex < 5)
        {

            /* Get next submultiple */
            /*----------------------*/
            swSubMult = MULT_R(pswLPeaks[0], psrSubMultFactor[siSMFIndex]);
            /*VPP_MULT_R(m_hi, m_lo, pswLPeaks[0], psrSubMultFactor[siSMFIndex]);
            swSubMult = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) );*/

        }

        siSMFIndex++;
    }

    /* Get pitch from fundamental peak: first, build array of fractional */
    /* lags around which to search for peak.  Note that these lags are   */
    /* NOT restricted to those in the lag table, but may take any value  */
    /* in the range LMIN_FR to LMAX_FR                                   */
    /*-------------------------------------------------------------------*/

    //siUpperBound = ADD_SAT16(pswLPeaks[siLPeakIndex - 1], OS_FCTR);
    siUpperBound = ADD(pswLPeaks[siLPeakIndex - 1], OS_FCTR);
    //siUpperBound = sub(siUpperBound, 1);
    siUpperBound--;// = SUB(siUpperBound, 1);
    if (  siUpperBound > LMAX_FR  )
        siUpperBound = LMAX_FR;

    //siLowerBound = sub(pswLPeaks[siLPeakIndex - 1], OS_FCTR);
    siLowerBound = SUB(pswLPeaks[siLPeakIndex - 1], OS_FCTR);
    //siLowerBound = ADD_SAT16(siLowerBound, 1);
    siLowerBound = ADD(siLowerBound, 1);
    if ( siLowerBound < LMIN_FR )
        siLowerBound = LMIN_FR;
    for (i = siLowerBound, j = 0; i <= siUpperBound; i++, j++)
        pswLBuf[j] = i;

    /* Second, find peak in the interpolated CC/G sequence. */
    /* The corresponding lag is the fundamental pitch.  The */
    /* interpolated C(pitch) and G(pitch) values are stored */
    /* for later use in calculating the Harmonic-Noise-     */
    /* Weighting coefficient                                */
    /*------------------------------------------------------*/

    //siNum = sub(siUpperBound, siLowerBound);
    siNum = SUB (siUpperBound, siLowerBound);
    //siNum = ADD_SAT16(siNum, 1);
    siNum++;// = ADD(siNum, 1);
    CGInterp(pswLBuf, siNum, pswIntCs, pswIntGs, LSMIN,
             pswCBuf, pswGBuf);
    sw1 = 0;
    sw2 = 0x003f;
    siPeakIndex = fnBest_CG(pswCBuf, pswGBuf, &sw1, &sw2, siNum);
    if (siPeakIndex == -1)
    {
        swCPitch = 0;
        *pswPitch = LMIN_FR;
        swGPitch = 0x003f;
    }
    else
    {
        swCPitch = pswCBuf[siPeakIndex];
        *pswPitch = pswLBuf[siPeakIndex];
        swGPitch = pswGBuf[siPeakIndex];
    }

    /* Find all peaks which are multiples of fundamental pitch */
    /*---------------------------------------------------------*/

    //swMult = ADD_SAT16(*pswPitch, *pswPitch);
    swMult = ADD(*pswPitch, *pswPitch);
    swMultInt = MULT_R(swMult, INV_OS_FCTR);

    while ( swMultInt <= LMAX )
    {

        /* Check if there is peak in the integer CC/G sequence within */
        /* PEAK_VICINITY of the multiple                              */
        /*------------------------------------------------------------*/

        swFullResPeak = findPeak(swMultInt, pswIntCs, pswIntGs);

        if (swFullResPeak)
        {

            /* Peak found at multiple: interpolate to get C's and G's    */
            /* corresponding to valid lags within one of the new found   */
            /* peak; get best C**2/G from these;  if it meets threshold, */
            /* store the info corresponding to this peak                 */
            /*-----------------------------------------------------------*/

            siNum = CGInterpValid(swFullResPeak, pswIntCs, pswIntGs,
                                  pswLBuf, pswCBuf, pswGBuf);

            sw1 = swCCThreshold;
            sw2 = 0x4000;
            siPeakIndex = fnBest_CG(pswCBuf, pswGBuf, &sw1, &sw2, siNum);
            if (siPeakIndex != -1)
            {

                pswCPeaks[siCPeakIndex] = pswCBuf[siPeakIndex];
                //siCPeakIndex = ADD_SAT16(siCPeakIndex, 1);
                siCPeakIndex = ADD(siCPeakIndex, 1);
                pswLPeaks[siLPeakIndex] = pswLBuf[siPeakIndex];
                //siLPeakIndex = ADD_SAT16(siLPeakIndex, 1);
                siLPeakIndex = ADD(siLPeakIndex, 1);
                pswGPeaks[siGPeakIndex] = pswGBuf[siPeakIndex];
                //siGPeakIndex = ADD_SAT16(siGPeakIndex, 1);
                siGPeakIndex = ADD(siGPeakIndex, 1);

            }
        }

        /* Get next multiple */
        /*-------------------*/

        //swMult = ADD_SAT16(*pswPitch, swMult);
        swMult += (*pswPitch);
        swMultInt = MULT_R(swMult, INV_OS_FCTR);
        /*VPP_MULT_R(m_hi, m_lo, swMult, INV_OS_FCTR);
        swMultInt = EXTRACT_H( VPP_SCALE64_TO_16(m_hi,m_lo) );//EXTRACT_H( SHL(m_lo,1) )*/
    }

    /* Get Harmonic-Noise-Weighting coefficient = 0.4 * C(pitch) / G(pitch) */
    /* Note: a factor of 0.5 is applied the the HNW coeffcient              */
    /*----------------------------------------------------------------------*/

    si2 = norm_s(swCPitch);
    sw1 = SHL(swCPitch, si2);
    L_1 = L_MULT(sw1, PW_FRAC);

    si1 = norm_s(swGPitch);
    sw1 = SHL(swGPitch, si1);

    //sw2 = ROUNDO(L_shr(L_1, 1));
    sw2 = ROUNDO(SHR(L_1, 1));
    //sw2 = divide_s(sw2, sw1);
    sw2 = DIVIDE_SAT16(sw2, sw1);

    //if (sub(si1, si2) > 0)
    if ( si1 > si2 )
    {
        //sw2 = shl(sw2, SUB(si1, si2));
        sw2 = SHL_g(sw2, SUB(si1, si2));
        //essai = SATURATE( SHL(sw2, SUB(si1, si2)));
        /*if (sw2!=essai)
        {
            essai = 0;
        }*/
    }

    if ( si1 < si2 )
        sw2 = SHIFT_R(sw2, SUB(si1, si2));

    *pswHNWCoef = sw2;

    /* Sort peaks into output arrays, largest first */
    /*----------------------------------------------*/

    siLPeaksSortedInit = siSortedIndex;
    *psiNumSorted = 0;
    siNumPeaks = siLPeakIndex;
    for (i = 0; i < siNumPeaks; i++)
    {

        sw1 = 0;
        sw2 = 0x003f;
        siPeakIndex = fnBest_CG(pswCPeaks, pswGPeaks, &sw1, &sw2, siNumPeaks);

        siRepeat = 0;
        swWorkingLag = pswLPeaks[siPeakIndex];
        for (j = 0; j < *psiNumSorted; j++)
        {

            if ( swWorkingLag == pswLPeaksSorted[j + siLPeaksSortedInit] )
                siRepeat = 1;
        }

        if (!siRepeat)
        {

            pswLPeaksSorted[siSortedIndex] = swWorkingLag;
            pswCPeaksSorted[siSortedIndex] = pswCPeaks[siPeakIndex];
            pswGPeaksSorted[siSortedIndex] = pswGPeaks[siPeakIndex];
            //siSortedIndex = ADD_SAT16(siSortedIndex, 1);
            siSortedIndex++;
            //*psiNumSorted = ADD_SAT16(*psiNumSorted, 1);
            (*psiNumSorted)++;
        }

        pswCPeaks[siPeakIndex] = 0x0;
    }
}                                      /* end of pitchLags */



/***************************************************************************
 *
 *   FUNCTION NAME: quantLag
 *
 *   PURPOSE:
 *
 *     Quantizes a given fractional lag according to the provided table
 *     of allowable fractional lags.
 *
 *   INPUTS:
 *
 *     swRawLag
 *
 *                     Raw lag value: a fractional lag value*OS_FCTR.
 *
 *     psrLagTbl[0:255]
 *
 *                     Fractional lag table.
 *
 *   OUTPUTS:
 *
 *     pswCode
 *
 *                     Index in lag table of the quantized lag-- that is,
 *                     the coded value of the lag.
 *
 *   RETURN VALUE:
 *
 *     Quantized lag value.
 *
 *
 *   REFERENCE:  Sub-clause 4.1.8.3 of GSM Recommendation 06.20
 *
 *   KEYWORDS: quantization, LTP, adaptive codebook
 *
 *************************************************************************/

INT16 quantLag(INT16 swRawLag,
               INT16 *pswCode)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */
    INT16 siOffset,
          swIndex1,
          swIndex2;

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    swIndex1 = 0;
    siOffset = SHR(LAG_TABLE_LEN, 1);
    swIndex2 = siOffset;
    siOffset = SHR(siOffset, 1);

    while ( swIndex2 != swIndex1 )
    {
        if ( swRawLag >= psrLagTbl[swIndex2] )
            swIndex1 = swIndex2;
        //swIndex2 = ADD_SAT16(swIndex1, siOffset);
        swIndex2 = ADD(swIndex1, siOffset);
        siOffset = SHR(siOffset, 1);
    }
    *pswCode = swIndex2;

    return (psrLagTbl[swIndex2]);

}                                      /* end of quantLag */

/***************************************************************************
 *
 *   FUNCTION NAME: r0Quant
 *
 *   PURPOSE:
 *
 *      Quantize the unquantized R(0).  Returns r0 codeword (index).
 *
 *   INPUTS:
 *
 *     L_UnqntzdR0
 *                     The average frame energy R(0)
 *
 *   OUTPUTS: none
 *
 *   RETURN VALUE:
 *
 *                     A 16 bit number representing the codeword whose
 *                     associated R(0) is closest to the input frame energy.
 *
 *   DESCRIPTION:
 *
 *     Returns r0 codeword (index) not the actual Rq(0).
 *
 *     Level compare input with boundary value (the boundary
 *     above ,louder) of candidate r0Index i.e.
 *     lowerBnd[i] <= inputR(0) < upperBnd[i+1]
 *
 *     The compare in the routine is:
 *     inputR(0) < upperBnd[i+1] if false return i as codeword
 *
 *   REFERENCE:  Sub-clause 4.1.3 of GSM Recommendation 06.20
 *
 *
 *************************************************************************/

INT16 r0Quant(INT32 L_UnqntzdR0)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 swR0Index,
          swUnqntzdR0;

    /*_________________________________________________________________________
     |                                                                         |
     |                            Executable Code                              |
     |_________________________________________________________________________|
    */

    swUnqntzdR0 = ROUNDO(L_UnqntzdR0);

    for (swR0Index = 0; swR0Index < (1 << R0BITS) - 1; swR0Index++)
    {
        if ( swUnqntzdR0 < psrR0DecTbl[2 * swR0Index + 1] )
        {
            return (swR0Index);
        }
    }
    return ((1 << R0BITS) - 1);          /* return the maximum */
}

/***************************************************************************
 *
 *   FUNCTION NAME: setupPreQ
 *
 *   PURPOSE:
 *     The purpose of this function is to setup the internal pointers so that
 *     getNextVec knows where to start.
 *
 *   INPUTS: iSeg, iVector
 *
 *   OUTPUTS: None
 *
 *   RETURN VALUE: none
 *
 *   DESCRIPTION:
 *
 *   REFERENCE:  Sub-clause  4.1.4.1 of GSM Recommendation 06.20
 *
 *   KEYWORDS:  vector quantizer preQ
 *
 *************************************************************************/

void   setupPreQ(int iSeg, int iVector)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    iLow = psvqIndex[iSeg - 1].l;
    iThree = ((psvqIndex[iSeg - 1].h - iLow) == 2);

    switch (iSeg)
    {
        case 1:
        {
            psrTable = psrPreQ1;
            iWordPtr = (iVector * 3) >> 1;
            if (odd(iVector))
                iWordHalfPtr = LOW;
            else
                iWordHalfPtr = HIGH;
            break;
        }

        case 2:
        {
            psrTable = psrPreQ2;
            iWordPtr = (iVector * 3) >> 1;
            if (odd(iVector))
                iWordHalfPtr = LOW;
            else
                iWordHalfPtr = HIGH;
            break;
        }

        case 3:
        {
            psrTable = psrPreQ3;
            iWordPtr = iVector * 2;
            iWordHalfPtr = HIGH;
            break;
        }
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: setupQuant
 *
 *   PURPOSE:
 *     The purpose of this function is to setup the internal pointers so that
 *     getNextVec knows where to start.
 *
 *
 *   INPUTS: iSeg, iVector
 *
 *   OUTPUTS: None
 *
 *   RETURN VALUE: none
 *
 *   DESCRIPTION:
 *
 *   REFERENCE:  Sub-clause 4.1.4.1 of GSM Recommendation 06.20
 *
 *   KEYWORDS:  vector quantizer Quant
 *
 *************************************************************************/

void   setupQuant(int iSeg, int iVector)
{

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    iLow = psvqIndex[iSeg - 1].l;
    iThree = ((psvqIndex[iSeg - 1].h - iLow) == 2);

    switch (iSeg)
    {
        case 1:
        {
            psrTable = psrQuant1;
            iWordPtr = (iVector * 3) >> 1;
            if (odd(iVector))
                iWordHalfPtr = LOW;
            else
                iWordHalfPtr = HIGH;
            break;
        }

        case 2:
        {
            psrTable = psrQuant2;
            iWordPtr = (iVector * 3) >> 1;
            if (odd(iVector))
                iWordHalfPtr = LOW;
            else
                iWordHalfPtr = HIGH;
            break;
        }

        case 3:
        {
            psrTable = psrQuant3;
            iWordPtr = iVector * 2;
            iWordHalfPtr = HIGH;
            break;
        }
    }
}

/***************************************************************************
 *
 *   FUNCTION NAME: weightSpeechFrame
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform the spectral
 *     weighting filter  (W(z)) of the input speech frame.
 *
 *   INPUTS:
 *
 *     pswSpeechFrm[0:F_LEN]
 *
 *                     high pass filtered input speech frame
 *
 *     pswWNumSpace[0:NP*N_SUB]
 *
 *                     W(z) numerator coefficients
 *
 *     pswWDenomSpace[0:NP*N_SUB]
 *
 *                     W(z) denominator coefficients
 *
 *     pswWSpeechBuffBase[0:F_LEN+LMAX+CG_INT_MACS/2]
 *
 *                     previous W(z) filtered speech
 *
 *   OUTPUTS:
 *
 *     pswWSpeechBuffBase[0:F_LEN+LMAX+CG_INT_MACS/2]
 *
 *                     W(z) filtered speech frame
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *   DESCRIPTION:
 *
 *   REFERENCE:  Sub-clause 4.1.7 of GSM Recommendation 06.20
 *
 *   KEYWORDS:
 *
 *************************************************************************/

void   weightSpeechFrame(INT16 pswSpeechFrm[],
                         INT16 pswWNumSpace[],
                         INT16 pswWDenomSpace[],
                         INT16 pswWSpeechBuffBase[])
{

    /*_________________________________________________________________________
     |                                                                         |
     |                            Automatic Variables                          |
     |_________________________________________________________________________|
    */

    INT16 pswScratch0[W_F_BUFF_LEN],
          *pswWSpeechFrm;
    short int siI;


    VPP_HR_PROFILE_FUNCTION_ENTER(weightSpeechFrame );

    /*_________________________________________________________________________
     |                                                                         |
     |                              Executable Code                            |
     |_________________________________________________________________________|
    */

    /* Delay the weighted speech buffer by one frame */
    /* --------------------------------------------- */

    for (siI = 0; siI < LSMAX; siI++)
    {
        pswWSpeechBuffBase[siI] = pswWSpeechBuffBase[siI + F_LEN];
    }

    /* pass speech frame through W(z) */
    /* ------------------------------ */

    pswWSpeechFrm = pswWSpeechBuffBase + LSMAX;

    for (siI = 0; siI < N_SUB; siI++)
    {
        lpcFir(&pswSpeechFrm[siI * S_LEN], &pswWNumSpace[siI * NP],
               pswWStateNum, &pswScratch0[siI * S_LEN]);
    }

    for (siI = 0; siI < N_SUB; siI++)
    {
        lpcIir(&pswScratch0[siI * S_LEN], &pswWDenomSpace[siI * NP],
               pswWStateDenom, &pswWSpeechFrm[siI * S_LEN]);
    }

    VPP_HR_PROFILE_FUNCTION_EXIT( weightSpeechFrame);


}

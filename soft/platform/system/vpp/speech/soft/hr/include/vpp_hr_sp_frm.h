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

#ifndef __SP_FRM
#define __SP_FRM

#include "vpp_hr_typedefs.h"
#include "vpp_hr_sp_rom.h"


struct QuantList
{
    /* structure which points to the beginning of a block of candidate vq
     * vectors.  It also stores the residual error for each vector. */
    int    iNum;                         /* total number in list */
    int    iRCIndex;                     /* an index to the first vector of the
                                        * block */
    INT16 pswPredErr[PREQ1_NUM_OF_ROWS];  /* PREQ1 is the biggest block */
};

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

void   iir_d(INT16 pswCoeff[], INT16 pswIn[],
             INT16 pswXstate[],
             INT16 pswYstate[],
             int npts, int shifts,
             INT16 swPreFirDownSh,
             INT16 swFinalUpShift);


void   filt4_2nd(INT16 pswCoeff[],
                 INT16 pswIn[],
                 INT16 pswXstate[],
                 INT16 pswYstate[],
                 int npts,
                 int shifts);

void   initPBarVBarL(INT32 pL_PBarFull[],
                     INT16 pswPBar[],
                     INT16 pswVBar[]);

void   initPBarFullVBarFullL(INT32 pL_CorrelSeq[],
                             INT32 pL_PBarFull[],
                             INT32 pL_VBarFull[]);

INT16 aflatRecursion(INT16 pswQntRc[],
                     INT16 pswPBar[],
                     INT16 pswVBar[],
                     INT16 *ppswPAddrs[],
                     INT16 *ppswVAddrs[],
                     INT16 swSegmentOrder);

void   aflatNewBarRecursionL(INT16 pswQntRc[],
                             int iSegment,
                             INT32 pL_PBar[],
                             INT32 pL_VBar[],
                             INT16 pswPBar[],
                             INT16 pswVBar[]);


void   setupPreQ(int iSeg, int iVector);

void   setupQuant(int iSeg, int iVector);

void   getNextVec(INT16 pswRc[]);

void   aflat(INT16 pswSpeechToLPC[],
             int piR0Index[],
             INT16 pswFinalRc[],
             int piVQCodewds[],
             INT16 swPtch,
             INT16 *pswVadFlag,
             INT16 *pswSP);


INT16 fnExp2(INT32 L_Input);

INT16 fnLog2(INT32 L_Input);

void   weightSpeechFrame(INT16 pswSpeechFrm[],
                         INT16 pswWNumSpace[],
                         INT16 pswWDenomSpace[],
                         INT16 pswWSpeechBuffBase[]);

void   getSfrmLpcTx(INT16 swPrevR0, INT16 swNewR0,
                    INT16 pswPrevFrmKs[],
                    INT16 pswPrevFrmAs[],
                    INT16 pswPrevFrmSNWCoef[],
                    INT16 pswNewFrmKs[],
                    INT16 pswNewFrmAs[],
                    INT16 pswNewFrmSNWCoef[],
                    INT16 pswHPFSpeech[],
                    short *pswSoftInterp,
                    struct NormSw *psnsSqrtRs,
                    INT16 ppswSynthAs[][NP],
                    INT16 ppswSNWCoefAs[][NP]);

short int fnBest_CG(INT16 pswCframe[],
                    INT16 pswGframe[],
                    INT16 *pswCmaxSqr,
                    INT16 *pswGmax,
                    short int siNumPairs);

short  compResidEnergy(INT16 pswSpeech[],
                       INT16 ppswInterpCoef[][NP],
                       INT16 pswPreviousCoef[],
                       INT16 pswCurrentCoef[],
                       struct NormSw psnsSqrtRs[]);

INT16 r0Quant(INT32 L_UnqntzdR0);

INT16 cov32(INT16 pswIn[],
            INT32 pppL_B[NP][NP][2],
            INT32 pppL_F[NP][NP][2],
            INT32 pppL_C[NP][NP][2],
            INT32 *pL_R0,
            INT32 pL_VadAcf[],
            INT16 *pswVadScalAuto);

INT32 flat(INT16 pswSpeechIn[],
           INT16 pswRc[],
           int *piR0Inx,
           INT32 pL_VadAcf[],
           INT16 *pswVadScalAuto);



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
                         INT16 swSP);

INT16 getCCThreshold(INT16 swRp0,
                     INT16 swCC,
                     INT16 swG);

void   pitchLags(INT16 swBestIntLag,
                 INT16 pswIntCs[],
                 INT16 pswIntGs[],
                 INT16 swCCThreshold,
                 INT16 pswLPeaksSorted[],
                 INT16 pswCPeaksSorted[],
                 INT16 pswGPeaksSorted[],
                 INT16 *psiNumSorted,
                 INT16 *pswPitch,
                 INT16 *pswHNWCoef);

short  CGInterpValid(INT16 swFullResLag,
                     INT16 pswCIn[],
                     INT16 pswGIn[],
                     INT16 pswLOut[],
                     INT16 pswCOut[],
                     INT16 pswGOut[]);

void   CGInterp(INT16 pswLIn[],
                short siNum,
                INT16 pswCIn[],
                INT16 pswGIn[],
                short siLoIntLag,
                INT16 pswCOut[],
                INT16 pswGOut[]);

INT16 quantLag(INT16 swRawLag,
               INT16 *psiCode);

void   findBestInQuantList(struct QuantList psqlInList,
                           int iNumVectOut,
                           struct QuantList psqlBestOutList[]);

INT16 findPeak(INT16 swSingleResLag,
               INT16 pswCIn[],
               INT16 pswGIn[]);

void   bestDelta(INT16 pswLagList[],
                 INT16 pswCSfrm[],
                 INT16 pswGSfrm[],
                 short int siNumLags,
                 short int siSfrmIndex,
                 INT16 pswLTraj[],
                 INT16 pswCCTraj[],
                 INT16 pswGTraj[]);

INT16
maxCCOverGWithSign(INT16 pswCIn[],
                   INT16 pswGIn[],
                   INT16 *pswCCMax,
                   INT16 *pswGMax,
                   INT16 swNum);

void   getNWCoefs(INT16 pswACoefs[],
                  INT16 pswHCoefs[]);

#endif

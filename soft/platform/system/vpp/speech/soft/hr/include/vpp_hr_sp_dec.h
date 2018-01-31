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

#ifndef __SP_DEC
#define __SP_DEC

#include "vpp_hr_typedefs.h"
#include "hal_speech.h"
/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

void   vpp_hr_decode(HAL_SPEECH_DEC_IN_T DecIn, HAL_SPEECH_PCM_HALF_BUF_T DecOut);
//void   speechDecoder(INT16 pswParameters[], INT16 pswDecodedSpeechFrame[]);

void   aFlatRcDp(INT32 *pL_R, INT16 *pswRc);

void   b_con(INT16 swCodeWord, short siNumBits,
             INT16 pswVectOut[]);

void   fp_ex(INT16 swOrigLagIn, INT16 pswLTPState[]);

INT16 g_corr1(INT16 *pswIn, INT32 *pL_out);

INT16 g_corr1s(INT16 pswIn[], INT16 swEngyRShft,
               INT32 *pL_out);

void   getSfrmLpc(short int siSoftInterpolation,
                  INT16 swPrevR0, INT16 swNewR0,
                  INT16 pswPrevFrmKs[],
                  INT16 pswPrevFrmAs[],
                  INT16 pswPrevFrmPFNum[],
                  INT16 pswPrevFrmPFDenom[],
                  INT16 pswNewFrmKs[],
                  INT16 pswNewFrmAs[],
                  INT16 pswNewFrmPFNum[],
                  INT16 pswNewFrmPFDenom[],
                  struct NormSw *psnsSqrtRs,
                  INT16 *ppswSynthAs[],
                  INT16 *ppswPFNumAs[],
                  INT16 *ppswPFDenomAs[]);

void   get_ipjj(INT16 swLagIn,
                INT16 *pswIp, INT16 *pswJj);

short int interpolateCheck(INT16 pswRefKs[],
                           INT16 pswRefCoefsA[],
                           INT16 pswOldCoefsA[],
                           INT16 pswNewCoefsA[],
                           INT16 swOldPer,
                           INT16 swNewPer,
                           INT16 swRq,
                           struct NormSw *psnsSqrtRsOut,
                           INT16 pswCoefOutA[]);

void   lpcFir(INT16 pswInput[], INT16 pswCoef[],
              INT16 pswState[], INT16 pswFiltOut[]);

void   lpcIir(INT16 pswInput[], INT16 pswCoef[],
              INT16 pswState[], INT16 pswFiltOut[]);

void   lpcIrZsIir(INT16 pswCoef[], INT16 pswFiltOut[]);

void   lpcZiIir(INT16 pswCoef[], INT16 pswState[],
                INT16 pswFiltOut[]);

void   lpcZsFir(INT16 pswInput[], INT16 pswCoef[],
                INT16 pswFiltOut[]);

void   lpcZsIir(INT16 pswInput[], INT16 pswCoef[],
                INT16 pswFiltOut[]);

void   lpcZsIirP(INT16 pswCommonIO[], INT16 pswCoef[]);

INT16 r0BasedEnergyShft(INT16 swR0Index);

short  rcToADp(INT16 swAscale, INT16 pswRc[],
               INT16 pswA[]);

void   rcToCorrDpL(INT16 swAshift, INT16 swAscale,
                   INT16 pswRc[], INT32 pL_R[]);

void   res_eng(INT16 pswReflecCoefIn[], INT16 swRq,
               struct NormSw *psnsSqrtRsOut);

void   rs_rr(INT16 pswExcitation[], struct NormSw snsSqrtRs,
             struct NormSw *snsSqrtRsRr);

void   rs_rrNs(INT16 pswExcitation[], struct NormSw snsSqrtRs,
               struct NormSw *snsSqrtRsRr);

INT16 scaleExcite(INT16 pswVect[],
                  INT16 swErrTerm, struct NormSw snsRS,
                  INT16 pswScldVect[]);

INT16 sqroot(INT32 L_SqrtIn);

void   v_con(INT16 pswBVects[], INT16 pswOutVect[],
             INT16 pswBitArray[], short int siNumBVctrs);

#endif

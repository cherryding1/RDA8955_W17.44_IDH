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

#ifndef __DTX
#define __DTX

#include "vpp_hr_typedefs.h"
#include "cs_types.h"


#define PN_INIT_SEED (INT32)0x1091988L       /* initial seed for Comfort
                                                 * noise pn-generator */

#define CNINTPER    12                 /* inperpolation period of CN
                                        * parameters */

#define SPEECH      1
#define CNIFIRSTSID 2
#define CNICONT     3
#define CNIBFI      4

#define VALIDSID    11
#define INVALIDSID  22
#define GOODSPEECH  33
#define UNUSABLE    44

/*________________________________________________________________________
 |                                                                        |
 |                      Function Prototypes                               |
 |________________________________________________________________________|
*/

void   avgCNHist(INT32 pL_R0History[],
                 INT32 ppL_CorrHistory[OVERHANG][NP + 1],
                 INT32 *pL_AvgdR0,
                 INT32 pL_AvgdCorrSeq[]);

void   avgGsHistQntz(INT32 pL_GsHistory[], INT32 *pL_GsAvgd);

INT16  swComfortNoise(INT16 swVadFlag,
                      INT32 L_UnqntzdR0, INT32 *pL_UnqntzdCorr);

INT16  getPnBits(int iBits, INT32 *L_PnSeed);

INT16  gsQuant(INT32 L_GsIn,  INT16 swVoicingMode);

void   updateCNHist(INT32 L_UnqntzdR0,
                    INT32 *pL_UnqntzdCorr,
                    INT32 pL_R0Hist[],
                    INT32 ppL_CorrHist[OVERHANG][NP + 1]);

void   lpcCorrQntz(INT32 pL_CorrelSeq[],
                   INT16  pswFinalRc[],
                   int piVQCodewds[]);

INT32 linInterpSid(INT32 L_New, INT32 L_Old, INT16 swDtxState);

INT16 linInterpSidShort( INT16 swNew,
                         INT16 swOld,
                         INT16 swDtxState);

void   rxInterpR0Lpc(INT16 *pswOldKs, INT16 *pswNewKs,
                     INT16 swRxDTXState,
                     INT16 swDecoMode,  INT16 swFrameType);

#endif

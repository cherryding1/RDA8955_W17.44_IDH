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




typedef short          HWORD;
typedef unsigned short UHWORD;
typedef unsigned int   UWORD;

#define Nhc       8
#define Na        7
#define Np       (Nhc+Na)
#define Npc      (1<<Nhc)
#define Amask    ((1<<Na)-1)
#define Pmask    ((1<<Np)-1)
#define Nh       16
#define Nb       16
#define Nhxn     14
#define Nhg      (Nh-Nhxn)
#define NLpScl   13
//#define IBUFFSIZE 4096          /* Input buffer size */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#define MAX_HWORD (32767)
#define MIN_HWORD (-32768)


long CII_Resample (unsigned int * pSrc, long nSrcLen,unsigned int * pDec);
int CII_InitResample(long inSample, long inChan, long layer);
static int ResampleLinear_Mono(HWORD X[], HWORD Y[], UHWORD Nx);
HWORD WordToHword(int v, int scl);
int WordToHwordDou(int left_v, int right_v, int scl);

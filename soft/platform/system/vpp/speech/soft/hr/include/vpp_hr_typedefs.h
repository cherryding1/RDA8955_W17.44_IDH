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

#ifndef __TYPEDEFS
#define __TYPEDEFS

#define DATE    "August 8, 1996    "
#define VERSION "Version 4.2       "

#define LW_SIGN (long)0x80000000       /* sign bit */
#define LW_MIN (long)0x80000000
#define LW_MAX (long)0x7fffffff

#define SW_SIGN (short)0x8000          /* sign bit for INT16 type */
#define SW_MIN (short)0x8000           /* smallest Ram */
#define SW_MAX (short)0x7fff           /* largest Ram */

/* Definition of Types *
 ***********************/

//typedef long int INT32;             /* 32 bit "accumulator" (L_*) */
//typedef short int INT16;           /* 16 bit "register"  (sw*) */
//typedef short int INT16;        /* 16 bit ROM data    (sr*) */
//typedef long int INT32Rom;          /* 32 bit ROM data    (L_r*)  */

struct NormSw
{
    /* normalized INT16 fractional
     * number snr.man precedes snr.sh (the
     * shift count)i */
    INT16 man;                       /* "mantissa" stored in 16 bit
                                        * location */
    INT16 sh;                        /* the shift count, stored in 16 bit
                                        * location */
};

/* Global constants *
 ********************/

#define NP 10                          /* order of the lpc filter */
#define N_SUB 4                        /* number of subframes */
#define F_LEN 160                      /* number of samples in a frame */
#define S_LEN 40                       /* number of samples in a subframe */
#define A_LEN 170                      /* LPC analysis length */
#define OS_FCTR 6                      /* maximum LTP lag oversampling
                                        * factor */

#define OVERHANG 8                     /* vad parameter */
#define strStr strStr16

/* global variables */
/********************/

extern int giFrmCnt;                   /* 0,1,2,3,4..... */
extern int giSfrmCnt;                  /* 0,1,2,3 */

extern int giDTXon;                    /* DTX Mode on/off */

#endif

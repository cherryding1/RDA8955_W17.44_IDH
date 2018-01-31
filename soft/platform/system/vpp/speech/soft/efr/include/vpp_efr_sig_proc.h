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

/*--------------------------------------------------------------*
* Function prototypes for general SIGnal PROCessing functions. *
*--------------------------------------------------------------*/

/* Mathematic functions  */

INT32 Inv_sqrt (      /* (o) : output value   (range: 0<=val<1)            */
    INT32 L_x         /* (i) : input value    (range: 0<=val<=7fffffff)    */
);
void Log2 (
    INT32 L_x,        /* (i) : input value                                 */
    INT16 *exponent,  /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    INT16 *fraction   /* (o) : Fractional part of Log2. (range: 0<=val<1)*/
);
INT32 Pow2 (          /* (o) : result       (range: 0<=val<=0x7fffffff)    */
    INT16 exponent,   /* (i) : Integer part.      (range: 0<=val<=30)      */
    INT16 fraction    /* (i) : Fractional part.  (range: 0.0<=val<1.0)     */
);

/* General signal processing */

void Init_Pre_Process (void);
void Pre_Process (
    INT16 signal[],   /* Input/output signal                               */
    INT16 lg          /* Lenght of signal                                  */
);

INT16 Autocorr (
    INT16* x,        /* (i)    : Input signal                             */
    INT16 m,          /* (i)    : LPC order                                */
    INT16* r_h,      /* (o)    : Autocorrelations  (msb)                  */
    INT16* r_l,      /* (o)    : Autocorrelations  (lsb)                  */
    const INT16* wind/* (i)    : window for LPC analysis.                 */
);
void Lag_window (
    INT16 m,          /* (i)    : LPC order                                */
    INT16 r_h[],      /* (i/o)  : Autocorrelations  (msb)                  */
    INT16 r_l[]       /* (i/o)  : Autocorrelations  (lsb)                  */
);
void Levinson (
    INT16 Rh[],       /* (i)    : Rh[m+1] Vector of autocorrelations (msb) */
    INT16 Rl[],       /* (i)    : Rl[m+1] Vector of autocorrelations (lsb) */
    INT16 A[],        /* (o)    : A[m]    LPC coefficients  (m = 10)       */
    INT16 rc[]        /* (o)    : rc[4]   First 4 reflection coefficients  */
);
void Az_lsp (
    INT16 a[],        /* (i)    : predictor coefficients                   */
    INT16 lsp[],      /* (o)    : line spectral pairs                      */
    INT16 old_lsp[]   /* (i)    : old lsp[] (in case not found 10 roots)   */
);
void Lsp_Az (
    INT16 lsp[],      /* (i)    : line spectral frequencies                */
    INT16 a[]         /* (o)    : predictor coefficients (order = 10)      */
);
void Lsf_lsp (
    INT16 lsf[],      /* (i)    : lsf[m] normalized (range: 0.0<=val<=0.5) */
    INT16 lsp[],      /* (o)    : lsp[m] (range: -1<=val<1)                */
    INT16 m           /* (i)    : LPC order                                */
);
void Lsp_lsf (
    INT16 lsp[],      /* (i)    : lsp[m] (range: -1<=val<1)                */
    INT16 lsf[],      /* (o)    : lsf[m] normalized (range: 0.0<=val<=0.5) */
    INT16 m           /* (i)    : LPC order                                */
);
void Reorder_lsf (
    INT16 *lsf,       /* (i/o)  : vector of LSFs   (range: 0<=val<=0.5)    */
    INT16 min_dist,   /* (i)    : minimum required distance                */
    INT16 n           /* (i)    : LPC order                                */
);
void Weight_Fac (
    INT16 gamma,      /* (i)    : Spectral expansion.                      */
    INT16 fac[]       /* (i/o)  : Computed factors.                        */
);
void Weight_Ai (
    INT16 a[],        /* (i)  : a[m+1]  LPC coefficients   (m=10)          */
    const INT16 fac[],/* (i)  : Spectral expansion factors.                */
    INT16 a_exp[]     /* (o)  : Spectral expanded LPC coefficients         */
);
void Residu (
    INT16 a[],        /* (i)  : prediction coefficients                    */
    INT16 x[],        /* (i)  : speech signal                              */
    INT16 y[],        /* (o)  : residual signal                            */
    INT16 lg          /* (i)  : size of filtering                          */
);
void Syn_filt (
    INT16 a[],        /* (i)  : a[m+1] prediction coefficients   (m=10)    */
    INT16 x[],        /* (i)  : input signal                               */
    INT16 y[],        /* (o)  : output signal                              */
    INT16 lg,         /* (i)  : size of filtering                          */
    INT16 mem[],      /* (i/o): memory associated with this filtering.     */
    INT16 update      /* (i)  : 0=no update, 1=update of memory.           */
);
void Convolve (
    INT16 x[],        /* (i)  : input vector                               */
    INT16 h[],        /* (i)  : impulse response                           */
    INT16 y[],        /* (o)  : output vector                              */
    INT16 L           /* (i)  : vector size                                */
);
void agc (
    INT16 *sig_in,    /* (i)  : postfilter input signal                    */
    INT16 *sig_out,   /* (i/o): postfilter output signal                   */
    INT16 agc_fac,    /* (i)  : AGC factor                                 */
    INT16 l_trm       /* (i)  : subframe size                              */
);
void agc2 (
    INT16 *sig_in,    /* (i)  : postfilter input signal                    */
    INT16 *sig_out,   /* (i/o): postfilter output signal                   */
    INT16 l_trm       /* (i)  : subframe size                              */
);
void preemphasis (
    INT16 *signal,    /* (i/o): input signal overwritten by the output     */
    INT16 g,          /* (i)  : preemphasis coefficient                    */
    INT16 L           /* (i)  : size of filtering                          */
);

/* General */

void Copy (
    INT16 x[],        /* (i)  : input vector                               */
    INT16 y[],        /* (o)  : output vector                              */
    INT16 L           /* (i)  : vector length                              */
);

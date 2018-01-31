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



#ifndef _IDCT_H_
#define _IDCT_H_

int idct_int32_init();

typedef void (idctFunc) (short *const block);
typedef idctFunc *idctFuncPtr;

extern idctFuncPtr idct;

idctFunc idct_int32;

#ifdef ARCH_IS_IA32
idctFunc idct_mmx;          /* AP-992, Peter Gubanov, Michel Lespinasse */
idctFunc idct_xmm;          /* AP-992, Peter Gubanov, Michel Lespinasse */
idctFunc idct_3dne;         /* AP-992, Peter Gubanov, Michel Lespinasse, Jaan Kalda */
idctFunc idct_sse2_skal;    /* Skal's one, IEEE 1180 compliant */
idctFunc idct_sse2_dmitry;  /* Dmitry Rozhdestvensky */
idctFunc simple_idct_mmx;   /* Michael Niedermayer; expects permutated data */
idctFunc simple_idct_mmx2;  /* Michael Niedermayer */
#endif

#ifdef ARCH_IS_IA64
idctFunc idct_ia64;
#endif

#ifdef ARCH_IS_PPC
idctFunc idct_altivec_c;
#endif

#ifdef ARCH_IS_X86_64
idctFunc idct_x86_64;
#endif

#endif                          /* _IDCT_H_ */


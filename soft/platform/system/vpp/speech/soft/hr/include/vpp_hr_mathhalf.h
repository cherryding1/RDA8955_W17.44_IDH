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

#ifndef __MATHHALF
#define __MATHHALF

#include "vpp_hr_typedefs.h"

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

/* addition */
/************/

//INT16 add(INT16 var1, INT16 var2);  /* 1 ops */
//INT16 sub(INT16 var1, INT16 var2);  /* 1 ops */
//INT32 L_add(INT32 L_var1, INT32 L_var2);       /* 2 ops */
//INT32 L_sub(INT32 L_var1, INT32 L_var2);       /* 2 ops */

/* multiplication */
/******************/

//INT16 mult(INT16 var1, INT16 var2); /* 1 ops */
//INT32 L_mult(INT16 var1, INT16 var2);        /* 1 ops */
//INT16 mult_r(INT16 var1, INT16 var2);       /* 2 ops */


/* arithmetic shifts */
/*********************/

//INT16 shr(INT16 var1, INT16 var2);  /* 1 ops */
//INT16 shl(INT16 var1, INT16 var2);  /* 1 ops */
//INT32 L_shr(INT32 L_var1, INT16 var2);        /* 2 ops */
//INT32 L_shl(INT32 L_var1, INT16 var2);        /* 2 ops */
//INT16 shift_r(INT16 var, INT16 var2);       /* 2 ops */
//INT32 L_shift_r(INT32 L_var, INT16 var2);     /* 3 ops */

/* absolute value  */
/*******************/

//INT16 abs_s(INT16 var1);       /* 1 ops */
//INT32 L_abs(INT32 var1);         /* 3 ops */


/* multiply accumulate  */
/************************/

//INT32 L_mac(INT32 L_var3, INT16 var1, INT16 var2);  /* 1 op */
//INT16 mac_r(INT32 L_var3, INT16 var1, INT16 var2); /* 2 op */
//INT32 L_msu(INT32 L_var3, INT16 var1, INT16 var2);  /* 1 op */
//INT16 msu_r(INT32 L_var3, INT16 var1, INT16 var2); /* 2 op */

/* negation  */
/*************/

//INT16 negate(INT16 var1);      /* 1 ops */
//INT32 L_negate(INT32 L_var1);    /* 2 ops */


/* Accumulator manipulation */
/****************************/

//INT32 L_deposit_l(INT16 var1);  /* 1 ops */
//INT32 L_deposit_h(INT16 var1);  /* 1 ops */
//INT16 extract_l(INT32 L_var1);  /* 1 ops */
//INT16 extract_h(INT32 L_var1);  /* 1 ops */

/* Round */
/*********/

//INT16 roundo(INT32 L_var1);      /* 1 ops */

/* Normalization */
/*****************/

INT16 norm_l(INT32 L_var1);     /* 30 ops */
INT16 norm_s(INT16 var1);      /* 15 ops */

/* Division */
/************/
//INT16 divide_s(INT16 var1, INT16 var2);     /* 18 ops */

/* Non-saturating instructions */
/*******************************/
//INT32 L_add_c(INT32 L_Var1, INT32 L_Var2);     /* 2 ops */
//INT32 L_sub_c(INT32 L_Var1, INT32 L_Var2);     /* 2 ops */
//INT32 L_sat(INT32 L_var1);       /* 4 ops */
//INT32 L_macNs(INT32 L_var3, INT16 var1, INT16 var2);        /* 1 ops */
//INT32 L_msuNs(INT32 L_var3, INT16 var1, INT16 var2);        /* 1 ops */

#endif

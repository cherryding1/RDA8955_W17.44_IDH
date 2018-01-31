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

extern INT32 Overflow;
extern INT32 Carry;

#define MAX_32 (INT32)0x7fffffffL
#define MIN_32 (INT32)0x80000000L

#define MAX_16 (INT16)0x7fff
#define MIN_16 (INT16)0x8000

/*___________________________________________________________________________
 |                                                                           |
 |   Prototypes for basic arithmetic operators                               |
 |___________________________________________________________________________|
*/

//INT16 add (INT16 var1, INT16 var2);    /* Short add,           1   */
//INT16 sub (INT16 var1, INT16 var2);    /* Short sub,           1   */
//INT16 abs_s (INT16 var1);               /* Short abs,           1   */
//INT16 shl (INT16 var1, INT16 var2);    /* Short shift left,    1   */
//INT16 shr(INT16 var1, INT16 var2);    /* Short shift right,   1   */
//INT16 mult (INT16 var1, INT16 var2);   /* Short mult,          1   */
//INT32 l_mult (INT16 var1, INT16 var2); /* Long mult,           1   */
//INT16 negate(INT16 var1);              /* Short negate,        1   */
//INT16 extract_h(INT32 L_var1);         /* Extract high,        1   */
//INT16 extract_l(INT32 L_var1);         /* Extract low,         1   */
//INT16 round (INT32 L_var1);             /* Round,               1   */
//INT32 L_mac (INT32 L_var3, INT16 var1, INT16 var2);   /* Mac,  1  */
//INT32 L_msu (INT32 L_var3, INT16 var1, INT16 var2);   /* Msu,  1  */
INT32 L_macNs (INT32 L_var3, INT16 var1, INT16 var2); /* Mac without
                                                             sat, 1   */
INT32 L_msuNs (INT32 L_var3, INT16 var1, INT16 var2); /* Msu without
                                                             sat, 1   */
//INT32 l_add (INT32 L_var1, INT32 L_var2);    /* Long add,        2 */
//INT32 L_sub (INT32 L_var1, INT32 L_var2);    /* Long sub,        2 */
INT32 L_add_c (INT32 L_var1, INT32 L_var2);  /* Long add with c, 2 */
INT32 L_sub_c (INT32 L_var1, INT32 L_var2);  /* Long sub with c, 2 */
//INT32 L_negate (INT32 L_var1);                /* Long negate,     2 */
//INT16 mult_r (INT16 var1, INT16 var2);       /* Mult with round, 2 */
//INT32 L_shl (INT32 L_var1, INT16 var2);      /* Long shift left, 2 */
//INT32 L_shr (INT32 L_var1, INT16 var2);      /* Long shift right, 2*/
//INT16 shr_r (INT16 var1, INT16 var2);        /* Shift right with round, 2           */
INT16 mac_r (INT32 L_var3, INT16 var1, INT16 var2); /* Mac with
                                                           rounding,2 */
INT16 msu_r (INT32 L_var3, INT16 var1, INT16 var2); /* Msu with
                                                           rounding,2 */
//INT32 l_deposit_h (INT16 var1);        /* 16 bit var1 -> MSB,     2 */
//INT32 l_deposit_l (INT16 var1);        /* 16 bit var1 -> LSB,     2 */
INT32 L_shr_r (INT32 L_var1, INT16 var2); /* Long shift right with round3             */
//INT32 L_abs (INT32 L_var1);            /* Long abs,              3  */
//INT32 L_sat (INT32 L_var1);            /* Long saturation,       4  */
INT16 norm_s (INT16 var1);             /* Short norm,           15  */
INT16 div_s (INT16 var1, INT16 var2); /* Short division,       18  */
INT16 norm_l (INT32 L_var1);           /* Long norm,            30  */












































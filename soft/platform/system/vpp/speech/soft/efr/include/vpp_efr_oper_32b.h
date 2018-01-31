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



void L_Extract (INT32 L_32, INT16 *hi, INT16 *lo);
INT32 L_Comp (INT16 hi, INT16 lo);
INT32 Mpy_32 (INT16 hi1, INT16 lo1, INT16 hi2, INT16 lo2);
INT32 Mpy_32_16 (INT16 hi, INT16 lo, INT16 n);
INT32 Div_32 (INT32 L_num, INT16 denom_hi, INT16 denom_lo);

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

#ifndef  __MATHDP31
#define  __MATHDP31

//#include "vpp_hr_typedefs.h"
#include "cs_types.h"

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

INT32 L_mpy_ls(INT32 L_var2, INT16 var1);
INT32 L_mpy_ll(INT32 L_var1, INT32 L_var2);
short  isSwLimit(INT16 swIn);
short  isLwLimit(INT32 L_In);

#endif

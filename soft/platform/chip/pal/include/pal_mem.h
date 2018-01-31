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

#ifndef _PAL_MEM_H
#define _PAL_MEM_H

#include "sxs_type.h"

#define PAL_RAM_BASE            0x80000000
#define PAL_INTERNAL_RAM_BASE   0x80C00000

#define PAL_CACHED(addr)    ((addr)&0xdfffffff)
#define PAL_UNCACHED(addr)  ((addr)|0x20000000)

#define PAL_FUNC_INTERNAL   __attribute__((section(".sramtext")))
#define PAL_DATA_INTERNAL   __attribute__((section(".sramdata")))


#endif /* _PAL_MEM_H */


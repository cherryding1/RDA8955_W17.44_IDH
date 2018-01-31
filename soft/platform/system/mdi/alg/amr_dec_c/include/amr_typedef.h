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




#ifndef _TYPEDEF_H
#define _TYPEDEF_H

#ifdef SHEEN_VC_DEBUG
#define mmc_MemMalloc malloc
#define diag_printf printf
#else
#include "cs_types.h"
#include "mcip_debug.h"
#include "mmc.h"
#include "string.h"

#define labs ABS
#define abs ABS

#endif

typedef char Word8;
typedef unsigned char UWord8;
typedef short Word16;
typedef long Word32;
typedef float Float32;
typedef double Float64;



#endif

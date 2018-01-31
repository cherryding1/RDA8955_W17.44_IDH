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



#ifndef  _HALP_EBC_H_
#define  _HALP_EBC_H_

#include "cs_types.h"
#include "global_macros.h"
#include "mem_bridge.h"
#include "hal_sys.h"
#include "hal_ebc.h"


#ifdef CHIP_HAS_EBC_CS2_BUG
/// Use to remember the CS0 timing configuration, as the register
/// cannot be read again.
extern PROTECTED UINT32 g_halCs0TimeReg ;
/// Use to remember the CS0 mode configuration, as the register
/// cannot be read again.
extern PROTECTED UINT32 g_halCs0ModeReg ;
#endif

///
///     Array of pointers to the different
///     CS configuration.
///
///     \c NULL in the i-th place means
///     the CS i is not enabled/used @todo Write that properly.
///
extern HAL_EBC_CS_CFG_T* g_halEbcCsConfigArray [EBC_NB_CS];

#endif // HAL_EBC_H

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



#ifndef _HALP_RDA_ABB_H_
#define _HALP_RDA_ABB_H_

#include "cs_types.h"

// ----------------
// APIs
// ----------------

//=============================================================================
// hal_AbbIspiRead
//-----------------------------------------------------------------------------
/// Read codec register on ISPI, the CS must already be activated
/// @param regIdx register to read
/// @return register value                 0 write 1 read
//=============================================================================
PROTECTED UINT32 hal_AbbIspiRead(UINT32 regIdx);


//=============================================================================
// hal_AbbIspiWrite
//-----------------------------------------------------------------------------
/// Write codec register on ISPI, the CS must already be activated
/// @param regIdx register to read
/// @param value register value
//=============================================================================
PROTECTED VOID hal_AbbIspiWrite(UINT32 regIdx, UINT32 value);


PROTECTED VOID hal_AbbIspiReadOutputSelect(UINT32 addr);

#endif // _HALP_RDA_ABB_H_


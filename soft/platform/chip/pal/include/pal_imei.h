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

#ifndef __PAL_IMEI_H__
#define __PAL_IMEI_H__

#include "sxs_type.h"


PUBLIC BOOL pal_CheckImei(CONST UINT8* imei);
PUBLIC BOOL pal_GetFactoryImei(UINT8 simIndex,UINT8 *imei);
PUBLIC BOOL pal_SaveFactoryImei(UINT8 simIndex, UINT8 *imei);

//======================================================================
// Phone's IMEI Getter :
//----------------------------------------------------------------------
/// This function returns the IMEI of the phone. It is usually
/// retrieved from the factory settings of the flash. Check the CRC of
/// the factory settings structure and the "factory step" bit is set,
/// then get the IMEI. Otherwise, return the default IMEI.
///
/// One IMEI is defined
/// per SIM in the case where the phone has several SIMs.
///
/// @param simIndex Index of the SIM whose related IMEI is to be
/// returned. Use 0 when the phone is single-sim.
/// @return Pointer to the IMEI for the sim index passed as a parameter.
//======================================================================
PUBLIC UINT8* pal_GetImei(UINT8 simIndex);



#endif


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

























#ifndef _SINGEN_H_
#define _SINGEN_H_


#include "cs_types.h"



// =============================================================================
// sinGen
// -----------------------------------------------------------------------------
/// Generate a sine wave
/// @param f Frequency
/// @param ampl Amplitude
/// @param sinBuf Pointer to the buffer where the sinwave is stored.
/// @param bufSize Size of the buffer, in bytes.
// =============================================================================
PROTECTED VOID sinGen(UINT16 f, UINT8 ampl, UINT32 *sinBuf, UINT32 bufSize);



#endif /* _SINGEN_H_ */


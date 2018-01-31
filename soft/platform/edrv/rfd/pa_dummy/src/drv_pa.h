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

#ifndef PA_DUMMY_H
#define PA_DUMMY_H

#include "cs_types.h"

#include "rfd_cfg.h"
#include "rfd_pa.h"
#include "rfd_defs.h"

//--------------------------------------------------------------------//
//                                                                    //
//                    GPIO and TCO Assignement                        //
//                                                                    //
//--------------------------------------------------------------------//

//--------------------------------------------------------------------//
//                                                                    //
//                            API for PAL                             //
//                                                                    //
//--------------------------------------------------------------------//

void rfd_PaTxOn(INT16 start, GSM_RFBAND_T band);
void rfd_PaTxOff(INT16 stop, GSM_RFBAND_T band);
void rfd_PaSetRamp(INT16 start, UINT8 rampId, RFD_PA_RAMPDIR_T rampDir);
void rfd_PaSetLowPower(UINT16 date);

#endif // PA_DUMMY_H

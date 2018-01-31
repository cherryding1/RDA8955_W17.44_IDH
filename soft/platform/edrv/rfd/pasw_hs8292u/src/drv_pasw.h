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

#ifndef PASW_RDA6231_H
#define PASW_RDA6231_H

#include "cs_types.h"
#include "rfd_defs.h"
#include "baseband_defs.h"

//--------------------------------------------------------------------//
//                                                                    //
//                    GPIO and TCO Assignement                        //
//                                                                    //
//--------------------------------------------------------------------//





//--------------------------------------------------------------------//
//                                                                    //
//                        TX slots sizes                              //
//                                                                    //
//--------------------------------------------------------------------//
#define PA_NBURST_DURATION          (4*BB_BURST_TOTAL_SIZE)
#define PA_ABURST_DURATION          (96*4)

#endif // PASW_RDA6231_H

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

#include <base.h>
#include <cfw.h>
#include <stdkey.h>
#include <sul.h>
#include "api_msg.h"
#include <ts.h>
#include <cfw_prv.h>
#include <event_prv.h>

//      nOption: initialization option.
//          nOption = 0x1: delete all of the database
//          nOption = 0x2: rebuild the all the database
//          nOption = 0x4: remap the SIM data to database
#if 0
BOOL SHL_PbkPowerOn(UINT8 nOption) // 0x02|0x04
{
    CFW_PbkPowerOn(nOption, APP_UTI_SHELL);
    return TRUE;
}
#endif

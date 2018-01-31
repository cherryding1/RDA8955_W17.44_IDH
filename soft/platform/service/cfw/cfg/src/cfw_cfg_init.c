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


#include <sul.h>
#include <cfw.h>
#include "cfw_cfg.h"
#include "csw_debug.h"
//
// Initialize CFG global variables when power on.
//
extern UINT16 IND_EVENT_general[];
UINT32 CFW_CfgInit()
{
    UINT32 ret = ERR_SUCCESS;
    UINT16 param = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CfgInit);
    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_CfgInit\n",0x081001a0));

    UINT8 i = 0x00;
    for( ; i < CFW_SIM_NUMBER; i++ )
    {
        IND_EVENT_general[i] = 0xFF;
    }
    CFW_CfgSimParamsInit();
    ret = CFW_CfgGetUserProfile ( );

    ret = NV_GetGenIndEvent(&param);
    if(ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NV_GetGenIndEvent, error ret =0x%x\n",0x081001a2), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CfgInit);
        return ret;
    }

    i = 0x00;
    for( ; i < CFW_SIM_NUMBER; i++ )
    {
        IND_EVENT_general[i] = param;
    }


    CSW_TRACE(_CSW|TLEVEL(CFW_CFG_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_CfgInit, exit ret =0x%x\n",0x081001a3), ret);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CfgInit);
    return ret;
}


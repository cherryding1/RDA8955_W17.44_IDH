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
#include <shell.h>
#include "csw_debug.h"

UINT32 g_ShellDisableModules = 0;

BOOL SHL_InitGlobalVariables(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(SHL_InitGlobalVariables);
    g_ShellDisableModules = 0;
    CSW_PROFILE_FUNCTION_EXIT(SHL_InitGlobalVariables);
    return TRUE;
}

BOOL SHL_DisableModules(UINT32 nDisableModule)
{
    CSW_PROFILE_FUNCTION_ENTER(SHL_DisableModules);
    g_ShellDisableModules |= nDisableModule;
    CSW_PROFILE_FUNCTION_EXIT(SHL_DisableModules);
    return TRUE;
}

UINT32 SHL_GetDisableModule(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(SHL_GetDisableModule);
    CSW_PROFILE_FUNCTION_EXIT(SHL_GetDisableModule);
    return g_ShellDisableModules;
}

#if 0
UINT32 SHL_CleanupData()
{
    CSW_PROFILE_FUNCTION_ENTER(SHL_CleanupData);
    CSW_PROFILE_FUNCTION_EXIT(SHL_CleanupData);
    FS_Format(NULL);
    // Registry import.
}
#endif

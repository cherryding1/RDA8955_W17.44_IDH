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

#include <cfw.h>
#include <cos.h>

extern BOOL hal_HstSendEvent(UINT32 ch);

typedef struct _APP_NODE
{
    PFN_CFW_EV_PORC pfnProc;
    PVOID pUserData;
} APP_NODE, *PAPP_NODE ;

PRIVATE APP_NODE g_app_node          = {0, };
//PRIVATE PFN_CFW_EV_PORC g_pfnCfwProc = NULL;

UINT32 CFW_BalSetEventPorc( VOID *pUserData,
                            PFN_CFW_EV_PORC pfnProc)
{

    UINT32 ret = ERR_INVALID_PARAMETER;

    if(pfnProc)
    {
        g_app_node.pUserData = pUserData;
        g_app_node.pfnProc  = pfnProc;
        ret = ERR_SUCCESS;
    }

    return ret;
}



#ifdef AT_MMI_SUPPORT

UINT32 BAL_Run_AT(CFW_EVENT *pEvent)
{
    if(g_app_node.pfnProc)
    {
        return g_app_node.pfnProc(pEvent, g_app_node.pUserData);
    }
    else
    {
        UINT16 n = MAKEUINT16(pEvent->nFlag, pEvent->nType);
        COS_EVENT ev;

        ev.nEventId = pEvent->nEventId;
        ev.nParam1  = pEvent->nParam1;
        ev.nParam2  = pEvent->nParam2;
        ev.nParam3  = MAKEUINT32(n, pEvent->nUTI);
        COS_SendEvent(COS_GetDefaultATTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        return BAL_PROC_DONT_FREE_MEM;
    }
}

UINT32 BAL_Run_MMI(CFW_EVENT *pEvent)
{
    if(g_app_node.pfnProc)
    {
        return g_app_node.pfnProc(pEvent, g_app_node.pUserData);
    }
    else
    {
        UINT16 n = MAKEUINT16(pEvent->nFlag, pEvent->nType);
        COS_EVENT ev;

        ev.nEventId = pEvent->nEventId;
        ev.nParam1  = pEvent->nParam1;
        ev.nParam2  = pEvent->nParam2;
        ev.nParam3  = MAKEUINT32(n, pEvent->nUTI);
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        return BAL_PROC_DONT_FREE_MEM;
    }
}
#endif

UINT32 BAL_Run(CFW_EVENT *pEvent)
{
    if(g_app_node.pfnProc)
    {
        return g_app_node.pfnProc(pEvent, g_app_node.pUserData);
    }
    else if( 0xFFFFFFFF == (UINT32)pEvent )
    {
        hal_HstSendEvent(0xfa022302);
        DBG_ASSERT(0,"SPECIAL MESSAGE!");
        return BAL_PROC_DONT_FREE_MEM;
    }
    else
    {
        UINT16 n = MAKEUINT16(pEvent->nFlag, pEvent->nType);
        COS_EVENT ev;

        ev.nEventId = pEvent->nEventId;
        ev.nParam1  = pEvent->nParam1;
        ev.nParam2  = pEvent->nParam2;
        ev.nParam3  = MAKEUINT32(n, pEvent->nUTI);
        COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        return BAL_PROC_DONT_FREE_MEM;
    }
}

PRIVATE PFN_BOOL pfnCfwSetupQueryEvent = NULL;

UINT32 SRVAPI CFW_SetupQueryEvent(PFN_BOOL pfnCfwQueryEvent)
{
    pfnCfwSetupQueryEvent = pfnCfwQueryEvent;

    return 0;
}

BOOL CFW_QueryEvent(VOID)
{
    if( pfnCfwSetupQueryEvent )
        return pfnCfwSetupQueryEvent();
    else
        return FALSE;
}

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


#include "sxs_idle.h"
#include "sxr_csp.h"


#define SXS_DEBUG_IDLE_HOOK_NUM 5


typedef struct
{
    void (*func)(void);
    bool enable;
} SXS_DEBUG_IDLE_HOOK_ELEM;


//------------------------------------------------------------------
// Idle hook variables

void (*g_sxsFsIdleHookFuncPtr)(void) = NIL;

SXS_DEBUG_IDLE_HOOK_ELEM g_sxsDebugIdleHookElem[SXS_DEBUG_IDLE_HOOK_NUM] =
{ { NIL, FALSE, }, };

volatile u32 __attribute__((section(".ucdata"))) g_sxsDebugIdleHookEnable = 0;


//------------------------------------------------------------------
// FS idle hook functions

void sxs_RegisterFsIdleHookFunc(void (*funcPtr)(void))
{
    g_sxsFsIdleHookFuncPtr = funcPtr;
}

void sxs_FsIdleHook(void)
{
    if (g_sxsFsIdleHookFuncPtr != NIL)
    {
        (*g_sxsFsIdleHookFuncPtr)();
    }
}


//------------------------------------------------------------------
// Debug idle hook functions

bool sxs_RegisterDebugIdleHookElem(void (*funcPtr)(void), bool enable)
{
    int i;
    bool result = FALSE;

    u32 status = sxr_EnterSc ();
    for (i=0; i<SXS_DEBUG_IDLE_HOOK_NUM; i++)
    {
        if (g_sxsDebugIdleHookElem[i].func == NIL)
            break;
    }

    if (i < SXS_DEBUG_IDLE_HOOK_NUM)
    {
        g_sxsDebugIdleHookElem[i].func = funcPtr;
        g_sxsDebugIdleHookElem[i].enable = enable;
        result = TRUE;
    }
    sxr_ExitSc (status);

    return result;
}

bool sxs_RegisterDebugIdleHookFunc(void (*funcPtr)(void))
{
    return sxs_RegisterDebugIdleHookElem(funcPtr, FALSE);
}

bool sxs_ActivateDebugIdleHookFunc(void (*funcPtr)(void))
{
    return sxs_RegisterDebugIdleHookElem(funcPtr, TRUE);
}

void sxs_DebugIdleHook(void)
{
    int i;
    for (i=0; i<SXS_DEBUG_IDLE_HOOK_NUM; i++)
    {
        if (g_sxsDebugIdleHookElem[i].func != NIL &&
                (g_sxsDebugIdleHookEnable ||
                 g_sxsDebugIdleHookElem[i].enable))
        {
            (*g_sxsDebugIdleHookElem[i].func)();
        }
    }
}


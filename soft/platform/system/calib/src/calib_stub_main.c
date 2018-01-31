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





#include "cs_types.h"
#include "hal_sys.h"

#include "sxs_srl.h"
#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "sxs_rmt.h"
#include "sxs_idle.h"
#include "cmn_defs.h"

#include "sxs_io.h"
#include "sxs_spy.h"
#include "sxr_csp.h"
#include "sxr_mutx.h"
#include "calib_sap_cfg.h"

#define PAL_DBG_PORT_IDX 0
typedef UINT32 MsgBody_t;

#include "itf_msg.h"

extern BOOL g_halLpsSleptBefore;

/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/
VOID calib_SxsTimerDelivery (UINT32 *Id)
{
    sxs_Raise (_SXR, "Timer not handled %x\n", Id);
}

/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/
VOID calib_SxsTaskIdle (VOID)
{
    UINT32 cnt = 0;

    // System enters LPS sleep for at least once
    g_halLpsSleptBefore = TRUE;

    while (TRUE)
    {
        hal_HstTraceIdleCheck ();
        hal_SysProcessIdle();
        pal_WDogCntRst();
        //pal_Sleep ();
        sxs_RmtIdleHook ();
        sxs_DebugIdleHook();
        cnt++;
    }
}


/*
==============================================================================
   Function   : calib_SxsInit
 ----------------------------------------------------------------------------
   Scope      : OS initialization
   Parameters : None.
   Return     : None.
==============================================================================
*/

VOID calib_SxsInit(VOID)
{
    UINT32 i;
// -----------------------
// Create Heaps
// -----------------------
    for (i=0; i<PAL_NB_RAM_AREA; i++)
    {
        if (i == 0)
            _sxr_NewHeap ( (VOID *)(((UINT8 *)PAL_HEAP_START(0)) + sizeof (sxs_Boot_t)), PAL_HEAP_SIZE(0) - sizeof (sxs_Boot_t), 0);
        else
            _sxr_NewHeap (PAL_HEAP_START(i), PAL_HEAP_SIZE(i), i);
    }

    if (sxs_Boot -> Pattern != SXS_BOOT_PATTERN)
    {
        memset (sxs_Boot, 0, sizeof (sxs_Boot_t));
        sxs_Boot -> Pattern = SXS_BOOT_PATTERN;
    }

    sxs_SetFnStamp( (UINT32*)(((UINT32)_sxs_SetFnStamp) & ~3) );

//------------------------
// Init OS timer
//------------------------

    sxr_InitTimer (calib_SxsTimerDelivery, pal_TimerSetCnt, pal_TimerGetCnt, SXR_REGULAR_TIMER);
//------------------------
// Init OS Tasks
//------------------------
    sxr_TkSdInit ();
//------------------------
// Init OS Jobs
//------------------------
    sxr_JbSdInit ();
//------------------------
// Init OS Mailboxes
//------------------------
    sxr_SbxInit ();
//------------------------
// Init OS Mutex
//------------------------
    sxr_MutexInit ();
//------------------------
// Init OS Queues
//------------------------
    sxr_InitQueue ();

//------------------------
// Init OS Trace
//------------------------
    sxs_IoInit ();

//-----------------------
// Start Idle Task
// ----------------------
    sxr_StartTask (sxr_NewTask (&g_calibSxsTaskIdleDesc), NIL);

}

/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/

int calib_StubMain (int argc, char *argv[])
{
#ifdef COMPRESS_PREMAIN
    extern u8 premain_code_start;
    unsigned char* premain_start;
    unsigned int premain_size;

    cdfu_get_premain_compressed_data(&premain_start, &premain_size);
    decompress_bare(premain_start, premain_size, &premain_code_start);
#endif

    calib_SxsInit();

    calib_SapInitUsr();

    // All sxr traces
    sxs_SetTraceLevel(_SXR, 0xff);

    SXS_TRACE(TSTDOUT,"start schedule ");
    sxr_StartScheduling ();
    return 0;
}





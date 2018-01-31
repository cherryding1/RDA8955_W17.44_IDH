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





#include "chip_id.h"
#include "hal_sys.h"

#if !defined(__TARGET__) || defined (__SIMU__)
#include <malloc.h>
#ifdef __STKTRACE__
#include "sxs_udp.h"
#endif
#endif

#include "sxs_srl.h"
#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "sxr_csp.h"
#include "sxr_mutx.h"
#include "sxs_rmt.h"
#include "sxs_idle.h"
#include "cmn_defs.h"
#include "sxr_tksd.hp"

#define __SAP_CFG_VAR__
#include "sap_cfg.h"
#undef  __SAP_CFG_VAR__

#include "sxs_io.h"
#include "sxs_spy.h"
#include "pal_mem.h"
#include "pal_gsm.h"
#include "sap_psm_itf.h"

typedef u32 MsgBody_t;

#include "itf_msg.h"

EXPORT HAL_HST_TRACE_PORT_T g_HalTracePort;

extern void  sap_InitUsr (void);
extern u8 sxs_ApFlush (void);

extern void cdfu_get_premain_compressed_data(unsigned char **pAddr, unsigned int *pSize);
extern void decompress_bare(unsigned char *buf, int in_len, unsigned char *output);

#ifdef LTE_NBIOT_SUPPORT
extern void osi_init(void);
extern void sys_InitPs(void);
extern void nvm_init(void);
#endif

u8 nb_lowpower_mbx;


/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/
void sxs_UsrInitTask (void(*UsrInitTask)(void))
{
    if (UsrInitTask != NIL)
    {
        (*UsrInitTask)();
    }
    sxr_StopTask(sxr_GetCurrentTaskId());
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
void sxs_FreeUsrInitTasks (void)
{
    u32 i;
    for (i=0; i<SXS_NB_USR_INIT_TASKS_DESC; i++)
    {
        if (sxs_usrInitTaskId[i] != SXR_NO_TASK)
        {
            if (sxr_GetTaskState(sxs_usrInitTaskId[i]) == SXR_STOPPED_TSK)
            {
                sxr_FreeTask(sxs_usrInitTaskId[i]);
                sxs_usrInitTaskId[i] = SXR_NO_TASK;
            }
        }
    }
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
void sxs_Task (sxs_TaskParse_t *TaskParse)
{
    Msg_t *Msg;
    u32   Evt [SXR_EVT_MBX_SIZE];
    u8    SenderTaskId;
    u32   SenderAddr;

    if (TaskParse -> TaskInit != NIL)
    {
        TaskParse -> TaskInit ();
    }

    while (TRUE)
    {
        Msg = (Msg_t *)sxr_WaitEx (Evt, TaskParse -> MailBox, &SenderTaskId, &SenderAddr);

        if (Msg)
        {
            TaskParse -> MsgParse (Msg);
        }
        else
        {
            if (Evt [0] & SXS_TIMER_EVT_ID)
            {
                TaskParse -> TimerParse (Evt);
            }
            else
            {
                if (Evt [0] == NIL)
                {
                    SXS_RAISE ((_SXR|TNB_ARG(2)|TABORT|TDB, TSTR("Invalid event received at sxs_Task. Sender task=%d, addr=0x%08x", 0x070b0002), SenderTaskId, SenderAddr));
                }
                ((void (*)(void *))Evt [0]) ((void *)Evt [1]);
            }
        }
    }
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
void sxs_TimerDelivery (u32 *Id)
{
    SXS_RAISE ((_SXR|TNB_ARG(1)|TDB, TSTR("Timer not handled %x", 0x070b0001), Id));
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
void sxs_StartAll (void)
{
    u32 i;

    sxr_TkSdInit ();
    sxr_JbSdInit ();

    sxr_SbxInit ();
    sxr_MutexInit ();

    for (i=0; i<CMN_LAST; i++)
        sxr_NewMailBox ();

    sxr_SetRmtMbx (STTPC_MBX, TRUE ) ;

#ifdef __STACKSIMU__
    sxr_SetRmtMbx (API_MBX, TRUE ) ;
#endif

    sxr_InitTimer (sxs_TimerDelivery, pal_TimerSetCnt, pal_TimerGetCnt, SXR_REGULAR_TIMER);

    sxr_InitQueue ();

    sxr_NewQueue ("Cs Data");

    sxs_IoInit ();

#ifdef __PROTO_REMOTE__
    for (i=CMN_RR; i< CMN_LAST; i++)
        sxr_SetRmtMbx (i, TRUE);
#endif

    sxr_StartTask (sxr_NewTask (&sxs_TaskIdleDesc), NIL);
#ifdef LTE_NBIOT_SUPPORT
    sxs_NBInitTaskId = sxr_NewTask (&sxs_NBInitDesc);
    sxr_StartTask (sxs_NBInitTaskId, NIL);
#else

    for (i=0; i<sxs_NbStaticTasksDesc; i++)
    {
        u8 TaskId =
            sxs_CmnTaskId [sxs_TaskCfg [i].TaskParse -> MailBox] = sxr_NewTask (&sxs_TaskCfg [i].TaskDesc);
        sxr_StartTask (TaskId, (void *)sxs_TaskCfg [i].TaskParse);
    }

    for (i=0; i<SXS_NB_USR_INIT_TASKS_DESC; i++)
    {
        u8 TaskId =
            sxs_usrInitTaskId [i] = sxr_NewTask (&sxs_UsrInitTaskCfg [i].TaskDesc);
        sxr_StartTask (TaskId, (void *)sxs_UsrInitTaskCfg [i].UsrInitTask);
    }
#endif
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


void sxs_TaskIdle (void)
{
    u32 Cnt = 0;
    u8 SerialDataAvail = FALSE;
    u8 SysPollingNeeded = FALSE;

    while (TRUE)
    {
        sxs_FreeUsrInitTasks();
        sxs_FsIdleHook();
        SerialDataAvail = hal_HstTraceIdleCheck();
#ifdef __MULTI_RAT__
        if(g_HalTracePort == HAL_HST_TRACE_PORT_AP)
        {
            SerialDataAvail = sxs_ApFlush();
        }
#endif
        SysPollingNeeded = hal_SysProcessIdle();
        pal_WDogCntRst ();
        sxs_SpyData ();
#ifndef _TTCN_TEST_
        if(!SerialDataAvail && !SysPollingNeeded)
        {
            pal_Sleep ();
        }
#endif
        sxs_RmtIdleHook ();
        sxs_DebugIdleHook();

        Cnt++;
    }
}

#ifdef LTE_NBIOT_SUPPORT
u8  psm_restore_ind = 0;
//sextern u32 simInitTime[5];
void sxs_NBPSMTask (void)
{
    int i;
    u8 TaskId;
    u32 status;

    nb_lowpower_mbx = sxr_NewMailBox();
    //disable L1C 1ms interrupt when stack is init and restore
    hal_IrqDisable(0x25);
    for (i=0; i<sxs_NbStaticTasksDesc; i++)
    {
        TaskId = sxs_CmnTaskId [sxs_TaskCfg [i].TaskParse -> MailBox] = sxr_NewTask (&sxs_TaskCfg [i].TaskDesc);
        sxr_StartTask (TaskId, (void *)sxs_TaskCfg [i].TaskParse);
    }

    for (i=0; i<SXS_NB_USR_INIT_TASKS_DESC; i++)
    {
        TaskId = sxs_usrInitTaskId [i] = sxr_NewTask (&sxs_UsrInitTaskCfg [i].TaskDesc);
        sxr_StartTask (TaskId, (void *)sxs_UsrInitTaskCfg [i].UsrInitTask);
    }


    if (osi_is_wakeup_from_deep_sleep())
    {
        psm_restore_ind =1;
    }
    sys_InitPs();

    if (psm_restore_ind == 1)
    {
        hal_HstSendEvent(0xbacdbacd);
        hal_HstSendEvent(hal_SysGetResetCause());
        //simInitTime[4] = hal_TimGetUpTime();
        osi_wakeup_notify();
    }
    //re-enable L1C 1ms interrupt
    hal_IrqEnable(0x25);
    sxr_CheckTask();

    while (1)
    {
        u32   Evt [SXR_EVT_MBX_SIZE];

        sxr_WaitEx (Evt, nb_lowpower_mbx, NULL, NULL);
        switch (Evt [0])
        {
            case OSI_PSM_RESTORE_CFW_DYN_CTX_IND:
                {
                    sdb_RestoreCswDynamicBakupInfo();
                }
            default:
                break;
        }
    }

}
#endif


// Used by cfw_shell_main.c
/// This function modifies a variable which is never used,
/// the calls to this function should be removed from CSW!!!
void PM_ProLPMDisable (void)
{
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
int main (int argc, char *argv[])
{
    u32 i;

#ifdef COMPRESS_PREMAIN
    extern u8 premain_code_start;
    unsigned char* premain_start;
    unsigned int premain_size;
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_250M);		//chenqiang 20161101
    cdfu_get_premain_compressed_data(&premain_start, &premain_size);
    decompress_bare(premain_start, premain_size, &premain_code_start);
#endif

    for (i=0; i<PAL_NB_RAM_AREA; i++)
    {
        if (i == 0)
            _sxr_NewHeap ( (void *)(((u8 *)PAL_HEAP_START(0)) + sizeof (sxs_Boot_t)), PAL_HEAP_SIZE(0) - sizeof (sxs_Boot_t), 0);
        else
            _sxr_NewHeap (PAL_HEAP_START(i), PAL_HEAP_SIZE(i), i);
    }

    sxs_SetFnStamp( (u32*)(((u32)_sxs_SetFnStamp) & ~3) );

    sxs_StartAll ();

#ifdef LTE_NBIOT_SUPPORT
    tra01_01init();
    osi_init();
#endif
    sxr_NewCluster (32, 200);
    sxr_NewCluster (64, 200);
    sxr_NewCluster (128, 100);
    sxr_NewCluster (512, 50);
    sxr_NewCluster (4096, 10);

    sap_InitUsr ();

    sxr_CheckTask();

// Never return
    sxr_StartScheduling ();

    return 0;
}


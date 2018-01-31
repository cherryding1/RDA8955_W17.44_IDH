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

#include "at.h"
#include "at_cmd_gc.h"
#include "at_module.h"
#include "at_sa.h"
#include "at_map.h"
#include "calib_m.h"
#include "at_utils.h"
#include "at_dispatch.h"
#include "at_cmux_engine.h"
#ifdef AT_MODULE_SUPPORT_OTA
#include "stack_config.h"
#endif
#include "mci.h"

#define FSM_STATUS_MASK_POWER_DOWN 0x00000001
#define FSM_STATUS_MASK_NORMAL 0x00000002
#define FSM_STATUS_MASK_CHGR_NORMAL 0x00000004
#define FSM_STATUS_MASK_CHGR_ONLY 0x00000008
#define FSM_STATUS_MASK_ALARM_NORMAL 0x00000010
#define FSM_STATUS_MASK_ALARM_ONLY 0x00000020
#define FSM_STATUS_MASK_SLEEP 0x00000040
#define PROC_MODE_MASK_COMMAND 0x00000001
#define PROC_MODE_MASK_ONLINE_CMD 0x00000002
#define PROC_MODE_MASK_CS 0x00000004
#define PROC_MODE_MASK_PS 0x00000008

// //////////////////////////////////////////////////
// System States
// //////////////////////////////////////////////////

typedef enum _sa_fsm_state_t {
    SA_FSM_STATE_POWER_DOWN,
    SA_FSM_STATE_NORMAL,
    SA_FSM_STATE_CHGR_NORMAL,
    SA_FSM_STATE_CHGR_ONLY,
    SA_FSM_STATE_ALARM_NORMAL,
    SA_FSM_STATE_ALARM_ONLY,
    SA_FSM_STATE_SLEEP,
    SA_FSM_STATE_COUNT_,   // counter of this enum.
    SA_FSM_STATE_INVALID_, // invalid value of this enum.
} sa_fsm_state_t;

typedef enum _sa_proc_mode_t {
    SA_PROC_MODE_COMMAND,
    SA_PROC_MODE_ONLINE_CMD,
    SA_PROC_MODE_CS,
    SA_PROC_MODE_PS,
    SA_PROC_MODE_COUNT_,   // counter of this enum.
    SA_PROC_MODE_INVALID_, // invalid value of this enum.
} sa_proc_mode_t;

// //////////////////////////////////////////////////
// GLOBAL VARIABLES DEFINITION
// //////////////////////////////////////////////////
//static sa_info_t g_SaInfo = {0};
//static sa_info_t g_SaInfoMux[MAX_DLC_NUM] = {
// {0},
//};
extern AT_ModuleInfo g_AtModule;

void GpsCreateTask(void);

// //////////////////////////////////////////////////
// PROTOTYPE DEFINITIONS OF INTERNAL FUNCTIONS
// //////////////////////////////////////////////////
void SA_DispatchMessage(COS_EVENT *pev);
VOID BAL_ATTask(VOID *pData);

// //////////////////////////////////////////////////
// PROTOTYPE DEFINITIONS OF EXTERNAL FUNCTIONS
// //////////////////////////////////////////////////
#ifdef AT_FTP_SUPPORT
extern void ftp_Init(void); // ats/ftp/src/ftp.c
#endif

#define MMI_VERSION_STRING "MMIv1.0.1"

#ifdef BUILD_DATE
#define MMI_BUILD_DATE BUILD_DATE
#else
#error BUILD_DATE is not defined ! Check your environment...
#endif

#ifndef MMI_WITH_AT
extern MMI_Default_Value g_MMI_Default_Value;
VOID BAL_SetMMIDefaultValue(VOID)
{
    g_MMI_Default_Value.nMinVol = 3200;
    g_MMI_Default_Value.nMemorySize = 50 * 1024;
}
#endif

#ifdef __BT_RDABT__
bool BT_IsInquiring(void)
{
    return TRUE;
}
UINT32 rdabt_adp_uart_rx()
{
    return ERR_SUCCESS;
}
#endif

UINT32 SA_GetSysStatus(SA_STAT_CLASS stat_class)
{
    /*
    switch (stat_class)
    {
    case SA_POWER_ON_MODE:
        return g_SaInfo.power_on_mode;

    case SA_ATCMD_SMSO_RESULT:
        return g_SaInfo.atcmd_smso_result;

    case SA_ATCMD_CFUN_RESULT:
        return g_SaInfo.atcmd_cfun_result;

    case SA_ATCMD_ATO_RESULT:
        return g_SaInfo.atcmd_ato_result;

    case SA_ATCMD_3PLUS_RESULT:
        return g_SaInfo.atcmd_3plus_result;

    case SA_ATCMD_ATH_RESULT:
        return g_SaInfo.atcmd_ath_result;

    case SA_ATCMD_ATA_RESULT:
        return g_SaInfo.atcmd_ata_result;

    case SA_ATCMD_CGDATA_RESULT:
        return g_SaInfo.atcmd_cgdata_result;

    case SA_INIT_STATUS:
        return g_SaInfo.init_status;

    case SA_SIM_STATUS:
        return g_SaInfo.sim_status;

    case SA_ATS_INIT_STATUS:
        return g_SaInfo.ats_init_status;

    case SA_NW_REG_STATUS:
        return g_SaInfo.nw_reg_status;

    case SA_INNER_FSM_STATUS_:
        return g_SaInfo.fsm_stat;

    case SA_INNER_PROC_MODE_:
        return g_SaInfo.proc_mode;

    case SA_INNER_RUNCOMMAND_STATE_:
        return g_SaInfo.has_node_in_cmdlist;

    default:
        break;
    }
*/
    return 0;
}
BOOL SA_IsCmdEnabled(AT_CMD_ENGINE_T *engine, const SA_COMMAND_TABLE_T *desc)
{
    UINT32 sys_stat = 0;

    if (desc->chief_enable_flag != 1)
        return FALSE;

    sys_stat = SA_FSM_STATE_NORMAL; // SA_GetSysStatusMux(engine, SA_INNER_FSM_STATUS_);

    AT_TC(g_sw_SA, "enable_mask.fsm_status_mask %d", desc->enable_mask.fsm_status_mask);

    switch (sys_stat)
    {
    case SA_FSM_STATE_POWER_DOWN:
        return FALSE;

    case SA_FSM_STATE_NORMAL:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_NORMAL) != FSM_STATUS_MASK_NORMAL)
            return FALSE;
        break;

    case SA_FSM_STATE_CHGR_NORMAL:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_CHGR_NORMAL) !=
            FSM_STATUS_MASK_CHGR_NORMAL)
            return FALSE;

        break;

    case SA_FSM_STATE_CHGR_ONLY:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_CHGR_ONLY) != FSM_STATUS_MASK_CHGR_ONLY)
            return FALSE;

        break;

    case SA_FSM_STATE_ALARM_NORMAL:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_ALARM_NORMAL) !=
            FSM_STATUS_MASK_ALARM_NORMAL)
            return FALSE;

        break;

    case SA_FSM_STATE_ALARM_ONLY:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_ALARM_ONLY) != FSM_STATUS_MASK_ALARM_ONLY)
            return FALSE;

        break;

    case SA_FSM_STATE_SLEEP:
        return FALSE;

    default:
        return FALSE;
    }

    sys_stat = SA_PROC_MODE_COMMAND; // SA_GetSysStatusMux(engine, SA_INNER_PROC_MODE_);

    AT_TC(g_sw_SA, "SA_GetSysStatus %d", sys_stat);
    AT_TC(g_sw_SA, "enable_mask.proc_mode_mask %d", desc->enable_mask.proc_mode_mask);

    switch (sys_stat)
    {
    case SA_PROC_MODE_COMMAND:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_COMMAND) != PROC_MODE_MASK_COMMAND)
            return FALSE;

        break;

    case SA_PROC_MODE_ONLINE_CMD:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_ONLINE_CMD) != PROC_MODE_MASK_ONLINE_CMD)
            return FALSE;

        break;

    // [[hameina[mod] 2008-4-22 :Ps, cs are different modes, can't do the same check.
    case SA_PROC_MODE_PS:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_PS) != PROC_MODE_MASK_PS)
            return FALSE;

        break;

    case SA_PROC_MODE_CS:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_CS) != PROC_MODE_MASK_CS)
            return FALSE;

        break;

    default:
        return FALSE;
    }

    return TRUE;
}

// TODO: need to check more status and module! now just check the initialization status for SMS and PBK.
BOOL SA_IsCmdReady(AT_CMD_ENGINE_T *engine, const SA_COMMAND_TABLE_T *desc)
{
    SA_CMD_CLASS_T cmd_cls = desc->cmd_class;

    switch (cmd_cls)
    {
    case SA_CMDCLS_SYS:
    case SA_CMDCLS_CC:
        break;

    case SA_CMDCLS_SMS:
        if (!at_ModuleGetSmsReady())
            return FALSE;

        break;

    case SA_CMDCLS_NW:
    case SA_CMDCLS_SS:
        break;

    case SA_CMDCLS_PBK:
        if (!at_ModuleGetPbkReady())
            return FALSE;

        break;

    case SA_CMDCLS_SAT:
    case SA_CMDCLS_ID:
    case SA_CMDCLS_SIM:
    case SA_CMDCLS_HW:
    case SA_CMDCLS_DS:
    case SA_CMDCLS_GC:
    case SA_CMDCLS_EX:
    default:
        break;
    }

    return TRUE;
}

/***************************************************************************************************
  SYSTEM AGENT
 ****************************************************************************************************/
/* The default AT (MMI) Task Handle. */
HANDLE g_hAtTask = HNULL;
#ifdef  __TTS_FEATURES__

extern HANDLE g_hTTSTask;
extern void ejTTSPlayer_Task_Main(void *pData);
#endif

//
// Marcos to create default MMI Task.
//
// ////////////////////////////////////////////////////////////////////////////////////////////////
// Old stack size is too small to process the case, this is there maybe lots of commands in one command line.
// Now, one command node will cost about 112 bytes, so if there are 20 commands in one command line,
// it will cost 112 * 20 = 2k bytes, so we must increase the stack size!
// Maybe we have another solution for this issue, I think we can reduce the memory cost of one command noce, and I think
// 112 per command node is too big!!!
#define AT_TASK_STACK_SIZE (2048 * 2)
#define AT_TASK_PRIORITY (COS_MMI_TASKS_PRIORITY_BASE + 9)

#ifdef MMI_ONLY_AT
#ifdef __VDS_QUICK_FLUSH__
UINT32 g_htstVdsTask = 0;

#define MMI_FLASHTASK_PRIORITY (COS_MMI_TASKS_PRIORITY_BASE + 10)
#define COS_CREATE_DEFAULT 0
extern VOID VDS_CacheTaskEntry(void *pData);
#endif
#endif
#ifdef AT_MODULE_SUPPORT_OTA
#define JAT_TRANSPORTTASK_PRIORITY (COS_MMI_TASKS_PRIORITY_BASE + 11)
#define TRANSPORT_TASK_STACK_SIZE 10240 // 10k
#endif
#ifdef MMI_ONLY_AT
VOID SRVAPI BAL_InitSysFreq(VOID)
{
    ; // do nothing
}

BOOL SRVAPI BAL_ApplicationInit(VOID)
{
    return TRUE;
}
#endif

#ifdef AT_MODULE_SUPPORT_OTA
void BAL_TransportTaskEntry(void *pArg)
{
    jdi_TransportTask(0, NULL);
}
#endif

BOOL BAL_ATInit(VOID)
{
    ATLOGI("BAL_ApplicationInit Started...");

    at_RegisterYourself();

    // Create a default MMI task by CSW automatically.
    // You only can change the task stack size.
    g_hAtTask = COS_CreateTask(BAL_ATTask,
                               NULL,
                               NULL,
                               AT_TASK_STACK_SIZE,
                               AT_TASK_PRIORITY,
                               COS_CREATE_DEFAULT,
                               0,
                               "AT Task");

#ifdef MMI_ONLY_AT
#ifdef __VDS_QUICK_FLUSH__
    g_htstVdsTask = COS_CreateTask(VDS_CacheTaskEntry,
                                   NULL, NULL,
                                   1024,
                                   MMI_FLASHTASK_PRIORITY,
                                   COS_CREATE_DEFAULT,
                                   0,
                                   "VDS Flush Task");
#endif
#ifdef  __TTS_FEATURES__

#define TTS_TASK_STACK_SIZE		1024	// 2k
#define TTS_TASK_PRIORITY          (5 + COS_PRI_PRV_BASE)
    ///g_hTTSTask
g_hTTSTask = COS_CreateTask(ejTTSPlayer_Task_Main,
                   NULL, NULL,
                   1024,
                   TTS_TASK_PRIORITY,
                   COS_CREATE_DEFAULT, 0, "TTS Task");
ASSERT(g_hTTSTask != HNULL);
#endif

#if defined(__TTS_FEATURES__) || defined(AT_SOUND_RECORDER_SUPPORT) || defined(AT_CAMERA_SUPPORT)
    MCI_TaskInit();
#endif
#endif

#ifdef AT_MODULE_SUPPORT_OTA
    HANDLE otaTask = COS_CreateTask(BAL_TransportTaskEntry,
                                    NULL,
                                    NULL,
                                    TRANSPORT_TASK_STACK_SIZE,
                                    JAT_TRANSPORTTASK_PRIORITY,
                                    COS_CREATE_DEFAULT,
                                    0,
                                    "Transport Task");
    SetTransportTaskHandle(MOD_TRANSPORT, hTask);
    jdd_TimerInitialize();
#endif
#ifdef GPS_SUPPORT
    GpsCreateTask();
#endif
    return TRUE;
}

extern PVOID g_CswBaseAdd;
extern UINT32 g_CswHeapSize;

VOID BAL_ATTask(VOID *pData)
{
    COS_EVENT event = {0};

    ATLOGI("Enter BAL_ATTask...");
    AT_ModuleInit();
    AT_InitUtiTable();

#ifdef AT_FTP_SUPPORT
    ftp_Init();
#endif

    for (;;)
    {
        if (event.nParam1 != NULL)
        {
            if (EV_CFW_SIM_READ_RECORD_RSP == event.nEventId)
            {
                hal_HstSendEvent(0xfa022212);
                hal_HstSendEvent(event.nParam1);
                if ((((UINT32)(UINT32 *)event.nParam1) >= (UINT32)g_CswBaseAdd) &&
                    (((UINT32)(UINT32 *)event.nParam1) < (UINT32)(g_CswBaseAdd + g_CswHeapSize)))
                {
                    UINT8 *pPrm = (UINT8 *)event.nParam1;
                    SUL_MemSet8(pPrm + event.nParam2, 0x0f, 1);
                }
            }
            if (AT_FREE((VOID *)event.nParam1) == FALSE)
            {
                hal_HstSendEvent(0x09250001);
                hal_HstSendEvent(event.nEventId);
                AT_ZERO_PARAM1(&event);
            }
        }

        COS_WaitEvent(g_hAtTask, &event, COS_WAIT_FOREVER);

#ifdef AT_MODULE_SUPPORT_OTA
        if (MSG_ID_TRANSP_TOMMI_IND == event.nEventId)
        {
            UINT32 nRet = COS_SendEventToJataayu(&event);
            if (ERR_SUCCESS != nRet)
            {
                COS_SetLastError(nRet);
            }
            event.nParam1 = 0;
            continue;
        }
#endif

        SA_DispatchMessage(&event);
    }
}

#if 0
void at_CmdAutoCallTimeout(AT_CMD_ENGINE_T *engine)
{
    CALIB_CALIBRATION_T *calibPtr = calib_GetPointers();
    if (calibPtr->hstSw->param[14])
    {
        //Set Auto call flag to 0,and save the calib data to flash
        calib_DaemonUsrDataProcess();
        calibPtr->hstSw->param[14] = 0x00;
    }
    else
    {
        at_StartCallbackTimer(1500, (COS_CALLBACK_FUNC_T)at_CmdAutoCallTimeout, engine);
    }
}
#endif

void SA_ProcPowerOnInd(COS_EVENT *pEvent)
{
    static UINT8 power_on_normal = 0;
    UINT32 ret = CFW_ShellControl(CFW_CONTROL_CMD_POWER_ON);

    ATLOGI("CFW_ShellControl: %d", ret);
    switch (LOUINT16(pEvent->nParam1))
    {
    case DM_POWRN_ON_CAUSE_KEY:   // power on by press power key
    case DM_POWRN_ON_CAUSE_RESET: // reset
        power_on_normal = 1;
        break;

    case DM_POWRN_ON_CAUSE_CHARGE: // power on by charging
    case DM_POWRN_ON_CAUSE_ALARM:  // power on by alarming
    case DM_POWRN_ON_CAUSE_EXCEPTION:
    default:
        break;
    }
    AT_ZERO_PARAM1(pEvent);
}

void SA_DispatchMessage(COS_EVENT *pev)
{
    ATLOGI("AT get event: 0x%08x/0x%08x/0x%08x/0x%08x",
           pev->nEventId, pev->nParam1, pev->nParam2, pev->nParam3);

    if (AT_InitCfw(pev) == TRUE)
        return;

    switch (pev->nEventId)
    {
    case EV_DM_POWER_ON_IND:
        SA_ProcPowerOnInd(pev);
        break;

    case EV_TIMER:
        break;

#ifdef __BT_RDABT__
    case EV_CFW_BLUETOOTH_DATA_IND:
        rda5868_uart_rx();
        break;
    case EV_CFW_BLUETOOTH_WAKEUP_IND:
        RDA5868_uart_tx_wakeup();
        break;
#endif

#ifdef __WIFI_RDAWIFI__
    case EV_CFW_WIFI_TEST_MODE_INFO_IND:
        ATLOGD("EV_CFW_WIFI_TEST_MODE_DATA_IND len/%d", pev->nParam2);
        at_CmdDirectWrite((const char *)pev->nParam1, pev->nParam2);
        break;
#endif

    default:
        AT_AsyncEventProcess(pev);
        break;
    }
}

VOID SA_DefaultCmdHandler_Error(AT_CMD_PARA *pParam)
{
    at_CmdRespCmeError(pParam->engine, ERR_AT_UNKNOWN);
}

VOID SA_DefaultCmdHandler_AT(AT_CMD_PARA *pParam)
{
    at_CmdRespOK(pParam->engine);
}

VOID SA_DefaultCmdHandler_CMUX(AT_CMD_PARA *pParam)
{
    UINT8 msg[80] = {0};
    AT_CMUX_CONFIG_T muxcfg = gAtCmuxDefaultCfg;

    if (pParam->iType == AT_CMD_TEST)
    {
        strcpy(msg, "+CMUX: (0,1),(0),(1-6),(1-255),(1-255),(0-100),(2-255),(1-255),(1-7)");
        at_CmdRespInfoText(pParam->engine, msg);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_CMUX_ENGINE_T *cmux = at_DispatchGetParentCmux(at_DispatchGetByChannel(pParam->nDLCI));
        if (cmux != NULL)
            muxcfg = at_CmuxGetConfig(cmux);

        sprintf(msg, "+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d",
                muxcfg.transparency, muxcfg.subset, muxcfg.portSpeed, muxcfg.maxFrameSize,
                muxcfg.ackTimer, muxcfg.maxRetransCount, muxcfg.respTimer, muxcfg.wakeupRespTimer,
                muxcfg.windowSize);
        at_CmdRespInfoText(pParam->engine, msg);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount > 9 || pParam->paramCount < 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        muxcfg.transparency = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        muxcfg.subset = at_ParamDefUintInRange(pParam->params[1], muxcfg.subset, 0, 0, &paramok);
        muxcfg.portSpeed = at_ParamDefUintInRange(pParam->params[2], muxcfg.portSpeed, 1, 6, &paramok);
        muxcfg.maxFrameSize = at_ParamDefUintInRange(pParam->params[3], muxcfg.maxFrameSize, 1, 255, &paramok);
        muxcfg.ackTimer = at_ParamDefUintInRange(pParam->params[4], muxcfg.ackTimer, 1, 255, &paramok);
        muxcfg.maxRetransCount = at_ParamDefUintInRange(pParam->params[5], muxcfg.maxRetransCount, 0, 100, &paramok);
        muxcfg.respTimer = at_ParamDefUintInRange(pParam->params[6], muxcfg.respTimer, 2, 255, &paramok);
        muxcfg.wakeupRespTimer = at_ParamDefUintInRange(pParam->params[7], muxcfg.wakeupRespTimer, 1, 255, &paramok);
        muxcfg.windowSize = at_ParamDefUintInRange(pParam->params[8], muxcfg.windowSize, 1, 7, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        at_CmdRespOK(pParam->engine);
        at_DispatchSetCmuxMode(at_DispatchGetByChannel(pParam->nDLCI), &muxcfg);
    }
    else
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

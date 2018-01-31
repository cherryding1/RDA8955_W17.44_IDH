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

#ifndef __AT_SA_H__
#define __AT_SA_H__

#include "at.h"
#include "at_cfg.h"

typedef enum _SA_STAT_CLASS
{
    SA_POWER_ON_MODE,
    SA_ATCMD_SMSO_RESULT,
    SA_ATCMD_CFUN_RESULT,
    SA_ATCMD_ATO_RESULT,
    SA_ATCMD_ATD_RESULT,
    SA_ATCMD_ATA_RESULT,  // jyk
    SA_ATCMD_3PLUS_RESULT,
    SA_ATCMD_ATH_RESULT,
    SA_ATCMD_CGDATA_RESULT,
    SA_INIT_STATUS,
    SA_SIM_STATUS,
    SA_ATS_INIT_STATUS,
    SA_NW_REG_STATUS,
    AT_EVENT,

    // DO NOT execute set operation on the following:
    SA_INNER_FSM_STATUS_,
    SA_INNER_PROC_MODE_,
    SA_INNER_RUNCOMMAND_STATE_,
} SA_STAT_CLASS;

/***************************************************************************************************
                                Command Table
****************************************************************************************************/

typedef struct
{
    // Every FSM status according to a bit.
    // b0: SA_FSM_STATE_POWER_DOWN,
    // b1: SA_FSM_STATE_NORMAL,
    // b2: SA_FSM_STATE_CHGR_NORMAL,
    // b3: SA_FSM_STATE_CHGR_ONLY,
    // b4: SA_FSM_STATE_ALARM_NORMAL,
    // b5: SA_FSM_STATE_ALARM_ONLY,
    // b6: SA_FSM_STATE_SLEEP,
    UINT32 fsm_status_mask;

    // Every process mode according to a bit.
    // b0: SA_PROC_MODE_COMMAND,
    // b1: SA_PROC_MODE_ONLINE_CMD,
    // b2: SA_PROC_MODE_ONLINE_DATA,
    UINT32 proc_mode_mask;
} SA_CMDTBL_ENABLE_MASK_T;

/*
Commonly, we can classify AT commands to several level, for example, CC and SMS and PBK commands all are SIM related commands,
and HW, GC commands all are SIM not related commands. This is the 1st level class, and CC, SMS, PBK is the 2nd level class, and so on.

So, we can use some bits to mark the 1st level class, and use another group of bits to mark 2nd level class, and so on.

But, now for simpleness purpose, we just make a single level class, these are:
    SYS
    CC
    SMS
    NW
    SS
    PBK
    SAT
    ID
    SIM
    HW
    DS
    GC
    EX
    ...
**/
typedef enum {
    SA_CMDCLS_SYS,
    SA_CMDCLS_CC,
    SA_CMDCLS_SMS,
    SA_CMDCLS_NW,
    SA_CMDCLS_SS,
    SA_CMDCLS_PBK,
    SA_CMDCLS_SAT,
    SA_CMDCLS_ID,
    SA_CMDCLS_SIM,
    SA_CMDCLS_HW,
    SA_CMDCLS_DS,
    SA_CMDCLS_GC,
    SA_CMDCLS_EX,
    SA_CMDCLS_COUNT_,
    SA_CMDCLS_GPRS,
    SA_CMDCLS_TCPIP,
    SA_CMDCLS_DTM,
    SA_CMDCLS_DFMS,
    SA_CMDCLS_INVALID_ = 0x9FFF, // An invalid enum value maker, and also force this enum type as UINT16 data type.
} SA_CMD_CLASS_T;

typedef struct SA_COMMAND_TABLE_T
{
    UINT8 *pName;             // command name
    pfn_AtCmdHandler pFunc;   // function point.
    SA_CMD_CLASS_T cmd_class; // class flag.
    UINT16 chief_enable_flag; // chief switcher. 0: disable, 1: enable.
    SA_CMDTBL_ENABLE_MASK_T enable_mask;
} SA_COMMAND_TABLE_T;

extern HANDLE g_hAtTask;

/*
**/
const SA_COMMAND_TABLE_T *at_GetCmdDescriptor(const uint8_t *name, unsigned len);
extern VOID SA_DefaultCmdHandler_Error(AT_CMD_PARA *pParam);
extern VOID SA_DefaultCmdHandler_AT(AT_CMD_PARA *pParam);
extern VOID SA_DefaultCmdHandler_CMUX(AT_CMD_PARA *pParam);
BOOL SA_IsCmdEnabled(AT_CMD_ENGINE_T *engine, const SA_COMMAND_TABLE_T *desc);
BOOL SA_IsCmdReady(AT_CMD_ENGINE_T *engine, const SA_COMMAND_TABLE_T *desc);

enum _AT_INHERIT_EVENT
{

    AT_INHERIT_EV_BASE,
    AT_INHERIT_EV_SMS_INIT,
    AT_INHERIT_EV_CFW_XXX_END_,
};
VOID BAL_ATInheritTask(VOID *pData);
extern HANDLE g_hAtInheritTask ;

#define SA_ATS_SMS_READY                0x00000001
#define SA_ATS_PBK_READY                0x00000002
#define SA_ATS_MASK_CLEAR               0x00000000
#endif

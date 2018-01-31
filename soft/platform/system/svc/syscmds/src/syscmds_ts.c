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


#include "syscmdsp_hdlr.h"
#include "syscmdsp_debug.h"
#include "syscmds_ts_cmd_id.h"
#include "hal_clk.h"

#include "string.h"

#include "sxr_tksd.h"

#include "gpio_i2c.h"


// =============================================================================
//  EXTERNAL FUNCTIONS
// =============================================================================

// TS driver functions
void syscmds_ts_get_buffer_info(u32 *addr,u32 *buff_size,u32 *number_piont);
void syscmds_ts_get_tp_info(u32 *addr,u8 *prt_len);
void syscmds_rda_ts_resume(void);
void syscmds_rda_ts_suspend(void);
void syscmds_rda_ts_reboot(void);
void syscmds_init_ts_panel(void);
BOOL syscmds_get_ts_point(void);
BOOL syscmds_get_ps_distance(void);
void syscmds_ap_is_ready(BOOL enale);
int syscmds_ts_switch_proximity_mode(int enable);

PRIVATE VOID syscmds_TsTask(VOID *param);
PROTECTED VOID syscmds_SendI2cSendDoneMsg(VOID);
PROTECTED VOID syscmds_SendI2cRecvDoneMsg(VOID);


// =============================================================================
// MACROS
// =============================================================================

// In number of words
#define TS_TASK_STACK_SIZE      (128)

#define TS_TASK_PRIORITY        (206)

#define TS_CMD_NONE             (0)
#define TS_CMD_DING             (1)


// =============================================================================
// TYPES
// =============================================================================


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE UINT8 g_tsTaskId = 0xFF;
//PRIVATE BOOL g_ts_buffer_info = FALSE;
#ifndef _TGT_MODEM_NO_TS_
PRIVATE u32 g_ts_buffer_ptr[3];
#endif
PRIVATE CONST sxr_TaskDesc_t gc_tsTaskDesc =
{
    .TaskBody = &syscmds_TsTask,
    .TaskExit = NULL,
    .Name = "Syscmds TS Task",
    .StackSize = TS_TASK_STACK_SIZE,
    .Priority = TS_TASK_PRIORITY,
};

PRIVATE UINT32 g_tsCmd = TS_CMD_NONE;
PRIVATE BOOL g_tsMode = TRUE;


// =============================================================================
//  FUNCTIONS
// =============================================================================

PROTECTED VOID syscmds_StartTsTask(VOID)
{
    g_tsTaskId = sxr_NewTask(&gc_tsTaskDesc);
    sxr_StartTask(g_tsTaskId, NULL);
}


PRIVATE VOID syscmds_TsTask(VOID *param)
{
#ifndef _TGT_MODEM_NO_TS_
    UINT32 scStatus;
    UINT32 cmd;
    hal_SwRequestClk(FOURCC_CAMS, HAL_SYS_FREQ_208M);
    while (1)
    {
        scStatus = hal_SysEnterCriticalSection();
        while (g_tsCmd == TS_CMD_NONE)
        {
            sxr_SuspendTask(g_tsTaskId);
        }
        cmd = g_tsCmd;
        g_tsCmd = TS_CMD_NONE;
        hal_SysExitCriticalSection(scStatus);

        if (cmd == AP_TS_CMD_I2C_SEND)
        {
            if(FALSE==  syscmds_get_ts_point()) // some error
            {

                SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "syscmds_TsTask discard! ");
            }
            else
            {
                syscmds_SendI2cSendDoneMsg();
            }
        }
        else if(cmd == AP_TS_CMD_SWITCH_PROXIMITY_MODE)
        {
            if(FALSE==  syscmds_get_ps_distance()) // some error
            {

                SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "syscmds_TsTask discard! ");
            }
            else
            {
                syscmds_SendI2cSendDoneMsg();
            }
        }


#if 0
        if (cmd == AP_TS_CMD_I2C_SEND)
        {
#ifdef GPIO_I2C
            gpio_i2c_write_data(addr, &buf[2], regLen, &buf[3 + regLen], len);
#endif
            syscmds_SendI2cSendDoneMsg();
        }
        else if (cmd == AP_TS_CMD_I2C_RECV)
        {
#ifdef GPIO_I2C
            gpio_i2c_read_data(addr, &buf[2], regLen, &buf[0], len);
#endif
            syscmds_SendI2cRecvDoneMsg();
        }
#endif
    }
#endif
}


PROTECTED BOOL syscmds_HandleTsCmd(SYSCMDS_RECV_MSG_T *pRecvMsg)
{
    BOOL result = TRUE;
#ifndef _TGT_MODEM_NO_TS_
    UINT32 scStatus;

    switch (pRecvMsg->msgId)
    {
        case AP_TS_CMD_I2C_INIT:
        {
#if 0
            UINT32 bufInfo[2];

            syscmds_StartTsTask();

            // bufInfo[0] = (UINT32)g_tsI2cBuffer;
            //  bufInfo[1] = sizeof(g_tsI2cBuffer);
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, bufInfo, sizeof(bufInfo));
#endif
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
            syscmds_init_ts_panel();


        }
        break;

        case AP_TS_CMD_I2C_BUFF_ADD:
        {
            syscmds_ts_get_buffer_info(&g_ts_buffer_ptr[0],&g_ts_buffer_ptr[1],&g_ts_buffer_ptr[2]);
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, &g_ts_buffer_ptr,12);
            syscmds_ap_is_ready(TRUE);
        }
        break;
        case AP_TS_CMD_I2C_INFO:
        {
            u8 Info_len;
            u32 addr = 0;
            syscmds_ts_get_tp_info(&addr,&Info_len);
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, (u32*)addr,Info_len);
        }
        break;
        case AP_TS_CMD_I2C_SUSPEND:
        {

            syscmds_ap_is_ready(FALSE);
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
            syscmds_rda_ts_suspend();
            hal_SwReleaseClk(FOURCC_CAMS);
        }
        break;
        case AP_TS_CMD_I2C_RESUME:
        {
            hal_SwRequestClk(FOURCC_CAMS, HAL_SYS_FREQ_208M);
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
            syscmds_rda_ts_resume();
            syscmds_ap_is_ready(TRUE);
        }
        break;
        case AP_TS_CMD_I2C_REBOOT:
        {
            syscmds_rda_ts_reboot();
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0, pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_TS_CMD_I2C_SEND:
        case AP_TS_CMD_I2C_RECV:
        {
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);

            scStatus = hal_SysEnterCriticalSection();
            SYSCMDS_ASSERT(g_tsCmd == TS_CMD_NONE,
                           "Previous cmd not finished yet: %d", g_tsCmd);
            g_tsCmd = pRecvMsg->msgId;
            hal_SysExitCriticalSection(scStatus);

            SYSCMDS_ASSERT(g_tsTaskId != 0xFF,
                           "%s not started", gc_tsTaskDesc.Name);

            sxr_ResumeTask(g_tsTaskId);
        }
        break;
        case AP_TS_CMD_SWITCH_PROXIMITY_MODE:
        {
            u32 enable = 0;
            memcpy(&enable, pRecvMsg->param, sizeof(enable));
            scStatus = hal_SysEnterCriticalSection();
            syscmds_ts_switch_proximity_mode(enable);
            g_tsMode = enable?FALSE:TRUE;
            hal_SysExitCriticalSection(scStatus);
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        default:
            result = FALSE;
            break;
    }
#endif
    return result;
}


PROTECTED VOID syscmds_SendI2cSendDoneMsg(VOID)
{
#if 0
    u8 len = 0;
    if(g_ts_buffer_info == FALSE)
    {
        g_ts_buffer_ptr[0] = cmd_ts_get_buffer_address();
        g_ts_buffer_ptr[1] = cmd_ts_get_buffer_size();
        g_ts_buffer_ptr[2] = cmd_ts_get_buffer_point_number();;
        g_ts_buffer_info = TRUE;
        len = 12;
    }
    // syscmds_Send(SYSCMDS_MOD_ID_TS, MODEM_TS_CMD_I2C_SEND_DONE, NULL, 0);
    syscmds_Send(SYSCMDS_MOD_ID_TS, MODEM_TS_CMD_I2C_SEND_DONE, &g_ts_buffer_ptr, len);
#else
    syscmds_Send(SYSCMDS_MOD_ID_TS, MODEM_TS_CMD_I2C_SEND_DONE, NULL, 0);
#endif /* #if 0 */
}


PROTECTED VOID syscmds_SendI2cRecvDoneMsg(VOID)
{
    syscmds_Send(SYSCMDS_MOD_ID_TS, MODEM_TS_CMD_I2C_RECV_DONE, NULL, 0);
}


PUBLIC VOID syscmds_Ts_Resume(void)
{
    UINT32 scStatus;

    if(g_tsCmd != TS_CMD_NONE)
    {
        //hal_HstSendEvent(0x1234ffff);
        return;
    }
    scStatus = hal_SysEnterCriticalSection();
    if(g_tsMode)
        g_tsCmd = AP_TS_CMD_I2C_SEND;
    else
        g_tsCmd = AP_TS_CMD_SWITCH_PROXIMITY_MODE;
    hal_SysExitCriticalSection(scStatus);

    SYSCMDS_ASSERT(g_tsTaskId != 0xFF,"%s not started", gc_tsTaskDesc.Name);

    sxr_ResumeTask(g_tsTaskId);


}


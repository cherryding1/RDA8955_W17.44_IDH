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
#include "syscmds_gen_cmd_id.h"

#include "string.h"
#include "stdlib.h"

#include "hal_sys.h"
#include "hal_trace.h"
#include "hal_host.h"
#include "hal_ap_comm.h"
#include "hal_map_engine.h"

#include "sxs_srl.h"

#include "tgt_m.h"
#include "ap_factory.h"


// =============================================================================
//  EXTERNS
// =============================================================================


// =============================================================================
// MACROS
// =============================================================================

#define BUILD_TIME_BUF_LEN          (32)


// =============================================================================
// TYPES
// =============================================================================

typedef enum
{
    MODULE_HAL,
    MODULE_AT,
    MODULE_CSW,
    MODULE_STACK,

    MODULE_QTY,
} MODULE_TYPE_T;

typedef struct
{
    UINT32 revision;
    UINT32 build_date;
} MODULE_VER_T;

typedef struct
{
    MODULE_VER_T ver[MODULE_QTY];
    char build_time[BUILD_TIME_BUF_LEN];
} MODEM_VER_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE HAL_SYS_CLOCK_OUT_ID_T g_syscmdsClkOutId = HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE;


// =============================================================================
//  FUNCTIONS
// =============================================================================

PROTECTED BOOL syscmds_HandleGeneralCmd(SYSCMDS_RECV_MSG_T *pRecvMsg)
{
    BOOL result = TRUE;

    switch (pRecvMsg->msgId)
    {
        case AP_GENERAL_CMD_SHUTDOWN:
        {
            hal_HstSendEvent(0xdead00a9);
            hal_SysShutdown();
        }
        break;
        case AP_GENERAL_CMD_RESTART:
        {
            UINT32 POSSIBLY_UNUSED status = hal_SysEnterCriticalSection();
            hal_HstSendEvent(0x11a100a9);
            hal_SysRestart();
        }
        break;
        case AP_GENERAL_CMD_TRACE_PATH:
        {
            UINT32 port;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&port, pRecvMsg->param, 4);

            SYSCMDS_ASSERT(port == HAL_HST_TRACE_PORT_HOST ||
                           port == HAL_HST_TRACE_PORT_AP,
                           "Invalid trace port: %d", port);

            hal_HstTraceSwitch(port);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_CLK_OUT:
        {
            UINT32 param;
            HAL_SYS_CLOCK_OUT_FREQ_T freq;
            UINT32 retVal = 1;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&param, pRecvMsg->param, 4);

            SYSCMDS_ASSERT(param < 3,
                           "Invalid clk out freq id: %d", param);

            if (g_syscmdsClkOutId == HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE)
            {
                if (param != 0)
                {
                    if (param == 1)
                    {
                        freq = HAL_SYS_CLOCK_OUT_FREQ_32K;
                    }
                    else //if (param == 2)
                    {
                        freq = HAL_SYS_CLOCK_OUT_FREQ_26M;
                    }
                    g_syscmdsClkOutId = hal_SysClkOutOpen(freq);
                    retVal = (g_syscmdsClkOutId == HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE);
                }
            }
            else
            {
                if (param == 0)
                {
                    hal_SysClkOutClose(g_syscmdsClkOutId);
                    g_syscmdsClkOutId = HAL_SYS_CLOCK_OUT_RESOURCE_UNAVAILABLE;
                    retVal = 0;
                }
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_AUX_CLK:
        {
            UINT32 enable;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&enable, pRecvMsg->param, 4);

            hal_SysAuxClkOut(enable);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_CLK_32K:
        {
            UINT32 enable;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&enable, pRecvMsg->param, 4);

            hal_Sys32kClkOut(enable);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_MODEM_VER:
        {
            MODEM_VER_T modemVer;
            CONST HAL_MAP_VERSION_T *halVer, *atVer, *cswVer, *stkVer;

            halVer = hal_MapEngineGetVersion(HAL_MAP_ID_HAL);
            atVer = hal_MapEngineGetVersion(HAL_MAP_ID_AT);
            cswVer = hal_MapEngineGetVersion(HAL_MAP_ID_CSW);
            stkVer = hal_MapEngineGetVersion(HAL_MAP_ID_STACK);

            memset(&modemVer, 0, sizeof(modemVer));

            if (halVer)
            {
#ifdef WITH_GIT
                modemVer.ver[MODULE_HAL].revision = halVer->revision;
#else
                modemVer.ver[MODULE_HAL].revision = (1<<31) | halVer->revision;
#endif
                modemVer.ver[MODULE_HAL].build_date = halVer->date;
            }

            if (atVer)
            {
                // AT is always compiled in SVN
                modemVer.ver[MODULE_AT].revision = (1<<31) | atVer->revision;
                modemVer.ver[MODULE_AT].build_date = atVer->date;
            }

            if (cswVer)
            {
                // CSW is always compiled in SVN
                modemVer.ver[MODULE_CSW].revision = (1<<31) | cswVer->revision;
                modemVer.ver[MODULE_CSW].build_date = cswVer->date;
            }

            if (stkVer)
            {
                // Stack is always compiled in SVN
                modemVer.ver[MODULE_STACK].revision = (1<<31) | stkVer->revision;
                modemVer.ver[MODULE_STACK].build_date = stkVer->date;
            }

            // Save the actual build time
            strncpy(modemVer.build_time, tgt_GetBuildDateTime(),
                    (BUILD_TIME_BUF_LEN - 1));
            modemVer.build_time[BUILD_TIME_BUF_LEN - 1] = '\0';

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &modemVer, sizeof(modemVer));
        }
        break;
        case AP_GENERAL_CMD_GET_BT_INFO:
        {
            TGT_BT_INFO_T btInfo;
            tgt_GetBtInfo(&btInfo);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &btInfo, sizeof(btInfo));
        }
        break;
        case AP_GENERAL_CMD_SET_BT_INFO:
        {
            BOOL ret;
            TGT_BT_INFO_T btInfo;
            tgt_GetBtInfo(&btInfo);

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(btInfo));

            memcpy(&btInfo, pRecvMsg->param, sizeof(btInfo));
            ret = tgt_SetBtInfo(&btInfo);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, !ret,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_TRACE_ENABLE:
        {
            UINT32 enable;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&enable, pRecvMsg->param, 4);

            if (enable)
            {
                sxs_SetFlowControl(PAL_DBG_PORT_IDX, SXS_RELEASE_TX);
            }
            else
            {
                sxs_SetFlowControl(PAL_DBG_PORT_IDX, SXS_HOLD_TX);
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_GET_TRACE_STATUS:
        {
            UINT32 status;

            if (sxs_GetFlowControlStatus(PAL_DBG_PORT_IDX) & SXS_HOLD_TX)
            {
                status = 0;
            }
            else
            {
                status = 1;
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &status, sizeof(status));
        }
        break;
        case AP_GENERAL_CMD_GET_WIFI_INFO:
        {
            TGT_WIFI_INFO_T wifiInfo;
            tgt_GetWifiInfo(&wifiInfo);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &wifiInfo, sizeof(wifiInfo));
        }
        break;
        case AP_GENERAL_CMD_SET_WIFI_INFO:
        {
            BOOL ret;
            TGT_WIFI_INFO_T wifiInfo;
            tgt_GetWifiInfo(&wifiInfo);

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(wifiInfo));

            memcpy(&wifiInfo, pRecvMsg->param, sizeof(wifiInfo));
            ret = tgt_SetWifiInfo(&wifiInfo);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, !ret,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_GET_BOOTLOGO_NAME:
        {
            extern UINT32 _calib_start;

            struct factory_data_sector *sector = (void *)&_calib_start;
            struct ap_factory_config *apFact = (void *)sector->ap_factory_data;
            UINT32 len = strnlen(apFact->bootlogo_name, FACT_NAME_LEN);
            if (len < FACT_NAME_LEN)
            {
                // Include the null terminated byte
                apFact->bootlogo_name[len++] = '\0';
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, apFact->bootlogo_name, len);
        }
        break;
        case AP_GENERAL_CMD_SET_BOOTLOGO_NAME:
        {
            extern UINT32 _calib_start;

            SYSCMDS_ASSERT(pRecvMsg->paramLen <= FACT_NAME_LEN,
                           "Invalid bootlogo name length: %d", pRecvMsg->paramLen);

            struct factory_data_sector *sector = (void *)&_calib_start;
            struct ap_factory_config *apFact = (void *)sector->ap_factory_data;

            strncpy(apFact->bootlogo_name, (char *)pRecvMsg->param, pRecvMsg->paramLen);
            if (pRecvMsg->paramLen < FACT_NAME_LEN)
            {
                apFact->bootlogo_name[pRecvMsg->paramLen] = '\0';
            }
            hal_ApCommSetFactoryUpdateCmd(HAL_AP_COMM_FACT_UPDATE_AP_FACT);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GENERAL_CMD_GET_MOBILE_BOARD_SN:
        {
            UINT8 sn[sizeof(((TGT_MOBILE_SN_T *)0)->mbSn)];
            UINT8 len = sizeof(sn);

            tgt_GetMobileBoardSn(sn, &len);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, sn, len);
        }
        break;
        case AP_GENERAL_CMD_GET_MOBILE_PHONE_SN:
        {
            UINT8 sn[sizeof(((TGT_MOBILE_SN_T *)0)->mpSn)];
            UINT8 len = sizeof(sn);

            tgt_GetMobilePhoneSn(sn, &len);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, sn, len);
        }
        break;
        default:
            result = FALSE;
            break;
    }

    return result;
}


PROTECTED VOID syscmds_SendRtcTriggerMsg(VOID)
{
    syscmds_IrqSend(SYSCMDS_MOD_ID_GENERAL, MODEM_GENERAL_CMD_RTC_TRIGGER,
                    NULL, 0);
}


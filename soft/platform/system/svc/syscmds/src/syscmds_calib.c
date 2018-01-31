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
#include "syscmds_calib_cmd_id.h"

#include "string.h"

#include "calib_m.h"
#include "hal_ap_comm.h"


// =============================================================================
//  EXTERNAL FUNCTIONS
// =============================================================================

extern VOID calib_DaemonHandleApResponse(BOOL result);


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================

PROTECTED BOOL syscmds_HandleCalibCmd(SYSCMDS_RECV_MSG_T *pRecvMsg)
{
    BOOL result = TRUE;

    switch (pRecvMsg->msgId)
    {
        case AP_CALIB_CMD_FLASH_RESULT:
        {
            UINT32 flashResult;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&flashResult, pRecvMsg->param, 4);

            calib_DaemonHandleApResponse(!!flashResult);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_CALIB_CMD_CALIB_STATUS:
        {
            UINT32 calibStatus[3];

            calibStatus[0] = calib_GetRfCalibStatus();
            calibStatus[1] = calib_GetAudioCalibStatus();
            calibStatus[2] = calib_GetPointers()->sw->param[14];

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, calibStatus, sizeof(calibStatus));
        }
        break;
        case AP_CALIB_CMD_UPDATE_CALIB:
        {
            BOOL ret = calib_DaemonUsrDataProcess();

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, !ret,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_CALIB_CMD_UPDATE_FACTORY:
        {
            hal_ApCommSetFactoryUpdateCmd(HAL_AP_COMM_FACT_UPDATE_FACT);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        default:
            result = FALSE;
            break;
    }

    return result;
}


PUBLIC VOID syscmds_SendCalibStartMsg(VOID)
{
    syscmds_Send(SYSCMDS_MOD_ID_CALIB, MODEM_CALIB_CMD_CALIB_START, NULL, 0);
}


PUBLIC VOID syscmds_SendCalibDoneMsg(VOID)
{
    syscmds_IrqSend(SYSCMDS_MOD_ID_CALIB, MODEM_CALIB_CMD_CALIB_DONE, NULL, 0);
}


PROTECTED VOID syscmds_CheckParamLen(UINT32 modId, UINT32 msgId,
                                     UINT32 recvLen, UINT32 expectedLen)
{
    SYSCMDS_ASSERT(recvLen == expectedLen,
                   "ParamLen=%u but expected %u for mod: %u, msg: %d",
                   recvLen, expectedLen, modId, msgId);
}


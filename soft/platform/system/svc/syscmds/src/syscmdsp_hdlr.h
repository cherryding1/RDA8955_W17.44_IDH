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


#ifndef _SYSCMDSP_HDLR_H_
#define _SYSCMDSP_HDLR_H_

#include "cs_types.h"

// =============================================================================
// MACROS
// =============================================================================

#define SYS_CMD_PARAM_MAX_SIZE              (128)
#define SYS_CMD_IRQ_SEND_PARAM_MAX_SIZE     (8)


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// SYSCMDS_RECV_MSG_T
// -----------------------------------------------------------------------------
/// The structure to save received system command messages
// =============================================================================
typedef struct
{
    UINT16 modId;
    UINT32 reqId;
    UINT32 retVal;
    UINT16 msgId;
    UINT16 paramLen;
    UINT8 param[SYS_CMD_PARAM_MAX_SIZE];
} SYSCMDS_RECV_MSG_T;


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

PROTECTED VOID syscmds_CheckParamLen(UINT32 modId, UINT32 msgId,
                                     UINT32 recvLen, UINT32 expectedLen);

PROTECTED BOOL syscmds_HandleGeneralCmd(SYSCMDS_RECV_MSG_T *pRecvMsg);

PROTECTED BOOL syscmds_HandleCalibCmd(SYSCMDS_RECV_MSG_T *pRecvMsg);

PROTECTED BOOL syscmds_HandlePmCmd(SYSCMDS_RECV_MSG_T *pRecvMsg);

PROTECTED BOOL syscmds_HandleAudioCmd(SYSCMDS_RECV_MSG_T *pRecvMsg);

PROTECTED BOOL syscmds_HandleTsCmd(SYSCMDS_RECV_MSG_T *pRecvMsg);

PROTECTED BOOL syscmds_HandleGpioCmd(SYSCMDS_RECV_MSG_T *pRecvMsg);


PROTECTED VOID syscmds_SendRtcTriggerMsg(VOID);


PROTECTED VOID syscmds_Send(UINT16 modId, UINT16 msgId, VOID *param, UINT16 len);

PROTECTED VOID syscmds_IrqSend(UINT16 modId, UINT16 msgId, VOID *param, UINT16 len);

PROTECTED VOID syscmds_Reply(UINT16 modId, UINT32 reqId, UINT32 retVal,
                             UINT16 msgId, VOID *param, UINT16 len);


#endif // _SYSCMDSP_HDLR_H_


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



#ifndef __AT_CMD_CC_H__
#define __AT_CMD_CC_H__

#include <csw.h>

#define AT_CC_MAX_NUM              7

struct AT_CMD_PARA;

#define CC_Status_Idle   0
#define CC_Status_Request   1
#define CC_Status_Alert   2
#define CC_Status_Connect   3
#define CC_Status_Hold   4

// [[ yy [add] 2008-04-23 for command implementation reorgnization
#define AT_CMD_D_ParaMaxLen                 41
#define AT_CMD_D_OPTYPE_ERR                -1
#define AT_CMD_D_OPTYPE_VOICECALL     1
#define AT_CMD_D_OPTYPE_GPRSDIAL       2
#define AT_CMD_D_OPTYPE_NOTSUPPORT 100

#define AT_GPRS_DIALSTR1 "T*98"
#define AT_GPRS_DIALSTR2 "T*99"
#define AT_GPRS_DIALSTR3 "t*98"
#define AT_GPRS_DIALSTR4 "t*99"
#define AT_GPRS_DIALSTR5 "*98"
#define AT_GPRS_DIALSTR6 "*99"

#ifdef AT_USER_DBS
typedef enum
{
    DBS_DIAL_TONE,
    DBS_DIAL_CONGESTION,
    DBS_DIAL_BUSY
} DBS_DIAL_PLAY;
VOID AT_CC_CmdFunc_SENDSOUND(AT_CMD_PARA *pParam);
#endif
INT8 AT_CC_CmdFunc_D_ParaParse(UINT8 *pPara, UINT8 *pDialNum);

// ]] yy [add] 2008-04-23 for command implementation reorgnization

VOID AT_CC_AsyncEventProcess(COS_EVENT *pEvent);

VOID AT_CC_CmdFunc_D(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_H(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_A(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_CLCC(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_VTS(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_CHUP(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_VTD(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_CRC(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_VTSEX(AT_CMD_PARA *pParam);
// [+]for AT^DLST redial last call 2007.11.09
VOID AT_CC_CmdFunc_DLST(AT_CMD_PARA *pParam);

VOID AT_CC_CmdFunc_CHLD(AT_CMD_PARA *pParam);
VOID AT_CC_CmdFunc_O(AT_CMD_PARA *pParam);  // hameina[+]2008-4-29

#endif

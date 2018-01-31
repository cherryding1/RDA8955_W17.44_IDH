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



#ifndef __AT_CMD_OTHER_H__
#define __AT_CMD_OTHER_H__

struct COS_EVENT;

struct AT_CMD_PARA;

extern VOID AT_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                         UINT8 nDLCI, UINT8 nSim);
extern VOID AT_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
UINT32 AT_SetCmeErrorCode(UINT32 nCfwErrorCode, BOOL bSetParamValid);

VOID AT_ID_CmdFunc_CGMM(AT_CMD_PARA *pParam);
VOID AT_ID_CmdFunc_GMM(AT_CMD_PARA *pParam);
VOID AT_ID_CmdFunc_CGMR(AT_CMD_PARA *pParam);
VOID AT_ID_CmdFunc_GMR(AT_CMD_PARA *pParam);
VOID AT_ID_CmdFunc_CGMI(AT_CMD_PARA *pParam);
VOID AT_ID_CmdFunc_GMI(AT_CMD_PARA *pParam);

#ifndef AT_USER_DBS
VOID AT_ID_CmdFunc_CGSN(AT_CMD_PARA *pParam);
#endif
VOID AT_ID_CmdFunc_GSN(AT_CMD_PARA *pParam);

VOID AT_ID_CmdFunc_I(AT_CMD_PARA *pParam);

VOID AT_ID_CmdFunc_CIMI(AT_CMD_PARA *pParam);

VOID AT_ID_CmdFunc_CGBV(AT_CMD_PARA *pParam);
VOID AT_ID_AsyncEventProcess(COS_EVENT *pEvent);

#endif


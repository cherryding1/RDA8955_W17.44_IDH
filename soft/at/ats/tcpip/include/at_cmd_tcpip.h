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



#ifndef __AT_CMD_TCPIP_H__
#define __AT_CMD_TCPIP_H__

VOID AT_TCPIP_CmdFunc_CIPMUX(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPSGTXT(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPSTART(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPCLOSE(AT_CMD_PARA *pstPara);

VOID AT_TCPIP_CmdFunc_CIPSTATUS(AT_CMD_PARA *pstPara);

VOID AT_TCPIP_CmdFunc_CIFSR(AT_CMD_PARA *pstPara);

VOID AT_TCPIP_CmdFunc_CIPSHUT(AT_CMD_PARA *pstPara);

VOID AT_TCPIP_CmdFunc_CIICR(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPSEND(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPQSEND(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CSTT(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPATS(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CDNSCFG(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPSPRT(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_CIPHEAD(AT_CMD_PARA *pParam);

VOID AT_TCPIP_Result_OK(UINT32 uReturnValue,
                        UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI);

VOID AT_TCPIP_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI);

VOID AT_TCPIP_CmdFunc_CDNSGIP(AT_CMD_PARA *pParam);

VOID AT_TCPIP_CmdFunc_PING(AT_CMD_PARA *pParam);

#ifdef LTE_NBIOT_SUPPORT
VOID AT_TCPIP_CmdFunc_CTDCP(AT_CMD_PARA *pParam);
void AT_TCPIP_CmdFunc_IPFLT(AT_CMD_PARA *pParam);
VOID AT_TCPIP_CmdFunc_PINGSTOP(AT_CMD_PARA *pParam);
#endif //LTE_NBIOT_SUPPORT
VOID AT_TCPIP_CmdFunc_CIPTKA(AT_CMD_PARA *pParam);
VOID AT_TCPIP_CmdFunc_CIPRDTIMER(AT_CMD_PARA *pParam);

#endif


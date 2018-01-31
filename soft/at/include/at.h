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



#ifndef __AT_H__
#define __AT_H__

#ifdef __MODEM_NO_AP_
#undef CHIP_HAS_AP
#endif

#include "stddef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "csw.h"
#include "cfw.h"
#include "at_feature.h"
#include "at_define.h"
#include "at_errcode.h"
#include "at_utility.h"
#include "at_common.h"
#include "at_trace.h"
#include "at_cmd_engine.h"
#include "sarft_sign.h"

extern UINT32 AT_GC_CfgGetEchoMode(UINT8 *pMode);
extern UINT32 AT_GC_CfgGetResultCodePresentMode(UINT8 *pMode);
extern UINT32 AT_GC_CfgGetResultCodeFormatMode(UINT8 *pMode);
extern UINT8 AT_GC_CfgGetS3Value(VOID);
extern UINT8 AT_GC_CfgGetS4Value(VOID);
extern UINT8 AT_GC_CfgGetS5Value(VOID);
extern UINT8 AT_GC_CfgGetS0Value(VOID);
extern BOOL AT_GC_GetAudioTest(VOID); // yangtt at 2008-04-25 for bug 8171

#define DEBUG_AT
extern INT32 CFW_FTPProcess( COS_EVENT ev);
extern INT32 CFW_MQTTProcess( COS_EVENT ev);
void TCpIPInit(void);
VOID AT_EMOD_CFGUPDATE_Indictation(COS_EVENT *pParam);

#ifdef __BT_RDABT__
UINT32 rda5868_uart_rx(VOID);
VOID RDA5868_uart_tx_wakeup(VOID);

#endif

extern HANDLE g_hAtTask;

#endif

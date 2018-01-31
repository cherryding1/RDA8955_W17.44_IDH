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



#ifndef _AT_CMD_EX_WIFI_H_
#define _AT_CMD_EX_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                          INCLUDES
 **************************************************************************************************/
#include "at_common.h"



#ifdef __cplusplus
}
#endif

typedef enum
{
    NO_TEST,
    RX_TEST,
    TX_TEST,
    OTHER
} WIFI_TEST_MODE;

typedef enum
{
    WIFI_TYPE_INFRA,
    WIFI_TYPE_ADHOC,
    WIFI_TYPE_AP
} WIFI_NETWORK_TYPE;

void wifi_test_mode_open(WIFI_TEST_MODE mode);
VOID AT_WIFI_CmdFunc_WPOWER(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_WSCAN(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_WJOIN(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_WADDR(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_WLEAV(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_WPRT(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_QMAC(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_BSSID(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_LKSTT(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_SSID(AT_CMD_PARA *pParam);
VOID AT_TCPIP_CmdFunc_WRSSI(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_SMARTCONFIG(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_SMARTCONFIG_RSP(AT_CMD_PARA *pParam);
VOID AT_WIFI_CmdFunc_SNIFFER(AT_CMD_PARA *pParam);
#ifdef USB_WIFI_SUPPORT
extern VOID wifi_PowerOn(UINT8 *local_mac);

#endif

#endif


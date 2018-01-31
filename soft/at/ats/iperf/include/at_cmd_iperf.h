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

#ifdef LTE_NBIOT_SUPPORT
#ifndef __AT_CMD_IPERF_H__
#define __AT_CMD_IPERF_H__

#include "sockets.h"

typedef struct UDP_datagram_Tag
{
    INT32	id;
    UINT32  tv_sec;
    UINT32  tv_usec;
}UDP_datagram;

typedef struct client_hdr_Tag
{
#define HEADER_VERSION1 0x80000000
#define RUN_NOW  0x00000001
    INT32	flags;
    INT32 numThreads;
    INT32 mPort;
    INT32 bufferLen;
    INT32 mWindBand;
    INT32 mAmount;
}client_hdr;


typedef struct lwiperf_settings{
    UINT8       role;  //0 is client, 1 is server   -c
    UINT8       *host;  //-c ipaddr
    UINT64      udpRate;   //-b
    UINT64      amount;  //-t
    DOUBLE      interval;   //-i
    UINT16      remote_port; //-p
    UINT16      bufferLen; //-l
    INT8        flags;
	
    UINT8       *mBuf;
    UINT8        udpFinSendingCount;
    BOOL         udpFinSendTimerRun;
    SOCKET       socket;
	union
	{
        CFW_TCPIP_SOCKET_ADDR   addr4;
		CFW_TCPIP_SOCKET_ADDR6  addr6;
	}remoteAddr;
    union
	{
        CFW_TCPIP_SOCKET_ADDR   addr4;
		CFW_TCPIP_SOCKET_ADDR6  addr6;
	}localAddr;
}Lwiperf_setting;

#define    FLAG_BUFLENSET		0x00000001
#define    FLAG_DOMAIN_IPV6		0x00000002
#define    FLAG_UDP				0x00000004
#define    FLAG_MODTIME			0x00000008

#define    isBufLenSet(setting)		((setting->flags & FLAG_BUFLENSET)!=0)
#define    isIPV6(setting)			((setting->flags & FLAG_DOMAIN_IPV6)!=0)
#define    isUDP(setting)			((setting->flags & FLAG_UDP)!=0)
#define    isModeTime(setting)		((setting->flags & FLAG_MODTIME)!=0)

#define    setBufLenSet(setting)	setting->flags |= FLAG_BUFLENSET
#define    setIpV6(setting)			setting->flags |= FLAG_DOMAIN_IPV6
#define    setUDP(setting)			setting->flags |= FLAG_UDP
#define    setModeTime(setting)	    setting->flags |= FLAG_MODTIME

#define    iperf_kMillion                  1000000

VOID AT_IPERF_AsyncEventProcess(COS_EVENT *pEvent);
VOID AT_TCPIP_CmdFunc_IPERF(AT_CMD_PARA *pParam);
VOID Udp_Sending_Fin_Timeout(VOID *para);
#endif //__AT_CMD_IPERF_H__
#endif //LTE_NBIOT_SUPPORT


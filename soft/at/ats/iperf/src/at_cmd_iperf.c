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
#include "stdio.h"
#include "at.h"
#include "cfw.h"
#include "at_module.h"
#include "at_cmd_gprs.h"
#include "at_cmd_iperf.h"
#include "base_prv.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_cmd_nw.h"
#include "at_dispatch.h"

static const u8_t iperf_buf[1600] = {
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
  '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
};

CONST DOUBLE kSecs_to_usecs = 1e6;
CONST INT32  kBytes_to_Bits = 8;

CONST UINT64 kKilo_to_Unit = 1024;
CONST UINT64 kMega_to_Unit = 1024*1024;
CONST UINT64 kGiga_to_Unit = 1024*1024*1024;
CONST UINT64 kkilo_to_Unit = 1000;
CONST UINT64 kmega_to_Unit = 1000*1000;
CONST UINT64 kgiga_to_Unit = 1000*1000*1000;

Lwiperf_setting    iperf_setting = {0};
BOOL               iperf_ongoing = FALSE;
extern int         gettimeofday(struct timeval *tv, struct timezone *tz);
extern UINT8       CFW_GprsGetPdpIpv4Addr(UINT8 nCid, UINT8 *nLength, UINT8 *ipv4Addr, CFW_SIM_ID nSimID);
const ip6_addr_t   *CFW_GprsGetPdpIpv6Address(UINT8 nCid, UINT8 nSim);


static UINT64   byte_atoi(UINT8 *inString)
{
    UINT16      len = 0;
    UINT64      theNum = 0;
    UINT8       suffix = 0;

    //sscanf(inString, "%lf%c", &theNum, &suffix);
    len = AT_StrLen(inString);
    suffix = inString[len -1];
    inString[len -1] = 0;
    theNum = atoi(inString);

    switch(suffix)
    {
        case 'G': theNum *=kGiga_to_Unit;  break;
	    case 'M': theNum *=kMega_to_Unit; break;
	    case 'K': theNum *=kKilo_to_Unit; break;
	    case 'g': theNum *=kgiga_to_Unit; break;
	    case 'm':theNum *=kmega_to_Unit; break;
	    case 'k': theNum *=kkilo_to_Unit; break;
	    default: break;
    }
    return (UINT64)theNum;
}

static BOOL iperf_time_before(struct timeval time1, struct timeval time2)
{
    return ((time1.tv_sec < time2.tv_sec) ||
		 (
		     (time1.tv_sec == time2.tv_sec) &&
		     (time1.tv_usec < time2.tv_usec)
		 ));
}

static INT32 iperf_sub_usec(struct timeval time1, struct timeval time2)
{
    return (time1.tv_sec - time2.tv_sec)*iperf_kMillion + (time1.tv_usec - time2.tv_usec);
}


static VOID AT_IPERF_Result_OK(UINT32 uReturnValue,
                        UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = NULL;
    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);
    if(pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }
    return;
}

static VOID AT_IPERF_PARAMS_PRINT(Lwiperf_setting *s)
{
    AT_TC(g_sw_TCPIP, "====AT_IPERF_PARAMS_PRINT BEGIN====");
    AT_TC(g_sw_TCPIP, "s->role: %d", s->role);
    AT_TC(g_sw_TCPIP, "s->host: %s", s->host);
    AT_TC(g_sw_TCPIP, "s->udpRate: %ld", s->udpRate);
    AT_TC(g_sw_TCPIP, "s->amount: %ld", s->amount);
    AT_TC(g_sw_TCPIP, "s->remote_port: %d", s->remote_port);
    AT_TC(g_sw_TCPIP, "s->bufferLen: %d", s->bufferLen);
	AT_TC(g_sw_TCPIP, "s->flags: 0x%x", s->flags);
    AT_TC(g_sw_TCPIP, "====AT_IPERF_PARAMS_PRINT END====");
}

static BOOL AT_IsUdpSendFinTimer(VOID)
{
    return iperf_setting.udpFinSendTimerRun;
}

static BOOL AT_KillUdpSendFinTimer(VOID)
{
    BOOL ret = FALSE;

    AT_TC(g_sw_UART, "AT_KillUdpSendFinTimer");
    if (AT_IsUdpSendFinTimer())
    {
        iperf_setting.udpFinSendTimerRun = FALSE;
        at_StopCallbackTimer(Udp_Sending_Fin_Timeout, &iperf_setting);
    }

    return ret;
}

static BOOL AT_SetUdpsendFinTimer(UINT32 nElapse)
{
    AT_TC(g_sw_UART, "AT_SetUdpsendFinTimer");
    if (AT_IsUdpSendFinTimer())
        AT_KillUdpSendFinTimer();
    iperf_setting.udpFinSendTimerRun = at_StartCallbackTimer(nElapse * 1000, Udp_Sending_Fin_Timeout, &iperf_setting);
    return iperf_setting.udpFinSendTimerRun;
}

static UINT8 Send_Udp_Fin(Lwiperf_setting *s)
{
    INT8 iResult = -1;
    AT_TC(g_sw_TCPIP, "Send_Udp_Fin\n");
    iResult = CFW_TcpipSocketSendto(s->socket, s->mBuf, (UINT16)(isBufLenSet(s)?s->bufferLen:1000), 0, &s->remoteAddr.addr4, s->remoteAddr.addr4.sin_len);
     if(iResult == SOCKET_ERROR)
     {
         CFW_TcpipSocketClose(s->socket);
         s->socket = INVALID_SOCKET;
         return 1;
     }
     s->udpFinSendingCount++;
     if(s->udpFinSendingCount > 10)
     {
         AT_KillUdpSendFinTimer();
         CFW_TcpipSocketClose(s->socket);
         s->socket = INVALID_SOCKET;
         return 1;
     }
     AT_SetUdpsendFinTimer(1);
     return 0;
}

static UINT8 AT_IPERF_CONNECTING(Lwiperf_setting *s, UINT8 DLCI)
{
    UINT32      destIp;
	ip6_addr_t  destIpv6;
	ip6_addr_t  *localIpv6;
    UINT8 uiLen = 0;
    UINT8 acIPAddr[18]={0};
    UINT8 iResult = 0;
	AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(DLCI);
    if(isIPV6(s) & isUDP(s)) {
        s->socket= CFW_TcpipSocket(AF_INET6, CFW_TCPIP_SOCK_DGRAM, CFW_TCPIP_IPPROTO_UDP);
    } else {
        s->socket = CFW_TcpipSocket(AF_INET, CFW_TCPIP_SOCK_DGRAM, CFW_TCPIP_IPPROTO_UDP);
    } 
    if(INVALID_SOCKET == s->socket)
    {
        AT_TC(g_sw_TCPIP, "exe       in AT_IPERF_CONNECT(), CFW_TcpipSocket() failed\n");
		AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
        return 1;
    }
    if(!isIPV6(s))
    {
       s->remoteAddr.addr4.sin_family = CFW_TCPIP_AF_INET;
       s->remoteAddr.addr4.sin_port   = htons(s->remote_port);
       s->remoteAddr.addr4.sin_len = SIZEOF(CFW_TCPIP_SOCKET_ADDR);
       destIp              = CFW_TcpipInetAddr(s->host);
       if (IPADDR_NONE == destIp)
       {
           CFW_TcpipSocketClose(s->socket);
           AT_TC(g_sw_TCPIP, "exe       in AT_IPERF_CONNECT(), CFW_TcpipInetAddr() failed\n");
		   AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
           return 2;
       }
       else
       {
           s->remoteAddr.addr4.sin_addr.s_addr = destIp;
       }
	   
	   CFW_GprsGetPdpIpv4Addr(1, &uiLen, acIPAddr, 0);
	   s->localAddr.addr4.sin_len         = SIZEOF(CFW_TCPIP_SOCKET_ADDR);;
       s->localAddr.addr4.sin_family      = CFW_TCPIP_AF_INET;
       s->localAddr.addr4.sin_port        = 0;
	   s->localAddr.addr4.sin_addr.s_addr = htonl(acIPAddr[0] << 24 | acIPAddr[1] << 16 | acIPAddr[2] << 8 | acIPAddr[3]);
	   iResult = CFW_TcpipSocketBind(s->socket, &s->localAddr.addr4, sizeof(CFW_TCPIP_SOCKET_ADDR));
    }
    else
    {
        s->remoteAddr.addr6.sin6_family = AF_INET6;
		s->remoteAddr.addr6.sin6_port = htons(s->remote_port);
		s->remoteAddr.addr6.sin6_len = SIZEOF(CFW_TCPIP_SOCKET_ADDR6);
		if(inet6_aton(s->host, &destIpv6) == 0)
		{
		    CFW_TcpipSocketClose(s->socket);
            AT_TC(g_sw_TCPIP, "exe       in AT_IPERF_CONNECT(), CFW_TcpipInetAddr() failed\n");
			AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
            return 2;
		}
		else
		{
		    AT_MemCpy(s->remoteAddr.addr6.sin6_addr.un.u32_addr, destIpv6.addr, SIZEOF(u32_t)*4);
		}
		localIpv6 = CFW_GprsGetPdpIpv6Address(1,0);
		if(localIpv6)
		{
		    s->localAddr.addr6.sin6_len         = SIZEOF(CFW_TCPIP_SOCKET_ADDR6);;
            s->localAddr.addr6.sin6_family      = AF_INET6;
            s->localAddr.addr6.sin6_port        = 0;
			AT_MemCpy(s->localAddr.addr6.sin6_addr.un.u32_addr, localIpv6->addr,SIZEOF(u32_t)*4);
			iResult = CFW_TcpipSocketBind(s->socket, &s->localAddr.addr6, sizeof(CFW_TCPIP_SOCKET_ADDR6));
		}
		else
		{
		    CFW_TcpipSocketClose(s->socket);
            AT_TC(g_sw_TCPIP, "exe       in AT_IPERF_CONNECT(), CFW_GprsGetPdpIpv6Address() failed\n");
			AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
            return 3;
		}
    }
    if(iResult == 0)
    {
        if(isIPV6(s))
        {
            iResult = CFW_TcpipSocketConnect(s->socket, &s->remoteAddr.addr6, SIZEOF(CFW_TCPIP_SOCKET_ADDR6));
        }
		else
		{
            iResult = CFW_TcpipSocketConnect(s->socket, &s->remoteAddr.addr4, SIZEOF(CFW_TCPIP_SOCKET_ADDR));
		}
	    if(SOCKET_ERROR == iResult)
        {
            CFW_TcpipSocketClose(s->socket);
            AT_TC(g_sw_TCPIP, "exe       in AT_IPERF_CONNECT(), CFW_TcpipSocketConnect() failed\n");
			AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
            return 4;
         }
    }
    else
    {
        CFW_TcpipSocketClose(s->socket);
        AT_TC(g_sw_TCPIP, "exe       in AT_IPERF_CONNECT(), CFW_TcpipSocketBind() failed\n");
		AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
	    return 5;
    }
	 
}

static UINT8 AT_IPERF_SENDING(Lwiperf_setting *s, UINT8 DLCI)
{
    struct timeval mTime, packetTime, lastTime;
    INT32   packetId = 0;
    INT32     delay_target = 0, adjust = 0, delay = 0;
    UDP_datagram *UDPhdr = (UDP_datagram *)s->mBuf;
    INT8 iResult = -1;
    DOUBLE  sec = s->amount/100.0;
    gettimeofday(&mTime, NULL);
    iperf_ongoing = TRUE;
    mTime.tv_sec += (long)sec;
    mTime.tv_usec += (long)((sec - ((long)sec)) *iperf_kMillion);
    if(mTime.tv_usec >= iperf_kMillion)
    {
        mTime.tv_usec -= iperf_kMillion;
	    mTime.tv_sec++;
    }
    if(isUDP(s))
    {
        delay_target = (INT32) (s->bufferLen *((kSecs_to_usecs*kBytes_to_Bits)/s->udpRate));
	    if(delay_target < 0 || delay_target > (INT32)1*kSecs_to_usecs)
	    {
	        delay_target = (INT32)kSecs_to_usecs*1;
	    }
    }
    
    gettimeofday(&lastTime, NULL);
    do{
        gettimeofday(&packetTime, NULL);
	    if(isUDP(s)){
	        UDPhdr->id = htonl(packetId++);
            UDPhdr->tv_sec = htonl(packetTime.tv_sec);
	        UDPhdr->tv_usec = htonl(packetTime.tv_usec);
	        adjust = delay_target + iperf_sub_usec(lastTime, packetTime);
	        lastTime.tv_sec = packetTime.tv_sec;
	        lastTime.tv_usec = packetTime.tv_usec;
	        if(adjust > 0 || delay > 0)
	        {
	            delay += adjust;
	        }
	    }
	    if(isIPV6(s))
	    {
	        iResult = CFW_TcpipSocketSendto(s->socket, s->mBuf, \
				(UINT16)(isBufLenSet(s)?s->bufferLen:1000), 0, &s->remoteAddr.addr6, s->remoteAddr.addr6.sin6_len);
	    }
		else
		{
	        iResult = CFW_TcpipSocketSendto(s->socket, s->mBuf, \
				(UINT16)(isBufLenSet(s)?s->bufferLen:1000), 0, &s->remoteAddr.addr4, s->remoteAddr.addr4.sin_len);
		}
	    if(iResult == SOCKET_ERROR)
	    {
	        AT_FREE(s->mBuf);
	        AT_TC(g_sw_TCPIP, "AT_IPERF_SENDING2 sendto fail\n");
	        CFW_TcpipSocketClose(s->socket);
	        s->socket = INVALID_SOCKET;
	        return 1;
	    }

	    if(delay > 0)
	    {
	        AT_TC(g_sw_TCPIP, "AT_IPERF_SENDING3 delay: %d\n", delay);
	        COS_Sleep(delay/1000);
	    }
      }while(iperf_time_before(packetTime, mTime));
      AT_TC(g_sw_TCPIP, "AT_IPERF_SENDING4 : sending over\n");

      if(isUDP(s)){
	    UDPhdr->id = htonl(-packetId);
        UDPhdr->tv_sec = htonl(packetTime.tv_sec);
	    UDPhdr->tv_usec = htonl(packetTime.tv_usec);
	    return Send_Udp_Fin(s);
     }

     return 0;
}

static UINT8 AT_IPERF_SETTING_GENERATECLIENTPARAS(Lwiperf_setting *s, UINT8 DLCI)
{
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(DLCI);
    UDP_datagram *UDPhdr = NULL;
    client_hdr      *temp_hdr = NULL;
    s->mBuf = AT_MALLOC(isBufLenSet(s)?s->bufferLen:1000);
    if(s->mBuf == NULL)
    {
        CFW_TcpipSocketClose(s->socket);
        s->socket = INVALID_SOCKET;
		AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
	    return 1;
    }
    AT_MemZero(s->mBuf, (isBufLenSet(s)?s->bufferLen:1000));
    AT_MemCpy(s->mBuf, iperf_buf, (isBufLenSet(s)?s->bufferLen:1000));
    UDPhdr = (UDP_datagram*)s->mBuf;
    temp_hdr = (client_hdr *)(UDPhdr + 1);
    if(s->role == 0)
        temp_hdr->flags = 0;//htonl(HEADER_VERSION1);
    temp_hdr->bufferLen = htonl(isBufLenSet(s)?s->bufferLen:1000);
    if(isUDP(s))
    {
        temp_hdr->mWindBand = htonl(s->udpRate);
    }
    else
    {
        AT_FREE(s->mBuf);
	    CFW_TcpipSocketClose(s->socket);
        s->socket = INVALID_SOCKET;
		AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
	    return 2;
    }
    temp_hdr->mPort = htonl(s->remote_port);
    if(isModeTime(s))
    {
        temp_hdr->mAmount = htonl(-(long)s->amount);
    }
    else
    {
        AT_FREE(s->mBuf);
	    CFW_TcpipSocketClose(s->socket);
        s->socket = INVALID_SOCKET;
		AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
	    return 3;
    }
    temp_hdr->numThreads = 1;
    return 0;
}


VOID AT_TCPIP_CmdFunc_IPERF(AT_CMD_PARA *pParam)
{
    UINT8 nNumParamter         = pParam->paramCount;
    AT_TC(g_sw_TCPIP, "AT+IPERF starting");
    Lwiperf_setting    *s = &iperf_setting;
    AT_MemZero(s, SIZEOF(Lwiperf_setting));
	iperf_ongoing = FALSE;
    UINT8  *udprate = NULL;
    #define BUFFER_SIZE 63
    UINT16 nBuffer = BUFFER_SIZE;
    UINT16   amount = 0;
	UINT8    iptype = 4;
    UINT8     pdpState = 0;
	bool      paramok = TRUE;
    if (NULL == pParam)
    {
        AT_TC(g_sw_TCPIP, "AT+IPERF: pParam = NULL");
		AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        return;
    }

    if (AT_CMD_SET == pParam->iType)
    {
        CFW_GetGprsActState(1, &pdpState, 0);
	    if(pdpState == 0)
	    {
	       AT_TC(g_sw_TCPIP, " AT_TCPIP_CmdFunc_IPERF0: Parameter = %s\n", pParam->pPara);
		   AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_GPRS_OPTION_NOT_SUPPORTED));
	       return;
	    }
        if (nNumParamter > 8)
        {
            AT_TC(g_sw_TCPIP, " AT_TCPIP_CmdFunc_IPERF1: Parameter = %s\n", pParam->pPara);
			AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            return;
        }
	    if(nNumParamter >=1)
	    {
	        s->role = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if(s->role == 0) //client
            {
                if(nNumParamter >=2)
                {
                   s->host = at_ParamStr(pParam->params[1], &paramok);
                }
				

                if(nNumParamter >=3)
		        {
					s->remote_port = at_ParamUintInRange(pParam->params[2], 0, 65535, &paramok);
		        }
				
		        if(nNumParamter >=4)
		        {
					s->bufferLen = at_ParamUintInRange(pParam->params[3], 0, 1600, &paramok);
		            setBufLenSet(s);
		        }
		        if(nNumParamter >=5)
		        {
					udprate = at_ParamStr(pParam->params[4], &paramok);
		            s->udpRate = byte_atoi(udprate);
		            setUDP(s);
		        }
		        if(nNumParamter >=6)
		        {
					amount = at_ParamUintInRange(pParam->params[5], 0, 65535, &paramok);
		            s->amount = amount * 100.0;
		            setModeTime(s);
		        }
				if(nNumParamter >=7)
		        {
					iptype = at_ParamUintInRange(pParam->params[6], 4, 6, &paramok);
		            if((iptype !=4) || (iptype != 6))
		            {
		                AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_IPERF9: pParam->pPara = %s\n", pParam->pPara);
						AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
						return;
		            }
				    else if(iptype == 6)
				    {
				        setIpV6(s);
				    }
		        }
            }
	        else  //server
	        {
	            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT));
	        }
	   }

	   AT_IPERF_PARAMS_PRINT(s);
       s->socket = INVALID_SOCKET;
	   if(0 == AT_IPERF_CONNECTING(s, pParam->nDLCI))
	   {
	     if(0 == AT_IPERF_SETTING_GENERATECLIENTPARAS(s, pParam->nDLCI))
	     {
	         if( 0 ==AT_IPERF_SENDING(s, pParam->nDLCI))
	         {
	             AT_IPERF_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
	         }
		     else
		     {
		         AT_FREE(s->mBuf);
				 AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
		         //AT_IPERF_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
		     }
	     }
	 }
	 

    }
    else if(AT_CMD_TEST == pParam->iType)
    { 
	    UINT8* pTestRsp = "+IPERF: 0,\"remote_ip\", remote_port, packet_len, \"bandwidth\", duration time[sec], iptype[4,6]";
		at_CmdRespInfoText(pParam->engine, pTestRsp);
		at_CmdRespOK(pParam->engine);
	    return;
    }
    return;
}

VOID AT_IPERF_AsyncEventProcess(COS_EVENT *pEvent)
{
    UINT8 uaRspStr[30] = { 0, };
    INT32 iResult      = 0;
    CFW_EVENT CfwEv;
    UINT8 nSim; // = (UINT8 )(pEvent->nParam3&0x000000ff);
    Lwiperf_setting    *s = &iperf_setting;
    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    nSim = CfwEv.nFlag;
	AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(CfwEv.nUTI);
    switch (CfwEv.nEventId)
    {
        case EV_CFW_IPERF_REV_DATA_IND:
	    do
        {
            iResult = CFW_TcpipSocketRecv(s->socket, s->mBuf, s->bufferLen, nSim);
            if (SOCKET_ERROR == iResult)
            {
                AT_FREE(s->mBuf);
		        CFW_TcpipSocketClose(s->socket);
                s->socket = INVALID_SOCKET;
                AT_TC(g_sw_TCPIP, "EV_CFW_IPERF_REV_DATA_IND, CFW_TcpipSocketRecv error");
				AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_EXE_FAIL));
                return;
            }
        }
        while(iResult > 0);

        if(iResult == 0)
        {
            AT_TC(g_sw_TCPIP, "iperf over");
            AT_FREE(s->mBuf);
	        AT_KillUdpSendFinTimer();
            CFW_TcpipSocketClose(s->socket);
            s->socket = INVALID_SOCKET;
			at_CmdRespOK(engine);
        }
	 break;
	 default: break;
    }
}

VOID Udp_Sending_Fin_Timeout(VOID *para)
{
    Send_Udp_Fin(para);
}
#endif


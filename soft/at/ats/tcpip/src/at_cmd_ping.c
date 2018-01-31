#ifdef AT_PING_SUPPORT
#include "stdio.h"
#include "at.h"
#include "cfw.h"
#include "at_module.h"
#include "at_cmd_gprs.h"
#include "at_cmd_tcpip.h"
#include "base_prv.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_cmd_nw.h"
#include "sockets.h"

//following global variables just used in AT+PING command
UINT16 AT_PING_TOTAL_NUM = 5;
UINT16 AT_PING_ECHO_NUM = 0;
ip_addr_t AT_PING_IP;
UINT8  AT_PING_TIMEOUT = 5;
UINT16 AT_PING_PACKET_LEN = 16;
UINT8 AT_PING_TTL = 0;
UINT16 AT_PING_ID = 0;
UINT16 AT_PING_SEQNO = 0;
SOCKET AT_PING_SOCKET = INVALID_SOCKET;
UINT8 AT_PING_CID = 0;


VOID ping_timeout(UINT32 *param);
#ifdef LTE_NBIOT_SUPPORT
BOOL ping_socket_is_invalid();
#endif
UINT8 AT_SendIP_ICMP_Ping(SOCKET socketfd, UINT8 nDLCI);

static VOID AT_PING_AsyncEventProcess(COS_EVENT *CosEv)
{
    UINT8 outStr[25] = {0};
    AT_TC(g_sw_TCPIP, "AT_PING_AsyncEventProcess Got %s: %d,0x%x,0x%x,0x%x",
        TS_GetEventName(CosEv->nEventId),CosEv->nEventId,CosEv->nParam1,CosEv->nParam2,CosEv->nParam3);
    UINT8 uSocket = (UINT8)CosEv->nParam1;    
    UINT8 nDLC = (UINT8)CosEv->nParam3;
    switch (CosEv->nEventId)
    {
        case EV_CFW_TCPIP_SOCKET_CONNECT_RSP:
            break;            
        case EV_CFW_TCPIP_SOCKET_CLOSE_RSP:
            AT_PING_SOCKET = INVALID_SOCKET;
            break;
        case EV_CFW_TCPIP_ERR_IND:

            AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLC);
            break;

        case EV_CFW_TCPIP_CLOSE_IND:
            
            break;
        case EV_CFW_ICMP_DATA_IND:
        {
            UINT8 Response[100]= {0};
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("pEvent->nParam1 = %d, pEvent->nParam2 = %d"), CosEv->nParam1 , CosEv->nParam2);
    
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("AT_PING_ID = %d"), AT_PING_ID);
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("AT_PING_ECHO_NUM = %d"), AT_PING_ECHO_NUM);
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("AT_PING_TOTAL_NUM = %d"), AT_PING_TOTAL_NUM);
            COS_StopCallbackTimer(CSW_AT_TASK_HANDLE,ping_timeout,nDLC);
#ifdef LTE_NBIOT_SUPPORT
            if(ping_socket_is_invalid() == TRUE)
	        {
	            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("ping socket is invalid, ignore this ping response!"));
	            return;
	        }
#endif
            INT8 *pcIpAddr = ipaddr_ntoa(&AT_PING_IP);
    
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("AT_PING_IP = %s"), pcIpAddr);
            UINT16 id = CosEv->nParam2 >> 16;
            UINT16 seqno = CosEv->nParam2 & 0xFFFF;
            if((id != AT_PING_ID) || (seqno != AT_PING_SEQNO))
            {
#ifdef LTE_NBIOT_SUPPORT
				CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("maybe old ping response AT_PING_SEQNO = %d,seqno = %d"), AT_PING_SEQNO, seqno);
				CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("maybe old ping response >>>pEvent->nParam2 = %d, AT_PING_ID = %d"), id, AT_PING_ID);
#else
                CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("AT_PING_SEQNO = %d,seqno = %d"), AT_PING_SEQNO, seqno);
                CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT(">>>pEvent->nParam2 = %d, AT_PING_ID = %d"), id, AT_PING_ID);
                CFW_TcpipSocketClose(AT_PING_SOCKET);
                AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLC);
                break;
#endif
            }
            UINT32 ticks = hal_TimGetUpTime();
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("Old Time = %d"), CosEv->nParam1);
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("New Time = %d"), ticks);

            ticks -= CosEv->nParam1;
            UINT32 mseconds = ticks * 1000 / 16384;
#ifdef LTE_NBIOT_SUPPORT			
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("Reply from %s: bytes= %d time = %d(ms), TTL = %d"), pcIpAddr,AT_PING_PACKET_LEN+((IP_IS_V4(&AT_PING_IP))?20:40), mseconds, AT_PING_TTL);
            AT_Sprintf(Response, "Reply from %s: bytes= %d time = %d(ms), TTL = %d", pcIpAddr,AT_PING_PACKET_LEN+((IP_IS_V4(&AT_PING_IP))?20:40), mseconds, AT_PING_TTL);
#else
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("Reply from %s: bytes= %d time = %d(ms), TTL = %d"), pcIpAddr,32, mseconds, AT_PING_TTL);
            AT_Sprintf(Response, "Reply from %s: bytes= %d time = %d(ms), TTL = %d", pcIpAddr,32, mseconds, AT_PING_TTL);
#endif
            AT_PING_ECHO_NUM++;
            AT_TCPIP_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, Response, strlen(Response), nDLC);
            ping_continue(nDLC,Response);
            break;
        }
        break;

        default:
            AT_TC(g_sw_TCPIP, "AT_NET_AsyncEventProcess NOT PROCESS THIS EVENT:%d",CosEv->nEventId);
            break;
    }
    AT_FREE(CosEv);
}
static void dnsReq_callback(COS_EVENT *ev) {
    ip_addr_t *ipaddr;
    INT8 *pcIpAddr = NULL;
    UINT8 nDLCI=(UINT8)ev->nParam3;
    
    if (ev->nEventId == EV_CFW_DNS_RESOLV_SUC_IND) {
        ipaddr = (ip_addr_t*)ev->nParam1;
        pcIpAddr = ipaddr_ntoa(ipaddr);
        CSW_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("IP address is %s"),pcIpAddr);
        ip_addr_copy(AT_PING_IP,*ipaddr);
        AT_PING_ID++;
        if (AT_PING_SOCKET == INVALID_SOCKET) {
            SOCKET socketfd = INVALID_SOCKET;
#if LWIP_IPV6
            if(IP_IS_V4(&AT_PING_IP) || ip6_addr_isipv4mappedipv6(ip_2_ip6(&AT_PING_IP))) {
              socketfd = CFW_TcpipSocket(AF_INET6, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
            } else {
              socketfd = CFW_TcpipSocket(AF_INET6, CFW_TCPIP_SOCK_RAW, IPPROTO_ICMPV6);
            }
#else
            socketfd = CFW_TcpipSocket(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
#endif
            if(INVALID_SOCKET == socketfd)
            {
                AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLCI);
                AT_FREE(ev);
                return;
            }
            AT_TC(g_sw_TCPIP, "socketfd = %d, AT_PING_SOCKET =%d", socketfd, AT_PING_SOCKET);            
            AT_PING_SOCKET = socketfd;
            CFW_TcpipSocketSetParam(socketfd,AT_PING_AsyncEventProcess,nDLCI);
        }
        UINT8 nRetValue = AT_SendIP_ICMP_Ping(AT_PING_SOCKET,nDLCI);
        if(nRetValue != ERR_SUCCESS)
        {
            CSW_TRACE(CFW_TCPIP_TS_ID_SXS, "ERROR: AT_SendIP_ICMP_Ping Return Failed!",pcIpAddr);
            AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLCI);
        }
		else
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL,0, nDLCI);
    } else if (ev->nEventId == EV_CFW_DNS_RESOLV_ERR_IND) {
        CFW_TcpipSocketClose(AT_PING_SOCKET);
        AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLCI);
    }
    AT_FREE(ev);
}

#ifdef LTE_NBIOT_SUPPORT
VOID AT_TCPIP_CmdFunc_PINGSTOP(AT_CMD_PARA *pParam)
{
    UINT8 response[100] = {0};
	AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(pParam->nDLCI);
    if (NULL == pParam)
    {
        AT_TC(g_sw_TCPIP, "AT+SOCKET: pParam = NULL");
		AT_CMD_RETURN(at_CmdRespCmeError(engine, ERR_AT_CME_PARAM_INVALID));
        return;
    }
    if (AT_CMD_EXE == pParam->iType)
    {
        if(!ping_socket_is_invalid())
        {
             COS_StopCallbackTimer(CSW_AT_TASK_HANDLE,ping_timeout,pParam->nDLCI);
             ping_result_statics(pParam->nDLCI,response);
        }
        at_CmdRespOK(engine);
    }
}
#endif

/*****************************************************************************
* Name:         AT_TCPIP_CmdFunc_PING
* Description:
* Parameter:  AT_CMD_PARA *pParam
* Return:       VOID
* Remark:      n/a
* Author:
* Data:          2016-3-28
******************************************************************************/
VOID AT_TCPIP_CmdFunc_PING(AT_CMD_PARA *pParam)
{
    INT32 iResult                       = 0;
    UINT8 nNumParamter         = 0;

    AT_TC(g_sw_TCPIP, "AT+PING starting");
    AT_PING_TTL = 255;
    AT_PING_SEQNO = 0;
	AT_PING_TOTAL_NUM = 5;
    AT_PING_CID = 0;
    AT_PING_ID = 0;
    AT_PING_PACKET_LEN = 16;
    AT_PING_ECHO_NUM = 0;
    AT_PING_TIMEOUT = 5;
#ifdef LTE_NBIOT_SUPPORT
	UINT8 iptype = 0;
	UINT16 size = 0;
	ip_addr_t IpAddr;
	AT_CMD_ENGINE_T * engine = at_CmdGetByChannel(pParam->nDLCI);
	ip_addr_set_zero(&IpAddr);
#endif

    if (NULL == pParam)
    {
        AT_TC(g_sw_TCPIP, "AT+SOCKET: pParam = NULL");
        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    //UINT8 i = 0;
    if (AT_CMD_SET == pParam->iType)
    {
        iResult = AT_Util_GetParaCount(pParam->pPara, &nNumParamter);
        if ((iResult != ERR_SUCCESS) || (nNumParamter > 4) || (nNumParamter < 1))
        {
            AT_TC(g_sw_TCPIP, " AT_TCPIP_CmdFunc_PING: iResult = %d, nNumParamter=%d\n", iResult, nNumParamter);
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

#define BUFFER_SIZE 63
        UINT16 nBuffer = BUFFER_SIZE;
        UINT8 nDNS[BUFFER_SIZE] = {0};
        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, &nDNS, &nBuffer);        
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: pParam->pPara = %s\n", pParam->pPara);
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: nDNS = %s",nDNS);
#ifdef LTE_NBIOT_SUPPORT
		if (AT_GPRS_IPv4v6AddrAnalyzer(nDNS, nBuffer, &iptype) == 1)
		{
	        if(iptype == 0)
	        {
	           inet_aton(nDNS,ip_2_ip4(&IpAddr));
			   IP_SET_TYPE(&IpAddr, IPADDR_TYPE_V4);
	        }
	        else
	        {
	           inet6_aton(nDNS, ip_2_ip6(&IpAddr));
	           IP_SET_TYPE(&IpAddr, IPADDR_TYPE_V6);
	        }
	        memcpy(&AT_PING_IP,&IpAddr,sizeof(ip_addr_t));
	    }
		else if (AT_GPRS_IPv4v6AddrAnalyzer(nDNS, nBuffer, &iptype) == 2)
#endif
		{
	        UINT32 nReturnValue = CFW_GethostbynameEX(nDNS, &AT_PING_IP, AT_PING_CID, nSim,dnsReq_callback,pParam->nDLCI);
	        if(nReturnValue == RESOLV_QUERY_QUEUED)
	        {
	            AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
	            return;
	        }
	        else if(nReturnValue == RESOLV_COMPLETE)
	        {
	            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: The IP Address = ");
	            ip_addr_debug_print(LWIP_DBG_ON,&AT_PING_IP);
	        }
	        else
	        {
	            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: CFW_Gethostbyname ERROR(%x)",nReturnValue);
	            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
	            return;

	        }
		}
#ifdef LTE_NBIOT_SUPPORT
        else
		{
		    AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: the first param error");
	        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
	        return;
		}
		if(nNumParamter >=2)
	    {
	        UINT8 ping_timeout = 0;
	        size = SIZEOF(ping_timeout);
	        iResult = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &ping_timeout, &size);     
	        if ((iResult != ERR_SUCCESS) && (iResult != ERR_AT_UTIL_CMD_PARA_NULL))
            {
                AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: pParam->pPara = %s\n", pParam->pPara);
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
	        if(iResult != ERR_AT_UTIL_CMD_PARA_NULL)
	        {
	            AT_PING_TIMEOUT = ping_timeout;
	        }
	    }
	    if(nNumParamter >=3)
	    {
	        UINT16 ping_len = 0;
	        size = SIZEOF(ping_len);
	        iResult = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT16, &ping_len, &size);         
	        if ((iResult != ERR_SUCCESS) && (iResult != ERR_AT_UTIL_CMD_PARA_NULL))
            {
                AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: pParam->pPara = %s\n", pParam->pPara);
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
	        if(iResult != ERR_AT_UTIL_CMD_PARA_NULL)
	        {
	            AT_PING_PACKET_LEN = ping_len;
	            if(iptype == 0)
 	            {
 	                if(AT_PING_PACKET_LEN < 36)
	                {
	                    AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: pParam->pPara = %s\n", pParam->pPara);
                        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
	                }
	                AT_PING_PACKET_LEN -= 20;
	            }
	            else 
	            {
	                if(AT_PING_PACKET_LEN < 56)
	                {
	                    AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: pParam->pPara = %s\n", pParam->pPara);
                        AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        return;
	                }
	                AT_PING_PACKET_LEN  -=40;
	            }
	        }
	    }
	    if(nNumParamter >=4)
	    {
	        UINT16 ping_num = 0;
	        size = SIZEOF(ping_num);
	        iResult = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT16, &ping_num, &size);    
	        if ((iResult != ERR_SUCCESS) && (iResult != ERR_AT_UTIL_CMD_PARA_NULL))
            {
                AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: pParam->pPara = %s\n", pParam->pPara);
                AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                return;
            }
	        if(iResult != ERR_AT_UTIL_CMD_PARA_NULL)
	        {
	            AT_PING_TOTAL_NUM = ping_num;
	        }
	    }
#endif
        SOCKET socketfd = INVALID_SOCKET;
#if LWIP_IPV6
        if(IP_IS_V4(&AT_PING_IP) || ip6_addr_isipv4mappedipv6(ip_2_ip6(&AT_PING_IP))) {
          socketfd = CFW_TcpipSocket(AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
        } else {
          socketfd = CFW_TcpipSocket(AF_INET6, CFW_TCPIP_SOCK_RAW, IPPROTO_ICMPV6);
        }
#else
        socketfd = CFW_TcpipSocket(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_RAW, CFW_TCPIP_IPPROTO_ICMP);
#endif
        if(INVALID_SOCKET == socketfd)
        {
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }
        AT_PING_SOCKET = socketfd;
        AT_TC(g_sw_TCPIP, "socketfd = %d, AT_PING_SOCKET =%d", socketfd, AT_PING_SOCKET);
        CFW_TcpipSocketSetParam(socketfd,AT_PING_AsyncEventProcess,pParam->nDLCI);
        //Get TTL
        INT32 len = 1;
        if(ERR_SUCCESS != CFW_TcpipSocketGetsockopt(socketfd,CFW_TCPIP_IPPROTO_IP, IP_TTL, &AT_PING_TTL, &len))
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: Using DEFAULT TTL");
            AT_PING_TTL = 255;
        }
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: AT_PING_TTL = %d", AT_PING_TTL);
#ifndef LTE_NBIOT_SUPPORT
        AT_PING_ID++;
#endif
        if(ERR_SUCCESS != AT_SendIP_ICMP_Ping(AT_PING_SOCKET, pParam->nDLCI))
        {
            AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: Cannot find IP address of this modem!", nDNS);
            AT_TCPIP_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            return;
        }

        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: OK");
#ifdef LTE_NBIOT_SUPPORT
        at_CmdRespOK(engine);
#else
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
#endif
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        UINT8* pTestRsp = "+PING: DNS\/IP address";
        AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0,  pTestRsp, AT_StrLen(pTestRsp), pParam->nDLCI);
        return;
    }
    else
    {
        AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: This type does not supported!");
        AT_TCPIP_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
        return;
    }

    return;
}

UINT8 AT_SendIP_ICMP_Ping(SOCKET socketfd, UINT8 nDLCI)
{
    struct sockaddr_storage to;
#ifdef LTE_NBIOT_SUPPORT
    UINT8 ipData[1500] = {0};
#else
    UINT8 ipData[16] = {0};
#endif
    struct icmp_echo_hdr *idur;
    idur = ipData;

    ICMPH_CODE_SET(idur, 0);
    idur->id = htons(++AT_PING_ID);
    idur->seqno = htons(++AT_PING_SEQNO);
    // in 16384Hz ticks.
    *((UINT32*)(ipData+8))=hal_TimGetUpTime();
    *((UINT32*)(ipData+12))= socketfd;
#ifdef LTE_NBIOT_SUPPORT
    for(UINT16 i=0; i < AT_PING_PACKET_LEN-16; i++)
    {
	    ipData[i+16] = i;
    }
#endif

    idur->chksum = 0;
    AT_TC(g_sw_TCPIP, "AT_TCPIP_CmdFunc_PING: Start time = %d, socketid = %d, AT_PING_SEQNO = %d", *((UINT32*)(ipData+8)), *((UINT32*)(ipData+12)), AT_PING_SEQNO);
    
#if LWIP_IPV4
      if(IP_IS_V4(&AT_PING_IP)) {
        struct sockaddr_in *to4 = (struct sockaddr_in*)&to;
        to4->sin_len    = sizeof(to4);
        to4->sin_family = AF_INET;
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&AT_PING_IP));
        ICMPH_TYPE_SET(idur, ICMP_ECHO);
#ifdef LTE_NBIOT_SUPPORT
		idur->chksum = inet_chksum(idur, AT_PING_PACKET_LEN);
#else
        idur->chksum = inet_chksum(idur, 16);
#endif
      }
#endif /* LWIP_IPV4 */
    
#if LWIP_IPV6
      if(IP_IS_V6(&AT_PING_IP) && !ip6_addr_isipv4mappedipv6(ip_2_ip6(&AT_PING_IP))) {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6*)&to;
        to6->sin6_len    = sizeof(to6);
        to6->sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&AT_PING_IP));
        ICMPH_TYPE_SET(idur, ICMP6_TYPE_EREQ);
      }
#endif /* LWIP_IPV6 */

#ifdef LTE_NBIOT_SUPPORT
	if(-1 == CFW_TcpipSocketSendto(socketfd, ipData, AT_PING_PACKET_LEN, 0, &to, sizeof(to)))
#else
    if(-1 == CFW_TcpipSocketSendto(socketfd, ipData, 16, 0, &to, sizeof(to)))
#endif
    {
        CFW_TcpipSocketClose(AT_PING_SOCKET);
		AT_PING_SOCKET = INVALID_SOCKET;
        return -1;
    }
#ifdef LTE_NBIOT_SUPPORT
	COS_StartCallbackTimer(CSW_AT_TASK_HANDLE,AT_PING_TIMEOUT * 1000,ping_timeout,nDLCI);
#else
    COS_StartCallbackTimer(CSW_AT_TASK_HANDLE,5 * 1000,ping_timeout,nDLCI);
#endif
    return ERR_SUCCESS;
}

BOOL ping_socket_is_invalid()
{
    return (AT_PING_SOCKET==INVALID_SOCKET)?TRUE:FALSE;
}

void ping_result_statics(UINT8 nDLC, UINT8 *Response)
{
    INT8 *pcIpAddr = ipaddr_ntoa(&AT_PING_IP);
    CFW_TcpipSocketClose(AT_PING_SOCKET);
	AT_PING_SOCKET = INVALID_SOCKET;
    AT_Sprintf(Response, "\n\rPing statistics for %s", pcIpAddr);
    AT_TCPIP_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, Response, strlen(Response), nDLC);
    AT_Sprintf(Response, "Packets: Sent = %d, Received = %d, Lose = %d <%d\%>", AT_PING_TOTAL_NUM, AT_PING_ECHO_NUM, \
               AT_PING_TOTAL_NUM -AT_PING_ECHO_NUM,  AT_PING_ECHO_NUM * 100/AT_PING_TOTAL_NUM);
#ifndef LTE_NBIOT_SUPPORT
    AT_TCPIP_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, Response, strlen(Response), nDLC);
#else
    AT_TCPIP_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, Response, strlen(Response), nDLC);
#endif
    AT_PING_SEQNO = 0;
    AT_PING_ECHO_NUM = 0;
}

VOID ping_continue(UINT8 nDLC, UINT8 *Response){
    if(AT_PING_SEQNO < AT_PING_TOTAL_NUM)
    {
        UINT8 nRetValue = AT_SendIP_ICMP_Ping(AT_PING_SOCKET,nDLC);
        if(nRetValue != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "ERROR: AT_SendIP_ICMP_Ping Return Failed!");
            AT_TCPIP_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, nDLC);
        }
#ifndef LTE_NBIOT_SUPPORT
        else
            AT_TCPIP_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, nDLC);
#endif
    }
    else
    {
        ping_result_statics(nDLC,Response);
    }
}

VOID ping_timeout(UINT32 *param) {
    UINT8 nDLC = (UINT8)param;
    UINT8 Response[100]= {0};
    AT_Sprintf(Response, "Request timed out.\r\n");
    AT_TCPIP_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, Response, strlen(Response), nDLC);
    ping_continue(nDLC,Response);
}
#endif

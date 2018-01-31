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
#ifdef AT_NETIF_SUPPORT
#include "stdio.h"
#include "cfw.h"
#include "at.h"
#include "at_sa.h"
#include "at_module.h"
#include "at_cfg.h"
#include "itf_api.h"
#include "sockets.h"


VOID AT_NET_CmdFunc_NetInfo(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 outString[50] = {0x00,};
    if (NULL == pParam)
    {
           AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
           return;
    }
    switch (pParam->iType)
    {
        case AT_CMD_READ:
            {
                struct netif *netinfo = NULL;

                if(netif_default !=NULL){
                    AT_Sprintf(outString, "default netif: %c%c%d\r\n", netif_default->name[0],netif_default->name[1], netif_default->num);
                    at_CmdRespInfoText(pParam->engine,outString);
                }else{
                    AT_Sprintf(outString, "default netif notset\r\n");
                    at_CmdRespInfoText(pParam->engine,outString);
                }
                NETIF_FOREACH(netinfo){
                    int cid = netinfo->sim_cid & 0x0f;
                    int sim = (netinfo->sim_cid & 0xf0) >> 4;
                    if(strstr(netinfo->name,"GP")!= NULL) {
                        AT_Sprintf(outString, "%c%c%d: sim:%d - cid:%d", netinfo->name[0],netinfo->name[1], netinfo->num, sim, cid);
                        at_CmdRespInfoText(pParam->engine,outString);
                    }else {
                        AT_Sprintf(outString, "%c%c%d:", netinfo->name[0],netinfo->name[1], netinfo->num);
                        at_CmdRespInfoText(pParam->engine,outString);
                    }
                    AT_Sprintf(outString, "  inet4 addr: %s", ipaddr_ntoa(&(netinfo->ip_addr)));
                    at_CmdRespInfoText(pParam->engine,outString);
#if LWIP_IPV6
                    AT_Sprintf(outString, "  inet6 addr0: %s", ipaddr_ntoa(&(netinfo->ip6_addr[0])));
                    at_CmdRespInfoText(pParam->engine,outString);                    
                    AT_Sprintf(outString, "  inet6 addr1: %s", ipaddr_ntoa(&(netinfo->ip6_addr[1])));
                    at_CmdRespInfoText(pParam->engine,outString);
#endif
                    AT_Sprintf(outString, "  gw addr: %s", ipaddr_ntoa(&(netinfo->gw)));
                    at_CmdRespInfoText(pParam->engine,outString);
                    AT_Sprintf(outString, "  mask addr %s", ipaddr_ntoa(&(netinfo->netmask)));
                    at_CmdRespInfoText(pParam->engine,outString);
                    ip_addr_t *dns1,*dns2;
                    if (netif_default == netinfo) {
                        dns1 = dns_getserver(0);
                        dns2 = dns_getserver(1);
                    } else if (cid >0 && cid <8 && sim < CFW_SIM_COUNT) {
                        UINT32* server = getDNSServer(cid, sim);
                        ip_addr_t dns_server = IPADDR4_INIT(server[0]);
                        ip_addr_t dns_server2 = IPADDR4_INIT(server[1]);
                        dns1 = &dns_server;
                        dns2 = &dns_server2;
                    } else if (netinfo->sim_cid == 0xf0|0x11) {
#ifdef WIFI_SUPPORT
                        dns1 = wifi_dns_getserver(0);
                        dns2 = wifi_dns_getserver(1);
#endif
                    }
                    if(dns1 != NULL) {
                        AT_Sprintf(outString, "  dns addr %s", ipaddr_ntoa(dns1));
                        at_CmdRespInfoText(pParam->engine,outString);
                    }
                    if(dns2 != NULL) {
                        AT_Sprintf(outString, "  dns2 addr %s", ipaddr_ntoa(dns2));
                        at_CmdRespInfoText(pParam->engine,outString);
                    }
                }
                AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                break;
            }
        case AT_CMD_SET:
            {
                UINT8 uParamCount  = 0;
                INT32 iRet = ERR_SUCCESS;
                UINT8 ifname[6];                
                UINT16 uSize = 6;
                iRet = AT_Util_GetParaCount(pParam->pPara, &uParamCount);
                if ((iRet != ERR_SUCCESS) || (uParamCount != 1))
                {
                    AT_TC(g_sw_SPEC, 
                          "AT_NET_CmdFunc_NetInfo, parameters error or parameter number not satisfied\n");
                    AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                }
                iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, ifname, &uSize);
                AT_TC(g_sw_AT_NW, "AT_NET_CmdFunc_NetInfo... ... ifname= %s", ifname);
                if (iRet != ERR_SUCCESS)
                {
                    AT_TC(g_sw_SPEC,  "AT_NET_CmdFunc_NetInfo, get parameter error\n");
                    AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    return;
                } else {
                    struct netif * netif = netif_find(ifname);
                    if (netif != NULL) {
                        if (netif_default != netif) {
                            netif_set_default(netif);
                            int cid = netif->sim_cid & 0x0f;
                            int sim = (netif->sim_cid & 0xf0) >> 4;
                            if (cid >0 && cid <8 && sim < CFW_SIM_COUNT) {
                                UINT32* server = getDNSServer(cid, sim);
                                ip_addr_t dns_server = IPADDR4_INIT(server[0]);
                                ip_addr_t dns_server2 = IPADDR4_INIT(server[1]);
                                dns_setserver(0,&dns_server);
                                dns_setserver(1,&dns_server2);
                            } else if (netif->sim_cid == 0xf0|0x11) {
#ifdef WIFI_SUPPORT
                                dns_setserver(0,wifi_dns_getserver(0));
                                dns_setserver(1,wifi_dns_getserver(1));
#endif
                            }
                        }
                        AT_GPRS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                    } else {
                        AT_TC(g_sw_GPRS, "NetInfo set ERROR");
                        AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    }
                }
            }
            break;
        default:
               AT_TC(g_sw_GPRS, "NetInfo ERROR");
               AT_GPRS_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
               break;
    }
}

#endif


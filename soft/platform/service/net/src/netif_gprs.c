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

#ifdef CFW_TCPIP_SUPPORT
#include <csw.h>
#include "sxs_io.h"
#include "string.h"
#include "sockets.h"
UINT8 isPppActived = 0;
#ifdef LTE_NBIOT_SUPPORT
UINT8  nbiot_netif_mode = 0;
#endif
static err_t data_output(struct netif *netif, struct pbuf *p,
                  ip_addr_t *ipaddr) {
    UINT16  offset = 0;
    struct pbuf *q;
    CFW_GPRS_DATA *pGprsData = NULL;
    sys_arch_printf("data_output ---------------------------------");
    pGprsData = CSW_TCPIP_MALLOC(sizeof(CFW_GPRS_DATA) + p ->tot_len);
    if(pGprsData == NULL) {
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("data_output : CSW_TCPIP_MALLOC == 0    !!!\n",0x08101161)));
        return !ERR_OK;
    } else {
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0) , TSTXT(TSTR("data_output : CSW_TCPIP_MALLOC  success   !!!\n",0x08101162)));
    }
    pGprsData->nDataLength = p ->tot_len;
    for (q = p; q != NULL; q = q->next) {
        SUL_MemCopy8 (&pGprsData->pData[offset], q->payload, q->len);
        offset += q->len ;
    }
    SXS_DUMP(_SXR | TLEVEL(9), 0, pGprsData->pData, pGprsData->nDataLength);
#ifdef LTE_NBIOT_SUPPORT
    extern UINT8 nas_rai;
    CFW_GprsSendData(netif->sim_cid & 0x0f,  pGprsData, nas_rai, 0, (netif->sim_cid & 0xf0) >> 4);
#else
    CFW_GprsSendData(netif->sim_cid & 0x0f,  pGprsData, (netif->sim_cid & 0xf0) >> 4);
#endif
    CSW_TCPIP_FREE(pGprsData);
    return ERR_OK;

}

static err_t netif_gprs_init(struct netif *netif) {
  /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made!
   */
  //MIB2_INIT_NETIF(netif, snmp_ifType_softwareLoopback, 0);

  netif->name[0] = 'G';
  netif->name[1] = 'P';
#if LWIP_IPV4
  netif->output = data_output;
#endif
#if LWIP_IPV6
  netif->output_ip6 = data_output;
#endif
  if(NULL == netif_default) {
      int nCid = netif->sim_cid & 0x0f;
      int nSimId = (netif->sim_cid & 0xf0) >> 4;
      netif_set_default(netif);
      UINT32* server = getDNSServer(nCid,nSimId);
      ip_addr_t dns_server0 = IPADDR4_INIT(server[0]);
      ip_addr_t dns_server1 = IPADDR4_INIT(server[1]);
      dns_setserver(0,&dns_server0);
      dns_setserver(1,&dns_server1);
  }
  return ERR_OK;
}

BOOL BAL_ApsTaskTcpipProc (COS_EVENT *pEvent)
{
    SXS_TRACE(0, "BAL_ApsTaskTcpipProc pEvent->nEventId :%d,isPppActived=%d\n", pEvent->nEventId,isPppActived);
    if(pEvent->nEventId == EV_CFW_GPRS_DATA_IND)
    {
        struct pbuf     *p, *q;
        UINT16          len;
        UINT16          offset;
        CFW_GPRS_DATA *Msg;
        UINT8 nCid,nSimId,T_cid;
        
        struct netif *ppp_netif = NULL;
        struct netif *inp_netif;
        Msg = (CFW_GPRS_DATA *)pEvent->nParam2;
        nCid = pEvent->nParam1;
        nSimId = LOUINT8(pEvent->nParam3);
        T_cid = nSimId<<4 | nCid;
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("EV_CFW_GPRS_DATA_IND, data len = %d \n",0x081000f6)),  Msg->nDataLength );    
#ifdef LTE_NBIOT_SUPPORT
		if((nbiot_netif_mode & (1 << nCid)) != 0)  //non-ip data
		{
		    pEvent->nEventId = EV_CFW_GPRS_NONIPDATA_IND; //
            return TRUE;
		}
		else if((nbiot_netif_mode & 0x80) != 0)  //direct ip mode
		{
		    extern UINT8  crtdcp_reporting;
		    if(crtdcp_reporting == 1)
		    {
		        pEvent->nEventId = EV_CFW_GPRS_CRTDCP_IND; //
		        return TRUE;
		    }
			else
			{
			    CSW_TCPIP_FREE(Msg);
				return FALSE;
			}
		}
#endif
#ifdef LWIP_PPP_ON
        ppp_netif = getPppNetIf(HIUINT16(pEvent->nParam3));
#endif
        inp_netif = getGprsNetIf(nSimId,nCid);
        len = Msg->nDataLength;
        SXS_DUMP(_SXR | TLEVEL(9), 0, Msg->pData, len);
        if (inp_netif != NULL || ppp_netif != NULL) {
            p = (struct pbuf *)pbuf_alloc(PBUF_RAW, len, PBUF_POOL);    //PBUF_RAW 3 // PBUF_POOL 3
            offset = 0;
            if(p != NULL) {
                if(len > p->len) {
                    for (q = p; len > q->len; q = q->next) {
                        SUL_MemCopy8 (q->payload, &(Msg->pData[offset]), q->len);
                        len -= q->len;
                        offset += q->len;
                    }
                    if(len != 0) {
                        SUL_MemCopy8 (q->payload, &(Msg->pData[offset]), len);
                    }
                } else {
                    SUL_MemCopy8 (p->payload, &(Msg->pData[offset]), len);
                }
                if (ppp_netif !=NULL) {
                    ppp_netif->output(ppp_netif,p,NULL);
                    pbuf_free(p);
                }
                else
                    inp_netif->input(p, inp_netif);
            } else {
                CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("****Receive Data request memory :out of place\n",0x081000f8)));
            }
        } else {
           sys_arch_printf("Error ########### EV_CFW_GPRS_DATA_IND can't find netif for CID=0x%x\n",T_cid);
        }
        CSW_TCPIP_FREE(Msg);
        return 0;
    }
    else if(EV_CFW_GPRS_CTRL_RELEASE_IND == pEvent->nEventId)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("EV_CFW_GPRS_CTRL_RELEASE_IND",0x08100094)));
        do_send_stored_packet(pEvent->nParam1, pEvent->nParam2);
    }
    else if(pEvent->nEventId == EV_CFW_GPRS_ACT_RSP)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("BAL_ApsTaskTcpipProc recive EV_CFW_GPRS_ACT_RSP \n",0x081000fb)) );
        struct netif *netif;
        UINT8 nCid,nSimId,T_cid;
        nCid = pEvent->nParam1;
        nSimId = LOUINT8(pEvent->nParam3);
        T_cid = nSimId<<4 | nCid;
#ifdef LTE_NBIOT_SUPPORT
		if(HIUINT8(pEvent->nParam3) == CFW_GPRS_ACTIVED) {
			if(nbiot_nvGetDirectIpMode() == 1)
			{
			    nbiot_netif_mode |= 0x80;
			    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("APS : nbiot using DirectIp Mode %x\n",0x081000fc)));
			}
			if(pEvent->nParam2 == CFW_GPRS_PDP_TYPE_NONIP)
			{
			    nbiot_netif_mode |= (1 << nCid);
			    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("APS : nbiot using NonIP Mode %x\n",0x081000fc)));
			}
			if(nbiot_netif_mode != 0x00)
			{
			    return TRUE;
			}
			UINT8 ipv4[4];
			UINT8 if_id[8];
			ip4_addr_t ip={0};
		    ip4_addr_set_any(&ip);
			UINT8 lenth;
			CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("APS : active success cid = 0x%x\n",0x081000fc)), T_cid);

			netif = CSW_TCPIP_MALLOC(SIZEOF(struct netif));
			memset(netif, 0, SIZEOF(struct netif));
			netif->sim_cid = T_cid;
			if((pEvent->nParam2 == CFW_GPRS_PDP_TYPE_IP) ||(pEvent->nParam2 == CFW_GPRS_PDP_TYPE_IPV4V6))
			{
				CFW_GprsGetPdpIpv4Addr(pEvent->nParam1, &lenth, ipv4, nSimId);
				IP4_ADDR(&ip, ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
			}
			netif_add(netif, &ip, NULL, NULL, NULL, netif_gprs_init, tcpip_input);
			if((pEvent->nParam2 == CFW_GPRS_PDP_TYPE_IPV6) ||(pEvent->nParam2 == CFW_GPRS_PDP_TYPE_IPV4V6))
			{
				CFW_GprsGetPdpIpv6IfId(pEvent->nParam1, &lenth, if_id, nSimId);
			    netif_create_ip6_linklocal_address_from_if_id(netif, if_id);
			}
			netif_set_up(netif);
			netif_set_link_up(netif);
			CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("APS, netif->num: 0x%x\n",0x081000fe)), netif->num);
					
		}

#else
        if(HIUINT8(pEvent->nParam3) == CFW_GPRS_ACTIVED) {
            UINT8 addr[4];
            UINT32 Ip_Addr;
            ip4_addr_t ip={0};
            UINT8 lenth;
            CFW_GprsGetPdpAddr(nCid, &lenth, addr, nSimId);
            Ip_Addr = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | addr[0] ;

            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("APS : active success cid = 0x%x\n",0x081000fc)), T_cid);
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(4), TSTXT(TSTR("IP: %d.%d.%d.%d",0x081000fd)),
                      (UINT16)(ntohl(Ip_Addr) >> 24) & 0xff,
                      (UINT16)(ntohl(Ip_Addr) >> 16) & 0xff,
                      (UINT16)(ntohl(Ip_Addr) >> 8) & 0xff,
                      (UINT16) ntohl(Ip_Addr) & 0xff);

            netif = CSW_TCPIP_MALLOC(SIZEOF(struct netif));
            memset(netif, 0, SIZEOF(struct netif));
            netif->sim_cid = T_cid;
            ip.addr = Ip_Addr;
            netif_add(netif, &ip, NULL, NULL, NULL, netif_gprs_init, tcpip_input);
            netif_set_up(netif);
            netif_set_link_up(netif);
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("APS, netif->num: 0x%x\n",0x081000fe)), netif->num);
        }
#endif
        else if(HIUINT8(pEvent->nParam3) == CFW_GPRS_DEACTIVED)
        {
#ifdef LTE_NBIOT_SUPPORT
            nbiot_netif_mode &= 0x7F;
            if((nbiot_netif_mode & (1 << nCid)) != 0) //clear non ip flag
            {
                nbiot_netif_mode &= ~(1 << nCid);
            }
			else
#endif
            {
            netif = getGprsNetIf(nSimId,nCid);
            if (netif != NULL) {
                netif_set_link_down(netif);
                netif_remove(netif);
                CSW_TCPIP_FREE(netif);
                tcp_debug_print_pcbs();                    
            } else {
                sys_arch_printf("Error ########### CFW_GPRS_DEACTIVED can't find netif for CID=0x%x\n",T_cid);
                }
            }
        }
    }
    else if(pEvent->nEventId == EV_CFW_GPRS_ATT_RSP)
    {
        sys_arch_printf("BAL_ApsTaskTcpipProc recive EV_CFW_GPRS_ATT_RSP \n");
        if(HIUINT8(pEvent->nParam3) == CFW_GPRS_DEACTIVED)
        {
            tcp_debug_print_pcbs();
        }
    }
    else if(pEvent->nEventId == EV_CFW_GPRS_CXT_DEACTIVE_IND)
    {
        struct netif *netif;
        UINT8 nCid,nSimId,T_cid;
        nCid = pEvent->nParam1;
        nSimId = LOUINT8(pEvent->nParam3);
        T_cid = nSimId<<4 | nCid;
        SXS_TRACE(CFW_TCPIP_TS_ID_SXS, TSTXT("APS : EV_CFW_GPRS_CXT_DEACTIVE_IND cid = 0x%x ,cause: 0x%x\n"), T_cid, pEvent->nParam2);
#ifdef LTE_NBIOT_SUPPORT
		nbiot_netif_mode &= 0x7F;
        if((nbiot_netif_mode & (1 << nCid)) != 0)  //clear non ip flag
        {
            nbiot_netif_mode &= ~(1 << nCid);
        }
        else
#endif
        {
        netif = getGprsNetIf(nSimId,nCid);
        if (netif != NULL) {
            netif_set_link_down(netif);
            netif_remove(netif);
            CSW_TCPIP_FREE(netif);
            tcp_debug_print_pcbs();                    
        } else {
            sys_arch_printf("Error ########### EV_CFW_GPRS_CXT_DEACTIVE_IND can't find netif for CID=0x%x\n",T_cid);
        }
    }
    }
    return TRUE;
}
#endif



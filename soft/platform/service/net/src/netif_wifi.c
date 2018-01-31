#ifdef WIFI_SUPPORT
#include "sockets.h"

extern HANDLE wifi_task_handle;
#define CFW_GPRS_TRACE_DATA  (_CSW| TLEVEL(9))

void showString(UINT8 *str)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("%s \n",0x08100096), str);
}

UINT32 wifi_receiveDataEx( UINT8 *Pdata,  UINT32 flag, UINT16 nlength)
{
    struct pbuf     *p, *q;

    UINT16          len = nlength;
    UINT16          offset = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("EV_CFW_WIFI_DATA_IND, data len = %d \n",0x081000c4)),  nlength );

    p = (struct pbuf *)pbuf_alloc(PBUF_RAW, len, PBUF_POOL);    //PBUF_RAW 3 // PBUF_POOL 3

    if(p != NULL)
    {
        if(len > p->len)
        {
            for (q = p; len > q->len; q = q->next)
            {
                SUL_MemCopy8 (q->payload, &(Pdata[offset]), q->len);
                len -= q->len;
                offset += q->len;
            }
            if(len != 0)
            {
                SUL_MemCopy8 (q->payload, &(Pdata[offset]), len);
            }
        }
        else
        {
            SUL_MemCopy8 (p->payload, &(Pdata[offset]), len);
        }
        return (UINT32)p ;
    }
    else
    {
        return NULL;
    }
}

VOID Aps_WifiRcvPck(UINT8 *destMac, UINT8 *srcMac, UINT8 *pdata, UINT16 len, UINT32 frameType)
{

    CFW_WIFI_DATA *ppData = NULL;
    UINT16 nDataLen = 0;
    nDataLen = len;
    //ppData = (CFW_WIFI_DATA *)CSW_GPRS_MALLOC(nDataLen + OFFSETOF(CFW_WIFI_DATA, pData));
    ppData = (CFW_WIFI_DATA *)CSW_GPRS_MALLOC(sizeof(CFW_WIFI_DATA));

    if(ppData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x08100098)));
        //COS_Sleep(20);//remove by wuys for gprs test 2008-9-03
        return ERR_BUSY_DEVICE;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("ppData: 0x%x,  nDataLen: %d\n",0x08100099)), ppData, nDataLen);

    ppData->nDataLength = nDataLen;
    SUL_MemCopy8(ppData->srcMac, srcMac, 6);
    SUL_MemCopy8(ppData->destMac, destMac, 6);
    ppData->pData = wifi_receiveDataEx(pdata, frameType, len);
    if(ppData->pData == NULL)
    {
        CSW_TCPIP_FREE(ppData);
        return;
    }
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_DATA_IND;
    EV.nParam1 = ppData;
    EV.nParam2 = 0x11;
    EV.nParam3 = frameType;
    //  wifi_receiveData(ppData,0x11,frameType);
    //  return;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

int test_rda5890_data_rx(UINT8 *data, unsigned short len)
{
    //  CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(4),  TSTR("rda5890_data_rx: %02x %02x %02x %02x\n",0x0810009c),data[12], data[13], data[14], data[15]);
    UINT16 datatype = data[12] << 8 | data[13];
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1),TSTR("rda5890_data_rx, data type:0x%x \n",0x0810009d),datatype);
    SXS_DUMP(_SXR | TLEVEL(15), 0, data, len);
    switch(datatype)  //the type, refer to ehtertype, one field of ethernet frame, IEEE802.3
    {
        case 0x888e:
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTR("rda5890_data_rx, EAPOL frame ! \n",0x081000a2));
            wifi_inputEapolPkt(&data[6], data + 14, len - 14);
            break;
        default:
            Aps_WifiRcvPck(&data[0], &data[6], data, len, datatype);
            break;
    }
    return 0;
}
UINT8 localMac[6] = {0x00,};

BOOL auth_failed = FALSE;

BOOL connect_ind_sent = FALSE;

extern HANDLE wifi_task_handle;

VOID ip_WifiConnectedInd(void)
{
    if(connect_ind_sent)
        return;
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("in ip_WifiConnectedInd......\n",0x081000a4)));

    CFW_WIFI_NETIF *pData = NULL;
    pData = (CFW_WIFI_NETIF *)CSW_GPRS_MALLOC(sizeof(CFW_WIFI_NETIF));
    if(pData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("received CSW_GPRS_MALLOC error\n",0x081000a5)));
        //COS_Sleep(20);//remove by wuys for gprs test 2008-9-03
        return ERR_BUSY_DEVICE;
    }
    auth_failed = FALSE;
    SUL_MemCopy8(pData->Mac, localMac, 6);
    //SUL_MemCopy8(pData->gwMac, gwMac, 6);
    ip_addr_set_ip4_u32(&(pData->ipaddr),0x000000000);
    //receive_wifiConnected(pData);
    //return;
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_CONNECTED_IND;
    EV.nParam1 = pData;
    EV.nParam2 = 1;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    connect_ind_sent = TRUE;
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("in ip_WifiConnectedInd send to APS\n",0x081000a6)));
}
VOID ip_WifiDisconnectedInd(void)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("in ip_WifiDisconnectedInd......\n",0x081000a7)));

    //return;
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_CONNECTED_IND;
    EV.nParam1 = 0;
    EV.nParam2 = 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("in ip_WifiDisconnectedInd send to APS\n",0x081000a8)));
}

err_t wifi_linkoutput(struct netif *netif, struct pbuf *p)
{
    UINT16  offset = 0;
    struct pbuf *q;
    UINT8 *pGprsData = NULL;
    pGprsData = CSW_TCPIP_MALLOC(p ->tot_len);
    if(pGprsData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi_linkoutput : CSW_TCPIP_MALLOC == 0    !!!\n",0x0810115b)));
        return ERR_TCPIP_MEM;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0) , TSTXT(TSTR("wifi_linkoutput : CSW_TCPIP_MALLOC  success   !!!\n",0x0810115c)));
    
    }
    for (q = p; q != NULL; q = q->next)
    {
        SUL_MemCopy8 (&pGprsData[offset], q->payload, q->len);
        offset += q->len ;
    }
    SXS_DUMP(_SXR | TLEVEL(15), 0, pGprsData, p->tot_len);
    wifi_mac_data_txReq(pGprsData, p->tot_len, 1);
    return 0;
}

VOID stop_wifiPollTimer(VOID)
{
    //  COS_KillTimerEX(  BAL_TH(BAL_CFW_ADV_TASK_PRIORITY), SDIO_POLL_TIMER_ID);

}
VOID restart_wifiPollTimer(VOID)
{
    // COS_SetTimerEX(  BAL_TH(BAL_CFW_ADV_TASK_PRIORITY), SDIO_POLL_TIMER_ID,  COS_TIMER_MODE_SINGLE, 100*16384/1000);

}

UINT32 wifi_enableSleepTimerCount = 0;
static ip_addr_t wifi_dns_servers[DNS_MAX_SERVERS];

void
wifi_dns_setserver(u8_t numdns, const ip_addr_t *dnsserver)
{
  if (numdns < DNS_MAX_SERVERS) {
    if (dnsserver != NULL) {
      wifi_dns_servers[numdns] = (*dnsserver);
    } else {
      wifi_dns_servers[numdns] = *IP_ADDR_ANY;
    }
  }
}

const ip_addr_t*
wifi_dns_getserver(u8_t numdns)
{
  if (numdns < DNS_MAX_SERVERS) {
    return &wifi_dns_servers[numdns];
  } else {
    return IP_ADDR_ANY;
  }
}

void wifi_dump(UINT8 *data, UINT16 len)
{
    SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, data, len);

}

static err_t netif_ether_init(struct netif *netif) {
    netif->output = etharp_output;
    netif->sim_cid = 0xf0|0x11;
    netif->linkoutput = wifi_linkoutput;
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    netif->name[0] = 'W';
    netif->name[1] = 'F';
    netif->mtu = GPRS_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
    netif->ip6_autoconfig_enabled = 1;
    netif_create_ip6_linklocal_address(netif, 1);
    netif->flags |= NETIF_FLAG_MLD6;
#endif
    netif_set_default(netif);

    return ERR_OK;
}

BOOL InDhcp = FALSE;
INT  test_rda5890_set_TxRate( UINT8 val);
extern UINT32 report_rssi_count;
UINT32 wifi_IpaddrUpdateInd(struct netif *netif_tmp)
{
    test_rda5890_set_TxRate(0);
    sxr_Sleep(16384);
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTR("##########wifi_IpaddrUpdateInd \n",0x081000b2));
    ipaddr_update_ind_struct *ipaddr_update = NULL;
    ipaddr_update = CSW_Malloc(sizeof(ipaddr_update_ind_struct));

    memset(ipaddr_update, 0x00, sizeof(ipaddr_update_ind_struct));
    report_rssi_count = 23;
    // ipaddr_update->msg_len;
    memcpy(ipaddr_update->gateway, (UINT8 *)&(netif_ip4_gw(netif_tmp)->addr), 4);
    memcpy(ipaddr_update->ip_addr, (UINT8 *)&(netif_ip4_addr(netif_tmp)->addr), 4);
    memcpy(ipaddr_update->netmask, (UINT8 *)&(netif_ip4_netmask(netif_tmp)->addr), 4);
    memcpy(ipaddr_update->pri_dns_addr, (UINT8 *)ip_2_ip4(dns_getserver(0)), 4);
    InDhcp = FALSE;
    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_IPADDR_UPDATE_IND;
    ev.nParam1  = ipaddr_update;
    ev.nParam2  = 0x11;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(HIUINT16(ev.nParam3)), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return 0;
}

UINT32 wifi_IpaddrUpdateErrorInd(VOID)
{
    //test_rda5890_set_TxRate(0);
    //sxr_Sleep(16384*2);
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTR("##########wifi_IpaddrUpdateErrorInd \n",0x081000b3));
    ipaddr_update_ind_struct *ipaddr_update = NULL;
    ipaddr_update = CSW_Malloc(sizeof(ipaddr_update_ind_struct));
    memset(ipaddr_update, 0x00, sizeof(ipaddr_update_ind_struct));
    InDhcp = FALSE;
    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_IPADDR_UPDATE_IND;
    ev.nParam1  = ipaddr_update;
    ev.nParam2  = 0x11;
    ev.nParam3  = 0xFF;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(HIUINT16(ev.nParam3)), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}


VOID Wifi_ipAddrChangeReq(void *req)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTR("##########Wifi_ipAddrChangeReq \n",0x081000b4));
    //test_rda5890_set_TxRate(1);
    //sxr_Sleep(16384/2);
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_IPADDR_CHANGE_REQ;
    EV.nParam1 = req;
    EV.nParam2 = 0x11;
    EV.nParam3 = 0;
    COS_SendEvent(CFW_bal_GetTaskHandle(CSW_TASK_ID_APS), &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTR("##########Wifi_ipAddrChangeReq end \n",0x081000b5));

}

VOID receive_wifiDisConnected (VOID)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("receive_wifiDisConnected  \n",0x081000b9)) );
    struct netif *netif = getEtherNetIf(0x11);
    if(netif)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("netif_remove: 0x%x\n",0x081000ba)), netif->num);
        dhcp_stop(netif);
        netif_remove(netif);
        CSW_TCPIP_FREE(netif);
    }
}


extern UINT32 tmp_wifi_enableSleepTimerCount ;
void wifi_inputEapolPkt(const u8 *src_addr, const u8 *buf, size_t len);

BOOL BAL_ApsTaskTcpipProcForWifi (COS_EVENT *pEvent)
{
    struct netif *netif;
    ip_addr_t ip = {0};
    if(pEvent->nEventId != 93)
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("pEvent->nEventId : %d \n",0x081000c5)), pEvent->nEventId);

    if(pEvent->nEventId == EV_CFW_WIFI_DATA_IND)
    {
        struct pbuf     *p;

        UINT16          len = 0;
        UINT16          offset;
        CFW_WIFI_DATA *Msg;
        Msg = (CFW_WIFI_DATA *)pEvent->nParam1;

        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("EV_CFW_WIFI_DATA_IND, data len = %d \n",0x081000c6)),  Msg->nDataLength );
        netif = getEtherNetIf(pEvent->nParam2);
        if(netif != NULL)
        {
            netif->input(Msg->pData,netif);
        }
        else
        {
            pbuf_free(Msg->pData);
            // #ifndef USE_WIFI_SLEEP
            wifi_enableSleepTimerCount = tmp_wifi_enableSleepTimerCount;
            //#endif
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("BAL_ApsTaskTcpipProcForWifi , this frame is not for us, will be  to: \n",0x081000c9)) );
            SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, Msg->destMac, 6);
        }

        CSW_TCPIP_FREE(Msg);
    }
    else if(pEvent->nEventId == EV_CFW_WIFI_EAPOL_IND)
    {
        CFW_WIFI_DATA *Msg;
        Msg = (CFW_WIFI_DATA *)pEvent->nParam1;
        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("EV_CFW_WIFI_EAPOL_IND, data len = %d \n",0x081000cf)),  Msg->nDataLength );
        wifi_inputEapolPkt(Msg->srcMac, Msg->pData, Msg->nDataLength);
        CSW_TCPIP_FREE(Msg);
    }
    else if(pEvent->nEventId == EV_CFW_WIFI_CONNECTED_IND)
    {
        if((pEvent->nParam2 == 0x00) || (pEvent->nParam1 == 0x00))
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR(" recive EV_CFW_WIFI_DISCONNECTED_IND \n",0x081000d2)) );
            receive_wifiDisConnected();
            //  if(InDhcp)
            //  wifi_IpaddrUpdateErrorInd();
            wifi_ConnectRsp(0);
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("BAL_ApsTaskTcpipProcwifi recive EV_CFW_WIFI_CONNECTED_IND \n",0x081000d3)) );
            CFW_WIFI_NETIF *Msg;

            Msg = (CFW_WIFI_NETIF *)pEvent->nParam1;
            netif = getEtherNetIf(0x11);
            if(netif)
            {
                dhcp_stop(netif);
                netif_remove(netif);
                if(InDhcp)
                {
                    wifi_IpaddrUpdateErrorInd();
                }
                CSW_TCPIP_FREE(netif);
            }
            //add by wys 2007-02-14, there are something tobe done to
            //set the netif configure value such as "mtu",by now, these are not be done
            netif = CSW_TCPIP_MALLOC(SIZEOF(struct netif));
            memset(netif, 0, SIZEOF(struct netif));

            ip_addr_copy(ip,Msg->ipaddr);

            netif_add(netif, ip_2_ip4(&ip), NULL, NULL, NULL, netif_ether_init, tcpip_input);

            // setdefaulGwMac(Msg->gwMac);

            SUL_MemCopy8 (netif->hwaddr,  Msg->Mac, 6);
            SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, netif->hwaddr, 6);
            netif_set_up(netif);
            netif_set_link_up(netif);
            SXS_DUMP(CFW_GPRS_TRACE_DATA, 0, netif->hwaddr, 6);
            wifi_ConnectRsp(1);
            dhcp_start(netif);
            InDhcp = TRUE;
            //start_dhcp_timer();
        }

    }
    else if(pEvent->nEventId == EV_CFW_WIFI_IPADDR_CHANGE_REQ)
    {
        ipaddr_change_req_struct *ip_req = NULL;
        ip_req = (ipaddr_change_req_struct *) pEvent->nParam1;

        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("received EV_CFW_WIFI_IPADDR_CHANGE_REQ \n",0x081000db)));
        SXS_DUMP(CFW_TCPIP_TS_ID_SXS, 0, ip_req, sizeof(ipaddr_change_req_struct));
        struct netif *net_tmp = getEtherNetIf(0x11);
        if(net_tmp == NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi has not been connected! \n",0x081000dc)));

            return TRUE;
        }
        if(!ip_req->use_dhcp)
        {
            struct ip4_addr   ipaddr, netmask, gw;
            ipaddr.addr = (ip_req->ip_addr[3] << 24) | (ip_req->ip_addr[2] << 16) | (ip_req->ip_addr[1] << 8) | ip_req->ip_addr[0];
            gw.addr = (ip_req->gateway[3] << 24) | (ip_req->gateway[2] << 16) | (ip_req->gateway[1] << 8) | ip_req->gateway[0];
            netmask.addr = (ip_req->netmask[3] << 24) | (ip_req->netmask[2] << 16) | (ip_req->netmask[1] << 8) | ip_req->netmask[0];
            netif_set_addr(net_tmp,&ipaddr,&netmask,&gw);
            //wifiDNSserverIP =  (ip_req->pri_dns_addr[3] << 24) | (ip_req->pri_dns_addr[2] << 16) | (ip_req->pri_dns_addr[1] << 8) | ip_req->pri_dns_addr[0];
        }
        else if (!InDhcp)
        {
            InDhcp = TRUE;
            dhcp_start(net_tmp);
            //start_dhcp_timer();

        }
    }
    else
    {
        return FALSE;

    }
    return TRUE;
}



#endif


#include "stddef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"

#include "csw.h"
#include "cfw.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include <netdb.h>
#include "http_lunch_api.h"


struct netif *getGprsNetIf(UINT8 nSim,UINT8 nCid) {
    return netif_get_by_cid(nSim<<4|nCid);
}

struct netif *getEtherNetIf(UINT8 nCid){
    if (nCid != 0x11)
        sys_arch_printf("getEtherNetIf nCid:%d is Error\n",nCid);
    return netif_get_by_cid(0xf0|nCid);
}

UINT8 startGprsLink(UINT8 *apn,UINT8 nSIMID) {
    UINT8  nAttstate = 0xFF;
    UINT32 nRet = 0x00;
    UINT8 nCID = 0x00, nState = 0x00;
    CFW_NW_STATUS_INFO sStatus;
    UINT8 nUTI=0x00;
    nRet = CFW_GetGprsAttState(&nAttstate,nSIMID);
    sys_arch_printf("startGprsLink GprsAttState nRet=0x%x,nAttstate=%d\n",nRet,nAttstate);
    if (nAttstate != CFW_GPRS_ATTACHED) {
    CFW_GetFreeUTI(0,&nUTI);
    nRet = CFW_GprsAtt(CFW_GPRS_ATTACHED, nUTI,nSIMID);
        sys_arch_printf("startGprsLink nUTI=%d,nRet=0x%x\n",nUTI,nRet);
        while (nRet == ERR_SUCCESS && nAttstate != CFW_GPRS_ATTACHED) {
            sys_arch_printf("startGprsLink Waiting ATT ... nAttstate=%d,nRet=0x%x\n",nAttstate,nRet);
            COS_Sleep(500);
            nRet = CFW_GetGprsAttState(&nAttstate,nSIMID);
        }
    }
    if (nRet != ERR_SUCCESS)
        return -1;
    nRet = CFW_GprsGetstatus(&sStatus, nSIMID);
    while (nRet == ERR_SUCCESS && sStatus.nStatus != CFW_NW_STATUS_REGISTERED_HOME && 
        sStatus.nStatus != CFW_NW_STATUS_REGISTERED_ROAMING) {
        sys_arch_printf("startGprsLink Waiting Service ... sStatus.nStatus=%d,nRet=0x%x\n",sStatus.nStatus,nRet);
        COS_Sleep(500);
        nRet = CFW_GprsGetstatus(&sStatus, nSIMID);
    }
    if (nRet != ERR_SUCCESS)
        return -1;
    nRet = CFW_GetFreeCID(&nCID, nSIMID);
    sys_arch_printf("startGprsLink CFW_GetFreeCID nCID=%d,nRet=0x%x\n",nCID,nRet);
    CFW_GPRS_QOS qos;
    qos.nDelay = 4;
    qos.nMean = 16;
    qos.nPeak = 4;
    qos.nPrecedence = 3;
    qos.nReliability = 3;
    nRet = CFW_GprsSetReqQos(1, &qos, nSIMID);
    sys_arch_printf("startGprsLink CFW_GprsSetReqQos ret nUTI=%d,nRet=0x%x\n",nUTI,nRet);
    CFW_GPRS_PDPCONT_INFO pdp_cont;
    pdp_cont.nApnSize = 5;
    pdp_cont.pApn = apn;
    
    pdp_cont.nPdpAddrSize = 0;
    pdp_cont.pPdpAddr = NULL;
    pdp_cont.nDComp = 0;
    pdp_cont.nHComp = 0;
    pdp_cont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
    
    pdp_cont.nApnUserSize = 0;
    pdp_cont.pApnUser = NULL;
    pdp_cont.nApnPwdSize = 0;
    pdp_cont.pApnPwd= NULL;
    
    nRet =CFW_GprsSetPdpCxt(nCID, &pdp_cont, nSIMID);
    sys_arch_printf("startGprsLink CFW_GprsSetPdpCxt nCID=%d,nRet=0x%x\n",nCID,nRet);
    nRet = CFW_GetGprsActState(nCID, &nState, nSIMID);
    if (nState != CFW_GPRS_ACTIVED) {
        nRet = CFW_GprsAct(CFW_GPRS_ACTIVED, nCID, nUTI, nSIMID);
        sys_arch_printf("startGprsLink CFW_GprsAct nCID=%d,nRet=0x%x\n",nCID,nRet);
        while (nRet == ERR_SUCCESS && nState != CFW_GPRS_ACTIVED) {
            sys_arch_printf("startGprsLink Waiting Act ... nState=%d,nRet=0x%x\n",nState,nRet);
            COS_Sleep(500);
            nRet = CFW_GetGprsActState(nCID, &nState, nSIMID);
        }
    }
    if (nRet != ERR_SUCCESS)
        return -1;
    return nCID;
}

#ifdef WIFI_SUPPORT
#include "wifi_config.h"
extern UINT8 g_wifiMacTest[6];

struct netif * startWifiLink(UINT8 *ssid,UINT8 *password) {
    struct netif * netif;
    if ((netif = getEtherNetIf(0x11)) == NULL) {
        wifi_conn_t tWifiConn;        
        wifi_PowerOn(g_wifiMacTest);
        wifi_ScanAps(5, NULL);
        COS_Sleep(5000);
        tWifiConn.ssid = ssid;
        tWifiConn.ssid_len = ssid==NULL?0:strlen(ssid);
        tWifiConn.password = password;
        tWifiConn.pwd_len= password==NULL?0:strlen(password);
        wifi_ConnectApEx(&tWifiConn, 15);
        COS_Sleep(5000);
        while ((netif = getEtherNetIf(0x11)) == NULL){
            COS_Sleep(1000);
        }
    }
    while (ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            COS_Sleep(1000);
    }
    return netif;
}
#endif



void lwipTestEntry(COS_EVENT *pEvent) {
#ifdef LWIP_TEST
    switch (pEvent->nParam2) {
        case 0:
#ifdef LWIP_COAP_TEST
            start_coap_test();
            //coap_client_main(0,NULL);
#endif
            break;
        case 1:
#ifdef LWIP_MQTT_TEST
            start_test_mqtt();
#endif
            break;
        case 2:
#ifdef LWIP_HTTP_TEST
           // lunch_http_get("http://123.57.221.42/",NULL);
#endif
            break;
        case 3:
#ifdef LWIP_HTTP_TEST
           // lunch_http_get("https://123.57.221.42/",NULL);
#endif
            break;
#ifdef LWIP_LIBWM2M_TEST
        case 6:
            start_libwm2m_test();
#endif
            break;
    }
#endif
}

int gettimeofday(struct timeval *tv, struct timezone *tz){
	TM_SYSTEMTIME SystemTime = {0,} ;
    const unsigned long long timeAfter2000 = 946684800;

	static unsigned int record_millis;
	static unsigned int record_time;
	volatile unsigned int ticks= hal_TimGetUpTime();
	ticks &=0x3fff;
	volatile unsigned int millis= (ticks*1000)>>14;// (ticks%16384 *1000)/16384;//(ticks*1000)>>14;
	volatile unsigned int time = TM_GetTimeZoneTime(0,&SystemTime);
	while (!time)
	{
		time = TM_GetTimeZoneTime(0,&SystemTime);
		sys_arch_printf("gettimeofday time return null\n");
		COS_Sleep(50);
	}
	time -= (int)(TM_GetTimeZone()*60*60); //get UTC Time
	if (millis < record_millis && time == record_time)
	{
		millis = 1000+millis;
	}
	record_millis = millis;
	record_time = time;
    if (tv == NULL)
        return -1;
    else {
        tv->tv_sec = time + timeAfter2000;
        tv->tv_usec = millis;
        return 0;
    }
}

time_t time(time_t* timer)
{
    struct timeval tv;
    time_t ret = gettimeofday(&tv,NULL);
    if (ret >=0)
        ret = tv.tv_sec;
    if (timer != NULL)
        *timer = ret;
    return ret;
}


UINT8 *strerror(int errno)
{
    static char str[10];
    return itoa(errno,str,10);
}

INT32 strcasecmp (const INT8 *pcStr2, const INT8 *pcStr1)
{

    INT32 iRetVal = 0 ;
    INT32 istr1_len ;
    INT32 istr2_len ;
    INT32 iNum = 0 ;

    if (pcStr2 != NULL || pcStr1 != NULL)
    {
        istr1_len = (INT32)(strlen (pcStr1)) ;
        istr2_len = (INT32)(strlen (pcStr2)) ;
        if (istr1_len != istr2_len)
        {
            iRetVal =  -1 ;
        }
        else
        {
            for (iNum=0; iNum < istr1_len; iNum++)
            {
                if (tolower((INT8)pcStr2 [iNum]) \
                        != tolower((INT8)pcStr1 [iNum]))
                {
                    iRetVal = -1 ;
                    break;
                }
            }
        }
    }
    return ( iRetVal ) ;
}

int IN6_IS_ADDR_V4MAPPED(const struct in6_addr *addr)
{
#if LWIP_IPV6
    return ip6_addr_isipv4mappedipv6((ip6_addr_t *)addr);
#else
    return 0;
#endif
}

int get_port(struct sockaddr *x)
{
   if (x->sa_family == AF_INET)
   {
       return ((struct sockaddr_in *)x)->sin_port;
#ifdef LWIP_IPV6_ON
   } else if (x->sa_family == AF_INET6) {
       return ((struct sockaddr_in6 *)x)->sin6_port;
#endif
   } else {
       printf("non IPV4 or IPV6 address\n");
       return  -1;
   }
}

int sockaddr_cmp(struct sockaddr *x, struct sockaddr *y)
{
    int portX = get_port(x);
    int portY = get_port(y);

    // if the port is invalid of different
    if (portX == -1 || portX != portY)
    {
        return 0;
    }

    // IPV4?
    if (x->sa_family == AF_INET)
    {
        // is V4?
        if (y->sa_family == AF_INET)
        {
            // compare V4 with V4
            return ((struct sockaddr_in *)x)->sin_addr.s_addr == ((struct sockaddr_in *)y)->sin_addr.s_addr;
            // is V6 mapped V4?
#ifdef LWIP_IPV6_ON
        } else if (IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6 *)y)->sin6_addr)) {
            struct in6_addr* addr6 = &((struct sockaddr_in6 *)y)->sin6_addr;
            uint32_t y6to4 = addr6->s6_addr[15] << 24 | addr6->s6_addr[14] << 16 | addr6->s6_addr[13] << 8 | addr6->s6_addr[12];
            return y6to4 == ((struct sockaddr_in *)x)->sin_addr.s_addr;
#endif
        } else {
            return 0;
        }
#ifdef LWIP_IPV6_ON
    } else if (x->sa_family == AF_INET6 && y->sa_family == AF_INET6) {
        // IPV6 with IPV6 compare
        return memcmp(((struct sockaddr_in6 *)x)->sin6_addr.s6_addr, ((struct sockaddr_in6 *)y)->sin6_addr.s6_addr, 16) == 0;
#endif
    } else {
        // unknown address type
        printf("non IPV4 or IPV6 address\n");
        return 0;
    }
}

struct netif *
ip4_src_route(const ip4_addr_t *dst, const ip4_addr_t *src)
{
  LWIP_UNUSED_ARG(dst); /* in case IPv4-only and source-based routing is disabled */
  struct netif *netif;

  if(src == NULL) return NULL;
  /* iterate through netifs */
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    /* is the netif up, does it have a link and a valid address? */
    if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
      /* network mask matches? */
      if (ip4_addr_cmp(src, netif_ip4_addr(netif))) {
        /* return netif on which to forward IP packet */
        return netif;
      }
    }
  }
  return NULL;
}


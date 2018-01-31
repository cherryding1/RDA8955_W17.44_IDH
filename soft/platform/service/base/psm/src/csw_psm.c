
#ifdef LTE_NBIOT_SUPPORT
#include "csw_psm.h"
#include <csw.h>
#include "cswtype.h"
#include "cfw_prv.h"
#include "cfw_multi_sim.h"
#include "at_cfg.h"
#include "at_trace.h"
//#include "aps.h"
//#include "csw_ppp.h"
#include "sap_psm_itf.h"
#include "ts.h"
#include "cs_types.h"

#include "sockets.h"


//#include "cfw_usim.h"

extern sa_info_t g_SaInfo;
extern AT_ModuleInfo g_AtModule;
extern UINT8 crtdcp_reporting;
extern UINT8 g_gc_pwroff;
extern UINT8 g_cfg_cfun[2];
extern UINT8 g_uATNWECSQFlag;
extern UINT8 g_COPS_Mode;
extern UINT8 g_FirstREG;
extern UINT8 g_uAtCereg;
//extern BOOL g_COPS_Set_Flag[CFW_SIM_COUNT];
//extern UINT8 g_arrPreferredOperater[];
//extern UINT8 g_uAtGprsEnterDataModeMux[MAX_DLC_NUM];
extern AT_Gprs_CidInfo g_staAtGprsCidInfo_e[4][AT_PDPCID_MAX + 1] ;
extern UINT32   UTI_Status[8];
extern CFW_GPRS_PDPCONT_INFO *PdpContList[CFW_SIM_COUNT][7];
extern CFW_GPRS_QOS pQosList[CFW_SIM_COUNT][7];
extern UINT32 storecid[CFW_SIM_COUNT][8];
extern UINT8 GprsCidSavedInApp[CFW_SIM_COUNT];

#ifdef NBIOT_APPLICATION_PSM
extern UINT8  udpflag;
extern UINT8 g_uATTcpipState[AT_MUXSOCKET_NUM];
extern UINT8 g_uATTcpipCid[AT_MUXSOCKET_NUM];
extern stAT_Tcpip_Paras g_stATTcpipParas[AT_MUXSOCKET_NUM];
extern UINT8 g_uATTcpipSocket[AT_MUXSOCKET_NUM];


struct lwip_sock {
  /** sockets currently are built on netconns, each socket has one netconn */
  struct netconn *conn;
  /** data that was left from the previous read */
  void *lastdata;
  /** offset in the data that was left from the previous read */
  u16_t lastoffset;
  /** number of times data was received, set by event_callback(),
      tested by the receive and select functions */
  s16_t rcvevent;
  /** number of times data was ACKed (free send buffer), set by event_callback(),
      tested by select */
  u16_t sendevent;
  /** error happened for this socket, set by event_callback(), tested by select */
  u16_t errevent;
  /** last error that occurred on this socket (in fact, all our errnos fit into an u8_t) */
  u8_t err;
  /** counter of how many threads are waiting for this socket using select */
  u8_t select_waiting;
};
extern struct lwip_sock sockets[];

extern struct udp_pcb *udp_pcbs;
#endif //NBIOT_APPLICATION_PSM
extern UINT8 g_uAtGprsCidNum;
extern UINT8 g_uAttState;
extern UINT8 DnsServerIP[2][7][2];

typedef struct _AO_CONTENT
{
    UINT32                UTI;
    UINT32                nProcCode;
    void                  *UserData;
    PFN_AO_PORC           pAO_Proc;
    UINT32                AO_State;
    UINT32                AO_NextState;////AO_NextState  1:we need processAO,add AO to AO_ControlBlock.0:we don't need add AO to AO_ControlBlock.

    PFN_CFW_EV_PORC    pfnEvProc;
    void                  *pEventUserData;

    struct _AO_CONTENT    *NextAO;
    UINT32                nServiceID;
    CFW_SIM_ID        nSimID;
    UINT8                  nWhichAO; // 0, Active AO, 1, Ready AO, 2, Special AO;
} AO_CONTENT;


typedef struct  _AO_CB_DOING
{
    AO_CONTENT                   *DoingAO;

    struct  _AO_CB_DOING         *NextDoing;

} AO_CB_DOING;

/*AO Control Block*/
typedef struct  _AO_CB
{
    AO_CONTENT         *AcitveAO;
    AO_CONTENT         *ReadyAO;

    AO_CONTENT         *SpecialAO;      //for something marked...

    AO_CB_DOING        *DoingHead;
    AO_CB_DOING        *DoingTail;

    UINT8              ActiveNum;
    UINT8              ReadyNum;
    UINT8              pading2;
    UINT8              pading1;

} AO_CB;

typedef struct CFW_UsimAidTag
{
    UINT8       data[16];           // AID value
    UINT8       length;                 // AID length
} CFW_UsimAid;

extern AO_CB    AO_ControlBlock[CFW_MAX_SRV_ID];
extern UINT8 nICCID[CFW_SIM_COUNT][ICCID_LENGTH];
extern CFW_UsimAid gUsimAid[CFW_SIM_COUNT] ;


//function declaration
extern VOID CSW_TC_MEMBLOCK(UINT16 uMask, UINT8 *buffer, UINT16 len, UINT8 radix);
extern BOOL at_CfgGetAtSettings(UINT8 flag, UINT8 set_id);
extern BOOL MUX_Init(UINT8 nMode, BOOL init);
extern HAO CFW_RegisterAo( UINT32 nServiceId, PVOID pUserData, PFN_AO_PORC pfnAoProc, CFW_SIM_ID nSimID);
extern UINT32 CFW_GprsAoProc (HAO hAo, CFW_EV *pEvent);
extern UINT32 CFW_GprsGetPdpAddr(UINT8 nCid, UINT8 *nLength, UINT8 *pPdpAdd, CFW_SIM_ID nSimID);
extern void sxr_Send (void *Msg, u8 Sbx, u8 MsgStatus);
extern UINT32 cfg_write_str(UINT8 *uSubkey, UINT8 *name, UINT8 *data, UINT8 size);

#ifdef NBIOT_APPLICATION_PSM
struct netif *
netif_add(struct netif *netif,
#if LWIP_IPV4
          const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
#endif /* LWIP_IPV4 */
          void *state, netif_init_fn init, netif_input_fn input);
extern err_t data_output(struct netif *netif, struct pbuf *p,ip_addr_t *ipaddr);
extern VOID netif_set_default(struct netif *netif);
extern VOID netif_set_up(struct netif *netif);
extern VOID recv_udp(VOID *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, UINT16 port);
extern UINT32 CFW_TcpipInetAddr(const INT8 *cp);
#endif //NBIOT_APPLICATION_PSM


#ifdef NBIOT_APPLICATION_PSM
static void CswPsmFindApsUdpSocketByPort(u32 localIpAddr, u32 remoteIpAddr, u16 remotePort, AT_Gprs_Psm_SocketInfo  *socketInfo);
static void CswPsmSaveApsTcpipStaticBackupInfo(T_Aps_Tcpip_Static_Context_Backup_Info *apsTcpipStaticCtxBackupInfo, u8 i);
static void CswPsmSaveApsStaticBackupInfo(T_Aps_Static_Context_Backup_Info *apsStaticCtxBackupInfo);

static void CswPsmRestoreApsUdpStaticBackupInfo(AT_Gprs_Psm_SocketInfo *udpSocketInfo, u8 i);
static void CswPsmRestoreApsTcpipStaticBackupInfo(T_Aps_Tcpip_Static_Context_Backup_Info  *apsTcpipStaticCtxBackupInfo, u8 i);
static void CswPsmRestoreApsPppStaticBackupInfo(T_Aps_Ppp_Static_Context_Backup_Info  *apsPppStaticCtxBackupInfo, u8 i);
static void CswPsmRestoreApsStaticBackupInfo(T_Aps_Static_Context_Backup_Info *apsStaticCtxBackupInfo);
#endif //NBIOT_APPLICATION_PSM

static void CswPsmSaveCfwAomStaticBackupInfo(T_Cfw_Aom_Static_Context_Backup_Info *backupInfo);
static UINT32 CswPsmSaveDnsServerToFlash(UINT8 * DnsServer);
static void CswPsmSaveCfwUsimStaticBackupInfo(T_Cfw_Usim_Static_Context_Backup_Info *backupInfo);
static void CswPsmSaveCfwGprsStaticBackupInfo(T_Cfw_Gprs_Static_Context_Backup_Info *cfwGprsStaticCtxBackupInfo);
static void CswPsmSaveCfwStaticBackupInfo(T_Cfw_Static_Context_Backup_Info *backupInfo);
static void CswPsmSaveAtStaticBackupInfo(T_AT_Static_Context_Backup_Info *backupInfo);
static void CswPsmSaveStaticBackupInfo(T_CSW_Static_Context_Backup_Info *backupInfo);
static void CswPsmSaveCfwNwDynamicBackupInfo(T_Cfw_Nw_Dynamic_Context_Backup_Info *backupInfo);
static void CswPsmSaveCfwGprsDynamicBackupInfo(T_Cfw_Gprs_Dynamic_Context_Backup_Info *backupInfo);
static void CswPsmSaveCfwDynamicBackupInfo(T_Cfw_Dynamic_Context_Backup_Info *backupInfo);
static void CswPsmSaveDynamicBackupInfo(T_CSW_Dynamic_Context_Backup_Info  *backupInfo);
static void CswPsmRestoreCfwAomStaticBackupInfo(T_Cfw_Aom_Static_Context_Backup_Info *backupInfo);
static void CswPsmRestoreCfwUsimStaticBackupInfo(T_Cfw_Usim_Static_Context_Backup_Info *backupInfo);
static void CswPsmRestoreCfwGprsStaticBackupInfo(T_Cfw_Gprs_Static_Context_Backup_Info *cfwGprsStaticCtxBackupInfo);
static void CswPsmRestoreCfwStaticBackupInfo(T_Cfw_Static_Context_Backup_Info *cfwStaticCtxBackupInfo);
static void CswPsmRestoreAtStaticBackupInfo(T_AT_Static_Context_Backup_Info *atStaticCtxBackupInfo);
static void CswPsmRestoreStaticBackupInfo(T_CSW_Static_Context_Backup_Info *backupInfo);
static void CswPsmRestoreCfwNwDynamicBackupInfo( T_Cfw_Nw_Dynamic_Context_Backup_Info *backupInfo);
static void CswPsmRestoreCfwGprsDynamicBackupInfo(T_Cfw_Gprs_Dynamic_Context_Backup_Info *backupInfo);
static void CswPsmRestoreCfwDynamicBackupInfo(T_Cfw_Dynamic_Context_Backup_Info *backupInfo);
static void CswPsmRestoreDynamicBackupInfo(T_CSW_Dynamic_Context_Backup_Info *backupInfo);


/*Common*/

/*save*/
static void CswPsmSaveCfwAomStaticBackupInfo(T_Cfw_Aom_Static_Context_Backup_Info *backupInfo)
{
    memcpy(backupInfo->UTI_Status, UTI_Status, 8*sizeof(UINT32));
}

static UINT32 CswPsmSaveDnsServerToFlash(UINT8 * DnsServer)
{
    UINT8 ret = 0;
    UINT8 saveDnsAddr[20] = {0};
    sprintf(saveDnsAddr, "%d.%d.%d.%d",DnsServer[0],DnsServer[1],DnsServer[2],DnsServer[3]);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("xuzuohua: saveDnsToFlash: %d.%d.%d.%d",0x081007a5),DnsServer[0],DnsServer[1],DnsServer[2],DnsServer[3]);
    ret = cfg_write_str("TCPIP","DNS_ADR1", (PVOID)saveDnsAddr, strlen(saveDnsAddr));
    if(ret == 0)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: CswPsmSaveDnsServerToFlash OK",0x081007a5));
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: CswPsmSaveDnsServerToFlash NOK",0x081007a5));
    }

    return ret;
}

static void CswPsmSaveCfwUsimStaticBackupInfo(T_Cfw_Usim_Static_Context_Backup_Info *backupInfo)
{
    memcpy(backupInfo->iccid, nICCID[0], ICCID_LENGTH);
   // CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: CswPsmSaveDnsServerToFlash NOK",0x081007a5));
    memcpy( &backupInfo->usimAid,gUsimAid, sizeof(USim_Aid));
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua backupInfo->usimAid.len :%d",0x081007a5), backupInfo->usimAid.length);
}

static void CswPsmSaveCfwGprsStaticBackupInfo(T_Cfw_Gprs_Static_Context_Backup_Info *cfwGprsStaticCtxBackupInfo)
{
    UINT8 i = 0, j =0;
    CFW_GPRS_PDPCONT_INFO * new_pdp = NULL;
    CfwStaticPdpContList * pdpCtxList = cfwGprsStaticCtxBackupInfo->cfwStaticPdpContList;

    for(i=0; i < 2; i++)
    {
        pdpCtxList[i].nCid = 0xff;
    }
    
    for(i = 0; i < 7; i++)
    {
        if(PdpContList[0][i] != NULL)
        {
            new_pdp = PdpContList[0][i];
            pdpCtxList[j].nCid = i+1;
	        pdpCtxList[j].nHComp_nDComp = new_pdp->nDComp;
	        pdpCtxList[j].nHComp_nDComp |= (new_pdp->nHComp << 4)&0xf0;
	        if(new_pdp->pApn != NULL)
	        {
	            memcpy(pdpCtxList[j].apnName, new_pdp->pApn, new_pdp->nApnSize);
		        //CSW_GPRS_FREE(new_pdp->pApn);
	        }
	        if(new_pdp->pPdpAddr != NULL)
	        {
	            memcpy(pdpCtxList[j].pdpAddr, new_pdp->pPdpAddr, new_pdp->nPdpAddrSize);
		        //CSW_GPRS_FREE(new_pdp->pPdpAddr);
	        }
	        CswPsmSaveDnsServerToFlash(DnsServerIP[0][i]); //need debug
	        //CFW_SetDnsServer(NULL, NULL);  //to be doing
	        pdpCtxList[j].nSlpi_PdnType = new_pdp->PdnType;
			pdpCtxList[j].nSlpi_PdnType |= (new_pdp->nSlpi << 4)&0xf0;
	        //CSW_GPRS_FREE(new_pdp);
	        pdpCtxList[j].eQos.nDlGbr = pQosList[0][i].nGuaranteedbitrateDL;
	        pdpCtxList[j].eQos.nUlGbr = pQosList[0][i].nGuaranteedbitrateUL;
	        pdpCtxList[j].eQos.nDlMbr = pQosList[0][i].nMaximumbitrateDL;
	        pdpCtxList[j].eQos.nUlMbr = pQosList[0][i].nMaximumbitrateUL;
            j++;
	        if(j>1)  //only store 2 pdp contexts
	        {
	            break;
	        }
        }
    }
	memcpy(cfwGprsStaticCtxBackupInfo->storeCid, &storecid[0][8], 8);
	cfwGprsStaticCtxBackupInfo->GprsCidSavedInApp = GprsCidSavedInApp[0];

}

static void CswPsmSaveCfwStaticBackupInfo(T_Cfw_Static_Context_Backup_Info *backupInfo)
{
    CswPsmSaveCfwAomStaticBackupInfo(&backupInfo->cfwAomStaticCtxBackupInfo);
    CswPsmSaveCfwUsimStaticBackupInfo(&backupInfo->cfwUsimStaticCtxBackupInfo);
    CswPsmSaveCfwGprsStaticBackupInfo(&backupInfo->cfwGprsStaticCtxBackupInfo);
}

AT_Gprs_Psm_CidInfo  saveGprsCidInfo[2];
static void CswPsmSaveAtStaticBackupInfo(T_AT_Static_Context_Backup_Info *backupInfo)
{
    u8     *dlciBitMap = &backupInfo->dlciBitMap;
    u8     i = 0, j = 0, k=0, m = 0;
    AT_Gprs_Psm_CidInfo  *atDestPsmGprsCidInfo = backupInfo->staAtGprsCidInfo;
	AT_Gprs_CidInfo *atSrcPsmGprsCidInfo = g_staAtGprsCidInfo_e[0];
  //  at_CfgGetAtSettings(USER_SETTING_1, 0);
   // at_CfgGetAtSettings(USER_SETTING_1, 1);
	
/*at_cmux*/
    
/*at_gc*/
    backupInfo->gc_pwroff = g_gc_pwroff;
    backupInfo->cfg_cfun = g_cfg_cfun[0];
	
    /*at_nw*/
    backupInfo->nwEcsqFlag = g_uATNWECSQFlag;
    backupInfo->copsMode = g_COPS_Mode; //取值范围是0~4，bitmap的话占3个bit
    backupInfo->firstReg = g_FirstREG;
    //backupInfo->copsSetFlag = g_COPS_Set_Flag[0];
    backupInfo->cereg = g_uAtCereg;  //Ranges from 0~5, 3 bits info enough
    //memcpy(backupInfo->preferOperator,g_arrPreferredOperater, 6*sizeof(UINT8));
	
   /*at_gprs*/
   backupInfo->crtdcp_reporting = crtdcp_reporting;
   //memcpy(backupInfo->gprsEnterDataModeMux, g_uAtGprsEnterDataModeMux, MAX_DLC_NUM*sizeof(UINT8));  //bitmap implment latter
   for(i=0; i < g_uAtGprsCidNum; i++)
   {
       for(j = k; j < 7; j++)
       {
           if(atSrcPsmGprsCidInfo[j].uState == AT_GPRS_ACT)
           {
               atDestPsmGprsCidInfo[i].nCid = atSrcPsmGprsCidInfo[j].uCid;
	        atDestPsmGprsCidInfo[i].uState = AT_GPRS_ACT;
		 atDestPsmGprsCidInfo[i].nDLCI = atSrcPsmGprsCidInfo[j].nDLCI;
		 memcpy(&saveGprsCidInfo[i], &atDestPsmGprsCidInfo[i], sizeof(AT_Gprs_Psm_CidInfo));
		 m++;
		 break;
           }
       }
	k = m;
   }
   g_uAttState = CFW_GPRS_ATTACHED;
   
  
}

#ifdef NBIOT_APPLICATION_PSM
static void CswPsmFindApsUdpSocketByPort(u32 localIpAddr, u32 remoteIpAddr, u16 remotePort, AT_Gprs_Psm_SocketInfo  *socketInfo)
{
    struct udp_pcb *ipcb;
    struct udp_pcb *pcb;
    struct lwip_sock *socketdsc = &sockets[socketInfo->uSocket];

    if(socketdsc->conn != NULL)
    {
        pcb = socketdsc->conn->pcb.udp;
	 if(pcb == NULL)
        {
            //have not found udp pcb
            socketInfo->socketState = UDP_SOCKET_STATE_NULL;
	     CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: no udp pcb found",0x081007a5));
	     return;
        }
    }
    for (ipcb = udp_pcbs; ipcb != NULL; ipcb = ipcb->next)
    {
        /* is this UDP PCB already on active list? */
	//if(ipcb !=NULL)
           //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTR("xuzuohua: socketInfo_print: %d/%d/%d/%d/%d/%d",0x081007a5),ipcb->local_ip.addr, ipcb->local_port, ipcb->remote_ip.addr,ipcb->remote_port,remoteIpAddr,remotePort);
        if (pcb == ipcb)
        {
            socketInfo->localIpAddr = ip_addr_get_ip4_u32(&ipcb->local_ip);// ipcb->local_ip.addr;
	     socketInfo->localPort = ipcb->local_port;
	     socketInfo->remoteIpAddr = remoteIpAddr;
            socketInfo->remotePort = remotePort;
	     if(localIpAddr ==  ip_addr_get_ip4_u32(&ipcb->local_ip))
	     {
		  socketInfo->socketState = UDP_SOCKET_STATE_BIND;
		  if((remoteIpAddr ==  ip_addr_get_ip4_u32(&ipcb->remote_ip)) && (remotePort == ipcb->remote_port) && (remotePort != 0))
		  {
			socketInfo->socketState = UDP_SOCKET_STATE_CONNECT;
		  }
	     }
	     else
	     {
	         socketInfo->socketState = UDP_SOCKET_STATE_NEW;
	     }
	     break;
        }
	 else
	 {
	     CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: nofindpcblist",0x081007a5) );
	 }
    }
    if(ipcb == NULL)
    {
        //have not found udp pcb
        socketInfo->socketState = UDP_SOCKET_STATE_ERROR;
	//     return;
     }
     CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua: socketState: %d",0x081007a5),socketInfo->socketState);
}

static void CswPsmSaveApsTcpipStaticBackupInfo(T_Aps_Tcpip_Static_Context_Backup_Info *apsTcpipStaticCtxBackupInfo, u8 i)
{
     UINT8 dlci = saveGprsCidInfo[i].nDLCI;
     UINT8  localIpLen = 0;
     UINT8  acIpAddr[18] = {0};
     UINT32 localIpAddr = 0;
     UINT32 remoteIpAddr = 0;
     apsTcpipStaticCtxBackupInfo->nCid[i] = saveGprsCidInfo[i].nCid;
     apsTcpipStaticCtxBackupInfo->isUdpSocket = udpflag;
      /*tcpip save*/
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(6), TSTR("xuzuohua: i/udpflag/cid/dlci/Aps_APP/tcpipState: %d/%d/%d/%d/%d/%d",0x081007a5),i, udpflag,saveGprsCidInfo[i].nCid,dlci,Aps_APP,g_uATTcpipState[dlci] );
    if((udpflag)||(Aps_APP == APS_TCP))
    {
           // if(g_uATTcpipState[dlci]  == AT_TCPIP_STATE_CONNECTING)
            {
                //save(udp bind) infomation
                apsTcpipStaticCtxBackupInfo->socketInfo[i].uSocket = g_stATTcpipParas[dlci].uSocket; //socket idx
	         apsTcpipStaticCtxBackupInfo->socketInfo[i].tcpipState = g_uATTcpipState[dlci];
			     CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("xuzuohua: i/dlci/socket/remotePort: %d/%d/%d/%d",0x081007a5),i, dlci,g_stATTcpipParas[dlci].uSocket,g_stATTcpipParas[dlci].uPort);

		  memcpy(apsTcpipStaticCtxBackupInfo->socketInfo[i].remoteIPAddrStr,g_stATTcpipParas[dlci].uaIPAddress,AT_TCPIP_ADDRLEN+3); //remote port
		  CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, g_stATTcpipParas[dlci].uaIPAddress, 23, 16); 
		  CFW_GprsGetPdpAddr(saveGprsCidInfo[i].nCid, &localIpLen, acIpAddr, 0);
		  localIpAddr = htonl(acIpAddr[0] << 24 | acIpAddr[1] << 16 | acIpAddr[2] << 8 | acIpAddr[3]);
		  remoteIpAddr = CFW_TcpipInetAddr(g_stATTcpipParas[dlci].uaIPAddress);
		  CswPsmFindApsUdpSocketByPort(localIpAddr,remoteIpAddr,htons(g_stATTcpipParas[dlci].uPort), &apsTcpipStaticCtxBackupInfo->socketInfo[i]);
        }
    }
}


static void CswPsmSaveApsStaticBackupInfo(T_Aps_Static_Context_Backup_Info *apsStaticCtxBackupInfo)
{
    u8 i = 0;
    u8 active_num = 0;
    if(APS_TCP == Aps_APP)
    {
        active_num = g_uAtGprsCidNum;
    }
    else if(APS_PPP == Aps_APP)
    {
        while(i < MAX_PPP_ID)
        {
            if(ppp_Data[i].ppp_Gprs_state == PPP_ACTIVED)
            {
                active_num++;
            }
	     i++;
	     if(active_num > 1) 
	     {
	         break;
	     }
        }
    }
    apsStaticCtxBackupInfo[0].aps_app = Aps_APP;
    for(i = 0; i < active_num; i++)
    {
        switch(Aps_APP)
        {
            case APS_TCP:
	         CswPsmSaveApsTcpipStaticBackupInfo(&apsStaticCtxBackupInfo[i].apsStaticInfo.apsTcpipStaticCtxBackupInfo, i);
	         break;

	         break;
            default: break;
        }
    }
}
#endif //NBIOT_APPLICATION_PSM
static void CswPsmSaveStaticBackupInfo(T_CSW_Static_Context_Backup_Info *backupInfo)
{
    CswPsmSaveCfwStaticBackupInfo(&backupInfo->cfwStaticCtxBackupInfo);
    CswPsmSaveAtStaticBackupInfo(&backupInfo->atStaticCtxBackupInfo);
#ifdef NBIOT_APPLICATION_PSM
    CswPsmSaveApsStaticBackupInfo(backupInfo->apsStaticCtxBackupInfo);
#endif //NBIOT_APPLICATION_PSM
}

static void CswPsmSaveCfwNwDynamicBackupInfo(T_Cfw_Nw_Dynamic_Context_Backup_Info *backupInfo)
{
   HAO  s_hAo;
   AO_CONTENT *pAo;
   NW_SM_INFO *s_proc;
   s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, 0);
   pAo = (AO_CONTENT *)s_hAo;
   if(pAo != NULL)
   {
       backupInfo->AO_State = pAo->AO_State;
	backupInfo->UTI = pAo->UTI;
	//user data
	s_proc     = CFW_GetAoUserData(s_hAo);
	if((s_proc != NULL) && (s_proc->bStaticAO == TRUE))
	{
	    backupInfo->saveStaticAO = TRUE;
	    memcpy(&backupInfo->NwSmInfo, s_proc, sizeof(NW_SM_INFO));
	}
   }
}

static void CswPsmSaveCfwGprsDynamicBackupInfo(T_Cfw_Gprs_Dynamic_Context_Backup_Info *backupInfo)
{
    UINT8 i = 0;
    UINT8 nServiceId ;
    UINT8 nActiveAONumber;// = AO_ControlBlock[nServiceId].ActiveNum;
    HAO hAo;
    AO_CONTENT *pAo;
    GPRS_SM_INFO* pUserData;
    nServiceId = CFW_GPRS_SRV_ID *CFW_SIM_COUNT +0;
    nActiveAONumber = AO_ControlBlock[nServiceId].ActiveNum;
    for(; i < nActiveAONumber; i++)
    {
        hAo = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_GPRS_SRV_ID, 0);
	    pAo = (AO_CONTENT *)hAo;
	    pUserData = CFW_GetAoUserData(hAo);
	    if(pAo != NULL)
	    {
	 	   backupInfo[i].UTI = pAo->UTI;
		   if(pUserData != NULL)
		   {
		       backupInfo[i].nSmIndex = pUserData->Cid2Index.nSmIndex; //have a padding
		       backupInfo[i].activated = TRUE;  
		   }
	    }
	 CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("xuzuohua_dyn_gprs0:idx/cid/smidx/pdpAddrSize/uti: %d/%d/%d/%d/%d",0x081007a5),
       i, pUserData->Cid2Index.nCid,pUserData->Cid2Index.nSmIndex,pUserData->nPDPAddrSize,pAo->UTI);
        
    }
}

static void CswPsmSaveCfwDynamicBackupInfo(T_Cfw_Dynamic_Context_Backup_Info *backupInfo)
{
    //cfw_nw
    CswPsmSaveCfwNwDynamicBackupInfo(&backupInfo->cfwNwDynamicCtxBackupInfo);
    //cfw_gprs
    CswPsmSaveCfwGprsDynamicBackupInfo(backupInfo->cfwGprsDynamicCtxBackupInfo);
}

static void CswPsmSaveDynamicBackupInfo(T_CSW_Dynamic_Context_Backup_Info  *backupInfo)
{
    CswPsmSaveCfwDynamicBackupInfo(&backupInfo->cfwDynamicCtxBackupInfo);
}

void CswPsmSaveBackupInfo(T_CSW_Context_Backup_Info *backupInfo)
{
    memset(backupInfo, 0x00, sizeof(T_CSW_Context_Backup_Info));
    CswPsmSaveStaticBackupInfo(&backupInfo->staticBackupInfo);
    CswPsmSaveDynamicBackupInfo(&backupInfo->dynamicBackupInfo);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua: csw_psm_backup_info_size: %d",0x081007a5), sizeof(T_CSW_Context_Backup_Info));

}

/*I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!*/
/*I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!*/
/*I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!I am the segment line, plz be attention!!!*/

/*restore*/

static void CswPsmRestoreCfwAomStaticBackupInfo(T_Cfw_Aom_Static_Context_Backup_Info *backupInfo)
{
	//AO_GoOnStatus ignore
    memcpy(UTI_Status, backupInfo->UTI_Status, 8*sizeof(UINT32));
}

static void CswPsmRestoreCfwUsimStaticBackupInfo(T_Cfw_Usim_Static_Context_Backup_Info *backupInfo)
{
    memcpy(nICCID[0], backupInfo->iccid, ICCID_LENGTH);
    memcpy(gUsimAid, &backupInfo->usimAid, sizeof(USim_Aid));
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("xuzuohua backupInfo->usimAid.len :%d/%d",0x081007a5), backupInfo->usimAid.length, gUsimAid[0].length);
}

static void CswPsmRestoreCfwGprsStaticBackupInfo(T_Cfw_Gprs_Static_Context_Backup_Info *cfwGprsStaticCtxBackupInfo)
{
    UINT8 i = 0;
    UINT8 nCid = 0;
    CFW_GPRS_PDPCONT_INFO * new_pdp = NULL;
    CfwStaticPdpContList * pdpCtxList = cfwGprsStaticCtxBackupInfo->cfwStaticPdpContList;
    ip_addr_t PriDnsSer;
    memset(&PriDnsSer, 0x00, sizeof(ip_addr_t));
    //CFW_EQOS                   *eQos = cfwGprsStaticCtxBackupInfo->eQos;
    for(i = 0; i < 2; i++)
    {
        nCid = pdpCtxList[i].nCid;
        if(nCid != 0xff)
        {
            g_uAtGprsCidNum++;
            new_pdp = CSW_GPRS_MALLOC(sizeof(CFW_GPRS_PDPCONT_INFO));
	        if(new_pdp != NULL)
	        {
	            memset(new_pdp, 0x00, sizeof(CFW_GPRS_PDPCONT_INFO));
	            new_pdp->nDComp = pdpCtxList[i].nHComp_nDComp & 0x0f;
	            new_pdp->nHComp = (pdpCtxList[i].nHComp_nDComp >> 4) & 0x0f;
                new_pdp->nApnSize = strlen(pdpCtxList[i].apnName);
		        if(new_pdp->nApnSize > 0)
		        {
	                new_pdp->pApn = CSW_GPRS_MALLOC(new_pdp->nApnSize);
		            if(new_pdp->pApn != NULL)
		            {
		               // memset(new_pdp->pApn, 0x00, new_pdp->nApnSize + 1);
		               memcpy(new_pdp->pApn, pdpCtxList[i].apnName,new_pdp->nApnSize);
		            }
		        }
		        /*we don't support pap/chap currently, so ignore apnUser&pwd at this moment*/
		        // new_pdp->apnUser   
		        // new_pdp->apnPwd
		        new_pdp->nPdpAddrSize = strlen(pdpCtxList[i].pdpAddr);
		        if(new_pdp->nPdpAddrSize > 0)
		        {
		            new_pdp->pPdpAddr = CSW_GPRS_MALLOC(new_pdp->nPdpAddrSize);
		            if(new_pdp->pPdpAddr != NULL)
		            {
		                //memset(new_pdp->pPdpAddr, 0x00, THE_PDP_ADDR_MAX_LEN*sizeof(UINT8));
			            memcpy(new_pdp->pPdpAddr, pdpCtxList[i].pdpAddr, new_pdp->nPdpAddrSize);
		            }
		        }
		        new_pdp->nPdpType = new_pdp->PdnType = pdpCtxList[i].nSlpi_PdnType & 0x0f;
				new_pdp->nSlpi = (pdpCtxList[i].nSlpi_PdnType >> 4) & 0x0f;
                PdpContList[0][nCid-1] = new_pdp;
	        }

	        pQosList[0][nCid-1].nGuaranteedbitrateDL = pdpCtxList[i].eQos.nDlGbr;
	        pQosList[0][nCid-1].nGuaranteedbitrateUL = pdpCtxList[i].eQos.nUlGbr;
	        pQosList[0][nCid-1].nMaximumbitrateDL    = pdpCtxList[i].eQos.nDlMbr;
	        pQosList[0][nCid-1].nMaximumbitrateUL    = pdpCtxList[i].eQos.nUlMbr;
	    //pMinQosList     to be stored
#if 0
            if(DnsServerIP[0][nCid-1] == NULL)
            {
                DnsServerIP[0][nCid-1] = (UINT8 *)CSW_GPRS_MALLOC(4);
	            DnsServerIP[0][nCid-1][0] = (UINT8) (ntohl(ip_addr_get_ip4_u32(&PriDnsSer)) >> 24 & 0xff); ;//PriDnsSer.addr & 0xff;
		        DnsServerIP[0][nCid-1][1] =(UINT8) (ntohl(ip_addr_get_ip4_u32(&PriDnsSer)) >> 16 & 0xff);
		        DnsServerIP[0][nCid-1][2] = (UINT8) (ntohl(ip_addr_get_ip4_u32(&PriDnsSer)) >> 8 & 0xff);
		        DnsServerIP[0][nCid-1][3] =(UINT8) (ntohl(ip_addr_get_ip4_u32(&PriDnsSer)) & 0xff);
		        CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("xuzuohua: restore dns server is: %d/ %d.%d.%d.%d",0x081007a5),nCid, DnsServerIP[0][nCid-1][0],DnsServerIP[0][nCid-1][1],DnsServerIP[0][nCid-1][2],DnsServerIP[0][nCid-1][3]);
            }
 #endif
        }
    }
	memcpy(&storecid[0][8], cfwGprsStaticCtxBackupInfo->storeCid, 8);
	GprsCidSavedInApp[0] = cfwGprsStaticCtxBackupInfo->GprsCidSavedInApp;
}

static void CswPsmRestoreCfwStaticBackupInfo(T_Cfw_Static_Context_Backup_Info *cfwStaticCtxBackupInfo)
{
/*cfw_aom*/
    CswPsmRestoreCfwAomStaticBackupInfo(&cfwStaticCtxBackupInfo->cfwAomStaticCtxBackupInfo);
/*cfw_gprs*/
    CswPsmRestoreCfwGprsStaticBackupInfo(&cfwStaticCtxBackupInfo->cfwGprsStaticCtxBackupInfo);
/*cfw_sim*/
    CswPsmRestoreCfwUsimStaticBackupInfo(&cfwStaticCtxBackupInfo->cfwUsimStaticCtxBackupInfo);
/*others*/
//none

}

static AT_Gprs_CidInfo restoreGprsCidInfo[2];
static void CswPsmRestoreAtStaticBackupInfo(T_AT_Static_Context_Backup_Info *atStaticCtxBackupInfo)
{
    u8     dlciBitMap = atStaticCtxBackupInfo->dlciBitMap;
    u8     i = 0;
    u8     nCid = 0;
    u8     dlci = 0;
    u8     state = 0;
    AT_Gprs_CidInfo *gprsCidInfo = g_staAtGprsCidInfo_e[0];
    CFW_GPRS_PDPCONT_INFO *pdp_ctx = NULL;
    /*at_uart*/
    //none
//    	 CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("xuzuohua: restore cmux: %d/%d",0x081007a5),atStaticCtxBackupInfo->bMuxActiveStatus,dlciBitMap);

    /*at_cmux*/
    
    /*at_gc*/
    g_gc_pwroff = atStaticCtxBackupInfo->gc_pwroff;
    g_cfg_cfun[0] = atStaticCtxBackupInfo->cfg_cfun;
	
    /*at_nw*/
    g_uATNWECSQFlag = atStaticCtxBackupInfo->nwEcsqFlag;
    g_COPS_Mode = atStaticCtxBackupInfo->copsMode;  //取值范围是0~4，bitmap的话占3个bit
    g_FirstREG = atStaticCtxBackupInfo->firstReg;
    //g_COPS_Set_Flag[0] = atStaticCtxBackupInfo->copsSetFlag;
    g_uAtCereg = atStaticCtxBackupInfo->cereg;  //Ranges from 0~5, 3 bits info enough
    //memcpy(g_arrPreferredOperater,atStaticCtxBackupInfo->preferOperator, 6*sizeof(UINT8));

   /*at_gprs*/
   crtdcp_reporting = atStaticCtxBackupInfo->crtdcp_reporting;
   //memcpy(g_uAtGprsEnterDataModeMux, atStaticCtxBackupInfo->gprsEnterDataModeMux, MAX_DLC_NUM*sizeof(UINT8));  //bitmap implment latter

   //g_uAtGprsCidNum = 0;  from pdpctxlist
   for(i=0; i< g_uAtGprsCidNum; i++)
   {
       nCid = atStaticCtxBackupInfo->staAtGprsCidInfo[i].nCid;
	dlci = atStaticCtxBackupInfo->staAtGprsCidInfo[i].nDLCI;
	state = atStaticCtxBackupInfo->staAtGprsCidInfo[i].uState;
       gprsCidInfo[nCid].uCid = nCid;
	restoreGprsCidInfo[i].uCid = nCid;
	gprsCidInfo[nCid].nDLCI = dlci;
	restoreGprsCidInfo[i].nDLCI = dlci;
	gprsCidInfo[nCid].uState = state;
	restoreGprsCidInfo[i].uState = state;
	CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), \
	    TSTR("xuzuohua: kaokaokao gprsCidInfo cid/dlci: %d/%d",0x081007a5),nCid, dlci);

	pdp_ctx = PdpContList[0][nCid-1] ;
	if(pdp_ctx  != NULL)
	{
	    gprsCidInfo[nCid].nHComp =  pdp_ctx->nHComp;
	    gprsCidInfo[nCid].nDComp =  pdp_ctx->nDComp;     //PdpContList[0][nCid-1]
	    gprsCidInfo[nCid].nPdpType = pdp_ctx->PdnType;
	    if(pdp_ctx->pApn != NULL)
	    {
	        memcpy(gprsCidInfo[nCid].pApn, pdp_ctx->pApn, pdp_ctx->nApnSize);
		 gprsCidInfo[nCid].nApnSize = pdp_ctx->nApnSize;
	    }
	    else 
	    {
	        //print
	    }
	    if(pdp_ctx->pPdpAddr != NULL)
	    {
	        memcpy(gprsCidInfo[nCid].pPdpAddr, pdp_ctx->pPdpAddr, pdp_ctx->nPdpAddrSize);
		 gprsCidInfo[nCid].nPdpAddrSize = pdp_ctx->nPdpAddrSize;
	    }
	    else
	    {
	        //print
	    }
	}
   }
  // memcpy(restoreGprsCidInfo, gprsCidInfo, sizeof(AT_Gprs_CidInfo)*2);
   /*at_tcpip*/
   //restore from aps tcpip
}

#ifdef NBIOT_APPLICATION_PSM
static void CswPsmRestoreApsUdpStaticBackupInfo(AT_Gprs_Psm_SocketInfo *udpSocketInfo, u8 i)
{
    struct lwip_sock *socket_dsc;

    struct netconn *conn;
    ip_addr_t localIp, remoteIp;
//    ip_addr_get_ip4_u32(&localIp) = udpSocketInfo[i].localIpAddr; //need convert format
    ip_addr_set_ip4_u32(&localIp, udpSocketInfo[i].localIpAddr);
   // ip_addr_get_ip4_u32(&remoteIp) = udpSocketInfo[i].remoteIpAddr;
   ip_addr_set_ip4_u32(&remoteIp, udpSocketInfo[i].remoteIpAddr);
   CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("xuzuohua: socketInfo: %d/%d/%d/%d/%d",0x081007a5),udpSocketInfo[i].socketState,udpSocketInfo[i].localIpAddr,udpSocketInfo[i].localPort,udpSocketInfo[i].remoteIpAddr,udpSocketInfo[i].remotePort);

   switch(udpSocketInfo[i].socketState)
   {
       case UDP_SOCKET_STATE_NULL:
	    return;
	case UDP_SOCKET_STATE_NEW:
	case UDP_SOCKET_STATE_BIND:
	case UDP_SOCKET_STATE_CONNECT:
	    conn = memp_malloc(MEMP_NETCONN);
	    if (conn == NULL)
	    {
	        //assert
	        CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("memp_malloc() == NULL !\n",0x08101163))); //zouyu
	        return;
	    }
#ifdef OLD_TCPIP_SUPPORT
	    conn->err = ERR_TCPIP_OK;
           conn->recv_avail = 0;
#else
           conn->last_err = 0;
#endif
	    conn->type = NETCONN_UDP;
	    conn->pcb.tcp = NULL;
	    conn->state = NETCONN_NONE;
	    conn->socket = udpSocketInfo[i].uSocket;
	    socket_dsc = &sockets[udpSocketInfo[i].uSocket];
	    socket_dsc->conn = conn;
	    socket_dsc->sendevent = 1;
	    socket_dsc->lastdata = NULL;
	    socket_dsc->lastoffset = 0;
	    socket_dsc->rcvevent = 0;
	    socket_dsc->err = 0;
#ifdef OLD_TCPIP_SUPPORT
	    socket_dsc->flags = 0;
	    SUL_MemSet8(&socketid[udpSocketInfo[i].uSocket], 0, sizeof(struct socket_data));
	    socketid[udpSocketInfo[i].uSocket].type = NETCONN_UDP;
#endif
	    conn->pcb.udp = udp_new();
	    if(conn->pcb.udp != NULL)
	        udp_recv(conn->pcb.udp, recv_udp, conn);
	    else
	    {
	    //assert();
	        return;
	    }
	    if(udpSocketInfo[i].socketState == UDP_SOCKET_STATE_BIND)
	    {
	        udp_bind(conn->pcb.udp, &localIp, udpSocketInfo[i].localPort);
	    }
	    else if(udpSocketInfo[i].socketState == UDP_SOCKET_STATE_CONNECT)
	    {
	        //bind
	        udp_bind(conn->pcb.udp, &localIp, udpSocketInfo[i].localPort);
		//connect
		 udp_connect(conn->pcb.udp, &remoteIp, udpSocketInfo[i].remotePort);
	    }
	    break;
      default:
	  	//assert
	   break;
   }
}


static void CswPsmRestoreApsTcpipStaticBackupInfo(T_Aps_Tcpip_Static_Context_Backup_Info  *apsTcpipStaticCtxBackupInfo, u8 i)
{
//    UINT8 i = 0;
    CFW_GPRS_PDPCONT_INFO *pdp_ctx = NULL; // PdpContList[0];
    UINT8 nCid = 1; //apsTcpipStaticCtxBackupInfo->nCid;
    UINT8 tcpipState = 0;
//    UINT32 remoteAddr = 0;
    UINT8  ipaddr[4] = {0};
    struct netif *netif = NULL;
    ip_addr_t *ip = NULL;
    ip_addr_t netmask;
    UINT8 dlci = restoreGprsCidInfo[i].nDLCI;
    nCid = apsTcpipStaticCtxBackupInfo->nCid[i];
    pdp_ctx = PdpContList[0][nCid-1];
    netif = CSW_TCPIP_MALLOC(SIZEOF(struct netif));
    memset(netif, 0, SIZEOF(struct netif));
    ip = CSW_TCPIP_MALLOC(SIZEOF(ip_addr_t));
    memset(ip, 0, SIZEOF(ip_addr_t));
    ip_addr_set_ip4_u32(&netmask, 0x00ffffff);
    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("xuzuohua:i/nCid/dlci: %d/%d/%d",0x081007a5),i, nCid,dlci);

    if(pdp_ctx != NULL)
    {
        memcpy(ipaddr, pdp_ctx->pPdpAddr, pdp_ctx->nPdpAddrSize);
	 ip_addr_set_ip4_u32(ip, (ipaddr[3]<<24|ipaddr[2]<<16|ipaddr[1]<<8|ipaddr[0]));
	    //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua: ip->addr:%d",0x081007a5), ip->addr );

    }
  
    netif_add(netif, ip_2_ip4(ip), ip_2_ip4(&netmask), NULL, NULL, NULL, NULL);
    netif->output = data_output;
    netif->lowaddr.cid = nCid; //实际上高4位是sim id，但只有一张卡，所以sim id = 0， 此处cid = T_cid
    netif->num = nCid;
    netif->mtu = 1500;
    if(NULL == netif_default)
    {
        netif_set_default(netif);
    }
    netif_set_up(netif);
	    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua: udpFlag:%d",0x081007a5), apsTcpipStaticCtxBackupInfo->isUdpSocket );

    if(apsTcpipStaticCtxBackupInfo->isUdpSocket)
    {
        udpflag = 1;
        tcpipState = apsTcpipStaticCtxBackupInfo->socketInfo[i].tcpipState;
		    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("xuzuohua: tcpipState/socketidx:%d/%d",0x081007a5), tcpipState ,apsTcpipStaticCtxBackupInfo->socketInfo[i].uSocket);
       g_uATTcpipState[dlci] = tcpipState;
	// if(tcpipState == AT_TCPIP_STATE_CONNECTING)
        {
            //save(udp bind) infomation
            g_stATTcpipParas[dlci].uType = CFW_TCPIP_SOCK_DGRAM;
            g_stATTcpipParas[dlci].uProtocol = CFW_TCPIP_IPPROTO_UDP;
	     g_stATTcpipParas[dlci].uDomain = CFW_TCPIP_AF_INET;
	    
	     g_stATTcpipParas[dlci].uPort =  ntohs(apsTcpipStaticCtxBackupInfo->socketInfo[i].remotePort); //remote port
	    CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua: remotePort:%d",0x081007a5),  g_stATTcpipParas[dlci].uPort);

            memset(g_stATTcpipParas[dlci].uaIPAddress,0x00, (AT_TCPIP_ADDRLEN + 3));
	     memcpy(g_stATTcpipParas[dlci].uaIPAddress,apsTcpipStaticCtxBackupInfo->socketInfo[i].remoteIPAddrStr,AT_TCPIP_ADDRLEN+3); //remote port
                //AT_TC_MEMBLOCK(9, g_stATTcpipParas[dlci].uaIPAddress, 20, 16);
            CSW_TC_MEMBLOCK(CFW_GPRS_TS_ID, g_stATTcpipParas[dlci].uaIPAddress, 23, 16);
		// remoteAddr = ntohl(apsTcpipStaticCtxBackupInfo->socketInfo[i].remoteIpAddr);
	     //convert ip addr
	     
	    // g_stATTcpipParas[dlci].uaIPAddress
	     g_stATTcpipParas[dlci].uSocket =  apsTcpipStaticCtxBackupInfo->socketInfo[i].uSocket; //socket idx
	     g_uATTcpipSocket[dlci] = g_stATTcpipParas[dlci].uSocket;
	     //dest_addr ignore
	     CswPsmRestoreApsUdpStaticBackupInfo(apsTcpipStaticCtxBackupInfo->socketInfo, i);
        }
	 g_uATTcpipCid[dlci] = nCid;
	 
    }
}

static void CswPsmRestoreApsPppStaticBackupInfo(T_Aps_Ppp_Static_Context_Backup_Info  *apsPppStaticCtxBackupInfo, u8 i)
{
   ppp_InitPPPVarsByIdx(i);
   ppp_Data[i].ppp_Gprs_state = PPP_ACTIVED;
   ppp_Data[i].NCP_ACKed = TRUE;
   ppp_Data[i].used = TRUE;
   ppp_Data[i].Status = PPP_STATUS_NCP_ACKED;
   ppp_Data[i].Id = apsPppStaticCtxBackupInfo->Id;
   ppp_Data[i].Cid = apsPppStaticCtxBackupInfo->Cid;
   ppp_Data[i].MagicNumber = apsPppStaticCtxBackupInfo->MagicNumber;
   ppp_Data[i].mux_id = apsPppStaticCtxBackupInfo->mux_id;
   ppp_Data[i].nSimID = 0;
 //  ncpHeader_extend = apsPppStaticCtxBackupInfo->ncpHeader_extend; //每个mux_id应该对应一个extend, bug
   //IPHeader_extend = apsPppStaticCtxBackupInfo->IPHeader_extend;
  // CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("xuzuohua_restore_ppp:idx/id/cid/magicNumber/mux_id: %d/%d/%d/%d/%d",0x081007a5),
   	//i, ppp_Data[i].Id,ppp_Data[i].Cid,ppp_Data[i].MagicNumber,ppp_Data[i].mux_id);
}

static void CswPsmRestoreApsStaticBackupInfo(T_Aps_Static_Context_Backup_Info *apsStaticCtxBackupInfo)
{
    u8 i = 0;
    Aps_APP = apsStaticCtxBackupInfo[0].aps_app;
    for(i = 0; i < g_uAtGprsCidNum; i++)
    {
        switch(Aps_APP)
        {
            case APS_TCP:
	         CswPsmRestoreApsTcpipStaticBackupInfo(&apsStaticCtxBackupInfo[i].apsStaticInfo.apsTcpipStaticCtxBackupInfo,i);
	         break;
	     case APS_PPP:
	         CswPsmRestoreApsPppStaticBackupInfo(&apsStaticCtxBackupInfo[i].apsStaticInfo.apsPppStaticCtxBackupInfo, i);
	         break;
            default: break;
        }
    }
    if(Aps_APP == APS_PPP)
    {
        g_uAtGprsCidNum = 0;  //ppp方式不需要存这个变量
    }
  
}
#endif //NBIOT_APPLICATION_PSM

static void CswPsmRestoreStaticBackupInfo(T_CSW_Static_Context_Backup_Info *backupInfo)
{
    CswPsmRestoreCfwStaticBackupInfo(&backupInfo->cfwStaticCtxBackupInfo);
    CswPsmRestoreAtStaticBackupInfo(&backupInfo->atStaticCtxBackupInfo);
#ifdef NBIOT_APPLICATION_PSM
    CswPsmRestoreApsStaticBackupInfo(backupInfo->apsStaticCtxBackupInfo);
#endif //NBIOT_APPLICATION_PSM
}

static void CswPsmRestoreCfwNwDynamicBackupInfo( T_Cfw_Nw_Dynamic_Context_Backup_Info *backupInfo)
{
    NW_SM_INFO *s_proc = NULL;  // static sm
    HAO s_hAo = 0; 
    AO_CONTENT *pAo = NULL;
//    UINT8 nServiceId = CFW_NW_SRV_ID *CFW_SIM_COUNT +0;
    if(backupInfo->saveStaticAO == TRUE)
    {
        s_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_NW_SRV_ID, 0);
	 pAo = (AO_CONTENT *)s_hAo;
	 s_proc   = CFW_GetAoUserData(s_hAo);
	 if(s_proc != NULL)
	 {
	     if(s_proc->bStaticAO != TRUE)
	     {
	         CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: donot happen at all",0x081007a5));
                CSW_NW_FREE(s_proc);
		  CFW_UnRegisterAO(CFW_NW_SRV_ID, s_hAo);
	     }
	     else
	     {
	     	     memcpy(s_proc, &backupInfo->NwSmInfo, sizeof(NW_SM_INFO));
	            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("xuzuohua: CswPsmRestoreCfwNwDynamicBackupInfo_state/psType: %d/%d",0x081007a5), s_proc->nGprsAttState,s_proc->sGprsStatusInfo.PsType);
                   if(pAo != NULL)
                   {
		         pAo->AO_State = backupInfo->AO_State;
	                pAo->UTI = backupInfo->UTI;
	                pAo->AO_NextState = 0;	
                   }
		     else
		     {
		         CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: pAo is NULL,  terrible",0x081007a5));
		     }
	     }
	 }
    }
    else
    {
           CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("xuzuohua: restore nw static ao is false",0x081007a5));
    }
}
static void CswPsmRestoreCfwGprsDynamicBackupInfo(T_Cfw_Gprs_Dynamic_Context_Backup_Info *backupInfo)
{
    UINT8 i = 0;
    UINT8 nServiceId ;
   // UINT8 nActiveAONumber;// = AO_ControlBlock[nServiceId].ActiveNum;
    HAO hAo;
    AO_CONTENT *pAo;
    GPRS_SM_INFO* pUserData;
    AO_CONTENT *head = NULL;
    nServiceId = CFW_GPRS_SRV_ID *CFW_SIM_COUNT +0;
    for(; i < 2; i++)
    {
        if(backupInfo[i].activated == TRUE)
        {
            AO_ControlBlock[nServiceId].ActiveNum++;
            pUserData = (GPRS_SM_INFO *)CSW_GPRS_MALLOC(SIZEOF(GPRS_SM_INFO));
	        if(pUserData != NULL)
	        {
	            pUserData->nActState = CFW_GPRS_ACTIVED;
		        pUserData->Cid2Index.nCid = restoreGprsCidInfo[i].uCid;
		        pUserData->Cid2Index.nNsapi = pUserData->Cid2Index.nCid + 4;
		        pUserData->Cid2Index.nSmIndex = backupInfo[i].nSmIndex;
				pUserData->Cid2Index.nLinkedEbi = pUserData->Cid2Index.nNsapi; //nbiot don't support dedicated bearer, so set linkedEbi as nsapi value
		        if(pUserData->Cid2Index.nCid > 0)
		        {
	    	         pUserData->Qos.nGuaranteedbitrateDL = pQosList[0][pUserData->Cid2Index.nCid-1].nGuaranteedbitrateDL;
		             pUserData->Qos.nGuaranteedbitrateUL = pQosList[0][pUserData->Cid2Index.nCid-1].nGuaranteedbitrateUL;
		             pUserData->Qos.nMaximumbitrateDL = pQosList[0][pUserData->Cid2Index.nCid-1].nMaximumbitrateDL;
		             pUserData->Qos.nMaximumbitrateUL = pQosList[0][pUserData->Cid2Index.nCid-1].nMaximumbitrateUL;
					 //need add tft related paramters  to be doing
		        }
		        if((PdpContList[0][pUserData->Cid2Index.nCid-1])&&(PdpContList[0][pUserData->Cid2Index.nCid-1]->pPdpAddr != NULL))
		        {
		            memcpy(pUserData->nPDPAddr, PdpContList[0][pUserData->Cid2Index.nCid-1]->pPdpAddr, 4);
		            pUserData->nPDPAddrSize = PdpContList[0][pUserData->Cid2Index.nCid-1]->nPdpAddrSize;
		        }
		        else
		        {
		            //assert for debug
		        }
	        }
	        hAo = CFW_RegisterAo(CFW_GPRS_SRV_ID, pUserData, CFW_GprsAoProc, 0);
	        pAo = (AO_CONTENT *)hAo;
	        if(pAo != NULL)
	        {
	            if(head == NULL)
	            {
	                head = pAo;
	            }
		        else
		        {
		            head->NextAO = pAo;  //最多存在两个，不做指针移动了
		        }
	            pAo->AO_State = CFW_GPRS_STATE_ACTIVE;
	            pAo->AO_NextState = 0;
	            pAo->UTI = backupInfo[i].UTI;
	        }
            CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("xuzuohua_dyn_gprs:idx/cid/smidx/pdpAddrSize/uti: %d/%d/%d/%d/%d",0x081007a5),
                  i, pUserData->Cid2Index.nCid,pUserData->Cid2Index.nSmIndex,pUserData->nPDPAddrSize,pAo->UTI);

        }
    }
    AO_ControlBlock[nServiceId].AcitveAO = head;
    AO_ControlBlock[nServiceId].DoingHead = NULL;
    AO_ControlBlock[nServiceId].DoingTail = NULL;
    AO_ControlBlock[nServiceId].ReadyAO = NULL;
    AO_ControlBlock[nServiceId].SpecialAO = NULL;
    AO_ControlBlock[nServiceId].ReadyNum = 0;
}


static void CswPsmRestoreCfwDynamicBackupInfo(T_Cfw_Dynamic_Context_Backup_Info *backupInfo)
{
    CswPsmRestoreCfwNwDynamicBackupInfo(&backupInfo->cfwNwDynamicCtxBackupInfo);
    CswPsmRestoreCfwGprsDynamicBackupInfo(backupInfo->cfwGprsDynamicCtxBackupInfo);
}

static void CswPsmRestoreDynamicBackupInfo(T_CSW_Dynamic_Context_Backup_Info *backupInfo)
{
    CswPsmRestoreCfwDynamicBackupInfo(&backupInfo->cfwDynamicCtxBackupInfo);
}

u32 simInitTime[5];
void CswPsmRestoreBackupInfo(T_CSW_Context_Backup_Info *backupInfo)
{
        //CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("xuzuohua: csw_restore_type: %d",0x081007a5), restoreType);
		CswPsmRestoreStaticBackupInfo(&backupInfo->staticBackupInfo);
		CSW_TRACE(_CSW|TLEVEL(CFW_GPRS_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("xuzuohua:%d/%d/%d/%d/%d",0x081007a5),simInitTime[0], simInitTime[4],simInitTime[1],simInitTime[2],simInitTime[3]);
	 	CswPsmRestoreDynamicBackupInfo(&backupInfo->dynamicBackupInfo);
}

#endif //

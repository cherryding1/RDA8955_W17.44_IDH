#ifdef LWIP_PPP_ON
#include "sockets.h"
#include "netif/ppp/pppos.h"
#include "at_module.h"
#include "cfw.h"
#include "at_dispatch.h"

#define AT_GPRS_DEA    CFW_GPRS_DEACTIVED
#define AT_GPRS_ACT    CFW_GPRS_ACTIVED

#define AT_GPRS_ATT    1
#define AT_GPRS_DAT    0

typedef enum
{
    PPP_DETACHED,
    PPP_ATTCHING,
    PPP_ATTACHED,
    PPP_DEACTIVED,
    PPP_ACTIVING,
    PPP_ACTIVED,
} gprs_state_t;

typedef struct __ctx_cb {
    ppp_pcb *nPppPcb;
    u8_t nDlc;
    u8_t nCid;
    u8_t nSimId;
    u8_t nUti;
    gprs_state_t gprsState;
 } ctx_cb_t;

ctx_cb_t *g_ppp_ctx[AT_MUXSOCKET_NUM];

static void ppp_attact_rsp(COS_EVENT *pEvent);

static u32_t output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx) {
  //return uart_write(UART, data, len);
  static u8_t delay_count;
  ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
  sys_arch_printf("output_cb delay_count=%d, lcp_fsm:%d, lpcp_fsm:%d, gprs status:%d\n",delay_count,pcb->lcp_fsm.state,pcb->ipcp_fsm.state,ctx_p->gprsState);
  if (ctx_p->gprsState != PPP_ACTIVED && pcb->lcp_fsm.state != PPP_FSM_OPENED && delay_count++<8) {
     return len;
  }
  if (pcb->lcp_fsm.state == PPP_FSM_OPENED || ctx_p->gprsState == PPP_ACTIVED)
    delay_count = 0;
  
  at_DataChannelWrite(data,len,ctx_p->nDlc);
  sys_arch_printf("output_cb uart_SendData nDlc=%d, ret =%d\n",ctx_p->nDlc,len);
  return len;
}

static void linkstate_cb(ppp_pcb *pcb, int err_code, void *ctx) {
    ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
    sys_arch_printf("linkstate_cb pcb=0x%x err_code=%d nDlc=%d\n",pcb,err_code,ctx_p->nDlc);
    if (err_code == 0) {
    } else/* if (err_code == PPPERR_CONNECT) */{
        CFW_PppTermInd(ctx_p->nDlc);
        ppp_free(pcb);
        CSW_TCPIP_FREE(pcb->netif);
        CSW_TCPIP_FREE(g_ppp_ctx[ctx_p->nDlc]);
        g_ppp_ctx[ctx_p->nDlc] = NULL;
    }
}

static void set_address(ctx_cb_t *ctx_p) {
    
    struct netif *g_gprsIf=NULL;
    g_gprsIf = getGprsNetIf(ctx_p->nSimId,ctx_p->nCid);
    sys_arch_printf("set_address g_gprsIf=0x%x\n",g_gprsIf);
    if (g_gprsIf != NULL) {
        ip4_addr_t addr;
        /* Set our address */
        IP4_ADDR(&addr, 192,168,0,1);
        ppp_set_ipcp_ouraddr(ctx_p->nPppPcb, &addr);
        
        /* Set peer(his) address */
        //IP4_ADDR(&addr, 192,168,0,2);
        //IP4_ADDR(&addr, 0xC0,0xC8,0x01,0x15);
        ppp_set_ipcp_hisaddr(ctx_p->nPppPcb, netif_ip4_addr(g_gprsIf));
                
        /* Set primary DNS server */
        //IP4_ADDR(&addr, 192,168,10,20);
        ppp_set_ipcp_dnsaddr(ctx_p->nPppPcb, 0, ip_2_ip4(dns_getserver(0)));
        
        /* Set secondary DNS server */
        //IP4_ADDR(&addr, 192,168,10,21);
        ppp_set_ipcp_dnsaddr(ctx_p->nPppPcb, 1, ip_2_ip4(dns_getserver(1)));
    }
}

static void ppp_notify_phase_cb(ppp_pcb *pcb, u8_t phase, void *ctx) {
    ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
    sys_arch_printf("ppp_notify_phase_cb pcb=0x%x phase=%d,gprs status:%d",pcb ,phase,ctx_p->gprsState);
    sys_arch_printf("status lcp_fsm:%d, lpcp_fsm:%d\n",pcb->lcp_fsm.state,pcb->ipcp_fsm.state);
#if PPP_IPV6_SUPPORT
    sys_arch_printf("status lcp_fsm:%d, ipv6cp_fsm:%d\n",pcb->lcp_fsm.state,pcb->ipv6cp_fsm.state);
#endif
    switch (phase) {

        case PPP_PHASE_NETWORK:
            if (pcb->lcp_fsm.state == PPP_FSM_STOPPING) {
                CFW_GprsActEX(CFW_GPRS_DEACTIVED,ctx_p->nCid, ctx_p->nDlc,ctx_p->nSimId,0,ppp_attact_rsp);
            } else if (pcb->lcp_fsm.state == PPP_FSM_OPENED) {
                if (pcb->ipcp_fsm.state == PPP_FSM_CLOSED                     
#if PPP_IPV6_SUPPORT
                    || pcb->ipv6cp_fsm.state == PPP_FSM_CLOSED
#endif
                    ) {
                    u8_t retryCount = 0;
                    struct netif *g_gprsIf= getGprsNetIf(ctx_p->nSimId,ctx_p->nCid);
                    while (g_gprsIf == NULL && retryCount++ <=20) {
                        sys_arch_printf("ppp_notify_phase_cb waiting for gprs actived:%d\n",retryCount);
                        COS_Sleep(500);
                        g_gprsIf = getGprsNetIf(ctx_p->nSimId,ctx_p->nCid);
                    }
                    sys_arch_printf("ppp_notify_phase_cb g_gprsIf=0x%x\n",g_gprsIf);
                    if (g_gprsIf != NULL) {
                        set_address(ctx_p);
                    }
                }
            }
            break;
        /* Session is down (either permanently or briefly) */
        case PPP_PHASE_DEAD:
        //d_set(PPP_LED, LED_OFF);
        break;

        /* We are between two sessions */
        case PPP_PHASE_HOLDOFF:
        //led_set(PPP_LED, LED_SLOW_BLINK);
        break;

        /* Session just started */
        case PPP_PHASE_INITIALIZE:
        //led_set(PPP_LED, LED_FAST_BLINK);
        break;

        /* Session is running */
        case PPP_PHASE_RUNNING:
        //led_set(PPP_LED, LED_ON);
        break;

        default:
        break;
    }
}

static int8_t do_active(uint8_t nDlc,uint8_t nSim,uint8_t nCid)
{
    if (nCid == 0) {
        UINT32 ret = CFW_GetFreeCID(&nCid,nSim);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "do_active, no cid allowed for TCPIP\n");
            return -1;
        }
    }
    return CFW_GprsActEX(AT_GPRS_ACT, nCid, nDlc, nSim, 0, ppp_attact_rsp);

}
static void ppp_attact_rsp(COS_EVENT *pEvent)
{
    AT_TC(g_sw_TCPIP, "ppp_attact_rsp Got %s: %d,0x%x,0x%x,0x%x",
        TS_GetEventName(pEvent->nEventId),pEvent->nEventId,pEvent->nParam1,pEvent->nParam2,pEvent->nParam3);

    CFW_EVENT CfwEv;
    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    nSim = CfwEv.nFlag;
    UINT8 nCid = pEvent->nParam1;
    UINT8 nDlc = CfwEv.nUTI;

    AT_TC(g_sw_TCPIP, "attact_rsp nCid =%d",nCid);
    if (CfwEv.nEventId == EV_CFW_GPRS_ATT_RSP)
    {
        if (CFW_GPRS_ATTACHED == CfwEv.nType)
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVING;
            if (do_active(CfwEv.nUTI, nSim, 0) != ERR_SUCCESS)
            {
                AT_TC(g_sw_TCPIP, "ppp_attact_rsp Activate PDP error while ppp");
            }
        }
    }
    else
    {
        switch (CfwEv.nEventId)
        {
        case EV_CFW_GPRS_CXT_ACTIVE_IND:
            AT_TC(g_sw_GPRS, "ppp_attact_rsp got EV_CFW_GPRS_CXT_ACTIVE_IND");
            break;;
        case EV_CFW_GPRS_CXT_DEACTIVE_IND:
            AT_TC(g_sw_GPRS, "ppp_attact_rsp got EV_CFW_GPRS_CXT_DEACTIVE_IND");
            break;
        case EV_CFW_GPRS_ACT_RSP:
            {
                AT_TC(g_sw_GPRS, "ppp_attact_rsp EV_CFW_GPRS_ACT_RSP nType: %d", CfwEv.nType);
                if (CFW_GPRS_ACTIVED == CfwEv.nType)
                {
                    if (g_ppp_ctx[nDlc] != NULL) {
                        g_ppp_ctx[nDlc]->nCid = nCid;
                        g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVED;
                        set_address(g_ppp_ctx[nDlc]);
                    }
                    else
                        CFW_GprsActEX(CFW_GPRS_DEACTIVED,nCid, nDlc,nSim,0,ppp_attact_rsp);
                    break;
                }
                else
                {
                    
                }
            }
            break;
        default :
            AT_TC(g_sw_GPRS, "ppp_attact_rsp unprocessed event:%d",pEvent->nEventId);
            break;
        }
    }
    AT_FREE(pEvent);
}

static INT8 ppp_GprsActive(UINT16 nDlc,UINT8 nSimID, UINT8 nCid)
{
    INT32 iResult    = 0;
    UINT8 nSim = AT_SIM_ID(nDlc);
    UINT8 m_uAttState = CFW_GPRS_DETACHED;
    UINT8 uPDPState  = 0;

    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS)
    {
        AT_TC(g_sw_TCPIP, "ppp_GprsActive, get GPRS attach state error\n");
        return -1;
    }
    if (AT_GPRS_DAT == m_uAttState)
    {
        g_ppp_ctx[nDlc]->gprsState = PPP_ATTCHING;
        iResult = CFW_GprsAttEX(AT_GPRS_ATT, nDlc, nSim, ppp_attact_rsp);
        if (iResult != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "ppp_GprsActive, attach failed: %d\n", iResult);
            return -1;
        }
        return 0;
    }
    else
    {
        g_ppp_ctx[nDlc]->gprsState = PPP_ATTACHED;
        if (nCid!=0 && CFW_GetGprsActState(nCid, &uPDPState, nSim) != ERR_SUCCESS)
        {
            AT_TC(g_sw_TCPIP, "ppp_GprsActive, get GPRS act state error\n");
            return -1;
        }
        if (uPDPState == CFW_GPRS_ACTIVED)
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVED;
        }
        else
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVING;
            if (do_active(nDlc, nSim, nCid) != ERR_SUCCESS)
            {
                AT_TC(g_sw_TCPIP, "ppp_GprsActive, activate failed: %d\n", iResult);
                return -1;
            }
        }
        return 0;
    }
}


INT8 startPppIf(u8_t nDlc, u8_t nSimId, u8_t nCid){
    struct netif *pppif;
    if (g_ppp_ctx[nDlc] == NULL) {
        g_ppp_ctx[nDlc] = CSW_TCPIP_MALLOC(SIZEOF(ctx_cb_t));
        memset(g_ppp_ctx[nDlc], 0, SIZEOF(ctx_cb_t));
    }
    g_ppp_ctx[nDlc]->nDlc = nDlc;
    g_ppp_ctx[nDlc]->nSimId = nSimId;
    g_ppp_ctx[nDlc]->nCid = nCid;
    if (ppp_GprsActive(nDlc,nSimId,g_ppp_ctx[nDlc]->nCid) == -1) {
        CSW_TCPIP_FREE(g_ppp_ctx[nDlc]);
        g_ppp_ctx[nDlc] = NULL;
        return -1;
    }
    pppif = CSW_TCPIP_MALLOC(SIZEOF(struct netif));
    memset(pppif, 0, SIZEOF(struct netif));
    g_ppp_ctx[nDlc]->nPppPcb = pppos_create(pppif,output_cb, linkstate_cb, (void *)g_ppp_ctx[nDlc]);
    
    ppp_set_notify_phase_callback(g_ppp_ctx[nDlc]->nPppPcb,ppp_notify_phase_cb);

#if 0
    /* Auth configuration, this is pretty self-explanatory */
    ppp_set_auth(g_ppp_ctx[nDlc]->nPppPcb, PPPAUTHTYPE_ANY, "login", "password");

    /* Require peer to authenticate */
    ppp_set_auth_required(g_ppp_ctx[nDlc]->nPppPcb, 1);
#endif

    /*
     * Only for PPPoS, the PPP session should be up and waiting for input.
     *
     * Note: for PPPoS, ppp_connect() and ppp_listen() are actually the same thing.
     * The listen call is meant for future support of PPPoE and PPPoL2TP server
     * mode, where we will need to negotiate the incoming PPPoE session or L2TP
     * session before initiating PPP itself. We need this call because there is
     * two passive modes for PPPoS, ppp_set_passive and ppp_set_silent.
     */
    ppp_set_silent(g_ppp_ctx[nDlc]->nPppPcb, 1);
    
    /*
     * Initiate PPP listener (i.e. wait for an incoming connection), can only
     * be called if PPP session is in the dead state (i.e. disconnected).
     */
    ppp_listen(g_ppp_ctx[nDlc]->nPppPcb);
    
    if (g_ppp_ctx[nDlc]->gprsState == PPP_ACTIVED)
        set_address(g_ppp_ctx[nDlc]);
    return 0;
}

struct netif *getPppNetIf(UINT8 nDlc) {
    if (g_ppp_ctx[nDlc] == NULL || g_ppp_ctx[nDlc]->nPppPcb == NULL)
        return NULL;
    return g_ppp_ctx[nDlc]->nPppPcb->netif;
}

INT32 CFW_PppSendData(UINT8 *pData, UINT16 nDataSize, UINT8 MuxId)
{
    if(NULL == g_ppp_ctx[MuxId])
    {
        return ERR_INVALID_HANDLE;
    }
    pppos_input(g_ppp_ctx[MuxId]->nPppPcb ,pData,nDataSize);
}

VOID CFW_PppTermInd(UINT8 muxid)
{
    AT_PppTerminateInd(muxid);
}

extern UINT8 isPppActived;

VOID AT_PppTerminateInd(UINT8 nDLCI)
{
    sys_arch_printf("AT_PppTerminateInd nDLCI = %d,isPppActived=%d", nDLCI,isPppActived);
    isPppActived = 0;
    at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_END, nDLCI);
}

// This function response "CONNECT" and change to data staues.
VOID AT_PppProcess(UINT8 nDLCI, UINT8 nSim, UINT8 nCid)
{
    sys_arch_printf("AT_PppProcess:%d,%d,%d,isPppActived=%d", nDLCI,nSim,nCid,isPppActived);
    
    at_ModuleModeSwitch(AT_MODE_SWITCH_DATA_START, nDLCI);
    isPppActived = 1;
    at_DataSetPPPMode(at_DispatchGetDataEngine(at_DispatchGetByChannel(nDLCI)));
    startPppIf(nDLCI,nSim,nCid);
}

#endif

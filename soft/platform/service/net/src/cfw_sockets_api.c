#include "sockets.h"
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/netdb.h"
#include "api_msg.h"


extern HANDLE g_hCosMmiTask;
extern HANDLE g_hCosATTask; // the default MMI task.
#ifndef ONLY_AT_SUPPORT
extern HANDLE g_TransportHandle;
#endif

#define MAX_SOCKET MEMP_NUM_NETCONN

typedef struct _cfw_socket_map_t {
    INT32 socket_used;
    UINT32 userParam;
    HANDLE taskHandle;
    COS_CALLBACK_FUNC_T func;
} CFW_SOCKET_MAP_T;

static CFW_SOCKET_MAP_T cfw_socket_map[MAX_SOCKET+LWIP_SOCKET_OFFSET];

static HANDLE getRequestTaskHandle() {
    HANDLE task = COS_GetCurrentTaskHandle();
#ifndef ONLY_AT_SUPPORT
    if (task == g_hCosMmiTask || task == g_TransportHandle) return g_hCosMmiTask;
#else
    if (task == g_hCosMmiTask) return g_hCosMmiTask;
#endif
    if (task == g_hCosATTask) return g_hCosATTask;
    if (g_hCosATTask != 0) return g_hCosATTask;
    return g_hCosMmiTask;
}

static VOID sys_post_event_to_APP(UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nParam3)
{
    HANDLE taskHandle = cfw_socket_map[nParam1].taskHandle;
    COS_CALLBACK_FUNC_T func = cfw_socket_map[nParam1].func;
    sys_arch_printf("sys_post_event_to_APP:%x,event:%d,nParam1:%d,nParam2:%d,nParam3:%d",taskHandle,nEventId,nParam1,nParam2,nParam3);
    if (func == NULL) {
        COS_EVENT ev;
        ev.nEventId =  nEventId;
        ev.nParam1  = nParam1;
        ev.nParam2  = nParam2;
        ev.nParam3 =  nParam3;
        COS_SendEvent(taskHandle, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    } else {
        COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
        ev->nEventId =  nEventId;
        ev->nParam1  = nParam1;
        ev->nParam2  = nParam2;
        ev->nParam3 =  nParam3;
        COS_TaskCallback(taskHandle,func,ev);
    }    
    return;
}


VOID sys_post_event_to_APS(VOID *msg, UINT32 msg_type)
{
    CFW_BalSendMessage(CFW_MBX, EV_INTER_APS_TCPIP_REQ, &msg, 4, CFW_SIM_END);
}

VOID sys_post_DNSevent_to_APP(UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, 
    UINT8 nType, UINT8 nFlag, UINT16 reserved, HANDLE hTask,COS_CALLBACK_FUNC_T func ,VOID *pParam)
{
    sys_arch_printf("sys_post_DNSevent_to_APP nEventId=%d,task=%x,func=0X%x",nEventId,hTask,func);
    if (func == NULL) {
        COS_EVENT ev;
        UINT16 n = MAKEUINT16(nFlag, nType);
        ev.nEventId =  nEventId;
        ev.nParam1  = nParam1;
        ev.nParam2  = nParam2;
        ev.nParam3 =  MAKEUINT32(n, reserved);
        COS_SendEvent(hTask, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    } else {
        COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
        ev->nEventId =  nEventId;
        ev->nParam1  = nParam1;
        ev->nParam2  = nParam2;
        ev->nParam3 =  pParam;
        COS_TaskCallback(hTask,func,ev);
    }
    return;
}

VOID sys_post_ICMPevent_to_APP(UINT32 nsocketId, UINT32 nParam1, UINT32 nParam2)
{
    UINT32 nDLC = cfw_socket_map[nsocketId].userParam;
    HANDLE taskHandle = cfw_socket_map[nsocketId].taskHandle;
    COS_CALLBACK_FUNC_T func = cfw_socket_map[nsocketId].func;
    sys_arch_printf("sys_post_ICMPevent_to_APP:%x,func:%x,nParam1:%d,nParam2:%d,nsocketId:%d",taskHandle,func,nParam1,nParam2,nsocketId);
    if (func == NULL) {
        COS_EVENT ev;
        ev.nEventId =  EV_CFW_ICMP_DATA_IND;
        ev.nParam1  = nParam1;
        ev.nParam2  = nParam2;
        ev.nParam3 =  nDLC;
        COS_SendEvent(taskHandle, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    } else {
        COS_EVENT *ev = COS_MALLOC(sizeof(COS_EVENT));
        ev->nEventId =  EV_CFW_ICMP_DATA_IND;
        ev->nParam1  = nParam1;
        ev->nParam2  = nParam2;
        ev->nParam3 =  nDLC;
        COS_TaskCallback(taskHandle,func,ev);
    }    
    return;
}


err_t lwip_tcp_event(void *arg, struct tcp_pcb *pcb,
         enum lwip_event lwip_event,
         struct pbuf *p,
         u16_t size,
         err_t err)
{
    struct netconn *conn = (struct netconn*)arg;
    
    sys_arch_printf("lwip_tcp_event conn=0x%x,lwip_event:%d,pcb:0x%x",conn,lwip_event,pcb);
    if (conn != NULL && pcb == NULL)
    {
        sys_arch_printf("lwip_tcp_event UDP event");
    }else {
        if (conn == NULL || !pcb->is_for_socket)
            return err;
    }
    
    if (lwip_event == LWIP_EVENT_SENT)
        conn->acked_size += size;
    sys_arch_printf("lwip_tcp_event sockedid=%d,cfw_socket_map[conn->socket]=%d,acked_size=%d",conn->socket,cfw_socket_map[conn->socket].socket_used,conn->acked_size);
    if (conn->socket <= 0 || cfw_socket_map[conn->socket].socket_used == 0 || cfw_socket_map[conn->socket].socket_used != (conn->socket))
        return err;
    switch (lwip_event) {
        case LWIP_EVENT_ACCEPT:
            sys_post_event_to_APP(EV_CFW_TCPIP_ACCEPT_IND, conn->socket, 0, cfw_socket_map[conn->socket].userParam);
            break;
        case LWIP_EVENT_SENT:
            sys_post_event_to_APP(EV_CFW_TCPIP_SOCKET_SEND_RSP, conn->socket, size, cfw_socket_map[conn->socket].userParam);
            break;
        case LWIP_EVENT_RECV:
            if (p == NULL)
                sys_post_event_to_APP(EV_CFW_TCPIP_CLOSE_IND, conn->socket, netconn_err(conn), cfw_socket_map[conn->socket].userParam);
            else
			{
#ifdef LTE_NBIOT_SUPPORT
                extern BOOL iperf_ongoing;
                if(iperf_ongoing == TRUE)
                {
                    sys_post_event_to_APP(EV_CFW_IPERF_REV_DATA_IND, conn->socket, p->tot_len, cfw_socket_map[conn->socket].userParam);
                }
		        else
#endif
                {
					sys_post_event_to_APP(EV_CFW_TCPIP_REV_DATA_IND, conn->socket, p->tot_len, cfw_socket_map[conn->socket].userParam);
                }
            }
            break;
        case LWIP_EVENT_CONNECTED:
            sys_post_event_to_APP(EV_CFW_TCPIP_SOCKET_CONNECT_RSP, conn->socket, 0, cfw_socket_map[conn->socket].userParam);
            break;
        case LWIP_EVENT_POLL:
            break;
        case LWIP_EVENT_ERR:
            sys_post_event_to_APP( EV_CFW_TCPIP_ERR_IND, conn->socket, err, cfw_socket_map[conn->socket].userParam);
            break;
        default :
            sys_arch_printf("lwip_tcp_event unprocessed event:%d",lwip_event);
            break;
    }
    return err;
}

VOID CFW_PostCloseRSP(struct netconn *conn)
{
    sys_arch_printf("CFW_PostCloseRSP sockedid=%d, cfw_socket_map[conn->socket]=%d",conn->socket,cfw_socket_map[conn->socket].socket_used);
    if (conn->socket <= 0 || cfw_socket_map[conn->socket].socket_used == 0 || cfw_socket_map[conn->socket].socket_used != (conn->socket))
        return;
    sys_post_event_to_APP(EV_CFW_TCPIP_SOCKET_CLOSE_RSP, conn->socket, 0, cfw_socket_map[conn->socket].userParam);
}

VOID CFW_PostConnectRSP(struct netconn *conn)
{
    sys_arch_printf("CFW_PostConnectRSP sockedid=%d, cfw_socket_map[conn->socket]=%d",conn->socket,cfw_socket_map[conn->socket].socket_used);
    if (conn->socket <= 0 || cfw_socket_map[conn->socket].socket_used == 0 || cfw_socket_map[conn->socket].socket_used != (conn->socket))
        return;
    sys_post_event_to_APP(EV_CFW_TCPIP_SOCKET_CONNECT_RSP, conn->socket, 0, cfw_socket_map[conn->socket].userParam);
}

UINT32 CFW_TcpipInetAddr(const INT8 *cp)
{
    return inet_addr(cp);
}

UINT16 CFW_TcpipAvailableBuffer(SOCKET nSocket)
{
    return lwip_availableBuffer(nSocket);
}

UINT16 CFW_TcpipGetRecvAvailable(SOCKET nSocket)
{
    int availsize=0;
    int ret = lwip_ioctl(nSocket,FIONREAD,&availsize);
    if (ret == 0)
        return availsize;
    return ret;
}

extern int errno;
UINT32 CFW_TcpipGetLastError(VOID)
{
    return errno;

}

SOCKET CFW_TcpipSocket(UINT8 nDomain, UINT8 nType, UINT8 nProtocol){
    return CFW_TcpipSocketEX(nDomain,nType,nProtocol,0,0);
}

SOCKET CFW_TcpipSocketEX(UINT8 nDomain, UINT8 nType, UINT8 nProtocol, COS_CALLBACK_FUNC_T func, UINT32 nUserParam){
    int socket = lwip_socket(nDomain, nType, nProtocol);
    if (socket >0) {
        if (cfw_socket_map[socket].socket_used != 0)
            hal_DbgAssert("socket is used!");
        cfw_socket_map[socket].socket_used = socket;
        cfw_socket_map[socket].userParam = nUserParam;
        cfw_socket_map[socket].taskHandle = getRequestTaskHandle();
        cfw_socket_map[socket].func = func;
        UINT32 NBIO = 1;
        lwip_ioctl(socket,FIONBIO,&NBIO);
    }
    return socket;
}

VOID CFW_TcpipSocketSetParam(SOCKET nSocket, COS_CALLBACK_FUNC_T func, UINT32 nUserParam){
    if (nSocket >0) {
        if (cfw_socket_map[nSocket].socket_used != nSocket)
            hal_DbgAssert("socket not used!");
        cfw_socket_map[nSocket].userParam = nUserParam;
        cfw_socket_map[nSocket].func = func;
    }
}

UINT32 CFW_TcpipSocketConnect(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen){
    int ret = lwip_connect(nSocket,pName,nNameLen);
    return (ret == 0 || errno == EINPROGRESS)? 0 : SOCKET_ERROR;
}
INT32 CFW_TcpipSocketSend(SOCKET nSocket, UINT8 *pData, UINT16 nDataSize, UINT32 nFlags){
    int ret =  lwip_send(nSocket,pData,nDataSize,nFlags);
    if (ret == -1 && (errno == EWOULDBLOCK || errno == EINPROGRESS)) ret = 0;
    return (ret == -1)? SOCKET_ERROR : ret;
}
UINT32 CFW_TcpipSocketRecv(SOCKET nSocket, UINT8 *pData, UINT16 nDataSize, UINT32 nFlags){
    return CFW_TcpipSocketRecvfrom(nSocket,pData,nDataSize,nFlags,NULL,NULL);
}

UINT32 CFW_TcpipSocketRecvfrom(SOCKET nSocket, VOID *pMem, INT32 nLen, UINT32 nFlags, CFW_TCPIP_SOCKET_ADDR *from, INT32 *fromlen){
    int ret = lwip_recvfrom(nSocket,pMem,nLen,nFlags,from,fromlen);
    if (ret == -1 && errno == EWOULDBLOCK) ret = 0;
    return (ret == -1)? SOCKET_ERROR : ret;
}

INT32 CFW_TcpipSocketClose(SOCKET nSocket){
    int ret = lwip_close(nSocket);
    if (ret == -1 && errno == EINPROGRESS) ret = 0;
    if (!ret &&( cfw_socket_map[nSocket].socket_used <= 0 || cfw_socket_map[nSocket].socket_used > MAX_SOCKET))
        hal_DbgAssert("socket not used!");
    if (!ret) {
        cfw_socket_map[nSocket].socket_used = 0;
        cfw_socket_map[nSocket].userParam = 0;
        cfw_socket_map[nSocket].taskHandle = 0;
        cfw_socket_map[nSocket].func = 0;
    }
    return ret;
}

UINT32 CFW_TcpipSocketIoctl(SOCKET nSocket, INT32 cmd, VOID *argp) {
    return lwip_ioctl(nSocket,cmd,argp);
}

UINT32 CFW_TcpipSocketShutdownOutput(SOCKET nSocket, INT32 how){
    return lwip_shutdown(nSocket,how);
}
UINT32 CFW_TcpipSocketSendto(SOCKET nSocket, VOID *pData, UINT16 nDataSize, UINT32 nFlags, CFW_TCPIP_SOCKET_ADDR *to, INT32 tolen){
    int ret = lwip_sendto(nSocket,pData,nDataSize,nFlags,to,tolen);
    if (ret == -1 && (errno == EWOULDBLOCK || errno == EINPROGRESS)) ret = 0;
    return (ret == -1)? SOCKET_ERROR : ret;
}
UINT32 CFW_TcpipSocketBind(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen){
    int ret = lwip_bind(nSocket,pName,nNameLen);
    if (ret == -1 && errno == EINPROGRESS) ret = 0;
    return (ret == -1)? SOCKET_ERROR : ret;
}

UINT32 CFW_TcpipSocketListen(SOCKET nSocket, UINT32 backlog){
    return lwip_listen(nSocket,backlog);
}
UINT32 CFW_TcpipSocketAccept(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *addr, UINT32 *addrlen){
    int socket =  lwip_accept(nSocket,addr,addrlen);
    if (socket >0) {
        if (cfw_socket_map[socket].socket_used != 0)
            hal_DbgAssert("socket is used!");
        cfw_socket_map[socket].socket_used = socket;
        cfw_socket_map[socket].userParam = cfw_socket_map[nSocket].userParam;
        cfw_socket_map[socket].taskHandle = cfw_socket_map[nSocket].taskHandle;
        cfw_socket_map[socket].func = cfw_socket_map[nSocket].func;
    }
    return socket;
}
UINT32 CFW_TcpipSocketGetsockname(SOCKET nSocket, CFW_TCPIP_SOCKET_ADDR *pName, INT32 *pNameLen){
    return lwip_getsockname(nSocket,pName,pNameLen);
}
UINT32 CFW_TcpipSocketGetpeername(SOCKET nSocket, CFW_TCPIP_SOCKET_ADDR *pName, INT32 *pNameLen){
    return lwip_getpeername(nSocket,pName,pNameLen);
}

// this method will blocked until dns sucess/fail when getRequestTaskHandle return NULL,
// this method will use system dns_servers when cid or sim is not valid
UINT32 CFW_Gethostbyname(char *hostname, ip_addr_t *addr, UINT8 nCid, CFW_SIM_ID nSimID){
    int ret = netconn_gethostbyname_ext(hostname,addr,nSimID << 8 | nCid,getRequestTaskHandle(),0,0);
    if (ret == ERR_OK)
        return RESOLV_COMPLETE;
    else if (ret == ERR_INPROGRESS)
        return RESOLV_QUERY_QUEUED;
    else
        return RESOLV_QUERY_INVALID;
}

UINT32 CFW_GethostbynameEX(char *hostname, ip_addr_t *addr, UINT8 nCid, CFW_SIM_ID nSimID, 
        COS_CALLBACK_FUNC_T func, VOID *param){
    int ret = netconn_gethostbyname_ext(hostname,addr,nSimID << 8 | nCid,getRequestTaskHandle(),func,param);
    if (ret == ERR_OK)
        return RESOLV_COMPLETE;
    else if (ret == ERR_INPROGRESS)
        return RESOLV_QUERY_QUEUED;
    else
        return RESOLV_QUERY_INVALID;
}


UINT32 CFW_TcpipSocketConnectEx(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen
                                , CFW_SIM_ID nSimID
                               ) {
    int ret = lwip_connect(nSocket,pName,nNameLen);
    return (ret == 0 || errno == EINPROGRESS)? 0 : SOCKET_ERROR;
}

UINT32 CFW_TcpipSocketGetsockopt(SOCKET nSocket, INT32 level, INT32 optname, VOID *optval, INT32 *optlen) {
    return lwip_getsockopt(nSocket,level,optname,optval,optlen);
}

UINT32 CFW_TcpipSocketSetsockopt(SOCKET nSocket, INT32 level, INT32 optname, const VOID *optval, INT32 optlen) {
    return lwip_setsockopt(nSocket,level,optname,optval,optlen);
}

UINT32 CFW_TcpipSocketGetAckedSize(SOCKET nSocket) {
    return lwip_getAckedSize(nSocket);
}

UINT32 CFW_TcpipSocketGetSentSize(SOCKET nSocket) {
    return lwip_getSentSize(nSocket);
}


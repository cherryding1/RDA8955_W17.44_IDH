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
#ifndef __LWIP_SOCKETS_H__
#define __LWIP_SOCKETS_H__
#include <cswtype.h>
#include "cfw.h"
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/errno.h"
#include "lwip/pbuf.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "lwip/ip4.h"
#include "lwip/icmp.h"
#include "lwip/etharp.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/opt.h"
#include "lwip/etharp.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/ethip6.h"

extern struct netif *netif_default;

#define CFW_TCPIP_SOCK_STREAM     SOCK_STREAM
#define CFW_TCPIP_SOCK_DGRAM      SOCK_DGRAM
#define CFW_TCPIP_SOCK_RAW        SOCK_RAW

#define CFW_TCPIP_AF_UNSPEC       AF_UNSPEC
#define CFW_TCPIP_AF_INET         AF_INET
#define CFW_TCPIP_PF_INET         PF_INET
#define CFW_TCPIP_PF_UNSPEC       PF_UNSPEC

#define CFW_TCPIP_IPPROTO_IP      IPPROTO_IP
#define CFW_TCPIP_IPPROTO_ICMP    IPPROTO_ICMP
#define CFW_TCPIP_IPPROTO_TCP     IPPROTO_TCP
#define CFW_TCPIP_IPPROTO_UDP     IPPROTO_UDP

#define ERR_TCPIP_ENODATA ENODATA
#define ERR_TCPIP_EWOULDBLOCK EWOULDBLOCK
#define ERR_TCPIP_IF    ERR_IF
#define ERR_TCPIP_MEM   ERR_MEM
#define ERR_TCPIP_OK    ERR_OK

#define CFW_ERR_TCPIP_CONN_TIMEOUT 13        /* connect time out       */
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR               -1

#ifndef GPRS_MTU
#define GPRS_MTU                      1500
#endif


typedef struct in_addr in_addr;
typedef struct in6_addr in6_addr;

#define IPH_VHLTOS_SET(hdr, v, hl, tos) do { IPH_VHL_SET(hdr, v, hl); IPH_TOS_SET(hdr, tos); } while (0)

typedef struct _CFW_TCPIP_SOCKET_ADDR
{
    UINT8 sin_len;
    UINT8 sin_family;
    UINT16 sin_port;
    in_addr sin_addr;
    INT8 sin_zero[8];
} CFW_TCPIP_SOCKET_ADDR;


typedef struct _CFW_TCPIP_SOCKET_ADDR6
{
    UINT8 sin6_len;
    UINT8 sin6_family;
    UINT16 sin6_port;
    UINT32 sin6_flowinfo;
    in6_addr  sin6_addr;
    UINT32  sin6_scope_id;
} CFW_TCPIP_SOCKET_ADDR6;


typedef struct _CFW_WIFI_DATA
{
    UINT16 nDataLength;
    UINT8 srcMac[6];
    UINT8 destMac[6];
    UINT8 *pData;
} CFW_WIFI_DATA;

typedef struct _CFW_WIFI_NETIF
{
    UINT8 Mac[6];
    //  UINT8 gwMac[6];
    ip_addr_t ipaddr;
} CFW_WIFI_NETIF;

typedef struct
{
    UINT8 ref_count;
    UINT16 msg_len;
    BOOL use_dhcp;
    UINT8 ip_addr[4];
    UINT8 gateway[4];
    UINT8 netmask[4];
    UINT8 pri_dns_addr[4];
    UINT8 sec_dns_addr[4];
} ipaddr_change_req_struct;


typedef struct
{
    UINT8 ref_count;
    UINT16 msg_len;
    UINT8 cause;
    UINT8 ip_addr[4];
    UINT8 pri_dns_addr[4];
    UINT8 sec_dns_addr[4];
    UINT8 gateway[4];
    UINT8 netmask[4];
} ipaddr_update_ind_struct;

VOID CFW_SetPppSendFun(BOOL (* sendCallBack) (UINT8 *pData, UINT16 uDataSize, UINT8 nDLCI));
VOID CFW_SetGetSimCidFun(VOID (*get_simid_cid)(UINT8 *pSimId, UINT8 *pCid, UINT8 nDLCI));

UINT32 CFW_TcpipInetAddr(const INT8 *cp);
UINT16 CFW_TcpipAvailableBuffer(SOCKET nSocket);
UINT32 CFW_TcpipGetLastError(VOID);
SOCKET CFW_TcpipSocket(UINT8 nDomain, UINT8 nType, UINT8 nProtocol);
SOCKET CFW_TcpipSocketEX(UINT8 nDomain, UINT8 nType, UINT8 nProtocol, COS_CALLBACK_FUNC_T func, UINT32 nUserParam);
VOID CFW_TcpipSocketSetParam(SOCKET nSocket, COS_CALLBACK_FUNC_T func, UINT32 nUserParam);
UINT32 CFW_TcpipSocketConnect(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen);
INT32 CFW_TcpipSocketSend(SOCKET nSocket, UINT8 *pData, UINT16 nDataSize, UINT32 nFlags);
UINT32 CFW_TcpipSocketRecv(SOCKET nSocket, UINT8 *pData, UINT16 nDataSize, UINT32 nFlags);
INT32 CFW_TcpipSocketClose(SOCKET nSocket);
UINT32 CFW_TcpipSocketShutdownOutput(SOCKET nSocket, INT32 how);
UINT32 CFW_TcpipSocketSendto(SOCKET nSocket, VOID *pData, UINT16 nDataSize, UINT32 nFlags, CFW_TCPIP_SOCKET_ADDR *to, INT32 tolen);
UINT32 CFW_TcpipSocketRecvfrom(SOCKET nSocket, VOID *pMem, INT32 nLen, UINT32 nFlags, CFW_TCPIP_SOCKET_ADDR *from, INT32 *fromlen);
UINT32 CFW_TcpipSocketBind(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen);
UINT32 CFW_TcpipSocketListen(SOCKET nSocket, UINT32 backlog);
UINT32 CFW_TcpipSocketAccept(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *addr, UINT32 *addrlen);
UINT32 CFW_TcpipSocketGetsockname(SOCKET nSocket, CFW_TCPIP_SOCKET_ADDR *pName, INT32 *pNameLen);
UINT32 CFW_TcpipSocketGetpeername(SOCKET nSocket, CFW_TCPIP_SOCKET_ADDR *pName, INT32 *pNameLen); //
UINT32 CFW_Gethostbyname(char *hostname, ip_addr_t *addr, UINT8 nCid, CFW_SIM_ID nSimID);

UINT32 CFW_TcpipSocketConnectEx(SOCKET nSocket,  CFW_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen
                                , CFW_SIM_ID nSimID
                               );
#endif /* __LWIP_SOCKETS_H__ */

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

#include "uctls_m.h"
#include "urndis_callback.h"

typedef HAL_USB_CALLBACK_EP_TYPE_T MsgBody_t;
#include "itf_msg.h"
#include "hal_usb.h"

#include "sxr_ops.h"
#include "sxs_io.h"
#include "urndis_m.h"
#include "urndis_data_m.h"
#include "urndisp_debug.h"
#include "sockets.h"
#include "netif/ethernet.h"
#include "lwip/udp.h"
#include "lwip/prot/ip4.h"
#include "lwip/prot/tcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/dns.h"
#include "lwip/prot/etharp.h"
#include "lwip/inet_chksum.h"
#include "string.h"
#include "tetheringappprot.h"

PRIVATE UINT8                        g_urndisBoInEp;
PRIVATE UINT8                        g_urndisBoOutEp;

PRIVATE sxr_TaskDesc_t               g_urndisTask;
PRIVATE UINT8                        g_urndisIdTask = 0;
UINT8                                g_urndisMbx;
PRIVATE UINT8                        g_urndisSendSemaphore;

extern PUBLIC UINT8 USB_UCBSS_INTERNAL      g_UsbBulkOut[4096 *2];
extern PUBLIC UINT8 USB_UCBSS_INTERNAL      g_UsbBulkIn [4096 *2];
extern USB_TETHERING_CTX s_tethering_ctx;

extern const u8 host_macaddr[];
PRIVATE const u8 device_macaddr[] = {0x92, 0x66, 0x34, 0x60, 0x36, 0x6C};
//for temp test,real ip addr should get from pdp addr
PRIVATE const ip4_addr_t dhcpclient_host_addr = { 0xC0A82AD7UL };
PRIVATE const ip4_addr_t dhcpclient_host_addr_n = { 0xD72AA8C0UL };
PRIVATE const ip4_addr_t dhcpserver_dns_router_addr = { 0xC0A82A81UL };
PRIVATE const ip4_addr_t dhcpserver_dns_router_addr_n = { 0x812AA8C0UL };

PRIVATE UINT8 *e_payload_rcv;
PRIVATE UINT8 *e_payload_send;
PRIVATE UINT8 *ip_data_send;

PRIVATE HAL_USB_CALLBACK_RETURN_T
rndisDataCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
                     HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_CMD:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis Data Callback Cmd,request=%x,value=%x,index=%d,length=%d\n",setup->request,setup->value,setup->index,setup->lenght);
            break;
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis Data Callback Data Cmd,lenght=%d\n",setup->lenght);
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis Data Callback Receive end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis Data Callback Transmit end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis Data Callback Enable\n");
            hal_UsbRecv(g_urndisBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis Data Callback Disable\n");
            break;
    }

    return(HAL_USB_CALLBACK_RETURN_KO);
}

PRIVATE HAL_USB_CALLBACK_RETURN_T
rndisCallbackEp(HAL_USB_CALLBACK_EP_TYPE_T   type,
                 HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis EP Callback Cmd data\n");
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
        {
            //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Data Receive end\n");
            Msg_t * MsgPtr ;
            UINT16     Len    = sizeof( Msg_t );
            MsgPtr = (Msg_t*) sxr_Malloc(Len);
            MsgPtr->B = URNDIS_MSG_RCV_DATA;
            if(g_urndisMbx != 0xff)
            {
                sxr_Send(MsgPtr, g_urndisMbx, SXR_SEND_MSG);
            }
            break;
        }
        case HAL_USB_CALLBACK_TYPE_CMD:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis EP Callback Cmd\n");
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Data Transmit end\n");
            sxr_ReleaseSemaphore(g_urndisSendSemaphore);
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis EP Callback Enable\n");
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Usb Rndis EP Callback Disable\n");
            break;
    }

    return(HAL_USB_CALLBACK_RETURN_OK);
}

PRIVATE INT32 rndis_ip_packet_send(UINT8* buf, UINT16 buflen)
{
    URNDIS_TRACE(URNDIS_DATA_TRC, 0, "rndis_ip_packet_send\n");
    CFW_GPRS_DATA *pGprsData;
    UINT32 rVal = 0;

	if (s_tethering_ctx.cid == -1 || s_tethering_ctx.simid == -1) {
        URNDIS_TRACE(URNDIS_INFO_TRC, 0, "rndis_ip_packet_send network not actived\n");
        return -1;
	}

    pGprsData = (CFW_GPRS_DATA *)sxr_Malloc(sizeof(CFW_GPRS_DATA) + buflen);
    if (pGprsData == NULL)
    {
        URNDIS_TRACE(URNDIS_INFO_TRC, 0, "rndis_ip_packet_send malloc fail\n");
        return -1;
    }
    memcpy(pGprsData->pData, buf, buflen);
    pGprsData->nDataLength = buflen;

#ifdef __ADP_MULTI_SIM__
    rVal = CFW_GprsSendData(s_tethering_ctx.cid, pGprsData, s_tethering_ctx.simid);
#else
    rVal = CFW_GprsSendData(s_tethering_ctx.cid, pGprsData);
#endif
    sxr_Free(pGprsData);
    return rVal;
}

PRIVATE void rndis_eth_packet_send(UINT8 *eth_payload_data, UINT16 eth_payload_len, UINT16 eth_type)
{
    sxr_TakeSemaphore(g_urndisSendSemaphore);
    memcpy(g_UsbBulkIn + sizeof(struct rndis_packet_msg_type) + sizeof(struct eth_hdr), eth_payload_data, eth_payload_len);

    struct eth_hdr *e_hdr_send = (struct eth_hdr *)(g_UsbBulkIn + sizeof(struct rndis_packet_msg_type));
    memcpy(e_hdr_send->dest.addr, host_macaddr, ETH_ALEN);
    memcpy(e_hdr_send->src.addr, device_macaddr, ETH_ALEN);
    e_hdr_send->type = eth_type;

    struct rndis_packet_msg_type *hdr_send = (struct rndis_packet_msg_type *)g_UsbBulkIn;
    memset(hdr_send, 0, sizeof(struct rndis_packet_msg_type));
    hdr_send->MessageType = cpu_to_le32(RNDIS_MSG_PACKET);
    hdr_send->MessageLength = cpu_to_le32(eth_payload_len + sizeof(struct rndis_packet_msg_type) + sizeof(struct eth_hdr));
    hdr_send->DataOffset = cpu_to_le32(36);
    hdr_send->DataLength = cpu_to_le32(eth_payload_len + sizeof(struct eth_hdr));

    URNDIS_TRACE(URNDIS_INFO_TRC, 0, "Usb Rndis tx MessageLength=%d\n",hdr_send->MessageLength);
    UINT32 csStatus = hal_SysEnterCriticalSection();
    hal_UsbSend(g_urndisBoInEp, g_UsbBulkIn, hdr_send->MessageLength, 1);
    hal_SysExitCriticalSection(csStatus);
}


PRIVATE INT32 process_dhcp_packet(UINT8* inbuf, UINT8* outbuf)
{
    UINT16 i;
    struct dhcp_msg *request_msg = (struct dhcp_msg *)inbuf;
    struct dhcp_msg *reply_msg = (struct dhcp_msg *)outbuf;
    struct ip_hdr *iphdr_in = (struct ip_hdr *)(inbuf - sizeof(struct udp_hdr) - sizeof(struct ip_hdr));

    //memset(reply_msg, 0, sizeof(struct dhcp_msg));
    reply_msg->op = DHCP_BOOTREPLY;
    /* TODO: make link layer independent */
    reply_msg->htype = DHCP_HTYPE_ETH;
    /* TODO: make link layer independent */
    reply_msg->hlen = DHCP_HLEN_ETH;
    reply_msg->hops = 0;
    reply_msg->xid = request_msg->xid;
    reply_msg->secs = 0;
    reply_msg->flags = 0;
    reply_msg->yiaddr.addr = htonl(dhcpclient_host_addr.addr);
    reply_msg->siaddr.addr = htonl(dhcpserver_dns_router_addr.addr);
    for (i = 0; i < DHCP_CHADDR_LEN; i++)
    {
        reply_msg->chaddr[i] = request_msg->chaddr[i];
    }
    //for (i = 0; i < DHCP_SNAME_LEN; i++) reply_msg->sname[i] = 0;
    //for (i = 0; i < DHCP_FILE_LEN; i++) reply_msg->file[i] = 0;
    reply_msg->cookie = htonl(0x63825363UL);

    /* Alter the incoming IP packet header so that the corrected IP source and destinations are used - this means that
    when the response IP header is generated, it will use the corrected addresses and not the null/broatcast addresses */
    iphdr_in->src.addr= htonl(dhcpclient_host_addr.addr);
    iphdr_in->dest.addr= htonl(dhcpserver_dns_router_addr.addr);

    UINT8 *DHCPOptionsINStart = request_msg->options;
    UINT8 *DHCPOptionsOUTStart = reply_msg->options;

    /* Process the incoming DHCP packet options */
    //while (DHCPOptionsINStart[0] != DHCP_OPTION_END)
    //{
        UINT16 DHCPOptionsOUTLen = 0;
        /* Find the Message Type DHCP option, to determine the type of DHCP packet */
        if (DHCPOptionsINStart[0] == DHCP_OPTION_MESSAGE_TYPE)
        {
            if ((DHCPOptionsINStart[2] == DHCP_DISCOVER) || (DHCPOptionsINStart[2] == DHCP_REQUEST))
            {
                /* Fill out the response DHCP packet options for a DHCP OFFER or ACK response */

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_MESSAGE_TYPE;
                *(DHCPOptionsOUTStart++) = 1;
                *(DHCPOptionsOUTStart++) = (DHCPOptionsINStart[2] == DHCP_DISCOVER) ? DHCP_OFFER: DHCP_ACK;
                DHCPOptionsOUTLen += 3;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_SERVER_ID;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;//dhcpserver_dns_router_addr
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0x81;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_SUBNET_MASK;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0x00;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_BROADCAST;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0xFF;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_ROUTER;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;//dhcpserver_dns_router_addr
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0x81;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_DNS_SERVER;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;//dhcpserver_dns_router_addr
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0x81;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_LEASE_TIME;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0xE0;
                *(DHCPOptionsOUTStart++) = 0x10;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_T1;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x07;
                *(DHCPOptionsOUTStart++) = 0x08;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_T2;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x0C;
                *(DHCPOptionsOUTStart++) = 0x4E;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_END;
                DHCPOptionsOUTLen += 1;
            }
            else if (DHCPOptionsINStart[2] == DHCP_INFORM)
            {
                /* Fill out the response DHCP packet options for a DHCP ACK response */

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_MESSAGE_TYPE;
                *(DHCPOptionsOUTStart++) = 1;
                *(DHCPOptionsOUTStart++) = DHCP_ACK;
                DHCPOptionsOUTLen += 3;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_SERVER_ID;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;//dhcpserver_dns_router_addr
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0x81;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_SUBNET_MASK;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0x00;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_BROADCAST;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0xFF;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_ROUTER;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;//dhcpserver_dns_router_addr
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0x81;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_DNS_SERVER;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0xC0;//dhcpserver_dns_router_addr
                *(DHCPOptionsOUTStart++) = 0xA8;
                *(DHCPOptionsOUTStart++) = 0x2A;
                *(DHCPOptionsOUTStart++) = 0x81;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_LEASE_TIME;
                *(DHCPOptionsOUTStart++) = 4;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0xE0;
                *(DHCPOptionsOUTStart++) = 0x10;
                DHCPOptionsOUTLen += 6;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_END;
                DHCPOptionsOUTLen += 1;
            }
            return (sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN + DHCPOptionsOUTLen);
        }

        /* Go to the next DHCP option - skip one byte if option is a padding byte, else skip the complete option's size */
        //DHCPOptionsINStart += ((DHCPOptionsINStart[0] == DHCP_OPTION_PAD) ? 1 : (DHCPOptionsINStart[1] + 2));
    //}
    return 0;
}

//#define USE_LWIP_GETHOSTBYNAME
#ifdef USE_LWIP_GETHOSTBYNAME
#define SIZEOF_DNS_QUERY 4
#define SIZEOF_DNS_ANS_NAME 2
#define DNS_MAX_TTL               604800
/** DNS answer message structure.
    No packing needed: only used locally on the stack. */
struct dns_answer {
  /* DNS answer record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
  u16_t type;
  u16_t cls;
  u32_t ttl;
  u16_t len;
};

PRIVATE UINT16 process_dns_tx_packet(UINT8* inbuf, UINT8* outbuf, const ip_addr_t *host_address)
{
    struct udp_hdr* udphdr_in = (struct udp_hdr *) inbuf;
    UINT8* dns_in = inbuf + sizeof(struct udp_hdr);
    UINT8* dns_send = outbuf + sizeof(struct udp_hdr);
    struct dns_hdr *dns_hdr_in = (struct dns_hdr *)dns_in;
    struct dns_hdr *dns_hdr_send = (struct dns_hdr *)dns_send;
    UINT8* query_in = dns_in + sizeof(struct dns_hdr);
    UINT16 query_in_len = ntohs(udphdr_in->len) - sizeof(struct udp_hdr) - SIZEOF_DNS_HDR;
    UINT8* query_send = dns_send + sizeof(struct dns_hdr);
    UINT16* answer_s_send = (UINT16*)(query_send + query_in_len);
    struct dns_answer* answer_send = (struct dns_answer*)(answer_s_send + 1);
    ip4_addr_t* addr = (ip4_addr_t*)(answer_send + 1);
    dns_hdr_send->id = dns_hdr_in->id;
    dns_hdr_send->flags1 = DNS_FLAG1_RESPONSE | DNS_FLAG1_RD;
    dns_hdr_send->flags2 = DNS_FLAG2_RA | DNS_FLAG2_ERR_NONE;
    dns_hdr_send->numquestions = dns_hdr_in->numquestions;
    dns_hdr_send->numanswers = PP_HTONS(1);
    memcpy(query_send, query_in, query_in_len);
    *answer_s_send = PP_HTONS(0xC00C);
    answer_send->type = PP_HTONS(DNS_RRTYPE_A);
    answer_send->cls = PP_HTONS(DNS_RRCLASS_IN);
    answer_send->ttl = PP_HTONS(DNS_MAX_TTL);
    answer_send->len = PP_HTONS(sizeof(ip4_addr_t));
    ip4_addr_set_u32(addr, ip_addr_get_ip4_u32(host_address));
    return SIZEOF_DNS_HDR + query_in_len + SIZEOF_DNS_ANS_NAME + sizeof(struct dns_answer) + answer_send->len;
}

PRIVATE void host_address_found(const char* hostname, const ip_addr_t *ipaddr, void *arg)
{
    URNDIS_TRACE(URNDIS_DATA_TRC, 0, "host_address_found hostname=%s get addr\n",hostname);
    UINT8* dns_ip_packet_in = (UINT8*)arg;
    UINT8* dns_ip_packet_send = sxr_Malloc(MAX_MTU);
    struct ip_hdr *iphdr = (struct ip_hdr *)dns_ip_packet_in;
    struct ip_hdr* iphdr_send = (struct ip_hdr *)dns_ip_packet_send;
    UINT16 iphdrlen;
    UINT16 dns_ip_packet_send_len;
    /* obtain IP header length in number of 32-bit words */
    iphdrlen = IPH_HL(iphdr);
    /* calculate IP header length in bytes */
    iphdrlen *= 4;
    dns_ip_packet_send_len = process_dns_tx_packet(dns_ip_packet_in + iphdrlen, dns_ip_packet_send + iphdrlen, ipaddr)
                             + iphdrlen + sizeof(struct udp_hdr);
    /* Fill out the response IP packet header */
    IPH_LEN_SET(iphdr_send, dns_ip_packet_send_len);
    IPH_VHLTOS_SET(iphdr_send, 4, IP_HLEN / 4, 0);
    IPH_OFFSET_SET(iphdr_send, htons(IP_DF));
	IPH_ID_SET(iphdr_send, 0);
    IPH_PROTO_SET(iphdr_send, IPH_PROTO(iphdr));
    IPH_TTL_SET(iphdr_send, 128);//set to default ttl;TODO
    ip4_addr_set(&(iphdr_send->src), &(iphdr->dest));
    ip4_addr_set(&(iphdr_send->dest), &(iphdr->src));
    IPH_CHKSUM_SET(iphdr_send, 0);
#if CHECKSUM_GEN_IP
    IPH_CHKSUM_SET(iphdr_send, inet_chksum(iphdr_send, IP_HLEN));
#endif
    rndis_eth_packet_send(dns_ip_packet_send, dns_ip_packet_send_len, htons(ETHTYPE_IP));
    sxr_Free(dns_ip_packet_in);
    sxr_Free(dns_ip_packet_send);
}
#endif

PRIVATE INT32 process_dns_packet(UINT8* inbuf, UINT8* outbuf)
{
    struct ip_hdr *iphdr_in = (struct ip_hdr *)(inbuf - sizeof(struct ip_hdr));
    if (ip4_addr_cmp(&iphdr_in->src, &dhcpclient_host_addr_n) &&
        ip4_addr_cmp(&iphdr_in->dest, &dhcpserver_dns_router_addr_n))
    {
        struct udp_hdr* udphdr_in = (struct udp_hdr *) inbuf;
#ifdef USE_LWIP_GETHOSTBYNAME
        UINT8* query = inbuf + sizeof(struct udp_hdr) + SIZEOF_DNS_HDR;
        UINT16 hostname_len = ntohs(udphdr_in->len) - sizeof(struct udp_hdr) - SIZEOF_DNS_HDR - SIZEOF_DNS_QUERY - 1;
        UINT8 hostname[hostname_len];
        UINT8* hostname_tmp = hostname;
        UINT8* hostname_part = query + 1;
        UINT8 copylen = *query;
        while (copylen != 0)
        {
            memcpy(hostname_tmp, hostname_part, copylen);
            hostname_tmp += copylen;
            hostname_part += copylen;
            copylen = *hostname_part;
            if (copylen != 0)
            {
                *hostname_tmp = '.';
                hostname_tmp++;
                hostname_part++;
            }
            else
            {
                *hostname_tmp = 0;
            }
        }
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "process_dns_packet hostname=%s\n",hostname);
        err_t err;
        ip_addr_t host_address;
        UINT16 ip_packet_len = sizeof(struct ip_hdr) + ntohs(udphdr_in->len);
        UINT8* ip_packet = sxr_Malloc(ip_packet_len);
        memcpy(ip_packet, iphdr_in, ip_packet_len);
        err = dns_gethostbyname(hostname, &host_address,
          host_address_found, ip_packet);
        if (err == ERR_INPROGRESS) {
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "dns query is sent\n");
        } else if (err == ERR_OK) {
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "process_dns_packet host address is get\n");
            sxr_Free(ip_packet);
            return process_dns_tx_packet(inbuf, outbuf, &host_address);
        }
#else
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "process_dns_packet ip match\n");
        ip4_addr_set(&iphdr_in->src, &s_tethering_ctx.ipaddr);
        ip4_addr_set(&iphdr_in->dest, ip_2_ip4(dns_getserver(0)));
        IPH_CHKSUM_SET(iphdr_in, 0);
        IPH_CHKSUM_SET(iphdr_in, inet_chksum(iphdr_in, IP_HLEN));

        //struct udp_hdr* udphdr_in = (struct udp_hdr *) inbuf;
        //test
        //ignore not (A and IN)
        UINT8 dns_type_class[] = {0, 1, 0, 1};
        UINT8 *endquery = inbuf + ntohs(udphdr_in->len) - 4;
        if (memcmp(endquery, dns_type_class, 4) != 0)
        {
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "ignore not (A and IN)\n");
            return -1;
        }

        UINT8 *name = inbuf + sizeof(struct udp_hdr) + SIZEOF_DNS_HDR + 1;
        UINT8 isatap[] = {0x69,0x73,0x61,0x74,0x61,0x70};
        if (memcmp(name, isatap, 6) == 0)
        {
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "ignore isatap\n");
            return -1;
        }
        UINT8 rdamicro[] = {0x72,0x64,0x61,0x6d,0x69,0x63,0x72,0x6f};
        if (memcmp(name, rdamicro, 8) == 0)
        {
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "ignore rdamicro\n");
            return -1;
        }

        udphdr_in->chksum = 0;//zero means no checksum;TODO gen checksum when CHECKSUM_GEN_UDP
#if CHECKSUM_GEN_UDP
#endif
        return 0;
#endif
    }
    return -1;
}

PRIVATE INT32 process_udp_packet(UINT8* inbuf, UINT8* outbuf)
{
    struct ip_hdr *iphdr_in = (struct ip_hdr *)(inbuf - sizeof(struct ip_hdr));
    struct udp_hdr* udphdr_in = (struct udp_hdr *) inbuf;
    INT32 ret_len = 0;
    if (ntohs(udphdr_in->dest) == DHCP_SERVER_PORT)
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP to DHCP server\n");
        ret_len = process_dhcp_packet(inbuf + sizeof(struct udp_hdr), outbuf + sizeof(struct udp_hdr));
    }
    else if(ntohs(udphdr_in->dest) == DNS_SERVER_PORT)
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP to DNS server\n");
        ret_len = process_dns_packet(inbuf, outbuf);
    }
    else if(ntohs(udphdr_in->dest) == 0x14EB)
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP to port 0x14EB,LLMNR ingore?\n");
        return -1;
    }
    else if(ntohs(udphdr_in->dest) == 0x0089)
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP to port 0x0089,NBNS ingore?\n");
        return -1;
    }
    else
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP to port %d,alternate src ip then send\n",ntohs(udphdr_in->dest));
        ip4_addr_set(&iphdr_in->src, &s_tethering_ctx.ipaddr);
        IPH_CHKSUM_SET(iphdr_in, 0);
        IPH_CHKSUM_SET(iphdr_in, inet_chksum(iphdr_in, IP_HLEN));

        udphdr_in->chksum = 0;//zero means no checksum;TODO gen checksum when CHECKSUM_GEN_UDP
#if CHECKSUM_GEN_UDP
#endif
        return 0;
    }

    if(ret_len > 0)
    {
        struct udp_hdr* udphdr_send = (struct udp_hdr *)outbuf;
        udphdr_send->src = udphdr_in->dest;
        udphdr_send->dest = udphdr_in->src;
        udphdr_send->chksum = 0;//zero means no checksum;TODO gen checksum when CHECKSUM_GEN_UDP
#if CHECKSUM_GEN_UDP
#endif
        udphdr_send->len = htons(sizeof(struct udp_hdr) + ret_len);
        return (sizeof(struct udp_hdr) + ret_len);
    }
    return ret_len;
}

PRIVATE UINT16 tcp_checksum(struct tcp_hdr *inbuf, ip4_addr_p_t src, ip4_addr_p_t dest, UINT16 inbuflen)
{
    UINT32 Checksum = 0;
    UINT16 CurrWord;

    Checksum += ((UINT16 *)&src.addr)[0];
    Checksum += ((UINT16 *)&src.addr)[1];
    Checksum += ((UINT16 *)&dest.addr)[0];
    Checksum += ((UINT16 *)&dest.addr)[1];
    Checksum += htons(IP_PROTO_TCP);
    Checksum += htons(inbuflen);

    for (CurrWord = 0; CurrWord < (inbuflen >> 1); CurrWord++)
        Checksum += ((UINT16 *)inbuf)[CurrWord];

    if (inbuflen & 0x01)
        Checksum += (((UINT16 *)inbuf)[inbuflen >> 1] & 0x00FF);

    while (Checksum & 0xFFFF0000)
        Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));

    return ~Checksum;
}

PRIVATE INT32 process_tcp_packet(UINT8* inbuf, UINT8* inbuf_tcp, UINT16 inbuf_tcp_len)
{
    struct ip_hdr *iphdr_in = (struct ip_hdr *)inbuf;
    struct tcp_hdr *tcphdr_in = (struct tcp_hdr *)inbuf_tcp;
    if (ip4_addr_cmp(&iphdr_in->src, &dhcpclient_host_addr_n))
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "process_tcp_packet ip match\n");
        ip4_addr_set(&iphdr_in->src, &s_tethering_ctx.ipaddr);
        IPH_CHKSUM_SET(iphdr_in, 0);
        IPH_CHKSUM_SET(iphdr_in, inet_chksum(iphdr_in, IP_HLEN));

        tcphdr_in->chksum = 0;
        tcphdr_in->chksum = tcp_checksum(tcphdr_in, iphdr_in->src, iphdr_in->dest, inbuf_tcp_len);
        return 0;
    }
    return -1;
}

PRIVATE INT32 process_arp_packet(UINT8* inbuf, UINT8* outbuf)
{
    struct etharp_hdr *arpmsg = (struct etharp_hdr *)inbuf;
    if((ntohs(arpmsg->proto) == ETHTYPE_IP) &&
         (ntohs(arpmsg->opcode) == ARP_REQUEST))
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx ARP req\n");
        if(arpmsg->sipaddr.addrw[0] == 0 && arpmsg->sipaddr.addrw[1] == 0)
        {
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "ignore ARP req as invalid sender ip\n");
            return -1;//sender ip in request is zero,no need reply
        }

        struct etharp_hdr *arpmsg_reply = (struct etharp_hdr *)outbuf;
        arpmsg_reply->hwtype = arpmsg->hwtype;
        arpmsg_reply->proto = arpmsg->proto;
        arpmsg_reply->hwlen = arpmsg->hwlen;
        arpmsg_reply->protolen = arpmsg->protolen;
        arpmsg_reply->opcode = htons(ARP_REPLY);

        memcpy(&arpmsg_reply->dhwaddr, &arpmsg->shwaddr, sizeof(struct eth_addr));
        memcpy(&arpmsg_reply->dipaddr, &arpmsg->sipaddr, sizeof(ip4_addr_t));

        memcpy(&arpmsg_reply->shwaddr, device_macaddr, sizeof(struct eth_addr));
        memcpy(&arpmsg_reply->sipaddr, &dhcpserver_dns_router_addr_n, sizeof(ip4_addr_t));
        return sizeof(struct etharp_hdr);
    }
    else if((ntohs(arpmsg->proto) == ETHTYPE_IP) &&
         (ntohs(arpmsg->opcode) == ARP_REPLY))
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx ARP rpy\n");
        return -1;
    }
    return -1;
}

PRIVATE INT32 process_ip_packet(UINT8* inbuf, UINT16 inbuflen, UINT8* outbuf)
{
    struct ip_hdr *iphdr = (struct ip_hdr *)inbuf;
    UINT16 iphdrlen;
    INT32 ret_len = 0;
    /* obtain IP header length in number of 32-bit words */
    iphdrlen = IPH_HL(iphdr);
    /* calculate IP header length in bytes */
    iphdrlen *= 4;
    /* verify checksum */
#if CHECKSUM_CHECK_IP
    if (inet_chksum(iphdr, iphdrlen) != 0)
    {
        URNDIS_TRACE(URNDIS_INFO_TRC, 0, "Usb Rndis IP checksum (0x%x) failed\n",inet_chksum(iphdr, iphdrlen));
        return -1;
    }
#endif

    switch (IPH_PROTO(iphdr))
    {
        case IP_PROTO_UDP:
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx UDP\n");
            ret_len = process_udp_packet(inbuf + iphdrlen, outbuf + iphdrlen);
            break;
        case IP_PROTO_TCP:
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx TCP\n");
            ret_len = process_tcp_packet(inbuf, inbuf + iphdrlen, inbuflen - iphdrlen);
            break;
        case IP_PROTO_ICMP:
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx ICMP,alternate src ip then send\n");
            ip4_addr_set(&iphdr->src, &s_tethering_ctx.ipaddr);
            IPH_CHKSUM_SET(iphdr, 0);
            IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));
            break;
        case IP_PROTO_IGMP:
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx IGMP\n");
            ret_len = -1;//TODO:process it
            break;
        default:
            URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx unknown ip proto %02x\n",IPH_PROTO(iphdr));
            ret_len = -1;//TODO:process it
            break;
    }

    if(ret_len > 0)
    {
        struct ip_hdr* iphdr_send = (struct ip_hdr *)outbuf;
 		/* Fill out the response IP packet header */
        IPH_LEN_SET(iphdr_send, htons(sizeof(struct ip_hdr) + ret_len));
        IPH_VHLTOS_SET(iphdr_send, 4, IP_HLEN / 4, 0);
        IPH_OFFSET_SET(iphdr_send, htons(IP_DF));
		IPH_ID_SET(iphdr_send, 0);
        IPH_PROTO_SET(iphdr_send, IPH_PROTO(iphdr));
        IPH_TTL_SET(iphdr_send, 128);//set to default ttl;TODO
        ip4_addr_set(&(iphdr_send->src), &(iphdr->dest));
        ip4_addr_set(&(iphdr_send->dest), &(iphdr->src));
        IPH_CHKSUM_SET(iphdr_send, 0);
#if CHECKSUM_GEN_IP
        IPH_CHKSUM_SET(iphdr_send, inet_chksum(iphdr_send, IP_HLEN));
#endif
		/* Return the size of the response so far */
		return (sizeof(struct ip_hdr) + ret_len);
    }
    return ret_len;
}

PRIVATE INT32 process_rx_data_packet(UINT8* buff, UINT32 recv_len)
{
    struct rndis_packet_msg_type *hdr = (void *)buff;
    struct eth_hdr *e_hdr = (struct eth_hdr *)(buff + sizeof(struct rndis_packet_msg_type));
    u32 msg_len, data_offset, data_len;
    INT32 ret_len = 0;
    msg_len = le32_to_cpu(hdr->MessageLength);
    data_offset = le32_to_cpu(hdr->DataOffset);
    data_len = le32_to_cpu(hdr->DataLength);
    URNDIS_TRACE(URNDIS_INFO_TRC, 0, "Usb Rndis rx recv_len=%d\n",recv_len);

    if (recv_len < msg_len ||
            ((data_offset + data_len + 8) > msg_len)) {
        pr_err("invalid rndis message: %d/%d/%d/%d, len:%d\n",
                le32_to_cpu(hdr->MessageType),
                msg_len, data_offset, data_len, recv_len);
        return -EOVERFLOW;
    }
    if (le32_to_cpu(hdr->MessageType) != RNDIS_MSG_PACKET) {
        pr_err("invalid rndis message: %d/%d/%d/%d, len:%d\n",
                le32_to_cpu(hdr->MessageType),
                msg_len, data_offset, data_len, recv_len);
        return -EINVAL;
    }

    if (data_len > MAX_MTU)
    {
        return -EINVAL;
    }

    UINT16 e_payload_rcv_len = data_len - sizeof(struct eth_hdr);
    e_payload_rcv = sxr_Malloc(e_payload_rcv_len);
    e_payload_send = sxr_Malloc(MAX_MTU);
    memset(e_payload_send, 0, MAX_MTU);
    memcpy(e_payload_rcv, buff + sizeof(struct rndis_packet_msg_type) + sizeof(struct eth_hdr), e_payload_rcv_len);

    if (ntohs(e_hdr->type) == ETHTYPE_ARP)
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx ARP\n");
        ret_len = process_arp_packet(e_payload_rcv, e_payload_send);
    }
    else if (ntohs(e_hdr->type) == ETHTYPE_IP)
    {
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis rx IP\n");
        ret_len = process_ip_packet(e_payload_rcv, e_payload_rcv_len, e_payload_send);
    }
    else
    {
        ret_len = -1;//IPV6? TODO:process it
    }

    if (ret_len > 0)
    {
        rndis_eth_packet_send(e_payload_send, ret_len, e_hdr->type);
    }
    else if (ret_len == 0)
    {
        rndis_ip_packet_send(e_payload_rcv, e_payload_rcv_len);
    }

    sxr_Free(e_payload_rcv);
    sxr_Free(e_payload_send);
    return 0;
}

PRIVATE INT32 process_tx_data_packet(UINT32 cid, UINT32 pData, UINT32 simid)
{
    if(cid == s_tethering_ctx.cid && simid == s_tethering_ctx.simid)
    {
        CFW_GPRS_DATA *pGprsData = (CFW_GPRS_DATA *)pData;
        UINT16 ip_data_send_len = pGprsData->nDataLength;
        UINT16 iphdrlen;
        URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis tx ip_data_send_len=%d\n",ip_data_send_len);

        ip_data_send = sxr_Malloc(MAX_MTU);
        memcpy(ip_data_send, pGprsData->pData, ip_data_send_len);
        CSW_GPRS_FREE(pGprsData);

        struct ip_hdr *iphdr = (struct ip_hdr *)ip_data_send;
        /* obtain IP header length in number of 32-bit words */
        iphdrlen = IPH_HL(iphdr);
        /* calculate IP header length in bytes */
        iphdrlen *= 4;
        switch (IPH_PROTO(iphdr))
        {
            case IP_PROTO_UDP:
                URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis tx UDP\n");
                struct udp_hdr* udphdr_send = (struct udp_hdr *) (ip_data_send + iphdrlen);
                if(ntohs(udphdr_send->src) == DNS_SERVER_PORT)
                {
                    URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP from DNS server,alternate src ip\n");
                    ip4_addr_set(&iphdr->src, &dhcpserver_dns_router_addr_n);
                }
                else
                {
                    URNDIS_TRACE(URNDIS_DATA_TRC, 0, "UDP from port %d\n",ntohs(udphdr_send->src));
                }

                udphdr_send->chksum = 0;//zero means no checksum;TODO gen checksum when CHECKSUM_GEN_UDP
                #if CHECKSUM_GEN_UDP
                #endif

                ip4_addr_set(&iphdr->dest, &dhcpclient_host_addr_n);
                IPH_CHKSUM_SET(iphdr, 0);
                IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));
                break;
            case IP_PROTO_TCP:
                URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis tx TCP\n");
                struct tcp_hdr* tcphdr_send = (struct tcp_hdr *) (ip_data_send + iphdrlen);
                ip4_addr_set(&iphdr->dest, &dhcpclient_host_addr_n);
                IPH_CHKSUM_SET(iphdr, 0);
                IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));

                tcphdr_send->chksum = 0;
                tcphdr_send->chksum = tcp_checksum(tcphdr_send, iphdr->src, iphdr->dest, ip_data_send_len - iphdrlen);
                break;
            case IP_PROTO_ICMP:
                URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis tx ICMP\n");
                ip4_addr_set(&iphdr->dest, &dhcpclient_host_addr_n);
                IPH_CHKSUM_SET(iphdr, 0);
                IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));
                break;
            case IP_PROTO_IGMP:
                URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis tx IGMP\n");
                return 0;//TODO:process it
                break;
            default:
                URNDIS_TRACE(URNDIS_DATA_TRC, 0, "Usb Rndis tx unknown ip proto %02x\n",IPH_PROTO(iphdr));
                break;
        }

        rndis_eth_packet_send(ip_data_send, ip_data_send_len, htons(ETHTYPE_IP));

        sxr_Free(ip_data_send);
    }
    return 0;
}

PUBLIC UINT32 BAL_Run_Rndis(CFW_EVENT *pEvent)
{
    //UINT16 n = MAKEUINT16(pEvent->nFlag, pEvent->nType);
    COS_EVENT ev;

    ev.nEventId = pEvent->nEventId;
    ev.nParam1  = pEvent->nParam1;
    ev.nParam2  = pEvent->nParam2;
    //ev.nParam3  = MAKEUINT32(n, pEvent->nUTI);
    ev.nParam3  = pEvent->nFlag;
    sxr_Send(&ev, g_urndisMbx, SXR_SEND_EVT);
    return BAL_PROC_DONT_FREE_MEM;
}

extern VOID rndis_control_send_intr(VOID);

VOID urndis_DataTask(VOID* param)
{
    Msg_t* msg;
    UINT32 evt[4];
    UINT32 recv_len;
    while(1)
    {
        msg = sxr_Wait(evt, g_urndisMbx);
        if (msg == NULL)
        {
            // We received an event from cfw task for EV_CFW_GPRS_DATA_IND
            //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "urndis_DataTask rcv evt=%x %x\n",evt[0],EV_CFW_GPRS_DATA_IND);
            if (evt[0] == EV_CFW_GPRS_DATA_IND)
            {
                process_tx_data_packet(evt[1], evt[2], evt[3]);
            }
        }
        else
        {
            switch(msg->B)
            {
                case URNDIS_MSG_RCV_DATA:
                    recv_len  = hal_UsbEpRxTransferedSize(g_urndisBoOutEp);
                    process_rx_data_packet(g_UsbBulkOut, recv_len);
                    //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Data Receive\n");
                    UINT32 csStatus = hal_SysEnterCriticalSection();
                    hal_UsbRecv(g_urndisBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
                    hal_SysExitCriticalSection(csStatus);
                    break;
                case URNDIS_MSG_SEND_INTR:
                    //URNDIS_TRACE(URNDIS_INFO_TRC, 0, "--- Send Intr\n");
                    rndis_control_send_intr();
                    break;
                default :
                    break;
            }
            sxr_Free(msg);
        }
    }
}

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T*
urndis_DataOpen(CONST URNDIS_DATA_PARAMETERS_T*  cfg)
{
    HAL_USB_EP_DESCRIPTOR_T** epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T**)
             sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T*)*3);

    memset(g_UsbBulkIn, 0, sizeof(g_UsbBulkIn));
    memset(g_UsbBulkOut, 0, sizeof(g_UsbBulkOut));

    if(g_urndisIdTask == 0)
    {
        g_urndisTask.TaskBody  = urndis_DataTask;
        g_urndisTask.TaskExit  = 0;
        g_urndisTask.Name      = "URNDIS DATA";
        g_urndisTask.StackSize = 512;
        g_urndisTask.Priority  = 51;
        g_urndisMbx            = sxr_NewMailBox();
        g_urndisIdTask         = sxr_NewTask(&g_urndisTask);
        sxr_StartTask(g_urndisIdTask, 0);
    }

    g_urndisSendSemaphore = sxr_NewSemaphore(1);

    g_urndisBoInEp  = HAL_USB_EP_DIRECTION_IN (cfg->inEpNum );
    g_urndisBoOutEp  = HAL_USB_EP_DIRECTION_OUT (cfg->outEpNum );

    epList[0] = uctls_NewBulkEpDescriptor(g_urndisBoInEp, rndisCallbackEp);
    epList[1] = uctls_NewBulkEpDescriptor(g_urndisBoOutEp, rndisCallbackEp);
    epList[2] = 0;

    URNDIS_TRACE(URNDIS_INFO_TRC, 0, "URNDIS Data Open\n");
    return uctls_NewInterfaceDescriptor(USB_CLASS_CDC_DATA, 0x00, 0x00, 0x01,
                                        epList, "RNDIS Ethernet Data", rndisDataCallback);
}
PRIVATE VOID urndis_DataClose(VOID)
{
    if(g_urndisIdTask != 0)
    {
        sxr_StopTask(g_urndisIdTask);
        sxr_FreeTask(g_urndisIdTask);
        sxr_FreeMailBox(g_urndisMbx);
    }
    g_urndisIdTask = 0;
    sxr_FreeSemaphore(g_urndisSendSemaphore);
}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST URNDIS_DATA_CALLBACK_T g_urndisDataCallback =
{
    .open  = urndis_DataOpen,
    .close = urndis_DataClose
};


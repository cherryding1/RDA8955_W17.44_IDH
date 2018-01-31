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


#ifndef __PAN_H__
#define __PAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PAN_BNEP_COMPRESSED_PACKET_HEADER_LENGTH  3
#define PAN_BNEP_COMPRESSED_ONLY_PACKET_HEADER_LENGTH 9
#define PAN_BNEP_PACKET_HEADER_LENGTH   15


//Service UUID
#define BNEP_SERVICE_ID_PANU 0x1115
#define BNEP_SERVICE_ID_NAP   0x1116
#define BNEP_SERVICE_ID_GN     0x1117

//protocol type
#define     PROTOCOL_IPV4        0x0800
#define     PROTOCOL_ARP         0x0806
#define     PROTOCOL_FRAME_RELAY_ARP 0x0808
#define     PROTOCOL_PPTP      0x880B
#define     PROTOCOL_GSMP     0x880C
#define     PROTOCOL_RARP     0x8035
#define     PROTOCOL_IPV6      0x86DD

typedef enum
{
    PAN_USER,
    PAN_NAP,
    PAN_GN
} PAN_ROLE;

typedef enum
{
    DHCP_DISCOVER=1,
    DHCP_OFFER,
    DHCP_REQUEST,
    DHCP_ACK=5
} DHCP_ROLE;

typedef enum
{
    BNEP_GENERAL_ETHERNET,
    BNEP_CONTROL,
    BNEP_COMPRESSED_ETHERNET,
    BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY,
    BNEP_COMPRESSED_ETHERNET_DEST_ONLY
} BNEP_TYPE;


typedef enum
{
    BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD,
    BNEP_SETUP_CONNECTION_REQUEST_MSG,
    BNEP_SETUP_CONNECTION_RESPONSE_MSG,
    BNEP_FILTER_NET_TYPE_SET_MSG,
    BNEP_FILTER_NET_TYPE_RESPONSE_MSG,
    BNEP_FILTER_MULTI_ADDR_SET_MSG,
    BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG
} BNEP_CONTROL_TYPE;

typedef enum
{
    BNEP_RESPONSE_SUCCESSFUL,
    BNEP_RESPONSE_INVALID_DESTINATION_SERVICE_UUID,
    BNEP_RESPONSE_INVALID_SOURCE_SERVICE_UUID,
    BNEP_RESPONSE_INVALID_SERVICE_UUID_SIZE,
    BNEP_RESPONSE_CONNECTION_NOT_ALLOWED
} BNEP_SETUP_CONNECTION_RESPONSE;

typedef enum
{
    BNEP_FILTER_CONTROL_RESPONSE_SUCCESSFUL,
    BNEP_FILTER_CONTROL_RESPONSE_UNSUPPORTED_REQUEST,
    BNEP_FILTER_CONTROL_RESPONSE_INVALID_TYPE_RANGE,
    BNEP_FILTER_CONTROL_RESPONSE_MAXIMUM_FILTER_LIMIT_REACHED,
    BNEP_FILTER_CONTROL_RESPONSE_UNABLE_FULFILL_REQUEST
} BNEP_NETWORK_PROTOCOL_TYPE_FILTER_RESPONSE;


typedef enum
{
    BNEP_MULTICAST_ADDRESS_FILTER_RESPONSE_SUCCESSFUL,
    BNEP_MULTICAST_ADDRESS_FILTER_RESPONSE_UNSUPPORTED_REQUEST,
    BNEP_MULTICAST_ADDRESS_FILTER_RESPONSE_INVALID_MULTICAST_ADDRESS,
    BNEP_MULTICAST_ADDRESS_FILTER_RESPONSE_MAXIMUM_MULTICAST_ADDRESS_FILTER_LIMIT_REACHED,
    BNEP_MULTICAST_ADDRESS_FILTER_RESPONSE_UNABLE_FULFILL_REQUEST
} BNEP_MULTICAST_ADDRESS_FILTER_RESPONSE;

typedef struct
{
    u_int8 result;
    u_int16 cid;
    t_bdaddr bdaddr;
} t_pan_connect_msg_t;

typedef struct
{
    u_int8  result;
    u_int16  cid;
    t_bdaddr bdaddr;
} t_pan_disconnect_msg_t;

typedef struct
{
    u_int8 bnep_type;
    u_int16 protocol_type;
    t_bdaddr dest_addr;
    t_bdaddr src_addr;
    u_int16 data_len;
    u_int8  *data;
} t_pan_ethernet_data_msg_t;


APIDECL1 t_api APIDECL2 PAN_Register_Service(u_int8 role);
APIDECL1 t_api APIDECL2 PAN_Setup_Connect(t_bdaddr address, u_int16 dest_service_id);
APIDECL1 t_api APIDECL2 PAN_Disconnect(void);
void PAN_Bnep_Send_ARP_Request(u_int16 ip_type, t_bdaddr sender_addr, t_bdaddr dest_addr,  u_int8* sender_protocol_addr,u_int8 * Dest_protocol_addr);
void PAN_Bnep_Send_IP_Data_Request(u_int8 bnep_type, u_int16 protocol_type, t_bdaddr src_addr, t_bdaddr dest_addr,  u_int8 **dhcp_data, u_int16 data_length);
void PAN_Bnep_Setup_Connection_Response(u_int16 cid,u_int8 result);


#ifdef __cplusplus
}
#endif

#endif //__PAN_H__


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



#ifndef BLUETOOTH_HOST_CONFIG_H
#define BLUETOOTH_HOST_CONFIG_H


/********************************** Host Stacks ***************************************************/
/* All configurable host stack parameters should be defined in this file, with a descriptive comment */
/* All timer values are in tenths of a second, i.e. 400 - 40 seconds */

/****************** host control ***************/
typedef struct _bt_control_config
{
    u_int8 max_timers;
    u_int8 max_buff;

    u_int8 max_message;
    u_int16 message_length;

    u_int8 host_mem_support;
    u_int16 host_mem_size; // in block(256Bytes)
} host_ctrl_config_t;


/******************* HCI ******************/
typedef struct _bt_hci_config
{
    u_int8 flow_control_support;
    u_int8 flow_control_timeout;
    u_int8 flow_control_threshold;

    u_int8 max_acl_number;
    u_int16 max_acl_size;
    u_int8 max_sco_number;
    u_int8 max_sco_size;

    u_int32 max_acl_queue_size;
} hci_config_t;


/******************* Manager ***************/
typedef struct _bt_mgr_config
{
    /* max support device/connects */
    u_int8 max_acl_links;
    u_int8 max_device_list;

    /* suport multi connections or not */
    u_int8 multi_connect_support;

    /* class of local device */
    u_int32 device_class;

    /* enable host security */
    u_int8 security_support;

    /* packet types for SCO/eSCO link
    avilable value of following bit or:
    #define HCI_SYN_HV1         0x0001
    #define HCI_SYN_HV2         0x0002
    #define HCI_SYN_HV3         0x0004
    #define HCI_SYN_EV3         0x0008
    #define HCI_SYN_EV4         0x0010
    #define HCI_SYN_EV5         0x0020
    #define HCI_SYN__2EV3          0x0040
    #define HCI_SYN__3EV3          0x0080
    #define HCI_SYN__2EV5          0x0100
    #define HCI_SYN__3EV5          0x0200
    */
    u_int16 sco_packets;
    u_int16 voice_setting;

    /* process sco packet by hci or not */
    u_int8 sco_via_hci;

    u_int8 ssp_support;

    /* local io capability for simple security pairing,
    for mobile, select MGR_IOCAP_DISPLAY_YESNO or  MGR_IOCAP_KEYBOARD_ONLY
    for other device, MGR_IOCAP_DISPLAY_ONLY and MGR_IOCAP_NOINPUT_NOOUTPUT can be selected */
    u_int8 io_capability;

    u_int8 sniff_timeout;
    u_int16 sniff_interval;
    u_int8 unsniff_packets;
    u_int8 linkto_support;
    u_int32 linkto_value;
} manager_config_t;


/****************** L2CAP ****************/
typedef struct _bt_l2cap_config
{
    u_int8 auto_config;
    u_int8 fragment_acl_support;
    u_int8 config_to_core;

    u_int8 max_channel;
    u_int8 max_links;

    u_int16 max_in_mtu;
    u_int16 max_out_mtu;

    /* If 0, send connectionless traffic as ACTIVE broadcast, if 1 use PICONET broadcast */
    u_int8 use_piconet_broadcast;
    u_int16 max_broadcast_mtu;
    /* handle used for broadcast acl data send */
    u_int16 broadcast_handle;
    /****************************************************************
     *   L2CAP  extended features bit mask   4 octs
     *   bit 0              |   bit 1               |   bit 2               |   bit 3                           |
     *   flow control mode  | retransmission mode   | Bi-directional QoS1   | Enhanced retransmission Mode  |
     *
     *   bit 4              |   bit 5               |   bit 6               |   bit 7                       |
     *   Streaming Mode | FCS Option            | Ex flow Spec BR/EDR   | Fixed Channels                |
     *
     *   bit 8              |   bit 9                           |   bit 9-31    |
     *   ex window size     | unicast connectionless Data reception| reserved       |
    */
    u_int32 extended_features;
} l2cap_config_t;


/******************** SDP *****************/
typedef struct _bt_sdp_config
{
    u_int16 mtu;        // normal 48
} sdp_config_t;

/******************** OBEX ***************/
typedef struct _bt_obex_config
{
    u_int16 local_mtu;
    u_int8 server_transport;
    //u_int16     max_path;

} obex_config_t;

/******************* RFCOMM ***************/
typedef struct _bt_rfcomm_config
{
    u_int8 init_credit;
    u_int8 max_credit;
    u_int8 min_credit;
    u_int8 credit_issue;

    u_int16 max_frame_size;

} rfcomm_config_t;


/******************************************* Profiles ****************************************************/
typedef struct _bt_hfp_config
{
    u_int16 hf_feature;
    u_int16 ag_feature;
    u_int8 handle_at_cmd; // handle at commond by application
} hfp_config_t;

typedef struct _bt_a2dp_config
{
    u_int16 media_mtu;
    u_int16 signal_mtu;
    u_int8 accetp_connect;
} a2dp_config_t;

typedef struct _bt_spp_config
{
    u_int8 max_ports; // max active spp virtual port
    u_int8 server_num; // max server number
    u_int16 max_data_len; // max data length
} spp_config_t;

typedef struct _bt_pan_config
{
    u_int8 min_mtu;
    u_int16 max_mtu;
} pan_config_t;

//added for SMP of bt4.0 2012-12-11
typedef struct _bt_smp_config
{
    u_int8 ioCap;
    u_int8 oobDataFlg;
    u_int8 authReq;
    u_int8 maxEncryptKeySize;
    u_int8 initKeyDistribution;
    u_int8 rspKeyDistribution;
} smp_config_t;



typedef struct _bt_config
{

    host_ctrl_config_t host_ctrl_config;

    hci_config_t hci_config;
    manager_config_t manager_config;
    l2cap_config_t l2cap_config;
    sdp_config_t sdp_config;
    obex_config_t obex_config;
    rfcomm_config_t rfcomm_config;

    hfp_config_t hfp_config;
    a2dp_config_t a2dp_config;
    spp_config_t spp_config;
    pan_config_t pan_config;
    smp_config_t smp_config;
} bt_config_t;



#endif /* BLUETOOTH_HOST_CONFIG_H */


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

#ifndef HID_H_
#define HID_H_

#include "bt_types.h"
#include "bt.h"
struct __SINK;
/*! The Sink type. */
typedef struct __SINK *Sink;


/* Defines MTU size that L2CAP will advertise.*/
#define HID_L2CAP_MTU                48

/*------------------------------------------
    QOS parameters
--------------------------------------------*/
#define SERVICE_NO_TRAFFIC              0x00
#define SERVICE_BEST_EFFORT             0x01
#define SERVICE_GUARANTEED              0x02

/*------------------------------------------
    QOS defaults
--------------------------------------------*/
#define MINIMUM_MTU             48
#define MAXIMUM_MTU             48


#define DEFAULT_FLUSH_TO        0xFFFF
#define DEFAULT_LINK_TO         0x7530
#define DEFAULT_SERVICE_TYPE    SERVICE_NO_TRAFFIC
#define DEFAULT_TOKEN_RATE      0x00000000
#define DEFAULT_TOKEN_BUCKET    0x00000000
#define DEFAULT_PEAK_BW         0x00000000
#define DEFAULT_LATENCY         0xFFFFFFFF
#define DEFAULT_DELAY_VAR       0xFFFFFFFF


/*This identifies the device as a general device.*/
#define HID_MAJOR_DEVICE_CLASS       0x0500
#define HID_MINOR_MOUSE              0x000080
#define HID_MINOR_KEYBOARD           0x000040

/* Base message number for host.*/
#define HID_MESSAGE_BASE                0x7100

/* HID L2CAP PSMs */
#define HID_PSM_CONTROL                 (0x11)
#define HID_PSM_INTERRUPT               (0x13)

/* HID Profile Library private messages */
#define HID_INTERNAL_MESSAGE_BASE       (0x0)

/* Timeout in seconds between control channel connected and interrupt channel connection */
#define HID_CONNECT_TIMEOUT             (10)

/* Timeout in seconds to complete L2CAP connection */
#define HID_L2CAP_TIMEOUT               (5)

/* Timeout in seconds to complete HID control request */
#define HID_REQUEST_TIMEOUT             (20)

/* Timeout in seconds to complete remote disconnection */
#define HID_DISCONNECT_TIMEOUT          (5)
#define HID_CONN_NUMBER                 (1)


/* HID device feature defines */
#define HID_VIRTUAL_CABLE                   0x0010    // SDP ID 0x0204
#define HID_RECONNECT_INITIATE              0x0020    // SDP ID 0x0205
#define HID_SDP_DISABLE                     0x0100    // SDP ID 0x0208
#define HID_BATTER_POWER                    0x0200    // SDP ID 0x0209
#define HID_REMOTE_WAKE                     0x0400    // SDP ID 0x020A
#define HID_NORMALLY_CONNECTABLE            0x2000    // SDP ID 0x020D
#define HID_BOOT_DEVICE                     0x4000    // SDP ID 0x020E


/* Packet types */

#define HID_PACKET_TYPE_MASK             0xf0
#define HID_HANDSHAKE                    0
#define HID_CONTROL                             1
#define HID_GET_REPORT                   4
#define HID_SET_REPORT                   5
#define HID_GET_PROTOCOL                 6
#define HID_SET_PROTOCOL                 7
#define HID_GET_IDLE                     8
#define HID_SET_IDLE                     9
#define HID_DATA                         10
#define HID_DATC                         11

#define MAX_NUM_HID_CONNECTIONS              2
#define HID_CON_CONTROL                  0
#define HID_CON_INTERRUPT                1

#define HID_IND_BASE        (HID_GET_REPORT_IND - HID_GET_REPORT)


#define HID_MAX_PAYLOAD                 23  //23


typedef enum
{
    HID_OPCODE_UNKNOWN  = 0x00,
    HID_OPCODE_KEYBOARD ,
    HID_OPCODE_MOUSE,
    HID_OPCODE_WII

}  t_hidOpcode;

enum hid_role
{
    HID_DEVICE = 0,
    HID_HOST = 1
};

/*Possible result codes for the HANDSHAKE messages.  See HID profile 7,4,1 table 5.  NOTE: These values must match those defined in the HID profile specification.*/
typedef enum
{
    Hid_success       = 0,             /*!< Successful operation.*/
    Hid_busy          = 1,             /*!< Device is busy with previous operation.*/
    Hid_invalid_id    = 2,             /*!< Invalid report ID.*/
    Hid_unsupported   = 3,             /*!< Operation is unsupported.*/
    Hid_invalid_param = 4,             /*!< Invalid parameter.*/
    Hid_failed        = 14,            /*!< Unknown failure.*/
    Hid_timeout        = 15,           /*!< Operation timeout.*/
    Hid_resource_error = 16            /*!< Not enough resource to complete operation. (Local error, never reported to remote host/device)*/
} HID_HANDSHAKE_MESSAGE;

/* HID states */
typedef enum
{
    HidIdle,                                 /*!< The HID is initialised and ready for connection.*/
    HidServiceSearching,                     /* search hid service attribute */
    HidConnecting,
    HidConnectingLocal=HidConnecting,        /*!< The HID is currently connecting (locally initiated).*/
    HidConnectingRemote=HidConnecting,       /*!< The HID is currently connecting.*/
    HidConnected,                            /*!< The HID is connected.*/
    HidDisconnecting,
    HidDisconnectingLocal=HidDisconnecting,  /*!< The HID is disconnecting, initiated locally. */
    HidDisconnectingRemote=HidDisconnecting, /*!< The HID is disconnecting, initiated remotely. */
    HidDisconnectingFailed,                  /*!< The HID is disconnecting, initiated due to connection failure. */
    HidDisconnected                          /*!< The HID is disconnected. */               /*!< The HID is disconnected. */
} HidState;


/* Possible status codes for the connection.*/
typedef enum
{
    Hid_connect_success,            /*!< Successful connection.*/
    Hid_connect_failed,             /*!< Connection failed. */
    Hid_connect_timeout,            /*!< Timeout waiting for connection. */
    Hid_connect_disconnected        /*!< Disconnected remotely during setup */
} Hid_connect_status;

/* Possible status codes for the disconnection.*/
typedef enum
{
    Hid_disconnect_success,        /*!< Successful disconnection.*/
    Hid_disconnect_link_loss,      /*!< Unsuccessful due to the link being lost.*/
    Hid_disconnect_timeout,        /*!< Unsuccessful due to time out.*/
    Hid_disconnect_violation,      /*!< Disconnection due to protocol violation */
    Hid_disconnect_error,          /*!< Unsuccessful for some other reason.*/
    Hid_disconnect_virtual_unplug  /*!< Virtual unplug disconnection */
} Hid_disconnect_status;


/* HID Packet reassembly status */
typedef enum
{
    hidPacketStatusCompleted,
    hidPacketStatusPending,
    hidPacketStatusFailed,
    hidPacketStatusBusy
} HidPacketStatus;

/* HID connection states */
typedef enum
{
    hidConIdle,
    hidConConnectPending,
    hidConConnecting,
    hidConConnected,
    hidConDisconnecting,
    hidConDisconnected
} HidConnectionState;

typedef enum
{
    HID_PINCODE_IDLE=0x00,
    HID_PINCODE_READY ,
    HID_PINCODE_INPUT

}  t_hidPinCode;

/* HID connection structure ,different channals */
typedef struct
{
    HidConnectionState        state;
    union
    {
        u_int32               id;
        Sink                  sink;
    } con;
    u_int16                   remote_mtu;
} HidConnection;

/* Power mode.*/
typedef enum
{
    /*! Use active power mode.*/
    lp_active,
    /*! Use sniff power mode.*/
    lp_sniff,
    /*! Use the passive power mode. Passive mode is a "don't care" setting
      where the local device will not attempt to alter the power mode. */
    lp_passive = 0xFF
} lp_power_mode;

/* Power table.*/
typedef struct
{
    /*! The power mode.*/
    lp_power_mode            state;
    /*! The minimum interval.*/
    u_int16                  min_interval;
    /*! The maximum interval.*/
    u_int16                  max_interval;
    /*! Determines for how many slots the slave shall listen when the slave is not treating this as a scatternet link.*/
    u_int16                  attempt;
    /*! Determines for how many additional slots the slave shall listen when the slave is not treating this as a scatternet link.*/
    u_int16                  timeout;
    /*! The time.*/
    u_int16                  time;
} lp_power_table;



/* HID connection configuration structure.*/
typedef struct
{
    u_int16                 power_table_size;
    lp_power_table          *power_table;
    u_int32                 latency;
    boolean                    qos_guaranteed;

} Hid_connection_config;


/*HID boot modes.  See HID profile 7.4.5, table 10 & 7.4.6, table 11.  NOTE: These values must match those defined in the HID profile specification.*/
typedef enum
{
    Hid_protocol_boot = 0,
    Hid_protocol_report = 1
} HID_PROTOCOL;
/*! @brief Status of pin code entry.*/
typedef enum
{
    Hid_pin_ok,
    Hid_pin_full,
    Hid_pin_complete,
    Hid_pin_add,
    Hid_pin_delete,
    Hid_pin_cleared,
    Hid_pin_error
} HID_PIN_STATUS;
typedef enum
{
    Hid_pin_enable,
    Hid_pin_disenable
} HID_PIN_FLAGS;

/*! @brief Maximum length of pin code.*/
#define HID_PIN_MAX_LENGTH (16)

/*! @brief Pin code state structure.*/
typedef struct
{
    boolean              pin_entered;
    HID_PIN_STATUS       pin_status;
    u_int8               pin_size;
    u_int8               pin_code[HID_PIN_MAX_LENGTH];

} HID_BT_KB_PINCODE_DATA;
extern HID_BT_KB_PINCODE_DATA  Hid_pinCodeBuffer;


/* HID control operations. See HID profile 7.4.2, table 6.  NOTE: These values must match those defined in the HID profile specification.*/
typedef enum
{
    Hid_control_op_nop           = 0,
    Hid_control_op_hard_reset    = 1,
    Hid_control_op_soft_reset    = 2,
    Hid_control_op_suspend       = 3,
    Hid_control_op_exit_suspend  = 4,
    Hid_control_op_unplug        = 5
} HID_CONTROL_OP;

/* HID report types.  See HID profile 7.4.3, table 7 and  7.4.4, table 8.
    NOTE: These values must match those defined in the HID profile  specification.*/
typedef enum
{
    Hid_report_other   = 0,
    Hid_report_input   = 1,
    Hid_report_output  = 2,
    Hid_report_feature = 3
} HID_REPORT_TYPE;

///////////////////////////////////////hid l2cap structure////////////////////////////////////////
/*      L2CAP Quality of Service Parameters

    The Quality of Service parameters are negotiated before an L2CAP connection
    is established.  A detailed explanation of each of these parameters can be
    found in the L2CAP section of the Bluetooth specification.
*/
typedef struct
{
    /*! Level of the service required e.g. best effort. */
    u_int8  service_type;
    /*! Average data rate with which data is transmitted. */
    u_int32 token_rate;
    /*! Specifies a limit on the "burstiness" with which data may be  transmitted. */
    u_int32 token_bucket;
    /*! This limits how fast L2CAP packets can be sent back-to-back. */
    u_int32 peak_bw;
    /*! Maximum acceptable latency of an L2CAP packet. */
    u_int32 latency;
    /*! Difference between the maximum and minimum acceptable delay of an L2CAP packet. */
    u_int32 delay_var;
} qos_flow;


/* L2CAP connection configuration parameters.*/
typedef struct
{
    /*! This is the L2CAP MTU the local device will advertise during the L2CAP
      connection configuration. The L2CAP MTU advertised by a device is defined
      in the L2CAP section of the Bluetooth specification.*/
    u_int16     mtu_local;
    /*! If the remote device advertises an MTU smaller than this value then the
      Connection library will not proceed with the L2CAP connection to that
      device. This field should be used when a profile mandates a specific
      minimum MTU since a device attempting to use a smaller MTU would be
      breaking that particular profile specification. Unless such an MTU value
      is specified in the profile being implemenmted it is recommended that
      this value is set to MINIMUM_MTU (as definedin l2cap_prim.h) in order to
      ensure better interoperability i.e. we allow the remote device to
      advertise any size of MTU as long as it is greater than the minimum MTU
      allowed in the L2CAP specification.*/
    u_int16     mtu_remote_min;
    /*! Informs the remote end of the local device's flush timeout settings.*/
    u_int16     flush_timeout;
    /*! If set to TRUE we will connect to a device regardless of its flush
      timeout setting. If set to FALSE we will not proceed with the L2CAP
      connection unless the remote device is using the default flush
      timeout. Once a connection is established the client will be informed of
      the remote device's flush timeout.*/
    boolean     accept_non_default_flush;
    /*! The local device's Quality of Service settings. */
    qos_flow    qos;
    /*! If set to TRUE the local device accepts the Quality of Service settings
      of the remote device whatever they are. If FALSE the local device will
      only proceed with the connection if the remote device has set its Quality
      of Service settings to "best effort". Once a connection is established
      the client will be informed of the Quality of Service settings of the remote device.*/
    boolean     accept_qos_settings;
    /*! Connection attempt timeout specified in seconds. If the connection is
      not established before this timeout expires then the connect attempt is aborted. */
    u_int32     timeout ;
} l2cap_config_params;


typedef struct
{
    u_int8 ReportType;
    u_int8 ReportID;
    u_int8 length;
    u_int8 data[HID_MAX_PAYLOAD];
} t_hid_input_data;


typedef struct
{
    t_bdaddr addr;
    u_int16 cid;
    u_int8 result;
} t_hid_connect;

typedef struct
{
    u_int16 version;
    u_int16 feature;
    u_int8  subClass;
    u_int8  countryCode;
    u_int16 language;
    u_int16 encoding;
    u_int16 langID;
    u_int16 supervisionTimeout;
    u_int16 desc_length;
    u_int8 *descriptor;
    u_int8 *serviceName;
} t_hid_attributes;


typedef struct
{
    u_int16 length;
    u_int8* value;
} t_hid_key;

#ifdef __cplusplus
extern "C" {
#endif


APIDECL1 t_api APIDECL2 HID_Connect(t_bdaddr device_addr, u_int8 remote_role);
APIDECL1 t_api APIDECL2 HID_Disconnect(void);

APIDECL1 t_api APIDECL2 HID_RegisterService(t_hidOpcode type, const t_hid_attributes *attribute); // attribute must be valid during bt open.
APIDECL1 t_api APIDECL2 HID_ServiceDiscover(t_bdaddr address);

APIDECL1 t_api APIDECL2 HID_Packet_Send_Interrupt(u_int8 header, const u_int8 *data, u_int32 length,u_int8 flag);

APIDECL1 t_api APIDECL2 HID_Send_Control_Op_Unplug(HID_CONTROL_OP op);
APIDECL1 t_api APIDECL2 HID_Packet_Send_Control(u_int8 header, const u_int8 *data, u_int32 length);
APIDECL1 t_api APIDECL2 HID_Packet_Send_Interrupt_Long(u_int8 header, const u_int8 *data, u_int32 length, u_int8 flag);


//following API for LE only
APIDECL1 t_api APIDECL2 hids_start_server();

APIDECL1 t_api APIDECL2 hids_DisconnectLink();

APIDECL1 t_api APIDECL2 hids_DiscoveryName();

int gatt_update_ll_param(t_bdaddr address);

APIDECL1 t_api APIDECL2 hidp_connect(t_bdaddr address, u_int8 bootPrior);

void hids_sendInputReport(t_bdaddr address, u_int8* data, u_int16 length);

#ifdef __cplusplus
} /* extern "C"  */
#endif

#endif /*HID_H_*/

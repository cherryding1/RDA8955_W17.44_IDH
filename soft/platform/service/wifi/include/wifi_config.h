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

#include "wifi_common.h"
#define MAX_STRING_LEN      512
#define MAX_CMD_LEN         MAX_STRING_LEN
#define RDA5890_MAX_WID_LEN   (MAX_CMD_LEN - 2)

#define MAC_CONNECTED          1
#define MAC_DISCONNECTED       0

#define WID_STATUS_SUCCESS       1

/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/

/* Mac Listen Interval */
#define WIFI_LISTEN_INTERVAL                    0x06

/* Link Loss Threshold */
#define WIFI_LINK_LOSS_THRESHOLD_90                0x20
#define WIFI_LINK_LOSS_THRESHOLD_91                0x40
#define WIFI_LINK_LOSS_THRESHOLD_91H               0x40

/* Number of basic WIDS */
#define NUM0_CHAR_WID        47
#define NUM0_SHORT_WID       18
#define NUM0_INT_WID         31
#define NUM0_STR_WID         30

/* Number of WIDS for this platform */
#define NUM1_CHAR_WID        (NUM0_CHAR_WID  + 1)
#define NUM1_SHORT_WID       NUM0_SHORT_WID
#define NUM1_INT_WID         NUM0_INT_WID
#define NUM1_STR_WID         NUM0_STR_WID

/* Number of WIDS with the current configuration */
#define NUM2_CHAR_WID        NUM1_CHAR_WID
#define NUM2_SHORT_WID       NUM1_SHORT_WID
#define NUM2_INT_WID         NUM1_INT_WID
#define NUM2_STR_WID         NUM1_STR_WID

/* Final number if WIDs */
#define NUM_CHAR_WID        NUM2_CHAR_WID
#define NUM_SHORT_WID       NUM2_SHORT_WID
#define NUM_INT_WID         NUM2_INT_WID
#define NUM_STR_WID         NUM2_STR_WID
#define NUM_BIN_DATA_WID           10

#define NUM_WID             (NUM_CHAR_WID + NUM_SHORT_WID\
                           + NUM_INT_WID + NUM_STR_WID + NUM_BIN_DATA_WID)

/* Number of rates supported (maximum) */
#define NUM_RATES           12

/* Miscellaneous */
#define MSG_DATA_OFFSET           4
#define WID_VALUE_OFFSET          3
#define WID_LENGTH_OFFSET         2
#define WRITE_RSP_LEN             4
#define MAX_QRSP_LEN              1000
#define MAX_WRSP_LEN              20
#define MSG_HDR_LEN               4
#define MAX_ADDRESS_LEN           6
#define WRSP_SUCCESS              1
#define WRSP_ERR_MSG              (-1)
#define WRSP_ERR_SEQ              (-2)
#define WRSP_ERR_LEN              (-3)
#define WRSP_ERR_WID              (-4)
#define MAX_SUPRATE_LEN           34
#define MAX_PROD_VER_LEN          10
#define MAX_GRPADDR_LEN           38
#define SITE_SURVEY_ELM_LEN       (MAX_SSID_LEN + 1 + 1 + 1)
#define WID_BIN_DATA_LEN_MASK     0x3FFF


#define ACTIVE_SCAN_TIME			            10
#define PASSIVE_SCAN_TIME			            1200
#define MIN_SCAN_TIME				            10
#define MAX_SCAN_TIME				            1200
#define DEFAULT_SCAN				            0
#define USER_SCAN				            BIT0


#define MAC_CONNECTED                     1
#define MAC_DISCONNECTED                  0
#define STATUS_MSG_LEN                    8

/*  Firmware Information    */
#define FINFO_RELEASE_MODE                0x00000000
#define FINFO_DEBUG_MODE                  0x00000001
#define FINFO_WLAN_STA                    0x00000000
#define FINFO_WLAN_AP                     0x00000002
#define FINFO_MEM_EEPROM                  0x00000000
#define FINFO_MEM_FLASH                   0x00000004

#define FINFO_HOST_SDIO                   0x00000000
#define FINFO_HOST_SPI                    0x00000010
#define FINFO_HOST_UART                   0x00000020
#define FINFO_HOST_USB                    0x00000040
#define FINFO_HOST_EHPI                   0x00000080

#define FINFO_PHY_11B                     0x00000000
#define FINFO_PHY_11G                     0x00000100
#define FINFO_PHY_11A                     0x00000200

#define FINFO_11I_DISABLE                 0x00000000
#define FINFO_11I_ENABLE                  0x00001000
#define FINFO_1X_DISABLE                  0x00000000
#define FINFO_1X_ENABLE                   0x00002000

#define FINFO_UART_LOG_DISABLE            0x00000000
#define FINFO_UART_LOG_ENABLE             0x10000000

/*  UNIT_TEST Flags    */
#define UNIT_TEST_MODE_RX                 0x01
#define UNIT_TEST_MODE_TX                 0x02

enum TYPESUBTYPE_T
{
    ASSOC_REQ             = 0x00,
    ASSOC_RSP             = 0x10,
    REASSOC_REQ           = 0x20,
    REASSOC_RSP           = 0x30,
    PROBE_REQ             = 0x40,
    PROBE_RSP             = 0x50,
    BEACON                = 0x80,
    ATIM                  = 0x90,
    DISASOC               = 0xA0,
    AUTH                  = 0xB0,
    DEAUTH                = 0xC0,
    ACTION                = 0xD0,
    PS_POLL               = 0xA4,
    RTS                   = 0xB4,
    CTS                   = 0xC4,
    ACK                   = 0xD4,
    CFEND                 = 0xE4,
    CFEND_ACK             = 0xF4,
    DATA                  = 0x08,
    DATA_ACK              = 0x18,
    DATA_POLL             = 0x28,
    DATA_POLL_ACK         = 0x38,
    NULL_FRAME            = 0x48,
    CFACK                 = 0x58,
    CFPOLL                = 0x68,
    CFPOLL_ACK            = 0x78,
    QOS_DATA              = 0x88,
    QOS_DATA_ACK          = 0x98,
    QOS_DATA_POLL         = 0xA8,
    QOS_DATA_POLL_ACK     = 0xB8,
    QOS_NULL_FRAME        = 0xC8,
    QOS_CFPOLL            = 0xE8,
    QOS_CFPOLL_ACK        = 0xF8,
    BLOCKACK_REQ          = 0x84,
    BLOCKACK              = 0x94
};
/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/
//scan requester type
enum
{
    NORMAL_SCAN = 0,
    SMARTCONFIG_SCAN,
    SNIFF_SCAN
};

/* WID Data Types */
typedef enum {WID_CHAR     = 0,
              WID_SHORT    = 1,
              WID_INT      = 2,
              WID_STR      = 3,
              WID_BIN      = 4,
              WID_UNKNOWN  = 5,
             } WID_TYPE_T;

/* WLAN Identifiers */
typedef enum
{
    WID_NIL                            = -1,
    WID_BSS_TYPE                       = 0x0000,
    WID_CURRENT_TX_RATE                = 0x0001,
    WID_CURRENT_CHANNEL                = 0x0002,
    WID_PREAMBLE                       = 0x0003,
    WID_11G_OPERATING_MODE             = 0x0004,
    WID_STATUS                         = 0x0005,
    WID_11G_PROT_MECH                  = 0x0006,

#ifdef MAC_HW_UNIT_TEST_MODE
    WID_GOTO_SLEEP                     = 0x0007,
#else /* MAC_HW_UNIT_TEST_MODE */
    WID_SCAN_TYPE                      = 0x0007,
#endif /* MAC_HW_UNIT_TEST_MODE */
    WID_PRIVACY_INVOKED                = 0x0008,
    WID_KEY_ID                         = 0x0009,
    WID_QOS_ENABLE                     = 0x000A,
    WID_POWER_MANAGEMENT               = 0x000B,
    WID_802_11I_MODE                   = 0x000C,
    WID_AUTH_TYPE                      = 0x000D,
    WID_SITE_SURVEY                    = 0x000E,
    WID_LISTEN_INTERVAL                = 0x000F,
    WID_DTIM_PERIOD                    = 0x0010,
    WID_ACK_POLICY                     = 0x0011,
    WID_RESET                          = 0x0012,
    WID_PCF_MODE                       = 0x0013,
    WID_CFP_PERIOD                     = 0x0014,
    WID_BCAST_SSID                     = 0x0015,

#ifdef MAC_HW_UNIT_TEST_MODE
    WID_PHY_TEST_PATTERN               = 0x0016,
#else /* MAC_HW_UNIT_TEST_MODE */
    WID_DISCONNECT                     = 0x0016,
#endif /* MAC_HW_UNIT_TEST_MODE */

    WID_READ_ADDR_SDRAM                = 0x0017,
    WID_TX_POWER_LEVEL_11A             = 0x0018,
    WID_REKEY_POLICY                   = 0x0019,
    WID_SHORT_SLOT_ALLOWED             = 0x001A,
    WID_PHY_ACTIVE_REG                 = 0x001B,
    WID_PHY_ACTIVE_REG_VAL             = 0x001C,
    WID_TX_POWER_LEVEL_11B             = 0x001D,
    WID_START_SCAN_REQ                 = 0x001E,
    WID_RSSI                           = 0x001F,
    WID_JOIN_REQ                       = 0x0020,
    WID_ANTENNA_SELECTION              = 0x0021,
    WID_USER_CONTROL_ON_TX_POWER       = 0x0027,
    WID_MEMORY_ACCESS_8BIT             = 0x0029,
    WID_UAPSD_SUPPORT_AP               = 0x002A,

    WID_CURRENT_MAC_STATUS             = 0x0031,
    WID_AUTO_RX_SENSITIVITY            = 0x0032,
    WID_DATAFLOW_CONTROL               = 0x0033,
    WID_SCAN_FILTER                    = 0x0036,
    WID_LINK_LOSS_THRESHOLD            = 0x0037,
    WID_AUTORATE_TYPE                  = 0x0038,
    WID_CCA_THRESHOLD                  = 0x0039,

    WID_802_11H_DFS_MODE               = 0x003B,
    WID_802_11H_TPC_MODE               = 0x003C,

    WID_PHY_REG_ADDR                   = 0x0040,
    WID_PHY_REG_VAL                    = 0x0041,

    WID_PTA_MODE                       = 0x0042,
    WID_TRAP_TEST                      = 0x0043,
    WID_PTA_BLOCK_BT                   = 0x0044,
    WID_NETWORK_INFO_EN                = 0x0045,

/*
 * NMAC Character WID list
 */
    WID_11N_PROT_MECH                  = 0x0080,
    WID_11N_ERP_PROT_TYPE              = 0x0081,
    WID_11N_ENABLE                     = 0x0082,
    WID_11N_OPERATING_TYPE             = 0x0083,
    WID_11N_OBSS_NONHT_DETECTION       = 0x0084,
    WID_11N_HT_PROT_TYPE               = 0x0085,
    WID_11N_RIFS_PROT_ENABLE           = 0x0086,
    WID_11N_SMPS_MODE                  = 0x0087,
    WID_11N_CURRENT_TX_MCS             = 0x0088,
    WID_11N_PRINT_STATS                = 0x0089,
    WID_HUT_FCS_CORRUPT_MODE           = 0x008A,
    WID_HUT_RESTART                    = 0x008B,
    WID_HUT_TX_FORMAT                  = 0x008C,
    WID_11N_SHORT_GI_ENABLE            = 0x008D,
    WID_HUT_BANDWIDTH                  = 0x008E,
    WID_HUT_OP_BAND                    = 0x008F,
    WID_HUT_STBC                       = 0x0090,
    WID_HUT_ESS                        = 0x0091,
    WID_HUT_ANTSET                     = 0x0092,
    WID_HUT_HT_OP_MODE                 = 0x0093,
    WID_RIFS_MODE                      = 0x0094,
    WID_HUT_SMOOTHING_REC              = 0x0095,
    WID_HUT_SOUNDING_PKT               = 0x0096,
    WID_HUT_HT_CODING                  = 0x0097,
    WID_HUT_TEST_DIR                   = 0x0098,

    WID_11N_IMMEDIATE_BA_ENABLED       = 0x00AF,

    WID_2040_ENABLE                    = 0x00C5,

    WID_11N_MINIMUM_MPDU_START_SPACING = 0x00CB,
    WID_11N_MAX_RX_AMPDU_FACTOR        = 0x00CC,
    WID_11N_ADDBA_TID                  = 0x00CD,
    WID_11N_AMPDU_TID                  = 0x00CE,
    WID_11N_AMSDU_TID                  = 0x00CF,
    WID_USRCTL_RX_FRAME_FILTER         = 0x00D0,

    WID_BOOTROM_DECRYPT                = 0x0401,

    WID_BOOTROM_START_APP              = 0x0402,
    WID_BOOTROM_DBGA                   = 0x0403,

    WID_BOOTROM_DBGL                   = 0x0404,
    WID_AP_START_REQ                   = 0x0410,
    WID_SCAN_CONNECT_RESULT            = 0x0411,
    WID_SCAN_STOP                      = 0x0414,

    WID_MAX_CHAR_ID                    = 0x0fff,//last char id
    WID_RTS_THRESHOLD                  = 0x1000,
    WID_FRAG_THRESHOLD                 = 0x1001,
    WID_SHORT_RETRY_LIMIT              = 0x1002,
    WID_LONG_RETRY_LIMIT               = 0x1003,
    WID_CFP_MAX_DUR                    = 0x1004,
    WID_PHY_TEST_FRAME_LEN             = 0x1005,
    WID_BEACON_INTERVAL                = 0x1006,
    WID_MEMORY_ACCESS_16BIT            = 0x1008,

    WID_RX_SENSE                       = 0x100B,
    WID_ACTIVE_SCAN_TIME               = 0x100C,
    WID_PASSIVE_SCAN_TIME              = 0x100D,
    WID_SITE_SURVEY_SCAN_TIME          = 0x100E,
    WID_JOIN_TIMEOUT                   = 0x100F,
    WID_AUTH_TIMEOUT                   = 0x1010,
    WID_ASOC_TIMEOUT                   = 0x1011,
    WID_11I_PROTOCOL_TIMEOUT           = 0x1012,
    WID_EAPOL_RESPONSE_TIMEOUT         = 0x1013,
    WID_CCA_BUSY_STATUS                = 0x1014,

    WID_MAX_SHORT_ID                   = 0x1fff,//last short wid
    WID_FAILED_COUNT                   = 0x2000,
    WID_RETRY_COUNT                    = 0x2001,
    WID_MULTIPLE_RETRY_COUNT           = 0x2002,
    WID_FRAME_DUPLICATE_COUNT          = 0x2003,
    WID_ACK_FAILURE_COUNT              = 0x2004,
    WID_RECEIVED_FRAGMENT_COUNT        = 0x2005,
    WID_MULTICAST_RECEIVED_FRAME_COUNT = 0x2006,
    WID_FCS_ERROR_COUNT                = 0x2007,
    WID_SUCCESS_FRAME_COUNT            = 0x2008,
    WID_PHY_TEST_PKT_CNT               = 0x2009,
    WID_PHY_TEST_TXD_PKT_CNT           = 0x200A,
    WID_TX_FRAGMENT_COUNT              = 0x200B,
    WID_TX_MULTICAST_FRAME_COUNT       = 0x200C,
    WID_RTS_SUCCESS_COUNT              = 0x200D,
    WID_RTS_FAILURE_COUNT              = 0x200E,
    WID_WEP_UNDECRYPTABLE_COUNT        = 0x200F,
    WID_REKEY_PERIOD                   = 0x2010,
    WID_REKEY_PACKET_COUNT             = 0x2011,
#ifdef MAC_HW_UNIT_TEST_MODE
    WID_Q_ENABLE_INFO                  = 0x2012,
#else /* MAC_HW_UNIT_TEST_MODE */
    WID_802_1X_SERV_ADDR               = 0x2012,
#endif /* MAC_HW_UNIT_TEST_MODE */
    WID_STACK_IP_ADDR                  = 0x2013,
    WID_STACK_NETMASK_ADDR             = 0x2014,
    WID_HW_RX_COUNT                    = 0x2015,
    WID_MEMORY_ADDRESS                 = 0x201E,
    WID_MEMORY_ACCESS_32BIT            = 0x201F,
    WID_RF_REG_VAL                     = 0x2021,
    WID_FIRMWARE_INFO                  = 0x2023,

    WID_MEMORY_LENGTH                  = 0x2030,
    WID_CHECKSUM_TYPE                  = 0x2031,
    WID_DECRYPT_TYPE                   = 0x2032,
    WID_FILTER_TYPE                    = 0x2085,
    WID_DISCONNECT_REASON              = 0x2086,
    WID_NETWORK_EVENT_EN               = 0x2100,
    WID_MON_FILTER                     = 0x2201,
    WID_SYS_FW_VER                     = 0x2801,
    WID_SYS_DBG_LVL                    = 0x2802,
    WID_SYS_DBG_AREA                   = 0x2803,
    WID_UT_MODE                        = 0x2804,
    WID_UT_TX_LEN                      = 0x2805,
    WID_PTA_CTS_FRAME_LEN              = 0x2806,
    WID_PREASSO_SLEEP                  = 0x2807,
    WID_MAX_INTEGER_ID                 = 0x2fff,//last int id

    WID_SSID                           = 0x3000,
    WID_FIRMWARE_VERSION               = 0x3001,
    WID_OPERATIONAL_RATE_SET           = 0x3002,
    WID_BSSID                          = 0x3003,
    WID_WEP_KEY_VALUE0                 = 0x3004,
    WID_WEP_KEY_VALUE1                 = 0x3005,
    WID_WEP_KEY_VALUE2                 = 0x3006,
    WID_WEP_KEY_VALUE3                 = 0x3007,
    WID_802_11I_PSK                    = 0x3008,
    WID_HCCA_ACTION_REQ                = 0x3009,
    WID_802_1X_KEY                     = 0x300A,
    WID_HARDWARE_VERSION               = 0x300B,
    WID_MAC_ADDR                       = 0x300C,
    WID_PHY_TEST_DEST_ADDR             = 0x300D,
    WID_PHY_TEST_STATS                 = 0x300E,
    WID_PHY_VERSION                    = 0x300F,
    WID_SUPP_USERNAME                  = 0x3010,
    WID_SUPP_PASSWORD                  = 0x3011,
    WID_SITE_SURVEY_RESULTS            = 0x3012,
    WID_RX_POWER_LEVEL                 = 0x3013,

    WID_ADD_WEP_KEY                    = 0x3019,
    WID_REMOVE_WEP_KEY                 = 0x301A,
    WID_ADD_PTK                        = 0x301B,
    WID_ADD_RX_GTK                     = 0x301C,
    WID_ADD_TX_GTK                     = 0x301D,
    WID_REMOVE_KEY                     = 0x301E,
    WID_ASSOC_REQ_INFO                 = 0x301F,
    WID_ASSOC_RES_INFO                 = 0x3020,
    WID_UPDATE_RF_SUPPORTED_INFO       = 0x3021,
    WID_COUNTRY_IE                     = 0x3022,

    WID_MAX_STRING_ID                  = 0x3fff,//last string id
    WID_CONFIG_HCCA_ACTION_REQ         = 0x4000,
    WID_UAPSD_CONFIG                   = 0x4001,
    WID_UAPSD_STATUS                   = 0x4002,
    WID_WMM_AP_AC_PARAMS               = 0x4003,
    WID_WMM_STA_AC_PARAMS              = 0x4004,
    WID_NETWORK_INFO                   = 0x4005,
    WID_STA_JOIN_INFO                  = 0x4006,
    WID_CONNECTED_STA_LIST             = 0x4007,
    WID_HUT_STATS                      = 0x4082,
    WID_STATISTICS                     = 0x4008,
    WID_MEMORY_ACCESS                  = 0x4010,
    WID_BOOTROM_CHECKSUM               = 0x4101,
    WID_MAX_BINARY_ID                  = 0x4fff,
    WID_ALL                            = 0x7FFE,
    WID_MAX                            = 0xFFFF
} WID_T;


typedef enum
{

    DONT_RESET   = 0,
    DO_RESET     = 1,
    NO_REQUEST   = 2

} RESET_REQ;
/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/

typedef struct
{
    UINT16 id;    /* WID Identifier       */
    BOOL  rsp;   /* WID_ALL response     */
    BOOL  reset; /* Reset MAC required   */
    void    *get;  /* Get Function Pointer */
    void    *set;  /* Set Function Pointer */
} host_wid_struct_t;

#define MAX_WPA_IE_LEN 80

#define RDA5890_MAX_NETWORK_NUM    16
#define IW_ESSID_MAX_SIZE   32
#define ETH_ALEN 6

typedef  struct rda5890_bss_descriptor
{
    unsigned char ssid[IW_ESSID_MAX_SIZE + 1];
    unsigned char bss_type;
    unsigned char channel;
    unsigned char dot11i_info;
    unsigned char bssid[ETH_ALEN];
    char rssi;
    unsigned char auth_info;
    unsigned char rsn_cap[2];
    unsigned char    rsn_grp;
    unsigned char   rsn_pairwise[3];
    unsigned char    rsn_auth[3];

    unsigned char    wpa_grp;
    unsigned char    wpa_pairwise[3];
    unsigned char    wpa_auth[3];
} RDA5890_BSS_DESC;


#define MAX_RATES           14
#define IEEE80211_MAX_SSID_LEN  34

typedef  struct rda5890_bss_network_info
{

    u8 bssid[ETH_ALEN];

    u8 ssid[IW_ESSID_MAX_SIZE + 1];
    u8 ssid_len;

    u16 capability;
    char rssi;
    u8 channel;
    u16 beaconperiod;

    /* IW_MODE_AUTO, IW_MODE_ADHOC, IW_MODE_INFRA */
    u8 mode;

    /* zero-terminated array of supported data rates */
    u8 rates[MAX_RATES + 1];
    u8 wpa_ie[MAX_WPA_IE_LEN];
    size_t wpa_ie_len;
    u8 rsn_ie[MAX_WPA_IE_LEN];
    size_t rsn_ie_len;

    u8 wapi_ie[100];
    size_t wapi_ie_len; //wapi valid payload length
} RDA5890_BSS_NETWORK_INFO;

/**********************************************************/

#define DEV_NAME_LEN            32

#define RDA5890_MAX_NETWORK_NUM    16

/** RSSI related MACRO DEFINITIONS */
#define RDA5890_NF_DEFAULT_SCAN_VALUE       (-96)
#define PERFECT_RSSI ((u8)50)
#define WORST_RSSI   ((u8)0)
#define RSSI_DIFF    ((u8)(PERFECT_RSSI - WORST_RSSI))

/** RTS/FRAG related defines */
#define RDA5890_RTS_MIN_VALUE       0
#define RDA5890_RTS_MAX_VALUE       2347
#define RDA5890_FRAG_MIN_VALUE      256
#define RDA5890_FRAG_MAX_VALUE      2346

#define KEY_LEN_WPA_AES         16
#define KEY_LEN_WPA_TKIP        32
#define KEY_LEN_WEP_104         13
#define KEY_LEN_WEP_40          5

#define BIT7                    (1 << 7)
#define BIT6                    (1 << 6)
#define BIT5                    (1 << 5)
#define BIT4                    (1 << 4)
#define BIT3                    (1 << 3)
#define BIT2                    (1 << 2)
#define BIT1                    (1 << 1)
#define BIT0                    (1 << 0)

#define RDA_SLEEP_ENABLE        BIT0
#define RDA_SLEEP_PREASSO       BIT1

/** KEY_TYPE_ID */
enum KEY_TYPE_ID
{
    KEY_TYPE_ID_WEP = 0,
    KEY_TYPE_ID_TKIP,
    KEY_TYPE_ID_AES
};

/** KEY_INFO_WPA (applies to both TKIP and AES/CCMP) */
enum KEY_INFO_WPA
{
    KEY_INFO_WPA_MCAST = 0x01,
    KEY_INFO_WPA_UNICAST = 0x02,
    KEY_INFO_WPA_ENABLED = 0x04
};



struct ieee802_1x_hdr
{
    u8 version;
    u8 type;
    be16 length;
    /* followed by length octets of data */
} STRUCT_PACKED;

#define EAPOL_VERSION 2

enum { IEEE802_1X_TYPE_EAP_PACKET = 0,
       IEEE802_1X_TYPE_EAPOL_START = 1,
       IEEE802_1X_TYPE_EAPOL_LOGOFF = 2,
       IEEE802_1X_TYPE_EAPOL_KEY = 3,
       IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT = 4
     };

enum { EAPOL_KEY_TYPE_RC4 = 1, EAPOL_KEY_TYPE_RSN = 2,
       EAPOL_KEY_TYPE_WPA = 254
     };


struct bss_descriptor
{
    struct rda5890_bss_descriptor data;

    u8 *wpa_ie;
    size_t wpa_ie_len;
    u8 *rsn_ie;
    size_t rsn_ie_len;

    unsigned long last_scanned;

    //struct list_head list;
};

typedef enum
{
    WIFI_POWER_OFF   = 0,
    WIFI_INIT_1         = 1,
    WIFI_INIT_PRE_ASSOC       = 2,
    WIFI_INIT_ASSOCIATING       = 3,
    WIFI_INIT_DHCPING       = 4,
    WIFI_INIT_DADA       = 5,
} WIFI_STATE_T;

enum SITE_SURVEY_T {
    SITE_SURVEY_1CH = 0,
    SITE_SURVEY_ALL_CH = 1,
    SITE_SURVEY_OFF = 2,
    P2P_SITE_SURVEY_SOCIAL = 3,
};

/* Element Ids used in Management frames in 802.11i mode */
typedef enum {
    IRSNELEMENT = 48, /* RSN Information Element  */
    IWPAELEMENT = 221 /* WPA Information Element  */
} ELEMENTID_11I_T;

/* CIPHER set for RSN or WPA element  */
typedef enum {
CIPHER_TYPE_USE_GROUP_SET  = 0,
    CIPHER_TYPE_WEP40          = 1,
    CIPHER_TYPE_TKIP           = 2,
    CIPHER_TYPE_CCMP           = 4,
    CIPHER_TYPE_WEP104         = 5
} CIPHER_TYPE_T;
typedef struct
{
    UINT8 *ssid;
    UINT8 ssid_len;
    UINT8* password;
    UINT8 pwd_len;
} wifi_conn_t;

typedef struct
{
    UINT8 *ssid;
    UINT8 ssid_len;
    UINT8* password;
    UINT8 pwd_len;
} wifi_scan_resp_t;

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define OPEN_SYSTEM BIT(0)
#define SHARED_KEY BIT(1)


#define NO_ENCRYPT                       0
#define ENCRYPT_ENABLED                 BIT(0)
#define WEP                             BIT(1)
#define WEP_EXTENDED                    BIT(2)
#define WPA                             BIT(3)
#define WPA2                            BIT(4)
#define AES                             BIT(5)
#define TKIP                            BIT(6)

typedef struct
{

    UINT8 cur_ssid[33];
    UINT8 cur_pwd[64];
    UINT8 cur_bssid[6];
    UINT8 WEP0[32];
    UINT8 auth_type;
    UINT8 dot11_mode;
	UINT8 channel;
    BOOL rda5890_connected ;
    bool first_connect;


    UINT32 ioport_w;
    UINT32 ioport_r;
    UINT32 ioport  ;
    UINT8 wifiDataInd ;
    BOOL firstWrite;
    BOOL useingIrq ;
    UINT16 test_numn ;
    BOOL in_interruptHandle ;
    UINT32 tx_data_counter ;
    UINT32 rx_data_counter ;
    UINT32 transfer_start_time ;
    BOOL wifi_sleep_flag ;
    BOOL sdio_sleep_flag ;
    BOOL wifi_sleep_enable ;
    UINT8 wifi_sleep_enable_flag; //0x00: wake up; 0x01: pre-association sleep; 0x10: post-association sleep
    BOOL ap_connected ;
    BOOL ap_connecting ;
    BOOL ap_disconnecting ;
    BOOL preasso_timer_enable ;

    BOOL gettingRssi ;
    UINT32 wifi_PowerOffCount ;
    BOOL auth_failed ;
    UINT32 report_rssi_count ;


    UINT32 wifi_enableSleepTimerCount;

    UINT8 tmp_read_data[16][1664];

    UINT16 tmp_read_data_flag ;

} wifi_cxt_t;

static inline WID_T wland_get_wid_type(WID_T firmw_wid)
{
	if (firmw_wid < WID_MAX_CHAR_ID) {
		return WID_CHAR;
	} else if (firmw_wid < WID_MAX_SHORT_ID) {
		return WID_SHORT;
	} else if (firmw_wid < WID_MAX_INTEGER_ID) {
		return WID_INT;
	} else if (firmw_wid < WID_MAX_STRING_ID) {
		return WID_STR;
	} else if (firmw_wid < WID_MAX_BINARY_ID) {
		return WID_BIN;
	} else {
		return WID_UNKNOWN;
	}
}

static inline u16 wland_get_wid_size(WID_T type, u16 len)
{
	switch (type) {
	case WID_CHAR:
		return sizeof(u8);
	case WID_SHORT:
		return sizeof(u16);
	case WID_INT:
		return sizeof(u32);
	case WID_STR:
	case WID_BIN:
	case WID_UNKNOWN:
		break;
	default:
		break;
	}
	return len;
}

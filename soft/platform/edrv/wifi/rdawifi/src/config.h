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







































































































#ifndef CONFIG_H
#define CONFIG_H

/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/

#include "common.h"
#include "core_mode_if.h"
#include "itypes.h"
#include "maccontroller.h"
#include "mib.h"
#include "mh.h"
#include "phy_if.h"
#include "host_if.h"
#include "mac_ant_dvt.h"
#include "sdio_host_if.h"
#include "ehpi_host_if.h"
#include "sys_debug.h"

#ifdef IBSS_BSS_STATION_MODE
#ifdef AUTO_RX_SENSITIVITY
#include "auto_rx_sens_sta.h"
#endif  /*  AUTO_RX_SENSITIVITY  */
#endif  /*  IBSS_BSS_STATION_MODE   */

#ifdef MAC_HW_UNIT_TEST_MODE
#include "mh_test.h"
#endif /* MAC_HW_UNIT_TEST_MODE */

#ifdef OS_LINUX_CSL_TYPE
#ifdef ETHERNET_DRIVER
#include "lan_driver.h"
#endif /* ETHERNET_DRIVER */
#endif /* OS_LINUX_CSL_TYPE */


/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/

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

/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

/* WID Data Types */
typedef enum {WID_CHAR     = 0,
              WID_SHORT    = 1,
              WID_INT      = 2,
              WID_STR      = 3,
              WID_BIN_DATA = 4
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

    WID_SYS_FW_VER                     = 0x2801,
    WID_SYS_DBG_LVL                    = 0x2802,
    WID_SYS_DBG_AREA                   = 0x2803,
    WID_UT_MODE                        = 0x2804,
    WID_UT_TX_LEN                      = 0x2805,

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

    WID_CONFIG_HCCA_ACTION_REQ         = 0x4000,
    WID_UAPSD_CONFIG                   = 0x4001,
    WID_UAPSD_STATUS                   = 0x4002,
    WID_WMM_AP_AC_PARAMS               = 0x4003,
    WID_WMM_STA_AC_PARAMS              = 0x4004,
    WID_NEWORK_INFO                    = 0x4005,
    WID_STA_JOIN_INFO                  = 0x4006,
    WID_CONNECTED_STA_LIST             = 0x4007,
    WID_HUT_STATS                      = 0x4082,
    WID_STATISTICS                     = 0x4008,
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
    UWORD16 id;    /* WID Identifier       */
    BOOL_T  rsp;   /* WID_ALL response     */
    BOOL_T  reset; /* Reset MAC required   */
    void    *get;  /* Get Function Pointer */
    void    *set;  /* Set Function Pointer */
} host_wid_struct_t;

/*****************************************************************************/
/* External Global Variables                                                 */
/*****************************************************************************/

extern BOOL_T            g_reset_mac;
extern UWORD16           g_current_len;
extern UWORD8            g_current_settings[MAX_QRSP_LEN];
extern UWORD8            g_phy_ver[MAX_PROD_VER_LEN + 1];
extern RESET_REQ         g_reset_req_from_user;

extern UWORD32           g_unit_test_mode;
extern UWORD32           g_unit_test_tx_len;
extern ALARM_HANDLE_T    *g_ut_tx_alarm_handle;
extern ALARM_HANDLE_T    *g_ut_rx_alarm_handle;
extern UWORD32           g_host_test_mode;

extern host_wid_struct_t g_char_wid_struct[NUM_CHAR_WID];

extern const host_wid_struct_t g_short_wid_struct[NUM_SHORT_WID];
extern const host_wid_struct_t g_int_wid_struct[NUM_INT_WID];
extern const host_wid_struct_t g_str_wid_struct[NUM_STR_WID];
extern const host_wid_struct_t g_binary_data_wid_struct[NUM_BIN_DATA_WID];

extern UWORD8            phy_reg_val_table[];
extern UWORD8            phy_reg_val_table_2[];


/*****************************************************************************/
/* Extern Function Declarations                                              */
/*****************************************************************************/

extern UWORD16 process_query(UWORD8* wid_req, UWORD8* wid_rsp, UWORD16 len);
extern void    process_write(UWORD8* wid_req, UWORD16 req_len);
extern void    send_host_rsp(mem_handle_t *mem_handle, UWORD8 *host_rsp, UWORD16 host_rsp_len);
extern void    send_mac_status(UWORD8 mac_status);
extern void    send_wake_status(UWORD8 wake_status);
extern void    send_network_info_to_host(UWORD8 *msa, UWORD16 rx_len, signed char rssi);
extern void    send_join_leave_info_to_host(UWORD16 aid, UWORD8* sta_addr, BOOL_T joining);
extern void    set_wid(UWORD8 *wid, UWORD16 len, UWORD8 count, UWORD8 wid_type);
extern UWORD16  get_wid(UWORD8 *wid, UWORD8 count, UWORD8 wid_type);
extern void    parse_config_message(mac_struct_t *mac, UWORD8* host_req,
                                    UWORD8 *buffer_addr, BOOL_T send_write_rsp);
extern void    save_wids(void);
extern void    restore_wids(void);


extern void make_wid_rsp(UWORD8 *wid_rsp_data, UWORD8 msg_id, UWORD8 wid_massage);
/*****************************************************************************/
/* Inline Functions                                                          */
/*****************************************************************************/
INLINE UWORD32 get_enable_ch(void);

INLINE void set_reset_req(UWORD8 val)
{
    g_reset_req_from_user =(RESET_REQ) val;
}

#ifdef IBSS_BSS_STATION_MODE
INLINE UWORD8* get_assoc_req_info(void)
{
    return ( ( UWORD8* )g_assoc_req_info );
}
INLINE UWORD8* get_assoc_res_info(void)
{
    return ( ( UWORD8* )g_assoc_res_info );
}
#endif /* IBSS_BSS_STATION_MODE */

/* Set the Short Slot option                                */
/* This function needs to be here because it is used in the */
/* subsequent functions                                     */
INLINE void set_short_slot_allowed(UWORD8 val)
{
    BOOL_T short_slot_enable = BFALSE;

#ifdef PHY_802_11g
    g_short_slot_allowed = (BOOL_T) val;

    /* Disable Short Slot */
    if(g_short_slot_allowed == BFALSE)
    {
        short_slot_enable = BFALSE;
    }
    /* Enable Short Slot if applicable */
    else
    {
        /* Enable short slot if it is allowed */
        short_slot_enable = BTRUE;

#ifdef IBSS_BSS_STATION_MODE
        /* For IBSS mode, disable short slot */
        if(mget_DesiredBSSType() == INDEPENDENT)
        {
            short_slot_enable = BFALSE;
        }
#endif /* IBSS_BSS_STATION_MODE  */

#ifdef BURST_TX_MODE
        /* Short slot must be enabled in Burst tx mode */
        if (g_burst_tx_mode_enabled == BTRUE)
        {
            short_slot_enable = BTRUE;
        }
#endif /* BURST_TX_MODE */
    }
#endif /* PHY_802_11g */

#ifdef PHY_802_11a
    short_slot_enable = BTRUE;
#endif /* PHY_802_11a */

#ifdef PHY_802_11b
    short_slot_enable = BFALSE;
#endif /* PHY_802_11b */

    /* Update PHY & MAC HW registers */
    if(short_slot_enable == BTRUE)
    {
        enable_short_slot();
        set_machw_short_slot_select();
    }
    else
    {
        disable_short_slot();
        set_machw_long_slot_select();
    }
}

/* This function returns the current slot option */
INLINE UWORD8 get_short_slot_allowed(void)
{
    return g_short_slot_allowed;
}

/* This function returns the SSID in the format required by the host. */
INLINE WORD8* get_DesiredSSID(void)
{
    WORD8 *ssid_temp = mget_DesiredSSID();

    g_cfg_val[0] = strlen((const char*)ssid_temp);
    strcpy( (char *)( g_cfg_val + 1 ), (const char*)ssid_temp);

    return ( ( WORD8* )g_cfg_val );
}

/* This function returns the group address in the format required by host. */
INLINE UWORD8* get_GroupAddress(void)
{
    UWORD8 *grpa_temp = mget_GroupAddress();

    g_cfg_val[0] = grpa_temp[GROUP_ADDRESS_SIZE_OFFSET]*6;
    strcpy((char *)(g_cfg_val + 1), (const char*)grpa_temp);

    return g_cfg_val;
}

/* This function returns the product version in the format required by host. */
INLINE UWORD8* get_manufacturerProductVersion(void)
{
    UWORD8 *prod_ver_temp = mget_manufacturerProductVersion();

    g_cfg_val[0] = strlen((const char*)prod_ver_temp);
    strcpy((char *)(g_cfg_val + 1), (const char*)prod_ver_temp);

    return g_cfg_val;
}

/* This function returns the product version in the format required by host. */
INLINE UWORD8* get_hardwareProductVersion(void)
{
    UWORD32 hard_ver_temp = get_machw_pa_ver();
    UWORD8  index         = 1;
    UWORD8  temp          = 0;

    /* Format version as 'x.x.x' */
    temp = ((hard_ver_temp & 0xF0000000) >> 28) * 16 +
           ((hard_ver_temp & 0x0F000000) >> 24);
    if(temp > 9)
    {
        g_cfg_val[index++] = (UWORD8)(temp/10) + '0';
    }
    g_cfg_val[index++] = temp - (((UWORD8)(temp/10))*10) + '0';
    g_cfg_val[index++] = '.';
    temp = ((hard_ver_temp & 0x00F00000) >> 20) * 16 +
           ((hard_ver_temp & 0x000F0000) >> 16);
    if(temp > 9)
    {
        g_cfg_val[index++] = (UWORD8)(temp/10) + '0';
    }
    g_cfg_val[index++] = temp - (((UWORD8)(temp/10))*10) + '0';
    g_cfg_val[index++] = '.';
    temp = ((hard_ver_temp & 0x0000F000) >> 12) * 16 +
           ((hard_ver_temp & 0x00000F00) >> 8);
    if(temp > 9)
    {
        g_cfg_val[index++] = (UWORD8)(temp/10) + '0';
    }
    g_cfg_val[index++] = temp - (((UWORD8)(temp/10))*10) + '0';
    g_cfg_val[0] = index - 1;
    return g_cfg_val;
}


/* This function returns the product version in the format required by host. */
INLINE UWORD8* get_phyProductVersion(void)
{
    return g_phy_ver;
}

/* This function returns the BSSID in the format required by the host.*/
INLINE UWORD8* get_bssid(void)
{
    UWORD8 *bssid_temp = mget_bssid();

    g_cfg_val[0] = MAX_ADDRESS_LEN;
    memcpy(g_cfg_val + 1, bssid_temp, MAX_ADDRESS_LEN);

    return g_cfg_val;
}

/* This function returns the MAC Address in the format required by the host.*/
INLINE UWORD8* get_mac_addr(void)
{
#ifndef MAC_HW_UNIT_TEST_MODE
    UWORD8 *mac_addr_temp = mget_StationID();

    g_cfg_val[0] = MAX_ADDRESS_LEN;
    memcpy(g_cfg_val + 1, mac_addr_temp, MAX_ADDRESS_LEN);

    return g_cfg_val;
#else /* MAC_HW_UNIT_TEST_MODE */
    g_cfg_val[0] = MAX_ADDRESS_LEN;
    memcpy(&g_cfg_val[1], g_test_params.mac_address, MAX_ADDRESS_LEN);
    return g_cfg_val;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function returns the Supported Rates in the format required by host. */
INLINE UWORD8* get_supp_rates(void)
{

#ifdef PHY_802_11b
    UWORD8 *str = (UWORD8 *)"1,2,5.5,11";
    g_cfg_val[0] = strlen((const char*)str);
    memcpy(g_cfg_val + 1, str, g_cfg_val[0]);
#endif /* PHY_802_11b */

#ifdef PHY_802_11a
    UWORD8 *str = (UWORD8 *)"6,9,12,18,24,36,48,54";
    g_cfg_val[0] = strlen((const char*)str);
    memcpy(g_cfg_val + 1, str, g_cfg_val[0]);
#endif /* PHY_802_11a */

#ifdef PHY_802_11g
    UWORD8 *str = (UWORD8 *)"1,2,5.5,11,6,9,12,18,24,36,48,54";
    g_cfg_val[0] = strlen((const char*)str);
    memcpy(g_cfg_val + 1, str, g_cfg_val[0]);
#endif /* PHY_802_11g */

    return g_cfg_val;
}

/*  BSS Type formats for Host and MAC                                        */
/*  --------------------------------------------------------------------     */
/*                  Infrastructure    Independent    Access Point            */
/*  Host :                 0               1            2                    */
/*  MIB  :                 1               2            3                    */
/*  --------------------------------------------------------------------     */

/* This function returns the BSS Type in the format required by the host. */
INLINE UWORD8 get_DesiredBSSType(void)
{
    UWORD8 btype = get_operating_bss_type();

    return (btype - 1);
}

/* This function returns the BSS Type in the format required by the MAC.*/
INLINE void set_DesiredBSSType(UWORD8 bss_type)
{
#ifndef MAC_HW_UNIT_TEST_MODE
    /* Reseting MAC is not required, when rate changes in Standard modes     */
    g_char_wid_struct[1].reset = BFALSE;
    /* Reseting MAC is not required, when Preamble changes in Standard modes */
    g_char_wid_struct[3].reset = BFALSE;
#endif /* MAC_HW_UNIT_TEST_MODE */

#ifdef PHY_CONTINUOUS_TX_MODE
    if(bss_type == 3)
    {
        g_phy_continuous_tx_mode_enable = BTRUE;
        /* If Rate changes, Reset MAC is needed        */
        g_char_wid_struct[1].reset = BTRUE;
        /* If Preamble changes, Reset MAC is needed    */
        g_char_wid_struct[3].reset = BTRUE;
    }
    else
    {
        g_phy_continuous_tx_mode_enable = BFALSE;
    }
#endif /* PHY_CONTINUOUS_TX_MODE */

#ifdef BURST_TX_MODE
    if(bss_type == 4)
    {
        g_burst_tx_mode_enabled = BTRUE;
        /*If Rate changes, Reset MAC is needed        */
        g_char_wid_struct[1].reset = BTRUE;
        /* If Preamble changes, Reset MAC is needed    */
        g_char_wid_struct[3].reset = BTRUE;
    }
    else
    {
        g_burst_tx_mode_enabled = BFALSE;
    }
#endif /* BURST_TX_MODE */

#ifdef MONITOR_MODE
    if(bss_type == 5)
    {
        g_monitor_mode_enabled = BTRUE;
    }
    else
    {
        g_monitor_mode_enabled = BFALSE;
    }
#endif /* MONITOR_MODE */

#ifdef IBSS_BSS_STATION_MODE
    /* Reset BSSID if BSS type is changing from Indepen sta */
    /* to Infra sta so that no de-auth is sent in reset_mac */
    if(mget_DesiredBSSType() == INDEPENDENT)
    {
        UWORD8      temp_addr[6] = {0};
        mset_bssid(temp_addr);
    }
    /* Inform the AP if BSS type is changing from Infra STA */
    /* if it is already connected to AP                     */
    else if(mget_DesiredBSSType() == INFRASTRUCTURE)
    {
        if(g_mac.state == ENABLED)
            send_deauth_frame(mget_bssid(), (UWORD16)UNSPEC_REASON);
    }
#endif /* IBSS_BSS_STATION_MODE  */

#ifdef BSS_ACCESS_POINT_MODE
    /* AccessPoint always belongs to infrastructure BSS  */
    mset_DesiredBSSType(INFRASTRUCTURE);
#else /* BSS_ACCESS_POINT_MODE */
    /* Set the MIB parameters */
    mset_DesiredBSSType((UWORD8)(bss_type + 1));
#endif /* BSS_ACCESS_POINT_MODE */

    /* Re-set slot type after changing BSS type */
    set_short_slot_allowed(g_short_slot_allowed);
}

/*  Channel formats for Host and MAC                                         */
/*  --------------------------------------------------------------------     */
/*          CHANNEL1      CHANNEL2 ....                     CHANNEL14        */
/*  Host :         1             2                                 14        */
/*  MIB  :         0             1                                 13        */
/*  --------------------------------------------------------------------     */

/* This function returns the Channel in the format required by the host.*/
INLINE UWORD8 get_host_chnl_num(void)
{
    UWORD8 cnum = mget_CurrentChannel();
    return (cnum + 1);
}

/* This function returns the Channel in the format required by the MAC.*/
INLINE void set_mac_chnl_num(UWORD8 cnum)
{
    mset_CurrentChannel((UWORD8)(cnum - 1));
    if(g_user_control_enabled == BFALSE)
    {
        set_default_tx_power_levels();
    }

#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_params.channel = cnum -1;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/*  Rate formats for Host and MAC                                            */
/*  -----------------------------------------------------------------------  */
/*          1   2   5.5   11   6  9  12  18  24  36  48   54  Auto           */
/*  Host :  1   2     3    4   5  6   7   8   9  10  11   12  0              */
/*  MAC  :  1   2     5   11   6  9  12  18  24  36  48   54  Not supported  */
/*  -----------------------------------------------------------------------  */

/* This function returns the transmission rate to the user.*/
INLINE UWORD8 get_tx_rate(void)
{
#ifndef MAC_HW_UNIT_TEST_MODE
    if(is_autorate_enabled() == BTRUE)
        return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */

    return get_curr_tx_rate();
}

/* This function sets the transmission rate as requested by the user.*/
INLINE void set_tx_rate(UWORD8 rate)
{


#ifdef MAC_HW_UNIT_TEST_MODE
    /* No Autorating in test mode */
    if(rate ==0)
        rate = 1;

    g_test_params.tx_rate = rate;
    g_test_stats.txci = 0;
    g_test_stats.rxd.type.ack = 0;
#endif /* MAC_HW_UNIT_TEST_MODE */

    if(rate == 0)
    {
        enable_autorate();
    }
    else
    {
#ifndef MAC_HW_UNIT_TEST_MODE
        disable_autorate();
#endif /* MAC_HW_UNIT_TEST_MODE */

        set_curr_tx_rate(rate);
    }


}

/*  Preamble formats for Host and MAC                                        */
/*  -----------------------------------------------------------------------  */
/*          Long          Short                                              */
/*  Host :  1              0                                                 */
/*  MAC  :  1              0                                                 */
/*  -----------------------------------------------------------------------  */

/* This function returns the preamble type as set by the user.*/
INLINE UWORD8 get_preamble_type(void)
{
    //Whether auto or long only
    return g_preamble_type;
}

/* This function sets the preamble type as requested by the user.*/
INLINE void set_preamble_type(UWORD8 idx)
{
    g_preamble_type = idx;

#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_params.preamble = idx;
#endif /* MAC_HW_UNIT_TEST_MODE */

    /* Value 1 means only Long Premble is allowed */
    if(1 == idx)
    {
        set_preamble(G_LONG_PREAMBLE);
    }
    /* and any other value is considered as Auto Preamble */
    else
    {
        set_preamble(G_AUTO_PREAMBLE);
    }
}

/* This function sets the RTS threshold as requested by the user. The MAC    */
/* H/w register is also set accordingly.                                     */
INLINE void set_RTSThreshold(UWORD16 limit)
{
    mset_RTSThreshold(limit);
    set_machw_rts_thresh(limit);
}

/* This function sets the Fragmentation threshold as requested by the user.  */
/* The MAC H/w register is also set accordingly.                             */
INLINE void set_FragmentationThreshold(UWORD16 limit)
{
    /* Fragmentation threshold must be even as per standard */
    if((limit % 2) != 0)
        limit -= 1;

    mset_FragmentationThreshold(limit);
    set_machw_frag_thresh(limit);
}

/* This function sets the Short Retry Limit as requested by the user. The    */
/* MAC H/w register is also set accordingly.                                 */
INLINE void set_ShortRetryLimit(UWORD8 limit)
{
    mset_ShortRetryLimit(limit);
    set_machw_srl(mget_ShortRetryLimit());
}

/* This function sets the Long Retry Limit as requested by the user. The     */
/* MAC H/w register is also set accordingly.                                 */
INLINE void set_LongRetryLimit(UWORD8 limit)
{
    mset_LongRetryLimit(limit);
    set_machw_lrl(mget_LongRetryLimit());
}

/* This function returns the current protection mode */
INLINE UWORD8 get_protection_mode(void)
{
#ifndef MAC_HW_UNIT_TEST_MODE
    if(is_autoprot_enabled() == BTRUE)
        return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */

    return get_protection();
}

/* This function sets the protection mode */
INLINE void set_protection_mode(UWORD8 prot_mode)
{

#ifndef MAC_HW_UNIT_TEST_MODE
    if(prot_mode == 0)
    {
        enable_autoprot();
    }
    else
    {
        disable_autoprot();
#else /* MAC_HW_UNIT_TEST_MODE */
    {
        g_test_config.tx.prot_type = prot_mode;
#endif /* MAC_HW_UNIT_TEST_MODE */
        set_protection(prot_mode);
    }
}

/* Get the current value of power management mode */
INLINE UWORD8 get_PowerManagementMode(void)
{
#ifdef BSS_ACCESS_POINT_MODE
    /* Return for AP mode */
    return 0;
#else /* BSS_ACCESS_POINT_MODE */
    /* Do for both STA mode MDUT mode */
    if(mget_PowerManagementMode() == MIB_POWERSAVE)
    {
        return get_user_ps_mode();
    }
    else
    {
        return (NO_POWERSAVE);
    }
#endif /* BSS_ACCESS_POINT_MODE */
}

/* Set the power management mode according to value */
INLINE void set_PowerManagementMode(UWORD8 val)
{
#ifdef BSS_ACCESS_POINT_MODE
    /* Return for AP mode */
    return ;
#else /* BSS_ACCESS_POINT_MODE */
    /* Do for both STA mode MDUT mode */
    if(val != NO_POWERSAVE)
    {
        set_user_ps_mode(val);
        mset_PowerManagementMode(MIB_POWERSAVE);
    }
    else
    {
        mset_PowerManagementMode(MIB_ACTIVE);
    }
#endif /* BSS_ACCESS_POINT_MODE */
}


/* Get the current value of ack policy type */
INLINE UWORD8 get_ack_type(void)
{
    return g_ack_policy;
}

/* Set the value of the Ack Policy to the value */
INLINE void set_ack_type(UWORD8 val)
{
    g_ack_policy = val;
}

/* Get the current value of PCF mode supported */
INLINE UWORD8 get_pcf_mode(void)
{
    return g_pcf_mode;
}

/* Set the value of the of PCF mode to the value */
INLINE void set_pcf_mode(UWORD8 val)
{
    g_pcf_mode = val;
}

/* Get the current value of PCF mode supported */
INLINE UWORD8 get_bcst_ssid(void)
{
    return g_bcst_ssid;
}

/* Set the value of the of PCF mode to the value */
INLINE void set_bcst_ssid(UWORD8 val)
{
    g_bcst_ssid = val;
}

INLINE UWORD8 get_phy_reg_addr(void)
{
    return g_phy_reg_addr;
}

INLINE void set_phy_reg_addr(UWORD8 val)
{
    g_phy_reg_addr = val;
}

INLINE UWORD8 get_phy_reg_val(void)
{
    UWORD8 val;
    read_phy_reg(g_phy_reg_addr, &val);
    return val;
}

INLINE void set_phy_reg_val(UWORD8 val)
{
    write_phy_reg(g_phy_reg_addr, val);
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE UWORD16 get_phy_test_frame_len(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE
#ifndef MAC_WMM
    return g_test_config.tx.nor.frame_len;
#else /* MAC_WMM */
    return g_test_config.tx.ac_vo.frame_len;
#endif /* MAC_WMM */
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */

}

INLINE void set_phy_test_frame_len(UWORD16 val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_stats.txci = 0;
    g_test_stats.rxd.type.ack = 0;
#ifndef MAC_WMM
    g_test_config.tx.nor.frame_len = val;
#else /* MAC_WMM */
    g_test_config.tx.ac_vo.frame_len = val;
#endif /* MAC_WMM */
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE UWORD8* get_phy_test_dst_addr(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_cfg_val[0] = MAX_ADDRESS_LEN;
    memcpy(&g_cfg_val[1], g_test_config.tx.da, MAX_ADDRESS_LEN);
    return g_cfg_val;
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_phy_test_dst_addr(UWORD8* val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    memcpy(g_test_config.tx.da, val, MAX_ADDRESS_LEN);
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE UWORD32 get_q_enable_info(UWORD32 val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    return (UWORD32) g_test_config.tx.q_enable_info;
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_q_enable_info(UWORD32 val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_config.tx.q_enable_info = val;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE UWORD32 get_phy_test_pkt_cnt(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    return g_test_config.tx.num_pkts;
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_phy_test_pkt_cnt(UWORD32 val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_config.tx.num_pkts = val;
    g_test_stats.txci = 0;
    g_test_stats.rxd.type.ack = 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE UWORD32 get_phy_test_txd_pkt_cnt(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    return g_test_stats.txci;
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_phy_test_txd_pkt_cnt(UWORD32 val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_stats.txci = 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE UWORD8 get_phy_test_pattern(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    return g_test_config.tx.pattern_type;
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_phy_test_pattern(UWORD8 val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_test_config.tx.pattern_type = val;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function gets the current test statistics. To interpret these stats at */
/* the host, structure test_stats_struct_t should be the same at the host and  */
/* device side.                                                                */
INLINE UWORD8* get_hut_stats(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE

    UWORD8 trigger_stat_lsb = 0;
    UWORD8 trigger_stat_msb = 0;
    UWORD16 stats_size      = 0;

    read_phy_reg(rAGCTRIGSTATLSB, &trigger_stat_lsb);
    read_phy_reg(rAGCTRIGSTATMSB, &trigger_stat_msb);
    g_test_stats.rxd.agc_trig_stats = (trigger_stat_msb << 8) + trigger_stat_lsb;

    stats_size = MIN(sizeof(test_stats_struct_t),(MAX_STATS_BUFFER_LEN - 1));

    g_stats_buf[0] = stats_size & 0x00FF;
    g_stats_buf[1] = (stats_size & 0xFF00) >> 8;

    memcpy(&g_stats_buf[2], &g_test_stats, stats_size);

    return g_stats_buf;
#else  /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function clears the current test statistics. */
INLINE void set_hut_stats(UWORD8* val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    memset(&g_test_stats, 0, sizeof(test_stats_struct_t));
#endif /* MAC_HW_UNIT_TEST_MODE */
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE UWORD8* get_phy_test_stats(void)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    g_cfg_val[0] = MIN(sizeof(test_stats_struct_t),(MAX_CFG_LEN - 1));
    memcpy(&g_cfg_val[1], &g_test_stats, g_cfg_val[0]);
    return g_cfg_val;
#else /* MAC_HW_UNIT_TEST_MODE */
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_phy_test_stats(UWORD8* val)
{

#ifdef MAC_HW_UNIT_TEST_MODE
    /* Reset & start the statistics counter*/
    write_phy_reg(rPHYRFCNTRL3, (PHYRFCNTRL3 | 0x03));
    write_phy_reg(rPHYRFCNTRL3, PHYRFCNTRL3);
#endif /* MAC_HW_UNIT_TEST_MODE */


    /* Print statistics if string entered is 'Px' */
    if((val[0] == 'P') || (val[0] == 'p'))
    {
#ifdef OS_LINUX_CSL_TYPE
#ifdef MAC_HW_UNIT_TEST_MODE
        print_test_stats(val[1] - '0');
        return;
#endif /* MAC_HW_UNIT_TEST_MODE */
#endif /* OS_LINUX_CSL_TYPE */

#ifdef DOT11H_TEST
        if(val[1] == '0')
        {
#ifdef BSS_ACCESS_POINT_MODE
            print_channel_table();
#endif /* BSS_ACCESS_POINT_MODE */

#ifdef IBSS_BSS_STATION_MODE
#ifdef DEBUG_PRINT
            debug_print("TSF H,L : %x,%x\n",rMAC_TSF_TIMER_HI,rMAC_TSF_TIMER_LO);
#endif /* DEBUG_PRINT */
#endif /* IBSS_BSS_STATION_MODE */

        }
        else if(val[1] == '1')
        {
            post_radar_detect_event();
        }
        return;
#endif /* DOT11H_TEST */

#ifdef DEBUG_PRINT
        print_debug_stats(val[1] - '0');
#endif /* DEBUG_PRINT */

    }
    else
    {
#ifdef MAC_HW_UNIT_TEST_MODE
        memset(&g_test_stats, 0, sizeof(test_stats_struct_t));
#endif /* MAC_HW_UNIT_TEST_MODE */
    }
}

/* Get/Set the read MAC Address from the SDRAM */
INLINE void set_read_addr_sdram(UWORD8 val)
{
    g_addr_sdram = val;
}

INLINE UWORD8 get_read_addr_sdram(void)
{
    return g_addr_sdram;
}

/* Get/Set the Tx Power Level */
INLINE void set_tx_power_level_11a(UWORD8 val)
{
    if(g_user_control_enabled == BTRUE)
    {
        g_tx_power_level_11a = val;
    }
    else
    {
        {
            g_tx_power_level_11a = g_tx_power_11a[mget_CurrentChannel()];
        }
    }

    /* valid power register values are from 0 to 63 */
    /* Take the default power for any invalid value */
    if(g_tx_power_level_11a > 63)
    {
        g_tx_power_level_11a = DEFAULT_TX_POWER;
    }

    phy_reg_val_table[rTXPOWER11A] = g_tx_power_level_11a;

    if(g_reset_mac_in_progress == BFALSE)
    {
        if(is_phy_spi_locked() == BTRUE)
            set_tx_power_11a_after_spi_lock(g_tx_power_level_11a);
        else
            write_phy_reg(rTXPOWER11A, g_tx_power_level_11a);

#ifdef MAC_HW_UNIT_TEST_MODE
        g_test_params.tx_power_11a = g_tx_power_level_11a;
#endif /* MAC_HW_UNIT_TEST_MODE */

        set_tx_power_rf(g_tx_power_level_11a);
    }
}

INLINE UWORD8 get_tx_power_level_11a(void)
{
    return g_tx_power_level_11a;
}

/* Set the new DTIM Period */
INLINE void set_dtim_period(UWORD8 val)
{
    mset_DTIMPeriod(val);
    init_dtim_period(val);
}

/* Set the local Stack IP Address */
INLINE void set_stack_ip_addr(UWORD32 val)
{
    g_src_ip_addr[0] = val & 0x000000FF;
    g_src_ip_addr[1] = (val & 0x0000FF00) >> 8;
    g_src_ip_addr[2] = (val & 0x00FF0000) >> 16;
    g_src_ip_addr[3] = (val & 0xFF000000) >> 24;
#ifdef OS_LINUX_CSL_TYPE
#ifdef ETHERNET_DRIVER
    update_itt_ip_addr(g_src_ip_addr);
#endif /* ETHERNET_DRIVER */
#endif /* OS_LINUX_CSL_TYPE */
}

INLINE UWORD32 get_stack_ip_addr(void)
{
    UWORD32 temp = 0;
    temp = g_src_ip_addr[0] |
           ((g_src_ip_addr[1] << 8) & 0x0000FF00) |
           ((g_src_ip_addr[2] << 16) & 0x00FF0000) |
           ((g_src_ip_addr[3] << 24) & 0xFF000000);
    return temp;
}


/* Set the local Stack netmask */
INLINE void set_stack_netmask_addr(UWORD32 val)
{
    g_src_netmask_addr[0] = val & 0x000000FF;
    g_src_netmask_addr[1] = (val & 0x0000FF00) >> 8;
    g_src_netmask_addr[2] = (val & 0x00FF0000) >> 16;
    g_src_netmask_addr[3] = (val & 0xFF000000) >> 24;
#ifdef OS_LINUX_CSL_TYPE
#ifdef ETHERNET_DRIVER
    update_itt_netmask_addr(g_src_netmask_addr);
#endif /* ETHERNET_DRIVER */
#endif /* OS_LINUX_CSL_TYPE */
}

INLINE UWORD32 get_stack_netmask_addr(void)
{
    UWORD32 temp = 0;
    temp = g_src_netmask_addr[0] |
           ((g_src_netmask_addr[1] << 8) & 0x0000FF00) |
           ((g_src_netmask_addr[2] << 16) & 0x00FF0000) |
           ((g_src_netmask_addr[3] << 24) & 0xFF000000);
    return temp;
}

/* Set Phy active Register */
INLINE void set_phy_active_reg(UWORD8 val)
{
    g_phy_active_reg = val;
}

/* Set Phy active Register */
INLINE UWORD8 get_phy_active_reg(UWORD8 val)
{
    return g_phy_active_reg;
}

/* Set Phy active Register value */
INLINE UWORD8 get_phy_active_reg_val(void)
{
    return phy_reg_val_table[g_phy_active_reg];
}

INLINE void set_phy_active_reg_val(UWORD8 val)
{
    write_phy_reg(g_phy_active_reg, val);
    update_phy_init_table(g_phy_active_reg, val);
}

INLINE void set_rf_reg_val(UWORD32 val)
{
    UWORD32 addr  = val & 0xF;
    UWORD32 value = (val & 0x000FFFF0) >> 4;
    write_RF_reg(addr, value);
}

/* Get/Set the Tx Power Level */
INLINE void set_tx_power_level_11b(UWORD8 val)
{
    if(g_user_control_enabled == BTRUE)
    {
        g_tx_power_level_11b = val;
    }
    else
    {
        g_tx_power_level_11b = g_tx_power_11b[mget_CurrentChannel()];
    }

    /* valid power register values are from 0 to 63 */
    /* Take the default power for any invalid value */
    if(g_tx_power_level_11b > 63)
    {
        g_tx_power_level_11b = DEFAULT_TX_POWER - g_reg_val_diff;
    }
    phy_reg_val_table[rTXPOWER11B] = g_tx_power_level_11b;

    if(g_reset_mac_in_progress == BFALSE)
    {
        if(is_phy_spi_locked() == BTRUE)
            set_tx_power_11b_after_spi_lock(g_tx_power_level_11b);
        else
            write_phy_reg(rTXPOWER11B, g_tx_power_level_11b);

#ifdef MAC_HW_UNIT_TEST_MODE
        g_test_params.tx_power_11b = g_tx_power_level_11b;
#endif /* MAC_HW_UNIT_TEST_MODE */

        set_tx_power_rf(g_tx_power_level_11b);
    }
}

INLINE UWORD8 get_tx_power_level_11b(void)
{
    return g_tx_power_level_11b;
}

/* Get the RSSI */
INLINE WORD8 get_rssi(void)
{
    return g_rssi;
}


/* Get the Current MAC Status */
INLINE UWORD8 get_current_mac_status(void)
{
    return g_current_mac_status;
}

INLINE UWORD8  get_auto_rx_sensitivity(void)
{
#ifdef IBSS_BSS_STATION_MODE
#ifdef  AUTO_RX_SENSITIVITY
    return g_auto_rx_sensitivity;
#else  /*  AUTO_RX_SENSITIVITY  */
    return 0;
#endif  /*  AUTO_RX_SENSITIVITY  */
#else   /*  IBSS_BSS_STATION_MODE   */
    return 0;
#endif  /*  IBSS_BSS_STATION_MODE   */
}

INLINE void  set_auto_rx_sensitivity(UWORD8 input)
{
#ifdef IBSS_BSS_STATION_MODE
#ifdef  AUTO_RX_SENSITIVITY

    g_auto_rx_sensitivity = input;
    auto_rx_sensitivity_init();

#endif  /*  AUTO_RX_SENSITIVITY  */
#endif  /*  IBSS_BSS_STATION_MODE   */
}

INLINE UWORD8  get_dataflow_control(void)
{
    return g_data_flow_control;
}

INLINE void  set_dataflow_control(UWORD8 input)
{
    /* enable or disable HW buffer based ack depending on previous setting */
    if(input & WLAN2HOST_FLOW_CONTROL)
    {
        if(!(g_data_flow_control & WLAN2HOST_FLOW_CONTROL))
        {
            enable_rxbuf_based_ack();
        }
    }
    else
    {
        if(g_data_flow_control & WLAN2HOST_FLOW_CONTROL)
        {
            disable_rxbuf_based_ack();
        }
    }

    g_data_flow_control = input;
}

INLINE UWORD8 get_scan_filter(void)
{
#ifdef IBSS_BSS_STATION_MODE
    return g_scan_filter;
#else  /*  IBSS_BSS_STATION_MODE   */
    return 0;
#endif  /*  IBSS_BSS_STATION_MODE   */
}

INLINE void set_scan_filter(UWORD8 input)
{
#ifdef IBSS_BSS_STATION_MODE
    g_scan_filter = input;
#endif  /*  IBSS_BSS_STATION_MODE   */
}

INLINE UWORD8 get_link_loss_threshold(void)
{
#ifdef IBSS_BSS_STATION_MODE
    return g_link_loss_threshold;
#endif  /*  IBSS_BSS_STATION_MODE   */

#ifdef BSS_ACCESS_POINT_MODE
    return g_aging_thresh_in_sec;
#endif /* BSS_ACCESS_POINT_MODE */

#ifdef MAC_HW_UNIT_TEST_MODE
    return 0;
#endif /* MAC_HW_UNIT_TEST_MODE */
}

INLINE void set_link_loss_threshold(UWORD8 input)
{
#ifdef IBSS_BSS_STATION_MODE
    if( MIN_LINK_LOSS_THRESHOLD > input )
    {
        g_link_loss_threshold = MIN_LINK_LOSS_THRESHOLD;
    }
    else
    {
        g_link_loss_threshold = input;
    }
#endif  /*  IBSS_BSS_STATION_MODE   */

#ifdef BSS_ACCESS_POINT_MODE
    g_aging_thresh_in_sec = input;
#endif /* BSS_ACCESS_POINT_MODE */
}

INLINE UWORD8 get_autorate_type(void)
{
    return g_autorate_type;
}

INLINE void set_autorate_type(UWORD8 input)
{
    g_autorate_type = input;
}

INLINE UWORD8 get_cca_threshold(void)
{
    return phy_reg_val_table[rPHYCCAMACTH1];
}

INLINE void set_cca_threshold(UWORD8 input)
{
    phy_reg_val_table[rPHYCCAMACTH1] = input;
    phy_reg_val_table[rPHYCCAMACTH2] = input;
}

INLINE UWORD32 get_enable_ch(void)
{
    return 0;
}

INLINE void set_enable_ch(UWORD32 input)
{

}

/* This function returns the antenna selected */
INLINE UWORD8 get_antenna_selection(void)
{
    if(g_ant_dvt_enable == BTRUE)
        return DIVERSITY;

    return get_current_antenna();
}

/* This function sets the antenna to be used */
INLINE void set_antenna_selection(UWORD8 mode)
{
    if(mode == DIVERSITY)
    {
        g_ant_dvt_enable = BTRUE;
        /* Initialize antenna diversity algrithm parameters */
        init_dvt();
    }
    else
    {
        g_ant_dvt_enable = BFALSE;
        /* Enable FCS fail packet filtering in MAC H/W */
        enable_machw_fcs_fail_filter();
        cancel_dvt_timer(g_diversity_timer);
        set_current_antenna(mode);
    }
}

INLINE void set_FailedCount_to_zero(UWORD32 val)
{
    mset_FailedCount(0);
}

INLINE void set_RetryCount_to_zero(UWORD32 val)
{
    mset_RetryCount(0);
}

INLINE void set_MultipleRetryCount_to_zero(UWORD32 val)
{
    mset_MultipleRetryCount(0);
}

INLINE void set_FrameDuplicateCount_to_zero(UWORD32 val)
{
    mset_FrameDuplicateCount(0);
    /* Reset the value of Duplicate count register */
    set_dup_count(0);
}

INLINE void set_ACKFailureCount_to_zero(UWORD32 val)
{
    mset_ACKFailureCount(0);
}

INLINE void set_ReceivedFragmentCount_to_zero(UWORD32 val)
{
    mset_ReceivedFragmentCount(0);
}

INLINE void set_MulticastReceivedFrameCount_to_zero(UWORD32 val)
{
    mset_MulticastReceivedFrameCount(0);
}

INLINE void set_FCSErrorCount_to_zero(UWORD32 val)
{
    mset_FCSErrorCount(0);
    /* Reset the value of FCS error count register */
    set_fcs_count(0);
}

INLINE void set_TransmittedFrameCount_to_zero(UWORD32 val)
{
    mset_TransmittedFrameCount(0);
}

INLINE void set_config_HCCA_actionReq(UWORD8 *req)
{

}

INLINE UWORD8 get_user_control_enabled(void)
{
    return g_user_control_enabled;
}

INLINE void set_user_control_enabled(UWORD8 val)
{
    g_user_control_enabled = (BOOL_T) val;
    if(g_reset_mac_in_progress == BFALSE)
    {
        if(g_user_control_enabled == BFALSE)
        {
            set_default_tx_power_levels();
        }
    }
}
INLINE UWORD32 get_firmware_info(void)
{
    UWORD32  ret = 0;

#ifdef DEBUG_MODE
    ret |= FINFO_DEBUG_MODE;
#else /* DEBUG_MODE */
    ret |= FINFO_RELEASE_MODE;
#endif /* DEBUG_MODE */

#ifdef IBSS_BSS_STATION_MODE
    ret |= FINFO_WLAN_STA;
#endif /* IBSS_BSS_STATION_MODE */

#ifdef BSS_ACCESS_POINT_MODE
    ret |= FINFO_WLAN_AP;
#endif /* BSS_ACCESS_POINT_MODE */


#ifdef USB_HOST
    ret |= FINFO_HOST_USB;
#endif /* USB_HOST */

#ifdef UART_HOST
    ret |= FINFO_HOST_UART;
#endif /* UART_HOST */

#ifdef SDIO_HOST
    ret |= FINFO_HOST_SDIO;
#endif /* SDIO_HOST */

#ifdef EHPI_HOST
    ret |= FINFO_HOST_EHPI;
#endif /* EHPI_HOST */

#ifdef PHY_802_11a
    ret |= FINFO_PHY_11A;
#endif /* PHY_802_11a */

#ifdef PHY_802_11b
    ret |= FINFO_PHY_11B;
#endif /* PHY_802_11b */

#ifdef PHY_802_11g
    ret |= FINFO_PHY_11G;
#endif /* PHY_802_11g */

#ifdef MAC_802_11I
    ret |= FINFO_11I_ENABLE;
#else /* MAC_802_11I */
    ret |= FINFO_11I_DISABLE;
#endif /* MAC_802_11I */

#ifdef MAC_802_1X
    ret |= FINFO_1X_ENABLE;
#else /* MAC_802_1X */
    ret |= FINFO_1X_DISABLE;
#endif /* MAC_802_1X */

    ret |= FINFO_UART_LOG_DISABLE;

    return ret;
}
INLINE void set_memory_address(UWORD32 input)
{
    g_memory_address = input;
}
INLINE UWORD32 get_memory_address(void)
{
    return g_memory_address;
}
INLINE UWORD8 get_memory_access_8bit(void)
{
    if(g_reset_mac_in_progress == BFALSE)
    {
        return *( ( UWORD8 * ) g_memory_address );
    }
    else
    {
        return 0;
    }
}
INLINE void set_memory_access_8bit(UWORD8 input)
{
    if(g_reset_mac_in_progress == BFALSE)
    {
        *( ( UWORD8 * ) g_memory_address ) = input;
    }
}
INLINE UWORD16 get_memory_access_16bit(void)
{
    if(g_reset_mac_in_progress == BFALSE)
    {
        return *( ( UWORD16 * ) g_memory_address );
    }
    else
    {
        return 0;
    }
}
INLINE void set_memory_access_16bit(UWORD16 input)
{
    if(g_reset_mac_in_progress == BFALSE)
    {
        *( ( UWORD16 * ) g_memory_address ) = input;
    }
}
INLINE UWORD32 get_memory_access_32bit(void)
{
    if(g_reset_mac_in_progress == BFALSE)
    {
        return *( ( UWORD32 * ) g_memory_address );
    }
    else
    {
        return 0;
    }
}
INLINE void set_memory_access_32bit(UWORD32 input)
{
    if(g_reset_mac_in_progress == BFALSE)
    {
        *( ( UWORD32 * ) g_memory_address ) = input;
    }
}

/* This function sets the MAC Address required for the MH/PHY Test.*/
INLINE void set_mac_addr(UWORD8 *val)
{
#ifdef MAC_HW_UNIT_TEST_MODE
    memcpy(g_test_params.mac_address, val, MAX_ADDRESS_LEN);
    mset_StationID(val);
#endif /* MAC_HW_UNIT_TEST_MODE */

    /* Set the MAC address from user */
    memcpy( mac_address, val, 6 );
    mset_StationID( val );
}


/*
  Rx sensitivity parameter format (WID view)
   +----------------------+----------------------+
   |   PHYAGCMAXVGAGAIN   | PHYAGCFINALVGAGAINTH |
   +----------------------+----------------------+
    7                    0 15                   8 (bit)
*/
INLINE UWORD16 get_rx_sense(void)
{
    return (((UWORD16)phy_reg_val_table[rPHYAGCFINALVGAGAINTH] << 8) & 0xFF00) |
           (((UWORD16)phy_reg_val_table[rPHYAGCMAXVGAGAIN]     << 0) & 0x00FF);
}

INLINE void set_rx_sense(UWORD16 val)
{
    phy_reg_val_table[rPHYAGCFINALVGAGAINTH] = (UWORD8)((val >> 8) & 0x00FF);
    phy_reg_val_table[rPHYAGCMAXVGAGAIN]     = (UWORD8)((val >> 0) & 0x00FF);
}

INLINE UWORD16 get_active_scan_time(void)
{
#ifdef IBSS_BSS_STATION_MODE
    return g_active_scan_time;
#else /* IBSS_BSS_STATION_MODE */
    return 0;
#endif /* IBSS_BSS_STATION_MODE */
}

INLINE void set_active_scan_time(UWORD16 val)
{
#ifdef IBSS_BSS_STATION_MODE
    g_active_scan_time = MAX(MIN(val,MAX_SCAN_TIME),MIN_SCAN_TIME);
#endif /* IBSS_BSS_STATION_MODE */
}

INLINE UWORD16 get_passive_scan_time(void)
{
#ifdef IBSS_BSS_STATION_MODE
    return g_passive_scan_time;
#else /* IBSS_BSS_STATION_MODE */
    return 0;
#endif /* IBSS_BSS_STATION_MODE */
}

INLINE void set_passive_scan_time(UWORD16 val)
{
#ifdef IBSS_BSS_STATION_MODE
    g_passive_scan_time = MAX(MIN(val,MAX_SCAN_TIME),MIN_SCAN_TIME);
#endif /* IBSS_BSS_STATION_MODE */
}

INLINE UWORD16 get_site_survey_scan_time(void)
{
#ifdef IBSS_BSS_STATION_MODE
    return g_site_survey_scan_time;
#else /* IBSS_BSS_STATION_MODE */
    return 0;
#endif /* IBSS_BSS_STATION_MODE */
}

INLINE void set_site_survey_scan_time(UWORD16 val)
{
#ifdef IBSS_BSS_STATION_MODE
    g_site_survey_scan_time = MAX(MIN(val,MAX_SCAN_TIME),MIN_SCAN_TIME);
#endif /* IBSS_BSS_STATION_MODE */
}

INLINE UWORD16 get_join_timeout(void)
{
    return g_join_timeout;
}

INLINE void set_join_timeout(UWORD16 val)
{
    g_join_timeout = val;
}

INLINE UWORD16 get_auth_timeout(void)
{
    return (UWORD16)mget_AuthenticationResponseTimeOut();
}

INLINE void set_auth_timeout(UWORD16 val)
{
    mset_AuthenticationResponseTimeOut( (UWORD32)val );
}

INLINE UWORD16 get_asoc_timeout(void)
{
    return (UWORD16)mget_AssociationResponseTimeOut();
}

INLINE void set_asoc_timeout(UWORD16 val)
{
    mset_AssociationResponseTimeOut((UWORD32)val);
}

INLINE UWORD16 get_11i_protocol_timeout(void)
{
    return g_11i_protocol_timeout;
}

INLINE void set_11i_protocol_timeout(UWORD16 val)
{
    g_11i_protocol_timeout = val;
}

INLINE UWORD16 get_eapol_response_timeout(void)
{

    return g_eapol_response_timeout;
}

INLINE void set_eapol_response_timeout(UWORD16 val)
{
    g_eapol_response_timeout = val;
}

INLINE UWORD16 get_cca_busy_status(void)
{
    return 0;
}

INLINE void set_cca_busy_start(UWORD16 val)
{
}

/* Get the current value of site servey */
INLINE UWORD8 get_site_survey_status(void)
{
#ifdef IBSS_BSS_STATION_MODE
    return (UWORD8)g_site_survey_enabled;
#else  /* IBSS_BSS_STATION_MODE */
    return 0;
#endif /* IBSS_BSS_STATION_MODE */
}

/* Set the site survey option according to the value */
INLINE void set_site_survey(UWORD8 val)
{
#ifdef IBSS_BSS_STATION_MODE
    g_site_survey_enabled = (SITE_SURVEY_T)val;

    /* Workaround for Existing configurators */
    if(g_site_survey_enabled == 0)
    {
        g_site_survey_enabled = SITE_SURVEY_OFF;
    }

    if( g_site_survey_enabled == SITE_SURVEY_OFF )
    {
        g_reset_mac = BTRUE;
    }
#endif /* IBSS_BSS_STATION_MODE */

#ifdef BSS_ACCESS_POINT_MODE
    return;
#endif /* BSS_ACCESS_POINT_MODE */
}


INLINE void set_preferred_bssid(UWORD8 *val)
{
#ifdef IBSS_BSS_STATION_MODE
    memcpy(g_prefered_bssid, val, 6);
#endif /* IBSS_BSS_STATION_MODE */
}
/* Function to set g_sta_uapsd_config value */
INLINE void set_uapsd_config(UWORD8 *val)
{
#ifdef MAC_WMM
#ifdef IBSS_BSS_STATION_MODE
    /* First 2 bytes are WID Data length */
    val = val + 2;

    /* --------------------------------------------------------------------- */
    /* Format of U-APSD config packet                 */
    /* --------------------------------------------------------------------- */
    /* | MAX SP   | Trigger/Delivery enabled status | */
    /* | Length   |  AC_BK | AC_BE | AC_VI | AC_VO  | */
    /* --------------------------------------------------------------------- */
    /* |   1      |  1     | 1     | 1     | 1      | */
    /* --------------------------------------------------------------------- */
    set_uapsd_config_max_sp_len(val[0]);
    set_uapsd_config_ac(AC_BK, val[1]);
    set_uapsd_config_ac(AC_BE, val[2]);
    set_uapsd_config_ac(AC_VI, val[3]);
    set_uapsd_config_ac(AC_VO, val[4]);
#endif /* IBSS_BSS_STATION_MODE */
#endif /* MAC_WMM */
}

/* Function to set g_sta_uapsd_config value */
INLINE UWORD8* get_uapsd_config(void)
{
#ifdef MAC_WMM
#ifdef IBSS_BSS_STATION_MODE
    /* ---------------------------------------------- */
    /* Format of U-APSD config message                */
    /* ---------------------------------------------- */
    /* | MAX SP   | Trigger/Delivery enabled status | */
    /* | Length   |  AC_BK | AC_BE | AC_VI | AC_VO  | */
    /* ---------------------------------------------- */
    /* |   1      |  1     | 1     | 1     | 1      | */
    /* ---------------------------------------------- */
    UWORD16 len  = 5; /* Length of U-APSD message */

    /* First 2 Bytes are length field and rest of the bytes are actual data  */
    /* Length field encoding */
    /* +----------------------------------+ */
    /* | BIT15  | BIT14  | BIT13 - BIT0   | */
    /* +----------------------------------+ */
    /* | First  | Last   | Message Length | */
    /* +----------------------------------+ */
    /* This packet is the first as well as last packet of the U-APSD config  */
    /* message                                                               */
    len           = len | 0xC000;
    g_cfg_val[0]  = (UWORD8)(len & 0x00FF);
    g_cfg_val[1]  = (UWORD8)(len & 0xFF00);

    /* Setting U-APSD config message body */
    g_cfg_val[2] = get_uapsd_config_max_sp_len();
    g_cfg_val[3] = get_uapsd_config_ac(AC_BK);
    g_cfg_val[4] = get_uapsd_config_ac(AC_BE);
    g_cfg_val[5] = get_uapsd_config_ac(AC_VI);
    g_cfg_val[6] = get_uapsd_config_ac(AC_VO);

    return &g_cfg_val[0];
#endif /* IBSS_BSS_STATION_MODE */
#endif /* MAC_WMM */
    return 0;
}

/* Function to set g_sta_uapsd_status value */
INLINE UWORD8* get_uapsd_status(void)
{
#ifdef MAC_WMM
#ifdef IBSS_BSS_STATION_MODE
    /* --------------------------------------------------------------------- */
    /* Format of U-APSD Status message                                         */
    /* --------------------------------------------------------------------- */
    /* | AP | MaxSP| Delivery enabled status   | Trigger enabled status    | */
    /* | Cap| Len  | AC_BK| AC_BE| AC_VI| AC_VO| AC_BK| AC_BE| AC_VI| AC_VO| */
    /* --------------------------------------------------------------------- */
    /* | 1   | 1     | 1    | 1    | 1    | 1    |  1   | 1    | 1    | 1    | */
    /* --------------------------------------------------------------------- */
    UWORD16 len  = 10; /* Length of U-APSD Status message */

    /* First 2 Bytes are length field and rest of the bytes are actual data  */
    /* Length field encoding */
    /* +----------------------------------+ */
    /* | BIT15  | BIT14  | BIT13 - BIT0   | */
    /* +----------------------------------+ */
    /* | First  | Last   | Message Length | */
    /* +----------------------------------+ */
    /* This packet is the first as well as last packet of the U-APSD Status  */
    /* message                                                               */
    len           = len | 0xC000;
    g_cfg_val[0]  = (UWORD8)(len & 0x00FF);
    g_cfg_val[1]  = (UWORD8)(len & 0xFF00);

    /* The UAPSD status values are set based on if the AP is UAPSD capable */
    if(is_ap_uapsd_capable() == BFALSE)
    {
        /* QAP UAPSD capability and UAPSD status values are set to all 0 if  */
        /* AP is not UAPSD capable                                           */
        memset(&g_cfg_val[2], 0, len);
    }
    else
    {
        /* QAP UAPSD capability */
        g_cfg_val[2] = 1;

        /* The UAPSD status values is same as the configured values if the   */
        /* AP is UAPSD capable. Note that currently the trigger and delivery */
        /* enabled configurations are the same. Hence the same values are    */
        /* copied for both.                                                  */
        g_cfg_val[3]  = get_uapsd_config_max_sp_len();
        g_cfg_val[4]  = get_uapsd_config_ac(AC_BK);
        g_cfg_val[5]  = get_uapsd_config_ac(AC_BE);
        g_cfg_val[6]  = get_uapsd_config_ac(AC_VI);
        g_cfg_val[7]  = get_uapsd_config_ac(AC_VO);
        g_cfg_val[8]  = g_cfg_val[4];
        g_cfg_val[9]  = g_cfg_val[5];
        g_cfg_val[10] = g_cfg_val[6];
        g_cfg_val[11] = g_cfg_val[7];
    }

    return &g_cfg_val[0];
#endif /* IBSS_BSS_STATION_MODE */
#endif /* MAC_WMM */
    return 0;
}

/*****************************************************************************/
/* WMM configuration functions for AP mode                                   */
/*****************************************************************************/

/* Function to set AC parameter values to be used by the AP */
INLINE void set_wmm_ap_ac_params(UWORD8 *val)
{
#ifdef MAC_WMM
#ifdef BSS_ACCESS_POINT_MODE
    set_ap_ac_params_prot_ap(val);
#endif /* BSS_ACCESS_POINT_MODE */
#endif /* MAC_WMM */
}

/* Function to get AC parameter values in use by the AP */
INLINE UWORD8* get_wmm_ap_ac_params(void)
{
#ifdef MAC_WMM
#ifdef BSS_ACCESS_POINT_MODE
    return get_ap_ac_params_prot_ap();
#endif /* BSS_ACCESS_POINT_MODE */
#endif /* MAC_WMM */
    return 0;
}

/* Function to set AC parameter values for STA associated with the AP */
INLINE void set_wmm_sta_ac_params(UWORD8 *val)
{
#ifdef MAC_WMM
#ifdef BSS_ACCESS_POINT_MODE
    set_sta_ac_params_prot_ap(val);
#endif /* BSS_ACCESS_POINT_MODE */
#endif /* MAC_WMM */
}

/* Function to get AC parameter values for STA associated with the AP */
INLINE UWORD8* get_wmm_sta_ac_params(void)
{
#ifdef MAC_WMM
#ifdef BSS_ACCESS_POINT_MODE
    return get_sta_ac_params_prot_ap();
#endif /* BSS_ACCESS_POINT_MODE*/
#endif /* MAC_WMM */
    return 0;
}

/* Function to set UAPSD SUPPORT in AP */
INLINE void set_uapsd_support_ap(UWORD8 val)
{
#ifdef MAC_WMM
#ifdef BSS_ACCESS_POINT_MODE
    mset_UAPSD_ap(val);
#endif /* BSS_ACCESS_POINT_MODE */
#endif /* MAC_WMM */
}

/* Function to get the UAPSD support of AP */
INLINE UWORD8 get_uapsd_support_ap(void)
{
#ifdef MAC_WMM
#ifdef BSS_ACCESS_POINT_MODE
    return mget_UAPSD_ap();
#endif /* BSS_ACCESS_POINT_MODE */
#endif /* MAC_WMM */
    return BFALSE;
}

/**************** 802.11H related functions************************/

INLINE void set_802_11H_DFS_mode(BOOL_T val)
{
#ifdef MAC_802_11H
    if(val == 1)
    {
#ifdef PHY_802_11a
        mset_enableDFS(BTRUE);
#endif /* PHY_802_11a */
    }
    else
    {
        mset_enableDFS(BFALSE);
        disable_radar_detection();
        mask_machw_radar_det_int();
    }
#endif /* MAC_802_11H */
}

INLINE BOOL_T get_802_11H_DFS_mode(void)
{
#ifdef MAC_802_11H
    if(mget_enableDFS() == BTRUE)
        return(BTRUE);
    else
#endif /* MAC_802_11H */
        return(BFALSE);
}


INLINE void set_802_11H_TPC_mode(BOOL_T val)
{
#ifdef MAC_802_11H
    if(val == 1)
    {
#ifdef PHY_802_11a
        mset_enableTPC(BTRUE);
#endif /* PHY_802_11a */
    }
    else
    {
        mset_enableTPC(BFALSE);
    }
#endif /* MAC_802_11H */
}

INLINE BOOL_T get_802_11H_TPC_mode(void)
{
#ifdef MAC_802_11H
    if(mget_enableTPC() == BTRUE)
        return(BTRUE);
    else
#endif /* MAC_802_11H */
        return(BFALSE);
}


INLINE UWORD8* get_rf_supported_info(UWORD8 ch_num)
{
    /*UWORD8 ch_index;
    UWORD8 info[3];
    ch_index = get_index_from_chnl_num(ch_num);
    info[0]  = convert_regval_to_dbm(g_tx_power_11a[ch_index], ch_index);
    info[1]  = convert_regval_to_dbm(g_tx_min_power_11a[ch_index], ch_index);
    info[2]  = g_channel_info_11a[ch_index];
    return info;*/
    return 0;

}

INLINE void set_rf_supported_info(UWORD8 *info)
{
    UWORD8 elem_len;
    UWORD8 len;
    UWORD8 *msg;
    UWORD8 cnt;
    UWORD8 ch_num;
    UWORD8 ch_index;

    /*************************************************************************/
    /* Format of RF SUPPORTED INFO Status message                            */
    /* --------------------------------------------------------------------- */
    /* |Length of| First Channel| Max Power    |Min Pow       | Channel |    */
    /* | string  | Number       | RF supported | RF supported | Byte    |    */
    /* ----------------------------------------------------------------------*/
    /* | 1       | 1            | 1 (reg value)| 1 (reg value)|  1      |    */
    /* ----------------------------------------------------------------------*/
    /*                                                                       */
    /* ----------------------------------------------------------------------*/
    /* | Second Channel| Max Power    |Min Pow       | Channel |Third Ch|    */
    /* | Number        | RF supported | RF supported | Byte    |Number  |    */
    /* ----------------------------------------------------------------------*/
    /* |  1            | 1 (reg value)| 1 (reg value)|  1      |        |    */
    /* ----------------------------------------------------------------------*/
    /*************************************************************************/

    /*************************************************************************/
    /*                'Format of Channel Info Byte'                          */
    /*-----------------------------------------------------------------------*/
    /* BIT0: '1'indicates this channel is supported by RF.                   */
    /* BIT1: '1'indicates whether this channel lies in RADAR band.           */
    /* BIT2 - BIT7: Not used.                                                */
    /*************************************************************************/

    elem_len = *info;
    len = elem_len / 4;
    msg = info + 1;

    for(cnt = 0; cnt < len ; cnt++)
    {
        ch_num  = msg[4*cnt];

        ch_index = get_index_from_chnl_num(ch_num);

        if(ch_index >= MAX_CHANNEL_COUNT)
            continue;

#ifdef PHY_802_11a
        g_tx_power_11a[ch_index]     = msg[4*cnt + 1];
        g_tx_min_power_11a[ch_index] = msg[4*cnt + 2];
        g_channel_info_11a[ch_index] = msg[4*cnt + 3];
#endif /* PHY_802_11a */

#ifdef PHY_802_11b
        g_tx_power_11b[ch_index]     = msg[4*cnt + 1];
#endif /* PHY_802_11b */

#ifdef PHY_802_11g
        g_tx_power_11a[ch_index]     = msg[4*cnt + 1];
#endif /* PHY_802_11g */
    }
}

/* Get the connected station list */
INLINE UWORD8* get_connected_sta_list(void)
{
    UWORD16 total_len;
    total_len     = get_all_sta_join_info(&g_cfg_val[2], MAX_CFG_LEN - 2);

    total_len     = total_len | 0xC000;
    g_cfg_val[0]  = (UWORD8)(total_len & 0x00FF);
    g_cfg_val[1]  = (UWORD8)(total_len & 0xFF00);

    return &g_cfg_val[0];
}

/* Function to set the Country information element. Currently this is valid  */
/* only for 11h protocol and AP mode. Note that the incoming bytes are       */
/* directly stored in the global country information element. The assumption */
/* is that the setting will be in the format specified in the standard.      */
/* Make sure RF Supported Info is set before, whenever this is updated       */
INLINE void set_country_ie(UWORD8 *val)
{
#ifdef MAC_802_11H
#ifdef BSS_ACCESS_POINT_MODE
    if(g_reset_mac_in_progress == BFALSE)
    {
        g_country_ie_len = strlen(val);
        memcpy(g_country_ie, val, g_country_ie_len);

        /* Update the channel information based on the country element */
        update_channel_info();
    }
#endif /* BSS_ACCES_POINT_MODE*/
#endif /* MAC_802_11H */
}

/* Function to get the Country information element. Currently this is valid  */
/* only for 11h protocol and AP mode. The value returned will be in the      */
/* format specified in the standard.                                         */
INLINE UWORD8* get_country_ie(void)
{
#ifdef MAC_802_11H
#ifdef BSS_ACCESS_POINT_MODE
    if(g_reset_mac_in_progress == BFALSE)
    {
        g_cfg_val[0] = g_country_ie_len;
        memcpy(g_cfg_val + 1, g_country_ie, g_country_ie_len);

        return &g_cfg_val[0];
    }
#endif /* BSS_ACCES_POINT_MODE*/
#endif /* MAC_802_11H */

    return 0;
}

#ifdef DEBUG_MODE
INLINE void set_test_case_name(const char* input)
{
#ifdef DEBUG_PRINT
    if(g_reset_mac_in_progress == BFALSE)
        debug_print("\n ---- %s ---- \n",input);
#endif /* DEBUG_PRINT */
}
#endif /* DEBUG_MODE */

INLINE UWORD32 get_sys_firmware_ver(void)
{
    return sys_get_fw_ver();
}

INLINE UWORD32 get_sys_debug_level(void)
{
    return sys_get_debug_level();
}

INLINE void set_sys_debug_level(UWORD32 val)
{
    sys_set_debug_level(val);
}

INLINE UWORD32 get_sys_debug_area(void)
{
    return sys_get_debug_area();
}

INLINE void set_sys_debug_area(UWORD32 val)
{
    sys_set_debug_area(val);
}

INLINE UWORD32 get_unit_test_mode(void)
{
    return g_unit_test_mode;
}

INLINE void set_unit_test_mode(UWORD32 val)
{
    if (!(val & UNIT_TEST_MODE_RX) &&
            (g_unit_test_mode & UNIT_TEST_MODE_RX))
    {
        if (g_ut_rx_alarm_handle)
        {
            stop_alarm(g_ut_rx_alarm_handle);
            delete_alarm(g_ut_rx_alarm_handle);
            g_ut_rx_alarm_handle = NULL;
        }
        sys_debug_print(DBG_AREA_UT, DBG_LEVEL_TRACE,
                        "UT: End RX Unit Test\n");
    }

    if (!(val & UNIT_TEST_MODE_TX) &&
            (g_unit_test_mode & UNIT_TEST_MODE_TX))
    {
        if (g_ut_tx_alarm_handle)
        {
            stop_alarm(g_ut_tx_alarm_handle);
            delete_alarm(g_ut_tx_alarm_handle);
            g_ut_tx_alarm_handle = NULL;
        }
        sys_debug_print(DBG_AREA_UT, DBG_LEVEL_TRACE,
                        "UT: End TX Unit Test\n");
    }

    g_unit_test_mode = val;
}

INLINE UWORD32 get_unit_test_tx_len(void)
{
    return g_unit_test_tx_len;
}

INLINE void set_unit_test_tx_len(UWORD32 val)
{
    g_unit_test_tx_len = val;
}

INLINE void update_cfg_statistics()
{
    UWORD16 len, idx = 0;

    /* copy from g_mac_stats first */
    len = (11 + 19) * sizeof(UWORD32);
    memcpy(&g_cfg_stats, &g_mac_stats, len);
    idx += len;

    /* copy mib counters */
    g_cfg_stats.mtxsucc = mget_TransmittedFrameCount();
    g_cfg_stats.mtxfrag = mget_TransmittedFragmentCount();
    g_cfg_stats.mtxmulticast = mget_MulticastTransmittedFrameCount();

    g_cfg_stats.mtxfail = mget_FailedCount();
    g_cfg_stats.mtxretry = mget_RetryCount();
    g_cfg_stats.mtxmultiretry = mget_MultipleRetryCount();
    g_cfg_stats.mtxackfail = mget_ACKFailureCount();
    g_cfg_stats.mtxctsfail = mget_RTSFailureCount();
    g_cfg_stats.mtxctssucc = mget_RTSSuccessCount();

    g_cfg_stats.mrxfragsuss = mget_ReceivedFragmentCount();
    g_cfg_stats.mrxmulticast = mget_MulticastReceivedFrameCount();

    g_cfg_stats.mrxdup = mget_FrameDuplicateCount();
    g_cfg_stats.mrxfcs = mget_FCSErrorCount();
    g_cfg_stats.mrxwepicv = mget_WEPICVErrorCount();

    /* hostif stats */
#ifdef SDIO_STATS
    g_cfg_stats.htx = sdio_stats.txqued;
    g_cfg_stats.htxack = sdio_stats.txack;
    g_cfg_stats.hrx = sdio_stats.rxdone;
    g_cfg_stats.hrxskip = sdio_stats.rxskip;
#endif /* SDIO_STATS */
}

INLINE void print_cfg_statistics()
{
    printf("###############################################################\n");
    printf("# TX SUCC: -TXMSDU- -TXMPDU- -TXMULC-\n"
           "#          %8d %8d %8d\n",
           g_cfg_stats.mtxsucc, g_cfg_stats.mtxfrag, g_cfg_stats.mtxmulticast);
    printf("# TX FAIL: -TXFAIL- -RETRY-- -MRETRY- -ACKFAIL -CTSFAIL -CTSSUCC\n"
           "#          %8d %8d %8d %8d %8d %8d\n",
           g_cfg_stats.mtxfail, g_cfg_stats.mtxretry, g_cfg_stats.mtxmultiretry,
           g_cfg_stats.mtxackfail, g_cfg_stats.mtxctsfail, g_cfg_stats.mtxctssucc);
    printf("# RX SUCC: -RXMPDU- -RXMULC-\n"
           "#          %8d %8d\n",
           g_cfg_stats.mrxfragsuss, g_cfg_stats.mrxmulticast);
    printf("# RX FAIL: -RXDUPL- -RXFSC-- -RXWEP--\n"
           "#          %8d %8d %8d\n",
           g_cfg_stats.mrxdup, g_cfg_stats.mrxfcs, g_cfg_stats.mrxwepicv);
    printf("# HW INTR: --TBTT-- ---RX--- --HPRX-- ---TX--- ---ERR-- -UNKNOWN\n"
           "#          %8d %8d %8d %8d %8d %8d\n",
           g_cfg_stats.itbtt, g_cfg_stats.irxc, g_cfg_stats.ihprxc,
           g_cfg_stats.itxc, g_cfg_stats.ierr, g_cfg_stats.iexc);
#ifdef SDIO_STATS
    printf("# HOST IF: ---TX--- -TXACK-- ---RX--- -RXSKIP-\n"
           "#          %8d %8d %8d %8d\n",
           g_cfg_stats.htx, g_cfg_stats.htxack,
           g_cfg_stats.hrx, g_cfg_stats.hrxskip);
#endif /* SDIO_STATS */
    printf("###############################################################\n");
}

/* Function to set statistics value */
INLINE void set_statistics(UWORD8 *val)
{
#ifdef IBSS_BSS_STATION_MODE
    /* DO clear */
#endif /* IBSS_BSS_STATION_MODE */
}

/* Function to set statistics value */
INLINE UWORD8* get_statistics(void)
{
#ifdef IBSS_BSS_STATION_MODE
#ifdef DEBUG_MODE
    UWORD16 len = sizeof(cfg_stats_t);

    update_cfg_statistics();
    print_cfg_statistics();

    g_cfg_val[0] = (UWORD8)(len & 0x00FF);
    g_cfg_val[1] = (UWORD8)((len & 0xFF00) >> 8);

    memcpy(&g_cfg_val[2], &g_cfg_stats, len);
    return g_cfg_val;
#endif /* DEBUG_MODE */
#endif /* IBSS_BSS_STATION_MODE */
    return 0;
}

#endif /* CONFIG_H */

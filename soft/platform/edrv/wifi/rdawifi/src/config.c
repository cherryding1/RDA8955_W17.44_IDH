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

















































#include "config.h"
#include "controller_mode_if.h"
#include "init.h"
#include "fsm.h"
#include "rf_if.h"


//#define CONFIG_PRINT
/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

#ifdef CONFIG_TEST_CALC_PTK
UWORD8 g_anonce[NONCE_LEN];
UWORD8 g_snonce[NONCE_LEN];
#endif

BOOL_T  g_reset_mac                      = BFALSE;
UWORD16 g_current_len                    = 0;
UWORD8  g_current_settings[MAX_QRSP_LEN] = {0};
UWORD8  g_cfg_val[MAX_CFG_LEN]           = {0};
UWORD8  g_phy_ver[MAX_PROD_VER_LEN + 1]  = {0};
UWORD32 g_last_w_seq                     = 0x1FF;
UWORD8  g_info_id                        = 0;
UWORD8  g_network_info_id                = 0;
RESET_REQ g_reset_req_from_user          = NO_REQUEST;

UWORD32 g_unit_test_mode                 = 0;
UWORD32 g_unit_test_tx_len               = 100;
ALARM_HANDLE_T *g_ut_tx_alarm_handle     = NULL;
ALARM_HANDLE_T *g_ut_rx_alarm_handle     = NULL;
UWORD32 g_host_test_mode                 = 0; // 1 - RX, 2 - TX, 3 - LOOP, 4 - TPUT

/* Host WLAN Identifier Structure                                            */
/* ID, Type, Response, Reset, Get Function Pointer,  Set Function Pointer    */

host_wid_struct_t g_char_wid_struct[NUM_CHAR_WID] =
{
    {
        WID_BSS_TYPE,                            /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_DesiredBSSType,               (void *)set_DesiredBSSType
    },

    {
        WID_CURRENT_TX_RATE,                     /*WID_CHAR,*/
#ifdef MAC_HW_UNIT_TEST_MODE
        BTRUE,                                    BTRUE,
#else /* MAC_HW_UNIT_TEST_MODE */
        BTRUE,                                    BFALSE,
#endif /* MAC_HW_UNIT_TEST_MODE */
        (void *)get_tx_rate,                      (void *)set_tx_rate
    },

    {
        WID_CURRENT_CHANNEL,                     /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_host_chnl_num,                (void *)set_mac_chnl_num
    },

    {
        WID_PREAMBLE,                            /*WID_CHAR,*/
#ifndef MAC_HW_UNIT_TEST_MODE
        BTRUE,                                    BFALSE,
#else /* MAC_HW_UNIT_TEST_MODE */
        BTRUE,                                    BTRUE,
#endif /* MAC_HW_UNIT_TEST_MODE */
        (void *)get_preamble_type,                (void *)set_preamble_type
    },

    {
        WID_11G_OPERATING_MODE,                  /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_running_mode,                 (void *)set_running_mode
    },

    {
        WID_STATUS,                              /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        0,                                        0
    },

    {
        WID_11G_PROT_MECH,                       /*WID_CHAR,*/
        BTRUE,                                   BTRUE,
        (void *)get_protection_mode,              (void *)set_protection_mode
    },

#ifdef MAC_HW_UNIT_TEST_MODE
    {
        WID_GOTO_SLEEP,                         /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)0,                               (void *)set_machw_sleep
    },
#else /* MAC_HW_UNIT_TEST_MODE */
    {
        WID_SCAN_TYPE,                           /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)mget_scan_type,                   (void *)mset_scan_type
    },
#endif /* MAC_HW_UNIT_TEST_MODE */

    {
        WID_PRIVACY_INVOKED,                     /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)mget_PrivacyInvoked,              (void *)0
    },

    {
        WID_KEY_ID,                              /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)mget_WEPDefaultKeyID,             (void *)mset_WEPDefaultKeyID
    },

    {
        WID_QOS_ENABLE,                         /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_qos_enable,                   (void *)set_qos_enable
    },

    {
        WID_POWER_MANAGEMENT,                    /*WID_CHAR,*/
        BTRUE,                                   BTRUE,
        (void *)get_PowerManagementMode,         (void *)set_PowerManagementMode
    },

    {
        WID_802_11I_MODE,                        /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_802_11I_mode,                 (void *)set_802_11I_mode
    },

    {
        WID_AUTH_TYPE,                           /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_auth_type,                    (void *)set_auth_type
    },

    {
        WID_SITE_SURVEY,                         /*WID_CHAR,*/
        BTRUE,                                    BFALSE,
        (void *)get_site_survey_status,           (void *)set_site_survey
    },

    {
        WID_LISTEN_INTERVAL,                     /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)mget_listen_interval,             (void *)mset_listen_interval
    },

    {
        WID_DTIM_PERIOD,                         /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)mget_DTIMPeriod,                  (void *)set_dtim_period
    },

    {
        WID_ACK_POLICY,                          /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_ack_type,                     (void *)set_ack_type
    },

    {
        WID_RESET,                              /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)0,                               (void *)set_reset_req
    },

    {
        WID_PCF_MODE,                            /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)get_pcf_mode,                     (void *)set_pcf_mode
    },

    {
        WID_CFP_PERIOD,                          /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)mget_CFPPeriod,                   (void *)mset_CFPPeriod
    },

    {
        WID_BCAST_SSID,                          /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_bcst_ssid,                    (void *)set_bcst_ssid
    },

#ifdef MAC_HW_UNIT_TEST_MODE
    {
        WID_PHY_TEST_PATTERN,                    /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)get_phy_test_pattern,             (void *)set_phy_test_pattern
    },
#else /* MAC_HW_UNIT_TEST_MODE */
    {
        WID_DISCONNECT,                          /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)disconnect_station
    },
#endif /* MAC_HW_UNIT_TEST_MODE */

    {
        WID_READ_ADDR_SDRAM ,                    /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)get_read_addr_sdram,              (void *)set_read_addr_sdram
    },

    {
        WID_TX_POWER_LEVEL_11A,                  /*WID_CHAR,*/
        BTRUE,                                   BFALSE,
        (void *)get_tx_power_level_11a,           (void *)set_tx_power_level_11a
    },

    {
        WID_REKEY_POLICY,                        /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_RSNAConfigGroupRekeyMethod,   (void *)set_RSNAConfigGroupRekeyMethod
    },

    {
        WID_SHORT_SLOT_ALLOWED,                  /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_short_slot_allowed,           (void *)set_short_slot_allowed
    },

    {
        WID_PHY_ACTIVE_REG,                      /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_phy_active_reg,               (void *)set_phy_active_reg
    },

    {
        WID_PHY_ACTIVE_REG_VAL,                  /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_phy_active_reg_val,           (void *)set_phy_active_reg_val
    },

    {
        WID_TX_POWER_LEVEL_11B,                  /*WID_CHAR,*/
        BTRUE,                                   BFALSE,
        (void *)get_tx_power_level_11b,           (void *)set_tx_power_level_11b
    },

    {
        WID_START_SCAN_REQ,                      /*WID_CHAR,*/
        BTRUE,                                   BTRUE,
        (void *)get_start_scan_req,               (void *)set_start_scan_req
    },

    {
        WID_RSSI,                                /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_rssi,                         (void *)0
    },

    {
        WID_JOIN_REQ,                            /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)0,                                (void *)set_join_req
    },

    {
        WID_ANTENNA_SELECTION,                   /*WID_CHAR,*/
        BTRUE,                                    BFALSE,
        (void *)get_antenna_selection,            (void *)set_antenna_selection
    },


    {
        WID_USER_CONTROL_ON_TX_POWER,            /*WID_CHAR,*/
        BTRUE,                                    BFALSE,
        (void *)get_user_control_enabled,         (void *)set_user_control_enabled
    },

    {
        WID_MEMORY_ACCESS_8BIT,                  /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_memory_access_8bit,           (void *)set_memory_access_8bit
    },

    {
        WID_CURRENT_MAC_STATUS,                  /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_current_mac_status,           (void *)0
    },

    {
        WID_AUTO_RX_SENSITIVITY,                 /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_auto_rx_sensitivity,           (void *)set_auto_rx_sensitivity
    },

    {
        WID_DATAFLOW_CONTROL,                    /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_dataflow_control,             (void *)set_dataflow_control
    },

    {
        WID_SCAN_FILTER,                         /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_scan_filter,                  (void *)set_scan_filter
    },

    {
        WID_LINK_LOSS_THRESHOLD,                 /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_link_loss_threshold,          (void *)set_link_loss_threshold
    },

    {
        WID_AUTORATE_TYPE,                       /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)get_autorate_type,                (void *)set_autorate_type
    },

    {
        WID_CCA_THRESHOLD,                       /*WID_CHAR,*/
        BFALSE,                                   BTRUE,
        (void *)get_cca_threshold,                (void *)set_cca_threshold
    },


    {
        WID_UAPSD_SUPPORT_AP,                    /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_uapsd_support_ap,             (void *)set_uapsd_support_ap
    },

    /* dot11H parameters */
    {
        WID_802_11H_DFS_MODE,                    /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_802_11H_DFS_mode,             (void *)set_802_11H_DFS_mode
    },

    {
        WID_802_11H_TPC_MODE,                    /*WID_CHAR,*/
        BTRUE,                                    BTRUE,
        (void *)get_802_11H_TPC_mode,             (void *)set_802_11H_TPC_mode
    },

    /* phy reg access */
    {
        WID_PHY_REG_ADDR,                        /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_phy_reg_addr,                 (void *)set_phy_reg_addr
    },

    {
        WID_PHY_REG_VAL,                         /*WID_CHAR,*/
        BFALSE,                                   BFALSE,
        (void *)get_phy_reg_val,                  (void *)set_phy_reg_val
    },
};

const host_wid_struct_t g_short_wid_struct[NUM_SHORT_WID] =
{
    {
        WID_RTS_THRESHOLD,                       /*WID_SHORT,*/
        BTRUE,                                    BFALSE,
        (void *)mget_RTSThreshold,                (void *)set_RTSThreshold
    },

    {
        WID_FRAG_THRESHOLD,                      /*WID_SHORT,*/
        BTRUE,                                    BFALSE,
        (void *)mget_FragmentationThreshold,      (void *)set_FragmentationThreshold
    },

    {
        WID_SHORT_RETRY_LIMIT,                   /*WID_SHORT,*/
        BTRUE,                                    BFALSE,
        (void *)mget_ShortRetryLimit,             (void *)set_ShortRetryLimit
    },

    {
        WID_LONG_RETRY_LIMIT,                    /*WID_SHORT,*/
        BTRUE,                                    BFALSE,
        (void *)mget_LongRetryLimit,              (void *)set_LongRetryLimit
    },

    {
        WID_CFP_MAX_DUR,                         /*WID_SHORT,*/
        BFALSE,                                   BTRUE,
        (void *)mget_CFPMaxDuration,              (void *)mset_CFPMaxDuration
    },

    {
        WID_PHY_TEST_FRAME_LEN,                  /*WID_SHORT,*/
        BFALSE,                                   BTRUE,
        (void *)get_phy_test_frame_len,           (void *)set_phy_test_frame_len
    },

    {
        WID_BEACON_INTERVAL,                     /*WID_SHORT,*/
        BTRUE,                                    BTRUE,
        (void *)mget_BeaconPeriod,                (void *)mset_BeaconPeriod
    },



    {
        WID_MEMORY_ACCESS_16BIT,                 /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_memory_access_16bit,          (void *)set_memory_access_16bit
    },

    {
        WID_RX_SENSE,                            /*WID_SHORT,*/
        BFALSE,                                   BTRUE,
        (void *)get_rx_sense,                     (void *)set_rx_sense
    },

    {
        WID_ACTIVE_SCAN_TIME,                    /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_active_scan_time,             (void *)set_active_scan_time
    },

    {
        WID_PASSIVE_SCAN_TIME,                   /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_passive_scan_time,            (void *)set_passive_scan_time
    },

    {
        WID_SITE_SURVEY_SCAN_TIME,               /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_site_survey_scan_time,        (void *)set_site_survey_scan_time
    },

    {
        WID_JOIN_TIMEOUT,                        /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_join_timeout,                 (void *)set_join_timeout
    },

    {
        WID_AUTH_TIMEOUT,                        /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_auth_timeout,                 (void *)set_auth_timeout
    },

    {
        WID_ASOC_TIMEOUT,                        /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_asoc_timeout,                 (void *)set_asoc_timeout
    },

    {
        WID_11I_PROTOCOL_TIMEOUT,                /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_11i_protocol_timeout,         (void *)set_11i_protocol_timeout
    },

    {
        WID_EAPOL_RESPONSE_TIMEOUT,               /*WID_SHORT,*/
        BFALSE,                                   BFALSE,
        (void *)get_eapol_response_timeout,       (void *)set_eapol_response_timeout
    },
};

const host_wid_struct_t g_int_wid_struct[NUM_INT_WID] =
{
    {
        WID_FAILED_COUNT,                        /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_FailedCount,                 (void *)set_FailedCount_to_zero
    },

    {
        WID_RETRY_COUNT,                         /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_RetryCount,                  (void *)set_RetryCount_to_zero
    },

    {
        WID_MULTIPLE_RETRY_COUNT,                /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_MultipleRetryCount,          (void *)set_MultipleRetryCount_to_zero
    },

    {
        WID_FRAME_DUPLICATE_COUNT,               /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_FrameDuplicateCount,         (void *)set_FrameDuplicateCount_to_zero
    },

    {
        WID_ACK_FAILURE_COUNT,                   /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_ACKFailureCount,             (void *)set_ACKFailureCount_to_zero
    },

    {
        WID_RECEIVED_FRAGMENT_COUNT,             /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_ReceivedFragmentCount,       (void *)set_ReceivedFragmentCount_to_zero
    },

    {
        WID_MULTICAST_RECEIVED_FRAME_COUNT,      /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_MulticastReceivedFrameCount, (void *)set_MulticastReceivedFrameCount_to_zero
    },

    {
        WID_FCS_ERROR_COUNT,                     /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_FCSErrorCount,               (void *)set_FCSErrorCount_to_zero
    },

    {
        WID_SUCCESS_FRAME_COUNT,                 /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_TransmittedFrameCount,       (void *)set_TransmittedFrameCount_to_zero
    },

    {
        WID_PHY_TEST_PKT_CNT,                    /*WID_INT,*/
        BFALSE,                                   BTRUE,
        (void *)get_phy_test_pkt_cnt,             (void *)set_phy_test_pkt_cnt
    },

    {
        WID_PHY_TEST_TXD_PKT_CNT,                /*WID_INT,*/
        BFALSE,                                   BTRUE,
        (void *)get_phy_test_txd_pkt_cnt,         (void *)set_phy_test_txd_pkt_cnt
    },

    {
        WID_TX_FRAGMENT_COUNT,                   /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_TransmittedFragmentCount,    (void *)0
    },

    {
        WID_TX_MULTICAST_FRAME_COUNT,               /*WID_INT,*/
        BFALSE,                                      BFALSE,
        (void *)mget_MulticastTransmittedFrameCount, (void *)0
    },

    {
        WID_RTS_SUCCESS_COUNT,                   /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_RTSSuccessCount,             (void *)0
    },

    {
        WID_RTS_FAILURE_COUNT,                   /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_RTSFailureCount,             (void *)0
    },

    {
        WID_WEP_UNDECRYPTABLE_COUNT,             /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)mget_WEPICVErrorCount,            (void *)0
    },

    {
        WID_REKEY_PERIOD,                        /*WID_INT,*/
        BTRUE,                                    BTRUE,
        (void *)get_RSNAConfigGroupRekeyTime ,    (void *)set_RSNAConfigGroupRekeyTime
    },

    {
        WID_REKEY_PACKET_COUNT,                  /*WID_INT,*/
        BTRUE,                                    BTRUE,
        (void *)get_RSNAConfigGroupRekeyPackets,  (void *)set_RSNAConfigGroupRekeyPackets
    },

#ifdef MAC_HW_UNIT_TEST_MODE
    {
        WID_Q_ENABLE_INFO,                       /*WID_INT,*/
        BTRUE,                                    BTRUE,
        (void *)get_q_enable_info,               (void *)set_q_enable_info
    },
#else /* MAC_HW_UNIT_TEST_MODE */
    {
        WID_802_1X_SERV_ADDR,                    /*WID_INT,*/
        BTRUE,                                    BTRUE,
        (void *)get_1x_serv_addr,                 (void *)set_1x_serv_addr
    },
#endif /* MAC_HW_UNIT_TEST_MODE */

    {
        WID_STACK_IP_ADDR,                       /*WID_INT,*/
        BTRUE,                                    BFALSE,
        (void *)get_stack_ip_addr,                (void *)set_stack_ip_addr
    },

    {
        WID_STACK_NETMASK_ADDR,                  /*WID_INT,*/
        BTRUE,                                    BFALSE,
        (void *)get_stack_netmask_addr,           (void *)set_stack_netmask_addr
    },

    {
        WID_HW_RX_COUNT,                         /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_machw_rx_end_count,           (void *)0
    },

    {
        WID_RF_REG_VAL,                          /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)set_rf_reg_val
    },


    {
        WID_MEMORY_ADDRESS,                      /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_memory_address,               (void *)set_memory_address
    },

    {
        WID_MEMORY_ACCESS_32BIT,                 /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_memory_access_32bit,          (void *)set_memory_access_32bit
    },

    {
        WID_FIRMWARE_INFO,                       /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_firmware_info,                (void *)0
    },


    {
        WID_SYS_FW_VER,                          /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_sys_firmware_ver,             (void *)0
    },

#ifdef DEBUG_PRINT
    {
        WID_SYS_DBG_LVL,                         /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_sys_debug_level,              (void *)set_sys_debug_level
    },

    {
        WID_SYS_DBG_AREA,                        /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)get_sys_debug_area,               (void *)set_sys_debug_area
    },
#else
    {
        WID_SYS_DBG_LVL,                         /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_SYS_DBG_AREA,                        /*WID_INT,*/
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },
#endif

    {
        WID_UT_MODE,                             /*WID_INT,*/
        BFALSE,                                   BTRUE,
        (void *)get_unit_test_mode,               (void *)set_unit_test_mode},

    {
        WID_UT_TX_LEN,                           /*WID_INT,*/
        BFALSE,                                   BTRUE,
        (void *)get_unit_test_tx_len,             (void *)set_unit_test_tx_len
    },
};

const host_wid_struct_t g_str_wid_struct[NUM_STR_WID] =
{
    {
        WID_SSID,                                /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_DesiredSSID,                  (void *)mset_DesiredSSID
    },

    {
        WID_FIRMWARE_VERSION,                    /*WID_STR,*/
        BTRUE,                                    BFALSE,
#ifdef DEBUG_MODE
        (void *)get_manufacturerProductVersion,   (void *)set_test_case_name
    },
#else /* DEBUG_MODE */
        (void *)get_manufacturerProductVersion,   0
    },
#endif /* DEBUG_MODE */

    {
        WID_OPERATIONAL_RATE_SET,                 /*WID_STR,*/
        BTRUE,                                    BFALSE,
        (void *)get_supp_rates,                    0
    },

    {
        WID_BSSID,                               /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_bssid,                        (void *)set_preferred_bssid
    },

    {
        WID_WEP_KEY_VALUE0,                      /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_WEP_key,                      (void *)set_WEP_key
    },

    {
        WID_WEP_KEY_VALUE1,                      /*WID_STR,*/
        BTRUE,                                    BFALSE,
        (void *)get_WEP_key,                      (void *)set_WEP_key
    },

    {
        WID_WEP_KEY_VALUE2,                      /*WID_STR,*/
        BTRUE,                                    BFALSE,
        (void *)get_WEP_key,                      (void *)set_WEP_key
    },

    {
        WID_WEP_KEY_VALUE3,                      /*WID_STR,*/
        BTRUE,                                    BFALSE,
        (void *)get_WEP_key,                      (void *)set_WEP_key
    },

    {
        WID_802_11I_PSK,                         /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_RSNAConfigPSKPassPhrase,     (void *)set_RSNAConfigPSKPassPhrase
    },

    {
        WID_HCCA_ACTION_REQ,                     /*WID_STR,*/
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)set_action_request
    },

    {
        WID_802_1X_KEY,                          /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_1x_key,                       (void *)set_1x_key
    },

    {
        WID_HARDWARE_VERSION,                    /*WID_STR,*/
        BTRUE,                                    BFALSE,
        (void *)get_hardwareProductVersion,       0
    },

    {
        WID_MAC_ADDR,                            /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_mac_addr,                     (void *)set_mac_addr
    },

    {
        WID_PHY_TEST_DEST_ADDR,                  /*WID_STR,*/
        BFALSE,                                   BTRUE,
        (void *)get_phy_test_dst_addr,            (void *)set_phy_test_dst_addr
    },

    {
        WID_PHY_TEST_STATS,                      /*WID_STR,*/
        BFALSE,                                   BFALSE,
        (void *)get_phy_test_stats,               (void *)set_phy_test_stats
    },

    {
        WID_PHY_VERSION,                         /*WID_STR,*/
        BTRUE,                                    BFALSE,
        (void *)get_phyProductVersion,            0
    },

    {
        WID_SUPP_USERNAME,                       /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_supp_username,                (void *)set_supp_username
    },

    {
        WID_SUPP_PASSWORD,                       /*WID_STR,*/
        BTRUE,                                    BTRUE,
        (void *)get_supp_password,                (void *)set_supp_password
    },

    {
        WID_SITE_SURVEY_RESULTS,                 /*WID_STR,*/
        BFALSE,                                   BFALSE,
        (void *)get_site_survey_results,          (void *)0
    },

    {
        WID_RX_POWER_LEVEL,                      /*WID_STR,*/
        BFALSE,                                   BFALSE,
        (void *)get_rx_power_level,               (void *)0
    },



#ifdef IBSS_BSS_STATION_MODE
    {
        WID_ADD_WEP_KEY,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)add_wep_key_sta
    },

    {
        WID_REMOVE_WEP_KEY,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)remove_wep_key_sta
    },

    {
        WID_ADD_PTK,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)add_ptk_sta
    },

    {
        WID_ADD_RX_GTK,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)add_rx_gtk_sta
    },

    {
        WID_ADD_TX_GTK,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)add_tx_gtk
    },

    {
        WID_REMOVE_KEY,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)remove_key_sta
    },

    {
        WID_ASSOC_REQ_INFO,
        BFALSE,                                   BFALSE,
        (void *)get_assoc_req_info,               (void *)0
    },

    {
        WID_ASSOC_RES_INFO,
        BFALSE,                                   BFALSE,
        (void *)get_assoc_res_info,               (void *)0
    },
#else /* IBSS_BSS_STATION_MODE */
    {
        WID_ADD_WEP_KEY,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_REMOVE_WEP_KEY,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_ADD_PTK,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_ADD_RX_GTK,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_ADD_TX_GTK,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_REMOVE_KEY,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_ASSOC_REQ_INFO,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },

    {
        WID_ASSOC_RES_INFO,
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)0
    },
#endif /* IBSS_BSS_STATION_MODE */

    {
        WID_UPDATE_RF_SUPPORTED_INFO,
        BFALSE,                                   BTRUE,
        (void *)get_rf_supported_info,            (void *)set_rf_supported_info
    },

    {
        WID_COUNTRY_IE,                          /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)get_country_ie,                   (void *)set_country_ie
    },
};

const host_wid_struct_t g_binary_data_wid_struct[NUM_BIN_DATA_WID] =
{
    {
        WID_CONFIG_HCCA_ACTION_REQ,              /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)0,                                (void *)set_config_HCCA_actionReq
    },

    {
        WID_UAPSD_CONFIG,                        /*WID_BIN_DATA,*/
        BFALSE,                                   BTRUE,
        (void *)get_uapsd_config,                 (void *)set_uapsd_config
    },

    {
        WID_UAPSD_STATUS,                        /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)get_uapsd_status,                 (void *)0
    },

    {
        WID_WMM_AP_AC_PARAMS,                    /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)get_wmm_ap_ac_params,             (void *)set_wmm_ap_ac_params
    },

    {
        WID_WMM_STA_AC_PARAMS,                   /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)get_wmm_sta_ac_params,            (void *)get_wmm_sta_ac_params
    },

    {
        WID_CONNECTED_STA_LIST,                   /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)get_connected_sta_list,           (void *)0
    },

    {
        WID_HUT_STATS,                           /* WID_BIN_DATA, */
        BFALSE,                                   BFALSE,
        (void *)get_hut_stats,                   (void *)set_hut_stats
    },

    {
        WID_STATISTICS,                          /*WID_BIN_DATA,*/
        BFALSE,                                   BFALSE,
        (void *)get_statistics,                   (void *)set_statistics
    },
};


/*****************************************************************************/
/*                                                                           */
/*  Function Name : parse_config_message                                     */
/*                                                                           */
/*  Description   : This function parses the host configuration messages.    */
/*                                                                           */
/*  Inputs        : 1) Pointer to the MAC structure                          */
/*                  2) Pointer to configuration message                      */
/*                  3) Pointer to Buffer address                             */
/*                                                                           */
/*  Globals       : g_reset_mac                                              */
/*                  g_current_settings                                       */
/*                  g_current_len                                            */
/*                                                                           */
/*  Processing    : This function processes all the configuration messages.  */
/*                  from the host based on the SME type. Currently only      */
/*                  SME types 'Test' and 'Auto' are supported. In case of a  */
/*                  Query request the query response is prepared using the   */
/*                  values of the queried parameters. No MAC state change    */
/*                  occurs in this case. In case of a Write request the      */
/*                  parameters are set and the Write response prepared. If   */
/*                  any parameter writing requires the MAC to be reset, the  */
/*                  appropriate function is called to reset MAC. The         */
/*                  paramters are saved in a global list. After MAC is reset */
/*                  these parameters are set once again. Thereafter, a Scan  */
/*                  Request is prepared and sent to the MAC. Response to the */
/*                  host is sent in the required format based on the SME     */
/*                  mode.                                                    */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void test_alarm_rx(ALARM_HANDLE_T* alarm, ADDRWORD_T data);
void test_alarm_tx(ALARM_HANDLE_T* alarm, ADDRWORD_T data);

void parse_config_message(mac_struct_t *mac, UWORD8* host_req,
                          UWORD8 *buffer_addr, BOOL_T send_write_rsp)
{
    UWORD8  *rtn_buff = NULL;
    UWORD16 rtn_len = 0;
    UWORD8  msg_type  = 0;
    UWORD8  msg_id    = 0;
    UWORD16 msg_len   = 0;
    BOOL_T  free_flag = BTRUE;
    mem_handle_t *mem_handle  = g_shared_mem_handle;
    UWORD16 offset = config_host_hdr_offset(BFALSE,0);


#ifdef HOST_RX_LOCAL_MEM
    if(g_eth2sram_trfr == BTRUE)
        mem_handle = (void *) g_shared_mem_handle;
    else
        mem_handle = (void *) g_local_mem_handle;
#endif /* HOST_RX_LOCAL_MEM */

    /* Extract the Type, Length and ID of the incoming host message. The     */
    /* format of the message is:                                             */
    /* +-------------------------------------------------------------------+ */
    /* | Message Type | Message ID |  Message Length |Message body         | */
    /* +-------------------------------------------------------------------+ */
    /* |     1 Byte   |   1 Byte   |     2 Bytes     | Message Length      | */
    /* +-------------------------------------------------------------------+ */

    msg_type = host_req[0];
    msg_id   = host_req[1];
    msg_len  = host_req[3];
    msg_len  = (msg_len << 8) | host_req[2];
    msg_len -= MSG_HDR_LEN;

    sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                    "CONFIG: message, len = %d\n", msg_len + MSG_HDR_LEN);
    sys_debug_dump_buf(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                       (UWORD8 *)host_req, msg_len + MSG_HDR_LEN);

    /* The  valid types of incoming messages are 'Q' (Query) and 'W' (Write) */
    switch(msg_type)
    {
        case 'Q':
        {
            UWORD16 rsp_len    = 0;
            UWORD8  *query_rsp = 0;

            /* To a Query message a Response message needs to be sent. This      */
            /* message has a type 'R' and should have an ID equal to the         */
            /* incoming Query message.                                           */
            query_rsp = (UWORD8*)mem_alloc(mem_handle, MAX_BUFFER_LEN);
            if(query_rsp == NULL)
            {
#ifdef DEBUG_PRINT
                debug_print("WID Q MemAlloc Fail\n");
#endif /* DEBUG_PRINT */
                mem_free(mem_handle, buffer_addr);
                return;
            }

            /* Set the Message Type and ID. The Message Length is set after the  */
            /* message contents are written. The length is known only after that.*/
            /* Process the Query message and set the contents as required. */
            rsp_len = process_query(&host_req[MSG_DATA_OFFSET],
                                    &query_rsp[offset + MSG_DATA_OFFSET], msg_len);

            /* The length of the message returned is set in the Message length   */
            /* field.                                                            */
            if (0 == rsp_len)
            {
                make_wid_rsp((query_rsp + offset),msg_id,(UWORD8)WRSP_SUCCESS);
                rsp_len = 4;
            }
            else
            {
                /* The length of the message returned is set in the Message length   */
                /* field.                                                            */
                query_rsp[offset + 0] = 'R';
                query_rsp[offset + 1] = msg_id;
                query_rsp[offset + 2] = (rsp_len + MSG_HDR_LEN) & 0xFF;
                query_rsp[offset + 3] = ((rsp_len + MSG_HDR_LEN) & 0xFF00) >> 8;
            }
            /* The response is sent back to the host. No MAC state changes occur */
            /* on reception of a Query message.                                  */
            rtn_buff = query_rsp;
            rtn_len  = (UWORD16)(rsp_len + MSG_HDR_LEN);
        }
        break;

        case 'W':
        {
            UWORD8  *write_rsp = 0;

#ifdef BURST_TX_MODE
            if(get_DesiredBSSType() != 4)
            {
                g_burst_tx_mode_enabled_earlier = BFALSE;
            }
            else
            {
                g_burst_tx_mode_enabled_earlier = BTRUE;
            }
#endif /* BURST_TX_MODE */

            /* Process the request only if it's sequence number is correct */
            if((g_last_w_seq != msg_id)
                    /* Exception for Debug mode as automation program sends seq no:0 always */
                    || (msg_id == 0)
              )
            {
                /* Process the Write message and set the parameters with the given   */
                /* values.                                                           */
                process_write(host_req + MSG_DATA_OFFSET, msg_len);

            }
            else
            {
                g_reset_mac = BFALSE;
            }
            g_last_w_seq = msg_id;


            /* Change MAC states as required. If it is already in Enabled state  */
            /* reset MAC and start again. The previous configuration values are  */
            /* retained.                                                         */
            if(g_reset_mac == BTRUE)
#ifndef MAC_HW_UNIT_TEST_MODE
                if(mac->state != DISABLED)
#endif /* MAC_HW_UNIT_TEST_MODE */
                {
#ifdef DEBUG_MODE
                    g_reset_stats.configchange++;
#endif /* DEBUG_MODE */

                    g_reset_mac_in_progress    = BTRUE;

                    /* Free the buffer */
                    mem_free(mem_handle, buffer_addr);
                    free_flag = BFALSE;

                    /* Save the current configuration before going for Reset         */
                    save_wids();

                    /* Reset MAC - Bring down PHY and MAC H/W, disable MAC           */
                    /* interrupts and release all OS structures. Further, this       */
                    /* function restarts the MAC again from start.                   */
                    sys_debug_print(DBG_AREA_GENERAL, DBG_LEVEL_TRACE,
                                    "user reset\n");
                    reset_mac(mac);

                    /* Restore the saved configuration before resetting              */
                    restore_wids();

                    g_reset_mac_in_progress       = BFALSE;

#ifdef BURST_TX_MODE
                    /* Start scaning only when burst tx mode is disabled */
                    if(g_burst_tx_mode_enabled == BTRUE)
                    {
                        init_mac_fsm_bt(&g_mac);
                    }
#endif /* BURST_TX_MODE */
                }

#ifndef MAC_HW_UNIT_TEST_MODE
            if(mac->state == DISABLED)
#else /* MAC_HW_UNIT_TEST_MODE */
            if(g_test_start == BTRUE)
#endif /* MAC_HW_UNIT_TEST_MODE */
            {
                start_mac_and_phy(&g_mac);
            }

            if(BTRUE == send_write_rsp)
            {
                /* To a Write message a Response message needs to be sent. This      */
                /* message has a type 'R' and should have an ID equal to the         */
                /* incoming Write message. The Message contents contain 'Y' to       */
                /* indicate Success.                                                 */
                write_rsp = (UWORD8*)mem_alloc(mem_handle, MAX_BUFFER_LEN);
                if(write_rsp == NULL)
                {
#ifdef DEBUG_PRINT
                    debug_print("WID S MemAlloc Fail\n");
#endif /* DEBUG_PRINT */
                    mem_free(mem_handle, buffer_addr);
                    return;
                }

                make_wid_rsp((write_rsp + offset),msg_id,WRSP_SUCCESS);
                rtn_buff = write_rsp;
                rtn_len  = WRITE_RSP_LEN + MSG_HDR_LEN;
            }

#ifndef MAC_HW_UNIT_TEST_MODE
            if(g_unit_test_mode & UNIT_TEST_MODE_RX)
            {
                sys_debug_print(DBG_AREA_UT, DBG_LEVEL_TRACE,
                                "UT: Start RX Unit Test\n");
                g_ut_rx_alarm_handle = create_alarm(test_alarm_rx, 0);
                start_alarm(g_ut_rx_alarm_handle, 1000);
            }
            if(g_unit_test_mode & UNIT_TEST_MODE_TX)
            {
                sys_debug_print(DBG_AREA_UT, DBG_LEVEL_TRACE,
                                "UT: Start TX Unit Test\n");
                g_ut_tx_alarm_handle = create_alarm(test_alarm_tx, 0);
                start_alarm(g_ut_tx_alarm_handle, 1000);
            }
#endif /* MAC_HW_UNIT_TEST_MODE */
        }
        break;

        default:
        {
            /* Unknown message type. Do nothing. */
            UWORD8  *error_rsp = 0;
#ifdef DEBUG_PRINT
            debug_print("WID: Neither Set Not Query\n");
#endif /* DEBUG_PRINT */

            /* To a Write message a Response message needs to be sent. This      */
            /* message has a type 'R' and should have an ID equal to the         */
            /* incoming Write message. The Message contents contain 'Y' to       */
            /* indicate Success.                                                 */
            error_rsp = (UWORD8*)mem_alloc(mem_handle, MAX_BUFFER_LEN);
            if(error_rsp == NULL)
            {
                mem_free(mem_handle, buffer_addr);
                return;
            }
            /* make returned data to host */
            make_wid_rsp((error_rsp + offset),msg_id,WRSP_SUCCESS);
            rtn_buff = error_rsp;
            rtn_len  = WRITE_RSP_LEN + MSG_HDR_LEN;
        }
    } /* end of switch(msg_type) */

    sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                    "CONFIG: response, len = %d\n", rtn_len);
    sys_debug_dump_buf(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                       rtn_buff + offset, rtn_len);

    /* send returned data to host */
    if(rtn_buff != NULL)
        send_host_rsp(mem_handle,rtn_buff, rtn_len);

    /* If the buffer is not freed yet, free it */
    if(free_flag == BTRUE)
        mem_free(mem_handle, buffer_addr);
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : get_wid_index                                            */
/*                                                                           */
/*  Description   : This function gets the WID index from WID ID & Type      */
/*                                                                           */
/*  Inputs        : 1) WID Identifier                                        */
/*                  2) WID Type                                              */
/*                                                                           */
/*  Globals       : 1) g_char_wid_struct                                     */
/*                  2) g_short_wid_struct                                    */
/*                  2) g_int_wid_struct                                      */
/*                  2) g_str_wid_struct                                      */
/*                  2) g_binary_data_wid_struct                              */
/*                                                                           */
/*  Processing    : This function searches for the WID ID in the relevent    */
/*                  global structure to find the index of the perticular WID */
/*                                                                           */
/*  Outputs       : None                                                     */
/*                                                                           */
/*  Returns       : Length of the query response                             */
/*                                                                           */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
UWORD8 get_wid_index(UWORD16 wid_id,UWORD8 wid_type)
{
    UWORD8 count;

    switch(wid_type)
    {
        case WID_CHAR:
            for(count = 0; count < NUM_CHAR_WID; count++)
            {
                if(g_char_wid_struct[count].id == wid_id)
                    return count;
            }
            break;
        case WID_SHORT:
            for(count = 0; count < NUM_SHORT_WID; count++)
            {
                if(g_short_wid_struct[count].id == wid_id)
                    return count;
            }
            break;
        case WID_INT:
            for(count = 0; count < NUM_INT_WID; count++)
            {
                if(g_int_wid_struct[count].id == wid_id)
                    return count;
            }
            break;
        case WID_STR:
            for(count = 0; count < NUM_STR_WID; count++)
            {
                if(g_str_wid_struct[count].id == wid_id)
                    return count;
            }
            break;
        case WID_BIN_DATA:
            for(count = 0; count < NUM_BIN_DATA_WID; count++)
            {
                if(g_binary_data_wid_struct[count].id == wid_id)
                    return count;
            }
            break;
    }

    return 0xFF;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : process_query                                            */
/*                                                                           */
/*  Description   : This function processes the host query message.          */
/*                                                                           */
/*  Inputs        : 1) Pointer to the query request message                  */
/*                  2) Pointer to the query response message                 */
/*                  3) Length of the query request                           */
/*                                                                           */
/*  Globals       : g_wid_struct                                             */
/*                                                                           */
/*  Processing    : This function reads the WID value in the Query message   */
/*                  and gets the corresponding WID length and value. It sets */
/*                  the WID, WID Length and value in the response message.   */
/*                  The process is repeated for all the WID types requested  */
/*                                                                           */
/*  Outputs       : None                                                     */
/*                                                                           */
/*  Returns       : Length of the query response                             */
/*                                                                           */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/

UWORD16 process_query(UWORD8* wid_req, UWORD8* wid_rsp, UWORD16 req_len)
{
    UWORD8  count   = 0;
    UWORD16 wid_id  = 0;
    UWORD16 req_idx = 0;
    UWORD16 rsp_idx = 0;

    /* The format of a message body of a message type 'Q' is:                */
    /* +-------------------------------------------------------------------+ */
    /* | WID0          | WID1         | WID2         | ................... | */
    /* +-------------------------------------------------------------------+ */
    /* |     2 Bytes   |    2 Bytes   |    2 Bytes   | ................... | */
    /* +-------------------------------------------------------------------+ */

    /* The format of a message body of a message type 'R' is:                */
    /* +-------------------------------------------------------------------+ */
    /* | WID0      | WID0 Length | WID0 Value  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 1 Byte      | WID Length  | ......................... | */
    /* +-------------------------------------------------------------------+ */

    /* The processing of a Query message consists of the following steps:    */
    /* 1) Read the WID value in the Query message                            */
    /* 2) Get the corresponding WID length and value                         */
    /* 3) Set the WID, WID Length and value in the response message          */
    /* 4) Repeat for all the WID types requested by the host                 */
    /* 5) Return the length of the response message                          */

#ifdef CONFIG_PRINT
    if(g_reset_mac_in_progress == BFALSE)
        debug_print("\nWID Q: ");
#endif /* CONFIG_PRINT */

    while(req_idx < req_len)
    {
        /* Read the WID type (2 Bytes) from the Query message and increment  */
        /* the Request Index by 2 to point to the next WID.                  */
        wid_id = wid_req[req_idx + 1];
        wid_id = (wid_id << 8) | wid_req[req_idx];
        req_idx += 2;

#ifdef CONFIG_PRINT
        if(g_reset_mac_in_progress == BFALSE)
            debug_print("%x ",wid_id);
#endif /* CONFIG_PRINT */

        sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                        "CONFIG: Query, wid_id = 0x%04x\n", wid_id);

        if(wid_id == WID_ALL)
        {
            /* If the WID type is WID_ALL all WID values need to be returned */
            /* to the host. Thus all the array elements of the global WID    */
            /* array are accessed one by one.                                */
            for(count = 0; count < NUM_CHAR_WID; count++)
            {
                if(g_char_wid_struct[count].rsp == BTRUE)
                    rsp_idx += get_wid(&wid_rsp[rsp_idx], count, WID_CHAR);
            }

            for(count = 0; count < NUM_SHORT_WID; count++)
            {
                if(g_short_wid_struct[count].rsp == BTRUE)
                    rsp_idx += get_wid(&wid_rsp[rsp_idx], count, WID_SHORT);
            }

            for(count = 0; count < NUM_INT_WID; count++)
            {
                if(g_int_wid_struct[count].rsp == BTRUE)
                    rsp_idx += get_wid(&wid_rsp[rsp_idx], count, WID_INT);
            }

            for(count = 0; count < NUM_STR_WID; count++)
            {
                if(g_str_wid_struct[count].rsp == BTRUE)
                    rsp_idx += get_wid(&wid_rsp[rsp_idx], count, WID_STR);
            }
            for(count = 0; count < NUM_BIN_DATA_WID; count++)
            {
                if(g_binary_data_wid_struct[count].rsp == BTRUE)
                    rsp_idx += get_wid(&wid_rsp[rsp_idx], count, WID_BIN_DATA);
            }

        }
        else
        {
            /* In case of any other WID, the static global WID array is      */
            /* searched to find a matching WID type for its corresponding    */
            /* length and value.                                             */
            UWORD8 wid_type = (wid_id & 0xF000) >> 12;
            count           = get_wid_index(wid_id,wid_type);

            if(count == 0xFF)
            {
                /* If the Queried WID type is not found it is an exception.  */
                /* Skip this and continue.                                   */
                continue;
            }
            else
            {
                /* In case of a valid WID, fill up the WID type, WID length  */
                /* and WID values in the Response message.                   */
                rsp_idx += get_wid(&wid_rsp[rsp_idx], count, wid_type);
            }
        }
    } /* end of while(index < req_len) */

#ifdef CONFIG_PRINT
    if(g_reset_mac_in_progress == BFALSE)
        debug_print("||\n");
#endif /* CONFIG_PRINT */

    /* Return Response length (given by the updated response index value)    */
    return rsp_idx;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : process_write                                            */
/*                                                                           */
/*  Description   : This function processes the host write message.          */
/*                                                                           */
/*  Inputs        : 1) Pointer to the write request message                  */
/*                  2) Length of the write request                           */
/*                                                                           */
/*  Globals       : g_wid_struct                                             */
/*                  g_reset_mac                                              */
/*                                                                           */
/*  Processing    : This function reads the WID value in the Write message   */
/*                  and sets the corresponding WID length and value. The     */
/*                  process is repeated for all the WID types requested by   */
/*                  the host. In case the setting of any WID requires the    */
/*                  reset of MAC, the global g_reset_mac is updated.         */
/*                                                                           */
/*  Outputs       : g_reset_mac is updated to indicate if a reset is needed  */
/*                                                                           */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/

void process_write(UWORD8* wid_req, UWORD16 req_len)
{
    UWORD8  count    = 0;
    UWORD16 req_idx  = 0;
    UWORD16 wid_len  = 0;
    UWORD16 wid_id   = 0;
    UWORD8  *wid_val = 0;
    UWORD8  wid_type = 0;

    /* Set the global flag for reset MAC to BFALSE. This will be updated to  */
    /* BTRUE if any parameter setting calls for a reset.                     */
    g_reset_mac = BFALSE;

    /* The format of a message body of a message type 'W' is:                */
    /* +-------------------------------------------------------------------+ */
    /* | WID0      | WID0 Length | WID0 Value  | ......................... | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 1 Byte      | WID Length  | ......................... | */
    /* +-------------------------------------------------------------------+ */

    /* The processing of a Write message consists of the following steps:    */
    /* 1) Read the WID value in the Write message                            */
    /* 2) Set the corresponding WID length and value                         */
    /* 3) Repeat for all the WID types requested by the host                 */

#ifdef CONFIG_PRINT
    if(g_reset_mac_in_progress == BFALSE)
        debug_print("\nWID S: ");
#endif /* CONFIG_PRINT */

    while(req_idx+3 < req_len)
    {

        /* Read the WID ID (2 Bytes) and the WID length from the Write       */
        /* message.                                                          */
        wid_id   = wid_req[req_idx + 1];
        wid_id   = (wid_id << 8) | wid_req[req_idx];
        wid_type = (wid_id & 0xF000) >> 12;
        count    = get_wid_index(wid_id,wid_type);

#ifdef CONFIG_PRINT
        if(g_reset_mac_in_progress == BFALSE)
            debug_print("%x %x",wid_id,count);
#endif /* CONFIG_PRINT */

        sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_VERB,
                        "CONFIG: Write, wid_id = 0x%04x\n", wid_id);

        if(wid_type != WID_BIN_DATA)
        {
            wid_len = wid_req[req_idx + 2];

            /* Set the 'wid' pointer to point to the WID value (skipping */
            /* the ID and Length fields). This is used in the set_wid    */
            /* function. It is assumed that all 'set' functions use the  */
            /* value directly as input.                                  */
            wid_val = &wid_req[req_idx + WID_VALUE_OFFSET];

            /* The Request Index is incremented by (WID Length + 3) to   */
            /* point to the next WID.                                    */
            req_idx += (wid_len + WID_VALUE_OFFSET);
        }
        else
        {
            UWORD16 i        = 0;
            UWORD8  checksum = 0;

            /* WID_BIN_DATA format */
            /* +---------------------------------------------------+ */
            /* | WID    | WID Length | WID Value        | Checksum | */
            /* +---------------------------------------------------+ */
            /* | 2Bytes | 2Bytes     | WID Length Bytes | 1Byte    | */
            /* +---------------------------------------------------+ */

            wid_len  = wid_req[req_idx + 2];
            wid_len |= ((UWORD16)wid_req[req_idx + 3] << 8) & 0xFF00 ;
            wid_len &= WID_BIN_DATA_LEN_MASK;

            /* Set the 'wid' pointer to point to the WID length (skipping */
            /* the ID field). This is used in the set_wid function. It    */
            /* is assumed that 'set' function for Binary data uses the    */
            /* length field for getting the WID Value                     */
            wid_val  = &wid_req[req_idx + WID_LENGTH_OFFSET];

            /* The Request Index is incremented by (WID Length + 5) to   */
            /* point to the next WID.                                    */
            req_idx += (wid_len + WID_LENGTH_OFFSET + 2 + 1);

            /* Compute checksum on the Data field */
            for(i = 0; i < wid_len; i++)
            {
                checksum += wid_val[i + 2];
            }
            /* Drop the packet, if checksum failed */
            if(checksum != wid_val[wid_len + 2])
            {
                continue;
            }
        }

        if(count == 0xFF)
        {
            /* If the Queried WID type is not found it is an exception.  */
            /* Skip this and continue.                                   */
            continue;
        }
        else
        {
            /* In case of a valid WID, set the corresponding parameter with  */
            /* the incoming value.                                           */
            set_wid(wid_val, wid_len, count, wid_type);

            /* Check if writing this WID requires resetting of MAC. The      */
            /* global g_reset_mac is 'OR'ed with the reset value obtained    */
            /* for this WID.                                                 */
            switch(wid_type)
            {
                case WID_CHAR:
                    g_reset_mac |= g_char_wid_struct[count].reset;
                    break;
                case WID_SHORT:
                    g_reset_mac |= g_short_wid_struct[count].reset;
                    break;
                case WID_INT:
                    g_reset_mac |= g_int_wid_struct[count].reset;
                    break;
                case WID_STR:
                    g_reset_mac |= g_str_wid_struct[count].reset;
                    break;
                case WID_BIN_DATA:
                    g_reset_mac |= g_binary_data_wid_struct[count].reset;
                    break;
            }
        }
    } /* end of while(index < req_len) */

#ifdef CONFIG_PRINT
    if(g_reset_mac_in_progress == BFALSE)
        debug_print("||\n");
#endif /* CONFIG_PRINT */

    /* Overwrite the Reset Flag with User request  */
    if(g_reset_req_from_user == DONT_RESET)
    {
        g_reset_mac = BFALSE;
    }
    else if (g_reset_req_from_user == DO_RESET)
    {
        g_reset_mac = BTRUE;
    }

    /* Re-initialize user request variable */
    g_reset_req_from_user = NO_REQUEST;
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : get_wid                                                  */
/*                                                                           */
/*  Description   : This function gets the WID value from the global WID     */
/*                  array and updates the response message contents.         */
/*                                                                           */
/*  Inputs        : 1) Pointer to the WID field                              */
/*                  2) Index of the global WID array                         */
/*                  3) The WID type                                          */
/*                                                                           */
/*  Globals       : g_wid_struct                                             */
/*                                                                           */
/*  Processing    : This function sets the WID ID, Length and Value in the   */
/*                  the incoming packet.                                     */
/*                                                                           */
/*  Outputs       : None                                                     */
/*                                                                           */
/*  Returns       : Length of WID response                                   */
/*                                                                           */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/

UWORD16 get_wid(UWORD8 *wid, UWORD8 count, UWORD8 wid_type)
{
    UWORD8  wid_len  = 0;
    UWORD16 wid_id   = 0;
    UWORD8  *wid_val = 0;
    /* NULL response is corresponded */
    BOOL_T  null_rsp = BFALSE;

    /* The WID is set in the following format:                               */
    /* +-------------------------------------------------------------------+ */
    /* | WID Type  | WID Length | WID Value                                | */
    /* +-------------------------------------------------------------------+ */
    /* | 2 Bytes   | 1 Byte     | WID length                               | */
    /* +-------------------------------------------------------------------+ */

    /* Set the WID value with the value returned by the get function         */
    /* associated with the particular WID type.                              */
    wid_val = (wid + WID_VALUE_OFFSET);

    /* If the member function is not NULL, get the value, else discard read */
    switch(wid_type)
    {
        case WID_CHAR:
        {
            wid_id   = g_char_wid_struct[count].id;
            if(g_char_wid_struct[count].get)
            {
                UWORD8 val =
                    ((UWORD8 (*)(void))g_char_wid_struct[count].get)();

                wid_len = 1;

                wid_val[0] = val;
            }
            else
            {
                wid_len = 0;
            }
        }
        break;

        case WID_SHORT:
        {
            wid_id   = g_short_wid_struct[count].id;
            if(g_short_wid_struct[count].get)
            {
                UWORD16 val =
                    ((UWORD16 (*)(void))g_short_wid_struct[count].get)();

                wid_len = 2;

                wid_val[0] = (UWORD8)(val & 0x00FF);
                wid_val[1] = (UWORD8)((val >> 8) & 0x00FF);
            }
            else
            {
                wid_len = 0;
            }
        }
        break;

        case WID_INT:
        {
            wid_id   = g_int_wid_struct[count].id;
            if(g_int_wid_struct[count].get)
            {
                UWORD32 val =
                    ((UWORD32 (*)(void))g_int_wid_struct[count].get)();

                wid_len = 4;

                wid_val[0] = (UWORD8)(val & 0x000000FF);
                wid_val[1] = (UWORD8)((val >> 8) & 0x000000FF);
                wid_val[2] = (UWORD8)((val >> 16) & 0x000000FF);
                wid_val[3] = (UWORD8)((val >> 24) & 0x000000FF);
            }
            else
            {
                wid_len = 0;
            }
        }
        break;

        case WID_STR:
        {
            wid_id   = g_str_wid_struct[count].id;
            if(g_str_wid_struct[count].get)
            {
                UWORD8 *val =
                    ((UWORD8 *(*)(void))g_str_wid_struct[count].get)();

                if(val != NULL)
                {
                    wid_len = val[0];

                    memcpy(wid_val, (val + 1), wid_len);
                }
                else
                {
                    /* NULL response is corresponded */
                    null_rsp = BTRUE;
                    wid_len = 0;
                }
            }
            else
            {
                wid_len = 0;
            }
        }
        break;

        case WID_BIN_DATA:
        {
            /* WID_BIN_DATA is Set in the following format */
            /* +---------------------------------------------------+ */
            /* | WID    | WID Length | WID Value        | Checksum | */
            /* +---------------------------------------------------+ */
            /* | 2Bytes | 2Bytes     | WID Length Bytes | 1Byte    | */
            /* +---------------------------------------------------+ */

            wid_id  = g_binary_data_wid_struct[count].id;
            if(g_binary_data_wid_struct[count].get)
            {
                UWORD16 length      = 0;
                UWORD8  checksum    = 0;
                UWORD16 i           = 0;
                UWORD8  *val        =
                    ((UWORD8 *(*)(void))g_binary_data_wid_struct[count].get)();

                if(val == NULL)
                {
                    return 0;
                }

                /* First 2 Bytes are length field and rest of the bytes are */
                /* actual data                                              */

                /* Length field encoding */
                /* +----------------------------------+ */
                /* | BIT15  | BIT14  | BIT13 - BIT0   | */
                /* +----------------------------------+ */
                /* | First  | Last   | Message Length | */
                /* +----------------------------------+ */
                length  = (val[0] & 0x00FF);
                length |= ((UWORD16) val[1] << 8) & 0xFF00;
                length &= WID_BIN_DATA_LEN_MASK;

                if(length != 0 )
                {
                    /* Set the WID type. */
                    wid[0] = wid_id & 0xFF;
                    wid[1] = (wid_id & 0xFF00) >> 8;

                    /* Copy the data including the length field */
                    memcpy((wid + WID_LENGTH_OFFSET), val, (length + 2));

                    /* Compute Checksum for the data field & append it at    */
                    /* the end of the packet                                 */
                    for(i = 0; i < length; i++)
                    {
                        checksum += val[2 + i];
                    }
                    *(wid + WID_LENGTH_OFFSET + 2 + length) = checksum;

                    /* Return the WID response length */
                    /*WID_TYPE        WID_Length  WID_Value  Checksum*/
                    return (WID_LENGTH_OFFSET + 2         + length   + 1);
                }
                else
                {
                    return 0;
                }
            }
        }
        break;
    } /* end of switch(wid_len) */

    /* NULL response is corresponded */
    if((wid_len != 0) || (BTRUE == null_rsp))
    {
        /* Set the WID type. The WID length field is set at the end after        */
        /* getting the value of the WID. In case of variable length WIDs the     */
        /* length s known only after the value is obtained.                      */
        wid[0] = wid_id & 0xFF;
        wid[1] = (wid_id & 0xFF00) >> 8;

        /* Set the length of the WID */
        wid[2] = wid_len;

        /* Return the WID response length */
        return (wid_len + WID_VALUE_OFFSET);
    }
    else
    {
        return (0);
    }
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : set_wid                                                  */
/*                                                                           */
/*  Description   : This function sets the WID value with the incoming WID   */
/*                  value.                                                   */
/*                                                                           */
/*  Inputs        : 1) Pointer to the WID value                              */
/*                  2) Length of the WID value                               */
/*                  3) Index of the global WID array                         */
/*                  4) The WID type                                          */
/*                                                                           */
/*  Globals       : g_wid_struct                                             */
/*                                                                           */
/*  Processing    : This function sets the WID Value with the incoming value.*/
/*                                                                           */
/*  Outputs       : The corresponding MAC parameter is set                   */
/*                                                                           */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/

void set_wid(UWORD8 *wid, UWORD16 len, UWORD8 count, UWORD8 wid_type)
{
    switch(wid_type)
    {
        case WID_CHAR:
        {
            if(g_char_wid_struct[count].set)
            {
                ((void (*)(UWORD8))g_char_wid_struct[count].set)(wid[0]);
            }
            else
            {
                /* WID cannot be set. Do nothing. */
            }
        }
        break;

        case WID_SHORT:
        {
            UWORD16 val = 0;

            val = wid[1];
            val = (val << 8) | wid[0];

            if(g_short_wid_struct[count].set)
            {
                ((void (*)(UWORD16))g_short_wid_struct[count].set)(val);
            }
            else
            {
                /* WID cannot be set. Do nothing. */
            }
        }
        break;

        case WID_INT:
        {
            UWORD32 val = 0;

            val  = wid[3];
            val  = (val << 8) | wid[2];
            val  = (val << 8) | wid[1];
            val  = (val << 8) | wid[0];

            if(g_int_wid_struct[count].set)
            {
                ((void (*)(UWORD32))g_int_wid_struct[count].set)(val);
            }
            else
            {
                /* WID cannot be set. Do nothing. */
            }
        }
        break;

        case WID_STR:
        {
            UWORD8 val[MAX_STRING_LEN];

            memcpy(val, wid, len);
            val[len] = '\0';

            if(g_str_wid_struct[count].set)
            {
                ((void (*)(UWORD8*))g_str_wid_struct[count].set)(val);
            }
            else
            {
                /* WID cannot be set. Do nothing. */
            }
        }
        break;

        case WID_BIN_DATA:
        {
            if(g_binary_data_wid_struct[count].set)
            {
                ((void (*)(UWORD8*))g_binary_data_wid_struct[count].set)(wid);
            }
            else
            {
                /* WID cannot be set. Do nothing. */
            }
        }
        break;

    } /* end of switch(wid_type) */
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : send_host_rsp                                            */
/*                                                                           */
/*  Description   : This function sends the response to the host             */
/*                                                                           */
/*  Inputs        : 1) Pointer to the response buffer                        */
/*                  2) Length of the response message                        */
/*                  3) Memory handle where the buffer belongs to             */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : prepares the config header and sends the packet to host  */
/*                  to the host.                                             */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         18 02 2010   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void send_host_rsp(mem_handle_t *mem_handle, UWORD8 *host_rsp, UWORD16 host_rsp_len)
{
    UWORD16 config_pkt_len = 0;

    if((host_rsp == NULL) ||
            (host_rsp_len == 0))
    {
#ifdef DEBUG_PRINT
        debug_print("Config Pkt NULL\n");
#endif /* DEBUG_PRINT */
        return;
    }

    config_pkt_len = prepare_config_hdr(host_rsp, host_rsp_len, BFALSE, 0);

    if(config_pkt_len < host_rsp_len)
    {
        mem_free(mem_handle, host_rsp);
#ifdef DEBUG_PRINT
        debug_print("Wrong Config Pkt Len\n");
#endif /* DEBUG_PRINT */
        return;
    }

    if(mem_handle == g_shared_mem_handle)
    {
        send_frame_to_host_if(host_rsp, config_pkt_len, g_shared_mem_handle,
                              host_shared_completion_fn, host_rsp);
    }
    else
    {
        send_frame_to_host_if(host_rsp, config_pkt_len, g_local_mem_handle,
                              host_local_completion_fn, host_rsp);
    }
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : send_mac_status                                          */
/*                                                                           */
/*  Description   : This function sends the status of MAC to host            */
/*                                                                           */
/*  Inputs        : MAC Status                                               */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : This function prepares an Information frame having the   */
/*                  MAC Status and calls send_host_rsp function to send the  */
/*                  prepared frame to host                                   */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         11 04 2008   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void send_mac_status(UWORD8 mac_status)
{
#ifdef SEND_MAC_STATUS

    mem_handle_t *mem_handle  = g_shared_mem_handle;
    UWORD8       *info_msg    = NULL;
    UWORD16 offset = config_host_hdr_offset(BFALSE,0);

    if( g_current_mac_status != mac_status )
    {
#ifdef HOST_RX_LOCAL_MEM
        if(g_eth2sram_trfr == BTRUE)
            mem_handle = (void *) g_shared_mem_handle;
        else
            mem_handle = (void *) g_local_mem_handle;
#endif /* HOST_RX_LOCAL_MEM */

        info_msg = (UWORD8*)mem_alloc(mem_handle, MAX_BUFFER_LEN);
        if(info_msg == NULL)
        {
            return;
        }

        info_msg[offset + 0] = 'I';
        info_msg[offset + 1] = g_info_id;
        /* Increment the id sequence counter by 1*/
        g_info_id++;

        info_msg[offset + 2] = (STATUS_MSG_LEN) & 0xFF;
        info_msg[offset + 3] = ((STATUS_MSG_LEN) & 0xFF00) >> 8;

        /* Set the WID_STATUS, Length to 1, Value to mac_status */
        info_msg[offset + 4] = WID_STATUS & 0xFF;
        info_msg[offset + 5] = (WID_STATUS & 0xFF00) >> 8;
        info_msg[offset + 6] = 1;
        info_msg[offset + 7] = mac_status;

        sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                        "CONFIG: send_mac_status, len = %d\n", STATUS_MSG_LEN);
        sys_debug_dump_buf(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                           info_msg + offset, STATUS_MSG_LEN);

        /* The response is sent back to the host */
        send_host_rsp(mem_handle,info_msg, STATUS_MSG_LEN);
        g_current_mac_status = mac_status;

#ifdef CONFIG_PRINT
        if(g_current_mac_status)
        {
            debug_print("Connected to: %s\n",(UWORD32)mget_DesiredSSID());
        }
        else
        {
            debug_print("Disconnected\n");
        }
#endif /* CONFIG_PRINT */
    }
#endif /* SEND_MAC_STATUS */
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : send_wake_status                                         */
/*                                                                           */
/*  Description   : This function sends the status of PS condition to host   */
/*                                                                           */
/*  Inputs        : PS Status                                                */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : This function prepares an Information frame having the   */
/*                  PS Status and calls send_host_rsp function to send the   */
/*                  prepared frame to host                                   */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         30 09 2008   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void send_wake_status(UWORD8 wake_status)
{
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : send_network_info_to_host                                */
/*                                                                           */
/*  Description   : This function sends the beacon/probe response from scan  */
/*                                                                           */
/*  Inputs        : BSSID, RSSI & InfoElements                               */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : This function prepares an Information frame having the   */
/*                  BSSID. RSSI value and Information elements and sends the */
/*                  prepared frame to host                                   */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         17 08 2009   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void send_network_info_to_host(UWORD8 *msa, UWORD16 rx_len, signed char rssi)
{
#ifndef ETHERNET_HOST
    mem_handle_t *mem_handle  = g_shared_mem_handle;
    UWORD8       *info_msg    = NULL;
    UWORD16      ninfo_data_len= 0;
    UWORD16 offset = config_host_hdr_offset(BFALSE,0);

    info_msg = (UWORD8*)mem_alloc(mem_handle, MAX_BUFFER_LEN);
    if(info_msg == NULL)
    {
#ifdef DEBUG_PRINT
        debug_print("No buffer to send Beacon/ProbeRsp\n");
#endif /* DEBUG_PRINT */
        return;
    }
    rx_len -= FCS_LEN;
    ninfo_data_len = (rx_len + 9);


    info_msg[offset + 0] = 'N';
    info_msg[offset + 1] = g_network_info_id;
    /* Increment the id sequence counter by 1*/
    g_network_info_id++;

    info_msg[offset + 2] = ninfo_data_len & 0xFF;
    info_msg[offset + 3] = (ninfo_data_len & 0xFF00) >> 8;

    info_msg[offset + 4] = WID_NEWORK_INFO & 0xFF;
    info_msg[offset + 5] = (WID_NEWORK_INFO & 0xFF00) >> 8;

    info_msg[offset + 6] = (rx_len + 1) & 0xFF;
    info_msg[offset + 7] = ((rx_len + 1) & 0xFF00) >> 8;

    info_msg[offset + 8] = rssi;

    memcpy32(&info_msg[offset + 9],msa,rx_len);

    sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                    "CONFIG: send_network_info, len = %d\n", ninfo_data_len);
    sys_debug_dump_buf(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                       info_msg + offset, ninfo_data_len);

    send_host_rsp(mem_handle,info_msg, ninfo_data_len);
#endif /* ETHERNET_HOST */
}


/*****************************************************************************/
/*                                                                           */
/*  Function Name : send_sta_join_info_to_host                               */
/*                                                                           */
/*  Description   : This function sends info of a joining/leaving sta to host*/
/*                                                                           */
/*  Inputs        : station address                                          */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : This function prepares an Information frame having the   */
/*                  information about a joining/leaving station.             */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         08 06 2010   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void send_join_leave_info_to_host(UWORD16 aid, UWORD8* sta_addr, BOOL_T joining)
{
#ifndef ETHERNET_HOST
    mem_handle_t *mem_handle  = g_shared_mem_handle;
    UWORD8       *info_msg    = NULL;
    UWORD16      info_len     = 16;
    UWORD16 offset = config_host_hdr_offset(BFALSE,0);

    info_msg = (UWORD8*)mem_alloc(mem_handle, MAX_BUFFER_LEN);
    if(info_msg == NULL)
    {
#ifdef DEBUG_PRINT
        debug_print("No buffer to send STA join/leave info to host\n");
#endif /* DEBUG_PRINT */
        return;
    }

    /* Get the infromation of joining station */
    if(joining == BTRUE)
    {
        info_len = get_sta_join_info(sta_addr, &info_msg[offset + 8]);
    }
    /* prepare the information of leaving station */
    else
    {
        /* Reset all the fields */
        memset(&info_msg[offset + 8], 0, info_len);

        /* Copy the association id of the leaving station */
        info_msg[offset + 8] = aid & 0xFF;

        /* Copy the MAC address of the leaving station */
        memcpy(&info_msg[offset + 9], sta_addr, 6);
    }

    if(info_len)
    {
        info_msg[offset + 0] = 'N';
        info_msg[offset + 1] =  g_info_id;

        /* Increment the id sequence counter by 1*/
        g_info_id++;

        /* Prepare the frame */
        info_msg[offset + 2] =  (info_len + 8) & 0xFF;
        info_msg[offset + 3] = ((info_len + 8) & 0xFF00) >> 8;

        info_msg[offset + 4] = WID_STA_JOIN_INFO & 0xFF;
        info_msg[offset + 5] = (WID_STA_JOIN_INFO & 0xFF00) >> 8;

        info_msg[offset + 6] =  info_len & 0xFF;
        info_msg[offset + 7] = (info_len & 0xFF00) >> 8;

        sys_debug_print(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                        "CONFIG: send_join_leave_info, len = %d\n", (info_len + 8));
        sys_debug_dump_buf(DBG_AREA_CONFIG, DBG_LEVEL_TRACE,
                           info_msg + offset, (info_len + 8));

        send_host_rsp(mem_handle,info_msg, (info_len + 8));
    }
    else
    {
        mem_free(g_shared_mem_handle, info_msg);
    }
#endif /* ETHERNET_HOST */
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : save_wids                                                */
/*                                                                           */
/*  Description   : This function saves all the wid values to a global arry  */
/*                                                                           */
/*  Inputs        : None                                                     */
/*                                                                           */
/*  Globals       : g_char_wid_struct                                        */
/*                  g_short_wid_struct                                       */
/*                  g_int_wid_struct                                         */
/*                  g_str_wid_struct                                         */
/*                  g_binary_data_wid_struct                                 */
/*                  g_current_settings                                       */
/*                  g_current_len                                            */
/*                                                                           */
/*  Processing    : Checks if the WID has a get and set function and stores  */
/*                  the values of all such WIDs in to global array for       */
/*                  restoring from it later                                  */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void save_wids(void)
{
    UWORD16 count = 0;
    UWORD16 idx   = 0;

    /* Save all the settings in the global array. This is saved as a */
    /* dummy Write message with all WID values. It is used to set    */
    /* the parameters once reset is done.                            */


    /* The global WID array is accessed and all the current WID  */
    /* values are saved in the format of WID ID, Length, Value.  */
    for(count = 0; count < NUM_CHAR_WID; count++)
    {
        if((g_char_wid_struct[count].set) &&
                (g_char_wid_struct[count].get))
            idx += get_wid(&g_current_settings[idx], count,
                           WID_CHAR);
    }

    for(count = 0; count < NUM_SHORT_WID; count++)
    {
        if((g_short_wid_struct[count].set)&&
                (g_short_wid_struct[count].get))
            idx += get_wid(&g_current_settings[idx], count,
                           WID_SHORT);
    }

    for(count = 0; count < NUM_INT_WID; count++)
    {
        if((g_int_wid_struct[count].set)&&
                (g_int_wid_struct[count].get))
            idx += get_wid(&g_current_settings[idx], count,
                           WID_INT);
    }

    for(count = 0; count < NUM_STR_WID; count++)
    {
        if((g_str_wid_struct[count].set)&&
                (g_str_wid_struct[count].get))
            idx += get_wid(&g_current_settings[idx], count,
                           WID_STR);

        /*To make sure that it doesnt cross g_current_settings array boundry*/
        if(idx >= (MAX_QRSP_LEN - 50))
            break;
    }

    /* There may not be enought memory for this operation, so     */
    /* disabling it for now                                       */
#if 0
    for(count = 0; count < NUM_BIN_DATA_WID; count++)
    {
        if((g_binary_data_wid_struct[count].set)&&
                (g_binary_data_wid_struct[count].get))
            idx += get_wid(&g_current_settings[idx], count,
                           WID_BIN_DATA);
    }
#endif
    /* Set the length of global current configuration settings */
    g_current_len = idx;

#ifdef DEBUG_MODE
    /* Check whether query response length exceeded max length */
    if(g_current_len > MAX_QRSP_LEN)
    {
        g_mac_stats.max_query_rsp_len_exceeded = BTRUE;
    }
#endif /* DEBUG_MODE */
}

/*****************************************************************************/
/*                                                                           */
/*  Function Name : restore_wids                                             */
/*                                                                           */
/*  Description   : This function restores all the wids from the global arry */
/*                                                                           */
/*  Inputs        : None                                                     */
/*                                                                           */
/*  Globals       : g_current_settings                                       */
/*                  g_current_len                                            */
/*                                                                           */
/*  Processing    : Calls the process write function for restoring all the   */
/*                  WID values                                               */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         21 05 2005   Ittiam          Draft                                */
/*                                                                           */
/*****************************************************************************/
void restore_wids(void)
{
    /* Set all the required parameters from the saved configuration  */
    /* settings.                                                     */
    process_write(g_current_settings, g_current_len);
}



/*****************************************************************************/
/*                                                                           */
/*  Function Name : make_wid_rsp                                             */
/*                                                                           */
/*  Description   : This function make response massage to host              */
/*                                                                           */
/*  Inputs        : 1) buffer to response for wid                            */
/*                  2) massage id of wid                                     */
/*                  3) massage data of wid                                   */
/*                                                                           */
/*  Globals       : None                                                     */
/*                                                                           */
/*  Processing    : set massage data of response                             */
/*                                                                           */
/*  Outputs       : None                                                     */
/*  Returns       : None                                                     */
/*  Issues        : None                                                     */
/*                                                                           */
/*  Revision History:                                                        */
/*                                                                           */
/*         DD MM YYYY   Author(s)       Changes (Describe the changes made)  */
/*         15 04 2007                   Draft                                */
/*                                                                           */
/*****************************************************************************/
void make_wid_rsp(UWORD8 *wid_rsp_data, UWORD8 msg_id, UWORD8 wid_massage)
{
    wid_rsp_data[0] = 'R';
    wid_rsp_data[1] = msg_id;
    wid_rsp_data[2] = (WRITE_RSP_LEN + MSG_HDR_LEN) & 0xFF;
    wid_rsp_data[3] = ((WRITE_RSP_LEN + MSG_HDR_LEN) & 0xFF00) >> 8;

    /* Set the WID_STATUS, Length to 1, Value to SUCCESS */
    wid_rsp_data[4] = WID_STATUS & 0xFF;
    wid_rsp_data[5] = (WID_STATUS & 0xFF00) >> 8;
    wid_rsp_data[6] = 1;
    wid_rsp_data[7] = wid_massage;

    return;
}

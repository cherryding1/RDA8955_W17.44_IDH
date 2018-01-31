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


#ifndef _RDA5890_DEV_H_
#define _RDA5890_DEV_H_
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <linux/ethtool.h>
#include <linux/debugfs.h>
#include <net/ieee80211.h>

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

/* RDA5890 defined bss descriptor, 44 bytes for each bss */
struct rda5890_bss_descriptor
{
    u8 ssid[IW_ESSID_MAX_SIZE + 1];
    u8 bss_type;
    u8 channel;
    u8 dot11i_info;
    u8 bssid[ETH_ALEN];
    u8 rssi;
    u8 auth_info;
    u8 rsn_cap[2];
} __attribute__ ((packed));

/**
 *  @brief Structure used to store information for each beacon/probe response
 */
struct bss_descriptor
{
    struct rda5890_bss_descriptor data;

    u8 wpa_ie[MAX_WPA_IE_LEN];
    size_t wpa_ie_len;
    u8 rsn_ie[MAX_WPA_IE_LEN];
    size_t rsn_ie_len;

    unsigned long last_scanned;

    struct list_head list;
};

/* Generic structure to hold all key types. */
struct enc_key
{
    u16 len;
    u16 flags;  /* KEY_INFO_* from defs.h */
    u16 type; /* KEY_TYPE_* from defs.h */
    u8 key[32];
};

struct rda5890_802_11_security
{
    u8 WPAenabled;
    u8 WPA2enabled;
    u8 wep_enabled;
    u8 auth_mode;
    u32 key_mgmt;
};

/** Private structure for the rda5890 device */
struct rda5890_private
{
    char name[DEV_NAME_LEN];

    void *card;
    struct net_device *dev;

    struct net_device_stats stats;

    /** current ssid/bssid related parameters and status */
    int connect_status;
    struct rda5890_bss_descriptor curbssparams;
    struct iw_statistics wstats;

    /** association flags */
    unsigned char assoc_ssid[IW_ESSID_MAX_SIZE + 1];
    unsigned char assoc_ssid_len;
#define ASSOC_FLAG_SSID         1
#define ASSOC_FLAG_CHANNEL      2
#define ASSOC_FLAG_BAND         3
#define ASSOC_FLAG_MODE         4
#define ASSOC_FLAG_BSSID        5
#define ASSOC_FLAG_WEP_KEYS     6
#define ASSOC_FLAG_WEP_TX_KEYIDX    7
#define ASSOC_FLAG_WPA_MCAST_KEY    8
#define ASSOC_FLAG_WPA_UCAST_KEY    9
#define ASSOC_FLAG_SECINFO      10
#define ASSOC_FLAG_WPA_IE       11
    unsigned long assoc_flags;
    unsigned char imode;
    unsigned char authtype;

    /** debugfs */
    struct dentry *debugfs_dir;
    struct dentry *debugfs_files[6];

    /** general purpose lock */
    struct mutex lock;

    /** for wid request and response */
    struct mutex wid_lock;
    struct completion wid_done;
    int wid_pending;
    char *wid_rsp;
    unsigned short wid_rsp_len;
    char wid_msg_id;

    /** delayed worker */
    struct workqueue_struct *work_thread;
    struct delayed_work scan_work;
    struct delayed_work assoc_work;
    struct delayed_work assoc_done_work;

    /** Hardware access */
    int (*hw_host_to_card) (struct rda5890_private *priv, u8 *payload, u16 nb);

    /** Scan results list */
    int scan_running;
    struct list_head network_list;
    struct list_head network_free_list;
    struct bss_descriptor *networks;

    /** Encryption parameter */
    struct rda5890_802_11_security secinfo;

    /** WEP keys */
    struct enc_key wep_keys[4];
    u16 wep_tx_keyidx;

    /** WPA keys */
    struct enc_key wpa_mcast_key;
    struct enc_key wpa_unicast_key;

    /** WPA Information Elements*/
    u8 wpa_ie[MAX_WPA_IE_LEN];
    u8 wpa_ie_len;

    u8 is_wapi;

    /** sleep/awake flag */
    atomic_t sleep_flag;
};

extern int rda5890_sleep_flags;

struct rda5890_private *rda5890_add_card(void *card, struct device *dmdev);
void rda5890_remove_card(struct rda5890_private *priv);
int rda5890_start_card(struct rda5890_private *priv);
void rda5890_stop_card(struct rda5890_private *priv);

#endif

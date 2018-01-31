#ifndef __SMARTCONFIG_H__
#define __SMARTCONFIG_H__

#include "cs_types.h"
#include "string.h"
#include "cos.h"

#define SMARTLING_REPORT_PORT 18266
#define SMARTLING_REPORT_MSG_OFFSET 9

#define ESP_SMART_CONFIG_GUIDE_MSG_LEN 517
#define ESP_SMART_CONFIG_FIXED_OFFSET 45
#define ESP_SMART_CONFIG_EXTRA_HEAD_LEN 9

#define CRC8_TABLE_SIZE         256
#define RECEVEBUF_MAXLEN        200

#define SMART_CONFIG_TIMER 240//ms
#define SMART_CONFIG_GETHANDLER_TIMER 4*1000//ms
#define SMART_CONFIG_GETOFFSET_TIMER 10*1000//s
#define SMART_CONFIG_LOCKED_TIMER 30*1000//s

typedef enum _encrytion_mode {
    ENCRY_NONE           = 1,
    ENCRY_WEP,
    ENCRY_TKIP,
    ENCRY_CCMP
} ENCYTPTION_MODE;

enum wland_smartlink_decode_status {
    SMART_CH_INIT,
    SMART_CH_LOCKED,
    SMART_CH_LOCKING,
    SMART_CH_DONE,
};

enum wland_smartlink_status{
    SMARTLINK_INIT,
    SMARTLINK_GET_FILTER,
    SMARTLINK_STE_FILTER,
    SMARTLINK_READY,
    SMARTLINK_ENABLE,
    SMARTLINK_DONE,
    SMARTLINK_ERR,
    SMARTLINK_DONE_SET_USRCTRL,
    SMARTLINK_END,
};

struct wland_smartlink_offset {
    u16 msg[4];
    u8 msg_ldpc[4];
    u16 len;
    u16 len_temp;
};
struct wland_smartlink_analyzer {
    u16 msg[3];
};

struct wland_smartlink {
    u8 total_len;
    u8 SSID_crc;
    u8 BSSID_crc;
    u8 total_xor;
    //char SSID[33];
    u8 SSID_len;
    u8 SSID_hiden;
    //char key[50];
    u8 key_len;
    u8 ip[4];

    char SSID[33];
    char key[64+1];
    enum wland_smartlink_status state;

    void *timer;
    int smart_lock_flag;
    unsigned long smart_lock_time;
    u8 BSSID[ETH_ALEN];
    u8 channel;
    enum wland_smartlink_decode_status decode_status;
    u16 channel_time;
    u16 channel_time_all;
    u8 current_channel;
    u8 channel_bits[20];
    u8 channel_bits_len;
    bool smartend;
    u16 smart_times;
    int from_ds;
    int ldpc;
    u8 buf[RECEVEBUF_MAXLEN+1];
    u8 buf_flag[RECEVEBUF_MAXLEN+1];
    u8 buf_len;
    u16 len_offset;
    struct wland_smartlink_offset offset_msg;
    struct wland_smartlink_analyzer analyzer_msg;
};

extern int smartconfig_getssid_start();
extern BOOL smartconfig_stop();
extern BOOL smartconfig_getssid_end(char *ssid, char *passwd, u8 *ip4_addr);
extern BOOL smartconfig_getssid_result(char *ssid, char *passwd, u8 *ip4_addr);
extern INT send_smartconfig_response_to_host();
extern INT rda_notify_smartlink_handler(u16 data_len, void *data);
#endif

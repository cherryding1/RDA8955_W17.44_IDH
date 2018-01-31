/******************************************************************************
 * Copyright 2014-2015  (By younger)
 *
 * FileName: smartconfig.c
 *
 * Description: smartconfig demo on sniffer
 *
 * Modification history:
 *     2015/1/24, v1.0 create this file.
*******************************************************************************/
#ifdef WIFI_SMARTCONFIG
#include "event.h"
#include "cos.h"
#include "ts.h"
#include "sxr_tim.h"
#include "base_prv.h"
#include "string.h"
#include "wifi_config.h"
#include "wifi_smartconfig.h"
#include "sockets.h"

#define printf(format, ...) \
    do \
    { \
        AT_OutputText(14, (format), ##__VA_ARGS__); \
    } while (0)

#define WLAND_DBG(level, fmt, ...)  do {\
        AT_OutputText(14, ("%s,"fmt), __func__, ##__VA_ARGS__); \
    } while (0)

#define MAX_TRAVERSE_TIMES		3
#define MAX_LOCK_TIMES			5
#define SCAN_TIME  				3000	//ms

#define SNIFF_FILTER_TYPE 0x800F7FFF
#define MACDBG                          "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STRDBG(ea)                  (ea)[0], (ea)[1], (ea)[2], (ea)[3], (ea)[4], (ea)[5]

static UINT8 smart_lock_flag=0;
static UINT8 smart_mac[6];
static UINT8 smart_channel=-1;
static UINT8 smart_status = SMART_CH_INIT;
static UINT8 channel_timer;
static UINT8 current_channel;
static UINT16 channel_bits;
static bool smartend = TRUE;
static UINT8 smart_times = 0;
static UINT8 lock_times = 0;
static UINT8 smart_recvbuf_len=0;
static UINT8 smart_recvbuf[RECEVEBUF_MAXLEN] = {0};
static UINT32 g_saved_filter_type;
static UINT32 g_saved_monfilter_type;
enum wland_smartlink_status rda_smartconfig_state = SMARTLINK_INIT;
static struct wland_smartlink *smartlink = NULL;
extern UINT8 g_smartconfigStop;

struct bss_descs_t
{
    unsigned char bss_descs[sizeof(RDA5890_BSS_DESC)*RDA5890_MAX_NETWORK_NUM ];
#ifdef __SCAN_INFO__
    unsigned char bss_info[sizeof(RDA5890_BSS_NETWORK_INFO)*RDA5890_MAX_NETWORK_NUM ];
#endif
    UINT8 count;
};
extern struct bss_descs_t bss_Rst;
extern HANDLE wifi_task_handle;
extern UINT8 localMac[6] ;

#if 0
static u8 *wland_crc8_table;
#else
static const u8 wland_crc8_table[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};
#endif

static void wland_smartlink_timer(void);
static void wlan_smartlink_timeout(void);

static void wland_smartlink_complete(BOOL success)
{
    sxr_StopFunctionTimer(wland_smartlink_timer);
    COS_StopCallbackTimer(wifi_task_handle, wlan_smartlink_timeout, NULL);
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_SMART_CONFIG_COMPL;
    EV.nParam1 = success;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

static inline int is_multicast_ether_addr(const u8 *addr)
{
    return (0x01 & addr[0]);
}

static int wland_smartlink_config(u8 channel, u16 time)
{
    wifi_SScanAps(channel,time);
}

static u8 wland_crc8(u8 *pdata, u32 nbytes, u8 crc)
{
    /* loop over the buffer data */
    while (nbytes-- > 0)
        crc = wland_crc8_table[(crc ^ *pdata++) & 0xff];

    return crc;
}

static void wland_smartlink_report(int done)
{
    #if 0
	wland_scan_result *bss = NULL;
    #endif

    char *msg = smartlink->buf;
    int ret = 0;

    WLAND_DBG(INFO, "Enter\r\n");

    if (smartlink->state != SMARTLINK_ENABLE)
        return ;

    if (!done) {
        WLAND_DBG(ERROR,"SmartLink end, doesn't find available network\r\n");
        ret = -1;
        goto done;
    }

    #if 0
    list_for_each_entry(bss, &bss_info.scan_list, list) {
        u8 BSSID_crc, SSID_crc;
        BSSID_crc = wland_crc8(bss->BSSID, ETH_ALEN, 0);
        SSID_crc = wland_crc8(bss->SSID, bss->SSID_len, 0);
        if (BSSID_crc==smartlink->BSSID_crc && SSID_crc==smartlink->SSID_crc) {
            memcpy(smartlink->BSSID, bss->BSSID, ETH_ALEN);
            strcpy(smartlink->SSID, bss->SSID, bss->SSID_len);
            break;
        }
    }
    #endif

    if (smartlink->SSID_hiden) {
        smartlink->SSID_len = smartlink->total_len - ESP_SMART_CONFIG_EXTRA_HEAD_LEN - smartlink->key_len;
        memcpy(smartlink->SSID, msg+ESP_SMART_CONFIG_EXTRA_HEAD_LEN+smartlink->key_len, smartlink->SSID_len);
        smartlink->SSID[smartlink->SSID_len] = '\0';
        WLAND_DBG(INFO, "Find Hiden SSID:(%s). BSSID:("MACDBG")\r\n", smartlink->SSID, MAC2STRDBG(smartlink->BSSID));
    } else
        WLAND_DBG(INFO, "Find unhiden SSID(%s). BSSID:("MACDBG")\r\n", smartlink->SSID, MAC2STRDBG(smartlink->BSSID));

    if (smartlink->key_len > 0) {
        memcpy(smartlink->key, msg+ESP_SMART_CONFIG_EXTRA_HEAD_LEN, smartlink->key_len);
        smartlink->key[smartlink->key_len] = '\0';
        WLAND_DBG(INFO, "Key:%s\r\n", smartlink->key);
    } else
        WLAND_DBG(INFO, "Unencrypted\r\n");

    WLAND_DBG(INFO, "SSID_crc=%d BSSID_crc=%d\r\n",smartlink->SSID_crc, smartlink->BSSID_crc);

    WLAND_DBG(INFO, "IP: %d.%d.%d.%d\r\n",
        smartlink->ip[0], smartlink->ip[1], smartlink->ip[2], smartlink->ip[3]);
    WLAND_DBG(INFO, "SmartConfig %s\r\n", smartlink->from_ds?"FROM DS":"TO DS");
done:

    if (ret < 0) {
        smartlink->state = SMARTLINK_ERR;
        WLAND_DBG(ERROR, "report error(%d), start smartlink again\r\n", ret);
        wland_smartlink_complete(FALSE);
    } else {
        smartlink->state = SMARTLINK_DONE;
        //while (1) {}
        WLAND_DBG(INFO, "report Done(%d), stop smartlink\r\n", ret);
        wland_smartlink_complete(TRUE);
    }
}

static int wland_smartlink_decode(u16 *msg)
{
    u32 i = 0;
    u8 pos=0, val=0, crc=0;
    u8 check_crc;
    u8 check_xor;
    //WLAND_DBG(DEBUG, "Enter\r\n");

    pos = msg[1]&0xff;
    crc = (msg[0]&0xf0) | ((msg[2]&0xf0)>>4);
    val = ((msg[0]&0x0f)<<4) | (msg[2]&0x0f);
    //WLAND_DBG(ERROR,"pos:%02x-val:%02x-crc:%02x\r\n", pos, val, crc);
    check_crc = wland_crc8(&val, 1, 0);
    //WLAND_DBG(ERROR,"CRC:%02x\r\n", check_crc);
    check_crc = wland_crc8(&pos, 1, check_crc);
    //WLAND_DBG(ERROR,"CRC:%02x\r\n", check_crc);
    if (check_crc == crc) {
        WLAND_DBG(ERROR, "Get IT:[%02x]=%02x\r\n", pos, val);
        if (pos > smartlink->buf_len)
            smartlink->buf_len = pos;
        if (!smartlink->buf_flag[pos]) {
            smartlink->buf[pos] = val;
            smartlink->buf_flag[pos] = 1;
            switch(pos) {
            case 0:
                smartlink->total_len = val;
                break;
            case 1:
                smartlink->key_len = val;
                break;
            case 2:
                smartlink->SSID_crc = val;
                break;
            case 3:
                smartlink->BSSID_crc = val;
                break;
            case 4:
                smartlink->total_xor = val;
                break;
            case 5:
                smartlink->ip[0]= val;
                break;
            case 6:
                smartlink->ip[1]= val;
                break;
            case 7:
                smartlink->ip[2]= val;
                break;
            case 8:
                smartlink->ip[3]= val;
                break;
            default:
                break;
            }
            return SMART_CH_LOCKED;
        }else {
            //WLAND_DBG(ERROR, "%02x:%02x\r\n", smartlink->total_len, smartlink->buf_len);
            if (smartlink->total_len != 0 &&
                smartlink->buf_len+1 >= ESP_SMART_CONFIG_EXTRA_HEAD_LEN) {
                    check_xor = 0;
                    for (i=0; i<=smartlink->buf_len; ++i) {
                        if (smartlink->buf_flag[i] == 0)
                            break;
                        else if (i != 4)
                            check_xor ^= smartlink->buf[i];
                    }
                    //WLAND_DBG(ERROR, "%02x:%02x\r\n", i, smartlink->buf_len);
                    if (i == smartlink->buf_len+1 && check_xor == smartlink->total_xor) {
                        if (smartlink->BSSID[0] != 0xff) {//BCC
                            if (wland_crc8(smartlink->BSSID, ETH_ALEN, 0) == smartlink->BSSID_crc) {
                                //WLAND_DBG(ERROR, "keylen %02x\r\n", i, smartlink->key_len);
                                if (smartlink->buf_len+1 == smartlink->total_len) {
                                    smartlink->SSID_hiden = 1;
                                    wland_smartlink_report(1);
                                    return SMART_CH_DONE;
                                } else if (smartlink->buf_len+1 == ESP_SMART_CONFIG_EXTRA_HEAD_LEN + smartlink->key_len) {
                                    smartlink->SSID_hiden = 0;
                                    wland_smartlink_report(1);
                                    return SMART_CH_DONE;
                                }
                            } else {
                                WLAND_DBG(ERROR,"%02x:%02x\r\n", wland_crc8(smartlink->BSSID, ETH_ALEN, 0), smartlink->BSSID_crc);
                                wland_smartlink_report(0);
                            }
                        } else {//LDPC
                            //WLAND_DBG(ERROR, "keylen %02x\r\n", i, smartlink->key_len);
                            if (smartlink->buf_len+1 == smartlink->total_len) {
                                smartlink->SSID_hiden = 1;
                                wland_smartlink_report(1);
                                return SMART_CH_DONE;
                            } else if (smartlink->buf_len+1 == ESP_SMART_CONFIG_EXTRA_HEAD_LEN + smartlink->key_len) {
                                smartlink->SSID_hiden = 0;
                                wland_smartlink_report(1);
                                return SMART_CH_DONE;
                            }

                        }
                    } else{
                        WLAND_DBG(ERROR,"%d:%d, %d:%d\r\n", i, smartlink->buf_len, check_xor, smartlink->total_xor);
                    }
            }
            return SMART_CH_LOCKED;
        }
    }
    return SMART_CH_INIT;
}

static void wland_smartlink_analyze(char *BSSID, int len)
{
    u32 ret;

    u16 *msg = smartlink->analyzer_msg.msg;

    u8 index_flag;
    WLAND_DBG(DEBUG, "Enter.len:%d\r\n", len);

    if (len<ESP_SMART_CONFIG_FIXED_OFFSET || len>ESP_SMART_CONFIG_FIXED_OFFSET+0x1ff) {
        WLAND_DBG(ERROR,"Length error:%d\r\n", len);
        return;
    }

    if (len >= ESP_SMART_CONFIG_GUIDE_MSG_LEN) {
        WLAND_DBG(DEBUG, "SmartConfig Guiding:%d\r\n", len);
        return;
    }

    len -= ESP_SMART_CONFIG_FIXED_OFFSET;
    index_flag = !!(len&0xff00);
    if (index_flag && ((len&0xff00)!=0x100)) {
        WLAND_DBG(ERROR,"Length error:%d\r\n", len);
        return;
    }

    if (BSSID) {
        if (memcmp(smartlink->BSSID, BSSID, ETH_ALEN)) {
            if (smartlink->smart_lock_flag == 0) {
                WLAND_DBG(INFO, "New BSSID "MACDBG"\r\n", MAC2STRDBG(BSSID));
                memcpy(smartlink->BSSID, BSSID, ETH_ALEN);
            } else {
                WLAND_DBG(ERROR,"BSSID error:%d\r\n", len);
                return;
            }
        }
    } else {
        smartlink->BSSID[0] = 0xff;//error bssid;
    }

    //if (index_flag && ((len&0xff)>smartlink->buf_len))
        //smartlink->buf_len = len&0xff;

    msg[0] = msg[1];
    msg[1] = msg[2];
    msg[2] = len;

    WLAND_DBG(DEBUG, "%02x %02x %02x\r\n", msg[0], msg[1], msg[2]);

    if (!index_flag &&
        (msg[1]&0xff00) == 0x100 &&
        (msg[0]&0xff00) == 0) {
        ret = wland_smartlink_decode(msg);
        if (ret != SMART_CH_INIT) {
            msg[0] = msg[1] = msg[2] = 0;
            if (smartlink->smart_lock_flag == 0) {
                smartlink->smart_lock_flag = 1;
                smartlink->channel = smartlink->channel_bits[smartlink->current_channel];
                smartlink->channel_time_all = SMART_CONFIG_LOCKED_TIMER/SMART_CONFIG_TIMER;//20s
                WLAND_DBG(INFO, "LOCK %d ms\r\n", SMART_CONFIG_LOCKED_TIMER);
            }
        }
    }

}

static void wland_smartlink_get_offset(u16 len, void *data)
{

    u16 *msg = smartlink->offset_msg.msg;
    u8 *msg_ldpc = smartlink->offset_msg.msg_ldpc;
    u16 *msg_len = &(smartlink->offset_msg.len);
    u16 *msg_len_temp = &(smartlink->offset_msg.len_temp);

    WLAND_DBG(DEBUG, "Enter:len(%d), %p\r\n", len, data);

    //set timer SMART_CONFIG_GETHANDLER_TIMER, when receive two udp multicast pkt
    if (*msg_len_temp == 1) {
        WLAND_DBG(INFO, "LOCK %d ms\r\n", SMART_CONFIG_GETHANDLER_TIMER);
        smartlink->channel_time_all = SMART_CONFIG_GETHANDLER_TIMER/SMART_CONFIG_TIMER;
    }
    *msg_len_temp = *msg_len_temp + 1;

    if (len<ESP_SMART_CONFIG_GUIDE_MSG_LEN || len>ESP_SMART_CONFIG_GUIDE_MSG_LEN+150)
        return;

    if (*msg_len == 0) {
        msg[0] = len;
        msg_ldpc[0] = (data == NULL);
        (*msg_len) = 1;
    } else if (msg[*msg_len-1] == len + 2) {
        if (*msg_len == 3) {
            smartlink->len_offset = len-ESP_SMART_CONFIG_GUIDE_MSG_LEN;
            smartlink->channel_time_all = SMART_CONFIG_GETOFFSET_TIMER/SMART_CONFIG_TIMER;//1s
            if (data==NULL && msg_ldpc[0] && msg_ldpc[1] && msg_ldpc[2])
                smartlink->ldpc = 1;
            else if (data && !msg_ldpc[0] && !msg_ldpc[1] && !msg_ldpc[2])
                smartlink->ldpc = 0;
            else
                WLAND_DBG(ERROR,"######ERROR, L AND B#######\r\n");

            WLAND_DBG(ERROR, "\\\\\\\\\\\\\\OFFSET:%d, l:%d\r\n", smartlink->len_offset, smartlink->ldpc);
        } else {
            msg[*msg_len] = len;
            msg_ldpc[*msg_len] = (data == NULL);
            ++ (*msg_len);
        }
    } else {
        msg[0] = len;
        msg_ldpc[0] = (data == NULL);
        (*msg_len) = 1;

    }
}


INT rda_notify_smartlink_handler(u16 data_len, void *data)
{
    u8 *pkt;
    const u8 *addr1;
    const u8 *addr2;
    const u8 *addr3;
    const u8 *seq_p;
    static u16 seq_num = 0xffff;
    const char *temp_bssid;
    const char *temp_da;

    WLAND_DBG(DEBUG, "Enter:len:%d(state:%d)\r\n", data_len, smartlink->state);
    //printf("#%d\r\n", data_len);
    if (smartlink==NULL || smartlink->state!=SMARTLINK_ENABLE)
        return 0;

    if (data != NULL) {//bcc
        pkt = (u8 *)data;

        if ((pkt[0]&0xFC)!= DATA && (pkt[0]&0xFC)!=QOS_DATA)
            return -1;

        addr1 = pkt + 4;
        addr2 = addr1 + 6;
        addr3 = addr2 + 6;
        seq_p = pkt + 22;


        if (smartlink->from_ds == 1) {
            if ((pkt[1]&0x03) != 0x02) //from DS
                return -1;
            temp_bssid = addr2;
            temp_da = addr1;
        } else {
            if ((pkt[1]&0x03) != 0x01) //to DS
                return -1;
            temp_bssid = addr1;
            temp_da = addr3;
        }

        if(!is_multicast_ether_addr(temp_da)) //Destion addr is broadcast addr
            return -2;

        if (pkt[0]!=0x08 && pkt[0]!=0x88)
            return -4;

        if (seq_num == (seq_p[0] | (seq_p[1]<<8))>>4)
            return 0;

        seq_num = (seq_p[0] | (seq_p[1]<<8))>>4;

        WLAND_DBG(DEBUG, "seq:%04x\r\n", seq_num);
    }

    if (smartlink->len_offset == 0)
        wland_smartlink_get_offset(data_len, data);
    else {
        if (smartlink->ldpc && !data)
            wland_smartlink_analyze(NULL, data_len-smartlink->len_offset);
        else if (!smartlink->ldpc && data)
            wland_smartlink_analyze(temp_bssid, data_len-smartlink->len_offset);
    }
    return 0;
}

static int wland_smartlink_init()
{
    smartlink->total_len = 0;
    smartlink->SSID_crc = 0;
    smartlink->BSSID_crc = 0;
    smartlink->total_xor = 0;
    smartlink->SSID_len = 0;
    smartlink->SSID_hiden = 0;
    smartlink->key_len = 0;
    memset(smartlink->ip, 0, 4);

    smartlink->smart_lock_flag = 0;

    smartlink->ldpc = 0;

    smartlink->SSID[0] = '\0';
    smartlink->key[0] = '\0';
    smartlink->smart_lock_flag = 0;

    memset(smartlink->BSSID, 0xff, ETH_ALEN);
    smartlink->decode_status = SMART_CH_INIT;
    smartlink->channel_time = 0;
    smartlink->channel_time_all = 1;
    smartlink->smartend = TRUE;
    memset(smartlink->buf, 0, RECEVEBUF_MAXLEN);
    memset(smartlink->buf_flag, 0, RECEVEBUF_MAXLEN);

    smartlink->buf_len = 0;
    smartlink->len_offset = 0;
    memset(&smartlink->offset_msg, 0, sizeof(smartlink->offset_msg));
    memset(&smartlink->analyzer_msg, 0, sizeof(smartlink->analyzer_msg));
}

static void wland_smartlink_timer(void)
{
    if (smartlink == NULL)
        return;
    sxr_StopFunctionTimer(wland_smartlink_timer);
    if (smartlink->state != SMARTLINK_ENABLE && smartlink->state != SMARTLINK_READY)
        return ;

    WLAND_DBG(DEBUG, "Enter:time%d/%d\r\n",
        smartlink->channel_time, smartlink->channel_time_all);

    smartlink->channel_time ++;
    if (smartlink->channel_time >= smartlink->channel_time_all) {
        smartlink->state = SMARTLINK_READY;
        smartlink->current_channel ++;
        if (smartlink->current_channel == smartlink->channel_bits_len) {
            smartlink->smart_times ++;
            smartlink->current_channel = 0;
#ifdef WLAND_SMARTCONFIG_FROMDS_SUPPORT
            if (smartlink->from_ds == 0) {
                smartlink->from_ds = 1;
                WLAND_DBG(INFO, "SmartConfig FROM DS\r\n");
                wland_smartlink_setfilter(0xF7FFF);
            } else {
                smartlink->from_ds = 0;
                WLAND_DBG(INFO, "SmartConfig TO DS\r\n");
                wland_smartlink_setfilter(0x800F7FFF);
            }
#endif /*WLAND_SMARTCONFIG_FROMDS_SUPPORT*/
        }
        WLAND_DBG(DEBUG, "Enter, channel:%d\r\n",
            smartlink->channel_bits[smartlink->current_channel]);

        wland_smartlink_init();
        wland_smartlink_config(smartlink->channel_bits[smartlink->current_channel], 0xffff);

        smartlink->state = SMARTLINK_ENABLE;
    }
    sxr_StartFunctionTimer(SMART_CONFIG_TIMER MILLI_SECONDS,wland_smartlink_timer,0,0);
}

static int smartconfig_enable()
{
    int ret = 0;
    int i;
    unsigned char filter_type[4];
    UINT32 filter;
    UINT8 filter_type_len;
    printf("smartconfig_enable");
    smartlink = COS_MALLOC(sizeof(struct wland_smartlink));
    if (smartlink == NULL) {
        WLAND_DBG(ERROR,"malloc smartlink error\r\n");
        return -1;
    }
    smartlink->state = SMARTLINK_INIT;
    smartlink->current_channel = 0xff;
    smartlink->smart_times = 0;
    smartlink->from_ds = 0;// to ds, first time
#ifdef WLAND_SMARTCONFIG_SCAN_OPTIMIZED
    bool bit_map[14];
    memset(bit_map, 0, sizeof(bit_map));
    rda5981_scan(NULL, 0, 0);
    rda5981_scan(NULL, 0, 0);
    smartlink->channel_bits_len = 0;
    list_for_each_entry(bss, &bss_info.scan_list, list) {
        bit_map[bss->channel-1] = 1;
    }
    for (i=0; i<14; ++i) {
        if (bit_map[i])
            smartlink->channel_bits[smartlink->channel_bits_len++] = i+1;
    }
#else
    smartlink->channel_bits_len = 16;//1,6,11,1,2,...,13.no channel 14
    smartlink->channel_bits[0] = 1;
    smartlink->channel_bits[1] = 6;
    smartlink->channel_bits[2] = 11;
    for(i=1; i<=13; ++i)//1-13
        smartlink->channel_bits[2+i] = i;
#endif

    rda_smartconfig_state = SMARTLINK_ENABLE;

    smartlink->channel_time = 0;
    smartlink->channel_time_all = 1;

    if (smartlink->state != SMARTLINK_INIT) {
        WLAND_DBG(ERROR,"Smartlink has beed executed\r\n");
        smartlink->state = SMARTLINK_INIT;
    }

    ret = test_rda5890_generic_set_uchar(WID_USRCTL_RX_FRAME_FILTER,1);
    if (ret)
    {
        printf("smartconfig_enable WID_USRCTL_RX_FRAME_FILTER fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }

    smartlink->state = SMARTLINK_GET_FILTER;
    ret = test_rda5890_generic_get_str(WID_FILTER_TYPE, filter_type, &filter_type_len);
    if (ret)
    {
        printf("smartconfig_enable get WID_FILTER_TYPE fail");
        return ret;
    }
    g_saved_filter_type = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    printf("smartconfig_enable save filter_type %08x",g_saved_filter_type);

    ret = test_rda5890_generic_get_str(WID_MON_FILTER, filter_type, &filter_type_len);
    if (ret)
    {
        printf("smartconfig_enable get WID_MON_FILTER fail");
        return ret;
    }
    g_saved_monfilter_type = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    printf("smartconfig_enable save monfilter_type %08x",g_saved_monfilter_type);

    smartlink->state = SMARTLINK_STE_FILTER;
    ret = test_rda5890_generic_set_ulong(WID_FILTER_TYPE,SNIFF_FILTER_TYPE);
    if (ret)
    {
        printf("smartconfig_enable set WID_FILTER_TYPE fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }
#if 0
    ret = test_rda5890_generic_get_str(WID_FILTER_TYPE, filter_type, &filter_type_len);
    if (ret)
    {
        printf("smartconfig_enable get WID_FILTER_TYPE agian fail");
        return ret;
    }
    filter = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    if(filter != SNIFF_FILTER_TYPE)
    {
        printf("smartconfig_enable check WID_FILTER_TYPE fail %d",filter);
        return -1;
    }
#endif
    smartlink->state = SMARTLINK_READY;

    return 0;
}

static void wlan_smartlink_timeout(void)
{
    //sxr_StopFunctionTimer(wlan_smartlink_timeout);
    sys_arch_printf("smartconfig timeout happens\n");
    g_smartconfigStop = TRUE;
    COS_StopCallbackTimer(wifi_task_handle, wlan_smartlink_timeout, NULL);
    smartconfig_stop();

    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_SMART_CONFIG_COMPL;
    EV.nParam1 = FALSE;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

static int smartconfig_disable()
{
    int ret = 0;
    unsigned char filter_type[4];
    UINT32 filter;
    UINT8 filter_type_len;
    printf("smartconfig_disable");

    ret = test_rda5890_generic_set_ulong(WID_FILTER_TYPE,g_saved_filter_type);
    if (ret)
    {
        printf("smartconfig_disable WID_FILTER_TYPE fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }
#if 0
    ret = test_rda5890_generic_get_str(WID_FILTER_TYPE, filter_type, &filter_type_len);
    if (ret)
    {
        printf("smartconfig_disable get WID_FILTER_TYPE fail");
        return ret;
    }
    filter = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    if(filter != g_saved_filter_type)
    {
        printf("smartconfig_disable check WID_FILTER_TYPE fail");
        return -1;
    }
#endif
    ret = test_rda5890_generic_set_ulong(WID_MON_FILTER,g_saved_monfilter_type);
    if (ret)
    {
        printf("smartconfig_disable WID_MON_FILTER fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }
#if 0
    ret = test_rda5890_generic_get_str(WID_MON_FILTER, filter_type, &filter_type_len);
    if (ret)
    {
        printf("smartconfig_disable get WID_MON_FILTER fail");
        return ret;
    }
    filter = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    if(filter != g_saved_monfilter_type)
    {
        printf("smartconfig_disable check WID_MON_FILTER fail");
        return -1;
    }
#endif
    ret = test_rda5890_generic_set_uchar(WID_USRCTL_RX_FRAME_FILTER,0);
    if (ret)
    {
        printf("smartconfig_disable WID_USRCTL_RX_FRAME_FILTER fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }

    ret = test_rda5890_generic_set_uchar(WID_SCAN_STOP,0x1F);
    if (ret)
    {
        printf("smartconfig_disable WID_SCAN_STOP fail");
        return ret;
    }

    if (-1== sdio_getInputData())
    {
        //return -1;
    }

    rda_smartconfig_state = SMARTLINK_DONE;

    return 0;
}

int smartconfig_getssid_start(void)
{
    int ret = smartconfig_enable();
    if(ret)
    {
        printf("smartconfig_enable fail ret=%d",ret);
        return ret;
    }
    wland_smartlink_timer();
    COS_StartCallbackTimer(wifi_task_handle,40*1000,wlan_smartlink_timeout,NULL);
    return 0;
}

static char s_ssid[IW_ESSID_MAX_SIZE + 1];
static char s_password[64+1];
static u8 s_ipaddr[4];

static smartconfig_store_data(char *ssid, char *passwd, u8 *ip4_addr)
{
    strcpy(s_ssid, ssid);
    strcpy(s_password, passwd);
    memcpy(s_ipaddr, ip4_addr, 4);
}

BOOL smartconfig_getssid_end(char *ssid, char *passwd, u8 *ip4_addr)
{
    int ret = smartconfig_disable();
    printf("smartconfig_disable ret=%d",ret);
    if (strlen(smartlink->SSID) == 0) {
        wifi_ScanAps(5, NULL);
        return FALSE;
    } else {
        strcpy(ssid, smartlink->SSID);
        strcpy(passwd, smartlink->key);
        //memcpy(bssid, smartlink->BSSID, 6);
        memcpy(ip4_addr, smartlink->ip, 4);
        //*channel = smartlink->channel;
        smartconfig_store_data(ssid,passwd,ip4_addr);
        COS_FREE(smartlink);
        return TRUE;
    }
}
BOOL smartconfig_stop(void)
{
    int ret = 0;
    sxr_StopFunctionTimer(wland_smartlink_timer);
    ret = smartconfig_disable();
    wifi_ScanAps(1, NULL);

    if(smartlink != NULL)
        COS_Free(smartlink);
}

BOOL smartconfig_getssid_result(char *ssid, char *passwd, u8 *ip4_addr)
{
    int i;
    RDA5890_BSS_DESC* tmp_p = (RDA5890_BSS_DESC*)bss_Rst.bss_descs;

    for(i=0; i <  bss_Rst.count; i++)
    {
        u8 BSSID_crc, SSID_crc;
        BSSID_crc = wland_crc8(tmp_p[i].bssid, ETH_ALEN, 0);
        SSID_crc = wland_crc8(tmp_p[i].ssid, strlen(tmp_p[i].ssid), 0);
        printf("smartconfig_getssid_result %d SSID_crc=%d BSSID_crc=%d",i,SSID_crc,BSSID_crc);
        if (BSSID_crc==smartlink->BSSID_crc && SSID_crc==smartlink->SSID_crc) {
            strcpy(ssid, tmp_p[i].ssid);
            strcpy(passwd, smartlink->key);
            //memcpy(bssid, smartlink->BSSID, 6);
            memcpy(ip4_addr, smartlink->ip, 4);
            //*channel = smartlink->channel;
            smartconfig_store_data(ssid,passwd,ip4_addr);
            COS_FREE(smartlink);
            return TRUE;
        }
    }
    return FALSE;
}

static char hex_to_number(char a)
{
    if (a>='0' && a<='9')
        return a-'0';
    else if (a>='a' && a<='z')
        return a-'a'+0xa;
    else if (a>='A' && a<='Z')
        return a-'A'+0xa;

    printf("Invalid char: %02X\n", a);
    return '\0';
}

INT send_smartconfig_response_to_host()
{
    int i;
    INT32 iResult;
    SOCKET wifiSocket;
    CFW_TCPIP_SOCKET_ADDR nDestAddr;
    UINT8 sm_rsp[11];

    wifiSocket = CFW_TcpipSocket(CFW_TCPIP_AF_INET, CFW_TCPIP_SOCK_STREAM, CFW_TCPIP_IPPROTO_TCP);
    if(SOCKET_ERROR == wifiSocket)
    {
        printf("send_smartconfig_response_to_host CFW_TcpipSocket fail");
        return -1;
    }

    CFW_TCPIP_SOCKET_ADDR stLocalAddr = { 0, };
    struct netif *netif;

    netif = getEtherNetIf(0x11);

    if (netif ==NULL) {
        printf("send_smartconfig_response_to_host netif fail");
        return -1;
    }

    ip4_addr_t *ip_addr = netif_ip4_addr(netif);

    stLocalAddr.sin_len         = 0;
    stLocalAddr.sin_family      = CFW_TCPIP_AF_INET;
    stLocalAddr.sin_port        = 0;
    stLocalAddr.sin_addr.s_addr = ip_addr->addr;

    CFW_TcpipSocketBind(wifiSocket, &stLocalAddr, sizeof(CFW_TCPIP_SOCKET_ADDR));

	int NBIO = 0;
	CFW_TcpipSocketIoctl(wifiSocket, FIONBIO, &NBIO);

    nDestAddr.sin_family = CFW_TCPIP_AF_INET;
    nDestAddr.sin_port = htons(SMARTLING_REPORT_PORT);
    nDestAddr.sin_addr.s_addr = htonl(s_ipaddr[0]<<24 | s_ipaddr[1]<<16 | s_ipaddr[2]<<8 | s_ipaddr[3]);
    iResult = CFW_TcpipSocketConnect(wifiSocket, &nDestAddr, SIZEOF(CFW_TCPIP_SOCKET_ADDR));

    if (SOCKET_ERROR == iResult)
    {
        printf("send_smartconfig_response_to_host CFW_TcpipSocketConnect fail");
        return iResult;
    }

    sm_rsp[0] = strlen(s_ssid) + strlen(s_password) + SMARTLING_REPORT_MSG_OFFSET;
    printf("ssid:%s(%d),passwd:%s(%d)\r\n", s_ssid, strlen(s_ssid), s_password, strlen(s_password));

    for (i=0; i<6; ++i)
        sm_rsp[1+i] = localMac[i];

    memcpy(sm_rsp+7, s_ipaddr, 4);

    iResult = CFW_TcpipSocketSend(wifiSocket, (UINT8 *)sm_rsp, 11, 0);
    if (iResult < 0)
    {
        printf("send_smartconfig_response_to_host send data error");
        return -1;
    }
    CFW_TcpipSocketClose(wifiSocket);

    return 0;
}
#endif

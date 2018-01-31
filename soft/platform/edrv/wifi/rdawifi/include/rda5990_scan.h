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

#ifndef __RDA5990_SCAN_H__
#define __RDA5990_SCAN_H__


#define RDA5890_MAX_NETWORK_NUM    16
#define IW_ESSID_MAX_SIZE   34
#define ETH_ALEN 6

typedef  struct rda5990_bss_info
{
    //struct rda5890_bss_descriptor data;

    u8 bssid[ETH_ALEN];
    u8 ssid[IW_ESSID_MAX_SIZE + 1];
    u8 ssid_len;

    u16 capability;
    u32 rssi;
    u32 channel;
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

    //unsigned long last_scanned;
    //struct list_head list;
//

} RDA5990_BSS_INFO;



#endif   // __RDA5990_SCAN_H__


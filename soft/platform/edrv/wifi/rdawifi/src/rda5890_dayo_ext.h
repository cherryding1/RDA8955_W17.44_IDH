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


#ifndef _RDA5890_DAYO_EXT_H
#define _RDA5890_DAYO_EXT_H

void ext_wifi_chip_on(void);
void ext_wifi_chip_off(void);

void dayo_ext_wifi_init(void);
void dayo_ext_wifi_deinit(void);

void dayo_ext_wifi_set_calibration(kal_uint16 datalen, void *calidata);
kal_uint16 dayo_ext_wifi_get_calibration(void *calidata);

void dayo_ext_wifi_power_on(kal_uint8 *bssid); // bssid should be STA mac address
void dayo_ext_wifi_power_off(void);

void dayo_ext_wifi_scan_ap_list(void);
void dayo_ext_wifi_connect_profile(void *p);
void dayo_ext_wifi_disconnect_profile(void);
void dayo_ext_wifi_get_link_quality(void);

void IPFrameTxFunction(
    kal_uint8 *dstMac, kal_uint8 *srcMac,
    kal_uint16 frameLen, kal_uint8 *frame);

/* Callback prototypes */
void IPFrameRxFunction(
    kal_uint8 *dstMac, kal_uint8 *srcMac,
    kal_uint16 frameLen, kal_uint8 *frame);

#endif   /*_RDA5890_DAYO_EXT_H*/

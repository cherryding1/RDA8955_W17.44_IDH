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


#include "drv_features.h"
#include "kal_release.h"
#include "reg_base.h"
#include "app_buff_alloc.h"
#include "msdc_def.h"
#include "sd_def.h"
#include "sdio_def.h"
#include "drv_comm.h"
#include "intrCtrl.h"
#include "sdio_sw.h"
#include "cache_sw.h"
#include "init.h"
#include "dma_hw.h"
#include "Drv_trc.h"

#include "wndrv_cnst.h"
#include "wndrv_supc_types.h"
#include "supc_abm_msgs.h"
#include "rda5890.h"

//kal_char dayo_ext_dbg_str[200];

void ext_wifi_chip_on(void)
{
    rda5890_sdio_init();
}

void ext_wifi_chip_off(void)
{
}

void dayo_ext_wifi_init(void)
{
}

void dayo_ext_wifi_deinit(void)
{
}

void dayo_ext_wifi_set_calibration(kal_uint16 datalen, void *calidata)
{
}

kal_uint16 dayo_ext_wifi_get_calibration(void *calidata)
{
}

void dayo_ext_wifi_power_on(kal_uint8 *bssid)
{
}

void dayo_ext_wifi_power_off(void)
{
}

void dayo_ext_wifi_scan_ap_list(void)
{
    rda5890_start_scan();
}

void dayo_ext_wifi_connect_profile(void *p)
{
    wifi_data_account_profile_struct *profile = (wifi_data_account_profile_struct *)p;
    rda5890_set_ssid(profile->ssid, profile->ssid_len);
}

void dayo_ext_wifi_disconnect_profile(void)
{
}

void dayo_ext_wifi_get_link_quality(void)
{
}

kal_uint8 tx_data_buf[1660];

void IPFrameTxFunction(
    kal_uint8 *dstMac, kal_uint8 *srcMac,
    kal_uint16 frameLen, kal_uint8 *frame)
{
    memcpy(tx_data_buf, dstMac, 6);
    memcpy(tx_data_buf+6, srcMac, 6);
    memcpy(tx_data_buf+12, frame, frameLen);

    rda5890_data_tx((char *)tx_data_buf, frameLen + 12);
}


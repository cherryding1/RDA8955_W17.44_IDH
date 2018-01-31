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


#ifndef _RDA5890_H
#define _RDA5890_H

#include "kal_release.h"

#define RDA5890_MAX_NETWORK_NUM    16
#define IW_ESSID_MAX_SIZE   32
#define ETH_ALEN 6

typedef __packed struct rda5890_bss_descriptor
{
    unsigned char ssid[IW_ESSID_MAX_SIZE + 1];
    unsigned char bss_type;//2:ap
    unsigned char channel;
    unsigned char dot11i_info; //bit 0 : 0 no security
    unsigned char bssid[ETH_ALEN];
    unsigned char rssi;
    unsigned char auth_info;
    unsigned char rsn_cap[2];
} RDA5890_BSS_DESC;

/* RDA5890 functions */
kal_bool rda5890_sdio_init(void);
int rda5890_start_scan(void);
int rda5890_get_fw_ver(unsigned long *fw_ver);
int rda5890_get_mac_addr(unsigned char *mac_addr);
int rda5890_get_scan_results(RDA5890_BSS_DESC *bss_desc);
int rda5890_get_bssid(unsigned char *bssid);
int rda5890_get_channel(unsigned char *channel);
int rda5890_get_rssi(unsigned char *rssi);
int rda5890_set_mac_addr(unsigned char *mac_addr);
int rda5890_set_ssid(unsigned char *ssid, unsigned char ssid_len);

int rda5890_data_tx(char *data, unsigned short len);

/* Testing Functions */
void SDIO_test_init(kal_uint32 mode);

void SDIO_test_write_cmd52(kal_uint8 func, kal_uint32 addr, kal_uint8 data);
void SDIO_test_read_cmd52(kal_uint8 func, kal_uint32 addr, kal_uint8 *data);

void SDIO_test_reg32_write(kal_uint32 addr, kal_uint32 data);
void SDIO_test_reg32_read(kal_uint32 addr, kal_uint32 *data);

void SDIO_test_data_write(kal_uint8 *data, kal_uint32 len);
void SDIO_test_data_read(kal_uint8 *data, kal_uint32 *len);
void SDIO_test_data_read_isr(kal_uint8 *data, kal_uint32 *len);

void SDIO_test_set_blksize(kal_uint32 size);
void SDIO_test_data_write_blk(kal_uint8 *data, kal_uint32 len);
void SDIO_test_data_read_blk(kal_uint8 *data, kal_uint32 *len);

void SDIO_debug_read_msdc_reg(kal_uint32 addr, kal_uint32 *data);

#endif   /*_RDA5890_H*/

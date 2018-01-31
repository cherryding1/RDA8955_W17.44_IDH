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

#ifndef __WLAN_VERSION_H__
#define __WLAN_VERSION_H__
#define WIFI_VERSION_D  0x7

#define WIFI_VERSION_E  0x4
#define WIFI_VERSION_F  0x5


#define WIFI_VERSION_5991  0x4a
#define WIFI_VERSION_5991E  0x5a

#define WIFI_DELAY(DURATION) sxr_Sleep(DURATION  MS_WAITING)

#define ARRAY_SZ  ARRAY_SIZE
#define I2C_DELAY_FLAG (0xFFFF)
#define DELAY_MS(x) {I2C_DELAY_FLAG, x},
//#define  COMBO_WITH_26MHZ


UINT8 wifi_i2c_core_read_data(UINT32 Regiter, UINT32 *pData, UINT8 datalen);
HAL_ERR_T wifi_i2c_rf_write_data(unsigned char regaddr, const unsigned short *data, unsigned char datalen);
int rda5991e_sdio_patch(void);


#endif

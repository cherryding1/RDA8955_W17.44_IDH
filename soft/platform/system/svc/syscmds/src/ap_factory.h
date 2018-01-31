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

#ifndef __FACTORY_DATA_H__
#define __FACTORY_DATA_H__

#define RDA_CAL_DATA_LEN        8192
#define RDA_MODEM_FACT_LEN      4096
#define RDA_AP_FACT_LEN         4096
#define RDA_FACT_TOTAL_LEN      \
    (RDA_CAL_DATA_LEN + RDA_MODEM_FACT_LEN + RDA_AP_FACT_LEN)

#define AP_FACTORY_MAJOR_VERSION    1
#define AP_FACTORY_MINOR_VERSION    1
#define AP_FACTORY_MARK_VERSION     0xFAC40000
#define AP_FACTORY_VERSION_NUMBER   (AP_FACTORY_MARK_VERSION | \
        (AP_FACTORY_MAJOR_VERSION << 8) | AP_FACTORY_MINOR_VERSION)
#define AP_FACTORY_CLOCK_MAGIC      0x55515263

#define FACT_NAME_LEN       128
#define AP_FACTORY_CLOCK_CFG_LEN    1024

struct ap_factory_config
{
    unsigned int version;
    unsigned int crc;
    unsigned char lcd_name[FACT_NAME_LEN];
    unsigned char bootlogo_name[FACT_NAME_LEN];
    unsigned char clock_config[AP_FACTORY_CLOCK_CFG_LEN];
};

struct factory_data_sector
{
    unsigned char modem_calib_data[RDA_CAL_DATA_LEN];
    unsigned char modem_factory_data[RDA_MODEM_FACT_LEN];
    unsigned char ap_factory_data[RDA_AP_FACT_LEN];
};

unsigned long factory_get_all(unsigned char *buf);
const unsigned char* factory_get_modem_calib(void);
const unsigned char* factory_get_lcd_name(void);
const unsigned char* factory_get_bootlogo_name(void);
int factory_set_ap_factory(unsigned char *data);
int factory_set_modem_calib(unsigned char *data);
int factory_set_modem_factory(unsigned char *data);
int factory_burn(void);
int factory_update_modem_calib(unsigned char *data);
int factory_update_modem_factory(unsigned char *data);
int factory_update_ap_factory(unsigned char *data);
int factory_update_all(unsigned char *data, unsigned long size);

#endif

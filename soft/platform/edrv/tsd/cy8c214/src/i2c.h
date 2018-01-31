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




#ifndef _TSD_CAP_M_H_
#define _TSD_CAP_M_H_

#include "cs_types.h"
BOOL tsd_cap_gpio_i2c_init_sensor();
BOOL tsd_cap_gpio_i2c_write_data_sensor(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen);
UINT8 tsd_cap_gpio_i2c_read_data_sensor(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen);
UINT8 tsd_cap_gpio_i2c_read_data_sensor_continue(UINT32 addr,UINT8 *datas, UINT8 datalen);



#endif //_PMD_M_H_


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


#ifndef _EDRV_GPIO_I2C_H_
#define _EDRV_GPIO_I2C_H_

#include "hal_error.h"

typedef enum
{
    GPIO_I2C_BPS_80K,
    GPIO_I2C_BPS_160K

} GPIO_I2C_BPS_T;

/*
if you want get a stop condition befort restart condition , do following
1, call gpio_i2c_write_data(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen);
the datalen == 0,get a stop condition
2, call gpio_i2c_read_data(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen);
the addrlen== 0

 for example:
        gpio_i2c_write_data(gSensorInfo.i2caddress,&Addr,1, (unsigned char *)&data, 0);
    return(gpio_i2c_read_data(gSensorInfo.i2caddress,&Addr,0, data, 1));
*/





BOOL gpio_i2c_open(VOID);
BOOL gpio_i2c_close(VOID);

//BOOL gpio_i2c_init(VOID);

//VOID gpio_i2c_Start(VOID);

//VOID gpio_i2c_Stop(VOID);

//UINT8 gpio_i2c_TxByte(UINT8 data);

//VOID gpio_i2c_RxByte(UINT8 *data, UINT8 ack);

HAL_ERR_T gpio_i2c_write_data(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen);
UINT8 gpio_i2c_read_data(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen);
HAL_ERR_T gpio_i2c_write_datas(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen);
UINT8 gpio_i2c_read_data_with_delay(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen,UINT8 delay);
HAL_ERR_T gpio_i2c_write_data_with_delay(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen,UINT8 delay);


#endif




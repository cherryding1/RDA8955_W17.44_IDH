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



#include "cs_types.h"
//#include "drv.h"
#include "gsensor_config.h"
#include "gsensor_m.h"
#include "tgt_gsensor_cfg.h"
#include "gsensor_debug.h"
#include "mma7660fc.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "cmn_defs.h"
#include "hal_i2c.h"

#define DURATION_INIT_1     10//600ns 
#define DURATION_INIT_2     10//600ns 
#define DURATION_INIT_3     10//600ns 

#define DURATION_START_1    10//600ns 
#define DURATION_START_2    12//600ns 
#define DURATION_START_3    12//800ns 

#define DURATION_STOP_1 12//800ns 
#define DURATION_STOP_2 10//600ns 
#define DURATION_STOP_3 25//1300ns

#define DURATION_HIGH       20//900ns 
#define DURATION_LOW        28//1600ns
//#define DURATION_HIGH     25//900ns
//#define DURATION_LOW      35//1600ns

extern CONST TGT_GSENSOR_CONFIG_T*  g_gsensorConfig;

#define DELAY(DURATION)     { UINT32 j; for(j = DURATION; j >0; j--){}}


VOID Gss_GpioSetDirection_iic( UINT8 nGpio, UINT8 nDirection)
{

    if ((nGpio&0x1) == 0x1)  //gpo
    {
        ;
    }
    else
    {
        if (nDirection == 1)
        {
            hal_GpioSetOut(HAL_GPIO_0 + (nGpio>>1));
        }
        else
        {
            hal_GpioSetIn(HAL_GPIO_0 + (nGpio>>1));
        }

    }
}
VOID Gss_GpioSetLevel_iic(UINT8 nGpio,UINT8 nLevel)
{
    if ((nGpio&0x1) == 0x1)  //gpo
    {
        nGpio = nGpio >> 1;
        HAL_APO_ID_T gpio = {{
                .type = HAL_GPIO_TYPE_O,
                .id = nGpio
            }
        };
        if (nLevel == 1)
        {
            hal_GpioSet(gpio);
        }
        else
        {
            hal_GpioClr(gpio);
        }
    }
    else
    {
        nGpio = nGpio >> 1;
        HAL_APO_ID_T gpio = {{
                .type = HAL_GPIO_TYPE_IO,
                .id = nGpio
            }
        };
        if (nLevel == 1)
        {
            hal_GpioSet(gpio);
        }
        else
        {
            hal_GpioClr(gpio);
        }


    }
}
VOID Gss_GpioGetLevel_iic(UINT8 nGpio,UINT8* nLevel)
{
    if ((nGpio&0x1) == 0x1)  //gpo
    {
        ;
    }
    else
    {
        nGpio = nGpio >> 1;
        *nLevel = hal_GpioGet(HAL_GPIO_0 + nGpio);
    }
}

// Global variant

PRIVATE UINT32 SCL_PIN,SDA_PIN;
PRIVATE UINT8 level = 0;


PRIVATE VOID GPIO_InitIO(UINT8 direction, UINT8 port)
{
    Gss_GpioSetDirection_iic(port,direction);
}

PRIVATE VOID GPIO_WriteIO(UINT8 data, UINT8 port)
{
    Gss_GpioSetLevel_iic(port,data);
}
PRIVATE UINT8 GPIO_ReadIO(UINT8 port)
{
    Gss_GpioGetLevel_iic(port,&level);
    return level;
}


//
//-------------------------------------------------------------------
// Function:  gpio_i2c_init
// Purpose:  This function is used to init I2C port of the  device
// In:
// Return:    bool
//-------------------------------------------------------------------
BOOL gpio_i2c_init_gsensor(VOID)
{
    //SXS_TRACE(TSTDOUT,"init I2C ports... \r\n");
    //u32 iGpioSetting = hal_board_config.Used_Gpio;
    if (g_gsensorConfig->scl_i2c_gpio == HAL_GPIO_NONE)
    {
        SCL_PIN = (g_gsensorConfig->scl_i2c_gpo)&0xffff;
        SDA_PIN = (g_gsensorConfig->sda_i2c)&0xffff;
        SCL_PIN = (SCL_PIN << 1) + 1;
        SDA_PIN = (SDA_PIN << 1) + 0;
    }
    else
    {
        SCL_PIN = (g_gsensorConfig->scl_i2c_gpio)&0xffff;
        SDA_PIN = (g_gsensorConfig->sda_i2c)&0xffff;
        SCL_PIN = (SCL_PIN << 1) + 0;
        SDA_PIN = (SDA_PIN << 1) + 0;
    }
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gpio_i2c_init_gsensor scl = %d ,  sda = %d   ",SCL_PIN,SDA_PIN);
    //iTemp = SCL_PIN | SDA_PIN ;
    // Set the GPIO pin to output status
    Gss_GpioSetDirection_iic(SCL_PIN,1);
    Gss_GpioSetDirection_iic(SDA_PIN,1);
    DELAY(DURATION_INIT_1);

    // Make the I2C bus in idle status
    Gss_GpioSetLevel_iic(SDA_PIN,1);
    DELAY(DURATION_INIT_1);
    Gss_GpioSetLevel_iic(SCL_PIN,1);
    DELAY(DURATION_INIT_1);

    return (TRUE);
}

PRIVATE VOID gpio_i2c_Start(VOID) /* start or re-start */
{
    GPIO_InitIO(1,SDA_PIN);
    GPIO_WriteIO(1,SDA_PIN);
    GPIO_WriteIO(1,SCL_PIN);

    DELAY(DURATION_START_1);
    GPIO_WriteIO(0,SDA_PIN);
    DELAY(DURATION_START_2);
    GPIO_WriteIO(0,SCL_PIN);
    DELAY(DURATION_START_3);/* start condition */
}

PRIVATE VOID gpio_i2c_Stop(VOID)
{
    GPIO_WriteIO(0,SCL_PIN);
    DELAY(DURATION_LOW);
    GPIO_InitIO(1,SDA_PIN);
    GPIO_WriteIO(0,SDA_PIN);
    DELAY(DURATION_STOP_1);
    GPIO_WriteIO(1,SCL_PIN);
    DELAY(DURATION_STOP_2);
    GPIO_WriteIO(1,SDA_PIN); /* stop condition */
    DELAY(DURATION_STOP_3);
}

PRIVATE UINT8 gpio_i2c_TxByte(UINT8 data) /* return 0 --> ack */
{
    int32 i;
    uint8 temp_value = 0;
    for(i=7; (i>=0)&&(i<=7); i--)
    {
        GPIO_WriteIO( 0, SCL_PIN); /* low */
        DELAY(DURATION_LOW);
        if(i==7)GPIO_InitIO(1,SDA_PIN);
        DELAY(DURATION_LOW);

        GPIO_WriteIO(((data>>i)&0x01), SDA_PIN);
        DELAY(DURATION_LOW/2);
        GPIO_WriteIO( 1, SCL_PIN); /* high */
        DELAY(DURATION_HIGH);
    }
    GPIO_WriteIO(0, SCL_PIN); /* low */
    DELAY(DURATION_LOW);
    GPIO_InitIO(0,SDA_PIN);/* input  */
    DELAY(DURATION_LOW/2);
    GPIO_WriteIO(1, SCL_PIN); /* high */
    DELAY(DURATION_HIGH);
    temp_value = GPIO_ReadIO(SDA_PIN);
    GPIO_WriteIO(0, SCL_PIN); /* low */
    DELAY(DURATION_LOW);
    return temp_value;
}

PRIVATE VOID gpio_i2c_RxByte(uint8 *data, uint8 ack)
{
    int32 i;
    uint32 dataCache;

    dataCache = 0;
    for(i=7; (i>=0)&&(i<=7); i--)
    {
        GPIO_WriteIO(0, SCL_PIN);
        DELAY(DURATION_LOW);
        if(i==7)GPIO_InitIO(0,SDA_PIN);
        DELAY(DURATION_LOW);
        GPIO_WriteIO(1, SCL_PIN);
        DELAY(DURATION_HIGH);
        dataCache |= (GPIO_ReadIO(SDA_PIN)<<i);
        DELAY(DURATION_LOW/2);
    }

    GPIO_WriteIO(0, SCL_PIN);
    DELAY(DURATION_LOW);
    GPIO_InitIO(1,SDA_PIN);
    GPIO_WriteIO(ack, SDA_PIN);
    DELAY(DURATION_LOW/2);
    GPIO_WriteIO(1, SCL_PIN);
    DELAY(DURATION_HIGH);
    GPIO_WriteIO(0, SCL_PIN); /* low */
    DELAY(DURATION_LOW);
    *data = (uint8)dataCache;
}


VOID gpio_i2c_write_data_gsensor(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen)
{
    UINT8 i;

    gpio_i2c_Start();///start
    gpio_i2c_TxByte(addr<<1);//chip adress


    for(i=addrlen; i>0; i--,regaddr++) //addr
    {
        gpio_i2c_TxByte(*regaddr);
    }


    for( i=datalen; i>0; i--,data++) //data
    {
        gpio_i2c_TxByte(*data);
    }
    gpio_i2c_Stop();
}

UINT8 gpio_i2c_read_data_gsensor(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen)
{
    UINT8 tempdata;
    UINT8 i;
    UINT8 *data = datas;

    gpio_i2c_Start();///start
    gpio_i2c_TxByte(addr<<1);//chip adress

    for(i=addrlen; i>0; i--,regaddr++) //addr
    {
        gpio_i2c_TxByte(*regaddr);
    }

    gpio_i2c_Start();//start
    gpio_i2c_TxByte((addr<<1)+1);//chip adress

    for( i=datalen-1; i>0; i--,data++) //data
    {
        gpio_i2c_RxByte(&tempdata, 0);
        *data = tempdata;
// GSS_TRACE(GSS_INFO_TRC, 0, "gss: gpio read 1*datas 0x%x",tempdata);
    }

    gpio_i2c_RxByte(&tempdata, 1);
    *data = tempdata;

    gpio_i2c_Stop();
// GSS_TRACE(GSS_INFO_TRC, 0, "gss: gpio read end *datas 0x%x",*data);
    return *datas;
}

PUBLIC HAL_ERR_T hal_I2cSendByte_gpio(UINT32 slaveAddr, UINT8 memAddr, UINT8 data)
{


#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data_gsensor(slaveAddr,&memAddr,1,&data,1);
#else
    hal_I2cSendRawByte(HAL_I2C_BUS_ID_2, ((slaveAddr<<1) & 0xfe), PHASE3_WR_PH1);
    hal_I2cSendRawByte(HAL_I2C_BUS_ID_2, memAddr, PHASE3_WR_PH2);
    hal_I2cSendRawByte(HAL_I2C_BUS_ID_2, data, PHASE3_WR_PH3);

#endif

    return HAL_ERR_NO;
}


PUBLIC HAL_ERR_T hal_I2cSendData_gpio(UINT32 slaveAddr,  UINT8 memAddr,UINT8* pData, UINT8 length)
{
#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_write_data_gsensor(slaveAddr,&memAddr,1,pData,length);
#else
    hal_I2cSendData(HAL_I2C_BUS_ID_2,slaveAddr,memAddr,pData,length);
#endif
    return HAL_ERR_NO;
}

PUBLIC HAL_ERR_T hal_I2cGetData_gpio(UINT32 slaveAddr,   UINT8 memAddr,UINT8* pData, UINT8 length)
{
#ifdef I2C_BASED_ON_GPIO
#error
    gpio_i2c_read_data_gsensor(slaveAddr,&memAddr,1,pData,length);
#else
    hal_I2cGetData(HAL_I2C_BUS_ID_2,slaveAddr,memAddr,pData,length);
#endif

    return HAL_ERR_NO;
}

PUBLIC HAL_ERR_T hal_I2cGetByte_gpio(UINT32 slaveAddr, UINT8 memAddr, UINT8* pData)
{
#ifdef I2C_BASED_ON_GPIO

    gpio_i2c_read_data_gsensor(slaveAddr,&memAddr,1,pData,1);

#else
    hal_I2cGetByte(HAL_I2C_BUS_ID_2, slaveAddr,memAddr,pData);

#endif
    return HAL_ERR_NO;
}





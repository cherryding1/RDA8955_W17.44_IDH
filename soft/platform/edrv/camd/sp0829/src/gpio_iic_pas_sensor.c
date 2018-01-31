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


#ifdef I2C_BASED_ON_GPIO
#include "cs_types.h"
//#include "drv.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxr_tls.h"

#if 0
#define DURATION_INIT_1     10//600ns 
#define DURATION_INIT_2     10//600ns 
#define DURATION_INIT_3     10//600ns 

#define DURATION_START_1    10//600ns 
#define DURATION_START_2    12//600ns 
#define DURATION_START_3    12//800ns 

#define DURATION_STOP_1 12//800ns 
#define DURATION_STOP_2 10//600ns 
//#define DURATION_STOP_3   25//1300ns

//#define DURATION_HIGH     20//900ns
//#define DURATION_LOW      28//1600ns
#else

#define DURATION_INIT_1     15//600ns 
#define DURATION_INIT_2     15//600ns 
#define DURATION_INIT_3     15//600ns 

#define DURATION_START_1    15//600ns 
#define DURATION_START_2    25//18//600ns 
#define DURATION_START_3    18//800ns 

#define DURATION_STOP_1 18//800ns 
#define DURATION_STOP_2 15//600ns 
#define DURATION_STOP_3 35//1300ns



#define DURATION_HIGH       27//900ns 
#define DURATION_LOW        36//1600ns
#endif

#define DELAY(DURATION)     {  volatile UINT32 j; for(j = DURATION; j >0; j--){}}


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
BOOL init_first = FALSE;
BOOL SP0829_gpio_i2c_init_sensor(VOID)
{
    if (init_first == FALSE)
    {
        init_first = TRUE;
    }
    else
    {
        return TRUE;
    }
    //SXS_TRACE(TSTDOUT,"init I2C ports... \r\n");
    //u32 iGpioSetting = hal_board_config.Used_Gpio;

    SCL_PIN = (25 << 1) + 0;
    SDA_PIN = (24 << 1) + 0;



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
PRIVATE VOID gpio_i2c_Start_bak(VOID) /* start or re-start */
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
    DELAY(DURATION_LOW / 4);
    GPIO_InitIO(0,SDA_PIN);/* input  */
    DELAY(DURATION_LOW);
    GPIO_WriteIO(1, SCL_PIN); /* high */
    DELAY(DURATION_HIGH *3 );
    temp_value = GPIO_ReadIO(SDA_PIN);
    GPIO_WriteIO(0, SCL_PIN); /* low */
    DELAY(DURATION_LOW*2);
// if(temp_value == 1)hal_DbgAssert("ack error");
    return temp_value;
}

PRIVATE UINT8 gpio_i2c_TxByte_bak(UINT8 data) /* return 0 --> ack */
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
    //DELAY(DURATION_LOW / 4);
    GPIO_InitIO(0,SDA_PIN);/* input  */
    DELAY(DURATION_LOW/2);
    GPIO_WriteIO(1, SCL_PIN); /* high */
    DELAY(DURATION_HIGH *2 );
    temp_value = GPIO_ReadIO(SDA_PIN);
    GPIO_WriteIO(0, SCL_PIN); /* low */
    DELAY(DURATION_LOW*2);
// if(temp_value == 1)hal_DbgAssert("ack error");
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

PRIVATE VOID gpio_i2c_RxByte_bak(uint8 *data)
{
    int32 i;
    uint32 dataCache;
    uint8 temp_value = 0;
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
    //DELAY(DURATION_LOW);
    GPIO_InitIO(0,SDA_PIN);
    DELAY(DURATION_LOW/2);
    GPIO_WriteIO(1, SCL_PIN);
    DELAY(DURATION_HIGH);
    temp_value = GPIO_ReadIO(SDA_PIN);
    GPIO_WriteIO(0, SCL_PIN); /* low */
    DELAY(DURATION_LOW);
    *data = (uint8)dataCache;
}

BOOL pas_gpio_i2c_write_data_sensor(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen)
{
    UINT8 i;
    UINT32 scStatus = hal_SysEnterCriticalSection();

    gpio_i2c_Start();///start
    if(1==gpio_i2c_TxByte(addr<<1)) {hal_SysExitCriticalSection(scStatus); return FALSE;}//chip adress


    for(i=addrlen; i>0; i--,regaddr++) //addr
    {
        if(gpio_i2c_TxByte(*regaddr) == 1) { hal_SysExitCriticalSection(scStatus); return FALSE;}
    }


    for( i=datalen; i>0; i--,data++) //data
    {
        if(gpio_i2c_TxByte(*data) == 1) { hal_SysExitCriticalSection(scStatus); return FALSE;}
    }
    gpio_i2c_Stop();
    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}

UINT8 pas_gpio_i2c_read_data_sensor(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen)
{
    UINT8 tempdata;
    UINT8 i;
    UINT8 *data = datas;
    UINT32 scStatus = hal_SysEnterCriticalSection();

    gpio_i2c_Start();//start
    if(1== gpio_i2c_TxByte((addr<<1)+0)) hal_DbgAssert("no ack");

    if(1== gpio_i2c_TxByte(*regaddr))  hal_DbgAssert("no ack");
    gpio_i2c_Stop();

    gpio_i2c_Start();//start
    if(1== gpio_i2c_TxByte((addr<<1)+1))  hal_DbgAssert("no ack");

    for( i=datalen-1; i>0; i--,data++) //data
    {
        gpio_i2c_RxByte(&tempdata, 0);
        *data = tempdata;
//  GSS_TRACE(GSS_INFO_TRC, 0, "gss: gpio read 1*datas 0x%x",tempdata);
    }

    gpio_i2c_RxByte(&tempdata,1);
    *data = tempdata;

    gpio_i2c_Stop();
// GSS_TRACE(GSS_INFO_TRC, 0, "gss: gpio read end *datas 0x%x",*data);
    hal_SysExitCriticalSection(scStatus);

    return *datas;
}

#if 0
VOID test_write111()
{
    UINT8 data_temp = 0x0;
    UINT8 reg_temp = 0xfe;


    pas_gpio_i2c_write_data_sensor(0x40,&reg_temp,1,&data_temp,1);
    reg_temp = 0x0;
    hal_HstSendEvent(0x300007);// DELAY(DURATION_LOW);

    gpio_i2c_read_data_gsensor(0x40,&reg_temp, 1,&data_temp,1);
    hal_HstSendEvent((UINT32)data_temp);
    reg_temp = 0x1;
    gpio_i2c_read_data_gsensor(0x40,&reg_temp, 1,&data_temp,1);
    hal_HstSendEvent((UINT32)data_temp);
    data_temp = 0x1;
    reg_temp = 0xfe;

    gpio_i2c_write_data_gsensor(0x40,&reg_temp,1,&data_temp,1);
    reg_temp = 0x0;
    hal_HstSendEvent(0x300007);// DELAY(DURATION_LOW);

    gpio_i2c_read_data_gsensor(0x40,&reg_temp, 1,&data_temp,1);
    hal_HstSendEvent((UINT32)data_temp);
    reg_temp = 0x1;
    gpio_i2c_read_data_gsensor(0x40,&reg_temp, 1,&data_temp,1);
    hal_HstSendEvent((UINT32)data_temp);


}
VOID test_write()
{
    UINT8 data = 0;
    UINT8 reg = 0;
    UINT8 temp = 0;
    reg = 0xfe; data = 0;
    gpio_i2c_write_data_gsensor(0x40,&reg,1,&data,1);
    reg = 0x0;
    gpio_i2c_read_data_gsensor(0x40,&reg, 1,&temp,1);
    hal_HstSendEvent(0x55555);
    hal_HstSendEvent((UINT32)temp);


}
#endif


VOID change_II2c_voltage(BOOL open)
{

    Gss_GpioSetDirection_iic(SCL_PIN,1);
    Gss_GpioSetDirection_iic(SDA_PIN,1);
    DELAY(DURATION_INIT_1);
    if(open == FALSE)
    {
        // Make the I2C bus in idle status
        Gss_GpioSetLevel_iic(SDA_PIN,0);
        Gss_GpioSetLevel_iic(SCL_PIN,0);
    }
    else
    {
        Gss_GpioSetLevel_iic(SDA_PIN,1);
        Gss_GpioSetLevel_iic(SCL_PIN,1);
    }

}
#endif


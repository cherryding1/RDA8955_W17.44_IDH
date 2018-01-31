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

#include "gsensor_config.h"
#include "gsensor_m.h"
#include "tgt_gsensor_cfg.h"
#include "gsensor_debug.h"
#include "msa300.h"
#include "hal_i2c.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "cmn_defs.h"
#include "cos.h"
#include "event.h"

// =============================================================================
//  MACROS
// =============================================================================
/******************************************
SD0=GND:
i2c_addr = 0x26;

SD0=High:
i2c_addr = 0x27;
*******************************************/
UINT8 msa300_i2c_addr = 0x26;
HAL_GPIO_CFG_T g_gsensor_gpioCfg;
PRIVATE CONST TGT_GSENSOR_CONFIG_T *g_gsensorConfig;

INT16 offset_x, offset_y, offset_z;

VOID msa300_DelayMS(UINT32 delay)
{
    hal_TimDelay(delay MS_WAITING);
}

VOID gsensor_I2cOpen(VOID)
{
    hal_I2cOpen(g_gsensorConfig->i2cBusId);
}

VOID gsensor_I2cClose(VOID)
{
    hal_I2cClose(g_gsensorConfig->i2cBusId);
}

UINT8 i2c_read_block_data(UINT8 base_addr, UINT8 count, UINT8 *data)
{
    GSENSOR_ERR_T ret = GSENSOR_ERR_NO;
    UINT8 i = 0;
    while (i < count)
    {
        ret = hal_I2cGetByte(g_gsensorConfig->i2cBusId, msa300_i2c_addr, (base_addr + i), (data + i));
        if (ret != GSENSOR_ERR_NO)
        {
            GSS_TRACE(GSS_INFO_TRC, 0, "gss: single read I2C busy ! ret = %d", ret);
        }
        i++;
    }

    return count;
}

GSENSOR_ERR_T msa300_register_read(UINT8 addr, UINT8 *data)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;

    sensor_err = hal_I2cGetByte(g_gsensorConfig->i2cBusId, msa300_i2c_addr, addr, data);
    if (sensor_err != HAL_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: single read I2C busy ! sensor_err = %d", sensor_err);
    }
    return GSENSOR_ERR_NO;
}

GSENSOR_ERR_T msa300_register_write(UINT8 addr, UINT8 data)
{
    hal_HstSendEvent(0x22000006);
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    sensor_err = hal_I2cSendByte(g_gsensorConfig->i2cBusId, msa300_i2c_addr, addr, data);
    if (sensor_err != HAL_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: single write I2C busy ! sensor_err = %d", sensor_err);
    }
    return GSENSOR_ERR_NO;
}

GSENSOR_ERR_T msa300_register_mask_write(UINT8 addr, UINT8 mask, UINT8 data)
{
    GSENSOR_ERR_T ret = GSENSOR_ERR_NO;
    UINT8 tmp_data;

    ret = msa300_register_read(addr, &tmp_data);
    if (ret != GSENSOR_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: single read I2C busy ! res = %d", ret);
        return ret;
    }

    tmp_data &= ~mask;
    tmp_data |= data & mask;
    ret = msa300_register_write(addr, tmp_data);
    if (ret != GSENSOR_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: single wirte I2C busy ! res = %d", ret);
        return ret;
    }

    return GSENSOR_ERR_NO;
}

GSENSOR_ERR_T msa300_register_read_continuously(UINT8 addr, UINT8 *data, UINT8 count)
{
    GSENSOR_ERR_T res = GSENSOR_ERR_NO;

    res = (count == i2c_read_block_data(addr, count, data)) ? GSENSOR_ERR_NO : GSENSOR_ERR_BAD_PARAMETER;
    if (res != GSENSOR_ERR_NO)
    {
        return res;
    }

    return res;
}

VOID msa300_read_adc(INT16 *x, INT16 *y, INT16 *z)
{
    UINT8 tmp_data[6] = {0};

    if (msa300_register_read_continuously(NSA_REG_ACC_X_LSB, tmp_data, 6) != 0)
    {
        return;
    }

    *x = (INT16)((tmp_data[1] << 4) | (tmp_data[0] >> 4));
    *y = (INT16)((tmp_data[3] << 4) | (tmp_data[2] >> 4));
    *z = (INT16)((tmp_data[5] << 4) | (tmp_data[4] >> 4));

    *x = *x + offset_x;
    *y = *y + offset_y;
    *z = *z + offset_z;

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:msa300_read_adc x = %d y = %d z = %d", *x, *y, *z);
    return;
}

/*return value: 0: is ok    other: is failed*/
UINT8 msa300_read_data(INT16 *x, INT16 *y, INT16 *z)
{
    UINT8 tmp_data[6] = {0};

    if (msa300_register_read_continuously(NSA_REG_ACC_X_LSB, tmp_data, 6) != 0)
    {
        return -1;
    }

    *x = ((INT16)(tmp_data[1] << 8 | tmp_data[0])) >> 4;
    *y = ((INT16)(tmp_data[3] << 8 | tmp_data[2])) >> 4;
    *z = ((INT16)(tmp_data[5] << 8 | tmp_data[4])) >> 4;

    *x = *x + offset_x;
    *y = *y + offset_y;
    *z = *z + offset_z;

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:msa300_read_data x = %d y = %d z = %d", *x, *y, *z);

    return 0;
}

GSENSOR_ERR_T msa300_set_enable(UINT8 enable)
{
    GSENSOR_ERR_T res = GSENSOR_ERR_NO;

    gsensor_I2cOpen();

    if (enable)
        res = msa300_register_mask_write(NSA_REG_POWERMODE_BW, 0xC0, 0x5E);
    else
        res = msa300_register_mask_write(NSA_REG_POWERMODE_BW, 0xC0, 0x80);

    gsensor_I2cClose();

    return res;
}

VOID msa300_pwr_up(VOID)
{

    msa300_set_enable(1);
}

VOID msa300_pwr_down(VOID)
{
    msa300_set_enable(0);
}

extern HANDLE GetWrtTaskHandle_Main(void);
UINT8 gsensor_shake = FALSE;

VOID gsensor_Irq_DetectDelay(VOID)
{
    sxr_StopFunctionTimer(gsensor_Irq_DetectDelay);

#if 0
		COS_EVENT event = { 0 };		
		BOOL r			= FALSE;
	
		gsensor_shake = TRUE;
		
		event.nEventId							 = EV_APP_GSENSOR_I2C_RECV_IND;
		event.nParam1							 = (UINT32)(gsensor_shake);
		r = COS_SendEvent(GetWrtTaskHandle_Main(), &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
		gsensor_shake = FALSE;
#endif
    hal_HstSendEvent(0x88888888);

    gsensor_ata_test();

    g_gsensor_gpioCfg.irqMask.rising = TRUE;
    g_gsensor_gpioCfg.irqMask.debounce = TRUE;
    g_gsensor_gpioCfg.irqMask.level = FALSE;
    g_gsensor_gpioCfg.irqMask.falling = FALSE;

    hal_GpioIrqSetMask(g_gsensorConfig->gsensorGpio, &g_gsensor_gpioCfg.irqMask);
    hal_GpioResetIrq(g_gsensorConfig->gsensorGpio);
}

PRIVATE VOID gsensor_GpioIrqHandler(VOID)
{
    g_gsensor_gpioCfg.irqMask.rising = FALSE;
    g_gsensor_gpioCfg.irqMask.debounce = TRUE;
    g_gsensor_gpioCfg.irqMask.level = FALSE;
    g_gsensor_gpioCfg.irqMask.falling = FALSE;

    hal_GpioIrqSetMask(g_gsensorConfig->gsensorGpio, &g_gsensor_gpioCfg.irqMask);
    hal_GpioResetIrq(g_gsensorConfig->gsensorGpio);

    sxr_StopFunctionTimer(gsensor_Irq_DetectDelay);
    sxr_StartFunctionTimer((1 MILLI_SECONDS), gsensor_Irq_DetectDelay, NULL, 0);
}

VOID msa300_init(VOID)
{
    GSENSOR_ERR_T res = GSENSOR_ERR_NO;
    UINT8 data = 0;
    hal_HstSendEvent(0x22000002);
    g_gsensorConfig = tgt_GetGsensorConfig();

    g_gsensor_gpioCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    g_gsensor_gpioCfg.irqMask.rising = TRUE;
    g_gsensor_gpioCfg.irqMask.debounce = TRUE;
    g_gsensor_gpioCfg.irqMask.level = FALSE;
    g_gsensor_gpioCfg.irqMask.falling = FALSE;
    g_gsensor_gpioCfg.irqHandler = gsensor_GpioIrqHandler;
    hal_GpioOpen(g_gsensorConfig->gsensorGpio, &g_gsensor_gpioCfg);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:g_gsensorConfig->i2cBusId = %d", g_gsensorConfig->i2cBusId);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:g_gsensor_gpioCfg.direction=%d g_gsensorConfig->gsensorGpio = %d", g_gsensor_gpioCfg.direction, g_gsensorConfig->gsensorGpio);

    gsensor_I2cOpen();
    msa300_register_read(NSA_REG_WHO_AM_I, &data);

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:read id = %d", data);

    if (data != 0x13)
    {
        msa300_i2c_addr = 0x4e;
        msa300_register_read(NSA_REG_WHO_AM_I, &data);
        if (data != 0x13)
        {
            GSS_TRACE(GSS_INFO_TRC, 0, "gss:msa300 read chip id  error = %d", data);
            return; // -1;
        }
    }

    res = msa300_register_mask_write(NSA_REG_SPI_I2C, 0x24, 0x24);

    msa300_DelayMS(10);

    res |= msa300_register_mask_write(NSA_REG_G_RANGE, 0xFF, 0x0A);
    res |= msa300_register_mask_write(NSA_REG_POWERMODE_BW, 0xFF, 0x5E);
    res |= msa300_register_mask_write(NSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x08);

    res |= msa300_register_mask_write(NSA_REG_INT_PIN_CONFIG, 0x0F, 0x05); //set int_pin level
    res |= msa300_register_mask_write(NSA_REG_INT_LATCH, 0x8F, 0x86);      //clear latch and set latch mode

    res |= msa300_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x83);
    res |= msa300_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x69);
    res |= msa300_register_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0xBD);
    res |= msa300_register_mask_write(NSA_REG_SWAP_POLARITY, 0xFF, 0x00);
    gsensor_I2cClose();

    return;
}

GSENSOR_ERR_T msa300_open_interrupt(int num)
{
    GSENSOR_ERR_T res = GSENSOR_ERR_NO;

    hal_HstSendEvent(0x22000003);
    gsensor_I2cOpen();
    res = msa300_register_write(NSA_REG_INTERRUPT_SETTINGS1, 0x87);

    res = msa300_register_write(NSA_REG_ACTIVE_DURATION, 0x02); //  0x00
#if defined(CUSTOMER_NAME_SHU)
    res = msa300_register_write(NSA_REG_ACTIVE_THRESHOLD, 0x1F);     //set int threhole 0x06 - 0xff more little more sensitice  //0x1f  0X0a(27/5)0X1a(27/5)
    res = msa300_register_mask_write(NSA_REG_INT_LATCH, 0xFF, 0x88); //0x55-4s 	0xDD-50ms  0X44-2s 0X33-1s   0x22-500ms 延时  //0x22
#else
    res = msa300_register_write(NSA_REG_ACTIVE_THRESHOLD, 0x0F); //set int threhole 0x06 - 0xff more little more sensitice  //0x1f  0X0a(27/5)0X1a(27/5)
    res = msa300_register_mask_write(NSA_REG_INT_LATCH, 0xFF, 0x88); //0x55-4s 	0xDD-50ms  0X44-2s 0X33-1s   0x22-500ms 延时  //0x22
#endif

    switch (num)
    {

    case 1:
        res = msa300_register_write(NSA_REG_INTERRUPT_MAPPING1, 0x04);
        break;

    case 2:
        res = msa300_register_write(NSA_REG_INTERRUPT_MAPPING3, 0x04);
        break;
    default:
        break;
    }
    gsensor_I2cClose();

    return res;
}

GSENSOR_ERR_T msa300_close_interrupt(int num)
{
    GSENSOR_ERR_T res = GSENSOR_ERR_NO;

    gsensor_I2cOpen();
    res = msa300_register_write(NSA_REG_INTERRUPT_SETTINGS1, 0x00);

    switch (num)
    {

    case 0:
        res = msa300_register_write(NSA_REG_INTERRUPT_MAPPING1, 0x00);
        break;

    case 1:
        res = msa300_register_write(NSA_REG_INTERRUPT_MAPPING3, 0x00);
        break;
    }

    gsensor_I2cOpen();

    return res;
}

VOID msa300_clear_int(VOID)
{
    gsensor_I2cOpen();
    msa300_register_mask_write(NSA_REG_INT_PIN_CONFIG, 0x80, 0x80);
    gsensor_I2cOpen();
}

PUBLIC VOID gsensor_Init(BOOL enable_int)
{
    hal_HstSendEvent(0x22000001);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:gsensor_init msa300");
    msa300_init();
    msa300_open_interrupt(1);
}

VOID gsensor_ata_test(VOID)
{
    UINT8 data = 0;
    UINT8 buff[129] = {0};
    INT16 x = 0, y = 0, z = 0;

    gsensor_I2cOpen();

    msa300_read_adc(&x, &y, &z);
    hal_HstSendEvent(0x22200001);
    hal_HstSendEvent(x);
    hal_HstSendEvent(y);
    hal_HstSendEvent(z);
    hal_HstSendEvent(0x22200002);

    gsensor_I2cClose();

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:gsensor_ata_test x = %d y= %d z = %d", x, y, z);
}

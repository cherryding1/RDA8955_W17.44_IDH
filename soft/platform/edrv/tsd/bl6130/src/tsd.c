#include "cs_types.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxr_tls.h"
#include "pmd_m.h"
#include "hal_gpio.h"
#include "hal_host.h"
#include "hal_i2c.h"
#include "tsdp_debug.h"
#include "tsd_config.h"
#include "tsdp_calib.h"
#include "tsd_m.h"
#include "tgt_tsd_cfg.h"

#include "gpio_i2c.h"
#include "lcdd.h"

#include "bl_ts.h"

#define PMD_POWER_TP  PMD_POWER_CAMERA

// I2C
#define I2C_MASTER_ACK              (1<<0)
#define I2C_MASTER_RD               (1<<4)
#define I2C_MASTER_STO              (1<<8)
#define I2C_MASTER_WR               (1<<12)
#define I2C_MASTER_STA              (1<<16)
#define	I2C_SLAVE_ADDR              CTP_SLAVE_ADDR //0x15

// TP
#define TD_STAT_ADDR BL_TD_STAT_ADDR //0x02
#define TD_STAT_NUMBER_TOUCH BL_TD_STAT_NUMBER_TOUCH //0x07
#define TD_TOUCH_POINT_COUNT 0X02 //0x04
#define	TOUCH1_XH_ADDR	0x03
#define	TOUCH2_XH_ADDR	0x09
#define	TOUCH3_XH_ADDR	0x0F
#define	TOUCH4_XH_ADDR	0x15

// Dec
#define DELTA   (UINT32)0x9e3779b9
#define CYCLE_C 16
#define K_A     (UINT32)0x12345678
#define K_B     (UINT32)0x87654321
#define K_C     (UINT32)0x13579086
#define K_D     (UINT32)0x24689753

#define GPIO_EINT

extern TSD_CONFIG_T*	g_tsdConfig_bl;
extern uint8_t bl_erase_flash(void);
extern BOOL gpio_i2c_init_ts(VOID);

void bl_ts_set_intmode(uint8_t mode);
void ctp_i2c_init(void);



PRIVATE CONST TSD_CONFIG_T*			g_tsdConfig;
PRIVATE TSD_CALLBACK_T      	 	g_userCallback;
PRIVATE TSD_REPORTING_MODE_T    	g_userReportingMode;
PRIVATE HAL_GPIO_CFG_T              g_gpioIrqCfg;
PRIVATE HAL_APO_ID_T                g_apoRstCfg;
PRIVATE TSD_POINT_T                 g_touchPoints[TD_TOUCH_POINT_COUNT];
PRIVATE TSD_PEN_STATE_T             g_lastPenState;
PRIVATE TSD_POINT_T                 g_lastPoint;
PRIVATE BOOL                        g_isCalibtated = TRUE;
PRIVATE UINT8                       g_powerMode = TSD_POWER_SLEEP;
PRIVATE CHAR                        g_hwversion[8] = {'U','\0',};

PRIVATE UINT8 g_PointValueAddr[] =
	{
		TOUCH1_XH_ADDR,
		TOUCH2_XH_ADDR,
		TOUCH3_XH_ADDR,
		TOUCH4_XH_ADDR,
	};

void ctp_delay_ms(uint16_t n)
{
	hal_TimDelay(n MS_WAITING);
}

PRIVATE VOID tsd_Reset(VOID)
{
	g_apoRstCfg.type = HAL_GPIO_TYPE_IO;
	g_apoRstCfg.gpioId = g_tsdConfig->pinReset;
	hal_GpioSetOut(g_apoRstCfg.gpioId);

	hal_GpioSet(g_apoRstCfg);
	hal_TimDelay(100 MS_WAITING);
	hal_GpioClr(g_apoRstCfg);
	hal_TimDelay(100 MS_WAITING);
	hal_GpioSet(g_apoRstCfg);

}

#if 1
PRIVATE HAL_ERR_T tsd_I2cWriteBytes(UINT8 *data, UINT32 len)
{
	HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    halErr = hal_I2cOpen(g_tsdConfig->i2cBusId);
    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, (I2C_SLAVE_ADDR << 1), I2C_MASTER_STA | I2C_MASTER_WR);
    for(int i = 0; i < len - 1; i++, data++)
    {
        halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, *data, I2C_MASTER_WR);
    }
    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, *data, I2C_MASTER_WR | I2C_MASTER_STO);
	hal_I2cClose(g_tsdConfig->i2cBusId);

	return halErr;
}
#else
PRIVATE HAL_ERR_T tsd_I2cWriteBytes(UINT8 *data, UINT32 len)
{
    HAL_ERR_T halErr;

    gpio_i2c_open();
    halErr = gpio_i2c_write_data(I2C_SLAVE_ADDR, NULL, 0, data, len);
    if (halErr != HAL_ERR_NO)
    {
        hal_HstSendEvent(0x5827e004);
    }
    gpio_i2c_close();

    return halErr;
}
#endif

#if 1
PRIVATE HAL_ERR_T tsd_I2cReadBytes(UINT8 addr, UINT8 *data, UINT32 len)
{
    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    halErr = hal_I2cOpen(g_tsdConfig->i2cBusId);
    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, (I2C_SLAVE_ADDR << 1), I2C_MASTER_STA | I2C_MASTER_WR);
    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, addr, I2C_MASTER_WR);
    halErr = hal_I2cSendRawByte(g_tsdConfig->i2cBusId, (I2C_SLAVE_ADDR << 1) | 0x01, I2C_MASTER_STA | I2C_MASTER_WR);
    for(int i = 0; i < len - 1; i++, data++)
    {
        (*data) = hal_I2cReadRawByte(g_tsdConfig->i2cBusId, I2C_MASTER_RD);
    }
    (*data) = hal_I2cReadRawByte(g_tsdConfig->i2cBusId, I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO);
	hal_I2cClose(g_tsdConfig->i2cBusId);

	return halErr;
}
#else
PRIVATE HAL_ERR_T tsd_I2cReadBytes(UINT8 addr, UINT8 *data, UINT32 len)
{
    gpio_i2c_open();
    gpio_i2c_read_data(I2C_SLAVE_ADDR, &addr, 1, data, len);
    gpio_i2c_close();
    return HAL_ERR_NO;
}
#endif

//define update tp firmware from code
//#define CTP_USE_FIRMWARE_UPDATE
#define BL_CTP_USE_FIRMWARE_UPDATE



#define PER_LEN	128
PRIVATE VOID tsd_UpdateFirmware(UINT16 startAddr, UINT16 len, UINT8* src)
{
    UINT8 per_len;
    UINT16 sum_len;
    UINT8 cmd[200];

    sum_len = len;
	hal_HstSendEvent(0x58271213);

    do{
        if (len > PER_LEN)
        {
            per_len = PER_LEN;
            len -= PER_LEN;
        }
        else if (len > 0)
        {
            per_len = len;
            len = 0;
        }
        else
        {
            return;
        }
        cmd[0] = 0xF0;
        cmd[1] = 0x96;
        cmd[2] = startAddr/128;
        tsd_I2cWriteBytes(cmd,3);
        hal_TimDelay(30 MS_WAITING);
        tsd_I2cReadBytes(0xF0,cmd,2);

        cmd[0] = 0x00;
        memcpy(cmd+1,src,per_len);
        tsd_I2cWriteBytes(cmd,per_len+1);

        cmd[0] = 0xF0;
        cmd[1] = 0xE1;
        cmd[2] = 0x80|(per_len-1);
        cmd[3] = startAddr;
        cmd[4] = startAddr>>8;
        cmd[5] = 0;
        cmd[6] = 0;
        tsd_I2cWriteBytes(cmd,7);
        hal_TimDelay(30 MS_WAITING);

        tsd_I2cReadBytes(0xF0,cmd,2);

        src += per_len;
        startAddr += per_len;
    }while(len);
	hal_HstSendEvent(0x58278888);
}

PRIVATE UINT16 tsd_ReadChecksum(UINT16 startAddr,UINT16 len)
{
    UINT8 cmd[200];
    cmd[0] = 0xF0;
    cmd[1] = 0xD2;
    cmd[2] = len/128-1;
    cmd[3] = startAddr;
    cmd[4] = startAddr>>8;
    tsd_I2cWriteBytes(cmd,5);

    hal_TimDelay(200 MS_WAITING);
    tsd_I2cReadBytes(0xF0,cmd,4);
    len = cmd[3];
    len = (len<<8)|cmd[2];

	hal_HstSendEvent(0x5827cccc);
	hal_HstSendEvent(len);
	hal_HstSendEvent(0x5827dddd);

    return len;
}

#ifdef CST016_USE_DECRYPT
PRIVATE VOID tsd_SwapValues(UINT8 *values)
{
	UINT8 temp;

	temp = values[0];
	values[0] = values[1];
	values[1] = temp;

	temp = values[2];
	values[2] = values[3];
	values[3] = temp;
}

PRIVATE VOID hynitron_decrypt(UINT16 *v, UINT8 num)
{
    num--;
    while(num--)
	{
        UINT16 y = v[num], z = v[num+1], sum = DELTA*CYCLE_C, i;
        for(i = 0; i < CYCLE_C; i++)
		{
            z -= ((y<<4) + K_C) ^ (y + sum) ^ ((y>>5) + K_D);
            y -= ((z<<4) + K_A) ^ (z + sum) ^ ((z>>5) + K_B);
            sum -= DELTA;
        }
        v[num] = y;
        v[num+1] = z;
    }
}
#endif /* CST016_USE_DECRYPT */

#if defined(__MMI_MAINLCD_240X240__)
#define LCD_WIDTH 240
#define LCD_HEIGHT 240
#elif defined(__MMI_MAINLCD_128X128__)
#define LCD_WIDTH 128
#define LCD_HEIGHT 128
#endif

PRIVATE TSD_ERR_T tsd_GetOnePoint(UINT8 addr, TSD_POINT_T *point)
{
	UINT8 values[4];
	UINT16 tempx = 0;
	UINT16 tempy = 0;

	tsd_I2cReadBytes(addr, values, 4);

#ifdef CST016_USE_DECRYPT
	tsd_SwapValues(values);
	hynitron_decrypt(values, 2);
	tsd_SwapValues(values);
#endif /* CST016_USE_DECRYPT */

	tempx = values[0] & 0x0f;
	tempx = (tempx << 8) | values[1];
	tempy = values[2] & 0x0f;
	tempy = (tempy << 8) | values[3];

	/* Swap X/Y */
	point->x = tempx;
	point->y = tempy;

 if (point->x > LCD_WIDTH)
    {
        point->x = LCD_WIDTH;
    }
    if (point->y > LCD_HEIGHT)
    {
        point->y = LCD_HEIGHT;
    }

	return TSD_ERR_NO;
}

/* 贝特莱IC读取触点坐标 */
PRIVATE TSD_ERR_T tsd_GetAllPoint(UINT8 x_base, TSD_POINT_T *points, UINT8 ponitnum)
{
	UINT8   values[14], i = 0;
	BOOL    ret = 1;

	if(ponitnum == 1)
	{
		tsd_I2cReadBytes(x_base, values, 8);
		points[0].x = (((uint16_t)(values[2] & 0x0f))<< 8) | values[3];
		points[0].y = (((uint16_t)(values[4] & 0x0f))<< 8) | values[5];
		//event->points[0].weight = values[6];
		//event->points[0].area = values[7];
	}
	if(ponitnum == 2)    
	{	
		tsd_I2cReadBytes(x_base, values, 14);
		points[0].x = (((uint16_t)(values[2] & 0x0f))<< 8) | values[3];
		points[0].y = (((uint16_t)(values[4] & 0x0f))<< 8) | values[5];
		//event->points[0].weight = values[6];
		//event->points[0].area = values[7];

		points[1].x = (((uint16_t)(values[8] & 0x0f))<< 8) | values[9];
		points[1].y = (((uint16_t)(values[10] & 0x0f))<< 8) | values[11];
		//event->points[1].weight = values[12];
		//event->points[1].area = values[13];
	}

	SXS_TRACE(TSTDOUT, "tsd_GetAllPoint values[] hex: %x %x %x %x %x %x",
						values[0], values[1],values[2],values[3],values[4],values[5]);

	return TSD_ERR_NO;
}

extern VOID  send_touch_key(void);
PRIVATE VOID tsd_GetData(TSD_POINT_T *point, TSD_PEN_STATE_T *state)
{
	UINT8 lvalue[2] = {0};
	UINT8 pointCount;
	UINT8 i;

	tsd_I2cReadBytes(TD_STAT_ADDR, lvalue, 2);
	pointCount = (lvalue[1] & TD_STAT_NUMBER_TOUCH);

	if (pointCount >= 1 && pointCount <= TD_TOUCH_POINT_COUNT)
	{
		*state = TSD_PEN_DOWN;
//	    for (i = 0; i < pointCount; i++)
//	    {
//	        tsd_GetOnePoint(g_PointValueAddr[i], &g_touchPoints[i]);
//	    }
		tsd_GetAllPoint(TD_STAT_ADDR, g_touchPoints, pointCount);
		point->x = g_touchPoints[0].x;
		point->y = g_touchPoints[0].y;
	}
	else
	{
		*state = TSD_PEN_UP;
		point->x = g_touchPoints[0].x;
		point->y = g_touchPoints[0].y;
		if(point->x == 200 && point->y == 500)
			send_touch_key();
	}
	hal_HstSendEvent(0x58276666);
	hal_HstSendEvent(*state);
	hal_HstSendEvent(point->x);
	hal_HstSendEvent(point->y);
	hal_HstSendEvent(0x58279999);

	SXS_TRACE(TSTDOUT, "tsd_GetData" );
	SXS_TRACE(TSTDOUT, "pointCount: %d", pointCount);
	SXS_TRACE(TSTDOUT, "state: %d", *state);
	SXS_TRACE(TSTDOUT, "point: (%d, %d)", point->x, point->y);
}

PRIVATE VOID tsd_ClearIrq()
{
	hal_HstSendEvent(0x58271100);
	hal_GpioResetIrq(g_tsdConfig->penGpio);
}

PRIVATE VOID tsd_EnableIrq(VOID)
{
	hal_HstSendEvent(0x58271200);
	tsd_ClearIrq();
	g_gpioIrqCfg.irqMask.falling= TRUE;
	hal_GpioIrqSetMask(g_tsdConfig->penGpio, &g_gpioIrqCfg.irqMask);
}

PRIVATE VOID tsd_DisableIrq(VOID)
{
	hal_HstSendEvent(0x58271300);
	g_gpioIrqCfg.irqMask.falling= FALSE;
	hal_GpioIrqSetMask(g_tsdConfig->penGpio, &g_gpioIrqCfg.irqMask);
	tsd_ClearIrq();
}

PRIVATE VOID tsd_ProcessData(TSD_POINT_T *point, TSD_PEN_STATE_T event)
{
	if (g_userCallback) {
		if (event == TSD_PEN_DOWN) {
			if (g_userReportingMode.onDown) {
				hal_HstSendEvent(0x58278800);
				(*g_userCallback)(point, TSD_PEN_DOWN);
			} else if (g_userReportingMode.onPressed) {
				hal_HstSendEvent(0x58278801);
				(*g_userCallback)(point, TSD_PEN_PRESSED);
			}
		} else if (event == TSD_PEN_UP) {
			if (g_userReportingMode.onUp) {
				hal_HstSendEvent(0x58278802);
				(*g_userCallback)(point, TSD_PEN_UP);
			}
		}
	}
}

PRIVATE VOID tsd_Debounce(VOID)
{
	TSD_PEN_STATE_T      event;
    TSD_POINT_T          point;

	TSD_PROFILE_FUNCTION_ENTRY(tsd_Debounce);

	hal_HstSendEvent(0x58274000);
	tsd_GetData(&point, &event);

	tsd_ProcessData(&point, event);

	tsd_ClearIrq();
	tsd_EnableIrq();
	TSD_PROFILE_FUNCTION_EXIT(tsd_Debounce);
}

PRIVATE VOID tsd_IrqHandler(VOID)
{
	hal_HstSendEvent(0x5827aaaa);
    if (g_powerMode == TSD_POWER_SLEEP)
    {
        return;
    }

//	UINT32 irq = hal_GpioGet(g_tsdConfig->penGpio);
//	if (irq == 0 /*1*/) {
//		tsd_DisableIrq();
//		sxr_StopFunctionTimer(tsd_Debounce);
//		sxr_StartFunctionTimer(5 MILLI_SECONDS, tsd_Debounce, (VOID *)NULL, 0x3);
//	}

	tsd_DisableIrq();
	tsd_Debounce();
}

PRIVATE VOID tsd_InitIrq(VOID)
{
	hal_HstSendEvent(0x58278100);
	g_gpioIrqCfg.direction = HAL_GPIO_DIRECTION_INPUT;
	g_gpioIrqCfg.irqMask.rising = FALSE; //TRUE;
	g_gpioIrqCfg.irqMask.debounce = FALSE;
	g_gpioIrqCfg.irqMask.level = FALSE;
	g_gpioIrqCfg.irqMask.falling = TRUE; //FALSE;
	g_gpioIrqCfg.irqHandler = tsd_IrqHandler;
	hal_GpioOpen(g_tsdConfig->penGpio, &g_gpioIrqCfg);
}

PRIVATE VOID tsd_EnterSleepMode()
{
    UINT8 sleepCmd[] = {0xA5, 0x03};

	hal_HstSendEvent(0x58278000);
    tsd_I2cWriteBytes(sleepCmd, sizeof(sleepCmd));
}

PUBLIC VOID tsd_SetReportingMode(TSD_REPORTING_MODE_T* mode)
{
    g_userReportingMode = *mode;

	
	hal_HstSendEvent(0x58270020);
    // don't actually enable IRQ before Open.
    if (g_tsdConfig)
    {
        // depending on wanted events we can disable the IRQ.
        if (g_userReportingMode.onDown
                || g_userReportingMode.onPressed
                || g_userReportingMode.onUp)
        {
			hal_HstSendEvent(0x58270021);
			
			tsd_InitIrq();
            tsd_EnableIrq();
        }
        else
        {
			hal_HstSendEvent(0x58270022);
            // no events enabled, stop pen Irq
            tsd_DisableIrq();
        }
    }
}

PUBLIC VOID tsd_GetReportingMode(TSD_REPORTING_MODE_T* mode)
{
    *mode = g_userReportingMode;
}

PRIVATE UINT8 tsd_EnterBootMode() 
{
    UINT8 retryC = 10;
	UINT8 data[] = {0x00, '0', '1', '2', '3'};
    UINT8 recDat[4];
    while(retryC--){
        hal_TimDelay(2 MS_WAITING);
        hal_HstSendEvent(0x58271000);
        hal_HstSendEvent(data[1]);
        hal_HstSendEvent(data[2]);
        hal_HstSendEvent(data[3]);
        hal_HstSendEvent(data[4]);
        hal_HstSendEvent(0x58271001);
        tsd_I2cWriteBytes(data, sizeof(data));
        hal_TimDelay(1 MS_WAITING);
        tsd_I2cReadBytes(0x00, recDat, sizeof(recDat));
        hal_HstSendEvent(0x58271002);
        hal_HstSendEvent(recDat[0]);
        hal_HstSendEvent(recDat[1]);
        hal_HstSendEvent(recDat[2]);
        hal_HstSendEvent(recDat[3]);
        hal_HstSendEvent(0x58271003);
        if ((recDat[0]!='3') || (recDat[1]!='2') || (recDat[2]!='1') || (recDat[3]!='0')){
            continue;
        } else {
            return 0;
        }
    }
    return -1;
}

VOID tsd_Init(VOID)
{
	int ret = 0;
	
    hal_HstSendEvent(0x58270000);
    g_tsdConfig = tgt_GetTsdConfig();
	g_tsdConfig_bl = tgt_GetTsdConfig();

    if (lcdd_LoadSuccess() == FALSE)
    {
        return;
    }

    // we use vpad, so never mind power
    pmd_EnablePower(PMD_POWER_TP, TRUE);
    hal_TimDelay(5 MS_WAITING);


#ifdef BL_CTP_USE_FIRMWARE_UPDATE

    #if 1
	bl_ts_set_intmode(0);
	bl_ts_set_intup(1);
	ctp_delay_ms(5); //5
	ret = bl_update_fw( );
	ctp_delay_ms(50); //5
	bl_ts_set_intmode(1);
	
	#else
	
	bl_ts_set_intmode(0);
	//erase 
	{
		bl_ts_set_intup(0);
		ctp_delay_ms(5);
		ret = bl_erase_flash();
		ctp_delay_ms(200);

		bl_ts_set_intup(1);
		ctp_delay_ms(50);
	}
	ctp_delay_ms(5);
	bl_ts_set_intmode(1);
	#endif
#endif

}

PUBLIC VOID tsd_Open(TSD_CALLBACK_T callback)
{
	UINT8 ret = 0;

    hal_HstSendEvent(0x58270001);
    g_tsdConfig = tgt_GetTsdConfig();
	g_userCallback = callback;
    g_tsdConfig_bl = tgt_GetTsdConfig();

	pmd_EnablePower(PMD_POWER_TP, TRUE);	


//	// reset
//	tsd_Reset();



	// init irq
	tsd_InitIrq();

	hal_HstSendEvent(0x5827ffff);

	// 这里读取版本号：
	/*
	1. 向reg0x00写入0x02 //工程模式信息
	2. 读reg0x01中为软件版本，1BYTE 可以读出来值为：01,02或者03表示为版本信息
	3. reg 0x30~0x50 存放软件编译时间（ascii码） 	可以不考虑这个步骤
	4. 向reg0x00写入0x00	 //报点模式
	*/
}

PUBLIC VOID tsd_Close(VOID)
{
	// never mind power
	//pmd_EnablePower(PMD_POWER_TP, FALSE);
	tsd_DisableIrq();
	sxr_StopFunctionTimer(tsd_Debounce);
	hal_HstSendEvent(0x58270005);
}

PUBLIC VOID tsd_SetPowerMode(TSD_POWER_MODE_T powerMode)
{
    if (powerMode == g_powerMode)
    {
        return;
    }

    if (powerMode == TSD_POWER_RESUME)
    {
		hal_HstSendEvent(0x58270010);
		
        //tsd_DisableIrq();		// minglong del
        //tsd_Reset();

		bl_ts_int_wakeup();

		MDELAY(3);
		
		{
			uint8_t   retry = 3;
			uint8_t   ret = 1;
			uint8_t   buf[3];	
			uint8_t   tp_id = 0;
			uint8_t   fw_id = 0xff;
			uint8_t   arg_id = 0xff;
			uint8_t   g_pj_id = 0xff;
			uint8_t   bin_fw_id = 0xff;
			uint8_t   bin_arg_id = 0xff;
	#if 0
			//read tp information from Hardware
			buf[1] = 0xff;
			while(retry--)
			{
				ret = bl_get_pjid_fwid_argid(buf);
				if(!ret)
					break;
				MDELAY(30);
			}
			if(!ret)
			{
				fw_id = buf[0];
				arg_id = buf[1];
				g_pj_id = buf[2];
				retry = 3;
				while(retry--)
				{
					ret = bl_get_tp_id(&tp_id);
					if(!ret)
						break;
					MDELAY(30);
				}
			}
	#endif
			bl_log_trace( "###############################################");
			bl_log_trace( "print tp info after update ");
			bl_log_trace( " fw_id : %d",  fw_id);
			bl_log_trace( " arg_id : %d",  arg_id);
			bl_log_trace( " g_pj_id : %d",  g_pj_id);
			bl_log_trace( "###############################################");
		}
        g_powerMode = TSD_POWER_RESUME;
		
    }
    else if (powerMode == TSD_POWER_SLEEP)
    {
		hal_HstSendEvent(0x58270011);
        g_powerMode = TSD_POWER_SLEEP;
//        tsd_DisableIrq();		// minglong del
//        tsd_EnterSleepMode();	// minglong del
		
		bl_ts_set_intmode(0);
		bl_ts_set_intup(1);
		
    }
}

PUBLIC TSD_ERR_T tsd_SetCalibPoints(TSD_CALIBRATION_POINT_T calibPoints[3])
{
	hal_HstSendEvent(0x58270006);
    return TSD_ERR_NO;
}

PUBLIC VOID tsd_SetCalibStatus(BOOL isCalibrated)
{
	hal_HstSendEvent(0x58270007);
	g_isCalibtated = isCalibrated;
}


/*************************** BetterLife functions ***************************/
#ifdef GPIO_EINT

void bl_ts_set_intmode(uint8_t mode)
{
	g_apoRstCfg.type = HAL_GPIO_TYPE_IO;
	g_apoRstCfg.gpioId = g_tsdConfig->penGpio;

	if(0 == mode)
	{
		//GPIO MODE
		tsd_DisableIrq();
		hal_GpioSetOut(g_apoRstCfg.gpioId);
	}
	else if(1 == mode)
	{
		//EINT MODE
		tsd_InitIrq();
//	    if (g_userReportingMode.onDown
//	            || g_userReportingMode.onPressed
//	            || g_userReportingMode.onUp)
//	    {
//	        tsd_EnableIrq();
//	    }

	}
}

void bl_ts_set_intup(uint8_t level)
{
	g_apoRstCfg.type = HAL_GPIO_TYPE_IO;
	g_apoRstCfg.gpioId = g_tsdConfig->penGpio;
	
	hal_GpioSetOut(g_apoRstCfg.gpioId);
	if(level==1)
		hal_GpioSet(g_apoRstCfg);
	else if(level==0)
		hal_GpioClr(g_apoRstCfg);
}
void bl_ts_int_wakeup(void)
{
	bl_ts_set_intmode(0);
	bl_ts_set_intup(1);
	ctp_delay_ms(5);
	bl_ts_set_intup(0);
	ctp_delay_ms(20);
	bl_ts_set_intup(1);
	bl_ts_set_intmode(1);
}



#endif

void ctp_i2c_init(void)
{
	//设置GPIO的工作模式是作为GPIO(0)，还是作为专有功能接口
	gpio_i2c_init_ts();
	
	/////////////20170228//////////////////
#ifdef  NEED_RESET_PIN
	bl_ts_reset_wakeup();
#else
	bl_ts_int_wakeup();
#endif
	/////////////////////////////////
}

PUBLIC CHAR* tsd_GetHwVersion(VOID)
{
    return (g_hwversion[0] != 'U') ?  g_hwversion : "Unknown";
}

/*************************** BetterLife functions end ***************************/


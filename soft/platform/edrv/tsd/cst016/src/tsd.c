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

// I2C
#define I2C_MASTER_ACK              (1<<0)
#define I2C_MASTER_RD               (1<<4)
#define I2C_MASTER_STO              (1<<8)
#define I2C_MASTER_WR               (1<<12)
#define I2C_MASTER_STA              (1<<16)
#define I2C_SLAVE_ADDR              0x15

// TP
#define TD_STAT_ADDR 0x02
#define TD_STAT_NUMBER_TOUCH 0x07
#define TD_TOUCH_POINT_COUNT 0x04
#define TOUCH1_XH_ADDR  0x03
#define TOUCH2_XH_ADDR  0x09
#define TOUCH3_XH_ADDR  0x0F
#define TOUCH4_XH_ADDR  0x15

// Dec
#define DELTA   (UINT32)0x9e3779b9
#define CYCLE_C 16
#define K_A     (UINT32)0x12345678
#define K_B     (UINT32)0x87654321
#define K_C     (UINT32)0x13579086
#define K_D     (UINT32)0x24689753

#ifdef JKT_WATCH_GLOBAL_FLAG
extern PUBLIC VOID hal_ConfigKeyin3ToGpio(VOID);
#endif /* JKT_WATCH_GLOBAL_FLAG */

PRIVATE CONST TSD_CONFIG_T*         g_tsdConfig;
PRIVATE TSD_CALLBACK_T              g_userCallback;
PRIVATE TSD_REPORTING_MODE_T        g_userReportingMode;
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

PRIVATE VOID tsd_Reset(VOID)
{
    g_apoRstCfg.type = HAL_GPIO_TYPE_IO;
    g_apoRstCfg.gpioId = g_tsdConfig->pinReset;
    hal_GpioSetOut(g_apoRstCfg.gpioId);

    hal_GpioSet(g_apoRstCfg);
    hal_TimDelay(2 MS_WAITING);
    hal_GpioClr(g_apoRstCfg);
    hal_TimDelay(2 MS_WAITING);
    hal_GpioSet(g_apoRstCfg);
}

#if 0
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
#define CTP_USE_FIRMWARE_UPDATE

#ifdef CTP_USE_FIRMWARE_UPDATE
#if defined(__MMI_MAINLCD_240X240__)
#include "firmware_240_240.h"
#elif defined(__MMI_MAINLCD_128X128__)
#include "firmware_128_128.h"
#else
#error "CTP_USE_FIRMWARE_UPDATE defined but firmware_xx_xx.h not found"
#endif /* __MMI_MAINLCD_zzXzz__ */

#define CTP_UPDATE_TIMEOUT (30 * HAL_TICK1S)

PRIVATE BOOL tsd_update_erase_block(UINT16 startAddr)
{
    UINT8 cmd[3];

	cmd[0] = 0xF0;
    cmd[1] = 0x96;
    cmd[2] = startAddr / 128;
    tsd_I2cWriteBytes(cmd, 3);
    hal_TimDelay(30 MS_WAITING);
    tsd_I2cReadBytes(0xF0, cmd, 2);

    return (cmd[0] == 0x00);
}

PRIVATE BOOL tsd_update_send_data(UINT8 *data, UINT16 len)
{
    tsd_I2cWriteBytes(data, len);
    return TRUE;
}

PRIVATE BOOL tsd_update_burn_data(UINT16 startAddr, UINT16 len)
{
	UINT8 cmd[7];

    cmd[0] = 0xF0;
    cmd[1] = 0xE1;
    cmd[2] = 0x80 | (len - 1);
    cmd[3] = startAddr;
    cmd[4] = startAddr >> 8;
    cmd[5] = 0;
    cmd[6] = 0;
    tsd_I2cWriteBytes(cmd, 7);
    hal_TimDelay(30 MS_WAITING);
    tsd_I2cReadBytes(0xF0, cmd, 2);

    return (cmd[0] == 0x00);
}

PRIVATE BOOL tsd_update_check_data(UINT16 startAddr, UINT8 *srcData, UINT16 len)
{
    UINT8 cmd[160];

    cmd[0] = 0xF0;
    cmd[1] = 0xF0;
    cmd[2] = 0x80 | (len - 1);
    cmd[3] = startAddr;
    cmd[4] = startAddr>>8;
    cmd[5] = 0;
    cmd[6] = 0;
    tsd_I2cWriteBytes(cmd, 7);

    tsd_I2cReadBytes(0xF0, cmd, len + 16);
    UINT8 *rdData = (cmd + 16);

    for (int i = 0; i < len; i++) {
        if (srcData[i] != rdData[i]) {
			hal_HstSendEvent(0x5827e005);
            return FALSE;
        }
    }

	return TRUE;
}

#define PER_LEN 128
PRIVATE BOOL tsd_UpdateFirmware(UINT16 startAddr, UINT16 len, UINT8* src)
{
    UINT8 per_len;
    UINT8 data[256];
    UINT32 second_time,first_time;
	BOOL update_timeout = FALSE;

    hal_HstSendEvent(0x58273333);
    first_time = hal_TimGetUpTime();

    do
    {
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
            return TRUE;
        }

        data[0] = 0x00;
        memcpy(data+1,src,per_len);

        tsd_update_erase_block(startAddr);
		do {
			tsd_update_send_data(data, per_len + 1);
			tsd_update_burn_data(startAddr, per_len);
			second_time = hal_TimGetUpTime();
			if (second_time - first_time > CTP_UPDATE_TIMEOUT) {
				update_timeout = TRUE;
				break;
			}
		} while(!tsd_update_check_data(startAddr, data + 1, per_len));

		if (update_timeout) {
			hal_HstSendEvent(0x5827e006);
	        break;
		}

        src += per_len;
        startAddr += per_len;
    }
    while(len);
    hal_HstSendEvent(0x58274444);
	return (!update_timeout);
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

    hal_HstSendEvent(0x58271111);
    hal_HstSendEvent(len);

    return len;
}
#endif /* CTP_USE_FIRMWARE_UPDATE */

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
    point->x = tempy;
    point->y = tempx;

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

PRIVATE VOID tsd_GetData(TSD_POINT_T *point, TSD_PEN_STATE_T *state)
{
    UINT8 pointCount;
    UINT8 i;

    tsd_I2cReadBytes(TD_STAT_ADDR, &pointCount, 1);
    pointCount = (pointCount & TD_STAT_NUMBER_TOUCH);

    if (pointCount >= 1 && pointCount <= TD_TOUCH_POINT_COUNT)
    {
        *state = TSD_PEN_DOWN;
        for (i = 0; i < pointCount; i++)
        {
            tsd_GetOnePoint(g_PointValueAddr[i], &g_touchPoints[i]);
        }
        point->x = g_touchPoints[0].x;
        point->y = g_touchPoints[0].y;
    }
    else
    {
        *state = TSD_PEN_UP;
        point->x = g_touchPoints[0].x;
        point->y = g_touchPoints[0].y;
    }
}

PRIVATE VOID tsd_ClearIrq()
{
    hal_GpioResetIrq(g_tsdConfig->penGpio);
}

PRIVATE VOID tsd_EnableIrq(VOID)
{
    tsd_ClearIrq();
    g_gpioIrqCfg.irqMask.rising = TRUE;
    hal_GpioIrqSetMask(g_tsdConfig->penGpio, &g_gpioIrqCfg.irqMask);
}

PRIVATE VOID tsd_DisableIrq(VOID)
{
    g_gpioIrqCfg.irqMask.rising = FALSE;
    hal_GpioIrqSetMask(g_tsdConfig->penGpio, &g_gpioIrqCfg.irqMask);
    tsd_ClearIrq();
}

PRIVATE VOID tsd_ProcessData(TSD_POINT_T *point, TSD_PEN_STATE_T event)
{
    if (g_userCallback)
    {
        if (event == TSD_PEN_DOWN)
        {
            if (g_userReportingMode.onDown)
            {
                (*g_userCallback)(point, TSD_PEN_DOWN);
            }
            else if (g_userReportingMode.onPressed)
            {
                (*g_userCallback)(point, TSD_PEN_PRESSED);
            }
        }
        else if (event == TSD_PEN_UP)
        {
            if (g_userReportingMode.onUp)
            {
                (*g_userCallback)(point, TSD_PEN_UP);
            }
        }
    }
}

PRIVATE VOID tsd_Debounce(VOID)
{
    TSD_PEN_STATE_T      event;
    TSD_POINT_T          point;

    tsd_GetData(&point, &event);
    tsd_ProcessData(&point, event);
    tsd_ClearIrq();
    tsd_EnableIrq();
}

PRIVATE VOID tsd_IrqHandler(VOID)
{
    if (g_powerMode == TSD_POWER_SLEEP)
    {
        return;
    }

    UINT32 irq = hal_GpioGet(g_tsdConfig->penGpio);
    if (irq == 1)
    {
        tsd_DisableIrq();
        sxr_StopFunctionTimer(tsd_Debounce);
        sxr_StartFunctionTimer(5 MILLI_SECONDS, tsd_Debounce, (VOID *)NULL, 0x3);
    }
}

PRIVATE VOID tsd_InitIrq(VOID)
{
#ifdef JKT_WATCH_GLOBAL_FLAG
    hal_ConfigKeyin3ToGpio();
#endif /* JKT_WATCH_GLOBAL_FLAG */
    g_gpioIrqCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    g_gpioIrqCfg.irqMask.rising = TRUE;
    g_gpioIrqCfg.irqMask.debounce = FALSE;
    g_gpioIrqCfg.irqMask.level = FALSE;
    g_gpioIrqCfg.irqMask.falling = FALSE;
    g_gpioIrqCfg.irqHandler = tsd_IrqHandler;
    hal_GpioOpen(g_tsdConfig->penGpio, &g_gpioIrqCfg);
}

PRIVATE VOID tsd_EnterSleepMode()
{
    UINT8 sleepCmd[] = {0xA5, 0x03};
    tsd_I2cWriteBytes(sleepCmd, sizeof(sleepCmd));
}

PUBLIC VOID tsd_SetReportingMode(TSD_REPORTING_MODE_T* mode)
{
    g_userReportingMode = *mode;

    // don't actually enable IRQ before Open.
    if (g_tsdConfig)
    {
        // depending on wanted events we can disable the IRQ.
        if (g_userReportingMode.onDown
                || g_userReportingMode.onPressed
                || g_userReportingMode.onUp)
        {
            tsd_EnableIrq();
        }
        else
        {
            // no events enabled, stop pen Irq
            tsd_DisableIrq();
        }
    }
}

PUBLIC VOID tsd_GetReportingMode(TSD_REPORTING_MODE_T* mode)
{
    *mode = g_userReportingMode;
}

PRIVATE VOID tsd_GetTsdVersion(VOID)
{
    UINT8 retryC = 5;
	UINT8 rDat[5] = {0,};

    hal_HstSendEvent(0x58275656);
    while(retryC--)
    {
        hal_TimDelay(2 MS_WAITING);
        tsd_I2cReadBytes(0xE0, rDat, 5);

        if (rDat[0] != 0xFF) {
            memcpy(g_hwversion, rDat, sizeof(rDat));
            g_hwversion[sizeof(rDat)] = '\0';
            break;
        }

        hal_HstSendEvent(rDat[0]);
        hal_HstSendEvent(rDat[1]);
        hal_HstSendEvent(rDat[2]);
        hal_HstSendEvent(rDat[3]);
        hal_HstSendEvent(rDat[4]);
    }
    hal_HstSendEvent(0x58278989);
}

PRIVATE UINT8 tsd_EnterBootMode()
{
    UINT8 retryC = 10;
    UINT8 data[] = {0x00, '0', '1', '2', '3'};
    UINT8 recDat[4] = {0,};

    hal_TimDelay(10 MS_WAITING);
    while(retryC--)
    {
        tsd_I2cWriteBytes(data, sizeof(data));
        hal_TimDelay(10 MS_WAITING);
        tsd_I2cReadBytes(0x00, recDat, sizeof(recDat));
        if ((recDat[0]=='3') && (recDat[1]=='2') && (recDat[2]=='1') && (recDat[3]=='0'))
        {
            tsd_GetTsdVersion();
            return 0;
        }
    }
    return -1;
}

BOOL tsd_Init(VOID)
{
    hal_HstSendEvent(0x58270000);
    g_tsdConfig = tgt_GetTsdConfig();

    if (lcdd_LoadSuccess() == FALSE)
    {
        return TRUE;
    }

    // we use vpad, so never mind power
    //pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
    //hal_TimDelay(5 MS_WAITING);

#ifdef CTP_USE_FIRMWARE_UPDATE
    {
        // check update ?
        if (sizeof(app_bin) > 10)
        {
            UINT16 startAddr = app_bin[1];
            UINT16 len = app_bin[3];
            UINT16 fileCheck = app_bin[5];
            startAddr = ((startAddr<<8)|app_bin[0]) - 0x1000;
            len = (len<<8)|app_bin[2];
            fileCheck = (fileCheck<<8)|app_bin[4];

            uint32_t sc = hal_SysEnterCriticalSection();
            tsd_Reset();
            if (tsd_EnterBootMode() != 0)
            {
                hal_HstSendEvent(0x5827e001);
            }
            else if (fileCheck != tsd_ReadChecksum(startAddr,len))
            {
                if (!tsd_UpdateFirmware(startAddr,len,(UINT8 *)(app_bin+6))) {
					return FALSE;
				}

                tsd_Reset();
                if (tsd_EnterBootMode() != 0)
                {
                    hal_HstSendEvent(0x5827e002);
                }
                else if (fileCheck != tsd_ReadChecksum(startAddr,len))
                {
                    hal_HstSendEvent(0x5827e003);
                }
            }
            hal_HstSendEvent(0x5827cccc);
            hal_SysExitCriticalSection(sc);
        }
    }
	return TRUE;
#endif /* CTP_USE_FIRMWARE_UPDATE */
}

PUBLIC VOID tsd_Open(TSD_CALLBACK_T callback)
{
    hal_HstSendEvent(0x58270001);
    g_tsdConfig = tgt_GetTsdConfig();
    g_userCallback = callback;

    // we use vpad, so never mind power
    //pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
    //hal_TimDelay(5 MS_WAITING);

    // reset
    tsd_Reset();

    // init irq
    tsd_InitIrq();
    hal_HstSendEvent(0x5827ffff);

    // 这里读取版本号：
    /*
    1. 向reg0x00写入0x02 //工程模式信息
    2. 读reg0x01中为软件版本，1BYTE 可以读出来值为：01,02或者03表示为版本信息
    3. reg 0x30~0x50 存放软件编译时间（ascii码）     可以不考虑这个步骤
    4. 向reg0x00写入0x00  //报点模式
    */
}

PUBLIC VOID tsd_Close(VOID)
{
    // never mind power
    //pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
    tsd_DisableIrq();
    sxr_StopFunctionTimer(tsd_Debounce);
}

PUBLIC VOID tsd_SetPowerMode(TSD_POWER_MODE_T powerMode)
{
    if (powerMode == g_powerMode)
    {
        return;
    }

    if (powerMode == TSD_POWER_RESUME)
    {
        tsd_DisableIrq();
        tsd_Reset();
        g_powerMode = TSD_POWER_RESUME;
    }
    else if (powerMode == TSD_POWER_SLEEP)
    {
        g_powerMode = TSD_POWER_SLEEP;
        tsd_DisableIrq();
        tsd_EnterSleepMode();
    }
}

PUBLIC TSD_ERR_T tsd_SetCalibPoints(TSD_CALIBRATION_POINT_T calibPoints[3])
{
    return TSD_ERR_NO;
}

PUBLIC VOID tsd_SetCalibStatus(BOOL isCalibrated)
{
    g_isCalibtated = isCalibrated;
}

PUBLIC CHAR* tsd_GetHwVersion(VOID)
{
    return (g_hwversion[0] != 'U') ?  g_hwversion : "Unknown";
}

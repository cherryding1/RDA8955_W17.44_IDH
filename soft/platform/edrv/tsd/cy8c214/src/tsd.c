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



#include "sxr_ops.h"
#include "sxs_io.h"
#include "cs_types.h"
#include "sxr_tls.h"
#include "tsdp_debug.h"
#include "tsd_config.h"
#include "tsd_m.h"
#include "tgt_tsd_cfg.h"
#include "tsdp_calib.h"
#include "pmd_m.h"

#include "hal_host.h"
#include "i2c.h"
// marco
#define SECOND        * HAL_TICK1S
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR
#define FS_TRACE_TIMER_CNT (1 SECOND)
PRIVATE CONST TSD_CONFIG_T*  g_tsdConfig;
PRIVATE HAL_GPIO_CFG_T       g_gpioCfg;

// ============================================================================
// LOCAL DEFINITIONS
// ============================================================================

// Number of samples we ask the touch screen controller for each coordinate.
// We need more than one because of the touch screen settling time.
// Indeed, the voltage across the touch panel "ring" and then settle down due
// to mechanical bouncing and switch driver parasiting.
// From 1 to 4 : 4 is obviously the more precise.
#define NUMBER_SAMPLES_PER_POINT 4


// With 12-bit Conversion, keeping the tenth most significant bits.
#define MAX_ADC_VAL    0x3ff

// Flag to indicate if the tsd_GetRawPoints succeeded or not.
#define SUCCESSFUL   1
#define NOT_SUCCESSFUL   0
#define SAMPLE_VALID 3




PUBLIC VOID tsd_Open(TSD_CALLBACK_T callback)
{}


PUBLIC VOID tsd_SetPowerMode(TSD_POWER_MODE_T mode)
{}

PRIVATE VOID tsd_Delay(UINT16 Wait_mS)
{
    UINT32 Ticks_16K;

    // Here is the delay function used by the camera driver.  It can be changed to use a non-blocking
    // wait if necessary, but for now, just use hal_TimDelay
    Ticks_16K = (16348 * Wait_mS)/1000;
    hal_TimDelay(Ticks_16K);
}




VOID  tsd_WriteOneReg( UINT8 Addr, UINT8 Data)
{
    if( tsd_cap_gpio_i2c_write_data_sensor(0x0a,&Addr,1,&Data,1) == FALSE)
    {
        hal_DbgAssert("no ack");
    }


}

VOID  tsd_WriteOneReg_bak( UINT8 Addr, UINT8 Data)
{
    if( tsd_cap_gpio_i2c_write_data_sensor(0x0a,&Addr,1,&Data,1) == FALSE)
    {
        if( tsd_cap_gpio_i2c_write_data_sensor(0x0a,&Addr,1,&Data,1) == FALSE)
        {
            if( tsd_cap_gpio_i2c_write_data_sensor(0x0a,&Addr,1,&Data,1) == FALSE)
            {

                hal_DbgAssert("no ack");
            }

        }

    }

}

UINT8 tsd_cap_ReadReg(UINT8 Addr)
{
    UINT8 data = 0;

    tsd_cap_gpio_i2c_read_data_sensor(0x0a,&Addr,1,&data,1);
    return data;

}


UINT8 cap_data[16];
VOID tsd_ReadReg_Continue()
{

    tsd_cap_gpio_i2c_read_data_sensor_continue(0x0a,cap_data,6);


}

PRIVATE VOID tsd_cap_DisableHostPenIrq(VOID)
{

    g_gpioCfg.irqMask.falling = FALSE;
    hal_GpioIrqSetMask(HAL_GPIO_5, &g_gpioCfg.irqMask);
    hal_GpioResetIrq(HAL_GPIO_5);

}


PRIVATE VOID tsd_cap_EnableHostPenIrq(VOID)
{

    hal_GpioResetIrq(HAL_GPIO_5);

    g_gpioCfg.irqMask.falling = TRUE;

    hal_GpioIrqSetMask(HAL_GPIO_5, &g_gpioCfg.irqMask);

}

extern VOID reftest_tsdCallback(VOID);

PRIVATE VOID tsd_cap_Debounce(VOID)
{

    reftest_tsdCallback();


    tsd_cap_EnableHostPenIrq();

}

PRIVATE VOID tsd_cap_GpioIrqHandler(VOID)
{

    if ( hal_GpioGet(HAL_GPIO_5) == 0 )
    {

        tsd_cap_DisableHostPenIrq();
        sxr_StartFunctionTimer(2 MILLI_SECONDS,tsd_cap_Debounce,(VOID*)NULL,0x03);
    }
}

PRIVATE VOID tsd_cap_Init_Gpio(VOID)
{
    g_gpioCfg.direction = HAL_GPIO_DIRECTION_INPUT;

    // GPIO irq mask cfg
    g_gpioCfg.irqMask.rising = FALSE;

    g_gpioCfg.irqMask.debounce = TRUE;

    g_gpioCfg.irqHandler = tsd_cap_GpioIrqHandler;


    g_gpioCfg.irqMask.level = FALSE;


    g_gpioCfg.irqMask.falling = TRUE;



    hal_GpioOpen(HAL_GPIO_5,&g_gpioCfg);

}
VOID test_cap_tourch()
{
    pmd_EnablePower(PMD_POWER_USB,TRUE);
    tsd_cap_Init_Gpio();
    tsd_cap_gpio_i2c_init_sensor();

    tsd_Delay(2000);
    hal_HstSendEvent(0x11223344);

    tsd_WriteOneReg(0x08,0x11); tsd_WriteOneReg(0x07,0x11);  // enter into multi touch mode

}

#define CAP_MULTI_TOUCH_BYTE1 0x0a
#define CAP_MULTI_TOUCH_BYTE2 0x03
#define CAP_MULTI_TOUCH_BYTE3 0x38

#define CAP_GESTURE 3
#define CAP_VALUE 4

#define CAP_TP_ZOOM 0x01
#define CAP_TP_ZOOM_X 0x02
#define CAP_TP_ZOOM_Y 0x03
#define CAP_TP_ROTATE 0x04
#define CAP_TP_SLIDE_X 0x05
#define CAP_TP_SLIDE_Y 0x06


#define GESTURE_ZOOM_IN 1
#define GESTURE_ZOOM_OUT 2
#define GESTURE_ZOOM_X_IN 3
#define GESTURE_ZOOM_X_OUT 4
#define GESTURE_ZOOM_Y_IN 5
#define GESTURE_ZOOM_Y_OUT 6
#define GESTURE_ROTATE_CCW 7
#define GESTURE_ROTATE_CW 8
#define GESTURE_SLIDE_LEFT 9
#define GESTURE_ROTATE_RIGHT 10

UINT16 g_gesture_status = 0;
UINT16 g_gesture_status_x = 0;
UINT16 g_gesture_status_y = 0;
UINT16 g_gesture_status_p = 0;

VOID test_cap_parse()
{
    if((cap_data[0]==CAP_MULTI_TOUCH_BYTE1)&&(cap_data[1]==CAP_MULTI_TOUCH_BYTE2)&&\
            (cap_data[2]==CAP_MULTI_TOUCH_BYTE3))
    {
        switch(cap_data[CAP_GESTURE])
        {
            case CAP_TP_ZOOM:
                if(cap_data[CAP_VALUE]==0xff)
                {
                    g_gesture_status = GESTURE_ZOOM_OUT;
                }
                else
                {
                    g_gesture_status = GESTURE_ZOOM_IN;
                }
                break;
            case CAP_TP_ZOOM_X:
                if(cap_data[CAP_VALUE]==0xff)
                {
                    g_gesture_status = GESTURE_ZOOM_X_OUT;
                }
                else
                {
                    g_gesture_status = GESTURE_ZOOM_X_IN;
                }
                break;
            case CAP_TP_ZOOM_Y:
                if(cap_data[CAP_VALUE]==0xff)
                {
                    g_gesture_status = GESTURE_ZOOM_Y_OUT;
                }
                else
                {
                    g_gesture_status = GESTURE_ZOOM_Y_IN;
                }
                break;
            case CAP_TP_ROTATE:
                if(cap_data[CAP_VALUE]==0xff)
                {
                    g_gesture_status = GESTURE_ROTATE_CW;
                }
                else
                {
                    g_gesture_status = GESTURE_ROTATE_CCW;
                }
                break;
            case CAP_TP_SLIDE_X:
                if(cap_data[CAP_VALUE]==0xff)
                {
                    g_gesture_status = GESTURE_ROTATE_RIGHT;
                }
                else
                {
                    g_gesture_status = GESTURE_SLIDE_LEFT;
                }
                break;
            default:
                g_gesture_status = 0;
                break;


        }
        // hal_HstSendEvent(0xee771004); hal_HstSendEvent(g_gesture_status);

    }
    else if(cap_data[0]==0x81)
    {
        g_gesture_status_x = (cap_data[2]&0x7f)|((cap_data[1]&0xf)<<7);
        g_gesture_status_y = (cap_data[4]&0x7f)|((cap_data[3]&0xf)<<7);
        g_gesture_status_p = cap_data[5]&0x7f;
        hal_HstSendEvent(0xaa771004);
        g_gesture_status = 0;

    }
    else
    {
        hal_HstSendEvent(0xcc771004); g_gesture_status = 0;
    }
}

VOID test_cap_read_tourch()
{
    UINT8 kk = 0;
    tsd_ReadReg_Continue();
    hal_HstSendEvent(0x88771004);
    //for (kk = 0;kk<6;kk++)
    hal_HstSendEvent(cap_data[0]);
    test_cap_parse();

}

UINT16 test_get_cap_gesture()
{
    return g_gesture_status;
}

PUBLIC CHAR* tsd_GetHwVersion(VOID)
{
    return "Unknown";
}

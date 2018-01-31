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
#include "mxc6225xu.h"
#include "hal_i2c.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "cmn_defs.h"


// =============================================================================
//  MACROS
// =============================================================================
#define SECOND        * HAL_TICK1S   //  HAL_TICK1S = 16384

#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR


//#define GSENSOR_CHIP_ADRESS   (0x2a>>1)                //0x11//0010001
//#define MEMSIC_ACC_SLAVE_ADDR      (0x2a>>1)
#define GSENSOR_CHIP_ADRESS   (0x15)                //0x11//0010001
#define MEMSIC_ACC_SLAVE_ADDR        (0x15)



#define gMaxValue 31
#define gMinValue -32

#define Shake_HighTh 28
#define Shake_LowTh -29

#define ShakeDelay_TimeOut 30 //40
#define ShakeDelay_Interval 15
#define SHAKE_INTERVAL 30 //40
#define Shake_detect 25


GSS_SAMPLE_DATA_T g_sample_data = {0};
UINT8 g_threshold_orientation = 7;
PRIVATE GSS_CALLBACK_T          g_GssCallback = NULL;

#define shake_threshold = 8;
/// This array contain the value of the corresponding mma7660 all registers

GSS_ACC_DATE_T g_acc_data =
{
    .x_adc = 0,
    .y_adc = 0,
    .status = 0,

};
// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================
/// This array contain the value of the corresponding INT sur registers

PUBLIC void gsensorhal_delay(UINT32 tktnum)
{
#if 1
    UINT32 wait_time = 0;

    wait_time = hal_TimGetUpTime();
    if((hal_TimGetUpTime() - wait_time) > tktnum)
    {
        return;
    }
#else
    sxr_Sleep(tktnum);
#endif
}





CONST TGT_GSENSOR_CONFIG_T*  g_gsensorConfig;
HAL_GPIO_CFG_T       g_gsensor_gpioCfg;
PRIVATE UINT32 g_sample_time = 100 MILLI_SECONDS;
PRIVATE BOOL g_start_monitor = FALSE;
VOID gsensor_monitor(VOID);
//extern BOOL pm_GsensorReport(UINT32 param1,UINT32 param2,UINT32 param3);
GSENSOR_ERR_T gsensor_Filt_LP(UINT8 count);

// ============================================================================
//  Gss_SetCallback
// ----------------------------------------------------------------------------
/// This function configures which user function will be called by the
/// gsensor driver when the callback is invoked.
// ============================================================================
PUBLIC VOID gsensor_SetCallback(GSS_CALLBACK_T callback)
{
    g_GssCallback = callback;
}

// ============================================================================
//  tsd_GpioIrqHandler
// ----------------------------------------------------------------------------
// This function handles GPIO hard interruptions.
// ============================================================================
PRIVATE VOID gsensor_GpioIrqHandler(VOID)
{

#if 0
    sxs_fprintf(TSTDOUT, "gss:  gsensor_GpioIrqHandler  ");

    sxr_StartFunctionTimer(g_sample_time,gsensor_monitor,(VOID*)NULL,0x03);

    // We check the value of the penIrqPin to avoid false triggering when we reset
    // the touch screen controler.

    if ( hal_GpioGet(g_tsdConfig->penGpio) == 1 )
    {
        // We disable host Pen IRQ while the screen is pressed.
        tsd_DisableHostPenIrq();
        sxr_StartFunctionTimer(g_tsdConfig->debounceTime,tsd_Debounce,(VOID*)NULL,0x03);
    }

#endif
}

VOID gsensor_I2cOpen(VOID)
{


}
VOID gsensor_I2cClose(VOID)
{

}

GSENSOR_ERR_T gsensor_ReadSingle(UINT8 addr, UINT8* data)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    hal_I2cOpen(HAL_I2C_BUS_ID_2);
    sensor_err =  hal_I2cGetByte(HAL_I2C_BUS_ID_2,GSENSOR_CHIP_ADRESS,addr,data);
    hal_I2cClose(HAL_I2C_BUS_ID_2);
    if (sensor_err != HAL_ERR_NO)
    {
        sxs_fprintf(TSTDOUT, "gss: single read sensor_err %d ",sensor_err);
    }
    return GSENSOR_ERR_NO;
}


GSENSOR_ERR_T gsensor_WriteSingle(UINT8 addr, UINT8 data)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;

    hal_I2cOpen(HAL_I2C_BUS_ID_2);
    sensor_err =  hal_I2cSendByte(HAL_I2C_BUS_ID_2,GSENSOR_CHIP_ADRESS,addr,data);
    hal_I2cClose(HAL_I2C_BUS_ID_2);

    if (sensor_err != HAL_ERR_NO)
    {
        sxs_fprintf(TSTDOUT, "gss: single write I2C busy ! sensor_err = %d",sensor_err); return GSENSOR_ERR_NO;
    }
    sxs_fprintf(TSTDOUT, "gss: single addr =%d   write data 0x%x  ! ",addr,data);

    return GSENSOR_ERR_NO;
}


GSENSOR_ERR_T gsensor_Write(UINT8 addr, UINT8* data,UINT8 length)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    hal_I2cOpen(HAL_I2C_BUS_ID_2);
    sensor_err =  hal_I2cSendData(HAL_I2C_BUS_ID_2,GSENSOR_CHIP_ADRESS,addr,data,length);
    hal_I2cClose(HAL_I2C_BUS_ID_2);
    if (sensor_err != HAL_ERR_NO)
    {
        sxs_fprintf(TSTDOUT, "gss: write I2C busy ! ");
        return GSENSOR_ERR_RESOURCE_BUSY;
    }
    else
    {
        return GSENSOR_ERR_NO;
    }
}


GSENSOR_ERR_T gsensor_Read(UINT8 addr, UINT8* data,UINT8 length)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    hal_I2cOpen(HAL_I2C_BUS_ID_2);
    sensor_err = hal_I2cGetData(HAL_I2C_BUS_ID_2, GSENSOR_CHIP_ADRESS,addr,data,length);
    hal_I2cClose(HAL_I2C_BUS_ID_2);
    if (sensor_err != HAL_ERR_NO)
    {
        sxs_fprintf(TSTDOUT, "gss: read  I2C busy ! sensor_err = %d ",sensor_err);
        return GSENSOR_ERR_RESOURCE_BUSY;
    }
    else
    {
        //sxs_fprintf(TSTDOUT, "gss: read data ok");
        return GSENSOR_ERR_NO;
    }
}

GSENSOR_ERR_T gsensor_enter_active_mode(VOID)
{
    gsensor_WriteSingle(mxc6225xu_adder_detection,0);
    return GSENSOR_ERR_NO;
}

GSENSOR_ERR_T gsensor_enter_standby_mode(VOID)
{
    gsensor_WriteSingle(mxc6225xu_adder_detection,0xff);
    return GSENSOR_ERR_NO;
}

UINT8 g_acc_status = 0;


// status
#define mxc6225xu_INTOR0             (1<<0)
#define mxc6225xu_INTOR1             (1<<1)
#define mxc6225xu_INTOR             (3<<0)

#define mxc6225xu_INTORI0             (1<<2)
#define mxc6225xu_INTORI1             (1<<3)
#define mxc6225xu_INTORI             (3<<2)

#define mxc6225xu_TILT             (1<<4)

#define mxc6225xu_INTSH0             (1<<5)
#define mxc6225xu_INTSH1             (1<<6)
#define mxc6225xu_INTSH             (3<<5)
VOID gsensor_monitor(VOID)
{
    if (GSENSOR_ERR_NO !=gsensor_Read(0,(UINT8 *)&g_acc_data,3))
    {
        sxr_StartFunctionTimer(g_sample_time,gsensor_monitor,(VOID*)NULL,0x03);
        sxs_fprintf(TSTDOUT, "gss:  gsensor monitor alert ");
        return;
    }


    sxs_fprintf(TSTDOUT, "gss:  hi  x = %d  y =%d  status =0x%x ",g_acc_data.x_adc,g_acc_data.y_adc,g_acc_data.status);
    g_sample_data.x = g_acc_data.x_adc;   g_sample_data.y = g_acc_data.y_adc;

    g_acc_status = (g_acc_data.status & mxc6225xu_INTSH)>>5;

    if (g_acc_status == 0x00)
    {
        g_sample_data.shake = GS_SHAKE_NO_HAPPEN; // doing something
        // sxs_fprintf(TSTDOUT, "gss: NO_HAPPEN  0x%x ",g_acc_status);
    }
    else if (g_acc_status == 0x01)
    {
        g_sample_data.shake = GS_MOTION_SHAKE_X_L_AXIS; // doing something
        sxs_fprintf(TSTDOUT, "gss: -----shake----- SHAKE_HAPPEN_LLLLLLL  0x%x ",g_acc_status);
    }
    else if (g_acc_status == 0x02)
    {
        g_sample_data.shake = GS_MOTION_SHAKE_X_R_AXIS; // doing something
        sxs_fprintf(TSTDOUT, "gss: -----shake----- SHAKE_HAPPEN_RRRRRRRR  0x%x ",g_acc_status);
    }
    else if (g_acc_status == 0x03)
    {
        g_sample_data.shake = GS_SHAKE_NO_HAPPEN; // doing something
        //sxs_fprintf(TSTDOUT, "gss: SHAKE_HAPPEN_ERROR  0x%x ",g_acc_status);
    }

    g_acc_status = (g_acc_data.status & mxc6225xu_INTOR);
    // rotate
    if ((g_acc_status & mxc6225xu_INTOR) == 0x00)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_LEFT;
        sxs_fprintf(TSTDOUT, "gss: ROTATE_LEFT  0x%x ",g_acc_status);
    }
    else if((g_acc_status & mxc6225xu_INTOR) == 0x01)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_RIGHT;
        sxs_fprintf(TSTDOUT, "gss: ROTATE_RIGHT  0x%x ",g_acc_status);
    }
    else if ((g_acc_status & mxc6225xu_INTOR) == 0x02)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_DOWN;
        sxs_fprintf(TSTDOUT, "gss: ROTATE_DOWN  0x%x ",g_acc_status);
    }
    else if ((g_acc_status & mxc6225xu_INTOR) == 0x03)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_UP;
        sxs_fprintf(TSTDOUT, "gss: ROTATE_UP  0x%x ",g_acc_status);
    }
    else
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_UNKNOW;
        // sxs_fprintf(TSTDOUT, "gss: ROTATE_UNKNOW  0x%x ",g_acc_status);

    }

#if 0
    //turn over
    if ((g_acc_data.tilt & MMA7660FC_BA_FRO_MASK) == MMA7660FC_BA_FRO_FRONT)
    {
        g_sample_data.tunover = GS_MOTION_TURNOVER_FRONT;
        //sxs_fprintf(TSTDOUT, "gss: GS_MOTION_TURNOVER_FRONT  0x%x ",g_sample_data.tunover);
    }
    else if ((g_acc_data.tilt & MMA7660FC_BA_FRO_MASK) == MMA7660FC_BA_FRO_BACK)
    {
        g_sample_data.tunover = GS_MOTION_TURNOVER_BACK;
        //sxs_fprintf(TSTDOUT, "gss: GS_MOTION_TURNOVER_BACK  0x%x ",g_sample_data.tunover);
    }
    else
    {
        g_sample_data.tunover = GS_MOTION_TURNOVER_UNKNOW;
        // sxs_fprintf(TSTDOUT, "gss: GS_MOTION_TURNOVER_UNKNOW  0x%x ",g_sample_data.tunover);
    }
#endif


    if (g_GssCallback != NULL)
    {
        g_GssCallback(&g_sample_data);
    }

    /// ShakeReport = NOSHAKE;
    // g_sample_data.shake = GS_SHAKE_NO_HAPPEN;

    sxr_StartFunctionTimer(g_sample_time,gsensor_monitor,(VOID*)NULL,0x03);

}





GSENSOR_ERR_T gsensor_start_init(VOID)
{
    return GSENSOR_ERR_NO;
}


//=============================================================================
// gsensor_start_monitor
//-----------------------------------------------------------------------------
///
/// @param sample_period_ms is the period of detectiong the change of gsensor.
/// generally 350ms is recommend.
//=============================================================================
GSENSOR_ERR_T gsensor_start_monitor(UINT32 sample_period_ms ) // generally 350ms is recommend.
{
    UINT32 scStatus = 0;
    GSENSOR_ERR_T ret = GSENSOR_ERR_RESOURCE_BUSY;
    scStatus = hal_SysEnterCriticalSection();
    g_sample_time = sample_period_ms MILLI_SECONDS;
    hal_SysExitCriticalSection(scStatus);
    if (g_start_monitor == FALSE)
    {
        sxr_StartFunctionTimer(g_sample_time,gsensor_monitor,(VOID*)NULL,0x03);
        ret = gsensor_enter_active_mode();
        g_start_monitor = TRUE;
    }
    sxs_fprintf(TSTDOUT, "gss:  start gsensor monitor period = %d ms  ",sample_period_ms);
    return ret;
}
GSENSOR_ERR_T gsensor_stop_monitor(VOID)
{
    GSENSOR_ERR_T ret = GSENSOR_ERR_RESOURCE_BUSY;
    if (g_start_monitor == TRUE)
    {
        sxr_StopFunctionTimer(gsensor_monitor);
        ret = gsensor_enter_standby_mode();
        g_start_monitor = FALSE;
    }
    sxs_fprintf(TSTDOUT, "gss:  stop gsensor monitor   ");
    return ret;
}



// =============================================================================
// gsensorClose
// -----------------------------------------------------------------------------
/// This function closes the FM driver and desactivate the FM chip
/// and stops any pending #gsensorTune or #gsensorSeek operation.
/// @return             \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC GSENSOR_ERR_T gsensorClose(VOID)
{
    return 1;
}

// ============================================================================
//  gsensor_Init
// ----------------------------------------------------------------------------
/// This function initializes the interrupter of gsensor module . The configuration of the
/// INT pin used of the gsensor stored in a GSENSOR_CONFIG_STRUCT_T struct .
/// @param enable_int is ture mean enable the interrupt
// ============================================================================
PUBLIC VOID gsensor_Init(BOOL enable_int)
{
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsensor init ...... ");

#if 0
    g_gsensorConfig = tgt_GetGsensorConfig();
    if (enable_int == TRUE)
    {
        // Configuration of the GPIO INT
        // GPIO cfg
        g_gsensor_gpioCfg.direction = HAL_GPIO_DIRECTION_INPUT;
        // GPIO irq mask cfg
        g_gsensor_gpioCfg.irqMask.rising = TRUE;
        g_gsensor_gpioCfg.irqMask.debounce = TRUE;
        g_gsensor_gpioCfg.irqHandler = gsensor_GpioIrqHandler;
        g_gsensor_gpioCfg.irqMask.level = FALSE;
        g_gsensor_gpioCfg.irqMask.falling =  FALSE;
        hal_GpioOpen(HAL_GPIO_4, &g_gsensor_gpioCfg);
        //hal_GpioOpen(HAL_GPIO_3, &g_gsensor_gpioCfg);
        //gsensor_set_interrupt(g_int_su);
    }

    // sxs_fprintf(TSTDOUT, "gss:  gsensor_Init  ");
    //gpio_i2c_init_gsensor();



    sxs_fprintf(TSTDOUT," gss:  gsensor_Init ");
    gsensor_start_init();
#endif
    sxs_fprintf(TSTDOUT," gss:  gsensor Init .... ");


}


VOID test_gsor()
{
    gsensor_Init(TRUE);
    gsensor_start_monitor(300);
}



// the follwing is test code .
#if 0
VOID test_inter_enable()
{
    gsensor_I2cOpen();
    gsensor_WriteSingle(MMA7660FC_ADDR_INTSU,0xff);
    gsensor_I2cClose();
}



VOID test_gsensor()
{
    //g_acc_data
    UINT8 g_test = 0;
    UINT8 x=0,y=0,z=0;
    gsensor_Init(TRUE);
    gsensor_I2cOpen();
    gsensor_enter_active_mode();
    gsensor_Filt_LP(g_threshold_orientation);
    gsensor_set_interrupt(g_int_su);
    gsensor_WriteSingle(5,0x6f);
    gsensor_WriteSingle(8,0x81);
    gsensor_ReadSingle(8,&g_test);

    // gsensor_WriteSingle(MMA7660FC_ADDR_INTSU,0xff);
    // for (i = 0;i < 11;i++)
    {
        //  gsensor_ReadSingle(i, &g_test);
        sxs_fprintf(TSTDOUT, "gss: reg 8 =  0x%x   ",g_test);
    }

    gsensor_I2cClose();
    gsensor_Read(0,(UINT8 *)&g_acc_data,11);
    sxs_fprintf(TSTDOUT, "gss:0 data[x_adc] = 0x%x ",g_acc_data.x_adc);
    sxs_fprintf(TSTDOUT, "gss:1 data[y_adc] = 0x%x ",g_acc_data.y_adc);
    sxs_fprintf(TSTDOUT, "gss:2 data[z_adc] = 0x%x ",g_acc_data.z_adc);
    sxs_fprintf(TSTDOUT, "gss:3 data[tilt] = 0x%x ",g_acc_data.tilt);
    sxs_fprintf(TSTDOUT, "gss:4 data[srsr] = 0x%x ",g_acc_data.srsr);
    sxs_fprintf(TSTDOUT, "gss:5 data[spcnt] = 0x%x ",g_acc_data.spcnt);
    sxs_fprintf(TSTDOUT, "gss:6 data[intsu] = 0x%x ",g_acc_data.intsu);
    sxs_fprintf(TSTDOUT, "gss:7 data[mode] = 0x%x ",g_acc_data.mode);
    sxs_fprintf(TSTDOUT, "gss:8 data[sr] = 0x%x ",g_acc_data.sr);
    sxs_fprintf(TSTDOUT, "gss:9 data[pdet] = 0x%x ",g_acc_data.pdet);
    sxs_fprintf(TSTDOUT, "gss:10 data[pd] = 0x%x ",g_acc_data.pd);
    gsensor_Read_xyz(&x,&y,&z);
    sxs_fprintf(TSTDOUT, "gss:  sensor  x= 0x%x  y= 0x%x   z= 0x%x  ",x,y,z);
    if ((x&0x10) == 0x10)
    {
        sxs_fprintf(TSTDOUT, "gss:  sensor  x = - 0x%x  ",x>>2);
    }
    else
    {
        sxs_fprintf(TSTDOUT, "gss:  sensor  x = + 0x%x  ",x>>2);
    }
    if ((y&0x10) == 0x10)
    {
        sxs_fprintf(TSTDOUT, "gss:  sensor  y = -0 x%x  ",y>>2);
    }
    else
    {
        sxs_fprintf(TSTDOUT, "gss:  sensor  y = + 0x%x  ",y>>2);

    }
    if ((z&0x10) == 0x10)
    {
        sxs_fprintf(TSTDOUT, "gss:  sensor  z = - 0x%x  ",z>>2);
    }
    else
    {
        sxs_fprintf(TSTDOUT, "gss:  sensor  z = + 0x%x  ",z>>2);
    }

}
#if 1
extern VOID mmi_LcdDispalyIcoytt(UINT8 position);

GSS_SAMPLE_DATA_T g_sample_data_old = {0};
UINT32 timer = 31;
UINT8 g_old_ff = 0;

VOID test_gsensor_proto()
{
    sxs_fprintf(TSTDOUT, "gss: timesr =%d , shake =  0x%x ShakeReport =%d",timer,g_sample_data.shake,ShakeReport);
    timer ++ ;
    if (g_sample_data.shake== GS_MOTION_SHAKE_Y_L_AXIS)
    {
        mmi_LcdDispalyIcoytt(8); timer = 0; g_sample_data_old.rotate = 0;   return;
    }
    if (g_sample_data.shake== GS_MOTION_SHAKE_Y_R_AXIS)
    {
        mmi_LcdDispalyIcoytt(7);  timer = 0; g_sample_data_old.rotate = 0;   return;

    }
    if (g_sample_data.shake== GS_MOTION_SHAKE_X_L_AXIS)
    {
        mmi_LcdDispalyIcoytt(10); timer = 0; g_sample_data_old.rotate = 0;   return;
    }
    if (g_sample_data.shake== GS_MOTION_SHAKE_X_R_AXIS)
    {
        mmi_LcdDispalyIcoytt(9);  timer = 0; g_sample_data_old.rotate = 0;   return;

    }

    if (g_sample_data.shake== GS_MOTION_SHAKE_Z_L_AXIS)
    {
        mmi_LcdDispalyIcoytt(11); timer = 0; g_sample_data_old.rotate = 0;   return;
    }
    if (g_sample_data.shake== GS_MOTION_SHAKE_Z_R_AXIS)
    {
        mmi_LcdDispalyIcoytt(12);  timer = 0; g_sample_data_old.rotate = 0;   return;

    }
    if(timer > 20)
    {
        if (g_sample_data.tunover == 0x22)
        {
            sxs_fprintf(TSTDOUT, "gss:  here 1 " );

            if (g_sample_data_old.tunover !=0x22)
            {
                sxs_fprintf(TSTDOUT, "gss:  here 2 " );
                g_sample_data_old.tunover = 0x22;
                sxs_fprintf(TSTDOUT, "gss:  testests 75  timesr =%d , old_turnover= 0x%x",timer,g_sample_data_old.tunover);

                mmi_LcdDispalyIcoytt(5);
                return ;
            }
        }
        if (g_sample_data.tunover == 0x21)
        {
            sxs_fprintf(TSTDOUT, "gss:  here 3 " );
            if (g_sample_data_old.tunover !=0x21)
            {
                g_sample_data_old.tunover = 0x21;
                sxs_fprintf(TSTDOUT, "gss:  testests 65  timesr =%d old_turnover= 0x%x",timer,g_sample_data_old.tunover);
                mmi_LcdDispalyIcoytt(6);
                return;
            }
        }
        if (g_sample_data.rotate == 0x3)
        {
            sxs_fprintf(TSTDOUT, "gss:  here 4 " );
            if (g_sample_data_old.rotate !=0x3)
            {
                g_sample_data_old.rotate = 0x3;
                sxs_fprintf(TSTDOUT, "gss:  testests 3  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(3); return;
            }
        }
        if (g_sample_data.rotate == 0x1)
        {
            if (g_sample_data_old.rotate !=0x1)
            {
                g_sample_data_old.rotate = 0x1;
                sxs_fprintf(TSTDOUT, "gss:  testests 1  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(4); return;
            }
        }
        if (g_sample_data.rotate == 0x4)
        {
            if (g_sample_data_old.rotate !=0x4)
            {
                g_sample_data_old.rotate = 0x4;
                sxs_fprintf(TSTDOUT, "gss:  testests 4  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(1); return;
            }
        }
        if (g_sample_data.rotate == 0x2)
        {
            if (g_sample_data_old.rotate !=0x2)
            {
                g_sample_data_old.rotate = 0x2;
                sxs_fprintf(TSTDOUT, "gss:  testests 2  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(2); return;
            }
        }



    }
}

VOID ytt_gsensor(GSS_SAMPLE_DATA_T* gsensor_data)
{

    sxs_fprintf(TSTDOUT, "gss:  gsscallback 0x%x",gsensor_data->tunover);


}

VOID set_test_callback()
{
    gsensor_SetCallback(ytt_gsensor);


}

VOID test_sleep_count_write()
{
    //gsensor_Read(0,(UINT8 *)&g_acc_data,11);
    //gsensor_Filt_LP(g_threshold_orientation);
    gsensor_WriteSingle(5,0x6f);
    gsensor_WriteSingle(6,0x80);
    sxs_fprintf(TSTDOUT, "gss:  test_sleep_count_ 2 write ");
}

VOID test_sleep_count_read()
{
    UINT8 data[4];
    // gsensor_ReadSingle(5,&data);
    //   gsensor_Read(4,data,4);
    //sxs_fprintf(TSTDOUT, "gss:  test_sleep_count_  read 0x%x",data[1]);
    gsensor_Read(5,data,1);
}
#endif
#endif



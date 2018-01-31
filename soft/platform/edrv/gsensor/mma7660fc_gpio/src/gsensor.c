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
#include "mma7660fc.h"
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


#define GSENSOR_CHIP_ADRESS   0x4c                //0x11//0010001


#define gMaxValue 31
#define gMinValue -32

#define Shake_HighTh 28
#define Shake_LowTh -29

#define ShakeDelay_TimeOut 30 //40
#define ShakeDelay_Interval 15
#define SHAKE_INTERVAL 30 //40
#define Shake_detect 25

INT8 Xnew8, Ynew8, Znew8;

UINT8 ShakeSenseStep;
INT8 Xfst, Yfst, Zfst;
INT8 Xsnd, Ysnd, Zsnd;
UINT8 Shake_interval_Delay = 0;
ShakeDir ShakeReport;
INT8 X_shake[Shake_detect] = {0};
INT8 Y_shake[Shake_detect] = {0};
INT8 Z_shake[Shake_detect] = {0};
UINT8 X_shake_index = 0;
UINT8 Y_shake_index = 0;
UINT8 Z_shake_index = 0;


GSS_SAMPLE_DATA_T g_sample_data = {0};
UINT8 g_threshold_orientation = 7;
PRIVATE GSS_CALLBACK_T          g_GssCallback = NULL;


#define shake_threshold = 8;
/// This array contain the value of the corresponding mma7660 all registers

GSS_ACC_DATE_T g_acc_data =
{
    .x_adc = 0,
    .y_adc = 0,
    .z_adc = 0,
    .tilt = 0,
    .srsr = 0,
    .spcnt = 0,
    .intsu = 0,
    .mode = 0,
    .sr = 0,
    .pdet = 0,
    .pd = 0

};
// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================
extern PUBLIC HAL_ERR_T hal_I2cSendByte_gpio(UINT32 slaveAddr, UINT8 memAddr, UINT8 data);
extern PUBLIC HAL_ERR_T hal_I2cGetByte_gpio(UINT32 slaveAddr, UINT8 memAddr, UINT8* pData);
extern PUBLIC HAL_ERR_T hal_I2cSendData_gpio(UINT32 slaveAddr,  UINT8 memAddr, UINT8* pData, UINT8 length);
extern PUBLIC HAL_ERR_T hal_I2cGetData_gpio(UINT32 slaveAddr,   UINT8 memAddr,UINT8* pData,  UINT8 length);

/// This array contain the value of the corresponding INT sur registers

#if 0
MMA7660FC_INTSU_CFG_T g_int_su =
{
    {
        .FBint = TRUE,
        .PLint = TRUE,
        .PDint = TRUE,
        .ASint = FALSE,
        .Gint = FALSE,
        .SHintZ = FALSE,
        .SHintY = FALSE,
        .SHintX = FALSE,
    },

};
#else
MMA7660FC_INTSU_CFG_T g_int_su =
{
    {
        .FBint = FALSE,
        .PLint = FALSE,
        .PDint = FALSE,
        .ASint = FALSE,
        .Gint = FALSE,
        .SHintZ = TRUE,
        .SHintY = TRUE,
        .SHintX = TRUE,
    },

};
#endif

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

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsensor_GpioIrqHandler  ");
    //sxr_StartFunctionTimer(g_sample_time,gsensor_monitor,(VOID*)NULL,0x03);
#if 0
    TSD_PROFILE_FUNCTION_ENTRY(tsd_GpioIrqHandler);
    // We check the value of the penIrqPin to avoid false triggering when we reset
    // the touch screen controler.

    if ( hal_GpioGet(g_tsdConfig->penGpio) == 1 )
    {
        // We disable host Pen IRQ while the screen is pressed.
        tsd_DisableHostPenIrq();
        sxr_StartFunctionTimer(g_tsdConfig->debounceTime,tsd_Debounce,(VOID*)NULL,0x03);
    }
    TSD_PROFILE_FUNCTION_EXIT(tsd_GpioIrqHandler);
#endif
}

VOID gsensor_I2cOpen(VOID)
{
    //hal_I2cOpen(g_gsensorConfig->i2cBusId);

}
VOID gsensor_I2cClose(VOID)
{
    //hal_I2cClose(g_gsensorConfig->i2cBusId);
}

GSENSOR_ERR_T gsensor_ReadSingle(UINT8 addr, UINT8* data)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,addr,data);
    if (sensor_err != HAL_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: single read I2C busy ! ");
    }
    return GSENSOR_ERR_NO;
}


GSENSOR_ERR_T gsensor_WriteSingle(UINT8 addr, UINT8 data)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    sensor_err = hal_I2cSendByte_gpio(GSENSOR_CHIP_ADRESS,addr,data);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: single addr =%d   write data 0x%x  ! ",addr,data);
    if (sensor_err != HAL_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: single write I2C busy ! ");
    }
    return GSENSOR_ERR_NO;
}


GSENSOR_ERR_T gsensor_Write(UINT8 addr, UINT8* data,UINT8 length)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    gsensor_I2cOpen();
    sensor_err = hal_I2cSendData_gpio(GSENSOR_CHIP_ADRESS, addr,data,length);
    gsensor_I2cClose();
    if (sensor_err != HAL_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: write I2C busy ! ");
        return GSENSOR_ERR_RESOURCE_BUSY;
    }
    else
    {
        return GSENSOR_ERR_NO;
    }
}

GSENSOR_ERR_T gsensor_Read_xyz(UINT8* x_adc, UINT8* y_adc,UINT8* z_adc)
{
    HAL_ERR_T sensor_err = HAL_ERR_RESOURCE_BUSY;
    gsensor_I2cOpen();
    while(sensor_err != HAL_ERR_NO)
    {
        sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,MMA7660FC_ADDR_XOUT,x_adc);
        if ((*x_adc)&MMA7660FC_XOUT_ALERT)
        {
            sxr_Sleep(1 MILLI_SECONDS);
            sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,MMA7660FC_ADDR_XOUT,x_adc);
            GSS_TRACE(GSS_INFO_TRC, 0, "gss: x_adc alert ! ");
            if ((*x_adc)&MMA7660FC_XOUT_ALERT)
            {
                GSS_ASSERT(0, "read x_adc error.");

            }
        }
    }
    sensor_err = HAL_ERR_RESOURCE_BUSY;
    while(sensor_err != HAL_ERR_NO)
    {
        sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,MMA7660FC_ADDR_YOUT,y_adc);
        if ((*y_adc)&MMA7660FC_XOUT_ALERT)
        {
            sxr_Sleep(1 MILLI_SECONDS);
            sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,MMA7660FC_ADDR_YOUT,y_adc);
            GSS_TRACE(GSS_INFO_TRC, 0, "gss: y_adc alert ! ");
            if ((*y_adc)&MMA7660FC_XOUT_ALERT)
            {
                GSS_ASSERT(0, "read y_adc error.");

            }
        }
    }

    sensor_err = HAL_ERR_RESOURCE_BUSY;
    while(sensor_err != HAL_ERR_NO)
    {
        sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,MMA7660FC_ADDR_ZOUT,z_adc);
        if ((*z_adc)&MMA7660FC_XOUT_ALERT)
        {
            sxr_Sleep(1 MILLI_SECONDS);
            sensor_err = hal_I2cGetByte_gpio(GSENSOR_CHIP_ADRESS,MMA7660FC_ADDR_ZOUT,z_adc);
            GSS_TRACE(GSS_INFO_TRC, 0, "gss: z_adc alert ! ");
            if ((*z_adc)&MMA7660FC_XOUT_ALERT)
            {
                GSS_ASSERT(0, "read z_adc error.");

            }
        }
    }
    gsensor_I2cClose();
    return sensor_err;

}


GSENSOR_ERR_T gsensor_Read(UINT8 addr, UINT8* data,UINT8 length)
{
    HAL_ERR_T sensor_err = HAL_ERR_NO;
    gsensor_I2cOpen();
    sensor_err = hal_I2cGetData_gpio(GSENSOR_CHIP_ADRESS,addr,data,length);
    gsensor_I2cClose();
    if (sensor_err != HAL_ERR_NO)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: read  I2C busy ! ");
        return GSENSOR_ERR_RESOURCE_BUSY;
    }
    else
    {
        return GSENSOR_ERR_NO;
    }
}

GSENSOR_ERR_T gsensor_enter_active_mode(VOID)
{
    UINT8 mode_reg = 0;
    gsensor_I2cOpen();
    gsensor_ReadSingle(MMA7660FC_ADDR_MODE,&mode_reg);
    mode_reg = mode_reg | MMA7660FC_MODE |MMA7660FC_IAH |MMA7660FC_IPP; // high mean interrupt
    gsensor_WriteSingle(MMA7660FC_ADDR_MODE,mode_reg);
    gsensor_I2cClose();
    return GSENSOR_ERR_NO;
}

GSENSOR_ERR_T gsensor_enter_standby_mode(VOID)
{
    UINT8 mode_reg = 0;
    gsensor_I2cOpen();
    gsensor_ReadSingle(MMA7660FC_ADDR_MODE,&mode_reg);
    mode_reg = mode_reg & 0xf8;
    gsensor_WriteSingle(MMA7660FC_ADDR_MODE,mode_reg);
    gsensor_I2cClose();
    return GSENSOR_ERR_NO;
}

//=============================================================================
// gsensor_set_interrupt
//-----------------------------------------------------------------------------
/// setup the config register according the  specific application.
/// @param int_reg is corresponding to interrupt register.
///
//=============================================================================
GSENSOR_ERR_T gsensor_set_interrupt(MMA7660FC_INTSU_CFG_T int_reg)
{
    gsensor_I2cOpen();
    // gsensor_WriteSingle(MMA7660FC_ADDR_INTSU,int_reg.intsu_reg);
    gsensor_WriteSingle(MMA7660FC_ADDR_INTSU,int_reg.intsu_reg);
    gsensor_I2cClose();
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: set the interrup 0x%x ! ",int_reg.intsu_reg);
    return GSENSOR_ERR_NO;
}

//===========================================================================================================
//Shake Detection Routine
//===========================================================================================================

UINT32 y_axis_peak = 0,y_axis_bottom = 0;
UINT32 x_axis_peak = 0,x_axis_bottom = 0;
UINT32 z_axis_peak = 0,z_axis_bottom = 0;

UINT8 Y_data[2]= {0};
UINT8 X_data[2]= {0};
UINT8 Z_data[2]= {0};

#define RANGE_MONITOR 23
VOID gsensor_shake_y()
{
    INT8 k = 0;
    UINT8 y_fst = 0,y_snd = 0;
    UINT8 i = 0;

    for ( i=32; i>0; i--)
    {
        if (y_axis_peak &(1<<(i-1)))
        {

            if (k==0)
            {
                y_fst ++;
                k=1;
            }
            if (k == -1)
            {
                y_fst = 1;
                k=1;

            }
            if (k == 1)
            {
                y_fst ++;
            }
        }
        else
        {
            if (y_axis_bottom & (1<<i))
            {
                if (k==1)
                {
                    y_snd =1;
                    k = -1;
                }
                if (k == 0)
                {
                    y_snd ++;
                    k = -1;
                }
                if (k == -1)
                {
                    y_snd ++;
                }
            }
        }

    }
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_detect_y  y_fst =%d y_snd=%d  k=%d",y_fst,y_snd,k);

    if((y_fst > 0) && (y_snd > 0))
    {
        Y_data[0] = y_fst + y_snd ;

        if (k ==1 )
        {
            Y_data[1] = 1;
        }
        else if( k == -1)
        {
            Y_data[1] = 2;
        }


    }
    else
    {
        Y_data[0] = 0;
        Y_data[1] = 0;

    }

}

VOID gsensor_shake_x()
{
    INT8 k = 0;
    UINT8 x_fst = 0,x_snd = 0;
    UINT8 i = 0;


    for ( i=32; i>0; i--)
    {
        if (x_axis_peak &(1<<(i-1)))
        {

            if (k==0)
            {
                x_fst ++;
                k=1;
            }
            if (k == -1)
            {
                x_fst = 1;
                k=1;

            }
            if (k == 1)
            {
                x_fst ++;
            }
        }
        else
        {
            if (x_axis_bottom & (1<<i))
            {
                if (k==1)
                {
                    x_snd =1;
                    k = -1;
                }
                if (k == 0)
                {
                    x_snd ++;
                    k = -1;
                }
                if (k == -1)
                {
                    x_snd ++;
                }
            }
        }

    }
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_detect_x  x_fst =%d x_snd=%d  k=%d",x_fst,x_snd,k);

    if((x_fst > 0) && (x_snd > 0))
    {
        X_data[0] = x_fst + x_snd ;

        if (k ==1 )
        {
            X_data[1] = 1;
        }
        else if( k == -1)
        {
            X_data[1] = 2;
        }


    }
    else
    {
        X_data[0] = 0;
        X_data[1] = 0;

    }

}



VOID gsensor_shake_z()
{
    INT8 k = 0;
    UINT8 z_fst = 0,z_snd = 0;
    UINT8 i = 0;


    for ( i=32; i>0; i--)
    {
        if (z_axis_peak &(1<<(i-1)))
        {

            if (k==0)
            {
                z_fst ++;
                k=1;
            }
            else
            {
                if (k == 1)
                {
                    z_fst ++;
                }
            }
            if (k == -1)
            {
                z_fst = 1;
                k=1;

            }

        }
        else
        {
            if (z_axis_bottom & (1<<i))
            {
                if (k==1)
                {
                    z_snd =1;
                    k = -1;
                }
                else
                {
                    if (k == -1)
                    {
                        z_snd ++;
                    }
                }
                if (k == 0)
                {
                    z_snd ++;
                    k = -1;
                }
            }
        }

    }
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_detect_z  z_fst =%d z_snd=%d  k=%d",z_fst,z_snd,k);

    if((z_fst > 0) && (z_snd > 0))
    {
        Z_data[0] = z_fst + z_snd ;

        if (k ==1 )
        {
            Z_data[1] = 1;
        }
        else if( k == -1)
        {
            Z_data[1] = 2;
        }


    }
    else
    {
        Z_data[0] = 0;
        Z_data[1] = 0;

    }

}





VOID gsensor_shake_detect_y()
{


    if ((Y_data[0] >=2))
    {
        if (Y_data[1] == 1)
        {
            ShakeReport = YSKNEG;
            Shake_interval_Delay = SHAKE_INTERVAL;
        }
        else if (Y_data[1] == 2)
        {
            ShakeReport = YSKPOS ;
            Shake_interval_Delay = SHAKE_INTERVAL;
        }
        else
        {
            ShakeReport = NOSHAKE ;
        }

    }
    else
    {
        ShakeReport = NOSHAKE ;
    }
    Y_data[0] = 0;
    Y_data[1] = 0;
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: y_axis_peak = 0x%x  bottom = 0x%x  ShakeReport = %d",y_axis_peak,y_axis_bottom,ShakeReport);
}
VOID gsensor_shake_detect_x()
{
    if ((X_data[0] >=2))
    {
        if (X_data[1] == 1)
        {
            ShakeReport = XSKNEG;
            Shake_interval_Delay = SHAKE_INTERVAL;
        }
        else if (X_data[1] == 2)
        {
            ShakeReport = XSKPOS ;
            Shake_interval_Delay = SHAKE_INTERVAL;
        }
        else
        {
            ShakeReport = NOSHAKE ;
        }

    }
    else
    {
        ShakeReport = NOSHAKE ;
    }
    X_data[0] = 0;
    X_data[1] = 0;
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: x_axis_peak = 0x%x  bottom = 0x%x  ShakeReport = %d",x_axis_peak,x_axis_bottom,ShakeReport);
}
VOID gsensor_shake_detect_z()
{
    if ((Z_data[0] >=2))
    {
        if (Z_data[1] == 1)
        {
            ShakeReport = ZSKNEG;
            Shake_interval_Delay = SHAKE_INTERVAL;
        }
        else if (Z_data[1] == 2)
        {
            ShakeReport = ZSKPOS ;
            Shake_interval_Delay = SHAKE_INTERVAL;
        }
        else
        {
            ShakeReport = NOSHAKE ;
        }

    }
    else
    {
        ShakeReport = NOSHAKE ;
    }
    Z_data[0] = 0;
    Z_data[1] = 0;
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: z_axis_peak = 0x%x  bottom = 0x%x  ShakeReport = %d",z_axis_peak,z_axis_bottom,ShakeReport);
}
VOID gsensor_shake_detect_all()
{
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: gsensor_shake_detect_all Y_data[0] =%d  Z_data[0] = %d X_data[0] = %d",Y_data[0],Z_data[0],X_data[0]);
    if (Y_data[0] > Z_data[0])
    {
        if (Y_data[0] > X_data[0])
        {
            gsensor_shake_detect_y();
            return;
        }

    }
    if (Z_data[0] > X_data[0])
    {
        if (Z_data[0] > Y_data[0])
        {
            gsensor_shake_detect_z();
            return;
        }

    }
    if (X_data[0] > Z_data[0])
    {
        if (X_data[0] > Y_data[0])
        {
            gsensor_shake_detect_x();
            return;
        }

    }

}

VOID gsensor_shake_store_y(VOID)
{
    if (Ynew8 >= Shake_HighTh)
    {
        y_axis_peak =  (y_axis_peak << 1)|0x1 ;
    }
    else
    {
        y_axis_peak =  (y_axis_peak << 1);
    }
    if (Ynew8 <= Shake_LowTh)
    {
        y_axis_bottom =  (y_axis_bottom << 1)|0x1 ;
    }
    else
    {
        y_axis_bottom =  (y_axis_bottom << 1);
    }

    // GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_store y_peak = 0x%x  y_bott = 0x%x " ,y_axis_peak,y_axis_bottom);

}
VOID gsensor_shake_store_x(VOID)
{
    if (Xnew8 >= Shake_HighTh)
    {
        x_axis_peak =  (x_axis_peak << 1)|0x1 ;
    }
    else
    {
        x_axis_peak =  (x_axis_peak << 1);
    }
    if (Xnew8 <= Shake_LowTh)
    {
        x_axis_bottom =  (x_axis_bottom << 1)|0x1 ;
    }
    else
    {
        x_axis_bottom =  (x_axis_bottom << 1);
    }

    // GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_store y_peak = 0x%x  y_bott = 0x%x " ,y_axis_peak,y_axis_bottom);

}
VOID gsensor_shake_store_z(VOID)
{
    if (Znew8 >= Shake_HighTh)
    {
        z_axis_peak =  (z_axis_peak << 1)|0x1 ;
    }
    else
    {
        z_axis_peak =  (z_axis_peak << 1);
    }
    if (Znew8 <= Shake_LowTh)
    {
        z_axis_bottom =  (z_axis_bottom << 1)|0x1 ;
    }
    else
    {
        z_axis_bottom =  (z_axis_bottom << 1);
    }

    // GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_store y_peak = 0x%x  y_bott = 0x%x " ,y_axis_peak,y_axis_bottom);

}
VOID gsensor_shake_all()
{
    if (Shake_interval_Delay >0)
    {
        Shake_interval_Delay --;
    }


    if (Shake_interval_Delay > 0)
    {

        GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool gsensor_shake_all shake delay =%d ",Shake_interval_Delay);
        return;
    }
    if (((x_axis_peak | x_axis_bottom) & 0x00010000 )||((z_axis_peak | z_axis_bottom) & 0x00010000 )||((y_axis_peak | y_axis_bottom) & 0x00010000 ))
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: cool start shake ");
        gsensor_shake_y();
        gsensor_shake_x();
        gsensor_shake_z();
        gsensor_shake_detect_all();

    }
}



// the g-cell allowable range of +31 to -32
VOID gsensor_monitor(VOID)
{
    if (GSENSOR_ERR_NO !=gsensor_Read(0,(UINT8 *)&g_acc_data,11))
    {
        sxr_StartFunctionTimer(2 MILLI_SECONDS,gsensor_monitor,(VOID*)NULL,0x03);
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsensor monitor alert ");
        return;

    }

    if (((g_acc_data.x_adc&0x40) == 0x40)||((g_acc_data.y_adc&0x40) == 0x40)||
            ((g_acc_data.z_adc&0x40) == 0x40)||((g_acc_data.tilt&0x40) == 0x40))
    {
        sxr_StartFunctionTimer(2 MILLI_SECONDS,gsensor_monitor,(VOID*)NULL,0x03);
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsensor monitor alert ");
        return;
    }
    if (g_acc_data.x_adc&0x20) g_acc_data.x_adc |= 0xC0;
    if (g_acc_data.y_adc&0x20) g_acc_data.y_adc |= 0xC0;
    if (g_acc_data.z_adc&0x20) g_acc_data.z_adc |= 0xC0;


    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  hi  x = 0x%x  ",g_acc_data.x_adc);
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  hi  y = 0x%x  ",g_acc_data.y_adc);
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  hi  z = 0x%x  ",g_acc_data.z_adc);

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  hi  x = %d  y =%d  z =%d ",g_acc_data.x_adc,g_acc_data.y_adc,g_acc_data.z_adc);
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  hi  y = %d  ",g_acc_data.y_adc);
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  hi  z = %d  ",g_acc_data.z_adc);
    Xnew8 = g_acc_data.x_adc;
    Ynew8 = g_acc_data.y_adc;
    Znew8 = g_acc_data.z_adc;
    g_sample_data.x = Xnew8;
    g_sample_data.y = Ynew8;
    g_sample_data.z = Znew8;
    gsensor_shake_store_y();
    gsensor_shake_store_z();
    gsensor_shake_store_x();
    gsensor_shake_all();

    if (ShakeReport == NOSHAKE )
    {
        g_sample_data.shake = GS_SHAKE_NO_HAPPEN; // doing something
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_NO_HAPPEN  0x%x ",g_sample_data.shake);
    }
    else if (ShakeReport == XSKPOS )
    {
        g_sample_data.shake = GS_MOTION_SHAKE_X_L_AXIS; // doing something
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_HAPPEN  0x%x ",g_sample_data.shake);
    }
    else if (ShakeReport == XSKNEG )
    {
        g_sample_data.shake = GS_MOTION_SHAKE_X_R_AXIS; // doing something
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_HAPPEN  0x%x ",g_sample_data.shake);
    }
    else if (ShakeReport == YSKPOS )
    {
        g_sample_data.shake = GS_MOTION_SHAKE_Y_L_AXIS; // doing something
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_HAPPEN  0x%x ",g_sample_data.shake);
    }
    else if (ShakeReport == YSKNEG )
    {
        g_sample_data.shake = GS_MOTION_SHAKE_Y_R_AXIS; // doing something
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_HAPPEN  0x%x ",g_sample_data.shake);
    }
    else if (ShakeReport == ZSKPOS )
    {
        g_sample_data.shake = GS_MOTION_SHAKE_Z_L_AXIS; // doing something
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_HAPPEN  0x%x ",g_sample_data.shake);
    }
    else if (ShakeReport == ZSKNEG )
    {
        g_sample_data.shake = GS_MOTION_SHAKE_Z_R_AXIS; // doing something
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: MMA7660FC_SHAKE_HAPPEN  0x%x ",g_sample_data.shake);
    }



    //turn over
    if ((g_acc_data.tilt & MMA7660FC_BA_FRO_MASK) == MMA7660FC_BA_FRO_FRONT)
    {
        g_sample_data.tunover = GS_MOTION_TURNOVER_FRONT;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_TURNOVER_FRONT  0x%x ",g_sample_data.tunover);
    }
    else if ((g_acc_data.tilt & MMA7660FC_BA_FRO_MASK) == MMA7660FC_BA_FRO_BACK)
    {
        g_sample_data.tunover = GS_MOTION_TURNOVER_BACK;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_TURNOVER_BACK  0x%x ",g_sample_data.tunover);
    }
    else
    {
        g_sample_data.tunover = GS_MOTION_TURNOVER_UNKNOW;
        // GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_TURNOVER_UNKNOW  0x%x ",g_sample_data.tunover);
    }
    // tap
    if ((g_acc_data.tilt & MMA7660FC_TAP_NO_HAPPEN) == MMA7660FC_TAP_NO_HAPPEN)
    {
        g_sample_data.tap = GS_MONTON_TAP_NO_HAPPEN ;
        // GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MONTON_TAP_NO_HAPPEN  0x%x ",g_sample_data.tap);
    }
    else
    {
        g_sample_data.tap = GS_MONTON_TAP_HAPPEN;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MONTON_TAP_HAPPEN  0x%x ",g_sample_data.tap);
    }



    // rotate
    if ((g_acc_data.tilt & MMA7660FC_PO_LA_MASK) == MMA7660FC_PO_LA_LEFT)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_LEFT;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_ROTATE_LEFT  0x%x ",g_sample_data.rotate);
    }
    else if ((g_acc_data.tilt & MMA7660FC_PO_LA_MASK) == MMA7660FC_PO_LA_RIGHT)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_RIGHT;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_ROTATE_RIGHT  0x%x ",g_sample_data.rotate);
    }
    else if ((g_acc_data.tilt & MMA7660FC_PO_LA_MASK) == MMA7660FC_PO_LA_DWON)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_DOWN;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_ROTATE_DOWN  0x%x ",g_sample_data.rotate);
    }
    else if ((g_acc_data.tilt & MMA7660FC_PO_LA_MASK) == MMA7660FC_PO_LA_UP)
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_UP;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_ROTATE_UP  0x%x ",g_sample_data.rotate);
    }
    else
    {
        g_sample_data.rotate = GS_MOTION_ROTATE_UNKNOW;
        //GSS_TRACE(GSS_INFO_TRC, 0, "gss: GS_MOTION_ROTATE_UNKNOW  0x%x ",g_sample_data.rotate);

    }

    // GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[tilt] ytt = 0x%x ",g_acc_data.tilt&0x1c);
#if 0
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[tilt] = 0x%x ",g_acc_data.tilt);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[srsr] = 0x%x ",g_acc_data.srsr);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[spcnt] = 0x%x ",g_acc_data.spcnt);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[intsu] = 0x%x ",g_acc_data.intsu);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[mode] = 0x%x ",g_acc_data.mode);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[sr] = 0x%x ",g_acc_data.sr);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[pdet] = 0x%x ",g_acc_data.pdet);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: data[pd] = 0x%x ",g_acc_data.pd);
#endif
    if (g_GssCallback != NULL)
    {
        g_GssCallback(&g_sample_data);
    }
    //test_gsensor_proto();
    ShakeReport = NOSHAKE;
    g_sample_data.shake = GS_SHAKE_NO_HAPPEN;
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss: gsensor_monitor exit ShakeReport = %d ",ShakeReport);
    sxr_StartFunctionTimer(g_sample_time,gsensor_monitor,(VOID*)NULL,0x03);

}


GSENSOR_ERR_T gsensor_start_init(VOID)
{
    UINT32 scStatus = 0;
    GSENSOR_ERR_T ret = GSENSOR_ERR_RESOURCE_BUSY;
    scStatus = hal_SysEnterCriticalSection();
    hal_SysExitCriticalSection(scStatus);

    gsensor_I2cOpen();
    gsensor_WriteSingle(MMA7660FC_ADDR_MODE,0);
    gsensor_I2cClose();


    gsensor_Filt_LP(g_threshold_orientation);
    gsensor_set_interrupt(g_int_su);
    gsensor_WriteSingle(5,0xf0);
    //gsensor_enter_active_mode();
    return ret;
}


//=============================================================================
// gsensor_start_monitor
//-----------------------------------------------------------------------------
///
/// @param sample_period_ms is the period of detectiong the change of gsensor.
/// generally 35ms is recommend.
//=============================================================================
GSENSOR_ERR_T gsensor_start_monitor(UINT32 sample_period_ms )
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
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  start gsensor monitor period = %d ms  ",sample_period_ms*1000/16384);
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
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  stop gsensor monitor   ");
    return ret;
}

//=============================================================================
// gsensor_Filt_LP
//-----------------------------------------------------------------------------
/// config the counts of filtering the portrail/landscapes samples.
/// @param counts
/// counts is 0 - 7 , deafult 4
//=============================================================================
GSENSOR_ERR_T gsensor_Filt_LP(UINT8 count)
{
    UINT8 reg_data = 0;
    gsensor_I2cOpen();
    gsensor_ReadSingle(MMA7660FC_ADDR_SR,&reg_data);
    reg_data = (reg_data & 0x1f)| (count <<5);
    gsensor_WriteSingle(MMA7660FC_ADDR_SR,reg_data);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsensor_Filt_LP 0x%x ",reg_data);
    gsensor_I2cClose();
    return GSENSOR_ERR_NO;
}


//=============================================================================
// gsensor_filt_orientation
//-----------------------------------------------------------------------------
/// config the counts of filtering the portrail/landscapes samples.
/// @param counts
/// counts is 0 - 7 , deafult 4
//=============================================================================
VOID gsensor_filt_orientation(UINT8 count)
{
    if (count<=7)
    {
        g_threshold_orientation = count;
    }
}

//=============================================================================
// gsensor_set_shake_Axis
//-----------------------------------------------------------------------------
/// this function decide on which axis the shake will be detected.
/// @param
/// dealut x axis is enable.
//=============================================================================
VOID gsensor_set_shake_Axis(GSENSOR_SHAKE_AXIS xyz)
{

    if (xyz == SHANK_X_AXIS)
    {
        g_int_su.SHintX = TRUE;
        g_int_su.SHintY = FALSE;
        g_int_su.SHintZ = FALSE;
    }
    else if (xyz == SHANK_Y_AXIS)
    {
        g_int_su.SHintX = FALSE;
        g_int_su.SHintY = TRUE;
        g_int_su.SHintZ = FALSE;
    }
    else if (xyz == SHANK_Z_AXIS)
    {
        g_int_su.SHintX = FALSE;
        g_int_su.SHintY = FALSE;
        g_int_su.SHintZ = TRUE;
    }
    else if (xyz == SHANK_XYZ_AXIS)
    {
        g_int_su.SHintX = TRUE;
        g_int_su.SHintY = TRUE;
        g_int_su.SHintZ = TRUE;
    }

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
extern BOOL gpio_i2c_init_gsensor(VOID);

// ============================================================================
//  gsensor_Init
// ----------------------------------------------------------------------------
/// This function initializes the interrupter of gsensor module . The configuration of the
/// INT pin used of the gsensor stored in a GSENSOR_CONFIG_STRUCT_T struct .
/// @param enable_int is ture mean enable the interrupt
// ============================================================================
PUBLIC VOID gsensor_Init(BOOL enable_int)
{
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
        hal_GpioOpen(g_gsensorConfig->gsensorGpio, &g_gsensor_gpioCfg);
        //hal_GpioOpen(HAL_GPIO_3, &g_gsensor_gpioCfg);
        //gsensor_set_interrupt(g_int_su);
    }

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsensor_Init  ");
    gpio_i2c_init_gsensor();

    gsensor_start_init();
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
        GSS_TRACE(GSS_INFO_TRC, 0, "gss: reg 8 =  0x%x   ",g_test);
    }

    gsensor_I2cClose();
    gsensor_Read(0,(UINT8 *)&g_acc_data,11);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:0 data[x_adc] = 0x%x ",g_acc_data.x_adc);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:1 data[y_adc] = 0x%x ",g_acc_data.y_adc);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:2 data[z_adc] = 0x%x ",g_acc_data.z_adc);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:3 data[tilt] = 0x%x ",g_acc_data.tilt);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:4 data[srsr] = 0x%x ",g_acc_data.srsr);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:5 data[spcnt] = 0x%x ",g_acc_data.spcnt);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:6 data[intsu] = 0x%x ",g_acc_data.intsu);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:7 data[mode] = 0x%x ",g_acc_data.mode);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:8 data[sr] = 0x%x ",g_acc_data.sr);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:9 data[pdet] = 0x%x ",g_acc_data.pdet);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:10 data[pd] = 0x%x ",g_acc_data.pd);
    gsensor_Read_xyz(&x,&y,&z);
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  x= 0x%x  y= 0x%x   z= 0x%x  ",x,y,z);
    if ((x&0x10) == 0x10)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  x = - 0x%x  ",x>>2);
    }
    else
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  x = + 0x%x  ",x>>2);
    }
    if ((y&0x10) == 0x10)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  y = -0 x%x  ",y>>2);
    }
    else
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  y = + 0x%x  ",y>>2);

    }
    if ((z&0x10) == 0x10)
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  z = - 0x%x  ",z>>2);
    }
    else
    {
        GSS_TRACE(GSS_INFO_TRC, 0, "gss:  sensor  z = + 0x%x  ",z>>2);
    }

}
#if 1
extern VOID mmi_LcdDispalyIcoytt(UINT8 position);

GSS_SAMPLE_DATA_T g_sample_data_old = {0};
UINT32 timer = 31;
UINT8 g_old_ff = 0;

VOID test_gsensor_proto()
{
    GSS_TRACE(GSS_INFO_TRC, 0, "gss: timesr =%d , shake =  0x%x ShakeReport =%d",timer,g_sample_data.shake,ShakeReport);
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
            GSS_TRACE(GSS_INFO_TRC, 0, "gss:  here 1 " );

            if (g_sample_data_old.tunover !=0x22)
            {
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  here 2 " );
                g_sample_data_old.tunover = 0x22;
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  testests 75  timesr =%d , old_turnover= 0x%x",timer,g_sample_data_old.tunover);

                mmi_LcdDispalyIcoytt(5);
                return ;
            }
        }
        if (g_sample_data.tunover == 0x21)
        {
            GSS_TRACE(GSS_INFO_TRC, 0, "gss:  here 3 " );
            if (g_sample_data_old.tunover !=0x21)
            {
                g_sample_data_old.tunover = 0x21;
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  testests 65  timesr =%d old_turnover= 0x%x",timer,g_sample_data_old.tunover);
                mmi_LcdDispalyIcoytt(6);
                return;
            }
        }
        if (g_sample_data.rotate == 0x3)
        {
            GSS_TRACE(GSS_INFO_TRC, 0, "gss:  here 4 " );
            if (g_sample_data_old.rotate !=0x3)
            {
                g_sample_data_old.rotate = 0x3;
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  testests 3  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(3); return;
            }
        }
        if (g_sample_data.rotate == 0x1)
        {
            if (g_sample_data_old.rotate !=0x1)
            {
                g_sample_data_old.rotate = 0x1;
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  testests 1  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(4); return;
            }
        }
        if (g_sample_data.rotate == 0x4)
        {
            if (g_sample_data_old.rotate !=0x4)
            {
                g_sample_data_old.rotate = 0x4;
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  testests 4  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(1); return;
            }
        }
        if (g_sample_data.rotate == 0x2)
        {
            if (g_sample_data_old.rotate !=0x2)
            {
                g_sample_data_old.rotate = 0x2;
                GSS_TRACE(GSS_INFO_TRC, 0, "gss:  testests 2  timesr =%d  old_rotate= 0x%x",timer,g_sample_data_old.rotate);
                mmi_LcdDispalyIcoytt(2); return;
            }
        }



    }
}

VOID ytt_gsensor(GSS_SAMPLE_DATA_T* gsensor_data)
{

    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  gsscallback 0x%x",gsensor_data->tunover);


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
    GSS_TRACE(GSS_INFO_TRC, 0, "gss:  test_sleep_count_ 2 write ");
}

VOID test_sleep_count_read()
{
    UINT8 data[4];
    // gsensor_ReadSingle(5,&data);
    //   gsensor_Read(4,data,4);
    //GSS_TRACE(GSS_INFO_TRC, 0, "gss:  test_sleep_count_  read 0x%x",data[1]);
    gsensor_Read(5,data,1);
}
#endif
#endif



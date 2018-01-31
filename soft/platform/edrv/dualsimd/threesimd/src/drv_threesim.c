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
#include "hal_sys.h"
#include "stdlib.h"
#include "sxs_io.h"
#include "hal_spi.h"

#include "hal_timers.h"
#include "drv_dualsim.h"
#include "dualsim_config.h"
#include "edrvp_debug.h"
#include "rda1203.h"
#include "hal_gpio.h"
#include "sxr_tls.h"


#define        DS_NULLVOLT_DLY          (20) // delay in 1/1000 sec
#define        DS_SETVOLT_DLY           (50) // delay in 1/1000 sec

#define        DS_SETCLK_DLY            (1)
#define        DS_SETDATA_DLY           (1)
#define        DS_SETRST_DLY            (1)
#define        DS_SETSIMEN_DLY          (1)


#ifndef DUALSIM_SWAP
#define        DUALSIM_CARD0_VSEL       (1<<0)
#define        DUALSIM_CARD1_VSEL       (1<<1)
#define        DUALSIM_CARD0_VCCEN      (1<<2)
#define        DUALSIM_CARD1_VCCEN      (1<<3)

#define        DUALSIM_CARD0_RSTSEL     (1<<0)
#define        DUALSIM_CARD1_RSTSEL     (1<<1)
#define        DUALSIM_CARD0_RSTVAL     (1<<2)
#define        DUALSIM_CARD1_RSTVAL     (1<<3)

#define        DUALSIM_CARD0_CPOL       (1<<0)
#define        DUALSIM_CARD1_CPOL       (1<<1)
#define        DUALSIM_CARD0_CPOH       (1<<2)
#define        DUALSIM_CARD1_CPOH       (1<<3)

#define        DUALSIM_CARD0_DATAEN     (1<<0)
#define        DUALSIM_CARD1_DATAEN     (1<<1)
#define        DUALSIM_CARD0_DATA_L     (1<<2)
#define        DUALSIM_CARD1_DATA_L     (1<<3)
#else
#define        DUALSIM_CARD1_VSEL       (1<<0)
#define        DUALSIM_CARD0_VSEL       (1<<1)
#define        DUALSIM_CARD1_VCCEN      (1<<2)
#define        DUALSIM_CARD0_VCCEN      (1<<3)

#define        DUALSIM_CARD1_RSTSEL     (1<<0)
#define        DUALSIM_CARD0_RSTSEL     (1<<1)
#define        DUALSIM_CARD1_RSTVAL     (1<<2)
#define        DUALSIM_CARD0_RSTVAL     (1<<3)

#define        DUALSIM_CARD1_CPOL       (1<<0)
#define        DUALSIM_CARD0_CPOL       (1<<1)
#define        DUALSIM_CARD1_CPOH       (1<<2)
#define        DUALSIM_CARD0_CPOH       (1<<3)

#define        DUALSIM_CARD1_DATAEN     (1<<0)
#define        DUALSIM_CARD0_DATAEN     (1<<1)
#define        DUALSIM_CARD1_DATA_L     (1<<2)
#define        DUALSIM_CARD0_DATA_L     (1<<3)
#endif

BOOL  g_threesimInitDone = FALSE;
BOOL  g_dualsimInitDone = FALSE;

enum  et6302_reg_map
{
    reset_ctrl_reg = 0,
    clock_ctrl_reg,
    data_ctrl_reg,
    vcc_ctrl_reg,
    ldo_ctrl_reg,
    bandgap_ctrl_reg,
};

#define TGT_THREESIM_CONFIG                  \
{                                                                   \
        HAL_SPI,                                                \
        HAL_SPI_CS0                                         \
}

/// Configuration structure for TARGET
PROTECTED CONST DUALSIM_CONFIG_T* g_dualsimConfig;
PROTECTED CONST DUALSIM_CONFIG_T g_threesimConfig = TGT_THREESIM_CONFIG;

extern DUALSIM_ERR_T pmd_DualSimSetVoltageClass(DUALSIM_SIM_VOLTAGE_T voltClass0,DUALSIM_SIM_VOLTAGE_T voltClass1);
extern BOOL pmd_SelectSimCard(UINT8 sim_card);
extern UINT16 pmd_OpalSpiRead(RDA_REG_MAP_T regIdx);
#define TEST_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }

PROTECTED FOURSIM_ERR_T  threesimsw_SpiActivateCs();
PROTECTED VOID threesimsw_SpiDeActivateCs();
PROTECTED VOID threesimsw_SetSim3VVoltage(DUALSIM_SIM_VOLTAGE_T level);
PROTECTED VOID threesimsw_SetSim1V8Voltage(DUALSIM_SIM_VOLTAGE_T level);
PROTECTED VOID threesimsw_SetSim0VVoltage(DUALSIM_SIM_VOLTAGE_T level);

//#if (NUMBER_OF_SIM==3)
// ======================================================================
// threesimsw_Open
// ----------------------------------------------------------------------
/// This function Open Spi for threesim
// ======================================================================
PUBLIC VOID threesimsw_Open(VOID)
{
    // Configure SPI
    HAL_SPI_CFG_T threesimsw_spiCfg =
    {
//        .enabledCS    = HAL_SPI_CS1,
        .csActiveLow    = TRUE,
        .inputEn        = FALSE,
        .clkFallEdge    = FALSE,
        .clkDelay       = HAL_SPI_HALF_CLK_PERIOD_1,
        .doDelay        = HAL_SPI_HALF_CLK_PERIOD_0,
        .diDelay        = HAL_SPI_HALF_CLK_PERIOD_2,
        .csDelay        = HAL_SPI_HALF_CLK_PERIOD_1,
        .csPulse        = HAL_SPI_HALF_CLK_PERIOD_0,
        .frameSize      = 8,
        .oeRatio        = 0,
        .spiFreq        = 500000,
        .rxTrigger      = HAL_SPI_RX_TRIGGER_4_BYTE,
        .txTrigger      = HAL_SPI_TX_TRIGGER_1_EMPTY,
        .rxMode         = HAL_SPI_DIRECT_POLLING,
        .txMode         = HAL_SPI_DIRECT_POLLING,
        .mask           = {0,0,0,0,0},
        .handler        = NULL
    };

    HAL_APO_ID_T gpio ;

    gpio.gpoId= HAL_GPO_3 ;

    // reset gpio_3
    hal_GpioClr(gpio);
    hal_TimDelay(160);
    hal_GpioSet(gpio);


    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_Open) ");


    EDRV_TRACE(EDRV_SIMD_TRC, 0, "g_threesimConfig spiBus =  %d, spiCs = %d", g_threesimConfig.spiBus, g_threesimConfig.spiCs);


    if(FALSE == g_threesimInitDone)
    {
        threesimsw_spiCfg.enabledCS = g_threesimConfig.spiCs;
        hal_SpiOpen(g_threesimConfig.spiBus,g_threesimConfig.spiCs,&threesimsw_spiCfg);

        g_threesimInitDone = TRUE;
    }


}

// ======================================================================
// threesimsw_Close
// ----------------------------------------------------------------------
/// This function Close threesim Spi
// ======================================================================

PUBLIC VOID  threesimsw_Close(VOID)
{
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_Close) ");

    if(TRUE == g_threesimInitDone)
    {
        hal_SpiClose(g_threesimConfig.spiBus, g_threesimConfig.spiCs);
        g_threesimInitDone=FALSE;
    }

}


// ======================================================================
// threesimsw_SpiActivateCs
// ----------------------------------------------------------------------
/// This function ActivateCs threesim Spi
// ======================================================================
PROTECTED FOURSIM_ERR_T  threesimsw_SpiActivateCs()
{
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SpiActivateCs) g_threesimInitDone = %d. g_threesimConfig.spiBus = %d, g_threesimConfig.spiCs = %d",
               g_threesimInitDone, g_threesimConfig.spiBus, g_threesimConfig.spiCs);


    if(FALSE == g_threesimInitDone)
    {
        return FOURSIM_ERR_NOT_INIT;
    }


    if (hal_SpiActivateCs(g_threesimConfig.spiBus, g_threesimConfig.spiCs))
    {
        return FOURSIM_ERR_NO;
    }
    else
    {
        return FOURSIM_ERR_RESOURCE_BUSY;
    }

}


//=============================================================================
// threesimsw_SpiDeActivateCs
//-----------------------------------------------------------------------------
/// Deactivate foursiml SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PROTECTED VOID threesimsw_SpiDeActivateCs()
{
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SpiDeActivateCs) ");

    if(TRUE == g_threesimInitDone)
    {
        hal_SpiDeActivateCs(g_threesimConfig.spiBus, g_threesimConfig.spiCs);
    }
}



// ======================================================================
// threesimsw_SendRegData
// ----------------------------------------------------------------------
/// This function send data to 6302 Reg
// ======================================================================
PROTECTED VOID threesimsw_SendRegData(enum et6302_reg_map regIdx, UINT8 data)
{
    u8 iTempData[3] = {0x0, 0x0, 0x0};
    u16 iCount = 0;

    EDRV_TRACE(EDRV_SIMD_TRC,0,"(threesimsw_SendRegData) Reg:%d Data:0x%02x",regIdx, data);

    iTempData[0] = (regIdx << 5) | data;

    hal_SpiSendData(g_threesimConfig.spiBus, g_threesimConfig.spiCs, iTempData, 1);

    //wait until any previous transfers have ended
    while(!hal_SpiTxFinished(g_threesimConfig.spiBus, g_threesimConfig.spiCs));

    for(iCount = 0; iCount < 800; iCount++);

}

// =======================================================================
// threesimsw_SetSim3VVoltage
// -----------------------------------------------------------------------
/// This function set sim1 Voltage and set sim0 Voltage 3V
// =======================================================================
PROTECTED VOID threesimsw_SetSim3VVoltage(DUALSIM_SIM_VOLTAGE_T level)
{

    UINT8  RegConfig;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SetSim3VVoltage)");

    switch (level)
    {
        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_NULL:
            RegConfig=DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;
    }
}


// ===================================================================
// threesimsw_SetSim1V8Voltage
// -------------------------------------------------------------------
/// This function set sim1 Voltage and set sim0 Voltage 1.8V
// ===================================================================

PROTECTED VOID threesimsw_SetSim1V8Voltage(DUALSIM_SIM_VOLTAGE_T level)
{
    UINT8  RegConfig;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SetSim1V8Voltage) ");

    switch (level)
    {

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD0_VCCEN;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_NULL:
            RegConfig=DUALSIM_CARD0_VCCEN;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;
    }

}


// ====================================================================
// threesimsw_SetSim0VVoltage
// --------------------------------------------------------------------
/// This function set sim1 Voltage and set sim0 Voltage 0V
// ====================================================================

PROTECTED VOID threesimsw_SetSim0VVoltage(DUALSIM_SIM_VOLTAGE_T level)
{
    UINT8  RegConfig;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SetSim0VVoltage)");

    switch (level)
    {
        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            RegConfig=DUALSIM_CARD1_VCCEN;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;

        case DUALSIM_SIM_VOLTAGE_NULL:
            RegConfig=0x00;
            threesimsw_SendRegData(vcc_ctrl_reg,RegConfig);
            break;
    }

}



// ==========================================================================
// threesimsw_SetSimVoltage
// --------------------------------------------------------------------------
/// This function set sim0 and sim1 Voltage
// ==========================================================================

PUBLIC DUALSIM_ERR_T  threesimsw_SetSimVoltage(DUALSIM_SIM_VOLTAGE_T sim0Level,DUALSIM_SIM_VOLTAGE_T sim1Level)
{
    UINT32 delay;
    DUALSIM_ERR_T  errStatus;
    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)  //2.8V
    errStatus = threesimsw_SpiActivateCs();

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SetSimVoltage)  sim0Level = %d, sim1Level = %d, spi Status = %d", sim0Level, sim1Level, errStatus);

    if(DUALSIM_ERR_NO==errStatus)
    {
        delay = DS_SETVOLT_DLY;
        switch (sim0Level)
        {
            case DUALSIM_SIM_VOLTAGE_CLASS_A:
                threesimsw_SetSim3VVoltage(sim1Level);
                break;

            case DUALSIM_SIM_VOLTAGE_CLASS_B:
                threesimsw_SetSim3VVoltage(sim1Level);
                break;

            case DUALSIM_SIM_VOLTAGE_CLASS_C:
                threesimsw_SetSim1V8Voltage(sim1Level);
                break;

            case DUALSIM_SIM_VOLTAGE_NULL:
                threesimsw_SetSim0VVoltage(sim1Level);
                break;
        }


        threesimsw_SpiDeActivateCs();
        hal_TimDelay(delay MS_WAITING);

    }

    return errStatus;

}


// ==========================================================================
// foursim_SetSimVoltage
// --------------------------------------------------------------------------
/// This function set sim0 and sim1 Voltage
// ==========================================================================

PUBLIC DUALSIM_ERR_T  threesimsw_SetGalliteVoltage(DUALSIM_SIM_VOLTAGE_T sim0Level,DUALSIM_SIM_VOLTAGE_T sim1Level)
{
    UINT32 delay;
    DUALSIM_ERR_T  errStatus;
    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)  //2.8V

#ifdef DUALSWITCH_SWAP
    DUALSIM_SIM_VOLTAGE_T temp;
    temp = sim1Level;
    sim1Level = sim0Level;
    sim0Level = temp;
#endif // DUALSIM_SWAP

    errStatus = pmd_DualSimSetVoltageClass(sim0Level, sim1Level);

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SetGalliteVoltage) sim0Level= %d, sim1Level= %d, errStatus = %d", sim0Level, sim1Level, errStatus);

    if (errStatus == DUALSIM_ERR_NO)
    {
        delay = DS_SETVOLT_DLY;
        hal_TimDelay(delay MS_WAITING);
    }

    return errStatus;
}



// ======================================================================
// threesimsw_SelectSwitch
// ----------------------------------------------------------------------
/// This function select the special 6302 switch.
//   parameter: number =0: select switch 0
//             number =1: select switch 1
// ======================================================================
PUBLIC FOURSIM_ERR_T  threesimsw_SelectSwitch(FOURSIM_SWITCH_T number)
{
    UINT32  delay;

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(threesimsw_SelectSwitch) switch ID= %d", number);

    if ((number != FOURSIM_SWITCH_0) &&  (number != FOURSIM_SWITCH_1))
    {
        return FOURSIM_ERR_NO;
    }

#ifdef FOURSIM_SWITCH_SWAP
    if (number == FOURSIM_SWITCH_0)
    {
        number = FOURSIM_SWITCH_1;
    }
    else if (number == FOURSIM_SWITCH_1)
    {
        number = FOURSIM_SWITCH_0;
    }
#endif

    if (pmd_SelectSimCard(number))
    {
        EDRV_TRACE(EDRV_SIMD_TRC, 0, "pmd_SelectSimCard  DONE");

        delay = DS_SETSIMEN_DLY;
        hal_TimDelay(delay MS_WAITING);

        return FOURSIM_ERR_NO;
    }

    return FOURSIM_ERR_RESOURCE_BUSY;
}

// ======================================================================
// dualsim_SetSimEnable
// ----------------------------------------------------------------------
/// This function set sim0 or  sim1 Enalbe
// ======================================================================
//            CARD0    CARD1
// RSTREG     1010     0101
// CLKREG     0001     0010
// DATAREG    0001     0010
// ======================================================================
PUBLIC FOURSIM_ERR_T  threesimsw_SetSimEnable(DUALSIM_SIM_CARD_T number)
{
    UINT32 delay;
    FOURSIM_ERR_T  errStatus;

    errStatus = threesimsw_SpiActivateCs();

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursim_SetSimEnable) sim card ID =  %d", number);
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "spi active state =  %d", errStatus);

    if(FOURSIM_ERR_NO==errStatus)
    {
        delay = DS_SETSIMEN_DLY;
        switch (number)
        {
            case FOURSIM_SIM_SELECT_CARD_0:
                threesimsw_SendRegData(reset_ctrl_reg,DUALSIM_CARD1_RSTSEL|DUALSIM_CARD1_RSTVAL);
                threesimsw_SendRegData(clock_ctrl_reg,DUALSIM_CARD0_CPOL|DUALSIM_CARD1_CPOL);
                threesimsw_SendRegData(data_ctrl_reg,DUALSIM_CARD0_DATAEN);
                break;

            case FOURSIM_SIM_SELECT_CARD_1:
                threesimsw_SendRegData(reset_ctrl_reg,DUALSIM_CARD0_RSTSEL|DUALSIM_CARD0_RSTVAL);
                threesimsw_SendRegData(clock_ctrl_reg,DUALSIM_CARD1_CPOL|DUALSIM_CARD0_CPOL);
                threesimsw_SendRegData(data_ctrl_reg,DUALSIM_CARD1_DATAEN);
                break;

            case FOURSIM_SIM_SELECT_NULL:
                threesimsw_SendRegData(reset_ctrl_reg,0x0F);
                threesimsw_SendRegData(clock_ctrl_reg,0x00);
                threesimsw_SendRegData(data_ctrl_reg,0x00);
                break;

            default:
                break;
        }

        threesimsw_SpiDeActivateCs();
        hal_TimDelay(delay MS_WAITING);

    }

    return errStatus;

}



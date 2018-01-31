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

BOOL  g_foursimInitDone = FALSE;
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

#define TGT_FOURSIM_CONFIG1                  \
{                                                                   \
        HAL_SPI,                                                \
        HAL_SPI_CS0                                         \
}

#define TGT_FOURSIM_CONFIG2                  \
{                                                                   \
        HAL_SPI,                                                \
        HAL_SPI_CS1                                         \
}


/// Configuration structure for TARGET
PROTECTED CONST DUALSIM_CONFIG_T* g_dualsimConfig;
PROTECTED CONST DUALSIM_CONFIG_T g_foursimConfig1 = TGT_FOURSIM_CONFIG1;
PROTECTED CONST DUALSIM_CONFIG_T g_foursimConfig2 = TGT_FOURSIM_CONFIG2;

extern DUALSIM_ERR_T pmd_DualSimSetVoltageClass(DUALSIM_SIM_VOLTAGE_T voltClass0,DUALSIM_SIM_VOLTAGE_T voltClass1);
extern BOOL pmd_SelectSimCard(UINT8 sim_card);
extern UINT16 pmd_OpalSpiRead(RDA_REG_MAP_T regIdx);
#define TEST_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }

PROTECTED FOURSIM_ERR_T  foursimsw_SpiActivateCs(UINT8 CsIdx);
PROTECTED VOID foursimsw_SpiDeActivateCs(UINT8 CsIdx);

// ======================================================================
// foursimsw_Open
// ----------------------------------------------------------------------
/// This function Open Spi for foursim
// ======================================================================
PUBLIC VOID foursimsw_Open(VOID)
{
    // Configure SPI
    HAL_SPI_CFG_T foursimsw_spiCfg =
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

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_Open) ");

//    g_foursimConfig = tgt_GetDualsimConfig();
//    g_foursimConfig1 = tgt_GetFoursimConfig1();   //SPI_CS0
//    g_foursimConfig2 = tgt_GetFoursimConfig2();   //SPI_CS1


    EDRV_TRACE(EDRV_SIMD_TRC, 0, "g_foursimConfig1 spiBus =  %d, spiCs = %d", g_foursimConfig1.spiBus, g_foursimConfig1.spiCs);
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "g_foursimConfig2 spiBus =  %d, spiCs = %d", g_foursimConfig2.spiBus, g_foursimConfig2.spiCs);


    if(FALSE==g_foursimInitDone)
    {
        foursimsw_spiCfg.enabledCS = g_foursimConfig1.spiCs;
        hal_SpiOpen(g_foursimConfig1.spiBus,g_foursimConfig1.spiCs,&foursimsw_spiCfg);

#if (NUMBER_OF_SIM > 2)
        foursimsw_spiCfg.enabledCS = g_foursimConfig2.spiCs;
        hal_SpiOpen(g_foursimConfig2.spiBus,g_foursimConfig2.spiCs,&foursimsw_spiCfg);
#endif

        g_foursimInitDone = TRUE;
    }


}

// ======================================================================
// foursimsw_Close
// ----------------------------------------------------------------------
/// This function Close foursim Spi
// ======================================================================

PUBLIC VOID  foursimsw_Close(VOID)
{
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_Close) ");

    if(TRUE==g_foursimInitDone)
    {
        hal_SpiClose(g_foursimConfig1.spiBus,g_foursimConfig1.spiCs);

#if (NUMBER_OF_SIM > 2)
        hal_SpiClose(g_foursimConfig2.spiBus,g_foursimConfig2.spiCs);
#endif

        g_foursimInitDone=FALSE;
    }

}


// ======================================================================
// foursimsw_SpiActivateCs
// ----------------------------------------------------------------------
/// This function ActivateCs foursim Spi
// ======================================================================
PROTECTED FOURSIM_ERR_T  foursimsw_SpiActivateCs(UINT8 CsIdx)
{
    if(CsIdx == 0)
        EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SpiActivateCs) g_foursimInitDone = %d. g_foursimConfig1.spiBus = %d, g_foursimConfig1.spiCs = %d",
                   g_foursimInitDone, g_foursimConfig1.spiBus, g_foursimConfig1.spiCs);
    else
        EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SpiActivateCs) g_foursimInitDone = %d. g_foursimConfig2.spiBus = %d, g_foursimConfig2.spiCs = %d",
                   g_foursimInitDone, g_foursimConfig2.spiBus, g_foursimConfig2.spiCs);


    if(FALSE==g_foursimInitDone)
    {
        return FOURSIM_ERR_NOT_INIT;
    }

    if(CsIdx == 0)
    {
        if (hal_SpiActivateCs(g_foursimConfig1.spiBus,g_foursimConfig1.spiCs))
        {
            return FOURSIM_ERR_NO;
        }
        else
        {
            return FOURSIM_ERR_RESOURCE_BUSY;
        }
    }
    else
    {
        if (hal_SpiActivateCs(g_foursimConfig2.spiBus,g_foursimConfig2.spiCs))
        {
            return FOURSIM_ERR_NO;
        }
        else
        {
            return FOURSIM_ERR_RESOURCE_BUSY;
        }
    }

}


//=============================================================================
// foursimsw_SpiDeActivateCs
//-----------------------------------------------------------------------------
/// Deactivate foursiml SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PROTECTED VOID foursimsw_SpiDeActivateCs(UINT8 CsIdx)
{
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SpiDeActivateCs) ");

    if(TRUE==g_foursimInitDone)
    {
        if(CsIdx == 0)
            hal_SpiDeActivateCs(g_foursimConfig1.spiBus,g_foursimConfig1.spiCs);
        if(CsIdx == 1)
            hal_SpiDeActivateCs(g_foursimConfig2.spiBus,g_foursimConfig2.spiCs);
    }
}



// ======================================================================
// fourlsim_SendRegData
// ----------------------------------------------------------------------
/// This function send data to 6302 Reg
// ======================================================================
PROTECTED VOID foursimsw_SendRegData(enum et6302_reg_map regIdx, UINT8 data, UINT8 CsIdx)
{

    u8 iTempData[3] = {0x0, 0x0, 0x0};
//    u8 OTempData[3] = {0x0, 0x0, 0x0};

    u16 iCount = 0;

    EDRV_TRACE(EDRV_SIMD_TRC,0,"(foursimsw_SendRegData) Reg:%d Data:0x%02x",regIdx, data);

    iTempData[0] = (regIdx << 5) | data;

    if(CsIdx == 0)
        hal_SpiSendData(g_foursimConfig1.spiBus,g_foursimConfig1.spiCs,iTempData, 1);
    if(CsIdx == 1)
        hal_SpiSendData(g_foursimConfig2.spiBus,g_foursimConfig2.spiCs,iTempData, 1);

    //wait until any previous transfers have ended
    if(CsIdx == 0)
        while(!hal_SpiTxFinished(g_foursimConfig1.spiBus,g_foursimConfig1.spiCs));
    if(CsIdx == 1)
        while(!hal_SpiTxFinished(g_foursimConfig2.spiBus,g_foursimConfig2.spiCs));

    for(iCount = 0; iCount < 800; iCount++);

//    Drv_OpalSpiRead(regIdx);


}

// =======================================================================
// dualsim_SetSim3VVoltage
// -----------------------------------------------------------------------
/// This function set sim1 Voltage and set sim0 Voltage 3V
// =======================================================================
PROTECTED VOID foursimsw_SetSim3VVoltage(DUALSIM_SIM_VOLTAGE_T level, UINT8 CsIdx)
{

    UINT8  RegConfig;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SetSim3VVoltage), SPICS = %d ", CsIdx);

    switch (level)
    {
        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_NULL:
            RegConfig=DUALSIM_CARD0_VCCEN|DUALSIM_CARD0_VSEL;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;
    }
}


// ===================================================================
// dualsim_SetSim1V8Voltage
// -------------------------------------------------------------------
/// This function set sim1 Voltage and set sim0 Voltage 1.8V
// ===================================================================

PROTECTED VOID foursimsw_SetSim1V8Voltage(DUALSIM_SIM_VOLTAGE_T level, UINT8 CsIdx)
{
    UINT8  RegConfig;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SetSim1V8Voltage) SPICS = %d", CsIdx);

    switch (level)
    {

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL|DUALSIM_CARD0_VCCEN;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD0_VCCEN;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_NULL:
            RegConfig=DUALSIM_CARD0_VCCEN;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;
    }

}


// ====================================================================
// dualsim_SetSim0VVoltage
// --------------------------------------------------------------------
/// This function set sim1 Voltage and set sim0 Voltage 0V
// ====================================================================

PROTECTED VOID foursimsw_SetSim0VVoltage(DUALSIM_SIM_VOLTAGE_T level, UINT8 CsIdx)
{
    UINT8  RegConfig;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SetSim0VVoltage), SPICS = %d", CsIdx);

    switch (level)
    {
        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            RegConfig=DUALSIM_CARD1_VCCEN|DUALSIM_CARD1_VSEL;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            RegConfig=DUALSIM_CARD1_VCCEN;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;

        case DUALSIM_SIM_VOLTAGE_NULL:
            RegConfig=0x00;
            foursimsw_SendRegData(vcc_ctrl_reg,RegConfig, CsIdx);
            break;
    }

}



// ==========================================================================
// foursimsw_SetSimVoltage
// --------------------------------------------------------------------------
/// This function set sim0 and sim1 Voltage
// ==========================================================================

PUBLIC DUALSIM_ERR_T  foursimsw_SetSimVoltage(DUALSIM_SIM_VOLTAGE_T sim0Level,DUALSIM_SIM_VOLTAGE_T sim1Level, UINT8 CsIdx)
{
    UINT32 delay;
    DUALSIM_ERR_T  errStatus;
    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)  //2.8V
    errStatus = foursimsw_SpiActivateCs(CsIdx);

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SetSimVoltage)  sim0Level = %d, sim1Level = %d, spi Status = %d, SPICS = %d", sim0Level, sim1Level, errStatus, CsIdx);

    if(DUALSIM_ERR_NO==errStatus)
    {
        delay = DS_SETVOLT_DLY;
        switch (sim0Level)
        {
            case DUALSIM_SIM_VOLTAGE_CLASS_A:
                foursimsw_SetSim3VVoltage(sim1Level, CsIdx);
                break;

            case DUALSIM_SIM_VOLTAGE_CLASS_B:
                foursimsw_SetSim3VVoltage(sim1Level, CsIdx);
                break;

            case DUALSIM_SIM_VOLTAGE_CLASS_C:
                foursimsw_SetSim1V8Voltage(sim1Level, CsIdx);
                break;

            case DUALSIM_SIM_VOLTAGE_NULL:
                foursimsw_SetSim0VVoltage(sim1Level, CsIdx);
                break;
        }


        foursimsw_SpiDeActivateCs(CsIdx);
        hal_TimDelay(delay MS_WAITING);

    }

    return errStatus;

}


// ==========================================================================
// foursimsw_SetSimVoltage
// --------------------------------------------------------------------------
/// This function set sim0 and sim1 Voltage
// ==========================================================================

PUBLIC DUALSIM_ERR_T  foursimsw_SetGalliteVoltage(DUALSIM_SIM_VOLTAGE_T sim0Level,DUALSIM_SIM_VOLTAGE_T sim1Level)
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

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SetGalliteVoltage) sim0Level= %d, sim1Level= %d, errStatus = %d", sim0Level, sim1Level, errStatus);

    if (errStatus == DUALSIM_ERR_NO)
    {
        delay = DS_SETVOLT_DLY;
        hal_TimDelay(delay MS_WAITING);
    }

    return errStatus;
}



// ======================================================================
// foursimsw_SelectSwitch
// ----------------------------------------------------------------------
/// This function select the special 6302 switch.
//   parameter: number =0: select switch 0
//             number =1: select switch 1
// ======================================================================
PUBLIC FOURSIM_ERR_T  foursimsw_SelectSwitch(FOURSIM_SWITCH_T number)
{
    UINT32  delay;

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SelectSwitch) switch ID= %d", number);

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
PUBLIC FOURSIM_ERR_T  foursimsw_SetSimEnable(DUALSIM_SIM_CARD_T number, UINT8 CsIdx)
{
    UINT32 delay;
    FOURSIM_ERR_T  errStatus;

    errStatus=foursimsw_SpiActivateCs(CsIdx);

    EDRV_TRACE(EDRV_SIMD_TRC, 0, "(foursimsw_SetSimEnable) sim card ID =  %d, SPICS = %d", number, CsIdx);
    EDRV_TRACE(EDRV_SIMD_TRC, 0, "spi active state =  %d", errStatus);

    if(FOURSIM_ERR_NO==errStatus)
    {
        delay = DS_SETSIMEN_DLY;
        switch (number)
        {
            case FOURSIM_SIM_SELECT_CARD_0:
                foursimsw_SendRegData(reset_ctrl_reg,DUALSIM_CARD1_RSTSEL|DUALSIM_CARD1_RSTVAL, CsIdx);
                foursimsw_SendRegData(clock_ctrl_reg,DUALSIM_CARD0_CPOL|DUALSIM_CARD1_CPOL, CsIdx);
                foursimsw_SendRegData(data_ctrl_reg,DUALSIM_CARD0_DATAEN, CsIdx);
                break;

            case FOURSIM_SIM_SELECT_CARD_1:
                foursimsw_SendRegData(reset_ctrl_reg,DUALSIM_CARD0_RSTSEL|DUALSIM_CARD0_RSTVAL, CsIdx);
                foursimsw_SendRegData(clock_ctrl_reg,DUALSIM_CARD1_CPOL|DUALSIM_CARD0_CPOL, CsIdx);
                foursimsw_SendRegData(data_ctrl_reg,DUALSIM_CARD1_DATAEN, CsIdx);
                break;

            case FOURSIM_SIM_SELECT_NULL:
                foursimsw_SendRegData(reset_ctrl_reg,0x0F, CsIdx);
                foursimsw_SendRegData(clock_ctrl_reg,0x00, CsIdx);
                foursimsw_SendRegData(data_ctrl_reg,0x00, CsIdx);
                break;

            default:
                break;
        }

        foursimsw_SpiDeActivateCs(CsIdx);
        hal_TimDelay(delay MS_WAITING);

    }

    return errStatus;

}



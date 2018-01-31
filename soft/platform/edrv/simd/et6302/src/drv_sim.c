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




#include "hal_sys.h"
#include "stdlib.h"
#include "sxs_io.h"
#include "hal_spi.h"

#include "hal_timers.h"
#include "hal_sim.h"
#include "simd_config.h"
#include "simdp_debug.h"

#include "simd_m.h"
#if 0
#include "globals.h" // for XHALT
#include "global_macros.h" // for XHALT
#endif

#define        DS_NULLVOLT_DLY          (20) // delay in 1/1000 sec
#define        DS_SETVOLT_DLY           (50) // delay in 1/1000 sec

#define        DS_SETCLK_DLY            (1)
#define        DS_SETDATA_DLY           (1)
#define        DS_SETRST_DLY            (1)
#define        DS_SETSIMEN_DLY          (1)

#define        SIM_CARD_VSEL        (1<<0)
#define        SIM_CARD_VCCEN       (1<<2)

#define        SIM_CARD_CPOL        (1<<0)
#define        SIM_CARD_CPOH        (1<<2)

#define        SIM_CARD_DATAEN      (1<<0)
#define        SIM_CARD_DATA_L      (1<<2)

#define        SIM_CARD_RSTSEL      (1<<0)
#define        SIM_CARD_RSTVAL      (1<<2)

#define        SIM_CARD0_VSEL       (1<<0)
#define        SIM_CARD1_VSEL       (1<<1)
#define        SIM_CARD0_VCCEN      (1<<2)
#define        SIM_CARD1_VCCEN      (1<<3)

#define        SIM_CARD0_RSTSEL     (1<<0)
#define        SIM_CARD1_RSTSEL     (1<<1)
#define        SIM_CARD0_RSTVAL     (1<<2)
#define        SIM_CARD1_RSTVAL     (1<<3)

#define        SIM_CARD0_CPOL       (1<<0)
#define        SIM_CARD1_CPOL       (1<<1)
#define        SIM_CARD0_CPOH       (1<<2)
#define        SIM_CARD1_CPOH       (1<<3)

#define        SIM_CARD0_DATAEN     (1<<0)
#define        SIM_CARD1_DATAEN     (1<<1)
#define        SIM_CARD0_DATA_L     (1<<2)
#define        SIM_CARD1_DATA_L     (1<<3)

#define MAX_ITF 2   // Maximum number of SIMs supported by the telephone

UINT8 g_simd_SpiOpen = 0;

/// These are the interface-specific configuration values that should be stored
/// for each interface.
/// Voltage and ClockStop are stored _with_ the corresponding shift for the
/// particular interface number so they can be used directly when writing to the
/// register without further shifting.
typedef struct
{
    UINT32                  Config;
    UINT32                  ClkDiv;
    UINT32                  Times;
    UINT32                  ChFilt;
    UINT32                  IntMask;
    UINT8                   Convention; // 1 for Inverse Convention, 0 for Direct Convention
    UINT8                   ResetCfg;   // 1 for Reset Active High, 0 for Reset Active Low (pre-shifted)
    UINT8                   Voltage;    // Voltage configuration for the card (pre-shifted)
    UINT8                   ClockStop;  // Configuration for clock stop mode (pre-shifted)
} SIMD_SCI_CONFIG_T;

typedef struct
{
    SIMD_CARD_T             ActIf;
    SIMD_SCI_CONFIG_T       SciConfig[MAX_ITF];
    CONST SIMD_CONFIG_T*    SpiConfig;
} SIMD_INTERFACE_T;

PRIVATE SIMD_INTERFACE_T g_simConfig;

enum  et6302_reg_map
{
    reset_ctrl_reg = 0,
    clock_ctrl_reg,
    data_ctrl_reg,
    vcc_ctrl_reg,
    ldo_ctrl_reg,
    bandgap_ctrl_reg,
};



// ======================================================================
// simd_SendRegData
// ----------------------------------------------------------------------
/// This function send data to 6302 Reg
// ======================================================================


PROTECTED VOID simd_SendRegData(enum et6302_reg_map regIdx, UINT8 data)
{
    HAL_SPI_CFG_T simd_spiCfg =
    {
        //.enabledCS    = HAL_SPI_CS2,
        .csActiveLow    = FALSE,
        .inputEn        = FALSE,
        .clkFallEdge    = FALSE,
        .clkDelay       = HAL_SPI_HALF_CLK_PERIOD_1,
        .doDelay        = HAL_SPI_HALF_CLK_PERIOD_0,
        .diDelay        = HAL_SPI_HALF_CLK_PERIOD_2,
        .csDelay        = HAL_SPI_HALF_CLK_PERIOD_0,
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

    u8 iTempData[3] = {0x0, 0x0, 0x0};
    u16 iCount = 0;

    simd_spiCfg.enabledCS = g_simConfig.SpiConfig->spiCs;
    ++g_simd_SpiOpen;
    if (g_simd_SpiOpen != 1)
    {
        // TODO: should use assert
#ifdef XHALT
        XHALT;
#endif
    }
    hal_SpiOpen(g_simConfig.SpiConfig->spiBus,g_simConfig.SpiConfig->spiCs,&simd_spiCfg);

    iTempData[0] = (regIdx << 5) | data;

    hal_SpiActivateCs(g_simConfig.SpiConfig->spiBus,g_simConfig.SpiConfig->spiCs);
    hal_SpiSendData(g_simConfig.SpiConfig->spiBus,g_simConfig.SpiConfig->spiCs,iTempData, 1);

    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMIF:%d simd_Send Reg:%d Data:0x%02x",g_simConfig.ActIf, regIdx, data);

    //wait until any previous transfers have ended
    while(!hal_SpiTxFinished(g_simConfig.SpiConfig->spiBus,g_simConfig.SpiConfig->spiCs));
    for(iCount = 0; iCount < 800; iCount++);
    hal_SpiDeActivateCs(g_simConfig.SpiConfig->spiBus,g_simConfig.SpiConfig->spiCs);

    for(iCount = 0; iCount < 800; iCount++);
    --g_simd_SpiOpen;
    hal_SpiClose(g_simConfig.SpiConfig->spiBus,g_simConfig.SpiConfig->spiCs);
}



// ============================================================================
// ============================================================================
// ============================================================================
// ============================================================================
// ============================================================================




// ============================================================================
// SIMD_OPEN
// ----------------------------------------------------------------------------
/// Saves the simConfig structure in an internal global variable.  This does
/// nothing on the interface.
/// @param simdConfig this is the tgt_GetSimConfig() return value
/// This function is interface INDEPENDENT.
// ============================================================================
// ***
PUBLIC VOID simd_Open(CONST SIMD_CONFIG_T* simdConfig)
{
    UINT8 Index;

    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_Open");
    g_simConfig.SpiConfig = simdConfig;
    g_simConfig.ActIf = 0xff;

    hal_SimOpen();
    //hal_SimPowerDown();

    // Copy the reset value of the SIM Card Interface module
    // to each local interface configuration
    for (Index = 0; Index < MAX_ITF; ++Index)
    {
        g_simConfig.SciConfig[Index].Config = hal_SimGetRegConfig();
        g_simConfig.SciConfig[Index].ClkDiv = hal_SimGetRegClkDiv();
        g_simConfig.SciConfig[Index].Times = hal_SimGetRegTimes();
        g_simConfig.SciConfig[Index].ChFilt = hal_SimGetRegChFilt();
        g_simConfig.SciConfig[Index].IntMask = hal_SimGetRegIntMask();
        // The Voltage value is the bit configuration in the register, not the enum
        g_simConfig.SciConfig[Index].Voltage = 0;
        // The Reset value is the bit configuration in the register, drive 0 as default
        g_simConfig.SciConfig[Index].ResetCfg = SIM_CARD_RSTSEL << Index;
        // By default, use the reset value of the regular SIM interface
        g_simConfig.SciConfig[Index].ClockStop = 0;
    }
    SIMD_TRACE(SIMD_DBG_TRC | TSTDOUT,0,"SIMD: simd_Open Config [0]:0x%08x [1]:0x%08x", g_simConfig.SciConfig[0].Config, g_simConfig.SciConfig[1].Config);
}

// ============================================================================
// simd_Close
// ----------------------------------------------------------------------------
/// Close the SIM driver and disables the use of its API.
/// simd_PowerDown should be called before this function for _all_ active
/// interfaces.
/// This function is interface INDEPENDENT.
// ============================================================================
// ***
PUBLIC VOID simd_Close()
{
    // TODO: Check to make sure no active interfaces otherwise Assert??
    hal_SimClose();
}

// ============================================================================
// simd_DelayAfterEpps
// --------------------------------------------------------------------------
/// Delay after EPPS procedure required for some SIM cards.  This does not
/// require the active card as it's simply an interface independent delay.
/// This function is interface INDEPENDENT.
// ============================================================================
// ***
PUBLIC VOID simd_DelayAfterEpps()
{
    hal_SimDelayAfterEpps();
}

// ============================================================================
// simd_SetSimEnable
// ----------------------------------------------------------------------------
/// This functions sets the active interface.  This should be called before any
/// other interface dependent functions are called with this interface value.
/// If an interface dependent function is called with an interface number
/// other than the active interface an assert is generated.
/// The SIM Card Interface registers will be reconfigured with the saved
/// interface configuration.
/// Before switching interfaces, the function checks that the clock has
/// stopped.  If the clock has stopped, it configures the SIM driver
/// to set the clock to the correct level before switching the driver.
/// ***NOTE***:  If any of the cards does not support clock stop mode,
/// the multiple card management becomes _extremely_ complicated and
/// probably not something we would support.
/// @param ItfNum is the index number of the interface to be activated.  Each
/// simd driver contains the maximum number of interfaces it supports and if
/// this value is greater than MAX_ITF defined in the driver, an assert is
/// generated.
/// @return If the clock of the current active interface has not stopped,
/// this function will return FALSE.  Otherwise, the function will switch
/// inhterfaces and return TRUE.
// ============================================================================
// ***
PUBLIC BOOL simd_SetSimEnable(SIMD_CARD_T ItfNum)
{
    UINT8 RegWrVal;
    UINT8 Index;

    SIMD_ASSERT(ItfNum < MAX_ITF, "SIMD trying to enable non-existent interface! %d \n", ItfNum);
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_SetSimEnable %d",ItfNum);

    // If the interface is already active, return TRUE
    if (ItfNum == g_simConfig.ActIf)
    {
        return TRUE;
    }

    // Check if the interface is still active (detected by the SCI clock still on)
    if (g_simConfig.ActIf!=0xff)
    {
        if (g_simConfig.SciConfig[g_simConfig.ActIf].Voltage != 0)
        {
            // If the active interface voltage is zero, it probably means that
            // the ATR failed, so it's ok to disable the interface.  Otherwise,
            // the interface is probably valid, so wait until the clock has stopped.
            if (hal_SimInterfaceActive())
            {
                SIMD_TRACE(SIMD_INFO_TRC,0,"SIMD: Switch FAILED.  Interface still active.");
                return FALSE;
            }
            else
            {
                SIMD_TRACE(SIMD_INFO_TRC,0,"SIMD: Switch OK");
            }
        }
    }

#if 1
    // If the active interface is a valid interface number (not 0xff), save the current SIM module config
    // registers in the g_simConfig structure.
    if (g_simConfig.ActIf != 0xff)
    {
        SIMD_TRACE(SIMD_INFO_TRC,0,"SIMD: Config [%d]:0x%08x reg:0x%08x [%d]:0x%08x",
                   g_simConfig.ActIf,
                   g_simConfig.SciConfig[g_simConfig.ActIf].Config,
                   hal_SimGetRegConfig(),
                   ItfNum,
                   g_simConfig.SciConfig[ItfNum].Config);
        if (g_simConfig.SciConfig[g_simConfig.ActIf].Config != hal_SimGetRegConfig())
        {
            SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: forced update: Config");
            g_simConfig.SciConfig[g_simConfig.ActIf].Config = hal_SimGetRegConfig();
        }
        if (g_simConfig.SciConfig[g_simConfig.ActIf].ClkDiv != hal_SimGetRegClkDiv())
        {
            SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: forced update: ClkDiv");
            g_simConfig.SciConfig[g_simConfig.ActIf].ClkDiv = hal_SimGetRegClkDiv();
        }
        if (g_simConfig.SciConfig[g_simConfig.ActIf].Times != hal_SimGetRegTimes())
        {
            SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: forced update: Times");
            g_simConfig.SciConfig[g_simConfig.ActIf].Times = hal_SimGetRegTimes();
        }
        if (g_simConfig.SciConfig[g_simConfig.ActIf].ChFilt != hal_SimGetRegChFilt())
        {
            SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: forced update: ChFilt");
            g_simConfig.SciConfig[g_simConfig.ActIf].ChFilt = hal_SimGetRegChFilt();
        }
        if (g_simConfig.SciConfig[g_simConfig.ActIf].IntMask != hal_SimGetRegIntMask())
        {
            SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: forced update: IntMask");
            g_simConfig.SciConfig[g_simConfig.ActIf].IntMask = hal_SimGetRegIntMask();
        }
    }
#endif

    // Disconnect clock of active interface and use saved MT6302 config.  Don't activate
    // new interface yet.
    RegWrVal = (g_simConfig.SciConfig[0].ClockStop) | (g_simConfig.SciConfig[1].ClockStop );
    simd_SendRegData(clock_ctrl_reg,RegWrVal);
    // Disconnect data line of active interface.  Don't activate new interface yet. 0 means disconnected
    RegWrVal = 0;
    simd_SendRegData(data_ctrl_reg,RegWrVal);

    // Reprogram SIM Card Interface registers
    hal_SimRestoreRegConfig(g_simConfig.SciConfig[ItfNum].Config);
    hal_SimRestoreRegClkDiv(g_simConfig.SciConfig[ItfNum].ClkDiv);
    hal_SimRestoreRegTimes(g_simConfig.SciConfig[ItfNum].Times);
    hal_SimRestoreRegChFilt(g_simConfig.SciConfig[ItfNum].ChFilt);
    hal_SimRestoreRegIntMask(g_simConfig.SciConfig[ItfNum].IntMask);

    //
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: RestoreReg Config:0x%08x ClkDiv:0x%08x Times:0x%08x ChFilt:0x%08x IntMask:0x%08x",
               g_simConfig.SciConfig[ItfNum].Config,
               g_simConfig.SciConfig[ItfNum].ClkDiv,
               g_simConfig.SciConfig[ItfNum].Times,
               g_simConfig.SciConfig[ItfNum].ChFilt,
               g_simConfig.SciConfig[ItfNum].IntMask);

    // Switch on the new interface of the MT6302
    // Only need to do Clock and Data.  Reset should be set once and shouldn't need passthrough
    // unless a new reset is required.
    /// Set Active Interface to passthrough and use MT6302 config for non-active interface
    RegWrVal = 0;
    for (Index = 0; Index < MAX_ITF; ++Index)
    {
        if (Index == ItfNum)
        {
            // Passthrough config for active IF
            RegWrVal |= (SIM_CARD_CPOL<<ItfNum);
        }
        else
        {
            // Saved MT6302 config for the rest...
            RegWrVal |= (g_simConfig.SciConfig[Index].ClockStop);
        }
    }
    simd_SendRegData(clock_ctrl_reg,RegWrVal);

    // Enable passthrough for active interface
    simd_SendRegData(data_ctrl_reg,(SIM_CARD_DATAEN)<<ItfNum);

    g_simConfig.ActIf = ItfNum;

    hal_TimDelay(DS_SETSIMEN_DLY MS_WAITING);
    return TRUE;
}


// ============================================================================
// ============================================================================
/// These are the Interface DEPENDENT functions of the driver
/// The sim_SetSimEnable function MUST be called to set the active SIM
/// interface and all subsequent calls to interface dependent functions will
/// be performed on that interface.  It is the responsibility of the calling
/// layer to manage the two cards 'simultaneously'.  An interface may _only_
/// be accessed after being activated by the simd_SetSimEnable function.  No
/// checking is done at this level (although it may be implemented later if
/// deemed necessary to help manage multiple interfaces.
// ============================================================================
// ============================================================================

// ============================================================================
// simd_PowerDown
// ----------------------------------------------------------------------------
/// PowerDown the currently active SIM interface.  This will switch the SIM
/// supply voltage to zero.  This function should be called for _all_ active
/// interfaces before calling simd_Close.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_PowerDown(VOID)
{
    // Shutdown the active interface
    hal_SimPowerDown();
    simd_SetSimVoltage(SIMD_VOLTAGE_NULL);
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_PowerDown Config [%d]:0x%08x reg:0x%08x", g_simConfig.ActIf, g_simConfig.SciConfig[g_simConfig.ActIf].Config, hal_SimGetRegConfig());
    // Update local configuration structure
    g_simConfig.SciConfig[g_simConfig.ActIf].Config = hal_SimGetRegConfig();
}

// ============================================================================
// simd_Reset
// ----------------------------------------------------------------------------
/// Perform the full reset sequence on the card on the specified interface.
/// This function will attempt to receive the ATR from the card and place the
/// data in the pRxBytes buffer.  There is no success code returned here as
/// the interrupt generated will contain the status of the reset.
/// @param pRxBytes is the pointer to the buffer to receive the data.  The
/// buffer must be large enough for the maximum expected data size.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_Reset(UINT8* pRxBytes)
{
    UINT8 RegWrVal;
    UINT8 Index;
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_Reset");
    // Program the active interface reset control register to passthrough
    // The non-active interfaces should use the reset value, or the saved
    // polarity value.
    RegWrVal = 0;

    for (Index = 0; Index < MAX_ITF; ++Index)
    {
        if (Index != g_simConfig.ActIf)
        {
            // Saved MT6302 config for the rest...
            RegWrVal |= (g_simConfig.SciConfig[Index].ResetCfg);
            SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_Reset %d 0x%02x", Index, RegWrVal);
        }
    }
    simd_SendRegData(reset_ctrl_reg,RegWrVal);

    hal_SimReset(pRxBytes);
    g_simConfig.SciConfig[g_simConfig.ActIf].Config = hal_SimGetRegConfig();
}

// ============================================================================
// simd_GetNumRxBytes
// ----------------------------------------------------------------------------
/// Gets the number of bytes received by the active SIM Card Interface module.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC UINT16 simd_GetNumRxBytes()
{
    return hal_SimGetNumRxBytes();
}

// ============================================================================
// simd_SetSimVoltage
// ----------------------------------------------------------------------------
/// Sets the voltage level of the active card.  Depending on the hardware, this
/// function may call the PMD driver, or if the voltage is controlled by the
/// SIM device, the voltage may be set directly here.
/// @param simLevel SIM voltage level.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetSimVoltage(SIMD_VOLTAGE_T simLevel)
{
    /// SIMD_VOLTAGE_CLASS_A (5 V)
    /// SIMD_VOLTAGE_CLASS_B (3 V)
    /// SIMD_VOLTAGE_CLASS_C (1.8 V)
    /// SIMD_VOLTAGE_NULL    (0 V)
    // Set Active Sim Voltage
    UINT32 delay;
    UINT8  RegConfig;

    if (g_simConfig.ActIf == 0xff)
    {
        return;
    }

    //XHALT;
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_SetSimVoltage %d", simLevel);
    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)

    SIMD_ASSERT(simLevel>SIMD_VOLTAGE_CLASS_A, "SIMD: This driver does not support Class A voltage\n")

    g_simConfig.SciConfig[g_simConfig.ActIf].Voltage = 0;
    switch (simLevel)
    {
        case SIMD_VOLTAGE_CLASS_B:
            g_simConfig.SciConfig[g_simConfig.ActIf].Voltage = (SIM_CARD_VCCEN|SIM_CARD_VSEL)<<g_simConfig.ActIf;
            break;
        case SIMD_VOLTAGE_CLASS_C:
            g_simConfig.SciConfig[g_simConfig.ActIf].Voltage = SIM_CARD_VCCEN<<g_simConfig.ActIf;
            break;
        case SIMD_VOLTAGE_NULL:
            break;
        default:
            SIMD_ASSERT(FALSE, "SIMD: Requesting unsupported voltage class\n")
            break;
    }
    // Build the register value which sets both voltages in a single write.
    RegConfig = (g_simConfig.SciConfig[0].Voltage) | (g_simConfig.SciConfig[1].Voltage);
    simd_SendRegData(vcc_ctrl_reg,RegConfig);
    delay = DS_SETVOLT_DLY;
    hal_TimDelay(delay MS_WAITING);
}

// ============================================================================
// simd_SetSimDataEnable
// ----------------------------------------------------------------------------
/// TODO: Remove???
/// On certain SIM drivers, the data lines can be completely disconnected.  In
/// these cases, this function provides the interface to control the switch.
/// If the driver does not support this functionality, an assert should be
/// raised.
/// This function is interface DEPENDENT.
// ============================================================================
// ***
PUBLIC VOID simd_SetSimDataEnable(SIMD_CARD_T number)
{
}

// ============================================================================
// simd_SetClockStopMode
// ----------------------------------------------------------------------------
/// This configures the clock stop mode of the active interface.  The card can
/// be generally be stopped in either high or low or don't care.  If any card
/// in the phone cannot be stopped (extremely unlikely)
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetClockStopMode(SIMD_CLOCK_STOP_MODE_T mode)
{
    // The active interface should have the clock mux set to passthrough
    // mode.  Here, program the clock stop mode and save the mode to know
    // how to configure the driver chip when a switch is done.
    hal_SimSetClockStopMode(mode);

    // Update the configuration for the MT6302 register for later when
    // the switch is done.
    g_simConfig.SciConfig[g_simConfig.ActIf].ClockStop = 0;
    switch (mode)
    {
        case SIMD_CLOCK_STOP_NO:
            g_simConfig.SciConfig[g_simConfig.ActIf].ClockStop = SIM_CARD_CPOL<<g_simConfig.ActIf;
            break;
        case SIMD_CLOCK_STOP_L:
            g_simConfig.SciConfig[g_simConfig.ActIf].ClockStop = 0;
            break;
        case SIMD_CLOCK_STOP_H:
            g_simConfig.SciConfig[g_simConfig.ActIf].ClockStop = (SIM_CARD_CPOH|SIM_CARD_CPOL)<<g_simConfig.ActIf;
            break;
    }

    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_SetClockStopMode Config [%d]:0x%08x reg:0x%08x", g_simConfig.ActIf, g_simConfig.SciConfig[g_simConfig.ActIf].Config, hal_SimGetRegConfig());
    // Update the local configuration structure.
    g_simConfig.SciConfig[g_simConfig.ActIf].Config = hal_SimGetRegConfig();
}

// ============================================================================
// simd_SetEtuDuration
// ----------------------------------------------------------------------------
/// Sets the Etu Duration for the specified interface when the clock frequency
/// change is required (after successful Enhanced PPS procedure).  The WWT is
/// automatically adjusted when this function is called.
/// @param F is the name for the divider values described in the SIM spec
/// @param D is the name for the divider values described in the SIM spec
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetEtuDuration(UINT16 F, UINT8 D)
{
    hal_SimSetEtuDuration(F, D);
    // Update the local configuration structure.
    g_simConfig.SciConfig[g_simConfig.ActIf].ClkDiv = hal_SimGetRegClkDiv();
    g_simConfig.SciConfig[g_simConfig.ActIf].Times = hal_SimGetRegTimes();
}

// ============================================================================
// simd_SetWorkWaitingTime
// ----------------------------------------------------------------------------
/// Sets the Work Waiting Time for the specified interface.  The Work Waiting
/// Time is a measure of the maximum time between two bytes in a transfer.
/// This time can be used to detect errors on the interface.  There are default
/// values for this parameter which can be overridden by the card.
/// @param wi should by default be set to 10 before an EPPS but scaled to 80
/// after the EPPS.  The card itself can override the WI value and the value
/// of the parameter should be scaled by 8 if fAndD is 512 and 8.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetWorkWaitingTime(UINT8 wi)
{
    hal_SimSetWorkWaitingTime(wi);
    // Update the local configuration structure.
    g_simConfig.SciConfig[g_simConfig.ActIf].Times = hal_SimGetRegTimes();
}

// ============================================================================
// simd_TxByteRxByte
// ----------------------------------------------------------------------------
/// Starts a SIM transfer on the active interface.
/// A sim transfer consists in sending nTxByte and receiving an answer of
/// nRxByte.
/// At the end of the transfer, an interrupt will be generated which will
/// be treated by HAL/PAL
///
/// @param nTxByte Number of byte to send (Size of the command)
/// @param pTxByte Pointer to the buffer containing the command to send
/// @param nRxByte Number of byte to receive (Size of the answer)
/// @param pRxByte Pointer to the buffer where to store the answer
/// @param stopClock In case of an operation consisting in two commands,
/// we do not want to stop the sim clock between the two commands. In that
/// case we set this parameter to \c FALSE for the first command.
/// Setting it to \c TRUE will stop the clock after the command (For the
/// second command of the pair or a standalone command).  In the case that
/// the clock stop is managed manually i.e. not using the automatic mechanism
///
// ============================================================================
PUBLIC VOID simd_TxByteRxByte(
    UINT16 nTxByte, CONST UINT8* pTxByte,
    UINT16 nRxByte, UINT8* pRxByte,
    BOOL stopClock)
{
    hal_SimTxByteRxByte(nTxByte, pTxByte, nRxByte, pRxByte, stopClock);
}

// ============================================================================
// simd_ConfigReset
// ----------------------------------------------------------------------------
/// This function should be called after a successful ATR using the ARG or not.
/// The reset pin can now be set to MT6302 control and the configuration
/// can be stored.
///
// ============================================================================
PUBLIC VOID simd_ConfigReset(VOID)
{
    UINT8 RegWrVal;

    RegWrVal = (((hal_SimGetResetStatus()<<2)|SIM_CARD_RSTSEL) << g_simConfig.ActIf);
    // This is the configuration for the currently active interface

    g_simConfig.SciConfig[g_simConfig.ActIf].ResetCfg = RegWrVal;


    RegWrVal = (g_simConfig.SciConfig[0].ResetCfg)|(g_simConfig.SciConfig[1].ResetCfg);
    // This is the full register value to configure the MT6302 to control both reset pins

    simd_SendRegData(reset_ctrl_reg,RegWrVal);

}

// ============================================================================
// simd_Switch2Manual
// ----------------------------------------------------------------------------
/// This function should be called after a successful ATR ONLY when using the
/// ARG.  After an ARG, the LLI and MSBH_LSBL control lines are internally
/// driven by the ARG state machine.  When in Dual SIM mode, the state machine
/// is restarted so the internal signals can no longer be used.  This function
/// parses the Ts value and switches to manual control for the format params.
/// This should only be necessary for Dual SIM drivers.
/// @param Ts is the Ts byte that is read and which will be used to determine
/// the transfer format.
// ============================================================================
PUBLIC VOID simd_Switch2Manual(UINT8 Ts)
{
    // Need to switch the format detection to manual mode since the state will be lost
    // when switching to the second SIM
    g_simConfig.SciConfig[g_simConfig.ActIf].Config = hal_SimSwitch2Manual(Ts);
}

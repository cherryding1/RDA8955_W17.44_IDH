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

#include "hal_timers.h"
#include "hal_sim.h"
#include "simd_config.h"
#include "simdp_debug.h"

#include "pmd_m.h"
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

#define MAX_ITF 1   // Maximum number of SIMs supported by the telephone



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
/// @param simdConfig this is the tgt_GetSimConfig() return value.  Should be
/// NULL for this driver.
/// This function is interface INDEPENDENT.
// ============================================================================
// ***
PUBLIC VOID simd_Open(CONST SIMD_CONFIG_T* simdConfig)
{
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_Open");
    hal_SimOpen();
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
    hal_SimClose();
}

// ============================================================================
// simd_DelayAfterEpps
// --------------------------------------------------------------------------
/// Delay after EPPS procedure required for some SIM cards.  This does not
/// require the active card as it's simply an interface independent delay.
/// This function is interface INDEPENDENT.
// ============================================================================
PUBLIC VOID simd_DelayAfterEpps()
{
    hal_SimDelayAfterEpps();
}

// ============================================================================
// simd_SetSimEnable
// ----------------------------------------------------------------------------
/// This function does nothing since there is only one interface
// ============================================================================
PUBLIC BOOL simd_SetSimEnable(SIMD_CARD_T ItfNum)
{
    SIMD_ASSERT(ItfNum < MAX_ITF, "SIMD trying to enable non-existent interface! %d \n", ItfNum);
    return TRUE;
}

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
}

// ============================================================================
// simd_Reset
// ----------------------------------------------------------------------------
/// Perform the full reset sequence on the card.
/// This function will attempt to receive the ATR from the card and place the
/// data in the pRxBytes buffer.  There is no success code returned here as
/// the interrupt generated will contain the status of the reset.
/// @param pRxBytes is the pointer to the buffer to receive the data.  The
/// buffer must be large enough for the maximum expected data size.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_Reset(UINT8* pRxBytes)
{
    hal_SimReset(pRxBytes);
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
    SIMD_TRACE(SIMD_DBG_TRC,0,"SIMD: simd_SetSimVoltage %d", simLevel);
    switch (simLevel)
    {
        case SIMD_VOLTAGE_NULL :
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
            break;
        case SIMD_VOLTAGE_CLASS_C :
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_1V8);
            break;
        case SIMD_VOLTAGE_CLASS_B :
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_3V0);
            break;
        case SIMD_VOLTAGE_CLASS_A :
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_5V0);
            break;
    }
}

// ============================================================================
// simd_SetClockStopMode
// ----------------------------------------------------------------------------
/// This configures the clock stop mode of the active interface.  The card can
/// be generally be stopped in either high or low or don't care.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetClockStopMode(SIMD_CLOCK_STOP_MODE_T mode)
{
    hal_SimSetClockStopMode(mode);
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
/// This function is not required for the single IF system.
// ============================================================================
PUBLIC VOID simd_ConfigReset(VOID)
{
}

// ============================================================================
// simd_Switch2Manual
// ----------------------------------------------------------------------------
/// This functions should not be required for single SIM
// ============================================================================
PUBLIC VOID simd_Switch2Manual(UINT8 Ts)
{
}

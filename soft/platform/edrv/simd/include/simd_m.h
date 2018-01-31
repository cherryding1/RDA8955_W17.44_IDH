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




#ifndef  _SIMD_M_H_
#define  _SIMD_M_H_


typedef enum
{
    /// Set the voltage class A (5 V)
    SIMD_VOLTAGE_CLASS_A = 0,
    /// Set the voltage class B (3 V)
    SIMD_VOLTAGE_CLASS_B = 1,
    /// Set the voltage class C (1,8 V)
    SIMD_VOLTAGE_CLASS_C = 2,
    /// Set the Sim voltage to  0 V
    SIMD_VOLTAGE_NULL    = 3
} SIMD_VOLTAGE_T;

typedef enum
{
    //only select card 0
    SIMD_SELECT_CARD_0 = 0,
    //only select card 1
    SIMD_SELECT_CARD_1 = 1,
    // No card selected
    SIMD_SELECT_NULL = 0xff
} SIMD_CARD_T;

// ============================================================================
// SIMD_CLOCK_STOP_MODE_T
// ----------------------------------------------------------------------------
/// This type is used to set the clock stop mode.
// ============================================================================
typedef enum
{
    /// Continuous clock mode, the autostop is disabled
    SIMD_CLOCK_STOP_NO = 0,
    /// Automatic clock stop mode, stopped at low-level
    SIMD_CLOCK_STOP_L = 1,
    /// Automatic clock stop mode, stopped at high-level
    SIMD_CLOCK_STOP_H =2
} SIMD_CLOCK_STOP_MODE_T;

typedef struct SIMD_CONFIG_STRUCT_T SIMD_CONFIG_T;


/// Make sure to include sim_config.h before simd_m.h or tgt_simd_cfg.h !

// ============================================================================
// ============================================================================
/// These are the Interface INDEPENDENT functions of the driver
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
PUBLIC VOID simd_Open(CONST SIMD_CONFIG_T* simdConfig);

// ============================================================================
// simd_Close
// ----------------------------------------------------------------------------
/// Close the SIM driver and disables the use of its API.
/// simd_PowerDown should be called before this function for _all_ active
/// interfaces.
/// This function is interface INDEPENDENT.
// ============================================================================
PUBLIC VOID simd_Close(VOID);


// ============================================================================
// simd_DelayAfterEpps
// --------------------------------------------------------------------------
/// Delay after EPPS procedure required for some SIM cards.  This does not
/// require the active card as it's simply an interface independent delay.
/// This function is interface INDEPENDENT.
// ============================================================================
PUBLIC VOID simd_DelayAfterEpps(VOID);



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
PUBLIC BOOL simd_SetSimEnable(SIMD_CARD_T ItfNum);



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
/// PowerDown the SIM interface specified.  This will turn switch the SIM
/// supply voltage to zero.  This function should be called for _all_ active
/// interfaces before calling simd_Close.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_PowerDown(VOID);



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
PUBLIC VOID simd_Reset(UINT8* pRxBytes);


// ============================================================================
// simd_GetNumRxBytes
// ----------------------------------------------------------------------------
/// Gets the number of bytes received by the active SIM Card Interface module.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC UINT16 simd_GetNumRxBytes(VOID);

// ============================================================================
// simd_SetSimVoltage
// ----------------------------------------------------------------------------
/// Sets the voltage level of the active card.  Depending on the hardware, this
/// function may call the PMD driver, or if the voltage is controlled by the
/// SIM device, the voltage may be set directly here.
/// @param simLevel SIM voltage level.
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetSimVoltage(SIMD_VOLTAGE_T simLevel);

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
PUBLIC VOID simd_SetSimDataEnable(SIMD_CARD_T ItfNum);

// ============================================================================
// simd_SetClockStopMode
// ----------------------------------------------------------------------------
/// This configures the clock stop mode of the active interface.  The card can
/// be generally be stopped in either high or low or don't care.  If any card
/// in the phone cannot be stopped (extremely unlikely)
/// This function is interface DEPENDENT.
// ============================================================================
PUBLIC VOID simd_SetClockStopMode(SIMD_CLOCK_STOP_MODE_T mode);

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
PUBLIC VOID simd_SetEtuDuration(UINT16 F, UINT8 D);

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
PUBLIC VOID simd_SetWorkWaitingTime(UINT8 wi);

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
    BOOL stopClock);

// ============================================================================
// simd_ConfigReset
// ----------------------------------------------------------------------------
/// This function should be called after a successful ATR using the ARG or not.
/// The reset pin can now be set to MT6302 control and the configuration
/// can be stored.
///
// ============================================================================
PUBLIC VOID simd_ConfigReset(VOID);

// ============================================================================
// simd_Switch2Manual
// ----------------------------------------------------------------------------
/// This function should be called after a successful ATR ONLY when using the
/// ARG.  After an ARG, the LLI and MSBH_LSBL control lines are internally
/// driven by the ARG state machine.  When in Dual SIM mode, the state machine
/// is restarted so the internal signals can no longer be used.  This function
/// parses the Ts value and switches to manual control for the format params.
/// This should only be necessary for Dual SIM drivers.
// ============================================================================
PUBLIC VOID simd_Switch2Manual(UINT8 Ts);

#endif //_SIMD_M_H_

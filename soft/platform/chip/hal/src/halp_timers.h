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

#ifndef  _HALP_TIMERS_H_
#define  _HALP_TIMERS_H_

#include "cs_types.h"
#include "global_macros.h"
#include "timer.h"

#include "hal_debug.h"



// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// =============================================================================
//  TYPES
// =============================================================================


// ============================================================================
// HAL_TIM_MODE_T
// ----------------------------------------------------------------------------
/// Timer modes
/// All the timer can operate in three different modes :
/// - single
/// - repetitive
/// - wrap.
// ============================================================================
typedef enum
{
    HAL_TIM_SINGLE_MODE = 0,   ///< Single mode
    HAL_TIM_REPEAT_MODE  = 1,///< Repetitive mode
    HAL_TIM_WRAP_MODE   = 2      ///< Wrap mode
} HAL_TIM_MODE_T;



// ============================================================================
// HAL_TIM_WD_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// Type of the user handler function for the Watchdog timer
// ============================================================================
typedef VOID (*HAL_TIM_WD_IRQ_HANDLER_T)(VOID);



// ============================================================================
// HAL_TIM_TICK_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// Type of the user handler function for the OS timer
// ============================================================================
typedef VOID (*HAL_TIM_TICK_IRQ_HANDLER_T)(VOID);



// ============================================================================
//
// ----------------------------------------------------------------------------
// ============================================================================

// ============================================================================
//  FUNCTIONS
// ============================================================================

// =============================================================================
// hal_TimWatchDogStop
// -----------------------------------------------------------------------------
/// Stop the watchdog timer
/// This function stops the watchdog timer. A reset will not occur because
/// of the watchdog, even if #hal_TimWatchDogKeepAlive is not called. The
/// watchdog can be re-enabled by a call to #hal_TimWatchDogKeepAlive.
// =============================================================================
PROTECTED VOID hal_TimWatchDogStop(VOID);



// =============================================================================
// hal_TimWatchDogRestart
// -----------------------------------------------------------------------------
/// Restart the watchdog timer
// =============================================================================
PROTECTED VOID hal_TimWatchDogRestart(VOID);



// ============================================================================
// hal_TimTimersIrqHandler
// ----------------------------------------------------------------------------
/// Timers module IRQ handler.
// ============================================================================
PROTECTED VOID hal_TimTimersIrqHandler(UINT8 interruptId);



// ============================================================================
// hal_TickInit
// ----------------------------------------------------------------------------
/// Initializes the OS tick timer and configure its driver.
/// This function is called by hal_Open.
// ============================================================================
PROTECTED VOID hal_TimTickOpen(VOID);



// ============================================================================
// hal_TimTickIrqHandler
// ----------------------------------------------------------------------------
/// OS tick IRQ handler, used by the IRQ module to clear the cause and call
/// the 'user' function.
// ============================================================================
PROTECTED VOID hal_TimTickIrqHandler(UINT8 interruptId);





#endif //_HALP_TIMERS_H_


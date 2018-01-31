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

#ifndef _HALP_LPS_H_
#define _HALP_LPS_H_

#include "hal_lps.h"
#include "hal_sys.h"

// =============================================================================
//  MACROS
// =============================================================================


// values for g_halSysLpsLock
#define HAL_SYS_LPS_LOCK_UNLOCKED 0
#define HAL_SYS_LPS_LOCK_FINT 1
#define HAL_SYS_LPS_LOCK_LGEF 2

// =============================================================================
// HAL_LPS_UNDEF_VALUE
// -----------------------------------------------------------------------------
/// When no calibration done, rate is not defined
// =============================================================================
#define     HAL_LPS_UNDEF_VALUE     0


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_LPS_IRQ_STATUS_T
// -----------------------------------------------------------------------------
/// Represent the IRQ cause and mask of the LPS module.
// =============================================================================
typedef struct
{
    /// 1 when the IRQ was triggered because the calibration is done
    UINT32 lpsCalibDone:1;
    /// 1 when the IRQ was triggered because the Slow Counter started.
    UINT32 lpsSlowRunning:1;
    /// 1 when the IRQ was triggered because the Power-Up sequence is done.
    UINT32 lpsPuDone:1;
    /// 1 when the IRQ was triggered because the Power-Up sequence reached the Ready step.
    UINT32 lpsPuReady:1;
    /// 1 when the IRQ was triggered because the Power-Up frame was reached.
    UINT32 lpsPuReached:1;
} HAL_LPS_IRQ_STATUS_T;

// =============================================================================
// HAL_LPS_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// Type of the user IRQ handling function, called when an LPS interruption
/// occurs. It is set by #hal_LpsIrqSetHandler.
// =============================================================================
typedef VOID (*HAL_LPS_IRQ_HANDLER_T)(HAL_LPS_IRQ_STATUS_T *);


// =============================================================================
// HAL_LPS_STATE_T
// -----------------------------------------------------------------------------
/// Record of LPS State
// =============================================================================
typedef enum
{
    HAL_LPS_IDLE,
    HAL_LPS_CALIBRATED,
    HAL_LPS_CALIB_RUNNING
} HAL_LPS_STATE_T;
// =============================================================================
// HAL_LPS_CONTEXT_T
// -----------------------------------------------------------------------------
/// Record of LPS context
// =============================================================================
// -- NOTE: Remember to update HAL_LPS_CONTEXT_2_T structure in
// --       hal_map.xmd if this structure is changed
typedef struct
{
    /// LPS State
    HAL_LPS_STATE_T lpsState;
    /// Calibration invalidation request
    BOOL lpsInvalidateCalib;
    /// Accuracy
    UINT8   lpsAccuracy;
    /// Low Power Synchronizer Rate value
    UINT32  lpsRate;
    /// Low Power Calibration frequency
    HAL_SYS_FREQ_T calibFreq;
    /// Low Power Synchronizer previous Rate value
    UINT32  lpsRateLast;
    /// Low Power Calibration frequency
    HAL_SYS_FREQ_T calibFreqLast;
    /// Allow Frames to be skipped
    BOOL lpsSkipFrameAllowed;
    /// Last event programmed in active frame
    UINT16 lpsFrameLastEvt;
    /// Number of Frame skipped
    UINT16 lpsSkipFrameNumber;
    /// Pu State machine configuration
    UINT32 lpsPuCtrl;
    /// Number of frames necessary to wake up
    UINT8 lpsWakeUpFrameNbr;
    /// Last time to skip frame
    UINT32 lpsLastTimeToSkipFrame;
    /// Callback called after each power up
    /// (to schedule events in sync with paging period)
    HAL_LPS_CALLBACK_T* lpsCallback;
    HAL_LPS_OS_CALLBACK_T *lpsOsCallback;
} HAL_LPS_CONTEXT_T;

// =============================================================================
//  GLOBALS
// =============================================================================
/// Global context of the lps
PROTECTED EXPORT HAL_LPS_CONTEXT_T g_halLpsCtx;

// ============================================================================
// g_halSysLpsLock
// ----------------------------------------------------------------------------
/// Prevent the hal_SysSetRequestFreq function from changing the system
/// frequency. This lock is used by the LPS mechanism to keep a coherent
/// calibration.
// ============================================================================
PROTECTED EXPORT UINT8 g_halSysLpsLock;

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_LpsDeepSleep
// -----------------------------------------------------------------------------
/// Enter the low power state (32kHz) whenever possible
/// @param die : If TRUE, the sytem will "die" in a low power state
/// This is for Idle power test purpose one. In regular usage always use FALSE
/// @return the irq cause register value, 0 is a normal LPS wake up
// =============================================================================
PROTECTED UINT32 hal_LpsDeepSleep (BOOL die);

PROTECTED VOID  hal_LpsLightSleep ( );
// =============================================================================
// hal_LpsReady
// -----------------------------------------------------------------------------
/// Check if PAL can go to 32k.
// =============================================================================
PROTECTED BOOL hal_LpsReady(VOID);


// =============================================================================
// hal_LpsFrameSkipping
// -----------------------------------------------------------------------------
/// Check if LPS is in frame skipping status.
// =============================================================================
PROTECTED BOOL hal_LpsFrameSkipping(VOID);


// =============================================================================
// hal_LpsIrqHandler
// -----------------------------------------------------------------------------
/// LPS module irq handler.
/// Clear the IRQ and call the IRQ handler user function.
// =============================================================================
PROTECTED VOID hal_LpsIrqHandler(UINT8 interrupt_id);


// =============================================================================
// hal_LpsIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user LPS IRQ handler. This function is called when a LPS IRQ occurs.
/// The status is specific and defined through the type #HAL_LPS_IRQ_HANDLER_T.
/// @param handler User function called in cased of LPS interrupt.
// =============================================================================
PROTECTED VOID hal_LpsIrqSetHandler(HAL_LPS_IRQ_HANDLER_T handler);


// =============================================================================
// hal_LpsIrqSetMask
// -----------------------------------------------------------------------------
/// Set the LPS IRQ mask.
/// The detailed mask configuration can be get from the type
/// #HAL_LPS_IRQ_STATUS_T.
/// @param mask Mask to set for the LPS interruption.
// =============================================================================
PROTECTED VOID hal_LpsIrqSetMask(HAL_LPS_IRQ_STATUS_T *mask);


// =============================================================================
// hal_LpsIrqGetMask
// -----------------------------------------------------------------------------
/// Get the LPS IRQ mask
/// @return The mask currently set for the LPS interrupt.
// =============================================================================
PROTECTED VOID hal_LpsIrqGetMask(HAL_LPS_IRQ_STATUS_T *mask);


// =============================================================================
// hal_LpsDeepSleepWrapper
// -----------------------------------------------------------------------------
/// Have the XCPU entered in critical section.
/// Set the stack to the IRQ Stack (in internal ram)
/// call hal_LpsDeepSleep() with unchanged parameters
///   hal_LpsDeepSleep() does the low power sleep sequence
/// Resore Stack & critical section
/// basically the prototype should be the one of hal_LpsDeepSleep() as this
/// wrapper is only some piece of assembly code.
///
/// Note: The critical section used here is the CPU's one not the IRQ Controller
/// to prevent all IRQ source (debug interrupts, page interrupt...) during low
/// power. So that the power up sequence is executed before executing the
/// interrupt code.
///
/// @param die : If TRUE, the sytem will "die" in a low power state
/// This is for Idle power test purpose one. In regular usage always use FALSE
/// @return the irq cause register value, 0 is a normal LPS wake up
// =============================================================================
PROTECTED UINT32 hal_LpsDeepSleepWrapper(BOOL die);


PROTECTED UINT32 hal_LpsLightSleepWrapper();

// =============================================================================
// hal_LpsClockSysChangeAllowed
// -----------------------------------------------------------------------------
/// blabla
/// Invalidate the previous LPS calib results for the current clock
/// @return TRUE if clock switch is allowed.
// =============================================================================
PROTECTED BOOL hal_LpsClockSysChangeAllowed(HAL_SYS_FREQ_T nextClock);


#endif // _HALP_LPS_H_



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

#include "chip_id.h"

#include "global_macros.h"
#include "sys_ctrl.h"
#include "timer.h"
#include "calendar.h"

#include "halp_debug.h"
#include "halp_timers.h"

#include "hal_mem_map.h"
#include "hal_timers.h"
#include "hal_error.h"
#include "hal_sys.h"
#include "hal_debug.h"
#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#endif

#include "boot.h"

#include "sxr_tim.h"
#include "sxs_rmt.h"


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================


// =============================================================================
// g_halWatchdogCtx
// -----------------------------------------------------------------------------
/// Watchdog context saving structure.
// =============================================================================
HAL_DBG_WATCHDOG_CTX_T HAL_DATA_BACKUP g_halWatchdogCtx;


// =============================================================================
// g_halTickRegistry
// -----------------------------------------------------------------------------
/// Variable to store the user OS tick irq handler
// ===========================================================================
PRIVATE HAL_TIM_TICK_IRQ_HANDLER_T g_halTickRegistry = NULL;


// =============================================================================
// g_halTickStartTick
// -----------------------------------------------------------------------------
/// Start tick of OS timer
// =============================================================================
PRIVATE VOLATILE UINT32 g_halTickStartTick;


// ============================================================================
//  FUNCTIONS
// ============================================================================


// =============================================================================
// hal_TimWatchDogOpen
// -----------------------------------------------------------------------------
/// This function starts a watchdog timer which will start with an initial value
/// of \c delay and decrement by one every 2 periods of a 32 khz clock. When
/// the watchdog timer reaches 0, a reset is generated.
///
/// @param delay Number of 16384 Hz clock cycles the WatchDog timer counts down
/// before reaching 0.
// =============================================================================
PUBLIC VOID hal_TimWatchDogOpen(UINT32 delay)
{
    // The delay time is multiplied by two to keep the tick format consistent
    // throughout all the HAL API. The watchdog is running on a 32 kHz clock.
    HAL_ASSERT( ((delay * 2) & (TIMER_LOADVAL_MASK >> TIMER_LOADVAL_SHIFT)) == delay * 2,
                "Delay overflow: %d", delay );

    hwp_timer->WDTimer_LoadVal = TIMER_LOADVAL(delay * 2);
    hwp_timer->WDTimer_Ctrl = (hwp_timer->WDTimer_Ctrl & TIMER_WDENABLED)? TIMER_RELOAD : TIMER_START;
    //while ((hwp_timer->WDTimer_Ctrl & TIMER_WDENABLED) == 0);
}


// =============================================================================
// hal_TimWatchDogSetPeriod
// -----------------------------------------------------------------------------
/// Setup the watchdog timer which will start at \c delay and decrement by one
/// every 2 clocks 32kHz, after #. Will generate a reset when zero is reached
/// If the WD timer is already started, the timer will be reseted to the new
/// value.
/// If the WD timer is not started yet, this will only setup the configuration.
/// The WD timer will be started the next time #hal_TimWatchDogKeepAlive is
/// called
/// @param delay Number of 16384 Hz clock cycles the WatchDog timer counts down
/// before reaching 0.
// =============================================================================
PUBLIC VOID hal_TimWatchDogSetPeriod(UINT32 delay)
{
    // this does not start the watchdog but initalizes the delay
    // if the WD is already started it will be reseted
    HAL_ASSERT( ((delay * 2) & (TIMER_LOADVAL_MASK >> TIMER_LOADVAL_SHIFT)) == delay * 2,
                "Delay overflow: %d", delay );

    hwp_timer->WDTimer_LoadVal = TIMER_LOADVAL(delay * 2);

    hwp_timer->WDTimer_Ctrl = TIMER_RELOAD;
}


// =============================================================================
// hal_TimWatchDogKeepAlive
// -----------------------------------------------------------------------------
/// Reloads the watchdog timer
/// This function restarts the watchdog timer with the \c delay used to start
/// it (#hal_TimWatchDogOpen) or to set it up (#hal_TimWatchDogSetPeriod),
/// preventing the reset from occurring.
// =============================================================================
PUBLIC VOID hal_TimWatchDogKeepAlive(VOID)
{
    // this resets the watchdog and starts it if not done yet
    if (hwp_timer->WDTimer_Ctrl & TIMER_WDENABLED)
    {
        hwp_timer->WDTimer_Ctrl = TIMER_RELOAD;
    }

#ifdef CHIP_HAS_AP
    hal_ApCommHeartbeatKeepAlive();
#endif
}


// =============================================================================
// hal_TimWatchDogClose
// -----------------------------------------------------------------------------
/// Disables the watchdog timer
/// This function disables the watchdog timer. A reset will not occur because
/// of the watchdog, even if #hal_TimWatchDogKeepAlive is not called. The
/// watchdog can be re-enabled by #hal_TimWatchDogOpen.
// =============================================================================
PUBLIC VOID hal_TimWatchDogClose(VOID)
{
    hwp_timer->WDTimer_Ctrl = TIMER_STOP;
    hwp_timer->WDTimer_LoadVal = 0;
}


// =============================================================================
// hal_TimWatchDogStop
// -----------------------------------------------------------------------------
/// Stop the watchdog timer
/// This function stops the watchdog timer. A reset will not occur because
/// of the watchdog, even if #hal_TimWatchDogKeepAlive is not called. The
/// watchdog can be re-enabled by a call to #hal_TimWatchDogKeepAlive.
// =============================================================================
PROTECTED VOID hal_TimWatchDogStop(VOID)
{
    hwp_timer->WDTimer_Ctrl = TIMER_STOP;

#ifdef CHIP_HAS_AP
    hal_ApCommHeartbeatStop();
#endif
}


// =============================================================================
// hal_TimWatchDogRestart
// -----------------------------------------------------------------------------
/// Restart the watchdog timer
// =============================================================================
PROTECTED VOID hal_TimWatchDogRestart(VOID)
{
    hwp_timer->WDTimer_Ctrl = TIMER_START;

#ifdef CHIP_HAS_AP
    hal_ApCommHeartbeatStart();
#endif
}


// ============================================================================
// hal_TimTickIrqHandler
// ----------------------------------------------------------------------------
/// OS tick IRQ handler, used by the IRQ module to clear the cause and call
/// the 'user' function.
// ============================================================================
PROTECTED VOID hal_TimTickIrqHandler(UINT8 interruptId)
{
    // Clear the Os tick timer IRQ
    hwp_timer->Timer_Irq_Clr = TIMER_OSTIMER_CLR;

    // call user handler
    if (g_halTickRegistry)
    {
        g_halTickRegistry();
    }
}


// ============================================================================
// hal_TimTickGetVal
// ----------------------------------------------------------------------------
/// @return The value of the OS timer.
// ============================================================================
INLINE UINT32 hal_TimTickGetVal(VOID)
{
    return hwp_timer->OSTimer_CurVal;
}


// ============================================================================
// hal_TimTickStart
// ----------------------------------------------------------------------------
/// Start the OS timer in the specified mode
/// @param loadVal Initial value of the OS timer
/// @param mode Timer mode.
// ============================================================================
INLINE VOID hal_TimTickStart(UINT32 loadVal)
{
    hwp_timer->OSTimer_Ctrl = TIMER_ENABLE | TIMER_LOAD | TIMER_LOADVAL(loadVal) | TIMER_WRAP;
}


// ============================================================================
// hal_TimTickStop
// ----------------------------------------------------------------------------
/// Stops the OS tick timer.
// ============================================================================
INLINE VOID hal_TimTickStop(VOID)
{
    hwp_timer->OSTimer_Ctrl = 0;
}


// ============================================================================
// hal_TimTickIrqSetHandler
// ----------------------------------------------------------------------------
/// Set the function called whent the OS timer wraps.
/// @param handler The function called when the OS timer reaches 0.
// ============================================================================
INLINE VOID hal_TimTickIrqSetHandler(HAL_TIM_TICK_IRQ_HANDLER_T handler)
{
    g_halTickRegistry = handler;
}


// ============================================================================
// hal_TimTickIrqSetMask
// ----------------------------------------------------------------------------
/// Set the OS timer IRQ mask.
/// @param mask OS timer IRQ mask. If \c TRUE, the IRQ is triggable.
// ============================================================================
INLINE VOID hal_TimTickIrqSetMask(BOOL mask)
{
    if (mask)
    {
        hwp_timer->Timer_Irq_Mask_Set = TIMER_OSTIMER_MASK;
    }
    else
    {
        hwp_timer->Timer_Irq_Mask_Clr = TIMER_OSTIMER_MASK;
    }
}


// ============================================================================
// hal_TimTickIrqGetMask
// ----------------------------------------------------------------------------
/// @return The OS IRQ mask
// ============================================================================
INLINE BOOL hal_TimTickIrqGetMask(VOID)
{
    if (hwp_timer->Timer_Irq_Mask_Set & TIMER_OSTIMER_MASK)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// ============================================================================
// hal_TickIrqHandler
// ----------------------------------------------------------------------------
/// IRQ handler for the OS timer, called when this timer wrap.
// ============================================================================
PUBLIC VOID hal_TickIrqHandler(VOID)
{
    hal_TimTickStop();
    sxr_Tampax(0, SXR_REGULAR_TIMER);
}


// ============================================================================
// hal_TimTickOpen
// ----------------------------------------------------------------------------
/// Initializes the OS tick timer and configure its driver.
/// This function is called by hal_Open.
// ============================================================================
PROTECTED VOID hal_TimTickOpen(VOID)
{
    HAL_PROFILE_FUNCTION_ENTER(hal_TimTickOpen);

    g_halTickStartTick = 0;

    hal_TimTickIrqSetHandler(hal_TickIrqHandler);
    hal_TimTickIrqSetMask(TRUE);

    HAL_PROFILE_FUNCTION_EXIT(hal_TimTickOpen);
}


// ============================================================================
// hal_TimSetExpirationTime
// ============================================================================
PUBLIC VOID hal_TimSetExpirationTime(UINT32 nextPeriod)
{
    UINT32 sc = hal_SysEnterCriticalSection();
    g_halTickStartTick = hal_TimGetUpTime();
    hal_TimTickStart(nextPeriod > HAL_TIM_MAX_PER? HAL_TIM_MAX_PER : nextPeriod);
    hal_SysExitCriticalSection(sc);
}


// ============================================================================
// hal_TimChangeExpirationTime
// ============================================================================
PUBLIC VOID hal_TimChangeExpirationTime(UINT32 delta, UINT32 nextPeriod)
{
    UINT32 sc = hal_SysEnterCriticalSection();
    g_halTickStartTick += delta;
    hal_TimTickStart(nextPeriod > HAL_TIM_MAX_PER? HAL_TIM_MAX_PER : nextPeriod);
    hal_SysExitCriticalSection(sc);
}


// ============================================================================
// hal_TimGetElapsedTime
// ============================================================================
PUBLIC UINT32 hal_TimGetElapsedTime(VOID)
{
    UINT32 sc = hal_SysEnterCriticalSection();
    UINT32 elapsed = hal_TimGetUpTime() - g_halTickStartTick;
    hal_SysExitCriticalSection(sc);

    return elapsed;
}


// =============================================================================
// hal_TimDelay
// -----------------------------------------------------------------------------
/// This function is to be used when we want to wait for a delay.
/// Attention must be paid to the fact that this is a blocking function
/// and that it could be avoided as possible, and OS function should be
/// used instead. But sometimes, we cannot use the OS functions ...
///
/// @param delay The delay to wait, in number of 16 kHz periods.
// =============================================================================
PUBLIC VOID HAL_BOOT_FUNC_INTERNAL hal_TimDelay(UINT32 delay)
{
    UINT32 initialValue;

    initialValue = hwp_timer->HWTimer_CurVal;

    // Wait
    while ((hwp_timer->HWTimer_CurVal - initialValue) < delay);
}


// =============================================================================
// hal_TimGetUpTime
// -----------------------------------------------------------------------------
/// Return the up time of the system in 16384Hz ticks.
/// The uptime is given as a 32 bits numbers, therefore the user of the
/// function must take care of the count wrap, that will occurs every
/// three days.
///
/// Put this function into RAM as single-bank flash driver will use it when erasing sectors.
///
/// NOTE: to implement timeouts user can calculate (newtime - starttime)
/// and then compare to the desired delay
/// see http://atlas/twiki/bin/view/CoolWiki/CodeAcademyWrapUpTime for details.
///
///
/// @return Up time, in 16384Hz ticks.
// =============================================================================
PUBLIC UINT32 HAL_BOOT_FUNC_INTERNAL hal_TimGetUpTime(VOID)
{
    return hwp_timer->HWTimer_CurVal;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//---------  Non Hal functions, accessed through hal_timers.h  ----------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

extern VOID hal_GdbWatchDogHandler(VOID);

// =============================================================================
// hal_TimeWdRecover
// -----------------------------------------------------------------------------
// Called early in the boot process when watchdog occured, so running in the memory
// context when the WDog occured. Displays the watchdog context.
// =============================================================================
PUBLIC VOID hal_TimeWdRecover(VOID)
{
    HAL_PROFILE_PULSE(XCPU_ERROR);
    hal_DbgFatalTrigger(HAL_DBG_EXL_FATAL);
    if (g_bootResetCause & SYS_CTRL_WATCHDOG_RESET_HAPPENED)
    {
        HAL_DBG_ERROR_CTX_T *errCtx = &(g_halWatchdogCtx.errorContext);

        // WatchDog detected
        // Do not clear watchdog bit or hal_TimWatchDogHappened wont work
        // anyway it is not required to clear it as other reset cause clears it:
        //  - full soft reset clears it
        //  - external hardware reset clears it
        // hwp_reg->sys_rst_clr = WD_HAPPENED;
        HAL_TRACE(_PAL|TSTDOUT, 0,"########################");
        HAL_TRACE(_PAL|TSTDOUT, 0,"WATCH DOG RESET DETECTED\n");
        HAL_TRACE(_PAL|TSTDOUT, 0,"Registers:");
        HAL_TRACE(_PAL|TSTDOUT, 0,"ze=0x00000000  at=0x%08X  v0=0x%08X  v1=0x%08X",
                  errCtx->at, errCtx->v0, errCtx->v1);
        HAL_TRACE(_PAL|TSTDOUT, 0,"a0=0x%08X  a1=0x%08X  a2=0x%08X  a3=0x%08X",
                  errCtx->a0, errCtx->a1,
                  errCtx->a2, errCtx->a3);
        HAL_TRACE(_PAL|TSTDOUT, 0,"t0=0x%08X  t1=0x%08X  t2=0x%08X  t3=0x%08X",
                  errCtx->t0, errCtx->t1,
                  errCtx->t2, errCtx->t3);
        HAL_TRACE(_PAL|TSTDOUT, 0,"t4=0x%08X  t5=0x%08X  t6=0x%08X  t7=0x%08X",
                  errCtx->t4, errCtx->t5,
                  errCtx->t6, errCtx->t7);
        HAL_TRACE(_PAL|TSTDOUT, 0,"s0=0x%08X  s1=0x%08X  s2=0x%08X  s3=0x%08X",
                  errCtx->s0, errCtx->s1,
                  errCtx->s2, errCtx->s3);
        HAL_TRACE(_PAL|TSTDOUT, 0,"s4=0x%08X  s5=0x%08X  s6=0x%08X  s7=0x%08X",
                  errCtx->s4, errCtx->s5,
                  errCtx->s6, errCtx->s7);
        HAL_TRACE(_PAL|TSTDOUT, 0,"t8=0x%08X  t9=0x%08X  k0= < N/A >    k1=0x%08X",
                  errCtx->t8, errCtx->t9, errCtx->k1);
        HAL_TRACE(_PAL|TSTDOUT, 0,"gp=0x%08X  sp=0x%08X  fp=0x%08X  ra=0x%08X",
                  errCtx->gp, errCtx->sp,
                  errCtx->fp, errCtx->ra);

        HAL_TRACE(_PAL|TSTDOUT, 0,"PC when WDog occured : 0x%08X" , errCtx->pc | 0x80000000);

        sxs_BackTrace(errCtx->pc | 0x80000000, (UINT32*)errCtx->sp);

        sxs_Boot -> WDog = TRUE;
//      sxs_Boot -> ExceptionExpected = TRUE;
//      sxs_Boot -> ExceptionCount++;
        sxs_SaveRmtBuffer ();
    }
}


// ============================================================================
// hal_TimTimersIrqHandler
// ----------------------------------------------------------------------------
/// Timers module IRQ handler.
// ============================================================================
PROTECTED VOID hal_TimTimersIrqHandler(UINT8 interruptId)
{
    // get status
    UINT16 status = hwp_timer->Timer_Irq_Cause;

    // OS Timer is on another line and use another module handler
    // so we get rid of its cause.
    status &= TIMER_OTHER_TIMS_IRQ_MASK;

    // clear irq
    hwp_timer->Timer_Irq_Clr = status;

    // According to the status bit, call the appropriate
    // user handler

    // Delay timer, if ever used as an interrupt
    // generator, will lie here :
}



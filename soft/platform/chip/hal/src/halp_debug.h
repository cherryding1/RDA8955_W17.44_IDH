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


#ifndef  _HALP_DEBUG_H_
#define  _HALP_DEBUG_H_

#include "hal_debug.h"
#include "hal_profile_codes.h"
#include "sxs_io.h"

#define HAL_POSSIBLY_UNUSED __attribute__((unused))

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// HAL is composed of several modules implementing several functionalities.
/// Each functionality that output traces will use one of the predefined
/// trace levels below.
// =============================================================================
/// Important trace that can be enabled all the time (low rate)
#define HAL_WARN_TRC        (_HAL | TLEVEL(1))
/// TIMER specific trace
#define HAL_TIMER_TRC       (_HAL | TLEVEL(2))
/// IO (GPIO, KEYPAD, I2C, PWM, EBC) specific trace
#define HAL_IO_TRC          (_HAL | TLEVEL(3))
/// RF and TCU specific trace
#define HAL_RF_TRC          (_HAL | TLEVEL(4))
/// AUDIO (and speech) specific trace
#define HAL_AUDIO_TRC       (_HAL | TLEVEL(5))
/// LCD specific trace
#define HAL_LCD_TRC         (_HAL | TLEVEL(6))
/// SDMMC specific trace
#define HAL_SDMMC_TRC       (_HAL | TLEVEL(7))
/// CAMERA specific trace
#define HAL_CAMERA_TRC      (_HAL | TLEVEL(8))
/// SPI specific trace
#define HAL_SPI_TRC         (_HAL | TLEVEL(9))
/// UART (and Host) specific trace
#define HAL_UART_TRC        (_HAL | TLEVEL(10))
/// USB specific trace
#define HAL_USB_TRC         (_HAL | TLEVEL(11))
/// VOC specific trace
#define HAL_VOC_TRC         (_HAL | TLEVEL(12))
/// DMA and IFC specific trace
#define HAL_DMA_TRC         (_HAL | TLEVEL(13))
/// SIM specific trace
#define HAL_SIM_TRC         (_HAL | TLEVEL(14))
/// LPS and system setup clock specific trace
#define HAL_LPS_TRC         (_HAL | TLEVEL(15))
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define HAL_DBG_TRC         (_HAL | TLEVEL(16))


// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
#ifdef  HAL_NO_PRINTF
#define HAL_TRACE(level, tsmap, fmt, ...)
#else
#define HAL_TRACE(level, tstmap, format, ...) \
     hal_DbgTrace(_HAL | level, tstmap, format, ##__VA_ARGS__)
#endif



#ifdef HAL_NO_ASSERT
#define HAL_ASSERT(condition, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define HAL_ASSERT(condition, format, ...)      \
    if (UNLIKELY(!(condition))) {               \
        hal_DbgAssert(format, ##__VA_ARGS__);   \
    }
#endif



// =============================================================================
//  PROFILING
// -----------------------------------------------------------------------------
/// Those are the macros to use for profiling
/// @todo Implement them with hal_DbgProfilingEnter etc instead of this :
// =============================================================================
#ifdef HAL_PROFILING


// =============================================================================
//  HAL_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define HAL_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_HAL, (CP_ ## pulseName))


// =============================================================================
//  HAL_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_WINDOW_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_WINDOW_ENTER(eventName) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_WINDOW_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define HAL_PROFILE_WINDOW_EXIT(eventName) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_HAL, (CP_ ## eventName))


// =============================================================================
//  HAL_PROFILE_SPI_ACTIVATION
// -----------------------------------------------------------------------------
/// Use this macro to profile the activation of SPI Chip Select.
/// (There is up to 4 CS per SPI.)
// =============================================================================
#define HAL_PROFILE_SPI_ACTIVATION(id, csNum) \
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_HAL, (CP_ACTIVATE_SPI1_CS0 + id*4+ csNum))

// =============================================================================
//  HAL_PROFILE_SPI_DEACTIVATION
// -----------------------------------------------------------------------------
/// Use this macro to profile the deactivation of SPI Chip Select.
/// (There is up to 4 CS per SPI.)
// =============================================================================
#define HAL_PROFILE_SPI_DEACTIVATION(id, csNum) \
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_HAL, (CP_ACTIVATE_SPI1_CS0 + id*4+ csNum))

#else // HAL_PROFILING

#define HAL_PROFILE_FUNCTION_ENTER(eventName)
#define HAL_PROFILE_FUNCTION_EXIT(eventName)
#define HAL_PROFILE_WINDOW_ENTER(eventName)
#define HAL_PROFILE_WINDOW_EXIT(eventName)
#define HAL_PROFILE_PULSE(pulseName)
#define HAL_PROFILE_SPI_ACTIVATION(id, csNum)
#define HAL_PROFILE_SPI_DEACTIVATION(id, csNum)

#endif // HAL_PROFILING

// Finer control for HAL profiling
#define HAL_CLK_PROFILING
#define HAL_RFSPI_PROFILING

#endif //_HALP_DEBUG_H_



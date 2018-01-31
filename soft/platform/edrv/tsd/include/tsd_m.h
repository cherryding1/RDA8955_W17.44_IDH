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





#ifndef _TSD_M_H
#define _TSD_M_H

#include "cs_types.h"


// ============================================================================
// TYPES
// ========================================================================

// =============================================================================
// TSD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type TSD_CONFIG_T for configuration
// ========================================================================
typedef struct TSD_CONFIG_STRUCT_T TSD_CONFIG_T;

// =============================================================================
// FMD_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the FMD module.
// =============================================================================
typedef enum
{
    /// No error occured
    TSD_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible at the time)
    TSD_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    TSD_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    TSD_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    TSD_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    TSD_ERR_NO_ITF,

    /// The calibration matrix is invalid
    TSD_ERR_INVALID_CALIBRATION,

    TSD_ERR_QTY
} TSD_ERR_T;


// ============================================================================
//  TSD_POINT_T
// ----------------------------------------------------------------------------
/// Type used to define a point with X and Y coordinates.
// ============================================================================
typedef struct
{
    UINT32 x;
    UINT32 y;
} TSD_POINT_T;


// ============================================================================
//  TSD_CALIBRATION_POINT_T
// ----------------------------------------------------------------------------
/// Type used to define a calibration point with its coordinates displayed on
/// the LCD and its coordinates returned by the touch screen controler, without
/// any calibration fixing.
/// Therefore, due to scaling,rotating and translating issues, the two sets of
/// coordinates might be significantly different.
/// This structure is given as paramater at tsd_SetCalibPoints().
// ============================================================================
typedef struct
{
    /// Expected coordinates on the LCD screen.
    TSD_POINT_T expected;
    /// Measured RAW values from uncalibrated measurements.
    TSD_POINT_T measured;
} TSD_CALIBRATION_POINT_T;


// ============================================================================
//  TSD_REPORTING_MODE_T
// ----------------------------------------------------------------------------
/// Type used to define upon which touch panel events the driver will report
/// to the callback.
// ============================================================================
typedef struct
{
    /// When \c TRUE, the callback is invoked each time the pen is down.
    /// sending #TSD_PEN_DOWN state.
    BOOL onDown;
    /// When \c TRUE, the callback is invoked periodically, as long as the
    /// pen is held down (i.e. is pressed).
    /// sending #TSD_PEN_PRESSED state.
    BOOL onPressed;
    /// When \c TRUE, the callback is invoked each time the pen is released.
    /// sending #TSD_PEN_UP state.
    BOOL onUp;
    /// The repetition period is for #TSD_PEN_PRESSED events.
    /// It defines a multiple of the debounce time.
    /// The delay unit is dependant on the target configuration of TSD.
    UINT16 repetitionPeriod;
} TSD_REPORTING_MODE_T;


// ============================================================================
//  TSD_PEN_STATE_T
// ----------------------------------------------------------------------------
/// Type used to define the state of the pen
// ============================================================================
typedef enum
{
    /// pen: pressed
    TSD_PEN_DOWN = 0,
    /// pen: hold down
    TSD_PEN_PRESSED,
    /// pen: released
    TSD_PEN_UP
} TSD_PEN_STATE_T;


// ============================================================================
//  TSD_POWER_MODE_T
// ----------------------------------------------------------------------------
/// Type used to resume and sleep TSD
// ============================================================================
typedef enum
{
    /// resume: work mode
    TSD_POWER_RESUME = 0,
    /// sleep: sleep mode
    TSD_POWER_SLEEP,
} TSD_POWER_MODE_T;


// ============================================================================
//  TSD_CALLBACK_T
// ----------------------------------------------------------------------------
/// Type used to define the callback function that will be called by the
/// touch screen driver when a pen event occurs: it becomes pressed, it is hold
/// down or it is released as selected with #TSD_REPORTING_MODE_T by tsd_SetReportingMode().
/// @param pointTouched A pointer to a #TSD_POINT_T which holds the coordinates of the touch point.
/// As long as the driver is not calibrated this point is a RAW Measurement of the touch pannel,
/// Once calibrated, the value is a corrected value.
/// @param penState The state of the pen. See #TSD_PEN_STATE_T structure for details.
///
/// WARNING: this function is called form an IRQ context it should not be doing
/// any long processing, (like updating a screen display) but instead send a
/// message to a task.
// ============================================================================
typedef VOID (*TSD_CALLBACK_T)(TSD_POINT_T* pointTouched,TSD_PEN_STATE_T penState);


// ============================================================================
//  TSD_KEY_CALLBACK_T
// ----------------------------------------------------------------------------
/// Type used to define the callback function that will be called by the
/// touch screen key driver when a key event occurs.
/// @param button The key index of the event.
/// @param penState The state of the key. See #TSD_PEN_STATE_T structure for details.
///
/// WARNING: this function is called form an IRQ context it should not be doing
/// any long processing, (like updating a screen display) but instead send a
/// message to a task.
// ============================================================================
typedef VOID (*TSD_KEY_CALLBACK_T)(UINT8 button,TSD_PEN_STATE_T penState);


// ============================================================================
// FUNCTIONS
// ============================================================================

EXTERN_C_START

// ============================================================================
//  tsd_KeyOpen
// ----------------------------------------------------------------------------
/// This function initializes the touch screen key driver. The configuration of the
/// touch screen pins used is board dependent and stored in a TSD_CONFIG_T struct.
// ============================================================================
PUBLIC VOID tsd_KeyOpen(TSD_KEY_CALLBACK_T callback);


// ============================================================================
//  tsd_Open
// ----------------------------------------------------------------------------
/// This function initializes the touch screen driver. The configuration of the
/// touch screen pins used is board dependent and stored in a TSD_CONFIG_T struct.
// ============================================================================
PUBLIC VOID tsd_Open(TSD_CALLBACK_T callback);


// ============================================================================
//  tsd_Close
// ----------------------------------------------------------------------------
/// This function closes the touch screen driver. Deactivate and close the SPI.
///
/// @param callback Pointer to a user function called when an interruption
/// is generated by the touch screen driver.
// ============================================================================
PUBLIC VOID tsd_Close(VOID);

// ============================================================================
//  tsd_SetPowerMode
// ----------------------------------------------------------------------------
/// This function switch the power mode of TSD.
///
/// @param powerMode The power mode to switch. See #TSD_POWER_MODE_T.
// ============================================================================
PUBLIC VOID tsd_SetPowerMode(TSD_POWER_MODE_T powerMode);

// ============================================================================
//  tsd_SetReportingMode
// ----------------------------------------------------------------------------
/// This function controls under which conditions the touch screen interrupt
/// will be generated. It can be when the pen is pressed, when it is hold down
/// and when it is released. The user function passed via #tsd_Open() will be
/// called in case of a touch screen interruption.
///
/// @param mode Defines which event will generate a call to the user
/// callback function. See the documentation of the type for details.
// ============================================================================
PUBLIC VOID tsd_SetReportingMode(TSD_REPORTING_MODE_T* mode);


// ============================================================================
//  tsd_GetReportingMode
// ----------------------------------------------------------------------------
/// This function returns the touch screen reporting mode. Refer to the type
/// documentation for its interpretation.
///
/// @param mode A pointer to receive the current reporting mode.
// ============================================================================
PUBLIC VOID tsd_GetReportingMode(TSD_REPORTING_MODE_T* mode);


// ============================================================================
//  tsd_SetCalibPoints
// ----------------------------------------------------------------------------
/// This function fixes the calibration parameters of the touch screen driver.
/// From an array of 3 samples per point, it sets an array of
/// 3 Display points and another array of 3 Screen points needed for calibration
///
/// @param calibPoints array of 3 calibration points
//  @return TSD_ERR_INVALID_CALIBRATION if the calibration matrix is invalid.
///
/// This function include a fatal assert that will be triggered if the coordinates
/// of the expected points do not allow a valid calibration (divide by 0)
/// this occurs when the point are aligned, or some have the same value for X or Y
// ============================================================================
PUBLIC TSD_ERR_T tsd_SetCalibPoints(TSD_CALIBRATION_POINT_T calibPoints[3]);


// ============================================================================
//  tsd_SetCalibStatus
// ----------------------------------------------------------------------------
/// Set the calibration status
/// @param isCalibrated \c TRUE when calibrated, \c FALSE to restart calibration
/// Once the status is set to \c TRUE, the callback #TSD_CALLBACK_T will be
/// called with corrected values.
/// note that tsd_SetCalibPoints() must be called before setting the calibration
/// to \c TRUE
// ============================================================================
PUBLIC VOID tsd_SetCalibStatus(BOOL isCalibrated);

// ============================================================================
//  tsd_GetHwVersion
// ----------------------------------------------------------------------------
/// This function returns the version of tp hardware.
// ============================================================================
PUBLIC CHAR* tsd_GetHwVersion(VOID);

EXTERN_C_END

///  @} <- End of the tsd group

#endif  // _TSD_M_H_



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


















#ifndef _RFD_PA_H
#define _RFD_PA_H

#include "gsm.h"
#include "rfd_defs.h"
#include "calib_m.h"
#include "rfd_config.h"

///@defgroup pa_API EDRV RFD PA Drivers
///@{
//======================================================================
// Structure   : RFD_PA_PARAMS_T
/// This structure stores PA's parameters exported to @b PAL via the
/// function \c #rfd_PaGetParameters.
//----------------------------------------------------------------------
//======================================================================
typedef struct
{

///   This constant holds the TCO mask that should be applied during DTX
///   (if applicable).\n
///   Must be defined through the macro #MASK_TCO (SW_TCO_TO_MASK).
    UINT32 dtxTcoSettings;

///   This constant holds the TCO mask that should be applied during rx pdn control.
///   Must be defined through the macro #MASK_TCO (XCV_TCO_TO_MASK).
    UINT32 rxOnTcoSettings;
///   Window timing extents
    RFD_WIN_BOUNDS_T winBounds;

} RFD_PA_PARAMS_T;

//======================================================================
// Function   : rfd_PaGetParameters
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at initialization by PAL to get various
///   PA dependant parameters (see #RFD_PA_PARAMS_T)
///
/// @return     : Pointer to the parameter structure
//======================================================================
extern const RFD_PA_PARAMS_T* rfd_PaGetParameters(VOID);

#ifdef INTERNAL_XCV_CONTROL_PASW

#define rfd_PaTxOn(start, band)
#define rfd_PaTxOff(stop, band)
#define rfd_PaWakeUp()
#define rfd_PaSleep(lp_mode)
#define rfd_PaSetRamps(Win, start)
#define rfd_PaFrameStart()

#else // !INTERNAL_XCV_CONTROL_PASW

//======================================================================
// Function   : rfd_PaTxOn
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///
///@param start : start time (in Qb) of the window, corresponds to
///             the beginning of the first sent bit of the usefull
///             part of the burst.
///             This time doesn't include any offset specific to the PA.
///@param band  : band selection
//======================================================================
extern void rfd_PaTxOn(INT16 start, GSM_RFBAND_T band);

//======================================================================
// Function   : rfd_PaTxOff
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///
///@param stop : stop time (in Qb) of the window, corresponds to
///            the end of the last sent bit of the usefull part
///            of the burst.
///            This time doesn't include any offset specific to the PA.
///@param band : band selection
//======================================================================
extern void rfd_PaTxOff(INT16 stop, GSM_RFBAND_T band);

//======================================================================
// Function   : rfd_PaWakeUp
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called when going out of Low-Power mode
///   (see #rfd_PaSleep). \n
///   It must bring back the PA in its Idle state.
//======================================================================
extern VOID rfd_PaWakeUp(VOID);

//======================================================================
// Function   : rfd_PaSleep
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called before going to LowPower mode.
///   The PA must go in a the Low-power state correponding to the
///   asked mode (if applicable). \n
///
///@param lp_mode : targeted Low Power mode
//======================================================================
extern VOID rfd_PaSleep(RFD_LP_MODE_T lp_mode);

//======================================================================
// Function   : rfd_PaSetRamps
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function is called at frame N with ramp parameters of frame
///   N+1. \n
///   This function must setup all synchronous actions concerning next
///   frame ramping.
///
///@param Win   : Tx window parameters
///
///@param start : start time (in Qb) of the window, corresponds to
///             the beginning of the first sent bit of the usefull
///             part of the burst.
///             This time doesn't include any offset specific to the PA.
//======================================================================
extern VOID rfd_PaSetRamps(RFD_WIN_T *Win, INT16 start);

//======================================================================
// Function   : rfd_PaFrameStart
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   Called at the beginning of the each frame.
///   Allows to update parameters or make actions relative to the
///   current frame
//======================================================================
extern VOID rfd_PaFrameStart(VOID);

#endif // !INTERNAL_XCV_CONTROL_PASW

//======================================================================
// Function   : rfd_PaOpen
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at first PAL initialization (and after an
///   eventual call to #rfd_PaClose).\n
///   It must perform any initialization/setup needed for the PA to be
///   functional.
//======================================================================
extern VOID rfd_PaOpen(CONST PA_CONFIG_T* paConfig);

//======================================================================
// Function   : rfd_PaClose
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called before switching off the PA.
///   To re-enable the PA a call to #rfd_PaOpen will be issued.
//======================================================================
extern VOID rfd_PaClose(VOID);


//======================================================================
// Function   : rfd_PaCalibGetDefault
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Returns the default calibration parameters of the PA.
///
///@return     : Pointer to a const structure containing the default
///   calibration parameters of the PA. See calibration header for
///   type details.
//======================================================================
extern const CALIB_PA_T *rfd_PaCalibGetDefault(void);

//======================================================================
// Function   : rfd_PaCalibUpdateValues
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   The calibration parameters have been modified by the Calib Tool,
///   the parameters used by the PA must be updated, if needed.
//======================================================================
extern void rfd_PaCalibUpdateValues(void);

//======================================================================
// Function   : rfd_PaCalibResetValues
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   The calibration parameters must be reset to their default values.
//======================================================================
extern void rfd_PaCalibResetValues(void);

//======================================================================
// Function   : rfd_PaCalibInit
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   Called when starting calib stub task.
///   Allocate memory for the PA calibration process, more specifically,
///   for the PA profile calibration process.
//======================================================================
extern void rfd_PaCalibInit(void);

//======================================================================
// Function   : rfd_PaCalibPaProfile
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function must implement the PA Profile calibration algorithm
///   for the considered PA.
///@param band      : band selection
///@param PowerMeas : Power measured for last asked PA-DAC Value
///                 (in 100th of dBm)
///@param *NextDAC  : Dac value to be applied for next measurement
///@return     : The return values are driving the stub behavior and
///   allow the implementation of any kind of algorithm.
///   - CALIB_DAEMON_PROCESS_STOP: end of calibration
///   - CALIB_DAEMON_PROCESS_CONTINUE: a measurement will be done.
///           rfd_PaCalibPaProfile will be called again.
///   - CALIB_DAEMON_PROCESS_NEED_CALM: no measurement needed, Tx will be
///           stopped. This allows for heavy computation (like
///           interpolations...) that can last more than a frame.
///           rfd_PaCalibPaProfile will be called again.
///   - CALIB_DAEMON_PROCESS_ERROR: end on error
//======================================================================
extern UINT8 rfd_PaCalibPaProfile(GSM_RFBAND_T band, INT32 PowerMeas, UINT16 *NextDAC);

extern UINT8 rfd_PaCalibfastPaProfile(GSM_RFBAND_T band);
///@} <- End of Pa API

#endif // _RFD_PA_H


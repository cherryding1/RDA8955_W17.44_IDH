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


















#ifndef _RFD_SW_H
#define _RFD_SW_H

#include "gsm.h"
#include "rfd_defs.h"
#include "rfd_config.h"

///@defgroup sw_API EDRV RFD Switch Drivers
///@{
//======================================================================
// Structure   : RFD_SW_PARAMS_T
/// This structure stores Xcver's parameters exported to @b PAL and
/// @b HAL via the function \c #rfd_SwGetParameters.
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

} RFD_SW_PARAMS_T;


//======================================================================
// Function   : rfd_SwOpen
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at first PAL initialization (and after an
///   eventual call to #rfd_SwClose).\n
///   It must perform any initialization/setup needed for the Switch to be
///   functional.
//======================================================================
extern VOID rfd_SwOpen(CONST SW_CONFIG_T* swConfig);

//======================================================================
// Function   : rfd_SwClose
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called before switching off the Switch.
///   To re-enable the Switch a call to #rfd_SwOpen will be issued.
//======================================================================
extern VOID rfd_SwClose(VOID);

//======================================================================
// Function   : rfd_SwGetParameters
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at initialization by PAL to get various
///   switch dependant parameters (see #RFD_SW_PARAMS_T)
///
///@return     Pointer to the parameter structure
//======================================================================
extern const RFD_SW_PARAMS_T* rfd_SwGetParameters(VOID);

#ifdef INTERNAL_XCV_CONTROL_PASW

#define rfd_SwWakeUp()
#define rfd_SwSleep(lp_mode)
#define rfd_SwSetPosition(direction, band, date)

#else // !INTERNAL_XCV_CONTROL_PASW

//======================================================================
// Function   : rfd_SwWakeUp
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called when going out of Low-Power mode
///   (see #rfd_SwSleep). \n
///   It must bring back the Switch in its Idle state.
//======================================================================
extern VOID rfd_SwWakeUp(VOID);

//======================================================================
// Function   : rfd_SwSleep
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called before going to LowPower mode.
///   The Switch must go in a the Low-power state correponding to the
///   asked mode (if applicable). \n
///
///@param lp_mode : targeted Low Power mode
//======================================================================
extern VOID rfd_SwSleep(RFD_LP_MODE_T lp_mode);

//======================================================================
// Function   : rfd_SwSetPosition
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Sets the Rf switch in the needed position depending on Rx, Tx
///   and Band parameters.
///
///@param direction : Rx / Tx selection
///@param band      : band selection
///@param date      : start time (in Qb) of the window, corresponds to
///          the beginning of the first sent (for Tx) or received (for
///          Rx) bit of the usefull part of the burst.
///          This time doesn't include any offset specific to the Rf
///          Switch.
//======================================================================
extern void rfd_SwSetPosition (RFD_RFDIR_T direction, GSM_RFBAND_T band, INT16 date);

#endif // !INTERNAL_XCV_CONTROL_PASW

//======================================================================
// Function   : rfd_SwCalibGetDefault
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Returns the default calibration parameters of the switch.
///
///@return     Pointer to a const structure containing the default
///   calibration parameters of the switch. See calibration header for
///   type details.
//======================================================================
extern const CALIB_SW_T *rfd_SwCalibGetDefault(void);

//======================================================================
// Function   : rfd_SwCalibResetValues
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Reset the calibration parameters must be reset to their default
///   values.
//======================================================================
extern void rfd_SwCalibResetValues(void);

//======================================================================
// Function   : rfd_SwCalibUpdateValues
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   The calibration parameters have been modified by the Calib Tool,
///   therefore the parameters used by the switch must be updated.
//======================================================================
extern void rfd_SwCalibUpdateValues(void);
///@} <- End of Switch API

#endif // _RFD_SW_H


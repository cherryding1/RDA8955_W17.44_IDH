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



















#ifndef _RFD_XCV_H
#define _RFD_XCV_H

#include "gsm.h"
#include "rfd_defs.h"
#include "calib_m.h"
#include "hal_rfspi.h"
#include "rfd_config.h"


///@defgroup xcv_API EDRV RFD Transceiver Drivers
///@{

#define GALLITE_CHIP_ID_8805_V5 (0x18880)
#define GALLITE_CHIP_ID_8805_V6 (0x188A0)
#define GALLITE_CHIP_ID_8805_V7 (0x188A2)
#define GALLITE_CHIP_ID_8805_V8 (0x188A3)

#define GALLITE_CHIP_ID_8806_U03 (0x188A3)
#define GALLITE_CHIP_ID_8806_U05 (0x188A5)
#define GALLITE_CHIP_ID_8806_U06 (0x188A6)

#define XCV_CHIP_ID_8808_V2 (0x18882)
#define XCV_CHIP_ID_8808_V3 (0x18883)
#define XCV_CHIP_ID_8808_V4 (0x18884)
#define XCV_CHIP_ID_8808_V5 (0x18885)
#define XCV_CHIP_ID_8808_VF (0x1888F)

#define XCV_CHIP_ID_8809_V2 (0x18882)
#define XCV_CHIP_ID_8809_V3 (0x18883)
#define XCV_CHIP_ID_8809_V4 (0x18884)

#define XCV_CHIP_ID_INVALID_EVENT (0x6220FFFF)

typedef struct
{
/// TRUE if using DigRF compliant Xcver.
/// If the Xcver uses I/Q analog interface this should be set to FALSE.
    BOOL used;

/// Selects 1 or 2 samples per symbol
    UINT8 rxRate;
/// Indicates the number of effective (valid) bits outputed by the Xcver
/// (up to 16).
/// NOTE: the baseband expects signed (2'complement) samples.
    UINT8 rxSampleWidth;
/// Indicates if the effective bits are MSB or LSB aligned (relevant
/// only if rxSampleWidth /= 16)
    UINT8 rxSampleAlign;
/// Compound gain value giving the relationship between numerical sample
/// value and dBm unit.
/// Should be composed of both the ADC gain and of the conversion to dBm
/// Exemple:
/// a 2Vppd/12bits ADC gives a gain of 20 Log ( 2048 / sqrt(2) ) = 63 dB
/// assuming a 50 Ohm load, 2 Vppd = 10 dBm, leading to a final value of
/// rxAdc2DbmGain = 63-10 = 53
    UINT8 rxAdc2DbmGain;
/// selects Rx clock polarity
    UINT8 rxClkPol;
/// selects stream or buffer mode
    UINT8 txMode;
/// selects Tx clock polarity
    UINT8 txClkPol;
} RFD_XCV_DIGRF_PARAMS_T;

//======================================================================
// Structure   : RFD_XCV_TX_BURST_MAPPING_PARAMS_T
//----------------------------------------------------------------------
// These 4 parameters are used to define the Tx burst mapping expected
// by the transceiver. 'Preamble' is used for transitions from slot OFF
// to slot ON (window start and beginning of burst after a 'hole' in a
// multi slot window).
// 'Postamble' is used for transitions from slot ON to slot OFF (end of
// window and end of burst before a 'hole' in a multi slot window).
// 'guardStart' is the number of guard symbols placed before symbol zero of
// the active part(with a maximum of 8), the remaining guard symbols (8-GS)
// will be placed at the end of the burst.
// Note that this parameter is used only for multi-burst windows and makes
// sense only if different modulations are used within the window.
//
//  ________________________________________________________________
// |PRE | Bst1 | 8-GS | GS | Bst2 | 8-GS | GS ...| GS | BstN | POST |
//  ----------------------------------------------------------------
//
//----------------------------------------------------------------------
//======================================================================
typedef struct
{
/// This is the number of preamble symbols that need to be inserted before
/// the first symbol of a 'non-consecutive' burst in a window. (transitions
/// from slot OFF to slot ON).
    UINT8 preamble;

/// This value is the number of guard symbols placed before symbol zero of
/// the active part(with a maximum of 8), the remaining guard symbols (8-GS)
/// will be placed at the end of the burst.
/// Note that this parameter is used only for multi-burst windows and makes
/// sense only if different modulations are used within the window.
    UINT8 guardStart;

/// This value is the number of postamble symbols required after last symbol
/// of the active part of a 'non-followed' burst in a window. (transitions
/// from slot ON to slot OFF).
    UINT8 postamble;
} RFD_XCV_TX_BURST_MAPPING_PARAMS_T;

//======================================================================
// Structure   : RFD_XCV_PARAMS_T
/// This structure stores Xcver's parameters exported to @b PAL and
/// @b HAL via the function \c #rfd_XcvGetParameters.
//----------------------------------------------------------------------
//======================================================================
typedef struct
{
///   This constant indicates how many frames the Xcver needs to
///   perform its wake-up sequence when going out of low-power
///   mode (Note that the first time init can be of any duration and
///   doesn't use this constant).
///   \n If the sequence last for less than 3 ms _and_ if first Rf window
///   can be issued 3+4.6 ms after the beginning of the sequence then
///   this parameter should be set to 1.
///   \n Every additional unit will allocate an extra 4.6 ms period for
///   the wake-up sequence. (i.e. '2' will allow a 7.6 ms sequence with
///   a first window issued not sooner than 12.2 ms after the beginning
///   of the sequence).
///   Using the value 0 will allow doing the wake-up and the first Rf window
///   setup (first fint for L1) during the same wake-up pseudo-frame,
///   only usable if rfd_XcvWakeUp is not lasting long (<0.2ms) as most
///   time is already taken by waiting for the clock.
    UINT8 wakeUpFrameNbr;

///   Delay needed between the power-up of Xcver Vdd and the Enable of the VCO.
///   \n The delay is expressed in number of 32KHz clock periods (P=30517 ns)
///   \n must be in the range 0-63
    UINT8 pu2VcoEnDelay;

///   Delay needed between the Enable of the VCO and the settling of a stable
///   clock at its ouput.
///   \n The delay is expressed in number of 32KHz clock periods (P=30517 ns)
///   \n must be in the range 0-4095 (even though current interface only allows
///   0-255)
    UINT8 vcoEn2VcoStableDelay;

///   Usage of V_RF to control the transceiver's LDO in low power
    RFD_LDO_BEHAVIOR_T vrfBehaviour;

///   This constant holds the TCO mask that should be applied during DTX.\n
///   Must be defined through the macro #MASK_TCO (XCV_TCO_TO_MASK).
    UINT32 dtxTcoSettings;

///   This constant holds the TCO mask that should be applied during rx pdn control.
///   Must be defined through the macro #MASK_TCO (XCV_TCO_TO_MASK).
    UINT32 rxOnTcoSettings;

///   This is the lower limit power value (in -dBm) for cells that should be
///   taken into account for synchronization attempt by L1 (i.e. cells reported
///   with a power higher than lowRla are eligible for synchronization).
///   \n This value is linked to the Xcver's noise floor when doing AGC power
///   measurement (for Monitoring and Interference windows).
    UINT8 lowRla;

///   This is a bitmap describing which DC Offset Correction mode(s) should
///   be enabled with the transceiver.
///   Set this to RFD_HW_DCOC_ENABLED if the Xcver offers a "DC Offset calibration
///   period" before the beginning of each reception. This period consist in a
///   zero'ed output of the Xcver on both I and Q lines, that can be sampled
///   by the baseband in order to evaluate the "static" DC Offset. This period
///   should last for at least 16 bits.
///   Set this to RFD_SW_DCOC_ENABLED if you want the "software DCOC" of the
///   baseband to be active (this is a post processing that doesn't require
///   special features from the Xcver, but that takes processing power and
///   can deteriorate Snr if not needed).
///   Set this to RFD_NO_DCOC if no DCOC is needed/supported
///   NOTE: both modes can be activated togheter
    UINT8 dcocMode;

///   Window timing extents
    RFD_WIN_BOUNDS_T winBounds;

///   DigRf related parameters
    RFD_XCV_DIGRF_PARAMS_T digRf;

///   Tx Burst mapping parameters for AB, NB GMSK and NB 8PSK
    RFD_XCV_TX_BURST_MAPPING_PARAMS_T txBstMap;
} RFD_XCV_PARAMS_T;

//======================================================================
// Function   : rfd_XcvGetParameters
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at initialization by PAL to get various
///   transceiver dependant parameters (see #RFD_XCV_PARAMS_T)
///
///@return     Pointer to the parameter structure
//======================================================================
extern const RFD_XCV_PARAMS_T* rfd_XcvGetParameters(VOID);

//======================================================================
// Function   : rfd_XcvRxOn
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function is called for every Rx window and must setup all
///   Transceiver operations needed to open the window (Spi and TCU
///   commands) through PAL API.
///
///@param Win   : Rx window parameters
///@param start : start time (in Qb) of the window, corresponds to
///               the beginning of the first sampled bit of the burst.
///            This time doesn't include any offset specific to the Xcv.
///@return      : returns the voltage gain of the Rx chain for the
///               current setting.
//======================================================================
extern UINT8   rfd_XcvRxOn(RFD_WIN_T *Win, INT16 start);

//======================================================================
// Function   : rfd_XcvRxOff
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function is called for every Rx window and must setup all
///   Transceiver operations needed to close the window (Spi and TCU
///   commands) through PAL API.
///
///@param Win  : Rx window parameters
///@param stop : stop time (in Qb) of the window, corresponds to
///              the end of the last sampled bit of the burst.
///            This time doesn't include any offset specific to the Xcv.
//======================================================================
extern VOID rfd_XcvRxOff(RFD_WIN_T *Win, INT16 stop);

//======================================================================
// Function   : rfd_XcvTxOn
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function is called for every Tx window and must setup all
///   Transceiver operations needed to open the window (Spi and TCU
///   commands) through PAL API.
///
///@param Win : Tx window parameters
///@param start : start time (in Qb) of the window, corresponds to
///               the beginning of the first sent bit of the usefull
///               part of the burst.
///            This time doesn't include any offset specific to the Xcv.
//======================================================================
extern VOID rfd_XcvTxOn(RFD_WIN_T *Win, INT16 start);

//======================================================================
// Function   : rfd_XcvTxOff
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function is called for every Rx window and must setup all
///   Transceiver operations needed to close the window (Spi and TCU
///   commands) through PAL API.
///
///@param Win : Tx window parameters
///@param stop : stop time (in Qb) of the window, corresponds to
///              the end of the last sent bit of the usefull part
///              of the burst.
///            This time doesn't include any offset specific to the Xcv.
//======================================================================
extern VOID rfd_XcvTxOff(RFD_WIN_T *Win, INT16 stop);


//======================================================================
// Function   : rfd_XcvGetAgcGain
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This function returns the gain used by the Xcver in case of AGC
///   for a couple of step/band parameters.
///   If the Xcver doesn't support the AGC mechanism, then the step
///   parameter is irrelevant.
///
///@param step : gain step that was reached during the AGC measurement.
///@param band : RF Band used during measurement.
///@param arfcn : ARFCN used during the measurement.
///@return       Xcver chain gain in dB
//======================================================================
extern UINT8   rfd_XcvGetAgcGain(UINT8 step, GSM_RFBAND_T band, UINT16 arfcn);

//======================================================================
// Function   : rfd_XcvOpen
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at HAL initialization.\n
///   It must configure the RF Spi according to the Xcver's needs. \n
///   It must perform the power-up sequence of the Xcver and setup the
///   Xtal parameters, taking into account the AfcOffset parameter (
///   see @ref xtal_aging "Crystal Aging Compensation" for more details).
///   After the call to this function, the call of rfd_XcvRfInit is needed
///   before the Xcver starts to Rx/Tx.
///
///@param AfcOffset : Afc Offset due to crystal aging that was measured
///                   during last turned-on period of the MS.
//======================================================================
extern VOID rfd_XcvOpen(INT32 AfcOffset, CONST XCV_CONFIG_T* xcvConfig);

//======================================================================
// Function   : rfd_XcvRfInit
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called at first PAL initialization.\n
///   It must configure the RF parameters of the Xcver.
///   After the call to this function the Xcver must be in its Idle state.
//======================================================================
extern VOID rfd_XcvRfInit(VOID);

//======================================================================
// Function   : rfd_XcvClose
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called before switching off the Xcver.
///   To re-enable the Xcver a call to #rfd_XcvOpen will be issued.
//======================================================================
extern VOID rfd_XcvClose(VOID);

//======================================================================
// Function   : rfd_XcvWakeUp
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called when going out of Low-Power mode
///   (see #rfd_XcvSleep). \n
///   It must bring back the Xcver in its Idle state.
//======================================================================
extern VOID rfd_XcvWakeUp(VOID);

//======================================================================
// Function   : rfd_XcvSleep
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   This function is called before going to LowPower mode.
///   The Xcver must go in a the Low-power state correponding to the
///   asked mode (if applicable). \n
///   It must perform the appropriate power-down sequence of the Xcver,
///   excepted what is relative to the power supply (Xcver and VCO
///   supplies are managed by the LowPowerSynchronizer)
///
///@param lp_mode : targeted Low Power mode
//======================================================================
extern VOID rfd_XcvSleep(RFD_LP_MODE_T lp_mode);

//======================================================================
// Function   : rfd_XcvFrequencyTuning
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   Compensate for asked frequency offset.
///   If an Analog AFC is used, this function must call palChangeAfcDac
///   to update the DAC value.
///
///@param FOf : frequency drift to be compensated (in Hertz)
///@param Band : Band used for the frequency offset measurement
//======================================================================
extern VOID rfd_XcvFrequencyTuning(INT32 FOf, GSM_RFBAND_T Band);
extern VOID rfd_XcvFrequencyTrans(UINT16 *afcOffset, INT16 *CdacOffset, S32 FOf_total, GSM_RFBAND_T band);

//======================================================================
// Function   : rfd_XcvGetAfcOffset
//----------------------------------------------------------------------
// Description:
///   Asynchronous.\n
///   Function used for Crystal Aging detection (see @ref xtal_aging
///   "PAL Cristal Aging detection scheme") \n
///   When this function is called the Xcver driver must return the
///   current AFC setting offset taking into account both :
///   - the normal mid-range value of the AFC
///   - the default crystal parameter.\n
///   The actual unit of the return value is let to the driver's
///   choice, and will be treated by PAL as a linearly varying value
///   (i.e. it will be averaged over successive calls to
///   rfd_XcvGetAfcOffset). DAC units is a possible choice.
///
///@return      AFC Offset
//======================================================================
extern INT32 rfd_XcvGetAfcOffset(VOID);

//======================================================================
// Function   : rfd_XcvCalibGetDefault
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Returns the default calibration parameters of the transceiver.
///
///@return     Pointer to a const structure containing the default
///   calibration parameters of the transceiver. See calibration
///   header for type details.
//======================================================================
extern const CALIB_XCV_T *rfd_XcvCalibGetDefault(VOID);

//======================================================================
// Function   : rfd_XcvCalibUpdateValues
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   The calibration parameters have been modified by the Calib Tool,
///   the parameters used by the XCV must be updated, if needed.
//======================================================================
extern VOID rfd_XcvCalibUpdateValues(VOID);

//======================================================================
// Function   : rfd_XcvCalibResetValues
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   The calibration parameters must be reset to their default values.
//======================================================================
extern VOID rfd_XcvCalibResetValues(VOID);

//======================================================================
// Function   : rfd_XcvCalibXtalFreqOffset
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Changes the Xtal settings for rough frequency adjustment and saves
///   modified parameters in calibration structure.
///   This function will be called sequentially as long as it hasn't
///   returned HAL_CALIB_PROCESS_STOP.\n
///   The convergency of the Xtal parameters must be handled here. The
///   algorithm is xcver driver responsability.
///   Note that this function is only used for production calibration
///   and should update the calibration structure only if
///   \c __CALIB_ENABLED__ compile flag is set.
///  FIXME __CALIB_ENABLED__ doens't exist anymore !
///
///@param FOf : Measured Frequency offset in hertz.
///@return      The return values are driving the stub behavior and
///             allow the implementation of any kind of algorithm.\n
///   - HAL_CALIB_PROCESS_STOP: when Xtal parameters have converged to
///             there best value\n
///   - HAL_CALIB_PROCESS_CONTINUE: a measurement will be done.
///             rfd_XcvCalibXtalFreqOffset will be called again.\n
///   - HAL_CALIB_PROCESS_ERROR: end on error (no convergency,
///             limit reached...).
//======================================================================
extern UINT8 rfd_XcvCalibXtalFreqOffset(INT32 FOf);

//======================================================================
// Function   : rfd_XcvCalibSetAfcBound
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Put the Xcver AFC to the minimum or maximum setting that should be
///   considered to calculate the AFC Gain.
///   This function is only used for production calibration.
///
///@param bound :
///   - #RFD_AFC_BOUND_CENTER for center frequency\n
///   - #RFD_AFC_BOUND_NEG for most negative frequency offset\n
///   - #RFD_AFC_BOUND_POS for most positive frequency offset.\n
//======================================================================
extern VOID rfd_XcvCalibSetAfcBound(RFD_AFC_BOUND_T bound);

extern VOID rfd_XcvCalibSetAfcBoundChangeDAC(RFD_AFC_BOUND_T bound);
//======================================================================
// Function   : rfd_XcvCalibCalcAfcGain
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   Derives the "AFC Gain" on considered band from the frequency
///   offset measured between the two AFC bounds.
///   This function is only used for production calibration and should
///   update the calibration structure only if PAL_CALIB compile flag
///   is set.
///
///@param FreqOffset : Frequency offset measured between the two AFC bounds
///@param band       : Considered band
//======================================================================
extern VOID rfd_XcvCalibCalcAfcFreqChangeDAC(INT32 FreqOffset, GSM_RFBAND_T band);
extern VOID rfd_XcvCalibCalcAfcGain(INT32 FreqOffset, GSM_RFBAND_T band);
extern VOID rfd_XcvCalibCalcAfcGainChangeDAC(INT32 FreqOffset, GSM_RFBAND_T band);
//======================================================================
// Function   : rfd_XcvCalibSetILoss
//----------------------------------------------------------------------
// Description:
///   Synchronous.\n
///   This first call to this function (after the restart of the system or
///   after a call to the rfd_XcvCalibResetValues function) will do nothing except
///   for returning the ARFCN on which the power measurement must be done.
///   The following calls will stores the Insertion Loss of the Rx path in the
///   calibration structure.
///   This function will be called sequentially as long as it hasn't
///   returned CALIB_PROCESS_STOP.
///   This function is only used for production calibration and should
///   update the calibration structure only if PAL_CALIB compile flag
///   is set.
// Parameters :
///@param offset: Measured Power offset in dB
///@param band  : Considered band
///@param arfcn : ARFCN for which the power loss must be measured and then
///               sent to the XCV through this function.
///@return     : The return values are driving the stub behavior and
///             allow the implementation of any kind of algorithm.
///   - #CALIB_PROCESS_STOP: when Xtal parameters have converged to
///             there best value
///   - #CALIB_PROCESS_CONTINUE: a measurement will be done.
///             rfd_XcvCalibSetILoss will be called again.
///   - #CALIB_PROCESS_ERROR: end on error (no convergency,
///             limit reached...).
//======================================================================
extern UINT8 rfd_XcvCalibSetILoss(INT8 offset, GSM_RFBAND_T band, UINT16* nextArfcn);

// =============================================================================
// rfd_XcvCalibSetPaRamp
// -----------------------------------------------------------------------------
/// This is a "calibration only" version of the ramp
/// driver. It sets the DAC ramp target value.
///
/// @param dacValue Summit Dac Value.
// =============================================================================
PUBLIC VOID rfd_XcvCalibSetPaRamp (UINT16* dacValue);


//======================================================================
// rfd_XcvGetAfcDacOffset
// This function used to get the difference between current g_afcDacVal and AFC_MID
//----------------------------------------------------------------------
PUBLIC INT16 rfd_XcvGetAfcDacOffset(VOID);


//======================================================================
// rfd_XcvGetCdacOffset
// This function used to get current CDAC offset
//----------------------------------------------------------------------
PUBLIC INT16 rfd_XcvGetCdacOffset(VOID);


//======================================================================
// rfd_XcvSetAfcDacOffset
// This function used to set g_afcDacVal
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvSetAfcDacOffset(INT16 offset);


//======================================================================
// rfd_XcvSetCdacOffset
// This function used to set CDAC offset
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvSetCdacOffset(INT16 offset);


//======================================================================
// rfd_XcvReadChipId
// This function used to read XCV chip ID from the chip
//----------------------------------------------------------------------
PUBLIC VOID rfd_XcvReadChipId(VOID);


//======================================================================
// rfd_XcvGetChipId
// This function used to get XCV chip ID from the driver variable
//----------------------------------------------------------------------
PUBLIC UINT32 rfd_XcvGetChipId(VOID);

PUBLIC VOID rfd_XcvEnablePower(BOOL dcdc_on, BOOL ldo_on);

///@} <- End of Xcver API

#endif // _RFD_XCV_H


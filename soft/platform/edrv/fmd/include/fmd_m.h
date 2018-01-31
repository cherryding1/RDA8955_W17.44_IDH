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



#ifndef _FMD_M_H_
#define _FMD_M_H_

#include "cs_types.h"

#include "tgt_fmd_cfg.h"


/// @defgroup fmd EDRV FM Driver (FMD)
/// @{
///
/// @author Coolsand Technologies, Inc.
/// @version $Revision: 34104 $
/// @date $Date: 2016-08-03 17:57:01 +0800 (Wed, 03 Aug 2016) $
///
/// This document describes the characteristics of the FM Radio Driver
///
/// FM Radio Driver (FMD) is the software layer controlling the radio FM chip.
///
/// @par Usage / Execution Flow
/// - Basic flow is to open with the full parameter to listen to the radio,
///   and close to stop it.
/// - Once open, one can tune to other stations, seek to find stations,
///   change the volume ...
/// - Tune and seek operations are long, the driver takes care of it and will call
///   the provided callback function once the operation is done.
///   One should not start a new tune or seek operation when there is already
///   one running.
/// - During tune or seek operation, changing the volume or getting the current
///   frequency is allowed (the driver might however decide to delay the volume
///   changing to after the end of the tune/seek operation or return the previous
///   frequency, this is up to driver implementation and the chip capacity)
/// - in audio option the \c forceMono flag can be used to improve reception
///   quality. (For instance some radio only enable the stereo if the channel
///   has been found by a seek operation,
///   if the frequency was tuned by the user, it is forced in mono...)
/// .
///


// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// FMD_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the FMD module.
// =============================================================================
typedef enum
{
    /// No error occured
    FMD_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible at the time)
    FMD_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    FMD_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    FMD_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    FMD_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    FMD_ERR_NO_ITF,


    FMD_ERR_QTY
} FMD_ERR_T;

// =============================================================================
// FMD_ANA_LEVEL_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the analog output volume.
// =============================================================================
typedef enum
{
    FMD_ANA_MUTE = 0,
    FMD_ANA_VOL_1,
    FMD_ANA_VOL_2,
    FMD_ANA_VOL_3,
    FMD_ANA_VOL_4,
    FMD_ANA_VOL_5,
    FMD_ANA_VOL_6,
    FMD_ANA_VOL_7,
    FMD_ANA_VOL_8,
    FMD_ANA_VOL_9,
    FMD_ANA_VOL_10,
    FMD_ANA_VOL_11,
    FMD_ANA_VOL_12,
    FMD_ANA_VOL_13,
    FMD_ANA_VOL_14,
    FMD_ANA_VOL_15,
    FMD_ANA_VOL_QTY
} FMD_ANA_LEVEL_T;

// =============================================================================
// FMD_BAND_T
// -----------------------------------------------------------------------------
/// This type describes the possible level value for the FM BAND:
// =============================================================================
typedef enum
{
    /// 87.5-108 MHz    (US/Europe)
    FMD_BAND_US_EUROPE = 0,
    /// 76-91 MHz       (Japan)
    FMD_BAND_JAPAN,
    /// 76-108 MHz      (World Wide)
    FMD_BAND_WORLD,
    /// 65-76 MHz       (East Europe)
    FMD_BAND_EAST_EUROPE,

    FMD_BAND_QTY
} FMD_BAND_T;

// =============================================================================
// FMD_FREQ_T
// -----------------------------------------------------------------------------
/// FM audio stream sample rate
// =============================================================================
typedef enum
{
    FMD_FREQ_8000HZ  =  0,
    FMD_FREQ_11025HZ,
    FMD_FREQ_12000HZ,
    FMD_FREQ_16000HZ,
    FMD_FREQ_22050HZ,
    FMD_FREQ_24000HZ,
    FMD_FREQ_32000HZ,
    FMD_FREQ_44100HZ,
    FMD_FREQ_48000HZ
} FMD_FREQ_T;


EXTERN_C_START


// =============================================================================
// FMD_CALLBACK_T
// -----------------------------------------------------------------------------
/// This type describes the callback prototype, this callback is called
/// when a #fmd_Tune operation finishes or when a #fmd_Seek operation finishes.
/// @param freqKHz  current FM channel frequency
/// @param found    whether or not a channel was found (ALWAYS \c TRUE for #fmd_Tune)
/// @param stereo   whether or not the channel is recieved in stereo
// =============================================================================
typedef VOID FMD_CALLBACK_T(UINT32 freqKHz, BOOL found, BOOL stereo);


// =============================================================================
// FMD_INIT_T
// -----------------------------------------------------------------------------
/// This type describes the initial values for #fmd_Open
// =============================================================================
typedef struct
{
    /// FM Frequency
    UINT32              freqKHz;
    /// Analog Volume
    FMD_ANA_LEVEL_T     volume;
    /// Enable/Disable Bass Boost
    BOOL                bassBoost;
    /// Enable/Disable Mono output of stereo radios
    BOOL                forceMono;
} FMD_INIT_T;

// =============================================================================
// FMD_I2S_CFG_T
// -----------------------------------------------------------------------------
/// This type describes the initial values for #fmd_I2sOpen
// =============================================================================
typedef struct
{

    /// If TRUE, FM is used as the slave
    BOOL                slave;

    /// Configure LRCK polarity.
    ///0 = high level on LRCK means left channel, low level means right.
    ///1 = high level on LRCK means right channel, low level means left.
    BOOL                polarity;

    ///0 = use normal sclk internally
    ///1 = inverted sclk internally
    BOOL                edge;

    /// I2S LRCK Frequency
    FMD_FREQ_T          freq;

    ///1 = Output signed 16-bit audio data
    BOOL                dataSigned;

    ///1 = Invert LRCK when master
    BOOL                wsInv;

    ///1 = Invert SCLK when master
    BOOL                slckInv;

    ///1 = Left channel data delay 1T
    BOOL                leftDelay;

    ///1 = Right channel data delay 1T
    BOOL                rightDelay;

} FMD_I2S_CFG_T;

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// fmd_Open
// -----------------------------------------------------------------------------
/// This function opens the FM driver and activate the FM chip.
/// @param fmdTgtCfg    configuration from TGT module, refer to documentation
///                     of target module for details.
///                     (nust not be \c NULL)
/// @param band         band selection
/// @param callback     callback for #fmd_Tune and #fmd_Seek
///                     (can be \c NULL)
/// @param initValues   initial channel & sound configuration
///                     (can be \c NULL, in that case, the FM output is muted)
/// @return             \c FMD_ERR_NO, \c FMD_ERR_ALREADY_OPENED
///                     \c FMD_ERR_BAD_PARAMETER or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Open(CONST TGT_FMD_CONFIG_T* fmdTgtCfg,
                          FMD_BAND_T band,
                          FMD_CALLBACK_T* callback,
                          FMD_INIT_T* initValues);


// =============================================================================
// fmd_Close
// -----------------------------------------------------------------------------
/// This function closes the FM driver and desactivate the FM chip
/// and stops any pending #fmd_Tune or #fmd_Seek operation.
/// @return             \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Close(VOID);


// =============================================================================
// fmd_Tune
// -----------------------------------------------------------------------------
/// This function tunes the FM channel to the desired frequency.
/// Once the tune operation has locked the channel, the callback registered
/// at #fmd_Open will be called.
/// @param freqKHz  FM frequency
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Tune(UINT32 freqKHz);


// =============================================================================
// fmd_Seek
// -----------------------------------------------------------------------------
/// This function seeks to the next FM channel.
/// Once the seek operation has found a channel, the callback registered
/// at #fmd_Open will be called.
/// @param seekUp   \c TRUE : increasing frequency, \c FALSE: decreasing frequency
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_Seek(BOOL seekUp);


// =============================================================================
// fmd_I2sOpen
// -----------------------------------------------------------------------------
/// This function open the I2S interface
/// @param fmdI2sCfg I2S configuration
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_I2sOpen(FMD_I2S_CFG_T fmdI2sCfg);


// =============================================================================
// fmd_I2sClose
// -----------------------------------------------------------------------------
/// This function close the I2S interface
/// @return         \c FMD_ERR_NO, or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_I2sClose(VOID);


// =============================================================================
// fmd_SetVolume
// -----------------------------------------------------------------------------
/// This function sets the volume and other audio related paramters.
/// @param volume       Analog Volume
/// @param bassBoost    Enable/Disable Bass Boost
/// @param forceMono    Enable/Disable Mono output of stereo radios
/// @return             \c FMD_ERR_NO or \c FMD_ERR_RESOURCE_BUSY
// =============================================================================
PUBLIC FMD_ERR_T fmd_SetVolume(FMD_ANA_LEVEL_T volume,
                               BOOL bassBoost,
                               BOOL forceMono);


// =============================================================================
// fmd_GetFreq
// -----------------------------------------------------------------------------
/// @param pFreqKHz     pointer to a UINT32 receiving the channel frequency
/// @return             \c FMD_ERR_NO, \c FMD_ERR_RESOURCE_BUSY or
///                     \c FMD_ERR_BAD_PARAMETER if \p pFreqKHz is \c NULL
// =============================================================================
PUBLIC FMD_ERR_T fmd_GetFreq(UINT32 *pFreqKHz);


// =============================================================================
// fmd_GetStereoStatus
// -----------------------------------------------------------------------------
/// @param pStereoStatus    pointer to a BOOL receiving the stereo status:
///                         \c TRUE the channel is received in stereo,
///                         \c FALSE the channel is recieved in mono.
/// @return                 \c FMD_ERR_NO, \c FMD_ERR_RESOURCE_BUSY or
///                         \c FMD_ERR_BAD_PARAMETER if \p pStereoStatus is \c NULL
// =============================================================================
PUBLIC FMD_ERR_T fmd_GetStereoStatus(BOOL *pStereoStatus);


// =============================================================================
// fmd_ValidStop
// -----------------------------------------------------------------------------
/// @param freqKHz      the channel frequency.
/// @return             \c TRUE if a channel is found or
///                     \c FALSE otherwise
// =============================================================================
PUBLIC BOOL fmd_ValidStop(UINT32 freqKHz);


// =============================================================================
// fmd_GetRssi
// -----------------------------------------------------------------------------
/// @param pRSSI        pointer to a UINT32 receiving a measure of the channel quality, should not be used as the meaning of this is dependant on the actual FM chip.
/// @return             \c FMD_ERR_NO, \c FMD_ERR_RESOURCE_BUSY or
///                     \c FMD_ERR_BAD_PARAMETER if \p pRSSI is \c NULL
// =============================================================================
DEPRECATED PUBLIC FMD_ERR_T fmd_GetRssi(UINT32* pRSSI);


// =============================================================================
// fmd_GetRssiThreshold
// -----------------------------------------------------------------------------
/// @return             a Threshold used to compare with RSSI measure of the channel quality, only useful with fmd_GetRssi.
// =============================================================================
DEPRECATED PUBLIC UINT32 fmd_GetRssiThreshold(VOID);


EXTERN_C_END

///  @} <- End of the fmd group

#endif // _FMD_M_H_


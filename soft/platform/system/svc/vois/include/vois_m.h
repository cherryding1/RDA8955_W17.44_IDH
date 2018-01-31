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



#ifndef _VOIS_M_H_
#define _VOIS_M_H_

#include "cs_types.h"

#include "hal_aif.h"
#include "aud_m.h"


/// @page vois_mainpage  VOIce Service API
/// @mainpage VOIce Service API
///
/// This service provides the sound management for the phone calls. It takes care of the Rx
/// decoding, the Tx encoding, the configuration of the audio interfaces in a fully
/// integrated way. The only things to do is to start this service when needed
/// (#vois_Start), to stop it when it becomes unneeded (#vois_Stop), and configure on
/// the run when needed (#vois_Setup).
///
/// @par Configuration requirements
/// HAL must be poperly configured in order to use the aps service.
///
/// The API is detailed in the following group: @ref vois
///
/// @par Organisation
/// @image latex vois.png
/// @image html vois.png
///
/// @defgroup vois VOIce Service (VOIS)
/// @{
///

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================




// =============================================================================
// VOIS_ERR_T
// -----------------------------------------------------------------------------
/// Error types for the VOIS module.
// =============================================================================
typedef enum
{
    /// No error occured
    VOIS_ERR_NO,

    /// An attempt to access a busy resource failed
    /// (Resource use not possible)
    VOIS_ERR_RESOURCE_BUSY,

    /// Attempt to open a resource already opened,
    /// (no need to open again to use).
    VOIS_ERR_ALREADY_OPENED,

    /// Timeout while trying to access the resource
    VOIS_ERR_RESOURCE_TIMEOUT,

    /// Invalid parameter
    VOIS_ERR_BAD_PARAMETER,

    /// The specified interface does not exist
    VOIS_ERR_NO_ITF,

    /// What ?
    VOIS_ERR_UNKNOWN,


    VOIS_ERR_QTY
} VOIS_ERR_T;




// =============================================================================
// VOIS_AUDIO_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure.
///
/// A configuration structure allows to record a stream with the proper configuration
/// set as far as the audio interface and the decoding are concerned.
// =============================================================================
typedef struct
{
    /// Speaker level.
    AUD_SPK_LEVEL_T spkLevel;

    /// Mic level
    AUD_MIC_LEVEL_T micLevel;

    /// Side tone
    AUD_SIDE_LEVEL_T sideLevel;

    /// Tone level
    AUD_TONE_ATTENUATION_T toneLevel;

    /// encoder gain
    INT16 encMute;

    /// decoder gain
    INT16 decMute;

} VOIS_AUDIO_CFG_T;


// =============================================================================
// VOIS_STATUS_T
// -----------------------------------------------------------------------------
/// Status returned by the VOIS to the calling entity. The possible value
/// describes various information about the status of the play.
// =============================================================================
typedef enum
{
    VOIS_STATUS_MID_BUFFER_REACHED,
    VOIS_STATUS_END_BUFFER_REACHED,
    VOIS_STATUS_NO_MORE_DATA,
    VOIS_STATUS_ERR,

    VOIS_STATUS_QTY
} VOIS_STATUS_T;

typedef enum
{
    VOIS_LOOP_NONE,
    VOIS_LOOP_NORMAL,
    VOIS_LOOP_DEALY,
    VOIS_LOOP_SAMPLE,
    VOIS_LOOP_QTY
} VOIS_LOOP_T;


typedef enum
{   
    MIXVOISE_STATE_IDLE,
    MIXVOISE_STATE_MIXLOCAL,
    MIXVOISE_STATE_MIXREMOTE,
    MIXVOISE_STATE_MIXALL,
}MIXVOISE_STATE;

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// vois_Open
// -----------------------------------------------------------------------------
/// Open the VOIS service. Must be called before any other VOIS function.
/// This function registers the VoiS context in HAL map engine.
// =============================================================================
PUBLIC VOID vois_Open(VOID);



// =============================================================================
// vois_Setup
// -----------------------------------------------------------------------------
/// Configure the VOIS service..
///
/// This functions configures the playing of the Rx stream and te recording of
/// the Tx stream on a given audio interface:
/// gain for the side tone, the microphone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker...
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #VOIS_AUDIO_CFG_T for more details.
/// @return #VOIS_ERR_NO, the configuration being applied.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Setup(AUD_ITF_T itf, CONST VOIS_AUDIO_CFG_T* cfg);



// =============================================================================
// vois_Start
// -----------------------------------------------------------------------------
/// Start the VOIS service.
///
/// This function records from the mic and outputs sound on the speaker
/// on the audio interface specified as a parameter, using the input and output
/// set by the \c parameter. \n
///
/// @param itf Interface providing the audion input and output.
/// @param cfg The configuration set applied to the audio interface
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #VOIS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Start(
    CONST AUD_ITF_T      itf,
    CONST VOIS_AUDIO_CFG_T* cfg);




// =============================================================================
// vois_Stop
// -----------------------------------------------------------------------------
/// This function stops the VOIS service.
/// If the function returns
/// #VOIS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #VOIS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Stop(VOID);


// =============================================================================
// vois_CalibUpdateValues
// -----------------------------------------------------------------------------
/// Update Vois related values depending on calibration parameters.
///
/// @return #VOIS_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #VOIS_ERR_NO otherwise
// =============================================================================
PUBLIC VOIS_ERR_T vois_CalibUpdateValues(VOID);


// =============================================================================
// VOIS_USER_HANDLER_T
// -----------------------------------------------------------------------------
/// To give back status to the calling task, the VOIS needs to be given a
/// callback function. This function is called whenever an event occurs
/// which needs to be reported back to the VOIS used. This status is reported
/// as the parameter of the function. The function is passed as a parameter to
/// the function #ars_Record. This type is the type describing such a function.
///
/// Note: This function must be as minimalist as possible, as it will be
/// executed during an interrupt. It should for example only send one event
/// to a task, asking for data refilling of the buffer. The data copy is
/// then done by the task, when it is rescheduled, but not directly by the
/// user handler.
// =============================================================================
typedef VOID (*VOIS_USER_HANDLER_T)(VOIS_STATUS_T);


PUBLIC VOIS_ERR_T vois_RecordStart( INT32 *startAddress,INT32 length,VOIS_USER_HANDLER_T handler);
PUBLIC VOIS_ERR_T vois_RecordStop( VOID);
PUBLIC VOID vois_SetRxToTxLoopFlag( BOOL flag);

#ifdef VOIS_DUMP_PCM
PUBLIC VOID vois_DumpPcmDataToTFlashProcess(VOID);
PUBLIC VOID  vois_DumpPcmDataToTFlashStart(VOID);
PUBLIC VOID vois_DumpPcmDataToTFlashFileOpen(VOID);
PUBLIC VOID  vois_DumpPcmDataToTFlashStop(VOID);
#endif


///  @} <- End of the aps group

// =============================================================================
// vois_GetLatestCfg
// -----------------------------------------------------------------------------
/// get the vois Latest config.
// =============================================================================
PUBLIC VOIS_AUDIO_CFG_T* vois_GetAudioCfg(VOID);

PUBLIC VOID vois_SetMsdfReloadFlag(VOID);

PUBLIC AUD_ERR_T vois_TestModeSetup(CONST SND_ITF_T         itf,
                                    CONST HAL_AIF_STREAM_T* stream,
                                    CONST AUD_LEVEL_T *     cfg,
                                    AUD_TEST_T              mode);

#endif // _VOIS_M_H_


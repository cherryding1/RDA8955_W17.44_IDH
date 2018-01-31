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

















#ifndef VPP_AMJR_H
#define VPP_AMJR_H

#include "cs_types.h"
#include "hal_voc.h"


///@defgroup vpp_amjr
///
///  This document describes the characteristics of the VPP AMJR module.
///
///  @par Processing configuration :
///  @par
///         The VPP AMJR is composed of different audio codecs (MP3, AMR, ...) and an mjpeg encoder.
///         It can also be selected whether the frame operation is triggered (started) on a
///         software-generated event (by XCPU) or on a hardware-generated event (by IFC).
///
///  @par Resource management :
///  @par
///         The VPP AMJR module needs to be first opened and configured using function
///         <b> \c vpp_AmjrOpen </b>. This function will also set the audio configuration
///         to a default value. When the module is no longer needed it should be closed using
///         <b> \c vpp_AmjrClose </b>. Not closing the module would not free the VoC resource
///         for other applications.
///
///  @par Audio configuration :
///  @par
///         Use the function <b> \c vpp_AmjrScheduleOneFrame </b>
///
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_AMJR_AUDIO_CFG_T
// ----------------------------------------------------------------------------
/// VPP AMJR audio configuration structure
// ============================================================================

typedef struct
{
    INT8   mode;
    INT16  reset;
    INT32* inBufAddr;
    INT32* outBufAddr;
    INT32  sampleRate;
    INT16  bitRate;
    INT16  channelNum;
    INT32* ifcStatusPtr;

} VPP_AMJR_AUDIO_CFG_T;


// ============================================================================
// VPP_AMJR_VIDEO_CFG_T
// ----------------------------------------------------------------------------
/// VPP AMJR video configuration structure
// ============================================================================

typedef struct
{
    // 1 For MJPEG
    UINT8   mode;
    // FIXME Check we don't need to set this parameter.
    UINT32* inBufAddr;
    // FIXME Check we don't need to set this parameter.
    UINT32* outBufAddr;
    UINT16  width;
    UINT16  height;
    UINT16  quality;
    // 0 = RGB; 1=YUV
    UINT16  format;
    // Swap pixel position in every 32-bit word
    UINT16  pxlSwap;
    // FIXME Check we don't need to set this parameter.
    UINT32* previewBufAddr;
    UINT16  previewWidth;
    UINT16  previewHeight;
    // 0 = RGB; 1=YUV
    UINT16  previewFormat;
    UINT16  previewScaleFactor;

} VPP_AMJR_VIDEO_CFG_T;


// ============================================================================
// VPP_AMJR_STATUS_T
// ----------------------------------------------------------------------------
/// VPP AMJR status structure
// ============================================================================

//struct of output parameters
typedef struct
{
    // ----------------
    // common status
    // ----------------
    INT8  audioMode;
    INT8  videoMode;
    INT16 errorStatus;
    INT16 strmStatus;
    INT16 reserved;

    // ----------------
    // audio status
    // ----------------
    INT32 outputLen;
    INT32 consumedLen;
    INT32 inStreamBufIfcStatus;

    // ----------------
    // video status
    // ----------------
    INT32 imagOutputLen;

} VPP_AMJR_STATUS_T;


// ============================================================================
// VPP_AMJR_AUDIO_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJR audio codec mode enumerator
// ============================================================================

typedef enum
{
    VPP_AMJR_AUDIO_MODE_NO   =  0,
    VPP_AMJR_AUDIO_MODE_AMR  =  1,
    VPP_AMJR_AUDIO_MODE_MP3  =  2,
    VPP_AMJR_AUDIO_MODE_AAC  =  3,
    VPP_AMJR_AUDIO_MODE_WMA  =  4

} VPP_AMJR_AUDIO_MODE_T;


// ============================================================================
// VPP_AMJR_STRM_ID_T
// ----------------------------------------------------------------------------
/// VPP AMJR stream id enumerator
// ============================================================================

typedef enum
{
    VPP_AMJR_STRM_ID_NO    =  0x0,
    VPP_AMJR_STRM_ID_AUDIO =  0x1,
    VPP_AMJR_STRM_ID_VIDEO =  0x2,
    VPP_AMJR_STRM_ID_INIT  =  0x3,

} VPP_AMJR_STRM_ID_T;


// ============================================================================
// VPP_AMJR_VIDEO_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJR video codec mode enumerator
// ============================================================================

typedef enum
{
    VPP_AMJR_VIDEO_MODE_NO   =  0,
    VPP_AMJR_VIDEO_MODE_JPEG =  1

} VPP_AMJR_VIDEO_MODE_T;


// ============================================================================
// VPP_AMJR_WAKEUP_MODE_T
// ----------------------------------------------------------------------------
/// Defines the order in which the tasks are executed an the way these
/// tasks are started
// ============================================================================

typedef enum
{
    /// No wakeup enabled
    VPP_AMJR_WAKEUP_NO = 0,
    /// Wake on the audio software event only (SOF0).
    /// This mode is used to for audio-only recording
    /// or for audio+video recording with priority to
    /// the audio (the video frame rate should not
    /// be superior to the audio frame rate).
    VPP_AMJR_WAKEUP_SW_AUDIO_ONLY,
    /// Wake on the audio ifc event only (IFC0).
    /// Same as the previous, but the VoC is waken on
    /// an IFC0 half or end buffer event.
    VPP_AMJR_WAKEUP_HW_AUDIO_ONLY,
    /// Wake both on the audio software event (SOF0)
    /// and on the video software event (SOF1).
    /// This mode is used for audio+video recording
    /// without priorities. It may lead to audio frame
    /// skipping, so scheduling the video frame recording
    /// should be cautiously controlled.
    VPP_AMJR_WAKEUP_SW_AUDIO_SW_VIDEO,
    /// Wake both on the audio ifc event (IFC0)
    /// and on the video software event (SOF1).
    /// Same as the previous, but the VoC is waken on
    /// an IFC0 half or end buffer event.
    VPP_AMJR_WAKEUP_HW_AUDIO_SW_VIDEO,
    /// Wake on the video software event (SOF1).
    /// This mode is used for video-only (or still
    /// picture) recording.
    VPP_AMJR_WAKEUP_SW_VIDEO_ONLY

} VPP_AMJR_WAKEUP_MODE_T;



// ============================================================================
// VPP_AMJR_JPEG_QUALITY_T
// ----------------------------------------------------------------------------
/// VPP AMJR Jpeg encoder output quality
// ============================================================================

typedef enum
{
    /// Normal quality (better compression)
    VPP_AMJR_JPEG_QUALITY_NORMAL = 0,
    /// Superior quality (worse compression)
    VPP_AMJR_JPEG_QUALITY_SUPERIOR = 1

} VPP_AMJR_JPEG_QUALITY_T;


// ============================================================================
// VPP_AMJR_BMP_FORMAT_T
// ----------------------------------------------------------------------------
/// VPP AMJR format of the input bitmap image
// ============================================================================

typedef enum
{
    /// RGB color pixels packed on 16 bits
    VPP_AMJR_BMP_RGB = 0,
    /// YUV color pixels packed on 16 bits
    VPP_AMJR_BMP_YUV = 1

} VPP_AMJR_BMP_FORMAT_T;


// ============================================================================
// VPP_AMJR_PXL_ORDER_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJR pixel order in the input bitmap image
// ============================================================================

typedef enum
{
    /// normal bitmap
    VPP_AMJR_PXL_ORDER_NORMAL = 0,
    /// swap the odd & even indexes
    VPP_AMJR_PXL_ORDER_SWAP = 1

} VPP_AMJR_PXL_ORDER_MODE_T;


// ============================================================================
// VPP_AMJR_NB_CHAN_T
// ----------------------------------------------------------------------------
/// VPP AMJR number of audio channels
// ============================================================================

typedef enum
{
    /// Mono (1 channel)
    VPP_AMJR_MONO = 1,
    /// Stereo (2 channels)
    VPP_AMJR_STEREO = 2

} VPP_AMJR_NB_CHAN_T;


// ============================================================================
// VPP_AMJR_ERROR_T
// ----------------------------------------------------------------------------
/// VPP AMJR error enumerator
// ============================================================================

typedef enum
{
    /// No error
    VPP_AMJR_ERROR_NONE                  =  0,
    /// Unknown audio mode
    VPP_AMJR_ERROR_UNKNOWN_AUDIO_MODE    = -1,
    /// Unknown video mode
    VPP_AMJR_ERROR_UNKNOWN_VIDEO_MODE    = -2,
    /// Video processing scheduled but
    /// output buffer pointers are NULL
    VPP_AMJR_ERROR_VIDEO_BAD_PARAMETER   = -3,
    /// Video preview bad parameter
    VPP_AMJR_ERROR_PREVIEW_BAD_PARAMETER = -4,
    /// JPEG encoder bad parameter
    VPP_AMJR_ERROR_JPEG_BAD_PARAMETER    = -5

} VPP_AMJR_ERROR_T;


// ============================================================================
// API Functions
// ============================================================================

//=============================================================================
// vpp_AmjrOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP AMJR code and environment variables. It also set the
/// VoC wakeup masks necessary for the synchronization. This function is
/// called each time an audio service is started.
/// @param vocIrqHandler : user defined handler function for the VoC interrupt.
/// If the function is set to NULL, no interrupts are used (the interrupt mask
/// is not set).
/// @param wakeupMode : choose to encode, decode or both. Select whether the
/// VoC module wakeup is controlled by XCPU or by IFC interrupts.
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_AmjrOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler,
                              VPP_AMJR_WAKEUP_MODE_T wakeupMode);

//=============================================================================
// vpp_AmjrClose function
//-----------------------------------------------------------------------------
/// Close VPP AMJR, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_AmjrClose(VOID);

//=============================================================================
// vpp_AmjrStatus function
//-----------------------------------------------------------------------------
/// Return the VPP AMJR status structure.
/// @param pStatus : result status structure of type VPP_AMJR_STATUS_T
//=============================================================================
PUBLIC VOID vpp_AmjrStatus(VPP_AMJR_STATUS_T * pStatus);

//=============================================================================
// vpp_AmjrAudioCfg function
//-----------------------------------------------------------------------------
/// @param cfg : audio configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_AmjrAudioCfg(VPP_AMJR_AUDIO_CFG_T* pCfg);

//=============================================================================
// vpp_AmjrVideoCfg function
//-----------------------------------------------------------------------------
/// @param cfg : video configuration (codec mode, ...)
//=============================================================================
PUBLIC VOID vpp_AmjrVideoCfg(VPP_AMJR_VIDEO_CFG_T* pCfg);

//=============================================================================
// vpp_AmjrScheduleOneAudioFrame function
//-----------------------------------------------------------------------------
/// @param pIn : pointer to the input buffer
/// @param pOut : pointer to the output buffer
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_AmjrScheduleOneAudioFrame(UINT32* pIn, UINT32* pOut);

//=============================================================================
// vpp_AmjrScheduleOneVideoFrame function
//-----------------------------------------------------------------------------
/// @param pIn : pointer to the input buffer
/// @param pOut : pointer to the coded output buffer
/// @param pPreview : pointer to the preview output buffer
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_AmjrScheduleOneVideoFrame(UINT32* pIn, UINT32* pOut, UINT32* pPreview);

///  @} <- End of the vpp_amjr group


#endif // VPP_AMJR_H

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

















#ifndef VPP_AMJP_H
#define VPP_AMJP_H

#include "cs_types.h"
#include "hal_voc.h"


///@defgroup vpp_amjp
///
///  This document describes the characteristics of the VPP AMJP module.
///
///  @par Processing configuration :
///  @par
///         The VPP AMJP is composed of different audio codecs (MP3, AAC, ...) and an mjpeg decoder.
///         It can also be selected whether the frame operation is triggered (started) on a
///         software-generated event (by XCPU) or on a hardware-generated event (by IFC).
///
///  @par Resource management :
///  @par
///         The VPP Speech module needs to be first opened and configured using function
///         <b> \c vpp_AmjpOpen </b>. This function will also set the audio configuration
///         to a default value. When the module is no longer needed it should be closed using
///         <b> \c vpp_AmjpClose </b>. Not closing the module would not free the VoC resource
///         for other applications.
///
///  @par Audio configuration :
///  @par
///         Use the audio configuration function <b> \c vpp_AmjpCfg </b>, which sets the mode,
///         the reset and the equalizer used during decoding.
///
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_AMJP_AUDIO_CFG_T
// ----------------------------------------------------------------------------
/// VPP AMJP audio configuration structure
// ============================================================================

typedef struct
{
    /// audio mode
    INT8 mode;
    /// global reset
    INT16 reset;
    /// equalizer
    INT16 eqType;
    /// start address of the input stream buffer
    /// (VOC format, burst read)
    INT32* inStreamBufAddrStart;
    /// first address after the last field of the input stream buffer
    /// (VOC format, burst read)
    INT32* inStreamBufAddrEnd;
    /// start address of the output audio buffer
    /// (VOC format, burst write)
    INT32* outStreamBufAddr;
    /// address of the IFC status register containing the current ifc read AHB address
    /// (VOC format, single read)
    INT32* outStreamIfcStatusRegAddr;

} VPP_AMJP_AUDIO_CFG_T;


// ============================================================================
// VPP_AMJP_VIDEO_CFG_T
// ----------------------------------------------------------------------------
/// VPP AMJP video configuration structure
// ============================================================================

typedef struct
{
    /// video mode
    INT8 mode;
    /// zoom
    INT16 zoomMode;
    /// rotate
    INT16 rotateMode;
    /// resize to width (valid if positive and if the native size is bigger)
    INT16 resizeToWidth;
    /// global reset
    INT16 vidBufMode;
    /// start address of the input video buffer
    /// (VOC format, burst read)
    INT32* inVidBufAddrStart;
    /// first address after the last field of the input video buffer
    /// (VOC format, burst read)
    INT32* inVidBufAddrEnd;
    /// start address of the output video buffer
    /// (VOC format, burst write)
    INT32* outVidBufAddr;

    // --------------
    // Extra Video config (for zoom)
    // --------------
    /// source width
    INT16 sourceWidth;
    /// source heigth
    INT16 sourceHeight;
    /// cropped width
    INT16 croppedWidth;
    /// cropped heigth
    INT16 croppedHeight;
    /// zoomed width
    INT16 zoomedWidth;
    /// zoomed heigth
    INT16 zoomedHeight;
    /// start address of the input Y component buffer
    /// (VOC format, burst read)
    INT32* inYBufAddrStart;
    /// start address of the input V component buffer
    /// (VOC format, burst read)
    INT32* inVBufAddrStart;

} VPP_AMJP_VIDEO_CFG_T;


// ============================================================================
// VPP_AMJP_STATUS_T
// ----------------------------------------------------------------------------
/// VPP AMJP status structure
// ============================================================================

#define VPP_AMJP_DISPLAY_BARS_QTY 10

typedef struct
{
    // --------------
    // General status
    // --------------
    /// codec mode
    INT8 audioMode;
    INT8 videoMode;

    /// Error status
    INT16 errorStatus;
    /// Stream status
    INT16 strmStatus;

    // --------------
    // Audio status
    // --------------
    /// Number of channels
    INT16 nbChannel;
    /// Sample rate
    INT32 sampleRate;
    /// Bit rate
    INT16 bitRate;
    /// Display bars (log2 of the inband power)
    UINT8 displayBars[VPP_AMJP_DISPLAY_BARS_QTY];
    /// address of the input audio stream buffer
    /// (VOC format, burst read)
    INT32* inStreamBufAddr;
    /// Current ifc read AHB address at event reception
    INT32* outStreamIfcStatus;

    // --------------
    // Video status
    // --------------
    /// Image width (used by jpeg decoder only)
    INT16 imgWidth;
    /// Image height (used by jpeg decoder only)
    INT16 imgHeight;
    /// address of the input video stream buffer
    /// (VOC format, burst read)
    INT32* inVidBufAddr;

} VPP_AMJP_STATUS_T;


// ============================================================================
// VPP_AMJP_AUDIO_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJP audio codec mode enumerator
// ============================================================================

typedef enum
{
    VPP_AMJP_AUDIO_MODE_NO    =  0x0,
    VPP_AMJP_AUDIO_MODE_AMR   =  0x1,
    VPP_AMJP_AUDIO_MODE_MP12  =  0x2,
    VPP_AMJP_AUDIO_MODE_MP3   =  0x2,
    VPP_AMJP_AUDIO_MODE_AAC   =  0x3,
    VPP_AMJP_AUDIO_MODE_WMA   =  0x4,

} VPP_AMJP_AUDIO_MODE_T;


// ============================================================================
// VPP_AMJP_VIDEO_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJP video codec mode enumerator
// ============================================================================


typedef enum
{
    VPP_AMJP_VIDEO_MODE_NO    =  0x0,
    VPP_AMJP_VIDEO_MODE_JPEG  =  0x1,
    VPP_AMJP_VIDEO_MODE_H263  =  0x2,
    VPP_AMJP_VIDEO_MODE_MPEG4 =  0x3
} VPP_AMJP_VIDEO_MODE_T;


// ============================================================================
// VPP_AMJP_STRM_ID_T
// ----------------------------------------------------------------------------
/// VPP AMJP stream id enumerator
// ============================================================================

typedef enum
{
    VPP_AMJP_STRM_ID_NO    =  0x0,
    VPP_AMJP_STRM_ID_AUDIO =  0x1,
    VPP_AMJP_STRM_ID_VIDEO =  0x2,
    VPP_AMJP_STRM_ID_INIT  =  0x3,

} VPP_AMJP_STRM_ID_T;


// ============================================================================
// VPP_AMJP_WAKEUP_MODE_T
// ----------------------------------------------------------------------------
/// Defines the order in which the tasks are executed an the way these
/// tasks are started

typedef enum
{
    /// No wakeup enabled
    VPP_AMJP_WAKEUP_NO = 0,
    /// Wake on the audio software event only (SOF1).
    /// This mode is used to for audio-only recording
    /// or for audio+video recording with priority to
    /// the audio (the video frame rate should not
    /// be superior to the audio frame rate).
    VPP_AMJP_WAKEUP_SW_AUDIO_ONLY,
    /// Wake on the audio ifc event only (IFC1).
    /// Same as the previous, but the VoC is waken on
    /// an IFC1 half or end buffer event.
    VPP_AMJP_WAKEUP_HW_AUDIO_ONLY,
    /// Wake both on the audio software event (SOF1)
    /// and on the video software event (SOF0).
    /// This mode is used for audio+video recording
    /// without priorities. It may lead to audio frame
    /// skipping, so scheduling the video frame recording
    /// should be cautiously controlled.
    VPP_AMJP_WAKEUP_SW_AUDIO_SW_VIDEO,
    /// Wake both on the audio ifc event (IFC1)
    /// and on the video software event (SOF0).
    /// Same as the previous, but the VoC is waken on
    /// an IFC1 half or end buffer event.
    VPP_AMJP_WAKEUP_HW_AUDIO_SW_VIDEO,
    /// Wake on the video software event (SOF0).
    /// This mode is used for video-only (or still
    /// picture) recording.
    VPP_AMJP_WAKEUP_SW_VIDEO_ONLY

} VPP_AMJP_WAKEUP_MODE_T;


// ============================================================================
// VPP_AMJP_ROTATE_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJP image rotate modes enumerator
// ============================================================================

typedef enum
{
    /// do not rotate image
    VPP_AMJP_ROTATE_NO = 0,
    /// rotate 90 clockwise
    VPP_AMJP_ROTATE_90 = 1,
    /// rotate 180 clockwise
    VPP_AMJP_ROTATE_180 = 2,
    /// rotate 270 clockwise
    VPP_AMJP_ROTATE_270 = 3

} VPP_AMJP_ROTATE_MODE_T;


// ============================================================================
// VPP_AMJP_EQUALIZER_MODE_T
// ----------------------------------------------------------------------------
/// VPP AMJP equalizer modes enumerator
// ============================================================================

typedef enum
{
    VPP_AMJP_EQUALIZER_OFF       = -1,
    VPP_AMJP_EQUALIZER_NORMAL    =  0,
    VPP_AMJP_EQUALIZER_BASS      =  1,
    VPP_AMJP_EQUALIZER_DANCE     =  2,
    VPP_AMJP_EQUALIZER_CLASSICAL =  3,
    VPP_AMJP_EQUALIZER_TREBLE    =  4,
    VPP_AMJP_EQUALIZER_PARTY     =  5,
    VPP_AMJP_EQUALIZER_POP       =  6,
    VPP_AMJP_EQUALIZER_ROCK      =  7,
    VPP_AMJP_EQUALIZER_CUSTOM    =  8,
    VPP_AMJP_EQUALIZER_QTY

} VPP_AMJP_EQUALIZER_MODE_T;


// ============================================================================
// VPP_AMJP_ZOOM_MODE_T
// ----------------------------------------------------------------------------
/// Zoom mode enumerator
// ============================================================================

typedef enum
{
    /// No zoom (original size)
    VPP_AMJP_ZOOM_NO = 0,
    /// Size set to 320x240
    VPP_AMJP_ZOOM_320x240 = 1,
    /// Size set to 160x120
    VPP_AMJP_ZOOM_160x120 = 2

} VPP_AMJP_ZOOM_MODE_T;


// ============================================================================
// VPP_AMJP_BUF_MODE_T
// ----------------------------------------------------------------------------
/// Buffer management mode enumerator
// ============================================================================

typedef enum
{
    /// swap mode
    VPP_AMJP_BUF_MODE_SWAP = 1,
    /// circular mode
    VPP_AMJP_BUF_MODE_CIRC = 0

} VPP_AMJP_BUF_MODE_T;

// ============================================================================
// VPP_AMJP_STREAM_STATUS_T
// ----------------------------------------------------------------------------
/// VPP AMJP stream status enumerator
// ============================================================================

typedef enum
{
    /// continue normally
    VPP_AMJP_STREAM_CONTINUE = 0x0000,
    /// stop decoding normally
    VPP_AMJP_STREAM_STOP     = 0x0010,
    /// stop decoding and signal an error
    VPP_AMJP_STREAM_BREAK    = 0x0011,
    /// ignore the current frame
    VPP_AMJP_STREAM_IGNORE   = 0x0020

} VPP_AMJP_STREAM_STATUS_T;


// ============================================================================
// VPP_AMJP_ERR_STATUS_T
// ----------------------------------------------------------------------------
/// VPP AMJP error status enumerator for AAC
// ============================================================================

typedef enum
{
    VPP_AMJP_ERR_NONE                        =   0,
    VPP_AMJP_ERR_INDATA_UNDERFLOW            =  -1,
    VPP_AMJP_ERR_NULL_POINTER                =  -2,
    VPP_AMJP_ERR_INVALID_ADTS_HEADER         =  -3,
    VPP_AMJP_ERR_INVALID_ADIF_HEADER         =  -4,
    VPP_AMJP_ERR_INVALID_FRAME               =  -5,
    VPP_AMJP_ERR_MPEG4_UNSUPPORTED           =  -6,
    VPP_AMJP_ERR_CHANNEL_MAP                 =  -7,
    VPP_AMJP_ERR_SYNTAX_ELEMENT              =  -8,
    VPP_AMJP_ERR_DEQUANT                     =  -9,
    VPP_AMJP_ERR_STEREO_PROCESS              = -10,
    VPP_AMJP_ERR_PNS                         = -11,
    VPP_AMJP_ERR_SHORT_BLOCK_DEINT           = -12,
    VPP_AMJP_ERR_TNS                         = -13,
    VPP_AMJP_ERR_IMDCT                       = -14,
    VPP_AMJP_ERR_NCH_TOO_HIGH                = -15,
    VPP_AMJP_ERR_SBR_INIT                    = -16,
    VPP_AMJP_ERR_SBR_BITSTREAM               = -17,
    VPP_AMJP_ERR_SBR_DATA                    = -18,
    VPP_AMJP_ERR_SBR_PCM_FORMAT              = -19,
    VPP_AMJP_ERR_SBR_NCH_TOO_HIGH            = -20,
    VPP_AMJP_ERR_SBR_UNSUPPORTED             = -21,
    VPP_AMJP_ERR_RAWBLOCK_PARAMS             = -22,
    VPP_AMJP_ERR_MODE                        = -23,
    VPP_AMJP_ERR_UNKNOWN                     = -9999

} VPP_AMJP_ERR_STATUS_T;


// ============================================================================
// API Functions
// ============================================================================

//=============================================================================
// vpp_AmjpOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP AMJP code and environment variables. It also set the
/// VoC wakeup masks necessary for the synchronization. This function is
/// called each time an audio service is started.
/// @param vocIrqHandler : user defined handler function for the VoC interrupt.
/// If the function is set to NULL, no interrupts are used (the interrupt mask
/// is not set).
/// @param wakeupMode : choose to encode, decode or both. Select whether the
/// VoC module wakeup is controlled by XCPU or by IFC interrupts.
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_AmjpOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler,
                              VPP_AMJP_WAKEUP_MODE_T wakeupMode);

//=============================================================================
// vpp_AmjpClose function
//-----------------------------------------------------------------------------
/// Close VPP AMJP, clear VoC wakeup masks. This function is
/// called each time a stream is stopped.
//=============================================================================
PUBLIC VOID vpp_AmjpClose(VOID);

//=============================================================================
// vpp_AmjpStatus function
//-----------------------------------------------------------------------------
/// Return the VPP AMJP status structure.
/// @param pStatus : result status structure of type VPP_AMJP_STATUS_T
//=============================================================================
PUBLIC VOID vpp_AmjpStatus(VPP_AMJP_STATUS_T * pStatus);

//=============================================================================
// vpp_AmjpAudioCfg function
//-----------------------------------------------------------------------------
/// @param cfg : audio configuration (audio mode, equalizer, reset...)
//=============================================================================
PUBLIC VOID vpp_AmjpAudioCfg(VPP_AMJP_AUDIO_CFG_T* pCfg);

//=============================================================================
// vpp_AmjpVideoCfg function
//-----------------------------------------------------------------------------
/// @param cfg : audio configuration (video mode...)
//=============================================================================
PUBLIC VOID vpp_AmjpVideoCfg(VPP_AMJP_VIDEO_CFG_T* pCfg);

//=============================================================================
// vpp_AmjpScheduleAudioOneFrame function
//-----------------------------------------------------------------------------
/// @param pOut : pointer to the output PCM buffer
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_AmjpScheduleOneAudioFrame(INT32* pOut);

//=============================================================================
// vpp_AmjpScheduleOneFrame function
//-----------------------------------------------------------------------------
/// @param pInStart : pointer to the input coded video buffer start
/// @param pOut : pointer to the output video BMP buffer
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_AmjpScheduleOneVideoFrame(INT32* pInStart, INT32* pOut);


///  @} <- End of the vpp_amjp group


#endif  // VPP_AMJP_H

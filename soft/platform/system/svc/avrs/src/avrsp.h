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


#ifndef _AVRSP_H_
#define _AVRSP_H_

#include "cs_types.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "avrs_m.h"
#include "avrsp_debug.h"

#include "aud_m.h"
#include "imgs_m.h"

#include "vpp_amjr.h"
#include "vpp_speech.h"

// =============================================================================
//  MACROS
// =============================================================================
/// Number of trames before unmuting AUD
#define AVRS_AUD_UNMUTE_TRAME_NB 2

/// Number of trames before unmuting VPP
#define AVRS_VPP_UNMUTE_TRAME_NB 2

/// Send a message to the recorded mailbox
/// holding the current buffer position.
#define AVRS_SEND_CUR_POS_EVT \
                {\
                    AVRS_EVENT_T ev;\
                    ev.id           = AVRS_EVENT_ID_AUDIO;\
                    ev.bufferPos    = (UINT32*)(g_avrsBufferCurPos);\
                    ev.frameSize    = 0;\
                    ev.reserved[0]  = 0;\
                    sxr_Send(&ev, g_avrsAudioMbx, SXR_SEND_EVT);\
                }


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Start address of the stream buffer
EXPORT PROTECTED UINT32* g_avrsBufferStart;

/// End address of the stream buffer
EXPORT PROTECTED UINT32 g_avrsBufferLength;

/// End address of the stream buffer
EXPORT PROTECTED UINT32* g_avrsBufferTop;

/// Current position in the buffer. Next read data will be
/// read from here.
EXPORT PROTECTED UINT32* g_avrsBufferCurPos;

/// Number of bytes of data to read from the buffer
EXPORT PROTECTED UINT32 g_avrsBufferAvailBytes;

/// Pointer to the ouput (encoded) buffer of VPP part of VPP decoding buffer.
EXPORT PROTECTED UINT32* g_avrsEncOutput;

/// Number of played trames
EXPORT PROTECTED UINT32 g_avrsTramePlayedNb;

/// Number of trames dropped
EXPORT PROTECTED UINT32 g_avrsTrameDroppedNb;

/// User handler to call with play status is stored here.
EXPORT PROTECTED UINT8 g_avrsAudioMbx;


/// Audio configuration used to play the stream in normal state.
/// (ie not when started or stopped.
EXPORT PROTECTED AUD_LEVEL_T g_avrsAudioCfg;


/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped.
EXPORT PROTECTED VPP_SPEECH_AUDIO_CFG_T g_avrsVppCfg;

/// Interface used to play the current stream.
EXPORT PROTECTED AUD_ITF_T g_avrsItf;

/// Index in the swap buffer (for PCM) which is the one where to copy
/// the new data
EXPORT PROTECTED UINT32 g_avrsSwIdx     ;


/// Local buffer (TODO will become voc pcm's buffer)
EXPORT PROTECTED UINT32 g_avrsPcmBuf[2*576*2];


/// Number of bytes composing one trame for the codec currently
/// used
EXPORT PROTECTED UINT32 g_avrsFrameSize ;


/// Handshake to prevent the same message to be sent several times
/// When a message is sent, this variable is set to FALSE.
/// When data are added, it is set to TRUE.
EXPORT PROTECTED BOOL g_avrsBufferingMsgReceived;


/// Video preview configuration structure.
EXPORT PROTECTED AVRS_DECODED_PICTURE_T g_avrsPreviewCfg;


/// Video preview configuration structure.
EXPORT PROTECTED AVRS_ENCODED_PICTURE_T g_avrsVideoFrameCfg;


/// Are we recording video ?
EXPORT PROTECTED BOOL g_avrsVideoEnabled;

/// Are we recording audio ?
EXPORT PROTECTED BOOL g_avrsAudioEnabled;

/// Latest encoded frame pointer.
EXPORT PROTECTED UINT8* g_avrsLatestEncodedFrame;

/// Latest encoded frame length.
EXPORT PROTECTED UINT32 g_avrsLatestEncodedFrameLength;


// Pointer to the first half of the PCM buffer
EXPORT PROTECTED UINT32* g_avrsPcmBuf_0;

// Pointer to the second half of the PCM buffer
EXPORT PROTECTED UINT32* g_avrsPcmBuf_1;


// Pointer to the first half of the encoded buffer, when using an encoded swap buffer.
EXPORT PROTECTED UINT32* g_avrsEncBuf_0;

// Pointer to the second half of the PCM buffer, when using an encoded swap buffer.
EXPORT PROTECTED UINT32* g_avrsEncBuf_1;

/// Video stream being produced by AVRS
EXPORT PROTECTED AVRS_ENC_VIDEO_STREAM_T g_avrsVideoStream;

/// Audio stream being produced by AVRS
EXPORT PROTECTED AVRS_ENC_AUDIO_STREAM_T g_avrsAudioStream;

/// Last set video configuration.
EXPORT PROTECTED AVRS_VIDEO_CFG_T  g_avrsVideoCfg;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// avrs_XXXPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the XXX module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PROTECTED VOID avrs_XXXPcmHandler(VOID);


// =============================================================================
// avrs_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PROTECTED VOID avrs_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* status);


// =============================================================================
// avrs_FreeStreams
// -----------------------------------------------------------------------------
/// This function frees the global variables used in the streams (video and
/// audio) management.
/// Call it only after both video and audio related processes are finished,
/// and none of the involved variables are still in use.
///
/// @return #AVRS_ERR_NO if it can execute the command.
// =============================================================================
PROTECTED AVRS_ERR_T avrs_FreeStreams(VOID);



#endif // _AVRSP_H_



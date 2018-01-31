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

#define VPP_WMA_DEC_CODE_M_SIZE 4080*4
#define VPP_WMA_DEC_CODE_H_SIZE 3380*4
#define VPP_WMA_DEC_CODE_L_SIZE 5200*4
#define VPP_WMA_DEC_ROM_BUFFER_X_SIZE 708*4
#define VPP_WMA_DEC_ROM_BUFFER_Y_SIZE 992*4
#define VPP_WMA_DEC_STREAM_BUF_SIZE 1024*4

#define VPP_WMA_DEC_CODE_ENTRY 0
#define VPP_WMA_DEC_CRITICAL_SECTION_MIN 0
#define VPP_WMA_DEC_CRITICAL_SECTION_MAX 0

#define WMA_OPEN_VOC            1
#define WMA_OPEN_VOC_NO     0



//#define VPP_WMA_DEC_STREAM_BUF_SIZE
/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2006, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
///
///   @file vpp_WMA_dec.h
///   This file defines VPP WMA Decoder API structures and functions.
///

#ifndef VPP_WMA_DEC_H
#define VPP_WMA_DEC_H

#include "cs_types.h"
#include "hal_voc.h"
#include "hal_error.h"

///
///  @defgroup vpp_WMA_dec VPP WMA Decoder API
///  @par
///  This document describes the characteristics of the VPP WMA Decoder module.
///
///  @par Processing configuration :
///  @par
///         The VPP WMA Decoder module is used to decode an WMA coded stream and output it to a
///         PCM buffer. It can be selected whether the frame operation is triggered (started) on
///         a software-generated event (by XCPU) or on a hardware-generated event (by IFC).
///         HW events are not available on Lily.
///
///  @par Resource management :
///  @par
///         The VPP WMA Decoder module needs to be first opened and configured using function
///         <b> \c vpp_WMADecOpen </b>. When the module is no longer needed it should be closed
///         using <b> \c vpp_WMADecClose </b>. Not closing the module would not free the VoC
///         resource for other applications.
///
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_WMA_DEC_STREAM_STATUS
// ----------------------------------------------------------------------------
/// Status after the last frame processing
// ============================================================================

typedef enum
{
    /// continue normally
    VPP_cWMA_NoErr = 0x0000,
    /// stop decoding normally
    VPP_cWMA_NoMoreDataThisTime = 0x0001,
    /// stop decoding normally
    VPP_cWMA_Start = 0x0010,

} VPP_WMA_DEC_STREAM_STATUS;

// ============================================================================
// VPP_WMA_DEC_IN_T
// ----------------------------------------------------------------------------
/// Defines the in/out stream buffers and the reset.
// ============================================================================
typedef struct
{
    /// Pointer to the input stream data
    int32* inStreamBufAddr;
    /// Pointer to the output stream data (pcm buffer)
    int32* outStreamBufAddr;
    /// Global reset
    short  reset;
//  short  reserved;

//  short  reserved;

} VPP_WMA_DEC_IN_T;

// ============================================================================
// VPP_WMA_DEC_OUT_T
// ----------------------------------------------------------------------------
/// Gives the status after the last frame processing and the mode of the last
/// processed frame.
// ============================================================================
typedef struct
{
    /// Length of the consumed data from the input stream in bytes
    int32 consumedLen;
    int32 inputOffset;
    int32 iSamplingRate;
    /// WMA decoder mode (coding rate, sample rate)
    short EQtype;
    /// type of equalizer//0 for none, 1/rock,2/classical,3/jazz,4/pop,5/bass
    short nbChannel;
    /// Length of the decoded PCM frame
    short pcmFrameLen;
    /// Stream status (size of int32)
    short WmaFileStatus;

} VPP_WMA_DEC_OUT_T;
// ============================================================================
// VPP_WMA_DEC_CONTENT_T
// ----------------------------------------------------------------------------
/// Gives the audio properties,content and descriptions of the wma file.
// ============================================================================
typedef struct
{
    /* audio prop */

    DWORD   nVersion;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBitsPerSec;
    DWORD   nChannels;
    DWORD   msDuration;

    /* Content Description */
    WORD    cbCDTitle;
    WORD    cbCDAuthor;
    WORD    cbCDCopyright;
    WORD    cbCDDescription;

} VPP_WMA_DEC_CONTENT_T;
//=============================================================================
// vpp_WMAGetInfo function
//-----------------------------------------------------------------------------
///Get the wma file infomation
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_WmaGetInfo( int32 * pStreamIn ,VPP_WMA_DEC_CONTENT_T * pWmaProperty );

//=============================================================================
// vpp_WMADecOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP WMAdecoder code and environment variables. It also set the
/// VoC wakeup masks necessary for the synchronization. This function is called
/// each time a WMA decoder stream is started. Preload the input stream buffer.
/// @param vocIrqHandler user defined handler function for the VoC interrupt.
/// @param pStreamIn pointer to the start of the input stream
/// If the function is set to NULL, no interrupts are used (the interrupt mask
/// is not set).
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_WmaDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler, int32 * pStreamIn,int32 *output, int16 * PrevOutput);
void vpp_Wma_voc_open(void);

//=============================================================================
// vpp_WMADecClose function
//-----------------------------------------------------------------------------
/// Close VPP WMA Decoder, clear VoC wakeup masks. This function is
/// called each time a WMA stream is stopped.
//=============================================================================
void vpp_WmaDecClose(void);

//=============================================================================
// vpp_WmaInitStatus function
//-----------------------------------------------------------------------------
/// This function will read the decoder status after initialization (valid at interrupt generation).
/// @param pDecStatus pointer to the destination status structure of type
/// VPP_WMA_DEC_OUT_T.
//=============================================================================
void vpp_WmaInitStatus(VPP_WMA_DEC_OUT_T * pDecStatus);
void vpp_WmaStart_Voc(void);


//=============================================================================
// vpp_WMADecStatus function
//-----------------------------------------------------------------------------
/// This function will read the decoder status (valid at interrupt generation).
/// @param pDecStatus pointer to the destination status structure of type
/// VPP_WMA_DEC_OUT_T.
//=============================================================================
void vpp_WmaDecStatus(VPP_WMA_DEC_OUT_T * pDecStatus);

//=============================================================================
// vpp_WMADecScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule decoding of one WMA coded frame.
/// @param pStreamIn pointer to the input stream
/// @param pStreamOut pointer to the output stream
/// @return error of type HAL_ERR_T
//=============================================================================
//HAL_ERR_T vpp_WmaDecScheduleOneFrame(INT32 * pStreamIn,
//                                           INT32 * pStreamOut, UINT8 EQMode);

HAL_ERR_T vpp_WmaDecScheduleOneFrame(/*INT32 * pStreamIn,*/ INT32 * pStreamOut, UINT8 EQMode);

HAL_ERR_T vpp_WmaDecScheduleOneFrame_For_Datafill( UINT8 EQMode);

//EQMode: 0 for none,   1 for bass, 2 for dance, 3 for classical, 4 for alt, 5 for party, 6 for pop, 7 for rock
///  @} <- End of the vpp_WMA_dec group

//void hal_VocUserWmaDecHandler(HAL_VOC_IRQ_STATUS_T * status);

HAL_ERR_T vpp_WmaDecScheduleSetFile_Point_Addr(INT32 * pStreamIn);



UINT32 vpp_WmaDecScheduleGetFile_Point_Addr(void);


HAL_ERR_T vpp_WmaDecScheduleSetOutput_Pcm_Addr(INT32 * pStreamout);

#endif  // VPP_WMA_DEC_H


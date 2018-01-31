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








#ifndef vpp_AudioJpeg_DEC_H
#define vpp_AudioJpeg_DEC_H

#include "cs_types.h"
#include "hal_voc.h"
#include "hal_error.h"


// ============================================================================
// vpp_AudioJpeg_DEC_STREAM_STATUS
// ----------------------------------------------------------------------------
/// Status after the last frame processing
// ============================================================================
typedef enum
{
    /// continue normally
    vpp_AudioJpeg_DEC_STREAM_CONTINUE = 0x0000,
    /// stop decoding normally
    vpp_AudioJpeg_DEC_STREAM_STOP     = 0x0010,
    /// stop decoding and signal an error
    vpp_AudioJpeg_DEC_STREAM_BREAK    = 0x0011,
    /// ignore the current frame
    vpp_AudioJpeg_DEC_STREAM_IGNORE   = 0x0020

} vpp_AudioJpeg_DEC_STREAM_STATUS;

typedef enum
{
    Rotate_with_0  = 0,
    Rotate_with_90 = 1,
    Rotate_with_270 = 2
} ROTATE_MODE;

// ============================================================================
// vpp_AudioJpeg_DEC_IN_T
// ----------------------------------------------------------------------------
/// Defines the in/out stream buffers and the reset.
// ============================================================================

// struct of input parameters
typedef struct
{
    INT16 mode;              //Unused, for alignment to Word32.
    INT16 reset;                 //Global VPP AUDIO_DEC reset, active ‘1’. It should be automatically cleared by the VPP code.

    INT16 BsbcEnable;            //use to be as a flag of whether using bluetooth
    INT16 audioItf;               //unused

    INT32* inStreamBufAddr;         //Input stream buffer address
    INT32* outStreamBufAddr;        //Output stream buffer address

    INT16 EQ_Type;             //Input stream buffer end address
    INT16 zoom_mode;//ROTATE_MODE

    INT16 Source_width;
    INT16 Source_height;

    INT16 Cut_width;
    INT16 Cut_height;
    INT16 Zoomed_width;
    INT16 Zoomed_height;
    INT32* inStreamUBufAddr;         //Input stream U buffer address
    INT32* inStreamVBufAddr;         //Input stream V buffer address

} vpp_AudioJpeg_DEC_IN_T;



// ============================================================================
// vpp_AudioJpeg_DEC_OUT_T
// ----------------------------------------------------------------------------
/// Gives the status after the last frame processing and the mode of the last
/// processed frame.

//struct of output parameters
typedef struct
{
    INT16 mode;     //used by AUDIO_DEC encode
    INT16 nbChannel;  //Number of channels
    INT32 output_len; //Size of the output PCM frame(bytes).
    INT32 SampleRate;
    INT16 BitRate;
    INT16 consumedLen; //Size of the consumed data since last frame
    INT16 streamStatus;         //Stream Status
    INT16 ErrorStatus;
    /// Image width parameter for MJPEG
    INT16  ImgWidth;
    /// Image Height parameter for MJPEG
    INT16  ImgHeight;
} vpp_AudioJpeg_DEC_OUT_T;



//=============================================================================
// vpp_AudioJpegDecOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP AUDIO_DEC code and environment variables. It also set the
/// VoC wakeup masks necessary for the synchronization. This function is called
/// each time a AUDIO_DEC stream is started. Preload the input stream buffer.
/// @param vocIrqHandler user defined handler function for the VoC interrupt.
/// @param pStreamIn pointer to the start of the input stream
/// If the function is set to NULL, no interrupts are used (the interrupt mask
/// is not set).
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_AudioJpegDecOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler);

//=============================================================================
// vpp_AudioJpegDecClose function
//-----------------------------------------------------------------------------
/// Close VPP AUDIO_DEC Decoder, clear VoC wakeup masks. This function is
/// called each time a AUDIO_DEC stream is stopped.
//=============================================================================
void vpp_AudioJpegDecClose(void);


//=============================================================================
// vpp_AudioJpegDecStatus function
//-----------------------------------------------------------------------------
/// This function will read the decoder status (valid at interrupt generation).
/// @param pDecStatus pointer to the destination status structure of type
/// vpp_AudioJpeg_DEC_OUT_T.
//=============================================================================
void vpp_AudioJpegDecStatus(vpp_AudioJpeg_DEC_OUT_T * pDecStatus);

//=============================================================================
// vpp_AudioJpegDecScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule decoding of one AUDIO_DEC coded frame.
/// @param pStreamIn pointer to the input stream
/// @param pStreamOut pointer to the output stream
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T  vpp_AudioJpegDecScheduleOneFrame(vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN);

//=============================================================================
// vpp_AudioJpegDecSetCurMode function
//-----------------------------------------------------------------------------
/// Set Up VoC Video and AUDIO mode
/// @return error of type HAL_ERR_T
//=============================================================================

HAL_ERR_T  vpp_AudioJpegDecSetCurMode(INT16 pVoc_Video_Mode, INT16 pVoc_Audio_Mode);


//=============================================================================
// vpp_AudioJpegDecSetInputMode function
//-----------------------------------------------------------------------------
/// Set Up VoC Video and AUDIO mode
/// @return error of type HAL_ERR_T
//=============================================================================

HAL_ERR_T  vpp_AudioJpegDecSetInputMode(INT16 pVoc_Input_Mode);

//=============================================================================
// vpp_AVDecSetMode function
//-----------------------------------------------------------------------------
/// Set Up VoC Video and AUDIO mode
/// @return error of type HAL_ERR_T
//=============================================================================

HAL_ERR_T  vpp_AVDecSetMode(INT16 DecMode, INT16 AVMode, INT16 Reset);


//=============================================================================
// vpp_AudioJpegDecGetMode function
//-----------------------------------------------------------------------------
/// Get VOC Video and AUDIO Input mode
/// @return mode
//=============================================================================
INT16  vpp_AudioJpegDecGetMode(void);

//=============================================================================
// vpp_AudioJpegSetReuseCode function
//reuse with VPP_H263_Zoom_Code_ExternalAddr_addr
//code_id: 0=zoom, 1=rmvb, 2=264 ...
//set after vpp_AudioJpegDecOpen
//-----------------------------------------------------------------------------
void vpp_AudioJpegSetReuseCode(INT16 code_id);

///  @} <- End of the vpp_AudioJpeg_DEC group

//=============================================================================
// set reCalcMp3EQ in vpp_audiojpeg_dec.c
// if reCalcMp3EQ is true,
// mp3 drc and eq is reset in next vpp_AudioJpegDecScheduleOneFrame
void vpp_AudioMP3DecSetReloadFlag(void);



#endif  // vpp_AudioJpeg_DEC_H

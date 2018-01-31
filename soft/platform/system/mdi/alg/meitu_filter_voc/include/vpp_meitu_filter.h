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








#ifndef VPP_MEITU_FILTER_H
#define VPP_MEITU_FILTER_H

#include "cs_types.h"
#include "hal_voc.h"
#include "hal_error.h"

// ============================================================================
// vpp_MEITU_IN_T
// ----------------------------------------------------------------------------
/// Defines the in/out stream buffers and the reset.
// ============================================================================

// struct of input parameters
typedef struct
{
    short mode;              //Unused, for alignment to Word32.
    short reset;                 //Global VPP AUDIO_DEC reset, active ‘1’. It should be automatically cleared by the VPP code.

    //short framecount;
    short reserved2;
    short reserved;

    short image_width;
    short image_height;

    short brightness;
    short contrast;

    int* inStreamBufAddr;         //Input stream buffer address
    int* outStreamBufAddr;        //Output stream buffer address
} vpp_MEITU_IN_T;


// ============================================================================
// vpp_MEITU_OUT_T
// ----------------------------------------------------------------------------
/// Gives the status after the last frame processing and the mode of the last
/// processed frame.

//struct of output parameters
typedef struct
{
    int consumedLen;
    int output_len;
    short ErrorStatus;
    short framecount;
} vpp_MEITU_OUT_T;

// enum of stream status
/*
typedef enum
{
    VPP_MEITU_STREAM_CONTINUE = 0x0000,
    VPP_MEITU_STREAM_STOP = 0x0010,
    VPP_MEITU_STREAM_BREAK = 0x0011,
    VPP_MEITU_STREAM_IGNORE = 0x0020,
} VPP_MEITU_STREAM_STATUS;
*/

//=============================================================================
// vpp_MeituOpen function
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
HAL_ERR_T vpp_MeituOpen(HAL_VOC_IRQ_HANDLER_T voc_IrqHandler);

//=============================================================================
// vpp_MeituClose function
//-----------------------------------------------------------------------------
/// Close VPP AUDIO_DEC Decoder, clear VoC wakeup masks. This function is
/// called each time a AUDIO_DEC stream is stopped.
//=============================================================================
void vpp_MeituClose(void);


//=============================================================================
// vpp_MeituStatus function
//-----------------------------------------------------------------------------
/// This function will read the decoder status (valid at interrupt generation).
/// @param pDecStatus pointer to the destination status structure of type
/// vpp_AudioJpeg_DEC_OUT_T.
//=============================================================================
void vpp_MeituStatus(vpp_MEITU_OUT_T * pMeituStatus);

//=============================================================================
// vpp_MeituScheduleOneFrame function
//-----------------------------------------------------------------------------
/// Schedule decoding of one AUDIO_DEC coded frame.
/// @param pStreamIn pointer to the input stream
/// @param pStreamOut pointer to the output stream
/// @return error of type HAL_ERR_T
//=============================================================================
HAL_ERR_T vpp_MeituScheduleOneFrame(vpp_MEITU_IN_T *pVoc_Voc_MeituIN);


///  @} <- End of the vpp_Meitu group

#endif  // vpp_MEITU_VOC_H

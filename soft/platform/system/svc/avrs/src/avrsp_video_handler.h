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



#ifndef _AVRSP_VIDEO_HANDLER_H_
#define _AVRSP_VIDEO_HANDLER_H_


// =============================================================================
// avrs_VideoHandler
// -----------------------------------------------------------------------------
/// This is the video handler called when the encoding of a frame is finished.
/// (This function being the callback of a #IMGS_ENCODED_PICTURE_T structure, it
/// must comply to the #IMGS_ENCODED_PICTURE_T type prototype.)
///
/// @param encodedBuffer Pointer to the beginning of the encoded picture.
/// @param length Length of the picture in the buffer.
// =============================================================================
PROTECTED VOID avrs_VideoHandler(UINT8* encodedBuffer, UINT32 length);














#endif // _AVRSP_VIDEO_HANDLER_H_






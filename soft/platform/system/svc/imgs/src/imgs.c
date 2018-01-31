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


#include "cs_types.h"
#include "lcdd_m.h"
#include "fs.h"
#include "sxr_mem.h"

#include "imgs_m.h"
#include "imgsp_jpeg_dec.h"
#include "imgsp_jpeg_enc.h"

#include "imgsp_debug.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
///
// =============================================================================

// =============================================================================
// MACROS
// =============================================================================

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// imgs_EncodeImageFile
// -----------------------------------------------------------------------------
/// Encode a raw picture into a file, at a given format.
/// The raw picture is given as a frame buffer window. The image to encode is
/// defined as the Region Of Interest of the Frame buffer window. The encoded
/// image is written in the \c filename file. The encoded image has the same
/// size as the frame buffer window region of interest, and is encoded in the \c
/// encParams->format format.
///
/// @param inFbw Input frame buffer window whose region of interest will be
/// encoded.
/// @param fileName Name of the file to save.
/// @param encParams Parameters to configure the encoder.
/// @param callback If not \c NULL, this user handler is a function called when
/// the encoding process is finished.
/// @return #IMGS_ERR_NO, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_UNSUPPORTED_FORMAT,
/// ...
// =============================================================================
PUBLIC IMGS_ERR_T imgs_EncodeImageFile( IMGS_FBW_T*             inFbw,
                                        PCSTR                   fileName,
                                        IMGS_ENCODING_PARAMS_T* encParams,
                                        IMGS_ENC_CALLBACK_T     callback)
{
    IMGS_PROFILE_FUNCTION_ENTER(imgs_EncodeImageFile);
    IMGS_ERR_T errStatus = IMGS_ERR_NO;
    switch (encParams->format)
    {
        case IMGS_IMG_FORMAT_JPG:
            errStatus = imgs_JpegEncode(inFbw, fileName, encParams, callback);
            break;

        // TODO: implement other formats
        default:
            IMGS_ASSERT(FALSE, "Unsupported type %d for IMGS decoding",
                        encParams->format);
            return IMGS_ERR_UNSUPPORTED_FORMAT;
    }

    // Debug remove
    IMGS_TRACE(TSTDOUT, 0, "imgs_EncodeImageFile: return %d", errStatus);
    return errStatus;
    IMGS_PROFILE_FUNCTION_EXIT(imgs_EncodeImageFile);
}


// =============================================================================
// imgs_EncodeImageBuffer
// -----------------------------------------------------------------------------
/// Encode a raw picture into a buffer, at a given format.
/// The raw picture is given as a frame buffer window. The image to encode is
/// defined as the Region Of Interest of the Frame buffer window. The encoded
/// image is written in the \c buffer buffer. The encoded image has the same
/// size as the frame buffer window region of interest, and is encoded in the \c
/// encParams->format format.
/// The callback is called with the encoded image length as a parameter.
///
/// @param inFbw Input frame buffer window whose region of interest will be
/// encoded.
/// @param buffer Buffer where the encoded image is saved.
/// @param encParams Parameters to configure the encoder.
/// @param callback If not \c NULL, this user handler is a function called when
/// the encoding process is finished.
/// @return #IMGS_ERR_NO, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_UNSUPPORTED_FORMAT,
/// ...
// =============================================================================
PUBLIC IMGS_ERR_T imgs_EncodeImageBuffer( IMGS_FBW_T*           inFbw,
        PCSTR                   buffer,
        IMGS_ENCODING_PARAMS_T* encParams,
        IMGS_ENC_CALLBACK_T     callback)
{
    IMGS_PROFILE_FUNCTION_ENTER(imgs_EncodeImageBuffer);
    // FIXME Implement this function.
    // We return an error for now.
    IMGS_PROFILE_FUNCTION_EXIT(imgs_EncodeImageBuffer);
    return IMGS_ERR_FILE_CREATE;
}

// =============================================================================
// imgs_DecodeImageFile
// -----------------------------------------------------------------------------
/// Decode an encoded image in a file at a given format into a frame buffer
/// window. In case of multiframe picture, only one frame is decoded.
///
/// The frame buffer window buffer is allocated by the call to #imgs_DecodeImage.
/// The raw picture is decoded in the output frame buffer window. The generated
/// frame buffer window contains the full image: the height and width of the
/// frame buffer window are the same as the encoded picture's, and the region
/// of interest is the full frame buffer size.
///
/// @param fileName Encoded image to decode.
/// @param outFbw Output frame buffer window which contains the decoded image.
/// @param frameNb Number of the frame of the encoded picture to decode, if
/// the encoded image uses a multiframe format. Otherwise, this parameter
/// value is ignored.
/// @param callback If not \c NULL, this user handler is a function called when
/// the decoding process is finished.
/// @return #IMGS_ERR_NO, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_UNSUPPORTED_FORMAT,
/// ...
// =============================================================================
PUBLIC IMGS_ERR_T imgs_DecodeImageFile(PCSTR                fileName,
                                       IMGS_FBW_T*          outFbw,
                                       UINT32               frameNb,
                                       IMGS_DEC_CALLBACK_T  callback)
{
    IMGS_PROFILE_FUNCTION_ENTER(imgs_DecodeImageFile);
    // Identify file type
    INT32   fileHandle;
    UINT8   fileHeader[64];
    IMGS_IMG_FORMAT_T fileFormat = 0;
    IMGS_ERR_T errStatus = IMGS_ERR_NO;

    IMGS_ASSERT((UINT32)fileName, "imgs_DecodeImageFile: no file");
    IMGS_ASSERT((UINT32)outFbw, "imgs_DecodeImageFile: no output FBW");

    // Open file
    fileHandle = FS_Open(fileName, FS_O_RDONLY, 0);
    if (fileHandle < 0)
    {
        IMGS_TRACE(TSTDOUT, 0, "imgs_OpenFile failed:%s, err:%d", fileName, fileHandle);
        return IMGS_ERR_FILE_OPEN;
    }

    // Get file header
    FS_Read(fileHandle, fileHeader, sizeof(fileHeader));

    // We don't need to access the file at that level anymore.
    FS_Close(fileHandle);

    // Check file type
    // TODO: more than just JPEG, please !

    // One passage through the loop, break allow an exit
    // such as in a switch construction
    do
    {
        // JPEG
        if (fileHeader[0] == 0xFF
                && fileHeader[1] == 0xD8)
        {
            fileFormat = IMGS_IMG_FORMAT_JPG;
            break;
        }

    }
    while (0);

    // Call proper decoder.
    switch (fileFormat)
    {
        case IMGS_IMG_FORMAT_JPG:
            errStatus = imgs_JpegDecode(fileName, outFbw, callback, 1);
            break;

        // TODO: implement other formats
        default:
            IMGS_ASSERT(FALSE, "Unsupported type %d for IMGS decoding",
                        fileFormat);
            return IMGS_ERR_UNSUPPORTED_FORMAT;
    }

    // Debug remove
    IMGS_TRACE(TSTDOUT, 0, "imgs_DecodeImageFile: return %d", errStatus);
    return errStatus;
    IMGS_PROFILE_FUNCTION_EXIT(imgs_DecodeImageFile);
}


// =============================================================================
// imgs_DecodeImageBuffer
// -----------------------------------------------------------------------------
/// Decode an encoded image in a buffer at a given format into a frame buffer
/// window. In case of multiframe picture, only one frame is decoded.
///
/// The frame buffer window buffer is allocated by the call to #imgs_DecodeImage.
/// The raw picture is decoded in the output frame buffer window. The generated
/// frame buffer window contains the full image: the height and width of the
/// frame buffer window are the same as the encoded picture's, and the region
/// of interest is the full frame buffer size.
///
/// @param buffer Encoded image to decode.
/// @param outFbw Output frame buffer window which contains the decoded image.
/// @param frameNb Number of the frame of the encoded picture to decode, if
/// the encoded image uses a multiframe format. Otherwise, this parameter
/// value is ignored.
/// @param callback If not \c NULL, this user handler is a function called when
/// the decoding process is finished.
/// @return #IMGS_ERR_NO, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_UNSUPPORTED_FORMAT,
/// ...
// =============================================================================
PUBLIC IMGS_ERR_T imgs_DecodeImageBuffer(UINT8*                 buffer,
        IMGS_FBW_T*            outFbw,
        UINT32                 frameNb,
        IMGS_DEC_CALLBACK_T    callback)
{
    IMGS_PROFILE_FUNCTION_ENTER(imgs_DecodeImageBuffer);
    IMGS_IMG_FORMAT_T format = 0;
    IMGS_ERR_T errStatus = IMGS_ERR_NO;

    IMGS_ASSERT((UINT32)buffer, "imgs_DecodeImageBuffer: no input buffer");
    IMGS_ASSERT((UINT32)outFbw, "imgs_DecodeImageBuffer: no output FBW");


    // Identify buffer type
    // TODO: more than just JPEG, please !

    // One passage through the loop, break allow an exit
    // such as in a switch construction
    do
    {
        // JPEG
        if (buffer[0] == 0xFF
                && buffer[1] == 0xD8)
        {
            format = IMGS_IMG_FORMAT_JPG;
            break;
        }

    }
    while (0);

    // Call proper decoder.
    switch (format)
    {
        case IMGS_IMG_FORMAT_JPG:
            errStatus = imgs_JpegDecode(buffer, outFbw, callback, 0);
            break;

        // TODO: implement other formats
        default:
            IMGS_ASSERT(FALSE, "Unsupported type %d for IMGS decoding",
                        format);
            return IMGS_ERR_UNSUPPORTED_FORMAT;
    }

    return errStatus;
    IMGS_PROFILE_FUNCTION_EXIT(imgs_DecodeImageBuffer);
}

// =============================================================================
// imgs_FreeFbw
// -----------------------------------------------------------------------------
/// Free from memory a frame buffer window created by the #imgs_DecodeImage.
/// It only releases the buffer pointed by the frame buffer 'buffer' field.
///
/// @param fbw Frame buffer window to free from memory.
/// @return #IMGS_ERR_NO.
// =============================================================================
PUBLIC IMGS_ERR_T imgs_FreeFbw(IMGS_FBW_T* fbw)
{
    IMGS_PROFILE_FUNCTION_ENTER(imgs_FreeFbw);
    if (fbw->fb.buffer)
    {
        sxr_Free(fbw->fb.buffer);
    }
    return IMGS_ERR_NO;
    IMGS_PROFILE_FUNCTION_EXIT(imgs_FreeFbw);
}




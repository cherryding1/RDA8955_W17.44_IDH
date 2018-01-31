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


#ifndef _IMGS_M_H_
#define _IMGS_M_H_

#include "cs_types.h"
#include "lcdd_m.h"
#include "fs.h"

// =============================================================================
// MACROS
// =============================================================================



// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// IMGS_ERR_T
// -----------------------------------------------------------------------------
/// Error values that can be returned by functions of the IMGS module.
// =============================================================================
typedef enum
{
    IMGS_ERR_NO,
    IMGS_ERR_UNSUPPORTED_FORMAT,
    IMGS_ERR_OUT_OF_MEMORY,
    IMGS_ERR_FILE,
    IMGS_ERR_FILE_OPEN,
    IMGS_ERR_FILE_CREATE,
    IMGS_ERR_FILE_CLOSE,
    IMGS_ERR_FUNC_FORMAT,
    IMGS_ERR_WRONG_DECOMPOSE ,
    IMGS_ERR_WRONG_SIZE,
    IMGS_ERR_BIG_SIZE,
    IMGS_ERR_WRONG_CASE,
    IMGS_ERR_WRONG_HEAD,
    IMGS_ERR_MALLOC,
    IMGS_ERR_INPUT_PARAMETERS,
    /// ?
    IMGS_ERR_ISP_DATA,
    IMGS_ERR_ISP_STATE,

    /// Legacy error ?
    IMGS_ERR_VLC,

    IMGS_ERR_QTY
} IMGS_ERR_T;


// =============================================================================
// IMGS_FBW_T
// -----------------------------------------------------------------------------
/// Frame buffer window. This type describes a window in a frame buffer as a
/// region of interest within this frame buffer. When the region of interest
/// starts at (0,0) and has the same dimensions as the frame buffer, the
/// window is in fact the whole frame buffer.
// =============================================================================
typedef LCDD_FBW_T IMGS_FBW_T;


// =============================================================================
// IMGS_IMG_FORMAT_T
// -----------------------------------------------------------------------------
/// This type describes the different format available with IMGS.
// =============================================================================
typedef enum
{
    /// Bitmap
    IMGS_IMG_FORMAT_BMP,

    /// Jpeg
    IMGS_IMG_FORMAT_JPG,

    /// GIF
    IMGS_IMG_FORMAT_GIF,

    /// PNG
    IMGS_IMG_FORMAT_PNG,

    IMGS_IMG_FORMAT_QTY
} IMGS_IMG_FORMAT_T;


// =============================================================================
// IMGS_ENCODING_PARAMS_BMP_T
// -----------------------------------------------------------------------------
/// Structure to configure encoding in BMP format.
// =============================================================================
typedef struct
{
} IMGS_ENCODING_PARAMS_BMP_T;


// =============================================================================
// IMGS_JPG_QUALITY_T
// -----------------------------------------------------------------------------
/// Define JPEG quality
// =============================================================================
typedef enum
{
    IMGS_JPG_QUALITY_LOW,
    IMGS_JPG_QUALITY_HIGH,

    IMGS_JPG_QUALITY_QTY
} IMGS_JPG_QUALITY_T;

// =============================================================================
// IMGS_ENCODING_PARAMS_JPG_T
// -----------------------------------------------------------------------------
/// Structure to configure encoding in Jpeg format.
// =============================================================================
typedef struct
{
    IMGS_JPG_QUALITY_T quality;
} IMGS_ENCODING_PARAMS_JPG_T;


// =============================================================================
// IMGS_ENCODING_PARAMS_GIF_T
// -----------------------------------------------------------------------------
/// Structure to configure encoding in GIF format.
// =============================================================================
typedef struct
{
} IMGS_ENCODING_PARAMS_GIF_T;


// =============================================================================
// IMGS_ENCODING_PARAMS_PNG_T
// -----------------------------------------------------------------------------
/// Structure to configure encoding in PNG format.
// =============================================================================
typedef struct
{
} IMGS_ENCODING_PARAMS_PNG_T;

// =============================================================================
// IMGS_ENCODING_PARAMS_T
// -----------------------------------------------------------------------------
/// This types describes the parameters needed to encode a picture.
/// As the encoding scheme is chosen by the user, to keep a light and robust
/// API, we use a union here.
/// @todo Define the structures for encoding.
// =============================================================================
typedef struct
{
    IMGS_IMG_FORMAT_T format;
    union
    {
        IMGS_ENCODING_PARAMS_BMP_T bmp;
        IMGS_ENCODING_PARAMS_JPG_T jpg;
        IMGS_ENCODING_PARAMS_GIF_T gif;
        IMGS_ENCODING_PARAMS_PNG_T png;
    };
} IMGS_ENCODING_PARAMS_T;

// =============================================================================
// IMGS_ENC_CALLBACK_T
// -----------------------------------------------------------------------------
/// The user of IMGS can define a callback using to this type that will be
/// called by IMGS when the encoding operation is over.
/// The parameters of the function are the pointer to the encoded buffer, or
/// NULL when the encoded image was recorded in a file, and
/// the length (in bytes) of the encoded image.
// =============================================================================
typedef VOID (*IMGS_ENC_CALLBACK_T)(UINT8*, UINT32) ;


// =============================================================================
// IMGS_DEC_CALLBACK_T
// -----------------------------------------------------------------------------
/// The user of IMGS can define a callback using to this type that will be
/// called by IMGS when the decoding operation is over.
/// The parameter is the decoded frame buffer window, where the decoded image
/// is stored.
// =============================================================================
typedef VOID (*IMGS_DEC_CALLBACK_T)(IMGS_FBW_T*);





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
/// The callback is called with the encoded image length as a parameter.
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
                                        IMGS_ENC_CALLBACK_T     callback);


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
        IMGS_ENC_CALLBACK_T     callback);


// =============================================================================
// imgs_DecodeImageFile
// -----------------------------------------------------------------------------
/// Decode an encoded image in a file at a given format into a frame buffer
/// window. In case of multiframe picture, only one frame is decoded.
///
/// The frame buffer window buffer is allocated by the call to #imgs_DecodeImageFile.
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
                                       IMGS_DEC_CALLBACK_T  callback);


// =============================================================================
// imgs_DecodeImageBuffer
// -----------------------------------------------------------------------------
/// Decode an encoded image in a buffer at a given format into a frame buffer
/// window. In case of multiframe picture, only one frame is decoded.
///
/// The frame buffer window buffer is allocated by the call to #imgs_DecodeImageBuffer.
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
        IMGS_DEC_CALLBACK_T    callback);



// =============================================================================
// imgs_FreeFbw
// -----------------------------------------------------------------------------
/// Free from memory a frame buffer window created by the #imgs_DecodeImageFile
/// or #imgs_DecodeImageBuffer functions.
/// It only releases the buffer pointed by the frame buffer 'buffer' field.
///
/// @param fbw Frame buffer window to free from memory.
/// @return #IMGS_ERR_NO.
// =============================================================================
PUBLIC IMGS_ERR_T imgs_FreeFbw(IMGS_FBW_T* fbw);


// =============================================================================
// imgs_SimilarizeImage
// -----------------------------------------------------------------------------
/// Similarize a raw picture. It resizes a window in a frame buffer window, described as
/// a region of interest, into the region of interest of another frame buffer window.
/// A part or the whole source picture can then be fitted into a part or the
/// whole destination frame buffer window.
///
/// @param inFbw Input picture. Its region of interest is the source picture.
/// @param outFbw Output picture. Its region of interest will receive the picture.
/// @param rotation Rotate the image while similarizing: number of trigonometrical
/// rotation in Pi/2 rad.
/// @return #IMGS_ERR_NO, #IMGS_ERR_OUT_OF_MEMORY, #IMGS_ERR_UNSUPPORTED_FORMAT,
/// ...
///
/// @todo What if the buffer are the same ? Enforce them to be different ?.
/// @todo Use a callback to warn the user about the end of the decoding ?
// =============================================================================
PUBLIC IMGS_ERR_T imgs_SimilarizeImage(IMGS_FBW_T* inFbw, IMGS_FBW_T* outFbw,
                                       UINT32 rotation);


/// @} // <-- End of the imgs

#endif // _IMGS_M_H_ 


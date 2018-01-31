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




#ifndef _CAMD_M_H_
#define _CAMD_M_H_


#include "cs_types.h"

// =============================================================================
// MACROS
// =============================================================================

/// Id to send event to mailboxes
#define CAMD_MSG_HEADER_ID 0xA0C01F63

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// CAMD_STATUS_T
// -----------------------------------------------------------------------------
/// Type used to describe the camera driver status in the message sent
/// through the CAMD message mailbox.
// =============================================================================
typedef enum
{
    CAMD_STATUS_CAPTURE_DONE,

    CAMD_STATUS_CAPTURE_QTY
} CAMD_STATUS_T;



// =============================================================================
// CAMD_MSG_T
// -----------------------------------------------------------------------------
/// Type used to encode the messages sent to the mailbox.
// (size must be at most four 32 bits words)
// =============================================================================
typedef struct
{
    UINT32          headerId;
    CAMD_STATUS_T   status;
} CAMD_MSG_T;


// =============================================================================
// CAMD_ERR_T
// -----------------------------------------------------------------------------
/// Error values that can be returned by functions of the CAMD module.
// =============================================================================
typedef enum
{
    /// Normal behaviour, no error
    CAMD_ERR_NO,

    /// The camera is busy
    CAMD_ERR_RESOURCE_BUSY,

    /// There is no camera device to open
    CAMD_ERR_NO_DEVICE,

    /// Unsupported parameter,
    CAMD_ERR_UNSUPPORTED_PARAM,

    /// Unsupported value
    CAMD_ERR_UNSUPPORTED_VALUE,

    CAMD_ERR_QTY
} CAMD_ERR_T;


// =============================================================================
// CAMD_CONFIG_T
// -----------------------------------------------------------------------------
/// That type is used to describe the configuration specific to a given target
/// for the CAMD driver. It defines all the control signals for each
/// target and the GPIO assignments.
// =============================================================================
typedef struct
{

} CAMD_CONFIG_T;


// =============================================================================
// CAMD_PARAM_T
// -----------------------------------------------------------------------------
/// This type lists the parameters of the camera that can be set by the
/// #camd_SetParam function
// =============================================================================
typedef enum
{
    /// Resolution of the picture to take, defined by the
    /// CAMD_PARAM_RESOLUTION_T type.
    CAMD_PARAM_RESOLUTION,

    /// Flash. Possible values are defined by the
    /// CAMD_PARAM_FLASH_T type.
    CAMD_PARAM_FLASH,

    /// Exposure. Possible values are defined by the
    /// CAMD_PARAM_EXPOSURE_T type.
    CAMD_PARAM_EXPOSURE,

    /// Exposure. Possible values are defined by the
    /// CAMD_PARAM_WHITE_BALANCE_T type.
    CAMD_PARAM_WHITE_BALANCE,

    /// Exposure. Possible values are defined by the
    /// CAMD_PARAM_SPECIAL_EFFECT_T type.
    CAMD_PARAM_SPECIAL_EFFECT,

    /// Exposure. Possible values are defined by the
    /// CAMD_PARAM_FORMAT_T type.
    CAMD_PARAM_FORMAT,

    /// Optical zoom. Possible values are defined by the
    /// CAMD_PARAM_OPTICAL_ZOOM_T type.
    CAMD_PARAM_OPTICAL_ZOOM, //int

    /// Digital zoom. Possible values are defined by the
    /// CAMD_PARAM_DIGITAL_ZOOM_T type.
    CAMD_PARAM_DIGITAL_ZOOM, //int

    /// Contrast. Possible values are defined by the
    /// CAMD_PARAM_CONTRAST_T type.
    CAMD_PARAM_CONTRAST, // int ?

    /// Brightness. Possible values are defined by the
    /// CAMD_PARAM_BRIGHTNESS_T type.
    CAMD_PARAM_BRIGHTNESS,  // int ?

    /// Night mode. Possible values are defined by the
    /// CAMD_PARAM_NIGHT_MODE_T type.
    CAMD_PARAM_NIGHT_MODE, //bool ?

    CAMD_PARAM_QTY
} CAMD_PARAM_T;


// =============================================================================
// CAMD_PARAM_FLASH_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_FLASH camera flash parameter.
// =============================================================================
typedef enum
{
    CAMD_PARAM_FLASH_NONE           = 0x00,
    CAMD_PARAM_FLASH_AUTO           = 0x01,
    CAMD_PARAM_FLASH_FORCED         = 0x02,
    CAMD_PARAM_FLASH_FILLIN         = 0x03,
    CAMD_PARAM_FLASH_REDEYEREDUCE   = 0x04
} CAMD_PARAM_FLASH_T;


// =============================================================================
// CAMD_PARAM_EXPOSURE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_EXPOSURE camera exposure parameter.
// =============================================================================
typedef enum
{
    CAMD_PARAM_EXPOSURE_AUTO        = 0x00,
    CAMD_PARAM_EXPOSURE_NEGATIVE_2  = 0x01,
    CAMD_PARAM_EXPOSURE_NEGATIVE_1  = 0x02,
    CAMD_PARAM_EXPOSURE_POSITIVE_1  = 0x03,
    CAMD_PARAM_EXPOSURE_POSITIVE_2  = 0x04
} CAMD_PARAM_EXPOSURE_T;


// =============================================================================
// CAMD_PARAM_WHITE_BALANCE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_WHITE_BALANCE camera white balance
/// parameter.
// =============================================================================
typedef enum
{
    CAMD_PARAM_WHITE_BALANCE_AUTO       = 0x00,
    CAMD_PARAM_WHITE_BALANCE_DAYLIGHT   = 0x01,
    CAMD_PARAM_WHITE_BALANCE_CLOUDY     = 0x02,
    CAMD_PARAM_WHITE_BALANCE_TUNGSTEN   = 0x03,
    CAMD_PARAM_WHITE_BALANCE_OFFICE     = 0x04
} CAMD_PARAM_WHITE_BALANCE_T;


// =============================================================================
// CAMD_PARAM_SPECIAL_EFFECT_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_SPECIAL_EFFECT camera special effect
/// selection.
// =============================================================================
typedef enum
{
    CAMD_PARAM_SPECIAL_EFFECT_NORMAL                = 0x00,
    CAMD_PARAM_SPECIAL_EFFECT_ANTIQUE               = 0x01,
    CAMD_PARAM_SPECIAL_EFFECT_REDISH                = 0x02,
    CAMD_PARAM_SPECIAL_EFFECT_GREENISH              = 0x03,
    CAMD_PARAM_SPECIAL_EFFECT_BLUEISH               = 0x04,
    CAMD_PARAM_SPECIAL_EFFECT_BLACKWHITE            = 0x05,
    CAMD_PARAM_SPECIAL_EFFECT_NEGATIVE              = 0x06,
    CAMD_PARAM_SPECIAL_EFFECT_BLACKWHITE_NEGATIVE   = 0x07
} CAMD_PARAM_SPECIAL_EFFECT_T;



// =============================================================================
// CAMD_PARAM_FORMAT_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_FORMAT camera output format parameter.
/// For now only support RGB565 format
// =============================================================================
typedef enum
{
    CAMD_PARAM_FORMAT_RGBNORM,
    CAMD_PARAM_FORMAT_RGB565,
    CAMD_PARAM_FORMAT_RGB555,
    CAMD_PARAM_FORMAT_YUV
} CAMD_PARAM_FORMAT_T;


// =============================================================================
// CAMD_PARAM_OPTICAL_ZOOM_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_OPTICAL_ZOOM camera zoom value.
// =============================================================================
typedef enum
{
    CAMD_PARAM_OPTICAL_ZOOM_X1,
    CAMD_PARAM_OPTICAL_ZOOM_X2,
    CAMD_PARAM_OPTICAL_ZOOM_X3,
    CAMD_PARAM_OPTICAL_ZOOM_X4
} CAMD_PARAM_OPTICAL_ZOOM_T;


// =============================================================================
// CAMD_PARAM_DIGITAL_ZOOM_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_DIGITAL_ZOOM camera digital zoom.
// =============================================================================
typedef enum
{
    CAMD_PARAM_DIGITAL_ZOOM_X1,
    CAMD_PARAM_DIGITAL_ZOOM_X2,
    CAMD_PARAM_DIGITAL_ZOOM_X3,
    CAMD_PARAM_DIGITAL_ZOOM_X4
} CAMD_PARAM_DIGITAL_ZOOM_T;


// =============================================================================
// CAMD_PARAM_CONTRAST_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_CONTRAST camera contrast.
// =============================================================================
typedef enum
{
    CAMD_PARAM_CONTRAST_0,
    CAMD_PARAM_CONTRAST_1,
    CAMD_PARAM_CONTRAST_2,
    CAMD_PARAM_CONTRAST_3,
    CAMD_PARAM_CONTRAST_4,
    CAMD_PARAM_CONTRAST_5,
    CAMD_PARAM_CONTRAST_6,
    CAMD_PARAM_CONTRAST_7
} CAMD_PARAM_CONTRAST_T;


// =============================================================================
// CAMD_PARAM_BRIGHTNESS_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_BRIGHTNESS camera brightness.
// =============================================================================
typedef enum
{
    CAMD_PARAM_BRIGHTNESS_0,
    CAMD_PARAM_BRIGHTNESS_1,
    CAMD_PARAM_BRIGHTNESS_2,
    CAMD_PARAM_BRIGHTNESS_3,
    CAMD_PARAM_BRIGHTNESS_4,
    CAMD_PARAM_BRIGHTNESS_5,
    CAMD_PARAM_BRIGHTNESS_6,
    CAMD_PARAM_BRIGHTNESS_7
} CAMD_PARAM_BRIGHTNESS_T;



// =============================================================================
// CAMD_PARAM_NIGHT_MODE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMD_PARAM_NIGHT_MODE camera night mode.
// =============================================================================
typedef enum
{
    CAMD_PARAM_NIGHT_MODE_OFF,
    CAMD_PARAM_NIGHT_MODE_ON
} CAMD_PARAM_NIGHT_MODE_T;


// =============================================================================
// CAMD_PARAM_RESOLUTION_T
// -----------------------------------------------------------------------------
/// This type defines the resolution to use to take a picture.
// =============================================================================
typedef enum
{
    /// VGA resolution: 640x480
    CAMD_PARAM_RESOLUTION_VGA     = 0,

    /// QVGA resolution: 320x240
    CAMD_PARAM_RESOLUTION_QVGA    = 1,

    /// QQVGA resolution: 160x120
    CAMD_PARAM_RESOLUTION_QQVGA   = 2,

    CAMD_PARAM_RESOLUTION_QTY
} CAMD_PARAM_RESOLUTION_T ;


// =============================================================================
// CAMD_IMAGE_T
// -----------------------------------------------------------------------------
/// CAMD image type. This type describes the buffer used to get a picture.
/// Such a buffer must be allocated by the user and be large enough to
/// contain the whole picture.
// =============================================================================
typedef UINT16* CAMD_IMAGE_T;



// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// camd_Open
// -----------------------------------------------------------------------------
/// Open the camera driver, and do all the initialization needed to use the
/// functions of its API.
/// #camd_Open must be called before any other function of the CAMD API can
/// be used.
/// @param mbx Mailbox. The CAMD driver will send its status message (like
/// capture done, etc ...) in as messages to this mailbox. The ID of CAMD
/// messages are #CAMD_MSG_HEADER_ID.
/// @return #CAMD_ERR_NO, or #CAMD_ERR_NO_DEVICE.
// =============================================================================
PUBLIC CAMD_ERR_T camd_Open(UINT8 mbx);


// =============================================================================
// camd_Close
// -----------------------------------------------------------------------------
/// Close the CAMD driver. After a call to #camd_Close, a call to #camd_Open is
/// needed to be allowed to use CAMD functions.
/// @return #CAMD_ERR_NO.
// =============================================================================
PUBLIC CAMD_ERR_T camd_Close(VOID);


// =============================================================================
// camd_SetParam
// -----------------------------------------------------------------------------
/// Set a camera parameter to a given value.
/// The parameter to set is among the values defined by the type #CAMD_PARAM_T.
/// If this parameter is not available for a given implementation of CAMD,
/// the #CAMD_ERR_UNSUPPORTED_PARAM error must be returned.
/// Depending on the CAMD_PARAM_XXX parameter to set, the value can either
/// be a proper UINT32 number, or a value of the CAMD_PARAM_XXX_T enum, might
/// it exists. This is detailed in the CAMD_PARAM_T definition.
/// If the value is not supported for this parameter, #CAMD_ERR_UNSUPPORTED_VALUE
/// is returned.
///
/// @param param Camera parameter to set.
/// @param value Value to set to the parameter to.
/// @return #CAMD_ERR_NO, #CAMD_ERR_UNSUPPORTED_PARAM, #CAMD_ERR_UNSUPPORTED_VALUE
/// ...
// =============================================================================
PUBLIC CAMD_ERR_T camd_SetParam(CAMD_PARAM_T param, UINT32 value);


// =============================================================================
// camd_GetImage
// =============================================================================
/// Capture a single image.
/// @param buffer Buffer where the image data will be stored.
/// When the capture is done, a message with the Id #CAMD_MSG_HEADER_ID, and
/// the status field set to #CAMD_STATUS_CAPTURE_QTY is sent to the mailbox
/// passed as a parameter to the #camd_Open function.
/// @return #CAMD_ERR_NO, #CAMD_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC CAMD_ERR_T camd_GetImage(CAMD_IMAGE_T buffer);



// =============================================================================
// camd_GetInfo
// -----------------------------------------------------------------------------
/// Returns information from the camera as bitfields.
/// @param resolutions pointer to an integer where available resolutions will be
/// returned as bitfields: for example, if
/// resolutions &(1<<CAMD_PARAM_RESOLUTION_QVGA)
/// is not 0, the QVGA resolution is supported.
/// @param parameters pointer to an integer where available parameters will be
/// returned as bitfields: for example, if parameters &(1<<CAMD_PARAM_EXPOSURE)
/// is not 0, the exposure can be configured on this camera.
/// @return #CAMD_ERR_NO, #CAMD_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC CAMD_ERR_T camd_GetInfo(UINT32* resolutions, UINT32* parameters);

/// @} // <-- End of the camd group

#endif // _CAMD_M_H_



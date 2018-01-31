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


#ifndef _CAMS_M_H_
#define _CAMS_M_H_

#include "cs_types.h"
#include "lcdd_m.h"


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
// CAMS_ERR_T
// -----------------------------------------------------------------------------
/// Error type for CAMS.
// =============================================================================
typedef enum
{
    CAMS_ERR_NO,
    CAMS_ERR_RESOURCE_BUSY,
    CAMS_ERR_NO_DEVICE,
    CAMS_ERR_UNSUPPORTED_PARAM,
    CAMS_ERR_UNSUPPORTED_VALUE,


    CAMS_ERR_QTY
} CAMS_ERR_T;


// =============================================================================
// CAMS_PARAM_T
// -----------------------------------------------------------------------------
/// This type lists the parameters of the camera that can be set by the
/// #cams_SetParam function
// =============================================================================
typedef enum
{
    /// Flash. Possible values are defined by the
    /// CAMS_PARAM_FLASH_T type.
    CAMS_PARAM_FLASH,

    /// Exposure. Possible values are defined by the
    /// CAMS_PARAM_EXPOSURE_T type.
    CAMS_PARAM_EXPOSURE,

    /// Exposure. Possible values are defined by the
    /// CAMS_PARAM_WHITE_BALANCE_T type.
    CAMS_PARAM_WHITE_BALANCE,

    /// Exposure. Possible values are defined by the
    /// CAMS_PARAM_SPECIAL_EFFECT_T type.
    CAMS_PARAM_SPECIAL_EFFECT,

    /// Exposure. Possible values are defined by the
    /// CAMS_PARAM_FORMAT_T type.
    CAMS_PARAM_FORMAT,

    /// Optical zoom. Possible values are defined by the
    /// CAMS_PARAM_OPTICAL_ZOOM_T type.
    CAMS_PARAM_OPTICAL_ZOOM, //int

    /// Digital zoom. Possible values are defined by the
    /// CAMS_PARAM_DIGITAL_ZOOM_T type.
    CAMS_PARAM_DIGITAL_ZOOM, //int

    /// Contrast. Possible values are defined by the
    /// CAMS_PARAM_CONTRAST_T type.
    CAMS_PARAM_CONTRAST, // int ?

    /// Brightness. Possible values are defined by the
    /// CAMS_PARAM_BRIGHTNESS_T type.
    CAMS_PARAM_BRIGHTNESS,  // int ?

    /// Night mode. Possible values are defined by the
    /// CAMS_PARAM_NIGHT_MODE_T type.
    CAMS_PARAM_NIGHT_MODE, //bool ?

    /// FrameRate
    CAMS_PARAM_FRAME_RATE,

    CAMS_PARAM_QTY
} CAMS_PARAM_T;


// =============================================================================
// CAMS_PARAM_FLASH_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_FLASH camera flash parameter.
// =============================================================================
typedef enum
{
    CAMS_PARAM_FLASH_NONE           = 0x00,
    CAMS_PARAM_FLASH_AUTO           = 0x01,
    CAMS_PARAM_FLASH_FORCED         = 0x02,
    CAMS_PARAM_FLASH_FILLIN         = 0x03,
    CAMS_PARAM_FLASH_REDEYEREDUCE   = 0x04
} CAMS_PARAM_FLASH_T;


// =============================================================================
// CAMS_PARAM_EXPOSURE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_EXPOSURE camera exposure parameter.
// =============================================================================
typedef enum
{
    CAMS_PARAM_EXPOSURE_NEGATIVE_2  = 0x00,
    CAMS_PARAM_EXPOSURE_NEGATIVE_1  = 0x01,
    CAMS_PARAM_EXPOSURE_AUTO        = 0x02,
    CAMS_PARAM_EXPOSURE_POSITIVE_1  = 0x03,
    CAMS_PARAM_EXPOSURE_POSITIVE_2  = 0x04
} CAMS_PARAM_EXPOSURE_T;


// =============================================================================
// CAMS_PARAM_WHITE_BALANCE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_WHITE_BALANCE camera white balance
/// parameter.
// =============================================================================
typedef enum
{
    CAMS_PARAM_WHITE_BALANCE_AUTO       = 0x00,
    CAMS_PARAM_WHITE_BALANCE_DAYLIGHT   = 0x01,
    CAMS_PARAM_WHITE_BALANCE_TUNGSTEN   = 0x02,
    CAMS_PARAM_WHITE_BALANCE_OFFICE     = 0x03,
    CAMS_PARAM_WHITE_BALANCE_CLOUDY     = 0x04,
} CAMS_PARAM_WHITE_BALANCE_T;


// =============================================================================
// CAMS_PARAM_SPECIAL_EFFECT_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_SPECIAL_EFFECT camera special effect
/// selection.
// =============================================================================
typedef enum
{
    CAMS_PARAM_SPECIAL_EFFECT_NORMAL                = 0x00,
    CAMS_PARAM_SPECIAL_EFFECT_ANTIQUE               = 0x01,
    CAMS_PARAM_SPECIAL_EFFECT_REDISH                = 0x02,
    CAMS_PARAM_SPECIAL_EFFECT_GREENISH              = 0x03,
    CAMS_PARAM_SPECIAL_EFFECT_BLUEISH               = 0x04,
    CAMS_PARAM_SPECIAL_EFFECT_BLACKWHITE            = 0x05,
    CAMS_PARAM_SPECIAL_EFFECT_NEGATIVE              = 0x06,
    CAMS_PARAM_SPECIAL_EFFECT_BLACKWHITE_NEGATIVE   = 0x07
} CAMS_PARAM_SPECIAL_EFFECT_T;



// =============================================================================
// CAMS_PARAM_FORMAT_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_FORMAT camera output format parameter.
/// For now only support RGB565 format
// =============================================================================
typedef enum
{
    CAMS_PARAM_FORMAT_RGBNORM,
    CAMS_PARAM_FORMAT_RGB565,
    CAMS_PARAM_FORMAT_RGB555,
    CAMS_PARAM_FORMAT_YUV
} CAMS_PARAM_FORMAT_T;


// =============================================================================
// CAMS_PARAM_OPTICAL_ZOOM_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_OPTICAL_ZOOM camera zoom value.
// =============================================================================
typedef enum
{
    CAMS_PARAM_OPTICAL_ZOOM_X1,
    CAMS_PARAM_OPTICAL_ZOOM_X2,
    CAMS_PARAM_OPTICAL_ZOOM_X3,
    CAMS_PARAM_OPTICAL_ZOOM_X4
} CAMS_PARAM_OPTICAL_ZOOM_T;


// =============================================================================
// CAMS_PARAM_DIGITAL_ZOOM_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_DIGITAL_ZOOM camera digital zoom.
// =============================================================================
typedef enum
{
    CAMS_PARAM_DIGITAL_ZOOM_X1,
    CAMS_PARAM_DIGITAL_ZOOM_X2,
    CAMS_PARAM_DIGITAL_ZOOM_X3,
    CAMS_PARAM_DIGITAL_ZOOM_X4
} CAMS_PARAM_DIGITAL_ZOOM_T;


// =============================================================================
// CAMS_PARAM_CONTRAST_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_CONTRAST camera contrast.
// =============================================================================
typedef enum
{
    CAMS_PARAM_CONTRAST_0,
    CAMS_PARAM_CONTRAST_1,
    CAMS_PARAM_CONTRAST_2,
    CAMS_PARAM_CONTRAST_3,
    CAMS_PARAM_CONTRAST_4,
    CAMS_PARAM_CONTRAST_5,
    CAMS_PARAM_CONTRAST_6,
    CAMS_PARAM_CONTRAST_7
} CAMS_PARAM_CONTRAST_T;


// =============================================================================
// CAMS_PARAM_BRIGHTNESS_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_BRIGHTNESS camera brightness.
// =============================================================================
typedef enum
{
    CAMS_PARAM_BRIGHTNESS_0,
    CAMS_PARAM_BRIGHTNESS_1,
    CAMS_PARAM_BRIGHTNESS_2,
    CAMS_PARAM_BRIGHTNESS_3,
    CAMS_PARAM_BRIGHTNESS_4,
    CAMS_PARAM_BRIGHTNESS_5,
    CAMS_PARAM_BRIGHTNESS_6,
    CAMS_PARAM_BRIGHTNESS_7
} CAMS_PARAM_BRIGHTNESS_T;



// =============================================================================
// CAMS_PARAM_NIGHT_MODE_T
// -----------------------------------------------------------------------------
/// Possible values for the #CAMS_PARAM_NIGHT_MODE camera night mode.
// =============================================================================
typedef enum
{
    CAMS_PARAM_NIGHT_MODE_OFF,
    CAMS_PARAM_NIGHT_MODE_ON
} CAMS_PARAM_NIGHT_MODE_T;


// =============================================================================
// CAMS_RESOLUTION_T
// -----------------------------------------------------------------------------
/// This type defines the resolution to use to take a picture.
// =============================================================================
typedef enum
{
    /// VGA resolution: 640x480
    CAMS_RESOLUTION_VGA     = 0,

    /// QVGA resolution: 320x240
    CAMS_RESOLUTION_QVGA    = 1,

    /// QQVGA resolution: 160x120
    CAMS_RESOLUTION_QQVGA   = 2,

    CAMS_RESOLUTION_QTY
} CAMS_RESOLUTION_T ;


// =============================================================================
// CAMS_PREVIEW_HANDLER_T
// -----------------------------------------------------------------------------
/// This type describes the user function called by the preview process
/// everytime a frame has been captured. The parameter is a pointer to a frame
/// buffer window, whose region of interest if filled with the captured picture,
/// and that the handler can use to produce the display image output on the
/// screen (add OSD, etc).
// =============================================================================
typedef VOID (*CAMS_PREVIEW_HANDLER_T)(LCDD_FBW_T*);


// =============================================================================
// CAMS_CAPTURE_HANDLER_T
// -----------------------------------------------------------------------------
/// This type describes the user function called by the capture process
/// after a frame has been captured. The parameters are two frame buffer
/// window pointers. The first receive the full resolution image, that the
/// handler can encode and save in memory. The second is  a preview quality
/// image, that can be used to show to the user.
// =============================================================================
typedef VOID (*CAMS_CAPTURE_HANDLER_T)(LCDD_FBW_T*, LCDD_FBW_T*);


// =============================================================================
// FUNCTIONS
// =============================================================================

// ==============================================================================
// cams_Open
// ------------------------------------------------------------------------------
/// Open the camera service, and start the underlying task. This function must
/// be called before any other function of the camera service can be called.
// ==============================================================================
PUBLIC CAMS_ERR_T cams_Open(VOID);


// =============================================================================
// cams_Close
// -----------------------------------------------------------------------------
/// When the camera service is no more needed, this function closes the service
/// and put the task to sleep. No other function of the service can be called,
/// but the #cams_Open function.
/// @return #CAMS_ERR_NO, #CAMS_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC CAMS_ERR_T cams_Close(VOID);


// =============================================================================
// cams_StartPreview
// -----------------------------------------------------------------------------
/// Start the preview process to aim the camera. The preview picture is
/// displayed/output in a frame buffer window. This frame buffer window is
/// automatically refreshed as long as the #cams_StopPreview function is not
/// called.
///
/// Everytime a preview frame has been filled in the frame buffer window, the
/// \c prvwHandler is called with the filled \c prvwFbw frame buffer window
/// as a parameter.
///
/// @param prvwFbw Frame buffer window where the preview result will be stored.
/// The image will be resized if needed (ie resolution different from the
/// window size).
/// @param frameRate Preview Frame Rate
/// @param prvwHandler Handler called every time a preview frame as been
/// captured in the preview frame buffer window \c prvwFbw.
/// @return #CAMS_ERR_NO, #CAMS_ERR_RESOURCE_BUSY, #CAMS_ERR_NO_DEVICE, ...
/// @todo How do we define the refresh rate ? (15fps/asap by tacit default)
// =============================================================================
PUBLIC CAMS_ERR_T cams_StartPreview(LCDD_FBW_T* prvwFbw,
                                    UINT8 frameRate,
                                    CAMS_PREVIEW_HANDLER_T prvwHandler);


// =============================================================================
// cams_StopPreview
// -----------------------------------------------------------------------------
/// Stop the preview process started by #cams_StartPreview. It is stopped after
/// the current frame processing is finished.
///
/// @return #CAMS_ERR_NO.
// =============================================================================
PUBLIC CAMS_ERR_T cams_StopPreview(VOID);


// =============================================================================
// cams_PreviewInProgress
// -----------------------------------------------------------------------------
/// Check if a preview is in progress.
/// @return \c TRUE if a preview is in progress, \c FALSE otherwise
// =============================================================================
PUBLIC BOOL cams_PreviewInProgress(VOID);


// =============================================================================
// cams_Capture
// -----------------------------------------------------------------------------
/// Capture one picture. The full size image is stored in the \c dataFbw, and
/// a preview version is stored in the \c prvwFbw frame buffer window. The
/// dataFbw frame buffer window shall be a full (ie the region of interest is
/// the full frame buffer) frame buffer window at the same size as the desired
/// resolution.
///
/// Once the capture is done, the \c captureHandler handler is called. It has
/// two parameters: \c dataFbw, holding the full resolution image, and
/// \c prvwFbw, with a scaled picture for the preview on the screen.
///
/// @param resolution Resolution of the picture to take.
/// @param dataFbw Frame buffer window where the full resolution picture is
/// saved. Must have the \c resolution's width and height.
/// @param prvwFbw Frame buffer window where a preview of the picture is stored.
/// It can have any dimension.
/// @param captureHandler User handler called when a picture is captured.
/// The two frame buffer window are passed as parameters to the handler:
/// the first one with the full resolution captured photograph, and the other
/// one for the picture preview.
/// @return #CAMS_ERR_NO, #CAMS_ERR_RESOURCE_BUSY, #CAMS_ERR_NO_DEVICE, ...
// =============================================================================
PUBLIC CAMS_ERR_T cams_Capture( LCDD_FBW_T*             dataFbw,
                                LCDD_FBW_T*             prvwFbw,
                                CAMS_CAPTURE_HANDLER_T  captureHandler);


// =============================================================================
// cams_SetParam
// -----------------------------------------------------------------------------
/// Set a camera parameter to a given value.
/// The parameter to set is among the values defined by the type #CAMS_PARAM_T.
/// If this parameter is not available for a given implementation of CAMS,
/// the #CAMS_ERR_UNSUPPORTED_PARAM error must be returned.
/// Depending on the CAMS_PARAM_XXX parameter to set, the value can either
/// be a proper UINT32 number, or a value of the CAMS_PARAM_XXX_T enum, might
/// it exists. This is detailed in the CAMS_PARAM_T definition.
/// If the value is not supported for this parameter, #CAMS_ERR_UNSUPPORTED_VALUE
/// is returned.
///
/// @param param Camera parameter to set.
/// @param value Value to set to the parameter to.
/// @return #CAMS_ERR_NO, #CAMS_ERR_UNSUPPORTED_PARAM, #CAMS_ERR_UNSUPPORTED_VALUE
/// ...
// =============================================================================
PUBLIC CAMS_ERR_T cams_SetParam(CAMS_PARAM_T param, UINT32 value);


// =============================================================================
// cams_ResizeRawToRequested
// -----------------------------------------------------------------------------
/// Fast resize of a Raw frame buffer window into a requested size frame
/// buffer window
/// @param rawFbw Camera source frame buffer window.
/// @param reqFbw Requested frame buffer window.
/// @param rotate Do we need to do a rotation of the screen?
/// @param zoom ZOOM_PRECISION is a zoom factor of 1
// =============================================================================
PUBLIC VOID cams_ResizeRawToRequested( LCDD_FBW_T* rawFbw,
                                       LCDD_FBW_T* reqFbw,
                                       UINT32      rotate,
                                       UINT32      zoom);


/// @} //<-- End of the cams group.

#endif // _CAMS_M_H_


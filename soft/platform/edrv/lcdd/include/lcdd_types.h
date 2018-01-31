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


#ifndef LCDD_TYPES_H
#define LCDD_TYPES_H

#ifdef __PRJ_WITH_SPILCD__
#include "hal_gpio.h"
#endif
// ============================================================================
// Defines
// ============================================================================

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// LCDD_ERR_T
// ----------------------------------------------------------------------------
/// Error codes used by the LCD API to return function executions status.
// ============================================================================
typedef enum
{
    /// No error occured
    LCDD_ERR_NO=0,

    /// No valid LCD device was found. Maybe you didn't install the
    /// LCD drive or the LCD device is bad. The LCD device should support
    /// the 'falt frame' display buffer type.
    LCDD_ERR_DEVICE_NOT_FOUND,

    /// The LCD was not properly opened before a call.
    /// to a driver function. #lcdd_Open must be called before
    /// the function producing with result.
    LCDD_ERR_NOT_OPENED,

    /// An invalid parameter was passed to a function.
    LCDD_ERR_INVALID_PARAMETER,

    /// The LCD screen is busy with another command in progress
    LCDD_ERR_RESOURCE_BUSY


} LCDD_ERR_T;






// ============================================================================
// LCDD_SCREEN_INFO_T
// ----------------------------------------------------------------------------
/// This structures stores relevant information about the LCD screen
// ============================================================================
typedef struct
{
    /// Specifies the width of the LCD screen in pixel.
    UINT16 width;

    /// Specifies the heigth of the LCD screen in pixel.
    UINT16 height;

    /// Specifies the bit depth of the LCD device, such as 8bit,16bit etc...
    UINT8 bitdepth;

    /// Reserved for future uses
    UINT16 nReserved;

} LCDD_SCREEN_INFO_T;



//=============================================================================
// LCDD_ROI_T
//-----------------------------------------------------------------------------
/// Region of interest in a LCD screen or a buffer.
//=============================================================================
typedef struct
{
    /// x coordinate of the top left corner of the region.
    UINT16 x;

    /// y coordinate of the top left corner of the region.
    UINT16 y;

    /// Width of the region.
    UINT16 width;

    /// Height of the region.
    UINT16 height;
} LCDD_ROI_T;


// =============================================================================
// LCDD_COLOR_FORMAT_T
// -----------------------------------------------------------------------------
/// This types describes how the color are encoded within a frame buffer.
// =============================================================================
typedef enum
{
    LCDD_COLOR_FORMAT_RGB_INVALID,

    LCDD_COLOR_FORMAT_RGB_565,

    LCDD_COLOR_FORMAT_QTY
} LCDD_COLOR_FORMAT_T;


//=============================================================================
// LCDD_FB_T
//-----------------------------------------------------------------------------
/// Frame buffer type: this type describes a frame buffer through its dimensions
/// and a pointer to the data of the buffer. It is mandatory for the buffer to
/// be able to hold the data describing all the pixels at the color format
/// choosen.
///(ie sizeof(buffer) = width*height*NbOfBytesPerPixelAtTheFormat(format))
//=============================================================================
typedef struct
{
    UINT16*             buffer;
    UINT16              width;
    UINT16              height;
    LCDD_COLOR_FORMAT_T colorFormat;
} LCDD_FB_T;


//=============================================================================
// LCDD_FBW_T
//-----------------------------------------------------------------------------
/// Frame buffer window. This type describes a window in a frame buffer as a
/// region of interest within this frame buffer. When the region of interest
/// starts at (0,0) and has the same dimensions as the frame buffer, the
/// window is in fact the whole frame buffer.
//=============================================================================
typedef struct
{
    LCDD_FB_T   fb;
    LCDD_ROI_T  roi;
} LCDD_FBW_T;

#ifdef __PRJ_WITH_SPILCD__
// =============================================================================
// TGT_SPILCD_CFG
// -----------------------------------------------------------------------------
/// This structure describes the SPILCD configuration for a given target.
// =============================================================================
typedef struct
{
    /// \c ID of the pin to LCD A0
    HAL_APO_ID_T lcdA0Pin;
    /// \c ID of the pin to reset LCD
    HAL_APO_ID_T lcdResetPin;
} SPILCD_CONFIG_STRUCT_T;
#endif

#include "gfx_types.h"

#endif // LCDD_TYPES_H

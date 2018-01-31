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


#ifndef LCDD_M_H
#define LCDD_M_H

#include "lcdd_types.h"

// ============================================================================
// Defines
// ============================================================================

/// The default LCD transfer mode is 16 bits parallel with DMA support.
/// Please export those two macros(LCDD_WITHOUT_DMA and LCDD_8_BITS_PARALLEL) in
/// target.def if you want to use 8 bits LCD or use 16 bits mode without DMA
/// support.
#if (!defined(LCDD_WITHOUT_DMA) && defined(LCDD_8_BITS_PARALLEL))
#error "LCDD configuration not allowed: DMA in 8-bit mode"
#endif

// ============================================================================
// Types
// ============================================================================


EXTERN_C_START

// ============================================================================
// Functions
// ============================================================================


// ============================================================================
// lcdd_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Open(VOID);



// ============================================================================
// lcdd_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to
/// #lcdd_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Close(VOID);



// ============================================================================
// lcdd_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetContrast(UINT32 contrast);



// ============================================================================
// lcdd_SetBrightness
// ----------------------------------------------------------------------------
/// Set the brightness of the 'main'LCD screen.
/// @param brightness Value to set the brightness to (0-7)
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetBrightness(UINT32 brightness);



// ============================================================================
// lcdd_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetStandbyMode(BOOL standbyMode);



// ============================================================================
// lcdd_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Sleep(VOID);



// ============================================================================
// lcdd_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_WakeUp(VOID);



// ============================================================================
// lcdd_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo);



// ============================================================================
// lcdd_SetPixel16
// ----------------------------------------------------------------------------
/// Draw a 16-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 16-bit pixel data to draw.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetPixel16(UINT16 x, UINT16 y, UINT16 pixelData);


// ============================================================================
// lcdd_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of a the active window  with some color
/// @param bgColor Color with which to fill the active window. It is a 16-bit
/// data, as the one of #lcdd_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
#ifdef LCDD_GFX_TYPES
PUBLIC LCDD_ERR_T lcdd_FillRect16(CONST GFX_ROI_T* regionOfInterrest, UINT16 bgColor);
#else
DEPRECATED PUBLIC LCDD_ERR_T lcdd_FillRect16(CONST LCDD_ROI_T* regionOfInterrest, UINT16 bgColor);
#endif /* LCDD_GFX_TYPES */



// ============================================================================
// lcdd_Blit16
// ----------------------------------------------------------------------------
/// This function provides the basic bit-block transfer capabilities.
/// This function copies the data (such as characters/bmp) on the LCD directly
/// as a (rectangular) block. The data is drawn in the active window.
/// @param frameBufferWin Pointer to the structure describing the data to be displayed
/// as a block. The dimension of this block are the ones of the active
/// window.
/// @param startX Top left corner X coordinate of the display zone to modify
/// @param startY Top left corner Y coordinate of the display zone to modify
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
#ifdef LCDD_GFX_TYPES
PUBLIC LCDD_ERR_T lcdd_Blit16(CONST GFX_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY);
#else
DEPRECATED PUBLIC LCDD_ERR_T lcdd_Blit16(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY);
#endif /* LCDD_GFX_TYPES */



// ============================================================================
// lcdd_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_PartialOn(UINT16 vsa, UINT16 vea);



// ============================================================================
// lcdd_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_PartialOff(VOID);



// ============================================================================
// lcdd_SubOpen
// ----------------------------------------------------------------------------
/// Open the LCDD subscreen driver.
/// It must be called before any call to any other function of this driver
/// concerning a subscreen
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubOpen(VOID);



// ============================================================================
// lcdd_SubClose
// ----------------------------------------------------------------------------
/// Close the LCDD subscreen driver
/// No other subscreen functions of this driver should be called after a call to
/// #lcdd_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubClose(VOID);



// ============================================================================
// lcdd_SubSetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the LCD subscreen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSetContrast(UINT32 contrast);



// ============================================================================
// lcdd_SubSetBrightness
// ----------------------------------------------------------------------------
/// Set the brightness of the LCD subscreen.
/// @param brightness Value to set the brightness to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSetBrightness(UINT32 brightness);



// ============================================================================
// lcdd_SubSetBacklight
// ----------------------------------------------------------------------------
/// Set the backlight of the LCD subscreen.
/// @param  backlight Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSetBacklight(UINT32 backlight);



// ============================================================================
// lcdd_SubSetStandbyMode
// ----------------------------------------------------------------------------
/// Set the LCD subscreen in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSetStandbyMode(BOOL standbyMode);



// ============================================================================
// lcdd_SubSleep
// ----------------------------------------------------------------------------
/// Set the LCD subscreen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSleep(VOID);



// ============================================================================
// lcdd_SubWakeUp
// ----------------------------------------------------------------------------
/// Wake the LCD subscreen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubWakeUp(VOID);



// ============================================================================
// lcdd_SubGetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the sub LCD device.
/// @param pscreenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubGetScreenInfo(LCDD_SCREEN_INFO_T* pscreenInfo);

PUBLIC LCDD_ERR_T lcdd_SetSubStandbyMode(BOOL standbyMode);

PUBLIC LCDD_ERR_T lcdd_SubSetPixel16(UINT16 x, UINT16 y, UINT16 pixelData);

PUBLIC LCDD_ERR_T lcdd_SubFillRect16(CONST LCDD_ROI_T* regionOfInterrest, UINT16 bgColor);

PUBLIC LCDD_ERR_T lcdd_SubBlit16(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY);

// ============================================================================
// lcdd_SubSetPixel12
// ----------------------------------------------------------------------------
/// Draw a 12-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 12-bit pixel data to draw.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSetPixel12(UINT16 x, UINT16 y, UINT16 pixelData);



// ============================================================================
// lcdd_SubFillRect12
// ----------------------------------------------------------------------------
/// This function performs a fill of the subscreen active window with some
/// color.
/// @param bgColor Color with which to fill the subscreen active window.
/// It is a 12-bit data, as the one of #lcdd_SubSetPixel12
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED.
// ============================================================================
#ifdef LCDD_GFX_TYPES
PUBLIC LCDD_ERR_T lcdd_SubFillRect12(GFX_ROI_T* regionOfInterrest, UINT16 bgColor);
#else
DEPRECATED PUBLIC LCDD_ERR_T lcdd_SubFillRect12(LCDD_ROI_T* regionOfInterrest, UINT16 bgColor);
#endif /* LCDD_GFX_TYPES */



// ============================================================================
// lcdd_SubBlit12
// ----------------------------------------------------------------------------
/// This function provides the basic bit-block transfer capabilities for the
/// subcreen.
/// This function copies the data (such as characters/bmp) on the LCD directly
/// as a (rectangular) block. The data is drawn in the subscreen active window.
/// @param pPixelData Pointer to the buffer holding the data to be displayed
/// as a block. The dimension of this block is the one of the subscreen active
/// window.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED.
// ============================================================================
#ifdef LCDD_GFX_TYPES
PUBLIC LCDD_ERR_T lcdd_SubBlit12(GFX_FBW_T frameBufferWin);
#else
DEPRECATED PUBLIC LCDD_ERR_T lcdd_SubBlit12(LCDD_FBW_T frameBufferWin);
#endif /* LCDD_GFX_TYPES */



// ============================================================================
// lcdd_Busy
// ----------------------------------------------------------------------------
/// This function checks the status of the LCD Controller.  It is used to
/// avoid starting a new transfer before the previous one has completed
/// @return TRUE if the controller is busy with a transfer, FALSE if the
/// controller can be used for a new transfer
// ============================================================================
PUBLIC BOOL lcdd_Busy(VOID);

PUBLIC BOOL lcdd_SetDirRotation(VOID);

PUBLIC BOOL lcdd_SetDirDefault(VOID);

PUBLIC char* lcdd_GetStringId(VOID);

PUBLIC UINT16 lcdd_GetLcdId(VOID);

PUBLIC VOID lcdd_ChangeLCDSPIFreq(BOOL high);
// ============================================================================
// lcdd_LayerMerge
// ----------------------------------------------------------------------------
/// This function provides the layer merging capabilities.
/// This function merges the overlay layer data to a given buffer or to a LCD screen.
///
/// @param frameBufferWin Pointer to the LCD frame buffer structure.
/// @param startX The coordinates of the start position.
/// @param startY The coordinates of the start position.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
LCDD_ERR_T lcdd_LayerMerge(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY,BOOL merge_state);

VOID lcd_set_overlay(
    U8 layer_id,
    CONST  UINT16*  frameBuffer,
    UINT16 srcX,
    UINT16 srcY,
    UINT16 src_pitch,
    UINT16 startX,
    UINT16 startY,
    UINT16 endX,
    UINT16 endY,
    BOOL src_key_enable,
    UINT32 src_key,
    UINT8 alpha
);

LCDD_ERR_T lcdd_GoudaMerge(
    CONST  UINT16*  frameBuffer,
    UINT16 startX,
    UINT16 startY,
    UINT16 endX,
    UINT16 endY,
    UINT16 width);

PUBLIC VOID lcd_SemFree(VOID);

PUBLIC VOID lcd_SemTake(VOID);

PUBLIC VOID lcdd_EnableVidLayerMerge(BOOL on);

EXTERN_C_END

/// @} //end of lcdd group

#endif // LCDD_M_H

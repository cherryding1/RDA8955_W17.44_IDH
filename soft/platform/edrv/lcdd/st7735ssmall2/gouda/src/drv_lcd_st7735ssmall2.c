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

#include "lcdd_config.h"
#include "lcdd_private.h"
#include "hal_timers.h"
#include "hal_gouda.h"
#include "lcddp_debug.h"
#include "hal_host.h"
#include "pmd_m.h"
#include "sxr_tls.h"

// To get config
#include "lcdd_tgt_params_gallite.h"
#include "lcdd.h"

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================

#define LCM_WR_DATA_8(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD_8(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}

#define LCDD_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width
#define LCDD_DISP_X     128

// Number of pixels in the display height
#define LCDD_DISP_Y     128

#define  LCD_ST7735S_ID        0x7C7C

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// LCDD screen config.
PRIVATE CONST LCDD_CONFIG_T g_tgtLcddCfg = TGT_LCDD_CONFIG;

PRIVATE BOOL g_lcddRotate = FALSE;

// wheter lcddp_GoudaBlitHandler() has to close ovl layer 0
PRIVATE BOOL g_lcddAutoCloseLayer = FALSE;

// Sleep status of the LCD
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// lcddp_GoudaBlitHandler
// -----------------------------------------------------------------------------
/// This function frees the lock to access the screen. It is set as the user
/// handler called by the DMA driver at the end of the writings on the screen.
// =============================================================================
PRIVATE VOID lcddp_GoudaBlitHandler(VOID)
{
    lcdd_MutexFree();
    if(g_lcddAutoCloseLayer)
    {
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
        g_lcddAutoCloseLayer = FALSE;
    }
}


// =============================================================================
// lcddp_Init
// -----------------------------------------------------------------------------
/// This function initializes LCD registers after powering on or waking up.
// =============================================================================
PRIVATE VOID lcddp_Init(VOID)
{
    // Init code
    LCM_WR_CMD_8(0x01);
    sxr_Sleep(200 MS_WAITING);
    //----------------------End ST7735S Reset Sequence --------------------------------------//
//天亿富7735s +汉彩
    LCM_WR_CMD_8(0x11); //Sleep out
    sxr_Sleep(120); //sxr_Sleep 120ms
//------------------------------------ST7735S Frame rate-----------------------------------------//
    LCM_WR_CMD_8(0xB1); //Frame rate 80Hz
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
    LCM_WR_CMD_8(0xB2); //Frame rate 80Hz
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
    LCM_WR_CMD_8(0xB3); //Frame rate 80Hz
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
//------------------------------------End ST7735S Frame rate-----------------------------------------//
    LCM_WR_CMD_8(0xB4); //Dot inversion
    LCM_WR_DATA_8(0x00);// 00  03

//------------------------------------ST7735S Power Sequence-----------------------------------------//
    LCM_WR_CMD_8(0xC0);
    LCM_WR_DATA_8(0xA2);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x84);
    LCM_WR_CMD_8(0xC1);
    LCM_WR_DATA_8(0xC5);
    LCM_WR_CMD_8(0xC2);
    LCM_WR_DATA_8(0x0D);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xC3);
    LCM_WR_DATA_8(0x8D);
    LCM_WR_DATA_8(0x2A);
    LCM_WR_CMD_8(0xC4);
    LCM_WR_DATA_8(0x8D);
    LCM_WR_DATA_8(0xEE);
//---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCM_WR_CMD_8(0xC5); //VCOM
    LCM_WR_DATA_8(0x15);      //03  13

    LCM_WR_CMD_8(0x36); //MX, MY, RGB mode
    LCM_WR_DATA_8(0xC8);
//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCM_WR_CMD_8(0XE0);
    LCM_WR_DATA_8(0x07);
    LCM_WR_DATA_8(0x1C);
    LCM_WR_DATA_8(0x10);
    LCM_WR_DATA_8(0x18);
    LCM_WR_DATA_8(0x33);
    LCM_WR_DATA_8(0x2C);
    LCM_WR_DATA_8(0x25);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x27);
    LCM_WR_DATA_8(0x2F);
    LCM_WR_DATA_8(0x3C);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x10);

    LCM_WR_CMD_8(0XE1);
    LCM_WR_DATA_8(0x07);
    LCM_WR_DATA_8(0x1C);
    LCM_WR_DATA_8(0x10);
    LCM_WR_DATA_8(0x18);
    LCM_WR_DATA_8(0x2D);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x23);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x26);
    LCM_WR_DATA_8(0x2F);
    LCM_WR_DATA_8(0x3B);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x10);
//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
    LCM_WR_CMD_8(0x3A); //65k mode
    LCM_WR_DATA_8(0x05);
    sxr_Sleep(220);
    LCM_WR_CMD_8(0x29); //Display on
    /*LCM_WR_CMD_8(0x11); //Sleep out
    sxr_Sleep (120 MS_WAITING); //Delay 120ms

    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    LCM_WR_CMD_8(0xB1);
    LCM_WR_DATA_8(0x05);
    LCM_WR_DATA_8(0x3A);
    LCM_WR_DATA_8(0x3A);
    LCM_WR_CMD_8(0xB2);
    LCM_WR_DATA_8(0x05);
    LCM_WR_DATA_8(0x3A); LCM_WR_DATA_8(0x3A);
    LCM_WR_CMD_8(0xB3);
    LCM_WR_DATA_8(0x05);
    LCM_WR_DATA_8(0x3A);
    LCM_WR_DATA_8(0x3A);
    LCM_WR_DATA_8(0x05);
    LCM_WR_DATA_8(0x3A); LCM_WR_DATA_8(0x3A);
    //------------------------------------End ST7735S Frame Rate-----------------------------------------//

    LCM_WR_CMD_8(0xB4); //Dot inversion
    LCM_WR_DATA_8(0x03);

    //------------------------------------ST7735S Power Sequence-----------------------------------------//
    LCM_WR_CMD_8(0xC0);
    LCM_WR_DATA_8(0x62);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x04);
    LCM_WR_CMD_8(0xC1);
    LCM_WR_DATA_8(0xC0);
    LCM_WR_CMD_8(0xC2);
    LCM_WR_DATA_8(0x0D);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xC3);
    LCM_WR_DATA_8(0x8D);
    LCM_WR_DATA_8(0x6A);
    LCM_WR_CMD_8(0xC4);
    LCM_WR_DATA_8(0x8D);
    LCM_WR_DATA_8(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//

    LCM_WR_CMD_8(0xC5); //VCOM
    LCM_WR_DATA_8(0x12);

    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCM_WR_CMD_8(0xE0);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x1B);
    LCM_WR_DATA_8(0x12);
    LCM_WR_DATA_8(0x11);
    LCM_WR_DATA_8(0x3F);
    LCM_WR_DATA_8(0x3A);
    LCM_WR_DATA_8(0x32);
    LCM_WR_DATA_8(0x34);
    LCM_WR_DATA_8(0x2F);
    LCM_WR_DATA_8(0x2B);
    LCM_WR_DATA_8(0x30);
    LCM_WR_DATA_8(0x3A);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x01);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x05);

    LCM_WR_CMD_8(0xE1);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x1B);
    LCM_WR_DATA_8(0x12);
    LCM_WR_DATA_8(0x11);
    LCM_WR_DATA_8(0x32);
    LCM_WR_DATA_8(0x2F);
    LCM_WR_DATA_8(0x2A);
    LCM_WR_DATA_8(0x2F);
    LCM_WR_DATA_8(0x2E);
    LCM_WR_DATA_8(0x2C);
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x3F);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x01);
    LCM_WR_DATA_8(0x05);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//

    LCM_WR_CMD_8(0xFC); //Enable Gate power save mode
    LCM_WR_DATA_8(0x8C);
    LCM_WR_CMD_8(0x3A); //65k mode
    LCM_WR_DATA_8(0x05);
    LCM_WR_CMD_8(0x29); //Display on*/
#if 0
//--************ Start Initial Sequence **********--//
    LCM_WR_CMD_8(0x11); //Exit Sleep
    sxr_Sleep(120 MS_WAITING);

    LCM_WR_CMD_8(0x11); //Sleep out
    hal_TimDelay(120 MS_WAITING);  //Delay 120ms
//------------------------------------ST7715R Frame rate-----------------------------------------//
    LCM_WR_CMD_8(0xB1); //Frame rate 80Hz
    LCM_WR_DATA_8(0x02);  //0x02
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
    LCM_WR_CMD_8(0xB2); //Frame rate 80Hz
    LCM_WR_DATA_8(0x02);   //0x02
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
    LCM_WR_CMD_8(0xB3); //Frame rate 80Hz
    LCM_WR_DATA_8(0x02);   //0x02
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x35);
    LCM_WR_DATA_8(0x36);
//------------------------------------End ST7715R Frame rate-----------------------------------------//
    LCM_WR_CMD_8(0xB4); //Column inversion
    LCM_WR_DATA_8(0x03);


    LCM_WR_CMD_8(0xB6);   //
    LCM_WR_DATA_8(0xB4); //
    LCM_WR_DATA_8(0xF0); //

//------------------------------------ST7715R Power Sequence-----------------------------------------//
    LCM_WR_CMD_8(0xC0);
    LCM_WR_DATA_8(0xA2);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x84);
    LCM_WR_CMD_8(0xC1);
    LCM_WR_DATA_8(0xC5);
    LCM_WR_CMD_8(0xC2);
    LCM_WR_DATA_8(0x0A);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xC3);
    LCM_WR_DATA_8(0x8A);
    LCM_WR_DATA_8(0x2A);
    LCM_WR_CMD_8(0xC4);
    LCM_WR_DATA_8(0x8A);
    LCM_WR_DATA_8(0xEE);
//---------------------------------End ST7715R Power Sequence-------------------------------------//
    LCM_WR_CMD_8(0xC5); //VCOM
    LCM_WR_DATA_8(0x07);   //0x03
    LCM_WR_CMD_8(0x36); //MX, MY, RGB mode
    LCM_WR_DATA_8(0xC8);
//------------------------------------ST7715R Gamma Sequence-----------------------------------------//
    LCM_WR_CMD_8(0xe0);
    LCM_WR_DATA_8(0x12);
    LCM_WR_DATA_8(0x1c);//亮度调节1C ->2C
    LCM_WR_DATA_8(0x10);
    LCM_WR_DATA_8(0x18);
    LCM_WR_DATA_8(0x33);
    LCM_WR_DATA_8(0x2c);
    LCM_WR_DATA_8(0x25);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x27);
    LCM_WR_DATA_8(0x2f);
    LCM_WR_DATA_8(0x3c);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x10);

    LCM_WR_CMD_8(0xe1);
    LCM_WR_DATA_8(0x12);
    LCM_WR_DATA_8(0x1c);  //亮度调节
    LCM_WR_DATA_8(0x10);
    LCM_WR_DATA_8(0x18);
    LCM_WR_DATA_8(0x2d);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x23);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x28);
    LCM_WR_DATA_8(0x26);
    LCM_WR_DATA_8(0x2f);
    LCM_WR_DATA_8(0x3b);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x10);

    /*
    LCM_WR_CMD_8(0x2a);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x02);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x81);

    LCM_WR_CMD_8(0x2B);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x03);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(0x82);
    */
//------------------------------------End ST7715R Gamma Sequence-----------------------------------------//
    LCM_WR_CMD_8(0xF0); //Enable test command
    LCM_WR_DATA_8(0x01);
    LCM_WR_CMD_8(0xF6); //Disable ram power save mode
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0x3A); //65k mode
    LCM_WR_DATA_8(0x05);
    LCM_WR_CMD_8(0x29); //Display on
#endif
    sxr_Sleep(220 MS_WAITING);
    LCM_WR_CMD_8(0x2C);
    sxr_Sleep(50 MS_WAITING);

}

// ============================================================================
// lcddp_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_Open(VOID)
{
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);



    // Init code
    sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms

    lcddp_Init();

    g_lcddInSleep = FALSE;

    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to
/// #lcddp_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_Close(VOID)
{

    hal_GoudaClose();

    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_SetContrast(UINT32 contrast)
{
    //#warning This function is not implemented yet
    return LCDD_ERR_NO;
}



// ============================================================================
// lcddp_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_SetStandbyMode(BOOL standbyMode)
{
    if (standbyMode)
    {
        lcddp_Sleep();
    }
    else
    {
        lcddp_WakeUp();
    }
    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================

PRIVATE LCDD_ERR_T lcddp_Sleep(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Sleep while another LCD operation in progress. Sleep %d ticks",
                   LCDD_TIME_MUTEX_RETRY);
    }

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
    LCM_WR_CMD_8(0x28);           // Display off
    sxr_Sleep(50 MS_WAITING);
    LCM_WR_CMD_8(0x10);
    sxr_Sleep(100 MS_WAITING);

    LCDD_TRACE(TSTDOUT, 0, "lcddp_Sleep: calling hal_GoudaClose");
    hal_GoudaClose();

    g_lcddInSleep = TRUE;

    lcdd_MutexFree();

    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_PartialOn(UINT16 vsa, UINT16 vea)
{
    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_PartialOff(VOID)
{
    return LCDD_ERR_NO;
}

// ============================================================================
// lcddp_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_WakeUp(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Wakeup while another LCD operation in progress. Sleep %d ticks",
                   LCDD_TIME_MUTEX_RETRY);
    }

    if (!g_lcddInSleep)
    {
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }

    LCDD_TRACE(TSTDOUT, 0, "lcddp_WakeUp: calling hal_GoudaOpen");
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);


    sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms
    LCM_WR_CMD_8(0x11); //Exit Sleep
    sxr_Sleep(120 MS_WAITING);
    lcddp_Init();


    g_lcddInSleep = FALSE;

    lcdd_MutexFree();

    // Set a comfortable background color to avoid screen flash
    LCDD_FBW_T frameBufferWin;
    frameBufferWin.fb.buffer = NULL;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;
    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;

    if (g_lcddRotate)
    {
        frameBufferWin.roi.width = LCDD_DISP_Y;
        frameBufferWin.roi.height = LCDD_DISP_X;
        frameBufferWin.fb.width = LCDD_DISP_Y;
        frameBufferWin.fb.height = LCDD_DISP_X;
    }
    else
    {
        frameBufferWin.roi.width = LCDD_DISP_X;
        frameBufferWin.roi.height = LCDD_DISP_Y;
        frameBufferWin.fb.width = LCDD_DISP_X;
        frameBufferWin.fb.height = LCDD_DISP_Y;
    }
    lcddp_Blit16(&frameBufferWin,frameBufferWin.roi.x,frameBufferWin.roi.y);

    return LCDD_ERR_NO;
}


// ============================================================================
// lcddp_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo)
{
    {
        screenInfo->width = LCDD_DISP_X;
        screenInfo->height = LCDD_DISP_Y;
        screenInfo->bitdepth = LCDD_COLOR_FORMAT_RGB_565;
        screenInfo->nReserved = 0;
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcddp_SetPixel16
// ----------------------------------------------------------------------------
/// Draw a 16-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 16-bit pixel data to draw.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_SetPixel16(UINT16 x, UINT16 y, UINT16 pixelData)
{
    if (0 == lcdd_MutexGet())
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }

        LCM_WR_CMD_8(0x2a);
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(x+2);
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(x+2);

        LCM_WR_CMD_8(0x2b);
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(y+0x00);//0x21->0x0
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(y+0x00);//0x21->0x00

        LCM_WR_CMD_8(0x2c);         // vertical RAM address position
        LCM_WR_DATA_8(pixelData);

        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcddp_BlitRoi2Win
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD
// ============================================================================
PRIVATE VOID lcddp_BlitRoi2Win(CONST HAL_GOUDA_WINDOW_T* pRoi, CONST HAL_GOUDA_WINDOW_T* pActiveWin)
{
    HAL_GOUDA_LCD_CMD_T cmd[13];

    if(!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        LCDD_TRACE(LCDD_WARN_TRC,0,"lcddp_BlitRoi2Win: Invalid Roi x:%d < %d, y:%d < %d",pRoi->tlPX, pRoi->brPX, pRoi->tlPY, pRoi->brPY);
        lcddp_GoudaBlitHandler();
        return;
    }

    // if(g_lcddRotate)
    LCM_WR_CMD_8(0x2A);  //Set Column Address
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->tlPX+2);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->brPX+2);

    LCM_WR_CMD_8(0x2B);  //Set Page Address
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->tlPY+33); //0x21->0x00  wmz modiy
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->brPY+33); //0x21->0x00

    LCM_WR_CMD_8(0x2c); //WRITE ram Data display

    // Send command after which the data we sent
    // are recognized as pixels.
#ifdef __USE_LCD_FMARK__
    if (lcdd_FmarkGetStatus() == TRUE)
    {
        lcdd_FmarkSetBufReady(pRoi);
    }
    else
    {
        while (HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 0, cmd, lcddp_GoudaBlitHandler));
        while (hal_GoudaIsActive());
    }
#else
    while (HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 0, cmd, lcddp_GoudaBlitHandler));
#endif

}


// ============================================================================
// lcddp_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of the active window  with some color.
/// @param bgColor Color with which to fill the rectangle. It is a 16-bit
/// data, as the one of #lcddp_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_FillRect16(CONST LCDD_ROI_T* regionOfInterrest, UINT16 bgColor)
{
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcdd_MutexGet())
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }

        // Set Active window
        activeWin.tlPX = regionOfInterrest->x;
        activeWin.brPX = regionOfInterrest->x + regionOfInterrest->width - 1;
        activeWin.tlPY = regionOfInterrest->y;
        activeWin.brPY = regionOfInterrest->y + regionOfInterrest->height - 1;

        // Check parameters
        // ROI must be within the screen boundary
        BOOL badParam = FALSE;
        if (g_lcddRotate)
        {
            if (    (activeWin.tlPX >= LCDD_DISP_Y) ||
                    (activeWin.brPX >= LCDD_DISP_Y) ||
                    (activeWin.tlPY >= LCDD_DISP_X) ||
                    (activeWin.brPY >= LCDD_DISP_X)
               )
            {
                badParam = TRUE;
            }
        }
        else
        {
            if (    (activeWin.tlPX >= LCDD_DISP_X) ||
                    (activeWin.brPX >= LCDD_DISP_X) ||
                    (activeWin.tlPY >= LCDD_DISP_Y) ||
                    (activeWin.brPY >= LCDD_DISP_Y)
               )
            {
                badParam = TRUE;
            }
        }
        if (badParam)
        {
            lcdd_MutexFree();
            return LCDD_ERR_INVALID_PARAMETER;
        }

        hal_GoudaSetBgColor(bgColor);
        lcddp_BlitRoi2Win(&activeWin,&activeWin);

        return LCDD_ERR_NO;
    }
}

// ============================================================================
// lcddp_Blit16
// ----------------------------------------------------------------------------
/// This function provides the basic bit-block transfer capabilities.
/// This function copies the data (such as characters/bmp) on the LCD directly
/// as a (rectangular) block. The data is drawn in the active window.
/// The buffer has to be properly aligned (@todo define properly 'properly')
///
/// @param pPixelData Pointer to the buffer holding the data to be displayed
/// as a block. The dimension of this block are the one of the #pDestRect
/// parameter
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T lcddp_Blit16(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY)
{
    LCDD_ASSERT((frameBufferWin->fb.width&1) == 0, "LCDD: FBW must have an even number "
                "of pixels per line. Odd support is possible at the price of a huge "
                "performance lost");
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcdd_MutexGet())
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }

        // Set Input window
        inputWin.tlPX = frameBufferWin->roi.x;
        inputWin.brPX = frameBufferWin->roi.x + frameBufferWin->roi.width - 1;
        inputWin.tlPY = frameBufferWin->roi.y;
        inputWin.brPY = frameBufferWin->roi.y + frameBufferWin->roi.height - 1;

        // Set Active window
        activeWin.tlPX = startX;
        activeWin.brPX = startX + frameBufferWin->roi.width - 1;
        activeWin.tlPY = startY;
        activeWin.brPY = startY + frameBufferWin->roi.height - 1;

        // Check parameters
        // ROI must be within the screen boundary
        // ROI must be within the Frame buffer
        // Color format must be 16 bits
        BOOL badParam = FALSE;
        if (g_lcddRotate)
        {
            if (    (activeWin.tlPX >= LCDD_DISP_Y) ||
                    (activeWin.brPX >= LCDD_DISP_Y) ||
                    (activeWin.tlPY >= LCDD_DISP_X) ||
                    (activeWin.brPY >= LCDD_DISP_X)
               )
            {
                badParam = TRUE;
            }
        }
        else
        {
            if (    (activeWin.tlPX >= LCDD_DISP_X) ||
                    (activeWin.brPX >= LCDD_DISP_X) ||
                    (activeWin.tlPY >= LCDD_DISP_Y) ||
                    (activeWin.brPY >= LCDD_DISP_Y)
               )
            {
                badParam = TRUE;
            }
        }
        if (!badParam)
        {
            if (    (frameBufferWin->roi.width > frameBufferWin->fb.width) ||
                    (frameBufferWin->roi.height > frameBufferWin->fb.height) ||
                    (frameBufferWin->fb.colorFormat != LCDD_COLOR_FORMAT_RGB_565)
               )
            {
                badParam = TRUE;;
            }
        }
        if (badParam)
        {
            lcdd_MutexFree();
            return LCDD_ERR_INVALID_PARAMETER;
        }

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers
        if (frameBufferWin->fb.buffer != NULL)
        {
            HAL_GOUDA_OVL_LAYER_DEF_T def;
            // configure ovl layer 0 as buffer
            def.addr = (UINT32*)frameBufferWin->fb.buffer; // what about aligment ?
            def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565; //TODO convert from .colorFormat
            //def.stride = frameBufferWin->fb.width * 2;
            def.stride = 0; // let hal gouda decide
            def.pos.tlPX = 0;
            def.pos.tlPY = 0;
            def.pos.brPX = frameBufferWin->fb.width - 1;
            def.pos.brPY = frameBufferWin->fb.height - 1;
            def.alpha = 255;
            def.cKeyEn = FALSE;

            // open the layer
            hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
            hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID0, &def);
            // tell the end handler not to close the layer when we are done
            g_lcddAutoCloseLayer = FALSE;
        }

        // gouda is doing everything ;)
        lcddp_BlitRoi2Win(&inputWin, &activeWin);

        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcddp_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented for the ebc version of the driver
// ============================================================================
PRIVATE BOOL lcddp_Busy(VOID)
{
    return FALSE;
}


// ============================================================================
// lcddp_SetDirRotation
// ----------------------------------------------------------------------------
///
// ============================================================================
PRIVATE BOOL lcddp_SetDirRotation(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = TRUE;

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return TRUE;
    }


    LCM_WR_CMD_8(0x36);  //Set Scanning Direction
    LCM_WR_DATA_8(0xc8);

    sxr_Sleep(1 MS_WAITING);

    lcdd_MutexFree();

    return TRUE;
}

// ============================================================================
// lcddp_SetDirDefault
// ----------------------------------------------------------------------------
///
// ============================================================================
PRIVATE BOOL lcddp_SetDirDefault(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = FALSE;

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return TRUE;
    }

    LCM_WR_CMD_8(0x36); //Set Scanning Direction
    LCM_WR_DATA_8(0xc8);


    sxr_Sleep(1 MS_WAITING);

    lcdd_MutexFree();

    return TRUE;
}

PRIVATE char* lcdd_get_id_string(void)
{
    static char ST7735Sh_id_str[] = "ST7735S\n";
    return ST7735Sh_id_str;
}
PRIVATE BOOL lcddp_CheckProductId()
{
    UINT16 productId=0;
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
    sxr_Sleep(20 MS_WAITING);

    hal_GoudaReadReg(0xda, &productId);
    hal_GoudaClose();
    hal_HstSendEvent(0x20121116);
    hal_HstSendEvent(productId);
    hal_HstSendEvent(0x20121116);

    SXS_TRACE(TSTDOUT, "ST7735S(0x%x): lcd read id is 0x%x ", LCD_ST7735S_ID, productId);
    if((productId) == LCD_ST7735S_ID)
        return TRUE;
    else
        return FALSE;
}

// ============================================================================
// lcdd_ili9163c_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL  lcdd_st7735ssmall2_RegInit(LCDD_REG_T *pLcdDrv)
{
    if( lcddp_CheckProductId())
    {
        pLcdDrv->lcdd_Open=lcddp_Open;
        pLcdDrv->lcdd_Close=lcddp_Close;
        pLcdDrv->lcdd_SetContrast=lcddp_SetContrast;

        pLcdDrv->lcdd_SetStandbyMode=lcddp_SetStandbyMode;
        pLcdDrv->lcdd_PartialOn=lcddp_PartialOn;
        pLcdDrv->lcdd_PartialOff=lcddp_PartialOff;
        pLcdDrv->lcdd_Blit16=lcddp_Blit16;
        pLcdDrv->lcdd_Busy=lcddp_Busy;
        pLcdDrv->lcdd_FillRect16=lcddp_FillRect16;
        pLcdDrv->lcdd_GetScreenInfo=lcddp_GetScreenInfo;
        pLcdDrv->lcdd_WakeUp=lcddp_WakeUp;
        pLcdDrv->lcdd_SetPixel16=lcddp_SetPixel16;
        pLcdDrv->lcdd_Sleep=lcddp_Sleep;
        pLcdDrv->lcdd_SetDirRotation=lcddp_SetDirRotation;
        pLcdDrv->lcdd_SetDirDefault=lcddp_SetDirDefault;
        pLcdDrv->lcdd_GetStringId=lcdd_get_id_string;
        pLcdDrv->lcdd_GoudaBltHdl = lcddp_GoudaBlitHandler;
        return TRUE;
    }

    return FALSE;
}



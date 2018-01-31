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

//#define ST7789S_DRIVER_1
#define ST7789S_DRIVER_2
//#define ST7789S_DRIVER_3

#ifndef USE_SPI_LCD //def lcd_line_8_low
//low 8bit
#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);   while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO); }
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(Cmd)      != HAL_ERR_NO);}


#else

#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO); }
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}

#endif


#define LCDD_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define LCDD_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define LCDD_BUILD_CMD_WR_REG(c,i,r,d) do{LCDD_BUILD_CMD_WR_CMD(c,i,r); LCDD_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)

#define LCDD_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width
#define LCDD_DISP_X             240
//#define LCDD_DISP_X               176

// Number of pixels in the display height
#define LCDD_DISP_Y             240
//#define LCDD_DISP_Y             220

#define  LCD_ST7789S_ID       0X8552
// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

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

    LCM_WR_CMD(0x11);
    sxr_Sleep(0x120);

#ifdef ST7789S_DRIVER_1
    LCM_WR_DAT(0x36); //Memory Data Access Control
    LCM_WR_DAT(0x00);

    LCM_WR_DAT(0x3A); //Interface Pixel Format
    LCM_WR_DAT(0x05); //MCU-18bit   06

    LCM_WR_DAT(0x2a);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0xef);

    LCM_WR_DAT(0x2b);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0xef);

    LCM_WR_DAT(0xb2); //Porch Setting
    LCM_WR_DAT(0x0C);//Normal BP
    LCM_WR_DAT(0x0C);//Normal FP
    LCM_WR_DAT(0x00);//Enable Seperate
    LCM_WR_DAT(0x33);//idle, BP[7:4], FP[3:0]
    LCM_WR_DAT(0x33);//partial, BP[7:4], FP[3:0]

    LCM_WR_DAT(0xb7); //Gate Control
    LCM_WR_DAT(0x56); //VGH=14.06V, VGL=-11.38

    LCM_WR_DAT(0xbb); //VCOMS Setting
    LCM_WR_DAT(0x1E);

    LCM_WR_DAT(0xc0);
    LCM_WR_DAT(0x2c);

    LCM_WR_DAT(0xc2);
    LCM_WR_DAT(0x01);

    LCM_WR_DAT(0xc3); //VRH Set
    LCM_WR_DAT(0x13); //4.5V

    LCM_WR_DAT(0xc4);
    LCM_WR_DAT(0x20);

    LCM_WR_DAT(0xc6); //Frame Rate Control in Normal Mode
    LCM_WR_DAT(0x0f);

    LCM_WR_DAT(0xd0); //Power Control 1
    LCM_WR_DAT(0xa4); //
    LCM_WR_DAT(0xa1); //AVDD=6.8V, AVCL=-4.8V, VDS=2.3V

//----GAMMA------------------------------------

    LCM_WR_DAT(0xe0);
    LCM_WR_DAT(0xD0);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x08);
    LCM_WR_DAT(0x0E);
    LCM_WR_DAT(0x11);
    LCM_WR_DAT(0x2B);
    LCM_WR_DAT(0x3B);
    LCM_WR_DAT(0x44);
    LCM_WR_DAT(0x4C);
    LCM_WR_DAT(0x2B);
    LCM_WR_DAT(0x16);
    LCM_WR_DAT(0x15);
    LCM_WR_DAT(0x1E);
    LCM_WR_DAT(0x21);

    LCM_WR_DAT(0xe1);
    LCM_WR_DAT(0xD0);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x08);
    LCM_WR_DAT(0x0E);
    LCM_WR_DAT(0x11);
    LCM_WR_DAT(0x2B);
    LCM_WR_DAT(0x3B);
    LCM_WR_DAT(0x54);
    LCM_WR_DAT(0x4C);
    LCM_WR_DAT( 0x2B);
    LCM_WR_DAT(0x16);
    LCM_WR_DAT(0x15);
    LCM_WR_DAT(0x1E);
    LCM_WR_DAT(0x21);
#endif

#ifdef ST7789S_DRIVER_2
    LCM_WR_CMD(0x21);

    // Display Setting
    LCM_WR_CMD(0x36);
    LCM_WR_DAT(0x00);     //0x0C      //0x00      //tony


    LCM_WR_CMD(0xD0);
    LCM_WR_DAT(0xA4);
    LCM_WR_DAT(0xA1);

    LCM_WR_CMD(0xE8);
    LCM_WR_DAT(0x93);

    // VCOM setting
    LCM_WR_CMD(0xBB);
    LCM_WR_DAT(0x27);

    //interface pixel format
    LCM_WR_CMD(0x3A);
    LCM_WR_DAT(0x55);

    // Gamma setting for Tainma 2.4 panel
    LCM_WR_CMD(0xE0);
    LCM_WR_DAT(0xD0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x08);
    LCM_WR_DAT(0x0D);
    LCM_WR_DAT(0x18);
    LCM_WR_DAT(0x2E);
    LCM_WR_DAT(0x54);
    LCM_WR_DAT(0x3F);
    LCM_WR_DAT(0x0C);
    LCM_WR_DAT(0x18);
    LCM_WR_DAT(0x14);
    LCM_WR_DAT(0x14);
    LCM_WR_DAT(0x18);

    LCM_WR_CMD(0xE1);
    LCM_WR_DAT(0xD0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x08);
    LCM_WR_DAT(0x06);
    LCM_WR_DAT(0x25);
    LCM_WR_DAT(0x2B);
    LCM_WR_DAT(0x44);
    LCM_WR_DAT(0x41);
    LCM_WR_DAT(0x0C);
    LCM_WR_DAT(0x17);
    LCM_WR_DAT(0x18);
    LCM_WR_DAT(0x13);
    LCM_WR_DAT(0x17);

    // Porch setting
    LCM_WR_CMD(0xB2); // Porch setting
    LCM_WR_DAT(0x0C);       // FPA
    LCM_WR_DAT(0x0C);       // BPA
    LCM_WR_DAT(0x0);        // PSEN
    LCM_WR_DAT(0x33);       // BPB FPB
    LCM_WR_DAT(0x33);       // BPC FPC

    //set TE on
    LCM_WR_CMD(0x35);//0x35 turn on TE signal,0x34 turn off TE signal
    LCM_WR_DAT(0x00); // Set TE Mode = Vsync Only

    //set LCM frame rate
    //ST7789S,
    LCM_WR_CMD(0xB3);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x09); // FR for idle mode, 75Hz
    LCM_WR_DAT(0x09); // FR for partial mode, 75Hz

    LCM_WR_CMD(0xC6);
    LCM_WR_DAT(0x09); // FR for normal mode, 75Hz

    // clear the screen with black color
    LCM_WR_CMD(0x2C);

#endif

#ifdef ST7789S_DRIVER_3
    LCM_WR_CMD(0x36);
    LCM_WR_DAT(0x00);

    LCM_WR_CMD(0xb2);
    LCM_WR_DAT(0x0c);
    LCM_WR_DAT(0x0c);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x33);
    LCM_WR_DAT(0x33);

    LCM_WR_CMD(0xb7);
    LCM_WR_DAT(0x35);

    LCM_WR_CMD(0xbb);
    LCM_WR_DAT(0x37);

    LCM_WR_CMD(0xc0);
    LCM_WR_DAT(0x2c);

    LCM_WR_CMD(0xc2);
    LCM_WR_DAT(0x01);

    LCM_WR_CMD(0xc3);
    LCM_WR_DAT(0x09);   //  07

    LCM_WR_CMD(0xc4);
    LCM_WR_DAT(0x20);

    LCM_WR_CMD(0xc6);
    LCM_WR_DAT(0x03);

    LCM_WR_CMD(0xd0);
    LCM_WR_DAT(0xa4);
    LCM_WR_DAT(0xa1);
    LCM_WR_CMD(0xb0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0xd0);
    LCM_WR_CMD(0xe0);
    LCM_WR_DAT(0xd0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x05);
    LCM_WR_DAT(0x0e);
    LCM_WR_DAT(0x15);
    LCM_WR_DAT(0x0d);
    LCM_WR_DAT(0x37);
    LCM_WR_DAT(0x43);
    LCM_WR_DAT(0x47);
    LCM_WR_DAT(0x09);
    LCM_WR_DAT(0x15);
    LCM_WR_DAT(0x12);
    LCM_WR_DAT(0x16);
    LCM_WR_DAT(0x19);

    LCM_WR_CMD(0xe1);
    LCM_WR_DAT(0xd0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x05);
    LCM_WR_DAT(0x0d);
    LCM_WR_DAT(0x0c);
    LCM_WR_DAT(0x06);
    LCM_WR_DAT(0x2d);
    LCM_WR_DAT(0x44);
    LCM_WR_DAT(0x40);
    LCM_WR_DAT(0x0e);
    LCM_WR_DAT(0x1c);
    LCM_WR_DAT(0x18);
    LCM_WR_DAT(0x16);
    LCM_WR_DAT(0x19);

    LCM_WR_CMD(0x35);
    LCM_WR_DAT(0x00);

    LCM_WR_CMD(0x3A);
    LCM_WR_DAT(0x55);
#endif

    LCM_WR_CMD(0x29);

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


#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

    // Init code
    sxr_Sleep(150 MS_WAITING);                         // Delay 50 ms

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

    LCM_WR_CMD(0x10); //  Sleep

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
    if (0 == lcdd_MutexGet())
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
#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

    // Init code
    sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms
    lcddp_Init();
    sxr_Sleep(120 MS_WAITING);

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

    hal_HstSendEvent(0xaabbdddd);
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

    // building set roi sequence:

    if(g_lcddRotate)
    {

        LCM_WR_CMD(0x2a);    //Set Column Address
        LCM_WR_DAT(pActiveWin->tlPX>>8);
        LCM_WR_DAT(pActiveWin->tlPX&0x00ff);
        LCM_WR_DAT(pActiveWin->brPX>>8);
        LCM_WR_DAT(pActiveWin->brPX&0x00ff);


        LCM_WR_CMD(0x2b);    //Set Page Address
        LCM_WR_DAT(pActiveWin->tlPY>>8);
        LCM_WR_DAT(pActiveWin->tlPY&0x00ff);
        LCM_WR_DAT(pActiveWin->brPY>>8);
        LCM_WR_DAT(pActiveWin->brPY&0x00ff);
    }
    else
    {
        LCM_WR_CMD(0x2a);    //Set Column Address
        LCM_WR_DAT(pActiveWin->tlPX>>8);
        LCM_WR_DAT(pActiveWin->tlPX&0x00ff);
        LCM_WR_DAT(pActiveWin->brPX>>8);
        LCM_WR_DAT(pActiveWin->brPX&0x00ff);


        LCM_WR_CMD(0x2b);    //Set Page Address
        LCM_WR_DAT(pActiveWin->tlPY>>8);
        LCM_WR_DAT(pActiveWin->tlPY&0x00ff);
        LCM_WR_DAT(pActiveWin->brPY>>8);
        LCM_WR_DAT(pActiveWin->brPY&0x00ff);

        //LCM_WR_CMD(0x2c); //WRITE ram Data display
    }
    LCM_WR_CMD(0x2c);
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
// return LCDD_ERR_NO;
    hal_HstSendEvent(0xaabbddde);
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

    LCM_WR_CMD(0x36);  //
    LCM_WR_DAT(0x60);
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

    LCM_WR_CMD(0x36);
    LCM_WR_DAT(0x00);//0x48

    lcdd_MutexFree();

    return TRUE;
}

PRIVATE UINT16 lcdd_get_lcd_id(void)
{

    UINT16 productId=0;
    UINT8 productIds[4];

    if(g_lcddInSleep)
    {
        lcddp_WakeUp();
    }
#ifdef USE_SPI_LCD
    hal_GoudaSetSpiFreq(LCDD_SPI_FREQ_READ);
    sxr_Sleep(200 MS_WAITING);
#endif
    hal_GoudaReadData(0x04,productIds,4);
#ifdef USE_SPI_LCD
    productId=((UINT16)(productIds[2])<<8 & 0xff00)|((UINT16)productIds[1] & 0x0ff);
#else
    productId=((UINT16)(productIds[2])<<8 & 0xff00)|((UINT16)productIds[1] & 0x0ff);
#endif
#ifdef USE_SPI_LCD
    hal_GoudaSetSpiFreq(LCDD_SPI_FREQ);
#endif
    SXS_TRACE(TSTDOUT, "get lcd id is 0x%x \n", productId);
    sxr_Sleep(100 MS_WAITING);
    return productId;
}

PRIVATE BOOL lcddp_CheckProductId()
{
    UINT16 productId=0;
    UINT8 productIds[4];
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ_READ, &lcddReadConfig.config, 0);
#else
    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
#endif

    sxr_Sleep(50 MS_WAITING);

    hal_GoudaReadData(0x04,productIds,4);
    //   productId=productIds[1];
#ifdef USE_SPI_LCD
    productId=((UINT16)(productIds[2])<<8 & 0xff00)|((UINT16)productIds[1] & 0x0ff);
#else
    productId=((UINT16)(productIds[2])<<8 & 0xff00)|((UINT16)productIds[1] & 0x0ff);
#endif
    hal_HstSendEvent(0x778901cd);
    hal_HstSendEvent(productId);

    SXS_TRACE(TSTDOUT, "st7789s(0x%x): lcd read id is 0x%x ", LCD_ST7789S_ID, productId);

    hal_GoudaClose();


    if(productId == LCD_ST7789S_ID)
        return TRUE;
    else
        return FALSE;
}

PRIVATE char* lcdd_get_id_string(void)
{
    static char st7789s_id_str[] = "st7789s\n";
    return st7789s_id_str;
}
// ============================================================================
// lcdd_ili9325_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL  lcdd_st7789s_RegInit(LCDD_REG_T *pLcdDrv)
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
        pLcdDrv->lcdd_GetLcdId=lcdd_get_lcd_id;
        pLcdDrv->lcdd_GoudaBltHdl = lcddp_GoudaBlitHandler;
        return TRUE;
    }

    return FALSE;
}



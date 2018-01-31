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
//
// =============================================================================
//  MACROS
// =============================================================================



#ifdef lcd_line_8_low

//low 8bit
#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);   while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(((Data)&0xFF))     != HAL_ERR_NO); }
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(((Cmd)&0xFF))      != HAL_ERR_NO);}


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
#define LCDD_DISP_X             128

// Number of pixels in the display height
#define LCDD_DISP_Y             160



#define  LCD_ST7735R_ID         0xf0f0

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// LCDD screen config.
PRIVATE CONST LCDD_CONFIG_T g_tgtLcddCfg = TGT_LCDD_CONFIG;

PRIVATE BOOL g_lcddRotate = FALSE;

#ifndef lcd_no_dma
// wheter lcddp_GoudaBlitHandler() has to close ovl layer 0
PRIVATE BOOL g_lcddAutoCloseLayer = FALSE;
#endif // lcd_no_dma

// Sleep status of the LCD
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
// =============================================================================


#ifndef lcd_no_dma
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
#ifdef LCD_SCALE_128_160_TO_176_220
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID1);
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID2);
        hal_GoudaVidLayerClose();
#endif
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
        g_lcddAutoCloseLayer = FALSE;
    }
}
#endif // lcd_no_dma

PRIVATE LCDD_ERR_T lcddp_FillRect16_init(UINT16 bgColor)
{
    UINT16 i,j;
    LCM_WR_CMD(0x2a);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(LCDD_DISP_X+2);

    LCM_WR_CMD(0x2b);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(LCDD_DISP_Y+3);

    LCM_WR_CMD(0x2c);       // vertical RAM address position
    for(i=0; i<LCDD_DISP_X+2; i++)
    {
        for(j=0; j<LCDD_DISP_Y+3; j++)
        {
            LCM_WR_DAT((bgColor>>8)&0xff);
            LCM_WR_DAT(bgColor&0xff);
        }
    }
}

// =============================================================================
// lcddp_Init
// -----------------------------------------------------------------------------
/// This function initializes LCD registers after powering on or waking up.
// =============================================================================
PRIVATE VOID lcddp_Init(VOID)
{
    LCM_WR_CMD(0x11); //Sleep out
    sxr_Sleep(60 MS_WAITING);; //Delay 120ms
#ifndef lcd_line_8_low
    //------------------------------------ST7735R Frame Rate-----------------------------------------//
    LCM_WR_CMD(0xB1);
    LCM_WR_DAT(0x00);//0x01
    LCM_WR_DAT(0x08);
    LCM_WR_DAT(0x05);
    LCM_WR_CMD(0xB2);
    LCM_WR_DAT(0x01);
    LCM_WR_DAT(0x2C);
    LCM_WR_DAT(0x2D);
    LCM_WR_CMD(0xB3);
    LCM_WR_DAT(0x01);
    LCM_WR_DAT(0x2C);
    LCM_WR_DAT(0x2D);
    LCM_WR_DAT(0x01);
    LCM_WR_DAT(0x2C);
    LCM_WR_DAT(0x2D);

    //------------------------------------End ST7735R Frame Rate-----------------------------------------//
    LCM_WR_CMD(0xB4); //Column inversion
    LCM_WR_DAT(0x00);
    //------------------------------------ST7735R Power Sequence-----------------------------------------//
    LCM_WR_CMD(0xC0);
    LCM_WR_DAT(0xA2);
    LCM_WR_DAT(0x02);
    LCM_WR_DAT(0x84);
    LCM_WR_CMD(0xC1);
    LCM_WR_DAT(0xC5);
    LCM_WR_CMD(0xC2);
    LCM_WR_DAT(0x0A);
    LCM_WR_DAT(0x00);
    LCM_WR_CMD(0xC3);
    LCM_WR_DAT(0x8A);
    LCM_WR_DAT(0x2A);
    LCM_WR_CMD(0xC4);
    LCM_WR_DAT(0x8A);
    LCM_WR_DAT(0xEE);
    //---------------------------------End ST7735R Power Sequence-------------------------------------//
    LCM_WR_CMD(0xC5); //VCOM
    LCM_WR_DAT(0x0E);
#ifdef LCM_Y_MIRROR_DISPLAY
    LCM_WR_CMD(0x36); //MX, MY, RGB mode
    LCM_WR_DAT(0x08);
#else
    LCM_WR_CMD(0x36); //MX, MY, RGB mode
    LCM_WR_DAT(0xC8);
#endif
    //LCM_WR_CMD(0x20); //rev off
    LCM_WR_CMD(0x21);   //rev on
    //------------------------------------ST7735R Gamma Sequence-----------------------------------------//
    LCM_WR_CMD(0xe0);
    LCM_WR_DAT(0x02);
    LCM_WR_DAT(0x1c);
    LCM_WR_DAT(0x07);
    LCM_WR_DAT(0x12);
    LCM_WR_DAT(0x37);
    LCM_WR_DAT(0x32);
    LCM_WR_DAT(0x29);
    LCM_WR_DAT(0x2d);
    LCM_WR_DAT(0x29);
    LCM_WR_DAT(0x25);
    LCM_WR_DAT(0x2b);
    LCM_WR_DAT(0x39);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x01);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x10);
    LCM_WR_CMD(0xe1);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x1d);
    LCM_WR_DAT(0x07);
    LCM_WR_DAT(0x06);
    LCM_WR_DAT(0x2e);
    LCM_WR_DAT(0x2c);
    LCM_WR_DAT(0x29);
    LCM_WR_DAT(0x2d);
    LCM_WR_DAT(0x2e);
    LCM_WR_DAT(0x2e);
    LCM_WR_DAT(0x37);
    LCM_WR_DAT(0x3f);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x02);
    LCM_WR_DAT(0x10);
#else
    LCM_WR_CMD(0xb4);
    LCM_WR_DAT(0x00);

    LCM_WR_CMD(0xb6);
    LCM_WR_DAT(0xb4);
    LCM_WR_DAT(0xf0);
    //LCM_WR_DAT(0x20);

    LCM_WR_CMD(0xc0);
    LCM_WR_DAT(0xa2);
    LCM_WR_DAT(0x02);
    LCM_WR_DAT(0x04);

    LCM_WR_CMD(0xc1);
    LCM_WR_DAT(0xc0);//C5

    LCM_WR_CMD(0xc2);
    LCM_WR_DAT(0x0a);
    LCM_WR_DAT(0x00);

    LCM_WR_CMD(0xc3);
    LCM_WR_DAT(0x8a);
    LCM_WR_DAT(0x26);//2A

    LCM_WR_CMD(0xc4);
    LCM_WR_DAT(0x8a);
    LCM_WR_DAT(0xee);

    LCM_WR_CMD(0xc5);
    LCM_WR_DAT(0x10);//15

    LCM_WR_CMD(0xc7);
    LCM_WR_DAT(0x0c);

#ifdef LCM_Y_MIRROR_DISPLAY
    LCM_WR_CMD(0x36); //MX, MY, RGB mode
    LCM_WR_DAT(0x08);
#else
    LCM_WR_CMD(0x36); //MX, MY, RGB mode
    LCM_WR_DAT(0xC8);
#endif

    LCM_WR_CMD(0xe0);
    LCM_WR_DAT(0x03);//02
    LCM_WR_DAT(0x1c);
    LCM_WR_DAT(0x12);//07
    LCM_WR_DAT(0x11);//12
    LCM_WR_DAT(0x3F);//37
    LCM_WR_DAT(0x3A);//32
    LCM_WR_DAT(0x32);//29
    LCM_WR_DAT(0x34);//2D
    LCM_WR_DAT(0x2F);//29
    LCM_WR_DAT(0x2b);//25
    LCM_WR_DAT(0x30);//2B
    LCM_WR_DAT(0x3A);//39
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x01);
    LCM_WR_DAT(0x02);//03
    LCM_WR_DAT(0x05);//10

    LCM_WR_CMD(0xe1);
    LCM_WR_DAT(0x03);
    LCM_WR_DAT(0x1B);//1D
    LCM_WR_DAT(0x12);//07
    LCM_WR_DAT(0x11);//06
    LCM_WR_DAT(0x22);//2E
    LCM_WR_DAT(0x2F);//2C
    LCM_WR_DAT(0x2A);//29
    LCM_WR_DAT(0x2F);//2D
    LCM_WR_DAT(0x2e);
    LCM_WR_DAT(0x2C);//2E
    LCM_WR_DAT(0x35);//37
    LCM_WR_DAT(0x3f);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x00);
    LCM_WR_DAT(0x01);//02
    LCM_WR_DAT(0x05);//10
    //------------------------------------End ST7735R Gamma Sequence-----------------------------------------//
#endif
    LCM_WR_CMD(0x3A); //65k mode
    LCM_WR_DAT(0x05);
    LCM_WR_CMD(0x29); //Display on
    //   sxr_Sleep(100 MS_WAITING);
    lcddp_FillRect16_init(0x0000);
    //  sxr_Sleep(100 MS_WAITING);

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
    LCM_WR_CMD(0x28);
    sxr_Sleep(50 MS_WAITING);
    LCM_WR_CMD(0x10); // Power Control 1
    sxr_Sleep(120 MS_WAITING);
    // Enter Standby mode

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
#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

    // Init code
    sxr_Sleep(100 MS_WAITING);                         // Delay 50 ms
#if 0
    LCM_WR_REG(0x0010, 0x0000); // Exit Standby mode
#else
    lcddp_Init();
#endif

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

        LCM_WR_REG(0x2c, pixelData);

        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
}

#ifndef lcd_no_dma

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

#if 0

    if(g_lcddRotate)
    {
        //Window Horizontal RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,0,0x37,pActiveWin->tlPY);
        //Window Horizontal RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,2,0x36,pActiveWin->brPY);
        //Window Vertical RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,4,0x39,LCDD_DISP_Y-1-pActiveWin->brPX);
        //Window Vertical RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,6,0x38,LCDD_DISP_Y-1-pActiveWin->tlPX);

        //Start point
        LCDD_BUILD_CMD_WR_REG(cmd,8,0x20,pActiveWin->tlPY);
        LCDD_BUILD_CMD_WR_REG(cmd,10,0x21,LCDD_DISP_Y-1-pActiveWin->tlPX);
    }
    else
    {
        //Window Horizontal RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,0,0x37,pActiveWin->tlPX);
        //Window Horizontal RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,2,0x36,pActiveWin->brPX);
        //Window Vertical RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,4,0x39,pActiveWin->tlPY);
        //Window Vertical RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,6,0x38,pActiveWin->brPY);

        //Start point
        LCDD_BUILD_CMD_WR_REG(cmd,8,0x20,pActiveWin->tlPX);
        LCDD_BUILD_CMD_WR_REG(cmd,10,0x21,pActiveWin->tlPY);
    }

    // Send command after which the data we sent
    // are recognized as pixels.
    LCDD_BUILD_CMD_WR_CMD(cmd,12,0x0022);
    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 13, cmd, lcddp_GoudaBlitHandler));

#else

    if(g_lcddRotate)
    {
        LCM_WR_CMD(0x2a);    //Set Column Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPX);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPX);


        LCM_WR_CMD(0x2b);    //Set Page Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPY);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPY);

        //LCM_WR_CMD(0x2c); //WRITE ram Data display
    }
    else
    {
        LCM_WR_CMD(0x2a);    //Set Column Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPX);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPX);


        LCM_WR_CMD(0x2b);    //Set Page Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPY);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPY);

        //LCM_WR_CMD(0x2c); //WRITE ram Data display
    }

    // Send command after which the data we sent
    // are recognized as pixels.
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
#ifdef LCD_SCALE_128_160_TO_176_220
            HAL_GOUDA_VID_LAYER_DEF_T   gouda_vid_def = {0, };

            gouda_vid_def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565;
            gouda_vid_def.addrY = (UINT32*)frameBufferWin->fb.buffer;

            gouda_vid_def.alpha = 0xFF;
            gouda_vid_def.cKeyColor = 0;
            gouda_vid_def.cKeyEn = FALSE;
            gouda_vid_def.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
            gouda_vid_def.depth = HAL_GOUDA_VID_LAYER_BEHIND_ALL;
            //gouda_vid_def.depth = HAL_GOUDA_VID_LAYER_BEHIND_ALL;

            gouda_vid_def.stride = 0;

            gouda_vid_def.width = frameBufferWin->fb.width;
            gouda_vid_def.height = frameBufferWin->fb.height;

            gouda_vid_def.pos.tlPX = 0;
            gouda_vid_def.pos.tlPY = 0;

            gouda_vid_def.pos.brPX = (frameBufferWin->fb.width )*176/128- 1;
            gouda_vid_def.pos.brPY = (frameBufferWin->fb.height)*220/160 - 1;

            hal_GoudaVidLayerOpen(&gouda_vid_def);
            g_lcddAutoCloseLayer = TRUE;

            // Set Input window
            inputWin.tlPX = gouda_vid_def.pos.tlPX;
            inputWin.brPX = gouda_vid_def.pos.brPX;
            inputWin.tlPY = gouda_vid_def.pos.tlPY;
            inputWin.brPY = gouda_vid_def.pos.brPY;

            // Set Active window
            activeWin.tlPX = gouda_vid_def.pos.tlPX;
            activeWin.brPX = gouda_vid_def.pos.brPX;
            activeWin.tlPY = gouda_vid_def.pos.tlPY;
            activeWin.brPY = gouda_vid_def.pos.brPY;
#else // !LCD_SCALE_128_160_TO_176_220
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
#endif // !LCD_SCALE_128_160_TO_176_220
        }

        // gouda is doing everything ;)
        lcddp_BlitRoi2Win(&inputWin, &activeWin);

        return LCDD_ERR_NO;
    }
}

#else // lcd_no_dma

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

#if 0
        // Check parameters
        // ROI must be within the screen boundary
        if (    (activeWin.tlPX >= LCDD_DISP_X) ||
                (activeWin.brPX >= LCDD_DISP_X) ||
                (activeWin.tlPY >= LCDD_DISP_Y) ||
                (activeWin.brPY >= LCDD_DISP_Y)
           )
        {
            lcdd_MutexFree();
            return LCDD_ERR_INVALID_PARAMETER;
        }
#endif

        hal_GoudaSetBgColor(bgColor);
//        lcddp_BlitRoi2Win(&activeWin,&activeWin);

        // Active window coordinates.
        UINT32 hsa = 1;
        UINT32 hea = 1;
        UINT32 vsa = 1;
        UINT32 vea = 1;

        // Set Active window
        hsa = regionOfInterrest->x;
        hea = regionOfInterrest->x + regionOfInterrest->width - 1;
        vsa = regionOfInterrest->y;
        vea = regionOfInterrest->y + regionOfInterrest->height - 1;


        LCM_WR_REG(0x0037,hsa);//Window Horizontal RAM Address Start
        LCM_WR_REG(0x0036,hea);//Window Horizontal RAM Address End
        LCM_WR_REG(0x0039,vsa);//Window Vertical RAM Address Start
        LCM_WR_REG(0x0038,vea);//Window Vertical RAM Address End

        LCM_WR_REG(0x0020,hsa);
        LCM_WR_REG(0x0021,vsa);

        // Set initial point
        LCM_WR_REG(0x22, bgColor);



        UINT32 x;
        for (x = 1; x < (hea-hsa+1) *
                (vea-vsa+1); ++x)
        {
            LCM_WR_DAT(bgColor);
        }
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
}

// ============================================================================
// lcdd_TransferData
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD
// ============================================================================
PRIVATE VOID lcdd_TransferData(CONST UINT16* pPixelData, UINT32 nPixelWrite, BOOL lastLine)
{
    UINT32 x;
    for (x = 0; x < nPixelWrite; x++)
    {
        LCM_WR_DAT(pPixelData[x]);
    }

    if (lastLine)
    {
        lcdd_MutexFree();
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

        if (frameBufferWin->fb.buffer != NULL)
        {

            // this will allow to keep LCDD interface for blit while using gouda
            // directly for configuring layers
            LCM_WR_REG(0x0037,activeWin.tlPX);//Window Horizontal RAM Address Start
            LCM_WR_REG(0x0036,activeWin.brPX);//Window Horizontal RAM Address End
            LCM_WR_REG(0x0039,activeWin.tlPY);//Window Vertical RAM Address Start
            LCM_WR_REG(0x0038,activeWin.brPY);//Window Vertical RAM Address End

            LCM_WR_REG(0x0020,activeWin.tlPX);
            LCM_WR_REG(0x0021,activeWin.tlPY);

            // Send command after which the data we sent
            // are recognized as pixels.
            LCM_WR_CMD(0x22);




            if (frameBufferWin->roi.width == frameBufferWin->fb.width)
            {
                INT32 transferlength=(frameBufferWin->roi.width )*frameBufferWin->roi.height;
                INT16* curBufAdd =frameBufferWin->fb.buffer+frameBufferWin->roi.y*frameBufferWin->roi.width;
                lcdd_TransferData(curBufAdd, transferlength, TRUE);
            }
            else
            {
                // The source buffer is wider than the roi
                // we have to do a 2D transfer
                UINT16 curLine=0;
                UINT16 startLine = frameBufferWin->roi.y;
                UINT16 endLine = frameBufferWin->roi.y+frameBufferWin->roi.height-1;
                // Start at the base of the buffer
                // add the number of pixels corresponding to the start line
                // add the number of pixel corresponding to the startx
                UINT16* curBuf = frameBufferWin->fb.buffer
                                 +(frameBufferWin->roi.y*frameBufferWin->fb.width)
                                 +(frameBufferWin->roi.x );

                for (curLine=startLine; curLine<=endLine; curLine++)
                {
                    // transfer one line
                    if (curLine == endLine)
                    {
                        lcdd_TransferData(curBuf, (frameBufferWin->roi.width), TRUE);
                    }
                    else
                    {
                        lcdd_TransferData(curBuf, (frameBufferWin->roi.width), FALSE);
                    }
                    // goto next line
                    curBuf+=frameBufferWin->fb.width;
                }
            }
        }

        return LCDD_ERR_NO;
    }
}

#endif // lcd_no_dma

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

#ifdef LCM_Y_MIRROR_DISPLAY
    LCM_WR_REG(0x36, 0x68);  //
#else
    LCM_WR_REG(0x36, 0xA8);  //
#endif

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


#ifdef LCM_Y_MIRROR_DISPLAY
    LCM_WR_REG(0x36, 0x08);  //
#else
    LCM_WR_REG(0x36, 0xC8);  //
#endif
    lcdd_MutexFree();

    return TRUE;
}


PRIVATE BOOL lcddp_CheckProductId()
{
    UINT16 productId=0;
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ_READ, &lcddReadConfig.config, 0);
#else
    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
#endif

    sxr_Sleep(20 MS_WAITING);

    hal_GoudaReadReg(0xdc, &productId);
    hal_GoudaClose();

    SXS_TRACE(TSTDOUT, "st7735r(0x%x): lcd read id is 0x%x ", LCD_ST7735R_ID, productId);
    hal_HstSendEvent(0x1007735c);
    hal_HstSendEvent(0x1cd001cd);
    hal_HstSendEvent(productId);



    if(productId == LCD_ST7735R_ID)
        return TRUE;
    else
        return FALSE;
}

PRIVATE char* lcdd_get_id_string(void)
{
    static char st7735r_id_str[] = "st7735r\n";
    return st7735r_id_str;
}
// ============================================================================
// lcdd_st7735r_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL lcdd_st7735r_RegInit(LCDD_REG_T *pLcdDrv)
{
    if( lcddp_CheckProductId())
    {
        pLcdDrv->lcdd_Open=lcddp_Open;
        pLcdDrv->lcdd_Close=lcddp_Close;
        pLcdDrv->lcdd_SetContrast=lcddp_SetContrast;
//
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



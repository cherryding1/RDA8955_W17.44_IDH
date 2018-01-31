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
#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);   while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
#define LCM_WR_DATA_8(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD_8(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}

#define LCDD_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================
/*
#if defined(__DRV_LCD_JIUSHENGDIANZI_GC9101_160__)  //九升电子
    #define JIUSHENGDIANZI_GC9101_0328
    #define LCDD_DISP_SIZE_GC9101_160X128
#elif defined(__DRV_LCD_DESHIPU_QIMEI_GC9101__) //德世浦奇美
    #define DESHIPU_QIMEI_GC9101_0507
#elif defined(__DRV_LCD_DESHIPU_HANCAI_GC9101__) //德世浦HSD
    #define DESHIPU_HANCAI_GC9101_0514
    #define LCDD_DISP_SIZE_GC9101_128X128
#elif defined(__DRV_LCD_HONGXIN_GC9101__)  //鸿信  CMO
    #define HONGXIN_GC9101_0615
#elif defined(__DRV_LCD_JIUSHENGDIANZI_GC9101_CMO__)//九升电子  CMO
    #define JIUSHENGDIANZI_GC9101_CMO
#else
*/
#define DEFAULT_GC9101
//#endif


#if defined(LCDD_DISP_SIZE_GC9101_160X128)
#define LCDD_DISP_X     160
#define LCDD_DISP_Y     128
#elif defined(LCDD_DISP_SIZE_GC9101_128X128)
#define LCDD_DISP_X     128
#define LCDD_DISP_Y     128
#else
#define LCDD_DISP_X     128
#define LCDD_DISP_Y     160
#endif

#if defined(USE_SPI_LCD)
#define  LCD_GC9101        0x9100
#else
#define  LCD_GC9101        0x9101
#endif


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
//UINT16 gc9101f=0x0d;
#if 1//defined(DEFAULT_GC9101)
PRIVATE VOID lcddp_Init(VOID)
{

#if 1
    LCM_WR_CMD_8(0x3a);
    LCM_WR_DATA_8(0x05);

    LCM_WR_CMD_8(0x36);
    LCM_WR_DATA_8(0xc0);

    LCM_WR_CMD_8(0xfe);
    LCM_WR_CMD_8(0xef);

    LCM_WR_CMD_8(0xff);
    LCM_WR_DATA_8(0x12);

    LCM_WR_CMD_8(0xfd);
    LCM_WR_DATA_8(0x13);
    //gc9101f=gc9101f+0x02;
    LCM_WR_CMD_8(0xa3);
    LCM_WR_DATA_8(0x09);

    LCM_WR_CMD_8(0xa4);
    LCM_WR_DATA_8(0x96);

    LCM_WR_CMD_8(0xe7);
    LCM_WR_DATA_8(0x94);
    LCM_WR_DATA_8(0x88);

    LCM_WR_CMD_8(0xed);
    LCM_WR_DATA_8(0x11);

    LCM_WR_CMD_8(0xe4);
    LCM_WR_DATA_8(0xc5);

    LCM_WR_CMD_8(0xe2);
    LCM_WR_DATA_8(0x80);

    LCM_WR_CMD_8(0xe3);
    LCM_WR_DATA_8(0x07);

    LCM_WR_CMD_8(0xe5);
    LCM_WR_DATA_8(0x10);

    //*****************GAMMA***************************//
    LCM_WR_CMD_8(0xF0);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xF1);
    LCM_WR_DATA_8(0x55);
    LCM_WR_CMD_8(0xF2);
    LCM_WR_DATA_8(0x07);
    LCM_WR_CMD_8(0xF3);
    LCM_WR_DATA_8(0x52);
    LCM_WR_CMD_8(0xF4);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xF5);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xF7);
    LCM_WR_DATA_8(0x07);
    LCM_WR_CMD_8(0xF8);
    LCM_WR_DATA_8(0x22);
    LCM_WR_CMD_8(0xF9);
    LCM_WR_DATA_8(0x77);
    LCM_WR_CMD_8(0xFA);
    LCM_WR_DATA_8(0x25);
    LCM_WR_CMD_8(0xFB);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xFC);
    LCM_WR_DATA_8(0x00);
    //********************************************//

    LCM_WR_CMD_8(0x11);
    sxr_Sleep(120 MS_WAITING);
    LCM_WR_CMD_8(0x29);
    sxr_Sleep(50 MS_WAITING);
    LCM_WR_CMD_8(0x2c);
#else
    LCM_WR_CMD_8(0x3a);
    LCM_WR_DATA_8(0x05);
    LCM_WR_CMD_8(0x36);
    LCM_WR_DATA_8(0xC8);

    LCM_WR_CMD_8(0xfe);
    LCM_WR_CMD_8(0xef);

    LCM_WR_CMD_8(0xff);
    LCM_WR_DATA_8(0x12);
    LCM_WR_CMD_8(0xfd);
    LCM_WR_DATA_8(0x1A);

    LCM_WR_CMD_8(0xa3);
    LCM_WR_DATA_8(0x0B);
    LCM_WR_CMD_8(0xa4);
    LCM_WR_DATA_8(0x96);



    LCM_WR_CMD_8(0xe7);
    LCM_WR_DATA_8(0x94);
    LCM_WR_DATA_8(0x88);

    LCM_WR_CMD_8(0xe4);
    LCM_WR_DATA_8(0xc5);

    LCM_WR_CMD_8(0xed);
    LCM_WR_DATA_8(0x11);

    LCM_WR_CMD_8(0xe2);
    LCM_WR_DATA_8(0x80);//80
    LCM_WR_CMD_8(0xe3);
    LCM_WR_DATA_8(0x07);
    LCM_WR_CMD_8(0xe5);
    LCM_WR_DATA_8(0x10);
    //*****************GAMMA***************************//
    LCM_WR_CMD_8(0xF0);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xF1);
    LCM_WR_DATA_8(0x56);
    LCM_WR_CMD_8(0xF2);
    LCM_WR_DATA_8(0x06);
    LCM_WR_CMD_8(0xF3);
    LCM_WR_DATA_8(0x54);
    LCM_WR_CMD_8(0xF4);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xF5);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xF7);
    LCM_WR_DATA_8(0x17);
    LCM_WR_CMD_8(0xF8);
    LCM_WR_DATA_8(0x12);
    LCM_WR_CMD_8(0xF9);
    LCM_WR_DATA_8(0x77);
    LCM_WR_CMD_8(0xFA);
    LCM_WR_DATA_8(0x45);
    LCM_WR_CMD_8(0xFB);
    LCM_WR_DATA_8(0x00);
    LCM_WR_CMD_8(0xFC);
    LCM_WR_DATA_8(0x00);
    //********************************************//

    LCM_WR_CMD_8(0x11);
    sxr_Sleep(200 MS_WAITING);
    LCM_WR_CMD_8(0x29);

#endif
}
#endif

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

    /*
        // Init code
        sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms
    #if defined(JIUSHENGDIANZI_GC9101_0328)
        lcddp_jsdz_Init();
    #elif defined(DESHIPU_QIMEI_GC9101_0507)
        lcddp_deshipu_Init();
    #elif defined(DESHIPU_HANCAI_GC9101_0514)
        lcddp_deshipu_hancai_Init();
    #elif defined(HONGXIN_GC9101_0615)
        lcddp_honxin_Init();
    #elif defined(JIUSHENGDIANZI_GC9101_CMO)
        lcddp_jsdz_cmo_Init();
    #else
    */
    sxr_Sleep(50 MS_WAITING);
    lcddp_Init();
//#endif

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
    LCM_WR_CMD_8(0x28);

    sxr_Sleep(200);
    LCM_WR_CMD_8(0xfe);
    LCM_WR_CMD_8(0xef);

    LCM_WR_CMD_8(0xe9);
    LCM_WR_DATA_8(0x08);     //RN1+RN0
    sxr_Sleep(20);
    LCM_WR_CMD_8(0xe9);
    LCM_WR_DATA_8(0x0c);     //RN1+RN0
    sxr_Sleep(20);
    LCM_WR_CMD_8(0xe9);
    LCM_WR_DATA_8(0x0e);     //RN1+RN0
    sxr_Sleep(20);
    LCM_WR_CMD_8(0xe9);
    LCM_WR_DATA_8(0x0f);     //RN1+RN0
    sxr_Sleep(20);
    LCM_WR_CMD_8(0x10);
    sxr_Sleep(120);

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

    /*     sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms
    LCM_WR_CMD_8(0x11); //Exit Sleep
    sxr_Sleep(120 MS_WAITING);
    #if defined(JIUSHENGDIANZI_GC9101_0328)
         lcddp_jsdz_Init();
    #elif defined(DESHIPU_QIMEI_GC9101_0507)
         lcddp_deshipu_Init();
    #elif defined(DESHIPU_HANCAI_GC9101_0514)
         lcddp_deshipu_hancai_Init();
    #elif defined(HONGXIN_GC9101_0615)
         lcddp_honxin_Init();
    #elif defined(JIUSHENGDIANZI_GC9101_CMO)
         lcddp_jsdz_cmo_Init();
    #else
    */
    LCM_WR_CMD_8(0xfe);
    LCM_WR_CMD_8(0xef);

    LCM_WR_CMD_8(0xe9);
    LCM_WR_DATA_8(0x00);     //RN1+RN0
    sxr_Sleep(20);
    lcddp_Init();
// #endif

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
        LCM_WR_DATA_8(x);
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(x);

        LCM_WR_CMD_8(0x2b);
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(y);
        LCM_WR_DATA_8(0x00);
        LCM_WR_DATA_8(y);

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
    /* #if defined(DESHIPU_HANCAI_GC9101_0514)
              LCM_WR_CMD_8(0x2A);   //Set Column Address
               LCM_WR_DATA_8(0x00);
               LCM_WR_DATA_8(pActiveWin->tlPX);
               LCM_WR_DATA_8(0x00);
               LCM_WR_DATA_8(pActiveWin->brPX);

               LCM_WR_CMD_8(0x2B);  //Set Page Address
               LCM_WR_DATA_8(0x00);
               LCM_WR_DATA_8(pActiveWin->tlPY+0x20);
               LCM_WR_DATA_8(0x00);
               LCM_WR_DATA_8(pActiveWin->brPY+0x20);
        LCM_WR_CMD_8(0x2c); //WRITE ram Data display
     #else
     */
    LCM_WR_CMD_8(0x2A);   //Set Column Address
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->tlPX);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->brPX);

    LCM_WR_CMD_8(0x2B);  //Set Page Address
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->tlPY);
    LCM_WR_DATA_8(0x00);
    LCM_WR_DATA_8(pActiveWin->brPY);
    LCM_WR_CMD_8(0x2c); //WRITE ram Data display
    // #endif
    
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

    /*   #if defined(JIUSHENGDIANZI_GC9101_0328)
          LCM_WR_CMD_8(0x36); //Set Scanning Direction
          LCM_WR_DATA_8(0x60);
       #elif defined(DESHIPU_QIMEI_GC9101_0507)||defined(__DRV_LCD_HONGXIN_GC9101__)||defined(JIUSHENGDIANZI_GC9101_CMO)
      LCM_WR_CMD_8(0x36);  //Set Scanning Direction
          LCM_WR_DATA_8(0xA0);
       #else
       */
    LCM_WR_CMD_8(0x36);  //Set Scanning Direction
    LCM_WR_DATA_8(0x60);
    //  #endif
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
    /* #if defined(JIUSHENGDIANZI_GC9101_0328)
        LCM_WR_CMD_8(0x36); //Set Scanning Direction
        LCM_WR_DATA_8(0x60);
     #elif defined(DESHIPU_QIMEI_GC9101_0507)||defined(__DRV_LCD_HONGXIN_GC9101__)||defined(JIUSHENGDIANZI_GC9101_CMO)
        LCM_WR_CMD_8(0x36);  //Set Scanning Direction
        LCM_WR_DATA_8(0xC0);
     #else
     */
    LCM_WR_CMD_8(0x36); //Set Scanning Direction
    LCM_WR_DATA_8(0xc0);
    // #endif

    sxr_Sleep(1 MS_WAITING);

    lcdd_MutexFree();

    return TRUE;
}

PRIVATE char* lcdd_get_id_string(void)
{
    static char gc9101_id_str[] = "gc9101\n";
    return gc9101_id_str;
}
PRIVATE BOOL lcddp_CheckProductId()
{
    UINT16 productId=0;
    unsigned char productId1[4];
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ_READ, &lcddReadConfig.config, 0);
#else
    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
#endif
    sxr_Sleep(20 MS_WAITING);

    hal_GoudaReadData(0x00,productId1,4);
    hal_GoudaClose();
    productId=((((productId1[2])&0xff)<<8)|((productId1[3])&0xff));
    //hal_HstSendEvent(productId);
    // SXS_TRACE(TSTDOUT, "===========gc9101(0x%x): lcd read id is 0x%x=============", LCD_GC9101, productId);
    LCDD_TRACE(LCDD_INFO_TRC, 0,"===========gc9101(0x%x): lcd read id is 0x%x=============", LCD_GC9101, productId);

    hal_HstSendEvent(0x10009101);
    hal_HstSendEvent(0x1cd001cd);
    hal_HstSendEvent(productId);

    if((productId&0xFFFF) == LCD_GC9101)
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
PUBLIC BOOL  lcdd_gc9101_RegInit(LCDD_REG_T *pLcdDrv)
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



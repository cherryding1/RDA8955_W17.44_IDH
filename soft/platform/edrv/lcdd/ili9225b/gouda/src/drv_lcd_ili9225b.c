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

#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO);}
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}

#define LCDD_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define LCDD_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define LCDD_BUILD_CMD_WR_REG(c,i,r,d) do{LCDD_BUILD_CMD_WR_CMD(c,i,r); LCDD_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)

#define LCDD_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width
#define LCDD_DISP_X             176

// Number of pixels in the display height
#define LCDD_DISP_Y             220

#define  LCD_ILI9225B_ID         0x9225

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// LCDD screen config.
PRIVATE CONST LCDD_CONFIG_T g_tgtLcddCfg = TGT_LCDD_CONFIG;

// Lock to protect the access to the LCD screen during a DMA access.
// When 1, access is allowed. When 0, it is denied.
PRIVATE UINT32 g_lcddLock = 0;

// Opened status of the driver, used to check that no
// call to blit is done before the driver is opened
PRIVATE BOOL g_lcddOpened = FALSE;

PRIVATE BOOL g_lcddRotate = FALSE;

// Sleep status of the LCD
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// lcddp_MutexFree
// -----------------------------------------------------------------------------
/// Free a previously taken mutex. The ownership of the mutex is not checked.
/// Only free a mutex you have previously got.
/// @param mutex Pointer to the integer used as a mutex.
// =============================================================================
INLINE VOID lcddp_MutexFree(UINT32* mutex)
{
    // Writing is an atomic operation
    *mutex = 1;
}


// =============================================================================
// lcddp_MutexGet
// -----------------------------------------------------------------------------
/// This function enter in critical section, read the value of the mutex and,
/// if this is a '1', returns '1' and set the mutex to 0. If this is a '0',
/// it does nothing. Then, in both cases, it exists the Critical Section.
///
/// @param mutex Pointer to the integer used as a mutex.
/// @return '1' if the mutex was taken, '0' otherwise.
// =============================================================================
PRIVATE UINT32 lcddp_MutexGet(UINT32* mutex)
{
    UINT32 status;
    UINT32 result;

    status = hal_SysEnterCriticalSection();
    if ((*mutex) == 1)
    {
        // Take the mutex
        *mutex = 0;
        result = 1;
    }
    else
    {
        // Too bad ...
        result = 0;
    }

    hal_SysExitCriticalSection(status);
    return result;
}


// =============================================================================
// lcddp_Init
// -----------------------------------------------------------------------------
/// This function initializes LCD registers after powering on or waking up.
// =============================================================================
PRIVATE VOID lcddp_Init(VOID)
{

    sxr_Sleep(10 MS_WAITING);

    // Init code
    sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms

    sxr_Sleep(50 MS_WAITING);                         // Delay 50 ms

//VCC=VCI=2.8V
//************* Start Initial Sequence **********//
    LCM_WR_REG(0x0001, 0x011C); // set SS and NL bit
    LCM_WR_REG(0x0002, 0x0100); // set 1 line inversion
    LCM_WR_REG(0x0003, 0x1030); // set GRAM write direction and BGR=1.
    LCM_WR_REG(0x0008, 0x0808); // set BP and FP
    LCM_WR_REG(0x000C, 0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for

    LCM_WR_REG(0x000F, 0x0c01); // Set frame rate
    LCM_WR_REG(0x0020, 0x0000); // Set GRAM Address
    LCM_WR_REG(0x0021, 0x0000); // Set GRAM Address
//*************Power On sequence ****************//
//delayms(50); // Delay 50ms
    sxr_Sleep(50 MS_WAITING);                         // Delay 50ms

    LCM_WR_REG(0x0010, 0x0A00); // Set SAP,DSTB,STB
    LCM_WR_REG(0x0011, 0x1038); // Set APON,PON,AON,VCI1EN,VC
///delayms(50); // Delay 50ms
    sxr_Sleep(50 MS_WAITING);                         // Delay 50ms

    LCM_WR_REG(0x0012, 0x1121); // Internal reference voltage= Vci;
    LCM_WR_REG(0x0013, 0x006e); // Set GVDD
    LCM_WR_REG(0x0014, 0x6568); // Set VCOMH/VCOML voltage
//------------------------ Set GRAM area --------------------------------//
    LCM_WR_REG (0x30, 0x0000);
    LCM_WR_REG (0x31, 0x00DB);
    LCM_WR_REG (0x32, 0x0000);
    LCM_WR_REG (0x33, 0x0000);
    LCM_WR_REG (0x34, 0x00DB);
    LCM_WR_REG (0x35, 0x0000);
    LCM_WR_REG (0x36, 0x00AF);
    LCM_WR_REG (0x37, 0x0000);
    LCM_WR_REG (0x38, 0x00DB);
    LCM_WR_REG (0x39, 0x0000);
// ----------- Adjust the Gamma Curve ----------//
    LCM_WR_REG(0x0050, 0x0000);
    LCM_WR_REG(0x0051, 0x0705);
    LCM_WR_REG(0x0052, 0x0E0a);
    LCM_WR_REG(0x0053, 0x0300);
    LCM_WR_REG(0x0054, 0x0a0E);
    LCM_WR_REG(0x0055, 0x0507);
    LCM_WR_REG(0x0056, 0x0000);
    LCM_WR_REG(0x0057, 0x0003);
    LCM_WR_REG(0x0058, 0x090a);
    LCM_WR_REG(0x0059, 0x0a09);
///delayms(50); // Delay 50ms
    sxr_Sleep(50 MS_WAITING);                         // Delay 50ms

    LCM_WR_REG(0x0007, 0x1017);


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


    lcddp_Init();

    g_lcddInSleep = FALSE;
    // Allow access
    g_lcddLock = 1;
    g_lcddOpened = TRUE;

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

    // Disable access
    g_lcddLock = 0;
    g_lcddOpened = FALSE;

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    while (0 == lcddp_MutexGet(&g_lcddLock))
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Sleep while another LCD operation in progress. Sleep %d ticks",
                   LCDD_TIME_MUTEX_RETRY);
    }

    if (g_lcddInSleep)
    {
        lcddp_MutexFree(&g_lcddLock);
        return LCDD_ERR_NO;
    }

    LCM_WR_REG(0x0007, 0x0000); // Set D1=0, D0=1
    sxr_Sleep(50 MS_WAITING);
    LCM_WR_REG(0x0011, 0x0007); // Set APON,PON,AON,VCI1EN,VC
    sxr_Sleep(50 MS_WAITING);
    LCM_WR_REG(0x0010, 0x0A01); // Enter Standby mode

    LCDD_TRACE(TSTDOUT, 0, "lcddp_Sleep: calling hal_GoudaClose");
    hal_GoudaClose();

    g_lcddInSleep = TRUE;

    lcddp_MutexFree(&g_lcddLock);

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (0 == lcddp_MutexGet(&g_lcddLock))
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Wakeup while another LCD operation in progress. Sleep %d ticks",
                   LCDD_TIME_MUTEX_RETRY);
    }

    if (!g_lcddInSleep)
    {
        lcddp_MutexFree(&g_lcddLock);
        return LCDD_ERR_NO;
    }

    LCDD_TRACE(TSTDOUT, 0, "lcddp_WakeUp: calling hal_GoudaOpen");
#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif
#if 0
    LCM_WR_REG(0x0010, 0x0000); // Exit Standby mode
#else
    lcddp_Init();
#endif

    g_lcddInSleep = FALSE;

    lcddp_MutexFree(&g_lcddLock);
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
    lcdd_Blit16(&frameBufferWin,frameBufferWin.roi.x,frameBufferWin.roi.y);

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (0 == lcddp_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcddp_MutexFree(&g_lcddLock);
            return LCDD_ERR_NO;
        }
        LCM_WR_REG(0x0036,LCDD_DISP_X-1-x);
        LCM_WR_REG(0x0037,LCDD_DISP_X-1-x);
        LCM_WR_REG(0x0038,LCDD_DISP_Y-1-y);
        LCM_WR_REG(0x0039,LCDD_DISP_Y-1-y);
        LCM_WR_REG(0x22, pixelData);

        lcddp_MutexFree(&g_lcddLock);
        return LCDD_ERR_NO;
    }
}

#ifndef lcd_no_dma


// wheter lcddp_GoudaBlitHandler() has to close ovl layer 0
PRIVATE BOOL g_lcddAutoCloseLayer = FALSE;

// =============================================================================
// lcddp_GoudaBlitHandler
// -----------------------------------------------------------------------------
/// This function frees the lock to access the screen. It is set as the user
/// handler called by the DMA driver at the end of the writings on the screen.
// =============================================================================
PRIVATE VOID lcddp_GoudaBlitHandler(VOID)
{
    lcddp_MutexFree(&g_lcddLock);
    if(g_lcddAutoCloseLayer)
    {
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
#ifdef LCD_SCALE_128_160_TO_176_220
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID1);
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID2);
        hal_GoudaVidLayerClose();
#endif
        g_lcddAutoCloseLayer = FALSE;
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


        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers
        LCM_WR_REG(0x0037,pActiveWin->tlPY);//Window Horizontal RAM Address Start
        LCM_WR_REG(0x0036,pActiveWin->brPY);//Window Horizontal RAM Address End
        LCM_WR_REG(0x0039,LCDD_DISP_Y-1-pActiveWin->brPX);//Window Vertical RAM Address Start
        LCM_WR_REG(0x0038,LCDD_DISP_Y-1-pActiveWin->tlPX);//Window Vertical RAM Address End

        LCM_WR_REG(0x0020,pActiveWin->tlPY);
        LCM_WR_REG(0x0021,LCDD_DISP_Y-1-pActiveWin->tlPX);
    }
    else
    {

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers
        LCM_WR_REG(0x0037,pActiveWin->tlPX);//Window Horizontal RAM Address Start
        LCM_WR_REG(0x0036,pActiveWin->brPX);//Window Horizontal RAM Address End
        LCM_WR_REG(0x0039,pActiveWin->tlPY);//Window Vertical RAM Address Start
        LCM_WR_REG(0x0038,pActiveWin->brPY);//Window Vertical RAM Address End

        LCM_WR_REG(0x0020,pActiveWin->tlPX);
        LCM_WR_REG(0x0021,pActiveWin->tlPY);
    }

    // Send command after which the data we sent
    // are recognized as pixels.
    LCM_WR_CMD(0x22);
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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    // Active window coordinates.
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcddp_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcddp_MutexFree(&g_lcddLock);
            return LCDD_ERR_NO;
        }

        // Set Active window
        activeWin.tlPX = regionOfInterrest->x;
        activeWin.brPX = regionOfInterrest->x + regionOfInterrest->width - 1;
        activeWin.tlPY = regionOfInterrest->y;
        activeWin.brPY = regionOfInterrest->y + regionOfInterrest->height - 1;


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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
    LCDD_ASSERT((frameBufferWin->fb.width&1) == 0, "LCDD: FBW must have an even number "
                "of pixels per line. Odd support is possible at the price of a huge "
                "performance lost");
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;
    HAL_GOUDA_WINDOW_T activeWin;
    if (0 == lcddp_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcddp_MutexFree(&g_lcddLock);
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
            lcddp_MutexFree(&g_lcddLock);
            return LCDD_ERR_INVALID_PARAMETER;
        }

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers
        if (frameBufferWin->fb.buffer != NULL)
        {
#ifndef LCD_SCALE_128_160_TO_176_220
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
            hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID0, &def);
            // tell the end handler to close the layer when we are done ;)
            g_lcddAutoCloseLayer = TRUE;
#else
//20100722
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

            gouda_vid_def.pos.brPX = (frameBufferWin->fb.width )*LCDD_DISP_X_SCREEN/LCDD_DISP_X -1;
            gouda_vid_def.pos.brPY = (frameBufferWin->fb.height)*LCDD_DISP_Y_SCREEN/LCDD_DISP_Y - 1;

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

#endif
        }

        // gouda is doing everything ;)
        lcddp_BlitRoi2Win(&inputWin, &activeWin);
        return LCDD_ERR_NO;
    }
}

#else

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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    // Active window coordinates.
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcddp_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcddp_MutexFree(&g_lcddLock);
            return LCDD_ERR_NO;
        }

        // Set Active window
        activeWin.tlPX = regionOfInterrest->x;
        activeWin.brPX = regionOfInterrest->x + regionOfInterrest->width - 1;
        activeWin.tlPY = regionOfInterrest->y;
        activeWin.brPY = regionOfInterrest->y + regionOfInterrest->height - 1;

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
        lcddp_MutexFree(&g_lcddLock);
        return LCDD_ERR_NO;


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
        lcddp_MutexFree(&g_lcddLock);
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
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
    LCDD_ASSERT((frameBufferWin->fb.width&1) == 0, "LCDD: FBW must have an even number "
                "of pixels per line. Odd support is possible at the price of a huge "
                "performance lost");
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;
    HAL_GOUDA_WINDOW_T activeWin;

    if (0 == lcddp_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcddp_MutexFree(&g_lcddLock);
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
            lcddp_MutexFree(&g_lcddLock);
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

#endif

// ============================================================================
// lcddp_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented for the ebc version of the driver
// ============================================================================
PRIVATE BOOL lcddp_Busy(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
    return FALSE;
}


// ============================================================================
// lcddp_SetDirRotation
// ----------------------------------------------------------------------------
///
// ============================================================================
PRIVATE BOOL lcddp_SetDirRotation(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    while (0 == lcddp_MutexGet(&g_lcddLock))
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = TRUE;

    if (g_lcddInSleep)
    {
        lcddp_MutexFree(&g_lcddLock);
        return TRUE;
    }

    LCM_WR_REG(0x03, 0x1028);  //

    lcddp_MutexFree(&g_lcddLock);

    return TRUE;
}

// ============================================================================
// lcddp_SetDirDefault
// ----------------------------------------------------------------------------
///
// ============================================================================
PRIVATE BOOL lcddp_SetDirDefault(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    while (0 == lcddp_MutexGet(&g_lcddLock))
    {
        sxr_Sleep(LCDD_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = FALSE;

    if (g_lcddInSleep)
    {
        lcddp_MutexFree(&g_lcddLock);
        return TRUE;
    }

    LCM_WR_REG(0x03, 0x1030);  //

    lcddp_MutexFree(&g_lcddLock);

    return TRUE;
}

PRIVATE char* lcdd_get_id_string(void)
{
    static char ili9225b_id_str[] = "ili9225b\n";
    return ili9225b_id_str;
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

    hal_GoudaReadReg(0x00, &productId);
    SXS_TRACE(TSTDOUT, "ili9225b: lcd read id 0x00 is 0x%08x ",  productId);

//    hal_GoudaReadReg(0x65, &productId);
    hal_GoudaClose();
    hal_HstSendEvent(0x1cd00006);
    hal_HstSendEvent(productId);
//    SXS_TRACE(TSTDOUT, "ili9225b: lcd read id 0x65 is 0x%08x ",  productId);
    if(productId  == LCD_ILI9225B_ID)
        return TRUE;
    else
        return FALSE;
}


PUBLIC BOOL lcdd_ili9225b_RegInit(LCDD_REG_T *pLcdDrv)
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



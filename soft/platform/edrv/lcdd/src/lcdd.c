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
#include "lcdd_m.h"
#include "lcdd.h"
#include "lcddp_debug.h"
#include "tgt_m.h"

#include "hal_sys.h"
#include "pmd_m.h"
#include "sxr_sbx.h"
#include "sxr_tls.h"

#include "hal_gouda.h"
#ifdef BACKLIGHT_TYPE_AW9656
#include "hal_gpio.h"
HAL_APO_ID_T g_bl_ctrl = {{.type = HAL_GPIO_TYPE_IO,
                           .id = 9}};
#define AW9676_ENABLE_PIN_HIGH hal_GpioSet(g_bl_ctrl)
#define AW9676_ENABLE_PIN_LOW hal_GpioClr(g_bl_ctrl)
#define AW9676_PIN_DIRECTION_OUT hal_GpioSetOut(g_bl_ctrl.gpioId)
#define AW9676_TOTAL_SCALE 16
uint8 is_bl_off_to_on = 1;
LOCAL uint8 N_from = 0;
LOCAL void LCD_SetBackLightBrightness_AW9656(uint8 brightness)
{
    uint8 i, j, N_to, pulse_count;

    if ((brightness > 0) && (brightness <= 7))
    {
        N_to = AW9676_TOTAL_SCALE + 1 - brightness;
        if (N_to > N_from)
        {
            pulse_count = N_to - N_from;
        }
        else if (N_to < N_from)
        {
            pulse_count = N_to + 16 - N_from;
        }
        else
        {
            goto nothing_to_do;
        }

#if defined(__PRJ_S385_PDA__) || defined(__PRJ_S590_3SIM__) || defined(__PRJ_S580_PDA__)

        AW9676_PIN_DIRECTION_OUT;

#endif

        if (is_bl_off_to_on)
        {
            AW9676_ENABLE_PIN_HIGH;
            for (i = 0; i < 70; i++)
                ;

            is_bl_off_to_on = 0;
            pulse_count--;
        }

        for (j = 0; j < pulse_count; j++)
        {
            AW9676_ENABLE_PIN_LOW;
            for (i = 0; i < 10; i++)
                ;

            AW9676_ENABLE_PIN_HIGH;
            for (i = 0; i < 7; i++)
                ;
        }
        sxr_Sleep(3); //need to hold for a while
        N_from = N_to;
    }
    else
    {
        AW9676_ENABLE_PIN_LOW;
        sxr_Sleep(3); //need to hold for a while too( >2.5ms shutdown)
        is_bl_off_to_on = 1;
        N_from = 0;
    }
nothing_to_do:; //nop
}
#endif

#define OVERLAY_ALL_MAP 1

#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
#ifdef RECV_USE_SPK_LINE
extern BOOL g_halAudRecvUseSpkLineRight;
#endif

extern VOID aud_LcdPowerOnCallback(VOID);
extern VOID aud_LcdPowerOffCallback(VOID);
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE LCDD_REG_T g_lcddInitTable;
PRIVATE LCDD_REG_T *g_lcddDrv;
PRIVATE BOOL g_lcddLoadSuccess = FALSE;

BOOL(*g_LcddRegInit[])
(LCDD_REG_T *) = {LCDD_REGINIT_LIST};
#define LCDD_REGINIT_NUM sizeof(g_LcddRegInit) / sizeof(UINT32)

// Lock to protect the access to the LCD screen during a DMA access.
// When 1, access is allowed. When 0, it is denied.
PRIVATE VOLATILE UINT32 g_lcddLock = 0;

// Opened status of the driver, used to check that no
// call to blit is done before the driver is opened
PRIVATE BOOL g_lcddOpened = FALSE;

PRIVATE UINT8 g_lcdSemaphore = 0xff;

PRIVATE BOOL g_lcdSleep = FALSE;
PRIVATE UINT32 g_lcdBrightness = 0;

PRIVATE LCDD_ERR_T (*g_lcddDrv_lcdd_Blit16)(CONST LCDD_FBW_T *frameBufferWin, UINT16 startX, UINT16 startY);

// whether lcddp_GoudaBlitHandler() has to close merged overlay layers
PRIVATE UINT8 g_lcddAutoCloseMergedLayer = 0;

PRIVATE BOOL g_lcddVidLayerMergeEnabled = FALSE;

VOID lcdd_GoudaBlitHandler(VOID);
// =============================================================================
//  FUNCTIONS
// =============================================================================

PUBLIC VOID lcd_SemFree(VOID)
{
    sxr_ReleaseSemaphore(g_lcdSemaphore);
}
PUBLIC VOID lcd_SemTake(VOID)
{
    UINT32 state = hal_SysEnterCriticalSection();
    if (g_lcdSemaphore == 0xff)
        g_lcdSemaphore = sxr_NewSemaphore(1);

    hal_SysExitCriticalSection(state);

    sxr_TakeSemaphore(g_lcdSemaphore);
}

PUBLIC VOID lcdd_EnableVidLayerMerge(BOOL on)
{
    g_lcddVidLayerMergeEnabled = on;
}

// =============================================================================
// lcdd_MutexFree
// -----------------------------------------------------------------------------
/// Free a previously taken mutex. The ownership of the mutex is not checked.
/// Only free a mutex you have previously got.
/// @param
// =============================================================================
PROTECTED VOID lcdd_MutexFree(VOID)
{
    // Writing is an atomic operation
    g_lcddLock = 1;
}

// =============================================================================
// lcdd_MutexGet
// -----------------------------------------------------------------------------
/// This function enter in critical section, read the value of the mutex and,
/// if this is a '1', returns '1' and set the mutex to 0. If this is a '0',
/// it does nothing. Then, in both cases, it exists the Critical Section.
///
/// @param
/// @return '1' if the mutex was taken, '0' otherwise.
// =============================================================================
PROTECTED UINT32 lcdd_MutexGet(VOID)
{
    UINT32 status;
    UINT32 result;

    status = hal_SysEnterCriticalSection();
    if (g_lcddLock == 1)
    {
        // Take the mutex
        g_lcddLock = 0;
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

PUBLIC VOID lcdd_SaveLcdId(UINT8 lcdId)
{
    TGT_MODULE_ID_T moduleId;
    tgt_GetModuleId(&moduleId);
    moduleId.lcd_id = lcdId;
    tgt_SetModuleId(&moduleId);
}

// ============================================================================
// Add Functions for FMark
// ----------------------------------------------------------------------------
///
// ============================================================================

#ifdef __USE_LCD_FMARK__
#include "hal_gpio.h"
#include "tgt_board_cfg.h"
typedef struct
{
    HAL_GOUDA_WINDOW_T pRoi;
    HAL_GPIO_GPIO_ID_T Fmark_gpio;
    BOOL fmark_enable;
    BOOL buffer_ready;
    HAL_GOUDA_LCD_CMD_T cmd[16];
} LCDD_FMARK_T;

PRIVATE LCDD_FMARK_T g_lcd_fmark =
    {
        .fmark_enable = FALSE,
        .Fmark_gpio = LCD_FMARK_PIN_USED_GPIO,
        .buffer_ready = FALSE,
};

VOID lcdd_FmarkInit(void)
{
    if (g_lcd_fmark.Fmark_gpio == HAL_GPIO_NONE)
        LCDD_ASSERT(0, "LCDD: Fmark Pin not set");
}

BOOL lcdd_FmarkGetStatus(VOID)
{
    return g_lcd_fmark.fmark_enable;
}

VOID lcdd_FmarkSetBufReady(CONST HAL_GOUDA_WINDOW_T *pRoi)
{
    UINT32 cri_status = hal_SysEnterCriticalSection();
    hal_GoudaSetFmarkBusy(true);
    g_lcd_fmark.pRoi = *pRoi;
    g_lcd_fmark.buffer_ready = TRUE;

    hal_SysExitCriticalSection(cri_status);
}

VOID lcdd_FmarkHandler(VOID)
{
    if ((g_lcd_fmark.buffer_ready == TRUE) && (g_lcd_fmark.fmark_enable == TRUE))
    {
        hal_GoudaSetFmarkBusy(false);
        hal_GoudaBlitRoi(&g_lcd_fmark.pRoi, 0, g_lcd_fmark.cmd, lcdd_GoudaBlitHandler);
        g_lcd_fmark.buffer_ready = FALSE;
    }
}

VOID lcdd_FmarkGpioEnable(BOOL on)
{
    HAL_GPIO_CFG_T FmarkCfg;

    if (on == TRUE)
    {
        g_lcd_fmark.fmark_enable = TRUE;
        FmarkCfg.direction = HAL_GPIO_DIRECTION_INPUT;
        FmarkCfg.irqHandler = lcdd_FmarkHandler;
        FmarkCfg.irqMask.rising = TRUE;
        FmarkCfg.irqMask.falling = FALSE;
        FmarkCfg.irqMask.debounce = FALSE;
        FmarkCfg.irqMask.level = FALSE;
        hal_GpioOpen(g_lcd_fmark.Fmark_gpio, &FmarkCfg);
    }
    else
    {
        g_lcd_fmark.fmark_enable = FALSE;
        FmarkCfg.irqMask.rising = FALSE;
        FmarkCfg.irqMask.falling = FALSE;
        FmarkCfg.irqMask.debounce = FALSE;
        FmarkCfg.irqMask.level = FALSE;
        hal_GpioIrqSetMask(g_lcd_fmark.Fmark_gpio, &FmarkCfg.irqMask);
        hal_GpioResetIrq(g_lcd_fmark.Fmark_gpio);
        hal_GoudaSetFmarkBusy(false);
    }
}

#endif

// ============================================================================
// lcdd_InitTable
// ----------------------------------------------------------------------------
/// This function is called in the lcdd_LoadDrv for init function pointer
// ============================================================================
PRIVATE VOID lcdd_InitTable(LCDD_REG_T *pLcd)
{
    pLcd->lcdd_Open = NULL;
    pLcd->lcdd_Close = NULL;
    pLcd->lcdd_SetContrast = NULL;
    // pLcd->lcdd_SetBrightness=NULL;
    pLcd->lcdd_SetStandbyMode = NULL;
    pLcd->lcdd_PartialOn = NULL;
    pLcd->lcdd_PartialOff = NULL;
    pLcd->lcdd_Blit16 = NULL;
    pLcd->lcdd_Busy = NULL;
    pLcd->lcdd_FillRect16 = NULL;
    pLcd->lcdd_GetScreenInfo = NULL;
    pLcd->lcdd_WakeUp = NULL;
    pLcd->lcdd_SetPixel16 = NULL;
    pLcd->lcdd_Sleep = NULL;
    pLcd->lcdd_SetDirRotation = NULL;
    pLcd->lcdd_SetDirDefault = NULL;
    pLcd->lcdd_GetStringId = NULL;
    pLcd->lcdd_GetLcdId = NULL;
    pLcd->lcdd_ChangeLcdSpiFreq = NULL;
    pLcd->lcdd_GoudaBltHdl = NULL;
}

// ============================================================================
// lcdd_LoadDrv
// ----------------------------------------------------------------------------
/// Load the the right LCDD driver by checking LCD ID.
/// This function is called in the lcdd_open
/// @return #TRUE or #FALSE.
// ============================================================================
PRIVATE BOOL lcdd_LoadDrv(VOID)
{
    UINT8 i;
    TGT_MODULE_ID_T moduleId;

    tgt_GetModuleId(&moduleId);

#ifdef RECV_USE_SPK_LINE
#ifdef RECV_USE_SPK_LINE_R
    g_halAudRecvUseSpkLineRight = TRUE;
#endif // RECV_USE_SPK_LINE_R
#endif // RECV_USE_SPK_LINE

    g_lcddDrv = &g_lcddInitTable;
    lcdd_InitTable(g_lcddDrv);

    if (moduleId.lcd_id < LCDD_REGINIT_NUM)
    {
        // Valid ID
        if (g_LcddRegInit[moduleId.lcd_id](g_lcddDrv))
        {
            g_lcddLoadSuccess = TRUE;
            LCDD_TRACE(LCDD_INFO_TRC | TSTDOUT, 0, "Lcdd load sucess, ID is %d", moduleId.lcd_id);
            g_lcddDrv_lcdd_Blit16 = g_lcddDrv->lcdd_Blit16;
            return TRUE;
        }
    }

    for (i = 0; i < LCDD_REGINIT_NUM; i++)
    {
        if (g_LcddRegInit[i](g_lcddDrv))
        {
            g_lcddLoadSuccess = TRUE;
            LCDD_TRACE(LCDD_INFO_TRC | TSTDOUT, 0, "Lcdd load sucess, ID is %d", i);
            g_lcddDrv_lcdd_Blit16 = g_lcddDrv->lcdd_Blit16;
            moduleId.lcd_id = i;
            tgt_SetModuleId(&moduleId);
            return TRUE;
        }
    }
    return FALSE;
}

// ============================================================================
// lcdd_LoadSuccess
// ----------------------------------------------------------------------------
// This function return whether lcd driver load success or not.
// ============================================================================
PUBLIC BOOL lcdd_LoadSuccess(VOID)
{
    if (g_lcddLoadSuccess == FALSE)
    {
        lcdd_LoadDrv();
    }
    return g_lcddLoadSuccess;
}

// ============================================================================
// lcdd_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Open(VOID)
{
    LCDD_ERR_T ret = LCDD_ERR_NO;

    // Allow access to other LCD functions

    if (lcdd_LoadDrv())
    {
        if (g_lcddDrv->lcdd_Open != NULL)
        {
            lcdd_SetBrightness(0);
            ret = g_lcddDrv->lcdd_Open();
            if (ret == LCDD_ERR_NO)
            {
                // Allow access
                g_lcddLock = 1;
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
                aud_LcdPowerOnCallback();
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD
            }
        }
    }
    else
    {
        LCDD_TRACE(LCDD_INFO_TRC, 0, "Lcdd load error");
    }
#ifdef __USE_LCD_FMARK__
    lcdd_FmarkInit();
    lcdd_FmarkGpioEnable(TRUE);
#endif
    //if not find lcdd ,must return LCDD_ERR_NO for produce
    g_lcddOpened = TRUE;
    return ret;
}

// ============================================================================
// lcdd_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to
/// #lcdd_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Close(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    LCDD_ERR_T ret = LCDD_ERR_NO;

    // Forbid any access to other LCD functions
    g_lcddOpened = FALSE;

    if (TRUE == g_lcddLoadSuccess)
    {
        g_lcddLoadSuccess = FALSE;
        if (g_lcddDrv->lcdd_Close != NULL)
        {
            lcdd_SetBrightness(0);
            ret = g_lcddDrv->lcdd_Close();
            if (ret == LCDD_ERR_NO)
            {
                // Disable access
                g_lcddLock = 0;
            }
        }
    }

    return ret;
}

// ============================================================================
// lcdd_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetContrast(UINT32 contrast)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_SetContrast != NULL)
            return g_lcddDrv->lcdd_SetContrast(contrast);
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_SetBrightness
// ----------------------------------------------------------------------------
/// Set the brightness of the 'main'LCD screen.
/// @param brightness Value to set the brightness to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetBrightness(UINT32 brightness)
{
    if (brightness > 7)
    {
        return LCDD_ERR_INVALID_PARAMETER;
    }

    if (g_lcdBrightness != brightness)
    {
        if (g_lcddDrv_lcdd_Blit16 && g_lcddOpened)
        {
            if (brightness == 0)
            {
                pmd_SetLevel(PMD_LEVEL_LCD, brightness);
                if (!g_lcdSleep)
                {
                    lcdd_Sleep();
                }
                lcd_SemTake();
                g_lcddDrv->lcdd_Blit16 = NULL;
                lcd_SemFree();
            }
            else
            {
                g_lcddDrv->lcdd_Blit16 = g_lcddDrv_lcdd_Blit16;
                if (g_lcdSleep)
                {
                    lcdd_WakeUp();
                }

#ifdef CHIP_DIE_8809E
                int i;
                if (brightness > g_lcdBrightness)
                {
                    for (i = g_lcdBrightness + 1; i <= brightness; i++)
                    {
                        pmd_SetLevel(PMD_LEVEL_LCD, i);
                        COS_Sleep(20);
                    }
                }
                else
                {
                    for (i = g_lcdBrightness - 1; i >= brightness; i--)
                    {
                        pmd_SetLevel(PMD_LEVEL_LCD, i);
                        COS_Sleep(20);
                    }
                }
#else
                pmd_SetLevel(PMD_LEVEL_LCD, brightness);
#endif
            }
        }
        else
        {
            pmd_SetLevel(PMD_LEVEL_LCD, brightness);
        }
    }

    g_lcdBrightness = brightness;

    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetStandbyMode(BOOL standbyMode)
{
    LCDD_ERR_T ret = LCDD_ERR_NO;
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_SetStandbyMode != NULL)
            ret = g_lcddDrv->lcdd_SetStandbyMode(standbyMode);
#ifdef __USE_LCD_FMARK__
        if (!standbyMode)
            lcdd_FmarkGpioEnable(TRUE);
#endif
    }
    return ret;
}

// ============================================================================
// lcdd_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_PartialOn(UINT16 vsa, UINT16 vea)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_PartialOn != NULL)
            return g_lcddDrv->lcdd_PartialOn(vsa, vea);
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_PartialOff(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_PartialOff != NULL)
            return g_lcddDrv->lcdd_PartialOff();
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_WakeUp(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if ((TRUE == g_lcddLoadSuccess) && g_lcdSleep)
    {
        if (g_lcddDrv->lcdd_WakeUp != NULL)
        {
            // Power on LCD
            g_lcddDrv->lcdd_Blit16 = g_lcddDrv_lcdd_Blit16;
            LCDD_ERR_T ret = g_lcddDrv->lcdd_WakeUp();
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
            // Deep sleep is NOT allowed within the callback,
            // so it must be called after LCD is powered on
            aud_LcdPowerOnCallback();
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD
#ifdef __USE_LCD_FMARK__
            lcdd_FmarkGpioEnable(TRUE);
#endif
            g_lcdSleep = FALSE;
            return ret;
            ;
        }
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_Sleep(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if ((TRUE == g_lcddLoadSuccess) && (!g_lcdSleep))
    {
        if (g_lcddDrv->lcdd_Sleep != NULL)
        {
#if defined(RECV_USE_SPK_LINE) || defined(AUD_SPK_ON_WITH_LCD) || defined(AUD_EXT_PA_ON_WITH_LCD)
            // Deep sleep is NOT allowed within the callback,
            // so it must be called before LCD is powered off
            aud_LcdPowerOffCallback();
#endif // RECV_USE_SPK_LINE || AUD_SPK_ON_WITH_LCD || AUD_EXT_PA_ON_WITH_LCD
            // Power off LCD
            LCDD_ERR_T ret = g_lcddDrv->lcdd_Sleep();
            g_lcdSleep = TRUE;
            return ret;
        }
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_GetScreenInfo(LCDD_SCREEN_INFO_T *screenInfo)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_GetScreenInfo != NULL)
            return g_lcddDrv->lcdd_GetScreenInfo(screenInfo);
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_SetPixel16
// ----------------------------------------------------------------------------
/// Draw a 16-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 16-bit pixel data to draw.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetPixel16(UINT16 x, UINT16 y, UINT16 pixelData)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_SetPixel16 != NULL)
            return g_lcddDrv->lcdd_SetPixel16(x, y, pixelData);
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of the active window  with some color.
/// @param bgColor Color with which to fill the rectangle. It is a 16-bit
/// data, as the one of #lcdd_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_FillRect16(CONST LCDD_ROI_T *regionOfInterrest, UINT16 bgColor)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
#if (CHIP_HAS_GOUDA == 1) && defined(__PRJ_WITH_SPILCD__)
        hal_GoudaSetBgColor(bgColor);
#endif
        if (g_lcddDrv->lcdd_FillRect16 != NULL)
            return g_lcddDrv->lcdd_FillRect16(regionOfInterrest, bgColor);
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Blit16
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
PUBLIC LCDD_ERR_T lcdd_Blit16(CONST LCDD_FBW_T *frameBufferWin, UINT16 startX, UINT16 startY)
{
    LCDD_ERR_T ret = LCDD_ERR_NO;
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);
#ifdef DUAL_LCD
    hal_GoudaSetCsPin(TRUE);
#endif
    if (TRUE == g_lcddLoadSuccess)
    {
#if (CHIP_HAS_GOUDA == 1) && defined(__PRJ_WITH_SPILCD__)

        lcdd_LayerMerge(frameBufferWin, startX, startY, FALSE);
        while (g_lcddAutoCloseMergedLayer)
        {
            sxr_Sleep(64);
        }
#endif
        lcd_SemTake();
        if (g_lcddDrv->lcdd_Blit16 != NULL)
        {
            ret = g_lcddDrv->lcdd_Blit16(frameBufferWin, startX, startY);
            lcd_SemFree();
            return ret;
        }
        lcd_SemFree();
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented now for the driver
// ============================================================================
PUBLIC BOOL lcdd_Busy(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_Busy != NULL)
            return g_lcddDrv->lcdd_Busy();
    }
    return FALSE;
}

PUBLIC char *lcdd_GetStringId(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_GetStringId != NULL)
            return g_lcddDrv->lcdd_GetStringId();
    }
    return NULL;
}

PUBLIC UINT16 lcdd_GetLcdId(VOID)
{
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_GetLcdId != NULL)
            return g_lcddDrv->lcdd_GetLcdId();
    }
    return 0;
}

// ============================================================================
// lcdd_SetDirRotation
// ----------------------------------------------------------------------------
///  This function set lcd show rotation
// ============================================================================
PUBLIC BOOL lcdd_SetDirRotation(VOID)
{
    BOOL ret = FALSE;
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_SetDirRotation != NULL)
            ret = g_lcddDrv->lcdd_SetDirRotation();
#ifdef __USE_LCD_FMARK__
        lcdd_FmarkGpioEnable(TRUE);
#endif
    }
    return ret;
}

// ============================================================================
// lcdd_SetDirDefault
// ----------------------------------------------------------------------------
//  This function set lcd show default
// ============================================================================
PUBLIC BOOL lcdd_SetDirDefault(VOID)
{
    BOOL ret = FALSE;
    LCDD_ASSERT(g_lcddOpened, "LCDD: Call to %s while LCDD is not opened", __FUNCTION__);

    if (TRUE == g_lcddLoadSuccess)
    {
        if (g_lcddDrv->lcdd_SetDirDefault != NULL)
            ret = g_lcddDrv->lcdd_SetDirDefault();
#ifdef __USE_LCD_FMARK__
        lcdd_FmarkGpioEnable(TRUE);
#endif
    }
    return ret;
}

// =============================================================================
// lcdd_GoudaMergeHandler
// -----------------------------------------------------------------------------
/// This function frees the lock to access the lcd driver. It is set as the user
/// handler called by the DMA driver at the end of the writings on the merged buffer.
// =============================================================================
PRIVATE VOID lcdd_GoudaMergeHandler(VOID)
{
    lcdd_MutexFree();
    if (g_lcddAutoCloseMergedLayer == 1)
    {
        hal_GoudaVidLayerClose();
        g_lcddAutoCloseMergedLayer = 0;
    }
    else if (g_lcddAutoCloseMergedLayer == 2)
    {
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
        g_lcddAutoCloseMergedLayer = 0;
    }
    else if (g_lcddAutoCloseMergedLayer == 3)
    {
        g_lcddAutoCloseMergedLayer = 0;
    }
}

// ============================================================================
// lcdd_BlitRoi2Merge
// ----------------------------------------------------------------------------
// Private function to merge overlay layers into a given buffer
// ============================================================================
PRIVATE VOID lcdd_BlitRoi2Merge(CONST HAL_GOUDA_WINDOW_T *pRoi, UINT16 *layer_Merge_buffer)
{
    if (!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        lcdd_GoudaMergeHandler();
        return;
    }
    // building set roi sequence:
    while (HAL_ERR_NO != hal_GoudaBlitRoi2Ram((UINT32 *)layer_Merge_buffer, 0, pRoi, lcdd_GoudaMergeHandler))
        ;
}

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
PUBLIC LCDD_ERR_T lcdd_LayerMerge(CONST LCDD_FBW_T *frameBufferWin, UINT16 startX, UINT16 startY, BOOL merge_state)
{
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;

    if (TRUE == g_lcddLoadSuccess)
    {

        if (0 == lcdd_MutexGet())
        {
            return LCDD_ERR_RESOURCE_BUSY;
        }
        else
        {
#ifdef OVERLAY_ALL_MAP
            // Set Input window
            inputWin.tlPX = 0;
            inputWin.brPX = (frameBufferWin->fb.width) - 1;
            inputWin.tlPY = 0;
            inputWin.brPY = (frameBufferWin->fb.height) - 1;
#else  // !OVERLAY_ALL_MAP
            // Set Input window
            inputWin.tlPX = frameBufferWin->roi.x;
            inputWin.brPX = frameBufferWin->roi.x + frameBufferWin->roi.width - 1;
            inputWin.tlPY = frameBufferWin->roi.y;
            inputWin.brPY = frameBufferWin->roi.y + frameBufferWin->roi.height - 1;
#endif // !OVERLAY_ALL_MAP

            // Check parameters
            // ROI must be within the screen boundary
            // ROI must be within the Frame buffer
            // Color format must be 16 bits
            BOOL badParam = FALSE;
            if (!badParam)
            {
                if ((frameBufferWin->roi.width > frameBufferWin->fb.width) ||
                    (frameBufferWin->roi.height > frameBufferWin->fb.height) ||
                    (frameBufferWin->fb.colorFormat != LCDD_COLOR_FORMAT_RGB_565))
                {
                    badParam = TRUE;
                    ;
                }
            }
            if (badParam)
            {
                lcdd_MutexFree();
                return LCDD_ERR_INVALID_PARAMETER;
            }

            if (merge_state)
            {
                if (frameBufferWin->fb.buffer != NULL)
                {
                    if (hal_GoudaVidLayerState() == 0 || g_lcddVidLayerMergeEnabled)
                    {
                        HAL_GOUDA_VID_LAYER_DEF_T gouda_vid_def = {
                            0,
                        };

                        gouda_vid_def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565;

                        gouda_vid_def.addrY = (UINT32 *)frameBufferWin->fb.buffer; // what about aligment ?

                        gouda_vid_def.alpha = 0xFF;
                        gouda_vid_def.cKeyColor = 0;
                        gouda_vid_def.cKeyEn = FALSE;
                        gouda_vid_def.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;
                        gouda_vid_def.depth = HAL_GOUDA_VID_LAYER_BEHIND_ALL;

                        gouda_vid_def.stride = 0;
#ifdef OVERLAY_ALL_MAP
                        gouda_vid_def.width = frameBufferWin->fb.width;
                        gouda_vid_def.height = frameBufferWin->fb.height;

                        gouda_vid_def.pos.tlPX = 0;
                        gouda_vid_def.pos.tlPY = 0;

                        gouda_vid_def.pos.brPX = (frameBufferWin->fb.width) - 1;
                        gouda_vid_def.pos.brPY = (frameBufferWin->fb.height) - 1;
#else  // !OVERLAY_ALL_MAP
                        gouda_vid_def.width = frameBufferWin->roi.width;
                        gouda_vid_def.height = frameBufferWin->roi.height;

                        gouda_vid_def.pos.tlPX = 0;
                        gouda_vid_def.pos.tlPY = 0;

                        gouda_vid_def.pos.brPX = (frameBufferWin->roi.width) - 1;
                        gouda_vid_def.pos.brPY = (frameBufferWin->roi.height) - 1;
#endif // !OVERLAY_ALL_MAP
                        hal_GoudaVidLayerClose();
                        hal_GoudaVidLayerOpen(&gouda_vid_def);
                        g_lcddAutoCloseMergedLayer = 1;
                    }
                    else
                    {
                        HAL_GOUDA_OVL_LAYER_DEF_T def;
                        // configure ovl layer 0 as buffer
                        def.addr = (UINT32 *)frameBufferWin->fb.buffer; // what about aligment ?
                        def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565;          //TODO convert from .colorFormat
                        //def.stride = frameBufferWin->fb.width * 2;
                        def.stride = 0; // let hal gouda decide
                        def.pos.tlPX = 0;
                        def.pos.tlPY = 0;
                        def.pos.brPX = frameBufferWin->fb.width - 1;
                        def.pos.brPY = frameBufferWin->fb.height - 1;
                        def.alpha = 255;
                        def.cKeyEn = FALSE;
                        //hal_HstSendEvent(0x1ee01);
                        // open the layer
                        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
                        hal_GoudaOvlLayerOpen(HAL_GOUDA_OVL_LAYER_ID0, &def);
                        // tell the end handler to close the layer when we are done ;)
                        g_lcddAutoCloseMergedLayer = 2;
                    }
                    lcdd_BlitRoi2Merge(&inputWin, frameBufferWin->fb.buffer);
                }
            }
            else
            {
                if (frameBufferWin->fb.buffer != NULL)
                {
                    g_lcddAutoCloseMergedLayer = 3;
                    lcdd_BlitRoi2Merge(&inputWin, frameBufferWin->fb.buffer);
                }
            }
            // gouda is doing everything ;)
            return LCDD_ERR_NO;
        }
    }
    return LCDD_ERR_NO;
}

void lcd_set_overlay(
    U8 layer_id,
    CONST UINT16 *frameBuffer,
    UINT16 srcX,
    UINT16 srcY,
    UINT16 src_pitch,
    UINT16 startX,
    UINT16 startY,
    UINT16 endX,
    UINT16 endY,
    bool src_key_enable,
    UINT32 src_key,
    UINT8 alpha)
{
    HAL_GOUDA_OVL_LAYER_DEF_T def;
    U32 layer_table[4];

    /* create this table, so we can use for loop to prcess each layer */

    layer_table[0] = HAL_GOUDA_OVL_LAYER_ID0;
    layer_table[1] = HAL_GOUDA_OVL_LAYER_ID1;
    layer_table[2] = HAL_GOUDA_OVL_LAYER_ID2;

    while (hal_GoudaIsActive())
    {
        sxr_Sleep(32);
    }

    hal_GoudaOvlLayerClose(layer_table[layer_id]);

    if (frameBuffer != NULL)
    {
        /* check if this layer need to merge or not */

        def.addr = (UINT32 *)(frameBuffer + (srcY)*src_pitch + srcX);
        def.pos.tlPX = startX;
        def.pos.tlPY = startY;
        def.pos.brPX = endX;
        def.pos.brPY = endY;

        // configure ovl layer 0 as buffer
        def.fmt = HAL_GOUDA_IMG_FORMAT_RGB565; //TODO convert from .colorFormat
        def.stride = src_pitch;                // let hal gouda decide

        def.alpha = alpha;
        def.cKeyEn = src_key_enable;
        def.cKeyColor = src_key;
        def.cKeyMask = HAL_GOUDA_CKEY_MASK_OFF;

        /* check if this layer need to merge or not */
        if (layer_id < HAL_GOUDA_OVL_LAYER_ID_QTY)
            hal_GoudaOvlLayerOpen(layer_table[layer_id], &def);
    }

} /* lcd_fb_update_overlay() */

// ============================================================================
// lcdd_GoudaMerge
// ----------------------------------------------------------------------------
/// This function provides the layer merging capabilities.
/// This function merges the overlay layer data to a given buffer.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_GoudaMerge(
    CONST UINT16 *frameBuffer,
    UINT16 startX,
    UINT16 startY,
    UINT16 endX,
    UINT16 endY,
    UINT16 width)
{
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;

    if (TRUE == g_lcddLoadSuccess)
    {

        if (0 == lcdd_MutexGet())
        {
            return LCDD_ERR_RESOURCE_BUSY;
        }
        else
        {
            if (frameBuffer != NULL)
            {
                // Set Input window
                inputWin.tlPX = startX;
                inputWin.brPX = endX;
                inputWin.tlPY = startY;
                inputWin.brPY = endY;

                // Check parameters
                // ROI must be within the screen boundary
                // ROI must be within the Frame buffer
                // Color format must be 16 bits
                /*
                            BOOL badParam = FALSE;
                            if (!badParam)
                            {
                                if (    (inputWin.brPX >= g_mciLcdScreenInfo.width) ||
                                        (inputWin.brPY >= g_mciLcdScreenInfo.height))
                                {
                                    badParam = TRUE;;
                                }
                            }
                            if (badParam)
                            {
                                lcdd_MutexFree();
                                return LCDD_ERR_INVALID_PARAMETER;
                            }
                */

                if (!((inputWin.tlPX < inputWin.brPX) && (inputWin.tlPY < inputWin.brPY)))
                {
                    lcdd_GoudaMergeHandler();
                    return LCDD_ERR_INVALID_PARAMETER;
                }
                // building set roi sequence:
                while (HAL_ERR_NO != hal_GoudaBlitRoi2Ram((UINT32 *)frameBuffer, width, &inputWin, lcdd_GoudaMergeHandler))
                    ;
            }
        }

        // gouda is doing everything ;)
        return LCDD_ERR_NO;
    }
    return LCDD_ERR_NO;
}

PUBLIC VOID lcdd_ChangeLCDSPIFreq(BOOL high)
{
    if (g_lcddDrv->lcdd_ChangeLcdSpiFreq != NULL)
    {
        g_lcddDrv->lcdd_ChangeLcdSpiFreq(high);
    }
}

PUBLIC VOID lcdd_GoudaBlitHandler(VOID)
{
    if (g_lcddDrv->lcdd_GoudaBltHdl != NULL)
        g_lcddDrv->lcdd_GoudaBltHdl();
}

#ifdef DUAL_LCD
//
/// Lock to protect the access to the LCD SUB screen during a DMA access.
/// When 1, access is allowed. When 0, it is denied.
PRIVATE UINT32 g_lcddSubLock = 0;
PRIVATE BOOL g_sublcdSleep = FALSE;
PUBLIC LCDD_REG_T *g_sublcddDrv;
PUBLIC LCDD_REG_T SublcddDrv_Table;
PRIVATE BOOL g_lcddSubOpened = FALSE;
BOOL(*g_SubLcddRegInit)
(LCDD_REG_T *) = {SUBLCDD_REGINIT_LIST};

PRIVATE BOOL lcdd_LoadSubDrv(VOID)
{
    //init sublcd Function
    g_sublcddDrv = &SublcddDrv_Table;
    lcdd_InitTable(g_sublcddDrv);
    if (g_SubLcddRegInit(g_sublcddDrv))
    {
        LCDD_TRACE(LCDD_INFO_TRC | TSTDOUT, 0, "SubLcdd load success");
        return TRUE;
    }
    return FALSE;
}

PUBLIC LCDD_ERR_T lcdd_SubOpen(VOID)
{
    LCDD_ERR_T subret = LCDD_ERR_NO;
    // Allow access to other LCD functions
    // Load SUBLCD driver
    hal_GoudaSetCsPin(FALSE);
    if (lcdd_LoadSubDrv())
    {
        if (g_sublcddDrv->lcdd_Open != NULL)
        {
            subret = g_sublcddDrv->lcdd_Open();
            if (subret == LCDD_ERR_NO)
            {
            }
        }
    }
    else
    {
        LCDD_TRACE(LCDD_INFO_TRC, 0, "SubLcdd load error");
    }
    g_lcddSubOpened = TRUE;
    return subret;
}

PUBLIC LCDD_ERR_T lcdd_SubClose(VOID)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);

    LCDD_ERR_T ret = LCDD_ERR_NO;

    // Forbid any access to other LCD functions
    g_lcddSubOpened = FALSE;

    if (g_sublcddDrv->lcdd_Close != NULL)
    {
        lcdd_SetBrightness(0);
        ret = g_sublcddDrv->lcdd_Close();
        if (ret == LCDD_ERR_NO)
        {
            // Disable access
            g_lcddSubLock = 0;
        }
    }

    return ret;
}

// ============================================================================
// lcdd_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SetSubStandbyMode(BOOL standbyMode)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);
    hal_GoudaSetCsPin(FALSE);
    if (g_sublcddDrv->lcdd_SetStandbyMode != NULL)
        return g_sublcddDrv->lcdd_SetStandbyMode(standbyMode);
    else
        return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================

PUBLIC LCDD_ERR_T lcdd_SubWakeUp(VOID)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);
    hal_GoudaSetCsPin(FALSE);
    if (g_sublcddDrv->lcdd_WakeUp != NULL)
    {
        LCDD_ERR_T ret = g_sublcddDrv->lcdd_WakeUp();
        g_sublcdSleep = FALSE;
        return ret;
    }

    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSleep(VOID)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);
    hal_GoudaSetCsPin(FALSE);
    if (!g_sublcdSleep)
    {
        if (g_sublcddDrv->lcdd_Sleep != NULL)
        {
            // Power off LCD
            LCDD_ERR_T ret = g_sublcddDrv->lcdd_Sleep();
            g_sublcdSleep = TRUE;
            return ret;
        }
    }
    return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubGetScreenInfo(LCDD_SCREEN_INFO_T *screenInfo)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);
    hal_GoudaSetCsPin(FALSE);
    if (g_sublcddDrv->lcdd_GetScreenInfo != NULL)
        return g_sublcddDrv->lcdd_GetScreenInfo(screenInfo);
    else
        return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_SetPixel16
// ----------------------------------------------------------------------------
/// Draw a 16-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 16-bit pixel data to draw.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubSetPixel16(UINT16 x, UINT16 y, UINT16 pixelData)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);
    hal_GoudaSetCsPin(FALSE);
    if (g_sublcddDrv->lcdd_SetPixel16 != NULL)
        return g_sublcddDrv->lcdd_SetPixel16(x, y, pixelData);
    else
        return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of the active window  with some color.
/// @param bgColor Color with which to fill the rectangle. It is a 16-bit
/// data, as the one of #lcdd_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PUBLIC LCDD_ERR_T lcdd_SubFillRect16(CONST LCDD_ROI_T *regionOfInterrest, UINT16 bgColor)
{
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);

    hal_GoudaSetCsPin(FALSE);
    if (g_sublcddDrv->lcdd_FillRect16 != NULL)
        return g_sublcddDrv->lcdd_FillRect16(regionOfInterrest, bgColor);
    else
        return LCDD_ERR_NO;
}

// ============================================================================
// lcdd_Blit16
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
PUBLIC LCDD_ERR_T lcdd_SubBlit16(CONST LCDD_FBW_T *frameBufferWin, UINT16 startX, UINT16 startY)
{
    LCDD_ERR_T ret = LCDD_ERR_NO;
    LCDD_ASSERT(g_lcddSubOpened, "LCDD: Call to %s while SubLcdd is not opened", __FUNCTION__);

    hal_GoudaSetCsPin(FALSE);
    if (g_sublcddDrv->lcdd_Blit16 != NULL)
    {
        //lcd_SemTake();
        ret = g_sublcddDrv->lcdd_Blit16(frameBufferWin, startX, startY);
        //lcd_SemFree();
        return ret;
    }
    else
        return LCDD_ERR_NO;
}

#endif

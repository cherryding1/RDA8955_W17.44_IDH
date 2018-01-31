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


#if (CSW_EXTENDED_API_LCD == 1)

#include "mci_lcd.h"
#include "mcip_debug.h"
#include "lcdd_m.h"
#include "string.h"
#include "sxr_tls.h"

#if (CHIP_HAS_GOUDA == 1)
#include "hal_gouda.h"
#include "hal_host.h"
#endif // (CHIP_HAS_GOUDA == 1)


PRIVATE LCDD_SCREEN_INFO_T  g_mciLcdScreenInfo;
PRIVATE UINT8               g_mciLcdBrightnessLevel = 0;
PRIVATE MCI_LCD_BPFUN_T     g_mciLcdBypassFunction = NULL;

// ============================================================================
// mci_LcdInit
// ----------------------------------------------------------------------------
// ============================================================================
VOID mci_LcdInit(UINT32 background)
{
    static VOLATILE BOOL visitFlag = 0;
    UINT32 status = hal_SysEnterCriticalSection();
    if (!visitFlag)
    {
        visitFlag = 1;
        g_mciLcdBypassFunction = NULL;
        hal_SysExitCriticalSection(status);

        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdInit");
        lcdd_Open();
        lcdd_GetScreenInfo(&g_mciLcdScreenInfo);
        mci_LcdClearScreen(background);
    }
    else
    {
        hal_SysExitCriticalSection(status);
    }
}

// ============================================================================
// mci_LcdStartBypass
// ----------------------------------------------------------------------------
// ============================================================================
BOOL mci_LcdStartBypass(MCI_LCD_BPFUN_T bypassFun)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (g_mciLcdBypassFunction == NULL)
    {
        g_mciLcdBypassFunction = bypassFun;
        hal_SysExitCriticalSection(status);
        lcdd_EnableVidLayerMerge(TRUE);
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdStartBypass");
        return TRUE;
    }
    else
    {
        hal_SysExitCriticalSection(status);
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdStartBypass : Error LCD already bypassed");
        return FALSE;
    }
}

// ============================================================================
// mci_LcdStopBypass
// ----------------------------------------------------------------------------
// ============================================================================
VOID mci_LcdStopBypass()
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdStopBypass");
    g_mciLcdBypassFunction = NULL;
    lcdd_EnableVidLayerMerge(FALSE);
}


// ============================================================================
// mci_LcdPowerOn
// ----------------------------------------------------------------------------
VOID mci_LcdPowerOn(BOOL on)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdPowerOn %d", on);
    // TODO
    // Currently not implemtented
    // Should use a pmd function
}

// ============================================================================
// mci_LcdScreenOn
// ----------------------------------------------------------------------------
VOID mci_LcdScreenOn(BOOL on)
{
    if ( on == TRUE )
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdScreenOn: Turn On with brightness %d", g_mciLcdBrightnessLevel);
        lcdd_SetStandbyMode(FALSE);
        lcdd_SetBrightness(g_mciLcdBrightnessLevel);
    }
    else
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdScreenOn: Turn Off");
        lcdd_SetBrightness(0);
        lcdd_SetStandbyMode(TRUE);
    }
}


#if (CHIP_HAS_GOUDA == 1)
extern PUBLIC LCDD_ERR_T lcdd_layerMerge(CONST LCDD_FBW_T *frameBufferWin, UINT16 startX, UINT16 startY);

// ============================================================================
// mci_LcdlayerMerge
// ----------------------------------------------------------------------------
VOID mci_LcdLayerMerge(UINT16 *buffer, UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy)
{
    LCDD_FBW_T frameBufferWin;
    UINT32 attempts = 0;

    if (!buffer)
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdLayerMerge buffer == 0");
    }
    else
    {
        //        MCI_TRACE(MCI_LCD_TRC_LVL,0,"mci_LcdLayerMerge %d %d %d %d",startx, starty, endx, endy);
        frameBufferWin.fb.buffer = buffer;
        frameBufferWin.fb.width = g_mciLcdScreenInfo.width;
        frameBufferWin.fb.height = g_mciLcdScreenInfo.height;
        frameBufferWin.fb.colorFormat = g_mciLcdScreenInfo.bitdepth;

        frameBufferWin.roi.x = startx;
        frameBufferWin.roi.y = starty;
        frameBufferWin.roi.width = endx - startx + 1;
        frameBufferWin.roi.height = endy - starty + 1;

        while(lcdd_LayerMerge(&frameBufferWin, startx, starty, TRUE) != LCDD_ERR_NO)
        {
            //            hal_HstSendEvent(0x1eee);
            attempts++;
            if (attempts % 1024)
            {
                MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdLayerMerge Access Denied :%d!", attempts);
            }
            sxr_Sleep(64);
        }
    }
}

#endif // (CHIP_HAS_GOUDA == 1)

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
#ifdef __NATIVE_JAVA__
unsigned char gcj_IsCurrentAppToLandscape();
unsigned char gcj_IsNeedRotate();
#endif

VOID mci_LcdBlockWrite(UINT16 *buffer, UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy)
{
    LCDD_FBW_T frameBufferWin;
    UINT32 attempts = 0;
    if (g_mciLcdBypassFunction)
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdBlockWrite bypassed %d %d %d %d", startx, starty, endx, endy);
        g_mciLcdBypassFunction(buffer, startx, starty, endx, endy);
    }
    else
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdBlockWrite %d %d %d %d", startx, starty, endx, endy);
        frameBufferWin.fb.buffer = buffer;
#ifdef __NATIVE_JAVA__
        if (gcj_IsCurrentAppToLandscape() && gcj_IsNeedRotate())
        {
            frameBufferWin.fb.width = g_mciLcdScreenInfo.height;
            frameBufferWin.fb.height = g_mciLcdScreenInfo.width;
        }
        else
#endif
        {
            frameBufferWin.fb.width = g_mciLcdScreenInfo.width;
            frameBufferWin.fb.height = g_mciLcdScreenInfo.height;
        }
        frameBufferWin.fb.colorFormat = g_mciLcdScreenInfo.bitdepth;

        frameBufferWin.roi.x = startx;
        frameBufferWin.roi.y = starty;
        frameBufferWin.roi.width = endx - startx + 1;
        frameBufferWin.roi.height = endy - starty + 1;

        while(lcdd_Blit16(&frameBufferWin, startx, starty) != LCDD_ERR_NO)
        {
            attempts++;
            if (attempts % 1024)
            {
                MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdBlockWrite Access Denied :%d!", attempts);
            }
            sxr_Sleep(64);
        }

    }
}

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
VOID mci_LcdGetDimension(UINT16 *out_LCD_width, UINT16 *out_LCD_height)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdGetDimension");
    *out_LCD_width = g_mciLcdScreenInfo.width;
    *out_LCD_height = g_mciLcdScreenInfo.height;
}

// ============================================================================
// mci_LcdSleep
// ----------------------------------------------------------------------------
VOID mci_LcdSleep(VOID)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSleep");
    lcdd_SetBrightness(0);
    lcdd_Sleep();
}

// ============================================================================
// mci_LcdWakeup
// ----------------------------------------------------------------------------
VOID mci_LcdWakeup(VOID)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdWakeup");
    lcdd_WakeUp();
    lcdd_SetBrightness(g_mciLcdBrightnessLevel);
}

// ============================================================================
// mci_LcdPartialOn
// ----------------------------------------------------------------------------
VOID mci_LcdPartialOn(UINT16 startLine, UINT16 endLine)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdPartialOn %d %d", startLine, endLine);
    //lcdd_PartialOn(startLine, endLine);
}

// ============================================================================
// mci_LcdPartialOff
// ----------------------------------------------------------------------------
VOID mci_LcdPartialOff(VOID)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdPartialOff");
    //lcdd_PartialOff();
}

// ============================================================================
// mci_LcdClearScreen
// ----------------------------------------------------------------------------
VOID mci_LcdClearScreen(UINT16 background)
{
    LCDD_ROI_T roi;

    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdClearScreen %d", background);
    roi.x = 0;
    roi.y = 0;
    roi.width = g_mciLcdScreenInfo.width;
    roi.height = g_mciLcdScreenInfo.height;

    lcdd_FillRect16(&roi, background);
}

// ============================================================================
// mci_LcdGetParam
// ----------------------------------------------------------------------------
UINT8 mci_LcdGetParam(MCI_LCD_PARAM_T param_id)
{
    switch(param_id)
    {
        // TODO implement other parameters
        case MCI_LCD_BIAS:
        case MCI_LCD_CONTRAST:
        case MCI_LCD_LINE_RATE:
        case MCI_LCD_TEMP_COMP:
            return 0;
            break;
        case MCI_LCD_BRIGHTNESS:
            return g_mciLcdBrightnessLevel;
            break;
        default:
            break;
    }
    return 0;
}

// ============================================================================
// mci_LcdSetBrightness
// ----------------------------------------------------------------------------
VOID mci_LcdSetBrightness(UINT8 level)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSetBrightness %d", level);
    g_mciLcdBrightnessLevel = level;
    lcdd_SetBrightness(g_mciLcdBrightnessLevel);
}
VOID mci_LcdSetBrightnessExt(UINT8 level)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSetBrightnessLevel %d", level);
    lcdd_SetBrightness(level);
}
// ============================================================================
// mci_LcdSetBias
// ----------------------------------------------------------------------------
VOID mci_LcdSetBias(UINT8 bias)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSetBias %d", bias);
}

// ============================================================================
// mci_LcdSetContrast
// ----------------------------------------------------------------------------
VOID mci_LcdSetContrast(UINT8 contrast)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSetContrast %d", contrast);
    lcdd_SetContrast(contrast);
}

// ============================================================================
// mci_LcdSetLineRate
// ----------------------------------------------------------------------------
VOID mci_LcdSetLineRate(UINT8 linerate)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSetLineRate %d", linerate);
}

// ============================================================================
// mci_LcdSetTempComp
// ----------------------------------------------------------------------------
VOID mci_LcdSetTempComp(UINT8 compensate)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSetTempComp %d", compensate);
}

VOID mci_LcdChangeSpiFreq(BOOL high)
{
    lcdd_ChangeLCDSPIFreq(high);
}
#ifdef DUAL_LCD

PRIVATE LCDD_SCREEN_INFO_T  g_mciSubLcdScreenInfo;
//PRIVATE UINT8               g_mciSubLcdBrightnessLevel=0;
PUBLIC MCI_LCD_BPFUN_T     g_mciSubLcdBypassFunction = NULL;


VOID mci_LcdSubClearScreen(UINT16 background)
{
    LCDD_ROI_T roi;

    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubClearScreen %d", background);
    roi.x = 0;
    roi.y = 0;
    roi.width = g_mciSubLcdScreenInfo.width;
    roi.height = g_mciSubLcdScreenInfo.height;

    lcdd_SubFillRect16(&roi, background);
}

VOID mci_LcdSubInit(UINT32 background)
{
    static BOOL subvisitFlag = 0;
    if (!subvisitFlag)
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubInit");
        lcdd_SubOpen();
        lcdd_SubGetScreenInfo(&g_mciSubLcdScreenInfo);
        mci_LcdSubClearScreen(background);
        g_mciSubLcdBypassFunction = NULL;
        subvisitFlag = 1;
    }
}

BOOL mci_LcdSubStartBypass(MCI_LCD_BPFUN_T bypassFun)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (g_mciSubLcdBypassFunction == NULL)
    {
        g_mciSubLcdBypassFunction = bypassFun;
        hal_SysExitCriticalSection(status);
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubStartBypass");
        return TRUE;
    }
    else
    {
        hal_SysExitCriticalSection(status);
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubStartBypass : Error SUBLCD already bypassed");
        return FALSE;
    }
}
VOID mci_LcdSubLayerMerge(UINT16 *buffer, UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy)
{
    LCDD_FBW_T frameBufferWin;
    UINT32 attempts = 0;

    if (!buffer)
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdLayerMerge buffer == 0");
    }
    else
    {
        frameBufferWin.fb.buffer = buffer;
        frameBufferWin.fb.width = g_mciSubLcdScreenInfo.width;
        frameBufferWin.fb.height = g_mciSubLcdScreenInfo.height;
        frameBufferWin.fb.colorFormat = g_mciSubLcdScreenInfo.bitdepth;

        frameBufferWin.roi.x = startx;
        frameBufferWin.roi.y = starty;
        frameBufferWin.roi.width = endx - startx + 1;
        frameBufferWin.roi.height = endy - starty + 1;

        while(lcdd_LayerMerge(&frameBufferWin, startx, starty, TRUE) != LCDD_ERR_NO)
        {
            attempts++;
            if (attempts % 1024)
            {
                MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdLayerMerge Access Denied :%d!", attempts);
            }
            sxr_Sleep(64);
        }
    }
}

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
VOID mci_LcdSubBlockWrite(UINT16 *buffer, UINT16 startx, UINT16 starty, UINT16 endx, UINT16 endy)
{
    LCDD_FBW_T frameBufferWin;
    UINT32 attempts = 0;

    if (g_mciSubLcdBypassFunction)
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubBlockWrite bypassed %d %d %d %d", startx, starty, endx, endy);
        g_mciLcdBypassFunction(buffer, startx, starty, endx, endy);
    }
    else
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubBlockWrite %d %d %d %d", startx, starty, endx, endy);
        frameBufferWin.fb.buffer = buffer;
        frameBufferWin.fb.width = g_mciSubLcdScreenInfo.width;
        frameBufferWin.fb.height = g_mciSubLcdScreenInfo.height;
        frameBufferWin.fb.colorFormat = g_mciSubLcdScreenInfo.bitdepth;

        frameBufferWin.roi.x = startx;
        frameBufferWin.roi.y = starty;
        frameBufferWin.roi.width = endx - startx + 1;
        frameBufferWin.roi.height = endy - starty + 1;

        while(lcdd_SubBlit16(&frameBufferWin, startx, starty) != LCDD_ERR_NO)
        {
            attempts++;
            if (attempts % 1024)
            {
                MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubBlockWrite Access Denied :%d!", attempts);
            }
            sxr_Sleep(64);
        }

    }
}

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
VOID mci_LcdSubGetDimension(UINT16 *out_LCD_width, UINT16 *out_LCD_height)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubGetDimension");
    *out_LCD_width = g_mciSubLcdScreenInfo.width;
    *out_LCD_height = g_mciSubLcdScreenInfo.height;
}

// ============================================================================
// mci_LcdSleep
// ----------------------------------------------------------------------------
VOID mci_LcdSubSleep(VOID)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubSleep");
    //lcdd_SetBrightness(0);
    lcdd_SubSleep();
}

// ============================================================================
// mci_LcdWakeup
// ----------------------------------------------------------------------------
VOID mci_LcdSubWakeup(VOID)
{
    MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubWakeup");
    lcdd_SubWakeUp();
    //lcdd_SetBrightness(g_mciSubLcdBrightnessLevel);
}
VOID mci_LcdSubPartialOn(UINT16 startLine, UINT16 endLine)
{

}
VOID mci_LcdSubPartialOff(VOID)
{

}
VOID mci_LcdSubScreenOn(BOOL on)
{
    if ( on == TRUE )
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubScreenOn: Turn On with brightness");
        lcdd_SetSubStandbyMode(FALSE);
    }
    else
    {
        MCI_TRACE(MCI_LCD_TRC_LVL, 0, "mci_LcdSubScreenOn: Turn Off");
        lcdd_SetSubStandbyMode(TRUE);
    }
}
VOID mci_LcdSubSetContrast(UINT8 contrast)
{

}
#endif
#endif // CSW_EXTENDED_API_LCD



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
#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr>>8)&0xFF))        != HAL_ERR_NO);while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);  while(hal_GoudaWriteData((((Data)>>8)&0xFF))      != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData((((Data)>>8)&0xFF))     != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))     != HAL_ERR_NO); }
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(((Cmd>>8)&0xFF))       != HAL_ERR_NO);while(hal_GoudaWriteCmd(((Cmd)&0xFF))      != HAL_ERR_NO);}
#else
#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO);}
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
#ifdef LCD_SCALE_176_220_TO_240_320
// Number of actual pixels in the display width
#define LCDD_DISP_X             176

// Number of pixels in the display height
#define LCDD_DISP_Y             220

#else // !LCD_SCALE_176_220_TO_240_320
// Number of actual pixels in the display width
#define LCDD_DISP_X             240

// Number of pixels in the display height
#define LCDD_DISP_Y             320

#endif // !LCD_SCALE_176_220_TO_240_320

#define  LCD_HX8347G_ID       0x75

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
#ifdef LCD_SCALE_176_220_TO_240_320
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID1);
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID2);
        hal_GoudaVidLayerClose();
#endif
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
#ifdef lcd_line_8_low
    LCM_WR_REG(0x2B,0x06);  //DUM
    LCM_WR_REG(0x29,0x8F);  //RTN
    LCM_WR_REG(0x2E,0x88);  //GDOFF

    LCM_WR_REG(0xE2,0x08);  //VREF
    LCM_WR_REG(0xE4,0x01);  //EQ1
    LCM_WR_REG(0xE5,0x02);  //EQ2
    LCM_WR_REG(0xE6,0x02);  //EQ3
    LCM_WR_REG(0xE7,0x01);  //EQ4

    LCM_WR_REG(0xEA,0x00);  //PTBA[15:8]
    LCM_WR_REG(0xEB,0x20);  //PTBA[7:0]
    LCM_WR_REG(0xEC,0x3C);  //STBA[15:8]
    LCM_WR_REG(0xED,0xC4);  //STBA[7:0]
    LCM_WR_REG(0xE8,0x70);  //OPON[7:0]

    //Gamma 2.2 Setting
    LCM_WR_REG(0x40,0x02); //
    LCM_WR_REG(0x41,0x08); //
    LCM_WR_REG(0x42,0x14); //
    LCM_WR_REG(0x43,0x23); //
    LCM_WR_REG(0x44,0x1E); //
    LCM_WR_REG(0x45,0x26); //
    LCM_WR_REG(0x46,0x2F); //
    LCM_WR_REG(0x47,0x75); //
    LCM_WR_REG(0x48,0x03); //
    LCM_WR_REG(0x49,0x17); //
    LCM_WR_REG(0x4A,0x1B); //
    LCM_WR_REG(0x4B,0x1A); //
    LCM_WR_REG(0x4C,0x16); //

    LCM_WR_REG(0x50,0x19); //
    LCM_WR_REG(0x51,0x21); //
    LCM_WR_REG(0x52,0x1C); //
    LCM_WR_REG(0x53,0x2B); //
    LCM_WR_REG(0x54,0x37); //
    LCM_WR_REG(0x55,0x3D); //
    LCM_WR_REG(0x56,0x0A); //
    LCM_WR_REG(0x57,0x50); //
    LCM_WR_REG(0x58,0x09); //
    LCM_WR_REG(0x59,0x05); //
    LCM_WR_REG(0x5A,0x04); //
    LCM_WR_REG(0x5B,0x08); //
    LCM_WR_REG(0x5C,0x1C); //
    LCM_WR_REG(0x5D,0xCC); //


    //Power Voltage Setting
    LCM_WR_REG(0x1B,0x1B);  //VRH=4.65V
    LCM_WR_REG(0x1A,0x02);  //BT (VGH~15V,VGL~-7V,DDVDH~5V)
    LCM_WR_REG(0x24,0x70);  //VMH(VCOM High voltage ~4.2V)
    LCM_WR_REG(0x25,0x58);  //VML(VCOM Low voltage -1.2V)
    //****VCOM offset**///
    LCM_WR_REG(0x23,0x86);  //for Flicker adjust //can reload from OTP

    //Power on Setting
    LCM_WR_REG(0x18,0x36);  //I/P_RADJ,N/P_RADJ, Normal mode 70Hz
    LCM_WR_REG(0x19,0x01);  //OSC_EN='1', start Osc
    LCM_WR_REG(0x01,0x00);  //DP_STB='0', out deep sleep
    LCM_WR_REG(0x1F,0x88);  // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x80);  // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x90);  // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0xD0);  // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    sxr_Sleep(5 MS_WAITING);

    //262k/65k color selection
    LCM_WR_REG(0x16,0xC0);
    LCM_WR_REG(0x17,0x05);  //default 0x06 262k color // 0x05 65k color

    //SET PANEL
    LCM_WR_REG(0x36,0x09);  //SS_P, GS_P,REV_P,BGR_P
    //Display ON Setting
    LCM_WR_REG(0x28,0x38);  //GON=1, DTE=1, D=1000
    sxr_Sleep(40 MS_WAITING);
    LCM_WR_REG(0x28,0x3C);  //GON=1, DTE=1, D=1100

    //Set GRAM Area
    LCM_WR_REG(0x02,0x00);
    LCM_WR_REG(0x03,0x00);  //Column Start
    LCM_WR_REG(0x04,0x00);
    LCM_WR_REG(0x05,0xEF);  //Column End

    LCM_WR_REG(0x06,0x00);
    LCM_WR_REG(0x07,0x00);  //Row Start
    LCM_WR_REG(0x08,0x01);
    LCM_WR_REG(0x09,0x3F);  //Row End

    LCM_WR_CMD(0x22);   //Start GRAM write
#else
    LCM_WR_REG(0xEA,0x00);
    LCM_WR_REG(0xEB,0x20);
    // Power saving for HX8347G
    LCM_WR_REG(0xEC,0x3C); // Command page 0
    LCM_WR_REG(0xED,0xC4); // GENON=0x00
    LCM_WR_REG(0xE8,0x48); // EQVCI_M1=0x00
    LCM_WR_REG(0xE9,0x38); // EQGND_M1=0x1C
    LCM_WR_REG(0xF1,0x01); // EQVCI_M0=0x1C
    LCM_WR_REG(0xF2,0x08);
    LCM_WR_REG(0x36,0x09);
    // Gamma   For this gamma VCOMH=0x92   VCOML=0x89   VCOMOFFSET=0x4D
    LCM_WR_REG(0x40,0x01);
    LCM_WR_REG(0x41,0x07);
    LCM_WR_REG(0x42,0x16);
    LCM_WR_REG(0x43,0x1e);
    LCM_WR_REG(0x44,0x1b);
    LCM_WR_REG(0x45,0x26);
    LCM_WR_REG(0x46,0x25);
    LCM_WR_REG(0x47,0x6f);
    LCM_WR_REG(0x48,0x02);
    LCM_WR_REG(0x49,0x1b);
    LCM_WR_REG(0x4A,0x1d);
    LCM_WR_REG(0x4B,0x1b);
    LCM_WR_REG(0x4C,0x16);
    LCM_WR_REG(0x50,0x19);
    LCM_WR_REG(0x51,0x24);
    LCM_WR_REG(0x52,0x21);
    LCM_WR_REG(0x53,0x29);
    LCM_WR_REG(0x54,0x38);
    LCM_WR_REG(0x55,0x3e);
    LCM_WR_REG(0x56,0x10);
    LCM_WR_REG(0x57,0x5a);
    LCM_WR_REG(0x58,0x09);
    LCM_WR_REG(0x59,0x04);
    LCM_WR_REG(0x5A,0x02);
    LCM_WR_REG(0x5B,0x04);
    LCM_WR_REG(0x5C,0x1d);
    LCM_WR_REG(0x5D,0xCC);
    // Set GRAM area 320x240
    LCM_WR_REG(0x02,0x00);
    LCM_WR_REG(0x03,0x00);
    LCM_WR_REG(0x04,0x00);
    LCM_WR_REG(0x05,0xEF);
    LCM_WR_REG(0x06,0x00);
    LCM_WR_REG(0x07,0x00);
    LCM_WR_REG(0x08,0x01);
    LCM_WR_REG(0x09,0x3F);
    // Power Setting
    LCM_WR_REG(0x1b,0x1b);
    LCM_WR_REG(0x1A,0x01);
    LCM_WR_REG(0x23,0x70);
    LCM_WR_REG(0x24,0x92); // Set VCOMH voltage, VHH=0x64
    LCM_WR_REG(0x25,0x89); // Set VCOML voltage, VML=0x71
    LCM_WR_REG(0x23,0x4D); // Set VCOM offset, VMF=0x52
    // Power on Setting
    LCM_WR_REG(0x18,0x36); // OSC_EN=1, Start to Oscillate
    LCM_WR_REG(0x19,0x01); // AP=011
    LCM_WR_REG(0x01,0x00); // Normal display(Exit Deep standby mode)
    LCM_WR_REG(0x1F,0x88); // Exit standby mode and Step-up circuit 1 enable
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x80); // Step-up circuit 2 enable
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x90);
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0xD0);
    sxr_Sleep(5 MS_WAITING);
    // Display ON Setting
    LCM_WR_REG(0x17,0x05); // GON=1, DTE=1, D[1:0]=10
    sxr_Sleep(40 MS_WAITING);
    LCM_WR_REG(0x28,0x38); // GON=1, DTE=1, D[1:0]=11
    LCM_WR_REG(0x28,0x3C); // 16-bit/pixel
    LCM_WR_CMD(0x0022);

    sxr_Sleep(50 MS_WAITING);

    LCM_WR_REG(0xFF,0x00); //Select Command Page 0
    LCM_WR_REG(0x19,0x01); //OSC_EN=1, Start to Oscillate
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x88);
    // Power on Setting
    LCM_WR_REG(0x1C,0x03); // AP=011
    LCM_WR_REG(0x1F,0x80); // Exit standby mode and Step-up circuit 1 enable
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x90); // Step-up circuit 2 enable
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0xD4);
    sxr_Sleep(5 MS_WAITING);
    // Display on Setting
    LCM_WR_REG(0x28,0x08); // GON=0, DTE=0, D[1:0]=01
    sxr_Sleep(40 MS_WAITING);
    LCM_WR_REG(0x28,0x38); // GON=1, DTE=1, D[1:0]=10
    sxr_Sleep(40 MS_WAITING);
    LCM_WR_REG(0x28,0x3C); // GON=1, DTE=1, D[1:0]=11
#endif

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

    LCM_WR_REG(0xFF,0x00); //Select Command Page 0
    // Display off Setting
    LCM_WR_REG(0x28,0x38); // GON=1, DTE=1, D[1:0]=10
    sxr_Sleep(40 MS_WAITING);
    LCM_WR_REG(0x28,0x04); // GON=0, DTE=0, D[1:0]=01
    // Power off Setting
    LCM_WR_REG(0x1F,0x90); // Stop VCOMG
    sxr_Sleep(5 MS_WAITING);
    LCM_WR_REG(0x1F,0x88); // Stop step-up circuit
    LCM_WR_REG(0x1C,0x00); // AP=000
    LCM_WR_REG(0x1F,0x89); // Enter Standby mode
    LCM_WR_REG(0x19,0x00); //OSC_EN=0, Stop to Oscillate

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
#ifdef LCD_SCALE_176_220_TO_240_320
        screenInfo->width = 176;
        screenInfo->height = 220;
#else // !LCD_SCALE_176_220_TO_240_320
        screenInfo->width = LCDD_DISP_X;
        screenInfo->height = LCDD_DISP_Y;
#endif // !LCD_SCALE_176_220_TO_240_320
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

        LCM_WR_REG(0x0002,(x>>8)&0x00ff); // Column address start2
        LCM_WR_REG(0x0003,x&0x00ff); // Column address start1
        LCM_WR_REG(0x0004,(x>>8)&0x00ff); // Column address start2
        LCM_WR_REG(0x0005,x&0x00ff); // Column address start1

        LCM_WR_REG(0x0006,(y>>8)&0x00ff); // Row address start2
        LCM_WR_REG(0x0007,y&0x00ff); // Row address start1
        LCM_WR_REG(0x0008,(y>>8)&0x00ff); // Row address start2
        LCM_WR_REG(0x0009,y&0x00ff); // Row address start1

        LCM_WR_REG(0x0022, pixelData);

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
    HAL_GOUDA_LCD_CMD_T cmd[1];

    if(!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        LCDD_TRACE(LCDD_WARN_TRC,0,"lcddp_BlitRoi2Win: Invalid Roi x:%d < %d, y:%d < %d",pRoi->tlPX, pRoi->brPX, pRoi->tlPY, pRoi->brPY);
        lcddp_GoudaBlitHandler();
        return;
    }

    // building set roi sequence:

    LCM_WR_REG(0x0002,(pActiveWin->tlPX>>8)&0x00ff); // Column address start2
    LCM_WR_REG(0x0003,pActiveWin->tlPX&0x00ff); // Column address start1
    LCM_WR_REG(0x0004,(pActiveWin->brPX>>8)&0x00ff); // Column address end2
    LCM_WR_REG(0x0005,pActiveWin->brPX&0x00ff); // Column address end1

    LCM_WR_REG(0x0006,(pActiveWin->tlPY>>8)&0x00ff); // Row address start2
    LCM_WR_REG(0x0007,pActiveWin->tlPY&0x00ff); // Row address start1
    LCM_WR_REG(0x0008,(pActiveWin->brPY>>8)&0x00ff); // Row address end2
    LCM_WR_REG(0x0009,pActiveWin->brPY&0x00ff); // Row address end1

    LCM_WR_CMD(0x0022);

    // Send command after which the data we sent
    // are recognized as pixels.
//    LCDD_BUILD_CMD_WR_CMD(cmd,16,0x0022);
//    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 17, cmd, lcddp_GoudaBlitHandler));
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
#ifdef LCD_SCALE_176_220_TO_240_320
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

            gouda_vid_def.pos.brPX = (frameBufferWin->fb.width )*240/176- 1;
            gouda_vid_def.pos.brPY = (frameBufferWin->fb.height)*320/220 - 1;

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
#else // !LCD_SCALE_176_220_TO_240_320
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
#endif // !LCD_SCALE_176_220_TO_240_320
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
#ifdef lcd_line_8_low
    LCM_WR_REG(0x0016, 0xA0);  //
#else
    LCM_WR_REG(0x0016, 0x60);  //
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
#ifdef lcd_line_8_low
    LCM_WR_REG(0x16,0xC0); //
#else
    LCM_WR_REG(0x0016, 0x0);  //
#endif
    lcdd_MutexFree();

    return TRUE;
}

PRIVATE char* lcdd_get_id_string(void)
{
    static char hx8347g_id_str[] = "hx8347g\n";
    return hx8347g_id_str;
}
PRIVATE BOOL lcddp_CheckProductId()
{
    UINT16 productId;
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
    sxr_Sleep(20 MS_WAITING);

    hal_GoudaReadReg(0x0, &productId);

    hal_GoudaClose();
    SXS_TRACE(TSTDOUT, "hx8347g(0x%x): lcd read id is 0x%x ", LCD_HX8347G_ID, productId);
    hal_HstSendEvent(0x1cd08347);

    hal_HstSendEvent(productId);
    if((productId & 0x0FF)== LCD_HX8347G_ID)
        return TRUE;
    else
        return FALSE;
}

// ============================================================================
// lcdd_hx8347g_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL  lcdd_hx8347g_RegInit(LCDD_REG_T *pLcdDrv)
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



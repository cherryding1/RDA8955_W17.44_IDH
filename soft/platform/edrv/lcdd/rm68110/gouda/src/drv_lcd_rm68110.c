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
#if 0
#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO); }
#define LCM_WR_DATA_8(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD_8(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}


//low 8bit
#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr>>8)&0xFF))        != HAL_ERR_NO);while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);  while(hal_GoudaWriteData((((Data)>>8)&0xFF))      != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
#define LCM_WR_DATA(Data)        { while(hal_GoudaWriteData((((Data)>>8)&0xFF))     != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))     != HAL_ERR_NO); }
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(((Cmd>>8)&0xFF))       != HAL_ERR_NO);while(hal_GoudaWriteCmd(((Cmd)&0xFF))      != HAL_ERR_NO);}
#endif

#if 0//def lcd_line_8_low

//low 8bit
//#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);   while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
//#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(((Data)&0xFF))     != HAL_ERR_NO); }
//#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(((Cmd)&0xFF))      != HAL_ERR_NO);}
#define  LCM_WR_REG(Addr, Data)      { while(hal_GoudaWriteCmd(((Addr>>8)&0xFF))        != HAL_ERR_NO);while(hal_GoudaWriteCmd(((Addr)&0xFF))        != HAL_ERR_NO);  while(hal_GoudaWriteData((((Data)>>8)&0xFF))      != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))      != HAL_ERR_NO); }
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData((((Data)>>8)&0xFF))     != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))     != HAL_ERR_NO); }
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(((Cmd>>8)&0xFF))       != HAL_ERR_NO);while(hal_GoudaWriteCmd(((Cmd)&0xFF))      != HAL_ERR_NO);}

#else

#define LCM_WR_REG(Addr, Data)  { while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO); }
#define LCM_WR_DAT(Data)        { while(hal_GoudaWriteData(Data)     != HAL_ERR_NO);}
#define LCM_WR_CMD(Cmd)         { while(hal_GoudaWriteCmd(Cmd)       != HAL_ERR_NO);}
#define LCM_WR_DATA(Data)        { while(hal_GoudaWriteData((((Data)>>8)&0xFF))     != HAL_ERR_NO);  while(hal_GoudaWriteData(((Data)&0xFF))     != HAL_ERR_NO); }//use it!
#endif


#if 0//def USE_SPI_LCD
#define main_Write_COM  LCM_WR_CMD_8
#define main_Write_DATA LCM_WR_DATA_8
#else
#define main_Write_COM  LCM_WR_CMD
#define main_Write_DATA LCM_WR_DAT
#endif

#define LCDD_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define LCDD_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define LCDD_BUILD_CMD_WR_REG(c,i,r,d) do{LCDD_BUILD_CMD_WR_CMD(c,i,r); LCDD_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)

#define LCDD_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width
#define LCDD_DISP_X     128

// Number of pixels in the display height
#define LCDD_DISP_Y     160

#ifdef USE_SPI_LCD
//#define  LCD_RM68110_ID        0x0054
#define  LCD_RM68110_ID        0xb400

#else
#define LCD_RM68110_ID     0x0068
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
PRIVATE VOID lcddp_Init(VOID)
{
    // Init code
#if 0
    LCM_WR_CMD(0x01);
    hal_TimDelay(100 MS_WAITING);


//--************ Start Initial Sequence **********--//
    main_Write_COM(0x11); //Exit Sleep
    hal_TimDelay(120 MS_WAITING);



    main_Write_COM(0x26); //Set Default Gamma
    main_Write_DATA(0x04);

    main_Write_COM(0xB1);
    main_Write_DATA(0x0e);
    main_Write_DATA(0x14);

    main_Write_COM(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    main_Write_DATA(0x08);
    main_Write_DATA(0x00);

    main_Write_COM(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    main_Write_DATA(0x05);

    main_Write_COM(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x46);//0x46
    main_Write_DATA(0x40);//0x40

    main_Write_COM(0xC7);
    main_Write_DATA(0xBD);  //0xC2




    main_Write_COM(0x3a); //Set Color Format
    main_Write_DATA(0x05);

    main_Write_COM(0x2A); //Set Column Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);
    main_Write_COM(0x2B); //Set Page Address
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0xB4); //Set Source Output Direction
    main_Write_DATA(0x00);

    main_Write_COM(0xf2); //Enable Gamma bit
    main_Write_DATA(0x01);


    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xC8);  //0xc0

    main_Write_COM(0xE0);
    main_Write_DATA(0x3F);//p1
    main_Write_DATA(0x26);//p2
    main_Write_DATA(0x23);//p3
    main_Write_DATA(0x30);//p4
    main_Write_DATA(0x28);//p5
    main_Write_DATA(0x10);//p6
    main_Write_DATA(0x55);//p7
    main_Write_DATA(0xB7);//p8
    main_Write_DATA(0x40);//p9
    main_Write_DATA(0x19);//p10
    main_Write_DATA(0x10);//p11
    main_Write_DATA(0x1E);//p12
    main_Write_DATA(0x02);//p13
    main_Write_DATA(0x01);//p14
    main_Write_DATA(0x00);//p15

    main_Write_COM(0xE1);
    main_Write_DATA(0x00);//p1
    main_Write_DATA(0x19);//p2
    main_Write_DATA(0x1C);//p3
    main_Write_DATA(0x0F);//p4
    main_Write_DATA(0x14);//p5
    main_Write_DATA(0x0F);//p6
    main_Write_DATA(0x2A);//p7
    main_Write_DATA(0x48);//p8
    main_Write_DATA(0x3F);//p9
    main_Write_DATA(0x06);//p10
    main_Write_DATA(0x1D);//p11
    main_Write_DATA(0x21);//p12
    main_Write_DATA(0x3D);//p13
    main_Write_DATA(0x3E);//p14
    main_Write_DATA(0x3F);//p15

    main_Write_COM(0x29); // Display On

    LCM_WR_CMD_8(0x2c); //WRITE ram Data display
    hal_TimDelay(100 MS_WAITING);
#else

#ifdef USE_SPI_LCD
    main_Write_COM(0x01);       // Software Reset
    main_Write_COM(0x11);       // Sleep Out
    hal_TimDelay(120 MS_WAITING);               // Must Delay At Least 100 ms

    main_Write_COM(0xC0);       // Power Control 1
    main_Write_DATA(0xF0);      // GVDD Setting//0xF1//调节亮度，值越大越亮，颜色就越淡，
    main_Write_DATA(0x10);      // GVCL Setting//0x11//以上面同步

    main_Write_COM(0xC1);     //set VGH&VGL driver voltage
    main_Write_DATA(0x09);//ff

    main_Write_COM(0xB4);
    main_Write_DATA(0x03);

    main_Write_COM(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x0A); //31

    main_Write_COM(0xF8);
    main_Write_DATA(0x01);

    main_Write_COM(0x3A);       //设置色位;
    main_Write_DATA(0x05);
    //************* Start Gamma Setting **********//

    main_Write_COM(0xE0);       // Gamma Command
    main_Write_DATA(0x00);
    main_Write_DATA(0x01);
    main_Write_DATA(0x05);
    main_Write_DATA(0x25);
    main_Write_DATA(0x22);
    main_Write_DATA(0x1F);
    main_Write_DATA(0x03);
    main_Write_DATA(0x0d);
    main_Write_DATA(0x07);
    main_Write_DATA(0x00);
    main_Write_DATA(0x0A);
    main_Write_DATA(0x09);
    main_Write_DATA(0x06);
    main_Write_DATA(0x0B);
    main_Write_DATA(0x03);
    main_Write_DATA(0x07);
    main_Write_COM(0xE1);       // Gamma Command
    main_Write_DATA(0x00);
    main_Write_DATA(0x01);
    main_Write_DATA(0x05);
    main_Write_DATA(0x25);
    main_Write_DATA(0x22);
    main_Write_DATA(0x1F);
    main_Write_DATA(0x03);
    main_Write_DATA(0x0d);
    main_Write_DATA(0x07);
    main_Write_DATA(0x00);
    main_Write_DATA(0x0A);
    main_Write_DATA(0x09);
    main_Write_DATA(0x06);
    main_Write_DATA(0x0B);
    main_Write_DATA(0x03);
    main_Write_DATA(0x07);

    main_Write_COM(0x36);       // Memory Data Access Control//
    main_Write_DATA(0xC8);   //0xC8   // RGB Color Filter Setting

    main_Write_COM(0xB1);       //set gamma ver
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x04);

    main_Write_COM(0xF4);
    main_Write_DATA(0x00);
    main_Write_DATA(0x08);


    main_Write_COM(0xC7);
    main_Write_DATA(0x18);//调节fincker

    main_Write_COM(0x2A);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);

    main_Write_COM(0x2B);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0x29);       // Display Ondelay(20);
    hal_TimDelay(20 MS_WAITING);
    main_Write_COM(0x2C);
    hal_TimDelay(20 MS_WAITING);

#else  //!USE_SPI_LCD
    main_Write_COM(0x01);       // Software Reset
    main_Write_COM(0x11);       // Sleep Out
    hal_TimDelay(120 MS_WAITING);               // Must Delay At Least 100 ms

    main_Write_COM(0xC0);       // Power Control 1
    main_Write_DATA(0xF0);      // GVDD Setting//0xF1//调节亮度，值越大越亮，颜色就越淡，//合进去的时候以前的代码这个值是多少现在就改多少
    main_Write_DATA(0x10);      // GVCL Setting//0x11//以上面同步//合进去的时候以前的代码这个值是多少现在就改多少

    main_Write_COM(0xC1);     //set VGH&VGL driver voltage
    main_Write_DATA(0x09);//ff

    main_Write_COM(0xC2);
    main_Write_DATA(0x05);

    main_Write_COM(0xB4);
    main_Write_DATA(0x02);

    main_Write_COM(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    main_Write_DATA(0x0B); //31

    main_Write_COM(0xF8);
    main_Write_DATA(0x01);

    main_Write_COM(0x3A);       //设置色位;
    main_Write_DATA(0x05);
    //************* Start Gamma Setting **********//

    main_Write_COM(0xE0);       // Gamma Command
    main_Write_DATA(0x00);
    main_Write_DATA(0x01);
    main_Write_DATA(0x05);
    main_Write_DATA(0x25);
    main_Write_DATA(0x22);
    main_Write_DATA(0x1F);
    main_Write_DATA(0x03);
    main_Write_DATA(0x0d);
    main_Write_DATA(0x07);
    main_Write_DATA(0x00);
    main_Write_DATA(0x0A);
    main_Write_DATA(0x09);
    main_Write_DATA(0x06);
    main_Write_DATA(0x0B);
    main_Write_DATA(0x03);
    main_Write_DATA(0x07);
    main_Write_COM(0xE1);       // Gamma Command
    main_Write_DATA(0x00);
    main_Write_DATA(0x01);
    main_Write_DATA(0x05);
    main_Write_DATA(0x25);
    main_Write_DATA(0x22);
    main_Write_DATA(0x1F);
    main_Write_DATA(0x03);
    main_Write_DATA(0x0d);
    main_Write_DATA(0x07);
    main_Write_DATA(0x00);
    main_Write_DATA(0x0A);
    main_Write_DATA(0x09);
    main_Write_DATA(0x06);
    main_Write_DATA(0x0B);
    main_Write_DATA(0x03);
    main_Write_DATA(0x07);

    main_Write_COM(0x36);       // Memory Data Access Control//
    main_Write_DATA(0xC8);      // RGB Color Filter Setting

    main_Write_COM(0xB1);       //set gamma ver
    main_Write_DATA(0x01);
    main_Write_DATA(0x04);
    main_Write_DATA(0x18);

    main_Write_COM(0xF4);
    main_Write_DATA(0x00);
    main_Write_DATA(0x08);

    main_Write_COM(0xD9);
    main_Write_DATA(0x00);

    main_Write_COM(0xC7);
    main_Write_DATA(0x19);//调节fincker

    main_Write_COM(0x2A);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x7F);

    main_Write_COM(0x2B);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x00);
    main_Write_DATA(0x9F);

    main_Write_COM(0x29);       // Display Ondelay(20);
    hal_TimDelay(20 MS_WAITING);
    main_Write_COM(0x2C);
    hal_TimDelay(20 MS_WAITING);
#endif  //USE_SPI_LCD

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

    LCM_WR_CMD(0x10);
    hal_TimDelay(120 MS_WAITING);

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
    LCM_WR_REG(0x0010, 0x0A00); // Exit Sleep/ Standby mode
    LCM_WR_REG(0x0011, 0x1038); // Set APON,PON,AON,VCI1EN,VC
    sxr_Sleep(50 MS_WAITING);
    LCM_WR_REG(0x0007, 0x1017); // Set D1=0, D0=1
#else
    main_Write_COM(0x11); //Exit Sleep
    hal_TimDelay(120 MS_WAITING);
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

        LCM_WR_CMD(0x2a);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(x);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(x);

        LCM_WR_CMD(0x2b);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(y);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(y);

        LCM_WR_CMD(0x2c);       // vertical RAM address position
#ifdef USE_SPI_LCD
        LCM_WR_DATA(pixelData);
#else

        LCM_WR_DAT(pixelData);
#endif
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

#if 0
    if(g_lcddRotate)
    {
        //Window Horizontal RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,0,0x0037,pActiveWin->tlPY);
        //Window Horizontal RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,2,0x0036,pActiveWin->brPY);
        //Window Vertical RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,4,0x0039,LCDD_DISP_Y-1-pActiveWin->brPX);
        //Window Vertical RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,6,0x0038,LCDD_DISP_Y-1-pActiveWin->tlPX);

        //Start point
        LCDD_BUILD_CMD_WR_REG(cmd,8,0x0020,pActiveWin->tlPY);
        LCDD_BUILD_CMD_WR_REG(cmd,10,0x0021,LCDD_DISP_Y-1-pActiveWin->tlPX);
    }
    else
    {
        //Window Horizontal RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,0,0x0037,LCDD_DISP_X-1-pActiveWin->brPX);
        //Window Horizontal RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,2,0x0036,LCDD_DISP_X-1-pActiveWin->tlPX);
        //Window Vertical RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,4,0x0039,LCDD_DISP_Y-1-pActiveWin->brPY);
        //Window Vertical RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,6,0x0038,LCDD_DISP_Y-1-pActiveWin->tlPY);

        //Start point
        LCDD_BUILD_CMD_WR_REG(cmd,8,0x0020,LCDD_DISP_X-1-pActiveWin->tlPX);
        LCDD_BUILD_CMD_WR_REG(cmd,10,0x0021,LCDD_DISP_Y-1-pActiveWin->tlPY);


    }

    // Send command after which the data we sent
    // are recognized as pixels.
    LCDD_BUILD_CMD_WR_CMD(cmd,12,0x0022);
    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 13, cmd, lcddp_GoudaBlitHandler));

#endif

    if(g_lcddRotate)
    {
        /*
                   LCM_WR_CMD(0x2A);     //Set Column Address
                LCM_WR_DAT(0x00);
                LCM_WR_DAT(pActiveWin->tlPY);
                LCM_WR_DAT(0x00);
                LCM_WR_DAT(pActiveWin->brPY);

                LCM_WR_CMD(0x2B);    //Set Page Address
                LCM_WR_DAT(0x00);
                LCM_WR_DAT(LCDD_DISP_Y-1- pActiveWin->brPX);
                LCM_WR_DAT(0x00);
                LCM_WR_DAT(LCDD_DISP_Y-1- pActiveWin->tlPX);
        */

        LCM_WR_CMD(0x2A);     //Set Column Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPX);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPX);

        LCM_WR_CMD(0x2B);    //Set Page Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPY);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPY);

    }
    else
    {

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers

        LCM_WR_CMD(0x2A);     //Set Column Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPX);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPX);

        LCM_WR_CMD(0x2B);    //Set Page Address
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->tlPY);
        LCM_WR_DAT(0x00);
        LCM_WR_DAT(pActiveWin->brPY);

    }

    // Send command after which the data we sent
    // are recognized as pixels.
    LCM_WR_CMD(0x2c); //WRITE ram Data display

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


    LCM_WR_CMD(0x36);    //Set Scanning Direction
    LCM_WR_DAT(0xA8);

    hal_TimDelay(1 MS_WAITING);

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

    main_Write_COM(0x36); //Set Scanning Direction
    main_Write_DATA(0xC8);  //0xC8   //0xc0
//LCM_WR_REG(0x36,0xC8);
//        LCM_WR_CMD(0x36);  //Set Scanning Direction
//        LCM_WR_DAT(0xC0);

    hal_TimDelay(1 MS_WAITING);

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

//ZLM MODIFIED
    // hal_GoudaReadReg(0xda, &productId);
    //   hal_GoudaReadReg(0xd3, &productId);
    //hal_HstSendEvent(productId);
    hal_GoudaReadReg(0xd3, &productId);
    hal_GoudaClose();

    //zlm
    hal_HstSendEvent(0x01cdf6810);
    hal_HstSendEvent(productId);
    //zlm

    SXS_TRACE(TSTDOUT, "RM68110(0x%x): lcd read id is 0x%x ", LCD_RM68110_ID, productId);

    if(productId == LCD_RM68110_ID)
        return TRUE;
    else
        return FALSE;
}
PRIVATE char* lcdd_get_id_string(void)
{
    static char RM68110_id_str[] = "RM68110\n";
    return RM68110_id_str;
}
// ============================================================================
// lcdd_RM68110_RegInitrm68110
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL  lcdd_rm68110_RegInit(LCDD_REG_T *pLcdDrv)
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



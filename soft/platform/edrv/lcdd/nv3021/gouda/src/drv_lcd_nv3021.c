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

PRIVATE LCDD_ERR_T NV3021_Sleep(VOID);
PRIVATE LCDD_ERR_T NV3021_WakeUp(VOID);

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================

#define   LCD_DataWrite_NV3021_8( Data )    \
                                            \
    while(hal_GoudaWriteData(Data)  != HAL_ERR_NO);

#define   LCD_CtrlWrite_NV3021_8( Cmd )     \
                                            \
    while(hal_GoudaWriteCmd(Cmd)    != HAL_ERR_NO);



#define   LCD_CtrlData_NV3021_8( Cmd, Data)              \
{                                                        \
    LCD_CtrlWrite_NV3021_8(Cmd);                          \
    LCD_DataWrite_NV3021_8(Data);                         \
}


//low 8bit

#define  LCD_DataWrite_NV3021_16( Data)                       \
{                                                              \
        LCD_DataWrite_NV3021_8 (   ((Data & 0xFF00) >> 8) );   \
        LCD_DataWrite_NV3021_8 (   (Data & 0x00FF) );           \
}

#define  LCD_CtrlWrite_NV3021_16( Cmd)                          \
{                                                               \
        LCD_CtrlWrite_NV3021_8 (  ((Cmd & 0xFF00) >> 8) );       \
        LCD_CtrlWrite_NV3021_8 (  (Cmd & 0x00FF) );               \
}


#define   LCD_CtrlData_NV3021_16( Cmd, Data)                \
{                                                           \
    LCD_CtrlWrite_NV3021_16(Cmd);                           \
    LCD_DataWrite_NV3021_16(Data);                          \
}


#define LCD_Delayms(x)  sxr_Sleep(x)


#define LCD_Reset() hal_GoudaInitResetPin(VOID)



#define NV3021_BUILD_CMD_WR_CMD(c,i,r) do{c[i].isData=FALSE; c[i].value=r;}while(0)
#define NV3021_BUILD_CMD_WR_DAT(c,i,d) do{c[i].isData=TRUE; c[i].value=d;}while(0)
#define NV3021_BUILD_CMD_WR_REG(c,i,r,d) do{NV3021_BUILD_CMD_WR_CMD(c,i,r); NV3021_BUILD_CMD_WR_DAT(c,i+1,d);}while(0)

#define NV3021_TIME_MUTEX_RETRY 50

// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width
// Number of pixels in the display height

#define NV3021_LCD_WIDTH        128
#define NV3021_LCD_HEIGHT       160




// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// LCDD screen config.
PRIVATE CONST LCDD_CONFIG_T g_tgtLcddCfg = TGT_LCDD_CONFIG;

// true  when video player working with full screen
PRIVATE BOOL g_lcddRotate = FALSE;

// wheter NV3021_GoudaBlitHandler() has to close ovl layer 0
PRIVATE BOOL g_lcddAutoCloseLayer = FALSE;

// Sleep status of the LCD( TRUE :sleep  FALSE: normal )
PRIVATE BOOL g_lcddInSleep = FALSE;

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// NV3021_GoudaBlitHandler
// -----------------------------------------------------------------------------
/// This function frees the lock to access the screen. It is set as the user
/// handler called by the DMA driver at the end of the writings on the screen.
// =============================================================================
PRIVATE VOID NV3021_GoudaBlitHandler(VOID)
{
    lcdd_MutexFree();
    if(g_lcddAutoCloseLayer)
    {
        hal_GoudaOvlLayerClose(HAL_GOUDA_OVL_LAYER_ID0);
        g_lcddAutoCloseLayer = FALSE;
    }
}


// =============================================================================
// NV3021_Init
// -----------------------------------------------------------------------------
/// This function initializes LCD registers after powering on or waking up.
// =============================================================================
PRIVATE VOID NV3021_Init(VOID)
{

#if defined(LCD_NV3021_TYF_CMO)

//CTC
// 天亿富 深超  128 160 1.77 for
//兼容 奇美玻璃


    // VCI=2.8V
    LCD_Delayms(10 MS_WAITING); // Del ay 50 ms
    LCD_CtrlWrite_NV3021_8(0xb4);
    LCD_DataWrite_NV3021_8(0x07);

    LCD_CtrlWrite_NV3021_8(0xb1);
    LCD_DataWrite_NV3021_8(0x14);
    //--------------------INTER REG_EN----------------------------
    LCD_CtrlWrite_NV3021_8(0xff );
    LCD_DataWrite_NV3021_8(0xa5);
    LCD_CtrlWrite_NV3021_8(0xfd);
    LCD_DataWrite_NV3021_8(0x86);
    LCD_CtrlWrite_NV3021_8(0xed);
    LCD_DataWrite_NV3021_8(0x25);
    LCD_CtrlWrite_NV3021_8(0xee);
    LCD_DataWrite_NV3021_8(0x22);
    LCD_CtrlWrite_NV3021_8(0xf6);
    LCD_DataWrite_NV3021_8(0x10);
    LCD_CtrlWrite_NV3021_8(0xc4);
    LCD_DataWrite_NV3021_8(0x0d);
    LCD_CtrlWrite_NV3021_8(0xc5);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xc6);
    LCD_DataWrite_NV3021_8(0x0e);
    LCD_CtrlWrite_NV3021_8(0xe0);
    LCD_DataWrite_NV3021_8(0x70);
    LCD_CtrlWrite_NV3021_8(0xe8);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xe1);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xe7);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xe2);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xe6);
    LCD_DataWrite_NV3021_8(0x70);
    LCD_CtrlWrite_NV3021_8(0xe3);
    LCD_DataWrite_NV3021_8(0x14); //
    LCD_CtrlWrite_NV3021_8(0xe9);
    LCD_DataWrite_NV3021_8(0x15); //
    LCD_CtrlWrite_NV3021_8(0xe4);
    LCD_DataWrite_NV3021_8(0x02); //
    LCD_CtrlWrite_NV3021_8(0xeb);
    LCD_DataWrite_NV3021_8(0x02); //
    LCD_CtrlWrite_NV3021_8(0xe5);
    LCD_DataWrite_NV3021_8(0x01); //
    LCD_CtrlWrite_NV3021_8(0xea);
    LCD_DataWrite_NV3021_8(0x01); //
    LCD_CtrlWrite_NV3021_8(0x11);
    LCD_Delayms(100 MS_WAITING);
    LCD_CtrlWrite_NV3021_8(0x3a);
    LCD_DataWrite_NV3021_8(0x05);
    LCD_CtrlWrite_NV3021_8(0x36);
    LCD_DataWrite_NV3021_8(0xc8);
    LCD_Delayms(100 MS_WAITING);
    LCD_CtrlWrite_NV3021_8(0x29);
    LCD_Delayms(100 MS_WAITING);
#elif defined(LCD_NV3021_SL_BOE)
    //VCI=2.8V
    //----------------Star Initial Sequence-------//

    LCD_CtrlWrite_NV3021_8(0xb4);
    LCD_DataWrite_NV3021_8(0x07);  //d2=1 frame inversion
    LCD_CtrlWrite_NV3021_8(0xb1);
    LCD_DataWrite_NV3021_8(0x14);  //frame rate
    //--------------------INTER REG_EN----------------------------
    LCD_CtrlWrite_NV3021_8(0xff);
    LCD_DataWrite_NV3021_8(0xa5);
    LCD_CtrlWrite_NV3021_8(0xfd);
    LCD_DataWrite_NV3021_8(0x86);

    LCD_CtrlWrite_NV3021_8(0xec);
    LCD_DataWrite_NV3021_8(0x89);//80--VGL-6V;88--VGL-5.6V;89--VGL-9V;8a--VGL-12V
    LCD_CtrlWrite_NV3021_8(0xed);
    LCD_DataWrite_NV3021_8(0x25);
    LCD_CtrlWrite_NV3021_8(0xee);
    LCD_DataWrite_NV3021_8(0x22);
    LCD_CtrlWrite_NV3021_8(0xf6);
    LCD_DataWrite_NV3021_8(0x10);
    LCD_CtrlWrite_NV3021_8(0xc4);
    LCD_DataWrite_NV3021_8(0x0c); //VRH 0d
    LCD_CtrlWrite_NV3021_8(0xc5);
    LCD_DataWrite_NV3021_8(0x15); //vcom
    LCD_CtrlWrite_NV3021_8(0xc6);
    LCD_DataWrite_NV3021_8(0x0e); //vdv

    LCD_CtrlWrite_NV3021_8(0xe0);
    LCD_DataWrite_NV3021_8(0x00); //gamma
    LCD_CtrlWrite_NV3021_8(0xe8);
    LCD_DataWrite_NV3021_8(0x77);
    LCD_CtrlWrite_NV3021_8(0xe1);
    LCD_DataWrite_NV3021_8(0x57);
    LCD_CtrlWrite_NV3021_8(0xe7);
    LCD_DataWrite_NV3021_8(0x02);
    LCD_CtrlWrite_NV3021_8(0xe2);
    LCD_DataWrite_NV3021_8(0x01);
    LCD_CtrlWrite_NV3021_8(0xe6);
    LCD_DataWrite_NV3021_8(0x46);
    LCD_CtrlWrite_NV3021_8(0xe3);
    LCD_DataWrite_NV3021_8(0x67);
    LCD_CtrlWrite_NV3021_8(0xe9);
    LCD_DataWrite_NV3021_8(0x10);
    LCD_CtrlWrite_NV3021_8(0xe4);
    LCD_DataWrite_NV3021_8(0x02);
    LCD_CtrlWrite_NV3021_8(0xeb);
    LCD_DataWrite_NV3021_8(0x02);
    LCD_CtrlWrite_NV3021_8(0xe5);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xea);
    LCD_DataWrite_NV3021_8(0x00);

    LCD_CtrlWrite_NV3021_8(0x11);
    LCD_Delayms(120 MS_WAITING);
    LCD_CtrlWrite_NV3021_8(0x3a);
    LCD_DataWrite_NV3021_8(0x05);
    LCD_CtrlWrite_NV3021_8(0x36);
    LCD_DataWrite_NV3021_8(0x80);//80
    LCD_CtrlWrite_NV3021_8(0x2a);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_DataWrite_NV3021_8(0x02);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_DataWrite_NV3021_8(0x81);
    LCD_CtrlWrite_NV3021_8(0x2b);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_DataWrite_NV3021_8(0x01);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_DataWrite_NV3021_8(0xA0);
    LCD_CtrlWrite_NV3021_8(0x29);
    LCD_CtrlWrite_NV3021_8(0x2C);
#elif defined(LCD_NV3021_SL_TM)
    LCD_CtrlWrite_NV3021_8(0xb4);
    LCD_DataWrite_NV3021_8(0x07); //frame Inversion
    // LCD_DataWrite_NV3021_8(0x00); //line Inversion

    LCD_CtrlWrite_NV3021_8(0xb1);
    LCD_DataWrite_NV3021_8(0x18); //120hz
    LCD_CtrlWrite_NV3021_8(0xff);
    LCD_DataWrite_NV3021_8(0xa5); //inter_en
    LCD_CtrlWrite_NV3021_8(0xfd);
    LCD_DataWrite_NV3021_8(0x86);
    LCD_CtrlWrite_NV3021_8(0xec);
    LCD_DataWrite_NV3021_8(0x89);
    LCD_CtrlWrite_NV3021_8(0xed);
    LCD_DataWrite_NV3021_8(0x25); //chp
    LCD_CtrlWrite_NV3021_8(0xee);
    LCD_DataWrite_NV3021_8(0x22); //chp
    LCD_CtrlWrite_NV3021_8(0xf6);
    LCD_DataWrite_NV3021_8(0x10);
    LCD_CtrlWrite_NV3021_8(0xc4);
    LCD_DataWrite_NV3021_8(0x12); //vrh
    LCD_CtrlWrite_NV3021_8(0xc5);
    LCD_DataWrite_NV3021_8(0x22); //vcm
    LCD_CtrlWrite_NV3021_8(0xc6);
    LCD_DataWrite_NV3021_8(0x12); //vdv

    LCD_CtrlWrite_NV3021_8(0xe2);
    LCD_DataWrite_NV3021_8(0x07); //kp5 kp4
    LCD_CtrlWrite_NV3021_8(0xe6);
    LCD_DataWrite_NV3021_8(0x70); //kn1 kn0
    LCD_CtrlWrite_NV3021_8(0xe3);
    LCD_DataWrite_NV3021_8(0x30); //rp1 rp0
    LCD_CtrlWrite_NV3021_8(0xe9);
    LCD_DataWrite_NV3021_8(0x03); // rn1 rn0
    LCD_CtrlWrite_NV3021_8(0xe4);
    LCD_DataWrite_NV3021_8(0x00); // vrp0
    LCD_CtrlWrite_NV3021_8(0xeb);
    LCD_DataWrite_NV3021_8(0x00); // vrn1
    LCD_CtrlWrite_NV3021_8(0xe5);
    LCD_DataWrite_NV3021_8(0x06); //vrp1
    LCD_CtrlWrite_NV3021_8(0xea);
    LCD_DataWrite_NV3021_8(0x03); // vrn0
    LCD_CtrlWrite_NV3021_8(0xe1);
    LCD_DataWrite_NV3021_8(0x02); //kp3 kp2
    LCD_CtrlWrite_NV3021_8(0xe7);
    LCD_DataWrite_NV3021_8(0x20); //kn3 kn2
    LCD_CtrlWrite_NV3021_8(0xe0);
    LCD_DataWrite_NV3021_8(0x02); //kp1 kp0
    LCD_CtrlWrite_NV3021_8(0xe8);
    LCD_DataWrite_NV3021_8(0x20); //kn5 kn4
    LCD_CtrlWrite_NV3021_8(0x11); //sleep out
    LCD_Delayms(120 MS_WAITING); // Del ay 50 ms
    LCD_CtrlWrite_NV3021_8(0x3a);
    LCD_DataWrite_NV3021_8(0x05);

    LCD_CtrlWrite_NV3021_8(0x36);
    LCD_DataWrite_NV3021_8(0x9c);// ss gs bgr
    LCD_Delayms(50 MS_WAITING); // Del ay 50 ms
    LCD_CtrlWrite_NV3021_8(0x29);//display on
#else
    // VCI=2.8V
    LCD_Delayms(10 MS_WAITING); // Del ay 50 ms
    LCD_CtrlWrite_NV3021_8(0xb4);
    LCD_DataWrite_NV3021_8(0x07);

    LCD_CtrlWrite_NV3021_8(0xb1);
    LCD_DataWrite_NV3021_8(0x14);
    //--------------------INTER REG_EN----------------------------
    LCD_CtrlWrite_NV3021_8(0xff );
    LCD_DataWrite_NV3021_8(0xa5);
    LCD_CtrlWrite_NV3021_8(0xfd);
    LCD_DataWrite_NV3021_8(0x86);
    LCD_CtrlWrite_NV3021_8(0xed);
    LCD_DataWrite_NV3021_8(0x25);
    LCD_CtrlWrite_NV3021_8(0xee);
    LCD_DataWrite_NV3021_8(0x22);
    LCD_CtrlWrite_NV3021_8(0xf6);
    LCD_DataWrite_NV3021_8(0x10);
    LCD_CtrlWrite_NV3021_8(0xc4);
    LCD_DataWrite_NV3021_8(0x0d);
    LCD_CtrlWrite_NV3021_8(0xc5);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xc6);
    LCD_DataWrite_NV3021_8(0x0e);
    LCD_CtrlWrite_NV3021_8(0xe0);
    LCD_DataWrite_NV3021_8(0x70);
    LCD_CtrlWrite_NV3021_8(0xe8);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xe1);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xe7);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xe2);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xe6);
    LCD_DataWrite_NV3021_8(0x70);
    LCD_CtrlWrite_NV3021_8(0xe3);
    LCD_DataWrite_NV3021_8(0x14); //
    LCD_CtrlWrite_NV3021_8(0xe9);
    LCD_DataWrite_NV3021_8(0x15); //
    LCD_CtrlWrite_NV3021_8(0xe4);
    LCD_DataWrite_NV3021_8(0x02); //
    LCD_CtrlWrite_NV3021_8(0xeb);
    LCD_DataWrite_NV3021_8(0x02); //
    LCD_CtrlWrite_NV3021_8(0xe5);
    LCD_DataWrite_NV3021_8(0x01); //
    LCD_CtrlWrite_NV3021_8(0xea);
    LCD_DataWrite_NV3021_8(0x01); //
    LCD_CtrlWrite_NV3021_8(0x11);
    LCD_Delayms(100 MS_WAITING);
    LCD_CtrlWrite_NV3021_8(0x3a);
    LCD_DataWrite_NV3021_8(0x05);
    LCD_CtrlWrite_NV3021_8(0x36);
    LCD_DataWrite_NV3021_8(0xc0);
    LCD_Delayms(100 MS_WAITING);
    LCD_CtrlWrite_NV3021_8(0x29);
    LCD_Delayms(100 MS_WAITING);
#endif

}


// ============================================================================
// NV3021_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
//   include reset options
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_Open(VOID)
{
#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

    // Init code
    LCD_Delayms(50 MS_WAITING);                         // Delay 50 ms

    NV3021_Init();

    g_lcddInSleep = FALSE;

    return LCDD_ERR_NO;
}


// ============================================================================
// NV3021_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to
/// #NV3021_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_Close(VOID)
{

    hal_GoudaClose();

    return LCDD_ERR_NO;
}


// ============================================================================
// NV3021_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_SetContrast(UINT32 contrast)
{
    //#warning This function is not implemented yet
    return LCDD_ERR_NO;
}





// ============================================================================
// NV3021_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================

PRIVATE LCDD_ERR_T NV3021_Sleep(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(NV3021_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Sleep while another LCD operation in progress. Sleep %d ticks",
                   NV3021_TIME_MUTEX_RETRY);
    }

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }


//enter sleep mode code

    LCD_CtrlWrite_NV3021_8(0x28);   // Display Off
    LCD_Delayms(50 MS_WAITING);

    LCD_CtrlWrite_NV3021_8(0x10);  //power off line setting
    LCD_Delayms(100 MS_WAITING);   //delay for clear screen

//end

    LCDD_TRACE(TSTDOUT, 0, "NV3021_Sleep: calling hal_GoudaClose");
    hal_GoudaClose();

    g_lcddInSleep = TRUE;

    lcdd_MutexFree();

    return LCDD_ERR_NO;
}


// ============================================================================
// NV3021_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_PartialOn(UINT16 vsa, UINT16 vea)
{
    return LCDD_ERR_NO;
}


// ============================================================================
// NV3021_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_PartialOff(VOID)
{
    return LCDD_ERR_NO;
}

// ============================================================================
// NV3021_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_WakeUp(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(NV3021_TIME_MUTEX_RETRY);
        LCDD_TRACE(TSTDOUT, 0, "LCDD: Wakeup while another LCD operation in progress. Sleep %d ticks",
                   NV3021_TIME_MUTEX_RETRY);
    }

    if (!g_lcddInSleep)
    {
        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }

    LCDD_TRACE(TSTDOUT, 0, "NV3021_WakeUp: calling hal_GoudaOpen");
#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ, &g_tgtLcddCfg.config, 0);
#else
    hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
#endif

//wake up start  Init code
    LCD_Delayms(100 MS_WAITING);                         // Delay 50 ms

    LCD_CtrlWrite_NV3021_8(0x11); //Exit Sleep
    LCD_Delayms(120 MS_WAITING);
    NV3021_Init();
//wake up end

    g_lcddInSleep = FALSE;

    lcdd_MutexFree();

    // Set a comfortable background color to avoid screen flash
    LCDD_FBW_T frameBufferWin;
    frameBufferWin.fb.buffer = NULL;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565; //LCDD_COLOR_FORMAT_RGB_565
    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;

    if (g_lcddRotate)
    {
        frameBufferWin.roi.width = NV3021_LCD_HEIGHT;
        frameBufferWin.roi.height = NV3021_LCD_WIDTH;
        frameBufferWin.fb.width = NV3021_LCD_HEIGHT;
        frameBufferWin.fb.height = NV3021_LCD_WIDTH;
    }
    else
    {
        frameBufferWin.roi.width = NV3021_LCD_WIDTH;
        frameBufferWin.roi.height = NV3021_LCD_HEIGHT;
        frameBufferWin.fb.width = NV3021_LCD_WIDTH;
        frameBufferWin.fb.height = NV3021_LCD_HEIGHT;
    }
    lcddp_Blit16(&frameBufferWin,frameBufferWin.roi.x,frameBufferWin.roi.y);

    return LCDD_ERR_NO;
}

// ============================================================================
// NV3021_SetStandbyMode
// ----------------------------------------------------------------------------
/// Set the main LCD in standby mode or exit from it
/// @param standbyMode If \c TRUE, go in standby mode.
///                    If \c FALSE, cancel standby mode.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_SetStandbyMode(BOOL standbyMode)
{
    if (standbyMode)  // enter standby
    {
        NV3021_Sleep();
    }
    else            // exit standby
    {
        NV3021_WakeUp();
    }
    return LCDD_ERR_NO;
}



// ============================================================================
// NV3021_GetScreenInfo
// ----------------------------------------------------------------------------
/// Get information about the main LCD device.
/// @param screenInfo Pointer to the structure where the information
/// obtained will be stored
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo)
{
    {
        screenInfo->width = NV3021_LCD_WIDTH;
        screenInfo->height = NV3021_LCD_HEIGHT;
        screenInfo->bitdepth = LCDD_COLOR_FORMAT_RGB_565;
        screenInfo->nReserved = 0;
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// NV3021_SetPixel16
// ----------------------------------------------------------------------------
/// Draw a 16-bit pixel a the specified position.
/// @param x X coordinate of the point to set.
/// @param y Y coordinate of the point to set.
/// @param pixelData 16-bit pixel data to draw.
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_SetPixel16(UINT16 x, UINT16 y, UINT16 pixelData)
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

        LCD_CtrlWrite_NV3021_8(0x2a);
        LCD_DataWrite_NV3021_8(0x00);   // start column [15:8]
        LCD_DataWrite_NV3021_8(x);      // start column [7:0]
        LCD_DataWrite_NV3021_8(0x00);   // end column [15:8]
        LCD_DataWrite_NV3021_8(x);      // endcolumn [15:8]

        LCD_CtrlWrite_NV3021_8(0x2b);   // start row [15:8]
        LCD_DataWrite_NV3021_8(0x00);   // start row [7:0]
        LCD_DataWrite_NV3021_8(y);      // end row [15:8]
        LCD_DataWrite_NV3021_8(0x00);   // end row [7:0]
        LCD_DataWrite_NV3021_8(y);

        LCD_CtrlWrite_NV3021_8(0x2c);       // memory write
        LCD_DataWrite_NV3021_16(pixelData); //0xffff  fffff

        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcddp_BlitRoi2Win
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD
//          display window (0x2a  0x2b  0x2c)
// ============================================================================
PRIVATE VOID lcddp_BlitRoi2Win(CONST HAL_GOUDA_WINDOW_T* pRoi, CONST HAL_GOUDA_WINDOW_T* pActiveWin)
{
    HAL_GOUDA_LCD_CMD_T cmd[13];

    if(!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        LCDD_TRACE(LCDD_WARN_TRC,0,"lcddp_BlitRoi2Win: Invalid Roi x:%d < %d, y:%d < %d",pRoi->tlPX, pRoi->brPX, pRoi->tlPY, pRoi->brPY);
        NV3021_GoudaBlitHandler();
        return;
    }

    // building set roi sequence:

#if 0
    if(g_lcddRotate)
    {
        //Window Horizontal RAM Address Start
        NV3021_BUILD_CMD_WR_REG(cmd,0,0x0037,pActiveWin->tlPY);
        //Window Horizontal RAM Address End
        NV3021_BUILD_CMD_WR_REG(cmd,2,0x0036,pActiveWin->brPY);
        //Window Vertical RAM Address Start
        NV3021_BUILD_CMD_WR_REG(cmd,4,0x0039,NV3021_LCD_HEIGHT-1-pActiveWin->brPX);
        //Window Vertical RAM Address End
        NV3021_BUILD_CMD_WR_REG(cmd,6,0x0038,NV3021_LCD_HEIGHT-1-pActiveWin->tlPX);

        //Start point
        NV3021_BUILD_CMD_WR_REG(cmd,8,0x0020,pActiveWin->tlPY);
        NV3021_BUILD_CMD_WR_REG(cmd,10,0x0021,NV3021_LCD_HEIGHT-1-pActiveWin->tlPX);
    }
    else
    {
        //Window Horizontal RAM Address Start
        NV3021_BUILD_CMD_WR_REG(cmd,0,0x0037,NV3021_LCD_WIDTH-1-pActiveWin->brPX);
        //Window Horizontal RAM Address End
        NV3021_BUILD_CMD_WR_REG(cmd,2,0x0036,NV3021_LCD_WIDTH-1-pActiveWin->tlPX);
        //Window Vertical RAM Address Start
        NV3021_BUILD_CMD_WR_REG(cmd,4,0x0039,NV3021_LCD_HEIGHT-1-pActiveWin->brPY);
        //Window Vertical RAM Address End
        NV3021_BUILD_CMD_WR_REG(cmd,6,0x0038,NV3021_LCD_HEIGHT-1-pActiveWin->tlPY);

        //Start point
        NV3021_BUILD_CMD_WR_REG(cmd,8,0x0020,NV3021_LCD_WIDTH-1-pActiveWin->tlPX);
        NV3021_BUILD_CMD_WR_REG(cmd,10,0x0021,NV3021_LCD_HEIGHT-1-pActiveWin->tlPY);


    }

    // Send command after which the data we sent
    // are recognized as pixels.
    NV3021_BUILD_CMD_WR_CMD(cmd,12,0x0022);
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

    if(g_lcddRotate)
    {
        /*
                   LCD_CtrlWrite_NV3021_8(0x2A);     //Set Column Address
                LCD_DataWrite_NV3021_8(0x00);
                LCD_DataWrite_NV3021_8(pActiveWin->tlPY);
                LCD_DataWrite_NV3021_8(0x00);
                LCD_DataWrite_NV3021_8(pActiveWin->brPY);

                LCD_CtrlWrite_NV3021_8(0x2B);    //Set Page Address
                LCD_DataWrite_NV3021_8(0x00);
                LCD_DataWrite_NV3021_8(NV3021_LCD_HEIGHT-1- pActiveWin->brPX);
                LCD_DataWrite_NV3021_8(0x00);
                LCD_DataWrite_NV3021_8(NV3021_LCD_HEIGHT-1- pActiveWin->tlPX);
        */

        LCD_CtrlWrite_NV3021_8(0x2A);    //Set Column Address
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->tlPX);
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->brPX);

        LCD_CtrlWrite_NV3021_8(0x2B);    //Set Page Address
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->tlPY);
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->brPY);

    }
    else
    {

        // this will allow to keep LCDD interface for blit while using gouda
        // directly for configuring layers

        LCD_CtrlWrite_NV3021_8(0x2A);    //Set Column Address
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->tlPX);
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->brPX);

        LCD_CtrlWrite_NV3021_8(0x2B);    //Set Page Address
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->tlPY);
        LCD_DataWrite_NV3021_8(0x00);
        LCD_DataWrite_NV3021_8(pActiveWin->brPY);

    }

    // Send command after which the data we sent
    // are recognized as pixels.
    LCD_CtrlWrite_NV3021_8(0x2c); //WRITE ram Data display

    // Send command after which the data we sent
    // are recognized as pixels.
    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 0, cmd, NV3021_GoudaBlitHandler));
}


// ============================================================================
// NV3021_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of the active window  with some color.
/// @param bgColor Color with which to fill the rectangle. It is a 16-bit
/// data, as the one of #NV3021_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T NV3021_FillRect16(CONST LCDD_ROI_T* regionOfInterrest, UINT16 bgColor)
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
            if (    (activeWin.tlPX >= NV3021_LCD_HEIGHT) ||
                    (activeWin.brPX >= NV3021_LCD_HEIGHT) ||
                    (activeWin.tlPY >= NV3021_LCD_WIDTH) ||
                    (activeWin.brPY >= NV3021_LCD_WIDTH)
               )
            {
                badParam = TRUE;
            }
        }
        else
        {
            if (    (activeWin.tlPX >= NV3021_LCD_WIDTH) ||
                    (activeWin.brPX >= NV3021_LCD_WIDTH) ||
                    (activeWin.tlPY >= NV3021_LCD_HEIGHT) ||
                    (activeWin.brPY >= NV3021_LCD_HEIGHT)
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
            if (    (activeWin.tlPX >= NV3021_LCD_HEIGHT) ||
                    (activeWin.brPX >= NV3021_LCD_HEIGHT) ||
                    (activeWin.tlPY >= NV3021_LCD_WIDTH) ||
                    (activeWin.brPY >= NV3021_LCD_WIDTH)
               )
            {
                badParam = TRUE;
            }
        }
        else
        {
            if (    (activeWin.tlPX >= NV3021_LCD_WIDTH) ||
                    (activeWin.brPX >= NV3021_LCD_WIDTH) ||
                    (activeWin.tlPY >= NV3021_LCD_HEIGHT) ||
                    (activeWin.brPY >= NV3021_LCD_HEIGHT)
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
// NV3021_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented for the ebc version of the driver
// ============================================================================
PRIVATE BOOL NV3021_Busy(VOID)
{
    return FALSE;
}


// ============================================================================
// NV3021_SetDirRotation
// ----------------------------------------------------------------------------
///
// ============================================================================

#if defined(LCD_NV3021_TYF_CMO)||defined(LCD_NV3021_SL_TM)
PRIVATE UINT16 u_RGB =0x08 ;
#else
PRIVATE UINT16 u_RGB =0x00 ;
#endif



PRIVATE BOOL NV3021_SetDirRotation(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(NV3021_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = TRUE;

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return TRUE;
    }


    LCD_CtrlWrite_NV3021_8(0x36);    //Set Scanning Direction
#if defined(LCD_NV3021_SL_TM)
    LCD_DataWrite_NV3021_8(0x94 |u_RGB);    //0x40
#else
    LCD_DataWrite_NV3021_8(0xe0 |u_RGB);    //0x40
#endif
    LCD_Delayms(10 MS_WAITING);



    lcdd_MutexFree();

    return TRUE;
}

// ============================================================================
// NV3021_SetDirection
// ----------------------------------------------------------------------------
///         MY    MX   MV  ML     RGB
// ============================================================================
PRIVATE BOOL NV3021_SetDirection(VOID)
{
    while (0 == lcdd_MutexGet())
    {
        LCD_Delayms(NV3021_TIME_MUTEX_RETRY);
    }

    g_lcddRotate = FALSE;

    if (g_lcddInSleep)
    {
        lcdd_MutexFree();
        return TRUE;
    }


    //normal
    LCD_CtrlWrite_NV3021_8(0x36); //Set Scanning Direction
#if defined(LCD_NV3021_SL_TM)
    LCD_DataWrite_NV3021_8(0x94 |u_RGB);    //0x40
#else
    LCD_DataWrite_NV3021_8(0xC0 |u_RGB);    //0x40
#endif
    //LCD_CtrlData_NV3021_16(0x36,0xC8);
    //LCD_CtrlWrite_NV3021_8(0x36);  //Set Scanning Direction
    //LCD_DataWrite_NV3021_8(0xC0);

    LCD_Delayms(10 MS_WAITING);

    lcdd_MutexFree();

    return TRUE;
}



/*
**  nv3021   readID()
**  read(0x04) << 0x31 0x31  0x30  0x21
**  read(0x04) << 0x31 0x31 0x80  0X66
**  read(0x04) << 0xd4 0xd4  0x80  0X66

**   HT2018
**  read(0x04) << 0xxx  0xd4  0x80 0x66
*/



//       NV3021  ID
#ifdef USE_SPI_LCD
#define  LCD_NV3021_ID         0x3031
#else
#define  LCD_NV3021_ID         0xd480
#endif


//extern char *HX_lcd_string;  //must be end with \n

PRIVATE BOOL NV3021_ReadID()
{

    UINT16 productId=0;

    UINT8 productIds[4];
    LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;

#ifdef USE_SPI_LCD
    hal_GoudaSerialOpen(LCDD_SPI_LINE_TYPE, LCDD_SPI_FREQ_READ, &lcddReadConfig.config, 0);
#else
    hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
#endif

    LCD_Delayms(20 MS_WAITING);



    hal_GoudaReadData(0x04,productIds,4);

    productId= ((productIds[1]&0xff) << 8)  |(productIds[2]&0xff);



    hal_HstSendEvent(0x11003021);
    hal_HstSendEvent(0x1cd001cd);
    hal_HstSendEvent(productId); //0x3021   0x6088      8bit
    hal_HstSendEvent(productIds[1]);
    hal_HstSendEvent(productIds[2]);
    hal_HstSendEvent(productIds[3]);


    hal_GoudaClose();

    SXS_TRACE(TSTDOUT, "NV3021(0x%x): lcd read id is 0x%x ", LCD_NV3021_ID, productId);


#ifdef USE_SPI_LCD
    //SPI  lcd
    if (productId == LCD_NV3021_ID)
    {
        //HX_lcd_string = "NV3021\n";
        return TRUE;
    }
    else

        return FALSE;

#else
    //8bit tft
    if (productId == LCD_NV3021_ID)
    {

        //HX_lcd_string = "NV3021\n";
        return TRUE;
    }
    else

        return FALSE;
#endif


}




//nv3021_string
PRIVATE char* NV3021_get_id_string(void)
{
    static char nv3021_id_str[] = "NV3021\n";

    return nv3021_id_str;
}

#ifdef CHECK_LCD_STATUS
PRIVATE  BOOL lcddp_CheckLCDStatus(VOID)
{
    UINT16 lcdStatus=0;
    unsigned char lcdStatus1[4];

    hal_GoudaReadData(0x09,lcdStatus1,4);

    lcdStatus=((((lcdStatus1[2])&0xff)<<8)|((lcdStatus1[3])&0xff));
#if 1
    hal_HstSendEvent(0x77778888);
    hal_HstSendEvent(lcdStatus);

#endif
    if((lcdStatus&0xFFFF) == 0x5304 || (lcdStatus&0xFFFF) == 0x0000)
        return TRUE;
    else
        return FALSE;
}

PRIVATE LCDD_ERR_T lcddp_Reset_InIdle(VOID)
{

    // Init code
    hal_SysWaitMicrosecond(100);                         // Delay 50 ms

    LCD_CtrlWrite_NV3021_8(0xb4);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xb1);
    LCD_DataWrite_NV3021_8(0x1c);
    //--------------------INTER REG_EN----------------------------

    LCD_CtrlWrite_NV3021_8(0xff);
    LCD_DataWrite_NV3021_8(0xa5);

    LCD_CtrlWrite_NV3021_8(0xfd);
    LCD_DataWrite_NV3021_8(0x86);

    LCD_CtrlWrite_NV3021_8(0xec);
    LCD_DataWrite_NV3021_8(0x89);

    LCD_CtrlWrite_NV3021_8(0xed);
    LCD_DataWrite_NV3021_8(0x25);

    LCD_CtrlWrite_NV3021_8(0xee);
    LCD_DataWrite_NV3021_8(0x22);

    LCD_CtrlWrite_NV3021_8(0xf6);
    LCD_DataWrite_NV3021_8(0x10);
    LCD_CtrlWrite_NV3021_8(0xc4);
    LCD_DataWrite_NV3021_8(0x0d);
    LCD_CtrlWrite_NV3021_8(0xc5);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xc6);
    LCD_DataWrite_NV3021_8(0x1f);
    LCD_CtrlWrite_NV3021_8(0xe0);
    LCD_DataWrite_NV3021_8(0x70);
    LCD_CtrlWrite_NV3021_8(0xe8);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xe1);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xe7);
    LCD_DataWrite_NV3021_8(0x00);
    LCD_CtrlWrite_NV3021_8(0xe2);
    LCD_DataWrite_NV3021_8(0x07);
    LCD_CtrlWrite_NV3021_8(0xe6);
    LCD_DataWrite_NV3021_8(0x70);
    LCD_CtrlWrite_NV3021_8(0xe3);
    LCD_DataWrite_NV3021_8(0x14); //
    LCD_CtrlWrite_NV3021_8(0xe9);
    LCD_DataWrite_NV3021_8(0x15); //
    LCD_CtrlWrite_NV3021_8(0xe4);
    LCD_DataWrite_NV3021_8(0x02); //
    LCD_CtrlWrite_NV3021_8(0xeb);
    LCD_DataWrite_NV3021_8(0x02); //
    LCD_CtrlWrite_NV3021_8(0xe5);
    LCD_DataWrite_NV3021_8(0x01); //
    LCD_CtrlWrite_NV3021_8(0xea);
    LCD_DataWrite_NV3021_8(0x01); //
    LCD_CtrlWrite_NV3021_8(0x11);
    hal_SysWaitMicrosecond(50);
    LCD_CtrlWrite_NV3021_8(0x3a);
    LCD_DataWrite_NV3021_8(0x05);
    LCD_CtrlWrite_NV3021_8(0x36);
    LCD_DataWrite_NV3021_8(0x9c);
    hal_SysWaitMicrosecond(200);

    LCD_CtrlWrite_NV3021_8(0x29);




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
        frameBufferWin.roi.width = NV3021_LCD_HEIGHT;
        frameBufferWin.roi.height = NV3021_LCD_WIDTH;
        frameBufferWin.fb.width = NV3021_LCD_HEIGHT;
        frameBufferWin.fb.height = NV3021_LCD_WIDTH;
    }
    else
    {
        frameBufferWin.roi.width = NV3021_LCD_WIDTH;
        frameBufferWin.roi.height = NV3021_LCD_HEIGHT;
        frameBufferWin.fb.width = NV3021_LCD_WIDTH;
        frameBufferWin.fb.height = NV3021_LCD_HEIGHT;
    }
    lcddp_Blit16(&frameBufferWin,frameBufferWin.roi.x,frameBufferWin.roi.y);

}
#endif


// ============================================================================
// lcdd_nv3021_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to NV3021_ReadID
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL  lcdd_nv3021_RegInit(LCDD_REG_T *pLcdDrv)
{
    if( NV3021_ReadID())
    {
        pLcdDrv->lcdd_Open=NV3021_Open;
        pLcdDrv->lcdd_Close=NV3021_Close;
        pLcdDrv->lcdd_SetContrast=NV3021_SetContrast;

        pLcdDrv->lcdd_SetStandbyMode=NV3021_SetStandbyMode;
        pLcdDrv->lcdd_PartialOn=NV3021_PartialOn;
        pLcdDrv->lcdd_PartialOff=NV3021_PartialOff;
        pLcdDrv->lcdd_Blit16=lcddp_Blit16;
        pLcdDrv->lcdd_Busy=NV3021_Busy;
        pLcdDrv->lcdd_FillRect16=NV3021_FillRect16;
        pLcdDrv->lcdd_GetScreenInfo=NV3021_GetScreenInfo;
        pLcdDrv->lcdd_WakeUp=NV3021_WakeUp;
        pLcdDrv->lcdd_SetPixel16=NV3021_SetPixel16;
        pLcdDrv->lcdd_Sleep=NV3021_Sleep;
        pLcdDrv->lcdd_SetDirRotation=NV3021_SetDirRotation;
        pLcdDrv->lcdd_SetDirDefault=NV3021_SetDirection;
        pLcdDrv->lcdd_GetStringId=NV3021_get_id_string;
#ifdef CHECK_LCD_STATUS
        pLcdDrv->lcdd_CheckLCDStatus=lcddp_CheckLCDStatus;
        pLcdDrv->lcdd_Reset_InIdle=lcddp_Reset_InIdle;
#endif
        pLcdDrv->lcdd_GoudaBltHdl = lcddp_GoudaBlitHandler;
        return TRUE;
    }

    return FALSE;
}



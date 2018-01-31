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
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_host.h"
#include "tgt_spilcd_cfg.h"
#include "lcdd_types.h"
// =============================================================================
// global variables
// -----------------------------------------------------------------------------
// =============================================================================

//#define CAMERA_DIRECT_TO_LCD

#define SSLCD_REG_L 5
PRIVATE UINT8 g_sscld_data= 0;
PRIVATE UINT8 g_image[2]= {0};


PRIVATE HAL_APO_ID_T g_slcd_a0;// = {{
//    .type = HAL_GPIO_TYPE_IO,
//    .id = 6
//     }};

PRIVATE HAL_APO_ID_T g_slcd_rst ;//= {{
// .type = HAL_GPIO_TYPE_O,
// .id = 11
//      }};


extern U16          simulator_Merge_buffer[128 * 160];
// =============================================================================
//  MACROS while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0); hal_SpiTxFinished
// =============================================================================
#define MS_WAITING      *  HAL_TICK1S / 1000

#define LCM_WR_REG(Addr, Data)  {  while(hal_GoudaWriteReg(Addr, Data)!= HAL_ERR_NO);}

#define SLCM_WR_CMD(n)         {  \
                                     hal_GpioClr(g_slcd_a0); \
                                     hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,&g_sscld_data,n);\
                                     while(!hal_SpiTxDmaDone(HAL_SPI,HAL_SPI_CS0));\
                                     hal_GpioSet(g_slcd_a0); \
                               }
#define SLCM_WR_DAT(n)        {\
                                  hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,&g_sscld_data,n); \
                                  while(!hal_SpiTxDmaDone(HAL_SPI,HAL_SPI_CS0));\
                             }

#define WriteCommand_Addr(addr)   {\
                                     g_sscld_data = addr;\
                                     hal_GpioClr(g_slcd_a0); \
                                     hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,&g_sscld_data,1);\
                                     while(!hal_SpiTxDmaDone(HAL_SPI,HAL_SPI_CS0));\
                                   }
#define WriteCommand_Data(data)   {                        \
                                     g_sscld_data = data;\
                                     hal_GpioSet(g_slcd_a0); \
                                     hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,&g_sscld_data,1);\
                                     while(!hal_SpiTxDmaDone(HAL_SPI,HAL_SPI_CS0));\
                                   }

#define Write_Data_Image           {                                                      \
                                     hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,g_image,2);\
                                     while(!hal_SpiTxDmaDone(HAL_SPI,HAL_SPI_CS0));\
                                   }

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
#define LCDD_DISP_X             128

// Number of pixels in the display height
#define LCDD_DISP_Y             160

#define  LCD_ILI9163C_ID        0x0054

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
PRIVATE VOID lcddp_SpiDeActivateCs(VOID)
{
    hal_SpiDeActivateCs(HAL_SPI,HAL_SPI_CS0);
}
PRIVATE VOID lcddp_SpiActivateCs(VOID)
{
    BOOL activated;
    activated = hal_SpiActivateCs(HAL_SPI,HAL_SPI_CS0);
    LCDD_ASSERT((activated == TRUE),"slcd spi busy");
}

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
    sxr_Sleep(50 MS_WAITING);
    lcddp_SpiActivateCs();
    //DelayX1ms(150);

    //************* Start Initial Sequence **********//
    WriteCommand_Addr(0x11); //Exit Sleep
    sxr_Sleep(120 MS_WAITING);

    WriteCommand_Addr(0xEC);
    WriteCommand_Data(0x0C);

    WriteCommand_Addr(0x26); //Set Default Gamma
    WriteCommand_Data(0x04);

    WriteCommand_Addr(0xB1);
    WriteCommand_Data(0x0A);
    WriteCommand_Data(0x14);

    WriteCommand_Addr(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    WriteCommand_Data(0x06);
    WriteCommand_Data(0x00);

    sxr_Sleep(50 MS_WAITING);

    WriteCommand_Addr(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    WriteCommand_Data(0x02);

    sxr_Sleep(20 MS_WAITING);

    WriteCommand_Addr(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    WriteCommand_Data(0x3C);
    WriteCommand_Data(0x40);

    sxr_Sleep(20 MS_WAITING);

    WriteCommand_Addr(0xC7);
    WriteCommand_Data(0xC3);

    WriteCommand_Addr(0x3A); //Set Color Format
    WriteCommand_Data(0x05);

    WriteCommand_Addr(0x2A); //Set Column Address
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x7F);

    WriteCommand_Addr(0x2B); //Set Page Address
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x9F);

    WriteCommand_Addr(0x36); //Set Scanning Direction
    WriteCommand_Data(0xC8);

    WriteCommand_Addr(0xF2); //Enable Gamma bit
    WriteCommand_Data(0x01);

    WriteCommand_Addr(0xE0);
    WriteCommand_Data(0x3F);//p1
    WriteCommand_Data(0x18);//p2
    WriteCommand_Data(0x18);//p3
    WriteCommand_Data(0x26);//p4
    WriteCommand_Data(0x20);//p5
    WriteCommand_Data(0x0D);//p6
    WriteCommand_Data(0x46);//p7
    WriteCommand_Data(0xF3);//p8
    WriteCommand_Data(0x32);//p9
    WriteCommand_Data(0x09);//p10
    WriteCommand_Data(0x02);//p11
    WriteCommand_Data(0x02);//p12
    WriteCommand_Data(0x00);//p13
    WriteCommand_Data(0x00);//p14
    WriteCommand_Data(0x00);//p15

    WriteCommand_Addr(0xE1);
    WriteCommand_Data(0x00);//p1
    WriteCommand_Data(0x27);//p2
    WriteCommand_Data(0x27);//p3
    WriteCommand_Data(0x09);//p4
    WriteCommand_Data(0x0F);//p5
    WriteCommand_Data(0x12);//p6
    WriteCommand_Data(0x39);//p7
    WriteCommand_Data(0xC0);//p8
    WriteCommand_Data(0x4D);//p9
    WriteCommand_Data(0x16);//p10
    WriteCommand_Data(0x1D);//p11
    WriteCommand_Data(0x2D);//p12
    WriteCommand_Data(0x3F);//p13
    WriteCommand_Data(0x3F);//p14
    WriteCommand_Data(0x3F);//p15

    WriteCommand_Addr(0x29); // Display On
    sxr_Sleep(120 MS_WAITING);
    WriteCommand_Addr(0x2c); // Display On

}


PRIVATE UINT16 lcddp_Open_Read_LcdId(VOID)
{
    // return 1;
    UINT32 count;
    // UINT8 data[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
    // UINT8 iTempData[6] = {0x9, 0xff,0xff,0xff,0xff,0xff};
    UINT8 data[3] = {0x0,0x0,0x0};
    UINT8 iTempData[3] = {0xda,0xff, 0xff};
    // UINT8 dummy = 0;
    HAL_SPI_CFG_T slcd_spiCfg =
    {
        .enabledCS    = HAL_SPI_CS0,
        .csActiveLow    = TRUE,
        .inputEn        = TRUE,
        .clkFallEdge    = TRUE,
        .clkDelay       = HAL_SPI_HALF_CLK_PERIOD_0,
        .doDelay        = HAL_SPI_HALF_CLK_PERIOD_0,
        .diDelay        = HAL_SPI_HALF_CLK_PERIOD_1,
        .csDelay        = HAL_SPI_HALF_CLK_PERIOD_1,
        .csPulse        = HAL_SPI_HALF_CLK_PERIOD_0,//HAL_SPI_HALF_CLK_PERIOD_1
        .frameSize      = 8,
        .oeRatio        = 8,
        .spiFreq        = 1000000,//5M 5000000
        .rxTrigger      = HAL_SPI_RX_TRIGGER_4_BYTE,
        .txTrigger      = HAL_SPI_TX_TRIGGER_1_EMPTY,
        .rxMode         = HAL_SPI_DIRECT_POLLING,
        .txMode         = HAL_SPI_DIRECT_POLLING,
        .mask           = {0,0,0,0,0},
        .handler        = NULL
    };
    hal_SpiOpen(HAL_SPI,HAL_SPI_CS0, &slcd_spiCfg);
    hal_SpiActivateCs(HAL_SPI,HAL_SPI_CS0);
    hal_GpioClr(g_slcd_a0);
    // hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,&dummy,1);
    //wait until any previous transfers have ended
    // while(!hal_SpiTxFinished(HAL_SPI,HAL_SPI_CS0));hal_GpioSet(g_slcd_a0);
    hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,iTempData,3);
    //wait until any previous transfers have ended
    while(!hal_SpiTxFinished(HAL_SPI,HAL_SPI_CS0));//hal_GpioSet(g_slcd_a0);
    count = hal_SpiGetData(HAL_SPI,HAL_SPI_CS0,data, 3);
    hal_SpiDeActivateCs(HAL_SPI,HAL_SPI_CS0);
    hal_SpiClose(HAL_SPI,HAL_SPI_CS0);
    SXS_TRACE(TSTDOUT, "hx8535d: lcd read id is 0x%x 0x%x 0x%x  ,count=%d",data[0],data[1],data[2],count);
    return (UINT16)data[1];

}

PRIVATE VOID lcddp_Open_Spi(VOID)
{
    HAL_SPI_CFG_T slcd_spiCfg =
    {
        .enabledCS    = HAL_SPI_CS0,
        .csActiveLow    = TRUE,
        .inputEn        = TRUE,
        .clkFallEdge    = TRUE,
        .clkDelay       = HAL_SPI_HALF_CLK_PERIOD_0,
        .doDelay        = HAL_SPI_HALF_CLK_PERIOD_0,
        .diDelay        = HAL_SPI_HALF_CLK_PERIOD_1,
        .csDelay        = HAL_SPI_HALF_CLK_PERIOD_1,
        .csPulse        = HAL_SPI_HALF_CLK_PERIOD_0,//HAL_SPI_HALF_CLK_PERIOD_1
        .frameSize      = 8,
        .oeRatio        = 0,
        .spiFreq        = 20000000,//5M 5000000 15M  20M(33ms-per-frame)  39M(13ms-per-frame)
        .rxTrigger      = HAL_SPI_RX_TRIGGER_4_BYTE,
        .txTrigger      = HAL_SPI_TX_TRIGGER_1_EMPTY,
        .rxMode         = HAL_SPI_DIRECT_POLLING,
        .txMode         = HAL_SPI_DMA_POLLING,
        .mask           = {0,0,0,0,0},
        .handler        = NULL
    };
    hal_SpiOpen(HAL_SPI,HAL_SPI_CS0, &slcd_spiCfg);
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
//    hal_HstSendEvent(0x88855503);
    //hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
    g_slcd_rst = tgt_GetSpiLcdConfig()->lcdResetPin;
    g_slcd_a0 = tgt_GetSpiLcdConfig()->lcdA0Pin;
    pmd_EnablePower(PMD_POWER_LCD,TRUE);
    hal_GpioSet(g_slcd_rst);
    hal_TimDelay(10 MS_WAITING);
    hal_GpioClr(g_slcd_rst);
    hal_TimDelay(10 MS_WAITING);
    hal_GpioSet(g_slcd_rst);
    hal_TimDelay(50 MS_WAITING);


    hal_GpioSetOut(g_slcd_a0.gpioId);
    lcddp_Open_Spi();

    lcddp_Init();
    hal_TimDelay(1 MS_WAITING);
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
    // hal_GoudaClose();
    lcddp_SpiDeActivateCs();
    hal_GpioClr(g_slcd_rst);
    pmd_EnablePower(PMD_POWER_LCD,FALSE);
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
PRIVATE VOID lcddp_Lcd_SetIO_TriState_For_Lowpower(BOOL tri);
extern VOID camerap_PAS_Enter_LowPower(BOOL tri);


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

    // WriteCommand_Addr(0x10); //Set Sleep In
    sxr_Sleep(120 MS_WAITING);
    LCDD_TRACE(TSTDOUT, 0, "lcddp_Sleep: calling hal_GoudaClose");

    g_lcddInSleep = TRUE;
    hal_SpiDeActivateCs(HAL_SPI,HAL_SPI_CS0);

    lcdd_MutexFree();
    hal_HstSendEvent(0x668899);

    lcddp_Lcd_SetIO_TriState_For_Lowpower(TRUE);
    camerap_PAS_Enter_LowPower(TRUE);

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
extern UINT32 lcd_frame_buffer_address;

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
    // hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
    hal_HstSendEvent(0x668877);
#if 0
    hal_HstSendEvent(0x668879);

    lcddp_Lcd_SetIO_TriState_For_Lowpower(FALSE);


    lcddp_SpiActivateCs();
    WriteCommand_Addr(0x11); //Set Sleep Out
    sxr_Sleep(120 MS_WAITING);
#else
    camerap_PAS_Enter_LowPower(FALSE);
    lcddp_Lcd_SetIO_TriState_For_Lowpower(FALSE);

    pmd_EnablePower(PMD_POWER_LCD,TRUE);
    hal_GpioSet(g_slcd_rst);
    hal_TimDelay(10 MS_WAITING);
    hal_GpioClr(g_slcd_rst);
    hal_TimDelay(10 MS_WAITING);
    hal_GpioSet(g_slcd_rst);
    hal_TimDelay(50 MS_WAITING);
    hal_GpioSetOut(g_slcd_a0.gpioId);
    lcddp_Init();
    hal_TimDelay(1 MS_WAITING);

#endif

    g_lcddInSleep = FALSE;

    lcdd_MutexFree();

    // Set a comfortable background color to avoid screen flash
    LCDD_FBW_T frameBufferWin;
    frameBufferWin.fb.buffer = (UINT16 *)(simulator_Merge_buffer);
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

//    sxr_Sleep(16384);

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

        WriteCommand_Addr(0x2a);
        WriteCommand_Data(0x00);
        WriteCommand_Data(x);
        WriteCommand_Data(0x00);
        WriteCommand_Data(x);

        WriteCommand_Addr(0x2b);
        WriteCommand_Data(0x00);
        WriteCommand_Data(y);
        WriteCommand_Data(0x00);
        WriteCommand_Data(y);

        WriteCommand_Addr(0x2c);        // vertical RAM address position
        WriteCommand_Data(pixelData);

        lcdd_MutexFree();
        return LCDD_ERR_NO;
    }
}

// ============================================================================
// lcddp_BlitRoi2Win
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD
// ============================================================================
PRIVATE VOID lcddp_BlitRoi2Win(CONST HAL_GOUDA_WINDOW_T* pRoi, CONST HAL_GOUDA_WINDOW_T* pActiveWin, UINT16 bgColor)
{
//    hal_HstSendEvent(0x88855509);
    HAL_GOUDA_LCD_CMD_T cmd[13];
    UINT32 j = 0,k =0;

    if(!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        LCDD_TRACE(LCDD_WARN_TRC,0,"lcddp_BlitRoi2Win: Invalid Roi x:%d < %d, y:%d < %d",pRoi->tlPX, pRoi->brPX, pRoi->tlPY, pRoi->brPY);
        lcddp_GoudaBlitHandler();
        return;
    }

    // building set roi sequence:

    if(g_lcddRotate)
    {
        //Window Horizontal RAM Address Start
        WriteCommand_Addr(0x2a);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->tlPX);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->brPX);

        WriteCommand_Addr(0x2b);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->tlPY);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->brPY);

    }
    else
    {
        //Window Horizontal RAM Address Start

        WriteCommand_Addr(0x2a);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->tlPX);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->brPX);

        WriteCommand_Addr(0x2b);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->tlPY);
        WriteCommand_Data(0x00);
        WriteCommand_Data(pActiveWin->brPY);

        //Start point
    }
    k = ((pActiveWin->brPX-pActiveWin->tlPX)+1)*(1+(pActiveWin->brPY-pActiveWin->tlPY));


    // Send command after which the data we sent
    // are recognized as pixels.
    WriteCommand_Addr(0x2c);
    hal_GpioSet(g_slcd_a0);
    //   g_image[0]=0xf8 ;
//    g_image[1]= 0x00;

    g_image[0]=(UINT8)((bgColor&0xff00)>>8); g_image[1]=(UINT8)(bgColor&0x00ff);
    for( j =0; j<k; j++ )
    {
        Write_Data_Image; // red
    }

    lcdd_MutexFree();
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

        // hal_GoudaSetBgColor(bgColor);
        lcddp_BlitRoi2Win(&activeWin,&activeWin,bgColor);

        return LCDD_ERR_NO;
    }
}
// ============================================================================
// lcdd_TransferData
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD CONST
// ============================================================================
//extern BOOL for_serial_sensor;
PRIVATE VOID lcdd_TransferData(UINT16* pPixelData,UINT16 nPixelWrite, BOOL lastLine)
{
#if 1
    UINT32 i;

    // if(for_serial_sensor == FALSE)
    if(1)
    {
        for(i = 0; i < nPixelWrite; i++)
        {
            pPixelData[i] = (pPixelData[i] >> 8) |(pPixelData[i] << 8);
        }
        hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,(UINT8*)(pPixelData),nPixelWrite*2);
        while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0))
        {
            sxr_Sleep(5);
        };
        for(i = 0; i < nPixelWrite; i++)
        {
            pPixelData[i] = (pPixelData[i] >> 8) |(pPixelData[i] << 8);
        }
    }
    else
    {
        //  hal_SpiClearTxDmaDone(HAL_SPI);
        //  hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,(UINT8*)(pPixelData),nPixelWrite*2);
        // while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0));

        hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,(UINT8*)(pPixelData),nPixelWrite*2);
        while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0))
        {
            sxr_Sleep(5);
        };

    }
    if (lastLine)
    {
        lcdd_MutexFree();
    }

#if 0
    HAL_DMA_CFG_T dmaCfg;
    dmaCfg.srcAddr = (UINT8*)(pPixelData);
    dmaCfg.dstAddr = (UINT8*) &LCM_DAT;
    dmaCfg.alterDstAddr = 0;
    dmaCfg.pattern = 0;
    // Number of bytes to transfer. One pixel being 16 bits,
    // transfer size is number of pixel * 2 !
    dmaCfg.transferSize = nPixelWrite*2;

    // Write the color in the data register of the screen
    dmaCfg.mode = HAL_DMA_MODE_CONST_ADDR;

    if (lastLine)
    {
        dmaCfg.userHandler = lcdd_FreeLock;
    }
#endif
#else
    UINT32 x;

    for (x = 0; x < nPixelWrite; x++)
    {
        g_image[0]=(UINT8)((pPixelData[x]&0xff00)>>8); g_image[1]=(UINT8)(pPixelData[x]&0x00ff);
        Write_Data_Image;

        //  hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,pPixelData[x],2);
        //   while(!hal_SpiTxFinished(HAL_SPI,HAL_SPI_CS0));
        //   while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0);



    }
    if (lastLine)
    {
        lcdd_MutexFree();
    }

#endif
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
//    hal_HstSendEvent(0x88855516);
    UINT32 now = hal_TimGetUpTime();

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
            g_lcddAutoCloseLayer = FALSE;
            WriteCommand_Addr(0x2a);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.tlPX);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.brPX);
            WriteCommand_Addr(0x2b);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.tlPY );
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.brPY);
#if 0
            LCM_WR_REG(0x0002,(hsa>>8)&0x00ff); // Column address start2
            LCM_WR_REG(0x0003,hsa&0x00ff); // Column address start1
            LCM_WR_REG(0x0004,(hea>>8)&0x00ff); // Column address end2
            LCM_WR_REG(0x0005,hea&0x00ff); // Column address end1
            LCM_WR_REG(0x0006,(vsa>>8)&0x00ff); // Row address start2
            LCM_WR_REG(0x0007,vsa&0x00ff); // Row address start1
            LCM_WR_REG(0x0008,(vea>>8)&0x00ff); // Row address end2
            LCM_WR_REG(0x0009,vea&0x00ff); // Row address end1
#endif
            WriteCommand_Addr(0x2c);
            hal_GpioSet(g_slcd_a0);
        }

        if (frameBufferWin->roi.width == frameBufferWin->fb.width)
        {
            // The source buffer and the roi have the same width, we can
            // do a single linear transfer
            lcdd_TransferData(frameBufferWin->fb.buffer+frameBufferWin->roi.y*frameBufferWin->roi.width
                              ,frameBufferWin->roi.width*frameBufferWin->roi.height,TRUE);
//              hal_HstSendEvent(0x88855528);

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
                             +(frameBufferWin->roi.x);

            for (curLine=startLine; curLine<=endLine; curLine++)
            {

                // transfer one line
                if (curLine == endLine)
                {
                    lcdd_TransferData(curBuf, frameBufferWin->roi.width, TRUE);
                    //hal_HstSendEvent(0x88855521);
                }
                else
                {
                    lcdd_TransferData(curBuf, frameBufferWin->roi.width, FALSE);
                }
                // goto next line
                curBuf+=frameBufferWin->fb.width;
            }
        }
        UINT32 now2 = hal_TimGetUpTime();
        SXS_TRACE(TSTDOUT, "lcd speed on frame %d ms ",(now2-now)*1000/16384);

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

    WriteCommand_Addr(0x36);     //Set Scanning Direction
    WriteCommand_Data(0xA8);
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

    WriteCommand_Addr(0x36);     //Set Scanning Direction
    WriteCommand_Data(0xC8);

    hal_TimDelay(1 MS_WAITING);
    lcdd_MutexFree();

    return TRUE;
}

extern VOID check_sensorID();

PRIVATE BOOL lcddp_CheckProductId()
{
    hal_HstSendEvent(0x654388);
    pmd_EnablePower(PMD_POWER_LCD,TRUE);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_Z);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_Z);


#ifdef __PRJ_WITH_SPICAM__
    hal_HstSendEvent(0x654399);

    check_sensorID();
#endif

    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_SPI);
    hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);


    UINT16 productId=0;
    g_slcd_rst = tgt_GetSpiLcdConfig()->lcdResetPin;
    g_slcd_a0 = tgt_GetSpiLcdConfig()->lcdA0Pin;

    // LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;
    hal_TimDelay(5 MS_WAITING);
    hal_GpioSet(g_slcd_rst);
    hal_TimDelay(10 MS_WAITING);
    hal_GpioClr(g_slcd_rst);
    hal_TimDelay(10 MS_WAITING);
    hal_GpioSet(g_slcd_rst);
    hal_TimDelay(50 MS_WAITING);

    // sxr_Sleep(20 MS_WAITING);
    productId = lcddp_Open_Read_LcdId();
    hal_GpioClr(g_slcd_rst);
    pmd_EnablePower(PMD_POWER_LCD,FALSE);


    SXS_TRACE(TSTDOUT, "ili9163c(0x%x): lcd read id is 0x%x ", LCD_ILI9163C_ID, productId);
    if(productId == LCD_ILI9163C_ID)
        return TRUE;
    else
    {
        return TRUE;
    }
}


// ============================================================================
// lcdd_spfd5408b_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL lcdd_ili9163c_RegInit(LCDD_REG_T *pLcdDrv)
{
//    hal_HstSendEvent(0x88855501);
    //LCDD_ASSERT(0,"dd");
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

        return TRUE;
    }

    return FALSE;
}

VOID test_read_spi_lcd_id()
{
    lcddp_CheckProductId();
    lcddp_Open_Read_LcdId();
}
VOID test_sleep()
{
    lcddp_Sleep();
}
VOID test_wake()
{
    lcddp_WakeUp();
}
//extern UINT16 display_sensor2[128*160];



VOID test_display_sensor(UINT16 * temp)
{

    UINT32 kk = 0;
    LCDD_ERR_T ret;
    // for (kk=0;kk<128*160;kk++) display_sensor2[kk]=0x00f8;
    LCDD_SCREEN_INFO_T screenInfo;
    LCDD_ROI_T roi;
    LCDD_FBW_T frameBufferWin;
    // Setup the display
    //lcdd_SetStandbyMode(FALSE);
    lcdd_GetScreenInfo(&screenInfo);
    roi.x=0;
    roi.y=0;
    roi.width=screenInfo.width;
    roi.height=screenInfo.height;
    frameBufferWin.fb.buffer = temp;
    frameBufferWin.fb.width =  128;
    frameBufferWin.fb.height = 160;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;

    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;
    frameBufferWin.roi.width= 128;
    frameBufferWin.roi.height= 160;
    ret = lcddp_Blit16(&frameBufferWin,0,0);// lcdd_Blit16

}




PRIVATE LCDD_ERR_T lcddp_SetWindows(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY)
{
    LCDD_ASSERT((frameBufferWin->fb.width&1) == 0, "LCDD: FBW must have an even number "
                "of pixels per line. Odd support is possible at the price of a huge "
                "performance lost");
    // Active window coordinates.
    HAL_GOUDA_WINDOW_T inputWin;
    HAL_GOUDA_WINDOW_T activeWin;
    UINT32 now = hal_TimGetUpTime();

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
        {
            g_lcddAutoCloseLayer = FALSE;
            WriteCommand_Addr(0x2a);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.tlPX);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.brPX); //127
            WriteCommand_Addr(0x2b);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.tlPY);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.brPY); //159
            WriteCommand_Addr(0x2c);
            hal_GpioSet(g_slcd_a0);
        }
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_0);
        return LCDD_ERR_NO;
    }
}

PRIVATE VOID lcddp_Lcd_SetIO_TriState_For_Lowpower(BOOL tri)
{
    if(tri == FALSE)
    {
        // spi_1
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_SPI);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);
        hal_GpioSet(g_slcd_rst);
        pmd_EnablePower(PMD_POWER_LCD,TRUE);

    }
    else
    {
        // spi_1
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_Z);
        hal_GpioClr(g_slcd_rst);
        hal_GpioClr(g_slcd_a0);
        pmd_EnablePower(PMD_POWER_LCD,FALSE);


    }

}


PRIVATE VOID lcddp_Lcd_SetIO_TriState(BOOL tri)
{
#ifdef _LCD_CAMERA_USE_SAME_SPI_
    if(tri == FALSE)
    {
        // spi_1
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        lcdd_MutexFree();
    }
    else
    {
        // spi_1
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_Z);
    }
#else
    if(tri == FALSE)
    {
        // spi_1
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_SPI);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_SPI);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_SPI);
        lcdd_MutexFree();
    }
    else
    {
        // spi_1
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CLK, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_DO, HAL_SPI_PIN_STATE_Z);
        hal_SpiForcePin(HAL_SPI,HAL_SPI_PIN_CS0, HAL_SPI_PIN_STATE_1);
    }
#endif

}

VOID Lcd_SetIO_TriState(BOOL tri)
{
    lcddp_Lcd_SetIO_TriState(tri);
}

VOID lcddp_SetWindowsForSpiSensor()
{
    LCDD_SCREEN_INFO_T screenInfo;
    LCDD_ROI_T roi;
    LCDD_FBW_T frameBufferWin;
    lcdd_GetScreenInfo(&screenInfo);
    roi.x=0;
    roi.y=0;
    //roi.width=screenInfo.width;
    // roi.height=screenInfo.height;
    roi.width=128;
    roi.height=160;

    frameBufferWin.fb.buffer = NULL;
    frameBufferWin.fb.width =  128;
    frameBufferWin.fb.height = 160;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;

    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;
    frameBufferWin.roi.width= 128;
    frameBufferWin.roi.height= 160;
    lcddp_SetWindows(&frameBufferWin,0,0);
    hal_HstSendEvent(0x55550017);

}

VOID lcd_ili9163c_SetWidows_sensor()
{
    LCDD_SCREEN_INFO_T screenInfo;
    LCDD_ROI_T roi;
    LCDD_FBW_T frameBufferWin;
    lcdd_GetScreenInfo(&screenInfo);
    roi.x=0;
    roi.y=0;
    //roi.width=screenInfo.width;
    // roi.height=screenInfo.height;
    roi.width=128;
    roi.height=160;

    frameBufferWin.fb.buffer = NULL;
    frameBufferWin.fb.width =  128;
    frameBufferWin.fb.height = 160;
    frameBufferWin.fb.colorFormat = LCDD_COLOR_FORMAT_RGB_565;

    frameBufferWin.roi.x=0;
    frameBufferWin.roi.y=0;
    frameBufferWin.roi.width= 128;
    frameBufferWin.roi.height= 160;
    lcddp_SetWindows(&frameBufferWin,0,0);
    hal_HstSendEvent(0x55550017);

}






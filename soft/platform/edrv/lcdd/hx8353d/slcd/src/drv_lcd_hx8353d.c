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

// =============================================================================
// global variables
// -----------------------------------------------------------------------------
// =============================================================================
#define SSLCD_REG_L 5
PRIVATE UINT8 g_sscld_data= 0;
PRIVATE UINT8 g_image[2]= {0};

PRIVATE HAL_APO_ID_T g_slcd_a0 = {{
        .type = HAL_GPIO_TYPE_IO,
        .id = 1
    }
};
PRIVATE HAL_APO_ID_T g_slcd_rst = {{
        .type = HAL_GPIO_TYPE_O,
        .id = 6
    }
};


// =============================================================================
//  MACROS while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0); hal_SpiTxFinished
// =============================================================================
#define MS_WAITING      *  HAL_TICK1S / 1000

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

#define  LCD_HX8352d_ID       0x0080

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
    hal_HstSendEvent(0x88855504);
    UINT32 i = 0;
    lcddp_SpiActivateCs();
    WriteCommand_Addr(0x11); // SLPOUT
    //DelayX1ms(150);
    sxr_Sleep(150 MS_WAITING);
    WriteCommand_Addr(0xB9);         //SETEXTC
    WriteCommand_Data(0xFF);
    WriteCommand_Data(0x83);
    WriteCommand_Data(0x53);

    WriteCommand_Addr(0xC6);        //UADJ Frame Rate=60Hz
    WriteCommand_Data(0x31);

    WriteCommand_Addr(0xB2); //SET DISPLAY
    WriteCommand_Data(0x00);

    WriteCommand_Addr(0xB1);         //SETPWCTR
    WriteCommand_Data(0x00);                 //DP_STB
    WriteCommand_Data(0x00);                 //Set BT_VGH
    WriteCommand_Data(0x0A);

    WriteCommand_Addr(0xBF);         //SETPTBA
    WriteCommand_Data(0x04);                 //PTBA[15:8] // Power driving setting
    WriteCommand_Data(0x38);                 //PTBA[7:0] // Power driving setting

    WriteCommand_Addr(0xC0);         //SETSTBA
    WriteCommand_Data(0x18);                 //N_OPON // OP driving--18
    WriteCommand_Data(0x08);                 //I_OPON // Idle for OP driving
    WriteCommand_Data(0x0C);                 //STBA[15:8] // Source Driving setting
    WriteCommand_Data(0xCA);         //STBA[7:0] // Source Driving setting


    WriteCommand_Addr(0xE3);         //EQ
    WriteCommand_Data(0x28);                 //VCI_M1
    WriteCommand_Data(0x08);                 //GND_M1
    WriteCommand_Data(0x08);                 //VCI_M0
    WriteCommand_Data(0x18);                //GND_M0

    WriteCommand_Addr(0xB6);         //VCOM
    WriteCommand_Data(0x93);                 //VMF
    WriteCommand_Data(0x42);                 //VMH
    WriteCommand_Data(0x62);                 //VML

    WriteCommand_Addr(0xE0);         //Gamma setting for  Panel
    WriteCommand_Data(0x00); //1
    WriteCommand_Data(0x77); //2
    WriteCommand_Data(0x21); //3
    WriteCommand_Data(0x08); //4
    WriteCommand_Data(0xDF); //5
    WriteCommand_Data(0x08); //6
    WriteCommand_Data(0x07); //7
    WriteCommand_Data(0x08); //8
    WriteCommand_Data(0x05); //9
    WriteCommand_Data(0x07); //10
    WriteCommand_Data(0x65); //11
    WriteCommand_Data(0x00); //12
    WriteCommand_Data(0x77); //13
    WriteCommand_Data(0x07); //14
    WriteCommand_Data(0xC0); //15
    WriteCommand_Data(0x07); //16
    WriteCommand_Data(0x08); //17
    WriteCommand_Data(0x07); //18
    WriteCommand_Data(0x0A); //19

    WriteCommand_Addr(0x2A);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x7f);

    WriteCommand_Addr(0x2B);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x01);
    WriteCommand_Data(0x00);
    WriteCommand_Data(0x9f);

    WriteCommand_Addr(0x36);
    WriteCommand_Data(0xc0);
    WriteCommand_Addr(0x3A);
    WriteCommand_Data(0x05);

    WriteCommand_Addr(0x2D);
    for(i=0 ; i<32; i++)
        WriteCommand_Data(i*2);
    for(i=0 ; i<64 ; i++)
        WriteCommand_Data(i);
    for(i=0 ; i<32 ; i++)
        WriteCommand_Data(i*2);

    WriteCommand_Addr(0xB0);                //RADJ
    WriteCommand_Data(0xC0);

    WriteCommand_Addr(0xCB);                //CADJ
    WriteCommand_Data(0x00);

    WriteCommand_Addr(0x2C);
    //DelayX1ms(150);
    sxr_Sleep(150 MS_WAITING);
    WriteCommand_Addr(0x29); // display on
    // DelayX1ms(150);}
    sxr_Sleep(150 MS_WAITING);
}


PRIVATE UINT16 lcddp_Open_Read_LcdId(VOID)
{
    // return 1;
    UINT32 count;
    // UINT8 data[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
    // UINT8 iTempData[6] = {0x9, 0xff,0xff,0xff,0xff,0xff};
    UINT8 data[3] = {0x0,0x0,0x0};
    UINT8 iTempData[3] = {0xdb,0xff, 0xff};
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
    hal_HstSendEvent(0x88855503);
    //hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);
    hal_GpioSetOut(g_slcd_a0.gpioId);
    lcddp_Open_Spi();



    lcddp_Init();
    sxr_Sleep(1 MS_WAITING);
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

    WriteCommand_Addr(0x10); //Set Sleep In
    sxr_Sleep(10 MS_WAITING);
    LCDD_TRACE(TSTDOUT, 0, "lcddp_Sleep: calling hal_GoudaClose");

    g_lcddInSleep = TRUE;
    hal_SpiDeActivateCs(HAL_SPI,HAL_SPI_CS0);

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
    // hal_GoudaOpen(&g_tgtLcddCfg.config, g_tgtLcddCfg.timings, 0);

#if 0
    lcddp_SpiActivateCs();
    WriteCommand_Addr(0x11); //Set Sleep Out
    sxr_Sleep(120 MS_WAITING);
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


        WriteCommand_Addr(0x20);
        WriteCommand_Data(x);
        WriteCommand_Addr(0x21);
        WriteCommand_Data(y);
        WriteCommand_Addr(0x22);
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
    hal_HstSendEvent(0x88855509);
    HAL_GOUDA_LCD_CMD_T cmd[13];
    UINT32 j = 0,k =0;

    if(!((pRoi->tlPX < pRoi->brPX) && (pRoi->tlPY < pRoi->brPY)))
    {
        LCDD_TRACE(LCDD_WARN_TRC,0,"lcddp_BlitRoi2Win: Invalid Roi x:%d < %d, y:%d < %d",pRoi->tlPX, pRoi->brPX, pRoi->tlPY, pRoi->brPY);
        lcddp_GoudaBlitHandler();
        return;
    }

    // building set roi sequence:

#if 1
    if(g_lcddRotate)
    {
        //Window Horizontal RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,0,0x0050,pActiveWin->tlPY);
        //Window Horizontal RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,2,0x0051,pActiveWin->brPY);
        //Window Vertical RAM Address Start
        LCDD_BUILD_CMD_WR_REG(cmd,4,0x0052,LCDD_DISP_Y-1-pActiveWin->brPX);
        //Window Vertical RAM Address End
        LCDD_BUILD_CMD_WR_REG(cmd,6,0x0053,LCDD_DISP_Y-1-pActiveWin->tlPX);

        //Start point
        LCDD_BUILD_CMD_WR_REG(cmd,8,0x0020,pActiveWin->tlPY);
        LCDD_BUILD_CMD_WR_REG(cmd,10,0x0021,LCDD_DISP_Y-1-pActiveWin->tlPX);
    }
    else
    {
        hal_HstSendEvent(0x88855510);
        //Window Horizontal RAM Address Start
        // LCDD_BUILD_CMD_WR_REG(cmd,0,0x0050,pActiveWin->tlPX);
        WriteCommand_Addr(0x2a); WriteCommand_Data(0x00); WriteCommand_Data(pActiveWin->tlPX);
        WriteCommand_Data(0x00); WriteCommand_Data(pActiveWin->brPX);
        //Window Horizontal RAM Address End
        // LCDD_BUILD_CMD_WR_REG(cmd,2,0x0051,pActiveWin->brPX);
        //Window Vertical RAM Address Start
        // LCDD_BUILD_CMD_WR_REG(cmd,4,0x0052,pActiveWin->tlPY);
        //Window Vertical RAM Address End
        // LCDD_BUILD_CMD_WR_REG(cmd,6,0x0053,pActiveWin->brPY);
        WriteCommand_Addr(0x2b); WriteCommand_Data(0x00); WriteCommand_Data(pActiveWin->tlPY);
        WriteCommand_Data(0x00); WriteCommand_Data(pActiveWin->brPY);

        //Start point
        //  LCDD_BUILD_CMD_WR_REG(cmd,8,0x0020,pActiveWin->tlPX);
        //  LCDD_BUILD_CMD_WR_REG(cmd,10,0x0021,pActiveWin->tlPY);
    }
    k = ((pActiveWin->brPX-pActiveWin->tlPX)+1)*(1+(pActiveWin->brPY-pActiveWin->tlPY));
    hal_HstSendEvent(0x88855511);

    hal_HstSendEvent(k);

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
#else

    LCM_WR_REG(0x0050,pActiveWin->tlPX);//Window Horizontal RAM Address Start
    LCM_WR_REG(0x0051,pActiveWin->brPX);//Window Horizontal RAM Address End
    LCM_WR_REG(0x0052,pActiveWin->tlPY);//Window Vertical RAM Address Start
    LCM_WR_REG(0x0053,pActiveWin->brPY);//Window Vertical RAM Address End

    LCM_WR_REG(0x0020,pActiveWin->tlPX);
    LCM_WR_REG(0x0021,pActiveWin->tlPY);

    LCDD_BUILD_CMD_WR_CMD(cmd,0,0x0022);

    while(HAL_ERR_NO != hal_GoudaBlitRoi(pRoi, 1, cmd, lcddp_GoudaBlitHandler));
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
    hal_HstSendEvent(0x88855506);

    if (0 == lcdd_MutexGet())
    {
        hal_HstSendEvent(0x88855507);

        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (g_lcddInSleep)
        {
            lcdd_MutexFree();
            return LCDD_ERR_NO;
        }
        hal_HstSendEvent(0x88855508);

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
//PRIVATE UINT16 g_buffer_data[128*160]={0};
PRIVATE VOID lcdd_TransferData(CONST UINT16* pPixelData,UINT16 nPixelWrite, BOOL lastLine)
{
#if 1
    UINT32 i;
    UINT16 g_dmaBusy = 0;
    for(i = 0; i<nPixelWrite; i++)
    {
        pPixelData[i]=(pPixelData[i]>>8)|(pPixelData[i]<<8);
    }
    g_dmaBusy = hal_SpiSendData(HAL_SPI,HAL_SPI_CS0,(UINT8*)(pPixelData),nPixelWrite*2);
    if(g_dmaBusy == 0)
    {
        hal_DbgAssert("dma busy");
    }
    while (!hal_SpiTxDmaDone(HAL_SPI, HAL_SPI_CS0));
    for(i = 0; i<nPixelWrite; i++)
    {
        pPixelData[i]=(pPixelData[i]>>8)|(pPixelData[i]<<8);
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
    hal_HstSendEvent(0x88855516);
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
            WriteCommand_Addr(0x2a); WriteCommand_Data(0x00); WriteCommand_Data(activeWin.tlPX);
            WriteCommand_Data(0x00); WriteCommand_Data(activeWin.brPX);
            WriteCommand_Addr(0x2b); WriteCommand_Data(0x00); WriteCommand_Data(activeWin.tlPY );
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
            hal_HstSendEvent(0x88855528);

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
                    lcdd_TransferData(curBuf, frameBufferWin->roi.width, TRUE); hal_HstSendEvent(0x88855521);
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

    // 80 = ORG = only work for blit at 0,0 : auto rotate
    WriteCommand_Addr(0x0003);
    WriteCommand_Data(0x1018);
//    LCM_WR_REG(0x0003,0x10a8);
//    LCM_WR_REG(0x0003,0x0030);

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

    WriteCommand_Addr(0x0003);
    WriteCommand_Data(0x1030);

    lcdd_MutexFree();

    return TRUE;
}


PRIVATE BOOL lcddp_CheckProductId()
{
    UINT16 productId=0;
    hal_HstSendEvent(0x88855502);
    // LCDD_CONFIG_T lcddReadConfig=LCDD_READ_CONFIG;
    pmd_EnablePower(PMD_POWER_LCD,TRUE);
    sxr_Sleep(5 MS_WAITING);
    // hal_GoudaOpen(&lcddReadConfig.config, lcddReadConfig.timings, 0);
    hal_GpioSet(g_slcd_rst);
    sxr_Sleep(10 MS_WAITING);
    hal_GpioClr(g_slcd_rst);
    sxr_Sleep(5 MS_WAITING);
    hal_GpioSet(g_slcd_rst);
    sxr_Sleep(50 MS_WAITING);

    // sxr_Sleep(20 MS_WAITING);
    productId = lcddp_Open_Read_LcdId();

    SXS_TRACE(TSTDOUT, "hx8535d(0x%x): lcd read id is 0x%x ", LCD_HX8352d_ID, productId);

    if(productId == LCD_HX8352d_ID)
        return TRUE;
    else
        return TRUE;
}


// ============================================================================
// lcdd_spfd5408b_RegInit
// ----------------------------------------------------------------------------
/// register the right lcd driver, according to lcddp_CheckProductId
/// @return #TRUE, #FALSE
// ============================================================================
PUBLIC BOOL lcdd_hx8353d_RegInit(LCDD_REG_T *pLcdDrv)
{
    hal_HstSendEvent(0x88855501);
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
        pLcdDrv->lcdd_GoudaBltHdl = lcddp_GoudaBlitHandler;
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


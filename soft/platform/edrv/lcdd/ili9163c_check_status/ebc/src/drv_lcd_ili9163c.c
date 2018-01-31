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

#include "hal_ebc.h"
#include "hal_pwm.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "hal_dma.h"

#include "pmd_m.h"

#include "lcdd_config.h"
//#include "lcdd_m.h"
#include "lcddp_debug.h"

#include "drv_lcd.h"

#include "sxr_tls.h"

#include "lcdd_private.h"
#include "lcdd_tgt_params_greenstone.h"
#include "lcdd.h"

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================

#define EXP2(n) (1<<(n))

#define LCM_REG                 *(volatile UINT8*)(g_lcddCsBase)
#define LCM_DAT                 *(volatile UINT8*)(g_lcddCsBase + g_lcddCsOffset)
#define LCM_WR_DATA_8(Data)       LCM_DAT = (UINT8)Data
#define LCM_WR_CMD_8(Data)       LCM_REG = (UINT8)Data
#define LCM_WR_CMD(Data)    {LCM_REG = (UINT16)Data>>8;\
                                LCM_REG = (UINT8)Data;\
                                }

#define LCM_WR_DATA(Data)   {LCM_DAT = (UINT16)Data>>8;\
                                LCM_DAT = (UINT8)Data;\
                                }
#define LCM_RD_REG(Addr)        (UINT8)({LCM_REG = (UINT8)Addr; LCM_DAT;})

#define main_Write_COM  LCM_WR_CMD_8
#define main_Write_DATA LCM_WR_DATA_8

#define LCM_CONTRAST_LVL0     0
#define LCM_CONTRAST_LVL1     32
#define LCM_CONTRAST_LVL2     64
#define LCM_CONTRAST_LVL3     96
#define LCM_CONTRAST_LVL4     128
#define LCM_CONTRAST_LVL5     160
#define LCM_CONTRAST_LVL6     196
#define LCM_CONTRAST_LVL7     255

// =============================================================================
// Screen properties
// =============================================================================
// Number of actual pixels in the display width
#define LCDD_DISP_X     128

// Number of pixels in the display height
#define LCDD_DISP_Y     160
#define  LCD_ILI9163C_ID         0x7735

//#define LCDD_DMA_ONE_SIZE   31680      //180*(176*2)
#define LCDD_WITHOUT_DMA
// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
PRIVATE CONST  LCDD_CONFIG_T g_tgtLcddCfg=TGT_LCDD_CONFIG;
/// LCDD both screen config.
PRIVATE CONST LCDD_CONFIG_T* g_lcddConfig
;

/// Main screen base address
PRIVATE UINT32 g_lcddCsBase  = 0;

/// Main screen offset address to access the second register.
PRIVATE UINT32 g_lcddCsOffset  = 0;

/// Structure holding the active window status for the main screen
PRIVATE LCDD_ACTIVE_WIN_STATUS_T   g_lcddActiveWin
;

/// Lock to protect the access to the LCD screen during a DMA access.
/// When 1, access is allowed. When 0, it is denied.
PRIVATE UINT32 g_lcddLock  = 0;

/// Constant table containing the actual Gain for each Level from 0 to 7
PRIVATE const UINT8 g_lcdContrastTable[8] =
{
    LCM_CONTRAST_LVL0,
    LCM_CONTRAST_LVL1,
    LCM_CONTRAST_LVL2,
    LCM_CONTRAST_LVL3,
    LCM_CONTRAST_LVL4,
    LCM_CONTRAST_LVL5,
    LCM_CONTRAST_LVL6,
    LCM_CONTRAST_LVL7
};



// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// lcdd_ReadReg
// -----------------------------------------------------------------------------
/// Write the address and then read a data from the LCD register
///
/// @param addr Address of the register to read.
/// @param pData Pointer to where to store the read data.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
PROTECTED VOID lcdd_ReadReg(UINT16 addr, UINT16* pData)
{
    *pData = LCM_RD_REG(addr);
}



// =============================================================================
// lcdd_ReadData
// -----------------------------------------------------------------------------
/// Read a data to the LCD screen.
///
/// @param pData Pointer to where to store the read data.
/// @return #HAL_ERR_NO when everything is fine, #HAL_ERR_RESOURCE_BUSY when
/// the LCDC controller is busy with another command.
// =============================================================================
PROTECTED VOID lcdd_ReadData(UINT16* pData)
{
    *pData = LCM_DAT;
}



// =============================================================================
// lcdd_MutexFree
// -----------------------------------------------------------------------------
/// Free a previously taken mutex. The ownership of the mutex is not checked.
/// Only free a mutex you have previously got.
/// @param mutex Pointer to the integer used as a mutex.
// =============================================================================
INLINE VOID lcdd_MutexFree(UINT32* mutex)
{
    // Writing is an atomic operation
    *mutex = 1;
}



// =============================================================================
// lcdd_MutexGet
// -----------------------------------------------------------------------------
/// This function enter in critical section, read the value of the mutex and,
/// if this is a '1', returns '1' and set the mutex to 0. If this is a '0',
/// it does nothing. Then, in both cases, it exists the Critical Section.
///
/// @param mutex Pointer to the integer used as a mutex.
/// @return '1' if the mutex was taken, '0' otherwise.
// =============================================================================
PRIVATE UINT32 lcdd_MutexGet(UINT32* mutex)
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

#ifndef LCDD_WITHOUT_DMA
// =============================================================================
// lcdd_FreeLock
// -----------------------------------------------------------------------------
/// This function frees the lock to access the screen. It is set as the user
/// handler called by the DMA driver at the end of the writings on the screen.
// =============================================================================
PRIVATE VOID lcdd_FreeLock(VOID)
{
    // Blitting op finish here
    LCDD_PROFILE_WINDOW_EXIT(lcdd_BlitWin);
    lcdd_MutexFree(&g_lcddLock);
}
#endif


// ============================================================================
// lcdd_Open
// ----------------------------------------------------------------------------
/// Open the LCDD driver.
/// It must be called before any call to any other function of this driver.
/// This function is to be called only once.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_Open(VOID)
{
    g_lcddConfig = &g_tgtLcddCfg;
    g_lcddCsBase     = (UINT32) hal_EbcCsOpen(g_lcddConfig->csNum, &g_lcddConfig->csConfig);
    g_lcddCsOffset = EXP2(g_lcddConfig->rsLine+1);

    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_DEVICE_NOT_FOUND;
    }
    else
    {
        // Turn off backlight so that we don't display anything wrong

        hal_SysResetOut(FALSE);


//--************ Start Initial Sequence **********--//
        main_Write_COM(0x11); //Exit Sleep
        sxr_Sleep(120 MS_WAITING);



        main_Write_COM(0x26); //Set Default Gamma
        main_Write_DATA(0x04);

        main_Write_COM(0xB1);
        main_Write_DATA(0x08);
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
        // Allow access
        g_lcddLock = 1;

        LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_Open: LCD Opened");
        return LCDD_ERR_NO;
    }
}



// ============================================================================
// lcdd_Close
// ----------------------------------------------------------------------------
/// Close the LCDD driver
/// No other functions of this driver should be called after a call to
/// #lcdd_Close.
/// @return #LCDD_ERR_NO or #LCDD_ERR_DEVICE_NOT_FOUND.
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_Close(VOID)
{
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_DEVICE_NOT_FOUND;
    }
    else
    {
        // Turn off backlight

        lcdd_Sleep();
        hal_SysResetOut(TRUE);
        hal_EbcCsClose(g_lcddConfig->csNum);
        LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_Close: LCD closed");
        return LCDD_ERR_NO;
    }
}



// ============================================================================
// lcdd_SetContrast
// ----------------------------------------------------------------------------
/// Set the contrast of the 'main'LCD screen.
/// @param contrast Value to set the contrast to.
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED or
/// #LCDD_ERR_INVALID_PARAMETER.
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_SetContrast(UINT32 contrast)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_SetContrast %d",contrast);
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        if (contrast>7)
        {
            return LCDD_ERR_INVALID_PARAMETER;
        }
        //LCM_WR_CMD(0x2A);
        //LCM_WR_CMD(g_lcdContrastTable[contrast]);
        return LCDD_ERR_NO;
    }
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
PRIVATE LCDD_ERR_T lcdd_SetStandbyMode(BOOL standbyMode)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_SetStandbyMode %d",standbyMode);
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        if (standbyMode)
        {
            lcdd_Sleep();
        }
        else
        {
            lcdd_WakeUp();
        }
        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcdd_Sleep
// ----------------------------------------------------------------------------
/// Set the main LCD screen in sleep mode.
/// @return #LCDD_ERR_NO or #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_Sleep(VOID)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_Sleep");
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        LCM_WR_CMD_8(0x10);
        sxr_Sleep(120 MS_WAITING);

        return LCDD_ERR_NO;
    }
}


// ============================================================================
// lcdd_PartialOn
// ----------------------------------------------------------------------------
/// Set the Partial mode of the LCD
/// @param vsa : Vertical Start Active
/// @param vea : Vertical End Active
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_PartialOn(UINT16 vsa, UINT16 vea)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_PartialOn vsa %d, vea %d",vsa, vea);
    if (0 == lcdd_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {


        lcdd_MutexFree(&g_lcddLock);
        return LCDD_ERR_NO;
    }
}

// ============================================================================
// lcdd_PartialOff
// ----------------------------------------------------------------------------
/// return to Normal Mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_PartialOff(VOID)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_PartialOff");
    if (0 == lcdd_MutexGet(&g_lcddLock))
    {
        return LCDD_ERR_RESOURCE_BUSY;
    }
    else
    {


        lcdd_MutexFree(&g_lcddLock);
        return LCDD_ERR_NO;
    }
}
// ============================================================================
// lcdd_WakeUp
// ----------------------------------------------------------------------------
/// Wake the main LCD screen out of sleep mode
/// @return #LCDD_ERR_NO, #LCDD_ERR_NOT_OPENED
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_WakeUp(VOID)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_WakeUp");
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        main_Write_COM(0x11); //Exit Sleep
        sxr_Sleep(120 MS_WAITING);



        main_Write_COM(0x26); //Set Default Gamma
        main_Write_DATA(0x04);

        main_Write_COM(0xB1);
        main_Write_DATA(0x08);
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

        return LCDD_ERR_NO;
    }
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
PRIVATE LCDD_ERR_T lcdd_GetScreenInfo(LCDD_SCREEN_INFO_T* screenInfo)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_GetScreenInfo");
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        screenInfo->width = LCDD_DISP_X;
        screenInfo->height = LCDD_DISP_Y;
        screenInfo->bitdepth = LCDD_COLOR_FORMAT_RGB_565;
        screenInfo->nReserved = 0;
        return LCDD_ERR_NO;
    }
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
PRIVATE LCDD_ERR_T lcdd_SetPixel16(UINT16 x, UINT16 y, UINT16 pixelData)
{
    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_SetPixel16 (%d,%d) %04x",x,y,pixelData);
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        if (0 == lcdd_MutexGet(&g_lcddLock))
        {
            return LCDD_ERR_RESOURCE_BUSY;
        }
        else
        {
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
            LCM_WR_DATA(pixelData);

            lcdd_MutexFree(&g_lcddLock);
            return LCDD_ERR_NO;
        }
    }
}
// ============================================================================
// lcdd_TransferRectDmaData
// ----------------------------------------------------------------------------
// Private function to transfer FillRect data to the LCD
// ============================================================================
#ifndef LCDD_WITHOUT_DMA
PRIVATE VOID lcdd_TransferRectDmaData(UINT32 nPixelLength, UINT16 dambgColor,BOOL lastTime)
{

    HAL_DMA_CFG_T dmaCfg;

    dmaCfg.srcAddr = 0;
    dmaCfg.dstAddr = (UINT8*) &LCM_DAT;
    dmaCfg.alterDstAddr = 0;
    dmaCfg.pattern = dambgColor;
    dmaCfg.transferSize = nPixelLength*2;

    // Write the color in the data register of the screen
    dmaCfg.mode = HAL_DMA_MODE_PATTERN_CONST_ADDR;
    // We free the lock at the end of the transfer

    if (lastTime)
    {
        dmaCfg.userHandler = lcdd_FreeLock;
    }
    else
    {
        dmaCfg.userHandler = NULL;
    }

    while(hal_DmaStart(&dmaCfg) != HAL_ERR_NO)
    {
    }

}

#endif
// ============================================================================
// lcdd_FillRect16
// ----------------------------------------------------------------------------
/// This function performs a fill of the active window  with some color.
/// @param bgColor Color with which to fill the rectangle. It is a 16-bit
/// data, as the one of #lcdd_SetPixel16
/// @return #LCDD_ERR_NO, #LCDD_ERR_RESOURCE_BUSY or #LCDD_ERR_NOT_OPENED.
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_FillRect16(CONST LCDD_ROI_T* regionOfInterrest, UINT16 bgColor)
{

    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_FillRect16 (%d,%d) (%d,%d) %04x",
               regionOfInterrest->x,
               regionOfInterrest->y,
               regionOfInterrest->x+regionOfInterrest->width-1,
               regionOfInterrest->y+regionOfInterrest->height-1,
               bgColor
              );
    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        if (0 == lcdd_MutexGet(&g_lcddLock))
        {
            return LCDD_ERR_RESOURCE_BUSY;
        }
        else
        {
            // Set Active window
            g_lcddActiveWin.hsa = regionOfInterrest->x;
            g_lcddActiveWin.hea = regionOfInterrest->x + regionOfInterrest->width - 1;
            g_lcddActiveWin.vsa = regionOfInterrest->y;
            g_lcddActiveWin.vea = regionOfInterrest->y + regionOfInterrest->height - 1;

            // Check parameters
            // ROI must be within the screen boundary
            if (    (g_lcddActiveWin.hea >= LCDD_DISP_X) ||
                    (g_lcddActiveWin.hsa >= LCDD_DISP_X) ||
                    (g_lcddActiveWin.vea >= LCDD_DISP_Y) ||
                    (g_lcddActiveWin.vsa >= LCDD_DISP_Y)
               )
            {
                lcdd_MutexFree(&g_lcddLock);
                return LCDD_ERR_INVALID_PARAMETER;
            }
            LCM_WR_CMD_8(0x2a);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.hsa);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.hea);

            LCM_WR_CMD_8(0x2b);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.vsa);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.vea);

            LCM_WR_CMD_8(0x2c);         // vertical RAM address position
#ifndef LCDD_WITHOUT_DMA

            UINT32 transferlength;   //240*320 Must <32640*3
            transferlength=(g_lcddActiveWin.hea-g_lcddActiveWin.hsa+1)
                           * (g_lcddActiveWin.vea-g_lcddActiveWin.vsa+1);

            if(transferlength<LCDD_DMA_ONE_SIZE)
            {
                lcdd_TransferRectDmaData(transferlength,bgColor,TRUE);
            }
            else
            {
                lcdd_TransferRectDmaData(LCDD_DMA_ONE_SIZE,bgColor,FALSE);
                lcdd_TransferRectDmaData(transferlength-LCDD_DMA_ONE_SIZE,bgColor,TRUE);
            }

#else

            UINT32 x;
            for (x = 0; x < (g_lcddActiveWin.hea-g_lcddActiveWin.hsa+1) *
                    (g_lcddActiveWin.vea-g_lcddActiveWin.vsa+1); ++x)
            {
                //LCM_DAT = bgColor;
                LCM_WR_DATA(bgColor);

            }
            lcdd_MutexFree(&g_lcddLock);

#endif

            return LCDD_ERR_NO;
        }
    }
}

// ============================================================================
// lcdd_TransferData
// ----------------------------------------------------------------------------
// Private function to transfer data to the LCD
// ============================================================================
PRIVATE VOID lcdd_TransferData(CONST UINT16* pPixelData,UINT32 nPixelWrite, BOOL lastLine)
{
#ifndef LCDD_WITHOUT_DMA
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
    else
    {
        dmaCfg.userHandler = NULL;
    }



    while(hal_DmaStart(&dmaCfg) != HAL_ERR_NO)
    {
    }
#else
    UINT32 x;

    for (x = 0; x < nPixelWrite; x++)
    {
        // LCM_DAT = pPixelData[x];
        LCM_WR_DATA(pPixelData[x]);

    }
    if (lastLine)
    {
        // Blitting op finish here
        LCDD_PROFILE_WINDOW_EXIT(lcdd_BlitWin);
        lcdd_MutexFree(&g_lcddLock);
    }
#endif
}

// ============================================================================
// lcdd_Blit16
// ============================================================================
PRIVATE LCDD_ERR_T lcdd_Blit16(CONST LCDD_FBW_T* frameBufferWin, UINT16 startX, UINT16 startY)
{

    LCDD_TRACE(LCDD_INFO_TRC, 0, "lcdd_Blit16 (%d,%d) (%d,%d)",
               startX,
               startY,
               startX+frameBufferWin->roi.width-1,
               startY+frameBufferWin->roi.height-1
              );

    if (0 == g_lcddCsBase)
    {
        return LCDD_ERR_NOT_OPENED;
    }
    else
    {
        if (0 == lcdd_MutexGet(&g_lcddLock))
        {
            return LCDD_ERR_RESOURCE_BUSY;
        }
        else
        {
            // Set Active window
            g_lcddActiveWin.hsa = startX;
            g_lcddActiveWin.hea = startX + frameBufferWin->roi.width - 1;
            g_lcddActiveWin.vsa = startY;
            g_lcddActiveWin.vea = startY + frameBufferWin->roi.height - 1;

            // Check parameters
            // ROI must be within the screen boundary
            // ROI must be within the Frame buffer
            // Color format must be 16 bits
            if (    (g_lcddActiveWin.hea >= LCDD_DISP_X) ||
                    (g_lcddActiveWin.hsa >= LCDD_DISP_X) ||
                    (g_lcddActiveWin.vea >= LCDD_DISP_Y) ||
                    (g_lcddActiveWin.vsa >= LCDD_DISP_Y) ||
                    (frameBufferWin->roi.width > frameBufferWin->fb.width) ||
                    (frameBufferWin->roi.height > frameBufferWin->fb.height) ||
                    (frameBufferWin->fb.colorFormat != LCDD_COLOR_FORMAT_RGB_565)
               )
            {
                lcdd_MutexFree(&g_lcddLock);
                return LCDD_ERR_INVALID_PARAMETER;
            }

            // Blitting op start here
            LCDD_PROFILE_WINDOW_ENTER(lcdd_BlitWin);

            LCM_WR_CMD_8(0x2a);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.hsa);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.hea);

            LCM_WR_CMD_8(0x2b);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.vsa);
            LCM_WR_DATA_8(0x00);
            LCM_WR_DATA_8(g_lcddActiveWin.vea);

            LCM_WR_CMD_8(0x2c);         // vertical RAM address position

            if (frameBufferWin->roi.width == frameBufferWin->fb.width)
            {
                // The source buffer and the roi have the same width,

                UINT32 transferlength=frameBufferWin->roi.width*frameBufferWin->roi.height;    //Must <32640*3
                UINT16* curBufAdd =frameBufferWin->fb.buffer+frameBufferWin->roi.y*frameBufferWin->roi.width;
#ifndef LCDD_WITHOUT_DMA
                if(transferlength<LCDD_DMA_ONE_SIZE)
                {
                    lcdd_TransferData(curBufAdd,transferlength,TRUE);
                }
                else
                {
                    lcdd_TransferData(curBufAdd,LCDD_DMA_ONE_SIZE,FALSE);
                    lcdd_TransferData(curBufAdd+LCDD_DMA_ONE_SIZE,transferlength-LCDD_DMA_ONE_SIZE,TRUE);
                }
#else
                lcdd_TransferData(curBufAdd,transferlength,TRUE);
#endif
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
                    }
                    else
                    {
                        lcdd_TransferData(curBuf, frameBufferWin->roi.width, FALSE);
                    }
                    // goto next line
                    curBuf+=frameBufferWin->fb.width;
                }
            }
            return LCDD_ERR_NO;
        }
    }
}
// ============================================================================
// lcdd_Busy
// ----------------------------------------------------------------------------
/// This function is not implemented for the ebc version of the driver
// ============================================================================
PRIVATE BOOL lcdd_Busy(VOID)
{
    return FALSE;
}

// ============================================================================
// lcdd_SetDirRotation
// ----------------------------------------------------------------------------
///
// ============================================================================
PRIVATE BOOL lcdd_SetDirRotation(VOID)
{


    return FALSE;
}
// ============================================================================
// lcdd_SetDirDefault
// ----------------------------------------------------------------------------
///
// ============================================================================
PRIVATE BOOL lcdd_SetDirDefault(VOID)
{

    return FALSE;
}

PRIVATE BOOL lcdd_CheckProductId()
{
    UINT16 manufacturer_ID=0;

    g_lcddConfig = &g_tgtLcddCfg;
    g_lcddCsBase = (UINT32) hal_EbcCsOpen(g_lcddConfig->csNum, &g_lcddConfig->csConfig);
    g_lcddCsOffset = EXP2(g_lcddConfig->rsLine+1);
    //manufacturer_ID=LCM_RD_REG(0x00);

    hal_EbcCsClose(g_lcddConfig->csNum);
    // force trace
    LCDD_TRACE(LCDD_INFO_TRC|TSTDOUT, 0,"lcd9320 ebc Id : %x ",manufacturer_ID);
// force true
    return TRUE;
    if((manufacturer_ID&0xFFFF)==LCD_ILI9163C_ID)
        return TRUE;
    else
        return FALSE;

}
//lcdd_CheckProductId
PUBLIC BOOL  lcdd_ili9163c_RegInit(LCDD_REG_T *pLcdDrv)
{
    if( lcdd_CheckProductId())
    {
        pLcdDrv->lcdd_Open=lcdd_Open;
        pLcdDrv->lcdd_Close=lcdd_Close;
        pLcdDrv->lcdd_SetContrast=lcdd_SetContrast;
        pLcdDrv->lcdd_SetStandbyMode=lcdd_SetStandbyMode;
        pLcdDrv->lcdd_PartialOn=lcdd_PartialOn;
        pLcdDrv->lcdd_PartialOff=lcdd_PartialOff;
        pLcdDrv->lcdd_Blit16=lcdd_Blit16;
        pLcdDrv->lcdd_Busy=lcdd_Busy;
        pLcdDrv->lcdd_FillRect16=lcdd_FillRect16;
        pLcdDrv->lcdd_GetScreenInfo=lcdd_GetScreenInfo;
        pLcdDrv->lcdd_WakeUp=lcdd_WakeUp;
        pLcdDrv->lcdd_SetPixel16=lcdd_SetPixel16;
        pLcdDrv->lcdd_Sleep=lcdd_Sleep;
        pLcdDrv->lcdd_SetDirRotation=lcdd_SetDirRotation;
        pLcdDrv->lcdd_SetDirDefault=lcdd_SetDirDefault;
        return TRUE;
    }
    return FALSE;

}



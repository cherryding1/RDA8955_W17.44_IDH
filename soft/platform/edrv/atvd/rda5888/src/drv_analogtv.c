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

#include "hal_sys.h"
#include "stdlib.h"
#include "sxs_io.h"
#include "hal_spi.h"

#include "hal_camera.h"
#include "hal_timers.h"
#include "drv_analogtv.h"
#include "pmd_m.h"
#include "hal_gpio.h"
#include "atvd_config.h"
#include "tgt_atvd_cfg.h"
#ifdef I2C_BASED_ON_GPIO
#include "gpio_i2c.h"
#else
PUBLIC HAL_I2C_BUS_ID_T g_atvdI2cBusId = HAL_I2C_BUS_ID_INVALID;
#endif

#include "rda5888app.h"
#include "chip_id.h"


PROTECTED CONST TGT_ATVD_CONFIG_T* g_atvdConfig;  /// Configuration structure for TARGET
PRIVATE BOOL g_isAtvCheckingStatus = FALSE;


//extern uint8 rdabt_getCurrentState(void);
extern PUBLIC UINT32 mmc_AnalogTvAudioOpen(VOID);
extern PUBLIC UINT32 mmc_AnalogTvAudioClose(VOID);


PRIVATE VOID atvd_Delay(UINT16 Wait_mS)
{
    hal_TimDelay(Wait_mS MS_WAITING);
}


//******************************************************************************
//**************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS *******************
//******************************************************************************

// =============================================================================
// atvd_Open
// -----------------------------------------------------------------------------
/// Open the analogtv driver, and do all the initialization needed to use the
/// functions of its API.
/// #atvd_Open must be called before any other function of the analogtv API
/// can be used.
/// @param none.
/// @return #ATVD_ERR_NO
// =============================================================================
PUBLIC ATVD_ERR_T atvd_Open(VOID)
{
    g_atvdConfig = tgt_GetAtvdConfig();

    sxs_fprintf(TSTDOUT,"atvd_Open");

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_open();
#else
    //xiaoyifeng for atv
    //for IIC pull up

    g_atvdI2cBusId = tgt_GetAtvdConfig()->i2cBusId;
    hal_I2cOpen(g_atvdI2cBusId);
#endif

    sxs_fprintf(TSTDOUT,"atvd_Open after IIC INIT");

#ifdef TGT_CAMERA_PDN_CHANGE_WITH_ATV
    hal_CameraPowerDownBoth(TRUE);
#endif
    pmd_EnablePower(PMD_POWER_CAMERA,TRUE);
    // Sets the PowerDown
    if(g_atvdConfig->pinPdn.type==HAL_GPIO_TYPE_IO)
    {
        hal_GpioSetOut(g_atvdConfig->pinPdn.gpioId);
    }
    hal_GpioSet(g_atvdConfig->pinPdn);

    // Enable 26M AUX clock
#ifdef RDA5888_26MCRYSTAL_ENABLE
    //if (0 == rdabt_getCurrentState())
    {
        hal_SysAuxClkOut(TRUE);
    }
#endif

    atvd_Delay(50);

    RDAAPP_Init();


//confirm the hw
    {
        extern U8 isEarphonePlugged_1;
        MCI_FmSetOutputDevice(1);
        if(isEarphonePlugged_1 ==0)
        {
            MCI_FmSetOutputDevice(2);
        }
    }

    atvd_Delay(50);

    mmc_AnalogTvAudioOpen();    //test audio xiaoyifeng

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_Close
// -----------------------------------------------------------------------------
/// Close the atvd driver. After a call to #atvd_Close, a call to
/// #catvd_Open is needed to be allowed to use atvd functions.
/// @return #ATVD_ERR_NO.
// =============================================================================
PUBLIC ATVD_ERR_T atvd_Close(VOID)
{

    RDAAPP_Exit();

#ifdef RDA5888_26MCRYSTAL_ENABLE
//if ( 0==rdabt_getCurrentState() )
    {
        hal_SysAuxClkOut(FALSE);
    }
#endif

#ifdef I2C_BASED_ON_GPIO
    gpio_i2c_close();
#else
    //xiaoyifeng for atv
    hal_I2cClose(g_atvdI2cBusId);
#endif

    // Disable the LDO  for the analogtv; FIXME
    // pmd_EnablePower(PMD_POWER_CAMERA,FALSE);

    hal_GpioClr(g_atvdConfig->pinPdn);
    mmc_AnalogTvAudioClose();   //test audio xiaoyifeng

    return ATVD_ERR_NO;
}

PUBLIC uint8 atvd_Is625Lines(void)
{
    return RDAAPP_Is625Lines();
}

// =============================================================================
// atvd_SetTestPattern
// -----------------------------------------------------------------------------
/// This function sets the chip to the test pattern.
/// @param
/// @return
// =============================================================================
PUBLIC ATVD_ERR_T atvd_SetTestPattern(void)
{
    RDA5888_SetTestPattern();

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_GetRSSI
// -----------------------------------------------------------------------------
/// This function gets the rssi value for the current channel.
/// @param
/// @return #rssi rssi value
// =============================================================================
PUBLIC int16 atvd_GetRSSI(void)
{
    int16 rssi;

    rssi = RDA5888_GetRSSI();

    return rssi;
}

// =============================================================================
// atvd_LumaAdjust
// -----------------------------------------------------------------------------
/// This function sets luminance for the current channel being processed.
/// @param #nLevel luminance level to set.
/// @return
// =============================================================================
PUBLIC uint32 atvd_LumaAdjust(uint8 nLevel)
{
    RDA5888_SetLumaLevel(nLevel);

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_ChromAdjust
// -----------------------------------------------------------------------------
/// This function sets chrominance for the current channel being processed.
/// @param #nLevel chrominance level to set.
/// @return
// =============================================================================
PUBLIC uint32 atvd_ChromAdjust(uint8 nLevel)
{
    RDA5888_SetChromLevel(nLevel);

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_SetVolume
// -----------------------------------------------------------------------------
/// This function used to adjust volume level.
/// @param #nLevel volume level to set.
/// @return
// =============================================================================
PUBLIC uint32 atvd_SetVolume(uint8 nLevel)
{
    RDAAPP_SetVolume(nLevel);

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_I2sOpen
// -----------------------------------------------------------------------------
/// This function open the I2S interface
/// @param atvdI2sCfg I2S configuration
/// @return
// =============================================================================
PUBLIC ATVD_ERR_T atvd_I2sOpen(ATVD_I2S_CFG_T atvdI2sCfg)
{
    //I2S slave
    RDA5888_I2SOpen(0);

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_I2sClose
// -----------------------------------------------------------------------------
/// This function close the I2S interface
/// @return
// =============================================================================
PUBLIC ATVD_ERR_T atvd_I2sClose(void)
{
    RDA5888_I2SClose();

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_SetTVArea
// -----------------------------------------------------------------------------
/// This function sets current country or area.
/// @return
// =============================================================================
PUBLIC ATVD_ERR_T atvd_SetTVArea(uint32 cTVArea)
{
    switch (cTVArea)
    {
        case TV_AREA_RUSSIA:
            RDAAPP_SetTVArea(RDAMTV_AREA_RUSSIA);
            break;

        case TV_AREA_CHINA:
            RDAAPP_SetTVArea(RDAMTV_AREA_CHINA);
            break;

        case TV_AREA_CHINA_SHENZHEN:
            RDAAPP_SetTVArea(RDAMTV_AREA_CHINA_SHENZHEN);
            break;

        case TV_AREA_CHINA_HONGKONG:
        case TV_AREA_UK:
        case TV_AREA_SOUTH_AFRICA:
        case TV_AREA_CHINA_MACAO:
            RDAAPP_SetTVArea(RDAMTV_AREA_CHINA_HONGKONG);
            break;

        case TV_AREA_CHINA_TAIWAN:
        case TV_AREA_AMERICA:
        case TV_AREA_MEXICO:
        case TV_AREA_PHILIPINES:
        case TV_AREA_KOREA:
        case TV_AREA_CHILE:
        case TV_AREA_VENEZUELA:
        case TV_AREA_CANADA:
            RDAAPP_SetTVArea(RDAMTV_AREA_AMERICA);
            break;

        case TV_AREA_ARGENTINA:
            RDAAPP_SetTVArea(RDAMTV_AREA_ARGENTINA);
            break;

        case TV_AREA_BRAZIL:
            RDAAPP_SetTVArea(RDAMTV_AREA_BRAZIL);
            break;

        case TV_AREA_JAPAN:
            RDAAPP_SetTVArea(RDAMTV_AREA_JAPAN);
            break;

        case TV_AREA_WESTERNEUROP:
        case TV_AREA_TURKEY:
        case TV_AREA_AFGHANISTA:
        case TV_AREA_SINGAPORE:
        case TV_AREA_THAI:
        case TV_AREA_CAMBODIA:
        case TV_AREA_INDONESIA:
        case TV_AREA_MALAYSIA:
        case TV_AREA_LAOS:
        case TV_AREA_INDIA:
        case TV_AREA_PAKISTAN:
        case TV_AREA_SPAIN:
        case TV_AREA_PORTUGAL:
        case TV_AREA_YEMEN:
        case TV_AREA_BAHRAIN:
        case TV_AREA_ABU_DHABI:
        case TV_AREA_KUWAIT:
            RDAAPP_SetTVArea(RDAMTV_AREA_WESTERNEUROP);
            break;

        case TV_AREA_VIETNAM:
            RDAAPP_SetTVArea(RDAMTV_AREA_VIETNAM);
            break;

        case TV_AREA_BURMA:
            RDAAPP_SetTVArea(RDAMTV_AREA_BURMA);
            break;

        case TV_AREA_FACTORYMODE:
            RDAAPP_SetTVArea(RDAMTV_AREA_FACTORYMODE);
            break;

        default:
            RDAAPP_SetTVArea(RDAMTV_AREA_CHINA);
            break;
    }

    return ATVD_ERR_NO;
}

// =============================================================================
// atvd_SetChannel
// -----------------------------------------------------------------------------
/// This function sets a channel.
/// @return #TRUE: channel set done, #FALSE: channel set fail.
// =============================================================================
PUBLIC uint8 atvd_SetChannel(uint32 nChannel)
{
    return RDAAPP_SetChannel(nChannel, 0);
}

// =============================================================================
// atvd_ScanChannel
// -----------------------------------------------------------------------------
/// This function scans a channel.
/// @return #TRUE: channel scan done, #FALSE: channel scan fail.
// =============================================================================
PUBLIC uint8 atvd_ScanChannel(uint32 nChannel)
{
    return RDAAPP_SetChannel(nChannel, 1);
}

// =============================================================================
// atvd_GetSignalLevel
// -----------------------------------------------------------------------------
/// This function gets current signal level.
/// @return
// =============================================================================
PUBLIC uint8 atvd_GetSignalLevel(void)
{
    return RDAAPP_GetSignalLevel();
}

// =============================================================================
// atvd_GetTotalChannels
// -----------------------------------------------------------------------------
/// This function gets total channels for current frequencies table.
/// @return
// =============================================================================
PUBLIC uint8 atvd_GetTotalChannels(void)
{
    return RDAAPP_GetChannelCount();
}

PUBLIC void atvd_start_check_status(void)
{
    g_isAtvCheckingStatus=TRUE;
}

PUBLIC void atvd_stop_check_status(void)
{
    g_isAtvCheckingStatus=FALSE;
}

PUBLIC BOOL atvd_is_checking_status(void)
{
    return g_isAtvCheckingStatus;
}




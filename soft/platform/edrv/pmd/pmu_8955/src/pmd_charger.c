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


#include "pmd_config.h"
#include "pmd_m.h"
#include "pmdp_debug.h"
#include "pmdp_charger.h"
#include "pmdp.h"

#include "opal.h"

#include "hal_ana_gpadc.h"
#include "hal_timers.h"
#include "hal_lps.h"
#include "hal_host.h"

// for function timers
#include "sxr_tim.h"

#include "tgt_pmd_cfg.h"


// externs
extern BOOL g_pmdChargerInIrqEnabled;
//13h[3:0]=vfb //  4350
//0->4.4; 1->4.392; 2->4.37; 3->4.34; 4->4.32; 5->4.3; 6->4.27
#ifdef _USE_HIGH_VOLTAGE_4350_BATTERY_

#define CHR_VFB_SEL_MIN_VAL (0) //if the battery tem volt is 4.35V ,it should changes 1.
#else
#define CHR_VFB_SEL_MIN_VAL (4) //if the battery tem volt is 4.35V ,it should changes 1.

#endif
#define CHR_VFB_SEL_MAX_VAL (0xe)

//13H[3:0]=vfb  chr_cur<5mA //
//0000              4.4100V
//0001              4.3900V
//0010              4.3700V
//0011              4.3400V
//0100              4.3200V
//0101              4.3000V
//0110              4.2700V
//0111              4.2500V
//1000              4.2300V
//1001              4.2100V
//1010              4.1800V
//1011              4.1600V
//1100              4.1400V
//1101              4.1200V
//1110              4.1000V
//1111              4.0700V


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// PMD_EFUSE_PAGE_T
// ----------------------------------------------------------------------------
/// This enum lists the efuse pages
// ============================================================================
typedef enum
{
    PMD_EFUSE_PAGE_CHARGER = 0,
    PMD_EFUSE_PAGE_VBAT_3P4 = 1,
    PMD_EFUSE_PAGE_VBAT_4P2 = 2,
} PMD_EFUSE_PAGE_T;

// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// RDA_ADDR_CHARGER_SETTING1 = 0x12
PRIVATE UINT16 g_pmdChargerSetting1 =
    RDA_PMU_CHR_AUXADC_SEL(0)|
//    RDA_PMU_CHR_BYPASS_CHOPPER|
    RDA_PMU_UV_SEL(2)|
    RDA_PMU_BG_DIS_CHOPPER_FILT|
    RDA_PMU_BG_DIS_CHOPPER|
    RDA_PMU_BG_CAL_TC_BIT2(26);

// RDA_ADDR_CHARGER_SETTING2 = 0x13
PRIVATE UINT16 g_pmdChargerSetting2 =
    RDA_PMU_CHR_CC_IBIT_REG(6)|
    RDA_PMU_CHR_PRE_IBIT_REG(7)|
    RDA_PMU_CHR_VFB_SEL_REG(8);

// RDA_ADDR_CHARGER_CONTROL = 0x15
PRIVATE UINT16 g_pmdChargerControl =
    RDA_PMU_CHR_CC_MODE_REG|
    RDA_PMU_CHR_EN_FREQ(1)|
    RDA_PMU_CHR_EN_DUTY_CYCLE(1);

// RDA_ADDR_CALIBRATION_SETTING1 = 0x16
PRIVATE UINT16 g_pmdCalibSetting1 =
    RDA_PMU_BGAP_CAL_POLARITY|
    RDA_PMU_BGAP_CAL_BIT_REG(32);

// BG LP mode registry
PRIVATE BOOL g_pmdBgLpModeRegistry[PMD_BG_LP_MODE_USER_ID_QTY];
// Whether BG LP mode is enabled
PRIVATE BOOL g_pmdBgLpModeEn = TRUE;


// ============================================================================
//  PRIVATE FUNCTIONS PROTOTYPING
// ============================================================================


// ============================================================================
//  FUNCTIONS
// ============================================================================


#ifdef __BATTERY_TEMPERATURE_DETECT__
#define TEMPERATURE_CHAN HAL_ANA_GPADC_CHAN_0
PUBLIC UINT32 pmd_GetChargerTemperature(VOID)
{
    UINT16 readBat = hal_AnaGpadcGet(TEMPERATURE_CHAN);
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_GetChargerTemperature  readBat  %d ", readBat);
    if (HAL_ANA_GPADC_BAD_VALUE == readBat)
    {
        return PMD_BAD_VALUE;
    }
    else
    {

//hal_HstSendEvent(0xbaadc);

//hal_HstSendEvent(6*readBat);

        return (6*readBat);
    }
}

#endif /* __BATTERY_TEMPERATURE_DETECT__ */



// ============================================================================
// pmd_GetGpadcTemperatureLevel
// ----------------------------------------------------------------------------
/// Return the more up to date value measured for the battery temperature
///
/// @return the temperature in mV or
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetGpadcTemperatureLevel(VOID)
{
    UINT16 gpadc = hal_AnaGpadcGet(g_pmdConfig->temperatureGpadcChannel);
    if (HAL_ANA_GPADC_BAD_VALUE == gpadc)
    {
        return PMD_BAD_VALUE;
    }
    else
    {
        return (gpadc);
    }
}

PROTECTED INT16 pmd_TemperatureVolt2Degree(UINT16 volt)
{
    if (volt == PMD_BAD_VALUE)
    {
        return PMD_BAD_TEMPERATURE;
    }

    return volt;
}

// ============================================================================
// pmd_GetBatteryTemperature
// ----------------------------------------------------------------------------
/// Return the more up to date value measured for the battery temperature
///
/// @return the battery temperature in degree or
/// #PMD_BAD_TEMPERATURE if no result is available yet.
// ============================================================================
PUBLIC INT16 pmd_GetBatteryTemperature(VOID)
{
    INT16 tp;

    if (!pmd_TemperatureMonEnabled())
    {
        return PMD_BAD_TEMPERATURE;
    }

    tp = pmd_TemperatureVolt2Degree(pmd_GetGpadcTemperatureLevel());
    return tp;
}

#ifdef TXPOWER_TEMPERATURE_OFFSET
PRIVATE UINT16 g_sw_tp;
PUBLIC VOID pmd_GetGpadcTemperatureSW(VOID)
{
    g_sw_tp = hal_AnaGpadcGetRaw(HAL_ANA_GPADC_CHAN_0);
}
PUBLIC UINT16 pmd_GeTemperatureSW(VOID)
{
    return g_sw_tp;
}

#endif

// ============================================================================
// pmd_GetGpadcChargerLevel
// ----------------------------------------------------------------------------
/// Return the more up do date value measured for the charger level
///
/// @return the charger level in mV or
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetGpadcChargerLevel(VOID)
{
    UINT16 gpadc = hal_AnaGpadcGet(g_pmdConfig->chargerGpadcChannel);
    if (HAL_ANA_GPADC_BAD_VALUE == gpadc)
    {
        return PMD_BAD_VALUE;
    }
    else
    {
        return (6*gpadc);
    }
}

#ifdef __BATTERY_TEMPERATURE_DETECT__

#if defined(__MMX_TEMP_CHARGER_FUNCTION__)
enum
{
    PMD_TEMPERATURE_STATUS_NORMAL,
    PMD_TEMPERATURE_STATUS_50_55,
    PMD_TEMPERATURE_STATUS_OVER_55,
    PMD_TEMPERATURE_STATUS_BAD_VALUE,
} PMD_TEMPERATURE_STATUS;


#define PMD_TEMPERATURE_45C  0x184    //1500   9000
#define PMD_TEMPERATURE_50C  0x160 //0x180    //1100   6600
#define PMD_TEMPERATURE_55C  0x126    //700     4200

#define PMD_BATTERY_OFF  0x2710
BOOL g_pmd_battemp_off_flag = FALSE;


BOOL g_pmd_over_battemp_flag = FALSE;
BOOL g_pmd_low_battemp_flag = FALSE;

#else/*__MMX_TEMP_CHARGER_FUNCTION__*/
static BOOL PMD_TEMPERATURE_STATUS_NORMAL = TRUE;

#define PMD_TEMPERATURE_MINUS_20C   0x118A
#define PMD_TEMPERATURE_0C  0xCF5
#define PMD_TEMPERATURE_65C  0x110

#define PMD_BATTERY_OFF  0x2710
BOOL g_pmd_battemp_off_flag = FALSE;

BOOL g_pmd_over_battemp_flag = FALSE;
BOOL g_pmd_low_battemp_flag = FALSE;
BOOL g_pmd_normal_flag = FALSE;

#endif/*__MMX_TEMP_CHARGER_FUNCTION__*/

PUBLIC UINT16 pmd_test_global_temp = 0;



PUBLIC U8 pmd_CheckChargerTemperature(VOID)
{
    UINT32 tmp;

    tmp = pmd_GetChargerTemperature();

    if(PMD_BAD_VALUE == tmp)
    {
#if defined(__MMX_TEMP_CHARGER_FUNCTION__)
        return PMD_TEMPERATURE_STATUS_BAD_VALUE;
#else
        return TRUE;
#endif
    }

    pmd_test_global_temp = tmp;
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_CheckChargerTemperature  tmp  %d ", tmp);

#if defined(__MMX_TEMP_CHARGER_FUNCTION__)

    if(tmp < PMD_TEMPERATURE_55C) // over temp
    {
        //    g_pmd_over_battemp_flag = TRUE;

        PMD_TEMPERATURE_STATUS = PMD_TEMPERATURE_STATUS_OVER_55;
    }
    else if(tmp>= PMD_TEMPERATURE_55C && tmp <PMD_TEMPERATURE_50C )
    {
        // g_pmd_low_battemp_flag = TRUE;

        PMD_TEMPERATURE_STATUS = PMD_TEMPERATURE_STATUS_50_55;
    }
    else if ( tmp >= PMD_TEMPERATURE_50C )
    {
        //  g_pmd_over_battemp_flag=FALSE;
        PMD_TEMPERATURE_STATUS= PMD_TEMPERATURE_STATUS_NORMAL;
    }


    if(tmp >= PMD_BATTERY_OFF) // battery off
    {
        g_pmd_battemp_off_flag = TRUE;
    }

    return  PMD_TEMPERATURE_STATUS;


#else/*__MMX_TEMP_CHARGER_FUNCTION__*/
    g_pmd_over_battemp_flag = FALSE;
    g_pmd_low_battemp_flag = FALSE;
    g_pmd_normal_flag = FALSE;

    if(tmp < PMD_TEMPERATURE_65C) // over temp
    {
        g_pmd_over_battemp_flag = TRUE;
        PMD_TEMPERATURE_STATUS_NORMAL = FALSE;
    }
    else if(tmp > PMD_TEMPERATURE_0C && tmp < PMD_TEMPERATURE_MINUS_20C ) // low temp
    {
        g_pmd_low_battemp_flag = TRUE;
        PMD_TEMPERATURE_STATUS_NORMAL = FALSE;
    }
    else
    {
        g_pmd_normal_flag = TRUE;

        PMD_TEMPERATURE_STATUS_NORMAL = TRUE;
    }
    if(tmp >= PMD_BATTERY_OFF) // battery off
    {
        g_pmd_battemp_off_flag = TRUE;
    }

    return  PMD_TEMPERATURE_STATUS_NORMAL;

#endif/*__MMX_TEMP_CHARGER_FUNCTION__*/


}
#endif /* __BATTERY_TEMPERATURE_DETECT__ */


// ============================================================================
// pmd_GetChargerLevel
// ----------------------------------------------------------------------------
/// Return the more up to date value measured for the charger level
///
/// @return the charger level in mV or
/// #PMD_BAD_VALUE if no result is available yet.
// ============================================================================
PUBLIC UINT16 pmd_GetChargerLevel(VOID)
{
    UINT16 level;
    PMD_CHARGER_STATUS_T status;

    status = pmd_GetChargerStatus();
    if (status == PMD_CHARGER_UNKNOWN || status == PMD_CHARGER_UNPLUGGED)
    {
        return PMD_BAD_VALUE;
    }

    level = pmd_GetGpadcChargerLevel();

    return level;
}

// ============================================================================
// pmd_PmuSetChargeCurrent
// ----------------------------------------------------------------------------
/// Set the current for the charge.
// ============================================================================
PROTECTED VOID pmd_PmuSetChargeCurrent(PMD_CHARGE_CURRENT_T current)
{
    UINT32 regVal = 0;
    BOOL pre_mode = FALSE;
    // should get bandgap from global/calib
    switch (current)
    {
        case PMD_CHARGER_50MA:

#ifdef CHIP_DIE_8809E2
            pre_mode = TRUE;
            regVal = RDA_PMU_CHR_PRE_IBIT_79MA;
#else
            regVal = RDA_PMU_CHR_CC_IBIT_100MA;
#endif
            break;
        case PMD_CHARGER_100MA:
            regVal = RDA_PMU_CHR_CC_IBIT_100MA;
            break;
        case PMD_CHARGER_150MA:
            regVal = RDA_PMU_CHR_CC_IBIT_200MA;
            break;
        case PMD_CHARGER_200MA:
            regVal = RDA_PMU_CHR_CC_IBIT_200MA;
            break;
        case PMD_CHARGER_300MA:
            regVal = RDA_PMU_CHR_CC_IBIT_300MA;
            break;
        case PMD_CHARGER_400MA:
            regVal = RDA_PMU_CHR_CC_IBIT_400MA;
            break;
        case PMD_CHARGER_500MA:
            regVal = RDA_PMU_CHR_CC_IBIT_500MA;
            break;
        case PMD_CHARGER_600MA:
            regVal = RDA_PMU_CHR_CC_IBIT_600MA;
            break;
        case PMD_CHARGER_700MA:
            regVal = RDA_PMU_CHR_CC_IBIT_700MA;
            break;
        case PMD_CHARGER_800MA:
            regVal = RDA_PMU_CHR_CC_IBIT_700MA;
            break;
        default:
            PMD_ASSERT(FALSE, "Unsupported charge current: %d", current);
            break;
    }
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_PmuSetChargeCurrent current %d,  regVal =0x%x ,g_pmdChargerControl=0x%x",current,regVal,g_pmdChargerControl);

    if(pre_mode == TRUE)
    {
        g_pmdChargerSetting2 = ( g_pmdChargerSetting2 &
                                 ~RDA_PMU_CHR_PRE_IBIT_REG_MASK ) |
                               regVal |
                               RDA_PMU_CHR_PRE_IBIT_DR;


        g_pmdChargerControl  = ( g_pmdChargerControl & ~(RDA_PMU_CHR_CC_MODE_DR|RDA_PMU_CHR_CC_MODE_REG)) |
                               (RDA_PMU_CHR_CC_MODE_DR|RDA_PMU_CHR_PRE_MODE_REG);

        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_PmuSetChargeCurrent current g_pmdChargerControl =0x%x",g_pmdChargerControl );

        pmd_RDAWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);
    }
    else
    {

        g_pmdChargerSetting2 = ( g_pmdChargerSetting2 &
                                 ~RDA_PMU_CHR_CC_IBIT_REG_MASK ) |
                               regVal |
                               RDA_PMU_CHR_CC_IBIT_DR;

        if((g_pmdChargerControl & RDA_PMU_CHR_CC_MODE_REG) == 0)
        {
            g_pmdChargerControl  = ( g_pmdChargerControl & ~(RDA_PMU_CHR_CC_MODE_DR|RDA_PMU_CHR_CC_MODE_REG)) |
                                   (RDA_PMU_CHR_CC_MODE_DR|RDA_PMU_CHR_CC_MODE_REG);

            pmd_RDAWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);
            PMD_TRACE(PMD_INFO_TRC, 0, "pmd_PmuSetChargeCurrent current g_pmdChargerControl2 =0x%x",g_pmdChargerControl );

        }

    }
    pmd_RDAWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
}

PROTECTED VOID pmd_TuneCharger(VOID)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_TuneCharger");

    // NOTE: Bandgap calib might cause LDO output burr for old 8809 chips
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdCalibSetting1 = (g_pmdCalibSetting1&~RDA_PMU_RESETN_BGAP_CAL_REG) |
                         RDA_PMU_RESETN_BGAP_CAL_DR;
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);

    g_pmdCalibSetting1 &= ~(RDA_PMU_RESETN_BGAP_CAL_DR|RDA_PMU_RESETN_BGAP_CAL_REG);
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);
    hal_SysExitCriticalSection(scStatus);
    // Wait until calib done (no sleep allowed)
    hal_TimDelay(2 MS_WAITING);
}
extern BOOL pmd_GetChargerVoltageValidity(VOID) ;

PUBLIC VOID pmd_ForceChargerFinished(BOOL stop)
{
#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
    stop = TRUE;
#endif
    if(stop == FALSE)
    {
        if(pmd_GetChargerVoltageValidity() == FALSE) return;
    }
    if (stop)
    {
        // Direct-reg to disable charging
        g_pmdChargerControl &= ~RDA_PMU_CHR_ENABLE_REG;
        g_pmdChargerControl |= RDA_PMU_CHR_ENABLE_DR;
    }
    else
    {
        // Direct-reg to enable charging
        // Bypass PMU charge timeout control (about 8 hours)
        g_pmdChargerControl |= (RDA_PMU_CHR_ENABLE_DR|
                                RDA_PMU_CHR_ENABLE_REG);
    }
    pmd_RDAWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);
}

PRIVATE VOID pmd_RestoreChargerControl(VOID)
{
    // Not to direct-reg charging
    g_pmdChargerControl &= ~(RDA_PMU_CHR_ENABLE_DR|
                             RDA_PMU_CHR_ENABLE_REG);
    pmd_RDAWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);
}

PROTECTED RDA_PMU_CHR_STATUS_T pmd_GetChargerHwStatus(VOID)
{
    RDA_PMU_CHR_STATUS_T status = CHR_STATUS_UNKNOWN;

    UINT16 ret;

    ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    PMD_TRACE(PMD_DBG_TRC, 0, "charger: charge stautus 0x%x", ret);

    if ((ret & RDA_PMU_CHR_AC_ON) == 0)
    {
        PMD_TRACE(PMD_DBG_TRC | TSTDOUT, 0, "charger: adapter off");
        status = CHR_STATUS_AC_OFF;
        pmd_RegIrqSettingClr(RDA_PMU_INT_CHR_MASK);
        g_pmdChargerInIrqEnabled = TRUE;
    }
    else
    {
        status = CHR_STATUS_AC_ON;
    }

    return status;
}

// ============================================================================
//  pmd_ChargerSetBgLpMode
// ============================================================================
PROTECTED VOID pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_ID_T user, BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdBgLpModeRegistry[user] = on;

    if (on)
    {
        // Enable BG LP mode only when all the users request so.
        for (UINT32 i=0; i<PMD_BG_LP_MODE_USER_ID_QTY; i++)
        {
            if (!g_pmdBgLpModeRegistry[i])
            {
                on = FALSE;
                break;
            }
        }
    }

    if (g_pmdBgLpModeEn != on)
    {
        g_pmdBgLpModeEn = on;

        if (on)
        {
            // Not to direct-reg (and bandgap LP mode
            // will be enabled automatically)
            g_pmdCalibSetting1 &= ~RDA_PMU_LP_MODE_BG_DR;
        }
        else
        {
            // Direct-reg to disable bandgap LP mode
            g_pmdCalibSetting1 |= RDA_PMU_LP_MODE_BG_DR;
            g_pmdCalibSetting1 &= ~RDA_PMU_LP_MODE_BG_REG;
        }
        pmd_RDAWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);
    }

    hal_SysExitCriticalSection(scStatus);
}

PROTECTED VOID pmd_PmuChargerPlugin(VOID)
{
    // Automatic chopper clock control in LP mode
    pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_CHARGER, TRUE);
    // Disable bandgap LP mode for accurate h/w charger control in LP
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_CHARGER, FALSE);
}

PROTECTED VOID pmd_PmuChargerPlugout(VOID)
{
    // Disable chopper clock in LP mode to save power
    pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_CHARGER, FALSE);
    // Enable bandgap LP mode to save power
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_CHARGER, TRUE);
}

// ============================================================================
// pmd_GetEfuseValue
// ----------------------------------------------------------------------------
/// Read efuse value of the given page.
///
// ============================================================================
PRIVATE UINT16 pmd_GetEfuseValue(PMD_EFUSE_PAGE_T page)
{
    UINT16 value;
    value = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value = PMU_SET_BITFIELD(value, RDA_PMU_EFUSE_SEL_WORD_REG, page);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);

    value |= (RDA_PMU_EFUSE_SENSE_DR|RDA_PMU_EFUSE_SENSE_REG);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);
    hal_TimDelay(2 MS_WAITING);
    value &= ~(RDA_PMU_EFUSE_SENSE_DR|RDA_PMU_EFUSE_SENSE_REG);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);

    value = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OUT);

    return value;
}

#ifdef CHIP_DIE_8809E2
// ============================================================================
// pmd_SetEfuseFor_LP
// ----------------------------------------------------------------------------
/// lp=0x11, normal=00;
///
// ============================================================================

PUBLIC VOID pmd_SetEfuseFor_LP(UINT16 lp)
{
    UINT16 value;
    value = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value = PMU_SET_BITFIELD(value, RDA_PMU_EFUSE_SEL_WORD_REG, lp);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value);

}

#endif

UINT32 pmd_GetEncryptUid()
{
    UINT32 val =(pmd_GetEfuseValue(PMD_EFUSE_PAGE_VBAT_4P2)&0x3FF)<<20;
    val +=(pmd_GetEfuseValue(PMD_EFUSE_PAGE_VBAT_3P4)&0x3FF)<<10;
    val +=(pmd_GetEfuseValue(PMD_EFUSE_PAGE_CHARGER)&0x3FF);

    return val;
}
// ============================================================================
// pmd_GetEfuseGpadc
// ----------------------------------------------------------------------------
/// Read efuse value of GPADC calib sample.
///
// ============================================================================
PRIVATE VOID pmd_GetEfuseGpadc(VOID)
{
    UINT32 high, low;
    high = pmd_GetEfuseValue(PMD_EFUSE_PAGE_VBAT_4P2);
    low = pmd_GetEfuseValue(PMD_EFUSE_PAGE_VBAT_3P4);
    if((low==0)||(high==0))
    {
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
        high = 0x2f8;
        low = 0x264;
#endif
    }

#define EFUSE_GPADC_CHECK(efuse) (((efuse) & 0x3c00) >> 10)
#define EFUSE_GPADC_VALUE(efuse) \
    ( ( (efuse) | \
        ( (EFUSE_GPADC_CHECK(efuse) == 0) ? 0 : \
          (1 << (EFUSE_GPADC_CHECK(efuse) - 1)) \
        ) \
      ) & 0x3ff \
    )

    high = EFUSE_GPADC_VALUE(high);
    low = EFUSE_GPADC_VALUE(low);

    hal_AnaGpadcSetCalibData(low, high);
}

PRIVATE VOID pmd_EnableLowBattVoltShutdown(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (on)
    {
        // uvSel->voltage: 0->2.6V, 1->2.7V, 2->2.9V, 3->3.0V
        UINT32 uvSel = 0x3;
        g_pmdChargerSetting1 = PMU_SET_BITFIELD(g_pmdChargerSetting1,
                                                RDA_PMU_UV_SEL,
                                                uvSel);
        pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, g_pmdChargerSetting1);
        // Enable vbat 3.2V detection
        g_pmdCalibSetting1 &= ~RDA_PMU_VBAT_OVER_3P2_BYPASS;
    }
    else
    {
        // Disable Vbat 3.2V check
        g_pmdCalibSetting1 |= RDA_PMU_VBAT_OVER_3P2_BYPASS;
    }
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING1, g_pmdCalibSetting1);
    hal_SysExitCriticalSection(scStatus);
}

#ifdef CHARGER_BATTERY_CHECK_BY_CRASH

PRIVATE BOOL pmd_GetBattConnStatusByCrash(VOID)
{
#ifdef NO_CHARGER_BATTERY_CHECK
    return TRUE;
#endif
    // If battery is not in place, the system will crash soon after
    // the charger current is cut off.
    // Since the charging is disabled, the system will lose all the power gradually,
    // and all the h/w setting will be restored to the default values.
    // Then the system will be booted up again, for there is a charger connected.
    // This senario will repeat again and again, and at last the system might
    // enter a stable state with a small current leakage.

#define CHARGER_BATTERY_CHECKING_TIME (100 MS_WAITING)

    // Disable charging
    pmd_StopCharging(TRUE);
    // Check if the system will crash during the following wait
    hal_TimDelay(CHARGER_BATTERY_CHECKING_TIME);
    // There is a battery. Enable charing again
    pmd_StopCharging(FALSE);

    return TRUE;
}

#else // !CHARGER_BATTERY_CHECK_BY_CRASH

PRIVATE BOOL pmd_GetBattConnStatusByVfb(VOID)
{
#ifdef NO_CHARGER_BATTERY_CHECK
    return TRUE;
#endif

    // PREREQUISITE: Charger current is no larger than 50 mA.

    // If battery is not in place, the battery voltage will change quickly
    // according to the VFB setting, and its value will equal the charing
    // cut-off voltage of the VFB.
    // The max VFB cut-off voltage should be larger than 4200 mV, and
    // the delta of cut-off voltage between max VFB and min VFB should
    // be larger than 200 mV.
    // A small charger current can avoid the measurement error introduced
    // by battery internal resistor.
    // When battery is not detected, the system will be continously rebooted.

#define VFB_VOLT_STABLE_TIME (50 MS_WAITING)

#define MAX_VFB_VOLT_MV 4200
#define MAX_MIN_VFB_VOLT_DELTA_MV 300

#define BATT_VOLT_SAMPLE_NUM 500

    UINT32 vfbSetting = g_pmdChargerSetting2;
    UINT32 maxVfbVolt, minVfbVolt;
    BOOL batteryInPlace = TRUE;

    // Set low gain to reduce the fluctuation of the battery voltage
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1,
                     (g_pmdChargerSetting1 | RDA_PMU_CHR_CV_LOWGAIN));

    // Min VFB value
    pmd_SetChargerVfb(PMD_CHARGER_VFB_MIN);
    hal_TimDelay(VFB_VOLT_STABLE_TIME);

    // If battery is not in place, the battery voltage might wave like
    // a sine signal and the period is about 400 us
    maxVfbVolt = pmd_GetAverageGpadcBatteryLevel(BATT_VOLT_SAMPLE_NUM);
    if (maxVfbVolt > MAX_VFB_VOLT_MV)
    {
        // Max VFB value
        pmd_SetChargerVfb(PMD_CHARGER_VFB_MAX);
        hal_TimDelay(VFB_VOLT_STABLE_TIME);

        // If battery is not in place, the battery voltage might wave like
        // a sine signal and the period is about 400 us
        minVfbVolt = pmd_GetAverageGpadcBatteryLevel(BATT_VOLT_SAMPLE_NUM);
        if (maxVfbVolt > minVfbVolt + MAX_MIN_VFB_VOLT_DELTA_MV)
        {
            // Battery is not in place
            batteryInPlace = FALSE;
        }
    }

    // Restore the original charger setting
    g_pmdChargerSetting2 = vfbSetting;
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
    // Restore the original low gain setting
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, g_pmdChargerSetting1);

    return batteryInPlace;
}

#endif // !CHARGER_BATTERY_CHECK_BY_CRASH

PRIVATE VOID pmd_ChargerBatteryDetection(VOID)
{
#define BATT_DETECT_RETRY_TIME (500 MS_WAITING)
    UINT32 POSSIBLY_UNUSED i = 0;

    while (1)
    {
#ifdef CHARGER_BATTERY_CHECK_BY_CRASH
        if (pmd_GetBattConnStatusByCrash())
        {
            break;
        }
#else // !CHARGER_BATTERY_CHECK_BY_CRASH
        if (pmd_GetBattConnStatusByVfb())
        {
            break;
        }
        else
        {
#ifdef SHUTDOWN_FOR_CHARGER_WITHOUT_BATTERY
            // Shutdown the system.
            // As there is a charger connected, the system will be booted up soon.
            hal_HstSendEvent(0xdeadc6a7);
            hal_SysShutdown();
#else
            hal_TimDelay(BATT_DETECT_RETRY_TIME);
            if (i == 0)
            {
                hal_HstSendEvent(0xba44dec4);
            }
            if (i++ > 60)
            {
                i = 0;
            }
#endif
        }
#endif // !CHARGER_BATTERY_CHECK_BY_CRASH
    }
}

PROTECTED RDA_PMU_CHR_STATUS_T pmd_PmuInitCharger(VOID)
{
    // Init BG LP mode registry (default on)
    for (UINT32 i=0; i<PMD_BG_LP_MODE_USER_ID_QTY; i++)
    {
        g_pmdBgLpModeRegistry[i] = TRUE;
    }

    // Allow GPADC channel 6 to monitor the charger voltage inside chip
    g_pmdChargerSetting1 = PMU_SET_BITFIELD(g_pmdChargerSetting1,
                                            RDA_PMU_CHR_AUXADC_SEL,
                                            0);
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING1, g_pmdChargerSetting1);

    // Init charger control (to disable bat_monitor)
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_CONTROL, g_pmdChargerControl);

    pmd_GetEfuseGpadc();
    // Voltages can be measured after clk_4m, ldo_avdd3, sar_vref_bit
    // and GPADC efuse are ready
    pmd_UsbDownloadByMicDetect();
#if 1
    // Enable Vbat 3.2V check
    pmd_EnableLowBattVoltShutdown(TRUE);
#else
    // Disable Vbat 3.2V check
    pmd_EnableLowBattVoltShutdown(FALSE);
#endif

    // Set efuse
    UINT32 efuse = pmd_GetEfuseValue(PMD_EFUSE_PAGE_CHARGER);
    UINT32 chr_pre_ibit = PMU_GET_BITFIELD(efuse, RDA_PMU_CHR_PRE_IBIT_REG);
    UINT32 chr_vfb_sel = PMU_GET_BITFIELD(efuse, RDA_PMU_CHR_VFB_SEL_REG);
#if 0
    // Lower EFUSE_CHR_VFB_SEL by 2 to increase the hw target charge voltage,
    // while not enlarging the charge current too much.
    // This is safe as sw will stop charging if the battery voltage is over 4.2V.
    chr_vfb_sel = (chr_vfb_sel<=2) ? 0 : chr_vfb_sel-2;
#else
    // Force EFUSE_CHR_VFB_SEL to max value to avoid over charge
    chr_vfb_sel = CHR_VFB_SEL_MAX_VAL;
#endif
    g_pmdChargerSetting2 = PMU_SET_BITFIELD(g_pmdChargerSetting2,
                                            RDA_PMU_CHR_PRE_IBIT_REG,
                                            chr_pre_ibit) |
                           RDA_PMU_CHR_PRE_IBIT_DR;
    g_pmdChargerSetting2 = PMU_SET_BITFIELD(g_pmdChargerSetting2,
                                            RDA_PMU_CHR_VFB_SEL_REG,
                                            chr_vfb_sel) |
                           RDA_PMU_CHR_VFB_SEL_DR;
    pmd_OpalSpiWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
    // End of efuse setting

    UINT16 ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    if ((ret & RDA_PMU_CHR_AC_ON) != 0)
    {
        // Detect whether battery is in place
        pmd_ChargerBatteryDetection();
        // Battery is in place. Go on ...

        // PMD IRQ setting register has been initialized in pmd_InitRdaPmu()
        pmd_RegIrqSettingSet(RDA_PMU_INT_CHR_MASK|RDA_PMU_INT_CHR_CLEAR);
        g_pmdChargerInIrqEnabled = FALSE;

        return CHR_STATUS_AC_ON;
    }
    else
    {
        return CHR_STATUS_AC_OFF;
    }
}

PUBLIC VOID pmd_RestoreChargerAtPowerOff(VOID)
{
    // Set charge current to lowest to avoid the damage of P-channel MOSFET
    // when the charger voltage is too high
    pmd_SetChargeCurrent(PMD_CHARGER_50MA);
    // Set VFB to max to avoid over charge
    pmd_SetChargerVfb(PMD_CHARGER_VFB_MAX);
    // Restore charge control setting (especailly for precharging)
    pmd_RestoreChargerControl();
    // Enable vbat 3.2V detection
    pmd_EnableLowBattVoltShutdown(TRUE);
}

PROTECTED BOOL pmd_SetChargerVfb(PMD_CHARGER_VFB_T vfb)
{
    UINT32 chr_vfb_sel;
    BOOL skip = FALSE;
    UINT32 scStatus = hal_SysEnterCriticalSection();

    if (vfb == PMD_CHARGER_VFB_MIN)
    {
        chr_vfb_sel = CHR_VFB_SEL_MIN_VAL;
    }
    else if (vfb == PMD_CHARGER_VFB_MAX)
    {
        chr_vfb_sel = CHR_VFB_SEL_MAX_VAL;
    }
    else
    {
        chr_vfb_sel = PMU_GET_BITFIELD(g_pmdChargerSetting2, RDA_PMU_CHR_VFB_SEL_REG);
        if (vfb == PMD_CHARGER_VFB_DEC)
        {
            if (chr_vfb_sel == CHR_VFB_SEL_MIN_VAL)
            {
                skip = TRUE;
            }
            else
            {
                --chr_vfb_sel;
            }
        }
        else
        {
            if (chr_vfb_sel == CHR_VFB_SEL_MAX_VAL)
            {
                skip = TRUE;
            }
            else
            {
                ++chr_vfb_sel;
            }
        }
    }

    if (!skip)
    {
        g_pmdChargerSetting2 = PMU_SET_BITFIELD(g_pmdChargerSetting2,
                                                RDA_PMU_CHR_VFB_SEL_REG,
                                                chr_vfb_sel) |
                               RDA_PMU_CHR_VFB_SEL_DR;
        pmd_RDAWrite(RDA_ADDR_CHARGER_SETTING2, g_pmdChargerSetting2);
        PMD_TRACE(PMD_INFO_TRC, 0, "chr_vfb_sel=%d", chr_vfb_sel);
    }

    hal_SysExitCriticalSection(scStatus);
    return !skip;
}

PROTECTED VOID pmd_ChargerSmallCurrent(BOOL enable)
{
    PMD_CHARGE_CURRENT_T current;
    if (enable)
    {

#ifdef CHIP_DIE_8809E2
        current = PMD_CHARGER_50MA;
#else
        current = PMD_CHARGER_100MA;
#endif
    }
    else
    {
        current = g_pmdConfig->batteryChargeCurrent;
    }
    pmd_SetChargeCurrent(current);
}

PROTECTED INT16 pmd_GetHighActivityVoltOffset(VOID)
{
    INT16 offset;
    UINT32 chr_vfb_sel;
    UINT32 scStatus = hal_SysEnterCriticalSection();

    if (pmd_GetHighActivity())
    {
        if (pmd_GetChargingStage() == PMD_CHARGING_STAGE_ON)
        {
            chr_vfb_sel = PMU_GET_BITFIELD(g_pmdChargerSetting2, RDA_PMU_CHR_VFB_SEL_REG);
            if (chr_vfb_sel > CHR_VFB_SEL_MIN_VAL)
            {
                offset = g_pmdConfig->batteryOffsetHighActivityMV >>
                         (chr_vfb_sel - CHR_VFB_SEL_MIN_VAL);
            }
            else
            {
                offset = g_pmdConfig->batteryOffsetHighActivityMV * 7 / 8;
            }
        }
        else
        {
            offset = g_pmdConfig->batteryOffsetHighActivityMV;
        }
    }
    else
    {
        offset = 0;
    }

    hal_SysExitCriticalSection(scStatus);

    return offset;
}

PROTECTED CONST PMD_CHARGER_PROC_CFG_T gc_pmdChargerProcCfg =
{
    .battOffsetVfbUpperLimit = 20, //2016.05.27 Yayuan  30 --> 0, for dynamic limit
    .battOffsetVfbLowerLimit = -25, //2016.05.27 Yayuan  -20 --> -45, for dynamic limit
    .battOffsetVfbLowerLimitSmallCurrent = 25,
    .highActVfbWindowOffset = 30,
    .battOffsetSmallCurrent = 5,    //2016.05.27 Yayuan -20 -->  -15
    .intervalBattLevelCheck = (10 MINUTES),
    //******************************************************//
    //for BatteryLevelChargeTermMV vary from 4220 --> 4200,         //
    //the actual charge terminal voltage expected to be unchanged,  //
    //so, offsets need to be adjusted(except highActVfbWindowOffset)    //
    //before: 0, -45   5, 30, -15                                   //
    //now:   20, -25, 25, 30,   5                                   //
    //******************************************************//


};

PROTECTED CONST PMD_CHARGER_PROC_OPS_T gc_pmdChargerProcOps =
{
    .setChargerCurrent = pmd_PmuSetChargeCurrent,
    .chargerPlugin = pmd_PmuChargerPlugin,
    .chargerPlugout = pmd_PmuChargerPlugout,
    .initCharger = pmd_PmuInitCharger,
    .stopCharging = pmd_ForceChargerFinished,
};


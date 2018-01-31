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
#include "pmd_map.h"
#include "pmdp_debug.h"
#include "pmdp_charger.h"
#include "pmdp.h"
#include "drv_dualsim.h"

// Opal hardware
#include "opal.h"

// hal drivers
#include "hal_ana_gpadc.h"
#include "hal_pwm.h"
#include "hal_ispi.h"
#include "hal_lps.h"
#include "hal_timers.h"
#include "hal_sys.h"
#include "ddr_config.h"
#include "memd_m.h"
#include "hal_sim.h"
#include "hal_rda_abb.h"
#include "hal_rda_audio.h"
#include "global_macros.h"
#include "cfg_regs.h"
#include "spi.h"

// sx
#include "sxr_tls.h"
#include "sxr_tim.h"
#include "sxs_idle.h"
#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
#include "sys_ctrl.h"
#endif
#include "boot_map.h"
#include "tgt_ddr_cfg.h"

#ifdef VMIC_POWER_ON_WORKAROUND
extern VOID aud_InitEarpieceSetting(VOID);
extern VOID aud_EnableEarpieceSetting(BOOL enable);
#endif // VMIC_POWER_ON_WORKAROUND

extern VOID hal_AudForceDcdc4ClockOn(BOOL on);

#ifdef __PRJ_WITH_SPIFLSH__
extern VOID memd_FlashWakeup(VOID);
extern VOID memd_FlashSleep(VOID);
#endif

extern VOID hal_ConfigFmIo(BOOL enable);
extern VOID hal_ConfigBtIo(BOOL enable);

#ifdef SUPPORT_QUICK_VCOM
VOID pmd_VcomGpioInit(HAL_GPIO_GPIO_ID_T gpioId);
#endif
// =============================================================================
//  DEFINES
// =============================================================================

#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef SPI_REG_DEBUG
#endif

#define PMD_FUNC_INTERNAL   __attribute__((section(".sramtext")))
#define PMD_DATA_INTERNAL   __attribute__((section(".sramdata")))

#define ENABLE_KEYPAD_LIGHT_PWM 0
#define KEYPAD_LIGHT_PWM_BY_PMU 1

#if (ENABLE_KEYPAD_LIGHT_PWM)
#warning "ENABLE_KEYPAD_LIGHT_PWM might interfere with speaker PA"
#endif

#define CONFIG_CLASS_K_SPEAKER_PA 0

#define DYNAMIC_DCDC_VCORE_ADJUSTMENT 1

// Whether left speaker line is in class AB mode
// If 0, the line is in class D or K mode
#ifndef AUD_SPK_LINE_L_CLASS_AB
#define AUD_SPK_LINE_L_CLASS_AB 0
#endif

// Whether right speaker line is in class AB mode
// If 0, the line is in class D or K mode
#ifndef AUD_SPK_LINE_R_CLASS_AB
#define AUD_SPK_LINE_R_CLASS_AB 0
#endif

#if ( AUD_SPK_LINE_NO_CLASS_K == 0)
#error "Bad config: don't support class k mode"
#endif


#ifdef RECV_USE_SPK_LINE_R
#error "Bad config: The  R line only use to SPK, can't use to RECV"
#endif


#if (AUD_SPK_LINE_L_ONLY || AUD_SPK_LINE_R_ONLY)
#error "Bad config: defined with L (or R) only mode, 8955 only suppot spk in line R and recv in line L"
#endif

#if (AUD_SPK_LINE_L_R_DOUBLE) && \
    (AUD_SPK_LINE_L_CLASS_AB) && (AUD_SPK_LINE_R_CLASS_AB)
#error "Bad config: L&R double mode defined with class ab mode"
#endif

// Whether to enable charge pump for LCD backlight
#if  (LCD_BACKLIGHT_CHARGE_PUMP)
#error "Bad config: don't support the charge pump"
#endif



#define PMD_TIME_IRQ_ACT_CS_RETRY (2 MS_WAITING)
#define PMD_TIME_IRQ_DEBOUNCE (100 MS_WAITING)

#define PMD_IRQ_DEBOUNCE_REQ (1<<0)
#define PMD_IRQ_CHECK_CHARGER (1<<1)
#define PMD_IRQ_CHECK_HP (1<<2)

#define POWER_LDO_USER(id) (1<<id)
#define LEVEL_LDO_USER(id) (1<<(id+PMD_POWER_ID_QTY))
#define DEFAULT_LDO_USER   (1<<31)

#define PMD_TIME_HP_DETECT_STABLE (1200 MS_WAITING)

// VMEM level
#define PMD_VMEM_LEVEL                  7

// CAUTION: for 8955, DDR PSRAM is very sensitive to vcore. During vcore change,
// DDR PSRAM access will be wrong before correct timing is written.
//
// Now, we will use a fixed high enough vcore to avoid dynamic vcore changes.
// And pay attention to choose closer LDO and DCDC voltage

// LDO vcore value settings
#define PMD_LDO_VCORE_VALUE_LP          9
#define PMD_LDO_VCORE_VALUE_LOW         7
#define PMD_LDO_VCORE_VALUE_MEDIUM      6
#define PMD_LDO_VCORE_VALUE_HIGH        6
#define PMD_LDO_VCORE_VALUE_BTTEST      6

#ifdef CHIP_DIE_8955_U01
// DCDC vcore value settings
#define PMD_DCDC_VCORE_VALUE_LP         2
#define PMD_DCDC_VCORE_VALUE_LOW        6
#define PMD_DCDC_VCORE_VALUE_MEDIUM     8
#define PMD_DCDC_VCORE_VALUE_HIGH       8
#define PMD_DCDC_VCORE_VALUE_BTTEST     8
#else
// DCDC vcore value settings
//10-1.252V,6-1.151V,4-1.102
#define PMD_DCDC_VCORE_VALUE_LP         2
#define PMD_DCDC_VCORE_VALUE_LOW        6
#define PMD_DCDC_VCORE_VALUE_MEDIUM     12
#define PMD_DCDC_VCORE_VALUE_HIGH       12
#define PMD_DCDC_VCORE_VALUE_BTTEST     12
#endif //CHIP_DIE_8955_U01

#define PMD_CORE_VOLTAGE_INIT           PMD_CORE_VOLTAGE_MEDIUM
#define PMD_LDO_VCORE_VALUE_INIT        PMD_LDO_VCORE_VALUE_MEDIUM
#define PMD_DCDC_VCORE_VALUE_INIT       PMD_DCDC_VCORE_VALUE_MEDIUM

//
// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_LDO_SETTINGS_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct
    {
        BOOL fmEnable:1;
        BOOL btEnable:1;
        BOOL keyEnable:1;
        BOOL tscEnable:1;
        BOOL vLcdEnable:1;
        BOOL vCamEnable:1;
        BOOL vMicEnable:1;
        BOOL vIbrEnable:1;
        UINT16 :1;
        BOOL vRfEnable:1;
        BOOL vAbbEnable:1;
        BOOL vMmcEnable:1;
        BOOL vSim4Enable:1;
        BOOL vSim3Enable:1;
        BOOL vSim2Enable:1;
        BOOL vSim1Enable:1;
    };
    UINT16 reg;
} PMD_LDO_SETTINGS_T;

// =============================================================================
// PMD_LDO_PROFILE_SETTING1_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct
    {
        BOOL normalMode:1;
        BOOL vSpiMemOff:1;
        // The hw bit for vBlLedOff is useless.
        // Sw should set rgbLedOff bit in LDO setting2 register instead.
        BOOL vBlLedOff:1;
        BOOL vMicOff:1;
        BOOL vUsbOff:1;
        BOOL vIbrOff:1;
        BOOL vMmcOff:1;
        BOOL vLcdOff:1;
        BOOL vCamOff:1;
        BOOL vRfOff:1;
        BOOL vAbbOff:1;
        BOOL vPadOff:1;
        BOOL vMemOff:1;
        BOOL buck1LdoOff:1;
        BOOL buck1PfmOn:1;
        BOOL buck1On:1;
    };
    UINT16 reg;
} PMD_LDO_PROFILE_SETTING1_T;

// =============================================================================
// PMD_LDO_PROFILE_SETTING2_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct
    {
        BOOL rgbLedOff:1;
        UINT16 :1;
        BOOL buck2LdoOff:1;
        BOOL buck2PfmOn:1;
        BOOL buck2On:1;
        UINT16 :1;
        BOOL vIntRfOff:1;
        BOOL vIbrIs1_8:1;
        BOOL vMmcIs1_8:1;
        BOOL vLcdIs1_8:1;
        BOOL vCamIs1_8:1;
        BOOL vRfIs1_8:1;
        BOOL vPadIs1_8:1;
        UINT16 vRtcVbit:3;
    };
    UINT16 reg;
} PMD_LDO_PROFILE_SETTING2_T;

// =============================================================================
// PMD_LDO_PROFILE_SETTING5_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct
    {
        // NOTE:
        //   vRgbLed & vAbb bits are located in RDA_ADDR_LDO_ACTIVE_SETTING5
        //   only, and they are valid for all profiles
        UINT16 vRgbLedIbit:3;
        UINT16 vRgbLedVsel:3;
        UINT16 vAbbIbit:3;
        // End of NOTE
        UINT16 vUsbIbit:3;
        BOOL vSim4Is1_8:1;
        BOOL vSim3Is1_8:1;
        BOOL vSim2Is1_8:1;
        BOOL vSim1Is1_8:1;
    };
    UINT16 reg;
} PMD_LDO_PROFILE_SETTING5_T;

// =============================================================================
// PMD_BL_LED_OUT_SETTING_T
// -----------------------------------------------------------------------------
/// BL_LED_OUT setting
// =============================================================================
typedef struct
{
    UINT8 vRgbLedIbit:3;
    UINT8 vRgbLedVsel:3;
} PMD_BL_LED_OUT_SETTING_T;

// =============================================================================
// PMD_CHANGE_PARAM_T
// -----------------------------------------------------------------------------
/// Structure for PMU registers writing
// =============================================================================
typedef struct
{
    UINT16  reg_count;
    UINT16  reg_idx[5];
    UINT16  reg_data[5];
    UINT16  delay_count;
} PMD_CHANGE_PARAM_T;

// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// Whether charger in IRQ is enabled
PROTECTED BOOL g_pmdChargerInIrqEnabled = TRUE;

#ifdef ABB_HP_DETECT
// Whether headphone out IRQ is enabled
PRIVATE BOOL g_pmdHpOutIrqEnabled = FALSE;

// Whether headphone out detect is started
PRIVATE BOOL g_pmdHpOutDetectStarted = FALSE;
#endif

//
PROTECTED BOOL g_pmdMuteSpk = FALSE;

//  RDA_ADDR_LDO_SETTINGS = 0x02
PRIVATE PMD_LDO_SETTINGS_T g_pmdLdoSettings =
{
    {
        .fmEnable = FALSE,
        .btEnable = FALSE,
        .keyEnable = FALSE,
        .tscEnable = FALSE,
        .vLcdEnable = TRUE,
        .vCamEnable = TRUE,
        .vMicEnable = TRUE,
        .vIbrEnable = TRUE,
        .vRfEnable = TRUE,
        .vAbbEnable = TRUE,
        .vMmcEnable = TRUE,
        .vSim4Enable = FALSE,
        .vSim3Enable = FALSE,
        .vSim2Enable = FALSE,
        .vSim1Enable = FALSE,
    }
};

//  RDA_ADDR_LDO_ACTIVE_SETTING1 = 0x03
//  RDA_ADDR_LDO_LP_SETTING1 = 0x08
PRIVATE PMD_LDO_PROFILE_SETTING1_T HAL_BOOT_DATA_INTERNAL g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_QTY] =
{
    {   {
            // PMD_PROFILE_MODE_NORMAL
            .normalMode = TRUE,
            .vSpiMemOff = FALSE,
            // The hw bit for vBlLedOff is useless.
            // Sw should set rgbLedOff bit in LDO setting2 register instead.
            .vBlLedOff = FALSE,
            .vMicOff = TRUE,
            // vMem is derived from vUsb, so vUsb should be always on
            .vUsbOff = FALSE,
            .vIbrOff = TRUE,
            .vMmcOff = TRUE,
            .vLcdOff = TRUE,
            .vCamOff = TRUE,
            .vRfOff = TRUE,
            .vAbbOff = TRUE,
            .vPadOff = FALSE,
            .vMemOff = FALSE,
            .buck1LdoOff = FALSE,
            .buck1PfmOn = FALSE,
            .buck1On = FALSE,
        }
    },
    {   {
            // PMD_PROFILE_MODE_LOWPOWER
            .normalMode = FALSE,
            .vSpiMemOff = FALSE,
            // The hw bit for vBlLedOff is useless.
            // Sw should set rgbLedOff bit in LDO setting2 register instead.
            .vBlLedOff = TRUE,
            .vMicOff = TRUE,
            // vMem is derived from vUsb, so vUsb should be always on
            .vUsbOff = FALSE,
            .vIbrOff = TRUE,
            .vMmcOff = TRUE,
            .vLcdOff = TRUE,
            .vCamOff = TRUE,
            .vRfOff = TRUE,
            .vAbbOff = TRUE,
            .vPadOff = FALSE,
            .vMemOff = FALSE,
            .buck1LdoOff = FALSE,
            .buck1PfmOn = FALSE,
            .buck1On = FALSE,
        }
    },
};


//  RDA_ADDR_LDO_ACTIVE_SETTING2 = 0x04
//  RDA_ADDR_LDO_LP_SETTING2 = 0x09
PRIVATE PMD_LDO_PROFILE_SETTING2_T g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_QTY] =
{
    {   {
            // PMD_PROFILE_MODE_NORMAL
            .rgbLedOff = FALSE,//NC
            .buck2LdoOff = FALSE,//NC
            .buck2PfmOn = FALSE,//NC
            .buck2On = TRUE,
            .vIntRfOff = FALSE,
            .vIbrIs1_8 = FALSE,
            .vMmcIs1_8 = TRUE,
            .vLcdIs1_8 = TRUE,
            .vCamIs1_8 = TRUE,
            .vRfIs1_8 = FALSE,
            .vPadIs1_8 = FALSE,
            .vRtcVbit = 4,
        }
    },
    {   {
            // PMD_PROFILE_MODE_LOWPOWER
            .rgbLedOff = FALSE,//NC
            .buck2LdoOff = FALSE,//NC
            .buck2PfmOn = FALSE,//NC
            .buck2On = FALSE,
            .vIntRfOff = TRUE,
            .vIbrIs1_8 = FALSE,
            .vMmcIs1_8 = TRUE,
            .vLcdIs1_8 = TRUE,
            .vCamIs1_8 = TRUE,
            .vRfIs1_8 = FALSE,
            .vPadIs1_8 = FALSE,
            .vRtcVbit = 0,
        }
    },
};

// Pad level
PRIVATE CONST UINT8 HAL_RODATA_INTERNAL gc_pmdPadLevelAct = 7;
PRIVATE CONST UINT8 HAL_RODATA_INTERNAL gc_pmdPadLevelLp = 0;

//  RDA_ADDR_LDO_ACTIVE_SETTING3 = 0x05
//  RDA_ADDR_LDO_LP_SETTING3 = 0x0a
PRIVATE UINT16 HAL_BOOT_DATA_INTERNAL g_pmdLdoProfileSetting3 =
    RDA_PMU_VBACKUP_VBIT(4)|
    RDA_PMU_VSPIMEM_IBIT(7)|
    RDA_PMU_VMEM_IBIT(4)|
    RDA_PMU_VPAD_IBIT(4);

//  RDA_ADDR_LDO_ACTIVE_SETTING4 = 0x06
PRIVATE UINT16 g_pmdLdoActiveSetting4 =
    RDA_PMU_VRF_IBIT(4)|
    RDA_PMU_VCAM_IBIT(4)|
    RDA_PMU_VLCD_IBIT(4)|
    RDA_PMU_VMMC_IBIT(4)|
    RDA_PMU_VIBR_IBIT(4);

//  RDA_ADDR_LDO_ACTIVE_SETTING5 = 0x07
//  RDA_ADDR_LDO_LP_SETTING5 = 0x0c
PRIVATE PMD_LDO_PROFILE_SETTING5_T g_pmdLdoProfileSetting5 =
{
    {
        // NOTE:
        //   vRgbLed & vAbb bits are located in RDA_ADDR_LDO_ACTIVE_SETTING5
        //   only, and they are valid for all profiles
        .vRgbLedIbit = 4,
        .vRgbLedVsel = 4,
#ifdef ABB_HP_DETECT
        // Workaround:
        // Headphone detection will encounter errors if vAbbIbit>=3
        .vAbbIbit = 2,
#else
        .vAbbIbit = 4,
#endif
        // End of NOTE
        .vUsbIbit = 4,
        .vSim4Is1_8 = TRUE,
        .vSim3Is1_8 = TRUE,
        .vSim2Is1_8 = TRUE,
        .vSim1Is1_8 = TRUE,
    }
};

// VCORE values
PRIVATE UINT8 g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_QTY];
// CCORE valuse of LDO mode
PRIVATE UINT8 g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_QTY];

// vcore staff
PRIVATE UINT8 g_pmdVcoreActRegistry[PMD_CORE_VOLTAGE_USER_ID_QTY];
PRIVATE UINT8 HAL_BOOT_BSS_INTERNAL g_pmdVcoreCurrIndex;
PRIVATE BOOL  HAL_BOOT_BSS_INTERNAL g_pmdVcoreLdoMode;
PRIVATE UINT8 HAL_BOOT_BSS_INTERNAL g_pmdVcoreVoltage;


PRIVATE UINT32 g_pmdLdoUserBitmap[PMD_PROFILE_MODE_QTY][PMD_LDO_QTY];

// RDA_ADDR_CALIBRATION_SETTING2 = 0x17
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdCalibSetting2 =
    RDA_PMU_TS_I_CTRL_BATTERY(8);

// RDA_ADDR_MISC_CONTROL = 0x18
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdMiscControlActive =
    RDA_PMU_CLK2M_FTUN_BIT(4)|
    RDA_PMU_LDO_AVDD3_BIT(4);

PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdMiscControlLp =
    RDA_PMU_PU_CLK_4M_DR|
    RDA_PMU_PD_LDO_AVDD3_DR|
    RDA_PMU_PD_LDO_AVDD3_REG|
    RDA_PMU_CLK2M_FTUN_BIT(4)|
    RDA_PMU_LDO_AVDD3_BIT(4);

// RDA_ADDR_LED_SETTING1 = 0x19
// Disable backlight PWM (0)
PRIVATE UINT16 g_pmdLedSetting1 =
#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
// keypad light PWM controlled by PMU
    RDA_PMU_PWM_RGB_PMU_MODE|
    RDA_PMU_PWM_RGB_FREQ(8)|
#else
// keypad light PWM controlled by baseband
#endif
#else // !ENABLE_KEYPAD_LIGHT_PWM
// keypad light always on (0)
    RDA_PMU_DIM_LED_R_DR|
    RDA_PMU_DIM_LED_G_DR|
    RDA_PMU_DIM_LED_B_DR|
#endif // !ENABLE_KEYPAD_LIGHT_PWM
// direct LCD backlight control, LCD backlight on (0)
    RDA_PMU_DIM_BL_DR;

// RDA_ADDR_LED_SETTING2 = 0x1a
PRIVATE UINT16 g_pmdLedSetting2 =
    RDA_PMU_BL_IBIT_PON(8)|
    RDA_PMU_BL_IBIT_ACT(8)|
    RDA_PMU_BL_IBIT_LP(8)|
    RDA_PMU_BL_OFF_PON|
    RDA_PMU_BL_OFF_ACT|
    RDA_PMU_BL_OFF_LP;

// RDA_ADDR_LED_SETTING3 = 0x1b
//PRIVATE UINT16 g_pmdLedSetting3 =
//                    RDA_PMU_PWM_BL_DUTY_CYCLE(16)|
//                    RDA_PMU_PWM_BL_FREQ(8);

// SIM Selection
PRIVATE UINT16 g_pmdSimSelect = 0;

// RDA_ADDR_SPEAKER_PA_SETTING1 = 0x27
// CP (charge pump) control configuration
// CP direct-reg controls will be initialized in pmd_SpeakerPaRestoreMode
PRIVATE UINT16 g_pmdSpeakerPaSetting1 =
    RDA_PMU_DELAY_BIT_CP(3)|
    RDA_PMU_VCP_OUT_BIT(0);

// RDA_ADDR_LDO_BUCK1 = 0x2d
PRIVATE CONST UINT16 gc_pmdLdoBuck1Act =
    RDA_PMU_VBUCK_LDO_BIT(4)|
    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
    RDA_PMU_PFM_THRESHOLD(2)|
    RDA_PMU_OSC_FREQ(2)|
    RDA_PMU_LOW_SENSE|
    RDA_PMU_DISCHARGE_EN|
    RDA_PMU_HEAVY_LOAD;


PRIVATE CONST UINT16 gc_pmdLdoBuck1Lp =
    RDA_PMU_VBUCK_LDO_BIT(7)| //vore in LDO mode,0-1.5v,4-1.4,7-1.23
    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
    RDA_PMU_PFM_THRESHOLD(2)|
    RDA_PMU_OSC_FREQ(0)|
    RDA_PMU_DISCHARGE_EN;

// RDA_ADDR_LDO_BUCK2 = 0x2e
PRIVATE CONST UINT16 gc_pmdLdoBuck2 =
    RDA_PMU_VBUCK_LDO_BIT(0)|
    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
    RDA_PMU_PFM_THRESHOLD(2)|
    RDA_PMU_OSC_FREQ(2)|
    RDA_PMU_LOW_SENSE|
    RDA_PMU_DISCHARGE_EN|
    RDA_PMU_HEAVY_LOAD;

// RDA_ADDR_DCDC_BUCK = 0x2f
// TODO: BT requires VBUCK1_NLP/LP=8,VBUCK2_NLP/LP=9 on U02 chips.
//       This might be skipped for U04 or later chips.

PRIVATE UINT16 HAL_BOOT_DATA_INTERNAL g_pmdDcdcBuck =
    RDA_PMU_VBUCK1_BIT_NLP(8)|
    RDA_PMU_VBUCK1_BIT_LP(8)|
    RDA_PMU_VBUCK2_BIT_NLP(7)|
    RDA_PMU_VBUCK2_BIT_LP(7);




// RDA_ADDR_THERMAL_CALIBRATION = 0x36
PRIVATE UINT16 g_pmdThermalCalib =
    RDA_PMU_CHR_CALMODE_EN_BG_REG|
    RDA_PMU_CHR_CALMODE_EN_NONCAL|
    RDA_PMU_CALMODE_EN_BG_CAL|
    RDA_PMU_THERMO_SEL(3)|
    RDA_PMU_PU_THERMO_PROTECT|
    RDA_PMU_PD_VD_LDO;

// RDA_ADDR_DEBUG3 = 0x37
PRIVATE UINT16 g_pmdDebug3 =
    RDA_PMU_OVER_TEMP_BYPASS|
    RDA_PMU_RESETN_BT_TIME_SEL(2)|
    RDA_PMU_RESETN_FM_TIME_SEL(2);

#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
// RDA_ADDR_LED_SETTING4 = 0x38
PRIVATE UINT16 g_pmdLedSetting4;
#endif
#endif

// RDA_ADDR_GPADC2 = 0x3b
PRIVATE UINT16 g_pmdGpadc2 =
    RDA_PMU_GPADC_START_TIME(4)|
    RDA_PMU_GPADC_VREF_BIT(2);

// RDA_ADDR_LED_SETTING5 = 0x3e
PRIVATE UINT16 g_pmdLedSetting5 =
    RDA_PMU_GBIT_ABB_EN|
    RDA_PMU_LED_R_OFF_ACT|
    RDA_PMU_LED_G_OFF_ACT|
    RDA_PMU_LED_B_OFF_ACT|
    RDA_PMU_LED_R_OFF_LP|
    RDA_PMU_LED_G_OFF_LP|
    RDA_PMU_LED_B_OFF_LP|
    RDA_PMU_LED_R_IBIT(0)|
    RDA_PMU_LED_G_IBIT(0)|
    RDA_PMU_LED_B_IBIT(0);

// RDA_ADDR_SPEAKER_PA_SETTING2 = 0x40
PRIVATE CONST UINT16 gc_pmdSpeakerPaSetting2 =
    RDA_PMU_VCLG_BIT(2)|
    RDA_PMU_VNCN_BIT(2)|
    RDA_PMU_I_BIT_CLG_REG(3);

// RDA_ADDR_SPEAKER_PA_SETTING3 = 0x41
// Speaker modes will be initialized in pmd_SpeakerPaRestoreMode
PRIVATE UINT16 g_pmdSpeakerPaSetting3 =
    RDA_PMU_SETUP_TMCTRL_BIT(0)|
    RDA_PMU_RESET_CLG_DELAY_NEG(1)|
    RDA_PMU_RESET_CLG_DELAY_POS(1);

PRIVATE UINT16 g_pmdMisc48H =
    PMU_NAND_WP_OUT_B_REG|
    PMU_VA24_VSEL|
    PMU_VA24_IBIT(4);

PRIVATE UINT16 HAL_BOOT_DATA_INTERNAL g_pmdLdoBuck =
    RDA_PMU_VBUCK_VCORE_LDO_BIT(8) |
    RDA_PMU_VBUCK_VCORE_LDO_LP_BIT(4);


/// Configuration structure for TARGET
PROTECTED CONST PMD_CONFIG_T* g_pmdConfig;

/// To avoid SPI deactivation/activation during Open
PRIVATE BOOL g_pmdInitDone = FALSE;

/// To track activation status
PRIVATE VOLATILE UINT8 g_pmdActivated = 0;
PRIVATE VOLATILE UINT8 g_tsdActivated = 0;
PRIVATE VOLATILE UINT8 g_SimActivated = 0;

// Workaround to disable PMU IRQ without blocking in PMU IRQ handler
PRIVATE UINT32 HAL_BOOT_BSS_INTERNAL g_pmdIrqSettingLatest = 0;

// Workaround to disable HP-Detect IRQ without blocking in PMU IRQ handler
PRIVATE UINT32 g_pmdHpDetectSettingLatest = 0;

// Clk4m and Avdd3 enabling registry in LP mode
PRIVATE BOOL g_pmdClk4mAvdd3EnRegistryLp[PMD_CLK4M_AVDD3_USER_ID_QTY];

PRIVATE BOOL g_pmdBacklightOn = FALSE;

PRIVATE BOOL g_pmdScreenAntiFlickerEnabled = FALSE;

#if (LCD_BACKLIGHT_CHARGE_PUMP == 0)
PRIVATE PMD_BL_LED_OUT_SETTING_T g_pmdBlLedOutDefault;

PRIVATE CONST PMD_BL_LED_OUT_SETTING_T gc_pmdBlLedOutAntiFlicker =
{
    .vRgbLedIbit = 0,
    .vRgbLedVsel = 5,
};
#endif

PRIVATE BOOL g_pmdAudExtPaExists = FALSE;

#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
PRIVATE BOOL g_pmdSpeakerPaCpLowOutput = FALSE;

PRIVATE BOOL g_pmdSpeakerPaEnabled = FALSE;

PRIVATE PMD_SPEAKER_PA_MODE_T g_pmdSpeakerLeftChanMode;

PRIVATE PMD_SPEAKER_PA_MODE_T g_pmdSpeakerRightChanMode;
#endif

PRIVATE PMD_EAR_HANDLER_T ear_handler = NULL;

PRIVATE HAL_GPIO_GPIO_ID_T g_ear_gpio = HAL_GPIO_NONE;

PRIVATE BOOL g_pmdFmInLpModeEnabled = FALSE;

PRIVATE VOID (*g_pmdAuxClkStatusHandler)(BOOL) = NULL;

PRIVATE PMD_CHANGE_PARAM_T HAL_UNCACHED_BSS_INTERNAL g_pmdChangeParam;

// ============================================================================
// Forward declarations
// ============================================================================
PRIVATE UINT16 pmd_RegRead(RDA_REG_MAP_T regIdx);
PRIVATE VOID pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT16 data);
PRIVATE VOID pmd_EarpDetectGpioInit(HAL_GPIO_GPIO_ID_T gpioId);
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
PRIVATE VOID pmd_SpeakerPaSetCpLevel(UINT32 level);
#endif
#if (LCD_BACKLIGHT_CHARGE_PUMP)
PRIVATE VOID pmd_UpdateBacklightChargePump(VOID);
#endif
PUBLIC VOID pmd_SetPadVoltage(UINT8 level);

#define LOOP_DELAY(count)       for (int n = (count); n > 0; --n) asm volatile ("nop")

// ============================================================================
// FUNCTIONS
// ============================================================================

//=============================================================================
// pmd_RegWriteProtected
//-----------------------------------------------------------------------------
/// Function to write PMU register without stack and cachable data access.
//=============================================================================
PRIVATE VOID __attribute__ ((nomips16, noinline)) HAL_FUNC_INTERNAL pmd_RegWriteProtected(PMD_CHANGE_PARAM_T* param)
{
    for (int n = 0; n < param->reg_count; n++)
    {
        UINT32 data = SPI_CS(HAL_ISPI_CS_PMU)
                      | (0<<25)
                      | ((param->reg_idx[n] & 0x1ff) << 16)
                      | param->reg_data[n];

        // boot_IspiTxFifoAvail
        while (GET_BITFIELD(hwp_spi3->status, SPI_TX_SPACE) < 1)
            ;

        // boot_IspiSendData
        hwp_spi3->rxtx_buffer = data;

        // boot_IspiTxFinished
        for (;;)
        {
            UINT32 status = hwp_spi3->status;
            if ((status & SPI_ACTIVE_STATUS) == 0 &&
                    GET_BITFIELD(status, SPI_TX_SPACE) == SPI_TX_FIFO_SIZE)
                break;
        }
    }

    LOOP_DELAY(param->delay_count);
}

//=============================================================================
// pmd_OpalSpiRead
//-----------------------------------------------------------------------------
/// Read Opal register on SPI, the CS must already be activated
/// @param regIdx register to read
/// @return register value                 0 write 1 read
//=============================================================================
PUBLIC UINT16 HAL_BOOT_FUNC pmd_OpalSpiRead(RDA_REG_MAP_T regIdx)
{
    return hal_IspiRegRead(HAL_ISPI_CS_PMU, regIdx);
}

PUBLIC UINT16 pmd_RDARead(RDA_REG_MAP_T regIdx)
{
    return hal_IspiRegRead(HAL_ISPI_CS_PMU, regIdx);
}

PUBLIC UINT16 pmd_RegRead(RDA_REG_MAP_T regIdx)
{
    return hal_IspiRegRead(HAL_ISPI_CS_PMU, regIdx);
}

//=============================================================================
// pmd_OpalSpiWrite
//-----------------------------------------------------------------------------
/// Write Opal register on SPI, the CS must already be activated
/// @param regIdx register to read
/// @param value register value
//=============================================================================
PUBLIC VOID HAL_BOOT_FUNC pmd_OpalSpiWrite(RDA_REG_MAP_T regIdx, UINT16 value)
{
    hal_IspiRegWrite(HAL_ISPI_CS_PMU, regIdx, value);
}

PUBLIC VOID pmd_RDAWrite(RDA_REG_MAP_T regIdx, UINT16 value)
{
    hal_IspiRegWrite(HAL_ISPI_CS_PMU, regIdx, value);
}

PUBLIC VOID pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT16 data)
{
    hal_IspiRegWrite(HAL_ISPI_CS_PMU, regIdx, data);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID HAL_FUNC_ROM pmd_RegIrqSettingSet(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdIrqSettingLatest |= flags;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);
    // Never save the clear bits

#ifdef CHIP_DIE_8955
    g_pmdIrqSettingLatest &= ~(RDA_PMU_INT_CHR_CLEAR);

#else
    g_pmdIrqSettingLatest &= ~(RDA_PMU_INT_CHR_CLEAR|RDA_PMU_PENIRQ_CLEAR|
                               RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_KEYIRQ_CLEAR);

#endif
    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingClr(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdIrqSettingLatest &= ~flags;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);

    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_HP_DETECT_SETTING must go through
// pmd_RegHpDetectSettingSet() or pmd_RegHpDetectSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegHpDetectSettingSet(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdHpDetectSettingLatest |= flags;
    pmd_OpalSpiWrite(RDA_ADDR_HP_DETECT_SETTING, (UINT16)g_pmdHpDetectSettingLatest);
    // Never save the clear bits
    g_pmdHpDetectSettingLatest &= ~(RDA_PMU_HP_OUT_CLEAR|RDA_PMU_HP_IN_CLEAR);

    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_HP_DETECT_SETTING must go through
// pmd_RegHpDetectSettingSet() or pmd_RegHpDetectSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegHpDetectSettingClr(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdHpDetectSettingLatest &= ~flags;
    pmd_OpalSpiWrite(RDA_ADDR_HP_DETECT_SETTING, (UINT16)g_pmdHpDetectSettingLatest);

    hal_SysExitCriticalSection(scStatus);
}

// ============================================================================
// pmd_EnableOpalLdo
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param ldo the id of the ldo to switch from enum #PMD_LDO_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
/// @param profile the profile applied.
/// @param user the LDO user set by POWER_LDO_USER or LEVEL_LDO_USER macro.
// ============================================================================
PROTECTED VOID pmd_EnableOpalLdo(PMD_LDO_ID_T ldo, BOOL on,
                                 PMD_PROFILE_MODE_T profile, UINT32 user)
{
    //PMD_TRACE(PMD_DBG_TRC, 0, "pmd_EnableOpalLdo: profile=%d on=%d ldo=0x%x", profile, on, ldo);

    PMD_ASSERT(profile < PMD_PROFILE_MODE_QTY, "pmd_EnableOpalLdo: Invalid profile number (%d)", profile);
    PMD_ASSERT(ldo.type == PMD_LDO_ID_TYPE_OPAL, "pmd_EnableOpalLdo: using ID of non LDO resource (%08x) ", ldo);
    PMD_ASSERT(ldo.id < (1<<PMD_LDO_QTY),
               "pmd_EnableOpalLdo: invalid LDO Id 0x%08x", ldo);

    // VMEM is derived from VUSB, so VUSB should be always on
    // (VUSB is guaranteed to be turned on during initialization)
    if (ldo.id & PMD_LDO_USB)
    {
        ldo.id &= ~PMD_LDO_USB;
    }
    // VMIC is derived from VABB
    if (ldo.id & PMD_LDO_MIC)
    {
        ldo.opal |= PMD_LDO_ABB;
    }

    if (ldo.id == 0)
    {
        return;
    }

    UINT32 reg_offset = 0;
    UINT32 idx = 0;
    UINT32 ldoId = ldo.id;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    while (ldoId)
    {
        if (ldoId & 0x1)
        {
            if (on)
            {
                if (g_pmdLdoUserBitmap[profile][idx] != 0)
                {
                    // This LDO has been enabled before, and we do not need to enable it again.
                    ldo.id &= ~(1<<idx);
                }
                g_pmdLdoUserBitmap[profile][idx] |= user;
            }
            else
            {
                g_pmdLdoUserBitmap[profile][idx] &= ~user;
                if (g_pmdLdoUserBitmap[profile][idx] != 0)
                {
                    // This LDO is still used by another user, and we can NOT disable it.
                    ldo.id &= ~(1<<idx);
                }
            }
        }
        ldoId >>= 1;
        idx++;
    }

    if (ldo.id == 0)
    {
        hal_SysExitCriticalSection(scStatus);
        return;
    }

    // NOTE:
    // The hw bit for PMD_LDO_BLLED is useless.
    // Sw should set rgbLedOff bit in LDO setting2 register instead.

    if (on)
    {
        g_pmdLdoProfileSetting1[profile].reg &= ~(ldo.id & RDA_PMU_LDO_EN_MASK);
        if (ldo.id & PMD_LDO_BLLED)
        {
            g_pmdLdoProfileSetting2[profile].rgbLedOff = FALSE;
        }
    }
    else
    {
        g_pmdLdoProfileSetting1[profile].reg |= ldo.id & RDA_PMU_LDO_EN_MASK;
        if (ldo.id & PMD_LDO_BLLED)
        {
            g_pmdLdoProfileSetting2[profile].rgbLedOff = TRUE;
        }
    }

    // ACT and LP LDO setting1 registers are in the same structure
    if (profile == PMD_PROFILE_MODE_LOWPOWER)
    {
        reg_offset = 5;
    }

    // during pmd_Open, don't write to Opal Profile reg for each LDO change
    // pmd_Open will write the final value before enabling the profile
    if (g_pmdInitDone)
    {
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1+reg_offset,g_pmdLdoProfileSetting1[profile].reg);
        if (ldo.id & PMD_LDO_BLLED)
        {
            pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2+reg_offset,g_pmdLdoProfileSetting2[profile].reg);
        }
    }

    hal_SysExitCriticalSection(scStatus);
}



// =============================================================================
//  Special API with lower level SPI handling (for multiple register access)
// =============================================================================


//=============================================================================
// pmd_OpalSpiSendCommand
//-----------------------------------------------------------------------------
/// Send a burst of commands to Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results ensure the command has been effectively send).
/// @param cmd array of bytes containing the command, use #PMD_PMD_SPI_BUILD_WRITE_COMMAND and #PMD_OPAL_SPI_BUILD_READ_COMMAND.
/// @param length number of SPI command to send (cmd array must by 3 time that)
//=============================================================================
PROTECTED VOID pmd_OpalSpiSendCommand(CONST UINT32* cmd, UINT32 length)
{
    UINT32 count = 0;
    while (count < length)
    {
        count += hal_IspiSendDataBuffer(HAL_ISPI_CS_PMU,
                                        cmd + count,
                                        length - count);
    }
}


//=============================================================================
// pmd_OpalSpiGetData
//-----------------------------------------------------------------------------
/// Read a burst of data from Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results before sending too much new commands).
/// @param data array of bytes to receive the data, use #PMD_OPAL_SPI_EXTRACT_DATA
/// @param length number of SPI command recieve (data array must by 3 time that)
///
/// This function is blocking, it will pool to get all the expected data,
/// use with care!
//=============================================================================
PROTECTED VOID pmd_OpalSpiGetData(UINT32* data, UINT32 length)
{
    UINT32 count;
    count = 0;
    while (count < length)
    {
        count += hal_IspiGetDataBuffer(HAL_ISPI_CS_PMU,
                                       data + count,
                                       length - count);
    }
}


// ============================================================================
//  pmd_EnableFmInLpMode
// ============================================================================
PRIVATE VOID pmd_EnableFmInLpMode(BOOL on)
{
    PMD_LDO_ID_T abbLdo = { .opal = PMD_LDO_ABB };
    pmd_EnableOpalLdo(abbLdo, on, PMD_PROFILE_MODE_LOWPOWER, POWER_LDO_USER(PMD_POWER_FM));

    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdFmInLpModeEnabled = on;

#ifndef CHIP_DIE_8809E
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_FM, !on);
#endif

    if (on)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = TRUE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = FALSE;
    }

    pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
    hal_SysExitCriticalSection(scStatus);
}

BOOL pmd_IsOpen(VOID)
{
    return g_pmdInitDone;
}

// ============================================================================
// pmd_AudExtPaExists
// ----------------------------------------------------------------------------
/// Tell if an external audio PA exists in the configuration
// ============================================================================
PUBLIC BOOL pmd_AudExtPaExists(VOID)
{
    return g_pmdAudExtPaExists;
}


// =============================================================================
// pmd_SetMmcVoltage
// -----------------------------------------------------------------------------
/// level :0-7
//  7-3.2V  3-3.09v  0-2.9v
// =============================================================================
PUBLIC VOID pmd_SetMmcVoltage(UINT8 level)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoActiveSetting4 =
        PMU_SET_BITFIELD(g_pmdLdoActiveSetting4,
                         RDA_PMU_VMMC_IBIT,
                         level);
    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING4, g_pmdLdoActiveSetting4);
    hal_SysExitCriticalSection(scStatus);
}


#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
PUBLIC VOID pmd_SetEfuseLockHost(void);
#endif

PRIVATE VOID pmd_VcoreMode_LDO_DCDC(BOOL dcdc)
{

}


// =============================================================================
// pmd_Open
// -----------------------------------------------------------------------------
/// Open the PMD driver. This function configure the devices and set the
/// global variables needed for proper operation of the PMD driver.
/// It must be called before any other function of the PMD driver. That's why
/// this function is called in the end of #hal_Open. That way, the PMD driver
/// functions should always be usable by the user program, as soon as its main
/// function is launched by HAL.
///
/// @param pmdConfig Pointer to the PMD configuration structure as obtained
/// through the TGT API (#tgt_GetPmdConfig).
// -----------------------------------------------------------------------------
// array to store information for shared pins:
// UINT32 g_pmdMapAccess.powerInfo[PMD_POWER_ID_QTY];
// the first ID of a share has bit 31 = 0 and lower bits represent resource used
// the other IDs have bit 31 = 1 and lower bits are the ID of the first one
// note: if more that 31 IDs are needed
// change the bit usage: instead of indexing with the bits with ID use bit 0
// for first, then add the bit to the LINK information (need 5 bits)
// then we can use IDs as we want but a shared group is limited to 31 IDs for
// the same pin.
// =============================================================================
PUBLIC VOID pmd_Open(CONST PMD_CONFIG_T* pmdConfig)
{
    UINT32 i;

    UINT32 vldo = pmd_OpalSpiRead(RDA_ADDR_REVISION_ID);
    hal_HstSendEvent(0x9999a244);
#if defined(_FLASH_PROGRAMMER)
#else
    //hal_TimDelay(16381*2);
#endif
#ifdef I2C_REG_DEBUG
    extern VOID fmd_Registeri2cRegCheckFunc(VOID);
    fmd_Registeri2cRegCheckFunc();
#endif

#ifdef SPI_REG_DEBUG
    extern VOID pmd_RegisterSpiRegCheckFunc(VOID);
    pmd_RegisterSpiRegCheckFunc();
#endif // SPI_REG_DEBUG

    // DEFAULT_LDO_USER is at bit 31
    PMD_ASSERT(PMD_POWER_ID_QTY + PMD_LEVEL_ID_QTY < 31,
               "Too many LDO users for current LDO user bitmap: powerIds=%d, levelIds=%d",
               PMD_POWER_ID_QTY, PMD_LEVEL_ID_QTY);
    PMD_ASSERT(PMD_POWER_ID_QTY == PMD_MAP_POWER_ID_QTY, "Wrong PMD_MAP_POWER_ID_QTY in pmd_map.xmd");
    PMD_ASSERT(PMD_LEVEL_ID_QTY == PMD_MAP_LEVEL_ID_QTY, "Wrong PMD_MAP_LEVEL_ID_QTY in pmd_map.xmd");
    PMD_ASSERT(pmdConfig != NULL, "pmdConfig must be not NULL.");

    g_pmdConfig = pmdConfig;
    g_pmdInitDone = FALSE;

    // Check chip version
    UINT32 metalId = hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID);

    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_BTTEST] = PMD_DCDC_VCORE_VALUE_BTTEST;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = PMD_DCDC_VCORE_VALUE_HIGH;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = PMD_DCDC_VCORE_VALUE_MEDIUM;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = PMD_DCDC_VCORE_VALUE_LOW;

    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_BTTEST] = PMD_LDO_VCORE_VALUE_BTTEST;
    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = PMD_LDO_VCORE_VALUE_HIGH;
    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = PMD_LDO_VCORE_VALUE_MEDIUM;
    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = PMD_LDO_VCORE_VALUE_LOW;

    for (int i = 0; i < PMD_CORE_VOLTAGE_USER_ID_QTY; i++)
        g_pmdVcoreActRegistry[i] = PMD_CORE_VOLTAGE_LOW;
    g_pmdVcoreActRegistry[PMD_CORE_VOLTAGE_USER_DDR] = PMD_CORE_VOLTAGE_MEDIUM;

    // BT requires VBUCK1_NLP/LP=8,VBUCK2_NLP/LP=9 on U02 chips.
    // Set default LDO on, more will be updated by power[] and level[]
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg &=
        ~(g_pmdConfig->ldoEnableNormal & RDA_PMU_LDO_EN_MASK);
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg &=
        ~(g_pmdConfig->ldoEnableLowPower & RDA_PMU_LDO_EN_MASK);
    // NOTE:
    // The hw bit for vBlLedOff is useless.
    // Sw should set rgbLedOff bit in LDO setting2 register instead.
    g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].rgbLedOff =
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].vBlLedOff;
    g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].rgbLedOff =
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].vBlLedOff;

    g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vPadIs1_8 = FALSE;
    // Init volatge selection
    // vPad should be lowered to 1.8V when in LP mode
    if (g_pmdConfig->ldoIbrIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vIbrIs1_8 = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIbrIs1_8 = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vIbrIs1_8 = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIbrIs1_8 = TRUE;
    }
    if (g_pmdConfig->ldoMMCIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vMmcIs1_8 = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vMmcIs1_8 = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vMmcIs1_8 = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vMmcIs1_8 = TRUE;
    }
    if (g_pmdConfig->ldoLcdIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vLcdIs1_8 = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vLcdIs1_8 = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vLcdIs1_8 = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vLcdIs1_8 = TRUE;
    }
    if (g_pmdConfig->ldoCamIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vCamIs1_8 = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vCamIs1_8 = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vCamIs1_8 = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vCamIs1_8 = TRUE;
    }
    if (g_pmdConfig->ldoRfIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vRfIs1_8 = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vRfIs1_8 = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vRfIs1_8 = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vRfIs1_8 = TRUE;
    }

    // Init LDO user bitmap
    for (i=0; i<PMD_LDO_QTY; i++)
    {
        g_pmdLdoUserBitmap[PMD_PROFILE_MODE_NORMAL][i] = 0;
        g_pmdLdoUserBitmap[PMD_PROFILE_MODE_LOWPOWER][i] = 0;
    }

    // Init Clk4m and Avdd3 enabling registry in LP mode (default off)
    for (i=0; i<PMD_CLK4M_AVDD3_USER_ID_QTY; i++)
    {
        g_pmdClk4mAvdd3EnRegistryLp[i] = FALSE;
    }

    UINT32 id;
    // power
    for (id = 0; id < PMD_POWER_ID_QTY; id++)
    {
        g_pmdMapAccess.powerInfo[id] = 0;
        // initial state as described by powerOnState from config
        BOOL powerOn = g_pmdConfig->power[id].powerOnState;

        if (id != PMD_POWER_USB) {
#ifdef TP_USE_VCAM_AND_I2C_BUS_2
			if (id == PMD_POWER_CAMERA) {
				powerOn = TRUE;
			}
#endif
            pmd_EnablePower(id, powerOn);
		}
        // For GPIO, set it to output mode
        if (g_pmdConfig->power[id].ldo.type == PMD_LDO_ID_TYPE_IO)
        {
            hal_GpioSetOut(g_pmdConfig->power[id].ldo.pin.gpioId);
        }
    }

    // level
    for (id = 0; id < PMD_LEVEL_ID_QTY; id++)
    {
        // initial state as described by powerOnState from config
        pmd_SetLevel(id, g_pmdConfig->level[id].powerOnState);
        // For GPIO, set it to output mode
        if (g_pmdConfig->level[id].type == PMD_LEVEL_TYPE_LDO &&
                g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_IO)
        {
            hal_GpioSetOut(g_pmdConfig->level[id].ldo.pin.gpioId);
        }
    }

    // Init LDO enable
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    // Set LDO setting1
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

    // Set LDO setting2
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);

    // Init LCD/LED setting
    pmd_OpalSpiWrite(RDA_ADDR_LED_SETTING1, g_pmdLedSetting1);
    pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);

    // Let ABB control Audio gains
    pmd_OpalSpiWrite(RDA_ADDR_LED_SETTING5, g_pmdLedSetting5);

    // Init BL_LED_OUT voltage
#if (LCD_BACKLIGHT_CHARGE_PUMP == 0)
    g_pmdBlLedOutDefault.vRgbLedIbit = g_pmdLdoProfileSetting5.vRgbLedIbit;
    g_pmdBlLedOutDefault.vRgbLedVsel = g_pmdLdoProfileSetting5.vRgbLedVsel;
#endif
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);

    // Init clock 4M and LDO avdd3
    pmd_OpalSpiWrite(RDA_ADDR_MISC_CONTROL, g_pmdMiscControlActive);

    // Init SIM setting (not to select any card)
    pmd_SelectSimCard(0xff);

    // battery monitor
    hal_AnaGpadcOpen(g_pmdConfig->batteryGpadcChannel, HAL_ANA_GPADC_ATP_2S);

    pmd_InitRdaPmu();

    g_pmdInitDone = TRUE;

    // Check if external audio PA exists
    if (g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].ldo.type != HAL_GPIO_TYPE_NONE)
    {
        g_pmdAudExtPaExists = TRUE;
    }
    // memd_set_flash_sysclk_to_stable(TRUE,HAL_SYS_FREQ_208M,9);

    // Earpiece detection
    pmd_EarpDetectGpioInit(g_pmdConfig->earpieceDetectGpio);
#ifdef SUPPORT_QUICK_VCOM
    pmd_VcomGpioInit(g_pmdConfig->vcomDetectGpio);
#endif

#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
    pmd_SetEfuseLockHost();
#endif
#if defined(_FLASH_PROGRAMMER)
#else
    //hal_TimDelay(16381*2);
#endif
}


// =============================================================================
// pmd_Close
// -----------------------------------------------------------------------------
/// Closes the PMD driver.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM pmd_Close(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    // Turn off LCD backlight
    pmd_SetLevel(PMD_LEVEL_LCD, 0);
    pmd_SetLevel(PMD_LEVEL_SUB_LCD, 0);
    // Turn off keypad lights
    pmd_SetLevel(PMD_LEVEL_KEYPAD, 0);
    pmd_SetLevel(PMD_LEVEL_LED0, 0);
    pmd_SetLevel(PMD_LEVEL_LED1, 0);
    pmd_SetLevel(PMD_LEVEL_LED2, 0);
    pmd_SetLevel(PMD_LEVEL_LED3, 0);
    // Power off ABB (The pop sound is smaller if ABB power is down at first)
    g_pmdLdoSettings.reg = 0;
    pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
    pmd_RDAWrite(RDA_ADDR_LDO_POWER_OFF_SETTING2,0x1fc7);
    hal_TimDelay(1000 MS_WAITING);
    // Power off audio module
    hal_AudForcePowerDown();
    // Disable GPADC
    pmd_GpadcDisable();
    // Restore charger h/w setting
    pmd_RestoreChargerAtPowerOff();
    // Disable headphone detect IRQs
    pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);
#if 0 // power down is put into another function
    // 1) Disable EOM, PEN, Charger IRQs;
    // 2) Enter the power down mode (power controlled by PMU default profile).
    g_pmdIrqSettingLatest = 0;
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_MASK|RDA_PMU_PENIRQ_MASK|
                         RDA_PMU_KEYIRQ_MASK|RDA_PMU_INT_CHR_MASK|
                         RDA_PMU_HOLD_PRECHARGER_EFUSE);
#endif

    hal_SysExitCriticalSection(scStatus);
}

PUBLIC VOID __attribute__ ((nomips16)) HAL_FUNC_ROM pmd_Deactive(VOID)
{
    // 1) Disable EOM, PEN, Charger IRQs;
    // 2) Enter the power down mode (power controlled by PMU default profile).
    g_pmdIrqSettingLatest = RDA_PMU_EOMIRQ_MASK|RDA_PMU_PENIRQ_MASK|
                            RDA_PMU_KEYIRQ_MASK|RDA_PMU_INT_CHR_MASK|
                            RDA_PMU_HOLD_PRECHARGER_EFUSE;

    LOOP_DELAY(10*50); // 50us under 52M

    g_pmdChangeParam.reg_count = 1;
    g_pmdChangeParam.reg_idx[0] = RDA_ADDR_LDO_BUCK1;
    g_pmdChangeParam.reg_data[0] = gc_pmdLdoBuck1Lp;
    g_pmdChangeParam.delay_count = 0;
    pmd_RegWriteProtected(&g_pmdChangeParam);
}

// ============================================================================
// pmd_EnterDeepSleepStep1
// ----------------------------------------------------------------------------
/// Enter deep sleep (low power) step1. Here, memory can be accessed.
// ============================================================================
PUBLIC VOID PMD_FUNC_INTERNAL pmd_EnterDeepSleepStep1(VOID)
{
    PMD_PROFILE_WINDOW_EXIT(PMD_MODE_IS_ACTIVE);

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DbgPxtsSetEnabled(FALSE);
#endif

    // Disable charge LDO to save power
    g_pmdCalibSetting2 |= RDA_PMU_PD_CHARGE_LDO;

    pmd_RDAWrite(RDA_ADDR_MISC_CONTROL, g_pmdMiscControlLp);
    pmd_RDAWrite(RDA_ADDR_CALIBRATION_SETTING2, g_pmdCalibSetting2);
    pmd_SetPadVoltage(gc_pmdPadLevelLp);

#ifdef __PRJ_WITH_SPIFLSH__
    memd_FlashSleep();
#endif
}

// ============================================================================
// pmd_EnterDeepSleepStep2
// ----------------------------------------------------------------------------
/// Enter deep sleep (low power) step2. Here, memory CAN NOT be accessed.
// ============================================================================
PUBLIC VOID __attribute__ ((nomips16)) PMD_FUNC_INTERNAL pmd_EnterDeepSleepStep2(VOID)
{
    LOOP_DELAY(10*10); // 10us under 52M

    g_pmdChangeParam.reg_count = 1;
    g_pmdChangeParam.reg_idx[0] = RDA_ADDR_LDO_BUCK1;
    g_pmdChangeParam.reg_data[0] = gc_pmdLdoBuck1Lp;
    g_pmdChangeParam.delay_count = 0;
    pmd_RegWriteProtected(&g_pmdChangeParam);
}

// ============================================================================
// pmd_ExitDeepSleepStep1
// ----------------------------------------------------------------------------
/// Exit deep sleep (low power) step1. Here, memory CAN NOT be accessed.
// ============================================================================
PUBLIC VOID __attribute__ ((nomips16)) PMD_FUNC_INTERNAL pmd_ExitDeepSleepStep1(VOID)
{
    g_pmdChangeParam.reg_count = 1;
    g_pmdChangeParam.reg_idx[0] = RDA_ADDR_LDO_BUCK1;
    g_pmdChangeParam.reg_data[0] = gc_pmdLdoBuck1Act;
    g_pmdChangeParam.delay_count = 10 * 40; // 40us under 52M
    pmd_RegWriteProtected(&g_pmdChangeParam);
}

// ============================================================================
// pmd_ExitDeepSleepStep2
// ----------------------------------------------------------------------------
/// Exit deep sleep (low power) step2. Here, memory can be accessed.
// ============================================================================
PUBLIC VOID PMD_FUNC_INTERNAL pmd_ExitDeepSleepStep2(VOID)
{
#ifdef __PRJ_WITH_SPIFLSH__
    memd_FlashWakeup();
#endif

    // Enable charge LDO to reduce vsim fluctuation in low battery
    g_pmdCalibSetting2 &= ~RDA_PMU_PD_CHARGE_LDO;

    pmd_RDAWrite(RDA_ADDR_MISC_CONTROL, g_pmdMiscControlActive);
    pmd_RDAWrite(RDA_ADDR_CALIBRATION_SETTING2, g_pmdCalibSetting2);
    pmd_SetPadVoltage(gc_pmdPadLevelAct);

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DbgPxtsSetEnabled(TRUE);
#endif

    PMD_PROFILE_WINDOW_ENTER(PMD_MODE_IS_ACTIVE);
}

// ============================================================================
// pmd_SetPowerMode
// ----------------------------------------------------------------------------
/// Set Power Mode to the desired mode
// ============================================================================
PUBLIC VOID pmd_SetPowerMode(PMD_POWERMODE_T powerMode)
{
    // We can't change power mode arbitrary.
    // Keep this empty function just to make link happy.
}

// ============================================================================
//  pmd_EnableClk4mAvdd3InLpMode
// ============================================================================
PROTECTED VOID pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_ID_T user,
        BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdClk4mAvdd3EnRegistryLp[user] = on;

    if (!on)
    {
        // Disable Clk4m and Avdd3 in LP mode only when all the users request so.
        for (UINT32 i=0; i<PMD_CLK4M_AVDD3_USER_ID_QTY; i++)
        {
            if (g_pmdClk4mAvdd3EnRegistryLp[i])
            {
                on = TRUE;
                break;
            }
        }
    }

    if (on)
    {
        // Not to direct-reg clock 4M and avdd3
        g_pmdMiscControlLp &= ~(RDA_PMU_PU_CLK_4M_DR|
                                RDA_PMU_PD_LDO_AVDD3_DR);
    }
    else
    {
        // Direct-reg to disable clock 4M and avdd3
        g_pmdMiscControlLp |= RDA_PMU_PU_CLK_4M_DR|
                              RDA_PMU_PD_LDO_AVDD3_DR|
                              RDA_PMU_PD_LDO_AVDD3_REG;
        g_pmdMiscControlLp &= ~RDA_PMU_PU_CLK_4M_REG;
    }

    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
// pmd_ConfigureRfLowPower
// ----------------------------------------------------------------------------
/// Configure the Low Power state for the RF
///
/// @param xcvUseVRF \c TRUE if the transciever use V_RF from PMD.
/// @param xcvLowPowerKeepVRFOn \c TRUE if the power on V_RF must stay on
///         during low power.
/// @return configuration for LPS (only fieds controlling LPSCO_0 and LPSCO_1)
///         actual type is #HAL_LPS_POWERUP_CTRL_T.
// ============================================================================
PUBLIC UINT32 pmd_ConfigureRfLowPower(BOOL xcvUseVRF, BOOL xcvLowPowerKeepVRFOn)
{
    union
    {
        UINT32 reg;
        HAL_LPS_POWERUP_CTRL_T bitfield;
    } puctrl;

    puctrl.reg = 0;

    // If called before pmd_Open (hal_BoardSetup) we don't configure the LDOs
    if (g_pmdInitDone)
    {
        // let LPSCO_0 be controlled by LPS FSM
        puctrl.bitfield.puCO0ForceOn = 0;
        // LP_MODE is active (low power) High
        // LPSCO_0 is active during normal mode, so invert the pin polarity
        puctrl.bitfield.puCO0Pol = 1;
    }
    else
    {
        // force off
        puctrl.bitfield.puCO0ForceOn = 1;
        puctrl.bitfield.puCO0Pol = 1;
    }
    // LPSCO_1 is not used by Opal, should be set as gpio in hal tgt
    // force off
    puctrl.bitfield.puCO1ForceOn = 1;
    puctrl.bitfield.puCO1Pol = 1;

    return puctrl.reg;
}


// ============================================================================
// pmd_SimSetVoltageClass
// ----------------------------------------------------------------------------
/// Set the voltage class
///
/// The #pmd_SimVoltageOpen function controlling the level shifter must have
/// previously called. \n
/// Voltage class must \b _ALWAYS_ be set to zero before re-programming
/// voltage
/// @param voltClass Voltage Class to set
// ============================================================================
PRIVATE VOID pmd_SimSetVoltageClass(PMD_SIM_VOLTAGE_CLASS_T voltClass)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "sim voltage: %d", voltClass);

    UINT32 scStatus = hal_SysEnterCriticalSection();

    if (voltClass == PMD_SIM_VOLTAGE_0V0)
    {
        g_pmdLdoSettings.vSim1Enable = FALSE;
        g_pmdLdoSettings.vSim2Enable = FALSE;
        g_pmdLdoSettings.vSim3Enable = FALSE;
        g_pmdLdoSettings.vSim4Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim1Enable = TRUE;
        g_pmdLdoSettings.vSim2Enable = TRUE;
        g_pmdLdoSettings.vSim3Enable = TRUE;
        g_pmdLdoSettings.vSim4Enable = TRUE;
    }

    switch (voltClass)
    {
        case PMD_SIM_VOLTAGE_0V0:
            break;

        case PMD_SIM_VOLTAGE_1V8:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = TRUE;
            g_pmdLdoProfileSetting5.vSim2Is1_8 = TRUE;
            g_pmdLdoProfileSetting5.vSim3Is1_8 = TRUE;
            g_pmdLdoProfileSetting5.vSim4Is1_8 = TRUE;
            break;

        case PMD_SIM_VOLTAGE_3V0:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        case PMD_SIM_VOLTAGE_5V0:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim voltage: invalid voltage class %d", voltClass);
            break;
    }

    if (voltClass == PMD_SIM_VOLTAGE_0V0)
    {
        hal_SimDisableSci();
    }

    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    if (voltClass != PMD_SIM_VOLTAGE_0V0 &&
            (g_pmdSimSelect & (HAL_ABB_SIM_SELECT_SIM0|
                               HAL_ABB_SIM_SELECT_SIM1|
                               HAL_ABB_SIM_SELECT_SIM2|
                               HAL_ABB_SIM_SELECT_SIM3)) != 0)
    {
        hal_SimEnableSci();
    }

    hal_SysExitCriticalSection(scStatus);
}

DUALSIM_ERR_T pmd_FourSimSetVoltageClass(DUALSIM_SIM_VOLTAGE_T voltClass0,
        DUALSIM_SIM_VOLTAGE_T voltClass1,
        DUALSIM_SIM_VOLTAGE_T voltClass2,
        DUALSIM_SIM_VOLTAGE_T voltClass3)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_FourSimSetVoltageClass: class1 = %d , class2 = %d  class3 = %d , class4 = %d ",
              voltClass0,voltClass1, voltClass2,voltClass3);

    UINT16 simSelMask = 0;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    if (voltClass0 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim1Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim1Enable = TRUE;
        simSelMask |= HAL_ABB_SIM_SELECT_SIM0;
    }

    switch (voltClass0)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim0 voltage: invalid voltage class %d", voltClass0);
            break;
    }

    if (voltClass1 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim2Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim2Enable = TRUE;
        simSelMask |= HAL_ABB_SIM_SELECT_SIM1;
    }

    switch (voltClass1)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim2Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim1 voltage: invalid voltage class %d", voltClass0);
            break;
    }

    if (voltClass2 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim3Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim3Enable = TRUE;
        simSelMask |= HAL_ABB_SIM_SELECT_SIM2;
    }

    switch (voltClass2)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim3Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim2 voltage: invalid voltage class %d", voltClass0);
            break;
    }

    if (voltClass3 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim4Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim4Enable = TRUE;
        simSelMask |= HAL_ABB_SIM_SELECT_SIM3;
    }

    switch (voltClass3)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim4Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim3 voltage: invalid voltage class %d", voltClass0);
            break;
    }

    if ((simSelMask & g_pmdSimSelect) == 0)
    {
        hal_SimDisableSci();
    }

    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    hal_TimDelay(1);//delay 60us, waiting vsim stable

    if ((simSelMask & g_pmdSimSelect) != 0)
    {
        hal_SimEnableSci();
    }

    hal_SysExitCriticalSection(scStatus);

    return DUALSIM_ERR_NO;
}


BOOL pmd_SelectSimCard(UINT8 sim_card)
{
    PMD_LDO_SETTINGS_T simEnMask;
    simEnMask.reg = 0;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdSimSelect = 0;

    if (sim_card == 0)
    {
        g_pmdSimSelect |= HAL_ABB_SIM_SELECT_SIM0;
        simEnMask.vSim1Enable = TRUE;
    }
    else if (sim_card == 1)
    {
        g_pmdSimSelect |= HAL_ABB_SIM_SELECT_SIM1;
        simEnMask.vSim2Enable = TRUE;
    }
    else if (sim_card == 2)
    {
        g_pmdSimSelect |= HAL_ABB_SIM_SELECT_SIM2;
        simEnMask.vSim3Enable = TRUE;
    }
    else if (sim_card == 3)
    {
        g_pmdSimSelect |= HAL_ABB_SIM_SELECT_SIM3;
        simEnMask.vSim4Enable = TRUE;
    }

    if ((simEnMask.reg & g_pmdLdoSettings.reg) == 0)
    {
        hal_SimDisableSci();
    }

    hal_AbbEnableSim(g_pmdSimSelect);

    if ((simEnMask.reg & g_pmdLdoSettings.reg) != 0)
    {
        hal_SimEnableSci();
    }

    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}


#if (CONFIG_CLASS_K_SPEAKER_PA)
VOID pmd_EnableClassKLoudSpeaker(VOID)
{
    HAL_APO_ID_T apo = g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].ldo.pin;

    VOID (*gpioActive)(HAL_APO_ID_T apo);
    VOID (*gpioInactive)(HAL_APO_ID_T apo);
    if (g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].polarity)
    {
        gpioActive = &hal_GpioSet;
        gpioInactive = &hal_GpioClr;
    }
    else
    {
        gpioActive = &hal_GpioClr;
        gpioInactive = &hal_GpioSet;
    }

    // Generate a pulse sequence to configure class K audio PA

    // TODO:
    //     Change the following codes to meet your PA specification.
    // CAUSION:
    //     There is no INT-LOCK in hal_SysWaitMicrosecond(). Please
    // by yourselves determine whether INT-LOCK is needed when
    // generating the pulse sequence.

    //UINT32 scStatus = hal_SysEnterCriticalSection();

    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);

    //hal_SysExitCriticalSection(scStatus);
}
#endif // CONFIG_CLASS_K_SPEAKER_PA

// ============================================================================
// pmd_EnablePower
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param id the id of the power to switch from enum #PMD_POWER_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_EnablePower(PMD_POWER_ID_T id, BOOL on)
{
    BOOL newState;
    UINT32 callerAddr = 0;
    UINT32 POSSIBLY_UNUSED scStatus;

    SXS_GET_RA(callerAddr);
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_EnablePower id=%i on=%i from 0x%x", id, on, callerAddr);
    newState = on;
    g_pmdMapAccess.powerInfo[id] = newState;

    if (id == PMD_POWER_USB)
    {
        // Power on/off USB PHY
        // If v_usb is shared by other modules, it might NOT be powered down when
        // USB is requested to be turned off. To avoid impacting USB reset, USB PHY
        // must be directly controlled (not by baseband USB module).
        hal_AbbEnableUsbPhy(on);
    }
    else if (id == PMD_POWER_FM)
    {
        // Allow FM playing in LP mode
        pmd_EnableFmInLpMode(on);
#if (defined (RDAFM_8809) || defined (RDAFM_8955))
        if (on)
        {
            // Power on FM
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.fmEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
            // Enable FM IO
            hal_ConfigFmIo(on);
        }
        else
        {
            // Disable FM IO
            hal_ConfigFmIo(on);
            // Power down FM
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.fmEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
        }
#endif // RDAFM_8809 and RDAFM_8955
    }
    else if (id == PMD_POWER_BT)
    {
#ifdef RDABT_8809
        if (on)
        {
            // Power on BT
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.btEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
            // Enable BT IO
            hal_ConfigBtIo(on);
        }
        else
        {
            // Disable BT IO
            hal_ConfigBtIo(on);
            // Power down BT
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.btEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
        }
#endif // RDABT_8809
        if (g_pmdAuxClkStatusHandler)
        {
            (*g_pmdAuxClkStatusHandler)(on);
        }
    }

    // actual update according to newState.
    if (g_pmdConfig->power[id].ldo.type == PMD_LDO_ID_TYPE_OPAL)
    {
        pmd_EnableOpalLdo(g_pmdConfig->power[id].ldo, newState,
                          PMD_PROFILE_MODE_NORMAL, POWER_LDO_USER(id));

        if ( id == PMD_POWER_EARPIECE &&
                (g_pmdConfig->power[id].ldo.id&PMD_LDO_MIC) != 0 )
        {
            // VMIC needs 15 ms to be stable, so it can NOT always be off during low power
            pmd_EnableOpalLdo(g_pmdConfig->power[id].ldo, newState,
                              PMD_PROFILE_MODE_LOWPOWER, POWER_LDO_USER(id));
#ifdef VMIC_POWER_ON_WORKAROUND
            if (g_pmdInitDone)
            {
                aud_EnableEarpieceSetting(on);
            }
            else
            {
                // OS is NOT ready yet. Audio module should be initialize directly.
                if (on)
                {
                    aud_InitEarpieceSetting();
                }
            }
#endif // VMIC_POWER_ON_WORKAROUND
        }
    }
    else
    {
        // Pin
        // For pin-controlled power items, e.g., external loud speaker
#if (CONFIG_CLASS_K_SPEAKER_PA)
        if (id == PMD_POWER_LOUD_SPEAKER && newState)
        {
            pmd_EnableClassKLoudSpeaker();
        }
        else
#endif // CONFIG_CLASS_K_SPEAKER_PA
        {
            if ( (newState && !g_pmdConfig->power[id].polarity) ||
                    (!newState && g_pmdConfig->power[id].polarity) )
            {
                hal_GpioClr(g_pmdConfig->power[id].ldo.pin);
            }
            else
            {
                hal_GpioSet(g_pmdConfig->power[id].ldo.pin);
            }
        }
    }

    if (id == PMD_POWER_LCD && g_pmdInitDone)
    {
        pmd_SetHighActivity(on);
    }
}

#ifdef GPS_SUPPORT
void open_GPS_power_vVIB(void)
{
	pmd_SetLevel(PMD_LEVEL_VIBRATOR, 1);
}

void close_GPS_power_vVIB(void)
{
	pmd_SetLevel(PMD_LEVEL_VIBRATOR, 0);
}

void open_GPS_power_vCam(void)
{
	g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].vCamOff = FALSE;
	g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].vCamOff = FALSE;	
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
	pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
}

void close_GPS_power_vCam(void)
{
	g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].vCamOff = TRUE;
	g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].vCamOff = TRUE;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
	pmd_EnablePower(PMD_POWER_CAMERA, FALSE);
}

#endif

// ============================================================================
// pmd_SetLevel(PMD_LEVEL_LOUD_SPEAKER)
// ----------------------------------------------------------------------------
/// Set a Level to desired value. PMD_LEVEL_LCD
///
/// @param id the id of the level to set from enum #PMD_LEVEL_ID_T
/// @param level the desired value, its acutal type depends on the \p id
// ============================================================================
PUBLIC VOID pmd_SetLevel(PMD_LEVEL_ID_T id, UINT32 level)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SetLevel id=%i level=%i", id, level);

    UINT32 status;
    UINT32 value;

    g_pmdMapAccess.levelInfo[id] = level;

    switch (g_pmdConfig->level[id].type)
    {
        case PMD_LEVEL_TYPE_NONE:
            break;
        case PMD_LEVEL_TYPE_PWL0:
            if (level > 7)
            {
                level = 7;
            }
            hal_PwlSelLevel(HAL_PWL_0,g_pmdConfig->lightLevelToPwm0[level]);
            break;
        case PMD_LEVEL_TYPE_PWL1:
            if (level > 7)
            {
                level = 7;
            }
            hal_PwlSelLevel(HAL_PWL_1,g_pmdConfig->lightLevelToPwm1[level]);
            break;
        case PMD_LEVEL_TYPE_LPG:
            PMD_ASSERT(FALSE, "TODO impelment LPG", id);
            break;
        case PMD_LEVEL_TYPE_LDO:
            PMD_ASSERT(g_pmdConfig->level[id].ldo.type != PMD_LDO_ID_TYPE_INVALID,
                       "Invalid LDO type: id=%d, type=%d",
                       id, g_pmdConfig->level[id].ldo.type);
            if (g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_OPAL)
            {
                // Opal LDO
                pmd_EnableOpalLdo(g_pmdConfig->level[id].ldo, (level != 0),
                                  PMD_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));

                if (id == PMD_LEVEL_VIBRATOR)
                {
                    // Allow vibrator to work in LP mode
                    pmd_EnableOpalLdo(g_pmdConfig->level[id].ldo, (level != 0),
                                      PMD_PROFILE_MODE_LOWPOWER, LEVEL_LDO_USER(id));
                }
            }
            else
            {
                // Pin
                switch (id)
                {
                    case PMD_LEVEL_LCD:
                    case PMD_LEVEL_KEYPAD:
                    case PMD_LEVEL_LED0:
                    case PMD_LEVEL_LED1:
                    case PMD_LEVEL_LED2:
                    case PMD_LEVEL_LED3:
                    case PMD_LEVEL_VIBRATOR:
                        if (0 == level)
                        {
                            hal_GpioClr(g_pmdConfig->level[id].ldo.pin);
                        }
                        else
                        {
                            hal_GpioSet(g_pmdConfig->level[id].ldo.pin);
                        }
                        break;
                    default:
                        PMD_ASSERT(g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_NONE,
                                   "Invalid level LDO config: id=%d, type=%d",
                                   id, g_pmdConfig->level[id].ldo.type);
                        break;
                }
            }
            break;
        case PMD_LEVEL_TYPE_OPAL:
            switch (id)
            {
                case PMD_LEVEL_SIM:
                    pmd_SimSetVoltageClass(level);
                    break;
                case PMD_LEVEL_KEYPAD:
                case PMD_LEVEL_LED0:
                case PMD_LEVEL_LED1:
                case PMD_LEVEL_LED2:
                case PMD_LEVEL_LED3:
                {
                    // Keypad LED backlight control is driven by PMU directly instead of HAL PWL0.
                    if (level > 7)
                    {
                        level = 7;
                    }
                    // Get a value from the PWM0 config in the target
                    value = g_pmdConfig->lightLevelToPwm0[level];
#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
                    // Change value range from [0, 255] (for HAL PWM) to [0, 31] (for PMU PWM)
                    value = (value & 0xff) >> 3;
                    UINT32 dutyCycleMask, dutyCycle;
                    if (id == PMD_LEVEL_LED0)
                    {
                        dutyCycleMask = RDA_PMU_PWM_R_DUTY_CYCLE_MASK;
                        dutyCycle = RDA_PMU_PWM_R_DUTY_CYCLE(value);
                    }
                    else if (id == PMD_LEVEL_LED1)
                    {
                        dutyCycleMask = RDA_PMU_PWM_G_DUTY_CYCLE_MASK;
                        dutyCycle = RDA_PMU_PWM_G_DUTY_CYCLE(value);
                    }
                    else if (id == PMD_LEVEL_LED2)
                    {
                        dutyCycleMask = RDA_PMU_PWM_B_DUTY_CYCLE_MASK;
                        dutyCycle = RDA_PMU_PWM_B_DUTY_CYCLE(value);
                    }
                    else     // PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
                    {
                        dutyCycleMask = RDA_PMU_PWM_R_DUTY_CYCLE_MASK|
                                        RDA_PMU_PWM_G_DUTY_CYCLE_MASK|
                                        RDA_PMU_PWM_B_DUTY_CYCLE_MASK;
                        dutyCycle = RDA_PMU_PWM_R_DUTY_CYCLE(value)|
                                    RDA_PMU_PWM_G_DUTY_CYCLE(value)|
                                    RDA_PMU_PWM_B_DUTY_CYCLE(value);
                    }
#endif
#else // !ENABLE_KEYPAD_LIGHT_PWM
                    // Change value range from [0, 255] (for HAL PWM) to [0, 7] (for PMU DIM)
                    value = (value & 0xff) >> 5;
                    UINT32 ibitMask, ibit;
                    if (id == PMD_LEVEL_LED0)
                    {
                        ibitMask = RDA_PMU_LED_R_IBIT_MASK;
                        ibit = RDA_PMU_LED_R_IBIT(value);
                    }
                    else if (id == PMD_LEVEL_LED1)
                    {
                        ibitMask = RDA_PMU_LED_G_IBIT_MASK;
                        ibit = RDA_PMU_LED_G_IBIT(value);
                    }
                    else if (id == PMD_LEVEL_LED2)
                    {
                        ibitMask = RDA_PMU_LED_B_IBIT_MASK;
                        ibit = RDA_PMU_LED_B_IBIT(value);
                    }
                    else     // PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
                    {
                        ibitMask = RDA_PMU_LED_R_IBIT_MASK|
                                   RDA_PMU_LED_G_IBIT_MASK|
                                   RDA_PMU_LED_B_IBIT_MASK;
                        ibit = RDA_PMU_LED_R_IBIT(value)|
                               RDA_PMU_LED_G_IBIT(value)|
                               RDA_PMU_LED_B_IBIT(value);
                    }
#endif // !ENABLE_KEYPAD_LIGHT_PWM
                    UINT32 mask;
                    PMD_BG_LP_MODE_USER_ID_T bgUser;
                    PMD_CLK4M_AVDD3_USER_ID_T clk4mUser;
                    if (id == PMD_LEVEL_LED0)
                    {
                        mask = RDA_PMU_LED_R_OFF_ACT|RDA_PMU_LED_R_OFF_LP;
                        bgUser = PMD_BG_LP_MODE_USER_LED0;
                        clk4mUser= PMD_CLK4M_AVDD3_USER_LED0;
                    }
                    else if (id == PMD_LEVEL_LED1)
                    {
                        mask = RDA_PMU_LED_G_OFF_ACT|RDA_PMU_LED_G_OFF_LP;
                        bgUser = PMD_BG_LP_MODE_USER_LED1;
                        clk4mUser= PMD_CLK4M_AVDD3_USER_LED1;
                    }
                    else if (id == PMD_LEVEL_LED2)
                    {
                        mask = RDA_PMU_LED_B_OFF_ACT|RDA_PMU_LED_B_OFF_LP;
                        bgUser = PMD_BG_LP_MODE_USER_LED2;
                        clk4mUser= PMD_CLK4M_AVDD3_USER_LED2;
                    }
                    else     // PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
                    {
                        if (id == PMD_LEVEL_LED3)
                        {
                            mask = RDA_PMU_LED_R_OFF_ACT|RDA_PMU_LED_R_OFF_LP;
                            bgUser = PMD_BG_LP_MODE_USER_LED3;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_LED3;
                        }
                        else
                        {
#ifdef KEYPAD_BL_USE_LEDR_B
                            mask= RDA_PMU_LED_R_OFF_ACT|RDA_PMU_LED_R_OFF_LP|
                                  RDA_PMU_LED_B_OFF_ACT|RDA_PMU_LED_B_OFF_LP;
#else
                            mask= RDA_PMU_LED_G_OFF_ACT|RDA_PMU_LED_G_OFF_LP|
                                  RDA_PMU_LED_B_OFF_ACT|RDA_PMU_LED_B_OFF_LP;

#endif
                            bgUser = PMD_BG_LP_MODE_USER_KEYPAD_LED;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_KEYPAD_LED;
                        }
                    }
                    // Keypad/led power dependency setting in LP mode
                    PMD_LDO_ID_T ldo;
                    ldo.opal = PMD_LDO_BLLED;
                    pmd_EnableOpalLdo(ldo, (level != 0),
                                      PMD_PROFILE_MODE_LOWPOWER, LEVEL_LDO_USER(id));
                    pmd_EnableOpalLdo(ldo, (level != 0),
                                      PMD_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));
                    pmd_ChargerSetBgLpMode(bgUser, (level == 0));
                    pmd_EnableClk4mAvdd3InLpMode(clk4mUser, (level != 0));
                    // Keypad/led power setting
                    status = hal_SysEnterCriticalSection();
                    if (0 == level)
                    {
                        g_pmdLedSetting5 |= mask;
                    }
                    else
                    {
                        g_pmdLedSetting5 &= ~mask;
#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
                        g_pmdLedSetting4 = (g_pmdLedSetting4 & (~dutyCycleMask)) |
                                           dutyCycle;
                        pmd_RDAWrite(RDA_ADDR_LED_SETTING4, g_pmdLedSetting4);
#else
                        hal_PwlSelLevel(HAL_PWL_0, value);
#endif
#else // !ENABLE_KEYPAD_LIGHT_PWM
                        g_pmdLedSetting5 = (g_pmdLedSetting5 & (~ibitMask)) |
                                           ibit;
#endif// !ENABLE_KEYPAD_LIGHT_PWM
                    }
                    pmd_RDAWrite(RDA_ADDR_LED_SETTING5, g_pmdLedSetting5);
                    hal_SysExitCriticalSection(status);
                }
                break;
                case PMD_LEVEL_LOUD_SPEAKER:
                // Speaker level is controlled by ABB rather than PMU on Gallite
                default:
                    PMD_ASSERT(FALSE, "Trying to use an invalid id for type PMD_LEVEL_TYPE_OPAL", id);
                    break;
            }
            break;
        case PMD_LEVEL_TYPE_BACKLIGHT:
        {
            if (level > 7)
            {
                level = 7;
            }

            if (level <= 3)
            {
                g_pmdMisc48H |= PMU_IDIV10_LED;  //  set the current to 1/10 of the target vlaue.
            }
            else
            {
                g_pmdMisc48H &= ~(PMU_IDIV10_LED);
            }


            PMD_TRACE(PMD_DBG_TRC|TSTDOUT, 0,  "PMD_LEVEL_TYPE_BACKLIGHT level=%d",level);

            value = g_pmdConfig->lightLevelToBacklight[level];
            // Change value range from [0, 255] (for HAL PWM) to [0, 15] (for PMU DIM)
            value = (value & 0xff) >> 4;

            status = hal_SysEnterCriticalSection();
            if (0 == level)
            {
                g_pmdBacklightOn = FALSE;
                g_pmdLedSetting2 |= RDA_PMU_BL_OFF_ACT;
            }
            else
            {
                g_pmdBacklightOn = TRUE;
                g_pmdLedSetting2 &= ~RDA_PMU_BL_OFF_ACT;
                g_pmdLedSetting2 = PMU_SET_BITFIELD(g_pmdLedSetting2,
                                                    RDA_PMU_BL_IBIT_ACT,
                                                    value);
            }
            pmd_RDAWrite(RDA_ADDR_LED_SETTING2, g_pmdLedSetting2);
            pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK_48H, g_pmdMisc48H);
            // Open/close BLLED along with backlight to increase the lifetime
            // of backlight module, especially when charge pump is active
            PMD_LDO_ID_T blLdo = { .opal = PMD_LDO_BLLED, };
            pmd_EnableOpalLdo(blLdo, (level != 0),
                              PMD_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));
            hal_SysExitCriticalSection(status);
#if (LCD_BACKLIGHT_CHARGE_PUMP)
            pmd_UpdateBacklightChargePump();
#endif
        }
        break;
        default:
            PMD_ASSERT(FALSE, "Trying to use an invalid type of level", g_pmdConfig->level[id].type);
            break;
    }
}


// ============================================================================
// pmd_EnableMemoryPower
// ----------------------------------------------------------------------------
/// Power up/down memory
// ============================================================================
PUBLIC VOID pmd_EnableMemoryPower(BOOL on)
{
    PMD_LDO_ID_T ldo;
    ldo.opal = PMD_LDO_MEM;
    pmd_EnableOpalLdo(ldo, on, PMD_PROFILE_MODE_NORMAL, DEFAULT_LDO_USER);
}


///      48H[11]  Va24_vsel
///          48H[10:8]  Va24_ibit<2:0>
///
///        Voltage(V)
///     1   000 1.884
///         001 1.932
///     010 1.986
///     011 2.049
///     100 2.124
///     101 2.212
///     110 2.320
///     111 2.452
///
///    0    000 2.130
///     001 2.195
///     010 2.269
///     011 2.355
///     100 2.455
///     101 2.576
///     110 2.723
///     111 2.904
///


PUBLIC VOID pmd_SetVa24Buck2Voltage(UINT8 volt)
{
    g_pmdMisc48H = PMU_SET_BITFIELD(g_pmdMisc48H,
                                    PMU_VA24_IBIT,
                                    volt);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK_48H, g_pmdMisc48H);
}

#define CHIP_DIE_8955_FOR_TEMP_DEBUG
// ============================================================================
// pmd_InitCoreVoltage
// ----------------------------------------------------------------------------
/// Init the core voltage level in active/idle mode
// ============================================================================
PUBLIC VOID pmd_InitCoreVoltage(VOID)
{
    pmd_SetVa24Buck2Voltage(3);
}

// ============================================================================
// pmd_GetVoreVoltageDDRIndex
// ----------------------------------------------------------------------------
/// Get core voltage for DDR timing index
// ============================================================================
PUBLIC UINT8 HAL_BOOT_FUNC pmd_GetVoreVoltageDDRIndex(VOID)
{
    return g_pmdVcoreLdoMode? g_pmdVcoreVoltage : (g_pmdVcoreVoltage | 0x80);
}

PRIVATE VOID PMD_FUNC_INTERNAL pmd_SetVcoreVoltageReal(UINT8  coreVolt, UINT16 regIdx,UINT16 data)
{
    HAL_SYS_MEM_FREQ_T ddrfreq = memd_GetRamClk();
    HAL_DDR_TIMING_T timing;

    g_pmdVcoreVoltage = coreVolt;
    memd_DdrGetTiming(pmd_GetVoreVoltageDDRIndex(), ddrfreq, &timing);
    memd_ChangeVcore(1, &regIdx, &data, &timing);
}

// ============================================================================
// pmd_ForceSetCoreVoltage
// ----------------------------------------------------------------------------
/// Set vcore mode and value without update DDR timing. It is for debugging or
/// ramrun only.
// ============================================================================
VOID pmd_ForceSetCoreVoltage(BOOL ldo, UINT8 vcore)
{
    if (ldo)
    {
        g_pmdVcoreLdoMode = TRUE;
        g_pmdVcoreVoltage = vcore;
        g_pmdLdoBuck = PMU_SET_BITFIELD(g_pmdLdoBuck, RDA_PMU_VBUCK_VCORE_LDO_BIT, vcore);
        pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK_4BH, g_pmdLdoBuck);

        // [15:13] == 000: LDO
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1PfmOn = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);

        // [15:13] == 000: LDO
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1PfmOn = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
    }
    else
    {
        g_pmdVcoreLdoMode = FALSE;
        g_pmdVcoreVoltage = vcore;
        g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck, RDA_PMU_VBUCK1_BIT_NLP, vcore);
        pmd_OpalSpiWrite(RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);

        // [15:13] == 100: DCDC PWM
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = TRUE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1PfmOn = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);

        // [15:13] == 110: DCDC PFM
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = TRUE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1PfmOn = TRUE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
    }
}

// ============================================================================
// pmd_UpdateCoreVoltage
// ----------------------------------------------------------------------------
/// Update vcore based on user registry
// ============================================================================
PRIVATE UINT32 pmd_UpdateCoreVoltage(BOOL imm, UINT16* regIdx, UINT16* regData)
{
    UINT32 sc = hal_SysEnterCriticalSection();

    // find the highest vcore level
    UINT32 vcoreIndex = PMD_CORE_VOLTAGE_LOW;
    for (UINT32 i=0; i<PMD_CORE_VOLTAGE_USER_ID_QTY; i++)
    {
        if (vcoreIndex < g_pmdVcoreActRegistry[i])
        {
            vcoreIndex = g_pmdVcoreActRegistry[i];
        }
    }

    if (g_pmdVcoreCurrIndex == vcoreIndex)
    {
        hal_SysExitCriticalSection(sc);
        return 0;
    }

    UINT32 regCount = 0;
    if (g_pmdVcoreLdoMode)
    {
        int vcoreFrom = g_pmdLDO_VcoreValueActiveAry[g_pmdVcoreCurrIndex];
        int vcoreTo = g_pmdLDO_VcoreValueActiveAry[vcoreIndex];

        if (vcoreFrom != vcoreTo)
        {
            g_pmdLdoBuck = PMU_SET_BITFIELD(g_pmdLdoBuck,
                                            RDA_PMU_VBUCK_VCORE_LDO_BIT,
                                            vcoreTo);
            if (imm)
            {
                pmd_SetVcoreVoltageReal(vcoreTo, RDA_ADDR_LDO_BUCK_4BH, g_pmdLdoBuck);
            }
            else
            {
                g_pmdVcoreVoltage = vcoreTo;
                regIdx[regCount] = RDA_ADDR_LDO_BUCK_4BH;
                regData[regCount] = g_pmdLdoBuck;
                ++regCount;
            }
        }
    }
    else
    {
        int vcoreFrom = g_pmdDcdcVcoreValueActiveAry[g_pmdVcoreCurrIndex];
        int vcoreTo = g_pmdDcdcVcoreValueActiveAry[vcoreIndex];

        if (vcoreFrom != vcoreTo)
        {
            g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                             RDA_PMU_VBUCK1_BIT_NLP,
                                             vcoreTo);
            if (imm)
            {
                pmd_SetVcoreVoltageReal(vcoreTo, RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);
            }
            else
            {
                g_pmdVcoreVoltage = vcoreTo;
                regIdx[regCount] = RDA_ADDR_DCDC_BUCK;
                regData[regCount] = g_pmdDcdcBuck;
                ++regCount;
            }
        }
    }

    switch (g_pmdVcoreCurrIndex)
    {
        case PMD_CORE_VOLTAGE_LOW:      PMD_PROFILE_WINDOW_EXIT(PMD_CORE_VOLTAGE_IS_LOW); break;
        case PMD_CORE_VOLTAGE_MEDIUM:   PMD_PROFILE_WINDOW_EXIT(PMD_CORE_VOLTAGE_IS_MEDIUM); break;
        case PMD_CORE_VOLTAGE_HIGH:     PMD_PROFILE_WINDOW_EXIT(PMD_CORE_VOLTAGE_IS_HIGH); break;
        case PMD_CORE_VOLTAGE_BTTEST:
        default:                        PMD_PROFILE_WINDOW_EXIT(PMD_CORE_VOLTAGE_IS_BTTEST); break;
    }
    switch (vcoreIndex)
    {
        case PMD_CORE_VOLTAGE_LOW:      PMD_PROFILE_WINDOW_ENTER(PMD_CORE_VOLTAGE_IS_LOW); break;
        case PMD_CORE_VOLTAGE_MEDIUM:   PMD_PROFILE_WINDOW_ENTER(PMD_CORE_VOLTAGE_IS_MEDIUM); break;
        case PMD_CORE_VOLTAGE_HIGH:     PMD_PROFILE_WINDOW_ENTER(PMD_CORE_VOLTAGE_IS_HIGH); break;
        case PMD_CORE_VOLTAGE_BTTEST:
        default:                        PMD_PROFILE_WINDOW_ENTER(PMD_CORE_VOLTAGE_IS_BTTEST); break;
    }

    g_pmdVcoreCurrIndex = vcoreIndex;

    hal_SysExitCriticalSection(sc);
    return regCount;
}

// ============================================================================
// pmd_GetCoreVoltageRequest
// ----------------------------------------------------------------------------
/// Get core voltage level request by user id.
// ============================================================================
PUBLIC PMD_CORE_VOLTAGE_T pmd_GetCoreVoltageRequest(PMD_CORE_VOLTAGE_USER_ID_T user)
{
    return g_pmdVcoreActRegistry[user];
}

// ============================================================================
// pmd_SetCoreVoltage
// ----------------------------------------------------------------------------
/// Set the core voltage level in active mode
// ============================================================================
PUBLIC VOID pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_ID_T user,
                               PMD_CORE_VOLTAGE_T coreVolt)
{
    g_pmdVcoreActRegistry[user] = coreVolt;

#if defined(FPGA) || defined(_FLASH_PROGRAMMER)
    return;
#endif

    return pmd_UpdateCoreVoltage(TRUE, NULL, NULL);
}

#ifdef FM_LDO_WORKAROUND
// ============================================================================
// pmd_EnableDcdcPower
// ----------------------------------------------------------------------------
/// enable DCDC or ldo of mode for Power
// ============================================================================
PUBLIC BOOL pmd_EnableDcdcPower(BOOL on)
{
    // switch DCDC and LDO
    UINT16 reg[3];
    UINT16 data[3];

    UINT32 sc = hal_SysEnterCriticalSection();
    UINT16 oldProfileNormal = g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg;
    UINT16 oldProfileLowPower = g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg;
    UINT16 oldLdoBuck = g_pmdLdoBuck;
    UINT16 oldDcdcBuck = g_pmdDcdcBuck;
    BOOL oldLdoMode = g_pmdVcoreLdoMode;
    UINT8 oldVcore = g_pmdVcoreVoltage;

    if (!on)   // LDO mode
    {
        // [15:13] == 000: LDO
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1PfmOn = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;

        // [15:13] == 000: LDO
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1PfmOn = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;

        g_pmdVcoreVoltage = g_pmdLDO_VcoreValueActiveAry[g_pmdVcoreCurrIndex];
        g_pmdLdoBuck = PMU_SET_BITFIELD(g_pmdLdoBuck,
                                        RDA_PMU_VBUCK_VCORE_LDO_BIT,
                                        g_pmdVcoreVoltage);

        reg[0] = RDA_ADDR_LDO_BUCK_4BH;
        data[0] = g_pmdLdoBuck;
        reg[1] = RDA_ADDR_LDO_ACTIVE_SETTING1;
        data[1] = g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg;
        reg[2] = RDA_ADDR_LDO_LP_SETTING1;
        data[2] = g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg;

        g_pmdVcoreLdoMode = TRUE;
    }
    else     //DCDC mode
    {
        // [15:13] == 100: DCDC PWM
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = TRUE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1PfmOn = FALSE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;

        // [15:13] == 110: DCDC PFM
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = TRUE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1PfmOn = TRUE;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;

        g_pmdVcoreVoltage = g_pmdDcdcVcoreValueActiveAry[g_pmdVcoreCurrIndex];
        g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                         RDA_PMU_VBUCK1_BIT_NLP,
                                         g_pmdVcoreVoltage);

        reg[0] = RDA_ADDR_DCDC_BUCK;
        data[0] = g_pmdDcdcBuck;
        reg[1] = RDA_ADDR_LDO_ACTIVE_SETTING1;
        data[1] = g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg;
        reg[2] = RDA_ADDR_LDO_LP_SETTING1;
        data[2] = g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg;

        g_pmdVcoreLdoMode = FALSE;
    }

    HAL_SYS_MEM_FREQ_T ddrfreq = memd_GetRamClk();
    HAL_DDR_TIMING_T timing;
    memd_DdrGetTiming(pmd_GetVoreVoltageDDRIndex(), ddrfreq, &timing);
    BOOL changed = memd_ChangeVcore(3, &reg[0], &data[0], &timing);
    if (!changed)
    {
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg = oldProfileNormal;
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg = oldProfileLowPower;
        g_pmdLdoBuck = oldLdoBuck;
        g_pmdDcdcBuck = oldDcdcBuck;
        g_pmdVcoreLdoMode = oldLdoMode;
        g_pmdVcoreVoltage = oldVcore;
    }

    hal_SysExitCriticalSection(sc);
    return changed;
}
#endif //FM_LDO_WORKAROUND

// ============================================================================
// pmd_SetCoreVoltage
// ----------------------------------------------------------------------------
/// Set the core voltage level in active mode
// ============================================================================
PUBLIC UINT32 pmd_SetCoreVoltageDelayed(PMD_CORE_VOLTAGE_USER_ID_T user,
                                        PMD_CORE_VOLTAGE_T coreVolt,
                                        UINT16* regIdx,
                                        UINT16* regData)
{
    g_pmdVcoreActRegistry[user] = coreVolt;

#if defined(FPGA) || defined(_FLASH_PROGRAMMER)
    return;
#endif

    return pmd_UpdateCoreVoltage(FALSE, regIdx, regData);
}

// ============================================================================
// pmd_SetVcoreToDefaultVolt
// ----------------------------------------------------------------------------
/// Set core voltage to default values.
// ============================================================================
PUBLIC VOID HAL_BOOT_FUNC pmd_SetVcoreToDefaultVolt(VOID)
{
    boot_XcvConfigure();

    // Reset the registers except some special regs
    pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_SOFT_RESETN);
    boot_SysWaitMicrosecond(1000); // 1ms
    pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_REGISTER_RESETN|RDA_PMU_SOFT_RESETN);

    // Set LDO LP and NLP value
    g_pmdLdoBuck = PMU_SET_BITFIELD(g_pmdLdoBuck,
                                    RDA_PMU_VBUCK_VCORE_LDO_BIT,
                                    PMD_LDO_VCORE_VALUE_INIT);
    g_pmdLdoBuck = PMU_SET_BITFIELD(g_pmdLdoBuck,
                                    RDA_PMU_VBUCK_VCORE_LDO_LP_BIT,
                                    PMD_LDO_VCORE_VALUE_LP);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK_4BH, g_pmdLdoBuck);

    // Set DCDC LP and NLP value
    g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                     RDA_PMU_VBUCK1_BIT_LP,
                                     PMD_DCDC_VCORE_VALUE_LP);
    g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                     RDA_PMU_VBUCK1_BIT_NLP,
                                     PMD_DCDC_VCORE_VALUE_INIT);
    pmd_OpalSpiWrite(RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);

    // Init DCDC frequency
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK1, gc_pmdLdoBuck1Act);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK2, gc_pmdLdoBuck2);

#ifdef _USE_VCORE_LDO_MODE_
    // [15:13] == 000: LDO
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = FALSE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1PfmOn = FALSE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);

    // [15:13] == 000: LDO
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = FALSE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1PfmOn = FALSE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

    g_pmdVcoreLdoMode = TRUE;
    g_pmdVcoreVoltage = PMD_LDO_VCORE_VALUE_INIT;
#else
    // [15:13] == 100: DCDC PWM
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = TRUE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1PfmOn = FALSE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);

    // [15:13] == 110: DCDC PFM
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = TRUE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1PfmOn = TRUE;
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

    g_pmdVcoreLdoMode = FALSE;
    g_pmdVcoreVoltage = PMD_DCDC_VCORE_VALUE_INIT;
#endif

    g_pmdVcoreCurrIndex = PMD_CORE_VOLTAGE_INIT;

    // Set Vmem voltage in active & lp mode
    g_pmdLdoProfileSetting3 = PMU_SET_BITFIELD(g_pmdLdoProfileSetting3,
                              RDA_PMU_VMEM_IBIT,
                              PMD_VMEM_LEVEL);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoProfileSetting3);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING3, g_pmdLdoProfileSetting3);

    g_pmdCalibSetting2 &= ~RDA_PMU_PD_CHARGE_LDO;
    pmd_OpalSpiWrite(RDA_ADDR_CALIBRATION_SETTING2, g_pmdCalibSetting2);

    // Set PMU to active stat
    g_pmdIrqSettingLatest = RDA_PMU_PD_MODE_SEL;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, g_pmdIrqSettingLatest);

    // Maybe wait a while (2ms) is helpfull
    boot_SysWaitMicrosecond(2000); // 2ms
}

PUBLIC VOID pmd_SetSpiMemVoltage(UINT8 level)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoProfileSetting3 =
        PMU_SET_BITFIELD(g_pmdLdoProfileSetting3,
                         RDA_PMU_VSPIMEM_IBIT,
                         level);
    pmd_RegWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoProfileSetting3);
    pmd_RegWrite(RDA_ADDR_LDO_LP_SETTING3, g_pmdLdoProfileSetting3);
    hal_SysExitCriticalSection(scStatus);
}

PUBLIC VOID HAL_FUNC_ROM pmd_SetPadVoltage(UINT8 level)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoProfileSetting3 =
        PMU_SET_BITFIELD(g_pmdLdoProfileSetting3,
                         RDA_PMU_VPAD_IBIT,
                         level);
    pmd_RegWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoProfileSetting3);
    pmd_RegWrite(RDA_ADDR_LDO_LP_SETTING3, g_pmdLdoProfileSetting3);
    hal_SysExitCriticalSection(scStatus);
}

VOID pmd_InitTouchScreen(VOID)
{
    UINT16 reg;
    // Set sar_vref_bit
    reg = pmd_OpalSpiRead(RDA_ADDR_TOUCH_SCREEN_CONTROL);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_SAR_VERF_BIT, 1);
    pmd_OpalSpiWrite(RDA_ADDR_TOUCH_SCREEN_CONTROL, reg);

    // Set delay_before_samp and timer_penirq_stable
    reg = pmd_OpalSpiRead(RDA_ADDR_AUDIO_CONTROL);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_DELAY_BEFORE_SAMP, 3);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_TIMER_PEN_IRQ_STABLE, 3);
#ifdef _USED_TSC_KEY_
    // Increase the sample interval for TSC key
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_TIMER_INT_TO_X_SEL, 7);
#endif
    pmd_OpalSpiWrite(RDA_ADDR_AUDIO_CONTROL, reg);
}

VOID pmd_InitSpeakerPa(VOID)
{
    // For U03/U04 chips, L_CLG and R_CLG should be reset at boot
    // time, in order to reduce current leak.
    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING2,
                     gc_pmdSpeakerPaSetting2|RDA_PMU_INTERFACE_MODE_EN);
    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING4,
                     RDA_PMU_RESET_R_CLG_DR|RDA_PMU_RESET_L_CLG_DR);
    hal_TimDelay(1 MS_WAITING);
    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING4, 0);

    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING2, gc_pmdSpeakerPaSetting2);
    // Init PA setting 1 & 3
    pmd_SpeakerPaRestoreMode();
}

VOID pmd_InitRdaPmu(VOID)
{
    // Init DCDC Vcore in active & lp mode
    pmd_InitCoreVoltage();

    // Set Vpad voltage in active mode
    pmd_SetPadVoltage(gc_pmdPadLevelAct);

    // Set v_mmc to 3V
    pmd_SetMmcVoltage(1);

    // Disable touch screen irqs (they will be enabled later if needed)
    // Disable key irq
    // Enable charger irq
    // Enter active mode
    g_pmdIrqSettingLatest = 0;

#ifdef CHIP_DIE_8955
    pmd_RegIrqSettingSet(RDA_PMU_HOLD_PRECHARGER_EFUSE| RDA_PMU_PD_MODE_SEL);
//pmd_RegIrqSettingSet(RDA_PMU_HOLD_PRECHARGER_EFUSE);

#else
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_MASK|RDA_PMU_PENIRQ_MASK|
                         RDA_PMU_KEYIRQ_MASK|RDA_PMU_HOLD_PRECHARGER_EFUSE|
                         RDA_PMU_PD_MODE_SEL);

#endif

    // Disable headphone detection irqs (they will be enabled later if needed)
    // Enter active mode
    g_pmdHpDetectSettingLatest = 0;
    pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);
    hal_HstSendEvent(0x77770006);

    // Init sar_vref_bit for GPADC and TSD
    pmd_InitTouchScreen();

    // Init speaker PA
    pmd_InitSpeakerPa();

    // Init charger
    pmd_InitCharger();

}

PUBLIC UINT32 pmd_GetBatteryGpadcChannel(VOID)
{
    return g_pmdConfig->batteryGpadcChannel;
}

#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
#if (LCD_BACKLIGHT_CHARGE_PUMP)
PRIVATE VOID pmd_EnableChargePump(BOOL enable)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    if (enable)
    {
        // Direct-reg to enable charge pump
        g_pmdSpeakerPaSetting1 |= RDA_PMU_PD_CP_DR|RDA_PMU_BYPASS_CP_DR;
        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_PD_CP_REG|RDA_PMU_BYPASS_CP_REG);
    }
    else
    {
        // Not to direct-reg charge pump
        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_PD_CP_DR|RDA_PMU_BYPASS_CP_DR);
    }
    hal_AudForceDcdc4ClockOn(enable);
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);

    hal_SysExitCriticalSection(scStatus);
}
#endif
#endif

PUBLIC UINT16 pmd_GetScreenAntiFlickerVolt(BOOL actThresh)
{
    UINT16 volt = g_pmdConfig->batteryMVScreenAntiFlicker;
    if (volt == 0)
    {
        // Return a large value to assume always on
        volt = 10000;
    }
    if (!actThresh)
    {
        volt += g_pmdConfig->batteryOffsetScreenNormal;
    }
    return volt;
}

#if (LCD_BACKLIGHT_CHARGE_PUMP)
PRIVATE VOID pmd_UpdateBacklightChargePump(VOID)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    pmd_EnableChargePump((g_pmdScreenAntiFlickerEnabled && g_pmdBacklightOn));

    hal_SysExitCriticalSection(scStatus);
#endif
}
#endif // LCD_BACKLIGHT_CHARGE_PUMP

PUBLIC VOID pmd_EnableScreenAntiFlicker(BOOL enable)
{
    g_pmdScreenAntiFlickerEnabled = enable;

#if (LCD_BACKLIGHT_CHARGE_PUMP)


    pmd_UpdateBacklightChargePump();

#else // !LCD_BACKLIGHT_CHARGE_PUMP

    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (enable)
    {
        g_pmdLdoProfileSetting5.vRgbLedIbit = gc_pmdBlLedOutAntiFlicker.vRgbLedIbit;
        g_pmdLdoProfileSetting5.vRgbLedVsel = gc_pmdBlLedOutAntiFlicker.vRgbLedVsel;
    }
    else
    {
        g_pmdLdoProfileSetting5.vRgbLedIbit = g_pmdBlLedOutDefault.vRgbLedIbit;
        g_pmdLdoProfileSetting5.vRgbLedVsel = g_pmdBlLedOutDefault.vRgbLedVsel;
    }
    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
    hal_SysExitCriticalSection(scStatus);

#endif // !LCD_BACKLIGHT_CHARGE_PUMP
}

PUBLIC VOID pmd_SpeakerPaCpLowOutput(BOOL low)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    g_pmdSpeakerPaCpLowOutput = low;
#endif
}

#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
PRIVATE VOID pmd_SpeakerPaSetCpLevel(UINT32 level)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    g_pmdSpeakerPaSetting1 =
        PMU_SET_BITFIELD(g_pmdSpeakerPaSetting1,
                         RDA_PMU_VCP_OUT_BIT,
                         level);
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);
    hal_SysExitCriticalSection(scStatus);
}
#endif

PUBLIC BOOL pmd_IsSpeakerDouble()
{
#if (AUD_SPK_LINE_L_R_DOUBLE)
    return TRUE;
#else
    return FALSE;
#endif
}
PUBLIC VOID pmd_SpeakerPaPreEnable(BOOL enable)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    // Class K mode might need a higher vcore voltage and a higher charge pump
    // output voltage.
    // At least one PA is in class K mode
    if (enable)
    {
// Speak CLK enable
        g_pmdMisc48H |= PMU_CLK_CLG_EN;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK_48H, g_pmdMisc48H);

    }
    else
    {
    }
#endif
}

PUBLIC VOID pmd_SpeakerPaPostEnable(BOOL enable)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    // Class K mode might need a higher vcore voltage and a higher charge pump
    // output voltage.
    // At least one PA is in class K mode
    if (enable)
    {
    }
    else
    {
        g_pmdMisc48H &= ~PMU_CLK_CLG_EN;
        pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK_48H, g_pmdMisc48H);

    }
#endif
}

PUBLIC VOID pmd_SpeakerPaSelectChannel(BOOL leftChanEn, BOOL rightChanEn)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    //  speaker L/R lines should NOT be enabled at the same time in 8955 prj
    PMD_ASSERT(!(leftChanEn && rightChanEn),  "Cannot enable both class AB PAs");

    UINT16 chanSel = RDA_PMU_PD_L_CLG_DR|RDA_PMU_PD_O_L_CLG_DR|
                     RDA_PMU_PD_R_CLG_DR|RDA_PMU_PD_O_R_CLG_DR;

    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel leftChanEn=%d rightChanEn=%d ", leftChanEn, rightChanEn);

#ifdef RECV_USE_SPK_LINE
    if(leftChanEn && !rightChanEn)
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel Class AB ");

        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3,( g_pmdSpeakerPaSetting3&( ~RDA_PMU_DMODE_EN_CLG_MASK))|RDA_PMU_DMODE_R_CLAB_L_CLAB);
    }
    else if(!leftChanEn && rightChanEn)
    {
#if (AUD_SPK_LINE_L_CLASS_AB) && (AUD_SPK_LINE_R_CLASS_AB)
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel Class AB ");

        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3,( g_pmdSpeakerPaSetting3&( ~RDA_PMU_DMODE_EN_CLG_MASK))|RDA_PMU_DMODE_R_CLAB_L_CLAB);
#else
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel Class D ");

        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, (g_pmdSpeakerPaSetting3&( ~RDA_PMU_DMODE_EN_CLG_MASK))|RDA_PMU_DMODE_R_CLD_L_CLD);
#endif
    }

#endif

    //

    if(!leftChanEn && !rightChanEn)   //close pa
    {
        chanSel |= RDA_PMU_RESET_R_CLG_DR|RDA_PMU_RESET_L_CLG_DR|RDA_PMU_RESET_L_CLG_REG|RDA_PMU_RESET_R_CLG_REG;
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);
        hal_TimDelay(1 MS_WAITING);

        chanSel |= RDA_PMU_PD_O_L_CLG_REG|RDA_PMU_PD_O_R_CLG_REG;
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);
        hal_TimDelay(1 MS_WAITING);

        chanSel |= RDA_PMU_PD_L_CLG_REG|RDA_PMU_PD_R_CLG_REG;
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);
        hal_TimDelay(1 MS_WAITING);
    }
    else
    {
        //open pd
        chanSel |= RDA_PMU_PD_O_L_CLG_REG;
        chanSel |= RDA_PMU_PD_O_R_CLG_REG;
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);

        hal_TimDelay(1 MS_WAITING);


        //open pd o
        if(leftChanEn)
        {
            chanSel &= ~RDA_PMU_PD_O_L_CLG_REG;
        }
        else     // close pd
        {
            chanSel |= RDA_PMU_RESET_L_CLG_DR|RDA_PMU_RESET_L_CLG_REG|RDA_PMU_PD_L_CLG_REG;
        }

        if (rightChanEn)
        {
            chanSel &= ~RDA_PMU_PD_O_R_CLG_REG;
        }
        else
        {
            chanSel |= RDA_PMU_RESET_R_CLG_DR|RDA_PMU_RESET_R_CLG_REG||RDA_PMU_PD_R_CLG_REG;
        }
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);

        hal_TimDelay(1 MS_WAITING);

        //reset
        if(leftChanEn)
        {
            chanSel |=  RDA_PMU_RESET_L_CLG_DR;
        }
        else
        {
            chanSel &= ~RDA_PMU_RESET_L_CLG_DR;
        }

        if (rightChanEn)
        {
            chanSel |=  RDA_PMU_RESET_R_CLG_DR;
        }
        else
        {
            chanSel &= ~RDA_PMU_RESET_R_CLG_DR;
        }
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);

    }
#endif
}

PUBLIC VOID pmd_SpeakerPaSetMode(PMD_SPEAKER_PA_MODE_T leftChanMode,
                                 PMD_SPEAKER_PA_MODE_T rightChanMode)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))


    UINT32 scStatus = hal_SysEnterCriticalSection();

    // Set speaker modes
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_DMODE_EN_CLG_MASK;

    if (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
    {
        if (rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLAB_L_CLAB;
        }
        else
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLD_L_CLAB;
        }
    }
    else
    {
        if (rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
        {
            PMD_ASSERT(1,  "Speaker Line don't support the both R clab, L clad ");

            // g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLAB_L_CLD;
        }
        else
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLD_L_CLD;
        }
    }


    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);

    g_pmdSpeakerLeftChanMode = leftChanMode;
    g_pmdSpeakerRightChanMode = rightChanMode;

    hal_SysExitCriticalSection(scStatus);

#endif // !(_FLASH_PROGRAMMER || _T_UPGRADE_PROGRAMMER)
}

PUBLIC VOID pmd_SpeakerPaRestoreMode(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    // Restore speaker modes
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_DMODE_EN_CLG_MASK;
    g_pmdSpeakerPaSetting3 |=
#if (AUD_SPK_LINE_L_CLASS_AB) && (AUD_SPK_LINE_R_CLASS_AB)
        RDA_PMU_DMODE_R_CLAB_L_CLAB
#elif (AUD_SPK_LINE_L_CLASS_AB)
        RDA_PMU_DMODE_R_CLD_L_CLAB
#elif (AUD_SPK_LINE_R_CLASS_AB)
        RDA_PMU_DMODE_R_CLAB_L_CLD
#else // Both L and R lines are in class D mode
        RDA_PMU_DMODE_R_CLD_L_CLD
#endif
        ;

#ifndef RECV_USE_SPK_LINE
#if (AUD_SPK_LINE_L_CLASS_AB) && (AUD_SPK_LINE_R_CLASS_AB)
    g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG;
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_L_ENABLE_CLG;

#elif (AUD_SPK_LINE_L_CLASS_AB)
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_R_ENABLE_CLG;
    g_pmdSpeakerPaSetting3 |= RDA_PMU_L_ENABLE_CLG;

#elif (AUD_SPK_LINE_R_CLASS_AB)
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_L_ENABLE_CLG;
    g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG;

#else //onle enable  R lines are in class D mode
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_L_ENABLE_CLG;
    g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG;
#endif
#endif



    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);

#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
#if (AUD_SPK_LINE_L_CLASS_AB)
    g_pmdSpeakerLeftChanMode = PMD_SPEAKER_PA_MODE_CLASS_AB;
#elif (AUD_SPK_LINE_NO_CLASS_K)
    g_pmdSpeakerLeftChanMode = PMD_SPEAKER_PA_MODE_CLASS_D;
#else
    g_pmdSpeakerLeftChanMode = PMD_SPEAKER_PA_MODE_CLASS_K;
#endif

#if (AUD_SPK_LINE_R_CLASS_AB)
    g_pmdSpeakerRightChanMode = PMD_SPEAKER_PA_MODE_CLASS_AB;
#elif (AUD_SPK_LINE_NO_CLASS_K)
    g_pmdSpeakerRightChanMode = PMD_SPEAKER_PA_MODE_CLASS_D;
#else
    g_pmdSpeakerRightChanMode = PMD_SPEAKER_PA_MODE_CLASS_K;
#endif
#endif // !(_FLASH_PROGRAMMER || _T_UPGRADE_PROGRAMMER)

    hal_SysExitCriticalSection(scStatus);
}

VOID pmd_TsdClearEomIrqInternal(VOID)
{
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_CLEAR);
}

BOOL pmd_ResetTouch(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdDebug3 |= RDA_PMU_RESETN_TSC_DR;
    g_pmdDebug3 &= ~RDA_PMU_RESETN_TSC_REG;
    pmd_RDAWrite(RDA_ADDR_DEBUG3, g_pmdDebug3);

    g_pmdDebug3 &= ~RDA_PMU_RESETN_TSC_DR;
    g_pmdDebug3 |= RDA_PMU_RESETN_TSC_REG;
    pmd_RDAWrite(RDA_ADDR_DEBUG3, g_pmdDebug3);

    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}

VOID pmd_TsdClearTouchIrq(VOID)
{
#ifdef _USED_TSC_KEY_
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_KEYIRQ_CLEAR);
#else
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_PENIRQ_CLEAR);
#endif
}

BOOL pmd_TsdTscEnabled(VOID)
{
#ifdef _USED_TSC_KEY_
    return g_pmdLdoSettings.keyEnable;
#else
    return g_pmdLdoSettings.tscEnable;
#endif
}

VOID pmd_TsdEnableTsc(BOOL enable)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

#ifdef _USED_TSC_KEY_
    g_pmdLdoSettings.keyEnable = enable;
#else
    g_pmdLdoSettings.tscEnable = enable;
#endif
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    pmd_ResetTouch();

    hal_SysExitCriticalSection(scStatus);
}

BOOL pmd_TsdEnableIrq(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    // enable touch screen
    pmd_TsdEnableTsc(TRUE);

    // clear previous pen irq
    pmd_TsdClearTouchIrq();

#ifdef _USED_TSC_KEY_
    // unmask TSC KEY
    pmd_RegIrqSettingClr(RDA_PMU_KEYIRQ_MASK);

#ifdef _USED_TSC_KEY_IN_LP_MODE_
    // Disable bandgap LP mode
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_TSC_KEY, FALSE);
    // Enable clock-4M and avdd3 in LP mode
    pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_TSC_KEY, TRUE);
#endif

#else
    // unmask pen irq
    pmd_RegIrqSettingClr(RDA_PMU_PENIRQ_MASK);
#endif

    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}

BOOL pmd_TsdDisableIrq(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    // disable touch screen
    pmd_TsdEnableTsc(FALSE);

#ifdef _USED_TSC_KEY_
    // mask TSC KEY
    pmd_RegIrqSettingSet(RDA_PMU_KEYIRQ_MASK);

#ifdef _USED_TSC_KEY_IN_LP_MODE_
    // Enable bandgap LP mode
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_TSC_KEY, TRUE);
    // Disable clock-4M and avdd3 in LP mode
    pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_TSC_KEY, FALSE);
#endif

#else
    // mask pen irq
    pmd_RegIrqSettingSet(RDA_PMU_PENIRQ_MASK);
#endif
    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}

// NOTE:
// Both the Charger detection and the headphone detection are now
// run in TIMER IRQ: pmd_IrqHanderInternal, pmd_ChargerManager,
// and pmd_EarDetectDelay.
// They will be synchronized in nature and
// there is no need to lock interrupt.
PRIVATE VOID pmd_IrqHanderInternal(UINT32 reqType)
{
    UINT32 nextAction = 0;
    UINT16 irq = 0;
    UINT16 chrStatus = 0;
#ifdef ABB_HP_DETECT
    UINT16 hpDetect= 0;
#endif

    // Read charger IRQ status
    if (reqType & PMD_IRQ_DEBOUNCE_REQ)
    {
        irq = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
    }
    // Read charger status
    if (reqType & PMD_IRQ_CHECK_CHARGER)
    {
        chrStatus = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    }
#ifdef ABB_HP_DETECT
    // Read headphone IRQ status
    hpDetect = pmd_OpalSpiRead(RDA_ADDR_HP_DETECT_SETTING);
#endif

#if 0
    PMD_TRACE(PMD_INFO_TRC, 0,
              "[%08d] PMD-INT: reqType=%d, irq(%d)=0x%04x, chr=0x%04x",
              hal_TimGetUpTime()*1000/16384, reqType,
              g_pmdChargerInIrqEnabled, irq, chrStatus);
#ifdef ABB_HP_DETECT
    PMD_TRACE(PMD_INFO_TRC, 0,
              "-- hp(%d)=0x%04x, hpStart=%d",
              g_pmdHpOutIrqEnabled, hpDetect, g_pmdHpOutDetectStarted);
#endif
#endif

    if (reqType & PMD_IRQ_DEBOUNCE_REQ)
    {
        // Check IRQ source
        if (irq & RDA_PMU_INT_CHR_ON)
        {
            pmd_RegIrqSettingSet(RDA_PMU_INT_CHR_CLEAR);
            if (g_pmdChargerInIrqEnabled)
            {
                nextAction |= PMD_IRQ_CHECK_CHARGER;
            }
        }
#ifdef ABB_HP_DETECT
        if (hpDetect & RDA_PMU_HP_OUT)
        {
            pmd_RegHpDetectSettingSet(RDA_PMU_HP_OUT_CLEAR);
            // Check current headphone status
            if ((hpDetect & RDA_PMU_HP_DETECT) == 0)
            {
                if (g_pmdHpOutIrqEnabled)
                {
                    nextAction |= PMD_IRQ_CHECK_HP;
                    if (ear_handler != NULL)
                    {
                        (*ear_handler)(PMD_EAR_STATUS_UNKNOWN);
                    }
                }
            }
        }
#endif
        if (nextAction)
        {
            // Never stop PMD IRQ function timers, as they have different jobs.
            sxr_StartFunctionTimer(PMD_TIME_IRQ_DEBOUNCE, pmd_IrqHanderInternal,
                                   nextAction, 0);
        }
    }

    if (reqType & PMD_IRQ_CHECK_CHARGER)
    {
        if (g_pmdChargerInIrqEnabled && (chrStatus & RDA_PMU_CHR_AC_ON))
        {
            g_pmdChargerInIrqEnabled = FALSE;
            // A timer will be started, and it will read the acutal charger status in non-blocking mode
            pmd_DcOnHandler(TRUE);
        }
    }

#ifdef ABB_HP_DETECT
    if (reqType & PMD_IRQ_CHECK_HP)
    {
        // Check current headphone status
        if (g_pmdHpOutIrqEnabled && (hpDetect & RDA_PMU_HP_DETECT) == 0)
        {
            g_pmdHpOutIrqEnabled = FALSE;
            if (ear_handler != NULL)
            {
                (*ear_handler)(PMD_EAR_STATUS_OUT);
            }
            // Enable GPIO detection for headphone plug-in
            HAL_GPIO_CFG_T eraCfg;
            eraCfg.irqMask.rising = FALSE;
            eraCfg.irqMask.falling = TRUE;
            eraCfg.irqMask.debounce = TRUE;
            eraCfg.irqMask.level = FALSE;
            hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
            hal_GpioResetIrq(g_ear_gpio);
        }
    }
#endif

    if (
        // This IRQ does NOT expect to check charger status
        ( (reqType & PMD_IRQ_DEBOUNCE_REQ) &&
          (nextAction & PMD_IRQ_CHECK_CHARGER) == 0 ) ||
        // The debounced charger status has been checked
        (reqType & PMD_IRQ_CHECK_CHARGER)
    )
    {
        if (g_pmdChargerInIrqEnabled)
        {
            pmd_RegIrqSettingClr(RDA_PMU_INT_CHR_MASK);
        }
    }
#ifdef ABB_HP_DETECT
    if (
        // This IRQ does NOT expect to check HP status
        ( (reqType & PMD_IRQ_DEBOUNCE_REQ) &&
          (nextAction & PMD_IRQ_CHECK_HP) == 0 ) ||
        // The debounced HP status has been checked
        (reqType & PMD_IRQ_CHECK_HP)
    )
    {
        if (g_pmdHpOutIrqEnabled)
        {
            pmd_RegHpDetectSettingClr(RDA_PMU_HP_OUT_MASK);
        }
    }
#endif
}

PUBLIC VOID pmd_IrqHandler(UINT8 interruptId)
{
    // Mask all available PMU IRQ sources.
    // IRQ handler cannot be blocked, so we write to PMU directly here.
    // (NOTE: PEN IRQ and KEY IRQ is driven by another interrupt line,
    // which is connected to GPIO_0 at baseband side.)
    pmd_RegIrqSettingSet(RDA_PMU_INT_CHR_MASK|RDA_PMU_EOMIRQ_MASK);
    pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);

    // 1. Never directly invoke pmd_IrqHanderInternal in PMU IRQ.
    //    Invoke it in TIMER IRQ instead for the purpose of synchronization.
    // 2. Never stop PMD IRQ function timers, as they have different jobs.
    sxr_StartFunctionTimer(PMD_TIME_IRQ_ACT_CS_RETRY, pmd_IrqHanderInternal,
                           PMD_IRQ_DEBOUNCE_REQ, 0);

    PMD_TRACE(PMD_INFO_TRC, 0, "[%08d] pmd_IrqHandler",
              hal_TimGetUpTime()*1000/16384);

    return;
}

BOOL pmd_TsdReadCoordinatesInternal(UINT16* gpadc_x, UINT16* gpadc_y)
{
#ifdef _USED_TSC_KEY_
    UINT16 x;

    x = hal_AnaGpadcGetTscX();

    if (x == HAL_ANA_GPADC_BAD_VALUE)
    {
        return FALSE;
    }

    *gpadc_x = x;

    return TRUE;
#else
    UINT16 x, y;

    x = hal_AnaGpadcGetTscX();
    y = hal_AnaGpadcGetTscY();

    if (x == HAL_ANA_GPADC_BAD_VALUE || y == HAL_ANA_GPADC_BAD_VALUE)
    {
        return FALSE;
    }

    *gpadc_x = x;
    *gpadc_y = y;

    return TRUE;
#endif
}

#ifdef SPI_REG_DEBUG
// ======================================================
// Debug: Read and Write Registers via SPI

#define PMD_SPI_REG_VALUE_LOCATION __attribute__((section(".pmu_reg_value")))

#define SPI_REG_WRITE_FLAG (1<<31)
#define SPI_REG_READ_FLAG (1<<30)
#define SPI_REG_READ_ALL_FLAG (1<<29)
#define SPI_REG_DATA_MASK (0xffff)

#define PMU_REG_NUM (0x4C)
UINT32 PMD_SPI_REG_VALUE_LOCATION g_pmdSpiRegValue[PMU_REG_NUM];

PROTECTED VOID pmd_SpiRegValueInit(VOID)
{
    for (int i=0; i<PMU_REG_NUM; i++)
    {
        g_pmdSpiRegValue[i] = 0;
    }
}

PUBLIC VOID pmd_SpiRegCheck(VOID)
{
    BOOL readAll = (g_pmdSpiRegValue[0] & SPI_REG_READ_ALL_FLAG)? TRUE : FALSE;

    g_pmdSpiRegValue[0] &= ~SPI_REG_READ_ALL_FLAG;
    for (int i=0; i<PMU_REG_NUM; i++)
    {
        if (g_pmdSpiRegValue[i] & SPI_REG_WRITE_FLAG)
        {
            pmd_RegWrite(i, g_pmdSpiRegValue[i]&SPI_REG_DATA_MASK);
            g_pmdSpiRegValue[i] &= ~SPI_REG_WRITE_FLAG;
        }
        if (readAll || (g_pmdSpiRegValue[i] & SPI_REG_READ_FLAG))
        {
            UINT16 data =pmd_RegRead(i);
            g_pmdSpiRegValue[i] &= ~(SPI_REG_READ_FLAG|SPI_REG_DATA_MASK);
            g_pmdSpiRegValue[i] |= data&SPI_REG_DATA_MASK;
        }
    }
}

PUBLIC VOID pmd_RegisterSpiRegCheckFunc(VOID)
{
    static BOOL registerIdleHook = FALSE;
    if (!registerIdleHook)
    {
        registerIdleHook = TRUE;
        pmd_SpiRegValueInit();
        sxs_RegisterDebugIdleHookFunc(&pmd_SpiRegCheck);
    }
}
#endif // SPI_REG_DEBUG

// =============================================================================
//  Earphone detection stuff
// =============================================================================

#ifdef ABB_HP_DETECT
PRIVATE VOID pmd_StartHpOutDetectInternal(VOID)
{
    UINT16 hpDetect;

    if (g_pmdHpOutDetectStarted)
    {
        // HP detection already started
        return;
    }

    // Read headphone IRQ status
    hpDetect = pmd_OpalSpiRead(RDA_ADDR_HP_DETECT_SETTING);

#if 0
    PMD_TRACE(PMD_INFO_TRC, 0,
              "[%08d] PMD-StartHpDetInter: hp(%d)=0x%04x",
              hal_TimGetUpTime()*1000/16384, g_pmdHpOutIrqEnabled, hpDetect);
#endif

    g_pmdHpOutDetectStarted = TRUE;

    if (hpDetect & RDA_PMU_HP_OUT)
    {
        pmd_RegHpDetectSettingSet(RDA_PMU_HP_OUT_CLEAR);
        // PMU needs two 32K clock cycles to clear the cause,
        // whereas IRQ mask change will take effect immediately.
        hal_TimDelay(2);
    }

    if (hpDetect & RDA_PMU_HP_DETECT)
    {
        // Headphone is plugged-in. Enable headphone out IRQ.
        g_pmdHpOutIrqEnabled = TRUE;
        pmd_RegHpDetectSettingClr(RDA_PMU_HP_OUT_MASK);
    }
    else
    {
        // Headphone might be plugged-out. Check the status again later.

        // Never stop PMD IRQ function timers, as they have different jobs.
        sxr_StartFunctionTimer(PMD_TIME_IRQ_DEBOUNCE, pmd_IrqHanderInternal,
                               PMD_IRQ_CHECK_HP, 0);
    }
}

PUBLIC VOID pmd_StartHpOutDetect(BOOL start)
{
#if 0
    PMD_TRACE(PMD_INFO_TRC, 0, "[%08d] PMD-StartHpDet: %d",
              hal_TimGetUpTime()*1000/16384, start);
#endif

    // Invoked at task level
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (start)
    {
        // Respond to HP-OUT INT IRQ after HP state is stable
        sxr_StartFunctionTimer(PMD_TIME_HP_DETECT_STABLE, pmd_StartHpOutDetectInternal, NULL, 0);
    }
    else
    {
        sxr_StopFunctionTimer(pmd_StartHpOutDetectInternal);

        if (g_pmdHpOutDetectStarted)
        {
            pmd_RegHpDetectSettingSet(RDA_PMU_HP_OUT_MASK|RDA_PMU_HP_OUT_CLEAR);
            g_pmdHpOutIrqEnabled = FALSE;
            g_pmdHpOutDetectStarted = FALSE;
        }
    }
    hal_SysExitCriticalSection(scStatus);
}
#endif // ABB_HP_DETECT

VOID pmd_EarDetectDelay(VOID)
{
    HAL_GPIO_CFG_T eraCfg;

    if (ear_handler != NULL)
    {
        if (hal_GpioGet(g_ear_gpio))
        {
#ifdef ABB_HP_DETECT
            pmd_StartHpOutDetect(FALSE);
#endif
            (*ear_handler)(PMD_EAR_STATUS_OUT);
            PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear Delay gpio = 1");
        }
        else
        {
            //restor spk gain
            if(g_pmdMuteSpk)
            {
                g_pmdMuteSpk = FALSE;
                hwp_configRegs->audio_spk_cfg |(CFG_REGS_AU_SPK_MUTE_N);
            }
            (*ear_handler)(PMD_EAR_STATUS_IN);
            PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear Delay gpio = 0");
        }
    }

    eraCfg.irqMask.rising = TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(g_ear_gpio);
}

VOID pmd_EarDetectHandler(VOID)
{
    PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear EarDetectHandler gpio");

    if (ear_handler != NULL)
    {
        (*ear_handler)(PMD_EAR_STATUS_UNKNOWN);
    }
    if (hal_GpioGet(g_ear_gpio))
    {
        if(hwp_configRegs->audio_spk_cfg &(CFG_REGS_AU_SPK_MUTE_N))
        {
            g_pmdMuteSpk = TRUE;
            hwp_configRegs->audio_spk_cfg &= ~(CFG_REGS_AU_SPK_MUTE_N);
        }
    }

    HAL_GPIO_CFG_T eraCfg;
    eraCfg.irqMask.rising = FALSE;
    eraCfg.irqMask.falling = FALSE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(g_ear_gpio);
    sxr_StopFunctionTimer(pmd_EarDetectDelay);
    sxr_StartFunctionTimer(HAL_TICK1S, pmd_EarDetectDelay, NULL, 0);
}

VOID pmd_EarpDetectGpioInit(HAL_GPIO_GPIO_ID_T gpioId)
{

    g_ear_gpio = gpioId;
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return;
    }

    HAL_GPIO_CFG_T eraCfg;
    eraCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    eraCfg.irqHandler = pmd_EarDetectHandler;
    eraCfg.irqMask.rising = TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioOpen(g_ear_gpio,&eraCfg);
}

#ifdef SUPPORT_QUICK_VCOM
extern CONST UINT8 g_tgtKeyMap[];
extern  VOID tgt_GetBootBbatKeyList(CONST UINT8 **ppList, UINT32 *pLen);
extern BOOL boot_MonitorKeyPressed(CONST UINT8 * monKeys, UINT32 listLen);
BOOL bQuickVcomFlag=FALSE;

UINT8 pmd_GetChargerAndBbatKeyStatus(VOID)
{
    UINT16 ret;
    CONST UINT8 *monKeys[1] = {g_tgtKeyMap[0]};

    ret = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);

    if ((ret & RDA_PMU_CHR_AC_ON) == 0)
    {
        return 0;
    }
    else
    {
        if (boot_MonitorKeyPressed(monKeys, 1))
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
}

VOID pmd_VcomGpioInit(HAL_GPIO_GPIO_ID_T gpioId)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    UINT8 temp=pmd_GetChargerAndBbatKeyStatus();
    bQuickVcomFlag=(temp==1)?TRUE:FALSE;

    if(gpioId > HAL_GPIO_NONE && gpioId < HAL_GPIO_31)
    {
        hal_GpioSetIn(gpioId);
    }
#endif
}

VOID pmd_VcomCheckGpioLevel(VOID)
{
    if(!bQuickVcomFlag)
    {
        HAL_GPIO_GPIO_ID_T gpioId=g_pmdConfig->vcomDetectGpio;
        if(gpioId > HAL_GPIO_NONE && gpioId < HAL_GPIO_31)
        {
            bQuickVcomFlag=(hal_GpioGet(gpioId) !=0)?TRUE:FALSE;
        }
    }
}

BOOL pmd_IsVcomGpioDetect(VOID)
{
    return bQuickVcomFlag;
}
#endif
PUBLIC VOID pmd_SetEarDetectHandler(PMD_EAR_HANDLER_T handler)
{
    ear_handler = handler;

    // Avoid reentrance of ear_handler coming from TIMER IRQ
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (ear_handler != NULL && pmd_GetEarModeStatus() == 1)
    {
        // Debouce the GPIO status
        pmd_EarDetectHandler();
    }
    hal_SysExitCriticalSection(scStatus);
}

UINT8 pmd_GetEarModeStatus(VOID)
{
#ifdef __MMI_R_WATCH__
	return 0;	// Not Plugin
#else
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return PMD_EAR_DETECT_POLLING;
    }
    else
    {
        return (hal_GpioGet(g_ear_gpio) == 0);
    }
#endif
}

PUBLIC UINT32 pmd_GetEarpieceGpadcChannel(VOID)
{
    return g_pmdConfig->earpieceGpadcChannel;
}

// =============================================================================
//  GPADC stuff
// =============================================================================

PUBLIC VOID pmd_GpadcSetInterval(UINT32 interval)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdGpadc2 =  PMU_SET_BITFIELD(g_pmdGpadc2, RDA_PMU_GPADC_START_TIME, interval);
    pmd_RDAWrite(RDA_ADDR_GPADC2, g_pmdGpadc2);

    hal_SysExitCriticalSection(scStatus);
    return;
}

PUBLIC VOID pmd_GpadcEnableChannel(UINT32 channel)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdGpadc2 =  PMU_SET_BITFIELD(g_pmdGpadc2, RDA_PMU_GPADC_ENABLE_CHANNEL, channel) |
                   RDA_PMU_GPADC_ENABLE|RDA_PMU_GPADC_CLK_DIV_ENABLE;
    pmd_RDAWrite(RDA_ADDR_GPADC2, g_pmdGpadc2);

    hal_SysExitCriticalSection(scStatus);
    return;
}

PUBLIC VOID pmd_GpadcDisable(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdGpadc2 = g_pmdGpadc2 & (~RDA_PMU_GPADC_ENABLE);
    pmd_RDAWrite(RDA_ADDR_GPADC2, g_pmdGpadc2);

    hal_SysExitCriticalSection(scStatus);
    return;
}

PUBLIC BOOL pmd_GpadcChannelEnabled(UINT32 channel)
{
    UINT16 reg;
#ifdef GPADC_CONFIG_READ_FROM_PMU
    // CAUTION: Cannot be invoked at interrupt level
    reg = pmd_RDARead(RDA_ADDR_GPADC2);
#else
    reg = g_pmdGpadc2;
#endif

    if( (reg&RDA_PMU_GPADC_ENABLE_CHANNEL_MASK) == RDA_PMU_GPADC_ENABLE_CHANNEL(channel) &&
            (reg&RDA_PMU_GPADC_ENABLE) )
    {
        return TRUE;
    }

    return FALSE;
}

PUBLIC VOID PMD_SetBuck2Voltage(BOOL bWakeup)
{
    UINT16  DcdcBuck2;

    DcdcBuck2 = g_pmdDcdcBuck; //pmd_RDARead(RDA_ADDR_DCDC_BUCK);

    if(bWakeup)
        DcdcBuck2 = PMU_SET_BITFIELD(DcdcBuck2,
                                     RDA_PMU_VBUCK2_BIT_NLP,
                                     8);
    else
        DcdcBuck2 = PMU_SET_BITFIELD(DcdcBuck2,
                                     RDA_PMU_VBUCK2_BIT_NLP,
                                     6);

    pmd_RDAWrite(RDA_ADDR_DCDC_BUCK, DcdcBuck2);

}

#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
#if 0  // gpio
static HAL_APO_ID_T gpioid = {{
        .type = HAL_GPIO_TYPE_IO,
        .id = 1
    }
};
#else
static HAL_APO_ID_T gpioid = {{
        .type = HAL_GPIO_TYPE_O,
        .id = 0
    }
};

#endif

UINT16 pmd_get_efuse_gpio(void)
{
    UINT32 temp;
    temp = (hwp_sysCtrl->Reset_Cause & SYS_CTRL_SW_BOOT_MODE(0x3f))>>SYS_CTRL_SW_BOOT_MODE_SHIFT;  //  27:22 reserved.
    if((temp >> 5) == 1)   // if 27bit equal 1 ,use the setting gpio
    {
        return temp&0x1f;
    }
    else
    {
        return 17;
    }

}

PRIVATE UINT16 pmd_GetEfuseGpadcValue(UINT8 page)
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
PUBLIC BOOL pmd_SetEfuseToDefault(VOID)
{
    UINT16 value1;
    value1 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_EFUSE_SEL_WORD_REG, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);
    if(value1 != pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1))
    {
        hal_HstSendEvent(0xbbbbffff);
        hal_HstSendEvent(0xbbbbfff6);
    }

    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0); // // [14:10] reserved [15] effuse of host
    if(0 != pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING3))
    {
        hal_HstSendEvent(0xbbbbffff);
        hal_HstSendEvent(0xbbbbfff7);
    }
    hal_TimDelay(100);
}


UINT16 efuse_gpadc1,efuse_gpadc2,efuse_gpadc1_done,efuse_gpadc2_done;
PUBLIC UINT16 pmd_GetEfuseGpadc(BOOL efuse_done)
{
    BOOL ret = TRUE;
    if(efuse_done == FALSE)
    {
        efuse_gpadc1 = pmd_GetEfuseGpadcValue(1);
        efuse_gpadc2 = pmd_GetEfuseGpadcValue(2);

        if(((efuse_gpadc1&0x3ff) > 640)||((efuse_gpadc1&0x3ff) < 600))
        {
            hal_HstSendEvent(0xbbbbffff);
            hal_HstSendEvent(0xbbbbfff1);
            hal_HstSendEvent(efuse_gpadc1);
            ret = FALSE;
        }

        if(((efuse_gpadc2&0x3ff) > 790)||((efuse_gpadc2&0x3ff) < 750))
        {
            hal_HstSendEvent(0xbbbbffff);
            hal_HstSendEvent(0xbbbbfff2);
            hal_HstSendEvent(efuse_gpadc2);
            ret = FALSE;
        }
        if(ret == FALSE)
        {
            while(1) {};
        }
        return ret;
    }
    else
    {
        efuse_gpadc1_done = pmd_GetEfuseGpadcValue(1);
        efuse_gpadc2_done = pmd_GetEfuseGpadcValue(2);
        if((efuse_gpadc1&0x3ff)!=(efuse_gpadc1_done&0x3ff))
        {
            hal_HstSendEvent(0xccccccc1);
            hal_HstSendEvent(efuse_gpadc1);
            hal_HstSendEvent(efuse_gpadc1_done);
            ret = FALSE;
        }
        if((efuse_gpadc2&0x3ff)!=(efuse_gpadc2_done&0x3ff))
        {
            hal_HstSendEvent(0xccccccc2);
            hal_HstSendEvent(efuse_gpadc2);
            hal_HstSendEvent(efuse_gpadc2_done);
            ret = FALSE;
        }
        return ret;
    }
}



PUBLIC UINT16 pmd_GetEfuseLockHost(void)
{
    UINT16 value1,value2;


    if(gpioid.type == HAL_GPIO_TYPE_IO)
        gpioid.id = pmd_get_efuse_gpio();

    if(gpioid.type == HAL_GPIO_TYPE_IO)
        hal_GpioSetOut(gpioid.gpioId);
    hal_GpioClr(gpioid);
    value1 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_EFUSE_SEL_WORD_REG, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_SW_PMOS_CTRL_CP, 3);//
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);
    hal_TimDelay(200);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_SW_PMOS_CTRL_CP, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);

    value2 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OUT);
    hal_HstSendEvent(0xbbbbbbc2); hal_HstSendEvent(value2); hal_HstSendEvent(gpioid.id );
    return value2;

}

#define _BURN_EFUSE_BIT_   1<<15     // // [14:10] reserved [15] effuse of host
UINT32 timedelay = 0;
PUBLIC BOOL pmd_SetEfuseLockHost_again(UINT32 delay)
{
    UINT16 value1;
    UINT16 valuer;

    if(gpioid.type == HAL_GPIO_TYPE_IO)
        gpioid.id = pmd_get_efuse_gpio();

    if(gpioid.type == HAL_GPIO_TYPE_IO)
        hal_GpioSetOut(gpioid.gpioId);
    hal_GpioClr(gpioid);
    valuer =  pmd_GetEfuseLockHost();
    value1 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_EFUSE_SEL_WORD_REG, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);


    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, _BURN_EFUSE_BIT_); // // [14:10] reserved [15] effuse of host
    if(_BURN_EFUSE_BIT_ != pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING3))
    {
        hal_HstSendEvent(0xbbbbffff);
        hal_HstSendEvent(0xbbbbfff8);
    }
    hal_TimDelay(100);
    hal_GpioSet(gpioid);
    if(delay != 0)
        hal_TimDelay(delay);  // just try again

    hal_GpioClr(gpioid);
    value1 = pmd_GetEfuseLockHost();
    if(value1 == (valuer|0x8000))
    {
        if(pmd_GetEfuseGpadc(TRUE) == TRUE)
        {
            pmd_SetEfuseToDefault();
            hal_HstSendEvent(0xbbbb0000);
        }
        else
        {
            hal_HstSendEvent(0xbbbbffff);
            hal_HstSendEvent(0xbbbbfff0);
        }

        while(1) {};
    }
    else
    {
        return FALSE;

    }

}

PUBLIC VOID pmd_SetEfuseLockHost(void)
{
    UINT16 value1;
    UINT16 valuer;
    if(gpioid.type == HAL_GPIO_TYPE_IO)
        gpioid.id = pmd_get_efuse_gpio();
    if(gpioid.type == HAL_GPIO_TYPE_IO)
        hal_GpioSetOut(gpioid.gpioId);

    hal_GpioClr(gpioid);
    pmd_SetPadVoltage(0);
    pmd_GetEfuseGpadc(FALSE);
    valuer =  pmd_GetEfuseLockHost();
    value1 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_EFUSE_SEL_WORD_REG, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);


    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, _BURN_EFUSE_BIT_);
    if(_BURN_EFUSE_BIT_ != pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING3))
    {
        hal_HstSendEvent(0xbbbbffff);
        hal_HstSendEvent(0xbbbbfff9);
    }
    hal_TimDelay(200);
    hal_GpioSet(gpioid);
    //hal_TimDelay(400);  // 2us is enough
    hal_GpioClr(gpioid);
    value1 = pmd_GetEfuseLockHost();
    if(value1 == (valuer|0x8000))
    {
        if(pmd_GetEfuseGpadc(TRUE) == TRUE)
        {

            pmd_SetEfuseToDefault();
            hal_HstSendEvent(0xbbbb0000);
        }
        else
        {
            hal_HstSendEvent(0xbbbbffff);
            hal_HstSendEvent(0xbbbbfff0);
        }
        while(1) {};
    }
    else
    {

        timedelay = 0;
        for(UINT8 i=0; i<10; i++)
        {
            if(i>=4)
            {
                timedelay += 100;
                pmd_SetEfuseLockHost_again(timedelay);
            }
            else
            {
                pmd_SetEfuseLockHost_again(0);
            }
        }
        pmd_SetEfuseToDefault();
        hal_HstSendEvent(0xbbbbffff);
        while(1) {};

    }

}

#endif

// =============================================================================
//  Aux clock stuff
// =============================================================================

PUBLIC VOID pmd_SetAuxClockStatusHandler(VOID (*handler)(BOOL))
{
    g_pmdAuxClkStatusHandler = handler;
}

PROTECTED VOID pmd_UsbDownloadByMicDetect(VOID)
{
#ifndef RAMRUN
    UINT32 lo, hi;
    if (hal_SysGetResetCause() == HAL_SYS_RESET_CAUSE_NORMAL && tgt_GetUsbDownloadByMicDet(&lo, &hi))
    {
        UINT16 chargerStatus = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
        if ((chargerStatus & RDA_PMU_CHR_AC_ON) != 0)
        {
            if (!g_pmdConfig->power[PMD_POWER_EARPIECE].powerOnState)
            {
                pmd_EnablePower(PMD_POWER_EARPIECE, TRUE);
                hal_TimDelay(5 MS_WAITING);
            }
            hal_AnaGpadcOpen(g_pmdConfig->earpieceGpadcChannel, HAL_ANA_GPADC_ATP_100MS);
            HAL_ANA_GPADC_MV_T mv = hal_AnaGpadcGet(pmd_GetEarpieceGpadcChannel());
            mon_Event(0x0DE00000 | mv);
            if (mv != HAL_ANA_GPADC_BAD_VALUE && mv >= lo && mv <= hi)
            {
                UINT32 __attribute__((unused)) status = hal_SysEnterCriticalSection();
                pmd_EnablePower(PMD_POWER_USB, TRUE);
                boot_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR);
                hal_DcacheFlushAll(TRUE);
                hal_SysSoftReset();
            }
            hal_AnaGpadcClose(g_pmdConfig->earpieceGpadcChannel);
            if (!g_pmdConfig->power[PMD_POWER_EARPIECE].powerOnState)
                pmd_EnablePower(PMD_POWER_EARPIECE, FALSE);
        }
    }
#endif
    BOOL usbPowerOn = g_pmdConfig->power[PMD_POWER_USB].powerOnState;
#ifdef FASTPF_USE_USB
    usbPowerOn = TRUE;  // For USB flash programmer
#endif
    pmd_EnablePower(PMD_POWER_USB, usbPowerOn);
}

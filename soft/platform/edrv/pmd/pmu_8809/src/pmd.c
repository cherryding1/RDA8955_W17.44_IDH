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
#ifdef RF_LDO_MODE
#include "rfd_xcv.h"
#endif

// Opal hardware
#include "opal.h"

// hal drivers
#include "hal_ana_gpadc.h"
#include "hal_pwm.h"
#include "hal_ispi.h"
#include "hal_lps.h"
#include "hal_timers.h"
#include "hal_sys.h"

#ifdef RF_LDO_MODE
#include "hal_rfspi.h"
#endif
#include "hal_sim.h"
#include "hal_rda_abb.h"
#include "hal_rda_audio.h"
#include "global_macros.h"
#include "cfg_regs.h"
// sx
#include "sxr_tls.h"
#include "sxr_tim.h"
#include "sxs_idle.h"
#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
#include "sys_ctrl.h"
#endif
#include "boot_map.h"

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

#ifdef FM_LDO_WORKAROUND
extern VOID rfd_XcvSetDcdcLdoMode(BOOL dcdcOn, BOOL ldoOn);
#endif

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

// Whether to disallow the speaker lines to be in class K mode
// If the speaker lines are in class AB mode, this macro
// is meaningless; otherwise, the lines are in class D mode
// if 1, or in class K mode if 0.
#ifndef AUD_SPK_LINE_NO_CLASS_K
#define AUD_SPK_LINE_NO_CLASS_K 0
#endif

// Check the validity of L-R-double configuration
// (The left and right speaker lines are shorted out to increase
// drive capability, meaningful for class D/K only)
//
// AUD_SPK_LINE_L_R_DOUBLE: Whether L&R lines are shorted out
// CLASS_D_K_SPK_L_R_DOUBLE: Whether class D/K L-R-double mode is enabled
//
#ifndef AUD_SPK_LINE_L_R_DOUBLE
#if (AUD_SPK_LINE_L_CLASS_AB) || (AUD_SPK_LINE_R_CLASS_AB)
#define AUD_SPK_LINE_L_R_DOUBLE 0
#else // L&R are both class D or class K
#define AUD_SPK_LINE_L_R_DOUBLE 1
#endif // L&R are both class D or class K
#endif

#if (AUD_SPK_LINE_L_R_DOUBLE) && (AUD_SPK_LINE_L_CLASS_AB) && \
    defined(RECV_USE_SPK_LINE) && defined(AUD_SPK_ON_WITH_LCD)
#error "Bad config: When L&R are shorted out for class AB speaker, \
RECV_USE_SPK_LINE and AUD_SPK_ON_WITH_LCD cannot be enabled simultaneously. \
Otherwise, class AB speaker L&R lines would be enabled at the same time"
#endif

#if (AUD_SPK_LINE_L_CLASS_AB != AUD_SPK_LINE_R_CLASS_AB)
#if (AUD_SPK_LINE_L_R_DOUBLE)
#error "L&R cannot be shorted out if their modes are different"
#endif
#define CLASS_D_K_SPK_L_R_DOUBLE 0
#else // L&R modes are the same
#if (AUD_SPK_LINE_L_CLASS_AB)
#define CLASS_D_K_SPK_L_R_DOUBLE 0
#else // class D/K
// In class D/K L&R double mode, L is the master line, and there will be
// no output if L is disabled
#if defined(RECV_USE_SPK_LINE)
#define CLASS_D_K_SPK_L_R_DOUBLE AUD_SPK_LINE_L_R_DOUBLE
#else
#define CLASS_D_K_SPK_L_R_DOUBLE AUD_SPK_LINE_L_R_DOUBLE
#endif
#endif // class D/K
#endif // L&R modes are the same

#if (AUD_SPK_LINE_L_ONLY && AUD_SPK_LINE_R_ONLY)
#error "Bad config: Speaker L only mode defined with R only mode"
#endif

#if (AUD_SPK_LINE_L_R_DOUBLE) && (AUD_SPK_LINE_L_ONLY || AUD_SPK_LINE_R_ONLY)
#error "Bad config: L&R double mode defined with L (or R) only mode"
#endif

#if defined(RECV_USE_SPK_LINE) && (AUD_SPK_LINE_L_ONLY || AUD_SPK_LINE_R_ONLY)
#error "Bad config: RECV_USE_SPK_LINE defined with L (or R) only mode"
#endif

// Whether to enable charge pump for LCD backlight
#ifndef LCD_BACKLIGHT_CHARGE_PUMP
#define LCD_BACKLIGHT_CHARGE_PUMP 1
#endif

#if (LCD_BACKLIGHT_CHARGE_PUMP) && \
    (AUD_SPK_LINE_L_CLASS_AB || AUD_SPK_LINE_R_CLASS_AB || AUD_SPK_LINE_NO_CLASS_K)
#error "LCD backlight charge pump requires speaker to be in class K mode"
#endif

// Whether to enable charge pump high output for speaker
#define AUD_SPK_CHARGE_PUMP_HIGH_OUTPUT 1

#define PMD_TIME_IRQ_ACT_CS_RETRY (2 MS_WAITING)
#define PMD_TIME_IRQ_DEBOUNCE (100 MS_WAITING)

#define PMD_IRQ_DEBOUNCE_REQ (1<<0)
#define PMD_IRQ_CHECK_CHARGER (1<<1)
#define PMD_IRQ_CHECK_HP (1<<2)

#define POWER_LDO_USER(id) (1<<id)
#define LEVEL_LDO_USER(id) (1<<(id+PMD_POWER_ID_QTY))
#define DEFAULT_LDO_USER   (1<<31)

#define PMD_TIME_HP_DETECT_STABLE (1200 MS_WAITING)

#ifdef CHIP_DIE_8809E2
#define PMD_CHARGE_PUMP_CP_LEVEL  8
#else
#define PMD_CHARGE_PUMP_CP_LEVEL  15
#endif

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
    struct {
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
    struct {
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
    struct {
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
    struct {
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
#ifndef FM_RADIO_WITH_EXTERNAL_ANT
PROTECTED BOOL g_pmdMuteSpk = FALSE;
#endif

//  RDA_ADDR_LDO_SETTINGS = 0x02
PRIVATE PMD_LDO_SETTINGS_T g_pmdLdoSettings =
{{
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
    }};

//  RDA_ADDR_LDO_ACTIVE_SETTING1 = 0x03
//  RDA_ADDR_LDO_LP_SETTING1 = 0x08
PRIVATE PMD_LDO_PROFILE_SETTING1_T g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_QTY] =
    {
{{  // PMD_PROFILE_MODE_NORMAL
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
                     .buck1LdoOff = TRUE,
                      .buck1PfmOn = FALSE,
                       .buck1On = TRUE,
    }},
{{  // PMD_PROFILE_MODE_LOWPOWER
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
                     .buck1LdoOff = TRUE,
                      .buck1PfmOn = TRUE,
                       .buck1On = TRUE,
    }},
    };


//  RDA_ADDR_LDO_ACTIVE_SETTING2 = 0x04
//  RDA_ADDR_LDO_LP_SETTING2 = 0x09
PRIVATE PMD_LDO_PROFILE_SETTING2_T g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_QTY] =
    {
{{  // PMD_PROFILE_MODE_NORMAL
        .rgbLedOff = FALSE,
         .buck2LdoOff = TRUE,
          .buck2PfmOn = FALSE,
           .buck2On = TRUE,
            .vIntRfOff = FALSE,
             .vIbrIs1_8 = FALSE,
              .vMmcIs1_8 = TRUE,
               .vLcdIs1_8 = TRUE,
                .vCamIs1_8 = TRUE,
                 .vRfIs1_8 = FALSE,
                  .vPadIs1_8 = FALSE,
                   .vRtcVbit = 4,
    }},
{{  // PMD_PROFILE_MODE_LOWPOWER
        .rgbLedOff = TRUE,
         .buck2LdoOff = TRUE,
          .buck2PfmOn = TRUE,
           .buck2On = FALSE,
            .vIntRfOff = TRUE,
             .vIbrIs1_8 = FALSE,
              .vMmcIs1_8 = TRUE,
               .vLcdIs1_8 = TRUE,
                .vCamIs1_8 = TRUE,
                 .vRfIs1_8 = FALSE,
                  .vPadIs1_8 = FALSE,
                   .vRtcVbit = 0,
    }},
    };

//  RDA_ADDR_LDO_ACTIVE_SETTING3 = 0x05
//  RDA_ADDR_LDO_LP_SETTING3 = 0x0a
PRIVATE UINT16 g_pmdLdoProfileSetting3 =
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
{{
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
    }};

// VCORE values
PRIVATE UINT8 g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_QTY];
PRIVATE UINT8 g_pmdDcdcVcoreValueLp;
// CCORE valuse of LDO mode
PRIVATE UINT8 g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_QTY];

PRIVATE UINT8 g_pmdDcdcVcoreActRegistry[PMD_CORE_VOLTAGE_USER_ID_QTY];
PRIVATE UINT8 g_pmdDcdcVcoreActCurIndex;

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

#ifdef RF_LDO_MODE
// RDA_ADDR_LDO_BUCK1 = 0x2d
PRIVATE CONST UINT16 HAL_RODATA_INTERNAL gc_pmdLdoBuck1Act =
    RDA_PMU_VBUCK_LDO_BIT(0)|
    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
    RDA_PMU_PFM_THRESHOLD(2)|
    RDA_PMU_OSC_FREQ(2)|
    RDA_PMU_LOW_SENSE|
    RDA_PMU_DISCHARGE_EN|
    RDA_PMU_HEAVY_LOAD;
#else
// RDA_ADDR_LDO_BUCK1 = 0x2d
PRIVATE CONST UINT16 HAL_RODATA_INTERNAL gc_pmdLdoBuck1Act =
    RDA_PMU_VBUCK_LDO_BIT(4)|
    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
    RDA_PMU_PFM_THRESHOLD(2)|
    RDA_PMU_OSC_FREQ(2)|
    RDA_PMU_LOW_SENSE|
    RDA_PMU_DISCHARGE_EN|
    RDA_PMU_HEAVY_LOAD;
#endif

PRIVATE CONST UINT16 HAL_RODATA_INTERNAL gc_pmdLdoBuck1Lp =
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

#ifdef CHIP_DIE_8809E2
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdDcdcBuck =
    RDA_PMU_VBUCK1_BIT_NLP(8)|
    RDA_PMU_VBUCK1_BIT_LP(8)|
    RDA_PMU_VBUCK2_BIT_NLP(7)|
    RDA_PMU_VBUCK2_BIT_LP(7);


#else
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdDcdcBuck =
    RDA_PMU_VBUCK1_BIT_NLP(8)|
    RDA_PMU_VBUCK1_BIT_LP(8)|
    RDA_PMU_VBUCK2_BIT_NLP(10)|
    RDA_PMU_VBUCK2_BIT_LP(10);

#endif

// RDA_ADDR_THERMAL_CALIBRATION = 0x36
PRIVATE UINT16 g_pmdThermalCalib =
    RDA_PMU_HV_MODE_BL|
#if (CLASS_D_K_SPK_L_R_DOUBLE)
    RDA_PMU_DOUBLE_MODE_EN_CLG|
#endif
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
    RDA_PMU_I_BIT_CLG_REG(2);

// RDA_ADDR_SPEAKER_PA_SETTING3 = 0x41
// Speaker modes will be initialized in pmd_SpeakerPaRestoreMode
PRIVATE UINT16 g_pmdSpeakerPaSetting3 =
    RDA_PMU_SETUP_TMCTRL_BIT(0)|
    RDA_PMU_RESET_CLG_DELAY_NEG(1)|
    RDA_PMU_RESET_CLG_DELAY_POS(1);

// Pad level
PRIVATE CONST UINT8 HAL_RODATA_INTERNAL gc_pmdPadLevelAct = 7;
PRIVATE CONST UINT8 HAL_RODATA_INTERNAL gc_pmdPadLevelLp = 0;

/// Configuration structure for TARGET
PROTECTED CONST PMD_CONFIG_T* g_pmdConfig;

/// To avoid SPI deactivation/activation during Open
PRIVATE BOOL g_pmdInitDone = FALSE;

/// To track activation status
PRIVATE VOLATILE UINT8 g_pmdActivated = 0;
PRIVATE VOLATILE UINT8 g_tsdActivated = 0;
PRIVATE VOLATILE UINT8 g_SimActivated = 0;

// Workaround to disable PMU IRQ without blocking in PMU IRQ handler
PRIVATE UINT32 g_pmdIrqSettingLatest = 0;

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

#ifdef RF_LDO_MODE
PRIVATE BOOL g_pmdLdoReqFlag = FALSE;

PRIVATE BOOL g_pmdHighVcoreReqFlag = FALSE;

PRIVATE BOOL g_pmdLdoModeEnabled = FALSE;
#else
#ifdef FM_LDO_WORKAROUND
PRIVATE BOOL g_pmdLdoReqFlag = FALSE;

PRIVATE BOOL g_pmdHighVcoreReqFlag = FALSE;

PRIVATE BOOL g_pmdLdoModeEnabled = FALSE;
#endif
#endif

PRIVATE VOID (*g_pmdAuxClkStatusHandler)(BOOL) = NULL;


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


// ============================================================================
// FUNCTIONS
// ============================================================================

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
PUBLIC VOID pmd_RegIrqSettingSet(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdIrqSettingLatest |= flags;
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);
    // Never save the clear bits
    g_pmdIrqSettingLatest &= ~(RDA_PMU_INT_CHR_CLEAR|RDA_PMU_PENIRQ_CLEAR|
                               RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_KEYIRQ_CLEAR);

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

#if (LCD_BACKLIGHT_CHARGE_PUMP == 0)
#ifdef CHIP_DIE_8809E
#else

    if (on)
    {
        // Prefer class AB mode for speaker
        pmd_SpeakerPaSetMode(PMD_SPEAKER_PA_MODE_CLASS_AB,
                             PMD_SPEAKER_PA_MODE_CLASS_AB);
    }
    else
    {
        // Restore the original speaker mode
        pmd_SpeakerPaRestoreMode();
    }
#endif

#endif

#ifdef FM_LDO_WORKAROUND
    if (g_pmdLdoModeEnabled)
    {
        if (on)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = FALSE;
        }
        else
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = TRUE;
        }
    }
    else
#endif
    {
        if (on)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = TRUE;
        }
        else
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = FALSE;
        }
    }
    pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
    hal_SysExitCriticalSection(scStatus);
}

#ifdef RF_LDO_MODE
VOID pmd_DcdcPower_Control(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdLdoModeEnabled = !on;
    if (on) // DCDC mode
    {
        // Active mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = TRUE;//enable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2On = TRUE;//enable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        //hal_TimDelay(10);

        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = TRUE;//disable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2LdoOff = TRUE;//disable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        // LP mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = TRUE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = TRUE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

        if (g_pmdFmInLpModeEnabled)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = TRUE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = TRUE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
        }
    }
    else // LDO mode
    {
        // Active mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;//enable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2LdoOff = FALSE;//enable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        //hal_TimDelay(10);

        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = FALSE;//disable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2On = FALSE;//disable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        // LP mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = FALSE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

        if (g_pmdFmInLpModeEnabled)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = FALSE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = FALSE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
        }
    }

    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
//  pmd_EnableDcdcPower
// ============================================================================
PUBLIC BOOL pmd_EnableDcdcPower(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdLdoReqFlag = !on;

    if (!on)
    {
        rfd_XcvSetDcdcLdoMode(TRUE, TRUE);
    }
#ifdef FM_8809E2_LOD_MODE
#else
    //if(!g_pmdHighVcoreReqFlag)
#endif
    {
        pmd_DcdcPower_Control(on);
    }

    if (on)
    {
        rfd_XcvSetDcdcLdoMode(TRUE, FALSE);
    }

    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}
#else
#ifdef FM_LDO_WORKAROUND
// ============================================================================
//  pmd_DcdcPower_Control
// ============================================================================
PRIVATE VOID pmd_DcdcPower_Control(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdLdoModeEnabled = !on;
    if (on) // DCDC mode
    {
        // Active mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = TRUE;//enable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2On = TRUE;//enable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        //hal_TimDelay(10);

        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = TRUE;//disable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2LdoOff = TRUE;//disable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        // LP mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = TRUE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = TRUE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

        if (g_pmdFmInLpModeEnabled)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = TRUE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = TRUE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
        }
    }
    else // LDO mode
    {
        // Active mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;//enable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2LdoOff = FALSE;//enable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        //hal_TimDelay(10);

        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = FALSE;//disable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2On = FALSE;//disable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        // LP mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = FALSE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

        if (g_pmdFmInLpModeEnabled)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = FALSE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = FALSE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
        }
    }

    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
//  pmd_EnableDcdcPower
// ============================================================================
PUBLIC BOOL pmd_EnableDcdcPower(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdLdoReqFlag = !on;

    if (!on)
    {
        rfd_XcvSetDcdcLdoMode(TRUE, TRUE);
    }
#ifdef FM_8809E2_LOD_MODE
#else
    if(!g_pmdHighVcoreReqFlag)
#endif
    {
        pmd_DcdcPower_Control(on);
    }

    if (on)
    {
        rfd_XcvSetDcdcLdoMode(TRUE, FALSE);
    }

    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}
#endif // FM_LDO_WORKAROUND
#endif

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

#ifdef CHIP_DIE_8809E2
extern VOID memd_set_flash_sysclk_to_stable(BOOL stable_sysclk,HAL_SYS_FREQ_T sysclk,UINT8 flshclk);
#endif
#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
PUBLIC VOID pmd_SetEfuseLockHost(void);
#endif
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

    // DCDC VCORE value->voltage:
    // 0->0.74, 1->0.81, 2->0.88, 3->0.95, 4->1.03, 5->1.09,
    // 6->1.16, 7->1.25, 8->1.33, 9->1.43, 10->1.49, 11->1.56,
    // 12->1.65, 13->1.69, 14->1.75, 15->1.80

    // Check chip version
    UINT32 metalId = hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID);

    //312M PLL need >= 8;
    //the active mode min voltage(PMD_CORE_VOLTAGE_LOW) could be  4
    //the low power mode min voltage could be 0

#ifdef CHIP_DIE_8809E2
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_BTTEST] = 15;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = 9;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = 8;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = 7;
    //LDO
    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = 3;
    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = 4;
    g_pmdLDO_VcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = 6;

#else
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = 12;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = 11;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = 10;

#endif


#ifdef CHIP_DIE_8809E2
    g_pmdDcdcVcoreValueLp = 3;
#else
    // BT requires VBUCK1_NLP/LP=8,VBUCK2_NLP/LP=9 on U02 chips.
    if (metalId < HAL_SYS_CHIP_METAL_ID_U04)
    {
        g_pmdDcdcVcoreValueLp = 8;
    }
    else
    {
        g_pmdDcdcVcoreValueLp = 3;
    }
#endif
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

#ifdef CHIP_DIE_8809E2
    g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vPadIs1_8 = FALSE;
#else
    // Init volatge selection
    if (metalId < HAL_SYS_CHIP_METAL_ID_U04)
    {
        // vPad should be lowered to 1.8V when in LP mode
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vPadIs1_8 = TRUE;
    }
#endif
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

    // PMU register reset does NOT seem reliable at present, and it conflicts
    // with bandgap calib disabling
    if(hal_SysGetResetCause() == HAL_SYS_RESET_CAUSE_NORMAL)
    {
        // Reset PMU registers
        pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_SOFT_RESETN);
        pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_REGISTER_RESETN|RDA_PMU_SOFT_RESETN);
    }

    UINT32 id;
    // power
    for (id = 0; id < PMD_POWER_ID_QTY; id++)
    {
        g_pmdMapAccess.powerInfo[id] = 0;
        // initial state as described by powerOnState from config
        BOOL powerOn = g_pmdConfig->power[id].powerOnState;

        if (id != PMD_POWER_USB)
            pmd_EnablePower(id, powerOn);
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
#ifdef CHIP_DIE_8809E2
#ifdef SET_SYSCLK_TO_STABLE_VALUE
    // memd_set_flash_sysclk_to_stable(TRUE,HAL_SYS_FREQ_208M,9);
#else
    // memd_set_flash_sysclk_to_stable(FALSE,HAL_SYS_FREQ_208M,9);
#endif
#endif

    // Earpiece detection
    pmd_EarpDetectGpioInit(g_pmdConfig->earpieceDetectGpio);
#ifdef SUPPORT_QUICK_VCOM
    pmd_VcomGpioInit(g_pmdConfig->vcomDetectGpio);
#endif

#ifdef _BURN_EFFUSE_TO_DISABLE_HOST_RX_
    pmd_SetEfuseLockHost();
#endif

}


// =============================================================================
// pmd_Close
// -----------------------------------------------------------------------------
/// Closes the PMD driver.
// =============================================================================
PUBLIC VOID pmd_Close(VOID)
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
    // 1) Disable EOM, PEN, Charger IRQs;
    // 2) Enter the power down mode (power controlled by PMU default profile).
    g_pmdIrqSettingLatest = 0;
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_MASK|RDA_PMU_PENIRQ_MASK|
                         RDA_PMU_KEYIRQ_MASK|RDA_PMU_INT_CHR_MASK|
                         RDA_PMU_HOLD_PRECHARGER_EFUSE);

    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
// pmd_SetPowerMode
// ----------------------------------------------------------------------------
/// Set Power Mode to the desired mode
// ============================================================================
PUBLIC VOID PMD_FUNC_INTERNAL pmd_SetPowerMode(PMD_POWERMODE_T powerMode)
{
    // this function is in internal ram because it is called in critical conditions (ECO Mode) so NO TRACE IN HERE !
    // PMD_TRACE(PMD_INFO_TRC, 0, __func__);

    // This function is now called far before the critical point of entering
    // the deepest low power mode and is responsible only for preparing
    // other stuff (like controlling V_ANA as bellow)

    UINT8 padLevel = 0;
    UINT16 miscCtrl = 0;
    UINT16 ldoBuck1 = 0;
    UINT32 scStatus = hal_SysEnterCriticalSection();

#ifdef __PRJ_WITH_SPIFLSH__
    if (powerMode == PMD_IDLEPOWER)
    {
        memd_FlashWakeup();
    }
#endif

    switch (powerMode)
    {
        case PMD_LOWPOWER:
            padLevel = gc_pmdPadLevelLp;
            miscCtrl = g_pmdMiscControlLp;
            ldoBuck1 = gc_pmdLdoBuck1Lp;
            // Disable charge LDO to save power
            g_pmdCalibSetting2 |= RDA_PMU_PD_CHARGE_LDO;
            break;
        case PMD_IDLEPOWER:
            padLevel = gc_pmdPadLevelAct;
            miscCtrl = g_pmdMiscControlActive;
            ldoBuck1 = gc_pmdLdoBuck1Act;
            // Enable charge LDO to reduce vsim fluctuation in low battery
            g_pmdCalibSetting2 &= ~RDA_PMU_PD_CHARGE_LDO;
            // we are out of EcoMode, so we can do some checks using external
            // rams (data or functions)
            break;
        default:
            break;
    }
    pmd_RDAWrite(RDA_ADDR_MISC_CONTROL, miscCtrl);
    pmd_RDAWrite(RDA_ADDR_CALIBRATION_SETTING2, g_pmdCalibSetting2);
    pmd_SetPadVoltage(padLevel);
    pmd_RDAWrite(RDA_ADDR_LDO_BUCK1, ldoBuck1);

#ifdef __PRJ_WITH_SPIFLSH__
    if (powerMode == PMD_LOWPOWER)
    {
        memd_FlashSleep();
    }
#endif

    hal_SysExitCriticalSection(scStatus);
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
#ifdef RDAFM_8809
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
#endif // RDAFM_8809
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
    {   // Pin
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
            {   // Opal LDO
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
            {   // Pin
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
                    else // PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
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
                    else // PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
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
                    else// PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
                    {
                        if (id == PMD_LEVEL_LED3)
                        {
                            mask = RDA_PMU_LED_R_OFF_ACT|RDA_PMU_LED_R_OFF_LP;
                            bgUser = PMD_BG_LP_MODE_USER_LED3;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_LED3;
                        }
                        else
                        {
                            mask= RDA_PMU_LED_G_OFF_ACT|RDA_PMU_LED_G_OFF_LP|
                                  RDA_PMU_LED_B_OFF_ACT|RDA_PMU_LED_B_OFF_LP;
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

            //value2 = pmd_RDARead(RDA_ADDR_THERMAL_CALIBRATION);
            if (level <= 3)
            {
                g_pmdThermalCalib |= 1<<1;  //  set the current to 1/10 of the target vlaue.
            }
            else
            {
                g_pmdThermalCalib &= ~(1<<1);
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
            pmd_RDAWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
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


// ============================================================================
// pmd_InitCoreVoltage
// ----------------------------------------------------------------------------
/// Init the core voltage level in active/idle mode
// ============================================================================
PUBLIC VOID pmd_InitCoreVoltage(VOID)
{
    // Init DCDC frequency
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK1, gc_pmdLdoBuck1Act);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK2, gc_pmdLdoBuck2);

    // Set DCDC VCORE
    UINT32 vcore;
    vcore = g_pmdDcdcVcoreValueLp;
    g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                     RDA_PMU_VBUCK1_BIT_LP,
                                     vcore);
    g_pmdDcdcVcoreActCurIndex = PMD_CORE_VOLTAGE_LOW;
    vcore = g_pmdDcdcVcoreValueActiveAry[g_pmdDcdcVcoreActCurIndex];
    g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                     RDA_PMU_VBUCK1_BIT_NLP,
                                     vcore);
    pmd_OpalSpiWrite(RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);
}

// ============================================================================
// pmd_GetCoreVoltageRequest
// ----------------------------------------------------------------------------
/// Get core voltage level request by user id.
// ============================================================================
PUBLIC PMD_CORE_VOLTAGE_T pmd_GetCoreVoltageRequest(PMD_CORE_VOLTAGE_USER_ID_T user)
{
    return g_pmdDcdcVcoreActRegistry[user];
}

// ============================================================================
// pmd_SetCoreVoltage
// ----------------------------------------------------------------------------
/// Set the core voltage level in active mode
// ============================================================================
PUBLIC VOID pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_ID_T user,
                               PMD_CORE_VOLTAGE_T coreVolt)
{
#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
    // It has risks to dynamically change DCDC VCORE, as
    // DCDC VCORE voltage stable time is about 100 us in a voltage change.
    if (g_pmdInitDone) return;
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

    g_pmdDcdcVcoreActRegistry[user] = coreVolt;

    UINT32 scStatus = hal_SysEnterCriticalSection();
    UINT16 gc_pmdLdoBuck1Acttemp = gc_pmdLdoBuck1Act;
    UINT32 vcoreIndex = 0;
    for (UINT32 i=0; i<PMD_CORE_VOLTAGE_USER_ID_QTY; i++)
    {
        if (vcoreIndex < g_pmdDcdcVcoreActRegistry[i])
        {
            vcoreIndex = g_pmdDcdcVcoreActRegistry[i];
        }
    }

    if (g_pmdDcdcVcoreActCurIndex != vcoreIndex)
    {
#ifdef FM_LDO_WORKAROUND
        if (g_pmdDcdcVcoreActCurIndex < PMD_CORE_VOLTAGE_HIGH &&
                vcoreIndex == PMD_CORE_VOLTAGE_HIGH)
        {
            g_pmdHighVcoreReqFlag = TRUE;
            if(g_pmdLdoReqFlag)
            {

#ifdef FM_8809E2_LOD_MODE
#else
                pmd_DcdcPower_Control(TRUE);
#endif
            }
        }
#endif

        // Set DCDC VCORE
        UINT32 vcore = g_pmdDcdcVcoreValueActiveAry[vcoreIndex];

        g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                         RDA_PMU_VBUCK1_BIT_NLP,
                                         vcore);
        pmd_RDAWrite(RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);
#ifdef FM_8809E2_LOD_MODE

        // Set DCDC VCORE IN LOD MODE
        UINT32 vcoreldo = g_pmdLDO_VcoreValueActiveAry[vcoreIndex];

        gc_pmdLdoBuck1Acttemp = PMU_SET_BITFIELD(gc_pmdLdoBuck1Acttemp,
                                RDA_PMU_VBUCK_LDO_BIT,
                                vcoreldo);
        pmd_RDAWrite(RDA_ADDR_LDO_BUCK1, gc_pmdLdoBuck1Acttemp);
#endif
#ifdef FM_LDO_WORKAROUND
        if (g_pmdDcdcVcoreActCurIndex == PMD_CORE_VOLTAGE_HIGH &&
                vcoreIndex < PMD_CORE_VOLTAGE_HIGH)
        {
            g_pmdHighVcoreReqFlag = FALSE;
            if(g_pmdLdoReqFlag)
            {

#ifdef FM_8809E2_LOD_MODE
#else
                pmd_DcdcPower_Control(FALSE);
#endif
            }
        }
#endif

        g_pmdDcdcVcoreActCurIndex = vcoreIndex;
    }

    hal_SysExitCriticalSection(scStatus);

#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1)
    //consider 8809e2 low vcore active values, wait DCDC VCORE voltage stable here.
#ifdef CHIP_DIE_8809E2
    // Wait until the voltage becomes stable
    if (g_pmdInitDone)
    {
        // DCDC VCORE voltage stable time is about 100 us in a voltage change
        // Freq might be changed in IRQ context, e.g., PAL
        hal_TimDelay(2);
    }
#endif
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1
}

PUBLIC VOID pmd_SetMemVoltage(UINT8 level)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoProfileSetting3 =
        PMU_SET_BITFIELD(g_pmdLdoProfileSetting3,
                         RDA_PMU_VMEM_IBIT,
                         level);
    pmd_RegWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoProfileSetting3);
    pmd_RegWrite(RDA_ADDR_LDO_LP_SETTING3, g_pmdLdoProfileSetting3);
    hal_SysExitCriticalSection(scStatus);
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

PUBLIC VOID pmd_SetPadVoltage(UINT8 level)
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
    // Init Vcore in active & lp mode
    pmd_InitCoreVoltage();

    // Set Vmem voltage in active & lp mode
    pmd_SetMemVoltage(7);

    // Set Vpad voltage in active mode
    pmd_SetPadVoltage(gc_pmdPadLevelAct);

    // Set v_mmc to 3V
    pmd_SetMmcVoltage(1);

    // Disable touch screen irqs (they will be enabled later if needed)
    // Disable key irq
    // Enable charger irq
    // Enter active mode
    g_pmdIrqSettingLatest = 0;
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_MASK|RDA_PMU_PENIRQ_MASK|
                         RDA_PMU_KEYIRQ_MASK|RDA_PMU_HOLD_PRECHARGER_EFUSE|
                         RDA_PMU_PD_MODE_SEL);

    // Disable headphone detection irqs (they will be enabled later if needed)
    // Enter active mode
    g_pmdHpDetectSettingLatest = 0;
    pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);

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
    if (g_pmdSpeakerPaEnabled != enable)
    {
        if (g_pmdSpeakerLeftChanMode != PMD_SPEAKER_PA_MODE_CLASS_K &&
                g_pmdSpeakerRightChanMode != PMD_SPEAKER_PA_MODE_CLASS_K)
        {
            return;
        }
        // At least one PA is in class K mode
        if (enable)
        {
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_CP, PMD_CORE_VOLTAGE_MEDIUM);
            hal_TimDelay(4);
            if (!g_pmdSpeakerPaCpLowOutput)
            {
#if (AUD_SPK_CHARGE_PUMP_HIGH_OUTPUT)
                // Allow CP to output 5V - 6V
                g_pmdThermalCalib |= RDA_PMU_LED_MODE_CP;
                pmd_RDAWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
                hal_TimDelay(4);
#endif
                for (int i=1; i<=PMD_CHARGE_PUMP_CP_LEVEL; i++)
                {
                    pmd_SpeakerPaSetCpLevel(i);
                    hal_TimDelay(4);
                }
            }
#if (CLASS_D_K_SPK_L_R_DOUBLE)
            if (g_pmdSpeakerPaSetting3 & RDA_PMU_R_ENABLE_CLG)
            {
                // Disable R line output
                pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3,
                             g_pmdSpeakerPaSetting3 & ~RDA_PMU_R_ENABLE_CLG);
            }
#endif
        }
        else
        {
        }
    }
#endif
}

PUBLIC VOID pmd_SpeakerPaPostEnable(BOOL enable)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    // Class K mode might need a higher vcore voltage and a higher charge pump
    // output voltage.
    if (g_pmdSpeakerPaEnabled != enable)
    {
        g_pmdSpeakerPaEnabled = enable;

        if (g_pmdSpeakerLeftChanMode != PMD_SPEAKER_PA_MODE_CLASS_K &&
                g_pmdSpeakerRightChanMode != PMD_SPEAKER_PA_MODE_CLASS_K)
        {
            return;
        }
        // At least one PA is in class K mode
        if (enable)
        {
#if (CLASS_D_K_SPK_L_R_DOUBLE)
            if (!g_pmdSpeakerPaCpLowOutput)
            {
                if (g_pmdSpeakerPaSetting3 & RDA_PMU_R_ENABLE_CLG)
                {
                    hal_TimDelay(4);
                    // Re-enable R line output
                    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3,
                                 g_pmdSpeakerPaSetting3);
                }
            }
#endif
        }
        else
        {
            if (!g_pmdSpeakerPaCpLowOutput)
            {
                for (int i=PMD_CHARGE_PUMP_CP_LEVEL; i>=0; i--)
                {
                    hal_TimDelay(4);
                    pmd_SpeakerPaSetCpLevel(i);
                }
#if (AUD_SPK_CHARGE_PUMP_HIGH_OUTPUT)
                hal_TimDelay(4);
                // Restrict CP to output 4.5V - 5.6V
                g_pmdThermalCalib &= ~RDA_PMU_LED_MODE_CP;
                pmd_RDAWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
#endif
            }
            hal_TimDelay(4);
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_CP, PMD_CORE_VOLTAGE_LOW);
        }
    }
#endif
}

PUBLIC VOID pmd_SpeakerPaSelectChannel(BOOL leftChanEn, BOOL rightChanEn)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
#if (AUD_SPK_LINE_L_R_DOUBLE)
    // L-R-double class AB speaker L/R lines should NOT be enabled at
    // the same time
    //    PMD_ASSERT(!(g_pmdSpeakerLeftChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB &&
    //    g_pmdSpeakerRightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB &&
    //    leftChanEn && rightChanEn),
    //   "Cannot enable both class AB PAs");
#endif

    UINT16 chanSel = RDA_PMU_PD_L_CLG_DR|RDA_PMU_PD_O_L_CLG_DR|
                     RDA_PMU_PD_R_CLG_DR|RDA_PMU_PD_O_R_CLG_DR;
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel leftChanEn=%d rightChanEn=%d ", leftChanEn, rightChanEn);

#if CLASS_D_K_SPK_L_R_DOUBLE
#ifdef RECV_USE_SPK_LINE
#ifdef RECV_USE_SPK_LINE_R

    if(!leftChanEn && rightChanEn)
    {
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, ( g_pmdSpeakerPaSetting3&( ~RDA_PMU_DMODE_EN_CLG_MASK))|RDA_PMU_DMODE_R_CLAB_L_CLAB);
        pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib&(~RDA_PMU_DOUBLE_MODE_EN_CLG));
    }
    else  if(leftChanEn && rightChanEn)
    {
        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);
        pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib|RDA_PMU_DOUBLE_MODE_EN_CLG);
    }


#else

    if(leftChanEn && !rightChanEn)
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel Class AB ");

        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3,( g_pmdSpeakerPaSetting3&( ~RDA_PMU_DMODE_EN_CLG_MASK))|RDA_PMU_DMODE_R_CLAB_L_CLAB);
        pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib&(~RDA_PMU_DOUBLE_MODE_EN_CLG));
    }
    else if(leftChanEn && rightChanEn)
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SpeakerPaSelectChannel Class D ");

        pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);
        pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib|RDA_PMU_DOUBLE_MODE_EN_CLG);
    }

#endif
#endif
#endif

    //
    chanSel |= RDA_PMU_PD_O_L_CLG_REG;
    chanSel |= RDA_PMU_PD_O_R_CLG_REG;
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);


    if(leftChanEn)
    {
        chanSel &= ~RDA_PMU_PD_O_L_CLG_REG;
    }
    else
    {
        chanSel |= RDA_PMU_PD_L_CLG_REG;
    }

    if (rightChanEn)
    {
        chanSel &= ~RDA_PMU_PD_O_R_CLG_REG;
    }
    else
    {
        chanSel |= RDA_PMU_PD_R_CLG_REG;
    }
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);


#endif
}

PUBLIC VOID pmd_SpeakerPaSetMode(PMD_SPEAKER_PA_MODE_T leftChanMode,
                                 PMD_SPEAKER_PA_MODE_T rightChanMode)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
#if (LCD_BACKLIGHT_CHARGE_PUMP)

    PMD_ASSERT( (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_K &&
                 rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_K),
                "PA class K mode expected. %d/%d",
                leftChanMode, rightChanMode);

    // The PA modes of both channels have been initialized
    // to class K, and there is no need to change them here.

#else // !LCD_BACKLIGHT_CHARGE_PUMP

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
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLAB_L_CLD;
        }
        else
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLD_L_CLD;
        }
    }

#if (AUD_SPK_LINE_L_R_DOUBLE)
    PMD_ASSERT(leftChanMode == rightChanMode,
               "Speaker L&R mode different when L&R lines are shorted out");

    // If RECV_USE_SPK_LINE is defined, receiver and speaker share the same
    // physical speaker, but one uses L line and the other uses R line.
    // In this case, both L&R lines should be enabled, even if they are
    // in class AB mode, assuming they will not work at the same time
    // under the control of pmd_SpeakerPaSelectChannel.
#ifndef RECV_USE_SPK_LINE
    // Disable speaker right line if speaker uses both L&R lines,
    // and both L&R lines are in class AB mode.
    if (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB &&
            rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
    {
        g_pmdSpeakerPaSetting3 &= ~RDA_PMU_R_ENABLE_CLG;
    }
    else
    {
        g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG;
    }
#endif
#endif

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);

    // Set CP direct-reg controls
    if (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_K &&
            rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_K)
    {
        // Not to direct-reg bypass CP
        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG);
    }
    else
    {
        // Direct-reg to bypass CP
        g_pmdSpeakerPaSetting1 |= RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG;
    }

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);

    g_pmdSpeakerLeftChanMode = leftChanMode;
    g_pmdSpeakerRightChanMode = rightChanMode;

    hal_SysExitCriticalSection(scStatus);

#endif // !LCD_BACKLIGHT_CHARGE_PUMP
#endif // !(_FLASH_PROGRAMMER || _T_UPGRADE_PROGRAMMER)
}

PUBLIC VOID pmd_SpeakerPaRestoreMode(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    // Restore CP direct-reg controls
#if (AUD_SPK_LINE_NO_CLASS_K) || \
    (AUD_SPK_LINE_L_CLASS_AB) || (AUD_SPK_LINE_R_CLASS_AB)
    // Direct-reg to bypass CP
    g_pmdSpeakerPaSetting1 |= RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG;
#else // Both PA in Class K mode
    // Not to direct-reg bypass CP
    g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG);
#endif // Both PA in Class K mode

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);

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

#if (AUD_SPK_LINE_L_ONLY)

    g_pmdSpeakerPaSetting3 |= RDA_PMU_L_ENABLE_CLG;
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_R_ENABLE_CLG;

#elif (AUD_SPK_LINE_R_ONLY)

    g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG;
    g_pmdSpeakerPaSetting3 &= RDA_PMU_L_ENABLE_CLG;

#else // both L and R are used

    g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG|RDA_PMU_L_ENABLE_CLG;

#if (AUD_SPK_LINE_L_R_DOUBLE) && \
    (AUD_SPK_LINE_L_CLASS_AB) && (AUD_SPK_LINE_R_CLASS_AB)
    // If RECV_USE_SPK_LINE is defined, receiver and speaker share the same
    // physical speaker, but one uses L line and the other uses R line.
    // In this case, both L&R lines should be enabled, even if they are
    // in class AB mode, assuming they will not work at the same time
    // under the control of pmd_SpeakerPaSelectChannel.
#ifndef RECV_USE_SPK_LINE
    // Disable speaker right line if speaker uses both L&R lines,
    // and both L&R lines are in class AB mode.
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_R_ENABLE_CLG;
#endif // !RECV_USE_SPK_LINE
#endif

#endif // both L and R are used

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

#define PMU_REG_NUM (0x47)
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
#ifndef FM_RADIO_WITH_EXTERNAL_ANT
            //restor spk gain
            if(g_pmdMuteSpk)
            {
                g_pmdMuteSpk = FALSE;
                hwp_configRegs->audio_spk_cfg |=  (CFG_REGS_AU_SPK_MUTE_N);
            }
#endif
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

#ifndef FM_RADIO_WITH_EXTERNAL_ANT
    if (hal_GpioGet(g_ear_gpio))
    {
        if(hwp_configRegs->audio_spk_cfg &(CFG_REGS_AU_SPK_MUTE_N))
        {
            g_pmdMuteSpk = TRUE;
            hwp_configRegs->audio_spk_cfg &= ~(CFG_REGS_AU_SPK_MUTE_N);
        }
    }
#endif

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
    if ( gpioId != HAL_GPIO_NONE &&
            (gpioId < HAL_GPIO_1 || gpioId > HAL_GPIO_7) )
    {
        PMD_ASSERT(FALSE, "Earpice detect GPIO should be none or from 1 to 7: 0x%08x", gpioId);
    }

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

    if ((ret & RDA_PMU_CHR_AC_ON) == 0){
        return 0;
    }else{
        if (boot_MonitorKeyPressed(monKeys, 1)){
            return 1;
        } else{
            return 2;
        }
    }
}

VOID pmd_VcomGpioInit(HAL_GPIO_GPIO_ID_T gpioId)
{
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    UINT8 temp=pmd_GetChargerAndBbatKeyStatus();
    bQuickVcomFlag=(temp==1)?TRUE:FALSE;

    if(gpioId > HAL_GPIO_NONE && gpioId < HAL_GPIO_31){
        hal_GpioSetIn(gpioId);
    }
#endif
}

VOID pmd_VcomCheckGpioLevel(VOID)
{
    if(!bQuickVcomFlag){
        HAL_GPIO_GPIO_ID_T gpioId=g_pmdConfig->vcomDetectGpio;
        if(gpioId > HAL_GPIO_NONE && gpioId < HAL_GPIO_31){
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
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return PMD_EAR_DETECT_POLLING;
    }
    else
    {
        return (hal_GpioGet(g_ear_gpio) == 0);
    }
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
    }};
#else
static HAL_APO_ID_T gpioid = {{
        .type = HAL_GPIO_TYPE_O,
         .id = 0
    }};

#endif

UINT16 pmd_get_efuse_gpio(void)
{
    UINT32 temp;
    temp = (hwp_sysCtrl->Reset_Cause & SYS_CTRL_SW_BOOT_MODE(0x3f))>>SYS_CTRL_SW_BOOT_MODE_SHIFT;  //  27:22 reserved.
    if((temp >> 5) == 1)  // if 27bit equal 1 ,use the setting gpio
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
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0);
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
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0);
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
            while(1){};
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

    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0);

    if(gpioid.type == HAL_GPIO_TYPE_IO)
        gpioid.id = pmd_get_efuse_gpio();

    if(gpioid.type == HAL_GPIO_TYPE_IO)
        hal_GpioSetOut(gpioid.gpioId);
    hal_GpioClr(gpioid);
    hal_TimDelay(20);
    value1 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_EFUSE_SEL_WORD_REG, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_SW_PMOS_CTRL_CP, 3);//
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);
    hal_TimDelay(200);
    value1 = PMU_SET_BITFIELD(value1, RDA_PMU_SW_PMOS_CTRL_CP, 0);//set to page 0 [13:12]
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, value1);

    value2 = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OUT);
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0);
    hal_HstSendEvent(0xbbbbbbc2);hal_HstSendEvent(value2);hal_HstSendEvent(gpioid.id );
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
    hal_TimDelay(20);
    valuer =  pmd_GetEfuseLockHost();
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0);
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
    hal_TimDelay(20);
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

        while(1){};
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
    {
        hal_GpioClr(gpioid);
        hal_GpioSetOut(gpioid.gpioId);
    }

    hal_GpioClr(gpioid);
    hal_TimDelay(20);
    pmd_SetPadVoltage(0);
    pmd_GetEfuseGpadc(FALSE);
    valuer =  pmd_GetEfuseLockHost();
    pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING3, 0);
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
    hal_TimDelay(20);
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
        while(1){};
    }
    else
    {

        timedelay = 0;
        for(UINT8 i=0;i<10;i++)
        {
            if(i>=4)
            {
                timedelay += 100;
                pmd_SetEfuseLockHost_again(timedelay);
            }else
            {
                pmd_SetEfuseLockHost_again(0);
            }
        }
        pmd_SetEfuseToDefault();
        hal_HstSendEvent(0xbbbbffff);
        while(1){};

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
                boot_BootSectorSetCommand(BOOT_SECTOR_CMD_ENTER_BOOT_MONITOR);
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


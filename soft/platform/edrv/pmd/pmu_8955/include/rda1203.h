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


#ifndef _OPAL_H_
#define _OPAL_H_

// =============================================================================
//  MACROS
// =============================================================================

typedef enum
{
    RDA_ADDR_REVISION_ID                      = 0x00,
    RDA_ADDR_IRQ_SETTINGS                     = 0x01,
    RDA_ADDR_LDO_SETTINGS                     = 0x02,
    RDA_ADDR_LDO_ACTIVE_SETTING1              = 0x03,
    RDA_ADDR_LDO_ACTIVE_SETTING2              = 0x04,
    RDA_ADDR_LDO_ACTIVE_SETTING3              = 0x05,
    RDA_ADDR_LDO_ACTIVE_SETTING4              = 0x06,
    RDA_ADDR_LDO_ACTIVE_SETTING5              = 0x07,
    RDA_ADDR_LDO_LP_SETTING1                  = 0x08,
    RDA_ADDR_LDO_LP_SETTING2                  = 0x09,
    RDA_ADDR_LDO_LP_SETTING3                  = 0x0A,
    RDA_ADDR_LDO_LP_SETTING4                  = 0x0B,
    RDA_ADDR_LDO_LP_SETTING5                  = 0x0C,
    RDA_ADDR_LDO_POWER_ON_SETTING1            = 0x0D,
    RDA_ADDR_LDO_POWER_ON_SETTING2            = 0x0E,
    RDA_ADDR_LDO_POWER_ON_SETTING3            = 0x0F,
    RDA_ADDR_LDO_POWER_ON_SETTING4            = 0x10,
    RDA_ADDR_HP_DETECT_SETTING                = 0x11,
    RDA_ADDR_CHARGER_SETTING1                 = 0x12,
    RDA_ADDR_CHARGER_SETTING2                 = 0x13,
    RDA_ADDR_CHARGER_STATUS                   = 0x14,
    RDA_ADDR_CHARGER_CONTROL                  = 0x15,
    RDA_ADDR_CALIBRATION_SETTING1             = 0x16,
    RDA_ADDR_CALIBRATION_SETTING2             = 0x17,
    RDA_ADDR_MISC_CONTROL                     = 0x18,
    RDA_ADDR_LED_SETTING1                     = 0x19,
    RDA_ADDR_LED_SETTING2                     = 0x1A,
    RDA_ADDR_LED_SETTING3                     = 0x1B,
    RDA_ADDR_AUDIO_CONTROL                    = 0x1C,
    RDA_ADDR_TOUCH_SCREEN_CONTROL             = 0x1D,
    RDA_ADDR_TOUCH_SCREEN_RESULTS1            = 0x1E,
    RDA_ADDR_TOUCH_SCREEN_RESULTS2            = 0x1F,
    RDA_ADDR_TOUCH_SCREEN_RESULTS3            = 0x20,
    RDA_ADDR_EFUSE_OPT_SETTING1               = 0x21,
    RDA_ADDR_EFUSE_OPT_SETTING2               = 0x22,
    RDA_ADDR_EFUSE_OPT_SETTING3               = 0x23,
    RDA_ADDR_EFUSE_OPT_SETTING4               = 0x24,
    RDA_ADDR_EFUSE_OPT_SETTING5               = 0x25,
    RDA_ADDR_DEBUG1                           = 0x26,
    RDA_ADDR_SPEAKER_PA_SETTING1              = 0x27,
    RDA_ADDR_SIM_ACTIVE_SETTING               = 0x28,
    RDA_ADDR_SIM_LP_SETTING                   = 0x29,
    RDA_ADDR_SIM_POWER_ON_SETTING             = 0x2A,
    RDA_ADDR_SIM_POWER_OFF_SETTING            = 0x2B,
    RDA_ADDR_DEBUG2                           = 0x2C,
    RDA_ADDR_LDO_BUCK1                        = 0x2D,
    RDA_ADDR_LDO_BUCK2                        = 0x2E,
    RDA_ADDR_DCDC_BUCK                        = 0x2F,
    RDA_ADDR_PMU_RESET                        = 0x30,
    RDA_ADDR_LDO_POWER_OFF_SETTING1           = 0x31,
    RDA_ADDR_LDO_POWER_OFF_SETTING2           = 0x32,
    RDA_ADDR_LDO_POWER_OFF_SETTING3           = 0x33,
    RDA_ADDR_LDO_POWER_OFF_SETTING4           = 0x34,
    RDA_ADDR_LDO_POWER_OFF_SETTING5           = 0x35,
    RDA_ADDR_THERMAL_CALIBRATION              = 0x36,
    RDA_ADDR_DEBUG3                           = 0x37,
    RDA_ADDR_LED_SETTING4                     = 0x38,
    RDA_ADDR_DEBUG4                           = 0x39,
    RDA_ADDR_GPADC1                           = 0x3A,
    RDA_ADDR_GPADC2                           = 0x3B,
    RDA_ADDR_POWER_TIMING1                    = 0x3C,
    RDA_ADDR_POWER_TIMING2                    = 0x3D,
    RDA_ADDR_LED_SETTING5                     = 0x3E,
    RDA_ADDR_EFUSE_OUT                        = 0x3F,
    RDA_ADDR_SPEAKER_PA_SETTING2              = 0x40,
    RDA_ADDR_SPEAKER_PA_SETTING3              = 0x41,
    RDA_ADDR_SPEAKER_PA_SETTING4              = 0x42,
    RDA_ADDR_SPEAKER_PA_SETTING5              = 0x43,
    RDA_ADDR_TSC_KEY_CONTROL                  = 0x46,

    RDA_ADDR_HOLE                             = 0x47,
    RDA_ADDR_LDO_BUCK_48H                     = 0x48,
    RDA_ADDR_LDO_BUCK_4BH                     = 0x4b,
} RDA_REG_MAP_T;


// IRQ settings 0x1
#define RDA_PMU_PD_MODE_SEL             (1<<0)
#define RDA_PMU_HOLD_PRECHARGER_EFUSE   (1<<2)
#define RDA_PMU_INT_CHR_MASK            (1<<4)
#define RDA_PMU_INT_CHR_CLEAR           (1<<5)
#define RDA_PMU_KEYIRQ_MASK             (1<<6)
#define RDA_PMU_INT_CHR_ON              (1<<7)
#define RDA_PMU_PENIRQ_MASK             (1<<8)
#define RDA_PMU_EOMIRQ_MASK             (1<<9)
#define RDA_PMU_PENIRQ_CLEAR            (1<<10)
#define RDA_PMU_EOMIRQ_CLEAR            (1<<11)
#define RDA_PMU_KEYIRQ_CLEAR            (1<<12)
#define RDA_PMU_KEYIRQ                  (1<<13)
#define RDA_PMU_PENIRQ                  (1<<14)
#define RDA_PMU_EOMIRQ                  (1<<15)
// end IRQ settings

//LDO settings 0x02
#define RDA_PMU_FM_ENABLE                    (1<<0)
#define RDA_PMU_BT_ENABLE                    (1<<1)
#define RDA_PMU_KEY_ENABLE                   (1<<2)
#define RDA_PMU_TSC_ENABLE                   (1<<3)
#define RDA_PMU_VLCD_ENABLE                  (1<<4)
#define RDA_PMU_VCAM_ENABLE                  (1<<5)
#define RDA_PMU_VMIC_ENABLE                  (1<<6)
#define RDA_PMU_VIBR_ENABLE                  (1<<7)
#define RDA_PMU_VRF_ENABLE                   (1<<9)
#define RDA_PMU_VABB_ENABLE                  (1<<10)
#define RDA_PMU_VMMC_ENABLE                  (1<<11)
#define RDA_PMU_VSIM4_ENABLE                 (1<<12)
#define RDA_PMU_VSIM3_ENABLE                 (1<<13)
#define RDA_PMU_VSIM2_ENABLE                 (1<<14)
#define RDA_PMU_VSIM1_ENABLE                 (1<<15)
#define RDA_PMU_VSIM_ENABLE_MASK             (0xf<<12)
// end LDO settings

//LDO active/lp/pon setting1 0x03/0x08/0x0d
#define RDA_PMU_NORMAL_MODE               (1<<0)
#define RDA_PMU_VSPIMEM_OFF               (1<<1)
//#define RDA_PMU_VBLLED_OFF                (1<<2)
#define RDA_PMU_VMIC_OFF                  (1<<3)
#define RDA_PMU_VUSB_OFF                  (1<<4)
#define RDA_PMU_VIBR_OFF                  (1<<5)
#define RDA_PMU_VMMC_OFF                  (1<<6)
#define RDA_PMU_VLCD_OFF                  (1<<7)
#define RDA_PMU_VCAM_OFF                  (1<<8)
#define RDA_PMU_VRF_OFF                   (1<<9)
#define RDA_PMU_VABB_OFF                  (1<<10)
#define RDA_PMU_VPAD_OFF                  (1<<11)
#define RDA_PMU_VMEM_OFF                  (1<<12)
#define RDA_PMU_BUCK1_LDO_OFF             (1<<13)
#define RDA_PMU_BUCK1_PFM_ON              (1<<14)
#define RDA_PMU_BUCK1_ON                  (1<<15)

#define RDA_PMU_LDO_EN(n)              (((n)&0xFFFF)<<0)
#define RDA_PMU_LDO_EN_MASK            (0xFFFF<<0)
#define RDA_PMU_LDO_EN_SHIFT           (0)
// end LDO active/lp/pon setting1

// LDO active/lp/pon setting2  0x04/0x09/0x0e
#define RDA_PMU_RGB_LED_OFF       (1<<0)
#define RDA_PMU_BUCK2_LDO_OFF     (1<<2)
#define RDA_PMU_BUCK2_PFM_ON      (1<<3)
#define RDA_PMU_BUCK2_ON          (1<<4)
//#define RDA_PMU_VBOOST_ON         (1<<5)
#define RDA_PMU_VINTRF_OFF        (1<<6)

#define RDA_PMU_VIBR_SEL_MASK     (1<<7)
#define RDA_PMU_VIBR_VSEL_1_8     (1<<7)
#define RDA_PMU_VIBR_VSEL_2_8     (0<<7)

#define RDA_PMU_VMMC_SEL_MASK     (1<<8)
#define RDA_PMU_VMMC_VSEL_1_8     (1<<8)
#define RDA_PMU_VMMC_VSEL_2_8     (0<<8)

#define RDA_PMU_VLCD_SEL_MASK     (1<<9)
#define RDA_PMU_VLCD_VSEL_1_8     (1<<9)
#define RDA_PMU_VLCD_VSEL_2_8     (0<<9)

#define RDA_PMU_VCAM_SEL_MASK     (1<<10)
#define RDA_PMU_VCAM_VSEL_1_8     (1<<10)
#define RDA_PMU_VCAM_VSEL_2_8     (0<<10)

#define RDA_PMU_VRF_SEL_MASK      (1<<11)
#define RDA_PMU_VRF_VSEL_1_8      (1<<11)
#define RDA_PMU_VRF_VSEL_2_8      (0<<11)

#define RDA_PMU_VPAD_SEL_MASK     (1<<12)
#define RDA_PMU_VPAD_VSEL_1_8     (1<<12)
#define RDA_PMU_VPAD_VSEL_2_8     (0<<12)

#define RDA_PMU_VRTC_VBIT_SHIFT   (13)
#define RDA_PMU_VRTC_VBIT_MASK    (0x7<<13)
#define RDA_PMU_VRTC_VBIT(n)      (((n)&0x7)<<13)

// end LDO active/lp/pon setting2

// LDO ACT/LP/PON setting3 0x5/0xa/0xf
#define RDA_PMU_VRTC_DCDC1_EN         (1<<0)
#define RDA_PMU_VRTC_DCDC2_EN         (1<<1)

#define RDA_PMU_VPAD_IBIT_SHIFT       (3)
#define RDA_PMU_VPAD_IBIT_MASK        (0x7<<3)
#define RDA_PMU_VPAD_IBIT(n)          (((n)&0x7)<<3)

#define RDA_PMU_VMEM_IBIT_SHIFT       (6)
#define RDA_PMU_VMEM_IBIT_MASK        (0x7<<6)
#define RDA_PMU_VMEM_IBIT(n)          (((n)&0x7)<<6)

#define RDA_PMU_VSPIMEM_IBIT_SHIFT    (9)
#define RDA_PMU_VSPIMEM_IBIT_MASK     (0x7<<9)
#define RDA_PMU_VSPIMEM_IBIT(n)       (((n)&0x7)<<9)

#define RDA_PMU_VBACKUP_VBIT_SHIFT    (12)
#define RDA_PMU_VBACKUP_VBIT_MASK     (0x7<<12)
#define RDA_PMU_VBACKUP_VBIT(n)       (((n)&0x7)<<12)
// end LDO ACT/LP/PON setting3

// LDO ACT setting4 0x6
// NOTE:
//   These settings are valid for all profiles
#define RDA_PMU_VIBR_IBIT_SHIFT       (0)
#define RDA_PMU_VIBR_IBIT_MASK        (0x7<<0)
#define RDA_PMU_VIBR_IBIT(n)          (((n)&0x7)<<0)

#define RDA_PMU_VMMC_IBIT_SHIFT       (3)
#define RDA_PMU_VMMC_IBIT_MASK        (0x7<<3)
#define RDA_PMU_VMMC_IBIT(n)          (((n)&0x7)<<3)

#define RDA_PMU_VLCD_IBIT_SHIFT       (6)
#define RDA_PMU_VLCD_IBIT_MASK        (0x7<<6)
#define RDA_PMU_VLCD_IBIT(n)          (((n)&0x7)<<6)

#define RDA_PMU_VCAM_IBIT_SHIFT       (9)
#define RDA_PMU_VCAM_IBIT_MASK        (0x7<<9)
#define RDA_PMU_VCAM_IBIT(n)          (((n)&0x7)<<9)

#define RDA_PMU_VRF_IBIT_SHIFT        (12)
#define RDA_PMU_VRF_IBIT_MASK         (0x7<<12)
#define RDA_PMU_VRF_IBIT(n)           (((n)&0x7)<<12)
// end LDO ACT setting4

// LDO ACT/LP setting5 0x7/0xc
// NOTE:
//   VRGB_LED/VABB bits are located in ACT setting5 only
//   and are valid for all profiles
#define RDA_PMU_VRGB_LED_IBIT_SHIFT  (0)
#define RDA_PMU_VRGB_LED_IBIT_MASK   (0x7<<0)
#define RDA_PMU_VRGB_LED_IBIT(n)     (((n)&0x7)<<0)

#define RDA_PMU_VRGB_LED_VSEL_SHIFT  (3)
#define RDA_PMU_VRGB_LED_VSEL_MASK   (0x7<<3)
#define RDA_PMU_VRGB_LED_VSEL(n)     (((n)&0x7)<<3)

#define RDA_PMU_VABB_IBIT_SHIFT      (6)
#define RDA_PMU_VABB_IBIT_MASK       (0x7<<6)
#define RDA_PMU_VABB_IBIT(n)         (((n)&0x7)<<6)
// End of NOTE

// NOTE:
//   VUSB/VSIM PON bits are located in LP setting5 only
//   and are valid for power-on profile
#define RDA_PMU_VUSB_IBIT_PON_SHIFT  (0)
#define RDA_PMU_VUSB_IBIT_PON_MASK   (0x7<<0)
#define RDA_PMU_VUSB_IBIT_PON(n)     (((n)&0x7)<<0)

#define RDA_PMU_VSIM4_VSEL_PON_1_8   (1<<3)
#define RDA_PMU_VSIM4_VSEL_PON_2_8   (0<<3)
#define RDA_PMU_VSIM4_SEL_PON_MASK   (1<<3)
#define RDA_PMU_VSIM3_VSEL_PON_1_8   (1<<4)
#define RDA_PMU_VSIM3_VSEL_PON_2_8   (0<<4)
#define RDA_PMU_VSIM3_SEL_PON_MASK   (1<<4)
#define RDA_PMU_VSIM2_VSEL_PON_1_8   (1<<5)
#define RDA_PMU_VSIM2_VSEL_PON_2_8   (0<<5)
#define RDA_PMU_VSIM2_SEL_PON_MASK   (1<<5)
#define RDA_PMU_VSIM1_VSEL_PON_1_8   (1<<6)
#define RDA_PMU_VSIM1_VSEL_PON_2_8   (0<<6)
#define RDA_PMU_VSIM1_SEL_PON_MASK   (1<<6)
// End of NOTE

#define RDA_PMU_VUSB_IBIT_SHIFT      (9)
#define RDA_PMU_VUSB_IBIT_MASK       (0x7<<9)
#define RDA_PMU_VUSB_IBIT(n)         (((n)&0x7)<<9)

#define RDA_PMU_VSIM4_VSEL_1_8       (1<<12)
#define RDA_PMU_VSIM4_VSEL_2_8       (0<<12)
#define RDA_PMU_VSIM4_SEL_MASK       (1<<12)
#define RDA_PMU_VSIM3_VSEL_1_8       (1<<13)
#define RDA_PMU_VSIM3_VSEL_2_8       (0<<13)
#define RDA_PMU_VSIM3_SEL_MASK       (1<<13)
#define RDA_PMU_VSIM2_VSEL_1_8       (1<<14)
#define RDA_PMU_VSIM2_VSEL_2_8       (0<<14)
#define RDA_PMU_VSIM2_SEL_MASK       (1<<14)
#define RDA_PMU_VSIM1_VSEL_1_8       (1<<15)
#define RDA_PMU_VSIM1_VSEL_2_8       (0<<15)
#define RDA_PMU_VSIM1_SEL_MASK       (1<<15)
// end LDO ACT/LP/PON setting5

// LDO LP setting4 0xb
// NOTE:
//   These settings are valid for all profiles
#define RDA_PMU_VINTRF_IBIT_SHIFT    (0)
#define RDA_PMU_VINTRF_IBIT_MASK     (0x7<<0)
#define RDA_PMU_VINTRF_IBIT(n)       (((n)&0x7)<<0)

#define RDA_PMU_VINTRF_SEL_MASK      (1<<12)
#define RDA_PMU_VINTRF_VSEL_1_8      (1<<12)
#define RDA_PMU_VINTRF_VSEL_2_8      (0<<12)
// end LDO LP setting4

// HP Detect setting 0x11
#define RDA_PMU_HP_OUT_MASK          (1<<0)
#define RDA_PMU_HP_IN_MASK           (1<<1)
#define RDA_PMU_HP_OUT_CLEAR         (1<<2)
#define RDA_PMU_HP_IN_CLEAR          (1<<3)
#define RDA_PMU_HP_OUT               (1<<4)
#define RDA_PMU_HP_IN                (1<<5)
#define RDA_PMU_HP_DETECT            (1<<6)
// end HP Detect setting

// Charger setting1 0x12
#define RDA_PMU_BG_CAL_TC_BIT2(n)     (((n)&0x1f)<<0)
#define RDA_PMU_BG_CAL_TC_BIT2_MASK   (0x1f<<0)
#define RDA_PMU_BG_CAL_TC_BIT2_SHIFT  (0)

#define RDA_PMU_BG_DIS_CHOPPER        (1<<5)
#define RDA_PMU_BG_DIS_CHOPPER_FILT   (1<<6)
#define RDA_PMU_IREF_CTL_BG_PMU       (1<<7)

#define RDA_PMU_UV_SEL(n)             (((n)&0x3)<<8)
#define RDA_PMU_UV_SEL_MASK           (0x3<<8)
#define RDA_PMU_UV_SEL_SHIFT          (8)

#define RDA_PMU_CHR_BYPASS_CHOPPER    (1<<11)

#define RDA_PMU_CHR_AUXADC_SEL(n)     (((n)&0x3)<<12)
#define RDA_PMU_CHR_AUXADC_SEL_MASK   (0x3<<12)
#define RDA_PMU_CHR_AUXADC_SEL_SHIFT  (12)

#define RDA_PMU_CHR_CV_LOWGAIN        (1<<14)
#define RDA_PMU_CHR_ALWAYS_CC         (1<<15)
// End of charger setting1

// Charger setting2 0x13
#define RDA_PMU_CHR_VFB_SEL_REG(n)     (((n)&0xf)<<0)
#define RDA_PMU_CHR_VFB_SEL_REG_MASK   (0xf<<0)
#define RDA_PMU_CHR_VFB_SEL_REG_SHIFT  (0)

#define RDA_PMU_CHR_PRE_IBIT_REG(n)    (((n)&0x7)<<4)
#define RDA_PMU_CHR_PRE_IBIT_REG_MASK  (0x7<<4)
#define RDA_PMU_CHR_PRE_IBIT_REG_SHIFT (4)
#define RDA_PMU_CHR_PRE_IBIT_79MA       RDA_PMU_CHR_PRE_IBIT_REG(0)
#define RDA_PMU_CHR_PRE_IBIT_98MA       RDA_PMU_CHR_PRE_IBIT_REG(1)
#define RDA_PMU_CHR_PRE_IBIT_116MA      RDA_PMU_CHR_PRE_IBIT_REG(2)
#define RDA_PMU_CHR_PRE_IBIT_135MA      RDA_PMU_CHR_PRE_IBIT_REG(3)
#define RDA_PMU_CHR_PRE_IBIT_154MA      RDA_PMU_CHR_PRE_IBIT_REG(4)
#define RDA_PMU_CHR_PRE_IBIT_172MA      RDA_PMU_CHR_PRE_IBIT_REG(5)
#define RDA_PMU_CHR_PRE_IBIT_191MA      RDA_PMU_CHR_PRE_IBIT_REG(6)
#define RDA_PMU_CHR_PRE_IBIT_213MA      RDA_PMU_CHR_PRE_IBIT_REG(7)

#define RDA_PMU_CHR_CC_IBIT_REG(n)     (((n)&0x7)<<7)
#define RDA_PMU_CHR_CC_IBIT_REG_MASK   (0x7<<7)
#define RDA_PMU_CHR_CC_IBIT_REG_SHIFT  (7)
#define RDA_PMU_CHR_CC_IBIT_100MA      RDA_PMU_CHR_CC_IBIT_REG(0)
#define RDA_PMU_CHR_CC_IBIT_200MA      RDA_PMU_CHR_CC_IBIT_REG(1)
#define RDA_PMU_CHR_CC_IBIT_300MA      RDA_PMU_CHR_CC_IBIT_REG(2)
#define RDA_PMU_CHR_CC_IBIT_400MA      RDA_PMU_CHR_CC_IBIT_REG(3)
#define RDA_PMU_CHR_CC_IBIT_450MA      RDA_PMU_CHR_CC_IBIT_REG(4)
#define RDA_PMU_CHR_CC_IBIT_500MA      RDA_PMU_CHR_CC_IBIT_REG(5)
#define RDA_PMU_CHR_CC_IBIT_600MA      RDA_PMU_CHR_CC_IBIT_REG(6)
#define RDA_PMU_CHR_CC_IBIT_700MA      RDA_PMU_CHR_CC_IBIT_REG(7)

#define RDA_PMU_CHR_VFB_SEL_DR         (1<<10)
#define RDA_PMU_CHR_PRE_IBIT_DR        (1<<11)
#define RDA_PMU_CHR_CC_IBIT_DR         (1<<12)
// End of charger setting 2

// Charger status 0x14
#define RDA_PMU_CHR_VREG   (1<<2)
#define RDA_PMU_CHR_AC_ON   (1<<7)
#define RDA_PMU_PENIRQ_B   (1<<11)
// end charger status

// Charger control 0x15
#define RDA_PMU_BAT_MON_TIME_DELAY(n)    (((n)&0x3)<<0)
#define RDA_PMU_BAT_MON_TIME_DELAY_MASK  (0x3<<0)
#define RDA_PMU_BAT_MON_TIME_DELAY_SHIFT (0)

#define RDA_PMU_BAT_MON_ENABLE           (1<<2)

#define RDA_PMU_CHR_EN_DUTY_CYCLE(n)     (((n)&0x3)<<3)
#define RDA_PMU_CHR_EN_DUTY_CYCLE_MASK   (0x3<<3)
#define RDA_PMU_CHR_EN_DUTY_CYCLE_SHIFT  (3)

#define RDA_PMU_CHR_EN_FREQ(n)           (((n)&0x3)<<5)
#define RDA_PMU_CHR_EN_FREQ_MASK         (0x3<<5)
#define RDA_PMU_CHR_EN_FREQ_SHIFT        (5)

#define RDA_PMU_CHR_CC_MODE_REG          (1<<12)
#define RDA_PMU_CHR_PRE_MODE_REG          (0<<12)
#define RDA_PMU_CHR_CC_MODE_DR           (1<<13)
#define RDA_PMU_CHR_ENABLE_REG           (1<<14)
#define RDA_PMU_CHR_ENABLE_DR            (1<<15)
// end charger control

// Charger calib setting1 0x16
#define RDA_PMU_BGAP_CAL_BIT_REG(n)      (((n)&0x3f)<<0)
#define RDA_PMU_BGAP_CAL_BIT_REG_MASK    (0x3f<<0)
#define RDA_PMU_BGAP_CAL_BIT_REG_SHIFT   (0)

#define RDA_PMU_BGAP_CAL_BIT_DR          (1<<6)
#define RDA_PMU_RESETN_BGAP_CAL_REG      (1<<7)
#define RDA_PMU_RESETN_BGAP_CAL_DR       (1<<8)
#define RDA_PMU_LP_MODE_BG_REG           (1<<10)
#define RDA_PMU_LP_MODE_BG_DR            (1<<11)
#define RDA_PMU_BGAP_CAL_POLARITY        (1<<12)
#define RDA_PMU_BGAP_CAL_CLOCK_INV       (1<<13)
#define RDA_PMU_VBAT_OVER_3P2_BYPASS     (1<<14)
#define RDA_PMU_BGAP_CAL_BYPASS          (1<<15)
// End of charget calib setting1

// Charger calib setting2 0x17
#define RDA_PMU_PD_CHARGE_LDO            (1<<0)

#define RDA_PMU_TS_I_CTRL_BATTERY(n)     (((n)&0xf)<<11)
#define RDA_PMU_TS_I_CTRL_BATTERY_MASK   (0xf<<11)
#define RDA_PMU_TS_I_CTRL_BATTERY_SHIFT  (11)

#define RDA_PMU_PU_TS_BATTERY            (1<<15)
// End of charget calib setting2

// MISC Control 0x18
#define RDA_PMU_LDO_AVDD3_BIT(n)         (((n)&7)<<0)
#define RDA_PMU_LDO_AVDD3_BIT_MASK       (7<<0)
#define RDA_PMU_LDO_AVDD3_BIT_SHIFT      (0)
#define RDA_PMU_CLK2M_FTUN_BIT(n)        (((n)&7)<<3)
#define RDA_PMU_CLK2M_FTUN_BIT_MASK      (7<<3)
#define RDA_PMU_CLK2M_FTUN_BIT_SHIFT     (3)
#define RDA_PMU_PD_LDO_AVDD3_REG         (1<<6)
#define RDA_PMU_PD_LDO_AVDD3_DR          (1<<7)
#define RDA_PMU_PU_CLK_4M_REG            (1<<8)
#define RDA_PMU_PU_CLK_4M_DR             (1<<9)
#define RDA_PMU_PU_CLK_32K_REG           (1<<10)
#define RDA_PMU_PU_CLK_32K_DR            (1<<11)
// End of MISC Control

// LED setting1 0x19
#define RDA_PMU_DIM_BL_REG               (1<<0)
#define RDA_PMU_DIM_BL_DR                (1<<1)
#define RDA_PMU_DIM_LED_B_REG            (1<<2)
#define RDA_PMU_DIM_LED_B_DR             (1<<3)
#define RDA_PMU_DIM_LED_G_REG            (1<<4)
#define RDA_PMU_DIM_LED_G_DR             (1<<5)
#define RDA_PMU_DIM_LED_R_REG            (1<<6)
#define RDA_PMU_DIM_LED_R_DR             (1<<7)
#define RDA_PMU_PWM_RGB_PMU_MODE         (1<<8)
#define RDA_PMU_PWM_BL_ENABLE            (1<<9)

#define RDA_PMU_PWM_RGB_FREQ(n)          (((n)&0xf)<<12)
#define RDA_PMU_PWM_RGB_FREQ_MASK        (0xf<<12)
#define RDA_PMU_PWM_RGB_FREQ_SHIFT       (12)
// end LED setting1 0x19

// LED setting2 0x1a
#define RDA_PMU_BL_OFF_LP                  (1<<1)
#define RDA_PMU_BL_OFF_ACT                 (1<<2)
#define RDA_PMU_BL_OFF_PON                 (1<<3)
#define RDA_PMU_BL_IBIT_LP(n)              (((n)&0xf)<<4)
#define RDA_PMU_BL_IBIT_LP_MASK            (0xf<<4)
#define RDA_PMU_BL_IBIT_LP_SHIFT           (4)
#define RDA_PMU_BL_IBIT_ACT(n)             (((n)&0xf)<<8)
#define RDA_PMU_BL_IBIT_ACT_MASK           (0xf<<8)
#define RDA_PMU_BL_IBIT_ACT_SHIFT          (8)
#define RDA_PMU_BL_IBIT_PON(n)             (((n)&0xf)<<12)
#define RDA_PMU_BL_IBIT_PON_MASK           (0xf<<12)
#define RDA_PMU_BL_IBIT_PON_SHIFT          (12)
// end LED setting2 0x1a

// LED setting3 0x1b
#define RDA_PMU_PWM_BL_FREQ_SEL            (1<<5)

#define RDA_PMU_PWM_BL_FREQ(n)             (((n)&0xf)<<6)
#define RDA_PMU_PWM_BL_FREQ_MASK           (0xf<<6)
#define RDA_PMU_PWM_BL_FREQ_SHIFT          (6)

#define RDA_PMU_PWM_BL_DUTY_CYCLE(n)       (((n)&0x1f)<<10)
#define RDA_PMU_PWM_BL_DUTY_CYCLE_MASK     (0x1f<<10)
#define RDA_PMU_PWM_BL_DUTY_CYCLE_SHIFT    (10)
// end LED setting3 0x1b

// audio control 0x1c
#define RDA_PMU_TIMER_SAMP_NEG_TSC         (1<<1)
#define RDA_PMU_TIMER_SAMP_POS_TSC         (1<<2)

#define RDA_PMU_TIMER_X_TO_Y_SEL(n)        (((n)&0x3)<<3)
#define RDA_PMU_TIMER_X_TO_Y_SEL_MASK      (0x3<<3)
#define RDA_PMU_TIMER_X_TO_Y_SEL_SHIFT     (3)

#define RDA_PMU_TIMER_INT_TO_X_SEL(n)      (((n)&0x7)<<5)
#define RDA_PMU_TIMER_INT_TO_X_SEL_MASK    (0x7<<5)
#define RDA_PMU_TIMER_INT_TO_X_SEL_SHIFT   (5)

#define RDA_PMU_TIMER_PEN_IRQ_STABLE(n)    (((n)&0x7)<<8)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_MASK  (0x7<<8)
#define RDA_PMU_TIMER_PEN_IRQ_STABLE_SHIFT (8)

#define RDA_PMU_PWM_CLK_DIV2_ENABLE        (1<<11)

#define RDA_PMU_DELAY_BEFORE_SAMP(n)       (((n)&3)<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_MASK     (3<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_SHIFT    (12)
#define RDA_PMU_DELAY_BEFORE_SAMP_2US      (0<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_4US      (1<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_8US      (2<<12)
#define RDA_PMU_DELAY_BEFORE_SAMP_16US     (3<<12)

#define RDA_PMU_BYPASS_BB_READ             (1<<14)
#define RDA_PMU_MEAS_WAIT_CLR_PENIRQ       (1<<15)
// end audio control 0x1c

// touch screen control 0x1d
#define RDA_PMU_CONV_CLK_INV               (1<<2)
#define RDA_PMU_SAMP_CLK_INV               (1<<3)
#define RDA_PMU_SAR_ADC_MODE               (1<<4)
#define RDA_PMU_SAR_OUT_POLARITY           (1<<5)
#define RDA_PMU_WAIT_BB_READ_TIMESL(n)     (((n)&3)<<6)
#define RDA_PMU_WAIT_BB_READ_TIMESL_MASK   (3<<6)
#define RDA_PMU_WAIT_BB_READ_TIMESL_SHIFT  (6)
#define RDA_PMU_SAR_VERF_BIT(n)            (((n)&3)<<8)
#define RDA_PMU_SAR_VERF_BIT_MASK          (3<<8)
#define RDA_PMU_SAR_VERF_BIT_SHIFT         (8)
#define RDA_PMU_TSC_SAR_SEL(n)             (((n)&3)<<10)
#define RDA_PMU_TSC_SAR_SEL_MASK           (3<<10)
#define RDA_PMU_TSC_SAR_SEL_SHIFT          (10)
#define RDA_PMU_TSC_CLK_DIV_EN             (1<<13)
#define RDA_PMU_TSC_XP_XN_INV              (1<<14)
#define RDA_PMU_TSC_YP_YN_INV              (1<<15)
// end touch screen control

// touch screen results1 0x1e
#define RDA_PMU_TSC_X_VALUE_BIT_MASK       (0x3ff<<0)
#define RDA_PMU_TSC_X_VALUE_BIT_SHIFT      (0)
#define RDA_PMU_TSC_X_VALUE_VALID          (1<<10)
// end touch screen results1

// touch screen results2 0x1f
#define RDA_PMU_TSC_Y_VALUE_BIT_MASK       (0x3ff<<0)
#define RDA_PMU_TSC_Y_VALUE_BIT_SHIFT      (0)
#define RDA_PMU_TSC_Y_VALUE_VALID          (1<<10)
// end touch screen results2

// touch screen results3 0x20
#define RDA_PMU_GPADC_VALUE_BIT_MASK       (0x3ff<<0)
#define RDA_PMU_GPADC_VALUE_BIT_SHIFT      (0)
#define RDA_PMU_GPADC_VALUE_VALID          (1<<10)
// end touch screen results3

// Efuse and OTP setting1 0x21
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_2(n)     (((n)&0x3)<<8)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_2_MASK   (0x3<<8)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_2_SHIFT  (8)

#define RDA_PMU_EFUSE_SEL_WORD_AUTO_1(n)     (((n)&0x3)<<10)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_1_MASK   (0x3<<10)
#define RDA_PMU_EFUSE_SEL_WORD_AUTO_1_SHIFT  (10)

#define RDA_PMU_EFUSE_SEL_WORD_REG(n)        (((n)&0x3)<<12)
#define RDA_PMU_EFUSE_SEL_WORD_REG_MASK      (0x3<<12)
#define RDA_PMU_EFUSE_SEL_WORD_REG_SHIFT     (12)

#define RDA_PMU_EFUSE_SENSE_REG              (1<<14)
#define RDA_PMU_EFUSE_SENSE_DR               (1<<15)
// end of efuse and OTP setting1

// Efuse and OTP setting2 0x22
#define RDA_PMU_PD_OTP_REG                   (1<<0)
#define RDA_PMU_LP_MODE_REG                  (1<<1)
#define RDA_PMU_LP_MODE_DR                   (1<<2)

#define RDA_PMU_SW_NMOS_CTRL_CP_SHIFT        (12)
#define RDA_PMU_SW_NMOS_CTRL_CP_MASK         (0x3<<12)
#define RDA_PMU_SW_NMOS_CTRL_CP(n)           (((n)&0x3)<<12)

#define RDA_PMU_SW_PMOS_CTRL_CP_SHIFT        (14)
#define RDA_PMU_SW_PMOS_CTRL_CP_MASK         (0x3<<14)
#define RDA_PMU_SW_PMOS_CTRL_CP(n)           (((n)&0x3)<<14)
// end of efuse and OTP setting2

// Efuse and OTP setting4 0x24
#define RDA_PMU_EFUSE_OUT_READ_1_MASK       (0xffff<<0)
#define RDA_PMU_EFUSE_OUT_READ_1_SHIFT      (0)
// end of efuse and OTP setting4

// Efuse and OTP setting4 0x25
#define RDA_PMU_EFUSE_OUT_READ_2_MASK       (0xffff<<0)
#define RDA_PMU_EFUSE_OUT_READ_2_SHIFT      (0)
// end of efuse and OTP setting4

// Speaker PA setting1 0x27
#define RDA_PMU_BYPASS_CP_REG            (1<<0)
#define RDA_PMU_BYPASS_CP_DR             (1<<1)
#define RDA_PMU_PD_CP_REG                (1<<2)
#define RDA_PMU_PD_CP_DR                 (1<<3)

#define RDA_PMU_VBP_CTRL_CP_SHIFT        (4)
#define RDA_PMU_VBP_CTRL_CP_MASK         (0x3<<4)
#define RDA_PMU_VBP_CTRL_CP(n)           (((n)&0x3)<<4)

#define RDA_PMU_CLG_DET_OUT_TIME_SHIFT   (6)
#define RDA_PMU_CLG_DET_OUT_TIME_MASK    (0x3<<6)
#define RDA_PMU_CLG_DET_OUT_TIME(n)      (((n)&0x3)<<6)

#define RDA_PMU_CLG_DET_OUT_POLL         (1<<8)
#define RDA_PMU_CLk_SEL_CP               (1<<9)

#define RDA_PMU_VCP_OUT_BIT_SHIFT        (10)
#define RDA_PMU_VCP_OUT_BIT_MASK         (0xf<<10)
#define RDA_PMU_VCP_OUT_BIT(n)           (((n)&0xf)<<10)

#define RDA_PMU_DELAY_BIT_CP_SHIFT       (14)
#define RDA_PMU_DELAY_BIT_CP_MASK        (0x3<<14)
#define RDA_PMU_DELAY_BIT_CP(n)          (((n)&0x3)<<14)
// end of speaker PA setting1

// sim ACT/LP/PON/POFF setting 0x28/0x29/0x2a/0x2b
#define RDA_PMU_VSIM4_IBIT_SHIFT         (0)
#define RDA_PMU_VSIM4_IBIT_MASK          (7<<0)
#define RDA_PMU_VSIM4_IBIT(n)            (((n)&7)<<0)

#define RDA_PMU_VSIM3_IBIT_SHIFT         (3)
#define RDA_PMU_VSIM3_IBIT_MASK          (7<<3)
#define RDA_PMU_VSIM3_IBIT(n)            (((n)&7)<<3)

#define RDA_PMU_VSIM2_IBIT_SHIFT         (6)
#define RDA_PMU_VSIM2_IBIT_MASK          (7<<6)
#define RDA_PMU_VSIM2_IBIT(n)            (((n)&7)<<6)

#define RDA_PMU_VSIM1_IBIT_SHIFT         (9)
#define RDA_PMU_VSIM1_IBIT_MASK          (7<<9)
#define RDA_PMU_VSIM1_IBIT(n)            (((n)&7)<<9)

#define RDA_PMU_VSIM4_OFF                (1<<12)
#define RDA_PMU_VSIM3_OFF                (1<<13)
#define RDA_PMU_VSIM2_OFF                (1<<14)
#define RDA_PMU_VSIM1_OFF                (1<<15)
// end sim interface2

// LDO buck1/buck2 0x2d/0x2e
#define RDA_PMU_ANTIRING_DIS_BUCK        (1<<0)
#define RDA_PMU_HEAVY_LOAD               (1<<1)
#define RDA_PMU_DISCHARGE_EN             (1<<2)
#define RDA_PMU_LOW_SENSE                (1<<3)

#define RDA_PMU_OSC_FREQ(n)              (((n)&3)<<4)
#define RDA_PMU_OSC_FREQ_MASK            (3<<4)
#define RDA_PMU_OSC_FREQ_SHIFT           (4)

#define RDA_PMU_PFM_THRESHOLD(n)         (((n)&3)<<6)
#define RDA_PMU_PFM_THRESHOLD_MASK       (3<<6)
#define RDA_PMU_PFM_THRESHOLD_SHIFT      (6)

#define RDA_PMU_COUNTER_DISABLE          (1<<8)
#define RDA_PMU_PFM_CLK_DIS_BUCK_REG     (1<<9)
#define RDA_PMU_PFM_CLK_DIS_BUCK_DR      (1<<10)

#define RDA_PMU_PFM_CLK_PRD_BUCK(n)      (((n)&0x3)<<11)
#define RDA_PMU_PFM_CLK_PRD_BUCK_MASK    (0x3<<11)
#define RDA_PMU_PFM_CLK_PRD_BUCK_SHIFT   (11)

#define RDA_PMU_VBUCK_LDO_BIT(n)         (((n)&7)<<13)
#define RDA_PMU_VBUCK_LDO_BIT_MASK       (7<<13)
#define RDA_PMU_VBUCK_LDO_BIT_SHIFT      (13)
// end of LDO buck1/buck2

// DCDC buck1/buck2 0x2f
#define RDA_PMU_VBUCK2_BIT_LP(n)         (((n)&0xf)<<0)
#define RDA_PMU_VBUCK2_BIT_LP_MASK       (0xf<<0)
#define RDA_PMU_VBUCK2_BIT_LP_SHIFT      (0)

#define RDA_PMU_VBUCK2_BIT_NLP(n)        (((n)&0xf)<<4)
#define RDA_PMU_VBUCK2_BIT_NLP_MASK      (0xf<<4)
#define RDA_PMU_VBUCK2_BIT_NLP_SHIFT     (4)

#define RDA_PMU_VBUCK1_BIT_LP(n)         (((n)&0xf)<<8)
#define RDA_PMU_VBUCK1_BIT_LP_MASK       (0xf<<8)
#define RDA_PMU_VBUCK1_BIT_LP_SHIFT      (8)

#define RDA_PMU_VBUCK1_BIT_NLP(n)        (((n)&0xf)<<12)
#define RDA_PMU_VBUCK1_BIT_NLP_MASK      (0xf<<12)
#define RDA_PMU_VBUCK1_BIT_NLP_SHIFT     (12)
// end of DCDC buck1/buck2

// PMU reset 0x30
#define RDA_PMU_SOFT_RESETN              (1<<0)
#define RDA_PMU_REGISTER_RESETN          (1<<15)
// end of PMU reset



// Thermal Calibration 0x36
#define PMU_RESERVED_ANALOG(n)     (((n)&0x3F)<<2)
#define PMU_PU_XTAL_32K            (1<<1)

#define RDA_PMU_BUCK_HEAVY_LOAD          (1<<8)
#define RDA_PMU_CHR_CALMODE_EN_BG_REG    (1<<9)
#define RDA_PMU_CHR_CALMODE_EN_NONCAL    (1<<10)
#define RDA_PMU_CALMODE_EN_BG_CAL        (1<<11)

#define RDA_PMU_THERMO_SEL(n)            (((n)&0x3)<<12)
#define RDA_PMU_THERMO_SEL_MASK          (0x3<<12)
#define RDA_PMU_THERMO_SEL_SHIFT         (12)

#define RDA_PMU_PU_THERMO_PROTECT        (1<<14)
#define RDA_PMU_PD_VD_LDO                (1<<15)
// end Thermal Calibration

// Debug3 0x37
#define RDA_PMU_RESETN_FM_REG            (1<<0)
#define RDA_PMU_RESETN_FM_DR             (1<<1)

#define RDA_PMU_RESETN_FM_TIME_SEL(n)    (((n)&0x3)<<2)
#define RDA_PMU_RESETN_FM_TIME_SEL_MASK  (0x3<<2)
#define RDA_PMU_RESETN_FM_TIME_SEL_SHIFT (2)

#define RDA_PMU_RESETN_BT_REG            (1<<4)
#define RDA_PMU_RESETN_BT_DR             (1<<5)

#define RDA_PMU_RESETN_BT_TIME_SEL(n)    (((n)&0x3)<<6)
#define RDA_PMU_RESETN_BT_TIME_SEL_MASK  (0x3<<6)
#define RDA_PMU_RESETN_BT_TIME_SEL_SHIFT (6)

#define RDA_PMU_OVER_TEMP_BYPASS         (1<<9)
#define RDA_PMU_PU_BGAP_REG              (1<<10)
#define RDA_PMU_PU_BGAP_DR               (1<<11)
#define RDA_PMU_RESETN_BB_REG            (1<<12)
#define RDA_PMU_RESETN_BB_DR             (1<<13)
#define RDA_PMU_RESETN_TSC_REG           (1<<14)
#define RDA_PMU_RESETN_TSC_DR            (1<<15)
// end Debug3

// LED Setting4 0x38
#define RDA_PMU_PWM_B_DUTY_CYCLE(n)         (((n)&0x1f)<<1)
#define RDA_PMU_PWM_B_DUTY_CYCLE_MASK       (0x1f<<1)
#define RDA_PMU_PWM_B_DUTY_CYCLE_SHIFT      (1)

#define RDA_PMU_PWM_G_DUTY_CYCLE(n)         (((n)&0x1f)<<6)
#define RDA_PMU_PWM_G_DUTY_CYCLE_MASK       (0x1f<<6)
#define RDA_PMU_PWM_G_DUTY_CYCLE_SHIFT      (6)

#define RDA_PMU_PWM_R_DUTY_CYCLE(n)         (((n)&0x1f)<<11)
#define RDA_PMU_PWM_R_DUTY_CYCLE_MASK       (0x1f<<11)
#define RDA_PMU_PWM_R_DUTY_CYCLE_SHIFT      (11)

// GPADC Setting2 / 0x3b
#define RDA_PMU_GPADC_ENABLE                (1<<0)
#define RDA_PMU_GPADC_ENABLE_CHANNEL(n)     ((1<<((n)&0x7))<<1)
#define RDA_PMU_GPADC_ENABLE_CHANNEL_MASK   (0xff<<1)
#define RDA_PMU_GPADC_ENABLE_CHANNEL_SHIFT  (1)
#define RDA_PMU_GPADC_CLK_DIV_ENABLE                (1<<12)

#define RDA_PMU_GPADC_VREF_BIT(n)           (((n)&0x3)<<9)
#define RDA_PMU_GPADC_VREF_BIT_MASK         (0x3<<9)
#define RDA_PMU_GPADC_VREF_BIT_SHIFT        (9)

#define RDA_PMU_GPADC_START_TIME(n)         (((n)&0x7)<<13)
#define RDA_PMU_GPADC_START_TIME_MASK       (0x7<<13)
#define RDA_PMU_GPADC_START_TIME_SHIFT      (13)

// LED Setting5 / 0x3e
#define RDA_PMU_LED_B_IBIT(n)         (((n)&0x7)<<0)
#define RDA_PMU_LED_B_IBIT_MASK       (0x7<<0)
#define RDA_PMU_LED_B_IBIT_SHIFT      (0)

#define RDA_PMU_LED_G_IBIT(n)         (((n)&0x7)<<3)
#define RDA_PMU_LED_G_IBIT_MASK       (0x7<<3)
#define RDA_PMU_LED_G_IBIT_SHIFT      (3)

#define RDA_PMU_LED_R_IBIT(n)         (((n)&0x7)<<6)
#define RDA_PMU_LED_R_IBIT_MASK       (0x7<<6)
#define RDA_PMU_LED_R_IBIT_SHIFT      (6)

#define RDA_PMU_LED_B_OFF_LP          (1<<9)
#define RDA_PMU_LED_G_OFF_LP          (1<<10)
#define RDA_PMU_LED_R_OFF_LP          (1<<11)
#define RDA_PMU_LED_B_OFF_ACT         (1<<12)
#define RDA_PMU_LED_G_OFF_ACT         (1<<13)
#define RDA_PMU_LED_R_OFF_ACT         (1<<14)
#define RDA_PMU_GBIT_ABB_EN           (1<<15)
// end LED Setting5

// Speaker PA setting2 / 0x40
#define RDA_PMU_NCN_EN_R              (1<<0)
#define RDA_PMU_NCN_EN_L              (1<<1)
#define RDA_PMU_NO_DETECT_EN          (1<<2)
#define RDA_PMU_INTERFACE_MODE_EN     (1<<3)

#define RDA_PMU_VCLG_BIT(n)           (((n)&0x3)<<4)
#define RDA_PMU_VCLG_BIT_MASK         (0x3<<4)
#define RDA_PMU_VCLG_BIT_SHIFT        (4)

#define RDA_PMU_VNCN_BIT(n)           (((n)&0x3)<<6)
#define RDA_PMU_VNCN_BIT_MASK         (0x3<<6)
#define RDA_PMU_VNCN_BIT_SHIFT        (6)

#define RDA_PMU_GBIT_R_CLG_REG(n)     (((n)&0x7)<<8)
#define RDA_PMU_GBIT_R_CLG_REG_MASK   (0x7<<8)
#define RDA_PMU_GBIT_R_CLG_REG_SHIFT  (8)

#define RDA_PMU_GBIT_L_CLG_REG(n)     (((n)&0x7)<<11)
#define RDA_PMU_GBIT_L_CLG_REG_MASK   (0x7<<11)
#define RDA_PMU_GBIT_L_CLG_REG_SHIFT  (11)

#define RDA_PMU_I_BIT_CLG_REG(n)      (((n)&0x3)<<14)
#define RDA_PMU_I_BIT_CLG_REG_MASK    (0x3<<14)
#define RDA_PMU_I_BIT_CLG_REG_SHIFT   (14)
// end of speaker PA setting2

// Speaker PA setting3 / 0x41
#define RDA_PMU_SETUP_TMCTRL_BIT(n)         (((n)&0x3)<<0)
#define RDA_PMU_SETUP_TMCTRL_BIT_MASK       (0x3<<0)
#define RDA_PMU_SETUP_TMCTRL_BIT_SHIFT      (0)

#define RDA_PMU_DMODE_EN_CLG(n)             (((n)&0x7)<<2)
#define RDA_PMU_DMODE_EN_CLG_MASK           (0x7<<2)
#define RDA_PMU_DMODE_EN_CLG_SHIFT          (2)
#define RDA_PMU_DMODE_R_CLAB_L_CLAB         RDA_PMU_DMODE_EN_CLG(1)
#define RDA_PMU_DMODE_R_CLAB_L_CLD          RDA_PMU_DMODE_EN_CLG(3)
#define RDA_PMU_DMODE_R_CLD_L_CLAB          RDA_PMU_DMODE_EN_CLG(5)
#define RDA_PMU_DMODE_R_CLD_L_CLD           RDA_PMU_DMODE_EN_CLG(7)

#define RDA_PMU_MONO_L_R_SEL                (1<<5)
#define RDA_PMU_TEST_N_CP                   (1<<6)
#define RDA_PMU_TEST_P_CP                   (1<<7)
#define RDA_PMU_R_ENABLE_CLG                (1<<8)
#define RDA_PMU_L_ENABLE_CLG                (1<<9)
#define RDA_PMU_OTHER_DELEY_NEG             (1<<10)
#define RDA_PMU_OTHER_DELEY_POS             (1<<11)

#define RDA_PMU_RESET_CLG_DELAY_NEG(n)      (((n)&0x3)<<12)
#define RDA_PMU_RESET_CLG_DELAY_NEG_MASK    (0x3<<12)
#define RDA_PMU_RESET_CLG_DELAY_NEG_SHIFT   (12)

#define RDA_PMU_RESET_CLG_DELAY_POS(n)      (((n)&0x3)<<14)
#define RDA_PMU_RESET_CLG_DELAY_POS_MASK    (0x3<<14)
#define RDA_PMU_RESET_CLG_DELAY_POS_SHIFT   (14)
// end of speaker PA setting3

// Speaker PA setting4 / 0x42
#define RDA_PMU_RESET_R_CLG_REG       (1<<0)
#define RDA_PMU_RESET_R_CLG_DR        (1<<1)
#define RDA_PMU_RESET_L_CLG_REG       (1<<2)
#define RDA_PMU_RESET_L_CLG_DR        (1<<3)
#define RDA_PMU_RESET_CLG_REG         (1<<4)
#define RDA_PMU_RESET_CLG_DR          (1<<5)
#define RDA_PMU_PD_O_R_CLG_REG        (1<<6)
#define RDA_PMU_PD_O_R_CLG_DR         (1<<7)
#define RDA_PMU_PD_R_CLG_REG          (1<<8)
#define RDA_PMU_PD_R_CLG_DR           (1<<9)
#define RDA_PMU_PD_O_L_CLG_REG        (1<<10)
#define RDA_PMU_PD_O_L_CLG_DR         (1<<11)
#define RDA_PMU_PD_L_CLG_REG          (1<<12)
#define RDA_PMU_PD_L_CLG_DR           (1<<13)
#define RDA_PMU_PU_CLG_REG            (1<<14)
#define RDA_PMU_PU_CLG_DR             (1<<15)
// end of speaker PA setting4

// TSC and key control / 0x46
#define RDA_PMU_SAR_VREF_MS_EN_GPADC  (1<<0)
#define RDA_PMU_SAR_VREF_MS_EN_TSC    (1<<1)
#define RDA_PMU_PD_SAR_REG            (1<<2)
#define RDA_PMU_PD_SAR_DR             (1<<3)
#define RDA_PMU_TSC_EN_ANA_REG        (1<<4)
#define RDA_PMU_TSC_EN_ANA_DR         (1<<5)
#define RDA_PMU_KEY_EN_REG            (1<<6)
#define RDA_PMU_KEY_EN_DR             (1<<7)
#define RDA_PMU_KEY_INT_EN_REG        (1<<8)
#define RDA_PMU_KEY_INT_EN_DR         (1<<9)
#define RDA_PMU_PU_TSC_Y_REG          (1<<10)
#define RDA_PMU_PU_TSC_Y_DR           (1<<11)
#define RDA_PMU_PU_TSC_X_REG          (1<<12)
#define RDA_PMU_PU_TSC_X_DR           (1<<13)
#define RDA_PMU_PU_TSC_INT_REG        (1<<14)
#define RDA_PMU_PU_TSC_INT_DR         (1<<15)
// end of TSC and key control





#define RDA_PMU_VLDO_8955_BIT_NLP(n)        (((n)&0xf)<<8)
#define RDA_PMU_VLDO_8955_BIT_NLP_MASK      (0xf<<8)
#define RDA_PMU_VLDO_8955_BIT_NLP_SHIFT     (8)

// 48H[2] Speak CLK enable
#define RDA_PMU_8955_SPK_CLK_BIT(n)        (0x1<<2)
#define RDA_PMU_8955_SPK_CLK_BIT_MASK      (0x1<<2)
#define RDA_PMU_8955_SPK_CLK_BIT_SHIFT     (2)
///
//  0x4b


#define RDA_PMU_VBUCK_VCORE_LDO_BIT(n)         (((n)&0xf)<<8)
#define RDA_PMU_VBUCK_VCORE_LDO_BIT_MASK       (0xf<<8)
#define RDA_PMU_VBUCK_VCORE_LDO_BIT_SHIFT      (8)

#define RDA_PMU_VBUCK_VCORE_LDO_LP_BIT(n)         (((n)&0xf)<<12)
#define RDA_PMU_VBUCK_VCORE_LDO_LP_BIT_MASK       (0xf<<12)
#define RDA_PMU_VBUCK_VCORE_LDO_LP_BIT_SHIFT      (12)
//pmu_8809_47H
//#define PMU_WRITE_REQ            (1<<31)
//#define PMU_READ_REQ             (1<<30)
//#define PMU_RESERVED(n)          (((n)&3)<<14)
#define PMU_NAND_WP_OUT_DR         (1<<13)
#define PMU_NAND_WP_EN_REG         (1<<12)
#define PMU_NAND_WP_VSEL(n)        (((n)&3)<<10)
#define PMU_TIMER_RST_EXT_SEL(n)   (((n)&3)<<8)
#define PMU_VUSB_PON_TIME_SEL(n)   (((n)&3)<<6)
#define PMU_VUSB_POFF_TIME_SEL(n)  (((n)&3)<<4)
//#define PMU_RESERVED(n)          (((n)&3)<<2)
#define PMU_PA_EN_DR               (1<<1)
#define PMU_PA_EN_REG              (1<<0)

//pmu_8809_48H
//#define PMU_WRITE_REQ            (1<<31)
//#define PMU_READ_REQ             (1<<30)
#define PMU_NAND_WP_OUT_B_REG      (1<<15)
//#define PMU_RESERVED             (1<<14)
#define PMU_CNT_EN_WITHOUT_BATTERY (1<<13)
#define PMU_VPAD_VSEL_EFUSE        (1<<12)
#define PMU_VA24_VSEL              (1<<11)
#define PMU_VA24_IBIT(n)           (((n)&7)<<8)
#define PMU_VA24_IBIT_MASK       (0x7<<8)

#define PMU_HV_MODE_BL             (1<<7)

#define PMU_IX2_BL                 (1<<6)
#define PMU_IDIV10_LED             (1<<5)
#define PMU_IDIV40_RGB             (1<<4)
#define PMU_DOUBLE_MODE_EN_CLG     (1<<3)
#define PMU_CLK_CLG_EN             (1<<2)
#define PMU_VH_BIT_CLG(n)          (((n)&3)<<0)

//pmu_8809_49H
//#define PMU_WRITE_REQ            (1<<31)
//#define PMU_READ_REQ             (1<<30)
#define PMU_CNT_OUT(n)             (((n)&0xFFFF)<<0)

//pmu_8809_4aH
//#define PMU_WRITE_REQ            (1<<31)
//#define PMU_READ_REQ             (1<<30)
#define PMU_VL_BIT_CLG(n)          (((n)&3)<<14)
#define PMU_FREQ_CLK_CLG_SEL(n)    (((n)&3)<<12)
#define PMU_PULLDOWN_VIB           (1<<11)
#define PMU_VBUCK2_PON_TIME_SEL(n) (((n)&3)<<9)
#define PMU_VBUCK2_POFF_TIME_SEL(n) (((n)&3)<<7)
#define PMU_BL_DIM_DELAY_CLK_SEL(n) (((n)&3)<<5)
//#define PMU_RESERVED(n)          (((n)&15)<<1)
#define PMU_FLAG_RESETN            (1<<0)

//pmu_8809_4bH
//#define PMU_WRITE_REQ            (1<<31)
//#define PMU_READ_REQ             (1<<30)
#define PMU_VBUCK1_LDO_BIT_LP(n)   (((n)&15)<<12)
#define PMU_VBUCK1_LDO_BIT_NLP(n)  (((n)&15)<<8)
//#define PMU_RESERVED(n)          (((n)&0xFF)<<0)

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================

/// to extract bitfield from register value
#define PMU_GET_BITFIELD(dword, bitfield) (((dword) & (bitfield ## _MASK)) >> (bitfield ## _SHIFT))
#define PMU_SET_BITFIELD(dword, bitfield, value) (((dword) & ~(bitfield ## _MASK)) | (bitfield(value)))

/// to build commands for pmd_OpalSpiSendCommand
#define PMU_SPI_BUILD_WRITE_COMMAND(cmd,regIdx,val) \
{ (*cmd) = (0<<25) | ((regIdx & 0x1ff) << 16) | (val & 0xffff); }

#define PMU_SPI_BUILD_READ_COMMAND(cmd,regIdx) \
{ (*cmd) = (1<<25) | ((regIdx & 0x1ff) << 16) | (val & 0xffff); }

#define PMU_SPI_EXTRACT_DATA(data) ((UINT16) (((data)[1] << 8) | (data)[2]))

// =============================================================================
//  FUNCTIONS
// =============================================================================

//=============================================================================
// pmd_OpalWrite
//-----------------------------------------------------------------------------
/// Read Opal register
/// @param regIdx register to read
/// @param value register value
///
/// Note: this function completely handles the SPI control
//=============================================================================
PUBLIC VOID pmd_RDAWrite(RDA_REG_MAP_T regIdx, UINT16 data);

//=============================================================================
// pmd_OpalRead
//-----------------------------------------------------------------------------
/// Read Opal register
/// @param regIdx register to read
/// @return register value
///
/// Note: this function completely handles the SPI control
//=============================================================================
PUBLIC UINT16 pmd_RDARead(RDA_REG_MAP_T regIdx);


// =============================================================================
//  Special API with lower level SPI handling (for multiple register access)
// =============================================================================

//=============================================================================
// pmd_OpalSpiActivateCs
//-----------------------------------------------------------------------------
/// Activate Opal SPI. refer to hal_SpiActivateCs() for details.
/// @return \c TRUE if the cs has been successfully activated
/// \c FALSE when another cs is already active
// =============================================================================
PUBLIC BOOL pmd_OpalSpiActivateCs(VOID);

//=============================================================================
// pmd_OpalSpiDeactivateCs
//-----------------------------------------------------------------------------
/// Deactivate Opal SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PUBLIC VOID pmd_OpalSpiDeactivateCs(VOID);

//=============================================================================
// pmd_OpalSpiSendCommand
//-----------------------------------------------------------------------------
/// Send a burst of commands to Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results ensure the command has been effectively send).
/// @param cmd array of bytes containing the command, use #PMD_OPAL_SPI_BUILD_WRITE_COMMAND and #PMD_OPAL_SPI_BUILD_READ_COMMAND.
/// @param length number of SPI command to send (cmd array must by 3 time that)
//=============================================================================
PUBLIC VOID pmd_OpalSpiSendCommand(CONST UINT32* cmd, UINT32 length);

//=============================================================================
// pmd_OpalSpiGetData
//-----------------------------------------------------------------------------
/// Read a burst of data from Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results before sending too much new commands).
/// @param data array of bytes to receive the data, use #PMD_OPAL_SPI_EXTRACT_DATA
/// @param length number of SPI command recieve (data array must by 3 time that)
//=============================================================================
PUBLIC VOID pmd_OpalSpiGetData(UINT32* data, UINT32 length);


PUBLIC BOOL pmd_SpiCsNonblockingActivate(BOOL singleWrite);
PUBLIC VOID pmd_OpalCsDeactivate(BOOL singleWrite);
PUBLIC UINT16 pmd_OpalSpiRead(RDA_REG_MAP_T regIdx);
PUBLIC VOID pmd_OpalSpiWrite(RDA_REG_MAP_T regIdx, UINT16 value);
// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingSet(UINT32 flags);
// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingClr(UINT32 flags);


VOID pmd_InitRdaPmu(VOID);


#endif

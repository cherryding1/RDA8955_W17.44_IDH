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



#ifndef _HALP_ABB_REGS_H_
#define _HALP_ABB_REGS_H_

#include "cs_types.h"
#include "chip_id.h"
// ----------------
// Registers
// ----------------

typedef enum
{
    CODEC_SIM_INTERFACE         = 0x01,
#ifdef CHIP_DIE_8809E
    CODEC_SETITING_1            = 0x02,
    CODEC_SETITING_3            = 0x03,
#endif
    USB_CONTROL                 = 0x04,
    CODEC_MIC_SETTING           = 0x05,
    CODEC_LINEIN_SETTING        = 0x08,
    CODEC_LDO_SETTING1          = 0x09,
    CODEC_LDO_SETTING2          = 0x0a,
    CODEC_MISC_SETTING          = 0x0b,
    CODEC_MODE_SEL              = 0x0c,
    CODEC_CALIB_SETTING         = 0x0d,
    CODEC_POWER_CTRL            = 0x0e,
    CODEC_SOFT_RESET            = 0x0f,
    CODEC_CLOCK_CLASS_K_PA      = 0x10,
    CODEC_CLOCK_CODEC           = 0x11,
    CODEC_HP_DETECT_12H         = 0x12,
    CODEC_PLL_13H               = 0x13,
    CODEC_PLL_14H               = 0x14,
    CODEC_SDM2_FREQ_HIGH        = 0x15,
    CODEC_SDM2_FREQ_LOW         = 0x16,
    CODEC_SDM2_DITHER           = 0x17,
    CODEC_RESET_CTRL            = 0x1f,
#ifdef CHIP_DIE_8809E
    CODEC_GAINRAMP_CTRL         = 0x127,
    CODEC_FM_MODE               = 0x128,
    CODEC_DIG_EN                = 0x129,
    CODEC_DIG_DAC_GAIN          = 0x12a,
    CODEC_DIG_MIC_GAIN          = 0x12b,
    CODEC_DIG_FREQ_SAMPLE_SEL   = 0x12c,
    CODEC_DIG_FREQ_SAMPLE_DIV   = 0x12d,

    CODEC_HP_DETECT_33H         = 0x133,
    CODEC_HP_DETECT_3CH         = 0x13c,
    CODEC_HP_DETECT_3DH         = 0x13d,
    CODEC_HP_DETECT_3EH         = 0x13e,
    CODEC_HP_DETECT_3FH         = 0x13f,
    CODEC_HP_DETECT_40H         = 0x110,
    CODEC_HP_DETECT_41H         = 0x111,
    CODEC_HP_DETECT_42H         = 0x112,
    CODEC_HP_DETECT_43H         = 0x113,
    CODEC_HP_DETECT_44H         = 0x114,
    CODEC_HP_DETECT_45H         = 0x115,
    CODEC_HP_DETECT_46H         = 0x116,
    CODEC_HP_DETECT_47H         = 0x117,
    CODEC_HP_DETECT_48H         = 0x118,
#else
    CODEC_GAINRAMP_CTRL         = 0x27,
    CODEC_FM_MODE               = 0x28,
    CODEC_DIG_EN                = 0x29,
    CODEC_DIG_DAC_GAIN          = 0x2a,
    CODEC_DIG_MIC_GAIN          = 0x2b,
    CODEC_DIG_FREQ_SAMPLE_SEL   = 0x2c,
    CODEC_DIG_FREQ_SAMPLE_DIV   = 0x2d,
    CODEC_HP_DETECT_3CH         = 0x3c,
    CODEC_HP_DETECT_3DH         = 0x3d,
    CODEC_HP_DETECT_3EH         = 0x3e,
    CODEC_HP_DETECT_3FH         = 0x3f,
    CODEC_HP_DETECT_40H         = 0x40,
    CODEC_HP_DETECT_41H         = 0x41,
    CODEC_HP_DETECT_42H         = 0x42,
    CODEC_HP_DETECT_43H         = 0x43,
    CODEC_HP_DETECT_44H         = 0x44,
    CODEC_HP_DETECT_45H         = 0x45,
    CODEC_HP_DETECT_46H         = 0x46,
    CODEC_HP_DETECT_47H         = 0x47,
#endif
} ABB_REG_INDEX_T;


#define CODEC_DIG_MIN_REG_INDEX (0x20)


// -------------------
// Register bit fields
// -------------------
#ifdef CHIP_DIE_8809E

#define ABB_AU_RCV_SEL_REG         (1<<1)
#endif
// CODEC_SIM_INTERFACE = 0x01
#define ABB_SIM_SELECT(n)              (((n)&0x3)<<14)
#define ABB_SIM_SELECT_MASK            (3<<14)
#define ABB_SIM_SELECT_SHIFT           (14)
#define ABB_SIM_SELECT_SIM1            (0<<14)
#define ABB_SIM_SELECT_SIM2            (1<<14)
#define ABB_SIM_SELECT_SIM3            (2<<14)
#define ABB_SIM_SELECT_SIM4            (3<<14)
#define ABB_PULLDOWN_ENABLE_1          (1<<11)
#define ABB_UNSEL_CLK_VAL_1            (1<<10)
#define ABB_UNSEL_RST_VAL_1            (1<<9)
#define ABB_PULLDOWN_ENABLE_2          (1<<8)
#define ABB_UNSEL_CLK_VAL_2            (1<<7)
#define ABB_UNSEL_RST_VAL_2            (1<<6)
#define ABB_PULLDOWN_ENABLE_3          (1<<5)
#define ABB_UNSEL_CLK_VAL_3            (1<<4)
#define ABB_UNSEL_RST_VAL_3            (1<<3)
#define ABB_PULLDOWN_ENABLE_4          (1<<2)
#define ABB_UNSEL_CLK_VAL_4            (1<<1)
#define ABB_UNSEL_RST_VAL_4            (1<<0)

// USB_CONTROL = 0x04
#define ABB_SSI_VREFN_RBIT(n)      (((n)&0x3)<<14)
#define ABB_SSI_VREFN_RBIT_MASK    (0x3<<14)
#define ABB_SSI_VREFN_RBIT_SHIFT   (14)
#define ABB_SSI_VREFP_RBIT(n)      (((n)&0x3)<<12)
#define ABB_SSI_VREFP_RBIT_MASK    (0x3<<12)
#define ABB_SSI_VREFP_RBIT_SHIFT   (12)
#define ABB_PADET_IBIT(n)          (((n)&0x7)<<9)
#define ABB_PADET_IBIT_MASK        (0x7<<9)
#define ABB_PADET_IBIT_SHIFT       (9)
#define ABB_USB_CTRL(n)         (((n)&0x7)<<4)
#define ABB_USB_CTRL_MASK       (0x7<<4)
#define ABB_USB_CTRL_SHIFT      (4)
#define ABB_USB_VREG_BIT(n)     (((n)&0x7)<<1)
#define ABB_USB_VREG_BIT_MASK   (0x7<<1)
#define ABB_USB_VREG_BIT_SHIFT  (1)
#define ABB_PD_USB              (1<<0)

// CODEC_MIC_SETTING = 0x05
#define ABB_MB_SEL_BIT(n)       (((n)&0x3)<<14)
#define ABB_MB_SEL_BIT_MASK     (0x3<<14)
#define ABB_MB_SEL_BIT_SHIFT    (14)
#define ABB_MIC_CHOP_EN         (1<<13)
#define ABB_MIC_HPF_MODE_EN     (1<<12)
#define ABB_MICCAP_MODE_EN      (1<<11)
#define ABB_TEST_EN_ADC         (1<<10)
#define ABB_MASH_EN_ADC         (1<<9)
#define ABB_MICIN_IBPGA_EN_DR   (1<<5)
#define ABB_MICIN_IBPGA_EN_REG  (1<<4)
#define ABB_MUXIN_LEFT_EN_DR    (1<<3)
#define ABB_MUXIN_LEFT_EN_REG   (1<<2)
#define ABB_MUXIN_RIGHT_EN_DR   (1<<1)
#define ABB_MUXIN_RIGHT_EN_REG  (1<<0)

// CODEC_PLL_SETTING1 = 0x06
#define ABB_MIC_STD_EN_DR       (1<<15)
#define ABB_MIC_STD_EN_REG      (1<<14)
#define ABB_MIC_AUX_EN_DR       (1<<13)
#define ABB_MIC_AUX_EN_REG      (1<<12)
#define ABB_VBIT_NEG1(n)           (((n)&0x3)<<9)
#define ABB_VBIT_NEG1_MASK         (0x3<<9)
#define ABB_VBIT_NEG1_SHIFT        (9)
#define ABB_CLK_SEL_BIT_NEG1(n)    (((n)&0x3)<<7)
#define ABB_CLK_SEL_BIT_NEG1_MASK  (0x3<<7)
#define ABB_CLK_SEL_BIT_NEG1_SHIFT (7)
#define ABB_HP_CAP_MODE_EN      (1<<6)
#define ABB_PD_HP_CAP_DR        (1<<5)
#define ABB_PD_HP_CAP_REG       (1<<4)
#define ABB_PD_NEG1_DR          (1<<3)
#define ABB_PD_NEG1_REG         (1<<2)
#define ABB_BOOST_BP_MIC_DR     (1<<1)
#define ABB_BOOST_BP_MIC_REG    (1<<0)

// CODEC_LINEIN_SETTING = 0x08
#define ABB_HP_RCV_MUTE_ENABLE  (1<<15)
#define ABB_ADC_PD_SEL_ADC      (1<<3)
#define ABB_MIC_TO_PA_MODE      (1<<2)
#define ABB_LINEIN_TO_MIC_MODE  (1<<1)
#define ABB_LINEIN_TO_PA_MODE   (1<<0)

// CODEC_LDO_SETTING1 = 0x09
#define ABB_PD_ALL_DR           (1<<15)
#define ABB_PD_ALL_REG          (1<<14)
#define ABB_PD_ADC_DR           (1<<13)
#define ABB_PD_ADC_REG          (1<<12)
#define ABB_PD_MICBIAS_DR       (1<<11)
#define ABB_PD_MICBIAS_REG      (1<<10)
#define ABB_PD_DAC_L_DR         (1<<9)
#define ABB_PD_DAC_L_REG        (1<<8)
#define ABB_PD_DAC_R_DR         (1<<7)
#define ABB_PD_DAC_R_REG        (1<<6)

#ifdef CHIP_DIE_8809E

#define ABB_PA_ENB_RECI_DR      (0<<5)
#define ABB_PA_ENB_RECI_REG     (0<<4)
#define ABB_PA_EN_RECI_DR      (1<<5)
#define ABB_PA_EN_RECI_REG     (1<<4)
#else
#define ABB_PA_ENB_RECI_DR      (1<<5)
#define ABB_PA_ENB_RECI_REG     (1<<4)
#endif

#define ABB_PA_ENB_HP_DR        (1<<3)
#define ABB_PA_ENB_HP_REG       (1<<2)
#define ABB_RESET_MUX_EN_DR     (1<<3)
#define ABB_RESET_MUX_EN_REG    (1<<2)

// CODEC_LDO_SETTING2 = 0x0a
#define ABB_VCOM_MODE(n)        (((n)&0x7)<<13)
#define ABB_VCOM_MODE_MASK      (0x7<<13)
#define ABB_VCOM_MODE_SHIFT     (13)
#define ABB_VREF_MODE(n)        (((n)&0x7)<<10)
#define ABB_VREF_MODE_MASK      (0x7<<10)
#define ABB_VREF_MODE_SHIFT     (10)
#define ABB_ISEL_BIT_B(n)       (((n)&0x3)<<8)
#define ABB_ISEL_BIT_B_MASK     (0x3<<8)
#define ABB_ISEL_BIT_B_SHIFT    (8)
#define ABB_SEL_BG_CODEC        (1<<7)
#define ABB_RESET_DAC_DR        (1<<6)
#define ABB_RESET_DAC_REG(n)    (((n)&0x3)<<4)
#define ABB_RESET_DAC_REG_MASK  (0x3<<4)
#define ABB_RESET_DAC_REG_SHIFT (4)
#define ABB_CLASSG_ENABLE_DR    (1<<3)
#define ABB_CLASSG_ENABLE_REG   (1<<2)

// CODEC_MISC_SETTING = 0x0b
#define ABB_HP_VCOM_SEL         (1<<15)
#define ABB_SYN_EN_DAC          (1<<0)

// CODEC_MODE_SEL = 0x0c
#define ABB_RECI_DAC_EN_DR      (1<<15)
#define ABB_RECI_DAC_EN_REG     (1<<14)
#define ABB_RECI_LINEIN_EN_DR   (1<<13)
#define ABB_RECI_LINEIN_EN_REG  (1<<12)
#define ABB_DAC_MODE_L_EN_DR    (1<<11)
#define ABB_DAC_MODE_L_EN_REG   (1<<10)
#define ABB_DAC_MODE_R_EN_DR    (1<<9)
#define ABB_DAC_MODE_R_EN_REG   (1<<8)
#define ABB_MICDAC_MODE_EN_DR   (1<<7)
#define ABB_MICDAC_MODE_EN_REG  (1<<6)
#define ABB_LINE_MODE_EN_DR     (1<<5)
#define ABB_LINE_MODE_EN_REG    (1<<4)
#define ABB_HP_AUX_EN           (1<<3)
#define ABB_SPK_MIC_EN          (1<<2)
//#define ABB_RESERVED          (1<<1)
//#define ABB_RESERVED          (1<<0)
#define ABB_MIC_MODE_EN         (1<<15)

// CODEC_POWER_CTRL = 0x0e
#define ABB_RESET_MIC_DR        (1<<15)
#define ABB_RESET_MIC_REG       (1<<14)
#define ABB_RESET_EN_AD_DR      (1<<13)
#define ABB_RESET_EN_AD_REG     (1<<12)
#define ABB_RESET_PA_RECI_DR    (1<<11)
#define ABB_RESET_PA_RECI_REG   (1<<10)
#define ABB_RESET_PA_HP_DR      (1<<9)
#define ABB_RESET_PA_HP_REG     (1<<8)
#define ABB_RESET_PA_SPK_DR     (1<<7)
#define ABB_RESET_PA_SPK_REG    (1<<6)
#define ABB_RSTN_CODEC_DR       (1<<5)
#define ABB_RSTN_CODEC          (1<<4)
#define ABB_ADC_EN_CODEC_DR     (1<<3)
#define ABB_ADC_EN_CODEC        (1<<2)
#define ABB_DAC_EN_CODEC_DR     (1<<1)
#define ABB_DAC_EN_CODEC        (1<<0)

// CODEC_SOFT_RESET = 0x0f
#define ABB_REG_RESET_SET      (0<<1)
#define ABB_REG_RESET_CLR      (1<<1)
#define ABB_SOFT_RESET_SET     (0<<0)
#define ABB_SOFT_RESET_CLR     (1<<0)

// CODEC_CLOCK_CLASS_K_PA = 0x10
#define ABB_DCDC4_RESETN_TIME_SEL(n)        (((n)&0x3)<<11)
#define ABB_DCDC4_RESETN_TIME_SEL_MASK      (0x3<<11)
#define ABB_DCDC4_RESETN_TIME_SEL_SHIFT     (11)

#define ABB_DCDC4_CLKGEN4_CLK_SEL           (1<<10)

#define ABB_DCDC4_CLKGEN4_DIV_BASE(n)       (((n)&0xff)<<2)
#define ABB_DCDC4_CLKGEN4_DIV_BASE_MASK     (0xff<<2)
#define ABB_DCDC4_CLKGEN4_DIV_BASE_SHIFT    (2)

#define ABB_DCDC4_CLKGEN4_RESETN_DR         (1<<1)
#define ABB_DCDC4_CLKGEN4_RESETN_REG        (1<<0)

// CODEC_CLOCK_CODEC = 0x11
#define ABB_DSSI_INV(n)                     (((n)&0x3)<<14)
#define ABB_DSSI_INV_MASK                   (0x3<<14)
#define ABB_DSSI_INV_SHIFT                  (14)

#define ABB_DCDC_CLKGEN3_DITHER_CT(n)       (((n)&0x7)<<10)
#define ABB_DCDC_CLKGEN3_DITHER_CT_MASK     (0x7<<10)
#define ABB_DCDC_CLKGEN3_DITHER_CT_SHIFT    (10)

#define ABB_DCDC_CLKGEN3_DIV_BASE(n)        (((n)&0xff)<<2)
#define ABB_DCDC_CLKGEN3_DIV_BASE_MASK      (0xff<<2)
#define ABB_DCDC_CLKGEN3_DIV_BASE_SHIFT     (2)
#define ABB_DCDC_CLKGEN3_DIV_BASE_FOR_FM_RECORD ABB_DCDC_CLKGEN3_DIV_BASE(23)

#define ABB_DCDC_CLKGEN3_RESETN_DR          (1<<1)
#define ABB_DCDC_CLKGEN3_RESETN_REG         (1<<0)

// CODEC_HP_DETECT_12H = 0x12
#define ABB_PD_SSI                          (1<<15)
#define ABB_DSSI_ANA_ENABLE                 (1<<14)

#define ABB_DSSI_STABLE_TIME_SEL(n)         (((n)&0x3)<<12)
#define ABB_DSSI_STABLE_TIME_SEL_MASK       (0x3<<12)
#define ABB_DSSI_STABLE_TIME_SEL_SHIFT      (12)

#define ABB_DSSI_LG_EN_TIME_SEL(n)          (((n)&0x3)<<10)
#define ABB_DSSI_LG_EN_TIME_SEL_MASK        (0x3<<10)
#define ABB_DSSI_LG_EN_TIME_SEL_SHIFT       (10)

#define ABB_SPK_GAIN_SEL_INV                (1<<9)
#define ABB_SPK_LOWGAIN_EN_L_DR             (1<<8)
#define ABB_SPK_LOWGAIN_EN_L_REG            (1<<7)
#define ABB_SPK_LOWGAIN_EN_R_DR             (1<<6)
#define ABB_SPK_LOWGAIN_EN_R_REG            (1<<5)
#define ABB_PADET_EN                        (1<<4)

#define ABB_RESET_PADET_TIME_SEL(n)         (((n)&0x3)<<2)
#define ABB_RESET_PADET_TIME_SEL_MASK       (0x3<<2)
#define ABB_RESET_PADET_TIME_SEL_SHIFT      (2)

#define ABB_RESET_PADET_DR                  (1<<1)
#define ABB_RESET_PADET_REG                 (1<<0)

// CODEC_PLL_13H = 0x13
#define ABB_PLL_AUX_CLKOUT_EN(n)        (((n)&0xf)<<12)
#define ABB_PLL_AUX_CLKOUT_EN_MASK      (0xf<<12)
#define ABB_PLL_AUX_CLKOUT_EN_SHIFT     (12)

#define ABB_PLL_AUX_CPAUX_BIT(n)        (((n)&0x7)<<9)
#define ABB_PLL_AUX_CPAUX_BIT_MASK      (0x7<<9)
#define ABB_PLL_AUX_CPAUX_BIT_SHIFT     (9)

#define ABB_PLL_AUX_FILTER_IBIT(n)      (((n)&0x7)<<6)
#define ABB_PLL_AUX_FILTER_IBIT_MASK    (0x7<<6)
#define ABB_PLL_AUX_FILTER_IBIT_SHIFT   (6)

#define ABB_PLL_AUX_CP_BIT(n)           (((n)&0x7)<<3)
#define ABB_PLL_AUX_CP_BIT_MASK         (0x7<<3)
#define ABB_PLL_AUX_CP_BIT_SHIFT        (3)

#define ABB_PLL_AUX_INT_MODE            (1<<2)
#define ABB_PLL_AUX_SDM_CLK_SEL_RESET   (1<<1)
#define ABB_PLL_AUX_SDM_CLK_SEL_NOR     (1<<0)

// CODEC_PLL_14H = 0x14
#define ABB_PLL_AUX_SDM_CLK_TEST_EN     (1<<14)
#define ABB_PLL_AUX_FEFMUITI2           (1<<13)
#define ABB_PLL_AUX_VCO_HIGH_TEST       (1<<12)
#define ABB_PLL_AUX_VCO_LOW_TEST        (1<<11)
#define ABB_PLL_AUX_TEST_EN             (1<<10)

#define ABB_PLL_AUX_VREG_BIT(n)         (((n)&0xf)<<6)
#define ABB_PLL_AUX_VREG_BIT_MASK       (0xf<<6)
#define ABB_PLL_AUX_VREG_BIT_SHIFT      (6)

#define ABB_PLL_AUX_PD_DR               (1<<5)
#define ABB_PLL_AUX_PD_REG              (1<<4)
#define ABB_PLL_AUX_RESET_DR            (1<<3)
#define ABB_PLL_AUX_RESET_REG           (1<<2)

// CODEC_SDM2_DITHER = 0x17
#define ABB_PLL_SDM2_INT_DEC_SEL(n)     (((n)&0x7)<<13)
#define ABB_PLL_SDM2_INT_DEC_SEL_MASK   (0x7<<13)
#define ABB_PLL_SDM2_INT_DEC_SEL_SHIFT  (13)

#define ABB_PLL_SDM2_DITHER_BYPASS      (1<<12)
#define ABB_PLL_SDM2_SS_EN              (1<<11)
#define ABB_PLL_SDM2_SS_SQURE_TRI_SEL   (1<<10)
#define ABB_PLL_SDM2_DCDC4_312M_AUX_34  (1<<6)
#define ABB_PLL_SDM2_DCDC4_624M_AUX_34  (0<<6)
#define ABB_PLL_SDM2_DCDC3_312M_AUX_34  (1<<5)
#define ABB_PLL_SDM2_DCDC3_624M_AUX_34  (0<<5)
#define ABB_PLL_SDM2_DCDC2_312M_12      (1<<4)
#define ABB_PLL_SDM2_DCDC2_624M_12      (0<<4)
#define ABB_PLL_SDM2_DCDC1_312M_12      (1<<3)
#define ABB_PLL_SDM2_DCDC1_624M_12      (0<<3)
#define ABB_PLL_SDM2_CLK_FBC_INV        (1<<2)
#define ABB_PLL_SDM2_RESETN_DR          (1<<1)
#define ABB_PLL_SDM2_RESETN_REG         (1<<0)

// CODEC_RESET_CTRL = 0x1f
#define ABB_SPI_OUT_ABB         (0<<0)
#define ABB_SPI_OUT_CODEC       (1<<0)
#define ABB_SPI_OUT_MASK        (1<<0)

// CODEC_FM_MODE = 0x28
#define ABB_CNT_ADC_CLK(n)      (((n)&0x7f)<<6)
#define ABB_CNT_ADC_CLK_MASK    (0x7f<<6)
#define ABB_CNT_ADC_CLK_SHIFT   (6)
#define ABB_CNT_ADC_CLK_FOR_FM_RECORD ABB_CNT_ADC_CLK(5)

#define ABB_S_ADC_OSR_SEL(n)    (((n)&0x3)<<4)
#define ABB_S_ADC_OSR_SEL_MASK  (0x3<<4)
#define ABB_S_ADC_OSR_SEL_SHIFT (4)
#define ABB_S_ADC_OSR_SEL_8M    ABB_S_ADC_OSR_SEL(3)
#define ABB_S_ADC_OSR_SEL_4M    ABB_S_ADC_OSR_SEL(2)
#define ABB_S_ADC_OSR_SEL_2M    ABB_S_ADC_OSR_SEL(1)
#define ABB_S_ADC_OSR_SEL_1M    ABB_S_ADC_OSR_SEL(0)

#define ABB_CLK_FM_INV          (1<<1)
#define ABB_FM_MODE             (1<<0)

// CODEC_DIG_EN = 0x29
#define ABB_DIG_S_DWA_EN        (1<<11)
#define ABB_DIG_MASH_EN_ADC     (1<<9)
#define ABB_DIG_PADET_CLK_INV   (1<<5)
#define ABB_DIG_S_ISPHONE       (1<<3)
#define ABB_DIG_S_CODEC_EN      (1<<2)
#define ABB_DIG_DAC_CLK_INVERT   (1<<7)
#define ABB_DIG_ADC_CLK_INVERT   (1<<6)

// CODEC_DIG_DAC_GAIN = 0x2a
#define ABB_DIG_SIDE_TONE_GAIN_L(n)     (((n)&0xf)<<12)
#define ABB_DIG_SIDE_TONE_GAIN_L_MASK   (0xf<<12)
#define ABB_DIG_SIDE_TONE_GAIN_L_SHIFT  (12)
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#define ABB_DIG_DITHER_GAIN_H(n)        (((n)&0xf)<<8)
#define ABB_DIG_DITHER_GAIN_H_MASK      (0xf<<8)
#define ABB_DIG_DITHER_GAIN_H_SHIFT     (8)
#else // 8809 or later
#define ABB_DIG_SDM_GAIN(n)             (((n)&0x3)<<10)
#define ABB_DIG_SDM_GAIN_MASK           (0x3<<10)
#define ABB_DIG_SDM_GAIN_SHIFT          (10)
#define ABB_DIG_DITHER_GAIN_H(n)        (((n)&0x3)<<8)
#define ABB_DIG_DITHER_GAIN_H_MASK      (0x3<<8)
#define ABB_DIG_DITHER_GAIN_H_SHIFT     (8)
#endif // 8809 or later
#define ABB_DIG_DAC_VOLUME_MUTE         (1<<7)
#define ABB_DIG_DAC_VOLUME(n)           (((n)&0x1f)<<2)
#define ABB_DIG_DAC_VOLUME_MASK         (0x1f<<2)
#define ABB_DIG_DAC_VOLUME_SHIFT        (2)

// CODEC_DIG_MIC_GAIN = 0x2b
#define ABB_DIG_S_DAC1K            (1<<8)

#define ABB_DIG_DITHER_GAIN_L(n)        (((n)&7)<<13)
#define ABB_DIG_DITHER_GAIN_L_MASK      (7<<13)
#define ABB_DIG_DITHER_GAIN_L_SHIFT     (13)
#define ABB_DIG_MIC_VOLUME(n)           (((n)&0xf)<<9)
#define ABB_DIG_MIC_VOLUME_MASK         (0xf<<9)
#define ABB_DIG_MIC_VOLUME_SHIFT        (9)
#define ABB_DIG_SIDE_TONE_GAIN_H(n)     (((n)&1)<<0)
#define ABB_DIG_SIDE_TONE_GAIN_H_MASK   (1<<0)
#define ABB_DIG_SIDE_TONE_GAIN_H_SHIFT  (0)

// CODEC_DIG_FREQ_SAMPLE_SEL = 0x2c
#define ABB_DIG_RESET_DAC_PATH          (1<<5)
#define ABB_DIG_RESET_ADC_PATH          (1<<6)
#define ABB_DIG_DAC_OSR_SEL(n)          (((n)&3)<<7)
#define ABB_DIG_DAC_OSR_SEL_MASK        (3<<7)
#define ABB_DIG_DAC_OSR_SEL_SHIFT       (7)
#define ABB_DIG_DAC_OSR_SEL_512         ABB_DIG_DAC_OSR_SEL(2)
#define ABB_DIG_DAC_OSR_SEL_256         ABB_DIG_DAC_OSR_SEL(1)
#define ABB_DIG_DAC_OSR_SEL_128         ABB_DIG_DAC_OSR_SEL(0)

// CODEC_HP_DETECT_3DH = 0x3d
#ifdef CHIP_DIE_8809E
#define ABB_DAC_HPF_MODE              (1<<14)
#define ABB_TONE20_GEN_ENABLE            (1<<15)
#else

#define ABB_SPK_GAIN_SEL_CTRL(n)        (((n)&0x3)<<14)
#define ABB_SPK_GAIN_SEL_CTRL_MASK      (0x3<<14)
#define ABB_SPK_GAIN_SEL_CTRL_SHIFT     (14)
#endif

#define ABB_NOTCH20_BYPASS              (1<<13)
#define ABB_EARPHONE_DET_BYPASS         (1<<12)
#define ABB_EARPHONE_DET_SEL            (1<<11)

#define ABB_EARPHONE_DET_TH(n)          (((n)&0x7f)<<4)
#define ABB_EARPHONE_DET_TH_MASK        (0x7f<<4)
#define ABB_EARPHONE_DET_TH_SHIFT       (4)

#define ABB_EARPHONE_DET_COEF_H(n)      (((n)&0xf)<<0)
#define ABB_EARPHONE_DET_COEF_H_MASK    (0xf<<0)
#define ABB_EARPHONE_DET_COEF_H_SHIFT   (0)

#endif // _HALP_ABB_REGS_H_


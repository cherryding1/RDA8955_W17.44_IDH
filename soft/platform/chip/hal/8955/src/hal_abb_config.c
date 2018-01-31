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
#include "cfg_regs.h"

#include "halp_abb_regs.h"
#include "halp_rda_abb.h"
#include "halp_debug.h"
#include "halp_sys.h"
#include "hal_sys.h"
#include "hal_rda_abb.h"
#include "hal_rda_audio.h"

#include "aud_m.h"
#include "sxr_tls.h"


// =============================================================================
// EXTERNS
// =============================================================================

extern VOID pmd_SpeakerPaCpLowOutput(BOOL low);
#ifdef ABB_HP_DETECT
extern VOID pmd_StartHpOutDetect(BOOL start);
#endif


// =============================================================================
//  MACROS
// =============================================================================

// Whether to enable DSSI to improve sound quality during silence
// 0: disable DSSI all the time
// 1: only enable DSSI in music mode
// 2: enable DSSI both voice and music mode
#define AUD_SPK_DSSI_DETECT 0


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_AUD_SAMPLE_RATE_INDEX_T
// -----------------------------------------------------------------------------
/// This type list the index of supported sample rates.
// =============================================================================
typedef enum
{
    HAL_AUD_SAMPLE_RATE_INDEX_8K,
    HAL_AUD_SAMPLE_RATE_INDEX_11_025K,
    HAL_AUD_SAMPLE_RATE_INDEX_12K,
    HAL_AUD_SAMPLE_RATE_INDEX_16K,
    HAL_AUD_SAMPLE_RATE_INDEX_22_05K,
    HAL_AUD_SAMPLE_RATE_INDEX_24K,
    HAL_AUD_SAMPLE_RATE_INDEX_32K,
    HAL_AUD_SAMPLE_RATE_INDEX_44_1K,
    HAL_AUD_SAMPLE_RATE_INDEX_48K,

    HAL_AUD_SAMPLE_RATE_INDEX_QTY,
} HAL_AUD_SAMPLE_RATE_INDEX_T;

// =============================================================================
// HAL_AUD_DAC_DIV_REG_T
// -----------------------------------------------------------------------------
/// This type describes the register values to set in codec for a sample rate.
// =============================================================================
typedef struct
{
    UINT16 sel;
    UINT16 div;
} HAL_AUD_DAC_DIV_REG_T;

// =============================================================================
// HAL_AUD_SDM2_FREQ_REG_T
// -----------------------------------------------------------------------------
/// This type describes the register values to set in SDM when
/// changing audio codec frequency for a sample rate.
// =============================================================================
typedef struct
{
    UINT16 sdm2FreqHigh;
    UINT16 sdm2FreqLow;
} HAL_AUD_SDM2_FREQ_REG_T;

#if defined(AUD_3_IN_1_SPK) || defined(ABB_HP_DETECT)
// =============================================================================
// HP_DETECT_REG_VAL_T
// -----------------------------------------------------------------------------
/// This type describes the headphone detect register values to
// set in ABB when changing sample rate.
// =============================================================================
typedef struct
{
    UINT16 hpDet_3eh;
    UINT16 hpDet_3fh;
    UINT16 hpDet_44h;
    UINT16 hpDet_45h;
    UINT16 hpDet_46h;
    UINT16 hpDet_47h;
} HP_DETECT_REG_VAL_T;
#endif


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

#if !defined(_FLASH_PROGRAMMER) && !defined(_T_UPGRADE_PROGRAMMER)

#ifdef ABB_HP_DETECT
// By default audio disallows headphone detection, but others allow.
PRIVATE UINT16 g_halHpDetectOffBitmap = (1<<HP_DETECT_REQ_SRC_AUD);
PRIVATE BOOL g_halAudHeadphoneDacMuted = FALSE;
#endif // ABB_HP_DETECT

// Power control
PRIVATE UINT16 g_halAbbCodecPowerCtrl =
// The registers of codec digital part (starting from
// CODEC_DIG_MIN_REG_INDEX), is accessible and sustainable only
// when codec digital part is out of reset (or REF is powered on).
    ABB_RSTN_CODEC_DR|
    ABB_RSTN_CODEC;

// MIC mode
PRIVATE CONST UINT16 gc_halAbbCodecMicSetting =
    ABB_MB_SEL_BIT(2)|
#ifdef MIC_MEMS_MODE
    ABB_MICCAP_MODE_EN|
#endif //MIC_MEMS_MODE
#ifndef MIC_CAPLESS_MODE
    ABB_MICCAP_MODE_EN|
    ABB_MIC_CHOP_EN|
#endif
    ABB_MASH_EN_ADC;

// VCOM VREF setting
PRIVATE CONST UINT16 gc_halAbbCodecLdoSetting2 =
    ABB_VCOM_MODE(2)|
    ABB_VREF_MODE(1)|
    ABB_ISEL_BIT_B(2)|
    ABB_RESET_DAC_REG(3);

// HP VCOM setting
PRIVATE CONST UINT16 gc_halAbbCodecMiscSetting =
    ABB_HP_MASK_STARTUP|
    0;

// Codec clock setting
PRIVATE CONST UINT16 gc_halAbbCodecClockCodec =
    ABB_DSSI_INV(3)|
    ABB_DCDC_CLKGEN3_DIV_BASE(47);

// PLL setting
PRIVATE CONST UINT16 gc_halAbbCodecPll_13h[HAL_SYS_PLL_MODE_QTY] =
{
    ABB_PLL_AUX_CLKOUT_EN(12)|
    ABB_PLL_AUX_CPAUX_BIT(4)|
    ABB_PLL_AUX_FILTER_IBIT(4)|
    ABB_PLL_AUX_CP_BIT(4)|
    ABB_PLL_AUX_SDM_CLK_SEL_RESET,
    ABB_PLL_AUX_CLKOUT_EN(12)|
    ABB_PLL_AUX_CPAUX_BIT(1)|
    ABB_PLL_AUX_FILTER_IBIT(1)|
    ABB_PLL_AUX_CP_BIT(1)|
    ABB_PLL_AUX_SDM_CLK_SEL_RESET,
    ABB_PLL_AUX_CLKOUT_EN(12)|
    ABB_PLL_AUX_CPAUX_BIT(0)|
    ABB_PLL_AUX_FILTER_IBIT(0)|
    ABB_PLL_AUX_CP_BIT(0)|
    ABB_PLL_AUX_SDM_CLK_SEL_RESET,
};

PRIVATE CONST UINT16 gc_halAbbCodecPll_14h[HAL_SYS_PLL_MODE_QTY] =
{
    ABB_PLL_AUX_PD_DR| //switch to pd_pll_aux_reg by default
    ABB_PLL_AUX_VREG_BIT(8) ,
    ABB_PLL_AUX_PD_DR|
    ABB_PLL_AUX_VREG_BIT(1),
    ABB_PLL_AUX_PD_DR|
    ABB_PLL_AUX_PD_REG|
    ABB_PLL_AUX_VREG_BIT(0),
};

PRIVATE UINT32 gc_halAbbCodecPllProfile = HAL_SYS_PLL_MODE_NORMAL;
PRIVATE UINT32 gc_halAbbCodecPllState = FALSE;
PRIVATE CONST UINT16 gc_halAbbCodecSdm2Dither =
    ABB_PLL_SDM2_DCDC4_312M_AUX_34;

PRIVATE UINT16 g_halAudCodecDigEn =
    ABB_DIG_S_DWA_EN|
    ABB_DIG_MASH_EN_ADC|
    ABB_DIG_DAC_CLK_INVERT|
    ABB_DIG_PADET_CLK_INV|
    ABB_DIG_S_CODEC_EN;

PRIVATE UINT16 g_halHpDetect_12h =
    ABB_PD_SSI|
    ABB_DSSI_ANA_ENABLE|
    ABB_DSSI_STABLE_TIME_SEL(1)|
    ABB_DSSI_LG_EN_TIME_SEL(2)|
    ABB_RESET_PADET_TIME_SEL(2);

PRIVATE CONST UINT16 gc_halAudCodecFmMode =
    ABB_CNT_ADC_CLK(12)|
    ABB_S_ADC_OSR_SEL(0);

// Class K PA clock setting
PRIVATE UINT16 g_halAudCodecClockClassKPa =
    ABB_DCDC4_RESETN_TIME_SEL(2)|
    ABB_DCDC4_CLKGEN4_DIV_BASE(160);

PRIVATE UINT16 g_halAudHpDetect_3dh =
#ifndef AUD_3_IN_1_SPK
    ABB_NOTCH20_BYPASS|
#endif
    ABB_EARPHONE_DET_BYPASS|
    ABB_EARPHONE_DET_TH(20)|
    ABB_EARPHONE_DET_COEF_H(7);

// AUD_3_IN_1_SPK mode has the higher priority to control the notch filter
#ifdef AUD_3_IN_1_SPK
PRIVATE BOOL g_halAudBypassNotchFilterApp = FALSE;
#elif defined(ABB_HP_DETECT)
PRIVATE BOOL g_halAudBypassNotchFilterFm = FALSE;
#endif // ABB_HP_DETECT

PRIVATE CONST HAL_AUD_DAC_DIV_REG_T
gc_halAudNormalDacDivRegs[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    {0x0000, 0x0300}, // 8K
    {0x0007, 0x0200}, // 11.025K
    {0x0007, 0x0200}, // 12K
    {0x0004, 0x0180}, // 16K
    {0x0005, 0x0100}, // 22.05K
    {0x0006, 0x0100}, // 24K
    {0x0001, 0x00c0}, // 32K
    {0x0002, 0x0080}, // 44.1K
    {0x0003, 0x0080}, // 48K
};

PRIVATE CONST HAL_AUD_SDM2_FREQ_REG_T
gc_halAudSdm2FreqRegs[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    {0xB57B, 0x0FB9}, // 8K
    {0xA6BD, 0x01BF}, // 11.025K
    {0xB57B, 0x0FB9}, // 12K
    {0xB57B, 0x0FB9}, // 16K
    {0xA6BD, 0x01BF}, // 22.05K
    {0xB57B, 0x0FB9}, // 24K
    {0xB57B, 0x0FB9}, // 32K
    {0xA6BD, 0x01BF}, // 44.1K
    {0xB57B, 0x0FB9}, // 48K
};

#if defined(AUD_3_IN_1_SPK) || defined(ABB_HP_DETECT)
#if 1
PRIVATE CONST HP_DETECT_REG_VAL_T
g_halAudHpDetectFmRegVals =
{0x7df4, 0x0000, 0xc808, 0x3fba, 0x7580, 0xa93f};
#else
PRIVATE CONST HP_DETECT_REG_VAL_T
g_halAudHpDetectFmRegVals =
{0x7df4, 0x0000, 0x080e, 0x3f8d, 0x1a00, 0x613f};
#endif
#endif

#ifdef AUD_3_IN_1_SPK
PRIVATE CONST HP_DETECT_REG_VAL_T
g_halAud3In1SpkMp3RegVals[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    {0xf5c3, 0x0028, 0x2962, 0x3567, 0xce30, 0x522a}, // 8K
    {0xb76b, 0x001d, 0xb988, 0x33f8, 0xf170, 0xb627}, // 11.025K
    {0x4e82, 0x001b, 0x996c, 0x34cd, 0x9b20, 0xfe29}, // 12K
    {0x7ae1, 0x0014, 0x791b, 0x3744, 0x88e0, 0x632e}, // 16K
    {0xdad6, 0x000e, 0xa8d4, 0x3970, 0xe150, 0x1332}, // 22.05K
    {0xa741, 0x000d, 0x28c4, 0x39ed, 0xda30, 0x3333}, // 24K
    {0x3d71, 0x000a, 0x4896, 0x3b57, 0xae80, 0x2736}, // 32K
    {0x6d6b, 0x0007, 0xb86e, 0x3c8d, 0x1b60, 0xdb39}, // 44.1K
    {0xd3a0, 0x0006, 0xf866, 0x3cd1, 0xa3e0, 0x3d39}, // 48K
};
#endif // AUD_3_IN_1_SPK

#ifdef ABB_HP_DETECT
PRIVATE CONST HP_DETECT_REG_VAL_T
g_halAudHpDetectMp3RegVals[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    {0xf5c3, 0x0028, 0xe828, 0x3ebe, 0x7dc0, 0x643d}, // 8K
    {0xb76b, 0x001d, 0xd81d, 0x3f15, 0x2ba0, 0x683e}, // 11.025K
    {0x4e82, 0x001b, 0x980d, 0x3f93, 0x2730, 0xaa3f}, // 12K
    {0x7ae1, 0x0014, 0xe814, 0x3f5d, 0xbbd0, 0x533e}, // 16K
    {0xdad6, 0x000e, 0x280e, 0x3f8a, 0x1440, 0xc53f}, // 22.05K
    {0xa741, 0x000d, 0x980d, 0x3f93, 0x2730, 0x943f}, // 24K
    {0x3d71, 0x000a, 0x980a, 0x3fae, 0x5d20, 0x323f}, // 32K
    {0x6d6b, 0x0007, 0x280e, 0x3f8a, 0x1440, 0xbf3f}, // 44.1K
    {0xd3a0, 0x0006, 0xa806, 0x3fc9, 0x9340, 0xce3f}, // 48K
};
#endif // ABB_HP_DETECT

#endif // !_FLASH_PROGRAMMER && !_T_UPGRADE_PROGRAMMER

//22579200
PRIVATE CONST UINT16
gc_halAudAdcDiv1[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    0x02c2, // 8K
    0x0202, // 11.025K
    0xC382, // 12K
    0xa142, // 16K
    0x0102, // 22.05K
    0xC0C2, // 24K
    0xE082, // 32K
    0x0082, // 44.1K
    0xE042, // 48K
};
//24576000
PRIVATE CONST UINT16
gc_halAudAdcDiv2[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    0x0302, // 8K
    0xe202, // 11.025K
    0x0202, // 12K
    0x0182, // 16K
    0x8102, // 22.05K
    0x0102, // 24K
    0x00C2, // 32K
    0x4082, // 44.1K
    0x0082, // 48K
};
//21700 fm record
PRIVATE CONST UINT16
gc_halAudAdcDiv3[HAL_AUD_SAMPLE_RATE_INDEX_QTY] =
{
    0x0292, // 8K
    0x01d2, // 11.025K
    0x01d2, // 12K
    0x0252, // 16K
    0x00d2, // 22.05K
    0x00d2, // 24K
    0x0092, // 32K
    0x0092, // 44.1K
    0x0052, // 48K
};

// =============================================================================
//  FUNCTIONS
// =============================================================================

PUBLIC VOID hal_AbbOpen(VOID)
{
    static BOOL abbInitDone = FALSE;
    UINT32 data;
    if (!abbInitDone)
    {
        // reset software and register
        hal_AbbRegWrite(CODEC_SOFT_RESET, ABB_REG_RESET_SET|
                        ABB_SOFT_RESET_SET);
        hal_TimDelay(1);
        hal_AbbRegWrite(CODEC_SOFT_RESET, ABB_REG_RESET_CLR|
                        ABB_SOFT_RESET_CLR);

#if !defined(_FLASH_PROGRAMMER) && !defined(_T_UPGRADE_PROGRAMMER)
        // pull down usb D+
        hal_AbbRegWrite(USB_CONTROL,&data);
        hal_AbbRegWrite(USB_CONTROL, data|ABB_PD_USB);

        // Codec digital part should be kept out of reset
        hal_AbbRegWrite(CODEC_POWER_CTRL, g_halAbbCodecPowerCtrl);

        // Disable codec DSP to start DSP configuration
        hal_AudEnableCodecDsp(FALSE);

        // MIC mode
        hal_AbbRegWrite(CODEC_MIC_SETTING, gc_halAbbCodecMicSetting);
#ifdef MIC_MEMS_MODE
		hal_AbbRegRead(CODEC_PLL_SETTING1, &data);
		data |= (ABB_BOOST_BP_MIC_DR | ABB_BOOST_BP_MIC_REG);
        hal_AbbRegWrite(CODEC_PLL_SETTING1, data);
#endif //MIC_MEMS_MODE

        // VCOM VREF setting
        hal_AbbRegWrite(CODEC_LDO_SETTING2, gc_halAbbCodecLdoSetting2);
        // HP VCOM setting
        hal_AbbRegWrite(CODEC_MISC_SETTING, gc_halAbbCodecMiscSetting);
        // DSSI setting
        hal_AudEnableDSSI(FALSE);
        // Clock settings
        hal_AbbRegWrite(CODEC_CLOCK_CODEC, gc_halAbbCodecClockCodec); // divider 1/24
        //turn off pll by default
        hal_AbbRegWrite(CODEC_PLL_13H, gc_halAbbCodecPll_13h[HAL_SYS_PLL_MODE_OFF]); 
        hal_AbbRegWrite(CODEC_PLL_14H, gc_halAbbCodecPll_14h[HAL_SYS_PLL_MODE_OFF]); 
        hal_AbbRegWrite(CODEC_SDM2_DITHER, gc_halAbbCodecSdm2Dither);

        hal_AbbRegWrite(CODEC_LDO_SETTING1, ABB_HP_DISCHG_OUT_DIS_DR|ABB_HP_DISCHG_OUT_DIS_REG);

        // Init codec sample rate
        hal_AudSetSampleRate(HAL_AUD_USER_CODEC, HAL_AIF_FREQ_8000HZ);
        // Init to non-FM mode
        hal_AudSetFmMode(FALSE);
        // Init to non-voice mode
        hal_AudSetVoiceMode(FALSE);
        // Init Dither gain
        hal_AudSetDacVolume(0);
        hal_AudSetAdcVolume(0);

        // Enable codec DSP
        hal_AudEnableCodecDsp(TRUE);

#ifdef SPI_REG_DEBUG
        extern VOID abb_RegisterSpiRegCheckFunc(VOID);
        abb_RegisterSpiRegCheckFunc();
#endif // SPI_REG_DEBUG

#endif // !_FLASH_PROGRAMMER && !_T_UPGRADE_PROGRAMMER

        abbInitDone = TRUE;
    }
}

PUBLIC VOID hal_AudSetHPMode(BOOL isHPMode)
{
    UINT16 CodecLdoSetting2 = 0;

    if (isHPMode)
    {
        CodecLdoSetting2 =
            ABB_VCOM_MODE(0)|
            ABB_VREF_MODE(5)|
            ABB_ISEL_BIT_B(2)|
            ABB_RESET_DAC_REG(3);
    }
    else
    {
        CodecLdoSetting2 =
            ABB_VCOM_MODE(2)|
            ABB_VREF_MODE(1)|
            ABB_ISEL_BIT_B(2)|
            ABB_RESET_DAC_REG(3);
    }

    // VCOM VREF setting
    hal_AbbRegWrite(CODEC_LDO_SETTING2, CodecLdoSetting2);

    hal_TimDelay(328);//20*16384/1000

}



PUBLIC VOID hal_AbbEnableUsbPhy(BOOL enable)
{
    UINT32 usbCtrl = ABB_USB_CTRL(1)|
                     ABB_USB_VREG_BIT(7)|
                     ABB_SSI_VREFN_RBIT(3)|
                     ABB_SSI_VREFP_RBIT(3)|
                     ABB_DAC_IBIT(7);
    if (!enable)
    {
        usbCtrl |= ABB_PD_USB;
    }
    hal_AbbRegWrite(USB_CONTROL, usbCtrl);
}

PUBLIC VOID HAL_BOOT_FUNC_INTERNAL hal_DisableUsbDp(VOID)
{
    hal_AbbIspiWrite(USB_CONTROL,0xa819);//disable D+ of usb.
}




PUBLIC VOID hal_AbbEnableSim(UINT32 simMask)
{
    UINT16 value = ABB_UNSEL_RST_VAL_1|
                   ABB_UNSEL_RST_VAL_2|
                   ABB_UNSEL_RST_VAL_3|
                   ABB_UNSEL_RST_VAL_4;

    if (simMask & HAL_ABB_SIM_SELECT_SIM0)
    {
        value |= ABB_SIM_SELECT_SIM1;
    }
    if (simMask & HAL_ABB_SIM_SELECT_SIM1)
    {
        value |= ABB_SIM_SELECT_SIM2;
    }
    if (simMask & HAL_ABB_SIM_SELECT_SIM2)
    {
        value |= ABB_SIM_SELECT_SIM3;
    }
    if (simMask & HAL_ABB_SIM_SELECT_SIM3)
    {
        value |= ABB_SIM_SELECT_SIM4;
    }

    hal_AbbRegWrite(CODEC_SIM_INTERFACE, value);
}

PROTECTED VOID hal_AbbIspiReadOutputSelect(UINT32 addr)
{
    UINT16 spiOutSel = hal_AbbIspiRead(CODEC_RESET_CTRL);

    spiOutSel = (spiOutSel & ~ABB_SPI_OUT_MASK);
    if (addr < CODEC_DIG_MIN_REG_INDEX)
    {
        spiOutSel |= ABB_SPI_OUT_ABB;
    }
    else
    {
        spiOutSel |= ABB_SPI_OUT_CODEC;
    }
    hal_AbbIspiWrite(CODEC_RESET_CTRL, spiOutSel);
}

#if !defined(_FLASH_PROGRAMMER) && !defined(_T_UPGRADE_PROGRAMMER)

#ifdef AUD_3_IN_1_SPK
PUBLIC VOID hal_AudBypassNotchFilterReq(BOOL bypass)
{
    UINT16 AudHpDetect_3dh;
    // 3-in-1 speaker enables the notch filter to filter out 170 Hz signal
    // to avoid vibration, and disables it to vibrate.
    UINT32 status = hal_SysEnterCriticalSection();
    g_halAudBypassNotchFilterApp = bypass;

    hal_AbbRegRead(CODEC_HP_DETECT_3DH, &AudHpDetect_3dh);
    if (g_halAudBypassNotchFilterApp)
    {
        AudHpDetect_3dh |= ABB_NOTCH20_BYPASS;
    }
    else
    {
        AudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
    }
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, AudHpDetect_3dh);
    hal_SysExitCriticalSection(status);
}
#endif // AUD_3_IN_1_SPK

/// Must wait for AUD_TIME_SET_FM_MODE if setting to non-FM mode
PUBLIC VOID hal_AudSetFmMode(BOOL isFmMode)
{
    UINT32 status;
    UINT32 fmModeSetting = gc_halAudCodecFmMode;
    UINT32 codecClockSetting = gc_halAbbCodecClockCodec;

    if (isFmMode)
    {
        // FM mode
        // FM record begin
        // 1. enable muxin lef and right
        //    this will be done in hal_AudSetCodecMode later
        // 2. set codec to dac_mode
        //    this is the default config in hal_AudSetSelCfg
        // 3. fm_mode adc clk to 5, means ~8K ( 2 means ~21K) when osr is 0
        fmModeSetting &= ~(ABB_CNT_ADC_CLK_MASK);
        fmModeSetting |= ABB_CNT_ADC_CLK_FOR_FM_RECORD;
        //   A. fm_mode osr to 00
        fmModeSetting &= ~(ABB_S_ADC_OSR_SEL_MASK);
        fmModeSetting |= ABB_S_ADC_OSR_SEL_2M;
        //   B. sample rate osr, this will be set every time when codec mode
        //      hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, xxxx);
        //      this will be done in hal_AudSetSampleRate
        //   C.  codec clock to 23
        codecClockSetting &= ~(ABB_DCDC_CLKGEN3_DIV_BASE_MASK);
        codecClockSetting |= ABB_DCDC_CLKGEN3_DIV_BASE_FOR_FM_RECORD;

        hal_AbbRegWrite(CODEC_CLOCK_CODEC, codecClockSetting);
        hal_AbbRegWrite(CODEC_FM_MODE, fmModeSetting | ABB_FM_MODE|ABB_CLK_FM_SEL);

        // Set DCDC4 clock divider for FM mode
        // Avoid conflicting with hal_AudForceDcdc4ClockOn
        status = hal_SysEnterCriticalSection();
        g_halAudCodecClockClassKPa &= ~ABB_DCDC4_CLKGEN4_DIV_BASE_MASK;
        g_halAudCodecClockClassKPa |=
            ABB_DCDC4_CLKGEN4_CLK_SEL|ABB_DCDC4_CLKGEN4_DIV_BASE(100);
        hal_AbbRegWrite(CODEC_CLOCK_CLASS_K_PA, g_halAudCodecClockClassKPa);
        hal_AudEnableAuxpll(FALSE);
        hal_SysExitCriticalSection(status);
    }
    else
    {
        // Codec mode
        hal_AudEnableAuxpll(TRUE);
        hal_AbbRegWrite(CODEC_CLOCK_CODEC, codecClockSetting);
        hal_AbbRegWrite(CODEC_FM_MODE, fmModeSetting);

        // Set DCDC4 clock divider for codec mode
        // Avoid conflicting with hal_AudForceDcdc4ClockOn
        status = hal_SysEnterCriticalSection();
        g_halAudCodecClockClassKPa &=
            ~(ABB_DCDC4_CLKGEN4_CLK_SEL|ABB_DCDC4_CLKGEN4_DIV_BASE_MASK);
        g_halAudCodecClockClassKPa |= ABB_DCDC4_CLKGEN4_DIV_BASE(160);
        hal_AbbRegWrite(CODEC_CLOCK_CLASS_K_PA, g_halAudCodecClockClassKPa);
        hal_SysExitCriticalSection(status);
    }

    // AUD_3_IN_1_SPK mode has the higher priority to control the notch filter
#if defined(AUD_3_IN_1_SPK) || !defined(ABB_HP_DETECT)
    // Just set the current value (or the initial value)
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
#else // !AUD_3_IN_1_SPK && ABB_HP_DETECT
    status = hal_SysEnterCriticalSection();
    g_halAudBypassNotchFilterFm = isFmMode;
    if ( (g_halAudHpDetect_3dh & ABB_EARPHONE_DET_BYPASS) == 0 &&
            !g_halAudBypassNotchFilterFm )
    {
        // HP DET is on and FM is off
        g_halAudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
    }
    else
    {
        g_halAudHpDetect_3dh |= ABB_NOTCH20_BYPASS;
    }
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
    hal_SysExitCriticalSection(status);
#endif // !AUD_3_IN_1_SPK && ABB_HP_DETECT
}

PUBLIC VOID hal_AudSetVoiceMode(BOOL isVoiceMode)
{
    if (isVoiceMode)
    {
        g_halAudCodecDigEn |= ABB_DIG_S_ISPHONE;
    }
    else
    {
        g_halAudCodecDigEn &= ~ABB_DIG_S_ISPHONE;
    }
    hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn);

    pmd_SpeakerPaCpLowOutput(isVoiceMode);
#if (AUD_SPK_DSSI_DETECT==2)
    hal_AudEnableDSSI(TRUE);
#elif (AUD_SPK_DSSI_DETECT==1)
    hal_AudEnableDSSI(!isVoiceMode);
#else
    //hal_AudEnableDSSI(FALSE); //DSSI mode does not work by default
#endif
}

#ifdef ABB_HP_DETECT
VOID hal_AudMuteHeadphoneDac(BOOL mute)
{
    g_halAudHeadphoneDacMuted = mute;
}
#endif

PUBLIC VOID hal_AudEnableDac(BOOL on)
{
    if (on)
    {
#ifdef AUD_MUTE_FOR_BT_WORKAROUND
        if (hal_AudOutputMuted())
        {
            on = FALSE;
        }
#endif // AUD_MUTE_FOR_BT_WORKAROUND
#ifdef ABB_HP_DETECT
        if (g_halAudHeadphoneDacMuted)
        {
            on = FALSE;
        }
#endif
    }

    if (on)
    {
        // Not to direct-reg codec DAC
        g_halAbbCodecPowerCtrl &= ~ABB_DAC_EN_CODEC_DR;
    }
    else
    {
        // Direct-reg to disable codec DAC
        g_halAbbCodecPowerCtrl |= ABB_DAC_EN_CODEC_DR;
    }
    hal_AbbRegWrite(CODEC_POWER_CTRL, g_halAbbCodecPowerCtrl);
}

PUBLIC VOID hal_AudEnableCodecDsp(BOOL on)
{
#ifdef ABB_HP_DETECT
    //HAL_TRACE(HAL_AUDIO_TRC, 0, "hal_AudEnableCodecDsp: %d, offMap=0x%04x",
    //    on, g_halHpDetectOffBitmap);
#endif

#ifdef VMIC_POWER_ON_WORKAROUND
#ifdef ABB_HP_DETECT
    if (!on)
    {
        // Stop headphone detection before resetting codec digital part
        if (g_halHpDetectOffBitmap == 0)
        {
            // Not respond to HP-OUT IRQ until codec is re-enabled
            pmd_StartHpOutDetect(FALSE);
        }
    }
#endif
#endif

    // Control the reset state of codec digital part
    if (!on)
    {
        // All the DAC modes should be disabled before enabling/disabling
        // codec digital part, as DAC mode control needs the clock from
        // codec digital part, and if the clock is stopped while PA
        // is working, pop sound might occur.
        hal_AbbRegWrite(CODEC_MODE_SEL, ABB_RECI_DAC_EN_DR|
                        ABB_DAC_MODE_L_EN_DR|
                        ABB_DAC_MODE_R_EN_DR);
    }

    if (on)
    {
        // CODEC DSP needs a little time to prepare before enabled
        CONST UINT32 dspPrepareTime = (1 MS_WAITING);
        UINT32 k1;
        HAL_SYS_GET_K1(k1);
        if (k1 != 0)
        {
            // Interrupt or startup context
            hal_TimDelay(dspPrepareTime);
        }
        else
        {
            // Task context
            sxr_Sleep(dspPrepareTime);
        }

        g_halAudCodecDigEn |= ABB_DIG_S_CODEC_EN;
    }
    else
    {
        g_halAudCodecDigEn &= ~ABB_DIG_S_CODEC_EN;
    }
    hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn);

    if (on)
    {
        // All the DAC modes should be disabled before enabling/disabling
        // codec digital part, as DAC mode control needs the clock from
        // codec digital part, and if the clock is stopped while PA
        // is working, pop sound might occur.
        hal_AbbRegWrite(CODEC_MODE_SEL, 0);
    }

#ifdef VMIC_POWER_ON_WORKAROUND
#ifdef ABB_HP_DETECT
    if (on)
    {
        // Restart headphone detection after resetting codec digital part
        if (g_halHpDetectOffBitmap == 0)
        {
            pmd_StartHpOutDetect(TRUE);
        }
    }
#endif
#endif
}

// =============================================================================
// hal_AudGetSampleRateIndex
// -----------------------------------------------------------------------------
/// get index of HAL audio sample rate according to aif sampleRate
/// @param  sampleRate The AIF sample rate
/// @return index of HAL audio sample rate
// =============================================================================
PRIVATE HAL_AUD_SAMPLE_RATE_INDEX_T hal_AudGetSampleRateIndex(HAL_AIF_FREQ_T sampleRate)
{
    HAL_AUD_SAMPLE_RATE_INDEX_T index = HAL_AUD_SAMPLE_RATE_INDEX_8K;

    switch (sampleRate)
    {
        case HAL_AIF_FREQ_8000HZ :
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_8K;
            break;
        }
        case HAL_AIF_FREQ_11025HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_11_025K;
            break;
        }
        case HAL_AIF_FREQ_12000HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_12K;
            break;
        }
        case HAL_AIF_FREQ_16000HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_16K;
            break;
        }
        case HAL_AIF_FREQ_22050HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_22_05K;
            break;
        }
        case HAL_AIF_FREQ_24000HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_24K;
            break;
        }
        case HAL_AIF_FREQ_32000HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_32K;
            break;
        }
        case HAL_AIF_FREQ_44100HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_44_1K;
            break;
        }
        case HAL_AIF_FREQ_48000HZ:
        {
            index = HAL_AUD_SAMPLE_RATE_INDEX_48K;
            break;
        }
        default:
        {
            HAL_ASSERT(FALSE, "Unsupported sample rate in HAL ABB: %d", sampleRate);
            break;
        }
    }

    return index;
}

PUBLIC VOID hal_AudSetSampleRate(HAL_AUD_USER_T user, HAL_AIF_FREQ_T sampleRate)
{
    HAL_AUD_SAMPLE_RATE_INDEX_T index;
    UINT32 selReg;
    UINT32 divReg;
    UINT32 data;
    UINT16 AudHpDetect_3dh;

    index = hal_AudGetSampleRateIndex(sampleRate);

    selReg = gc_halAudNormalDacDivRegs[index].sel;
    if(user == HAL_AUD_USER_FM)
    {
        // fm mode, set osr reg_2c<7:8> to 00, for fm record
        selReg &= (~ABB_DIG_DAC_OSR_SEL_MASK);
        selReg |= ABB_DIG_DAC_OSR_SEL_128;
    }
    divReg = gc_halAudNormalDacDivRegs[index].div;

    hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, selReg);
    hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_DIV, divReg);

    if (user == HAL_AUD_USER_CODEC)
    {
        hal_AbbRegWrite(CODEC_SDM2_FREQ_HIGH, gc_halAudSdm2FreqRegs[index].sdm2FreqHigh);
        hal_AbbRegWrite(CODEC_SDM2_FREQ_LOW, gc_halAudSdm2FreqRegs[index].sdm2FreqLow);

#ifdef ABB_HP_DETECT
        // Generate 20 Hz signal
        hal_AbbRegWrite(CODEC_HP_DETECT_3EH, g_halAudHpDetectMp3RegVals[index].hpDet_3eh);
        hal_AbbRegWrite(CODEC_HP_DETECT_3FH, g_halAudHpDetectMp3RegVals[index].hpDet_3fh);
#endif // ABB_HP_DETECT

#ifdef AUD_3_IN_1_SPK
        // Set notch filter coefficients
        // Filter out 170 Hz signal
        hal_AbbRegWrite(CODEC_HP_DETECT_44H, g_halAud3In1SpkMp3RegVals[index].hpDet_44h);
        hal_AbbRegWrite(CODEC_HP_DETECT_45H, g_halAud3In1SpkMp3RegVals[index].hpDet_45h);
        hal_AbbRegWrite(CODEC_HP_DETECT_46H, g_halAud3In1SpkMp3RegVals[index].hpDet_46h);
        hal_AbbRegWrite(CODEC_HP_DETECT_47H, g_halAud3In1SpkMp3RegVals[index].hpDet_47h);
#else // !AUD_3_IN_1_SPK
#ifdef ABB_HP_DETECT
        // Set notch filter coefficients
        // Filter out 20 Hz signal
        hal_AbbRegWrite(CODEC_HP_DETECT_44H, g_halAudHpDetectMp3RegVals[index].hpDet_44h);
        hal_AbbRegWrite(CODEC_HP_DETECT_45H, g_halAudHpDetectMp3RegVals[index].hpDet_45h);
        hal_AbbRegWrite(CODEC_HP_DETECT_46H, g_halAudHpDetectMp3RegVals[index].hpDet_46h);
        hal_AbbRegWrite(CODEC_HP_DETECT_47H, g_halAudHpDetectMp3RegVals[index].hpDet_47h);
#endif // ABB_HP_DETECT
#endif // !AUD_3_IN_1_SPK
    }
    else if (user == HAL_AUD_USER_FM)
    {
#ifdef ABB_HP_DETECT
        // Generate 20 Hz signal
        hal_AbbRegWrite(CODEC_HP_DETECT_3EH, g_halAudHpDetectFmRegVals.hpDet_3eh);
        hal_AbbRegWrite(CODEC_HP_DETECT_3FH, g_halAudHpDetectFmRegVals.hpDet_3fh);
#endif

#ifdef AUD_3_IN_1_SPK
        // Set notch filter coefficients
        // Filter out 170 Hz signal
        AudHpDetect_3dh = g_halAudHpDetect_3dh;
        AudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
        AudHpDetect_3dh |= ABB_DAC_HPF_MODE;
        AudHpDetect_3dh |= ABB_EARPHONE_DET_BYPASS;
        AudHpDetect_3dh |=ABB_EARPHONE_DET_SEL;
        AudHpDetect_3dh &= ~ABB_EARPHONE_DET_TH_MASK;
        AudHpDetect_3dh &= ~ABB_EARPHONE_DET_COEF_H_MASK;
        AudHpDetect_3dh |=  ABB_EARPHONE_DET_TH(10)|
                            ABB_EARPHONE_DET_COEF_H(7);
        hal_AbbRegWrite(CODEC_HP_DETECT_3DH, AudHpDetect_3dh);

        hal_AbbRegWrite(CODEC_HP_DETECT_44H, 0x08d3);
        hal_AbbRegWrite(CODEC_HP_DETECT_45H, 0x4000);
        hal_AbbRegWrite(CODEC_HP_DETECT_46H, 0x0e08);
        hal_AbbRegWrite(CODEC_HP_DETECT_47H, 0x9034);
        hal_AbbRegWrite(CODEC_HP_DETECT_48H, 0x0001);


#else //usert hp Filter out the DC
        //hal_AbbRegWrite(CODEC_HP_DETECT_3DH, 0x58a7);
        AudHpDetect_3dh = g_halAudHpDetect_3dh;
        AudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
        AudHpDetect_3dh |= ABB_DAC_HPF_MODE;
        AudHpDetect_3dh |= ABB_EARPHONE_DET_BYPASS;
        AudHpDetect_3dh |=ABB_EARPHONE_DET_SEL;
        AudHpDetect_3dh &= ~ABB_EARPHONE_DET_TH_MASK;
        AudHpDetect_3dh &= ~ABB_EARPHONE_DET_COEF_H_MASK;
        AudHpDetect_3dh |=  ABB_EARPHONE_DET_TH(10)|
                            ABB_EARPHONE_DET_COEF_H(7);
        hal_AbbRegWrite(CODEC_HP_DETECT_3DH, AudHpDetect_3dh);

        hal_AbbRegWrite(CODEC_HP_DETECT_44H, 0x082b);
        hal_AbbRegWrite(CODEC_HP_DETECT_45H, 0x4000);
        hal_AbbRegWrite(CODEC_HP_DETECT_46H, 0x5938);
        hal_AbbRegWrite(CODEC_HP_DETECT_47H, 0x533d);
        hal_AbbRegWrite(CODEC_HP_DETECT_48H, 0x0001);

		// reset audio codec
    	hal_AbbRegRead(CODEC_DIG_FREQ_SAMPLE_SEL, &data);
    	hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, (data & (~(ABB_DIG_RESET_DAC_PATH | ABB_DIG_RESET_ADC_PATH))));
    	hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, (data | ABB_DIG_RESET_DAC_PATH | ABB_DIG_RESET_ADC_PATH));
    	hal_AbbRegWrite(CODEC_DIG_FREQ_SAMPLE_SEL, (data & (~(ABB_DIG_RESET_DAC_PATH | ABB_DIG_RESET_ADC_PATH))));

        hal_AbbRegRead(CODEC_HP_DETECT_33H, &data);
        hal_AbbRegWrite(CODEC_HP_DETECT_33H, data|2);
#endif
    }
}

PUBLIC VOID hal_AudSetAdcSampleRate(HAL_AIF_FREQ_T dacRate, HAL_AIF_FREQ_T adcRate, BOOL IsPlaying)
{
    UINT32 data;
    UINT32 fmModeSetting;
    HAL_AUD_SAMPLE_RATE_INDEX_T index;
    UINT32 clock_source = 24576000;

    index = hal_AudGetSampleRateIndex(adcRate);

    // The audio PLL freq is determined by DAC sample rate
    if(IsPlaying)
    {
        switch(dacRate)
        {
            case HAL_AIF_FREQ_8000HZ:
            case HAL_AIF_FREQ_12000HZ:
            case HAL_AIF_FREQ_16000HZ:
            case HAL_AIF_FREQ_24000HZ:
            case HAL_AIF_FREQ_32000HZ:
            case HAL_AIF_FREQ_48000HZ:
                clock_source = 24576000;
                break;
            case HAL_AIF_FREQ_11025HZ:
            case HAL_AIF_FREQ_44100HZ:
            case HAL_AIF_FREQ_22050HZ:
                clock_source = 22579200;
                break;
            default:
                HAL_ASSERT(FALSE, "hal_AudSetAdcSampleRate: Unsupported DAC rate: %d", dacRate);
                return;
        }
    }
    else
    {
        switch(adcRate)
        {
            case HAL_AIF_FREQ_8000HZ:
            case HAL_AIF_FREQ_12000HZ:
            case HAL_AIF_FREQ_16000HZ:
            case HAL_AIF_FREQ_24000HZ:
            case HAL_AIF_FREQ_32000HZ:
            case HAL_AIF_FREQ_48000HZ:
                clock_source = 24576000;
                break;

            case HAL_AIF_FREQ_11025HZ:
            case HAL_AIF_FREQ_44100HZ:
            case HAL_AIF_FREQ_22050HZ:
                clock_source = 22579200;
                break;
            default:
                HAL_ASSERT(FALSE, "hal_AudSetAdcSampleRate: Unsupported DAC rate: %d", dacRate);
                return;
        }
    }

    //haven set fmmode in hal_AudSetFmMode
    hal_AbbRegRead(CODEC_FM_MODE, &data);
    fmModeSetting = data;

    fmModeSetting &= ~(ABB_CNT_ADC_CLK_MASK);

    if(clock_source == 22579200)
        fmModeSetting |= ABB_CNT_ADC_CLK_MASK&gc_halAudAdcDiv1[index];
    else if((fmModeSetting & ABB_CLK_FM_SEL) &&(fmModeSetting & ABB_FM_MODE))
    {
        fmModeSetting |= ABB_CNT_ADC_CLK_MASK&gc_halAudAdcDiv3[index];
    }
    else 
        fmModeSetting |= ABB_CNT_ADC_CLK_MASK&gc_halAudAdcDiv2[index];

    hal_AbbRegWrite(CODEC_FM_MODE,fmModeSetting);
    // The audio PLL freq is determined by DAC sample rate
    if(!IsPlaying)
    {
        hal_AbbRegWrite(CODEC_SDM2_FREQ_HIGH, gc_halAudSdm2FreqRegs[index].sdm2FreqHigh);
        hal_AbbRegWrite(CODEC_SDM2_FREQ_LOW, gc_halAudSdm2FreqRegs[index].sdm2FreqLow);
    }

    hal_AbbRegRead(CODEC_HP_DETECT_33H, &data);
    hal_AbbRegWrite(CODEC_HP_DETECT_33H, data|2);
}

// Force to enable charge pump clock.
// Called by PMD when LCD backlight is on if LCD_BACKLIGHT_CHARGE_PUMP enabled.
PUBLIC VOID hal_AudForceDcdc4ClockOn(BOOL on)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if (on)
    {
        g_halAudCodecClockClassKPa |= (ABB_DCDC4_CLKGEN4_RESETN_DR|
                                       ABB_DCDC4_CLKGEN4_RESETN_REG);
    }
    else
    {
        g_halAudCodecClockClassKPa &= ~(ABB_DCDC4_CLKGEN4_RESETN_DR|
                                        ABB_DCDC4_CLKGEN4_RESETN_REG);
    }
    hal_AbbRegWrite(CODEC_CLOCK_CLASS_K_PA, g_halAudCodecClockClassKPa);
    hal_SysExitCriticalSection(status);
}

PUBLIC VOID hal_AudEnableDSSI(BOOL on)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if(on)
    {
        g_halHpDetect_12h &= ~ABB_PD_SSI;
    }
    else
    {
        g_halHpDetect_12h |= ABB_PD_SSI;
    }
    hal_AbbRegWrite(CODEC_HP_DETECT_12H, g_halHpDetect_12h);
    hal_SysExitCriticalSection(status);
}

//hal_AudEnableAuxpll
//enable/disable aux pll
PUBLIC VOID hal_AudEnableAuxpll (BOOL on)
{
    UINT32 status = hal_SysEnterCriticalSection();
    
    gc_halAbbCodecPllProfile = HAL_SYS_PLL_MODE_NORMAL;
    
    if(on == 0)
    {
	   gc_halAbbCodecPllProfile = HAL_SYS_PLL_MODE_OFF;
    }

    hal_AbbRegWrite(CODEC_PLL_14H, gc_halAbbCodecPll_14h[gc_halAbbCodecPllProfile]); //enable AUX_PLL
    hal_AbbRegWrite(CODEC_PLL_13H, gc_halAbbCodecPll_13h[gc_halAbbCodecPllProfile]); // int_mode=0    

    hal_SysExitCriticalSection(status);

    gc_halAbbCodecPllState = on;
}


//hal_AudSetAuxPllConfig
//set aux profile mode
PUBLIC VOID hal_AudSetAuxPllConfig(HAL_SYS_PLL_PROFILE_T Mode)
{
    if(Mode == HAL_SYS_PLL_MODE_OFF)
    {
        hal_AudEnableAuxpll(FALSE);
    }
    else
    {
        hal_AudEnableAuxpll(TRUE);
    }
}

PUBLIC VOID hal_AudHpDetectEnable(BOOL on)
{
    UINT32 micSetting;




    if(on)
    {
        g_halHpDetect_12h |= ABB_RESET_PADET_DR;
        g_halHpDetect_12h |= ABB_RESET_PADET_REG;
    }
    else
    {
        g_halHpDetect_12h |= ABB_RESET_PADET_DR;
        g_halHpDetect_12h &= ~ABB_RESET_PADET_REG;
    }

    hal_AbbRegWrite(CODEC_HP_DETECT_12H, g_halHpDetect_12h);

}
#ifdef ABB_HP_DETECT
PUBLIC VOID hal_AudEnableHpDetect(BOOL on, HP_DETECT_REQ_SRC_T src)
{
    //HAL_TRACE(HAL_AUDIO_TRC, 0, "hal_AudEnableHpDetect: %d, src=%d, offMap=0x%04x",
    //    on, src, g_halHpDetectOffBitmap);

    UINT16 prevBitmap = g_halHpDetectOffBitmap;
    if (on)
    {
        g_halHpDetectOffBitmap &= ~(1<<src);
    }
    else
    {
        g_halHpDetectOffBitmap |= (1<<src);
    }

    if (!on && prevBitmap != 0)
    {
        // Headphone detection has been already disabled
        return;
    }
    if (on)
    {
        if (g_halHpDetectOffBitmap != 0)
        {
            // Another src still disables headphone detection
            return;
        }
        if ((g_halAudHpDetect_3dh & ABB_EARPHONE_DET_BYPASS) == 0)
        {
            // Headphone detection is running, and we cannot enable it twice
            // (codec will be reset during the enabling process)
            return;
        }
    }

    if (!on)
    {
        pmd_StartHpOutDetect(FALSE);
    }

    hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn&~ABB_DIG_S_CODEC_EN);

    UINT32 status = hal_SysEnterCriticalSection();
    if (on)
    {
        g_halAudHpDetect_3dh &= ~ABB_EARPHONE_DET_BYPASS;
        g_halHpDetect_12h |= ABB_PADET_EN;
    }
    else
    {
        g_halAudHpDetect_3dh |= ABB_EARPHONE_DET_BYPASS;
        g_halHpDetect_12h &= ~ABB_PADET_EN;
    }
    // AUD_3_IN_1_SPK mode has the higher priority to control the notch filter
#ifndef AUD_3_IN_1_SPK
    if (on && !g_halAudBypassNotchFilterFm)
    {
        g_halAudHpDetect_3dh &= ~ABB_NOTCH20_BYPASS;
    }
    else
    {
        g_halAudHpDetect_3dh |= ABB_NOTCH20_BYPASS;
    }
#endif
    // CODEC_HP_DETECT_3DH controls the HP DET digital part, and the
    // whole codec digital part should be reset if it changes
    hal_AbbRegWrite(CODEC_HP_DETECT_3DH, g_halAudHpDetect_3dh);
    // CODEC_HP_DETECT_12H controls the HP DET analog part, and it is
    // irrelevant to codec digital part
    hal_AbbRegWrite(CODEC_HP_DETECT_12H, g_halHpDetect_12h);
    hal_SysExitCriticalSection(status);

    // Keep disabling codec for FM to avoid pop sound when searching
    // FM stations
    if (on || src != HP_DETECT_REQ_SRC_FM)
    {
        hal_AbbRegWrite(CODEC_DIG_EN, g_halAudCodecDigEn);
    }

    if (on)
    {
        pmd_StartHpOutDetect(TRUE);
    }
}
#endif // ABB_HP_DETECT

PUBLIC VOID hal_AudSetDacVolume(UINT32 vol)
{
    // DacVolume[6:2] = 0 ... 31 = -26dB ... 5dB
    UINT32 data;
    data = ABB_DIG_SIDE_TONE_GAIN_L(15)|
           ABB_DIG_DAC_VOLUME(vol);
    data |= ABB_DIG_DITHER_GAIN_H(0)|
            ABB_DIG_SDM_GAIN(3);
    hal_AbbRegWrite(CODEC_DIG_DAC_GAIN, data);
}

PUBLIC VOID hal_AudSetAdcVolume(UINT32 vol)
{
    // MIC AdcVolume[12:9] = 0 ... 15 = MUTE -12dB -10dB ... 14dB 16dB
    UINT32 data;
    data = ABB_DIG_MIC_VOLUME(vol)|
           ABB_DIG_SIDE_TONE_GAIN_H(1);
    data |= ABB_DIG_DITHER_GAIN_L(4);
    hal_AbbRegWrite(CODEC_DIG_MIC_GAIN, data);
}

PROTECTED VOID hal_AudSetCodecMode(HAL_AUD_CODEC_MODE_PARAM_T *modeParam)
{
    HAL_AUD_SEL_T orgSelCfg = modeParam->orgSelCfg;
    // Line-in & MIC config
    UINT32 lineInSetting = 0;
    UINT32 micSetting = gc_halAbbCodecMicSetting;

    if (modeParam->curUser == HAL_AUD_USER_LINEIN ||
            modeParam->curUser == HAL_AUD_USER_FM)
    {
        if (orgSelCfg.headSel || orgSelCfg.rcvSel ||
                orgSelCfg.spkSel || orgSelCfg.spkMonoSel)
        {
            if(modeParam->curUser == HAL_AUD_USER_LINEIN)
            {
                lineInSetting |= ABB_LINEIN_TO_PA_MODE;
            }

            // A. muxin enable
            micSetting |= ABB_MUXIN_LEFT_EN_DR|ABB_MUXIN_LEFT_EN_REG|
                          ABB_MUXIN_RIGHT_EN_DR|ABB_MUXIN_RIGHT_EN_REG;

            // B. normal/mux mic disable
            micSetting |= ABB_MICIN_IBPGA_EN_DR;
            micSetting &= ~ABB_MICIN_IBPGA_EN_REG;
        }
        else // line-in mute
        {
            // A. muxin enable
            micSetting |= ABB_MUXIN_LEFT_EN_DR| ABB_MUXIN_RIGHT_EN_DR;
            // TODO: Need to mute the speaker DAC line too?
        }
    }

    hal_AbbRegWrite(CODEC_LINEIN_SETTING, lineInSetting);
    hal_AbbRegWrite(CODEC_MIC_SETTING, micSetting);
#ifdef MIC_MEMS_MODE
    hal_AbbRegRead(CODEC_PLL_SETTING1, &micSetting);
    micSetting |= (ABB_BOOST_BP_MIC_DR | ABB_BOOST_BP_MIC_REG);
    hal_AbbRegWrite(CODEC_PLL_SETTING1, micSetting);
#endif //MIC_MEMS_MODE

#ifdef RECV_USE_SPK_LINE
    HAL_AUD_SEL_T selCfg = modeParam->curSelCfg;
    UINT32 ldoSetting1 = 0;
    if (selCfg.spkMonoSel || selCfg.spkSel)
    {
        // Direct-reg codec dac control
        ldoSetting1 |= ABB_PD_DAC_L_DR;
    }
    else
        ldoSetting1 |= ABB_PD_DAC_L_DR|ABB_PD_DAC_L_REG;

    // 3-in-1 or 2-in-1 speaker has one physical speaker only, and
    // there is no need to mute DAC line
#if !defined(AUD_3_IN_1_SPK) && !defined(AUD_2_IN_1_SPK)
    // DAC control
    // (if headphone outputs sound along with speaker, it will be affected too)
    hal_AbbRegWrite(CODEC_LDO_SETTING1, ldoSetting1);
#endif
    // Speaker PA control
    if (selCfg.spkMonoSel || selCfg.spkSel)
    {
        pmd_SpeakerPaSelectChannel(modeParam->spkLeftChanSel,
                                   modeParam->spkRightChanSel);
    }
    else
    {
        pmd_SpeakerPaSelectChannel(FALSE, FALSE);
    }
#endif // RECV_USE_SPK_LINE
}

PUBLIC UINT32 hal_AudDacGainDb2Val(INT32 db)
{
    UINT32 value;
    // DacVolume[6:2] = 0 ... 31 = -26dB ... 5dB
    if (db < -26)
        db = -26;
    else if (db > 5)
        db = 5;
    value = db + 26;
    return value;
}

PUBLIC UINT32 hal_AudAdcGainDb2Val(INT32 db)
{
    UINT32 value;
    // MIC AdcVolume[12:9] = 0 ... 15 = MUTE -12dB -10dB ... 14dB 16dB
    if (db > 16)
        db = 16;
    if (db < -12)
        value = 0; // MIC ADC MUTE
    else
        value = (db + 12) / 2 + 1;
    return value;
}

#endif // !_FLASH_PROGRAMMER && !_T_UPGRADE_PROGRAMMER


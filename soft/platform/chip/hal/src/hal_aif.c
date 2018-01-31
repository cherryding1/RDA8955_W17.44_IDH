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
#include "chip_id.h"
#include "global_macros.h"
#include "aif.h"
#include "cfg_regs.h"
#include "sys_ctrl.h"
#include "reg_alias.h"

#ifdef CHIP_HAS_AP
#include "sys_ctrl_ap.h"
#include "apb1_ifc.h"
#elif defined(CHIP_HAS_SYS_IFC2)
#include "sys_ifc2.h"
#else
#include "bb_ifc.h"
#endif

#include "hal_aif.h"
#include "hal_clk.h"
#include "hal_host.h"
#include "hal_error.h"
#include "hal_rda_abb.h"
#include "halp_aif.h"
#include "halp_debug.h"
#include "halp_sys.h"



// =============================================================================
//  MACROS
// =============================================================================

#ifdef CHIP_HAS_AP
#define hwp_aif hwp_apAif
#endif


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

#if defined (CHIP_DIE_8955)
#define AIF_SOURCE_CLOCK 52000000
#else
#define AIF_SOURCE_CLOCK 48000000
#endif

#define VOICE_BCK_LRCK_RATIO_MIN 16
#define VOICE_BCK_LRCK_RATIO_MAX 47
#define AUDIO_BCK_LRCK_RATIO_MIN 32
#define AUDIO_BCK_LRCK_RATIO_MAX 94

#ifdef FPGA
#undef FAST_CLOCK_156M
#ifdef FPGA_SLOW
#define FAST_CLOCK_156M 39000000
#else
#define FAST_CLOCK_156M 52000000
#endif
#endif // FPGA


// =============================================================================
// AIF_FREQ
// -----------------------------------------------------------------------------
/// Defines for readable tone
// =============================================================================
#define AIF_FREQ_1                ( AIF_DTMF_FREQ_ROW_697_HZ | AIF_DTMF_FREQ_COL_1209_HZ )
#define AIF_FREQ_2                ( AIF_DTMF_FREQ_ROW_697_HZ | AIF_DTMF_FREQ_COL_1336_HZ )
#define AIF_FREQ_3                ( AIF_DTMF_FREQ_ROW_697_HZ | AIF_DTMF_FREQ_COL_1477_HZ )
#define AIF_FREQ_A                ( AIF_DTMF_FREQ_ROW_697_HZ | AIF_DTMF_FREQ_COL_1633_HZ )
#define AIF_FREQ_4                ( AIF_DTMF_FREQ_ROW_770_HZ | AIF_DTMF_FREQ_COL_1209_HZ )
#define AIF_FREQ_5                ( AIF_DTMF_FREQ_ROW_770_HZ | AIF_DTMF_FREQ_COL_1336_HZ )
#define AIF_FREQ_6                ( AIF_DTMF_FREQ_ROW_770_HZ | AIF_DTMF_FREQ_COL_1477_HZ )
#define AIF_FREQ_B                ( AIF_DTMF_FREQ_ROW_770_HZ | AIF_DTMF_FREQ_COL_1633_HZ )
#define AIF_FREQ_7                ( AIF_DTMF_FREQ_ROW_852_HZ | AIF_DTMF_FREQ_COL_1209_HZ )
#define AIF_FREQ_8                ( AIF_DTMF_FREQ_ROW_852_HZ | AIF_DTMF_FREQ_COL_1336_HZ )
#define AIF_FREQ_9                ( AIF_DTMF_FREQ_ROW_852_HZ | AIF_DTMF_FREQ_COL_1477_HZ )
#define AIF_FREQ_C                ( AIF_DTMF_FREQ_ROW_852_HZ | AIF_DTMF_FREQ_COL_1633_HZ )
#define AIF_FREQ_S                ( AIF_DTMF_FREQ_ROW_941_HZ | AIF_DTMF_FREQ_COL_1209_HZ )
#define AIF_FREQ_0                ( AIF_DTMF_FREQ_ROW_941_HZ | AIF_DTMF_FREQ_COL_1336_HZ )
#define AIF_FREQ_P                ( AIF_DTMF_FREQ_ROW_941_HZ | AIF_DTMF_FREQ_COL_1477_HZ )
#define AIF_FREQ_D                ( AIF_DTMF_FREQ_ROW_941_HZ | AIF_DTMF_FREQ_COL_1633_HZ )






// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
/// Kept in mind the openness status of the AIF
PRIVATE BOOL g_halAifOpened
    = FALSE;

/// User handler called when the playing reaches the middle of the buffer
PRIVATE  HAL_AIF_XFER_HANDLER_T g_halAifHandlers[2]
=
{{NULL, NULL}, {NULL, NULL}};


/// Global variable to remember the state of the Rx and Tx interfaces, to
/// deal with a hardware feature ...
PRIVATE  BOOL g_halAifPlaying  = FALSE;
PRIVATE  BOOL g_halAifRecording  = FALSE;

/// Use to store the constants in the control register
PRIVATE UINT32 g_halAifControlReg  = 0;

HAL_CLK_T *gAifClk = NULL;


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_AifSetSideTone
// -----------------------------------------------------------------------------
/// Set the side-tone volume.
/// @param vol Ume.
// =============================================================================
PUBLIC VOID hal_AifSetSideTone(UINT32 vol)
{
    hwp_aif->side_tone = AIF_SIDE_TONE_GAIN(vol);
}


#if defined (CHIP_DIE_8955)

PRIVATE UINT32 hal_AifGetClkDivider(HAL_AIF_TX_STB_SEL_T clksel,  CONST HAL_AIF_FREQ_T  sampleRate)
{

    UINT32 clkDivider = 0;


    if(HAL_AIF_TX_STB_SEL_CODEC == clksel)
    {
        // Formula: AIF_SOURCE_CLOCK / config->sampleRate - 2;
        // AIF_SOURCE_CLOCK is determined during audio codec configuration
        // in aud_CodecCommonCodecPrePowerOnConfig().
        switch (sampleRate)
        {
        // AIF_SOURCE_CLOCK = 24576000
            case HAL_AIF_FREQ_8000HZ:
                clkDivider = 1536;
                break;
            case HAL_AIF_FREQ_12000HZ:
                clkDivider = 1024;
                break;
            case HAL_AIF_FREQ_16000HZ:
                clkDivider = 768;
                break;
            case HAL_AIF_FREQ_24000HZ:
                clkDivider = 512;
                break;
            case HAL_AIF_FREQ_32000HZ:
                clkDivider = 384;
                break;
            case HAL_AIF_FREQ_48000HZ:
                clkDivider = 256;
                break;
        // AIF_SOURCE_CLOCK = 22579200
            case HAL_AIF_FREQ_11025HZ:
               clkDivider = 1024;
               break;
            case HAL_AIF_FREQ_22050HZ:
               clkDivider = 512;
               break;
            case HAL_AIF_FREQ_44100HZ:
                clkDivider = 256;
                break;
            default:
                HAL_ASSERT(FALSE, "Invalid sample rate: %d", sampleRate);
                break;
        }
    }
    else if(HAL_AIF_TX_STB_SEL_XTAL)
    {
        clkDivider = AIF_SOURCE_CLOCK / sampleRate ;

    }
    else //to do
    {
        clkDivider = AIF_SOURCE_CLOCK / sampleRate ;
    }

    return clkDivider;
}
#elif defined(CHIP_DIE_8809E2)
PRIVATE UINT32 hal_AifGetClkDivider(HAL_AIF_TX_STB_SEL_T clksel,  CONST HAL_AIF_FREQ_T  sampleRate)
{

    UINT32 clkDivider = 0;


    // Formula: AIF_SOURCE_CLOCK / config->sampleRate - 2;
    // AIF_SOURCE_CLOCK is determined during audio codec configuration
    // in aud_CodecCommonCodecPrePowerOnConfig().
    switch (sampleRate)
    {
        // AIF_SOURCE_CLOCK = 24576000
        case HAL_AIF_FREQ_8000HZ:
            clkDivider = 1534;
            break;
        case HAL_AIF_FREQ_12000HZ:
            clkDivider = 1022;
            break;
        case HAL_AIF_FREQ_16000HZ:
            clkDivider = 766;
            break;
        case HAL_AIF_FREQ_24000HZ:
            clkDivider = 510;
            break;
        case HAL_AIF_FREQ_32000HZ:
            clkDivider = 382;
            break;
        case HAL_AIF_FREQ_48000HZ:
            clkDivider = 254;
            break;
        // AIF_SOURCE_CLOCK = 22579200
        case HAL_AIF_FREQ_11025HZ:
            clkDivider = 1022;
            break;
        case HAL_AIF_FREQ_22050HZ:
            clkDivider = 510;
            break;
        case HAL_AIF_FREQ_44100HZ:
            clkDivider = 254;
            break;
        default:
            HAL_ASSERT(FALSE, "Invalid sample rate: %d", sampleRate);
            break;
    }

    return clkDivider;
}
#endif
// =============================================================================
// hal_AifOpen
// -----------------------------------------------------------------------------
/// Open the AIF and set the appropriate config.
/// When the analog interface is used, leave the \c serialCfg field of the
/// \c config parameter at 0.
///
/// @param config Pointer to the configuration of the Audio InterFace.
/// @return HAL_ERR_NO if everything is alright or HAL_ERR_RESOURCE_BUSY if
/// the AIF has already been opened.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifOpen(CONST HAL_AIF_CONFIG_T* config)
{
    UINT32 controlReg = 0;
    UINT32 serialCfgReg = 0;
    UINT32 lrck;
    UINT32 bck;
    UINT32 clkDivider;
    UINT32 bckLrckDivider;

    // Pointer to the serial config
    HAL_AIF_SERIAL_CFG_T* serialCfg;


    // Already opened ?
    if (g_halAifOpened == TRUE)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    //  set the resource as active
    hal_ClkEnable(gAifClk);

#ifndef CHIP_HAS_AP
    // Unrest AIF module.
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Sys_Mode |= SYS_CTRL_MODE_SYS_PCLK_CONF_MANUAL
                                 |SYS_CTRL_MODE_SYS_PCLK_DATA_MANUAL
                                 |SYS_CTRL_MODE_SYS_AMBA_MANUAL;

#if defined (CHIP_DIE_8955)
    hwp_sysCtrl->BB_Rst_Clr =  SYS_CTRL_CLR_SYS_MSB_RST(SYS_CTRL_CLR_RST_AIF);
#else
    hwp_sysCtrl->BB_Rst_Clr =  SYS_CTRL_CLR_BB_RST(SYS_CTRL_CLR_RST_AIF);
#endif

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) && defined(GALLITE_IS_8805)

    if (config->sampleRate < HAL_AIF_FREQ_12000HZ)
    {
        hal_AbbEnableAifFreqDiv2(TRUE);
        clkDivider = AIF_SOURCE_CLOCK / 2 / config->sampleRate - 2;
    }
    else
    {
        clkDivider = AIF_SOURCE_CLOCK / config->sampleRate - 2;
    }

#elif  defined(GALLITE_IS_8806) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)

#if defined (CHIP_DIE_8955)
    clkDivider = hal_AifGetClkDivider(HAL_AIF_TX_STB_SEL_CODEC,    config->sampleRate);
#elif defined(CHIP_DIE_8809E2)
    clkDivider = hal_AifGetClkDivider(HAL_AIF_TX_STB_SEL_CODEC,    config->sampleRate);
#else
    clkDivider = AIF_SOURCE_CLOCK / config->sampleRate - 2;
#endif

#else // smart phones, 8809P or later feature phones

    clkDivider = 0;
    // Formula: AIF_SOURCE_CLOCK / config->sampleRate - 2;
    // AIF_SOURCE_CLOCK is determined during audio codec configuration
    // in aud_CodecCommonCodecPrePowerOnConfig().
    switch (config->sampleRate)
    {
        // AIF_SOURCE_CLOCK = 24576000
        case HAL_AIF_FREQ_8000HZ:
            clkDivider = 3070;
            break;
        case HAL_AIF_FREQ_12000HZ:
            clkDivider = 2046;
            break;
        case HAL_AIF_FREQ_16000HZ:
            clkDivider = 1534;
            break;
        case HAL_AIF_FREQ_24000HZ:
            clkDivider = 1022;
            break;
        case HAL_AIF_FREQ_32000HZ:
            clkDivider = 766;
            break;
        case HAL_AIF_FREQ_48000HZ:
            clkDivider = 510;
            break;
        // AIF_SOURCE_CLOCK = 22579200
        case HAL_AIF_FREQ_11025HZ:
            clkDivider = 2046;
            break;
        case HAL_AIF_FREQ_22050HZ:
            clkDivider = 1022;
            break;
        case HAL_AIF_FREQ_44100HZ:
            clkDivider = 510;
            break;
        default:
            HAL_ASSERT(FALSE, "Invalid sample rate: %d", config->sampleRate);
            break;
    }

#endif

#ifdef CHIP_HAS_AP
    hwp_aif->Cfg_Aif_Tx_Stb = AIF_AIF_TX_STB_EN
                              | AIF_AIF_TX_STB_DIV(clkDivider);
#else
    hwp_sysCtrl->Cfg_Aif_Tx_Stb_Div = SYS_CTRL_AIF_TX_STB_EN
#if defined (CHIP_DIE_8955)
                                      |SYS_CTRL_AIF_TX_STB_SEL(HAL_AIF_TX_STB_SEL_CODEC)
#endif
                                      | SYS_CTRL_AIF_TX_STB_DIV(clkDivider);
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8809) // 8809P or later feature phones
    hwp_sysCtrl->Cfg_Aif_Tx_Stb_Div |= SYS_CTRL_AIF_TX_STB_CODEC_EN;
#endif

#endif

    if (config->interface == HAL_AIF_IF_PARALLEL)
    {
        // Analog mode
        controlReg |= AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA | AIF_LOOP_BACK_NORMAL;
        serialCfgReg |= AIF_MASTER_MODE_MASTER; // Well, it's analog mode ...
#ifdef SUPPORT_DUAL_MIC
#ifdef CHIP_HAS_AP
        controlReg |= AIF_PARALLEL_IN2_EN_ENABLE;
#endif
#endif
    }
    else if (config->interface == HAL_AIF_IF_PARALLEL_STEREO)
    {
#if (CHIP_HAS_STEREO_DAC == 1)
        controlReg |= AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA | AIF_LOOP_BACK_NORMAL | AIF_TX_STB_MODE;
        // Parallel stereo out expects stereo data
        if (config->channelNb == HAL_AIF_MONO)
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_MONO_STEREO_DUPLI;
        }
        else
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_STEREO_STEREO;
        }
#ifdef CHIP_HAS_AP
#ifdef SUPPORT_DUAL_MIC
        controlReg |= AIF_PARALLEL_IN2_EN_ENABLE;
#endif
#endif

#else
        HAL_ASSERT(FALSE, "This chip has not the Parallel Stereo Interface.");
#endif
    }
    else
    {
        // Digital (serial) modes
        HAL_ASSERT(config->serialCfg != NULL, "hal_AifOpen in serial mode without serial config\n");

        if (config->interface == HAL_AIF_IF_SERIAL ||
                config->interface == HAL_AIF_IF_SERIAL_1 ||
                config->interface == HAL_AIF_IF_SERIAL_2)
        {
            // Serial in and serial out
            controlReg |=  AIF_PARALLEL_OUT_CLR_PARA |
                           AIF_PARALLEL_IN_CLR_PARA |
                           AIF_LOOP_BACK_NORMAL;
        }
        else
        {
            // Serial in and parallel out
            controlReg |=  AIF_PARALLEL_OUT_SET_PARA |
                           AIF_PARALLEL_IN_CLR_PARA |
                           AIF_LOOP_BACK_NORMAL|
                           AIF_TX_STB_MODE;
            // Parallel stereo out expects stereo data
            if (config->channelNb == HAL_AIF_MONO)
            {
                serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_MONO_STEREO_DUPLI;
            }
            else
            {
                serialCfgReg |= AIF_MASTER_MODE_MASTER | AIF_TX_MODE_STEREO_STEREO;
            }
        }

        // Mode
        serialCfg = config->serialCfg;
        HAL_ASSERT((serialCfg->mode < HAL_SERIAL_MODE_QTY),
                   "Improper serial mode");
        serialCfgReg |= AIF_SERIAL_MODE(serialCfg->mode);

        // TODO FIXME Implement the clock configuration
        // here from the sample rate given at the aif opening

        // FIXME
#if (CHIP_HAS_I2S_DI_1 == 1)
        if (config->interface == HAL_AIF_IF_SERIAL_2 ||
                config->interface == HAL_AIF_IF_SERIAL_2_IN_PARALLEL_OUT)
        {
            serialCfgReg |= AIF_I2S_IN_SEL_I2S_IN_2;
        }
        else if (config->interface == HAL_AIF_IF_SERIAL_1 ||
                 config->interface == HAL_AIF_IF_SERIAL_1_IN_PARALLEL_OUT)
        {
            serialCfgReg |= AIF_I2S_IN_SEL_I2S_IN_1;
        }
        else // HAL_AIF_IF_SERIAL or HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT
        {
            serialCfgReg |= AIF_I2S_IN_SEL_I2S_IN_0;
        }
#else
        HAL_ASSERT(config->interface == HAL_AIF_IF_SERIAL ||
                   config->interface == HAL_AIF_IF_SERIAL_IN_PARALLEL_OUT,
                   "There is only one serial interface available");

#endif
        // Master
        if (serialCfg->aifIsMaster)
        {
            serialCfgReg |= AIF_MASTER_MODE_MASTER;
        }
        else
        {
            serialCfgReg |= AIF_MASTER_MODE_SLAVE;
        }

        // LSB first
        if (serialCfg->lsbFirst)
        {
            serialCfgReg |= AIF_LSB_LSB;
        }
        else
        {
            serialCfgReg |= AIF_LSB_MSB;
        }

        // LRCK polarity
        if (serialCfg->polarity)
        {
            serialCfgReg |= AIF_LRCK_POL_LEFT_L_RIGHT_H;
        }
        else
        {
            serialCfgReg |= AIF_LRCK_POL_LEFT_H_RIGHT_L;
        }

        // Rx delay
        HAL_ASSERT(serialCfg->rxDelay<HAL_AIF_RX_DELAY_QTY,
                   "Improper delay for serial Rx");
        serialCfgReg |= AIF_RX_DLY(serialCfg->rxDelay);

        // Tx delay
        if (serialCfg->txDelay == HAL_AIF_TX_DELAY_ALIGN)
        {
            serialCfgReg |= AIF_TX_DLY_ALIGN;
        }
        else
        {
            serialCfgReg |= AIF_TX_DLY_DLY_1;
        }

        // Rx Mode
        if (serialCfg->rxMode == HAL_AIF_RX_MODE_STEREO_STEREO)
        {
            serialCfgReg |= AIF_RX_MODE_STEREO_STEREO;
        }
        else
        {
            serialCfgReg |= AIF_RX_MODE_STEREO_MONO_FROM_L;
        }

        // Tx Mode
        HAL_ASSERT(serialCfg->txMode < HAL_AIF_TX_MODE_QTY,
                   "Improper mode for serial Tx" );
        serialCfgReg |= AIF_TX_MODE(serialCfg->txMode);

        // LRCK frequency ...
        lrck = serialCfg->fs;
        bck  = lrck * serialCfg->bckLrckRatio;

        // We get the BCK from the the audio clock.
#ifdef CHIP_HAS_AP
        UINT32 clockSource = 0;

        if(HAL_AIF_FREQ_8000HZ == lrck || HAL_AIF_FREQ_12000HZ == lrck
                || HAL_AIF_FREQ_16000HZ == lrck || HAL_AIF_FREQ_24000HZ == lrck
                || HAL_AIF_FREQ_32000HZ == lrck || HAL_AIF_FREQ_48000HZ == lrck)
        {
            clockSource = 24576000;
        }
        else
        {
            clockSource = 22579200;
        }

        clkDivider = (clockSource / bck) - 2;

        if (serialCfg->invertBck)
            hwp_aif->Cfg_Clk_AudioBCK = AIF_AUDIOBCK_DIVIDER(clkDivider) | AIF_BCK_PLL_SOURCE_PLL_CODEC | AIF_BCK_POL_INVERT;
        else
            hwp_aif->Cfg_Clk_AudioBCK = AIF_AUDIOBCK_DIVIDER(clkDivider) | AIF_BCK_PLL_SOURCE_PLL_CODEC | AIF_BCK_POL_NORMAL;
#else
        clkDivider = (FAST_CLOCK_156M / bck) - 2;

        hwp_sysCtrl->Cfg_Clk_AudioBCK_Div = SYS_CTRL_AUDIOBCK_DIVIDER(clkDivider);
#endif

        // The ratio is calculated differently depending
        // on whether we're using voice or audio mode.
        if (serialCfg->mode == HAL_SERIAL_MODE_VOICE)
        {
            // Check the ratio
            HAL_ASSERT(serialCfg->bckLrckRatio >= VOICE_BCK_LRCK_RATIO_MIN
                       && serialCfg->bckLrckRatio<=VOICE_BCK_LRCK_RATIO_MAX,
                       "HAL: AIF: In voice mode, ratio is from %d to %d, %d unsupported",
                       VOICE_BCK_LRCK_RATIO_MIN,
                       VOICE_BCK_LRCK_RATIO_MAX,
                       serialCfg->bckLrckRatio);
            bckLrckDivider = serialCfg->bckLrckRatio - 16;
        }
        else
        {
            HAL_ASSERT(serialCfg->bckLrckRatio >= AUDIO_BCK_LRCK_RATIO_MIN
                       && serialCfg->bckLrckRatio<=AUDIO_BCK_LRCK_RATIO_MAX,
                       "HAL: AIF: In audio mode, ratio is from %d to %d, %d unsupported",
                       AUDIO_BCK_LRCK_RATIO_MIN,
                       AUDIO_BCK_LRCK_RATIO_MAX,
                       serialCfg->bckLrckRatio);
            bckLrckDivider = serialCfg->bckLrckRatio/2 - 16;
        }

        serialCfgReg |= AIF_BCK_LRCK(bckLrckDivider);

        // BCK polarity
#ifndef CHIP_HAS_AP
        if (serialCfg->invertBck)
        {
            serialCfgReg |= AIF_BCK_POL_INVERT;
        }
        else
        {
            serialCfgReg |= AIF_BCK_POL_NORMAL;
        }
#endif

        // Output Half Cycle Delay
        if (serialCfg->outputHalfCycleDelay)
        {
            serialCfgReg |= AIF_OUTPUT_HALF_CYCLE_DLY_DLY;
        }
        else
        {
            serialCfgReg |= AIF_OUTPUT_HALF_CYCLE_DLY_NO_DLY;
        }

        // Input Half Cycle Delay
        if (serialCfg->inputHalfCycleDelay)
        {
            serialCfgReg |= AIF_INPUT_HALF_CYCLE_DLY_DLY;
        }
        else
        {
            serialCfgReg |= AIF_INPUT_HALF_CYCLE_DLY_NO_DLY;
        }

        // BckOut gating
        if (serialCfg->enableBckOutGating)
        {
            serialCfgReg |= AIF_BCKOUT_GATE_GATED;
        }
        else
        {
            serialCfgReg |= AIF_BCKOUT_GATE_NO_GATE;
        }
    }

    // Initializes global variables
    g_halAifHandlers[RECORD].halfHandler   = NULL;
    g_halAifHandlers[RECORD].endHandler    = NULL;
    g_halAifHandlers[PLAY].halfHandler     = NULL;
    g_halAifHandlers[PLAY].endHandler      = NULL;

    g_halAifPlaying      = FALSE;
    g_halAifRecording    = FALSE;

#if defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955)
    hwp_aif->load_pos = AIF_RX_LOAD_POS(14);
#endif
    // Write register
    hwp_aif->serial_ctrl = serialCfgReg;
    // hwp_aif->ctrl = controlReg;
    g_halAifControlReg   = controlReg;

    return HAL_ERR_NO;

}

// =============================================================================
// hal_AifLoopBack
// -----------------------------------------------------------------------------
/// Set AIF loopback mode
// =============================================================================
PUBLIC VOID hal_AifLoopBack(BOOL loop)
{
    if(loop == TRUE)
    {
        g_halAifControlReg |= AIF_LOOP_BACK;
        hwp_aif->ctrl =
            (hwp_aif->ctrl & (~(AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW)))
            | AIF_LOOP_BACK;
    }
    else
    {
        g_halAifControlReg &= ~(AIF_LOOP_BACK);
        hwp_aif->ctrl = hwp_aif->ctrl &
                        ~(AIF_LOOP_BACK | AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW);
    }
}

// =============================================================================
// hal_AifClose
// -----------------------------------------------------------------------------
/// Close the AIF.
// =============================================================================
PUBLIC VOID hal_AifClose(VOID)
{
    hwp_aif->ctrl        = 0;
    hwp_aif->serial_ctrl = AIF_MASTER_MODE_MASTER;
    hwp_aif->side_tone   = 0;

#ifdef CHIP_HAS_AP
    hwp_aif->Cfg_Aif_Tx_Stb = 0;
#else // !CHIP_HAS_AP
    hwp_sysCtrl->Cfg_Aif_Tx_Stb_Div = 0;

    // Reset AIF module, to force output lines (I2S clocks and data lines)
    // to be low.
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Sys_Mode &= ~(SYS_CTRL_MODE_SYS_PCLK_CONF_MANUAL|SYS_CTRL_MODE_SYS_AMBA_MANUAL|SYS_CTRL_MODE_SYS_PCLK_DATA_MANUAL);
    hwp_sysCtrl->BB_Rst_Set=SYS_CTRL_SET_RST_AIF;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SysExitCriticalSection(status);
#endif // !CHIP_HAS_AP

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) && defined(GALLITE_IS_8805)
    hal_AbbEnableAifFreqDiv2(FALSE);
#endif

    g_halAifOpened       = FALSE;

    g_halAifPlaying      = FALSE;
    g_halAifRecording    = FALSE;

    //  Release the resource
    hal_ClkDisable(gAifClk);
}




// =============================================================================
// hal_AifStream
// -----------------------------------------------------------------------------
/// Play or record a stream from or to a buffer
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes.
/// @param xfer Describes the buffer and size
/// @param direction Define the direction: PLAY for play, RECORD to record.
// =============================================================================
PRIVATE HAL_ERR_T hal_AifStream(CONST HAL_AIF_STREAM_T* xfer, UINT32 direction)
{
    UINT32 irqMask = 0;

    if (!hal_ClkIsEnabled(gAifClk))
    {
        HAL_ASSERT(FALSE, "AIF Stream when resource not active");
    }


#ifdef CHIP_HAS_AP
    if (hwp_auIfc->ch[direction].status & APB1_IFC_ENABLE)
#elif defined(CHIP_HAS_SYS_IFC2)
    if (hwp_sysIfc2->ch[direction].status & SYS_IFC2_ENABLE)
#else
    if (hwp_bbIfc->ch[direction].status & BB_IFC_ENABLE)
#endif
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    // Assert on word alignement
    HAL_ASSERT(((UINT32)xfer->startAddress)%4 == 0, "BB IFC transfer start \
address not aligned: 0x%x",((UINT32)xfer->startAddress));

    // Size must be a multiple of 32 bytes
    HAL_ASSERT((xfer->length)%32 == 0, "BB IFC transfer size not mult. of 32-bits");

#ifdef CHIP_HAS_AP
    // convert modem-seen psram address to ap-seen param address
    hwp_auIfc->ch[direction].start_addr = (((UINT32)xfer->startAddress & 0x0FFFFFFF) | 0x10000000);
    hwp_auIfc->ch[direction].Fifo_Size  = xfer->length;
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[direction].start_addr = (UINT32) xfer->startAddress;
    hwp_sysIfc2->ch[direction].Fifo_Size  = xfer->length;
#else
    hwp_bbIfc->ch[direction].start_addr = (UINT32) xfer->startAddress;
    hwp_bbIfc->ch[direction].Fifo_Size  = xfer->length;
#endif

    if (xfer->halfHandler != NULL)
    {
        g_halAifHandlers[direction].halfHandler = xfer->halfHandler;
#ifdef CHIP_HAS_AP
        irqMask |= APB1_IFC_HALF_FIFO;
#elif defined(CHIP_HAS_SYS_IFC2)
        irqMask |= SYS_IFC2_HALF_FIFO;
#else
        irqMask |= BB_IFC_HALF_FIFO;
#endif
    }
    else
    {
        g_halAifHandlers[direction].halfHandler = NULL;
    }

    if (xfer->endHandler != NULL)
    {
        g_halAifHandlers[direction].endHandler = xfer->endHandler;
#ifdef CHIP_HAS_AP
        irqMask |= APB1_IFC_END_FIFO;
#elif defined(CHIP_HAS_SYS_IFC2)
        irqMask |= SYS_IFC2_END_FIFO;
#else
        irqMask |= BB_IFC_END_FIFO;
#endif
    }
    else
    {
        g_halAifHandlers[direction].endHandler = NULL;
    }

#ifdef CHIP_HAS_AP
    hwp_auIfc->ch[direction].int_mask = irqMask;
    // Enable audio IFC IRQs on modem side
    if (direction == RECORD)
    {
        hwp_apSysCtrl->Cfg_Reserve &= ~SYS_CTRL_AP_AUIFC_CH0_IRQ_MASK;
        hwp_sysCtrl->Cfg_Reserve |= SYS_CTRL_AUIFC_CH0_IRQ_MASK;
    }
    else
    {
        hwp_apSysCtrl->Cfg_Reserve &= ~SYS_CTRL_AP_AUIFC_CH1_IRQ_MASK;
        hwp_sysCtrl->Cfg_Reserve |= SYS_CTRL_AUIFC_CH1_IRQ_MASK;
    }
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[direction].int_mask = irqMask;
#else
    hwp_bbIfc->ch[direction].int_mask = irqMask;
#endif

    if (xfer->playSyncWithRecord)
    {
        if (direction == PLAY)
        {
            // Not to start the play stream
            return HAL_ERR_NO;
        }
        else if (direction == RECORD)
        {
            // Start the play stream just before starting the record one
#ifdef CHIP_HAS_AP
            hwp_auIfc->ch[PLAY].control  = APB1_IFC_ENABLE;
#elif defined(CHIP_HAS_SYS_IFC2)
            hwp_sysIfc2->ch[PLAY].control  = SYS_IFC2_ENABLE;
#else
            hwp_bbIfc->ch[PLAY].control  = BB_IFC_ENABLE;
#endif
        }
    }

#ifdef CHIP_HAS_AP
    hwp_auIfc->ch[direction].control  = APB1_IFC_ENABLE;
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[direction].control  = SYS_IFC2_ENABLE;
#else
    hwp_bbIfc->ch[direction].control  = BB_IFC_ENABLE;
#endif

    return HAL_ERR_NO;
}


// =============================================================================
// hal_AifPlayStream
// -----------------------------------------------------------------------------
/// Play a stream, copied from a buffer in memory to the AIF fifos, in infinite
/// mode: when the end of the buffer is reached, playing continues from the
/// beginning.
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes.
///
/// @param playedStream Pointer to the played stream. A stream pointing to
/// a NULL buffer (startAddress field) only enable the audio, without
/// playing data from anywhere.
/// @return HAL_ERR_NO if everything is alright or HAL_ERR_RESOURCE_BUSY if
/// a play is already in process.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPlayStream(CONST HAL_AIF_STREAM_T* playedStream)
{
    UINT32 status;
    HAL_ERR_T errStatus = HAL_ERR_NO;

    if (!hal_ClkIsEnabled(gAifClk))
    {
        HAL_ASSERT(FALSE, "AIF Play Stream when resource not active");
    }


    status = hal_SysEnterCriticalSection();
    if (playedStream->startAddress != NULL)
    {
        errStatus = hal_AifStream((HAL_AIF_STREAM_T*) playedStream, PLAY);
    }

    if (errStatus == HAL_ERR_NO)
    {
        g_halAifPlaying = TRUE;
        // Allow symbols to be sent
        hwp_aif->ctrl = (g_halAifControlReg | AIF_ENABLE_H_ENABLE) & ~AIF_TX_OFF;
    }
    hal_SysExitCriticalSection(status);

    return errStatus;
}




// =============================================================================
// hal_AifRecordStream
// -----------------------------------------------------------------------------
/// Record a stream, copied from the AIF fifos to a buffer in memory, in infinite
/// mode: when the end of the buffer is reached, playing continues from the
/// beginning.
/// The buffer start address must be aligned on a 32-bit address, and the size
/// must be a multiple of 32 bytes.
///
/// @param recordedStream Pointer to the recorded stream. A stream pointing to
/// a NULL buffer (startAddress field) only enable
/// the audio, without recording data from anywhere.
///
// =============================================================================
PUBLIC HAL_ERR_T hal_AifRecordStream(CONST HAL_AIF_STREAM_T* recordedStream)
{
    UINT32 status;
    HAL_ERR_T errStatus = HAL_ERR_NO;

    if (!hal_ClkIsEnabled(gAifClk))
    {
        HAL_ASSERT(FALSE, "AIF Record Stream when resource not active");
    }

    status = hal_SysEnterCriticalSection();
    if (recordedStream->startAddress != NULL)
    {
        errStatus = hal_AifStream(recordedStream, RECORD);
    }

    if (errStatus == HAL_ERR_NO)
    {
        g_halAifRecording = TRUE;
        // In Loopback mode, do not start now - the Tx will do it
        // This is usefull for synchronization purpose
        if ((!g_halAifPlaying) && ((g_halAifControlReg & AIF_LOOP_BACK)==0))
        {
            // Need to start the clock: write 4 data not to send
            hwp_aif->ctrl = g_halAifControlReg | AIF_ENABLE_H_ENABLE | AIF_TX_OFF_TX_OFF;
            hwp_aif->data = 0;
            hwp_aif->data = 0;
            hwp_aif->data = 0;
            hwp_aif->data = 0;
        }


    }
    hal_SysExitCriticalSection(status);

    return errStatus;

}



// =============================================================================
// hal_AifStopPlay
// -----------------------------------------------------------------------------
/// Stop playing a buffer
// =============================================================================
PUBLIC VOID hal_AifStopPlay(VOID)
{
    g_halAifPlaying = FALSE;
    // More caution needed ?
#ifdef CHIP_HAS_AP
    hwp_auIfc->ch[PLAY].control        = APB1_IFC_DISABLE;
    hwp_auIfc->ch[PLAY].int_mask       = 0;
    // Disable audio IFC IRQs on modem side
    hwp_sysCtrl->Cfg_Reserve &= ~SYS_CTRL_AUIFC_CH1_IRQ_MASK;
    hwp_apSysCtrl->Cfg_Reserve |= SYS_CTRL_AP_AUIFC_CH1_IRQ_MASK;
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[PLAY].control        = SYS_IFC2_DISABLE;
    hwp_sysIfc2->ch[PLAY].int_mask       = 0;
#else
    hwp_bbIfc->ch[PLAY].control        = BB_IFC_DISABLE;
    hwp_bbIfc->ch[PLAY].int_mask       = 0;
#endif
    g_halAifHandlers[PLAY].halfHandler = NULL;
    g_halAifHandlers[PLAY].endHandler  = NULL;

    // Disable the AIF if not recording
    UINT32 status = hal_SysEnterCriticalSection();
    if (!g_halAifRecording)
    {
        // To have the clock allowing the disabling.
        hwp_aif->ctrl = AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA;
    }
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_AifStopRecord
// -----------------------------------------------------------------------------
/// Stop playing a buffer
// =============================================================================
PUBLIC VOID hal_AifStopRecord(VOID)
{
    g_halAifRecording                    = FALSE;

#ifdef CHIP_HAS_AP
    hwp_auIfc->ch[RECORD].control        = APB1_IFC_DISABLE;
    hwp_auIfc->ch[RECORD].int_mask       = 0;
    // Disable audio IFC IRQs on modem side
    hwp_sysCtrl->Cfg_Reserve &= ~SYS_CTRL_AUIFC_CH0_IRQ_MASK;
    hwp_apSysCtrl->Cfg_Reserve |= SYS_CTRL_AP_AUIFC_CH0_IRQ_MASK;
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[RECORD].control        = SYS_IFC2_DISABLE;
    hwp_sysIfc2->ch[RECORD].int_mask       = 0;
#else
    hwp_bbIfc->ch[RECORD].control        = BB_IFC_DISABLE;
    hwp_bbIfc->ch[RECORD].int_mask       = 0;
#endif

    g_halAifHandlers[RECORD].halfHandler = NULL;
    g_halAifHandlers[RECORD].endHandler  = NULL;

    // Disable the AIF if not recording
    UINT32 status = hal_SysEnterCriticalSection();
    if (!g_halAifPlaying)
    {
        hwp_aif->ctrl = AIF_PARALLEL_OUT_SET_PARA | AIF_PARALLEL_IN_SET_PARA ;
    }
    hal_SysExitCriticalSection(status);
}

// =============================================================================
// hal_AifPlayReachedHalf
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the middle and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifPlayReachedHalf(VOID)
{
    BOOL ret = FALSE;

#ifdef CHIP_HAS_AP
    if (hwp_auIfc->ch[PLAY].status & APB1_IFC_IHF)
    {
        hwp_auIfc->ch[PLAY].int_clear = APB1_IFC_HALF_FIFO;
        ret = TRUE;
    }
#elif defined(CHIP_HAS_SYS_IFC2)
    if (hwp_sysIfc2->ch[PLAY].status & SYS_IFC2_IHF)
    {
        hwp_sysIfc2->ch[PLAY].int_clear = SYS_IFC2_HALF_FIFO;
        ret = TRUE;
    }
#else
    if (hwp_bbIfc->ch[PLAY].status & BB_IFC_IHF)
    {
        hwp_bbIfc->ch[PLAY].int_clear = BB_IFC_HALF_FIFO;
        ret = TRUE;
    }
#endif

    return ret;
}


// =============================================================================
// hal_AifPlayReachedEnd
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the end and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifPlayReachedEnd(VOID)
{
    BOOL ret = FALSE;

#ifdef CHIP_HAS_AP
    if (hwp_auIfc->ch[PLAY].status & APB1_IFC_IEF)
    {
        hwp_auIfc->ch[PLAY].int_clear = APB1_IFC_END_FIFO;
        ret = TRUE;
    }
#elif defined(CHIP_HAS_SYS_IFC2)
    if (hwp_sysIfc2->ch[PLAY].status & SYS_IFC2_IEF)
    {
        hwp_sysIfc2->ch[PLAY].int_clear = SYS_IFC2_END_FIFO;
        ret = TRUE;
    }
#else
    if (hwp_bbIfc->ch[PLAY].status & BB_IFC_IEF)
    {
        hwp_bbIfc->ch[PLAY].int_clear = BB_IFC_END_FIFO;
        ret = TRUE;
    }
#endif

    return ret;
}



// =============================================================================
// hal_AifRecordReachedHalf
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the middle and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifRecordReachedHalf(VOID)
{
    BOOL ret = FALSE;

#ifdef CHIP_HAS_AP
    if (hwp_auIfc->ch[RECORD].status & APB1_IFC_IHF)
    {
        hwp_auIfc->ch[RECORD].int_clear = APB1_IFC_HALF_FIFO;
        ret = TRUE;
    }
#elif defined(CHIP_HAS_SYS_IFC2)
    if (hwp_sysIfc2->ch[RECORD].status & SYS_IFC2_IHF)
    {
        hwp_sysIfc2->ch[RECORD].int_clear = SYS_IFC2_HALF_FIFO;
        ret = TRUE;
    }
#else
    if (hwp_bbIfc->ch[RECORD].status & BB_IFC_IHF)
    {
        hwp_bbIfc->ch[RECORD].int_clear = BB_IFC_HALF_FIFO;
        ret = TRUE;
    }
#endif

    return ret;
}


// =============================================================================
// hal_AifRecordReachedEnd
// -----------------------------------------------------------------------------
/// Check if the buffer has reached the end and clear the status.
// =============================================================================
PUBLIC BOOL hal_AifRecordReachedEnd(VOID)
{
    BOOL ret = FALSE;

#ifdef CHIP_HAS_AP
    if (hwp_auIfc->ch[RECORD].status & APB1_IFC_IEF)
    {
        hwp_auIfc->ch[RECORD].int_clear = APB1_IFC_END_FIFO;
        ret = TRUE;
    }
#elif defined(CHIP_HAS_SYS_IFC2)
    if (hwp_sysIfc2->ch[RECORD].status & SYS_IFC2_IEF)
    {
        hwp_sysIfc2->ch[RECORD].int_clear = SYS_IFC2_END_FIFO;
        ret = TRUE;
    }
#else
    if (hwp_bbIfc->ch[RECORD].status & BB_IFC_IEF)
    {
        hwp_bbIfc->ch[RECORD].int_clear = BB_IFC_END_FIFO;
        ret = TRUE;
    }
#endif

    return ret;
}





// =============================================================================
// hal_AifIrqHandler
// -----------------------------------------------------------------------------
/// Handler called by the IRQ module when a BB-IFC interrupt occurs.
///
// =============================================================================
PROTECTED VOID hal_AifIrqHandler(UINT8 interruptId)
{
    UINT32 direction;

#ifdef CHIP_HAS_AP
    if (interruptId == SYS_IRQ_AUIFC0)
#else
    if (interruptId == SYS_IRQ_BBIFC0)
#endif
    {
        direction = RECORD;
    }
    else
    {
        direction = PLAY;
    }

    UINT32 status;
    BOOL halfFifoInt = FALSE;
    BOOL endFifoInt = FALSE;

#ifdef CHIP_HAS_AP
    status = hwp_auIfc->ch[direction].status & (APB1_IFC_CAUSE_IEF|APB1_IFC_CAUSE_IHF);
    // Clear cause
    hwp_auIfc->ch[direction].int_clear = status;

    if (status & APB1_IFC_CAUSE_IHF)
    {
        halfFifoInt = TRUE;
    }

    if (status & APB1_IFC_CAUSE_IEF)
    {
        endFifoInt = TRUE;
    }
#elif defined(CHIP_HAS_SYS_IFC2)
    status = hwp_sysIfc2->ch[direction].status & (SYS_IFC2_CAUSE_IEF|SYS_IFC2_CAUSE_IHF);
    // Clear cause
    hwp_sysIfc2->ch[direction].int_clear = status;

    if (status & SYS_IFC2_CAUSE_IHF)
    {
        halfFifoInt = TRUE;
    }

    if (status & SYS_IFC2_CAUSE_IEF)
    {
        endFifoInt = TRUE;
    }
#else
    status = hwp_bbIfc->ch[direction].status & (BB_IFC_CAUSE_IEF|BB_IFC_CAUSE_IHF);
    // Clear cause
    hwp_bbIfc->ch[direction].int_clear = status;

    if (status & BB_IFC_CAUSE_IHF)
    {
        halfFifoInt = TRUE;
    }

    if (status & BB_IFC_CAUSE_IEF)
    {
        endFifoInt = TRUE;
    }
#endif

    if (halfFifoInt)
    {
        if (g_halAifHandlers[direction].halfHandler)
        {
            g_halAifHandlers[direction].halfHandler();
        }
    }

    if (endFifoInt)
    {
        if (g_halAifHandlers[direction].endHandler)
        {
            g_halAifHandlers[direction].endHandler();
        }
    }
}


// =============================================================================
// hal_AifTone
// -----------------------------------------------------------------------------
//  Manage the playing of a tone: DTMF or Comfort Tone.
///
/// Outputs a DTMF or comfort tone
///
/// When the audio output is enabled, a DTMF or a comfort tones can be output
/// as well. This function starts or stops the output of a tone generated in the audio
/// module. \n
/// You can call this function several, if you just need to change the attenuation
/// or the tone type, without stopping the tone generation before (with the \c start
/// parameter set to \c FALSE. \n
/// If the function returns #HAL_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param tone The tone to generate
/// @param attenuation The attenuation level of the tone generator
/// @param start If \c TRUE, start to play a tone. If \c FALSE, stop it.
///
/// @return #HAL_ERR_NO.
// =============================================================================
PUBLIC HAL_ERR_T hal_AifTone(
    const HAL_AIF_TONE_TYPE_T         tone,
    const HAL_AIF_TONE_ATTENUATION_T  attenuation,
    const BOOL start)
{
//  Some init values for the configuration.
    UINT32 tonesReg = 0;


    if (!start)
    {
        hwp_aif->tone = AIF_ENABLE_H_DISABLE;
        return HAL_ERR_NO;
    }
    else
    {
        // Let's just say that it's not improper to stop
        // the tone even when it's already been stopped, ok ?
        if (!hal_ClkIsEnabled(gAifClk))
        {
            HAL_ASSERT(FALSE, "AIF Tone when resource not active");
        }

        tonesReg |= AIF_ENABLE_H_ENABLE;

        //  Setup the proper configuration word.
        // Tone frequency
        switch (tone)
        {
            case HAL_AIF_DTMF_0:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_0;
                break;
            case HAL_AIF_DTMF_1:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_1;
                break;
            case HAL_AIF_DTMF_2:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_2;
                break;
            case HAL_AIF_DTMF_3:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_3;
                break;
            case HAL_AIF_DTMF_4:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_4;
                break;
            case HAL_AIF_DTMF_5:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_5;
                break;
            case HAL_AIF_DTMF_6:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_6;
                break;
            case HAL_AIF_DTMF_7:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_7;
                break;
            case HAL_AIF_DTMF_8:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_8;
                break;
            case HAL_AIF_DTMF_9:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_9;
                break;
            case HAL_AIF_DTMF_A:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_A;
                break;
            case HAL_AIF_DTMF_B:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_B;
                break;
            case HAL_AIF_DTMF_C:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_C;
                break;
            case HAL_AIF_DTMF_D:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_D;
                break;
            case HAL_AIF_DTMF_S:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_S;
                break;
            case HAL_AIF_DTMF_P:
                tonesReg |= AIF_TONE_SELECT_DTMF | AIF_FREQ_P;
                break;
            case HAL_AIF_COMFORT_425:
                tonesReg |=  AIF_TONE_SELECT_COMFORT_TONE | AIF_COMFORT_FREQ_425_HZ;
                break;
            case HAL_AIF_COMFORT_950:
                tonesReg |=  AIF_TONE_SELECT_COMFORT_TONE | AIF_COMFORT_FREQ_950_HZ;
                break;
            case HAL_AIF_COMFORT_1400:
                tonesReg |=  AIF_TONE_SELECT_COMFORT_TONE | AIF_COMFORT_FREQ_1400_HZ;
                break;
            case HAL_AIF_COMFORT_1800:
                tonesReg |=  AIF_TONE_SELECT_COMFORT_TONE | AIF_COMFORT_FREQ_1800_HZ;
                break;
            default:
                return FALSE;
                break;
        }

        //  Setup the gain.
        switch (attenuation)
        {
            case HAL_AIF_TONE_M3DB:
                tonesReg |= AIF_TONE_GAIN_M3_DB;
                break;
            case HAL_AIF_TONE_M9DB:
                tonesReg |= AIF_TONE_GAIN_M9_DB;
                break;
            case HAL_AIF_TONE_M15DB:
                tonesReg |= AIF_TONE_GAIN_M15_DB;
                break;
            default:
                tonesReg |= AIF_TONE_GAIN_0_DB;
                break;
        }

        //  Configure the registers.
        // Set serial/parallel and loopback
        if (!(hwp_aif->ctrl & AIF_ENABLE_ENABLE))
        {
            hwp_aif->ctrl = g_halAifControlReg;
        }
        // Done during the opening
        // Enable tones
        hwp_aif->tone = tonesReg;

        return HAL_ERR_NO;
    }
}






// =============================================================================
// hal_AifPause
// -----------------------------------------------------------------------------
/// Pauses the streaming through the AIF
/// @param pause If \c TRUE, pauses a running stream. If \c FALSE, resume a
/// pause stream.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPause(BOOL pause)
{
    if (pause)
    {
        hwp_aif->ctrl &= ~AIF_ENABLE;
    }
    else
    {
        hwp_aif->ctrl |= AIF_ENABLE_ENABLE;
    }

    return HAL_ERR_NO;
}


// =============================================================================
// hal_AifTonePause
// -----------------------------------------------------------------------------
/// Pauses a tone generated by the AIF
/// @param pause If \c TRUE, pauses a bipping tone. If \c FALSE, resume a
/// paused tone.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifTonePause(BOOL pause)
{
    if (pause)
    {
        hwp_aif->tone &= ~AIF_ENABLE_H;
    }
    else
    {
        hwp_aif->tone |= AIF_ENABLE_H_ENABLE;
    }

    return HAL_ERR_NO;
}



PROTECTED BOOL hal_AifResourceMgmt(VOID)
{
    if ((hwp_aif->tone & AIF_ENABLE_H) != 0)
    {
        return TRUE;
    }
    if (g_halAifPlaying == TRUE)
    {
        return TRUE;
    }
    if (g_halAifRecording == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

PROTECTED VOID hal_AifSleep(VOID)
{

    if (hal_AifResourceMgmt() == TRUE)
    {
        HAL_ASSERT(FALSE, "AIF Sleep called when AIF still in use!");
    }
    //  release the resource
    if (hal_ClkIsEnabled(gAifClk))
        hal_ClkDisable(gAifClk);
}

PROTECTED VOID hal_AifWakeup(VOID)
{
    //  set the resource as active
    if (!hal_ClkIsEnabled(gAifClk))
        hal_ClkEnable(gAifClk);
}



// =============================================================================
// hal_AifGetIfcStatusRegPtr
// -----------------------------------------------------------------------------
/// Get a pointer to the IFC Curr_AHB_Addr register.
/// Used to Read this register by VoC.
/// @return INT32* pointer to the IFC Curr_AHB_Addr register.
// =============================================================================
PUBLIC INT32* hal_AifGetIfcStatusRegPtr(VOID)
{
    // NOTE : If this IFC register (or the IFC itself) does not exist, the function should return NULL
#ifdef CHIP_HAS_AP
    return (INT32*)&(hwp_auIfc->ch[PLAY].cur_ahb_addr);
#elif defined(CHIP_HAS_SYS_IFC2)
    return (INT32*)&(hwp_sysIfc2->ch[PLAY].cur_ahb_addr);
#else
    return (INT32*)&(hwp_bbIfc->ch[PLAY].cur_ahb_addr);
#endif
}

// =============================================================================
// hal_AifGetOverflowStatus
// -----------------------------------------------------------------------------
/// Returns Rx and Tx Overflow status bits
// =============================================================================
PUBLIC UINT32 hal_AifGetOverflowStatus(VOID)
{
    return (hwp_aif->ctrl & (AIF_OUT_UNDERFLOW | AIF_IN_OVERFLOW));
}


// =============================================================================
// hal_AifPlayDisableIfcAtNextIrq
// -----------------------------------------------------------------------------
/// During Play the IFC channel is in Fifo loop mode
/// In order to stop it cleanly, you can ask to automatically disable
/// the IFC channel at the next Half Fifo/End Fifo interrupt.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifPlayDisableIfcAtNextIrq(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();

#ifdef CHIP_HAS_AP
    hwp_auIfc->ch[PLAY].control |= APB1_IFC_AUTO_DISABLE;
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[PLAY].control |= SYS_IFC2_AUTO_DISABLE;
#else
    hwp_bbIfc->ch[PLAY].control |= BB_IFC_AUTO_DISABLE;
#endif

    hal_SysExitCriticalSection(status);

    return HAL_ERR_NO;
}

// =============================================================================
// hal_AifRecordDisableIfcAtNextIrq
// -----------------------------------------------------------------------------
/// During Record the IFC channel is in Fifo loop mode
/// In order to stop it cleanly, you can ask to automatically disable
/// the IFC channel at the next Half Fifo/End Fifo interrupt.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_AifRecordDisableIfcAtNextIrq(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();

#ifdef CHIP_HAS_AP
    hwp_auIfc->ch[RECORD].control |= APB1_IFC_AUTO_DISABLE;
#elif defined(CHIP_HAS_SYS_IFC2)
    hwp_sysIfc2->ch[RECORD].control |= SYS_IFC2_AUTO_DISABLE;
#else
    hwp_bbIfc->ch[RECORD].control |= BB_IFC_AUTO_DISABLE;
#endif

    hal_SysExitCriticalSection(status);

    return HAL_ERR_NO;
}

// =============================================================================
// hal_AifSideToneGainDb2Val
// -----------------------------------------------------------------------------
/// Convert a nominal AIF sidetone gain in dB unit to the corresponding register value.
/// @param db nominal gain in dB unit
/// @return register value
// =============================================================================
PUBLIC UINT32 hal_AifSideToneGainDb2Val(INT32 db)
{
    if (db < -36) return 0;
    if (db > 6)
        db = 6;
    return 1+(db+36)/3;
}


PUBLIC VOID  hal_AIFSetTxStb(HAL_AIF_FREQ_T sampleRate)
{
    UINT32 clkDivider;

    HAL_TRACE(HAL_AUDIO_TRC, 0, "hal_AIFSetTxStb: sample%d",sampleRate);

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) && defined(GALLITE_IS_8805)

    if (sampleRate < HAL_AIF_FREQ_12000HZ)
    {
        hal_AbbEnableAifFreqDiv2(TRUE);
        clkDivider = AIF_SOURCE_CLOCK / 2 / sampleRate - 2;
    }
    else
    {
        clkDivider = AIF_SOURCE_CLOCK / sampleRate - 2;
    }

#elif  defined(GALLITE_IS_8806) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)


#if defined (CHIP_DIE_8955)
    clkDivider = hal_AifGetClkDivider(HAL_AIF_TX_STB_SEL_CODEC,    sampleRate);
#elif defined(CHIP_DIE_8809E2)
    clkDivider = hal_AifGetClkDivider(HAL_AIF_TX_STB_SEL_CODEC,    sampleRate);
#else
    clkDivider = AIF_SOURCE_CLOCK / sampleRate - 2;
#endif

#else // smart phones, 8809P or later feature phones

    clkDivider = 0;
    // Formula: AIF_SOURCE_CLOCK / config->sampleRate - 2;
    // AIF_SOURCE_CLOCK is determined during audio codec configuration
    // in aud_CodecCommonCodecPrePowerOnConfig().
    HAL_TRACE(HAL_AUDIO_TRC, 0, "hal_AIFSetTxStb::clkDivider%d",clkDivider);

    switch (sampleRate)
    {
        // AIF_SOURCE_CLOCK = 24576000
        case HAL_AIF_FREQ_8000HZ:
            clkDivider = 3070;
            break;
        case HAL_AIF_FREQ_12000HZ:
            clkDivider = 2046;
            break;
        case HAL_AIF_FREQ_16000HZ:
            clkDivider = 1534;
            break;
        case HAL_AIF_FREQ_24000HZ:
            clkDivider = 1022;
            break;
        case HAL_AIF_FREQ_32000HZ:
            clkDivider = 766;
            break;
        case HAL_AIF_FREQ_48000HZ:
            clkDivider = 510;
            break;
        // AIF_SOURCE_CLOCK = 22579200
        case HAL_AIF_FREQ_11025HZ:
            clkDivider = 2046;
            break;
        case HAL_AIF_FREQ_22050HZ:
            clkDivider = 1022;
            break;
        case HAL_AIF_FREQ_44100HZ:
            clkDivider = 510;
            break;
        default:
            HAL_ASSERT(FALSE, "Invalid sample rate: %d", sampleRate);
            break;
    }

#endif

#ifdef CHIP_HAS_AP
    #ifdef FM_LININ_AUXMIC
    if(g_FM_Mode)
        hwp_aif->Cfg_Aif_Tx_Stb = AIF_AIF_TX_STB_DIV(clkDivider);
    else
    #endif
    hwp_aif->Cfg_Aif_Tx_Stb = AIF_AIF_TX_STB_EN
                              | AIF_AIF_TX_STB_DIV(clkDivider);


#else
    hwp_sysCtrl->Cfg_Aif_Tx_Stb_Div = SYS_CTRL_AIF_TX_STB_EN
                                      | SYS_CTRL_AIF_TX_STB_DIV(clkDivider);
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8809) // 8809P or later feature phones
    hwp_sysCtrl->Cfg_Aif_Tx_Stb_Div |= SYS_CTRL_AIF_TX_STB_CODEC_EN;
#endif
#endif
}


#ifdef CHIP_DIE_8955
PUBLIC VOID hal_AifEnFMRecNewPath(BOOL en)
{
    if(en)
        hwp_aif->fm_record_enable |= AIF_FM_RECORD_EN;
    else
        hwp_aif->fm_record_enable &= ~AIF_FM_RECORD_EN;
}
#endif

HAL_MODULE_CLK_INIT(AIF)
{
    gAifClk = hal_ClkGet(FOURCC_AIF);
    HAL_ASSERT((gAifClk != NULL), "Clk AIF not found!");
}


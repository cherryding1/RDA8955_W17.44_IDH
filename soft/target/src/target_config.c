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

#include "hal_config.h"
#include "hal_tcu.h"
#include "hal_host.h"

#include "string.h"

#include "rfd_config.h"
#include "pmd_config.h"
#include "memd_config.h"

#include "tgt_m.h"
#include "tgt_aud_cfg.h"
#include "tgt_hal_cfg.h"
#include "tgt_pmd_cfg.h"
#include "tgt_rfd_cfg.h"
#include "csw_config.h"
#if (CHIP_HAS_USB == 1)
#include "tgt_uctls_cfg.h"
#endif /* CHIP_HAS_USB */

#ifdef TGT_WITH_CAMERA
#include "tgt_camd_cfg.h"
#include "tgt_cams_cfg.h"
#endif

/// MCD is optional so only link to MCD if needed
#ifdef TGT_WITH_MCD
#include "mcd_config.h"
#include "mcd_m.h"
#include "tgt_mcd_cfg.h"
#endif

/// FM is optional so only link to FM if needed
#ifdef TGT_WITH_FM
#include "fmd_config.h"
#include "tgt_fmd_cfg.h"
#endif

/// GSENSOR is optional so only link to GSENSOR if needed
#ifdef TGT_WITH_GSENSOR
#include "gsensor_config.h"
#include "tgt_gsensor_cfg.h"
#endif

/// GPIOI2C is optional so only link to GPIOI2C if needed
#ifdef TGT_WITH_GPIOI2C
#include "i2cgpio_config.h"
#include "tgt_gpioi2c_cfg.h"
#endif

#ifdef TGT_WITH_GPIOSPI
#include "spigpio_config.h"
#include "tgt_gpiospi_cfg.h"
#endif

/// TS is optional so only link to TS if needed
#ifdef TGT_WITH_TS
#include "tsd_config.h"
#include "tgt_tsd_cfg.h"
#endif

/// BT is optional so only link to BT if needed
#ifdef TGT_WITH_BT
#include "btd_config.h"
#include "tgt_btd_cfg.h"
#include "btd_map.h"
#endif

/// ATV is optional so only link to ATV if needed
#ifdef TGT_WITH_ATV
#include "atvd_config.h"
#include "tgt_atvd_cfg.h"
#endif
#ifdef __PRJ_WITH_SPILCD__
#include "lcdd_types.h"
#endif

#include "tgt_board_cfg.h"
#include "tgt_app_cfg.h"
#include "key_defs.h"
#include "hal_key.h"
#include "hal_gpio.h"
#include "hal_ebc.h"
#include "hal_sys.h"
#include "hal_timers.h"
#include "cmn_defs.h"
#include "hal_mem_map.h"
#include "aud_m.h"
#if (CSW_EXTENDED_API == 0)
#include "aud_bt.h"
#endif /* CSW_EXTENDED_API */
#include "aud_codec_gallite.h"
#include "aud_fm.h"
#include "aud_null.h"
#ifdef FPGA
#include "aud_ti.h"
#endif
#include "aud_tv.h"

#if (CSW_EXTENDED_API_AUDIO_VIDEO==1)
#include "avps_map.h"
/* #include "mps_map.h" */
#else
#include "aps_map.h"
#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */

#include "ars_map.h"
#include "aud_map.h"
#include "calib_map.h"
#include "hal_map.h"
#include "lcdd_map.h"
#include "memd_map.h"
#include "pal_map.h"
#include "pmd_map.h"
#include "rfd_map.h"
#include "stack_map.h"
#include "std_map.h"
#include "sx_map.h"
#include "vois_map.h"
#include "vpp_map.h"
// #include "uctls_map.h"
#ifdef WITH_MDI
#include "mdi_map.h"
#endif
#ifdef TGT_WITH_EXPKEY
#include "tgt_expkey_cfg.h"
#endif

#include "bcpu_registermap.h"
#include "dsm_dev_driver.h"
#include "dsm_config.h"

#include "tgt_dsm_cfg.h"

#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#else
#include "memd_m.h"
#include "memd_cache.h"
#endif

#ifdef USE_OPIDDR_PSRAM
#include "tgt_ddr_cfg.h"
#include "tgt_ddr_freq_cfg.h"
#endif

#include "sxr_sbx.h"
#include "sxr_tls.h"
#include "fs/sf_api.h"


// =============================================================================
//  EXTERNS
// =============================================================================

extern CONST UINT8 g_tgtKeyMap[];


// =============================================================================
//  DEFINES
// =============================================================================

#define BATT_CURVE_MAGIC_NUMBER         0xBA44CA9A

#define READ_SEQ_LOCK(seq, read) \
    while (1) \
    { \
        UINT32 startSeq; \
        while (UNLIKELY((startSeq = seq) & 1)) \
        { \
            sxr_Sleep(5); \
        } \
        read; \
        if (LIKELY(startSeq == seq)) \
        { \
            break; \
        } \
    }

#define WRITE_SEQ_LOCK(seq, write) \
    do \
    { \
        ++seq; \
        write; \
        seq++; \
    } while (0)


// =============================================================================
//  TYPES
// =============================================================================




// =============================================================================
//  FORWARD DECLARATIONS
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PROTECTED CONST TGT_HW_MODULE_T g_tgtHwModule =
{
    .target = STRINGIFY_VALUE(CT_TARGET),
    .chipAsic = STRINGIFY_VALUE(CT_ASIC),
    .chipDie = STRINGIFY_VALUE(CT_CHIP_DIE),
    .rfPa = STRINGIFY_VALUE(PA_MODEL),
    .flash = STRINGIFY_VALUE(FLSH_MODEL),
    .lcd = STRINGIFY_VALUE(LCD_MODEL),
    .cam = STRINGIFY_VALUE(CAMERA_MODEL),
    .bt = STRINGIFY_VALUE(BT_MODEL),
    .fm = STRINGIFY_VALUE(FM_MODEL),
    .atv = STRINGIFY_VALUE(ATV_MODEL),
};

// HAL configuration structure
PROTECTED CONST HAL_CFG_CONFIG_T g_tgtHalCfg = TGT_HAL_CONFIG;


#define TGT_AUD_FUNC_NAME(s, func) aud_ ## s ## func

#define TGT_STREAM_START_DRIVER(driver)         TGT_AUD_FUNC_NAME(driver, StreamStart)
#define TGT_STREAM_RECORD_DRIVER(driver)        TGT_AUD_FUNC_NAME(driver,StreamRecord)
#define TGT_STREAM_STOP_DRIVER(driver)          TGT_AUD_FUNC_NAME(driver,StreamStop)
#define TGT_STREAM_PAUSE_DRIVER(driver)         TGT_AUD_FUNC_NAME(driver,StreamPause)
#define TGT_SETUP_DRIVER(driver)                TGT_AUD_FUNC_NAME(driver,Setup)
#define TGT_TONE_DRIVER(driver)                 TGT_AUD_FUNC_NAME(driver,Tone)
#define TGT_TONE_PAUSE_DRIVER(driver)           TGT_AUD_FUNC_NAME(driver,TonePause)
#define TGT_CALIB_UPDATE_VALUES_DRIVER(driver)  TGT_AUD_FUNC_NAME(driver,CalibUpdateValues)

// AUD configuration structure
PROTECTED CONST AUD_CONFIG_T g_tgtAudCfg =
{
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_RECEIVER_DRIVER),
        TGT_AUD_CONFIG_RECEIVER_PARAM,
        TGT_AUD_CONFIG_RECEIVER_OUTPUT_PATH,
        TGT_AUD_CONFIG_RECEIVER_OUTPUT_TYPE,
        TGT_AUD_CONFIG_RECEIVER_INPUT_PATH
    },
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_EAR_PIECE_DRIVER),
        TGT_AUD_CONFIG_EAR_PIECE_PARAM,
        TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_PATH,
        TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_TYPE,
        TGT_AUD_CONFIG_EAR_PIECE_INPUT_PATH
    },
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER),
        TGT_AUD_CONFIG_LOUD_SPEAKER_PARAM,
        TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_PATH,
        TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_TYPE,
        TGT_AUD_CONFIG_LOUD_SPEAKER_INPUT_PATH
    },
#if defined(TGT_WITH_BT) || defined(CHIP_HAS_AP)
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_BT_DRIVER),
        TGT_AUD_CONFIG_BT_PARAM,
        TGT_AUD_CONFIG_BT_OUTPUT_PATH,
        TGT_AUD_CONFIG_BT_OUTPUT_TYPE,
        TGT_AUD_CONFIG_BT_INPUT_PATH
    },
#else // TGT_WITH_BT
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
#endif // TGT_WITH_BT
#if defined(TGT_WITH_FM) || defined(CHIP_HAS_AP)
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_FM_DRIVER),
        TGT_AUD_CONFIG_FM_PARAM,
        TGT_AUD_CONFIG_FM_OUTPUT_PATH,
        TGT_AUD_CONFIG_FM_OUTPUT_TYPE,
        TGT_AUD_CONFIG_FM_INPUT_PATH
    },
#else // TGT_WITH_FM
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
#endif // TGT_WITH_FM
#if defined(TGT_WITH_ATV) || defined(CHIP_HAS_AP)
    {
        TGT_STREAM_START_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_STREAM_RECORD_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_STREAM_STOP_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_STREAM_PAUSE_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_SETUP_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_TONE_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_TONE_PAUSE_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_CALIB_UPDATE_VALUES_DRIVER(TGT_AUD_CONFIG_TV_DRIVER),
        TGT_AUD_CONFIG_TV_PARAM,
        TGT_AUD_CONFIG_TV_OUTPUT_PATH,
        TGT_AUD_CONFIG_TV_OUTPUT_TYPE,
        TGT_AUD_CONFIG_TV_INPUT_PATH
    }
#else // TGT_WITH_ATV
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    }
#endif // TGT_WITH_ATV
};

PROTECTED CONST PMD_CONFIG_T g_tgtPmdCfg = TGT_PMD_CONFIG;

#if defined(TGT_PMD_BATT_CAP_CURVE_ARRAY) && !defined(TGT_PMD_BATT_CAP_CURVE_STEP_MV)
PROTECTED CONST TGT_PMD_VOLT_CAP_T g_tgtPmdBattCapCurveAry[] =
    TGT_PMD_BATT_CAP_CURVE_ARRAY;
#endif

#ifdef TGT_PMD_TEMPERATURE_CURVE_ARRAY
PROTECTED CONST TGT_PMD_TEMPERATURE_RESISTANCE_T g_tgtPmdTpCurveAry[] =
    TGT_PMD_TEMPERATURE_CURVE_ARRAY;
#endif

#ifdef INTERNAL_XCV_CONTROL_PASW
#undef TGT_XCV_CONFIG
#undef TGT_PA_CONFIG
#undef TGT_SW_CONFIG
#define TGT_XCV_CONFIG    {.RST = TCO_UNUSED, .PDN  = TCO(11)               }
#define TGT_PA_CONFIG     {.ENA = TCO_UNUSED, .TXEN = TCO_UNUSED, .BS  = TCO_UNUSED }
#define TGT_SW_CONFIG     {.SW1 = TCO_UNUSED, .SW2  = TCO_UNUSED, .SW3 = TCO_UNUSED }
#endif

PROTECTED CONST XCV_CONFIG_T g_tgtXcvCfg = TGT_XCV_CONFIG;
PROTECTED CONST PA_CONFIG_T  g_tgtPaCfg  = TGT_PA_CONFIG;
PROTECTED CONST SW_CONFIG_T  g_tgtSwCfg  = TGT_SW_CONFIG;


PROTECTED CONST CSW_CONFIG_T g_tgtCswCfg = TGT_CSW_CONFIG;


// =============================================================================
// g_tgtDsmCfg
// -----------------------------------------------------------------------------
/// Dsm configuration.
// =============================================================================
PROTECTED CONST DSM_CONFIG_T g_tgtDsmCfg = TGT_DSM_CONFIG;

#if (USER_DATA_SIZE < (TGT_DSM_FLASH_SECCNT * 4096))
#error "DSM partition use more FLASH than user data size"
#endif


#ifdef TGT_WITH_MCD
// =============================================================================
// g_tgtMcdCfg
// -----------------------------------------------------------------------------
/// MCD configuration.
// =============================================================================
#ifdef DUAL_TFLASH_SUPPORT
PROTECTED CONST MCD_CONFIG_STRUCT_T g_tgtMcdCfg = TGT_MCD_CONFIG;
#else
PROTECTED CONST TGT_MCD_CONFIG_T g_tgtMcdCfg = TGT_MCD_CONFIG;
#endif
#endif


#ifdef TGT_WITH_FM
// =============================================================================
// g_tgtFmdCfg
// -----------------------------------------------------------------------------
/// FM Driver configuration.
// =============================================================================
PROTECTED CONST TGT_FMD_CONFIG_T g_tgtFmdCfg = TGT_FMD_CONFIG;
#endif

#ifdef TGT_WITH_GSENSOR
// =============================================================================
// g_tgtGsensorCfg
// -----------------------------------------------------------------------------
/// gsensor Driver configuration.
// =============================================================================
PROTECTED CONST TGT_GSENSOR_CONFIG_T g_tgtGsensorCfg = TGT_GSENSOR_CONFIG;
#endif

#ifdef TGT_WITH_GPIOI2C
// =============================================================================
//
// -----------------------------------------------------------------------------
/// GpioI2c Driver configuration.
// =============================================================================
PROTECTED CONST TGT_GPIOI2C_CONFIG_T g_tgtGpioi2cCfg = TGT_GPIOI2C_CONFIG;
#endif

#ifdef TGT_WITH_GPIOSPI
// =============================================================================
//
// -----------------------------------------------------------------------------
/// GpioI2c Driver configuration.
// =============================================================================
PROTECTED CONST TGT_GPIOSPI_CONFIG_T g_tgtGpiospiCfg = TGT_GPIOSPI_CONFIG;
#endif



#ifdef TGT_WITH_BT
// =============================================================================
// g_tgtBtdCfg
// -----------------------------------------------------------------------------
/// BT Driver configuration.
// =============================================================================
PROTECTED CONST TGT_BTD_CONFIG_T g_tgtBtdCfg = TGT_BTD_CONFIG;
#endif

#ifdef TGT_WITH_ATV
// =============================================================================
// g_tgtDAtvdCfg
// -----------------------------------------------------------------------------
/// ATV driver configuration.
// =============================================================================
PROTECTED CONST TGT_ATVD_CONFIG_T g_tgtAtvdCfg = TGT_ATVD_CONFIG;
#endif

#ifdef TGT_WITH_TS
// =============================================================================
// g_tgtTsdCfg
// -----------------------------------------------------------------------------
/// TSD configuration.
// =============================================================================
PROTECTED CONST TSD_CONFIG_T g_tgtTsdCfg = TGT_TSD_CONFIG;

#ifdef TGT_TSD_KEY_ADC_VALUE_ARRAY
PROTECTED CONST UINT16 g_tgtTsdKeyAdcValAry[] = TGT_TSD_KEY_ADC_VALUE_ARRAY;
#endif

#ifdef TGT_TSD_KEY2_ADC_VALUE_ARRAY
PROTECTED CONST UINT16 g_tgtTsdKey2AdcValAry[] = TGT_TSD_KEY2_ADC_VALUE_ARRAY;
#endif
#endif // TGT_WITH_TS


#ifdef TGT_WITH_EXPKEY
// =============================================================================
// g_tgtExpKeyCfg
// -----------------------------------------------------------------------------
/// Expand Key configuration.
// =============================================================================
PROTECTED CONST TGT_CFG_EXPKEY_T g_tgtExpKeyCfg = TGT_EXPKEY_CONFIG;
#endif


// =============================================================================
#if (CHIP_HAS_USB == 1)
// =============================================================================
// g_tgtUctlsCfg
// -----------------------------------------------------------------------------
/// UCTLS configuration (USB Controller)
// =============================================================================
PROTECTED CONST UCTLS_CONFIG_T g_tgtUctlsCfg[] = 
{
#ifdef _UTRACE_
    {
        .servicesList[0] =
        {
            .serviceCallback = &g_utracesCallback,
        }
    },
#endif
#ifdef _UMSS_
    {
        .servicesList[0] =
        {
            .serviceCallback = &g_umssCallback,
            .serviceCfg      =
            {
                .mss.transportCfg =
                {
                    .transportParameters =
                    {
                        .inEpNum  = 2,
                        .outEpNum = 2
                    },
                    .transportCallback = &g_umssTransportBoScsiCallback,
                },
                .mss.storageCfg =
                {
                    {
                        .storageCallback = &g_umssStorageFlashCallback,
                    },
                    {
                        .storageCallback        = &g_umssStorageMcCallback,
                    },
                    {
                        .storageCallback        = 0,
                        .storageParameters.size = 0
                    },
                    {
                        .storageCallback        = 0,
                        .storageParameters.size = 0
                    }
                }
            }
        }
    },
#endif
#ifdef _UCDCACM_
    {
        .servicesList[0] =
        {
            .serviceCallback = &g_uatCallback,
            .serviceCfg      =
            {
                .at.controlCfg =
                {
                    #ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_
                    .controlParameters =
                    {
                        .inEpNum  = 3,
                        .outEpNum = 3
                    },
                    #else
                    .controlParameters =
                    {
                        .intrEpNum  = 3,
                    },
                    #endif
                    .controlCallback = &g_uatctrolCallback
                },
                .at.streamCfg =
                {
                    .streamParameters =
                    {
                        .inEpNum  = 2,
                        .outEpNum = 2
                    },
                    .streamCallback = &g_uattransportCallback
                }
            }
        }
    },
#endif
#ifdef _UVIDEO_
    {
        .servicesList[0] =
        {
            .serviceCallback = &g_uvideosCallback,
            .serviceCfg      =
            {
                .videos.iad = &g_uvideosIad,
                .videos.controlCfg =
                {
                    .controlParameters =
                    {
                        .intrEpNum  = 3,
                    },
                    .controlCallback = &g_uvideosControlCallback
                },
                .videos.streamCfg =
                {
                    .streamParameters =
                    {
                        .inEpNum  = 2,
                    },
                    .streamCallback = &g_uvideosStreamCallback
                }
            }
        }
    },
#endif
#ifdef _URNDIS_
    {
        .servicesList[0] =
        {
            .serviceCallback = &g_urndisCallback,
            .serviceCfg      =
            {
                .rndis.iad = &g_urndisIad,
                .rndis.controlCfg =
                {
                    .controlParameters =
                    {
                        .intrEpNum  = 3,
                    },
                    .controlCallback = &g_urndisControlCallback
                },
                .rndis.dataCfg =
                {
                    .dataParameters =
                    {
                        .inEpNum  = 2,
                        .outEpNum = 2
                    },
                    .dataCallback = &g_urndisDataCallback
                }
            }
        }
    },
#endif
    {
        .servicesList[0].serviceCallback = 0
    }
};
#endif /* CHIP_HAS_USB */


#ifdef TGT_WITH_CAMERA
// =============================================================================
// g_tgtCamdCfg
// -----------------------------------------------------------------------------
/// Camera driver configuration.
// =============================================================================
PROTECTED CONST CAMD_CONFIG_T g_tgtCamdCfg = TGT_CAMD_CONFIG;


// =============================================================================
// g_tgtCamsCfg
// -----------------------------------------------------------------------------
/// Camera service configuration.
// =============================================================================
PROTECTED CONST CAMS_CONFIG_T g_tgtCamsCfg = TGT_CAMS_CONFIG;
#endif


#ifdef __PRJ_WITH_SPILCD__
// =============================================================================
// g_tgtSpiLcdCfg
// -----------------------------------------------------------------------------
/// spilcd service configuration.
// =============================================================================
PROTECTED CONST SPILCD_CONFIG_STRUCT_T g_tgtSpiLcdCfg = TGT_SPILCD_CONFIG;
#endif


#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)

PROTECTED CONST DDR_TIMING_T g_ddr_table[]= TGT_DDR_TIMING;

#endif


//  FUNCTIONS
// =============================================================================
extern UINT32 TS_OutputText(UINT16 nIndex, PCSTR fmt, ...);
#define TGT_SendEvent(x) //hal_HstSendEvent(x)
#define TGT_Trace(pFormat, ...)  //TS_OutputText(13, (PCSTR)pFormat, ##__VA_ARGS__)

#ifdef BUILD_ON_PLATFORM_TEST
#define TGT_Assert(bl, format, ...)
#else
#define TGT_Assert(bl, format, ...)         \
{if (!(bl)) {                                                            \
    hal_DbgAssert(format, ##__VA_ARGS__);       \
}}
#endif

PRIVATE HANDLE g_tgt_semaphore = NULL;
VOID tgt_SemInit(VOID)
{
    g_tgt_semaphore = sxr_NewSemaphore(1);
    if((HANDLE)NULL == g_tgt_semaphore)
    {

    }
}


VOID tgt_SemWaitFor(VOID)
{
    if((HANDLE)NULL != g_tgt_semaphore)
    {
        sxr_TakeSemaphore(g_tgt_semaphore);
    }
    else
    {
    }
}


VOID tgt_SemRelease(VOID)
{
    if((HANDLE)NULL != g_tgt_semaphore)
    {
        sxr_ReleaseSemaphore(g_tgt_semaphore);
    }
    else
    {
    }
}

#if !defined(_T_UPGRADE_PROGRAMMER) && defined(REDUNDANT_DATA_REGION)
PUBLIC UINT32 tgt_get_factory_setting_size(VOID)
{
    return sizeof(TGT_FACTORY_CFG_T);
}
#endif

// =============================================================================
// tgt_GetMobileId
// -----------------------------------------------------------------------------
/// This function is used to get the Mobile ID description.
// =============================================================================
PUBLIC INT32 tgt_GetImeiSv(UINT8 simId,UINT8* imeiSv)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetImeiSv begin.");
    if(NULL == imeiSv)
    {
        TGT_Trace(" tgt_GetImeiSv imeiSv = NULL.");
        return 1;
    }
    if(simId >= FACT_SIM_COUNT)
    {
        TGT_Trace(" tgt_GetImeiSv simId = %d.");
        return 2;
    }
    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,imeiSv) + (simId*sizeof(FACT_IMEI_T));
    size = sizeof(FACT_IMEI_T);
    pbuff = (UINT8*)imeiSv;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetImeiSv OK end.");
    return ret;
}

// =============================================================================
// tgt_SetMobileId
// -----------------------------------------------------------------------------
/// This function is used to update the Mobile ID description.
// =============================================================================
PUBLIC INT32 tgt_SetImeiSv(UINT8 simId,UINT8* imeiSv)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetImeiSv begin.");
    if(NULL == imeiSv)
    {
        TGT_Trace(" tgt_SetImeiSv imeiSv = NULL.");
        return 1;
    }
    if(simId >= FACT_SIM_COUNT)
    {
        TGT_Trace(" tgt_SetImeiSv simId = %d.");
        return 2;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,imeiSv) + (simId*sizeof(FACT_IMEI_T));
    size = sizeof(FACT_IMEI_T);
    pbuff = (UINT8*)imeiSv;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetImeiSv end.");
    return ret;
}

// =============================================================================
// tgt_GetMbSn
// -----------------------------------------------------------------------------
/// This function is used to get the Mobile Serial Number description.
// =============================================================================
PUBLIC INT32 tgt_GetMbSn(UINT8 *mbSn)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetMbSn begin.");
    if(NULL == mbSn)
    {
        TGT_Trace(" tgt_GetMbSn mbSn = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,mbSn);
    size = FIELD_SIZE(FACTORY_BLOCK_T,mbSn);
    pbuff = (UINT8*)mbSn;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetMbSn OK end.");
    return ret;
}

// =============================================================================
// tgt_SetMbSn
// -----------------------------------------------------------------------------
/// This function is used to update the mbSn.
// =============================================================================
PUBLIC INT32 tgt_SetMbSn(UINT8 *mbSn)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetMbSn begin.");
    if(NULL == mbSn)
    {
        TGT_Trace(" tgt_SetMbSn mbSn = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,mbSn);
    size = FIELD_SIZE(FACTORY_BLOCK_T,mbSn);
    pbuff = (UINT8*)mbSn;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetMbSn OK end.");
    return ret;
}

// =============================================================================
// tgt_GetMpSn
// -----------------------------------------------------------------------------
/// This function is used to get the mpSn.
// =============================================================================
PUBLIC INT32 tgt_GetMpSn(UINT8 *mpSn)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetMpSn begin.");
    if(NULL == mpSn)
    {
        TGT_Trace(" tgt_GetMpSn mpSn = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,mpSn);
    size = FIELD_SIZE(FACTORY_BLOCK_T,mpSn);
    pbuff = (UINT8*)mpSn;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetMpSn OK end.");
    return ret;
}

// =============================================================================
// tgt_SetMpSn
// -----------------------------------------------------------------------------
/// This function is used to update the mpSn.
// =============================================================================
PUBLIC INT32 tgt_SetMpSn(UINT8 *mpSn)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetMpSn begin.");

    if(NULL == mpSn)
    {
        TGT_Trace(" tgt_SetMpSn mpSn = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,mpSn);
    size = FIELD_SIZE(FACTORY_BLOCK_T,mpSn);
    pbuff = (UINT8*)mpSn;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetMpSn OK end.");
    return ret;
}


// =============================================================================
// tgt_GetBtInfo
// -----------------------------------------------------------------------------
/// This function is used to get the BT information.
// =============================================================================
PUBLIC INT32 tgt_GetBtInfo(TGT_BT_INFO_T *btInfo)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetBtInfo begin.");

    if(NULL == btInfo)
    {
        TGT_Trace(" tgt_GetBtInfo btInfo = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,btInfo);
    size = FIELD_SIZE(FACTORY_BLOCK_T,btInfo);
    pbuff = (UINT8*)btInfo;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetBtInfo OK end.");
    return ret;
}


// =============================================================================
// tgt_SetBtInfo
// -----------------------------------------------------------------------------
/// This function is used to update the BT information.
// =============================================================================
PUBLIC INT32  tgt_SetBtInfo(TGT_BT_INFO_T *btInfo)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetBtInfo begin.");

    if(NULL == btInfo)
    {
        TGT_Trace(" tgt_SetBtInfo btInfo = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,btInfo);
    size = FIELD_SIZE(FACTORY_BLOCK_T,btInfo);
    pbuff = (UINT8*)btInfo;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetBtInfo OK end.");
    return ret;
}


// =============================================================================
// tgt_GetWifiInfo
// -----------------------------------------------------------------------------
/// This function is used to get the WIFI information.
// =============================================================================
PUBLIC INT32 tgt_GetWifiInfo(TGT_WIFI_INFO_T *wifiInfo)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetWifiInfo begin.");

    if(NULL == wifiInfo)
    {
        TGT_Trace(" tgt_GetWifiInfo wifiInfo = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,wifiInfo);
    size = FIELD_SIZE(FACTORY_BLOCK_T,wifiInfo);
    pbuff = (UINT8*)wifiInfo;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetWifiInfo OK end.");
    return ret;
}


// =============================================================================
// tgt_SetWifiInfo
// -----------------------------------------------------------------------------
/// This function is used to set the WIFI information.
// =============================================================================
PUBLIC INT32 tgt_SetWifiInfo(TGT_WIFI_INFO_T *wifiInfo)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetWifiInfo begin.");

    if(NULL == wifiInfo)
    {
        TGT_Trace(" tgt_SetWifiInfo wifiInfo = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,wifiInfo);
    size = FIELD_SIZE(FACTORY_BLOCK_T,wifiInfo);
    pbuff = (UINT8*)wifiInfo;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetWifiInfo OK end.");
    return ret;
}


// =============================================================================
// tgt_GetStationName
// -----------------------------------------------------------------------------
/// This function is used to get the StationName.
// =============================================================================
PUBLIC INT32 tgt_GetStationName(UINT8 stationid,UINT8* stationNames)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetStationName begin.");

    if(NULL == stationNames)
    {
        TGT_Trace(" tgt_GetStationName stationNames = NULL.");
        return 1;
    }

    if(stationid >= FACT_STATION_COUNT)
    {
        TGT_Trace(" tgt_GetStationName stationid = %d.");
        return 2;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationNames) + (stationid*sizeof(FACT_STATION_NAME_T));
    size = sizeof(FACT_STATION_NAME_T);
    pbuff = (UINT8*)stationNames;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_GetStationName OK end.");
    return ret;
}

// =============================================================================
// tgt_SyncGetStationName
// -----------------------------------------------------------------------------
/// This function is used to Sync get the StationName.
// =============================================================================
PUBLIC INT32 tgt_SyncGetStationName(UINT8 stationid,UINT8* stationNames)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetStationName begin.");

    if(NULL == stationNames)
    {
        TGT_Trace(" tgt_GetStationName stationNames = NULL.");
        return 1;
    }

    if(stationid >= FACT_STATION_COUNT)
    {
        TGT_Trace(" tgt_GetStationName stationid = %d.");
        return 2;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationNames) + (stationid*sizeof(FACT_STATION_NAME_T));
    size = sizeof(FACT_STATION_NAME_T);
    pbuff = (UINT8*)stationNames;

    result = FACTORY_SyncReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_GetStationName OK end.");
    return ret;
}



// =============================================================================
// tgt_SetStationName
// -----------------------------------------------------------------------------
/// This function is used to Set the StationName.
// =============================================================================
PUBLIC INT32 tgt_SetStationName(UINT8 stationid,UINT8* stationNames)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetStationName begin.");

    if(NULL == stationNames)
    {
        TGT_Trace(" tgt_SetStationName stationNames = NULL.");
        return 1;
    }

    if(stationid >= FACT_STATION_NAME_LEN)
    {
        TGT_Trace(" tgt_SetStationName stationid = %d.");
        return 2;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationNames) + (stationid*sizeof(FACT_STATION_NAME_T));
    size = sizeof(FACT_STATION_NAME_T);
    pbuff = (UINT8*)stationNames;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SetStationName end.");
    return ret;
}


// =============================================================================
// tgt_GetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_GetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_GetStationPerformed(UINT16 *stationPerformed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetStationPerformed begin.");
    if(NULL == stationPerformed)
    {
        TGT_Trace(" tgt_GetStationPerformed stationPerformed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPerformed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPerformed);
    pbuff = (UINT8*)stationPerformed;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_GetStationPerformed OK end.");
    return ret;
}

// =============================================================================
// tgt_SyncGetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_GetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_SyncGetStationPerformed(UINT16 *stationPerformed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SyncGetStationPerformed begin.");
    if(NULL == stationPerformed)
    {
        TGT_Trace(" tgt_SyncGetStationPerformed stationPerformed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPerformed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPerformed);
    pbuff = (UINT8*)stationPerformed;

    result = FACTORY_SyncReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SyncGetStationPerformed OK end.");
    return ret;
}



// =============================================================================
// tgt_SetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_SetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_SetStationPerformed(UINT16 *stationPerformed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetStationPerformed begin.");
    if(NULL == stationPerformed)
    {
        TGT_Trace(" tgt_SetStationPerformed stationperformed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPerformed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPerformed);
    pbuff = (UINT8*)stationPerformed;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SetStationPerformed end.");
    return ret;
}


// =============================================================================
// tgt_SyncSetStationPerformed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_SetStationPerformed.
// =============================================================================
PUBLIC INT32 tgt_SyncSetStationPerformed(UINT16 *stationPerformed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SyncSetStationPerformed begin.");
    if(NULL == stationPerformed)
    {
        TGT_Trace(" tgt_SyncSetStationPerformed stationperformed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPerformed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPerformed);
    pbuff = (UINT8*)stationPerformed;

    result = FACTORY_SyncWriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_SyncWriteBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SyncSetStationPerformed end.");
    return ret;
}

//=============================================================================
// tgt_GetStationIDByName
// -----------------------------------------------------------------------------
/// This function is used to get the Station Perform bit Index By Station Name.
// =============================================================================
PUBLIC UINT8 tgt_GetStationIDByName(UINT8* stationName)
{
    FACT_STATION_NAME_T    l_stationNames[FACT_STATION_COUNT];
    UINT8 index=FACT_STATION_COUNT;
    UINT8 i=0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;
    UINT8*pstr=NULL;

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationNames);
    size = sizeof(l_stationNames);
    pbuff = (UINT8*)&(l_stationNames[0][0]);

    result = FACTORY_ReadField(offs,size,pbuff);
    TGT_Trace("tgt_GetStationIDByName ,  result=%d\n",result);
    if(!result)
    {
        for(; i<FACT_STATION_COUNT; i++)
        {
            pstr=stationName;
            if(!strncmp(&(l_stationNames[i][0]),pstr,strlen(pstr)))
            {
                index=i;
                break;
            }
        }
    }
    return index;
}

//=============================================================================
// tgt_SyncGetStationIDByName
// -----------------------------------------------------------------------------
/// This function is used to sync get the Station Perform bit Index By Station Name.
// =============================================================================
PUBLIC UINT8 tgt_SyncGetStationIDByName(UINT8* stationName)
{
    FACT_STATION_NAME_T    l_stationNames[FACT_STATION_COUNT];
    UINT8 index=FACT_STATION_COUNT;
    UINT8 i=0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;
    UINT8*pstr=NULL;

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationNames);
    size = sizeof(l_stationNames);
    pbuff = (UINT8*)&(l_stationNames[0][0]);

    result = FACTORY_SyncReadField(offs,size,pbuff);
    TGT_Trace("tgt_SyncGetStationIDByName ,  result=%d\n",result);
    if(!result)
    {
        for(; i<FACT_STATION_COUNT; i++)
        {
            pstr=stationName;
            if(!strncmp(&(l_stationNames[i][0]),pstr,strlen(pstr)))
            {
                index=i;
                break;
            }
        }
    }
    return index;
}


// =============================================================================
// tgt_SetStationPerformedByID
// -----------------------------------------------------------------------------
/// This function is used to set the Station Perform bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SetStationPerformedByID(UINT8 stationIdx, BOOL bVal)
{
    UINT16 performed_Val=0;
    UINT32 perfer_ret=0xFFFF;

    if(stationIdx>=FACT_STATION_COUNT)
    {
        return FALSE;
    }
    else
    {
        perfer_ret=tgt_GetStationPerformed(&performed_Val);
        TGT_Trace("Last Station performed is %x, ret is %x\n",performed_Val,perfer_ret);

        if(perfer_ret  !=0)
        {
            return FALSE;
        }
        else
        {
            if(bVal)
            {
                performed_Val |= (UINT16)1<<stationIdx;
            }
            else
            {
                performed_Val &= (UINT16)~(1<<stationIdx);
            }
            perfer_ret=tgt_SetStationPerformed(&performed_Val);
            TGT_Trace("Cur Station performed is %x, ret is %x\n",performed_Val,perfer_ret);

            if(perfer_ret  !=0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
}

// =============================================================================
// tgt_GetStationPerformedByID
// -----------------------------------------------------------------------------
/// This function is used to get the Station Perform bit By Index.
/// -1: invalid   0: FAIL  1: OK
// =============================================================================
PUBLIC char tgt_GetStationPerformedByID(UINT8 stationIdx)
{
    UINT16 performed_Val=0;
    INT32 perform_ret=0xFFFF;

    if(stationIdx>=FACT_STATION_COUNT)
    {
        return -1;
    }
    else
    {
        perform_ret=tgt_GetStationPerformed(&performed_Val);
        TGT_Trace("Last Station perform is %x, ret is %x\n",performed_Val,perform_ret);

        if(perform_ret !=0)
        {
            return -1;
        }
        else
        {
            performed_Val = performed_Val & (1<<stationIdx);

            if(performed_Val !=0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

}

// =============================================================================
// tgt_SyncSetStationPerformedByID
// -----------------------------------------------------------------------------
/// This function is used to sync set the Station Perform bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SyncSetStationPerformedByID(UINT8 stationIdx, BOOL bVal)
{
    UINT16 performed_Val=0;
    UINT32 perfer_ret=0xFFFF;

    if(stationIdx>=FACT_STATION_COUNT)
    {
        return FALSE;
    }
    else
    {
        perfer_ret=tgt_SyncGetStationPerformed(&performed_Val);
        TGT_Trace("Last Station performed is %x, ret is %x\n",performed_Val,perfer_ret);

        if(perfer_ret  !=0)
        {
            return FALSE;
        }
        else
        {
            if(bVal)
            {
                performed_Val |= (UINT16)1<<stationIdx;
            }
            else
            {
                performed_Val &= (UINT16)~(1<<stationIdx);
            }
            perfer_ret=tgt_SyncSetStationPerformed(&performed_Val);
            TGT_Trace("Cur Station performed is %x, ret is %x\n",performed_Val,perfer_ret);

            if(perfer_ret  !=0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
}


// =============================================================================
// tgt_SetStationPassedByID
// -----------------------------------------------------------------------------
/// This function is used to set the Station Pass bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SetStationPassedByID(UINT8 stationIdx, BOOL bVal)
{
    UINT16 passed_Val=0;
    UINT32 pass_ret=0xFFFF;

    if(stationIdx>=FACT_STATION_COUNT)
    {
        return FALSE;
    }
    else
    {
        pass_ret=tgt_GetStationPassed(&passed_Val);
        TGT_Trace("Last Station passed is %x, ret is %x\n",passed_Val,pass_ret);

        if(pass_ret !=0)
        {
            return FALSE;
        }
        else
        {
            if(bVal)
            {
                passed_Val |= (UINT16)1<<stationIdx;
            }
            else
            {
                passed_Val &= (UINT16)~(1<<stationIdx);
            }
            pass_ret=tgt_SetStationPassed(&passed_Val);
            TGT_Trace("Cur Station passed is %x, ret is %x\n",passed_Val,pass_ret);

            if(pass_ret !=0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
}

// =============================================================================
// tgt_SyncSetStationPassedByID
// -----------------------------------------------------------------------------
/// This function is used to sync set the Station Pass bit By Index.
// =============================================================================
PUBLIC BOOL tgt_SyncSetStationPassedByID(UINT8 stationIdx, BOOL bVal)
{
    UINT16 passed_Val=0;
    UINT32 pass_ret=0xFFFF;

    if(stationIdx>=FACT_STATION_COUNT)
    {
        return FALSE;
    }
    else
    {
        pass_ret=tgt_SyncGetStationPassed(&passed_Val);
        TGT_Trace("Last Station passed is %x, ret is %x\n",passed_Val,pass_ret);

        if(pass_ret !=0)
        {
            return FALSE;
        }
        else
        {
            if(bVal)
            {
                passed_Val |= (UINT16)1<<stationIdx;
            }
            else
            {
                passed_Val &= (UINT16)~(1<<stationIdx);
            }
            pass_ret=tgt_SyncSetStationPassed(&passed_Val);
            TGT_Trace("Cur Station passed is %x, ret is %x\n",passed_Val,pass_ret);

            if(pass_ret !=0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
}

// =============================================================================
// tgt_GetStationPassedByID
// -----------------------------------------------------------------------------
/// This function is used to get the Station Pass bit By Index.
/// -1: invalid   0: FAIL  1: OK
// =============================================================================
PUBLIC char tgt_GetStationPassedByID(UINT8 stationIdx)
{
    UINT16 passed_Val=0;
    UINT32 pass_ret=0xFFFF;

    if(stationIdx>=FACT_STATION_COUNT)
    {
        return -1;
    }
    else
    {
        pass_ret=tgt_GetStationPassed(&passed_Val);
        TGT_Trace("Last Station passed is %x, ret is %x\n",passed_Val,pass_ret);

        if(pass_ret !=0)
        {
            return -1;
        }
        else
        {
            passed_Val = passed_Val & (1<<stationIdx);

            if(passed_Val !=0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
}



// =============================================================================
// tgt_SetStationPerformedByName
// -----------------------------------------------------------------------------
/// This function is used to set the Station Perform bit By Station Name.
// =============================================================================
PUBLIC BOOL tgt_SetStationPerformedAndPassedByName(UINT8* stationNames, BOOL bProferm, BOOL bPass)
{
    UINT8 index=FACT_STATION_COUNT;
    BOOL ret_pf=FALSE;
    BOOL ret_ps=FALSE;
    BOOL bLastProform;
    BOOL bLastPass;
    char lastPerform;
    char lastpass;

    TGT_Trace("StationName is %s\n",stationNames);
    index=tgt_GetStationIDByName(stationNames);
    TGT_Trace("Station index is %d\n",index);
    lastPerform=tgt_GetStationPerformedByID(index);
    lastpass=tgt_GetStationPassedByID(index);

    if((lastPerform !=-1)&&(lastpass !=-1))
    {
        if(lastPerform==1) bLastProform=TRUE;
        else bLastProform=FALSE;
        if(lastpass==1) bLastPass=TRUE;
        else bLastPass=FALSE;
        if((bProferm==lastPerform)&&(bPass==bLastPass))
            return; //value is the same , not write
    }
    ret_pf= tgt_SetStationPerformedByID(index,bProferm);
    ret_ps= tgt_SetStationPassedByID(index,bPass);

    return (ret_pf & ret_ps);
}

// =============================================================================
// tgt_SyncSetStationPerformedByName
// -----------------------------------------------------------------------------
/// This function is used to sync set the Station Perform bit By Station Name.
// =============================================================================
PUBLIC BOOL tgt_SyncSetStationPerformedAndPassedByName(UINT8* stationNames, BOOL bProferm, BOOL bPass)
{
    UINT8 index=FACT_STATION_COUNT;
    BOOL ret_pf=FALSE;
    BOOL ret_ps=FALSE;

    TGT_Trace("StationName is %s\n",stationNames);
    index=tgt_SyncGetStationIDByName(stationNames);
    TGT_Trace("Station index is %d\n",index);
    ret_pf= tgt_SyncSetStationPerformedByID(index,bProferm);
    ret_ps= tgt_SyncSetStationPassedByID(index,bPass);

    return (ret_pf & ret_ps);

}


// =============================================================================
// tgt_GetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_GetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_GetStationPassed(UINT16 *stationPassed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetStationPassed begin.");
    if(NULL == stationPassed)
    {
        TGT_Trace(" tgt_GetStationPassed stationpassed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPassed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPassed);
    pbuff = (UINT8*)stationPassed;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_GetStationPassed OK end.");
    return ret;
}

// =============================================================================
// tgt_SyncGetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_GetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_SyncGetStationPassed(UINT16 *stationPassed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SyncGetStationPassed begin.");
    if(NULL == stationPassed)
    {
        TGT_Trace(" tgt_SyncGetStationPassed stationpassed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPassed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPassed);
    pbuff = (UINT8*)stationPassed;

    result = FACTORY_SyncReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_SyncReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SyncGetStationPassed OK end.");
    return ret;
}


// =============================================================================
// tgt_SetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_SetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_SetStationPassed(UINT16 *stationPassed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetStationPassed begin.");
    if(NULL == stationPassed)
    {
        TGT_Trace(" tgt_SetStationPassed stationpassed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPassed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPassed);
    pbuff = (UINT8*)stationPassed;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SetStationPassed end.");
    return ret;
}

// =============================================================================
// tgt_SyncSetStationPassed
// -----------------------------------------------------------------------------
/// This function is used to sync get the tgt_SetStationPassed.
// =============================================================================
PUBLIC INT32 tgt_SyncSetStationPassed(UINT16 *stationPassed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SyncSetStationPassed begin.");
    if(NULL == stationPassed)
    {
        TGT_Trace(" tgt_SyncSetStationPassed stationpassed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationPassed);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationPassed);
    pbuff = (UINT8*)stationPassed;

    result = FACTORY_SyncWriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_SyncWriteBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SyncSetStationPassed end.");
    return ret;
}



// =============================================================================
// tgt_GetStationLastDesc
// -----------------------------------------------------------------------------
/// This function is used to get the tgt_GetStationLastDesc.
// =============================================================================
PUBLIC INT32 tgt_GetStationLastDesc(UINT8 *stationLastDesc)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetStationLastDesc begin.");
    if(NULL == stationLastDesc)
    {
        TGT_Trace(" tgt_GetStationLastDesc StationLastDesc = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationLastDesc);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationLastDesc);
    pbuff = (UINT8*)stationLastDesc;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadBlock fail.");
        ret = result;
    }

    TGT_Trace(" tgt_GetStationLastDesc OK end.");
    return ret;
}


// =============================================================================
// tgt_SetStationLastDesc
// -----------------------------------------------------------------------------
/// This function is used to update the tgt_SetStationLastDesc.
// =============================================================================
PUBLIC INT32 tgt_SetStationLastDesc(UINT8 *stationLastDesc)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetStationLastDesc begin.");
    if(NULL == stationLastDesc)
    {
        TGT_Trace(" tgt_SetStationLastDesc StationLastDesc = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,stationLastDesc);
    size = FIELD_SIZE(FACTORY_BLOCK_T,stationLastDesc);
    pbuff = (UINT8*)stationLastDesc;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }

    TGT_Trace(" tgt_SetStationLastDesc end.");
    return ret;
}


// tgt_GetModuleId
// -----------------------------------------------------------------------------
/// This function is used to get the Module ID description.
// =============================================================================
PUBLIC INT32 tgt_GetModuleId(TGT_MODULE_ID_T *moduleId)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetModuleId begin.");
    if(NULL == moduleId)
    {
        TGT_Trace(" tgt_GetModuleId moduleId = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,moduleId);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,moduleId);
    pbuff = (UINT8*)moduleId;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetModuleId OK end.");
    return ret;
}

// =============================================================================
// tgt_SetModuleId
// -----------------------------------------------------------------------------
/// This function is used to update the Module ID description.
// =============================================================================
PUBLIC INT32 tgt_SetModuleId(TGT_MODULE_ID_T *moduleId)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetModuleId begin.");
    if(NULL == moduleId)
    {
        TGT_Trace(" tgt_SetModuleId moduleId = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,moduleId);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,moduleId);
    pbuff = (UINT8*)moduleId;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetModuleId OK end.");
    return ret;
}

// =============================================================================
// tgt_GetEncryptedUid
// -----------------------------------------------------------------------------
/// This function is used to get the encrypted UID.
// =============================================================================
PUBLIC INT32 tgt_GetEncryptedUid(UINT32 *uid)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetEncryptedUid begin.");
    if(NULL == uid)
    {
        TGT_Trace(" tgt_GetEncryptedUid uid = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,uid);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,uid);
    pbuff = (UINT8*)uid;
    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetEncryptedUid OK end.");
    return ret;
}

// =============================================================================
// tgt_SetEncryptedUid
// -----------------------------------------------------------------------------
/// This function is used to set the encrypted UID.
// =============================================================================
PUBLIC INT32 tgt_SetEncryptedUid(UINT32 uid)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetEncryptedUid begin.");

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,uid);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,uid);
    pbuff = (UINT8*)uid;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetEncryptedUid OK end.");
    return ret;
}


#ifdef SALES_TRACK_APP
// =============================================================================
// tgt_GetSalesTrackInfo
// -----------------------------------------------------------------------------
/// This function is used to get the Sales Track information.
// =============================================================================
PUBLIC INT32 tgt_GetSalesTrackInfo(TGT_SALES_TRACK_INFO_T *trackInfo)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_GetSalesTrackInfo begin.");
    if(NULL == trackInfo)
    {
        TGT_Trace(" tgt_GetSalesTrackInfo trackInfo = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,trackInfo);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,trackInfo);
    pbuff = (UINT8*)trackInfo;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_GetSalesTrackInfo OK end.");
    return ret;
}

// =============================================================================
// tgt_SetSalesTrackInfo
// -----------------------------------------------------------------------------
/// This function is used to update the Sales Track information.
// =============================================================================
PUBLIC INT32 tgt_SetSalesTrackInfo(TGT_SALES_TRACK_INFO_T *trackInfo)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" tgt_SetSalesTrackInfo begin.");
    if(NULL == trackInfo)
    {
        TGT_Trace(" tgt_SetSalesTrackInfo trackInfo = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,trackInfo);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,trackInfo);
    pbuff = (UINT8*)trackInfo;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" tgt_SetSalesTrackInfo OK end.");
    return ret;
}
#endif

// =============================================================================
// tgt_FlushFactorySettings
// -----------------------------------------------------------------------------
/// Wait previous factory setting write are done.
// =============================================================================
PUBLIC VOID tgt_FlushFactorySettings(VOID)
{
    COS_TaskWaitEventHandled(sf_async_task_handle());
}


#ifdef AT_USER_DBS
PUBLIC INT32 CFW_GetDBSSeed(INT8 *seed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" CFW_GetDBSSeed begin.");
    if(NULL == seed)
    {
        TGT_Trace(" CFW_GetDBSSeed seed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,seed);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,seed);
    pbuff = (UINT8*)seed;

    result = FACTORY_ReadField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_ReadField fail.");
        ret = result;
    }
    TGT_Trace(" CFW_GetDBSSeed OK end.");
    return ret;
}

PUBLIC INT32 TGT_SetDBSSeed(INT8 *seed)
{
    UINT32 ret = 0;
    INT32 result;
    UINT32 offs;
    UINT32 size;
    UINT8* pbuff;

    TGT_Trace(" TGT_SetDBSSeed begin.");
    if(NULL == seed)
    {
        TGT_Trace(" TGT_SetDBSSeed seed = NULL.");
        return 1;
    }

    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T,supplementary) +
           FIELD_OFFSETOF(TGT_FACTORY_CFG_T,seed);
    size = FIELD_SIZE(TGT_FACTORY_CFG_T,seed);
    pbuff = (UINT8*)seed;

    result = FACTORY_WriteField(offs,size,pbuff);
    if(result)
    {
        TGT_Assert(0,"FACTORY_WriteField fail.");
        ret = result;
    }
    TGT_Trace(" TGT_SetDBSSeed OK end.");
    return ret;
}

#endif

// =============================================================================
// tgt_GetHalConfig
// -----------------------------------------------------------------------------
/// This function is used by HAL to get access to its configuration structure.
/// This is the only way HAL can get this information.
// =============================================================================
PUBLIC CONST HAL_CFG_CONFIG_T* tgt_GetHalConfig(VOID)
{
    return &g_tgtHalCfg;
}

// =============================================================================
// tgt_GetAudConfig
// -----------------------------------------------------------------------------
/// This function is used by Aud to get access to its configuration structure.
/// This is the only way Aud can get this information.
// =============================================================================
PUBLIC CONST AUD_ITF_CFG_T* tgt_GetAudConfig(VOID)
{
    return g_tgtAudCfg;
}


// =============================================================================
// tgt_GetPmdConfig
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to its configuration structure.
/// This is the only way PMD can get this information.
// =============================================================================
PUBLIC CONST PMD_CONFIG_T* tgt_GetPmdConfig(VOID)
{
    return &g_tgtPmdCfg;
}


// =============================================================================
// tgt_GetPmdBattCapCurve
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to the battery capacity curve.
// =============================================================================
PUBLIC VOID tgt_GetPmdBattCapCurve(CONST TGT_PMD_VOLT_CAP_T **pArray, UINT8 *pLen)
{
#if defined(BATT_CURVE_IN_FACTORY_DATA) || \
    (defined(TGT_PMD_BATT_CAP_CURVE_ARRAY) && defined(TGT_PMD_BATT_CAP_CURVE_STEP_MV))
    static TGT_BATT_CURVE_T battCurve;
#endif

#ifdef BATT_CURVE_IN_FACTORY_DATA
    battCurve = tgt_GetFactorySettings()->battCurve;
    if (battCurve.magic == BATT_CURVE_MAGIC_NUMBER)
    {
        if (battCurve.len > ARRAY_SIZE(battCurve.curve))
        {
            hal_DbgAssert("Invalid battery curve length in factory data: %d",
                          battCurve.len);
        }

        *pArray = battCurve.curve;
        *pLen = battCurve.len;
        return;
    }
#endif

#ifdef TGT_PMD_BATT_CAP_CURVE_ARRAY
#ifdef TGT_PMD_BATT_CAP_CURVE_STEP_MV
    static CONST UINT8 capList[] = TGT_PMD_BATT_CAP_CURVE_ARRAY;

    STATIC_ASSERT(ARRAY_SIZE(battCurve.curve) >= ARRAY_SIZE(capList),
                  "Failed to convert battery capacity curve");

    for (int i = 0; i < ARRAY_SIZE(capList); i++)
    {
        battCurve.curve[i].cap = capList[i];
        battCurve.curve[i].volt = g_tgtPmdCfg.powerDownVoltageMV +
                                  i * TGT_PMD_BATT_CAP_CURVE_STEP_MV;
        // PMD driver will check the validity of the curve points
        if (i == ARRAY_SIZE(capList) - 1 &&
                g_tgtPmdCfg.batteryLevelFullMV > battCurve.curve[i].volt - TGT_PMD_BATT_CAP_CURVE_STEP_MV &&
                g_tgtPmdCfg.batteryLevelFullMV < battCurve.curve[i].volt)
        {
            battCurve.curve[i].volt = g_tgtPmdCfg.batteryLevelFullMV;
        }
    }

    *pArray = battCurve.curve;
    *pLen = ARRAY_SIZE(capList);
#else
    *pArray = g_tgtPmdBattCapCurveAry;
    *pLen = ARRAY_SIZE(g_tgtPmdBattCapCurveAry);
#endif
#else // !TGT_PMD_BATT_CAP_CURVE_ARRAY
    *pArray = NULL;
    *pLen = 0;
#endif // !TGT_PMD_BATT_CAP_CURVE_ARRAY
}


// =============================================================================
// tgt_GetPmdTemperatureCurve
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to the temperature curve.
// =============================================================================
PUBLIC VOID tgt_GetPmdTemperatureCurve(CONST TGT_PMD_TEMPERATURE_RESISTANCE_T **pArray, UINT8 *pLen)
{
#ifdef TGT_PMD_TEMPERATURE_CURVE_ARRAY
    *pArray = g_tgtPmdTpCurveAry;
    *pLen = ARRAY_SIZE(g_tgtPmdTpCurveAry);
#else
    *pArray = NULL;
    *pLen = 0;
#endif
}


#ifdef TGT_WITH_MCD
// =============================================================================
// tgt_GetMcdConfig
// -----------------------------------------------------------------------------
/// This function is used by MCD to get access to its configuration structure.
/// This is the only way MCD can get this information.
// =============================================================================
#ifdef DUAL_TFLASH_SUPPORT
PUBLIC CONST MCD_CONFIG_STRUCT_T* tgt_GetMcdConfig(VOID)
#else
PUBLIC CONST TGT_MCD_CONFIG_T* tgt_GetMcdConfig(VOID)
#endif
{
    return &g_tgtMcdCfg;
}
#endif


#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)

PUBLIC CONST DDR_TIMING_T* MEMD_BOOT_FUNC tgt_GetDDRConfig(UINT16* configNum)
{

    *configNum = ((sizeof(g_ddr_table))/(sizeof(g_ddr_table[0])));
    mon_Event(*configNum);
    return &g_ddr_table;
}

#endif


#ifdef TGT_WITH_FM
// =============================================================================
// tgt_GetFmdConfig
// -----------------------------------------------------------------------------
/// This function is used by FMD to get access to its configuration structure.
/// This is the only way FMD can get this information.
// =============================================================================
PUBLIC CONST TGT_FMD_CONFIG_T* tgt_GetFmdConfig(VOID)
{
    return &g_tgtFmdCfg;
}
#endif


#ifdef TGT_WITH_GSENSOR
// =============================================================================
// tgt_GetGsensorConfig
// -----------------------------------------------------------------------------
/// This function is used by gsensor to get access to its configuration structure.
/// This is the only way gsensor can get this information.
// =============================================================================
PUBLIC CONST TGT_GSENSOR_CONFIG_T* tgt_GetGsensorConfig(VOID)
{
    return &g_tgtGsensorCfg;
}
#endif


#ifdef TGT_WITH_GPIOI2C
// =============================================================================
// tgt_GetGpioi2cConfig
// -----------------------------------------------------------------------------
/// This function is used by GpioI2c to get access to its configuration structure.
/// This is the only way GpioI2c can get this information.
// =============================================================================
PUBLIC CONST TGT_GPIOI2C_CONFIG_T* tgt_GetGpioi2cConfig(VOID)
{
    return &g_tgtGpioi2cCfg;
}
#endif

#ifdef TGT_WITH_GPIOSPI
// =============================================================================
// tgt_GetGpioi2cConfig
// -----------------------------------------------------------------------------
/// This function is used by GpioI2c to get access to its configuration structure.
/// This is the only way GpioI2c can get this information.
// =============================================================================
PUBLIC CONST TGT_GPIOSPI_CONFIG_T* tgt_GetGpiospiConfig(VOID)
{
    return &g_tgtGpiospiCfg;
}
#endif


#ifdef TGT_WITH_BT
// =============================================================================
// tgt_GetBtdConfig
// -----------------------------------------------------------------------------
/// This function is used by BTD to get access to its configuration structure.
/// This is the only way BTD can get this information.
// =============================================================================
PUBLIC CONST TGT_BTD_CONFIG_T* tgt_GetBtdConfig(VOID)
{
    return &g_tgtBtdCfg;
}
#endif


#ifdef TGT_WITH_TS
// =============================================================================
// tgt_GetTsdConfig
// -----------------------------------------------------------------------------
/// This function is used by TSD to get access to its configuration structure.
/// This is the only way TSD can get this information.
// =============================================================================
PUBLIC CONST TSD_CONFIG_T* tgt_GetTsdConfig(VOID)
{
    return &g_tgtTsdCfg;
}

// =============================================================================
// tgt_GetTsdKeyAdcValue
// -----------------------------------------------------------------------------
/// This function is used by TSD to get access to the TSD key ADC values.
// =============================================================================
PUBLIC VOID tgt_GetTsdKeyAdcValue(CONST UINT16 **pArray, UINT8 *pLen)
{
#ifdef TGT_TSD_KEY_ADC_VALUE_ARRAY
    *pArray = g_tgtTsdKeyAdcValAry;
    *pLen = ARRAY_SIZE(g_tgtTsdKeyAdcValAry);
#else
    *pArray = NULL;
    *pLen = 0;
#endif
}

// =============================================================================
// tgt_GetTsdKey2AdcValue
// -----------------------------------------------------------------------------
/// This function is used by TSD to get access to the TSD key2 ADC values.
// =============================================================================
PUBLIC VOID tgt_GetTsdKey2AdcValue(CONST UINT16 **pArray, UINT8 *pLen)
{
#ifdef TGT_TSD_KEY2_ADC_VALUE_ARRAY
    *pArray = g_tgtTsdKey2AdcValAry;
    *pLen = ARRAY_SIZE(g_tgtTsdKey2AdcValAry);
#else
    *pArray = NULL;
    *pLen = 0;
#endif
}
#endif // TGT_WITH_TS

#ifdef TGT_WITH_ATV
// =============================================================================
// tgt_GetAtvDConfig
// -----------------------------------------------------------------------------
/// This function is used by ATVD to get access to its configuration structure.
/// This is the only way ATVD can get this information.
// =============================================================================
PUBLIC CONST TGT_ATVD_CONFIG_T* tgt_GetAtvdConfig(VOID)
{
    return &g_tgtAtvdCfg;
}
#endif

#ifdef TGT_WITH_EXPKEY
// =============================================================================
// tgt_GetExpKeyConfig
// -----------------------------------------------------------------------------
/// This function is used by expand key to get its configuration.
// =============================================================================
PUBLIC CONST TGT_CFG_EXPKEY_T* tgt_GetExpKeyConfig(VOID)
{
    return &g_tgtExpKeyCfg;
}
#endif


#if (CHIP_HAS_USB == 1)
///=============================================================================
// tgt_GetUctlsConfig
// -----------------------------------------------------------------------------
/// This function is used by UCTL to get access to its configuration structure.
/// This is the only way UCTLS can get this information.
// =============================================================================
PUBLIC CONST UCTLS_CONFIG_T* tgt_GetUctlsConfig(VOID)
{
    return g_tgtUctlsCfg;
}
#endif // CHIP_HAS_USB


#ifdef TGT_WITH_CAMERA
///=============================================================================
// tgt_GetCamdConfig
// -----------------------------------------------------------------------------
/// This function is used by CAMD to get access to its configuration structure.
/// This is the only way CAMD can get this information.
// =============================================================================
PUBLIC CONST CAMD_CONFIG_T* tgt_GetCamdConfig(VOID)
{
    return &g_tgtCamdCfg;
}

///=============================================================================
// tgt_GetCamsConfig
// -----------------------------------------------------------------------------
/// This function is used by CAMS to get access to its configuration structure.
/// This is the only way CAMS can get this information.
// =============================================================================
PUBLIC CONST CAMS_CONFIG_T* tgt_GetCamsConfig(VOID)
{
    return &g_tgtCamsCfg;
}
#endif


#ifdef __PRJ_WITH_SPILCD__
// =============================================================================
// tgt_GetSpiLcdConfig
// -----------------------------------------------------------------------------
/// This function is used by SPILCD to get access to its configuration structure.
/// This is the only way SPILCD can get this information.
// =============================================================================
PUBLIC CONST SPILCD_CONFIG_STRUCT_T* tgt_GetSpiLcdConfig(VOID)
{
    return &g_tgtSpiLcdCfg;
}
#endif


// =============================================================================
// tgt_GetKeyCode
// -----------------------------------------------------------------------------
/// This function returns the code of a key from its column number. It
/// implements thus the key mapping, between the key detected and the
/// keyboard of a given target.
/// @param key Key number as given by the HAL Key driver
/// @return The key code as mapped for a specifc target.
// =============================================================================
PUBLIC UINT8 tgt_GetKeyCode(UINT8 key)
{
    if (key == HAL_KEY_ON_OFF)
    {
        return KP_PWR;
    }
    else if (key >= TGT_KEY_NB)
    {
        return TGT_KEY_NB;
    }
    else
    {
        return g_tgtKeyMap[key];
    }
}


PUBLIC CONST XCV_CONFIG_T* tgt_GetXcvConfig(VOID)
{
    return &g_tgtXcvCfg;
}


PUBLIC CONST PA_CONFIG_T* tgt_GetPaConfig(VOID)
{
    return &g_tgtPaCfg;
}


PUBLIC CONST SW_CONFIG_T* tgt_GetSwConfig(VOID)
{
    return &g_tgtSwCfg;
}


// =============================================================================
// tgt_GetDsmConfig
// -----------------------------------------------------------------------------
// This function is used by DSM to get access to its configuration structure.
//
// =============================================================================
PUBLIC CONST DSM_CONFIG_T* tgt_GetDsmCfg(VOID)
{
    return &g_tgtDsmCfg;
}


// =============================================================================
// tgt_GetCswConfig
// -----------------------------------------------------------------------------
/// User heap configration
// =============================================================================
PUBLIC CONST CSW_CONFIG_T* tgt_GetCswConfig(VOID)
{
    return &g_tgtCswCfg;
}


// =============================================================================
// tgt_GetBuildVerNo
// -----------------------------------------------------------------------------
/// This function is used to access the version number of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildVerNo(VOID)
{
#ifndef CT_PRODUCT
#define CT_PRODUCT test_version
#endif
    const static char* verno = STRINGIFY_VALUE(CT_PRODUCT);
    return verno;
}


// =============================================================================
// tgt_GetBuildDateTime
// -----------------------------------------------------------------------------
/// This function is used to access the date time of this build.
// =============================================================================
PUBLIC CONST CHAR* tgt_GetBuildDateTime(VOID)
{
    const static char* datetime = STRINGIFY_VALUE(BUILD_TIME);
    return datetime;
}


PUBLIC CONST CHAR* tgt_GetBuildRelease(VOID)
{
    const static char* datetime = STRINGIFY_VALUE(CT_RELEASE);
    return datetime;
}
// =============================================================================
// tgt_GetHwModule
// -----------------------------------------------------------------------------
/// This function is used to access the hardware module names on this target.
// =============================================================================
PUBLIC CONST TGT_HW_MODULE_T* tgt_GetHwModule(VOID)
{
    return &g_tgtHwModule;
}


// =============================================================================
// tgt_GetMobileBoardSn
// -----------------------------------------------------------------------------
/// This function is used to get the mobile board SN.
// =============================================================================
PUBLIC VOID tgt_GetMobileBoardSn(UINT8 *pSn, UINT8 *pLen)
{
    UINT32 i;

    UINT8 mbSn[FACT_SN_LEN];

    tgt_GetMbSn((UINT8*)mbSn);

    // According to TOOL's implementation, the format of mbSn/mpSn is:
    // Byte 0 -- Other usage
    // Byte 1 -- SN[0]
    // Byte 2 -- SN[1]
    // Byte 3 -- SN[2]
    // Byte 4 -- SN_LEN + 1
    // Byte 5 -- SN[3]
    // ......
    if (mbSn[4] != 0xFF && mbSn[4] > 1 &&
            mbSn[4] < sizeof(mbSn))
    {
        *pLen = MIN(mbSn[4] - 1, *pLen);
        for (i = 0; i < *pLen; i++)
        {
            pSn[i] = mbSn[i + 1 + (i >= 3) ? 1 : 0];
        }
    }
    else
    {
        *pLen = 0;
        pSn[0] = '\0';
    }
}


// =============================================================================
// tgt_GetMobilePhoneSn
// -----------------------------------------------------------------------------
/// This function is used to get the mobile phone SN.
// =============================================================================
PUBLIC VOID tgt_GetMobilePhoneSn(UINT8 *pSn, UINT8 *pLen)
{
    UINT32 i;
    UINT8 mpSn[FACT_SN_LEN];

    tgt_GetMpSn(mpSn);

    // According to TOOL's implementation, the format of mbSn/mpSn is:
    // Byte 0 -- Other usage
    // Byte 1 -- SN[0]
    // Byte 2 -- SN[1]
    // Byte 3 -- SN[2]
    // Byte 4 -- SN_LEN + 1
    // Byte 5 -- SN[3]
    // ......
    if (mpSn[4] != 0xFF && mpSn[4] > 1 &&
            mpSn[4] < sizeof(mpSn))
    {
        *pLen = MIN(mpSn[4] - 1, *pLen);
        for (i = 0; i < *pLen; i++)
        {
            pSn[i] = mpSn[i + 1 + (i >= 3) ? 1 : 0];
        }
    }
    else
    {
        *pLen = 0;
        pSn[0] = '\0';
    }
}


// =============================================================================
// tgt_RegisterAllModules
// -----------------------------------------------------------------------------
/// This function call the register functions for each modules of platform.
// =============================================================================
PUBLIC VOID tgt_RegisterAllModules(VOID)
{
    // Required modules.
    hal_RegisterYourself();
    calib_RegisterYourself();
    sx_RegisterYourself();
    std_RegisterYourself();
    rfd_RegisterYourself();
    memd_RegisterYourself();
    pmd_RegisterYourself();
#if (CHIP_HAS_USB == 1)
//    uctls_RegisterYourself();
#endif /* (CHIP_HAS_USB == 1) */

    // All BCPU modules are registered by this:
    bcpu_RegisterYourself();

#ifdef TGT_REGISTER_PLATFORM_VERSIONS
    // Top level modules.
    pal_RegisterYourself();
    stack_RegisterYourself();
    vpp_RegisterYourself();

    // External drivers.
    aud_RegisterYourself();

    // Services.
#if (CSW_EXTENDED_API_AUDIO_VIDEO==1)
    avps_RegisterYourself();
    /*     mps_RegisterYourself(); */
#else
    // aps_RegisterYourself();
#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */

#if (CSW_EXTENDED_API_MRS == 1)
// TODO Implement this AVRS registration function
//  avrs_RegisterYourself();
#else
    ars_RegisterYourself();
#endif

    vois_RegisterYourself();

#ifndef CHIP_HAS_AP
    lcdd_RegisterYourself();
    // ffs_RegisterYourself();
    // mps_RegisterYourself();
    // cams_RegisterYourself();
#ifdef WITH_MDI
    mdi_RegisterYourself();
#endif
#ifdef TGT_WITH_BT
    btd_RegisterYourself();
#endif
#endif // !CHIP_HAS_AP

#endif // TGT_REGISTER_PLATFORM_VERSIONS
}

// =============================================================================
// (weak) mmi_RegisterYourself
// -----------------------------------------------------------------------------
/// Weak symbol to make linker happy in case of building target without MMI.
// =============================================================================
PUBLIC VOID __attribute__((weak)) mmi_RegisterYourself(VOID)
{
}
PUBLIC BOOL __attribute__((weak)) GetIsEarphonePlugIn(void)
{
    return 0;// IsEarphonePlugIn;
}
// =============================================================================
// tgt_RegisterMonitorModules
// -----------------------------------------------------------------------------
/// This function call the register functions for modules for boot monitor.
// =============================================================================
PUBLIC VOID tgt_RegisterMonitorModules(VOID)
{
    memd_RegisterYourself();
    calib_RegisterYourself();

#ifndef CHIP_HAS_AP
    mmi_RegisterYourself();
#endif // !CHIP_HAS_AP
}

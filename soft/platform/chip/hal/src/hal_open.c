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
#include "page_spy.h"
#include "sys_ctrl.h"
#include "sys_irq.h"
#include "cfg_regs.h"

#if (CHIP_HAS_ASYNC_BCPU)
#include "bb_ctrl.h"
#endif

#include "tcu.h"

#include "tgt_m.h"
#include "tgt_hal_cfg.h"
#include "tgt_pmd_cfg.h"

#include "pmd_m.h"
#include "rfd_xcv.h"
#include "calib_m.h"

#include "hal_rfspi.h"
#include "hal_timers.h"
#include "hal_ana_gpadc.h"
#include "hal_debug.h"
#include "hal_pwm.h"
#include "hal_mem_map.h"
#include "hal_gpio.h"
#include "hal_sys.h"
#include "hal_config.h"
#include "hal_map_engine.h"
#include "hal_speech.h"
#include "hal_usb.h"
#include "hal_camera.h"
#include "hal_ispi.h"
#include "hal_rda_abb.h"
#include "hal_voc.h"
#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#endif

#include "halp_page_spy.h"
#include "halp_sys.h"
#include "halp_sys_ifc.h"
#include "halp_timers.h"
#include "halp_calib.h"
#include "halp_config.h"
#include "halp_debug.h"
#include "halp_speech.h"
#include "halp_ebc.h"
#include "halp_proxy_memory.h"
#include "halp_profile.h"

#include "boot_usb_monitor.h"
#include "halp_usb_monitor.h"

#include "patch_m.h"

#include "stdio.h"

#include "pal_gsm.h"


// =============================================================================
//  EXTERNS
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================

// In 32 bits words, we keep the last 100 writes plus a stamp
#define SP_BUFFER_SIZE  (4*2048)

#ifdef FPGA_8955_DEBUG_TEMP
#define ENABLE_AUTO_CLOCK_GATING 0
#else
#define ENABLE_AUTO_CLOCK_GATING 1
#endif
#ifndef ENABLE_PXTS_EXTRA_LVL
#define ENABLE_PXTS_EXTRA_LVL 0
#endif


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// g_halCalibration
// -----------------------------------------------------------------------------
/// Since the calibration is managed by the Calib module, we need to use
/// an accessor function to get the pointer to the calibration structure.
/// To avoid to call that function numerous times, the g_halCalibration is
/// used as a pointer to the calibration structure, and is initialized by
/// hal_Open. g_halCalibration can be used by all driver of HAL.
// =============================================================================
PROTECTED CALIB_CALIBRATION_T*  g_halCalibration = NULL;

#ifdef PAGE_SPY_AT_ZERO
// placeholder for protecting arrea at the beginning of ram (addr 0 and above)
PROTECTED UINT32 g_SpZero[256] __attribute__((section (".ram")));
#endif


// =============================================================================
//  FUNCTIONS
// =============================================================================

/// Define CHIP_CHECK_UID_ENCRYPT in target.def.
/// Encryption is only applicable for release versions.
#if defined(CHIP_CHECK_UID_ENCRYPT) && defined(CT_NO_DEBUG)
PRIVATE UINT32 hal_UidEncryptAlgorithm(UINT32 num)
{
    UINT32  value;
    /// modify your encrypt algorithm here
    /// also modify the encrypt algorithm in tools
    value = ~(num<<2) + 1;

    return value;
}

PRIVATE VOID hal_CheckUidEncrypt()
{
    INT32 result;
    UINT32 val = pmd_GetEncryptUid();
    UINT32 checksum = hal_UidEncryptAlgorithm(val);
    UINT32 uid;

    result = tgt_GetEncryptedUid(&uid);

    if(result == 0 || uid != checksum)
    {
        HAL_ASSERT(0,"%x,%x,%x,The LOD file has been encrypted, please using authorized code!",val,checksum,uid);
        hal_SysSoftReset();
    }
}
#endif

// =============================================================================
// hal_Open
// -----------------------------------------------------------------------------
/// Initializes the HAL layer.
/// This function also do the open of the PMD driver and set the idle-mode.
// =============================================================================
PUBLIC VOID hal_Open(CONST HAL_CFG_CONFIG_T* halCfg)
{
    hal_HstTraceOpen(0, 0);

    // Prevent from going into test mode
    // through the test mode pin.

    // Checkers to ensures that defines used to publicly access registers are
    // coherent with the register map.
    HAL_ASSERT((UINT32)&(hwp_sysIrq->SC) == HAL_SYS_CRITICAL_SECTION_ADDRESS,
               "The critical section public access is not up to date with"
               "the memory map. Edit hal_sys.h and update "
               "HAL_SYS_CRITICAL_SECTION_ADDRESS.%08X %08X",
               (UINT32)&(hwp_sysIrq->SC),
               HAL_SYS_CRITICAL_SECTION_ADDRESS);

#if (CHIP_HAS_BCPU_ROM == 0)
    // Install all patches
    patch_Open();
#endif

    // Initialize the map engine.
    hal_MapEngineOpen();

#if (CHIP_HAS_USB == 1)
    // Avoid crashing when calling hal_UsbClose() without hal_UsbOpen() before.
    boot_UsbInitVar();
#endif

#if defined(HAL_PROFILE_NOT_ON_ROMULATOR) || defined(HAL_PROFILE_ON_BUFFER)
    // Configure PXTS to enable record in RAM
    hal_DbgPxtsOpen();
#endif

    // Initialize the System IFC.
    hal_IfcOpen();

#if (CHIP_HAS_PROXY_MEMORY == 1)
    // Disable access to the proxy memory from the external
    // bus. (ie. Enable the access at cpu speed)
    hal_PMemEnableExternalAccess(FALSE);
#endif

    // Unlock following registers.
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

#ifndef CHIP_DIE_8955
    // cache ram automatic disable mode
    hwp_sysCtrl->Cfg_Cpus_Cache_Ram_Disable |=
        SYS_CTRL_XCPU_CACHE_RAM_DISABLE(1)
        | SYS_CTRL_BCPU_CACHE_RAM_DISABLE(1);
#endif

#if (ENABLE_AUTO_CLOCK_GATING)
    // automatic clock gating enable
    hwp_sysCtrl->Clk_Sys_Mode = 0
#ifndef CHIP_HAS_AP
                                | SYS_CTRL_MODE_SYS_MAILBOX_MANUAL
#endif
#ifdef CHIP_DIE_8955
                                | SYS_CTRL_MODE_SYS_GOUDA_MANUAL
                                | SYS_CTRL_MODE_SYSD_PWM_MANUAL
#endif
                                | SYS_CTRL_MODE_SYS_A2A_MANUAL
                                ;

    hwp_sysCtrl->Clk_BB_Mode = 0
#ifndef CHIP_HAS_AP
                               | SYS_CTRL_MODE_BB_MAILBOX_MANUAL
                               | SYS_CTRL_MODE_BB_ROM_AHB_MANUAL
                               | SYS_CTRL_MODE_BB_COM_REGS_MANUAL
#endif
                               | SYS_CTRL_MODE_BB_A2A_MANUAL
                               ;

    hwp_sysCtrl->Clk_Other_Mode = 0;
    hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_ENABLE_OC_USBPHY;
#endif // ENABLE_AUTO_CLOCK_GATING

#ifndef CHIP_HAS_AP
    // SDMMC might output voltage and clock when power-key is
    // pressed, which might cause T-card initialized to a false
    // state. SDMMC clock is disabled here to ensure T-card can
    // be initialized correctly later.
#if (CHIP_HAS_SDIO == 1)
    hwp_sysCtrl->Clk_Per_Disable = SYS_CTRL_DISABLE_PER_SDMMC
                                   | SYS_CTRL_DISABLE_PER_SDMMC2;
    hwp_sysCtrl->Clk_Per_Mode = SYS_CTRL_MODE_PER_SDMMC_MANUAL
                                | SYS_CTRL_MODE_PER_SDMMC2_MANUAL;
#else
    hwp_sysCtrl->Clk_Per_Disable = SYS_CTRL_DISABLE_PER_SDMMC;
    hwp_sysCtrl->Clk_Per_Mode = SYS_CTRL_MODE_PER_SDMMC_MANUAL;
#endif

#ifdef DUAL_TFLASH_SUPPORT
    hwp_sysCtrl->BB_Rst_Set = SYS_CTRL_SET_RST_SDMMC2;
#endif

#endif // !CHIP_HAS_AP

    // Lock registers.
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    // Set the board specific configuration, depending on
    // the target.
    hal_BoardSetup(halCfg);

    // Set the sys clock, and tcu clock, and analog clocks
    // In FPGA, the frequency doesn't change after the initial setup
    // (hal_SysChangeSystemClock is empty)
    // With the Chip, the frequency is set according to the users
    // need. For example, PAL asks for at least 39 Mhz.
    hal_SysSetupSystemClock(HAL_SYS_FREQ_26M);
    hal_ClkInit();

    // ********************
    //   Calibration Init
    // ********************

    // This function check if the platform has been calibrated. If not,
    // it uses the default calibration parameters that have been written
    // in the Flash, with the code. If the platform has been calibrated,
    // it uses the values stored in the calibration dedicated Flash sector.

    // If we are in PAL_CALIB mode, the global calibration buffer will
    // be stored in Ram. Jade Debug Server or the CalibTool will be able to
    // write in an other buffer and swap it with the global calibration
    // buffer. After the calibration is done one of these buffer will be
    // burnt in Flash, in the calibration dedicated Flash sector.
    calib_DaemonInit();

    /// Init HAL internal pointer to the calibration structure.
    g_halCalibration = calib_GetPointers();

    // Read XCV chip ID
    // (must be done after sys clk setup, as it needs to request freq)
    // The XCV chip ID must be used when configuring ABB, PMU and XCV
    rfd_XcvReadChipId();

    // Open ISPI with default (romed) config
    // (must be done after sys clk setup, as it needs to request freq)
    // (must be done before hal_AbbOpen/pmd_Open)
    hal_IspiOpen(NULL);
    // Initialize ABB (must be done before pmd_Open, as the ABB settings
    // in pmd_Open cannot be overridden or reset to defaults)
    hal_AbbOpen();
    // Initializes Power Management Driver and set
    // it as no-eco mode.
    pmd_Open(tgt_GetPmdConfig());
    pmd_SetPowerMode(PMD_IDLEPOWER);

    // EBC clock has been setup in boot_Sector

    // Clock-out init
    hal_SysClkInit();

    // VoC init
    hal_VocInit();

    // OS timer init.
    hal_TimTickOpen();

#ifdef HAL_GDB_CONFIG
    hal_TimWatchDogOpen(30 * HAL_TICK1S);
#endif
#ifdef CHIP_HAS_AP
    hal_ApCommHeartbeatStart();
#endif

    // reset the speech fifo
    hal_SpeechFifoReset();

    // Configure debugging
#ifdef ENABLE_PXTS
#ifdef ENABLE_EXL
    hal_DbgPortSetup(HAL_DBG_EXL_AND_PXTS);
#else
    // Enable PXTS
    hal_DbgPortSetup(HAL_DBG_PXTS);
#endif
    // Enable some interesting levels by default
    hal_DbgPxtsSetup( 1<<HAL_DBG_PXTS_HAL
                      | 1<<HAL_DBG_PXTS_SX
                      | 1<<HAL_DBG_PXTS_PAL
                      | 1<<HAL_DBG_PXTS_BB_IRQ
                      | 1<<HAL_DBG_PXTS_SPC
                      | ENABLE_PXTS_EXTRA_LVL);
#else // !ENABLE_PXTS
#ifdef ENABLE_EXL
    hal_DbgPortSetup(HAL_DBG_EXL);
#endif
#endif // !ENABLE_PXTS

    // Ask Target to request every modules to register itself.
    tgt_RegisterAllModules();
    // initialise pal and rf stuff (transceiver, lps).
    pal_Open();

#ifndef CHIP_HAS_AP
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
    // Camera: Configure the camera RST and PDN lines so that
    // the camera is off by now.
    // We trick this by opening and closing the camera drivers
    // with dummy configurations.
#if defined(FPGA)
#else
    if (halCfg->camCfg.camUsed)
    {
        HAL_CAMERA_CFG_T cameraConfig = {0, };

        // Camera 0
        cameraConfig.rstActiveH = halCfg->camCfg.camRstActiveH;
        cameraConfig.pdnActiveH = halCfg->camCfg.camPdnActiveH;
        cameraConfig.dropFrame  = FALSE; // Irrelevant parameter.
        cameraConfig.camClkDiv  = 0xFF; // Irrelevant parameter.
        cameraConfig.endianess  = NO_SWAP; // Irrelevant parameter.
        cameraConfig.colRatio   = COL_RATIO_1_1; // Irrelevant parameter.
        cameraConfig.rowRatio   = ROW_RATIO_1_1; // Irrelevant parameter.
        cameraConfig.camId      = 0;

        hal_CameraOpen(&cameraConfig);
        hal_CameraClose();
    }
    if (halCfg->camCfg.cam1Used)
    {
        HAL_CAMERA_CFG_T cameraConfig = {0, };

        // Camera 1
        cameraConfig.rstActiveH = halCfg->camCfg.cam1RstActiveH;
        cameraConfig.pdnActiveH = halCfg->camCfg.cam1PdnActiveH;
        cameraConfig.dropFrame  = FALSE; // Irrelevant parameter.
        cameraConfig.camClkDiv  = 0xFF; // Irrelevant parameter.
        cameraConfig.endianess  = NO_SWAP; // Irrelevant parameter.
        cameraConfig.colRatio   = COL_RATIO_1_1; // Irrelevant parameter.
        cameraConfig.rowRatio   = ROW_RATIO_1_1; // Irrelevant parameter.
        cameraConfig.camId      = 1;

        hal_CameraOpen(&cameraConfig);
        hal_CameraClose();
    }
#endif
#endif // !(_FLASH_PROGRAMMER || _T_UPGRADE_PROGRAMMER)
#endif // !CHIP_HAS_AP

#if defined(CHIP_CHECK_UID_ENCRYPT) && defined(CT_NO_DEBUG)
    hal_CheckUidEncrypt();
#endif

#ifdef EARLY_TRACE_OPEN
    extern void sxs_HostTraceEarlyOpenDone (void);
    sxs_HostTraceEarlyOpenDone ();
#endif
}



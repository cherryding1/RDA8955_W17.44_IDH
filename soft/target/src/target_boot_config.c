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
#include "hal_mem_map.h"
#include "boot_sys.h"
#include "boot_sector_driver.h"
#include "memd_config.h"
#include "memd_m.h"

#include "key_defs.h"
#include "tgt_board_cfg.h"


// =============================================================================
// MEMD config
// =============================================================================
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#ifndef GALLITE_IS_8806
#include "memd_tgt_params_gallite.h"
#else
#include "memd_tgt_params_8806.h"
#endif
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#include "memd_tgt_params_8808.h"
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#include "memd_tgt_params_8809.h"
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8809P)
#include "memd_tgt_params_8809p.h"
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8850 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8820 || \
       CHIP_ASIC_ID == CHIP_ASIC_ID_8850E)
#include "memd_tgt_params_8810.h"
#else
#error "Unsupported CHIP_ASIC_ID"
#endif


// =============================================================================
// g_tgtMemdFlashCfg
// -----------------------------------------------------------------------------
/// MEMD Flash configuration configuration.
// =============================================================================
PROTECTED CONST MEMD_FLASH_CONFIG_T
g_tgtMemdFlashCfg = TGT_MEMD_FLASH_CONFIG;


// =============================================================================
// g_tgtMemdRamCfg
// -----------------------------------------------------------------------------
/// MEMD Ram configuration.
// =============================================================================
PROTECTED CONST MEMD_RAM_CONFIG_T
g_tgtMemdRamCfg[RDA_PSRAM_QTY] =
{
#ifdef CHIP_HAS_AP
    TGT_MEMD_RAM_CONFIG,
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    TGT_MEMD_RAM_CONFIG,
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
    TGT_MEMD_RAM_CONFIG,
    TGT_RAM_CONFIG_8808S_AP,
    TGT_RAM_CONFIG_8808S_ETRON,
    TGT_RAM_CONFIG_8808S_WINBOND,
    TGT_RAM_CONFIG_8808S_EMC,
#else
    TGT_MEMD_RAM_CONFIG,
    TGT_RAM_CONFIG_8809_AP,
    TGT_RAM_CONFIG_8809_ETRON,
    TGT_RAM_CONFIG_8809_WINBOND,
    TGT_RAM_CONFIG_8809_EMC,
#endif
};


// =============================================================================
// tgt_GetMemdFlashConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Flash configuration structure.
// =============================================================================
PUBLIC CONST MEMD_FLASH_CONFIG_T* tgt_GetMemdFlashConfig(VOID)
{
    return &g_tgtMemdFlashCfg;
}


// =============================================================================
// tgt_GetMemdRamConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Ram configuration structure.
// =============================================================================
PUBLIC CONST MEMD_RAM_CONFIG_T* tgt_GetMemdRamConfig(VOID)
{
    CONST MEMD_RAM_CONFIG_T* ramCfg;

    ramCfg = &(g_tgtMemdRamCfg[boot_BootSectorGetEbcRamId()]);

    return ramCfg;
}


// =============================================================================
// Key config
// =============================================================================

PROTECTED CONST UINT8 g_tgtKeyMap[] = KEY_MAP;

#ifdef KEY_BOOT_DOWNLOAD
PROTECTED CONST UINT8 g_tgtKeyBootDownload[] = KEY_BOOT_DOWNLOAD;
#endif


PUBLIC UINT8 tgt_GetKeyMapIndex(unsigned char KeyCode)
{
    for (int i=0; i<TGT_KEY_NB; i++)
    {
        if (g_tgtKeyMap[i] == KeyCode)
        {
            return i;
        }
    }
    return 0xFF;
}


PUBLIC VOID tgt_GetBootDownloadKeyList(CONST UINT8 **ppList, UINT32 *pLen)
{
#ifdef KEY_BOOT_DOWNLOAD
    *ppList = g_tgtKeyBootDownload;
    *pLen = sizeof(g_tgtKeyBootDownload)/sizeof(g_tgtKeyBootDownload[0]);
#else
    *ppList = NULL;
    *pLen = 0;
#endif
}


PUBLIC VOID tgt_GetBootUsbDownloadKeyList(CONST UINT8 **ppList, UINT32 *pLen)
{
#ifdef KEY_USB_BOOT_DOWNLOAD
    static CONST UINT8 g_tgtKeyBootUsbDownload[] = KEY_USB_BOOT_DOWNLOAD;
#else
    static CONST UINT8 g_tgtKeyBootUsbDownload[] = { KP_OK };
#endif
    *ppList = g_tgtKeyBootUsbDownload;
    *pLen = sizeof(g_tgtKeyBootUsbDownload)/sizeof(g_tgtKeyBootUsbDownload[0]);
}

PUBLIC BOOL tgt_GetUsbDownloadByMicDet(UINT32 *lo, UINT32 *hi)
{
#ifdef USB_DOWNLOAD_BY_MIC_DET
    *lo = USB_DOWNLOAD_BY_MIC_DET_LO;
    *hi = USB_DOWNLOAD_BY_MIC_DET_HI;
    return TRUE;
#else
    return FALSE;
#endif
}

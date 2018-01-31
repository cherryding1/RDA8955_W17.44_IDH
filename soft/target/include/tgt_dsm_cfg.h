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

#ifndef _TGT_DSM_CFG_H_
#define _TGT_DSM_CFG_H_

#include "tgt_app_cfg.h"

// =============================================================================
// TGT_DSM_PART_CONFIG
// -----------------------------------------------------------------------------
// This structure describes the DSM(Data Storage Mangage) configuration.
/// Field description:
/// szPartName: Partition name string,the max size is 15 bytes.
/// eDevType: can be either DSM_MEM_DEV_FLASH for onboard flash combo or DSM_MEM_DEV_TFLASH
/// eCheckLevel: VDS Module cheking level.
//                        DSM_CHECK_LEVEL1: Check the PBD writing and PB Writing.
//                        DSM_CHECK_LEVEL2: Check the PDB writing only.
//                        DSM_CHECK_LEVEL3: Not check.
/// uSecCnt: Number of sector used by this partition (when relevant)
/// uRsvBlkCnt: Number of reseved block. When want the write speed speedy, increase this field value.
/// eModuleId: Module identification.
//                    DSM_MODULE_FS_ROOT: FS Moudle for root directory.
//                    DSM_MODULE_FS:   FS Moudle for mounting device.
// =============================================================================

#if (TGT_DSM_VDS0_SECCNT > 0)
#define TGT_DSM_PART_CONFIG_VDS0          \
    {                                     \
        .szPartName = "VDS0",             \
        .eDevType = DSM_MEM_DEV_FLASH,    \
        .eCheckLevel = DSM_CHECK_LEVEL_1, \
        .uSecCnt = TGT_DSM_VDS0_SECCNT,   \
        .uRsvBlkCnt = 1,                  \
        .eModuleId = DSM_MODULE_FS_ROOT},
#else
#define TGT_DSM_PART_CONFIG_VDS0 //
#endif

#if (TGT_DSM_VDS1_SECCNT > 0)
#define TGT_DSM_PART_CONFIG_VDS1          \
    {                                     \
        .szPartName = "VDS1",             \
        .eDevType = DSM_MEM_DEV_FLASH,    \
        .eCheckLevel = DSM_CHECK_LEVEL_1, \
        .uSecCnt = TGT_DSM_VDS1_SECCNT,   \
        .uRsvBlkCnt = 1,                  \
        .eModuleId = DSM_MODULE_FS},
#else
#define TGT_DSM_PART_CONFIG_VDS1 //
#endif

#if (TGT_DSM_WAPMMS_SECCNT > 0)
#define TGT_DSM_PART_CONFIG_WAPMMS        \
    {                                     \
        .szPartName = "WAP_MMS",          \
        .eDevType = DSM_MEM_DEV_FLASH,    \
        .eCheckLevel = DSM_CHECK_LEVEL_3, \
        .uSecCnt = TGT_DSM_WAPMMS_SECCNT, \
        .uRsvBlkCnt = 1,                  \
        .eModuleId = DSM_MODULE_FS},
#else
#define TGT_DSM_PART_CONFIG_WAPMMS //
#endif

#if (TGT_DSM_NVRAM_SECCNT > 0)
#define TGT_DSM_PART_CONFIG_NVRAM         \
    {                                     \
        .szPartName = "NVRAM",            \
        .eDevType = DSM_MEM_DEV_FLASH,    \
        .eCheckLevel = DSM_CHECK_LEVEL_1, \
        .uSecCnt = TGT_DSM_NVRAM_SECCNT,  \
        .uRsvBlkCnt = 1,                  \
        .eModuleId = DSM_MODULE_NVRAM},
#else
#define TGT_DSM_PART_CONFIG_NVRAM //
#endif

#if (TGT_DSM_LTENV_SECCNT > 0)
#define TGT_DSM_PART_CONFIG_LTENV         \
    {                                     \
        .szPartName = "LTENV",            \
        .eDevType = DSM_MEM_DEV_FLASH,    \
        .eCheckLevel = DSM_CHECK_LEVEL_1, \
        .uSecCnt = TGT_DSM_LTENV_SECCNT,  \
        .uRsvBlkCnt = 1,                  \
        .eModuleId = DSM_MODULE_LTENV},
#else
#define TGT_DSM_PART_CONFIG_LTENV //
#endif

#if (TGT_DSM_NBPSM_SECCNT > 0)
#define TGT_DSM_PART_CONFIG_NBPSM         \
    {                                     \
        .szPartName = "NBPSM",            \
        .eDevType = DSM_MEM_DEV_FLASH,    \
        .eCheckLevel = DSM_CHECK_LEVEL_1, \
        .uSecCnt = TGT_DSM_NBPSM_SECCNT,  \
        .uRsvBlkCnt = 1,                  \
        .eModuleId = DSM_MODULE_NBPSM},
#else
#define TGT_DSM_PART_CONFIG_NBPSM //
#endif

#if (TGT_DSM_MMC0_ENABLED)
#define TGT_DSM_PART_CONFIG_MMC0          \
    {                                     \
        .szPartName = "MMC0",             \
        .eDevType = DSM_MEM_DEV_TFLASH,   \
        .eCheckLevel = DSM_CHECK_LEVEL_3, \
        .uSecCnt = 0,                     \
        .uRsvBlkCnt = 0,                  \
        .eModuleId = DSM_MODULE_FS},
#else
#define TGT_DSM_PART_CONFIG_MMC0 //
#endif

#define TGT_DSM_PART_CONFIG        \
    {                              \
        TGT_DSM_PART_CONFIG_VDS0   \
        TGT_DSM_PART_CONFIG_VDS1   \
        TGT_DSM_PART_CONFIG_WAPMMS \
        TGT_DSM_PART_CONFIG_NVRAM  \
        TGT_DSM_PART_CONFIG_LTENV  \
        TGT_DSM_PART_CONFIG_NBPSM  \
        TGT_DSM_PART_CONFIG_MMC0   \
    }

#define TGT_DSM_FLASH_SECCNT ( \
    TGT_DSM_VDS0_SECCNT +      \
    TGT_DSM_VDS1_SECCNT +      \
    TGT_DSM_WAPMMS_SECCNT +    \
    TGT_DSM_NVRAM_SECCNT +     \
    TGT_DSM_LTENV_SECCNT +     \
    TGT_DSM_NBPSM_SECCNT +     \
    1)

#define DSM_PART_COUNT (                   \
    (TGT_DSM_VDS0_SECCNT == 0 ? 0 : 1) +   \
    (TGT_DSM_VDS1_SECCNT == 0 ? 0 : 1) +   \
    (TGT_DSM_WAPMMS_SECCNT == 0 ? 0 : 1) + \
    (TGT_DSM_NVRAM_SECCNT == 0 ? 0 : 1) +  \
    (TGT_DSM_LTENV_SECCNT == 0 ? 0 : 1) +  \
    (TGT_DSM_NBPSM_SECCNT == 0 ? 0 : 1) +  \
    (TGT_DSM_MMC0_ENABLED == 0 ? 0 : 1))

#define TGT_DSM_CONFIG                           \
    {                                            \
        .dsmPartitionInfo = TGT_DSM_PART_CONFIG, \
        .dsmPartitionNumber = DSM_PART_COUNT     \
    }

// =============================================================================
// tgt_GetDsmCfg
// -----------------------------------------------------------------------------
// This function is used by DSM to get its configuration structure.
//
// =============================================================================

PUBLIC CONST DSM_CONFIG_T *tgt_GetDsmCfg(VOID);

#endif //_TGT_DSM_CFG_H_

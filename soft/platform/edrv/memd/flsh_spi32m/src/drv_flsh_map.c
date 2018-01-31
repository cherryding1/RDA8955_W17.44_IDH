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
#include "memd_m.h"

#if (SPIFFS_FLASH_START < FOTA_BACKUP_AREA_START + FOTA_BACKUP_AREA_SIZE)
#error "SPIFFS should after FOTA backup area"
#endif

#if (USER_DATA_BASE < SPIFFS_FLASH_START + SPIFFS_FLASH_SIZE)
#error "USER_DATA should after SPIFFS"
#endif

#if (CALIB_BASE < USER_DATA_BASE + USER_DATA_SIZE)
#error "CALIB should be after USER_DATA"
#endif

#if (FACT_SETTINGS_BASE != CALIB_BASE + CALIB_SIZE)
#error "FACTORY should be after CALIB without no gap"
#endif

#if (FLASH_SIZE != FACT_SETTINGS_BASE + FACT_SETTINGS_SIZE)
#error "FACTORY should be at the end of flash"
#endif

// =============================================================================
//  MACROS
// =============================================================================
#define KBYTES 1024
#define MBYTES (1024 * KBYTES)

// -------------------------------
// FLASH Mapping
// -------------------------------
#define NUMBER_OF_BANKS 1
#define BANK0_BASE 0

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
PUBLIC CONST MEMD_BANK_LAYOUT_T g_memdFlashBankLayout[NUMBER_OF_BANKS] = {
    {BANK0_BASE,
     {{63, 64 * KBYTES},
      {16, 4 * KBYTES},
      {0, 0 * KBYTES}}}};
#else
PUBLIC CONST MEMD_BANK_LAYOUT_T g_memdFlashBankLayout[NUMBER_OF_BANKS] = {
    {BANK0_BASE,
     {{USER_DATA_BASE / (64 * KBYTES), 64 * KBYTES},
      {(FLASH_SIZE - USER_DATA_BASE + (USER_DATA_BASE % (64 * KBYTES))) / (4 * KBYTES), 4 * KBYTES},
      {0, 0 * KBYTES}}}};
#endif

PUBLIC CONST MEMD_FLASH_LAYOUT_T g_memdFlashLayout = {
    NUMBER_OF_BANKS,
    g_memdFlashBankLayout,
    FLASH_SIZE,
    USER_DATA_BASE,
    USER_DATA_SIZE,
    USER_DATA_BLOCK_SIZE};

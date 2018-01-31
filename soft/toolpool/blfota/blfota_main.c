/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include <bl_entry.h>
#include <bl_platform.h>
#include <bl_spi_flash.h>
#include <fota/fota.h>
#include <fs/sf_api.h>
#include <stdint.h>
#include <stdbool.h>

#define FOTA_EVENT_START_CHECK 0xf07a0000
#define FOTA_EVENT_VERSION_ERROR 0xf07a0001
#define FOTA_EVENT_STATUS_EMPTY 0xf07a0002
#define FOTA_EVENT_STATUS_DOWNLOADING 0xf07a0003
#define FOTA_EVENT_STATUS_DOWNLOADED 0xf07a0004
#define FOTA_EVENT_STATUS_UPGRADING 0xf07a0005
#define FOTA_EVENT_STATUS_UPGRADED 0xf07a0006
#define FOTA_EVENT_AREA_INVALID 0xf07a0007
#define FOTA_EVENT_UPGRADE_START 0xf07a0008
#define FOTA_EVENT_UPGRADE_PROGRESS 0xf07a0009
#define FOTA_EVENT_UPGRADE_FAILED 0xf07a000a
#define FOTA_EVENT_UPGRADE_DONE 0xf07a000b

#define FOTA_MEM_START (0x82000000 + 0x10000)
#define FOTA_MEM_SIZE (0x400000 - 0x10000)

#define BL_FOTA_ENABLE_WATCHDOG
#define BL_FOTA_WATCHDOG_PERIOD 20

#define FOTA_CONTEXT_WORDS (64)

static uint32_t gBlFotaContextBuf[FOTA_CONTEXT_WORDS];

static bool bl_fota_check(void)
{
    FOTA_CONTEXT_T *fc = (FOTA_CONTEXT_T *)gBlFotaContextBuf;
    FOTA_ENV_T fenv = {
        .packFname = FOTA_PACK_DEFAULT_FILENAME,
        .idxFname = FOTA_INDEX_DEFAULT_FILENAME,
    };

    if (fotaContextSize() > sizeof(gBlFotaContextBuf))
        bl_panic();

    sf_config_t cfg = {
        .phys_start = SPIFFS_FLASH_START,
        .phys_size = SPIFFS_FLASH_SIZE,
        .format_enable = false,
        .check_enable = false,
    };
    if (sf_init(&cfg) < 0)
        return false;

    bl_send_event(FOTA_EVENT_START_CHECK);
    bool result = fotaInit(fc, &fenv);
    if (!result)
    {
        bl_send_event(FOTA_EVENT_VERSION_ERROR);
    }
    else
    {
        FOTA_AREA_STATUS_T status = fotaGetStatus(fc);
        bl_send_event(status - FOTA_AREA_EMPTY + FOTA_EVENT_STATUS_EMPTY);
        result = (status == FOTA_AREA_UPGRADING || status == FOTA_AREA_DOWNLOADED);
    }

    sf_destroy();
    return result;
}

static void bl_fota_progress(uint16_t total, uint16_t current)
{
    bl_send_event(FOTA_EVENT_UPGRADE_PROGRESS);
    bl_send_event((total << 16) | current);
    bl_watchdog_reload();
}

static void bl_fota_upgrade(void)
{
    FOTA_CONTEXT_T *fc = (FOTA_CONTEXT_T *)gBlFotaContextBuf;
    FOTA_ENV_T fenv = {
        .packFname = FOTA_PACK_DEFAULT_FILENAME,
        .idxFname = FOTA_INDEX_DEFAULT_FILENAME,
        .backupStart = FOTA_BACKUP_AREA_START,
        .backupSize = FOTA_BACKUP_AREA_SIZE,
        .upgradeMemAddr = (uint8_t *)FOTA_MEM_START,
        .upgradeMemSize = FOTA_MEM_SIZE,
    };

    if (fotaContextSize() > sizeof(gBlFotaContextBuf))
        bl_panic();

    sf_config_t cfg = {
        .phys_start = SPIFFS_FLASH_START,
        .phys_size = SPIFFS_FLASH_SIZE,
        .format_enable = false,
        .check_enable = true,
    };
    if (sf_init(&cfg) < 0)
        return;

    bool ok = fotaInit(fc, &fenv);
    if (!ok)
    {
        bl_send_event(FOTA_EVENT_VERSION_ERROR);
        goto end;
    }

    bl_send_event(FOTA_EVENT_UPGRADE_START);
    bl_watchdog_reload();
    ok = fotaUpgrade(fc, bl_fota_progress);
    bl_send_event(ok ? FOTA_EVENT_UPGRADE_DONE : FOTA_EVENT_UPGRADE_FAILED);

end:
    sf_destroy();
}

static void bl_fota(void)
{
    if (!bl_fota_check())
        return;

    bl_watchdog_reload();
    bl_init();
    bl_fota_upgrade();
    bl_platform_reset();
}

void bl_main(uint32_t param)
{
#ifdef BL_FOTA_ENABLE_WATCHDOG
    bl_watchdog_start(BL_FOTA_WATCHDOG_PERIOD);
#endif
    bl_fota();
    bl_watchdog_stop();
}

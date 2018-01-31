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

#include "fs/sf_api.h"
#include "spiffs.h"
#include "../src/spiffs_nucleus.h" // HACK to use sizeof(spiffs_fd)
#include "bl_spi_flash.h"
#include "bl_platform.h"
#include <string.h>

#define SF_WORK_BUF_SIZE (SPI_FLASH_PAGE_SIZE * 2)
#define SF_FDS_BUF_SIZE (4 * sizeof(spiffs_fd))

struct bl_spiffs_context
{
    spiffs fs;
    uint32_t work_buf[SF_WORK_BUF_SIZE / 4];
    uint32_t fds_buf[SF_FDS_BUF_SIZE / 4];
};

static struct bl_spiffs_context gBlSfContext;

void sf_fslock_impl(spiffs *fs) {}
void sf_fsunlock_impl(spiffs *fs) {}

static s32_t bl_sf_flash_read(u32_t addr, u32_t size, u8_t *dst)
{
    if (addr < SPIFFS_FLASH_START || (addr + size) > SPIFFS_FLASH_START + SPIFFS_FLASH_SIZE)
        return SPIFFS_ERR_NOT_READABLE;
    memcpy(dst, SPI_FLASH_UNCACHE_ADDRESS(addr), size);
    return SPIFFS_OK;
}

static s32_t bl_sf_flash_write(u32_t addr, u32_t size, u8_t *src)
{
    if (addr < SPIFFS_FLASH_START || (addr + size) > SPIFFS_FLASH_START + SPIFFS_FLASH_SIZE)
        return SPIFFS_ERR_NOT_WRITABLE;
    return bl_SpiFlashWrite(addr, src, size) ? SPIFFS_OK : SPIFFS_ERR_NOT_WRITABLE;
}

static s32_t bl_sf_flash_erase(u32_t addr, u32_t size)
{
    if (addr < SPIFFS_FLASH_START || (addr + size) > SPIFFS_FLASH_START + SPIFFS_FLASH_SIZE)
        return SPIFFS_ERR_NOT_WRITABLE;
    return bl_SpiFlashErase(addr, size) ? SPIFFS_OK : SPIFFS_ERR_NOT_WRITABLE;
}

int sf_init(sf_config_t *cfg)
{
    spiffs *fs = &gBlSfContext.fs;

    spiffs_config scfg = {
        .hal_read_f = bl_sf_flash_read,
        .hal_write_f = bl_sf_flash_write,
        .hal_erase_f = bl_sf_flash_erase,
        .phys_size = cfg->phys_size,
        .phys_addr = cfg->phys_start,
        .phys_erase_block = SPI_FLASH_SECTOR_SIZE,
        .log_block_size = SPI_FLASH_SECTOR_SIZE,
        .log_page_size = SPI_FLASH_PAGE_SIZE,
    };

    int res = SPIFFS_mount(fs, &scfg, (uint8_t *)gBlSfContext.work_buf,
                           (uint8_t *)gBlSfContext.fds_buf, SF_FDS_BUF_SIZE,
                           NULL, 0, 0);
    if (res == 0 && cfg->check_enable)
        SPIFFS_check(fs);
    return res;
}

void sf_destroy()
{
    spiffs *fs = &gBlSfContext.fs;
    SPIFFS_unmount(fs);
}

sf_file sf_open(const char *path, sf_flags flags)
{
    spiffs *fs = &gBlSfContext.fs;
    return (sf_file)SPIFFS_open(fs, path, (spiffs_flags)flags, 0);
}

int sf_close(sf_file fh)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_close(fs, (spiffs_file)fh);
}

int sf_create(const char *path, int mode)
{
    (void)mode;
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_creat(fs, path, 0);
}

int sf_remove(const char *path)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_remove(fs, path);
}

int sf_fremove(sf_file fh)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_fremove(fs, (spiffs_file)fh);
}

int sf_read(sf_file fh, void *buffer, uint32_t len)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_read(fs, (spiffs_file)fh, buffer, (int32_t)len);
}

int sf_write(sf_file fh, const void *data, uint32_t len)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_write(fs, (spiffs_file)fh, (void *)data, (int32_t)len);
}

int sf_rename(const char *old, const char *newph)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_rename(fs, old, newph);
}

int sf_rename_atomic(const char *old, const char *newph)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_rename_atomic(fs, old, newph);
}

int sf_access(const char *path, int mode)
{
    (void)mode;
    spiffs *fs = &gBlSfContext.fs;
    spiffs_file fh = SPIFFS_open(fs, path, SPIFFS_RDONLY, 0);
    SPIFFS_close(fs, fh);
    return fh < 0 ? -1 : 0;
}

int sf_lseek(sf_file fh, int32_t offset, int whence)
{
    spiffs *fs = &gBlSfContext.fs;
    return SPIFFS_lseek(fs, (spiffs_file)fh, offset, whence);
}

static inline void bl_spiffs2sf_stat(struct sf_stat *sf, spiffs_stat *sp)
{
    sf->size = sp->size;
    memcpy(sf->name, sp->name, strlen(sp->name));
}

int sf_stat(const char *path, struct sf_stat *s)
{
    if (s == NULL)
        return -1;

    spiffs *fs = &gBlSfContext.fs;
    spiffs_stat st;
    int r = SPIFFS_stat(fs, path, &st);
    if (r == 0)
        bl_spiffs2sf_stat(s, &st);
    return r ? -1 : 0;
}

int sf_fstat(sf_file fh, struct sf_stat *s)
{
    if (s == NULL)
        return -1;

    spiffs *fs = &gBlSfContext.fs;
    spiffs_stat st;
    int r = SPIFFS_fstat(fs, (spiffs_file)fh, &st);
    if (r == 0)
        bl_spiffs2sf_stat(s, &st);
    return r ? -1 : 0;
}

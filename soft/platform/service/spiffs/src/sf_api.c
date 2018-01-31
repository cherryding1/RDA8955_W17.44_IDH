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

// #define LOCAL_LOG_LEVEL LOG_LEVEL_VERBOSE

#include "fs/sf_api.h"
#include "sf_debug.h"
#include "spiffs.h"
#include "../src/spiffs_nucleus.h" // HACK to use sizeof(spiffs_fd)
#include <string.h>
#include "hal_sys.h"
#include "hal_spi_flash.h"
#include "cos.h"
#include "crc32_calc.h"

#define SF_TEST_CASE 0

#define SF_ASYNC_TASK_STACK_SIZE (1024)
#define SF_ASYNC_TASK_PRIORITY (253)

#undef FLASH_SECTOR_SIZE
#undef LOG_PAGE_SIZE
#define FLASH_SECTOR_SIZE (4 * 1024)
#define LOG_PAGE_SIZE 256
#define SPI_FLASH_OFFSET(address) ((uint32_t)(address)&0x00ffffff)
#define SPI_FLASH_UNCACHE_ADDRESS(offset) ((void *)(SPI_FLASH_OFFSET(offset) | 0xa8000000))

#define SF_WORK_BUF_SIZE (LOG_PAGE_SIZE * 2)
#define SF_FDS_BUF_SIZE (32 * sizeof(spiffs_fd))

struct sf_context
{
    spiffs fs;
    HANDLE task;
    COS_SEMA spiffs_sema;
    uint32_t work_buf[SF_WORK_BUF_SIZE / 4];
    uint32_t fds_buf[SF_FDS_BUF_SIZE / 4];
};

struct sf_safefile_header
{
    unsigned size;
    uint32_t crc;
};

static struct sf_context gSfContext;
#define path2fs(p_) (spiffs *)(&gSfContext.fs)
#define fd2fs(f_) path2fs('/')

#if (SF_TEST_CASE != 0)
static void sf_testcase(void *param);
#endif

void sf_fslock_impl(spiffs *fs)
{
    COS_SemaTake(&gSfContext.spiffs_sema);
}

void sf_fsunlock_impl(spiffs *fs)
{
    COS_SemaRelease(&gSfContext.spiffs_sema);
}

sf_file sf_open(const char *path, sf_flags flags)
{
    spiffs *fs = (spiffs *)path2fs(path);

    spiffs_file f = SPIFFS_open(fs, path, (spiffs_flags)flags, 0);
    SF_LOG(V, "SF open return [%i]", f);
    return (sf_file)f;
}

int sf_close(sf_file fh)
{
    spiffs *fs = (spiffs *)fd2fs(fh);
    s32_t r = SPIFFS_close(fs, (spiffs_file)fh);
    SF_LOG(V, "SF close return [%i]", r);
    return r;
}

int sf_create(const char *path, int mode)
{
    (void)mode;
    spiffs *fs = (spiffs *)path2fs(path);

    int r = SPIFFS_creat(fs, path, 0);
    SF_LOG(V, "SF create return [%i]", r);
    return r;
}

int sf_read(sf_file fh, void *buffer, uint32_t len)
{
    spiffs *fs = (spiffs *)fd2fs(fh);

    int r = SPIFFS_read(fs, (spiffs_file)fh, (uint8_t *)buffer, len);
    SF_LOG(V, "SF read return [%i]", r);
    return r;
}

int sf_write(sf_file fh, const void *data, uint32_t len)
{
    spiffs *fs = (spiffs *)fd2fs(fh);

    int r = SPIFFS_write(fs, (spiffs_file)fh, (void *)data, len);
    SF_LOG(V, "SF write return [%i]", r);
    return r;
}

int sf_lseek(sf_file fh, int32_t offset, int whence)
{
    spiffs *fs = (spiffs *)fd2fs(fh);

    int r = SPIFFS_lseek(fs, (spiffs_file)fh, offset, whence);
    SF_LOG(V, "SF lseek return [%i]", r);
    return r;
}

int sf_remove(const char *path)
{
    spiffs *fs = (spiffs *)path2fs(path);
    return SPIFFS_remove(fs, path);
}

int sf_fremove(sf_file fh)
{
    spiffs *fs = (spiffs *)fd2fs(fh);
    int r = SPIFFS_fremove(fs, (spiffs_file)fh);
    SF_LOG(V, "SF fremove return [%i]", r);
    return r;
}

static inline void spiffs2sf_stat(struct sf_stat *sf, spiffs_stat *sp)
{
    sf->size = sp->size;
    memcpy(sf->name, sp->name, strlen(sp->name));
}

int sf_stat(const char *path, struct sf_stat *s)
{
    if (s == NULL)
        return -1;

    spiffs *fs = (spiffs *)path2fs(path);
    spiffs_stat st;
    int r = SPIFFS_stat(fs, path, &st);
    SF_LOG(V, "SF stat return [%i]", r);
    if (!r)
        spiffs2sf_stat(s, &st);
    return r;
}

int sf_fstat(sf_file fh, struct sf_stat *s)
{
    if (s == NULL)
        return -1;

    spiffs *fs = (spiffs *)fd2fs(fh);
    spiffs_stat st;
    int r = SPIFFS_fstat(fs, (spiffs_file)fh, &st);
    SF_LOG(V, "SF fstat return [%i]", r);
    if (!r)
        spiffs2sf_stat(s, &st);
    return r;
}

int sf_rename(const char *old, const char *newph)
{
    spiffs *fs = (spiffs *)path2fs(old);
    return SPIFFS_rename(fs, old, newph);
}

int sf_rename_atomic(const char *oldpath, const char *newpath)
{
    spiffs *fs = (spiffs *)path2fs(oldpath);
    int r = SPIFFS_rename_atomic(fs, oldpath, newpath);
    SF_LOG(V, "SF rename_safe return [%i]", r);
    return r;
}

int sf_access(const char *pth, int mode)
{
    (void)mode;
    spiffs *fs = (spiffs *)path2fs(pth);
    spiffs_file fh = SPIFFS_open(fs, pth, SPIFFS_RDONLY, 0);
    SPIFFS_close(fs, fh);
    if (fh < 0)
        return -1;
    return 0;
}

static s32_t spi_flash_read(u32_t addr, u32_t size, u8_t *dst)
{
    SF_LOG(V, "SF flash read address [0x%08x], size[%u]", addr, size);
    addr = (uint32_t)SPI_FLASH_UNCACHE_ADDRESS(addr);
    memcpy(dst, (void *)addr, size);
    return SPIFFS_OK;
}

static s32_t spi_flash_write(u32_t addr, u32_t size, u8_t *src)
{
    SF_LOG(V, "SF flash write address [0x%08x], size[%u]", addr, size);
    return hal_SpiFlashWrite(addr, (void *)src, size) ? SPIFFS_OK : SPIFFS_ERR_NOT_WRITABLE;
}

static s32_t spi_flash_erase(u32_t addr, u32_t size)
{
    SF_LOG(V, "SF flash erase address [0x%08x], size[%u]", addr, size);
    return hal_SpiFlashErase(addr, size) ? SPIFFS_OK : SPIFFS_ERR_NOT_WRITABLE;
}

static int spiffs_init_specific(spiffs *fs, sf_config_t *sf_cfg)
{
    spiffs_config cfg;

    cfg.hal_read_f = spi_flash_read;
    cfg.hal_write_f = spi_flash_write;
    cfg.hal_erase_f = spi_flash_erase;
    cfg.phys_size = sf_cfg->phys_size;
    cfg.phys_addr = sf_cfg->phys_start;
    cfg.phys_erase_block = FLASH_SECTOR_SIZE;
    cfg.log_block_size = FLASH_SECTOR_SIZE;
    cfg.log_page_size = LOG_PAGE_SIZE;

    int res = SPIFFS_mount(fs, &cfg, (uint8_t *)gSfContext.work_buf,
                           (uint8_t *)gSfContext.fds_buf, SF_FDS_BUF_SIZE,
                           NULL, 0, 0);
    if (res != SPIFFS_OK)
    {
        if (!sf_cfg->format_enable)
            return res;

        SF_LOG(W, "SF mount failed, try unmounr/format/mount");
        SPIFFS_unmount(fs);
        res = SPIFFS_format(fs);
        if (res != SPIFFS_OK)
        {
            SF_LOG(W, "SF format failed");
            return res;
        }

        res = SPIFFS_mount(fs, &cfg, (uint8_t *)gSfContext.work_buf,
                           (uint8_t *)gSfContext.fds_buf, SF_FDS_BUF_SIZE,
                           NULL, 0, 0);
        if (res != SPIFFS_OK)
            SF_LOG(W, "SF mount failed");
        if (res == SPIFFS_OK && sf_cfg->check_enable)
            SPIFFS_check(fs);
    }

    return res;
}

static void sf_async_task_entry(void *param)
{
    HANDLE task = COS_GetCurrentTaskHandle();

    for (;;)
    {
        COS_EVENT event = {};
        COS_WaitEvent(task, &event, COS_WAIT_FOREVER);

        SF_LOG(E, "SF async unknown event %08x/%08x/%08x/%08x",
               event.nEventId, event.nParam1, event.nParam2, event.nParam3);
    }
}

int sf_init(sf_config_t *cfg)
{
    SF_LOG(V, "SF init file system");
    spiffs *fs = path2fs('/'); // cfg->mount_point

    COS_SemaInit(&gSfContext.spiffs_sema, 1);
    gSfContext.task = COS_CreateTask(sf_async_task_entry,
                                     NULL, NULL,
                                     SF_ASYNC_TASK_STACK_SIZE, SF_ASYNC_TASK_PRIORITY,
                                     COS_CREATE_DEFAULT, 0, "sf_async");

#if (SF_TEST_CASE != 0)
    COS_StartCallbackTimer(gSfContext.task, 5000, sf_testcase, NULL);
#endif

    uint32_t sc = hal_SysEnterCriticalSection();
    int ret = spiffs_init_specific(fs, cfg);
    hal_SysExitCriticalSection(sc);

    return ret;
}

HANDLE sf_async_task_handle(void)
{
    return gSfContext.task;
}

void sf_destroy()
{
    // foreach partition; do ..
    spiffs *fs = path2fs('/');
    if (!SPIFFS_mounted(fs))
        return;
    SPIFFS_unmount(fs);
}

static bool sf_safefile_backup_name(const char *path, char *backup)
{
    if (path == NULL)
        return false;

    int pathlen = strlen(path);
    if (pathlen >= SF_OBJ_NAME_LEN - 1)
        return false;

    memcpy(backup, path, pathlen);
    backup[pathlen] = '@';
    backup[pathlen + 1] = '\0';
    return true;
}

bool sf_safefile_data_valid(const char *path)
{
    sf_file fd = sf_open(path, SF_RDONLY);
    if (fd < 0)
        return -1;

    struct sf_stat st;
    if (sf_fstat(fd, &st) < 0)
        goto failed_close;

    struct sf_safefile_header header;
    if (sf_read(fd, &header, sizeof(header)) != sizeof(header))
        goto failed_close;

    if (st.size != header.size + sizeof(header))
    {
        goto failed_close;
    }

    uint8_t buffer[32];
    uint32_t crc = crc32_init();
    int len = header.size;
    while (len > 0)
    {
        int trans = (len > 32) ? 32 : len;
        if (sf_read(fd, buffer, trans) != trans)
            goto failed_close;

        crc = crc32_update(crc, buffer, trans);
        len -= trans;
    }

    if (header.crc != crc)
        goto failed_close;

    sf_close(fd);
    return true;

failed_close:
    sf_close(fd);
    return false;
}

bool sf_safefile_init(const char *path)
{
    char backup[SF_OBJ_NAME_LEN];
    if (!sf_safefile_backup_name(path, backup))
        return false;

    bool valid;
    bool pathexist = (sf_access(path, 0) < 0) ? false : true;
    bool backupexist = (sf_access(backup, 0) < 0) ? false : true;
    SF_LOGS(I, TSM(0), "SF safefile %s init, exist %d/%d", path, pathexist, backupexist);

    if (pathexist)
    {
        if (backupexist)
        {
            // Be conservative to remove one
            if (sf_safefile_data_valid(path))
            {
                SF_LOGS(I, TSM(0), "SF safefile %s init, remove backup", path);
                sf_remove(backup);
                return true;
            }
            SF_LOGS(I, TSM(0), "SF safefile %s init, rename backup", path);
            sf_rename_atomic(backup, path);
        }
        goto check_valid;
    }

    if (backupexist)
    {
        SF_LOGS(I, TSM(0), "SF safefile %s init, rename backup", path);
        sf_rename_atomic(backup, path);
        goto check_valid;
    }

    SF_LOGS(I, TSM(0), "SF safefile %s init, create empty", path);
    sf_create(path, 0);
    return false;

check_valid:
    valid = sf_safefile_data_valid(path);
    SF_LOGS(I, TSM(0), "SF safefile %s init, valid/%d", path, valid);
    return valid;
}

int sf_safefile_data_size(const char *path)
{
    struct sf_stat st;
    if (sf_stat(path, &st) < 0)
        return -1;

    return ((int)st.size - (int)sizeof(struct sf_safefile_header));
}

int sf_safefile_read(const char *path, void *buffer, uint32_t len)
{
    sf_file fd = sf_open(path, SF_RDONLY);
    if (fd < 0)
    {
        SF_LOGS(E, TSM(0), "SF safefile failed to open %s for read", path);
        return -1;
    }

    struct sf_stat st;
    if (sf_fstat(fd, &st) < 0)
        goto failed_close;

    struct sf_safefile_header header;
    if (sf_read(fd, &header, sizeof(header)) != sizeof(header))
        goto failed_close;

    if (st.size != header.size + sizeof(header))
        goto failed_close;

    if (len < header.size)
        goto failed_close;

    if (sf_read(fd, buffer, header.size) != header.size)
        goto failed_close;

    if (header.crc != crc32_calc(buffer, header.size))
        goto failed_close;

    sf_close(fd);
    return header.size;

failed_close:
    SF_LOGS(E, TSM(0), "SF safefile read %s failed", path);
    sf_close(fd);
    return -1;
}

bool sf_safefile_write(const char *path, const void *data, uint32_t len)
{
    char backup[SF_OBJ_NAME_LEN];
    if (!sf_safefile_backup_name(path, backup))
    {
        SF_LOG(E, "SF safefile bad file name");
        return false;
    }

    sf_file fd = sf_open(backup, SF_CREAT | SF_TRUNC | SF_RDWR);
    if (fd < 0)
    {
        SF_LOGS(E, TSM(0), "SF safefile failed to open %s for write", backup);
        return false;
    }

    uint32_t crc = crc32_calc(data, len);
    struct sf_safefile_header header = {len, crc};
    if (sf_write(fd, &header, sizeof(header)) != sizeof(header))
    {
        SF_LOG(E, "SF safefile failed to write header");
        goto failed_close;
    }

    if (sf_write(fd, data, len) != len)
    {
        SF_LOG(E, "SF safefile failed to write data");
        goto failed_close;
    }

    sf_close(fd);

    if (!sf_safefile_data_valid(backup))
    {
        SF_LOG(E, "SF safefile failed to check backup");
        return false;
    }

    if (sf_rename_atomic(backup, path) < 0)
    {
        SF_LOG(E, "SF safefile failed to rename");
        return false;
    }

    return true;

failed_close:
    sf_close(fd);
    return false;
}

#if (SF_TEST_CASE == 1)
static uint64_t sf_test_read_file(const char *fname)
{
    sf_file fp = sf_open(fname, SF_RDONLY);
    if (fp < 0)
        return 0;
    uint64_t data = 0;
    int len = sf_read(fp, &data, 8);
    sf_close(fp);

    if (len != 8)
        COS_Assert(0, "SF test read size mismatch");
    return data;
}

static void sf_test_write_file(const char *fname, uint64_t data)
{
    sf_file fp = sf_open(fname, SF_RDWR | SF_CREAT | SF_TRUNC);
    if (fp < 0)
        COS_Assert(0, "SF test fail to open file for write");
    int len = sf_write(fp, &data, 8);
    sf_close(fp);

    if (len != 8)
        COS_Assert(0, "SF test write size mismatch");
}

static uint64_t g_sf_test_data = 0;
static void sf_testcase(void *param)
{
    const char *fname = "test.bin";
    const char *fbname = "test.bin@";

    sf_remove(fname);
    sf_remove(fbname);
    for (;;)
    {
        if (sf_test_read_file(fname) != g_sf_test_data &&
            sf_test_read_file(fbname) == g_sf_test_data)
        {
            sf_rename_atomic(fbname, fname);
        }

        g_sf_test_data++;

        sf_test_write_file(fbname, g_sf_test_data);
        if (sf_test_read_file(fbname) != g_sf_test_data)
            COS_Assert(0, "SF test write backup content mismatch");
        if (sf_rename_atomic(fbname, fname) < 0)
            COS_Assert(0, "SF test atomic rename failed");
        if (sf_test_read_file(fname) != g_sf_test_data)
            COS_Assert(0, "SF test atomic rename content mismatch");
    }
}

#endif

#if (SF_TEST_CASE == 2)
#define TEST_MAX_FILE_SIZE (128*1024)
static void sf_testcase(void *param)
{
    const char *fname = "test.bin";
    uint32_t *data = (uint32_t *)COS_Malloc(TEST_MAX_FILE_SIZE, 0);
    uint32_t *datav = (uint32_t *)COS_Malloc(TEST_MAX_FILE_SIZE, 0);
    for (;;)
    {
        for (int size = 1024; size <= TEST_MAX_FILE_SIZE; size *= 2)
        {
            for (int n = 0; n < size / 4; n++)
                data[n] = (uint32_t)rand();

            uint32_t total, used;
            SPIFFS_info(&gSfContext.fs, &total, &used);
            SF_LOG(I, "SF TEST total %d used %d", total, used);

            uint32_t stick = COS_GetTickCount();
            sf_file fp = sf_open(fname, SF_RDWR | SF_CREAT | SF_TRUNC);
            COS_Assert(fp > 0, "Failed to create file");
            int wsize = sf_write(fp, data, size);
            COS_Assert(wsize == size, "Failed to write %d wsize %d", size, wsize);
            sf_close(fp);
            uint32_t etick = COS_GetTickCount();
            SF_LOG(I, "SF TEST write %d, tick %d", size, etick - stick);

            stick = COS_GetTickCount();
            fp = sf_open(fname, SF_RDONLY);
            COS_Assert(fp > 0, "Failed to read file");
            int rsize = sf_read(fp, datav, size);
            COS_Assert(rsize == size, "Failed to read %d rsize %d", size, rsize);
            sf_close(fp);
            etick = COS_GetTickCount();
            COS_Assert(memcmp(data, datav, size) == 0, "Cmp failed");
            SF_LOG(I, "SF TEST read %d, tick %d", size, etick - stick);

            stick = COS_GetTickCount();
            int res = sf_remove(fname);
            COS_Assert(res == 0, "Failed to read file");
            etick = COS_GetTickCount();
            SF_LOG(I, "SF TEST remove %d, tick %d", size, etick - stick);
        }
    }
}
#endif

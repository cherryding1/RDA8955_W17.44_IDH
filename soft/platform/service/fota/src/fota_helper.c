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

#include <fota/fota.h>
#include <stdint.h>
#include <hal_spi_flash.h>
#include <fs/sf_api.h>

#define SPI_FLASH_OFFSET(address) ((uint32_t)(address)&0x00ffffff)
#define SPI_FLASH_UNCACHE_ADDRESS(offset) ((void *)(SPI_FLASH_OFFSET(offset) | 0xa8000000))

void *fotaFlashUncacheMap(unsigned address)
{
    return SPI_FLASH_UNCACHE_ADDRESS(address);
}

void fotaFlashErase(unsigned address, unsigned length)
{
    hal_SpiFlashErase(address, length);
}

void fotaFlashProgram(unsigned address, void *data, unsigned length)
{
    hal_SpiFlashWrite(address, data, length);
}

void fotaFlashEraseProgram(unsigned address, void *data, unsigned length)
{
    hal_SpiFlashErase(address, length);
    hal_SpiFlashWrite(address, data, length);
}

bool fotaFlashIsEmpty(unsigned address, unsigned size)
{
    uint8_t *p = SPI_FLASH_UNCACHE_ADDRESS(address);
    while (size--)
    {
        if (*p++ != 0xff)
            return false;
    }
    return true;
}

int fotaFileOpen(const char *fn, bool new_wr)
{
    sf_flags flag = new_wr ? (SF_RDWR | SF_CREAT | SF_TRUNC) : SF_RDONLY;
    sf_file fh = sf_open(fn, flag);
    if (fh < 0)
        return FOTA_FILE_INVALID;

    return (int)fh;
}

void fotaFileClose(int fp)
{
    sf_file fh = (sf_file)fp;
    sf_close(fh);
}

int fotaFileWrite(int fp, uint32_t offset, const void *data, uint32_t length)
{
    sf_file fh = (sf_file)fp;
    int ret = sf_lseek(fh, offset, SF_SEEK_SET);
    if (ret < 0)
        return -1;
    ret = sf_write(fh, (void *)data, length);
    return ret;
}

int fotaFileRead(int fp, uint32_t offset, void *data, uint32_t length)
{
    sf_file fh = (sf_file)fp;
    int ret = sf_lseek(fh, offset, SF_SEEK_SET);
    if (ret < 0)
        return -1;
    ret = sf_read(fh, data, length);
    return ret;
}

int fotaFileRenameAtomic(const char *oldpath, const char *newpath)
{
    int ret = sf_rename_atomic(oldpath, newpath);
    return ret;
}

int fotaFileRemove(const char *path)
{
    int ret = sf_remove(path);
    return (ret == SF_ERR_NOT_FOUND ? 0 : ret);
}

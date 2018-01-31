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

#include <bl_spi_flash.h>
#include <fota/fota.h>

void *fotaFlashUncacheMap(unsigned address)
{
    return SPI_FLASH_UNCACHE_ADDRESS(address);
}

void fotaFlashErase(unsigned address, unsigned length)
{
    bl_SpiFlashErase(address, length);
}

void fotaFlashProgram(unsigned address, void *data, unsigned length)
{
    bl_SpiFlashWrite(address, data, length);
}

void fotaFlashEraseProgram(unsigned address, void *data, unsigned length)
{
    bl_SpiFlashErase(address, length);
    bl_SpiFlashWrite(address, data, length);
}

#include <fs/sf_api.h>

int fotaFileOpen(const char *fn, bool new_wr)
{
    sf_flags flags = new_wr ? (SF_RDWR | SF_CREAT | SF_TRUNC) : SF_RDONLY;
    sf_file f = sf_open(fn, flags);
    if (f < 0)
        return FOTA_FILE_INVALID;
    return (int)f;
}

void fotaFileClose(int fp)
{
    sf_close((sf_file)fp);
}

int fotaFileWrite(int fp, uint32_t offset, const void *data, uint32_t length)
{
    sf_file f = (sf_file)fp;
    int len = sf_lseek(f, offset, SF_SEEK_SET);
    if (len != offset)
        return -1;
    len = sf_write(f, data, length);
    return len;
}

int fotaFileRead(int fp, uint32_t offset, void *buffer, uint32_t length)
{
    sf_file f = (sf_file)fp;
    int len = sf_lseek(f, offset, SF_SEEK_SET);
    if (len != offset)
        return -1;
    len = sf_read(f, buffer, length);
    return len;
}

int fotaFileRenameAtomic(const char *oname, const char *nname)
{
    return sf_rename_atomic(oname, nname);
}

int fotaFileRemove(const char *name)
{
    int ret = sf_remove(name);
    return (ret == SF_ERR_NOT_FOUND ? 0 : ret);
}

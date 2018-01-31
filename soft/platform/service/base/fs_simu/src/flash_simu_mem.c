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

#if (defined _DSM_SIMUFLASH_MEMORY)

#include "string.h"
#include "dsm_cf.h"
#include "drv_flash.h"
#include "flash_simu_mem.h"

#define _STDIN 1

#if defined(CHIP_HAS_AP)
#define MODEM_RAM_USER_BASEADDR (0)
#define MODEM_RAM_USER_TOTALSIZE (MODEM_RAM_USER_SECTORSIZE*MODEM_RAM_USER_SECTORCOUNT)
#define MODEM_RAM_USER_SECTORCOUNT 35
#define MODEM_RAM_USER_SECTORSIZE (4*1024)
#define MODEM_RAM_USER_BLOCKSIZE (512)
#elif defined(MMI_ON_WIN32)
#define USERGEN_FLASH_BASEADDR (0)
#define USERGEN_FLASH_TOTALSIZE (USERGEN_FLASH_SECTORSIZE*USERGEN_FLASH_SECTORCOUNT)
#define USERGEN_FLASH_SECTORCOUNT 252
#define USERGEN_FLASH_SECTORSIZE (4*1024)
#define USERGEN_FLASH_BLOCKSIZE (512)
#else
extern   VOID memd_FlashGetUserFieldInfo(UINT32 *userFieldBase, UINT32 *userFieldSize, UINT32 *sectorSize, UINT32 *blockSize);
#endif

BOOL g_IsFlashSimuStrictCheck = TRUE;
DRV_FLASH_DEV_INFO g_sUFlashInfo = {0, 0, 0, 0};
UINT8 *g_pFlashImg = NULL;
UINT32 g_sector_size = 0;

UINT32 DRV_FlashPowerUp(VOID)
{
    UINT32 iBaseAddr = 0;
    UINT32 iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 0;
#if defined(CHIP_HAS_AP)
    iBaseAddr = MODEM_RAM_USER_BASEADDR;
    iTotalSize = MODEM_RAM_USER_TOTALSIZE;
    iSectorSize = MODEM_RAM_USER_SECTORSIZE;
    iBlockSize = MODEM_RAM_USER_BLOCKSIZE;
#elif defined(MMI_ON_WIN32)
    iBaseAddr = USERGEN_FLASH_BASEADDR;
    iTotalSize = USERGEN_FLASH_TOTALSIZE;
    iSectorSize = USERGEN_FLASH_SECTORSIZE;
    iBlockSize = USERGEN_FLASH_BLOCKSIZE;
#else
    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    iBaseAddr = 0;
#endif
#if defined(CHIP_HAS_AP)
    g_pFlashImg = DSM_MAlloc(iTotalSize);
#else
    g_pFlashImg = malloc(iTotalSize);
#endif
    if(NULL == g_pFlashImg)
    {
        DSM_ASSERT(0, "drv_flash_init: malloc(0x%x) fail.\n", iTotalSize);
        return -1;
    }
    DSM_MemSet(g_pFlashImg, 0xff, iTotalSize);
    return ERR_SUCCESS;
}
UINT32 DRV_FlashPowerDown(VOID)
{
    if(g_pFlashImg)
    {
#if defined(CHIP_HAS_AP)
        DSM_Free(g_pFlashImg);
#else
        free(g_pFlashImg);
#endif
    }
    return ERR_SUCCESS;
}

BOOL DRV_FlashInit(VOID)
{
    UINT32 iBaseAddr = 0;
    UINT32 iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 0;

#if defined(CHIP_HAS_AP)
    iBaseAddr = MODEM_RAM_USER_BASEADDR;
    iTotalSize = MODEM_RAM_USER_TOTALSIZE;
    iSectorSize = MODEM_RAM_USER_SECTORSIZE;
    iBlockSize = MODEM_RAM_USER_BLOCKSIZE;
#elif defined(MMI_ON_WIN32)
    iBaseAddr = USERGEN_FLASH_BASEADDR;
    iTotalSize = USERGEN_FLASH_TOTALSIZE;
    iSectorSize = USERGEN_FLASH_SECTORSIZE;
    iBlockSize = USERGEN_FLASH_BLOCKSIZE;
#else
    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    iBaseAddr = 0;
#endif

    g_sUFlashInfo.block_size = iBlockSize;
    g_sUFlashInfo.pBaseAddr = (UINT8 *)iBaseAddr;
    g_sUFlashInfo.sector_size = iSectorSize;
    g_sUFlashInfo.total_size = iTotalSize;

    g_sector_size = iSectorSize;

        // CSW_TRACE(_CSW|TLEVEL(_STDIN)|TDB|TNB_ARG(1), TSTR("drv_flash_init: malloc(0x%x) fail.\n",0x080000c7), iTotalSize);
    return ERR_SUCCESS;

}


UINT32 DRV_FlashRegionMap( DRV_FLASH_DEV_INFO *pParam )
{
    if ( pParam == NULL )
    {
        return -1;
    }
    *pParam = g_sUFlashInfo;
    return 0;
}

UINT32 DRV_ReadFlash (
    UINT32 ulAddrOffset,
    BYTE *pBuffer,
    UINT32 nNumberOfBytesToRead,
    UINT32 *pNumberOfBytesRead
)
{
    UINT8 *p;

    if((ulAddrOffset + nNumberOfBytesToRead) > g_sUFlashInfo.total_size)
    {
        DSM_ASSERT(0, "DRV_ReadFlash: read addr is overflow. offs = 0x%x,read_size = 0x%x,total_size = 0x%x.", ulAddrOffset, nNumberOfBytesToRead, g_sUFlashInfo.total_size);
        CSW_TRACE(_CSW|TLEVEL(_STDIN)|TDB|TNB_ARG(3), TSTR("DRV_ReadFlash: read addr is overflow. offs = 0x%x,read_size = 0x%x,total_size = 0x%x.",0x080000c8), ulAddrOffset, nNumberOfBytesToRead, g_sUFlashInfo.total_size);
        return -1;
    }

    p = g_pFlashImg + ulAddrOffset;

    memcpy(pBuffer, p, nNumberOfBytesToRead);
    *pNumberOfBytesRead = nNumberOfBytesToRead;
    return 0;
}


UINT32 DRV_WriteFlash( UINT32 ulAddrOffset, CONST BYTE *pBuffer,
                       UINT32 nNumberOfBytesToWrite, UINT32 *pNumberOfBytesWritten )
{
    UINT32 byte4R;
    UINT32 byte4W;
    UINT8 byte1R;
    UINT8 byte1W;
    UINT32 i;
    INT32 iRet;
    UINT8 *p;

    if((ulAddrOffset + nNumberOfBytesToWrite) > g_sUFlashInfo.total_size)
    {
        DSM_ASSERT(0, "DRV_WriteFlash: read addr is overflow. offs = 0x%x,read_size = 0x%x,total_size = 0x%x.", ulAddrOffset, nNumberOfBytesToWrite, g_sUFlashInfo.total_size);
        CSW_TRACE(_CSW|TLEVEL(_STDIN)|TDB|TNB_ARG(3), TSTR("DRV_WriteFlash: read addr is overflow. offs = 0x%x,read_size = 0x%x,total_size = 0x%x.",0x080000c9), ulAddrOffset, nNumberOfBytesToWrite, g_sUFlashInfo.total_size);
        return -1;
    }
    p = g_pFlashImg + ulAddrOffset;
    if ( g_IsFlashSimuStrictCheck )
    {
        for ( i = 0; i + 4 <= nNumberOfBytesToWrite; i += 4 )
        {
            byte4R = *( ( UINT32 * ) ( p + i ) );
            byte4W = *( ( UINT32 * ) ( pBuffer + i ) );
            byte4R = ~byte4R;
            if ( byte4R & byte4W )
            {
                iRet = -2;
                goto _write_flash_fun;
            }
        }

        if ( 0 != i )
        {
            ++i;
        }
        while ( i < nNumberOfBytesToWrite )
        {
            byte1R = *( ( UINT8 * ) ( p + i ) );
            byte1W = *( ( UINT8 * ) ( pBuffer + i ) );
            byte1R = ( UINT8 ) ~byte1R;
            if ( byte1R & byte1W )
            {
                iRet = -3;
                goto _write_flash_fun;
            }

            ++i;
        }
    }

    memcpy(p, pBuffer, nNumberOfBytesToWrite);
    *pNumberOfBytesWritten = nNumberOfBytesToWrite;
    iRet = 0;

_write_flash_fun:
    return iRet;
}

UINT32 DRV_EraseFlashSector (
    UINT32 ulAddrOffset
)
{
    UINT8 *p;

    if ( 0 != (( ulAddrOffset - VFLASH_BASE_ADDR ) % g_sUFlashInfo.sector_size) ||
            (ulAddrOffset + g_sUFlashInfo.sector_size) > g_sUFlashInfo.total_size)
    {
        return -1;
    }
    p = g_pFlashImg + ulAddrOffset;
    memset(p, 0xFF, g_sUFlashInfo.sector_size);
    return ERR_SUCCESS;
}

UINT32 DRV_EraseAllUserDataSector(VOID)
{
    DRV_FLASH_DEV_INFO sFlashInfo;
    UINT32 iSecNr = 0;
    UINT32 iSecAddr;
    INT32 iResult;
    UINT32 i;

    iResult = DRV_FlashRegionMap( &sFlashInfo );
    if ( ERR_SUCCESS != iResult )
    {
        CSW_TRACE(_CSW|TLEVEL(_STDIN)|TDB|TNB_ARG(1),  TSTR("DRV_EraseAllUserDataSector: get flash information failed err code = %d.",0x080000ca), iResult);
        return 1;
    }
    iSecNr = sFlashInfo.total_size / sFlashInfo.sector_size;

    for(i = 0; i < iSecNr; i++)
    {
        iSecAddr = i * sFlashInfo.sector_size;
        iResult = DRV_EraseFlashSector(iSecAddr);
        if(ERR_SUCCESS !=  iResult)
        {
            CSW_TRACE(_CSW|TLEVEL(_STDIN)|TDB|TNB_ARG(2),  TSTR("DRV_EraseAllUserDataSector: erase sector failed err code = %d, addr = 0x%x.",0x080000cb), iResult, iSecAddr);
            return 2;
        }
    }
    return 0;
}

VOID DRV_FlashGetUserFieldInfo(UINT32 *userFieldBase, UINT32 *userFieldSize, UINT32 *sectorSize, UINT32 *blockSize)
{
    //UINT32 Start, End;

    *userFieldBase = (UINT32)g_sUFlashInfo.pBaseAddr;
    *userFieldSize = g_sUFlashInfo.total_size;
    *sectorSize = g_sUFlashInfo.sector_size;
    *blockSize = g_sUFlashInfo.block_size;
}
#endif

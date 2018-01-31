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

#if (defined _DSM_SIMUFLASH_FILE)

#include "dsm_cf.h"


#define FLASH_SECTOR_COUNT             252 // 17 //5
#define FLASH_SECTOR_SIZE                (4*1024)

#define FLASH_BASE_ADDR 0x00000000
#define FLASH_BLOCK_SIZE 512


int g_sector_size = FLASH_SECTOR_SIZE;
BOOL g_IsFlashSimuStrictCheck = TRUE;
UINT32 g_TstCtrSize = 0;

DRV_FLASH_DEV_INFO g_sUFlashInfo = {0, 0, 0, 0};

extern UINT32 g_iDevDescripCount;
extern UINT32 tgt_GetSectCount();

BOOL DRV_FlashInit(VOID)
{
    UINT32 iBaseAddr = FLASH_BASE_ADDR;
    UINT32 iTotalSize = (FLASH_SECTOR_COUNT * FLASH_SECTOR_SIZE);
    UINT32 iSectorSize = FLASH_SECTOR_SIZE;
    UINT32 iBlockSize = FLASH_BLOCK_SIZE;

    //memd_FlashGetUserFieldInfo(&iBaseAddr,&iTotalSize,&iSectorSize,&iBlockSize);
    g_sUFlashInfo.block_size = iBlockSize;
    g_sUFlashInfo.pBaseAddr = (UINT8 *)iBaseAddr;
    g_sUFlashInfo.sector_size = iSectorSize;

    g_sector_size = iSectorSize;
    g_sUFlashInfo.total_size = iSectorSize * tgt_GetSectCount();
    return TRUE;
}

UINT32 DRV_FlashRegionMap( DRV_FLASH_DEV_INFO *pParam )
{
    if ( pParam == NULL )
        return ERR_INVALID_PARAMETER;


    pParam->pBaseAddr = FLASH_BASE_ADDR;
    pParam->total_size = g_sector_size * tgt_GetSectCount();
    pParam->sector_size = g_sector_size;
    pParam->block_size = FLASH_BLOCK_SIZE;
    return ERR_SUCCESS;
}


UINT32 DRV_ReadFlash( UINT32 ulAddrOffset, BYTE *pBuffer,
                      UINT32 nNumberOfBytesToRead, UINT32 *pNumberOfBytesRead )
{
    INT32 fd;

    fd = _open( FLASH_SIMU_FILE, OPEN_FLAG_FOR_FLASH, MODE_FOR_FLASH );
    if ( -1 == fd )
    {
        return ERR_FAILURE;
    }

    if ( -1 == _lseeki64( fd, ulAddrOffset, SEEK_SET ) )
    {
        _close( fd );
        return ERR_FAILURE;
    }

    *pNumberOfBytesRead = _read( fd, pBuffer, nNumberOfBytesToRead );

    _close( fd );
    return ( *pNumberOfBytesRead >= 0 ) ? ERR_SUCCESS : ERR_FAILURE;
}


UINT32 DRV_WriteFlash( UINT32 ulAddrOffset, CONST BYTE *pBuffer,
                       UINT32 nNumberOfBytesToWrite, UINT32 *pNumberOfBytesWritten )
{
    BYTE *pBufferR = NULL;
    UINT32 byte4R;
    UINT32 byte4W;
    UINT8 byte1R;
    UINT8 byte1W;
    UINT32 i;
    UINT32 numOfBytesRead;
    INT32 fd;
    UINT32 iRet;


    fd = _open( FLASH_SIMU_FILE, OPEN_FLAG_FOR_FLASH, MODE_FOR_FLASH );
    if ( -1 == fd )
    {
        return ERR_FAILURE;
    }

    if ( -1 == _lseeki64( fd, ulAddrOffset, SEEK_SET ) )
    {
        iRet = ERR_FAILURE;
        goto step0_failed;
    }

    if ( g_IsFlashSimuStrictCheck )
    {
        pBufferR = ( BYTE * ) DSM_MAlloc( nNumberOfBytesToWrite );
        if ( !pBufferR )
        {
            iRet = ERR_FAILURE;
            goto step0_failed;
        }

        numOfBytesRead = _read( fd, pBufferR, nNumberOfBytesToWrite );
        if ( nNumberOfBytesToWrite != numOfBytesRead )
        {
            iRet = ERR_FAILURE;
            goto step1_failed;
        }

        for ( i = 0; i + 4 <= nNumberOfBytesToWrite; i += 4 )
        {
            byte4R = *( ( UINT32 * ) ( pBufferR + i ) );
            byte4W = *( ( UINT32 * ) ( pBuffer + i ) );
            byte4R = ~byte4R;
            if ( byte4R & byte4W )
            {
                iRet = ERR_VFLASH_WRITE_FAIL;
                goto step1_failed;
            }
        }

        if ( 0 != i )
        {
            ++i;
        }
        while ( i < nNumberOfBytesToWrite )
        {
            byte1R = *( ( UINT8 * ) ( pBufferR + i ) );
            byte1W = *( ( UINT8 * ) ( pBuffer + i ) );
            byte1R = ( UINT8 ) ~byte1R;
            if ( byte1R & byte1W )
            {
                iRet = ERR_VFLASH_WRITE_FAIL;
                goto step1_failed;
            }

            ++i;
        }

        if ( -1 == _lseeki64( fd, ulAddrOffset, SEEK_SET ) )
        {
            iRet = ERR_FAILURE;
            goto step1_failed;
        }
    }
    *pNumberOfBytesWritten = _write( fd, pBuffer, nNumberOfBytesToWrite );

    iRet = ERR_SUCCESS;

step1_failed:
    if ( g_IsFlashSimuStrictCheck )
        DSM_Free( pBufferR );
step0_failed:
    _close( fd );
    return iRet;
}




INT32 DRV_EraseFlashSector( UINT32 ulAddrOffset )
{
#define ERASE_BUF_SIZE 32

    INT8 buf[ ERASE_BUF_SIZE ];
    INT32 iOffset;
    INT32 fd;

    if ( 0 != ( ulAddrOffset - FLASH_BASE_ADDR ) % g_sector_size ||
            0 != g_sector_size % ERASE_BUF_SIZE )
    {
        return ERR_INVALID_PARAMETER;
    }

    fd = _open( FLASH_SIMU_FILE, OPEN_FLAG_FOR_FLASH, MODE_FOR_FLASH );
    if ( -1 == fd )
    {
        return ERR_FAILURE;
    }

    if ( -1 == _lseeki64( fd, ulAddrOffset, SEEK_SET ) )
    {
        _close( fd );
        return ERR_FAILURE;
    }

    memset( buf, 0xFF, ERASE_BUF_SIZE );
    for ( iOffset = 0; iOffset < g_sector_size; iOffset += ERASE_BUF_SIZE )
    {
        if ( ERASE_BUF_SIZE != _write( fd, buf, ERASE_BUF_SIZE ) )
        {
            _close( fd );
            return ERR_FAILURE;
        }
    }

    _close( fd );

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
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1),  TSTR("DRV_EraseAllUserDataSector: get flash information failed err code = %d.",0x080000c5), iResult);
        return 1;
    }
    iSecNr = sFlashInfo.total_size / sFlashInfo.sector_size;

    for(i = 0; i < iSecNr; i++)
    {
        iSecAddr = i * sFlashInfo.sector_size;
        iResult = DRV_EraseFlashSector(iSecAddr);
        if(ERR_SUCCESS !=  iResult)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2),  TSTR("DRV_EraseAllUserDataSector: erase sector failed err code = %d, addr = 0x%x.",0x080000c6), iResult, iSecAddr);
            return 2;
        }
    }
    return 0;
}


#endif


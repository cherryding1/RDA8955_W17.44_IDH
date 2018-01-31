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

#include "dsm_cf.h"
#ifdef _FS_SIMULATOR_
#else
#include <base_prv.h>
#include <drv_flash.h>
#include "ts.h"
#endif

#include "dsm_dbg.h"
#include "vds_cd.h"
#include "vds_dbg.h"
#include "vds_local.h"
#include "vds_tools.h"

// extern variable declaring.
// partiton table.
extern VDS_PARTITION g_VDS_partition_table[];

// partition count.
extern UINT32 g_vds_partition_count;

// sector ref chain.
extern VDS_SECT_REF *g_psSectRef;

// remain sector chain.
extern VDS_SECT_REF *g_psRemainRef;

// flash information.
extern VDS_FLASH_INFO g_sFlashInfo;

// vds semaphore handle.
extern HANDLE g_hModuleSem;


INT32 VDS_Init(VOID)
{
    INT32 iErrCode = ERR_SUCCESS;

    VDS_ModuleSemInit();
    iErrCode = vds_InitEntry();
    if(iErrCode != ERR_SUCCESS)
    {
        D( ( DL_VDSERROR, "Initiate VDS failed! errorcode = %d.", iErrCode));
        if(_ERR_VDS_FREE_LIST_NULL == iErrCode ||
                _ERR_VDS_SCAN_SECTOR_FAILED == iErrCode)
        {
            iErrCode = DRV_EraseAllUserDataSector();
            if(0 == iErrCode)
            {
                iErrCode = vds_InitEntry();
            }
        }
        else
        {
#ifndef _FS_SIMULATOR_
            DSM_ASSERT(0, "vds_InitEntry faile, err_code = %d.", iErrCode);
#endif
        }
        return iErrCode;
    }
    else
    {
        return ERR_SUCCESS;
    }
}


INT32 VDS_FormatPartition(PCSTR pszPartName)
{
    VDS_SECT_REF *psRef = NULL;
    VDS_SECT_REF *psTmpRef = NULL;
    VDS_PARTITION *psCP = NULL;
    UINT32 iPartCount;
    UINT32 iSecCount;
    UINT32 iSecAddr;
    UINT32 i;
    INT32 iResult;

    VDS_WaitForSemaphore();
    iPartCount = VDS_GetPartitionCount();
    // get partition description.
    for(i = 0; i < iPartCount; i++)
    {
        psCP = g_VDS_partition_table + i;
        if(0 == DSM_StrCmp(pszPartName, psCP->sPartInfo.acPartName))
        {
            break;
        }
    }

    // can't find the partition name in partition table.
    if(i == iPartCount)
    {
        VDS_ReleaseSemaphore();
        return ERR_VDS_PARA_ERROR;
    }

    // pop all the sector ref be part of this partition,add them to temp list.
    // dirty list.
    do
    {
        psRef = vds_PopFistSecRef(&(psCP->pDirtyList));
        if(psRef)
        {
            vds_AddSecRef(&psTmpRef, psRef);
        }
    }
    while(psRef);

    // free list.
    do
    {
        psRef = vds_PopFistSecRef(&(psCP->pFreeList));
        if(psRef)
        {
            vds_AddSecRef(&psTmpRef, psRef);
        }
    }
    while(psRef);


    // current ref.
    do
    {
        psRef = vds_PopFistSecRef(&(psCP->pCurrentSect));
        if(psRef)
        {
            vds_AddSecRef(&psTmpRef, psRef);
        }
    }
    while(psRef);

    // gcing ref.
    do
    {
        psRef = vds_PopFistSecRef(&(psCP->pGcingSect));
        if(psRef)
        {
            vds_AddSecRef(&psTmpRef, psRef);
        }
    }
    while(psRef);

    // temp ref.
    do
    {
        psRef = vds_PopFistSecRef(&(psCP->pTempSect));
        if(psRef)
        {
            vds_AddSecRef(&psTmpRef, psRef);
        }
    }
    while(psRef);

    // pop all the sector ref from tmp list.
    // erease and format it,
    // add it to the free list of this partition.
    // erase remanent ref and lost it.
    psRef = vds_PopFistSecRef(&psTmpRef);
    iSecCount = 0;
    psCP->iRefCount = 0;
    while(psRef)
    {
        iSecAddr = VDS_ADDR_SECT(psRef->iSN);
        if(iSecCount < psCP->sPartInfo.iFlashSectorCount)
        {
            iResult = vds_EraseAndFormatSec(iSecAddr, psCP->sPartInfo);
            if(ERR_SUCCESS != iResult)
            {
                return ERR_VDS_FORMAT_SEC_FAILED;
            }

            vds_AddSecRef(&(psCP->pFreeList), psRef);
            psCP->iRefCount ++;
        }
        else
        {
            iResult = vds_EraseSec(iSecAddr);
            if(ERR_SUCCESS != iResult)
            {
                VDS_ReleaseSemaphore();
                return ERR_VDS_FLASH_DRV_ERASE;
            }
        }
        iSecCount ++;
        psRef = vds_PopFistSecRef(&psTmpRef);
    }

    // chang a free setor to current sector.
    psRef = vds_PopFistSecRef(&(psCP->pFreeList));
    if(psRef)
    {
        psRef->iCurrentPBN = VDS_BLOCK_NONE;
        psRef->next = NULL;
        vds_AddSecRef(&(psCP->pCurrentSect), psRef);
        iResult = vds_SetSectStatus( psCP->pCurrentSect->iSN, VDS_STATUS_SECT_CURRENT);
        if(ERR_SUCCESS != iResult)
        {
            VDS_ReleaseSemaphore();
            return ERR_VDS_CHANGE_SECTOR_STATUS;
        }
    }
    else
    {
        VDS_ReleaseSemaphore();
        return ERR_VDS_FREE_LIST_NULL;
    }

    VDS_ReleaseSemaphore();
    return ERR_SUCCESS;
}


INT32 VDS_Open(PCSTR pszPartName, INT32 *pDevHandle )
{
    VDS_PARTITION *pPart;
    UINT32 i;

    if ( !pszPartName || !pDevHandle )
    {
        return ERR_VDS_PARA_ERROR;
    }

    for ( i = 0; i < VDS_GetPartitionCount(); i++ )
    {
        pPart = g_VDS_partition_table + i;
        if ( pPart->iRefCount &&
                0 == DSM_StrCmp( pszPartName, pPart->sPartInfo.acPartName ))
        {
            *pDevHandle = i;
            return ERR_SUCCESS;
        }
    }

    D( ( DL_VDSERROR, "VDS_Open: No Such partition(%s)", pszPartName ));
    return ERR_VDS_PARTITION_NOT_EXIST;
}


INT32 VDS_Close( PCSTR pszPartName )
{
    pszPartName = pszPartName;

    return ERR_SUCCESS;
}


INT32 VDS_WriteBlock( UINT32 iPartId, UINT32 iVBNum, UINT8 *pBuff )
{
    VDS_PARTITION *psCP;
    UINT32 uLen;
    INT32 lResult;
    UINT16 wOldSN;
    UINT16 wOldPBN;
    UINT16 wNewSN;
    UINT16 wNewPBN;
    UINT16 wStatus;
    VDS_PBD sPBD;

    DSM_MemSet ( &sPBD, 0xff, VDS_SZ_PBD );

    //check parameter.
    if ( iPartId >= VDS_GetPartitionCount() )
    {
        D( ( DL_VDSERROR, "In VDS_WriteBlock,PARA_ERR.DevHd = 0x%x\n", iPartId ) );
        return ERR_VDS_PARA_ERROR;
    }
    psCP = &g_VDS_partition_table[ iPartId ];
    if ( psCP->sPartInfo.iFlashSectorCount != psCP->iRefCount )
    {
        D( ( DL_VDSERROR, "In VDS_WriteBlock, partition[%d] not initialized.\n", iPartId ) );
        return ERR_VDS_UNINIT;
    }

    if ( iVBNum >= VDS_NR_MT_VALID_ENTRY( psCP )  )
    {
        D( ( DL_VDSERROR, "In VDS_WriteBlock, VBNum overflow(%d, %d)\n",
             iVBNum, VDS_NR_MT_VALID_ENTRY( psCP ) ) );
        return ERR_VDS_PARA_ERROR;
    }
    // CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("###########vbn = %d,max vbn = %d.",0x08000188),
    // iVBNum, VDS_NR_MT_VALID_ENTRY( psCP ));

    if ( NULL == pBuff )
    {
        return ERR_VDS_PARA_ERROR;
    }
    VDS_WaitForSemaphore();
    lResult = vds_GetCurrentPos( psCP, &wNewSN, &wNewPBN );
    if ( ERR_SUCCESS != lResult )
    {
        if ( _ERR_VDS_PARAM_ERROR == lResult )
        {
            D( ( DL_VDSERROR, "In VDS_WriteBlock,GetCurrentPos_ERR, ErrCode = %d.\n", lResult ) );
            VDS_ReleaseSemaphore();
            return ERR_VDS_FLASH_DRV_WRITE;
        }
        else
        {
            D( ( DL_WARNING, "In VDS_WriteBlock,GetCurrentPos_ERR2, ErrCode = %d.\n", lResult ) );
            VDS_ReleaseSemaphore();
            return ERR_VDS_SPACE_FULL;
        }
    }

    uLen = VDS_SZ_PB( psCP );
    wOldSN = psCP->psMT[ iVBNum ].iSN;
    wOldPBN = psCP->psMT[ iVBNum ].iPBN;

    //if the position has already been written data before,we should change the block_status there first.

    if ( ( VDS_SECT_NONE != wOldSN ) && ( VDS_BLOCK_NONE != wOldPBN ) )
    {
        lResult = vds_ReadPBD( psCP, wOldSN, wOldPBN, &sPBD );
        if ( ERR_SUCCESS != lResult )
        {
            D( ( DL_VDSERROR,
                 "In VDS_WriteBlock,ReadPBD_ERR1 ERR_CODE=%d, wOldSN=%d, wOldPBN=%d\n",
                 lResult, wOldSN, wOldPBN ) );
            D( ( DL_VDSERROR,
                 "PBCRC=0x%x, PbdCRC=0x%x, sPBD.iStatus=0x%x, sPBD.iVBN=0x%x, sPBD.iVersion=%d\n",
                 sPBD.iPBCRC, sPBD.iPBCRC, sPBD.iStatus, sPBD.iVBN, sPBD.iVersion ) );
            D( ( DL_VDSERROR, "vb number: %d\n", iVBNum ) );
            VDS_ReleaseSemaphore();
            return ERR_VDS_FLASH_DRV_READ;
        }
        sPBD.iVersion++;
    }
    else
    {
        sPBD.iVersion = 0;
    }
    sPBD.iStatus = VDS_STATUS_PB_VALID;
    sPBD.iVBN = (UINT16)iVBNum;
    sPBD.iPBDCRC = vds_GetPBDCRC( &sPBD );
    sPBD.iPBCRC = vds_CRC32( pBuff, uLen );
    lResult = vds_WritePBDAndPB( psCP, wNewSN, wNewPBN, &sPBD, pBuff );
    if ( ERR_SUCCESS != lResult )
    {
        D( ( DL_VDSERROR,
             "In VDS_WriteBlock,WriteBlock_ERR, ErrCode = %d,wNewSN = %d,wNewPBN=%d\n",
             lResult, wNewSN, wNewPBN ) );
        D( ( DL_VDSERROR,
             "sPBD.iPbCRC=%d, sPBD.iPbdCRC=%d, sPBD.iStatus=0x%x, sPBD.iVBN=0x%x, sPBD.iVersion=%d\n",
             sPBD.iPBCRC, sPBD.iPBDCRC, sPBD.iStatus, sPBD.iVBN, sPBD.iVersion ) );
        VDS_ReleaseSemaphore();
        return ERR_VDS_FLASH_DRV_WRITE;
    }
    if ( ( VDS_SECT_NONE == wOldSN ) || ( VDS_BLOCK_NONE == wOldPBN ) )
    {
        psCP->psMT[ iVBNum ].iSN = wNewSN;
        psCP->psMT[ iVBNum ].iPBN = wNewPBN;
    }
    else
    {
        // Whether or no the old pbd if write success,the bmp must change.
        psCP->psMT[ iVBNum ].iSN = wNewSN;
        psCP->psMT[ iVBNum ].iPBN = wNewPBN;

        wStatus = VDS_STATUS_PB_INVALID;
        lResult = vds_SetPBStatus( psCP, wOldSN, wOldPBN, wStatus );
        if ( lResult != ERR_SUCCESS )
        {
            D( ( DL_VDSERROR,
                 "In VDS_WriteBlock, SetPBStatus ERR, ErrCode = %d, sn =%d, pbn=%d\n",
                 lResult, wOldSN, wOldPBN ) );
            VDS_ReleaseSemaphore();
            return ERR_VDS_FLASH_DRV_WRITE;
        }
        else
        {
            // Do nothing.
        }
    }
    VDS_ReleaseSemaphore();
    return ERR_SUCCESS;
}


INT32 VDS_ReadBlock( UINT32 iPartId, UINT32 iVBNum, UINT8* pBuff )
{
    VDS_PARTITION *psCP;
    UINT32 uLen;
    INT32 lResult;
    UINT16 wSN;
    UINT16 wPBN;
    BOOL bCRCResult;
    VDS_PBD sPBD;
    UINT16 iVBN;
    lResult = 0;
    DSM_MemSet( &sPBD, 0xff, VDS_SZ_PBD );

    //check parameter.
    if ( iPartId >= VDS_GetPartitionCount() )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock,PARA_ERR.DevHd = 0x%x.", iPartId ) );
        DSM_ASSERT(0, "In VDS_ReadBlock,PARA_ERR.DevHd = 0x%x.", iPartId);
        return ERR_VDS_PARA_ERROR;
    }

    psCP = &g_VDS_partition_table[ iPartId ];
    if ( psCP->sPartInfo.iFlashSectorCount != psCP->iRefCount )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock, partition[%d] not initialized.", iPartId ) );
        DSM_ASSERT(0, "In VDS_ReadBlock, partition[%d] not initialized.", iPartId);
        return ERR_VDS_UNINIT;
    }

    VDS_WaitForSemaphore();
    iVBN = (UINT16)iVBNum;
    if ( ( iVBN < 0 ) || ( iVBN >= VDS_NR_MT_VALID_ENTRY( psCP ) ) )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock,PARA_ERR.VBNum = 0x%x.", iVBN ) );
  	 DSM_ASSERT(0,"In VDS_ReadBlock,PARA_ERR.VBNum = 0x%x.", iVBN);
        VDS_ReleaseSemaphore();
        return ERR_VDS_PARA_ERROR;
    }
    if ( NULL == pBuff )
    {
        DSM_ASSERT(0, "In VDS_ReadBlock,pBuff = NULL.");
        VDS_ReleaseSemaphore();
        return ERR_VDS_PARA_ERROR;
    }

    uLen = VDS_SZ_PB( psCP );
    wSN = psCP->psMT[ iVBN ].iSN;
    wPBN = psCP->psMT[ iVBN ].iPBN;
    if ( ( VDS_SECT_NONE == wSN ) || ( VDS_BLOCK_NONE == wPBN ) )
    {
        D( ( DL_DETAIL, "In VDS_ReadBlock,wSN==VDS_SECT_NONE ||wPBN == VDS_BLOCK_NONE, return from here\n" ) );
        DSM_MemSet ( pBuff, 0x0, uLen );
        VDS_ReleaseSemaphore();
        return ERR_SUCCESS;
    }
    lResult = vds_ReadPBD( psCP, wSN, wPBN, &sPBD );
    if ( lResult != ERR_SUCCESS )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock,ReadPBDERR.ErrCode = %d,SN=%d, PBN=%d\n", lResult, wSN, wPBN ) );
  	 DSM_ASSERT(0,"In VDS_ReadBlock,ReadPBDERR.ErrCode = %d,SN=%d, PBN=%d\n", lResult, wSN, wPBN ) ;
        VDS_ReleaseSemaphore();
        return ERR_VDS_FLASH_DRV_READ;
    }
    if ( VDS_STATUS_PB_VALID != sPBD.iStatus )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock,PBD_STATUS_ERR. sPBD.iStatus = 0x%x.(iPartId = %d,iVBNum = %d,wSN = %d, wPBN = %d.)\n", sPBD.iStatus,iPartId,iVBN, wSN, wPBN) );
        DSM_ASSERT(0,"In VDS_ReadBlock,PBD_STATUS_ERR. sPBD.iStatus = 0x%x.(iPartId = %d,iVBNum = %d,wSN = %d, wPBN = %d.)\n", sPBD.iStatus,iPartId,iVBN, wSN, wPBN);
        VDS_ReleaseSemaphore();
        return ERR_VDS_STATUS_INVALID;
    }
    lResult = vds_ReadPB( psCP, wSN, wPBN, pBuff );
    if ( ERR_SUCCESS != lResult )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock,ReadPB_ERR. ErrCode = %d,wSN=%d, wPBN=%d\n", lResult, wSN, wPBN ) );
 	 DSM_ASSERT(0,"In VDS_ReadBlock,ReadPB_ERR. ErrCode = %d,wSN=%d, wPBN=%d\n", lResult, wSN, wPBN );
        VDS_ReleaseSemaphore();
        return ERR_VDS_FLASH_DRV_READ;
    }

    /* pb crc check */
    bCRCResult = vds_IsCRCChecked( pBuff, uLen, sPBD.iPBCRC, psCP->sPartInfo.eCheckLevel );
    if ( !bCRCResult )
    {
        D( ( DL_VDSERROR, "In VDS_ReadBlock,PB CRC check failed. SN = %d, PBN = %d\n", wSN, wPBN ) );
        DSM_ASSERT(0, "In VDS_ReadBlock,PB CRC check failed. SN = %d, PBN = %d\n", wSN, wPBN );
        VDS_ReleaseSemaphore();
        return ERR_VDS_CRC_ERROR;
    }

    VDS_ReleaseSemaphore();
    return ERR_SUCCESS;
}

INT32 VDS_RevertBlock( UINT32 iPartId, UINT32 iVBNum)
{
    VDS_PARTITION *psCP;
    INT32 lResult;
    UINT16 wSN;
    UINT16 wPBN;
    VDS_PBD sPBD = {VDS_STATUS_PB_INVALID, 0, 0, 0, 0};
    UINT16 iVBN;

    DSM_MemSet ( &sPBD, 0x0, VDS_SZ_PBD );

    //check parameter.
    if ( iPartId >= VDS_GetPartitionCount() )
    {
        D( ( DL_VDSERROR, "In VDS_RevertBlock,PARA_ERR.DevHd = 0x%x\n", iPartId ) );
        return ERR_VDS_PARA_ERROR;
    }
    psCP = &g_VDS_partition_table[ iPartId ];
    if ( psCP->sPartInfo.iFlashSectorCount != psCP->iRefCount )
    {
        D( ( DL_VDSERROR, "In VDS_RevertBlock, partition[%d] not initialized.\n", iPartId ) );
        return ERR_VDS_UNINIT;
    }

    iVBN = (UINT16)iVBNum;
    if ( iVBN >= VDS_NR_MT_VALID_ENTRY( psCP )  )
    {
        D( ( DL_VDSERROR, "In VDS_RevertBlock, VBNum overflow(%d, %d)\n", iVBN, VDS_NR_MT_VALID_ENTRY( psCP ) ) );
        return ERR_VDS_PARA_ERROR;
    }

    VDS_WaitForSemaphore();

    // Get the SN and PBN from MT.
    wSN = psCP->psMT[ iVBN ].iSN;
    wPBN = psCP->psMT[ iVBN ].iPBN;

    // Write PBD and change MT.
    if ( ( VDS_SECT_NONE != wSN ) && ( VDS_BLOCK_NONE != wPBN ) )
    {
        // Write the PBD status to invalid.
        lResult = vds_WritePBD( psCP, wSN, wPBN, &sPBD );
        if ( ERR_SUCCESS != lResult )
        {
            D( ( DL_VDSERROR, "In VDS_RevertBlock,vds_WritePBD ERR_CODE=%d, wOldSN=%d, wOldPBN=%d\n", lResult, wSN, wPBN ) );
            D( ( DL_VDSERROR, "vb number: %d\n", iVBN ) );
            VDS_ReleaseSemaphore();
            return ERR_VDS_FLASH_DRV_WRITE;
        }
        // Change the MT.
        psCP->psMT[ iVBN ].iSN = VDS_SECT_NONE;
        psCP->psMT[ iVBN ].iPBN = VDS_BLOCK_NONE;
    }

    VDS_ReleaseSemaphore();
    return ERR_SUCCESS;
}

INT32 VDS_GetPartitionInfo( UINT32 iPartId, UINT32 *piNrBlock, UINT32 *piBlockSize )
{
    VDS_PARTITION *psPart;
    UINT16 iVBN;

    if ( iPartId >= VDS_GetPartitionCount() || !piNrBlock || !piBlockSize )
    {
        return _ERR_VDS_PARAM_ERROR;
    }

    psPart = &g_VDS_partition_table[ iPartId ];

    if ( psPart->sPartInfo.iFlashSectorCount != psPart->iRefCount )
    {
        return ERR_VDS_UNINIT;
    }

    iVBN = (UINT16)(VDS_NR_MT_VALID_ENTRY( psPart )*(psPart->sPartInfo.iVBSize/VDS_BLOCK_SIZE));
    *piNrBlock = (UINT32)iVBN;
    *piBlockSize = VDS_BLOCK_SIZE;

    D((DL_WARNING, "iPartID = %d, piNrBlock = 0x%x, piBlockSize = 0x%x", iPartId, *piNrBlock, *piBlockSize));

    return ERR_SUCCESS;
}


UINT32 VDS_GetPartitionCount(VOID)
{
    return g_vds_partition_count;
}


/**************************************************************************************/
// Function: This function get the  version of VDS module.
// Parameter:
// None.
// Return value: return the version of VDS module..
/***************************************************************************************/
UINT32 VDS_GetVersion(VOID)
{
    return VDS_LAYOUT_VERSION;
}


VOID  VDS_Deactive(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return;
}


INT32 VDS_Active(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}


UINT32 VDS_GetRCacheSize(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}


UINT32 VDS_GetWCacheSize(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}


VOID VDS_SetWCacheSize(HANDLE dev_handle, UINT32 cache_size)
{
    dev_handle = dev_handle;
    cache_size = cache_size;
    return;
}


VOID VDS_SetRCacheSize(HANDLE dev_handle, UINT32 cache_size)
{
    dev_handle = dev_handle;
    cache_size = cache_size;
    return;
}


INT32 VDS_Flush(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}

INT32 VDS_SetAllSectorsEraseFlag(VOID)
{
    INT32 iResult;

    iResult = vds_SetAllSectorsEraseFlag();
    if(0 == iResult)
    {
        return ERR_SUCCESS;
    }
    else
    {
        return ERR_VDS_FLASH_DRV_WRITE;
    }
}


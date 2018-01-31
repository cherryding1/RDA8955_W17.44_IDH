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
#include "sxs_io.h"
#include <drv_flash.h>
#endif

#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "vds_cd.h"
#include "vds_dbg.h"
#include "vds_local.h"
#include "vds_tools.h"

//Global variable

// partition table.
VDS_PARTITION g_VDS_partition_table[ VDS_NR_PART_MAX ] = { {0,}, };

// partition count.
UINT32 g_vds_partition_count = 0;

// sector ref chain.
VDS_SECT_REF *g_psSectRef = NULL;

// remain sector chain.
VDS_SECT_REF *g_psRemainRef = NULL;

// flash information.
VDS_FLASH_INFO g_sFlashInfo = { FALSE, 0, 0, 0 };

UINT32 g_tstVdsErrCode = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

// vds cache flush alloc sign.
UINT32 g_iFlushAllowed = 0;

// vds cache flush initialized sign.
UINT32 g_iVdsCacheHasInited = 0;
UINT32 g_iFlushDisableKeepTime = 0;
UINT32 g_iFlushDisableBeginTime = 0;
#else

// vds semaphore handle.
HANDLE g_hModuleSem = (HANDLE)NULL;

#endif


INT32 vds_SetSectStatus(UINT16 iSN, UINT32 iStatus )
{
    UINT32 iSectAddr = VDS_ADDR_SECT(iSN );
    UINT32 iWriteSize;
    UINT32 iReadSize;
    INT32 iRet;
    UINT32 status;


    iRet = DRV_WriteFlash( iSectAddr + VDS_POS_SH_STATUS,
                           ( void * ) & iStatus, VDS_SZ_SH_STATUS, &iWriteSize );
    if ( ( iRet != ERR_SUCCESS ) || ( VDS_SZ_SH_STATUS != iWriteSize ) )
    {
        D( ( DL_ERROR, "vds_SetSectStatus:call DRV_WriteFlash() failed,iRet = %d,write size = %d, written size = %d. \n", iRet, VDS_SZ_SH_STATUS, iWriteSize) );
        DSM_ASSERT(0, "vds_SetSectStatus:DRV_ReadFlash fail.err_code = %d", iRet);
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }


    iReadSize = 0;
    iRet = DRV_ReadFlash( iSectAddr + VDS_POS_SH_STATUS,
                          ( void * ) & status, VDS_SZ_SH_STATUS, &iReadSize );
    if ( ( iRet != ERR_SUCCESS ) || ( iReadSize != VDS_SZ_SH_STATUS ) )
    {
        D((BASE_FFS_TS_ID, "vds_SetSectStatus: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
           iSectAddr + VDS_POS_SH_STATUS,
           iRet,
           VDS_SZ_SH_STATUS,
           iReadSize ));
        DSM_ASSERT(0, "vds_SetSectStatus:DRV_ReadFlash fail.err_code = %d", iRet);
        return _ERR_VDS_READ_FLASH_FAILED;
    }


    if ( status != iStatus )
    {
        D( ( DL_VDSERROR, "In vds_SetSectStatus, _ERR_VDS_WRITE_CHECK_FAILED!\n" ) );
        DSM_ASSERT(0, "vds_SetSectStatus:writing check error.iStatus = 0x%x,status = 0x%x,", iStatus, status);
        return _ERR_VDS_WRITE_CHECK_ERROR;
    }

    return ERR_SUCCESS;

}



INT32 vds_CountBlock( VDS_PARTITION *psPart, VDS_SECT_REF *psRef, UINT16 iBegIndex, UINT16 iStatus )
{
    INT32 iCounter = 0;
    INT32 iReadCount = 0;
    INT32 iCheckCount = 0;
    UINT16 i;
    VDS_PBD saPbdt[ VDS_NR_PBD_PER_GROUP ];
    INT32 iNrPbPerSect;
    INT32 iResult;


    iNrPbPerSect = VDS_NR_PB_PER_SECT( psPart );
    if ( NULL == psPart || NULL == psRef )
    {
        return _ERR_VDS_PARAM_ERROR;
    }

    for ( i = iBegIndex; i < iNrPbPerSect; i++ )
    {
        if (0 == iReadCount)
        {
            if(iNrPbPerSect  >  i + VDS_NR_PBD_PER_GROUP)
            {
                iReadCount = VDS_NR_PBD_PER_GROUP;
            }
            else
            {
                iReadCount = iNrPbPerSect - i;
            }
            iResult = vds_ReadPBDT( psPart, psRef ->iSN, i, iReadCount, saPbdt );
            if ( iResult < 0 )
            {
                return _ERR_VDS_READ_FLASH_FAILED;
            }
            iCheckCount = 0;
        }

        if ( iStatus == saPbdt[ iCheckCount ].iStatus )
        {
            iCounter++;
        }
        else
        {
        }

        iCheckCount ++;
        iReadCount--;
    }

    return iCounter;

}



INT32 vds_GetFirstBlockPos( VDS_PARTITION *psPart, VDS_SECT_REF *psRef, UINT16 iBlockStatus, INT16 *pPBN )
{
    INT16 i;
    INT32 iNrPbPerSec = VDS_NR_PB_PER_SECT( psPart );
    VDS_PBD sPbd;
    INT32 iRet;

    for ( i = 0; i < iNrPbPerSec; i++ )
    {
        iRet = vds_ReadPBD( psPart, psRef->iSN, i, &sPbd );
        if ( ERR_SUCCESS != iRet )
        {
            return _ERR_VDS_READ_FLASH_FAILED;
        }

        if ( iBlockStatus == sPbd.iStatus )
        {
            *pPBN = i;
            return ERR_SUCCESS;
        }
    }

    return _ERR_VDS_NO_MORE_PBD;

}


INT32 vds_SetPBStatus( VDS_PARTITION *psPart, UINT16 iSN, UINT16 iPBN, UINT16 iStatus )
{
    UINT32 iPbdtAddr;
    UINT32 iPbdOffset;
    UINT32 iWriteSize = 0;
    UINT32 iReadSize = 0;
    INT32 iRet;
    UINT16 iReadStatus;
    VOID *pStatus;
    UINT16 iOriginalStatus;

    iPbdtAddr = VDS_ADDR_PBDT( psPart, iSN );
    iPbdOffset = VDS_SZ_PBD * iPBN;
    if ( NULL == psPart
            || FALSE == vds_IsValidSn(psPart, iSN)
            || iPBN >= VDS_NR_PB_PER_SECT( psPart ) )
    {
        D( ( DL_VDSERROR, "vds_SetPBStatus: iSN = %d, iPBN = %d", iSN, iPBN ) );
        return _ERR_VDS_PARAM_ERROR;
    }

    pStatus = &iOriginalStatus;
    iRet = DRV_ReadFlash( iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
                          pStatus, VDS_SZ_PBD_STATUS, &iReadSize );
    if ( iRet != ERR_SUCCESS || iReadSize != VDS_SZ_PBD_STATUS )
    {
        D((BASE_FFS_TS_ID, "vds_SetPBStatus: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
           iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
           iRet,
           VDS_SZ_PBD_STATUS,
           iReadSize ));
        return _ERR_VDS_READ_FLASH_FAILED;
    }
    D( ( DL_BRIEF, "vds_SetPBStatus: DRV_ReadFlash with addr = 0x%x,iOriginalStatus = 0x%x,VDS_SZ_PBD_STATUS = 0x%x,iReadSize = 0x%x",
         iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
         iOriginalStatus,
         VDS_SZ_PBD_STATUS,
         iReadSize) );

    pStatus = &iStatus;
    iRet = DRV_WriteFlash( iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
                           pStatus, VDS_SZ_PBD_STATUS, &iWriteSize );
    if ( ( iRet != ERR_SUCCESS ) || ( iWriteSize != VDS_SZ_PBD_STATUS ) )
    {
        D( ( DL_VDSERROR, "vds_SetPBStatus: Call  DRV_WriteFlash failed,iRet = %d,write size = %d,written size = %d.", iRet, VDS_SZ_PBD_STATUS, iWriteSize) );
        D( ( DL_VDSERROR, "vds_SetPBStatus: Write status failed: original status 0x%x, status to set: 0x%x.", iOriginalStatus, iStatus ) );
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }
    D( ( DL_BRIEF, "vds_SetPBStatus: DRV_WriteFlash with addr = 0x%x,iStatus = 0x%x,VDS_SZ_PBD_STATUS = 0x%x,iWriteSize = 0x%x",
         iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
         iStatus,
         VDS_SZ_PBD_STATUS,
         iWriteSize ) );

    pStatus = &iReadStatus;
    iReadSize = 0;
    iRet = DRV_ReadFlash( iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
                          pStatus, VDS_SZ_PBD_STATUS, &iReadSize );
    if ( iRet != ERR_SUCCESS || iReadSize != VDS_SZ_PBD_STATUS )
    {
        D( ( DL_VDSERROR, "vds_SetPBStatus: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
             iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS,
             iRet,
             VDS_SZ_PBD,
             iReadSize ));
        return _ERR_VDS_READ_FLASH_FAILED;
    }

    if ( iReadStatus != iStatus )
    {
        D( ( DL_VDSERROR, "vds_SetPBStatus: write check failed: original status 0x%x, status to set: 0x%x, status readback: 0x%x,addr = 0x%x.",
             iOriginalStatus, iStatus, iReadStatus , iPbdtAddr + iPbdOffset + VDS_POS_PBD_STATUS) );
        return _ERR_VDS_WRITE_CHECK_ERROR;
    }

    return ERR_SUCCESS;
}


///////////////////////////////////
// vds_SetMT
// Update the entry in map table according the information of the pbd.
/////////////////////////////////////////
INT32 vds_SetMT( VDS_PARTITION *psCP,
                 VDS_PBD *pPBD, UINT16 iSectNo, UINT16 iPBN)
{
    UINT16 iVbn;
    UINT32 iVersion;
    UINT16 iOldSn;
    UINT16 iOldPbn;
    VDS_PBD sOldPbd;
    VDS_SH sSh;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;

    if ( pPBD->iVBN >= VDS_NR_MT_VALID_ENTRY( psCP ) )
    {
        D( ( DL_VDSERROR, "In vds_SetMT, vbn overflow( %d, %d ).\n", pPBD->iVBN, VDS_NR_MT_VALID_ENTRY( psCP )) );
        return ERR_VDS_PARA_ERROR;
    }



    iVbn = pPBD->iVBN;
    iVersion = pPBD->iVersion;
    iOldSn = psCP->psMT[ iVbn ].iSN;
    iOldPbn = psCP->psMT[ iVbn ].iPBN;

    if(iOldSn == iSectNo && iOldPbn == iPBN)
    {
        return ERR_SUCCESS;
    }

    if (iOldSn == VDS_MT_SN_FREE || iOldPbn == VDS_MT_PBN_FREE )
    {
        psCP->psMT[ iVbn ].iSN = iSectNo;
        psCP->psMT[ iVbn ].iPBN = iPBN;
    }
    else if((TRUE == vds_IsValidSn(psCP, iOldSn)) &&
            (iPBN < ( UINT16 ) VDS_NR_PB_PER_SECT( psCP ) ))
    {
        iResult = vds_ReadSH(psCP, iOldSn, &sSh);
        if(ERR_SUCCESS != iResult )
        {
            psCP->psMT[ iVbn ].iSN = iSectNo;
            psCP->psMT[ iVbn ].iPBN = iPBN;
        }
        else
        {
            if(VDS_STATUS_SECT_DIRTY != sSh.iSectorStatus &&
                    VDS_STATUS_SECT_CURRENT != sSh.iSectorStatus)
            {
                psCP->psMT[ iVbn ].iSN = iSectNo;
                psCP->psMT[ iVbn ].iPBN = iPBN;
            }
            else
            {
                iResult = vds_ReadPBD( psCP, iOldSn, iOldPbn, &sOldPbd );
                if ( ERR_SUCCESS == iResult )
                {
                    if ( sOldPbd.iVBN != iVbn )
                    {
                        psCP->psMT[ iVbn ].iSN = iSectNo;
                        psCP->psMT[ iVbn ].iPBN = iPBN;
                    }

                    if ( iVersion >= sOldPbd.iVersion )
                    {
                        psCP->psMT[ iVbn ].iSN = iSectNo;
                        psCP->psMT[ iVbn ].iPBN = iPBN;
                        iResult = vds_SetPBStatus( psCP, iOldSn, iOldPbn, VDS_STATUS_PB_INVALID );
                    }
                    else
                    {
                        iResult = vds_SetPBStatus( psCP, iSectNo, iPBN, VDS_STATUS_PB_INVALID );
                    }
                    iRet = iResult;

                }
                else
                {
                    D( ( DL_VDSERROR, "In vds_SetMT, vds_ReadPBD return failed, ErrCode = %d.\n", iRet ) );
                    iRet = iRet;
                }
            }
        }
        /* TODO..
        if ( sOldPbd.iPBCRC != vds_GetPBDCRC( &sOldPbd ) )
        {
          return ERR_VDS_UNKNOWN;
        }
        */
    }
    else
    {
        psCP->psMT[ iVbn ].iSN = iSectNo;
        psCP->psMT[ iVbn ].iPBN = iPBN;
    }

    return iRet;

}



///////////////////////////////////
// Update map table by scanning sector. Scanning countinues until FIRST free block
// is found or all blocks have been found.
//
// Return value:
// IF successful, ERR_SUCCESS is returned, and *iNrBlkScaaned is set to the number of
// the blocks which are before the FIRST free block, ( especially, if all the blocks are
// not FREE, the number of total blocks in sector is set); else error code is returned.
/////////////////////////////////////////

INT32 vds_SetMTByScanSec( VDS_PARTITION *psCP, UINT16 iSectNo, UINT16 *iNrBlkScaaned)
{
    /* Index of the block in group */
    INT32 iIndexIngrp = VDS_NR_PBD_PER_GROUP;
    UINT16 iPbn = 0;
    VDS_PBD saPbdt[ VDS_NR_PBD_PER_GROUP ];
    INT32 iRet;

    while ( iPbn < VDS_NR_PB_PER_SECT( psCP ) )
    {
        if ( VDS_NR_PBD_PER_GROUP <= iIndexIngrp )
        {
            iRet = vds_ReadPBDT( psCP, iSectNo, iPbn, VDS_NR_PBD_PER_GROUP, saPbdt );
            if ( iRet < 0 )
            {
                D( ( DL_VDSERROR, "In vds_SetMTByScanSec, vds_ReadPBDT return failed, ErrCode = %d\n", iRet ) );
                return iRet;
            }
            iIndexIngrp = 0;
        }

        if ( VDS_STATUS_PB_VALID == saPbdt[ iIndexIngrp ].iStatus )
        {
            if(saPbdt[ iIndexIngrp ].iVBN >= VDS_NR_MT_VALID_ENTRY( psCP ))
            {
                D( ( DL_WARNING, "In vds_SetMTByScanSec, saPbdt[ iIndexIngrp ].iVBN Is invalid , saPbdt[ iIndexIngrp ].iVBN = %d\n", saPbdt[ iIndexIngrp ].iVBN ) );
                iRet = vds_SetPBStatus( psCP, iSectNo, iPbn, VDS_STATUS_PB_INVALID );
                if ( iRet < 0 )
                {
                    D( ( DL_VDSERROR, "In vds_SetMTByScanSec, vds_ReadPBDT return failed, ErrCode = %d\n", iRet ) );
                    return iRet;
                }
            }
            else
            {
                iRet = vds_SetMT( psCP, &saPbdt[ iIndexIngrp ], iSectNo, iPbn );
                if ( ERR_SUCCESS != iRet )
                {
                    D( ( DL_VDSERROR, "In vds_SetMTByScanSec, vds_SetMT return failed, ErrCode = %d\n", iRet ) );
                    return iRet;
                }
            }
        }
        else if ( VDS_STATUS_PB_FREE == saPbdt[ iIndexIngrp ].iStatus )
        {
            *iNrBlkScaaned = iPbn;
            return ERR_SUCCESS;
        }
        else
        {
            // do noting.
        }

        ++iIndexIngrp;
        ++iPbn;
    }

    *iNrBlkScaaned = iPbn;
    return ERR_SUCCESS;
}



INT32 vds_MoveCurrentPos( VDS_PARTITION *psVDSPart )
{
    INT32 lResult;
    INT32 lErrCode;
    INT32 lNrPbPerSec;
    UINT16 wSN;

    wSN = 0;
    lResult = 0;
    lErrCode = ERR_SUCCESS;
    lNrPbPerSec = VDS_NR_PB_PER_SECT( psVDSPart );

    if ( NULL == psVDSPart )
    {
        return _ERR_VDS_PARAM_ERROR;
    }

    // If existed current sector and the sector existed free block,move the pos to next.
    if ( NULL != psVDSPart->pCurrentSect && psVDSPart->pCurrentSect->iCurrentPBN != ( UINT16 ) ( lNrPbPerSec - 1 ) )
    {
        psVDSPart->pCurrentSect->iCurrentPBN ++;
        return ERR_SUCCESS;
    }

    // If the curent sector no more free block, change it to dirty sector.
    if ( NULL != psVDSPart->pCurrentSect && psVDSPart->pCurrentSect->iCurrentPBN == ( UINT16 ) ( lNrPbPerSec - 1 ) )
    {
        wSN = psVDSPart->pCurrentSect->iSN;
        lErrCode = vds_SetSectStatus( wSN, VDS_STATUS_SECT_DIRTY );
        if ( ERR_SUCCESS != lErrCode )
        {
            D( ( DL_VDSERROR, "In MoveCurrentPos, SetSectStatus1 ERR,ErrCode = %d,wSN = %d\n", lErrCode, wSN) );
            return _ERR_VDS_PARAM_ERROR;
        }
        vds_AppendSecRef( &( psVDSPart->pDirtyList ), psVDSPart->pCurrentSect );
        psVDSPart->pCurrentSect = NULL;
        if ( NULL != psVDSPart->pFreeList )
        {
            lErrCode = vds_Pop( psVDSPart, &(psVDSPart->pFreeList) );
            if ( ERR_SUCCESS != lErrCode )
            {
                return _ERR_VDS_PARAM_ERROR;
            }
            return ERR_SUCCESS;
        }
    }
    // To GC.
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    if(vds_IsCacheInited())
    {
        if( NULL == psVDSPart->pCurrentSect )
        {
            D( ( DL_DETAIL, "Active gc...\n" ) );
            vds_GcActive( psVDSPart );

            return _ERR_VDS_PARTITION_FULL;
        }
    }
    else
    {
        while ( NULL == psVDSPart->pCurrentSect )
        {
            D( ( DL_DETAIL, "BEGIN TO GC...\n" ) );
            lResult = vds_Gc( psVDSPart );
            if ( ERR_SUCCESS != lResult )
            {
                D( ( DL_VDSERROR, "In MoveCurrentPos, vds_Gc ERR,ErrCode = %d.\n", lResult) );
                return ( _ERR_VDS_DIRTY_LIST_NULL == lResult ) ? _ERR_VDS_PARTITION_FULL : _ERR_VDS_GC_FAILED;
            }
            if(psVDSPart->pCurrentSect)
            {
                psVDSPart->pCurrentSect->iCurrentPBN ++;
            }
        }
    }
#else
    while ( NULL == psVDSPart->pCurrentSect )
    {
        D( ( DL_DETAIL, "BEGIN TO GC...\n" ) );
        lResult = vds_Gc( psVDSPart );
        if ( ERR_SUCCESS != lResult )
        {
            D( ( DL_VDSERROR, "In MoveCurrentPos, vds_Gc ERR,ErrCode = %d.\n", lResult) );
            return ( _ERR_VDS_DIRTY_LIST_NULL == lResult ) ? _ERR_VDS_PARTITION_FULL : _ERR_VDS_GC_FAILED;
        }
        if(psVDSPart->pCurrentSect)
        {
            psVDSPart->pCurrentSect->iCurrentPBN ++;
        }
    }
#endif

    return ERR_SUCCESS;
}


INT32 vds_GetCurrentPos( VDS_PARTITION *psCP, UINT16 *pSN, UINT16 *pPBN )
{
    INT32 iErrCode = 0;
    UINT16 iSN = 0;
    UINT16 iPBN = 0;

    if ( ( NULL == psCP ) || ( NULL == pSN ) || ( NULL == pPBN ) )
    {
        return _ERR_VDS_PARAM_ERROR;
    }

    iErrCode = vds_MoveCurrentPos( psCP );
    if(ERR_SUCCESS != iErrCode)
    {
        return iErrCode;
    }
    else
    {
        iSN = ( psCP->pCurrentSect ) ->iSN;
        iPBN = ( psCP->pCurrentSect ) ->iCurrentPBN;
        *pSN = iSN;
        *pPBN = iPBN;
        return ERR_SUCCESS;
    }
}



INT32 vds_Pop( VDS_PARTITION *psVDSPart, VDS_SECT_REF **list )
{
    UINT16 wSN;
    VDS_SH sSH;
    INT32 lErrCode;

    psVDSPart->pCurrentSect = vds_GetFirstSecRef( *list );
    wSN = psVDSPart->pCurrentSect->iSN;

    lErrCode = vds_ReadSHWithCrc( psVDSPart, wSN, &sSH );
    if ( ERR_SUCCESS != lErrCode )
    {
        D( ( DL_VDSERROR, "In vds_GetPos,ReadSH erro, ErrCode = %d,", lErrCode ) );
        D( ( DL_VDSERROR, "sSh.iInvalidBlockCount=%d,sSh. iMagic=0x%x, sSh.iSectorStatus=0x%x\n", sSH.iEraseCounter, sSH.iMagic, sSH.iSectorStatus ) );
        return _ERR_VDS_READ_SH_FAILED ;
    }
    sSH.iSectorStatus = VDS_STATUS_SECT_CURRENT;

    /*CRC will NOT change!*/

    lErrCode = vds_WriteSH( psVDSPart, wSN, &sSH );
    if ( ERR_SUCCESS != lErrCode )
    {
        D( ( DL_VDSERROR, "In vds_GetPos,WriteSH erro, ErrCode = %d,", lErrCode ) );
        D( ( DL_VDSERROR, "sSh.iInvalidBlockCount=%d,sSh. iMagic=0x%x, sSh.iSectorStatus=0x%x\n", sSH.iEraseCounter, sSH.iMagic, sSH.iSectorStatus ) );
        return _ERR_VDS_WRITE_SH_FAILED;
    }
    vds_RemoveSecRef( list, psVDSPart->pCurrentSect );
    ( psVDSPart->pCurrentSect->iCurrentPBN ) ++;
    return ERR_SUCCESS;
}


INT32 vds_ReadPBD( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, VDS_PBD *pPBD )
{
    // INT32 lSecTotalNum = VDS_NR_SECT( psCP );
    INT32 lPbdSize = VDS_SZ_PBD;
    UINT32 lAddr = 0;
    INT32 lRet = ERR_SUCCESS;
    UINT32 lRSize = 0;


    if ((TRUE == vds_IsValidSn(psCP, iSN)) &&
            (iPBN < ( UINT16 ) VDS_NR_PB_PER_SECT( psCP ) ) &&
            (NULL != pPBD))
    {
        lAddr = VDS_ADDR_SECT( iSN ) + lPbdSize * iPBN + VDS_SZ_SH;
        lRet = DRV_ReadFlash( lAddr, ( UINT8 * ) pPBD, VDS_SZ_PBD, (UINT32 *)&lRSize );
        if ( ( ERR_SUCCESS == lRet ) && ( lRSize == VDS_SZ_PBD ) )
        {
            lRet = ERR_SUCCESS;
        }
        else
        {
            D( ( DL_VDSERROR, "vds_ReadPBD: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
                 lAddr,
                 lRet,
                 VDS_SZ_PBD,
                 lRSize ));
            lRet = _ERR_VDS_READ_FLASH_FAILED;
        }

    }
    else
    {
        D( ( DL_VDSERROR, "In vds_ReadPBD,_ERR_VDS_PARAM_ERROR" ) );
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    return lRet;

}


INT32 vds_WritePBD( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, VDS_PBD *pPBD )
{
    // INT32 lSecTotalNum = VDS_NR_SECT( psCP );
    INT32 lPbdSize = VDS_SZ_PBD;
    INT32 lAddr = 0;
    INT32 lRet = ERR_SUCCESS;
    UINT32 lWSize = 0;

    if (
        ( TRUE == vds_IsValidSn(psCP, iSN) ) &&
        ( iPBN < ( UINT16 ) VDS_NR_PB_PER_SECT( psCP ) ) &&
        ( NULL != pPBD ))
    {
        lAddr = VDS_ADDR_SECT(iSN ) + lPbdSize * iPBN + VDS_SZ_SH;
        lRet = DRV_WriteFlash( lAddr, ( UINT8 * ) pPBD, VDS_SZ_PBD, (UINT32 *)&lWSize );
        if ( ( ERR_SUCCESS == lRet ) && ( lWSize == VDS_SZ_PBD ) )
        {
            lRet = ERR_SUCCESS;
        }
        else
        {
            D( ( DL_VDSERROR, "vds_WritePBD: Call  DRV_WriteFlash failed,iRet = %d,write size = %d,written size = %d.", lRet, VDS_SZ_PBD, lWSize) );
            lRet = _ERR_VDS_WRITE_FLASH_FAILED;
        }

    }
    else
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }

    return lRet;
}


INT32 vds_ReadPB( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, UINT8 *pBuff )
{
    UINT32 uLen;
    UINT32 uReadLen;
    UINT32 uAddr;
    INT32 lResult;
    INT32 lRet;

    if ( NULL == psCP )
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else if ( NULL == pBuff )
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else if (  iPBN >= VDS_NR_PB_PER_SECT( psCP )  )
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else if ( FALSE == vds_IsValidSn(psCP, iSN))
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else
    {
        uLen = VDS_SZ_PB( psCP );
        uReadLen = 0;
        uAddr = VDS_ADDR_PB( psCP, iSN ) + uLen * iPBN;
        lResult = DRV_ReadFlash( uAddr, pBuff, uLen, &uReadLen );
        if ( ( lResult == ERR_SUCCESS ) && ( uReadLen == uLen ) )
        {
            lRet = ERR_SUCCESS;
        }
        else
        {
            D( ( DL_VDSERROR, "vds_ReadPB: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
                 uAddr,
                 lResult,
                 uLen,
                 uReadLen ));
            lRet = _ERR_VDS_READ_FLASH_FAILED;
        }
    }
    return lRet;

}


INT32 vds_WritePB( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, UINT8 *pBuff )
{
    UINT32 uLen;
    UINT32 uWriteLen;
    UINT32 uAddr;
    INT32 lResult;
    INT32 lRet;

    if ( NULL == psCP )
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else if ( NULL == pBuff )
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else if (iPBN >= VDS_NR_PB_PER_SECT( psCP )  )
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else if ( FALSE == vds_IsValidSn(psCP, iSN))
    {
        lRet = _ERR_VDS_PARAM_ERROR;
    }
    else
    {
        uLen = VDS_SZ_PB( psCP );
        uWriteLen = 0;
        uAddr = VDS_ADDR_PB( psCP, iSN ) + uLen * iPBN;
        /*
        CSW_TRACE(BASE_DSM_TS_ID,
            "write:uAddr = 0x%x,VDS_ADDR_PB = 0x%x,VDS_ADDR_PBDT = 0x%x,VDS_SZ_PBDT = 0x%x,VDS_SZ_PB = 0x%x,uLen = %d,iPBN = %d.",
           uAddr,
           VDS_ADDR_PB( psCP, iSN ),
           VDS_ADDR_PBDT(psCP, iSN),
           VDS_SZ_PBDT(psCP),
           VDS_SZ_PB(psCP),
           uLen,
           iPBN
         );
        */
        lResult = DRV_WriteFlash( uAddr, pBuff, uLen, &uWriteLen );
        if ( ( lResult == ERR_SUCCESS ) && ( uWriteLen == uLen ) )
        {
            lRet = ERR_SUCCESS;
        }
        else
        {
            D( ( DL_VDSERROR, "vds_WritePB: Call  DRV_WriteFlash failed,iRet = %d,write size = %d,written size = %d.", lResult, uLen, uWriteLen) );
            lRet = _ERR_VDS_WRITE_FLASH_FAILED;
        }
    }
    return lRet;
}

INT32 vds_ReadPBDT( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iBegIndex, UINT16 iNum, VOID *pBuf )
{
    UINT32 saddr = VDS_ADDR_PBDT ( psCP, iSN );
    UINT32 rsize;
    UINT32 err_code;

    //if ( ( NULL == psCP ) || ( iSN >= VDS_NR_SECT( psCP ) ) )
    if ( NULL == psCP  )
    {
        D( ( DL_VDSERROR, "In vds_ReadPBDT, _ERR_VDS_PARAM_ERROR.\n") );
        return _ERR_VDS_PARAM_ERROR;
    }

    err_code = DRV_ReadFlash( saddr + iBegIndex * VDS_SZ_PBD, pBuf, VDS_SZ_PBD * iNum, &rsize );
    if ( ( err_code != ERR_SUCCESS ) || ( rsize != VDS_SZ_PBD * iNum ) )
    {
        D( ( DL_VDSERROR, "vds_ReadPBDT: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
             saddr + iBegIndex * VDS_SZ_PBD,
             err_code,
             VDS_SZ_PBD * iNum,
             rsize ));
        return _ERR_VDS_READ_FLASH_FAILED;
    }
    return ( iBegIndex + iNum );
}

INT32 vds_ReadSHWithCrc( VDS_PARTITION *psCP, UINT16 iSN, VDS_SH *psSH )
{
    UINT32 saddr = VDS_ADDR_SECT(iSN );
    UINT32 rsize;
    UINT32 iCrc;
    UINT32 err_code;
    UINT32 iOriginalMagic;
    UINT32 iOriginalStatus;

    if ( ( NULL == psCP ) ||
            (FALSE == vds_IsValidSn(psCP, iSN)) )
    {
        return _ERR_VDS_PARAM_ERROR;
    }
    err_code = DRV_ReadFlash( saddr, ( UINT8 * ) psSH, VDS_SZ_SH, &rsize );
    if ( ( err_code != ERR_SUCCESS ) || ( rsize != VDS_SZ_SH ) )
    {
        D( ( DL_VDSERROR, "vds_ReadSHWithCrc: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
             saddr,
             err_code,
             VDS_SZ_SH,
             rsize ));
        return _ERR_VDS_READ_FLASH_FAILED;
    }

    iOriginalMagic = psSH->iMagic;
    iOriginalStatus = psSH->iSectorStatus;

    iCrc = vds_GetShCrc(*psSH);
    if ( iCrc != psSH->iCRC )
    {
        D( ( DL_VDSERROR, "vds_ReadSHWithCrc: saddr = 0x%x, crc = 0x%x, calculated crc: 0x%x\n",
             saddr, psSH->iCRC, iCrc ) );
        return ERR_VDS_CRC_ERROR;
    }
    psSH->iMagic = iOriginalMagic;
    psSH->iSectorStatus = iOriginalStatus;
    return ERR_SUCCESS;
}


INT32 vds_WriteSH( VDS_PARTITION *psCP, UINT16 iSN, VDS_SH *psSH )
{
    UINT32 saddr = VDS_ADDR_SECT(iSN );
    UINT32 wsize = 0, rsize = 0;
    UINT32 err_code;
    VDS_SH sh;

    if (( NULL == psCP ) ||
            (FALSE == vds_IsValidSn(psCP, iSN)))
    {
        return _ERR_VDS_PARAM_ERROR;
    }
    /*
    VDS_SH tst_sh;
    err_code = DRV_ReadFlash( saddr, ( UINT8* ) & tst_sh, VDS_SZ_SH, &rsize );
    if ( ( err_code != ERR_SUCCESS ) || ( rsize != VDS_SZ_SH ) )
    {
        D( ( DL_ERROR, "DRV_ReadFlash failed.error code = %d,saddr = %d,read size = %d, out size = %d.\n",
             err_code,saddr,VDS_SZ_SH,rsize));
        return _ERR_VDS_READ_FLASH_FAILED;
    }
    */
    err_code = DRV_WriteFlash( saddr, ( UINT8 * ) psSH, VDS_SZ_SH, &wsize );
    if ( ( err_code != ERR_SUCCESS ) || ( wsize != VDS_SZ_SH ) )
    {
        D( ( DL_VDSERROR, "vds_WriteSH:call DRV_WriteFlash failed.error code = %d,saddr = %d,read size = %d, out size = %d.\n",
             err_code, saddr, VDS_SZ_SH, rsize));
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }
    rsize = 0;

    err_code = DRV_ReadFlash( saddr, ( UINT8 * ) & sh, VDS_SZ_SH, &rsize );
    if ( ( err_code != ERR_SUCCESS ) || ( rsize != VDS_SZ_SH ) )
    {
        D( ( DL_VDSERROR, "DRV_ReadFlash failed.error code = %d,saddr = %d,read size = %d, out size = %d.\n",
             err_code, saddr, VDS_SZ_SH, rsize));
        return _ERR_VDS_READ_FLASH_FAILED;
    }
    if ( psSH->iMagic != sh.iMagic || psSH->iSectorStatus != sh.iSectorStatus )
    {
        D( ( DL_VDSERROR, "Checking write no pass.mem magic = 0x%x,flash magic = 0x%x,mem status = 0x%x,flash status = 0x%x.\n",
             psSH->iMagic, sh.iMagic, psSH->iSectorStatus, sh.iSectorStatus));
        return _ERR_VDS_WRITE_CHECK_ERROR;
    }

    return ERR_SUCCESS;
}


INT32 vds_ReadSH( VDS_PARTITION *psCP, UINT16 iSN, VDS_SH *psSH )
{
    UINT32 saddr = VDS_ADDR_SECT(iSN );
    UINT32 rsize = 0;
    UINT32 err_code;
    VDS_SH sh;

    if ( ( NULL == psCP ) ||
            ( FALSE == vds_IsValidSn(psCP, iSN) ))
    {
        return _ERR_VDS_PARAM_ERROR;
    }


    rsize = 0;
    err_code = DRV_ReadFlash( saddr, ( UINT8 * ) &sh, VDS_SZ_SH, &rsize );
    if ( ( err_code != ERR_SUCCESS ) || ( rsize != VDS_SZ_SH ) )
    {
        D( ( DL_VDSERROR, "DRV_ReadFlash failed.error code = %d,saddr = %d,read size = %d, out size = %d.\n",
             err_code, saddr, VDS_SZ_SH, rsize));
        return _ERR_VDS_READ_FLASH_FAILED;
    }
    DSM_MemCpy(psSH, &sh, VDS_SZ_SH);

    return ERR_SUCCESS;
}

/*
typedef enum
{
    HAL_DBG_PAGE_UD0=0,          ///< User defined, page 0
    HAL_DBG_PAGE_IRQ_STACKWATCH=1, ///< Reserved for IRQ stack overflow protection
    HAL_DBG_PAGE_CODEWATCH=2,    ///< Reserved for code protection
    HAL_DBG_PAGE_INT_CODEWATCH=3, ///< Reserved for internal SRAM code protection
    HAL_DBG_PAGE_4,
    HAL_DBG_PAGE_5,

    HAL_DBG_PAGE_QTY
}HAL_DBG_PAGE_NUM_T;
extern VOID hal_DbgPageProtectSetup(
        HAL_DBG_PAGE_NUM_T pageNum, HAL_DBG_PAGE_SPY_MODE_T mode,
        UINT32 startAddr, UINT32 endAddr);
extern VOID hal_DbgPageProtectEnable(HAL_DBG_PAGE_NUM_T pageNum);
extern VOID hal_DbgPageProtectDisable(HAL_DBG_PAGE_NUM_T pageNum)
*/
INT32 vds_WritePBDAndPB( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, VDS_PBD *psPBD, UINT8 *pPB )
{
    INT32 lResult;
    // UINT32 uLen;
    //   VDS_PBD sCheckPBD;
    INT32 lPBCount;
    UINT16 lStatus;

    //   UINT8 szWBefor[512];

    if ( ( NULL == psCP ) || ( NULL == pPB ) || ( NULL == psPBD ) )
    {
        D( ( DL_VDSERROR, "In vds_WritePBDAndPB,PARA_ERR3,\n" ) );
        return _ERR_VDS_PARAM_ERROR;
    }
    if (FALSE == vds_IsValidSn(psCP, iSN) )
    {
        D( ( DL_VDSERROR, "In vds_WritePBDAndPB,sn is invalid.iSN = 0x%x.", iSN) );
        return _ERR_VDS_PARAM_ERROR;
    }


    // uLen = VDS_SZ_PB( psCP );
    lPBCount = VDS_NR_PB_PER_SECT( psCP );
    if ( iPBN >= lPBCount )
    {
        D( ( DL_VDSERROR, "In vds_WritePBDAndPB,PARA_ERR1,iPBN = 0x%x, iPBCount = 0x%x\n", iPBN, lPBCount ) );
        return _ERR_VDS_PARAM_ERROR;
    }

    lStatus = VDS_STATUS_PB_VALID;
    lResult = vds_SetPBStatus( psCP, iSN, iPBN, lStatus );
    if ( lResult != ERR_SUCCESS )
    {
        D( ( DL_VDSERROR, "In vds_WritePBDAndPB,SETPBStatus ERR1,iSN = 0x%x, iPBN = 0x%x,lStatus = 0x%x\n", iSN, iPBN, lStatus ) );
        lStatus = VDS_STATUS_PB_INVALID;
        lResult = vds_SetPBStatus( psCP, iSN, iPBN, lStatus );
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }


    lResult = vds_WritePB( psCP, iSN, iPBN, pPB );
    if ( ERR_SUCCESS != lResult )
    {
        // sxr_Sleep(50);
        //
        D( ( DL_VDSERROR, "In vds_WritePBDAndPB,write pb failed,err code = %d,SN = 0x%x, iPBN = 0x%x\n", lResult, iSN, iPBN ) );
        lStatus = VDS_STATUS_PB_INVALID;
        // Todo ..to dubug download smm no enough space!.
        lResult = vds_SetPBStatus( psCP, iSN, iPBN, lStatus );
        if(lResult != ERR_SUCCESS)
        {
            D( ( DL_VDSERROR, "In vds_WritePBDAndPB,set pb status failed.err code = %d,iSN = 0x%x, iPBN = 0x%x\n", lResult, iSN, iPBN ) );
        }
        //  sxr_Sleep(50);
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }

#if 1
    // caoxh add
    // hal_DbgPageProtectSetup(0, 2, (UINT32)pPB, (UINT32)(pPB+VDS_SZ_PB( psCP )));
    // hal_DbgPageProtectEnable(0);
#endif


    lResult = vds_WritePBD( psCP, iSN, iPBN, psPBD );
    if ( ERR_SUCCESS != lResult )
    {
        D( ( DL_VDSERROR, "In vds_WritePBDAndPB,WritePBD ERR, ErrCode = %d,iSN = 0x%x, iPBN = 0x%x\n", lResult, iSN, iPBN ) );
        D( ( DL_DETAIL, "sPBD.iPbCRC=%x, sPBD.iPbdCRC=%x, sPBD.iStatus=0x%x, sPBD.iVBN=0x%x, sPBD.iVersion=%d\n", psPBD->iPBCRC, psPBD->iPBDCRC, psPBD->iStatus, psPBD->iVBN, psPBD->iVersion ) );
        lStatus = VDS_STATUS_PB_INVALID;
        lResult = vds_SetPBStatus( psCP, iSN, iPBN, lStatus );
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }
    return ERR_SUCCESS;
}


/* do:
1. Update map table by scanning sector list
2. set the currentPBD for the sector reference.
   Set dirty sector's CurrentPBN to VDS_BLOCK_NONE;
   Set current sector's CurrentPBN to the last valid block.

*/
INT32 vds_ListScan( VDS_PARTITION *psCP, UINT32 uStatus )
{
    UINT16 iNrBlkScanned;
    INT32 lErrCode;
    VDS_SECT_REF *psRef = NULL;

    if ( ( NULL == psCP ) || ( ( uStatus != VDS_STATUS_SECT_CURRENT ) && ( uStatus != VDS_STATUS_SECT_DIRTY ) && (uStatus != VDS_STATUS_SECT_GCING )) )
    {
        lErrCode = _ERR_VDS_PARAM_ERROR;
        D( ( DL_VDSERROR, "In vds_ListScan, ErrCode = %d\n", lErrCode ) );
        return lErrCode;
    }
    if(VDS_STATUS_SECT_DIRTY == uStatus)
    {
        psRef = psCP->pDirtyList;
    }
    else if(VDS_STATUS_SECT_CURRENT == uStatus)
    {
        psRef = psCP->pCurrentSect;
    }
    else
    {
        psRef = NULL;
    }

    while ( NULL != psRef )
    {
        lErrCode = vds_SetMTByScanSec( psCP, psRef->iSN, &iNrBlkScanned );
        if ( ERR_SUCCESS != lErrCode )
        {
            D( ( DL_VDSERROR, "In vds_ListScan, Update map table by scanning sector failed, ErrCode = %d\n", lErrCode ) );
            return lErrCode;
        }
        if ( VDS_STATUS_SECT_DIRTY == uStatus )
        {
            psRef->iCurrentPBN = VDS_BLOCK_NONE;    //set DirtySect's CurrentPBN
        }
        else if ( VDS_STATUS_SECT_CURRENT == uStatus )
        {
            if (0 == iNrBlkScanned)
            {
                psRef->iCurrentPBN = VDS_BLOCK_NONE;
            }
            else
            {
                iNrBlkScanned--;
                psRef->iCurrentPBN = ( UINT16 )iNrBlkScanned;
            }
        }
        psRef = psRef->next;
    }
    return ERR_SUCCESS;
}


VDS_SECT_REF *vds_AllocSecRef(VOID)
{
    UINT32 i;
    VDS_SECT_REF *psRef;

    for ( i = 0; i < g_sFlashInfo.iNrSector; i++ )
    {
        psRef = g_psSectRef + i;
        if ( ( VDS_SECT_REF * ) VDS_INVALID_POINTER == psRef->next )
        {
            psRef->next = NULL;
            psRef->iSN = VDS_SECT_NONE;
            psRef->iCurrentPBN = VDS_BLOCK_NONE;
            return psRef;
        }
    }

    return NULL;
}


VDS_SECT_REF *vds_AddSecRef( VDS_SECT_REF **list, VDS_SECT_REF *psRef )
{
    if ( NULL == list || NULL == psRef )
    {
        return NULL;
    }

    psRef->next = *list;
    *list = psRef;

    return psRef;
}


VDS_SECT_REF *vds_AppendSecRef( VDS_SECT_REF **list, VDS_SECT_REF *psRef )
{
    VDS_SECT_REF **pp = list;

    if ( NULL == list || NULL == psRef )
    {
        return NULL;
    }

    while ( *pp )
    {
        pp = &( ( *pp ) ->next );
    }

    *pp = psRef;

    return psRef;
}


BOOL vds_IsSecRefListEmpty( VDS_SECT_REF *list )
{
    if(NULL == list)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


VDS_SECT_REF *vds_RemoveSecRef( VDS_SECT_REF **list, VDS_SECT_REF *psRef )
{
    VDS_SECT_REF **pp = list;

    if ( NULL == list || NULL == psRef )
    {
        return NULL;
    }

    while ( *pp )
    {
        if ( *pp == psRef )
        {
            break;
        }

        pp = &( ( *pp ) ->next );
    }

    if ( !*pp )
    {
        return NULL;
    }

    *pp = psRef->next;
    psRef->next = NULL;

    return psRef;
}


VDS_SECT_REF *vds_GetFirstSecRef( VDS_SECT_REF *list )

{
    return list;
}


VDS_SECT_REF *vds_GetNextSecRef( VDS_SECT_REF *list, VDS_SECT_REF *psRef )
{
    if ( NULL == list || NULL == psRef )
    {
        return NULL;
    }

    return psRef->next;
}


// vds_GetRemainSecRef
// Get a remain sector node.
// return the last node in remain sector chain.
VDS_SECT_REF  *vds_GetRemainSecRef(VOID)
{
    VDS_SECT_REF *pRef = NULL;

    if(NULL != g_psRemainRef)
    {
        pRef = g_psRemainRef;
        while(NULL != pRef)
        {
            if(NULL == pRef->next)
            {
                return pRef;
            }
            pRef = pRef->next;
        };
    }

    return pRef;
}


BOOL vds_IsValidSn(VDS_PARTITION *psCP, UINT16 iSn)
{
    VDS_SECT_REF *psRef = NULL;

    if(NULL == psCP)
    {
        return FALSE;
    }

    psRef = psCP->pDirtyList;
    while(psRef)
    {
        if(psRef->iSN == iSn)
        {
            return TRUE;
        }
        psRef = psRef->next;
    }

    psRef = psCP->pCurrentSect;
    if(NULL != psRef)
    {
        if(psRef->iSN == iSn)
        {
            return TRUE;
        }
    }
    psRef = psCP->pGcingSect;
    if(NULL != psRef)
    {
        if(psRef->iSN == iSn)
        {
            return TRUE;
        }
    }
    psRef = psCP->pTempSect;
    if(NULL != psRef)
    {
        if(psRef->iSN == iSn)
        {
            return TRUE;
        }
    }

    psRef = g_psRemainRef;
    if(NULL != psRef)
    {
        if(psRef->iSN == iSn)
        {
            return TRUE;
        }
    }


    return FALSE;
}


INT32 vds_SetPBDCRC( VDS_PARTITION *psCP, UINT16 iSN, UINT16 iPBN, UINT32 iCRC )
{
    UINT32 iSectAddr = VDS_ADDR_SECT( iSN );
    UINT32 iPBDAddr = VDS_SZ_PBD * iPBN;
    UINT32 iWSize;
    UINT32 iRSize;
    UINT32 err_code;
    UINT32 iCRCRead;

    if ( NULL == psCP
            || FALSE == vds_IsValidSn(psCP, iSN)
            || iPBN > VDS_NR_PB_PER_SECT( psCP ) )
    {
        return _ERR_VDS_PARAM_ERROR;
    }

    err_code = DRV_WriteFlash( iSectAddr + iPBDAddr + VDS_POS_PBD_PBDCRC,
                               ( UINT8 * ) & iCRC, VDS_SZ_PBD_PBDCRC, &iWSize );

    if ( ( err_code != ERR_SUCCESS ) || ( iWSize != VDS_SZ_PBD_PBDCRC ) )
    {
        D( ( DL_VDSERROR, "vds_SetPBDCRC:call DRV_WriteFlash failed.error code = %d,read size = %d, out size = %d.\n",
             err_code, VDS_SZ_PBD_PBDCRC, iWSize));
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }

    /*read back for safety*/
    iRSize = 0;
    err_code = DRV_ReadFlash( iSectAddr + iPBDAddr + VDS_POS_PBD_PBDCRC,
                              ( UINT8 * ) & iCRCRead, VDS_SZ_PBD_PBDCRC, &iRSize );

    if ( ( err_code != ERR_SUCCESS ) || ( iRSize != VDS_SZ_PBD_PBDCRC ) )
    {
        return _ERR_VDS_READ_FLASH_FAILED;
    }

    if ( iCRCRead != iCRC )
    {
        D( ( DL_VDSERROR, "vds_SetPBDCRC: call DRV_ReadFlash failed. addr = 0x%x, iRet = %d,read size= %d, read out  = %d.",
             iSectAddr + iPBDAddr + VDS_POS_PBD_PBDCRC,
             err_code,
             VDS_SZ_PBD_PBDCRC,
             iRSize ));
        return _ERR_VDS_WRITE_CHECK_ERROR;
    }

    return ERR_SUCCESS;
}

INT32 vds_SetAllSectorsEraseFlag(VOID)
{
    INT32 iResult;
    UINT16 i;
    INT32 iRet = 0;
    UINT32 iSecAddr;
    UINT32 iWrittenLen = 0;
    UINT8 buff[8];

    // Get flash information.
    iResult = VDS_GetFlashInfo( &g_sFlashInfo );
    if ( ERR_SUCCESS != iResult )
    {
        D( ( DL_VDSERROR, "vds_SetAllSectorsEraseFlag: get flash information failed err code = %d.", iResult ));
        g_tstVdsErrCode = 1001;
        iRet = _ERR_VDS_FLASH_INFO_ERROR;
        return iRet;
    }
    DSM_MemSet(buff,0,8);
    for(i = 0; i < g_sFlashInfo.iNrSector; i++)
    {
        iSecAddr = g_sFlashInfo.iFlashStartAddr + i*g_sFlashInfo.iFlashSectorSize;
        iResult = DRV_WriteFlash(iSecAddr,buff,8,&iWrittenLen);
        if(iResult != 0)
        {
            D( ( DL_VDSERROR, "vds_SetAllSectorsEraseFlag: write flash fail code = %d.", iResult ));
            return _ERR_VDS_WRITE_SH_FAILED;
        }
    }
    return 0;
}
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
VOID VDS_ModuleSemInit(VOID)
{
    return;

}


VOID VDS_WaitForSemaphore(VOID)
{
    return;

}


VOID VDS_ReleaseSemaphore(VOID)
{
    return;
}


VOID vds_SetFlushAllowed(VOID)
{
    g_iFlushAllowed = 1;
}


VOID vds_SetFlushUnallowed(VOID)
{
    g_iFlushAllowed = 0;
}

VOID vds_DisableFlush(UINT32 keep_time)
{
    g_iFlushDisableKeepTime = keep_time;
    g_iFlushDisableBeginTime = hal_TimGetUpTime();
}

VOID vds_EnableFlush(VOID)
{
    g_iFlushDisableKeepTime = 0;
    g_iFlushDisableBeginTime = 0;
}

BOOL vds_IsFlushAllowed(VOID)
{
    BOOL bRet = FALSE;

    if(g_iFlushAllowed == 1)
    {
        bRet = TRUE;
        /*
        if(g_iFlushDisableBeginTime > 0 &&  g_iFlushDisableKeepTime > 0)
        {
             if((INT32)((INT32)hal_TimGetUpTime() - (INT32)g_iFlushDisableBeginTime) < (INT32)g_iFlushDisableKeepTime)
             {
                  bRet = FALSE;
             }
             else
             {
                  bRet = TRUE;
             }
        }
        */
    }

    return bRet;
}




VOID vds_InitCache(VOID)
{
    g_iVdsCacheHasInited = 1;
}


BOOL vds_IsCacheInited(VOID)
{
    return  g_iVdsCacheHasInited == 1 ?  TRUE : FALSE;
}

#else

VOID VDS_ModuleSemInit(VOID)
{
    g_hModuleSem = sxr_NewSemaphore(1);
    if((HANDLE)NULL == g_hModuleSem)
    {
        D( ( DL_WARNING, "VDS_ModuleSemInit failed."));
    }
}


VOID VDS_WaitForSemaphore(VOID)
{
    if((HANDLE)NULL != g_hModuleSem)
    {
        sxr_TakeSemaphore(g_hModuleSem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleDown failed."));
    }

}


VOID VDS_ReleaseSemaphore(VOID)
{

    if((HANDLE)NULL != g_hModuleSem)
    {
        sxr_ReleaseSemaphore(g_hModuleSem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleUp failed."));
    }
}

#endif



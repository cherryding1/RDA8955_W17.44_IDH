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
#endif

#include "dsm_cf.h"
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
UINT8 g_pGcBuff[VDS_NR_PBD_PER_GROUP * VDS_BLOCK_SIZE];

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
extern UINT32 hal_TimGetUpTime(VOID);
// gc scane.
VDS_GC_SCENE g_GcScane = {FALSE, VDS_GC_FINISH, NULL, NULL, NULL, 0, NULL, 0, 0};
#endif

VDS_SECT_REF *vds_GetGcingSector( VDS_PARTITION *psPart )
{
    VDS_SECT_REF *psGcSect = NULL;
    VDS_SECT_REF *psTmpSect = NULL;
    INT32 iMinValidNum;
    INT32 iTmpNum = 0;
    INT32 iRet;

    if ( NULL != psPart->pGcingSect )
    {
        return psPart->pGcingSect;
    }

    if ( vds_IsSecRefListEmpty( psPart->pDirtyList ) )
    {
        return NULL;
    }

    // Find the least valid-block sector in dirty list.
    iMinValidNum = VDS_NR_PB_PER_SECT(psPart);
    psTmpSect = vds_GetFirstSecRef( psPart->pDirtyList );
    while ( NULL != psTmpSect )
    {
        iTmpNum = vds_CountBlock( psPart, psTmpSect, 0, VDS_STATUS_PB_VALID );
        if ( iTmpNum < iMinValidNum )
        {
            iMinValidNum = iTmpNum;
            psGcSect = psTmpSect;
        }
        psTmpSect = vds_GetNextSecRef( psPart->pDirtyList, psTmpSect );
    }

    if ( NULL == psGcSect )
    {
        return NULL;
    }
    else
    {
        iRet = vds_ChangeSectStatus( psPart, psGcSect,
                                     VDS_STATUS_SECT_DIRTY, VDS_STATUS_SECT_GCING );
    }
    if ( ERR_SUCCESS != iRet )
    {
        return NULL;
    }

    psPart->pGcingSect = psGcSect;

    return psGcSect;

};



VDS_SECT_REF *vds_GetRemainSector( VDS_PARTITION *psPart )
{
    VDS_SECT_REF *remain = NULL;
    INT32 iRet;

    if ( NULL == psPart )
    {
        return NULL;
    }
    remain = g_psRemainRef;

    if ( NULL != remain )
    {
        iRet = vds_ChangeSectStatus( psPart, remain, VDS_STATUS_SECT_REMAIN, VDS_STATUS_SECT_TEMP);
        if ( ERR_SUCCESS != iRet )
        {
            D( ( DL_VDSERROR, "vds get remain sector:change sector status failed, iRet = %d\n", iRet ) );
            return NULL;
        }
    }
    else
    {
        D( ( DL_WARNING, "vds get remain sector:the remain ref is NULL.") );
        return NULL;
    }

    if ( VDS_BLOCK_NONE == remain->iCurrentPBN )
    {
        iRet = vds_GetFirstBlockPos( psPart, remain, VDS_STATUS_PB_FREE, (INT16 *)&remain->iCurrentPBN );
        if ( ERR_SUCCESS != iRet )
        {
            DSM_ASSERT(0,"iRet = %d",iRet);
            D( ( DL_VDSERROR, "In vds_GetRemainSector, call vds_GetFirstBlockPos return failed, iRet = %d\n", iRet ) );
            return NULL;
        }
        if(remain->iCurrentPBN >= VDS_NR_PB_PER_SECT( psPart ))
        {
            DSM_ASSERT(0,"iCurrentPBN = 0x%x",remain->iCurrentPBN);
        }
    }
    return remain;
}


/////////////////////////////////////
// just for GC.
////////////////////////////////////
INT32 vds_ChangeSectStatus( VDS_PARTITION *psPart,
                            VDS_SECT_REF *psSect, UINT32 iOldStatus, UINT32 iNewStatus )
{
    VDS_SH sSH;
    UINT32 iSecAddr;
    UINT32 iEraseCounter;
    INT32 iRet;

    if ( NULL == psPart || NULL == psSect )
    {
        return _ERR_VDS_PARAM_ERROR;
    }

    switch ( iNewStatus )
    {
        case VDS_STATUS_SECT_FREE:
            if ( VDS_STATUS_SECT_GCING == iOldStatus )
            {
                psPart->pGcingSect = NULL;
            }
            else if ( VDS_STATUS_SECT_REMAIN == iOldStatus )
            {
                g_psRemainRef = NULL;
            }

            psSect->iCurrentPBN = VDS_BLOCK_NONE;

            iRet = vds_ReadSH( psPart, psSect->iSN, &sSH );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues: read SH failed,err code = %d.", iRet  ) );
                return _ERR_VDS_READ_SH_FAILED;
            }

            iEraseCounter = sSH.iEraseCounter;

            DSM_MemSet(&sSH, 0x0, VDS_SZ_SH);
            sSH.iMagic = VDS_ERASING_MAGIC;
            sSH.iSectorStatus = VDS_STATUS_SECT_ERASING;
            sSH.iEraseCounter = iEraseCounter;

            iRet = vds_WriteSH( psPart, psSect->iSN, &sSH );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues:write sh failed.err code = %d,sn = %d.", iRet, psSect->iSN) );
                return _ERR_VDS_WRITE_SH_FAILED;
            }

            iSecAddr = VDS_ADDR_SECT( psSect->iSN );
            iRet = vds_EraseSec( iSecAddr );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues:(->free)erase sector failed.err code = %d,addr = %d.", iRet, iSecAddr) );
                return _ERR_VDS_ERASE_FLASH_FAILED;
            }
            vds_AppendSecRef( &psPart->pFreeList, psSect );
            return ERR_SUCCESS;
            break;
        case VDS_STATUS_SECT_GCING:
            if ( VDS_STATUS_SECT_DIRTY == iOldStatus )
            {
                psPart->pGcingSect = vds_RemoveSecRef( &(psPart->pDirtyList), psSect );
                iRet = vds_SetSectStatus(psPart->pGcingSect->iSN, VDS_STATUS_SECT_GCING );
                if ( ERR_SUCCESS != iRet )
                {
                    D( ( DL_VDSERROR, "change sect statues:set the sector statues(dirty -> gcing) failed.err code = %d,sn = %d.", iRet, psPart->pGcingSect->iSN) );
                    return _ERR_VDS_CHANGE_STATUS_FAILED;
                }
                else
                {
                    return iRet;
                }
            }
            else
            {
                return _ERR_VDS_PARAM_ERROR;
            }
            break;
        case VDS_STATUS_SECT_CURRENT:
            if ( VDS_STATUS_SECT_TEMP != iOldStatus )
            {
                return _ERR_VDS_PARAM_ERROR;
            }

            iRet = vds_SetSectStatus(psSect->iSN, VDS_STATUS_SECT_CURRENT );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues:set the sector statues(gcdest -> current) failed.err code = %d,sn = %d.", iRet, psPart->pGcingSect->iSN) );
                return _ERR_VDS_CHANGE_STATUS_FAILED;
            }
            psSect->iCurrentPBN--;
            vds_AppendSecRef( &(psPart->pCurrentSect), psSect );
            psPart->pTempSect = NULL;
            return ERR_SUCCESS;
            break;
        case VDS_STATUS_SECT_DIRTY:
            if ( VDS_STATUS_SECT_REMAIN == iOldStatus )
            {
                g_psRemainRef = NULL;
            }
            else
            {
                return _ERR_VDS_PARAM_ERROR;
            }

            iRet = vds_SetSectStatus(psSect->iSN, VDS_STATUS_SECT_DIRTY );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues:set the sector statues(remain -> dirty) failed.err code = %d,sn = %d.", iRet, psPart->pGcingSect->iSN) );
                return _ERR_VDS_CHANGE_STATUS_FAILED;
            }

            psSect->iCurrentPBN = VDS_BLOCK_NONE;

            vds_AppendSecRef( &(psPart->pDirtyList), psSect );
            return ERR_SUCCESS;
        case VDS_STATUS_SECT_TEMP:
            if ( VDS_STATUS_SECT_REMAIN != iOldStatus )
            {
                return _ERR_VDS_PARAM_ERROR;
            }
            // DSM_MemSet(&sSH,0x0,VDS_SZ_SH);

            iRet = vds_ReadSH( psPart, psSect->iSN, &sSH );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues: read SH failed,err code = %d.", iRet  ) );
                return _ERR_VDS_READ_SH_FAILED;
            }

            sSH.iLayoutVer = VDS_LAYOUT_VERSION;
            sSH.iMagic = VDS_MAGIC;
            sSH.iFlashSectorSize = psPart->sPartInfo.iFlashSectorSize;
            sSH.iVBSize = psPart->sPartInfo.iVBSize;
            sSH.iFlashSecCount = psPart->sPartInfo.iFlashSectorCount;
            sSH.iReservedBlock = psPart->sPartInfo.iRsvBlkCount;
            sSH.iSectorStatus = VDS_STATUS_SECT_TEMP;
            DSM_StrCpy( sSH.acPartName, psPart->sPartInfo.acPartName);
            sSH.iCRC = vds_GetShCrc(sSH);

            iRet = vds_WriteSH( psPart, psSect->iSN, &sSH );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "Change sector status failed*********, %d", iRet  ) );
                return _ERR_VDS_WRITE_SH_FAILED;
            }
            psSect->iCurrentPBN = VDS_BLOCK_NONE;
            vds_AppendSecRef( &(psPart->pTempSect), psSect );
            g_psRemainRef = NULL;
            return ERR_SUCCESS;
            break;
        case VDS_STATUS_SECT_REMAIN:
            iRet = vds_ReadSH( psPart, psSect->iSN, &sSH );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues: read SH failed,err code = %d.", iRet  ) );
                return _ERR_VDS_READ_SH_FAILED;
            }

            iEraseCounter = sSH.iEraseCounter;

            psSect->iCurrentPBN = VDS_BLOCK_NONE;

            DSM_MemSet(&sSH, 0x0, VDS_SZ_SH);
            sSH.iMagic = VDS_ERASING_MAGIC;
            sSH.iSectorStatus = VDS_STATUS_SECT_ERASING;
            sSH.iEraseCounter = iEraseCounter;
            iRet = vds_WriteSH( psPart, psSect->iSN, &sSH );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues:wite sh failed.err code = %d,sn = %d.", iRet, psSect->iSN) );
                return _ERR_VDS_WRITE_SH_FAILED;
            }

            iSecAddr = VDS_ADDR_SECT( psSect->iSN );
            iRet = vds_EraseSec( iSecAddr );
            if ( ERR_SUCCESS != iRet )
            {
                D( ( DL_VDSERROR, "change sect statues:(->remain)erase sector failed.err code = %d,addr = %d.", iRet, iSecAddr) );
                return _ERR_VDS_ERASE_FLASH_FAILED;
            }

            if ( VDS_STATUS_SECT_GCING == iOldStatus )
            {
                psPart->pGcingSect = NULL;
            }
            else if ( NULL != g_psRemainRef)
            {
                return _ERR_VDS_REM_SEC_EXIST;
            }
            psSect->iCurrentPBN = VDS_BLOCK_NONE;
            g_psRemainRef = psSect;
            return ERR_SUCCESS;
            break;
        default:
            return _ERR_VDS_PARAM_ERROR;

    }
    return ERR_SUCCESS;
}

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
INT32 vds_GcInit(VDS_PARTITION *psPart)
{

    //vds_Gc begin.

    // 0 Check parameter.
    if ( NULL == psPart )
    {
        D( ( DL_VDSERROR, "In vds_Gc, \"psPart\" paramater null." ) );
        DSM_HstSendEvent(0xC0000001);
        return _ERR_VDS_PARAM_ERROR;
    }

    if ( psPart->sPartInfo.iFlashSectorCount != psPart->iRefCount )
    {
        D( ( DL_VDSERROR, "In vds_Gc, partition not initialized." ) );
        DSM_HstSendEvent(0xC0000002);
        return ERR_VDS_UNINIT;

    }

    // 1 Get GC sector.
    g_GcScane.psGcSect = vds_GetGcingSector( psPart );
    if ( NULL == g_GcScane.psGcSect )
    {
        D( ( DL_VDSERROR, "In vds_Gc, get gcing sector failed!" ) );
        DSM_HstSendEvent(0xC0000003);
        return _ERR_VDS_DIRTY_LIST_NULL;
    }

    // 2 Get remain sector.
    g_GcScane.psRemSect = vds_GetRemainSector( psPart );
    if ( NULL == g_GcScane.psRemSect )
    {
        D( ( DL_VDSERROR, "In vds_Gc, get remain sector failed!" ) );
        DSM_HstSendEvent(0xC0000004);
        return _ERR_VDS_REM_SECT_NULL;
    }
    else
    {
        /*
        CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(4), TSTR("vds_GcInit: remain-sector:sn = 0x%x, cur pbn = 0x%x.gc-sector:remain-sector:sn = 0x%x, cur pbn = 0x%x.",0x08000189),
         g_GcScane.psRemSect->iSN,
         g_GcScane.psRemSect->iCurrentPBN,
         g_GcScane.psGcSect->iSN,
         g_GcScane.psGcSect->iCurrentPBN);
         */
    }
    // DSM_HstSendEvent(0x77000000|(g_GcScane.psRemSect->iCurrentPBN<<8)|g_GcScane.psRemSect->iSN);
    return ERR_SUCCESS;
}


INT32 vds_GcCopy(VDS_PARTITION *psPart, UINT32 iCount)
{
    INT32 iRet = ERR_SUCCESS;
    INT32 iResult;
    UINT16 iWCount = 0;
    UINT16 iCheckCount = 0;
    VDS_PBD   psPbdBuff[VDS_NR_PBD_PER_GROUP];
    VDS_PBD   psTmpPbdBuff[VDS_NR_PBD_PER_GROUP];
    VDS_PBD *pTmpPbd = NULL;
    VDS_PBD *pPbd = NULL;
    UINT8 *pPb = NULL;
    UINT32 iPbdAddr, iPbAddr;
    UINT32 iGroupCount = 0;
    UINT32 iWritenLen;
    UINT32 i;

    //Copy PB and PBD.
    if(g_GcScane.pGcBuff)
    {
        pPb = g_GcScane.pGcBuff;
    }
    else
    {
        DSM_HstSendEvent(0xbb02);
        D( ( DL_VDSERROR, "In vds_Gc: gc buffer = NULL.") );
        return _ERR_VDS_GC_DEACTIVE;
    }
    //CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3), TSTR("gc copy: psGcSect->iSN = 0x%x,g_GcScane.psRemSect->iSN = 0x%x,g_GcScane.iIndex = 0x%x.",0x0800018a),g_GcScane.psGcSect->iSN,g_GcScane.psRemSect->iSN,g_GcScane.iIndex);
    pPbd = psPbdBuff;

    iGroupCount = VDS_NR_PBD_PER_GROUP;
    iPbdAddr = VDS_ADDR_PBDT(psPart, g_GcScane.psRemSect->iSN) + (g_GcScane.psRemSect->iCurrentPBN * VDS_SZ_PBD);
    iPbAddr = VDS_ADDR_PB(psPart, g_GcScane.psRemSect->iSN) + (g_GcScane.psRemSect->iCurrentPBN * VDS_SZ_PB(psPart));
    do
    {
        if(g_GcScane.iIndex >= VDS_NR_PB_PER_SECT(psPart))
        {
            iRet = iCheckCount;
            // DSM_HstSendEvent(0xb1000000|(g_GcScane.iIndex<<8)|g_GcScane.psRemSect->iSN);
            // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(2), TSTR("vds_GcCopy: g_GcScane.iIndex(0x%x) > VDS_NR_PB_PER_SECT(0x%x),break.",0x0800018b),g_GcScane.iIndex,VDS_NR_PB_PER_SECT(psPart));
            break;
        }

        if( iCount <= iCheckCount)
        {
            iRet = iCheckCount;
            // DSM_HstSendEvent(0xb2000000|(iCheckCount<<8)|g_GcScane.psRemSect->iSN);
            // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(2), TSTR("vds_GcCopy: g_GcScane.iCount(0x%x) <= iCheckCount(0x%x),break.",0x0800018c),iCount,iCheckCount);
            break;
        }

        if(iGroupCount < iCount - iCheckCount)
        {
            iGroupCount = VDS_NR_PBD_PER_GROUP;
        }
        else
        {
            iGroupCount = iCount - iCheckCount;
        }

        iGroupCount = (g_GcScane.iIndex + iGroupCount) >  VDS_NR_PB_PER_SECT(psPart) ? (VDS_NR_PB_PER_SECT(psPart) - g_GcScane.iIndex) : iGroupCount;

        // Read original PBD entries.
        iResult = vds_ReadPBDT(psPart, g_GcScane.psGcSect->iSN, g_GcScane.iIndex, iGroupCount, psTmpPbdBuff);
        if(iResult < 0)
        {
            D( ( DL_VDSERROR, "In vds_Gc, Read  PBDT failed,err code = %d.", iResult ) );
            // DSM_HstSendEvent(0xb3000000|(g_GcScane.iIndex<<8)|g_GcScane.psRemSect->iSN);
            iRet = _ERR_VDS_READ_FLASH_FAILED;
            goto _func_end;
        }

        // Select the Valid PBD from original PBD Entries,add it to PBD buff.
        // Read the valid PB to PB buffer.
        // When Pbd buff is full,the PB buff is full too,Write the PB buff and PBD buffer to GCing sector.
        for(i = 0; i < iGroupCount; i ++)
        {
            pTmpPbd = psTmpPbdBuff + i;
            if(VDS_NR_PB_PER_SECT(psPart) == g_GcScane.iIndex)
            {
                // Has gone to last PB.
                CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(1), TSTR("vds_GcCopy: complete.g_GcScane.iIndex = 0x%x.",0x0800018d), g_GcScane.iIndex);
            }
            else if(VDS_STATUS_PB_FREE == pTmpPbd->iStatus)
            {
                // find free PB,stop scan.
                CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(0), TSTR("vds_GcCopy: pbd is free,break1.",0x0800018e));
            }
            else if(VDS_STATUS_PB_VALID == pTmpPbd->iStatus)
            {
                // Copy a valid PBD to PBD buffer in original PBD entris.
                // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(1), TSTR("vds_GcCopy: pbd(0x%x) is valid.",0x0800018f),pTmpPbd->iVBN);
                pTmpPbd->iVersion ++;
                DSM_MemCpy(pPbd, pTmpPbd, VDS_SZ_PBD);

                // Read a valid PB to PB buffer.
                iResult = vds_ReadPB(psPart, g_GcScane.psGcSect->iSN, g_GcScane.iIndex, pPb);
                if(ERR_SUCCESS != iResult)
                {
                    D( ( DL_VDSERROR, "In vds_Gc, Read  PB failed,err code = %d.", iResult ) );
                    iRet = _ERR_VDS_READ_FLASH_FAILED;
                    DSM_HstSendEvent(0xb4000000 | (g_GcScane.iIndex << 8) | g_GcScane.psRemSect->iSN);
                    goto _func_end;
                }

                pPbd ++;
                pPb += VDS_SZ_PB(psPart);
                iWCount ++;
            }
            else if(VDS_STATUS_PB_INVALID == pTmpPbd->iStatus)
            {
                // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(1), TSTR("vds_GcCopy: pbd(0x%x) is invalid,continue.",0x08000190),pTmpPbd->iVBN);
            }
            else
            {
                D( ( DL_VDSERROR, "In vds_Gc, error.") );
            }
            g_GcScane.iIndex++;
            iCheckCount ++;

            // group buffer is full || index has gone to last pb || scan pb count equat to input count ||find free pb .
            if((VDS_NR_PBD_PER_GROUP == iWCount) ||
                    (VDS_NR_PB_PER_SECT(psPart) == g_GcScane.iIndex) ||
                    (iCount <= iCheckCount) ||
                    (VDS_STATUS_PB_FREE == pTmpPbd->iStatus))
            {
                // Write PBD buffer to gcing sector.
                iResult = DRV_WriteFlash(iPbdAddr, (UINT8 *)psPbdBuff, iWCount * VDS_SZ_PBD, &iWritenLen);
                if(iResult < 0 || iWritenLen != iWCount * VDS_SZ_PBD)
                {
                    D( ( DL_VDSERROR, "In vds_Gc, write PBD failed,err code = %d.", iResult ) );
                    iRet = _ERR_VDS_WRITE_FLASH_FAILED;
                    DSM_HstSendEvent(0xb5000000 | (g_GcScane.iIndex << 8) | g_GcScane.psRemSect->iSN);
                    goto _func_end;
                }
                else
                {
                    // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(1), TSTR("vds_GcCopy: write pdb ok.wcount = 0x%x.",0x08000191),iWCount);
                }

                // Write PB buffer to gcing sector.
                iResult = DRV_WriteFlash(iPbAddr, g_GcScane.pGcBuff, iWCount * VDS_SZ_PB(psPart), &iWritenLen);
                if(iResult < 0 || iWritenLen != iWCount * VDS_SZ_PB(psPart))
                {
                    D( ( DL_VDSERROR, "In vds_Gc, write PB failed,err code = %d.", iResult ) );
                    iRet = _ERR_VDS_WRITE_FLASH_FAILED;
                    DSM_HstSendEvent(0xb6000000 | (g_GcScane.iIndex << 8) | g_GcScane.psRemSect->iSN);
                    goto _func_end;
                }
                else
                {
                    //CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(1), TSTR("vds_GcCopy: write PB ok.wcount = 0x%x.",0x08000192),iWCount);
                }
                pPbd = psPbdBuff;
                pPb = g_GcScane.pGcBuff;
                iPbdAddr += iWCount * VDS_SZ_PBD;
                iPbAddr += iWCount * VDS_SZ_PB(psPart);
                g_GcScane.psRemSect->iCurrentPBN  += iWCount;
                iWCount = 0;
            }
        }

        if(VDS_STATUS_PB_FREE == pTmpPbd->iStatus)
        {
            iRet = iCheckCount;
            CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(0), TSTR("vds_GcCopy: pbd status is free,break2.",0x08000193));
            //DSM_HstSendEvent(0xb7000000|(g_GcScane.iIndex<<8)|g_GcScane.psRemSect->iSN);
            break;
        }
        if(VDS_NR_PB_PER_SECT(psPart)  == g_GcScane.iIndex)
        {
            // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(2), TSTR("vds_GcCopy: VDS_NR_PB_PER_SECT(0x%x)  == g_GcScane.iIndex(0x%x),break.",0x08000194),VDS_NR_PB_PER_SECT(psPart),g_GcScane.iIndex);

        }
    }
    while(VDS_NR_PB_PER_SECT(psPart)  > g_GcScane.iIndex);
    // CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(2), TSTR("vds_GcCopy: g_GcScane end.g_GcScane.iIndex = 0x%x,psRemSect->iCurrentPBN  = 0x%x.",0x08000195),g_GcScane.iIndex,g_GcScane.psRemSect->iCurrentPBN);
    // if all the valid-block on gcing-sector are copied,
    // or there is not valid-block on gcing-sector, finish sector GC .
_func_end:
    //DSM_HstSendEvent(0xbb000000|(g_GcScane.iIndex<<8)|g_GcScane.psRemSect->iSN);
    return iRet;
}


INT32 vds_GcSetMt(VDS_PARTITION *psPart)
{
    INT32 iRet = ERR_SUCCESS;
    INT32 iResult;
    UINT16 iScannedNr;

    UINT16 tstCurPBN;
    UINT16 tstCurSN;
    UINT16 tstPbNrPerSec;

    // Change gcing sector to remain sector.
    tstCurPBN = g_GcScane.psRemSect->iCurrentPBN;
    tstCurSN = g_GcScane.psRemSect->iSN;
    tstPbNrPerSec = VDS_NR_PB_PER_SECT( psPart );

    // CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("gc set mt: psGcSect->iSN = 0x%x,g_GcScane.psRemSect->iSN = 0x%x.",0x08000196),g_GcScane.psGcSect->iSN,g_GcScane.psRemSect->iSN);
    if(g_GcScane.psRemSect->iCurrentPBN < ( UINT16 ) VDS_NR_PB_PER_SECT( psPart ))
    {
        iRet = vds_ChangeSectStatus( psPart, g_GcScane.psRemSect,
                                     VDS_STATUS_SECT_TEMP, VDS_STATUS_SECT_CURRENT );

        if ( ERR_SUCCESS != iRet )
        {
            D( ( DL_VDSERROR, "vds_GcSetMt: Change Temp2Current failed.sn = 0x%x,cur_pbn = 0x%x.", g_GcScane.psRemSect->iSN, g_GcScane.psRemSect->iCurrentPBN));
            return _ERR_VDS_CHANGE_STATUS_FAILED;
        }
    }
    else
    {
        D( ( DL_VDSERROR, "vds_GcSetMt: cur_pbn is error.sn = 0x%x,cur_pbn = 0x%x.", g_GcScane.psRemSect->iSN, g_GcScane.psRemSect->iCurrentPBN) );
        return _ERR_VDS_CURRENT_POS_ERROR;
    }

    // Scan sector and set MT.
    // update map-table.
    iResult = vds_SetMTByScanSec( psPart, g_GcScane.psRemSect->iSN, &iScannedNr );
    if (ERR_SUCCESS != iResult)
    {
        DSM_HstSendEvent(0xdf000000 | (g_GcScane.psRemSect->iCurrentPBN << 8) | g_GcScane.psRemSect->iSN);
        D(( DL_VDSERROR, "vds_GcSetMt: update map-table failed!sn = 0x%x,err_code = %d.", g_GcScane.psRemSect->iSN, iRet));
        iRet = _ERR_VDS_UPDATE_MT_FAILED;
    }
    else
    {
        if(( UINT16 ) ( g_GcScane.psRemSect->iCurrentPBN + 1) != iScannedNr)
        {
            DSM_HstSendEvent(0xde000000 | (g_GcScane.psRemSect->iCurrentPBN << 8) | g_GcScane.psRemSect->iSN);
            D( ( DL_VDSERROR, "vds_GcSetMt:vds_SetMTByScanSec temp sector is full.") );
            iRet =  _ERR_VDS_UPDATE_MT_FAILED;
        }
        else
        {
            //DSM_HstSendEvent(0xdd000000|(g_GcScane.psRemSect->iCurrentPBN<<8)|g_GcScane.psRemSect->iSN);
            iRet = ERR_SUCCESS;
        }
    }

    return iRet;
}

#if 0
INT32 vds_GcErase(VDS_PARTITION *psPart)
{
    INT32 iRet = ERR_SUCCESS;
    INT32 iResult;
    UINT16 tstCurPBN;
    UINT16 tstCurSN;

    // Change gcing sector to remain sector.
    tstCurPBN = g_GcScane.psGcSect->iCurrentPBN;
    tstCurSN = g_GcScane.psGcSect->iSN;

    iResult = vds_ChangeSectStatus( psPart, g_GcScane.psGcSect,
                                    VDS_STATUS_SECT_GCING, VDS_STATUS_SECT_REMAIN);
    if ( ERR_SUCCESS != iResult )
    {
        DSM_HstSendEvent(0xef000000 | (tstCurPBN << 8) | tstCurSN);
        iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
    }
    else
    {
        //DSM_HstSendEvent(0xee000000|(tstCurPBN<<8)|tstCurSN);
        iRet = ERR_SUCCESS;
    }
    return iRet;
}
#endif

INT32 vds_GcEraseBegin(VDS_PARTITION *psPart)
{
    INT32 iRet;
    INT32 iResult;
    VDS_SH sSH;
    UINT32 iSecAddr;
    // UINT32 iRSize = 0;
    // UINT32 iWSize = 0;

    iRet = vds_ReadSH( psPart, g_GcScane.psGcSect->iSN, &sSH );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_VDSERROR, "gc erase begin: read SH failed,err code = %d.", iRet  ) );
        return _ERR_VDS_READ_SH_FAILED;
    }


    g_GcScane.psGcSect->iCurrentPBN = VDS_BLOCK_NONE;

    g_GcScane.iEraseCounter = sSH.iEraseCounter;
    DSM_MemSet(&sSH, 0x0, VDS_SZ_SH);
    sSH.iMagic = VDS_ERASING_MAGIC;
    iRet = vds_WriteSH( psPart, g_GcScane.psGcSect->iSN, &sSH );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_VDSERROR, "Erase begin:wite sh failed.err code = %d,sn = %d.", iRet, g_GcScane.psGcSect->iSN) );
        return _ERR_VDS_WRITE_SH_FAILED;
    }


    iSecAddr = VDS_ADDR_SECT(g_GcScane.psGcSect->iSN );
    iResult = DRV_EraseFlashSectorFirst(iSecAddr, VDS_CACHE_ERASE_TIME_MAX);
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1), TSTR("Erase begin:erase 0x%x ok.",0x08000197), iSecAddr);
        iRet = ERR_SUCCESS;
    }
    else
    {
        if(ERR_FLASH_ERASE_SUSPEND == iResult)
        {
            g_GcScane.iSuspendTime = hal_TimGetUpTime();
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3), TSTR("Erase begin: suspend, psGcSect->iSN = 0x%x,addr = 0x%x,erase counter = %d.",0x08000198), g_GcScane.psGcSect->iSN, iSecAddr, g_GcScane.iEraseCounter);
            iRet = _ERR_VDS_GC_EREASE_SUSPEND;
        }
        else if(ERR_FLASH_DEVICE_BUSY == iResult)
        {
            g_GcScane.iSuspendTime = hal_TimGetUpTime();
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3), TSTR("Erase begin: suspend, psGcSect->iSN = 0x%x,addr = 0x%x,erase counter = %d.",0x08000199), g_GcScane.psGcSect->iSN, iSecAddr, g_GcScane.iEraseCounter);
            iRet = _ERR_VDS_GC_EREASE_BUSY;
        }
        else
        {
            iRet = _ERR_VDS_GC_ERASE;
            D( ( DL_VDSERROR, "Erase begin:erase sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
        }

    }
    return iRet;
}


INT32 vds_GcEraseBegin2(VDS_PARTITION *psPart)
{
    INT32 iRet;
    INT32 iResult;
    UINT32 iSecAddr;
    iSecAddr = VDS_ADDR_SECT(g_GcScane.psGcSect->iSN );
    iResult = DRV_EraseFlashSectorFirst(iSecAddr, VDS_CACHE_ERASE_TIME_MAX);
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1), TSTR("Erase begin:erase 0x%x ok.",0x0800019a), iSecAddr);
        iRet = ERR_SUCCESS;
    }
    else
    {
        if(ERR_FLASH_ERASE_SUSPEND == iResult)
        {
            g_GcScane.iSuspendTime = hal_TimGetUpTime();
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3), TSTR("Erase begin: suspend, psGcSect->iSN = 0x%x,addr = 0x%x,erase counter = %d.",0x0800019b), g_GcScane.psGcSect->iSN, iSecAddr, g_GcScane.iEraseCounter);
            iRet = _ERR_VDS_GC_EREASE_SUSPEND;
        }
        else if(ERR_FLASH_DEVICE_BUSY == iResult)
        {
            g_GcScane.iSuspendTime = hal_TimGetUpTime();
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3), TSTR("Erase begin: suspend, psGcSect->iSN = 0x%x,addr = 0x%x,erase counter = %d.",0x0800019c), g_GcScane.psGcSect->iSN, iSecAddr, g_GcScane.iEraseCounter);
            iRet = _ERR_VDS_GC_EREASE_BUSY;
        }
        else
        {
            iRet = _ERR_VDS_GC_ERASE;
            D( ( DL_VDSERROR, "Erase begin:erase sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
        }

    }
    return iRet;
}


INT32 vds_GcErase(VDS_PARTITION *psPart)
{
    INT32 iRet;
    INT32 iResult;
    VDS_SH sSH;
    UINT32 iSecAddr;
    // UINT32 iRSize = 0;
    // UINT32 iWSize = 0;

    iRet = vds_ReadSH( psPart, g_GcScane.psGcSect->iSN, &sSH );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_VDSERROR, "gc erase begin: read SH failed,err code = %d.", iRet  ) );
        return _ERR_VDS_READ_SH_FAILED;
    }


    g_GcScane.psGcSect->iCurrentPBN = VDS_BLOCK_NONE;

    g_GcScane.iEraseCounter = sSH.iEraseCounter;
    DSM_MemSet(&sSH, 0x0, VDS_SZ_SH);
    sSH.iMagic = VDS_ERASING_MAGIC;
    iRet = vds_WriteSH( psPart, g_GcScane.psGcSect->iSN, &sSH );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_VDSERROR, "Erase begin:wite sh failed.err code = %d,sn = %d.", iRet, g_GcScane.psGcSect->iSN) );
        return _ERR_VDS_WRITE_SH_FAILED;
    }


    iSecAddr = VDS_ADDR_SECT(g_GcScane.psGcSect->iSN );
    iResult = DRV_EraseFlashSector(iSecAddr);
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1), TSTR("Erase begin:erase 0x%x ok.",0x0800019d), iSecAddr);
        iRet = ERR_SUCCESS;
    }
    else
    {
        iRet = _ERR_VDS_GC_ERASE;
        D( ( DL_VDSERROR, "Erase begin:erase sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
    }
    return iRet;
}

INT32 vds_GcEraseResume(VDS_PARTITION *psPart)
{
    INT32 iRet;
    INT32 iResult;
    //VDS_SH sSH;
    UINT32 iSecAddr;
    // UINT32 iRSize = 0;
    // UINT32 iWSize = 0;


    if((hal_TimGetUpTime() - g_GcScane.iSuspendTime) < (8 * 16384 / 1000))
    {
        return _ERR_VDS_GC_EREASE_SUSPEND;
    }

    iSecAddr = VDS_ADDR_SECT(g_GcScane.psGcSect->iSN );
    iResult = DRV_EraseFlashSectorResume(iSecAddr, VDS_CACHE_ERASE_TIME_MAX);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("Erase resume:g_GcScane.psGcSect->iSN = 0x%x,erase 0x%x ok.",0x0800019e), g_GcScane.psGcSect->iSN, iSecAddr);
    }

    else
    {
        if(ERR_FLASH_ERASE_SUSPEND == iResult || ERR_FLASH_DEVICE_BUSY == iResult)
        {
            g_GcScane.iSuspendTime =  hal_TimGetUpTime();
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("Erase resume:suspend, g_GcScane.psGcSect->iSN = 0x%x,addr = 0x%x.",0x0800019f), g_GcScane.psGcSect->iSN, iSecAddr);
            iRet = _ERR_VDS_GC_EREASE_SUSPEND;
        }
        else
        {
            iRet = _ERR_VDS_GC_ERASE;
            D( ( DL_VDSERROR, "Erase resume:erase sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
        }

    }
    return iRet;
}


INT32 vds_GcEraseFinish(VDS_PARTITION *psPart)
{
    INT32 iRet;
    INT32 iResult;
    VDS_SH sSH;
    UINT32 iSecAddr;
    UINT32 iWSize = 0;

    iSecAddr = VDS_ADDR_SECT(g_GcScane.psGcSect->iSN );

    DSM_MemSet(&sSH, 0xff, VDS_SZ_SH);
    sSH.iEraseCounter = g_GcScane.iEraseCounter + 1;
    iResult = DRV_WriteFlash(iSecAddr, (UINT8 *)(&sSH), VDS_SZ_SH, &iWSize);
    if(ERR_SUCCESS == iResult && VDS_SZ_SH == iWSize)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1), TSTR("Erase finish: addr = 0x%x.",0x080001a0), iSecAddr);
        iRet = ERR_SUCCESS;
    }
    else
    {
        D( ( DL_VDSERROR, "Erase finish:write sector failed!err code = %d,addr = 0x%x,iWSize = 0x%x.", iResult, iSecAddr, iWSize));
        iRet = _ERR_VDS_GC_ERASE;
    }
    g_GcScane.psGcSect->iCurrentPBN = VDS_BLOCK_NONE;
    psPart->pGcingSect = NULL;
    g_psRemainRef = g_GcScane.psGcSect;
    //g_GcScane.psGcSect = NULL;
    //g_GcScane.psRemSect = NULL;

    return iRet;
}

UINT32 g_tst_erase_suspend_count = 0;
INT32 vds_GcExt(UINT8 iIsSuspend)
{

    INT32 iRet = _ERR_VDS_GC_FINISH;
    INT32 iResult;
    /*
    if(FALSE == g_GcScane.bIsActive)
    {
        DSM_HstSendEvent(0xaca000a0);
        return _ERR_VDS_GC_FINISH;
    }
    */
    while(VDS_GC_FINISH != g_GcScane.eStatus)
    {
        switch(g_GcScane.eStatus)
        {
            case VDS_GC_INIT:
                g_tst_erase_suspend_count = 0;
                // DSM_HstSendEvent(0xaca100f1);
                iResult = vds_GcInit(g_GcScane.psPart);
                if(ERR_SUCCESS == iResult)
                {
                    // DSM_HstSendEvent(0xaca200f2);
                    g_GcScane.eStatus = VDS_GC_COPY;
                    iRet = _ERR_VDS_GC_INIT;
                }
                else
                {
                    DSM_HstSendEvent(0xaca300fe);
                    DSM_HstSendEvent(0 - iResult);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FAILED;
                }
                break;
            case VDS_GC_COPY:
                // DSM_HstSendEvent(0xacb100f1);
                iResult = vds_GcCopy(g_GcScane.psPart, VDS_NR_PBD_PER_GROUP);
                if(VDS_NR_PBD_PER_GROUP == iResult)
                {
                    // DSM_HstSendEvent(0xacb20000|g_GcScane.iIndex);
                    g_GcScane.eStatus = VDS_GC_COPY;
                    iRet = _ERR_VDS_GC_COPY;
                }
                else if(iResult >= 0 && iResult < VDS_NR_PBD_PER_GROUP)
                {
                    // DSM_HstSendEvent(0xacb30000|g_GcScane.iIndex);
                    g_GcScane.eStatus = VDS_GC_SETMT;
                    iRet = _ERR_VDS_GC_COPY_COMPLETE;
                }
                else
                {
                    DSM_HstSendEvent(0xacb301fe);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FAILED;
                }
                break;

            case VDS_GC_SETMT:
                // DSM_HstSendEvent(0xacc100f1);
                iResult = vds_GcSetMt(g_GcScane.psPart);
                if(ERR_SUCCESS == iResult)
                {
                    // DSM_HstSendEvent(0xacc200f2);
                    g_GcScane.eStatus = VDS_GC_ERASE_BEGIN;
                    iRet = _ERR_VDS_GC_SETMT;
                }
                else
                {
                    DSM_HstSendEvent(0xacc302fe);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FAILED;
                }
                break;
            case VDS_GC_ERASE_BEGIN:                  // erase temp sector being.
                if(1 == iIsSuspend)
                {
                    iResult = vds_GcEraseBegin(g_GcScane.psPart);
                    if(ERR_SUCCESS == iResult)
                    {
                        // DSM_HstSendEvent(0xacd100f1);
                        g_GcScane.eStatus = VDS_GC_ERASE_FINISH;
                        iRet = _ERR_VDS_GC_ERASE;
                    }
                    else if(_ERR_VDS_GC_EREASE_SUSPEND == iResult)
                    {
                        // DSM_HstSendEvent(0xacd20000|g_tst_erase_suspend_count);
                        g_GcScane.eStatus = VDS_GC_ERASE_SUSPEND;
                        iRet = _ERR_VDS_GC_EREASE_SUSPEND;
                        g_tst_erase_suspend_count ++;
                    }
                    else if(_ERR_VDS_GC_EREASE_BUSY == iResult)
                    {
                        // DSM_HstSendEvent(0xacd20000|g_tst_erase_suspend_count);
                        g_GcScane.eStatus = VDS_GC_ERASE_BUSY;
                        iRet = _ERR_VDS_GC_EREASE_BUSY;
                        g_tst_erase_suspend_count ++;
                    }
                    else
                    {
                        DSM_HstSendEvent(0xacd303fe);
                        g_GcScane.eStatus = VDS_GC_FINISH;
                        iRet = _ERR_VDS_GC_FAILED;
                    }
                }
                else
                {
                    iResult = vds_GcErase(g_GcScane.psPart);
                    if(ERR_SUCCESS == iResult)
                    {
                        // DSM_HstSendEvent(0xacd100f1);
                        g_GcScane.eStatus = VDS_GC_ERASE_FINISH;
                        iRet = _ERR_VDS_GC_ERASE;
                    }
                    else
                    {
                        DSM_HstSendEvent(0xacd304fe);
                        g_GcScane.eStatus = VDS_GC_FINISH;
                        iRet = _ERR_VDS_GC_FAILED;
                    }
                }
                break;
            case VDS_GC_ERASE_BUSY:
                iResult = vds_GcEraseBegin2(g_GcScane.psPart);
                if(ERR_SUCCESS == iResult)
                {
                    // DSM_HstSendEvent(0xace10000|g_tst_erase_suspend_count);
                    g_GcScane.eStatus = VDS_GC_ERASE_FINISH;
                    iRet = _ERR_VDS_GC_ERASE;
                }
                else if(_ERR_VDS_GC_EREASE_SUSPEND == iResult)
                {
                    // DSM_HstSendEvent(0xace20000|g_tst_erase_suspend_count);
                    g_GcScane.eStatus = VDS_GC_ERASE_SUSPEND;
                    iRet = _ERR_VDS_GC_EREASE_SUSPEND;
                    g_tst_erase_suspend_count++;
                }
                else if(_ERR_VDS_GC_EREASE_BUSY == iResult)
                {
                    // DSM_HstSendEvent(0xacd20000|g_tst_erase_suspend_count);
                    g_GcScane.eStatus = VDS_GC_ERASE_BUSY;
                    iRet = _ERR_VDS_GC_EREASE_BUSY;
                    g_tst_erase_suspend_count ++;
                }
                else
                {
                    DSM_HstSendEvent(0xace305fe);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FAILED;
                }
                break;
            case VDS_GC_ERASE_SUSPEND:             // erase suspend.
                iResult = vds_GcEraseResume(g_GcScane.psPart);
                if(ERR_SUCCESS == iResult)
                {
                    // DSM_HstSendEvent(0xace10000|g_tst_erase_suspend_count);
                    g_GcScane.eStatus = VDS_GC_ERASE_FINISH;
                    iRet = _ERR_VDS_GC_ERASE;
                }
                else if(_ERR_VDS_GC_EREASE_SUSPEND == iResult)
                {
                    // DSM_HstSendEvent(0xace20000|g_tst_erase_suspend_count);
                    g_GcScane.eStatus = VDS_GC_ERASE_SUSPEND;
                    iRet = _ERR_VDS_GC_EREASE_SUSPEND;
                    g_tst_erase_suspend_count++;
                }
                else
                {
                    DSM_HstSendEvent(0xace306fe);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FAILED;
                }
                break;
            case VDS_GC_ERASE_FINISH:                // erase temp sector finish.

                iResult = vds_GcEraseFinish(g_GcScane.psPart);
                if(ERR_SUCCESS == iResult)
                {
                    // DSM_HstSendEvent(0xacf10000|g_tst_erase_suspend_count);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FINISH;
                }
                else
                {
                    DSM_HstSendEvent(0xacf307fe);
                    g_GcScane.eStatus = VDS_GC_FINISH;
                    iRet = _ERR_VDS_GC_FAILED;
                }
                break;
            case VDS_GC_FINISH:
            default:
                //DSM_HstSendEvent(0xacee00ef);
                iRet = _ERR_VDS_GC_FINISH;
                break;
        }
    }
    return  iRet;
}


VOID vds_GcActive(VDS_PARTITION *psPart)
{

    g_GcScane.pGcBuff = g_pGcBuff;
    CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(0), TSTR("vds_GcActive: active.",0x080001a1));
    g_GcScane.bIsActive = TRUE;
    g_GcScane.eStatus = g_GcScane.eStatus == VDS_GC_IDLE  ? VDS_GC_INIT : g_GcScane.eStatus;
    g_GcScane.psPart = psPart;

}

VOID vds_GcDeactive(VOID)
{
    g_GcScane.bIsActive = FALSE;
    g_GcScane.eStatus = VDS_GC_IDLE;
    g_GcScane.psGcSect = NULL;
    g_GcScane.psRemSect = NULL;
    g_GcScane.psPart = NULL;
    g_GcScane.iIndex = 0;
    CSW_TRACE(_CSW|TLEVEL(BASE_VDS_TS_ID)|TDB|TNB_ARG(0), TSTR("vds_GcDeactive: Dective.",0x080001a2));
}

BOOL vds_GcIsActive(VOID)
{
    return g_GcScane.bIsActive;
}

#endif

INT32 vds_Gc( VDS_PARTITION *psPart )
{

    INT32 iRet;
    INT32 iResult;

    // pointer to the gcing-sector reference.
    VDS_SECT_REF *psGcSect = NULL;
    // pointer to the remain-sector reference.
    VDS_SECT_REF *psRemSect = NULL;
    UINT16 iPbIndex = 0;
    UINT16 iCount = 0;
    UINT16 iPbCount = 0;
    UINT16 iReadPbn = 0;
    VDS_PBD   psPbdBuff[VDS_NR_PBD_PER_GROUP];
    VDS_PBD   psTmpPbdBuff[VDS_NR_PBD_PER_GROUP];
    VDS_PBD *pTmpPbd = NULL;
    VDS_PBD *pPbd = NULL;
    UINT8 *pPbBuff = NULL;
    UINT8 *pPb = NULL;
    UINT32 i;

    UINT32 iPbdAddr, iPbAddr;
    UINT32 iWritenLen;
    UINT16 iNrBlkScanned;

    D( ( DL_BRIEF, "vds_Gc begin."  ) );

    // 0 Check parameter.
    if ( NULL == psPart )
    {
        D( ( DL_VDSERROR, "In vds_Gc, \"psPart\" paramater null." ) );
        return _ERR_VDS_PARAM_ERROR;
    }

    if ( psPart->sPartInfo.iFlashSectorCount != psPart->iRefCount )
    {
        D( ( DL_VDSERROR, "In vds_Gc, partition not initialized." ) );
        iRet = ERR_VDS_UNINIT;
        goto Func_End;
    }

    // 1 Get GC sector.
    psGcSect = vds_GetGcingSector( psPart );
    if ( NULL == psGcSect )
    {
        D( ( DL_VDSERROR, "In vds_Gc, get gcing sector failed!" ) );
        iRet = _ERR_VDS_DIRTY_LIST_NULL;
        goto Func_End;
    }


    // 2 Get remain sector.
    psRemSect = vds_GetRemainSector( psPart );
    if ( NULL == psRemSect )
    {
        D( ( DL_VDSERROR, "In vds_Gc, get remain sector failed!" ) );
        iRet = _ERR_VDS_REM_SECT_NULL;
        goto Func_End;
    }

    // 3 Copy PB and PBD.
    iPbIndex = 0;
    pPbBuff = g_pGcBuff;
    pPb = pPbBuff;
    pPbd = psPbdBuff;
    iPbdAddr = VDS_ADDR_PBDT(psPart, psRemSect->iSN);
    iPbAddr = VDS_ADDR_PB(psPart, psRemSect->iSN);
    do
    {
        if(iPbIndex >= VDS_NR_PB_PER_SECT(psPart))
        {
            break;
        }
        // Read original PBD entries.
        iReadPbn  = iPbIndex + VDS_NR_PBD_PER_GROUP < VDS_NR_PB_PER_SECT(psPart) ? VDS_NR_PBD_PER_GROUP : (VDS_NR_PB_PER_SECT(psPart) - iPbIndex);
        iResult = vds_ReadPBDT(psPart, psGcSect->iSN, iPbIndex, iReadPbn, psTmpPbdBuff);
        if(iResult < 0)
        {
            D( ( DL_VDSERROR, "In vds_Gc, Read  PBDT failed,err code = %d.", iResult ) );
            iRet = _ERR_VDS_READ_FLASH_FAILED;
            goto Func_End;
        }
        // CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1),TSTR("vds GC:  iReadPbn = %d.",0x080001a3), iReadPbn);

        // Select the Valid PBD from original PBD Entries,add it to PBD buff.
        // Read the valid PB to PB buffer.
        // When Pbd buff is full,the PB buff is full too,Write the PB buff and PBD buffer to GCing sector.
        for(i = 0; i < iReadPbn; i ++)
        {
            pTmpPbd = psTmpPbdBuff + i;
            if(iPbIndex >= VDS_NR_PB_PER_SECT(psPart))
            {
                // Copying pbs and pbds complete.

                break;
            }

            if(VDS_STATUS_PB_VALID == pTmpPbd->iStatus)
            {
                // Copy a valid PBD to PBD buffer in original PBD entris.
                pTmpPbd->iVersion ++;
                DSM_MemCpy(pPbd, pTmpPbd, VDS_SZ_PBD);

                // Read a valid PB to PB buffer.
                iResult = vds_ReadPB(psPart, psGcSect->iSN, iPbIndex, pPb);
                if(ERR_SUCCESS != iResult)
                {
                    D( ( DL_VDSERROR, "In vds_Gc, Read  PB failed,err code = %d.", iResult ) );
                    iRet = _ERR_VDS_READ_FLASH_FAILED;
                    goto Func_End;
                }
                pPbd ++;
                pPb += VDS_SZ_PB(psPart);
                iCount ++;
                iPbCount ++;
            }

            iPbIndex ++;

            if(iReadPbn == iCount ||
                    (0 < iPbIndex && VDS_NR_PB_PER_SECT(psPart) == iPbIndex))
            {
                // Write PBD buffer to gcing sector.
                iResult = DRV_WriteFlash(iPbdAddr, (UINT8 *)psPbdBuff, iCount * VDS_SZ_PBD, &iWritenLen);
                if(iResult < 0 || iWritenLen != iCount * VDS_SZ_PBD)
                {
                    D( ( DL_VDSERROR, "In vds_Gc, write PBD failed,err code = %d.", iResult ) );
                    iRet = _ERR_VDS_WRITE_FLASH_FAILED;
                    goto Func_End;
                }

                // Write PB buffer to gcing sector.
                iResult = DRV_WriteFlash(iPbAddr, pPbBuff, iCount * VDS_SZ_PB(psPart), &iWritenLen);
                if(iResult < 0 || iWritenLen != iCount * VDS_SZ_PB(psPart))
                {
                    D( ( DL_VDSERROR, "In vds_Gc, write PB failed,err code = %d.", iResult ) );
                    iRet = _ERR_VDS_WRITE_FLASH_FAILED;
                    goto Func_End;
                }


                pPbd = psPbdBuff;
                pPb = pPbBuff;
                iPbdAddr += iCount * VDS_SZ_PBD;
                iPbAddr += iCount * VDS_SZ_PB(psPart);
                iCount = 0;
            }

        }
    }
    while(VDS_NR_PB_PER_SECT(psPart)  > iPbIndex);

    psRemSect->iCurrentPBN = iPbCount;
    // if all the valid-block on gcing-sector are copied,
    // or there is not valid-block on gcing-sector, finish sector GC .



    // 4 Change temp sector to current sector.

    if ( psRemSect->iCurrentPBN < ( UINT16 ) VDS_NR_PB_PER_SECT( psPart ) )
    {
        D( ( DL_BRIEF, "GC: change remain-sector(%d) as current-candidate-sector.", psRemSect->iSN ) );
        iRet = vds_ChangeSectStatus( psPart, psRemSect,
                                     VDS_STATUS_SECT_TEMP, VDS_STATUS_SECT_CURRENT );
        if ( ERR_SUCCESS != iRet )
        {
            iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
            goto Func_End;
        }
    }
    else
    {
        D( ( DL_VDSERROR, "GC: setor(#%d)'s iCurrentPBN(%d) is error.", psRemSect->iSN, psRemSect->iCurrentPBN));
        DSM_ASSERT(0, "GC: setor(#%d)'s iCurrentPBN(%d) is error.", psRemSect->iSN, psRemSect->iCurrentPBN);
        iRet = _ERR_VDS_CURRENT_POS_ERROR;
        goto Func_End;
    }

    // 5. Scan sector and set MT.
    D( ( DL_BRIEF, "GC: update map-table by scanning old remain-sector(%d)", psRemSect->iSN ) );
    // update map-table.
    iRet = vds_SetMTByScanSec( psPart, psRemSect->iSN, &iNrBlkScanned );
    if ( ERR_SUCCESS != iRet)
    {
        D( ( DL_VDSERROR, "GC:vds_SetMTByScanSec update map-table return failed!iRet = %d, psRemSect->iSN = 0x%x, "\
             "psGcSect->iSN = 0x%x\n", iRet, psRemSect->iSN, psGcSect->iSN ) );
        iRet = _ERR_VDS_UPDATE_MT_FAILED;
        goto Func_End;

    }

    // 6.Change gcing sector to remain sector.
    iRet = vds_ChangeSectStatus( psPart, psGcSect,
                                 VDS_STATUS_SECT_GCING, VDS_STATUS_SECT_REMAIN);
    if ( ERR_SUCCESS != iRet )
    {
        iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
        goto Func_End;
    }

    D( ( DL_BRIEF, "GC: change free-sector(%d) as remain-sector\n", g_psRemainRef->iSN ) );
    D( ( DL_BRIEF, "------------------- vds_Gc END ------------------\n" ) );


Func_End:
    return iRet;
}




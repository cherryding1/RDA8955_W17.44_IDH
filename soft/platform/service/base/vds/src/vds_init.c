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

extern UINT32 g_tstVdsErrCode;

INT32 VDS_GetFlashInfo( VDS_FLASH_INFO *psInfo )
{
    DRV_FLASH_DEV_INFO flash_dev_info = {0, 0, 0};
    INT32 iResult = 0;

    // Get the flash information.
    iResult = DRV_FlashRegionMap( &flash_dev_info);
    if(ERR_SUCCESS != iResult)
    {
        D( ( DL_VDSERROR, "DRV_FlashRegionMap failing,return:0x%x.", iResult));
        return ERR_VDS_CONFIG_ERROR;
    }

    // Set the output parameter.
    if(0 < flash_dev_info.sector_size)
    {
        psInfo->iFlashStartAddr = (UINT32)flash_dev_info.pBaseAddr;
        psInfo->iFlashSectorSize = flash_dev_info.sector_size;
        psInfo->iNrSector = (flash_dev_info.total_size / flash_dev_info.sector_size);
    }
    else
    {
        D( ( DL_VDSERROR, "iFlashSectorSize = 0."));
        return ERR_VDS_CONFIG_ERROR;
    }

    // Set the initialization flash is TRUE.
    psInfo->bHasInit = TRUE;

    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("flash info: start_addr: 0x%x, sector_size: 0x%x(%d K), sector_num: %d.",0x080001a4),
              psInfo->iFlashStartAddr,
              psInfo->iFlashSectorSize,
              psInfo->iFlashSectorSize / 1024,
              psInfo->iNrSector );


    return ERR_SUCCESS;
}


VDS_SECT_REF *vds_PopLastSecRef(VDS_SECT_REF **list)
{
    VDS_SECT_REF *psRef = NULL;
    VDS_SECT_REF *psOldRef = NULL;

    if(NULL == list)
    {
        return  NULL;
    }
    psRef = *list;

    while(psRef)
    {
        if(NULL == psRef->next)
        {
            if(psOldRef)
            {
                psOldRef->next = NULL;
            }
            else
            {
                *list = NULL;
            }
            break;
        }
        psOldRef = psRef;
        psRef = psRef->next;
    }
    return psRef;
}


VDS_SECT_REF *vds_PopFistSecRef(VDS_SECT_REF **list)
{
    VDS_SECT_REF *psRef = NULL;

    if(NULL == list)
    {
        return  NULL;
    }

    psRef = *list;

    if(psRef)
    {
        *list = psRef->next;
    }
    return psRef;
}


BOOL vds_CheckRemainSec(UINT32 iSecAddr, VDS_FLASH_INFO *pFlashInfo)
{
    INT iResult;
    UINT8 *pBuffer = NULL;
    UINT32 iLen;
    UINT32 iReadLen = 0;
    UINT32 i;
    VDS_SH *pSh;
    BOOL bRet = TRUE;

    iLen = pFlashInfo->iFlashSectorSize;
    pBuffer = DSM_MAlloc(iLen);

    iResult = DRV_ReadFlash(iSecAddr, pBuffer, iLen, &iReadLen);
    if(iResult == 0)
    {
        pSh = (VDS_SH *)pBuffer;

        if(pSh->iMagic != 0xffffffff ||
                pSh->iSectorStatus != VDS_STATUS_SECT_REMAIN ||
                pSh->iLayoutVer != 0xffffffff ||
                pSh->iFlashSecCount != 0xffffffff ||
                pSh->iFlashSectorSize != 0xffffffff ||
                pSh->iVBSize != 0xffffffff ||
                pSh->iReservedBlock != 0xffffffff ||
                pSh->iCRC != 0xffffffff)
        {
            bRet = FALSE;
        }
        else
        {
            for(i = 0; i < 16; i++)
            {
                if(pSh->acPartName[i] != 0xff)
                {
                    bRet = FALSE;
                    break;
                }
            }
            for(i = 0; i < 16; i++)
            {
                if(pSh->reserved[i] != 0xff)
                {
                    bRet = FALSE;
                    break;
                }
            }
        }

        if(bRet == TRUE)
        {
            for(i = sizeof(VDS_SH); i < pFlashInfo->iFlashSectorSize; i++)
            {
                if(pBuffer[i] != 0xff)
                {
                    bRet = FALSE;
                    break;
                }
            }
        }
    }
    else
    {
        bRet = FALSE;
    }

    if(pBuffer)
    {
        DSM_Free(pBuffer);
    }
    return bRet;
}

//-----------------------------------------------------------------------------------------------------
// vds_Init_Entry
// Get the partition config and scan the sector list to create the vds partiton table.
//------------------------------------------------------------------------------------------------------
INT32 vds_InitEntry(VOID)
{
    INT32 iResult;
    UINT32 iPartCount = 0;
    UINT32 iSectorCnt = 0;
    DSM_PARTITION_CONFIG *pPartConfigInfo = NULL;
    CONST DSM_CONFIG_T *pDsmConfig = NULL;
    UINT16 i, j;
    INT32 iRet = 0;
    UINT32 iSecAddr;
    UINT32 iReadedSize = 0;
    VDS_SH sSh;
    VDS_PARTITION *psCP = NULL;
    VDS_SECT_REF *psRef = NULL;
    VDS_SECT_REF *psUnformatRef = NULL;

    // Get flash information.
    iResult = VDS_GetFlashInfo( &g_sFlashInfo );
    if ( ERR_SUCCESS != iResult )
    {
        D( ( DL_VDSERROR, "vds init: get flash information failed err code = %d.", iResult ));
        g_tstVdsErrCode = 1001;
        iRet = _ERR_VDS_FLASH_INFO_ERROR;
        return iRet;
    }

    // Get the partition config information.
    pDsmConfig = tgt_GetDsmCfg();

    pPartConfigInfo = (DSM_PARTITION_CONFIG *)pDsmConfig->dsmPartitionInfo;

    // Compute the sum of flash sector  in paritition config table.
    for(i = 0; i < pDsmConfig->dsmPartitionNumber; i++)
    {
        if(pPartConfigInfo[i].eDevType != DSM_MEM_DEV_FLASH)
        {
            continue;
        }
        iSectorCnt += pPartConfigInfo[i].uSecCnt;
    }
    // Add a remain sector.
    iSectorCnt++;

    // Check the partition config.
    // request the sector number not less than the sum of config partitition sector number + a reserved sector,
    // else return partition config error.
    if(iSectorCnt > g_sFlashInfo.iNrSector)
    {
        g_tstVdsErrCode = 1002;
        return _ERR_VDS_CONFIG_ERROR;
    }

    // Create VDS partition table.
    iPartCount = 0; // vds partition table index.
    for(i = 0; i < pDsmConfig->dsmPartitionNumber; i++)
    {
        if(pPartConfigInfo[i].eDevType != DSM_MEM_DEV_FLASH)
        {
            continue;
        }
        psCP = g_VDS_partition_table + iPartCount;
        psCP->iRefCount = 0;

        DSM_StrCpy( psCP->sPartInfo.acPartName, pPartConfigInfo[i].szPartName);
        psCP->sPartInfo.iFlashSectorSize = g_sFlashInfo.iFlashSectorSize;
        psCP->sPartInfo.iVBSize = VDS_BLOCK_SIZE;
        psCP->sPartInfo.eDevType = pPartConfigInfo[i].eDevType;
        psCP->sPartInfo.eModuleId = pPartConfigInfo[i].eModuleId;
        psCP->sPartInfo.eCheckLevel = pPartConfigInfo[i].eCheckLevel;
        psCP->sPartInfo.iRsvBlkCount = pPartConfigInfo[i].uRsvBlkCnt;
        psCP->sPartInfo.iFlashSectorCount = pPartConfigInfo[i].uSecCnt;

        if(( VDS_NR_PB_PER_SECT(psCP) * (VDS_NR_SECT(psCP))) <= psCP->sPartInfo.iRsvBlkCount)
        {
            D( ( DL_VDSERROR, "vds_InitEntry: partition configure error. rsv_blk_cnt > all_blk_cnt." ));
            iRet = _ERR_VDS_CONFIG_ERROR;
            g_tstVdsErrCode = 1003;
            goto step0_failed;
        }

        psCP->psMT = DSM_MAlloc(VDS_SZ_MT( psCP ));
        if(NULL == psCP->psMT )
        {
            D( ( DL_VDSERROR, "VDS_PartitionInit: malloc error,psCP->psMT = NULL." ));
            iRet = _ERR_VDS_NO_MORE_MEM;
            DSM_ASSERT(0, "vds_InitEntry: 1.malloc(0x%x) failed.", (VDS_SZ_MT( psCP )));
            g_tstVdsErrCode = 1004;
            goto step0_failed;
        }
        DSM_MemSet(psCP->psMT, 0xff, VDS_SZ_MT( psCP ));
        psCP->pCurrentSect = NULL;
        psCP->pFreeList = NULL;
        psCP->pDirtyList = NULL;
        psCP->pGcingSect = NULL;
        psCP->iRefCount = 0;


        DSM_MemSet( psCP->psMT, VDS_VALUE_MT_INIT, VDS_SZ_MT( psCP ) );
        iPartCount++;
    }

    g_psSectRef = DSM_MAlloc((iSectorCnt) * sizeof( VDS_SECT_REF ));
    if ( NULL == g_psSectRef)
    {
        D( ( DL_VDSERROR, "vds init:sec_ref malloc failed size = %d.", VDS_NR_SECT( psCP ) * sizeof( VDS_SECT_REF ) ));
        iRet = _ERR_VDS_NO_MORE_MEM;
        DSM_ASSERT(0, "vds_InitEntry: 2.malloc(0x%x) failed.", ((iSectorCnt) * sizeof( VDS_SECT_REF )));
        g_tstVdsErrCode = 1005;
        goto  step0_failed;
    }

    DSM_MemSet(g_psSectRef, 0xff, (iSectorCnt ) * sizeof( VDS_SECT_REF ));

    // Scan the sector header table,to find the sectors for every partition.add remain sector to remain sector chain.
    for(i = 0; i < iSectorCnt; i++)
    {
        iSecAddr = VDS_SECT_INDEX_TO_ADDR( i , g_sFlashInfo);
        iResult = DRV_ReadFlash( iSecAddr, ( UINT8 * ) &sSh, VDS_SZ_SH, &iReadedSize );
        if ( ( iResult != ERR_SUCCESS ) || ( iReadedSize != VDS_SZ_SH ) )
        {
            D( ( DL_VDSERROR, "vds init:  call DRV_ReadFlash failed return. addr = 0x%x, iRet = %d,read size= %d, readed size  = %d.",
                 iSecAddr, iRet, VDS_SZ_SH, iReadedSize ));
            iRet =  _ERR_VDS_READ_FLASH_FAILED;
            g_tstVdsErrCode = 1006;
            goto step0_failed;
        }

        // Alloc a sec_ref.
        psRef = vds_AllocSecRef();
        if(NULL == psRef)
        {
            D( ( DL_VDSERROR, "vds init: alloc a  sec_ref failed,index = %d, addr = 0x%x.",
                 i,
                 iSecAddr
               ));
            iRet =  ERR_VDS_NO_MORE_SECT_REF;
            g_tstVdsErrCode = 1007;
            goto step0_failed;
        }

        // Set the field iSn.
        psRef->iSN = i;

        // Check status
        // remain.
        if(VDS_STATUS_SECT_REMAIN == sSh.iSectorStatus)
        {
            if(g_psRemainRef)
            {
                // Add this sector to unformat sector list.
                vds_AddSecRef( &psUnformatRef, psRef );
                /*
                 CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2),TSTR("vds init: remain sector more than one, add it to unformat list. index = %d,addr = 0x%x.",0x080001a5),
                                     i,
                                     iSecAddr
                                     );
                */
            }
            else
            {
                // Add this sector to remain list.
                if(vds_CheckRemainSec(iSecAddr, &g_sFlashInfo))
                {
                    vds_AddSecRef( &g_psRemainRef, psRef );
                }
                else
                {
                    vds_AddSecRef( &psUnformatRef, psRef );
                }
                /*
                CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("vds init: sector is remain, add it to remain ref. index = %d,addr = 0x%x.",0x080001a6),
                                    i,
                                    iSecAddr
                                    );
                */
            }
            continue;
        }

        // Check magic number.
        if ( VDS_MAGIC != sSh.iMagic && VDS_ERASING_MAGIC != sSh.iMagic )
        {
            // Add this sector to unformat sector list.
            vds_AddSecRef( &psUnformatRef, psRef );
            /*
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3), TSTR("vds init: sector with invalid magic number,add it to unformat list. index = %d,addr = 0x%x,magic number = 0x%x.",0x080001a7),
                i,
                iSecAddr,
                sSh.iMagic );
            */
            continue;
        }

        // Check version.
        if ( VDS_LAYOUT_VERSION != sSh.iLayoutVer )
        {
            // Add this sector to unformat sector list.
            vds_AddSecRef( &psUnformatRef, psRef );
            /*
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("vds init: sector layout ver is changed, add it to unformat list. index = %d, addr = 0x%x , sh LayoutVer = 0x%x, LayoutVer = 0x%x.",0x080001a8),
                                i,
                                iSecAddr,
                                sSh.iLayoutVer,
                                VDS_LAYOUT_VERSION );
            */
            continue;
        }

        // Check CRC.
        if(vds_GetShCrc(sSh) != sSh.iCRC)
        {
            // Add this sector to unformat sector list.
            vds_AddSecRef( &psUnformatRef, psRef );
            /*
            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("vds init: sector sh crc is error,add it to unformat list. index = %d, addr = 0x%x,iCrc = 0x%x, VDS_GET_HEADER_CRC(sSh) = 0x%x.",0x080001a9),
                i,
                iSecAddr,
                sSh.iMagic,
                vds_GetShCrc(sSh)
                );
            */
            continue;
        }

        // add this sector to vds partition table depend with partition name.
        for(j = 0; j < iPartCount; j++)
        {
            psCP = g_VDS_partition_table + j;
            // if name is matching and flash parameter is maching add it to that vds partition,else add it to remain sector list.
            if( 0 == DSM_StrCmp( psCP->sPartInfo.acPartName, sSh.acPartName))
            {
                // Check partition sector count .
                if(sSh.iFlashSecCount != psCP->sPartInfo.iFlashSectorCount)
                {
                    // Add this sector to unformat sector list.
                    vds_AddSecRef( &psUnformatRef, psRef );
                    /*
                    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("vds init: partition table sec_count  is not equal to config value, add it to unformat list. index = %d,addr = 0x%x,sec count = 0x%x,config value = 0x%x.",0x080001aa),
                        i,
                        iSecAddr,
                        sSh.iFlashSecCount,
                        psCP->sPartInfo.iFlashSectorCount);
                     */
                    break; // break the compareing loop.
                }

                // Check sector size.
                if(sSh.iFlashSectorSize != psCP->sPartInfo.iFlashSectorSize)
                {
                    // Add this sector to unformat sector list.
                    vds_AddSecRef( &psUnformatRef, psRef );
                    /*
                    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("vds init: partition table sec_size  is not equal to config value, add it to unformat list. index = %d,addr = 0x%x,sec_size = %d,config value = %d.",0x080001ab),
                        i,
                        iSecAddr,
                        sSh.iFlashSectorSize,
                        psCP->sPartInfo.iFlashSectorSize);
                    */
                    break; // break the compareing loop.
                }

                // Check VB size.
                if(sSh.iVBSize != psCP->sPartInfo.iVBSize)
                {
                    // Add this sector to unformat sector list.
                    vds_AddSecRef( &psUnformatRef, psRef );
                    /*
                    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("vds init: partition table vb_size  is not equal to config value, add it to unformat list. index = %d,addr = 0x%x,vb_size = %d,config value = %d.",0x080001ac),
                        i,
                        iSecAddr,
                        sSh.iVBSize,
                        psCP->sPartInfo.iVBSize);
                     */
                    break; // break the compareing loop.
                }

                // Check reserved block count.
                if(sSh.iReservedBlock != psCP->sPartInfo.iRsvBlkCount)
                {
                    // Add this sector to unformat sector list.
                    vds_AddSecRef( &psUnformatRef, psRef );
                    /*
                    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(4), TSTR("vds init: partition table reserved_block  is not equal to config value, add it to unformat list.. index = %d,addr = 0x%x,reserved_block = %d,config value = %d.",0x080001ad),
                        i,
                        iSecAddr,
                        sSh.iReservedBlock,
                        psCP->sPartInfo.iRsvBlkCount);
                     */
                    break; // break the compareing loop.
                }


                switch(sSh.iSectorStatus)
                {

                    case VDS_STATUS_SECT_DIRTY:
                        vds_AddSecRef( &(psCP->pDirtyList), psRef );
                        /*
                        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("vds init: sector is dirty, add it to [%s] dirty list. index = %d, addr = 0x%x.",0x080001ae),
                                            psCP->sPartInfo.acPartName,
                                            i,
                                            iSecAddr
                                            );
                        */
                        psCP->iRefCount ++;
                        break;
                    case VDS_STATUS_SECT_FREE: // free.

                        // Add this sector to free list.
                        vds_AddSecRef( &(psCP->pFreeList), psRef );
                        /*
                        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("vds init: sector is free, add it to [%s] free list. index = %d, addr = 0x%x.",0x080001af),
                        sCP->sPartInfo.acPartName,

                        SecAddr
                        ;
                        */
                        psCP->iRefCount ++;
                        break; // break the compareing loop.

                    case VDS_STATUS_SECT_GCING:
                        if ( psCP->pGcingSect )
                        {
                            // Add this sector to unformat sector list.
                            vds_AddSecRef( &psUnformatRef, psRef );
                            /*
                            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("vds init: partition [%s] gcing sector more than one, add it to unformat list. index = %d,addr = 0x%x.",0x080001b0),
                                                psCP->sPartInfo.acPartName,
                                                i,
                                                iSecAddr
                                                );
                            */
                        }
                        else
                        {
                            vds_AddSecRef( &(psCP->pGcingSect), psRef );
                            /*
                            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("vds init: sector is gcing, add it to [%s] gc ref. index = %d,addr = 0x%x.",0x080001b1),
                                                psCP->sPartInfo.acPartName,
                                                i,
                                                iSecAddr
                                                );
                             */
                        }
                        psCP->iRefCount ++;
                        break;

                    case VDS_STATUS_SECT_CURRENT:
                        if(psCP->pCurrentSect)
                        {
                            // Add this sector to unformat sector list.
                            vds_AddSecRef( &psUnformatRef, psRef );
                            /*
                            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1),TSTR("vds init: partition [%s] current sector more than one, add it to unformat list. index = %d,addr = 0x%x.",0x080001b2),
                                                psCP->sPartInfo.acPartName,
                                                i,
                                                iSecAddr
                                                );
                             */
                        }
                        else
                        {
                            vds_AddSecRef( &(psCP->pCurrentSect), psRef );
                            /*
                            CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(3)|TSMAP(1), TSTR("vds init: sector is current, add it to [%s] current ref. index = %d,addr = 0x%x.",0x080001b3),
                                                psCP->sPartInfo.acPartName,
                                                i,
                                                iSecAddr
                                                );
                             */
                        }
                        psCP->iRefCount ++;
                        break;
                    default:
                        // Add this sector to unformat sector list.
                        vds_AddSecRef( &psUnformatRef, psRef );
                        /*
                        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("vds init: unknow sector statues, add it to unformat list. index = %d,addr = 0x%x.",0x080001b4),
                                                i,
                                                iSecAddr
                                                );
                        */
                        break;
                }

                // break the compareing loop.
                break;
            }
        }
        // Unkown partition name
        // Add this sector to unformat sector list.
        if(j == iPartCount)
        {
            vds_AddSecRef( &psUnformatRef, psRef );
            D( ( DL_WARNING, "vds init: unknow sector partition name, add it to unformat list. index = %d,addr = 0x%x.",
                 i,
                 iSecAddr
               ));
        }
    }

    for(i = 0; i < iPartCount; i++)
    {
        psCP = g_VDS_partition_table + i;
        if(psCP->iRefCount > psCP->sPartInfo.iFlashSectorCount)
        {
            if(psCP->pGcingSect != NULL && psCP->pCurrentSect != NULL)
            {
                if(NULL == g_psRemainRef)
                {
                    iResult = vds_ChangeSectStatus( psCP, psCP->pGcingSect,
                                                    VDS_STATUS_SECT_GCING, VDS_STATUS_SECT_REMAIN);
                    if ( ERR_SUCCESS != iResult )
                    {
                        iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
                        g_tstVdsErrCode = 1008;
                        goto step0_failed;
                    }
                    else
                    {
                        psCP->iRefCount--;
                        break;
                    }
                }
                else
                {
                    iResult = vds_ChangeSectStatus( psCP, psCP->pGcingSect,
                                                    VDS_STATUS_SECT_GCING, VDS_STATUS_SECT_FREE);
                    if ( ERR_SUCCESS != iResult )
                    {
                        iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
                        g_tstVdsErrCode = 1009;
                        goto step0_failed;
                    }
                    else
                    {
                        psCP->iRefCount--;
                        break;
                    }
                }
            }
        }
    }


    // Clearup fragmentary partiton.

    for(i = 0; i < iPartCount; i++)
    {
        psCP = g_VDS_partition_table + i;

        if(psCP->iRefCount != psCP->sPartInfo.iFlashSectorCount)
        {
            // cleanup dirty sector.

            psRef = vds_PopLastSecRef(&(psCP->pDirtyList));
            while(psRef)
            {
                vds_AddSecRef(&psUnformatRef, psRef);
                psCP->iRefCount--;
                psRef = vds_PopLastSecRef(&(psCP->pDirtyList));
            }

            // cleanup current sector.
            psRef = vds_PopLastSecRef(&(psCP->pCurrentSect));
            while(psRef)
            {
                vds_AddSecRef(&psUnformatRef, psRef);
                psCP->iRefCount--;
                psRef = vds_PopLastSecRef(&(psCP->pCurrentSect));
            }
            // cleanup gc sector.
            psRef = vds_PopLastSecRef(&(psCP->pGcingSect));
            while(psRef)
            {
                vds_AddSecRef(&psUnformatRef, psRef);
                psCP->iRefCount--;
                psRef = vds_PopLastSecRef(&(psCP->pGcingSect));
            }

            // cleanup temp sector.
            psRef = vds_PopLastSecRef(&(psCP->pTempSect));
            while(psRef)
            {
                vds_AddSecRef(&psUnformatRef, psRef);
                psCP->iRefCount--;
                psRef = vds_PopLastSecRef(&(psCP->pTempSect));
            }

            // cleanup free sector.
            psRef = vds_PopLastSecRef(&(psCP->pFreeList));
            while(psRef)
            {
                vds_AddSecRef(&psUnformatRef, psRef);
                psCP->iRefCount--;
                psRef = vds_PopLastSecRef(&(psCP->pFreeList));
            }
        }

    }

    // Set remain sector.
    if(NULL == g_psRemainRef)
    {
        g_psRemainRef = vds_PopLastSecRef(&psUnformatRef);
        if(NULL != g_psRemainRef)
        {
            iSecAddr = VDS_SECT_INDEX_TO_ADDR(g_psRemainRef->iSN, g_sFlashInfo);
            iResult = vds_EraseSec(iSecAddr);
            if(ERR_SUCCESS != iResult)
            {
                iRet = _ERR_VDS_ERASE_FLASH_FAILED;
                g_tstVdsErrCode = 1009;
                goto step0_failed;
            }
            vds_RemoveSecRef( & psUnformatRef , g_psRemainRef);
            iResult = vds_SetSectStatus( g_psRemainRef->iSN, VDS_STATUS_SECT_REMAIN );
            if(ERR_SUCCESS != iResult)
            {
                iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
                g_tstVdsErrCode = 1010;
                goto step0_failed;
            }
        }
        else
        {
            iRet = _ERR_VDS_FREE_LIST_NULL;
            g_tstVdsErrCode = 1011;
            goto step0_failed;
        }
    }

    // Check the partition table,set the partition count value.
    g_vds_partition_count = 0;
    for(i = 0; i < iPartCount; i++)
    {
        psCP = g_VDS_partition_table + i;
        // create a new partition.
        while(psCP->iRefCount < psCP->sPartInfo.iFlashSectorCount)
        {
            // Get a unformat sector and set it to free sector
            // Add the free sector to free list.
            psRef = vds_PopLastSecRef(&psUnformatRef);
            if(NULL != psRef)
            {
                iSecAddr = VDS_SECT_INDEX_TO_ADDR(psRef->iSN, g_sFlashInfo);
                iResult = vds_EraseAndFormatSec(iSecAddr, psCP->sPartInfo);
                if(ERR_SUCCESS != iResult)
                {
                    iRet = _ERR_VDS_ERASE_FLASH_FAILED;
                    g_tstVdsErrCode = 1012;
                    goto step0_failed;
                }

                psRef->iCurrentPBN =  VDS_BLOCK_NONE;
                vds_AddSecRef(&(psCP->pFreeList), psRef);
                psCP->iRefCount ++;
            }
            else
            {
                // This case is impossibility.
            }
        };

        // create a current sector if current sector is null and gc sector is null.
        if(NULL == psCP->pCurrentSect && NULL == psCP->pGcingSect)
        {
            // Get a free sector and set it to current sector.
            if(NULL == psCP->pFreeList)
            {
                iResult = vds_Gc(psCP);
                if(ERR_SUCCESS != iResult)
                {
                    iRet = _ERR_VDS_SCAN_SECTOR_FAILED;
                    g_tstVdsErrCode = 1013;
                    goto step0_failed;
                }
            }
            else
            {
                psRef = vds_PopLastSecRef(&(psCP->pFreeList));
                if(NULL != psRef)
                {
                    psRef->iCurrentPBN =  VDS_BLOCK_NONE;
                    vds_AddSecRef(&(psCP->pCurrentSect), psRef);
                    iResult = vds_SetSectStatus( psCP->pCurrentSect->iSN, VDS_STATUS_SECT_CURRENT);
                    if(ERR_SUCCESS != iResult)
                    {
                        iRet = _ERR_VDS_CHANGE_STATUS_FAILED;
                        g_tstVdsErrCode = 1014;
                        goto step0_failed;
                    }
                }
                else
                {
                    iRet = _ERR_VDS_FREE_LIST_NULL;
                    g_tstVdsErrCode = 1015;
                    goto step0_failed;
                }
            }
        }

        // Scan dirty list,set MT.
        if(NULL != psCP->pDirtyList)
        {
            // scan sector and set MT.
            iResult = vds_ListScan( psCP, VDS_STATUS_SECT_DIRTY );
            if(ERR_SUCCESS != iResult)
            {
                iRet = _ERR_VDS_SCAN_SECTOR_FAILED;
                g_tstVdsErrCode = 1016;
                goto step0_failed;
            }
        }

        // Scan current sector,set MT.
        if(NULL != psCP->pCurrentSect)
        {
            // scan sector and set MT.
            iResult = vds_ListScan( psCP, VDS_STATUS_SECT_CURRENT);
            if(ERR_SUCCESS != iResult)
            {
                iRet = _ERR_VDS_SCAN_SECTOR_FAILED;
                g_tstVdsErrCode = 1017;
                goto step0_failed;
            }

        }

        // Process GC sector.
        if(NULL != psCP->pGcingSect)
        {
            //  scan sector and set MT.
            iResult = vds_ListScan( psCP, VDS_STATUS_SECT_GCING);
            if(ERR_SUCCESS != iResult)
            {
                iRet = _ERR_VDS_SCAN_SECTOR_FAILED;
                g_tstVdsErrCode = 1018;
                goto step0_failed;
            }
            iResult = vds_Gc(psCP);
            if(ERR_SUCCESS != iResult)
            {
                iRet = _ERR_VDS_SCAN_SECTOR_FAILED;
                g_tstVdsErrCode = 1019;
                goto step0_failed;
            }

        }
        g_vds_partition_count ++;
        //VDS_SemInit(i);
    }
    return ERR_SUCCESS;


step0_failed:
    for(i = 0; i < iPartCount; i++)
    {
        if(pPartConfigInfo[i].eDevType != DSM_MEM_DEV_FLASH)
        {
            continue;
        }
        psCP = g_VDS_partition_table + i;
        if(psCP->psMT)
        {
            DSM_Free(psCP->psMT);
        }
    }
    if(g_psSectRef)
    {
        DSM_Free(g_psSectRef);
    }
    return iRet;
}



// vds_EraseSec
// Function: Erase a sector.
// step1:get the erase_counter
// step2:erase the sector
// step3 write the erase_counter + 1 to sector.
INT32 vds_EraseSec(UINT32 iSecAddr)
{
    VDS_SH sSh;
    UINT32 iRSize = 0;
    UINT32 iWSize = 0;
    UINT32 iEraseCounter = 0;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;

    DSM_MemSet(&sSh, 0, VDS_SZ_SH);

    iResult = DRV_ReadFlash(iSecAddr, (UINT8 *)(&sSh), VDS_SZ_SH, &iRSize);
    if(ERR_SUCCESS == iResult && VDS_SZ_SH == iRSize)
    {

        iEraseCounter = sSh.iEraseCounter + 1;


        DSM_MemSet(&sSh, 0xff, VDS_SZ_SH);
        sSh.iEraseCounter = iEraseCounter;
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1),  TSTR("debug_flash:vds_EraseSec 1 call  DRV_EraseFlashSector(0x%x)",0x080001b5), iSecAddr);
        iResult = DRV_EraseFlashSector(iSecAddr);
        if(ERR_SUCCESS == iResult)
        {
            iResult = DRV_WriteFlash(iSecAddr, (UINT8 *)(&sSh), VDS_SZ_SH, &iWSize);
            if(ERR_SUCCESS == iResult && VDS_SZ_SH == iRSize)
            {
                iRet = ERR_SUCCESS;
            }
            else
            {
                D( ( DL_VDSERROR, "Erase sector:write sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
                iRet = ERR_VDS_FLASH_DRV_WRITE;
            }

        }

        else
        {
            D( ( DL_VDSERROR, "Erase sector:erase sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
            iRet = ERR_VDS_FLASH_DRV_ERASE;
        }
    }
    else
    {
        DSM_MemSet(&sSh, 0xff, VDS_SZ_SH);
        iEraseCounter = 1;
        sSh.iEraseCounter = iEraseCounter;
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1),  TSTR("debug_flash:vds_EraseSec 2 call  DRV_EraseFlashSector(0x%x)",0x080001b6), iSecAddr);
        iResult = DRV_EraseFlashSector(iSecAddr);
        if(ERR_SUCCESS == iResult)
        {
            iResult = DRV_WriteFlash(iSecAddr, (UINT8 *)(&sSh), VDS_SZ_SH, &iWSize);
            if(ERR_SUCCESS == iResult && VDS_SZ_SH == iWSize)
            {
                iRet = ERR_SUCCESS;
            }
            else
            {
                D( ( DL_VDSERROR, "Erase sector:write sector failed!err code = %d,addr = 0x%x.", iResult, iSecAddr));
                iRet = ERR_VDS_FLASH_DRV_WRITE;
            }
        }
    }

    CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2), TSTR("Erase sector: addr = 0x%x,erase counter = %d.",0x080001b7), iSecAddr, iEraseCounter);
    return iRet;
}


UINT32 vds_GetShCrc(VDS_SH sh)
{
    PVOID pData;
    UINT32 iLen;

    pData = (PVOID) (&(sh.iEraseCounter));
    iLen = (UINT32) & (sh.iCRC) - (UINT32) & (sh.iEraseCounter);
    return vds_CRC32(pData, iLen);
}

INT32 vds_FormatSec(UINT32 iSecAddr, VDS_PARTITION_INFO sPartInfo)
{

    VDS_SH sh;
    UINT32 wsize = 0;
    UINT32 rsize = 0;
    UINT32 erase_counter = 0;
    INT32 iRet;

    DSM_MemSet( &sh, 0xFF, VDS_SZ_SH );
    iRet = DRV_ReadFlash( iSecAddr, ( UINT8 * )&sh, VDS_SZ_SH, &rsize );
    if ( ( iRet != ERR_SUCCESS ) || ( rsize != VDS_SZ_SH ) )
    {
        D( ( DL_VDSERROR, "vds_format_sector: DRV_ReadFlash failed.iRet = %d, sect_addr = 0x%x,reading size = %d,readed size = %d.", iRet, iSecAddr, VDS_SZ_SH, rsize));
        // return _ERR_VDS_WRITE_FLASH_FAILED;
        erase_counter = sh.iEraseCounter;
    }


    DSM_MemSet( &sh, 0xFF, VDS_SZ_SH );
    sh.iLayoutVer = VDS_LAYOUT_VERSION;
    sh.iMagic = VDS_MAGIC;
    sh.iSectorStatus = VDS_STATUS_SECT_FREE;
    sh.iEraseCounter = erase_counter;
    sh.iFlashSectorSize = sPartInfo.iFlashSectorSize;
    sh.iVBSize = sPartInfo.iVBSize;
    sh.iFlashSecCount = sPartInfo.iFlashSectorCount;
    sh.iReservedBlock = sPartInfo.iRsvBlkCount;
    DSM_StrCpy( sh.acPartName, sPartInfo.acPartName);
    sh.iCRC = vds_GetShCrc(sh);

    iRet = DRV_WriteFlash( iSecAddr, ( UINT8 * )&sh, VDS_SZ_SH, &wsize );
    if ( ( iRet != ERR_SUCCESS ) || ( wsize != VDS_SZ_SH ) )
    {
        D( ( DL_VDSERROR, "vds_format_sector: DRV_WriteFlash failed.iRet = %d, sect_addr = 0x%x,writing size = %d,written size = %d.", iRet, iSecAddr, VDS_SZ_SH, wsize));
        return _ERR_VDS_WRITE_FLASH_FAILED;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(2)|TSMAP(2), TSTR("Format sector :sect_addr = 0x%x,partition name = %s",0x080001b8), iSecAddr, sh.acPartName);

    }
    return ERR_SUCCESS;
}

INT32 vds_EraseAndFormatSec(UINT32 iSecAddr, VDS_PARTITION_INFO sPartInfo)
{
    INT32 iResult;
    INT32 iRet;

    iResult = vds_EraseSec(iSecAddr);
    if(ERR_SUCCESS == iResult)
    {
        iResult = vds_FormatSec(iSecAddr, sPartInfo);
        if(ERR_SUCCESS == iResult)
        {
            iRet = ERR_SUCCESS;
        }

        else
        {
            iRet = iResult;
        }

    }

    else
    {
        iRet = iResult;
    }

    return iRet;
}





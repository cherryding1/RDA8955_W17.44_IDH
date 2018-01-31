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

#ifdef _FS_SIMULATOR_

#else
#include "hal_lps.h"
#include "hal_comregs.h"
#include "hal_fint.h"
#include <csw_csp.h>
#ifndef _T_UPGRADE_PROGRAMMER
#include "umss_m.h"
#endif //_T_UPGRADE_PROGRAMMER
#endif //_FS_SIMULATOR_
#include "dsm_cf.h"

#include "vds_local.h"
#include "vds_api.h"
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#include "memd_m.h"
#include "memd_cache.h"
#include "vds_cache.h"
#endif

extern VDS_PARTITION g_VDS_partition_table[];
extern UINT32 g_vds_partition_count;

// Device description table.this table include name,module id,handle,state,device type and point of option function.
DSM_DEV_DESCRIP *g_pDevDescrip = NULL;

// Device description table recodes number.The arry 0 is reserved,so the value equal to (recode number + 1)
UINT32 g_iDevDescripCount = 0;

// Device simple information table.
DSM_DEV_INFO *g_pDsmDevInfo = NULL;

// Device simple information table record number.
UINT32 g_iDsmDevCount = 0;

// The semaphore of  DSM device  access.
HANDLE g_hDevSem = (HANDLE)NULL;


VOID DSM_SemInit(VOID);
VOID DSM_SemDown(VOID);
VOID DSM_SemUp(VOID);

/**************************************************************************************/
// Function: This function init the device table.
//   Firstly, get the partition config, get the flash partition count and t-flash partition count throught scan the partition config table;
//   Secondly, add the flash partiton into the device talbe, add get the device handle for every flash partition;
//   Thirdly,  add the t-flash partiton into the device talbe, add get the device handle for every t-flash partition;
// Parameter:
// None
// Return value:
// TODO
// Node: the device table uint 0 is reserved. when access the device with dev_no, the zero value is invalid device No.
/***************************************************************************************/
INT32 DSM_DevInit( VOID )
{
    UINT32 iPartCount = 0;
    UINT32 iDeviceCount = 0;
    DSM_PARTITION_CONFIG *pPartConfigInfo = NULL;
    CONST DSM_CONFIG_T *pDsmConfig = NULL;
    UINT32 i, n;
    INT32 dev_handle;
    INT32 iRet;


    // Init the semaphore
    DSM_SemInit();

    // Get the partition config information.
    pDsmConfig = tgt_GetDsmCfg();

    pPartConfigInfo = (DSM_PARTITION_CONFIG *)pDsmConfig->dsmPartitionInfo;
    iPartCount = pDsmConfig->dsmPartitionNumber;



    // Malloc for device table.
    iDeviceCount = iPartCount + 1;
    g_pDevDescrip = DSM_MAlloc(SIZEOF(DSM_DEV_DESCRIP) * iDeviceCount);
    if(NULL == g_pDevDescrip)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(0), TSTR("In DSM_DevInit,malloc failed.\n",0x08000170));
        DSM_ASSERT(0, "DSM_DevInit:1. malloc(0x%x) failed.", (SIZEOF(DSM_DEV_DESCRIP)*iDeviceCount));
        return ERR_DRV_NO_MORE_MEM;
    }
    DSM_MemSet(g_pDevDescrip, 0, SIZEOF(DSM_DEV_DESCRIP)*iDeviceCount);

    // step1:Initialize the device descrip table.
    // The array 0 is reseved,when dev_no is zero, we consider it is invalid device no,so set the n = 1,
    n = 1;
    for ( i = 0; i < iPartCount; i++ )
    {
        DSM_StrCpy(g_pDevDescrip[n].dev_name, pPartConfigInfo[i].szPartName);
        g_pDevDescrip[n].dev_handle = -1;
        g_pDevDescrip[n].module_id = pPartConfigInfo[i].eModuleId;
        g_pDevDescrip[n].dev_state = DEV_STATE_CLOSE;
        g_pDevDescrip[n].dev_type = pPartConfigInfo[i].eDevType;

        // flash partition. When open successed,this partition will been add to the device descrip table.
        if(pPartConfigInfo[i].eDevType == DSM_MEM_DEV_FLASH)
        {
            g_pDevDescrip[n].open = (PF_DEV_OPEN)VDS_Open;
            g_pDevDescrip[n].close = (PF_DEV_ClOSE)VDS_Close;
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)VDS_CacheReadBlock;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)VDS_CacheWriteBlock;
            g_pDevDescrip[n].brevert = (PF_DEV_BLOCKREVERT)VDS_CacheRevertBlock;
#else
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)VDS_ReadBlock;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)VDS_WriteBlock;
            g_pDevDescrip[n].brevert = (PF_DEV_BLOCKREVERT)VDS_RevertBlock;
#endif
            g_pDevDescrip[n].get_dev_info = (PF_DEV_GETDEVINFO)VDS_GetPartitionInfo;
            g_pDevDescrip[n].set_rcache_size = (PF_DEV_SET_CACHE_SIZE)VDS_SetRCacheSize;
            g_pDevDescrip[n].get_rcache_size = (PF_DEV_GET_CACHE_SIZE)VDS_GetRCacheSize;
            g_pDevDescrip[n].set_wcache_size = (PF_DEV_SET_CACHE_SIZE)VDS_SetWCacheSize;
            g_pDevDescrip[n].get_wcache_size = (PF_DEV_GET_CACHE_SIZE)VDS_GetWCacheSize;
            g_pDevDescrip[n].active = (PF_DEV_ACTIVE)VDS_Active;
            g_pDevDescrip[n].deactive = (PF_DEV_ACTIVE)VDS_Deactive;
            g_pDevDescrip[n].flush = (PF_DEV_ACTIVE)VDS_Flush;
            /*
            // Open the device, get the device handle.
            iRet = (*g_pDevDescrip[n].open)( ((UINT8*)g_pDevDescrip[n].dev_name), &dev_handle );
            if(ERR_SUCCESS == iRet)
            {
                CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Open device[%s] successfully with handle[%d] returned.",0x08000171),
                g_pDevDescrip[n].dev_name, dev_handle );
                g_pDevDescrip[n].dev_state = DEV_STATE_OPEN;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Open device[%s] failed with errcode[%d].",0x08000172),
                g_pDevDescrip[n].dev_name, iRet );
                continue;
            }
            g_pDevDescrip[n].dev_handle = dev_handle;
            */
#ifndef _T_UPGRADE_PROGRAMMER
#if (CHIP_HAS_USB == 1)
            if(DSM_StrCaselessCmp(pPartConfigInfo[i].szPartName, "VDS1") == 0)
            {
                UINT32 iBlkNr = 0;
                UINT32 iBlkSz = 0;
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
                init_uFlash_Func (VDS_CacheWriteBlock, VDS_CacheReadBlock);
#else
#ifndef _FS_SIMULATOR_
                init_uFlash_Func (VDS_WriteBlock, VDS_ReadBlock);
#endif
#endif // USER_DATA_CACHE_SUPPORT
                VDS_GetPartitionInfo(i, &iBlkNr, &iBlkSz);
#ifndef _FS_SIMULATOR_
                init_uFlash_Param(i, iBlkNr, iBlkSz);
#endif
            }
#endif // CHIP_HAS_USB
#endif  //_T_UPGRADE_PROGRAMMER

        }
        // t-flash partiton. Don't check the option of opening,and have no use for device handle.
        else
        {
#ifndef CHIP_HAS_AP
            g_pDevDescrip[n].open = (PF_DEV_OPEN)DRV_OpenTFlash;
            g_pDevDescrip[n].close = (PF_DEV_ClOSE)DRV_CloseTFlash;
            g_pDevDescrip[n].bread = (PF_DEV_BLOCKREAD)DRV_ReadTFlash;
            g_pDevDescrip[n].bwrite = (PF_DEV_BLOCKWRITE)DRV_WriteTFlash;
            g_pDevDescrip[n].get_dev_info = (PF_DEV_GETDEVINFO)DRV_GetDevInfoTFlash;
            g_pDevDescrip[n].set_rcache_size = (PF_DEV_SET_CACHE_SIZE)DRV_SetRCacheSizeTFlash;
            g_pDevDescrip[n].get_rcache_size = (PF_DEV_GET_CACHE_SIZE)DRV_GetRCacheSizeTFlash;
            g_pDevDescrip[n].set_wcache_size = (PF_DEV_SET_CACHE_SIZE)DRV_SetWCacheSizeTFlash;
            g_pDevDescrip[n].get_wcache_size = (PF_DEV_GET_CACHE_SIZE)DRV_GetWCacheSizeTFlash;
            g_pDevDescrip[n].active = (PF_DEV_ACTIVE)DRV_ActiveTFlash;
            g_pDevDescrip[n].deactive = (PF_DEV_ACTIVE)DRV_DeactiveTFlash;
            g_pDevDescrip[n].flush = (PF_DEV_ACTIVE)DRV_FlushTFlash;
#endif
        }

        // Open the device, get the device handle.
        iRet = (*g_pDevDescrip[n].open)( ((UINT8 *)g_pDevDescrip[n].dev_name), (UINT32 *)&dev_handle );
        if(ERR_SUCCESS == iRet)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Open device[%s] successfully with handle[%d] returned.",0x08000173),
                      g_pDevDescrip[n].dev_name, dev_handle );
            g_pDevDescrip[n].dev_state = DEV_STATE_OPEN;
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Open device[%s] failed with errcode[%d].",0x08000174),
                      g_pDevDescrip[n].dev_name, iRet );
            continue;
        }
        g_pDevDescrip[n].dev_handle = dev_handle;
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Device[%s] is added to Device dscription,DevNo = %d.\n",0x08000175), g_pDevDescrip[n].dev_name, n);
        n ++;

    }

    // Todo step2 initialize the table for others partition.

    // Step3: init dsm device information table.
    g_iDevDescripCount = n;
    g_iDsmDevCount = n - 1;

    if(g_iDsmDevCount > 0)
    {
        g_pDsmDevInfo = DSM_MAlloc(SIZEOF(DSM_DEV_INFO) * g_iDsmDevCount);
        if(NULL == g_pDsmDevInfo)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(0), TSTR("In DSM_DevInit,malloc failed.",0x08000176));
            DSM_ASSERT(0, "DSM_DevInit: 2.malloc(0x%x) failed.", (SIZEOF(DSM_DEV_INFO)*g_iDsmDevCount));
            return ERR_DRV_NO_MORE_MEM;
        }
        DSM_MemSet(g_pDsmDevInfo, 0, SIZEOF(DSM_DEV_INFO)*g_iDsmDevCount);
        for(i = 1; i < g_iDevDescripCount; i++)
        {
            g_pDsmDevInfo[i - 1].dev_no  = i;
            DSM_StrCpy( g_pDsmDevInfo[i - 1].dev_name, g_pDevDescrip[i].dev_name);
            g_pDsmDevInfo[i - 1].dev_type = g_pDevDescrip[i].dev_type;
            g_pDsmDevInfo[i - 1].module_id = g_pDevDescrip[i].module_id;
        }
    }
    return ERR_SUCCESS;
}



UINT32 DSM_DevName2DevNo( PCSTR pszDevName )
{
    UINT32 i;

    if ( !pszDevName )
    {
        return INVALID_DEVICE_NUMBER;
    }

    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3)|TSMAP(2)|TSMAP(3), TSTR("g_pDevDescrip[%d].dev_name = %s,input device name= %s\n",0x08000177), i,g_pDevDescrip[i].dev_name,pszDevName );
        if ( g_pDevDescrip[i].dev_name && 0 == DSM_StrCaselessCmp( pszDevName, g_pDevDescrip[i].dev_name ) )
        {
            break;
        }
    }

    if ( i == g_iDevDescripCount )
    {
        return INVALID_DEVICE_NUMBER;
    }

    return i;
}


INT32 DSM_DevNr2Name( UINT32 uDevNo, PSTR pszDevName )
{
    if ( INVALID_DEVICE_NUMBER == uDevNo || uDevNo >= g_iDevDescripCount || !pszDevName )
    {
        return ERR_DRV_INVALID_DEVNO;
    }

    DSM_StrCpy( pszDevName, g_pDevDescrip[uDevNo].dev_name );

    return ERR_SUCCESS;
}

INT32 DSM_GetDevSpaceSize(PSTR pszDevName, UINT32 *pBlkCount, UINT32 *pBlkSize)
{
    UINT32 i;
    INT32 iResult;
    UINT32 uBlkCount = 0;
    UINT32 uBlkSize = 0;

    if ( !pszDevName )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }

    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3)|TSMAP(2)|TSMAP(3), TSTR("g_pDevDescrip[%d].dev_name = %s,input device name= %s\n",0x08000178), i,g_pDevDescrip[i].dev_name,pszDevName );
        if ( g_pDevDescrip[i].dev_name && 0 == DSM_StrCaselessCmp( pszDevName, g_pDevDescrip[i].dev_name ) )
        {
            iResult = g_pDevDescrip[i].get_dev_info(g_pDevDescrip[i].dev_handle, &uBlkCount, &uBlkSize);
            if(ERR_SUCCESS != iResult)
            {
                return ERR_DRV_DEV_NOT_INIT;
            }
            else
            {
                *pBlkCount = uBlkCount;
                *pBlkSize = uBlkSize;
                break;
            }
        }
    }

    if ( i == g_iDevDescripCount )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }

    return ERR_SUCCESS;
}


UINT32 DSM_GetDevHandle( PCSTR pszDevName )
{
    UINT32 i;

    if ( !pszDevName )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }

    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3)|TSMAP(2)|TSMAP(3), TSTR("g_pDevDescrip[%d].dev_name = %s,input device name= %s\n",0x08000179), i,g_pDevDescrip[i].dev_name,pszDevName );
        if ( g_pDevDescrip[i].dev_name && 0 == DSM_StrCaselessCmp( pszDevName, g_pDevDescrip[i].dev_name ) )
        {
            break;
        }
    }

    if ( i == g_iDevDescripCount )
    {
        return ERR_DRV_INVALID_DEVNAME;
    }
    else
    {
        return g_pDevDescrip[i].dev_handle;
    }
}



INT32 DSM_GetDevType(UINT32 uDevNo)
{
    if(uDevNo == INVALID_DEVICE_NUMBER || (UINT32)uDevNo >= g_iDevDescripCount)
    {
        return ERR_DRV_INVALID_DEVNO;
    }

    return  g_pDevDescrip[uDevNo].dev_type;


}

INT32 DSM_GetFsRootDevName(PSTR pszDevName )
{
    UINT32 i;

    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
        if ( g_pDevDescrip[i].module_id == DSM_MODULE_FS_ROOT)
        {
            break;
        }
    }
    if ( i == g_iDevDescripCount )
    {
        return ERR_DRV_GET_DEV_FAILED;
    }

    DSM_StrCpy(pszDevName, g_pDevDescrip[i].dev_name);
    return ERR_SUCCESS;
}


INT32 DSM_GetDevNrOnModuleId(DSM_MODULE_ID eModuleId)
{
    UINT32 i;

    for ( i = 1; i < g_iDevDescripCount; i++ )
    {
        if ( g_pDevDescrip[i].module_id == eModuleId)
        {
            break;
        }
    }

    if ( i == g_iDevDescripCount )
    {
        return INVALID_DEVICE_NUMBER;
    }
    return i;

}

INT32 DSM_GetDevCount(VOID)
{
    return g_iDsmDevCount;
}


DSM_MODULE_ID DSM_GetDevModuleId(UINT32 uDevNo)
{
    if(uDevNo == INVALID_DEVICE_NUMBER || (UINT32)uDevNo >= g_iDevDescripCount)
    {
        return ERR_DRV_INVALID_DEVNO;
    }
    return g_pDevDescrip[uDevNo].module_id;
}

//Function
//This function to get the DSM device information.
//    INT32 DSM_GetDeviceInfo(UINT32* pDeviceCount,DSM_DEV_INFO** ppDevInfo);
//Parameter
//   pDeviceCount [out]: Output the count of device.
//   ppDevInfo[out]: Output a array for DSM_DEV_INFO structure.
//Return value
//   Upon successful completion, 0 shall be returned. Otherwise, Error Code is returned. The following error codes may be returned.
//   ERR_DRV_INVALID_PARAMETER: The point of parameter is NULL.
INT32 DSM_GetDeviceInfo(UINT32 *pDeviceCount, DSM_DEV_INFO **ppDevInfo)
{
    // Check input parameters.
    if(NULL == pDeviceCount ||
            NULL == ppDevInfo)
    {
        return ERR_DRV_INVALID_PARA;
    }

    // Device not initialized.
    if(NULL == g_pDsmDevInfo ||
            0 == g_iDsmDevCount)
    {
        return ERR_DRV_DEV_NOT_INIT;
    }

    // output the dsm device information
    *ppDevInfo = g_pDsmDevInfo;
    *pDeviceCount  = g_iDsmDevCount;
    return ERR_SUCCESS;

}


///////////////////////////////////////////////////////////////////////
//  Read the partition on byte.
// If read to the end of partition, output the real reading size.
/////////////////////////////////////////////////////////////////////
INT32 DSM_Read( UINT32 uDevNr, UINT32 ulAddrOffset, UINT8 *pBuffer,
                UINT32 ulBytesToRead, UINT32 *pBytesRead )
{
    INT32 iResult;
    UINT32 ulModuleSize;
    UINT32 ulBlockOffset;
    UINT32 uBlockNO = 0;
    UINT32 uNrBlock = 0;
    UINT32 uBlockSize = 0;

    UINT8 pBlockBuffer[DSM_DEFAULT_SECSIZE] = {0,};
    UINT32 ulPos;
    UINT32 ulLenRead = 0;


    // Check the input parameter.
    if ( INVALID_DEVICE_NUMBER == uDevNr ||
            uDevNr >= g_iDevDescripCount ||
            NULL == pBuffer ||
            NULL == pBytesRead)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("Invalid parameter,dev_no = %d,pbuffer = 0x%x,pbytesread = 0x%x.\n",0x0800017a),
                  uDevNr,
                  pBuffer,
                  pBytesRead
                 );
        return ERR_DRV_INVALID_PARA;
    }

    DSM_SemDown();

    // Get the block count and block size.
    iResult = DRV_GET_DEV_INFO( uDevNr, &uNrBlock, &uBlockSize);
    if(ERR_SUCCESS != iResult)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("get dev info failed,dev_nr = %d,err code = %d.\n",0x0800017b),
                  uDevNr,
                  iResult
                 );
        DSM_SemUp();
        return ERR_DRV_GET_DEV_FAILED;
    }

    // Judge the address offset if exceed the moudule size.
    ulModuleSize = uBlockSize * uNrBlock;
    if ( ulAddrOffset >= ulModuleSize )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("block number to read overflow total block nuber. to read = 0x%x,total = 0x%x.\n",0x0800017c),
                  ulAddrOffset,
                  ulModuleSize
                 );
        DSM_SemUp();
        return ERR_DRV_INVALID_PARA;
    }

    // Figure out the spare size.
    if ( ulAddrOffset + ulBytesToRead > ulModuleSize )
    {
        ulBytesToRead = ulModuleSize - ulAddrOffset;
    }

    ulPos = ulAddrOffset;
    ulBlockOffset = ulPos % uBlockSize;
    uBlockNO = ulPos / uBlockSize;

    // Read the first block.
    if ( ulBlockOffset != 0 )
    {

        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_readblock(first) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x0800017d),
                      g_pDevDescrip[uDevNr].dev_handle,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        if ( ulBlockOffset + ulBytesToRead <= uBlockSize )
        {
            DSM_MemCpy( pBuffer + ulLenRead, pBlockBuffer + ulBlockOffset, ulBytesToRead );
            ulPos += ulBytesToRead;
            ulLenRead += ulBytesToRead;
        }
        else
        {
            DSM_MemCpy( pBuffer + ulLenRead, pBlockBuffer + ulBlockOffset, uBlockSize - ulBlockOffset );
            ulPos += ( uBlockSize - ulBlockOffset );
            ulLenRead += ( uBlockSize - ulBlockOffset );
        }
    }

    // Read middle block.
    while ( ulLenRead + uBlockSize <= ulBytesToRead )
    {
        uBlockNO = ulPos / uBlockSize;
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBuffer + ulLenRead );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_readblock(next) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x0800017e),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        ulPos += uBlockSize;
        ulLenRead += uBlockSize;
    }

    // Read the last block.
    if ( ulLenRead < ulBytesToRead )
    {
        uBlockNO =  ulPos / uBlockSize;
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_readblock(last) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x0800017f),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        DSM_MemCpy( pBuffer + ulLenRead, pBlockBuffer, ulBytesToRead - ulLenRead );
        ulLenRead += ( ulBytesToRead - ulLenRead );
    }

    *pBytesRead = ulLenRead;
    DSM_SemUp();
    return ERR_SUCCESS;

}



///////////////////////////////////////////////////////////////////////
//  Write the partition on byte.
// If write to the end of partition, output the real writing size.
/////////////////////////////////////////////////////////////////////
INT32 DSM_Write( UINT32 uDevNr, UINT32 ulAddrOffset, CONST UINT8 *pBuffer,
                 UINT32 ulBytesToWrite, UINT32 *pBytesWritten )
{
    INT32 iResult;
    UINT32 ulModuleSize;
    UINT32 ulBlockOffset;
    UINT32 uBlockNO;
    UINT32 uNrBlock = 0;
    UINT32 uBlockSize = 0;
    UINT8 pBlockBuffer[DSM_DEFAULT_SECSIZE] = {0,};
    UINT32 ulPos;
    UINT32 ulLenWriten = 0;

    // Check the input parameter.
    if ( INVALID_DEVICE_NUMBER == uDevNr ||
            uDevNr >= g_iDevDescripCount ||
            NULL == pBuffer ||
            NULL == pBytesWritten)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("Invalid parameter,dev_no = %d,pbuffer = 0x%x,pbyteswritten = 0x%x.\n",0x08000180),
                  uDevNr,
                  pBuffer,
                  pBytesWritten
                 );
        return ERR_DRV_INVALID_PARA;
    }

    DSM_SemDown();

    // Get the block count and block size.
    iResult = DRV_GET_DEV_INFO( uDevNr, &uNrBlock, &uBlockSize);
    if(ERR_SUCCESS != iResult)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("get dev info failed,dev_nr = %d,err code = %d.\n",0x08000181),
                  uDevNr,
                  iResult
                 );
        DSM_SemUp();
        return ERR_DRV_GET_DEV_FAILED;
    }

    // Judge the address offset if exceed the moudule size.
    ulModuleSize = uBlockSize * uNrBlock;
    if ( ulAddrOffset >= ulModuleSize )
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("block number to write overflow total block nuber. to write = 0x%x,total = 0x%x.\n",0x08000182),
                  ulAddrOffset,
                  ulModuleSize
                 );
        DSM_SemUp();
        return ERR_DRV_INVALID_PARA;
    }

    // Figure out the spare size.
    if ( ulAddrOffset + ulBytesToWrite > ulModuleSize )
    {
        ulBytesToWrite = ulModuleSize - ulAddrOffset;
    }

    ulPos = ulAddrOffset;
    ulBlockOffset = ulPos % uBlockSize;
    uBlockNO = ulPos / uBlockSize ;

    // Write the first block.
    if ( ulBlockOffset != 0 )
    {
        iResult = DRV_BLOCK_READ(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_readblock(write before) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x08000183),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        if ( ulBlockOffset + ulBytesToWrite <= uBlockSize )
        {
            DSM_MemCpy( pBlockBuffer + ulBlockOffset, pBuffer + ulLenWriten, ulBytesToWrite );
            ulPos += ulBytesToWrite;
            ulLenWriten += ulBytesToWrite;
        }
        else
        {
            DSM_MemCpy( pBlockBuffer + ulBlockOffset, pBuffer + ulLenWriten, uBlockSize - ulBlockOffset );
            ulPos += ( uBlockSize - ulBlockOffset );
            ulLenWriten += ( uBlockSize - ulBlockOffset );
        }

        iResult = DRV_BLOCK_WRITE(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_writeblock(first) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x08000184),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_WRITE_FAILED;
        }
    }

    // Write middle block.
    while ( ulLenWriten + uBlockSize <= ulBytesToWrite )
    {
        uBlockNO =  ulPos / uBlockSize;
        DSM_MemCpy(pBlockBuffer, pBuffer + ulLenWriten, DSM_DEFAULT_SECSIZE);
        iResult = DRV_BLOCK_WRITE(uDevNr, uBlockNO, ( UINT8 * )  pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_writeblock(next) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x08000185),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_WRITE_FAILED;
        }

        ulPos += uBlockSize;
        ulLenWriten += uBlockSize;
    }

    // Write the last block.
    if ( ulLenWriten < ulBytesToWrite )
    {
        uBlockNO = ulPos / uBlockSize ;
        iResult = DRV_BLOCK_READ( uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_readblock(last befor) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x08000186),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_READ_FAILED;
        }

        DSM_MemCpy( pBlockBuffer, pBuffer + ulLenWriten, ulBytesToWrite - ulLenWriten );
        iResult = DRV_BLOCK_WRITE(uDevNr, uBlockNO, pBlockBuffer );
        if ( ERR_SUCCESS != iResult )
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("vds_writeblock(last) failed,dev_handle = %d,block_no= 0x%x,err code = %d.\n",0x08000187),
                      uDevNr,
                      uBlockNO,
                      iResult
                     );
            DSM_SemUp();
            return ERR_DRV_WRITE_FAILED;
        }
        ulLenWriten += ( ulBytesToWrite - ulLenWriten );
    }

    *pBytesWritten = ulLenWriten;
    DSM_SemUp();
    return ERR_SUCCESS;

}


//-------------------------------------------------------------------------------------------------
//  Clear the data on flash user region.
//  Must reset the system after called this function.
//  Return value:
//     ERR_SUCCESS:                      Clear data succed.
//     ERR_DRV_GET_DEV_FAILED:   Get the device information failed.
//     ERR_DRV_ERASE_FAILED:       Erase flash sector failed.
//
//------------------------------------------------------------------------------------------------
INT32 DSM_UserDataClear(VOID)
{
    // UINT32 cri_status;

    // hal_FintIrqSetMask(FALSE);
    // hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_0));
    // hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_1, hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_1));
    // cri_status = hal_EnterCriticalSection();

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    VDS_CacheSetEraseAll();
#endif
    VDS_SetAllSectorsEraseFlag();
    //DM_Reset();
    return ERR_SUCCESS;
}


VOID DSM_SemInit(VOID)
{
    g_hDevSem = sxr_NewSemaphore(1);
    if((HANDLE)NULL == g_hDevSem)
    {
        D( ( DL_WARNING, "VDS_ModuleSemInit failed."));
    }
}


VOID DSM_SemDown(VOID)
{
    if((HANDLE)NULL != g_hDevSem)
    {
        sxr_TakeSemaphore(g_hDevSem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleDown failed."));
    }

}


VOID DSM_SemUp(VOID)
{

    if((HANDLE)NULL != g_hDevSem)
    {
        sxr_ReleaseSemaphore(g_hDevSem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleUp failed."));
    }
}



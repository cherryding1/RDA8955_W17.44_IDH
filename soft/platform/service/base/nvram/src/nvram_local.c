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
#include "dsm_dbg.h"
#include "vds_api.h"
#include "nvram.h"
#include "nvram_local.h"


// Global variable.
NVRAM_GLOBAL_VAL g_nvram_global_var = {0, 0, {0,}, NULL, NULL, NULL};


BOOL nvram_Existed(VOID)
{
    NVRAM_H_INFO sHInfo;
    INT32 iResult;
    UINT32 iCrc;

    iResult = nvram_ReadNH(&sHInfo);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_existed: read h_info failed,err_code = %d.", iResult));
        return FALSE;
    }
    iCrc = nvram_CRC32(&sHInfo, (NVRAM_H_SIZE - sizeof(sHInfo.iCrc) - sizeof(sHInfo.szServed)));
    if(NVRAM_MAGIC == sHInfo.iMagic                          &&
            NVRAM_VERSION == sHInfo.iVer                          &&
            nvram_GetDevSize() == sHInfo.iDevSize                 &&
            NVRAM_H_BASE_ADDR == sHInfo.iHOffs                    &&
            NVRAM_H_SIZE == sHInfo.iHSize                         &&
            NVRAM_NDT_BASE_ADDR == sHInfo.iNDOffs                 &&
            NVRAM_NDT_SIZE == sHInfo.iNDSize                      &&
            NVRAM_R_BASE_ADDR == sHInfo.iROffs                    &&
            NVRAM_R_SIZE == sHInfo.iRSize                         &&
            NVRAM_R_COUNT == sHInfo.iRCount                       &&
            iCrc == sHInfo.iCrc)
    {
        return TRUE;
    }
    else
    {
        if(!(NVRAM_MAGIC == sHInfo.iMagic))
        {
            D((DL_NVRAMERROR, "NVRAM_MAGIC(0x%x) == sHInfo.iMagic(0x%x).", NVRAM_MAGIC, sHInfo.iMagic));
        }
        if(!(NVRAM_VERSION == sHInfo.iVer))
        {
            D((DL_NVRAMERROR, "NVRAM_VERSION(0x%x) == sHInfo.iVer(0x%x).", NVRAM_VERSION, sHInfo.iVer));
        }
        if(!(nvram_GetDevSize() == sHInfo.iDevSize))
        {
            D((DL_NVRAMERROR, "nvram_GetDevSize(0x%x) == sHInfo.iMagic(0x%x).", nvram_GetDevSize(), sHInfo.iDevSize));
        }
        if(!(NVRAM_H_BASE_ADDR == sHInfo.iHOffs))
        {
            D((DL_NVRAMERROR, "NVRAM_H_BASE_ADDR(0x%x) == sHInfo.iHOffs(0x%x).", NVRAM_H_BASE_ADDR, sHInfo.iHOffs));
        }
        if(!(NVRAM_H_SIZE == sHInfo.iHSize))
        {
            D((DL_NVRAMERROR, "NVRAM_H_SIZE(0x%x) == sHInfo.iHSize(0x%x).", NVRAM_H_SIZE, sHInfo.iHSize));
        }
        if(!(NVRAM_NDT_BASE_ADDR == sHInfo.iNDOffs))
        {
            D((DL_NVRAMERROR, "NVRAM_NDT_BASE_ADDR(0x%x) == sHInfo.iNDOffs(0x%x).", NVRAM_NDT_BASE_ADDR, sHInfo.iNDOffs));
        }
        if(!(NVRAM_NDT_SIZE == sHInfo.iNDSize))
        {
            D((DL_NVRAMERROR, "NVRAM_NDT_SIZE(0x%x) == sHInfo.iNDSize(0x%x).", NVRAM_NDT_SIZE, sHInfo.iNDSize));
        }
        if(!(NVRAM_R_BASE_ADDR == sHInfo.iROffs))
        {
            D((DL_NVRAMERROR, "NVRAM_R_BASE_ADDR(0x%x) == sHInfo.iROffs(0x%x).", NVRAM_R_BASE_ADDR, sHInfo.iROffs));
        }
        if(!(NVRAM_R_SIZE == sHInfo.iRSize))
        {
            D((DL_NVRAMERROR, "NVRAM_R_SIZE(0x%x) == sHInfo.iRSize(0x%x).", NVRAM_R_SIZE, sHInfo.iRSize));
        }
        if(!(NVRAM_R_COUNT == sHInfo.iRCount))
        {
            D((DL_NVRAMERROR, "NVRAM_R_COUNT(0x%x) == sHInfo.iRCount(0x%x).", NVRAM_R_COUNT, sHInfo.iRCount));
        }
        if(!(iCrc == sHInfo.iCrc))
        {
            D((DL_NVRAMERROR, "iCrc(0x%x) == sHInfo.iCrc(0x%x).", iCrc, sHInfo.iCrc));
        }
        return FALSE;
    }
}

INT32 nvram_Format(VOID)
{
    INT32 iResult;
    UINT32 iAddr;
    NVRAM_R_INFO sRInfo;
    NVRAM_H_INFO sHInfo;
    NVRAM_ND_INFO *psNDT = NULL;
    NVRAM_R_INFO *psRInfo = NULL;
    UINT32 iRCount;
    UINT32 iWRCount = 0;
    UINT32 iCurCount = 0;
    UINT32 iGroupCount = NVRAM_GROUP_NR;
    UINT32 i;
    INT32 iRet = ERR_SUCCESS;

    iRCount = NVRAM_R_COUNT;
    // Write Record.
    DSM_MemSet(sRInfo.pData, NVRAM_DEFAULT_VALUE, NVRAM_DATA_SIZE);
    sRInfo.sCtr.iStatus = NVRAM_STATUS_FREE;
    sRInfo.sCtr.iInd = NVRAM_INVALID_IND;
    sRInfo.sCtr.iId = NVRAM_FREE_ID;
    sRInfo.sCtr.iCrc = NVRAM_INVALID_CRC;

    psRInfo = (NVRAM_R_INFO *)DSM_MAlloc(NVRAM_R_SIZE * iGroupCount);
    if(NULL == psRInfo)
    {
        iGroupCount = 1;
        psRInfo = &sRInfo;
    }
    else
    {
        for(i = 0; i < iGroupCount; i++)
        {
            DSM_MemCpy(psRInfo + i, &sRInfo, NVRAM_R_SIZE);
        }
    }

    while(iWRCount < iRCount)
    {
        iCurCount = (iWRCount + iGroupCount <=  iRCount) ? iGroupCount : (iRCount - iWRCount);
        iAddr = NVRAM_R_BASE_ADDR + (iWRCount * NVRAM_R_SIZE);
        iResult = nvram_Write(iAddr, (UINT8 *)psRInfo, NVRAM_R_SIZE * iCurCount);
        if(ERR_SUCCESS == iResult)
        {
            iWRCount += iCurCount;
        }
        else
        {
            D((DL_NVRAMERROR, "nvram format:nvram_write failed,addr = 0x%x,rn = 0x%x,err_code = %d.", iAddr, iWRCount, iResult));
            iRet = _ERR_NVRAM_MEDIA_WRITE_FAILED;
            goto nvram_format_func;
        }

    }
    if(NVRAM_GROUP_NR == iGroupCount &&
            NULL != psRInfo)
    {
        DSM_Free(psRInfo);
        psRInfo = NULL;
    }

    // Write NDT.
    psNDT = DSM_MAlloc(NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE);
    if(NULL == psNDT)
    {
        D((DL_NVRAMERROR, "nvram_format: malloc(0x%x) failed.", (NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE)));
        DSM_ASSERT(0, "nvram_format:malloc(0x%x) failed.", (NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE));
        iRet =  _ERR_NVRAM_MALLOC_FAILED;
        goto nvram_format_func;
    }

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        psNDT[i].iId = NVRAM_FREE_ID;
        psNDT[i].iSize = 0;
    }
    iAddr = NVRAM_NDT_BASE_ADDR;
    iResult = nvram_Write(iAddr, (UINT8 *)psNDT, (NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE));
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_format: write ndt failed,err_code = %d.", iResult));
        iRet = iResult;
        goto nvram_format_func;
    }


    sHInfo.iMagic = NVRAM_MAGIC;
    sHInfo.iVer = NVRAM_VERSION;
    sHInfo.iDevSize = nvram_GetDevSize();
    sHInfo.iHOffs = NVRAM_H_BASE_ADDR;
    sHInfo.iHSize = NVRAM_H_SIZE;
    sHInfo.iNDOffs = NVRAM_NDT_BASE_ADDR;
    sHInfo.iNDSize = NVRAM_NDT_SIZE;
    sHInfo.iROffs = NVRAM_R_BASE_ADDR;
    sHInfo.iRSize = NVRAM_R_SIZE;
    sHInfo.iRCount = NVRAM_R_COUNT;
    DSM_MemSet(sHInfo.szServed, 0, sizeof(sHInfo.szServed));
    sHInfo.iCrc = nvram_CRC32(&sHInfo, (NVRAM_H_SIZE - sizeof(sHInfo.iCrc) - sizeof(sHInfo.szServed)));

    iResult = nvram_WriteNH(sHInfo);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_format: write h_info failed,err_code = %d.", iResult));
        iRet = iResult;
    }
nvram_format_func:
    if(NULL != psNDT)
    {
        DSM_Free(psNDT);
        psNDT = NULL;
    }
    if(NVRAM_GROUP_NR == iGroupCount &&
            NULL != psRInfo)
    {
        DSM_Free(psRInfo);
        psRInfo = NULL;
    }

    return iRet;
}


VOID nvram_CloseNE(VOID)
{
    UINT32 i;

    for(i = 0; i < NVRAM_OPEN_MAX; i++)
    {
        if(NVRAM_INVALID_ID != g_nvram_global_var.psNInfo[i].iId)
        {
            g_nvram_global_var.psNInfo[i].iId = NVRAM_INVALID_ID;
            g_nvram_global_var.psNInfo[i].iSize = 0;
            g_nvram_global_var.psNInfo[i].iFstRN = NVRAM_INVALID_RN;
        }
    }
    return;
}


INT32 nvram_ScanNR(UINT32 iNDCount, NVRAM_ND_INFO *psNDT, UINT32 iRefCount, NVRAM_REF *pRef)
{
    typedef struct
    {
        UINT32 iId;
        UINT32 iInd;
    } NVRMA_NRD;
    UINT32 i;
    NVRAM_R_INFO sRInfo;
    UINT32 iRN = 0;
    UINT32 iPreRN;
    UINT32 iNextRN;
    UINT32 iInd;
    UINT32 iFstRN = 0;
    UINT32 iNRCount = 0;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    NVRMA_NRD *psNRD = NULL;

    psNRD = DSM_MAlloc(iRefCount * sizeof(NVRMA_NRD));
    if(NULL == psNRD)
    {
        DSM_ASSERT(0, "nvram scan nr:malloc(0x%x) psNRC failed.", (iRefCount * sizeof(NVRMA_NRD)));
        iRet = _ERR_NVRAM_MALLOC_FAILED;
        return iRet;
    }
    for(i = 0; i < iRefCount; i++)
    {
        pRef[i].iId = NVRAM_INVALID_ID;
        pRef[i].iNext = NVRAM_INVALID_IND;

        psNRD[i].iId = NVRAM_INVALID_ID;
        psNRD[i].iInd = NVRAM_INVALID_IND;
    }


    for(i = 0; i < iRefCount; i++)
    {
        iResult = nvram_ReadR(&sRInfo, i);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "nvram scan nr:nvram_read failed,iRN = 0x%x,err_code = %d.", i, iResult));
            iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
            break;
        }

        // arrive free record,break lookup.
        if(NVRAM_STATUS_FREE == sRInfo.sCtr.iStatus &&
                NVRAM_FREE_ID == sRInfo.sCtr.iId)
        {
            break;
        }

        if(NVRAM_STATUS_VALID == sRInfo.sCtr.iStatus &&
                NVRAM_IS_VALID_ID(sRInfo.sCtr.iId))
        {
            psNRD[i].iId = sRInfo.sCtr.iId;
            psNRD[i].iInd = sRInfo.sCtr.iInd;
        }
    }

    for(i = 0; i < iNDCount; i++)
    {
        if(NVRAM_FREE_ID == psNDT[i].iId)
        {
            break;
        }

        if(NVRAM_IS_VALID_ID(psNDT[i].iId))
        {
            iNRCount = NVRAM_NR_COUNT(psNDT[i].iSize);
            iInd = 0;

            iFstRN = iPreRN = NVRAM_INVALID_RN;

            for(iRN = 0; iRN < iRefCount; iRN ++)
            {
                if(NVRAM_FREE_ID == psNRD[iRN].iId)
                {
                    break;
                }
                if(NVRAM_IS_VALID_ID(psNRD[iRN].iId))
                {
                    if(psNDT[i].iId == psNRD[iRN].iId)
                    {
                        if(iInd == psNRD[iRN].iInd)
                        {
                            if(NVRAM_IS_VALID_RN(iPreRN))
                            {
                                pRef[iPreRN].iNext = iRN;
                            }
                            pRef[iRN].iId = psNRD[iRN].iId;
                            pRef[iRN].iNext = NVRAM_INVALID_RN;

                            iPreRN = iRN;
                            iFstRN = NVRAM_INVALID_RN == iFstRN ? iRN : iFstRN;
                            iInd ++;
                        }
                        else
                        {
                            //pRef[iRN].iId = NVRAM_INVALID_ID;
                            //pRef[iRN].iNext = NVRAM_INVALID_IND;
                        }
                    }
                    else
                    {

                    }
                }
                if(iInd == iNRCount)
                {
                    break;
                }
            }

            if(iInd != iNRCount)
            {
                iRN = iFstRN;
                while(NVRAM_IS_VALID_RN(iRN))
                {
                    iNextRN = pRef[iRN].iNext;
                    pRef[iRN].iId = NVRAM_INVALID_ID;
                    pRef[iRN].iNext = NVRAM_INVALID_RN;
                    iRN = iNextRN;
                }
                psNDT[i].iId = NVRAM_INVALID_ID;
            }
        }
    }

    if(psNRD)
    {
        DSM_Free(psNRD);
        psNRD = NULL;
    }
    return iRet;
}

INT32 nvram_InitGlobal(VOID)
{
    UINT32 iAddr;
    NVRAM_H_INFO sHInfo;
    NVRAM_INFO *psNInfo = NULL;
    NVRAM_ND_INFO *psNDT = NULL;
    NVRAM_REF *psRef = NULL;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    UINT32 i;

    // Get nvram device size.
    if(nvram_GetDevSize() ==  0)
    {
        D((DL_NVRAMERROR, "nvram_init_global: GetDevSize = 0."));
        return _ERR_NVRAM_GET_DEV_INFO_FAILED;
    }

    // Read  nvram header.
    iResult = nvram_ReadNH(&sHInfo);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_init_global: read h_info failed,err_code = %d.", iResult));
        return iResult;
    }

    g_nvram_global_var.sNH = sHInfo;

    // Init psNInfo.
    psNInfo = DSM_MAlloc(NVRAM_OPEN_MAX * NVRAM_INFO_SIZE);
    if(NULL == psNInfo)
    {
        DSM_ASSERT(0, "nvram_init_global:malloc(0x%x) psNInfo failed.", (NVRAM_OPEN_MAX * NVRAM_INFO_SIZE));
        iRet = _ERR_NVRAM_MALLOC_FAILED;
        goto nvram_init_global_fail;
    }

    for(i = 0; i < NVRAM_OPEN_MAX; i++)
    {
        psNInfo[i].iId = NVRAM_FREE_ID;
        psNInfo[i].iSize = 0;
        psNInfo[i].iFstRN = NVRAM_INVALID_RN;
    }
    g_nvram_global_var.psNInfo = psNInfo;

    // Init psNDT.
    psNDT = DSM_MAlloc(NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE);
    if(NULL ==  psNDT)
    {
        D((DL_NVRAMERROR, "nvram_init_global: malloc(0x%x) psNDT failed.", (NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE)));
        DSM_ASSERT(0, "nvram_init_global:malloc(0x%x) psNDT failed.", (NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE));
        iRet =  _ERR_NVRAM_MALLOC_FAILED;
        goto nvram_init_global_fail;
    }

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        psNDT[i].iId = NVRAM_FREE_ID;
        psNDT[i].iSize = 0;
    }
    iAddr = NVRAM_NDT_BASE_ADDR;
    iResult = nvram_Read(iAddr, (UINT8 *)psNDT, (NVRAM_ENTRIES_COUNT * NVRAM_ND_SIZE));
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_init_global: read ndt failed,err_code = %d.", iResult));
        iRet = iResult;
        goto nvram_init_global_fail;
    }
    g_nvram_global_var.psNDT = psNDT;


    // Init psRef.

    psRef = DSM_MAlloc(NVRAM_R_COUNT * NVRAM_REF_SIZE);
    if(NULL == psRef)
    {
        DSM_ASSERT(0, "nvram_init_global:malloc(0x%x) psRef failed.", (NVRAM_R_COUNT * NVRAM_REF_SIZE));
        iRet = _ERR_NVRAM_MALLOC_FAILED;
        goto nvram_init_global_fail;
    }

    iResult = nvram_ScanNR(NVRAM_ENTRIES_COUNT, psNDT, NVRAM_R_COUNT, psRef);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_init_global: scan nr failed,err_code = %d.", iResult));
        iRet =  iResult;
        goto nvram_init_global_fail;
    }
    g_nvram_global_var.psRef = psRef;

    nvram_SemInit();
    g_nvram_global_var.iHasInited = 1;
    return ERR_SUCCESS;

nvram_init_global_fail:
    if(psNInfo)
    {
        DSM_Free(psNInfo);
        psNInfo = NULL;
    }
    if(psNDT)
    {
        DSM_Free(psNDT);
        psNDT = NULL;
    }
    if(psRef)
    {
        DSM_Free(psRef);
        psRef = NULL;
    }
    return iRet;
}


INT32 nvram_Read(UINT32 iAddrOffset, UINT8 *pBuffer, UINT32 iSize)
{
    INT32 iDevNo;
    UINT32 iBaseAddr = 0;
    UINT32 iBytesRead = 0;

    iDevNo = DSM_GetDevNrOnModuleId(DSM_MODULE_NVRAM);
    iBaseAddr = DSM_NVRAM_BASE_ADDR;


    if(INVALID_DEVICE_NUMBER == iDevNo)
    {
        D((DL_NVRAMERROR, "nvram_Read: get DevNr iPartId = %d.", iDevNo));
        return _ERR_NVRAM_GET_DEV_NR_FAILED;
    }

    return DSM_Read(iDevNo, iAddrOffset + iBaseAddr, pBuffer, iSize, &iBytesRead);

}


INT32 nvram_Write(UINT32 iAddrOffset, CONST UINT8 *pBuffer, UINT32 iSize)
{
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    INT32 iDevNo;
    UINT32 iTimes = 0;
    UINT32 iBaseAddr = 0;
    UINT32 iByteWriten = 0;

    iDevNo = DSM_GetDevNrOnModuleId(DSM_MODULE_NVRAM);
    iBaseAddr = DSM_NVRAM_BASE_ADDR;

    if(INVALID_DEVICE_NUMBER == iDevNo)
    {
        D((DL_NVRAMERROR, "nvram_Write: get DevNr iPartId = %d.", iDevNo));
        return _ERR_NVRAM_GET_DEV_NR_FAILED;
    }
    do
    {
        iResult = DSM_Write(iDevNo, iAddrOffset + iBaseAddr, pBuffer, iSize, &iByteWriten);
        if(iResult != ERR_SUCCESS)
        {
            D((DL_NVRAMERROR, "nvram_Write failed. err code = %d,times = %d.", iResult, iTimes));
            iRet = iResult;
        }
        else
        {
            iRet = ERR_SUCCESS;
            break;
        }
        iTimes ++;

    }
    while(iTimes < 3);
    return iRet;
}


UINT32 nvram_GetDevSize(VOID)
{
    UINT32 iSize = 0;
    UINT32 iPartId;
    UINT32 iNrBlock = 0;
    UINT32 iBlockSize = 0;
    INT32 iResult;

    iPartId = DSM_GetDevNrOnModuleId(DSM_MODULE_NVRAM);
    if(INVALID_DEVICE_NUMBER != iPartId)
    {
        iResult = DRV_GET_DEV_INFO(iPartId, &iNrBlock, &iBlockSize);
        if(ERR_SUCCESS == iResult)
        {
            iSize = (iNrBlock * iBlockSize);
        }
        else
        {
            D((DL_NVRAMERROR, "nvram get size:get partition info failed,err_code = %d.", iResult));
        }

    }
    else
    {
        D((DL_NVRAMERROR, "nvram get size:get DevNr failed,iPartId = %d.", iPartId));
    }
    return iSize;
}


INT32   nvram_GetUsedSize(UINT32 *piUsedSize)
{

    UINT32 i;
    UINT32 iSize = 0;
    INT32 iRet = ERR_SUCCESS;

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        if(NVRAM_FREE_ID == g_nvram_global_var.psNDT[i].iId)
        {
            break;
        }
        else if(NVRAM_IS_VALID_ID(g_nvram_global_var.psNDT[i].iId))
        {
            iSize += ((g_nvram_global_var.psNDT[i].iSize + NVRAM_DATA_SIZE - 1) / NVRAM_DATA_SIZE) * NVRAM_DATA_SIZE;
        }
    }

    *piUsedSize = iSize;
    return iRet;
}


INT32   nvram_GetEntries(NVRAM_ND_INFO *psEntries, UINT32 *piCount)
{
    UINT32 i;
    UINT32 iCount = 0;

    INT32 iRet = ERR_SUCCESS;

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        if(NVRAM_FREE_ID == g_nvram_global_var.psNDT[i].iId)
        {
            break;
        }
        else if(NVRAM_IS_VALID_ID(g_nvram_global_var.psNDT[i].iId) &&
                0 != g_nvram_global_var.psNDT[i].iSize)
        {
            psEntries[iCount] = g_nvram_global_var.psNDT[i];
            iCount++;
        }
    }

    *piCount = iCount;
    return iRet;
}

INT32   nvram_GetEntriesCount(UINT32 *piCount)
{
    UINT32 i;
    UINT32 iCount = 0;

    INT32 iRet = ERR_SUCCESS;

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        if(NVRAM_FREE_ID == g_nvram_global_var.psNDT[i].iId)
        {
            break;
        }
        else if(NVRAM_IS_VALID_ID(g_nvram_global_var.psNDT[i].iId) &&
                0 != g_nvram_global_var.psNDT[i].iSize)
        {
            iCount++;
        }
    }

    *piCount = iCount;
    return iRet;
}



INT32 nvram_WriteNH(NVRAM_H_INFO sNH)
{
    UINT32 iAddr;
    // UINT32 iSize = 0;
    INT32 iResult;
    INT32 iRet;

    iAddr = NVRAM_H_BASE_ADDR;
    iResult = nvram_Write(iAddr, (UINT8 *)&sNH, NVRAM_H_SIZE);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }
    else
    {
        D((DL_NVRAMERROR, "nvram write NH:nvram_write failed,iAddr = 0x%x,err_code = %d.", iAddr, iResult));
        iRet = _ERR_NVRAM_MEDIA_WRITE_FAILED;
    }
    return iRet;
}


INT32 nvram_ReadNH(NVRAM_H_INFO *psNH)
{
    UINT32 iAddr;
    // UINT32 iSize = 0;
    INT32 iResult;
    INT32 iRet;

    iAddr = NVRAM_H_BASE_ADDR;
    iResult = nvram_Read(iAddr, (UINT8 *)psNH, NVRAM_H_SIZE);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }
    else
    {
        D((DL_NVRAMERROR, "nvram read NH:nvram_read failed,iAddr = 0x%x,err_code = %d.", iAddr, iResult));
        iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
    }
    return iRet;
}


INT32 nvram_AddND(NVRAM_ND_INFO sNDInfo)
{
    UINT32 iFreeND = NVRAM_ENTRIES_COUNT;
    UINT32 iAddr;
    UINT32 i;
    NVRAM_ND_INFO *psND;
    INT32 iResult;
    INT32 iRet;

    psND = g_nvram_global_var.psNDT;

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        if(!NVRAM_IS_VALID_ID(psND[i].iId))
        {
            iFreeND = i;
            break;
        }
    }

    if(NVRAM_IS_VALID_NDN(iFreeND))
    {
        iAddr = NVRAM_NDT_BASE_ADDR + (iFreeND * NVRAM_ND_SIZE);
        iResult = nvram_Write(iAddr, (UINT8 *)&sNDInfo, NVRAM_ND_SIZE);
        if(ERR_SUCCESS == iResult)
        {
            iRet = ERR_SUCCESS;
            psND[i] = sNDInfo;
        }
        else
        {
            D((DL_NVRAMERROR, "nvram add nd:nvram_write failed,addr = 0x%x,free_nd = 0x%x,err_code = %d.", iAddr, iFreeND, iResult));
            iRet = _ERR_NVRAM_MEDIA_WRITE_FAILED;
        }
    }
    else
    {
        D((DL_NVRAMERROR, "nvram add nd:no more nd . nd_offs = 0x%x.", i));
        iRet = _ERR_NVRAM_NO_MORE_ND;
    }

    return iRet;
}


INT32 nvram_SetNDE(NVRAM_ND_INFO sNDInfo, UINT32 iNDN)
{
    NVRAM_ND_INFO *psND;
    UINT32 iAddr;
    INT32 iResult;
    INT32 iRet;


    if(NVRAM_IS_VALID_NDN(iNDN))
    {
        psND = g_nvram_global_var.psNDT;
        iAddr = NVRAM_NDT_BASE_ADDR + (iNDN * NVRAM_ND_SIZE);
        iResult = nvram_Write(iAddr, (UINT8 *)&sNDInfo, NVRAM_ND_SIZE);
        if(ERR_SUCCESS == iResult)
        {
            psND[iNDN] = sNDInfo;
            iRet = ERR_SUCCESS;
        }

        else
        {
            D((DL_NVRAMERROR, "nvram set nde:nvram_write failed,addr = 0x%x,ndn = 0x%x,err_code = %d.", iAddr, iNDN, iResult));
            iRet = _ERR_NVRAM_MEDIA_WRITE_FAILED;
        }

    }
    else
    {
        D((DL_NVRAMERROR, "nvram set nde: parameter error. ndn = 0x%x.", iNDN));
        iRet = _ERR_NVRAM_PARAM_ERROR;
    }
    return iRet;
}


INT32 nvram_GetNDE(UINT32 iId, NVRAM_ND_INFO *psNDInfo, UINT32 *piNDN)
{
    UINT32 i;
    INT32 iRet = NDN_NOT_EXIST;

    for(i = 0; i < NVRAM_ENTRIES_COUNT; i++)
    {
        if(NVRAM_FREE_ID == g_nvram_global_var.psNDT[i].iId)
        {
            break;
        }
        if(iId == g_nvram_global_var.psNDT[i].iId)
        {
            *piNDN = i;
            *psNDInfo = g_nvram_global_var.psNDT[i];
            iRet = NDN_IS_EXIST;
            break;
        }
    }
    return iRet;
}


INT32 nvram_DelNDE(UINT32 iNDN)
{
    NVRAM_ND_INFO *psND;
    NVRAM_ND_INFO sNDInfo;
    UINT32 iAddr;
    INT32 iResult;
    INT32 iRet;


    if(NVRAM_IS_VALID_NDN(iNDN))
    {
        psND = g_nvram_global_var.psNDT;
        sNDInfo.iId = NVRAM_INVALID_ID;
        sNDInfo.iSize = 0;
        iAddr = NVRAM_NDT_BASE_ADDR + (iNDN * NVRAM_ND_SIZE);
        iResult = nvram_Write(iAddr, (UINT8 *)&sNDInfo, NVRAM_ND_SIZE);
        if(ERR_SUCCESS == iResult)
        {
            psND[iNDN] = sNDInfo;
            iRet = ERR_SUCCESS;
        }

        else
        {
            D((DL_NVRAMERROR, "nvram del ndenvram_write failed,addr = 0x%x,ndn = 0x%x,err_code = %d.", iAddr, iNDN, iResult));
            iRet = _ERR_NVRAM_MEDIA_WRITE_FAILED;
        }
    }
    else
    {
        D((DL_NVRAMERROR, "nvram del nde: parameter error. ndn = 0x%x.", iNDN));
        iRet = _ERR_NVRAM_PARAM_ERROR;
    }
    return iRet;
}


INT32 nvram_WriteR(NVRAM_R_INFO sRInfo, UINT32 iRN)
{
    UINT32 iAddr;
    INT32 iResult;
    INT32 iRet;

    if(!NVRAM_IS_VALID_RN(iRN))
    {
        D((DL_NVRAMERROR, "nvram write r:invalid rn,rn = 0x%x.", iRN));
        return _ERR_NVRAM_INVALID_ID;
    }
    iAddr = NVRAM_R_BASE_ADDR + (iRN * NVRAM_R_SIZE);

    iResult = nvram_Write(iAddr, (UINT8 *)&sRInfo, NVRAM_R_SIZE);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }

    else
    {
        D((DL_NVRAMERROR, "nvram write r:nvram_write failed,addr = 0x%x,rn = 0x%x,err_code = %d.", iAddr, iRN, iResult));
        iRet = _ERR_NVRAM_MEDIA_WRITE_FAILED;
    }
    return iRet;
}


INT32 nvram_DeleteR(UINT32 iRN)
{
    NVRAM_R_INFO sRInfo;
    INT32 iResult;

    DSM_MemSet(sRInfo.pData, NVRAM_DEFAULT_VALUE, NVRAM_DATA_SIZE);
    sRInfo.sCtr.iStatus = NVRAM_STATUS_INVALID;
    sRInfo.sCtr.iInd = NVRAM_INVALID_IND;
    sRInfo.sCtr.iId = NVRAM_INVALID_ID;
    sRInfo.sCtr.iCrc = NVRAM_INVALID_CRC;
    iResult = nvram_WriteR(sRInfo, iRN);
    if(ERR_SUCCESS == iResult)
    {
        g_nvram_global_var.psRef[iRN].iId = NVRAM_INVALID_ID;
        g_nvram_global_var.psRef[iRN].iNext = NVRAM_INVALID_RN;
    }

    else
    {
        D((DL_NVRAMERROR, "nvram delete rn:nvram_write rn failed,rn = 0x%x,err_code = %d.", iRN, iResult));
    }
    return iResult;
}


INT32 nvram_ReadR(NVRAM_R_INFO *psRInfo, UINT32 iRN)
{
    UINT32 iAddr;
    INT32 iResult;
    INT32 iRet;

    if(!NVRAM_IS_VALID_RN(iRN))
    {
        D((DL_NVRAMERROR, "nvram read r:invalid rn,rn = 0x%x.", iRN));
        return _ERR_NVRAM_INVALID_ID;
    }
    iAddr = NVRAM_R_BASE_ADDR + (iRN * NVRAM_R_SIZE);

    iResult = nvram_Read(iAddr, (UINT8 *)psRInfo, NVRAM_R_SIZE);
    if(ERR_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }

    else
    {
        D((DL_NVRAMERROR, "nvram read r:nvram_write failed,addr = 0x%x,rn = 0x%x,err_code = %d.", iAddr, iRN, iResult));
        iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
    }
    return iRet;
}


INT32 nvram_WriteNV(NVRAM_INFO *psNInfo, UINT32 iId, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff)
{
    UINT32 iRN;
    UINT32 iNextRN;
    UINT32 iInd;
    UINT32 iROffs;
    INT32 iResult;
    UINT32 iPos;
    UINT32 iWritenSize = 0;
    NVRAM_R_INFO sRInfo;
    INT32 iRet;

    iRet = ERR_SUCCESS;
    // Check the input parameter.
    if(NULL == psNInfo ||
            !NVRAM_IS_VALID_ID(iId) ||
            NULL == pBuff)
    {
        D((DL_NVRAMERROR, "nvram write nv:param error,psNInfo = 0x%x,iId = 0x%x,pBuff = 0x%x.", psNInfo, iId, pBuff));
        iRet = _ERR_NVRAM_PARAM_ERROR;
        return iRet;
    }


    if(iOffs >= psNInfo->iSize)
    {
        D((DL_NVRAMERROR, "nvram write nv:offs overflow,offs = 0x%x, size = 0x%x.", iOffs, psNInfo->iSize));
        iRet = _ERR_NVRAM_OFFS_OVERFLOW;
        return iRet;
    }

    // Figure out the spare size.
    if ( iOffs + iSize > psNInfo->iSize )
    {
        iSize = psNInfo->iSize - iOffs;
    }

    iPos = iOffs;
    iROffs = iPos % NVRAM_DATA_SIZE;

    iInd = 0;
    iRN = psNInfo->iFstRN;
    while(NVRAM_IS_VALID_RN(iRN))
    {
        if(iInd == iPos / NVRAM_DATA_SIZE)
        {
            break;
        }
        iNextRN = NVRAM_INVALID_RN;
        iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
        if(ERR_SUCCESS == iResult)
        {
        }
        else
        {
            D((DL_NVRAMERROR, "nvram write nv: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
            return iResult;
        }
        iRN = iNextRN;
        iInd ++;
    }

    if(NVRAM_IS_VALID_RN(iRN))
    {
        // Write the first record.
        if ( iROffs != 0 )
        {
            //iRN = pRef->iRN;
            iResult = nvram_ReadR(&sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram write nv:nvram_read fst failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }
            if ( iROffs + iSize <= NVRAM_DATA_SIZE )
            {
                DSM_MemCpy( sRInfo.pData + iROffs, pBuff, iSize);
                iPos += iSize;
                iWritenSize += iSize;
            }
            else
            {
                DSM_MemCpy(sRInfo.pData + iROffs, pBuff, NVRAM_DATA_SIZE - iROffs );
                iPos += ( NVRAM_DATA_SIZE - iROffs );
                iWritenSize += ( NVRAM_DATA_SIZE - iROffs );
            }

            sRInfo.sCtr.iId = psNInfo->iId;
            sRInfo.sCtr.iInd = iInd;
            sRInfo.sCtr.iStatus = NVRAM_STATUS_VALID;
            sRInfo.sCtr.iCrc = nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE);

            iResult = nvram_WriteR(sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read r:nvram_write fst failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet =  _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }
            iNextRN = NVRAM_INVALID_RN;
            iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS == iResult)
            {

            }
            else
            {
                D((DL_NVRAMERROR, "nvram_WriteNV: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                return iResult;
            }
            iRN = iNextRN;
        }
        // Write middle record.
        while ( iWritenSize + NVRAM_DATA_SIZE <= iSize )
        {

            if(!NVRAM_IS_VALID_RN(iRN))
            {
                D((DL_NVRAMERROR, "nvram write nv:param error,iRInd = 0x%x,iPos = 0x%x.", iInd, iPos));
                iRet = _ERR_NVRAM_PARAM_ERROR;
                return iRet;
            }

            //iRN = pRef->iRN;
            DSM_MemCpy(sRInfo.pData, pBuff + iWritenSize, NVRAM_DATA_SIZE);
            sRInfo.sCtr.iId = psNInfo->iId;
            sRInfo.sCtr.iInd = iInd;
            sRInfo.sCtr.iStatus = NVRAM_STATUS_VALID;
            sRInfo.sCtr.iCrc = nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE);

            iResult = nvram_WriteR(sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read r:nvram_write mid failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }
            iPos += NVRAM_DATA_SIZE;
            iWritenSize += NVRAM_DATA_SIZE;
            iInd =  iPos / NVRAM_DATA_SIZE;
            iNextRN = NVRAM_INVALID_RN;
            iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS == iResult)
            {
            }
            else
            {
                D((DL_NVRAMERROR, "nvram_WriteNV: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                return iResult;
            }
            iRN = iNextRN;
        }

        // Write the last record.
        if (iWritenSize < iSize)
        {
            if(!NVRAM_IS_VALID_RN(iRN))
            {
                D((DL_NVRAMERROR, "nvram write nv:write last block,param error,iRInd = 0x%x,iPos = 0x%x.", iInd, iPos));
                iRet = _ERR_NVRAM_PARAM_ERROR;
                return iRet;
            }
            iInd =  iPos / NVRAM_DATA_SIZE;
            //iRN = pRef->iRN;
            iResult = nvram_ReadR(&sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram write nv:read last failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }

            DSM_MemCpy(sRInfo.pData, pBuff + iWritenSize, (iSize - iWritenSize));
            iPos += ( iSize - iWritenSize );
            iWritenSize += ( iSize - iWritenSize );


            sRInfo.sCtr.iId = psNInfo->iId;
            sRInfo.sCtr.iInd = iInd;
            sRInfo.sCtr.iStatus = NVRAM_STATUS_VALID;
            sRInfo.sCtr.iCrc = nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE);

            iResult = nvram_WriteR(sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read r:write last failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }

            iWritenSize += ( iSize - iWritenSize );
        }

    }
    else
    {
        D((DL_NVRAMERROR, "nvram write nv:param error,iRInd = 0x%x.", iInd));
        iRet = _ERR_NVRAM_PARAM_ERROR;
    }

    return iRet;
}


INT32 nvram_ReadNV(NVRAM_INFO *psNInfo, UINT32 iId, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff)
{
    UINT32 iRN;
    UINT32 iNextRN;
    UINT32 iInd;
    UINT32 iROffs;
    INT32 iResult;
    UINT32 iPos;
    UINT32 iReadSize = 0;
    NVRAM_R_INFO sRInfo;
    INT32 iRet;

    iRet = ERR_SUCCESS;
    // Check the input parameter.
    if(NULL == psNInfo ||
            !NVRAM_IS_VALID_ID(iId) ||
            NULL == pBuff)
    {
        D((DL_NVRAMERROR, "nvram read nv:param error,psNInfo = 0x%x,iId = 0x%x,pBuff = 0x%x.", psNInfo, iId, pBuff));
        iRet = _ERR_NVRAM_PARAM_ERROR;
        return iRet;
    }

    if(iOffs >= psNInfo->iSize)
    {
        D((DL_NVRAMERROR, "nvram read nv:offs overflow,offs = 0x%x, size = 0x%x.", iOffs, psNInfo->iSize));
        return _ERR_NVRAM_OFFS_OVERFLOW;
    }

    // Figure out the spare size.
    if ( iOffs + iSize > psNInfo->iSize )
    {
        iSize = psNInfo->iSize - iOffs;
    }

    iPos = iOffs;
    iROffs = iPos % NVRAM_DATA_SIZE;

    iInd = 0;
    iRN = psNInfo->iFstRN;
    while(NVRAM_IS_VALID_RN(iRN))
    {
        if(iInd == iPos / NVRAM_DATA_SIZE)
        {
            break;
        }
        iNextRN = NVRAM_INVALID_RN;
        iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
        if(ERR_SUCCESS == iResult)
        {
        }
        else
        {
            D((DL_NVRAMERROR, "nvram read nv: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
            return iResult;
        }
        iRN = iNextRN;
        iInd ++;
    }

    if(NVRAM_IS_VALID_RN(iRN))
    {
        // Read the first record.
        if ( iROffs != 0)
        {
            iResult = nvram_ReadR(&sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read nv:read fst failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                return _ERR_NVRAM_MEDIA_READ_FAILED;
            }
            if(NVRAM_STATUS_VALID != (sRInfo.sCtr.iStatus & NVRAM_STATUS_VALID) &&
                    NVRAM_STATUS_USED != (sRInfo.sCtr.iStatus & NVRAM_STATUS_USED))
            {
                D((DL_NVRAMERROR, "nvram read nv:read fst status error,iStatus = 0x%x,iRN = 0x%x.", sRInfo.sCtr.iStatus, iRN));
                iRet =  _ERR_NVRAM_STATUS_ERROR;
                return iRet;
            }
#if 0
            else if(nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE) != sRInfo.sCtr.iCrc)
            {
                D((DL_NVRAMERROR, "nvram read nv:read fst verify error,sCtr.crc = 0x%x,crc = 0x%x.", sRInfo.sCtr.iCrc, nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE)));
                return _ERR_NVRAM_CHECKOUT_ERROR;
            }
#endif
            else
            {
                if ( iROffs + iSize <= NVRAM_DATA_SIZE )
                {
                    DSM_MemCpy(pBuff, sRInfo.pData + iROffs, iSize);
                    iPos += iSize;
                    iReadSize += iSize;
                }
                else
                {
                    DSM_MemCpy(pBuff, sRInfo.pData + iROffs, NVRAM_DATA_SIZE - iROffs );
                    iPos += ( NVRAM_DATA_SIZE - iROffs );
                    iReadSize += ( NVRAM_DATA_SIZE - iROffs );
                }
            }
            iNextRN = NVRAM_INVALID_RN;
            iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS == iResult)
            {
            }
            else
            {
                D((DL_NVRAMERROR, "nvram read nv: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                return iResult;
            }
            iRN = iNextRN;
        }

        // Read middle record.
        while ( iReadSize + NVRAM_DATA_SIZE <= iSize )
        {

            if(!NVRAM_IS_VALID_RN(iRN))
            {
                D((DL_NVRAMERROR, "nvram read nv:param error,iRInd = 0x%x,iPos = 0x%x.", iInd, iPos));
                iRet = _ERR_NVRAM_PARAM_ERROR;
                return iRet;
            }

            iResult = nvram_ReadR(&sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read nv:read mid failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }
            if(NVRAM_STATUS_VALID != (sRInfo.sCtr.iStatus & NVRAM_STATUS_VALID) &&
                    NVRAM_STATUS_USED != (sRInfo.sCtr.iStatus & NVRAM_STATUS_USED))
            {
                D((DL_NVRAMERROR, "nvram read nv:read mid status error,iStatus = 0x%x,iRN = 0x%x.", sRInfo.sCtr.iStatus, iRN));
                iRet = _ERR_NVRAM_STATUS_ERROR;
                return iRet;
            }
            else
            {
                DSM_MemCpy(pBuff + iReadSize, sRInfo.pData, NVRAM_DATA_SIZE);
            }
            iPos += NVRAM_DATA_SIZE;
            iReadSize += NVRAM_DATA_SIZE;
            iInd =  iPos / NVRAM_DATA_SIZE;

            iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
            if(ERR_SUCCESS == iResult)
            {
                iRN = iNextRN;
            }
            else
            {
                D((DL_NVRAMERROR, "nvram read nv: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
                return iResult;
            }
        }

        // Read the last record.
        if (iReadSize < iSize)
        {
            if(!NVRAM_IS_VALID_RN(iRN))
            {
                D((DL_NVRAMERROR, "nvram read nv:read last param error,iRInd = 0x%x,iPos = 0x%x.", iInd, iPos));
                iRet = _ERR_NVRAM_PARAM_ERROR;
                return iRet;
            }
            iInd =  iPos / NVRAM_DATA_SIZE;
            iResult = nvram_ReadR(&sRInfo, iRN);
            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read nv:read last failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }


            if(ERR_SUCCESS != iResult)
            {
                D((DL_NVRAMERROR, "nvram read nv:read mid failed,rn = 0x%x,err_code = %d.", iRN, iResult));
                iRet = _ERR_NVRAM_MEDIA_READ_FAILED;
                return iRet;
            }

            if(NVRAM_STATUS_VALID != (sRInfo.sCtr.iStatus & NVRAM_STATUS_VALID) &&
                    NVRAM_STATUS_USED != (sRInfo.sCtr.iStatus & NVRAM_STATUS_USED) )
            {
                D((DL_NVRAMERROR, "nvram read nv:read mid status error,iStatus = 0x%x,iRN = 0x%x.", sRInfo.sCtr.iStatus, iRN));
                iRet = _ERR_NVRAM_STATUS_ERROR;
                return iRet;
            }
            else
            {

                DSM_MemCpy(pBuff + iReadSize, sRInfo.pData, iSize - iReadSize);
                iPos += ( iSize - iReadSize );
                iReadSize += ( iSize - iReadSize );

            }
        }

    }
    else
    {
        D((DL_NVRAMERROR, "nvram read nv:param error,iRInd = 0x%x.", iInd));
        iRet = _ERR_NVRAM_PARAM_ERROR;
    }

    return iRet;
}




INT32 nvram_GetFstRN(UINT32 iId, UINT32 *piFstRN)
{
    UINT32 i;
    INT32 iRet = ERR_SUCCESS;

    *piFstRN = NVRAM_INVALID_RN;
    for(i = 0; i < NVRAM_R_COUNT; i++)
    {
        // arrive free record,break lookup.
        if(NVRAM_FREE_ID == g_nvram_global_var.psRef[i].iId)
        {
            break;
        }

        if(iId == g_nvram_global_var.psRef[i].iId)
        {
            *piFstRN = i;
            break;
        }

    }
    if(i == NVRAM_R_COUNT)
    {
        D((DL_NVRAMERROR, "nvram_getfst_nr: can't find the first RN for this id. iId = 0x%x.", iId));
        iRet = _ERR_NVRAM_RN_ERROR;
    }
    return iRet;
}


INT32 nvram_GetNextRN(UINT32 iId, UINT32 iCurRN, UINT32 *piNextRN)
{
    INT32 iRet = ERR_SUCCESS;

    *piNextRN = NVRAM_INVALID_RN;

    if(NVRAM_IS_VALID_RN(iCurRN))
    {
        if(iId == g_nvram_global_var.psRef[iCurRN].iId)
        {
            *piNextRN = g_nvram_global_var.psRef[iCurRN].iNext;
        }
    }
    else
    {
        D((DL_NVRAMERROR, "nvram_getnext_nr: iCurRN nr error. iCurRN = 0x%x.", iCurRN));
        iRet = _ERR_NVRAM_RN_ERROR;
    }
    return iRet;
}


INT32 nvram_GetRNCount(UINT32 iId, UINT32 *piRNCount)
{
    UINT32 i;
    UINT32  iRNCount = 0;
    UINT32 iRN = NVRAM_INVALID_RN;
    UINT32 iNextRN;
    INT32 iRet = ERR_SUCCESS;

    *piRNCount = 0;
    for(i = 0; i < NVRAM_R_COUNT; i++)
    {
        // arrive free record,break lookup.
        if(NVRAM_FREE_ID == g_nvram_global_var.psRef[i].iId)
        {
            break;
        }

        if(iId == g_nvram_global_var.psRef[i].iId)
        {
            iRN = i;
            break;
        }

    }

    while(NVRAM_IS_VALID_RN(iRN))
    {
        iRNCount ++;
        iNextRN = g_nvram_global_var.psRef[iRN].iNext;
        iRN = iNextRN;
    }


    *piRNCount = iRNCount;
    iRet = ERR_SUCCESS;

    return iRet;
}


INT32 nvram_GetFreeRN(UINT32 iId, UINT32 iCurRN, UINT32 *piFreeRN)
{
    UINT32 i;
    INT32 iRet = ERR_SUCCESS;
    UINT32 iStartRN;

    *piFreeRN = NVRAM_INVALID_RN;
    iStartRN = NVRAM_IS_VALID_RN(iCurRN) ? iCurRN : 0;
    for(i = iStartRN; i < NVRAM_R_COUNT; i++)
    {
        // arrive free record,break lookup.
        if(NVRAM_FREE_ID == g_nvram_global_var.psRef[i].iId ||
                NVRAM_INVALID_ID == g_nvram_global_var.psRef[i].iId)
        {
            if NVRAM_IS_VALID_RN(iCurRN)
            {
                g_nvram_global_var.psRef[iCurRN].iNext = i;
            }
            g_nvram_global_var.psRef[i].iId = iId;
            g_nvram_global_var.psRef[i].iNext = NVRAM_INVALID_RN;
            *piFreeRN = i;
            break;
        }
    }

    if(i == NVRAM_R_COUNT)
    {
        D((DL_NVRAMERROR, "nvram_get_free_rn: no more free nr. iId = 0x%x,iStartRN = 0x%x.", iId, iCurRN));
        iRet = _ERR_NVRAM_NO_MORE_NR;
    }
    return iRet;
}



BOOL nvram_CheckId(UINT32 iId, UINT32 iRNCount)
{
    //UINT32 iGetRNCount = 0;
    UINT32 iRN = NVRAM_INVALID_RN;
    UINT32 iNextRN = NVRAM_INVALID_RN;
    UINT32 iInd = 0;
    UINT32 iCrc;
    NVRAM_R_INFO sRInfo;
    INT32 iResult;
    //INT32 bRet = TRUE;

    iResult = nvram_GetRNCount(iId, &iRNCount);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_CheckId: get rn count failed,iId = 0x%x,err_code = %d.", iId, iResult));
        //nvram_ErrCode(iResult);
        return FALSE;
    }
    if(iRNCount != iRNCount)
    {
        return FALSE;
    }

    iResult = nvram_GetFstRN(iId, &iRN);
    if(ERR_SUCCESS != iResult)
    {
        D((DL_NVRAMERROR, "nvram_CheckId: get fst rn failed,iId = 0x%x,err_code = %d.", iId, iResult));
        //nvram_ErrCode(iResult);
        return FALSE;
    }
    iInd = 0;
    while(NVRAM_IS_VALID_RN(iRN) && iInd < iRNCount)
    {

        iResult = nvram_ReadR(&sRInfo, iRN);
        if(ERR_SUCCESS != iResult)
        {
            D((DL_NVRAMERROR, "nvram_CheckId: nvram read rn failed,iId = 0x%x,iRN = 0x%x,err_code = %d.", iId, iRN, iResult));
            //nvram_ErrCode(iResult);
            return FALSE;
        }

        if(sRInfo.sCtr.iId != iId ||
                sRInfo.sCtr.iInd != iInd ||
                (NVRAM_STATUS_VALID != sRInfo.sCtr.iStatus &&
                 NVRAM_STATUS_USED != sRInfo.sCtr.iStatus))
        {
            D((DL_NVRAMERROR, "nvram_CheckId: nvram sctr error,iId = 0x%x,iInd = 0x%x,iStatus = 0x%x.",
               sRInfo.sCtr.iId,
               sRInfo.sCtr.iInd,
               sRInfo.sCtr.iStatus));
            iResult = _ERR_NVRAM_RN_ERROR;
            //nvram_ErrCode(iResult);
            return FALSE;
        }
        iCrc = nvram_CRC32(sRInfo.pData, NVRAM_DATA_SIZE);
        if(iCrc != sRInfo.sCtr.iCrc)
        {
            D((DL_NVRAMERROR, "nvram_CheckId: crc error,iId = 0x%x,iCrc = 0x%x,sCtr.iCrc = 0x%x.", iId, iCrc, sRInfo.sCtr.iCrc));
            iResult = _ERR_NVRAM_CHECKOUT_ERROR;
            //nvram_ErrCode(iResult);
            return FALSE;
        }
        iInd++ ;
        iNextRN = NVRAM_INVALID_RN;
        iResult = nvram_GetNextRN(iId, iRN, &iNextRN);
        if(ERR_SUCCESS == iResult)
        {
        }
        else
        {
            D((DL_NVRAMERROR, "nvram_CheckId: get next rn failed,iId = 0x%x,err_code = %d.", iRN, iResult));
            // nvram_ErrCode(iResult);
            return FALSE;
        }
        iRN = iNextRN;
    }
    if(iRNCount == iInd)
    {
        return TRUE;
    }
    else
    {
        D((DL_NVRAMERROR, "NVRAM_Create: iInd error,iId = 0x%x,iInd = 0x%x,iRNCount = 0x%x.", iId, iInd, iRNCount));
        iResult = _ERR_NVRAM_RN_ERROR;
        return FALSE;
    }
}


VOID nvram_SemInit(VOID)
{

    g_nvram_global_var.hSem = sxr_NewSemaphore(1);
    if(0 == g_nvram_global_var.hSem)
    {
        D( ( DL_WARNING, "nvram_ModuleSemInit failed."));
    }
}


VOID nvram_WaitForSem(VOID)
{

    if((HANDLE)NULL != g_nvram_global_var.hSem)
    {
        sxr_TakeSemaphore(g_nvram_global_var.hSem);
    }
    else
    {
        D( ( DL_WARNING, "nvram_ModuleDown failed."));
    }

}


VOID nvram_ReleaseSem(VOID)
{

    if((HANDLE)NULL != g_nvram_global_var.hSem)
    {
        sxr_ReleaseSemaphore(g_nvram_global_var.hSem);
    }
    else
    {
        D( ( DL_WARNING, "nvram_ModuleUp failed."));
    }
}



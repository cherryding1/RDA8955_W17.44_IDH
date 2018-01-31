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


#include <cswtype.h>
#include <errorcode.h>
// #include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <cos.h>
#include <sul.h>
#include <ts.h>
#include <csw_mem_prv.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_pbk.h"

//
// main steps will be done in this function:
// 1. check input parameters
// 2. call CFW_CfgGetPbkStorage() to get the current phonebook
// 3. call cfw_setUTI(pTransId) to inform system
// 4. check pEntryInfo->phoneIndex is zero or not
// 5. if =0, store pEntryInfo to context and call _SimSeekReq(nStorage) to
// search for the 1st free record.
// 6. else call _SimUpdateRecordReq(phoneIndex,Name+Type+Number)
//
extern  CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;
extern  u8             g_cfw_sim_adn_status[CFW_SIM_COUNT];  // 0: USIM GB ADN OK; 1: only USIM APP ADN; 2: only SIM ADN

UINT8 CFW_SimPbkGetNameLength(UINT8 nStorage, CFW_SIM_ID nSimID)
{
	SIM_PBK_PARAM *pSimPBKPara = NULL;
	UINT8 len = 0;
	CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
	switch(nStorage)
	{
		case CFW_PBK_SIM:
			{
				UINT8 nRecordSize = 0;
				if(CFW_GetSimType(nSimID) == 1)
				{
					nRecordSize = getADNRecordSize(1, nSimID);
				}
				else
				{
					nRecordSize = pSimPBKPara->nADNRecordSize;
				}
				len = nRecordSize - CFW_SIM_REMAINLENGTH;
			}
			break;
		case CFW_PBK_SIM_FIX_DIALLING:
			len = pSimPBKPara->nFDNRecordSize - CFW_SIM_REMAINLENGTH;
			break;
		case CFW_PBK_ON:
			len = pSimPBKPara->nMSISDNRecordSize - CFW_SIM_REMAINLENGTH;
			break;
		case CFW_PBK_SIM_BARRED_DIALLING:
			len = pSimPBKPara->nBDNRecordSize - CFW_SIM_REMAINLENGTH;
			break;
		default:
			break;
	}
	CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "len for nStorage %d, sim %d  =%d ", nStorage,nSimID, len);
	return len;
}
//
// main steps will be done in this function:
// 1. check input parameters
// 2. call CFW_CfgGetPbkStorage() to get the current phonebook
// 3. call cfw_setUTI(pTransId) to inform system
// 4. check pEntryInfo->phoneIndex is zero or not
// 5. if =0, store pEntryInfo to context and call _SimSeekReq(nStorage) to
// search for the 1st free record.
// 6. else call _SimUpdateRecordReq(phoneIndex,Name+Type+Number)
//
UINT32 CFW_SimAddPbkEntry(UINT8 nStorage, CFW_SIM_PBK_ENTRY_INFO *pEntryInfo, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;

    SIM_SM_INFO_ADDPBK *pAddPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimAddPbkEntry);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nStorage  =%d ",0x08100ce9), nStorage);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEntryInfo->iFullNameSize  =%d ",0x08100cea), pEntryInfo->iFullNameSize);
    //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("pEntryInfo->pFullName  =%s ",0x08100ceb), pEntryInfo->pFullName);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEntryInfo->nNumberSize  =%d ",0x08100cec), pEntryInfo->nNumberSize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEntryInfo->pNumber  =%d ",0x08100ced), pEntryInfo->pNumber);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEntryInfo->phoneIndex  =%d ",0x08100cee), pEntryInfo->phoneIndex);
    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
        return Ret;
    }
    if (nStorage == 0)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);

    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
    pAddPBK = (SIM_SM_INFO_ADDPBK *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_ADDPBK));
    if (pAddPBK == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100cef));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }
    SUL_ZeroMemory32(pAddPBK, SIZEOF(SIM_SM_INFO_ADDPBK));

    switch (nStorage)
    {
        case CFW_PBK_SIM:
        {
            UINT8 nRecordSize = 0;
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pAddPBK->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pAddPBK->nCurrentFile = API_USIM_EF_ADN;
                nRecordSize = getADNRecordSize(pEntryInfo->phoneIndex, nSimID);
            }
            else
            {
                pAddPBK->nCurrentFile = API_SIM_EF_ADN;
                nRecordSize = pSimPBKPara->nADNRecordSize;
            }
            if (pEntryInfo->phoneIndex > pSimPBKPara->nADNTotalNum)
            {
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error pEntryInfo->phoneIndex>pSimPBKPara->nADNTotalNum\n",0x08100cf0));
                return ERR_CFW_INVALID_PARAMETER;
            }

            if ((pEntryInfo->nNumberSize > SIM_PBK_NUMBER_SIZE)
                    || (pEntryInfo->iFullNameSize > (nRecordSize - CFW_SIM_REMAINLENGTH)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error pEntryInfo->nNumberSize 0x%x pEntryInfo->iFullNameSize 0x%x\n",0x08100cf1),
                          pEntryInfo->nNumberSize, pEntryInfo->iFullNameSize);

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nRecordSize = %d",0x08100cf2),nRecordSize);
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;
        }
        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pAddPBK->nCurrentFile = API_USIM_EF_FDN;
            else
                pAddPBK->nCurrentFile = API_SIM_EF_FDN;
            if (pEntryInfo->phoneIndex > pSimPBKPara->nFDNTotalNum)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error pEntryInfo->phoneIndex>pSimPBKPara->nFDNTotalNum\n",0x08100cf3));
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if ((pEntryInfo->nNumberSize > SIM_PBK_NUMBER_SIZE)
                    || (pEntryInfo->iFullNameSize > (pSimPBKPara->nFDNRecordSize - CFW_SIM_REMAINLENGTH)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error pEntryInfo->nNumberSize 0x%x pEntryInfo->iFullNameSize 0x%x\n",0x08100cf4),
                          pEntryInfo->nNumberSize, pEntryInfo->iFullNameSize);
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            pAddPBK->nCurrentFile = API_SIM_EF_LND;
            if (pEntryInfo->phoneIndex > pSimPBKPara->nLNDTotalNum)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error pEntryInfo->phoneIndex>pSimPBKPara->nLNDTotalNum\n",0x08100cf5));
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if ((pEntryInfo->nNumberSize > SIM_PBK_NUMBER_SIZE)
                    || (pEntryInfo->iFullNameSize > (pSimPBKPara->nLNDRecordSize - CFW_SIM_REMAINLENGTH)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error pEntryInfo->nNumberSize 0x%x pEntryInfo->iFullNameSize 0x%x\n",0x08100cf6),
                          pEntryInfo->nNumberSize, pEntryInfo->iFullNameSize);

                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_BARRED_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pAddPBK->nCurrentFile = API_USIM_EF_BDN;
            else
                pAddPBK->nCurrentFile = API_SIM_EF_BDN;
            if (pEntryInfo->phoneIndex > pSimPBKPara->nBDNTotalNum)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error pEntryInfo->phoneIndex>pSimPBKPara->nLNDTotalNum\n",0x08100cf7));
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if ((pEntryInfo->nNumberSize > SIM_PBK_NUMBER_SIZE)
                    || (pEntryInfo->iFullNameSize > (pSimPBKPara->nBDNRecordSize - CFW_SIM_REMAINLENGTH)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error pEntryInfo->nNumberSize 0x%x pEntryInfo->iFullNameSize 0x%x\n",0x08100cf8),
                          pEntryInfo->nNumberSize, pEntryInfo->iFullNameSize);
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pAddPBK->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pAddPBK->nCurrentFile = API_SIM_EF_MSISDN;
            if (pEntryInfo->phoneIndex > pSimPBKPara->nMSISDNTotalNum)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error pEntryInfo->phoneIndex>pSimPBKPara->nMSISDNTotalNum\n",0x08100cf9));
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            if ((pEntryInfo->nNumberSize > SIM_PBK_NUMBER_SIZE)
                    || (pEntryInfo->iFullNameSize > (pSimPBKPara->nMSISDNRecordSize - CFW_SIM_REMAINLENGTH)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error pEntryInfo->nNumberSize 0x%x pEntryInfo->iFullNameSize 0x%x\n",0x08100cfa),
                          pEntryInfo->nNumberSize, pEntryInfo->iFullNameSize);
                CSW_SIM_FREE(pAddPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        default:
            CSW_SIM_FREE(pAddPBK);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nStorage 0x%x \n",0x08100cfb), nStorage);
            return ERR_CFW_INVALID_PARAMETER;
    }

    // [[hameina[mod] 2008.10.07:bug 10006, move here from above
    // 对于sim卡上的电话簿，统一改为：仅当number type=CFW_TELNUMBER_TYPE_INTERNATIONAL时
    // 才允许numbersize=0，namesize=0.其它条件均不允许numbersize,namesize同时为0
    if ((pEntryInfo->iFullNameSize == 0) && (pEntryInfo->nNumberSize == 0) && (pEntryInfo->nType != 145)) // niej 080503 number is only '+'
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error NULL\n",0x08100cfc));
        CSW_SIM_FREE(pAddPBK);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
        return ERR_CFW_INVALID_PARAMETER;
    }

    // ]]hameina[mod]2008.10.07

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pAddPBK, CFW_SimPBKAddProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    if (pEntryInfo->nNumberSize > 0)
        SUL_MemCopy8(pAddPBK->pNum, pEntryInfo->pNumber, pEntryInfo->nNumberSize);
    if (pEntryInfo->iFullNameSize > 0)
        SUL_MemCopy8(pAddPBK->pData, pEntryInfo->pFullName, pEntryInfo->iFullNameSize);
    pAddPBK->nNumType  = pEntryInfo->nType;
    pAddPBK->nNameSize = pEntryInfo->iFullNameSize;
    pAddPBK->nNumSize  = pEntryInfo->nNumberSize;
    if (pEntryInfo->phoneIndex != 0)
    {
        pAddPBK->nAddRecInd = pEntryInfo->phoneIndex;
    }
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimAddPbkEntry);
    return (ERR_SUCCESS);
}

//
// main steps will be done in this function:
// 1. check input parameters to make sure nIndex != 0
// 2. call CFW_CfgGetPbkStorage() to get the current phonebook
// 3. call cfw_setUTI(pTransId)
// 4. call _SimUpdateRecordReq(nIndex, "0xFFFFFF....")
//
UINT32 CFW_SimDeletePbkEntry(UINT8 nStorage, UINT16 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0x00000000;

    SIM_SM_INFO_DELEPBK *pDeletPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimDeletePbkEntry);
    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
        return Ret;
    }
    if (nStorage == CFW_PBK_AUTO)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);
    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100cfd));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pDeletPBK = (SIM_SM_INFO_DELEPBK *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_DELEPBK));
    if (pDeletPBK == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100cfe));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }

    SUL_ZeroMemory32(pDeletPBK, SIZEOF(SIM_SM_INFO_DELEPBK));
    switch (nStorage)
    {
        case CFW_PBK_SIM:
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pDeletPBK->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pDeletPBK->nCurrentFile = API_USIM_EF_ADN;
            }
            else
                pDeletPBK->nCurrentFile = API_SIM_EF_ADN;

            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nADNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100cff), nIndex);
                CSW_SIM_FREE(pDeletPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pDeletPBK->nCurrentFile = API_USIM_EF_FDN;
            else
                pDeletPBK->nCurrentFile = API_SIM_EF_FDN;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nFDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d00), nIndex);
                CSW_SIM_FREE(pDeletPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            pDeletPBK->nCurrentFile = API_SIM_EF_LND;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nLNDTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d01), nIndex);
                CSW_SIM_FREE(pDeletPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_BARRED_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pDeletPBK->nCurrentFile = API_USIM_EF_BDN;
            else
                pDeletPBK->nCurrentFile = API_SIM_EF_BDN;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nBDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d02), nIndex);
                CSW_SIM_FREE(pDeletPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pDeletPBK->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pDeletPBK->nCurrentFile = API_SIM_EF_MSISDN;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nMSISDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d03), nIndex);
                CSW_SIM_FREE(pDeletPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        default:
            CSW_SIM_FREE(pDeletPBK);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
            return ERR_CFW_INVALID_PARAMETER;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pDeletPBK, CFW_SimPBKDelProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    pDeletPBK->nDeleRecInd = nIndex;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimDeletePbkEntry);
    return ERR_SUCCESS;
}

UINT8 CFW_GetPbkInfo(CFW_SIM_SEARCH_PBK* pSearchPbk, CFW_SIM_PBK_INFO* pPbkInfo, UINT16 nIndex)
{
    if((pSearchPbk == NULL) || (pPbkInfo == NULL))
    {
        return 0xFF;
    }
    if(pSearchPbk->nMatchRecords < nIndex)
        return 0xFF;
    UINT16 i = 0;
    UINT8* p = pSearchPbk->nData;
    UINT16 offset = 0;
    for(i = 0; i < nIndex - 1; i++)
    {
        offset += 2/* two bytes Index*/ + 1/* one byte type*/;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "--offset = %d, p[offset] = %x", offset, p[offset]);

        offset += p[offset] + 1;
        
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "-1-offset = %d, p[offset] = %x", offset, p[offset]);
        offset += p[offset] + 1;
        
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "-2-offset = %d, p[offset] = %x", offset, p[offset]);
        if((offset & 0x01) != 0)
            offset++;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "--offset = %d, nIndex = %d", offset, nIndex);

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "-->>>offset = %d, nIndex = %d", offset, nIndex);    
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nData, 100, 16);
    
    pPbkInfo->nIndex = *(UINT16*)(p + offset);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pPbkInfo->nIndex = %d", pPbkInfo->nIndex);
    offset += 2;
    pPbkInfo->nType = p[offset++];
    
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pPbkInfo->nType = %d", pPbkInfo->nType);
    
    pPbkInfo->nLengthAl = p[offset++];
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, p + offset, 50, 16);

    memcpy(pPbkInfo->nAlpha, p + offset, pPbkInfo->nLengthAl);
    pPbkInfo->nAlpha[pPbkInfo->nLengthAl] = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pPbkInfo->nLengthAl = %d, pPbkInfo->nAlpha = %s", pPbkInfo->nLengthAl, pPbkInfo->nAlpha);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pPbkInfo->nAlpha = %x", pPbkInfo->nAlpha[0]);
    offset += pPbkInfo->nLengthAl;
    pPbkInfo->nLengthNb = p[offset++];

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, p + offset, 50, 16);
    memcpy(pPbkInfo->nNumber, p + offset, pPbkInfo->nLengthNb);
    pPbkInfo->nNumber[pPbkInfo->nLengthNb] = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pPbkInfo->nLengthNb = %d, pPbkInfo->nNumber = %s", pPbkInfo->nLengthNb, pPbkInfo->nNumber);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pPbkInfo->nNumber = %x", pPbkInfo->nNumber[0]);

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pPbkInfo, 100, 16);
    return 0;
}


// Search SIM phonebook(ADN,FDN,MSISDN,/*BDN*/,SDN,LND) for alpha identifier or number.
// nType should be:
//                  SEEK_SIM_ADN, SEEK_SIM_FDN, SEEK_SIM_MSISDN, 
//                  /*SEEK_SIM_BDN,*/ SEEK_SIM_SDN, SEEK_SIM_LND
// nObject should be:
//                  SEEK_SIM_NAME, SEEK_SIM_NUMBER, SEEK_SIM_USED
//                  that is you want to search for.
// nMode should be:
//                  FIRST_MATCH, ALL_MATCH
// nPattern is what you want to search exactly. when nObject is SEEK_NULL_RECORDER, nPattern will be ignored
// return 0 when successful, otherwise error occurence.
extern UINT8 getADNID(UINT8 nIndex, CFW_SIM_ID nSimID);
UINT32 CFW_SimSearchPbk(UINT8 nType, UINT8 nObject, UINT8 nMode, UINT8* nPattern, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSearchPbk);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "CFW_SimSearchPbk: nType = %x, nObject = %x, nMode =%x, nSimID = %d", nType, nObject, nMode, nSimID);

    if(nSimID >= CFW_SIM_COUNT)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSearchPbk);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "SIM ID ERROR:%d", nSimID);
PARAM_ERROR:  
        return ERR_CFW_INVALID_PARAMETER;
    }
    UINT8 type = CFW_GetSimType(nSimID);
    switch(nType)
    {
        case SEEK_SIM_ADN:
            if(type == 1)
                nType = getADNID(0,nSimID);
            else
                nType = API_SIM_EF_ADN;
            break;
        case SEEK_SIM_FDN:
            if(type == 1)
                nType = API_USIM_EF_FDN;
            else
                nType = API_SIM_EF_FDN;
            break;
        case SEEK_SIM_MSISDN:
            if(type == 1)
                nType = API_USIM_EF_MSISDN;
            else
                nType = API_SIM_EF_MSISDN;
            break;
#if 0
        case SEEK_SIM_BDN:
            if(type == 1)
                nType = API_USIM_EF_BDN;
            else
                nType = API_SIM_EF_BDN;
            break;
#endif
        case SEEK_SIM_SDN:
            if(type == 1)
                nType = API_USIM_EF_SDN;
            else
                nType = API_SIM_EF_SDN;
            break;

        case SEEK_SIM_LND:
            nType = API_SIM_EF_LND;
            break;
           
        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "nType ERROR:%d", nType);
            goto PARAM_ERROR;
    }

    if((nObject != SEEK_SIM_NAME) && (nObject != SEEK_SIM_NUMBER) && (nObject != SEEK_SIM_USED))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "nObject ERROR:%d", nObject);
        goto PARAM_ERROR;
    }
    if((nMode != FIRST_MATCH) && (nMode != ALL_MATCH) && (nMode != WILDCARD_MATCH))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "nMode ERROR:%d", nMode);
        goto PARAM_ERROR;
    }
    if((nMode == WILDCARD_MATCH) && (nObject != SEEK_SIM_NAME))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "nMode(%d) was not match nObject(%d)", nMode, nObject);
        goto PARAM_ERROR;
    }

    UINT8 length = strlen(nPattern);
    if(((nObject == SEEK_SIM_NAME) && (length > SIM_PBK_RECORDER_MAX_LEN)) || ((nObject == SEEK_SIM_NUMBER) && (length > 20)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "length of Pattern ERROR:%d,%d", length, nObject);
        goto PARAM_ERROR;
    }

    SIM_SEARCH_PBK* pSearchPbk = (SIM_SEARCH_PBK*)CSW_SIM_MALLOC(SIZEOF(SIM_SEARCH_PBK));
    if(pSearchPbk == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d05));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSearchPbk);
        return ERR_NO_MORE_MEMORY;
    }
    if(length != 0)
    {
        if(nObject == SEEK_SIM_NUMBER)      //convert to BCD format
        {
            UINT8 bcd[10]={0};
            length = SUL_AsciiToGsmBcd(nPattern, length, bcd);
            if((length == 0)||(length > 10))   //Max length of pattern is 256, and first two bytes reserved for enhanced search for USIM
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "length of Pattern ERROR:%d", length);
                goto PARAM_ERROR;
            }
            memcpy(pSearchPbk->nPattern + 2, bcd, length);
            pSearchPbk->nPattern[length + 2] = 0;
        }
        else if(nObject == SEEK_SIM_NAME)
        {
            memcpy(pSearchPbk->nPattern, nPattern, length);
            pSearchPbk->nPattern[length] = 0;
        }
        else    //search for NULL recorder
        {
            memset(pSearchPbk->nPattern, 0xFF, SIM_PBK_RECORDER_MAX_LEN);
            pSearchPbk->nPattern[SIM_PBK_RECORDER_MAX_LEN] = 0;
        }
    }
    else
    {
        pSearchPbk->nPattern[0] = 0xFF;
        pSearchPbk->nPattern[1] = 0;
    }
    pSearchPbk->nEF = nType;
    pSearchPbk->nMode = nMode;
    pSearchPbk->nObject = nObject;
    pSearchPbk->nState = CFW_SIM_PBK_IDLE;
    
    pSearchPbk->nRecordIndex = 1;
    pSearchPbk->nLastRecordNb = 0;
    pSearchPbk->nRecordLength = 0;
    pSearchPbk->nLengthData = 0;
    pSearchPbk->pPbkInfo = NULL;
    pSearchPbk->nIndexADN = 0;
    pSearchPbk->nLastIndex = 0;
    HAO hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSearchPbk, CFW_SimSearchPbkProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "======CFW_SimSearchPbk start==%d=====", strlen(pSearchPbk->nPattern+2)+2);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSearchPbk);
    return (ERR_SUCCESS);
}

//
// main steps will be done in this function:
// 1. check input parameters to make sure nIndex != 0
// 2. call CFW_CfgGetPbkStorage() to get the current phonebook
// 3. call cfw_setUTI(pTransId)
// 4. call _SimReadRecordReq(nIndex)
//

UINT32 CFW_SimGetPbkEntry(UINT8 nStorage, UINT16 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;

    SIM_SM_INFO_GETPBK *pGetPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPbkEntry);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SimGetPbkEntry: nStorage = %d, nIndex =%d, nSimID = %d",0x08100d04), nStorage, nIndex, nSimID);
    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
        return Ret;
    }
    if (nStorage == CFW_PBK_AUTO)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);

    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);

    pGetPBK = (SIM_SM_INFO_GETPBK *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETPBK));
    if (pGetPBK == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d05));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }

    SUL_ZeroMemory32(pGetPBK, SIZEOF(SIM_SM_INFO_GETPBK));

    switch (nStorage)
    {
        case CFW_PBK_SIM:
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pGetPBK->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pGetPBK->nCurrentFile = API_USIM_EF_ADN;
            }
            else
                pGetPBK->nCurrentFile = API_SIM_EF_ADN;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADN: pGetPBK->nCurrentFile = %d",0x08100d06), pGetPBK->nCurrentFile);

            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nADNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d07), nIndex);

                CSW_SIM_FREE(pGetPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pGetPBK->nCurrentFile = API_USIM_EF_FDN;
            else
                pGetPBK->nCurrentFile = API_SIM_EF_FDN;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nFDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d08), nIndex);

                CSW_SIM_FREE(pGetPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            pGetPBK->nCurrentFile = API_SIM_EF_LND;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nLNDTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d09), nIndex);

                CSW_SIM_FREE(pGetPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pGetPBK->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pGetPBK->nCurrentFile = API_SIM_EF_MSISDN;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nMSISDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d0a), nIndex);

                CSW_SIM_FREE(pGetPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_EN:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pGetPBK->nCurrentFile = API_USIM_EF_ECC;
            else
                pGetPBK->nCurrentFile = API_SIM_EF_ECC;
            if ((0 >= nIndex) || (nIndex > pSimPBKPara->nECCTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid paramter 0x%x\n",0x08100d0b), nIndex);

                CSW_SIM_FREE(pGetPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        default:
            CSW_SIM_FREE(pGetPBK);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Invalid nStorage 0x%x\n",0x08100d0c), nStorage);

            return ERR_CFW_INVALID_PARAMETER;

    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetPBK, CFW_SimPBKEntryProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    pGetPBK->nCurrRecInd = nIndex;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkEntry);
    return (ERR_SUCCESS);
}

//
// main steps will be done in this function:
// 1. check input parameters
// 2. call CFW_CfgGetPbkStorage() to get the current phonebook
// 3. call cfw_setUTI(pTransId)
// 4. call _SimReadRecordReq(nIndexStart)
//

UINT32 CFW_SimListCountPbkEntries(UINT8 nStorage, UINT16 nIndexStart, UINT16 nCount, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0x00000000;

    SIM_SM_INFO_LISTCOUNTPBK *pListCountPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimListCountPbkEntries);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
        return Ret;
    }
    if (nStorage == CFW_PBK_AUTO)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);

    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100d0d));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pListCountPBK = (SIM_SM_INFO_LISTCOUNTPBK *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_LISTCOUNTPBK));
    if (pListCountPBK == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d0e));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }

    SUL_ZeroMemory32(pListCountPBK, SIZEOF(SIM_SM_INFO_LISTCOUNTPBK));
    switch (nStorage)
    {
        case CFW_PBK_SIM:
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pListCountPBK->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pListCountPBK->nCurrentFile = API_USIM_EF_ADN;
            }
            else
                pListCountPBK->nCurrentFile = API_SIM_EF_ADN;

            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nADNTotalNum))
            {
                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListCountPBK->nCurrentFile = API_USIM_EF_FDN;
            else
                pListCountPBK->nCurrentFile = API_SIM_EF_FDN;
            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nFDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nCount %d\n",0x08100d0f), nIndexStart, nCount);

                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SIM  TEST =====>CFW_SimListCountPbkEntries--LND\n",0x08100d10)));
            pListCountPBK->nCurrentFile = API_SIM_EF_LND;
            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nLNDTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nCount %d\n",0x08100d11), nIndexStart, nCount);

                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_BARRED_DIALLING:
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SIM  TEST =====>CFW_SimListCountPbkEntries--BDN\n",0x08100d12)));
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListCountPBK->nCurrentFile = API_USIM_EF_BDN;
            else
                pListCountPBK->nCurrentFile = API_SIM_EF_BDN;
            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nBDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nCount %d\n",0x08100d13), nIndexStart, nCount);

                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_SERVICE_DIALLING:
        case CFW_PBK_SDN:
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SIM  TEST =====>CFW_SimListCountPbkEntries--SDN\n",0x08100d14)));
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListCountPBK->nCurrentFile = API_USIM_EF_SDN;
            else
                pListCountPBK->nCurrentFile = API_SIM_EF_SDN;
            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nSDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nCount %d\n",0x08100d15), nIndexStart, nCount);

                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListCountPBK->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pListCountPBK->nCurrentFile = API_SIM_EF_MSISDN;
            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nMSISDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nCount %d\n",0x08100d16), nIndexStart, nCount);

                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_EN:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListCountPBK->nCurrentFile = API_USIM_EF_ECC;
            else
                pListCountPBK->nCurrentFile = API_SIM_EF_ECC;
            if ((0 >= nIndexStart) || (nCount < 1) || (nCount > pSimPBKPara->nECCTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nCount %d\n",0x08100d17), nIndexStart, nCount);

                CSW_SIM_FREE(pListCountPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        default:
            CSW_SIM_FREE(pListCountPBK);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
            return ERR_CFW_INVALID_PARAMETER;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pListCountPBK, CFW_SimPBKListCountEntryProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    pListCountPBK->nStartRecInd = nIndexStart;
    pListCountPBK->nCurrRecInd  = nIndexStart;
    pListCountPBK->nCount       = nCount;
    pListCountPBK->nStructNum   = 0;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimListCountPbkEntries);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimListPbkEntries(UINT8 nStorage, UINT16 nIndexStart, UINT16 nIndexEnd, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0x00000000;

    SIM_SM_INFO_LISTPBK *pListPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimListPbkEntries);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
        return Ret;
    }
    if (nStorage == CFW_PBK_AUTO)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);

    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100d18));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    if (nIndexEnd < nIndexStart)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error nIndexEnd < nIndexStart\n",0x08100d19));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
        return ERR_CFW_INVALID_PARAMETER;
    }

    pListPBK = (SIM_SM_INFO_LISTPBK *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_LISTPBK));
    if (pListPBK == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d1a));
        return ERR_NO_MORE_MEMORY;  // should be updated by macro

    }

    SUL_ZeroMemory32(pListPBK, SIZEOF(SIM_SM_INFO_LISTPBK));

    switch (nStorage)
    {
        case CFW_PBK_SIM:
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pListPBK->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pListPBK->nCurrentFile = API_USIM_EF_ADN;
            }
            else
                pListPBK->nCurrentFile = API_SIM_EF_ADN;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nADNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d1b), nIndexStart, nIndexEnd);
                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListPBK->nCurrentFile = API_USIM_EF_FDN;
            else
                pListPBK->nCurrentFile = API_SIM_EF_FDN;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nFDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d1c), nIndexStart, nIndexEnd);

                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            pListPBK->nCurrentFile = API_SIM_EF_LND;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nLNDTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d1d), nIndexStart, nIndexEnd);

                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_BARRED_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListPBK->nCurrentFile = API_USIM_EF_BDN;
            else
                pListPBK->nCurrentFile = API_SIM_EF_BDN;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nBDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d1e), nIndexStart, nIndexEnd);

                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_SIM_SERVICE_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListPBK->nCurrentFile = API_USIM_EF_SDN;
            else
                pListPBK->nCurrentFile = API_SIM_EF_SDN;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nSDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d1f), nIndexStart, nIndexEnd);
                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListPBK->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pListPBK->nCurrentFile = API_SIM_EF_MSISDN;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nMSISDNTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d20), nIndexStart, nIndexEnd);
                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        case CFW_PBK_EN:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pListPBK->nCurrentFile = API_USIM_EF_ECC;
            else
                pListPBK->nCurrentFile = API_SIM_EF_ECC;
            if ((0 >= nIndexStart) || (nIndexEnd > pSimPBKPara->nECCTotalNum))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error Invalid paramter nIndexStart 0x%x nIndexEnd %d\n",0x08100d21), nIndexStart, nIndexEnd);
                CSW_SIM_FREE(pListPBK);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
                return ERR_CFW_INVALID_PARAMETER;
            }
            break;

        default:
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("SIM  TEST =====>CFW_SimListPbkEntries--NO\n",0x08100d22)));
            CSW_SIM_FREE(pListPBK);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
            return ERR_CFW_INVALID_PARAMETER;
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pListPBK, CFW_SimPBKListEntryProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    pListPBK->nStartRecInd = nIndexStart;
    pListPBK->nEndRecInd   = nIndexEnd;
    pListPBK->nCurrRecInd  = nIndexStart;
    pListPBK->nStructNum   = 1;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimListPbkEntries);
    return (ERR_SUCCESS);
}

//
// main steps will be done in this function:
// 1. call CFW_CfgGetPbkStorage() to get the current phonebook
// 2. call cfw_setUTI(pTransId)
// 3. call _SimSeekReq(nStorage) to search for the 1st free record
//

UINT32 CFW_SimGetPbkStrorageInfo(UINT8 nStorage, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;

    SIM_SM_INFO_STORAGEINFO *pStorageInfo = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPbkStrorageInfo);
    SIM_PBK_PARAM *pSimPBKPara;

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStrorageInfo);
        return Ret;
    }
    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
    if (nStorage == CFW_PBK_AUTO)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100d23));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStrorageInfo);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pStorageInfo = (SIM_SM_INFO_STORAGEINFO *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_STORAGEINFO));
    if (pStorageInfo == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d24));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStrorageInfo);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }
    SUL_ZeroMemory32(pStorageInfo, SIZEOF(SIM_SM_INFO_STORAGEINFO));

    switch (nStorage)
    {
        case CFW_PBK_SIM:
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pStorageInfo->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pStorageInfo->nCurrentFile = API_USIM_EF_ADN;
            }
            else
                pStorageInfo->nCurrentFile = API_SIM_EF_ADN;
            break;

        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorageInfo->nCurrentFile = API_USIM_EF_FDN;
            else
                pStorageInfo->nCurrentFile = API_SIM_EF_FDN;
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            pStorageInfo->nCurrentFile = API_SIM_EF_LND;
            break;

        case CFW_PBK_SIM_BARRED_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorageInfo->nCurrentFile = API_USIM_EF_BDN;
            else
                pStorageInfo->nCurrentFile = API_SIM_EF_BDN;
            break;

        case CFW_PBK_SIM_SERVICE_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorageInfo->nCurrentFile = API_USIM_EF_SDN;
            else
                pStorageInfo->nCurrentFile = API_SIM_EF_SDN;
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorageInfo->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pStorageInfo->nCurrentFile = API_SIM_EF_MSISDN;
            break;

        case CFW_PBK_EN:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorageInfo->nCurrentFile = API_USIM_EF_ECC;
            else
                pStorageInfo->nCurrentFile = API_SIM_EF_ECC;
            break;

        default:
            CSW_SIM_FREE(pStorageInfo);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStrorageInfo);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nStorage %d\n",0x08100d25), nStorage);

            return ERR_CFW_INVALID_PARAMETER;
    }

    pStorageInfo->nState.nNextState = CFW_SIM_PBK_IDLE;
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pStorageInfo, CFW_SimPBKStorageInfoProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStrorageInfo);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetPbkStorage(UINT8 nStorage, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;

    SIM_SM_INFO_STORAGE *pStorage = NULL;
    SIM_PBK_PARAM *pSimPBKPara;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPbkStorage);

    UINT32 Ret = ERR_SUCCESS;

    Ret = CFW_CheckSimId(nSimID);
    if (Ret != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStorage);
        return Ret;
    }
    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
    if (nStorage == CFW_PBK_AUTO)
        CFW_CfgGetPbkStorage(&nStorage, nSimID);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100d26));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStorage);
        return ERR_CFW_UTI_IS_BUSY;

    }
#endif
    pStorage = (SIM_SM_INFO_STORAGE *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_STORAGE));
    if (pStorage == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d27));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStorage);
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }
    SUL_ZeroMemory32(pStorage, SIZEOF(SIM_SM_INFO_STORAGE));
    switch (nStorage)
    {
        case CFW_PBK_SIM:
            if(CFW_GetSimType(nSimID) == 1)
            {
                if(g_cfw_sim_adn_status[nSimID] == 0)
                    pStorage->nCurrentFile = API_USIM_EF_GB_ADN;
                else if (g_cfw_sim_adn_status[nSimID] == 1)
                    pStorage->nCurrentFile = API_USIM_EF_ADN;
            }
            else
                pStorage->nCurrentFile = API_SIM_EF_ADN;
            break;

        case CFW_PBK_SIM_FIX_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorage->nCurrentFile = API_USIM_EF_FDN;
            else
                pStorage->nCurrentFile = API_SIM_EF_FDN;
            break;

        case CFW_PBK_SIM_LAST_DIALLING:
            pStorage->nCurrentFile = API_SIM_EF_LND;
            break;

        case CFW_PBK_SIM_BARRED_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorage->nCurrentFile = API_USIM_EF_BDN;
            else
                pStorage->nCurrentFile = API_SIM_EF_BDN;
            break;

        case CFW_PBK_SIM_SERVICE_DIALLING:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorage->nCurrentFile = API_USIM_EF_SDN;
            else
                pStorage->nCurrentFile = API_SIM_EF_SDN;
            break;

        case CFW_PBK_ON:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorage->nCurrentFile = API_USIM_EF_MSISDN;
            else
                pStorage->nCurrentFile = API_SIM_EF_MSISDN;
            break;

        case CFW_PBK_EN:
            if(g_cfw_sim_status[nSimID].UsimFlag)
                pStorage->nCurrentFile = API_USIM_EF_ECC;
            else
                pStorage->nCurrentFile = API_SIM_EF_ECC;
            break;

        default:
            CSW_SIM_FREE(pStorage);
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStorage);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error nStorage %d\n",0x08100d28), nStorage);

            return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SimGetPbkStrorage() %d\r\n",0x08100d29)), nStorage);

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pStorage, CFW_SimPBKStorageProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPbkStorage);
    return (ERR_SUCCESS);
}

#if 0 // Remove by lixp at 20090204
// *****************************************
// add by wanghb  for temp use begin  2007.7.27
// *****************************************
UINT32 CFW_SimPBKListCount_TimeOut()  // Stack_MsgMalloc
{
    api_SimUpdateRecordCnf_t *pSimUpdaterecordCnf = CSW_SIM_MALLOC(SIZEOF(api_SimUpdateRecordCnf_t));
    SUL_ZeroMemory8(pSimUpdaterecordCnf, sizeof(api_SimUpdateRecordCnf_t));

    pSimUpdaterecordCnf->Sw1 = 0x93;
    pSimUpdaterecordCnf->Sw2 = 0x00;

    UINT32 nRet =
        CFW_BalSendMessage(CFW_MBX, API_SIM_UPDATERECORD_CNF, pSimUpdaterecordCnf, sizeof(api_SimUpdateRecordCnf_t));
    return nRet;
}

// *****************************************
// add by wanghb  for temp use end  2007.7.27
// *****************************************
#endif


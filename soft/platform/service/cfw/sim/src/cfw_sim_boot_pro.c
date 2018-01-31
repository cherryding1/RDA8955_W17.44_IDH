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

#include <stdio.h>
#include "sxs_lib.h"
#include <cswtype.h>
#include <errorcode.h>
// #include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <ts.h>
#include "csw_debug.h"

#include <base_prv.h>

#include "cfw_sim.h"
#include "cfw_sim_boot.h"
#include "cfw_usim.h"
#include "cfw_sim_mis.h"

#include "hal_host.h"

struct SimADNInfo ADNInfo[CFW_SIM_COUNT][2];        //Store ADN info for USIM, EF ID and Max number of Record, and Length of record
UINT8 getADNID(UINT8 nIndex, CFW_SIM_ID nSimID)
{
    return ADNInfo[nSimID][nIndex].EFID;
}
UINT8 getADNRecordLength(UINT8 nIndex, CFW_SIM_ID nSimID)
{
    return ADNInfo[nSimID][nIndex].RecordSize;
}
UINT8 getADNMaxRecord(UINT8 nIndex, CFW_SIM_ID nSimID)
{
    return ADNInfo[nSimID][nIndex].MaxRecords;
}

CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] = {{0}, };
UINT8 g_cfw_sim_adn_status[CFW_SIM_COUNT] = {0, };  // 0: USIM GB ADN OK; 1: only USIM APP ADN; 2: only SIM ADN
CFW_SIM_STATUS gSimStatus[CFW_SIM_COUNT] = { CFW_SIM_STATUS_END, CFW_SIM_STATUS_END };
CFW_SPN_INFO gSPNInfo[CFW_SIM_COUNT] = {{0x00, 0x00, {0x00,}}, {0x00, 0x00, {0x00,}}};
CFW_UsimAid gUsimAid[CFW_SIM_COUNT] ;
CFW_UsimAidLabel  gUsimLabel[CFW_SIM_COUNT];
CFW_UsimEfStatus gUsimEfStatus[CFW_SIM_COUNT];
CFW_UsimPin gUsimPin[CFW_SIM_COUNT];
UINT8 nICCID[CFW_SIM_COUNT][ICCID_LENGTH]= {{0},};
UINT8 gTestSim[CFW_SIM_COUNT] = {0,0};

VOID SetTestSim(UINT8 nSimID, UINT8 isTestSim)
{
    gTestSim[nSimID] = isTestSim;
}

UINT8 isTestSim(UINT8 nSimID)
{
    return (gTestSim[nSimID] == 1)?1:0;
}

UINT8* CFW_GetICCID(CFW_SIM_ID nSimID)
{
    if(nICCID[nSimID][0] != 0)
        return nICCID[nSimID];
    else
        return 0;
}

VOID CFW_SetSimStatus(CFW_SIM_ID nSimID, CFW_SIM_STATUS nSimStatus)
{
    if (CFW_SIM_END > nSimID)
    {
        gSimStatus[nSimID] = nSimStatus;
    }
}

CFW_SIM_STATUS CFW_GetSimStatus(CFW_SIM_ID nSimID)
{
    if (CFW_SIM_END > nSimID)
    {
        return gSimStatus[nSimID];
    }
    return CFW_SIM_STATUS_END;
}
VOID CFW_InitVar(CFW_SIM_ID nSimID)
{
    gSimStatus[nSimID] = CFW_SIM_ABSENT;
    memset(&gUsimAid[nSimID],           0x0,  sizeof(CFW_UsimAid));
    memset(&gUsimLabel[nSimID],             0x0,  sizeof(CFW_UsimAidLabel));
    memset(&gUsimEfStatus[nSimID],      0x0,  sizeof(CFW_UsimEfStatus));
    memset(&gUsimPin[nSimID],                 0x0,  sizeof(CFW_UsimPin));
}

UINT32 CFW_GetSimSPN(CFW_SIM_ID nSimID, CFW_SPN_INFO *pSPNInfo)
{
    SUL_MemCopy8(pSPNInfo, &(gSPNInfo[nSimID]), SIZEOF(CFW_SPN_INFO));

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSPNInfo.nSPNDisplayFlag 0x%x \n",0x08100ab7), pSPNInfo->nSPNDisplayFlag);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSPNInfo.nSPNameLen 0x%x \n",0x08100ab8), pSPNInfo->nSPNameLen);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("nSpnName %s \n",0x08100ab9), pSPNInfo->nSpnName);

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSPNInfo->nSpnName, pSPNInfo->nSPNameLen, 16);

    return ERR_SUCCESS;
}


UINT32 CFW_SetSimSPN(CFW_SIM_ID nSimID, CFW_SPN_INFO nSPNInfo)
{
    SUL_MemCopy8(&(gSPNInfo[nSimID]), &nSPNInfo, SIZEOF(CFW_SPN_INFO));

    return ERR_SUCCESS;
}


static UINT32 CFW_GetSPNLen(UINT8 nSPN[16])
{
    UINT8 i = 0x00;
    if(nSPN[0] > 0x7F)
    {
        //UCS2
        UINT16 *p = (UINT16 *)nSPN;
        for(; i < 8; i++)
        {
            if(*p == 0xFFFF)
                break;
        }
    }
    else
    {
        for(; i < 16; i++)
        {
            if(nSPN[i] == 0xFF)
                break;
        }
    }
    return i;
}

BOOL CFW_GetSimType(CFW_SIM_ID nSimID)
{
    return g_cfw_sim_status[nSimID].UsimFlag;
}

u8 CFW_GetSimADNType(CFW_SIM_ID nSimID)
{
    return g_cfw_sim_adn_status[nSimID];
}

UINT32 CFW_GetPBKTotalNum(CFW_SIM_ID nSimID)
{
    SIM_PBK_PARAM *pG_Pbk_Param = NULL;
    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    if(pG_Pbk_Param)
        return pG_Pbk_Param->nADNTotalNum;
    else
        return 0;
}

UINT32 CFW_GetPBKRecordNum(CFW_SIM_ID nSimID)
{
    SIM_PBK_PARAM *pG_Pbk_Param = NULL;
    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    if(pG_Pbk_Param)
        return pG_Pbk_Param->nADNRecordNum;
    else
        return 0;
}


//Test AT Command
/*
//EF_MBDN  6FC7         56
AT+CRSM=192,28615,0,0,32

//USIM_EF_LI  6F05          57
AT+CRSM=192,28421,0,0,32

//USIM_EF_NAFKCA  6FDD  137
AT+CRSM=192,28637,0,0,32

//USIM_EF_MEXE_ST   4F40    138
AT+CRSM=192,20288,0,0,32

//USIM_EF_DIR   2F00        146
AT+CRSM=192,12032,0,0,32

//USIM_EF_ARR_MF            147
AT+CRSM=192,12038,0,0,32

//USIM_EF_KC                142
AT+CRSM=192,20256,0,0,32

//USIM_EF_GB_PBR                148
AT+CRSM=192,20272,0,0,32

//USIM_EF_GB_IAP                149
AT+CRSM=192,20273,0,0,32

//USIM_EF_GB_PSC            171
AT+CRSM=192,20258,0,0,32

//USIM_EF_GB_PUID       173
AT+CRSM=192,20260,0,0,32
*/
static UINT8 UsimFirstEF =0xFF;
static UINT8 UsimFirst4F =0xFF;
UINT32 CFW_GetStackSimFileID(UINT16 n3GppFileID, UINT16 EFPath, CFW_SIM_ID nSimID)
{
#define SIM_CARD            0
#define USIM_CARD         1

#include "itf_sim.h"

    extern EfAddress_t Usim_ADNAddress[STK_MAX_SIM][EF_EID - USIM_EF_GB_PBR];
    //bSim == 0, SIM
    //              1, USIM
    BOOL bSim = CFW_GetSimType(nSimID);

//  Micro NB_ELEM_FILE is the max number of SIM EF Files
    EfAddress_t* pEF = sim_ElementaryFileAddress;
#if 0
    UINT8 j = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("EF ID :",0x08100aba));
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("======================================",0x08100abb));
    for(j = 0; j <USIM_EF_GB_PBR; j++)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("%02X%02X",0x08100abc), sim_ElementaryFileAddress[j].EfAddress[0],sim_ElementaryFileAddress[j].EfAddress[1]);
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("================USIM ADN======================",0x08100abd));
    for(j = 0; j < EF_EID - USIM_EF_GB_PBR; j++)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("%02X%02X",0x08100abe), Usim_ADNAddress[nSimID][j].EfAddress[0],Usim_ADNAddress[nSimID][j].EfAddress[1]);
    }
#endif

    UINT8 i = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("n3GppFileID = %x,EFPath = %x",0x08100abf), n3GppFileID,EFPath);
    //Following Three EF have the same ID both SIM and USIM
    if(n3GppFileID == 0x2F00)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("EF ID = %d",0x08100ac0), USIM_EF_DIR);
        return USIM_EF_DIR;
    }
    else if(n3GppFileID == 0x2F05)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("EF ID = %d",0x08100ac1), EF_ELP);
        return EF_ELP;
    }
    else if(n3GppFileID == 0x2F06)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("EF ID = %d",0x08100ac2), USIM_EF_ARR_MF);
        return USIM_EF_ARR_MF;
    }
    else    if(n3GppFileID == 0x2FE2)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("EF ID = %d",0x08100ac3), EF_ICCID);
        return EF_ICCID;
    }
    else if(n3GppFileID == 0x6FDE)
        return USIM_EF_SPNI;
    else if(n3GppFileID == 0x6FDF)
        return USIM_EF_PNNI;
    else if((n3GppFileID > 0x6FE1) && (n3GppFileID < 0x6FE5))
    {
        return USIM_EF_NCPIP + n3GppFileID - 0x6FE2;
    }
    else if((n3GppFileID > 0x6FE5) && (n3GppFileID < 0x6FE9))
    {
        return USIM_EF_UFC + n3GppFileID - 0x6FE6;
    }

    //Try to find the First EF of USIM
    if(UsimFirstEF == 0xFF)
    {
        for(i = 0; i<NB_ELEM_FILE; i++)
        {
            UINT16 EFID = (pEF[i].DfAddress[0]<<8) | pEF[i].DfAddress[1];
            if(EFID == 0x7FFF)
                break;                  //offset of USIM
        }

        if(i < NB_ELEM_FILE)
            UsimFirstEF = i;
        else
            return 0xFFFF;                  //The table has a problem
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("UsimFirstEF = %d",0x08100ac4),UsimFirstEF);

    if(bSim == SIM_CARD)                    //SIM card
    {
        for(i = 0; i<UsimFirstEF; i++)
        {
            UINT16 EFID = (pEF[i].EfAddress[0]<<8) | pEF[i].EfAddress[1];
            if(EFID == n3GppFileID)
                break;
        }

        if(i < UsimFirstEF)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SIM EF ID = %d",0x08100ac5), i);
            return i;
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The EF %x cannot be found!",0x08100ac6),n3GppFileID);
            return 0xFFFF;
        }
    }
    else                                    //This is a USIM card
    {
        if(UsimFirst4F == 0xFF)
        {
            i = UsimFirstEF;
            while((pEF[i].EfAddress[0] == 0x6F) && (i < NB_ELEM_FILE))
                i++;                    //offset of USIM 4F

            if(i < NB_ELEM_FILE)
                UsimFirst4F = i;
            else
                return 0xFFFF;
        }

        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("UsimFirst4F = %d",0x08100ac7),UsimFirst4F);
        if((n3GppFileID >> 8) == 0x6F)        //USIM 6F file
        {
            if((n3GppFileID & 0xFF) == 0x3A)    //in USIM, the id of ADN should be 4Fxx, but STK will issue a commnd to update 6F3A.
                return EF_ADN;
            for(i = UsimFirstEF; i<UsimFirst4F; i++)
            {
                UINT16 EFID = (pEF[i].EfAddress[0]<<8) | pEF[i].EfAddress[1];
                if(EFID == n3GppFileID)
                    break;
            }

            if(i < UsimFirst4F)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("USIM EF ID = %d",0x08100ac8), i);
                return i;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The EF %x cannot be found!",0x08100ac9),n3GppFileID);
                return 0xFFFF;
            }
        }
        else        //USIM  4F
        {
            if(EFPath == 0x5F3A)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0x5F3A",0x08100aca));
                //  bAdnType == 0, it is a global ADN
                //                  == 1, it is a ADF ADN
                UINT8 nAdnType = CFW_GetSimADNType(nSimID);
                pEF = Usim_ADNAddress[nSimID];
                if(nAdnType == 0)
                    nAdnType = USIM_EF_GB_PBR;
                else
                    nAdnType = USIM_EF_PBR;

                for(i = nAdnType; i  <NB_ELEM_FILE; i++)
                {
                    UINT16 EFID = (pEF[i -USIM_EF_GB_PBR].EfAddress[0]<<8) | pEF[i - USIM_EF_GB_PBR].EfAddress[1];
                    //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("----Look for EFID(%x)----",0x08100acb), EFID);
                    if(EFID == n3GppFileID)
                        break;
                }

                if(i < NB_ELEM_FILE)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("EF ID in DFphonebook = %d",0x08100acc), i);
                    return i;           //Return File ID
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("The EF %x cannot be found!",0x08100acd),n3GppFileID);
                    return 0xFFFF;      //Cannot find the EF.
                }
            }
            else if(EFPath == 0x5F3B)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0x5F3B",0x08100ace));
                if(n3GppFileID == 0x4F20)
                    return USIM_EF_KC;
                else if(n3GppFileID == 0x4F52)
                    return USIM_EF_KC_GPRS;
                else if(n3GppFileID == 0x4F63)
                    return USIM_EF_CPBCCH;
                else if(n3GppFileID == 0x4F64)
                    return USIM_EF_INVSCAN;
                else
                    return 0xFFFF;
            }
            else if(EFPath == 0x5F3C)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("0x5F3C",0x08100acf));
                if(n3GppFileID == 0x4F40)
                    return USIM_EF_MEXE_ST;
                else if(n3GppFileID == 0x4F42)
                    return USIM_EF_ARPK;
                else if(n3GppFileID == 0x4F43)
                    return USIM_EF_TPRK;
                else if(n3GppFileID == 0x4F41)
                    return USIM_EF_ORPK;
                else
                    return 0xFFFF;
            }
            else
                return 0xFFFF;

        }
    }
}

static UINT8 gMNCLen[CFW_SIM_COUNT] = {0x00,};

VOID CFW_SetMNCLen(UINT8 nLen, CFW_SIM_ID nSimID)
{
    gMNCLen[nSimID] = nLen;
}

VOID CFW_GetMNCLen(UINT8 *pLen, CFW_SIM_ID nSimID)
{
    *pLen = gMNCLen[nSimID] ;
}

UINT32 CFW_SimCloseProc(HAO hAo, CFW_EV *pEvent)
{
    CFW_SIM_ID nSimID;
    CFW_GetSimID(hAo, &nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("===========CFW_SimCloseProc===========",0x08100ad0));
    if(pEvent == (UINT32)(-1))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send API_SIM_CLOSE_IND to stack, and waiting API_SIM_CLOSE_CNF",0x08100ad1));
        CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID);
    }
    else if(pEvent->nEventId == API_SIM_CLOSE_CNF)
    {
        //api_SimCloseCnf
        UINT32 nUTI;
        CFW_GetUTI(hAo, &nUTI);

        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_SIM_CLOSE_RSP to MMI!",0x08100ad2));
        CFW_PostNotifyEvent(EV_CFW_SIM_CLOSE_RSP, 0, 0, nUTI | (nSimID << 24), 0);
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimInitStage3Proc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStage3Proc);

    CFW_SIMINITS3 *pSimInitS3Data = NULL;


    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pSimInitS3Data = CFW_GetAoUserData(hSimInitializeAo);

    switch (pSimInitS3Data->nSm_SimInitS3_currStatus)
    {
        case SM_SIMINITS3_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;

                pSimInitS3Data->nEF = API_SIM_EF_SPN;
                Result = SimReadBinaryReq(API_SIM_EF_SPN, 0x00, 17, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq EF_SPN return 0x%x \n",0x08100ad3), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF, GENERATE_SHELL_UTI() | (nSimID << 24),
                                        SIM_INIT_EV_STAGE3);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                    return ERR_SUCCESS;
                }

                pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_READBINARY;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS3_ELEMENTARY:
        {
            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                SIM_PBK_PARAM *pG_Pbk_Param;
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

                CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("ELEMFILESTATUS SW1[0x%x] nEF[0x%x]\n",0x08100ad4), pSimElemFileStatusCnf->Sw1, pSimInitS3Data->nEF);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimInitS3Data->nEF %d\n",0x08100ad5), pSimInitS3Data->nEF);

                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2))
                        || (0x91 == pSimElemFileStatusCnf->Sw1)
                        || ((0x98 == pSimElemFileStatusCnf->Sw1) && (0x10 == pSimElemFileStatusCnf->Sw2)))
                {
                    pSimInitS3Data->nFileSize   = ((UINT16)pSimElemFileStatusCnf->Data[2] << 8) | (pSimElemFileStatusCnf->Data[3]);
                    pSimInitS3Data->nRecordSize = pSimElemFileStatusCnf->Data[14];

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nRecordSize 0x%x\n",0x08100ad6), pSimInitS3Data->nRecordSize);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimInitS3Data->nEF %d\n",0x08100ad7), pSimInitS3Data->nEF);

                    if ((pSimInitS3Data->nEF == API_SIM_EF_AD) || (pSimInitS3Data->nEF == API_SIM_EF_SST))
                    {
                        //if (pSimInitS3Data->nFileSize != 0)
                        {
                            UINT32 Result = SimReadBinaryReq(pSimInitS3Data->nEF, 0, (UINT8)pSimInitS3Data->nFileSize, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100ad8), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }

                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_READBINARY; // updated by wanghb-20070711
                            CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                            return ERR_SUCCESS;
                        }
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_LOCI)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Data[11] & 0x01 = 0x%x \n",0x08100ad9), (pSimElemFileStatusCnf->Data[11] & 0x01));

                        if (((pSimElemFileStatusCnf->Data[11] & 0x01) == 1)
                                && (0x98 != pSimElemFileStatusCnf->Sw1))
                        {
                            UINT32 Result = 0;

                            pSimInitS3Data->nEF = API_SIM_EF_IMSI;
                            Result            = SimElemFileStatusReq(API_SIM_EF_IMSI, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100ada), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }
                        }
                        else
                        {
                            UINT32 Result = 0;

                            CFW_PROFILE *pMeProfile = NULL;
                            CFW_SimGetMeProfile(&pMeProfile, nSimID);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pMeProfile->nStart_Profile= 0x%x \n",0x08100adb), pMeProfile->nStart_Profile);
                            if ((pMeProfile->nStart_Profile & 0x04) == 0x04)  // ME support CC.
                            {
                                if ((pMeProfile->nStart_Profile & 0x03) == 0x03)
                                {
                                    pSimInitS3Data->nMEStatus = 1;  // 1: ALL SERVICES ENABLED
                                }
                                else
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error SIM Card Abnormity\n",0x08100adc));
                                    SIM_MISC_PARAM *pG_Misc_Param = NULL;
                                    CFW_CfgSimGetMicParam(&pG_Misc_Param, nSimID);
                                    pG_Misc_Param->nSimMode = CFW_SIM_CARD_ABNORMITY;
                                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF,
                                                        GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3);
                                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                    return ERR_SUCCESS;
                                }
                            }
                            else
                            {
                                if (((pMeProfile->nStart_Profile & 0x01) == 0x01) && (pG_Pbk_Param->bADNStatus == EF_DISABLE))
                                {
                                    pSimInitS3Data->nMEStatus = 0;
                                }
                                else
                                {
                                    SIM_MISC_PARAM *pG_Misc_Param = NULL;
                                    CFW_CfgSimGetMicParam(&pG_Misc_Param, nSimID);
                                    pG_Misc_Param->nSimMode = CFW_SIM_CARD_ABNORMITY;
                                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF,
                                                        GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3);
                                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                    return ERR_SUCCESS;
                                }
                            }
                            Result = SimRehabilitateReq(API_SIM_EF_LOCI, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(1), TSTR("Error SimRehabilitateReq return 0x%x \n",0x08100add), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_REHABILITATE;
                        }


                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                        return ERR_SUCCESS;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_IMSI)
                    {

                        if (((pSimElemFileStatusCnf->Data[11] & 0x01) == 1)   && (0x98 != pSimElemFileStatusCnf->Sw1))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("IMSI[11] 0x%x\n",0x08100ade), pSimElemFileStatusCnf->Data[11]);
                            if (CFW_SimGetPS() == TRUE)
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_ME_PS, 0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                    SIM_INIT_EV_STAGE3);
                            else if (CFW_SimGetPF() == TRUE)
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_ME_PF, 0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                    SIM_INIT_EV_STAGE3);
                            else
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Normal Sim Card %d\n",0x08100adf), nSimID);
                                SIM_MISC_PARAM *pG_Mic_Param;
                                CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                                CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Mic_Param->nSimMode,  // modify by wanghb from CFW_SIM_CARD_NORMAL to pG_Mic_Param->nSimMode
                                                    pG_Pbk_Param->bBDNStatus << 1 | pG_Pbk_Param->bADNStatus,
                                                    GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_GSM);
                            }
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                        else
                        {
                            UINT32 Result = SimRehabilitateReq(API_SIM_EF_IMSI, nSimID);

                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(1), TSTR("Error SimRehabilitateReq return 0x%x \n",0x08100ae0), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_REHABILITATE;

                        }
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                        return ERR_SUCCESS;
                    }
                    else if (pSimInitS3Data->nRecordSize != 0)
                        pSimInitS3Data->nFileSize = pSimInitS3Data->nFileSize / pSimInitS3Data->nRecordSize;

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimInitS3Data->nEF 0x%x\n",0x08100ae1), pSimInitS3Data->nEF);
                    if (pSimInitS3Data->nEF == API_SIM_EF_ADN)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADN nRecordSize 0x%x\n",0x08100ae2), pSimInitS3Data->nRecordSize);

                        pG_Pbk_Param->nADNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nADNTotalNum   = pSimInitS3Data->nFileSize;
                        pG_Pbk_Param->bADNStatus     = pSimElemFileStatusCnf->Data[11] & 0x01;  // 1: EF_ADN 0 :EF_FDN
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADN Status 0x%x\n",0x08100ae3), pG_Pbk_Param->bADNStatus);

                        pSimInitS3Data->nEF = API_SIM_EF_FDN;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_FDN)
                    {
                        pG_Pbk_Param->nFDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nFDNTotalNum   = (UINT8)pSimInitS3Data->nFileSize;
                        pSimInitS3Data->nEF          = API_SIM_EF_BDN;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_BDN)
                    {
                        //
                        // Modify by wangxj at 20060819
                        //
                        pG_Pbk_Param->nBDNRecordSize = pSimInitS3Data->nRecordSize;

                        pG_Pbk_Param->nBDNTotalNum = (UINT8)pSimInitS3Data->nFileSize;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("API_SIM_EF_BDN nBDNTotalNum %d\n",0x08100ae4), pG_Pbk_Param->nBDNTotalNum);
                        pG_Pbk_Param->bBDNStatus = pSimElemFileStatusCnf->Data[11] & 0x01;  // 1: enable 0 :disable
                        pSimInitS3Data->nEF      = API_SIM_EF_LND;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_LND)
                    {
                        //
                        // Modify by wangxj at 20060819
                        //
                        pG_Pbk_Param->nLNDRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nLNDTotalNum   = (UINT8)pSimInitS3Data->nFileSize;
                        pSimInitS3Data->nEF          = API_SIM_EF_SDN;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_SDN)
                    {
                        pG_Pbk_Param->nSDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nSDNTotalNum   = (UINT8)pSimInitS3Data->nFileSize;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("API_SIM_EF_SDN nSDNTotalNum %d\n",0x08100ae5), pG_Pbk_Param->nSDNTotalNum);
                        pSimInitS3Data->nEF = API_SIM_EF_MSISDN;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_MSISDN)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("EF_MSISDN\n",0x08100ae6));
                        pG_Pbk_Param->nMSISDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nMSISDNTotalNum   = (UINT8)pSimInitS3Data->nFileSize;
                        pSimInitS3Data->nEF             = API_SIM_EF_ECC;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_ECC)
                    {
                        pG_Pbk_Param->nECCRecordSize = 3; // pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nECCTotalNum   = pSimInitS3Data->nFileSize;
                        pSimInitS3Data->nEF          = API_SIM_EF_LOCI;
                    }
                }
                else if(pSimInitS3Data->nEF == API_SIM_EF_AD)
                {
                    SIM_MISC_PARAM *pG_Mic_Param;

                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    pG_Mic_Param->nSimMode = CFW_SIM_CARD_NORMAL;
                    SetTestSim(nSimID, 0);
                    CFW_SetSimStatus(nSimID, CFW_SIM_NORMAL);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("CFW_SIM_NORMAL Maybe Phase1 SIM ELEMFILESTATUS SW1[0x%x] SW2[0x%x]nEF[0x%x]\n",0x08100ae7), pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, pSimInitS3Data->nEF);

                    pSimInitS3Data->nEF = API_SIM_EF_SST;
                }
                else if ((pSimInitS3Data->nEF == API_SIM_EF_SST) ||
                         (pSimInitS3Data->nEF == API_SIM_EF_LOCI) || (pSimInitS3Data->nEF == API_SIM_EF_IMSI))
                {

                    SIM_MISC_PARAM *pG_Mic_Param;

                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    pG_Mic_Param->nSimMode = CFW_SIM_CARD_BAD;
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_BAD, 0, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3); // need to add error code to param2.
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                    return ERR_SUCCESS;
                }

                // else if(pSimInitS3Data->nEF == API_SIM_EF_AD)
                // {
                // pSimInitS3Data->nEF = API_SIM_EF_SST;
                // }
                // else if(pSimInitS3Data->nEF == API_SIM_EF_SST)
                // {
                // pSimInitS3Data->nEF = API_SIM_EF_ADN;
                // }
                else if (pSimInitS3Data->nEF == API_SIM_EF_ADN)
                {
                    pG_Pbk_Param->nADNRecordSize = 0;
                    pG_Pbk_Param->nADNTotalNum   = 0;
                    pG_Pbk_Param->bADNStatus     = EF_DISABLE;
                    pSimInitS3Data->nEF          = API_SIM_EF_FDN;
                }
                else if (pSimInitS3Data->nEF == API_SIM_EF_FDN)
                {
                    pG_Pbk_Param->nFDNRecordSize = 0;
                    pG_Pbk_Param->nFDNTotalNum   = 0;
                    pSimInitS3Data->nEF          = API_SIM_EF_BDN;
                }
                else if (pSimInitS3Data->nEF == API_SIM_EF_BDN)
                {
                    pG_Pbk_Param->bBDNStatus = EF_DISABLE;
                    pSimInitS3Data->nEF      = API_SIM_EF_LND;
                }
                else if (pSimInitS3Data->nEF == API_SIM_EF_LND)
                {
                    pG_Pbk_Param->nLNDRecordSize = 0;
                    pG_Pbk_Param->nLNDTotalNum   = 0;

                    //
                    // Modify by wangxj at 20060819
                    //
                    pSimInitS3Data->nEF = API_SIM_EF_SDN;
                }
                else if (pSimInitS3Data->nEF == API_SIM_EF_SDN)
                {
                    pG_Pbk_Param->nSDNRecordSize = 0;
                    pG_Pbk_Param->nSDNTotalNum   = 0;
                    pSimInitS3Data->nEF          = API_SIM_EF_MSISDN;
                }
                else if (pSimInitS3Data->nEF == API_SIM_EF_MSISDN)
                {
                    pG_Pbk_Param->nMSISDNRecordSize = 0;
                    pG_Pbk_Param->nMSISDNTotalNum   = 0;
                    pSimInitS3Data->nEF             = API_SIM_EF_ECC; // This will be remove.
                }
                else if (pSimInitS3Data->nEF == API_SIM_EF_ECC)
                {
                    pG_Pbk_Param->nECCRecordSize = 0;
                    pG_Pbk_Param->nECCTotalNum   = 0;
                    pSimInitS3Data->nEF          = API_SIM_EF_LOCI;  // This will be remove.
                }

                UINT32 Result = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);

                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100ae8), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                    return ERR_SUCCESS;
                }


                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS3_READBINARY:
        {

            if (nEvtId == API_SIM_READBINARY_CNF)
            {
                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("api_SimReadBinaryCnf_t: SW1 %x,SW2 %x \n",0x08100ae9), pSimReadBinaryCnf->Sw1,
                          pSimReadBinaryCnf->Sw2);
                if ((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2))
                {
                    SIM_MISC_PARAM *pG_Mic_Param;
                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("Stage3 SM_SIMINITS3_READBINARY nEF(%s)\n",0x08100aea), TS_GetEFFileName(pSimInitS3Data->nEF));

                    if (pSimInitS3Data->nEF == API_SIM_EF_SPN)
                    {

                        CFW_SPN_INFO nSPNInfo = {0x00, 0x00, {0x00,}};
                        nSPNInfo.nSPNameLen = CFW_GetSPNLen(&(pSimReadBinaryCnf->Data[1]));
                        nSPNInfo.nSPNDisplayFlag = pSimReadBinaryCnf->Data[0] & 0x01;

                        SUL_MemCopy8(nSPNInfo.nSpnName, &(pSimReadBinaryCnf->Data[1]), nSPNInfo.nSPNameLen);
                        CFW_SetSimSPN(nSimID, nSPNInfo);

                        UINT32 Result = 0;

                        pSimInitS3Data->nEF = API_SIM_EF_AD;
                        Result = SimElemFileStatusReq(API_SIM_EF_AD, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq EF_AD return 0x%x \n",0x08100aeb), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_AD)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimReadBinaryCnf->Data[0] 0x%x \n",0x08100aec), pSimReadBinaryCnf->Data[0]);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The size of AD EF in SIM = %d",pSimInitS3Data->nFileSize);
                        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pSimReadBinaryCnf->Data, pSimInitS3Data->nFileSize, 16);

                        UINT8 lengthMNC = 0xFF;
                        if( pSimInitS3Data->nFileSize >= 4 )
                            lengthMNC = pSimReadBinaryCnf->Data[3];
                        CFW_SetMNCLen(lengthMNC, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The length of MNC = %d", lengthMNC);

                        if (0x00 == pSimReadBinaryCnf->Data[0])
                            SetTestSim(nSimID, 0);
                        else
                            SetTestSim(nSimID, 1);

                        if ((0x00 == pSimReadBinaryCnf->Data[0]) || (0xFF == pSimReadBinaryCnf->Data[0]) || (0x01 == pSimReadBinaryCnf->Data[0])|| (0x04 == pSimReadBinaryCnf->Data[0]))
                        {
                            UINT32 Result = 0;

                            pG_Mic_Param->nSimMode = CFW_SIM_CARD_NORMAL;

                            CFW_SetSimStatus(nSimID, CFW_SIM_NORMAL);

                            pSimInitS3Data->nEF = API_SIM_EF_SST;
                            Result              = SimElemFileStatusReq(API_SIM_EF_SST, nSimID);

                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100aed), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        }
                        else if ((pSimReadBinaryCnf->Data[0] == 0x80) || (pSimReadBinaryCnf->Data[0] == 0x81))// || (pSimReadBinaryCnf->Data[0] == 0x04)
                        {
                            //Add for GCF test by XP 20130913
                            pG_Mic_Param->nSimMode = CFW_SIM_CARD_TEST;
                            CFW_SetSimStatus(nSimID, CFW_SIM_TEST);

                            UINT32 Result = 0;

                            pSimInitS3Data->nEF = API_SIM_EF_SST;
                            Result              = SimElemFileStatusReq(API_SIM_EF_SST, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100aee), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        }
                        else
                        {
                            CFW_SetSimStatus(nSimID, CFW_SIM_ABNORMAL);
                            pG_Mic_Param->nSimMode = CFW_SIM_CARD_ABNORMITY;
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_SST)
                    {
                        UINT8 i       = 0;
                        UINT32 Result = 0;

                        if (pSimInitS3Data->nFileSize > SERVICES_NUM)
                            pSimInitS3Data->nFileSize = SERVICES_NUM;

                        for (i = 0; i < pSimInitS3Data->nFileSize; i++)
                            pG_Mic_Param->nServices[i] = pSimReadBinaryCnf->Data[i];

                        pG_Mic_Param->nServicesSize = pSimInitS3Data->nFileSize;

                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("SST services total number %d \n",0x08100aef), pG_Mic_Param->nServicesSize);

                        CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pG_Mic_Param->nServices, pG_Mic_Param->nServicesSize, 16);

                        if(0x03 == ( pSimReadBinaryCnf->Data[4] & 0x03 ))
                        {
                            CFW_SPN_INFO nSPNInfo = {0x00, 0x00, {0x00,}};

                            CFW_GetSimSPN( nSimID, &nSPNInfo );

                            if( nSPNInfo.nSPNameLen > 0x00 )
                            {
                                nSPNInfo.nSPNDisplayFlag = 0x01;
                            }

                            CFW_SetSimSPN( nSimID, nSPNInfo );
                        }
                        pSimInitS3Data->nEF = API_SIM_EF_ADN;
                        Result              = SimElemFileStatusReq(API_SIM_EF_ADN, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100af0), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                    }
                }
                else
                {
                    if(pSimInitS3Data->nEF == API_SIM_EF_SPN)
                    {
                        UINT32 Result = 0;

                        pSimInitS3Data->nEF = API_SIM_EF_AD;
                        Result = SimElemFileStatusReq(API_SIM_EF_AD, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq EF_AD return 0x%x \n",0x08100af1), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                    }
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS3_REHABILITATE:
        {
            if (nEvtId == API_SIM_REHABILITATE_CNF)
            {
                api_SimRehabilitateCnf_t *SimRehabilitateCnf = (api_SimRehabilitateCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("SimRehabilitateCnf: SW1 %x,SW2 %x \n",0x08100af2), SimRehabilitateCnf->Sw1,
                          SimRehabilitateCnf->Sw2);


                if ((0x90 == SimRehabilitateCnf->Sw1) && (0x00 == SimRehabilitateCnf->Sw2))
                {
                    if (pSimInitS3Data->nEF == API_SIM_EF_LOCI)
                    {
                        UINT32 Result = 0;

                        pSimInitS3Data->nEF = API_SIM_EF_IMSI;
                        Result              = SimElemFileStatusReq(API_SIM_EF_IMSI, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100af3), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                            return ERR_SUCCESS;
                        }
                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                    }
                    else  // API_SIM_EF_IMSI
                    {
                        SIM_PBK_PARAM *pG_Pbk_Param;
                        CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);

                        if (CFW_SimGetPS() == TRUE)
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_ME_PS, 0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                SIM_INIT_EV_STAGE3);
                        else if (CFW_SimGetPF() == TRUE)
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_ME_PF, 0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                SIM_INIT_EV_STAGE3);
                        else
                        {
                            UINT8 nFDNStatus = 0;

                            SIM_MISC_PARAM *pG_Misc_Param = NULL;
                            CFW_CfgSimGetMicParam(&pG_Misc_Param, nSimID);
                            pG_Misc_Param->nSimMode = CFW_SIM_CARD_NORMAL;

                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Normal Sim Card\n",0x08100af4));

                            CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                            if (pSimInitS3Data->nMEStatus == 0)
                            {
                                nFDNStatus = 1;
                            }
                            else
                            {
                                nFDNStatus = pG_Pbk_Param->bADNStatus;
                            }
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_NORMAL,
                                                pG_Pbk_Param->bBDNStatus << 1 | nFDNStatus, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                SIM_INIT_EV_GSM);
                        }
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("ABNormal Sim Card\n",0x08100af5));
                    SIM_MISC_PARAM *pG_Misc_Param = NULL;
                    CFW_CfgSimGetMicParam(&pG_Misc_Param, nSimID);
                    pG_Misc_Param->nSimMode = CFW_SIM_CARD_ABNORMITY;
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF,
                                        GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                    return ERR_SUCCESS;
                }
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS3_MMI:
            break;
        case SM_SIMINITS3_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
    return ERR_SUCCESS;
}

UINT32 CFW_USimInitStage3Proc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;
    SIM_PBK_PARAM *pG_Pbk_Param;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStage3Proc);

    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimInitializeAo);
    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    CFW_SIMINITS3 *pSimInitS3Data = CFW_GetAoUserData(hSimInitializeAo);

    hal_HstSendEvent(0x89300000 +  (pSimInitS3Data->nSm_SimInitS3_currStatus << 8) + pSimInitS3Data->nEF);
    switch (pSimInitS3Data->nSm_SimInitS3_currStatus)
    {
        case SM_SIMINITS3_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;
                pSimInitS3Data->nEF = API_USIM_EF_SPN;
                Result = SimReadBinaryReq(API_USIM_EF_SPN, 0x00, 17, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq EF_SPN return 0x%x \n",0x08100af6), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF, GENERATE_SHELL_UTI() | (nSimID << 24),
                                        SIM_INIT_EV_STAGE3);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                    return ERR_SUCCESS;
                }

                pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_READBINARY;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS3_ELEMENTARY:
        {
            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("ELEMFILESTATUS SW1[0x%x] nEF[%d]\n",0x08100af7), pSimElemFileStatusCnf->Sw1, pSimInitS3Data->nEF);
                //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pSimInitS3Data->nEF %d\n",0x08100af8), pSimInitS3Data->nEF);
                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2)) ||(0x91 == pSimElemFileStatusCnf->Sw1))
                {
                    CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &gUsimEfStatus[nSimID]);
                    pSimInitS3Data->nFileSize = gUsimEfStatus[nSimID].fileSize;  // transparent file: length of the body part of the EF.  ELSE EF: record length *  number of records
                    pSimInitS3Data->nRecordSize = gUsimEfStatus[nSimID].recordLength; // transparent file: 0.  else file: one record length

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("nRecordSize 0x%x\n",0x08100af9), pSimInitS3Data->nRecordSize);
                    //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pSimInitS3Data->nEF %d\n",0x08100afa), pSimInitS3Data->nEF);
                    if ((pSimInitS3Data->nEF == API_USIM_EF_AD) || (pSimInitS3Data->nEF == API_USIM_EF_UST) || (pSimInitS3Data->nEF == API_USIM_EF_EST))
                    {
                        UINT32 Result = SimReadBinaryReq(pSimInitS3Data->nEF, 0, (UINT8)pSimInitS3Data->nFileSize, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100afb), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_READBINARY; // updated by wanghb-20070711
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_LOCI)
                    {
                        UINT32 Result = 0;
                        pSimInitS3Data->nEF = API_USIM_EF_IMSI;
                        Result = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100afc), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_IMSI)
                    {
                        UINT32 Result = 0;
                        pSimInitS3Data->nEF = API_USIM_EF_GB_PBR;
                        Result  = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100afd), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                    else if((pSimInitS3Data->nEF == API_USIM_EF_GB_PBR) || (pSimInitS3Data->nEF == API_USIM_EF_PBR))
                    {
                        UINT32 Result = 0;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Normal Sim Card %d\n",0x08100afe), nSimID);
                        CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimElemFileStatusCnf->Data, pSimElemFileStatusCnf->Data[1] + 2, 16);
                        if(gUsimEfStatus[nSimID].numberOfRecords > 2)
                            gUsimEfStatus[nSimID].numberOfRecords = 2;
                        pG_Pbk_Param->nADNFileNum = gUsimEfStatus[nSimID].numberOfRecords;

                        if(gUsimEfStatus[nSimID].numberOfRecords > 0)
                        {
                            Result = SimReadRecordReq(pSimInitS3Data->nEF, 1, 4, gUsimEfStatus[nSimID].recordLength, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("ERROR: SimReadBinaryReq(EF=%d) return 0x%x \n",0x08100aff), pSimInitS3Data->nEF, Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                    SIM_INIT_EV_STAGE3);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }

                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_READRECORD;
                            CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                        else
                        {
                            SIM_MISC_PARAM *pG_Mic_Param;
                            CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                            CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Mic_Param->nSimMode,  // modify by wanghb from CFW_SIM_CARD_NORMAL to pG_Mic_Param->nSimMode
                                                pG_Pbk_Param->bBDNStatus << 1 | pG_Pbk_Param->bADNStatus,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_GSM);

                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                    }
                    else if((pSimInitS3Data->nEF == API_USIM_EF_GB_ADN) || (pSimInitS3Data->nEF == API_USIM_EF_ADN))
                    {
                        ADNInfo[nSimID][pSimInitS3Data->nIndex].RecordSize = gUsimEfStatus[nSimID].recordLength;
                        ADNInfo[nSimID][pSimInitS3Data->nIndex].MaxRecords = gUsimEfStatus[nSimID].numberOfRecords;
                        pG_Pbk_Param->nADNTotalNum += ADNInfo[nSimID][pSimInitS3Data->nIndex].MaxRecords;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("The number of ADN = %d",0x08100b00), pSimInitS3Data->nIndex);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("EF ID = %d, Record size = %d, Max number of record = %d",0x08100b01), ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID,
                                  ADNInfo[nSimID][pSimInitS3Data->nIndex].RecordSize, ADNInfo[nSimID][pSimInitS3Data->nIndex].MaxRecords);

                        //Convert SIM's EFID to an index used in stack. it must wait initialization of the ADN table finised.
                        UINT16 EFID = CFW_GetStackSimFileID(ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID, 0x5F3A, nSimID);
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("The nIndex is = %d",0x08100b02), pSimInitS3Data->nIndex);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("The id of first ADN is = %d",0x08100b03), EFID);
                        if(EFID != 0xFFFF)
                            ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID = EFID;
                        else
                            ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID = pSimInitS3Data->nEF + pSimInitS3Data->nIndex;
                        if(pSimInitS3Data->nIndex < pG_Pbk_Param->nADNFileNum - 1)
                        {
                            pSimInitS3Data->nIndex++;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("pSimInitS3Data->nIndex = %d, pSimInitS3Data->nEF + 1 = %d",0x08100b04), pSimInitS3Data->nIndex, pSimInitS3Data->nEF + 1);
                            //UINT32 Result = SimElemFileStatusReq(ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID, nSimID);
                            //Don't chenge the value pSimInitS3Data->nEF
                            UINT32 Result = SimElemFileStatusReq(pSimInitS3Data->nEF + 1, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b05), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;

                            CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                            return ERR_SUCCESS;
                        }
                        else
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("pG_Pbk_Param->nADNTotalNum = %d(Sim ID = %d)",0x08100b06), pG_Pbk_Param->nADNTotalNum, nSimID);
                            SIM_MISC_PARAM *pG_Mic_Param;
                            CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                            // modify by wanghb from CFW_SIM_CARD_NORMAL to pG_Mic_Param->nSimMode
                            CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Mic_Param->nSimMode, pG_Pbk_Param->bBDNStatus << 1 | pG_Pbk_Param->bADNStatus,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_GSM);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                    }
                    else if (pSimInitS3Data->nRecordSize != 0)
                        pSimInitS3Data->nFileSize = pSimInitS3Data->nFileSize / pSimInitS3Data->nRecordSize;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pSimInitS3Data->nEF 0x%x\n",0x08100b07), pSimInitS3Data->nEF);

                    if (pSimInitS3Data->nEF == API_USIM_EF_FDN)
                    {
                        pG_Pbk_Param->nFDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nFDNTotalNum   = gUsimEfStatus[nSimID].numberOfRecords;
                        pSimInitS3Data->nEF          = API_USIM_EF_BDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_BDN)
                    {
                        pG_Pbk_Param->nBDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nBDNTotalNum = gUsimEfStatus[nSimID].numberOfRecords;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("API_SIM_EF_BDN nBDNTotalNum %d\n",0x08100b08), pG_Pbk_Param->nBDNTotalNum);

                        pG_Pbk_Param->bBDNStatus = EF_ENABLE;
                        pSimInitS3Data->nEF      = API_SIM_EF_LND;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_LND)
                    {
                        pG_Pbk_Param->nLNDRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nLNDTotalNum   = gUsimEfStatus[nSimID].numberOfRecords;
                        pSimInitS3Data->nEF          = API_USIM_EF_SDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_SDN)
                    {
                        pG_Pbk_Param->nSDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nSDNTotalNum   = gUsimEfStatus[nSimID].numberOfRecords;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("API_SIM_EF_SDN nSDNTotalNum %d\n",0x08100b09), pG_Pbk_Param->nSDNTotalNum);
                        pSimInitS3Data->nEF = API_USIM_EF_MSISDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_MSISDN)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("EF_MSISDN\n",0x08100b0a));
                        pG_Pbk_Param->nMSISDNRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nMSISDNTotalNum   = gUsimEfStatus[nSimID].numberOfRecords;
                        pSimInitS3Data->nEF             = API_USIM_EF_ECC;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_ECC)
                    {
                        pG_Pbk_Param->nECCRecordSize = pSimInitS3Data->nRecordSize;
                        pG_Pbk_Param->nECCTotalNum   = gUsimEfStatus[nSimID].numberOfRecords;
                        pSimInitS3Data->nEF          = API_USIM_EF_LOCI;
                    }
                    else if(pSimInitS3Data->nEF == API_SIM_EF_ADN)
                    {
                        ADNInfo[nSimID][pSimInitS3Data->nIndex].RecordSize = gUsimEfStatus[nSimID].recordLength;
                        ADNInfo[nSimID][pSimInitS3Data->nIndex].MaxRecords = gUsimEfStatus[nSimID].numberOfRecords;
                        ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID = 0x6F3A;
                        pG_Pbk_Param->nADNTotalNum = gUsimEfStatus[nSimID].numberOfRecords;
                        pG_Pbk_Param->nADNFileNum = 0;
                        pG_Pbk_Param->nADNRecordNum = gUsimEfStatus[nSimID].numberOfRecords;
                        pG_Pbk_Param->nADNRecordSize = gUsimEfStatus[nSimID].recordLength;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pG_Pbk_Param->nADNRecordNum = %d", pG_Pbk_Param->nADNRecordNum);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pG_Pbk_Param->nADNRecordSize = %d", pG_Pbk_Param->nADNRecordSize);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSimInitS3Data->nIndex = %d(Sim ID = %d)", pSimInitS3Data->nIndex, nSimID);

                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("pG_Pbk_Param->nADNTotalNum = %d(Sim ID = %d)",0x08100b06), pG_Pbk_Param->nADNTotalNum, nSimID);
                        SIM_MISC_PARAM *pG_Mic_Param;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        // modify by wanghb from CFW_SIM_CARD_NORMAL to pG_Mic_Param->nSimMode
                        CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Mic_Param->nSimMode, pG_Pbk_Param->bBDNStatus << 1 | pG_Pbk_Param->bADNStatus,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_GSM);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                }
                else
                {
                    if(pSimInitS3Data->nEF == API_USIM_EF_AD)
                    {
                        SetTestSim(nSimID, 0);
                        SIM_MISC_PARAM *pG_Mic_Param;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->nSimMode = CFW_SIM_CARD_NORMAL;
                        CFW_SetSimStatus(nSimID, CFW_SIM_NORMAL);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3) , TSTR("CFW_SIM_NORMAL Maybe Phase1 SIM ELEMFILESTATUS SW1[0x%x] SW2[0x%x]nEF[0x%x]\n",0x08100b0b), pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, pSimInitS3Data->nEF);

                        pSimInitS3Data->nEF = API_USIM_EF_UST;
                    }
                    else if ((pSimInitS3Data->nEF == API_USIM_EF_UST) || (pSimInitS3Data->nEF == API_USIM_EF_LOCI) || (pSimInitS3Data->nEF == API_USIM_EF_IMSI))
                    {
                        SIM_MISC_PARAM *pG_Mic_Param;

                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->nSimMode = CFW_SIM_CARD_BAD;
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_BAD, 0, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3); // need to add error code to param2.
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_EST)
                    {
                        pSimInitS3Data->nEF = API_USIM_EF_FDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_FDN)
                    {
                        pG_Pbk_Param->nFDNRecordSize = 0;
                        pG_Pbk_Param->nFDNTotalNum   = 0;
                        pSimInitS3Data->nEF = API_USIM_EF_BDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_BDN)
                    {
                        pG_Pbk_Param->bBDNStatus = EF_DISABLE;
                        pSimInitS3Data->nEF      = API_SIM_EF_LND;
                    }
                    else if (pSimInitS3Data->nEF == API_SIM_EF_LND)
                    {
                        pG_Pbk_Param->nLNDRecordSize = 0;
                        pG_Pbk_Param->nLNDTotalNum   = 0;

                        pSimInitS3Data->nEF = API_USIM_EF_SDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_SDN)
                    {
                        pG_Pbk_Param->nSDNRecordSize = 0;
                        pG_Pbk_Param->nSDNTotalNum   = 0;
                        pSimInitS3Data->nEF          = API_USIM_EF_MSISDN;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_MSISDN)
                    {
                        pG_Pbk_Param->nMSISDNRecordSize = 0;
                        pG_Pbk_Param->nMSISDNTotalNum   = 0;
                        pSimInitS3Data->nEF             = API_USIM_EF_ECC; // This will be remove.
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_ECC)
                    {
                        pG_Pbk_Param->nECCRecordSize = 0;
                        pG_Pbk_Param->nECCTotalNum   = 0;
                        pSimInitS3Data->nEF          = API_USIM_EF_LOCI;  // This will be remove.
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_GB_PBR)
                    {
                        pSimInitS3Data->nEF = API_USIM_EF_PBR;
                        g_cfw_sim_adn_status[nSimID] = 1;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_PBR)
                    {
                        pSimInitS3Data->nEF = API_SIM_EF_ADN;
                        g_cfw_sim_adn_status[nSimID] = 2;
                    }
                    else if((pSimInitS3Data->nEF == API_USIM_EF_GB_ADN) || (pSimInitS3Data->nEF == API_USIM_EF_ADN) || (pSimInitS3Data->nEF == API_SIM_EF_ADN))
                    {
                        SIM_MISC_PARAM *pG_Mic_Param;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Mic_Param->nSimMode,  // modify by wanghb from CFW_SIM_CARD_NORMAL to pG_Mic_Param->nSimMode
                                            pG_Pbk_Param->bBDNStatus << 1 | pG_Pbk_Param->bADNStatus,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_GSM);

                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                }

                UINT32 Result = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b0c), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                    return ERR_SUCCESS;
                }

                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS3_READBINARY:
        {
            if (nEvtId == API_SIM_READBINARY_CNF)
            {
                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("api_SimReadBinaryCnf_t: SW1 %x,SW2 %x \n",0x08100b0d), pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2);
                if ((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2))
                {
                    SIM_MISC_PARAM *pG_Mic_Param;
                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    if ( pSimInitS3Data->nEF == API_USIM_EF_SPN)
                    {
                        CFW_SPN_INFO nSPNInfo = {0x00, 0x00, {0x00,}};
                        nSPNInfo.nSPNameLen = CFW_GetSPNLen(&(pSimReadBinaryCnf->Data[1]));
                        nSPNInfo.nSPNDisplayFlag = pSimReadBinaryCnf->Data[0] & 0x01;

                        SUL_MemCopy8(nSPNInfo.nSpnName, &(pSimReadBinaryCnf->Data[1]), nSPNInfo.nSPNameLen);
                        CFW_SetSimSPN(nSimID, nSPNInfo);

                        pSimInitS3Data->nEF = API_USIM_EF_AD;
                        UINT32 Result = SimElemFileStatusReq(API_USIM_EF_AD, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq EF_AD return 0x%x \n",0x08100b0e), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_AD)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("pSimReadBinaryCnf->Data[0] 0x%x \n",0x08100b0f), pSimReadBinaryCnf->Data[0]);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The size of AD EF in USIM = %d",pSimInitS3Data->nFileSize);
                        CSW_TC_MEMBLOCK(CFW_SAT_TS_ID, pSimReadBinaryCnf->Data, pSimInitS3Data->nFileSize, 16);

                        UINT8 lengthMNC = 0xFF;
                        if( pSimInitS3Data->nFileSize >= 4 )
                            lengthMNC = pSimReadBinaryCnf->Data[3];
                        CFW_SetMNCLen(lengthMNC, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The length of MNC = %d",lengthMNC);

                        if (0x00 == pSimReadBinaryCnf->Data[0])
                            SetTestSim(nSimID, 0);
                        else
                            SetTestSim(nSimID, 1);

                        //add 0x4 cell test operator
                        if ((0x00 == pSimReadBinaryCnf->Data[0]) || (0xFF == pSimReadBinaryCnf->Data[0]) || (0x01 == pSimReadBinaryCnf->Data[0])|| (0x04 == pSimReadBinaryCnf->Data[0]))
                        {
                            UINT32 Result = 0;
                            pG_Mic_Param->nSimMode = CFW_SIM_CARD_NORMAL;
                            CFW_SetSimStatus(nSimID, CFW_SIM_NORMAL);
                            pSimInitS3Data->nEF = API_USIM_EF_UST;

                            Result  = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b10), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        }
                        else if ((pSimReadBinaryCnf->Data[0] == 0x80) || (pSimReadBinaryCnf->Data[0] == 0x81))
                        {
                            pG_Mic_Param->nSimMode = CFW_SIM_CARD_TEST;
                            CFW_SetSimStatus(nSimID, CFW_SIM_TEST);
                            pSimInitS3Data->nEF = API_USIM_EF_UST;

                            UINT32 Result = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b11), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                                return ERR_SUCCESS;
                            }
                            pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                            pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        }
                        else
                        {
                            CFW_SetSimStatus(nSimID, CFW_SIM_ABNORMAL);
                            pG_Mic_Param->nSimMode = CFW_SIM_CARD_ABNORMITY;
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE3);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_UST)
                    {
                        UINT8 i       = 0;
                        UINT32 Result = 0;

                        if (pSimInitS3Data->nFileSize > SERVICES_NUM)
                            pSimInitS3Data->nFileSize = SERVICES_NUM;
                        for (i = 0; i < pSimInitS3Data->nFileSize; i++)
                            pG_Mic_Param->nServices[i] = pSimReadBinaryCnf->Data[i];
                        if(0x04 == ( pSimReadBinaryCnf->Data[2] & 0x04 ))   //service 19: Service Provider Name
                        {
                            CFW_SPN_INFO nSPNInfo = {0x00, 0x00, {0x00,}};
                            CFW_GetSimSPN( nSimID, &nSPNInfo );

                            if( nSPNInfo.nSPNameLen > 0x00 )
                            {
                                nSPNInfo.nSPNDisplayFlag = 0x01;
                            }

                            CFW_SetSimSPN( nSimID, nSPNInfo );
                        }

                        if((pSimReadBinaryCnf->Data[0] & 0x02) == 0x02)
                        {
                            pG_Pbk_Param->bFDNAvailable = 1;
                        }
                        else
                        {
                            pG_Pbk_Param->bADNStatus = EF_ENABLE;
                            pG_Pbk_Param->bFDNEnable= EF_DISABLE;
                        }
                        if((pSimReadBinaryCnf->Data[0] & 0x20) == 0x20)
                        {
                            pG_Pbk_Param->bBDNAvailable = 1;
                        }
                        else
                        {
                            pG_Pbk_Param->bBDNStatus = EF_DISABLE;
                        }
                        pSimInitS3Data->nEF = API_USIM_EF_EST;   //service n°2, 6 or 35 is "available"  :FDN / BDN / ACL

                        Result  = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b12), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                    }
                    else if (pSimInitS3Data->nEF == API_USIM_EF_EST) //
                    {
                        UINT32 Result = 0;
                        if(((pSimReadBinaryCnf->Data[0] & 0x01) == 0x01) && (pG_Pbk_Param->bFDNAvailable))  //service 1: FDN
                        {
                            pG_Pbk_Param->bADNStatus     = EF_DISABLE;
                            pG_Pbk_Param->bFDNEnable     = EF_ENABLE;
                        }
                        else
                        {
                            pG_Pbk_Param->bADNStatus     = EF_ENABLE;
                            pG_Pbk_Param->bFDNEnable     = EF_DISABLE;
                        }
                        if(((pSimReadBinaryCnf->Data[0] & 0x02) == 0x02) && (pG_Pbk_Param->bBDNAvailable))  //service 1: FDN
                        {
                            pG_Pbk_Param->bBDNEnable    = EF_ENABLE;
                        }
                        else
                        {
                            pG_Pbk_Param->bBDNEnable    = EF_DISABLE;
                        }

                        pSimInitS3Data->nEF = API_USIM_EF_FDN;
                        Result              = SimElemFileStatusReq(API_USIM_EF_FDN, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b13), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }
                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                    }
                }
                else
                {
                    if(pSimInitS3Data->nEF == API_USIM_EF_SPN)
                    {
                        pSimInitS3Data->nEF = API_USIM_EF_AD;

                        UINT32 Result = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq EF_AD return 0x%x \n",0x08100b14), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                    }
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_SIMINITS3_READRECORD:
        {
            static UINT8 PbkRecordNum[4] = {1, 1, 1, 1};

            if (nEvtId == API_SIM_READRECORD_CNF)
            {
                UINT32 Result = 0;
                api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("INIT3 API_SIM_READRECORD_CNF nSimID :0x%x\n",0x08100b15), nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("INIT3 API_SIM_READRECORD_CNF  Sw1 0x%x, sw2 0x%x EF[%d]\n",0x08100b16),
                          pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, pSimInitS3Data->nEF);

                if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("numberOfRecords :0x%x\n",0x08100b17), gUsimEfStatus[nSimID].numberOfRecords);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadRecordCnf->Data, gUsimEfStatus[nSimID].recordLength, 16);

                    if(pSimInitS3Data->nEF == API_USIM_EF_GB_PBR)
                    {
                        SimSetUsimPbk(pSimReadRecordCnf->Data, gUsimEfStatus[nSimID].recordLength, PbkRecordNum[nSimID], 0, nSimID);
                        //ADNInfo[nSimID][0].EFID = API_USIM_EF_GB_ADN;
                        //ADNInfo[nSimID][1].EFID = API_USIM_EF_GB_ADN1;
                    }
                    if(pSimInitS3Data->nEF == API_USIM_EF_PBR)
                    {
                        SimSetUsimPbk(pSimReadRecordCnf->Data, gUsimEfStatus[nSimID].recordLength, PbkRecordNum[nSimID], 1, nSimID);
                        //ADNInfo[nSimID][0].EFID = API_USIM_EF_ADN;
                        //ADNInfo[nSimID][1].EFID = API_USIM_EF_ADN1;
                    }

                    UINT8 i = 0;
                    for(i = 0; i < 255; i++)             //Get EFID of ADN in PBR
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("pSimReadRecordCnf->Data[%d] = 0x%x",0x08100b18), i, pSimReadRecordCnf->Data[i]);
                        if(pSimReadRecordCnf->Data[i] == 0xC0)
                        {
                            UINT8 j = PbkRecordNum[nSimID] - 1;
                            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadRecordCnf->Data + i + 2, 10, 16);
                            UINT16 EFID = pSimReadRecordCnf->Data[i + 2]<<8|pSimReadRecordCnf->Data[i + 3];
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("EFID of first ADN is = 0x%x",0x08100b19), EFID);
                            ADNInfo[nSimID][j].EFID = EFID;

                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("ADN File ID =  %d, index = %d\n",0x08100b1a), ADNInfo[nSimID][j], j);
                            break;
                        }
                        else if(pSimReadRecordCnf->Data[i] == 0xFF)
                        {
                            pG_Pbk_Param->nADNFileNum--;
                            break;
                        }
                    }

                    PbkRecordNum[nSimID]++;
                    if (gUsimEfStatus[nSimID].numberOfRecords >= PbkRecordNum[nSimID])
                    {
                        Result = SimReadRecordReq(pSimInitS3Data->nEF, PbkRecordNum[nSimID], 4, gUsimEfStatus[nSimID].recordLength, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq EF_SPN return 0x%x \n",0x08100b1b), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_ABNORMITY, 0xFF, GENERATE_SHELL_UTI() | (nSimID << 24),
                                                SIM_INIT_EV_STAGE3);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_READRECORD;
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                    else if(pG_Pbk_Param->nADNFileNum)
                    {
                        PbkRecordNum[nSimID] = 1;
                        //Check EFIDs in both records of PBR, if they are the same, just process one of them.
                        if(pG_Pbk_Param->nADNFileNum == 2)
                        {
                            if(ADNInfo[nSimID][0].EFID == ADNInfo[nSimID][1].EFID)
                            {
                                pG_Pbk_Param->nADNFileNum --;
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Two record of PBR are equal to each other!",0x08100b1c));
                            }
                            else
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Two record of PBR are different",0x08100b1d));
                            }
                        }

                        if(pSimInitS3Data->nEF == API_USIM_EF_GB_PBR)
                            pSimInitS3Data->nEF = API_USIM_EF_GB_ADN;

                        if(pSimInitS3Data->nEF == API_USIM_EF_PBR)
                            pSimInitS3Data->nEF = API_USIM_EF_ADN;

                        //Result = SimElemFileStatusReq(ADNInfo[nSimID][pSimInitS3Data->nIndex].EFID, nSimID);
                        Result = SimElemFileStatusReq( pSimInitS3Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b1e), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;

                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                    }
                    else
                    {
                        if (pSimInitS3Data->nEF == API_USIM_EF_GB_PBR)
                        {
                            pSimInitS3Data->nEF = API_USIM_EF_PBR;
                            g_cfw_sim_adn_status[nSimID] = 1;
                        }
                        else if (pSimInitS3Data->nEF == API_USIM_EF_PBR)
                        {
                            pSimInitS3Data->nEF = API_USIM_EF_ADN;      //????
                            g_cfw_sim_adn_status[nSimID] = 2;
                        }

                        Result = SimElemFileStatusReq(pSimInitS3Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b1f), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);

                            return ERR_SUCCESS;
                        }

                        pSimInitS3Data->nSm_SimInitS3_prevStatus = pSimInitS3Data->nSm_SimInitS3_currStatus;
                        pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_ELEMENTARY;
                        CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                        return ERR_SUCCESS;
                    }
                }
                else // READ EF PBR ERROR
                {
                    PbkRecordNum[nSimID] = 1;
                    SIM_MISC_PARAM *pG_Mic_Param;
                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Mic_Param->nSimMode,  // modify by wanghb from CFW_SIM_CARD_NORMAL to pG_Mic_Param->nSimMode
                                        pG_Pbk_Param->bBDNStatus << 1 | pG_Pbk_Param->bADNStatus,
                                        GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_GSM);

                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
                    return ERR_SUCCESS;
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }
        }
        break;
        case SM_SIMINITS3_REHABILITATE:
            break;
        case SM_SIMINITS3_MMI:
            break;
        case SM_SIMINITS3_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3Proc);
    return ERR_SUCCESS;
}


#define CFW_CHV_ET /*CHV Early treatment*/
extern CFW_SIM_OC_STATUS SIMOCStatus[]  ;
extern BOOL gSimDropInd[];
extern CFW_SIM_STATUS gSimStatus[];
extern BOOL gSimStage3Finish[];

u8 DIR_RECORD_NUM[CFW_SIM_COUNT];
u8 DIR_CURRENT_NUM[CFW_SIM_COUNT];

UINT32 CFW_SimInitStageBootProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;

    CFW_SIMINITS *pSimInitData = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStageBootProc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pSimInitData = CFW_GetAoUserData(hSimInitializeAo);

    switch (pSimInitData->nSm_SimInit_currStatus)
    {
        case SM_SIMINITS1_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("SimResetReq nSimID 0x%x \n",0x08100b20), nSimID);
                CFW_InitVar(nSimID);

                Result = SimResetReq(nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error Fail to send REST message,erro:0x%x \n",0x08100b21), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                    return ERR_SUCCESS;
                }
                pSimInitData->nResetNum                = 0;
                pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_RESET;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS1_READBINARY:
        {
            if(pEvent->nEventId == API_SIM_READBINARY_CNF)
            {
                UINT32 Result = 0;
                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("--API_SIM_READBINARY_CNF, sw1 = %x, sw2=%x\n",0x08100b22),pSimReadBinaryCnf->Sw1,pSimReadBinaryCnf->Sw2);

                if((0x90 == pSimReadBinaryCnf->Sw1) || (0x91 == pSimReadBinaryCnf->Sw1))
                {
                    SUL_MemCopy8(nICCID[nSimID], pSimReadBinaryCnf->Data, ICCID_LENGTH);
                    UINT8 Buffer[30]= {0};
                    SUL_hex2ascii(nICCID[nSimID], ICCID_LENGTH, Buffer);
                    TS_OutputText(CFW_SIM_TS_ID, TSTXT("GET ICCID(%s) Success!\n"),Buffer);
                }
NextStep:
                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                {
                    Result = SimElemFileStatusReq(API_USIM_EF_DIR, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b23), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }

                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                    pSimInitData->nEF = API_USIM_EF_DIR;
                }
                else  // SIM
#ifdef CFW_CHV_ET
                {
                    Result               = SimDedicFileStatusReq(1, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b24), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                }

#else
                {
                    pSimInitData->nEF = API_SIM_EF_ECC;
                    Result              = SimElemFileStatusReq(API_SIM_EF_ECC, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b25), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                }
#endif
            }
        }
        case SM_SIMINITS1_RESET:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;
                CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEvent == 0xFFFFFFFF:Reset Success[Sim%d]\n",0x08100b26), nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_INSERTED, 0x0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                    SIM_INIT_EV_STAGE1);

                CFW_SetSimOpenStatus(SOS_NO_SEND_SIM_OPEN, nSimID);

                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                {
                    Result              = SimElemFileStatusReq(API_USIM_EF_DIR, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b27), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }

                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                    pSimInitData->nEF = API_USIM_EF_DIR;
                }
                else  // SIM
#ifdef CFW_CHV_ET
                {
                    Result               = SimDedicFileStatusReq(1, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b28), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                }

#else
                {
                    pSimInitData->nEF = API_SIM_EF_ECC;
                    Result              = SimElemFileStatusReq(API_SIM_EF_ECC, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b29), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                }
#endif

            }
            else if (nEvtId == API_SIM_RESET_CNF)
            {
                api_SimResetCnf_t *pSimResetCnf = (api_SimResetCnf_t *)nEvParam;
                gSimDropInd[nSimID] = FALSE;
                gSimStatus[nSimID] = CFW_SIM_STATUS_END;
                gSimStage3Finish[nSimID] = FALSE;
                //if (((pSimResetCnf->Sw1 == 0x90) && (pSimResetCnf->Sw2 == 0x00)))
                if (((pSimResetCnf->Sw1 == 0x90) && (pSimResetCnf->Sw2 == 0x00))&&(0x00 != pSimResetCnf->ResetResult))
                {
                    CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nEvtId == API_SIM_RESET_CNF:Reset Success[Sim%d]\n",0x08100b2a), nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_INSERTED, 0x0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                        SIM_INIT_EV_STAGE1);

                    CFW_SetSimOpenStatus(SOS_NO_SEND_SIM_OPEN, nSimID);
                    pSimInitData->nResetNum = 0;

                    g_cfw_sim_status[nSimID].UsimFlag =
                        sxs_UsimSupported() ? pSimResetCnf->UsimFlag : FALSE;
                    //read ICCID
                    UINT32 ErrorCode = SimReadBinaryReq(API_SIM_EF_ICCID, 0, 10, nSimID);
                    if(ERR_SUCCESS != ErrorCode)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Read API_SIM_EF_ICCID ERROR\n",0x08100b2b));
                        goto NextStep;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_READBINARY;
                }
                else if((pSimResetCnf->Sw1 == 0x91) && (pSimInitData->nResetNum<2))
                {
                    UINT32 Result = 0;
    
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimResetReq nSimID 0x%x \n", nSimID);
                    CFW_InitVar(nSimID);
    
                    Result = SimResetReq(nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Error Fail to send REST message,erro:0x%x \n", Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nResetNum++;
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_RESET;
                }
                else
                {
                    if (sxs_UsimSupported())
                    {
                        if (pSimResetCnf->ResetResult == API_SIM_RESET_OK_FILE_FAIL)
                            g_cfw_sim_status[nSimID].UsimFlag = pSimResetCnf->UsimFlag;
                    }
                    else
                    {
                        g_cfw_sim_status[nSimID].UsimFlag = FALSE;
                    }

                    if (pSimInitData->nResetNum == 2) // here we may need a discussion
                    {
                        SIM_MISC_PARAM *pG_Misc_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Misc_Param, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SIM_CARD_REMOVED: SW1 %x,SW2 %x \n",0x08100b2c), pSimResetCnf->Sw1,
                                  pSimResetCnf->Sw2);
                        pG_Misc_Param->nSimMode = CFW_SIM_CARD_REMOVED;
                        CFW_SetSimStatus(nSimID, CFW_SIM_ABSENT);

#ifdef __MULTI_RAT__
                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        pSimInfo->SimStatus = 0xFF;   //0: sim; 1:Usim; 0xFF sim no exist;
                        if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("No sim card send SIM Open ERROR!!!!(For 3G) \n",0x08100b2d)));
                        }
#endif
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_REMOVED, 0xFF,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Num %x\n",0x08100b2e), pSimInitData->nResetNum);
                        pSimInitData->nResetNum++;
                        SimResetReq(nSimID);
                    }
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_SIMINITS1_ELEMENTARY:   // select EF_DIR
        {
            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2))
                        || (0x91 == pSimElemFileStatusCnf->Sw1))
                {
                    UINT32 Result;
                    CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &gUsimEfStatus[nSimID]);
                    pSimInitData->nFileSize = gUsimEfStatus[nSimID].fileSize;
                    pSimInitData->nRecordSize = gUsimEfStatus[nSimID].recordLength;
                    DIR_RECORD_NUM[nSimID] = gUsimEfStatus[nSimID].numberOfRecords;
                    DIR_CURRENT_NUM[nSimID] = 1;		

                    if(pSimInitData->nEF == API_USIM_EF_DIR)
                        Result = SimReadRecordReq(pSimInitData->nEF, 1, 4, (UINT8)(pSimInitData->nRecordSize), nSimID);
                    else
                        Result = SimReadBinaryReq(pSimInitData->nEF, 0, (UINT8)(pSimInitData->nRecordSize), nSimID);

                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100b2f), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);

                        return ERR_SUCCESS;
                    }

                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;

                    if(pSimInitData->nEF == API_USIM_EF_DIR)
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_READRECORD;
                    else
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_READBINARY;

                }
                else // EF DIR does not exist
                {
                    UINT32 Result = 0;
                    g_cfw_sim_status[nSimID].UsimFlag = FALSE;
                    {
                        Result               = SimDedicFileStatusReq(1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b30), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                            return ERR_SUCCESS;
                        }
                        pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                    }
                    CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                }
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;

        case SM_SIMINITS1_READRECORD:
        {
            if (nEvtId == API_SIM_READRECORD_CNF)
            {
                api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("API_SIM_READRECORD_CNF nSimID :0x%x\n",0x08100b31), nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("API_SIM_READRECORD_CNF  Sw1 0x%x, sw2 0x%x EF[0x%x]\n",0x08100b32),
                          pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, pSimInitData->nEF);

                if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
                {
                    if (pSimInitData->nEF == API_USIM_EF_DIR)
                    {
                        UINT32 Result = 0;

                        Result = CFW_UsimDecodeEfDirRecord (&(pSimReadRecordCnf->Data[0]), &gUsimAid[nSimID], &gUsimLabel[nSimID]);

                        if(gUsimLabel[nSimID].applicationType < 4)
                            g_cfw_sim_status[nSimID].UsimFlag = FALSE;
                        else
                            g_cfw_sim_status[nSimID].UsimFlag = TRUE;						  	

                        if( (Result == ERR_SUCCESS) && (g_cfw_sim_status[nSimID].UsimFlag == TRUE))
                        {
                            Result               = SimSelectApplicationReq(&(gUsimAid[nSimID].data[0]), gUsimAid[nSimID].length, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b33), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                                return ERR_SUCCESS;
                            }

                            pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                            pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_SELECTAPPICATION;
                        }
                        else if(DIR_CURRENT_NUM[nSimID]++ < DIR_RECORD_NUM[nSimID])
                        {
                          Result = SimReadRecordReq(pSimInitData->nEF, DIR_CURRENT_NUM[nSimID], 4, (UINT8)(pSimInitData->nRecordSize), nSimID);
  
                          if (ERR_SUCCESS != Result)
                          {
                              CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100b2f), Result);
                              CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                              CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                              CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
      
                              return ERR_SUCCESS;
                          }

                          pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                          pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_READRECORD;
                        }
                        else
                        {
                            Result               = SimDedicFileStatusReq(1, nSimID);  //GSM; select DF_TELECOM
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b34), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                                return ERR_SUCCESS;
                            }
                            pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                            pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                        }
                    }
                }
                else // EF DIR does not exist
                {
                    UINT32 Result = 0;
                    g_cfw_sim_status[nSimID].UsimFlag = FALSE;

                    Result               = SimDedicFileStatusReq(1, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b35), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                }

                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }

        }
        break;

        case SM_SIMINITS1_SELECTAPPICATION:
        {
            UINT32 Result = 0;
            if (nEvtId == API_SIM_SELECTAPPLICATION_CNF)
            {
                api_SimSelectApplicationCnf_t *pSimSelectApplicationCnf = (api_SimSelectApplicationCnf_t *)nEvParam;

                hal_HstSendEvent(0x99000000 +  (pSimSelectApplicationCnf->Sw1 << 8) + pSimSelectApplicationCnf->Sw2 );

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_SELECTAPPLICATION_CNF! return SW1 SW2 0x%x 0x%x ",0x08100b36),
                          pSimSelectApplicationCnf->Sw1, pSimSelectApplicationCnf->Sw2);

                if(((pSimSelectApplicationCnf->Sw1 != 0x90) || (pSimSelectApplicationCnf->Sw2 != 0x00)) && (pSimSelectApplicationCnf->Sw1 != 0x91))
                    g_cfw_sim_status[nSimID].UsimFlag = FALSE;

                if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)
                    Result  = SimDedicFileStatusReq(1, nSimID); //SELECT DF_TELECOM
                else // get USIM PIN status
                {
                    u8 SimAuthPIN[8] = {0};
                    CFW_UsimPin *UsimPin;
                    CFW_CfgUSimGetPinParam(&UsimPin, nSimID);
                    CFW_UsimDecodeDFFcp (&(pSimSelectApplicationCnf->Data[0]), UsimPin);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("UsimPin->usimSpecificData.pinStatus  used/enabled 0x%x 0x%x ",0x08100b37),
                              UsimPin->usimSpecificData.pinStatus.used, UsimPin->usimSpecificData.pinStatus.enabled);

                    Result  = SimVerifyCHVReq(&(SimAuthPIN[0]), CHV10, nSimID);
                }
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b38), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                    return ERR_SUCCESS;
                }

                pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;

                if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                else
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_PINSTATUS;

            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_SIMINITS1_PINSTATUS:            // get pin status
        {
            static UINT8 PinNum[4] = {1, 1, 1, 1};
            if (nEvtId == API_SIM_VERIFYCHV_CNF)
            {
                api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b39),
                          pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);

                //            hal_HstSendEvent(0x99100000 +  (pSimVerifyCHVCnf->Sw1 << 8) + pSimVerifyCHVCnf->Sw2 );

                if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;

                    if(PinNum[nSimID] == 1)
                    {
                        u8 SimAuthPIN[8] = {0};
                        UINT32 Result;
                        CFW_UsimPin *UsimPin;
                        CFW_CfgUSimGetPinParam(&UsimPin, nSimID);
                        CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

                        Remains = CFW_USimInitPinStatus(pSimVerifyCHVCnf, pG_Chv_Param, UsimPin, nSimID);

                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("PIN pG_Chv_Param->nCHVAuthen =  0x%x",0x08100b3a),  pG_Chv_Param->nCHVAuthen);

                        PinNum[nSimID] = 2;
                        Result  = SimVerifyCHVReq(&(SimAuthPIN[0]), CHV20, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b3b), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                            return ERR_SUCCESS;
                        }
                        break;
                    }
                    else if(PinNum[nSimID] == 2)
                    {
                        CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                        Remains = CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, PinNum[nSimID], nSimID);
                        PinNum[nSimID] = 1;

                        if(pG_Chv_Param->nPin1Remain == 0)
                        {
                            UINT8  SimAuthPuk[8] = {0};
                            SimUnblockCHVReq(&(SimAuthPuk[0]), &(SimAuthPuk[0]), CHV10, nSimID);
                            break;
                        }
                        else if(pG_Chv_Param->nPin2Remain == 0)
                        {
                            UINT8  SimAuthPuk[8] = {0};
                            PinNum[nSimID] = 2;
                            SimUnblockCHVReq(&(SimAuthPuk[0]), &(SimAuthPuk[0]), CHV20, nSimID);
                            break;
                        }
                        else  if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                        {
                            SIM_MISC_PARAM *pG_Mic_Param = NULL;
                            CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                            pG_Mic_Param->bStartPeriod = FALSE;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b3c));
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);
                        }
                        else
                        {
                            SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ!G_Chv_Param.nCHVAuthen %x\n",0x08100b3e), pG_Chv_Param->nCHVAuthen);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Chv_Param->nCHVAuthen,
                                                Remains, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);
                        }
                    }
                }

                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                return ERR_SUCCESS;
            }
            else  if (nEvtId == API_SIM_UNBLOCKCHV_CNF)
            {
                api_SimUnblockCHVCnf_t *pSimUnblockCHVCnf = (api_SimUnblockCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b3f),
                          pSimUnblockCHVCnf->Sw1, pSimUnblockCHVCnf->Sw2);

                if ((0x63 == pSimUnblockCHVCnf->Sw1) || (0x69 == pSimUnblockCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

                    Remains = CFW_USimParsePukStatus((api_SimVerifyCHVCnf_t *)pSimUnblockCHVCnf, pG_Chv_Param, PinNum[nSimID], nSimID);

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("PUK pG_Chv_Param->nCHVAuthen =  0x%x",0x08100b40),  pG_Chv_Param->nCHVAuthen);

                    if((pG_Chv_Param->nPin2Remain == 0) && (PinNum[nSimID] == 1))
                    {
                        UINT8  SimAuthPuk[8] = {0};
                        PinNum[nSimID] = 2;
                        SimUnblockCHVReq(&(SimAuthPuk[0]), &(SimAuthPuk[0]), CHV20, nSimID);
                        break;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->bStartPeriod = FALSE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b41));
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);
                        /*
                                                //            CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
                                                CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                                                CFW_SIM_ID s = CFW_SIM_0;

                                                for( ; s < CFW_SIM_COUNT; s++ )
                                                {
                                                    CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                                                }
                                                pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                                                CSW_TRACE(_CSW|TLEVEL(0)|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b42), nSimID);

                                                CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
                        */

                    }
                    else
                    {
                        SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ!G_Chv_Param.nCHVAuthen %x\n",0x08100b43), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Chv_Param->nCHVAuthen,
                                            Remains, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);
                    }

                }

                PinNum[nSimID] = 1;
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                return ERR_SUCCESS;

            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;

        case SM_SIMINITS1_DEDICATED:
        {
            if (nEvtId == API_SIM_DEDICFILESTATUS_CNF)
            {


                api_SimDedicFileStatusCnf_t *pSimDedicFileStatusCnf = (api_SimDedicFileStatusCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("API_SIM_DEDICFILESTATUS_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b44),
                          pSimDedicFileStatusCnf->Sw1, pSimDedicFileStatusCnf->Sw2);

                if (((0x90 == pSimDedicFileStatusCnf->Sw1) && (0x00 == pSimDedicFileStatusCnf->Sw2))
                        || (0x91 == pSimDedicFileStatusCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;
                    Remains = SimParseDedicatedStatus(pSimDedicFileStatusCnf->Data, nSimID);
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->bStartPeriod = FALSE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b45));
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);


                        //            CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        UINT32 Result = 0;

                        CFW_SIM_ID s = CFW_SIM_0;

                        for( ; s < CFW_SIM_COUNT; s++ )
                        {
                            CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                        }
                        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim


                        // CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Reset Success[Sim%d]\n",0x08100b46), nSimID);
                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b47), nSimID);
                        if (ERR_SUCCESS != (Result = CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID)))
                        {
                            //return (ret | BAL_PROC_DONT_FREE_MEM | BAL_PROC_COMPLETE);
                        }

                    }
                    else
                    {
                        SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ!G_Chv_Param.nCHVAuthen %x\n",0x08100b48), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Chv_Param->nCHVAuthen,
                                            Remains, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                    }
                }

                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                return ERR_SUCCESS;


            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;

    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
    return ERR_SUCCESS;
}


UINT32 CFW_SimInitStage1Proc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;

    CFW_SIMINITS1 *pSimInitS1Data = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStage1Proc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }


    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pSimInitS1Data = CFW_GetAoUserData(hSimInitializeAo);

    switch (pSimInitS1Data->nSm_SimInitS1_currStatus)
    {
        case SM_SIMINITS1_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;

                pSimInitS1Data->nEF = API_SIM_EF_ECC;
                Result              = SimElemFileStatusReq(API_SIM_EF_ECC, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b49), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                    return ERR_SUCCESS;
                }

                pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS1_ELEMENTARY:
        {
            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(4), TSTR("API_SIM_ELEMFILESTATUS_CNF nSimID :0x%x,Sw1=0x%x, sw2=0x%x  EF=%d\n",0x08100b4a),
                          nSimID, pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, pSimElemFileStatusCnf->File);
                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2))
                        || (0x91 == pSimElemFileStatusCnf->Sw1))
                {
                    pSimInitS1Data->nFileSize   = ((UINT16)pSimElemFileStatusCnf->Data[2] << 8) | (pSimElemFileStatusCnf->Data[3]);
                    pSimInitS1Data->nRecordSize = pSimElemFileStatusCnf->Data[14];

                    //if (pSimInitS1Data->nFileSize != 0)
                    {
                        UINT32 Result = SimReadBinaryReq(pSimInitS1Data->nEF, 0, (UINT8)(pSimInitS1Data->nFileSize), nSimID);

                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100b4b), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                            return ERR_SUCCESS;
                        }


                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_READBINARY;
                    }
                }
                else if (pSimInitS1Data->nEF == API_SIM_EF_ECC)
                {
                    UINT32 Result = 0;

                    // SET ecc is null;
                    CFW_SimSetECC(NULL);

                    // Read EFLP file.This should read EFELP file,but this file cannot be support by StackCom.
                    pSimInitS1Data->nEF = API_SIM_EF_LP;
                    Result = SimElemFileStatusReq(API_SIM_EF_LP, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b4c), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                        return ERR_SUCCESS;
                    }
                }
                else if (pSimInitS1Data->nEF == API_SIM_EF_LP)
                {
                    UINT32 Result = 0;

                    // Set LP code is NULL
                    CFW_SimSetLP(NULL, 0);

                    // Read EFPHASE
                    pSimInitS1Data->nEF = API_SIM_EF_PHASE;
                    Result = SimElemFileStatusReq(API_SIM_EF_PHASE, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b4d), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                        return ERR_SUCCESS;
                    }

                }
                else
                {
                    //UINT32 Result = 0;

                    SIM_MISC_PARAM *pG_Mic_Param;
                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    pG_Mic_Param->nPhase = SIM_PHASE1;  // default phase1
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PHASE, 0,
                                        GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                    return ERR_SUCCESS;


                    pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                    pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_DEDICATED;
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS1_READBINARY:
        {
            if (nEvtId == API_SIM_READBINARY_CNF)
            {

                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("API_SIM_READBINARY_CNF nSimID :0x%x\n",0x08100b4e), nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("API_SIM_READBINARY_CNF  Sw1 0x%x, sw2 0x%x EF[0x%x]\n",0x08100b4f),
                          pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, pSimInitS1Data->nEF);
                if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2))
                        || (0x91 == pSimReadBinaryCnf->Sw1))

                {
                    if (pSimInitS1Data->nEF == API_SIM_EF_ECC)
                    {
                        UINT32 Result = 0;

                        CFW_SimSetECC(pSimReadBinaryCnf->Data);

                        // Read EFLP file.This should read EFELP file,but this file cannot be support by StackCom.
                        pSimInitS1Data->nEF = API_SIM_EF_LP;
                        Result = SimElemFileStatusReq(API_SIM_EF_LP, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b50), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                            return ERR_SUCCESS;
                        }


                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                    }
                    else if (pSimInitS1Data->nEF == API_SIM_EF_LP)
                    {
                        UINT32 Result = 0;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimReadBinaryCnf->Data[0] 0x%x \n",0x08100b51), pSimReadBinaryCnf->Data[0]);

                        // process lp file
                        if (pSimReadBinaryCnf->Data[0] != 0xFF)
                        {
                            UINT8 *pLangPreferred = NULL;

                            pLangPreferred = (UINT8 *)CSW_SIM_MALLOC(pSimInitS1Data->nFileSize);
                            if (pLangPreferred == NULL)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error ERR_CME_MEMORY_FULL \n",0x08100b52));
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, ERR_NO_MORE_MEMORY, 0xFF,
                                                    GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                                return ERR_NO_MORE_MEMORY;
                            }
                            SUL_MemCopy8(pLangPreferred, pSimReadBinaryCnf->Data, pSimInitS1Data->nFileSize);
                            CFW_SimSetLP(pLangPreferred, pSimInitS1Data->nFileSize);
                        }
                        else
                            CFW_SimSetLP(NULL, 0);

                        // Read EFPHASE file.
                        pSimInitS1Data->nEF = API_SIM_EF_PHASE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pSimReadBinaryCnf->Data[0] 0x%x \n",0x08100b53), pSimReadBinaryCnf->Data[0]);

                        Result = SimElemFileStatusReq(API_SIM_EF_PHASE, nSimID);
                        if (ERR_SUCCESS != Result)
                        {

                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b54), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                            return ERR_SUCCESS;
                        }


                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                    }
                    else  // API_SIM_EF_PHASE
                    {
                        //            UINT32 Result = 0;


                        SIM_MISC_PARAM *pG_Mic_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->nPhase = pSimReadBinaryCnf->Data[0];
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("EF_PHASE Data[0] 0x%x\r\n",0x08100b55), pSimReadBinaryCnf->Data[0]);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PHASE, 0,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                        return ERR_SUCCESS;



                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_DEDICATED;
                    }

                    // CFW_SetAoProcCode(hSimInitializeAo,CFW_AO_PROC_CODE_CONSUMED);
                    // return ERR_SUCCESS;
                }
                else
                {
                    if (pSimInitS1Data->nEF == API_SIM_EF_ECC)
                    {
                        UINT32 Result = 0;

                        CFW_SimSetECC(NULL);  // SET ecc is null;
                        pSimInitS1Data->nEF = API_SIM_EF_LP;
                        Result = SimElemFileStatusReq(API_SIM_EF_LP, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b56), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                            return ERR_SUCCESS;
                        }


                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                    }
                    else if (pSimInitS1Data->nEF == API_SIM_EF_LP)
                    {
                        UINT32 Result = 0;

                        CFW_SimSetLP(NULL, 0);
                        pSimInitS1Data->nEF = API_SIM_EF_PHASE;
                        Result = SimElemFileStatusReq(API_SIM_EF_PHASE, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b57), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                            return ERR_SUCCESS;
                        }


                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                    }
                    else  // API_SIM_EF_PHASE
                    {

                        SIM_MISC_PARAM *pG_Mic_Param;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->nPhase = SIM_PHASE1;  // default phase1

#ifdef CFW_CHV_ET
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                        return ERR_SUCCESS;

#else
                        Result               = SimDedicFileStatusReq(1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b58), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                            return ERR_SUCCESS;
                        }
#endif


                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_DEDICATED;
                    }

                    // CFW_SetAoProcCode(hSimInitializeAo,CFW_AO_PROC_CODE_CONSUMED);
                    // return ERR_SUCCESS;
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS1_DEDICATED:
        {
            if (nEvtId == API_SIM_DEDICFILESTATUS_CNF)
            {

                api_SimDedicFileStatusCnf_t *pSimDedicFileStatusCnf = (api_SimDedicFileStatusCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("API_SIM_DEDICFILESTATUS_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b59),
                          pSimDedicFileStatusCnf->Sw1, pSimDedicFileStatusCnf->Sw2);

                if (((0x90 == pSimDedicFileStatusCnf->Sw1) && (0x00 == pSimDedicFileStatusCnf->Sw2))
                        || (0x91 == pSimDedicFileStatusCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;
                    Remains = SimParseDedicatedStatus(pSimDedicFileStatusCnf->Data, nSimID);
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->bStartPeriod = FALSE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b5a));
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);
                    }
                    else
                    {
                        SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ!G_Chv_Param.nCHVAuthen %x\n",0x08100b5b), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Chv_Param->nCHVAuthen,
                                            Remains, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                    }
                }

#ifdef CFW_CHV_ET

                pSimInitS1Data->nEF = API_SIM_EF_ECC;
                UINT32 Result              = SimElemFileStatusReq(API_SIM_EF_ECC, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b5c), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                    return ERR_SUCCESS;
                }
                pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
#else
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                return ERR_SUCCESS;
#endif
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;
        case SM_SIMINITS1_MMI:
            break;
        case SM_SIMINITS1_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
    return ERR_SUCCESS;
}


UINT32 CFW_USimInitStage1Proc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;

    CFW_SIMINITS1 *pSimInitS1Data = NULL;


    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("(CFW_USimInitStage1Proc)  ",0x08100b5d) );

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStage1Proc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pSimInitS1Data = CFW_GetAoUserData(hSimInitializeAo);

    hal_HstSendEvent(0x89110000 + pSimInitS1Data->nSm_SimInitS1_currStatus);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR(" current status = %d",0x08100b5e), pSimInitS1Data->nSm_SimInitS1_currStatus);

    switch (pSimInitS1Data->nSm_SimInitS1_currStatus)
    {
        case SM_SIMINITS1_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;

                pSimInitS1Data->nEF = API_USIM_EF_ECC;

                Result              = SimElemFileStatusReq(pSimInitS1Data->nEF, nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b5f), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                    return ERR_SUCCESS;
                }

                pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

            break;
        }

        case SM_SIMINITS1_ELEMENTARY:
        {
            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

                hal_HstSendEvent(0x89111100 + pSimInitS1Data->nEF);
                hal_HstSendEvent(0x89010000 + (pSimElemFileStatusCnf->Sw1 << 8) + pSimElemFileStatusCnf->Sw2);

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("API_SIM_ELEMFILESTATUS_CNF nSimID :0x%x,Sw1=0x%x, sw2=0x%x  \n",0x08100b60),
                          nSimID, pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2);
                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2))
                        || (0x91 == pSimElemFileStatusCnf->Sw1))
                {
                    UINT32 Result;

                    CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &gUsimEfStatus[nSimID]);
                    pSimInitS1Data->nFileSize = gUsimEfStatus[nSimID].fileSize;
                    pSimInitS1Data->nRecordSize = gUsimEfStatus[nSimID].recordLength;

                    if( gUsimEfStatus[nSimID].efStructure == 1 )  //  transparent file
                    {
                        Result = SimReadBinaryReq(pSimInitS1Data->nEF, 0, (UINT8)(pSimInitS1Data->nFileSize), nSimID);
                    }
                    else
                    {
                        Result = SimReadRecordReq(pSimInitS1Data->nEF, 1, 4, (UINT8)(pSimInitS1Data->nRecordSize), nSimID);
                    }

                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100b61), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                        return ERR_SUCCESS;
                    }

                    pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;

                    if( gUsimEfStatus[nSimID].efStructure != 1 )  // USIM and not transparent
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_READRECORD;
                    else
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_READBINARY;


                }
                else if (pSimInitS1Data->nEF == API_USIM_EF_LI)   // USIM
                {
                    CFW_SimSetLP(NULL, 0);          // Set LP code is NULL

                    SIM_MISC_PARAM *pG_Mic_Param;
                    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                    pG_Mic_Param->nPhase = SIM_PHASE3G;  // default phase2+
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PHASE, 0,
                                        GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                    {
                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        CFW_SIM_ID s = CFW_SIM_0;

                        for( ; s < CFW_SIM_COUNT; s++ )
                        {
                            CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                        }
                        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b42), nSimID);

                        CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
                    }
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                    return ERR_SUCCESS;

                }
                else
                {
                    UINT32 Result = 0;

                    if (pSimInitS1Data->nEF == API_USIM_EF_ECC)     // USIM
                    {
                        CFW_USimSetECC(NULL, 0);    // SET ecc is null;
                        pSimInitS1Data->nEF = API_USIM_EF_LI;
                    }

                    Result = SimElemFileStatusReq(pSimInitS1Data->nEF, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b62), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                        return ERR_SUCCESS;
                    }

                }

                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS1_READBINARY:
        {
            if (nEvtId == API_SIM_READBINARY_CNF)
            {
                hal_HstSendEvent(0x89112200 + pSimInitS1Data->nEF);

                api_SimReadBinaryCnf_t *pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_READBINARY_CNF nSimID :0x%x; EF FILE:0x%x\n",0x08100b63), nSimID, pSimInitS1Data->nEF);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("API_SIM_READBINARY_CNF  Sw1 0x%x, sw2 0x%x EF[0x%x]\n",0x08100b64),
                          pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, pSimInitS1Data->nEF);

                if ((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2))
                {
                    if (pSimInitS1Data->nEF == API_USIM_EF_LI)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param = NULL;

                        if (pSimReadBinaryCnf->Data[0] != 0xFF)
                        {
                            UINT8 *pLangPreferred = NULL;

                            pLangPreferred = (UINT8 *)CSW_SIM_MALLOC(pSimInitS1Data->nFileSize);
                            if (pLangPreferred == NULL)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Error ERR_CME_MEMORY_FULL \n",0x08100b65));

                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, ERR_NO_MORE_MEMORY, 0xFF,
                                                    GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);

                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                                return ERR_NO_MORE_MEMORY;
                            }
                            SUL_MemCopy8(pLangPreferred, pSimReadBinaryCnf->Data, pSimInitS1Data->nFileSize);
                            CFW_SimSetLP(pLangPreferred, pSimInitS1Data->nFileSize);
                        }
                        else
                            CFW_SimSetLP(NULL, 0);


                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->nPhase = SIM_PHASE3G;

                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PHASE, 0,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);
#if 0
                        {
                            CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                            CFW_SIM_ID s = CFW_SIM_0;

                            for( ; s < CFW_SIM_COUNT; s++ )
                            {
                                CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                            }
                            pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b42), nSimID);

                            CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
                        }
#endif
#ifdef LTE_NBIOT_SUPPORT
                         {
						   
							CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));
							
						  //  CFW_SIM_ID s = CFW_SIM_0;
							
						//	  for( ; s < CFW_SIM_COUNT; s++ )
							{
								CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[nSimID], nSimID);
							}
							pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim
							
							CSW_TRACE(_CSW|TLEVEL(0)|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b42), nSimID);
							
							CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
						}
#endif
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                        return ERR_SUCCESS;

                    }


                }
                else
                {
                    if (pSimInitS1Data->nEF == API_USIM_EF_LI)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param;
                        CFW_SimSetLP(NULL, 0);

                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->nPhase = SIM_PHASE3G;  // default phase2+
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PHASE, 0,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                        {
                            CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                            CFW_SIM_ID s = CFW_SIM_0;

                            for( ; s < CFW_SIM_COUNT; s++ )
                            {
                                CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                            }
                            pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b42), nSimID);

                            CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
                        }

                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                        return ERR_SUCCESS;
                    }

                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_SIMINITS1_READRECORD:
        {
            if (nEvtId == API_SIM_READRECORD_CNF)
            {
                hal_HstSendEvent(0x88115500 + pSimInitS1Data->nEF);

                api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("API_SIM_READRECORD_CNF nSimID :0x%x\n",0x08100b66), nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("API_SIM_READRECORD_CNF  Sw1 0x%x, sw2 0x%x EF[0x%x]\n",0x08100b67),  pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, pSimInitS1Data->nEF);

                if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
                {
                    if (pSimInitS1Data->nEF == API_USIM_EF_ECC)
                    {
                        UINT32 Result = 0;

                        CFW_USimSetECC(pSimReadRecordCnf->Data, gUsimEfStatus[nSimID].fileSize);

                        pSimInitS1Data->nEF = API_USIM_EF_LI;        // Read EFLP file.

                        Result = SimElemFileStatusReq(pSimInitS1Data->nEF, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadRecord  return 0x%x \n",0x08100b68), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);

                            return ERR_SUCCESS;
                        }

                        pSimInitS1Data->nSm_SimInitS1_prevStatus = pSimInitS1Data->nSm_SimInitS1_currStatus;
                        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_ELEMENTARY;
                    }
                }
            }

        }
        break;

        case SM_SIMINITS1_DEDICATED:
            break;
        case SM_SIMINITS1_MMI:
            break;
        case SM_SIMINITS1_STACK:
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
    return ERR_SUCCESS;
}
UINT32 CFW_SimCloseMsg(
    CFW_SIM_ID nSimID
)
{

    UINT32 result    = 0x00000000;
    hal_HstSendEvent(0x0723aaaa);

    result = CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID);
    if (ERR_SUCCESS != result)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SendSclMessage return 0x%x \n",0x08100b69), result);
        return result;

    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimResetProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI      = 0x00000000;
    UINT32 nEvtId    = 0x00000000;
    UINT32 ErrorCode = 0x00000000;
    UINT32 result    = 0x00000000;
    VOID *nEvParam   = NULL;
    CFW_EV ev;

    SIM_SM_RESET *pReset = NULL;
    api_SimResetCnf_t *pSimResetCnf = NULL;

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hAo);
    pReset = CFW_GetAoUserData(hAo);
    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                = 0xff;
        pEvent                    = &ev;
        pReset->nState.nNextState = CFW_SIM_MIS_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    switch (pReset->nState.nNextState)
    {
        case CFW_SIM_MIS_IDLE:

            result = CFW_SendSclMessage(API_SIM_RESET_REQ, NULL, nSimID);
            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_SendSclMessage return 0x%x \n",0x08100b6a), result);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                hAo = HNULL;
                CFW_PostNotifyEvent(EV_CFW_SIM_RESET_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
            pReset->nState.nCurrState = CFW_SIM_MIS_IDLE;
            pReset->nState.nNextState = CFW_SIM_MIS_RESET;
            break;

        case CFW_SIM_MIS_RESET:
            pSimResetCnf = (api_SimResetCnf_t *)nEvParam;
            CFW_GetUTI(hAo, &nUTI);

            //if ((pSimResetCnf->ResetResult == 0x02) || (pSimResetCnf->ResetResult == 0x03))
            if (((pSimResetCnf->Sw1 == 0x90) && (pSimResetCnf->Sw2 == 0x00)) || (pSimResetCnf->Sw1 == 0x91))
            {
                CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(0), TSTR("Send sim open \n",0x08100b6b));
                CFW_SendSclMessage(API_SIM_OPEN_IND, NULL, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_RESET_RSP, (UINT32)0x00, 0x00, nUTI | (nSimID << 24), 0x00);  // 2006-11-13
            }
            else
            {
                CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID);
                ErrorCode = Sim_ParseSW1SW2(pSimResetCnf->Sw1, pSimResetCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_RESET_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
            }
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            hAo = HNULL;
            break;

        default:
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            hAo = HNULL;
            break;
    }
    return ERR_SUCCESS;
}
#ifdef _DROPCARD_AUTOPIN_
extern UINT8 gTempPin[12];
UINT32 CFW_SimResetExProc(HAO hAo, CFW_EV *pEvent)
{
    HAO hSimInitializeAo = hAo;

    UINT32 nEvtId  = 0;
    VOID *nEvParam = 0;

    CFW_SIMINITS *pSimInitData = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStageBootProc);
    if ((UINT32)pEvent != 0xFFFFFFFF)
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_SIM_ID nSimID;

    nSimID = CFW_GetSimCardID(hSimInitializeAo);
    pSimInitData = CFW_GetAoUserData(hSimInitializeAo);

    switch (pSimInitData->nSm_SimInit_currStatus)
    {
        case SM_SIMINITS1_IDLE:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("SimResetReq nSimID 0x%x \n",0x08100b6c), nSimID);
                CFW_InitVar(nSimID);
                hal_HstSendEvent(0x09290001);
                Result = SimResetReq(nSimID);
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error Fail to send REST message,erro:0x%x \n",0x08100b6d), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                    return ERR_SUCCESS;
                }
                pSimInitData->nResetNum                = 0;
                pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_RESET;
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;
        case SM_SIMINITS1_RESET:
        {
            if ((UINT32)pEvent == 0xFFFFFFFF)
            {
                UINT32 Result = 0;
                CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pEvent == 0xFFFFFFFF:Reset Success[Sim%d]\n",0x08100b6e), nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_INSERTED, 0x0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                    SIM_INIT_EV_STAGE1);

                CFW_SetSimOpenStatus(SOS_NO_SEND_SIM_OPEN, nSimID);

                if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                {
                    Result              = SimElemFileStatusReq(API_USIM_EF_DIR, nSimID);//应用标识符
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b6f), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }

                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                    pSimInitData->nEF = API_USIM_EF_DIR;
                }
                else  // SIM
#ifdef CFW_CHV_ET
                {
                    Result               = SimDedicFileStatusReq(1, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b70), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                }

#else
                {
                    pSimInitData->nEF = API_SIM_EF_ECC;
                    Result              = SimElemFileStatusReq(API_SIM_EF_ECC, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b71), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                }
#endif

            }
            else if (nEvtId == API_SIM_RESET_CNF)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nEvtId == API_SIM_RESET_CNF: 1 Reset Success[Sim%d]\n",0x08100b72), nSimID);
                api_SimResetCnf_t *pSimResetCnf = (api_SimResetCnf_t *)nEvParam;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSimResetCnf->Sw1%d,Sw2= %d\n",0x08100b73), pSimResetCnf->Sw1,pSimResetCnf->Sw2);

                if (((pSimResetCnf->Sw1 == 0x90) && (pSimResetCnf->Sw2 == 0x00)) || (pSimResetCnf->Sw1 == 0x91))
                {
                    UINT32 Result = 0;

                    gSimDropInd[nSimID] = FALSE;
                    gSimStatus[nSimID] = CFW_SIM_STATUS_END;
                    gSimStage3Finish[nSimID] = FALSE;


                    CFW_RegisterCreateAoProc(API_SIM_STATUSERROR_IND, SimSpySimCard);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nEvtId == API_SIM_RESET_CNF: 1 Reset Success[Sim%d]\n",0x08100b74), nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_INSERTED, 0x0, GENERATE_SHELL_UTI() | (nSimID << 24),
                                        SIM_INIT_EV_STAGE1);

                    CFW_SetSimOpenStatus(SOS_NO_SEND_SIM_OPEN, nSimID);

                    g_cfw_sim_status[nSimID].UsimFlag =
                        sxs_UsimSupported() ? pSimResetCnf->UsimFlag : FALSE;

                    if(g_cfw_sim_status[nSimID].UsimFlag) // USIM
                    {
                        Result              = SimElemFileStatusReq(API_USIM_EF_DIR, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b75), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                            return ERR_SUCCESS;
                        }

                        pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                        pSimInitData->nEF = API_USIM_EF_DIR;
                    }
                    else  // SIM
#ifdef CFW_CHV_ET
                    {
                        Result               = SimDedicFileStatusReq(1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b76), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                            return ERR_SUCCESS;
                        }
                        pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                    }

#else
                    {
                        pSimInitData->nEF = API_SIM_EF_ECC;
                        Result              = SimElemFileStatusReq(API_SIM_EF_ECC, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimElemFileStatusReq return 0x%x \n",0x08100b77), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage1Proc);
                            return ERR_SUCCESS;
                        }
                        pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_ELEMENTARY;
                    }
#endif
                }
                else
                {
                    if (sxs_UsimSupported())
                    {
                        if (pSimResetCnf->ResetResult == API_SIM_RESET_OK_FILE_FAIL)
                            g_cfw_sim_status[nSimID].UsimFlag = pSimResetCnf->UsimFlag;
                    }
                    else
                    {
                        g_cfw_sim_status[nSimID].UsimFlag = FALSE;
                    }

                    if (pSimInitData->nResetNum == 2) // here we may need a discussion
                    {
                        SIM_MISC_PARAM *pG_Misc_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Misc_Param, nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CFW_SIM_CARD_REMOVED: SW1 %x,SW2 %x \n",0x08100b78), pSimResetCnf->Sw1,
                                  pSimResetCnf->Sw2);
                        pG_Misc_Param->nSimMode = CFW_SIM_CARD_REMOVED;
                        CFW_SetSimStatus(nSimID, CFW_SIM_ABSENT);

#ifdef __MULTI_RAT__
                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        pSimInfo->SimStatus = 0xFF;   //0: sim; 1:Usim; 0xFF sim no exist;
                        if (ERR_SUCCESS != CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_NW_TS_ID)|TDB|TNB_ARG(0), TSTXT(TSTR("No sim card send SIM Open ERROR!!!!(For 3G) \n",0x08100b79)));
                        }
#endif
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_REMOVED, 0xFF,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Num %x\n",0x08100b7a), pSimInitData->nResetNum);
                        pSimInitData->nResetNum++;
                        SimResetReq(nSimID);
                        hal_HstSendEvent(0x09290003);
                    }
                }
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_SIMINITS1_ELEMENTARY:   // select EF_DIR
        {
            if (nEvtId == API_SIM_ELEMFILESTATUS_CNF)
            {
                api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)nEvParam;

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("pSimElemFileStatusCnf->Sw1->Sw1%d,Sw2= %d\n",0x08100b7b), pSimElemFileStatusCnf->Sw1,pSimElemFileStatusCnf->Sw2);
                if (((0x90 == pSimElemFileStatusCnf->Sw1) && (00 == pSimElemFileStatusCnf->Sw2))
                        || (0x91 == pSimElemFileStatusCnf->Sw1))
                {
                    UINT32 Result;
                    CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &gUsimEfStatus[nSimID]);
                    pSimInitData->nFileSize = gUsimEfStatus[nSimID].fileSize;
                    pSimInitData->nRecordSize = gUsimEfStatus[nSimID].recordLength;

                    if(pSimInitData->nEF == API_USIM_EF_DIR)
                        Result = SimReadRecordReq(pSimInitData->nEF, 1, 4, (UINT8)(pSimInitData->nRecordSize), nSimID);
                    else
                        Result = SimReadBinaryReq(pSimInitData->nEF, 0, (UINT8)(pSimInitData->nRecordSize), nSimID);

                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100b7c), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);

                        return ERR_SUCCESS;
                    }

                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;

                    if(pSimInitData->nEF == API_USIM_EF_DIR)
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_READRECORD;
                    else
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_READBINARY;

                }
                else // EF DIR does not exist
                {
                    UINT32 Result = 0;
                    g_cfw_sim_status[nSimID].UsimFlag = FALSE;
                    {
                        Result               = SimDedicFileStatusReq(1, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b7d), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                            return ERR_SUCCESS;
                        }
                        pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                        pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                    }
                    CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
                }
            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;

        case SM_SIMINITS1_READRECORD:
        {
            if (nEvtId == API_SIM_READRECORD_CNF)
            {
                api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("API_SIM_READRECORD_CNF nSimID :0x%x\n",0x08100b7e), nSimID);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(3), TSTR("API_SIM_READRECORD_CNF  Sw1 0x%x, sw2 0x%x EF[0x%x]\n",0x08100b7f),
                          pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, pSimInitData->nEF);

                if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
                {
                    if (pSimInitData->nEF == API_USIM_EF_DIR)
                    {
                        UINT32 Result = 0;

                        Result = CFW_UsimDecodeEfDirRecord (&(pSimReadRecordCnf->Data[0]), &gUsimAid[nSimID], &gUsimLabel[nSimID]);

                        if(gUsimLabel[nSimID].applicationType < 4)
                            g_cfw_sim_status[nSimID].UsimFlag = FALSE;

                        if( (Result == ERR_SUCCESS) && (g_cfw_sim_status[nSimID].UsimFlag == TRUE))
                        {
                            Result               = SimSelectApplicationReq(&(gUsimAid[nSimID].data[0]), gUsimAid[nSimID].length, nSimID);
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b80), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                                return ERR_SUCCESS;
                            }

                            pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                            pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_SELECTAPPICATION;
                        }
                        else
                        {
                            Result               = SimDedicFileStatusReq(1, nSimID);  //GSM; select DF_TELECOM
                            if (ERR_SUCCESS != Result)
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b81), Result);
                                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                                return ERR_SUCCESS;
                            }
                            pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                            pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                        }
                    }
                }
                else // EF DIR does not exist
                {
                    UINT32 Result = 0;
                    g_cfw_sim_status[nSimID].UsimFlag = FALSE;

                    Result               = SimDedicFileStatusReq(1, nSimID);
                    if (ERR_SUCCESS != Result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b82), Result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                        return ERR_SUCCESS;
                    }
                    pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                }

                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONSUMED);
            }
            else
            {
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            }

        }
        break;

        case SM_SIMINITS1_SELECTAPPICATION:
        {
            UINT32 Result = 0;
            if (nEvtId == API_SIM_SELECTAPPLICATION_CNF)
            {
                api_SimSelectApplicationCnf_t *pSimSelectApplicationCnf = (api_SimSelectApplicationCnf_t *)nEvParam;

                hal_HstSendEvent(0x99000000 +  (pSimSelectApplicationCnf->Sw1 << 8) + pSimSelectApplicationCnf->Sw2 );

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_SELECTAPPLICATION_CNF! return SW1 SW2 0x%x 0x%x ",0x08100b83),
                          pSimSelectApplicationCnf->Sw1, pSimSelectApplicationCnf->Sw2);

                if( (pSimSelectApplicationCnf->Sw1 != 0x90) || (pSimSelectApplicationCnf->Sw2 != 0x00))
                    g_cfw_sim_status[nSimID].UsimFlag = FALSE;

                if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)
                    Result  = SimDedicFileStatusReq(1, nSimID); //SELECT DF_TELECOM
                else // get USIM PIN status
                {
                    u8 SimAuthPIN[8] = {0};
                    CFW_UsimPin *UsimPin;
                    CFW_CfgUSimGetPinParam(&UsimPin, nSimID);
                    CFW_UsimDecodeDFFcp (&(pSimSelectApplicationCnf->Data[0]), UsimPin);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("UsimPin->usimSpecificData.pinStatus  used/enabled 0x%x 0x%x ",0x08100b84),
                              UsimPin->usimSpecificData.pinStatus.used, UsimPin->usimSpecificData.pinStatus.enabled);

                    Result  = SimVerifyCHVReq(&(SimAuthPIN[0]), CHV10, nSimID);
                }
                if (ERR_SUCCESS != Result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b85), Result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                    return ERR_SUCCESS;
                }

                pSimInitData->nSm_SimInit_prevStatus = pSimInitData->nSm_SimInit_currStatus;

                if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_DEDICATED;
                else
                    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_PINSTATUS;

            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
        }
        break;

        case SM_SIMINITS1_PINSTATUS:            // get pin status
        {
            static UINT8 PinNum[4] = {1, 1, 1, 1};
            if (nEvtId == API_SIM_VERIFYCHV_CNF)
            {
                api_SimVerifyCHVCnf_t *pSimVerifyCHVCnf = (api_SimVerifyCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b86),
                          pSimVerifyCHVCnf->Sw1, pSimVerifyCHVCnf->Sw2);


                if ((0x63 == pSimVerifyCHVCnf->Sw1) || (0x69 == pSimVerifyCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;

                    if(PinNum[nSimID] == 1)
                    {
                        u8 SimAuthPIN[8] = {0};
                        UINT32 Result;
                        CFW_UsimPin *UsimPin;
                        CFW_CfgUSimGetPinParam(&UsimPin, nSimID);
                        CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                        pG_Chv_Param->bCHV2VerifyReq = FALSE;
                        Remains = CFW_USimInitPinStatus(pSimVerifyCHVCnf, pG_Chv_Param, UsimPin, nSimID);

                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("PIN pG_Chv_Param->nCHVAuthen =  0x%x",0x08100b87),  pG_Chv_Param->nCHVAuthen);

                        PinNum[nSimID] = 2;
                        Result  = SimVerifyCHVReq(&(SimAuthPIN[0]), CHV20, nSimID);
                        if (ERR_SUCCESS != Result)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("Error SimDedicFileStatusReq return 0x%x \n",0x08100b88), Result);
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, Result, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
                            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                            CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                            return ERR_SUCCESS;
                        }
                        break;
                    }
                    else if(PinNum[nSimID] == 2)
                    {
                        CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                        Remains = CFW_USimParsePinStatus(pSimVerifyCHVCnf, pG_Chv_Param, PinNum[nSimID], nSimID);
                        PinNum[nSimID] = 1;

                        if(pG_Chv_Param->nPin1Remain == 0)
                        {
                            UINT8  SimAuthPuk[8] = {0};
                            SimUnblockCHVReq(&(SimAuthPuk[0]), &(SimAuthPuk[0]), CHV10, nSimID);
                            break;
                        }
                        else if(pG_Chv_Param->nPin2Remain == 0)
                        {
                            UINT8  SimAuthPuk[8] = {0};
                            PinNum[nSimID] = 2;
                            SimUnblockCHVReq(&(SimAuthPuk[0]), &(SimAuthPuk[0]), CHV20, nSimID);
                            break;
                        }
                        else  if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                        {
                            SIM_MISC_PARAM *pG_Mic_Param = NULL;
                            CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                            pG_Mic_Param->bStartPeriod = FALSE;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b89));
                            CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                                GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);

                            CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                            CFW_SIM_ID s = CFW_SIM_0;

                            for( ; s < CFW_SIM_COUNT; s++ )
                            {
                                CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                            }
                            pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                            CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b8a), nSimID);

                            CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
#ifdef _DROPCARD_AUTOPIN_
                            SimRetryReq(nSimID);
#endif
                        }
                        else
                        {

                            SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ 1!G_Chv_Param.nCHVAuthen %x\n",0x08100b8b), pG_Chv_Param->nCHVAuthen);
                            if(ERR_SUCCESS == CFW_SimEnterAuthenticationEX(gTempPin,(UINT8)strlen(gTempPin),NULL,0,1,APP_UTI_SHELL,nSimID))
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_SimEnterAuthenticationEX ok!\n",0x08100b8c));
                            }

                        }
                    }
                }
                hal_HstSendEvent(0x1009cccc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                return ERR_SUCCESS;
            }
            else  if (nEvtId == API_SIM_UNBLOCKCHV_CNF)
            {
                api_SimUnblockCHVCnf_t *pSimUnblockCHVCnf = (api_SimUnblockCHVCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("API_SIM_VERIFYCHV_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b8d),
                          pSimUnblockCHVCnf->Sw1, pSimUnblockCHVCnf->Sw2);

                if ((0x63 == pSimUnblockCHVCnf->Sw1) || (0x69 == pSimUnblockCHVCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);

                    Remains = CFW_USimParsePukStatus((api_SimVerifyCHVCnf_t *)pSimUnblockCHVCnf, pG_Chv_Param, PinNum[nSimID], nSimID);

                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("PUK pG_Chv_Param->nCHVAuthen =  0x%x",0x08100b8e),  pG_Chv_Param->nCHVAuthen);

                    if((pG_Chv_Param->nPin2Remain == 0) && (PinNum[nSimID] == 1))
                    {
                        UINT8  SimAuthPuk[8] = {0};
                        PinNum[nSimID] = 2;
                        SimUnblockCHVReq(&(SimAuthPuk[0]), &(SimAuthPuk[0]), CHV20, nSimID);
                        break;
                    }
                    else if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->bStartPeriod = FALSE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b8f));
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);

                        //            CFW_STORELIST_INFO *pStoreList = CFW_MemAllocMessageStructure(sizeof(CFW_STORELIST_INFO));
                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        CFW_SIM_ID s = CFW_SIM_0;

                        for( ; s < CFW_SIM_COUNT; s++ )
                        {
                            CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                        }
                        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim

                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b90), nSimID);

                        CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID);
                    }
                    else
                    {
                        SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ! 2 G_Chv_Param.nCHVAuthen %x\n",0x08100b91), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Chv_Param->nCHVAuthen,
                                            Remains, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);
                    }

                }
                PinNum[nSimID] = 1;
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
#ifdef _DROPCARD_AUTOPIN_
                SimRetryReq(nSimID);
#endif
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                return ERR_SUCCESS;

            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;

        case SM_SIMINITS1_DEDICATED:
        {
            if (nEvtId == API_SIM_DEDICFILESTATUS_CNF)
            {


                api_SimDedicFileStatusCnf_t *pSimDedicFileStatusCnf = (api_SimDedicFileStatusCnf_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("API_SIM_DEDICFILESTATUS_CNF  Sw1 0x%x, sw2 0x%x \n",0x08100b92),
                          pSimDedicFileStatusCnf->Sw1, pSimDedicFileStatusCnf->Sw2);

                if (((0x90 == pSimDedicFileStatusCnf->Sw1) && (0x00 == pSimDedicFileStatusCnf->Sw2))
                        || (0x91 == pSimDedicFileStatusCnf->Sw1))
                {
                    UINT32 Remains = 0;

                    SIM_CHV_PARAM *pG_Chv_Param;
                    Remains = SimParseDedicatedStatus(pSimDedicFileStatusCnf->Data, nSimID);
                    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
                    if (pG_Chv_Param->nCHVAuthen == CFW_SIM_CARD_PIN1DISABLE)
                    {
                        SIM_MISC_PARAM *pG_Mic_Param = NULL;
                        CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
                        pG_Mic_Param->bStartPeriod = FALSE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SIM_CARD_PIN1DISABLE!\n",0x08100b93));
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_PIN1DISABLE, Remains,
                                            GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE2);


                        CFW_SIM_INFO *pSimInfo = CFW_MemAllocMessageStructure(sizeof(CFW_SIM_INFO));

                        UINT32 Result = 0;

                        CFW_SIM_ID s = CFW_SIM_0;

                        for( ; s < CFW_SIM_COUNT; s++ )
                        {
                            CFW_CfgGetStoredPlmnList(&pSimInfo->p_StoreList[s], s);
                        }
                        pSimInfo->SimStatus = g_cfw_sim_status[nSimID].UsimFlag;   //0: sim; 1:Usim


                        CSW_TRACE(_CSW|TSTDOUT|TDB|TNB_ARG(1), TSTR("Send Sim Open to Stack[sim%d]\n",0x08100b94), nSimID);
                        if (ERR_SUCCESS != (Result = CFW_SendSclMessage(API_SIM_OPEN_IND, pSimInfo, nSimID)))
                        {
                        }

                    }
                    else
                    {
                        SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SIM_CARD_pin REQ! 3 G_Chv_Param.nCHVAuthen %x\n",0x08100b95), pG_Chv_Param->nCHVAuthen);
                        CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, pG_Chv_Param->nCHVAuthen,
                                            Remains, GENERATE_SHELL_UTI() | (nSimID << 24), SIM_INIT_EV_STAGE1);

                    }
                }
#ifdef _DROPCARD_AUTOPIN_
                SimRetryReq(nSimID);
#endif
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hSimInitializeAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
                return ERR_SUCCESS;


            }
            else
                CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_NOT_CONSUMED);

        }
        break;

    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStageBootProc);
    return ERR_SUCCESS;
}

#endif
/*
Bytes   Description M/O Length
1   Services n°1 to n°4   M   1 byte
2   Services n°5 to n°8   M   1 byte
3   Services n°9 to n°12  O   1 byte
4   Services n°13 to n°16 O   1 byte
5   Services n°17 to n°20 O   1 byte
6   Services n°21 to n°24 O   1 byte
7   Services n°25 to n°28 O   1 byte
8   Services n°29 to n°32 O   1 byte
etc.
X   Services (4X 3) to (4X) O   1 byte

 Services
     Contents:  Service n°1 :  CHV1 disable function
    Service n°2 :  Abbreviated Dialling Numbers (ADN)
    Service n°3 :  Fixed Dialling Numbers (FDN)
    Service n°4 :  Short Message Storage (SMS)
    Service n°5 :  Advice of Charge (AoC)
    Service n°6 :  Capability Configuration Parameters (CCP)
    Service n°7 :  PLMN selector
    Service n°8 :  RFU
    Service n°9 :  MSISDN
    Service n°10:  Extension1
    Service n°11:  Extension2
    Service n°12:  SMS Parameters
    Service n°13:  Last Number Dialled (LND)
    Service n°14:  Cell Broadcast Message Identifier
    Service n°15:  Group Identifier Level 1
    Service n°16:  Group Identifier Level 2
    Service n°17:  Service Provider Name
    Service n°18:  Service Dialling Numbers (SDN)
    Service n°19:  Extension3
    Service n°20:  RFU
    Service n°21:  VGCS Group Identifier List (EFVGCS and EFVGCSS)
    Service n°22:  VBS Group Identifier List (EFVBS and EFVBSS)
    Service n°23:  enhanced Multi Level Precedence and Pre emption Service
    Service n°24:  Automatic Answer for eMLPP
    Service n°25:  Data download via SMS CB
    Service n°26:  Data download via SMS PP
    Service n°27:  Menu selection
    Service n°28:  Call control
    Service n°29:  Proactive SIM
    Service n°30:  Cell Broadcast Message Identifier Ranges
    Service n°31:  Barred Dialling Numbers (BDN)
    Service n°32:  Extension4
    Service n°33:  De personalization Control Keys
    Service n°34:  Co operative Network List
    Service n°35:  Short Message Status Reports
    Service n°36:  Network's indication of alerting in the MS
    Service n°37:  Mobile Originated Short Message control by SIM
    Service n°38:  GPRS
    Service n°39:  Image (IMG)
    Service n°40:  SoLSA (Support of Local Service Area)
    Service n°41:  USSD string data object supported in Call Control
    Service n°42:  RUN AT COMMAND command
    Service n 43:   User controlled PLMN Selector with Access Technology
    Service n 44:   Operator controlled PLMN Selector with Access Technology
    Service n 45    HPLMN Selector with Access Technology
    Service n 46:   CPBCCH Information
    Service n 47:   Investigation Scan
    Service n°48:  Extended Capability Configuration Parameters
    Service n°49:  MExE
    Service n°50   RPLMN last used Access Technology

*/

UINT8 CFW_GetSSTStatus(UINT8 nServiceNum, CFW_SIM_ID nSimID)
{
    SIM_MISC_PARAM *pG_Mic_Param;

    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);

    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pG_Mic_Param->nServices, pG_Mic_Param->nServicesSize, 16);

    UINT8 uRet = (pG_Mic_Param->nServices[nServiceNum / 4 - 1] >> (((nServiceNum + 3) % 4) * 2)) & 0x03;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(5), TSTR("pG_Mic_Param->nServices[%d/4-1] %d nServicesSize %d uRet %d (((nServiceNum+3)%4)*2) %d\n",0x08100b96),
              nServiceNum, pG_Mic_Param->nServices[nServiceNum / 4 - 1], pG_Mic_Param->nServicesSize, uRet, (((nServiceNum + 3) % 4) * 2));

    return uRet;

}



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



#include "at.h"
#include "at_cmd_sim.h"
#include "csw.h"

UINT8 SUL_ascii2hex(UINT8* pInput, UINT8* pOutput);
UINT8 SUL_hex2ascii(UINT8* pInput, UINT16 nInputLen, UINT8* pOutput);
// define trigger debug inform output by trace:   1 ---> debug;   0 ---> colse debug  //add by wxd
#define SIM_DEBUG 0

// CPIN related definition.actually this  is enough for all the code
#define PIN1CODEMAX 12
#define PINNEEDPUK 3

// status of PUK1
UINT8 g_Pin1PukStauts[CFW_SIM_COUNT] = {0,};
static UINT8 g_Pin1ErrorTimes[CFW_SIM_COUNT] = {0,};
extern UINT32 CFW_GetStackSimFileID(UINT16 n3GppFileID, UINT16 EFPath, CFW_SIM_ID nSimID);

// #define PUK1CODEMAX 12
#define SIM_UTI 6

// CLCK related
#define U16_SWAP(x)   ((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))
#define FAC_MAX_NUM 6
#define CLCK_PWD_MAX 12
// static UINT8 clckTestSTR[] = "+CLCK: (\"PS\",\"PF\",\"SC\",\"FD\",\"PN\",\"PU\",\"PP\",\"PC\")";

// CPWD related
/*
******************************************************************************
*   #define CFW_STY_FAC_TYPE_SC  0x5343
*   #define CFW_STY_FAC_TYPE_PS  0x5053
*   #define CFW_STY_FAC_TYPE_P2  0x5032
*   #define CFW_STY_FAC_TYPE_PF  0x5046
*   #define CFW_STY_FAC_TYPE_PN  0x504E
*   #define CFW_STY_FAC_TYPE_PU  0x5055
*   #define CFW_STY_FAC_TYPE_PP  0x5050
*   #define CFW_STY_FAC_TYPE_PC  0x5043
*   #define CFW_STY_FAC_TYPE_FD  0x4644
*
*   +CPWD: ("PS",4),("PF",8),("SC",8),("AO",4),("OI",4),("OX",4),("AI",4),("IR",4),(
*   "AB",4),("AG",4),("AC",4),("PN",8),("PU",8),("PP",8),("PC",8),("P2",8)
*****************************************************************************
*/

// static UINT8 cpwdTestSTR[] = "+CPWD: (\"PS\",4),(\"PF\",8),(\"SC\",8),(\"PN\",8),(\"PU\",8),(\"P2\",8),(\"PC\",8),(\"FD\",8)";
// *************time stamp for asyn event timeout in ATM*************//
static UINT32 u_gCurrentCmdStamp[CFW_SIM_COUNT] = {0x00,};
/*
******************************************************************************************************
#define CFW_STY_AUTH_READY         0  //Phone is not waiting for any password.
#define CFW_STY_AUTH_PIN1_READY        1  //Phone is not waiting for PIN1 password.
#define CFW_STY_AUTH_SIMPIN        2  //Phone is waiting for the SIM Personal Identification Number (PIN)
#define CFW_STY_AUTH_SIMPUK        3  //Phone is waiting for the SIM Personal Unlocking Key (PUK).
#define CFW_STY_AUTH_PHONE_TO_SIMPIN       4  //Phone is waiting for the phone-to-SIM card password.
#define CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN 5  //Phone is waiting for the phone-to-first-SIM card PIN.
#define CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK 6  //Phone is waiting for the phone-to-first-SIM card PUK.
#define CFW_STY_AUTH_SIMPIN2           7  //Phone is waiting for the SIM PIN2.
#define CFW_STY_AUTH_SIMPUK2  ----NO         8  //Phone is waiting for the SIM PUK2.
#define CFW_STY_AUTH_NETWORKPIN        9  //Phone is waiting for the network personalization PIN.
#define CFW_STY_AUTH_NETWORKPUK        10 //Phone is waiting for the network personalization PUK.
#define CFW_STY_AUTH_NETWORK_SUBSETPIN     11 //Phone is waiting for the network subset personalization PIN.
#define CFW_STY_AUTHNETWORK_SUBSETPUK      12 //Phone is waiting for the network subset personalization PUK.
#define CFW_STY_AUTH_PROVIDERPIN       13 //Phone is waiting for the service provider personalization PIN.
#define CFW_STY_AUTH_PROVIDERPUK       14 //Phone is waiting for the service provider personalization PUK.
#define CFW_STY_AUTH_CORPORATEPIN      15 //Phone is waiting for the corporate personalization PIN.
#define CFW_STY_AUTH_CORPORATEPUK      16 //Phone is waiting for the corporate personalization PUK.
#define CFW_STY_AUTH_NOSIM         17 //No SIM inserted.
#define CFW_STY_AUTH_PIN1BLOCK         18
#define CFW_STY_AUTH_PIN2BLOCK         19
#define CFW_STY_AUTH_PIN1_DISABLE      20
#define CFW_STY_AUTH_SIM_PRESENT       21
#define CFW_STY_AUTH_SIM_END           22
********************************************************************************************************
*/
/************CFW_SimGetAuthenticationStatus string********************/
static UINT8 cpinasynSTR[23][20] =
{
    "+CPIN:READY",
    "+CPIN:PIN1 READY",
    "+CPIN:SIM PIN",
    "+CPIN:SIM PUK",
    "+CPIN:PH-SIM PIN", // no  PUK supported!!
    "+CPIN:PH-FSIM PIN",
    "+CPIN:PH-FSIM PUK",
    "+CPIN:SIM PIN2",
    "+CPIN:SIM PUK2",
    "+CPIN:PH-NET PIN",
    "+CPIN:PH-NET PUK",
    "+CPIN:PH-NETSUB PIN",
    "+CPIN:PH-NETSUB PUK",
    "+CPIN:PH-SP PIN",
    "+CPIN:PH-SP PUK",
    "+CPIN:PH-CORP PIN",
    "+CPIN:PH-CORP PUK",
    "+CPIN:NO SIM",
    "+CPIN:PIN1 BLOCK",
    "+CPIN:PIN2 BLOCK",
    "+CPIN:PIN1 DISABLE",
    "+CPIN:SIM PRESENT",
};

/*
********************************************************************************
* because CPIN CPIN2,CPIN2 will call some CFW interface,and wait same event    *
* so following definition is to seperate it                                    *
* ERR_CME_SIM_UNKNOW                                                           *
********************************************************************************
*/
#define CPIN        1
#define CPIN2       2
#define CPINCEXE    3
#define CPINCREAD   4
#define CLCK        5
#define CPWD        6

// for get auth  response ,different cmd call wait same event
static UINT8 AT_security_currentCMD[CFW_SIM_COUNT] = {0x00,};

static UINT8 gLstStatusRec; // 1--->pin1 or puk1,2--->pin2 or puk2
UINT8 gAtSimID = 0;

#ifdef AT_SUPPORT_BT
UINT8 gAtBTFlag = 0;
#endif

/************************cmd realization***************************************/
UINT32 AT_CAMM_STRtoUINT32(UINT8 *pData)
{
    UINT32 iLen = 0;
    UINT32 res  = 0;
    UINT8 *data = pData;
    UINT8 iCnt  = 0;
    UINT8 temp;
    UINT8 t;

    iLen = AT_StrLen(pData);
    if (iLen != 6)
        return ERR_AT_CME_PARAM_INVALID;

    for (iCnt = 0; iCnt < iLen; iCnt++)
    {
        temp = * (data + iCnt);

        if (temp <= 57 && temp >= 48) // '0'-'9'
        {
            t = temp - 48;
            res |= (((UINT32)t) << ((5 - iCnt) * 4));
        }
        else if ((temp <= 102 && temp >= 97) || (temp <= 70 && temp >= 65)) // 'a'-'f','A'-'F'
        {
            if (temp <= 102 && temp >= 97)
                temp -= 32;

            t = 10 + temp - 65;

            res |= (((UINT32)t) << ((5 - iCnt) * 4));
        }
        else
        {
            return ERR_AT_CME_PARAM_INVALID;
        }

    }
    AT_TC(g_sw_AT_SIM, "res = %lu", res);
    return res;
}

// add for crsm begin
#define CRSM_READ_BINARY    176
#define CRSM_READ_RECORD    178
#define CRSM_GET_RESPONSE   192
#define CRSM_UPDATE_BINARY  214
#define CRSM_UPDATE_RECORD  220
#define CRSM_STATUS         242

VOID AT_SIM_Result_OK(UINT32 uReturnValue,
                      UINT32 uResultCode, UINT8 *pBuffer, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = AT_CreateRC(uReturnValue, uResultCode,
                                         CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME,
                                         0, pBuffer, AT_StrLen(pBuffer), nDLCI);
    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, nDLCI);
        AT_FREE(pResult);
    }
}

VOID AT_SIM_Result_Err(UINT32 uErrorCode, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                                         uErrorCode, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0, nDLCI);
    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, nDLCI);
        AT_FREE(pResult);
    }
}

UINT32 gCrsmCommandDataLen[CFW_SIM_COUNT] = {0x00,};
UINT32 ATGetRealFileID(UINT16 nFileID,UINT16 nCurrentIndex,CFW_SIM_ID nSimID)
{
    if((API_USIM_EF_GB_ADN == nFileID)||(API_USIM_EF_ADN == nFileID))
    {
        if(nCurrentIndex > CFW_GetPBKRecordNum(nSimID))
            return nFileID + 1;
        else
            return nFileID;
    }
    else
    {
        return nFileID;
    }
}

UINT32 ATGetRealCurrIndex(UINT16 nFileID,UINT16 nCurrentIndex,CFW_SIM_ID nSimID)
{
    if((API_USIM_EF_GB_ADN == nFileID) || (API_USIM_EF_ADN == nFileID))
    {
        if(nCurrentIndex > CFW_GetPBKRecordNum(nSimID))
            return nCurrentIndex - CFW_GetPBKRecordNum(nSimID);
        else
            return nCurrentIndex;
    }
    else
    {
        return nCurrentIndex;
    }
}

//Add access USIM function with full path, the syntax is the same as 3GPP except for following:
//first parameter: command = 3GPP command + 1, for example: READ BINARY command = 177, this used to read USIM EF
//second parameter: String parameter to represent full path EF ID, just need one level directory close the EF,
//                              for example: 5F3B4F52
//Other parameter is the same as 3GPP.
//
//at+crsm=179,"5F3A4F3A",1,4,28     //read ADN
//at+crsm=243,"5F3B4F52",0,0,32     //read status of KC_GPRS

//AT+CRSM=193,"7FFF6F05",0,0,32
//AT+CRSM=193,"7FFF6FDD",0,0,32
//AT+CRSM=193,"5F3C4F40",0,0,32
VOID AT_SIM_CmdFunc_CRSM(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount        = 0;
    UINT32 eParamOk        = 0;
    UINT8 uCommand         = 0;
    UINT32 uiP1                  = 0;
    UINT32 uiP2                  = 0;
    UINT32 uiP3                  = 0;
    UINT8 usData[CRMPDATALENGTH] = { 0 }; // /????????data长度应该是多少?
    UINT8 nFullPathFileID[20] = {0};
    UINT32 nEFID              = 0;
    UINT32 nEFPath             = 0;

    UINT16 uStrLen                = 0;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if ((uParaCount > 6) || (uParaCount < 1) ||(uParaCount == 3 || uParaCount == 4)) // if param count equal to 3 or 4,then the fifth param must be filled
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID,pParam->nDLCI);
            return;
        }
        uStrLen = 1;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &uCommand, &uStrLen);
        if (eParamOk != ERR_SUCCESS)  /* get uCallType failed */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (uCommand != CRSM_STATUS && uParaCount < 2)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (((uCommand & 0xFE) != CRSM_GET_RESPONSE) && (uCommand != CRSM_STATUS) && uParaCount == 2)  // except STATUS and GET RESPONSE, <P1>,<P2>,<P3> is needed.
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (uParaCount > 1)
        {
            if((uCommand & 0x01) == 0)  //access EF with FileID
            {
                uStrLen  = 4;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT32, &nEFID, &uStrLen);
                if (eParamOk != ERR_SUCCESS)
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
                nEFPath = nEFID >> 16;      //get parent directory
                nEFID = nEFID & 0xFFFF;
            }
            else    //access EF with FileID of FULL path
            {
                uStrLen  = 20;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &nFullPathFileID, &uStrLen);
                if((eParamOk != ERR_SUCCESS) || (uStrLen < 8))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
                //get uiFileID
                uStrLen = SUL_ascii2hex((UINT8*)(nFullPathFileID+ AT_StrLen(nFullPathFileID) - 4), (UINT8*)&nEFID);
                if(uStrLen == 0)
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
                nEFID = ((nEFID >>8) & 0xFF) | ((nEFID<<8)&0xFF00);
                //get uiPath
                uStrLen = AT_StrLen(nFullPathFileID) -4;
                nFullPathFileID[uStrLen] = 0;
                uStrLen = SUL_ascii2hex((UINT8*)(nFullPathFileID+ AT_StrLen(nFullPathFileID) - 4), (UINT8*)&nEFPath);
                if(uStrLen == 0)
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
                nEFPath = nEFPath& 0xFFFF;
                nEFPath = ((nEFPath >>8) & 0xFF) | ((nEFPath<<8)&0xFF00);

                AT_TC(g_sw_AT_SAT, "uCommand = %d, nEFPath = %x, nEFID = %x\n",uCommand, nEFPath, nEFID);
            }

            if (uParaCount > 2)
            {
                uStrLen  = 4;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT32, &uiP1, &uStrLen);
                if (eParamOk != ERR_SUCCESS)  /* get uType failed */
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                uStrLen = 4;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT32, &uiP2, &uStrLen);
                if (eParamOk != ERR_SUCCESS)  /* get uType failed */
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                uStrLen = 4;
                eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT32, &uiP3, &uStrLen);
                if ((eParamOk != ERR_SUCCESS) || (uiP3 > 0xFF) || (uiP3 < 0)) /* get uType failed */
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                if (uParaCount > 5)
                {
                    uStrLen  = CRMPDATALENGTH;
                    eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 5, AT_UTIL_PARA_TYPE_STRING, usData, &uStrLen);
                    if (eParamOk != ERR_SUCCESS)  /* get uIndex failed */
                    {
                        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                        return;
                    }
                }
            }
        }

        AT_TC(g_sw_AT_SIM,
              "uParaCount = %d,uCommand = %d,nEFID = %d,uiP1 = %d,uiP2 = %d,uiP3 = %d,usData = %s,uStrLen = %d",
              uParaCount, uCommand, nEFID, uiP1, uiP2, uiP3, usData, uStrLen);
        UINT16 nFileID = CFW_GetStackSimFileID(nEFID, nEFPath, nSim);
        if(nFileID == 0xFFFF)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        AT_TC(g_sw_AT_SIM,"EF ID = %d",nFileID);
        UINT32 nRet = 0;
        gCrsmCommandDataLen[nSim] = uiP3;
        switch (uCommand&0xFE)
        {
        case CRSM_READ_BINARY:
        {
            nRet = CFW_SimReadBinary(nFileID, ((uiP1) << 8) + uiP2, uiP3, pParam->nDLCI, nSim);
        }
        break;
        case CRSM_UPDATE_BINARY:
        {
            uStrLen = uiP3;
            UINT8* pOutput = AT_MALLOC(uStrLen);
            if(pOutput == NULL)
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            UINT8 i = uiP3 << 1;
            if(i < strlen(usData))
                usData[i] = 0;

            if(0 == SUL_ascii2hex(usData, pOutput))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
            nRet = CFW_SimUpdateBinary(nFileID, ((uiP1) << 8) + uiP2, pOutput, uiP3, pParam->nDLCI, nSim);
            AT_FREE(pOutput);
        }
        break;

        case CRSM_READ_RECORD:
        {
            nRet = CFW_SimReadRecord(nFileID, uiP1, pParam->nDLCI, nSim);
        }
        break;

        case CRSM_UPDATE_RECORD:
        {
            uStrLen = uiP3;
            UINT8* pOutput = AT_MALLOC(uStrLen);
            if(pOutput == NULL)
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            UINT8 i = uiP3 << 1;
            if(i < strlen(usData))
                usData[i] = 0;

            if(0 == SUL_ascii2hex(usData, pOutput))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            if(0x6F3B == nEFID)             //FDN file
            {
                AT_TC(g_sw_AT_SIM,"---AT_FDN_UpdateRecord---");
                AT_FDN_UpdateRecord(uiP1, uiP3, pOutput, nSim);
            }

            AT_TC(g_sw_AT_SIM,"((uiP1) << 8) + uiP2 = %x",((uiP1) << 8) + uiP2);
            AT_TC(g_sw_AT_SIM,"uiP3 = %x",uiP3);
            nRet = CFW_SimUpdateRecord(nFileID, uiP1, uiP3, pOutput, pParam->nDLCI, nSim);
            AT_FREE(pOutput);
        }
        break;

        case CRSM_GET_RESPONSE:
        case CRSM_STATUS:
        {
            nRet = CFW_SimGetFileStatus(nFileID, pParam->nDLCI, nSim);
        }
        break;

        default:
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        }

        if (ERR_SUCCESS != nRet)
        {
            AT_TC(g_sw_AT_SIM, "nRet = 0x%x", nRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
        return;
    }
}

typedef struct _AT_CRSML
{
    UINT32 nStartIndex;
    UINT32 nCount;
    UINT32 nCurrentIndex;
    UINT32 nLen;
    UINT32 nFileID;
} AT_CRSML;
AT_CRSML gCrsml[CFW_SIM_COUNT];

VOID AT_SIM_CmdFunc_CRSML(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount             = 0;
    UINT32 eParamOk              = 0;
    UINT32 nEFFileID                  = 0;
    UINT32 uiP3                       = 0;
    UINT16 uStrLen                = 0;

    UINT32 nStartIndex = 0x00;
    UINT32 nCount = 0x00;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    if (pParam->iType == AT_CMD_READ)
    {
        UINT8 pADNTotalNum[20] = {0x00,};
        AT_Sprintf(pADNTotalNum, "PBK :%d", CFW_GetPBKTotalNum(nSim));
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pADNTotalNum,pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (3 != uParaCount)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        /* get all param */
        uStrLen = 4;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT32, &nEFFileID, &uStrLen);
        if (eParamOk != ERR_SUCCESS)  /* get uCallType failed */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        uStrLen  = 4;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT32, &nStartIndex, &uStrLen);
        if (eParamOk != ERR_SUCCESS)  /* get uType failed */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        uStrLen = 4;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT32, &nCount, &uStrLen);
        if (eParamOk != ERR_SUCCESS)  /* get uType failed */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        AT_TC(g_sw_AT_SIM,
              "uParaCount = %d,nFileID = %d,nStartIndex = %d,nCount = %d", uParaCount, nEFFileID, nStartIndex, nCount);
        UINT32 nRet = 0x00;
        // call csw function
        // CFW_XXXXXXXX();
        gCrsmCommandDataLen[nSim] = uiP3;
        gCrsml[nSim].nCount = nCount;
        gCrsml[nSim].nStartIndex  = nStartIndex;
        gCrsml[nSim].nCurrentIndex = nStartIndex;
        gCrsml[nSim].nFileID = nEFFileID;

        UINT16 nFileID = CFW_GetStackSimFileID(gCrsml[nSim].nFileID & 0XFFFF, gCrsml[nSim].nFileID >> 16, nSim);
        if(nFileID == 0xFFFF)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        nRet = CFW_SimGetFileStatus(nFileID, pParam->nDLCI, nSim);
        if (ERR_SUCCESS != nRet)
        {
            AT_TC(g_sw_AT_SIM, "nRet = 0x%x", nRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    else //if (pParam->iType == AT_CMD_TEST)
    {
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
        return;
    }
}

VOID AT_SIM_CmdFunc_CAMM(AT_CMD_PARA *pParam)
{
    UINT8 uParaCount                 = 0;
    UINT32 eParamOk                  = 0;
    UINT32 iACMMax                   = 0;
    UINT8 usACMMax[CAMMLENGTH]       = { 0 }; // /????????data长度应该是多少?
    UINT8 usPassword[PIN2CODELENGTH] = { 0 };
    UINT16 uStrLen                    = 0;
    UINT32 nResult                   = 0;
    //  UINT8 nUTI                       = pParam->nDLCI;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        nResult = CFW_SimGetACMMax(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == nResult)
        {
            AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
            return;
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
            return;
        }
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if ((pParam->pPara == NULL) || (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &uParaCount))) /* GET param count */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (uParaCount != 2)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        uStrLen = CAMMLENGTH;
        eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, usACMMax, &uStrLen);
        if (eParamOk != ERR_SUCCESS)  /* get uCallType failed */
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (uParaCount > 1)
        {

            uStrLen  = PIN2CODELENGTH;
            eParamOk = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &usPassword, &uStrLen);
            if (eParamOk != ERR_SUCCESS)  /* get uVolume failed */
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
        }
        AT_TC(g_sw_AT_SIM, "uParaCount = %d,usACMMax = %s,usPassword = %s,uStrLen = %d", uParaCount,
              usACMMax, usPassword, uStrLen);

        iACMMax = AT_CAMM_STRtoUINT32(usACMMax);
        if (iACMMax == ERR_AT_CME_PARAM_INVALID)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        //format of iACMMax is 0x00FFFFFF
        AT_TC(g_sw_AT_SIM, "uParaCount = %d,usACMMax = %s,usPassword = %s,uStrLen = %d,iACMMax = %lu", uParaCount, usACMMax,
              usPassword, uStrLen, iACMMax);
        nResult = CFW_SimSetACMMax(iACMMax, usPassword, uStrLen, pParam->nDLCI, nSim);
        // execute result return
        if (ERR_SUCCESS == nResult)
        {
            AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
            return;
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_EXE_FAIL, pParam->nDLCI);
            return;
        }
    }
    else  // changyg[+] 2008-6-12 default wrong process for bug 8758
    {
        AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
        return;
    }
}

//AT+SIMIF=<type>,<mode>
//===============================
//|                 |   0     |    1      |
//|  mode      | value  | text    |
//-------------------------------------
//|       |        |   0     |   SIM   |
//|       | 1     ------------------------------
//|       |        |   1     |  UICC  |
//|type| -------------------------------
//|       |todo...
//|===============================
UINT8 g_simif_type = 1;
UINT8 g_simif_mode = 0;
VOID AT_SIM_CmdFunc_SIMIF(AT_CMD_PARA *pParam)
{
    UINT8 Count = 0;
    UINT32 uErrCode  = ERR_AT_CME_PARAM_INVALID;
    UINT8 nResp[50]= {0};

    if((pParam == NULL) ||(pParam->pPara == NULL))
    {
        AT_TC(g_sw_AT_SIM, "SIMIF: (pParam == NULL) ||(pParam->pPara == NULL)");
        goto FAILED_EXIT;
    }

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    AT_TC(g_sw_AT_SIM, "pParam->iType = %d", pParam->iType);
    if (pParam->iType == AT_CMD_SET)
    {
        if(ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &Count))
        {
            AT_TC(g_sw_AT_SIM, "SIMIF: AT_Util_GetParaCount return Error!");
            goto FAILED_EXIT;
        }
        if((Count != 1) && (Count != 2))
        {
            AT_TC(g_sw_AT_SIM, "SIMIF: AT_Util_GetParaCount = %d", Count);
            goto FAILED_EXIT;
        }

        BOOL simType = CFW_GetSimType(nSim); //0: sim; 1:Usim
        UINT8 type = 0xFF;
        UINT16 length = 1;
        if(ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &type, &length))
        {
            AT_TC(g_sw_AT_SIM, "SIMIF: AT_Util_GetParaWithRule get first parameter return Error ");
            goto FAILED_EXIT;
        }
        if(type != 1)           //the first parameter just support 1 in this version, other for the furture expansion.
            goto FAILED_EXIT;
        g_simif_type = type;
        AT_TC(g_sw_AT_SIM, "SIMIF: type = %d", type);

        UINT8 mode = 0xFF;
        length = 1;
        if (Count == 2)
        {
            if(ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &mode, &length))
            {
                AT_TC(g_sw_AT_SIM, "SIMIF: AT_Util_GetParaWithRule get second parameter return Error ");
                goto FAILED_EXIT;
            }
            if(mode > 1)
                goto FAILED_EXIT;
            g_simif_mode = mode;
            AT_TC(g_sw_AT_SIM, "SIMIF: mode = %d", mode);
        }

        if((mode == 0) ||(mode == 0xFF))
        {
            AT_Sprintf(nResp,"^SIMIF: %d",simType);
        }
        else        //mode == 1
        {
            UINT8* pType;
            if(simType == 0)
                pType = "SIM";
            else
                pType = "UICC";

            AT_Sprintf(nResp,"^SIMIF: %s",pType);
        }
        goto SUCC_EXIT;
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        AT_Sprintf(nResp,"^SIMIF: (1), (0,1)");
        goto SUCC_EXIT;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_Sprintf(nResp,"^SIMIF: %d,%d", g_simif_type, g_simif_mode);
        goto SUCC_EXIT;
    }
    else
    {
        uErrCode = ERR_AT_CME_EXE_NOT_SURPORT;
    }

FAILED_EXIT:
    AT_SIM_Result_Err(uErrCode, pParam->nDLCI);
    return;

SUCC_EXIT:
    AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, pParam->nDLCI);
    return;
}

/*
 *******************************************************************************
 *  possible input from ATM:                                                   *
 *  AT+cpin=? -->test, return OK;                                              *
 *  AT+cpin? --> read,wait asyncrounous event,response e.g.:                   *
 *          ---> "+CPIN: SIM PIN","+CPIN: READY"                               *
 *  AT+cpin="1234"(4<pinSize<8) -->reponse:OK,ERROR                            *
 *  AT+cpin="12345678","1234"(pukSize=8),(4<pinSize<8) -->reponse:OK,ERROR     *
 *******************************************************************************
*/
void AT_SIM_CmdFunc_CPIN(AT_CMD_PARA *pParam)
{
    UINT32 return_val;
    UINT16 i                    = 20;
    UINT8 NumOfParam           = 0;
    UINT8 uPin[PIN1CODEMAX]    = { 0 };
    UINT8 nPinSize             = 0;
    UINT8 uNewPin[PIN1CODEMAX] = { 0 };
    UINT8 nNewPinSize          = 0;
    UINT8 nOption              = 1;

    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_1!");
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;
    UINT8 nUTI = pParam->nDLCI;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        // AT_CMD_SET-------------------->
        /********************************************************
             *  UINT32 CFW_SimEnterAuthentication(      *
             *                UINT8* pPin,      *
             *                UINT8  nPinSize,  *
             *                UINT8* pNewPin,   *
             *                UINT8  nNewPinSize,   *
             *                UINT8  nOption,   *
             *                UINT16 nUTI);     *
             ********************************************************/
        if (NULL == pParam->pPara)
        {
            AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_2!");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        // 2. get parameter count and check it.  if error ,return.
        if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &NumOfParam))
        {
            AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_3!");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if ((NumOfParam != 1) && (NumOfParam != 2))
        {
            AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_4!");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        i = PIN1CODEMAX;  // 7.24  changed form 15 to PIN1CODEMAX.

        // 3.1 get and check it.!if error return.
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uPin, &i))
        {
            AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_5!");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        // 3.3 update nPinSize for call  CFW interface
        nPinSize = (UINT8)AT_StrLen(uPin);
        if ((nPinSize > 8) || (nPinSize < 1))
        {
            AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_6!");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        for (i = 0; i < nPinSize; i++)
        {
            if ((*(uPin + i) < '0') || (*(uPin + i) > '9'))
            {
                AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_7!");
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
        } // ----------->the first parameter is correct.

        // 4.if  there are two paras, get and check it.
        if (NumOfParam == 2)  //
        {
            i = PIN1CODEMAX;
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, uNewPin, &i))
            {
                AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_8!");
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            // 4.3 update nNewPinSize for call CFW interface
            nNewPinSize = (UINT8)AT_StrLen(uNewPin);
            if ((nNewPinSize > 8) || (nNewPinSize < 1))
            {
                AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_9!");
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            for (i = 0; i < nNewPinSize; i++)
            {
                if ((*(uNewPin + i) < '0') || (*(uNewPin + i) > '9'))
                {
                    AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_10!");
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
        } // -------->end if two parameter
        else
        {
            nNewPinSize = 0;
            // nOption = 0; yaoal;7.24; delete this for cpin  can accept cpin2.
            // add by wangxd for bug8046
#if SIM_DEBUG
            AT_TC(g_sw_AT_SIM, "CPIN g_Pin1PukStauts=%d,g_Pin1ErrorTimes=%d", g_Pin1PukStauts[nSim], g_Pin1ErrorTimes[nSim]);
#endif
            if (g_Pin1PukStauts[nSim])
            {
                AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_11!");
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
        }
        if (ERR_SUCCESS != AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI))
        {
            AT_TC(g_sw_AT_SIM, "_CPIN_PARAM_12!");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        // 6. call CFW interface
        if ((nNewPinSize > 0) && (nPinSize != 8))
        {
            AT_TC(g_sw_AT_SIM, "Length of PUK code should be 8, but it is %d",nPinSize);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        else
        {
            AT_TC(g_sw_AT_SIM, "Enter CFW_SimEnterAuthentication!");
            return_val = CFW_SimEnterAuthentication(uPin, nPinSize, uNewPin, nNewPinSize, nOption, pParam->nDLCI, nSim);
            if( return_val != ERR_SUCCESS )
            {
                if (ERR_CFW_INVALID_PARAMETER == return_val)
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
                else if (ERR_NO_MORE_MEMORY == return_val)
                {
                    AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
                    return;
                }
                else
                {
                    AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
                    return;
                }
            }
        }
        gLstStatusRec = 1;
        AT_security_currentCMD[nSim] = CPIN;
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN,CMD_RC_OK,"", pParam->nDLCI);
        return;
    }
    break;

    case AT_CMD_TEST:
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC,CMD_RC_OK,"", pParam->nDLCI);
        return;
    }

    case AT_CMD_READ:
    {
#if SIM_DEBUG
        return_val = AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI);
        if (ERR_SUCCESS != return_val)
        {
            AT_SIM_Result_Err(return_val, pParam->nDLCI);
            return;
        }

        AT_TC(g_sw_AT_SIM, "UTI =    %d", nUTI);
#endif
        nUTI = SIM_UTI;
        return_val = CFW_SimGetAuthenticationStatus(pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else if ((ERR_CME_SIM_NOT_INSERTED == return_val) || (ERR_CFW_SIM_NOT_INITIATE == return_val))
                AT_SIM_Result_Err(ERR_AT_CME_SIM_NOT_INSERTED, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }

        AT_security_currentCMD[nSim] = CPIN;
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    }
}

/*
 *******************************************************************************
 *  possible input from ATM:
 *  AT+cpin2=? -->test, return OK;
 *  AT+cpin2? --> read,wait asyncrounous event,response e.g.:
 *            --> "+CPIN: SIM PIN","+CPIN: READY"
 *  AT+cpin2="1234"(4<pinSize<8)    -->reponse:OK,ERROR
 *  AT+cpin2="12345678","1234"(pukSize=8),(4<pinSize<8) -->reponse:OK,ERROR
 *  remain problem:
 *  1.if data from UART is larger than actual buffer
 *******************************************************************************
*/
void AT_SIM_CmdFunc_CPIN2(AT_CMD_PARA *pParam)
{
    UINT32 return_val;
    UINT16 i           = 20;
    UINT8 NumOfParam  = 0;
    UINT8 uPin[20]    = { 0 };
    UINT8 nPinSize    = 0;
    UINT8 uNewPin[20] = { 0 };
    UINT8 nNewPinSize = 0;
    UINT8 nOption     = 2;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;
    UINT8 nUTI = pParam->nDLCI;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        if (NULL == pParam->pPara)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &NumOfParam))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if ((NumOfParam != 1) && (NumOfParam != 2))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uPin, &i))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        nPinSize = (UINT8)AT_StrLen(uPin);
        if ((nPinSize > 8) || (nPinSize < 1))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        for (i = 0; i < nPinSize; i++)
        {
            if ((*(uPin + i) < '0') || (*(uPin + i) > '9'))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

        }
        if (NumOfParam == 2)
        {
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, uNewPin, &i))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            // 4.3 update nNewPinSize for call CFW interface
            nNewPinSize = (UINT8)AT_StrLen(uNewPin);
            if ((nNewPinSize > 8) || (nNewPinSize < 1))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            for (i = 0; i < nNewPinSize; i++)
            {
                if ((*(uNewPin + i) < '0') || (*(uNewPin + i) > '9'))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
        }
#if SIM_DEBUG
        if (ERR_SUCCESS != AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
#else
        nUTI = SIM_UTI;
#endif
        return_val = CFW_SimEnterAuthentication(uPin, nPinSize, uNewPin, nNewPinSize, nOption, pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }
        gLstStatusRec = 2;
        AT_security_currentCMD[nSim] = CPIN2;

        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    break;

    case AT_CMD_TEST:
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    break;

    case AT_CMD_READ:
    {
#if SIM_DEBUG
        return_val = AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI);
        if (ERR_SUCCESS != return_val)
        {
            AT_SIM_Result_Err(return_val, pParam->nDLCI);
            return;
        }
#else
        nUTI = SIM_UTI;
#endif
        return_val = CFW_SimGetAuthenticationStatus(pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }
        AT_security_currentCMD[nSim] = CPIN2;

        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    }
}

/*
*******************************************************************************
*  Write: AT+CLCK=<fac>,<mode>[,<passwd>[,<class>]]
*  Test:  AT+CLCK=?
*  possible input from ATM:
*  AT+CLCK=?            -->res:+CLCK: ("PS","PF","SC","AO","OI","OX","AI",
*                "IR","AB","AG","AC","FD","PN","PU","PP","PC")
*                OK
*  AT+CLCK="SC",1,"1234"---->reponse:OK,ERROR
*  AT+CLCK="SC",2       ---->read,res:1.ERROR(when need to enter pin),2.+clck:1(No need to enter pin)
*  AT+CLCK="SC",0,"1234"---->reponse:OK,ERROR
*******************************************************************************
*/
void AT_SIM_CmdFunc_CLCK(AT_CMD_PARA *pParam)
{
    UINT32 return_val = 0;
    UINT16 i                 = 20;
    UINT8 NumOfParam        = 0;
    UINT8 uFAC[FAC_MAX_NUM] = { 0 };
    UINT16 nFac;
    UINT8 uBufPwd[CLCK_PWD_MAX] = { 0 };
    UINT8 nPwdSize              = 0;
    UINT8 nClassx               = 0;
    UINT8 nMode;

    AT_TC(g_sw_AT_SIM, "CLCK: start\n");
    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 nUTI        = pParam->nDLCI;
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        if (NULL == pParam->pPara)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &NumOfParam))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if ((NumOfParam != 2) && (NumOfParam != 3) && (NumOfParam != 4))
        {
            AT_TC(g_sw_AT_SIM, "CLCK: error num range!\n");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        i = FAC_MAX_NUM;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uFAC, &i))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        UINT8 FACsize = (UINT8)AT_StrLen(uFAC);
        if (2 != FACsize)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        /***************************************************************************
            *   #define CFW_STY_FAC_TYPE_SC  0x5343 --support
            *   #define CFW_STY_FAC_TYPE_PS  0x5053
            *   #define CFW_STY_FAC_TYPE_P2  0x5032
            *   #define CFW_STY_FAC_TYPE_PF  0x5046
            *   #define CFW_STY_FAC_TYPE_PN  0x504E
            *   #define CFW_STY_FAC_TYPE_PU  0x5055
            *   #define CFW_STY_FAC_TYPE_PP  0x5050
            *   #define CFW_STY_FAC_TYPE_PC  0x5043
            *   #define CFW_STY_FAC_TYPE_FD  0x4644 --support
            ****************************************************************************/
        nFac = * (UINT16 *)uFAC;
        nFac = U16_SWAP(nFac);
        switch (nFac)
        {
        case CFW_STY_FAC_TYPE_AO:
        case CFW_STY_FAC_TYPE_OI:
        case CFW_STY_FAC_TYPE_OX:
        case CFW_STY_FAC_TYPE_AI:
        case CFW_STY_FAC_TYPE_IR:
        case CFW_STY_FAC_TYPE_AB:
        case CFW_STY_FAC_TYPE_AG:
        case CFW_STY_FAC_TYPE_AC:
        case CFW_STY_FAC_TYPE_SC:
        case CFW_STY_FAC_TYPE_PS:
        case CFW_STY_FAC_TYPE_P2:
        case CFW_STY_FAC_TYPE_PF:
        case CFW_STY_FAC_TYPE_PN:
        case CFW_STY_FAC_TYPE_PU:
        case CFW_STY_FAC_TYPE_PP:
        case CFW_STY_FAC_TYPE_PC:
        case CFW_STY_FAC_TYPE_FD:
            break;
        default:
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        }

        i = 1;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nMode, &i))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (!((((0 == nMode) || (1 == nMode)) && ((3 == NumOfParam) || (4 == NumOfParam)))
                || ((2 == nMode) && ((2 == NumOfParam) || (3 == NumOfParam)))))
        {
            AT_TC(g_sw_AT_SIM, "CLCK: error 2 para range!\n");
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (NumOfParam >= 3)
        {
            if (2 == nMode)
            {
                i = 1;
                if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &nClassx, &i))
                {
                    AT_TC(g_sw_AT_SIM, "CLCK: error 3 para mode ==2!\n");
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
            else
            {
                i = CLCK_PWD_MAX;
                if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, uBufPwd, &i))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                nPwdSize = (UINT8)AT_StrLen(uBufPwd);
                if ((nPwdSize > 8) || (nPwdSize < 1))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }

                for (i = 0; i < nPwdSize; i++)
                {
                    if ((*(uBufPwd + i) < '0') || (*(uBufPwd + i) > '9'))
                    {
                        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                        return;
                    }
                }
            }
        }
        if (NumOfParam == 4)
        {
            i = 1;
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &nClassx, &i))
            {
                AT_TC(g_sw_AT_SIM, "CLCK: error 4 para!\n");
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
        }
#if SIM_DEBUG
        if (ERR_SUCCESS != AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI))
        {
            AT_TC(g_sw_AT_SIM, "CLCK:NO UTI! error=0x%x", return_val);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
#else
        nUTI = SIM_UTI;
#endif
        // 6. if mode == 0 or mode == 1  call CFW interface SetFacilityLock
        if ((0 == nMode) || (1 == nMode))
        {
            if((nFac == CFW_STY_FAC_TYPE_SC) || (nFac == CFW_STY_FAC_TYPE_FD))
            {
                /* CFW_SimSetFacilityLock(UINT16  nFac, UINT8 * pBufPwd, UINT8 nPwdSize, UINT8 nMode, UINT16 nUTI) */
                AT_TC(g_sw_AT_SIM, "Set Facility Lock nFac:0x%x,uBufPwd:%s,nPwdSize:0x%x,nMode:0x%x\n", nFac, uBufPwd, nPwdSize,
                      nMode);
                return_val = CFW_SimSetFacilityLock(nFac, uBufPwd, nPwdSize, nMode, pParam->nDLCI, nSim);
            }
            else
            {
                return_val = CFW_SsSetFacilityLock(nFac, uBufPwd, nPwdSize, nClassx, nMode, pParam->nDLCI, nSim);
            }
        }
        else
        {
            if((nFac == CFW_STY_FAC_TYPE_SC) || (nFac == CFW_STY_FAC_TYPE_FD))
            {
                return_val = CFW_SimGetFacilityLock(nFac, pParam->nDLCI, nSim);
            }
            else
            {
                switch(nClassx)
                {
                //Specify the sum of class to represent the class information.
                //Reference to csw development document
                case 1:
                    nClassx = 11;
                    break;
                case 2:
                    nClassx = 16;
                    break;
                case 4:
                    nClassx = 13;
                    break;
                case 7:
                    nClassx = 20;
                    break;
                default:
                    break;
                }
                return_val = CFW_SsQueryFacilityLock(nFac, nClassx, pParam->nDLCI, nSim);
            }
        }
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }
        if (nFac == CFW_STY_FAC_TYPE_SC)
            gLstStatusRec = 1;
        else if (nFac == CFW_STY_FAC_TYPE_FD)
            gLstStatusRec = 2;
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    break;

    case AT_CMD_TEST:
    {
        UINT8* pResp = "+CLCK: (\"SC\",\"FD\",\"AO\",\"OX\",\"OI\")";
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pResp, pParam->nDLCI);
    }
    break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    } // <----------end of switch cmd  type-------
}

/*
 *******************************************************************************
 *  AT+CPWD=<fac>,<oldpwd>,<newpwd>; -->OK,ERROR
 *  AT+CPWD=<fac>,<newpwd>;
 *  AT+CPWD=?   -->+CPWD: list of supported (<fac>,<pwdlength>)s
 *  possible input from ATM:
 *  AT+CPWD=?  -->test, return OK;
 *******************************************************************************
*/
void AT_SIM_CmdFunc_CPWD(AT_CMD_PARA *pParam)
{
    UINT32 return_val;
    UINT16 i                 = 20; // a temp
    UINT8 NumOfParam        = 0; // NumOfParam from pParam,UART's input
    UINT8 uFAC[FAC_MAX_NUM] = { 0 };
    UINT8 numofpFAC;
    UINT16 nFac;
    UINT8 uBufOldPwd[PIN1CODEMAX] = { 0 };
    UINT8 nOldPwdSize             = 0;
    UINT8 uBufNewPwd[PIN1CODEMAX] = { 0 };
    UINT8 nNewPwdSize             = 0;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    UINT8 nUTI = pParam->nDLCI;
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        AT_TC(g_sw_AT_SIM, "---1---");
        if (NULL == pParam->pPara)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if (ERR_SUCCESS != AT_Util_GetParaCount(pParam->pPara, &NumOfParam))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if ((NumOfParam != 3) && (NumOfParam != 2))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        i = FAC_MAX_NUM;
        if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, uFAC, &i))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        numofpFAC = (UINT8)AT_StrLen(uFAC);
        if (2 != numofpFAC)
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        /***************************************************************************
            *   #define CFW_STY_FAC_TYPE_SC  0x5343   ---supported
            *   #define CFW_STY_FAC_TYPE_PS  0x5053
            *   #define CFW_STY_FAC_TYPE_P2  0x5032   ---supported
            *   #define CFW_STY_FAC_TYPE_PF  0x5046
            *   #define CFW_STY_FAC_TYPE_PN  0x504E
            *   #define CFW_STY_FAC_TYPE_PU  0x5055
            *   #define CFW_STY_FAC_TYPE_PP  0x5050
            *   #define CFW_STY_FAC_TYPE_PC  0x5043
            *   #define CFW_STY_FAC_TYPE_FD  0x4644
            ****************************************************************************/
        nFac = * (UINT16 *)uFAC;
        nFac = U16_SWAP(nFac);
        switch (nFac)
        {
        case CFW_STY_FAC_TYPE_SC:
        // case CFW_STY_FAC_TYPE_PS :
        case CFW_STY_FAC_TYPE_P2:
            // case CFW_STY_FAC_TYPE_PF :
            // case CFW_STY_FAC_TYPE_PN :
            // case CFW_STY_FAC_TYPE_PU :
            // case CFW_STY_FAC_TYPE_PP :
            // case CFW_STY_FAC_TYPE_PC :
            // case CFW_STY_FAC_TYPE_FD :
            break;

        default:
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        }

        if (NumOfParam == 2)
        {
            i = PIN1CODEMAX;
            AT_TC(g_sw_AT_SIM, "---4---");
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, uBufNewPwd, &i))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            nNewPwdSize = (UINT8)AT_StrLen(uBufNewPwd);
            AT_TC(g_sw_AT_SIM, "---5---");
            if ((nNewPwdSize > 8) || (nNewPwdSize < 1))
            {
                AT_TC(g_sw_AT_SIM, "(nNewPwdSize > 8) || (nNewPwdSize < 1) New pin size = %d", nNewPwdSize);
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }
            AT_TC(g_sw_AT_SIM, "---6---");
            for (i = 0; i < nNewPwdSize; i++)
            {
                if ((*(uBufNewPwd + i) < '0') || (*(uBufNewPwd + i) > '9'))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
        }
        else
        {
            i = PIN1CODEMAX;
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, uBufOldPwd, &i))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            nOldPwdSize = (UINT8)AT_StrLen(uBufOldPwd);
            if ((nOldPwdSize > 8) || (nOldPwdSize < 1))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            for (i = 0; i < nOldPwdSize; i++)
            {
                if ((*(uBufOldPwd + i) < '0') || (*(uBufOldPwd + i) > '9'))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
            i = PIN1CODEMAX;
            if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, uBufNewPwd, &i))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            nNewPwdSize = (UINT8)AT_StrLen(uBufNewPwd);
            if ((nNewPwdSize > 8) || (nNewPwdSize < 1))
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                return;
            }

            for (i = 0; i < nNewPwdSize; i++)
            {
                if ((*(uBufNewPwd + i) < '0') || (*(uBufNewPwd + i) > '9'))
                {
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
                    return;
                }
            }
        }
#if SIM_DEBUG
        if (ERR_SUCCESS != AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
#else
        nUTI = SIM_UTI;
#endif
        AT_TC(g_sw_AT_SIM, "---7---");
        return_val = CFW_SimChangePassword(nFac, uBufOldPwd, nOldPwdSize, uBufNewPwd, nNewPwdSize, pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {

            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }
        if (nFac == CFW_STY_FAC_TYPE_SC)
            gLstStatusRec = 1;
        else if (nFac == CFW_STY_FAC_TYPE_P2)
            gLstStatusRec = 2;
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    break;

    case AT_CMD_TEST:
    {
        UINT8* pResp =  "+CPWD: (\"SC\",8),(\"P2\",8)";
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pResp, pParam->nDLCI);
        return;
    }
    break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    } // <----------end of switch cmd  type-------
}

/*
 *******************************************************************************
 *  possible input from ATM:
 *  AT^cpinc=? -->test, return OK;
 *  AT^cpinc?  --> read,wait asyncrounous event,response e.g.:
 *             --> "^SPIC: SIM PIN2"
 *  AT^cpinc   --> exe  -->reponse:  "^SPIC: 3"
 *  AT^cpinc="SC" ---we didn't support this
 *******************************************************************************
*/
void AT_SIM_CmdFunc_CPINC(AT_CMD_PARA *pParam)
{
    UINT32 return_val;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nUTI = pParam->nDLCI;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
    {
#if SIM_DEBUG
        return_val = AT_GetFreeUTI(CFW_SIM_SRV_ID, &nUTI);
        if (ERR_SUCCESS != return_val)
        {
            AT_SIM_Result_Err(return_val, pParam->nDLCI);
            return;
        }
#else
        nUTI = SIM_UTI;
#endif
        return_val = CFW_SimGetAuthenticationStatus(pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_CFW_INVALID_PARAMETER == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            else if (ERR_CME_OPERATION_NOT_ALLOWED == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
            else if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }

        AT_security_currentCMD[nSim] = CPINCEXE;
        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }
    case AT_CMD_TEST:
    {
        UINT8* pResp = "^CPINC: PIN1&PIN2: (1-3), PUK1&PUK2: (1-10)";
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, pResp, pParam->nDLCI);
	 break;
    }
    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
	return;
    } // <----------end of switch cmd  type-------
}

/*
 *******************************************************************************
 *  #define EV_CFW_SIM_CHANGE_PWD_RSP        (EV_CFW_SIM_RSP_BASE+8)
 *  #define EV_CFW_SIM_GET_AUTH_STATUS_RSP       (EV_CFW_SIM_RSP_BASE+9)
 *  #define EV_CFW_SIM_ENTER_AUTH_RSP        (EV_CFW_SIM_RSP_BASE+11)
 *  #define EV_CFW_SIM_SET_FACILITY_LOCK_RSP     (EV_CFW_SIM_RSP_BASE+12)
 *  #define EV_CFW_SIM_GET_FACILITY_LOCK_RSP     (EV_CFW_SIM_RSP_BASE+13)
 *******************************************************************************
*/
VOID AT_SIM_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CFWev;
    CFW_EVENT *pcfwEv = &CFWev;
    AT_CosEvent2CfwEvent(pEvent, pcfwEv);
    PAT_CMD_RESULT pResult = NULL;
    UINT8 sStr[10]         = { 0 };
    UINT8 nSim = pcfwEv->nFlag;

    AT_TC(g_sw_AT_SIM, "---- event id = %x,param1 = %x,param3 = %x,nSim = %d", pEvent->nEventId, pEvent->nParam1, pEvent->nParam3, nSim);
    pcfwEv->nUTI = AT_ASYN_GET_DLCI(nSim);
    if (pcfwEv->nParam1 == PINNEEDPUK)
    {
        g_Pin1PukStauts[nSim] = 1;
    }
    else
    {
        g_Pin1PukStauts[nSim] = 0;
    }

    switch (pcfwEv->nEventId)
    {
    case EV_CFW_SIM_SET_ACMMAX_RSP:
        if (AT_IsAsynCmdAvailable("+CAMM", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            if (pcfwEv->nType == 0)
            {
                if (pcfwEv->nParam1 == ERR_SUCCESS)
                    AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pcfwEv->nUTI);
                else
                    AT_SIM_Result_Err(pcfwEv->nParam1 - ERR_CME_PHONE_FAILURE, pcfwEv->nUTI);
            }
            else
            {
                AT_SIM_Result_Err(pcfwEv->nParam1 - ERR_CME_PHONE_FAILURE, pcfwEv->nUTI);
            }
        }

        AT_ZERO_PARAM1(pEvent);
        break;

    case EV_CFW_SIM_GET_ACMMAX_RSP:
        if (AT_IsAsynCmdAvailable("+CAMM", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            if (pcfwEv->nType == 0)
            {
                UINT8* pData = (UINT8*)&pcfwEv->nParam1;
                AT_Sprintf(sStr, "+CAMM:\"%02X%02X%02X\"", pData[0],pData[1],pData[2]);
                AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,sStr,pcfwEv->nUTI);
            }
            else
            {
                AT_SIM_Result_Err(pcfwEv->nParam1 - ERR_CME_PHONE_FAILURE, pcfwEv->nUTI);
            }
        }
        break;

    case EV_CFW_SIM_ENTER_AUTH_RSP:
    {
        if (CPIN == AT_security_currentCMD[nSim])
        {
            AT_security_currentCMD[nSim] = 0;

            if (AT_IsAsynCmdAvailable("+CPIN", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_CPIN_Set_AsyncEventProcess(pcfwEv, pResult);
            }
        }
        else if (CPIN2 == AT_security_currentCMD[nSim])
        {
            AT_security_currentCMD[nSim] = 0;

            if (AT_IsAsynCmdAvailable("+CPIN2", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
            {
                AT_SIM_CmdFunc_CPIN2_Set_AsyncEventProcess(pcfwEv, pResult);
            }
        }

        if (CFWev.nType != 0)
            AT_ZERO_PARAM1(pEvent);
        break;
    }
    /**********************************************
     * AT+cpin? -->:+CPIN: READY,+CPIN: SIM PIN  *
     * AT+cpin2?-->:+CPIN2: SIM PUK2                 *
     * AT^cpinc -->:^SPIC: 2                         *
     * AT^cpinc?-->:^cpinc:SIM PUK(not supported) *
     **********************************************/
    case EV_CFW_SIM_GET_AUTH_STATUS_RSP:
    {
        switch (AT_security_currentCMD[nSim])
        {
            case CPIN:
                {
                    AT_security_currentCMD[nSim] = 0;

                    if (AT_IsAsynCmdAvailable("+CPIN", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
                    {

                        AT_SIM_CmdFunc_CPIN1_Read_AsyncEventProcess(pcfwEv, pResult);
                    }
                }

                break;

            case CPIN2:
                {
                    AT_security_currentCMD[nSim] = 0;
                    if (AT_IsAsynCmdAvailable("+CPIN2", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
                    {
                        AT_SIM_CmdFunc_CPIN2_Read_AsyncEventProcess(pcfwEv, pResult);
                    }
                }
                break;

            case CPINCEXE:
                {
                    AT_security_currentCMD[nSim] = 0;
                    if (AT_IsAsynCmdAvailable("^CPINC", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
                    {
                        AT_SIM_CmdFunc_CPINC_Exe_AsyncEventProcess(pcfwEv, pResult);
                    }
                }
                break;

            default:
                {
                    AT_TC(g_sw_AT_SIM, "no wait RSP ignore EV_CFW_SIM_GET_AUTH_STATUS_RSP");
                    break;
                }
        }

        AT_ZERO_PARAM1(pEvent);
        break;
    }
    case EV_CFW_SIM_CHANGE_PWD_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CPWD", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CPWD_Set_AsyncEventProcess(pcfwEv, pResult);
        }
    }
    break;

    case EV_CFW_SIM_SET_FACILITY_LOCK_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CLCK", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CLCK_Set_AsyncEventProcess(pcfwEv, pResult);
        }
    }
    break;

    case EV_CFW_SIM_GET_FACILITY_LOCK_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CLCK", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CLCK_Read_AsyncEventProcess(pcfwEv, pResult);
        }
    }
    break;
    case EV_CFW_SIM_READ_BINARY_RSP:
    case EV_CFW_SIM_UPDATE_BINARY_RSP:
    case EV_CFW_SIM_READ_RECORD_RSP:
    case EV_CFW_SIM_UPDATE_RECORD_RSP:
    case EV_CFW_SIM_GET_FILE_STATUS_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CRSM", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CRSM_AsyncEventProcess(pcfwEv, pResult);
        }
        else if (AT_IsAsynCmdAvailable("+CRSML", u_gCurrentCmdStamp[nSim], CFWev.nUTI))
        {
            AT_SIM_CmdFunc_CRSML_AsyncEventProcess(pcfwEv, pResult);
        }
        
        if (CFWev.nType != 0)
            AT_ZERO_PARAM1(pEvent);
    }
    break;

    case EV_CFW_SIM_RESET_RSP:
    {
        AT_TC(g_sw_AT_SIM, "EV_CFW_SIM_RESET_RSP asyn event nType=%d,nParam1=%d", pcfwEv->nType, pcfwEv->nParam1);
        if (pcfwEv->nType == 0)
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pcfwEv->nUTI);
        else
            AT_SIM_Result_Err(CMD_RC_SIMDROP, pcfwEv->nUTI);
    }
    break;
    default:
    {
        break;
    }
    }
}

VOID AT_SIM_CmdFunc_CPIN_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;
    UINT8 nSim = pcfwEv->nFlag;
#if SIM_DEBUG
    AT_TC(g_sw_AT_SIM, "CPIN asyn event nType=%d,nParam1=%d", pcfwEv->nType, pcfwEv->nParam1);
#endif
    if (0 == pcfwEv->nType && 0 == pcfwEv->nParam1)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pcfwEv->nUTI);
    }
    else
    {

#if SIM_DEBUG
        AT_TC(g_sw_AT_SIM, "CPIN AsyncEventProcess g_Pin1PukStauts=%d,g_Pin1ErrorTimes=%d", g_Pin1PukStauts[nSim], g_Pin1ErrorTimes[nSim]);
#endif
        // add by wangxd for bug 8046
        if (g_Pin1PukStauts[nSim])
            g_Pin1ErrorTimes[nSim] = 0;
        else
        {
            if ((pcfwEv->nParam1 != ERR_CFW_QUEUE_FULL) || (pcfwEv->nParam1 != ERR_CMS_MEMORY_FAILURE))
                g_Pin1ErrorTimes[nSim]++;
            if (g_Pin1ErrorTimes[nSim] >= 3)
            {
                g_Pin1PukStauts[nSim]  = 1;
                g_Pin1ErrorTimes[nSim] = 0;
            }
        }
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
}

VOID AT_SIM_CmdFunc_CPIN2_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;
#if SIM_DEBUG
    AT_TC(g_sw_AT_SIM, "CPIN asyn event nType=%d,nParam1=%d", pcfwEv->nType, pcfwEv->nParam1);
#endif

    if (0 == pcfwEv->nType && 0 == pcfwEv->nParam1)
    {
        // end ok yaoal changed it //&& 0 == pcfwEv->nParam1//
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pcfwEv->nUTI);
    }
    else
    {
        // end error
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
}

/*************************************************************
* in 27007,this command is not included
* so i do the same as at55 does
* if nType == 0, i will return OK,no matter nParam1'value is
*************************************************************/
VOID AT_SIM_CmdFunc_CPIN2_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;

    if (0 == pcfwEv->nType)
    {
        UINT8* pResp = NULL;
        if (CFW_STY_AUTH_NOSIM == pcfwEv->nParam1)
            pResp = "+CPIN2:NO SIM";
        else if ((LOUINT16(pcfwEv->nParam2) & 0xF000) >> 12 == 0)
            pResp = "+CPIN2:PIN2 BLOCK";
        else if ((LOUINT16(pcfwEv->nParam2) & 0xF00) >> 8 == 0)
            pResp = "+CPIN2:SIM PUK2";
        else if ((LOUINT16(pcfwEv->nParam2) & 0xF00) >> 8 == 3)
            pResp = "+CPIN2: READY";
        else if ((LOUINT16(pcfwEv->nParam2) & 0xF00) >> 8 != 0)
            pResp = "+CPIN2:SIM PIN2";
	else
	{
            AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pcfwEv->nUTI);
	    return;
	}
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pResp,pcfwEv->nUTI);
    }
    else
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
}

/*************************************************************
*   OK or FAIL
*************************************************************/
VOID AT_SIM_CmdFunc_CPWD_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;

    if (0 == pcfwEv->nType)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pcfwEv->nUTI);
    }
    else if (0xF0 == pcfwEv->nType)
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
    }
}

/*************************************************************
*   OK or FAIL
*************************************************************/
VOID AT_SIM_CmdFunc_CLCK_Set_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;
    UINT8 nSim = pcfwEv->nFlag;
    if (0 == pcfwEv->nType)
    {
        AT_FDN_SetStatus(nSim);
        AT_SIM_Result_OK(CMD_FUNC_SUCC,CMD_RC_OK,"", pcfwEv->nUTI);
    }
    else if (0xF0 == pcfwEv->nType)
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        if(ERR_AT_CME_SIM_FAILURE == retErrCode)
        {
            if(CFW_STY_FAC_TYPE_FD == pcfwEv->nParam2)
            {
                AT_SIM_Result_Err(ERR_AT_CME_SIM_PUK2_REQUIRED, pcfwEv->nUTI);
            }
        }
        else
            AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
    }
}

VOID AT_SIM_CmdFunc_CLCK_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;
    if (0 == pcfwEv->nType)
    {
        UINT8 nResp[10] = {0};
        AT_Sprintf(nResp, "+CLCK:%d", LOUINT8(pcfwEv->nParam1));
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, nResp, pcfwEv->nUTI);
    }
    else if (0xF0 == pcfwEv->nType)
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
    else
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
    }
}

/*************************************************************
* in 27007,this command is not included
* so i do the same as at55 does
* if nType == 0, i will return OK,no matter nParam1'value is
*************************************************************/
VOID AT_SIM_CmdFunc_CPINC_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;
    if (0 == pcfwEv->nType)
    {
        // end ok
        // check value if in a valued range.
        switch (pcfwEv->nParam1)
        {
        case CFW_STY_AUTH_READY:
        case CFW_STY_AUTH_PIN1_READY:
        case CFW_STY_AUTH_SIMPIN:
        case CFW_STY_AUTH_SIMPUK:
        case CFW_STY_AUTH_SIMPIN2:
        case CFW_STY_AUTH_SIMPUK2:
        case CFW_STY_AUTH_PHONE_TO_SIMPIN:
        case CFW_STY_AUTH_NETWORKPUK:
        case CFW_STY_AUTH_PIN1_DISABLE:
            break;

        default:
        {
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pcfwEv->nUTI);
            return;
        }
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,cpinasynSTR[pcfwEv->nParam1],pcfwEv->nUTI);
    }
    else
    {
        // modify by frank
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);

        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
}

VOID AT_SIM_CmdFunc_CRSM_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT8 nSim = pcfwEv->nFlag;
    AT_TC(g_sw_AT_SIM, TSTXT("CRSM_Async nType: %d,nEventId:%d\n"),pcfwEv->nType,pcfwEv->nEventId);
    if (pcfwEv->nType == 0)
    {
        //UINT8 nRetString[530] = { 0x00, };
        UINT8 *pRetString = NULL;
        UINT8 *pAsciiRetString =NULL;
        UINT8 *pTmpPara1 = NULL;
        UINT16 nAsciiLen = 0;
        UINT16 nRetStrLen = 0;
        if ((pcfwEv->nEventId == EV_CFW_SIM_UPDATE_RECORD_RSP) || (pcfwEv->nEventId == EV_CFW_SIM_UPDATE_BINARY_RSP))
        {
            nRetStrLen = 15;
            nAsciiLen = 0;
        }
        else if((pcfwEv->nEventId == EV_CFW_SIM_GET_FILE_STATUS_RSP)
                || (pcfwEv->nEventId == EV_CFW_SIM_READ_RECORD_RSP))
        {
            if((UINT8 *)(pcfwEv->nParam1)  != NULL)
            {
                pTmpPara1 = (UINT8 *)(pcfwEv->nParam1);
            }
            else
            {
                AT_TC(g_sw_AT_SIM, TSTXT("CRSM_Async ERROR!\n"));
                AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pcfwEv->nUTI);
                return ;
            }

            if(0x62 == pTmpPara1[0] )
            {
                nAsciiLen = pcfwEv->nParam2*2;
                nRetStrLen = nAsciiLen + 15;
                pAsciiRetString = AT_MALLOC(nAsciiLen + 2);
                if(NULL == pAsciiRetString )
                {
                    AT_SIM_Result_Err(ERR_AT_CME_MEMORY_FAILURE, pcfwEv->nUTI);
                    return ;
                }
                AT_MemZero(pAsciiRetString, nAsciiLen + 2);
                AT_TC_MEMBLOCK(g_sw_AT_SIM, (UINT8 *)(pcfwEv->nParam1), pcfwEv->nParam2, 16);
                SUL_hex2ascii((UINT8 *)(pcfwEv->nParam1),  pcfwEv->nParam2, pAsciiRetString);
                AT_TC_MEMBLOCK(g_sw_AT_SIM, pAsciiRetString, nAsciiLen, 16);
            }
            else
            {
                nAsciiLen = gCrsmCommandDataLen[nSim]*2;
                nRetStrLen = nAsciiLen + 15;
                pAsciiRetString = AT_MALLOC(nAsciiLen + 2);
                if(NULL == pAsciiRetString )
                {
                    AT_SIM_Result_Err(ERR_AT_CME_MEMORY_FAILURE, pcfwEv->nUTI);
                    return ;
                }
                AT_MemZero(pAsciiRetString, nAsciiLen + 2);
                AT_TC_MEMBLOCK(g_sw_AT_SIM, (UINT8 *)(pcfwEv->nParam1), gCrsmCommandDataLen[nSim], 16);
                SUL_hex2ascii((UINT8 *)(pcfwEv->nParam1), gCrsmCommandDataLen[nSim], pAsciiRetString);
                AT_TC_MEMBLOCK(g_sw_AT_SIM, pAsciiRetString,nAsciiLen, 16);
            }
        }
        else
        {
            nAsciiLen = gCrsmCommandDataLen[nSim]*2;
            nRetStrLen = nAsciiLen + 15;
            pAsciiRetString = AT_MALLOC(nAsciiLen + 2);
            if(NULL == pAsciiRetString )
            {
                AT_SIM_Result_Err(ERR_AT_CME_MEMORY_FAILURE, pcfwEv->nUTI);
                return ;
            }
            AT_MemZero(pAsciiRetString, nAsciiLen + 2);
            AT_TC(g_sw_AT_SIM, TSTXT("CRSM_Async nType: else\n"));
            AT_TC_MEMBLOCK(g_sw_AT_SIM, (UINT8 *)(pcfwEv->nParam1), gCrsmCommandDataLen[nSim], 16);
            SUL_hex2ascii((UINT8 *)(pcfwEv->nParam1), gCrsmCommandDataLen[nSim], pAsciiRetString);
            AT_TC_MEMBLOCK(g_sw_AT_SIM, pAsciiRetString,nAsciiLen, 16);
        }
        pRetString = AT_MALLOC(nRetStrLen);
        if(NULL == pRetString )
        {
            if( pAsciiRetString != NULL)
            {
                AT_FREE(pAsciiRetString);

                pAsciiRetString = NULL;
            }

            AT_SIM_Result_Err(ERR_AT_CME_MEMORY_FAILURE, pcfwEv->nUTI);
            return ;
        }
        AT_MemZero(pRetString, nRetStrLen);
        strcpy(pRetString, "+CRSM:144,0,");
        if(nAsciiLen > 0)
        {
            strcat(pRetString, pAsciiRetString);
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pRetString,pcfwEv->nUTI);
        if( pAsciiRetString != NULL)
        {
            AT_FREE(pAsciiRetString);
            pAsciiRetString =  NULL;
        }
        if( pRetString != NULL )
        {
            AT_FREE(pRetString);
            pRetString =  NULL;
        }
    }
    else if (pcfwEv->nType == 0xf0)
    {
        AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pcfwEv->nUTI);
    }
    return;
}

extern CFW_SIMSTATUS g_cfw_sim_status[];
typedef struct _CFW_UsimEfStatus
{
    UINT8   efStructure;   // 1:Transparent ; 2: Linear Fixed ; 6: Cyclic
    INT16   fileId;
    INT16   fileSize;
    UINT8   recordLength;
    UINT8   numberOfRecords;
} CFW_UsimEfStatus;

extern VOID CFW_UsimDecodeEFFcp (UINT8 *RespData, CFW_UsimEfStatus *pSimEfStatus);
VOID AT_SIM_CmdFunc_CRSML_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT8 nSim = pcfwEv->nFlag;
    if(EV_CFW_SIM_GET_FILE_STATUS_RSP == pcfwEv->nEventId)
    {
        if (pcfwEv->nType == 0)
        {
            if(g_cfw_sim_status[nSim].UsimFlag == FALSE)
            {
                gCrsml[nSim].nLen = ((UINT8 *)pcfwEv->nParam1)[14];
            }
            else
            {
                CFW_UsimEfStatus gUsim;
                CFW_UsimDecodeEFFcp((UINT8 *)pcfwEv->nParam1,  &gUsim);
                gCrsml[nSim].nLen = gUsim.recordLength;
            }
            AT_TC(g_sw_AT_SIM, TSTXT("gUsim.recordLength %d\n"), gCrsml[nSim].nLen);

            UINT16 nFileID = CFW_GetStackSimFileID(gCrsml[nSim].nFileID & 0XFFFF, gCrsml[nSim].nFileID >> 16, nSim);
            if(nFileID == 0xFFFF)
            {
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
                return;
            }
//            nFileID = ATGetRealFileID(nFileID,gCrsml[nSim].nCurrentIndex,nSim);
//            UINT8 nLength = ATGetRealCurrIndex(nFileID,gCrsml[nSim].nCurrentIndex,nSim);
            UINT32 nRet = CFW_SimReadRecordWithLen(nFileID, gCrsml[nSim].nCurrentIndex, gCrsml[nSim].nLen, pcfwEv->nUTI, nSim);
            if (ERR_SUCCESS != nRet)
            {
                AT_TC(g_sw_AT_SIM, "nRet = 0x%x", nRet);
                AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
                return;
            }
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pcfwEv->nUTI);
            return;
        }
    }
    else if(EV_CFW_SIM_READ_RECORD_RSP == pcfwEv->nEventId)
    {
        if (pcfwEv->nType == 0)
        {
            UINT8 *pRetString = NULL;
            UINT8 *pAsciiRetString = NULL;
            UINT16 nAsciiLen = 0;
            UINT16 nRetStrLen = 0;

            nAsciiLen = pcfwEv->nParam2*2;
            nRetStrLen = nAsciiLen + 15;
            pRetString = (UINT8 *)AT_MALLOC(nRetStrLen);
            if(NULL == pRetString )
            {
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pcfwEv->nUTI);
                return;
            }
            pAsciiRetString = (UINT8 *)AT_MALLOC(nAsciiLen +2);
            if(NULL == pAsciiRetString )
            {
                AT_FREE(pRetString);
                pRetString = NULL;
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pcfwEv->nUTI);
                return;
            }
            AT_MemZero(pRetString, nRetStrLen);
            AT_MemZero(pAsciiRetString, nAsciiLen +2);
            SUL_hex2ascii((UINT8 *)(pcfwEv->nParam1), pcfwEv->nParam2, pAsciiRetString);
            strcpy(pRetString, "+CRSM:144,0,");
            strcat(pRetString, pAsciiRetString);

            if(gCrsml[nSim].nCurrentIndex < (gCrsml[nSim].nStartIndex + gCrsml[nSim].nCount))
            {
                gCrsml[nSim].nCurrentIndex++;
                AT_SIM_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK,pRetString,pcfwEv->nUTI);
                AT_TC_MEMBLOCK(g_sw_AT_SIM, pRetString, AT_StrLen(pRetString), 16);
                UINT16 nFileID = CFW_GetStackSimFileID(gCrsml[nSim].nFileID & 0XFFFF, gCrsml[nSim].nFileID >> 16, nSim);

                if(nFileID == 0xFFFF)
                {
                    AT_FREE(pRetString);
                    AT_FREE(pAsciiRetString);
                    pAsciiRetString = NULL;
                    pRetString = NULL;
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
                    return;
                }
//                nFileID = ATGetRealFileID(nFileID,gCrsml[nSim].nCurrentIndex,nSim);
//                UINT8 nLength = ATGetRealCurrIndex(nFileID,gCrsml[nSim].nCurrentIndex,nSim);
                UINT32 nRet = CFW_SimReadRecordWithLen(nFileID, gCrsml[nSim].nCurrentIndex, gCrsml[nSim].nLen, pcfwEv->nUTI, nSim);
                if (ERR_SUCCESS != nRet)
                {
                    AT_FREE(pRetString);
                    AT_FREE(pAsciiRetString);
                    pAsciiRetString = NULL;
                    pRetString = NULL;
                    AT_TC(g_sw_AT_SIM, "nRet = 0x%x", nRet);
                    AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
                    return;
                }
                AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"", pcfwEv->nUTI);
            }
            else
            {
                AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pRetString,pcfwEv->nUTI);
            }
            AT_FREE(pRetString);
            AT_FREE(pAsciiRetString);
            pAsciiRetString = NULL;
            pRetString = NULL;
        }
        else if (pcfwEv->nType == 0xf0)
        {
            AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pcfwEv->nUTI);
        }
    }
    return;
}

/***********************************************************************
This function is used to tackle bug8124
When receive async event about CPIN1, SIM modle will call this function
This function is added by wangxd
************************************************************************/
VOID AT_SIM_CmdFunc_CPIN1_Read_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode = 50;
    UINT8 cPin1Param1 = pcfwEv->nParam1;
    if (pcfwEv->nType == 0)
    {
        if (!((0 <= pcfwEv->nParam1) && (22 >= pcfwEv->nParam1)))
        {
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pcfwEv->nUTI);
            return;
        }
        if ((cPin1Param1 == CFW_STY_AUTH_PIN1_DISABLE) || (cPin1Param1 == CFW_STY_AUTH_PIN1_READY))
        {
            cPin1Param1 = 0;
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,cpinasynSTR[cPin1Param1], pcfwEv->nUTI);
        return;
    }

    if((pcfwEv->nType == 0xf0) && (at_ModuleGetInitResult() == AT_MODULE_INIT_NO_SIM))
    {
        cPin1Param1 = 17;
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,cpinasynSTR[cPin1Param1], pcfwEv->nUTI);
        return;
    }

    retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
    AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
}

/*************************************************************************************
This function is used to tackle bug8124 and bug8310
When receive async event about CPINC, SIM event process modle will call this function
This function is added by wangxd
*************************************************************************************/
VOID AT_SIM_CmdFunc_CPINC_Exe_AsyncEventProcess(CFW_EVENT *pcfwEv, PAT_CMD_RESULT pResult)
{
    UINT32 retErrCode    = 50;
#if SIM_DEBUG
    AT_TC(g_sw_AT_SIM, "pcfwEv->nType == %d, pcfwEv->nParam1 == 0x%x", pcfwEv->nType, pcfwEv->nParam1);
    AT_TC(g_sw_AT_SIM, "gLstStatusRec == %d", gLstStatusRec);
    AT_TC(g_sw_AT_SIM, "CPINC_Event pcfwEv->nParam1 = %d", pcfwEv->nParam1);
#endif

    if (pcfwEv->nType == 0)
    {
        UINT8 nResp[20] = {0};
        AT_Sprintf(nResp, "^CPINC:%d,%d,%d,%d", LOUINT16(pcfwEv->nParam2) & 0x0F, (LOUINT16(pcfwEv->nParam2) & 0xF0) >> 4,
                   (LOUINT16(pcfwEv->nParam2) & 0xF00) >> 8, (LOUINT16(pcfwEv->nParam2) & 0xF000) >> 12);

        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nResp, pcfwEv->nUTI);

    }
    else
    {
        retErrCode = AT_SetCmeErrorCode(pcfwEv->nParam1, 1);
        AT_SIM_Result_Err(retErrCode, pcfwEv->nUTI);
    }
}

VOID AT_SIM_CmdFunc_SETSIM(AT_CMD_PARA *pParam)
{
    UINT8 paraCount = 0;
    INT32 iRet      = 0;
    UINT16 nTypSize = 0;
    UINT8 nIndex   = 0;
    UINT8 nSim     = 0;

    AT_TC(g_sw_GPRS, "AT+SIM:CmdFunc_SETSIM ");
    if (pParam == NULL)
    {
        AT_TC(g_sw_GPRS, "AT+SIM:pParam == NULL !");
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            AT_TC(g_sw_GPRS, "AT+SIM:paraCount: %d", paraCount);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        nTypSize = sizeof(nSim);
        iRet     = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nSim, &nTypSize);
        if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        if (nSim < 5)
        {
            gAtSimID = nSim;
            AT_TC(g_sw_GPRS, "From SIM:%d", nSim);
        }
        else
        {
            AT_TC(g_sw_GPRS, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, "", pParam->nDLCI);
        break;

    case AT_CMD_READ:
        break;

    case AT_CMD_TEST:
        break;

    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, pParam->nDLCI);
        break;
    }
    return;
}

/*
 *******************************************************************************
 *  possible input from ATM:
 *  AT^creset=? -->test,    return OK;
 *  AT^creset   --> exe -->reponse:  "OK" && "+CME ERROR:11"
 *******************************************************************************
*/
void AT_SIM_CmdFunc_CRESET(AT_CMD_PARA *pParam)
{
    UINT32 return_val;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, 0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        return_val = CFW_SimReset(pParam->nDLCI, nSim);
        if (ERR_SUCCESS != return_val)
        {
            if (ERR_NO_MORE_MEMORY == return_val)
                AT_SIM_Result_Err(ERR_AT_CME_NO_MEMORY, pParam->nDLCI);
            else
                AT_SIM_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, pParam->nDLCI);
            return;
        }

        AT_SIM_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        break;

    case AT_CMD_TEST:
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"^CRESET: ",pParam->nDLCI);
    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    }
}

/*
 ========================================================================================================
 *  possible input from ATM:
 *  AT+CSST=? -->test,  return OK;
 *  AT+CSST=n   --> set -->reponse:  "service xxx allocated ( not allocated ) and activated(not activated)"
 ========================================================================================================
*/
const UINT8 gSSTServicesName[][50] =
{
    "CHV1disablefunction",
    "AbbreviatedDiallingNumbers(ADN)",
    "FixedDiallingNumbers(FDN)",
    "ShortMessageStorage(SMS)",
    "AdviceofCharge(AoC)",
    "CapabilityConfigurationParameters(CCP)",
    "PLMNselector",
    "RFU",
    "MSISDN",
    "Extension1",
    "Extension2",
    "SMSParameters",
    "LastNumberDialled(LND)",
    "CellBroadcastMessageIdentifier",
    "GroupIdentifierLevel1",
    "GroupIdentifierLevel2",
    "ServiceProviderName",
    "ServiceDiallingNumbers(SDN)",
    "Extension3",
    "RFU",
    "VGCSGroupIdentifierList(EFVGCSandEFVGCSS)",
    "VBSGroupIdentifierList(EFVBSandEFVBSS)",
    "enhancedMultiLevelPrecedenceandPreemptionService",
    "AutomaticAnswerforeMLPP",
    "DatadownloadviaSMSCB",
    "DatadownloadviaSMSPP",
    "Menuselection",
    "Callcontrol",
    "ProactiveSIM",
    "CellBroadcastMessageIdentifierRanges",
    "BarredDiallingNumbers(BDN)",
    "Extension4",
    "DepersonalizationControlKeys",
    "CooperativeNetworkList",
    "ShortMessageStatusReports",
    "Network'sindicationofalertingintheMS",
    "MobileOriginatedShortMessagecontrolbySIM",
    "GPRS",
    "Image(IMG)",
    "SoLSA(SupportofLocalServiceArea)",
    "USSDstringdataobjectsupportedinCallControl",
    "RUNATCOMMANDcommand",
    "UsercontrolledPLMNSelectorwithAccessTechnology",
    "OperatorcontrolledPLMNSelectorwithAccessTechnology",
    "HPLMNSelectorwithAccessTechnology",
    "CPBCCHInformation",
    "InvestigationScan",
    "ExtendedCapabilityConfigurationParameters",
    "MExE",
    "RPLMNlastusedAccessTechnology",
};

void AT_SIM_CmdFunc_CSST(AT_CMD_PARA *pParam)
{
    UINT32 return_val;

    if (pParam == NULL)
    {
        AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID,0);
        return;
    }
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    u_gCurrentCmdStamp[nSim] = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        UINT8 paraCount = 0;
        UINT8 nService = 0;
        UINT16 nSize = 0x01;

        INT32 iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            AT_TC(g_sw_GPRS, "AT+CSST:paraCount: %d", paraCount);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        iRet     = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nService, &nSize);
        if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+CSST:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        return_val = CFW_GetSSTStatus(nService, nSim);
        UINT8 nOutString[300] = {0x00,};
        AT_Sprintf(nOutString, "Services %s ", gSSTServicesName[nService - 1]);

        if(return_val & 0x01)
            SUL_StrCat(nOutString, " < allocated > ");
        else
            SUL_StrCat(nOutString, " <  not allocated > ");

        if(return_val & 0x02)
            SUL_StrCat(nOutString, " <  activated > ");
        else
            SUL_StrCat(nOutString, " <  not activated > ");
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nOutString,pParam->nDLCI);
    }
    break;
    case AT_CMD_TEST:
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"+CSST: ",pParam->nDLCI);
    default:
        AT_SIM_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
    }
}

VOID AT_SIM_CmdFunc_SIM(AT_CMD_PARA *pParam)
{
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"", pParam->nDLCI);
        return;
    }

    else if (pParam->iType == AT_CMD_EXE)
    {

        if(CFW_GetSimStatus(nSim) == 0x01)
        {
            UINT8 *pRsp = AT_MALLOC(20);

            memset(pRsp,0x00,20);

            AT_Sprintf(pRsp, "+SIM");
            AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,  pRsp, pParam->nDLCI);

            AT_FREE(pRsp);
        }
        else
        {
            AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pParam->nDLCI);
        }
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        UINT8 paraCount = 0;
        UINT8 nIndex = 0;
        UINT8 nSimID = 0;
        INT32 iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            AT_TC(g_sw_GPRS, "AT+SIM:paraCount: %d", paraCount);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        UINT16 nTypSize = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, nIndex, AT_UTIL_PARA_TYPE_UINT8, &nSimID, &nTypSize);
        if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        if (nSimID < CFW_SIM_COUNT)
        {
            AT_TC(g_sw_GPRS, "From SIM:%d", nSim);
            if(CFW_GetSimStatus(nSimID) == 0x01)
            {
                UINT8 *pRsp = AT_MALLOC(20);

                memset(pRsp,0x00,20);

                AT_Sprintf(pRsp, "+SIM %d",nSimID);
                AT_SIM_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,  pRsp, pParam->nDLCI);

                AT_FREE(pRsp);
            }
            else
            {
                AT_SIM_Result_Err(ERR_AT_CME_SIM_FAILURE, pParam->nDLCI);
            }
        }
        else
        {
            AT_TC(g_sw_GPRS, "AT+SIM:iRet : %d !", iRet);
            AT_SIM_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }
        return;
    }
}





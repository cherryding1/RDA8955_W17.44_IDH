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
#include "at_module.h"
#include "at_cmd_sat.h"
#include "at_cmd_sim.h"
#include "at_cfg.h"
#include "cfw.h"  // wangcf [+]2008-5-11

#define CFW_SAT_IND_PDU 0
#define CFW_SAT_IND_TEXT 1
#define CFW_SAT_IND_UCS2 2

BOOL gATSATComQualifier[NUMBER_OF_SIM]  = {0,};
BOOL gATSATSendSMSFlag[NUMBER_OF_SIM]   = {FALSE, FALSE};
BOOL gATSATLocalInfoFlag[NUMBER_OF_SIM] = {FALSE, FALSE};
BOOL gATSATSendUSSDFlag[NUMBER_OF_SIM]  = {FALSE, FALSE};
BOOL gATSATSetupCallFlag[NUMBER_OF_SIM] = {FALSE, FALSE};
BOOL gATSATSetupCallProcessingFlag[NUMBER_OF_SIM] = {FALSE, FALSE};

UINT8 AllowedInstance = INSTANCE;
static UINT8 g_ActiveSatFlag = 0;



#ifdef AT_STSF
//Start For STSF command
UINT8 STK_Mode = 0;
UINT8 STK_ProfileBuffer[12] = {0};
UINT8 STK_TimeOut = 1;
UINT8 STK_AutoResp = 0;
//End For STSF command
#endif

UINT8 AT_SS_GetUSSDNum();
UINT8 AT_SS_GetUSSDState();
UINT8 CFW_GetCurrentCmdNum(CFW_SIM_ID nSimID);
UINT32 sms_tool_CheckDcs(UINT8 *pDcs);
VOID _ResetSimTimer(UINT8 nTimeInterval, UINT8 nTimeUnit, CFW_SIM_ID nSimID);
VOID CFW_SetSATIndFormate(BOOL bMode);
UINT32 ML_Unicode2LocalLanguageBigEnding(const UINT8 *in, UINT32 in_len,
        UINT8 **out, UINT32 *out_len, UINT8 nCharset[12]);

UINT32 CFW_SimInit( BOOL bRstSim, CFW_SIM_ID nSimID);
UINT8 SUL_hex2ascii(UINT8* pInput, UINT16 nInputLen, UINT8* pOutput);
BOOL charset_gsm2ascii(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);

VOID AT_SAT_Result_OK(UINT32 uReturnValue,
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

VOID AT_SAT_Result_Err(UINT32 uErrorCode, UINT8 nDLCI)
{
    PAT_CMD_RESULT pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                                         uErrorCode, CMD_ERROR_CODE_TYPE_CME, 0, 0, 0, nDLCI);
    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, nDLCI);
        AT_FREE(pResult);
    }
}

UINT32 AT_SAT_SpecialCMDSendSMS(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{
    UINT8 nAlphaLen  = 0;
    UINT8 nAddrLen   = 0;
    UINT8 nAddrType  = 0;
    UINT8 nTPDULen   = 0;

    UINT8 *nAlphaStr = NULL;
    UINT8 *nAddrStr  = NULL;
    UINT8 *nTPDUStr  = NULL;

    CFW_SAT_SMS_RSP *pSmsResp = NULL;
    UINT16 nMemsize = SIZEOF(CFW_SAT_SMS_RSP);
    UINT16 nLength = 0x00;
    UINT32 nIndex = 0x00;
    UINT8 *pOffset = pFetchData;

    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS gATSATSendSMSFlag %d\n", gATSATSendSMSFlag[nSimID]);

    gATSATSendSMSFlag[nSimID] = TRUE;
    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    UINT8 nCmdQual      = pOffset[nIndex + 5];
    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;

        nLength   = pOffset[nIndex++];
        nAlphaLen = nLength;
        nAlphaStr = pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex    = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x06) || (pOffset[nIndex] == 0x86))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength   = pOffset[nIndex++] - 1;
        nAddrLen  = nLength;
        nAddrType = pOffset[nIndex++];
        nAddrStr  = pOffset + nIndex;
    }
    else
        nLength = 0;
    nIndex    = nIndex + nLength;
    nMemsize += nLength;

    if ((pOffset[nIndex] == 0x0B) || (pOffset[nIndex] == 0x8B))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength  = pOffset[nIndex++];
        nTPDULen = nLength;
        nTPDUStr = (UINT8 *)pOffset + nIndex;
    }
    else
    {
        AT_TC(g_sw_AT_SAT, "Error Invalid data Not 0x0B or 0x8B But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;
    }
    nMemsize += nLength;

    pSmsResp = (CFW_SAT_SMS_RSP *)AT_MALLOC(nMemsize);
    if (pSmsResp == NULL)
    {
        AT_TC(g_sw_AT_SAT, "Error Malloc failed\n");
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory8(pSmsResp, nMemsize);

    pSmsResp->nAlphaLen = nAlphaLen;
    pSmsResp->pAlphaStr = (UINT8 *)pSmsResp + SIZEOF(CFW_SAT_SMS_RSP);
    SUL_MemCopy8(pSmsResp->pAlphaStr, nAlphaStr, nAlphaLen);

    pSmsResp->nAddrLen  = nAddrLen;
    pSmsResp->nAddrType = nAddrType;
    pSmsResp->pAddrStr = pSmsResp->pAlphaStr + nAlphaLen;
    if (nAddrLen != 0)
    {
        SUL_MemCopy8(pSmsResp->pAddrStr, nAddrStr, nAddrLen);
    }

    pSmsResp->nPDULen = nTPDULen;
    pSmsResp->pPDUStr = pSmsResp->pAddrStr + nAddrLen;

    SUL_MemCopy8(pSmsResp->pPDUStr, nTPDUStr, nTPDULen);
    // check the DCS and tpdu, if dcs is 7 bits and all the tpdu bit7 is 0, it means the DCS is not equal to the data,
    // we need to transfer the 8 bit data to 7 bit.

    UINT8 uDCSOffsize   = 0;
    UINT8 uVPLen        = 0;
    UINT8 uVPF          = 0;
    UINT8 UdataOff      = 0;

    UINT8 buff7[140] = { 0, };
    UINT8 buff7len   = 0;

    // MTI+MR+DALEN/2+DATYPE+PID + DCS
    uDCSOffsize = 2 + (pSmsResp->pPDUStr[2] + 1) / 2 + 1 + 2;

    // bit3,4 of MTI is vpf
    uVPF = (pSmsResp->pPDUStr[0] >> 3) & 3;
    if (!uVPF)  // NOT present
        uVPLen = 0;
    else if (uVPF == 2) // relative format
        uVPLen = 1;
    else
        uVPLen = 7;

    AT_TC(g_sw_AT_SAT, "uDCSOffsize=%d,pPDUPacketTmp[uDCSOffsize]=0x%02x, uVPLen=%d pSmsResp->pPDUStr[uDCSOffsize-1] %d\n",
          uDCSOffsize, pSmsResp->pPDUStr[uDCSOffsize], uVPLen, pSmsResp->pPDUStr[uDCSOffsize - 1]);
    // dcs offsize + udl+1-->userdata
    // cause VPF is 0, it means vp field is not present, so when test, we don't count this, othersize VP will take 1~7 octs.
    UdataOff = uDCSOffsize + 2 + uVPLen;

    //SEND SHORT MESSAGE;
    //bit 1:                  0 = packing not required
    //                          1 = SMS packing by the ME required
    if( 0x00 == nCmdQual )
    {
        //packing not required
    }
    else if (!pSmsResp->pPDUStr[uDCSOffsize])   // check if match the condition 1, DCS is 0
    {
        UINT8 uLoop = 0;

        // here need to check whether all the bit7 of userdata is 0,
        for (uLoop = 0; uLoop < pSmsResp->pPDUStr[UdataOff - 1]; uLoop++)
        {
            if ((pSmsResp->pPDUStr[UdataOff + uLoop] >> 7) & 1)
                break;
        }

        AT_TC(g_sw_AT_SAT, "uLoop=%d, pSmsResp->pPDUStr[UdataOff-1]=%d\n", uLoop,
              pSmsResp->pPDUStr[UdataOff - 1]);

        // check if match the condition 2: all the bit 7 of user data is 0.
        if (uLoop && (uLoop == pSmsResp->pPDUStr[UdataOff - 1]))
        {
            buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);

            SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);

            AT_TC(g_sw_AT_SAT, "UdataOff=%d, buff7len= %d\n", UdataOff, buff7len);

            AT_TC(g_sw_AT_SAT, "pSmsResp->nPDULen before = %d\n", pSmsResp->nPDULen);

            pSmsResp->nPDULen +=  (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);

            AT_TC(g_sw_AT_SAT, "pSmsResp->nPDULen end = %d\n", pSmsResp->nPDULen);

        }
    }
    else if(0xF2 == pSmsResp->pPDUStr[uDCSOffsize])
    {
        // change DCS to 0, 7  bit decode
        pSmsResp->pPDUStr[uDCSOffsize] = 0;
        UINT8 uLoop = 0;
        AT_TC(g_sw_AT_SAT, "New pSmsResp->pPDUStr[UdataOff - 1]= %d\n", pSmsResp->pPDUStr[UdataOff - 1]);
        // here need to check whether all the bit7 of userdata is 0,
        for (uLoop = 0; uLoop < pSmsResp->pPDUStr[UdataOff - 1]; uLoop++)
        {
            if ((pSmsResp->pPDUStr[UdataOff + uLoop] >> 7) & 1)
                break;
        }
        if (uLoop && (uLoop == pSmsResp->pPDUStr[UdataOff - 1]))
        {
            // encode user data to 7 bit data
            buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);
            SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);

            AT_TC(g_sw_AT_SAT, "UdataOff=%d, buff7len= %d\n", UdataOff, buff7len);
            AT_TC(g_sw_AT_SAT, "pSmsResp->nPDULen before = %d\n", pSmsResp->nPDULen);

            pSmsResp->nPDULen +=  (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);

            AT_TC(g_sw_AT_SAT, "pSmsResp->nPDULen end = %d\n", pSmsResp->nPDULen);
        }
    }
    else if((4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0x0F)) && (0x00 == pSmsResp->pPDUStr[uDCSOffsize - 1]))
    {
        AT_TC(g_sw_AT_SAT, "((4 == (pSmsResp->pPDUStr[uDCSOffsize] & 0x0F)) && (0x00 == pSmsResp->pPDUStr[uDCSOffsize-1]))\n");
        // change DCS to 0, 7  bit decode
        pSmsResp->pPDUStr[uDCSOffsize] = 0;
        // encode user data to 7 bit data
        buff7len = SUL_Encode7Bit(&pSmsResp->pPDUStr[UdataOff], buff7, pSmsResp->pPDUStr[UdataOff - 1]);
        SUL_MemCopy8(&pSmsResp->pPDUStr[UdataOff], buff7, buff7len);

        AT_TC(g_sw_AT_SAT, "UdataOff=%d, buff7len= %d\n", UdataOff, buff7len);
        AT_TC(g_sw_AT_SAT, "pSmsResp->nPDULen before = %d\n", pSmsResp->nPDULen);

        pSmsResp->nPDULen +=  (buff7len - pSmsResp->pPDUStr[UdataOff - 1]);
        AT_TC(g_sw_AT_SAT, "pSmsResp->nPDULen end = %d\n", pSmsResp->nPDULen);

    }

    UINT32 result           = 0;
    UINT8 *pPDUPacket       = NULL;
    UINT8 *pPDUPacketTmp    = NULL;
    UINT16 nPDUPacket       = 0;
    UINT16 nPDUPacketTmp    = 0;
    UINT8 pAdd[12] = {0x00,};

    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS nAddrLen = %d", pSmsResp->nAddrLen);
    if (pSmsResp->nAddrLen == 0)   //如果没有传地址上来，就取得短信息中心号码
    {
        CFW_SMS_PARAMETER smsInfo;
        memset(&smsInfo, 0, sizeof(CFW_SMS_PARAMETER));
        CFW_CfgGetSmsParam(&smsInfo, 0, nSimID);

        pSmsResp->nAddrLen = smsInfo.nNumber[0];
        memcpy(pAdd, &(smsInfo.nNumber[1]), smsInfo.nNumber[0]);
        nPDUPacketTmp = pSmsResp->nPDULen + pSmsResp->nAddrLen + 1;

        pPDUPacket = (UINT8 *)AT_MALLOC(nPDUPacketTmp);
        if(pPDUPacket == NULL)
        {
            AT_FREE(pSmsResp);
            pSmsResp = NULL;
            return ERR_NO_MORE_MEMORY;
        }

        memset(pPDUPacket, 0xFF, nPDUPacketTmp);
        pPDUPacketTmp = pPDUPacket;
        *(pPDUPacketTmp++) = pSmsResp->nAddrLen;
    }
    else //如果传上来地址，在前面加91 代表加号
    {
        nPDUPacketTmp = pSmsResp->nPDULen + pSmsResp->nAddrLen + 2; //tpdu length ，加2是因为增加了2位: 地址长度和 91
        pPDUPacket = (UINT8 *)AT_MALLOC(nPDUPacketTmp);
        if(pPDUPacket == NULL)
        {
            AT_FREE(pSmsResp);
            pSmsResp = NULL;
            return ERR_NO_MORE_MEMORY;
        }
        memset(pPDUPacket, 0xFF, nPDUPacketTmp);
        memcpy(pAdd, pSmsResp->pAddrStr, pSmsResp->nAddrLen);
        pPDUPacketTmp = pPDUPacket;
        *(pPDUPacketTmp++) = pSmsResp->nAddrLen + 1;   //地址的长度，加一是因为多了一个91
        AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS pSmsResp->nAddrType = %d", pSmsResp->nAddrType);
        *(pPDUPacketTmp++) = pSmsResp->nAddrType ;//0x91;   // "+"
    }

    nPDUPacket  = nPDUPacketTmp;
    if (pSmsResp->nAddrLen != 0)
    {
        memcpy(pPDUPacketTmp, pAdd, pSmsResp->nAddrLen);
        pPDUPacketTmp += pSmsResp->nAddrLen;
    }
    if (pSmsResp->nPDULen != 0)
    {
        memcpy(pPDUPacketTmp, pSmsResp->pPDUStr, pSmsResp->nPDULen);
    }

    UINT8 nFormat = 0x00;
    UINT8 Uti  = 0;

    CFW_CfgGetSmsFormat(&nFormat, nSimID);
    CFW_CfgSetSmsFormat(0, nSimID);
    AT_GetFreeUTI(CFW_SS_SRV_ID, &Uti);
    result = CFW_SmsSendMessage(NULL, pPDUPacket, nPDUPacket, Uti, nSimID);
    CFW_CfgSetSmsFormat(nFormat, nSimID);

    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS nAddrLen = %d", pSmsResp->nAddrLen);
    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS nPDULen = %d", pSmsResp->nPDULen);
    if (result == ERR_SUCCESS)
    {
        AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS Success");
    }
    else
    {
        AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDSendSMS CFW_SmsSendMessage error!!!! = %x", result);
    }

    AT_FREE(pPDUPacket);
    pPDUPacket = NULL;
    if(pSmsResp  != NULL )
    {
        AT_FREE(pSmsResp);
        pSmsResp = NULL;
    }
    return ERR_SUCCESS;
}

UINT32 gSATCurrentCmdStamp[CFW_SIM_COUNT] = {0x00,};

UINT32 AT_SAT_SpecialCMDProvideLocalInfo(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{

    UINT8 *pOffset  = pFetchData;
    UINT8 nIndex    = 0x00;
    UINT8 nUTI      = 0x00;
    UINT32 nRet = ERR_SUCCESS;
    nUTI = AT_ASYN_GET_DLCI(nSimID);

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    UINT8 nComQualifier = pOffset[nIndex + 5];
    //UINT8 nCmdNum         = pOffset[nIndex + 3];
    gATSATComQualifier[nSimID] = nComQualifier;
    nIndex = nIndex + 10;
    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDProvideLocalInfo nComQualifier %d\n", nComQualifier);

    /*
    -   PROVIDE LOCAL INFORMATION
    '00' = Location Information (MCC, MNC, LAC and Cell Identity)
    '01' = IMEI of the ME
    '02' = Network Measurement results
    '03' = Date, time and time zone
    '04' = Language setting
    '05' = Timing Advance
    '06' to 'FF' = Reserved
    */
    if(0x00 == nComQualifier)
    {
        CFW_TSM_FUNCTION_SELECT SelecFUN;
        SelecFUN.nNeighborCell = FALSE;
        SelecFUN.nServingCell = TRUE;
        gATSATLocalInfoFlag[nSimID] = TRUE;
        nRet = CFW_EmodOutfieldTestStart(&SelecFUN, nUTI, nSimID);
        if(nRet == ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SAT,  " Func: %s PROVIDE LOCAL INFO ERR_SUCCESS", __FUNCTION__);
        }
        else
        {
            AT_TC(g_sw_AT_SAT,  " Func: %s PROVIDE LOCAL INFO ERROR!!! 0x%x", __FUNCTION__, nRet);
        }
        return ERR_CONTINUE;
    }
    else if(0x01 == nComQualifier)
    {
        UINT8 pImei[16] = {0x00,};
        UINT8 nImeiLen = 0x00;
        UINT8 pResponseData[26] = {0x81, 0x03, 0x01, 0x26, 0x01, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00, 0x94, 0x08,/*0x10,0x72,0x84,0x00,0x53,0x56,0x68,0xF8*/};
        gATSATLocalInfoFlag[nSimID] = TRUE;
        pResponseData[2] = CFW_GetCurrentCmdNum(nSimID);
        CFW_EmodGetIMEIBCD(pImei, &nImeiLen, nSimID);

        if(nImeiLen > 8)
        {
            nImeiLen = 8;
        }
        SUL_MemCopy8(&pResponseData[14], pImei, nImeiLen);

        nRet = CFW_SatResponse (0xFF, 0x00, 0x0, pResponseData, 22, nUTI, nSimID);
        AT_TC(g_sw_AT_NW, "CFW_SatResponse nRet:0x%x!\n", nRet);
        return ERR_CONTINUE;
    }
    else if((0x02 == nComQualifier) || (0x03 == nComQualifier))
    {

    }
    else if(0x05 == nComQualifier)
    {
        CFW_TSM_FUNCTION_SELECT SelecFUN;
        SelecFUN.nNeighborCell = FALSE;
        SelecFUN.nServingCell = TRUE;

        gATSATLocalInfoFlag[nSimID] = TRUE;

        nRet = CFW_EmodOutfieldTestStart(&SelecFUN, nUTI, nSimID);
        if(nRet == ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SAT,  " Func: %s PROVIDE LOCAL INFO ERR_SUCCESS", __FUNCTION__);
        }
        else
        {
            AT_TC(g_sw_AT_SAT,  " Func: %s PROVIDE LOCAL INFO ERROR!!! 0x%x", __FUNCTION__, nRet);
        }
        return ERR_CONTINUE;
    }
    else
    {
        return ERR_NOT_SUPPORT;
    }
    return ERR_SUCCESS;
}
extern BOOL gSatFreshComm[];

UINT32 AT_SAT_SpecialCMDRefresh(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{

    AT_TC(g_sw_AT_SAT, "%s\n", __func__);

    UINT8 nIndex    = 0x00;
    UINT8 nLength   =  0x00;
    UINT8 *pOffset  = pFetchData;
    UINT8 nNumOfFiles = 0x00;
    UINT8 *pFiles   = NULL;

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    UINT8 nComQualifier = pOffset[nIndex + 5];
    nIndex = nIndex + 10;
    /*
    '00' =SIM Initialization and Full File Change Notification;
    '01' = File Change Notification;
    '02' = SIM Initialization and File Change Notification;
    '03' = SIM Initialization;
    '04' = SIM Reset;
    '05' to 'FF' = reserved values.
    */
    AT_TC(g_sw_AT_SAT, "nComQualifier %d\n", nComQualifier);
    if( 0x00 == nComQualifier )
    {
        AT_TC(g_sw_AT_SAT, "nComQualifier 0x00 re attachment NW\n");
        //TODO
        //目前只让手机重新驻网
        //_SAT_SendSimClose( nSimID );
        //_SAT_SendSimOpen( nSimID);

        UINT8 nUTI = 0x00;
        CFW_GetFreeUTI(0, &nUTI);

        UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
        gSatFreshComm[nSimID] = 0x01;
        AT_TC(g_sw_AT_SAT, "CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
    }
    else if( 0x01 == nComQualifier )
    {
        if ((pOffset[nIndex] == 0x12) || (pOffset[nIndex] == 0x92))
        {
            if (pOffset[++nIndex] == 0x81)
                nIndex++;
            nLength     = pOffset[nIndex++];
            nNumOfFiles   = pOffset[nIndex++];

            pFiles = (UINT8 *)CSW_SIM_MALLOC(nLength);
            SUL_MemCopy8(pFiles, &pOffset[nIndex], nLength);
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID );
            //Add by Lixp at 20130721
            //目前只让手机重新驻网
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID);
            UINT8 nUTI = 0x00;
            CFW_GetFreeUTI(0, &nUTI);

            UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
            gSatFreshComm[nSimID] = 0x11;
            AT_TC(g_sw_AT_SAT, "CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
            return ERR_SUCCESS;
        }
        else
        {
            return ERR_SUCCESS;
        }
    }
    else if( 0x02 == nComQualifier )
    {
        if ((pOffset[nIndex] == 0x12) || (pOffset[nIndex] == 0x92))
        {
            if (pOffset[++nIndex] == 0x81)
                nIndex++;

            nLength     = pOffset[nIndex++];
            nNumOfFiles   = pOffset[nIndex++];

            pFiles = (UINT8 *)CSW_SIM_MALLOC(nLength);
            SUL_MemCopy8(pFiles, &pOffset[nIndex], nLength);
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID );
            //Add by Lixp at 20130721
            //目前只让手机重新驻网
            //_SAT_SendSimClose( nSimID );
            //_SAT_SendSimOpen( nSimID);

            UINT8 nUTI = 0x00;
            CFW_GetFreeUTI(0, &nUTI);
            UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM, 1, nUTI, nSimID);
            gSatFreshComm[nSimID] = 0x21;
            AT_TC(g_sw_AT_SAT, "CFW_SetComm  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
            return ERR_SUCCESS;
        }
    }
    else if( 0x03 == nComQualifier )
    {
        //Add by Lixp at 20130721
        //目前只让手机重新驻网
        //_SAT_SendSimClose( nSimID );
        //_SAT_SendSimOpen( nSimID);
        UINT8 nUTI = 0x00;
        CFW_GetFreeUTI(0, &nUTI);
        //UINT32 nRet = CFW_SetComm(CFW_DISABLE_COMM,1,nUTI,nSimID);
        UINT32 nRet = CFW_SimReset(nUTI, nSimID);
        gSatFreshComm[nSimID] = 0x31;
        AT_TC(g_sw_AT_SAT, "CFW_SimReset  nRet[0x%x] gSatFreshComm[%d] %d\n", nRet, nSimID, gSatFreshComm[nSimID]);
        return ERR_SUCCESS;
    }
    else if( 0x04 == nComQualifier )
    {
        AT_TC(g_sw_AT_SAT, "CFW_SimInit[%d] %d\n", nSimID);
        gSatFreshComm[nSimID] = 0x41;
        CFW_SimInit( 0, nSimID );
        return ERR_SUCCESS;
    }
    CFW_SatResponse(0x01, 0x00, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSimID), nSimID);
    return ERR_SUCCESS;
}

UINT32 AT_SAT_SpecialCMDDisplayTxt(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{
    UINT8 nCodeSch  = 0;
    UINT8 *pTextStr = NULL;
    UINT32 nIndex = 0x00;
    UINT8 *pOffset = pFetchData;
    UINT16 nLength = 0x00;

    //pOffset += 2;
    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDDisplayTxt\n");

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x0D) || (pOffset[nIndex] == 0x8D))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength  = pOffset[nIndex++] - 1;
        nCodeSch = pOffset[nIndex++];
        pTextStr = pOffset + nIndex;
    }
    else
    {
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "Error Invalid data Not 0x0D or 0x8D But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;;
    }
    UINT8 nCSWDCS = nCodeSch;
    sms_tool_CheckDcs(&nCSWDCS);
    CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "AT_SAT_SpecialCMDDisplayTxt nLength %d nCodeSch %d nCSWDCS %d\n", nLength, nCodeSch, nCSWDCS);
    if(0x00 == nCSWDCS)
    {
        UINT8 *p = (UINT8 *)AT_MALLOC(2 * nLength);
        SUL_ZeroMemory8(p, 2 * nLength);

        SUL_Decode7Bit(pTextStr, p, nLength);
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "AT_SAT_SpecialCMDDisplayTxt p %s\n", p);
        AT_FREE(p);
    }
    else
    {
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "AT_SAT_SpecialCMDDisplayTxt pTextStr %s\n", pTextStr);
    }
    return ERR_SUCCESS;
}

UINT32 AT_SAT_SpecialCMDPollInterval(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{

    UINT32 nIndex       = 0x00;
    UINT8 *pOffset      = pFetchData;
    UINT8 nLen          = 0x00;
    UINT8 nTimeUnit     = 0x00;
    UINT8 nTimeInterval = 0x00;

    AT_TC(g_sw_AT_SAT, "%s\n", __func__);

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x04) || (pOffset[nIndex] == 0x84))
    {
        nLen          = pOffset[nIndex + 1];
        nTimeUnit     = pOffset[nIndex + 2];
        nTimeInterval = pOffset[nIndex + 3];
    }
    else
    {
        AT_TC(g_sw_AT_SAT, "Invald tag pOffset[nIndex] 0x%x", pOffset[nIndex]);
        return ERR_SUCCESS;
    }
    AT_TC(g_sw_AT_SAT, "Success nTimeInterval %d nTimeUnit %d", nTimeInterval, nTimeUnit);
    _ResetSimTimer(nTimeInterval, nTimeUnit, nSimID);
    return ERR_SUCCESS;
}

extern UINT32 gSATTimerValue[ ][CFW_SIM_COUNT];
extern UINT8 _GetTimerIdentifier(UINT8 *p);
extern UINT32 _GetTimerValue(UINT8 *p);
extern u32 sxs_ExistTimer (u32 Id);

UINT32 AT_SAT_SpecialCMDTimerManagement(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{
    UINT32 nIndex       = 0x00;
    UINT8 *pOffset      = pFetchData;
    UINT8 nTimerID      = 0x00;
    UINT8 nHour         = 0x00;
    UINT8 nMinute       = 0x00;
    UINT8 nSecond       = 0x00;
    UINT32 nTimerValue  = 0x00;
    UINT32 nRet = ERR_SUCCESS;

    AT_TC(g_sw_AT_SAT, "%s\n", __func__);

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    UINT8 nComQualifier = pOffset[nIndex + 5];
    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x24) || (pOffset[nIndex] == 0xA4))
    {
        nTimerID = _GetTimerIdentifier(&pOffset[nIndex]) ;
        nIndex += 3;
    }
    if ((pOffset[nIndex] == 0x25) || (pOffset[nIndex] == 0xA5))
    {
        nTimerValue = _GetTimerValue(&pOffset[nIndex]);
        nHour = nTimerValue >> 16;
        nMinute = nTimerValue >> 8;
        nSecond = nTimerValue & 0xFF;
    }
    if(nTimerID)
    {
        gSATTimerValue[nTimerID - 1][nSimID] = nTimerValue ;
    }
    BOOL bRet = FALSE;
    /*
    -   TIMER MANAGEMENT
    bits 1 to 2
            00 = start
            01 = deactivate
            10 = get current value
            11 = RFU
    bits 3 to 8 RFU
    */
    CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "nComQualifier %d\n", nComQualifier);
    if( 0x00 == nComQualifier )
    {
        bRet = COS_SetTimerEX(0xFFFFFFFF, PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1, COS_TIMER_MODE_SINGLE, (nHour * 60 * 60 + nMinute * 60 + nSecond) * 1000 MILLI_SECOND);
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "COS_SetTimerEX	  %d\n", bRet);
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "nTimerID	  %d\n", nTimerID);
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "Second	  %d\n", (nHour * 60 * 60 + nMinute * 60 + nSecond));
    }
    else if( 0x01 == nComQualifier )
    {
        bRet = COS_KillTimerEX(0xFFFFFFFF, PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID + nSimID * 8 + nTimerID - 1);
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "COS_KillTimerEX	  %d\n", bRet);
        CSW_TRACE(CFW_SAT_TS_ID | C_DETAIL, "nTimerID	  %d\n", nTimerID);
    }

    UINT8 pResponseDataQual1[250] = {0x81, 0x03, 0x01, 0x27, 0x01, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00, 0x04, 0x01,
                                     0x01, 0xa5, 0x03, 0x00, 0x00, (UINT8)sxs_ExistTimer(HVY_TIMER_IN + PRV_CFW_STK_TM_ID1_TIMER_SIM0_ID
                                             + nSimID * 8 + nTimerID - 1) / 16384
                                    };
    UINT8 pResponseDataQual0[250] = {0x81, 0x03, 0x01, 0x27, 0x01, 0x82, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
    pResponseDataQual1[2] = CFW_GetCurrentCmdNum(nSimID);
    pResponseDataQual0[2] = CFW_GetCurrentCmdNum(nSimID);

    if(nComQualifier == 1)
        nRet = CFW_SatResponse (0xFF, 0x00, 0x0, pResponseDataQual1, 20,  AT_ASYN_GET_DLCI(nSimID), nSimID);
    else if(nComQualifier == 0)
        nRet = CFW_SatResponse (0xFF, 0x00, 0x0, pResponseDataQual0, 12,  AT_ASYN_GET_DLCI(nSimID), nSimID);
    AT_TC(g_sw_AT_SAT, "CFW_SatResponse nRet:0x%x!\n", nRet);

    return ERR_CONTINUE;
}

UINT32 AT_SAT_SpecialCMDSendUSSD(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{
    AT_TC(g_sw_AT_SAT, "%s\n", __func__);

    UINT8 nAlphaLen  = 0;
    UINT8 nUSSDLen   = 0;
    UINT8 *nAlphaStr = NULL;

    UINT8 *nUSSDStr     = NULL;
    UINT8 *pUSSDPacket  = NULL;
    UINT8   nUSSDdcs    = 0;

    UINT8   UTI     = AT_ASYN_GET_DLCI(nSimID);
    UINT32  nRet    = 0;

    UINT16 nMemsize = SIZEOF(CFW_SAT_SMS_RSP);
    UINT16 nLength = 0x00;
    UINT32 nIndex = 0x00;
    UINT8 *pOffset = pFetchData;

    AT_TC(g_sw_AT_SAT, "gATSATSendUSSDFlag %d\n", gATSATSendUSSDFlag[nSimID]);

    gATSATSendUSSDFlag[nSimID] = TRUE;
    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength   = pOffset[nIndex++];
        nAlphaLen = nLength;
        nAlphaStr = pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex    = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x0A) || (pOffset[nIndex] == 0x8A))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;

        nLength  = pOffset[nIndex++];
        nUSSDdcs = pOffset[nIndex++];

        nUSSDLen = nLength;
        nUSSDStr = (UINT8 *)pOffset + nIndex;
    }
    else
    {
        AT_TC(g_sw_AT_SAT, "Error Invalid data Not 0x0A or 0x8A But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;
    }
    nMemsize += nLength;
    pUSSDPacket = (UINT8 *)AT_MALLOC(nUSSDLen - 1); //-1 is DCS
    if(pUSSDPacket == NULL)
    {
        return ERR_NO_MORE_MEMORY;
    }
    memset(pUSSDPacket, 0xFF, nUSSDLen - 1);
    memcpy(pUSSDPacket, nUSSDStr, nUSSDLen - 1);

    AT_TC(g_sw_AT_SAT, " nAddrLen = %d", nUSSDLen);
    AT_TC_MEMBLOCK(g_sw_AT_SAT, pUSSDPacket, nUSSDLen - 1, 16);

    if (AT_SS_GetUSSDState())
        nRet = CFW_SsSendUSSD(pUSSDPacket, nUSSDLen - 1, 3, nUSSDdcs, AT_SS_GetUSSDNum(), nSimID);
    else
        nRet = CFW_SsSendUSSD(pUSSDPacket, nUSSDLen - 1, 3, nUSSDdcs, UTI, nSimID);

    if (nRet == ERR_SUCCESS)
    {
        AT_TC(g_sw_AT_SAT, " Success");
    }
    else
    {
        AT_TC(g_sw_AT_SAT, " CFW_SsSendUSSD error!!!! = %x", nRet);
    }

    AT_FREE(pUSSDPacket);
    pUSSDPacket = NULL;
    return ERR_SUCCESS;
}

UINT32 AT_SAT_SpecialCMDCallSetup(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{
    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDCallSetup AT_SAT_SpecialCMDCallSetup %d\n", gATSATSetupCallFlag[nSimID]);
    UINT8 nAlphaConfirmLen  = 0;
    UINT8 *pAlphaConfirmStr = NULL;

    UINT8 nAlphaSetupLen  = 0;
    UINT8 *pAlphaSetupStr = NULL;

    UINT8 nAddrLen  = 0;
    UINT8 nAddrType = 0;
    UINT8 *pAddrStr = NULL;

    UINT8 nSubAddrLen  = 0;
    UINT8 *pSubAddrStr = NULL;

    UINT8 nCapabilityCfgLen  = 0;
    UINT8 *pCapabilityCfgStr = NULL;

    UINT8 nTuint     = 0;
    UINT8 nTinterval = 0;

    CFW_SAT_CALL_RSP *pCallResp = NULL;
    UINT16 nMemsize = SIZEOF(CFW_SAT_CALL_RSP);
    UINT16 nLength = 0x00;
    UINT32 nIndex = 0x00;
    UINT8 *pOffset = pFetchData;

    gATSATSetupCallFlag[nSimID] = TRUE;
    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;
    nIndex = nIndex + 10;
    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength          = pOffset[nIndex++];
        nAlphaConfirmLen = nLength;
        pAlphaConfirmStr = pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x06) || (pOffset[nIndex] == 0x86))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength   = pOffset[nIndex++] - 1;
        nAddrLen  = nLength;
        nAddrType = pOffset[nIndex++];
        pAddrStr  = pOffset + nIndex;
    }
    else
    {
        AT_TC(g_sw_AT_SAT, "Error Invalid data Not 0x06 or 0x86 But 0x%x", pOffset[nIndex]);
        return ERR_INVALID_PARAMETER;;
    }
    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x07) || (pOffset[nIndex] == 0x87))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength           = pOffset[nIndex++];
        nCapabilityCfgLen = nLength;
        pCapabilityCfgStr = (UINT8 *)pOffset + nIndex;
    }
    else
        nLength = 0;

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x08) || (pOffset[nIndex] == 0x88)) // SubAddress
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength     = pOffset[nIndex++];
        nSubAddrLen = nLength;
        pSubAddrStr = (UINT8 *)pOffset + nIndex;
    }

    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x04) || (pOffset[nIndex] == 0x84)) // Duration
    {
        nTuint     = pOffset[nIndex + 2];
        nTinterval = pOffset[nIndex + 3];
        nLength    = 4;
    }
    nIndex = nIndex + nLength;
    nMemsize += nLength;
    if ((pOffset[nIndex] == 0x1E) || (pOffset[nIndex] == 0x9E))
        nIndex = nIndex + 4;

    if ((pOffset[nIndex] == 0x05) || (pOffset[nIndex] == 0x85))
    {
        if (pOffset[++nIndex] == 0x81)
            nIndex++;
        nLength        = pOffset[nIndex++];
        nAlphaSetupLen = nLength;
        pAlphaSetupStr = pOffset + nIndex;
    }
    nMemsize += nLength;
    pCallResp = (CFW_SAT_CALL_RSP *)AT_MALLOC(nMemsize); //4//46
    if (pCallResp == NULL)
    {
        AT_TC(g_sw_AT_SAT, "Error Malloc failed\n");
        return ERR_NO_MORE_MEMORY;;
    }
    pCallResp->nAlphaConfirmLen = nAlphaConfirmLen;
    pCallResp->pAlphaConfirmStr = (UINT8 *)pCallResp + SIZEOF(CFW_SAT_CALL_RSP);
    SUL_MemCopy8(pCallResp->pAlphaConfirmStr, pAlphaConfirmStr, nAlphaConfirmLen);

    pCallResp->nAddrLen  = nAddrLen;
    pCallResp->nAddrType = nAddrType;
    pCallResp->pAddrStr  = pCallResp->pAlphaConfirmStr + nAlphaConfirmLen;
    SUL_MemCopy8(pCallResp->pAddrStr, pAddrStr, nAddrLen);

    pCallResp->nCapabilityCfgLen = nCapabilityCfgLen;
    pCallResp->pCapabilityCfgStr = pCallResp->pAddrStr + nAddrLen;
    SUL_MemCopy8(pCallResp->pCapabilityCfgStr, pCapabilityCfgStr, nCapabilityCfgLen);

    pCallResp->nSubAddrLen = nSubAddrLen;
    pCallResp->pSubAddrStr = pCallResp->pCapabilityCfgStr + nCapabilityCfgLen;
    SUL_MemCopy8(pCallResp->pSubAddrStr, pSubAddrStr, nSubAddrLen);

    pCallResp->nTuint     = nTuint;
    pCallResp->nTinterval = nTinterval;

    pCallResp->nAlphaSetupLen = nAlphaSetupLen;
    pCallResp->pAlphaSetupStr = pCallResp->pSubAddrStr + nSubAddrLen;
    SUL_MemCopy8(pCallResp->pAlphaSetupStr, pAlphaSetupStr, nAlphaSetupLen);

    CFW_DIALNUMBER nDialNumber;
    UINT8 nCCIndex = 0x00;
    nDialNumber.pDialNumber = AT_MALLOC(pCallResp->nAddrLen);
    SUL_MemSet8(nDialNumber.pDialNumber, 0x00, pCallResp->nAddrLen);
    SUL_MemCopy8(nDialNumber.pDialNumber, pCallResp->pAddrStr, pCallResp->nAddrLen);
    nDialNumber.nDialNumberSize = pCallResp->nAddrLen;
    nDialNumber.nType = pCallResp->nAddrType;

    if (pCallResp != NULL)
    {
        AT_FREE(pCallResp);
        pCallResp = NULL;
    }
    CFW_CcInitiateSpeechCallEx(&nDialNumber, &nCCIndex, AT_ASYN_GET_DLCI(nSimID), nSimID);
    return ERR_SUCCESS;
}

UINT8 *gSATSpecialCMDData[CFW_SIM_COUNT] = {NULL,};
UINT8 gSATSpecialCMDDataLen[CFW_SIM_COUNT] = {0x00,};

UINT32 AT_SAT_SpecialCMDProcess(UINT8 *pFetchData, UINT32 nFetchDatalen, CFW_SIM_ID nSimID)
{
    if(pFetchData == NULL)
    {
        return 0x00;
    }

    AT_TC_MEMBLOCK(g_sw_AT_SAT, pFetchData, nFetchDatalen, 16);

    UINT32 nIndex   = 0x00;
    UINT32 nLength  = 0x00;
    UINT32 nRet     = 0x00;
    UINT8 *pOffset  = pFetchData;

    if (pOffset[1] == 0x81)
        nIndex = 2;
    else
        nIndex = 1;

    nLength  = pOffset[nIndex] + nIndex; // Update the length

    AT_TC(g_sw_AT_SAT, "%s DataLen[%d]\n\n", __func__, nLength);
    UINT8 nSAT_CmdType      = pOffset[nIndex + 4];
    AT_TC(g_sw_AT_SAT, "Fetch data nSAT_CmdType 0x%x\n\n", nSAT_CmdType);

    switch(nSAT_CmdType)
    {
    //SAT PROVIDE LOCAL INFORMATION
    case 0x26:
        nRet = AT_SAT_SpecialCMDProvideLocalInfo(pFetchData, nFetchDatalen, nSimID);
        break;
    //SAT SMS
    case 0x13:
        AT_SAT_SpecialCMDSendSMS(pFetchData, nFetchDatalen, nSimID);
        break;
#if 0
    //SAT USSD
    case 0x12:
        AT_SAT_SpecialCMDSendUSSD(pFetchData, nFetchDatalen, nSimID);
        break;

    case 0x05:
    {
        UINT8 nRspData[] = {0x81, 0x03, 0x02, 0x05, 0x00, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
        CFW_SatResponse(0xFF, 0x00, 0, nRspData, 0x0b, AT_ASYN_GET_DLCI(nSimID), nSimID);
        nRet = ERR_CONTINUE;
    }
    break;
    case 0x03:
    {
        UINT8 nRspData[] = {0x81, 0x03, 0x02, 0x03, 0x00, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00};
        CFW_SatResponse(0xFF, 0x00, 0, nRspData, 0x0b, AT_ASYN_GET_DLCI(nSimID), nSimID);
        nRet = ERR_CONTINUE;
    }
#endif
    break;
    //SAT CALL
    case 0x10:
    {
        if(gSATSpecialCMDData[nSimID])
        {
            AT_FREE(gSATSpecialCMDData[nSimID]);
            gSATSpecialCMDData[nSimID] = NULL;
        }

        gSATSpecialCMDData[nSimID] = (UINT8 *)AT_MALLOC(nFetchDatalen);
        SUL_ZeroMemory8(gSATSpecialCMDData[nSimID], nFetchDatalen);
        SUL_MemCopy8(gSATSpecialCMDData[nSimID], pFetchData, nFetchDatalen);
        gSATSpecialCMDDataLen[nSimID] = nFetchDatalen;
    }
    AT_TC(g_sw_AT_SAT, "AT_SAT_SpecialCMDCallSetup no process\n\n");
        //AT_SAT_SpecialCMDCallSetup(pFetchData, nFetchDatalen,nSimID);
    break;
    case 0x01:
    {
        AT_SAT_SpecialCMDRefresh(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    case 0x21:
    {
        AT_SAT_SpecialCMDDisplayTxt(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    case 0x3:
    {
        AT_SAT_SpecialCMDPollInterval(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    case 0x27:
    {
        nRet = AT_SAT_SpecialCMDTimerManagement(pFetchData, nFetchDatalen, nSimID);
    }
    break;
    default:
        break;
    }
    if(ERR_CONTINUE == nRet)
        return 0x00;
    else
        return nSAT_CmdType;
}

UINT8 gDestAsciiData[CFW_SIM_COUNT][512]            = {{0x00,},};
UINT8 gDestAsciiDataSetUpMenu[CFW_SIM_COUNT][512]   = {{0x00,},};

UINT8 _GetStarSharp(UINT8 n)
{
    if( 0x0a == n ) return '*';
    else if( 0x0b == n ) return '#';
    else if( 0x0c == n ) return 'p';
    else if( 0x0d == n ) return 'w';
    else if( 0x0e == n ) return 'w';
    else if( 0x0f == n ) return 'w';
    else return (n + '0');
}

UINT8 *_GetSSString(UINT8 *pString, UINT32 nLen)
{
    UINT8 *p = (UINT8 *)AT_MALLOC(2 * nLen + 2);
    if(p == NULL)
        return 0;
    SUL_ZeroMemory8(p, 2 * nLen + 2);

    UINT32 i = 0;
    UINT32 j = 0;
    for( ; i < nLen; i++)
    {
        p[j] = _GetStarSharp(pString[i] & 0xF);
        p[j + 1] = _GetStarSharp((pString[i] >> 4) & 0xF);
        j = j + 2;
    }

    if(nLen)
    {
        if(p[2 * nLen - 1] == 'w') p[2 * nLen - 1] = 0x00;
    }

    AT_TC(g_sw_AT_SAT, "_GetUSSDString strlen %d %s\n", strlen(p), p);
    return p;
}


#define _IsAddressTag(nTag)     ((nTag == 0x06) ||  (nTag == 0x86))
#define _IsSSStringTag(nTag)    ((nTag == 0x09) ||  (nTag == 0x89))
#define _IsUSSDStringTag(nTag)  ((nTag == 0x0A) ||  (nTag == 0x8A))

/*
SS string
Byte(s) Description Length
1   SS string tag   1
2 to (Y 1)+2    Length (X)  Y
(Y 1)+3     TON and NPI 1
(Y 1)+4 to (Y 1)+X+2    SS or USSD string   X - 1

*/
UINT32 _ProcessCallControlSS(UINT8 *pData, CFW_SIM_ID nSim)
{
    UINT32 nRet = ERR_SUCCESS;
    UINT8 *w     = _GetSSString( &( pData[5] ), pData[3] - 1 );

    gATSATSendUSSDFlag[nSim] = TRUE;
    nRet = CFW_SsSendUSSD(w, strlen(w), 131, 0, AT_ASYN_GET_DLCI(nSim), nSim);

    AT_FREE(w);
    w = NULL;

    AT_TC(g_sw_AT_SAT, "_ProcessCallControlSS nRet 0x%x", nRet);

    if( ERR_SUCCESS != nRet )
    {
        CFW_SatResponse(0x11, 0x34, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSim), nSim);
        gATSATSendUSSDFlag[nSim] = FALSE;
    }

    return nRet;
}


/*
USSD string
Byte(s) Description Length
1   USSD string tag 1
2 to (Y-1)+2    Length (X)  Y
(Y-1)+3 Data coding scheme  1
(Y-1)+4 to (Y-1)+X+2    USSD string X-1
*/
UINT32 _ProcessCallControlUSSD(UINT8 *pData, CFW_SIM_ID nSim)
{
    UINT32 nRet = ERR_SUCCESS;
    //UINT8*w     = _GetUSSDString( &( pData[5] ), pData[3] - 1 );

    gATSATSendUSSDFlag[nSim] = TRUE;
    nRet = CFW_SsSendUSSD(&(pData[5]), pData[3], 3, pData[4], AT_ASYN_GET_DLCI(nSim), nSim);
    AT_TC(g_sw_AT_SAT, "_ProcessCallControlUSSD nRet 0x%x", nRet);
    if( ERR_SUCCESS != nRet )
    {
        CFW_SatResponse(0x12, 0x34, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSim), nSim);
        gATSATSendUSSDFlag[nSim] = FALSE;
    }
    return nRet;
}

/*
Address
Byte(s) Description Length
1   Address tag 1
2 to (Y 1)+2    Length (X)  Y
(Y 1)+3 TON and NPI 1
(Y 1)+4 to (Y 1)+X+2    Dialling number string  X 1
*/
UINT32 _ProcessCallControlAdress(UINT8 *pData, CFW_SIM_ID nSim)
{
    UINT32 nRet = ERR_SUCCESS;
    gATSATSetupCallFlag[nSim] = TRUE;

    CFW_DIALNUMBER nDialNumber;
    UINT8 nCCIndex = 0x00;

    nDialNumber.pDialNumber = AT_MALLOC(pData[3]);
    SUL_MemSet8(nDialNumber.pDialNumber, 0x00, pData[3]);
    SUL_MemCopy8(nDialNumber.pDialNumber, &(pData[5]), pData[3] - 1);
    nDialNumber.nDialNumberSize = pData[3] - 1;
    nDialNumber.nType           = pData[4];

    nRet = CFW_CcInitiateSpeechCallEx(&nDialNumber, &nCCIndex, AT_ASYN_GET_DLCI(nSim), nSim);
    AT_TC(g_sw_AT_SAT, "_ProcessCallControlAdress nRet 0x%x", nRet);
    if( ERR_SUCCESS != nRet )
    {
        CFW_SatResponse(0x10, 0x34, 0x00, NULL, 0x00, AT_ASYN_GET_DLCI(nSim), nSim);
        gATSATSetupCallFlag[nSim] = FALSE;
    }
    return nRet;
}

UINT8 _GetCallControlStringTag(UINT8 *pData)
{
    return (pData[2]);
}

#ifdef AT_STSF
//Start For STSF command
#define SIM_SAT_REFRESH_COM             0x01
#define SIM_SAT_SEND_SS_COM             0x11
#define SIM_SAT_SEND_USSD_COM           0x12
#define SIM_SAT_SEND_SMS_COM            0x13
#define SIM_SAT_PLAY_TONE_COM           0x20
//End For STSF command
#endif

VOID AT_SAT_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT CfwEvent;
    CFW_EVENT *pCfwEvent = &CfwEvent;

    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, pCfwEvent);

    nSim = pCfwEvent->nFlag;
    AT_TC(g_sw_AT_SAT, "SAT_AsyncEventProcess: nEventId = %d, nType:0x%x\n", pCfwEvent->nEventId, pCfwEvent->nType);

    if ( EV_CFW_SAT_RESPONSE_RSP == pCfwEvent->nEventId )
    {
        if (pCfwEvent->nType == 0)
        {
            AT_TC(g_sw_AT_SAT, "SAT_AsyncEventProcess: gATSATSetupCallFlag=%d, gATSATLocalInfoFlag=%d,gATSATSendUSSDFlag=%d,gATSATSendSMSFlag=%d \n",
                  gATSATSetupCallFlag[nSim], gATSATLocalInfoFlag[nSim], gATSATSendUSSDFlag[nSim], gATSATSendSMSFlag[nSim]);

            if(!(gATSATSetupCallFlag[nSim] || gATSATLocalInfoFlag[nSim] || gATSATSendUSSDFlag[nSim] || gATSATSendSMSFlag[nSim]))
            {
                AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",AT_ASYN_GET_DLCI(nSim));
            }
            else
            {
                gATSATLocalInfoFlag[nSim] = FALSE;
                gATSATSendUSSDFlag[nSim] = FALSE;
                gATSATSendSMSFlag[nSim] = FALSE;
                gATSATSetupCallFlag[nSim] = FALSE;
                gATSATSetupCallProcessingFlag[nSim]= FALSE;
            }
            AT_TC(g_sw_AT_SAT, "EV_CFW_SAT_RESPONSE_RSP    CMD_FUNC_SUCC  pCfwEvent->nParam1~2 0x%x 0x%x", pCfwEvent->nParam1, pCfwEvent->nParam2);
            if((0xD4 != pCfwEvent->nParam1 ) && (0xFE != pCfwEvent->nParam1))
            {
                if (0x90 == pCfwEvent->nParam2)
                {
                    UINT8 nString[] = "STNN";
                    AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,nString,AT_ASYN_GET_DLCI(nSim));
                }
            }
            else //if(0xD4 == pCfwEvent->nParam1)
            {
                CFW_SAT_TERMINAL_ENVELOPE_CNF *p = (CFW_SAT_TERMINAL_ENVELOPE_CNF *)( pCfwEvent->nParam2);
#ifndef CHIP_HAS_AP
                if ( 0x90 == ( p->Sw1 & 0xFF ))
                {
                    UINT8 nString[] = "STNN";
                    AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,nString,AT_ASYN_GET_DLCI(nSim));
                    pEvent->nParam1 = 0;
                    return;
                }
#endif
                AT_TC(g_sw_AT_SAT, "EV_CFW_SAT_RESPONSE_RSP STNN 0x%x p->NbByte %d", pCfwEvent->nParam1, p->NbByte);
                if( 0x00 != p->NbByte )
                {
                    /*
                    '00' = Allowed, no modification
                    '01' = Not allowed
                    '02' = Allowed with modifications
                    */
                    UINT8 nFlag =  (p->ResponseData[0]) & 0xFF;
                    UINT8 *pString = NULL;
                    AT_TC_MEMBLOCK(g_sw_AT_SAT, p->ResponseData, p->NbByte, 16);
                    if(0x00 == nFlag)
                    {
                        pString = "Allowed, no modification";
                    }
                    else if(0x01 == nFlag)
                    {
                        pString = "Not allowed";
                    }
                    else if(0x02 == nFlag)
                    {
                        /*
                        Description Section M/O Min Length
                        Call control result -   M   Y   1
                        Length (A+B+C+D+E+F)    -   M   Y   1 or 2
                        Address or SS string or USSD string 12.1, 12.14 or 12.17
                        O
                        N
                        A
                        Capability configuration parameters 1   12.4    O   N   B
                        Subaddress  12.3    O   N   C
                        Alpha identifier    12.2    O   N   D
                        BC repeat indicator 12.42   M/O N   E
                        Capability configuration parameters 2   12.4    O   N   F
                        */
                        pString = "Allowed with modifications";

                        UINT32 nRet = ERR_SUCCESS;
                        UINT8 nTag = _GetCallControlStringTag(p->ResponseData);
                        if(_IsAddressTag(nTag))
                        {
                            AT_TC(g_sw_AT_SAT, "_IsAddressTag");
                            nRet = _ProcessCallControlAdress(p->ResponseData, nSim);
                        }
                        else if(_IsSSStringTag(nTag))
                        {
                            AT_TC(g_sw_AT_SAT, "_IsSSStringTag");
                            nRet = _ProcessCallControlSS(p->ResponseData, nSim);
                        }
                        else if(_IsUSSDStringTag(nTag))
                        {
                            AT_TC(g_sw_AT_SAT, "_IsUSSDStringTag");
                            nRet = _ProcessCallControlUSSD(p->ResponseData, nSim);
                        }
                        else
                        {
                            AT_TC(g_sw_AT_SAT, "else");
                        }

                        AT_TC(g_sw_AT_SAT, "After Proscess nRet 0x%x", nRet);
                    }
                    AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,pString,AT_ASYN_GET_DLCI(nSim));
                }
                else
                {
                    UINT8 nString[] = "STNN";
                    AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,nString,AT_ASYN_GET_DLCI(nSim));
                }
                AT_FREE(p);
            }
        }
        else
        {
            if(!(gATSATSetupCallFlag[nSim] || gATSATLocalInfoFlag[nSim] || gATSATSendUSSDFlag[nSim] || gATSATSendSMSFlag[nSim]))
            {
                AT_TC(g_sw_AT_SAT, "error code  p1=  %x,p2=%x\n", pCfwEvent->nParam1, pCfwEvent->nParam2);
                AT_SAT_Result_Err(CMD_FUNC_FAIL, AT_ASYN_GET_DLCI(nSim));
            }
            else
            {
                gATSATSetupCallFlag[nSim] = gATSATLocalInfoFlag[nSim] = gATSATSendUSSDFlag[nSim] = gATSATSendSMSFlag[nSim] = FALSE;
            }
            AT_TC(g_sw_AT_SAT, "EV_CFW_SAT_RESPONSE_RSP: CMD_FUNC_FAIL pCfwEvent->nType =0x%x",pCfwEvent->nType);
        }
        pEvent->nParam1 = 0;
        return;
    }
    else if (pCfwEvent->nEventId == EV_CFW_SAT_ACTIVATION_RSP)
    {
        AT_TC(g_sw_AT_SAT, "p1=%x,p2=%x,type==%x\n", pCfwEvent->nParam1, pCfwEvent->nParam2, pCfwEvent->nType);
        if (pCfwEvent->nType == 0)
        {
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",AT_ASYN_GET_DLCI(nSim));
            AT_TC(g_sw_AT_SAT, "EV_CFW_SAT_ACTIVATION_RSP    CMD_FUNC_SUCC");
            AllowedInstance = INSTANCE;
            if (0x90 == pCfwEvent->nParam2)
            {
                UINT8 nString[] = "STNN";
                AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,nString,AT_ASYN_GET_DLCI(nSim));
            }
        }
        else
        {
            AT_SAT_Result_Err(CMD_FUNC_FAIL, AT_ASYN_GET_DLCI(nSim));
        }
        pEvent->nParam1 = 0;
        return;
    }
    else if (pCfwEvent->nEventId == EV_CFW_SAT_CMDTYPE_IND)
    {
        if(CFW_GetSATIndFormate())
        {
            UINT8 nMessage[30] = {0};
            UINT8 nString[] = "^STN: %u";

            AT_Sprintf(nMessage, nString, pCfwEvent->nParam1);
            AT_TC(g_sw_AT_SAT, "nMessage == %s", nMessage);
            AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,nMessage,AT_ASYN_GET_DLCI(nSim));
#ifdef AT_STSF
//Start For STSF command
            //Start timer
            if(STK_TimeOut != 0)
            {
                // TODO: COS_SetTimer(CSW_AT_TASK_HANDLE,  ATE_SAT_TIMER,COS_TIMER_MODE_SINGLE, STK_TimeOut * 1000 MILLI_SECOND);
                AT_TC(g_sw_AT_SAT, "====================timer start...====ATE_SAT_TIMER=%d==================",ATE_SAT_TIMER);
                AT_TC(g_sw_AT_SAT, "---------------ATE_SAT_TIMER----------------");
            }
            if(STK_AutoResp == 1)
            {
                UINT8 CmdType = pCfwEvent->nParam1;
                UINT8 nResponse[50] = {0};
                switch(CmdType)
                {
                case SIM_SAT_PLAY_TONE_COM:
                case SIM_SAT_REFRESH_COM:
                case SIM_SAT_SEND_SS_COM:
                case SIM_SAT_SEND_USSD_COM:
                case SIM_SAT_SEND_SMS_COM:
                {
                    void* pCmdData = 0;
                    if(ERR_SUCCESS != CFW_SatGetInformation(CmdType, &pCmdData , nSim))
                    {
                        AT_TC(g_sw_AT_SAT, "The inforation of Command %d cannot been retrieved!", CmdType);
                        AT_Sprintf(nResponse,"+STGI : Unknow Command");
                    }
                    if(CmdType == SIM_SAT_PLAY_TONE_COM)
                    {
                        CFW_SAT_PLAYTONE_RSP* pPlayTone = (CFW_SAT_PLAYTONE_RSP*)pCmdData;
                        //Add process Play Tone code here
                        AT_Sprintf(nResponse,"+STGI : %d", pPlayTone->Tone);

                    }
                    else if(CmdType == SIM_SAT_REFRESH_COM)
                    {
                        CFW_SAT_FILELIST_RSP* pFilelist = (CFW_SAT_FILELIST_RSP*)pCmdData;
                        //Add process File list code here
                        AT_Sprintf(nResponse,"+STGI : %d", pFilelist->nFiles[0]);

                    }
                    else if(CmdType == SIM_SAT_SEND_SS_COM)
                    {
                        CFW_SAT_SS_RSP* pSS = (CFW_SAT_SS_RSP*)pCmdData;
                        //Add process SS code here
                        AT_Sprintf(nResponse,"+STGI : %s", pSS->pSSStr);
                    }
                    else if(CmdType == SIM_SAT_SEND_USSD_COM)
                    {
                        CFW_SAT_USSD_RSP* pUSSD = (CFW_SAT_USSD_RSP*)pCmdData;
                        //Add process USSD code here
                        AT_Sprintf(nResponse,"+STGI : %s", pUSSD->pUSSDStr);
                    }
                    else if(CmdType == SIM_SAT_SEND_SMS_COM)
                    {
                        CFW_SAT_SMS_RSP* pSMS = (CFW_SAT_SMS_RSP*)pCmdData;
                        //Add process SMS code here
                        AT_Sprintf(nResponse,"+STGI : %d", pSMS->nPDULen);
                    }
                }

                }

                AT_SAT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,nResponse, AT_ASYN_GET_DLCI(nSim));
            }
//End For STSF command
#endif
        pEvent->nParam1 = 0;
        }
        else
        {
            UINT32 nCMD = AT_SAT_SpecialCMDProcess((UINT8 *)pCfwEvent->nParam1, pCfwEvent->nParam2, nSim);
            if(0x00 == nCMD)
            {
                return;
            }

            AT_TC(g_sw_AT_SAT, "We got sat command");
            // AT_TC_MEMBLOCK(g_sw_SAT, (UINT8*)pCfwEvent->nParam1, AT_StrLen((UINT8*)pCfwEvent->nParam1), pCfwEvent->nParam2);
            UINT8 nDestAsciiData[512] = { 0x00, };
            UINT16 nDestAsciiDataLen  = 0x00;

            UINT32 i          = 0x00;
            UINT8 nSwitchData = 0x00;
            UINT8 *p          = (UINT8 *)pCfwEvent->nParam1;

            for (i = 0x00; i < pCfwEvent->nParam2; i++)
            {
                nSwitchData = (p[i] >> 4) + (p[i] << 4);
                p[i]        = nSwitchData;

            }
            nDestAsciiDataLen = SUL_GsmBcdToAsciiEx(((UINT8 *)pCfwEvent->nParam1), pCfwEvent->nParam2, nDestAsciiData);
            UINT8 nMessage[518] = { 0x00, };

            strcpy(nMessage, "^STN:");  // by wulc debug 20121012
            strcat(nMessage, nDestAsciiData);

            //if( 0x25 == nCMD )
            SUL_MemCopy8(gDestAsciiDataSetUpMenu[nSim], nDestAsciiData, 512);
            SUL_MemCopy8(gDestAsciiData[nSim], nDestAsciiData, 512);
            AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,nMessage,AT_ASYN_GET_DLCI(nSim));
        }
        return;
    }
    else
    {
        AT_TC(g_sw_AT_SAT, "SAT_AsyncEventProcess: WARNING no process nEventId = %d\n", pCfwEvent->nEventId);
        return;
    }
    return;
}

#ifndef AT_STSF
VOID AT_SAT_CmdFunc_STSF(AT_CMD_PARA *pParam)
{
}

//extern UINT32 CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID);
VOID AT_SAT_CmdFunc_STA(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    AT_TC(g_sw_AT_SAT, "STA Function start...");

    if (pParam == NULL)
    {
        goto STA_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
        if (Param->pPara == NULL) // First parameter(Length in PDU mode,Address in Text mode) is NULL.
            goto STA_ERROR;

        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 NumOfParam = 0;
            UINT8 *Num       = Param->pPara;
            AT_TC(g_sw_AT_SAT, "6");

            if (ERR_SUCCESS != AT_Util_GetParaCount(Num, &NumOfParam))
                goto STA_ERROR;

            if (NumOfParam != 1)
                goto STA_ERROR;

            AT_TC(g_sw_AT_SAT, "5");
            // Get first parameter
            if ((Param->pPara[1] != 0) || (Param->pPara[0] > '1'))
                goto STA_ERROR;

            if (*Num == '1')
                gATCurrentAlphabet = UCS2_SET;
            else if (*Num == '0')
                gATCurrentAlphabet = GSM_SET;
            else
                goto STA_ERROR;

            if (AllowedInstance == NONE_INSTANCE)
            {
                if (ERR_SUCCESS == CFW_SatActivation(0, pParam->nDLCI, nSim))
                {
                    AT_SAT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
                    g_ActiveSatFlag = 1;
                }
                else
                    AT_SAT_Result_Err(CMD_FUNC_FAIL, pParam->nDLCI);
            }
            else
            {
                AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pParam->nDLCI);
            }
            AT_TC(g_sw_AT_SAT, "STA EXEC ok");
            return; // ERR_SUCCESS;
        }
        case AT_CMD_TEST:
        {
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"^STA: (0,1)",pParam->nDLCI);
            return;
        }
        case AT_CMD_READ:
        {
            CFW_PROFILE *pProfile;
            UINT8 nString[80] = {0};
            UINT8 *pDest = nString;

            pDest += AT_Sprintf(nString, "^STA: %u,%u,", gATCurrentAlphabet, AllowedInstance);
            // Adjust the code because csw code has been upgrade.
            CFW_SimGetMeProfile(&pProfile, nSim);
            SUL_hex2ascii(pProfile->pSat_ProfileData, pProfile->nSat_ProfileLen,pDest);
            AT_TC(g_sw_AT_SAT, "STA=(%s),LEN=%d", nString, pProfile->nSat_ProfileLen);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        default:
            goto STA_ERROR;
        }
    }

STA_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}

#else
VOID AT_SAT_CmdFunc_STA(AT_CMD_PARA *pParam)
{
}

//Start For STSF command
#define STK_ACTIVATES_PROFILE   0x00
#define STK_ACTIVATES           0x01
#define STK_DEACTIVATES         0x02
UINT8 g_STK_Profile = 0;            //This variable used to mark whether the profile Download has executed.
UINT8 g_Profile_Change = 0;         //This variable used to mark whether the profile data has changed or not

extern UINT32 CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID);
VOID AT_SAT_CmdFunc_STSF(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    PAT_CMD_RESULT pResult;
    AT_TC(g_sw_AT_SAT, "STSF Function start...");

    if (pParam == NULL)
        goto STSF_ERROR;
    AT_CMD_PARA *Param = pParam;
    if (Param->pPara == NULL) // First parameter(Length in PDU mode,Address in Text mode) is NULL.
        goto STSF_ERROR;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    switch (Param->iType)
    {
    case AT_CMD_SET:
    {
        UINT8 Mode = 0;
        UINT8 ProfileBuffer[12] = {0};
        UINT16 TimeOut = 0;
        UINT8 AutoResp =0;

        UINT8 NumOfParam = 0;
        INT32 RetValue = 0;

        if (ERR_SUCCESS != AT_Util_GetParaCount(Param->pPara, &NumOfParam))
            goto STSF_ERROR;

        AT_TC(g_sw_AT_SAT, "STSF: Verfy number of parameter!NumOfParam = %d",NumOfParam);
        if((NumOfParam == 0) || (NumOfParam > 4))
            goto STSF_ERROR;

        UINT16 uSize  = 1;
        RetValue = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &Mode, &uSize);
        if ((ERR_SUCCESS != RetValue) ||(uSize > 1)||(Mode > 2))
        {
            AT_TC(g_sw_AT_SAT, "STSF:ERROR!Get first parameter is err.");
            goto STSF_ERROR;
        }

        if(NumOfParam > 1)
        {
            uSize = 12;
            RetValue = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, &ProfileBuffer, &uSize);
            AT_TC(g_sw_AT_SAT, "============RetValue = %d,uSize =%d============",RetValue,uSize);

            if((RetValue == ERR_SUCCESS) &&(uSize < 11))
            {
                UINT8 ProfileDownload[6] = {0};
                UINT8 Length = SUL_ascii2hex((UINT8*)ProfileBuffer, (UINT8*)ProfileDownload);
                AT_TC(g_sw_AT_SAT, "Length = %d,uSize = %d",Length,uSize);
                if((Length == 0) ||(Length != (uSize >> 1)))
                    goto STSF_ERROR;
                CFW_SimSetMeProfile((UINT8*)ProfileDownload, Length);
                SUL_MemCopy8(STK_ProfileBuffer, ProfileBuffer, uSize);      //For Read command
                g_Profile_Change = 1;
            }
            else if(ERR_AT_UTIL_CMD_PARA_NULL == RetValue)
                STK_ProfileBuffer[0] = 0;
            else
                goto STSF_ERROR;
        }
        if(NumOfParam > 2)
        {
            uSize = 2;
            RetValue = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT16, &TimeOut, &uSize);
            AT_TC(g_sw_AT_SAT, "RetValue = %d--uSize = %d",RetValue,uSize);
            if((RetValue != ERR_SUCCESS) ||(TimeOut > 255))
                goto STSF_ERROR;
            STK_TimeOut = TimeOut;
        }

        if(NumOfParam > 3)
        {
            uSize = 1;
            RetValue = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_UINT8, &AutoResp, &uSize);
            AT_TC(g_sw_AT_SAT, "AutoResp = %d, uSize = %d",AutoResp, uSize);
            if((RetValue != ERR_SUCCESS) ||(uSize > 1)||((AutoResp & 0xFE) != 0))
                goto STSF_ERROR;
            AutoResp = 1;
        }

        if(Mode == 0)           //Deactives STK
        {
            if(ERR_SUCCESS != CFW_SatActivation(STK_DEACTIVATES, pParam->nDLCI, nSim))
                goto STSF_ERROR;
            if(g_Profile_Change == 1)
                g_STK_Profile = 0;
        }
        else if(Mode == 1)      //Activates STK
        {
            if(g_STK_Profile == 1)
            {
                if(ERR_SUCCESS != CFW_SatActivation(STK_ACTIVATES_PROFILE, pParam->nDLCI, nSim))
                    goto STSF_ERROR;
                g_STK_Profile = 1;
                g_Profile_Change = 0;
            }
            else
            {
                if(ERR_SUCCESS != CFW_SatActivation(STK_ACTIVATES, pParam->nDLCI, nSim))
                    goto STSF_ERROR;
            }
        }
        STK_Mode = Mode;
        AT_SAT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
        return;
    }

    case AT_CMD_TEST:
    {
        UINT8 nResp[] = "+STSF: (0-2),(160060C01F-5FFFFFFF7F),(1-255),(0-1)";
        AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nResp,pParam->nDLCI);
        return;
    }

    case AT_CMD_READ:
    {
        CFW_PROFILE *pProfile;
        UINT8 nResp[80]= {0};
        UINT8 ProfileBuffer[10] = {0};
        UINT8 i = 0, j;
        i += AT_Sprintf(nResp, "+STSF: %u", STK_Mode);

        if(STK_ProfileBuffer[0] == 0)
        {
            CFW_SimGetMeProfile(&pProfile,nSim);
            j = SUL_hex2ascii(pProfile->pSat_ProfileData, pProfile->nSat_ProfileLen,ProfileBuffer);
            if(j == 0)
                goto STSF_ERROR;

            i += AT_Sprintf(nResp + i , ",%s", ProfileBuffer);
            AT_TC(g_sw_AT_SAT, "ProfileDownload = %s, Length = %d", ProfileBuffer, pProfile->nSat_ProfileLen);
        }
        else
        {
            i += AT_Sprintf(nResp + i , ",%s", STK_ProfileBuffer);
            AT_TC(g_sw_AT_SAT, "ProfileDownload = %s, Length = %d,i = %d", STK_ProfileBuffer, AT_StrLen(STK_ProfileBuffer),i);
        }
        i += AT_Sprintf(nResp + i , ",%d,%d", STK_TimeOut, STK_AutoResp);
        AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nResp,pParam->nDLCI);
        return;
    }
    default:
        goto STSF_ERROR;
    }

STSF_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}
//End For STSF command
#endif

VOID AT_SAT_CmdFunc_STR(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    UINT32 nRet = ERR_SUCCESS;
    UINT8 InString[255] = {0,};
    UINT8 pSATPDUGsmData[255] = { 0x00, };
    UINT16 i   = 0;
    UINT8* pInput = InString;

    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_SAT, "CMD STR:ERROR! pParam == NULL");
        goto STR_ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
        AT_TC(g_sw_AT_SAT, "CMD STR:start  nSim:%d", nSim);

        if (Param->pPara == NULL) // First parameter(Length in PDU mode,Address in Text mode) is NULL.
        {
            AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Param->pPara == NULL");
            goto STR_ERROR;
        }

        AT_TC(g_sw_AT_SAT, "CMD STR: Param->iType %d", Param->iType);
        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 NumOfParam = 0;
            UINT8 *Num       = Param->pPara;
            UINT8 Buf[5];
            UINT8 Cmd, State, Item;
            UINT8 UTI = pParam->nDLCI;
            UINT8 length = 0;
            if (ERR_SUCCESS != AT_Util_GetParaCount(Num, &NumOfParam))
            {
                AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!GetParaCount");
                goto STR_ERROR;
            }
            AT_TC(g_sw_AT_SAT, "CMD STR:NumOfParam %d", NumOfParam);

            if ((NumOfParam < 2) || (NumOfParam > 5))
            {
                AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!NumOfParam reage is err.");
                goto STR_ERROR;
            }

            i   = 5;
            // Get first parameter, That is command type
            if (ERR_SUCCESS != AT_Util_GetPara(Param->pPara, 0, Buf, &i))
            {
                AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get first parameter is err.");
                goto STR_ERROR;
            }

            Cmd = AT_StrAToI(Buf);          // COMMAND ID
            AT_TC(g_sw_AT_SAT, "CMD STR: command ID : %d", Cmd);
            if(((Cmd & 0xFE) == 0xFE) && (NumOfParam == 3))
            {
                BOOL bRet = CFW_GetSATIndFormate();
                //bRet = CFW_SAT_IND_TEXT ;
                AT_TC(g_sw_AT_SAT, "CMD STR:Get Ind formate:%d", bRet);

                if (CFW_SAT_IND_PDU == bRet)
                {
                    UINT16 nSATDataLen = 400;
                    UINT8 pSATPDUData[400];

                    if (ERR_SUCCESS != AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, pSATPDUData, &nSATDataLen))
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get third parameter is err.");
                        goto STR_ERROR;
                    }

                    AT_TC(g_sw_AT_SAT, "Cmd %d  pSATPDUData %s", Cmd, pSATPDUData);
                    nSATDataLen = SUL_AsciiToGsmBcdEx(pSATPDUData, nSATDataLen, pSATPDUGsmData);

                    UINT8 *p = pSATPDUGsmData;
                    for (i = 0; i < nSATDataLen; i++)
                        p[i]  = (p[i] >> 4) + (p[i] << 4);
                    AT_TC(g_sw_AT_SAT, "Cmd %d  pSATPDUData %s", Cmd, pSATPDUData);

                    CFW_GetFreeUTI(0 , &UTI);
                    nRet = CFW_SatResponse(Cmd, 0x00, 0x00, pSATPDUGsmData, nSATDataLen, UTI, nSim);
                    I_AM_HERE();

                    if (ERR_SUCCESS == nRet)
                    {
                        AT_SAT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
                        return;
                    }
                    else
                    {
                        uErrorCode = CMD_FUNC_FAIL;
                        goto STR_ERROR;
                    }
                }
            }

            // Get second parameter, that is the result of previous command
            i = 5;
            if (ERR_SUCCESS != AT_Util_GetPara(Param->pPara, 1, Buf, &i))
            {
                AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get second parameter is err.");
                goto STR_ERROR;
            }

            State = AT_StrAToI(Buf);
            AT_TC(g_sw_AT_SAT, "CMD STR: command result : %d", State);
            // Get third parameter, that is the ID of menu item selected
            if (NumOfParam == 3 || NumOfParam == 4 || NumOfParam == 5)
            {
                // The first Code is null.
                i = 5;
                if (ERR_SUCCESS != AT_Util_GetPara(Param->pPara, 2, Buf, &i))
                {
                    AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get third parameter is err.");
                    goto STR_ERROR;
                }
                Item = AT_StrAToI(Buf);
            }
            else
                Item = 0;
            AT_TC(g_sw_AT_SAT, "CMD STR: The ID of Item Selected : %d", Item);
            // Get fourth parameter
            if (NumOfParam == 4  || NumOfParam == 5)
            {
                i = 254;
                // Get fourth parameter, that is input string
                if (ERR_SUCCESS != AT_Util_GetPara(Param->pPara, 3, InString, &i))
                {
                    AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get fourth parameter is err.");
                    goto STR_ERROR;
                }
                length = i - 2;     // the length of i include two '"'
                *(InString + length + 1) = 0;
                //For Debugging
                AT_TC(g_sw_AT_SAT, "%s", InString+1);

                if ((Cmd != 36 ) && (Cmd != 35 ) && (Cmd != 34 ) && (Cmd != 33 ) && (Cmd != 16 ) && (Cmd != 20 ) && (Cmd != 23 ) && (Cmd != 211 ))
                {
                    AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get 0 parameter isn't 35&&34 %d.", Cmd);
                    goto STR_ERROR;
                }
                AT_TC(g_sw_AT_SAT, "CMD STR: InString : %s", InString);
                if( NumOfParam == 5)
                {
                    UINT16 nSchLen  = 1;
                    UINT8 nSch  = 0;
                    nRet = AT_Util_GetParaWithRule(pParam->pPara, 4, AT_UTIL_PARA_TYPE_UINT8, &nSch, &nSchLen);

                    if (nRet != ERR_SUCCESS)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!Get 5 parameter is fail.");
                        goto STR_ERROR;
                    }
                    *InString = nSch;
                }
                else
                    *InString = 0x04;

                AT_TC(g_sw_AT_SAT, "CMD STR: The scheme of InString : %d", *InString);
                UINT8 *CmdData = NULL;
                if (Cmd == GET_INPUT_COM)
                {
                    UINT8 nMin, nMax;
                    if (CFW_SatGetInformation(Cmd, (PVOID)&CmdData, nSim) != ERR_SUCCESS)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!SatGetInformation is fail");
                        goto STR_ERROR;
                    }
                    CFW_SAT_INPUT_RSP *pGetInput = (CFW_SAT_INPUT_RSP *)CmdData;
                    nMax = pGetInput->nResMaxLen;
                    nMin = pGetInput->nResMinLen;

                    AT_TC(g_sw_AT_SAT, "max = %u,min=%u,Len=%u", nMax, nMin, length);
                    if ((length > nMax) || (length < nMin))
                        goto STR_ERROR;
                    length++;       //Plus one byte of sch
                }
                else if (Cmd == PROVIDE_LOCAL_INFO_COM) //In this case, needn't provide sch, length = strlen(InString+1)
                    pInput++;

//              else if (Cmd == GET_INKEY_COM )
//              {
//              }
            }
            AT_TC(g_sw_AT_SAT, "CMD STR:para vual Cmd0x%x,State:%d,Item:%d,length:%d", Cmd, State, Item, length);
            nRet = CFW_SatResponse(Cmd, State, Item, pInput, length, UTI, nSim);
            if(nRet == ERR_SUCCESS)
            {
                AT_SAT_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK,"",pParam->nDLCI);
                AT_TC(g_sw_AT_SAT, "CMD STR: SatResponse is OK.");
                return;
            }
            else
            {
                uErrorCode = CMD_FUNC_FAIL;
                AT_TC(g_sw_AT_SAT, "CMD STR:ERROR!SatResponse is fail.");
                goto STR_ERROR;
            }
        }
        case AT_CMD_TEST:
        {

            UINT8 nString[] = "^STR: (16,19,33,35,36,37,38,211,254,255)";
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        case AT_CMD_READ:
        {
            UINT8 nString[50];
            UINT8 nCmd = CFW_SatGetCurCMD(nSim);
            if (nCmd == 0xFF)
            {
                goto STR_ERROR;
            }

            AT_Sprintf(nString, "^STR: %u", nCmd);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        default:
            goto STR_ERROR;
        }
    }

STR_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}

//
//Using to get STN message data
//we will save the STN message for android.
//
VOID AT_SAT_CmdFunc_STNR(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    if (pParam == NULL)
        goto STNR_ERROR;
    else
    {
        AT_CMD_PARA *Param = pParam;
        UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
        AT_TC(g_sw_AT_SAT, "-------->>STNR Param->iType %d", Param->iType);

        if (Param->pPara == NULL) // First parameter(Length in PDU mode,Address in Text mode) is NULL.
        {
            AT_TC(g_sw_AT_SAT, "-------->>xx");
            goto STNR_ERROR;
        }
        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
#if 0
            UINT8 NumOfParam = 0;
            UINT8 *Num       = Param->pPara;
            UINT8 Buf[5];
            UINT8 Cmd, State, Item;
            UINT8 *InString = NULL;

            //CFW_SimListCountPbkEntries(CFW_PBK_SIM_FIX_DIALLING,1,2,3,0);

            CFW_SimInit( nSim );

            pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                                  CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

            I_AM_HERE();

            AT_Notify2ATM(pResult, pParam->nDLCI);

            if (pResult != NULL)
            {
                AT_FREE(pResult);
                pResult = NULL;
            }
            return;
#endif
            UINT8 nString[524] = {0x00,};
            AT_Sprintf(nString, "^STNR: %s", gDestAsciiDataSetUpMenu[nSim]);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        case AT_CMD_TEST:
        {
            UINT8 nString[] = "^STNR: (Setup Menu, Display text.......)";
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        case AT_CMD_READ:
        {
            UINT8 nString[524] = {0,};
            AT_Sprintf(nString, "^STNR: %s", gDestAsciiData[nSim]);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        default:
            goto STNR_ERROR;
        }
    }

STNR_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}

//Do communciation work from RIL
#define AT_STRC_MAKE_CALL   0x01
#define AT_STRC_SEND_SMS    0x02
#define AT_STRC_SEND_USSD   0x03

#define AT_STRC_OK          0x01
#define AT_STRC_CANCEL      0x00

VOID AT_SAT_CmdFunc_STRC(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    UINT32 nRet = ERR_SUCCESS;
    INT32 iRet = ERR_SUCCESS;

    if (pParam == NULL)
        goto STRC_ERROR;
    else
    {
        AT_CMD_PARA *Param = pParam;
        UINT8 nSimID = AT_SIM_ID(pParam->nDLCI);
        AT_TC(g_sw_AT_SAT, "-------->>AT_SAT_CmdFunc_STRC");
        if (Param->pPara == NULL) // First parameter(Length in PDU mode,Address in Text mode) is NULL.
        {
            AT_TC(g_sw_AT_SAT, "STRC");
            goto STRC_ERROR;
        }

        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT8 NumOfParam = 0;
            UINT8 *Num       = Param->pPara;
            UINT8 Buf;

            //UINT8* InString = NULL;
            //UINT8 UTI = 0;
            UINT8 nEnableFlag = 0x00;
            UINT16 i = 1;
            AT_TC(g_sw_AT_SAT, "AT_SAT_CmdFunc_STRC Enter AT_CMD_SET");
            if (ERR_SUCCESS != AT_Util_GetParaCount(Num, &NumOfParam))
                goto STRC_ERROR;

            AT_TC(g_sw_AT_SAT, "NumOfParam %d", NumOfParam);

            if (NumOfParam > 2)
            {
                goto STRC_ERROR;
            }

            // Get first parameter
            if (ERR_SUCCESS != (iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nEnableFlag, &i))) // Get first parameter.
            {
                AT_TC(g_sw_AT_SAT, "AT_Util_GetPara iRet Err 0x%x", iRet);
                goto STRC_ERROR;
            }

            i = 1;
            // Get second parameter
            if (ERR_SUCCESS != (iRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &Buf, &i))) // Get first parameter.
            {
                AT_TC(g_sw_AT_SAT, "AT_Util_GetPara iRet Err 0x%x", iRet);
                goto STRC_ERROR;
            }
            if( nEnableFlag == AT_STRC_OK )
            {
                switch(Buf)
                {
#if 0
                case AT_STRC_SEND_SMS:
                    AT_SAT_SpecialCMDSendSMS(gSATSpecialCMDData[nSimID], gSATSpecialCMDDataLen[nSimID], nSimID);
                    break;

                case AT_STRC_SEND_USSD:
                    AT_SAT_SpecialCMDSendUSSD(gSATSpecialCMDData[nSimID], gSATSpecialCMDDataLen[nSimID], nSimID);
                    break;
#endif
                case AT_STRC_MAKE_CALL:
                    if( gSATSpecialCMDData[nSimID] && gSATSpecialCMDDataLen[nSimID])
                    {
                        AT_SAT_SpecialCMDCallSetup(gSATSpecialCMDData[nSimID], gSATSpecialCMDDataLen[nSimID], nSimID);
                    }
                    else
                    {
                        goto STRC_ERROR;
                    }
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch(Buf)
                {
                case AT_STRC_SEND_SMS:
                    nRet = CFW_SatResponse(0x13, 10, 0x00, NULL, 0x00, pParam->nDLCI, nSimID);
                    break;
                case AT_STRC_SEND_USSD:
                    nRet = CFW_SatResponse(0x12, 10, 0x00, NULL, 0x00, pParam->nDLCI, nSimID);
                    break;
                case AT_STRC_MAKE_CALL:
                    nRet = CFW_SatResponse(0x10, 10, 0x00, NULL, 0x00, pParam->nDLCI, nSimID);
                    break;
                default:
                    break;
                }
                AT_TC(g_sw_AT_SAT, "User cancel nRet 0x%x", nRet);
            }
            AT_FREE(gSATSpecialCMDData[nSimID]);
            gSATSpecialCMDData[nSimID]      = NULL;
            gSATSpecialCMDDataLen[nSimID]   = 0x00;
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pParam->nDLCI);
            return;
        }
        case AT_CMD_TEST:
        {
            UINT8 nString[] = "^STRC: (AT_STRC_MAKE_CALL, AT_STRC_SEND_SMS, AT_STRC_SEND_USSD)";
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        case AT_CMD_READ:
        {
            UINT8 nString[50];
            UINT8 nCmd = CFW_SatGetCurCMD(nSimID);
            if (nCmd == 0xFF)
            {
                goto STRC_ERROR;
            }

            AT_Sprintf(nString, "^STRC: %u", nCmd);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        default:
            goto STRC_ERROR;
        }
    }

STRC_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}

VOID _OutputLocalLanuge(UINT16 *pUCS2, UINT32 nLen)
{
    UINT8 nOutString;
    UINT8 *pOutString = &nOutString ;
    UINT32 nOutStringLen;
    UINT16 *p = (UINT16 *)pOutString;

    if(nLen)
        ML_Unicode2LocalLanguageBigEnding((UINT8 *)pUCS2, nLen, &pOutString, &nOutStringLen, ML_CP936);
    AT_SAT_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR,pOutString, 0);
    AT_TC(g_sw_AT_SAT, "pOutString %s", p);
    AT_TC(g_sw_AT_SAT, "pOutString %x%x", pOutString[0], pOutString[1]);
    AT_FREE(pOutString);
}

VOID swap16bits(UINT8* pOutput, UINT8* pInput, UINT8 nLength)
{
    UINT8 i;
    for(i = 0; i < nLength; i+=2)
    {
        *(pOutput + i) = *(pInput + i + 1);
        *(pOutput + i + 1) = *(pInput + i);
        //AT_TC(g_sw_AT_SAT, "pOutString %02x%02x", *(pOutput + i) , *(pOutput + i + 1));
    }
}
extern VOID CSW_TC_MEMBLOCK(UINT16 uMask, UINT8 *buffer, UINT16 len, UINT8 radix);

UINT16 unicode2gbk(UINT8* pOutput, UINT8* pInput,UINT8 nLength)
{
    UINT8 k = 255;
    UINT8 output[255];

    swap16bits(output, pInput, nLength);
    //SUL_MemCopy8(output,pInput,nLength);
    UINT8* p;
    AT_TC(g_sw_AT_SAT, "nLength = %d",nLength);
    UINT32 nRet = ML_Unicode2LocalLanguage(output, nLength, &p, (UINT32*)&k, "cp936");
    AT_TC(g_sw_AT_SAT, "p = %x, k = %d", p, k);
    if(ERR_SUCCESS == nRet)
    {
        AT_MemCpy(pOutput,p,k);
        CSW_ML_FREE(p);
        CSW_TC_MEMBLOCK(CFW_ML_TS_ID | C_DETAIL, (UINT8 *)pOutput, 12 , 16);
        pOutput += k;

        return k;
    }
    else
        return 0;
}

//extern BOOL charset_gsm2ascii(UINT8 *src, UINT16 slen, UINT8 *dst, UINT16 *dlen);
VOID AT_SAT_CmdFunc_STGI(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    UINT8 *pMessageBuf = NULL;
    UINT16 len         = 0;

    if (pParam == NULL)
    {
        AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR! pParam == NULL");
        AT_SAT_Result_Err(uErrorCode, 0);
        return;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
        AT_TC(g_sw_AT_SAT, "CMD STGI:start  nSim:%d", nSim);
        if (Param->pPara == NULL) // First parameter(Length in PDU mode,Address in Text mode) is NULL.
        {
            AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR! Param->pPara == NULL");
            goto STGI_ERROR;
        }
        AT_TC(g_sw_AT_SAT, "CMD STGI: Param->iType %d", Param->iType);
        switch (Param->iType)
        {
        case AT_CMD_SET:
        {
            UINT16 Len = 5;
            UINT8 CmdBuf[5];
            UINT8 NumOfParam = 0;
            UINT8 *Num       = Param->pPara;
            UINT8 nCmdType   = 0;
            CFW_SAT_MENU_RSP *p = NULL;
            UINT8 *CmdData;

            if (ERR_SUCCESS != AT_Util_GetParaCount(Num, &NumOfParam))
            {
                AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!GetParaCount");
                goto STGI_ERROR;
            }

            if (NumOfParam != 1)
            {
                AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!NumOfParam reage is err.");
                goto STGI_ERROR;
            }
            // Get first parameter.
            if (ERR_SUCCESS != AT_Util_GetPara(Param->pPara, 0, CmdBuf, &Len))
            {
                AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!Get first parameter is err.");
                goto STGI_ERROR;
            }

            nCmdType = CmdBuf[0];
            AT_TC(g_sw_AT_SAT, "CmdBuf[0] = %x,Len = %d",CmdBuf[0], Len);
            if(Len == 2)
                nCmdType = (nCmdType - '0') * 10 + CmdBuf[1] - '0';
            else
                nCmdType = nCmdType - '0';
            AT_TC(g_sw_AT_SAT, "nCmdType = %x",nCmdType);

            BOOL nMode = CFW_GetSATIndFormate();
            AT_TC(g_sw_AT_SAT, "CMD STGI:Mode = %d",nMode);
            if(nMode == CFW_SAT_IND_PDU)
            {
                UINT8 nBuffer[50]= {0,};
                UINT8* q = nBuffer;
                AT_TC(g_sw_AT_SAT, "3");

                q += AT_Sprintf(q, "^STGI: %u(Unsupported in PDU mode!)", nCmdType);
                //q += AT_Sprintf(q,"(Unsupported in PDU mode!)");
                AT_TC(g_sw_AT_SAT, "1");
                AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nBuffer,pParam->nDLCI);
                return;
            }
            else
            {
                AT_TC(g_sw_AT_SAT, "nCmdType == %d, Len = %d", nCmdType, Len);
                AT_TC(g_sw_AT_SAT, "CmdBuf=%s : %x %x %x", CmdBuf, CmdBuf[0], CmdBuf[1], CmdBuf[2]);
                AT_TC(g_sw_AT_SAT, "Param ->pPara=%s : %x %x %x", Param->pPara, Param->pPara[0], Param->pPara[1],
                      Param->pPara[2]);
                UINT32 nReturn = 0;
                if ((nReturn = CFW_SatGetInformation(nCmdType, (PVOID)&CmdData, nSim)) != ERR_SUCCESS)
                {
                    AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!SatGetInformation is fail,nReturn = %x",nReturn);
                    goto STGI_ERROR;
                }

                AT_TC(g_sw_AT_SAT, "gATCurrentAlphabet = %d, GSM_SET = %d", gATCurrentAlphabet, GSM_SET);
                if (nCmdType == SETUP_MENU_COM || nCmdType == SELECT_ITEM_COM)
                {
                    UINT8 i;
                    UINT32 Count = 0;
                    UINT16 nNum, nMem;
                    UINT8 *q = NULL;

                    CFW_SAT_ITEM_LIST *pItemList = NULL;
                    p = (CFW_SAT_MENU_RSP *)CmdData;
                    if (p == NULL)
                        goto STGI_ERROR;

                    pItemList = p->pItemList;
                    AT_TC(g_sw_AT_SAT, "pItemList = %x",pItemList);
                    Count = AT_SAT_Count(pItemList);
                    nNum = Count >> 16;
                    nMem = Count & 0xFFFF;

                    AT_TC(g_sw_AT_SAT, "1nMem = %d,12*(UINT8)nNum=%d,%d", nMem, 27 * (UINT8)nNum, (p->nAlphaLen - 1) * 2 + 28);
                    nMem = (nMem<<1) + 19 * (UINT8)nNum + (MEM_ALIGN_SIZE(p->nAlphaLen - 1) << 1) + 10/*^STGI: 37,*/+ 2/*""*/+ 12/*3x???,*/;
                    AT_TC(g_sw_AT_SAT, "2nMem = %d", nMem);

                    pMessageBuf = (UINT8 *)AT_MALLOC(nMem);
                    AT_TC(g_sw_AT_SAT, "pMessageBuf Address = 0x%x, nMem = %x", pMessageBuf,nMem);

                    if (pMessageBuf == NULL)
                        goto STGI_ERROR;
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += AT_Sprintf(q, "^STGI: %u,0,%u", nCmdType, nNum);

                    q+=AT_Sprintf(q,",\"");
                    if (p->nAlphaLen > 0)
                    {
                        BOOL nDone = 0;
                        UINT16 nOutLen = nMem;
                        AT_TC(g_sw_AT_SAT, "p->nAlphaLen = %d",p->nAlphaLen);
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            AT_TC(g_sw_AT_SAT, "p->pAlphaStr[0] = %x",p->pAlphaStr[0]);
                            if((p->pAlphaStr[0]&0x80) == 0)
                            {
                                charset_gsm2ascii(p->pAlphaStr ,(UINT16)p->nAlphaLen,q,(UINT16*)&nOutLen);
                                q+= nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if(p->pAlphaStr[0] == 0x80)
                            {
                                AT_TC(g_sw_AT_SAT, "q  adddress= %x",q);
                                UINT8* pStart = q;
                                q += unicode2gbk(q, p->pAlphaStr + 1, p->nAlphaLen - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                            q += SUL_hex2ascii(p->pAlphaStr, p->nAlphaLen, q);
                    }
                    q+=AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, ",%u\r\n", p->nComQualifier);
                    if (pItemList != NULL)
                    {
                        for (i = 0; i < nNum; i++)
                        {
                            BOOL nDone = 0;
                            UINT16 nOutLen = nMem;
                            AT_TC(g_sw_AT_SAT, "nCmdType = %d,pItemList->nItemID = %d,nDone = %d",nCmdType, pItemList->nItemID, nDone);
                            q += AT_Sprintf(q, "%d,%d",nCmdType, pItemList->nItemID);

                            q+=AT_Sprintf(q,",\"");
                            if(pItemList->nItemLen > 0)
                            {
                                if(gATCurrentAlphabet == GSM_SET)
                                {
                                    AT_TC(g_sw_AT_SAT, "pItemList->pItemStr[0] = %x", pItemList->pItemStr[0]);
                                    if((pItemList->pItemStr[0]&0x80) == 0)
                                    {
                                        charset_gsm2ascii(pItemList->pItemStr, (UINT16)pItemList->nItemLen,q,(UINT16*)&nOutLen);
                                        q+=nOutLen;
                                        nDone = 1;
                                    }
                                }
                                else
                                {
                                    if(pItemList->pItemStr[0]  == 0x80)
                                    {

                                        UINT8* pStart = q;
                                        q += unicode2gbk(q, pItemList->pItemStr + 1, pItemList->nItemLen - 1);
                                        if(pStart != q)
                                            nDone = 1;
                                    }
                                }
                                if(nDone == 0)
                                    q += SUL_hex2ascii(pItemList->pItemStr + 1, pItemList->nItemLen - 1, q);
                            }
                            q+=AT_Sprintf(q,"\"");

                            q += AT_Sprintf(q, ",%u\r\n", p->nComQualifier);
                            AT_TC(g_sw_AT_SAT, "pItemList->ID = %x", pItemList->nItemID);

                            if (pItemList->pNextItem != NULL)
                                pItemList = pItemList->pNextItem;
                            else
                                break;
                        }
                    }
                    *q = '\0';

                    AT_TC(g_sw_AT_SAT, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    AT_TC(g_sw_AT_SAT, "36-37nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = AT_StrLen(pMessageBuf);
                    AT_TC(g_sw_AT_SAT, "pMessageBuf end Address = 0x%x", pMessageBuf+AT_StrLen(pMessageBuf));

                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pMessageBuf,pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == DISPLAY_TEXT_COM)
                {
                    UINT16 nMem;
                    CFW_SAT_DTEXT_RSP *pDText;
                    UINT8 *q = NULL;
                    pDText = (CFW_SAT_DTEXT_RSP *)CmdData;
                    if(pDText == NULL)
                        goto STGI_ERROR;
                    nMem        = 2 * (UINT8)(pDText->nTextLen) + 15 + 10;
                    pMessageBuf = (UINT8 *)AT_MALLOC(nMem);
                    if (pMessageBuf == NULL)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!nCmdType == 33 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;

                    q += AT_Sprintf(q, "^STGI: 33");
                    AT_TC(g_sw_AT_SAT, "pDText->nTextSch =%x", pDText->nTextSch);

                    BOOL nDone = 0;
                    UINT16 nOutLen = nMem;

                    q+=AT_Sprintf(q,",\"");
                    if(gATCurrentAlphabet == GSM_SET)
                    {
                        if((pDText->nTextSch&0xF4) == 0xF0)     //GSM 7 bits
                        {
                            charset_gsm2ascii(pDText->pTextStr, (UINT16)pDText->nTextLen,q,(UINT16*)&nOutLen);
                            q+=nOutLen;
                            nDone = 1;
                        }
                        else if((pDText->nTextSch&0xF4) == 0xF4)    //8 bits
                        {
                            AT_MemCpy(q,pDText->pTextStr,pDText->nTextLen);
                            q+= pDText->nTextLen;
                            nDone = 1;
                        }
                        else if((pDText->nTextSch & 0x20) == 0)
                        {
                            if((pDText->nTextSch & 0x0C) == 0)  //GSM 7 bits
                            {
                                charset_gsm2ascii(pDText->pTextStr, (UINT16)pDText->nTextLen,q,(UINT16*)&nOutLen);
                                q+=nOutLen;
                                nDone = 1;
                            }
                            else if((pDText->nTextSch & 0x0C) == 0x04)  //8 bits
                            {
                                AT_MemCpy(q,pDText->pTextStr,pDText->nTextLen);
                                q+= pDText->nTextLen;
                                nDone = 1;
                            }
                        }
                    }
                    else
                    {
                        if((pDText->nTextSch & 0x0C) == 0x08)       //UCS2 as Chinese
                        {
                            UINT8* pStart = q;
                            AT_TC(g_sw_AT_SAT, "pDText->nTextLen =%d", pDText->nTextLen);
                            AT_TC(g_sw_AT_SAT, "pDText->pTextStr =%x", pDText->pTextStr);
                            CSW_TC_MEMBLOCK(g_sw_AT_SAT, pDText->pTextStr, pDText->nTextLen, 16);
                            q += unicode2gbk(q, pDText->pTextStr, pDText->nTextLen);
                            if(pStart != q)
                                nDone = 1;
                        }
                    }
                    if(nDone == 0)
                        q += SUL_hex2ascii(pDText->pTextStr, pDText->nTextLen,q);
                    q+=AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, ",%u,%u\r\n", pDText->nTextSch, pDText->nComQualifier);
                    *q = '\0';

                    AT_TC(g_sw_AT_SAT, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    AT_TC(g_sw_AT_SAT, "33nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pMessageBuf,pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == PROVIDE_LOCAL_INFO_COM)
                {
                    SAT_BASE_RSP *pBase = NULL;
                    UINT8 nMessageBuf[20]= {0};
                    pBase = (SAT_BASE_RSP *)CmdData;
                    if(pBase == NULL)
                        goto STGI_ERROR;
                    AT_Sprintf(nMessageBuf, "^STGI: %u,%u", pBase->nComID, pBase->nComQualifier);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nMessageBuf,pParam->nDLCI);
                    return;
                }
                else if (nCmdType == SEND_SHORT_MESSAGE_COM)
                {
                    UINT16 nMem;
                    UINT8 *q = NULL;
                    CFW_SAT_SMS_RSP *pShortMsg;

                    pShortMsg   = (CFW_SAT_SMS_RSP *)CmdData;
                    if(pShortMsg == NULL)
                        goto STGI_ERROR;
                    nMem        = pShortMsg->nAlphaLen + pShortMsg->nAddrLen + pShortMsg->nPDULen;
                    nMem        = 2 * nMem + 8 + 6 + 12;
                    pMessageBuf = (UINT8 *)AT_MALLOC(nMem);

                    if (pMessageBuf == NULL)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!nCmdType == 19 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += AT_Sprintf(q, "^STGI: 19");

                    AT_TC(g_sw_AT_SAT, "pShortMsg->pAlphaStr[0] = %x",pShortMsg->pAlphaStr);
                    AT_TC(g_sw_AT_SAT, "pShortMsg->nAlphaLen = %d",pShortMsg->nAlphaLen);

                    q+=AT_Sprintf(q,",\"");
                    if (pShortMsg->nAlphaLen > 0)
                    {
                        BOOL nDone = 0;
                        UINT16 nOutLen = nMem;

                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            if((pShortMsg->pAlphaStr[0]&0x80) == 0)
                            {
                                charset_gsm2ascii( pShortMsg->pAlphaStr, (UINT16)pShortMsg->nAlphaLen,q,(UINT16*)&nOutLen);
                                q+=nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if(pShortMsg->pAlphaStr[0] == 0x80)
                            {
                                UINT8* pStart = q;
                                q += unicode2gbk(q, pShortMsg->pAlphaStr + 1, pShortMsg->nAlphaLen - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                            q += SUL_hex2ascii(pShortMsg->pAlphaStr, pShortMsg->nAlphaLen, q);
                    }
                    q+=AT_Sprintf(q,"\"");
                    q += AT_Sprintf(q, ",%u", pShortMsg->nAddrType);

                    q += AT_Sprintf(q,",\"");
                    if (pShortMsg->nAddrLen > 0)
                    {
                        BOOL nDone = 0;
                        UINT16 nOutLen = nMem;
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            charset_gsm2ascii( pShortMsg->pAddrStr, (UINT16)pShortMsg->nAddrLen,q,(UINT16*)&nOutLen);
                            q += nOutLen;
                            nDone = 1;
                        }
                        else
                        {
                            q += SUL_GsmBcdToAscii(pShortMsg->pAddrStr, pShortMsg->nAddrLen,q);
                        }
                    }
                    q += AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q,",\"");
                    if (pShortMsg->nPDULen > 0)
                        q += SUL_hex2ascii(pShortMsg->pPDUStr, pShortMsg->nPDULen,q);
                    q += AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, "\r\n");

                    AT_TC(g_sw_AT_SAT, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    AT_TC(g_sw_AT_SAT, "19nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pMessageBuf,pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == SETUP_CALL_COM)
                {
                    UINT16 nMem = 0;
                    UINT8 *q    = NULL;
                    CFW_SAT_CALL_RSP *pCall = (CFW_SAT_CALL_RSP *)CmdData;
                    if(pCall == NULL)
                        goto STGI_ERROR;
                    nMem        = pCall->nAlphaConfirmLen + pCall->nAddrLen + pCall->nSubAddrLen + pCall->nAlphaSetupLen;
                    nMem        = 2 * (nMem + 16) + 6 + 17;
                    pMessageBuf = (UINT8 *)AT_MALLOC(nMem);
                    if (pMessageBuf == NULL)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!nCmdType == 16 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += AT_Sprintf(q, "^STGI: 16");

                    q+=AT_Sprintf(q,",\"");
                    if (pCall->nAlphaConfirmLen > 0)
                    {
                        BOOL nDone = 0;
                        UINT16 nOutLen = nMem;
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            if((pCall->pAlphaConfirmStr[0]&0x80) == 0)
                            {
                                charset_gsm2ascii(pCall->pAlphaConfirmStr, (UINT16)pCall->nAlphaConfirmLen,q,(UINT16*)&nOutLen);
                                q+=nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if(pCall->pAlphaConfirmStr[0] == 0x80)
                            {
                                UINT8* pStart = q;
                                q += unicode2gbk(q, pCall->pAlphaConfirmStr + 1, pCall->nAlphaConfirmLen - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                            q += SUL_hex2ascii(pCall->pAlphaConfirmStr, pCall->nAlphaConfirmLen,q);
                    }

                    q+=AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, ",%u", pCall->nAddrType);

                    q += AT_Sprintf(q, ",\"");
                    if (pCall->nAddrLen > 0)
                        q += SUL_GsmBcdToAscii(pCall->pAddrStr, pCall->nAddrLen,q);
                    q += AT_Sprintf(q, "\"");

                    q += AT_Sprintf(q, ",\"");
                    if (pCall->nSubAddrLen > 0)
                        q += SUL_GsmBcdToAscii(pCall->pSubAddrStr, pCall->nSubAddrLen,q);
                    q += AT_Sprintf(q, "\"");

                    q += AT_Sprintf(q, ",\"");
                    AT_TC(g_sw_AT_SAT, "pCall->nAlphaSetupLen = %d",pCall->nAlphaSetupLen);
                    CSW_TC_MEMBLOCK(g_sw_AT_SAT, pCall->pAlphaSetupStr, 50, 16);

                    if (pCall->nAlphaSetupLen > 0)
                    {
                        BOOL nDone = 0;
                        UINT16 nOutLen = nMem;
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            if((pCall->pAlphaSetupStr[0]&0x80) == 0)
                            {
                                charset_gsm2ascii(pCall->pAlphaSetupStr, (UINT16)pCall->nAlphaSetupLen,q,(UINT16*)&nOutLen);
                                q+=nOutLen;
                                nDone = 1;
                            }
                        }
                        else
                        {
                            if(pCall->pAlphaSetupStr[0] == 0x80)
                            {
                                UINT8* pStart = q;
                                q += unicode2gbk(q, pCall->pAlphaSetupStr + 1, pCall->nAlphaSetupLen - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                            q += SUL_hex2ascii(pCall->pAlphaSetupStr, pCall->nAlphaSetupLen,q);
                    }
                    q+=AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, ",%u,%u,%u\r\n", pCall->nTuint, pCall->nTinterval, pCall->nComQualifier);
                    *q = '\0';

                    AT_TC(g_sw_AT_SAT, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    AT_TC(g_sw_AT_SAT, "nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pMessageBuf,pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if (nCmdType == GET_INPUT_COM)
                {
                    UINT16 nMem;
                    UINT8 *q = NULL;
                    CFW_SAT_INPUT_RSP *pInput;

                    pInput      = (CFW_SAT_INPUT_RSP *)CmdData;
                    if(pInput == NULL)
                        goto STGI_ERROR;
                    nMem        = pInput->nDefTextLen + pInput->nTextLen;
                    nMem        = 2 * (nMem + 3) + 28 + 30;  /* 3; */
                    pMessageBuf = (UINT8 *)AT_MALLOC(nMem);
                    if (pMessageBuf == NULL)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STGI:ERROR!nCmdType == 35 malloc is fail");
                        goto STGI_ERROR;
                    }
                    AT_MemZero(pMessageBuf, nMem);
                    q = pMessageBuf;
                    q += AT_Sprintf(q, "^STGI: 35");

                    BOOL nDone = 0;
                    UINT16 nOutLen = nMem;
                    q += AT_Sprintf(q, ",\"");
                    if (pInput->nTextLen > 0)
                    {
                        AT_TC(g_sw_AT_SAT, "CMD STGI:pInput->nTextSch = %x",pInput->nTextSch);
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            if((pInput->nTextSch&0xF4) == 0xF0)
                            {
                                charset_gsm2ascii(pInput->pTextStr, (UINT16)pInput->nTextLen,q,(UINT16*)&nOutLen);
                                q+=nOutLen;
                                nDone = 1;
                            }
                            else if((pInput->nTextSch&0xF4) == 0xF4)
                            {
                                AT_MemCpy(q,pInput->pTextStr,pInput->nTextLen);
                                q+= pInput->nTextLen;
                                nDone = 1;
                            }
                            else if((pInput->nTextSch & 0x20) == 0)
                            {
                                if((pInput->nTextSch & 0x0C) == 0)
                                {
                                    charset_gsm2ascii(pInput->pTextStr, (UINT16)pInput->nTextLen,q,(UINT16*)&nOutLen);
                                    q+=nOutLen;
                                    nDone = 1;
                                }
                                else if((pInput->nTextSch & 0x0C) == 0x04)
                                {
                                    AT_MemCpy(q,pInput->pTextStr,pInput->nTextLen);
                                    q+= pInput->nTextLen;
                                    nDone = 1;
                                }
                            }
                        }
                        else
                        {
                            if((pInput->nTextSch & 0x0C) == 0x08)
                            {
                                UINT8* pStart = q;
                                q += unicode2gbk(q, pInput->pTextStr, pInput->nTextLen - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                            q += SUL_hex2ascii(pInput->pTextStr, pInput->nTextLen,q);
                    }
                    q+=AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, ",%u", pInput->nDefTextSch);
                    nOutLen = nMem;

                    q += AT_Sprintf(q, ",\"");
                    if (pInput->nDefTextLen > 0)
                    {
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            if((pInput->nDefTextSch&0xF4) == 0xF0)
                            {
                                charset_gsm2ascii(pInput->pDefTextStr, (UINT16)pInput->nDefTextLen,q,(UINT16*)&nOutLen);
                                q+=nOutLen;
                                nDone = 1;
                            }
                            else if((pInput->nDefTextSch&0xF4) == 0xF4)
                            {
                                AT_MemCpy(q, pInput->pDefTextStr, pInput->nDefTextLen);
                                q+= pInput->nTextLen;
                                nDone = 1;
                            }
                            else if((pInput->nDefTextSch & 0x20) == 0)
                            {
                                if((pInput->nDefTextSch & 0x0C) == 0)
                                {
                                    charset_gsm2ascii(pInput->pDefTextStr, (UINT16)pInput->nDefTextLen,q,(UINT16*)&nOutLen);
                                    q+=nOutLen;
                                    nDone = 1;
                                }
                                else if((pInput->nDefTextSch & 0x0C) == 0x04)
                                {
                                    AT_MemCpy(q,pInput->pDefTextStr,pInput->nDefTextLen);
                                    q+= pInput->nTextLen;
                                    nDone = 1;
                                }
                            }
                        }
                        else
                        {
                            if((pInput->nDefTextSch & 0x0C) == 0x08)
                            {
                                UINT8* pStart = q;
                                q += unicode2gbk(q, pInput->pDefTextStr, pInput->nDefTextLen - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                            q += SUL_hex2ascii(pInput->pDefTextStr, pInput->nDefTextLen,q);
                    }
                    q+=AT_Sprintf(q,"\"");

                    q += AT_Sprintf(q, ",%u", pInput->nDefTextSch);
                    q += AT_Sprintf(q, ",%u,%u,%u", pInput->nResMaxLen, pInput->nResMinLen, pInput->nComQualifier);
                    q += AT_Sprintf(q, "\r\n");
                    *q = '\0';
                    AT_TC(g_sw_AT_SAT, "Message end = %x,finished address =%x", pMessageBuf + nMem, q);
                    AT_TC(g_sw_AT_SAT, "35nMem = %d,message len = %d", nMem, AT_StrLen(pMessageBuf));

                    len = strlen(pMessageBuf);
                    AT_Util_TrimRspStringSuffixCrLf(pMessageBuf, &len);
                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pMessageBuf,pParam->nDLCI);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else if(nCmdType == SEND_SS_COM)
                {
                    AT_TC(g_sw_AT_SAT, "SEND_SS_COM");
                }
                else if(nCmdType == SEND_USSD_COM)
                {
                    AT_TC(g_sw_AT_SAT, "SEND_USSD_COM");
                }
                else if(nCmdType == LAUNCH_BROWSER_C_COM)
                {
                    UINT16 nMem;
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER_C_COM");
                    CFW_SAT_BROWSER_RSP *pLB = (CFW_SAT_BROWSER_RSP *)CmdData;
                    if(pLB == NULL)
                        goto STGI_ERROR;

                    AT_TC(g_sw_AT_SAT, "pLB->URL Length =%d", pLB->nURLLength);
                    nMem = (pLB->nAlphaLength<< 1) + (pLB->nTextLength << 1) + (pLB->nURLLength<<1);

                    nMem += (pLB->nTextLength <<1) ;
                    UINT8 i =0;
                    for(i = 0; i< pLB->nProvisioningFileRefNum; i++)
                    {
                        nMem += ((pLB->pProvisioningFileRef[i])[0] << 1);
                    }

                    for(i =0; i< pLB->nURLLength; i++)
                        AT_TC(g_sw_AT_SAT, "%c-%x", pLB->pURL[i], pLB->pURL[i]);
                    nMem += 30;     //plus ^STGI: 21,"," ...
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER: nMem =%d",nMem);

                    pMessageBuf = (UINT8 *)AT_MALLOC(nMem);

                    AT_MemZero(pMessageBuf, nMem);

                    UINT8* q = pMessageBuf;
                    //Command ID, Qualifier
                    q +=AT_Sprintf(pMessageBuf,"^STGI: %d,%d,",pLB->nComID,pLB->nComQualifier);
                    //Browser Identity
                    if(pLB->nBrowserIdentity == 0)
                        q +=AT_Sprintf(q,"0,");     //Default browser
                    else
                        q +=AT_Sprintf(q,",");
                    //URL
                    if(pLB->nURLLength != 0)
                    {
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            AT_MemCpy(q,pLB->pURL,pLB->nURLLength);
                            q += pLB->nURLLength;
                        }
                        else
                        {
                            q +=SUL_hex2ascii(pLB->pURL, pLB->nURLLength, q);
                        }
                    }
                    //Gateway/Proxy
                    //scheme, gateway or proxy
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER: pLB->nTextLength =%d",pLB->nTextLength);
                    if(pLB->nTextLength != 0)
                    {
                        q +=AT_Sprintf(q,",%d,", pLB->nCodeScheme);
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            AT_MemCpy(q,pLB->nTextString, pLB->nTextLength);
                            q += pLB->nTextLength;
                        }
                        else
                        {
                            q +=SUL_hex2ascii(pLB->nTextString, pLB->nTextLength, q);
                        }
                        q +=AT_Sprintf(q,",");
                    }
                    else
                    {
                        q +=AT_Sprintf(q,",,,");
                    }
                    //alphaidentifier
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER: pLB->nAlphaLength =%d",pLB->nAlphaLength);
                    if(pLB->nAlphaLength != 0)
                    {
                        BOOL nDone = 0;
                        if(gATCurrentAlphabet == GSM_SET)
                        {
                            if((pLB->nAlphaIdentifier[0]&0x80) == 0)
                            {
                                AT_MemCpy(q,pLB->nAlphaIdentifier, pLB->nAlphaLength);
                                q += pLB->nAlphaLength;

                                nDone = 1;
                            }
                        }
                        else
                        {
                            if(pLB->nAlphaIdentifier[0] == 0x80)
                            {
                                UINT8* pStart = q;
                                q += unicode2gbk(q, pLB->nAlphaIdentifier + 1, pLB->nAlphaLength - 1);
                                if(pStart != q)
                                    nDone = 1;
                            }
                        }
                        if(nDone == 0)
                        {
                            AT_TC(g_sw_AT_SAT, "----------Hex---------");
                            q +=SUL_hex2ascii(pLB->nAlphaIdentifier, pLB->nAlphaLength, q);
                        }
                    }

                    //Provision File Reference
                    if(pLB->nProvisioningFileRefNum != 0)
                    {
                        q +=AT_Sprintf(q,",%d,",pLB->nProvisioningFileRefNum);
                        for(i = 0; i< pLB->nProvisioningFileRefNum; i++)
                        {
                            if(gATCurrentAlphabet == GSM_SET)
                            {
                                AT_MemCpy(q,pLB->pProvisioningFileRef[i]+1, pLB->pProvisioningFileRef[i][0]);
                                q +=pLB->pProvisioningFileRef[i][0];
                            }
                            else
                            {
                                q +=SUL_hex2ascii(pLB->pProvisioningFileRef[i]+1, pLB->pProvisioningFileRef[i][0], q);
                            }
                        }
                        q +=AT_Sprintf(q,",");
                    }
                    else
                    {
                        q +=AT_Sprintf(q,",0,,");
                    }

                    //IconQualifier
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER_C_COM: pLB->nIconQualifier =%x",pLB->nIconQualifier);
                    if(pLB->nIconQualifier != 0xFF)
                    {
                        q +=AT_Sprintf(q,"%d,",pLB->nIconQualifier);
                    }
                    else
                    {
                        q +=AT_Sprintf(q,",");
                    }

                    //IconIdentifier
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER_C_COM: pLB->nIconIdentifier =%x",pLB->nIconIdentifier);
                    if(pLB->nIconIdentifier != 0xFF)
                    {
                        q +=AT_Sprintf(q,"%d",pLB->nIconIdentifier);
                    }

                    AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pMessageBuf,pParam->nDLCI);
                    AT_TC(g_sw_AT_SAT, "LAUNCH_BROWSER_C_COM: nMem =%d",nMem);
                    AT_FREE(pMessageBuf);
                    return;
                }
                else
                    Len = 0;
            }
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,"",pParam->nDLCI);
            return;
        }
        case AT_CMD_TEST:
        {
            UINT8 nString[] = "^STGI: (16,19,33,35,36,37,38,211)";
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        break;
        case AT_CMD_READ:
        {
            UINT8 nString[50];
            UINT8 nCmd = CFW_SatGetCurCMD(nSim);
            if (nCmd == 0xFF)
                goto STGI_ERROR;
            AT_Sprintf(nString, "^STGI: %u", nCmd);
            AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,nString,pParam->nDLCI);
            return;
        }
        break;
        default:
            goto STGI_ERROR;
        }
    }

STGI_ERROR:
    AT_SAT_Result_Err(uErrorCode, pParam->nDLCI);
    return;
}

UINT32 AT_SAT_Count(CFW_SAT_ITEM_LIST *pMenu)
{
    CFW_SAT_ITEM_LIST *p = pMenu;
    UINT16 nMemSum = 0;
    UINT16 i       = 0;

    while (p != NULL)
    {
        nMemSum += MEM_ALIGN_SIZE(p->nItemLen - 1);
        i++;
        p = p->pNextItem;
    }
    AT_TC(g_sw_AT_SAT, "Item number = %d", i);
    nMemSum = (nMemSum << 1);

    return (i << 16) | nMemSum;
}

void AT_SAT_CmdFunc_STF(AT_CMD_PARA *pParam)
{
    UINT32 uErrorCode = ERR_AT_CME_PARAM_INVALID;
    if (pParam == NULL)
    {
        AT_SAT_Result_Err(uErrorCode, 0);
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        UINT8 paraCount = 0x00;
        UINT8 nMode = 0x00;
        UINT16 nSize = 0x01;

        INT32 iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if ((iRet != ERR_SUCCESS) || (paraCount != 1))
        {
            AT_TC(g_sw_GPRS, "AT+STF:paraCount: %d", paraCount);
            AT_SAT_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        iRet     = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nMode, &nSize);
        if (iRet != ERR_SUCCESS)
        {
            AT_TC(g_sw_GPRS, "AT+STF:iRet : %d !", iRet);
            AT_SAT_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        if(nMode > 2)
        {
            AT_TC(g_sw_GPRS, "AT+STF:nService : %d !", nMode);
            AT_SAT_Result_Err(ERR_AT_CME_PARAM_INVALID, pParam->nDLCI);
            return;
        }

        CFW_SetSATIndFormate(nMode);
        //CFW_SetSATIndFormate(1);
        UINT8* pResp = NULL;
        if(0x00 == nMode)
            pResp = "Set STF: PDU Mode";
        else if(0x01 == nMode)
            pResp = "Set STF: TEXT Mode";
        else if(0x02 == nMode)
            pResp = "Set STF: UCS2 Mode";
        AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pResp,pParam->nDLCI);
        return;
    }
    break;
    case AT_CMD_TEST:
    {
        UINT8* pResp = "^STF: (0,1)";
        AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pResp,pParam->nDLCI);
        return;
    }
    break;
    case AT_CMD_READ:
    {
        UINT8* pResp = NULL;
        if(0x00 == CFW_GetSATIndFormate())
            pResp = "^STF: PDU mode";
        else
            pResp = "^STF: TEXT mode";
        AT_SAT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK,pResp,pParam->nDLCI);
        return;
    }
    break;
    default:
    {
        AT_SAT_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, pParam->nDLCI);
        return;
    }
    }
    return;
}


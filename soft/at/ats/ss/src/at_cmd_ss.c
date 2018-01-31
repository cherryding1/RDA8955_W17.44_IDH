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
#include "ctype.h"
#include "at_cmd_ss.h"
#include "At_ss_def.h"
#include "At_ss_api.h"
#include "at_cfg.h"
#include "at_define.h"

#define AT_SS_DEBUG
UINT8 g_SsCaocStatus = 0;
UINT32 g_uiCurrentCmdStampSs     = 0;
UINT8 g_ucAtSsModeCSVMOrCCFCRead = 0;

const UINT8 *strTCCWA = "+CCWA:(0,1)";  // "+CCWA:(0-1),(0-2),(1,2,4)";
const UINT8 *strTCCFC = "+CCFC:(0,1,2,3,4,5)";  // "+CCFC:(0-5),(0-4),(NUMBER),(128,129,145,161,208),(5-30)";
const UINT8 *strTCLIP = "+CLIP:(0,1)";
const UINT8 *strTCLIR = "+CLIR:(0,1,2)";
const UINT8 *strTCOLP = "+COLP:(0,1)";
const UINT8 *strTCOLR = "+COLR:(0,2)";
const UINT8 *strTCUSD = "+CUSD:(0,1,2)";
const UINT8 *strTCSSN = "+CSSN:(0,1),(0,1)";
const UINT8 *strTCSVM = "+CSVM:(0,1),(129,145)";

const UINT8 *strFCCWA = ""; // "+CCWA=[<n>[,<mode>[,<class>]]]\r\n";

const UINT8 *strRCSSN = "+CSSN:%d,%d";  // hameina2007.12.07 [mod] :delete the last sp.
const UINT8 *strRCAMM = "+CAMM:%s ";
const UINT8 *strRCAOC = "+CAOC:%d";
const UINT8 *strRCPUC = "+CPUC:%s,%s";
const UINT8 *strRCUSD = "+CUSD:%d";
const UINT8 *strRCOLP = "+COLP:%d";

UINT8 g_ss_ussdVer     = 2;

extern VOID AT_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                         UINT8 nDLCI, UINT8 nSim);
extern VOID AT_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
extern VOID AT_SMS_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer,
                             UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim);
extern BOOL SMS_PDUBCDToASCII(UINT8 *pBCD, UINT16 *pLen, UINT8 *pStr);
extern UINT32 CFW_SimSetPUCT(CFW_SIM_PUCT_INFO *pPUCT, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimGetPUCT(UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimSetACM(UINT32 iCurValue, UINT8 *pPIN2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimGetACM(UINT16 nUTI, CFW_SIM_ID nSimID);

struct ussd_record_state
{
    UINT8 ussd_valid;
    UINT8 ussd_num;
};

static struct ussd_record_state ussd_state = {0,0};
VOID AT_SS_SetUSSDNum(UINT8 num)
{
    AT_TC(g_sw_AT_SS, "%s num=%d", __FUNCTION__, num);
    ussd_state.ussd_valid = 1;
    ussd_state.ussd_num = num;
}

UINT8 AT_SS_GetUSSDState()
{
    AT_TC(g_sw_AT_SS, "%s state=%d", __FUNCTION__, ussd_state.ussd_valid);
    return ussd_state.ussd_valid;
}

UINT8 AT_SS_GetUSSDNum()
{
    AT_TC(g_sw_AT_SS, "%s num =%d", __FUNCTION__, ussd_state.ussd_num);
    return ussd_state.ussd_num;
}

VOID AT_SS_ResetUSSDState()
{
    AT_TC(g_sw_AT_SS, "%s ", __FUNCTION__);
    ussd_state.ussd_valid = 0;
    ussd_state.ussd_num = 0;
}

UINT32 SS_ReadAocFromSim(UINT8 ucFileType, UINT32 *uiRetAoc)
{
    return 1;
}

// *************************************
// discription: this function check the DCS type, 7bit, 8 bit or UCS2
// *****************************************************
// return value:  1:    7bit
// 2:    8 bit
// 4:    UCS2
// 0xff:   err
// ************************************************
UINT8 AT_CBS_CheckDCS(UINT8 uDCS)
{
    UINT8 ret   = 0xff;
    UINT8 uHigh = 0;
    UINT8 uLow  = 0;

    uHigh = uDCS >> 4;
    uLow  = uDCS & 0x0f;

    if (!uHigh) // 0
    {
        ret = 1;
    }
    else if (1 == uHigh)
    {
        if (uLow == 0)
            ret = 1;
        else if (uLow == 1)
            ret = 4;
    }
    else if (2 == uHigh || 3 == uHigh)
    {
        ret = 1;
    }
    else if ((uHigh >> 2) == 01)  // 01xx
    {
        if ((uLow >> 2) == 0)
            ret = 1;
        else if ((uLow >> 2) == 1)
            ret = 2;
        else if ((uLow >> 2) == 2)
            ret = 4;
    }
    else if (uHigh == 15)
    {
        if (((uLow >> 2) & 1) == 0)
            ret = 1;
        else if (((uLow >> 2) & 1) == 1)
            ret = 2;
    }

    return (ret);

}

UINT32 AT_SS_QUERYERR(UINT8 nType, UINT16 nErrCode)
{
    UINT32 uiErrCode = ERR_AT_OK;

    if (0xfa == nType || (0xfc == nType)) // Local Error /net work reject
    {
        switch (nErrCode)
        {

        case 10: // CFW_Error_SS_Service_Not_Implemented

        case 11: // CFW_Error_SS_Invalid_Operation

        case 12: // CFW_Error_SS_Invalid_Service_Code
            uiErrCode = ERR_AT_CME_OPERATION_NOT_ALLOWED;
            break;

        case 14: // CFW_Error_SS_Ctxt_Allocation
            uiErrCode = ERR_AT_CME_MEMORY_FAILURE;
            break;

        case 20: // CFW_Error_SS_MM_Establishment

        case 21: // CFW_Error_SS_TransactionTimeout

        case 22: // CFW_Error_SS_MMConnectionReleased

        case 23: // CFW_Error_SS_UnknownComponentReceived
            uiErrCode = ERR_AT_CME_PHONE_FAILURE;
            break;

        default:
            uiErrCode = ERR_AT_UNKNOWN;
            break;
        }
    }
    else if (0xfb == nType) // Nwk returned a return error
    {
        uiErrCode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
    }
    else if (0xfd == nType) // Nwk returned a Cause IE
    {
        uiErrCode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
    }
    else  // return event error
    {
        uiErrCode = ERR_AT_UNKNOWN;
    }

    return uiErrCode;

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: called by ATM,supplementary service initial
//
// Parameter:
// Caller:
// Called:
// Return:
// Remark:
//
// ////////////////////////////////////////////////////////////////////////////

UINT32 AT_SS_Init(VOID)
{

    // global variable initial

    return 0;

}
typedef  enum _AT_MSG_ALPHABET
{
    AT_MSG_GSM_7 = 0,
    AT_MSG_8_BIT_DATA,
    AT_MSG_UCS2,
    AT_MSG_RESERVED,
} AT_MSG_ALPHABET;

//Parse TP-DCS , more in "3G TS 23.038 V3.3.0"
void AT_USSDParseDCS(UINT8 nInputData, UINT8 *pAlphabet)
{
    //Parse TP-DCS , more in "3G TS 23.038 V3.3.0"
    *pAlphabet =  AT_MSG_RESERVED;

    if((nInputData & 0xf0) == 0x00)// bit(7-0): 0000 xxxx
    {
        *pAlphabet  = AT_MSG_GSM_7;
    }
    else if((nInputData & 0xf0) == 0x10)// bit(7-0): 0001 xxxx
    {
        if((nInputData & 0x0f) == 0x00) // bit(7-0): 0001 0000
            *pAlphabet  = AT_MSG_GSM_7;
        else if ((nInputData & 0x0f) == 0x01)// bit(7-0): 0001 0001
            *pAlphabet  = AT_MSG_UCS2;
        else
            *pAlphabet  = AT_MSG_RESERVED;

    }
    else if((nInputData & 0xc0) == 0x40)  //bit(7-0): 01xx xxxx
    {
        //Set pAlphabet
        if((nInputData & 0x0c) == 0x00)// bit(7-0): 01xx 00xx
        {
            *pAlphabet = AT_MSG_GSM_7;
        }
        else if((nInputData & 0x0c) == 0x04)// bit(7-0): 01xx 01xx
        {
            *pAlphabet = AT_MSG_8_BIT_DATA;
        }
        else if((nInputData & 0x0c) == 0x08)// bit(7-0): 01xx 10xx
        {
            *pAlphabet = AT_MSG_UCS2;
        }
        else if((nInputData & 0x0c) == 0x0c)// bit(7-0): 01xx 11xx
        {
            *pAlphabet = AT_MSG_RESERVED;
        }
    }
    else if((nInputData & 0xf0) == 0x90)  //bit(7-0): 1001 xxxx
    {
        //Set pAlphabet
        if((nInputData & 0x0c) == 0x00)// bit(7-0): 1001 00xx
        {
            *pAlphabet = AT_MSG_GSM_7;
        }
        else if((nInputData & 0x0c) == 0x04)// bit(7-0): 1001 01xx
        {
            *pAlphabet = AT_MSG_8_BIT_DATA;
        }
        else if((nInputData & 0x0c) == 0x08)// bit(7-0): 1001 10xx
        {
            *pAlphabet = AT_MSG_UCS2;
        }
        else if((nInputData & 0x0c) == 0x0c)// bit(7-0): 1001 11xx
        {
            *pAlphabet = AT_MSG_RESERVED;
        }
    }
    else if((nInputData & 0xf0) == 0xf0)  //bit(7-0): 1111 0xxx
    {
        if((nInputData & 0x04) == 0x04) // bit(7-0): 1111 01xx
            *pAlphabet  = AT_MSG_8_BIT_DATA;
        else// bit(7-0): 1111 00xx
            *pAlphabet  = AT_MSG_GSM_7;
    }


    if(*pAlphabet == AT_MSG_RESERVED)
    {
        *pAlphabet =  AT_MSG_GSM_7;
    }

}
#define AT_MAX_DIGITS_USSD          183

typedef struct
{
    UINT8 ussd_input[AT_MAX_DIGITS_USSD];
    BOOL b_used;
} AT_USSD_STRING;
AT_USSD_STRING at_ussdstring[NUMBER_OF_SIM];
/*==========================================================================
 * This function is used to process SS PROC_USS REQ to SSCode related REQ.
 *=========================================================================*/

U8 SS_AT_ReSendProcUssV1Req(UINT8 nUTI, CFW_SIM_ID nSIMID)
{
    UINT32 ret;
    UINT8 srcLen ;

    at_ussdstring[nSIMID].b_used = TRUE;

    AT_TC(g_sw_AT_SS, "USSD Func: %s ussd_input:%s", __FUNCTION__, at_ussdstring[nSIMID].ussd_input);
    srcLen = strlen(at_ussdstring[nSIMID].ussd_input);

    ret = CFW_SsSendUSSD(at_ussdstring[nSIMID].ussd_input, srcLen, 131, 0x0F, nUTI, nSIMID);
    if (ret != ERR_SUCCESS)
    {
        AT_TC(g_sw_AT_SS,  "SS_ReSendProcUssV1Req()   CFW_CmSendUSSD  ret = %d", ret);
        return ERR_AT_CME_PHONE_FAILURE;
    }
    /*Must add code here*/
    return ERR_AT_CME_PHONE_FAILURE;
}


extern at_chset_t cfg_GetTeChset(void);

VOID _GenerateCUSData(UINT8 *pUSSDString, UINT8 nUSSDLen, UINT8 *pDestUSSDString, UINT8 nUSSDDCS)
{

    AT_TC(g_sw_AT_SS, "_GenerateCUSData nUSSDDCS %d, nUSSDLen: %d\n", nUSSDDCS, nUSSDLen);

    if (nUSSDDCS == AT_MSG_UCS2)
    {
        UINT8  *in  = NULL;
        UINT8   nSwitchData = 0x00;
        UINT8   i = 0x00;

        in = pUSSDString;

        for (i = 0x00; i < nUSSDLen; i++)
        {
            nSwitchData = (in[i] >> 4) + (in[i] << 4);
            in[i]       = nSwitchData;
        }

    }
    if( cs_hex == cfg_GetTeChset())
    {
        AT_TC(g_sw_AT_SS, "_GenerateCUSData cs_hex == cfg_GetTeChset()\n");

        if(nUSSDDCS == AT_MSG_GSM_7 || nUSSDDCS == AT_MSG_8_BIT_DATA)
        {
            UINT8  *in  = NULL;
            UINT8   nSwitchData = 0x00;
            UINT8   i = 0x00;

            in = pUSSDString;

            for (i = 0x00; i < nUSSDLen; i++)
            {
                nSwitchData = (in[i] >> 4) + (in[i] << 4);
                in[i]       = nSwitchData;
            }
        }
        SUL_GsmBcdToAsciiEx(pUSSDString, nUSSDLen, pDestUSSDString);

    }
    else
    {
        AT_TC(g_sw_AT_SS, "_GenerateCUSData TE Chset not HEX\n");
        SUL_MemCopy8(pDestUSSDString, pUSSDString, nUSSDLen);
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: called by ATM,supplementary service asynchronous event process
//
// Parameter:   pstOsEvent,os event structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      same respond for different function,from flag to process
//
// ////////////////////////////////////////////////////////////////////////////
extern BOOL gATSATSendUSSDFlag[NUMBER_OF_SIM] ;
VOID AT_SS_AsyncEventProcess(COS_EVENT *pstOsEvent)
{

    AT_CMD_RESULT **ppstResult = NULL;
    CFW_EVENT stCfwEvent;

    UINT8 ucBit                          = 0;
    UINT8 aucBuffer[MAX_LENGTH_BUFF]     = { 0 }; // 256
    UINT8 aucBufferTemp[MAX_LENGTH_BUFF] = { 0 };
    UINT32 uiErrCode                     = ERR_AT_OK;
    //UINT8 i;

    // parameter check
    AT_CosEvent2CfwEvent(pstOsEvent, &stCfwEvent);

    UINT8 nSim = stCfwEvent.nFlag;
    stCfwEvent.nUTI = AT_ASYN_GET_DLCI(nSim);

    if (NULL == pstOsEvent)
    {
        AT_CC_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
        return;
    }

#ifdef AT_SS_DEBUG
    AT_TC(g_sw_AT_SS, "Event structure:EventId: %d Para1: %d Para2: %d Type: %d Flag: %d UTI: %d\n",
          stCfwEvent.nEventId, stCfwEvent.nParam1, stCfwEvent.nParam2, stCfwEvent.nType, stCfwEvent.nFlag,
          stCfwEvent.nUTI);

#endif
    // receive event from CSW and process

    switch (stCfwEvent.nEventId)
    {
    case EV_CFW_SS_QUERY_FACILITY_LOCK_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CLCK", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            if (0 == stCfwEvent.nType)
            {
                //when <mode>=2 and command successful:
                //+CLCK: <status>[,<class1>[<CR><LF>+CLCK: <status>,<class2>

                AT_Sprintf(aucBufferTemp, "+CLCK:%d,%d", LOUINT8(stCfwEvent.nParam2), HIUINT8(stCfwEvent.nParam2));
                AT_StrCat(aucBuffer, aucBufferTemp);
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
                pstOsEvent->nParam1 = 0;

                return;
            }
            else if (0xF0 == stCfwEvent.nType)
            {
                uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

                if (ERR_AT_OK != uiErrCode)
                {
                    AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
                    pstOsEvent->nParam1 = 0;
                    return;
                }
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
                pstOsEvent->nParam1 = 0;
                return;
            }
        }

    }
    break;
    case EV_CFW_SS_SET_FACILITY_LOCK_RSP:
    {
        pstOsEvent->nParam1 = 0;
        if (AT_IsAsynCmdAvailable("+CLCK", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {

            if (0 == stCfwEvent.nType)
            {
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, stCfwEvent.nUTI, nSim);
            }
            else if (0xF0 == stCfwEvent.nType)
            {
                uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);
                if (ERR_AT_OK != uiErrCode)
                {
                    AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
                    return;
                }
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
                return;

            }
        }
    }
    break;
    case EV_CFW_SS_SET_CALL_WAITING_RSP: // EV_CSW_SS_SET_CW_RSP:
    {
        pstOsEvent->nParam1 = 0;
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CCWA", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            // OK return
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, stCfwEvent.nUTI, nSim);
        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }
    }

    break;

    // CCWA read response event +CCWA: <n>

    case EV_CFW_SS_QUERY_CALL_WAITING_RSP: // EV_CSW_SS_QUERY_CW_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CCWA", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success  VOICE DATA FAX SMS ...
        {
            UINT8 usTmpVal = 1;
            UINT8 ucTemp   = 0;

            // parmeter2 is Class

            for (ucTemp = 0; ucTemp < 3; ucTemp++)
            {
                // Bit1 is 0 or 1
                if (AT_IS_BIT_SET(stCfwEvent.nParam1, ucTemp))  // 1 voice is active
                {
                    ucBit = 1;
                }
                else
                {
                    ucBit = 0;
                }

                // parmeter2 is Class
                AT_Sprintf(aucBufferTemp, "+CCWA:%u,%u\r\n", ucBit, usTmpVal);
                AT_StrCat(aucBuffer, aucBufferTemp);
                usTmpVal = usTmpVal * 2; // equal 2 ^  n

            }
            pstOsEvent->nParam1 = 0;

            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);
            pstOsEvent->nParam1 = 0;

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }
    }

    break;

    // CCFC,set response event

    case EV_CFW_SS_SET_CALL_FORWARDING_RSP:  // EV_CSW_SS_SET_CF_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CCFC", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, stCfwEvent.nUTI, nSim);
            pstOsEvent->nParam1 = 0;
        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);
            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }

    }

    break;

    // CCFC, read response event

    case EV_CFW_SS_QUERY_CALL_FORWARDING_RSP:  // EV_CSW_SS_QUERY_CF_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CCFC", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            CFW_SS_QUERY_CALLFORWARDING_RSP *pstSsQueryCallForwardingRsp = NULL;
            UINT8 aucAsciiNumber[43] = { 0 }; // 43
            UINT8 ucDestClass        = 0;
            UINT8 aucStatus          = 0;
            UINT8 aucType            = 0;

            pstSsQueryCallForwardingRsp = (CFW_SS_QUERY_CALLFORWARDING_RSP *)(stCfwEvent.nParam1);

            // convert class software and 3gpp
            AT_SsClassConvert2Ccfc(pstSsQueryCallForwardingRsp->nClass, &ucDestClass);
            aucType   = pstSsQueryCallForwardingRsp->nNumber.nType;
            aucStatus = pstSsQueryCallForwardingRsp->nStatus;

            // paucAsciiNumber[ucDestClass] = aucAsciiNumber;
            // set status of current class is 1
            // [[Changyg[mod] 2008-04-22 :add "+" when acuType is 145

            if (145 == aucType)
            {
                aucAsciiNumber[0] = 0x2b;
                SUL_GsmBcdToAscii(pstSsQueryCallForwardingRsp->nNumber.nTelNumber, pstSsQueryCallForwardingRsp->nNumber.nSize,
                                  &aucAsciiNumber[1]);
            }
            else
            {
                SUL_GsmBcdToAscii(pstSsQueryCallForwardingRsp->nNumber.nTelNumber, pstSsQueryCallForwardingRsp->nNumber.nSize,
                                  aucAsciiNumber);
            }

            // ]]Changyg[mod] 2008-04-22 :end

            // CCFC respond
            // parameter2 is size of structure, not The counter of structure array
            if (pstSsQueryCallForwardingRsp->nStatus)
            {
                if (!pstSsQueryCallForwardingRsp->nTime)
                {
                    // number exist,display all information
                    AT_Sprintf(aucBufferTemp, "+CCFC:%u,%u,\"%s\",%u", aucStatus, ucDestClass, aucAsciiNumber, aucType);
                }
                else
                {
                    AT_Sprintf(aucBufferTemp, "+CCFC:%u,%u,\"%s\",%u,,,%u", aucStatus, ucDestClass, aucAsciiNumber, aucType,
                               pstSsQueryCallForwardingRsp->nTime);
                }

            }
            else
            {
                AT_Sprintf(aucBufferTemp, "+CCFC:%u,%u", aucStatus, ucDestClass);
            }

            // OK retrun
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBufferTemp, AT_StrLen(aucBufferTemp), stCfwEvent.nUTI, nSim);
        }
        else
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
            pstOsEvent->nParam1 = 0;
        }
    }

    break;

    // CLIP read response event

    case EV_CFW_SS_QUERY_CLIP_RSP: // EV_CSW_SS_QUERY_CLIP_RSP: +CLIP: <n>,<m>
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CLIP", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            // parameter1 parsing 1 indicate presentation of the CLIP; 0 don't presentation of the CLIP. error
            // parameter2 parsing 0 CLIP not provisioned 1 CLIP provisioned 2 unknown (e.g. no network, etc.)
            AT_Sprintf(aucBufferTemp, "+CLIP:%u,%u", gATCurrentucClip, (UINT8)stCfwEvent.nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBufferTemp, AT_StrLen(aucBufferTemp), stCfwEvent.nUTI, nSim);
        }
        else
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;

    }

    break;

    // CLIR read response event

    case EV_CFW_SS_QUERY_CLIR_RSP: // EV_CSW_SS_QUERY_CLIR_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CLIR", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            // parameter1 parsing
            // 0 Presentation indicator is used according to the subscription of the CLIR service
            // 1 CLIR invocation. Activate CLIR. Disable presentation of own phone number to called party
            // 2 CLIR suppression. Suppress CLIR. Enable presentation of own phone number to called party.

            // parameter2 parsing
            // 0 CLIR not provisioned
            // 1 CLIR provisioned in permanent mode
            // 2 unknown (e.g. no network, etc.)
            // 3 temporary mode presentation restricted
            // 4 CLIR temporary mode presentation allowed
            AT_Sprintf(aucBuffer, "+CLIR:%u,%u", gATCurrentucClir, (UINT8)stCfwEvent.nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;
    }

    break;

    // CLOR read response event

    case EV_CFW_SS_QUERY_COLR_RSP: // EV_CSW_SS_QUERY_COLR_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+CLOR", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            // parameter1 parsing
            // 0 means don't presentation of the COLR.
            // 1 indicate presentation of the COLR

            // parameter2 parsing
            // 0 COLR not active
            // 1 COLR active
            // 2 unknown (e.g. no network, etc.)

            AT_Sprintf(aucBuffer, "+COLR:%u,%u", (UINT8)stCfwEvent.nParam1, (UINT8)stCfwEvent.nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;
    }

    break;

    // COLP read response event

    case EV_CFW_SS_QUERY_COLP_RSP: // EV_CSW_SS_QUERY_COLP_RSP:
    {
        // check command life cycle
        if (!AT_IsAsynCmdAvailable("+COLP", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            return;
        }

        if (0 == stCfwEvent.nType)  // success
        {
            // parameter1 parsing
            // 0 Disable  to show the COLP
            // 1 Enable  to show the COLP

            // parameter2 parsing
            // 0 COLP not provisioned
            // 1 COLP provisioned
            // 2 unknown (e.g. no network, etc.)
            AT_Sprintf(aucBuffer, "+COLP:%u,%u", gATCurrentucColp, (UINT8)stCfwEvent.nParam2);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            uiErrCode = AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);

            // ERROR return

            if (ERR_AT_OK != uiErrCode)
            {
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }
        }
        pstOsEvent->nParam1 = 0;

    }

    break;

    // CUSD set response event
    case EV_CFW_SS_USSD_IND:  // EV_CFW_SS_USSD_IND:
    {
        CFW_SS_USSD_IND_INFO *pUSSDInd = NULL;
        UINT8     nUSSDDCS    = 0x00;
        UINT8     aucDestUsd[2 * MAX_LENGTH_STRING_USSD + 2] = { 0 };

        pUSSDInd = (CFW_SS_USSD_IND_INFO *)stCfwEvent.nParam1;

        AT_USSDParseDCS(pUSSDInd->nDcs, &nUSSDDCS);
        AT_TC(g_sw_AT_SS, "pUSSDInd->nDcs=%d, nUSSDDCS: %d\n", pUSSDInd->nDcs, nUSSDDCS);
        _GenerateCUSData(pUSSDInd->pUsdString, pUSSDInd->nStingSize, aucDestUsd, nUSSDDCS);

        AT_SS_SetUSSDNum(HIUINT16(pstOsEvent->nParam3));
        AT_TC(g_sw_AT_SS, "EV_CFW_SS_USSD_IND strlen(aucDestUsd)=%d, aucDestUsd: %s\n", strlen(aucDestUsd), aucDestUsd);
        AT_Sprintf(aucBuffer, "+CUSD: %u, \"%s\" ,%u", pUSSDInd->nOption, aucDestUsd, pUSSDInd->nDcs);
        AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
        break;
    }

    case EV_CFW_SS_SEND_USSD_RSP:
    {
        UINT8 nUSSDstr[256] = {0x00,};
        UINT8 nUSSDlen = 0x00;
        if ( 0x00 == stCfwEvent.nType )  // success
        {
            CFW_SS_USSD_IND_INFO *pstSsUssdIndInfo = NULL;
            UINT8 aucDestUsd[2 * MAX_LENGTH_STRING_USSD + 2] = { 0 }; // 140
            UINT8 nUSSDDCS                                    = 0xff;

            if (AT_isWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, stCfwEvent.nUTI))
            {
                AT_DelWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, stCfwEvent.nUTI);
                AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, stCfwEvent.nUTI, nSim);
            }
            if (0 == stCfwEvent.nParam1)
            {
                AT_SS_ResetUSSDState();
                if( 0x00 != gATSATSendUSSDFlag[nSim] )
                {
                    AT_TC(g_sw_AT_SS, "ATSendUSSD:gATSATSendUSSDFlag[nSim]%d\n", gATSATSendUSSDFlag[nSim]);
                    CFW_SatResponse(0x12, 0x00, 0x00, 0x00, 0x00, stCfwEvent.nUTI, nSim);
                    gATSATSendUSSDFlag[nSim] = FALSE;
                }
                return;
            }

            pstSsUssdIndInfo = (CFW_SS_USSD_IND_INFO *)stCfwEvent.nParam1;
            if (pstSsUssdIndInfo->nOption != 0 && pstSsUssdIndInfo->nOption != 1)
            {
                AT_SS_SetUSSDNum(HIUINT16(pstOsEvent->nParam3));
            }
            if (gATCurrentss_ussd && stCfwEvent.nParam1)
            {
                if (pstSsUssdIndInfo->nStingSize)
                {
                    AT_MemZero(nUSSDstr, 256);
                    if(pstSsUssdIndInfo != NULL && pstSsUssdIndInfo->pUsdString != NULL && pstSsUssdIndInfo->nStingSize != 0)
                    {
                        nUSSDstr[0] = pstSsUssdIndInfo->nDcs;
                        AT_MemCpy(&nUSSDstr[1], pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize);
                        nUSSDlen = 1 + pstSsUssdIndInfo->nStingSize;
                    }
                    //nUSSDDCS = AT_CBS_CheckDCS(pstSsUssdIndInfo->nDcs);
                    AT_USSDParseDCS(pstSsUssdIndInfo->nDcs, &nUSSDDCS);
                    nUSSDstr[0] = nUSSDDCS;
                    AT_TC(g_sw_AT_SS, "pstSsUssdIndInfo->nDcs=%d, nUSSDDCS: %d\n", pstSsUssdIndInfo->nDcs, nUSSDDCS);

                    _GenerateCUSData(pstSsUssdIndInfo->pUsdString, pstSsUssdIndInfo->nStingSize, aucDestUsd, nUSSDDCS);
                    AT_TC(g_sw_AT_SS, "EV_CFW_SS_SEND_USSD_RSP aucDestUsd: %s, strlen(aucDestUsd)=%d\n", aucDestUsd, strlen(aucDestUsd));
                    AT_Sprintf(aucBuffer, "+CUSD: %u, \"%s\" ,%u", pstSsUssdIndInfo->nOption, aucDestUsd,
                               pstSsUssdIndInfo->nDcs);
                }
                else
                {
                    AT_Sprintf(aucBuffer, "+CUSD: %u", pstSsUssdIndInfo->nOption);
                }

                AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_CR, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
            }

        }
        else  // if (0xfa == stCfwEvent.nType || (0xfc == stCfwEvent.nType)) //  Local Error /net work reject
        {
            AT_TC(g_sw_AT_SS,  "SS func: %s nType = %x", __FUNCTION__, stCfwEvent.nType );
            if (AT_isWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, stCfwEvent.nUTI))
            {
            	   if(stCfwEvent.nParam2 == 0x24)
            	   {
			AT_DelWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, stCfwEvent.nUTI);
			AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, stCfwEvent.nUTI, nSim);
			return;
		   }
                AT_DelWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, stCfwEvent.nUTI);
                AT_CC_Result_Err(uiErrCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            }

            if( 0xFC == stCfwEvent.nType )
            {
                SS_AT_ReSendProcUssV1Req(stCfwEvent.nUTI, nSim);
                pstOsEvent->nParam1 = 0;
                return;
            }
            else
            {
                AT_SS_ResetUSSDState();
                AT_SS_QUERYERR(stCfwEvent.nType, stCfwEvent.nParam2 & 0xffff);
            }
        }

        if( 0x00 != gATSATSendUSSDFlag[nSim] )
        {
            if (0 == stCfwEvent.nType)
            {
                CFW_SS_USSD_IND_INFO *pstSsUssdIndInfo = NULL;
                pstSsUssdIndInfo = (CFW_SS_USSD_IND_INFO *)stCfwEvent.nParam1;

                if(( NULL != pstSsUssdIndInfo) && ( NULL != pstSsUssdIndInfo->pUsdString ))
                {
                    AT_TC_MEMBLOCK(g_sw_AT_SS, nUSSDstr, nUSSDlen, 16);
                }

                AT_TC(g_sw_AT_SS, "ATSendUSSD:nDcs=%d, nOption: %d, nStingSize:%d, nParam2:%d\n", pstSsUssdIndInfo->nDcs, pstSsUssdIndInfo->nOption, pstSsUssdIndInfo->nStingSize, stCfwEvent.nParam2);

                if( 0x02 == pstSsUssdIndInfo->nOption )
                {
                    UINT32 nRet = CFW_SatResponse(0x12, 0x00, 0x00, nUSSDstr, nUSSDlen, stCfwEvent.nUTI, nSim);
                    AT_TC(g_sw_AT_SS, "CFW_SatResponse:CFW_SatResponse nOption =2 release complete nRet:%x\n", nRet);
                }
            }
            else
            {
            }

            gATSATSendUSSDFlag[nSim] = FALSE;
        }
    }
    break;

    /*
    // get ACM response
    case EV_CFW_SIM_GET_ACM_RSP:
    {
    // check command life cycle
    if (!AT_IsAsynCmdAvailable("+CACM", g_uiCurrentCmdStampSs))
    {
    return;
    }

    //AT_CmdCallback_CLIP_QUERY(uiEventId, pBody);
    if (0 == stCfwEvent.nType) // success
    {
    // parameter1 is vaule
    // parameter2 0
    AT_Sprintf(aucBuffer, "+CACM:%u", stCfwEvent.nParam1);

    // OK retrun
    AT_SS_EVENT_TRUE_RETURN(ppstResult, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI);

    }
    else if (0xf0 == stCfwEvent.nType) //  Local Error
    {
    // parameter1 is error code,parameter2 is 0
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else if (0xfb == stCfwEvent.nType) // Nwk returned a return error
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else if (0xfc == stCfwEvent.nType) // Network returned a reject component
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else if (0xfd == stCfwEvent.nType) // Nwk returned a Cause IE
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else // return event error
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }

    // ERROR return
    if (FAIL == ucRetFlag)
    {
    uiErrCode = stCfwEvent.nParam2;
    AT_SS_EVENT_FAIL_RETURN(ppstResult, uiErrCode, NULL, 0, stCfwEvent.nUTI);
    }
    }
    break;
    */
    // get PUC response

    case EV_CFW_SIM_GET_PUCT_RSP:
    {
        AT_TC(g_sw_AT_SS, "EV_CFW_SIM_GET_PUCT_RSP-COME\n");

        // check command life cycle

        if (!AT_IsAsynCmdAvailable("+CPUC", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            AT_TC(g_sw_AT_SS, "EV_CFW_SIM_GET_PUCT_RSP -----error---AT_IsAsynCmdAvailable");
            return;
        }

        AT_TC(g_sw_AT_SS, "EV_CFW_SIM_GET_PUCT_RSP-AT_IsAsynCmdAvailable SUCCESS\n");

        if (0 == stCfwEvent.nType)  // success
        {
            CFW_SIM_PUCT_INFO *pstPucInfo = (CFW_SIM_PUCT_INFO *)stCfwEvent.nParam1;

            AT_TC(g_sw_AT_SS, "EV_CFW_SIM_GET_PUCT_RSP-Currency=%c%c%c\n", \
                  pstPucInfo->iCurrency[0], pstPucInfo->iCurrency[1], pstPucInfo->iCurrency[2]);
            AT_TC(g_sw_AT_SS, "EV_CFW_SIM_GET_PUCT_RSP-iEPPU=%x,EX=%x,sign = %x\n", pstPucInfo->iEPPU,pstPucInfo->iEX,pstPucInfo->iSign);

            UINT8 currency[4];
            AT_MemZero(currency,4);
            AT_MemCpy(currency, pstPucInfo->iCurrency, sizeof(pstPucInfo->iCurrency));

            // get ppu
            UINT8 aucPPU[MAX_LENGTH_PPU_CPUC];

            AT_MemZero(aucPPU, sizeof(aucPPU));
            if(! SS_PUCT2String(aucPPU, pstPucInfo))
            {
                AT_TC(g_sw_AT_SS, "SS_PPUIntStructureToFloatStr error-\n\r");
                AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
                return;
            }

            AT_Sprintf(aucBuffer, "+CPUC:\"%s\",\"%s\"", currency, aucPPU);

            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), stCfwEvent.nUTI, nSim);
            return;

        }
        else if (0xf0 == stCfwEvent.nType)  // Local Error
        {
            AT_TC(g_sw_AT_SS, "EV_CFW_SIM_GET_PUCT_RSP error-\n\r");
            UINT32 nErrorCode = AT_SetCmeErrorCode(stCfwEvent.nParam1, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            pstOsEvent->nParam1 = 0;
            return;
        }
        else  // return event error
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            return;
        }
    }

    break;

    /*
    // set ACM response
    case EV_CFW_SIM_SET_ACM_RSP:
    {
    // check command life cycle
    if (!AT_IsAsynCmdAvailable("+CACM", g_uiCurrentCmdStampSs))
    {
    return;
    }

    // set success
    if (0 == stCfwEvent.nType) // success
    {
    // parameter1 1 or 2(this means need pin1 or pin2)
    // parameter2 0

    // OK retrun
    AT_SS_EVENT_TRUE_RETURN(ppstResult, NULL, 0, stCfwEvent.nUTI);
    }
    else if (0xf0 == stCfwEvent.nType) //  Local Error
    {
    // // parameter1 is error code,parameter2 is 0
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else if (0xfb == stCfwEvent.nType) // Nwk returned a return error
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else if (0xfc == stCfwEvent.nType) // Network returned a reject component
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else if (0xfd == stCfwEvent.nType) // Nwk returned a Cause IE
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }
    else // return event error
    {
    AT_Sprintf(aucBuffer, "err");
    ucRetFlag = FAIL;
    }

    // ERROR return
    if (FAIL == ucRetFlag)
    {
    uiErrCode = stCfwEvent.nParam2;
    AT_SS_EVENT_FAIL_RETURN(ppstResult, uiErrCode, NULL, 0, stCfwEvent.nUTI);
    }
    }
    break;
    */
    // set PUC response

    case EV_CFW_SIM_SET_PUCT_RSP:
    {
        // check command life cycle
        AT_TC(g_sw_AT_SS, "EV_CFW_SIM_SET_PUCT_RSP-COME\n");

        if (!AT_IsAsynCmdAvailable("+CPUC", g_uiCurrentCmdStampSs, stCfwEvent.nUTI))
        {
            AT_TC(g_sw_AT_SS, "EV_CFW_SIM_SET_PUCT_RSP -----error---AT_IsAsynCmdAvailable");
            return;
        }

        AT_TC(g_sw_AT_SS, "EV_CFW_SIM_SET_PUCT_RSP-COME --AT_IsAsynCmdAvailable  SUCCESS\n");

        if (0 == stCfwEvent.nType)  // success
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, stCfwEvent.nUTI, nSim);
            return;
        }
        else if (0xf0 == stCfwEvent.nType)  // Local Error
        {
            AT_TC(g_sw_AT_SS, "EV_CFW_SIM_SET_PUCT_RSP error-\n\r");
            UINT32 nErrorCode = AT_SetCmeErrorCode(stCfwEvent.nParam1, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            pstOsEvent->nParam1 = 0;
            return;
        }
        else  // return event error
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, stCfwEvent.nUTI, nSim);
            return;
        }
    }

    break;

    case EV_CFW_SIM_SET_ACM_RSP: // [+]for csw update AT+CACM 2007.11.19

        AT_TC(g_sw_AT_SS, "<<<<<===== AT SS Get Async Event:  EV_CFW_SIM_SET_ACM_RSP\n ");
        AT_SS_SetACM_rsp(pstOsEvent);
        AT_ZERO_PARAM1(pstOsEvent);
        break;

    case EV_CFW_SIM_GET_ACM_RSP: // [+]for csw update AT+CACM 2007.11.19

        AT_TC(g_sw_AT_SS, "<<<<<===== AT SS Get Async Event:  EV_CFW_SIM_GET_ACM_RSP\n ");
        AT_SS_GetACM_rsp(pstOsEvent);
        break;

    default:
        uiErrCode = ERR_AT_CMS_UNKNOWN_ERROR;

        AT_SS_EVENT_FAIL_RETURN(ppstResult, uiErrCode, NULL, 0, stCfwEvent.nUTI);

        break;
    }

    return;

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +CCWA=[<n>[,<mode>[,<class>]]]
// Call waiting
// AT+CCWA=1,1,1
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      query mode,only for single class,not for all class
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CCWA(AT_CMD_PARA *pParam)
{
    // AT_CMD_RESULT    **ppstResult = NULL;

    UINT16 ucLen;
    UINT8 ucParaCount = 0;

    // UINT8            ucMode;
    UINT8 *pucParam = (UINT8 *)(pParam->pPara);

    // UINT8            ucClass;
    // UINT32           uiServiceId = CFW_SS_SRV_ID;
    UINT32 uiRetVal;
    INT32 iRetVal = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
#ifdef AT_SS_DEBUG
    // output debug information
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

#endif

    // parameter check

    if (NULL == pParam)
    {
        // return parameter error
        AT_CC_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // call ATM function
    // at command type judge
    switch (pParam->iType)
    {

    // extend command (set)

    case AT_CMD_SET:
    {
        // UINT8  ucNValue;
        UINT8 ucN     = 0, ucMode = 0;
        UINT8 usClass = 0;

        // UINT8  ucPara1Flg = EXIST, ucPara2Flg = EXIST, ucPara3Flg = EXIST;

        iRetVal = AT_Util_GetParaCount(pucParam, &ucParaCount);

        // result check

        if (ERR_SUCCESS != iRetVal || ucParaCount > 3)
        {
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        // first parameter
        if (ucParaCount > 0)
        {
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucN, &ucLen);

            if (ERR_SUCCESS != iRetVal || ucN > 1)
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }
        }

        // second parameter
        if (ucParaCount > 1)
        {
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 1, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucMode, &ucLen);
            if (ERR_SUCCESS != iRetVal || ucMode > 2)
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

        }

        // third parameter
        if (ucParaCount == 3)
        {
            UINT8 tmp = 0;

            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 2, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&tmp, &ucLen);

            if (ERR_SUCCESS != iRetVal)
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

            // [[hameina[mod] 2007.11.26, for fix bug 7100
            if (tmp & 1)
                AT_SsClassConvertCcfc(tmp, &usClass);
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

            // ]]hameina[mod] 2007.11.26, for fix bug 7100
        }
        else
            usClass = 11;

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "Para count:%d ucN 1: %d  ucMode 2: %d  usClass 3: %d\n", ucParaCount, ucN, ucMode, usClass);

#endif
        // set URC control value
        gATCurrentucSsCallWaitingPresentResultCode = ucN;

        if (ucParaCount == 1)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        AT_TC(g_sw_AT_SS, "1111\n");

        if (ucMode == 2)
        {
            uiRetVal = CFW_SsQueryCallWaiting(usClass, pParam->nDLCI, nSim);
            AT_TC(g_sw_AT_SS, "222uiRetVal=0x%x\n", uiRetVal);
            if (uiRetVal == ERR_SUCCESS)
            {
                AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }
        else
        {

            uiRetVal = CFW_SsSetCallWaiting(ucMode, usClass, pParam->nDLCI, nSim);
            if (uiRetVal == ERR_SUCCESS)
            {
                AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
            else
            {
                AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
        }

    }

    break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "+CCWA:<0-1>,<0-2>,<1,2,4>"
        // AT_SS_SYN_RETURN(ppstResult, strTCCWA, AT_StrLen(strTCCWA), pParam->nDLCI);
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCCWA, AT_StrLen(strTCCWA), pParam->nDLCI, nSim);
        break;
    case AT_CMD_READ:
    {
        UINT8 aucBuffer[32] = { 0 };

        AT_Sprintf(aucBuffer, "+CCWA: %u", gATCurrentucSsCallWaitingPresentResultCode, pParam->nDLCI);
        // AT_SS_SYN_RETURN(ppstResult, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI);
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
    }

    break;

    // error command type

    default:
        // AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        AT_CC_Result_Err(ERR_AT_UNKNOWN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: AT+CCFC = <reason>,<mode!=2>[,<number>[,<type>[,<class>[,<subaddr>[,<satype>[,<time>]]]]]]
// Call forwarding number and conditions
// AT+CCFC=0,3,"02164708411",129
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      query mode,only for single class,not for all class
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CCFC(AT_CMD_PARA *pParam)
{

    AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
    COS_Sleep(50);
    UINT8 *pucParam = (UINT8 *)(pParam->pPara);
    UINT32 uiRetVal;
    INT32 iRetVal = 0;
    INT32 i;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
#ifdef AT_SS_DEBUG
    // output debug information
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

#endif

    // parameter check

    if (NULL == pParam)
    {
        AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
        AT_CC_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // at command type judge
    switch (pParam->iType)
    {
        AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);

    // extend command (set)

    case AT_CMD_SET:
    {
        AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
        UINT16 ucLen;
        UINT8 ucParaCount                   = 0;
        UINT8 ucReason                      = 0XFF;
        UINT8 ucMode                        = 0xff,  ucNumType = 0, ucClass = 0;
        UINT16 ucNumLen = 0;
        UINT8 ucNum[MAX_LENGTH_NUMBER_CCFC] = { 0, };

        AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
        UINT8 ucTime = 0;

        // call ATM function
        iRetVal = AT_Util_GetParaCount(pucParam, &ucParaCount);

        // result check and parameter count

        if (ERR_SUCCESS != iRetVal || (8 < ucParaCount) || ucParaCount < 2)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // first parameter
        ucLen = 1;

        iRetVal = AT_Util_GetParaWithRule(pucParam, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)(&ucReason), &ucLen);
        if ((ERR_SUCCESS != iRetVal) || ucReason > 5)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            AT_TC(g_sw_AT_SS, "CCFC ERR 2\n");
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // second parameter
        ucLen = 1;

        iRetVal = AT_Util_GetParaWithRule(pucParam, 1, AT_UTIL_PARA_TYPE_UINT8, (PVOID)(&ucMode), &ucLen);

        if ((ERR_SUCCESS != iRetVal) || ucMode > 4)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            AT_TC(g_sw_AT_SS, "CCFC ERR 3\n");
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // third parameter
        if (ucParaCount > 2)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            UINT8 tempNum[MAX_LENGTH_NUMBER_CCFC] = { 0, };

            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            ucNumLen = MAX_LENGTH_NUMBER_CCFC;
            iRetVal  = AT_Util_GetParaWithRule(pucParam, 2, AT_UTIL_PARA_TYPE_STRING, (PVOID)tempNum, &ucNumLen);

            if (ERR_SUCCESS != iRetVal && ERR_AT_UTIL_CMD_PARA_NULL != iRetVal)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_TC(g_sw_AT_SS, "CCFC ERR 4\n");
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INDIAL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

            if (tempNum[0] == '+')
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);

                // [[Changyg[mod] 2008-04-30 :call number identify
                for (i = 1; i < ucNumLen; i++)
                {
                    AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                    if ((tempNum[i] < 0x30) || (tempNum[i] > 0x39))
                    {
                        AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                        AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INDIAL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        return;
                    }
                }

                AT_TC(g_sw_AT_SS, "identify success %d\n", ucNumLen);

                // [[Changyg[mod] 2008-04-29 :end
                ucNumLen = AT_SsAsciiToGsmBcd(&tempNum[1], ucNumLen - 1, ucNum);
            }
            else
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);

                // [[Changyg[mod] 2008-04-30 :call number identify
                for (i = 0; i < ucNumLen; i++)
                {
                    AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                    if ((tempNum[i] < 0x30) || (tempNum[i] > 0x39))
                    {
                        AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                        AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INDIAL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                        AT_TC(g_sw_AT_SS, "identify fail %d\n", ucNumLen);
                        return;
                    }
                }

                AT_TC(g_sw_AT_SS, "identify success %d\n", ucNumLen);

                // [[Changyg[mod] 2008-04-29 :end
                ucNumLen = AT_SsAsciiToGsmBcd(tempNum, ucNumLen, ucNum);
            }
        }

        // forth parameter
        if (ucParaCount > 3)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 3, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucNumType, &ucLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == iRetVal)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                if (ucNum[0] == '+')
                    ucNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
                else
                    ucNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else if (ERR_SUCCESS != iRetVal)  //
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_TC(g_sw_AT_SS, "CCFC ERR 5\n");
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

            // [[hameina[+]2007.12.14 for bug 7232
            else
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                if (ucNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL && ucNumType != CFW_TELNUMBER_TYPE_UNKNOWN)
                {
                    AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                    AT_TC(g_sw_AT_SS, "CCFC ERR 6\n");
                    AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                    break;
                }
            }

            // ]]hameina[+]2007.12.14 for bug 7232

        }
        else
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            ucNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }

        // fifth parameter
        if (ucParaCount > 4)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 4, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucClass, &ucLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == iRetVal)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                ucClass = 11;
            }
            else if (ERR_SUCCESS != iRetVal || ((ucClass != 1) && (ucClass != 255)))
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_TC(g_sw_AT_SS, "CCFC ERR 6\n");
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }
            else
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);

                ucClass = 11;
            }
        }
        else
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            ucClass = 11;
        }

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "Para count:%d reason=%d,  mode=%d, type=%d, class=%d\n",
              ucParaCount, ucReason, ucMode, ucNumType, ucClass);

#endif
        // eighth parameter: time
        if (ucParaCount == 8)
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 7, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucTime, &ucLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == iRetVal)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                ucTime = 20;
            }
            else if (ERR_SUCCESS != iRetVal)  //
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_TC(g_sw_AT_SS, "CCFC ERR 7\n");
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

        }
        else
            ucTime = 20;  // hameina[+] for bug 6935

        // excuete condition
        if (ucMode == 2)  // query status
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);

            uiRetVal = CFW_SsQueryCallForwarding(ucReason, ucClass, pParam->nDLCI, nSim);
            AT_TC(g_sw_AT_SS, "uiRetVal:0x%x reason=%d,   class=%d\n", uiRetVal, ucReason, ucClass);
            if (uiRetVal != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_TC(g_sw_AT_SS, "CCFC ERR 8\n");
                AT_CC_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }
        }
        else  // set call forwarding
        {
            AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
            CFW_SS_SET_CALLFORWARDING_INFO sSetCCFC;

            memset(&sSetCCFC, 0, sizeof(CFW_SS_SET_CALLFORWARDING_INFO));
            sSetCCFC.nReason = ucReason;
            sSetCCFC.nMode   = ucMode;
            sSetCCFC.nClass  = ucClass;
            sSetCCFC.nTime   = ucTime;

            if (ucNumLen)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                sSetCCFC.nNumber.nDialNumberSize = ucNumLen;
                sSetCCFC.nNumber.nType           = ucNumType;
                sSetCCFC.nNumber.pDialNumber     = ucNum;
                sSetCCFC.nNumber.nClir           = 0;
            }
            uiRetVal = CFW_SsSetCallForwarding(&sSetCCFC, pParam->nDLCI, nSim);
            AT_TC(g_sw_AT_SS, "222uiRetVal:0x%x reason=%d,   class=%d\n", uiRetVal, ucReason, ucClass);
            if (uiRetVal != ERR_SUCCESS)
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_TC(g_sw_AT_SS, "CCFC ERR 9\n");
                AT_CC_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            }
            else
            {
                AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
                AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            }

        }
    }

    break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "+CCFC:<0-5>
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCCFC, AT_StrLen(strTCCFC), pParam->nDLCI, nSim);
        break;

    // error command type

    default:
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;

    }
    AT_TC(g_sw_AT_SS, "%s:%d", __func__, __LINE__);
    return;

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: AT+CLIP =<n>
// Calling line identification presentation
// AT+CLIP=1
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      set only on/off function
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CLIP(AT_CMD_PARA *pParam)
{
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT32 uiRetVal   = ERR_SUCCESS;
    INT32 iRetVal     = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

#ifdef AT_SS_DEBUG
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

#endif

    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // at command type judge
    switch (pParam->iType)
    {
    // extend command (set)

    case AT_CMD_SET:
    {
        UINT8 ucClip = 0;

        // call ATM function
        iRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);

        // result check

        if (ERR_SUCCESS != iRetVal || ucParaCount > 1)
        {
            // return parameter error
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // parameter check
        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        // first parameter
        ucLen = sizeof(UINT8);

        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucClip, &ucLen);

        if (ERR_SUCCESS != iRetVal || ucClip > 1)
        {
            // return parameter error
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

#ifdef AT_SS_DEBUG
        AT_TC(g_sw_AT_SS, "Para count:%d Parameter 1: %d\n", ucParaCount, ucClip);

#endif

        // call CSW interface function
        // uiRetVal = CFW_CfgSetClip(ucClip);
        gATCurrentucClip = ucClip;

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "CLIP set ret: %x\n", uiRetVal);

#endif

        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    break;

    case AT_CMD_TEST:
        // return command format "+CLIP:(0,1)"
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCLIP, AT_StrLen(strTCLIP), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        // get UTI value
        if (ERR_SUCCESS != uiRetVal)
        {
            // get UTI error
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // parameter initial
        // call CSW interface function

        uiRetVal = CFW_SsQueryClip(pParam->nDLCI, nSim);
#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "CLIP read ret: %x\n", uiRetVal);
#endif

        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 5, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }

        break;

    // error command type

    default:
        AT_CC_Result_Err(ERR_AT_UNKNOWN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }
}

VOID AT_SS_CmdFunc_SAVECLIR(AT_CMD_PARA *pParam)
{
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT32 uiRetVal   = ERR_SUCCESS;
    INT32 iRetVal     = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    UINT8 ucClir = 0;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {

        iRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);
        if (ERR_SUCCESS != iRetVal || ucParaCount > 1)
        {
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        ucLen = 1;
        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucClir, &ucLen);

        if (ERR_SUCCESS != iRetVal || ucClir > 2)
        {
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        uiRetVal = CFW_CfgSetClir(ucClir, nSim);
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }
    break;

    case AT_CMD_TEST:
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCLIR, AT_StrLen(strTCLIR), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:

        uiRetVal = AT_GetFreeUTI(CFW_SS_SRV_ID, &pParam->nDLCI);

        if (ERR_SUCCESS != uiRetVal)
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }


        if (CFW_CfgGetClir(&ucClir, nSim) == ERR_SUCCESS)
        {
            UINT8 aucBuffer[16] = { 0 };
            AT_Sprintf(aucBuffer, "+SAVECLIR: %d", ucClir, pParam->nDLCI);
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
        }

        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }

        break;

    default:
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;

    }

}


// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +CLIR=[<n>]
// Calling line identification restriction
// AT+CLIR=0
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      set only on/off function
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CLIR(AT_CMD_PARA *pParam)
{
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT32 uiRetVal   = ERR_SUCCESS;
    INT32 iRetVal     = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
#ifdef AT_SS_DEBUG
    // output debug information
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

#endif
    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // at command type judge
    switch (pParam->iType)
    {
    // extend command (set)

    case AT_CMD_SET:
    {
        UINT8 ucClir = 0;

        // call ATM function
        iRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);

        // result check

        if (ERR_SUCCESS != iRetVal || ucParaCount > 1)
        {
            // return parameter error
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // parameter check
        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        // first parameter
        ucLen = 1;

        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucClir, &ucLen);

        if (ERR_SUCCESS != iRetVal || ucClir > 2)
        {
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "Para count:%d Parameter 1: %d\n", ucParaCount, ucClir);

#endif
        // call CSW interface function
        // uiRetVal = CFW_CfgSetClir(ucClir);
        gATCurrentucClir = ucClir;

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "CLIR set ret: %x\n", uiRetVal);

#endif

        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "+CLIR:(0,1)"
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCLIR, AT_StrLen(strTCLIR), pParam->nDLCI, nSim);
        break;

    // extend command (read)

    case AT_CMD_READ:

        // get UTI value
        uiRetVal = AT_GetFreeUTI(CFW_SS_SRV_ID, &pParam->nDLCI);

        if (ERR_SUCCESS != uiRetVal)
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        uiRetVal = CFW_SsQueryClir(pParam->nDLCI, nSim);
#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "CLIR read ret: %x\n", uiRetVal);

#endif
        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }

        break;

    default:
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;

    }

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +COLP=[<n>]
// Connected line identification presentation
// AT+COLP=0
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      set only on/off function
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_COLP(AT_CMD_PARA *pParam)
{
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT32 uiRetVal   = ERR_SUCCESS;
    INT32 iRetVal     = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
#ifdef AT_SS_DEBUG
    // output debug information
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

#endif
    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // at command type judge
    switch (pParam->iType)
    {
    // extend command (set)

    case AT_CMD_SET:
    {
        UINT8 ucPara1 = 0;

        iRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);

        if (ERR_SUCCESS != iRetVal || ucParaCount > 1)
        {
            // return parameter error
            AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        // parameter check
        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        // first parameter
        ucLen = 1;
        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucPara1, &ucLen);

        if (ERR_SUCCESS != iRetVal || ucPara1 > 1)
        {
            // return parameter error
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "Para count:%d Parameter 1: %d\n", ucParaCount, ucPara1);

#endif

        // call CSW interface function
        // uiRetVal = CFW_CfgSetColp(ucPara1);
        gATCurrentucColp = ucPara1;

#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "COLP set ret: %x\n", uiRetVal);

#endif
        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "+COLP:(0,1)"
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCOLP, AT_StrLen(strTCOLP), pParam->nDLCI, nSim);
        break;
    case AT_CMD_READ:
    {
        // get UTI value

        if (ERR_SUCCESS != uiRetVal)
        {
            // get UTI error
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
        // call CSW interface function
        uiRetVal = CFW_SsQueryColp(pParam->nDLCI, nSim);  // network don't support
#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "COLP read ret: %x\n", uiRetVal);
#endif

        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            // AT_SS_TRUE_RETURN(ppstResult, CMD_FUNC_SUCC_ASYN, ASYN_DELAY_CLIP, NULL, 0, pParam->nDLCI);
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 5, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
        break;
    }

    // error command type

    default:
        AT_CC_Result_Err(ERR_AT_UNKNOWN, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:
// Connected Line Identification Restriction
// Parameter:  pParam, parsed command structure
// Caller:
// Called:     ATM
// Return:     fail or succeed
// Remark:     set only on/off function
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_COLR(AT_CMD_PARA *pParam)
{
    AT_CMD_RESULT **ppstResult = NULL;
    UINT32 uiServiceId = CFW_SS_SRV_ID;
    UINT32 uiRetVal    = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

#ifdef AT_SS_DEBUG
    // output debug information
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);

#endif

    // parameter check

    if (NULL == pParam)
    {
        // return parameter error
        AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (UINT8 *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // at command type judge
    switch (pParam->iType)
    {
    // extend command (set) not support

    case AT_CMD_SET:
        // retrieve command parameter

        // execute result return

        if (ERR_SUCCESS == uiRetVal)
        {
            AT_SS_TRUE_RETURN(ppstResult, CMD_FUNC_SUCC_ASYN, ASYN_DELAY_COLR, NULL, 0, pParam->nDLCI);
        }
        else
        {
            AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (UINT8 *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        }

        break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "+COLR:(0,2)"
        AT_SS_SYN_RETURN(ppstResult, (UINT8 *)strTCOLR, AT_StrLen(strTCOLR), pParam->nDLCI);

        break;

    // extend command (read)

    case AT_CMD_READ:

        // get UTI value
        uiRetVal = AT_GetFreeUTI(uiServiceId, &pParam->nDLCI);

        if (ERR_SUCCESS != uiRetVal)
        {
            // get UTI error
            AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (UINT8 *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        }

        uiRetVal = CFW_SsQueryColr(pParam->nDLCI, nSim);
#ifdef AT_SS_DEBUG
        // output debug information
        AT_TC(g_sw_AT_SS, "COLR read ret: %x\n", uiRetVal);

#endif

        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_SS_TRUE_RETURN(ppstResult, CMD_FUNC_SUCC_ASYN, ASYN_DELAY_COLR, NULL, 0, pParam->nDLCI);
        }
        else
        {
            AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (UINT8 *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);
        }

        break;

    // error command type

    default:
        AT_SS_FAIL_RETURN(ppstResult, CMD_FUNC_FAIL, (UINT8 *)strFCCWA, AT_StrLen(strFCCWA), pParam->nDLCI);

        break;

    }

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +CUSD=[<n>[,<str>[,<dcs>]]]
// Unstructured supplementary service data
// AT+CUSD=1,"*#100#",0x0f
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      during same session,must have same UTI
// only judge session start/end from respond event
// ////////////////////////////////////////////////////////////////////////////
VOID AT_SS_CmdFunc_CUSD(AT_CMD_PARA *pParam)
{
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT8 *pucParam   = (UINT8 *)(pParam->pPara);
    UINT32 uiRetVal;
    INT32 iRetVal = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
#ifdef AT_SS_DEBUG
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);
#endif

    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_MEMORY_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
    {
        UINT8 ucOption                         = 3; // MS originate USSD service
        UINT8 ucDcs                            = DEFAULT_ALPHABET; // default alphabet 0x0F
        UINT8 ucPara1                          = 0, aucPara2[MAX_LENGTH_STRING_USSD + 2] = { 0 };
        UINT16 aucPara2len                      = 0;
        UINT8 u7bitStr[MAX_LENGTH_STRING_USSD] = { 0, }, u7bitLen = 0;

        iRetVal = AT_Util_GetParaCount(pucParam, &ucParaCount);

        if (ERR_SUCCESS != iRetVal || ucParaCount > 3)
        {
            AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            break;
        }

        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        if (ucParaCount > 0)
        {
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucPara1, &ucLen);

            if (ERR_SUCCESS != iRetVal || ucPara1 > 2)
            {
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }
        }

        if (ucParaCount > 1)
        {
            aucPara2len = MAX_LENGTH_STRING_USSD;
            iRetVal     = AT_Util_GetParaWithRule(pucParam, 1, AT_UTIL_PARA_TYPE_STRING, (PVOID)aucPara2, &aucPara2len);

            if (ERR_SUCCESS != iRetVal)
            {
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }

        }

        ucLen = 0;
        if (ucParaCount > 2)
        {
            ucLen   = sizeof(UINT8);
            iRetVal = AT_Util_GetParaWithRule(pucParam, 2, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucDcs, &ucLen);

            if (ERR_SUCCESS != iRetVal)
            {
                // return parameter error
                AT_CC_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                break;
            }
        }

        AT_TC(g_sw_AT_SS, "Para count:%d Parameter 1: %d  string: %s ucDcs: %d\n", ucParaCount, ucPara1, aucPara2, ucDcs);
        //
        // [[hameina[mod] 2008-4-14 for bug 8005: if send USSD string failed, the first param can also be changed.
        if (ucParaCount == 1 && ucPara1 != 2) // hameina[+]07.10.16
        {
            if (1 == ucPara1)
            {
                gATCurrentss_ussd = 1;
            }
            else if (0 == ucPara1)
            {
                gATCurrentss_ussd = 0;
            }

            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            break;
        }

        /*
           option Value Description
           0 MS 返回 Error(nUsdStringSize标识 Errsource , pUsdString[0]标识具体的 ErrCode );USSD version 2 operation.
           1 MS返回拒绝 (nUsdStringSize标识 ProblemCodeTag ,pUsdString[0]标识具体的 ProblemCode );USSD version 2 operation.
           2 terminate the session（release complete）; USSD version 2 operation.
           3 MS发起 ussd 或返回网络发起的 ussd 操作;USSD version 2 operation.
           128 MS 返回 Error(nUsdStringSize标识 Errsource, pUsdString[0]标识具体的 ErrCode ); USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           129 MS返回拒绝(nUsdStringSize标识 ProblemCodeTag, pUsdString[0]标识具体的 ProblemCode); USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           130 terminate the session（release complete）; USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           131 MS发起ussd或返回网络发起的ussd操作; USSD version 1 operation.（The string must be encoded as an IA5 string, nDcs is ignored）
           */
        if (ucPara1 == 2)
        {
            if (g_ss_ussdVer == 2)
                ucOption = 2;
            else
                ucOption = 130;
        }
        else
        {
            if (ucLen)  // has dcs
            {
                INT32 rst;

                g_ss_ussdVer = 2;
                ucOption     = 3;
                rst          = SUL_Encode7Bit(aucPara2, u7bitStr, aucPara2len);
                AT_TC(g_sw_AT_SS, "rst= %u \n", rst);
                u7bitLen = (aucPara2len % 8) ? (aucPara2len * 7 / 8 + 1) : (aucPara2len * 7 / 8);
                AT_TC(g_sw_AT_SS, "u7bitLen = %u \n", u7bitLen);
                u7bitStr[u7bitLen] = '\0';
            }
            else
            {
                g_ss_ussdVer = 1;
                ucOption     = 131;
            }
        }

        AT_TC(g_sw_AT_SS, "USSD AT:  u7bitLen=%d ucDcs= %d   pParam->nDLCI=%d, u7bitStr[0]=0x%x, u7bitStr[1]=0x%x\n",
              u7bitLen, ucDcs, pParam->nDLCI, u7bitStr[0], u7bitStr[1]);

        memcpy(at_ussdstring[nSim].ussd_input, u7bitStr, AT_MAX_DIGITS_USSD);
        at_ussdstring[nSim].b_used = FALSE;


        if (AT_SS_GetUSSDState())
            uiRetVal = CFW_SsSendUSSD(u7bitStr, u7bitLen, ucOption, ucDcs, AT_SS_GetUSSDNum(), nSim);
        else
            uiRetVal = CFW_SsSendUSSD(u7bitStr, u7bitLen, ucOption, ucDcs, pParam->nDLCI, nSim);

        AT_TC(g_sw_AT_SS, "USSD AT: ucOption=%d, uiRetVal=0x%x\n", ucOption, uiRetVal);
        if (ERR_SUCCESS == uiRetVal)
        {
            if (1 == ucPara1)
            {
                gATCurrentss_ussd = 1;
            }
            else if (0 == ucPara1)
            {
                gATCurrentss_ussd = 0;
            }

            AT_AddWaitingEvent(EV_CFW_SS_SEND_USSD_RSP, nSim, pParam->nDLCI);
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_SS_ResetUSSDState();
            AT_CC_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    break;

    case AT_CMD_TEST:
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCUSD, AT_StrLen(strTCUSD), pParam->nDLCI, nSim);
        break;
    case AT_CMD_READ:
    {
        UINT8 uOutString[20];

        memset(uOutString, 0, 20);
        AT_Sprintf(uOutString, strRCUSD, gATCurrentss_ussd);
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, uOutString, AT_StrLen(uOutString), pParam->nDLCI, nSim);
    }

    break;

    default:
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;

    }

}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description: +CSSN=[<n>[,<m>]]
// Supplementary service notifications,+CSSI messages,+CSSU messages
// AT+CSSN=1,0
// Parameter:   pParam, parsed command structure
// Caller:
// Called:      ATM
// Return:      fail or succeed
// Remark:      have on/off function to CC module
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CSSN(AT_CMD_PARA *pParam)
{
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT8 ucCSSI      = 0;
    UINT8 ucCSSU      = 0;
    UINT8 aucBuffer[20] = { 0 };
    UINT8 *pucParam     = (UINT8 *)(pParam->pPara);
    UINT32 uiRetVal = ERR_SUCCESS;
    INT32 iRetVal   = 0;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

#ifdef AT_SS_DEBUG
    // output debug information
    AT_TC(g_sw_AT_SS, "Parameter string: %s  Parameter type: %d\n", pParam->pPara, pParam->iType);
#endif
    if (NULL == pParam)
    {
        AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    // call ATM function

    // at command type judge
    switch (pParam->iType)
    {
    // extend command (set)

    case AT_CMD_SET:
    {

        // UINT8  ucPara1 = 0, ucPara2 = 0;
        // UINT8  ucPara1Flg = EXIST, ucPara2Flg = EXIST;
        iRetVal = AT_Util_GetParaCount(pucParam, &ucParaCount);

        // result check

        if (ERR_SUCCESS != iRetVal || ucParaCount > 2)
        {
            // return parameter error
            AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        if (!ucParaCount)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }

        if (ucParaCount > 0)
        {
            // first parameter gATCurrentucCSSI
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucCSSI, &ucLen);

            if (ERR_SUCCESS != iRetVal || (ucCSSI > 1 && ucCSSI != 0xff))
            {
                // return parameter error
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        // second parameter
        if (ucParaCount == 2)
        {
            ucLen   = 1;
            iRetVal = AT_Util_GetParaWithRule(pucParam, 1, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucCSSU, &ucLen);

            if (ERR_SUCCESS != iRetVal || ucCSSU > 1)
            {
                // return parameter error
                AT_CC_Result_Err(ERR_AT_CME_INVALID_CHAR_INTEXT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }
        else

            // hameina[mod] 2008-4-15 for bug 8119, if only set param <n>, param<m> will be changed to 255
            ucCSSU = gATCurrentucCSSU;

#ifdef AT_SS_DEBUG
        AT_TC(g_sw_AT_SS, "Para count:%d Parameter 1: %d  Parameter 2: %d\n", ucParaCount, ucCSSI, ucCSSU);

#endif

        // call CSW interface function
        // uiRetVal = CFW_CfgSetSSN(ucCSSI, ucCSSU);
        gATCurrentucCSSI = ucCSSI;

        gATCurrentucCSSU = ucCSSU;

#ifdef AT_SS_DEBUG
        AT_TC(g_sw_AT_SS, "CSSN set ret: %x\n", uiRetVal);

#endif

        // execute result return
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        }
        else
        {
            AT_CC_Result_Err(ERR_AT_CME_PHONE_FAILURE, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        }
    }

    break;

    // extend command (test)

    case AT_CMD_TEST:
        // return command format "+CSSN:(0-1),(0-1)"
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (UINT8 *)strTCSSN, AT_StrLen(strTCSSN), pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:
        // call CSW interface function
        // uiRetVal = CFW_CfgGetSSN(&ucCSSI, &ucCSSU);
#ifdef AT_SS_DEBUG
        AT_TC(g_sw_AT_SS, "CSSN read ret: %x\n", uiRetVal);
#endif

        // execute result return
        AT_Sprintf(aucBuffer, strRCSSN, gATCurrentucCSSI, gATCurrentucCSSU);

        // AT_SS_SYN_RETURN(ppstResult, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI);
        AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
        break;

    default:
        AT_CC_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}

// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:+CAOC[=<mode>]
// Advice of Charge
// AT+CAOC=2
// Parameter:  pParam, parsed command structure
// Caller:
// Called:     ATM
// Return:     fail or succeed
// Remark:     no CSW interface
// not support the AT command
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CAOC(AT_CMD_PARA *pParam)
{
    AT_CMD_RESULT **ppstResult = NULL;
    UINT16 ucLen;
    UINT8 ucParaCount = 0;
    UINT8 aucBuffer[MAX_LENGTH_BUFF] = { 0 };
    UINT8 *pucParam                  = (UINT8 *)(pParam->pPara);
    INT32 iRetVal = 0;
    UINT32 uiRetVal;
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;    
    }
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;
    iRetVal = AT_Util_GetParaCount(pucParam, &ucParaCount);
    if (ERR_SUCCESS != iRetVal)
    {
	 AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
	 return;   
    }

    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            UINT8 ucFileType = AT_SIM_FILE_CAOC;
            UINT32 uiRetAoc;
            UINT8 ucPara1    = 0;
            UINT8 ucPara1Flg = EXIST;
            if (PARA_CNT_CAOC != ucParaCount)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
	     }
            ucLen = sizeof(UINT8);
            iRetVal = AT_Util_GetParaWithRule(pucParam, 0, AT_UTIL_PARA_TYPE_UINT8, (PVOID)&ucPara1, &ucLen);
            if (ERR_SUCCESS != iRetVal && ERR_AT_UTIL_CMD_PARA_NULL != iRetVal)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;            
	     }
            if (0 == ucLen)
            {
                ucPara1Flg = NOT_EXIST;
            }
            if (NOT_EXIST != ucPara1Flg)
            {
                if (0 == ucPara1)
                {
                    uiRetVal = SS_ReadAocFromSim(ucFileType, &uiRetAoc);
                    if (ERR_SUCCESS == uiRetVal)
                    {
            	   	   AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
                    }
                    else
                    {
	                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
	                return;
                    }
                }
                else
                {
                    g_SsCaocStatus = ucPara1;
			AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
                }

            }
            else
            	{               
			AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
			return;
		}
            }
        break;
		
        case AT_CMD_READ:
		AT_Sprintf(aucBuffer, strRCAOC, g_SsCaocStatus);
		AT_CC_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, aucBuffer, AT_StrLen(aucBuffer), pParam->nDLCI, nSim);
		return; 
        default:
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }


// ////////////////////////////////////////////////////////////////////////////
// Name:
// Description:+CPUC=<currency>,<ppu>[,<passwd>]
// Price per unit and currency table
// AT+CPUC="GBP","0.125","2222"
// Parameter:  pParam, parsed command structure
// Caller:
// Called:     ATM
// Return:     fail or succeed
// Remark:     PPU must be change into 3gpp 11.890 format
//
// ////////////////////////////////////////////////////////////////////////////

VOID AT_SS_CmdFunc_CPUC(AT_CMD_PARA *pParam)
{
    UINT16 ucLen       = 0;
    UINT8 ucParaCount = 0;
    UINT32 uiRetVal   = 0;
    UINT8 arrCurrency[MAX_LENGTH_CURRENCY_CPUC];
    UINT8 arrPricePerUnit[MAX_LENGTH_PPU_CPUC];
    UINT8 arrPassword[MAX_LENGTH_PIN2];
    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (NULL == pParam)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    // check event respond exclusive
    g_uiCurrentCmdStampSs = pParam->uCmdStamp;

    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        INT32 iRetVal = 0;

        AT_TC(g_sw_AT_SS, "cpuc set ---in\n");
        iRetVal = AT_Util_GetParaCount(pParam->pPara, &ucParaCount);

        if ((ERR_SUCCESS != iRetVal) || (ucParaCount < 2) || (ucParaCount > 3))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // parameter initial
        CFW_SIM_PUCT_INFO stPUCT;
        AT_MemSet(&stPUCT, 0, sizeof(CFW_SIM_PUCT_INFO));
        //      SIM_PUCT_INFO stPUCTTmp;
        // include string must be initial
        //      AT_MemSet(&stPUCTTmp, 0, sizeof(SIM_PUCT_INFO));
        // get parameter: <currency>
        AT_MemZero(arrCurrency, sizeof(arrCurrency));

        ucLen = MAX_LENGTH_CURRENCY_CPUC;

        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, arrCurrency, &ucLen);

        if ((ERR_SUCCESS != iRetVal) || (ucLen > 3))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // AT_StrUpr(arrCurrency);
        // if arrCurrency size <3, default char is filled with ' '.
        AT_MemSet(stPUCT.iCurrency, ' ', 3);
        AT_MemCpy(stPUCT.iCurrency, arrCurrency, AT_StrLen(arrCurrency));
        // get parameter: <ppu>
        AT_MemZero(arrPricePerUnit, sizeof(arrPricePerUnit));

        ucLen = MAX_LENGTH_PPU_CPUC;

        iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, arrPricePerUnit, &ucLen);

        if (ERR_SUCCESS != iRetVal || (!ucLen)) // can't input a string which len=0
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get parameter: <password>
        AT_MemZero(arrPassword, sizeof(arrPassword));

        if (ucParaCount > 2)
        {
            ucLen   = MAX_LENGTH_PIN2;
            iRetVal = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, arrPassword, &ucLen);

            if (ERR_SUCCESS != iRetVal)
            {
                AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
                return;
            }
        }

        // decimal be changed into integer format to store
        //if (!SS_PPUFloatStrToIntStructure(&stPUCTTmp, arrPricePerUnit, AT_StrLen(arrPricePerUnit)))
        if(!SS_String2PUCT(&stPUCT, arrPricePerUnit))
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        // get changed parameter
        //stPUCT.iEPPU = stPUCTTmp.iEPPU;
        //      stPUCT.iEX = stPUCTTmp.iEX;
        //  stPUCT.iSign = stPUCTTmp.iSign;

        AT_TC(g_sw_AT_SS, "CFW_SimSetPUCT,iEPPU --%d, iEX--%d, iSign--%d", stPUCT.iEPPU, stPUCT.iEX, stPUCT.iSign);
        uiRetVal = CFW_SimSetPUCT(&stPUCT, arrPassword, AT_StrLen(arrPassword), pParam->nDLCI, nSim);

        if (ERR_SUCCESS == uiRetVal)
        {
            AT_TC(g_sw_AT_SS, "CFW_SimSetPUCT SUCCESS");
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_TC(g_sw_AT_SS, "CFW_SimSetPUCT error");
            UINT32 nErrorCode = AT_SetCmeErrorCode(uiRetVal, TRUE);
            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

    }

    break;

    case AT_CMD_TEST:
        AT_TC(g_sw_AT_SS, "cpuc test ---in\n");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
        break;

    case AT_CMD_READ:
    {
        AT_TC(g_sw_AT_SS, "cpuc read-in\n");

        uiRetVal = CFW_SimGetPUCT(pParam->nDLCI, nSim);
        if (ERR_SUCCESS == uiRetVal)
        {
            AT_TC(g_sw_AT_SS, "CFW_SimGetPUCT SUCCESS");
            AT_CC_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_TC(g_sw_AT_SS, "CFW_SimGetPUCT error");
            UINT32 nErrorCode = AT_SetCmeErrorCode(uiRetVal, FALSE);

            AT_Result_Err(nErrorCode, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        break;
    }

    default:
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
        break;

    }

}

/******************************************************************************************
Function            :   AT_SS_SetACM_rsp
Description     :       process the response of AT ACM command
Called By           :   AT_SMS_AsyncEventProcess
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :   for AT+CACM SET COMMAND 2007.11.19 by unknown author
modify and add comment by wangqunyang 2008.04.07
 *******************************************************************************************/
VOID AT_SS_SetACM_rsp(COS_EVENT *pEvent)
{
    CFW_EVENT sCfwEvent = { 0 };
    UINT8 nSim;

    AT_CosEvent2CfwEvent(pEvent, &sCfwEvent);
    nSim = sCfwEvent.nFlag;

    if (sCfwEvent.nType != 0)
    {
        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, sCfwEvent.nUTI, nSim);
        return;
    }

    AT_TC(g_sw_AT_SMS, " @@@ %ld \n ", sCfwEvent.nParam1);
    AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, sCfwEvent.nUTI, nSim);
    return;

}

/******************************************************************************************
Function            :   AT_SS_GetACM_rsp
Description     :       get ACM information of ACM response
Called By           :   AT_SMS_AsyncEventProcess
Data Accessed       :
Data Updated        :
Input           :       COS_EVENT *pEvent
Output          :
Return          :   VOID
Others          :   for AT+CACM READ COMMAND 2007.11.19 by unknown author
modify and add comment by wangqunyang 2008.04.07
 *******************************************************************************************/
VOID AT_SS_GetACM_rsp(COS_EVENT *pEvent)
{
    CFW_EVENT sCfwEvent      = { 0 };
    UINT8 PromptInfoBuff[20] = { 0 };
    UINT8 nType              = 0x0;
    UINT32 nParam1;
    UINT32 nParam2;

    UINT8 nSim;
    AT_CosEvent2CfwEvent(pEvent, &sCfwEvent);
    nSim = sCfwEvent.nFlag;
    nType   = sCfwEvent.nType;
    nParam1 = sCfwEvent.nParam1;
    nParam2 = sCfwEvent.nParam2;

    AT_TC(g_sw_AT_SMS, "@@@@@ nType==OX%x    nParam1==0x%x  nParam2==0x%x\n  ", nType, nParam1, nParam2);

    if (nType != 0)
    {
        AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, sCfwEvent.nUTI, nSim);
        return;
    }
    CSW_TRACE(CFW_SIM_TS_ID | C_DETAIL, "--nParam1= 0x%x%x%x\n",*((UINT8*)&nParam1),*((UINT8*)&nParam1+1),*((UINT8*)&nParam1+2));
    UINT8* pData = (UINT8*)&nParam1;
    AT_Sprintf(PromptInfoBuff, "+CACM: \"%02X%02X%02X\"", pData[0], pData[1], pData[2]);
    AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptInfoBuff, AT_StrLen(PromptInfoBuff), sCfwEvent.nUTI, nSim);
    return;

}

/******************************************************************************************
Function            :   AT_SS_CmdFunc_CACM
Description     :       CACM procedure function
Called By           :   ATS SMS module
Data Accessed       :
Data Updated        :
Input           :       AT_CMD_PARA *pParam
Output          :
Return          :   VOID
Others          :   build by unknown author 2007.11.15
modify and add comment by wangqunyang 2008.04.07
 *******************************************************************************************/
VOID AT_SS_CmdFunc_CACM(AT_CMD_PARA *pParam)
{

    UINT16 nParamLen     = 0;
    UINT8 nParamCount = 0;
    UINT8 pPIN2[8]      = { 0 };
    //UINT8 nPinSize      = 4;
    UINT32 iCurValue    = 0;
    INT32 nOperationRet = 0;

    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);

    // g_SMS_CurCmdStamp=pParam->uCmdStamp;
    AT_TC(g_sw_AT_SS, "pParam->iType = %d", pParam->iType);
    switch (pParam->iType)
    {

    case AT_CMD_SET:
    {
        // Parse parameter and do it!
        nParamLen = 8;  // pin2 length

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        AT_TC(g_sw_AT_SS, "COUNT: nOperationRet = %d,nParamCount = %d", nOperationRet, nParamCount);
        if (nOperationRet != ERR_SUCCESS || nParamCount != 1)
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // get NO.1 param
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (void *)pPIN2, &nParamLen);

        // changyg[+]2008.5.20 for bug 8496
        if ((nOperationRet != ERR_SUCCESS) || (nParamLen > 4) )
        {
            AT_Result_Err(ERR_AT_CME_TEXT_LONG, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        // changyg[+] end
        AT_TC(g_sw_AT_SS, "pPIN2 = %s,ppAT_StrLen(pPIN2) = %d", pPIN2, AT_StrLen(pPIN2));
        nOperationRet = CFW_SimSetACM(iCurValue, pPIN2, AT_StrLen(pPIN2), pParam->nDLCI, nSim);
        if (nOperationRet == ERR_SUCCESS)
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }


    }
    break;
    case AT_CMD_TEST:
    {
        AT_TC(g_sw_AT_SS, "AT_SS_CmdFunc_CACM: Test");
        AT_SMS_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
        return;
    }
    break;
    case AT_CMD_READ:
    {
        nOperationRet = CFW_SimGetACM(pParam->nDLCI, nSim);
        AT_TC(g_sw_AT_SS, "nOperationRet = %d\n",nOperationRet);
        if (nOperationRet == ERR_SUCCESS)
        {
            AT_SMS_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, 0, 0, pParam->nDLCI, nSim);
            return;
        }
        else
        {
            AT_Result_Err(ERR_AT_CME_EXE_FAIL, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
    }
    break;
    default:
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        break;
    }

    return;
}



UINT8 ATssPswdServiceCode[] = {'0', '3', 0};
UINT8 ATssClipCode[] = {'3', '0', 0};
UINT8 ATssClirCode[] = {'3', '1', 0};
UINT8 ATssColpCode[] = {'7', '6', 0};
UINT8 ATssColrCode[] = {'7', '7', 0};
UINT8 ATssCnapCode[] = {'3', '0', '0', 0};
UINT8 ATssCfuCode[] = {'2', '1', 0};
UINT8 ATssCfbCode[] = {'6', '7', 0};
UINT8 ATssCfnryCode[] = {'6', '1', 0};
UINT8 ATssCfnrcCode[] = {'6', '2', 0};
UINT8 ATssAllForwardingCode[] = {'0', '0', '2', 0};
UINT8 ATssAllCondForwardingCode[] = {'0', '0', '4', 0};
UINT8 ATssCwCode[] = {'4', '3', 0};
UINT8 ATssBaocCode[] = {'3', '3', 0};
UINT8 ATssBoicCode[] = {'3', '3', '1', 0};
UINT8 ATssBoicExhcCode[] = {'3', '3', '2', 0};
UINT8 ATssBaicCode[] = {'3', '5', 0};
UINT8 ATssBicroamCode[] = {'3', '5', '1', 0};
UINT8 ATssAllCallRestrictionCode[] = {'3', '3', '0', 0};
UINT8 ATssBarringOfOutgoingCode[] = {'3', '3', '3', 0};
UINT8 ATssBarringOfIncomingCode[] = {'3', '5', '3', 0};

const ATCodeToSsCode ssATCodeToSsCode[] =
{
    {ATssClipCode, SS_CLIP},
    {ATssClirCode, SS_CLIR},
    {ATssColpCode, SS_COLP},
    {ATssColrCode, SS_COLR},
    {ATssCnapCode, SS_CNAP},
    {ATssCfuCode, SS_CFU},
    {ATssCfbCode, SS_CFB},
    {ATssCfnryCode, SS_CFNRY},
    {ATssCfnrcCode, SS_CFNRC},
    {ATssAllForwardingCode, SS_ALLFORWARDINGSS},
    {ATssAllCondForwardingCode, SS_ALLCONDFORWARDINGSS},
    {ATssCwCode, SS_CW},
    {ATssBaocCode, SS_BAOC},
    {ATssBoicCode, SS_BOIC},
    {ATssBoicExhcCode, SS_BOICEXHC},
    {ATssBaicCode, SS_BAIC},
    {ATssBicroamCode, SS_BICROAM},
    {ATssAllCallRestrictionCode, SS_ALLBARRINGSS},
    //{ATssBarringOfOutgoingCode, SS_BARRINGOFOUTGOINGCALLS},
    {ATssBarringOfIncomingCode, SS_BARRINGOFINCOMINGCALLS},
    {(UINT8 *)NULL, (ss_code_enum)0}
};

static BOOL CallForwardingFlag = FALSE;
BOOL AT_IsDigit(INT8  CharValue)
{
    if ((CharValue >= '0') && (CharValue <= '9'))
        return  TRUE;
    else
        return  FALSE;
}
//INT8 at_branchbuf[80] ;//man
static BOOL AT_IsCCString( INT8 *s )
{
    INT8    len;
    UINT16  i;

    if(NULL == s)
    {
        AT_TC(g_sw_AT_SAT, "AT_IsCCString:ERROR!string is NULL");
        return  FALSE;;
    }

    len = strlen(s);
    AT_TC(g_sw_AT_SAT, "AT_IsCCString:s string is %s", s);

    if (0 == len)
        return  FALSE;
    len = strlen(s);

    // special case for #+number, it is a number, we just dial
    if ((s[0] == '#') && (s[len - 1] != '#'))
        return TRUE;

    if ((s[0] == '+') || AT_IsDigit (s[0]) || ((s[0] == '*') && (s[len - 1] != '#')))
    {
        for  (i = 1; i < len - 1; i++)
        {
            if (!(AT_IsDigit (s[i])
                    || s[i] == 'P'  //CHAR_DTMF ||   /* allow DTMF separator */
                    || s[i] == 'p'
                    || s[i] == '*'
                    || s[i] == '#'
                    || s[i] == 'w'
                    || s[i] == 'W'))  //man

                return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}
BOOL AT_GetCallInfo(CFW_CC_CURRENT_CALL_INFO *pCallInfo, UINT8  CallStatus, CFW_SIM_ID nSimID)
{
    CFW_CC_CURRENT_CALL_INFO    CallInfo[AT_MAX_CC_NUM];
    UINT8   Count = 0;
    BOOL    RtnValue = FALSE;
    UINT8   i;

    ASSERT(pCallInfo != NULL);
    memset(CallInfo, 0, AT_MAX_CC_NUM * sizeof(CFW_CC_CURRENT_CALL_INFO));
    RtnValue = CFW_CcGetCurrentCall(CallInfo, &Count, nSimID);
    AT_TC(g_sw_AT_SAT, "AT_GetCallInfo:Count:%d,rtnValue:%d", Count, RtnValue);
    if (ERR_SUCCESS ==  RtnValue )
    {

        if ((Count > 0) && (Count <= AT_MAX_CC_NUM))
        {
            for (i = 0; i < Count; i++)
            {
                if (CallStatus == CallInfo[i].status)
                {
                    *pCallInfo = CallInfo[i];
                    return  TRUE;

                }
            }
        }
    }

    return  FALSE;
}

BOOL AT_IsSSString( INT8 *s , CFW_SIM_ID nSimID)
{

    INT8 len;

    CFW_CC_CURRENT_CALL_INFO    CallInfo;
    BOOL        CallStarting;

    if(NULL == s)
    {
        AT_TC(g_sw_AT_SAT, "AT_IsSSString:ERROR!string is NULL");
        return  FALSE;;
    }
    if(AT_IsCCString(s))
    {
        return FALSE;
    }
    CallStarting = AT_GetCallInfo (&CallInfo, CFW_CM_STATUS_ACTIVE, nSimID);
    len = strlen( s );

    if (0 == len)
        return  FALSE;

    if ((len > 4) && (s[len - 1] == '#') && (s[len - 2] != '#') && (s[len - 2] != '*'))
        return TRUE;

    if ( ( (s[0] == '*') || (s[0] == '#') ) && (s[len - 1] == '#') && (s[len - 2] != '#')  && (s[len - 2] != '*'))
        return TRUE;

    if ((len < 3) && (len > 0))
    {
        /* There is at least one call active */
        if ( !CallStarting && s[0] == '1' )
            return FALSE;

        return TRUE;
    }

    return FALSE;
}

/*==========================================================================
 * This function scans the user input string from the position specified
 * and parses upto four * deleimited parameters.
 *
 * On entry it is assumed that the last character is a '#'
 *=========================================================================*/
static void BreaktItUp(UINT8 *pStr, UINT8 len, UINT8 pos, SSParsedParameters *pParsed)
{
    UINT8 parmNum = 0;

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);
    ASSERT(pStr[len - 1] == CHR_HASH);

    /* Pos points to the first parameter */
    pParsed->parmStart[parmNum] = pos;

    /* scan string until end of string or upto MAX_PARMS parameter parsed */
    /* or upto the first # */
    while ((parmNum < MAX_PARMS) && (pos <= len))
    {
        if (pStr[pos] == CHR_HASH)
        {
            /* abort scanning after updating the length */
            pParsed->parmLen[parmNum] = pos - pParsed->parmStart[parmNum];
            break;
        }
        else
        {
            /* Not terminator so check if delimitor */
            if (pStr[pos] == CHR_STAR)
            {
                /* It is a delimiter so update the length of the current parm*/
                pParsed->parmLen[parmNum] = pos - pParsed->parmStart[parmNum];
                /* advance pointer and increment the parm number*/
                parmNum++;
                pos++;
                /* and update start position of this parameter */
                if (parmNum < MAX_PARMS)
                {
                    pParsed->parmStart[parmNum] = pos;
                }
            }
            else
            {
                /* not a delimitor so increment pos */
                pos++;
            }
        }
    }
}

/*==========================================================================
 * Given a length terminated string, this routine parses it so that upto 4
 * parameters seperated by delimitors is extracted. The start of the string
 * can be one of the following:-
 *
 *                *                            SSMMI_STAR
 *                #                            SSMMI_HASH
 *                *#                           SSMMI_STARHASH
 *                **                           SSMMI_STARSTAR
 *                ##                           SSMMI_HASHHASH
 *                none of the above.           SSMMI_USS
 *
 * the rest of the string will consist of parameters delimited by a '*' and
 * will be terminated by a '#'.
 *
 * Assumptions on entry:-
 *
 *  1) The input string is not empty
 *  2) The last chr is # if len > 2
 *
 *=========================================================================*/
static UINT8 ParseUserInput(UINT8 *pStr, UINT8 len, SSParsedParameters *pParsed)
{
    UINT8 i, pos = 0;
    UINT8 resultFlag = 1;

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);
    ASSERT(len != 0);
    /* Check that the last chr is # if the length is > 2 */
    /*Correct assert, allow a user_len <=2*/
    //ASSERT((len <= 2) || ((len > 2) && (pStr[len - 1] == CHR_HASH)));

    /* make all parms empty */
    for (i = 0; i < MAX_PARMS; i++)
    {
        pParsed->parmStart[i] = 0;
        pParsed->parmLen[i] = 0;
    }

    /* Not empty so look at the beginning of the string .. treat special if len <=2 */
    if (len <= 2)
    {
        pParsed->type = SSMMI_USS;
        pParsed->parmStart[0] = pos;
        pParsed->parmLen[0] = len;
    }
    else
    {
        /* the string length is 3 or more */
        switch (pStr[pos])
        {
        case CHR_STAR:
            /* it could be * or *# or ** */
            pos++;
            if ((pStr[pos + 1] >= '0' && pStr[pos + 1] <= '9') || (pStr[pos] >= '0' && pStr[pos] <= '9'))
            {
                switch (pStr[pos])
                {
                case CHR_HASH:
                    pParsed->type = SSMMI_STARHASH;
                    pos++;
                    break;

                case CHR_STAR:
                    pParsed->type = SSMMI_STARSTAR;
                    pos++;
                    break;

                default:
                    pParsed->type = SSMMI_STAR;
                    break;
                }
                BreaktItUp(pStr, len, pos, pParsed);
            }
            else
            {
                resultFlag = 0;
            }
            break;

        case CHR_HASH:
            /* it could be # or ## */
            pos++;
            if ((pStr[pos + 1] >= '0' && pStr[pos + 1] <= '9') || (pStr[pos] >= '0' && pStr[pos] <= '9'))
            {
                if (pStr[pos] == CHR_HASH)
                {
                    pos++;
                    pParsed->type = SSMMI_HASHHASH;
                }
                else
                {
                    pParsed->type = SSMMI_HASH;
                }
                BreaktItUp(pStr, len, pos, pParsed);
            }
            else
            {
                resultFlag = 0;
            }
            break;

        default:
            /* It does not begin with a * or # so treat it as a uss message */
            pParsed->type = SSMMI_USS;
            pParsed->parmStart[0] = pos;
            pParsed->parmLen[0] = len;
            break;
        }
    }

    return resultFlag;
}


/*==========================================================================
 * This routine is used to check if the length terminated string specified
 * matches the null terminated string
 *=========================================================================*/
static BOOL CmpzStrToLStr(const UINT8 *pZStr, const UINT8 *pLStr, UINT8 strLength)
{
    BOOL retValue = TRUE; /* Assume they match */

    ASSERT(pZStr != NULL);
    ASSERT(pLStr != NULL);

    /* Check if both of same length */
    if (strlen(pZStr) == strLength)
    {
        /* the lengths are the same so they could be the same */
        while (strLength)
        {
            if (*pZStr != *pLStr)
            {
                /* Not same so the strings don't match */
                retValue = FALSE;
                break;
            }
            pZStr++;
            pLStr++;
            strLength--;
        }
    }
    else
    {
        /* Not of same length so set result to FALSE */
        retValue = FALSE;
    }

    /* return the result */
    return retValue;
}

/*==========================================================================
 * This routine converts a text string assumed to be a number into
 * a binary enum value. If non digit characters are encountered then
 * a false is returned otherwise a true.
 *=========================================================================*/
static BOOL ValidMmiBsCode(UINT8 *pBsCodeStr, UINT8 bsCodeLen, Adp_bs_code_enum *pMmiBsCode)
{
    UINT32 code = 0;
    BOOL retValue = TRUE; /* assume it is ok */

    ASSERT(pBsCodeStr != NULL);
    ASSERT(pMmiBsCode != NULL);
    ASSERT(bsCodeLen != 0);

    while (bsCodeLen != 0)
    {
        /* check if valid digit */
        if (!isdigit(*pBsCodeStr))
        {
            /* A non digit has been encountered */
            retValue = FALSE;
            break;
        }

        /* This is a digit */
        code = (code * 10) + (*pBsCodeStr - '0');

        if (code > 0xFFFF)
        {
            /* the bs code value is too big so flag it as illegal */
            retValue = FALSE;
            break;
        }

        /* advance pointers and decrement count */
        pBsCodeStr++;
        bsCodeLen--;
    }

    /* copy the decoded code into the output variable */
    *pMmiBsCode = (Adp_bs_code_enum)code;

    if (!((*pMmiBsCode >= 10 && *pMmiBsCode <= 25 && *pMmiBsCode != 14 && *pMmiBsCode != 15) || (*pMmiBsCode == 29) || (*pMmiBsCode == 0)))
        retValue = FALSE;

    AT_TC(g_sw_AT_SS, "ValidMmiBsCode *pMmiBsCode %d, %d", *pMmiBsCode, retValue);
    /* return the conversion status */
    return retValue;
}

/*==========================================================================
 * This routine copies the "UINT8" substring starting at
 * 'pSrc' and 'subLen' characters upto 'maxlen' long to the "UINT8" string
 * 'pDst[0]'. It also copies the value of 'subLen' into
 * the destination string length variable provided.
 *=========================================================================*/
static void CopySubString(UINT8 *pSrc, UINT8 subLen, UINT8 *pDst, UINT8 *dstLen, UINT8 maxLen)
{
    ASSERT(pSrc != NULL);
    ASSERT(pDst != NULL);
    ASSERT(dstLen != NULL);

    /* update the destiantion string length */
    *dstLen = subLen = (subLen > maxLen) ? maxLen : subLen;

    /* copy the substring */
    while (subLen != 0)
    {
        *pDst = *pSrc;
        pDst++;
        pSrc++;
        subLen--;
    }
}

/*==========================================================================
 * This routine is used to determine which of the operations listed below
 * the user mmi string should result in:
 *                     REGISTER,          pSSCode supplied
 *                     ERASE,             pSSCode supplied
 *                     ACTIVATE,          pSSCode supplied
 *                     DEACTIVATE,        pSSCode supplied
 *                     INTERROGATE,       pSSCode supplied
 *                     REG_PASSWORD,      pSSCode supplied
 *                     USSD
 *
 * If it is of the first 5 types then the sscode is also updated
 *=========================================================================*/
static void GetTypeOfOperation(UINT8 *pStr, SSParsedParameters *pParsedParameters, l4_op_code_enum *pOpCode, ss_code_enum *pSSCode)
{
    const ATCodeToSsCode *pTable = &ssATCodeToSsCode[0];
    BOOL found;
    UINT8 *servCode_p = NULL;
    UINT8 servCodeLen;

    ASSERT(pStr != NULL);
    ASSERT(pParsedParameters != NULL);
    ASSERT(pOpCode != NULL);
    ASSERT(pSSCode != NULL);

    /* check if the user string prefix is ussd type */
    if (pParsedParameters->type == SSMMI_USS)
    {
        /* It is */
        *pOpCode = SS_OP_PROCESSUNSTRUCTUREDSS_DATA;
    }
    else
    {
        /* It is NOT but it could be if the service code is meaningless */
        /* so first look through the recognised service code table */
        found = FALSE;
        servCode_p = &pStr[pParsedParameters->parmStart[0]];
        servCodeLen = pParsedParameters->parmLen[0];

        /* search through table */
        while (pTable->pzMmiStr != NULL)
        {
            if (CmpzStrToLStr(pTable->pzMmiStr, servCode_p, servCodeLen) == TRUE)
            {
                /* A match has been found */
                found = TRUE;
                break;
            }
            /* increment the table pointer */
            pTable++;
        }

        if (found == TRUE)
        {
            /* A match found in the table so it is a recognised type */
            *pSSCode = pTable->SSCode;

            switch (pParsedParameters->type)
            {
            case SSMMI_STAR: /* activate or register */
                /* assume that it is activation */
                *pOpCode = SS_OP_ACTIVATESS;
                break;

            case SSMMI_HASH: /* deactivate */
                *pOpCode = SS_OP_DEACTIVATESS;
                break;

            case SSMMI_STARHASH: /* interrogate */
                *pOpCode = SS_OP_INTERROGATESS;
                break;

            case SSMMI_STARSTAR: /* register */
                *pOpCode = SS_OP_REGISTERSS;
                break;

            case SSMMI_HASHHASH: /* erase */
                *pOpCode = SS_OP_ERASESS;
                break;

            default:
                //The program can not go here.
                AT_TC(g_sw_AT_SS, "SS MMI Parsing should never reach here");
                ASSERT(0);
                break;
            }
        }
        else
        {
        }
    }

}

/*Invoke CSW function to set or guery call waiting.*/
static void ADP_SS_SetCallWaiting(l4_op_code_enum OpCode, BOOL BsCodePresent, Adp_bs_code_enum MmiBsCode,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    UINT32 ret;

    if (BsCodePresent == FALSE)
    {
        BsCodePresent = TRUE;
        MmiBsCode = Adp_TELEPHONY;
    }

    switch (OpCode)
    {
    case SS_OP_REGISTERSS:
    case SS_OP_ACTIVATESS:

        ret = CFW_SsSetCallWaiting(CW_ENABLE, MmiBsCode, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallWaiting()   CFW_SsSetCallWaiting  ret = %d", ret);
        }
        break;

    case SS_OP_ERASESS:
    case SS_OP_DEACTIVATESS:
        ret = CFW_SsSetCallWaiting(CW_DISABLE, MmiBsCode, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallWaiting()   CFW_SsSetCallWaiting  ret = %d", ret);
        }
        break;

    case SS_OP_INTERROGATESS:
        ret = CFW_SsQueryCallWaiting(MmiBsCode, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallWaiting()   CFW_SsQueryCallWaiting  ret = %d", ret);
        }
        break;

    default:
        //The program can not go here.
        AT_TC(g_sw_AT_SS, "ADP_SS_SetCallWaiting Error OpCode,%d", OpCode);
        break;
    }

}

/*Invoke CSW function to set or guery call forwarding.*/
static void ADP_SS_SetCallForward(l4_op_code_enum OpCode, ss_code_enum SSCode, BOOL BsCodePresent, Adp_bs_code_enum MmiBsCode, BOOL TimePresent, INT8 Time, UINT8 numberBCDBytesLen, UINT8 *pNumberBCD,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    CFW_SS_SET_CALLFORWARDING_INFO CallForward;
    UINT32 ret;
    memset(&CallForward, 0, sizeof(CFW_SS_SET_CALLFORWARDING_INFO));

    if (BsCodePresent == FALSE)
    {
        BsCodePresent = TRUE;
        MmiBsCode = Adp_TELEPHONY;
    }

    CallForward.nClass = MmiBsCode;

    AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward MmiBsCode = %d", MmiBsCode);
    AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward SSCode = 0x%x", SSCode);
    AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward OpCode = %d", OpCode);
    switch (SSCode)
    {
    case SS_CFU:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_UNCONDITIONAL;
        break;

    case SS_CFB:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_MOBILEBUSY;
        break;

    case SS_CFNRY:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_NOREPLY;
        break;

    case SS_CFNRC:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_NOTREACHABLE;
        break;

    case SS_ALLFORWARDINGSS:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_ALL_CALLFORWARDING;
        break;

    case SS_ALLCONDFORWARDINGSS:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_ALL_CONDCALLFORWARDING;
        break;

    default:
        CallForward.nReason = CFW_SS_FORWARDING_REASON_UNCONDITIONAL;
        break;
    }

    switch (OpCode)
    {
    case SS_OP_REGISTERSS:
    case SS_OP_ACTIVATESS:
        if (pNumberBCD != NULL)
        {
            CallForward.nNumber.pDialNumber = AT_MALLOC(numberBCDBytesLen);
            ASSERT(CallForward.nNumber.pDialNumber != NULL);
            memcpy(CallForward.nNumber.pDialNumber, pNumberBCD, numberBCDBytesLen);
        }
        CallForward.nNumber.nDialNumberSize = numberBCDBytesLen;
        CallForward.nNumber.nClir = 0;
        if (CallForwardingFlag == FALSE)
        {
            CallForward.nNumber.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
        }
        else
        {
            CallForward.nNumber.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
            CallForwardingFlag = FALSE;
        }

        if (TimePresent == TRUE)
        {
            CallForward.nTime = Time;
        }
        CallForward.nMode = CFW_SS_FORWARDING_MODE_REGISTRATION;
        ret = CFW_SsSetCallForwarding(&CallForward, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward()   CFW_SsSetCallForwarding  ret = %d", ret);
        }
        if (CallForward.nNumber.pDialNumber != NULL)
        {
            AT_FREE(CallForward.nNumber.pDialNumber);
            CallForward.nNumber.pDialNumber = NULL;
        }

        break;

    case SS_OP_ERASESS:
        CallForward.nMode = CFW_SS_FORWARDING_MODE_ERASURE;
        ret = CFW_SsSetCallForwarding(&CallForward, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward()   CFW_SsSetCallForwarding  ret = %d", ret);
        }
        break;

    case SS_OP_DEACTIVATESS:
        CallForward.nMode = CFW_SS_FORWARDING_MODE_DISABLE;
        ret = CFW_SsSetCallForwarding(&CallForward, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward()   CFW_SsSetCallForwarding  ret = %d", ret);
        }
        break;

    case SS_OP_INTERROGATESS:
        AT_TC(g_sw_AT_SS, "CFW_SsQueryCallForwarding reason = %d", CallForward.nReason);
        AT_TC(g_sw_AT_SS, "CFW_SsQueryCallForwarding mode = %d", CallForward.nMode);
        AT_TC(g_sw_AT_SS, "CFW_SsQueryCallForwarding class = %d", CallForward.nClass);
        AT_TC(g_sw_AT_SS, "CFW_SsQueryCallForwarding time= %d", CallForward.nTime);

        ret = CFW_SsQueryCallForwarding(CallForward.nReason, CallForward.nClass, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward()   CFW_SsQueryCallForwarding  ret = %d", ret);
        }
        break;

    default:
        //The program can not go here.
        AT_TC(g_sw_AT_SS, "ADP_SS_SetCallForward Error OpCode,%d", OpCode);
        break;
    }
}

/*Invoke CSW function to set or guery call barring.*/
static void ADP_SS_SetCallBarring(l4_op_code_enum OpCode, ss_code_enum SSCode, BOOL BsCodePresent, Adp_bs_code_enum MmiBsCode, UINT8 PasswordLength, UINT8 *pPassword,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    UINT16 nFac = 0;
    UINT8 nClass = 0;
    UINT32 ret = 0;
    ASSERT(pPassword != NULL);

    AT_TC(g_sw_AT_SS, "ADP_SS_SetCallBarring SSCode = %d", SSCode);
    switch (SSCode)
    {
    case SS_BAOC:
        nFac = CFW_STY_FAC_TYPE_AO;
        break;

    case SS_BOIC:
        nFac = CFW_STY_FAC_TYPE_OI;
        break;

    case SS_BOICEXHC:
        nFac = CFW_STY_FAC_TYPE_OX;
        break;

    case SS_BAIC:
        nFac = CFW_STY_FAC_TYPE_AI;
        break;

    case SS_BICROAM:
        nFac = CFW_STY_FAC_TYPE_IR;
        break;

    case SS_ALLBARRINGSS:
        nFac = CFW_STY_FAC_TYPE_AB;
        break;

    case SS_BARRINGOFINCOMINGCALLS:
        nFac = CFW_STY_FAC_TYPE_AC;
        break;

    default:
        break;
    }

    if (BsCodePresent == FALSE)
    {
        BsCodePresent = TRUE;
        MmiBsCode = Adp_TELEPHONY;
    }
    nClass = MmiBsCode;

    switch (OpCode)
    {
    case SS_OP_REGISTERSS:
    case SS_OP_ACTIVATESS:
        ret = CFW_SsSetFacilityLock(nFac, pPassword, PasswordLength, nClass, CFW_CM_MODE_LOCK, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallBarring()   CFW_SsSetFacilityLock  ret = %d", ret);
        }
        break;

    case SS_OP_ERASESS:
    case SS_OP_DEACTIVATESS:
        ret = CFW_SsSetFacilityLock(nFac, pPassword, PasswordLength, nClass, CFW_CM_MODE_UNLOCK, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallBarring()   CFW_SsSetFacilityLock  ret = %d", ret);
        }
        break;

    case SS_OP_INTERROGATESS:
        ret = CFW_SsQueryFacilityLock(nFac, nClass, nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "ADP_SS_SetCallBarring()   CFW_SsQueryFacilityLock  ret = %d", ret);
        }
        break;
    default:
        //The program can not go here.
        AT_TC(g_sw_AT_SS, "ADP_SS_SetCallBarring Error OpCode,%d", OpCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS REGISTER REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_RegisterREQ(UINT8 *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    BOOL MmiBsCodePresent = FALSE;
    Adp_bs_code_enum MmiBsCode;
    UINT8 PasswordLength = 0;
    UINT8 pwd[MAXPASSWORDLEN] = {0};
    BOOL TimePresent = 0;
    INT8 Time;
    UINT8 NumberBCD[AT_MAX_CC_ADDR_LEN] = {0};
    UINT8 BytesLenBCD = 0;

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);

    /*. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . */
    /* Copy the optional mmi basic service code */
    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in second parameter */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_REGISTERSS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        /* and will always be in the second paremeter */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in the third parameter */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_REGISTERSS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFNRY:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* The reply timeout is always in the fourth parameter */
        if (pParsed->parmLen[3] != 0)
        {
            /* A timer value has been supplied so try and decode it */
            TimePresent = TRUE;//ValidReplyTime(&pStr[pParsed->parmStart[3]], pParsed->parmLen[3], &Time);
        }
        else
        {
            TimePresent = FALSE;
            Time = 0;
        }
        if(SS_CFNRY == SSCode && Time == 0)
        {
            TimePresent = TRUE;
            Time = 5;
        }
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    case SS_CFU:
    case SS_CFB:
    case SS_CFNRC:
        /* For these BS is in the third parameter */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        memset(NumberBCD, 0, AT_MAX_CC_ADDR_LEN);
        /* The fwd to number is in the second parameter  */
        if (pParsed->parmLen[1] != 0)
        {
            /* A forwarding number has been supplied so try and bcd encode it */
            if (strncmp(&pStr[pParsed->parmStart[1]], "+", 1) == 0)
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1] + 1], pParsed->parmLen[1] - 1, &NumberBCD[0]);
                CallForwardingFlag = TRUE;
                BytesLenBCD = (pParsed->parmLen[1] - 1) / 2;
                BytesLenBCD += (pParsed->parmLen[1] - 1) % 2;
            }
            else
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &NumberBCD[0]);
                BytesLenBCD = pParsed->parmLen[1] / 2;
                BytesLenBCD += pParsed->parmLen[1] % 2;
            }

        }

        ADP_SS_SetCallForward(SS_OP_REGISTERSS, SSCode, MmiBsCodePresent, MmiBsCode, TimePresent, Time, BytesLenBCD, NumberBCD, nDLCI, nSimID);
        break;

    default:
        AT_TC(g_sw_AT_SS, "SS_RegisterREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS ERASE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_EraseREQ(UINT8 *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    BOOL MmiBsCodePresent = FALSE;
    Adp_bs_code_enum MmiBsCode;
    UINT8 PasswordLength = 0;
    UINT8 pwd[MAXPASSWORDLEN] = {0};

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);

    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_ERASESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_ERASESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFU:
    case SS_CFB:
    case SS_CFNRY:
    case SS_CFNRC:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }
        else
        {
            /* Basic Service code can be included in parm1 in CF erase */
            if (pParsed->parmLen[1] != 0)
            {
                /* A basic service code has been supplied so try and decode it */
                MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
            }
        }

        ADP_SS_SetCallForward(SS_OP_ERASESS, SSCode, MmiBsCodePresent, MmiBsCode, FALSE, 0, 0, NULL, nDLCI, nSimID);
        break;

    default:
        AT_TC(g_sw_AT_SS, "SS_EraseREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS ACTIVATE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_ActivateREQ(UINT8 *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    BOOL MmiBsCodePresent = FALSE;
    Adp_bs_code_enum MmiBsCode;
    UINT8 PasswordLength = 0;
    UINT8 pwd[MAXPASSWORDLEN] = {0};
    BOOL TimePresent = 0;
    INT8 Time;
    UINT8 NumberBCD[AT_MAX_CC_ADDR_LEN] = {0};
    UINT8 BytesLenBCD = 0;

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);

    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_ACTIVATESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_ACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFNRY:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* The reply timeout is always in the fourth parameter */
        if (pParsed->parmLen[3] != 0)
        {
            /* A timer value has been supplied so try and decode it */
            TimePresent = TRUE;//ValidReplyTime(&pStr[pParsed->parmStart[3]], pParsed->parmLen[3], &Time);
        }
        else
        {
            TimePresent = FALSE;
            Time = 0;
        }
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    /*Don't break here, WARNING, WARNING, WARNING, WARNING*/
    case SS_CFU:
    case SS_CFB:
    case SS_CFNRC:
        /* For these BS is in the third parameter */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        memset(NumberBCD, 0, AT_MAX_CC_ADDR_LEN);
        /* The fwd to number is in the second parameter  */
        if (pParsed->parmLen[1] != 0)
        {
            /* A forwarding number has been supplied so try and bcd encode it */
            if (strncmp(&pStr[pParsed->parmStart[1]], "+", 1) == 0)
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1] + 1], pParsed->parmLen[1] - 1, &NumberBCD[0]);
                CallForwardingFlag = TRUE;
            }
            else
            {
                SUL_AsciiToGsmBcd(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &NumberBCD[0]);
            }
            BytesLenBCD = pParsed->parmLen[1] / 2;
            BytesLenBCD += pParsed->parmLen[1] % 2;
        }
        ADP_SS_SetCallForward(SS_OP_ACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, TimePresent, Time, BytesLenBCD, NumberBCD, nDLCI, nSimID);
        break;

    default:
        AT_TC(g_sw_AT_SS, "SS_ActivateREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS DEACTIVATE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_DeactivateREQ(UINT8 *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    BOOL MmiBsCodePresent = FALSE;
    Adp_bs_code_enum MmiBsCode;
    UINT8 PasswordLength = 0;
    UINT8 pwd[MAXPASSWORDLEN] = {0};

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);

    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_DEACTIVATESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_DEACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFU:
    case SS_CFB:
    case SS_CFNRY:
    case SS_CFNRC:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallForward(SS_OP_DEACTIVATESS, SSCode, MmiBsCodePresent, MmiBsCode, FALSE, 0, 0, NULL, nDLCI, nSimID);
        break;

    default:
        AT_TC(g_sw_AT_SS, "SS_DeactivateREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/*==========================================================================
 * This function is used to process SS INTERROGATE REQ to SSCode related REQ.
 *=========================================================================*/
static void SS_InterrogateREQ(UINT8 *pStr, SSParsedParameters *pParsed, ss_code_enum SSCode,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    UINT32 ret;
    BOOL MmiBsCodePresent = FALSE;
    Adp_bs_code_enum MmiBsCode;
    UINT8 PasswordLength = 0;
    UINT8 pwd[MAXPASSWORDLEN] = {0};

    ASSERT(pStr != NULL);
    ASSERT(pParsed != NULL);

    switch (SSCode)
    {
    case SS_CW:
        /* For call waiitng the BS is in parm 1 */
        if (pParsed->parmLen[1] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &MmiBsCode);
        }

        /* Change to CSW request */
        ADP_SS_SetCallWaiting(SS_OP_INTERROGATESS, MmiBsCodePresent, MmiBsCode, nDLCI, nSimID);
        break;

    case SS_BAOC:
    case SS_BOIC:
    case SS_BOICEXHC:
    case SS_BAIC:
    case SS_BICROAM:
    case SS_ALLBARRINGSS:
    case SS_BARRINGOFOUTGOINGCALLS:
    case SS_BARRINGOFINCOMINGCALLS:
        /* The password is required for the call restriction services only */
        CopySubString(&pStr[pParsed->parmStart[1]], pParsed->parmLen[1], &pwd[0], &PasswordLength, MAXPASSWORDLEN);

        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }

        ADP_SS_SetCallBarring(SS_OP_INTERROGATESS, SSCode, MmiBsCodePresent, MmiBsCode, PasswordLength, pwd, nDLCI, nSimID);
        break;

    case SS_CFU:
    case SS_CFB:
    case SS_CFNRY:
    case SS_CFNRC:
    case SS_ALLFORWARDINGSS:
    case SS_ALLCONDFORWARDINGSS:
        /* For these BS is in parm 2 */
        if (pParsed->parmLen[2] != 0)
        {
            /* A basic service code has been supplied so try and decode it */
            MmiBsCodePresent = ValidMmiBsCode(&pStr[pParsed->parmStart[2]], pParsed->parmLen[2], &MmiBsCode);
        }
        ADP_SS_SetCallForward(SS_OP_INTERROGATESS, SSCode, MmiBsCodePresent, MmiBsCode, FALSE, 0, 0, NULL, nDLCI, nSimID);
        break;

    case SS_CLIP:

        ret = CFW_SsQueryClip(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "SS_InterrogateREQ()   CFW_SsQueryClip  ret = %d", ret);
        }
        break;

    case SS_CLIR:
        ret  = CFW_SsQueryClir(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "SS_InterrogateREQ()   CFW_SsQueryClir  ret = %d", ret);
        }
        break;

    case SS_COLP:
        ret = CFW_SsQueryColp(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "SS_InterrogateREQ()   CFW_SsQueryColp  ret = %d", ret);
        }
        break;

    case SS_COLR:
        ret = CFW_SsQueryColr(nDLCI, nSimID);
        if (ret != ERR_SUCCESS)
        {
            AT_TC(g_sw_AT_SS, "SS_InterrogateREQ()   CFW_SsQueryColr  ret = %d", ret);
        }
        break;

    default:
        AT_TC(g_sw_AT_SS, "SS_InterrogateREQ have any undeveloped SSCode %d", SSCode);
        break;
    }
}

/**************************************************************
  FUNCTION NAME     : SS_Adaption_ParseStringREQ
PURPOSE             : Parse SS string.
INPUT PARAMETERS    : mmi_ss_parsing_string_req_struct *pReq
 **************************************************************/
BOOL AT_SS_Adaption_ParseStringREQ(at_ss_parsing_string_req_struct *pReq,  UINT8 nDLCI, CFW_SIM_ID nSimID)
{
    UINT8 resultFlag = 1;
    SSParsedParameters Parsed;
    l4_op_code_enum OpCode = 0;
    ss_code_enum SSCode = 0;

    ASSERT(pReq != NULL);

    /* parse the mmi string */
    resultFlag = ParseUserInput(pReq->input, pReq->length, &Parsed);
    if (resultFlag == 1)
    {
        /* Determine the type of operation and process accordingly */
        GetTypeOfOperation(pReq->input, &Parsed, &OpCode, &SSCode);
        /* process accordingly */
        switch (OpCode)
        {
        case SS_OP_REGISTERSS:
            /*SS Register REQ, invoke CSW function.*/
            SS_RegisterREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_ERASESS:
            /*SS Erase REQ, invoke CSW function.*/
            SS_EraseREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_ACTIVATESS:
            /*SS Activate REQ, invoke CSW function.*/
            SS_ActivateREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_DEACTIVATESS:
            /*SS Deactivate REQ, invoke CSW function.*/
            SS_DeactivateREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_INTERROGATESS:
            /*SS Interrogate REQ, invoke CSW function.*/
            SS_InterrogateREQ(pReq->input, &Parsed, SSCode, nDLCI, nSimID);
            break;

        case SS_OP_REGISTERPASSWORD:
            resultFlag = 0;
            AT_TC(g_sw_AT_SS, "ParseStringREQ ERROR! don't support password!");
            /*GSM2.30 V4.11.0 page 15 states that only registartion shall be allowed*/
            /*this means that process only if * or ** prefix in string           */
            if ((Parsed.type == SSMMI_STAR) || (Parsed.type == SSMMI_STARSTAR))
            {
                /*SS RegPassword REQ, invoke CSW function.*/
                //SS_RegPasswordREQ(pReq->input, &Parsed, SSCode);
            }
            else
            {
                /*Indicate Parse string failed.*/
                //SS_Adaption_SendParseStringRSP(PARSE_STRING_FAILED);
            }
            break;

        case SS_OP_PROCESSUNSTRUCTUREDSS_DATA:
            resultFlag = 0;
            AT_TC(g_sw_AT_SS, "ParseStringREQ ERROR! don't support USSD!");
            /*SS ProcUss REQ, invoke CSW function.*/
            //resultFlag = SS_ProcUssReq(pReq->input, pReq->length);
            //SS_Adaption_SendParseStringRSP(resultFlag);
            break;

        default:
            /* this should never happen */
            resultFlag = 0;
            AT_TC(g_sw_AT_SS, "AT_SS_Adaption_ParseStringREQ default resultFlag = %d", resultFlag);
            ASSERT(0);
            break;
        }
    }
    else
    {
        resultFlag = 0;
        //SS_Adaption_SendParseStringRSP(resultFlag);
    }

    return (BOOL)resultFlag;
}



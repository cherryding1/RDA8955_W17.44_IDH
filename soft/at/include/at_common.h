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

#ifndef __AT_COMMON_H__
#define __AT_COMMON_H__

#include "cos.h"
#include "cfw.h"
#include "at_cmd_engine.h"
#include "at_parse.h"

// frank add start:USE TO 直播星: 如果不定义，就是正常模式

// if define this macro that the CSW layer two sim ,but the AT layer single sim.
extern UINT8 gAtSimID;

#define AT_BACKSIMID(a)
#define AT_SIM_ID(channel) at_ModuleGetChannelSimID(channel)
#define AT_ASYN_GET_DLCI(sim) at_ModuleGetSimDlci(sim)

#define AT_CMD_CR 0x0D     // the ASCII value of <CR>
#define AT_CMD_LF 0x0A     // the ASCII value of <LF>

extern HANDLE COS_GetDefaultATTaskHandle(VOID);

#define CSW_AT_TASK_HANDLE COS_GetDefaultATTaskHandle()

#define MAX_DLC_NUM 10

#define ATE_RING_TIME_ELAPSE 1         // 1s
#define ATE_EXTENSION_TIME_ELAPSE 2    // 2s
#define AT_DEFAULT_ELAPSE 40           // 40s
#define ATE_OTA_CHECK_ELAPSE (20 * 60) // 20min
#define ATE_CCED_ELAPSE 10             //10s

// +CMER mode/////////
#define AT_CMER_ONLY_BUF_MODE 0
#define AT_CMER_DATA_DIS_MODE 1
#define AT_CMER_DATA_BUF_MODE 2
#define AT_CMER_DMA_MODE 3
// //////////////////////

/* ATL_CmdMainFunc return code */
#define CMD_FUNC_SUCC 0      // command sync success
#define CMD_FUNC_SUCC_ASYN 1 // command async success,
// need to be ended by async event
#define CMD_FUNC_FAIL 2                // command fail
#define CMD_FUNC_CONTINUE 3            // command need to be continue
#define CMD_FUNC_WAIT_SMS 4            // waiting for sms
#define CSW_IND_NOTIFICATION 5         // indication from csw
#define CMD_FUNC_WAIT_IP_DATA 6        // waiting for ip data

/*  DCE Result code             */
#define CMD_RC_OK 0         // "OK"
#define CMD_RC_CONNECT 1    // "CONNECT"
#define CMD_RC_RING 2       // "RING/CRING"
#define CMD_RC_NOCARRIER 3  // "NO CARRIER"
#define CMD_RC_ERROR 4      // "ERROR"
#define CMD_RC_NODIALTONE 5 // "NO DIALTONE"
#define CMD_RC_BUSY 6       // "BUSY"
#define CMD_RC_NOANSWER 7   // "NO ANSWER"
#define CMD_RC_NOTSUPPORT 8 // "NOT SUPPORT"
#define CMD_RC_INVCMDLINE 9 // "INVALID COMMAND LINE"
#define CMD_RC_CR 10        // "\r\n"
#define CMD_RC_SIMDROP 11   // "Sim drop"

#define CMD_ERROR_CODE_OK 255
#define CMD_ERROR_CODE_TYPE_CME 0
#define CMD_ERROR_CODE_TYPE_CMS 1

#define CMD_CMEE_ERROR 0
#define CMD_CMEE_ERROR_CODE 1
#define CMD_CMEE_ERROR_TEXT 2

// 2: for supportting GBK and USC2 character set
#define AT_CMD_LINE_BUFF_LEN (1024 * 5)

// /////////////////////////////////////////////
// Indicat to Short Message body max length,
// text mode and pdu mode are allowed
// add by wangqunyang 2008.05.23
// /////////////////////////////////////////////
#define AT_SMS_BODY_BUFF_LEN 176

// define for user transaction id size
// add by wangqunyang 2008.06.02
#define AT_USER_TRANS_ID_SIZE 199 // 0-199 for GSM; 200-255 is for GPRS

#define AT_DTMF_STRING_LEN  (20)

typedef enum {
    cs_gsm,
    cs_hex,
    cs_ucs2,
    cs_gbk, // equals to PCCP 936 char set
    cs_COUNT_,
    cs_INVALID_ = 0x8F,
} at_chset_t;

typedef struct _AT_CMD_PARA
{
    UINT8 *pPara;      // parameter for command, memory not hold by me
    UINT32 iExDataLen; // length of extra data
    UINT8 *pExData;    // extra data, memory not hold by me
    UINT8 iType;       // command type
    UINT32 uCmdStamp;  // command stamp
    UINT8 nDLCI;

    // Parsed parameters. "pParam" should be phased out.
    AT_CMD_ENGINE_T *engine;
    uint8_t paramCount;
    AT_COMMAND_PARAM_T *params[AT_COMMAND_PARAM_MAX]; // memory not hold by me
} AT_CMD_PARA;

typedef VOID (*pfn_AtCmdHandler)(AT_CMD_PARA *pParam);

typedef struct _AT_CMD_RESULT
{
    UINT32 uReturnValue;
    UINT32 uResultCode;
    UINT32 uErrorCode;
    UINT32 uCmdStamp; // command stamp
    UINT16 nDataSize;
    UINT8 nErrorType;
    UINT32 nDelayTime;
    AT_CMD_ENGINE_T *engine;
    UINT8 pData[80];
} AT_CMD_RESULT, *PAT_CMD_RESULT;

// //////////////////////////////////////
// AT Command Defination
// //////////////////////////////////////
// caoxh [mod] 16->20 2008-5-20
#define AT_CMD_NAME_SIZE 20
#define AT_CMD_PARA_SIZE 1024

#define AT_ZERO_PARAM1(pEv)      \
    do {                         \
        (pEv)->nParam1 = 0;      \
    } while (0)

typedef struct _AT_CMD
{
    UINT8 pName[AT_CMD_NAME_SIZE + 1];
    UINT8 pPara[AT_CMD_PARA_SIZE + 1];
    UINT8 iType;
    UINT32 uCmdStamp; // command stamp
    UINT8 pad[3];
    pfn_AtCmdHandler pCmdFunc;
    AT_CMD_ENGINE_T *engine;
    struct _AT_CMD *pNext;
} AT_CMD;

typedef struct _AT_IND
{
    AT_CMD_RESULT result;

    struct _AT_IND *pNext;
} AT_IND;

#define AT_WAITING_EVENT_MAX 20
struct ATWaitingEvent
{
    UINT32 nEvent;
    UINT8 nSim;
    UINT8 nDLCI;
    UINT8 enable;
};

extern struct ATWaitingEvent gAT_WaitingEvent[AT_WAITING_EVENT_MAX];

// caoxh [mod] 16->20 2008-5-20
#define AT_IND_NAME_SIZE 20
#define AT_IND_PARA_SIZE 80

// define for user transaction id table
// add by wangqunyang 2008.06.02
typedef struct _AT_UTI_TABLE
{
    UINT8 nUTI;
    INT32 nNextUTISpareTable;
} AT_UTI_TABLE;

#define AT_Notify2ATM(result, nDLCI) at_ModuleNotifyResult(result)

// Create the command result code
PAT_CMD_RESULT AT_CreateRC(UINT32 uReturnValue,
                           UINT32 uResultCode,
                           UINT32 uErrorCode,
                           UINT8 nErrorType,
                           UINT32 nDelayTime,
                           UINT8 *pBuffer,
                           UINT16 nDataSize,
                           UINT8 nDLCI);

extern VOID AT_CosEvent2CfwEvent(COS_EVENT *pCosEvent, CFW_EVENT *pCfwEvent);

// Drive ATM to RUNNING status.
extern VOID sync_ApWakeUpBpDetectDebounce(VOID);
void SendUA(BYTE bAdd);
void SendDM(BYTE bAdd);
bool SendDISC(BYTE nDLCI);

extern VOID ATE_ReceiveDateFromUartMux(VOID);
extern VOID ATE_ReceiveDateFromUart(VOID);
extern BOOL AT_InitCfw(COS_EVENT *pEvent);
extern BOOL AT_GETLPMode(); //for sleep

extern BOOL AT_GetCfwInitSmsInfo(CFW_EVENT *pCfwEvent);

// CSW Phone active status operations
#define AT_ACTIVE_STATUS_READY 0
#define AT_ACTIVE_STATUS_UNAVAILABLE 1
#define AT_ACTIVE_STATUS_UNKNOWN 2
#define AT_ACTIVE_STATUS_RINGING 3
#define AT_ACTIVE_STATUS_CALL_IN_PROGRESS 4
#define AT_ACTIVE_STATUS_ASLEEP 5
extern UINT32 AT_GetPhoneActiveStatus(UINT8 nSim);

// caoxh [+]2008-04-19
extern VOID AT_PppProcess(UINT8 nDLCI, UINT8 nSim, UINT8 nCid);

// starting add by wangqunyang 2008.04.25
UINT32 AT_SetCmeErrorCode(UINT32 nCfwErrorCode, BOOL bSetParamValid);
UINT8 *AT_Get_RetureValue_Name(UINT32 uReturnValue);
UINT8 *AT_Get_ResultCode_Name(UINT32 uResultCode);
VOID AT_Set_MultiLanguage();
BOOL AT_UnicodeBigEnd2Unicode(UINT8 *pUniBigData, UINT8 *pUniData, UINT16 nDataLen);
BOOL AT_Bytes2String(UINT8 *pDest, UINT8 *pSource, UINT8 *nSourceLen);
BOOL AT_String2Bytes(UINT8 *pDest, UINT8 *pSource, UINT8 *pLen);
VOID AT_InitUtiTable();
UINT32 AT_GetFreeUTI(UINT32 nServiceId, UINT8 *pUTI);
UINT16 AT_AllocUserTransID();
VOID AT_FreeUserTransID(UINT16 nUTI);
UINT8 GetHandsetInPhone(VOID);
VOID SetHandsetInPhone(UINT8 h_status);

// ending add by wangqunyang 2008.04.25

extern UINT8 BP_wakeup_AP_flag;

extern UINT16 SUL_GsmBcdToAsciiEx(
    UINT8 *pBcd, // input
    UINT8 nBcdLen,
    UINT8 *pNumber // output
    );
extern UINT16 SUL_AsciiToGsmBcdEx(
    INT8 *pNumber, // input
    UINT8 nNumberLen,
    UINT8 *pBCD // output should >= nNumberLen/2+1
    );

extern BOOL CFW_GetSATIndFormate(VOID);
UINT32 AT_OutputText(UINT16 nIndex, PCSTR fmt, ...);
//
//

#define I_AM_HERE()                                                           \
    do                                                                        \
    {                                                                         \
        AT_OutputText(1, "I am here %s %s:%d", __FILE__, __func__, __LINE__); \
    } while (0)

#endif

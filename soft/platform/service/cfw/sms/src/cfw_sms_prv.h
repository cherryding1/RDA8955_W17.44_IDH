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














#if !defined(__CFW_SMS_PRV_H__)
#define __CFW_SMS_PRV_H__

#include <sul.h>
#include <cswtype.h>
#include <errorcode.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <cfw_sim_prv.h>
#include <event.h>
#include <ts.h>
#include <fs.h>
#include <tm.h>
#include "cfw_sms_db.h"

//For displaying debug info
#define  SMS_DEBUG

#define SMS_INIT_GET_ME_PARA 1  //  1:遍沥ME上SMS, 0: 不遍沥.   ME init,just one time
#ifdef SMS_DEBUG
#define SMS_Disp_Debug_Info  CSW_TRACE
#define SMS_Disp_GetTime     TM_GetTime
#else
#define SMS_Disp_Debug_Info
#define SMS_Disp_GetTime
#endif
#define SMS_MO_ONE_PDU_SIZE     176     //use for malloc in compose PDU,for save a msg
#define SMS_MT_UTI              0x00    //User will set this value
#define SMS_MO_SEND_TEXT_SIZE   140     //Max size for sending a normal msg

//////////////////////////////////////////////////////////////////////////////////////
//Use FS to save long sms para or not
#define USING_FS_SAVE_LONG_PARA  0  // 0: don't use ; 1: use it

#define SMS_SUPPORT_PDU_LONG 1 //xueww[+] 2007.12.06 for AT to support long pdu
#define SMS_MAX_ENTRY_IN_ME     SMS_ME_MAX_INDEX  //max short message count in Flash 
//////////////////////////////////////////////////////////////////////////////////////


#define DECOMPOSE_TYPE____CFW_SMS_TXT_DELIVERED_NO_HRD_INFO       1  //SMS Text
#define DECOMPOSE_TYPE____CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO     2  //SMS Text
#define DECOMPOSE_TYPE____CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO       3  //SMS Text
#define DECOMPOSE_TYPE____CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO     4  //SMS Text
#define DECOMPOSE_TYPE____CFW_SMS_TXT_STATUS_IND                  5  //Status Report Text
#define DECOMPOSE_TYPE____CFW_SMS_TXT_COMMAND_INFO                6  //Command Text
#define DECOMPOSE_TYPE____CFW_SMS_TXT_CB_INFO                     7  //CB Text

#define DECOMPOSE_TYPE____CFW_SMS_PDU_INFO                        8  //SMS PDU
#define DECOMPOSE_TYPE____CFW_SMS_PDU_STATUS_REPORT_INFO          9  //Status Report PDU
#define DECOMPOSE_TYPE____CFW_SMS_PDU_CB_INFO                     10 //CB PDU

#define LONG_SMS_PARA_CREATE   1
#define LONG_SMS_PARA_ADD      2
#define LONG_SMS_PARA_DELETE   3

#define GSM_7BIT_UNCOMPRESSED 0x00
#define GSM_8BIT_UNCOMPRESSED 0x04  //xueww[mod] 2007.08.21  0x01--->0x04
#define GSM_UCS2_UNCOMPRESSED 0x08
#define GSM_7BIT_COMPRESSED   0x20 //xueww[mod] 2007.08.21  0x10--->0x20
#define GSM_8BIT_COMPRESSED   0x24 //xueww[mod] 2007.08.21  0x11--->0x24
#define GSM_UCS2_COMPRESSED     0x28 //xueww[mod] 2007.08.21 0x18--->0x28

//#define ERR_INVALID_SCA 0x14080003
//#define ERR_NO_NEED_SAVE      0x14080004
/**********************************************************************************/
/**********************************************************************************/

//Once got from event.h,erase all the followings


//Once got from cfw.h,erase all the followings
#define IS_GOT_API_SMSCB_RECEIVE_IND  1

///////////////////////////////////////////////////////////////////////////////
/*                          Private Data Structure                           */
///////////////////////////////////////////////////////////////////////////////

//SMS MT private data for longer msg
typedef struct _CFW_SMS_MT_LONGER_MSG
{
    UINT8   gIEIa;           //0:8-bit reference number; 8: 16-bit reference number
    UINT16  gLongerMsgID;    //Save the longer msg ID when receiving the first one of a longer msg
    UINT8   gLongerMsgCount;
    UINT8   gLongerMsgCurrent;
    UINT8   padding1;
    UINT16  gLongerMsgFirstIndex;
    UINT16  gLongerMsgSize;  //The total size of a longer msg
    UINT8   padding[2];
    UINT8  *pgBuf;           //Malloc the buffer
    UINT8  *pgBufBackup;     //Pointer to pgBuf
    UINT32  nTmp;
} CFW_SMS_MT_LONGER_MSG;

typedef struct _CFW_SMS_MT_LONGER_MSG_INDEX
{
    UINT16  gLongerMsgFirstIndex;
    UINT8   padding[2];
} CFW_SMS_MT_LONGER_MSG_INDEX;


typedef  enum _MSG_CLASS_TYPE
{
    MSG_CLASS_0 = 0,
    MSG_CLASS_1,
    MSG_CLASS_2,
    MSG_CLASS_3,
    MSG_CLASS_RESERVED,
} MSG_CLASS_TYPE;

typedef  enum _MSG_ALPHABET
{
    MSG_GSM_7 = 0,
    MSG_8_BIT_DATA,
    MSG_UCS2,
    MSG_RESERVED,
} MSG_ALPHABET;

typedef struct _CFW_SMS_INIT_HANDLE
{
    HAO   hAo;             //save the register handle

} CFW_SMS_INIT_HANDLE;


typedef struct _CFW_SMS_LONG_INFO_NODE
{
    UINT8  nStatus;
    UINT8  padding;
    UINT16 nConcatPrevIndex;
    UINT16 nConcatCurrentIndex;
    UINT16 nConcatNextIndex;
} CFW_SMS_LONG_INFO_NODE;

typedef struct _CFW_SMS_MO_INFO
{
    HAO   hAo;             //save the register handle
    UINT8 *pTmpData;       //For send long msg
    UINT8 *pTmpPara;       //For send long msg
    UINT8 *pTmpData_Addr;       //For send long msg
    UINT8 *pTmpPara_Addr;       //For send long msg
    api_SmsPPSendReq_t *pSendSmsReq; //For Send PDU, and normal text sms

    //The data set by user, now need to be saved in handle
    CFW_DIALNUMBER  sNumber;         //User input data
    UINT8          *pData;           //User input data

    UINT16  nDataSize;       //User input data
    UINT16  nLocation;
    UINT16  nIndex;
    UINT16  nUTI_UserInput;   //User input
    UINT16  nUTI_Internal;     //set for invoking SIM api
    UINT16  i;    //for sth

    UINT8  nStatus;
    UINT8  nPath;
    UINT8  nFormat; //0:PDU; 1:Text
    UINT8  nMR;

    UINT32 nErrCode;
    CFW_SMS_NODE *sOutData;  //add by qidd for read long msg[bug 13605]

    UINT16  nListCount;
    UINT16  nListStartIndex;
    UINT32  nListAddr;

    UINT16  nListSize;
    UINT8   nListOption;
    UINT8   isLongerMsg;    // 1:a longer msg; 0:a normal msg or the last part of a longer msg;

    CFW_SMS_LONG_INFO *pStru_SmsLongInfo; //for Write CFW_SMS_LONG_INFO to file
    UINT16  nLongerMsgID;     // The Msgs which belongs to a same longer msg have a same ID
    UINT8   nIEIa;            // 0:8-bit reference number; 8: 16-bit reference number
    UINT8   nLongerMsgCount;  // How many Msgs comprise a longer msg.

    UINT8   nLongerMsgCurrent;// The current sequence number of a longer msg
    UINT8   nLongerLastUserDataSize;
    UINT8   nLongerEachPartSize;
    UINT8   nLongerIndexOffset;

    UINT8   nReadSmsStatus;
    UINT8   nDCS;
    UINT8   nResendCnt; //hameina[+] for record SMS resend times.0: not yet been resend, 1,2,3..... resend times.CFW_SMS_MAX_RESEND_COUNT is defined for the max times
    UINT16  nDestIndex;
    UINT8   padding[1];

    CFW_EV  sCfw_Event;
} CFW_SMS_MO_INFO;

#define CFW_SMS_MAX_RESEND_COUNT 1

//SMS MT private data
typedef struct _CFW_SMS_MT_INFO
{
    HAO      hAo;

    UINT8   isLongerMsg;      // 1: the msg received is a longer msg; 0: not
    UINT8   nIEIa;            // 0:8-bit reference number; 8: 16-bit reference number
    UINT16  nLongerMsgID;     // The Msgs which belongs to a same longer msg have a same ID
    UINT8   nLongerMsgCount;  // How many Msgs comprise a longer msg.
    UINT8   nLongerMsgCurrent;// The current sequence number of a longer msg

    UINT8   nSmsStorageID;
    UINT8   nSRStorageID;
    UINT16  nLongerMsgMallocAddr;
    UINT16  nIndex;

    api_SmsPPReceiveInd_t *pInMsg; // Store the SmsPPReceiveInd

    UINT8  isBackupToSIM;  // non zero: if nSmsStorageID is CFW_SMS_STORAGE_ME, new sms will be backuped to SIM; 0: no
    UINT8  isListLongFirst;     //  non zero: if long msg, just list the first one; 0: if long msg,  list all
    UINT8  isRoutDetail;     //
    UINT8  nSmsType;  //0:STATUS REPORT; 1:DELIVER

} CFW_SMS_MT_INFO;

typedef struct _CFW_SMS_CB_INFO
{
    HAO    hAo;
    UINT16 nMID;
    UINT16 nTpUdl;
    UINT8 *pData;
    UINT16 nIndex;
    UINT8  nCBStorageID;
    UINT8  isRoutDetail;     //
} CFW_SMS_CB_INFO;
///////////////////////////////////////////////////////////////////////////////
//                Main Proc API Invoked by CFW_RegisterAo                        //
///////////////////////////////////////////////////////////////////////////////
UINT32 sms_SendProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_DleteAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_WriteAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_ReadAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_ListAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_ListHeaderAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_SetUnRead2ReadProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_SetRead2UnReadProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_SetUnSent2SentProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_CopyAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_MTAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_CBAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_InitAoProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SmsInitCompleteProc(HAO hAo, CFW_EV *pEvent);
UINT32 sms_MoveAoProc(HAO hAo, CFW_EV* pEvent);
///////////////////////////////////////////////////////////////////////////////
//                         Tool API For SMS Proc                            //
///////////////////////////////////////////////////////////////////////////////
HAO    sms_mt_Reg(CFW_EV *pEvent);
HAO    sms_cb_Reg(CFW_EV *pEvent);

UINT32 sms_mo_Parse_SmsPPErrorInd(HAO hAo, api_SmsPPErrorInd_t *pErrorInd);
UINT32 sms_mo_Parse_PDU(UINT8 nFormat, UINT8 nShow, UINT8 nStatus, UINT16 nStorage, PVOID pNode, UINT8 *pType, UINT32 *pDecomposeOutData, CFW_SMS_MULTIPART_INFO *pLongerMsg);
UINT32 sms_mo_InitMeLongMSGPara();
UINT32 sms_mt_Parse_SmsPPReceiveInd(HAO hAo);
UINT32 sms_mt_Parse_SmsPPReceiveInd_SR(HAO hAo);
UINT32 sms_mt_Parse_LongMSGPara(HAO hAo);
UINT32 sms_mt_Store_SmsPPReceiveInd(HAO hAo, api_SmsPPReceiveInd_t *pInMsg, CFW_EV *pMeEvent);
VOID   sms_mt_SerializeTime(TM_SMS_TIME_STAMP *pTimeStamp, CONST UINT8 *pSrc);
UINT32 sms_mt_SetLongPara(HAO hAo);
UINT32 sms_tool_DecomposePDU(PVOID pInData, UINT8 nType, UINT32 *pOutData, UINT8 *pSMSStatus, CFW_SMS_MULTIPART_INFO *pLongerMsg);

UINT32 sms_mo_SmsPPSendMMAReq(CFW_SIM_ID nSimId);
UINT32 sms_mt_SmsPPAckReq(CFW_SIM_ID nSimId);
UINT32 sms_mt_SmsPPErrorReq(UINT8 nCause, CFW_SIM_ID nSimId);
UINT32 sms_mo_ComposeLongPDU(CFW_SMS_MO_INFO *pSmsMOInfo, CFW_SIM_ID nSimId);
UINT32 sms_tool_ComposePDU(UINT8   Concat,  UINT8   SMSStatus, UINT8  *pData, UINT16  nDataSize, CFW_DIALNUMBER *pNumber, CFW_SIM_ID nSimId, CFW_EV *pEvent);
UINT32 SRVAPI CFW_SmsComposePdu(
    UINT8 *pTpUd,  // "ABC"
    UINT16 nTpUdl, // 3
    CFW_DIALNUMBER *pNumber, // telephone number to send, 135XXX,
    CFW_SMS_PDU_PARAM *pSmsParam,  // NULL
    UINT8 * *pSmsData,             // ouput need to maloc.
    CFW_SIM_ID nSimId,
    UINT16 *pSmsDataSize          // size
);

UINT32 sms_tool_SetLongMSGPara(UINT8 nStorage, PSTR pWriteBuf, INT32 nWriteBufSize, UINT8 nOption);
UINT32 sms_tool_Parse_LongMSGPara(CFW_SMS_LONG_INFO *pLongInfo, UINT16 nLongStruCount, UINT32 *pWriteBuf);
VOID   sms_tool_ASCII2BCD(UINT8 *pDst, CONST UINT8 *pSrc, UINT8 nSrcLength);
VOID   sms_tool_BCD2ASCII(UINT8 *pDst, CONST UINT8 *pSrc, UINT8 nSrcLength);

VOID   sms_tool_SMSTime_TO_SCTS(TM_SMS_TIME_STAMP SMSTime, UINT8 *SCTS);
UINT8 *sms_tool_SUL_AsciiToGsmBcd(CONST INT8 *pNumber, UINT8 Len, UINT8 *pBCD);
VOID   sms_tool_SUL_GsmBcdToAscii(UINT8 *pBCD, UINT8 Len, UINT8 *pNumber);

UINT32 sms_tool_CheckDcs(UINT8 *pDcs);//xueww[+] 2007.08.21
//                         APIs for Sync operation                           //
///////////////////////////////////////////////////////////////////////////////


#endif //


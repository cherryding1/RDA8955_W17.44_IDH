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



#ifndef __AT_CMD_SMS_H__
#define __AT_CMD_SMS_H__

#include <csw.h>
#ifdef AT_MODULE_SUPPORT_OTA
#include <ddl.h>
#include <jcal.h>
#include "jdi_defines.h"
#include <provisioncommonwrapper.h>
#include <jdi_provisionstructures.h>
#include <jdi_provisionapi.h>
#include <jdi_wapgsm.h>
#include <jdi_trfetch.h>
#include <jdi_communicator.h>
#endif
// //////////////////////////////////////////////////////
// reference of other module declares
// //////////////////////////////////////////////////////
struct COS_EVENT;
struct AT_CMD_PARA;

// //////////////////////////////////////////////////////
// struct defines of SMS
// //////////////////////////////////////////////////////
typedef struct _CFW_SMS_MO_INFO_ADD
{
    HAO hAo;  // save the register handle
    UINT8 *pTmpData;  // For send long msg
    UINT8 *pTmpPara;  // For send long msg
    UINT8 *pTmpData_Addr; // For send long msg
    UINT8 *pTmpPara_Addr; // For send long msg
    // api_SmsPPSendReq_t *pSendSmsReq; //For Send PDU
    // The data set by user, now need to be saved in handle
    CFW_DIALNUMBER sNumber; // User input data
    UINT8 *pData; // User input data
    UINT16 nDataSize; // User input data
    UINT16 nLocation;
    UINT16 nIndex;
    UINT16 nUTI_UserInput;  // User input
    UINT16 nUTI_Internal; // set for invoking SIM api
    UINT16 i; // for sth
    UINT8 nStatus;
    UINT8 nPath;
    UINT8 nFormat;  // 0:PDU; 1:Text
    UINT8 nMR;
    UINT32 nErrCode;
    UINT16 nListCount;
    UINT16 nListStartIndex;
    UINT32 nListAddr;
    UINT16 nListSize;
    UINT8 nListOption;
    UINT8 isLongerMsg;  // 1:a longer msg; 0:a normal msg or the last part of a longer msg;
    // CFW_SMS_LONG_INFO* pStru_SmsLongInfo; //for Write CFW_SMS_LONG_INFO to file
    UINT16 nLongerMsgID;  // The Msgs which belongs to a same longer msg have a same ID
    UINT8 nIEIa;  // 0:8-bit reference number; 8: 16-bit reference number
    UINT8 nLongerMsgCount;  // How many Msgs comprise a longer msg.
    UINT8 nLongerMsgCurrent;  // The current sequence number of a longer msg
    UINT8 nLongerLastUserDataSize;
    UINT8 padding[2];
    UINT8 nLongerEachPartSize;
    UINT8 nLongerIndexOffset;
    UINT8 nReadSmsStatus;
    UINT8 nDCS;

    // CFW_EV  sCfw_Event;
} CFW_SMS_MO_INFO_ADD;

// /////////////////////////////////////////////////////////////////////
// MACRO for SMS module
// ////////////////////////////////////////////////////////////////////
#define GSM_7BIT_UNCOMPRESSED 0x00
#define GSM_8BIT_UNCOMPRESSED 0x04
#define GSM_UCS2_UNCOMPRESSED 0x08

#define GSM_7BIT_COMPRESSED   0x20
#define GSM_8BIT_COMPRESSED   0x24
#define GSM_UCS2_COMPRESSED   0x28
#define CB_ACTIVE 0
#define CB_DEACTIVE 1

// /////////////////////////////////////////////////////////////////////
// Enterance and interface functions
// ////////////////////////////////////////////////////////////////////
UINT32 AT_SMS_INIT(UINT8 nSim);
VOID AT_SMS_AsyncEventProcess(COS_EVENT *pEvent);

// /////////////////////////////////////////////////////////////////////
// AT SMS Command functions
// ////////////////////////////////////////////////////////////////////
VOID AT_SMS_CmdFunc_CMMS(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMSS(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMGS(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMGW(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CPMS(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMGL(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CSCA(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CSMS(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMGD(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMGF(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CMGR(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CSDH(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CNMI(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CSMP(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CNMA(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CSAS(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CRES(AT_CMD_PARA *pParam);
VOID AT_SMS_CmdFunc_CSCB(AT_CMD_PARA *pParam);

// /////////////////////////////////////////////////////////////////////
// SMS async event  process functions
// ////////////////////////////////////////////////////////////////////
VOID AT_SMS_CMGS_CMSS_SendMessage_rsp(COS_EVENT *pEvent);
VOID AT_SMS_ListMessage_rsp(COS_EVENT *pEvent);
VOID AT_SMS_WriteMessage_rsp(COS_EVENT *pEvent);
VOID AT_SMS_CMSS_ReadMessage_rsp(COS_EVENT *pEvent);
VOID AT_SMS_CMGR_ReadMessage_rsp(COS_EVENT *pEvent);
VOID AT_SMS_DeleteMessage_rsp(COS_EVENT *pEvent);
VOID AT_SMS_RecvBuffOverflow_Indictation(COS_EVENT *pEvent);
VOID AT_SMS_RecvNewMessage_Indictation(COS_EVENT *pEvent);


VOID AT_SMS_MOInit_ReadMessage_rsp(COS_EVENT *pEvent);
// /////////////////////////////////////////////////////////////////////
// SMS utility functions declare
// ////////////////////////////////////////////////////////////////////
BOOL AT_SMS_StatusMacroFlagToStringOrData(UINT8 *pSetStatusBuff, UINT8 nFormat);
BOOL AT_SMS_StringOrDataToStatusMacroFlag(UINT8 *pStatusFlag, UINT8 *pGetSaveStatus, UINT8 nFormat);
BOOL AT_SMS_CheckPDUIsValid(UINT8 *pPDUData, UINT8 *nPDULen, BOOL bReadOrListMsgFlag);
BOOL AT_SMS_IsValidPhoneNumber(UINT8 *arrNumber, UINT8 nNumberSize, BOOL *bIsInternational);
UINT32 AT_SMS_ERR_Change(UINT32 nInErrcode);
BOOL SMS_PDUBCDToASCII(UINT8 *pBCD, UINT16 *pLen, UINT8 *pStr);
UINT8 AT_SMS_DCSChange(UINT8 InDCS, UINT8 OutDCS);

BOOL AT_CIND_SMS(UINT8 *pNewMsg, UINT8 *pMemFull, UINT8 nSim);
VOID AT_SMS_CDSTextReport(CFW_NEW_SMS_NODE *pNewMessageData, UINT8 nUTI, UINT8 nSim);
VOID AT_CC_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
VOID AT_SMS_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                      UINT8 nUTI, UINT8 nSim);
VOID AT_SMS_Result_Err(UINT32 uErrorCode, UINT8 *pBuffer, UINT16 nDataSize, UINT8 *pExtendErrInfo, UINT8 nUTI,
                       UINT8 nSim);
VOID AT_SMS_CDSPDUReport(CFW_NEW_SMS_NODE *pNewMessageData, UINT8 nUTI, UINT8 nSim);
BOOL SMS_AddSCA2PDU(UINT8 *InPDU, UINT8 *OutPDU, UINT16 *pSize, UINT8 nSim);
UINT16 SMS_PDUAsciiToBCD(UINT8 *pNumber, UINT16 nNumberLen, UINT8 *pBCD);
#ifdef AT_MODULE_SUPPORT_OTA

void jMms_comm_init ();

VOID AT_SMS_NewPushMessage(COS_EVENT *pEvent);

JC_RETCODE AT_HandlePushMsg (ST_COMM_PUSH_IND *pstPushData);

JC_RETCODE jdi_ProvisionDeinitialize (JC_HANDLE vHandle);

JC_RETCODE prv_DeInitCommunicator (void);

JC_RETCODE prv_CheckNETPINAuthentication(UINT8 * pIMSIasc , UINT8 OutLen);

JC_RETCODE jdi_ProvisionAuthenticate (JC_HANDLE vHandle,JC_UINT8 *pucPIN,JC_UINT32 uiDataLen);

JC_RETCODE prv_HandleContinuousAndBootstrapProvisioning (void);

JC_RETCODE prv_FillPxLogicalInfo (ST_PROV_PXLOGICAL *pstPxLogic,ST_PROV_INFO_LIST **ppstProfList);

JC_RETCODE prv_FillPxPhysicalInfo (ST_PROV_PXPHYSICAL *pstPxPhys,ST_PROV_INFO_LIST **ppstProfList);

JC_RETCODE prv_FillNapDefInfo (ST_PROV_NAPDEF *pstNapDef, ST_PROV_INFO_LIST **ppstProfList);

JC_RETCODE prv_FillApplicationInfo (ST_PROV_INFO_LIST **ppstProfList) ;

JC_RETCODE prv_DeInitContext (void);

JC_RETCODE prv_HeaderIndication (ST_COMM_HEADER_IND *pstHeaderIndication);

JC_RETCODE prv_CBCommProvIndication(void *pvAppArg,E_COMM_INDICATION eCommIndicationType,void *pvIndication);

JC_RETCODE prv_AppendHeader (const JC_UINT8 *pucHeader, const JC_UINT32 uiHeaderLen, const JC_BOOLEAN bIsHTTPHeader, const JC_UINT32 uiRequestID);

void prv_AddToProvList(ST_COMM_PUSH_IND *pstProvPushData);

void prv_ProcessProvInfo(void);

void prv_DeleteProfList(void);

void ConvertIMSIToSemiOctet (const JC_UINT8 *pucIMISI, JC_UINT8 *pucIMSISemiOctet, JC_UINT8 *pucLength);

void prv_FormProfList(void);

void prv_HandleDownloadContinousProvData();

void prv_DownloadContinousProvData ();

void prv_ResetContinousProvdata();

void prv_GetNapDef (ST_PROV_VALUES *pstToNapid, ST_PROV_NAPDEF **ppstNapDef);

void prv_FillPortServiceInfo (ST_PROV_PORT *pstPortService, ST_PROV_INFO_LIST *pstProfile);

void prv_GetPxLogic (ST_PROV_VALUES *pstToProxy, ST_PROV_PXLOGICAL **ppstPxLogic);

void prv_updateMissingProfileInfo (DATACONN_PROFILE *pstDestProf);

void LogProfileList(void);

void DeleteHeadProvList(void);

void prv_DeleteAllCSProfiles(void);

void prv_FormCSProfiles(void);

void AT_GetApnCfg(UINT8 nSimID);

void prv_FormCSDataConnProfile(ST_PROV_INFO_LIST *pstProfNode);

void prv_AddToCSProfiles(void * pProfile, E_CS_PROFILE_TYPE eCSProfileType);

void prv_FormCSProxyProfile(ST_PROV_INFO_LIST *pstProfNode);

extern ST_PROV_CONTEXT gstProvContext ;
extern JC_HANDLE       hCommunicator; //hComm ;


#endif

#endif
INT32 SMS_GetStorageInfoEX(UINT16 *pUsedSlot, UINT16 *pMaxSlot, CFW_SIM_ID nSimId, UINT8 nStatus);
// ///////////////////////////////////////////////////////////////////////////////////////////////
// The end of the file
// ///////////////////////////////////////////////////////////////////////////////////////////////


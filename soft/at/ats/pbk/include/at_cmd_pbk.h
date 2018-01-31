/******************************************************************************************
    Copyright (C), 2002-2010, Coolsand. Co., Ltd.
    File name     : at_cmd_gc.c
    Author         : wangqunyang
    Version        : 2.0
    Date            : 2008.03.20
    Description   : This file implements AT General Command functions
    Others         :
    History         :

   1.        Date   : 2007/11/25
           Author   : Felix
      Description  : build this file
          Version   : V1.0

   2.        Date   : 2008.03.20
           Author   : wangqunyang
      Description  : modify some code and added comment
          Version   : V2.0

*******************************************************************************************/
#ifndef __AT_CMD_PBK_H__
#define __AT_CMD_PBK_H__

// ///////////////////////////////////////////
// This macro used for list pbk entry step, we
// can set any value if the csw system can
// upload with different steps
// ///////////////////////////////////////////
#define   AT_PBK_LIST_ENTRY_STEP   20

//add start by frank
//for android FDN
#define AT_FDN_NAME_SIZE        SIM_PBK_NAME_SIZE +1
#define AT_FDN_NUMBER_SIZE  SIM_PBK_NUMBER_SIZE +1

typedef struct _AT_FDN_PBK_RECORD
{
    UINT8 nIndex;
    UINT8 nTpye;
    UINT8 nNumber[AT_FDN_NUMBER_SIZE];
    UINT8 nName[AT_FDN_NAME_SIZE];
} AT_FDN_PBK_RECORD;

typedef struct _AT_FDN_PBK_LIST
{
    UINT8 nFDNSatus;
    UINT8 nTotalRecordNum;
    UINT8 nRealRecordNum;
    UINT8 nCurrentRecordIndx;

    AT_FDN_PBK_RECORD sRecord[1];
} AT_FDN_PBK_LIST;
//add this two line from cfw/sim(API_GETPBK,CFW_SIM_REMAINLENGTH)
#define API_AT_GETPBK                       0
#define AT_SIM_REMAINLENGTH         14

typedef struct _AT_SIM_SM_PBK_STATE
{

    UINT8 nPreState;
    UINT8 nCurrState;
    UINT8 nNextState;
    UINT8 pad;
} AT_SIM_SM_PBK_STATE;
typedef struct _AT_FDN_UPDATERECORD
{
    UINT8 nRecordSize;
    UINT8 nCurrentFile;
    UINT8 nCurrRecInd;
    UINT8 pad;
    AT_SIM_SM_PBK_STATE nState;
} AT_FDN_UPDATERECORD;

//add end by frank

// /////////////////////////////////////////////////////////////////////
// Initial and interface functions
// ////////////////////////////////////////////////////////////////////
VOID AT_PBK_Init(UINT8 nSim);
VOID AT_PBK_AsyncEventProcess(COS_EVENT *pEvent);

// /////////////////////////////////////////////////////////////////////
// AT PBK Command functions
// ////////////////////////////////////////////////////////////////////
VOID AT_PBK_CmdFunc_CPBS(AT_CMD_PARA *pParam);
VOID AT_PBK_CmdFunc_CPBR(AT_CMD_PARA *pParam);
VOID AT_PBK_CmdFunc_CPBF(AT_CMD_PARA *pParam);
VOID AT_PBK_CmdFunc_CPBW(AT_CMD_PARA *pParam);
VOID AT_PBK_CmdFunc_CNUM(AT_CMD_PARA *pParam);

// /////////////////////////////////////////////////////////////////////
// PBK utility functions declare
// ////////////////////////////////////////////////////////////////////
VOID AT_PBK_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                      UINT8 nDLCI, UINT8 nSim);
VOID AT_PBK_Result_Err(UINT32 uErrorCode, UINT8 *pBuffer, UINT16 nDataSize, UINT8 *pExtendErrInfo, UINT8 nDLCI,
                       UINT8 nSim);
BOOL AT_PBK_ProcessInputName(UINT8 *arrDest, UINT8 nDestLen, UINT8 *arrSource, UINT8 *pLen);
BOOL AT_PBK_ProcessOutputName(UINT8 *arrDest, UINT8 nDestLen, UINT8 *arrSource, UINT8 *pLen);
BOOL AT_PBK_IsValidPhoneNumber(UINT8 *arrNumber, UINT8 nNumberSize, BOOL *bIsInternational);
BOOL AT_PBK_SetPBKCurrentStorage(UINT8 *pStorageFlag);
BOOL AT_PBK_GetPBKCurrentStorage(UINT16 nStorageId, UINT8 *pStorageBuff);
VOID AT_PBK_GetStorageInfoRequest(BOOL bCallStorageInofFuncFlag, UINT8 nDLCI, UINT8 nSim);

// /////////////////////////////////////////////////////////////////////
// PBK async event  process functions
// ////////////////////////////////////////////////////////////////////
VOID AT_PBK_ListPBKEntryRequest(UINT16 nIndexStart, UINT16 nIndexEnd, BOOL bContinueFlag, UINT8 nDLCI, UINT8 nSim);
VOID AT_PBK_AsyncEventProcess_GetPbkStorage(CFW_EVENT CfwEvent);
VOID AT_PBK_AsyncEventProcess_GetPbkStorageInfo(CFW_EVENT CfwEvent);
VOID AT_PBK_AsyncEventProcess_ListPbkEntry(CFW_EVENT CfwEvent);
VOID AT_PBK_AsyncEventProcess_DeletePbkEntry(CFW_EVENT CfwEvent);
VOID AT_PBK_AsyncEventProcess_AddPbkEntry(CFW_EVENT CfwEvent);
VOID AT_PBK_AsyncEventProcess_InitSaveMaxSize(CFW_EVENT CfwEvent);

extern AT_FDN_PBK_LIST *pgATFNDlist[CFW_SIM_COUNT];
VOID AT_FDN_Init(UINT8 nSimID);
BOOL AT_FDN_CompareNumber(UINT8 *pNumber, UINT8 nLen, UINT8 nType,UINT8 nSim);
VOID AT_FDN_SetStatus(UINT8 nSim);
VOID AT_FDN_UpdateRecord(UINT8 nIdx, UINT8 nRecordSize, UINT8 *pData, UINT8 nSim);
VOID AT_FDN_DelRecord(UINT8 nIdx, UINT8 nSim);
VOID AT_FDN_AddRecord(UINT8 nIdx, CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry, UINT8 nSim);

#endif

// ///////////////////////////////////////////////////////////////////////////////////////////////
// The end of the file
// ///////////////////////////////////////////////////////////////////////////////////////////////


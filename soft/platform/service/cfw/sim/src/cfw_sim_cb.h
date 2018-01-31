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

#ifndef CFW_SIM_CB_H
#define CFW_SIM_CB_H


#define BINARY_READ 0x00
/*
================================================================================
  Structure  : api_SmsCBPageInd_t
--------------------------------------------------------------------------------
  Direction  : API -> MMI
  Scope      : Used to indicate MMI that an SMS-CB page has been received
================================================================================
*/
typedef struct
{
    u8        DataLen;    // Page size, in byte
    u8        Data[1];    // In fact, DataLen elts must be allocated for this
    // array
} CB_PAGE_IND_STRUCT ;


typedef struct _CFW_CB_SIM_READ_REQ
{
    UINT8 Mode;
    UINT8 DCS;
    UINT8 FileId;
} CFW_CB_SIM_READ_REQ;
typedef struct _CFW_CB_ACTIVE_REQ
{
    UINT32 MsgId;
    UINT8  CbType;
} CFW_CB_ACTIVE_REQ;

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

#ifdef CB_SUPPORT
typedef struct _CFW_SMS_CB_MID_INFO
{
    UINT16  nMidList[20];
    UINT8  nSize;
    SIM_SM_STATE nState;
} CFW_SMS_CB_MID_INFO;
#endif

UINT32 CFW_CbSimRead( UINT8 nMode, UINT8 FileId, UINT16 nUTI, CFW_SIM_ID nSimID);

UINT32 CFW_CbAct(UINT16 nUTI, CFW_SIM_ID nSimID);

#ifdef CB_SUPPORT
UINT32 CFW_SimSetMidList(UINT16 *pMidList, UINT8 nCnt, UINT16 nUTI, CFW_SIM_ID nSimID);
#endif
#endif //CFW_SIM_CB_H

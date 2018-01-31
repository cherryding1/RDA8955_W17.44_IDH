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

#ifndef CFW_SIM_SMS_H
#define CFW_SIM_SMS_H

#include "cfw_sim.h"




#define SMS_MODE_NEXT_RECORD                 0x02
#define SMS_MODE_PRE_RECORD                  0x03
#define SMS_MODE_ABSOLUTE                    0x04

#define SMS_STATUS_IDLE                      0x01
#define SMS_STATUS_READRECORD                0x02
#define SMS_STATUS_UPDATERECORD              0x03
#define SMS_STATUS_SEEKRECORD                0x04
#define SMS_STATUS_READBINARY                0x05
#define SMS_STATUS_UPDATEBINARY              0x06
#define SMS_STATUS_ELEMFILE                  0x07

#define SMS_PDU_LEN                            0xB0 //The length is 176 bytes.
#define CFW_SMS_STORED_STATUS_STORED_ALL    0x80

typedef struct _SIM_INFO_READSMS
{
    UINT8   n_PrevStatus;
    UINT8   n_CurrStatus;
    UINT8   nTryCount;              //If the Timer isn't existing,The value of
    //nTryCount will be 1.
    UINT16  nCurrentFile;           //Element file type
    UINT16  nIndex;
    UINT8   pattern[1];
} SIM_INFO_READSMS;
typedef struct _SIM_INFO_PATCH
{
    UINT8   n_PrevStatus;
    UINT8   n_CurrStatus;
    UINT8   nTryCount;              //If the Timer isn't existing,The value of
    //nTryCount will be 1.
    UINT16  nCurrentFile;           //Element file type
    UINT16  nIndex;
    UINT32 *pData;
    UINT8   pattern[1];
} SIM_INFO_PATCH;
typedef struct _SIM_INFO_WRITESMS
{
    UINT8   n_PrevStatus;
    UINT8   n_CurrStatus;
    UINT8   nTryCount;              //If the Timer isn't existing,The value of
    //nTryCount will be 1.
    UINT8   SeekPattern[SMS_PDU_LEN];         // seek pattern,just like "0x00";
    UINT16  nCurrentFile;           // elem file type
    UINT8   nIndex;                    // index which to write record
    UINT8   nFlag;
    UINT8   nReserveIndex;
    UINT8   pData[SMS_PDU_LEN];     // send to scl

} SIM_INFO_WRITESMS;

typedef struct _SIM_INFO_LISTSMS
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT16 nLocation;
    UINT8  SeekPattern[3];           // seek pattern,just like "0x00";
    UINT8  nDataSize;                // record length
    UINT8  nCurrentFile;             //elem file type
    UINT8  nCurrIndex;
    UINT16 nCount;
    UINT8  nCurrCount;
    UINT8  nStatus;
    UINT16 nStartIndex;

    UINT8  pattern[3];
} SIM_INFO_LISTSMS;


typedef struct _SIM_INFO_DELESMS
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT32 nLocation;
    UINT8  SeekPattern[3];            // seek pattern,just like "0x00";
    UINT8  nStatus;
    UINT16 nIndex;
    UINT8  nCurrentFile;              //elem file type
    UINT8  nCurrInd;
    UINT8  pData[176];
    UINT8  pattern[1];
} SIM_INFO_DELESMS;

typedef struct _SIM_INFO_READBINARY
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT8  nFileId;
    UINT8  nOffset;
    UINT8  nBytesToRead;
    UINT8  pattern[2];
} SIM_INFO_READBINARY;

typedef struct _SIM_INFO_UPDATEBINARY
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT8  nFileId;
    UINT8  nOffset;
    UINT8  nBytesToWrite;
    UINT8  pDate[256];
    UINT8  pattern[2];
} SIM_INFO_UPDATEBINARY;

typedef struct _SIM_INFO_SET_MR
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT8  nFileId;
    UINT8  nMR;
    UINT8  pattern[3];
} SIM_INFO_SET_MR;

typedef struct _SIM_INFO_GET_MR
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT8  nFileId;
    UINT8  pattern[4];
} SIM_INFO_GET_MR;


typedef struct _SIM_INFO_GetSMSP
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT8  nFileId;
    UINT8  pattern[4];
    UINT8  nTotalRecordNumber;
    UINT8  nCurrRecordNumber;
    UINT8  RecordLength;
    UINT8  nIndex;
    CFW_SIM_SMS_PARAMETERS *pNode;
} SIM_INFO_GetSMSP;

typedef struct _SIM_INFO_SETSMSP
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;

    UINT8  nFileId;
    UINT8  pattern[3];
    UINT8  nTotalRecordNumber;
    UINT8  nCurrRecordNumber;
    UINT8  RecordLength;
    UINT8  nIndex;
    UINT8  nArrayCount;
    CFW_SIM_SMS_PARAMETERS Node[1];
} SIM_INFO_SETSMSP;

typedef struct _SIM_INFO_GETSMSINFO
{
    UINT8   n_PrevStatus;
    UINT8   n_CurrStatus;
    UINT8   nTryCount;

    UINT8   nCurrentFile;            //elem file type
    UINT16  nTotalNum;               //The total SMS storage capacity of sim
    UINT32   nSpecialNum;             //The total number special SMS
    UINT8   nStatus;
    UINT8   nCurrInd;
    UINT8   pattern[4];              // seek pattern,just like "0x00";
} SIM_INFO_GETSMSINFO;

UINT32 CFW_SimReadMessageProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimWriteMessageProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimListMessageProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimDeleMessageProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimReadBinaryProc  (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimUpdateBinaryProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetMRProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimSetMRProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetSmsStorageInfoProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetSmsParametersProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimSetSmsParametersProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimSmsInitProc (HAO hAO, CFW_EV *pEvent);
UINT32 CFW_SimGetSmsTotalNumProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimPatchProc(HAO hAo, CFW_EV *pEvent);

#define GSM_7BIT_UNCOMPRESSED 0x00
#define GSM_8BIT_UNCOMPRESSED 0x01
#define GSM_UCS2_UNCOMPRESSED 0x08

#define GSM_7BIT_COMPRESSED   0x10
#define GSM_8BIT_COMPRESSED   0x11
#define GSM_UCS2_COMPRESSED   0x18


#endif //CFW_SIM_SMS_H


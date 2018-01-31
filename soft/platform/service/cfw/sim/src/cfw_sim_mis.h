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

#ifndef CFW_SIM_MIS_H
#define CFW_SIM_MIS_H

#include <cfw_sim_prv.h>



//MCC and MNC format
//---------------------------
// MCC digit 2	| MCC digit 1
// MNC digit 3	| MCC digit 3
// MNC digit 2	| MNC digit 1
//---------------------------

//write PLMNsel file for sim
//the format of PLMNsel consisted of many group as:
//three digits MCC and tow or three digits MNC

//write PLMNwAct file for usim
//the format of PLMNwAct consisted of many group as:
//three digits MCC and two or three digits MNC is the same at SIM and plus two bytes Access Technology Identifier
//here only process MCC and MNC
struct write_prefer_plmn
{
    UINT8 status;
    UINT8 index;    //from 1 to 255

    UINT8 EF;
    UINT8 step;
    UINT16 size;     //length of data read, only for USIM
    UINT16 file_size;    
    UINT8 PLMN[3];  //GSM BCD format
};

struct read_prefer_plmn
{
    UINT8 status;

    UINT8 EF;
    UINT8 step;
    UINT16 size;    //length of data read, only for USIM

    //INT16 index;
    UINT16 file_size;
    INT16 offset;   //holds the location to where data will write.
    UINT8* ret_data;
};


//The EFLP file hold some bytes, and one byte for one language.
//The size of EFLP is variable.
typedef struct _CFW_SIM_LP
{
    UINT8 nLPSize;
    UINT8 *pLP;
} CFW_SIM_LP;

EXPORT CFW_SIM_LP Sim_Lp;

//nNum is the size of Buffer as input,
//and as ouput it is the number of ECC
UINT32 CFW_SimGetECC(UINT8 *pBuffer, UINT8 *nNum);
VOID CFW_SimSetECC(UINT8 *pBuffer);
VOID CFW_USimSetECC(UINT8 *pBuffer, UINT16 length);

UINT32 CFW_SimGetLP(UINT8 *pBuffer, UINT16 *nNum);
UINT32 CFW_SimSetLP(UINT8 *pBuffer, UINT16 nNum);

UINT32 CFW_SimGetLP(UINT8 *pBuffer, UINT16 *nNum);
UINT32 CFW_SimSetLP(UINT8 *pBuffer, UINT16 nNum);

BOOL CFW_SimGetPS(VOID);
BOOL CFW_SimGetPF(VOID);

UINT32 CFW_SimSetPF(BOOL bValue);
UINT32 CFW_SimSetPS(BOOL bValue);
#define SIM_SPYSIMCARD_INTERVAL 3000 // 30000

//For Stage3
#define SM_SIMINITS3_INIT 0x00
#define SM_SIMINITS3_IDLE 0x01
#define SM_SIMINITS3_RESET 0x02
#define SM_SIMINITS3_MMI 0x03
#define SM_SIMINITS3_STACK 0x04
#define SM_SIMINITS3_ELEMENTARY 0x05
#define SM_SIMINITS3_READBINARY 0x06
#define SM_SIMINITS3_DEDICATED 0x07
#define SM_SIMINITS3_REHABILITATE 0x08
#define SM_SIMINITS3_READRECORD 0x09

typedef struct _SIM_SM_INFO_GETICCID
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;
    UINT8 pattern[1];
} SIM_SM_INFO_GETICCID;

//For Stage1
#define SM_SIMINITS1_INIT 0x00
#define SM_SIMINITS1_IDLE 0x01
#define SM_SIMINITS1_RESET 0x02
#define SM_SIMINITS1_MMI 0x03
#define SM_SIMINITS1_STACK 0x04
#define SM_SIMINITS1_ELEMENTARY 0x05
#define SM_SIMINITS1_READBINARY 0x06
#define SM_SIMINITS1_DEDICATED 0x07
#define SM_SIMINITS1_SELECTAPPICATION 0x08
#define SM_SIMINITS1_READRECORD 0x09

//For Old Starting procedure
#define SM_SIMINIT_INIT 0x00
#define SM_SIMINIT_IDLE 0x01
#define SM_SIMINIT_RESET 0x02
#define SM_SIMINIT_MMI 0x03
#define SM_SIMINIT_STACK 0x04
#define SM_SIMINIT_ELEMFILESTATUS 0x05
#define SM_SIMINIT_CARDTYPE 0x06
#define SM_SIMINIT_DEDICATED 0x07
#define SM_SIMINIT_REHABILITATE 0x08

//For GetPUCT
#define SM_GETPUCT_INIT 0x00
#define SM_GETPUCT_IDLE 0x01
#define SM_GETPUCT_READBINARY 0x02
#define SM_GETPUCT_MMI 0x03
#define SM_GETPUCT_STACK 0x04

typedef struct _CFW_SIMGETPUCT
{
    UINT8 nSm_GetPUCT_prevStatus;
    UINT8 nSm_GetPUCT_currStatus;
} CFW_SIMGETPUCT;

//For SetPUCT
#define SM_SETPUCT_INIT 0x00
#define SM_SETPUCT_IDLE 0x01
#define SM_SETPUCT_ELEMENTSTATUS 0x02
#define SM_SETPUCT_VERIFYCHV 0x03
#define SM_SETPUCT_UPDATEBINARY 0x04
#define SM_SETPUCT_MMI 0x05
#define SM_SETPUCT_STACK 0x06

typedef struct _CFW_SIMSETPUCT
{
    UINT8 iCurrency[3];
    UINT16 iEPPU;
    INT8 iEX;
    UINT8 pPin2[8];

    UINT8 nSm_SetPUCT_prevStatus;
    UINT8 nSm_SetPUCT_currStatus;
} CFW_SIMSETPUCT;

//add by wulc 2009 10-25
typedef struct _SIM_SM_INFO_GETSERVERPROVIDERNAME
{
    UINT8 nSize;
    UINT8 pad[3];
    SIM_SM_STATE nState;
} SIM_SM_INFO_GETSERVERPROVIDERNAME;
//add by wulc 2009 10-25

typedef struct _SIM_SM_INFO_GETPROVIDERID
{
    SIM_SM_STATE nState;
} SIM_SM_INFO_GETPROVIDERID;

typedef struct _SIM_SM_INFO_SETPREFOPREATORLIST
{
    UINT8 nOperatorList[255];
    UINT8  nSize;
    SIM_SM_STATE nState;
} SIM_SM_INFO_SETPREFOPREATORLIST;

typedef struct _SIM_SM_INFO_GETPREFOPREATORLIST
{
    UINT16 nSize;
    UINT8 nTotalTimes;
    UINT8 nReadTimes;
    UINT8 pad[4];
    UINT8 *pData;
    SIM_SM_STATE nState;
} SIM_SM_INFO_GETPREFOPREATORLIST;

typedef struct _SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM
{
    SIM_SM_STATE nState;
} SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM;

UINT32 CFW_SimResetProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimResetExProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimCloseMsg();

UINT32 CFW_SimMisGetProviderIdProc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimMisSetPrefListProc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimMisGetPrefListProc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimMisGetPrefListMAXNumProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetServiceProviderNameProc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimInitProc(HAO hAo, CFW_EV *pEvent);

#define CFW_SIM_MIS_IDLE 0x00
#define CFW_SIM_MIS_READBINARY 0x01
#define CFW_SIM_MIS_UPDATEBINARY 0x02
#define CFW_SIM_MIS_READELEMENT 0x03
#define CFW_SIM_MIS_RESET 0x04
#define CFW_SIM_MIS_PREREADBINARY 0x05

typedef struct _SIM_INFO_READBINARY
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;

    UINT8 nFileId;
    UINT16 nOffset;
    UINT8 nBytesToRead;
    UINT8 pattern[2];
} SIM_INFO_READBINARY;

typedef struct _SIM_INFO_READRECORD
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;

    UINT8 nFileId;
    UINT8 nRecordNum;
    UINT8 nMode;
    UINT8 nRecordSize;
    UINT8 pattern[1];
} SIM_INFO_READRECORD;

typedef struct _SIM_INFO_UPDATERECORD
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;

    UINT8 nFileId;
    UINT8 nRecordNum;
    UINT8 nMode;
    UINT8 nRecordSize;
    //UINT8  nPIN2Size; Remove by lixp for at issue
    //UINT8  nPIN2[8];

    UINT8 pData[256];
} SIM_INFO_UPDATERECORD;

#define SIM_MODE_NEXT_RECORD 0x02
#define SIM_MODE_PRE_RECORD 0x03
#define SIM_MODE_ABSOLUTE 0x04

#define SIM_STATUS_IDLE 0x01
#define SIM_STATUS_READRECORD 0x02
#define SIM_STATUS_UPDATERECORD 0x03
#define SIM_STATUS_SEEKRECORD 0x04
#define SIM_STATUS_READBINARY 0x05
#define SIM_STATUS_UPDATEBINARY 0x06
#define SIM_STATUS_ELEMFILE 0x07
#define SIM_STATUS_VERIFY 0x08

UINT32 CFW_SimReadRecordProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimReadRecordWithLenProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimUpdateRecordProc(HAO hAo, CFW_EV *pEvent);

VOID CFW_PLMNtoBCDEX(UINT8 *pIn, UINT8 *pOut, UINT8 *nInLength);

#define CFW_SIM_MIS_VERIFY 0x05
#define CFW_SIM_MIS_READRECORD 0x06
#define CFW_SIM_MIS_UPDATERECORD 0x07

#define CFW_SIM_ICCID_IDLE 0x00
#define CFW_SIM_ICCID_READBINARY 0x01
typedef struct _SIM_SM_INFO_SETACM
{
    UINT32 iACMValue;
    UINT8 nPIN2[8];
    UINT8 nPIN2Size;
    UINT8 nMark;
    UINT8 pad[2];
    SIM_SM_STATE nState;
} SIM_SM_INFO_SETACM;

typedef struct _SIM_SM_INFO_GETACM
{
    SIM_SM_STATE nState;
} SIM_SM_INFO_GETACM;

#define ACM_STATUS_IDLE 0x01
#define ACM_STATUS_READBINARY 0x02
#define ACM_STATUS_UPDATEBINARY 0x03
#define ACM_STATUS_VERIFYCHV 0x04
#define SMS_STATUS_GETACMMAX 0x01
#define SMS_STATUS_SETACMMAX 0x02
#define SMS_STATUS_VERIFYCHV 0x03

typedef struct _ACM_INFO_SETACMMAX
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;

    UINT32 iACMMaxValue;
    UINT8 pPin2[8];
    UINT8 nPinSize;
} ACM_INFO_SETACMMAX;

typedef struct _ACM_INFO_GETACMMAX
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;

    UINT8 pattern[1];
} ACM_INFO_GETACMMAX;
typedef struct _GET_FILE_STATUS_INFO
{
    UINT8 n_PrevStatus;
    UINT8 n_CurrStatus;
    UINT8 nTryCount;
    UINT8 nFileID;

} GET_FILE_STATUS_INFO;
UINT32 CFW_SimSetACMMaxProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetACMMaxProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimMisGetACMProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimMisSetACMProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetPUCTProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimSetPUCTProc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimGetICCIDProc(HAO hAo, CFW_EV *pEvent);

BOOL CFW_SimInitStage2(CFW_SIM_ID nSimID);
UINT32 CFW_SimInitStage3(CFW_SIM_ID nSimID);
UINT32 CFW_SimGetPUCT(UINT16 nUTI, CFW_SIM_ID nSimID);

UINT32 CFW_SimSetPUCT(
    CFW_SIM_PUCT_INFO *pPUCT,
    UINT8 *pPin2,
    UINT8 nPinSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimID);

UINT32 CFW_SimReset(
    UINT16 nUTI,
    CFW_SIM_ID nSimID);
UINT32 CFW_SimResetEx(
    UINT16 nUTI,
    CFW_SIM_ID nSimID, BOOL flag);

UINT32 CFW_SimSetACMMax(
    UINT32 iACMMaxValue,
    UINT8 *pPin2,
    UINT8 nPinSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimID);

UINT32 CFW_SimGetACMMax(
    UINT16 nUTI,
    CFW_SIM_ID nSimID);

UINT32 CFW_SimGetACM(UINT16 nUTI, CFW_SIM_ID nSimID);

UINT32 CFW_SimSetACM(
    UINT32 iCurValue,
    UINT8 *pPIN2,
    UINT8 nPinSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimID);

UINT32 CFW_CfgGetIMSI(UINT8 *pIMSI, CFW_SIM_ID nSimID);
UINT32 CFW_CfgSetIMSI(UINT8 *pIMSI, CFW_SIM_ID nSimID);
HAO SimSpySimCard(CFW_EV *pEvent);

//#ifdef CFW_EXTENDED_API
#if 0
// ---------------------
// MACROS & DEFINES
// ---------------------

#define CFW_WITH_SIM_ID(a, b) (a), (b)
#define CFW_NOTIFY_SIM_ID(a, b) (a) | ((b) << 24)

#define CFW_SIM_ERROR(pStatus) (!(((0x90 == pStatus->Sw1) && (0x00 == pStatus->Sw2)) || (0x91 == pStatus->Sw1)))

#define CFW_SIM_ASSERT_ERROR(isError, errorCode, event)                                                                                    \
    if (isError)                                                                                                                           \
    {                                                                                                                                      \
        UINT32 nUTI;                                                                                                                       \
        CFW_GetUTI(hAo, &nUTI);                                                                                                            \
        CFW_PostNotifyEvent(event, errorCode, 0, CFW_NOTIFY_SIM_ID(nUTI, simId), 0XF0);                                                    \
        CSW_TRACE(CFW_SIM_TS_ID | C_DETAIL, TSTXT("CFW SIM : ERROR n°0x%x, sending notification with event id 0x%x\n"), errorCode, event); \
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);                                                                                             \
        hAo = HNULL;                                                                                                                       \
        return errorCode;                                                                                                                  \
    }

#define CFW_SIM_SEND_RESULT(param1, param2, event)                                                                   \
    {                                                                                                                \
        UINT32 nUTI;                                                                                                 \
        CFW_GetUTI(hAo, &nUTI);                                                                                      \
        CFW_PostNotifyEvent(event, (UINT32)(param1), (UINT32)(param2), CFW_NOTIFY_SIM_ID(nUTI, simId), 0);           \
        CSW_TRACE(CFW_SIM_TS_ID | C_DETAIL, TSTXT("CFW SIM : SUCCESS, sending result with event id 0x%x\n"), event); \
        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);                                                                       \
        hAo = HNULL;                                                                                                 \
    } \

// ---------------------
// TYPES
// ---------------------

typedef struct
{
    SIM_SM_STATE nState;
    UINT8  nTryCount;
    UINT8  fileSize;
    UINT8  fileId;
    BOOL   update;

} SIM_SM_INFO_EF;

typedef VOID (*SIM_DATA_PARSER_EF_T)(   HAO,
                                        UINT8 *,
                                        api_SimReadBinaryCnf_t *,
                                        SIM_SM_INFO_EF *
                                        , CFW_SIM_ID
                                    );

// ---------------------
// FUNCTION DECLARATIONS
// ---------------------

UINT32 CFW_SimMisSetPrefListProcEX(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimMisGetPrefListProcEX(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimGetFileStatusProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimReadRecordProc (HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimUpdateRecordProc (HAO hAo, CFW_EV *pEvent);
UINT32 cfw_SimReadUpdateElementaryFileProc(HAO hAo, CFW_EV *pEvent);

#endif //  CFW_EXTENDED_API

#endif // CFW_SIM_MIS_H

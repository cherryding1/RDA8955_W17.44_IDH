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

#include <cswtype.h>
#include <cfw.h>



#define FLOW_CONTROL_ENABLED   0x01
#define FLOW_CONTROL_DISABLED  0x02

//#define CSW_USER_DBS
typedef struct _NW_SM_INFO
{
    UINT32 nMsgId;
    VOID *pMsgBody;
    UINT32 nIndex;
    UINT8  nNetMode;      // 0: automatic, 1: manual, 4: maual/automatic
    UINT8  nNetModeEx;    // net work mode last used
    UINT8  Rssi;          // received signal strength indication
    UINT8  Ber;           // channel bit error rate
    UINT32 nAreaCellId;   // area code and cell id
    CFW_NW_STATUS_INFO sStatusInfo; // status info
    CFW_NW_STATUS_INFO sGprsStatusInfo; // status info
    UINT8  nOperatorId[6];  // the current PLMN
    UINT8  nAttOperatorId[6]; // the PLMN that want to attached
    BOOL   bStaticAO;       // static ao ?
    BOOL   bReselNW;        // sel or resel
    UINT32 nCOPSUTI;        // UTI sent to at
    UINT32 nBandUTI;        // change band UTI
    UINT32 nAttachUTI;        // change attach UTI

    UINT8  nBand;           // the band frequence
#ifdef __MULTI_RAT__
    UINT8  nUBand;
#endif
    BOOL   bDetached;       // stoped or detached
    BOOL   bChangeBand;     // change band ?
    BOOL   bNetAvailable;   // net work available

    UINT16  nReselUTI;       // resel or sel UTI u8 from at
    BOOL   bRoaming;        // roaming ?
    UINT8  nGprsAttState;   // gprs state for attach or detach?

    UINT8  nAttach;                  //doing attach 0 : detach 1 : attach 0xFF nothing
    UINT8  nCsStatusIndCount;          // count the cs status from stack
    //add for gcf teset, 2013-11-28 wuys
    UINT8  DetachType;           // 1 Gprs, 2 non gprs, 3 combined ;
    UINT8  AttachType;           // 1 Gprs, 2 non gprs, 3 combined ;
    //add end
    BOOL  bTimerneed;           // timer need, true or false
    BOOL  bTimeDeleted;        // the timer deleted
    BOOL  bPowerOff;
    UINT8 nStpCause;           //0: no using; 1: flight mode; 2: Frequency Band
#ifdef CSW_USER_DBS
    UINT8 nStrtFlag;
#endif
    UINT8 nReListNum;
#ifdef LTE_NBIOT_SUPPORT
	UINT8  bOnlyAtt;
#endif
    COS_CALLBACK_FUNC_T func;
} NW_SM_INFO;
//FM:flight mode; FB:brequency band; DBS: 直播星;
typedef enum e_stop_cause
{
    STOP_CAUSE_NOMAL,
    STOP_CAUSE_FM,
    STOP_CAUSE_FB
#ifdef CSW_USER_DBS
    , STOP_CAUSE_DBS
#endif
#if defined(SIMCARD_HOT_DETECT)
    ,STOP_CAUSE_HOT_DETECT
#endif
    ,STOP_CAUSE_POWEROFF
} STOP_CAUSE;
typedef struct _NW_FM_INFO
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nTryCount;
    UINT8  nMode;
    UINT8  nStorageFlag;
} NW_FM_INFO;
typedef struct _NW_STORELIST_INFO
{
    CFW_StoredPlmnList p_StoreList[CFW_SIM_COUNT];
} CFW_STORELIST_INFO;


typedef enum
{
    LOCK_BCCH_START,
    LOCK_BCCH_PROCESS,
    LOCK_BCCH_END
} LOCK_BCCH_STATUS;

typedef struct _CFW_LOCK_BCCH
{
    BOOL bLockBCCH;
    LOCK_BCCH_STATUS bLockBCCHStatus;
} CFW_LOCK_BCCH;

typedef struct _CFW_XCPUTEST
{
    UINT32 para[6];
    UINT32 type;
    UINT8 nXcpuTest_currStatus;
    UINT8 nXcpuTest_nextStatus;
} CFW_XCPUTEST;

typedef struct
{
    u32      result;
    u32      para[4];
} CFW_XCPUTEST_RSP;
typedef struct _CFW_WCPUTEST
{
    UINT32 para[6];
    UINT32 type;
    UINT8 nWcpuTest_currStatus;
    UINT8 nWcpuTest_nextStatus;
} CFW_WCPUTEST;

typedef struct
{
    u32      result;
    u32      para[4];
} CFW_WCPUTEST_RSP;

typedef struct
{
   UINT16         Arfcn ;   // [0..1024]
   UINT16         Rssi  ;   // [0..120] dBm after L1_LOW_RLA  it's noise only
} NW_FREQ_INFO ;
typedef struct _NW_QSCANF_INFO
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    UINT8  nBand;
	UINT8  nAllBand;//1,is all band sweep frequency.2,had swept band850 and band1900,3,had swept band1800 and 900 
	UINT16  nFreqNb;
    NW_FREQ_INFO nFreqInfo[600] ;
} NW_QSCANF_INFO;

UINT32 CFW_SimPatchEX(UINT16 nFileID, UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 _SATTimerProc(UINT8 TimerID);
VOID _AutoCallTimerProc(CFW_SIM_ID nSimID);





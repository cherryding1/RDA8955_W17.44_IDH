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












#if !defined(__CFW_PRV_H__)
#define __CFW_PRV_H__

#ifdef HAVE_CFW_MFE_ONLY
BOOL SRVAPI CFW_QueryEvent(VOID);
#else

#ifdef HAVE_CFW_CONFIG
#include <cfw_config_prv.h>
#endif

#define CFW_SM_STATE_IDLE               0x00
#define CFW_SM_STATE_READY              0x01
#define CFW_SM_STATE_INIT               0x02
#define CFW_SM_STATE_WAIT               0x03// when CFW call SIM Management function, lead to enter to state.

#define CFW_SM_NW_STATE_XX              0x0F
#define CFW_SM_NW_STATE_STOP            0x10
#define CFW_SM_NW_STATE_SEARCH          0x11
#define CFW_SM_NW_STATE_IDLE            0x12
#define CFW_SM_NW_STATE_END_            0x1F

#define CFW_SM_CM_STATE_XX              0x1F
#define CFW_SM_CM_STATE_IDLE            0x20
#define CFW_SM_CM_STATE_MOPROC          0x21
#define CFW_SM_CM_STATE_ACTIVE          0x22
#define CFW_SM_CM_STATE_HELD            0x23
#define CFW_SM_CM_STATE_INMEETING       0x24
#define CFW_SM_CM_STATE_MEETINGHELD     0x25
#define CFW_SM_CM_STATE_MODISC          0x26
#define CFW_SM_CM_STATE_MTDISC          0x27
#define CFW_SM_CM_STATE_MTPROC          0x28
#define CFW_SM_CM_STATE_END_            0x2F

#define CFW_SM_SS_STATE_XX              0x2F
#define CFW_SM_SS_STATE_CONNECT         0x30
#define CFW_SM_SS_STATE_SIM             0x31
#define CFW_SM_SS_STATE_END_            0x3F

#define CFW_SM_SMS_STATE_XX             0x3F
#define CFW_SMS_MO_STATUS_SIM           0x40
#define CFW_SMS_MO_STATUS_FLASH         0x41
#define CFW_SMS_MO_STATUS_WAITACK       0x42
#define CFW_SMS_MO_STATUS_OUTPUT        0x43
#define CFW_SMS_MO_STATUS_ERROR         0x44
#define CFW_SM_SMS_STATE_END_           0x4F

#define CFW_SM_SIM_STATE_PINM           0x3F
#define CFW_SM_SIM_STATE_READ           0x40
#define CFW_SM_SIM_STATE_WRITE          0x41
#define CFW_SM_SIM_STATE_END_           0x42

#define CFW_SM_STATE_ERROR              0xFFFE
#define CFW_SM_STATE_UNKONW             0xFFFF

#define CFW_MOVE_SMS_STATE_IDLE         0
#define CFW_MOVE_SMS_STATE_READ         1
#define CFW_MOVE_SMS_STATE_WRITE        2
#define CFW_MOVE_SMS_STATE_DELETE       3
#define CFW_MOVE_SMS_STATE_END          4
#define CFW_XCPU_TEST_STATE_IDLE               0x00
#define CFW_XCPU_TEST_STATE_READY              0x01
#define CFW_WCPU_TEST_STATE_IDLE               0x00
#define CFW_WCPU_TEST_STATE_READY              0x01

typedef struct _CFW_EV
{
    UINT32 nEventId;
    UINT32 nTransId;  // only low 8bit is valid for all CFW applications.
    UINT32 nParam1;
    UINT32 nParam2;
    UINT8 nType;
    UINT8 nFlag;
    UINT8 padding[2];
} CFW_EV;

typedef UINT32 (*PFN_AO_PORC) (HAO hAo, CFW_EV *pEvent);
typedef HAO(*PFN_EV_IND_TRIGGER_PORC) (CFW_EV *pEvent);

#define SPECIAL_MESSAGE         (CFW_EV*)0xFFFFFFFF

/*=================================================================
Range of UTI: 0x00 ~ 0xFFFF
=================================================================*/
/*APP: 0x00 ~ 0xFF*/
#define APP_UTI_MIN  0x00
#define APP_UTI_MAX  0xFF

/*System reserved UTI value*/
#define APP_UTI_SHELL  0x0100
#define APP_UTI_URC    0x0101

/*CFW: 0x1000 ~ 0xCFFF*/
#define CFW_UTI_MIN  0x1000
#define CFW_UTI_MAX  0xEFFF
//#define CFW_UTI_MAX  0xF3C0
/*APP_Ex: 0xE000 ~ 0xFFFF*/
#define APP_EX_UTI_MIN  0xF000
//#define APP_EX_UTI_MIN  0xF3C1
#define APP_EX_UTI_MAX  0xFFFF
#define DISABLE_SERIAL_RX 0x1
#define ENABLE_SERIAL_RX 0x2

/*==================================================================
The detail range define for CFW

NW：        0x1000 ~ 0x100F
SIM：       0x1000 ~ 0x101F
CM：        0x1020 ~ 0x102F
SS：        0x1030 ~ 0x103F
SMS_MO：    0x1040 ~ 0x104F
SMS_MT：    0x1050 ~ 0x105F
SMS_CB：    0x1060 ~ 0x106F
PDP：       0x1070 ~ 0x107F
PSD：       0x1080 ~ 0x108F
CSD：       0x1090 ~ 0x109F

CFW_APP:    0x2000 ~ 0xEFFF

==================================================================*/
#define  NW_UTI_MIN   0x1000
#define  NW_UTI_MAX   0x100F

#define  SIM_UTI_MIN  0x1010
#define  SIM_UTI_MAX  0x101F

#define  CC_UTI_MIN   0x1020
#define  CC_UTI_MAX   0x102F

#define  SS_UTI_MIN   0x1030
#define  SS_UTI_MAX   0x103F

#define  SMS_MO_UTI_MIN   0x1040
#define  SMS_MO_UTI_MAX   0x104F

#define  SMS_MT_UTI_MIN   0x1050
#define  SMS_MT_UTI_MAX   0x105F

#define  SMS_CB_UTI_MIN   0x1060
#define  SMS_CB_UTI_MAX   0x106F

#define  PDP_UTI_MIN   0x1070
#define  PDP_UTI_MAX   0x107F

#define  PSD_UTI_MIN   0x1080
#define  PSD_UTI_MAX   0x108F

#define  CSD_UTI_MIN   0x1090
#define  CSD_UTI_MAX   0x109F

#define  CFW_APP_UTI_MIN   0x2000
#define  CFW_APP_UTI_MAX   0xEFFF

//
// FUNCTION CLASS.
//
// In the sight of the range of CFW_APP_UTI, the SYS now supports
// 256 internal CFW functions.
//
#define  CFW_FUCTION_SMS_SEND_ID    0
#define  CFW_FUCTION_PBK_READ_ID    1

// ...Others...

#define  GENERATE_INTERNAL_UTI(FUN_ID, UTI_EX)\
            (UINT16)( CFW_APP_UTI_MIN + (FUN_ID << 8) + ((UTI_EX >> 8) & 0x00FF) )

//
// When Posting a URC notify event, the parameter UTI in the event
// should be obtained by using the MACRO.
//
#define GENERATE_URC_UTI()   (APP_UTI_URC << 8)
#define GENERATE_SHELL_UTI() (APP_UTI_SHELL << 8)

// #define __221_UNIT_TEST__

//
// Base Service
//
BOOL SRVAPI CFW_ResetEventQueue(VOID);
BOOL SRVAPI CFW_GetEventQueue(CFW_EV *msg);

UINT32 SRVAPI CFW_PostNotifyEvent(UINT32 nEventId, UINT32 nParam1, UINT32 nParam2, UINT32 nUTI, UINT8 nType);

UINT32 CFW_CheckSimId(CFW_SIM_ID nSimId);
UINT32 SRVAPI CFW_SendSclMessage(UINT32 nMessageId, VOID *pMessasgeStruct, CFW_SIM_ID nSimId);

PVOID SRVAPI CFW_MemAllocMessageStructure(UINT16 nSize);

// Set Service Id for current AO.
UINT32 SRVAPI CFW_SetServiceId(UINT32 nServiceId);

//
// AOM
//

typedef UINT32 (*PFN_CFW_DEFAULT_PROC) (CFW_EV *);

//
// CAUTION:
// The following two functions are only used by System only,
// Don't referenced by CFW service.
//
UINT32 SRVAPI CFW_AoInit(VOID);
UINT32 SRVAPI CFW_AoExit(VOID);
UINT32 SRVAPI CFW_AoScheduler(PVOID);

UINT32 SRVAPI CFW_SetAoEventNotifyProc(HAO hAo, PVOID pUserData, PFN_CFW_EV_PORC pfnEvProc);

UINT32 SRVAPI CFW_SetAoEventNotifyProcEx(UINT16 nUTI, PVOID pUserData, PFN_CFW_EV_PORC pfnEvProc);
UINT32 SRVAPI CFW_RegSysDefaultProc(PFN_CFW_DEFAULT_PROC pfnProc);

//
// Used by CFW Service(CM, SIM, SMS,...)
//
HAO CFW_RegisterAo(UINT32 nServiceId, PVOID pUserData, PFN_AO_PORC pfnAoProc, CFW_SIM_ID nSimID);
UINT32 SRVAPI CFW_UnRegisterAO(UINT32 nServiceId, HANDLE hAo);

UINT32 CFW_RelaseUsingUTI(UINT32 nServiceId, UINT8 UTI);

UINT32 CFW_GetUsingUTI(HAO hAO, UINT8 *pUTI);

// Get AO Handle by index from 0 to GetAoCount().
// if return NULL, indicate there is not any AO existing by index.
UINT8 SRVAPI CFW_GetAoCount(UINT32 nServiceId, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_SetAoState(HAO hAo, UINT32 nState);
UINT32 SRVAPI CFW_GetAoState(HAO hAo);  // if hAo is NULL, retrieve the current AO state.
UINT32 SRVAPI CFW_SetAoNextState(HAO hAo, UINT32 nState);

//
// Set current AO state to next state used in XXAoProc.
// if hAO is NULL, indicate current AO.
//
UINT32 SRVAPI CFW_ChangeAoState(HAO hAo);
UINT32 SRVAPI CFW_SetUTI(HAO hAo, UINT16 nUTI, UINT8 mode); // mode: 0 表示APP 的 UTI, 1 表示是CFW的UTI, 2: 表示是Stack的UTI
UINT32 SRVAPI CFW_GetUTI(HAO hAo, UINT32 *pUTI);  // Get entitle UTI(necessary for CFW_PostNotifyEvent).
UINT32 SRVAPI CFW_UTIIsBusy(HAO hAo, UINT16 nUTI);  // Check UTI is busy(occupied by AO)
PVOID SRVAPI CFW_GetAoUserData(HAO hAo);  // if hAO is NULL, retrieve the current AO's user data registered.
UINT32 SRVAPI CFW_RegisterCreateAoProc(UINT32 nEventId, PFN_EV_IND_TRIGGER_PORC pfnProc);

//
// if nState = CFW_SM_STATE_UNKONW, this function ignore this parameter.
// if nIndex the default value is zero if support single instance application.
// index from 0 to GetAoCount().
//
// Used by CFW Service(CM, SIM, SMS,...)

HAO SRVAPI CFW_GetAoHandle(UINT8 nIndex, UINT32 nState, UINT32 nServiceId, CFW_SIM_ID nSimID);

#define  CFW_AO_PROC_CODE_SINGLE            1
// the event are processed one time, and don't need to process again in this state.
#define  CFW_AO_PROC_CODE_MULTIPLE          2
// continue process AO proc.
#define  CFW_AO_PROC_CODE_CONTINUE          2
// continue process AO proc.
#define  CFW_AO_PROC_CODE_CONSUMED          4
// consume the current event.
#define  CFW_AO_PROC_CODE_NOT_CONSUMED      8
// don't consume the current event.

UINT32 SRVAPI CFW_SetAoProcCode(HAO hAo, UINT32 nProcCode); // Set current AO process result code.

//
// Used by CFW Service(CM, SIM, SMS,...)
// AO <--> AO communication, normally used to notify other AO to change state or process event.
UINT32 SRVAPI CFW_PostAoEvent(HAO hAo, UINT32 nEventId, // event
                              UINT32 nParam1, // event parameter
                              UINT32 nParam2  // AO source you can access when event process.
                             );

BOOL SRVAPI RFM_MemInit(VOID *pMemPoolStart, UINT32 nMemPoolSize);
VOID SRVAPI CFW_SclTest();

//
// Used by AOM or System-layer modules(such as APP)
//
UINT32 SRVAPI CFW_ServicesInit();
UINT32 SRVAPI CFW_ServicesExit();

//
// CAUTION:
// Don't use the following interfaces by AOM or any other modules.
// They are only used by System when CFW startup.
//
UINT32 SRVAPI CFW_NwInit(VOID);
UINT32 SRVAPI CFW_CmInit(VOID);
UINT32 SRVAPI CFW_SsInit(VOID);

UINT32 SRVAPI CFW_SimGetSmsParameters(UINT8 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID);
BOOL   SRVAPI CFW_SimInitStage2(CFW_SIM_ID nSimID);
UINT32 SRVAPI CFW_SimInitStage3(CFW_SIM_ID nSimID);
UINT32 SRVAPI CFW_SmsMoInit(UINT16 nUti, CFW_SIM_ID nSimId);
UINT32 SRVAPI CFW_SimInit(BOOL bRstSim, CFW_SIM_ID nSimID);
CFW_SIM_ID CFW_GetSimCardID(HAO hAO);

UINT32 SRVAPI CFW_SmsMtInit(VOID);
UINT32 SRVAPI CFW_SmsCbInit(VOID);
UINT32 SRVAPI CFW_PbkInit(VOID);

//
// CAUTION:
// AO Internal Functions, don't referenced by other services.
//
UINT32 SRVAPI CFW_BaseInit();

BOOL   SRVAPI CFW_QueryEvent(VOID);

UINT32 SRVAPI CFW_ShellProc(CFW_EV *pEvent);
#endif

UINT32 SRVAPI CFW_CfgSetSmsFullForNew(UINT8 n, CFW_SIM_ID nSimID);
UINT32 SRVAPI CFW_CfgGetSmsFullForNew(UINT8 *pn, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_CfgGetDefaultSmsParam(CFW_SMS_PARAMETER *pInfo, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_CfgSetDefaultSmsParam(CFW_SMS_PARAMETER *pInfo, CFW_SIM_ID nSimID);


UINT32 SRVAPI CFW_CfgSetPbkCapacityInfo(UINT8 nStorageId, CFW_PBK_CAPACITY_INFO *pStorageInfo);

UINT32 SRVAPI CFW_CfgSetCalllogCapacityInfo(UINT8 nStorageId, CFW_PBK_CAPACITY_INFO *pStorageInfo);

UINT32 SRVAPI BAL_Run(CFW_EVENT *pEvent);

UINT32 SRVAPI CFW_CfgGetTimeParam(TM_FILETIME *pTM);
UINT32 SRVAPI CFW_CfgSetTimeParam(TM_FILETIME *pTM);

#include "cfw_sim_prv.h"

typedef struct _TM_PARAMETERS
{
    UINT32 iBase;
    UINT16 iFormat;
    INT8 iZone;
    UINT8 padding;
    UINT32 iRtcBase;
} TM_PARAMETERS;

UINT32 SRVAPI CFW_CfgSetTmParam(TM_PARAMETERS *pTmParam);

UINT32 SRVAPI CFW_CfgGetTmParam(TM_PARAMETERS *pTmParam);


#define CFW_SIM_INIT_EV_TYPE_BASE    0x00
#define CFW_SMS_INIT_EV_TYPE_BASE    0x10
#define CFW_PBK_INIT_EV_TYPE_BASE    0x20
#define CFW_SAT_INIT_EV_TYPE_BASE    0x30

#define SIM_INIT_EV_TYPE0            CFW_SIM_INIT_EV_TYPE_BASE
#define SIM_INIT_EV_TYPE1            CFW_SIM_INIT_EV_TYPE_BASE+1
#define SIM_INIT_EV_TYPE2            CFW_SIM_INIT_EV_TYPE_BASE+2

#define SMS_INIT_EV_OK_TYPE          CFW_SMS_INIT_EV_TYPE_BASE
#define SMS_INIT_EV_ERR_TYPE         CFW_SMS_INIT_EV_TYPE_BASE+1

#define PBK_INIT_EV_OK_TYPE          CFW_PBK_INIT_EV_TYPE_BASE
#define PBK_INIT_EV_ERR_TYPE         CFW_PBK_INIT_EV_TYPE_BASE+1
#define PBK_EXIT_EV_OK_TYPE          CFW_PBK_INIT_EV_TYPE_BASE+2
#define PBK_EXIT_EV_ERR_TYPE         CFW_PBK_INIT_EV_TYPE_BASE+3
//
// add by lixp
//
typedef struct _CFW_SMS_LONG_INFO
{
    UINT16 index;
    UINT8 nStatus;
    UINT8 nIEIa;
    UINT8 count;
    UINT8 current;
    UINT16 id;
    UINT32 *pNext;
} CFW_SMS_LONG_INFO;

typedef struct _SIM_SMS_INIT_INFO
{
    UINT16 usedSlot;
    UINT16 totalSlot;
    UINT16 readRecords;
    UINT16 unReadRecords;
    UINT16 sentRecords;
    UINT16 unsentRecords;
    UINT16 unknownRecords;
    UINT16 storageId;
    UINT16 nLongStruCount;
    UINT16 nCurrentIndex;
    CFW_SMS_LONG_INFO *pLongInfo;
}
SIM_SMS_INIT_INFO;

typedef struct _CFW_SMS_INIT_INFO
{
    UINT16 usedSlot;
    UINT16 totalSlot;
    UINT16 readRecords;
    UINT16 unReadRecords;
    UINT16 sentRecords;
    UINT16 unsentRecords;
    UINT16 unknownRecords;
    UINT16 storageId;
    UINT16 nLongStruCount;
    UINT8 padding[2];
    CFW_SMS_LONG_INFO *pLongInfo;
}
CFW_SMS_INIT_INFO;

typedef struct _EMOD_PARAMETERS
{
    UINT8 CC_Cause;
} EMOD_PARAMETERS;

typedef struct _BIND_SIM_NM
{
    UINT8 simNo[90];
    UINT8 pad[2];
} BIND_SIM_NM;

typedef struct _BIND_NW_NM
{
    UINT8 nwNo[15];
    UINT8 pad;
} BIND_NW_NM;

UINT32 SRVAPI Cfg_Emod_UpdateSIMNm(BIND_SIM_NM *simnm);
UINT32 SRVAPI Cfg_Emod_ReadNWNm(BIND_NW_NM *nwnm);
UINT32 SRVAPI Cfg_Emod_UpdateNWNm(BIND_NW_NM *nwnm);
UINT32 SRVAPI Cfg_Emod_ReadSign(BOOL *sim_Sign, BOOL *nw_Sign);
UINT32 SRVAPI Cfg_Emod_UpdateSign(BOOL sim_Sign, BOOL nw_Sign);

typedef enum
{

    CFW_SIM_OC_IDLE = 0x00, // OC : open and close
    CFW_SIM_OC_OPEN = 0x01, // OC : open and close
    CFW_SIM_OC_CLOSE = 0x02,  // OC : open and close
    CFW_SIM_OC_ERROR = 0x03 // OC : open and close
} CFW_SIM_OC_STATUS;

// ====================================================================================
// Get Sim Open Status Return Value:
// 0x00 No Sim Card
// 0x01 No send sim open
// 0x02 Has sent the sim open command
// ====================================================================================
#define SOS_NO_SIM            0x00
#define SOS_NO_SEND_SIM_OPEN  0x01
#define SOS_SENT_SIM_OPEN     0x02

UINT32 SRVAPI CFW_GetSimOpenStatus(CFW_SIM_ID nSimID);
VOID SRVAPI CFW_SetSimOpenStatus(UINT8 nStatus, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_CfgNwSetFM(CFW_COMM_MODE bMode, CFW_SIM_ID nSimID);
UINT32 SRVAPI CFW_CfgNwGetFM(CFW_COMM_MODE *bMode, CFW_SIM_ID nSimID);
UINT32 CFW_CfgNwSetIpr( UART_BAUDRATE bMode);
UINT32 CFW_CfgNwGetIpr( UART_BAUDRATE *bMode);

UINT32 SRVAPI CFW_GetSimID(HAO hAO, CFW_SIM_ID *pSimID);
typedef enum
{

    CFW_SIM_ABSENT = 0x00,
    CFW_SIM_NORMAL = 0x01,
    CFW_SIM_TEST = 0x02,
    CFW_SIM_ABNORMAL = 0x03,
    CFW_SIM_STATUS_END = 0x04
} CFW_SIM_STATUS;
VOID SRVAPI CFW_SetSimStatus(CFW_SIM_ID nSimID, CFW_SIM_STATUS nSimStatus);
CFW_SIM_STATUS CFW_GetSimStatus(CFW_SIM_ID nSimID);
VOID CFW_InitVar(CFW_SIM_ID nSimID);

UINT32 SRVAPI Cfg_Emod_ReadSIMNm(BIND_SIM_NM *simnm);

// [[hameina[+] 2009.4.2
// param radix: 10, output memery using %d
// 16, output memery using %x
// uMask:  trace ID defined in CSW
extern VOID CSW_TC_MEMBLOCK(UINT16 uMask, UINT8 *buffer, UINT16 len, UINT8 radix);

// Add by Wug for supportting the 4 frequency.
BOOL SRVAPI CFW_GetAutoCallFreqBand(UINT8 *pBand);

// ----------------------------------------------------------------------------
// author: Hameina
// this function called by CFW_PostNotifyEvent to output trace about message send from CSW to MMI.
// --------------------------------------------------------------------------
VOID TS_OutPutEvId(
    CFW_SIM_ID nSIMID,
    UINT32 EvID);


BOOL _SetAutoCallTiemr(UINT32 nElapse, CFW_SIM_ID nSimID);

VOID CFW_ReceiveStopStackCNF(CFW_SIM_ID nSimID);
UINT32 CFW_SendStartStackREQ(UINT32 nMsgId, VOID *pMessageBuf, CFW_SIM_ID nSimID);



// ]]hameina [+]
typedef enum
{
    CFW_SMS_NOINITED = 0x00,
    CFW_SMS_WAITING_MSG = 0x01,
    CFW_SMS_INITED = 0x02
} CFW_SMS_INIT_STATUS;
#endif   // _H_


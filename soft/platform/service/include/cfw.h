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























#ifndef _CFW_H_
#define _CFW_H_

//#define CFW_PBK_SYNC_VER
//
// the necessary header and pragma defines.
//
#include <stdio.h>
#include <dbg.h>
#include <cswtype.h>
#include <errorcode.h>
#include "cfw_multi_sim.h"
#include <event.h>
#include <tm.h>
#include <csw_mem_prv.h> //shawn
#include <pm.h>

#if defined(CFW_SERVICE_DEV_SINGLE_DEMO)  //CT_PLATFORM!=CT_JADE
#pragma comment (lib,"cfw_shell.lib")
#pragma message ("Linking with cfw_sim.lib")
#endif

#define CFW_CC_IND_UTI       0x01
#define CFW_SS_IND_UTI       0x02
#define CFW_NW_IND_UTI       0x03
#define CFW_SIM_IND_UTI      0x04
#define CFW_PBK_IND_UTI      0x05
#define CFW_MBX              MMI_MBX
#define CFW_SMS_PATH_GSM  0
#define CFW_SMS_PATH_GPRS 1


typedef struct _CFW_EVENT
{
    UINT32 nEventId;
    UINT32 nParam1;
    UINT32 nParam2;
    UINT16 nUTI;
    UINT8 nType;
    UINT8 nFlag;
} CFW_EVENT;

typedef UINT32 (*PFN_CFW_EV_PORC)(CFW_EVENT *pEvent, VOID *pUserData);

#define CFW_NW_SRV_ID         0     // Indicate the network management  Service
#define CFW_SIM_SRV_ID        1     // Indicate the SIM management  Service
#define CFW_CC_SRV_ID         2     // Indicate the Call management Service
#define CFW_SS_SRV_ID         3     // Indicate the SS Service
#define CFW_SMS_MO_SRV_ID     4     // Indicate the SMS MO management  Service
#define CFW_SMS_MT_SRV_ID     5     // Indicate the SMS MT management  Service
#define CFW_SMS_CB_ID         6     // Indicate the SMS CB management  Service

#define CFW_GPRS_SRV_ID       7
#define CFW_PDP_SRV_ID        7
#define CFW_PSD_SRV_ID        7

#define CFW_CSD_SRV_ID        8
#define CFW_EMOD_SRV_ID       9      // Engineering Mode
#define CFW_APP_SRV_ID        10
#define CFW_TIMER_SRV_ID      11

#define CFW_MAX_SRV_ID        (11*CFW_SIM_COUNT)


#define CFW_PBK_SRV_ID        CFW_APP_SRV_ID

#define CFW_SRV_NW_EV_RSP     (0)
#define CFW_SRV_CC_EV_RSP     (1)
#define CFW_SRV_SS_EV_RSP     (2)
#define CFW_SRV_SIM_EV_RSP    (3)
#define CFW_SRV_SMS_EV_RSP    (4)
#define CFW_SRV_PBK_EV_RSP    (5)
#define CFW_SRV_GPRS_EV_RSP   (6)
#define CFW_SRV_FAX_EV_RSP    (7)
#define CFW_SRV_EV_IND        (8)
#define CFW_SRV_EV_MAX        (8)

#define CFW_SRV_EV_UNKNOW     (0xFF)

#define CFW_TELNUMBER_TYPE_INTERNATIONAL 145  //International number
#define CFW_TELNUMBER_TYPE_NATIONAL      161  //National number.
#define CFW_TELNUMBER_TYPE_UNKNOWN       129  //Unknown number type (default)

//
// Basic Utility Macros
//
#define  CFW_SET_BIT(val, n)               ((val)|=(1<<n))
#define  CFW_CLR_BIT(val, n)               ((val)&=~(1<<n))
#define  CFW_IS_BIT_SET(val, n)            ((val) & (1<<n))
#define  CFW_IS_BIT_CLR(val, n)            (~((val) & (1<<n)))
#define  CFW_SWITCH_BIT(val, n)            ((val)^=(1<<n))
#define  CFW_GET_BIT(val, n)               (((val) >> n) & 1)

#define  CFW_CFG_RECEIVE_STORAGE(sms, bm, sr) ((sms)|(bm)|(sr))
#define  CFW_CFG_SET_ALARM_RECUR(mon, tue, wed, thu, fri, sta, sun ) ((1<<mon|1<<tue|1<<wed|1<<thu|1<<fri|1<<sta|1<<sun)&0xFE)
#define CFW_CFG_CMER(mode,keyp,disp,ind,bfr) ((mode)|((keyp)<<2)|((disp)<<4)|((ind)<<6)|((bfr)<<8))
#define CFW_CFG_CMER_MODE(cmer)               (cmer&0x03)
#define CFW_CFG_CMER_KEYP(cmer)               (cmer>>2&0x03)
#define CFW_CFG_CMER_DISP(cmer)               (cmer>>4&0x03)
#define CFW_CFG_CMER_IND(cmer)                (cmer>>6&0x03)
#define CFW_CFG_CMER_BFR(cmer)                (cmer>>8&0x03)

#define  TEL_NUMBER_MAX_LEN  21
typedef enum _UART_BAUDRATE
{
    UART_BAUD_RATE_2400 = 2400,
    UART_BAUD_RATE_4800 = 4800,
    UART_BAUD_RATE_9600 = 9600,
    UART_BAUD_RATE_14400 = 14400,
    UART_BAUD_RATE_19200 = 19200,
    UART_BAUD_RATE_28800 = 28800,
    UART_BAUD_RATE_33600 = 33600,
    UART_BAUD_RATE_38400 = 38400,
    UART_BAUD_RATE_57600 = 57600,
    UART_BAUD_RATE_115200 = 115200,
    UART_BAUD_RATE_230400 = 230400,
    UART_BAUD_RATE_460800 = 460800,
    UART_BAUD_RATE_921600 = 921600,
    UART_BAUD_RATE_1843200 = 1843200,
} UART_BAUDRATE;


#define CSW_ASSERT(expr) \
if (!(expr)) {                       \
    char assert_ret[64] = {0,}; \
    sprintf(assert_ret, "File:%s Line:%d ASSERT", __FILE__, __LINE__);  \
    DBG_ASSERT(0, assert_ret); \
    }

typedef struct _CFW_DIALNUMBER
{
    UINT8 *pDialNumber;
    UINT8 nDialNumberSize;
    UINT8 nType;
    UINT8 nClir;
    UINT8 padding[1];
} CFW_DIALNUMBER;

typedef struct _CFW_TELNUMBER
{
    UINT8 nTelNumber[TEL_NUMBER_MAX_LEN];
    UINT8 nType;
    UINT8 nSize;
    UINT8 padding[1];
} CFW_TELNUMBER;

UINT32 SRVAPI CFW_SetEventProc (
    UINT8 nServiceId,
    PVOID pUserData,
    PFN_CFW_EV_PORC pfnRspEvProc
);

#define BAL_PROC_DEFAULT           0
#define BAL_PROC_COMPLETE          1
#define BAL_PROC_CONTINUE          2
#define BAL_PROC_NOT_CONSUME       4
#define BAL_PROC_DONT_FREE_MEM     8 // Tell CFW don't free the EVETN struct.
#define BAL_PROC_UNKOWN_ERR        0

UINT32 SRVAPI CFW_BalInit (
    VOID
);

UINT32 SRVAPI CFW_BalSetEventPorc (
    PVOID pUserData,
    PFN_CFW_EV_PORC pfnProc
);

UINT32 SRVAPI CFW_IsFreeUTI (
    UINT16 nUTI,
    UINT32 nServiceId
);

UINT32 SRVAPI CFW_GetFreeUTI (
    UINT32 nServiceId,
    UINT8 *pUTI
);

BOOL SRVAPI CFW_BlockEvent (
    UINT32 event
);

BOOL SRVAPI CFW_UnBlockEvent (
    UINT32 event
);

UINT32 SRVAPI CFW_IsServiceAvailable (
    UINT32 nServiceId,
    UINT8 *pAvailable
);

UINT32 SRVAPI CFW_SendReqEvent (
    UINT32 nEventId,
    UINT32 nParam1,
    UINT32 nParam2,
    UINT8 nUTI
);

#define CFW_CONTROL_CMD_POWER_ON   0
#define CFW_CONTROL_CMD_POWER_OFF  1
#define CFW_CONTROL_CMD_RESTART    2
#define CFW_CONTROL_CMD_RESET      3
#define CFW_CONTROL_CMD_RF_OFF     4

typedef struct _CFW_SPN_INFO
{
    UINT8 nSPNameLen;
    UINT8 nSPNDisplayFlag;
    UINT8 nSpnName[17];
} CFW_SPN_INFO;
UINT32 SRVAPI CFW_ShellControl (
    UINT8 nCommand
);

//
// Configuration(CFG)  Management
//

// General Configuration
UINT32 SRVAPI CFW_CfgSetPhoneFunctionality(
    UINT8 nLevel,
    UINT8 nRst
);


UINT32 SRVAPI CFW_CfgGetPhoneFunctionality(
    UINT8 *pLevel
);

UINT32 SRVAPI CFW_CfgStoreUserProfile (
    VOID
);

UINT32 SRVAPI CFW_CfgRestoreFactoryProfile (
    VOID
);

UINT32 SRVAPI CFW_CfgGetUserProfile (VOID);

#define CFW_CHSET_UCS2 0
#define CFW_CHSET_GSM  1

UINT32 SRVAPI CFW_CfgGetCharacterSet (
    UINT8 *chset
);

UINT32 SRVAPI CFW_CfgSetCharacterSet (
    UINT8 chset
);

#define CFW_CFG_IND_BC_EVENT           (1<<0)
#define CFW_CFG_IND_SQ_EVENT           (1<<1)
#define CFW_CFG_IND_NW_EVENT           (1<<2)
#define CFW_CFG_IND_SOUNDER_EVENT      (1<<3)
#define CFW_CFG_IND_SMS_RECEIVE_EVENT  (1<<4)
#define CFW_CFG_IND_CALL_EVENT         (1<<5)
#define CFW_CFG_IND_VOX_EVENT          (1<<6)
#define CFW_CFG_IND_ROAM_EVENT         (1<<7)
#define CFW_CFG_IND_SMS_FULL_EVENT     (1<<8)
#define CFW_CFG_IND_BIT_ERR_EVENT      (1<<9)
#define CFW_CFG_IND_USER_SQ_EVENT      (1<<10)

#define CFW_CFG_IND_EVENT(bc,sq,nw,sounder,sms_receive, call, vox, roam, sms_full, biterr) \
    ((bc)|((sq)<<1)|((nw)<<2)|((sounder)<<3)|((sms_receive)<<4)|((call)<<5)|((vox)<<6)|((roam)<<7)|((sms_full)<<8|biterr<<9)

#define IS_CFG_IND_BC_EVENT(ind)           (ind>>0&1)
#define IS_CFG_IND_SQ_EVENT(ind)           (ind>>1&1)
#define IS_CFG_IND_NW_EVENT(ind)           (ind>>2&1)
#define IS_CFG_IND_SOUNDER_EVENT(ind)      (ind>>3&1)
#define IS_CFG_IND_SMS_RECEIVE_EVENT(ind)  (ind>>4&1)
#define IS_CFG_IND_CALL_EVENT(ind)         (ind>>5&1)
#define IS_CFG_IND_VOX_EVENT(ind)          (ind>>6&1)
#define IS_CFG_IND_ROAM_EVENT(ind)         (ind>>7&1)
#define IS_CFG_IND_SMS_FULL_EVENT(ind)     (ind>>8&1)
#define IS_CFG_IND_BIT_ERR_EVENT(ind)      (ind>>9&1)

#ifdef _WIN32
#pragma warning(disable: 4214)  // local variable *may* be used without init
#endif

typedef struct _CFW_IND_EVENT_INFO
{
    UINT32 bc: 3;
    UINT32 sq: 3;
    UINT32 nw_srv: 1;
    UINT32 sms_receive: 1;
    UINT32 call: 1;
    UINT32 roam: 1;
    UINT32 sms_full: 1;
    UINT32 sounder : 1;
    UINT32 bit_err: 8;
} CFW_IND_EVENT_INFO;
typedef enum
{
    /// Chip product ID
    CFW_SYS_CHIP_PROD_ID,

    /// Chip bond ID
    CFW_SYS_CHIP_BOND_ID,

    /// Chip metal ID
    CFW_SYS_CHIP_METAL_ID,

    /// Chip ID whole
    CFW_SYS_CHIP_ID_WHOLE,

} CFW_SYS_CHIP_ID_T;

UINT32 SRVAPI CFW_CfgSetIndicatorEvent (
    UINT16 ind, CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetIndicatorEvent (
    UINT16 *ind, CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetIndicatorEventInfo (
    CFW_IND_EVENT_INFO *pIndInfo,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetIndicatorEventInfo (
    CFW_IND_EVENT_INFO *pIndInfo,
    CFW_SIM_ID nSimID
);
UINT32 CFW_SetQualReport(
    BOOL bStart,
    CFW_SIM_ID nSimID
);


UINT32 CFW_CfgSetFirstPowerOnFlag(
    BOOL bFPO
);

UINT32 CFW_CfgCheckFirstPowerOnFlag(
    BOOL *pbFPO
);

UINT32 SRVAPI CFW_CfgGetEventReport(
    UINT16 *pMer
);

UINT32 SRVAPI CFW_CfgSetEventReport(
    UINT16 pMer
);

UINT32 SRVAPI CFW_CfgSetResultCodePresentMode(
    UINT8 nMode
);

UINT32 SRVAPI CFW_CfgSetResultCodeFormatMode(
    UINT8 nMode
);

UINT32 SRVAPI CFW_CfgGetResultCodePresentMode(
    UINT8 *pMode
);

UINT32 SRVAPI CFW_CfgGetResultCodeFormatMode(
    UINT8 *pMode
);

UINT32 SRVAPI CFW_CfgGetProductIdentification(
    UINT8 *pIdText
);

UINT32 SRVAPI CFW_CfgGetExtendedErrorV0 (
    PSTR *pErrReport
);

UINT32 SRVAPI CFW_CfgSetExtendedErrorV0 (
    UINT32 nErrorCode
);

UINT32 SRVAPI CFW_CfgSetEchoMode(UINT8 nMode);
UINT32 SRVAPI CFW_CfgGetEchoMode(UINT8 *nMode);

typedef struct _CFW_EXTENDED_ERROR
{
    UINT8 locationId;
    UINT8 reason;
    UINT8 ss_reason;
    UINT8 padding;
} CFW_EXTENDED_ERROR;

UINT32 SRVAPI CFW_CfgGetExtendedErrorV1 (
    CFW_EXTENDED_ERROR *pExtendedError
);

UINT32 SRVAPI CFW_CfgGetErrorReport(
    UINT8 *pLevel
);

UINT32 SRVAPI CFW_CfgSetErrorReport(
    UINT8 nLevel
);


UINT32 SRVAPI CFW_CfgSetEchoMode(UINT8 nMode);

UINT32 SRVAPI CFW_CfgSetResultCodeFormatMode( UINT8 nMode );

UINT32 SRVAPI CFW_CfgGetResultCodePresentMode( UINT8 *pMode );

UINT32 SRVAPI CFW_CfgSetResultCodePresentMode(UINT8 nMode);

UINT32 SRVAPI CFW_CfgGetResultCodeFormatMode( UINT8 *pMode );

UINT32 SRVAPI CFW_CfgDeviceSwithOff (VOID);

UINT32 SRVAPI CFW_CfgSetResultCodeFormatMode( UINT8 nMode );

UINT32 SRVAPI CFW_CfgSetConnectResultFormatCode(UINT8 nValue);

UINT32 SRVAPI CFW_CfgGetConnectResultFormatCode(UINT8 *nValue);

UINT32 CFW_CfgSetSoftReset(UINT8 cause);
UINT32 CFW_CfgGetSoftReset(UINT8 *cause);
UINT32 CFW_CfgSetResetCause(UINT8 cause);
UINT32 CFW_CfgGetResetCause(UINT8 *cause);
//SMS Configuration
#define CFW_SMS_STORAGE_AUTO    0x00

// SMS module
//message storage
#define CFW_SMS_STORAGE_ME 0x01
#define CFW_SMS_STORAGE_SM 0x02
#define CFW_SMS_STORAGE_NS  0x04 //No Save
#define CFW_SMS_STORAGE_MT  0x08 //SM preferred

//message type
#define CFW_SMS_TYPE_PP 0X01
#define CFW_SMS_TYPE_CB 0X02
#define CFW_SMS_TYPE_SR 0X04

#define CFW_SMS_STORED_STATUS_UNREAD                     0x01
#define CFW_SMS_STORED_STATUS_READ                       0x02
#define CFW_SMS_STORED_STATUS_UNSENT                     0x04
#define CFW_SMS_STORED_STATUS_SENT_NOT_SR_REQ            0x08
#define CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV       0x10
#define CFW_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE      0x20
#define CFW_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE     0x40
#define CFW_SMS_STORED_STATUS_STORED_ALL                 0x80

#define CFW_SMS_COPY_ME2SM                               0
#define CFW_SMS_COPY_SM2ME                               1

typedef struct _CFW_SMS_PARAMETER
{
    UINT8 vp;
    UINT8 pid;
    UINT8 dcs;
    UINT8 mti;
    UINT8 ssr;
    UINT8 bearer: 2;
    UINT8 bSimChange: 1;
    UINT8 bReserved: 5;
    UINT8 mr;
    UINT8 rp;
    UINT8 nNumber[12]; // nNumber[0]=len,nNumber[1]=[91] nNumber[2]=[86] ... in BCD.
    //added by fengwei 20080707 for AT command begin
    UINT8 udhi;
    UINT8 rd;
    UINT8 vpf;
    UINT8 nSaveToSimFlag;// 1 save sca to sim, 2 save pid to sim, 4 save dcs to sim, 8 save vp to sim, 0 not save to sim.
    UINT8 nIndex;//the index of param set, 0 to 4 is valuable
    BOOL bDefault;//if set param to current(Default) param, TRUE: save, FALSE, not save.
    //added by fengwei 20080707 for AT command end
} CFW_SMS_PARAMETER;

typedef struct _CFW_SMS_STORAGE_INFO
{
    UINT16 usedSlot;
    UINT16 totalSlot;
    UINT16 unReadRecords;
    UINT16 readRecords;
    UINT16 sentRecords;
    UINT16 unsentRecords;
    UINT16 unknownRecords;
    UINT16 storageId;
} CFW_SMS_STORAGE_INFO;


UINT32 CFW_CfgSetSmsStorage (
    UINT8 nStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSmsStorage (
    UINT8 *pStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetNewSmsOption (
    UINT8 nOption,
    UINT8 nNewSmsStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetNewSmsOption (
    UINT8 *pOption,
    UINT8 *pNewSmsStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetSmsOverflowInd (
    UINT8 nMode,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSmsOverflowInd (
    UINT8 *pMode,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetSmsFormat (
    UINT8 nFormat,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSmsFormat (
    UINT8 *nFormat,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetSmsShowTextModeParam (
    UINT8 nShow,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSmsShowTextModeParam (
    UINT8 *pShow,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetSmsParam (
    CFW_SMS_PARAMETER *pInfo,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSmsParam (
    CFW_SMS_PARAMETER *pInfo,
    UINT8 nIndex,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetSmsStorageInfo (
    CFW_SMS_STORAGE_INFO *pStorageInfo,
    UINT16 nStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSmsStorageInfo (
    CFW_SMS_STORAGE_INFO *pStorageInfo,
    UINT16 nStorage,
    CFW_SIM_ID nSimID
);




UINT32 SRVAPI CFW_CfgSelectSmsService(
    UINT8 nService,
    UINT8 *pSupportedType
);

UINT32 SRVAPI CFW_CfgQuerySmsService (
    UINT8 *pService
);

UINT32 SRVAPI CFW_CfgSetSmsCB (
    UINT8 mode,
    UINT8 mids[6],
    UINT8 dcss[6]
);

UINT32 SRVAPI CFW_CfgGetSmsCB (
    UINT8 *pMode,
    UINT8 mids[6],
    UINT8 dcss[6]
);



#ifdef SIM_SWITCH_ENABLE
//SIM Switch
UINT32 SRVAPI CFW_CfgSetSimSwitch(UINT8 n);

UINT32 SRVAPI CFW_CfgGetSimSwitch(UINT8 *n);

VOID SimSwitch(UINT8 SimNum);
#endif


// PBK Configuration
#define CFW_PBK_SIM                             0x01
#define CFW_PBK_ME                              0x02

#define CFW_PBK_ME_MISSED                       0x1
#define CFW_PBK_ME_RECEIVED                     0x2
#define CFW_PBK_SIM_FIX_DIALLING                0x4
#define CFW_PBK_SIM_LAST_DIALLING               0x8
#define CFW_PBK_ME_LAST_DIALLING                0x10
#define CFW_PBK_SIM_BARRED_DIALLING             0x11
#define CFW_PBK_SIM_SERVICE_DIALLING            0x12
#define CFW_PBK_ON                              0x20
#define CFW_PBK_EN                              0x40
#define CFW_PBK_SDN                             0x80

#define CFW_PBK_AUTO                            0x00
#define CFW_PBK_ALL                             0x00

// Pbk Option
#define CFW_PBK_OPTION_BASIC                      0x00
#define CFW_PBK_OPTION_All                        0x01
#define CFW_PBK_OPTION_BASIC_NAME_DESCEND_ORDER   0x04
#define CFW_PBK_OPTION_BASIC_NAME_ASCEND_ORDER    0x08
#define CFW_PBK_OPTION_All_NAME_DESCEND_ORDER     0x10
#define CFW_PBK_OPTION_All_NAME_ASCEND_ORDER      0x20

typedef struct _CFW_PBK_STRORAGE_INFO
{
    UINT16 index;
    UINT8 iNumberLen;
    UINT8 txtLen;
    UINT8 Bitmap[63];
} CFW_PBK_STRORAGE_INFO;

UINT32 SRVAPI CFW_CfgGetPbkStrorageInfo (
    CFW_PBK_STRORAGE_INFO *pStorageInfo
);

UINT32 SRVAPI CFW_CfgSetPbkStrorageInfo (
    CFW_PBK_STRORAGE_INFO *pStorageInfo
);

// NW Configuration

#define CFW_STORE_MAX_COUNT 33
typedef struct //this must be the same with api_NwStoreListInd_t
{
    u8 Plmn[3];
    u8 nCount;
    u16 ArfcnL[CFW_STORE_MAX_COUNT];
} CFW_StoredPlmnList;

typedef struct _CFW_SIMSTATUS
{
    BOOL UsimFlag;
} CFW_SIMSTATUS;


typedef struct _NW_SIM_INFO
{
    CFW_StoredPlmnList p_StoreList[CFW_SIM_COUNT];
    u8  SimStatus;  // 0: sim; 1: Usim
} CFW_SIM_INFO;

UINT32 SRVAPI CFW_CfgSetPbkStorage (
    UINT8 nStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetPbkStorage (
    UINT8 *nStorage,
    CFW_SIM_ID nSimID
);



UINT32 CFW_CfgSetStoredPlmnList(
    CFW_StoredPlmnList *pPlmnL,
    CFW_SIM_ID nSimId
);

UINT32 CFW_CfgGetStoredPlmnList(
    CFW_StoredPlmnList *pPlmnL,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CfgNwSetFrequencyBand (
    UINT8 nBand,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgNwGetFrequencyBand (
    UINT8 *pBand,
    CFW_SIM_ID nSimID
);
#ifdef __MULTI_RAT__
UINT32 SRVAPI CFW_CfgNwSetFrequencyUMTSBand (
    UINT8 nBand,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgNwGetFrequencyUMTSBand (
    UINT8 *pBand,
    CFW_SIM_ID nSimID
);
#endif
UINT32 SRVAPI CFW_CfgNwGetNetWorkMode(UINT8 *pMode, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_CfgNwSetNetWorkMode(UINT8 nMode, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_CfgGetNwStatus (
    UINT8 *pCmd,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetNwStatus (
    UINT8 n,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_CfgNwGetOperatorInfo (
    UINT8 **pOperatorId,
    UINT8 **pOperatorName,
    UINT32 nIndex
);

UINT32 SRVAPI CFW_CfgGetNwOperDispFormat(
    UINT8 *nFormat
);

UINT32 SRVAPI CFW_CfgNwGetOperatorId (
    UINT8 **pOperatorId,
    UINT8 *pOperatorName
);

UINT32 SRVAPI CFW_CfgNwGetOperatorName (
    UINT8 pOperatorId[6],
    UINT8 **pOperatorName
);

UINT32 SRVAPI CFW_CfgSetNwOperDispFormat(
    UINT8 nFormat
);

// CM Configuration

UINT32 SRVAPI CFW_CfgSetSSN (
    UINT8 nCSSI,
    UINT8 nCSSU,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetSSN (
    UINT8 *pCSSI,
    UINT8 *pCSSU,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetClir (
    UINT8 nClir,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetClir (
    UINT8 *n,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetClip (
    UINT8 nEnable,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetClip (
    UINT8 *n,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetColp (
    UINT8 n,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetColp (
    UINT8 *n,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgSetCallWaiting (
    UINT8 n,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_CfgGetCallWaiting (
    UINT8 *n,
    CFW_SIM_ID nSimID
);

UINT32 CFW_CfgGetIMSI(UINT8 *pIMSI, CFW_SIM_ID nSimID);

UINT32 CFW_CfgSetIMSI(UINT8 *pIMSI, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_CfgSetToneDuration (
    UINT8  nToneDuration
);

UINT32 SRVAPI CFW_CfgGetToneDuration (
    UINT8 *pToneDuration
);

UINT32 CFW_CfgSetIncomingCallResultMode(UINT8 nMode);

UINT32 CFW_CfgGetIncomingCallResultMode(UINT8 *nMode);


// Device Configuration
typedef struct _CFW_AUD_OUT_PARAMETER
{
    UINT8 outBbcGain;
    UINT8 outStep;
    UINT16 sideTone;
    UINT16 outCalibrate[4];
} CFW_AUD_OUT_PARAMETER;

UINT32 SRVAPI CFW_CfgSetAudioOutputParam (
    CFW_AUD_OUT_PARAMETER *pAudOutParam
);

UINT32 SRVAPI CFW_CfgGetAudioOutputParam (
    CFW_AUD_OUT_PARAMETER *pAudOutParam
);

UINT32 SRVAPI CFW_CfgSetAudioAudioMode (
    UINT8 nAudioMode
);

UINT32 SRVAPI CFW_CfgGetAudioAudioMode (
    UINT8 *pAudioMode
);

UINT32 SRVAPI CFW_CfgSetLoudspeakerVolumeLevel(
    UINT8 nVolumeLevel
);

UINT32 SRVAPI CFW_CfgGetLoudspeakerVolumeLevel(
    UINT8 *pVolumeLevel
);

UINT32 SRVAPI CFW_CfgGetLoudspeakerVolumeLevelRange(
    UINT8 *pMinVolumeLevel,
    UINT8 *pMaxVolumeLevel
);

UINT32 SRVAPI CFW_CfgDeviceSwithOff (
    VOID
);

//
// NW Service
//
#define CFW_NW_STATUS_NOTREGISTERED_NOTSEARCHING 0// Not registered and not searching.
#define CFW_NW_STATUS_REGISTERED_HOME            1 //Registered and in the home area.
#define CFW_NW_STATUS_NOTREGISTERED_SEARCHING    2 //Not registered and searching for a new operator.
#define CFW_NW_STATUS_REGISTRATION_DENIED        3 //Registration denied.
#define CFW_NW_STATUS_UNKNOW                     4 //Unknown registration.
#define CFW_NW_STATUS_REGISTERED_ROAMING         5

#define CFW_NW_AUTOMATIC_MODE              0
#define CFW_NW_MANUAL_MODE                 1
#define CFW_NW_MANUAL_AUTOMATIC_MODE       4

#define CFW_NW_OPERATOR_STATUS_UNKNOW      0 //unknown
#define CFW_NW_OPERATOR_STATUS_AVAILABLE   1 //available
#define CFW_NW_OPERATOR_STATUS_CURRENT     2 //current
#define CFW_NW_OPERATOR_STATUS_FORBIDDEN   3  //forbidden


#define CFW_RAT_AUTO        1
#define CFW_RAT_GSM_ONLY    2
#define CFW_RAT_UMTS_ONLY   4
#ifdef LTE_NBIOT_SUPPORT
#define CFW_RAT_NBIOT_ONLY  8
#endif

typedef struct _CFW_NW_NETWORK_INFO
{
    UINT8 nLocalTimeZoneLen;
    UINT8 nLocalTimeZone;
    UINT8 nUniversalTimeZoneLen;
    UINT8 nUniversalTimeZone[7];
    UINT8 nDaylightSavingTimeLen;
    UINT8 nDaylightSavingTime;
    UINT8 nNwkFullNameLen;
    UINT8 nNwkShortNameLen;
    UINT8 nNwkNames[247];
} CFW_NW_NETWORK_INFO;

typedef struct _CFW_NW_OPERATOR_INFO
{
    UINT8 nOperatorId[6];
    UINT8 nStatus;
} CFW_NW_OPERATOR_INFO;



typedef struct _CFW_NW_STATUS_INFO
{
    UINT8 nStatus;
    UINT8 nCause;     //only use PS no service
    UINT8 PsType;   //add for diff GPRS or EDGE by wuys 2013-07-19.  1:GPRS  2:EDGE
    UINT8 nCsStatusChange;//add for if CS status have changed,we will set the TRUE.
    UINT8 nAreaCode[5];
    UINT8  nCurrRat;
#ifdef LTE_NBIOT_SUPPORT
	UINT8  nCellId [4] ; //cell id in umts is bitstring(28), so 4 bytes is necessary.
	UINT8 cause_type;
	UINT8 reject_cause;
	UINT8  activeTime;
	UINT8  periodicTau;
	//UINT8 activeTime[8];
	//UINT8 periodicTau[8];
	u8 nwSupportedCiot; //[0-3]
	u8 edrxEnable;	//0 off, 1 on
	u8 edrxPtw; 	//[0-15]
	u8 edrxValue;	 //[0-15]
	UINT8 ConnMode;
#else
	UINT8 nCellId[2];
#endif

} CFW_NW_STATUS_INFO;



UINT32 CFW_NWSetRat(UINT8 nRat, CFW_SIM_ID nSimID);
UINT8 CFW_NWGetRat(CFW_SIM_ID nSimID);
UINT32 CFW_NWSetStackRat(UINT8 nRat, CFW_SIM_ID nSimID);
UINT8 CFW_NWGetStackRat(CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_NwGetAvailableOperators (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_NwGetCurrentOperator (
    UINT8 OperatorId[6],
    UINT8 *pMode,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_NwGetImei (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_NwGetSignalQuality (
    UINT8 *pSignalLevel,
    UINT8 *pBitError,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_NwGetStatus (
    CFW_NW_STATUS_INFO *pStatusInfo,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_NwSetRegistration (
    UINT8 nOperatorId[6],
    UINT8 nMode,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_NwDeRegister (
    UINT16 nUTI,
#ifdef LTE_NBIOT_SUPPORT
	UINT8 detachType,
#else
    BOOL bPowerOff,
#endif
	CFW_SIM_ID nSimID
);

typedef enum
{
    CFW_DISABLE_COMM = 0x00,   //don't start
    CFW_ENABLE_COMM,        // start stack
    CFW_CHECK_COMM
} CFW_COMM_MODE;

//nMode: 0 : CFW_DISABLE_COMM; 1: CFW_ENABLE_COMM; 2: CFW_CHECK_COMM
UINT32 CFW_SetComm (
    CFW_COMM_MODE   nMode,
    UINT8                       nStorageFlag,
    UINT16          nUTI,
    CFW_SIM_ID      nSimID
);

UINT32 CFW_GetComm (
    CFW_COMM_MODE *pMode,
    CFW_SIM_ID  nSimID
);


UINT32 SRVAPI CFW_NwSetFrequencyBand (
    UINT8 nBand,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_NwAbortListOperators(
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);



/**
 *  Function Description:
 *      Get the Network short name string, short name string length, Coding Scheme
 *  @param1
 *      pNwkNames: input network indication string
 *  @param2
 *      ppShortName: Output the short name string
 *  @param3
 *      pShortNameLen: Output the short name string length
 *  @param4
 *      pDCS: Output the short name string Coding Scheme,0->ASCII 1->UCS2
 *
 *  @return  ERR_SUCCESS | ERR_NO_MORE_MEMORY
 */

UINT32 CFW_GetNWIShortName(UINT8 *pNwkNames,UINT8 ** ppShortName,UINT8 * pShortNameLen,UINT8 * pDCS);

/**
 *  Function Description:
 *      Get the Network long name string, long name string length, Coding Scheme
 *  @param1
 *      pNwkNames: input network indication string
 *  @param2
 *      ppLongName: Output the long name string
 *  @param3
 *      pShortNameLen: Output the long name string length
 *  @param4
 *      pDCS: Output the long name string Coding Scheme,0->ASCII 1->UCS2
 *
 *  @return  ERR_SUCCESS | ERR_NO_MORE_MEMORY
 */

UINT32 CFW_GetNWILongName(UINT8 *pNwkNames,UINT8 ** ppLongName,UINT8 * pLongNameLen,UINT8 * pDCS);

BOOL CFW_GetNWTimerOutFlag(CFW_SIM_ID nSimID);
VOID CFW_SetNWTimerOutFlag(BOOL bClear, CFW_SIM_ID nSimID);


#define CFW_NW_BAND_GSM_450   ( 1 << 0 )
#define CFW_NW_BAND_GSM_480   ( 1 << 1 )
#define CFW_NW_BAND_GSM_900P  ( 1 << 2 )
#define CFW_NW_BAND_GSM_900E  ( 1 << 3 )
#define CFW_NW_BAND_GSM_900R  ( 1 << 4 )
#define CFW_NW_BAND_GSM_850   ( 1 << 5 )
#define CFW_NW_BAND_DCS_1800  ( 1 << 6 )
#define CFW_NW_BAND_PCS_1900  ( 1 << 7 )


//
// CM Service
//

#define CFW_CM_MODE_VOICE      0
#define CFW_CM_MODE_DATA       1
#define CFW_CM_MODE_FAX        2

//Call direction
#define CFW_DIRECTION_MOBILE_ORIGINATED 0
#define CFW_DIRECTION_MOBILE_TERMINATED 1

//Call state
#define CFW_CM_STATUS_ACTIVE   0
#define CFW_CM_STATUS_HELD     1
#define CFW_CM_STATUS_DIALING  2  //MO CALL)
#define CFW_CM_STATUS_ALERTING 3  //(MO CALL)
#define CFW_CM_STATUS_INCOMING 4  //(MT CALL)
#define CFW_CM_STATUS_WAITING  5  //(MT call)
#define CFW_CM_STATUS_RELEASED 7

typedef struct _CFW_SPEECH_CALL_IND
{
    CFW_TELNUMBER TelNumber;
    UINT8 nCode;
    UINT8 nCLIValidity;
    UINT8 nDisplayType;
    UINT8 padding;
} CFW_SPEECH_CALL_IND;

typedef struct _CFW_CC_CURRENT_CALL_INFO
{
    CFW_TELNUMBER dialNumber;
    BOOL multiparty;
    UINT8 idx;
    UINT8 direction;
    UINT8 status;
    UINT8 mode;
    UINT8 padding[3];
} CFW_CC_CURRENT_CALL_INFO;

//Remove CPP_KEYWORD_SUPPORT  by lixp for MMI compilation issue at 20090512.

typedef struct _CFW_SS_QUERY_CALLFORWARDING_RSP
{
    CFW_TELNUMBER nNumber;
    UINT8 nStatus;
    UINT8 nClass;
    UINT8 nReason;
    UINT8 nTime;
} CFW_SS_QUERY_CALLFORWARDING_RSP ;
typedef struct _CFW_SS_SET_CALLFORWARDING_INFO
{
    UINT8 nReason;
    UINT8 nMode;
    UINT8 nClass;
    UINT8 nTime;
    CFW_DIALNUMBER nNumber;
} CFW_SS_SET_CALLFORWARDING_INFO;

typedef struct _CFW_SS_SET_CALLFORWARDING_RSP_INFO
{
    UINT8 nReason;
    UINT8 nMode;
    UINT8 nClass;
    UINT8 nTime;
    CFW_DIALNUMBER nNumber;
} CFW_SS_SET_CALLFORWARDING_RSP_INFO;


UINT32 SRVAPI CFW_CcAcceptSpeechCall (
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcGetCallStatus(
    CFW_SIM_ID nSimId
);
UINT8  CFW_GetCcCount( CFW_SIM_ID nSimID);


UINT32 SRVAPI CFW_CcGetCurrentCall (
    CFW_CC_CURRENT_CALL_INFO CallInfo[7],
    UINT8 *pCnt,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcCallHoldMultiparty (
    UINT8 nCmd,
    UINT8 nIndex,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcInitiateSpeechCall (
    CFW_DIALNUMBER *pDialNumber,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);


UINT32 SRVAPI CFW_CcInitiateSpeechCallEx(
    CFW_DIALNUMBER *pDialNumber,
    UINT8 *pIndex,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcEmcDial (
    UINT8 *pNumber,
    UINT8 nSize,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcEmcDialEx(
    UINT8 *pDialNumber,
    UINT8 nDialNumberSize,
    UINT8 *pIndex,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcReleaseCall (
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcRejectCall (
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcPlayTone (
    INT8 iTone,
    CFW_SIM_ID nSimId
);

UINT32 SRVAPI CFW_CcStopTone (
    CFW_SIM_ID nSimId
);



UINT32 SRVAPI CFW_SsQueryCallWaiting (
    UINT8 nClass,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsSetCallWaiting (
    UINT8 nMode,
    UINT8 nClass,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsSetCallForwarding (
    CFW_SS_SET_CALLFORWARDING_INFO *pCallForwarding,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsQueryCallForwarding (
    UINT8 nReason,
    UINT8 nClass,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);



#define CFW_STY_FAC_TYPE_AO 0x414F //BAOC (Bar All Outgoing Calls)
#define CFW_STY_FAC_TYPE_OI 0x4F49 //BOIC (Bar Outgoing International Calls)
#define CFW_STY_FAC_TYPE_OX 0x4F58 //BOIC-exHC (Bar Outgoing International Calls except to Home Country)
#define CFW_STY_FAC_TYPE_AI 0x4149 //BAIC (Bar All Incoming Calls)
#define CFW_STY_FAC_TYPE_IR 0x4952 //BIC-Roam (Bar Incoming Calls when Roaming outside the home country)
#define CFW_STY_FAC_TYPE_AB 0x4142 //All Barring services (only applicable for mode=0)
#define CFW_STY_FAC_TYPE_AG 0x4147 //All outGoing barring services (only applicable for mode=0)
#define CFW_STY_FAC_TYPE_AC 0x4143 //All inComing barring services (only applicable for mode=0)

#define CFW_CM_MODE_UNLOCK 0 //unlock
#define CFW_CM_MODE_LOCK   1 //lock

#define CFW_SS_FORWARDING_REASON_UNCONDITIONAL          0 //Forward unconditionally.
#define CFW_SS_FORWARDING_REASON_MOBILEBUSY             1 //Forward if the mobile phone is busy
#define CFW_SS_FORWARDING_REASON_NOREPLY                2 //Forward if there is no answer
#define CFW_SS_FORWARDING_REASON_NOTREACHABLE           3 //Forward if the number is unreachable.
#define CFW_SS_FORWARDING_REASON_ALL_CALLFORWARDING     4 //All call forwarding reasons(includes reasons 0, 1, 2 and 3).
#define CFW_SS_FORWARDING_REASON_ALL_CONDCALLFORWARDING 5 //All conditional call forwarding reasons(includes reasons 1, 2 and 3).

#define CFW_SS_FORWARDING_MODE_DISABLE       0
#define CFW_SS_FORWARDING_MODE_ENABLE        1
#define CFW_SS_FORWARDING_MODE_QUERY         2
#define CFW_SS_FORWARDING_MODE_REGISTRATION  3
#define CFW_SS_FORWARDING_MODE_ERASURE       4


UINT32 SRVAPI CFW_SsSetFacilityLock (
    UINT16 nFac,
    UINT8 *pBufPwd,
    UINT8 nPwdSize,
    UINT8 nClass,
    UINT8 nMode,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsQueryFacilityLock (
    UINT16 nFac,
    UINT8 nClass,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsChangePassword (
    UINT16 nFac,
    UINT8 *pBufOldPwd,
    UINT8 nOldPwdSize,
    UINT8 *pBufNewPwd,
    UINT8 nNewPwdSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsQueryClip (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsQueryClir (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsQueryColp (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SsQueryColr (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI  CFW_SsSendUSSD (
    UINT8 *pUsdString,
    UINT8  nUsdStringSize,
    UINT8  nOption,
    UINT8  nDcs,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);


typedef struct _CFW_SS_USSD_IND_INFO
{
    UINT8 *pUsdString;
    UINT8 nStingSize;
    UINT8 nOption;
    UINT8 nDcs;
    UINT8 padding;
} CFW_SS_USSD_IND_INFO;

//
// SIM Service
//
typedef struct _CFW_SAT_RESPONSE
{
    UINT8 nSm_Act_prevStatus;
    UINT8 nSm_Act_currStatus;
    UINT8 nSAT_Mode;

    UINT8 nSm_Resp_prevStatus;
    UINT8 nSm_Resp_currStatus;

    UINT8 nSAT_CmdType;
    UINT8 nSAT_Status;          //command status
    UINT8 nSAT_ItemID;
    UINT8 nSAT_InnerProcess;
    UINT8 nSAT_InputSch;        //text code scheme.
    UINT8 nSAT_InputLen;        //text length
    UINT8 padding;          //DON'T remove this byte, because the next member nSAT_InputStr should be aligned 4 bytes, it'll convert to a pointer.
    UINT8 nSAT_InputStr[0];
} CFW_SAT_RESPONSE;

typedef struct _CFW_SAT_TERMINAL_ENVELOPE_CNF
{
    UINT8        Sw1               ;
    UINT8        NbByte            ; // Nb of byte of the response
    UINT8        ResponseData[256] ;

} CFW_SAT_TERMINAL_ENVELOPE_CNF ;


typedef struct _SAT_CHSTATUS_RSP
{
    UINT8 nChannelID;      //1..7
    UINT8 nStatus;         // 0: Link not established or PDP context not activated
    // 1: Link established or PDP context activated
    //
    UINT8 nType;           //00 : No further info can be given
    //05 : Link dropped
} SAT_CHSTATUS_RSP;

typedef struct _SAT_BASE_RSP        //for proactive command 26
{
    UINT8 nComID;                   //Command id
    UINT8 nComQualifier;            //Command Qualifier
} SAT_BASE_RSP;

typedef struct _SAT_LOCALINFO_RSP
{
    UINT8 nMCC_MNC_Code[6];        //BCD code ,example:4,6,0,0,0,F for china mobile
    UINT16 nLocalAreaCode;         //Hex Local area code
    UINT16 nCellIdentify;          //Hex Cell identify
} SAT_LOCALINFO_RSP;

typedef struct _SAT_IMEI_RSP
{
    UINT8 nInterMobileId[8];        //BCD code,IMEI
} SAT_IMEI_RSP;

typedef struct _SAT_MEASURE_RSP
{
    UINT8 nMeasure[16];             //Net work measurement
} SAT_MEASURE_RSP;

typedef struct _SAT_DATETIME_RSP
{
    UINT8 nDateTimeZone[7];         //Date,Time,Zone
} SAT_DATETIME_RSP;

typedef struct _SAT_LANGUAGE_RSP
{
    UINT8 nLanguageCode[2];         //Language
} SAT_LANGUAGE_RSP;

typedef struct _SAT_TIMINGADV_RSP
{
    UINT8 nME_Status;               //Me status
    UINT8 nTimingAdv;               //Timing advance
} SAT_TIMINGADV_RSP;



typedef struct _CFW_SAT_SMS_RSP     //for proactive command 0x13
{
    UINT8  nComID ;
    UINT8  nComQualifier;
    UINT16 nAlphaLen;
    UINT8 *pAlphaStr;
    UINT16 nAddrLen;
    UINT8  nAddrType;
    UINT8 *pAddrStr;
    UINT8  nPDULen;
    UINT8 *pPDUStr;
} CFW_SAT_SMS_RSP;

typedef struct _CFW_SAT_DTEXT_RSP        //for proactive command 0x21
{
    UINT8  nComID;
    UINT8  nComQualifier;
    UINT8  nTextLen;
    UINT8 nTextSch;
    UINT8 *pTextStr;
} CFW_SAT_DTEXT_RSP;

typedef struct _CFW_SAT_POLL_INTERVAL_RSP        //for proactive command 0x21
{
    UINT8  nComID;
    UINT8  nComQualifier;
    UINT8 nTimeUnit;
    UINT8 nTimeInterval;
    UINT8 nPending[2];

} CFW_SAT_POLL_INTERVAL_RSP;


typedef struct _CFW_SAT_INPUT_RSP        //for proactive command 0x23
{
    UINT8 nComID;
    UINT8 nComQualifier;

    UINT8 nTextLen;
    UINT8 nTextSch;
    UINT8 *pTextStr;

    UINT8 nResMinLen;
    UINT8 nResMaxLen;

    UINT8 nDefTextLen;
    UINT8 nDefTextSch;
    UINT8 *pDefTextStr;
} CFW_SAT_INPUT_RSP;

typedef struct _CFW_SAT_INKEY_RSP        //for proactive command 0x23
{
    UINT8 nComID;
    UINT8 nComQualifier;

    UINT8 nTextLen;
    UINT8 nTextSch;
    UINT8 *pTextStr;
} CFW_SAT_INKEY_RSP;

typedef struct _CFW_SAT_CALL_RSP        //for proactive command 0x10
{
    UINT8  nComID;
    UINT8  nComQualifier;

    UINT16 nAlphaConfirmLen;
    UINT8 *pAlphaConfirmStr;

    UINT8  nAddrLen;
    UINT8  nAddrType;
    UINT8 *pAddrStr;

    UINT8  nCapabilityCfgLen;
    UINT8 *pCapabilityCfgStr;

    UINT8  nSubAddrLen;
    UINT8 *pSubAddrStr;

    UINT8  nTuint;
    UINT8  nTinterval;

    UINT16 nAlphaSetupLen;
    UINT8 *pAlphaSetupStr;
} CFW_SAT_CALL_RSP;


typedef struct _CFW_SAT_SS_RSP        //for proactive command 0x10
{
    UINT8  nComID;
    UINT8  nComQualifier;

    UINT8  nAlphaLength;
    UINT8  *nAlphaIdentifier;

    UINT8  nIconQualifier;
    UINT8  nIconIdentifier;

    UINT8  nSSLength;
    UINT8  nTONNPI;
    UINT8 *pSSStr;
} CFW_SAT_SS_RSP;

typedef struct _CFW_SAT_USSD_RSP        //for proactive command 0x10
{
    UINT8  nComID;
    UINT8  nComQualifier;

    UINT8  nAlphaLength;
    UINT8  *nAlphaIdentifier;

    UINT8  nIconQualifier;
    UINT8  nIconIdentifier;

    UINT8 nUSSDLength;
    UINT8  nCodeScheme;
    UINT8 *pUSSDStr;
} CFW_SAT_USSD_RSP;

#define MAX_NUM_EVENT_LIST      11          // 11 type of events
typedef struct _CFW_SAT_EVENTLIST_RSP        //for proactive command 0x10
{
    UINT8 nComID;
    UINT8 nComQualifier;
    UINT8 nNumEvent;
    UINT8 nEventList[MAX_NUM_EVENT_LIST];
} CFW_SAT_EVENTLIST_RSP;

typedef struct _CFW_SAT_FILELIST_RSP        //for proactive command 0x10
{
    UINT8 nComID;
    UINT8 nComQualifier;
    UINT8 nMode;
    UINT8 nNumFiles;
    UINT8 nFiles[0];
} CFW_SAT_FILELIST_RSP;

typedef struct _CFW_SAT_PLAYTONE_RSP
{
    UINT8 nComID;
    UINT8 nComQualifier;
    UINT8 TimeUnit;
    UINT8 TimeInterval;
    UINT8 Tone;
    UINT8 IconQualifier;
    UINT8 IconIdentifier;
    UINT8 AlphaIDLen;
    UINT8 AlphaIDString[0];
} CFW_SAT_PLAYTONE_RSP;

typedef struct _CFW_SAT_BROWSER_RSP        //for proactive command 0x15
{
    UINT8  nComID;
    UINT8  nComQualifier;

    UINT8  nBrowserIdentity;

    UINT8  nURLLength;

    UINT8  nBearerLength;

    UINT8  nCodeScheme;
    UINT8  nTextLength; //Gateway/Proxy

    UINT8  nAlphaLength;

    UINT8  nIconQualifier;
    UINT8  nIconIdentifier;

    UINT8  nProvisioningFileRefNum;

    UINT8  nPading;
    UINT8 *pURL;
    UINT8 *pListBearer;
    UINT8 *nTextString;
    UINT8 *nAlphaIdentifier;
    UINT8 *pProvisioningFileRef[0];

} CFW_SAT_BROWSER_RSP;

typedef struct _CFW_SAT_TIMER_RSP        //for proactive command 0x15
{
    UINT8  nComID;
    UINT8  nComQualifier;

    UINT8  nTimerID;

    UINT8  nHour;
    UINT8  nMinute;
    UINT8 nSecond;
} CFW_SAT_TIMER_RSP;

typedef struct _CFW_SAT_SEND_DTMF_RSP        //for proactive command
{
    UINT8  nComID;
    UINT8  nComQualifier;
    UINT8 nAlphaLen;
    UINT8 *pAlphaStr;
    UINT8  nDTMFLen;
    UINT8 *pDTMFStr;
} CFW_SAT_SEND_DTMF_RSP;

typedef struct _CFW_SAT_BASE_RSP
{
    UINT8  nComID;
    UINT8  nComQualifier;
} CFW_SAT_BASE_RSP;


#if 1
typedef struct _CFW_SAT_PDU_RSP        //for proactive command 0x15
{
    UINT8  nComID;
    UINT8  nComNum;
    UINT8  nComQualifier;
    UINT8 nLength;
    UINT8 *pPDU;
} CFW_SAT_PDU_RSP;
#endif

//These Macro will be add to system header files.


//This Macro definition is used for ParsSW1SW2 function.
#define ERR_CME_SIM_VERIFY_FAIL                             0x20
#define ERR_CME_SIM_UNBLOCK_FAIL                            0x21
#define ERR_CME_SIM_CONDITION_NO_FULLFILLED     0x22
#define ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT            0x23
#define ERR_CME_SIM_VERIFY_FAIL_NO_LEFT                 0x24
#define ERR_CME_SIM_INVALID_PARAMETER                   0x25
#define ERR_CME_SIM_UNKNOW_COMMAND                      0x26
#define ERR_CME_SIM_WRONG_CLASS                         0x27
#define ERR_CME_SIM_TECHNICAL_PROBLEM                   0x28
#define ERR_CME_SIM_CHV_NEED_UNBLOCK                    0x29


#define ERR_CME_SIM_NOEF_SELECTED                           0x2A
#define ERR_CME_SIM_FILE_UNMATCH_COMMAND            0x2B
#define ERR_CME_SIM_CONTRADICTION_CHV                    0x2C
#define ERR_CME_SIM_CONTRADICTION_INVALIDATION  0x2D
#define ERR_CME_SIM_MAXVALUE_REACHED                    0x2E
#define ERR_CME_SIM_PATTERN_NOT_FOUND                  0x2F
#define ERR_CME_SIM_FILEID_NOT_FOUND                0x30
#define ERR_CME_SIM_STK_BUSY                                0x31
#define ERR_CME_SIM_UNKNOW                                     0x32
#define ERR_CME_SIM_PROFILE_ERROR                           0x33

// WARNING
#define ERR_CME_SIM_NO_INFORMATION              0x34
#define ERR_CME_SIM_RETURN_DATA_CORRUPTED       0x35
#define ERR_CME_SIM_LESS_DATA                       0x36
#define ERR_CME_SIM_SELECT_FILE_INVALIDATED     0x37
#define ERR_CME_SIM_SELECT_FILE_TERMINATION     0x38
#define ERR_CME_SIM_MORE_DATA                   0x39
#define ERR_CME_SIM_MORE_DATA_PROACTIVE_COMMAND_PENDING         0x3A
#define ERR_CME_SIM_RESPONSE_DATA_AVAILABLE     0x3B

#define ERR_CME_SIM_MORE_DATA_EXPECTED          0x3C
#define ERR_CME_SIM_MORE_DATA_EXPECTED_PROACTIVE_COMMAND_PENDING        0x3D
#define ERR_CME_SIM_COMMAND_SUCCESS_VERIFICATION_FAIL           0x3E

//Execution errors
#define ERR_CME_SIM_MEMORY_PROBLEM              0x3F

// Functions in CLA not supported
#define ERR_CME_SIM_LOGIC_CHANNEL_NO_SUPPORTED      0x40
#define ERR_CME_SIM_SECURE_MESSAGING_NO_SUPPORTED       0x41

//Command not allowed
#define ERR_CME_SIM_COMMAND_IMCOMPATIBLE                0x42
#define ERR_CME_SIM_SECURITY_STATUS_NO_SATISFIED        0x43
#define ERR_CME_SIM_AUTH_PIN_METHOD_BLOCKED         0x44
#define ERR_CME_SIM_REFERENCED_DATA_INVALIDATED     0x45
#define ERR_CME_SIM_USE_CONDITIONS_NOT_SATISFIED        0x46
#define ERR_CME_SIM_COMMAND_NOT_ALLOWED             0x47
#define ERR_CME_SIM_COMMAND_NOT_ALLOWED_SECURITY_NOT_SATISFIED      0x48

//Wrong parameters
#define ERR_CME_SIM_INCORRECT_PARA_IN_DATA_FIELD        0x49
#define ERR_CME_SIM_FUNCTION_NOT_SUPPORTED              0x4a
#define ERR_CME_SIM_FILE_NOT_FOUND                      0x4B
#define ERR_CME_SIM_RECORD_NOT_FOUND                    0x4C
#define ERR_CME_SIM_NOT_ENOUGH_MEMORY                   0x4D
#define ERR_CME_SIM_INCORRECT_PARA_P1_P2                0x4E
#define ERR_CME_SIM_LC_INCONSISTENT_WITH_P1_P2          0x4F
#define ERR_CME_SIM_REFERENCED_DATA_NOT_FOUND           0x50

//Application errors
#define ERR_CME_SIM_AUTH_ERROR_APPLICATION_SPECIFIC         0x51
#define ERR_CME_SIM_SECURITY_SESSION_ASSOCIATION_EXPIRED        0x52



//#define EV_CFW_SIM_LISTCOUNT_PBK_ENTRY_RSP        16        //for List counter function event id
#define ERR_SCL_QUEUE_FULL                                  0x23    //
#define ERR_CFW_INVALID_COMMAND                         1
//end


#define SIM_PBK_NAME_SIZE    50  //Lixp updated,20071225
#define SIM_PBK_NUMBER_SIZE  10
#define SIM_PBK_EXTR_SIZE    (SIM_PBK_NUMBER_SIZE+SIM_PBK_NAME_SIZE)

typedef struct _CFW_SIM_PBK_ENTRY_INFO
{
    UINT8 *pNumber;
    UINT8 *pFullName;
    UINT8 iFullNameSize;
    UINT8 nNumberSize;
    UINT8 nType;
    UINT16 phoneIndex;
    UINT8 *pRecordData;
} CFW_SIM_PBK_ENTRY_INFO;


#define FIRST_MATCH     0xA5
#define ALL_MATCH       0x5A
#define WILDCARD_MATCH  0x55

#define SEEK_SIM_ADN        0
#define SEEK_SIM_FDN        1
#define SEEK_SIM_MSISDN     2
#define SEEK_SIM_BDN        3
#define SEEK_SIM_SDN        4
#define SEEK_SIM_LND        5

#define SEEK_SIM_NAME       3
#define SEEK_SIM_NUMBER     4
#define SEEK_SIM_USED       5

typedef struct _CFW_SIM_PBK_INFO
{
    UINT16 nIndex;
    UINT8 nType;
    UINT8 nLengthAl;
    UINT8 nLengthNb;
    UINT8 nAlpha[40];
    UINT8 nNumber[20];
} CFW_SIM_PBK_INFO;

typedef struct _CFW_SIM_SEARCH_PBK
{
    UINT16 nMatchRecords;    //is the number of the LV
    UINT16 nUsedRecords;    //is the number of the LV
    UINT16 nTotalRecords;
    UINT8 nData[0];         //nData is composed of two bytes index, one byte type,
    //one byte length of name and name
    //one byte length of numbre and number
} CFW_SIM_SEARCH_PBK;

typedef struct _CFW_SIM_PUCT_INFO
{
    UINT8  iCurrency[3];  //currency code
    UINT16 iEPPU;
    UINT8  iEX;
    BOOL   iSign;          //the sign of EX,1 is the negetive sign
    UINT8  padding;
} CFW_SIM_PUCT_INFO;


#define CFW_SIM_PBKMODE_ADN  0
#define CFW_SIM_PBKMODE_FDN  1

UINT32 SRVAPI CFW_SimGetPbkMode (
    UINT16 nUTI
);

UINT32 SRVAPI CFW_SimSetPbkMode (
    UINT8 nPbkMode,
    UINT16 nUTI
);

typedef struct _CFW_PBK_STORAGE
{
    UINT16 usedSlot;
    UINT16 totalSlot;
    UINT16 storageId;
    UINT8 padding[2];
} CFW_PBK_STORAGE;

//add by wulc  begin
typedef struct _CFW_SPN
{
    UINT8 flag;
    UINT8 spn[16];
} CFW_SPN;
//add by wulc end

#define CFW_STY_FAC_TYPE_SC  0x5343 //SIM (lock SIM cards). SIM requests password upon ME power up and when this lock command is issued. "SC" lock is protected with SIM PIN1. The number can be modified with this function.
#define CFW_STY_FAC_TYPE_PS  0x5053 //Phone locked to SIM (device code). The "PS" password may either be individually specified by the client or, depending on the subscription, supplied from the provider (e.g. with a prepaid mobile).
#define CFW_STY_FAC_TYPE_P2  0x5032 //SIM PIN2.
#define CFW_STY_FAC_TYPE_PF  0x5046 //lock Phone to the very first SIM card
#define CFW_STY_FAC_TYPE_PN  0x504E //Network Personalisation
#define CFW_STY_FAC_TYPE_PU  0x5055 //Network-subset Personalisation
#define CFW_STY_FAC_TYPE_PP  0x5050 //Service-Provider Personalisation
#define CFW_STY_FAC_TYPE_PC  0x5043 //Corporate Personalisation
#define CFW_STY_FAC_TYPE_FD  0x4644 //the phone numbers stored to the "FD" memory can be dialled (depending on the SIM card, usually up to 7 numbers). If PIN2 authentication has not been performed during the current session, PIN2 is requested as <passwd>.

#define CFW_STY_AUTH_READY                 0  //Phone is not waiting for any password.
#define CFW_STY_AUTH_PIN1_READY            1  //Phone is not waiting for PIN1 password.
#define CFW_STY_AUTH_SIMPIN                2  //Phone is waiting for the SIM Personal Identification Number (PIN)
#define CFW_STY_AUTH_SIMPUK                3  //Phone is waiting for the SIM Personal Unlocking Key (PUK).
#define CFW_STY_AUTH_PHONE_TO_SIMPIN       4  //Phone is waiting for the phone-to-SIM card password.
#define CFW_STY_AUTH_PHONE_TO_FIRST_SIMPIN 5  //Phone is waiting for the phone-to-first-SIM card PIN.
#define CFW_STY_AUTH_PHONE_TO_FIRST_SIMPUK 6  //Phone is waiting for the phone-to-first-SIM card PUK.
#define CFW_STY_AUTH_SIMPIN2               7  //Phone is waiting for the SIM PIN2.
#define CFW_STY_AUTH_SIMPUK2               8  //Phone is waiting for the SIM PUK2.
#define CFW_STY_AUTH_NETWORKPIN            9  //Phone is waiting for the network personalization PIN.
#define CFW_STY_AUTH_NETWORKPUK            10 //Phone is waiting for the network personalization PUK.
#define CFW_STY_AUTH_NETWORK_SUBSETPIN     11 //Phone is waiting for the network subset personalization PIN.
#define CFW_STY_AUTHNETWORK_SUBSETPUK      12 //Phone is waiting for the network subset personalization PUK.
#define CFW_STY_AUTH_PROVIDERPIN           13 //Phone is waiting for the service provider personalization PIN.
#define CFW_STY_AUTH_PROVIDERPUK           14 //Phone is waiting for the service provider personalization PUK.
#define CFW_STY_AUTH_CORPORATEPIN          15 //Phone is waiting for the corporate personalization PIN.
#define CFW_STY_AUTH_CORPORATEPUK          16 //Phone is waiting for the corporate personalization PUK.
#define CFW_STY_AUTH_NOSIM                 17 //No SIM inserted.
//
//add by lixp
//
#define CFW_STY_AUTH_PIN1BLOCK             18
#define CFW_STY_AUTH_PIN2BLOCK             19
#define CFW_STY_AUTH_PIN1_DISABLE          20
#define CFW_STY_AUTH_SIM_PRESENT           21
#define CFW_STY_AUTH_SIM_PHASE             22
#define CFW_STY_AUTH_SIM_END               23
#define CFW_STY_AUTH_PIN2_DISABLE          24

typedef struct _CFW_PROFILE
{

    //b0  == FDN
    //b1  == BDN
    //b2  == CC
    //0: Not Support,1: Support
    UINT16 nStart_Profile;

    UINT8 nSat_ProfileLen;
    UINT8 pSat_ProfileData[33];         //  3gpp 11.14
} CFW_PROFILE;

UINT32 SRVAPI CFW_SimChangePassword (
    UINT16 nFac,
    UINT8 *pBufOldPwd,
    UINT8 nOldPwdSize,
    UINT8 *pBufNewPwd,
    UINT8 nNewPwdSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetAuthenticationStatus (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimEnterAuthentication (
    UINT8 *pPin,
    UINT8 nPinSize,
    UINT8 *pNewPin,
    UINT8 nNewPinSize,
    UINT8 nOption,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimSetFacilityLock (
    UINT16 nFac,
    UINT8 *pBufPwd,
    UINT8 nPwdSize,
    UINT8 nMode,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetFacilityLock (
    UINT16 nFac,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetProviderId (
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimSetPrefOperatorList(
    UINT8 *pOperatorList,
    UINT8 nSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetPrefOperatorListMaxNum(
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetPrefOperatorList(
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT8 SRVAPI CFW_SimPbkGetNameLength(
    UINT8 nStorage,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimAddPbkEntry (
    UINT8 nStorage,
    CFW_SIM_PBK_ENTRY_INFO *pEntryInfo,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimDeletePbkEntry (
    UINT8 nStorage,
    UINT16 nIndex,
    UINT16 nUTI ,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetPbkEntry (
    UINT8 nStorage,
    UINT16 nIndex,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimListPbkEntries (
    UINT8 nStorage,
    UINT16 nIndexStart,
    UINT16 nIndexEnd,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimListCountPbkEntries (
    UINT8 nStorage,
    UINT16 nIndexStart,
    UINT16 nCount,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetPbkStrorageInfo (
    UINT8 nStorage,
    UINT16 nUTI ,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SimGetPbkStorage(
    UINT8 nStorage,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);


UINT32 SRVAPI CFW_SatActivation (
    UINT8 nMode,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SatResponse (
    UINT8 nCmdType,
    UINT8 nStatus,
    UINT8 nItemId,
    PVOID pInputString,
    UINT8 InputStrLen,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);

UINT32 SRVAPI CFW_SatGetInformation (
    UINT8 nCmdType,
    PVOID *pCmdData,
    CFW_SIM_ID nSimID
);
UINT8 CFW_SatGetCurCMD(CFW_SIM_ID nSimID);
VOID CFW_GetMNCLen(UINT8 *pLen, CFW_SIM_ID nSimID);

UINT32 CFW_CfgSimGetMeProfile(CFW_PROFILE *pMeProfile);
UINT32 CFW_CfgSimSetMeProfile(CFW_PROFILE *pMeProfile);

UINT32 CFW_SimGetICCID(UINT16 nUTI, CFW_SIM_ID nSimID);


// SAT

typedef struct _CFW_SAT_ITEM_LIST
{
    UINT8 nItemID;
    UINT8 nItemLen;
    UINT8 padding[2];
    UINT8 *pItemStr;
    struct _CFW_SAT_ITEM_LIST *pNextItem;
} CFW_SAT_ITEM_LIST;

//
//modify by lixp
//
#if 0
typedef struct _CFW_SAT_MENU_RSP_LIST
{
    UINT8 nComID;
    UINT8 nComQualifier;
    UINT16 nAlphaLen;
    UINT8 *pAlphaStr;
    CFW_SAT_ITEM_LIST *pItemList;  //Link list
} CFW_SAT_MENU_RSP_LIST;
#endif

UINT32 SRVAPI CFW_SatGetStatus (
    UINT16 nUTI
);

//
// SMS Service
//

typedef struct _CFW_SMS_TXT_DELIVERED_NO_HRD_INFO
{
    UINT8 oa[TEL_NUMBER_MAX_LEN]; // BCD
    UINT8 oa_size;
    UINT8 padding[2];
    TM_SMS_TIME_STAMP scts;
    UINT8 stat;
    UINT8 tooa;
    UINT16 length;
    UINT8 data[1];
} CFW_SMS_TXT_DELIVERED_NO_HRD_INFO;

typedef struct _CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO
{
    UINT8 oa[TEL_NUMBER_MAX_LEN];
    UINT8 oa_size;
    UINT8 sca[TEL_NUMBER_MAX_LEN];
    UINT8 sca_size;
    TM_SMS_TIME_STAMP scts;
    UINT8 tooa;
    UINT8 tosca;
    UINT8 stat;
    UINT8 fo;
    UINT8 pid;
    UINT8 dcs;
    UINT16 length;
    UINT8 data[1];
} CFW_SMS_TXT_DELIVERED_WITH_HRD_INFO;

typedef struct _CFW_SMS_TXT_REPORT_INFO
{
    TM_SMS_TIME_STAMP scts;
    TM_SMS_TIME_STAMP dt;
    UINT8 stat;
    UINT8 fo;
    UINT8 mr;
    UINT8 padding;
    UINT32 st;
} CFW_SMS_TXT_REPORT_V0_INFO;

typedef struct _CFW_SMS_TXT_REPORT_V1_INFO
{
    UINT8 ra[TEL_NUMBER_MAX_LEN];
    UINT8 ra_size;
    TM_SMS_TIME_STAMP scts;
    TM_SMS_TIME_STAMP dt;
    UINT8 tora;
    UINT8 stat;
    UINT8 fo;
    UINT8 mr;
    UINT32 st;
} CFW_SMS_TXT_REPORT_V1_INFO;

typedef struct _CFW_SMS_PDU_INFO
{
    UINT16 nTpUdl;
    UINT16 nDataSize;
    UINT8 nStatus;
    UINT8 padding[3];
    UINT8 pData[1];
} CFW_SMS_PDU_INFO;

typedef struct _CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO
{
    UINT8 da[TEL_NUMBER_MAX_LEN];
    UINT8 da_size;
    UINT8 stat;
    UINT8 padding;
    UINT16 length;
    UINT8 data[1];
} CFW_SMS_TXT_SUBMITTED_NO_HRD_INFO;

typedef struct _CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO
{
    UINT8 da[TEL_NUMBER_MAX_LEN];
    UINT8 da_size;
    UINT8 sca[TEL_NUMBER_MAX_LEN];
    UINT8 sca_size;
    UINT8 toda;
    UINT8 tosca;
    UINT8 stat;
    UINT8 fo;
    UINT8 pid;
    UINT8 dcs;
    UINT8 vp;
    UINT8 padding[3];
    UINT16 length;
    UINT32 nTime;
    UINT8 data[1];
} CFW_SMS_TXT_SUBMITTED_WITH_HRD_INFO;

typedef struct _CFW_SMS_TXT_NO_HRD_IND
{
    UINT8 oa[TEL_NUMBER_MAX_LEN];
    UINT8 oa_size;
    UINT8 tooa;
    UINT8 dcs;
    TM_SMS_TIME_STAMP scts;
    UINT16 nStorageId;
    UINT16 nIndex;
    UINT8 padding;
    UINT16 nDataLen;
    UINT8 pData[1];
} CFW_SMS_TXT_NO_HRD_IND, CFW_SMS_TXT_HRD_V0_IND;

typedef struct _CFW_SMS_TXT_HRD_V1_IND
{
    UINT8 oa[TEL_NUMBER_MAX_LEN];
    UINT8 oa_size;
    UINT8 sca[TEL_NUMBER_MAX_LEN];
    UINT8 sca_size;
    TM_SMS_TIME_STAMP scts;
    UINT8 tosca;
    UINT8 tooa;
    UINT8 fo;
    UINT8 pid;
    UINT8 dcs;
    UINT8 padding;
    UINT16 nStorageId;
    UINT16 nIndex;
    UINT16 nDataLen;
    UINT8 pData[1];
} CFW_SMS_TXT_HRD_V1_IND;

typedef struct _CFW_SMS_TXT_STATUS_IND
{
    UINT8 ra[TEL_NUMBER_MAX_LEN];
    UINT8 ra_size;
    UINT8 tora;
    UINT8 fo;
    UINT8 mr;
    UINT8 st;
    UINT16 nStorageId;
    UINT16 nIndex;
    TM_SMS_TIME_STAMP scts;
    TM_SMS_TIME_STAMP dt;
} CFW_SMS_TXT_STATUS_IND;

typedef struct _CFW_SMS_MULTIPART_INFO
{
    UINT8 id;
    UINT8 count;
    UINT8 current;
    UINT8 padding;
} CFW_SMS_MULTIPART_INFO;

typedef struct _CFW_SMS_LIST
{
    UINT16 nCount;
    UINT16 nStartIndex;
    UINT8 nStorage;
    UINT8 nType;
    UINT8 nStatus;
    UINT8 nOption;
    UINT8 padding[3];
} CFW_SMS_LIST;

typedef struct _CFW_SMS_COPY
{
    UINT8 nOption;
    UINT8 nStatus;
    UINT16 nCount;
    UINT16 nStartIndex;
    UINT8 padding[2];
} CFW_SMS_COPY;

typedef struct _CFW_SMS_PDU_PARAM
{
    UINT8 vp;      //167
    UINT8 pid;     // 0
    UINT8 dcs;     // 0
    UINT8 mti;     // 0x01
    UINT8 ssr;     // 0
    UINT8 bearer;  // 0
    UINT8 mr;      // 0
    UINT8 SCAIndex; // default index
    UINT8 nNumber[12]; // BCD format
    UINT8 rp;
} CFW_SMS_PDU_PARAM;

typedef struct _CFW_SMS_WRITE
{
    CFW_DIALNUMBER sNumber;
    UINT8 *pData;
    UINT16 nDataSize;
    UINT16 nIndex;
    UINT8 nStorage;
    UINT8 nType;
    UINT8 nStatus;
    UINT8 padding[1];
} CFW_SMS_WRITE;

typedef struct _CFW_SMS_NODE
{
    PVOID pNode;
    UINT16 nStorage;
    UINT16 nStatus;
    UINT16 nConcatPrevIndex;
    UINT16 nConcatNextIndex;
    UINT16 nConcatCurrentIndex;
    UINT8 nType;
    UINT8 padding;
} CFW_SMS_NODE;



typedef struct _CFW_NEW_SMS_NODE
{
    PVOID pNode;
    UINT16 nStorage;
    UINT16 nConcatPrevIndex;
    UINT16 nConcatCurrentIndex;
    UINT8 nType;
} CFW_NEW_SMS_NODE;

#define CFW_SMS_LIST_OPTION__TIME_DESCEND_ORDER  0x01
#define CFW_SMS_STORED_STATUS_TIME_ASCEND_ORDER  0x02
#define CFW_SMS_MULTIPART_OPTION_ALL             0x04
#define CFW_SMS_MULTIPART_OPTION_FIRST           0x08
#define CFW_SMS_MULTIPART_OPTION_SIM_BACKUP      0x10
#define CFW_SMS_ROUT_LOCATION                    0x20
#define CFW_SMS_ROUT_DETAIL_INFO                 0x40

#define SCA_MAX_NUMBER  21


typedef struct _CFW_SMS_HEADER
{
    UINT8 pTelNumber[TEL_NUMBER_MAX_LEN];
    UINT8 nNumber_size;
    UINT8 nTelNumberType;
    UINT8 nStatus;
    UINT16 nStorage;
    UINT16 nConcatPrevIndex;
    UINT16 nConcatNextIndex;
    UINT16 nConcatCurrentIndex;
    TM_SMS_TIME_STAMP nTime;
} CFW_SMS_HEADER;


typedef struct _CFW_SMS_SCA
{
    UINT8 nNumber[SCA_MAX_NUMBER];
    UINT8 nType;
    UINT8 nFlag;
    UINT8 padding[1];
} CFW_SMS_SCA;

typedef struct _CFW_SMS_INIT
{
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    BOOL   bInitFlag;
    UINT8  padding[1];
} CFW_SMS_INIT;

#define CFW_CB_MAX_CH 20
#define CFW_CB_MAX_DCS 17

typedef struct _CB_CTX_
{
    UINT16  SimFileSize ;
    UINT8   CbActiveStatus;
    UINT8   CbType;
    UINT8   MidNb;
    UINT16  Mid[CFW_CB_MAX_CH];
    UINT8   MirNb;
    UINT16  Mir[CFW_CB_MAX_CH];
    UINT8   DcsNb;
    UINT8   Dcs[CFW_CB_MAX_DCS];
} CB_CTX_T;

typedef struct
{
    UINT16  MId;        // Message Id the SMS-CB belongs to
    UINT8   DCS;            // Data Coding Scheme applicable to the SMS-CB message
    UINT16  DataLen;
    UINT8   Data[1];        // In fact, DataLen elts must be allocated for this
    // array
} CB_MSG_IND_STRUCT ;

//[[hameina[+]2008.11.21 SMS data len
#define CFW_SMS_LEN_7BIT_NOMAL     160
#define CFW_SMS_LEN_UCS2_NOMAL      140
#define CFW_SMS_LEN_7BIT_LONG          153
#define CFW_SMS_LEN_UCS2_LONG         134

//]]hameina[+]2008.11.21


UINT32 CFW_SmsAbortSendMessage(CFW_SIM_ID nSimID);

PUBLIC UINT32 CFW_SmsDeleteMessage( UINT16 nIndex, UINT8 nStatus, UINT8 nStorage, UINT8 nType, UINT16 nUTI, CFW_SIM_ID nSimId);

UINT32 SRVAPI CFW_SmsSetUnRead2Read (
    UINT8 nStorage,
    UINT8 nType,
    UINT16 nIndex,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);



UINT32 SRVAPI CFW_SmsListMessages(
    CFW_SMS_LIST *pListInfo,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);



UINT32 SRVAPI CFW_SmsCopyMessages(
    CFW_SMS_COPY *pCopyInfo,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);

UINT32 CFW_SmsReadMessage(
    UINT8 nStorage,
    UINT8 nType,
    UINT16 nIndex,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);
UINT32 SRVAPI CFW_SimGetServiceProviderName(UINT16 nUTI, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_SmsSetRead2UnRead (
    UINT8 nStorage,
    UINT8 nType,
    UINT16 nIndex,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);
VOID CFW_CheckIfAllowedRecvSms(BOOL bIfAllowed, CFW_SIM_ID nSimId);

UINT32 SRVAPI CFW_SmsSendMessage (
    CFW_DIALNUMBER *pNumber,
    UINT8 *pData,
    UINT16 nDataSize,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);



UINT32 SRVAPI CFW_SmsWriteMessage(
    CFW_SMS_WRITE *pSMSWrite,
    UINT16 nUTI,
    CFW_SIM_ID nSimId
);


PUBLIC UINT32 CFW_SmsSetUnSent2Sent(UINT8 nStorage, UINT8 nType, UINT16 nIndex, UINT8 nSendStatus, UINT16 nUTI, CFW_SIM_ID nSimId);


UINT32 SRVAPI CFW_SmsComposePdu(
    UINT8 *pTpUd, // "ABC"
    UINT16 nTpUdl, // 3
    CFW_DIALNUMBER *pNumber, // telephone number to send, 135XXX,
    CFW_SMS_PDU_PARAM *pSmsParam,  // NULL
    UINT8 **pSmsData,              // ouput need to maloc.
    CFW_SIM_ID nSimId,
    UINT16 *pSmsDataSize          // size
);


UINT32 SRVAPI CFW_SmsGetMessageNode (
    PVOID pListResult,
    UINT16 nIndex,
    CFW_SMS_NODE **pNode,
    CFW_SIM_ID nSimId
);

UINT32 CFW_SmsInitComplete(
    UINT16 nUTI,
    CFW_SIM_ID nSIMID
);

UINT32 CFW_SetSMSConcat (
    BOOL bConcat,
    CFW_SIM_ID nSimID
);


typedef struct _CFW_EMS_INFO
{
    UINT8   nMaxNum;      //Maximum number of short messages in the enhanced concatenated short message.
    UINT8   nCurrentNum;  //Sequence number of the current short message
    UINT16  nRerNum;      //reference number
    UINT16  nDesPortNum;  //Destination port
    UINT16  nOriPortNum;  //Originator port
} CFW_EMS_INFO;



// CC Network Error code
#define    CFW_CC_CAUSE_NOUSED_CAUSE                                        0
#define    CFW_CC_CAUSE_UNASSIGNED_NUMBER                                   1
#define    CFW_CC_CAUSE_NO_ROUTE_IN_TRANSIT_NETWORK                         2
#define    CFW_CC_CAUSE_NO_ROUTE_TO_DESTINATION                             3
#define    CFW_CC_CAUSE_CHANNEL_UNACCEPTABLE                                6
#define    CFW_CC_CAUSE_CALL_AWARDED                                        7
#define    CFW_CC_CAUSE_OPERATOR_DETERMINED_BARRING                         8
#define    CFW_CC_CAUSE_NORMAL_CALL_CLEARING                                16
#define    CFW_CC_CAUSE_USER_BUSY                                           17
#define    CFW_CC_CAUSE_NO_USER_RESPONDING                                  18
#define    CFW_CC_CAUSE_USER_ALERTING_NO_ANSWER                             19
#define    CFW_CC_CAUSE_CALL_REJECTED                                       21
#define    CFW_CC_CAUSE_NUMBER_CHANGED                                      22
#define    CFW_CC_CAUSE_PRE_EMPTION                                         25
#define    CFW_CC_CAUSE_NON_SELECTED_USER_CLEARING                          26
#define    CFW_CC_CAUSE_DESTINATION_OUT_OF_ORDER                            27
#define    CFW_CC_CAUSE_INVALID_NUMBER_FORMAT                               28
#define    CFW_CC_CAUSE_FACILITY_REJECTED                                   29
#define    CFW_CC_CAUSE_RESPONSE_TO_STATUS_ENQUIRY                          30
#define    CFW_CC_CAUSE_NORMAL_UNSPECIFIED                                  31
#define    CFW_CC_CAUSE_NO_CIRCUIT_CHANNEL_AVAILABLE                        34
#define    CFW_CC_CAUSE_NETWORK_OUT_OF_ORDER                                38
#define    CFW_CC_CAUSE_TEMPORARY_FAILURE                                   41
#define    CFW_CC_CAUSE_SWITCHING_EQUIPMENT_CONGESTION                      42
#define    CFW_CC_CAUSE_ACCESS_INFORMATION_DISCARDED                        43
#define    CFW_CC_CAUSE_CHANNEL_UNAVAILABLE                                 44
#define    CFW_CC_CAUSE_RESOURCES_UNAVAILABLE                               47
#define    CFW_CC_CAUSE_QOS_UNAVAILABLE                                     49
#define    CFW_CC_CAUSE_FACILITY_NOT_SUBSCRIBED                             50
#define    CFW_CC_CAUSE_MT_CALLS_BARRED                                     55
#define    CFW_CC_CAUSE_BC_NOT_AUTHORIZED                                   57
#define    CFW_CC_CAUSE_BC_NOT_PRESENTLY_AVAILABLE                          58
#define    CFW_CC_CAUSE_SERVICE_UNAVAILABLE                                 63
#define    CFW_CC_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED                      65
#define    CFW_CC_CAUSE_CHANNEL_NOT_IMPLEMENTED                             66
#define    CFW_CC_CAUSE_ACM_EQUAL_TO_OR_GREATER_THAN_ACMMAX                 68
#define    CFW_CC_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED                  69
#define    CFW_CC_CAUSE_ONLY_RESTRICTED_DIGITAL_INFORMATION_BC_AVAILABLE    70
#define    CFW_CC_CAUSE_SERVICE_NOT_IMPLEMENTED                             71
#define    CFW_CC_CAUSE_INVALID_TI_VALUE                                    81
#define    CFW_CC_CAUSE_CHANNEL_DOES_NOT_EXIST                              82
#define    CFW_CC_CAUSE_CALL_IDENTITY_DOESNT_EXIST                          83
#define    CFW_CC_CAUSE_CALL_IDENTITY_IN_USE                                84
#define    CFW_CC_CAUSE_NO_CALL_SUSPENDED                                   85
#define    CFW_CC_CAUSE_CALL_CLEARED                                        86
#define    CFW_CC_CAUSE_USER_NOT_MEMBER_OF_CUG                              87
#define    CFW_CC_CAUSE_INCOMPATIBLE_DESTINATION                            88
#define    CFW_CC_CAUSE_INVALID_TRANSIT_NETWORK                             91
#define    CFW_CC_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE                      94
#define    CFW_CC_CAUSE_INVALID_MESSAGE                                     95
#define    CFW_CC_CAUSE_INVALID_MANDATORY_INFORMATION                       96
#define    CFW_CC_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED                        97
#define    CFW_CC_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE                         98
#define    CFW_CC_CAUSE_IE_NOT_IMPLEMENTED                                  99
#define    CFW_CC_CAUSE_CONDITIONAL_IE_ERROR                                100
#define    CFW_CC_CAUSE_MESSAGE_NOT_COMPATIBLE                              101
#define    CFW_CC_CAUSE_RECOVERY_ON_TIMER_EXPIRY                            102
#define    CFW_CC_CAUSE_PROTOCOL_ERROR_UNSPECIFIED                          111
#define    CFW_CC_CAUSE_INTERWORKING_UNSPECIFIED                            127
#define    CFW_CM_CAUSE_SUCCESS                                             255


// CC local error
#define CFW_CC_ERR_NORMAL_RELEASE          0
#define CFW_CC_ERR_LOW_LAYER_FAIL          1
#define CFW_CC_ERR_REQUEST_IMPOSSIBLE      2
#define CFW_CC_ERR_INCOMING_CALL_BARRED    3
#define CFW_CC_ERR_OUTGOING_CALL_BARRED    4
#define CFW_CC_ERR_CALL_HOLD_REJECTED      5
#define CFW_CC_ERR_CALL_RTRV_REJECTED      6
#define CFW_CC_ERR_CC_TIMER_EXPIRY         7
#define CFW_CC_ERR_CC_PROTOCOL_ERROR       8
#define CFW_CC_ERR_NOT_USED                0xff

// Crss error
#define    CFW_CRSS_ERR_RETURN_ERROR                                    0x0002
#define    CFW_CRSS_ERR_GENERAL_PROBLEM_UNRECOGNIZED_COMPONENT          0x8000
#define    CFW_CRSS_ERR_GENERAL_PROBLEM_MISTYPED_COMPONENT              0x8001
#define    CFW_CRSS_ERR_GENERAL_PROBLEM_BADLY_STRUCTURED_COMPONENT      0x8002
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_DUPLICATE_INVOKE_ID              0x8100
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_UNRECOGNIZED_OPERATION           0x8101
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_MISTYPED_PARAMETER               0x8102
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_RESOURCE_LIMITATION              0x8103
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_INITIATING_RELEASE               0x8104
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_UNRECOGNIZED_LINKED_ID           0x8105
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_LINKED_RESPONSE_UNEXPECTED       0x8106
#define    CFW_CRSS_ERR_INVOKE_PROBLEM_UNEXPECTED_LINKED_OPERATION      0x8107
#define    CFW_CRSS_ERR_RETURN_RESULT_PROBLEM_UNRECOGNIZED_INVOKE_ID    0x8200
#define    CFW_CRSS_ERR_RETURN_RESULT_PROBLEM_RETURN_RESULT_UNEXPECTED  0x8201
#define    CFW_CRSS_ERR_RETURN_RESULT_PROBLEM_MISTYPED_PARAMETER        0x8202
#define    CFW_CRSS_ERR_RETURN_ERROR_PROBLEM_UNRECOGNIZED_INVOKE_ID     0x8300
#define    CFW_CRSS_ERR_RETURN_ERROR_PROBLEM_RETURN_ERROR_UNEXPECTED    0x8301
#define    CFW_CRSS_ERR_RETURN_ERROR_PROBLEM_UNRECOGNIZED_ERROR         0x8302
#define    CFW_CRSS_ERR_RETURN_ERROR_PROBLEM_UNEXPECTED_ERROR           0x8303
#define    CFW_CRSS_ERR_RETURN_ERROR_PROBLEM_MISTYPED_PARAMETER         0x8304


//SS code
#define    CFW_SS_NO_CODE
#define    CFW_SS_CLIP   30                       // CLIP
#define    CFW_SS_CLIR   31                       // CLIR
#define    CFW_SS_COLP   76                       // COLP
#define    CFW_SS_COLR   77                       // COLR
#define    CFW_SS_CNAP   300                      // CNAP
#define    CFW_SS_CFA   2                         // All forwarding
#define    CFW_SS_CFU   21                        // CF unconditional
#define    CFW_SS_CD   66                         // call deflection
#define    CFW_SS_CFC   4                         // CF conditional
#define    CFW_SS_CFB   67                        // CF on MS busy
#define    CFW_SS_CFNRY   61                      // CF on MS no reply
#define    CFW_SS_CFNRC   62                      // CF on MS not reachable
#define    CFW_SS_USSD_V2   100                   // USSD version 2 services  supports
// all USSD Operations except
// ss_ProcessUnstructuredSSData
#define    CFW_SS_USSD_V1   101                   // USSD version 1 services  supports
// ss_ProcessUnstructuredSSData
// Operations Only
#define    CFW_SS_ECT    96                       // explicit call transfer
#define    CFW_SS_CW     43                       // call waiting
#define    CFW_SS_HOLD   102                      // call hold  (defined by CIITEC)
#define    CFW_SS_MPTY   103                      // multiparty (defined by CIITEC)
#define    CFW_SS_AOCI   104                      // advice of charge information (defined by CIITEC)
#define    CFW_SS_AOCC   105                      // advice of charge charging    (defined by CIITEC)
#define    CFW_SS_UUS1   361                      // uus1
#define    CFW_SS_UUS2   362                      // uus2
#define    CFW_SS_UUS3   363                      // uus3
#define    CFW_SS_AllBarringSS   330              // all call barring
#define    CFW_SS_BarringOfOutgoingCalls   333    // barring of MO call
#define    CFW_SS_BAOC   33                       // barring of all MO call
#define    CFW_SS_BOIC   331                      // barring of international MO call
#define    CFW_SS_BOIC_ExHC   332                 // barring of international MO call
// except those going to the home plmn
#define    CFW_SS_BarringOfIncomingCalls   353    // barring of MT call
#define    CFW_SS_BAIC   35                       // barring of all MT call
#define    CFW_SS_BIC_ROAM   351                  // barring of MT call when roaming
// outside of home plmn

//SS operation code
#define    CFW_SS_REGISTER   10                       // register a SS valid for CF
#define    CFW_SS_ERASURE   11                        // erase a SS valid for CF
#define    CFW_SS_ACTIVATE   12                       // activate a SS valid for all
// SS operations except line
// identification
#define    CFW_SS_DEACTIVATE   13                     // deactivate a SS valid for all
// SS operations except line
// identification
#define    CFW_SS_INTERROGATE   14                    // interrogate a SS
#define    CFW_SS_NOTIFY   16
#define    CFW_SS_REGISTERPASSWORD   17               // register a password valid for CB
#define    CFW_SS_GETCALLBARRINGPASSWORD   18         // this operation is invoked by
// network only
#define    CFW_SS_PROCESSUNSTRUCTUREDSSDATA   19      // This operation is used only
// for USSD Version 1
#define    CFW_SS_PROCESSUNSTRUCTUREDSSREQUEST   59   // This operation is used only
// for USSD Version 2
#define    CFW_SS_UNSTRUCTUREDSSREQUEST   60          // This operation is used only
// for USSD Version 2
#define    CFW_SS_UNSTRUCTUREDSSNOTIFY   61           // This operation is used only
// for USSD Version 2
#define    CFW_SS_CALLDEFLECTION    117
#define    CFW_SS_USERUSERSERVICE   118
#define    CFW_SS_SPLITMPTY         121
#define    CFW_SS_RETRIEVEMPTY      122
#define    CFW_SS_HOLDMPTY          123
#define    CFW_SS_BUILDMPTY         124
#define    CFW_SS_FORWARDCHARGEADVICE   125
#define    CFW_SS_EXPLICITCT            126

//
// PBK Services
//

#define CFW_PBK_DATA_MAX_SIZE       20
#define PBK_SIM_NUMBER_SIZE         SIM_PBK_NUMBER_SIZE
#define CFW_PBK_NAME_MAX_SIZE       20
#define CFW_PBK_NUMBER_MAX_SIZE     20 // BCD

#define PBK_HINT_SIZE               16

typedef struct _CFW_PBK_ENTRY_SIMPLE_HRD
{
    UINT16 nPosition;
    UINT8 nStorageId;
    UINT8 nNameSize;
    UINT8 nNumberSize;
    UINT8 nNumberType;
    UINT8 padding[2];
    UINT8 *pName;
    UINT8 *pNumber;
} CFW_PBK_ENTRY_SIMPLE_HRD;

typedef struct _CFW_PBK_ENTRY_HRD
{
    UINT16 nPosition;
    UINT16 nAppIndex;
    UINT8 *pName;
    UINT8 *pNumber;
    UINT8 nNameSize;
    UINT8 nNumberSize;
    UINT8 nNumberType;
    UINT8 nStorageId;
    UINT32 nItemGroupType;
    UINT32 nGroup;
} CFW_PBK_ENTRY_HRD;

typedef struct _CFW_PBK_ENTRY_ITEM
{
    UINT32 nPosition;
    UINT32 nItemType;
    UINT32 nItemId;
    UINT8 *pItemName;
    UINT8 *pItemData;
    UINT8 nItemNameSize;
    UINT8 nItemDataSize;
    UINT8 nDataType;
    UINT8 padding;
} CFW_PBK_ENTRY_ITEM;

typedef struct _CFW_PBK_DETAIL_ENTRY
{
    CFW_PBK_ENTRY_HRD sHeader;
    UINT32 iItemNum;
    CFW_PBK_ENTRY_ITEM *pItemArray;
} CFW_PBK_DETAIL_ENTRY;

#define CFW_PBK_SEARCH_NAME_FIRST_MATCH  0x01
#define CFW_PBK_SEARCH_NAME_FULL_MATCH   0x02
#define CFW_PBK_SEARCH_NAME_PY_MATCH     0x04
#define CFW_PBK_SEARCH_NUM_FIRST_MATCH   0x08
#define CFW_PBK_SEARCH_NUM_NEXT_MATCH    0x10
#define CFW_PBK_SEARCH_ENTRY_SIMPLE_INFO 0x20
#define CFW_PBK_SEARCH_ENTRY_HEADER_INFO 0x40
#define CFW_PBK_SEARCH_ENTRY_FULL_INFO   0x80

#define CFW_PBK_OPTION_ENTRY_SIMPLE_HEADER 0x01
#define CFW_PBK_OPTION_ENTRY_FULL_HEADER   0x02
#define CFW_PBK_OPTION_ENTRY_ITEM          0x04

#define CFW_PBK_COPY_ME2SM      0x00
#define CFW_PBK_COPY_SM2ME      0x01
#define CFW_PBK_COPY_ME2SM_STEP 0x02
#define CFW_PBK_COPY_SM2ME_STEP 0x04

#define CFW_PBK_ITEM_TYPE_PHONE_NUMBER   0x1 //Specify the phone number such as mobile number or home number or office numberW
#define CFW_PBK_ITEM_TYPE_EAMIL          0x2 //
#define CFW_PBK_ITEM_TYPE_ADDERSS        0x4 //
#define CFW_PBK_ITEM_TYPE_WEBSITE        0x8 //
#define CFW_PBK_ITEM_TYPE_STRING         0x10 //
#define CFW_PBK_ITEM_TYPE_RES_ID         0x20 // Specify the resource identifier
#define CFW_PBK_ITEM_TYPE_RES_PATH       0x40 // Specify the resource identifier or resource file path such as icon name.

typedef struct _CFW_PBK_CAPACITY_INFO
{
    UINT16 nTotalNum;
    UINT8 nPhoneNumberLen;
    UINT8 nTextLen;
    UINT16 nUsedNum;
    UINT8 nStorageId;
    UINT8 padding;
} CFW_PBK_CAPACITY_INFO;

typedef struct _CFW_PBK_CALLLOG_ENTRY
{
    UINT8 *pNumber;
    UINT8 nNumberSize;
    UINT8 nNumberType;
    UINT16 nPosition;
    UINT8 nTimes;
    UINT8 nStorageId;
    UINT16 nDuration;
    UINT32 nStartTime;
} CFW_PBK_CALLLOG_ENTRY;

#ifdef CFW_PBK_SYNC_VER

// Synchronous Function for PBK.
typedef struct _CFW_PBK_OUT_PARAM
{
    UINT8 nType;
    UINT8 padding[3];
    UINT32 nParam1;
    UINT32 nParam2;
} CFW_PBK_OUT_PARAM;

UINT32 SRVAPI CFW_PbkAddEntry (
    CFW_PBK_ENTRY_HRD *pEntryHrd,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkUpdateEntry (
    CFW_PBK_ENTRY_HRD *pEntryHrd,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkDeleteEntry (
    UINT8 nStorageId,
    UINT16 nPosition,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkDeleteBatchEntries (
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nStorageId,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkFindEntry (
    UINT8 *pString,
    UINT8 nStringSize,
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nStorageId,
    UINT16 nGroup,
    UINT16 nOption,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkListEntries (
    UINT16 nStartPos,
    UINT16 nPageCount,
    UINT16 nSkipPage,
    UINT8 nStorageId,
    UINT16 nGroup,
    UINT8 nOption,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkGetEntry (
    UINT16 nPosition,
    UINT8 nStorageId,
    UINT8 nOption,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkCopyEntries (
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nOption,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkAddEntryItem (
    CFW_PBK_ENTRY_ITEM *pEntryItem,
    UINT8 nCount,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkGetEntryItems (
    UINT16 nPosition,
    UINT32 nItemGroupType,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkDeleteEntryItem (
    UINT16 nItemId,
    /*UINT32 nItemGroupType,*/
    CFW_PBK_OUT_PARAM *pOutParam
);


UINT32 SRVAPI CFW_PbkGetCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pCapInfo
);

UINT32 SRVAPI CFW_PbkGetCalllogCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pCapInfo
);

UINT32 SRVAPI CFW_PbkAddCallLogEntry (
    CFW_PBK_CALLLOG_ENTRY *pEntryInfo,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkGetCallLogEntry (
    UINT16 nPosition,
    UINT8 nStorageId,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkListCallLogEntries (
    UINT16 nStartPosition,
    UINT16 nCount,
    UINT8 nStorage,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkDeleteBatchCallLogEntries (
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nStorageId,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI CFW_PbkGetCalllogCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pCapInfo
);
#else

UINT32 SRVAPI CFW_PbkAddEntry (
    CFW_PBK_ENTRY_HRD *pEntryHrd,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkUpdateEntry (
    CFW_PBK_ENTRY_HRD *pEntryHrd,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkDeleteEntry (
    UINT8 nStorageId,
    UINT16 nPosition,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkDeleteBatchEntries (
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nStorageId,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkFindEntry (
    UINT8 *pString,
    UINT8 nStringSize,
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nStorageId,
    UINT16 nGroup,
    UINT16 nOption,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkListEntries (
    UINT16 nStartPos,
    UINT16 nPageCount,
    UINT16 nSkipPage,
    UINT8 nStorageId,
    UINT16 nGroup,
    UINT8 nOption,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkGetEntry (
    UINT16 nPosition,
    UINT8 nStorageId,
    UINT8 nOption,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkCopyEntries (
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nOption,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkAddEntryItem (
    CFW_PBK_ENTRY_ITEM *pEntryItem,
    UINT8 nCount,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkGetEntryItems (
    UINT16 nPosition,
    UINT32 nItemGroupType,
    UINT16 nUTI
);
//
//modify by lixp
//
#if 0
UINT32 SRVAPI CFW_PbkDeleteEntryItem (
    UINT16 nItemId,
    UINT32 nItemGroupType,
    UINT16 nUTI
);
#endif
UINT32 SRVAPI CFW_PbkDeleteEntryItem (
    UINT16 nItemId,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkGetCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pCapInfo
);

UINT32 SRVAPI CFW_PbkGetCalllogCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pCapInfo
);

UINT32 SRVAPI CFW_PbkAddCallLogEntry (
    CFW_PBK_CALLLOG_ENTRY *pEntryInfo,
    UINT16 nUTI
);

//
//modify by lixp
//
#if 0
UINT32 SRVAPI CFW_PbkGetCallLogEntry (
    UINT16 nPosition,
    UINT8 nStorageId,
    UINT16 nUTI
);
#endif
UINT32 SRVAPI CFW_PbkGetCallLogEntry (
    UINT16 nPosition,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkListCallLogEntries (
    UINT16 nStartPosition,
    UINT16 nCount,
    UINT8 nStorage,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkDeleteBatchCallLogEntries (
    UINT16 nStartPos,
    UINT16 nCount,
    UINT8 nStorageId,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_PbkGetCalllogCapacityInfo (
    UINT8 nStorageId,
    CFW_PBK_CAPACITY_INFO *pCapInfo
);

UINT32 SRVAPI CFW_PbkCancel (
    UINT16 nOption,
    UINT16 nUTI
);

#endif

//
// GPRS
//
#define CFW_GPRS_DETACHED       0   //detached
#define CFW_GPRS_ATTACHED       1   //attached

#define CFW_GPRS_DEACTIVED      0    //deactivated
#define CFW_GPRS_ACTIVED        1    //activated

// error define
#define ERR_CFW_GPRS_INVALID_CID       0
#define ERR_CFW_GPRS_HAVE_ACTIVED      1
#define ERR_CFW_GPRS_HAVE_DEACTIVED      2 //add wuys 2008-06-03

// state self defined
#define CFW_GPRS_STATE_IDLE                 0
#define CFW_GPRS_STATE_CONNECTING           2
#define CFW_GPRS_STATE_DISCONNECTING        3
#define CFW_GPRS_STATE_ACTIVE               4
#define CFW_GPRS_STATE_MODIFYING            5
#ifdef LTE_NBIOT_SUPPORT
#define CFW_GPRS_STATE_EPS_ALLOCATING       6
#define CFW_GPRS_STATE_ERROR                7
#else
#define CFW_GPRS_STATE_ERROR                6
#endif


// Packet Data Protocol type
#define CFW_GPRS_PDP_TYPE_X25   0   // x.25
#define CFW_GPRS_PDP_TYPE_IP    1   // IP
#define CFW_GPRS_PDP_TYPE_IPV6  2   // IP Version 6
#define CFW_GPRS_PDP_TYPE_OSPIH 1   // internet Hosted Octect Stream Protocol
#define CFW_GPRS_PDP_TYPE_PPP   1   // Point to Point Protocol
#define CFW_GPRS_PDP_TYPE_IPV4V6  3   //DualStack
#define CFW_GPRS_PDP_TYPE_UNUSED 4  //according to TS24.301 R14 9.9.4.10, shall be interpreted as "IPV6" if received by the network, here reserved
#ifdef LTE_NBIOT_SUPPORT
#define CFW_GPRS_PDP_TYPE_NONIP 5 /*value set according to TS24.301 R14 9.9.4.10*/
#endif


// PDP data compression
#define CFW_GPRS_PDP_D_COMP_OFF  0   // off (default if value is omitted)
#define CFW_GPRS_PDP_D_COMP_ON   1   // on(manufacturer preferred compression)
#define CFW_GPRS_PDP_D_COMP_V42  2   // V.42bis
#define CFW_GPRS_PDP_D_COMP_V44  3   // V.44

// PDP header compression
#define CFW_GPRS_PDP_H_COMP_OFF         0   // off (default if value is omitted)
#define CFW_GPRS_PDP_H_COMP_ON          1   // on (manufacturer preferred compression)
#define CFW_GPRS_PDP_H_COMP_RFC1144     2   // RFC1144 (applicable for SNDCP only)
#define CFW_GPRS_PDP_H_COMP_RFC2507     3   // RFC2507
#define CFW_GPRS_PDP_H_COMP_RFC3095     4   // RFC3095 (applicable for PDCP only)

//L2P
#define CFW_GPRS_L2P_NULL               0 // NULL (Obsolete)
#define CFW_GPRS_L2P_PPP                1 // PPP
#define CFW_GPRS_L2P_PAD                2 // PAD (Obsolete)
#define CFW_GPRS_L2P_X25                3 // x25 (Obsolete)

// Automatic response to a network request for PDP context activation
#define CFW_GPRS_AUTO_RESPONSE_PACKET_DOMAIN_OFF 0  // turn off automatic response for Packet Domain only
#define CFW_GPRS_AUTO_RESPONSE_PACKET_DOMAIN_ON  1  // turn on automatic response for Packet Domain only
#define CFW_GPRS_AUTO_RESPONSE_PS_ONLY           2  // modem compatibility mode, Packet Domain only
#define CFW_GPRS_AUTO_RESPONSE_PS_CS             3  // modem compatibility mode, Packet Domain and circuit switched calls (default)
#ifdef CFW_GPRS_SUPPORT

typedef struct _CFW_GPRS_PDPCONT_INFO
{
    UINT8  nPdpType;
    UINT8  nDComp;
    UINT8  nHComp;
    UINT8  nApnSize;
    UINT8  nPdpAddrSize;
    UINT8  nApnUserSize;
    UINT8  nApnPwdSize;
    UINT8  padding[1];
    UINT8 *pApnUser;
    UINT8 *pApnPwd;
    UINT8 *pApn;
    UINT8 *pPdpAddr;
#ifdef LTE_NBIOT_SUPPORT
    UINT8  PdnType; // IPV4/V6/Non-IP
    UINT8  nSlpi;	 //indicates the NAS signalling priority requested for this PDP context
#endif

} CFW_GPRS_PDPCONT_INFO;

typedef struct _CFW_GPRS_QOS
{
    UINT8 nPrecedence;
    UINT8 nDelay;
    UINT8 nReliability;
    UINT8 nPeak;
    UINT8 nMean;
    UINT8 padding[3];
#ifdef LTE_NBIOT_SUPPORT
    UINT16 nTrafficClass;
    UINT16 nMaximumbitrateUL;
    UINT16 nMaximumbitrateDL;
    UINT16 nGuaranteedbitrateUL;
    UINT16 nGuaranteedbitrateDL;
    UINT16 nDeliveryOrder;
    UINT16 nMaximumSDUSize;
    UINT16 nDeliveryOfErroneousSDU;
    UINT16 nTransferDelay;
    UINT16 nTrafficHandlingPriority;
    UINT16 nSDUErrorRatio;
    UINT16 nResidualBER;
#endif
} CFW_GPRS_QOS;

#ifdef LTE_NBIOT_SUPPORT
typedef struct _CFW_EQOS
{
    UINT8  nQci;
    UINT32 nDlGbr;
    UINT32 nUlGbr;
    UINT32 nDlMbr;
    UINT32 nUlMbr;
} CFW_EQOS;

typedef struct _CFW_TFT
{
    UINT8 uLength;
    UINT8 uTFT[255];            //257 - 3 should be OK.
} CFW_TFT;

typedef struct _CFW_TFT_SET
{
    UINT16 Pf_BitMap; /*LSB: 0, 1, 2, 3, ...  */
    UINT8  Pf_ID;                /*LSB: Bit 0  */
    UINT8  Pf_EvaPreIdx;         /*LSB: Bit 1  */
    UINT8  Pf_RmtAddrIPv4[8];    /*LSB: Bit 2  */
    UINT8  Pf_RmtAddrIPv6[32];   /*LSB: Bit 3  */
    UINT8  Pf_PortNum_NH;        /*LSB: Bit 4  */
    UINT16 Pf_LocalPortRangeMin; /*LSB: Bit 5  */
    UINT16 Pf_LocalPortRangeMax;
    UINT16 Pf_RemotPortRangeMin; /*LSB: Bit 6  */
    UINT16 Pf_RemotPortRangeMax;
    UINT32 Pf_SPI;               /*LSB: Bit 7  */
    UINT8  Pf_TOS_TC[2];         /*LSB: Bit 8  */
    UINT32 Pf_FlowLabel;         /*LSB: Bit 9  */
    UINT8  Pf_Direct;            /*LSB: Bit 10  */
    UINT8  Pf_LocalAddrIPv4[8];  /*LSB: Bit 11  */
    UINT8  Pf_LocalAddrIPv6[32]; /*LSB: Bit 12  */

    struct _CFW_TFT_SET* pNext;
} CFW_TFT_SET;


/* Packet Fileter component */
typedef struct _CFW_PF_CPNT
{
/*  IPv4 remote address type:               8 octet (a four octet IPv4 address field and a four octet IPv4 address mask field)
    IPv4 local address type:                8 octet (a four octet IPv4 address field and a four octet IPv4 address mask field)
    IPv6 remote address type:               32 octet (a sixteen octet IPv6 address field and a sixteen octet IPv6 address mask field)
    IPv6 remote address/prefix length type: 17 octet(a sixteen octet IPv6 address field and one octet prefix length field)
    IPv6 local address/prefix length type:  17 octet(a sixteen octet IPv6 address field and one octet prefix length field)
    Protocol identifier/Next header type:   one octet
    Single local port type:                 two octet
    Local port range type:                  4 octet (a two octet port range low limit field and a two octet port range high limit field)
    Single remote port type:                two octet
    Remote port range type:                 4 octet (a two octet port range low limit field and a two octet port range high limit field)
    Security parameter index type:          four octet
    Type of service/Traffic class type:     2 octet (a one octet Type-of-Service/Traffic Class field and a one octet Type-of-Service/Traffic Class mask field)
    Flow label type:                        three octet
*/
    UINT8 uCpntType;      /* Packet filter component type identifier */
    UINT8 uCpntLen;       /* Packet filter component Length */
    UINT8 uCpntCont[32];  /* Packet filter component Content */

    struct _CFW_PF_CPNT* pNext;
}CFW_PF_CPNT;

/* One Packet Fileter */
typedef struct _CFW_TFT_PF
{
    UINT8 uEBI;
    UINT8 uPF_Dir;    /* Packet filter direction */
    UINT8 uPF_ID;     /* Packet filter identifier */
    UINT8 uPF_Pri;    /* Packet filter evaluation precedence */

    CFW_PF_CPNT *Cpnt; /* Packet filter Component */

    /* Next Component */
    struct _CFW_TFT_PF *pNext;
} CFW_TFT_PF;

typedef struct
{
    UINT8       Ebi;
    UINT8       LinkEbi;
    UINT8       EbiDeact[7];
    UINT8       EbiDeactNum;
    UINT8       TftOpCode;
    CFW_TFT_PF* pTftPf;
}CFW_TFT_PARSE;
#endif // LTE_NBIOT_SUPPORT

typedef struct _CFW_GPRS_DATA
{
    UINT16 nDataLength;
    UINT8 padding;
    UINT8 pData[1];
} CFW_GPRS_DATA;
#ifdef __USE_SMD__


//#include <stack_ipc.h>
typedef struct
{
    UINT32   w_index;
    UINT32   r_index;
} PSCMD_RECV_MSG_T;
#define SMD_BUFFER_LEN  1504
#define RESERVED_LEN    96
#define MEMORY_BASE_ADDR  0xa2e00000

typedef struct
{
    UINT32         nextOffset;
    UINT8          cid;
    UINT8          simid;
    UINT16        length;
    UINT8          reserved2[RESERVED_LEN-8];
} IP_DATA_HEADER_T;

typedef struct
{
    IP_DATA_HEADER_T    hdr;
    UINT8          ipData[SMD_BUFFER_LEN];
} IP_DATA_BUFFER_T;


UINT16  smd_alloc_ul_ipdata_buf(UINT32 *offSet, UINT8 count);

UINT16  smd_alloc_dl_ipdata_buf(UINT32 *offSet, UINT8 count);
void smd_free_ul_ipdata_buf(UINT32 offSet);

void smd_free_dl_ipdata_buf(UINT32 offSet);
#endif
//synchronization function
// AT+CGDCONT
UINT32 SRVAPI CFW_GprsSetPdpCxt(
    UINT8 nCid,
    CFW_GPRS_PDPCONT_INFO *pPdpCont,
    CFW_SIM_ID nSimID
); // [in]nCid, [in]pPdpCont

UINT32 SRVAPI CFW_GprsGetPdpCxt(
    UINT8 nCid,
    CFW_GPRS_PDPCONT_INFO *pPdpCont,
    CFW_SIM_ID nSimID
); // [in]nCid, [out]pPdpCont

// AT+CGQMIN
// This command allows the TE to specify a Min Quality of Service Profile
// that is used when the MT sends an Activate PDP Context Request message
// to the network.
UINT32 SRVAPI CFW_GprsSetMinQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);

UINT32 SRVAPI CFW_GprsGetMInQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);


// AT+CGQREQ
// This command allows the TE to specify a Quality of Service Profile
// that is used when the MT sends an Activate PDP Context Request message
// to the network.
UINT32 SRVAPI CFW_GprsSetReqQos(
    UINT8 nCid,
    CFW_GPRS_QOS *pQos,
    CFW_SIM_ID nSimID
); // [in]nCid, [in]pQos

UINT32 SRVAPI CFW_GprsGetReqQos(
    UINT8 nCid,
    CFW_GPRS_QOS *pQos,
    CFW_SIM_ID nSimID); // [in]nCid, [out]pQos

//AT+CGPADDR
//get the address of the selected Pdp context of the gprs network
//[in]nCid: [1-7]
// Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition.
//[in/out]nLength: [in] nLength should be the length that malloc the pdpAdd.
//                 [out] nLength should be the length of the Pdp address.
//[out]pPdpAdd: Point to the buffer to hold the PDP address.
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,

UINT32 SRVAPI CFW_GprsGetPdpAddr(
    UINT8 nCid, UINT8 *nLength,
    UINT8 *pPdpAdd
    , CFW_SIM_ID nSimID
);

// using net parameter
//AT+CGREG
// get the status of the gprs network.
// [out]pStatus: Pointer to CFW_NW_STATUS_INFO structure to contain the network status information
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,
UINT32 SRVAPI CFW_GprsGetstatus(
    CFW_NW_STATUS_INFO *pStatus,
    CFW_SIM_ID nSimID
);  //

// AT+CGATT?
// retrieves the state of the GPRS service.
// [out]pState: Point to UINT8 to hold the state of GPRS attachment.
// This parameter can be one of the following value:
// CFW_GPRS_DETACHED 0 detached
// CFW_GPRS_ATTACHED 1 attached
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,

UINT32 SRVAPI CFW_GetGprsAttState (
    UINT8 *pState,
    CFW_SIM_ID nSimID
);

//
// AT+CGACT?
// retrieves the state of the GPRS Service according to the selected cid
// [in]nCid:[1-7]
// Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition.
// [out]pState:Point to UINT8 to hold the state of GPRS activation.
// CFW_GPRS_DEACTIVED 0 deactivated
// CFW_GPRS_ACTIVED 1 activated
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,

UINT32 SRVAPI CFW_GetGprsActState (
    UINT8 nCid,
    UINT8 *pState,
    CFW_SIM_ID nSimID
);

// asynchronism
////------> move to Net

// AT+CGATT
// ps attach and detach
// nState:
// CFW_GPRS_DETACHED 0 detached
// CFW_GPRS_ATTACHED 1 attached
// event:EV_CFW_GPRS_ATT_NOTIFY
// parameter1: 0
// parameter2: 0
// type: 0 or 1 to distinguish the attach or detach
//UINT32 SRVAPI CFW_GprsAtt(UINT8 nState, UINT16 nUTI);    // notify: success or fail

// AT: AT+CGACT
// PDP context activate or deactivate
// nState:
// CFW_GPRS_DEACTIVED 0 deactivated
// CFW_GPRS_ACTIVED 1 activated
// event:EV_CFW_GPRS_ACT_NOTIFY
// parameter1:0
// parameter2:0
// type: 0 or 1 to distingush the active or deactive
UINT32 SRVAPI CFW_GprsAct(
    UINT8 nState,
    UINT8 nCid,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);     // [in] nCid  // notify: success or fail

UINT32 CFW_GprsActEX(
    UINT8 nState,
    UINT8 nCid,
    UINT16 nUTI,
    CFW_SIM_ID nSimID,
    BOOL SavedInApp,
    COS_CALLBACK_FUNC_T func);

//
// AT: ATA
// Manual acceptance of a network request for PDP context activation 'A'
// [in]nUTI:[1-7]
// Specify the UTI (Unique Transaction Identifier) operation,
// which support multi-application in parallel. This parameter will be
// return in notification event (see below). You also can get a free UTI by
// CFW_GetFreeUTI function if you application is a dynamical module.
// This parameter should be less than 255 for all upper layer application.
// [in]nCid: Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition.
// response event: EV_CFW_GPRS_CXT_ACTIVE_IND_NOTIFY
// parameter1: 0
// parameter2: 0
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,
UINT32 SRVAPI CFW_GprsManualAcc(
    UINT16 nUTI,
    UINT8 nCid,
    CFW_SIM_ID nSimID
);


//
// ATH
// Manual rejection of a network request for PDP context activation 'H'
// The function is used to reject the request of a network for GPRS PDP
// context activation which has been signaled to the TE by the RING or CRING
// unsolicited result code.
// [in]UTI:
// [in]nCid:
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,
// event: None
// parameter1: None
// parameter2: None
UINT32 SRVAPI CFW_GprsManualRej(
    UINT16 nUTI,
    UINT8 nCid,
    CFW_SIM_ID nSimID
);


// modify needed
// The function is used to modify the specified PDP context with QoS parameter.
// After this function has completed, the MT returns to V.25ter online data state.
// If the requested modification for any specified context cannot be achieved,
// an event with ERROR information is returned.
//nCid:
//[in]a numeric parameter which specifies a particular PDP context definition;
//
//nUTI:
//[in]
//
//QOS:
//[in] Point to CFW_GPRS_QOS structure to set the Quality of Service.
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,

UINT32 SRVAPI CFW_GprsCtxModify(
    UINT16 nUTI,
    UINT8 nCid,
    CFW_GPRS_QOS *Qos,
    CFW_SIM_ID nSimID
);

// no response
// This message is sent by MMI to accept the PDP context modification requested by the
// network in an api_PdpCtxModifyInd message.
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,
// event: NO
UINT32 SRVAPI CFW_GprsCtxModifyAcc(
    UINT16 nUTI,
    UINT8 nCid,
    CFW_SIM_ID nSimID
);

// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER, ERR_CME_UNKNOWN,
// event: NO
UINT32 SRVAPI CFW_GprsCtxModifyRej(
    UINT16 nUTI,
    UINT8 nCid,
    CFW_SIM_ID nSimID
);

//The function is used to send PDP data to the network. The implementation of
// this function is always called by upper protocol
// send data   //UINT8 nCid, UINT16 nDataLength, UINT8 *pData
// nCid:
//[in]a numeric parameter which specifies a particular PDP context definition;
//pGprsData: point to the data that need to send
// return: If the function succeeds, the return value is ERR_SUCCESS,
// if the function fails, the following error code may be returned.
// ERR_CFW_INVALID_PARAMETER,
// event: None
#ifdef LTE_NBIOT_SUPPORT
UINT32 SRVAPI CFW_GprsSendData(
    UINT8 nCid,
    CFW_GPRS_DATA *pGprsData,
    UINT8     nasRai,
    UINT16    type_of_user_data,
    CFW_SIM_ID nSimID
);

#else
UINT32 SRVAPI CFW_GprsSendData(
    UINT8 nCid,
    CFW_GPRS_DATA *pGprsData,
    CFW_SIM_ID nSimID
);
#endif
UINT32 CFW_PsSendData(UINT8 nCid, UINT32 pDataOffset, CFW_SIM_ID nSimID);

UINT32 CFW_GprsAtt(
    UINT8 nState,
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);
UINT32 CFW_GprsAttEX(UINT8 nState, UINT16 nUTI
                   , CFW_SIM_ID nSimID, COS_CALLBACK_FUNC_T func);

UINT32 CFW_GetGprsSum(INT32 *upsum, INT32 *downsum, CFW_SIM_ID nSimID );
UINT32 CFW_ClearGprsSum(CFW_SIM_ID nSimID );

#endif
//
//Tcpip
//
#ifndef __SOCKET__
typedef int SOCKET;
#define __SOCKET__
#endif
typedef enum e_resolv_result
{
    RESOLV_QUERY_INVALID,
    RESOLV_QUERY_QUEUED,
    RESOLV_COMPLETE
} RESOLV_RESULT;

#define RESOLV_NETWORK_ERROR      0x01
#define RESOLV_HOST_NO_FOUND      0x02

// Shell
//
UINT32 SRVAPI CFW_MeChangePassword (
    UINT16 nFac,
    UINT8 *pBufPwd,
    UINT8  nPwdSize,
    UINT8  nMode,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_MeGetFacilityLock (
    UINT16 nFac,
    UINT16 nUTI
);

UINT32 SRVAPI CFW_MeSetFacilityLock (
    UINT16 nFac,
    UINT8 *pBufPwd,
    UINT8  nPwdSize,
    UINT8  nMode,
    UINT16 nUTI
);

#ifdef CFW_GPRS_SUPPORT
//Errors related to a failure to perform an Attach
#define ERR_CFW_GPRS_ILLEGAL_MS                                 103 //Illegal MS (#3)
#define ERR_CFW_GPRS_ILLEGAL_ME                                 106 //Illegal ME (#6)
#define ERR_CFW_GPRS_GPRS_SERVICES_NOT_ALLOWED                  107 //GPRS services not allowed (#7)
#define ERR_CFW_GPRS_PLMN_NOT_ALLOWED                           111 //PLMN not allowed (#11)
#define ERR_CFW_GPRS_LOCATION_AREA_NOT_ALLOWED                  112 //Location area not allowed (#12)
#define ERR_CFW_GPRS_ROAMING_NOT_ALLOWED_IN_THIS_LOCATION_AREA  113 //Roaming not allowed in this location area (#13)


//Errors related to a failure to Activate a Context
#define ERR_CFW_GPRS_SERVICE_OPTION_NOT_SUPPORTED                   132 //service option not supported (#32)
#define ERR_CFW_GPRS_REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED        133 // requested service option not subscribed (#33)
#define ERR_CFW_GPRS_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER        134 // service option temporarily out of order (#34)
#define ERR_CFW_GPRS_PDP_AUTHENTICATION_FAILURE                     149 // PDP authentication failure
#define ERR_CFW_GPRS_UNSPECIFIED                                    148 // unspecified GPRS error
#endif
//
// Some functions need be supported by OS Kernerl(KL) based on the current target platform(MIPS).
//
typedef UINT32 (*PFN_KL_SENDMESSAGDE2APP)(UINT32 nEventId, VOID *pData, UINT32 nSize);
typedef UINT32 (*PFN_KL_SENDMESSAGDE_EX)(UINT32 nMailBox, UINT32 nEventId, VOID *pData, UINT32 nSize);
typedef PVOID (*PFN_KL_MALLOC)(UINT32 nSize);
typedef BOOL (*PFN_KL_FREE)(PVOID nAddr);
typedef BOOL (*PFN_KL_IS_VALIDHEAP)(VOID *pData, UINT32 nSize);
typedef BOOL (*PFN_AUD_START_SPK_MIC)(VOID);
typedef BOOL (*PFN_AUD_STOP_SPK_MIC)(VOID);

typedef struct _CFW_PFN_AUD_CBS
{
    PFN_AUD_START_SPK_MIC m_pfnAudStartSpkMic;
    PFN_AUD_STOP_SPK_MIC m_pfnAudStopSpkMic;
} CFW_PFN_AUD_CBS;

typedef struct _CFW_PFN_KL_CBS
{
    PFN_KL_SENDMESSAGDE2APP m_pfnKlSendMsg2App;
    PFN_KL_SENDMESSAGDE_EX m_pfnKlSendMsgEx;
    PFN_KL_MALLOC m_pfnKlMalloc;
    PFN_KL_FREE m_pfnKlFree;
    PFN_KL_IS_VALIDHEAP m_pfnKlIsValidHeap;
} CFW_PFN_KL_CBS;

//
//add by lixp
//
typedef struct _CFW_SAT_MENU_RSP_LIST
{
    UINT8 nComID;
    UINT8 nComQualifier;
    UINT16 nAlphaLen;
    UINT8 *pAlphaStr;
    CFW_SAT_ITEM_LIST *pItemList;  //Link list
} CFW_SAT_MENU_RSP_LIST, CFW_SAT_MENU_RSP;

#define CFW_PBK_HINT_MAX_SIZE       16

typedef struct _CFW_SMS_OUT_PARAM
{
    UINT8 nType;
    UINT8 padding[3];
    UINT32 nParam1;
    UINT32 nParam2;
} CFW_SMS_OUT_PARAM;
// CFW Init status phase.
#define CFW_INIT_STATUS_NO_SIM      (0x01)
#define CFW_INIT_STATUS_SIM         (0x02)
#define CFW_INIT_STATUS_SAT         (0x04)
#define CFW_INIT_STATUS_SIMCARD     (0x08)
#define CFW_INIT_STATUS_SMS         (0x10)
#define CFW_INIT_STATUS_PBK         (0x20)
#define CFW_INIT_STATUS_NET         (0x40)
#define CFW_INIT_STATUS_AUTODAIL    (0x80)
#define CFW_INIT_STATUS_NO_MEMORY   (0x81)
#define CFW_INIT_STATUS_SIM_DROP    (0x82)
#define CFW_INIT_STATUS_SIM_COMPLTED (0xF0)
//
// CFW Init status
//
#define CFW_INIT_SIM_CARD_BLOCK     0x4
#define CFW_INIT_SIM_WAIT_PIN1      0x8
#define CFW_INIT_SIM_WAIT_PUK1      0x10
#define CFW_INIT_SIM_PIN1_READY     0x20

#define CFW_INIT_SIM_WAIT_PS        0x40
#define CFW_INIT_SIM_WAIT_PF        0x80

#define CFW_INIT_SIM_CARD_BAD       0x100
#define CFW_INIT_SIM_CARD_ABNORMITY 0x200
#define CFW_INIT_SIM_TEST_CARD      0x400
#define CFW_INIT_SIM_NORMAL_CARD    0x800
#define CFW_INIT_SIM_CARD_CHANGED   0x1000
#define CFW_INIT_SIM_SAT            0x2000

//
// CFW Exit status
//
#define CFW_EXIT_STATUS_NETWORK_DETACHMENT           (0x01)
#define CFW_EXIT_STATUS_SIM_POWER_OFF                (0x02)
#define CFW_EXIT_STATUS_CFW_EXIT                     (0x10)

/************************************************ENGINERRING TEST MODE**************************************************/
//
//add  for emod at 20060819
//

typedef struct _CFW_EMOD_BATTERY_CHARGE_INFO
{
    UINT8 nBcs;                             //Point to the unsigned 8-bit variable  to retrieve the connection status of battery pack.
    //This parameter can be one of the following:
    //0 No charging adapter is connected
    //1 Charging adapter is connected
    //2 Charging adapter is connected, charging in progress
    //3 Charging adapter is connected, charging has finished
    //4 Charging error, charging is interrupted
    //5 False charging temperature, charging is interrupted while temperature is beyond allowed range
    UINT8 nBcl;                             //Point to the unsigned 8-bit variable to retrieve the battery level. This parameter can be
    //one of the following:
    //0 battery is exhausted, or does not have a battery connected
    // 20, 40, 60, 80, 100 percent of remaining capacity.
    //The percent is not accurate ,but a estimated expression.
    UINT16 nMpc;                                //Point to the unsigned 16-bit variable to retrieve the average power consumption.
    //This parameter can be one of the following value:
    //0-5000 Value (0...5000) of average power consumption (mean value over a couple of seconds) in mA.
} CFW_EMOD_BATTERY_CHARGE_INFO;

typedef struct _CFW_EMOD_BATTERY_INFO
{

    PM_BATTERY_INFO              nBatState;
    CFW_EMOD_BATTERY_CHARGE_INFO nChargeInfo;
    UINT8                        nChemistry;//
} CFW_EMOD_BATTERY_INFO;


BOOL CFW_EmodGetBatteryInfo(CFW_EMOD_BATTERY_INFO *pBatInfo);

void CFW_EmodAudioTestStart(UINT8 type);            //type value: 0 is normal; 1 is mic AUX and speak AUX; 2 is mic loud and speak loud.

void CFW_EmodAudioTestEnd(void);

BOOL CFW_EmodClearUserInfo(void);                   //return TRUE is success,and FALSE is failure.
void CFW_EmodGetIMEI(UINT8 *pImei, UINT8 *pImeiLen, CFW_SIM_ID nSimID);             //pImei is the buffer that
//
//bind
//

typedef struct _CFW_EMOD_BIND_SIM
{
    UINT8 nIndex;
    UINT8 nImsiLen;
    UINT8 pImsiNum[15];
    UINT8 pad[3];
} CFW_EMOD_BIND_SIM;

typedef struct _CFW_EMOD_BIND_NW
{
    UINT8 nIndex;
    UINT8 pNccNum[3];
} CFW_EMOD_BIND_NW;

typedef struct _CFW_EMOD_READ_BIND_SIM
{
    UINT8 nNumber;
    UINT32 nPointer;
    UINT8 pad[3];
} CFW_EMOD_READ_BIND_SIM;

typedef struct _CFW_EMOD_READ_BIND_NW
{
    UINT8 nNumber;
    UINT32 nPointer;
    UINT8 pad[3];
} CFW_EMOD_READ_BIND_NW;

void CFW_EmodReadBindSIM(CFW_EMOD_READ_BIND_SIM *pReadBindSim);

void CFW_EmodReadBindNW(CFW_EMOD_READ_BIND_NW *pReadBindNw);

void CFW_EmodGetBindInfo(BOOL *bSim, BOOL *bNetWork);   //ture: had been bond.false:not bind

UINT32 CFW_EmodSetBindInfo(BOOL bSim, BOOL bNetWork);

typedef struct _CFW_EMOD_UPDATE_RETURN
{
    UINT32 nErrCode;
    UINT8  nIndex;
} CFW_EMOD_UPDATE_RETURN;

CFW_EMOD_UPDATE_RETURN CFW_EmodUpdateSimBind(UINT8 nIndex, UINT8 *pBindSimNum, UINT8 nLen);

CFW_EMOD_UPDATE_RETURN CFW_EmodUpdateNwBind(UINT8 nIndex, UINT8 *pBindNwNum, UINT8 nLen);

//SN number
typedef struct _CFW_EMOD_LOCT_PRODUCT_INFO
{
    UINT8 nBoardSNLen;
    UINT8 nBoardSN[64];
    UINT8 nPhoneSNLen;
    UINT8 nPhoneSN[64];
} CFW_EMOD_LOCT_PRODUCT_INFO;

void CFW_EmodGetProductInfo(CFW_EMOD_LOCT_PRODUCT_INFO *pProduct);

typedef struct _CFW_EMOD_RF_CALIB_INFO
{

    UINT16 nLowDacLimit[6];
    UINT16 nLowVoltLimit[6];
    UINT32 nName;
    INT16   nPcl2dbmArfcn_d[30];
    INT16   nPcl2dbmArfcn_g[30];
    INT16   nPcl2dbmArfcn_p[30];
    INT16  nProfiledBmMax_dp;
    INT16  nProfiledBmMax_g;
    INT16  nProfiledBmMin_dp;
    INT16  nProfiledBmMin_g;
    //  UINT16 nProfile_dp[1024];
    //  UINT16 nProfile_g[1024];
    UINT16 nProfileInterp_dp[16];
    UINT16 nProfileInterp_g[16];
    UINT16 nRampHigh[32];
    UINT16 nRampLow[32];
    UINT16 nRampSwap_dp;
    UINT16 nRampSwap_g;
    INT32  nRaram[15];
    INT16  nTimes[15];
} CFW_EMOD_RF_CALIB_INFO;

void CFW_EmodGetRFCalibrationInfo(CFW_EMOD_RF_CALIB_INFO *pRfCalib);

//
//  test mode of outfield
//

#define CFW_TSM_CURRENT_CELL                0x01
#define CFW_TSM_NEIGHBOR_CELL               0x02


/*********************************************************************************************************/
typedef struct _CFW_TSM_CURR_CELL_INFO
{
    UINT16   nTSM_Arfcn;            //Absolute Radio Frequency Channel Number ,[0..1023].
    UINT8    nTSM_LAI[5];           //Location Area Identification.
    UINT8    nTSM_RAC;              //Routing Area Code ( GPRS only),  [0..255].
#ifdef LTE_NBIOT_SUPPORT
    UINT8    nTSM_CellID[4];        //Cell Identity.
#else
    UINT8    nTSM_CellID[2];        //Cell Identity.
#endif
    UINT8    nTSM_Bsic;             //Synchronisation code,[0..63].
    BOOL     nTSM_BCCHDec;          //Indicates if the BCCH info has been decoded on the cell.
    UINT8    nTSM_NetMode;          //Network Mode of Operation (GPRS only),[0..2].
    UINT8    nTSM_AvRxLevel;        //Average Rx level in Abs Val of dBm ,[0..115].
    UINT8    nTSM_MaxTxPWR;         //Max allowed transmit power, [0..31].
    INT8     nTSM_C1Value;          //Cell quality ,[-33..110].
    UINT8    nTSM_BER;              //Bit Error Rate [0..255],255 = non significant.
    UINT8    nTSM_TimeADV;          //Timing Advance (valid in GSM connected mode only).[0..255] 255 = non significant.
    UINT8    nTSM_CCCHconf ;        //ccch configuration
    //0:CCCH_1_CHANNEL_WITH_SDCCH meanning 1 basic physical channel used for CCCH, combined with SDCCH
    //1:CCCH_1_CHANNEL  meanning 1 basic physical channel used for CCCH, not combined with SDCCH
    //2:CCCH_2_CHANNEL  meanning 2 basic physical channel used for CCCH, not combined with SDCCH
    //3:CCCH_3_CHANNEL  meanning 3 basic physical channel used for CCCH, not combined with SDCCH
    //4:CCCH_4_CHANNEL  meanning 4 basic physical channel used for CCCH, not combined with SDCCH
    //0xFF:INVALID_CCCH_CHANNEL_NUM meanning Invalid value
    UINT8    nTSM_RxQualFull ;      //receive quality full  [0…7]
    UINT8    nTSM_RxQualSub;        //receive quality sub   [0…7]
    INT16    nTSM_C2;               //Value of the cell reselection criterium   [-96…173]
    UINT8    nTSM_CurrChanType;     //Current channel type
    //0:CHANNEL_TYPE_SDCCH meanning Current channel type is SDCCH
    //1:CHANNEL_TYPE_TCH_H0 meanning Current channel type is TCH ,half rate,Subchannel 0
    //2:CHANNEL_TYPE_TCH_H1 meanning Current channel type is TCH ,half rate,Subchannel 1
    //3:CHANNEL_TYPE_TCH_F meanningCurrent channel type is TCH ,full rate
    //0xff:INVALID_CHANNEL_TYPE meanning Invalid channel type
    UINT32   nTSM_CurrChanMode;     //current channel mode ,only valid in decicated mode
    //API_SIG_ONLY  1<<0    signalling only
    //API_SPEECH_V1 1<<1    speech full rate or half rate version 1
    //API_SPEECH_V2 1<<2    speech full rate or half rate version 2
    //API_SPEECH_V3 1<<3    speech full rate or half rate version 3
    //API_43_14_KBS 1<<4    data, 43.5kbit/s dwnlnk, 14.5kbit/s uplnk
    //API_29_14_KBS 1<<5    data, 29.0kbit/s dwnlnk, 14.5kbit/s uplnk
    //API_43_29_KBS 1<<6    data, 43.5kbit/s dwnlnk, 29.0kbit/s uplnk
    //API_14_43_KBS 1<<7    data, 14.5kbit/s dwnlnk, 43.5kbit/s uplnk
    //API_14_29_KBS 1<<8    data, 14.5kbit/s dwnlnk, 29.0kbit/s uplnk
    //API_29_43_KBS 1<<9    data, 29.0kbit/s dwnlnk, 43.5kbit/s uplnk
    //API_43_KBS    1<<10   data, 43.5kbit/s
    //API_32_KBS    1<<11   data, 32.0kbit/s
    //API_29_KBS    1<<12   data, 29.0kbit/s
    //API_14_KBS    1<<13   data, 14.5kbit/s
    //API_12_KBS    1<<14   data, 12.0kbit/s
    //API_6_KBS     1<<15   data, 6.0kbit/s
    //API_3_KBS     1<<16   data, 3.6kbit/s
    //API_INVALID_CHAN_MODE 0xFFFFFFFF  Invalid channel mode
    UINT8    nTSM_CurrBand;         //current band
    //9:BAND_GSM900 meanning Band GSM900P, GSM900E and GSM900R
    //18:BAND_DCS1800 meanning Band DCS1800
    //19:BAND_PCS1900 meanning Band PCS 1900
    //0xFF:BAND_INVALID meanning Invalid band value
    UINT8    nTSM_MaxRetrans;       //max number of random access retransmission    1,2,4,7
    UINT8    nTSM_BsAgBlkRes;       //block number kept for AGCH in common channel  [0…7]
    UINT8    nTSM_AC[2];            //Access Class  [0…255]
    UINT8    nTSM_RxLevAccMin;      //receive level access minimum  [0…63]
    BOOL     bTSM_EC;               //Indicates if emergency call has been allowed
    BOOL     bTSM_SI7_8;            //Indicates if SI7 and 8 are broadcast
    BOOL     bTSM_ATT;              //Indicates if IMSI attach has been allowed
    UINT8    pad[3];
} CFW_TSM_CURR_CELL_INFO;



typedef struct _CFW_TSM_NEIGHBOR_CELL_INFO
{
    UINT16 nTSM_Arfcn;          //Absolute Radio Frequency Channel Number ,[0..1023].
    UINT8 nTSM_Bsic;            //Synchronisation code,[0..63].
    BOOL  nTSM_BCCHDec;         //Indicates if the BCCH info has been decoded on the cell.
    UINT8 nTSM_AvRxLevel;       //Average Rx level in Abs Val of dBm ,[0..115].
    UINT8 nTSM_RxLevAM;         //RxLev Access Min in RxLev [0..63].
    UINT8 nTSM_MaxTxPWR;        //Max allowed transmit power, [0..31].
    INT8 nTSM_C1Value;          //Cell quality ,[-33..110].
    UINT8    nTSM_LAI[5];           //Location Area Identification.
    UINT8    nTSM_CellID[2];        //Cell Identity.
} CFW_TSM_NEIGHBOR_CELL_INFO;

typedef struct _CFW_TSM_ALL_NEBCELL_INFO
{
    UINT8                       nTSM_NebCellNUM;    //neighbor cell number.
    CFW_TSM_NEIGHBOR_CELL_INFO  nTSM_NebCell[6];    //the max of the neighbor cell is 6.
    UINT8                       pad[3];
} CFW_TSM_ALL_NEBCELL_INFO;



typedef struct _CFW_TSM_FUNCTION_SELECT
{
    BOOL    nServingCell;                           //true:enable the function ,false:disable.
    BOOL    nNeighborCell;                          //true:enable the function ,false:disable.
    UINT8   pad[2];
} CFW_TSM_FUNCTION_SELECT;

/***************************************API FUNCTION OF TSM***********************************************/
UINT32 CFW_EmodOutfieldTestStart(CFW_TSM_FUNCTION_SELECT *pSelecFUN, UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 SRVAPI CFW_EmodOutfieldTestEnd(UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_GetCellInfo(CFW_TSM_CURR_CELL_INFO *pCurrCellInfo, CFW_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo, CFW_SIM_ID nSimID);

UINT32 CFW_EmodSyncInfoTest(BOOL bStart, UINT16 nUTI, CFW_SIM_ID nSimID);

#define CFW_SYNC_INFO_IND   0x01
#define CFW_POWER_LIST_IND  0x02

#define CFW_EMOD_POWER_LIST_COUNT  50

typedef struct
{
    UINT16  nArfcn;
    UINT16  nRssi;
} CFW_EmodPowerInfo;

typedef struct
{
    CFW_EmodPowerInfo nPowerList[CFW_EMOD_POWER_LIST_COUNT];
    UINT8   nCount;
} CFW_EmodPowerListInd;


typedef enum
{
    PHY_VERSION,
    STACK_VERSION,
    PHY_CFG_VERSION,
    CSW_VERSION,
    MMI_VERSION
} SOFT_VERSION;

typedef enum
{
    XCV_VERSION,
    PA_VERSION,
    SW_VERSION,
    CHIP_VERSION,
    FLASH_VERSION
} HW_VERSION;

typedef struct _Soft_Version
{
    UINT8  nVersion[30];
    UINT32 nDate;
} Soft_Version;

typedef struct _HW_Version
{
    UINT8  nVersion[30];
} HW_Version;

#define ML_CP437        "CP437"
#define ML_CP737        "CP737"
#define ML_CP775        "CP775"
#define ML_CP850        "CP850"
#define ML_CP852        "CP852"
#define ML_CP855        "CP855"
#define ML_CP857        "CP857"
#define ML_CP860        "CP860"
#define ML_CP861        "CP861"
#define ML_CP862        "CP862"
#define ML_CP863        "CP863"
#define ML_CP864        "CP864"
#define ML_CP865        "CP865"
#define ML_CP869        "CP869"
#define ML_CP874        "CP874"
#define ML_CP932        "CP932"
#define ML_CP936        "CP936"
#define ML_CP950        "CP950"
#define ML_CP1250               "CP1250"
#define ML_CP1251       "CP1251"
#define ML_CP1255       "CP1255"
#define ML_CP1256       "CP1256"
#define ML_CP1258       "CP1258"
#define ML_ISO8859_1    "ISO8859_1"
#define ML_ISO8859_2    "ISO8859_2"
#define ML_ISO8859_3    "ISO8859_3"
#define ML_ISO8859_4    "ISO8859_4"
#define ML_ISO8859_5    "ISO8859_5"
#define ML_ISO8859_6    "ISO8859_6"
#define ML_ISO8859_7    "ISO8859_7"
#define ML_ISO8859_9    "ISO8859_9"
#define ML_ISO8859_13   "ISO8859_13"
#define ML_ISO8859_14   "ISO8859_14"
#define ML_ISO8859_15   "ISO8859_15"

extern UINT32 NV_GetNvSize(VOID);
extern INT32 NV_Read(UINT32 offs, UINT8 *buff, UINT32 to_read, UINT32 *pread);
extern INT32 NV_Write(UINT32 offs, UINT8 *buff, UINT32 to_write, UINT32 *pwitten);

UINT32 ML_Init();
UINT32 ML_SetCodePage(UINT8 nCharset[12]);

UINT32 ML_LocalLanguage2Unicode(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12]);
UINT32 ML_Unicode2LocalLanguage(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12]);
UINT32 ML_LocalLanguage2UnicodeBigEnding(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12]);



//hameina[+]20081031 for bug 10176
//
VOID  CFW_CcSetCSSI (  BOOL nCSSI);
VOID CFW_CcSetCSSU ( BOOL nCSSU );

UINT32 CFW_GetFreeCID(UINT8 *pCID, CFW_SIM_ID nSimID);
UINT32 CFW_ReleaseCID(UINT8 nCID, CFW_SIM_ID nSimID);
UINT32 CFW_SetCID(UINT8 nCID, CFW_SIM_ID nSimID);
BOOL CFW_isPdpActiveAvailable(CFW_SIM_ID nSimID);



//#ifdef CFW_EXTENDED_API
UINT32 cfw_SimReadElementaryFile(UINT16 nUTI, UINT32 fileId, CFW_SIM_ID nSimID);
UINT32 CFW_SimGetACMMax(UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_SimSetACMMax(UINT32 iACMMaxValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 Nuti, CFW_SIM_ID nSimID);
UINT32 CFW_SimGetACM(UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_SimSetACM(UINT32 iCurValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 Nuti, CFW_SIM_ID nSimID);
UINT32 CFW_SimReadRecord(UINT8 nFileID, UINT8 nRecordNum, UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_SimGetPUCT(UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_SimSetPUCT(CFW_SIM_PUCT_INFO  *pPUCT, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID);

//Add by wcf, 2015/1/21, process polling timer
#ifdef _SIM_HOT_PLUG_
VOID CFW_SetPollingTimer(UINT8 PollingInterval, CFW_SIM_ID nSimID);
VOID CFW_GetPollingTimer(UINT8 *pPollingInterval, CFW_SIM_ID nSimID);
#endif
//End, process polling timer

VOID CSW_FacilityCrssDeflec(
    UINT8 nIndex,
    CFW_SIM_ID  nSimId,
    UINT8 DTN[20],   //Deflec number
    UINT8 DTNLen,
    UINT8 DTSA[21],  //sub number
    UINT8 DTSALen
);
extern UINT32 CFW_SimInit(
    BOOL bRstSim,
    CFW_SIM_ID nSimID
);

extern UINT32 CFW_SimReset(
    UINT16 nUTI,
    CFW_SIM_ID nSimID
);
extern UINT32 CFW_SimReadMessage(UINT16 nLocation, UINT32 nIndex, UINT16 nUTI, CFW_SIM_ID nSimID);
extern void ADP_CB_Init(void * buf, UINT32 lenth, CFW_SIM_ID nSimID);
extern VOID CFW_CbGetSimContext(CFW_SIM_ID nSimID);
extern UINT32 CFW_CbAct( UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_CbDeAct(UINT16 nUTI, CFW_SIM_ID nSimID);
#ifdef CB_SUPPORT
extern UINT32 CFW_CbGetContext(VOID *CtxData , CFW_SIM_ID nSimID);
extern UINT32 CFW_CbSetContext(VOID *CtxData , CFW_SIM_ID nSimID);
#endif
extern UINT32 CFW_SmsMoveMessage( UINT16 nIndex, UINT8 nStorage, UINT16 nUTI, CFW_SIM_ID nSimId);
extern UINT32 CFW_SimSetMidList(UINT16 *pMidList, UINT8 nCnt, UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_SimWriteMessage(UINT16 nLocation, UINT32 nIndex, UINT8 *pData, UINT8 nDataSize, UINT16 nUTI, CFW_SIM_ID nSimID);
#define ICCID_LENGTH    10
UINT8* CFW_GetICCID(CFW_SIM_ID nSimID);

#ifdef LTE_NBIOT_SUPPORT
UINT8 CFW_GprsGetPsType(UINT8 nSimID);
UINT32 CFW_GprsSecPdpAct(UINT8 nState, UINT8 nCid, UINT8 nPCid, UINT16 nUTI, CFW_SIM_ID nSimID);
UINT32 CFW_GprsSetCtxTFT(UINT8 nCid, CFW_TFT_SET* pTft, UINT8 nSimID );
UINT32 CFW_GprsSetCtxEQos(UINT8 nCid, CFW_EQOS* pQos,UINT8 nSimID );
UINT32 CFW_GprsGetCtxEQos(UINT8 nCid, CFW_EQOS *pQos, CFW_SIM_ID nSimID);
UINT32 CFW_GprsCtxEpsModify(UINT16 nUTI, UINT8 nCid, CFW_SIM_ID nSimID);
#endif //LTE_NBIOT_SUPPORT

#endif // _H


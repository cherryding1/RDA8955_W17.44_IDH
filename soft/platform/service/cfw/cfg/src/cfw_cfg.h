#ifndef _cfw_cfg_h_
#define _cfw_cfg_h_
//#include "api_msg.h"
#include "cfw.h"
#include "cfw_prv.h"
#include "cfw_nv.h"
#include "cfw_cfgRange.h"
//#include "reg_local.h"

#define CFW_IND_BC          0x1
#define CFW_IND_SQ          0x2
#define CFW_IND_NW          0x4
#define CFW_IND_SMS_RECEIVE 0x8
#define CFW_IND_CALL        0x10
#define CFW_IND_ROAM        0x20
#define CFW_IND_SMS_FULL    0x40

#define SET_BIT_VALUE(param32, nBit, v)                            \
                if(v)                                              \
                   param32 = param32 | (1 << nBit);                \
                else                                               \
                   param32 = param32 ^ (param32 & (1 << nBit))

#define GET_BIT_VALUE(param32, nBit, v)               \
                    if(param32 & (1 << nBit))         \
                        v = 1;                        \
                    else                              \
                        v = 0;
/************************************************************************/
/* Special for RTC issue                                                */
/************************************************************************/
#define BASE_YARE                       1980

#define SEC_IN_MINUTE                   60
#define MIN_IN_HOUR                     60
#define HOUR_IN_DAY                     60
#define MON_IN_YEAR                     12

#define SEC_IN_HOUR                     3600
#define SEC_IN_DAY                      86400


typedef struct _LIMIT
{
    UINT32 min;
    UINT32 MAX;
} LIMIT;

typedef struct
{
    UINT32    sec;      /* [0,59]     */
    UINT32    min;      /* [0,59]     */
    UINT32    hour;     /* [0,23]     */
    UINT32    day;      /* [1,31]     */
    UINT32    month;    /* [1,12]     */
    UINT32    year;     /* 1980--2050 */
} TIME_DATE;


#define _ISLEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define isleap(y)   (_ISLEAP(y) ? 1 : 0)

#define DAYS_IN_MONTH(x, month, y)                             \
    {                                                           \
        switch(month)                                           \
        {                                                       \
        case 0: case 2: case 4: case 6: case 7: case 9: case 11:\
            y = 31;                                             \
            break;                                              \
        case 1:                                                 \
            y = (x ? 29 : 28);                                  \
            break;                                              \
        case 3:  case 5: case 8: case 10:                       \
            y = 30;                                             \
            break;                                              \
        }                                                       \
    }


#define PAST_DAYS_IN_THIS_YEAR(x, month, y)     \
{                                               \
    UINT8 i = 0; UINT8 md = 0;                  \
    for(i = 0; i < month; i++){                 \
        DAYS_IN_MONTH(x, i, md);               \
        y += md; }                              \
}

#define DAYS_IN_YEAR(x) (365 + x)

#define FLASH_INFO 0
/************************************************************************/
/* Information storage (They may be stored in FLASH.)                   */
/************************************************************************/

typedef struct _MEM_INFO_STORAGE
{
    /*CFW_CfgSetEchoMode*/
    UINT8 EchoMode;

    /*CFW_CfgSetResultCodePresentMode*/
    UINT8 ResultCodePresentMode;

    /*CFW_CfgSetResultCodeFormatMode*/
    UINT8 ResultCodeFormatMode;

    /*CFW_CfgSetConnectResultFormatCode*/
    UINT8 ConnectResultFormatCode;

    /*CFW_CfgSetBearServType*/
    UINT8 BearServType;

    /*CFW_CfgSetRLPType*/
    UINT8 RLPType_iws;
    UINT8 RLPType_mws;
    UINT8 RLPType_T1;
    UINT8 RLPType_N2;

    /*CFW_CfgSetIncomingCallResultMode*/
    UINT8 IncomingCallResultMode;

    /*CFW_CfgSetServiceReportingMode*/
    UINT8 ServiceReportingMode;

    /*CFW_CfgSetNewSmsIndication */
    UINT8 NewSmsIndication;

    /*CFW_CfgSetErrorReport*/
    UINT8 ErrorReport;

    /*CFW_CfgSelectSmsService*/
    UINT8 SelectSmsService;
    /*CFW_CfgSetNwStatus*/
    UINT8 NwStatus0;
    UINT8 NwStatus1;
    UINT8 NwStatus2;
    UINT8 NwStatus3;
    /*CFW_CfgSetClip */
    UINT8 Clip_Value0;
    UINT8 Clip_Value1;
    UINT8 Clip_Value2;
    UINT8 Clip_Value3;


    /*CFW_CfgSetPhoneFunctionality */
    UINT8 CFUN_Level;
    UINT8 CFUN_Result;

    /*CFW_CfgSetCharacterSet */
    UINT8 CharacterSet;

    /*CFW_CfgSetEventReport*/
    UINT16  CMER_parameter;

    UINT16  CNMI_parameter;


    UINT8 pading[3];

} MEM_INFO_STORAGE;

#define PRODUCTION_IDENTIFICATION "CIITec XXX Version: 1.0.0.0(Just for demo)"



/************************************************************************/
/* Operator information                                                 */
/************************************************************************/
typedef struct _OPER_INFO
{
    UINT8 OperatorId[6];
    UINT8 *OperatorName;
} OPER_INFO;

typedef struct _SPECIAL_OPER_INFO
{
    UINT8 *CompanyName; //It is operator company name.
    UINT8 OperatorId[6];
    UINT8 *OperatorName;//The means is brand,not operator company
} SPECIAL_OPER_INFO;

extern MEM_INFO_STORAGE MemoryInfo;

extern  UINT32 gLastErrorNo;
extern  const OPER_INFO OperatorInfo[];
extern  const SPECIAL_OPER_INFO SpecialOperatorInfo[];
#define CEER_INFO_TABlE_SIZE    5

typedef struct _CFW_CFG_CALLFORWARDING
{
    CFW_TELNUMBER number;
    UINT8 reason;
    UINT8 mode;
    UINT8 class;
    UINT8 time;
    UINT8 status;
    UINT8 padding[3];
} CFW_CFG_CALLFORWARDING;
//[[hameina [+] 2006.8.24 new cfg functions
INT32 CFW_Cfg_strcmp(const UINT8 *s1, const UINT8 *s2);
UINT32 CFW_CfgNumRangeCheck(UINT32 nInput, UINT8 *pString);
UINT32 CFW_CfgStrRangeCheck(UINT8 *pInputStr, UINT8 *pString);
extern CFW_IND_EVENT_INFO   IndEventInfo[CFW_SIM_COUNT];

#ifdef CFW_GPRS_SUPPORT



UINT32 CFW_CfgGprsSetPdpCxt(UINT8 nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID);
UINT32 CFW_CfgGprsGetPdpCxt(UINT8 nCid, CFW_GPRS_PDPCONT_INFO *pPdpCont, CFW_SIM_ID nSimID);
UINT32 CFW_CfgGprsSetReqQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
UINT32 CFW_CfgGprsGetReqQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
UINT32 CFW_CfgGprsSetMinQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);
UINT32 CFW_CfgGprsGetMInQos(UINT8 nCid, CFW_GPRS_QOS *pQos, CFW_SIM_ID nSimID);


#endif
UINT32 CFW_CfgGetEmodParam(EMOD_PARAMETERS             *pEmodParam);
UINT32 CFW_CfgSetEmodParam(EMOD_PARAMETERS    *pEmodParam);
UINT32 CFW_Cfg_test();
UINT32 CFW_Cfg_test2();
UINT32 cfg_read_8(UINT8 *uSubkey, UINT8 *name, UINT8 *n);
UINT32 cfg_write_8(UINT8 *uSubkey, UINT8 *name, UINT8 n);
UINT32 cfg_write_str(UINT8 *uSubkey, UINT8 *name, UINT8 *data, UINT8 size);
UINT32 cfg_read_str(UINT8 *uSubkey, UINT8 *name, UINT8 *uData, UINT8 *uSize);
UINT32 cfg_read_32(UINT8 *uSubkey, UINT8 *name, UINT32 *n);
UINT32 cfg_write_32(UINT8 *uSubkey, UINT8 *name, UINT32 n);
UINT32 cfg_read_hex(UINT8 *uSubkey, UINT8 *name, PVOID udata, UINT8 usize);
UINT32 cfg_write_hex(UINT8 *uSubkey, UINT8 *name, PVOID data, UINT8 usize);
UINT32 cfg_readdefault(UINT8 *uSubkey, UINT8 *name, void *data);
UINT32 CFW_CfgSimParamsInit();
#ifdef CHIP_HAS_AP
INT32 cfg_set_ind_to_at(UINT8 ind);
VOID cfg_update_notify(UINT32 offset, UINT32 len);
#endif

#define ERR_OPEN_KEY_FAILED        1001
#define ERR_GET_VALUE_FAILED      1002
#define ERR_SET_VALUE_FAILED      1003
#define ERR_CLOSE_KEY_FAILED      1004

#endif //_cfw_cfg_h_
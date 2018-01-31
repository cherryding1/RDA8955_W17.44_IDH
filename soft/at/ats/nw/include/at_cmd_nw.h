/*********************************************************
 *
 * File Name
 *  at_cmd_nw.h
 * Author
 *  Felix
 * Date
 *  2007/11/25
 * Descriptions:
 *  ...
 *
 *********************************************************/

#ifndef __AT_CMD_NW_H__
#define __AT_CMD_NW_H__

#define COPN_INFO_LEN 0x40  // 64
#define COPN_COUNT    0xFF  // 33

// #define NW_PREFERRED_OPERATOR_FORMAT_NUMERIC    2
#define NW_PREFERRED_OPERATOR_FORMAT_ALPHANUMERIC_LONG   0

// #define NW_GETPREFERREDOPERATORS_CALLER_READ 0
#define NW_GETPREFERREDOPERATORS_CALLER_SET 1

struct COS_EVENT;

struct AT_CMD_PARA;

typedef struct _AT_POSI
{
    UINT8 sMcc[3];
    UINT8 sMnc[3];
    UINT16 sLac;
    UINT16 sCellID;
    UINT8 iBsic;
    UINT8 iRxLev;
    UINT8 iEndED;
} AT_POSI;

typedef struct _AT_POSI_LIST
{

    UINT8 iNum;
    UINT8 iMode;
    UINT8 sRandNum[40];
    UINT8 sNdssign[1];
    AT_POSI PosiList[14]; // 2 * curr cell info + 2* 6 neigh cell info
} AT_POSI_LIST;

typedef struct _AT_CS_STAUTS
{
    UINT32 nParam1;
    UINT8 nType;
} AT_CS_STAUTS;

typedef struct _AT_arresposi
{
    UINT8 ndsposi[32];
} AT_arresposi;

typedef struct _AT_ARRES
{
    AT_arresposi arres[14]; //
} AT_ARRES;

typedef struct AT_CCED
{
    UINT8 sMcc[3];
    UINT8 sMnc[3];
    UINT16 sLac;
    UINT16 sCellID;
    UINT8 iBsic;
    UINT8 iRxLev;
    UINT8 iRxLevSub;
    UINT8 iEndED;
    UINT16 nArfcn;
} AT_CCED;
typedef struct
{
    u32      result;
    u32      para[4];
} AT_XCPUTEST_RSP;
typedef struct
{
    u32      result;
    u32      para[4];
} AT_WCPUTEST_RSP;

UINT32 AT_SetCmeErrorCode(UINT32 nCfwErrorCode, BOOL bSetParamValid);

VOID AT_NW_AsyncEventProcess(COS_EVENT *pEvent);

VOID AT_NW_CmdFunc_COPS(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_XCPUTEST(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_WCPUTEST(AT_CMD_PARA *pParam);

VOID AT_NW_CmdFunc_CSQ(AT_CMD_PARA *pParam);

VOID AT_NW_CmdFunc_COPN(AT_CMD_PARA *pParam);

VOID AT_NW_CmdFunc_CREG(AT_CMD_PARA *pParam);

VOID AT_NW_CmdFunc_CBindArfcn(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_SETBND(AT_CMD_PARA *pParam);

BOOL AT_CIND_NetWork(UINT8 *pSignalQ, UINT8 *pRegistS, UINT8 *pRoamS, UINT8 nDLCI, UINT8 nSim);
#ifdef __MULTI_RAT__
VOID AT_NW_CmdFunc_SNWR(AT_CMD_PARA *pParam);
#endif
VOID AT_NW_CmdFunc_CTEC(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_CPOL(AT_CMD_PARA *pParam);
UINT16 SUL_AsciiToGsmBcdExForNw(
    INT8 *pNumber,  // input
    UINT8 nNumberLen,
    UINT8 *pBCD  // output should >= nNumberLen/2+1
);


// frank add start
#ifdef AT_USER_DBS
VOID AT_NW_CmdFunc_POSI(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_CGSN(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_GSMSTATUS(AT_CMD_PARA *pParam);
UINT32 CFW_GetDBSSeed(INT8 *seed);
UINT16 AT_DBSAsciiToGsmBcd(INT8 *pNumber, UINT8 nNumberLen, UINT8 *pBCD);
extern UINT8 g_Gidver[];  // dbsupdate
#else
VOID AT_NW_CmdFunc_CCED(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_LOCKBCCH(AT_CMD_PARA *pParam);

#endif

UINT32 CFW_SimClose(UINT16 nUTI
                    , CFW_SIM_ID nSimID
                   );
UINT32 CFW_SetCommSIMOpen(
#ifdef CFW_SIM_ID
    CFW_SIM_ID nSimID
#endif
);

BOOL CFW_NwGetLockBCCH();
UINT32 CFW_NwSetLockBCCH(BOOL bLock,UINT16 nUTI,CFW_SIM_ID nSimID );
UINT32 _ClearCommandPatch4Cops(CFW_SIM_ID nSimID);
VOID AT_ReportRssi(UINT8 nDLCI);

VOID AT_NW_CmdFunc_ECSQ(AT_CMD_PARA *pParam);

#ifdef LTE_NBIOT_SUPPORT
VOID AT_GPRS_CmdFunc_CSCON(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CEDRXRDP(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CCIOTOPT(AT_CMD_PARA *pParam);
UINT8 AT_Cscon_respond(UINT8 *respond, UINT8 mode, BOOL reportN);
#endif
VOID AT_NW_CmdFunc_QSCANF(AT_CMD_PARA *pParam);
VOID AT_NW_CmdFunc_SDMBS(AT_CMD_PARA *pParam);
#endif
// frank add end



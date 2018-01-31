// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2012, Coolsand Technologies, Inc.
// All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmission of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: at_cmd_gprs.h
//
// DESCRIPTION:
// TODO: ...
//
// REVISION HISTORY:
// NAME              DATE                REMAKS
// Lixp      2012-2-20       Created initial version 1.0
//
// //////////////////////////////////////////////////////////////////////////////
#ifdef CFW_GPRS_SUPPORT
#ifndef __AT_CMD_GPRS_H__
#define __AT_CMD_GPRS_H__

struct COS_EVENT;

struct AT_CMD_PARA;

// GPRS parameter size define.
#define AT_GPRS_APN_MAX_LEN      50
#define AT_GPRS_PDPADDR_MAX_LEN  50
#define AT_GPRS_PDPTYPE_MAX_LEN  50
#define AT_GPRS_USR_MAX_LEN          50
#define AT_GPRS_PAS_MAX_LEN           50

#define AT_PDPCID_MIN 1
#define AT_PDPCID_MAX 7
//#define AT_MAXALLOWEDACTPDP 3
#define AT_MAXALLOWEDACTPDP 8

#define AT_PDPCID_ERROR 0xFF
#define AT_PDPCID_NDEF 0
#define AT_PDP_ACTED    1
#define AT_PDP_DEACTED   0

// [NEW] by yy at 2008-04-21 begin
#define AT_GPRS_DEA    CFW_GPRS_DEACTIVED
#define AT_GPRS_ACT    CFW_GPRS_ACTIVED

#define AT_GPRS_ATT    1
#define AT_GPRS_DAT    0

#define AT_GPRS_ACTIVEDELAY     45
#define AT_GPRS_ATTDELAY     45 // ad by wulc

#define AT_GPRSUTI_BEG        200
#define AT_GPRSUTI_ATT_OFF 0
#define AT_GPRSUTI_ACT_OFF 5
#define AT_GPRSUTI_DEA_OFF 15
#define AT_GPRSUTI_TCPIP_OFF 25
// [NEW] by yy at 2008-04-21 end

#define AT_GPRS_ATT_NOTREG       0
#define AT_GPRS_ATT_HOME_OK     1
#define AT_GPRS_ATT_TRYING        2
#define AT_GPRS_ATT_DNY              3
#define AT_GPRS_ATT_UNKNOWN    4
#define AT_GPRS_ATT_ROAM_OK     5

#define CLASS_TYPE_B 0
#define CLASS_TYPE_CC 1

#define AT_TCPIPACTPHASE_GPRS           0

#ifdef LTE_NBIOT_SUPPORT
#define AT_GPRS_NONIP_DATA_MAX_LEN  1600
#define AT_GPRS_CP_DATA_MAX_LEN  1600

#define NV_CHANGED_PSM      1
#define NV_CHANGED_EDRX     2
#endif


extern UINT8 g_uAtWaitActRsp[MAX_DLC_NUM];
extern UINT8 g_uAttState;
extern UINT8 g_uAtGprsCidNum;


typedef struct _AT_Gprs_CidInfo
{
    UINT8 uCid;
#ifdef LTE_NBIOT_SUPPORT
    UINT8 uPCid; /*  */
#endif
    UINT8 nDLCI;
    UINT8 uStateStr[15];
    UINT8 uState;

    // UINT8   uAutoRspState;
    UINT8 nPdpType;
    UINT8 nDComp;
    UINT8 nHComp;
    UINT8 pApn[AT_GPRS_APN_MAX_LEN];
    UINT8 pPdpAddr[AT_GPRS_PDPADDR_MAX_LEN];
    UINT8 nApnSize;
    UINT8 nPdpAddrSize;

    // [[ yy [add]  for TCPIP 2008-5-19
    UINT8 uaUsername[AT_GPRS_USR_MAX_LEN];
    UINT8 nUsernameSize;
    UINT8 uaPassword[AT_GPRS_PAS_MAX_LEN];
    UINT8 nPasswordSize;

    // ]] yy [add] for TCPIP 2008-5-19
#ifdef LTE_NBIOT_SUPPORT
    UINT8 apnacAer;
    UINT8 apnacUnit;
    UINT32 apnacRate;
#endif

} AT_Gprs_CidInfo;

extern AT_Gprs_CidInfo g_staAtGprsCidInfo_e[4][AT_PDPCID_MAX + 1];

VOID AT_GPRS_Result_OK(UINT32 uReturnValue,
                       UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize, UINT8 nDLCI, UINT8 nSim);
VOID AT_GPRS_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
UINT32 CFW_AttDetach (UINT8 nState, UINT16 nUTI, UINT8 AttDetachType, CFW_SIM_ID nSimID, COS_CALLBACK_FUNC_T func);
VOID AT_GPRS_CmdFunc_CGDCONT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGDSCONT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGTFT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGQREQ(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGEQREQ(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGEQMIN(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGQMIN(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGATT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGACT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGPADDR(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGDATA(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGAUTO(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGANS(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGCLASS(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGEREP(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGREG(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGSMS(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CRC(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGCID(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGSEND(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGPDNSADDR(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGTFT(AT_CMD_PARA *pParam);
VOID AT_GPRS_Att_Rsp(CFW_EVENT *pEvent);
VOID AT_GPRS_Act_Rsp(CFW_EVENT *pCfwEv);
#ifdef LTE_NBIOT_SUPPORT
INT32 AT_GPRS_IPv4v6AddrAnalyzer(UINT8 *pPdpAddr, UINT8 uSize, UINT8 *iptype);
#endif
INT32 AT_GPRS_IPAddrAnalyzer(const UINT8 *pPdpAddr, UINT8 uSize);

INT32 AT_GPRS_IPStringToPDPAddr(UINT8 *pPdpAddrStr, UINT8 uSize, UINT8 *pPdpAddr, UINT8 *pAddrSize);

UINT8 AT_Get_ClassType();
VOID AT_Set_ClassType(UINT8 Class);
#ifdef AT_HTTP_SUPPORT
VOID AT_GPRS_CmdFunc_HTTPGET(AT_CMD_PARA *pParam);
#endif
void AT_GprsStatusInd(COS_EVENT *pEvent);

VOID AT_CLearGprsCtx(UINT8 nDLCI);
UINT8 DlciGetCid(UINT8 nDLCI);

#ifdef LTE_NBIOT_SUPPORT
VOID AT_GPRS_CmdFunc_CGCMOD(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CSODCP(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CRTDCP(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_VERSIONCONTROL(AT_CMD_PARA *pParam);
VOID AT_TCPIP_CmdFunc_NIPDATA(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CFGCIOT(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CFGDFTPDN(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CEDRXS(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGAPNRC(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CFGEDRX(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CPSMS(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CFGHCCP(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_NASCFG(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CRCES(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_VER(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGEQOS(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CGEQOSRDP(AT_CMD_PARA *pParam);
VOID AT_GPRS_CmdFunc_CEREG(AT_CMD_PARA *pParam);
VOID AT_TCPIP_CmdFunc_DIRECTIP(AT_CMD_PARA *pParam);
UINT8 AT_Cereg_respond(UINT8 *respond, CFW_NW_STATUS_INFO *sStatus, UINT8 stat,  BOOL reportN);
void CrtdcpDataHandler( UINT8 cid, CFW_GPRS_DATA *pGprsData, UINT8 nSimId);
void NonIpDataHandler( UINT8 cid, CFW_GPRS_DATA *pGprsData, UINT8 nSimId);

UINT8  AT_Gprs_Mapping_Act_From_PsType(UINT8 pstype);

#endif


#endif
#endif

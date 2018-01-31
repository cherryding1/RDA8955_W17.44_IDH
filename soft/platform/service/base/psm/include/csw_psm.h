#ifndef CSW_PSM_H
#define CSW_PSM_H
#ifdef LTE_NBIOT_SUPPORT
#include "cs_types.h"
#include "at_sa.h"
#include "at_module.h"
#include "at_cmd_gprs.h"
#include "at_cmd_tcpip.h"
#include "cfw_gprs_data.h"
#include "cfw_nw_data.h"
#include "cfw.h"

#define CSW_RESTORE_STATIC_CTX       0
#define CSW_RESTORE_DYNAMIC_CTX    1

#ifdef NBIOT_APPLICATION_PSM
#define UDP_SOCKET_STATE_NULL 0
#define UDP_SOCKET_STATE_NEW  1
#define UDP_SOCKET_STATE_BIND  2
#define UDP_SOCKET_STATE_CONNECT  3
#define UDP_SOCKET_STATE_ERROR  4
typedef struct AT_Gprs_Psm_SocketInfo_Tag
{
    UINT8   tcpipState;
    UINT8   uSocket;
    UINT8   socketState;
    UINT16   localPort;
    //UINT8   localIPAddr[AT_TCPIP_ADDRLEN + 3];
    UINT32 localIpAddr;
    UINT16   remotePort;
    UINT8   remoteIPAddrStr[AT_TCPIP_ADDRLEN + 3];
    UINT32 remoteIpAddr;
}AT_Gprs_Psm_SocketInfo;

typedef struct T_Aps_Tcpip_Static_Context_Backup_Info_Tag
{
   UINT8 nCid[2];
   BOOL  isUdpSocket;
   //at_tcpip
    AT_Gprs_Psm_SocketInfo   socketInfo[2];
}
T_Aps_Tcpip_Static_Context_Backup_Info;


typedef struct T_Aps_Ppp_Static_Context_Backup_Info_Tag
{
   UINT8 ncpHeader_extend;
   UINT8 IPHeader_extend;
   UINT8 mux_id;
   UINT8 Cid;
   UINT8 Id;  //nessasry?
   UINT32 MagicNumber;
}
T_Aps_Ppp_Static_Context_Backup_Info;

typedef struct T_Aps_Static_Context_Backup_Info_Tag
{
   UINT8 aps_app;
   union
   {
       T_Aps_Tcpip_Static_Context_Backup_Info apsTcpipStaticCtxBackupInfo; 
	T_Aps_Ppp_Static_Context_Backup_Info apsPppStaticCtxBackupInfo;
	// non ip reserved;
   }apsStaticInfo;
}
T_Aps_Static_Context_Backup_Info;
/*
typedef struct T_Aps_Dynamic_Context_Backup_Info_Tag
{

   union
   {
       T_Aps_Tcpip_Dynamic_Context_Backup_Info apsTcpipDynamicCtxBackupInfo; 
	T_Aps_Ppp_Dynamic_Context_Backup_Info apsPppDynamicCtxBackupInfo;
	// non ip reserved;
   }apsDynamicInfo;
}
T_Aps_Dynamic_Context_Backup_Info;
*/
#endif //NBIOT_APPLICATION_PSM

typedef struct AT_Gprs_Psm_CidInfo_Tag
{
    UINT8    nCid;
    UINT8    nDLCI;
    UINT8    uState;
}AT_Gprs_Psm_CidInfo;

// FIXME: a dummy definition to make compile pass
typedef struct _sa_info_t
{
} sa_info_t;
typedef struct _AT_MuxChannel_Socket
{
} AT_MuxChannel_Socket;
// FIXME !end

typedef struct T_AT_Static_Context_Backup_Info_Tag
{
//at_sa&at_mux
   UINT8     dlciBitMap;

//at_gc
    UINT8 gc_pwroff;
    UINT8 cfg_cfun;
//at_nw
    UINT8  nwEcsqFlag;
    UINT8  copsMode;
    UINT8  firstReg;
    UINT8  copsSetFlag;
    UINT8  cereg;
    UINT8  preferOperator[6];
//at_gprs
    UINT8 crtdcp_reporting;
    //UINT8 gprsEnterDataModeMux[MAX_DLC_NUM];
    AT_Gprs_Psm_CidInfo  staAtGprsCidInfo[2];// = g_staAtGprsCidInfo_e[nSim];
}
T_AT_Static_Context_Backup_Info;
/*
typedef struct T_AT_Dynamic_Context_Backup_Info_Tag
{
   u8 a;
}
T_AT_Dynamic_Context_Backup_Info;
*/


typedef struct T_Cfw_Aom_Static_Context_Backup_Info_Tag
{
    UINT32   UTI_Status[8];
}T_Cfw_Aom_Static_Context_Backup_Info;
/*
typedef struct T_Cfw_Aom_Dynamic_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Aom_Dynamic_Context_Backup_Info;

typedef struct T_Cfw_Sim_Static_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Sim_Static_Context_Backup_Info;

typedef struct T_Cfw_Sim_Dynamic_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Sim_Dynamic_Context_Backup_Info;

typedef struct T_Cfw_Nw_Static_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Nw_Static_Context_Backup_Info;
*/
typedef struct T_Cfw_Nw_Dynamic_Context_Backup_Info_Tag
{
    BOOL   saveStaticAO;
    UINT8   UTI;
    UINT8   AO_State;
    NW_SM_INFO   NwSmInfo;
}T_Cfw_Nw_Dynamic_Context_Backup_Info;

typedef struct CfwStaticPdpContList_Tag
{
    UINT8  nCid;
    UINT8  nHComp_nDComp;
	/*size could be caculated*/
    //UINT8  nApnSize; 
   // UINT8  nPdpAddrSize;
    //UINT8  nApnUserSize;
    //UINT8  nApnPwdSize;
    UINT8  apnName[THE_APN_MAX_LEN];
	//UINT8  apnName[50];
   // UINT8  apnUser[THE_APN_USER_MAX_LEN];
    //UINT8  apnPwd[THE_APN_PWD_MAX_LEN];
    UINT8  pdpAddr[THE_PDP_ADDR_MAX_LEN];
  // UINT8  pdpAddr[18];
    UINT8  nSlpi_PdnType; // IPV4/V6/Non-IP
    CFW_EQOS                   eQos;
}CfwStaticPdpContList;

typedef struct T_Cfw_Gprs_Static_Context_Backup_Info_Tag
{
    UINT8                    storeCid[8];
	UINT8                    GprsCidSavedInApp;
    CfwStaticPdpContList     cfwStaticPdpContList[2];
}T_Cfw_Gprs_Static_Context_Backup_Info;

typedef struct USim_Aid_Tag
{
    UINT8 data[16];
    UINT8 length;
}USim_Aid;
typedef struct T_Cfw_Usim_Static_Context_Backup_Info_Tag
{
    UINT8 iccid[ICCID_LENGTH];
    USim_Aid   usimAid;
}T_Cfw_Usim_Static_Context_Backup_Info;
typedef struct T_Cfw_Gprs_Dynamic_Context_Backup_Info_Tag
{
    //u8 a; //reserved
    //AO_STATE   I think it must be activated state, otherwise cannot enter psm state
    UINT8   UTI;
    UINT8   nSmIndex;
	UINT8   GprsCidSavedInApp;
    BOOL    activated;
}T_Cfw_Gprs_Dynamic_Context_Backup_Info;
/*
typedef struct T_Cfw_Sms_Static_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Sms_Static_Context_Backup_Info;

typedef struct T_Cfw_Sms_Dynamic_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Sms_Dynamic_Context_Backup_Info;

typedef struct T_Cfw_Pbk_Static_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Pbk_Static_Context_Backup_Info;

typedef struct T_Cfw_Pbk_Dynamic_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Pbk_Dynamic_Context_Backup_Info;

typedef struct T_Cfw_Cc_Static_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Cc_Static_Context_Backup_Info;

typedef struct T_Cfw_Cc_Dynamic_Context_Backup_Info_Tag
{
 u8 a; //reserved
}T_Cfw_Cc_Dynamic_Context_Backup_Info;
*/
typedef struct T_Cfw_Static_Context_Backup_Info_Tag
{
    //  aom
    T_Cfw_Aom_Static_Context_Backup_Info cfwAomStaticCtxBackupInfo;
    //  sim
    T_Cfw_Usim_Static_Context_Backup_Info cfwUsimStaticCtxBackupInfo;
    //  nw
    //T_Cfw_Nw_Static_Context_Backup_Info cfwNwStaticCtxBackupInfo;
    // gprs
    T_Cfw_Gprs_Static_Context_Backup_Info cfwGprsStaticCtxBackupInfo;
    // sms
   // T_Cfw_Sms_Static_Context_Backup_Info cfwSmsStaticCtxBackupInfo;
    // pbk
    //T_Cfw_Pbk_Static_Context_Backup_Info cfwPbkStaticCtxBackupInfo;
    // cc
    //T_Cfw_Cc_Static_Context_Backup_Info cfwCcStaticCtxBackupInfo;
    // ???
}T_Cfw_Static_Context_Backup_Info;

typedef struct T_Cfw_Dynamic_Context_Backup_Info_Tag
{
    //  aom
    //T_Cfw_Aom_Dynamic_Context_Backup_Info cfwAomDynamicCtxBackupInfo;
    //  sim
   // T_Cfw_Sim_Dynamic_Context_Backup_Info cfwSimDynamicCtxBackupInfo;
    //  nw
    T_Cfw_Nw_Dynamic_Context_Backup_Info cfwNwDynamicCtxBackupInfo;
    // gprs
    T_Cfw_Gprs_Dynamic_Context_Backup_Info cfwGprsDynamicCtxBackupInfo[2];
    // sms
   // T_Cfw_Sms_Dynamic_Context_Backup_Info cfwSmsDynamicCtxBackupInfo;
    // pbk
   // T_Cfw_Pbk_Dynamic_Context_Backup_Info cfwPbkDynamicCtxBackupInfo;
    // cc
   // T_Cfw_Cc_Dynamic_Context_Backup_Info cfwCcDynamicCtxBackupInfo;
    // ???
}T_Cfw_Dynamic_Context_Backup_Info;

typedef struct T_CSW_Static_Context_Backup_Info_Tag
{
//AT task
   T_AT_Static_Context_Backup_Info atStaticCtxBackupInfo;
// 

#ifdef NBIOT_APPLICATION_PSM
//APS task
T_Aps_Static_Context_Backup_Info apsStaticCtxBackupInfo[2];
#endif //NBIOT_APPLICATION_PSM

//CFW
T_Cfw_Static_Context_Backup_Info cfwStaticCtxBackupInfo;
}
T_CSW_Static_Context_Backup_Info;

typedef struct T_CSW_Dynamic_Context_Backup_Info_Tag
{
//AT task
  // T_AT_Dynamic_Context_Backup_Info atDynamicCtxBackupInfo;
// 

//APS task
//T_Aps_Dynamic_Context_Backup_Info apsDynamicCtxBackupInfo[4];


//CFW
T_Cfw_Dynamic_Context_Backup_Info cfwDynamicCtxBackupInfo;
}
T_CSW_Dynamic_Context_Backup_Info;


typedef struct T_CSW_Context_Backup_Info_Tag
{
    //static ctx
    T_CSW_Static_Context_Backup_Info staticBackupInfo;

    //dynamic ctx
    T_CSW_Dynamic_Context_Backup_Info dynamicBackupInfo;
}
T_CSW_Context_Backup_Info;


void CswPsmDynamicRestoreTrigger();
void CswPsmSaveBackupInfo(T_CSW_Context_Backup_Info *backupInfo);
void CswPsmRestoreBackupInfo(T_CSW_Context_Backup_Info *backupInfo);
#endif//LTE_NBIOT_SUPPORT

#endif//CSW_SDB_H

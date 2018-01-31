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

#ifdef CFW_GPRS_SUPPORT

#include <csw.h>



#ifndef _CFW_GPRS_DATA_H_
#define _CFW_GPRS_DATA_H_



#include <cswtype.h>
#include <cfw.h>


#define READ_NUMBER_SETTING_8(WHICH, X, FLAG)\
{HANDLE hkey;INT32 data;UINT8 type;UINT32 size;\
    if(REG_OpenKey(REG_HKEY_CURRENT_COMM, TEXT(FLAG), &hkey))\
    {\
        if(REG_QueryValue(hkey, TEXT(WHICH), &type, (void*)&data, &size))\
        {REG_CloseKey(hkey); X = (UINT8)data;}\
        else{REG_CloseKey(hkey);return ERR_CFG_READ_REG_FAILURE;}\
    }else{return ERR_CFG_READ_REG_FAILURE;}\
}

#define WRITE_NUMBER_SETTING_8(WHICH, X, FLAG)\
{ \
     if(ERR_SUCCESS != cfg_write_8(FLAG, TEXT(WHICH),X))\
          return ERR_CFG_WRITE_REG_FAILURE;\
}

#define WRITE_STRING_SETTING(WHICH, X, FLAG)\
{ \
     if(ERR_SUCCESS != cfg_write_str(FLAG, TEXT(WHICH),(UINT8*)X,SUL_Strlen((UINT8*)X))\
          return ERR_CFG_WRITE_REG_FAILURE;\
}

#define WRITE_BIN_SETTING(WHICH, X, FLAG, LEN)\
{ \
     if(ERR_SUCCESS != cfg_write_hex(FLAG, TEXT(WHICH),(UINT8*)X,LEN))\
          return ERR_CFG_WRITE_REG_FAILURE;\
}

#define READ_STRING_SETTING(WHICH, X, FLAG)\
{HANDLE hkey;UINT8 type;UINT32 size;\
    if(REG_OpenKey(REG_HKEY_CURRENT_COMM, TEXT(FLAG), &hkey))\
    {\
        if(REG_QueryValue(hkey, TEXT(WHICH), &type, X, &size))\
        {REG_CloseKey(hkey); }\
        else{REG_CloseKey(hkey);return ERR_CFG_READ_REG_FAILURE;}\
    }else{return ERR_CFG_READ_REG_FAILURE;}\
}

#define READ_BIN_SETTING(WHICH, X, FLAG, len)\
{HANDLE hkey;UINT8 type;UINT32 size;\
    if(REG_OpenKey(REG_HKEY_CURRENT_COMM, TEXT(FLAG), &hkey))\
    {\
        if(REG_QueryValue(hkey, TEXT(WHICH), &type, X, &size))\
        {REG_CloseKey(hkey); len = (UINT8)size;}\
        else{REG_CloseKey(hkey);return ERR_CFG_READ_REG_FAILURE;}\
    }else{return ERR_CFG_READ_REG_FAILURE;}\
}

#ifdef LTE_NBIOT_SUPPORT
#define CFW_PDPCID_MAX							7


#define MAX_TFT_FILTER_NUM                      16
#define IPV4_REMOTE_ADDRESS_TYPE                0x10
#define IPV4_LOCAL_ADDRESS_TYPE                 0x11
#define IPV6_REMOTE_ADDRESS_TYPE                0x20
#define IPV6_REMOTE_ADDRESS_PREFIX_LENGTH_TYPE  0x21
#define IPV6_LOCAL_ADDRESS_PREFIX_LENGTH_TYPE   0x23
#define PROTOCOL_IDENTIFIER_NEXT_HEADER_TYPE    0x30
#define SINGLE_LOCAL_PORT_TYPE                  0x40
#define LOCAL_PORT_RANGE_TYPE                   0x41
#define SINGLE_REMOTE_PORT_TYPE                 0x50
#define REMOTE_PORT_RANGE_TYPE                  0x51
#define SECURITY_PARAMETER_INDEX_TYPE           0x60
#define TYPE_OF_SERVICE_TRAFFIC_CLASS_TYPE      0x70
#define FLOW_LABEL_TYPE                         0x80

typedef enum
{
    E_CFW_TFT_ERR_NO,
    E_CFW_TFT_ERR_SAME_PRIO_NOT_DEFAULT,
    E_CFW_TFT_ERR_SAME_PRIO_DEFAULT,
    E_CFW_TFT_ERR_OP_SEMA = 41,
    E_CFW_TFT_ERR_OP_SYNT = 42,
    E_CFW_TFT_ERR_PF_SEMA = 44,
    E_CFW_TFT_ERR_PF_SYNT = 45,
    E_CFW_TFT_ERR_UNDEFINE= 0xFF
}E_CFW_TFT_ERR;


typedef enum
{
    E_TFT_OP_IGNORE,
    E_TFT_OP_CREATE,
    E_TFT_OP_DELETE,
    E_TFT_OP_ADD,
    E_TFT_OP_REPLACE,
    E_TFT_OP_DEL_PF,
    E_TFT_OP_NO_OP,
    E_TFT_OP_REVERSE
}E_TFT_OP;

typedef struct _tft_head_bit
{
    UINT8 numOfPf:4;
    UINT8 Ebit:1;
    UINT8 tftOp:3;
}TFT_HEAD_BIT;

typedef struct _pf_head_bit
{
    UINT8 id:4;
    UINT8 dir:2;
    UINT8 spare:2;
}PF_HEAD_BIT;

typedef struct _pf_head
{
    UINT8 idAndDir;
    UINT8 prior;
    UINT8 length;
}PF_HEAD;

typedef struct 
{
    UINT8 remoteAddr;
    UINT8 locPortType;
    UINT8 remotePortType;
}CFW_CPNT_CFLT;
#endif
typedef struct _CID2INDEX
{
    UINT8 nCid;
    UINT8 nNsapi;
    UINT8 nSmIndex;
    UINT8 nLinkedEbi;
} CID2INDEX ;

typedef struct _GPRS_SM_INFO
{
    UINT32 nMsgId;           // message id
    VOID *pMsgBody;          // message body
    CID2INDEX Cid2Index;     // cid smindex, nsapi
    CFW_GPRS_QOS ModifyQos;  // modify qos???
    UINT8 nModifyCid;        // ...???
    UINT8 nActState;         // activated?
    UINT8 nPDPAddrSize;   // PDP address size , only if the flash error;
    BOOL  bAttManualAcc;  // manual acc ?

    UINT8 nPDPAddr[4];        // used for ip address only if the flash is error ;// ip v4
    //CFW_GPRS_PDPCONT_INFO PdpCXT;
    CFW_GPRS_QOS Qos;
#ifdef LTE_NBIOT_SUPPORT
    CFW_EQOS ModifyEQos;
    CFW_EQOS EQos;
    CFW_TFT Tfa;
    CFW_TFT Tft;
    UINT8 nSecActCid;
    BOOL  bDeactByCfw;
#endif
    UINT8 nFlowCtr;              // 1 uplink congestion or low memory, 2 uplink congestion or low memory disappear
#ifdef _QUICK_QUIT_WAP_
    //Add control that send deactived message once
    //QQWDMO == quickly quick wap deactived message only send once
    UINT8 nQQWDMO;
#endif
    COS_CALLBACK_FUNC_T func;
} GPRS_SM_INFO;
#define THE_APN_MAX_LEN    100
#define THE_PDP_ADDR_MAX_LEN    18
#define THE_APN_USER_MAX_LEN    20
#define THE_APN_PWD_MAX_LEN    20

#ifdef LTE_NBIOT_SUPPORT
VOID CFW_NvParasChangedNotification(UINT8  ChangedValue);

typedef struct pcoContainer_tag
{
    UINT32          type;
    UINT16        ContId;
}pcoContainer;

typedef enum 
{
    PCO_TYPE_LCP,                    //C021H
    PCO_TYPE_PAP,                    //C023H
    PCO_TYPE_CHAP,                   //C223H
    PCO_TYPE_IPCP,                    //8021H
    PCO_TYPE_PCSCF_IPV6,         //0001H
    //PCO_TYPE_IMCN,
    PCO_TYPE_IPV6DNS,              //0003H
    PCO_TYPE_BCI,                      //0005H
    PCO_TYPE_IPADDR_VIA_NAS, //000AH
    PCO_TYPE_PCSCF_IPV4,  //000CH
    PCO_TYPE_IPV4DNS,      //000DH
    PCO_TYPE_MSISDN,      //000E
    PCO_TYPE_IPV4_MTU,   //0010H
    PCO_TYPE_TFT_IND,   //0011H
    PCO_TYPE_NONIP_MTU, //0015H
    PCO_TYPE_APN_RATE,  //0016H
    PCO_TYPE_MAX
}PCO_TYPE_ENUM_T;
static VOID CFW_Decode_ePCO(UINT8 pdnType,UINT8 cid, UINT16 pcoLen, UINT8 *ePCO);
static UINT16 CFW_Encode_ePCO(UINT8 pdnType, UINT8 *ePCO, CFW_GPRS_PDPCONT_INFO *pdpCtx);
UINT8 CFW_GprsGetPdpIpv4Addr(UINT8 nCid, UINT8 *nLength, UINT8 *ipv4Addr, CFW_SIM_ID nSimID);
UINT8 CFW_GprsGetPdpIpv6IfId(UINT8 nCid, UINT8 *nLength, UINT8 *if_id, CFW_SIM_ID nSimID);
//const ip6_addr_t *CFW_GprsGetPdpIpv6AddressFromDest(UINT8 nCid, const ip6_addr_t *dest, UINT8 nSim);
//const ip6_addr_t *CFW_GprsGetPdpIpv6Address(UINT8 nCid, UINT8 nSim);
UINT32 CFW_GprsClrCtxTFT(UINT8 nCid, UINT8 nSimID);
UINT8 CFW_GprsSavePFinActProc(CFW_TFT_PARSE *pTftParse, UINT8 nSimID);
UINT8 CFW_GprsTftCheckInAct(CFW_TFT* pTft, CFW_TFT_PARSE *pTftParse, CFW_SIM_ID nSimID);
UINT8 CFW_GprsGetCidFromNsapi(UINT8 Nsapi, UINT8 nSimID);
UINT8 CFW_GprsSetCidContext(UINT8 PCid, UINT8 LinkEBI, UINT8 nSimID);
UINT8 CFW_GprsSavePFinModProc(CFW_TFT_PARSE *pTftParse, UINT8 nSimID);
UINT8 CFW_GprsTftCheckInMod(CFW_TFT* pTft, CFW_TFT_PARSE *pTftParse, CFW_SIM_ID nSimID);

#endif //LTE_NBIOT_SUPPORT

#endif //
#endif


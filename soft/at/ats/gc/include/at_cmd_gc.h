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



#ifndef __AT_CMD_GC_H__
#define __AT_CMD_GC_H__

struct COS_EVENT;

struct AT_CMD_PARA;


typedef struct _CALA
{
    UINT8 time[20];
    UINT8 text[36];
    UINT8 reccurr[16];  // 15->16
    UINT8 index;
    UINT8 type;
    UINT8 silent;
    UINT8 padding;
} CALA;

extern UINT8 g_gc_pwroff;
extern UINT8 g_pCeer[];

// TODO: need to load configuration from flash storage, and be called by Init module when system boot up.
/*
FUNCTION:
Init GC module, includes load stored configuration value from flash storage.

PARAMETER:
N/A

RETURN:
N/A
**/
extern BOOL AT_GC_Init(VOID);
extern VOID AT_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                         UINT8 nDLCI, UINT8 nSim);
extern VOID AT_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
extern VOID AT_GC_AsyncEventProcess(COS_EVENT *pEvent);
extern VOID AT_GC_CmdFunc_CMEE(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CFUN(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_SFUN(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CSCLK(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CSCS(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CCLK(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_E(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_V(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_Q(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_Z(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CALA(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_AndF(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_X(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CPOF(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_S3(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_S4(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_S5(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_ICF(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_IFC(AT_CMD_PARA *pParam); // add by wulc 2012.02.07
extern VOID AT_GC_CmdFunc_IPR(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CPAS(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CIND(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CEER(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CMER(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_CALD(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_AndW(AT_CMD_PARA *pParam);  // hameina[+]20080325
extern VOID AT_GC_CmdFunc_S0(AT_CMD_PARA *pParam);  // hameina[+]20080327
extern VOID AT_GC_CmdFunc_EGMR(AT_CMD_PARA *pParam);  // add by wulc 2012.11.13

extern VOID AT_GC_CmdFunc_SUPS(AT_CMD_PARA *pParam);
// [+]for AT^CDTMF 2007.11.12
extern VOID AT_GC_CmdFunc_CDTMF(AT_CMD_PARA *pParam);

extern UINT16 gc_GetCfgInfo_IndCtrlMask(VOID);
extern BOOL gc_SetCfgInfo_IndCtrlMask(UINT16 mask);

extern at_chset_t cfg_GetTeChset(void);
extern void cfg_SetTeChset(at_chset_t chset);
extern at_chset_t cfg_GetMtChset(void);
extern VOID AT_GC_CmdFunc_ACLB(AT_CMD_PARA *pParam);

extern VOID AT_GC_SETOOKMODE(AT_CMD_PARA* pParam);
extern VOID AT_GC_CmdFunc_OTP(AT_CMD_PARA *pParam);

BOOL _GetACLBNWStatus(CFW_SIM_ID nSimID);
UINT32 _SetACLBNWStatus(UINT8 nACLBNWStatus,CFW_SIM_ID nSimID);
#ifdef MMI_ONLY_AT
BOOL _GetACLBStatus();
UINT32 _SetACLBStatus(UINT8 nACLBNWStatus);
#endif
BOOL _GetACLBStep();
UINT32 _SetACLBStep(UINT8 nACLBNWStep);
extern VOID AT_GC_CmdFunc_GCHS(AT_CMD_PARA *pParam);
extern BOOL hal_HstSendEvent(UINT32 ch);

#ifdef LTE_NBIOT_SUPPORT
extern VOID AT_GC_CmdFunc_NVCFGARFCN(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_NVSETSCMODE(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_LOCKFREQ(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_NVSETBAND(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_NVSWITCHBS(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_NVSETRSRPOFFSET(AT_CMD_PARA *pParam);
extern VOID AT_GC_CmdFunc_NVSETRRCRLSTIMER10(AT_CMD_PARA *pParam);
#endif


#endif


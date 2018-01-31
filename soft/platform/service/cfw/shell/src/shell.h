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























#ifndef _CSW_SHELL_H_
#define _CSW_SHELL_H_

#define ERR_CFW_REPEAT_TRANSFER         0x10000011

// =================================================================
// For Shell states
//
// =================================================================
#define SHELL_STATE_SIM                     0x00
#define SHELL_STATE_SMS                     0x01
#define SHELL_STATE_PBK                     0x02
#define SHELL_STATE_SIM_FAC                 0x03
#define SHELL_STATE_SIM_IMSI                0x04
#define SHELL_STATE_SAT                         0x05
#define SHELL_STATE_WAIT_NW_WITHSIM         0x06
#define SHELL_STATE_AUTO_RESP               0x07
#define SHELL_STATE_WAIT                            0xFF

#define CFW_INIT_SIM_FULL_SERVER            0x02
#define CFW_INIT_SIM_ROAMING_SERVER         0x03
#define CFW_INIT_SIM_NO_SERVER              0x04
#define CFW_INIT_SIM_LIMITED_SERVER         0x05

// =================================================================
// For SAT
// SAT_SUPPORT ( 1 == 1 ) | ( 1 == 0 )
// =================================================================
#define SAT_SUPPORT                         (1 == 1)

// =================================================================
// For TestMode
//
// =================================================================


VOID CFW_EMCDial( CFW_SIM_ID nSimID );
UINT8 CFW_GetAutoCallCmd(CFW_SIM_ID nSimID);
extern VOID PM_ProLPMDisable(VOID);

extern const UINT8 TerminalResponse[12];
extern UINT32 CFW_SatActivationProc(HAO Hao, CFW_EV *pEvent);
extern BOOL CFW_BalSetupHwCbs(CFW_PFN_AUD_CBS *pHwFun);
#ifdef LTE_NBIOT_SUPPORT
extern UINT32 CFW_Exit(UINT8 detachType);
#else
extern UINT32 CFW_Exit(VOID);

#endif
extern BOOL DM_KeyboardInit();
extern UINT32 CFW_CfgGetIMSI(UINT8 *pIMSI, CFW_SIM_ID nSimID);


#define AUTO_CALL_SETTIGN_DIAL                0x01
#define AUTO_CALL_SETTING_RESP                0x02
#define DIAL_EMC_MAXNUM                     0x03

#endif // _H


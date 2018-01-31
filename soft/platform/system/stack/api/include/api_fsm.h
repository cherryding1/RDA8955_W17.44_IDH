//================================================================================
//  File       : api_fsm.h
//--------------------------------------------------------------------------------
//  Scope      : Finite State Machine defintions for lapdm
//  History    :
//--------------------------------------------------------------------------------
//  Mar 31 06  |  MCE   | Added api_TrsUlCongInd and api_TrsMemoryAlert        
//  Nov 15 05  |  MCE   | Added api_TrsDbgInfoCmd 
//  Mar 10 04  |  PCA   | Creation
//================================================================================

#ifndef __API_FSM_H__
#define __API_FSM_H__

#include "fsm.h"

/*
=======================
  API state definitions
=======================
*/
#define API_GENE                   0
#define API_INIT                   1
#define API_SEARCHING              2
#define API_IDLE		   3
#define API_CONNECTED              4

#if !(STK_MAX_SIM==1)
extern Fsm_t Api_Fsm[STK_MAX_SIM];
#else
extern Fsm_t Api_Fsm[];
#endif
/*
** Transitions prototypes
*/
u8 api_DoNothing            ( void );
u8 api_TrsStartReq          ( void );
u8 api_TrsSimOpenInd        ( void );
u8 api_TrsSimCloseInd       ( void );
u8 api_TrsDbgInfoCmd        ( void );
u8 api_TrsStopReq           ( void );
u8 api_TrsImeiReq           ( void );
//add by yanzhili
#ifdef LTE_NBIOT_SUPPORT
u8 api_TrsSelModeReq( void );
void api_AdaptNwSelReq(void);
void api_AdaptStkStopCnf(void);
void api_AdaptNwListAbortReq(void);
void api_NbNwPsRegInd(void);
#endif
 // [[  add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
u8 api_PsAttachReq(void);
u8 api_PsDetachReq(void);
 // ]] add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP

// RR -> API
u8 api_TrsNwListInd         ( void );
u8 api_TrsStopCnf           ( void );
u8 api_TrsSimCloseCnf( void );/*added by liujg*/

// SMS -> API
u8 api_TrsSmsPPSendReq      ( void );
u8 api_TrsSmsPPSendMMAReq   ( void );
u8 api_TrsSmsPPAckReq       ( void );
u8 api_TrsSmsPPErrorReq     ( void );
u8 smspp_TrsAckInd          ( void );
u8 smspp_TrsErrorInd        ( void );
u8 smspp_TrsReceiveInd      ( void );

// RLU -> API
u8 api_TrsUlCongInd         ( void );
#ifdef LTE_NBIOT_SUPPORT
// DS -> API
u8 api_RabmFlowCtrlInd    (void);   
#endif 

// SX -> API
void api_TrsMemoryAlert ( u32 Status ) ; // This is actually a callback function

// API -> API
u8 api_TrsMemAlertExp       ( void );

#endif // __API_FSM_H__

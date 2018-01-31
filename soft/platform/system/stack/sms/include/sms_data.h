//================================================================================
//  File       : sms_data.h
//--------------------------------------------------------------------------------
//  Scope      : Defintion of Global data for sms
//  History    :
//--------------------------------------------------------------------------------
// DMX | 20081024 | Add dual Sim support
//  Feb 17 04  |  ABA   | Creation
//================================================================================
#ifndef __SMS_DATA_H__
#define __SMS_DATA_H__

#include "sms_msg.h"
#include "smr_data.h"   // Structures for smr
#include "smc_data.h"   // Structures for smc

typedef enum
{
  SMS_MO,
  SMS_MT,
  DIRECTION_NOT_INITIALIZED
}SmsDirection_t;

typedef struct {

  /*
  ** General global data
  */
  Msg_t          * RecMsg      ; // Received message
  Msg_t          * OutMsg      ; // Message to send

  Fsm_t          * Fsm         ; // Current selected Fsm
  InEvtDsc_t     * IEvtDsc     ; // Incoming Event
  OutEvtDsc_t    * OEvtDsc     ; // Outgoing Event

  bool             KeepMsg     ; // TRUE if RecMsg must not be deleted

  smr_MOData_t     MOSmr       ; // Data for mobile originating smr
  smr_MTData_t     MTSmr       ; // Data for mobile terminating smr
  smc_MOData_t     MOSmc       ; // Data for mobile originating smc
  smc_MTData_t     MTSmc       ; // Data for mobile terminating smc
  Msg_t          *      InternalMsg ; // Internal message smr<->smc
  SmsDirection_t   SmsDirection; // 0: SMS_MO (TX)  1: SMS_MT (RX)
  bool                    MOSmsConcat ;
  bool                    Path        ; // GSM or GPRS
  bool                    PreMOSmsConcat ;

  // dingmx 这四个指针是为了减小单双卡版本代码的差异
  Fsm_t*			FSM_RO;
  Fsm_t*			FSM_RT;
  Fsm_t*			FSM_CO;
  Fsm_t*			FSM_CT;
#if !(STK_MAX_SIM==1)
	u32					SimIndex;  // 双卡模式{0,1}， 单卡模式1
#endif
}sms_Data_t;

#ifdef __SMS_C__
#define DefExtern
#else
#define DefExtern extern
#endif

// SMS Finite State Machines


#if !(STK_MAX_SIM==1)
extern Fsm_t      smr_MO_Fsm [STK_MAX_SIM] ;
extern Fsm_t      smr_MT_Fsm [STK_MAX_SIM] ;
extern Fsm_t      smc_MO_Fsm [STK_MAX_SIM] ;
extern Fsm_t      smc_MT_Fsm [STK_MAX_SIM] ;
#else
extern Fsm_t      smr_MO_Fsm  ;
extern Fsm_t      smr_MT_Fsm  ;
extern Fsm_t      smc_MO_Fsm ;
extern Fsm_t      smc_MT_Fsm  ;
#endif

DefExtern sms_Data_t  *gp_smsCurData ;//


#undef DefExtern
#endif

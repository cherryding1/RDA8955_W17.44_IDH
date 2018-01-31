//------------------------------------------------------------------------------
//  $Log: sms_evt.h,v $
//  Revision 1.7  2006/06/08 13:35:37  aba
//  Abort of the MMA procedure
//
//  Revision 1.6  2006/05/22 09:20:21  aba
//  Memory optimization
//
//  Revision 1.5  2006/05/22 07:56:04  aba
//  Copyrigth is updated
//
//  Revision 1.4  2006/02/28 16:15:46  aba
//  Issue 360: New SMS MO sending impossible after wrong SC address used (CM_Service_Rej) => Send of CP_ERROR during abort
//  Add PDTI in MM_RELEASE_REQ
//
//  Revision 1.3  2006/01/24 15:41:55  aba
//  Changing of DefExtern into extern for the event definition
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : sms_evt.h
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of sms

  History    :
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __SMS_EVT_H__
#define __SMS_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition

#ifdef SMS_EVT
#define DefExtern
#else
#define DefExtern extern
#endif

/*
==========================================
                   Flags
==========================================
*/
#define SMS_FLAG_SMR      (1<<0)   // SMR FSM
#define SMS_FLAG_SMC      (1<<1)   // SMC FSM
#define SMS_FLAG_MT       (1<<2)   // Mobile terminating
#define SMS_FLAG_MO       (1<<3)   // Mobile originating

/*
==========================================
             Incoming Events
==========================================
*/
/*
** Internal event
*/
extern InEvtDsc_t smr_smrTR1MExpEvt           ;
extern InEvtDsc_t smr_smrTR1MReExpEvt     ;  // SMR_RESEND_DELAY
extern InEvtDsc_t smr_smrTR2MExpEvt           ;
extern InEvtDsc_t smr_smrTRAMExpEvt           ;
extern InEvtDsc_t smc_smcMOTC1MExpEvt         ;
extern InEvtDsc_t smc_smcMTTC1MExpEvt         ;
extern InEvtDsc_t smc_smrMODataReqEvt         ;
extern InEvtDsc_t smc_smrMTDataReqEvt         ;
extern InEvtDsc_t smc_smrEstabReqEvt          ;
extern InEvtDsc_t smc_smrMOReleaseReqEvt      ;
extern InEvtDsc_t smc_smrMOAbortReqEvt        ;
extern InEvtDsc_t smc_smrMTAbortReqEvt  ;
extern InEvtDsc_t smr_smcDataIndEvt           ;
extern InEvtDsc_t smr_smcEstabErrEvt          ;
extern InEvtDsc_t smr_smcMOErrorIndEvt        ;
extern InEvtDsc_t smr_smcMTErrorIndEvt        ;
extern InEvtDsc_t smr_SelfSendEvt			  ;

/*
** Interface with API
*/
extern InEvtDsc_t  smr_apiSmsPPSendReqEvt     ;
extern InEvtDsc_t  smr_apiSmsPPSendMMAReqEvt  ;
extern InEvtDsc_t  smr_apiSmsPPAckReqEvt      ;
extern InEvtDsc_t  smr_apiSmsPPErrorReqEvt    ;
extern InEvtDsc_t  smr_apiSmsPPAbortMMAReqEvt ;
extern InEvtDsc_t  smr_apiSmsPPAbortReqEvt ;


/*
** Interface with MM (GSM)
*/
extern InEvtDsc_t  smc_mmServReqAckEvt  ;
extern InEvtDsc_t  smc_mmEstabCnfEvt          ;
extern InEvtDsc_t  smc_mmEstabErrEvt          ;
extern InEvtDsc_t  smc_mmReleaseIndEvt        ;
extern InEvtDsc_t  smc_mmDataIndEvt           ;

/*
** Interface with MMP (GPRS)
*/
extern InEvtDsc_t  smc_mmpAbortIndEvt         ;
extern InEvtDsc_t  smc_mmpDataIndEvt          ;

extern InEvtDsc_t  smc_mmCpDataEvt            ; // Derived event
extern InEvtDsc_t  smc_mmCpAckEvt             ; // Derived event
extern InEvtDsc_t  smc_mmCpErrorEvt           ; // Derived event
extern InEvtDsc_t  smc_mmError97Evt           ; // Dervied event
extern InEvtDsc_t  smr_smcRpDataEvt           ; // Derived event
extern InEvtDsc_t  smr_smcRpAckEvt            ; // Derived event
extern InEvtDsc_t  smr_smcRpErrorEvt          ; // Derived event
extern InEvtDsc_t  smr_smcError95Evt          ; // Derived event
extern InEvtDsc_t  smr_smcError97Evt          ; // Derived event

/*
** General table of incoming events for SMS
*/
extern InEvtDsc_t  * const sms_EvtList[]      ;

/*
==========================================
             Outgoing Events
==========================================
*/
/*
**  Outgoing SMC --> MM Events
*/
extern OutEvtDsc_t smc_mmEstabReqEvt          ;
extern OutEvtDsc_t smc_mmReleaseReqEvt        ;
extern OutEvtDsc_t smc_mmDataReqEvt           ;
extern OutEvtDsc_t smc_mmpDataReqEvt          ;
extern OutEvtDsc_t smc_mmEstabIndEvt          ;
extern OutEvtDsc_t smr_apiAckIndEvt           ;
extern OutEvtDsc_t smr_apiErrorIndEvt         ;
extern OutEvtDsc_t smr_apiReceiveIndEvt       ;
extern OutEvtDsc_t smr_OutSelfSendEvt	      ;

/*
** Prototypes
*/
extern s16 sms_GiveEvtdx (u32 EvtId ) ;

#undef DefExtern
#endif  // __SMS_EVT_H__

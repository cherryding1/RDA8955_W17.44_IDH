//================================================================================
//  File       : mmp_ctxt.h
//--------------------------------------------------------------------------------
//  Scope      : Context for the Packet Switch MM layer
//  History    :
//--------------------------------------------------------------------------------
//  Revision 1.8  2006/06/05 16:06:28  fch  Review of the RR_SUSPEND_REQ (do not lead to change state).  When a reselection occurs during the GPRS Idle period (Suspend + CellInd) while MMP is waiting for an answer from NW, on the CellInd the CellUpdate is performed before continuing the ongoing procedure (Mantis #433)
//  Revision 1.6  2006/02/28 13:14:02  fch  Correction for T3302 handling
//  Revision 1.5  2006/02/15 19:15:55  fch  Added flag MMP_MM_LOCUP_PENDING to prevent MMC to perform locup while MMP performs combined proc
//  Revision 1.4  2006/02/15 15:45:41  fch  Add RR_SUSPEND_REQ handling.  Flag __MMP_NO_SUSPEND_REQ__ (temporary) allows to revert to MMP not handling RR_SUSPEND_REQ
//  Revision 1.3  2006/02/09 15:37:23  fch  Added ResumeToDo field in mmpCtx variable to handle the Resume field included in the RR_RELEASE_IND received by MMC
//  Aug 06 03  |  OTH   | Creation            
//================================================================================


#ifdef __GPRS__

#ifndef __MMP_CTXT_H__
#define __MMP_CTXT_H__

// System includes
#include "sxs_type.h"

// Internal includes
#include "mm_intra.h"
#include "mmc_mmp.h"

#ifdef __MM_C__
#define DefExtern
#else
#define DefExtern extern
#endif

typedef struct
{
  u8 Unit;
  u8 Value;
}mmp_Timer_t;


//================================================================================
//  Data       : mmp_Ctxt_t
//--------------------------------------------------------------------------------
//  Scope      : Global Packet Switch MM context
//================================================================================

typedef struct
{
  u8 Flags;
#define MMP_FLAGS_CLEAR     0
#define MMP_ATTACHED        ( 1 << 0 )
#define MMP_READY_STATE     ( 1 << 1 )
#define MMP_EF_GKC_OK       ( 1 << 2 )
#define MMP_EF_GLOCI_OK     ( 1 << 3 )
#define MMP_CELL_NOTIF      ( 1 << 4 )
#define MMP_STK_STOPPED     ( 1 << 5 )
#define MMP_EXIT_PENDING    ( 1 << 6 )

#define MMP_SIM_GPRS_AWARE  (MMP_EF_GKC_OK | MMP_EF_GLOCI_OK)

  u16   FlagsProc;
#define MMP_ATTACH_NEEDED         ( 1 << 0  ) // Attach requested by API
#define MMP_COMBINED_PIP          ( 1 << 1  ) // Combined proc in progress
#define MMP_ATTEMPT_UPDT_MM       ( 1 << 2  ) // MM updating has failed in comb proc
#define MMP_RETRY_NEW_CELL        ( 1 << 3  ) // Retry the proc in new cell
#define MMP_UPDATE_NEEDED         ( 1 << 4  ) // Update to be done
#define MMP_RAU_DELAYED           ( 1 << 5  ) // RAU delayed after MM update
#define MMP_T3312_EXP_OUT_PLMN    ( 1 << 6  ) // T3312 has expired while out of PLMN
#define MMP_T3312_EXP_OUT_GPRS    ( 1 << 7  ) // T3312 has expired while out of GPRS
#define MMP_DETACH_NEEDED         ( 1 << 8  ) // Detach preempted by a RAU
#define MMP_UPDATE_INTERRUPTED    ( 1 << 9  ) // Procedure (Att or RAU) interrupted by MMC
#define MMP_MM_LOCUP_PENDING      ( 1 << 10 ) // MM updating is pending


#define MMP_T3312_EXP_FLAGS         \
         (MMP_T3312_EXP_OUT_PLMN | MMP_T3312_EXP_OUT_GPRS)

  u16    FlagsSusp;
#define MMP_LLC_SUSPENDED         ( 1 << 0 )
#define MMP_GPRS_SUSPENDED        ( 1 << 1 )


#define MMP_SUSP_T3310            ( 1 << 2 )  
#define MMP_SUSP_T3321            ( 1 << 3 )  
#define MMP_SUSP_T3330            ( 1 << 4 )  
#define MMP_SUSP_T3311            ( 1 << 5 )  
#define MMP_SUSP_T3302            ( 1 << 6 )  
#define MMP_SUSP_T3312            ( 1 << 7 )  
/*added by liujg*/
#define MMP_DUAL_SUSPENDED  ( 1 << 8)


  u8    CurProc;
#define MMP_NO_PROC     0
#define MMP_GPRS_ATTACH 1
#define MMP_RAU         2
#define MMP_GPRS_DETACH 3

  u8    LastRauType;        // Last type of RAU done
  mmp_Timer_t ReadyTim;     // T3314
  u32   PeriodicRauTimVal;  // T3312
  u8    LastRegNmo;         // Last NMO where the MS has registered

  u8    DetachStuff;        // Info about Detach
  u8    DetTimExpCnt;       // Counter of the number of expiry for T3321

  u32   Tlli;
  u8    LlcQueueMmp;  // Id of the queue for the interface MMP/LLC (Sapi 1)
  u8    LlcQueueSms;  // Id of the queue for the interface SMS/LLC (Sapi 7)
  u8    RadioPrioSMS; // SMS Radio Priority
  u8    DataReqQ;     // Queue to store the DATA_REQ from L3 if necessary
#define MMP_NO_QUEUE  0xFF

  u8    AttAtmptCnt;  // GPRS attach attempt counter
  u8    RauAtmptCnt;  // Routing Area Update attempt counter
  u8    TimFlag;
#define MMP_TIM_FLAG_CLEAR    0
#define MMP_T3302_FLG   ( 1 << 0 )
#define MMP_T3310_FLG   ( 1 << 1 )
#define MMP_T3311_FLG   ( 1 << 2 )
#define MMP_T3312_FLG   ( 1 << 3 )
#define MMP_T3314_FLG   ( 1 << 4 )
#define MMP_T3321_FLG   ( 1 << 5 )
#define MMP_T3330_FLG   ( 1 << 6 )
//#define MMP_T3316_FLG ( 1 << 5 )  // UMTS Only
//#define MMP_T3318_FLG ( 1 << 6 )  // UMTS Only
//#define MMP_T3320_FLG ( 1 << 7 )  // UMTS Only

  u32   T3302Val;     

  u8    TimExpCnt;    // Counter of the number of expiry (used for T3310 and T3330)
  u8    FPlmnQ;       // Identifier of the Forbidden PLMN for GPRS service list
  u8    PdpCtxStatus[2];  // Status of the PDP context (NSAPI) to include in RAU Req
  u8    SynchRef;     // Reference for synchronisation with RLC
#define MMP_SYNCH_ID_MSK          0x1F  // 5 bits used
#define MMP_SYNCH_CAUSE_MSK       0xE0  // 3 bits used
#define MMP_SYNCH_CAUSE_IGNORE    ( 0 << 5 )
#define MMP_SYNCH_CAUSE_GOINACTIF ( 1 << 5 )
#define MMP_SYNCH_CAUSE_T3310     ( 2 << 5 )
#define MMP_SYNCH_CAUSE_T3330     ( 3 << 5 )
#define MMP_SYNCH_CAUSE_DETACH    ( 4 << 5 )

  u8    OldPTmsi[4];  // Old PTMSI still valid until LLC frame reception with
                      // local TLLI derived from the new PTMSI

  u8    BackState;    

 /*added by yangyang for edge at 2007-04-02*/
 #ifdef __EGPRS__
  u8    AccTypeSum;
  u8    AccType[14];
  u8    DefAccType[9]; //modified by yangyang for edge test at 2007-09-24
 #endif
 /* added by yangyang for edge at 2007-04-02*/
					  
  bool  ResumeToDo;
 
  u8 LastRand[MM_RAND_SIZE+1];
  u8 LastRes[MM_SRES_SIZE+MM_SRES_EXT_SIZE+1];
  bool bStkSimCloseCnfToSend ; //dmx 20150706,  MMP also need send SIM_CLOSE_CNF
  mm_PsAttachReq_t       mmPsAttachReq; // for support Follow On request
} mmp_Ctxt_t;



/*
================================================================================
  MMP Timers stuff
================================================================================
*/
typedef struct
{
  ascii *TimName;
  u16   TimFlag;
} mmp_TimDsc_t;


DefExtern const mmp_TimDsc_t mmpTimDsc[MMP_NB_TIM]
#ifdef __MM_C__
 =
{
  { "T3302", MMP_T3302_FLG },
  { "T3310", MMP_T3310_FLG },
  { "T3311", MMP_T3311_FLG },
  { "T3312", MMP_T3312_FLG },
  { "T3314", MMP_T3314_FLG },
  { "T3321", MMP_T3321_FLG },
  { "T3330", MMP_T3330_FLG }
//  { "T3316", MMP_T3316_FLG },
//  { "T3318", MMP_T3318_FLG },
//  { "T3320", MMP_T3320_FLG },
}
#endif
;


/*
================================================================================
  Combined result for MMC
================================================================================
*/
DefExtern const u8 mmpRauType2CombUpdType[4]
#ifdef __MM_C__
 =
{
 /* MMP_NORMAL_RA_UPDATE        */  MMC_MMP_RA_UPDATE,
 /* MMP_COMB_RA_UPDATE          */  MMC_MMP_COMB_RA_UPDATE,
 /* MMP_COMB_RA_UPDATE_IMSI_ATT */  MMC_MMP_COMB_RA_UPDATE,
 /* MMP_PERIODIC_UPDATING       */  MMC_MMP_PERIODIC_UPDATING
}
#endif
;





#undef DefExtern

#endif // __MMP_CTXT_H__

#endif // __GPRS__

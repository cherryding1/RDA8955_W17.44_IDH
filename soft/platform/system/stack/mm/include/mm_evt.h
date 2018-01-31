//------------------------------------------------------------------------------
//  $Log: mm_evt.h,v $
//  Revision 1.3  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : mm_evt.h  
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events od mm Task               

  History    :
--------------------------------------------------------------------------------
  Aug 06 03  |  OTH   | Creation            
================================================================================
*/
#ifndef __MM_EVT_H__
#define __MM_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition
#include "itf_rr.h"
#include "itf_sim.h"
#include "itf_stk.h"
#include "itf_llc.h"

/*
** Flags for the MM events
*/
    // Flags for the MM incomming events
#define MM_MMC      ( 1 << 0 )
#define MM_MMP      ( 1 << 1 )
#define MM_RM       ( 1 << 2 )  // Radio Message
//#define MM_HAT ( 1 << 3 )

    // Flags for the MM outgoing events
#define MM_IL_MSG   ( 1 << 3 )
#define MMP_QF      ( 1 << 4 )  // Send with SXR_QUEUE_FIRST


/*
==========================================
             Incoming Events
==========================================
*/
/*
**  Internal Interface with RR
*/
extern InEvtDsc_t mm_rrIConnectCnf ;   
extern InEvtDsc_t mm_rrIReleaseInd ;   
extern InEvtDsc_t mm_rrICellInd    ;
extern InEvtDsc_t mm_rrIDataInd    ;
extern InEvtDsc_t mm_rrIPagingInd  ;
extern InEvtDsc_t mm_rrICiphInd    ;
extern InEvtDsc_t mm_rrICmkInd     ;
extern InEvtDsc_t mm_rrIEndCongInd ;

#ifdef __GPRS__
extern InEvtDsc_t mmp_rrIPReleaseInd;   
extern InEvtDsc_t mmp_rrISuspendReq;
extern InEvtDsc_t mmp_rrIPPagingInd;
#endif

/*
**  Interface with CM
*/
extern InEvtDsc_t mm_cmIEstabReq   ;
extern InEvtDsc_t mm_cmIEstabInd   ;
extern InEvtDsc_t mm_cmIReleaseReq ;
extern InEvtDsc_t mm_cmIAbortReq   ;
extern InEvtDsc_t mm_cmIReestabReq ;
extern InEvtDsc_t mm_cmIDataReq    ;

/*
**  Interface with SIM
*/
extern InEvtDsc_t mm_simIUpdBinCnf;

/*
** Interface with MM itself
*/
extern InEvtDsc_t mmc_ISelfLocUp  ;
extern InEvtDsc_t mmc_ISelfPaging;
#ifdef __GPRS__
extern InEvtDsc_t mmp_ISelfPaging;
extern InEvtDsc_t mmp_mmcAbortCurProc;
extern InEvtDsc_t mmp_mmcReStartPrevProc;
extern InEvtDsc_t mmp_GprsPending;
#endif

#ifdef __USING_SMS_STORE_PS_LOCI__
extern OutEvtDsc_t mm_simOReadRecodeReq;
extern OutEvtDsc_t mm_simOUpdRecodeReq ;
#endif

/*
** Interface with API
*/
extern InEvtDsc_t mm_apiISimOpenInd   ;
extern InEvtDsc_t mm_apiISimClosedInd ;
extern InEvtDsc_t mm_apiIStartReq     ;
extern InEvtDsc_t mm_apiIStopReq      ;


#ifdef __GPRS__
/*
** Interface with LLC
*/
extern InEvtDsc_t mmp_llcIQueueCreateInd;
extern InEvtDsc_t mmp_llcINewTlliInd;
extern InEvtDsc_t mmp_llcIStatusInd;

/*
** Interface with L3 (SM or SMS)
*/
extern InEvtDsc_t mmp_l3IDataReq;
extern InEvtDsc_t mmp_L3IDelSaveDataReq; // DMX add 20161031.  MMP_DEL_SAVE_DATA_REQ

/*
** Interface with SM
*/
extern InEvtDsc_t mmp_smIPdpCtxStatusInd;
extern InEvtDsc_t mmp_smINoNetrspInd ;  // DMX add 20140625, PDP active req no respond

/*
** Interface with RLC
*/
extern InEvtDsc_t mmp_rlcITReadyInd;
extern InEvtDsc_t mmp_rlcISynchInd;

/*
** Interface with API
*/
extern InEvtDsc_t mmp_apiIAttachReq;
extern InEvtDsc_t mmp_apiIPsDetachReq;
#endif

/*
**  MM Internal event
*/
extern InEvtDsc_t T3210Exp;
extern InEvtDsc_t T3211Exp;
extern InEvtDsc_t T3212Exp;
extern InEvtDsc_t T3213Exp;
extern InEvtDsc_t T3230Exp;
extern InEvtDsc_t TWNKCExp;
extern InEvtDsc_t TDetachExp;

#ifdef __GPRS__
extern InEvtDsc_t T3302Exp;   
extern InEvtDsc_t T3310Exp;
extern InEvtDsc_t T3311Exp;   
extern InEvtDsc_t T3312Exp;
extern InEvtDsc_t T3321Exp;
extern InEvtDsc_t T3330Exp;
#endif

/*
** Derived Events
*/
extern InEvtDsc_t mmc_AuthReqEvt       ;
extern InEvtDsc_t mmc_AuthRejEvt       ;
extern InEvtDsc_t mmc_IdentityReqEvt   ;
extern InEvtDsc_t mmc_LocUpAccEvt      ;
extern InEvtDsc_t mmc_LocUpRejEvt      ;
extern InEvtDsc_t mmc_CmServAccEvt     ;  
extern InEvtDsc_t mmc_CmServRejEvt     ;  
extern InEvtDsc_t mmc_TmsiReallocCmdEvt;
extern InEvtDsc_t mmc_AbortEvt         ;
extern InEvtDsc_t mmc_HdleOtherPd      ;
extern InEvtDsc_t mmc_InformationEvt   ;

#ifdef __GPRS__
extern InEvtDsc_t mmp_AttachAccEvt ;
extern InEvtDsc_t mmp_AttachRejEvt ;
extern InEvtDsc_t mmp_IdReqEvt     ;
extern InEvtDsc_t mmp_DetachReqEvt;
extern InEvtDsc_t mmp_HdleOtherPd  ;
extern InEvtDsc_t mmp_AuthAndCiphReqEvt;
extern InEvtDsc_t mmp_AuthAndCiphRejEvt;
extern InEvtDsc_t mmp_RauAccEvt;
extern InEvtDsc_t mmp_RauRejEvt;
extern InEvtDsc_t mmp_TmsiReallocCmdEvt;
extern InEvtDsc_t mmp_DetachAccEvt;
extern InEvtDsc_t mmp_DetachReqEvt;
extern InEvtDsc_t mmp_InformationEvt;
extern InEvtDsc_t mmp_GmmStatus;

extern InEvtDsc_t mmp_GprsTestModeCmdEvt;

/*added by yangyang for EDGE at 2007-03-30*/
#ifdef __EGPRS__
extern InEvtDsc_t mmp_EGprsTestModeCmdEvt; 
extern InEvtDsc_t mmp_rrIAccTechTypeInd;
#endif
/*added by yangyang for EDGE at 2007-03-30*/


#endif

InEvtDsc_t * mm_GiveDsc ( u32 MsgId );

// Routage function for the DATA_IND
InEvtDsc_t * mmc_GiveDataIndDsc( void );
InEvtDsc_t * mmp_GiveDataIndDsc( void );


/*
==========================================
             Outgoing Events
==========================================
*/

/*
** Interface with RR
*/
extern OutEvtDsc_t mm_rrOConnectReq;
extern OutEvtDsc_t mm_rrOCellRsp   ;
extern OutEvtDsc_t mm_rrODataReq   ;
extern OutEvtDsc_t mm_rrOAbortReq  ;
extern OutEvtDsc_t mm_rrOLocUpSta  ;
extern OutEvtDsc_t mm_rrOKcInd     ;
extern OutEvtDsc_t mm_rrStartReq   ;   // DMX MOD, 20140721, the STK_START_REQ to RR will forward by MM
extern OutEvtDsc_t mm_rrSimOpenInd  ;   // DMX MOD, 20140721, the STK_SIM_OPEN_IND to RR will forward by MM


/*
** Interface with CM
*/
extern OutEvtDsc_t mm_cmOReleaseInd;
extern OutEvtDsc_t mm_cmOEstabCnf  ;
extern OutEvtDsc_t mm_cmOEstabErr ;
extern OutEvtDsc_t mm_cmOReestabCnf;
extern OutEvtDsc_t mm_cmOReestabErr;
extern OutEvtDsc_t mm_cmOServReqAck;
extern OutEvtDsc_t mm_cmODataInd   ;
extern OutEvtDsc_t mm_cmOPageBeginInd; //__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

/*
** Interface with SIM
*/
extern OutEvtDsc_t mm_simOUmtsAlgoReq;
extern OutEvtDsc_t mm_simOGsmAlgoReq;
extern OutEvtDsc_t mm_simOReadBinReq;
extern OutEvtDsc_t mm_simOUpdBinReq ;
extern OutEvtDsc_t mm_simOReadFileStatusReq;
extern OutEvtDsc_t mm_simOStopReq;
extern OutEvtDsc_t mm_simOErrInd;

/*
** Interface with API
*/
extern OutEvtDsc_t mm_apiOStopCnf;
extern OutEvtDsc_t mm_apiONwkInfoInd;

extern OutEvtDsc_t mm_apiOSimColseCnf;

/*
** Intra-face ;-)
*/
extern OutEvtDsc_t mm_OEstabReq   ;
extern OutEvtDsc_t mmc_OSelfLocUp ;



#ifdef __GPRS__
/*
** Interface with LLC
*/
extern OutEvtDsc_t mmp_llcOTlliInd;
extern OutEvtDsc_t mmp_llcOCiphParamInd;
extern OutEvtDsc_t mmp_llcOUnitDataReq;
extern OutEvtDsc_t mmp_llcOTriggerReq;
extern OutEvtDsc_t mmp_llcOSuspendReq;
extern OutEvtDsc_t mmp_llcOResumeReq;

/*
** Interface with L3 (SM or SMS)
*/
extern OutEvtDsc_t mmp_l3ODataInd;
extern OutEvtDsc_t mmp_smOAbortInd;
extern OutEvtDsc_t mmp_smORauAccPdpStaInd; // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__

/*
** Interface with SNDCP
*/
extern OutEvtDsc_t mmp_snOSequenceInd;


/*
** Interface with MAC
*/
extern OutEvtDsc_t mmp_macONewTlliInd;


/*
** Interface with RR
*/
extern OutEvtDsc_t mmp_rrORaUpSta;
extern OutEvtDsc_t mmp_rrOMmpProcInd;
extern OutEvtDsc_t mmp_rrOGprsReq;
extern OutEvtDsc_t mmp_rrOMmpReadyStInd;

/*
** Interface with RLC
*/
extern OutEvtDsc_t mmp_rluTestModeReq;
extern OutEvtDsc_t mmp_rluTReadyReq;
#ifdef __EGPRS__
extern OutEvtDsc_t mmp_rluEgprsTestModeReq; //added start by yangyang for EDGE at 2007-04-04
#endif
/*
** Intra-face ;-)
*/
//extern OutEvtDsc_t mmp_OSelfAttach;


#endif // __GPRS__




#endif

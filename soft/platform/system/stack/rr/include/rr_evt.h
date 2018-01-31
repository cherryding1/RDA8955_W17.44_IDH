/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : rr_evt.h  
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events od rr Task               

  History    :
--------------------------------------------------------------------------------
  Mar 17 06  |  MCE   | Suppression of NonDrxPerExpEvt. NonDrxTimExpEvt renamed 
             |        | NonDrxExpEvt
  Jan 17 06  |  MCE   | Addidition of rr_IPCCOIndEvt and rr_OPCCOResultEvt 
  Nov 14 05  |  MCE   | Created RR_RRI_TRC and RR_RRD_TRC flags
  Nov 11 05  |  MCE   | Created RR_IL flag
  Sep 27 05  |  MCE   | Removed rr_NasUpdateIndEvt
  Apr 11 05  |  MCE   | Addition of rr_rrEndCongIndEvt    
  Mar 31 05  |  MCE   | Addition of rr_l1CbchReqEvt and rr_CbchReqEvt 
  Mar 10 05  |  MCE   | Addition of rr_IPlmnNasUpdEvt and rr_OPlmnNasUpdEvt
  Sep 22 04  |  MCE   | Addition of rr_rrGprsReqEvt
  Sep 07 04  |  MCE   | Addition of rr_MmReadyStInd                       
  Aug 18 04  |  MCE   | Addition of rr_FrqRdfEvt, rr_l1FrqRdfReqEvt and 
             |        | rrd_l1FrqRdfCnfEvt            
  Aug 12 04  |  MCE   | Addition of rr_l1RachUpdReqEvt  
  Aug 08 04  |  MCE   | Suppression of rr_IAutoConnectReqEvt
  Jun 22 04  |  MCE   | Addition of rr_rrMmpProcIndEvt
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RR_EVT_H__
#define __RR_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition




// FIFO ADD
/*
** Flag Usage definition
*/
#define RR_RRD      (1<<0)   // RRD FSM           
#define RR_RRI_C    (1<<1)   // RRI Cell Selec FSM
#define RR_RRI_P    (1<<2)   // RRI Plmn Selec FSM
#define RR_KEEP     (1<<3)   // Msg is not to Delete
#define RR_CODEC    (1<<4)   // Msg is to Decode 
#define RR_ARFC     (1<<5)   // Trace Arfcn
#define RR_PRIO     (1<<6)   // Send with Priority ON
#define RR_RM       (1<<7)   // Radio Message
#define RR_IL       (1<<8)   
#define RR_RRI_TRC  (1<<9)   
#define RR_RRD_TRC  (1<<10)  

#define RR_FSMFLGS   ( RR_RRD | RR_RRI_C | RR_RRI_P )

/*
==========================================
             Incoming Events
==========================================
*/

/*
**  Incoming RRI <-- RRI Events
*/
extern InEvtDsc_t   rr_IPlmnSearchReqEvt    ;   
extern InEvtDsc_t   rr_IPlmnAcceptIndEvt    ;   
extern InEvtDsc_t   rr_IPlmnRejectIndEvt    ;   
extern InEvtDsc_t   rr_IPlmnScanReqEvt      ;     
extern InEvtDsc_t   rr_IPlmnStopScanReqEvt  ; 
extern InEvtDsc_t   rr_IPlmnFoundIndEvt     ;    
extern InEvtDsc_t   rr_IPlmnNotFoundIndEvt  ; 
extern InEvtDsc_t   rr_IPlmnLostIndEvt      ;     
extern InEvtDsc_t   rr_IPlmnAvailableIndEvt ;
extern InEvtDsc_t   rr_IPlmnListDoneIndEvt  ; 
extern InEvtDsc_t   rr_IPlmnInitReqEvt      ; 
extern InEvtDsc_t   rr_IPlmnNasUpdEvt       ;
extern InEvtDsc_t   rr_IPlmnReFreshReqEvt   ;   

// Rri Internal Events
extern InEvtDsc_t   rr_IBaCompleteEvt    ;
extern InEvtDsc_t   rr_ILookBestNearEvt  ;
extern InEvtDsc_t   rr_RecntRslExpEvt    ;
extern InEvtDsc_t   rr_ForwardPagExpEvt  ;
extern InEvtDsc_t   rr_GuardRslExpEvt    ;
extern InEvtDsc_t   rr_PnltyRslExpEvt    ;
extern InEvtDsc_t   rr_NoRetRslExpEvt    ;
extern InEvtDsc_t   rr_AsleepExpEvt      ;
extern InEvtDsc_t   rr_FreshSiExpEvt     ;
extern InEvtDsc_t   rr_MonitIndExpEvt    ;
#ifdef __GPRS__
extern InEvtDsc_t   rr_NonDrxExpEvt      ;
#endif
extern InEvtDsc_t   rr_IPlmnSimLoadedEvt ;
extern InEvtDsc_t   rr_NSvcTimExpEvt     ;
extern InEvtDsc_t   rr_LtdSvcTimExpEvt   ;
extern InEvtDsc_t   rr_HPlmnTimExpEvt    ;
#ifdef __LOCATION_LOCK__
extern InEvtDsc_t   rr_LocationLockTimExpEvt;
#endif
extern InEvtDsc_t    rr_SendAdjCReportTimExpEvt;

// Rrd Internal Events

extern InEvtDsc_t   rr_rrICellIndEvt        ;
extern InEvtDsc_t   rr_IL3DataIndEvt        ;
extern InEvtDsc_t   rr_T3122ExpEvt          ;
extern InEvtDsc_t   rr_T3126ExpEvt          ;
extern InEvtDsc_t   rr_T3124ExpEvt          ;

/*
**  Incoming RRI <-- RRD Events
*/
extern InEvtDsc_t   rr_IReselReqEvt         ; 
extern InEvtDsc_t   rr_Il1ConIndEvt         ; 
extern InEvtDsc_t   rr_Il1NotConIndEvt      ; 
extern InEvtDsc_t   rr_ICellChangeIndEvt    ;
extern InEvtDsc_t   rr_IPCCOIndEvt          ;
extern InEvtDsc_t   rr_IPCCOCompleteEvt     ;

/*
**  Incoming RRD <-- RRI Events
*/
extern InEvtDsc_t   rr_IReselGrantReqEvt    ; 
extern InEvtDsc_t   rr_ISendMeasReqEvt      ;
extern InEvtDsc_t   rr_IPCCOResultEvt       ;

/*
**  Incoming RRI <-- L1 Events
*/
extern InEvtDsc_t   rr_l1PowerRspEvt      ;
extern InEvtDsc_t   rr_l1SynchFoundEvt    ;
extern InEvtDsc_t   rr_l1SynchNotFoundEvt ;
extern InEvtDsc_t   rr_l1BcchDataIndEvt   ;
extern InEvtDsc_t   rr_l1PchDataIndEvt    ;
extern InEvtDsc_t   rr_l1MonitorIndEvt    ; 
extern InEvtDsc_t   rr_l1NcMonitorIndEvt  ; 

/*
**  Incoming RRD <-- L1 Events
*/
extern InEvtDsc_t   rr_l1RachIndEvt        ;
extern InEvtDsc_t   rr_l1ConnectCnfEvt     ;
extern InEvtDsc_t   rr_l1FrqRdfCnfEvt      ;

/*
** RR <-- MM Events 
*/
extern InEvtDsc_t   rr_rrConnectReqEvt     ;
extern InEvtDsc_t   rr_rrCellRspEvt        ;
extern InEvtDsc_t   rr_rrLocUpStaEvt       ;
extern InEvtDsc_t   rr_rrDataReqEvt        ;
extern InEvtDsc_t   rr_rrKcIndEvt          ;
extern InEvtDsc_t   rr_rrAbortReqEvt       ;
#ifdef __GPRS__
extern InEvtDsc_t   rr_rrRaUpdStaEvt       ;
extern InEvtDsc_t   rr_rrMmpProcIndEvt     ;
extern InEvtDsc_t   rr_rrMmReadyStIndEvt   ;
extern InEvtDsc_t   rr_rrGprsReqEvt        ;
#endif

/*
** RR <-- LAPDm Events
*/
extern InEvtDsc_t  rr_lapEstablishCnfEvt        ;
extern InEvtDsc_t  rr_lapEstablishFailureIndEvt ;
extern InEvtDsc_t  rr_lapReleaseIndEvt          ;
extern InEvtDsc_t  rr_lapResumeCnfEvt           ;
extern InEvtDsc_t  rr_lapResumeFailureIndEvt    ;
extern InEvtDsc_t  rr_lapReconnectCnfEvt        ;
extern InEvtDsc_t  rr_lapReconnectFailureIndEvt ;
extern InEvtDsc_t  rr_lapDataIndEvt             ;
extern InEvtDsc_t  rr_lapErrorIndEvt            ;
extern InEvtDsc_t  rr_lapEstablishIndEvt        ;

/*
** Derived Events from LAPDm
*/
extern InEvtDsc_t   rr_ImmAssCmdEvt        ;
extern InEvtDsc_t   rr_ImmAssExtCmdEvt     ;
extern InEvtDsc_t   rr_ImmAssRejEvt        ;
extern InEvtDsc_t   rr_AssCmdEvt           ;
extern InEvtDsc_t   rr_HovCmdEvt           ;
extern InEvtDsc_t   rr_FrqRdfEvt           ;
extern InEvtDsc_t   rr_ChanRelEvt          ;
extern InEvtDsc_t   rr_CipModCmdEvt        ;
extern InEvtDsc_t   rr_CmkEnqEvt           ;
extern InEvtDsc_t   rr_ChnModEvt           ;
extern InEvtDsc_t   rr_SysInf5Evt          ;
extern InEvtDsc_t   rr_SysInf5BEvt         ;
extern InEvtDsc_t   rr_SysInf5TEvt         ;
extern InEvtDsc_t   rr_SysInf6Evt          ;
extern InEvtDsc_t   rr_ExtMeasOrdEvt       ;
extern InEvtDsc_t   rr_PhyInfoEvt          ;
extern InEvtDsc_t   rr_ClsTchLoopEvt       ;
extern InEvtDsc_t   rr_OpnTchLoopEvt       ;
extern InEvtDsc_t   rr_DaiTestEvt          ;
extern InEvtDsc_t   rr_StatusEvt           ;

/*
** Derived Events from L1
*/
extern InEvtDsc_t   rrd_l1ErrorIndEvt     ;
extern InEvtDsc_t   rri_l1ErrorIndEvt     ;

#ifdef __GPRS__
#ifdef __RRP__
extern InEvtDsc_t   mac_L1PDchIndEvt       ;
extern InEvtDsc_t   mac_L1PBcchIndEvt      ;
extern InEvtDsc_t   mac_L1ChnQualIndEvt    ;
extern InEvtDsc_t   rrp_L1USFInd           ;
extern InEvtDsc_t   rrp_L1TBFCfgCnf        ;
extern InEvtDsc_t   rrp_L1PSDataInd        ;
#ifdef __EGPRS__
extern InEvtDsc_t rrp_L1IRBufferStatusEvt		   ;
#endif
extern InEvtDsc_t   mac_RrIDataReq         ;
extern InEvtDsc_t   rr_MacCtrlDataInd      ;
extern InEvtDsc_t   rr_MacPUlAckNackInd    ;
extern InEvtDsc_t   rr_MacPQueNotif        ;
extern InEvtDsc_t   rr_MacContResFail      ;
extern InEvtDsc_t   rr_MacAccRejInd        ;
extern InEvtDsc_t   rr_MacEstabReq         ;
extern InEvtDsc_t   mac_UlIDataReq         ;
extern InEvtDsc_t   mac_IDlAckReq          ;
extern InEvtDsc_t   mac_IPurgeDQReq        ;
extern InEvtDsc_t   rrp_RlcConnectReqEvt   ;
extern InEvtDsc_t   rrp_RlcReleaseReqEvt   ;
extern InEvtDsc_t   mac_NewTlliIndEvt      ;
extern InEvtDsc_t   mac_NewTlliCnfEvt      ;
extern InEvtDsc_t   mac_T3166Exp           ;
extern InEvtDsc_t   mac_T3200AExp          ;
extern InEvtDsc_t   mac_T3200BExp          ;
extern InEvtDsc_t   mac_T3200CExp          ;
extern InEvtDsc_t   mac_T3200DExp          ;
extern InEvtDsc_t   mac_T3200EExp          ;
extern InEvtDsc_t   mac_T3200FExp          ;
extern InEvtDsc_t   mac_T3200GExp          ;
extern InEvtDsc_t   mac_T3200HExp          ;
extern InEvtDsc_t   rrp_T3142Exp           ;
extern InEvtDsc_t   rrp_T3146Exp           ;
extern InEvtDsc_t   rrp_T3162Exp           ;
extern InEvtDsc_t   rrp_T3164Exp           ;
extern InEvtDsc_t   rrp_T3168Exp           ;
extern InEvtDsc_t   rrp_T3170Exp           ;
extern InEvtDsc_t   rrp_T3172Exp           ;
extern InEvtDsc_t   rrp_T3174Exp           ;
extern InEvtDsc_t   rrp_T3176Exp           ;
extern InEvtDsc_t   rrp_T3180Exp           ;
extern InEvtDsc_t   rrp_T3184Exp           ;
extern InEvtDsc_t   rrp_T3186Exp           ;
extern InEvtDsc_t   rrp_T3188Exp           ;
extern InEvtDsc_t   rrp_T3190Exp           ;
extern InEvtDsc_t   rrp_T3192Exp           ;
extern InEvtDsc_t   rrp_SegImmAssExp       ;
extern InEvtDsc_t   rrp_IImmAssCmplt       ;
extern InEvtDsc_t   rrp_PUlAssEvt          ;
extern InEvtDsc_t   rrp_PDlAssEvt          ;
extern InEvtDsc_t   rrp_PTSReconfEvt       ;
extern InEvtDsc_t   rrp_PPDCHRelEvt        ;
extern InEvtDsc_t   rrp_PPwrCtrlTAEvt      ;
extern InEvtDsc_t   rrp_PTBFRelEvt         ;
extern InEvtDsc_t   rrp_PCCOEvt            ;
extern InEvtDsc_t   rrp_L1EndFixedInd      ;
extern InEvtDsc_t   rrp_UlRelDelayEvt      ;
extern InEvtDsc_t   rrp_DlRelDelayEvt      ;
extern InEvtDsc_t   rrp_SglDlExp           ;
extern InEvtDsc_t   rrp_SglUlExp           ;
#ifndef STK_SINGLE_SIM
extern InEvtDsc_t   rr_ConfliConnectReqEvt;
#endif
#endif

/*
** Derived events from MAC_CTRLDATA_IND
*/
extern InEvtDsc_t   rr_PsiEvt              ;
extern InEvtDsc_t   rr_PmoEvt              ;


#endif

/*
** RR <-- SIM  Events
*/
extern InEvtDsc_t   rr_simReadBinaryCnfEvt ;
extern InEvtDsc_t   rr_simFileStatusCnfEvt ;





extern InEvtDsc_t   rr_CbchReqEvt          ;

/*
** Interface with API
*/
extern InEvtDsc_t   rr_apiSimOpenIndEvt    ;
extern InEvtDsc_t   rr_apiSimClosedIndEvt  ;
extern InEvtDsc_t   rr_apiStartReqEvt      ;
extern InEvtDsc_t   rr_apiStopReqEvt       ;

extern InEvtDsc_t   rr_rrReinitReqEvt      ;

extern InEvtDsc_t   rr_apiNwSelModeReqEvt   ;
extern InEvtDsc_t   rr_apiNwListReqEvt      ;
extern InEvtDsc_t   rr_apiNwListAbortReqEvt ;
extern InEvtDsc_t   rr_apiNwSelReqEvt       ;
extern InEvtDsc_t   rr_apiNwReselReqEvt     ;
extern InEvtDsc_t   rr_apiQualReportReqEvt  ;
extern InEvtDsc_t   rr_apiCurCellInfoReqEvt ;
extern InEvtDsc_t   rr_apiAdjCellInfoReqEvt ;
extern InEvtDsc_t   rr_apiSyncInfoReqEvt;
extern InEvtDsc_t   rr_apiPowerScanReqEvt;
extern InEvtDsc_t   rr_apiDetectMBSReqEvt;
#ifdef __GPRS__
extern InEvtDsc_t   rr_apiNmoCfgReqEvt      ;
#endif

/*
==========================================
             Outgoing Events
==========================================
*/

/*
**  Outgoing  RRI --> RRI Events
*/
extern OutEvtDsc_t   rr_OPlmnSearchReqEvt    ;   
extern OutEvtDsc_t   rr_OPlmnAcceptIndEvt    ;   
extern OutEvtDsc_t   rr_OPlmnRejectIndEvt    ;   
extern OutEvtDsc_t   rr_OPlmnScanReqEvt      ;     
extern OutEvtDsc_t   rr_OPlmnStopScanReqEvt  ; 
extern OutEvtDsc_t   rr_OPlmnFoundIndEvt     ;    
extern OutEvtDsc_t   rr_OPlmnNotFoundIndEvt  ; 
extern OutEvtDsc_t   rr_OPlmnLostIndEvt      ;     
extern OutEvtDsc_t   rr_OPlmnAvailableIndEvt ;
extern OutEvtDsc_t   rr_OPlmnListDoneIndEvt  ; 
extern OutEvtDsc_t   rr_OPlmnInitReqEvt      ; 
extern OutEvtDsc_t   rr_OPlmnNasUpdEvt       ; 
extern OutEvtDsc_t   rr_OPlmnReFreshReqEvt   ;   

// Rri Internal Events
extern OutEvtDsc_t   rr_OBaCompleteEvt       ;
extern OutEvtDsc_t   rr_OLookBestNearEvt     ;
extern OutEvtDsc_t   rr_OPlmnSimLoadedEvt    ;

/*
**  Outgoing  RRI --> RRD Events
*/
extern OutEvtDsc_t   rr_OReselGrantReqEvt    ;
extern OutEvtDsc_t   rr_OPCCOResultEvt       ;

/*
**  Outgoing  RRD --> RRI Events
*/
extern OutEvtDsc_t   rr_OReselReqEvt         ;
extern OutEvtDsc_t   rr_OConnectedIndEvt     ; 
extern OutEvtDsc_t   rr_ONotConnectedIndEvt  ; 
extern OutEvtDsc_t   rr_OCellChangeIndEvt    ;
extern OutEvtDsc_t   rr_OPCCOIndEvt          ;
extern OutEvtDsc_t   rr_OPCCOCompleteEvt     ;

/*
**  Outgoing  RRD --> RRD Events
*/
extern OutEvtDsc_t   rr_ORrConnectReqEvt     ;

/*
**  Outgoing  RRI --> L1 Events
*/
extern OutEvtDsc_t  rr_l1InitReqEvt        ;
extern OutEvtDsc_t  rr_l1PowerReqEvt       ;
extern OutEvtDsc_t  rr_l1SynchReqEvt       ;
extern OutEvtDsc_t  rr_l1SynchUpdateReqEvt ;
extern OutEvtDsc_t  rr_l1StopSynchReqEvt   ;
extern OutEvtDsc_t  rr_l1CchReqEvt         ;
extern OutEvtDsc_t  rr_l1BcchReqEvt        ;
extern OutEvtDsc_t  rr_l1BcchModeReqEvt;
extern OutEvtDsc_t  rr_l1PBcchReqEvt       ;
extern OutEvtDsc_t  rr_l1MonitorReqEvt     ;
extern OutEvtDsc_t  rr_l1ExtdMonitorReqEvt ;
extern OutEvtDsc_t  rr_l1NcMonitorReqEvt   ;
extern OutEvtDsc_t  rr_l1PMeasCtrlReqEvt   ;
#ifdef __GPRS__
extern OutEvtDsc_t  rr_l1PccchReqEvt      ;
#endif
extern OutEvtDsc_t  rr_l1CbchReqEvt        ;
extern OutEvtDsc_t  l1_HighBandChangeReq;
extern OutEvtDsc_t  rr_l1AdjCellReportReqEvt;
/*
**  Outgoing  RRD --> L1 Events
*/
extern OutEvtDsc_t  rr_l1RachOnReqEvt     ;
extern OutEvtDsc_t  rr_l1RachOffReqEvt    ;
extern OutEvtDsc_t  rr_l1ConnectReqEvt    ;
extern OutEvtDsc_t  rr_l1SuspendReqEvt    ;
extern OutEvtDsc_t  rr_l1ReleaseReqEvt    ;
extern OutEvtDsc_t  rr_l1CiphCfgReqEvt    ;
extern OutEvtDsc_t  rr_l1ChnModEvt        ;
extern OutEvtDsc_t  rr_l1PhyInfoReqEvt    ;
extern OutEvtDsc_t  rr_l1CellOptUpdEvt    ;
extern OutEvtDsc_t  rr_l1SetTstLoopEvt    ;
extern OutEvtDsc_t  rr_l1StopChReqEvt     ;
extern OutEvtDsc_t  rr_l1RachUpdReqEvt    ;
extern OutEvtDsc_t  rr_l1FrqRdfReqEvt      ;

/*
**  Outgoing  RR --> MM Events
*/
extern OutEvtDsc_t  rr_rrReleaseIndEvt    ;
extern OutEvtDsc_t  rr_rrConnectCnfEvt    ;
extern OutEvtDsc_t  rr_rrOCellIndEvt      ;
extern OutEvtDsc_t  rr_rrDataIndEvt       ;
extern OutEvtDsc_t  rr_rrPagingEvt        ;
extern OutEvtDsc_t  rr_rrCiphIndEvt       ;
extern OutEvtDsc_t  rr_rrCmkIndEvt        ;
extern OutEvtDsc_t  rr_rrEndCongIndEvt    ;

/*
** Outgoing RR--> LAPDm Events
*/
extern OutEvtDsc_t  rr_lapEstablishReqEvt ;
extern OutEvtDsc_t  rr_lapNReleaseReqEvt  ;
extern OutEvtDsc_t  rr_lapLReleaseReqEvt  ;
extern OutEvtDsc_t  rr_lapSuspendReqEvt   ;
extern OutEvtDsc_t  rr_lapResumeReqEvt    ;
extern OutEvtDsc_t  rr_lapReconnectReqEvt ;
extern OutEvtDsc_t  rr_lapDataReqEvt      ;
extern OutEvtDsc_t  rr_lapUnitDataReqEvt  ;

/*
** Outgoing RR--> CC events
*/
extern OutEvtDsc_t  rr_rrChanModeIndEvt   ;

/*
** Outgoing RR--> SIM Events
*/
extern OutEvtDsc_t  rr_SimReadBinReqEvt      ;
extern OutEvtDsc_t  rr_SimFileStatusReqEvt   ;
extern OutEvtDsc_t  rr_SimUpdateBinReqEvt    ;
extern OutEvtDsc_t  rr_SimReadRecordReqEvt      ;

/*
** Outgoing RR--> API Events
*/
extern OutEvtDsc_t   rr_apiNwListIndEvt      ;
extern OutEvtDsc_t   rr_apiNwListAbortIndEvt ;
extern OutEvtDsc_t   rr_apiNwCsRegStaIndEvt  ;
extern OutEvtDsc_t   rr_apiNwPsRegStaIndEvt  ;
extern OutEvtDsc_t   rr_apiNwStoreListInd;
extern OutEvtDsc_t   rr_apiQualReportCnfEvt  ;
extern OutEvtDsc_t   rr_apiCurCReportIndEvt  ;
extern OutEvtDsc_t   rr_apiAdjCReportIndEvt  ;
extern OutEvtDsc_t   rr_apiStopCnfEvt        ;
extern OutEvtDsc_t   rr_apiPowerScanRspEvt;

extern OutEvtDsc_t   rr_apiCurCellInfoIndEvt ;
extern OutEvtDsc_t   rr_apiAdjCellInfoIndEvt ;
extern OutEvtDsc_t   rr_apiPowerListIndEvt;
extern OutEvtDsc_t   rr_apiSyncInfoIndEvt;
/*
** Outgoing RR -> MAC Events 
*/
extern OutEvtDsc_t mac_rrODataReq;

#ifdef __RRP__
/*
** Outgoing MAC -> L1 Events 
*/
extern OutEvtDsc_t mac_l1PSDataReq ;
extern OutEvtDsc_t mac_l1PurgeDQReq ;
#ifdef __EGPRS__
extern OutEvtDsc_t mac_EgprsLoopbackSwiReq;
#endif

/*
** Outgoing RRP -> L1 Events 
*/
extern OutEvtDsc_t rrp_l1PPDCHRel;
extern OutEvtDsc_t rrp_l1PPwrCtrlTAReq;
extern OutEvtDsc_t rrp_l1TBFRelReq;
extern OutEvtDsc_t rrp_l1TBFCfgReq;
extern OutEvtDsc_t rrp_l1MacFixedUpdtReq;

/*
** Outgoing MAC -> RRP Events 
*/
extern OutEvtDsc_t mac_CtrlDataInd ;
extern OutEvtDsc_t mac_PUlAckNackInd;
extern OutEvtDsc_t mac_PQueNotif;
extern OutEvtDsc_t mac_ContResFail;
extern OutEvtDsc_t mac_AccRejInd;
extern OutEvtDsc_t mac_EstabReq;

/*
** Outgoing MAC -> RLC Events 
*/
extern OutEvtDsc_t mac_DlDataInd ;
extern OutEvtDsc_t mac_UlAckInd;
extern OutEvtDsc_t mac_OTlliInd;
extern OutEvtDsc_t mac_ContResCnf;
extern OutEvtDsc_t mac_UlSuspReq;
extern OutEvtDsc_t mac_UlResReq;

/*
** Outgoing MAC -> MMP Events 
*/
extern OutEvtDsc_t rrp_PagingInd;

/*
** Outgoing RRP -> RRP Events 
*/
extern OutEvtDsc_t rrp_OImmAssCmplt;

/*
** Outgoing RRP -> RLC Events 
*/
extern OutEvtDsc_t rrp_DlConnectInd;
extern OutEvtDsc_t rrp_UlConnectCnf;
extern OutEvtDsc_t rrp_DlRelInd;
extern OutEvtDsc_t rrp_UlRelStartReq;
extern OutEvtDsc_t rrp_UlRelInd;
extern OutEvtDsc_t rrp_CellOptInd;
#ifdef __EGPRS__
extern OutEvtDsc_t rrp_EgprsLoopbackCmd;
#endif

/*
** Outgoing RRP -> MMP Events 
*/
extern OutEvtDsc_t rrp_ReleaseInd;
extern OutEvtDsc_t rr_SuspendReq;
#ifdef __EGPRS__
extern OutEvtDsc_t rr_AccTechTypeIndEvt;
#endif
#endif // __RRP__

#endif

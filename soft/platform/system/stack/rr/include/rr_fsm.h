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
  File       : rr_fsm.h  
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for rr             

  History    :
--------------------------------------------------------------------------------
  Jun 26 06  |  MCE   | rri_TrsnNotConInd split into rriCCamped_TrsnNotConInd and 
             |        | rriCTrans_TrsnNotConInd
  Mar 20 06  |  MCE   | added rriCResel_TrsnNotConInd 
  Jan 17 06  |  MCE   | Addition of rri_TrsnPcco
  Oct 12 05  |  MCE   | Addition of rri_TrsnRaUpdStaConn
  Apr 07 05  |  MCE   | Addition of rriCGene_TrsnCbchReq
  Aug 18 04  |  MCE   | Addition of rrgCond_TrsFrqRdf and rrgCond_TrsL1FrqRdfCnf
  Aug 09 04  |  MCE   | Suppression of RRD_WRSI state
  Jun 23 04  |  MCE   | Addition of RRI_CTRANSF state
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/

#ifndef __RR_FSM_H__
#define __RR_FSM_H__

/*
** Save message and save event macros 
*/
#define RR_SAVE_MSG(_Fsm,_Msg)     fsm_Save (_Fsm, TRUE, _Msg)
#define RR_SAVE_EVT(_Fsm,_Evt)     fsm_Save (_Fsm, FALSE, _Evt)


/* 
** rr (Common) Transitions prototypes
*/
u8     rr_TrsnSaveMsg     (void);
u8     rr_TrsnSaveEvt     (void);
u8     rr_TrsnEmpty       (void);
void   rr_DispatchCon     (void);
void   rr_DispatchCellRsp (void);

/*
===================
  RRI definitions
===================
*/

/*
** RRI Cell selection States definitions
** FSM rri_C
*/
#define  RRI_CGENE    (0             )
#define  RRI_CNULL    (1+ FSM_STABLE )
#define  RRI_CRSSI    (2             )
#define  RRI_CSYNCHR  (3             )
#define  RRI_CSVERIF  (4             )
#define  RRI_CRVERIF  (5             )
#define  RRI_CCAMPED  (6+ FSM_STABLE )
#define  RRI_CRESEL   (7             )
#define  RRI_CCONN    (8             )
#define  RRI_CTRANSF  (9             )
#define  RRI_CENDCO   (10            )



/* 
** rri_C Transitions prototypes
*/

u8 rri_TrsnReinit              (void);
u8 rri_TrnsGuardRslExp         (void);
u8 rri_TrsnPnltyExp            (void);
u8 rri_TrsnAsleepExp           (void);
u8 rri_TrsnNoRetExp            (void);
u8 rri_TrsnRVerifNoRetExp      (void);
u8 rriCGene_TrsnFreshSiExp     (void);
u8 rriCCamped_TrsnFreshSiExp   (void); 
u8 rri_TrsnUpdGprsWanted       (void);
u8 rri_TrsnGprsReq             (void);
#ifdef __GPRS__
u8 rriCGene_TrsnMmReadyStInd   (void);
u8 rriCGene_TrsnMmpProcInd     (void);
u8 rriCCamped_TrsnMmpProcInd   (void);
u8 rriCGene_TrsnNonDrxExp      (void);
u8 rriCCamped_TrsnNonDrxExp    (void);
u8 rriCCamped_TrsnPsi1         (void);
u8 rriCResel_TrsnNonDrxExp (void);
u8 rri_TrsnNmoCfg              (void);
u8 rri_TrsnPmo                 (void);
u8 rri_TrsnPccoInd             (void);
u8 rri_TrsnPccoComplete        (void);
#endif

u8 rri_TrsnPlmnSearch          (void);
u8 rri_TrsnPlmnScan            (void);
u8 rri_TrsnPlmnChge            (void);
u8 rri_TrsnPlmnUpdte           (void);
u8 rri_TrsnPlmnAccept          (void);
u8 rri_TrsnPlmnReFresh         (void);
u8 rri_TrsnPowerScan_Null(void);
u8 rri_TrsnPowerRsp_Null(void);

u8 rri_TrsnStartSelection      (void);
u8 rri_TrsnStartScan           (void);
u8 rri_TrsnStopScan            (void);

u8 rri_TrsnSynchrFound         (void);
u8 rri_TrsnDoInitSearch        (void);
u8 rri_TrsnDoSearch            (void);
u8 rri_TrsnLookSysInfo         (void);
u8 rri_TrsnForwardPaging       (void);
u8 rri_TrsnSavePaging          (void);
u8 rri_TrsnSelecVerify         (void);
u8 rri_TrsnReselVerify         (void);
u8 rri_TrsnEndCoVerify         (void);
u8 rri_TrsnSaveMonitor         (void);
u8 rri_TrsnIdleMonitor         (void);
u8 rri_TrsnConnMonitor         (void);
u8 rri_TrsnNcMonitor           (void);
u8 rri_TrsnSynchNeighFound     (void);
u8 rri_TrsnSynchNeighNotFound  (void);
u8 rriCResel_TrsnSynchNeighNotFound  (void);
u8 rri_TrsnL1ErrorInd          (void);

u8 rri_TrsnBaComplete          (void);
u8 rri_TrsnLookBestNear        (void);

u8 rri_TrsnReselection         (void);
u8 rri_TrsnConnectedInd        (void);
u8 rriCconn_TrsnNotConInd      (void); 
u8 rriCCamped_TrsnNotConInd    (void);
u8 rriCTransf_TrsnNotConInd    (void);
u8 rriCResel_TrsnNotConInd     (void);
u8 rri_TrsnRstConState         (void);
u8 rri_TrsnCellChangeInd       (void);
u8 rri_TrsnUnitDataInd         (void);
u8 rri_TrsnSysInf5N            (void);
u8 rri_TrsnSysInf5B            (void);
u8 rri_TrsnSysInf5T            (void);
u8 rri_TrsnExtMeasOrd          (void);
u8 rri_TrsnCellRsp             (void);
u8 rriCGene_TrsnCbchReq        (void);




void  rri_EnterResel  ( void ) ;
void  rri_EnterCamp   ( void ) ;

/*
** RRI Plmn selection States definitions
** FSM rri_P
*/
#define  RRI_PGENE   ( 0             )
#define  RRI_PNULL   ( 1             )
#define  RRI_PWCS    ( 2             )
//#define  RRI_PWMM    ( 3             )
#define  RRI_PCAMP   ( 3+ FSM_STABLE )
#define  RRI_PLTD    ( 4+ FSM_STABLE )
#define  RRI_PNSVC   ( 5+ FSM_STABLE )
#define  RRI_PCONN   ( 6             )

u8 rri_TrsnStartReq          (void);
u8 rri_TrsnStopReq           (void);
u8 rri_TrsnPSimOpenInd       (void);
u8 rri_TrsnPSimClosedInd     (void);
u8 rri_TrsnUpdtSPlmn         (void);
u8 rri_PwcsTrsnNewAvailPlmn  (void);
u8 rri_TrsnChgeSPlmn         (void);
u8 rri_TrsnSimAwake          (void);
u8 rri_TrsnPlmnEndCo         (void);
u8 rri_TrsnPlmnFound         (void);
u8 rri_TrsnPlmnNotFound      (void);
u8 rri_TrsnLocUpStaConn      (void);
u8 rri_TrsnLocUpSta          (void);
u8 rri_TrsnSearchPlmn        (void);
u8 rri_TrsnPlmnLost          (void);
u8 rri_TrsnSelMode           (void);
u8 rri_TrsnDetectMBS         (void);
u8 rri_TrsnSelReq            (void);
u8 rri_TrsnReselReq          (void);
u8 rri_TrsnNwListReq         (void);
u8 rri_TrsnNwListAbortReq    (void);
u8 rri_TrsnAPlmnListDone     (void);
u8 rri_TrsnNewAvailPlmn      (void);
u8 rri_TrsnNSvcTimExp        (void);
u8 rri_TrsnLtdSvcTimExp      (void);
u8 rri_TrsnScanHPlmn         (void);
u8 rri_TrsnPsCon             (void);
u8 rri_TrsnDenyList          (void);   
u8 rri_TrsnEmptyList         (void);   
u8 rri_TrsnRaUpdSta          (void);
u8 rri_TrsnRaUpdStaConn      (void);
u8 rri_TrsnPNULL_SimOpenInd(void);  //added by leiming 20090319
u8 rri_TrsnPNULL_SimCloseInd(void);
#ifdef __LOCATION_LOCK__
u8 rri_TranPNULL_LocationLockTim (void);
#endif
u8 rri_TrsnSendAdjCReportTimExp  (void);
u8  rri_TrsnUpdateMsRACap  ( void );

/*
===================
  RRD definitions
===================
*/


#define RRD_GENE 0
#define RRD_VOID (RRD_GENE + 1 + FSM_STABLE ) // no connection is started
#define RRD_ACCI (RRD_GENE + 2)               // RACHing and waiting for
                                              // initial channel assignment
#define RRD_WCNI (RRD_GENE + 3)               // waiting for lower layers to 
                                              // connect initial channel
#define RRD_COND (RRD_GENE + 4+ FSM_STABLE )  // connection established
#define RRD_WCNN (RRD_GENE + 5)               // waiting for estab. of the
                                              // new channel by lower layers
#define RRD_ACCN (RRD_GENE + 6)               // RACHing and waiting for new 
                                              // channel assignment
#define RRD_WRSL (RRD_GENE + 7)               // waiting for end of cell resel
                                              // to estab new cnx
#define RRD_WRLS (RRD_GENE + 8)               // waiting for release of the 
                                              // connection by lower layers
/* 
** Transitions prototypes
*/
// Common to several RRD states -> rrg_util.c
u8 rrg_TrsSysInf6             ( void ) ;
u8 rrg_TrsL3DataInd           ( void ) ;
u8 rrg_TrsLapEstablishInd     ( void ) ;


u8 rrp_TrsContResFail         ( void ) ;

// RRD_GENE
u8 rrdGene_TrsT3122Exp        ( void ) ;
u8 rrdGene_TrsAbortReq        ( void ) ;
u8 rrdGene_TrsReselGrant      ( void ) ;
u8 rrdGene_TrsConReq          ( void ) ;
#ifndef STK_SINGLE_SIM
bool rrd_JudgeConfliTrsConReq ( void* Msg );
#endif

#ifdef __RRP__
#ifndef STK_SINGLE_SIM
u8 rrdGene_ConfliTrsConReq        ( void );
#endif

u8 rrpGene_TrsCellRsp         ( void ) ;
u8 rrpGene_TrsUSFInd          ( void ) ;
u8 rrpGene_TrsT3142Exp        ( void ) ;
u8 rrpGene_TrsT3172Exp        ( void ) ;
u8 rrpGene_TrsT3174Exp        ( void ) ;
u8 rrpGene_TrsT3176Exp        ( void ) ;
u8 rrpGene_TrsConReq          ( void ) ;
#endif

// RRD_VOID
u8 rrgVoid_TrsConReq          ( void ) ;
u8 rrgVoid_TrsStartAccess     ( void ) ;
#ifdef __RRP__
u8 rrpVoid_TrsConReq          ( void ) ;
u8 rrpVoid_TrsImmAss          ( void ) ;
u8 rrpVoid_TrsImmAssCmp       ( void ) ;
u8 rrpVoid_TrsSIAExp          ( void ) ;
u8 rrpVoid_TrsPDlAss          ( void ) ;
u8 rrpVoid_TrsMacEstabReq     ( void ) ;
u8 rrpVoid_TrsPCCO            ( void ) ;
#endif
u8 rrdVoid_TrsAbortReq        ( void ) ;

// RRD_ACCI
u8 rrdAccI_TrsRachInd         ( void ) ;
u8 rrdAccI_TrsImmAss          ( void ) ;
u8 rrgAccI_TrsImmAssExt       ( void ) ;
u8 rrdAccI_TrsImmAssRej       ( void ) ;
u8 rrdAccI_TrsT3126Exp        ( void ) ;
u8 rrdAccI_TrsAbortReq        ( void ) ;
u8 rrdAccI_ReleaseTbf ( void );
u8 rrdAccI_TrsReselGrant      ( void ) ;
u8 rrgAccI_TrsT3122Exp        ( void ) ;
#ifdef __RRP__
u8 rrpAccI_TrsImmAssCmp       ( void ) ;
u8 rrpAccI_TrsSIAExp          ( void ) ;
u8 rrpAccI_TrsPUlAss          ( void ) ;
u8 rrpAccI_TrsPQueNotif       ( void ) ;
u8 rrpAccI_TrsPDlAss          ( void ) ;
u8 rrpAccI_TrsL1PSDataInd     ( void ) ;
u8 rrpAccI_TrsT3146Exp        ( void ) ;
u8 rrpAccI_TrsT3162Exp        ( void ) ;
u8 rrpAccI_TrsT3168Exp        ( void ) ;
u8 rrpAccI_TrsT3170Exp        ( void ) ;
u8 rrpAccI_TrsT3186Exp        ( void ) ;
u8 rrpAccI_TrsAccRejInd       ( void ) ;
u8 rrpAccI_TrsL1TBFCfgCnf     ( void ) ;
u8 rrpAccI_TrsRelReq          ( void ) ;
#endif

// RRD_WCNI
u8 rrgWcnI_TrsL1ConnectCnf        ( void ) ;
u8 rrgWcnI_TrsLapEstablishCnf     ( void ) ;
u8 rrgWcnI_TrsLapReconnectCnf     ( void ) ;
u8 rrgWcnI_TrsL1ErrorInd          ( void ) ;
u8 rrgWcnI_TrsLapEstablishFailInd ( void ) ;
u8 rrgWcnI_TrsLapReconnectFailInd ( void ) ;
u8 rrg_Wcni_TrsDataReq      (void);//added by fengwei 20100106 for memory leap
#ifdef __RRP__
u8 rrpWcnI_TrsPDlAss              ( void ) ;
u8 rrpWcnI_TrsPUlAss              ( void ) ;
u8 rrpWcnI_TrsL1TBFCfgCnf         ( void ) ;
u8 rrpWcnI_TrsL1PSDataInd         ( void ) ;
u8 rrpWcnI_TrsT3168Exp            ( void ) ;
#endif

//RRD_COND
u8 rrdCond_TrsL1ErrorInd      ( void ) ;
u8 rrgCond_TrsAssCmd          ( void ) ;
u8 rrgCond_TrsHovCmd          ( void ) ;
u8 rrgCond_TrsChanRel         ( void ) ;
u8 rrgCond_TrsDataReq         ( void ) ;
u8 rrgCond_TrsKcInd           ( void ) ;
u8 rrgCond_TrsCipModCmd       ( void ) ;
u8 rrgCond_TrsChnMod          ( void ) ;
u8 rrgCond_TrsLapReleaseInd   ( void ) ;
u8 rrgCond_TrsCloseTchLoop    ( void ) ;
u8 rrgCond_TrsOpenTchLoop     ( void ) ;
u8 rrgCond_TrsDaiTest         ( void ) ;
u8 rrgCond_TrsCmkEnq          ( void ) ;
u8 rrgCond_TrsLapEstablishCnf ( void ) ;
u8 rrgCond_TrsFrqRdf          ( void ) ;
u8 rrgCond_TrsL1FrqRdfCnf     ( void ) ;
#ifdef __RRP__
u8 rrpCond_TrsPTSReconf       ( void ) ;
u8 rrpCond_TrsPUlAss          ( void ) ;
u8 rrpCond_TrsPDlAss          ( void ) ;
u8 rrpCond_TrsL1TBFCfgCnf     ( void ) ;
u8 rrpCond_TrsPPDCHRel        ( void ) ;
u8 rrpCond_TrsPTBFRel         ( void ) ;
u8 rrpCond_TrsPPwrCtrlTA      ( void ) ;
u8 rrpCond_TrsT3164Exp        ( void ) ;
u8 rrpCond_TrsT3168Exp        ( void ) ;
u8 rrpCond_TrsT3180Exp        ( void ) ;
u8 rrpCond_TrsT3188Exp        ( void ) ;
u8 rrpCond_TrsT3184Exp        ( void ) ;
u8 rrpCond_TrsT3190Exp        ( void ) ;
u8 rrpCond_TrsT3192Exp        ( void ) ;
u8 rrpCond_TrsL1PSDataInd     ( void ) ;
u8 rrpCond_TrsPUlAckNackInd   ( void ) ;
u8 rrpCond_TrsAccRejInd       ( void ) ;
u8 rrpCond_TrsConReq          ( void ) ;
u8 rrpCond_TrsRelReq          ( void ) ;
u8 rrpCond_TrsEndFixedInd     ( void ) ;
u8 rrpCond_TrsUlRelDalay      ( void ) ;
u8 rrpCond_TrsDlRelDalay      ( void ) ;
u8 rrpCond_TrsPCCO            ( void ) ;
u8 rrpCond_TrsSglDlExp        ( void ) ;
u8 rrpCond_TrsSglUlExp        ( void ) ;
#ifdef __EGPRS__
u8 rrpCond_TrsnIRBufferInd	  ( void ) ;
#endif
#endif


#ifdef __GPRS__
u8 rrdWrsl_TrsPCCOResult      ( void ) ;
u8 rrpWrsl_TrsConReq          ( void ) ;
#endif


#ifdef __GPRS__
u8 rrpAccN_TrsImmAss          ( void );
u8 rrpAccN_TrsImmAssRej       ( void );
u8 rrpAccN_TrsImmAssCmp       ( void );
u8 rrpAccI_TrsSIAExp          ( void );
u8 rrpAccN_TrsPUlAss          ( void );
u8 rrpAccN_TrsL1PSDataInd     ( void );
u8 rrpAccN_TrsTExp            ( void );
u8 rrpAccN_TrsT3168Exp        ( void );
u8 rrp_TrsContResFail         ( void );
u8 rrpAccN_TrsAccRejInd       ( void );
u8 rrpAccN_TrsL1TBFCfgCnf     ( void );
#endif

// RRD_WCNN
u8 rrgWcnN_TrsPhyInfo         ( void ) ;
u8 rrgWcnN_TrsL1ConnectCnf    ( void ) ;
u8 rrgWcnN_TrsLapResumeCnf    ( void ) ;
u8 rrgWcnN_TrsDataReq         ( void ) ;
u8 rrgWcnN_ResumeInitial      ( void ) ;
u8 rrgWcnN_TrsL1ErrorInd      ( void ) ;
u8 rrgWcnN_TrsLapReleaseInd   ( void ) ;
#ifdef __GPRS__
u8 rrpWcnN_TrsPUlAss          ( void );
u8 rrpWcnN_TrsL1TBFCfgCnf     ( void );
u8 rrpWcnN_TrsL1PSDataInd     ( void );
u8 rrp_TrsContResFail         ( void );
u8 rrpWcnN_TrsT3168Exp        ( void );
#endif

// RRD_WRLS
u8 rrgWrls_TrsLapReleaseInd   ( void ) ;
u8 rrgWrls_TrsL1ErrorInd      ( void ) ;
u8 rrdWrls_TrsAbortReq        ( void ) ;

#endif




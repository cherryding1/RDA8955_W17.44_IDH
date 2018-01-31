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
  File       : RR_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for rr Task

  History    :
--------------------------------------------------------------------------------
  Sep 27 05  |  MCE   | Removed rrNasUpdateInd
  Mar 31 05  |  MCE   | Added l1CbchReq and rrCbchReq 
  Mar 10 05  |  MCE   | Added rr_PlmnUpdInd       
  Sep 22 04  |  MCE   | Added rrGprsReq
  Aug 18 04  |  MCE   | Added l1FrqRfdReq       
  Aug 12 04  |  MCE   | Added l1RachUpdReq      
  Jun 22 04  |  MCE   | Added rrMmpProcInd                     
  Jun 09 04  |  MCE   | Added rrRaUpdSta and rrGrpsStatusInd 
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RR_MSG_H__
#define __RR_MSG_H__

#include "itf_l1.h"
#include "itf_rr.h"
#include "itf_lap.h"
#include "itf_sim.h"
#include "itf_stk.h"

#include "rr_iitf.h"


/*
** Shortcut to access fields in Interlayer messages
*/
#define RR_ILS  B.IL.Hdr.Specific  // specific header 
#define RR_ILD  B.IL.Data          // Radio Data



/*
** RR interlayer headers
*/
typedef union
{
   /*
   ** With MM or RLG
   */
   rr_ConnectReq_t          rrConnectReq   ;
#ifdef __RRP__
   rrp_ConnectReq_t         rrpConnectReq  ;
   mac_DlDataInd_t          macDlDataInd   ;
   mac_UlDataReq_t          macUlDataReq   ;
#endif
   
   /*
   ** With LAP
   */
   lap_EstablishReq_t        lapEstablishReq        ;
   lap_ResumeReq_t           lapResumeReq           ;
   lap_ReconnectReq_t        lapReconnectReq        ;
   lap_DataReq_t             lapDataReq             ;
   lap_UnitDataReq_t         lapUnitDataReq         ;
   lap_DataInd_t             lapDataInd             ;
   lap_UnitDataInd_t         lapUnitDataInd         ;

   /*
   ** With L1
   */
   l1_DataInd_t              l1DataInd      ;
   
#ifdef __RRP__
   l1_DataReq_t              l1DataReq      ;
   l1_PSDataReq_t            l1PSDataReq    ;
#endif
   /*
   ** With RRD 
   */ 
   rr_ConnectReq_t          rr_RrConnectReq ;
   mac_CtrlDataInd_t        rr_MacCtrlDataInd;
   mac_RrDataReq_t          rr_MacDataReq;
} IlHdr_t ;                


#include "itf_il.h" // interlayer body definition

/*
** RR messages body
*/

typedef union 
{

   /*
   ** InterLayer interfaces
   */
   IlBody_t                 IL                 ;

   /*
   ** Internal Interface with RR
   */
   rr_PlmnSearchReq_t      rr_PlmnSearchReq    ;
   rr_PlmnAcceptInd_t      rr_PlmnAcceptInd    ;
   rr_PlmnRejectInd_t      rr_PlmnRejectInd    ;
   rr_PlmnScanReq_t        rr_PlmnScanReq      ;
   rr_PlmnStopScanReq_t    rr_PlmnStopScanReq  ;
   rr_PlmnFoundInd_t       rr_PlmnFoundInd     ;
   rr_PlmnUpdInd_t         rr_PlmnUpdInd       ;
   rr_PlmnNotFoundInd_t    rr_PlmnNotFoundInd  ;
   rr_PlmnLostInd_t        rr_PlmnLostInd      ;
   rr_PlmnAvailableInd_t   rr_PlmnAvailableInd ;
   rr_PlmnListDoneInd_t    rr_PlmnListDoneInd  ;
   rr_BaComplete_t         rr_BaComplete       ;
   rr_ReselGrantReq_t      rr_ReselGrantReq    ;
   rr_ReselReq_t           rr_ReselReq         ;
   rr_CellChangeInd_t      rr_CellChangeInd    ;
   rr_L1ConInd_t           rr_L1ConInd         ;
   rr_L1NotConInd_t        rr_L1NotConInd      ;
   rr_GprsStatusInd_t      rr_GprsStatusInd    ;
   rr_PCCOResult_t         rr_PCCOResult       ;
   rr_PCCOComplete_t       rr_PCCOComplete     ;

#ifdef __RRP__
   mac_PUlAckNackInd_t     macPUlAckNackInd    ;
   mac_PQueNotif_t         macPQueNotif        ;
#endif

	
   /*
   ** Interface with L1
   */
   l1_InitReq_t            l1_InitReq        ;
   l1_PowerReq_t           l1_PowerReq       ;
   l1_PowerRsp_t           l1_PowerRsp       ;
   l1_SynchReq_t           l1_SynchReq       ;
   l1_SynchUpdateReq_t     l1_SynchUpdateReq ;
   l1_StopSynchReq_t       l1_StopSynchReq   ;
   l1_SynchFound_t         l1_SynchFound     ;
   l1_SynchNotFound_t      l1_SynchNotFound  ;
   l1_CchReq_t             l1_CchReq         ;
   l1_PCchReq_t            l1_PcchReq        ;
   l1_BcchReq_t            l1_BcchReq        ;

   l1_BcchModeReq_t     l1_BcchModeReq;
   l1_AdjCellReportReq_t  l1_AdjCellReportReq;

   l1_PBcchReq_t           l1_PBcchReq       ;
   l1_StopChReq_t          l1_StopChReq      ;
   l1_MonitorReq_t         l1_MonitorReq     ;
   l1_MonitorInd_t         l1_MonitorInd     ;
   l1_NcMonitorReq_t       l1_NcMonitorReq   ;
   l1_NcMonitorInd_t       l1_NcMonitorInd   ;
   l1_PMeasCtrlReq_t       l1_PMeasCtrlReq   ;
   l1_RachOnReq_t          l1RachOnReq       ;
   l1_RachInd_t            l1RachInd         ; 
   l1_ConnectReq_t         l1ConnectReq      ;
   l1_ConnectCnf_t         l1ConnectCnf      ;
   l1_CiphParam_t          l1CiphCfgReq      ;
   l1_ChanModeModifyReq_t  l1ChnModReq       ;
   l1_ErrorInd_t           l1ErrorInd        ;
   l1_PhyInfoReq_t         l1PhyInfoReq      ;
   l1_CellOptUpdtReq_t     l1CellOptUpdReq   ;
   l1_SetTestLoopReq_t     l1SetTestLoop     ;
   l1_RachUpdtReq_t        l1RachUpdReq      ;
   l1_FreqRedefReq_t       l1FrqRdfReq       ;
   l1_CbchReq_t            l1CbchReq         ;
   l1_HighBandChangeReq_t  l1HighBandChangeReq;
#ifdef __RRP__
   l1_TBFCfgReq_t          l1TBFCfgReq       ;
   l1_TBFCfgCnf_t          l1TBFCfgCnf       ;
   l1_PDCHRelReq_t         l1PDCHRelReq      ;
   l1_PwrCtrlTAReq_t       l1PwrCtrlTAReq    ;
   l1_TBFRelReq_t          l1TBFRelReq       ;
   l1_PSDataInd_t          l1PSDataInd       ;
   l1_MacFixedUpdtReq_t    l1MacFixedUpdtReq ;
   l1_EndFixedPeriodInd_t  l1EndFixedInd     ;
   l1_ChnQualityInd_t      l1ChnQualInd      ;
#ifdef __EGPRS__
   l1_IRBufferStatus_Ind_t l1_IRBuffStatInd;
   l1_LoopbackSwiReq_t	   l1LoopbackSwiReq  ;
#endif
#endif

   /*
   ** Interface with LAP
   */
   lap_EstablishCnf_t        lapEstablishCnf        ;
   lap_SuspendReq_t          lapSuspendReq          ;
   lap_NormalReleaseReq_t    lapNReleaseReq         ;
   lap_LocalReleaseReq_t     lapLReleaseReq         ;
   lap_ReleaseInd_t          lapReleaseInd          ;
   lap_NormalReleaseCnf_t    lapNReleaseCnf         ;
   lap_EstablishFailureInd_t lapEstablishFailureInd ;
   lap_ResumeFailureInd_t    lapResumeFailureInd    ;
   lap_ReconnectFailureInd_t lapReconnectFailureInd ;
   
   /*
   ** Interface with CC
   */
   rr_ChanModeInd_t          rrChanModeInd          ;

   /*
   ** RR Connection interface ( MM and RLG use it )
   */ 
   rr_ReleaseInd_t           rrReleaseInd           ;

#ifdef __RRP__
   /*
   ** Interface MAC / RLG
   */
   mac_AckNackInd_t         macAckNackInd;
   mac_TlliInd_t            macTlliInd;

   /*
   ** Interface MAC / RRP
   */
   mac_AccRej_t             macAccRej;

   


   mac_NewTlliCnf_t         macNewTlliCnf;

   /*
   ** Interface RRP / RRP
   */
   rrp_ImmAssCmp_t          rrpImmAssCmp;

   /*
   ** Interface RRP / RLG
   */
   rrp_ConnectReq_t         rrpConnectReq;
   rrp_DlConnectInd_t       rrpDlConnectInd;
   rrp_UlConnectCnf_t       rrpUlConnectCnf;
   rrp_UlReleaseReq_t       rrpUlReleaseReq;
   rrp_CellOptInd_t         rrpCellOptInd;
#ifdef __EGPRS__
   rrp_Egprs_LoopBackCmd_t  rrpEgprsLoopbackCmd;
#endif
   rrp_TestModeReq_t  rrpTestModeReq;
#endif

   /*
   ** Interface with MM
   */
   rr_CellInd_t             rrCellInd      ;
   rr_CellRsp_t             rrCellRsp      ;
   rr_LocUpSta_t            rrLocUpSta     ;
   rr_KcInd_t               rrKcInd        ;
   rr_CmkInd_t              rrCmkInd       ;
   mac_NewTlliInd_t         macNewTlliInd  ;
   rr_AbortReq_t            rrAbortReq;
   rr_RaUpSta_t             rrRaUpdSta     ;
   rr_MmpProcInd_t          rrMmpProcInd   ;
   rr_MmpReadyStInd_t       rrMmReadyStInd ;
   rr_GprsReq_t             rrGprsReq      ;
#ifdef __EGPRS__
   rrp_AccTechTypeList_t  rrpAccTechTypeListInd;
 #endif

   /*
   ** Interface with SIM
   */
   sim_ReadBinaryReq_t       simReadBinaryReq   ;
   sim_ReadRecordReq_t       simReadRecordReq   ;
   //sim_FileStatusReq_t   simFileStatusReq   ;
   sim_ElemFileStatusReq_t   simFileStatusReq   ;
   sim_UpdateBinaryReq_t     simUpdateBinaryReq ;

   sim_ReadBinaryCnf_t       simReadBinaryCnf ;
   sim_ReadRecordCnf_t       simReadRecordCnf ;
   //sim_FileStatusCnf_t   simFileStatusCnf ;
   sim_ElemFileStatusCnf_t   simFileStatusCnf ;


   


   rr_CbchReq_t              rrCbchReq        ;

   /*
   ** Interface with API
   */
   rr_NwSelModeReq_t      rrNwSelModeReq   ;
   rr_NwSelReq_t          rrNwSelReq       ;
   rr_NwCsRegStaInd_t     rrNwCsRegStaInd  ;
   rr_NwPsRegStaInd_t     rrNwPsRegStaInd  ;
   rr_NwListInd_t         rrNwListInd      ;
   rr_NwListAbortInd_t         rrNwListAbortInd      ;
   rr_QualReportReq_t     rrQualReportReq  ;
   rr_QualReportCnf_t     rrQualReportCnf  ;
   rr_CurCellInfoReq_t    rrCurCInfoReq    ;
   rr_CurCellInfoInd_t    rrCurCInfoInd    ;
   rr_AdjCellInfoReq_t    rrAdjCInfoReq    ;
   rr_AdjCellInfoInd_t    rrAdjCInfoInd    ;

   rr_SyncInfoReq_t     rrSyncInfoReq;
   rr_PowerListInd_t     rrPowerListInd;
   rr_SyncInfoInd_t      rrSyncInfoInd;
	 
   stk_StartReq_t         rrStartReq       ;
   rr_NmoCfgReq_t         rrNmoCfgReq      ;
   rr_NwStoreListInd_t        rrNwStoreListInd;
   rr_SimOpenInd_t            rrSimOpenInd;
   stk_PowerScanReq_t    rrPowerScanReq;
   stk_PowerScanRsp_t    rrPowerScanRsp;
   stk_StopCnf_t            stkStopCnf;   
   rr_DetectMBSReq_t      rrDectectMBSReq   ;

} MsgBody_t;

#include "itf_msg.h"

#endif

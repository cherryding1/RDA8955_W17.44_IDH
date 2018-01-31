//================================================================================
//  File       : MM_MSG.H
//--------------------------------------------------------------------------------
//  Scope      : Defintion of Msg body for mm Task
//  History    :
//--------------------------------------------------------------------------------
//  Aug 06 03  |  OTH   | Creation            
//================================================================================
#ifndef __MM_MSG_H__
#define __MM_MSG_H__

#include "itf_rr.h"
#include "itf_mm.h"
#include "itf_sim.h"
#include "itf_stk.h"
#ifdef __GPRS__
#include "itf_llc.h"
#include "itf_rlc.h"
#endif

#define MM_ILS  B.IL.Hdr.Specific  // specific header 
#define MM_ILD  B.IL.Data          // Radio Data

//============================================================================
// MM/MMP interlayer headers
//============================================================================
typedef union
{
	// Interlayer header with RR
	rr_ConnectReq_t rrConnectReq;

#ifdef __GPRS__
	// Interlayer header with LLC
	llc_UnitDataReq_t llcUnitDataReq;

	// Interlayer header with SM and SMS
	mmp_DataReq_t       mmpDataReq;
#endif
} IlHdr_t;

#include "itf_il.h"
#include "mm_intra.h"


//============================================================================
// MM/MMP messages body
//============================================================================
typedef union 
{
	IlBody_t IL;

	// Interface with RR
	rr_CellInd_t    rrCellInd;
	rr_CellRsp_t    rrCellRsp;
	rr_ReleaseInd_t rrRelInd;
	rr_LocUpSta_t   rrLocUpSta;
	rr_KcInd_t      rrKcInd;
	rr_CmkInd_t     rrCmkInd;
	rr_AbortReq_t  rrAbortReq;

	// Interface with CM
	mm_EstabReq_t      mmEstabReq;
	mm_ReestabReq_t    mmReestabReq;
	mm_ReleaseInd_t    mmRelInd;
	mm_PDTIInd_t       mmPDTIInd; // used for the ServReqAck and the EstabInd, ReleaseReq
	mm_EstabErrInd_t   mmEstabErrInd; // used for the ServReqAck and the EstabInd

	// Interface with SIM
	usim_RunUmtsAlgoReq_t   simRunUmtsAlgoReq;
	usim_RunUmtsAlgoCnf_t   simRunUmtsAlgoCnf;
	sim_RunGsmAlgoReq_t     simRunGsmAlgoReq;
	sim_RunGsmAlgoCnf_t     simRunGsmAlgoCnf;
	sim_ReadBinaryReq_t     simReadBinaryReq;
	sim_ReadBinaryCnf_t     simReadBinaryCnf;
	sim_UpdateBinaryReq_t   simUpdateBinaryReq;
	sim_UpdateBinaryCnf_t   simUpdateBinaryCnf;
	#ifdef __USING_SMS_STORE_PS_LOCI__
	sim_ReadRecordReq_t     simReadRecodeReq;
	sim_ReadRecordCnf_t     simReadRecodeCnf;
	sim_UpdateRecordReq_t   simUpdateRecodeReq;
	sim_UpdateRecordCnf_t   simUpdateRecodeCnf;
	#endif

	
	sim_ElemFileStatusReq_t simElemFileStatusReq;
	sim_ElemFileStatusCnf_t simElemFileStatusCnf;
	sim_StatusErrorInd_t    simStatusErrorInd;
	// Interface with API
	mm_StopReq_t           mmStopReq;
	mm_NwkInfoInd_t        mmNwkInfoInd;
	stk_StopCnf_t               stkStopCnf;
	mm_SimCloseCnf_t      mmSimCloseCnf; /*added by liujg 090806*/
	api_SimOpenInd_t       apiSimOpenInd; 
	//[[ add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP	
	mm_PsAttachReq_t       mmPsAttachReq;
	mm_PsDetachReq_t       mmPsDetachReq;
	//]] add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP	
#ifdef __EGPRS__
	mm_apiStartReq_t      mmStartReq;  // added by yangyang for EDGE at 2007-04-03
#endif

	// Interface with MM
	mmc_SelfLocup_t mmcSelfLocup;
	mmc_SelfPaging_t       mmcSelfPaging;
#ifdef __GPRS__
	mmp_SelfPaging_t       mmpSelfPaging;
#endif

#ifdef __GPRS__
	// Interface with RR
	rr_RaUpSta_t       rrRaUpSta;
	mac_NewTlliInd_t   macNewTlliInd;
	rr_MmpProcInd_t    rrMmpProcInd;
	rr_GprsReq_t           rrGprsReq;
	rr_MmpReadyStInd_t     rrMmpReadyStInd;

#ifdef __EGPRS__
	rrp_AccTechTypeList_t   rrp_AccTechTypeListInd; // added by yangyang for EDGE at 2007-03-30
#endif

	// Interface with RLC
	rlu_TestModeReq_t      rluTestModeReq;
	rlu_TReadyReq_t        rluTReadyReq;
	rlu_TReadyInd_t        rluTReadyInd;
	rlu_SynchInd_t         rluSynchInd;
#ifdef __EGPRS__
	rlu_EgprsLoopBackMode_t	 rluEgprsTestModeReq; // added by yangyang for EDGE at 2007-03-30
#endif
	// Interface with LLC
	llc_TlliInd_t        llcTlliInd;
	llc_CiphParamInd_t   llcCiphParamInd;
	llc_QueueCreateInd_t llcQueueCreateInd;
	llc_TriggerReq_t       llcTriggerReq;
	llc_NewTlliInd_t       llcNewTlliInd;

	// Interface with SM
	mmp_PdpCtxStatusInd_t  mmpPdpCtxStatusInd;
	mmp_AbortInd_t         mmpAbortInd;
	mmp_DelSavaDataReq_t 	mmpDelSavaDataReq; // DMX add 20161031.  MMP_DEL_SAVE_DATA_REQ

	// Interface with SNDCP
	mmp_SequenceInd_t      mmpSequenceInd;
	mmp_SequenceRsp_t      mmpSequenceRsp;
#endif

} MsgBody_t;

#include "itf_msg.h"

#endif

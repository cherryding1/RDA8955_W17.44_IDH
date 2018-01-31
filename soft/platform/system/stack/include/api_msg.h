//================================================================================
//  File       : API_MSG.H
//--------------------------------------------------------------------------------
//  Scope      : Defintion of Msg body for Api Task
//  History    :
//--------------------------------------------------------------------------------
//  Mar 31 06  |  MCE   | Added api_RluUlCongestInd 
//  Jan 11 06  |  MCE   | Added rr_ChanModeInd and api_CcChanModeInd
//  Nov 15 05  |  MCE   | Added api_DbgInfoCmd
//  Nov 15 05  |  ADA   | Added api_DbgInfoInd 
//--------------------------------------------------------------------------------
//  Mar 10 04  |  PCA   | Creation
//================================================================================

#ifndef __API_MSG_H__
#define __API_MSG_H__

#include "itf_msgc.h"
#include "itf_api.h"
#include "itf_sim.h"
#include "itf_sms.h"
#include "itf_cc.h"
#include "itf_rr.h"
#include "itf_stk.h"
#include "itf_snd.h"
#include "itf_rlc.h" 

/*
** Shortcut to access fields in Interlayer messages
*/

/*
** API interlayer headers
*/
typedef union
{
 /*
 ** With MMI
 */

 /*
 ** With RR
 */

 /*
 ** With CC
 */

 /*
 ** With SND
 */
 snd_SendReq_t         snd_SendReq;
 snd_DataInd_t         snd_DataInd;

 /*
 ** With SMS
 */
 smspp_SendReq_t       smspp_SendReq;
 smspp_SendMMAReq_t    smspp_SendMMAReq;
 smspp_AckReq_t        smspp_AckReq;
 smspp_ErrorReq_t      smspp_ErrorReq;

 smspp_ErrorInd_t      smspp_ErrorInd;
 smspp_ReceiveInd_t    smspp_ReceiveInd;

} IlHdr_t ;


#include "itf_il.h" // interlayer body definition

/*
** API messages body
*/
typedef union
{
 /*
 ** With MMI 
 */
 api_StartReq_t                    api_StartReq;
 api_StopReq_t                     api_StopReq ;
 api_ImeiReq_t                     api_ImeiReq ;
 api_ImeiInd_t                     api_ImeiInd ;
 api_DbgInfoInd_t                  api_DbgInfoInd;
 api_DbgInfoCmd_t                  api_DbgInfoCmd;

 api_QualReportReq_t               api_QualReportReq ;
 api_QualReportCnf_t               api_QualReportCnf ;
 api_NwSelModeReq_t                api_NwSelModeReq ;
 api_NwListReq_t                   api_NwListReq ;
 api_NwListAbortReq_t              api_NwListAbortReq ;
 api_NwSelReq_t                    api_NwSelReq ;
 api_NwReselReq_t                  api_NwReselReq ;

//[[ add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
 api_NwPsAttachReq_t               api_NwPsAttachReq ;
 api_NwPsDetachReq_t               api_NwPsDetachReq ;
//]] add, dmx 20140103, [support_attach_detach_type],eg.support DETACH_CS_USING_MMP
 api_NwCsRegStatusInd_t            api_NwCsRegStatusInd ;
 api_NwPsRegStatusInd_t            api_NwPsRegStatusInd ;
 api_NwStoreListInd_t                 api_NwStoreListInd;
 api_NwListInd_t                   api_NwListInd ;
 api_SimOpenInd_t               api_SimOpenInd;
 api_PowerScanReq_t           api_PowerScanReq;
 api_PowerScanRsp_t		api_PowerScanRsp;

 api_SmsPPSendReq_t                api_SmsPPSendReq;
 api_SmsPPSendMMAReq_t             api_SmsPPSendMMAReq;
 api_SmsPPAckReq_t                 api_SmsPPAckReq;
 api_SmsPPErrorReq_t               api_SmsPPErrorReq;
 api_SmsCBActivateReq_t            api_SmsCBActivateReq;
 api_SmsCBDeActivateReq_t          api_SmsCBDeactivateReq;

 api_SmsPPReceiveInd_t             api_SmsPPReceiveInd;
 api_SmsPPAckInd_t                 api_SmsPPAckInd;
 api_SmsPPErrorInd_t               api_SmsPPErrorInd;
 api_SmsCBReceiveInd_t             api_SmsCBReceiveInd;

 api_SsActionReq_t                 api_SsActionReq ;

 api_SsActionCnf_t                 api_SsActionCnf ;
 api_SsTIInd_t                     api_SsTIInd ;
 api_SsErrorInd_t                  api_SsErrorInd ;
 api_SsActionCnf_t                 api_SsActionInd ;

 api_PdpCtxActivateReq_t           api_PdpCtxActivateReq ;
 api_PdpCtxModifyReq_t             api_PdpCtxModifyReq ;
 api_PdpCtxDeactivateReq_t         api_PdpCtxDeactivateReq ;
 api_PSDataReq_t                   api_PSDataReq ;
#ifdef LTE_NBIOT_SUPPORT
 api_PSDataReqLte_t                api_PSDataReqLte;
#endif
 api_PdpCtxActivateCnf_t           api_PdpCtxActivateCnf ;
 api_PdpCtxModifyAcc_t             api_PdpCtxModifyAcc ;
 api_PdpCtxActivateInd_t           api_PdpCtxActivateInd ;
 api_PdpCtxModifyCnf_t             api_PdpCtxModifyCnf ;
 api_PdpCtxModifyInd_t             api_PdpCtxModifyInd ;
 api_PdpCtxDeactivateInd_t         api_PdpCtxDeactivateInd ;
#ifdef LTE_NBIOT_SUPPORT
api_PSDataInd_t                    api_PSDataIndEvt;
api_PSDataIndLte_t                 api_PSDataIndLteEvt;
#else
 api_PSDataInd_t                   api_PSDataInd ;
#endif 
 api_SimResetReq_t                 api_SimResetReq ;
 api_SimReadBinaryReq_t            api_SimReadBinaryReq ;
 api_SimUpdateBinaryReq_t          api_SimUpdateBinaryReq ;
 api_SimReadRecordReq_t            api_SimReadRecordReq ;
 api_SimUpdateRecordReq_t          api_SimUpdateRecordReq ;
 api_SimInvalidateReq_t            api_SimInvalidateReq ;
 api_SimRehabilitateReq_t          api_SimRehabilitateReq ;
 api_SimSeekReq_t                  api_SimSeekReq ;
 api_SimIncreaseReq_t              api_SimIncreaseReq ;
 api_SimVerifyCHVReq_t             api_SimVerifyCHVReq ;
 api_SimChangeCHVReq_t             api_SimChangeCHVReq ;
 api_SimDisableCHVReq_t            api_SimDisableCHVReq ;
 api_SimEnableCHVReq_t             api_SimEnableCHVReq ;
 api_SimUnblockCHVReq_t            api_SimUnblockCHVReq ;
 api_SimElemFileStatusReq_t        api_SimElemFileStatusReq ;
 api_SimDedicFileStatusReq_t       api_SimDedicFileStatusReq ;
 api_SimTerminalProfileReq_t       api_SimTerminalProfileReq ;
 api_SimTerminalEnvelopeReq_t      api_SimTerminalEnvelopeReq ;
 api_SimTerminalResponseReq_t      api_SimTerminalResponseReq ;
 api_SimToolkitActivateReq_t       api_SimToolkitActivateReq ;

 api_SimResetCnf_t                 api_SimResetCnf ;
 api_SimStatusErrorInd_t           api_SimStatusErrorInd ;
 api_SimReadBinaryCnf_t            api_SimReadBinaryCnf ;
 api_SimUpdateBinaryCnf_t          api_SimUpdateBinaryCnf ;
 api_SimReadRecordCnf_t            api_SimReadRecordCnf ;
 api_SimUpdateRecordCnf_t          api_SimUpdateRecordCnf ;
 api_SimInvalidateCnf_t            api_SimInvalidateCnf ;
 api_SimRehabilitateCnf_t          api_SimRehabilitateCnf ;
 api_SimSeekCnf_t                  api_SimSeekCnf ;
 api_SimIncreaseCnf_t              api_SimIncreaseCnf ;
 api_SimVerifyCHVCnf_t             api_SimVerifyCHVCnf ;
 api_SimChangeCHVCnf_t             api_SimChangeCHVCnf ;
 api_SimDisableCHVCnf_t            api_SimDisableCHVCnf ;
 api_SimEnableCHVCnf_t             api_SimEnableCHVCnf ;
 api_SimUnblockCHVCnf_t            api_SimUnblockCHVCnf ;
 api_SimElemFileStatusCnf_t        api_SimElemFileStatusCnf ;
 api_SimDedicFileStatusCnf_t       api_SimDedicFileStatusCnf ;
 api_SimTerminalProfileCnf_t       api_SimTerminalProfileCnf ;
 api_SimTerminalEnvelopeCnf_t      api_SimTerminalEnvelopeCnf ;
 api_SimTerminalResponseCnf_t      api_SimTerminalResponseCnf ;
 api_SimFetchInd_t                 api_SimFetchInd ;

 /*
 ** With RR
 */
 rr_NwListInd_t                    rr_NwListInd   ;
 stk_StopCnf_t                     stk_StopCnf    ;
 mm_SimCloseCnf_t            stk_SimCloseCnf    ; //added by liujg 090806

 stk_StartReq_t                    stk_StartReq   ;
 rr_ChanModeInd_t                  rr_ChanModeInd ;

 /* 
 ** With RLU
 */
 api_UlCongestInd_t                api_RluUlCongestInd ;

 /*
 ** With SIM
 */
 sim_ReadBinaryReq_t sim_ReadBinaryReq;
 sim_VerifyCHVReq_t  sim_VerifyCHVReq ;
 sim_VerifyCHVCnf_t  sim_VerifyCHVCnf ;
#ifdef LTE_NBIOT_SUPPORT
 api_EpsCtxActivateInd_t          api_EpsCtxActivateInd;
 api_EpsCtxActivateRsp_t          api_EpsCtxActivateRsp;
 api_EpsCtxActivateRej_t          api_EpsCtxActivateRej;

 api_EpsCtxAllocateReq_t          api_EpsCtxAllocateReq;
 api_EpsCtxAllocateCnf_t          api_EpsCtxAllocateCnf;
 api_EpsCtxAllocateRej_t          api_EpsCtxAllocateRej; 
#endif
 /*
 ** With MMI 
 */
 api_CcTIInd_t          api_CcTIInd;
 api_CcStartDTMFReq_t   api_CcStartDtmfReq ;
 api_CcStopDTMFReq_t    api_CcStopDtmfReq  ;
 api_CcChanModeInd_t    api_CcChanModeInd  ;

 api_FlowCtrInd_t       api_FlowCtrInd;
 /*
 ** InterLayer interfaces
 */
 IlBody_t IL;

 //add by yanzhili
 api_NwListAbortInd_t api_NwListAbortInd;
 #ifdef LTE_NBIOT_SUPPORT
 api_MmcStartReq_t api_MmcStartReq;
 api_MmcNwSelReq_t api_MmcNwSelReq;
 api_MmcStopReq_t api_MmcStopReq;
 api_MmcNwListReq_t api_MmcNwListReq;
 api_MmcNwListAbortReq_t api_MmcNwListAbortReq;

 api_CmcSmsppSendReq_t api_CmcSmsppSendReq;
 api_CmcSmsPPSendMMAReq_t api_CmcSmsPPSendMMAReq;
 api_CmcSmsPPAbortReq_t api_CmcSmsPPAbortReq;
 api_CmcSmsPPAckReq_t api_CmcSmsPPAckReq;
 api_CmcSmsPPErrorReq_t api_CmcSmsPPErrorReq;

 
 api_CmcPdpCtxActivateReq_t api_CmcPdpCtxActivateReq;
 #ifdef __MINQOS__
 api_CmcPdpSetMinQoSReq_t api_CmcPdpSetMinQoSReq;
 #endif
 api_CmcPdpCtxModifyReq_t api_CmcPdpCtxModifyReq;
 api_CmcPdpCtxModifyAcc_t api_CmcPdpCtxModifyAcc;
 api_CmcPdpCtxDeactivateReq_t api_CmcPdpCtxDeactivateReq;
 api_CmcDefaultPdnCtxConfigReq_t api_CmcDefaultPdnCtxConfigReq;
 #endif
} MsgBody_t;

#include "itf_msg.h"  // message definition

#endif

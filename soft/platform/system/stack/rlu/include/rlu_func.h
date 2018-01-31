//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_func.h,v 1.4 2006/03/08 00:18:10 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is not property of StackCom.
  This source code is from the public domain.
  The information contained in this file is not confidential.
  Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is allowed.

================================================================================
*/
/*
================================================================================
  File       : rlu_func.h
--------------------------------------------------------------------------------

  Scope      : Protoypes of RLU functions

  History    :
--------------------------------------------------------------------------------
  Jul 06 06  |  MCE   | removed rlu_NonUserLlcPdu ( )
  Jun 28 06  |  MCE   | Added rlu_SearchLlcPduQ() and rlu_NonUserLlcPdu ( )
  Jun 17 06  |  MCE   | Added rlu_StartLlcTbf and rlu_StartTstTbf
  May 03 06  |  MCE   | Added rlu_SetTbfCause
  Apr 14 06  |  MCE   | Added rlu_ResumeTx
  Apr 07 06  |  MCE   | Added rlu_UpdPanVar
  Mar 30 06  |  MCE   | Added rlu_UlFlowCtrl()
  Feb 11 06  |  MCE   | Added rlu_PrepCsDiscard()
  Feb 01 06  |  MCE   | Added rlu_InitEarlyRel() and rlu_ShortenTbf ( ) 
  Jan 31 06  |  MCE   | added rlu_LlcPduPurge()
  Jan 12 06  |  MCE   | added rlu_SndRrRelReq()
  Dec 08 05  |  MCE   | added rlu_SetCurFlow()
  Apr 22 05  |  MCE   | Added rlu_PurgeDQ()
  Sep 09 04  |  MCE   | Removal of rlu_UpdateSduSizes
  Sep 09 04  |  MCE   | Addition of rlu_RetrieveLlcPdus
  Feb 05 04  |  MCE   | Creation 
================================================================================
*/

#ifndef __RLU_FUNC_H__
#define __RLU_FUNC_H__

// System includes
#include "sxs_type.h"    
#include "cmn_defs.h"

// Interfaces

// RLU includes
#include "rlu_data.h"


// rlu_evt.c
InEvtDsc_t * rlu_GiveDsc ( u32 MsgId ) ;

// rlu_out.c
void  rlu_PrepMsg       ( OutEvtDsc_t * Evt, u16 Len )   ;
void  rlu_SendMsg       ( u8 Mbx )                      ;
void  rlu_SndContinueTx ( void )                         ;
void  rlu_SndQDataReq   ( void )                         ;

// rlu_util.c
void        rlu_ResumeTx        ( void )                 ;
void        rlu_CleanCtx        ( bool KeepSdu )              ;
u8          rlu_PduPurge        ( void * Unused, void * Pdu ) ;
u8          rlu_SduPurge        ( void * Unused, void * Sdu ) ;
rlu_Sdu_t * rlu_SearchSduQ      ( rlu_Sdu_t * Searched ) ;
rlu_Pdu_t * rlu_SearchShortPduQ ( u32 ShortBsn )         ;
void        rlu_SendPdu         ( void )                 ;
#ifdef __EGPRS__
u8 egprs_Ack_PDU(mac_AckNackInd_t *pDes, u8 *p_DiscId,u8 *p_DiscLen,u16 *pRpBSN);
u8  gprs_Ack_PDU(mac_AckNackInd_t * p_Des, u8 *p_DiscId,u8 *p_DiscLen,u16 *pRpBSN );

void egprs_rlu_RepeatPdu( rlu_Pdu_t * MyPdu, u8 If_only_Nack,u8 If_only_LastOne);

#endif
void        rlu_RepeatPdu       ( rlu_Pdu_t * Pdu )      ;

void        rlu_AckSdu          ( rlu_Pdu_t * Pdu )      ;
void        rlu_AckPdu          ( rlu_Pdu_t * Pdu )      ;
rlu_Pdu_t * rlu_SearchPduQ      ( u32 Bsn )              ;
rlu_Pdu_t * rlu_SearchNackedPdu ( void )                 ;
rlu_Flow_t *rlu_SetCurFlow      ( void )                 ;
rlu_Flow_t *rlu_SearchNextFlow  ( rlu_Flow_t * Flow )    ;
rlu_Pdu_t  * rlu_SearchCvPduQ   ( u8 Cv )                ;
rlu_Sdu_t  * rlu_BuildSdu       ( Msg_t * LlcPdu )       ;
u8          rlu_ComputeCV       ( u32 Bsn )              ;
void        rlu_ComputeTbc      ( u8 Cs, bool CloseEnded , 
                                  u8 BlockGranted )      ;
u32         rlu_ComputeFlowTbc  ( u8 Cs, bool ContRes, rlu_Flow_t * Flow ) ;
void        rlu_RmvAllLlcPdus   ( u8 Cause )                      ;
Msg_t     * rlu_SearchLlcPduQ   ( u8 Cause )                      ;  
rlu_Sdu_t * rlu_NextSdu2Send        ( void )                      ;
rlu_Sdu_t * rlu_FindFirstSduInFlows ( u8 Status )                 ;
void        rlu_RmvAllSdusFromFlows ( u8 LlcCause, bool OneFlow ) ;
void        rlu_InitConnection  ( void )                 ;
rlu_Flow_t *rlu_CreateFlow      ( Msg_t * LlcPdu )       ;
bool        rlu_SameQoS         ( rlu_Flow_t * Flow1     ,
                                  rlu_Flow_t * Flow2 )   ;
u8          rlu_IsWeakerFlow    ( rlu_Flow_t * Flow1     ,
                                  rlu_Flow_t * Flow2 )   ;
u8          rlu_IsWeakerOrEqFlow( rlu_Flow_t * Flow1     ,
                                  rlu_Flow_t * Flow2 )   ;
void        rlu_SaveSdu         ( rlu_Sdu_t * Sdu, rlu_Flow_t * Flow ) ;
bool        rlu_RetrieveLlcPdus ( void )                 ;
void        rlu_StartTxFill     ( void )                 ;
u8          rlu_StartLlcTbf     ( void )                 ;
u8          rlu_StartTstTbf     ( void )                 ;
u8 			rlu_StartTbf( void ) ;
void        rlu_ConnectRr       ( void )                 ;
void        rlu_ExtendTbf       ( rlu_Flow_t * Flow )    ;
void        rlu_SndRrConnectReq ( rlu_Flow_t * Flow, u32 Tbc, bool Extend ) ;
void        rlu_SndRrRelReq     ( u8 RelCause )          ;
void        rlu_SndMmpTReadyInd ( u8 Status )            ;
bool        rlu_IsSduTx         ( rlu_Sdu_t * Sdu )      ;
bool        rlu_Need2StartTReady( rlu_Sdu_t * Sdu )      ;
void        rlu_SyncMm          ( rlu_Sdu_t * Sdu, u8 Action ) ;
void        rlu_PurgeDQ         ( void )                 ;
void        rlu_UndoTx          ( rlu_Pdu_t * Pdu )      ;

void        rlu_SndRluQDReq     ( void )                 ;
u8          rlu_LlcPduPurge     ( void * Unused, void * LlcPdu ) ;
void        rlu_InitEarlyRel    ( void )                 ;
void        rlu_ShortenTbf      ( void )                 ;
void        rlu_SndUlCongestInd ( bool Status )          ;
void        rlu_UpdPanVars      ( rrp_CellOptInd_t * CellOpt ) ;
void        rlu_SetTbfCause     ( void )                 ;


Msg_t     * rlu_PrepCsDiscard   ( Msg_t * DiscardMsg )   ;
Msg_t     * rlu_PrepDiscardMsg  ( void )                 ;
u8          rlu_BackTrack( l1_TxDiscardInd_t * DiscardL );

#ifdef __EGPRS__
bool egprs_BuildPdu(rlu_Pdu_t * Pdu,u8 Mcs_in_use );
void egprs_SendMsg (Msg_t *send_msg,u16 Mbx, OutEvtDsc_t *p_OEvtDsc) ;

u8 release_block_Id(u8 block_Id, u16 *p_Bsn1,u16 *p_Bsn2);
u8 find_Id_fromBsn(u16 Bsn, u8 *pId_group,u8 *pId_num);

u8 egprs_TxBlock(u8 Mcs,rlu_Pdu_t *MyPdu1, rlu_Pdu_t * MyPdu2,u8 Padding,u8 RSB,u8 SI);
void egprs_Tx_SplitBlock(u8 Mcs_use,rlu_Pdu_t *MyPdu, u8 Padding,u8 SI);


void MCS_switch_without_Seg(u8 MCS_ini, u8 MCS_cmd, u8 *pMCS_result,u8 *pPad);
u8  MCS_switch_with_Seg(u8 MCS_ini, u8 MCS_cmd, u8 *pMCS_result,u8 *pPad);
u8 RB_Decompression(u8 StartColour, u16 len_out_max,u8 *p_In, u16 len_In, u8 *p_out, u16 *len_Out);
u8 deal_Msg_UpLinkAckNack(u16 *pRpBSN);
void rlu_GenerateLoopBackFlow () ;

#endif
void        rlu_StartNextSdu    ( void )                 ;
u8          rlu_TxSduPart       ( void )                 ;


Msg_t     * rlu_BuildTstSdu     ( void )                 ;
void        rlu_GenerateTstFlow ( u8 NbReqSdu )          ;


void        rlu_UlFlowCtrl      ( void )                 ;


void        rlu_UpdTbfParams    ( rrp_UlConnectCnf_t * TbfParams ) ;
u8           rlu_PurgeQ(void * Unused, void * Block );
extern   bool sxs_IsExtULTBFSupported(void);
#endif

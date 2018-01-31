//------------------------------------------------------------------------------
//  $Log: sm_data.h,v $
//  Revision 1.6  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.5  2006/02/23 13:15:59  aba
//  Additional commentaries
//
//  Revision 1.4  2006/01/27 18:05:44  aba
//  *** empty log message ***
//
//  Revision 1.3  2006/01/24 11:54:44  aba
//  minimum quality of service
//
//  Revision 1.2  2005/12/29 17:39:16  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/

/*
================================================================================
  File       : SM_DATA.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for SM

  History    :
--------------------------------------------------------------------------------
  Jun 22 04  |  PCA   | Creation
================================================================================
*/

#ifndef __SM_DATA_H__
#define __SM_DATA_H__

#include "cmn_defs.h"   // Common definition

#include "sm_msg.h"     // SM messages definition
#include "sm_defs.h"    // SM internal definition
#include "itf_sm.h"     // SM interface definition


#ifdef __SM_C__
#define DefExtern
#else
#define DefExtern extern
#endif

#define NB_MAX_NSAPI 11
#define NSAPIBASE    5
#define SM_MAX_QOS_LENGTH   API_QOS_LEN_MAX

// For TX TI flag
#define NETWORK_ORIGINATED_TI  0x80
#define MOBILE_ORIGINATED_TI   0

// For CurrentProc
#define NO_PROC                                 0
#define NTW_INITIATED_DEACTIVATION              1
#define API_INITIATED_DEACTIVATION              2
#define SM_INITIATED_DEACTIV_FOR_ACTIV_FAILURE  3
#define ACTIVATION_QOS_FAILURE                  4
#define MMP_ABORT_DEACTIVATION                  5
#define SM_STATUS_REQ_DEACTIVATION              6
#define LOCAL_DEACTIVATION                      7
#define MODIFICATION_QOS_FAILURE                8
 // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
#define NTW_DEACTIVATION_PDP_IN_RAU_ACC         9  

//============================================================================
//  Structure definition
//============================================================================

typedef struct
{
  u8     TxTIFlag;            // TX Value of the transaction identifier flag
                              // 0x80 => Network originated TI
                              // 0x00 => Mobile originated TI
  u8     TI;                  //  SM-SM context Transaction Identifier
  u8     Sapi;                // LLC Sapi
  u8     CurrentQoSLen;
  u8     ModifyReqQoSLen;     // Storage of the QoS before sending the modify pdp ctx request to mmp
  u8     RetransCounter;     // Retransmission counter used for the sending of MMP messages
  u8     CurrentQoS[SM_MAX_QOS_LENGTH];    /* QoS requested by user / negociated         */
  u8     ModifyReqQoS[SM_MAX_QOS_LENGTH];
  Msg_t *LinkedTxMsg;        // Only used to know if the message has been sended by lower layer
  Msg_t *RetransMsg;         // Storage of the transmitted message for potential retransmission
  Msg_t *ApiCnfMsg;          // Storage of the api confirm message before sending it
  u8     PdpCtxDeactivateReqCause; // Storage of the cause of the API_PDP_CTXDEACTIVATE_REQ message
  u8     RadioPrio;          // Radio Priority
  u8     Pfi;                // Packet flow identifier
  u8     CurrentProc;        // Used to determine the processing sequence
}PdpCtx_t;


// Global SM Data
typedef struct
{

#if !(STK_MAX_SIM==1)
  u32             nCurSimId;		     //Current used SimId for 221. added by akui 20090313.
#endif

  Msg_t         *RecMsg;                // Received message
  Msg_t         *OutMsg;                // Message to send

  InEvtDsc_t    *InEvtDsc;              // Incoming Event
  OutEvtDsc_t   *OEvtDsc;               // Outgoing Event
  bool          KeepMsg;                // TRUE if RecMsg must not be deleted
  u8            NSapiIdx;               // Current NSAPI index (0 to 10)
  bool          TiExtMechanism;         // Transaction identifier extension mechanism
  u16           AllocatedTi;            // SM-SM allocated Ti
  PdpCtx_t      *PtPdpCtx[11];          // Pointer on Pdp Context indexed by NSapiIdx
  Msg_t         *StoredSmPdpCtxActivateReq; // Used for activation of an already activated pdp ctx
#ifdef  __MINQOS__
  bool          SetMinQoS;
  u8            MinQoSLen;              // Length of the minimum Quality of service
  u8            MinQoS[SM_MAX_QOS_LENGTH];             // Minimum Quality of Service
#endif
}sm_Data_t;

//============================================================================
//  Data declaration
//============================================================================
#if !(STK_MAX_SIM==1)
DefExtern sm_Data_t    sm_Data[STK_MAX_SIM];
#else
DefExtern sm_Data_t    sm_Data;
#endif

DefExtern sm_Data_t   *psm_Data;

DefExtern u8           sm_TabMobileOrgTI[64];
DefExtern u8           sm_TabNetworkOrgTI[64];

#undef DefExtern
#endif // endif __SM_DATA_H__

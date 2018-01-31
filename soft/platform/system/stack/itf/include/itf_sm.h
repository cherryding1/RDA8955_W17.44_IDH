//------------------------------------------------------------------------------
//  $Log: itf_sm.h,v $
//  Revision 1.2  2005/12/29 17:38:44  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : ITF_SM.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by the SM entity
  History    :
--------------------------------------------------------------------------------
  Jun 30 04  |  PCA   | Creation
================================================================================
*/

#ifndef __ITF_SM_H__
#define __ITF_SM_H__

#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_msgc.h"

#include "itf_api.h"   // Match SM itf from here


//============================================================================
// SM Internal msg Start at INTRABASE + 0
//============================================================================
#define SM_INTRA_BASE        ( HVY_SM + 0x8000 )




//============================================================================
// Primitive ID definitions for SNDCP <=> SM Messages
//============================================================================
#define SM_ACTIVATE_IND          ( HVY_SM + 0  )
#define SM_ACTIVATE_RSP          ( HVY_SM + 1  )
#define SM_DEACTIVATE_IND        ( HVY_SM + 2  )
#define SM_DEACTIVATE_RSP        ( HVY_SM + 3  )
#define SM_MODIFY_IND            ( HVY_SM + 4  )
#define SM_MODIFY_RSP            ( HVY_SM + 5  )
#define SM_STATUS_REQ            ( HVY_SM + 6  )
#define SM_SEQUENCE_IND          ( HVY_SM + 7  )
#define SM_SEQUENCE_RSP          ( HVY_SM + 8  )

//============================================================================
// Primitive ID definitions for SNDCP <=> API Messages
//============================================================================
// API -> SND
#define SM_PDP_CTXACTIVATE_REQ   ( HVY_SM + 9  )
#define SM_PDP_CTXMODIFY_REQ     ( HVY_SM + 10 )
#define SM_PDP_CTXMODIFY_ACC     ( HVY_SM + 11 )
#define SM_PDP_CTXDEACTIVATE_REQ ( HVY_SM + 12 )
#define SM_PDP_SETMINQOS_REQ     ( HVY_SM + 13 )

// SND -> API
#define SM_PDP_CTXACTIVATE_CNF   ( HVY_SM + 14  )
#define SM_PDP_CTXSMINDEX_IND    ( HVY_SM + 15  )
#define SM_PDP_CTXACTIVATE_REJ   ( HVY_SM + 16  )
#define SM_PDP_CTXACTIVATE_IND   ( HVY_SM + 17  )
#define SM_PDP_CTXMODIFY_CNF     ( HVY_SM + 18  )
#define SM_PDP_CTXMODIFY_REJ     ( HVY_SM + 19  )
#define SM_PDP_CTXMODIFY_IND     ( HVY_SM + 20  )
#define SM_PDP_CTXDEACTIVATE_IND ( HVY_SM + 21  )
#define SM_PDP_CTXDEACTIVATE_CNF ( HVY_SM + 22  )

//============================================================================
// Message definitions  for SNDCP <=> SM
//============================================================================

/*
================================================================================
  Structure  : sm_ActivateInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> SNDCP
  Message    : sm_Activate
  Scope      :
================================================================================
*/
typedef struct
{
 u8             NSapi;
 u8             DataComp;
 u8             HeaderComp;
 u8             QoS[2]; // Only two first bytes of the QoS are used by sndcp
 u8             Pfi;
 u8             Sapi;
 u8             RadioPrio;
} sm_ActivateInd_t;

/*
================================================================================
  Structure  : sm_ActivateRsp_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> SM
  Message    : sm_ActivateRsp
  Scope      :
================================================================================
*/
#define SM_ACTIVATE_FAILURE 0
#define SM_ACTIVATE_SUCCESS 1
typedef struct
{
 u8         NSapi;
 u8         Status;
} sm_ActivateRsp_t;

/*
================================================================================
  Structure  : sm_DeactivateInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> SNDCP
  Message    : sm_DeactivateInd
  Scope      :
================================================================================
*/
typedef struct
{
 u8         NSapi;
} sm_DeactivateInd_t;

/*
================================================================================
  Structure  : sm_PdpSetMinQoSReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SM
  Scope      : to configure a minimum QoS
================================================================================
*/
typedef struct
{
  bool  Cfg              ;
  u8    QoSLen           ;
  u8    QoS[11]          ;
} sm_PdpSetMinQoSReq_t;

/*
================================================================================
  Structure  : sm_DeactivateRsp_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> SM
  Message    : sm_DeactivateRsp
  Scope      :
================================================================================
*/
typedef struct
{
 u8         NSapi;
} sm_DeactivateRsp_t;

/*
================================================================================
  Structure  : sm_ModifyInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> SNDCP
  Message    : sm_ModifyInd
  Scope      :
================================================================================
*/
typedef struct
{
 u8         NSapi;
 u8         QoS[2]; // Only two first bytes of the QoS are used by sndcp
 u8         Pfi;
 u8         Sapi;
 u8         Local;  // TRUE => local, FALSE => ModifyInd is initiated by the network
 u8         RadioPrio; // The radio priority of a sapi cannot be changed but this parameter
                       // is usefull for the creation of a sapi
} sm_ModifyInd_t;

/*
================================================================================
  Structure  : sm_ModifyRsp_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> SM
  Message    : sm_ModifyRsp
  Scope      :
================================================================================
*/
#define SM_MODIFY_FAILURE 0
#define SM_MODIFY_SUCCESS 1
typedef struct
{
 u8         NSapi;
 u8         Status;
} sm_ModifyRsp_t;

/*
================================================================================
  Structure  : sm_StatusReq_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> SM
  Message    : sm_StatusReq
  Scope      :
================================================================================
*/
#define SM_LLC_RELEASE_IND 0
// LLC has indicated a release for the sapi associated with this NSapi. SNDCP
// send this cause for each nsapi in ack mode associated to this sapi. The cause
// of the release is either receiving of a DM/DISC frame or no response for
// a XID REQ. SNDCP don't try to re-establish the LLC link. SNDCP is no more able
// transfer acknoledge or unacknoledge data on this nsapi

typedef struct
{
 u8         NSapi;
 u8         Cause;
} sm_StatusReq_t;

/*
================================================================================
  Structure  : sm_SequenceInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> SNDCP
  Message    : sm_Sequence
  Scope      :
================================================================================
*/
typedef struct
{
 u8         NSapi;
 u8         NPDUNumber;
} sm_SequenceInd_t;

/*
================================================================================
  Structure  : sm_SequenceRsp_t
--------------------------------------------------------------------------------
  Direction  : SM -> SNDCP
  Message    : sm_Sequence
  Scope      :
================================================================================
*/
typedef struct
{
 u8         NSapi;
 u8         NPDUNumber;
} sm_SequenceRsp_t;


//============================================================================
// Message definitions  for API <=> SM
//============================================================================

/*
================================================================================
  Structure  : sm_PdpCtxActivateReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SM
  Scope      : to activate a PDP context
================================================================================
*/
typedef api_PdpCtxActivateReq_t sm_PdpCtxActivateReq_t;

/*
================================================================================
  Structure  : sm_PdpCtxModifyReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SM
  Scope      : to request a change of QoS on the active PDP context
================================================================================
*/
typedef api_PdpCtxModifyReq_t sm_PdpCtxModifyReq_t;

/*
================================================================================
  Structure  : sm_PdpCtxModifyAcc_t
--------------------------------------------------------------------------------
  Direction  : API -> SM
  Scope      : to accept the PDP context modification requested by the network
================================================================================
*/
typedef api_PdpCtxModifyAcc_t sm_PdpCtxModifyAcc_t;

/*
================================================================================
  Structure  : sm_PdpCtxDeactivateReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SM
  Scope      : to de-activate the active PDP context
================================================================================
*/
typedef api_PdpCtxDeactivateReq_t sm_PdpCtxDeactivateReq_t;

/*
================================================================================
  Structure  : sm_PdpCtxActivateCnf_t
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to confirm that the requested PDP context was successfully
               activated
================================================================================
*/
typedef api_PdpCtxActivateCnf_t sm_PdpCtxActivateCnf_t;

/*
================================================================================
  Structure  : sm_PdpCtxSmIndexInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : Indicates SmIndex after a api_PdpCtxActivateReq
================================================================================
*/
typedef api_PdpCtxSmIndexInd_t sm_PdpCtxSmIndexInd_t;

/*
================================================================================
  Structure  : sm_PdpCtxDeactivateCnf_t
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to confirm that the PDP context was successfully deactivated
================================================================================
*/
typedef api_PdpCtxDeactivateCnf_t sm_PdpCtxDeactivateCnf_t;

/*
================================================================================
  Structure  : sm_PdpCtxActivateRej_t
--------------------------------------------------------------------------------
  Direction  : SM <->API
  Scope      : to indicate a failure during pdp context activation
================================================================================
*/
typedef api_PdpCtxActivateRej_t sm_PdpCtxActivateRej_t;

/*
================================================================================
  Structure  : sm_PdpCtxModifyRej_t
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to indicate a failure during pdp context modification
================================================================================
*/
typedef api_PdpCtxModifyRej_t sm_PdpCtxModifyRej_t;

/*
================================================================================
  Structure  : sm_PdpCtxActivateInd_t
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to indicate that the network wishes to activate a PDP context
================================================================================
*/
typedef api_PdpCtxActivateInd_t sm_PdpCtxActivateInd_t;

/*
================================================================================
  Structure  : sm_PdpCtxModifyCnf
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to confirm that the active PDP context was successfully modified
================================================================================
*/
typedef api_PdpCtxModifyCnf_t sm_PdpCtxModifyCnf_t;

/*
================================================================================
  Structure  : sm_PdpCtxModifyInd
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to indicate that the network wishes to change the QoS for the
               active PDP context
================================================================================
*/
typedef api_PdpCtxModifyInd_t sm_PdpCtxModifyInd_t;

/*
================================================================================
  Structure  : sm_PdpCtxDeactivateInd
--------------------------------------------------------------------------------
  Direction  : SM -> API
  Scope      : to indicate that the PDP context has been deactivated by the
               network
================================================================================
*/
typedef api_PdpCtxDeactivateInd_t sm_PdpCtxDeactivateInd_t;

#endif  // __ITF_SM_H__

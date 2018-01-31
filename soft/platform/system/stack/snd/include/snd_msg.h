//------------------------------------------------------------------------------
//  $Log: snd_msg.h,v $
//  Revision 1.3  2006/05/23 14:37:18  aba
//  Update of the copyright
//
//  Revision 1.2  2005/12/29 17:39:17  pca
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
  File       : SND_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for sndcp Task

  History    :
--------------------------------------------------------------------------------
  JUl 15 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SND_MSG_H__
#define __SND_MSG_H__

#include "itf_llc.h"
#include "itf_snd.h"
#include "itf_sm.h"
#include "itf_mm.h"


/*
** SND interlayer headers
*/
typedef union
{
   /*
   ** With API
   */
   snd_SendReq_t                  snd_SendReq                   ;
   snd_DataInd_t                  snd_DataInd                   ;
   /*
   ** With LLC
   */
   llc_EstabReq_t                 llc_EstabReq                  ;
   llc_EstabRsp_t                 llc_EstabRsp                  ;
   llc_EstabCnf_t                 llc_EstabCnf                  ;
   llc_EstabInd_t                 llc_EstabInd                  ;
   llc_DataReq_t                  llc_DataReq                   ;
   llc_UnitDataReq_t              llc_UnitDataReq               ;
   llc_XidInd_t                   llc_XidInd                    ;
   llc_XidReq_t                   llc_XidReq                    ;
   llc_XidCnf_t                   llc_XidCnf                    ;
   llc_XidRsp_t                   llc_XidRsp                    ;
} IlHdr_t ;


#include "itf_il.h" // interlayer body definition

#ifdef __BORLANDC__
#if ( (sizeof(snd_SendReq_t) + sizeof(snd_NSapiPar)) > ILHDR_SPECIFIC_SIZE)
#error (snd_SendReq_t + snd_NSapiPar) exceed ILHDR_SPECIFIC_SIZE
#endif
#endif

/*
** SNDCP messages body
*/
typedef union
{
   sm_ActivateInd_t               sm_ActivateInd                ;
   sm_ActivateRsp_t               sm_ActivateRsp                ;
   sm_DeactivateInd_t             sm_DeactivateInd              ;
   sm_DeactivateRsp_t             sm_DeactivateRsp              ;
   sm_ModifyInd_t                 sm_ModifyInd                  ;
   sm_ModifyRsp_t                 sm_ModifyRsp                  ; 
   sm_StatusReq_t                 sm_StatusReq                  ;
   mmp_SequenceInd_t              mmp_SequenceInd               ;
   mmp_SequenceRsp_t              mmp_SequenceRsp               ;
   llc_DataCnf_t                  llc_DataCnf                   ;
   llc_ReleaseReq_t               llc_ReleaseReq                ;
   llc_ReleaseCnf_t               llc_ReleaseCnf                ;
   llc_ReleaseInd_t               llc_ReleaseInd                ;
   llc_QueueCreateInd_t           llc_QueueCreateInd            ;
   llc_ChangeLlcParReq_t          llc_ChangeLlcParReq           ;
   snd_QueueCreateInd_t           snd_QueueCreateInd            ;
   /*
   ** InterLayer interfaces
   */
   IlBody_t                       IL                            ;
} MsgBody_t;

#include "itf_msg.h"  // message definition

#endif

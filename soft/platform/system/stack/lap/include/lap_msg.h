//------------------------------------------------------------------------------
//  $Log: lap_msg.h,v $
//  Revision 1.3  2006/05/22 12:46:12  aba
//  Copyrigth is updated
//
//  Revision 1.2  2005/12/29 17:38:45  pca
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
  File       : LAP_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for lap Task

  History    :
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __LAP_MSG_H__
#define __LAP_MSG_H__

#include "itf_l1.h"
#include "itf_lap.h"

/*
** Shortcut to access fields in Interlayer messages
*/
#define LAP_ILS  B.IL.Hdr.Specific  // specific header
#define LAP_ILD  B.IL.Data          // Radio Data


/*
** LAP interlayer headers
*/
typedef union
{
   /*
   ** With RR
   */
   lap_EstablishReq_t             lap_EstablishReq              ;
   lap_DataReq_t                  lap_DataReq                   ;
   lap_UnitDataReq_t              lap_UnitDataReq               ;
   lap_DataInd_t                  lap_DataInd                   ;
   lap_UnitDataInd_t              lap_UnitDataInd               ;
   lap_ReconnectReq_t             lap_ReconnectReq              ;
   lap_ResumeReq_t                lap_ResumeReq                 ;
   /*
   ** With L1
   */
   l1_DataReq_t                   l1_DataReq                    ;
} IlHdr_t ;


#include "itf_il.h" // interlayer body definition

/*
** LAP messages body
*/
typedef union
{
   lap_EstablishFailureInd_t      lap_EstablishFailureInd       ;
   lap_ResumeFailureInd_t         lap_ResumeFailureInd          ;
   lap_ReconnectFailureInd_t      lap_ReconnectFailureInd       ;
   lap_ResumeReq_t                lap_ResumeReq                 ;
   lap_NormalReleaseReq_t         lap_NormalReleaseReq          ;
   lap_NormalReleaseCnf_t         lap_NormalReleaseCnf          ;
   lap_NormalReleaseFailureInd_t  lap_NormalReleaseFailureInd   ;
   lap_LocalReleaseReq_t          lap_LocalReleaseReq           ;
   lap_ReleaseInd_t               lap_ReleaseInd                ;
   lap_SuspendReq_t               lap_SuspendReq                ;
   lap_EstablishCnf_t             lap_EstablishCnf              ;
   l1_TxInd_t                     l1_TxInd                      ;
   lap_GenericPartMsg_t           lap_GenericPartMsg            ;
   /*
   ** InterLayer interfaces
   */
   IlBody_t                       IL                            ;
} MsgBody_t;

#include "itf_msg.h"  // message definition

#endif

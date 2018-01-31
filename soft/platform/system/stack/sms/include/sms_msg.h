//------------------------------------------------------------------------------
//  $Log: sms_msg.h,v $
//  Revision 1.5  2006/05/22 07:56:44  aba
//  Copyrigth is updated
//
//  Revision 1.4  2006/03/02 14:23:48  aba
//  Bad MM connection release
//
//  Revision 1.3  2006/02/28 16:15:46  aba
//  Issue 360: New SMS MO sending impossible after wrong SC address used (CM_Service_Rej) => Send of CP_ERROR during abort
//  Add PDTI in MM_RELEASE_REQ
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : SMS_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for sms Task

  History    :
--------------------------------------------------------------------------------
  Feb 17 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SMS_MSG_H__
#define __SMS_MSG_H__

#include "itf_mm.h"
#include "itf_sms.h"

/*
** SMS interlayer headers
*/
typedef union
{
   /*
   ** With API
   */
   smspp_SendReq_t                smspp_SendReq                 ;
   smspp_ReceiveInd_t             smspp_ReceiveInd              ;
   smspp_AckReq_t                 smspp_AckReq                  ;
   smspp_ErrorReq_t               smspp_ErrorReq                ;
   smspp_ErrorInd_t               smspp_ErrorInd                ;
   /*
   ** With MM
   */
#ifdef __GPRS__
   mmp_DataReq_t                  mmp_DataReq                   ;
#endif // __GPRS__
} IlHdr_t ;


#include "itf_il.h" // interlayer body definition

/*
** SMS messages body
*/
typedef union
{
   /*
   ** InterLayer interfaces
   */
   IlBody_t                       IL                            ;
   smspp_SendMMAReq_t             smspp_SendMMAReq              ;
   smspp_AbortReq_t                   smspp_AbortReq          ;
   
   mm_EstabReq_t                  mm_EstabReq                   ;
   mm_PDTIInd_t                   mm_ReleaseReq                 ;
   mm_ReleaseInd_t                mm_ReleaseInd                 ;
   mm_PDTIInd_t                   mm_EstabInd                   ;
} MsgBody_t;

#include "itf_msg.h"  // message definition

#endif

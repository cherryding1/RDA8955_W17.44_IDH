//------------------------------------------------------------------------------
//  $Log: snd_uti.h,v $
//  Revision 1.6  2006/05/23 14:37:18  aba
//  Update of the copyright
//
//  Revision 1.5  2006/02/23 10:59:20  aba
//  Additional commentaries
//
//  Revision 1.4  2005/12/30 14:40:35  aba
//  Change footer size for PPP
//
//  Revision 1.3  2005/12/30 09:14:06  pca
//  Add Footer and Header for PPP
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
  File       : SND_UTI.H
--------------------------------------------------------------------------------

  Scope      : Functions declaration of snd_uti.c

  History    :
--------------------------------------------------------------------------------
  Jul 20 04  |  ABA   | Creation
================================================================================
*/

#ifndef __SND_UTI_H__
#define __SND_UTI_H__

#include "sxr_ops.h"
#include "cmn_defs.h"

#define SND_API_FOOTER_SIZE 2
#define SND_API_HEADER_SIZE 1

extern void snd_ReleaseAllUnackSeg        (void)                       ;
extern void snd_ReleaseAllAckSeg          (void)                       ;
extern void snd_Reassemble                (u8 LagNb, bool Mode)        ;
extern void snd_GetNSapiCtx               (u8 NSndex)                  ;
extern void snd_ReleaseNSapiCtx           (u8 NSapiIndex)              ;
extern void snd_BuildUnitDataReqSpecific  (Msg_t* PtMsg)               ;
extern void snd_RetransmitBufNPDU         (void)                       ;
extern void snd_SendDataReq               (Msg_t* PtRxMsg)             ;
extern void snd_SendUnitDataReq           (Msg_t* PtRxMsg);
extern void snd_ReestabReq                (void)                       ;
extern u8   snd_Purge                     (void * var, void * PtMsg )  ;
extern void snd_ResetXidParameter         (void);
extern void snd_ResumeUnackTransmission   (void);
extern void snd_RAUInterRetransmission    (u8 SeqIndNpduNb);
extern bool snd_DecodeXidCnf              (u8* PtXidData, u8* PtLastData);
#ifdef __SND_UNACK_ENHANCE__
extern bool snd_CheckAndsndReassemble(u8 mode);
#endif
#endif  // __SND_UTI_H__



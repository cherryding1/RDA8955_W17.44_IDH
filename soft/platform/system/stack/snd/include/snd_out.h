//------------------------------------------------------------------------------
//  $Log: snd_out.h,v $
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
  File       : snd_out.h
--------------------------------------------------------------------------------
  Scope      : Prototypes for the snd_out.c utilities
  History    :
--------------------------------------------------------------------------------
  Jul 15 04  | ABA    | Creation
================================================================================
*/
#ifndef __SND_OUT_H__
#define __SND_OUT_H__

// SNCDP includes
#include "snd_msg.h"

void snd_PrepMsg (OutEvtDsc_t * Evt,  u16 Len );
void snd_SendMsg ( u8 Mbx ) ;
void  snd_SendPduToAPI(Msg_t*Msg);

#endif // __SND_OUT_H__

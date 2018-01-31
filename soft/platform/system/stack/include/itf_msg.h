//------------------------------------------------------------------------------
//  $Log: itf_msg.h,v $
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
  File       : itf_msg.h
--------------------------------------------------------------------------------

  Scope      : Definition of a Message        

  History    :
--------------------------------------------------------------------------------
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __ITF_MSG_H__
#define __ITF_MSG_H__

/*
** This file has to be included in each task after body definition 
*/

#include "itf_msgc.h"

typedef struct 
{
   MsgHead_t   H;
   MsgBody_t   B;

}Msg_t;

#endif

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
  File       : api_out.h
--------------------------------------------------------------------------------

  Scope      : Header file for api_out.c

  History    :
--------------------------------------------------------------------------------
  Mar 10 04  |  PCA   | Creation            
================================================================================
*/

#ifndef __API_OUT_H__
#define __API_OUT_H__

// System includes
#include "sxs_type.h"
#include "fsm.h"

void  api_PrepMsg (OutEvtDsc_t * Evt, u16 Len);
void  api_SendMsg (u8 Mbx);
void  api_PrepSmsMsg (OutEvtDsc_t * Evt, u16 Len);


#endif // __API_OUT_H__

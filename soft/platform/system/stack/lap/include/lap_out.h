//------------------------------------------------------------------------------
//  $Log: lap_out.h,v $
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
  File       : lap_out.h
--------------------------------------------------------------------------------
  Scope      : Prototypes for the lap_out.c utilities
  History    :
--------------------------------------------------------------------------------
  Sep 02 03  | ABA    | Creation
================================================================================
*/
#ifndef __LAP_OUT_H__
#define __LAP_OUT_H__

// LAP includes
#include "lap_msg.h"

void lap_SendMsg ( u8 Mbx ) ;
void lap_PrepMsg (OutEvtDsc_t * Evt, u16 Len );

#endif // __LAP_OUT_H__

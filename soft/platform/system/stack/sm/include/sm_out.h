//------------------------------------------------------------------------------
//  $Log: sm_out.h,v $
//  Revision 1.3  2006/05/22 19:35:41  aba
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
  File       : sm_out.h
--------------------------------------------------------------------------------

  Scope      : Header file for sm_out.c

  History    :
--------------------------------------------------------------------------------
 Aug 13 04  |  PCA   | Creation
================================================================================
*/
#ifndef __SM_OUT_H__
#define __SM_OUT_H__

// System includes
#include "sxr_ops.h"
#include "sxs_io.h"
#include "cmn_defs.h"
#include "fsm.h"

// SM specific includes
#include "sm_data.h"

void  sm_PrepMsg (OutEvtDsc_t * Evt,  u16 Len );
void  sm_SendMsg (u8 Mbx);

#endif


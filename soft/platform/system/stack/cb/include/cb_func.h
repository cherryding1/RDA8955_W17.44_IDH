/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/
//------------------------------------------------------------------------------
//  $Log: cb_func.h,v $
//  Revision 1.3  2006/05/23 10:18:35  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:40  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is not property of StackCom.
  This source code is from the public domain.
  The information contained in this file is not confidential.
  Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is allowed.

================================================================================
*/
/*
================================================================================

  File       : CB_FUNC.H
--------------------------------------------------------------------------------

  Scope      : Protoypes of CB functions

  History    :
--------------------------------------------------------------------------------
  Dec 07 04  |  FCH   | Creation            
================================================================================
*/

#ifndef __CB_FUNC_H__
#define __CB_FUNC_H__

#include "sxr_ops.h"


void    cb_PrepMsg            ( OutEvtDsc_t *, u16 );
void    cb_SendMsg            ( u8 Mbx);

void    cb_UpdCellPar         ( void );
void    cb_SndCbchReq         ( bool );
void    cb_SaveCfgLists       ( void );
void    cb_ReleaseCfgLists    ( void );
bool    cb_StopRcvSmsCbMsg    ( void );
void    cb_ClearSmsCbCtx      ( void );
void    cb_DelSmsCbCtx        ( u8 );
void    cb_ProcessCbMsg       ( void );
void    cb_DelHistory         ( u8 );
void    cb_AddInHistory       ( void );
void    cb_SndSkipReq         ( void );
bool    cb_StopRcvSchedMsg    ( void );
void    cb_FreeSchedCtx       ( void );
void    cb_CheckSchedValidity ( void );
void    cb_UpdtSchedCtx       ( void );
u8      cb_GetRelSlot         ( void );
u8      cb_HdleMsgDsc         ( u8 *, u8 );
bool    cb_IsWantedMId        ( u16 );
bool    cb_IsWantedDCS        ( u8 );

#ifdef __CB_SPY__
void    cb_InitSpy            ( void );
void    cb_UpdateSpy          ( void );
#endif

#endif // __CB_FUNC_H__

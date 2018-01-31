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
//  $Log: rlp_func.h,v $
//  Revision 1.4  2006/05/23 10:25:35  fch
//  Update Copyright
//
//  Revision 1.3  2006/01/27 10:14:45  fch
//  Handling of RLP system parameters provides by user and XID command sending
//
//  Revision 1.2  2005/12/29 17:39:12  pca
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
  File       : RLP_FUNC.H
--------------------------------------------------------------------------------

  Scope      : Protoypes of RLP functions

  History    :
--------------------------------------------------------------------------------
  Mar 31 05  |  FCH   | Creation            
================================================================================
*/

#ifndef __RLP_FUNC_H__
#define __RLP_FUNC_H__

#include "sxr_ops.h"
#include "cmn_defs.h"
#include "rlp_defs.h"


/*
================================================================================
  rlp_evt.c 
================================================================================
*/
InEvtDsc_t      *rlp_GiveDsc                ( u32 );


/*
================================================================================
  rlp_rout.c 
================================================================================
*/
void            rlp_L1DataInd               ( void );

/*
================================================================================
  rlp_util.c 
================================================================================
*/
bool            rlp_IsFcsCorrect            ( u8 *, u16 );
void            rlp_SendFrm                 ( void );
void            rlp_BuildUFrm               ( u8, u8, u8, Msg_t * );
void            rlp_InitLinkVars            ( void );
bool            rlp_SendTxU                 ( void );
void            rlp_ClearLinkVars           ( void );
void            rlp_StartTimer              ( u32 );
void            rlp_StopTimer               ( u32 );
rlp_XidBitMap_t rlp_DecXidPar               ( rlp_SysPar_t * );
void            rlp_BuildXidPdu             (rlp_XidBitMap_t, rlp_SysPar_t *);
void            rlp_StoreXIDRsp             ( rlp_CrlpReq_t * );
void            rlp_SndConnectCnf           ( u8 );
void            rlp_ProcessDiscReq          ( void );
void            rlp_SndFailInd              ( void );
void            rlp_StoreInQ                ( Msg_t **, Msg_t ** );
Msg_t*          rlp_GetFromQ                ( Msg_t **, Msg_t ** );
void            rlp_PurgeQ                  ( Msg_t **, Msg_t ** );
void            rlp_SndFlowCtrlInd          ( u8 );


/*
================================================================================
  rlp_i_s.c 
================================================================================
*/
void            rlp_SHandler                ( u8 );
bool            rlp_IHandler                ( void );
void            rlp_ResetRxSlot             ( u8, u8 );
bool            rlp_IsSREJ2Send             ( u8 * );
u8              rlp_BuildXXResp             ( u8 );
u8              rlp_BuildI_SCmd             ( u8, u8 );


/*
================================================================================
  rlp_out.c 
================================================================================
*/
void            rlp_PrepMsg                 ( u16 );
void            rlp_SendMsg                 ( u8 );


#endif // __RLP_FUNC_H__

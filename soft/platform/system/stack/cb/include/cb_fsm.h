//------------------------------------------------------------------------------
//  $Log: cb_fsm.h,v $
//  Revision 1.3  2006/05/23 10:36:37  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:40  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/
/*
================================================================================

  File       : CB_FSM.H
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for CB

  History    :
--------------------------------------------------------------------------------
  Dec 07 04  |  FCH   | Creation
================================================================================
*/
#ifndef __CB_FSM_H__
#define __CB_FSM_H__

#include "fsm.h"

//============================
//  SMSCB states definition
//============================
#define CB_GENE       0   // Generic processing
#define CB_NULL       1   // SMS-CB not activated
#define CB_IDLE       2   // SMS-CB activated and ready to receive
#define CB_RXBLK      3   // Block reception



//============================
// FSM context data
//============================
#if !(STK_MAX_SIM==1)
extern Fsm_t cb_Fsm[STK_MAX_SIM];
#else
extern Fsm_t cb_Fsm;
#endif
//============================
// Transitions prototypes
//============================
extern u8 cb_TrsDoNothing       ( void );
extern u8 cbGene_CellInd        ( void );
extern u8 cbGene_DeactivateReq  ( void );
extern u8 cbGene_ActivateReq    ( void );


extern void cb_InitNull         ( void );
extern u8 cbNull_CellInd        ( void );
extern u8 cbNull_ActivateReq    ( void );
extern u8 cbNull_DeactivateReq  ( void );

extern void cb_InitIdle         ( void );
extern u8 cbIdle_DataInd        ( void );
                                         
extern u8 cbRxBlk_DataInd       ( void );
                                         
                                         
                                         
#endif // endif __CB_FSM_H__




//------------------------------------------------------------------------------
//  $Log: rlp_fsm.h,v $
//  Revision 1.5  2006/05/23 10:37:37  fch
//  Update Copyright
//
//  Revision 1.4  2006/01/27 15:13:34  fch
//  Added interrogation from API of the current RLP system parameters settings
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

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/
/*
================================================================================
  File       : RLP_FSM.H
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for RLP

  History    :
--------------------------------------------------------------------------------
  Mar 31 05  |  FCH   | Creation
================================================================================
*/
#ifndef __RLP_FSM_H__
#define __RLP_FSM_H__

#include "fsm.h"

//============================
//  RLP states definition
//============================
#define RLP_GENE    ( 0              )
#define RLP_ADMDET  ( 1 | FSM_STABLE )    // ADM Detached
#define RLP_ADMATT  ( 2 | FSM_STABLE )    // ADM Attached
#define RLP_OCON    ( 3 | FSM_STABLE )    // Outgoing establishment (MS initiated)
//  RLP_ICON,      // Incoming establishment
#define RLP_ABM     ( 4 | FSM_STABLE )    // Asynchronous Balanced Mode
#define RLP_ODISC   ( 5 | FSM_STABLE )    // Outgoing Disconnection (MS initiated)



//============================
// FSM context data
//============================
extern Fsm_t rlp_Fsm;

//============================
// Transitions prototypes
//============================
u8    rlp_TrsDoNothing          ( void );

u8    rlpGene_RxTEST            ( void );
u8    rlpGene_RxXID             ( void );
u8    rlpGene_RxUI              ( void );
u8    rlpGene_UDataReq          ( void );
u8    rlpGene_T1_XIDExp         ( void );
u8    rlpGene_DetachReq         ( void );
u8    rlpGene_CrlpReq           ( void );
u8    rlpGene_CrlpRdReq         ( void );

void  rlp_InitADMDET            ( void );
u8    rlpAdmDet_ConfReq         ( void );
u8    rlpAdmDet_AttachReq       ( void );
u8    rlpAdmDet_RxSABM          ( void );
u8    rlpAdmDet_RxDISC          ( void );
u8    rlpAdmDet_SndOpp          ( void );
u8    rlpAdmDet_CrlpReq         ( void );
u8    rlpAdmDet_CrlpRdReq       ( void );

void  rlp_InitADMATT            ( void );
u8    rlpAdmAtt_ConnectReq      ( void );
u8    rlpAdmAtt_RxSABM          ( void );
u8    rlpAdmAtt_RxDISC          ( void );
u8    rlpAdmAtt_SndOpp          ( void );

u8    rlpOcon_SndOpp            ( void );
u8    rlpOcon_RxSABM            ( void );
u8    rlpOcon_RxUA              ( void );
u8    rlpOcon_RxDM              ( void );
u8    rlpOcon_RxDISC            ( void );
u8    rlpOcon_DiscReq           ( void );
u8    rlpOcon_T1_SABMExp        ( void );

u8    rlpAbm_DiscReq            ( void );
u8    rlpAbm_ResetReq           ( void );
u8    rlpAbm_SndOpp             ( void );
u8    rlpAbm_RxDISC             ( void );
u8    rlpAbm_RxSABM             ( void );
u8    rlpAbm_DataReq            ( void );
u8    rlpAbm_RxSFrm             ( void );
u8    rlpAbm_RxIFrm             ( void );
u8    rlpAbm_IFrmT1Exp          ( void );
u8    rlpAbm_T1_RCVSExp         ( void );
u8    rlpAbm_T1_RCVRExp         ( void );
u8    rlpAbm_FlowCtrlReq        ( void );

void  rlp_InitODISC             ( void );
u8    rlpOdisc_RxDISC           ( void );
u8    rlpOdisc_RxUAorDM         ( void );
u8    rlpOdisc_T1_DISCExp       ( void );
u8    rlpOdisc_SndOpp           ( void );


#endif // __RLP_FSM_H__




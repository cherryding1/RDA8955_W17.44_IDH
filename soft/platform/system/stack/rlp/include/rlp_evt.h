//------------------------------------------------------------------------------
//  $Log: rlp_evt.h,v $
//  Revision 1.4  2006/05/23 10:37:37  fch
//  Update Copyright
//
//  Revision 1.3  2006/01/27 15:13:34  fch
//  Added interrogation from API of the current RLP system parameters settings
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
  File       : RLP_EVT.H
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of RLP Task

  History    :
--------------------------------------------------------------------------------
  Mar 31 05  |  FCH   | Creation
================================================================================
*/
#ifndef __RLP_EVT_H__
#define __RLP_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition


//extern InEvtDsc_t *rlp_DataIndEvtList [RLP_NB_FRM_TYP];


      //-------------------------------------------------------
      //  Flags for the events
      //-------------------------------------------------------
#define RLP_NO_FLAG     0
#define RLP_IL_MSG      ( 1 << 0 )  // For interlayer messages


//==========================================
//             Incoming Events
//==========================================




extern InEvtDsc_t  rlp_L1DataIndEvt;





extern InEvtDsc_t  rlp_L2rConfReqEvt;
extern InEvtDsc_t  rlp_L2rAttReqEvt;
extern InEvtDsc_t  rlp_L2rDetReqEvt;
extern InEvtDsc_t  rlp_L2rConnectReqEvt;
extern InEvtDsc_t  rlp_L2rDataReqEvt;
extern InEvtDsc_t  rlp_L2rUDataReqEvt;
extern InEvtDsc_t  rlp_L2rResetReqEvt;
extern InEvtDsc_t  rlp_L2rDiscReqEvt;
extern InEvtDsc_t  rlp_L2rFlowCtrlReqEvt;





extern InEvtDsc_t  rlp_ApiCrlpReqEvt;
extern InEvtDsc_t  rlp_ApiCrlpRdReqEvt;



/*
** TIMERS
*/
extern InEvtDsc_t  rlp_T1_SABM_DISCExpEvt;
extern InEvtDsc_t  rlp_T1_XIDExpEvt;
extern InEvtDsc_t  rlp_IFrmT1ExpEvt;
extern InEvtDsc_t  rlp_T1_RCVRExpEvt;
extern InEvtDsc_t  rlp_T1_RCVSExpEvt;


/*
** DERIVED EVENTS :
*/
extern InEvtDsc_t  rlp_RxSFrameEvt;
extern InEvtDsc_t  rlp_RxIFrameEvt;
extern InEvtDsc_t  rlp_RxU_UIFrameEvt;
extern InEvtDsc_t  rlp_RxU_DMFrameEvt;
extern InEvtDsc_t  rlp_RxU_SABMFrameEvt;
extern InEvtDsc_t  rlp_RxU_DISCFrameEvt;
extern InEvtDsc_t  rlp_RxU_UAFrameEvt;
extern InEvtDsc_t  rlp_RxU_NULLFrameEvt;
extern InEvtDsc_t  rlp_RxU_XIDFrameEvt;
extern InEvtDsc_t  rlp_RxU_TESTFrameEvt;

extern InEvtDsc_t  rlp_SendOppEvt;


//==========================================
//             Outgoing Events
//==========================================




extern OutEvtDsc_t  rlp_L1DataReqEvt;





extern OutEvtDsc_t rlp_RlpConnectCnfEvt;
extern OutEvtDsc_t rlp_RlpConnectIndEvt;
extern OutEvtDsc_t rlp_RlpDataIndEvt;
extern OutEvtDsc_t rlp_RlpUDataIndEvt;
extern OutEvtDsc_t rlp_RlpResetIndEvt;
extern OutEvtDsc_t rlp_RlpDiscCnfEvt;
extern OutEvtDsc_t rlp_RlpDiscIndEvt;
extern OutEvtDsc_t rlp_RlpFailIndEvt;
extern OutEvtDsc_t rlp_RlpFlowCtrlIndEvt;




extern OutEvtDsc_t rlp_RlpCrlpRdIndEvt;



#endif  // __RLP_EVT_H__

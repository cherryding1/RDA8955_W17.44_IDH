//------------------------------------------------------------------------------
//  $Log: sm_route.h,v $
//  Revision 1.4  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.3  2006/01/24 11:55:56  aba
//  minimum quality of service
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
  File       : SM_ROUTE.H
--------------------------------------------------------------------------------

  Scope      : prototypes for SM routing functions

  History    :
--------------------------------------------------------------------------------
  Oct 05 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SM_ROUTE_H__
#define __SM_ROUTE_H__

#define M_GET_MSG_TYPE(_PtMsg,_TiExtMechanism)  (_PtMsg->B.IL.Data[_PtMsg->B.IL.DataOffset+1+(_TiExtMechanism==FALSE?0:1)])

#define M_GET_EXTENDED_TI(_PtMsg) (_PtMsg->B.IL.Data[_PtMsg->B.IL.DataOffset+1] & 0x7F)
#define M_GET_TI(_PtMsg) ((_PtMsg->B.IL.Data[_PtMsg->B.IL.DataOffset] & 0x70)>>4)

#define M_GET_TI_FLAG(_PtMsg) (_PtMsg->B.IL.Data[_PtMsg->B.IL.DataOffset]&0x80)

#define M_GET_TI_EXT(_PtMsg) (_PtMsg->B.IL.Data[_PtMsg->B.IL.DataOffset+1]&0x80)

extern void sm_RouteDataInd             (void);
extern void sm_RouteAbortInd            (void);
extern void sm_RouteRauAccPdpStatuInd(void);  // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__
extern void sm_RouteStatusReq           (void);
extern void sm_RoutePdpCtxActivateReq   (void);
extern void sm_RoutePdpCtxActivateRej   (void);
extern void sm_RouteActivateRsp         (void);
extern void sm_RoutePdpCtxDeactivateReq (void);
extern void sm_RoutePdpCtxModifyAcc     (void);
extern void sm_RoutePdpCtxModifyRej     (void);
extern void sm_RouteDeactivateRsp       (void);
extern void sm_RoutePdpCtxModifyReq     (void);
extern void sm_RouteModifyRsp           (void);
#ifdef  __MINQOS__
extern void sm_RoutePdpSetMinQoSreq     (void);
#endif
#endif // __SM_ROUTE_H__


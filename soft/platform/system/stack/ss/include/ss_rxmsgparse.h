////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: ss.c
// 
// DESCRIPTION:
//   Main body for SS Task
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SS_RXMSGPARSE_H_
#define _SS_RXMSGPARSE_H_

#define SS_IMSG_IL            (gp_ssCurData->CurCtx->rxMsgPtr->B.IL)        //reference to start of L3 message 
#define SS_MSG                ((gp_ssCurData->CurCtx->rxMsgPtr->B.IL).Data)
#define SS_MSG_INDEX        (gp_ssCurData->CurCtx->msgIndex)
#if 0
#define RX_COMPONENT        (gp_ssCurData->CurCtx->rxMsgInfo.RxComponentPresent) //The type of Rx Component that was found
#endif
#define INVOKE_ID            (gp_ssCurData->CurCtx->rxMsgInfo.MsgHdr.invokeID)      //The invoke ID that was found

//==============================================================================
//Function    : ss_rxMsgParse
//----------------------------------------------------------------------
//Scope        : Parses a received message
//Parameters    : u8 msgIndex - index to msg field of L3 message
//Return        : The type of component received, parsed message is stored in Ctxt
//==============================================================================
ss_RxMessageRoutingInfo_t ss_rxMsgParse( void );

#endif //end of file

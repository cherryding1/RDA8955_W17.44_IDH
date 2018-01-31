//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlc_defs.h,v 1.2 2006/03/08 00:18:10 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : rlc_defs.h 
--------------------------------------------------------------------------------

  Scope      : Defintion of Constants and Macros for the RLC entities

  History    :
--------------------------------------------------------------------------------
  Feb 09 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLC_DEFS_H__
#define __RLC_DEFS_H__

#include "rlu_msg.h"
#include "itf_il.h"

/* 
** Size of the RLC-MAC header
*/
#define RLC_MAC_HDR_SIZE ( RAD_HDR_MAC_SIZE + 1 )

/*
** SNS value and its modulo
*/
#define RLC_SNS            128
#ifndef __EGPRS__
#define RLC_MOD_SNS(_val)  ((_val) & (RLC_SNS - 1))
#endif
/*
** Window Size
*/
#define RLC_WS             64

#ifdef __EGPRS__
#define RLC_EDGE_SNS 2048
#define RLC_MOD_SNS(_val)  ((_val) & (pRLUCtx->Sns))

#define GET_LITTLE(_val1, _val2)  (_val1<_val2)? _val1 : _val2
#define GET_DIFF(_V1,_V2, _Step) 	(((s16)(_V1-_V2)>=0)?  (_V1- _V2): (_V1-_V2+_Step))

#define COPY_BITS(Dst,Dst_Pos,Src,Src_Pos,Len)\
{\
for( i=0; i<Len; i++,Dst_Pos++)\
	{\
	if(*(Src+((Src_Pos+i)>>3))&(1<<(7-((Src_Pos+i)&7)) ))	*(Dst+(Dst_Pos>>3)) |=1<<(7-(Dst_Pos&7));\
	else *(Dst+(Dst_Pos>>3)) &=~(1<<(7-(Dst_Pos&7)));\
	}\
}

enum SPB_TYPE
{
     NO_RETRANSMISSION = 0,
     RESERVE = 1,
     RET_FIRST_PART = 2,
     RET_SECOND_PART = 3
};
#endif

#endif	


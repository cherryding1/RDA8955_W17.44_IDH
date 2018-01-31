//------------------------------------------------------------------------------
//  $Log: lap_frm.h,v $
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
  File       : LAP_FRM.H
--------------------------------------------------------------------------------

  Scope      : Definitions used to build a frame and send the message to the L1

  History    :
--------------------------------------------------------------------------------
  Oct 10 03  |  ABA   | Creation
================================================================================
*/

#ifndef __LAP_FRM_H__
#define __LAP_FRM_H__

#include "sxr_ops.h"
#include "cmn_defs.h"


#ifdef __LAP_FRM_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

typedef enum
{
  SABM_FRAME    ,
  UA_FRAME      ,
  DM_FRAME      ,
  DISC_FRAME    ,
  RR_FRAME      ,
  REJ_FRAME     ,
  UI_FRAME      ,
  I_FRAME
}FrameType_t;

// Control field
DefExtern const u8 lap_ControlField[8]
#ifdef __LAP_FRM_VAR__
={
0x2f,        // SABM_FRAME
0x63,        // UA_FRAME
0x0f,        // DM_FRAME
0x43,        // DISC_FRAME
0x01,        // RR_FRAME
0x09,        // REJ_FRAME
0x03,        // UI_FRAME
0x00}        // I_FRAME
#endif
;


DefExtern const bool lap_SupervisoryFrame[8]
#ifdef __LAP_FRM_VAR__
={
FALSE,        // SABM_FRAME
FALSE,        // UA_FRAME
FALSE,        // DM_FRAME
FALSE,        // DISC_FRAME
TRUE,        // RR_FRAME
TRUE,        // REJ_FRAME
FALSE,        // UI_FRAME
FALSE}        // I_FRAME
#endif
;


DefExtern u8 lap_Channel;

#define LAP_HEAD_SIZE 3

#define LAP_SET_P    1
#define LAP_SET_F    1
#define LAP_SET_PF   1
#define LAP_RESET_P  0
#define LAP_RESET_F  0
#define LAP_RESET_PF 0

#define M_SUPERVISORY_FRAME(_FrameType) lap_SupervisoryFrame[_FrameType]

#define M_GET_SAPI(_PtL2Head) (((*_PtL2Head)>>2) & 0x7)

#define M_GET_EA(_PtL2Head) ((*_PtL2Head) & 1)

#define M_GET_PF(_PtL2Head) ((*(((u8 *)_PtL2Head)+1)&0x10) >> 4)

#define M_SET_PF(_PtL2Head)        \
{ *((u8 *)_PtL2Head+1) |= (0x10);  \
}

#define M_SET_NS(_PtL2Head, _NS)        \
{                                       \
 *((u8 *)_PtL2Head+1) &= 0xF1;          \
 *((u8 *)_PtL2Head+1) |= ((_NS) << 1);  \
}

#define M_GET_NS(_PtL2Head) (((*((u8 *)_PtL2Head+1)) & 0xE) >> 1)

#define M_CLEAR_NR(_PtL2Head)           \
{ *((u8 *)_PtL2Head+1) &= 0x1F;         \
}

#define M_SET_NR(_PtL2Head, _NR)        \
{                                       \
  *((u8 *)_PtL2Head+1) &= 0x1F;         \
  *((u8 *)_PtL2Head+1) |= ((_NR) << 5); \
}

#define M_GET_NR(_PtL2Head) (((*((u8 *)_PtL2Head+1)) & 0xE0) >> 5)

#define M_SET_M(_PtL2Head)              \
{  *((u8 *)_PtL2Head+2) |= (1<<1);      \
}

#define M_GET_M(_PtL2Head) (((*((u8 *)_PtL2Head+2)) & 0x2)>> 1)

#define M_GET_BS_COMMAND(_PtL2Head) (((*(u8*)_PtL2Head) & 0x2)>> 1)

#define LAP_RESPONSE 1
#define LAP_COMMAND  0
#define M_SET_ADDRESS(_PtL2Head, _ControlResponse)                         \
{ if (lap_SapiIdx == 0)                                                    \
    *_PtL2Head =  ((_ControlResponse) << 1) | 1;                           \
  else                                                                     \
    *_PtL2Head = ( 3 << 2) | ((_ControlResponse) << 1) | 1;                \
}

#define M_SET_CONTROL(_PtL2Head, _FrameType, _PollFinal)                   \
{ *((u8 *)_PtL2Head+1) = lap_ControlField[(_FrameType)] | ((_PollFinal) << 4); \
}

#define MORE_SEGMENT    1
#define NO_MORE_SEGMENT 0
#define M_SET_LENGTH(_PtL2Head, _Length, _MoreSegment)                     \
{ *((u8 *)_PtL2Head+2) = ((_Length) << 2) | (((_MoreSegment) << 1) | 1);       \
}
#define M_GET_LENGTH(_PtL2Head) (*((u8 *)_PtL2Head+2) >> 2)

#define M_GET_N201(_Channel) (_Channel == LAP_SACCH ? 18:20)

DefExtern const u8 lap_TabN200[4]
#ifdef __LAP_FRM_VAR__
=
{
  23, // LAP_SDCCH
  29, // LAP_FACCH_HALF
  34, // LAP_FACCH_FULL
  5   // LAP_SACCH
}
#endif
;
#define M_GET_N200(_Channel) (lap_TabN200[_Channel])

void lap_ProcessDataReq(Msg_t *PtMsg);
void lap_SendFrame(FrameType_t FrameType, Msg_t *PtMsg, bool PollFinal, bool CommandResponse);
void lap_SendAgainLastFrame(void);
void lap_SendFirstSegmentIFrame(void);
void lap_SendSubsequentSegmentIFrame(void);
void lap_SendUIL1DataReq(Msg_t* PtMsg);
void lap_SendL1DataReq(Msg_t* PtMsg);
void lap_SendLapDataInd(void);
void lap_ConcatenateRxI(void);
u8   lap_Purge ( void * var, void * PtMsg );
void lap_PurgeDataReqQueue( void );
bool lap_TestNRSequenceError(u8 NR);
void lap_StopT200(void);
void lap_PurgeLapQueue(void);
void lap_PurgeL1Queue(void);

#undef DefExtern

#endif  // __LAP_FRM_H__

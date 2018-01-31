//------------------------------------------------------------------------------
//  $Log: snd_trs.h,v $
//  Revision 1.4  2006/05/23 14:37:18  aba
//  Update of the copyright
//
//  Revision 1.3  2006/02/23 10:59:20  aba
//  Additional commentaries
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
  File       : SND_TRS.H
--------------------------------------------------------------------------------

  Scope      : Defintions for snd_trs.c

  History    :
--------------------------------------------------------------------------------
  Jul 13 04  |  ABA   | Creation
================================================================================
*/

#ifndef __SND_TRS_H__
#define __SND_TRS_H__

#include "sxr_ops.h"
#include "cmn_defs.h"

#ifdef __SND_TRS_C__
#define DefExtern
#else
#define DefExtern extern
#endif

#define NPDU_NB_NOT_GENERATED 0xFFFF
#define SND_DEFAULT_N201_U 500

// Debug define :
// Debug code for trace
#define __SND_DBG1__

// Debug ActivateInd/DeactivateInd verification
#define __SND_DBG2__

//******************************************************************************
//              SNDCP HEADER PROCESSING
//******************************************************************************
#define M_GET_BIT_MORE(_PtSndHead) (((*(_PtSndHead))&0x10)>>4)

#define M_GET_BIT_FIRST_SEG(_PtSndHead) (((*(_PtSndHead))&0x40)>>6)

#define F_SHIFT      6
#define T_SHIFT      5
#define M_SHIFT      4
#define SEG_NB_SHIFT 4

// This value is used if there is no compression
#define SND_XID_VERSION_SIZE 3

#define SND_DATA_HEAD_SIZE      3
#define SND_UNIT_DATA_HEAD_SIZE 4
// When the ack segment is not the first, there is no comp byte neither pdu number
#define SND_DATA_SUBS_HEAD_SIZE      1

// When the unack segment is not the first, there is no comp byte
#define SND_UNIT_DATA_SUBS_HEAD_SIZE 3

#define SET_FIRST_BIT   1
#define RESET_FIRST_BIT 0
#define SET_MORE_BIT    1
#define RESET_MORE_BIT  0
#define T_UNIT_DATA     1
#define T_DATA          0

#define M_SET_UNIT_DATA_HEADER(_PtSndHead, _First, _More, _SegNb)                                           \
{                                                                                                           \
  *(_PtSndHead++) = (_First<<F_SHIFT) | (T_UNIT_DATA<<T_SHIFT) | (_More<<M_SHIFT) | (5+pSndCtx->NSapiIdx);  \
  if (_First)                                                                                               \
    *(_PtSndHead++) = pSndCtx->NSAPI[pSndCtx->NSapiIdx]->UnackComp;                                         \
  *(_PtSndHead++) = ((_SegNb & 0xF)<<SEG_NB_SHIFT) | ((pSndCtx->NSAPI[pSndCtx->NSapiIdx]->UnackTxNPDUNb&0xF00)>>8); \
  *(_PtSndHead)   = (pSndCtx->NSAPI[pSndCtx->NSapiIdx]->UnackTxNPDUNb & 0xFF);                              \
}

#define M_SET_DATA_HEADER(_PtSndHead, _First, _More, _NPDUNb)                                                        \
{                                                                                                           \
  *(_PtSndHead++) = (_First<<F_SHIFT) | (T_DATA<<T_SHIFT) | (_More<<M_SHIFT) | (5+pSndCtx->NSapiIdx);       \
  if (_First)                                                                                               \
  {                                                                                                         \
    *(_PtSndHead++) = pSndCtx->NSAPI[pSndCtx->NSapiIdx]->AckComp;                                           \
    *(_PtSndHead)   = _NPDUNb;                                                                              \
  }                                                                                                         \
}

#define M_GET_M_BIT(_PtSndHead)      ((*_PtSndHead & (1<<M_SHIFT))>>M_SHIFT)
#define M_GET_F_BIT(_PtSndHead)      ((*_PtSndHead & (1<<F_SHIFT))>>F_SHIFT)
#define M_GET_T_BIT(_PtSndHead)      ((*_PtSndHead & (1<<T_SHIFT))>>T_SHIFT)
#define M_GET_COMP_UNACK(_PtSndHead) (*(_PtSndHead+1))
#define M_GET_COMP_ACK(_PtSndHead)   (*(_PtSndHead+1))
#define M_GET_NSAPI(_PtSndHead)      ((*_PtSndHead)&0xF)

// dmx 20160921, the NPDU_NB is 12 bit
#define M_GET_FIRST_SEG_RX_NPDU_NB_UNACK(_PtSndHead) ((u16)((((u32)((*(_PtSndHead+2))&0xF))<<8)|(*(_PtSndHead+3)))) 
#define M_GET_SUBS_SEG_RX_NPDU_NB_UNACK(_PtSndHead)  ((u16)((((u32)((*(_PtSndHead+1))&0xF))<<8)|(*(_PtSndHead+2)))) 
#define M_GET_NPDU_NB_ACK(_PtSndHead) (*(_PtSndHead+2))


// M_GET_SEG_NB is used for subsequent segment
#define M_GET_SEG_NB(_PtSndHead)     ((*(_PtSndHead+1))>>4)

#define M_CONV_SAPI_TO_SAPIIDX(_Sapi)  (snd_TabSapiToSapiIdx[_Sapi])
DefExtern const u8 snd_TabSapiToSapiIdx[12]
#ifdef __SND_TRS_C__
//      3   5       9  11
={0,0,0,0,0,1,0,0,0,2,0,3 }
#endif
;

#define M_CONV_SAPIIDX_TO_SAPI(_SapiIdx)  (snd_TabSapiIdxToSapi[_SapiIdx])
DefExtern const u8 snd_TabSapiIdxToSapi[4]
#ifdef __SND_TRS_C__
={3,5,9,11}
#endif
;

#if !(STK_MAX_SIM==1)
#define M_SEND_LLC_QUEUEDATA_REQ                       \
    {                                                  \
      u32 Evt [2];                                     \
      Evt [0] = LLC_QUEUEDATA_REQ;                     \
      Evt [0] = CMN_SET_SIM_ID(Evt [0], pSndCtx->nCurSimId);    \
      Evt [1] = M_CONV_SAPIIDX_TO_SAPI(SapiIdx);       \
      sxr_Send (Evt, LLC_MBX, SXR_SEND_EVT);           \
    }
#else
#define M_SEND_LLC_QUEUEDATA_REQ                       \
    {                                                  \
      u32 Evt [2];                                     \
      Evt [0] = LLC_QUEUEDATA_REQ;                     \
      Evt [1] = M_CONV_SAPIIDX_TO_SAPI(SapiIdx);       \
      sxr_Send (Evt, LLC_MBX, SXR_SEND_EVT);           \
    }
#endif

#undef DefExtern
#endif  // __SND_TRS_H__



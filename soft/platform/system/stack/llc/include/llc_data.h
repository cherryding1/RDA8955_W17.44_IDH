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
//  $Log: llc_data.h,v $
//  Revision 1.3  2006/05/23 10:19:27  fch
//  Change to set array of pointer in ROM
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:46  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : LLC_DATA.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for LLC  

  History    :
--------------------------------------------------------------------------------
  Mar 03 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __LLC_DATA_H__
#define __LLC_DATA_H__

#include "cmn_defs.h"   // Common definition

#include "llc_msg.h"    // LLC messages definition
#include "llc_defs.h"   // LLC internal definition
#include "llc_frm.h"
#include "itf_llc.h"   // LLC internal definition


#ifdef __LLC_C__
#define DefExtern 
#else
#define DefExtern extern
#endif
#ifdef __TEMP_FLOW_CONTROL__
/* added by laixf 2009.03.06 for GPRS UDP test*/
#define  LLC_FLOWCTRL_OFF   0
#define  LLC_FLOWCTRL_ON   1
#endif


//============================================================================
//  Structure definition
//============================================================================
typedef struct
{
#ifndef STK_SINGLE_SIM
  u32	   SimIndex;    //Sim Index for dual SIM support
#endif
  Msg_t             *RecMsg;      // Received message
  Msg_t             *OutMsg;      // Message to send

  InEvtDsc_t        *InEvtDsc;    // Incoming Event
  OutEvtDsc_t       *OutEvtDsc;   // Outgoing Event
                    
  bool              KeepMsg;      // TRUE if RecMsg must not be deleted
                    
  u8                RecFrmTyp;    // Received Frame format type
  #ifdef __TEMP_FLOW_CONTROL__
  u8 			LLC_FlowCtrl_State;
  #endif
  u8                CurSapi;      // Current SAPI processed
  u8                GlbFlag;      // Flags
#define LLC_FLG_CLEAR         0x00
#define LLC_FLG_NEW_TLLI      ( 1 << 0 )  // A new TLLI has been assigned, inform MMP
                                          // when a frame is received on it
#define LLC_FLG_SUSPENDED     ( 1 << 1 )  // LLC suspended
#define LLC_FLG_RLC_QUEUE     ( 1 << 2 )  // Indication that a frame has been
                                          // inserted in the Rlc Queue
#define LLC_FLG_TLLI_ASSIGNED ( 1 << 3 )
                                    
  u8                LlcVersion;   // LLC version from SGSN
  u32               Iov_UI;       // Input Offset Value for UI frame, for ciphering
  u32               CurTlli;      // Current TLLI
  u32               OldTlli;      // Old TLLI
  u8                Kc[LLC_KC_LENGTH];  // Ciphering Key
  u8                CiphAlgo;     // Ciphering algorithm to use
//  llc_TlliPar_t     CurTlliPar;   // Current TLLI parameters
  u8                RlcQueueId;   // Id of the queue shared with RLC
                    
  llc_LleCtx_t      *CurLLE;      // Pointer to the current LLE
  llc_LleCtx_t      *LLEsCtx[LLC_SAPI_NB];   // Array of pointer on the LLEs


}llc_Data_t;


//============================================================================
//  Data declaration
//============================================================================

#ifndef STK_SINGLE_SIM
DefExtern llc_Data_t    llc_Data[STK_MAX_SIM];
#else
DefExtern llc_Data_t    llc_Data;
#endif

DefExtern llc_Data_t    *pLLCCtx;

DefExtern const llc_LleDefPar_t llc_LleDefParam[LLC_SAPI_NB]
#ifdef __LLC_C__
=
{
    //     Iov_I               T200             N200    N201_U N201_I  mD   mU   kD  kU

  {                    0, LLC_DEF_T200_5S,  LLC_DEF_N200, 400,    0,    0,    0,  0,  0 },   // SAPI 1
  { (LLC_SAPI_DATA3<<27), LLC_DEF_T200_5S,  LLC_DEF_N200, 500, 1503, 1520, 1520, 16, 16 },   // SAPI 3
  { (LLC_SAPI_DATA5<<27), LLC_DEF_T200_10S, LLC_DEF_N200, 500, 1503,  760,  760,  8,  8 },   // SAPI 5
  {                    0, LLC_DEF_T200_20S, LLC_DEF_N200, 270,    0,    0,    0,  0,  0 },   // SAPI 7
  { (LLC_SAPI_DATA9<<27), LLC_DEF_T200_20S, LLC_DEF_N200, 500, 1503,  380,  380,  4,  4 },   // SAPI 9
  {(LLC_SAPI_DATA11<<27), LLC_DEF_T200_40S, LLC_DEF_N200, 500, 1503,  190,  190,  2,  2 }    // SAPI 11
}
#endif
;


DefExtern const llc_CheckXid_t llc_CheckXid[LLC_XID_TYPE_MAX+1]
#ifdef __LLC_C__
=
{
//  Size   Min    Max                             Actions 
  {  1,     0,     15,                         LLC_XID_CHECK_RANGE                       },  // VERSION
  {  4,     0,      0,                         LLC_XID_NOCHECK_RANGE                     },  // IOV_UI
  {  4,     0,      0, LLC_XID_CHECK_SIG_LLE | LLC_XID_NOCHECK_RANGE                     },  // IOV_I
  {  2,     1,   4095,                         LLC_XID_CHECK_RANGE                       },  // T200
  {  1,     1,     15,                         LLC_XID_CHECK_RANGE                       },  // N200
  {  2,   140,   1520,                         LLC_XID_CHECK_RANGE                       },  // N201_U
  {  2,   140,   1520, LLC_XID_CHECK_SIG_LLE | LLC_XID_CHECK_RANGE | LLC_XID_CHECK_VALUE },  // N201_I
  {  2,     9,  24320, LLC_XID_CHECK_SIG_LLE | LLC_XID_CHECK_RANGE                       },  // MD
  {  2,     9,  24320, LLC_XID_CHECK_SIG_LLE | LLC_XID_CHECK_RANGE | LLC_XID_CHECK_VALUE },  // MU
  {  1,     1,    255, LLC_XID_CHECK_SIG_LLE | LLC_XID_CHECK_RANGE | LLC_XID_CHECK_VALUE },  // KD
  {  1,     1,    255, LLC_XID_CHECK_SIG_LLE | LLC_XID_CHECK_RANGE | LLC_XID_CHECK_VALUE },  // KU
  {  1,     0,      0,                         LLC_XID_NOCHECK_RANGE                     },  // L3
  {  0,     0,      0,                         LLC_XID_NOCHECK_RANGE                     }   // RESET
}
#endif
;

#undef DefExtern 

#endif // endif __LLC_DATA_H__

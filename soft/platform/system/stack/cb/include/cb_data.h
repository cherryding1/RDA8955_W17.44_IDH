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
//  $Log: cb_data.h,v $
//  Revision 1.3  2006/05/23 10:18:35  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:40  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  File       : CB_DATA.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for CB  

  History    :
--------------------------------------------------------------------------------
  Dec 07 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __CB_DATA_H__
#define __CB_DATA_H__

#include "cb_msg.h"    // CB messages definition

#include "cb_defs.h"

#ifdef __CB_C__
#define DefExtern 
#else
#define DefExtern extern
#endif



//============================================================================
//  Structure definition
//============================================================================
typedef struct
{
  Msg_t             *IMsg;      // Received message
  Msg_t             *OMsg;      // Message to send
  OutEvtDsc_t       *OEvtDsc;   // Outgoing Event
                    
  bool              KeepMsg;      // TRUE if RecMsg must not be deleted
  u8                Flags;
#define CB_CBCH_SUPPORTED   ( 1 << 0 )  // Cell supporting CBCH
#define CB_CBS_ACTIVATED    ( 1 << 1 )  // CBS activated
#define CB_SCHEDULE_MSG     ( 1 << 2 )  // Schedule message in reception
#define CB_SCHED_RECEIVED   ( 1 << 3 )  // Schedule message received for the
                                        // current period

  u8                ItfType;      // Kind of interface with API (Msg or Page)

#if !(STK_MAX_SIM==1)
  u8                SimId;      // Sim Id number
#endif

  cb_CellPar_t      CellPar;      // Current cell parameters
  cb_Cfg_t          *CfgList;     // Pointer on conf list (MId and DCS)

  u8                NextBlk;      // Next waited block
  u16               CurMId;       // MId of the current SMSCB message
  u16               CurSerNum;    // Serial Number of the current SMSCB message
  u8                CurDCS;       // DCS of the current SMSCB message
  u8                *CbMsg;       // Data of the CB message

  cb_SmsCbHist_t    History[CB_HISTORY_SIZE]; // History of the received SMS CB
  
  cb_SmsCbCtx_t     SmsCbCtx[CB_SMSCB_CTX_SIZE];  // Current SMSCB messages in
                                                  // reception
  cb_SchedCtx_t*    SchedCtx;     
  u8                CurSlot;      
}cb_Data_t;


//============================================================================
//  Data declaration
//============================================================================

DefExtern cb_Data_t    *gp_cb_Data;

#undef DefExtern 

#endif // endif __CB_DATA_H__

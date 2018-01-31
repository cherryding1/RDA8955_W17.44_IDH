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
//  $Log: rlp_data.h,v $
//  Revision 1.6  2006/06/07 12:50:10  fch
//  Change to set array rlp_DataIndEvtList in ROM
//
//  Revision 1.5  2006/05/23 10:25:35  fch
//  Update Copyright
//
//  Revision 1.4  2006/05/23 10:22:05  fch
//  Change to set array of pointer in ROM
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
  File       : RLP_DATA.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for RLP

  History    :
--------------------------------------------------------------------------------
  Mar 31 05  |  FCH   | Creation            
================================================================================
*/
#ifndef __RLP_DATA_H__
#define __RLP_DATA_H__

#include "cmn_defs.h"   

#include "rlp_msg.h"
#include "rlp_defs.h"
#include "rlp_evt.h"


#ifdef __RLP_C__
#define DefExtern 
#else
#define DefExtern extern
#endif



//============================================================================
//  Structure definition
//============================================================================
typedef struct
{
  Msg_t             *RecMsg;      
  Msg_t             *OutMsg;      

  InEvtDsc_t        *InEvtDsc;    
  OutEvtDsc_t       *OutEvtDsc;   
                    
  u8                Flags;
#define RLP_KEEP_MSG            ( 1 << 0 )
#define RLP_CONF_RCV            ( 1 << 1 )  
#define RLP_REMOTE_NOT_READY    ( 1 << 2 )  
#define RLP_LOCAL_NOT_READY     ( 1 << 3 )  
#define RLP_TX_BUSY             ( 1 << 4 )  

  u8                ChanCod;      
  u8                FrmSize;      
                                  
                                    
  rlp_SysPar_t      SysPar;       
  rlp_CrlpReq_t     *UserSysPar;  
  u16               RecHeader;    
  
  Msg_t             *UiQFirst;    
  Msg_t             *UiQLast;     

  Msg_t             *TESTPdu;     
  Msg_t             *XIDPdu;      
                                  

  u8                PollXchgState;

  u8                SABMState;    
  u8                DISCState;    
  u8                DISC_P;       
  u8                XIDState;     
  u8                XID_C;        
  u8                DMState;      
  u8                DM_F;         
  u8                UAState;      
  u8                UA_F;         
  u8                TESTState;    
  u8                TEST_F;       

  u8                SABM_DISCCount; 
  u8                XIDCount;     
  
    
    
    
  rlp_RxSlot_t      RxSlot[RLP_MAX_WINDOW_LEN];
  u8                Vr;           
                                  
  u8                LastRxNs;     
                                  
  u8                PollState;    
  u8                PollCount;
//  u8                AcknState;    
  bool              Ackn_F;       
  u8                RxREJState;   
  u8                RxSREJNb;     
  
  u8                SeqNumT_RCVSExp;  

  rlp_TxSlot_t      TxSlot[RLP_MAX_WINDOW_LEN];
  u8                Vs;           
  u8                Va;           
  u8                Vd;           
  Msg_t             *IQFirst;     
  Msg_t             *IQLast;      
  
}rlp_Data_t;


//============================================================================
//  Data declaration
//============================================================================

DefExtern rlp_Data_t    rlp_Data;



DefExtern const rlp_SysPar_t rlp_DefSysPar[3]
#ifdef __RLP_C__
=
{
  {
    RLP_DEF_KIM,
    RLP_DEF_KMI,
    RLP_DEF_T1_48,
    RLP_DEF_N2,
    RLP_DEF_VERSION,
    RLP_DEF_NA,
    RLP_DEF_T2
  },
  { 
    RLP_DEF_KIM,
    RLP_DEF_KMI,
    RLP_DEF_T1_96,
    RLP_DEF_N2,
    RLP_DEF_VERSION,
    RLP_DEF_NA,
    RLP_DEF_T2
  },
  { 
    RLP_DEF_KIM,
    RLP_DEF_KMI,
    RLP_DEF_T1_144,
    RLP_DEF_N2,
    RLP_DEF_VERSION,
    RLP_DEF_NA,
    RLP_DEF_T2
  }
}
#endif
;

  /*
  ==========================================
      Array for U frame type conversion
  ==========================================
  */
DefExtern const u8 rlp_UFrameTypeCnv [32]
#ifdef __RLP_C__
=
{
  UI_FRM,
  UNDEFINED_FRM,
  DISC_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UA_FRM,
  TEST_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,  // REMAP
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  DM_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  UNDEFINED_FRM,
  SABM_FRM,
  XID_FRM,
  NULL_FRM,
  UNDEFINED_FRM,
}
#endif
;

  /*
  ==========================================
      List of the derived events for a
      L1_DATA_IND (frame reception)
  ==========================================
  */
DefExtern InEvtDsc_t * const rlp_DataIndEvtList [RLP_NB_FRM_TYP]
#ifdef __RLP_C__
=
{
  &rlp_RxSFrameEvt,

  &rlp_RxIFrameEvt,

  &rlp_RxU_UIFrameEvt,
  &rlp_RxU_DMFrameEvt,
  &rlp_RxU_SABMFrameEvt,
  &rlp_RxU_DISCFrameEvt,
  &rlp_RxU_UAFrameEvt,
  NULL,
  &rlp_RxU_XIDFrameEvt,
  &rlp_RxU_TESTFrameEvt,
  NULL
}
#endif
;

  /*
  ==========================================
      Name of frame for trace purpose
  ==========================================
  */
DefExtern const ascii * const rlp_FrmName [RLP_NB_FRM_TYP]
#ifdef __RLP_C__
=
{
  "S",

  "I+S",

  "UI",
  "DM",
  "SABM",
  "DISC",
  "UA",
  "NULL",
  "XID",
  "TEST",
  "UNDEF"
}
#endif
;







#if 0
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
#endif

#undef DefExtern 

#endif // endif __LLC_DATA_H__

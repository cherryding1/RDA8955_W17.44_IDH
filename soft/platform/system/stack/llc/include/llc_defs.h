//------------------------------------------------------------------------------
//  $Log: llc_defs.h,v $
//  Revision 1.3  2006/05/23 10:36:46  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:46  pca
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
  File       : LLC_DEFS.H
--------------------------------------------------------------------------------

  Scope      : LLC constants, macros and internal structures defintion  

  History    :
--------------------------------------------------------------------------------
  Mar 11 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __LLC_DEFS_H__
#define __LLC_DEFS_H__

#include "cmn_defs.h"   // Common definition
#include "sxs_io.h"     // Trace purpose

#include "itf_pal.h"

#include "llc_trc.h"    // LLC trace levels
#include "itf_llc.h"
#include "llc_msg.h"    // LLC messages definition
#include "llc_fsm.h"
#include "llc_func.h"


//============================================================================
//  MACROS
//============================================================================


    // Conversion between SAPI (1;3;5;7;9;11) and associated Idx (0 to 5)
#define LLC_IDX_2_SAPI(_Idx)        (((_Idx) * 2) + 1)
#define LLC_SAPI_2_IDX(_Sapi)       (((_Sapi) - 1)/2)


    // Did the LLE corresponding to _Sapi is a signalling entity ?
#define LLC_IS_SIG_LLE(_Sapi)       ( (_Sapi == LLC_SAPI_MMP) || (_Sapi == LLC_SAPI_SMS) ? TRUE : FALSE )
#define LLC_ISNOT_SIG_LLE(_Sapi)    (!LLC_IS_SIG_LLE(_Sapi))


    // Did the LLE corresponding to _Sapi is a user data entity ?
#define LLC_IS_USR_LLE(_Sapi)       ( (_Sapi == LLC_SAPI_DATA3) || (_Sapi == LLC_SAPI_DATA5) ||               \
                                      (_Sapi == LLC_SAPI_DATA9) || (_Sapi == LLC_SAPI_DATA11)? TRUE : FALSE )

#define LLC_ISNOT_USR_LLE(_Sapi)    (!LLC_IS_USR_LLE(_Sapi))



    // Did the SAPI is correct
#define LLC_IS_SAPI_CORRECT(_Sapi)  ( (_Sapi == LLC_SAPI_MMP)   || (_Sapi == LLC_SAPI_SMS)   ||			\
                                      (_Sapi == LLC_SAPI_DATA3) || (_Sapi == LLC_SAPI_DATA5) ||			\
                                      (_Sapi == LLC_SAPI_DATA9) || (_Sapi == LLC_SAPI_DATA11) ? TRUE : FALSE ) 


    // FSM state stuff
#ifndef STK_SINGLE_SIM
#define LLC_STATE                   FSM_GET_STATE(&lle_Fsm[pLLCCtx->SimIndex][LLC_SAPI_2_IDX(pLLCCtx->CurSapi)])
#define LLC_SET_STATE(_State)       FSM_SET_STATE(&lle_Fsm[pLLCCtx->SimIndex][LLC_SAPI_2_IDX(pLLCCtx->CurSapi)], _State)
#else
#define LLC_STATE                   FSM_GET_STATE(&lle_Fsm[LLC_SAPI_2_IDX(pLLCCtx->CurSapi)])
#define LLC_SET_STATE(_State)       FSM_SET_STATE(&lle_Fsm[LLC_SAPI_2_IDX(pLLCCtx->CurSapi)], _State)
#endif

#ifdef __LLC_DEBUG__
#define LLC_RX_IFRM_IND_PRINT       llc_RxIFrmIndPrint()
#else
#define LLC_RX_IFRM_IND_PRINT
#endif

//============================================================================
//  CONSTANTS
//============================================================================
#define LLC_OK      TRUE
#define LLC_ERROR   FALSE

#define LLC_NO_QUEUE  0xFF

#define LLC_MS_TO_SGSN    PAL_MS_TO_SGSN
#define LLC_SGSN_TO_MS    PAL_SGSN_TO_MS

#define LLC_N202        4


  // Constants for the Context reset
#define LLC_CLR2NULL      0
#define LLC_CLR2ADM       1

#define LLC_XID_CLEAR       0x00  // Clear the XID part (XidRcv array) while Ctx is cleared
#define LLC_XID_NO_CLEAR    0x01  // Do not clear the XID part (XidRcv array) while Ctx is cleared
#define LLC_UCMD_NO_CLEAR   0x02  // Do not clear the UCmd context while Ctx is cleared

//============================================================================
//  INTERNAL STRUCTURES AND ASSOCIATED CONSTANTS
//============================================================================

      //-------------------------------------------------------
      //  TLLI Param
      //-------------------------------------------------------
typedef struct
{
  u8    Kc[LLC_KC_LENGTH];    // Ciphering Key
  u8    CiphAlgo;             // Ciphering algorithm to use
} llc_TlliPar_t;



      //-------------------------------------------------------
      //  CurSapi
      //-------------------------------------------------------
#define LLC_UNKNOWN_SAPI      0xFF





      //-------------------------------------------------------
      //  Default LLE's parameters
      //-------------------------------------------------------
typedef struct
{
  u32   Iov_I;  // Input Offset Value for I frames
  u16   T200;   // Retransmission timer : 1 to 4095 (0,1sec to 409,5sec)
  u8    N200;   // Max number of retransmission of a frame : 1 to 15
  u16   N201_U; // Max number of bytes in an information field (frame U & UI): 140 to 1520
  u16   N201_I; // Max number of bytes in an information field (frame I): 140 to 1520
  u16   mD;     // Max I frame buffer size in downlink : 0, 9 through 24320
  u16   mU;     // Max I frame buffer size in uplink : 0, 9 through 24320
  u8    kD;     // Max number of outstanding I frames in downlink : 1 to 255
  u8    kU;     // Max number of outstanding I frames in uplink : 1 to 255
  
} llc_LleDefPar_t;


#define LLC_DEF_VERSION       0
#define LLC_DEF_IOV_UI        0
#define LLC_DEF_N200          3

#define LLC_DEF_T200_5S       50
#define LLC_DEF_T200_10S      100
#define LLC_DEF_T200_20S      200
#define LLC_DEF_T200_40S      400



      //-------------------------------------------------------
      //  Ciphering constants and macros
      //-------------------------------------------------------
  // Overflow counters (OC) are incremented by steps of 512
#define LLC_OC_INCREMENT      512


      //-------------------------------------------------------
      //  Miscellaneous constants
      //-------------------------------------------------------
  // Mask for the received UI frame list
#define LLC_RCV_UI_FRM_MSK    0x80000000



      //-------------------------------------------------------
      //  Parameters to manage the U command
      //-------------------------------------------------------
typedef struct
{
  Msg_t   *UCmdPtr;     // Pointer to the last U command sent (SABM or XID, DISC are not traced)
  u8      OnGoing;      // Bit map info of the ongoing command (see below)
  u8      NbRetrans;    // Number of retransmission of the command
} llc_UCmd_t;

#define LLC_CMD_WITH_L3_XID   0x80  // Indicates if the command contains L3 Xid
#define LLC_CMD_REESTAB       0x40  // Indicates if the command result from re-establishment
#define LLC_CMD_ONGOING_MSK   0x3F  // Mask to get the Cmd type in the OnGoing field

  // Macros to manage UCmd
#define LLC_IS_CMD_ONGOING                (pLLCCtx->CurLLE->Sig.UCmd.UCmdPtr != NULL)
#define LLC_GET_ONGOING_CMD               (pLLCCtx->CurLLE->Sig.UCmd.OnGoing & LLC_CMD_ONGOING_MSK)
#define LLC_IS_ONGOING_CMD(_cmd)          (LLC_GET_ONGOING_CMD == _cmd)
#define LLC_IS_ONGOING_CMD_WITH_L3XID     (pLLCCtx->CurLLE->Sig.UCmd.OnGoing & LLC_CMD_WITH_L3_XID)
#define LLC_IS_ONGOING_CMD_REESTAB        (pLLCCtx->CurLLE->Sig.UCmd.OnGoing & LLC_CMD_REESTAB)





      //-------------------------------------------------------
      //  XID utilities
      //-------------------------------------------------------
typedef struct
{
  u8    Size;   // Param Size
  u8    Min;    // Val min
  u16   Max;    // Val Max
  u8    Action; // Bit map of check to do
} llc_CheckXid_t;


#define LLC_XID_NOCHECK_RANGE 0x00
#define LLC_XID_CHECK_RANGE   0x01  // Check if value is out of range
#define LLC_XID_CHECK_VALUE   0x02  // Check the value compared with the current one
#define LLC_XID_CHECK_SIG_LLE 0x04  // Parameter that should not be present for a Signalling LLE


typedef struct
{
  bool  Present;
  u8    Status; // see below
  u32   Val;
}llc_XidRcv_t;

#define LLC_XID_PB_DETECTED    0x01
#define LLC_XID_TOBE_RESPONDED 0x02




      //-------------------------------------------------------
      //  For transmitted acknowledged L3Pdu storage
      //-------------------------------------------------------

#define LLC_IFRM_NOT_YET_TRANSMITTED    0x00
#define LLC_IFRM_TRANSMITED             0x01
#define LLC_IFRM_ACKNOWLEDGED           0x02
#define LLC_IFRM_MARK_FOR_RETRANS       0x03

  /*
  ** Shortcut for TxL3Pdu declaration and access
  */

    // Access to the Frame paramaters associated with a TxL3Pdu
#define FRM_PAR   LLC_ILS.llcDataReq  // Specific header 







      //-------------------------------------------------------
      //  Signalling LLC entity parameters
      //-------------------------------------------------------
typedef struct
{
  u8            DataQWithL3;    // Identifier of the queue which is used in interface with
                                // L3 for the DataReq and UnitDataReq
  
  u16           T200;           // Retransmission timer
  u16           N201_U;         // Max number of bytes for the info field of U & UI frames
  u8            N200;           // Frame max retransmission number (associated with t200 & t201)

  u16           Vu;             // Unconfirmed send state variable
  u16           Vur;            // Unconfirmed receive state variable
  u32           RcvUIFrm;       // Indicator to know if the 32 frames before V(UR) has been received
  u32           OcUIDwn;        // Downlink OC for UI frame (ciphering purpose)
  u32           OcUIUp;         // Uplink OC for UI frame (ciphering purpose)

  u8            UnitDataReqQ;   // Identifier of the queue to save UnitDataReq when suspended

  u8            RadioPrio;      // Radio priority
  u8            Throughput;     // Peak throughput class
  u8            RlcMode;        // RLC mode (From reliability in EstabReq
  u8            Pfi;            // Packet Flow Identifier (for RLC)
  bool          PM;             // Protection mode (from reliability in EstabReq)

  llc_UCmd_t    UCmd;           // Command U frame parameters
  Msg_t         *SavMsg;        // Saved message (EstabReq or ReleaseReq) to be processed later

  llc_XidRcv_t  *XidRcv;        // Pointer on the received XID parameters
  bool          EstabXidRspWaited;   // Indicated that the LLE has transmitted a XID_IND to L3
                                // and is waited a XID_RSP

} llc_SigLlePar_t;


      //-------------------------------------------------------
      //  User data LLC entity parameters
      //-------------------------------------------------------
typedef struct
{
  u32           Iov_I;          // Input Offset Value for I frames (ciphering purpose)
//  u32           MD;             // Max I frame buffer size, in bytes, for downlink
  u32           MU;             // Max I frame buffer size, in bytes, for uplink
  u16           N201_I;         // Max number of bytes for the info field of I frames
  u8            kD;             // Max number of outstanding I frames in downlink
  u8            kU;             // Max number of outstanding I frames in uplink

  u16           Vs;             // Send state variable
  u16           Va;             // Acknowledge state variable (first frame of the Tx window)
  u16           Vr;             // Received state variable
  s32           B;              // I frames buffered size (in bytes)
  u32           OcIDwn;         // Downlink OC for I frame (ciphering purpose)
  u32           OcIUp;          // Uplink OC for I frame (ciphering purpose)
  
  u8            BusyCond;       // Receiver busy conditions (peer and own)

  u8            DataReqQ;       // Identifier of the queue to save DataReq for later processing
  u8            TxL3PduQ;       // Identifier of the queue of the sent L3 PDU (but unacknowledged yet)
  u8            RxIFrmQ;        // Identifier of the queue of the Rx L3 PDU (but not transmitted to L3 yet)

  u32           *RxIFrmInd;     // Indicator for the Rx I frames. This indicator is allocated at establishment
                                // One bit is used for one received PDU in the window.
                                // The Msb of the first int corresponds to V(R)+1
  u16           LastRxNs;       // N(S) of the last I frame received from the peer.
                                // Used to detect in sequence errors
  u16           T201AssPar;     // Parameters associated with T201 (N(S), running for N(S), running for busy)
  u8            BusyRetransNb;  // Number of retransmission in peer busy condition
  bool          Reestab;        // Indicator if a re-establishment must be done
} llc_UsrLlePar_t;



    // Busy conditions constants and macros
#define LLC_NO_BUSY     0x00
#define LLC_OWN_BUSY    0x01
#define LLC_PEER_BUSY   0x02

#define LLC_IS_OWN_BUSY           ((pLLCCtx->CurLLE->u.Usr.BusyCond & LLC_OWN_BUSY) == LLC_OWN_BUSY)
#define LLC_IS_PEER_BUSY          ((pLLCCtx->CurLLE->u.Usr.BusyCond & LLC_PEER_BUSY) == LLC_PEER_BUSY)

#define LLC_CLEAR_ALL_BUSY_COND   (pLLCCtx->CurLLE->u.Usr.BusyCond = LLC_NO_BUSY)
#define LLC_CLEAR_OWN_BUSY        (pLLCCtx->CurLLE->u.Usr.BusyCond &= ~LLC_OWN_BUSY)
#define LLC_CLEAR_PEER_BUSY       (pLLCCtx->CurLLE->u.Usr.BusyCond &= ~LLC_PEER_BUSY)

#define LLC_SET_OWN_BUSY          (pLLCCtx->CurLLE->u.Usr.BusyCond |= LLC_OWN_BUSY)
#define LLC_SET_PEER_BUSY         (pLLCCtx->CurLLE->u.Usr.BusyCond |= LLC_PEER_BUSY)


    // Constants and macros for the parameters associated with T201 (T201AssPar)
    //
    //     -------------------------------------------------
    //     |15|14|13|12|11|10|9 |8 |7 |6 |5 |4 |3 |2 |1 |0 |
    //     -------------------------------------------------
    //     |              |  |  |      N(S) associated     |
    //                     V  V
    //                     |  |--> T201 running for N(S)
    //                     |-----> T201 running for peer busy

#define LLC_T201_ASS_PAR_RESET    0x0000
#define LLC_T201_ASS_NS_MSK       0x01FF
#define LLC_T201_RUN_NS_MSK       0x0200
#define LLC_T201_RUN_BUSY_MSK     0x0400

#define LLC_T201_GET_ASS_NS           (pLLCCtx->CurLLE->u.Usr.T201AssPar & LLC_T201_ASS_NS_MSK)
#define LLC_T201_IS_RUNNING_NS        ((pLLCCtx->CurLLE->u.Usr.T201AssPar & LLC_T201_RUN_NS_MSK)==LLC_T201_RUN_NS_MSK)
#define LLC_T201_IS_RUNNING_BUSY      ((pLLCCtx->CurLLE->u.Usr.T201AssPar & LLC_T201_RUN_BUSY_MSK)==LLC_T201_RUN_BUSY_MSK)

#define LLC_T201_SET_RUNNING_NS(_Ns)  (pLLCCtx->CurLLE->u.Usr.T201AssPar =                              \
                                          (pLLCCtx->CurLLE->u.Usr.T201AssPar & ~LLC_T201_ASS_NS_MSK) |  \
                                          LLC_T201_RUN_NS_MSK | ((_Ns) & LLC_T201_ASS_NS_MSK))        \
                                          
#define LLC_T201_SET_RUNNING_BUSY     (pLLCCtx->CurLLE->u.Usr.T201AssPar |= LLC_T201_RUN_BUSY_MSK)
#define LLC_T201_CLEAR_RUNNING_NS     (pLLCCtx->CurLLE->u.Usr.T201AssPar &= ~LLC_T201_RUN_NS_MSK)
#define LLC_T201_CLEAR_RUNNING_BUSY   (pLLCCtx->CurLLE->u.Usr.T201AssPar &= ~LLC_T201_RUN_BUSY_MSK)




      //-------------------------------------------------------
      //  LLC entity context structure
      //-------------------------------------------------------
typedef struct
{
  llc_SigLlePar_t   Sig;      // For Signalling & User LLE
  
  union
  {
    llc_UsrLlePar_t   Usr;    // For User LLE only
  }u;
} llc_LleCtx_t;


#define LLC_SIG_CTX_SIZE      sizeof(llc_SigLlePar_t)
#define LLC_USR_CTX_SIZE      sizeof(llc_LleCtx_t)




#endif // endif __LLC_DEFS_H__












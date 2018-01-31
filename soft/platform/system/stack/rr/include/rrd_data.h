//------------------------------------------------------------------------------
//  $Log: rrd_data.h,v $
//  Revision 1.13  2006/05/22 14:38:00  oth
//  Update the copyright
//
//  Revision 1.12  2006/05/02 16:18:32  oth
//  Memorize that a connection request have been asked by MMP and alert it of the release only when returning to void state
//
//  Revision 1.11  2006/04/21 14:48:01  oth
//  T3174 is stopped on reception of an assignment and not at the end of the contention resolution
//
//  Revision 1.10  2006/04/06 09:34:48  oth
//  Indentation
//
//  Revision 1.9  2006/04/03 13:23:31  oth
//  Latch the use of the Timeslot allocation bitmap when there is a starting time
//
//  Revision 1.8  2006/03/27 08:17:44  oth
//  - Memorize the cause of PCC failure
//  - Memorize the reception of the PCC indication from RRI
//
//  Revision 1.7  2006/03/10 16:51:01  oth
//  Memorized that we have asked to come back to the old main cell during a PCCO to avoid to ask it twice
//
//  Revision 1.6  2006/02/27 10:36:34  oth
//  New flag to remember if we were in an Uplink TBF when receiving a PCCO
//
//  Revision 1.5  2006/02/16 10:10:22  oth
//  Handling of the T3176 Expiration
//
//  Revision 1.4  2006/02/08 08:50:47  oth
//  Addition for the handling of the PCCO
//
//  Revision 1.3  2006/01/26 23:46:40  oth
//  Addition of a RRP flag to delay the PS Data Req on Immediate Assignement after sending the TBF Cfg Req
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

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : rrd_data.h  
--------------------------------------------------------------------------------
  Scope      : Declaration of RRD's data
  History    :
--------------------------------------------------------------------------------
  Sep 23 05  |  MCE   | Addition of CaSave in rrd_L1Con_t       
  Nov 01 04  |  MCE   | Addition of LastChanType to rrg_Ctx_t
  Sep 09 04  |  OTH   | Move the TchLoopMode from rrd_Data_t to rrg_Ctx_t
  Aug 18 04  |  MCE   | Suppression of the ChnDesc field in rrd_L1Con_t
  Jul 06 04  |  MCE   | Suppression of RslReqCount
  Jun 12 03  |  MCE   | Creation            
================================================================================
*/
#ifndef __RRD_DATA_H__
#define __RRD_DATA_H__

#include "rr_msg.h"
#include "itf_msg.h"
#include "rr_defs.h"

#ifdef __EGPRS__
// Endec includes
#include "edc1.h"
#endif

/*
** RRD access parameters
*/
// possible values for Type : Are we raching to establish a CS or
// a PS connection ? And if it's a PS connection is it RACH or PRACH ?
#define    RRD_ACC_GSM      0
#define    RRD_ACC_GPRS     1
#define    RRD_PRACH        ( 1 << 1 )
#ifdef __EGPRS__
#define    RRD_ACC_EGPRS   ( 1 << 2 )
#endif

typedef struct
{
  u8   Type       ; // RRD_ACC_GSM or RRD_ACC_GPRS ( if GPRS->RACH or PRACH )
  u8   TxRachNb   ; // Number of transmitted RACHs; 0xff means RACHing stopped
  bool RachOn     ; // TRUE if L1 is transmitting RACHs
  bool AccTimerOn ; // True if access timer is started
  bool Rejected   ; // True if a Reject was received 
  u8   WaitInd    ; // T3122/3142 or 3172 duration (seconds)
  bool Retry      ; // True if this is a retry after a cont. failure (GSM)
  u8   NextIdx    ; // Index of next writtable Request Reference 
  u16  Info[4]    ; // Storage for the last 3 Random Access Info fields 
                    // (4 instead of 3 makes circular mgt easier)
  u16  RFN[4]     ; // and their transmission time (4 instead of 3 makes 
                    // circular mgt easier)
} rrd_Access_t ;

/*
** RRD l1 connection parameters
*/
typedef struct
{
 u8 CxType;                     /* Which type of connection is active */
 union
 {
  struct 
  {
   l1_ConnectReq_t L1Param ; // L1 configuration for this connection
   u32       OTD            ; // Observed Time Difference
   bool      ROT            ; // True if Observed Time Diff must be reported 
                              // on this connection
   u8        Bsic           ; // Bsic of the new cell; only for intercell hov
   rr_CaL_t *CaSave         ; // Save for CA to be able to resume channel
  } rrg_L1Con;

  struct 
  {
   u8     ConnReq;        /* Who is asking for the Uplink connection ?      */
   u8     MacMode;        /* Allocation type (define in itf_l1)             */
   u8     TsBitmap[4];    /* Bitmap for allocated Timeslot L1_TBF_UL, DL    */
   u16    AllocLen;       /* Number of timeslot allocated in the last fixed */
   Msg_t *L1Param;        // Pointer to L1 configuration for this connection
                          // only used in case of immediate assignment
  } rrp_L1Con;
 } u;
} rrd_L1Con_t ;




#define RRP_TSBMP_NEXT_OFFSET 2

/*
** GSM context 
*/
typedef struct
{
  u8      V_SD           ; 
  u8      RrNwCause      ; // RR Cause to send to the network
  bool    CipCmdReceived ;
  u8      TchLoopMode    ; // value as defined in itf_l1.h(l1_SetTchLoop_t)
  u8      SacchCellOpts  ; // SACCH Cell options as received in last SysInf6
  u8      LastChanType   ; // Type of the last L1-connected channel
  u8      LapState[2]    ; // SAPI 0 and 3 connection state
  u8      FreshKc [8]    ; 
  u8      Kc [8]         ; 
  Msg_t * Sapi3DReq      ;
} rrg_Ctx_t;


/*
** Possible values for LapState
*/
#define    RRD_SAP_VOID  0 // Not Used
#define    RRD_SAP_WEST  1 // Waiting for Establishment
#define    RRD_SAP_ESTA  2 // Established
#define    RRD_SAP_SUSP  3 // Suspended
#define    RRD_SAP_WRES  4 // Waiting for resumption
#define    RRD_SAP_WREC  5 // Waiting for reconnection
#define    RRD_SAP_WREL  6 // Waiting for Release

/*
** Indexes to access lapState for each Sapi
*/
#define    RRD_SAP0_IDX     0
#define    RRD_SAP3_IDX     1

/*
** Masks to access SimAcc and CellAcc
*/
#define    RRD_ACC11_15_MSK  0x88
#define    RRD_ACC12TO14_MSK 0x70
#define    RRD_ACC8_9_MSK    0x03
#define    RRD_ACC_EMERG     0x04

#ifdef __RRP__
/*
** GPRS context 
*/
typedef struct
{
 u8     PFI;              
 u8     SegImmAss;
 u8     NbAccAttempt;     /* nb of access attempt                           */
 u8     UlEstabCnter;     /* nb of successive emission of ChanReq Desc      */
 u8     PCCFailCause;     
 u8     Bsic;             /* Current main parameters                        */
 #ifdef __EGPRS__
   u8 	     	AccTechType[14];
   u8			AccTechTypeSum;
   u8			AccTechTypeMark;
   u8		       AracRetran;
   u8		       PrrRetran;
   u8	   		LinkQualMeasMode;
   u8	   		EPolling;
   u8			OutOfMem;
   u8			PrrRetranBitCurPos;
 #endif
 u16    Arfcn;            /* Current main parameters                        */
 u8     SegImmAssReqRef[4];

 u8     GPRSTestMode;
 #ifdef __EGPRS__
 bool     AccTechTypePresent;
 bool     EgprsLoopbackModeOn;
 bool     EgprsLoopbackSubModeOn;
  #endif
 u32    Flags;
 Msg_t *RlcCon;
 Msg_t *PccoMsg;
  #ifdef __EGPRS__
 Msg_t *PrrBackUp;
 Msg_t* AracBackUp;
 c_PDownlinkAckNack EgprsDlAckMsg;
  #endif


  u8    UplinkRlcMode;

} rrp_Ctx_t;

/*
** Define for SegImmAss
*/
#define RRP_SIA_NO       0
#define RRP_SIA_FIRST_UL 1
#define RRP_SIA_FIRST_DL 2

/*
** Define for the ConnReq
*/
#define RRP_NO_CONN    0
#define RRP_CONN_RLC   1
#define RRP_CONN_RR    2

/*
** Define for the flags
*/
#define RRP_FLG_SGL_DL            ( 1 << 0 ) /* Dlk Single block Alloc      */
#define RRP_FLG_CLOSE_ENDED       ( 1 << 1 ) /* Is the TBF close-ended ?    */
#define RRP_FLG_UL_TBF_EXIST      ( 1 << 2 ) /* An Ulk TBF is in progress   */
#define RRP_FLG_DL_TBF_EXIST      ( 1 << 3 ) /* An Dlk TBF is in progress   */
#define RRP_FLG_CHAN_REQ_VALID    ( 1 << 4 ) /* ChanReq to be processed     */
#define RRP_FLG_FIRST_BLOCK       ( 1 << 5 ) /* Used for the T3164 handling */
#define RRP_FLG_CONTRES_MODE      ( 1 << 6 ) /* The cont res has not been ok*/
                                             /* => don't reset NbAccAttempt */
#define RRP_FLG_SUDDEN_REL        ( 1 << 7 ) /* PTBF Rel => "sudden" release*/
#define RRP_FLG_ABREL_WAR         ( 1 << 8 ) /* Ab Rel with AR => No resel. */
#define RRP_FLG_TBF_IN_PROG       ( 1 << 9 ) /* A TBF with a ST exists      */
#define RRP_FLG_CNTDWN_STARTED    ( 1 << 10 ) /* The countdown has started  */
#define RRP_FLG_PCCO_IN_PROG      ( 1 << 11 ) /* A PCCO is in progress      */
#define RRP_FLG_POLL_IMMASS       ( 1 << 12 ) 
#define RRP_FLG_T3176_EXP         ( 1 << 13 ) 
#define RRP_FLG_PCCO_ULEXIST      ( 1 << 14 ) 
#define RRP_FLG_PCCO_BACKOLD      ( 1 << 15 ) 
#define RRP_FLG_PCCO_RESRCVD      ( 1 << 16 ) 
#define RRP_FLG_PCCO_CONN         ( 1 << 17 ) 
#define RRP_FLG_ALERT_MMP         ( 1 << 18 ) 

//add by lincq for first ul tbf release while second ul tbf in prog on 2007.11.15 begin 
#define RRP_FLG_UL_TBF_IN_PROG         ( 1 << 19 ) 
//add by lincq for first ul tbf release while second ul tbf in prog on 2007.11.15 end

//add by fengw for gprs suspend when CS connect reqested if TBF exist begin
#define RRP_FLG_SUSPEND_GRPS            ( 1 << 20 )
//add by fengw for gprs suspend when CS connect reqested if TBF exist end


#define RRP_RLC_MODE_CHANGING          (1 << 21)

/*
** Define for TbfEstCause
*/
#define RRP_RR_MEASURE     4
#define RRP_RR_CELL_UPDATE 2
// Other values, see itf_rr.h in rrp_ConnectReq_t

#endif // __RRP__

/*
** RRD context 
*/
typedef struct 
{
  u8              RrEstCause  ; // same as received from MM or RLC (itf_rr.h)
  u8              RejEstCause ; // Estab Cause of the last rejected 
                                // connection attemps
  u8              NbAccAttempt; // Number of access procedure attempted
  u8              SimAcc[2]   ; // Ms Access classes from the SIM
  Msg_t         * L3InitMsg   ; // same as received by MM (itf_rr.h)
                                // Must be set to 0 AS SOON as RR frees its
                                // last instance
  rrd_Access_t  * Access      ; // pointer to RRD's local access parameters
  rrd_L1Con_t   * L1ConI      ; // pointer to initial l1 connection parameters
  rrd_L1Con_t   * L1ConN      ; // pointer to new l1 connection parameters
  rrg_Ctx_t       GsmCtx      ; // GSM Context
#ifdef __RRP__
  rrp_Ctx_t       GprsCtx     ; // Context GPRS
#endif

} rrd_Data_t ;

/*
** Define for CxType
*/
#define RRD_NO_CX   0
#define RRD_GSM_CX  1
#define RRD_GPRS_CX 2
#ifdef __EGPRS__
#define RRD_EGPRS_CX 3
#endif

#define RRG_CTX    RRD_DATA.GsmCtx
#define RRP_CTX    RRD_DATA.GprsCtx
#define RRG_L1CONI RRD_DATA.L1ConI->u.rrg_L1Con
#define RRG_L1CONN RRD_DATA.L1ConN->u.rrg_L1Con
#define RRG_L1PARI RRG_L1CONI.L1Param
#define RRG_L1PARN RRG_L1CONN.L1Param
#define RRP_L1CONI RRD_DATA.L1ConI->u.rrp_L1Con
#define RRP_L1CONN RRD_DATA.L1ConN->u.rrp_L1Con
#define RRP_L1PARI RRD_DATA.L1ConI->u.rrp_L1Con.L1Param
#define RRP_L1PARN RRD_DATA.L1ConN->u.rrp_L1Con.L1Param

#endif // __RRD_DATA_H__

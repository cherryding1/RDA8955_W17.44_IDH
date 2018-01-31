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
  File       : rr_iitf.h
--------------------------------------------------------------------------------

  Scope      : Internal Interface messages to RR

  History    :
--------------------------------------------------------------------------------
  Mar 01 06  |  MCE   | Suppressed TargetArfcn from rr_ReselReq_t
  Jan 31 06  |  MCE   | Addition of  RR_RSL_NETWORDER_FAIL reselection cause
  Mar 10 05  |  MCE   | Addition of RR_PLMN_UPD_IND          
  Aug 09 04  |  MCE   | Suppression of RR_AUTOCONNECT_REQ     
  Jun 09 04  |  MCE   | Addition of RR_GPRS_STATUS_IND        
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/

#ifndef __RR_IITF_H__
#define __RR_IITF_H__

#include "sxs_type.h"     // Comon definitions for standards types
#include "itf_msgc.h"     // Definition of Bases

#ifdef __RRP__
#include "ed_c.h"
#endif 


/*
================================================================================
  Data       : Cell Selection <-> Plmn Selection Primitives
--------------------------------------------------------------------------------

  Scope      : 

================================================================================
*/

#define RR_GPRS_INTRA_BASE (RR_INTRA_BASE + 0x800)//gprs intra id should not be larger than 0x400, because RR_PNLTY_ARFCN_BASE is based on it

// Cell Selection <--> Plmn Selection

/*
** Psel to Csel
*/
#define RR_PLMN_SEARCH_REQ       ( RR_INTRA_BASE + 1 )
#define RR_PLMN_ACCEPT_IND       ( RR_INTRA_BASE + 2 )
#define RR_PLMN_REJECT_IND       ( RR_INTRA_BASE + 3 )
#define RR_PLMN_SCAN_REQ         ( RR_INTRA_BASE + 4 )
#define RR_PLMN_STOP_SCAN_REQ    ( RR_INTRA_BASE + 5 )
#define RR_PLMN_INIT_REQ         ( RR_INTRA_BASE + 6 )
#define RR_PLMN_REFRESH_REQ      ( RR_INTRA_BASE + 7 )
#define RR_GPRS_STATUS_IND       ( RR_INTRA_BASE + 8 )

/*
** Csel to Psel
*/
#define RR_PLMN_FOUND_IND        ( RR_INTRA_BASE + 0x10 )
#define RR_PLMN_NOT_FOUND_IND    ( RR_INTRA_BASE + 0x11 )
#define RR_PLMN_LOST_IND         ( RR_INTRA_BASE + 0x12 )
#define RR_PLMN_AVAILABLE_IND    ( RR_INTRA_BASE + 0x13 )
#define RR_PLMN_LIST_DONE_IND    ( RR_INTRA_BASE + 0x14 )
#define RR_PLMN_UPD_IND          ( RR_INTRA_BASE + 0x15 )


/*
** Csel to Csel
*/
#define RR_BA_COMPLETE           ( RR_INTRA_BASE + 0x20 )
#define RR_LOOK_BESTNEAR         ( RR_INTRA_BASE + 0x21 )

// CSel Timers 
#define RR_RECNT_RSL_EXP         ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x25 )
#define RR_GUARD_RSL_EXP         ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x26 )
#define RR_PNLTY_RSL_EXP         ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x27 ) 
#define RR_NORET_RSL_EXP         ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x28 )
#define RR_ASLEEP_EXP            ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x29 )
#define RR_FRESH_SI_EXP          ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x2a )
#define RR_NONDRX_EXP            ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x2b )
#define RR_MONIT_IND_EXP         ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x2d )
#define RR_FORWARD_PAG_EXP       ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x2e )
#define RR_PNLTY_ARFCN_FLAG   ((SXS_TIMER_EVT_ID + RR_INTRA_BASE ) | 0x400) //fengw modified for pnlty for each cell should have 
#define RR_PNLTY_ARFCN_BASE   RR_PNLTY_ARFCN_FLAG//individual timer id
#define RR_PNLTY_ARFCN_END    (RR_PNLTY_ARFCN_BASE |0x3FF)
#define RR_ASLEEP_ARFCN_FLAG  ((SXS_TIMER_EVT_ID + RR_INTRA_BASE ) | 0x1000) 
#define RR_ASLEEP_ARFCN_BASE   RR_ASLEEP_ARFCN_FLAG         
#define RR_ASLEEP_ARFCN_END    (RR_ASLEEP_ARFCN_BASE |0x3FF)
/*
** Psel to Psel
*/
#define RR_PLMN_SIM_LOADED       ( RR_INTRA_BASE + 0x30 )

// PSel Timers 
#define RR_NSVC_TIM_EXP          ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x31 )
#define RR_LTDSVC_TIM_EXP        ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x32 )
#define RR_HPLMN_TIM_EXP         ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x33 )
#define RR_LOCATION_LOCK_TIM_EXP  (SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x34)
#define RR_SEND_ADJ_CREPORT_TIM_EXP (SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x35)
/*
** RRD to RRD
*/
#define RR_L3DATA_IND            ( RR_INTRA_BASE + 0x40 )

/*
** Rrd to Csel (and PSel)
*/
#define RR_RESEL_REQ             ( RR_INTRA_BASE + 0x51 )
#define RR_L1CON_IND             ( RR_INTRA_BASE + 0x52 )
#define RR_CSCON_IND             ( RR_INTRA_BASE + 0x53 )
#define RR_PSCON_IND             ( RR_INTRA_BASE + 0x54 )
#define RR_L1NOTCON_IND          ( RR_INTRA_BASE + 0x55 )
#define RR_CELLCHANGE_IND        ( RR_INTRA_BASE + 0x56 )
#define RR_L1TRANSF_IND          ( RR_INTRA_BASE + 0x57 )
#define RR_L1ACCESS_IND          ( RR_INTRA_BASE + 0x58 )
#define RR_PCCO_IND              ( RR_INTRA_BASE + 0x59 )
#define RR_PCCO_COMPLETE         ( RR_INTRA_BASE + 0x5a )


/*
** Csel to Rrd
*/
#define RR_RESEL_GRANT_REQ       ( RR_INTRA_BASE + 0x60 )
#define RR_SEND_MEAS_REQ         ( RR_INTRA_BASE + 0x61 )
#define RR_PCCO_RESULT           ( RR_INTRA_BASE + 0x62 )


/*
** Timers
*/
#define RR_T3122_TIM             ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x70 )
#define RR_T3124_TIM             ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x71 )
#define RR_T3126_TIM             ( SXS_TIMER_EVT_ID + RR_INTRA_BASE + 0x72 )

/*
** Sub events (derived from radio messages or from  L1)
*/
#define RRD_IMM_ASS_CMD          ( RR_INTRA_BASE + 0x80 )
#define RRD_IMM_ASS_EXT_CMD      ( RR_INTRA_BASE + 0x81 )
#define RRD_IMM_ASS_REJ          ( RR_INTRA_BASE + 0x82 )
#define RRD_ASS_CMD              ( RR_INTRA_BASE + 0x83 )
#define RRD_HOV_CMD              ( RR_INTRA_BASE + 0x84 )
#define RRD_PHY_INFO             ( RR_INTRA_BASE + 0x85 )
#define RRD_CHAN_REL             ( RR_INTRA_BASE + 0x86 )
#define RRD_CIP_MOD_CMD          ( RR_INTRA_BASE + 0x87 )
#define RRD_CHN_MOD              ( RR_INTRA_BASE + 0x88 )
#define RRI_SYSINFO_IND          ( RR_INTRA_BASE + 0x89 )
#define RRI_PAGING_IND           ( RR_INTRA_BASE + 0x8a )
#define RRI_L1_ERROR             ( RR_INTRA_BASE + 0x8b )
#define RRD_L1_ERROR             ( RR_INTRA_BASE + 0x8c )
#define RRI_SYS_INF5_IND         ( RR_INTRA_BASE + 0x8d )
#define RRI_SYS_INF5B_IND        ( RR_INTRA_BASE + 0x8e )
#define RRI_SYS_INF5T_IND        ( RR_INTRA_BASE + 0x8f )
#define RRI_SYS_INF6_IND         ( RR_INTRA_BASE + 0x90 )
#define RRI_EXT_MEAS_ORD         ( RR_INTRA_BASE + 0x91 )
#define RRD_CMK_ENQ              ( RR_INTRA_BASE + 0x92 )
#define RRD_FRQ_RDF              ( RR_INTRA_BASE + 0x93 )
#define RRI_PSYS_INF             ( RR_INTRA_BASE + 0x94 )
#define RRD_STATUS               ( RR_INTRA_BASE + 0x95 )

#define RRI_PSYS_INFO_IND        ( RR_GPRS_INTRA_BASE + 0x00 )
#define RRI_PMO_IND              ( RR_GPRS_INTRA_BASE + 0x01 )

#ifdef __GPRS__
/*
** MAC to RRP
*/
#define MAC_CTRLDATA_IND         ( RR_GPRS_INTRA_BASE + 0x10 )

/*
** RR to MAC
*/
#define MAC_RRDATA_REQ           ( RR_GPRS_INTRA_BASE + 0x16 )

#define MAC_QUEUEING_NOTIF       ( RR_GPRS_INTRA_BASE + 0x11 )
#define MAC_PULACKNACK_IND       ( RR_GPRS_INTRA_BASE + 0x12 )
#define MAC_CONTRES_FAIL         ( RR_GPRS_INTRA_BASE + 0x13 )
#define MAC_ACCREJ_IND           ( RR_GPRS_INTRA_BASE + 0x14 )
#define MAC_ESTAB_REQ            ( RR_GPRS_INTRA_BASE + 0x15 )

/*
** MAC events
*/
#define MAC_DATA_IND           ( RR_GPRS_INTRA_BASE + 0x17 )
#define MAC_T3166_EXP          ((RR_GPRS_INTRA_BASE + 0x18) | SXS_TIMER_EVT_ID)

#define MAC_T3200A_EXP         ((RR_GPRS_INTRA_BASE + 0x20) | SXS_TIMER_EVT_ID)
#define MAC_T3200B_EXP         ((RR_GPRS_INTRA_BASE + 0x21) | SXS_TIMER_EVT_ID)
#define MAC_T3200C_EXP         ((RR_GPRS_INTRA_BASE + 0x22) | SXS_TIMER_EVT_ID)
#define MAC_T3200D_EXP         ((RR_GPRS_INTRA_BASE + 0x23) | SXS_TIMER_EVT_ID)
#define MAC_T3200E_EXP         ((RR_GPRS_INTRA_BASE + 0x24) | SXS_TIMER_EVT_ID)
#define MAC_T3200F_EXP         ((RR_GPRS_INTRA_BASE + 0x25) | SXS_TIMER_EVT_ID)
#define MAC_T3200G_EXP         ((RR_GPRS_INTRA_BASE + 0x26) | SXS_TIMER_EVT_ID)
#define MAC_T3200H_EXP         ((RR_GPRS_INTRA_BASE + 0x27) | SXS_TIMER_EVT_ID)
#define MAC_T3200_EXP          MAC_T3200A_EXP

/*
** RRP events
*/
#define RRP_T3142_EXP          ((RR_GPRS_INTRA_BASE + 0x2A) | SXS_TIMER_EVT_ID)
#define RRP_T3146_EXP          ((RR_GPRS_INTRA_BASE + 0x2B) | SXS_TIMER_EVT_ID)
#define RRP_T3162_EXP          ((RR_GPRS_INTRA_BASE + 0x2C) | SXS_TIMER_EVT_ID)
#define RRP_T3164_EXP          ((RR_GPRS_INTRA_BASE + 0x2D) | SXS_TIMER_EVT_ID)
#define RRP_T3168_EXP          ((RR_GPRS_INTRA_BASE + 0x2E) | SXS_TIMER_EVT_ID)
#define RRP_T3170_EXP          ((RR_GPRS_INTRA_BASE + 0x2F) | SXS_TIMER_EVT_ID)
#define RRP_T3172_EXP          ((RR_GPRS_INTRA_BASE + 0x30) | SXS_TIMER_EVT_ID)
#define RRP_T3174_EXP          ((RR_GPRS_INTRA_BASE + 0x31) | SXS_TIMER_EVT_ID)
#define RRP_T3176_EXP          ((RR_GPRS_INTRA_BASE + 0x32) | SXS_TIMER_EVT_ID)
#define RRP_T3180_EXP          ((RR_GPRS_INTRA_BASE + 0x33) | SXS_TIMER_EVT_ID)
#define RRP_T3184_EXP          ((RR_GPRS_INTRA_BASE + 0x34) | SXS_TIMER_EVT_ID)
#define RRP_T3186_EXP          ((RR_GPRS_INTRA_BASE + 0x35) | SXS_TIMER_EVT_ID)
#define RRP_T3188_EXP          ((RR_GPRS_INTRA_BASE + 0x36) | SXS_TIMER_EVT_ID)
#define RRP_T3190_EXP          ((RR_GPRS_INTRA_BASE + 0x37) | SXS_TIMER_EVT_ID)
#define RRP_T3192_EXP          ((RR_GPRS_INTRA_BASE + 0x38) | SXS_TIMER_EVT_ID)
#define RRP_SEGIMMASS_EXP      ((RR_GPRS_INTRA_BASE + 0x39) | SXS_TIMER_EVT_ID)

#define RRP_IMMASS_CMPLT       ( RR_GPRS_INTRA_BASE + 0x3A )
#define RRP_PUL_ASS            ( RR_GPRS_INTRA_BASE + 0x3B )
#define RRP_PDL_ASS            ( RR_GPRS_INTRA_BASE + 0x3C )
#define RRP_PTS_RECONF         ( RR_GPRS_INTRA_BASE + 0x3D )
#define RRP_PPDCH_REL          ( RR_GPRS_INTRA_BASE + 0x3E )
#define RRP_PPWRCTRL_TA        ( RR_GPRS_INTRA_BASE + 0x3F )
#define RRP_PTBF_REL           ( RR_GPRS_INTRA_BASE + 0x40 )
#define RRP_PCCO               ( RR_GPRS_INTRA_BASE + 0x41 )

// Mailbox timers
#define RRP_ULRELTBF_DELAYED   ((RR_GPRS_INTRA_BASE + 0x50) | SXS_TIMER_EVT_ID)
#define RRP_DLRELTBF_DELAYED   ((RR_GPRS_INTRA_BASE + 0x51) | SXS_TIMER_EVT_ID)
#define RRP_SGLDL_EXP          ((RR_GPRS_INTRA_BASE + 0x52) | SXS_TIMER_EVT_ID)
#define RRP_SGLUL_EXP          ((RR_GPRS_INTRA_BASE + 0x53) | SXS_TIMER_EVT_ID)

#endif // __GPRS__


// definition of Reselection Causes               
#define        RR_RSL_NORESEL        0  // No reselection requested
#define        RR_RSL_BTERNEIGH      1  // Better neighbour 
#define        RR_RSL_RLFAIL         2  // Radio link Failure          
#define        RR_RSL_C1BCRIT        3  // Main is not suitable anymore
#define        RR_RSL_RACHFAIL       4  // Rach failure
#define        RR_RSL_NORMREL        5  // Normal channel release
#define        RR_RSL_ABNORMREL      6  // Abnormal GPRS Release
#define        RR_RSL_RESTBLSHT      7  // Reestablishment        
#define        RR_RSL_NETWORDER      8  // Network oder a  Cell Change 
#define        RR_RSL_RETMAIN        9  
#define        RR_RSL_BADZONE       10  

/*
================================================================================
  Primitive  : PlmnSearchReq                                 
--------------------------------------------------------------------------------

  Scope      : Plmn Selection request Cell Selection to find a cell of PlmnId

================================================================================
*/
typedef struct 
{
   u8          PlmnId[3]   ;  // Mcc + Mnc 

} rr_PlmnSearchReq_t;


/*
================================================================================
  Primitive  : PlmnAcceptInd                                 
--------------------------------------------------------------------------------

  Scope      : Plmn Selection notifies Cell Selection that the Plmn is Ok     
               This can occur 
                  - after MM has accepted the Plmn or 
                  - immediately in limited sercvice

================================================================================
*/
                        u8 rri_TrsnReinit              (void);
typedef struct 
{
   u8          PlmnId[3]   ;  // Mcc + Mnc 

} rr_PlmnAcceptInd_t;

/*
================================================================================
  Primitive  : PlmnRejectInd                                 
--------------------------------------------------------------------------------

  Scope      : Plmn Selection notifies Cell Selection that the Plmn is Ko     

================================================================================
*/
typedef struct 
{
   u8          PlmnId[3]   ;  // Mcc + Mnc 

} rr_PlmnRejectInd_t;

/*
================================================================================
  Primitive  : PlmnScanReq                                   
--------------------------------------------------------------------------------

  Scope      : Plmn Selection ask Cell Selection to scan all possible Plmns

================================================================================
*/
typedef struct 
{
   u8          Dummy     ;  // No content is needed

} rr_PlmnScanReq_t;

/*
================================================================================
  Primitive  : PlmnStopScanReq                                   
--------------------------------------------------------------------------------

  Scope      : Plmn Selection ask Cell Selection to stop the ongoing scan

================================================================================
*/
typedef struct 
{
   u8          Dummy     ;  // No content is needed

} rr_PlmnStopScanReq_t;

/*
================================================================================
  Primitive  : GprsStatusInd                                   
--------------------------------------------------------------------------------

  Scope      : Plmn Selection gives Cell the status of the GPRS 

================================================================================
*/
typedef struct 
{
   bool        Registered ;

} rr_GprsStatusInd_t;

/*
================================================================================
  Primitive  : PlmnFoundInd
--------------------------------------------------------------------------------

  Scope      : Cell Selection notifies Plmn Selection that 
               The requested Plmn (from last PlmnSearchReq) has been found

================================================================================
*/
typedef struct 
{
   u8          Lai [5]    ;  // Mcc + Mnc + Lac
   u8          NasUpdType ;  
} rr_PlmnFoundInd_t ;




#define RR_UPD_NONAS 0
#define RR_UPD_MM    ( 1 << 0 ) 
#define RR_UPD_CB    ( 1 << 1 )


/*
================================================================================
  Primitive  : PlmnUpdInd
--------------------------------------------------------------------------------

  Scope      : Cell Selection notifies Plmn Selection that 
               Nas Info has changed on current PLMN
================================================================================
*/
typedef rr_PlmnFoundInd_t rr_PlmnUpdInd_t ;

/*
================================================================================
  Primitive  : PlmnNotFoundInd
--------------------------------------------------------------------------------

  Scope      : Cell Selection notifies Plmn Selection that 
               The requested Plmn (from last PlmnSearchReq) cannot be found

================================================================================
*/
typedef struct 
{
   u8          PlmnId[3]   ;  // Mcc + Mnc 

} rr_PlmnNotFoundInd_t ;

/*
================================================================================
  Primitive  : PlmnLostInd
--------------------------------------------------------------------------------

  Scope      : Cell Selection notifies Plmn Selection that 
               The current Plmn (Last that was accepted) is lost 

================================================================================
*/
typedef struct 
{
   u8          PlmnId[3]   ;  // Mcc + Mnc 

} rr_PlmnLostInd_t ;

/*
================================================================================
  Primitive  : PlmnAvailableInd
--------------------------------------------------------------------------------

  Scope      : Cell Selection notifies Plmn Selection that 
               A Plmn (PlmnId) is seen has available 
               This message can occur when an scan is underway 
               (ie after a PlmnScanReq has been sent by Plmn Selection)

================================================================================
*/
typedef struct 
{
   u8          Lai[5]   ;  // Mcc + Mnc + Lac

} rr_PlmnAvailableInd_t;

/*
================================================================================
  Primitive  : PlmnListDoneInd
--------------------------------------------------------------------------------

  Scope      : Cell Selection notifies Plmn Selection that 
               The Plmn List of available Plmns is complete 
               A PlmnScanReq has been previously sent by Plmn Selection

================================================================================
*/
typedef struct 
{
   u8          Plmn[3]   ;  // Mcc + Mnc 

} rr_PlmnListDoneInd_t;

/*
================================================================================
  Primitive  : BaComplete        
--------------------------------------------------------------------------------

  Scope      : A New Ba is Complete for Main Cell          

================================================================================
*/
typedef struct 
{
   u16         Arfcn ; 
   bool        Psi   ;  // Built from PSi3/3bis

} rr_BaComplete_t;

/*
================================================================================
  Primitive  : rr_ReselGrantReq     
--------------------------------------------------------------------------------

  Scope      : Cell Selection request RRd to grant the reselection

================================================================================
*/
typedef struct 
{
   u8          ReselCause; 

}rr_ReselGrantReq_t;

/*
================================================================================
  Primitive  : rr_ReselReq       
--------------------------------------------------------------------------------

  Scope      : RRD orders RRI to perform a reselection 

Note         : Can be used as a Reselection Grant Response   
================================================================================
*/
typedef struct 
{
   u8          ReselCause  ; 

}rr_ReselReq_t;


/*
================================================================================
  Primitive  : rr_CellChangeInd       
--------------------------------------------------------------------------------

  Scope      : RRD indicates RRI that the main cell has changed
================================================================================
*/
typedef struct 
{
   u8          Bsic  ;
   u16         Arfcn ;
}rr_CellChangeInd_t ;


/*
================================================================================
  Primitive  : rr_PCCOResult_t
--------------------------------------------------------------------------------

  Scope      : RRI indicates RRD the result of the reselection following a
               Packet Cell Change Order
================================================================================
*/
typedef struct 
{
 u8  Result;
 u8  TargetBsic;
 u16 TargetArfcn;
} rr_PCCOResult_t;


#define RR_PCCORES_SUCCESS    0
#define RR_PCCORES_FAIL_OLD   1
#define RR_PCCORES_FAIL_OTHER 2
#define RR_PCCORES_BACKOLD_OK 3
#define RR_PCCORES_BACKOLD_KO 4

/*
================================================================================
  Primitive  : rr_PCCOComplete_t
--------------------------------------------------------------------------------

  Scope      : RRD indicates RRI if PCCO is Completed successfully or not
               Packet Cell Change Order
================================================================================
*/
typedef struct 
{
 u8 Cause       ;  //0 OK,  Otherwise not OK
} rr_PCCOComplete_t;


/*
===============================================================================
  Primitive  : rr_L1ConInd_t     
--------------------------------------------------------------------------------

  Scope      : RRD Indicates to RRI a beginning of a connexion 

  Note       : 
================================================================================
*/
#define RR_L1_RELEASE  0 // Used internally by RRI_C
#define RR_L1_ACCESS   1
#define RR_L1_CONNECT  2
#define RR_L1_TRANSFER 3

typedef struct 
{
   u8          ConnState ;  // See defines above

}rr_L1ConInd_t;


/*
===============================================================================
  Primitive  : rr_L1NotConInd_t     
--------------------------------------------------------------------------------

  Scope      : RRD Indicates to RRI the end of a connection

  Note       : 
================================================================================
*/

typedef struct 
{
   u8          WithSysInfo ;  // See defines above

}rr_L1NotConInd_t;

/*
================================================================================
  Primitive  : CtrlDataInd
--------------------------------------------------------------------------------

  Scope      : MAC is forwarding a control data block to RRP
  Comment    : interlayer message. DataOffset = nb of bits
                                   DataLen    = nb of bytes

================================================================================
*/
typedef struct 
{
 s16 Arfcn    ;  // Synchronized Arfcn
 u8  MessageType;
   /*modified by lincq on 2006/11/17*/
  u8 ChnId;
   /*modified by lincq on 2006/11/17*/
 u32 Timestamp;
} mac_CtrlDataInd_t,
  rr_PCCOInd_t;

/*
================================================================================
  Primitive  : mac_rrDataReq
--------------------------------------------------------------------------------

  Scope      : RR ask to MAC to send a Packet Control
  Comment    : interlayer message. DataOffset = nb of bits
                                   DataLen    = nb of bytes

================================================================================
*/
#ifdef __EGPRS__
#define RR_PRESREQ_PRIO   1
#define RR_AMSCAP_PRIO  2
#endif
typedef struct 
{
#ifdef __EGPRS__
u8 Prioritary;
#else
 bool Prioritary; 
#endif
} mac_RrDataReq_t;

#ifdef __RRP__
/*
================================================================================
  Primitive  : mac_PUlAckNackInd
--------------------------------------------------------------------------------

  Scope      : MAC is forwarding a PACKET UPLINK ACK/NACK to RRP

================================================================================
*/
typedef struct 
{
 u8                 BlkIndex  ;
 u16                Timestamp ;
 c_PUplinkAckNack * PUlAckNack;
} mac_PUlAckNackInd_t;

/*
================================================================================
  Primitive  : mac_PQueNotif_t
--------------------------------------------------------------------------------

  Scope      : MAC is forwarding a PACKET QUEUEING NOTIFICATION to RRP

================================================================================
*/
typedef struct 
{
 u16 TQI;
} mac_PQueNotif_t;

/*
================================================================================
  Primitive  : rrp_ImmAssCmp_t
--------------------------------------------------------------------------------

  Scope      : RRP send this message to itself to indicate the recieving of
               a complete Immediate Assignment

================================================================================
*/
typedef struct 
{
 u8 AssgtType;
} rrp_ImmAssCmp_t;

#define RRP_ASS_UL 1
#define RRP_ASS_DL 2


/*
================================================================================
  Primitive  : mac_AccRej_t
--------------------------------------------------------------------------------

  Scope      : MAC inform RRP that a Packet Access Reject has been received
               with a Wait Indication.

================================================================================
*/
typedef struct 
{
 u32 WaitTime;
} mac_AccRej_t;

#endif // __RRP__

#endif

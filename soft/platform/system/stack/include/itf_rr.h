/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : ITF_RR.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by RR
  History    :
--------------------------------------------------------------------------------
  Apr 07 06  |  MCE   | Added RR_NO_PAN_XXX
  Mar 10 06  |  OTH   | Addition of UsfGranularity in rrp_UlConnectCnf
  Mar 03 06  |  OTH   | Addition of a Release cause between RR and RLC
  Jan 23 06  |  OTH   | Removed rrp_DlRelInd_t body (not used)
             |        | Redefined RRP_ULREL_IND into RR_RELEASE_IND
  Jan 06 06  |  OTH   | Added RR_TBF_END
  Nov 19 05  |  OTH   | Add the PFI related fields in rrp_ConnectReq_t and 
             |        | rrp_UlConnectCnf_t structures.
  Nov 10 05  |  PFA   | Add Cur and Adj Cell Info Req/ Ind     
  Nov 10 05  |  OTH   | Change mac_NewTlliReq to mac_NewTlliCnf
  Oct 04 05  |  MCE   | Added RR_BAD_STATE
  Sep 27 05  |  MCE   | Removed RR_NAS_UPDATE_IND and rr_NasUpdateInt_t
  Aug 22 05  |  MCE   | replaced RR_REsELECTION_CAUSE and RR_UNACKED_CELL with
             |        | RR_NO_CELL
  Aug 10 05  |  MCE   | Added GprsResume in rr_ReleaseInd_t ; rrp_UlReleaseInd_t
             |        | has a structure of its own now (no GprsResume field)
  Jul 05 05  |  OTH   | Change BlockGranted from u8 to u16 because of fixed
             |        | allocation in rrp_UlConnectCnf_t.
  May 22 05  |  MCE   | Added RR_NAS_UPDATE_IND    
  Apr 22 05  |  MCE   | Added MAC_PURGE_DATAQ_REQ
  Apr 11 05  |  MCE   | Added RR_ENDCONGEST_IND
  Mar 04 05  |  MCE   | Added RR_GPRS_PENDING 
  Feb 25 05  |  MCE   | Added rr_NmoCfg_t and RR_NMO_REQ
  Jan 19 05  |  OTH   | Added IsRdyTimerEqZero bool in rr_MmpReadyStInd_t
  Dec 08 04  |  FCH   | Added RR_CBCH_REQ and rr_CbchReq_t
  Nov 18 04  |  MCE   | Added RR_NW_REG_PENDING 
  Nov 05 04  |  FCH   | Change RR_P_NOT_UPDATED by RR_P_REG_PENDING and added
             |        | RR_P_NWK_DET for rr_LocUpSta_t Status
  Nov 01 04  |  MCE   | Added RR_REEST_TCHH and RR_REEST_TCHF (and re-ordered)
  Sep 22 04  |  MCE   | Added RR_GPRS_REQ and rr_GprsReq_t
  Sep 16 04  |  MCE   | Added RR_RESELECTION_CAUSE and RR_UNACKED_CELL for 
             |        | rr_ReleaseInd_t Cause 
  Aug 18 04  |  FCH   | Align rr_RaUpdateStatus_t with rr_LocUpSta_t
  Aug 09 04  |  OTH   | Added RR_REPEAT_REEST cause in the rr_ConnectReq_t
  Jun 22 04  |  MCE   | Added rr_MmpProcInd_t and RR_MMPPROC_IND
  Jun 09 04  |  MCE   | Added rr_RaUpdateStatus_t and RR_RAUPD_STA
  Jun 08 04  |  OTH   | Added rr_ChanModeInd_t for CC
  Apr 29 04  |  MCE   | Added rr_CmkInd_t and RR_CMK_IND
  Apr 23 04  |  MCE   | Changed Classmark1 -> Classmark2 in CellInd
  Apr 02 04  |  PFA   | Add Classmark1 in CellInd
  Aug 02 03  |  MCE   | Creation            
================================================================================
*/

#ifndef __ITF_RR_H__
#define __ITF_RR_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_msgc.h"
#include "itf_l1.h"  // for ChanMode possible values
#include "itf_api.h"

/*
** RR Internal msg Start at INTRABASE + 0 
*/
#define RR_GPRS_BASE  ( HVY_RR + 0x0400 )
#define RR_INTRA_BASE ( HVY_RR + 0x8000 )

/*
** Primitive ID definitions
*/
#define RR_REINIT_REQ         ( HVY_RR + 0x1 )
#define RR_CONNECT_REQ        ( HVY_RR + 0x2 )
#define RR_CONNECT_CNF        ( HVY_RR + 0x3 )
#define RR_RELEASE_IND        ( HVY_RR + 0x4 )
#define RR_ABORT_REQ          ( HVY_RR + 0x5 )
#define RR_DATA_REQ           ( HVY_RR + 0x6 )
#define RR_DATA_IND           ( HVY_RR + 0x7 )
#define RR_CELL_IND           ( HVY_RR + 0x8 )
#define RR_CELL_RSP           ( HVY_RR + 0x9 )
#define RR_LOCUP_STA          ( HVY_RR + 0xa )
#define RR_PAGING_IND         ( HVY_RR + 0xb )
#define RR_KC_IND             ( HVY_RR + 0xc )
#define RR_CIPH_IND           ( HVY_RR + 0xd )
#define RR_CMK_IND            ( HVY_RR + 0xe )
#define RR_CHANMODE_IND       ( HVY_RR + 0xf ) 

#define RR_NW_SELMODE_REQ     ( HVY_RR + 0x10 )
#define RR_NW_LIST_REQ        ( HVY_RR + 0x11 )
#define RR_NW_LIST_ABORT_REQ  ( HVY_RR + 0x12 )
#define RR_NW_LIST_ABORT_IND  ( HVY_RR + 0x13 )
#define RR_NW_LIST_IND        ( HVY_RR + 0x14 )
#define RR_NW_SEL_REQ         ( HVY_RR + 0x15 )
#define RR_NW_RESEL_REQ       ( HVY_RR + 0x16 )
#define RR_NW_CS_REG_IND      ( HVY_RR + 0x17 )
#define RR_NW_PS_REG_IND      ( HVY_RR + 0x18 )
#define RR_QUAL_REPORT_REQ    ( HVY_RR + 0x19 )            
#define RR_QUAL_REPORT_CNF    ( HVY_RR + 0x1a )            
#define RR_CBCH_REQ           ( HVY_RR + 0x1b )            
#define RR_NMO_CFG_REQ        ( HVY_RR + 0x1c )
#define RR_ENDCONGEST_IND     ( HVY_RR + 0x1d )

#define RR_CURCELL_INFO_REQ   ( HVY_RR + 0x1e )
#define RR_ADJCELL_INFO_REQ   ( HVY_RR + 0x1f )
#define RR_CURCELL_INFO_IND   ( HVY_RR + 0x20 )
#define RR_ADJCELL_INFO_IND   ( HVY_RR + 0x21 )
#define RR_NW_STORE_LIST_IND   (HVY_RR + 0x22)

#define RR_SYNC_INFO_REQ        (HVY_RR + 0x23)    
#define RR_POWER_LIST_IND     (HVY_RR + 0x24)
#define RR_SYNC_INFO_IND        (HVY_RR + 0x25)

#define RR_POWER_SCAN_REQ  (HVY_RR + 0x26)
#define RR_POWER_SCAN_RSP        ( HVY_RR + 0x27 )

#define RR_DETECT_MBS_REQ     ( HVY_RR + 0x28 )

/*
================================================================================
  Structure  : rr_ConnectReq_t
--------------------------------
  Direction  : MM -> RR
  Scope      : asks for the establishment of an RR connection
  Comment    : InterLayer message
================================================================================
*/
typedef struct 
{
  u8 Cause  ;  // Cause for the establishment 
} rr_ConnectReq_t ;

/*
** Possible values for Cause
*/
 /*
 ** CS causes 
 ** !!!! Always add values at the end
 */
#define RR_EMERG        0  // Emergency call
#define RR_REEST_TCHH   1  // Reestablisht after loosing a TCHH ; RR use only
#define RR_REEST_TCHF   2  // Reestablisht after loosing a TCHF ; RR use only
#define RR_PAGRSP_ANYCH 3  // Answer to paging where Channel Needed=Any Channel
#define RR_PAGRSP_SDCCH 4  // Answer to paging where Channel Needed=SDCCH
#define RR_PAGRSP_TCHF  5  // Answer to paging where Channel Needed=TCH/F
#define RR_PAGRSP_TCH   6  // Answer to paging where Channel Needed=TCH/F or H
#define RR_MO_TCHF      7  // MO call and TCHF is needed
#define RR_MO_TCHH      8  // MO call and TCHH is sufficient
#define RR_MO_SD_PROC   9  // MO Procedure that can be completed with a SDCCH
#define RR_LOCUP        10 // Location updating

 /*
 ** PS causes 
 ** for RR use only
 */
#define RR_ONE_PHASE      11  // One phase packet access
#define RR_SGLE_TWO_PHASE 12  // Single block and/or two phase packet access

 /*
 ** Temporary causes, not used by RR to generate channel request cause
 */
#define RR_REEST        13 // Call re-establishment
#define RR_REPEAT_REEST 14 // Repeat the re-establishment
 /*
 ** For RR use only
 */
#define RR_NB_CAUSE    13  // Nb of itf causes (excludes internal causes)
                           // 11 for CS + 2 for PS
#define RR_NO_CAUSE    0xFF

/*
================================================================================
  Message  : RR_CONNECT_CNF
--------------------------------
  Direction  : MM -> RR
  Scope      : Indicate the successful RR connection
  Comment    : No body
================================================================================
*/
/*
================================================================================
  Structure  : rr_ReleaseInd_t
--------------------------------
  Direction  : RR -> MM or RLG
  Scope      : RR indicates that the connection failed to establish, or 
               was released
  Comment    : 
================================================================================
*/
typedef struct
{
  u8 Cause        ;
  u8 GprsResume   ;  
} rr_ReleaseInd_t ;


#define RR_NORMAL_RELEASE        0 
#define RR_ACCESS_BARRED         1  // also for MMP
#define RR_ASSIGNMENT_REJECT     2  // also for MMP
#define RR_RANDOM_ACCESS_FAILURE 3  // also for MMP
#define RR_CONNECTION_FAILURE    4
#define RR_NO_CELL               5 
#define RR_BAD_STATE             6
// GPRS specific reject cause
#define RR_ABNORMAL_REL_WAR      10  // RR -> RLC only
#define RR_ABNORMAL_REL_NOAR     11  // RR -> RLC only
#define RR_P_LOWER_LAYER_FAILURE 12  // RR -> MMP
#define RR_TBF_END               13  // RR -> MMP
#define RR_RESELECTION           14  // RR -> RLC only


#define RR_GPRS_RESUME_NS        0  // Gprs resume not significant
#define RR_GPRS_RESUME_KO        1  // Resume failed
#define RR_GPRS_RESUME_OK        2  // Resume succeeded
#define RR_GPRS_SUSPENDED        4  // for RR internal use only


/*
================================================================================
  Message    : RR_DATA_REQ
--------------------------------
  Direction  : MM -> RR
  Scope      : asks for the transmission of MM signaling 
  Comment    : InterLayer message
               No specific header 
================================================================================
*/

/*
================================================================================
  Message    : RR_DATA_IND 
--------------------------------
  Direction  : RR -> MM
  Scope      : indicates the reception of MM signaling   
  Comment    : InterLayer message
               No specific header 
================================================================================
*/

/*
================================================================================
  Structure  : rr_CellInd_t
--------------------------------
  Direction  : RR -> MM
  Scope      : indicates a new cell is camped on
  Comment    : 
================================================================================
*/
typedef struct 
{
 u16  Arfcn;
 u8   Bsic;
 bool AttFlag;
 u8   T3212Duration;
 u8   AvailableServ;
 u8   Cbch;               // Supported CBCH
 union {
  u8  Rai[6];
  struct {
   u8 Lai[5];
   u8 Rac;
  }rr;
 }u;
 u8   NMO;                      /* Network Mode of Operation (packet) */
 u8   Classmark2[3];
 #ifdef __GPRS__
 u8   PrioAccThr;
 #endif
} rr_CellInd_t;

/*
** Possible values for AvailableServ
*/
#define RR_NO_SERVICE     0
#define RR_EMERGENCY_ONLY 1
#define RR_FULL_SERVICE   2
#ifdef __EGPRS__
#define RR_EGPRS_OFFERED        (1 << 5 ) //added by leiming for EGPRS supporting
#endif
#define RR_GPRS_PENDING   ( 1 << 6 )
#define RR_GPRS_OFFERED   ( 1 << 7 )

/*
** Possible values for Cbch
*/
#define RR_NO_CBCH        0
#define RR_BASIC_CBCH     ( 1 << 0 )
#define RR_EXTENDED_CBCH  ( 1 << 1 )

/*
** Possible values for NMO
*/
#define RR_NMO1    0  // Network mode of operation I
#define RR_NMO2    1  // Network mode of operation II
#define RR_NMO3    2  // Network mode of operation III


/*
================================================================================
  Structure  : rr_CellRsp_t
--------------------------------
  Direction  : MM -> RR 
  Scope      : acknowledged the indicated cell
  Comment    : 
================================================================================
*/
typedef struct 
{
  u16 Arfcn ;
} rr_CellRsp_t ;

/*
================================================================================
  Message    : RR_ABORT_REQ
--------------------------------
  Direction  : MM -> RR
  Scope      : Request an abort
  Comment    : No body
               No specific header 
================================================================================
*/

/*
================================================================================
  Message    : RR_PAGING_IND  
--------------------------------
  Direction  : RR -> MM
  Scope      : RR forward the last received paging
  Comment    : InterLayer message
               No specific header 
================================================================================
*/



//================================================================================
//  Message    : RR_ABORT_REQ
//--------------------------------
//  Direction  : MM -> RR
//  Scope      : Abort RR connection 
//================================================================================
typedef struct 
{
	 u8   Domain;  // CS STK_CNFFSM_MMC; PS STK_CNFFSM_MMP;
} rr_AbortReq_t;
/*
================================================================================
  Structure  : rr_LocUpSta_t
--------------------------------
  Direction  : MM -> RR
  Scope      : a location update has been done
  Comment    : Msg
================================================================================
*/
// for R99 15, for R6 45, test in IDEA network 201609
#define MAX_EQPLMN_IE_LEN  45
typedef struct 
{
 bool EqPlmnChange;
 u8   Status;
 u8   EqPlmnListSize;
 u8   EqPlmnList[MAX_EQPLMN_IE_LEN];
} rr_LocUpSta_t;

/*
** Possible values for Status (LocUp)
*/
#define RR_REG_OK          0  // registration was successfull
#define RR_REG_PENDING     1  // registration result not known yet
#define RR_ILLEGAL_USR     2  // IMSI unknown in xLR, illegal MS, 
                              // IMEI not accepted, illegal ME
#define RR_4_FAIL          3  // MM has made 4 attempts without success
#define RR_FBD_PLMN        4  // PLMN not allowed
#define RR_NO_ROAM_LA      5  // Roaming not allowed in this Location Area
#define RR_FBD_LA          6  // Location Area not allowed
#define RR_NO_CELL_IN_LA   7  // No suitable Cells in Location Area
#define RR_SRVC_NOT_ALLOWD 8  // Service not allowed on this PLMN

#define RR_USER_DETACH       9  // Detach following user request
// GPRS-specific values : TO BE COMPLETED

#define RR_P_REG_OK          10 // registration was successfull
#define RR_P_REG_PENDING            11  // registration result not known yet
#define RR_P_ILLEGAL_USR            12  // illegal MS, illegal ME
                                        // GPRS services and non GPRS services not allowed
#define RR_P_SRVC_NOT_ALLOWD        13  // GPRS Service not allowed
#define RR_P_FBD_PLMN               14  // PLMN not allowed
#define RR_P_NO_ROAM_LA             15  // Roaming not allowed in this Location Area
#define RR_P_FBD_LA                 16  // Location Area not allowed
#define RR_P_NO_CELL_IN_LA          17  // No suitable Cells in Location Area
#define RR_P_SRVC_NOT_ALLOWD_INPLMN 18  // GPRS Service not allowed in this PLMN

#define RR_P_NWK_DET                19  // Detach from the Nwk (re-attach shall be
                                // requested by MMI)

//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
#define RR_LU_RAU_STATE_UNKNOW      0xFF
//]]__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX


/*
================================================================================
  Structure  : rr_KcInd_t
--------------------------------
  Direction  : MM -> RR 
  Scope      : give RR the new GSM ciphering key
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8 Kc[8] ;
} rr_KcInd_t ;


/*
================================================================================
  Structure  : rr_CmkInd_t
--------------------------------
  Direction  : RR -> MM 
  Scope      : RR gives MM the new Ms Classmark 2
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8 MsCm2[3] ;
} rr_CmkInd_t ;


/*
================================================================================
  Structure  : rr_ChanModeInd_t
--------------------------------
  Direction  : RR -> CC, RR -> API
  Scope      : RR gives CC or API the new channel mode
  Comment    : when sent to CC, possible values for ChanMode are the same as 
               in itf_l1.h, the possible values for ChanMode
               when sent to API, they are as defined in itf_api.h api_StackCfg_t
================================================================================
*/
typedef api_CcChanModeInd_t rr_ChanModeInd_t ;


/*
================================================================================
  Structure  : rr_NwSelModeReq_t
--------------------------------
  Direction  : API -> RR 
  Scope      : give RR the new Network selection mode ( MANU/ AUTO )
  Comment    : 
================================================================================
*/
#define RR_NW_AUTO API_NW_AUTOMATIC_MODE
#define RR_NW_MANU API_NW_MANUAL_MODE

typedef api_NwSelModeReq_t rr_NwSelModeReq_t ;


/*
================================================================================
  Structure  : rr_NwSelReq_t
--------------------------------
  Direction  : API -> RR 
  Scope      : give RR the Network ro select 
  Comment    : 
================================================================================
*/
typedef api_NwSelReq_t rr_NwSelReq_t ;

/*
================================================================================
  Structure  : rr_NwCsRegStaInd_t
--------------------------------
  Direction  : RR --> API 
  Scope      : give API teh Status od current regiostration for circuit switch
  Comment    : 
================================================================================
*/
#define RR_NW_NO_SVCE      API_NW_NO_SVCE     // No Service
#define RR_NW_FULL_SVCE    API_NW_FULL_SVCE   // Full Service
#define RR_NW_LTD_SVCE     API_NW_LTD_SVCE    // Lmtd Service(Emerg Only);CS only
#define RR_NW_NO_OUTGO     API_NW_NO_OUTGO    // No MO call(LA not Allowed);CS only
#define RR_NW_REG_PENDING  API_NW_REG_PENDING // Reslt of reg not known yet;PS only

typedef api_NwCsRegStatusInd_t rr_NwCsRegStaInd_t ;

/*
================================================================================
  Structure  : rr_NwPsRegStaInd_t
--------------------------------
  Direction  : RR --> API 
  Scope      : give API teh Status od current regiostration for Packet (GPRS) 
  Comment    : 
================================================================================
*/
typedef api_NwPsRegStatusInd_t rr_NwPsRegStaInd_t ;


/*
================================================================================
  Structure  : rr_NwListInd_t
--------------------------------
  Direction  : RR --> API 
  Scope      : give API the list of available Plmns                           
  Comment    : 
================================================================================
*/

/*
** Restriction definitions
*/
#define RR_PLMN_OK        API_PLMN_OK 
#define RR_PLMN_FBD       API_PLMN_FBD

#define RR_MAX_AVAIL_PLMN API_MAX_AVAIL_PLMN 

typedef api_NwListInd_t rr_NwListInd_t;

typedef api_NwListAbortInd_t rr_NwListAbortInd_t;

typedef api_NwStoreListInd_t rr_NwStoreListInd_t;

typedef api_SimOpenInd_t     rr_SimOpenInd_t;

typedef api_PowerScanReq_t stk_PowerScanReq_t ;  
typedef api_PowerScanRsp_t stk_PowerScanRsp_t ;  
/*
================================================================================
  Structure  : rr_NmoCfgReq_t 
--------------------------------------------------------------------------------
  Direction  : API  -> RR
  Scope      : Request RR to start / stop taking NMO into account
  Comment    : if Stop, RR will consider that Network Mode of Operation = mode 1
================================================================================
*/
typedef struct 
{
 bool  On ; // TRUE if RR should take NMO into account
} rr_NmoCfgReq_t ;

/*
================================================================================
  Message    : RR_ENDCONGEST_IND
--------------------------------
  Direction  : MM -> RR
  Scope      : Indicate that the congestion timer has expired
  Comment    : No body
================================================================================
*/

/*
================================================================================
  Structure  : rr_QualReportReq_t
--------------------------------------------------------------------------------
  Direction  : API -> RR 
  Scope      : Requests a radio signal quality report
  Comment    : 
================================================================================
*/
typedef api_QualReportReq_t rr_QualReportReq_t ;
typedef api_QualReportCnf_t  rr_QualReportCnf_t ;

/*
================================================================================
  Structure  : rr_CurCellInfos 
--------------------------------------------------------------------------------
  Direction  : API -> RR 
  Scope      : Requests a radio signal quality report
  Comment    : 
================================================================================
*/
typedef  api_CurCellInfoReq_t rr_CurCellInfoReq_t  ;
typedef  api_CurCellInfoInd_t rr_CurCellInfoInd_t  ;

/*
================================================================================
  Structure  : rr_AdjCellInfos 
--------------------------------------------------------------------------------
  Direction  : API -> RR 
  Scope      : Requests a radio signal quality report
  Comment    : 
================================================================================
*/
typedef  api_AdjCellInfoReq_t rr_AdjCellInfoReq_t  ;
typedef  api_AdjCellInfoInd_t rr_AdjCellInfoInd_t  ;


/*
================================================================================
  Structure  : rr_SyncInfos 
--------------------------------------------------------------------------------
  Direction  : API -> RR 
  Scope      : Requests power list and sync info report
  Comment    : 
================================================================================
*/
typedef api_SyncInfoReq_t  rr_SyncInfoReq_t;
typedef api_PowerListInd_t  rr_PowerListInd_t;
typedef api_SyncInfoInd_t   rr_SyncInfoInd_t;
typedef api_DetectMBSReq_t rr_DetectMBSReq_t ;//api_DetectMaliciousBSReq_t

/*
================================================================================
  Structure  : rr_CbchReq_t
--------------------------------------------------------------------------------
  Direction  : CB  -> RR
  Scope      : Request to start/stop CBCH listening
================================================================================
*/
typedef struct 
{
 bool       Status;
} rr_CbchReq_t ;

/*
** Possible values for Status
*/
#define RR_CBCH_ON    TRUE
#define RR_CBCH_OFF   FALSE



/*
** PACKET SWITCH RR/MAC INTERFACE <-> RLC
*/
#define MAC_DLDATA_IND      ( RR_GPRS_BASE + 0x00 )
#define MAC_ULACK_IND       ( RR_GPRS_BASE + 0x01 )
#define MAC_ULDATA_REQ      ( RR_GPRS_BASE + 0x02 )
#define MAC_TLLI_IND        ( RR_GPRS_BASE + 0x03 )
#define MAC_CONTRES_IND     ( RR_GPRS_BASE + 0x04 )
#define MAC_DLACK_REQ       ( RR_GPRS_BASE + 0x05 )
#define MAC_ULSUSP_REQ      ( RR_GPRS_BASE + 0x06 )
#define MAC_ULRES_REQ       ( RR_GPRS_BASE + 0x07 )
#define RRP_CONNECT_REQ     ( RR_GPRS_BASE + 0x08 )
#define RRP_DLCONNECT_IND   ( RR_GPRS_BASE + 0x09 )
#define RRP_ULCONNECT_CNF   ( RR_GPRS_BASE + 0x0A )
#define RRP_DLREL_IND       ( RR_GPRS_BASE + 0x0B )
#define RRP_ULRELSTART_REQ  ( RR_GPRS_BASE + 0x0C )
#define RRP_ULREL_IND       RR_RELEASE_IND
#define RRP_ULREL_REQ       ( RR_GPRS_BASE + 0x0E )
#define RRP_CELLOPT_IND     ( RR_GPRS_BASE + 0x0F )
#define MAC_PURGE_DATAQ_REQ ( RR_GPRS_BASE + 0x10 )

#ifndef STK_SINGLE_SIM
#ifdef __GPRS__
#define RR_CONFLICONNECT_REQ     ( RR_GPRS_BASE + 0x11 )
#endif
#endif
/*#Added by PENGQIANG 2007.3.19 for testmode begin.*/
#ifdef __EGPRS__
#define RRP_EGPRS_LOOPBACK_CMD ( RR_GPRS_BASE + 0x12 )
#endif
/*#Added by PENGQIANG 2007.3.19 for testmode end.*/
/*
** PACKET SWITCH RR/MAC INTERFACE <-> MMP
*/
#define RR_PPAGING_IND      ( RR_GPRS_BASE + 0x20 )
#define MAC_NEWTLLI_IND     ( RR_GPRS_BASE + 0x21 )
#define RR_RAUPD_STA        ( RR_GPRS_BASE + 0x22 )
#define RR_MMPPROC_IND      ( RR_GPRS_BASE + 0x23 )
#define RR_PRELEASE_IND     ( RR_GPRS_BASE + 0x24 )
#define RR_MMREADY_ST_IND   ( RR_GPRS_BASE + 0x25 )            
#define RR_GPRS_REQ         ( RR_GPRS_BASE + 0x26 )
#define RR_SUSPEND_REQ      ( RR_GPRS_BASE + 0x27 )
#ifdef __EGPRS__
#define RR_ACCTECHTYPE_IND   ( RR_GPRS_BASE + 0x28 )
#endif
#define RR_TESTMODE_REQ   (RR_GPRS_BASE + 0x29)

/*
** PACKET SWITCH RR/MAC INTERFACE <-> LLC
*/
#define MAC_NEW_TLLI_CNF     ( RR_GPRS_BASE + 0x30 )


/*
** Structures for the PACKET SWITCH RR/MAC INTERFACE <-> RLC
*/
/*
================================================================================
  Structure  : mac_DlDataInd_t
--------------------------------
  Direction  : MAC -> RLC
  Scope      : MAC forward a packet data to RLC
  Comment    : Interlayer message
================================================================================
*/
typedef struct 
{
 u16 ST;       // Frame nb modulo 42432
 u8  TN;
 u8  CS;
 u8  BitField;
#ifdef __EGPRS__
 u8  len_of_Ack;
 u8  Es_p;
 u8  BsnValid;
 bool ChanReqDes;// indicate if Channel Request Description is present
 bool Meas_Present;
#endif
} mac_DlDataInd_t;


#ifdef __EGPRS__

/* starting modify by wangqunyang for stack testing 9.6 
**/
#if 0
#define L1_BSN1_VALID  (1)
#define L1_BSN2_VALID  (1<<1)
enum { L1_IRBUF_OVERFL, L1_IRBUF_CLEAR }; 
#endif
/* endiing modify by wangqunyang for stack testing 9.6 */

/* Egprs RLC data begin position */
#define MAC_EGPRS_RLCDATA_POS 6

/*
** Block size depending on the MCS
*/
#define MAC_DLDATASIZE_MCS1 22
#define MAC_DLDATASIZE_MCS2 28
#define MAC_DLDATASIZE_MCS3 37
#define MAC_DLDATASIZE_MCS4 44
#define MAC_DLDATASIZE_MCS5 56
#define MAC_DLDATASIZE_MCS6 74
#define MAC_DLDATASIZE_MCS7 112
#define MAC_DLDATASIZE_MCS8 136
#define MAC_DLDATASIZE_MCS9 148

#ifdef __MAC_CTRL_C__
//#define DefExtern 

u8 macDLDataSizeMCS[13] = 
{ 0,0,0,0,
   MAC_DLDATASIZE_MCS1, MAC_DLDATASIZE_MCS2,MAC_DLDATASIZE_MCS3,
   MAC_DLDATASIZE_MCS4, MAC_DLDATASIZE_MCS5,MAC_DLDATASIZE_MCS6,
   MAC_DLDATASIZE_MCS7, MAC_DLDATASIZE_MCS8,MAC_DLDATASIZE_MCS9
};
#else
//#define DefExtern extern
extern u8 macDLDataSizeMCS[13];
#endif



#endif

/*
** bits significance for BitField
*/
#define MAC_FB_IND        ( 1 << 0 )
#define MAC_DLACKNACK_REQ ( 1 << 1 )

/*
================================================================================
  Structure  : mac_AckNackInd_t (used for MAC_ULACK_IND and MAC_DLACK_REQ)
--------------------------------
  Direction  : MAC -> RLC and RLC -> MAC
  Scope      : MAC forwards a packet Uplink Ack/Nack to RLC
            or RLC asks MAC to emit a packet Downlink Ack/Nack
  Comment    : 
================================================================================
*/
#define MAC_ACKNACKDESC_SIZE 9
typedef struct 
{
 u16 ST;       // RLC <-> MAC; Frame nb modulo 42432
 u8  TN;       // RLC <-> MAC;
 u8  CS;       // MAC -> RLC only; 
 u8  Desc[MAC_ACKNACKDESC_SIZE]; // 3GPP 04.60 AckNack Description IE
 #ifdef __EGPRS__
 bool Fills_rest;
 u8  Resegment;
 u8	Pre_emptive;
 u8  FAI;
 u8  BOW;
 u8  EOW;
 u16  SSN;
 u8  Len_comp;
 u8  Start_colour;
 u8  CRBB[20];
 u8  Len_uncom;
 u8  URBB[20];
 bool ChanReqDes;// indicate if Channel Request Description is present
 bool Meas_Present; 
 #endif
} mac_AckNackInd_t;

/*
================================================================================
  Structure  : rrp_AccTechTypeList_t
--------------------------------
  Direction  : RRP -> MMP
  Scope      : RRP report Access Technology Type network request to MMP.
  Comment    : InterLayer message
================================================================================
*/
#ifdef __EGPRS__
typedef struct 
{
 u8   AccTechType[14];
 u8	Sum;
} rrp_AccTechTypeList_t;
 #endif

/*
================================================================================
  Structure  : mac_UlDataReq_t
--------------------------------
  Direction  : RLC -> MAC
  Scope      : RLC ask MAC to send a data block
  Comment    : InterLayer message
================================================================================
*/
typedef struct 
{
 u8   CS;
 u8   CV;          // Countdown value
 u8   Indicators;
 bool TxIndReq  ;  // Send Tx ind.

 u8    TN;            //timeslot number

 u32  Id        ;  // Id of the block 
} mac_UlDataReq_t;

/*
** Coding Scheme values
*/
#define MAC_CS1 0
#define MAC_CS2 1
#define MAC_CS3 2
#define MAC_CS4 3
#ifdef __EGPRS__
#define MAC_MCS1 4
#define MAC_MCS2 5
#define MAC_MCS3 6
#define MAC_MCS4 7
#define MAC_MCS5 8
#define MAC_MCS6 9
#define MAC_MCS7 10
#define MAC_MCS8 11
#define MAC_MCS9 12
#define MAC_MCS57 13
#define MAC_MCS69 14
#endif
/*
** CountdownValue specific values
*/
#define MAC_CV_MAX      0x0F

/*
** Bit significance for Indicators
*/
// Do not change the value of the shift...
#define MAC_TI_ON ( 1 << 0 )
#define MAC_SI_ON ( 1 << 1 )
#define MAC_PI_ON ( 1 << 6 )

/*
================================================================================
  Structure  : mac_TlliInd_t
--------------------------------
  Direction  : MAC -> RLC
  Scope      : MAC indicates a new TLLI to RLC
  Comment    : 
--------------------------------
  Structure  : mac_NewTlliInd_t
--------------------------------
  Direction  : MMP -> MAC
  Scope      : MMP gives the new TLLI to MAC
  Comment    : 
================================================================================
*/
typedef struct 
{
 u32 NewTlli;
} mac_TlliInd_t,
  mac_NewTlliInd_t;

/*
================================================================================
  Structure  : mac_ContResInd_t 
--------------------------------
  Direction  : MAC -> RLC
  Scope      : MAC indicates contentions resolution was successful
  Comment    : no body
================================================================================
*/
/*
================================================================================
  Structure  : mac_UlSuspReq_t 
--------------------------------
  Direction  : MAC -> RLC
  Scope      : MAC asks RLC to suspend itself
  Comment    : no body
================================================================================
*/
/*
================================================================================
  Structure  : mac_UlResReq_t 
--------------------------------
  Direction  : MAC -> RLC
  Scope      : MAC asks RLC to resume its operation
  Comment    : no body
================================================================================
*/


/*
================================================================================
  Structure  : rrp_ConnectReq_t
--------------------------------
  Direction  : RLC -> RRP
  Scope      : RLC ask for the establishment of a TBF
  Comment    : 
================================================================================
*/
typedef struct 
{
 u8  ChanReqDesc[3];  // 3GPP 04.60 Packet Channel Request message
 u8  RadioPrio;
 u8  NbBlocks;        // 1 to 8 with a CS1 encoding
 u8  Cause;
 u8  RlcMode;
 u8  PFI;
 #ifdef __EGPRS__
 u8  EgLoopBack;
u8 DlTsOff;
u8 ForceUlGmsk;
 #endif
} rrp_ConnectReq_t;

#define RR_NO_PFI 0xFF

/*
** Specific values for NbBlocks
*/
#define RRP_NBBLOCKS_INVALID 0xFF // opend ended TBF, or more than 8 blocks 

/*
** Possible values for Cause
*/
#define RRP_USER_DATA    0      /* Values comes from the ACCESS_TYPE */
#define RRP_PAGE_RSP     1      /* of the Packet Resource Request    */
#define RRP_CELL_UPDATE  2
#define RRP_MM_PROCEDURE 3

/*
** Possible values for RlcMode
*/
#define RRP_RLCACK_MODE   0
#define RRP_RLCUNACK_MODE 1

#ifdef __EGPRS__
/*
** Possible values for EgLoopBack
*/
#define RRP_EGPRS_LOOPBACK_MODE_OFF   0
#define RRP_EGPRS_LOOPBACK_MODE_ON     1
#endif
/*
================================================================================
  Structure  : rrp_DlConnectInd_t
--------------------------------
  Direction  : RRP -> RLD
  Scope      : RRP indicates to RLC the establishment of a new downlink TBF
  Comment    : 
================================================================================
*/
typedef struct 
{
 u8  RlcMode;
 u32 TLLI;
#ifdef __EGPRS__
 u8 TBF_Mode;
 u16 EG_WS;
#endif
} rrp_DlConnectInd_t;

/*#Added by PENGQIANG 2007.3.19 for testmode begin.*/
#ifdef __EGPRS__
/*
================================================================================
  Structure  :  rrp_Egprs_LoopBackCmd_t
--------------------------------
  Direction  : RRP -> RLD
  Scope      : RRP indicates to RLC the loopback mode is on or off
  Comment    : 
================================================================================
*/
typedef struct 
{
 u8  Egprs_LoopBack_Mode;
} rrp_Egprs_LoopBackCmd_t;
#endif
/*#Added by PENGQIANG 2007.3.19 for testmode end.*/

/*
================================================================================
  Structure  : rrp_UlConnectCnf_t
--------------------------------
  Direction  : RRP -> RLC
  Scope      : RRP indicates to RLC the establishment of a new uplink TBF
  Comment    : 
================================================================================
*/
typedef struct 
{
 u8   PanDec;
#define RR_NO_PAN_DEC      0xFF 

 u8   PanInc;
#define RR_NO_PAN_INC      0xFF 

 u8   PanMax;
#define RR_NO_PAN_MAX      0xFF 

 u8   CS;
 u8   TlliBlockCS;           
#define RRP_TLLIBLK_CS1    0
#define RRP_TLLIBLK_REG    1
 u8   BsCvMax;          
 u8   CloseEnded;
#define RRP_ULCON_UNCHANGE 0
#define RRP_ULCON_CLOSEEND 1
#define RRP_ULCON_OPENEND  2
 u8   NTS;                   // Nb of TS assigned to the uplink TBF
#define RRP_USFGRAN_OFF    0 // 1 USF / block
#define RRP_USFGRAN_ONN    1 // 1 USF / 4 block
#define RRP_USFGRAN_SAB    2 // Same As Before
 u8   UsfGranularity;
 bool ContResOn;
 bool PfcNwkSupp;         
 u16  BlockGranted;       //significant only if CloseEnded = RRP_ULCON_CLOSEEND
#ifdef __EGPRS__
 u8  Resegment;
 u16  EG_WS;
#endif

 bool ExtendedTBF; //indicate for extended uplink mode
 
} rrp_UlConnectCnf_t;


/*
================================================================================
  Structure  : rrp_DlRelInd_t
--------------------------------
  Direction  : RRP -> RLD
  Scope      : RRP indicates that the DL TBF has been released
  Comment    : no body
================================================================================
*/

/*
================================================================================
  Structure  : rrp_UlStartRelReq_t
--------------------------------
  Direction  : RRP -> RLU
  Scope      : RRP asks RLU to start the Countdown procedure
  Comment    : no body
================================================================================
*/

/*
================================================================================
  Structure  : rrp_UlRelInd_t
--------------------------------
  Direction  : RRP -> RLU
  Scope      : RRP indicates that the UL TBF has been released
  Comment    : 
================================================================================
*/
typedef rr_ReleaseInd_t rrp_UlRelInd_t ;

/*
================================================================================
  Structure  : rrp_UlReleaseReq_t
--------------------------------
  Direction  : RLU -> RRP
  Scope      : RLU asks RRP to release the uplink TBF
  Comment    : 
================================================================================
*/
typedef struct 
{
 u8 Type;
#define RRP_REL_NOR         0
#define RRP_REL_ABWAR       1   /* ALERT RRP if these values are changed */
#define RRP_REL_ABCELLRESEL 2
} rrp_UlReleaseReq_t;


/*
================================================================================
  Structure  : rrp_CellOptInd_t
--------------------------------
  Direction  : RRP -> RLU
  Scope      : RRP indicates to RLU the change of GPRS Cell options
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8 TestMode;
}rrp_TestModeReq_t;
typedef struct 
{
 bool IsNewCell;                /* TRUE if the event is because of a new cell */
 u8   PanDec;       
 u8   PanInc;
 u8   PanMax;
} rrp_CellOptInd_t;

/*
** Structures for the PACKET SWITCH RR/MAC INTERFACE <-> MMP
*/
/*
================================================================================
  Structure  : rr_PPagingInd_t
--------------------------------
  Direction  : RRP -> MMP
  Scope      : RRP forwards a PPaging to MMP
  Comment    : 
================================================================================
*/


/*
================================================================================
  Structure  : rr_RaUpSta_t
--------------------------------
  Direction  : MMP -> RRP
  Scope      : MMP gives the results of the RA update to RR
  Comment    : 
================================================================================
*/
#define rr_RaUpSta_t rr_LocUpSta_t

/*
** Possible values for Status : LocUpSatus possible values
*/



/*
================================================================================
  Structure  : rr_MmpProcInd_t
--------------------------------
  Direction  : MMP -> RR
  Scope      : MMP indicates the start / stop of an MMP procedure 
               ( GPRS ATTACH / RA UPDATE, cf 04.60, DRX )
================================================================================
*/
typedef struct
{
 bool Start ; 
} rr_MmpProcInd_t ;   

/*
================================================================================
  Structure  : rr_MmpReadyStInd
--------------------------------
  Direction  : MMP -> RR
  Scope      : MMP indicates the Entering /Leaving of MMP Ready State 
              (TRUE means MMP is in Ready State )
================================================================================
*/
typedef struct
{
 bool ReadySt ;
 bool IsRdyTimerEqZero;

} rr_MmpReadyStInd_t; 

/*
================================================================================
  Structure  : rr_GprsReq_t
--------------------------------
  Direction  : MMP -> RR
  Scope      : MMP indicates if the GPRS service is wanted or not
================================================================================
*/
typedef struct
{
 bool Wanted ; // TRUE if GPRS service wanted, FALSE otherwise

} rr_GprsReq_t; 


/*
** Structures for the PACKET SWITCH RR/MAC INTERFACE <-> LLC
*/
/*
================================================================================
  Structure  : mac_NewTlliCnf_t
--------------------------------------------------------------------------------
  Direction  : LLC -> MAC
  Message    : MAC_NEW_TLLI_CNF
  Scope      : LLC informs MAC that a frame with the new TLLI has been received
  Comment    : 
================================================================================
*/
typedef struct 
{
  u32   Tlli;    // TLLI

} mac_NewTlliCnf_t;


#endif  // __ITF_RR_H__

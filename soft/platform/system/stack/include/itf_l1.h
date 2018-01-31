//------------------------------------------------------------------------------
//  $Log: itf_l1.h,v $
//  Revision 1.4  2006/05/09 17:46:48  fch
//  Update with Bob REF v2.22
//
//  Revision 1.3  2006/03/03 13:45:55  fch
//  Update with Bob
//
//  Revision 1.2  2005/12/29 17:38:44  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : ITF_L1.H
--------------------------------------------------------------------------------

  Scope      : Interface Mesages provided by L1

  History    :
--------------------------------------------------------------------------------
  Jun 12 03  |  PFA   | Creation
  Oct 28 03  |  MCE   | Addition of l1_ChanModeModifyReq_t and l1_DataReq_t
  Oct 29 03  |  ADA   | - Mbx in Data request message.
             |        | - l1_TxInd_t for data transmission acknowlegment.
             |        | - l1_DataReq_t field TxIndReq
  Mar 05 04  |  ADA   | - Field IdleType in l1_RachOnReq_t.
             |        | - PCchReq, PBcchReq
             |        | - Packet Measures.
  Apr 01 04  |  ADA   | - Type l1_InitReq_t + Defines for band selection.
             |        | - PBcch identification fields in DataInd
             |        | - PBcchReq_t upgrade.
  Apr 19 04  |  MCE   | Addition of the SetTestLoop primitive
  May 17 04  |  OTH   | Move the L1_PB_MACDYN flag to TBFBitmap of
             |        | l1_TBFCfgReq_t structure => L1_TBF_MACDYN.
  May 31 04  |  ADA   | TxInd_t -> add EndTxDueDate.
  Jun 03 04  |  ADA   | Update for Packet measures.
  Aug 06 04  |  ADA   | AMR management.
  Aug 17 04  |  ADA   | CBCH management.
  Aug 18 04  |  MCE   | Addition of L1_FREQ_REDEF_CNF
  Aug 19 04  |  ADA   | Fixed allocation.
  Sep 13 04  |  OTH   | Addition of the TFI field in the l1_CmnAssgt_t
  Sep 06 04  |  JFN   | Split L1_PACKET_MONITOR_REQ into L1_NC_MONITOR_REQ and
             |        | L1_EXTP_MONITOR_REQ
  Oct 22 04  |  JFN   | - Remove L1_EXTP_MONITOR_REQ and IND, replaced by
             |        |   L1_EXT_INT_REQ and IND
             |        | - Add enhanced measurement in MonitorInd
  Nov 02 04  |  ADA   | l1_DataInd_t:
             |        | - BlkIndex  Index of the block in the allocation period in fixed MAC mode.
             |        | - Time stamp is now the Fn instead of Fn % 42432
             |        | l1_MacFixedUpdtReq_t: BlkIndex instead of PeriodCnt.
             |        | - PacketDataPurge Req + Ind.
 Dec 30  04  |  ADA   | - Pb added in PBcchReq and PCchReq.
 Jan 04  04  |  ADA   | - L1_SYNCH_EXTB_REQUIRED to require the reception of
             |        | extended Bcch with a SynchReq.
 Feb 23  05  |  MCE   | - redefined BandMap defines to be compatible with API
 Apr 28  05  |  MCE   | - changed Id from u8[4] to u32 in l1_PurgeInd_t       
 Oct 25  05  |  MCE   | - Added L1_TX_DISCARD_REQ and l1_TxDiscardReq_t       
 Nov 17  05  |  MCE   | - Added L1_CLOSE_I_LOOP    
 Jan 16  06  |  JFN   | - Added L1_TX_DISCARD_IND and l1_TxDiscardInd_t 
 Mar 02  06  |  JFN   | - Add l1_PacketMeasurementReportPtr for PMR in transfer 
             |        | mode
 Mar 10  06  |  MCE   | - changed Pad[3] to Pad[2] in l1_PDataPurgeInd_t
================================================================================
*/

#ifndef __ITF_L1_H__
#define __ITF_L1_H__

#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_api.h"
#include "pal_gsm.h"

#ifdef __ITF_L1_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

/*
** Shared data.
*/

DefExtern u8     l1_Sapi0FacchQueue ;
DefExtern u8     l1_Sapi3Queue      ;
DefExtern void * l1_Sapi0SacchPtr   ;
DefExtern u8     l1_PCtrlQueue      ;
DefExtern u8     l1_PDataQueue      ;

DefExtern void *l1_PacketMeasurementReportPtr;


#undef DefExtern

#define L1_RRD_FIRST        ( HVY_L1 + 0x80 )
#define L1_TST_FIRST        ( HVY_L1 + 0x100 )

/*
** Primitive ID definitions
*/
#define L1_INIT_REQ            ( HVY_L1 + 1 )
#define L1_POWER_REQ           ( HVY_L1 + 2 )
#define L1_POWER_RSP           ( HVY_L1 + 3 )
#define L1_SYNCH_REQ           ( HVY_L1 + 4 )
#define L1_STOP_SYNCH_REQ      ( HVY_L1 + 5 )
#define L1_SYNCH_FOUND         ( HVY_L1 + 6 )
#define L1_SYNCH_NOT_FOUND     ( HVY_L1 + 7 )
#define L1_SYNCH_UPDATE_REQ    ( HVY_L1 + 8 )
#define L1_DATA_IND            ( HVY_L1 + 9 )
#define L1_CCH_REQ             ( HVY_L1 + 0x0a)
#define L1_STOP_CH_REQ         ( HVY_L1 + 0x0b)
#define L1_PCCH_REQ            ( HVY_L1 + 0x0c)
#define L1_BCCH_REQ            ( HVY_L1 + 0x0d)
#define L1_BCCH_MODE_REQ  ( HVY_L1 + 0x17)  //change bcch job mode.
#define L1_PBCCH_REQ           ( HVY_L1 + 0x0e)
#define L1_CBCH_REQ            ( HVY_L1 + 0x0f)
#define L1_CBCH_SKIP_REQ       ( HVY_L1 + 0x10)
#define L1_MONITOR_REQ         ( HVY_L1 + 0x11)
#define L1_EXTD_MONITOR_REQ    ( HVY_L1 + 0x12)
#define L1_MONITOR_IND         ( HVY_L1 + 0x13)
#define L1_DATA_REQ            ( HVY_L1 + 0x14)
#define L1_ERROR_IND           ( HVY_L1 + 0x15)
#define L1_HIGBAND_CHANGE_REQ ( HVY_L1 + 0x18) //Bandindicator of SI1 has changed
#define L1_ADJ_CELL_REPORT_REQ ( HVY_L1 + 0x19) // for adj cell report, L1 doesn't suspend FINT IREQ

#define L1_STATE_IND           ( HVY_L1 + 0x16)   // For debug purpose.

#define L1_RACHON_REQ           ( L1_RRD_FIRST + 0 )
#define L1_RACHOFF_REQ          ( L1_RRD_FIRST + 1 )
#define L1_RACHUPDT_REQ         ( L1_RRD_FIRST + 2 )
#define L1_RACH_IND             ( L1_RRD_FIRST + 3 )
#define L1_CONNECT_REQ          ( L1_RRD_FIRST + 4 )
#define L1_CONNECT_CNF          ( L1_RRD_FIRST + 5 )
#define L1_CELLOPTUPDT_REQ      ( L1_RRD_FIRST + 6 )
#define L1_FREQ_REDEF_REQ       ( L1_RRD_FIRST + 7 )
#define L1_FREQ_REDEF_CNF       ( L1_RRD_FIRST + 9 )
#define L1_CIPHER_REQ           ( L1_RRD_FIRST + 10 )
#define L1_CHAN_MODE_MODIFY_REQ ( L1_RRD_FIRST + 11 )
#define L1_RELEASE_REQ          ( L1_RRD_FIRST + 12 )
#define L1_TX_IND               ( L1_RRD_FIRST + 13 )
#define L1_PHY_INFO_REQ         ( L1_RRD_FIRST + 14 )

#define L1_TBF_CFG_REQ          ( L1_RRD_FIRST + 17 )
#define L1_TBF_CFG_CNF          ( L1_RRD_FIRST + 18 )
#define L1_PWRCTRL_TA_REQ       ( L1_RRD_FIRST + 19 )
#define L1_PDCH_REL_REQ         ( L1_RRD_FIRST + 20 )
#define L1_TBF_REL_REQ          ( L1_RRD_FIRST + 21 )
#define L1_PS_DATA_REQ          ( L1_RRD_FIRST + 22 )
#define L1_PS_DATA_IND          ( L1_RRD_FIRST + 23 )
#define L1_USF_IND              ( L1_RRD_FIRST + 24 )
#define L1_EXT_INT_REQ          ( L1_RRD_FIRST + 25 )
#define L1_EXT_INT_IND          ( L1_RRD_FIRST + 26 )
#define L1_NC_MONITOR_REQ       ( L1_RRD_FIRST + 27 )
#define L1_NC_MONITOR_IND       ( L1_RRD_FIRST + 28 )
#define L1_PMEAS_CTRL_REQ       ( L1_RRD_FIRST + 29 )
#define L1_CHN_QUALITY_REQ      ( L1_RRD_FIRST + 30 )
#define L1_CHN_QUALITY_IND      ( L1_RRD_FIRST + 31 )
#define L1_MAC_FIXED_UPDT_REQ   ( L1_RRD_FIRST + 32 )
#define L1_END_FIXED_PERIOD_IND ( L1_RRD_FIRST + 33 )
#define L1_PDATA_PURGE_REQ      ( L1_RRD_FIRST + 34 )
#define L1_PDATA_PURGE_IND      ( L1_RRD_FIRST + 35 )
#define L1_TX_DISCARD_REQ       ( L1_RRD_FIRST + 36 )
#define L1_TX_DISCARD_IND       ( L1_RRD_FIRST + 37 )
#ifdef __EGPRS__
#define L1_IRBUF_OVERFL_IND    (L1_RRD_FIRST + 38)
#define L1_EGPRS_LOOPBACK_SWI_REQ (L1_RRD_FIRST + 39)
#endif

/*
** Test Mode primitives
*/
#define L1_SET_TEST_LOOP_REQ    ( L1_TST_FIRST + 0 )
#define L1_GPRS_TEST_MODE_REQ (L1_TST_FIRST + 1)
enum {L1_CS1, L1_CS2, L1_CS3, L1_CS4
#ifdef __EGPRS__
, L1_MCS1, L1_MCS2, L1_MCS3, L1_MCS4, L1_MCS5, L1_MCS6, L1_MCS7, L1_MCS8, L1_MCS9};
#else
};
#endif
/*
** Couple ( Arfcn, Rssi ) definition
*/

#define L1_LOW_RLA   PAL_LOW_RLA  // After PAL_LOW_RLA consider it as noise, used to be 103, changed by alex 2007-02-01
#define L1_SIZ_RSSI  672  // Max Number of Arfcn = PGSM + EGSM + DCS + 850

typedef struct
{
   u16         Arfcn ;   // [0..1024]
   u16         Rssi  ;   // [0..120] dBm after L1_LOW_RLA  it's noise only
   u16         CellId [ 2 ];
   u8          Bsic;
   u8          Lai [5];

} ArfRssi_t ;


/*
** Freq List definition
*/
#define L1_SIZ_FREQL  64  // Max 64 Frequencies (Hopping, Monitor...)
typedef struct
{
   u16   FreqNb               ;
   u16   Pad;
   u16   FreqL[L1_SIZ_FREQL]  ;

} l1_FreqList_t;

/*
** Primitive Content definitions
*/

/*
================================================================================
  Primitive  : InitReq
--------------------------------------------------------------------------------

  Scope      : RR ask to L1 to reset itslef and to get ready to work
               in the selected bands.

================================================================================
*/
#define L1_450_BAND  API_GSM_450
#define L1_480_BAND  API_GSM_480
#define L1_850_BAND  API_GSM_850
#define L1_GSM_BAND  API_GSM_900P
#define L1_EGSM_BAND API_GSM_900E
#define L1_RGSM_BAND API_GSM_900R
#define L1_DCS_BAND  API_DCS_1800
#define L1_PCS_BAND  API_PCS_1900
typedef struct
{
 u8 BandMap;
 u8 TotalInit; // 1 indicates that xcv reset is requested, otherwise, no xcv reset shall be done. added by zhanghy.20110307 
} l1_InitReq_t;

#define L1_XCV_RESET 1
#define L1_XCV_CALIB 1<<1

//RR has dectected the change of BandIndicator in SI1.RR requires L1 to update bandmap.
typedef struct
{
 u8 NewHighBand;
 } l1_HighBandChangeReq_t;


/*
================================================================================
  Primitive  : PowerReq
--------------------------------------------------------------------------------

  Scope      : RR has prepared a List of Arfcn and requires L1 to make  k
               Rssi Measurements on this list

================================================================================
*/
typedef struct
{
   u16         Nb                ;  // Number of Elem in Table
   u16         Pad;
   ArfRssi_t   List[L1_SIZ_RSSI] ;  // List of (Arfcn, Measure)

} l1_PowerReq_t;

typedef  l1_PowerReq_t l1_Power_t;

/*
================================================================================
Primitive  : PowerRsp
--------------------------------------------------------------------------------

  Scope      : L1 returns to RR the list of Arfcn along with the RSSI level
               for each of them

================================================================================
*/
typedef struct
{
   u16         Nb                ;  // Number of Elem in Table
   u16         Pad;
   ArfRssi_t   List[L1_SIZ_RSSI] ;  // List of (Arfcn, Measure)

} l1_PowerRsp_t;

/*
================================================================================
  Primitive  : SynchReq
--------------------------------------------------------------------------------

  Scope      : RR asks L1 to synchronize to an Arfcn (Bsic Decoding)
               in case of success BcchRead tells L1 what to read after Bsic

================================================================================
*/
/*
** Values for Bcch Read
*/
#define L1_SYNCH_SERVING_CELL      1   // Continuous read of BCCH Norm.
#define L1_SYNCH_PLMN_CELL         2   // Read Sysinfo 3 or 4 just once
#define L1_SYNCH_NEAR_CELL         3   // Read Sysinfo 3 or 4 every 5 mins
#define L1_SYNCH_NEAR_CELL_NO_BCCH 4   // No Sysinfo required.

// Used for a Serving cell, a Plmn cell or a Near cell to require the reception
// of the extended Bcch.
// For the serving cell, all the extended Bcch are received along with the
// Normal Bcch.
// For a Near or a Plmn Cell, the extended Bcch is received only along with
// a Normal Bcch containing a SI 4 (The extended Bcch corresponds to a SI 7 or 8).

#define L1_SYNCH_EXTB_REQUIRED   (1 << 4)


typedef struct
{
   s16         Arfcn    ;  // Arfcn to synchronize to
   u16         Rla      ;  // Rla of Arfcn  [0..115]dBm
   u8          BcchRead ;  // What to do after synchro
   u8          Pad [3];

} l1_SynchReq_t;

/*
================================================================================
  Primitive  : SynchUpdateReq
--------------------------------------------------------------------------------

  Scope      : RR asks L1 to change L1 behaviour on a synchronized cell, 
               according to the BcchRead paramter 
               only the following values are correct:

                   L1_SYNCH_NEAR_CELL  
                   L1_SYNCH_NEAR_CELL_NO_BCCH 

================================================================================
*/
typedef struct
{
   s16         Arfcn    ;  // Arfcn to synchronize to
   u8          BcchRead ;  // What to do after synchro
   u8          Pad      ;

} l1_SynchUpdateReq_t;

/*
================================================================================
  Primitive  : SynchFound
--------------------------------------------------------------------------------

  Scope      : L1 has been able to synchromize to an Arfcn (Bsic Decoding)

================================================================================
*/
typedef struct
{
   s16         Arfcn    ;  // Synchronized Arfcn
   u8          Bsic     ;  // Decoded Bsic
   u8          Pad [3];

} l1_SynchFound_t ;

typedef  l1_SynchFound_t l1_SynchRsp_t ;

/*
================================================================================
  Primitive  : SynchNotFound
--------------------------------------------------------------------------------

  Scope      : L1 has not been able to synchromize to an Arfcn

================================================================================
*/
typedef struct
{
   s16         Arfcn     ;  // non Synchronized Arfcn

} l1_SynchNotFound_t ;

typedef struct
{
   s16         Arfcn     ;  // Synchronized Arfcn

} l1_StopSynchReq_t ;

typedef struct
{
   u8   Start; // 1 to start the adjacent report, 0 to stop

}l1_AdjCellReportReq_t;
/*
================================================================================
  Primitive  : DataInd
--------------------------------------------------------------------------------

  Scope      : L1 Reports Data received on the Ccch to RR
               Can be Bcch, PAging or Access Grant

================================================================================
*/
enum                    // definition of Channel type
{
 L1_SCH_ID,
 L1_BCCH_ID,
 L1_CCCH_ID,
 L1_PCH_ID,
 L1_PBCCH_ID,
 L1_PCCCH_ID,
 L1_PPCH_ID,
 L1_PDCH_ID,
 L1_PTCCH_ID,
 L1_SDCCH_ID,
 L1_TCHF_ID,
 L1_TCHH_ID,
 L1_FACCH_ID,
 L1_SACCH_ID,
 L1_CBCH_ID
};

/* Value for PsiType field. */
#define L1_PSI_HR 0
#define L1_PSI_LR 1

typedef struct
{
   /***************************************************
   ** ATTENTION : Please leave ARFCN in first position
   ***************************************************/
   s16         Arfcn     ;      // Serving cell
   u8          ChnId     ;      // Identifier of the Channel
   u8          Cs        ;      // Coding Scheme (GPRS use)
   u8          TN        ;      // Timeslot Number (GPRS use)
   u8          PsiType   ;      // High Rate or Low Rate
   u8          PsiIdx    ;      // Index in Psi bit map of the received block.
   u8          BlkIndex  ;      // Index of the block in the allocation period in fixed MAC mode.
   u32         Timestamp ;      // Reception date (GPRS use)  Fn instead of Fn % 42432
#ifdef __EGPRS__ 
   u8 	  BsnValid;	//0x01 BSN1 valid; 0x02 BSN2 valid; 0x03 both valid.  added by weiking. 2006-11-14
//define some Macro for BsnValid. added by zhanghy. 2006-11-14
#define L1_BSN1_VALID  1
#define L1_BSN2_VALID  (1<<1)

#endif
} l1_DataInd_t ;	


/*
================================================================================
  Primitive  : l1_DataReq_t
--------------------------------------------------------------------------------

  Scope      : L2/L3 request to L1 to send a block of data.

================================================================================
*/
typedef struct
{
   u32        Id         ;      // Multi purpose Id. Sent back in the TxInd msg.
   u8         Mbx        ;      // Mbx to be used to send a potential TxInd.
   u8         TxIndReq   ;      // Send Tx ind.
   u8         Cs;
   u8         Pad;
} l1_DataReq_t ;

/*
================================================================================
  Primitive  : l1_TxInd_t
--------------------------------------------------------------------------------

  Scope      : L1 Reports to L2 that a transmission just occur.

================================================================================
*/
typedef struct
{
 u32    Id;             // Multi purpose Id coming from the data request.
 u16    T200;           // For Lap only, T200 value.
 u16    Timestamp;      // Reception date (GPRS use)
 u8     EndTxDueDate;   // Nunber of frames to wait before Tx completion.
 u8     Pad [3];
} l1_TxInd_t;

/*
================================================================================
  Structure  : l1_FreqParam_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : this structure is used by the l1_ConnectReq_t and
               l1_FreqRedef_t structures. It gives the frequency parameters of
               the l1 connection
================================================================================
*/
// Maximum number of valid Arfcn in the Mobile Allocation
#define         L1_MA_SIZE   64

typedef struct
{
  u8            NbArfcn        ; // =1 if no hopping, > 1 if hopping
  u8            Maio           ; // significant only if NbArfcn > 1
  u8            Hsn            ; // significant only if NbArfcn > 1
  u8            Pad;
  u16           MA[L1_MA_SIZE] ; // only the first NbArfcn elts are significant
} l1_FreqParam_t ;


/*
================================================================================
  Primitive  : CchReq
--------------------------------------------------------------------------------

  Scope      : RR Aks L1 to start GSM Idle Mode on a Arfcn
               Giving Paging Config

================================================================================
*/

/* Do not change the page mode values below. */

#define L1_NORMAL_PAGING        0
#define L1_EXTENDED_PAGING      1
#define L1_PAGING_REORG         2
#define L1_SAME_AS_BEFORE       3
#define L1_FULL_CCH             4

typedef struct
{
    u16  Arfcn        ;   // Frequency to camp on
    u8   BcchSdcchComb;   // Bcch channel combined with sdcch/4 channel
    u8   BsAgBlkRes   ;   // Numbers of bloks reserved for AGCH : [0..7]
    u8   BsPaMfrms    ;   // [2..9]
    u8   Group        ;   // Paging group [0..9*BsPaMfrms - 1]
    u8   Tn           ;   // Timeslot number
    u8   Mode         ;   // Full ccch, paging reorg or normal paging.
    u8   Cause ; //added by zhanghy. 2009-12-30. ccch for rach or not. 
} l1_CchReq_t ;
#define L1_RACH      1
#define L1_NORMAL  0
/*
================================================================================
  Primitive  : CbchReq
--------------------------------------------------------------------------------

  Scope      : RR Aks L1 to start listening the Cbch channel

================================================================================
*/

typedef struct
{
  u8             BitMap         ;  // Indicates the combination among Basic and
                                   // extended to be listened.
  u8             Tn             ;
  u8             Tsc            ;
  u8             Pad            ;
  l1_FreqParam_t FreqParam      ;
} l1_CbchReq_t;

/*
================================================================================
  Primitive  : CbchSkipReq
--------------------------------------------------------------------------------

  Scope      : RR Aks L1 to start listening the Cbch channel

================================================================================
*/

typedef struct
{
 u8 BitMap ;  // Indicates the combination among Basic and extended to be listened.
 u8 NbPeriod; // Number of period to skip.
} l1_CbchSkipReq_t;


/*
================================================================================
  Primitive  : StopChReq
--------------------------------------------------------------------------------

  Scope      : RR Aks L1 to stop to listen the identified channel
               configurations among:
               - Cch configuration,
               - Bcch configuration,
               - PCch configuration,
               - PBcch configuration.

================================================================================
*/

#define L1_CCH_CFG           (1 << 0)
#define L1_BCCH_CFG          (1 << 1)
#define L1_PCCH_CFG          (1 << 2)
#define L1_PBCCH_CFG         (1 << 3)
#define L1_CBCH_BASIC_CFG    (1 << 4)
#define L1_CBCH_EXTENDED_CFG (1 << 5)

typedef struct
{
 u8 CfgBitMap;
} l1_StopChReq_t;



/*
================================================================================
  Primitive  : BchReq
--------------------------------------------------------------------------------

  Scope      : RR gives a Bcch configuration to L1.

================================================================================
*/
typedef struct
{
 u16 Arfcn;
 u8  Tn;                // Timeslot to be used for reception.
 u8  Mode;              // Acquisition or periodic.
 u8  Pad [2];
 u8  TcBitMap [2];      // When bit set, listen corresponding Bcch.
                        // Idx 0 regular, Idx 1 Extended.
 u8  TcCnt [2][8];      // Reception counter for a given TC.
} l1_BcchReq_t;

#define L1_SI_NORMAL            0     // Index for regular system information.
#define L1_SI_EXTENDED          1     // Index for extended system information.

#define L1_BCCH_ACQUISITION     0     // One shot listening.
#define L1_BCCH_PERIODIC        1     // Periodic listening (30 seconds period).
/*
================================================================================
  Primitive  : BchModeReq
--------------------------------------------------------------------------------

  Scope      : RR sets a new Bcch mode to L1.
  Author: zhanghy

================================================================================
*/
typedef struct
{
  u16 Arfcn;
  u8  Mode;              // Acquisition or periodic.
 } l1_BcchModeReq_t;

/*
================================================================================
  Primitive  : l1_RLFailureInd_t
--------------------------------------------------------------------------------

  Scope      : L1 notify RR that a radio link failure has occured on the
               identified cell.

================================================================================
*/

/*
** Cause values
*/

#define L1_RADIO_LINK_FAILURE   0
#define L1_RADIO_LINK_TIME_OUT  1
#define L1_SYNCH_LOST           2
#define L1_TIM_ADV_OOR          3

/*
================================================================================
  Structure  : l1_ErrorInd_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 indicates the connection could not be established / was lost
================================================================================
*/
typedef struct
{
 u16 Arfcn;
 u8  Cause ;
 u8  Pad [3];
} l1_ErrorInd_t ;


/*
================================================================================
  Primitive  : l1_MonitorReq_t
--------------------------------------------------------------------------------

  Scope      : RR gives to L1 a list of cells to Measure
               Used for L1_MONITOR_REQ, L1_EXTD_MONITOR_REQ

================================================================================
*/
#define L1_SIZ_MON 33
#define L1_CIRCUIT_MEAS 0
#define L1_PACKET_MEAS  1

typedef struct
{
    u8            MonNb               ;   // Number of elem in list
    bool          Tag                 ;   // Discard MonitorInd if diff Tag
    u8            Pad [2];
    u16           ArfcnL [L1_SIZ_MON] ;   // List of arfcn to monitor
} l1_MonitorReq_t;

/*
================================================================================
  Primitive  : l1_MonitorInd_t
--------------------------------------------------------------------------------

  Scope      : L1 gives measurement result.
               This structure is used in L1_MONITOR_IND message:
               - in Idle/Transfer for Cell reselection
               - in Connected mode for measurement report
               - in Connected mode for extended measurement report

================================================================================
*/
typedef struct
{
    u8         MonNb              ;   // Number of elem in list
    bool       Tag                ;   // Discard MonitorInd if diff Tag
    u8         RlaL [L1_SIZ_MON]  ;   // List of Rla
    u8         RxLevFull          ;   // RxLev  Full of Main
    u8         RxLevSub           ;   // RxLev  Sub  of Main
    u8         RlaVal             ;   // Rla in absolute dBm for RxLevVal computation in enhanced measurement
    u8         RxQualFull         ;   // RxQual Full of Main
    u8         RxQualSub          ;   // RxQual Sub  of Main
    u8         AverageSNR;         //added by zhanghy, consider this SNR when do cell reselection in IDLE State.
    u8         NbrRcvdBlocks      ;   // For enhanced measurement, see 05.08 8.4.8.2
    u8         MeanBEP            ;   // Mean Bit Error Probability [MEAN_BEP_0..MEAN_BEP_31]
    u8         CvBEP              ;   // Coefficient of variation of the Bit Error Probability [CV_BEP0..CV_BEP7].
    bool       DtxUsed            ;   // Dtx has been used or not
    bool       ExtendedMeas       ;   // Extended measurement report when TRUE.
    u8         TA                 ;   // Time Advice value of Main, 0~63, 0xFF is unvalid
} l1_MonitorInd_t ;

/*
================================================================================
  Primitive  : l1_NcMonitorReq_t
--------------------------------------------------------------------------------

  Scope      : RR requests start/stop NC periodic measurement reporting
               This structure is used in L1_NC_MONITOR_REQ message

================================================================================
*/
#define L1_NC_MON_STOP  0
#define L1_NC_MON_START 1

typedef struct
{
 u8     NcMode      ;         // L1_NC_MON_START, L1_NC_MON_STOP
 u8     NcRPIdle    ;         // NC measurement Reporting Period for Idle state. Expressed in 480 ms period [1..128].
 u8     NcRPTransfer;         // NC measurement Reporting Period for Transfer state. Expressed in 480 ms period [1..128].
 u8     Pad;
} l1_NcMonitorReq_t;


/*
================================================================================
  Primitive  : l1_NcMonitorInd_t
--------------------------------------------------------------------------------

  Scope      : L1 sends NC measurement result
               This structure is used in L1_NC_MONITOR_IND message

================================================================================
*/
typedef struct
{
    u8         MonNb              ;   // Number of elem in list
    bool       Tag                ;   // Discard MonitorInd if diff Tag
    u8         IntLevel           ;   // Interf on Serv in Rxlev ( or 0xff)
    u8         RxLev[L1_SIZ_MON]  ;   // List of Rxlevs

} l1_NcMonitorInd_t ;

/*
================================================================================
  Primitive  : l1_ExtIntReq_t
--------------------------------------------------------------------------------

  Scope      : RR gives an Arfcn were Extended measurement should be done in
               Packet Idle mode

================================================================================
*/
#define L1_NO_FREQ  0xFFFF

typedef struct
{
 u16           IntFrequency        ;   // Arfcn were Extended Interference measurement should be done
} l1_ExtIntReq_t;


/*
================================================================================
  Primitive  : l1_ExtIntInd_t
--------------------------------------------------------------------------------

  Scope      : L1 sends Extended Interference measurement

================================================================================
*/
#define L1_INT_INVALID 0xFF

typedef struct
{
 u8     IntLevel[8] ;         // Interference level on IntFrequency in RxLev value [0..63]. 0xFF if not valid
} l1_ExtIntInd_t;


/*
================================================================================
  Primitive  : l1_ChnQualityInd_t
--------------------------------------------------------------------------------

  Scope      : L1 sends Channel Quality report to RR

================================================================================
*/
typedef struct
{
 u8     RxQual     ;          // For serving cell [0..7]
 u8     SignVar    ;          // [0..63] expressed in 0.25 dB2
 u8     GChBitMap  ;          // Indicates which slot to be considered in GammaCh array
 u8     GammaCh [8];          // Interference level for the serving cell [I_LEVEL0..I_LEVEL15].
 u8     C          ;          // C value [0..63]
#ifdef __EGPRS__
 //possible value of every valid bit in MBepChTypeBitMap.  added by zhanghy. 2006-11-14
#define L1_BEP_GSMK  0
#define L1_BEP_8PSK   1

  u8 GmskMeanBep; 	// GMSK MEAN BEP
  u8 ApskMeanBep; 	//8PSK MEAN BEP
  u8 GmskCvBep;		 // GMSK CV BEP
  u8 ApskCvBep;		 //8PSK CV BEP
  u8 MBepChValidBitMap; //Mean BEP for time slot valid bitmap,1 valid 0 invalid, LSB : TN0
  u8 MBepChTypeBitMap; //Mean BEP for time slot modulation type, 1 8psk, 0 gmsk, LSB : TN0, 
  						//only valid while relative bit set in MBepChValidBitMap
  u8 MeanBepCh[8]; //Possible Mean BEP value for slot0-7, MeanBepCh[0] for TN0
#endif
} l1_ChnQualityInd_t;


/*
================================================================================
  Structure  : l1_RachOnReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR asks L1 to start transmitting RACHs
================================================================================
*/
typedef struct
{
   u16            RachInfo     ; // Establishment cause, left aligned
   u8             Type         ; // GSM RACH, GPRS 8 bits, GPRS 11 bits
   u8             IdleType     ; // CCCH IDLE or PCCCH IDLE
   u8             RandSize     ; // Nb of bits reserved for the random reference
   u8             TxPower      ; // Rach transmit power
   u8             S            ; // S parameter 4.18 3.3.1.1.2, 4.60 12.14
   u8             T            ; // Tx integer converted value see 4.18
                                 // 10.5.2.29, 4.60 12.14
   u8             NbMaxRetrans ;
   u8             PersistenceLevel; // GPRS only
   u8             RadioPrio    ;    // GPRS only
   u8             Pad          ;
} l1_RachOnReq_t ;

/*
** Possible values for Type
*/
#define L1_RACH_GSM      1
#define L1_RACH_GPRS_8   2
#define L1_RACH_GPRS_11  3
#ifdef __EGPRS__
//EGPRS type. added by zhanghy 2006-11-14
#define L1_RACH_EGPRS	4
#endif
/*
** Possible values for IdleType
*/
#define L1_CCCH_IDLE     1
#define L1_PCCCH_IDLE    2


/*
================================================================================
  Structure  : l1_RachUpdtReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR update RACH relatif broadcast parameters.
================================================================================
*/
typedef struct
{
   u8             TxPower      ; // Rach transmit power
   u8             S            ; // as described in 05.08
   u8             T            ; // converted value see 4.18
   u8             NbMaxRetrans ;
   u8             PersistenceLevel; // GPRS only
   u8 Pad [3];
} l1_RachUpdtReq_t;


/*
================================================================================
  Structure  : l1_RachOffReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR asks L1 to stop transmitting RACHs
  Comment    : This message body is dummy
================================================================================
*/
typedef u8 l1_RachOffReq_t ;


/*
================================================================================
  Structure  : l1_RachInd_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 indicates it has just transmitted a RACH
================================================================================
*/
typedef struct
{
  u16             Info ; // Random Access Information, right aligned
  u16             RFN  ; // Frame Nb (modulo 42432) at which the tx occurred
} l1_RachInd_t ;


/*
================================================================================
  Primitive  : PCchReq
--------------------------------------------------------------------------------

  Scope      : RR Aks L1 to start GSM Packet Idle Mode on a Arfcn
               Giving Packet Paging Config

================================================================================
*/

/* Do not change the page mode values below. */

#define L1_P_NORMAL_PAGING        0
#define L1_P_EXTENDED_PAGING      1
#define L1_P_PAGING_REORG         2
#define L1_P_SAME_AS_BEFORE       3
#define L1_P_ACCESS               4
#define L1_P_NON_DRX              5

typedef struct
{
 u16  Arfcn        ;       // Frequency to camp on
 l1_FreqParam_t FreqParam; // Frequency Parameter.
 u8   MultiframeType;      // 51 or 52
 u8   Tn           ;       // Timeslot number
 u8   Tsc          ;       // Training sequence
 u8   AgBlkRes   ;         // Numbers of bloks reserved for (P)AGCH : [0..12]
 u8   PBcchBlks;           // Number of PBcch in the multiframe.
 u8   BcchSdcchComb;       // Bcch channel combined with sdcch/4 channel
 u8   SplitPagingCycle;
 u8   Mode         ;       // Full ccch, paging reorg or normal paging.
 u16  Group        ;       // (((Imsi mod 1000) div (KC*N)) * N) + (Imsi Mod 1000) mod M. See 04.60 for detailed definition.
 u8   Pb;                  // Power reduction.
} l1_PCchReq_t ;


/*
================================================================================
  Primitive  : PBcchReq
--------------------------------------------------------------------------------

  Scope      : RR gives a Bcch configuration to L1.

================================================================================
*/
typedef struct
{
 u16     Arfcn;                  // Bcch Arfcn.
 l1_FreqParam_t FreqParam;       // Frequency parameters.
 u8      Tn;                     // Timeslot number.
 u8      Tsc;                    // Training sequence.
 u32     PsiHrBitMap;            // A bit set indicates that the SI corresponding to the bit index in the High Rate PSI sequence must be received.
 u32     PsiLrBitMap [2];        // A bit set indicates that the SI corresponding to the bit index in the Low Rate PSI sequence must be received.
 u8      PS1RepeatPeriod;        // Number of multi-frame before PSI-1 re-appears [1..16]
 u8      PBcchBlks;              // Number of PBCCH  Blocks per multi-frame [1..4]
 u8      PsiHrNb;                // Number of Blocs for high rate repetitions [0..16 + 2]
 u8      PsiLrNb;                // Number of blocs for low rate repetitions [0..63]
 u8      Pb;                     // Power reduction.
} l1_PBcchReq_t;


/*
================================================================================
  Structure  : l1_CiphParam_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : this structure is used by the l1_ConnectReq_t and
               l1_CiphReq_t structures. It gives the ciphering configuration
               on the l1 connection
================================================================================
*/
typedef struct
{
  u8   Kc[8] ; // significant only if Start is TRUE
  u8   Algo  ;
  bool Start ; // TRUE : cipher ON ; FALSE : no ciphering
  u8  Pad [2];
} l1_CiphParam_t;

/*
** Possible values for Algo
*/
#define L1_A51 0
#define L1_A52 1
#define L1_A53 2
#define L1_A54 3
#define L1_A55 4
#define L1_A56 5
#define L1_A57 6


/*
================================================================================
  Structure  : l1_CellOpt_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : this structure is used by the l1_ConnectReq_t and
               l1_CellOptUpdtReq_t structures. It gives the DTX, PWRC and RLT
               requirements on the current cell
================================================================================
*/
typedef struct
{
  bool Pwrc ; // TRUE if power control is set, FALSE otherwise
  u8   Rlt  ; // between 4 and 64
  u8   Dtx  ; // Significant for TCH only
  u8   Pad;
}  l1_CellOpt_t ;

/*
** Possible values for Dtx
*/
#define L1_DTX_MAY    0    // dtx may be used on the uplink
#define L1_DTX_ON     1    // dtx must be used on the uplink
#define L1_DTX_OFF    2    // dtx must not be used on the uplink

/*
================================================================================
  Structure  : l1_AMRCfg_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : AMR configuration.
================================================================================
*/

#define L1_NO_ICM 0xFF

typedef struct
{
 bool AMRCfgValid;
 u8 Version;            // AMR version (set to 1)
 u8 NoiseSuppression;   // Boolean indicating if the noise suppression is allowed.
 u8 StartModeIdx;       // ICM: Index in the ACS array of the Initial Codec to
                        // be used. When value is L1_NO_ICM initial codec selection
                        // is done according to 05.09 requirement.
 u8 ACS;                // Active Codec Set bit map  (cf 04.18)
 u8 Threshold  [3];     // NbActiveCodecSet -1 valid Threshold.
 u8 Hysteresis [3];     // NbActiveCodecSet -1 valid hysteresis.
} l1_AMRCfg_t;


/*
================================================================================
  Structure  : l1_ConnectReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR asks L1 to start a connection
================================================================================
*/
typedef struct
{
  l1_FreqParam_t * FreqParam[2] ; // Frequency Param for
                                  //    - immediate action only ([0]) or
                                  //    - later action only ([0]) or
                                  //    - both ([0] and [1])
  l1_CiphParam_t CipherParam    ;

  l1_CellOpt_t   CellOptions    ;
  l1_AMRCfg_t    AMRCfg         ; // AMR configuration.

  u16            ST             ; // Starting Time (FN modulo 42432) ;
                                  // L1_NO_ST = non significant
  u16            Arfcn          ; // Bcch Arfcn
  u8             ChanType       ; // MCE : voir avec ADA si on merge les 2
                                  // champs (ChanType et TdmaOffset)
  u8             TdmaOffset     ; // [0..7] (TS 04.18, channel description)
  u8             TN             ; // cf TS 04.18
  u8             Tsc            ; // cf TS 04.18
  u8             PowerLevel     ; // [0..31] (TS 04.18, power command)
  u8             ChanMode       ;
  u8             TA             ; // Timing Advance ( = L1_NO_TA if not known )

  // Assignment command and Handover command specific fields
  u8             HoType         ; // NONE, INTRA, NON_SYNCH, PRE_SYNCH,
                                  // PSEUDO_SYNCH, SYNCH, L1_RESUME
  bool           FPC            ; // Fast Meas Reporting Power Ctrl ;

  // Handover command specific fields
  u8             HORef          ;
  bool           Nci            ; // TRUE if new cell accepts extended TA
  bool           ATC            ; // TRUE if HO access optional, FALSE if
                                  // mandatory; 3GPP TS 4.18: if new cell
                                  // supports extended TA and TA > 63 and
                                  // ATC == TRUE, MS must not send HO access
  u8             RTD            ; // Real Time Difference, 1/2 bit periods
                                  // significant for pseudo-synch HO only
                                  //       to be checked with Antoine

} l1_ConnectReq_t ;

/*
** Possible values for ChanType
*/
#define         L1_SDCCH (1 << 6)
#define         L1_TCH   (1 << 7)

#define         L1_TCHF (1 | L1_TCH)
#define         L1_TCHH (2 | L1_TCH)
#define         L1_SD_4 (3 | L1_SDCCH)
#define         L1_SD_8 (4 | L1_SDCCH)


/*
** Possible values for ChanMode ( Do not change ; TS 04.18)
*/
#define         L1_CHN_MODE_DATA (1 << 7)

#define         L1_SIG_ONLY  0x0
#define         L1_SPEECH_V1 0x1
#define         L1_SPEECH_V2 0x21
#define         L1_SPEECH_V3 0x41
#define         L1_43_14_KBS (0x61 | L1_CHN_MODE_DATA) //data, 43.5kbit/s dwnlink, 14.5kbit/s uplink
#define         L1_29_14_KBS (0x62 | L1_CHN_MODE_DATA) //data, 29.0kbit/s dwnlink, 14.5kbit/s uplink
#define         L1_43_29_KBS (0x64 | L1_CHN_MODE_DATA) //data, 43.5kbit/s dwnlink, 29.0kbit/s uplink
#define         L1_14_43_KBS (0x67 | L1_CHN_MODE_DATA) //data, 14.5kbit/s dwnlink, 43.5kbit/s uplink
#define         L1_14_29_KBS (0x65 | L1_CHN_MODE_DATA) //data, 14.5kbit/s dwnlink, 29.0kbit/s uplink
#define         L1_29_43_KBS (0x66 | L1_CHN_MODE_DATA) //data, 29.0kbit/s dwnlink, 43.5kbit/s uplink
#define         L1_43_KBS    (0x27 | L1_CHN_MODE_DATA) //data, 43.5kbit/s
#define         L1_32_KBS    (0x63 | L1_CHN_MODE_DATA) //data, 32.0kbit/s
#define         L1_29_KBS    (0x43 | L1_CHN_MODE_DATA) //data, 29.0kbit/s
#define         L1_14_KBS    (0x0f | L1_CHN_MODE_DATA) //data, 14.5kbit/s
#define         L1_12_KBS    (0x03 | L1_CHN_MODE_DATA) //data, 12.0kbit/s
#define         L1_6_KBS     (0x0b | L1_CHN_MODE_DATA) //data, 6.0kbit/s
#define         L1_3_KBS     (0x13 | L1_CHN_MODE_DATA) //data, 3.6kbit/s

// Non-significant value for ST
#define         L1_NO_ST     0xffff

// Possible values for HoType
#define         L1_NO_HO           0
#define         L1_INTRA_HO        1
#define         L1_NON_SYNCH_HO    2
#define         L1_PRE_SYNCH_HO    3
#define         L1_PSEUDO_SYNCH_HO 4
#define         L1_SYNCH_HO        5
#define         L1_RESUME_HO       6

// Non-significant value for TA
#define         L1_NO_TA     0xff

//L1_CHN_MODE_INVALID used by RR to notify CC when a sdcch or tch is no longer available. alex 2007-09-10
#define         L1_CHN_MODE_INVALID  0xff

/*
================================================================================
  Structure  : l1_ConnectCnf_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 confirms establishment of the connection
================================================================================
*/
typedef struct
{
 u32 Otd        ; // Observed Time Difference exressed in 1/2 bit periods
 u8  FrqParIdx  ; // Index of the current frequency parameters
 u8  Pad [3];
} l1_ConnectCnf_t ;


/*
================================================================================
  Structure  : l1_CellOptUpdtReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR gives new Cell Options to L1
================================================================================
*/
typedef l1_CellOpt_t l1_CellOptUpdtReq_t ;


/*
================================================================================
  Structure  : l1_FreqRedefReq_t;
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR gives frequency parameter to L1.
================================================================================
*/
typedef struct
{
 u16 ST;
 u16 Pad;
 l1_FreqParam_t FreqParam;
} l1_FreqRedefReq_t;

#define L1_ST_INVALID 0xFFFF

/*
================================================================================
  Structure  : l1_FreqRedefCnf_t;
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 confirms that a change of frequency params has occured,
               following ST expiry for a Frequency Redefinition or a
               Connect Request
  Note       : No body
================================================================================
*/
typedef u8 l1_FreqRedefCnf_t;

/*
================================================================================
  Structure  : l1_CiphReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR gives ciphering configuration to L1.
================================================================================
*/
typedef l1_CiphParam_t l1_CiphReq_t;


/*
================================================================================
  Structure  : l1_ChanModeModifyReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR gives a new channel mode to L1
================================================================================
*/
typedef struct
{
  u8             ChanMode ; // new channel mode
  u8             Pad [3]  ;
  l1_AMRCfg_t    AMRCfg   ; // AMR configuration.
} l1_ChanModeModifyReq_t;


/*
================================================================================
  Structure  : l1_ReleaseReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR requests L1 to release the main DCCH
               and its associated channel
================================================================================
*/
typedef u8 l1_ReleaseReq_t ;


/*
================================================================================
  Structure  : l1_PhyInfoReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR sends to L1 the physical information received from the
               network
================================================================================
*/
typedef struct
{
  u8             TA;
} l1_PhyInfoReq_t ;


/*
================================================================================
  Primitive  : l1_TxDiscardReq_t
--------------------------------------------------------------------------------

  Scope      : L2/L3 requests L1 to remove all occurences of a list of radio 
               blocks from  the given transmission Queue

================================================================================
*/
typedef struct
{
   u8         QueueId    ;
   u8         NbId       ;      // Nb of block IDs to discard 
   u8         Pad [2]    ;
   u32        Id  [1]    ;      // List of blocks IDs to discard; 
                                // Multi purpose Id.
} l1_TxDiscardReq_t ;

/*
================================================================================

                                GPRS

================================================================================
*/
/*
** Downlink Power Control parameters
*/
typedef struct
{
 u8 P0;
 u8 BtsPwrCtrlMode;
 u8 PrMode;

} l1_DlPwrCtrl_t;

/*
** Possible values for BtsPwrCtrlMode
*/
#define L1_MODE_A 0
#define L1_MODE_B 1
#define L1_PWR_INVALID 0xFF

/*
** Possible values for PrMode
*/
#define L1_PR_MODE_A 0
#define L1_PR_MODE_B 1

/*
** Packet Timing Advance parameters
*/
typedef struct
{
 u8 TAValue;
 u8 TAIndex[2];
 u8 TATN[2];
 u8 PTCchUlFormat;

} l1_GPTimAdv_t;
#define L1_TA_INVALID 0xFF
#define L1_PTCCH_AB_8  0
#define L1_PTCCH_AB_11 1

/*
** TA Array index
*/
#define L1_TBF_UL 0
#define L1_TBF_DL 1

/*
** Dynamic allocation structure
*/
typedef struct
{
 u8           USFGranularity;
 u8           USF[8];

} l1_MacDyn_t;


/*
** Fixed allocation structure
*/
typedef struct
{
 u8  DlCtrlTs;          // Downlink control timeslot number.
 u8  AllocationLen;     // For setup only. Number of blocks period described by
                        // the array Allocation bitmap (Set to 0 for an update).
 u8 *AllocationBitmap;  // For setup only. Slot allocation for each block period
                        // of the allocation period (NIL for an update).
} l1_MacFixed_t;

/*
** Power control parameters
*/
typedef struct
{
 u8 Alpha;
 u8 Gamma[8];

} l1_PwrCtrl_t;
#define L1_ALPHA_INVALID 0xFF

/*
** Common structure for assignment
*/
typedef struct
{
 u8             MacMode;
#ifdef __EGPRS__
 u8 TbfMode; // lowest bit 0. bit 0,1,2,3 for DL TBF mode, bit 4,5,6 for UL TBF mode. bit 7 for test mode
 u8 LinkQualMeasMode;
 u8 BepPeriod;
 u8 BepPeriod2;
#endif
 u8             TSC;
#if 1
 u8             TFI[2];
 u16             ParamBitmap;
#else
  u8             TFI;
  u8             ParamBitmap;
#endif
 u8             TsAlloc[2];   /* MSB : TN0 */
 u16            ST;
 l1_DlPwrCtrl_t DlPwrCtrl;
 l1_PwrCtrl_t   PwrCtrl;
 l1_FreqParam_t FreqParam;
 l1_GPTimAdv_t  GPTimAdv;

} l1_CmnAssgt_t;

/*
** Possible values for MacMode
*/
#define L1_MAC_SINGLE 1
#ifdef __EGPRS__
#define L1_MAC_MULTI 2   // for EGPRS use. added by zhanghy. 2006-11-14
#define L1_MAC_DYN    3
#define L1_MAC_EXT    4
#define L1_MAC_FIXED  5

#define L1_BEP_NVALID 0xFF

// possible value for Tbfmode. added by zhanghy. 2006-11-14, lowest bit 0,1,2,3 for DL TBF, bit 4,5,6 for UL TBF, bit 7 for Test Mode.
#define L1_TBF_GPRS 0
#define L1_TBF_EGPRS_NACK (1<<0)
#define L1_TBF_EGPRS_ACK_IR (1<<1)
#define L1_TBF_EGPRS_ACK_NOIR (1<<2)
#define L1_TBF_DL_MODE_MASK (0xF)
#define L1_TBF_UL_EGPRS (1<<4)
#define L1_TBF_EGPRS_RESET (1<<6)
#define L1_TBF_EGPRS_TESTMODE  (1<<7) //for EGPRS test mode. by zhanghy. 2007-09-06

//possible value for LinkQualMeasMode. for egprs channel qualitiy for every timeslot. added by zhanghy. 2006-11-14
#define L1_LINKMEAS_NONE    0
#define L1_LINKMEAS_ILEVEL   1
#define L1_LINKMEAS_BEP      2
#define L1_LINKMEAS_BOTH    3
#else
#define L1_MAC_DYN    2
#define L1_MAC_EXT    3
#define L1_MAC_FIXED  4
#endif

/*
** Possible values for ParamBitmap
*/
#define L1_PB_DLPWRCTRL ( 1 << 0 )
#define L1_PB_FREQPARAM ( 1 << 1 )
#define L1_PB_GPTIMADV  ( 1 << 2 )
#define L1_PB_ST        ( 1 << 3 )
#define L1_PB_PWRCTRL   ( 1 << 4 )
#define L1_PB_GPWRCTRL  ( 1 << 5 )
#define L1_PB_MEASMAP   ( 1 << 6 )
#if 1
#define L1_PB_ULTFI  (1<<7)
#define L1_PB_DLTFI  (1<<8)
#else
#define L1_PB_TFI       ( 1 << 7 )
#endif


/*
================================================================================
  Structure  : l1_UsfInd_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 notify to RR that a valid USF has been found.
================================================================================
*/
typedef u8 l1_UsfInd_t;

/*
================================================================================
  Structure  : l1_TBFCfgReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR sends to L1 a TBF configuration
================================================================================
*/
/*
** TBFBitmap for the l1_TBFCfgReq_t
*/
#define L1_TBF_UL_SET ( 1 << L1_TBF_UL )
#define L1_TBF_DL_SET ( 1 << L1_TBF_DL )
#define L1_TBF_MACDYN ( 1 << 2 )
#define L1_TBF_MACFIX ( 1 << 3 )
#define L1_TBF_DL_MODE_RESET ( 1 << 4 )
typedef struct
{
 u8            TBFBitmap;
 l1_CmnAssgt_t CmnAssgt;
 l1_MacDyn_t   MacDyn;
 l1_MacFixed_t MacFixed;

} l1_TBFCfgReq_t;

/*
================================================================================
  Structure  : l1_TBFCfgCnf_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 sends to rr a TBF configuration confirmation after starting
               time due date.
================================================================================
*/
typedef struct
{
 u8            TBFBitmap;
} l1_TBFCfgCnf_t;

/*
================================================================================
  Structure  : l1_MacFixedUpdtReq_t;
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR sends an update configuration for the current fixed allocation
================================================================================
*/
typedef struct
{
 u8  BlkIndex;     // Block index in the allocation period (retrieved in the interlayer carrying the Ack/Nack)
 u8  TsOverRide;   // Used to over ride the current timeslot allocation.
} l1_MacFixedUpdtReq_t;

/*
================================================================================
  Structure  : l1_EndFixedPeriodInd_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 indicates to RR that the current period of the fixed
               allocation has ended.
================================================================================
*/
typedef struct
{
 bool Repeat;          // Indicates if the current allocation is being repeated
                       // for the next period (when required by a MacFixedUpdt).
} l1_EndFixedPeriodInd_t;

/*
** Global Power Control parameters
*/
typedef struct
{
 u8 Alpha;
 u8 TAvgW;
 u8 TAvgT;
 u8 Pb;
 u8 PCMeasChan;
 u8 NAvgI;
 u8 TxPwrMax; // GPRS_MS_TXPWR_MAX_CCH if PBCCH exist MS_TXPWR_MAX_CCH otherwise
} l1_GPwrCtrl_t;

typedef l1_GPwrCtrl_t l1_PMeasCtrlReq_t;

/*
================================================================================
  Structure  : l1_PwrCtrlTAReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR sends to L1 a new power control and/or TA configuration
================================================================================
*/
typedef struct
{
 u8            ParamBitmap;
 l1_PwrCtrl_t  PwrCtrl;
 l1_GPTimAdv_t GPTimAdv;
 l1_GPwrCtrl_t GPwrCtrl;

} l1_PwrCtrlTAReq_t;

/*
================================================================================
  Structure  : l1_PDCHRelReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR asks L1 to release some PDCH
================================================================================
*/
typedef struct
{
 u8 NewTsAlloc;

} l1_PDCHRelReq_t;

/*
================================================================================
  Structure  : l1_TBFRelReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR asks L1 to release a or all the TBFs
================================================================================
*/
typedef struct
{
 u8 TBFBitmap;

} l1_TBFRelReq_t;

/*
================================================================================
  Structure  : l1_PDataPurgeInd_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RLU
  Scope      : L1 indicates to RLU that the PData queue request has been
               taken into account => Queue is purged.
================================================================================
*/
typedef struct
{
 u8  QueueId;
 u8  NbId   ;
 u8  Pad [2];
 u32 Id  [1];
} l1_PDataPurgeInd_t;

typedef l1_PDataPurgeInd_t l1_TxDiscardInd_t ;

/*
================================================================================
  Structure  : l1_PSDataReq_t
--------------------------------------------------------------------------------
  Direction  : MAC -> L1
  Scope      : MAC requests L1 to send a polling at a certain date
  Comment    : This message is an interlayer
================================================================================
*/
typedef struct
{
 u16 ST;
 u8  TN;
 u8  CS;
 u8  Flags;
 u8  Format;
 u8  Index;
 u32 Fn;         // For L1 private use.
} l1_PSDataReq_t;

/*
** Possible values for Flags
*/
#define L1_PSREQ_TX_MANDATORY   ( 1 << 0 )
#define L1_PSREQ_DLK_ACK_NACK   ( 1 << 1 )
#define L1_PSREQ_PAYLOADTYPE_10 ( 1 << 2 )
#define L1_PSREQ_UL_TBF         ( 1 << 3 )
#define L1_PSREQ_DL_TBF         ( 1 << 4 )
#define L1_PSREQ_REPLACE_OLDONE ( 1 << 5 )
#define L1_PSREQ_PCCO           ( 1 << 6 )
#define L1_PSREQ_DL_ASSIGNMENT    (1 << 7)

/*
** Possible values for Format
*/
#define L1_PSREQ_NB    1
#define L1_PSREQ_AB_8  2
#define L1_PSREQ_AB_11 3

/*
================================================================================
  Structure  : l1_PSDataInd_t
--------------------------------------------------------------------------------
  Direction  : L1 -> MAC
  Scope      : L1 indicates the status of emission of the Packet Synchro Data
================================================================================
*/
typedef struct
{
 u8 Index;
 u8 EndTxDueDate;   // Nunber of frames to wait before Tx completion.

} l1_PSDataInd_t;


/*
** Packet Uplink Dummy Control Block
*/
#ifdef __ITF_L1_VAR__
u8 l1_PUlDumCtrl [23] = { 0x40, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x2B, 0x2B, 0x2B,
                          0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
                          0x2B, 0x2B, 0x2B, 0x2B, 0x2B };
                          
//u8 l1_PUlDumCtrl_test [23] = { 0x40, 0x0C,0x00, 0x00, 0x00, 0x00, 0x19, 0x69, 0xb1, 0xa9, 0x5e, 0xaf, 0x1c, 0x20, 0x84, 0xa6, 0xeb, 0xce, 0x68, 0x36, 0x5d, 0x9a, 0x24};
#else
extern u8 l1_PUlDumCtrl [23];
//extern u8 l1_PUlDumCtrl_test [23];
#endif
#ifdef __EGPRS__
/*
================================================================================
  Structure  : l1_IRBufferStatus_Ind_t
--------------------------------------------------------------------------------
  Direction  : L1 -> RR
  Scope      : L1 indicates the RR that the status of the buffer for IR has been changed.  
  by	     : zhanghy
================================================================================
*/
typedef struct
{
 u8  staus;
} l1_IRBufferStatus_Ind_t;
// possible status of the buffer
enum { L1_IRBUF_CLEAR, L1_IRBUF_OVERFL};

/*
================================================================================
  Structure  : l1_LoopbackSwiReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : ask L1 to switch on/off EGPRS test mode.  
  by	     : zhanghy.2007-09-06
================================================================================
*/
typedef struct
{
 				u8           SubMode;
} l1_LoopbackSwiReq_t;

#define L1_EGPRS_LOOPBACK_SUB_OFF 0
#define L1_EGPRS_LOOPBACK_SUB_ON 1

#endif
/*
================================================================================

                                TEST MODE

================================================================================
*/


/*
================================================================================
  Structure  : l1_SetTestLoop_t
--------------------------------------------------------------------------------
  Direction  : RR -> L1
  Scope      : RR activates or deactivates a TCH loop or DAI test mode
================================================================================
*/
typedef struct
{
 u8 Mode ;

} l1_SetTestLoopReq_t ;

/*
** Possible values for Mode
*/
#define  L1_CLOSE_A_LOOP     0
#define  L1_CLOSE_B_LOOP     1
#define  L1_CLOSE_C_LOOP     2
#define  L1_CLOSE_D_LOOP     3
#define  L1_CLOSE_E_LOOP     4
#define  L1_CLOSE_F_LOOP     5
#define  L1_CLOSE_I_LOOP     6
#define  L1_SPCH_DECOD_LOOP  7
#define  L1_SPCH_ENCOD_LOOP  8
#define  L1_ACCOUSTIC_TEST   9
#define  L1_OPEN_LOOP        10
/*
================================================================================
  Structure  : l1_GprsTestModeReq_t
--------------------------------------------------------------------------------
  Direction  : RLU -> L1
  Scope      : notify the test mode of gprs, test mode type A or Type B(B is loop)
================================================================================
*/
typedef struct
{
    u8 TestMode;
}l1_GprsTestModeReq_t;

#define L1_GPRS_TST_MODE_A 1
#define L1_GPRS_TST_MODE_B 2











































#endif

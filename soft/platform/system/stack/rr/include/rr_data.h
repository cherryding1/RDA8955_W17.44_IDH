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
  File       : rr_data.h
--------------------------------------------------------------------------------

  Scope      : Defintion of Global data for rr  

  History    :
--------------------------------------------------------------------------------
  Mar 31 06  |  MCE   | Moved CbchStatus to rri_data.h
  Sep 23 05  |  MCE   | Moved Ca and CaAvail from rr_cell.h             
  Aug 09 05  |  MCE   | Moved GprsAttached from rri_data.h
  Mar 31 05  |  MCE   | Added CbchStatus
  Mar 28 05  |  MCE   | Moved BandList from rri_data.h
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RR_DATA_H__
#define __RR_DATA_H__

#include "rr_msg.h"
#include "rri_data.h"   // Structures for Idle mode        
#include "rrd_data.h"   // Structures fro connected mode 
#include "rr_dec.h"
#include "mac_data.h"
#define MAC_DATA pRRCtx->Mac
#include "rrp_data.h"

#define RRI_DATA pRRCtx->Rri
#define RRD_DATA pRRCtx->Rrd




typedef struct {
   u8   Len      ; // Nb of bytes in Val
   u8   Val[12]  ; // Value field of MsCm3, ready to send
} rr_MsCm3_t ;




typedef struct {

   /*
   ** General global data        
   */
   #ifndef STK_SINGLE_SIM
   u32	   SimIndex;
   #endif
   
   Msg_t       * RecMsg           ;// Received message
   Msg_t       * OutMsg           ;// Message to send
   u32         * RecSig           ;// Received Signal

                                   
   Fsm_t       * Fsm              ;// Current selected Fsm
   InEvtDsc_t  * IEvtDsc          ;// Incoming Event
   OutEvtDsc_t * OEvtDsc          ;// Outgoing Event
                                   
   void        * DecOut           ;// Ptr to Ouptut of the decoder 
   bool          KeepMsg          ;// TRUE if RecMsg must not be deleted 
                                   
   rr_Cell_t   * MainCell         ;// Pointer to the Main Cell
   rr_Cell_t   * RecCell          ;// Pointer to the Curr Cell
   u16           RecArfcn         ;// Value of current Arfcn  
   u8            BandList         ;// Which band to search
   //bool          CaAvail          ;// True if Ca received   deleted by leiming 20071227
   rr_CaL_t      Ca               ;// Cell Allocation (SysInfo 1,contd mode) 
                                   
   rri_Data_t    Rri              ;// Data for Idle Mode   
   rrd_Data_t    Rrd              ;// Data for Circuit Switch Mode   
   mac_Data_t    Mac              ;// Data for MAC GPRS
                                   
   bool          NewConn          ;// Set to TRUE  after a new L1 Conn estab
                                   // Set to FALSE after the first MeasReport
   u8            OperMode         ;// From Sim Admin Data
   bool          GprsWanted       ;// TRUE if MM wants the GPRS service
   bool          GprsAttached     ;// TRUE when GPRS attached
   u8            GprsNwkStatus    ;
                                   
                                   
   bool          SimPresent       ;// Sim is present
                                                //in fact , this var is mean if an valid SIM was found(by leim 20090319)
                                                
   bool          MmReadySt        ;// MM is in ready State
   bool          MmReadyTimEqZero ;// MM Ready Timer set to 0 ?
   u8            MsCm2[3]         ;// MS ClassMark 2
   rr_MsCm3_t    MsCm3            ;
   //u8            Ta               ;  //deleted by leiming 20080721 moved to cell->servp

   u8            L1ConnState      ;   // See rr_rr Itf l1ConnInd

   bool         SimOpened;  //added by leim 20090319 for the sequence of STK start request and SIM open indication

   u8           RejCnt            ;//20140811,if rej then RRP_ABREL_WAR.if more than 5 times on a same arfcn ,resel.part 3 of 5.
   s16          RejCellArfcn      ;//20140811,if rej then RRP_ABREL_WAR.if more than 5 times on a same arfcn ,resel.part 3 of 5.   
   u8 			bLockArf;

}rr_Data_t;

#ifdef __RR_C__
#define DefExtern 




u8 rr_MultislotCap[12][3] =
  { {1,1,2}, {2,1,3}, {2,2,3}, {3,1,4}, {2,2,4}, {3,2,4},
    {3,3,4}, {4,1,5}, {3,2,5}, {4,2,5}, {4,3,5}, {4,4,5} };
#else
#define DefExtern extern
extern u8 rr_MultislotCap[12][3];
#endif

#define RR_MSCAP_RXMAX 0
#define RR_MSCAP_TXMAX 1
#define RR_MSCAP_SUM   2

/*
** RR Finite State Machines
*/
#ifndef STK_SINGLE_SIM
extern Fsm_t      rrd_Fsm[STK_MAX_SIM]  ;
extern Fsm_t      rri_CFsm[STK_MAX_SIM] ;
extern Fsm_t      rri_PFsm[STK_MAX_SIM] ;
#else
extern Fsm_t      rrd_Fsm  ;
extern Fsm_t      rri_CFsm ;
extern Fsm_t      rri_PFsm ;
#endif

#ifndef STK_SINGLE_SIM
DefExtern rr_Data_t  rr_Data[STK_MAX_SIM]  ;
#else
DefExtern rr_Data_t  rr_Data  ;
#endif

DefExtern rr_Data_t  *pRRCtx  ;

DefExtern const api_StackCfg_t *psap_StackCfg  ;

#undef DefExtern

#endif

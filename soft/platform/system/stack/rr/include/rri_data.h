//================================================================================
//  File       : rri_data.h
//--------------------------------------------------------------------------------
//  Scope      : Defintion of Global data for rri  
//  History    :
//--------------------------------------------------------------------------------
// 20091208| DMX | Delete NoSvceDur, added NoSvceSearchCount
//  Apr 12 06  |  MCE   | moved PmoInd to rri_DynMeasPar_t
//  Mar 31 06  |  MCE   | Moved CbchStatus from rr_data.h
//  Mar 07 06  |  MCE   | Added ArfcnBeforePcco and BsicBeforePcco
//  Jan 11 06  |  MCE   | Renamed TargetCell -> PbcchTargetCell
//  Dec 21 05  |  MCE   | Removed LaunchResel
//  Dec 02 05  |  MCE   | Added  PccoMeasPar, PccoRmvFrqL and PccoAddFrqL
//  Nov 01 05  |  MCE   | Removed RRI_READYST_NONDRX_MODE        
//  Oct 21 05  |  MCE   | Added RRI_READYST_NONDRX_MODE        
//  Aug 09 05  |  MCE   | Moved GprsAttached to rr_data.h
//  Mar 28 05  |  MCE   | Moved BandList to rr_data.h
//  Jun 12 03  |  PFA   | Creation            
//================================================================================

#ifndef __RRI_DATA_H__
#define __RRI_DATA_H__

#include "fsm.h"        // In and Out Evt description
#include "rr_msg.h"
#include "rr_freq.h"    // Bands definition stuff
#include "rr_cell.h"    // Cell structures

#include "rri_defs.h"   // Definitions of Mult Ins for Sysinfos
#include "rri_cmon.h"   // BA List structure



/*
** CSel Processes
*/
#define  RRI_MAX_NCDSC  64     // Max Cell in one Neighb Cell Descrip IE
#define  RR_MAX_STORED  33     // Max Number of stored Arfcns 

/*
** PSel Processes
*/
#define RR_SCAN_RUN   (1<<0)   // A scan of Plmn is Running        
#define RR_API_LIST   (1<<1)   // API has required to do a PlmnList
#define RR_HIGH_PRIO  (1<<2)   // A Plmn is targeted (possible reason for scan)
#define RR_EXIT_LTD   (1<<3)   // Try to get Full Svc(possible reason for scan)
#define RR_PLMN_RSL   (1<<4)   // Plmn Reselection has been asked by MMI       

/*
** DrxMode bitmap definitioins
*/
#define RRI_MM_NONDRX_MODE             ( 1<< 0 )
#define RRI_TRANSFER_NONDRX_MODE       ( 1<< 1 )
#define RRI_NC2_NONDRX_MODE            ( 1<< 2 )

#define FIRST_HPLMN_DUR (2 MINUTES)


#define RRI_SCHED_NOCHGE     0 //No chge of schedg, no partial acquisition to do
#define RRI_SCHED_SI2SET     1 //No chge of sched, need to listen to SI2Set
#define RRI_SCHED_SI2Q       2 //No chge of sched, need to listen to SI2Q
#define RRI_SCHED_PART       3 //No chge of sched, need to listen to spec SIs
#define RRI_SCHED_CHANGED    4 //Chge of sched(si13/Nosi13,PBCCH/NoPBCCH),
                               //sched info known
#define RRI_SCHED_START_PBCH 5 
#define RRI_SCHED_FIRST      6 //PBCCH: first listening after Rx of PSI1
#define RRI_SCHED_PSI2SET    7 //No chge of sched, need to listen to PSI2
#define RRI_SCHED_PSI3SET    8 //No chge of sched, need to listen to PSI3+3B
#define RRI_SCHED_PSI5SET    9 //No chge of sched, need to listen to PSI5
#define RRI_SCHED_SCRATCH 0xff //Sched info not known,complte acquisitn needed




#define RR_NO_CBCH           0  
#define RR_CBCH_WANTED (1 << 0) 
#define RR_CBCH_READY  (1 << 1) 

/* #Added by LIUBO 2009.10.20 begin */
/* Decrease the duration of RR_LTDSVC_TIM_EXP from 60s to 30s */
#define RR_LTDSVC_TIM_DUR    2
/* #Added by LIUBO 2009.10.20 end */

//added by leiming 20070807(begin)
// defination of this micro will enable black list strategy
#define RR_BLACK_LIST_ENABLED
//added by leiming 20070807(end)


#ifdef RR_BLACK_LIST_ENABLED
//added by leiming 2007.05.23(begin)
// this defination should not larger than 255
#define RRI_BLACK_LIST_CAPA 8

// this structure will be used to save such "0x4c" cell
// to resolve discontinuouity issue
typedef struct{
	u16 ARFCN;
	u8 BSIC;
	u8 RxLevAccMin;
}rri_black_list_item;

typedef struct{

	u8 ItemNo;
	rri_black_list_item Items[RRI_BLACK_LIST_CAPA];
}rri_black_list;
//added by leiming 2007.05.23(end)
#endif

typedef struct {

   /*
   **  Miscellaneous            
   */
   u16           ImsiMod1000              ;   // Imsi Modulo 1000        
   u8            MncLen                   ;   // Mnc coded on 2 or 3 bcd 

//   u8            L1ConnState              ;   // See rr_rr Itf l1ConnInd
#ifdef __GPRS__
   bool          L1NmoOn                  ;   
#endif
   bool          DbgReport                ;   // TRUE for Cyclic DBG report

   /*
   **  Cell (Re) Selection data 
   */
   ArfRssi_t     ARssiL[ L1_SIZ_RSSI ]    ;   // List of (Arfcn, Measure)
   u16           ARssiNb                  ;   // Number of Elems in List 
   u16           ARssiIdx                 ;   // Current Index in List   
   bool          ARssiValid               ;   // Recent Rssi List
   u16           ARssiStopIdx             ;   // LastIndex to try

   u16           StoredL[ RR_MAX_STORED ] ;   // List of stored Arfcn
   u16           StoredNb                 ;   // Number of stored Arfcns
   u8 	     StoredBand; // added by zhanghy, memorize the band of last regilsted.

   bool          LowCellDetected          ;   // A Low Cell has been seen
   bool          LowCellAccepted          ;   // Accept Low Cell when found
   bool          ForwardPag               ;   // Forward or Not Pag to MM  

   u16           ArfcnFromAvailNb         ;   // Numb of Arfcn from Avail Plmn
   u8            CSPlmn  [ 3 ]            ;   // Csel view of of Selected Plmn
   u8            OldMainIdx               ;   // Set After Resel
   u8            ReestCount               ;   // Limit to 6 Reestab Attempts

   /*
   ** Plmn Selection data 
   */   
   u8            PlmnSelMode              ;   // Auto - Manual
   bool          LtdSvc                   ;   // Limited Service Indicator
   // [[[mod][dingmx][20091208]
   // use a counter instead of the timer duration 
   //   u8            NoSvceDur           ;   // Current duration of NoNvceTimer
   u8            NoSvceSearchCount        ;   // how many times has done for PLMN search since lost PLMN;
   // ]][mod][dingmx][20091208]
   u8            HPlmnDur                 ;   // Duration for HPlmn scan 
   u8            LtdSvceSearchCount  ;  // how many times has done for PLMN search since LTD service;
   u8            PSelProc                 ;   // Running Processes
   u8            HPlmnFlag                ;   //Indicate if the first time to do HPlmn scan
   u8            SPlmn  [ 3 ]             ;   // Selected Plmn code
   u8            UPlmn  [ 3 ]             ;   // User Selected Plmn code
   u8            HPlmn  [ 3 ]             ;   // Home     Plmn code
   u8            LRPlmn [ 3 ]             ;   // Last Reg Plmn code

   u8            AdminSize                ;   // Size of EF_AD  File in Sim     
   u8            SstSize                  ;   // Size of EF_SST File in Sim     
   u8            UPlmnWatNb               ;   // User Plmns with Acces tech Nb
   u8            OPlmnWatNb               ;   // Oper Plmns with Acces tech Nb
   u8            PlmnSelecNb              ;   // Plmn Selector Nb        

                                              // 255/3 * 2 = 170
 // OTH
   u8          * PlmnSimL                 ;   // List of Plmns   from Sim
   u8            PlmnSimNb                ;   // Total nb of Plmns from Sim


   u8            FPlmnQ                   ;   // Queue of Forbidden Plmns
   u8            FLaiQ                    ;   // Queue of Forbidden Lai  
   u8            FLaiRegProQ              ;   // Queue of Forbidden LA for regional provision of service   
   u8            APlmnQ                   ;   // Queue of Available Plmns
   bool          APlmnQReady              ;   // RList Done              
   bool          AnyLai                   ;   // Accept Any Lai             

   u8            WPlmnIdx                 ;   // Current Index in WPlmnL
   u8          * WPlmnL                   ;   // Working list of Plmns (Sorted)
   u8            WPlmnNb                  ;   // Number of Plmn in WPlmnL
   bool          WPlmnMade                ;   // The list has been made or not

   u8            ReqBand   [ 2 ]          ;   // Table of Requested Bands
   u8            ReqBandNb                ;   // One or two requested Bands
   u8            CurBandIdx               ;   // where are we in the Tab

   
   /*
   ** Measurement and Cell List Stuff 
   */
   u8             BaNb                    ;   // Total Arfcn Nb in Ba  
   u8             BaInd                   ;   // BaInd of BaList
   u8             AddNcBaIdx              ;   // Idx where NC Add Freqs Start
                                              
   rri_BaElem_t * BaL                     ;   // Balist

   u8             MonNb                   ;   // Total Arfcn Nb in Monitor Lst
   rri_MonElem_t  MonL [L1_SIZ_MON]       ;   // Monitor List 33 Max
   bool           MonTag                  ;   // Toggle for list chge with l1
   bool           NewL1Mon                ;   // Set to True after each MonInd 
   u8             MainMonIdx              ;   // Idx of Main in MonL        
   l1_MonitorReq_t L1MonitorReqSav        ;   // Sav of last L1MonitorReq
   rr_NwStoreListInd_t *MsStoreL;//monitor list stored in flash

   rri_RtdList_t      Rtd6L [RRI_MAX_COUNT] ; // Tmp Rtd6 L  given in PsiInst 
   rri_RtdList_t      Rtd12L[RRI_MAX_COUNT] ; // Tmp Rtd12L  given in PsiInst 
   rri_BsicList_t     BsicL [RRI_MAX_COUNT] ; // Tmp Bsics   given in PsiInst 
   rri_PrioList_t     PrioL [RRI_MAX_COUNT] ; // Tmp RepPrio given in PsiInst 

#ifdef __GPRS__
   rri_NCellParList_t NCellParL [RRI_MAX_COUNT+1]; // 0:PSI3, 1-16:Psi3Bis 
   rri_NCellParList_t NCellPar2L[RRI_MAX_COUNT+1]; // 0:[]    1-16 for Psi3Bis
   rri_RmvNcFrqList_t PmoRmvFrqL[RRI_PMO_MAX_COUNT]; 
   rri_NCellParList_t PmoAddFrqL[RRI_PMO_MAX_COUNT]; 
   rri_RmvNcFrqList_t PccoRmvFrqL                  ; 
   rri_NCellParList_t PccoAddFrqL                  ; 
#endif

   u8                    GsmNCellNb       ;   // Total Cell in Gsm Neigh list
   u8                    GsmNCellBaInd    ;   // BaInd of GsmNCell
   u8                    AddNcGsmNCellIdx ;   // Idx where NC Add Freqs Start
                                              
   rri_GsmNCellElem_t *  GsmNCellL        ;   // Gsm Neigh Cell List
   bool                  InvBsicSeen      ;   // Inv Bsic Among the stronguest

   u8             ReportNb                ;   // Number of reported Cells 
   rr_Cell_t    * ReportCell   [32]       ;   // Table of Cells to report

   u8             EnhReportNb             ;   // Number of Enh reported Cells 
   rr_EnhRep_t    EnhReportCell[32]       ;   // Table of Cells to Enh report

#define           QUAL_REPORT    ( 1<< 0) 
#define           CURC_REPORT    ( 1<< 1) 
#define           ADJC_REPORT    ( 1<< 2) 
#define           SYNC_REPORT    (1 << 3)
   u8             InfoReport              ;   // Reports Infos to API if TRUE
   u8             SynchQ                  ;   // Synchronized cells Queue

#ifdef __GPRS__
   rri_DynMeasPar_t * Psi5MeasPar         ;   
                                              
   rri_DynMeasPar_t * PmoMeasPar          ;   
   rri_DynMeasPar_t * PccoMeasPar         ;   
   rr_Cell_t        * PccoCell            ;   
   u16                ArfcnBeforePcco     ;   
   u8                 BsicBeforePcco      ;   
#endif
     
   /*
   ** Reselection data
   */
   u8            ReselCause      ;// Cause of current reselection
   bool          RecntRsl        ;// TRUE during 15 sec after a resel
   u16           OrderArfcn      ;// Arfcn given by RRD or nwk to reselect on
   u8            OrderBsic       ;
   rr_Cell_t   * BestNeighb      ;
   rr_Cell_t   * PbcchTargetCell ;
                                  
                                  

   /*
   ** Plmn Scanning data
   */
   rr_Cell_t  *  ScanCell[RRI_MAX_SCAN_CELL]    ;    // Cell used for Plmn Scan process        
   bool          ScanProc    ;    // TRUE if a Plmn Scan has started         
   
   bool          PowerReq    ;    // Added by LIUBO 2009.07.30 to remember whether we have sent a 
                                            // RR_POWER_REQ message and have not received the response 
                                            // when RRI_C is doing scan on RRI_CCAMP state.        

   /*
   ** (P)Bcch stuff
   */
   rri_TcInfo_t  NewNeedTc   ;    // New needed TCs follwng a chge of sch.info
   u8            NewSched    ;    

   /*
   ** Paging stuff       
   */
   Msg_t       * PagMsg      ;    // Save of one Paging Msg
   bool          PagSaved    ;    // One Paging has been saved                
   u8            NonDrxMode  ;    // Bit field, see descr. at head of file

   /*
   ** CBCBH stuff
   */
   u8            CbchStatus  ;    
   bool          ScanBa; //flag indicate is ask L1 to scan the Arfcns of Ba list
   u8             CurrNwState;// full, limit or no service

	u8            EqualPlmnSize;
	u8            EqualPlmn[MAX_EQPLMN_IE_LEN];
#ifdef RR_BLACK_LIST_ENABLED
   //added by leiming 2007.05.23(begin)
   rri_black_list BlackList;
   //added by leiming 2007.05.23(end)
#endif
   bool    LocationLock;
   bool    CalibMode;
   u8            SimType;
   u8 		ReleaseRSelBest;
   u8 		ReleaseGRSelBest;
   bool     bDetectMBS;   
}rri_Data_t;



#endif


/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

===============================================================================
*/

/*
================================================================================
  File       : rr_cell.h
--------------------------------------------------------------------------------

  Scope      : Defintion of cell strcutures     

  History    :
--------------------------------------------------------------------------------
  May 03 06  |  MCE   | Added Acs Si7_8 in rri_ReselPar_t
  Mar 23 06  |  MCE   | Added bit RR_SI13 in RecInfo and ReqInfo bitmap.
  Feb 17 06  |  MCE   | Addition of RR_PCCH_SOON_REL      
  Dec 23 05  |  MCE   | Moved GprsInd and PcchStatus frm ServPar_t to ReselPar_t
  Dec 22 05  |  MCE   | Added PbcchCfgOk
  Dec 21 05  |  MCE   | Moved RcvPsi and RcvSi to rr_Cell_t
  Dec 20 05  |  MCE   | Moved Pbcch description to rr_ReselPar_t
  Oct 26 05  |  MCE   | changed rri_PTcInfo_t.Psi to an u32 
  Sep 23 05  |  MCE   | Moved Ca and CaAvail to rr_data.h
             |        | Moved rr_SubBaL_t and rr_CaL_t to rr_defs.h
  Sep 21 05  |  MCE   | Changed RecInfo ReqInfo and RcvPsi to u32; merged 
             |        | their flags into the same u32
  Jun 08 05  |  FCH   | Change Rac from u8 to u16 in rr_ServPar_t
  Mar 31 04  |  MCE   | Addition of CbchCfg, TmpCbchCfg, CbchMaLen and CbchRawMa
  Nov 22 04  |  MCE   | Changed MaxRetrans to an array in GprsAcc
  Sep 23 04  |  MCE   | Changed IsPcchPres (bool) to PcchStatus (u8)
  Aug 12 04  |  MCE   | Addition of RR_CNEWRACH for the rr_Cell_t Status field
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RR_CELL_H__
#define __RR_CELL_H__

#include "itf_l1.h"   // Freq List definition
#include "rr_freq.h"  // Band definition stuff 
#include "rri_defs.h" // MultInst  definition
#include "rr_dec.h"   // ed_GprsMa_t definition
#include "rr_defs.h" 
#include "rr_msg.h"






#define RR_RACH      (1<<0)    // Rach Params
#define RR_PLMN      (1<<1)    // Lai 
#define RR_RSEL      (1<<2)    // Reselection Params
#define RR_ORSEL     (1<<3)    // Optional reselection Params
#define RR_PAG       (1<<4)    // Paging Params
#define RR_CA        (1<<5)    // Cell Allocation
#define RR_BA        (1<<6)    // At least BA form Si2B or Si2N
#define RR_PRACH     (1<<7)    // PRach Params
#define RR_RX_SI13   (1<<8)    
                               

#define RR_PSI1  (1<<16)
#define RR_PSI2  (1<<17)
#define RR_PSI3  (1<<18)
#define RR_PSI3B (1<<19)
#define RR_PSI3T (1<<20)
#define RR_PSI3Q (1<<21)
#define RR_PSI4  (1<<22)
#define RR_PSI5  (1<<23)
#define RR_PSI8  (1<<24)
#define RR_PSI13 (1<<25)
#define RR_PSI_NB    10        
                               


#define RR_NO_PSI 0
#ifdef __EGPRS__
#define RR_PSI_ALL  (u32) (( RR_PSI1  \
                           | RR_PSI2  \
                           | RR_PSI3  \
                           | RR_PSI3B \
                           | RR_PSI3T \
                           | RR_PSI3Q \
                           | RR_PSI5 \
                           | RR_PSI13))
#else
#define RR_PSI_ALL  (u32) (( RR_PSI1  \
                           | RR_PSI2  \
                           | RR_PSI3  \
                           | RR_PSI3B \
                           | RR_PSI3T \
                           | RR_PSI3Q \
                           | RR_PSI5 ))
#endif
                             

#define RR_PSI_UNSPE  RR_PSI_ALL


/*
** Bit Map for Status of Cell 
*/
#define RR_CSYNCS   (1<<0)    // Cell synchronization pending (BSIC Search)
#define RR_CSYNCF   (1<<1)    // Cell is synchronized (BSIC Found)
#define RR_CSYNCR   (1<<2)    // Cell Read Bcch Requested                   
#define RR_CAVAIL   (1<<3)    // Cell has been seen for Available Plmn List
#define RR_CSCAN    (1<<4)    // Cell is used for scan            
#define RR_CNEWPAG  (1<<5)    // Cell Just got new Pag Param      
#define RR_CNEWNAS  (1<<6)    // Cell Just got new Nas Param      
#define RR_CPNLTY   (1<<7)    // Cell Used Pnlty Timer            
#define RR_CREPORT  (1<<8)    // Cell used for Measrt Report      
#define RR_CSYNCKP  (1<<9)    // Cell to keep synchro on ( strong )
#define RR_CNEWRACH (1<<10)   // Cell's Rach Ctrl Params have changed
#define RR_NCMEAS   (1<<11)   // Cell's NC Meas Timers                
#define RR_CCCHR    (1<<12)   
#define RR_CPCCHR   (1<<13)   
#define RR_CPBCCHR  (1<<14)   
#define RR_CNEWCB   (1<<15)   

/*
** Ba List Components for Main only
** 3 Components may exist : 3 Indexes in Main SubBa  
**   -   SI2  (or SI5 )
**   -   SI2B (or SI5B )
**   -   SI2T (or SI5T )
*/
#define RR_SUB25N  0               // Sub list from SI2
#define RR_SUB25B  1               // Sub list from SI2Bis
#define RR_SUB25T  2               // Sub list from SI2Ter

/*
** BaReq and BaRec bit Field ( for serving )
*/
#define RR_BA25N (1<<0)    // Sysinfo 2   or 5
#define RR_BA25B (1<<1)    // Sysinfo 2B, or 5B
#define RR_BA25T (1<<2)    // Sysinfo 2B, or 5B

/*
** Si bitmaps ( for serving ) RcvSi
*/
#define RR_NO_SI 0
#define RR_SI1  (1<<0)
#define RR_SI2  (1<<1)
#define RR_SI3  (1<<2)
#define RR_SI4  (1<<3)
#define RR_SI2B (1<<4)
#define RR_SI2T (1<<5)
#define RR_SI2Q (1<<6)
#define RR_SI7  (1<<7)
#define RR_SI8  (1<<8)
#define RR_SI13 (1<<9)

#define RR_SI_BA   ( RR_SI2 | RR_SI2B | RR_SI2T ) 
#define RR_SI_ALL  ( RR_SI1    \
                     | RR_SI2  \
                     | RR_SI3  \
                     | RR_SI4  \
                     | RR_SI2B \
                     | RR_SI2T \
                     | RR_SI2Q \
                     | RR_SI7  \
                     | RR_SI8  \
                     | RR_SI13 )

#define RR_SI_UNSPE  RR_SI_ALL

/*
** Si13GMaInfo bit field
*/
#define RR_GMA_CA         (1<<0)     // CA received
#define RR_GMA_RFL        (1<<1)     // RFLs received (or not broadcast)
#define RR_GMA_SI13MA     (1<<2)     // Si13 GPRS MA received
#define RR_GMA_CA_CHG     (0x10<<0)  // CA has changed
#define RR_GMA_RFL_CHG    (0x10<<1)  // RFLs have changed
#define RR_GMA_SI13MA_CHG (0x10<<2)  // Si13 GPRS MA has changed
  
/*
** Tools to set TC bits in TC bitmaps
** ( use with "|=" )
*/
#define RR_NO_TC   0
#define RR_TC0    (1<<0)
#define RR_TC1    (1<<1)
#define RR_TC2    (1<<2)
#define RR_TC3    (1<<3)
#define RR_TC4    (1<<4)
#define RR_TC5    (1<<5)
#define RR_TC6    (1<<6)
#define RR_TC7    (1<<7)

#define RR_TC_ALL (RR_TC0|RR_TC1|RR_TC2|RR_TC3|RR_TC4|RR_TC5|RR_TC6|RR_TC7)

/*
** Values for NcOrder
*/
#define RR_NC0      0
#define RR_NC1      1
#define RR_NC2      2
#define RR_NC_RESET 3

/*
** Values for Mscr and Sgsnr
*/
#define RR_SGSNR_OLD 0 // R98 or older
#define RR_SGSNR_NEW 1 // R99 onwards




#define RR_PBCH_NO_MAIO 0xFF




#define RR_PBCH_NO_TSC 0xFF




#define RR_PBCH_NO_ARFCN 0xFFFF




#define RR_PBCH_NO_PSI1RP 0




#define RR_GPRS_STAT_UNKNOWN 0
#define RR_GPRS_PRESENT      ( 1 << 0 ) 
#define RR_GPRS_ENABLED      ( 1 << 1 ) 
                                        
#define RR_GPRS_PKTIDLE      ( 1 << 2 ) 

#ifdef __EGPRS__
#define RR_EGPRS_SUPPORT		(1 << 4 )//added by leiming for EGPRS supporting
#endif
#define RR_IS_GPRS_PRES(_val) \
        ((_val & RR_GPRS_PRESENT) == RR_GPRS_PRESENT ) 




#define RR_PBCCH_STAT_UNKNOWN  0
#define RR_PBCCH_CFGRXD        ( 1 << 0 ) 
#define RR_PBCCH_ASKED         ( 1 << 1 ) 
                                          
#define RR_PBCCH_UP2DAT        ( 1 << 2 ) 
#define RR_PBCCHR_ASKED        ( 1 << 3 ) 




#define RR_PCCH_STAT_UNKNOWN  0
#define RR_PCCH_PRESENT       ( 1 << 0 )   
#define RR_PCCH_CFGRXD        ( 1 << 1 )   
#define RR_PCCH_SOON_REL      ( 1 << 2 )   
#define RR_PCCH_CAMPED        ( 1 << 3 )   

/*
** ReSelection paramters
*/
typedef struct {

  s8     C1             ;   // Used in GSM and GPRS
  s8     C31            ;   // Only for GPRS
  s16     C232           ;   // C2 GSM  C32 GPRS      
  s8     C32Qual        ;   // 
  u8     C31Hyst        ;   // Gprs Hysteresis  
  u8     MsTxPwrMaxCcch ;   // MsTxPwrMaxCcch in GSM/GPRS        
  u8     PowerOffset    ;   // Used for DCS class 3 otherwise set to 0
  u8     RxLevAccMin    ;   // RxlevAccMin in GSM/GPRS                  
  u8     Re             ;   // Reestablish Allowed 
  u8     TmpOffset      ;   // GPRS, GSM and UMTS
  u8     Cro            ;   // In GPRS Cro(n) -= CRo(s)
  u8     GprsCro        ;   // Cro provided for GPRS 
  u8     GprsCrh        ;   // Gprs Cell Resel Hyst (Crh if don't exist)
  u8     HcsThrshld     ;   // 0 in GSM  
  u8     HcsPrio        ;   // 0 in GSM, PriorityClass in GPRS
  u16    PenaltyTime    ;   // Brodacasted Penalty Timer          
  u16    PenaltyDur     ;   // Penalty Duration in Seconds 0 means No pnlty
  bool   PenaltyActiv   ;   // TRUE if PenaltyTimer running         
  u8     TResel         ;   // TRUE if NoRet TResel timer is active(Def= 5sec)
  bool   NoRetActiv     ;   // TRUE if NoRet TResel timer is active(Def= 5sec)
  u8     SameArea       ;   // 0xff at init,1 if same RA (routing) than Serv
  u8     RaColour       ;   // RA Colour 
  u8     BrdcstBsic     ;   // Broadcasted Bsic
  u8     Cba            ;   // Cell Barr Acces 
  u8     Cbq            ;   // Cell Barr Qualify    
  u8     ExcAcc         ;   // Solsa Exclusive Access
  u8     Lai [5]        ;   // Location Area Id 
  u8     Acs            ;   

                            // BCCH SCHEDULE
	                    //-------------------------------------
  bool   Si7_8          ;   
  bool   Si2Ter         ;   // SI2Ter is transmitted 
  u8     Si13Pos        ;   // 0(Norm) 1(Ext)  0xff(Not Trans) 
  u8     Si2QPos        ;   // 0(Norm) 1(Ext)  0xff(Not Trans) 
 
#ifdef __GPRS__
  u8     GprsStatus     ;
#ifdef __EGPRS__
  u8     BEP_period		;
  bool  ChanReqType   ;
#endif
  u8     PbcchStatus    ;
  u8     PcchStatus     ;

                            // PBCCH DESCRIPTION
	                    //-------------------------------------
  u8     PbcchMaio      ;   // PBCCH Maio; Range [0..63,RR_PBCH_NO_MAIO]
  u16    PbcchArfcn     ;   // PBCCH Arfcn; Range [0..1023, RR_PBCH_NO_ARFCN]
  u8     PbcchTn        ;   // PBCCH Timeslot Number; Range [0..7]
  u8     PbcchTsc       ;   // PBCCH TSC; Range [0..7,RR_PBCH_NO_TSC]
  u8     PbcchBlks      ;   // BS_PBCCH_BLKS as expected by L1
                            
  u8     Pb             ;   // Range [0..15,RR_PBCH_NO_PB]
    
                            // PBCCH SCHEDULE
	                    //-------------------------------------
  u8     Psi1RepPeriod  ;   // PSI1 repeat period   ; 
                            
  u8     PsiCntHr       ;   // PSI_COUNT_HR         ; Range [0..16+2]
                            // (ready to give to L1)
  u8     PsiCntLr       ;   // PSI_COUNT_LR         ; Range [0..63]
#endif

} rr_ReselPar_t;





typedef struct 
{
  u8   NbRflL      ; 
  u8   RflNbL  [4] ; 
  u8   MaLen       ; 
  u8   MaBitMap[8] ; 
  u8   NbArfcnIdx  ; 
  u8 * ArfcnIdxL   ; 
} rr_RawGMa_t ;




typedef struct 
{
  u16         *FreqL  ; // Ptr to array of NbFreq Arfcn; = 0 if FreqNb = 0
  u8           FreqNb ; 
  u8           Hsn    ; // Significant only when FreqNb != 0
  u8           MaNb   ; // Significant only when FreqNb != 0
  rr_RawGMa_t *RawGMa ; 
} rr_GprsMa_t ;
// special values for MaNb
#define RR_MA_NB_SI13   14
#define RR_MA_NB_ASGT   15
#define RR_NO_MA_NB   0xff

// total nb of saved GPRS Mobile Allocations
#define RR_NB_GMA     8      

// Index in GMa of oldest PSI2 Gprs Mobile Allocation
#define RR_MA_PSI2_IDX_FIRST 0

// Index in GMa of most recent PSI2 Gprs Mobile Allocation
#define RR_MA_PSI2_IDX_LAST (RR_NB_GMA - 3) // We leave the last 2 spots for 
                                            // MA((P)SI13) and MA(Assgt Msgs)

// Index in GMa of the MA received in SI13 or PSI13 
// ( MaNb = 14)
#define RR_MA_SI13_IDX      (RR_NB_GMA -2)

// Index in GMa of the MA received in Assignment message
// ( MaNb = 15)
#define RR_MA_ASGT_IDX      (RR_NB_GMA -1)




typedef struct
{
  u8         RflNb  ;
  u8         FreqNb ;
  u16       *FreqL  ;        // RFL contents IE as received in PSI2
} rr_Rfl_t ; 

// Nb of saved Reference Frequency Lists
#define RR_NB_RFL      4     

// Value of FreqL[i] when no valid frequeny
#define RR_NO_FRQ 0xFF

/*
** Reporting Type ( Normal / Enhanced )
*/
#define RR_NO_REPORT  0
#define RR_NOR_REPTYP 1
#define RR_ENH_REPTYP 2

/*
** Offset & Threshold struct used for Meas
*/
typedef struct {
   u8 Offset      ;
   u8 Threshold   ;

} rr_OffThr_t ;

/*
** L1 configuration for BCCH listening
*/
typedef struct {
    u8  Tc[2]       ; // Bitmap of TCs
    u8  TcCnt[2][8] ; // Number of blocks on each TC
    u16 Si[2]       ; // Bitmaps of SIs for TC4 (idx 0) and TC5 (idx 1)
} rri_TcInfo_t ;

/*
** L1 configuration for PBCCH listening
*/
typedef struct {
    u32  Psi         ; // Bitmap of needed PSIs 
    u32  HrBitMap    ; // Bitmap of High Rate message instances (incl PSI1) 
    u32  LrBitMap[2] ; // Bitmap of Low Rate message instances 
} rri_PTcInfo_t ;
#define RRI_PSIBM_ALL 0xffffffff




#define RR_NBMAX_PCCH 16 // Maximum nb of Pcch channels in cell
typedef struct {
   u8  Tsc     ;
   u8  MaNb    ; 
   u8  Maio    ; 
   u8  Tn      ; 
   u16 Arfcn   ; 
} rri_PCcchDsc_t;
#define RR_NO_MAIO 0xff


/*
** Serving Parameters
*/
typedef struct {
	                            // GENERAL
	                            //-------------------------------------
    bool            MmAcked     ;   // MM has acknowledged the cell
    bool            FreshSi     ;   // SI13 or PSI1 was rcvd less than 30s ago
    u8              MissedSi13  ;   // Nb of "missed" SI13. When 2, resel
    u8              MissedPsi1  ;   // Nb of "missed" PSi1. When 2, resel
    u8              NccPerm     ;   // Ncc Permitted SysInfo 2,6 
    //have been moved to struct rr_Cell_t  //modified by leiming 20080718
    //u8              CellId [ 2 ];   // Cell Identifier
    u8              BandInd     ;   // 0 : Arfcn indicates 1800 band, 1 : 1900

                                    // NAS INFO for MM
	                            //-------------------------------------
    u8              Att         ;   // Attach Flag for MM
    u8              T3212       ;   // Periodic Update Timer

                                    // ACCESS - CONNECTED
	                            //-------------------------------------
    u8              MaxRetrans  ;   // Number of Re Trans after 1st Rach
    u8              TxInteger   ;   // For Rach Computation 
    u8              Ac [2]      ;   // Acces Class 
    u8              Pwrc        ;   // Power Control
    u8              Dtx         ;   // Discontinuous Transmission 
    u8              Rlt         ;   // radio Link Time Out 
    u8              Neci        ;   // New Estab Cause Indicator
    u8              Acs         ;   // Additional Cell Select Param
    bool            Ecsc        ;   // ECS Ctrl: TRUE if ECS allowed, FALSE
    u8              Mscr        ;   // MSC revision                      

                                      // BCCH reception
                                      //-------------------------------------
                                      // Needed Tcs and Sis
                                      // - - - - - - - - - -
    rri_TcInfo_t    NeedTc      ; 
                                      // Received Tcs and Sis
                                      // - - - - - - - - - - -
    u8              RcvTc[2]       ; // Bitmap of TCs
    u8              RcvTcCnt[2][8] ; // Number of blocks on each TC

                                      // Tcs requested to L1
                                      // - - - - - - - - - -
    u8              ReqTc[2]    ;     // Bitmap of TCs requested to L1 

                                    // BCCH SCHEDULE
	                            //-------------------------------------
    bool            ExtInd      ;   // SI2Bis is transmitted              
                                    // Other Stuff in ReselP              

                                    // PAGING
	                            //-------------------------------------
    u8              BsAgBlksRes ;   // Reserved Blks for Acc Grant 
    u8              CcchConf    ;   // Config Ccch                
    u8              BsPaMfrms   ;   // Paging Channel reccurency 
    u8              TN          ;   // Time Slot Number           



                                    // MEASURE PARAMS
	                            //-------------------------------------
    u8              SeqCod      ;   // Seq Code used in Extended Measurt 
    u8              MBReport    ;   // # of cells to report per Band 
    u8              RxLevFull   ;
    u8              RxLevSub    ; 
    u8              RxQualFull  ; 
    u8              RxQualSub   ;
    u8              RepType     ;   // Enhanced / Normal
    u8              SBReport    ;   // Serving Band Reporting        
    u8              RepRate     ;   // Reporting Rate
    u8              InvBsicRep  ;   // Invalid Bsic Reporting
    u8              ScaleOrd    ;   // Scale Order
    u8              NcOrder     ;   // Network Control Order
    u8              NcNonDrxPer ;   // NC Non DRX Period as rcved in Psi5
    u8              NcRepPerI   ;   // NC Reporting Period I
    u8              NcRepPerT   ;   // NC Reporting Period T
    u8              PmoInd      ;   
    bool            PMeasOrdRec ;   // True if Pkt Meas Order  Msg received
    u8              Crh         ;   // Cell Resel Crh 
    u8              RaCrh       ;   // Cell RaCrh (GPRS)(Crh if don't exist)

    rr_OffThr_t     RepOffThr[RR_LAST_BAND ]; // Offset & Threshold per Band


                                    // BA & list stuff 
	                            //-------------------------------------
    u8              BaInd       ;   // BA Indicator SysInfo 2,2B 5,5bis
                                    // Used as a Change Mark
    u8              BaRec       ;   // Bit field of Sys 2, 2B, 2T Received
    rr_SubBaL_t     SubBaL[3]   ;   // Save of components of Ba

    bool            BaCmplte    ;   // The BA is complete
    bool            NCellCmplte ;   // NCell list is complete

    bool            AllBsicRec  ;   // Each Arfcn has been allocated a Bsic


                                    // Multi Instance stuff
	                            //-------------------------------------
    rri_PsiInst_t   PsiInst [RRI_LAST_PSIIDX]  ;


                                    // GPRS related Params
	                            //-------------------------------------
                                    
    u8              BcchChgeMark ;  // 3 bit field

#define RR_CHGE_MARK_INVALID 0xFF
    u8              Si13ChgeMark ;  
#ifdef __GPRS__
    u8              PBcchChgeMark;  // 3 bit field
    u8              Psi2ChgeMark ;  // PSI2 Change Mark of last Rx PSi2 Set
    u8              Psi3ChgeMark ;  // PSI3 Change Mark
    u8              Psi5ChgeMark ;  // PSI5 Change Mark 

    rr_Rfl_t        Rfl[RR_NB_RFL]; // Reference Frequency Lists
    rr_GprsMa_t     GMa[RR_NB_GMA]; // GPRS Mobile Allocations
    u8              RflIdxNext    ; // Idx in Rfl of next saved PSI12 Rfl
    u8              Psi2GmaIdxNext; // Idx in GMa of next saved PSI12 Gprs Ma
    u8              CaRflNbL[4]  ;  // Array of CaRfl Numbers
    u8              NbCaRfl      ;  // Nb Of RFL in CaRflNbL
    u8              Si13GMaInfo  ;  // Bitfield of Rx info for Si13GMa computatn
#endif
    u16             Rac          ;  // Routing Area Code
#ifdef __GPRS__
    bool            SpgcCcchSup  ;  // support for SPLIT_PG_CYCLE param on CCCH
    u8              PrioAccThr   ;  // Priority Access Threshold
    u8              Sgsnr       ;   // SGSN Release                      

                                    // GPRS CELL OPTIONS
	                            //-------------------------------------
    u8              NwkMode      ;  // Network Mode of Operation 
                                    
    u16             T3168Dur     ;  //  T3168 duration unit: OS tick
    u16             T3192Dur     ;  //  T3192 duration unit: OS tick
    u16             DrxTimMax    ;  // Exprime en OS tick
    u8              AccBurstType ;  // Access Burst Type, 0: 8 bits, 1 : 11 bits
    u8              CtrlAckType  ;  // Ctrl Ack Type 0:4 AB,1: RLC/MAC ctrl blk
    u8              BsCvMax      ;  // BS CV MAX
    u8              PanDec       ;  // Pan Decrement, default value 0
    u8              PanInc       ;  // Pan Increment, default value 0
    u8              PanMax       ;  // Max Pan's value, default value 0
    u8              BepPeriod    ;  // BEP filter averaging period VOIR ADA
    bool            PfcSupport   ;  // Support for Packet Flow Context Procs.
    bool            BssPagCoord  ;  // Support for CS paging coordination

 //add for support REL 4, wuys 2013-11-27, begin

    bool            NwExtUTbf  ;  //   supports the extended uplink TBF mode: 
    // add end

                                    // GPRS TRANSFER
	                            //-------------------------------------
    bool            RandAccRetry ;  // Random Access Retry bit

                                    // GPRS POWER CONTROL PARAMETERS
	                            //-------------------------------------
    u8              Alpha        ;  // Range [0..10]
    u8              TAvgW        ;  // Range [0..25]
    u8              TAvgT        ;  // Range [0..25]
    u8              PcMeasChan   ;  // 0: Bcch, 1: Pdch
    u8              NAvgI        ;  // Range [0..15]

                                    // PCCCH DESCRIPTION
	                            //-------------------------------------
    u8              BsPAgBlksRes ;  // Reserved Blks for PAGCH
    u8              BsPRachBlks  ;  // Reserved Blks for PRACH
    u8              BsPccChans   ;  // Nb of TS carrying PCCCH in the cell
    rri_PCcchDsc_t *PCcchDsc     ;  // PCCCH timeslots descriptions

                                    // PBCCH reception
                                    //-------------------------------------
    rri_PTcInfo_t   NeedPTc      ;  // Needed TPcs and Psis

                                    
    bool            Psi3T        ;  // TRUE if PSI3T is broadcast
    bool            Psi3Q        ;  // TRUE if PSI3Q is broadcast
    bool            Psi4         ;  // TRUE if PSI4 is broadcast
    bool            Psi5         ;  // TRUE if PSI5 is broadcast
    bool            Psi8         ;  // TRUE if PSI8 is broadcast

                                    // GPRS Access Params
    rr_GprsAcc_t    GprsAcc      ;  //----------------------------------

#endif // GPRS

                                   
                                   
    Msg_t          *CbchCfg      ; 
    Msg_t          *TmpCbchCfg   ; 
    u8              CbchMaLen    ; 
    u8              CbchRawMa[4] ; 

    u8              Ta; //added by leiming 20080721 for Ta of current connection 
  
} rr_ServPar_t;


#define RR_SIGNAL_NOTREPORT_YET 0xFF
#define RR_SIGNAL_CHANGE               0x01
#define RR_SIGNAL_NOT_CHANGE       0x02

#define RR_CELL_RSL_SNR_THRESH       13
/*
** Normal Cell Parameters
*/
typedef struct {
	                            // GENERAL
	                            //-------------------------------------
    u16             Arfcn       ;   // Absolute Radio Frequency number 
     // moved to here from struct rr_ServPar_t  modified by leiming 20080718
    u8              CellId [ 2 ];   // Cell Identifier
    u8              Rla         ;   // Rla Current value [0..115]dBm            
    u8              RxLev       ;   // CurrenT Rxlev [0..63] Nc Measrt
    u8              RxQual      ;   // RxQualFull reported in Idle              
    u8              IdxBa       ;   // Index in the Ba List            
    u8              Bsic        ;   // Bsic of the Cell                
    
    u8              SignalChange;//flag indicate if RSSI change more than 5 DB
	
    u16             Status      ;   // Gal Status of Cell (AVAIL)
    u32             ReqInfo     ;   // Bit Map of Requested info (RACH,PLMN...)
    u32             RecInfo     ;   // Bit Map of Received  info (RACH,PLMN...)
    u32             RcvPsi      ;   // Bitmap of received PSIs
    u16             RcvSi       ;   // Bitmap of received SIs 
    bool            Psi1Once    ;   // TRUE if PSI1 was received once 

    bool            RejCsTimerOn;   // True if GSM Reject timer is running 
    bool            RejPsTimerOn;   // True if GPRS Reject timer is running 3142    

    u8               SnrAvg;   //average Snr for main cell
    

    rr_ReselPar_t * ReselP      ;   // Ptr to Resel Param           
    rr_ServPar_t  * ServP       ;   // Ptr to Serving specific Params 

} rr_Cell_t;

#endif


/*
================================================================================

  This source code is
								    property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : rri_cmon.h
--------------------------------------------------------------------------------

  Scope      : Defintion of strcutures used for monitoting of cells    

  History    :
--------------------------------------------------------------------------------
  Apr 12 06  |  MCE   | Added PmoInd and NewFrqL to rri_DynMeasPar_t
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RRI_CMON_H__
#define __RRI_CMON_H__

#include "rr_cell.h"   // Cell structures       



#define RR_MAX_MON      32       // No more than 32 cells to monitor           
#define RR_BSIC_NEEDED (1<<0)    // Need to ask a Synchro on this Arfcn        

/*
** Elem of a Gsm Monitor List                
*/
typedef struct
{
    u8   IdxBa        ;   // Index in the BA List                 
    u8   Rla          ;   // [0..115] dBm
    u8   Status       ;   // Remember that BSIC may be needed
    
} rri_MonElem_t ;

/*
** Bit Map for Status of Element of the BA 
*/
#define RR_ARF_ASLEEP  (1<<0)    // The arfcn is sleeping (Too many Synch err) 
#define RR_BSIC_ALLOW  (1<<1)    // At least one Bsic is given for This Arfcn  
#define RR_RESEL_BCST  (1<<2)    // Resel Parameters broadcasted (Psi3) 
#define RR_REMOV_ARF   (1<<3)    // Arfcn Removed ( Pckt Measrt order) 

/*
** Elem of a BA List
*/
typedef struct 
{
   u16         Arfcn    ;   // Arfcn as given in Broadcasted Sysinfos
   u8          SynchErr ;   // Counter of successive Synchro error
   u8          Status   ;   // Asleep, Bsic Known, Arfcn Removed
   u8          AsleepRla; //Rla when Synchro failed last time
   rr_Cell_t * Cell     ;   // Ptr to a cell object

} rri_BaElem_t;


/*
** Elem Bsic Description list 
*/
typedef struct
{
   u8              Bsic   ;   // Bsic given by the network
   u8              IdxBa  ;   // Index in an table of Ba Elem

} rri_BsicElem_t;

/*
** List of Bsics  ( use is temporary when receiving Instances of Msgs ) 
*/
typedef struct
{
   u8              Nb  ;  // Number of Bsic in the list   
   rri_BsicElem_t *Elem;

} rri_BsicList_t;


/*
** Elem Rtd  Description list 
*/
typedef struct
{
   u16             Rtd    ;   // Bsic given by the network
   u8              IdxBa  ;   // Index in an table of Ba Elem
} rri_RtdElem_t;


/*
** List of Rtd  ( use is temporary when receiving Instances of Msgs ) 
*/
typedef struct
{
   u8              Nb  ;  // Number of Bsic in the list   
   rri_RtdElem_t * Elem;

} rri_RtdList_t;


/*
** List of RepPrio( use is temporary when receiving Instances of Msgs ) 
*/
typedef struct
{
   u8     Nb      ;  // Number Rep Prio in the Table 
   bool * Elem    ;  // Report Prio Ptr

} rri_PrioList_t;

/*
** Elem of a Gsm Neighbour Cell element list
*/
typedef struct 
{
   u8              Bsic   ;   // Bsic given by the network
   u8              IdxBa  ;   // Index in an table of Ba Elem
//   u8              Rtd    ;   // Real Time Difference
   bool            RepPrio;   // Report Priority
   rr_ReselPar_t * ReselP ;   // Ptr to Resel Param (given in PSI3s / PMOs)

} rri_GsmNCellElem_t;

/*
** Elem of a Add NC Freq element list
*/
typedef struct 
{
   u8              Bsic   ;   // Bsic given by the network
   u16             Arfcn  ;   // Arfcn given by the network   
   rr_ReselPar_t * ReselP ;   // Ptr to Resel Param (given in PSI3s / PMOs)

} rri_NCellParam_t ;

/*
** NC Freq list
*/
typedef struct 
{
   u8                 Nb   ;  // Number of NC Freq in the list   
   rri_NCellParam_t * Elem ;  // Added Neigh Cell Param

} rri_NcFrqList_t;

typedef rri_NcFrqList_t rri_NCellParList_t;




typedef struct 
{
   u8   Nb   ; 
   u8 * IdxL ; 
} rri_RmvNcFrqList_t ;

/*
** Elem of a Enhance report Cell                    
*/
typedef struct 
{
   u8              Idx    ;   // Index of teh Cell in the GsmNCellL
   rr_Cell_t *     Cell   ;   // Pointer to the cell

}   rr_EnhRep_t ;

/*
** Elem of a MeasReport for a specific Band 
*/
typedef struct 
{
   u8   MonIdx[L1_SIZ_MON] ;  // Table of Indexes to the MonList
   u8   MonNb              ;  // Number of Indexes

} rri_BandDistrb_t;


#define RRI_MAX_BANDREPORT 20  
typedef struct 
{
   rr_EnhRep_t * CellL  ;  // Pointer to the cell
   u8            CellNb ;  // Number of Cells  

} rri_BandReport_t;





typedef struct {
   u8          ChgeMark                ; 
   u8          NcOrder                 ; 
   u8          NcNonDrxPer             ; 
   u8          NcRepPerI               ; 
   u8          NcRepPerT               ; 
   u8          RepType                 ; 
   u8          RepRate                 ; 
   u8          InvBsicRep              ; 
   u8          MBReport                ; 
   u8          SBReport                ; 
   u8          ScaleOrd                ; 
   u8          PmoInd                  ; 
                                         
   bool        NewFrqL                 ; 
   rr_OffThr_t RepOffThr[RR_LAST_BAND ]; 

} rri_DynMeasPar_t ;

#endif

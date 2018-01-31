/*******************************************************************************
  User defined includes
*******************************************************************************/
#ifndef __ED_USER_H
#define __ED_USER_H

/* include here your own .h files */
#include "sxs_type.h"
#include "sxs_io.h"

#ifdef __RR_DEC_C__
#define DefExtern
#else
#define DefExtern extern
#endif

/*
** Used to know if the dynamic allocation comes from a
** Packet Uplink Assignment or from a Packet Timeslot Reconfigure
*/
DefExtern bool IsPUlAss;

extern u8 LenMsRACap2;

/*
** TS 04.60, 11.2.9b.2.a table
** used for the decoding of the 3GNeighbourCellDescription
*/
DefExtern const u8 ED_RR_P[32]
#ifdef __SAP_USR_C__
 = {   0, 10, 19, 28, 36, 44, 52, 60, 67, 74, 81, 88, 95, 102, 109, 116, 
     122,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0 }
#endif
;

/*
** TS 04.60, 11.2.9b.2.b table
** used for the decoding of the 3GNeighbourCellDescription
*/
DefExtern const u8 ED_RR_Q[32]
#ifdef __SAP_USR_C__
 = {   0,   9,  17,  25,  32, 39, 46, 53, 59, 65, 71, 77, 83, 89, 95, 101, 
     106, 111, 116, 121, 126,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0 }
#endif
;


#define ED_SIGNAL_ERROR(errtext) SXS_TRACE( _SXR|TLEVEL(2), errtext );

#define EDAlloc sxr_Malloc
#define EDFree  sxr_Free

#undef DefExtern

/*---------------------------- definitions for RR decoder ------------------*/
#include "ed_rr.h"

/*
** RR TAG identifiers
*/
#define ED_TAG_RRRTD_TYPE   0
#define ED_TAG_RRUTRAN_TYPE 1
#define ED_TAG_RRLEN1       2
#define ED_TAG_RRLEN2       3
#define ED_TAG_RRLEN3       4
#define ED_TAG_RRLEN4       5
#define ED_TAG_RRLEN5       6
#define ED_TAG_RRLEN6       7
 
/*
** TAG Values for ED_TAG_RRRTD_TYPE
*/
#define ED_RR_RTD6      0
#define ED_RR_RTD12     1

/*
** TAG Values for ED_TAG_RRUTRAN_TYPE
*/
#define ED_RR_FDD       0
#define ED_RR_TDD       1
 

/*---------------------------- definitions for RR decoder ------------------*/
/*
** SS TAG identifiers
*/
#define ED_TAG_SS_IMPL     0
#define ED_TAG_SS_FACILITY 1
#define ED_TAG_SS3   2
#define ED_TAG_SS4   3
#define ED_TAG_SS5   4
#define ED_TAG_SS6   5

#endif

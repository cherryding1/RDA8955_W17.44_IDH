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
  File       : ed_rr.h 
--------------------------------------------------------------------------------

  Scope      : variables shared between ED and RR

  History    :
--------------------------------------------------------------------------------
  Aug 09 05  |  MCE   | Added RR_GPRS_SUSP_REQ
  Aug 12 04  |  OTH   | Creation            
================================================================================
*/
#ifndef __ED_RR_H__
#define __ED_RR_H__
#include "cmn_dec.h"

#define RR_PAG_REQ1        0x21    // Paging request type 1           
#define RR_PAG_REQ2        0x22    // Paging request type 2           
#define RR_PAG_REQ3        0x24    // Paging request type 3           
                                 
#define RR_SYS_INF1        0x19    // System information type 1       
#define RR_SYS_INF2        0x1a    // System information type 2       
#define RR_SYS_INF2B       0x02    // System information type 2 bis   
#define RR_SYS_INF2T       0x03    // System information type 2 ter   
#define RR_SYS_INF2Q       0x07    // System information type 2 quarter
#define RR_SYS_INF3        0x1b    // System information type 3       
#define RR_SYS_INF4        0x1c    // System information type 4       
#define RR_SYS_INF5        0x1d    // System information type 5       
#define RR_SYS_INF5B       0x05    // System information type 5 bis   
#define RR_SYS_INF5T       0x06    // System information type 5 ter   
#define RR_SYS_INF6        0x1e    // System information type 6       
#define RR_SYS_INF7        0x1f    // System information type 7       
#define RR_SYS_INF8        0x18    // System information type 8       
#define RR_SYS_INF13       0x00    // System information type 13      

#define RR_MEAS_INFO       0x03    // Short message type
#define RR_MEAS_REP        0x15    // Measurment Report               
#define RR_EXT_MEAS_ORD    0x37    // Extended Measurt Order          
#define RR_EXT_MEAS_REP    0x36    // Extended Measurt Order          

#define RR_INIT_REQ        0x3C
#define RR_IMM_ASS_CMD     0x3F
#define RR_IMM_ASS_EXT_CMD 0x39  
#define RR_IMM_ASS_REJ     0x3A  

#define RR_CIP_MOD_CMD     0x35
#define RR_CIP_MOD_CMP     0x32

#define RR_ASS_CMD         0x2E     
#define RR_ASS_CMP         0x29
#define RR_ASS_FAIL        0x2F
#define RR_HOV_CMD         0x2B
#define RR_HOV_CMP         0x2C
#define RR_HOV_FAIL        0x28
#define RR_PHY_INFO        0x2D

#define RR_CHAN_REL        0x0D

#define RR_STATUS          0x12
#define RR_CHAN_MOD        0x10
#define RR_CHAN_MOD_ACK    0x17
#define RR_FRQ_RDF         0x14
#define RR_CMK_CHG         0x16
#define RR_CMK_ENQ         0x13

#define RR_GPRS_SUSP_REQ   0x34

// TEST MODE MESSAGE TYPES
#define TST_CLOSE_TCH_LOOP_CMD 0x00
#define TST_CLOSE_TCH_LOOP_ACK 0x01
#define TST_OPEN_TCH_LOOP_CMD  0x06
#define TST_INTERFACE          0x14

/*
** GPRS MESSAGES : 04.60 section 11.2.0.1
*/
#define RR_PSYS_INF1       0x31    // Packet System Information 1
#define RR_PSYS_INF2       0x32    // Packet System Information 2
#define RR_PSYS_INF3       0x33    // Packet System Information 3
#define RR_PSYS_INF3B      0x34    // Packet System Information 3 bis
#define RR_PSYS_INF4       0x35    // Packet System Information 4
#define RR_PSYS_INF5       0x36    // Packet System Information 5
#define RR_PSYS_INF13      0x37    // Packet System Information 13
#define RR_PSYS_INF3T      0x3C    // Packet System Information 3 ter

#define RR_PCO             0x01    // Packet Cell Change Order
#define RR_PMO             0x03    // Packet Measurement Order

/*
** EnDec context
*/
typedef struct
{
 u8 MT ; // L3 Radio Message Type
} rr_EdCtx_t ;

#ifdef __RR_C__
#define DefExtern 
#else
#define DefExtern extern
#endif

DefExtern rr_EdCtx_t    rr_EdCtx ;
#undef DefExtern

#endif	


//------------------------------------------------------------------------------
//  $Log: mac_func.h,v $
//  Revision 1.4  2006/05/22 14:36:19  oth
//  Update the copyright
//
//  Revision 1.3  2006/02/08 08:44:18  oth
//  Remove the PCCO special case
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
  File       : mac_func.h
--------------------------------------------------------------------------------

  Scope      : Header file for mac functions

  History    :
--------------------------------------------------------------------------------
  Oct 14 03  |  OTH   | Creation            
================================================================================
*/
#ifndef __MAC_FUNC_H__
#define __MAC_FUNC_H__

#include "sxs_type.h"
#include "ed_lib.h"

typedef struct
{
 u32 EmissionDate;
 u8  FormatSce;
// Format
#define MAC_SCE_POLLING_ACK    0
#define MAC_SCE_POLLING_NORMAL 1
#define MAC_SCE_MAIN_CELL      2
 u8  Info;
// RespType (LSB)
#define MAC_PL_OTHER    0
#define MAC_PL_10       1
// SegmStatus (MSB)
#define MAC_SEG_CMPL    0
#define MAC_SEG_LOST    1
#define MAC_SEG_RBSN0   2
 u8  TN;
} mac_PCtrlAck_t;

/*
** from mac_tool.c
*/
u8 mac_FdCtrlAck( void* CurData, void* QueueData );
#define MAC_8_BITS_FORMAT      0
#define MAC_11_BITS_FORMAT     1
#define MAC_NORMAL_FORMAT      2
Msg_t* mac_PrepPCtrlAck     ( mac_PCtrlAck_t * );
u8     mac_FdRefQ           ( void* RefData, void* QueueData );
s16    mac_HdleSegmentation ( s16, mac_CtrlQ_t* );
void   mac_ContResSuccess   ( void );
void   mac_ContResFailed    ( void );
void   mac_PurgeCtrlQ       ( void );
#define MAC_PS_DATA 0
#define MAC_PDATA   1
#define MAC_PCTRL   2
void   mac_SdBlock          ( Msg_t *, u8, u8 );
bool   mac_IsRejForUs       ( c_Reject_struct * );
void   mac_FinishEncBlock   ( u8 *, u16 );
long   ENCODE_c_PDownlinkAckNack( char* ED_CONST, ED_CONST long,
                                  const c_PDownlinkAckNack* ED_CONST );
#ifdef __EGPRS__
s16 ENCODE_c_EGPRS_Packet_Downlink_AckNack_Msg( u8 * Buffer, const u32 BitOffset,
											   const c_PDownlinkAckNack* Source );
#endif
/*
** from mac_in.c
*/
void mac_L1PDchInd    ( void );
void mac_L1PBcchInd   ( void );
void mac_L1ChnQualInd ( void );

/*
** from mac_ctrl.c
*/
void mac_CtrlBlock    ( void );
void mac_NewTlliInd   ( void );
void mac_NewTlliCnf   ( void );
void mac_T3200Exp     ( void );

/*
** from mac_data.c
*/
void mac_DataBlock    ( void );
void mac_PurgeDQReq   ( void );

/*
** from mac_out.c
*/
void mac_RrDataReq  ( void );
void mac_UlDataReq  ( void );
void mac_DlAckReq   ( void );
#endif

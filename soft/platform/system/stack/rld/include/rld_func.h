//------------------------------------------------------------------------------
//  $Log: rld_func.h,v $
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

  Copyright StackCom (c) 2003

================================================================================
*/
/*
================================================================================
  File       : rld_func.h
--------------------------------------------------------------------------------

  Scope      : Protoypes of rld functions

  History    :
--------------------------------------------------------------------------------
  Mar 04 04  |  OTH   | Creation 
================================================================================
*/

#ifndef __RLD_FUNC_H__
#define __RLD_FUNC_H__

#include "sxs_type.h"    // System includes

#include "rld_msg.h"
#include "itf_msg.h"
#include "rld_data.h"
/*
================================================================================
  Common prototypes for RLD
--------------------------------------------------------------------------------

  Scope      : Functions used by core RLD task

================================================================================
*/

//           rld_evt.c
InEvtDsc_t * rld_GiveDsc ( u32 ) ;

//           rld_out.c
void         rld_PrepMsg ( OutEvtDsc_t *, u16 ) ;
void         rld_SendMsg ( u8 ) ;

//           rld_trsf.c
void         rld_ConInd  ( void ) ;
#ifdef __EGPRS__
void         egprs_rld_DataInd ( void ) ;
#else
void         rld_DataInd ( void ) ;
#endif
void         rld_RelInd  ( void ) ;
void         rld_TestModeSet(void);
/*#Added by PENGQIANG 2007.3.19 for testmode begin.*/
#ifdef __EGPRS__
void         rld_LoopBackCmd ( void );
#endif
/*#Added by PENGQIANG 2007.3.19 for testmode end.*/

//           rld_tool.c
#ifdef __EGPRS__
void        egprs_rld_BuildAndFwdSdu();//(str_PDU_REC *LastPdu);ss
u8          get_Padding_accord_CPS_MCS(u8 Cps, u8 Mcs);

u8          RB_limit_Compression( u8 StartColour,u16 len_max, u8 *p_In, u16 len_In, u8 *p_out, u16 *len_Out, u16 * len_read);
u8          rld_FindSplitPdu(u16* BSN, SPLIT_TMP_DATA *SplitPdu);
#endif
void         rld_BuildAndFwdSdu( Msg_t * );

#endif

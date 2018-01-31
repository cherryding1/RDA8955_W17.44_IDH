//------------------------------------------------------------------------------
//  $Log: rrp_data.h,v $
//  Revision 1.8  2006/05/22 14:39:08  oth
//  Update the copyright
//
//  Revision 1.7  2006/05/11 13:03:49  oth
//  The rrp_HdleFreqParam is now returning an error index to determine if the frequencies are not in one band.
//
//  Revision 1.6  2006/03/27 08:16:45  oth
//  Single block duration timer has been doubled in case RRI receives a control block in 2 parts
//
//  Revision 1.5  2006/02/27 10:33:04  oth
//  - Increase T3176 duration to 15 sec. to be compliant with 51.10
//  - Increase SEG_IMMASS timer to be able receive the 2nd part of the Imm. Ass on the 51st block
//
//  Revision 1.4  2006/02/15 08:13:27  oth
//  Change the name of the Single block duration define
//
//  Revision 1.3  2006/02/08 08:50:47  oth
//  Addition for the handling of the PCCO
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
  File       : rrp_data.h
--------------------------------------------------------------------------------

  Scope      : General context for the GPRS MAC

  History    :
--------------------------------------------------------------------------------
  Oct 13 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __RRP_DATA_H__
#define __RRP_DATA_H__

// System includes
#include "sxs_type.h"

// Interface includes
#include "itf_rr.h"

// EnDec includes

// MAC internal includes

/*
** Save Message macro
*/
#define RRP_SAVE_MSG( _Msg )                    \
  fsm_Save( pRRCtx->Fsm, TRUE, _Msg );

/*
** Timer duration
*/
#define RRP_T3162_DURATION     ( 5 SECONDS )
#define RRP_T3164_DURATION     ( 5 SECONDS )
#define RRP_T3174_DURATION     ( 15 SECONDS )
#define RRP_T3176_DURATION     ( 15 SECONDS )
#define RRP_T3180_DURATION     ( 5 SECONDS )
#define RRP_T3184_DURATION     ( 5 SECONDS )
#define RRP_T3186_DURATION     ( 5 SECONDS )
#define RRP_T3188_DURATION     ( 5 SECONDS )
#define RRP_T3190_DURATION     ( 5 SECONDS )
#define RRP_SEGIMMASS_DURATION ( (51 * 2 + 2) GSM_FRAMES )
//#define RRP_SGL_DURATION       ( 50 MILLI_SECONDS )
// For 20.22.26
#define RRP_SGL_DURATION       ( 100 MILLI_SECONDS )

/*
** Abnormal releases
*/
#define RRP_ABREL_WAR       RRP_REL_ABWAR       /* 1 in itf_rr.h */
#define RRP_ABREL_CELLRESEL RRP_REL_ABCELLRESEL /* 2 in itf_rr.h */
#define RRP_ABREL_NOAR      3
#define RRP_ABREL_SYSINFO   4 

/*
** Access Attempt Threshold
*/
#define RRP_ACC_ATT_T3164 3
#define RRP_ACC_ATT_REG   6 //4

/*
** Radio priority for the access in case of Packet Measurement
*/
#define RRP_MEAS_RP 0




#define RRP_SGLE_WNO_TBF ( RRP_MM_PROCEDURE + 1 )




#define RRP_GET_MULTISLOTCL psap_StackCfg->MSClass

/*
** Channel Request Description in PResReq or PDlAck/Nack threshold
*/
#define RRP_MAX_ULESTAB 4

/*
** Structure for the Fixed allocation bitmap management
*/
typedef struct
{
 bool BlockPeriod;              /* TRUE if Block periods encoding */
 u8   OrigLen;
 u8   TsBitmap;
 u8   NbTs;
 u8*  Orig;
 u8*  Dest;
} rrp_FixedBitmap_t;




#define RRP_FREQ_OK           0
#define RRP_FREQ_NOT_ONE_BAND 1
#define RRP_FREQ_OTHER_PB     2

#endif // __RRP_DATA_H__

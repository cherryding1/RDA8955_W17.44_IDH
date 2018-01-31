//------------------------------------------------------------------------------
//  $Log: mm_intra.h,v $
//  Revision 1.3  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------


/*
================================================================================
  File       : MM_INTRA.H
--------------------------------------------------------------------------------

  Scope      : Internal event for MM
  History    :
--------------------------------------------------------------------------------
  Aug 12 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __MM_INTRA_H__
#define __MM_INTRA_H__
 
#include "sxs_type.h"
#include "sxs_cfg.h"
#include "itf_msgc.h"

/*
** MM Internal msg Start at INTRABASE + 0 
*/
#define MMC_INTRA_BASE ( HVY_MM + 0x8000 )
#define MMP_INTRA_BASE ( HVY_MM + 0xA000 )

/*
** Primitive intra MM timer ID definitions
*/
#define MMC_T3210_EXP  ( (MMC_INTRA_BASE + 1) | SXS_TIMER_EVT_ID )
#define MMC_T3211_EXP  ( (MMC_INTRA_BASE + 2) | SXS_TIMER_EVT_ID )
#define MMC_T3212_EXP  ( (MMC_INTRA_BASE + 3) | SXS_TIMER_EVT_ID )
#define MMC_T3213_EXP  ( (MMC_INTRA_BASE + 4) | SXS_TIMER_EVT_ID )
#define MMC_T3230_EXP  ( (MMC_INTRA_BASE + 5) | SXS_TIMER_EVT_ID )
/* T3220 & T3240 */
#define MMC_TWNKC_EXP  ( (MMC_INTRA_BASE + 6) | SXS_TIMER_EVT_ID ) 
#define MMC_DETACH_EXP ( (MMC_INTRA_BASE + 7) | SXS_TIMER_EVT_ID )

/*
** Primitive intra MM Radio message definitions
*/
#define MMC_ID_REQ       ( MMC_INTRA_BASE + 8  )
#define MMC_LOCUP_ACC    ( MMC_INTRA_BASE + 9  )
#define MMC_LOCUP_REJ    ( MMC_INTRA_BASE + 10 )
#define MMC_AUTH_REQ     ( MMC_INTRA_BASE + 11 )
#define MMC_AUTH_REJ     ( MMC_INTRA_BASE + 12 )
#define MMC_CMSERV_ACC   ( MMC_INTRA_BASE + 13 )
#define MMC_CMSERV_REJ   ( MMC_INTRA_BASE + 14 )
#define MMC_TMSIREAL_CMD ( MMC_INTRA_BASE + 15 )
#define MMC_ABORT        ( MMC_INTRA_BASE + 16 )
#define MMC_HDL_OTHER_PD ( MMC_INTRA_BASE + 17 )
#define MMC_MM_INFO      ( MMC_INTRA_BASE + 18 )

/*
** MM send messages to itself
*/
#define MMC_SELF_LOCUP   ( MMC_INTRA_BASE + 19 )
#define MMC_SELF_PAGING  ( MMC_INTRA_BASE + 20 )

typedef struct
{
 u8 LocUpType;
} mmc_SelfLocup_t;

typedef struct
{
  u8  IdType;
  u8  CN;
} mmc_SelfPaging_t;



//------------------------------------------------
//              GPRS internal events
//------------------------------------------------
typedef struct
{
  u8  IdType;
} mmp_SelfPaging_t;


#define MMP_TIMER_BASE ( MMP_INTRA_BASE + 0x80 )

enum
{
//================================================
// MESSAGES IDs
//================================================

  /*
  ** Primitive intra MMP Radio message definitions
  */
  MMP_ATT_ACC         = ( MMP_INTRA_BASE + 0x00 ),
  MMP_ATT_REJ         ,
  MMP_ID_REQ          ,
  MMP_DETACHMT_REQ    ,
  MMP_AUTHCIPH_REQ    ,
  MMP_AUTHCIPH_REJ    ,
  MMP_RAU_ACC         ,
  MMP_RAU_REJ         ,
  MMP_TMSIREALLOC_CMD ,
  MMP_DETACH_ACC      ,
  MMP_GMM_INFO        ,
  MMP_GMM_STATUS      ,
  MMP_HDL_OTHER_PD    ,
  MMP_TESTMODE_CMD    ,
#ifdef __EGPRS__
  MMP_ETESTMODE_CMD   ,
#endif
/*
** MM send messages to itself
*/
  MMP_SELF_PAGING     ,
  MMP_ABORT_PROC      ,
  MMP_RESTART_PROC    ,
  MMP_GPRS_PENDING    ,

//================================================
// TIMERS IDs
//================================================

  MMP_FIRST_TIM       = (MMP_TIMER_BASE | SXS_TIMER_EVT_ID ),
  MMP_T3302_EXP       = MMP_FIRST_TIM,
  MMP_T3310_EXP,      // GPRS ATTACH proc ongoing
  MMP_T3311_EXP,
  MMP_T3312_EXP,
  MMP_T3314_EXP,      // READY timer
  MMP_T3321_EXP,      // GPRS DETACH proc ongoing
  MMP_T3330_EXP,      // RAU proc ongoing
//  MMP_T3316_EXP,      // UMTS Only
//  MMP_T3318_EXP,      // UMTS Only
//  MMP_T3320_EXP,      // UMTS Only
  MMP_LAST_TIM        // INSERT NEW ELEMENT BEFORE

};

#define MMP_NB_TIM  (MMP_LAST_TIM - MMP_FIRST_TIM)




#if 0
/*
** Primitive intra MMP Radio message definitions
*/
#define MMP_ATT_ACC           ( MMP_INTRA_BASE + 0x01 )
#define MMP_ATT_REJ           ( MMP_INTRA_BASE + 0x02 )
#define MMP_ID_REQ            ( MMP_INTRA_BASE + 0x03 )
#define MMP_DETACHMT_REQ      ( MMP_INTRA_BASE + 0x04 )
#define MMP_AUTHCIPH_REQ      ( MMP_INTRA_BASE + 0x05 )
#define MMP_AUTHCIPH_REJ      ( MMP_INTRA_BASE + 0x06 )
#define MMP_RAU_ACC           ( MMP_INTRA_BASE + 0x07 )
#define MMP_RAU_REJ           ( MMP_INTRA_BASE + 0x08 )
#define MMP_TMSIREALLOC_CMD   ( MMP_INTRA_BASE + 0x09 )
#define MMP_DETACH_ACC        ( MMP_INTRA_BASE + 0x0A )
#define MMP_GMM_INFO          ( MMP_INTRA_BASE + 0x0B )

#define MMP_HDL_OTHER_PD      ( MMP_INTRA_BASE + 0x0C )
#define MMP_TESTMODE_CMD      ( MMP_INTRA_BASE + 0x0D )

/*
** MM send messages to itself
*/
#define MMP_SELF_PAGING       ( MMP_INTRA_BASE + 0x10 )
#define MMP_ABORT_PROC        ( MMP_INTRA_BASE + 0x11 )
#define MMP_RESTART_PROC      ( MMP_INTRA_BASE + 0x12 )
//#define MMP_SELF_ATTACH       ( MMP_INTRA_BASE + 0x13 )


/*
** Primitive intra MMP timer ID definitions
*/

#define MMP_FIRST_TIM     ( (MMP_TIMER_BASE + 0x00) | SXS_TIMER_EVT_ID )

enum
{
  MMP_T3302_EXP = MMP_FIRST_TIM,
  MMP_T3310_EXP,                // GPRS ATTACH proc ongoing
  MMP_T3311_EXP,
  MMP_T3312_EXP,
  MMP_T3314_EXP,                // READY timer
  MMP_T3321_EXP,                // GPRS DETACH proc ongoing
  MMP_T3330_EXP,                // RAU proc ongoing
//  MMP_T3316_EXP,                // UMTS Only
//  MMP_T3318_EXP,                // UMTS Only
//  MMP_T3320_EXP,                // UMTS Only
  MMP_LAST_TIM    // INSERT NEW ELEMENT BEFORE
};

#define MMP_NB_TIM  (MMP_LAST_TIM - MMP_FIRST_TIM)
#endif


#endif  // __MM_INTRA_H__

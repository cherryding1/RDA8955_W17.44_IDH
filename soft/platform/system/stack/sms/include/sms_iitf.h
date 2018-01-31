//------------------------------------------------------------------------------
//  $Log: sms_iitf.h,v $
//  Revision 1.4  2006/05/22 07:56:29  aba
//  Copyrigth is updated
//
//  Revision 1.3  2006/01/24 15:42:14  aba
//  Changing of DefExtern into extern for the event definition
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : SMS_IITF.H
--------------------------------------------------------------------------------

  Scope      : Defintion of SMS internal events

  History    :
--------------------------------------------------------------------------------
  Feb 17 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SMS_IITF_H__
#define __SMS_IITF_H__

#include "itf_sms.h"

#define SMR_TR1M_EXP           (( SMS_INTRA_BASE + 0  ) | SXS_TIMER_EVT_ID )
#define SMR_TR2M_EXP           (( SMS_INTRA_BASE + 1  ) | SXS_TIMER_EVT_ID )
#define SMC_TC1M_EXP_MO        (( SMS_INTRA_BASE + 2  ) | SXS_TIMER_EVT_ID )
#define SMC_TC1M_EXP_MT        (( SMS_INTRA_BASE + 3  ) | SXS_TIMER_EVT_ID )
#define SMR_TRAM_EXP           (( SMS_INTRA_BASE + 4  ) | SXS_TIMER_EVT_ID )
#define SMR_TR1M_RE_EXP_MO     (( SMS_INTRA_BASE + 5  ) | SXS_TIMER_EVT_ID )  // SMR_RESEND_DELAY
#define SMC_ABORT_REQ           ( SMS_INTRA_BASE + 5  )
#define SMC_DATA_REQ            ( SMS_INTRA_BASE + 6  )
#define SMC_ESTAB_REQ           ( SMS_INTRA_BASE + 7  )
#define SMC_RELEASE_REQ         ( SMS_INTRA_BASE + 8  )
#define SMC_DATA_IND            ( SMS_INTRA_BASE + 9  )
#define SMC_ESTAB_IND           ( SMS_INTRA_BASE + 10 )
#define SMC_ERROR_IND           ( SMS_INTRA_BASE + 11 )
#define SMC_CP_DATA             ( SMS_INTRA_BASE + 12 )
#define SMS_CP_ACK              ( SMS_INTRA_BASE + 13 )
#define SMC_CP_ERROR            ( SMS_INTRA_BASE + 14 )
#define SMC_ERROR_97            ( SMS_INTRA_BASE + 15 )
#define SMR_RP_DATA             ( SMS_INTRA_BASE + 16 )
#define SMR_RP_ACK              ( SMS_INTRA_BASE + 17 )
#define SMR_RP_ERROR            ( SMS_INTRA_BASE + 18 )
#define SMR_ERROR_95            ( SMS_INTRA_BASE + 19 )
#define SMR_ERROR_97            ( SMS_INTRA_BASE + 20 )
#define SMC_ESTAB_ERR           ( SMS_INTRA_BASE + 21 )
#define SMR_SELF_RESEND         ( SMS_INTRA_BASE + 22 )

/*
================================================================================
  Structure  : smc_DataReq_t
--------------------------------
  Direction  : SMS (SMR) -> SMS (SMC)
  Scope      : SMR asks for the sending of a RPDU on the established CM connection
  Comment    : InterLayer message, no specific parameter
================================================================================
*/

/*
================================================================================
  Structure  : smc_EstabReq_t
--------------------------------
  Direction  : SMS (SMR) -> SMS (SMC)
  Scope      : SMR asks for the sending of a RPDU on the established CM connection
  Comment    : InterLayer message, no specific parameter
================================================================================
*/

/*
================================================================================
  Structure  : smc_ReleaseReq_t
--------------------------------
  Direction  : SMS (SMR) -> SMS (SMC)
  Scope      : SMR ask for the release of the CM connection
  Comment    :
================================================================================
*/
typedef struct
{
  u8  Cause;
}smc_ReleaseReq_t;

#endif

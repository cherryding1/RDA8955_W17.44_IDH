//------------------------------------------------------------------------------
//  $Log: itf_lap.h,v $
//  Revision 1.2  2005/12/29 17:38:44  pca
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
  File       : ITF_LAP.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by LAPDm
  History    :
--------------------------------------------------------------------------------
  Aug 11 03  |  MCE   | Creation            
================================================================================
*/

#ifndef __ITF_LAP_H__
#define __ITF_LAP_H__

#include "sxs_type.h"
#include "cmn_defs.h"

/*
** LAPDm Internal msg Start at INTRABASE + 0
*/
#define LAP_INTRA_BASE ( HVY_LAP + 0x8000 )

/*
** Primitive ID definitions
*/
#define LAP_ESTABLISH_REQ               ( HVY_LAP + 1  )
#define LAP_ESTABLISH_CNF               ( HVY_LAP + 2  )
#define LAP_ESTABLISH_IND               ( HVY_LAP + 3  )
#define LAP_ESTABLISH_FAILURE_IND       ( HVY_LAP + 4  )
#define LAP_SUSPEND_REQ                 ( HVY_LAP + 5  )
#define LAP_RESUME_REQ                  ( HVY_LAP + 6  )
#define LAP_RESUME_CNF                  ( HVY_LAP + 7  )
#define LAP_RESUME_FAILURE_IND          ( HVY_LAP + 8  )
#define LAP_RECONNECT_REQ               ( HVY_LAP + 9  )
#define LAP_RECONNECT_CNF               ( HVY_LAP + 10 )
#define LAP_RECONNECT_FAILURE_IND       ( HVY_LAP + 11 )
#define LAP_NORMAL_RELEASE_REQ          ( HVY_LAP + 12 )
#define LAP_NORMAL_RELEASE_CNF          ( HVY_LAP + 13 )
#define LAP_NORMAL_RELEASE_FAILURE_IND  ( HVY_LAP + 14 )
#define LAP_LOCAL_RELEASE_REQ           ( HVY_LAP + 15 )
#define LAP_LOCAL_RELEASE_CNF           ( HVY_LAP + 16 )
#define LAP_RELEASE_IND                 ( HVY_LAP + 17 )
#define LAP_DATA_REQ                    ( HVY_LAP + 18 )
#define LAP_UNIT_DATA_REQ               ( HVY_LAP + 19 )
#define LAP_DATA_IND                    ( HVY_LAP + 21 )
#define LAP_UNIT_DATA_IND               ( HVY_LAP + 22 )
#define LAP_ERROR_IND                   ( HVY_LAP + 23 )

/*
** Possible values for the SAPI field
*/
#define LAP_SAPI_0          0
#define LAP_SAPI_3          1

/*
** Possible values for the Chan field
*/
#define LAP_SDCCH           0
#define LAP_FACCH_HALF      1
#define LAP_FACCH_FULL      2
#define LAP_SACCH           3


/*
================================================================================
  Structure  : lap_EstablishReq_t
--------------------------------
  Direction  : RR -> LAP
  Scope      : RR asks for the establishment of the connected mode
  Comment    : InterLayer message
================================================================================
*/
/*
================================================================================
  Structure  : lap_ResumeReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : RR requests LAP to resume connected mode
  Comment    : InterLayer message
================================================================================
*/
typedef struct
{
  u8  Sapi ; // LAP_SAPI_0 or LAP_SAPI_3
  u8  Chan ; // LAP_SDCCH or LAP_FACCH_FULL or LAP_FACCH_HALF or LAP_SACCH
} lap_EstablishReq_t,
  lap_ResumeReq_t;

/*
================================================================================
  Structure  : lap_UnitDataReq_t
--------------------------------
  Direction  : RR -> LAP
  Scope      : RR asks for the transmission of l3 data in unacknowledged mode
  Comment    : InterLayer message
================================================================================
*/
/*
================================================================================
  Structure  : lap_DataReq_t
--------------------------------
  Direction  : RR -> LAP
  Scope      : RR asks for the transmission of l3 data in acknowledged mode
  Comment    : InterLayer message
================================================================================
*/
/*
================================================================================
  Structure  : lap_DataInd_t
--------------------------------
  Direction  : LAP -> RR
  Scope      : LAP indicates the reception of l3 data in acknowledged mode
  Comment    : InterLayer message
================================================================================
*/
typedef struct
{
  u8  Sapi ; // LAP_SAPI_0 or LAP_SAPI_3
} lap_DataReq_t,
  lap_UnitDataReq_t,
  lap_DataInd_t ;

/*
================================================================================
  Structure  : lap_UnitDataInd_t
--------------------------------
  Direction  : LAP -> RR
  Scope      : LAP indicates the reception of l3 data
  Comment    : InterLayer message
================================================================================
*/
typedef struct
{
  u8  Sapi ; // LAP_SAPI_0 or LAP_SAPI_3
  u8  Chan ;
} lap_UnitDataInd_t;

/*
================================================================================
  Structure  : lap_EstablishCnf_t
--------------------------------------------------------------------------------
  Direction  : LAP -> RR
  Scope      : LAP indicates the successful establishment of connected mode
  Comment    :
================================================================================
*/
typedef struct
{
  u8  Sapi ; // LAP_SAPI_0 or LAP_SAPI_3
} lap_EstablishCnf_t;

/*
================================================================================
  Structure  : lap_EstablishInd_t
--------------------------------------------------------------------------------
  Direction  : LAP -> RR
  Scope      : LAP indicates the successful establishment of connected mode
               triggered by the network
  Comment    : used for SAPI 3 only
  Comment    : no body
================================================================================
*/

/*
================================================================================
  Structure  : lap_EstablishFailureInd_t
--------------------------------------------------------------------------------
  Direction  : LAP -> RR
  Scope      : LAP indicates the failure of establishment
================================================================================
*/
#define  LAP_CONT_ERR                                       1
#define  LAP_ESTAB_FAILURE_CAUSE_DISC_RECEIVED              2
#define  LAP_ESTAB_FAILURE_CAUSE_DM_RECEIVED                3
#define  LAP_ESTAB_FAILURE_CAUSE_UA_WITH_F_0                4
#define  LAP_ESTAB_FAILURE_CAUSE_N200                       5
typedef struct
{
  u8  Sapi ;          // LAP_SAPI_0 or LAP_SAPI_3
  u8  Cause;
} lap_EstablishFailureInd_t ;

/*
================================================================================
  Structure  : lap_SuspendReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : RR asks LAP to suspend the SAPI 0
  Comment    : used for SAPI 0 only
  Comment    :
================================================================================
*/
typedef struct
{
  u8  Sapi ;
} lap_SuspendReq_t ;


/*
================================================================================
  Structure  : lap_ResumeCnf_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : LAP successfully answers to the RR resume request
  Comment    : used for SAPI 0 only
  Comment    : no body
================================================================================
*/


/*
================================================================================
  Structure  : lap_ResumeFailureInd_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : LAP indicates a resume failure to RR
  Comment    : used for SAPI 0 only
================================================================================
*/
#define  LAP_RESUME_FAILURE_CAUSE_DISC_RECEIVED              2
#define  LAP_RESUME_FAILURE_CAUSE_DM_RECEIVED                3
#define  LAP_RESUME_FAILURE_CAUSE_UA_WITH_F_0                4
#define  LAP_RESUME_FAILURE_CAUSE_N200                       5
typedef struct
{
  u8  Cause;
} lap_ResumeFailureInd_t ;

/*
================================================================================
  Structure  : lap_ReconnectReq_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : RR requests LAP to reconnect the old lapdm
  Comment    : used for SAPI 0 only
  Comment    : InterLayer message
================================================================================
*/
typedef struct
{
  u8  Sapi ;
} lap_ReconnectReq_t ;


/*
================================================================================
  Structure  : lap_ReconnectCnf_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : LAP successfully answers to the RR reconnect request
  Comment    : used for SAPI 0 only
  Comment    : no body
================================================================================
*/


/*
================================================================================
  Structure  : lap_ReconnectFailureInd_t
--------------------------------------------------------------------------------
  Direction  : RR -> LAP
  Scope      : LAP successfully answers to the RR reconnect request
  Comment    : used for SAPI 0 only
================================================================================
*/
#define  LAP_RECONNECT_FAILURE_CAUSE_DISC_RECEIVED              2
#define  LAP_RECONNECT_FAILURE_CAUSE_DM_RECEIVED                3
#define  LAP_RECONNECT_FAILURE_CAUSE_UA_WITH_F_0                4
#define  LAP_RECONNECT_FAILURE_CAUSE_N200                       5
typedef struct
{
  u8  Cause;
} lap_ReconnectFailureInd_t ;


/*
================================================================================
  Structure  : lap_NormalReleaseReq_t
--------------------------------
  Direction  : RR -> LAP
  Scope      : RR asks for a normal release of the acknowledged mode. A normal
               release implies the sending of a DISC frame to the peer lapdm
================================================================================
*/
typedef struct
{
  u8  Sapi ;
} lap_NormalReleaseReq_t ;

/*
================================================================================
  Structure  : lap_NormalReleaseCnf_t
--------------------------------
  Direction  : LAP -> RR
  Scope      : LAP successfully answers to the RR normal release request
================================================================================
*/
typedef struct
{
  u8  Sapi ;
} lap_NormalReleaseCnf_t ;

/*
================================================================================
  Structure  : lap_NormalReleaseFailureInd_t
--------------------------------
  Direction  : LAP->RR
  Scope      : Bad answer or no answer to the DISC frame
================================================================================
*/
#define LAP_NORMAL_RELEASE_FAILURE_CAUSE_N200              5
typedef struct
{
  u8  Sapi  ;
  u8  Cause ;
} lap_NormalReleaseFailureInd_t ;

/*
================================================================================
  Structure  : lap_LocalReleaseReq_t
--------------------------------
  Direction  : RR -> LAP
  Scope      : RR asks for the local release of the acknowledged mode. Local
               release means that the peer lapdm is not informed of the release
================================================================================
*/
typedef struct
{
  u8  Sapi ;
} lap_LocalReleaseReq_t ;


/*
================================================================================
  Structure  : lap_ReleaseInd_t
--------------------------------
  Direction  : LAP -> RR
  Scope      : LAP indicates the release of the connected mode.
================================================================================
*/
#define  LAP_RELEASE_CAUSE_DISC_RECEIVED 0
#define  LAP_RELEASE_CAUSE_DM_RECEIVED   1
#define  LAP_RELEASE_CAUSE_NR_SEQ_ERROR  2
#define  LAP_RELEASE_CAUSE_LAST_T200     3
typedef struct
{
  u8  Sapi ;
  u8  Cause;
} lap_ReleaseInd_t ;

/*
================================================================================
  Structure  : lap_GenericPartMsg_t
--------------------------------
  Direction  : RR -> LAP
  Scope      : Used for common Sapi identification

  SAPI must be the first parameter of all request messages
================================================================================
*/
typedef struct
{
  u8  Sapi ;
} lap_GenericPartMsg_t ;




#endif  // __ITF_LAP_H__

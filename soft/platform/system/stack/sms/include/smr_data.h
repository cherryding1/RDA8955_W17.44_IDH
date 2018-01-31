//------------------------------------------------------------------------------
//  $Log: smr_data.h,v $
//  Revision 1.4  2006/05/22 07:05:58  aba
//  Copyrigth is updated
//
//  Revision 1.3  2006/02/23 13:52:20  aba
//  Additional commentaries
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : smr_data.h
--------------------------------------------------------------------------------

  Scope      : Declaration of SMR context variables

  History    :
--------------------------------------------------------------------------------
  Feb 17 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SMR_DATA_H__
#define __SMR_DATA_H__

#include "fsm.h"
#include "sxr_cnf.h"
#include "sms_msg.h"


// Smr context (all global of the smr)
typedef struct
{
  u8     MsgRef;            // Message reference
  u8     MMARetrans;        // Counter for the retransmission of the memory available message
  u8     Retrans;			// Counter for the retransmission, not inclue first send // add 20160126 for SMR resend
  						     // only SMR_RESEND_COUNT_MAX set to 1 tested, if set to more than 1, need test in such case
  #define SMR_RESEND_COUNT_MAX 2
  u8	 toto;
  Msg_t*    savedInterMsg;		//  add 20160126 for SMR resend
} smr_MOData_t ;

typedef struct
{
  u8   MsgRef;              // Message reference
} smr_MTData_t ;


#endif // __SMR_DATA_H__

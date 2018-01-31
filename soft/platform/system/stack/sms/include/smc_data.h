//================================================================================
//  File       : smc_data.h
//--------------------------------------------------------------------------------
//  Scope      : Declaration of SMC context variables
//  History    :
//--------------------------------------------------------------------------------
//  2006/05/22  aba  Memory optimization
//  2005/12/29  pca  Log keyword added to all files
//  Feb 17 04  |  ABA   | Creation
//================================================================================

#ifndef __SMC_DATA_H__
#define __SMC_DATA_H__

#include "fsm.h"
#include "sxr_cnf.h"
#include "sms_msg.h"


// Smc context (all global of the smc)
typedef struct
{
  u8            TI                    ;  // Transaction identifier
  u8            CpDataRetrans         ;
  u8            TxCpDataMsgDataOffset ;  // Used for retransmission (DataOffset is stored because
                                         // it could be midified by lower levels)
  bool          reest;
  u16           TxCpDataMsgDataLen    ;  // Used for retransmission (DataLen is stored because
  Msg_t       * PtTxCpDataMsg         ;  // Used for retransmission
                                         // it could be midified by lower levels - not usefull)
  Msg_t       * PtStoredCpData        ;  // Used for concatenation
  
} smc_MOData_t ;

typedef struct
{
  u8            TI                    ;  // Transaction identifier
  u8            TxCpDataMsgDataOffset ;  // Used for retransmission (DataOffset is stored because
  u8            CpDataRetrans         ;
  u16           TxCpDataMsgDataLen    ;  // Used for retransmission (DataLen is stored because
                                         // it could be midified by lower levels - not usefull)
  Msg_t       * PtTxCpDataMsg         ;  // Used for retransmission
                                         // it could be midified by lower levels)
} smc_MTData_t ;

#endif // __SMC_DATA_H__

//================================================================================
//  File       : api_func.h
//--------------------------------------------------------------------------------
//  Scope      : prototypes of API functions (except transitions)
//  History    :
//--------------------------------------------------------------------------------
//  Mar 31 06  |  MCE   | Creation
//================================================================================

#ifndef __API_FUNC_H__
#define __API_FUNC_H__

// System includes
#include "sxs_type.h"
#include "sxr_ops.h"

extern void api_UlFlowCtrl ( u8 NewUlCongStatus ) ;
u32 api_CheckTestSim(u32 nSimId);
u32 api_CheckCurrentSimTestSim();


#endif // __API_FSM_H__

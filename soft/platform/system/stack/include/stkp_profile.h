////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL$ //
//	$Author$                                                        // 
//	$Date$                     //   
//	$Revision$                                                          //   
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file stkp_profile.h                                                          //
/// That file provides assert, trace and profiling macros for use in          //
/// Stack implementation                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "cs_types.h"
#include "hal_debug.h"

#ifndef STKP_PROFILE_H
#define STKP_PROFILE_H

// =============================================================================
//  MACROS
// =============================================================================
#define STACK_PROFILE_TO_HAL (1<<HAL_DBG_PXTS_HAL)
#define STACK_PROFILE_TO_STACK (1<<HAL_DBG_PXTS_STACK)
#define STACK_PXTX_BUF_SIZE 512

#define CPMASK     0xffff3fff
#define CPEXITFLAG 0x8000

// =============================================================================
//  TYPES
// =============================================================================
typedef struct
{
	UINT32 time;
	UINT32 code;
}SACK_PROFILE_BUFFER_STRUCT_T;

// =============================================================================
// FUNCTIONS
// =============================================================================
PUBLIC VOID stk_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_LEVEL_T level,
                                            UINT32 functionId);
                                            
PUBLIC VOID stk_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_LEVEL_T level,
                                           UINT32 functionId);

#endif //STKP_PROFILE_H



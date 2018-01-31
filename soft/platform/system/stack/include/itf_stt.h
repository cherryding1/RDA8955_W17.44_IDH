//------------------------------------------------------------------------------
//  $Log: itf_stt.h,v $
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
  File       : ITF_STT.H
--------------------------------------------------------------------------------

  Scope      : Interface Mesages provided by L1

  History    :
--------------------------------------------------------------------------------
  Nov 01 04  |  MCE   | Changed STT_TIM_ID to add SXS_TIMER_EVT_ID 
  Jan 19 04  |  PCA   | Creation
================================================================================
*/

#ifndef __ITF_STT_H__
#define __ITF_STT_H__

#include "sxs_type.h"
#include "cmn_defs.h"

#ifdef __ITF_STT_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

/*
** Shared data.
*/


#undef DefExtern

/*
** Primitive ID definitions
*/
#define STT_INIT_REQ           ( HVY_STT + 1 )
#define STT_INIT_RSP           ( HVY_STT + 2 )
#define STT_RUN_REQ            ( HVY_STT + 3 )
#define STT_RUN_RSP            ( HVY_STT + 4 )
#define STT_SUCCESS            ( HVY_STT + 5 )
#define STT_FAILURE            ( HVY_STT + 6 )
#define STT_TIM_ID             ((HVY_STT + 7)| SXS_TIMER_EVT_ID )
#define STT_EXIT               ( HVY_STT + 8 )
#define STT_PARAM_REQ          ( HVY_STT + 9 )
#define STT_HELP_IND           ( HVY_STT + 10 )


/*
================================================================================
  Structure  : stt_InitReq_t
--------------------------------------------------------------------------------
  Direction  : STT_PC -> STT
  Scope      : STT_PC ask for a list of test cases included in the Scenario
================================================================================
*/
typedef struct {
u8 Data;
} stt_InitReq_t;
/*
================================================================================
  Structure  : stt_InitRsp_t
--------------------------------------------------------------------------------
  Direction  : STT -> STT_PC
  Scope      : STT respond the list of test cases included in the Scenario
================================================================================
*/
typedef struct
{
u8 TestCount;
u8 Names[1];
} stt_InitRsp_t;
/*
================================================================================
  Structure  : stt_RunReq_t
--------------------------------------------------------------------------------
  Direction  : STT_PC -> STT
  Scope      : STT_PC ask to run a specific test case
================================================================================
*/
typedef struct
{
u8 TestCaseNumber;
} stt_RunReq_t;

/*
================================================================================
  Structure  : stt_RunRsp_t
--------------------------------------------------------------------------------
  Direction  : STT -> STT_PC
  Scope      : STT returns test case result
================================================================================
*/
typedef struct
{
u8 TestCaseNumber;
u8 Result;
} stt_RunRsp_t;
/*
================================================================================
  Structure  : stt_ParamReq_t
--------------------------------------------------------------------------------
  Direction  : STT_PC -> STT
  Scope      : STT PC send new Parameters
================================================================================
*/
typedef struct
{
u8 TestCaseNumber;
u8 Params[1];
} stt_ParamReq_t;


#endif

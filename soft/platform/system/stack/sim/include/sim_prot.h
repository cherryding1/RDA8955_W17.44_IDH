//------------------------------------------------------------------------------
//  $Log: sim_prot.h,v $
//  Revision 1.3  2006/05/22 20:51:18  aba
//  Update of the copyright
//
//  Revision 1.2  2005/12/29 17:39:16  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/

/*
================================================================================
  File       : sim.h
--------------------------------------------------------------------------------

  Scope      : Include of the sim software unit

  History    :
--------------------------------------------------------------------------------
  Sep 21 03  |  ABA   | Creation
================================================================================
*/
#ifndef __SIM_PROT_H__
#define __SIM_PROT_H__

#include "sxs_type.h"

typedef enum
{
  SELECT_MF,					// 0
  ANALYZE_SW_SELECT_MF,		// 1
  SELECT_DF,					// 2
  ANALYZE_SW_SELECT_DF,		// 3
  SELECT_EF,						// 4
  ANALYZE_SW_SELECT_EF,		// 5
  MAIN_INSTRUCTION,			// 6
  GET_RESPONSE,					//7
  FETCH,							//8
  ANALYZE_SW_FETCH			//9
}SimState_t;

typedef enum
{
  SIM_IDLE,
  WAITING_ATR,
  WAITING_PPS_ANSWER,
  WAITING_ENHANCED_PPS_ANSWER,
  RESET_DONE
}SimResetState_t;

typedef enum
{
  RIGHT_ATR,
  BAD_ATR,
  START_PPS_PROCEDURE,
  START_ENHANCED_PPS_PROCEDURE
}ATRAnalysisResult_t;

typedef enum
{
  INSTRUCTION_WITHOUT_DATA,
  INSTRUCTION_WITH_INPUT,   // Data sent to the sim
  INSTRUCTION_WITH_OUTPUT   // Data received coming from the sim
}InstructionType_t;


typedef struct
{
  u8 Cla;
  u8 Ins;
  u8 P1;
  u8 P2;
  u8 P3;
  u8 *PtTxData;
  u8 NbDataByteSent;
  u8 Sw1;
  u8 Sw2;
  InstructionType_t InstructionType;
}Instruction_t;

typedef enum
{
	RIGHT,				// status word is right
	ERROR_RETRY,		// status word is error and can retry command
	ERROR_NO_RETRY	// status word is error and neet not retry
}SimAnswerState_t;


/*******************************************************************************
 *  Function Statement
 ******************************************************************************/
void                sim_Interrupt(u32 Cause);
#ifdef LTE_NBIOT_SUPPORT
void                sim_InterruptProc(u32 Cause);
#endif
void                sim_Reset(void);
void                sim_SetNextVoltage(void);
ATRAnalysisResult_t sim_AnalyzeATR();
void                sim_StartPPSProcedure(void);
void                sim_StartEnhancedPPSProcedure();
void                sim_ProcessInstruction(void);
void                sim_BuildAnswerMessage(void);
void                sim_AnalyzeReceivedBytes(void);
void                sim_TimerWaitSw2(void);
SimAnswerState_t                sim_TestSW1SW2(bool IsAssert);
void                sim_SendInstruction(void);
void                sim_SendDataOfInstructionWithInput(void);
void                sim_SendOneByteOfData(void);
void                sim_InitVar(void);
void                sim_ReleaseResource(void);
void                sim_CheckVoltageFault(void);



#endif  // __SIM_PROT_H__

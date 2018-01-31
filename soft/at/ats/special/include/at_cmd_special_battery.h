/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#ifndef _AT_CMD_EX_BATTERY_H_
#define _AT_CMD_EX_BATTERY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                          INCLUDES
 **************************************************************************************************/
#  include "at_common.h"

/**************************************************************************************************
 *                                         CONSTANTS
 **************************************************************************************************/
#define PM_ACSTATUS_OFFLINE     0
#define PM_ACSTATUS_ONLINE  1
#define PM_ACSTATUS_UNKNOWN     255

#define AT_BATTERY_IND_ENABLE 1
#define AT_BATTERY_IND_DISABLE 0
/**************************************************************************************************
 *                                          MACROS
 **************************************************************************************************/
//
// ERROR
//
#define AT_EX_FAIL_CreateRC(pResult,errCode,errorType,delayTime,pBuff,dataSize ,nDLCI)\
    do{                                                                                            \
        pResult = AT_CreateRC(CMD_FUNC_FAIL,CMD_RC_ERROR,errCode,errorType,delayTime,pBuff,dataSize,nDLCI);\
    } while(0)

//
// SUCCESS
//
#define AT_EX_SUCC_CreateRC(pResult,delayTime,pBuff,dataSize,nDLCI)\
    do{\
        pResult = AT_CreateRC(CMD_FUNC_SUCC,CMD_RC_OK,CMD_ERROR_CODE_OK,CMD_ERROR_CODE_TYPE_CME,delayTime,pBuff,dataSize,nDLCI);\
    }while(0)

//
// success,need waite asyn
//
#define AT_EX_SUCC_ASYN_CreateRC(pResult,delayTime,pBuff,dataSize,nDLCI)\
    do{\
        pResult = AT_CreateRC(CMD_FUNC_SUCC_ASYN,CMD_RC_OK,CMD_ERROR_CODE_OK,CMD_ERROR_CODE_TYPE_CME,delayTime,pBuff,dataSize,nDLCI);\
    }while(0)

#define START_BATTERY_TIMER(nDLC) COS_StartCallbackTimer(g_hAtTask, 10000, (COS_CALLBACK_FUNC_T)AT_BATTERY_Timer_Handle, (void*)(nDLC))

#define STOP_BATTERY_TIMER(nDLC) COS_StopCallbackTimer(g_hAtTask, (COS_CALLBACK_FUNC_T)AT_BATTERY_Timer_Handle, (void*)(nDLC))

#define AT_EX_MONI_TIM_ID         20
#define AT_EX_MONP_TIM_ID         21

/**************************************************************************************************
 *                                         TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 *                                     FUNCTIONS - API
 **************************************************************************************************/
extern VOID AT_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                         UINT8 nDLCI, UINT8 nSim);
extern VOID AT_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
/*
 * AT^CBC
 */
VOID AT_BATTERY_CmdFunc_CBC(AT_CMD_PARA *pParam);
VOID AT_BATTERY_CmdFunc_CBCE(AT_CMD_PARA *pParam);

/*
 * AT^CBCM
 */
VOID AT_BATTERY_CmdFunc_CBCM(AT_CMD_PARA *pParam);

VOID AT_SPECIAL_Battery_AsyncEventProcess(COS_EVENT *pEvent);
VOID AT_SPECIAL_Battery_Init();
VOID AT_BATTERY_Timer_Handle(UINT8 nDLCI);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif


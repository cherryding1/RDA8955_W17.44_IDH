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



#ifndef _AT_CMD_EX_FLASH_H_
#define _AT_CMD_EX_FLASH_H_

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

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif

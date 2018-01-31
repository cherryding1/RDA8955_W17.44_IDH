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



#ifndef _AT_SS_API_H_
#define _AT_SS_API_H_

VOID AT_SsEventSuccess(AT_CMD_RESULT **ppResult, UINT8 *pBuf, UINT8 uiBufLen, UINT8 nUTI);

// VOID AT_SsEventSuccessContinue(AT_CMD_RESULT **ppResult, UINT8 *pBuf, UINT8 uiBufLen);

VOID AT_SsEventFail(AT_CMD_RESULT **ppResult, UINT32 errCode, UINT8 *pBuf, UINT8 uiBufLen, UINT8 nUTI);
VOID AT_SsFuncFail(AT_CMD_RESULT **ppResult, UINT32 errCode, UINT8 *pBuf, UINT8 uiBufLen, UINT8 nUTI);
VOID AT_SsFuncSuccess(AT_CMD_RESULT **ppResult, UINT32 returnValue, UINT8 delaytime, UINT8 *pBuf, UINT8 uiBufLen,
                      UINT8 nUTI);
VOID AT_SsSynSuccessWithStr(AT_CMD_RESULT **ppResult, UINT8 *buf, UINT16 len, UINT8 nUTI);
UINT16 AT_SsAsciiToGsmBcd(UINT8 *pNumber, UINT8 nNumberLen, UINT8 *pBCD); // output should >= nNumberLen/2+1
VOID AT_SsClassConvertCcfc(UINT8 ucSrcClass, UINT8 *pucDestClass);
VOID AT_SsClassConvert2Ccfc(UINT8 ucSrcClass, UINT8 *pucDestClass);
// [LLP[ADD]2007/11/26
BOOL SS_PPUIntStructureToFloatStr(UINT8 *pucFloatStr, UINT8 *pucLen, CFW_SIM_PUCT_INFO *pstPUCTInfo);
BOOL SS_PPUFloatStrToIntStructure(SIM_PUCT_INFO *pstPUCT, UINT8 *pucFloatStr, UINT8 ucFloatStrLen);
// LLP[ADD]2007/11/26]
// [[hameina[+]2007.9.17
extern void AT_CC_Result_Err(UINT32 uErrorCode, UINT8 nErrorType, UINT8 nDLCI, UINT8 nSim);
extern void AT_CC_Result_OK(UINT32 uReturnValue, UINT32 uResultCode, UINT8 nDelayTime, UINT8 *pBuffer, UINT16 nDataSize,
                            UINT8 nDLCI, UINT8 nSim);
#endif

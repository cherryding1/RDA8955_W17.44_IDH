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



#ifndef _AT_SS_DEF_H_
#define _AT_SS_DEF_H_

typedef struct _SIM_PUCT_INFO
{
    UINT16 iEPPU;
    UINT8 iEX;
    UINT8 iSign;  // the sign of EX,1 is the negetive sign

} SIM_PUCT_INFO;

// ss macro

#define PARA_CNT_CACM  1
#define PARA_CNT_CAMM  2
#define PARA_CNT_CAOC  1
#define PARA_CNT_CPUC  3
#define PARA_CNT_CSVM  3

// asynchronous event delay time definition

#define ASYN_DELAY_CCFC  5
#define ASYN_DELAY_COLR  4
#define ASYN_DELAY_CUSD  100
#define ASYN_DELAY_CACM  10
#define ASYN_DELAY_CAMM  10
#define ASYN_DELAY_CAOC  10
#define ASYN_DELAY_CPUC  10

#define MAX_LENGTH_BUFF               256

// #define SUCC                            0
#define FAIL                            1

// ccwa
#define NOT_EXIST                         1
#define EXIST                             0

// change from 1-->0 20070913
#define DISABLE                           0
// change from 0-->1  20070913
#define ENABLE                            1

// ccfc
#define MAX_LENGTH_NUMBER_CCFC          (20 + 3)
#define DOMESTIC_CALL                    129

// cusd
// changyg [mod] 2008-5-20 :from 140->160
#define MAX_LENGTH_STRING_USSD         160
#define DEFAULT_ALPHABET                0x0f

// cacm
#define MAX_LENGTH_PIN2                 (4 + 3)
#define AT_SIM_FILE_CACM                4
// camm
#define AT_SIM_FILE_CAMM                5
#define MAX_LENGTH_CAMM                 (6 + 3)
// caoc
#define AT_SIM_FILE_CAOC               1
// cpuc
#define AT_SIM_FILE_CPUC               2

#define MAX_LENGTH_CURRENCY_CPUC       (3 + 3)
#define MAX_LENGTH_PPU_CPUC            (5 + 3)

#define USSD_CANCEL_SESSION             2

#define AT_IS_BIT_SET(uiNum, ucBit) (((uiNum) & (0x00000001 << (ucBit))) >> (ucBit))

//
// success only for event process
//
#define AT_SS_EVENT_TRUE_RETURN(ppResult, pBuf, uiBufLen,nUTI)\
    {\
        AT_SsEventSuccess(ppResult, pBuf, uiBufLen,nUTI);\
        AT_Notify2ATM(*ppResult,nUTI);\
        if (*ppResult != NULL)\
        {\
            AT_FREE(*ppResult);\
            *ppResult = NULL;\
        }\
        return;\
    }

//
// fail only for event process
//
#define AT_SS_EVENT_FAIL_RETURN(ppResult, errCode, pBuf, uiBufLen,nUTI)\
    {\
        AT_SsEventFail(ppResult, errCode, pBuf, uiBufLen,nUTI);\
        AT_Notify2ATM(*ppResult,nUTI);\
        if (*ppResult != NULL)\
        {\
            AT_FREE(*ppResult);\
            *ppResult = NULL;\
        }\
        return;\
    }

//
// fail only for command function,
//

#define AT_SS_FAIL_RETURN(ppResult, errCode, pBuf, uiBufLen,nUTI)\
    {\
        AT_SsFuncFail(ppResult, errCode, pBuf, uiBufLen, nUTI);\
        AT_Notify2ATM(*ppResult,nUTI);\
        if (*ppResult != NULL)\
        {\
            AT_FREE(*ppResult);\
            *ppResult = NULL;\
        }\
        return;\
    }

//
// only out string ERROR,no additional strings
//
#define AT_SS_TRUE_RETURN(ppResult, returnValue, delaytime, pBuf, uiBufLen,nUTI)\
    {\
        AT_SsFuncSuccess(ppResult, returnValue, delaytime, pBuf, uiBufLen,nUTI);\
        AT_Notify2ATM(*ppResult,nUTI);\
        if (*ppResult != NULL)\
        {\
            AT_FREE(*ppResult);\
            *ppResult = NULL;\
        }\
        return;\
    }

//
// success, out strings and other
//
#define AT_SS_SYN_RETURN(ppResult,buf,len,nUTI)\
    {\
        AT_SsSynSuccessWithStr(ppResult,buf,len,nUTI);\
        AT_Notify2ATM(*ppResult,nUTI);\
        if (*ppResult != NULL)\
        {\
            AT_FREE(*ppResult);\
            *ppResult = NULL;\
        }\
        return;\
    }

#endif

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


#include "at.h"
#include "at_errinfo.h"
#include <stdlib.h>
#include "at_cfg.h"
#include <ctype.h>
#include "cos.h"

#define ERR_BAL_INVALID_PARAMETER 0
#define AT_UTLI_BUF_LEN 1024
#define AT_UTLI_U8      8
#define AT_UTLI_U16     16
#define AT_UTLI_U32     32
#define AT_UTIL_NR_PARA 16

#ifndef ERR_SUCCESS
#define ERR_SUCCESS 0
#endif

#define AT_ERROR_INFO_LEN  128

typedef struct _PARA_POS
{
    UINT16 begin;
    UINT16 len;
} PARA_POS, *PPARA_POS, * *PPPARA_POS;

typedef struct _PARA
{
    UINT32 uParaNum;
    PARA_POS sParaPos[AT_UTIL_NR_PARA];
} PARA, *PPARA, * *PPPARA;

#define IS_WHITE_SPACE(ch) ((ch) == 32 || (ch) == 9 || (ch) == 10 || (ch) == 13)

static INT32 save_para(PPARA pResult, UINT32 uOrg, UINT32 uBegin, UINT32 uForward)
{
    if (pResult->uParaNum < AT_UTIL_NR_PARA)
    {
        pResult->sParaPos[pResult->uParaNum].begin = uBegin - uOrg;
        pResult->sParaPos[pResult->uParaNum].len   = uForward - uBegin;
        pResult->uParaNum++;
        return ERR_SUCCESS;
    }
    else
    {
        return ERR_AT_UTIL_TOO_MANY_CMD_PARA;
    }
}

INT32 AT_Util_ParaParser(PPARA pResult, UINT8 *pPara)
{
    UINT32 cheat   = 1;
    UINT32 iStatus = 0;
    UINT8 c;
    UINT8 *begin   = (UINT8 *)pPara;
    UINT8 *forward = (UINT8 *)pPara;
    INT32 lErrCode;

    AT_Util_TrimLeft(pPara);
    if (AT_StrLen(pPara) == 0)
    {
        pResult->uParaNum = 0;
        return ERR_SUCCESS;
    }

    while (1 == cheat)
    {
        switch (iStatus)
        {
        case 0:
            c = * forward++;
            if ('"' == c)
            {
                iStatus = 2;
            }
            else if ('\0' == c)
            {
                iStatus = 3;
            }
            else if (',' == c)
            {
                iStatus = 1;
            }
            else
            {
                iStatus = 0;
            }
            break;

        case 1:
            lErrCode = save_para(pResult, (UINT32)pPara, (UINT32)begin, (UINT32)(forward - 1));
            if (ERR_SUCCESS != lErrCode)
            {
                return lErrCode;
            }

            iStatus = 0;
            begin = forward;
            break;

        case 2:
            c = *forward++;
            if ('"' == c)
            {
                iStatus = 0;
            }
            else if ('\0' == c)
            {
                iStatus = 3;
            }
            else
            {
                iStatus = 2;
            }
            break;

        case 3:
            lErrCode = save_para(pResult, (UINT32)pPara, (UINT32)begin, (UINT32)(forward - 1));
            if (ERR_SUCCESS != lErrCode)
            {
                return lErrCode;
            }
            cheat = 0;
            break;
        }
    }
    return ERR_SUCCESS;
}

// Trim left whitespace characters from the string
// the whitespace include ' ', '\t',<CR>,<LF>
VOID AT_Util_TrimLeft(UINT8 *pString)
{
    UINT8 *p; // temp point.
    if (NULL == pString)    // If string is NULL, do not anything.
    {
        return;
    }

    p = pString;
    while ('\0' != *p)
    {
        // If the character is whitespace,move rightward the point of string,
        if ((*p == 32) || (*p == 9) || (*p == 10) || (*p == 13))
        {
            p++;
        }
        else  // is not whitespace,break the loop.
        {
            break;
        }
    }
    if (NULL != p)
    {
        AT_StrCpy(pString, p);  // copy the temp to header.
    }
}

// Trim right whitespace characters from the string
// the whitespace include ' ', '\t',<CR>,<LF>
VOID AT_Util_TrimRight(UINT8 *pString)
{
    UINT8 *p;
    UINT32 iLen;
    INT32 i;

    if (NULL == pString)
    {
        return;
    }

    p = pString;
    iLen = (UINT8)AT_StrLen(p);
    for (i = (UINT8)(iLen - 1); i >= 0; i--)
    {
        if (IS_WHITE_SPACE(p[i]))
        {
            p[i] = '\0';  // if is whitespace,set it with '\0'.
        }
        else
        {
            break;
        }
    }
}

// Trim left and right whitespace characters from the string
// the whitespace include ' ', '\t',<CR>,<LF>
VOID AT_Util_Trim(UINT8 *pString)
{
    AT_Util_TrimLeft(pString);  // Trim the left whitespace.
    AT_Util_TrimRight(pString); // Trim the right whitespace.
}

// Trim all whitespace characters from the string
// the whitespace include ' ', '\t',<CR>,<LF>
VOID AT_Util_TrimAll(UINT8 *pString)
{
    UINT8 *p;
    UINT32 i, n;
    UINT32 iLen;

    if (NULL == pString)
    {
        return;
    }

    p = pString;
    iLen = (UINT8)AT_StrLen(p);

    n    = 0;
    for (i = 0; i < iLen; i++)
    {
        if ((p[i] == 32) || (p[i] == 9) || (p[i] == 10) || (p[i] == 13))
        {
            continue; // ignore whitespace.
        }
        else
        {
            p[n] = p[i];    // if is not whitespace,move to left tight.
            n++;
        }
    }

    p[n] = '\0';
}

// Trim all whitespace characters from the string
// the whitespace include ' ', '\t',<CR>,<LF>
/*
A trim function specially for trimming space characters (includes \0x20, \0x08 characters) in parameter string.
**/
#define DOUBLE_QUOTATION_CHAR 0x22  // " character
#define SPACE_CHAR 0x20 // space character
#define TAB_CHAR 0x09 // TAB character
VOID AT_Util_TrimAll_Param(UINT8 *szParam)
{
    UINT8 *p    = szParam;
    UINT32 i, n = 0;
    size_t len;
    INT32 fDoubleQuotationMaker = FALSE;

    if (szParam == NULL || (len = strlen(szParam)) <= 0)
        return;

    for (i = 0; i < len; i++)
    {
        if (p[i] == DOUBLE_QUOTATION_CHAR)
            fDoubleQuotationMaker = !fDoubleQuotationMaker;

        // If space character not inner double quotaton marker then ignore it.
        if (fDoubleQuotationMaker == FALSE && (p[i] == SPACE_CHAR || p[i] == TAB_CHAR))
            continue;
        else
            p[n++] = p[i];
    }
    p[n] = '\0';
}

// Trim the left specify characters from the string.
VOID AT_Util_TrimLeftChar(UINT8 *pString, CONST UINT8 cTrimChar)
{
    UINT8 *p;
    if (NULL == pString)
    {
        return;
    }

    p = pString;
    while ('\0' != *p)
    {
        // if the value is specify character,move the point to right.
        if (*p == cTrimChar)
        {
            p++;
        }
        else
        {
            break;
        }
    }

    if (NULL != p)
    {
        AT_StrCpy(pString, p);
    }
}

// Trim backspace characters from the string.
// General the <BS> is 0x08.
VOID AT_Util_TrimBackspace(UINT8 *pString, CONST UINT8 iBSChar)
{
    UINT8 *p = NULL;
    UINT32 i, n;
    UINT32 iLen;

    if (NULL == pString)
    {
        return;
    }

    iLen = AT_StrLen(pString);
    if (iLen == 0)
    {
        return;
    }

    p = pString;
    n = 0;
    for (i = 0; i < iLen; i++)
    {
        if (pString[i] == iBSChar)  // if is backspace.
        {
            if (n > 0)  // if not arrived the header, move ahead the point.
            {
                n--;
            }
        }
        else
        {
            p[n] = pString[i];
            n++;
        }
    }
    p[n] = '\0';
}

VOID AT_Util_TrimRspStringSuffixCrLf(UINT8 *buff, UINT16 *plen)
{
    //if last char(s) is CR and/or LF then  trim it END
    while (*plen > 0 && (buff[*plen - 1] == AT_CMD_CR || buff[*plen - 1] == AT_CMD_LF))
    {
        UINT8 s[21] = { 0 };

        // trace an warning information.
        memcpy(s, buff, * plen > 20 ? 20 : * plen);

        AT_TC(g_sw_UTIL, "Unwanted CR and/or LF suffix character is found in the RSP string!!! rsp=%s", s);
        buff[*plen--] = 0;
    }
}

// This function check the string if is a numeric string.
BOOL AT_Util_IsStringNumeric(CONST UINT8 *pString)
{
    UINT32 i, iLen;

    if (NULL == pString)
    {
        return FALSE;
    }

    iLen = AT_StrLen(pString);
    if (0 == iLen)
    {
        return FALSE;
    }

    for (i = 0; i < iLen; i++)
    {
        // Check the character if in '0'-'9'
        if (pString[i] >= '0' && pString[i] <= '9')
        {
            continue;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}

// This function convert a numeric string to BCD string.
BOOL AT_Util_Numeric2BCD(CONST UINT8 *pNumeric, UINT8 *pBcd, UINT8 *pLen)
{
    UINT8 iInLen; // input string length.
    UINT8 iOutLen;  // output string length.
    UINT32 i;
    UINT8 iLocalLen;  // local used.

    if ((NULL == pNumeric) || (NULL == pBcd) || (NULL == pLen))
    {
        return FALSE;
    }

    AT_MemSet(pBcd, 0x00, (UINT32)(*pLen));
    iInLen = (UINT8)AT_StrLen(pNumeric);
    if (((iInLen) / 2) * 2 == iInLen) // Check the count of input is even number.
    {
        iOutLen = (UINT8)(iInLen / 2);
    }
    else  // is odd number
    {
        iOutLen = (UINT8)(iInLen / 2 + 1);  // the value of iOutLen adding one.
    }

    // if the buffer is less than the result of conversion,return false.
    if ((*pLen) < iOutLen)
    {
        return FALSE;
    }

    *pLen = iOutLen;
    iLocalLen = (UINT8)(iInLen / 2);
    for (i = 0; i < iLocalLen; i++)
    {
        // set the left number to low bit, set the right number to high bit.
        pBcd[i] = (UINT8)((pNumeric[2 * i] & 0x0f) | ((pNumeric[2 * i + 1] & 0x0f) << 4));
    }

    if (iLocalLen < iOutLen)
    {
        // the output length adding one,
        // set the numeric to low bit,set high bit with 'F'.
        pBcd[i] = (UINT8)((pNumeric[2 * i]) | (0xf0));
    }
    return TRUE;
}

// This function convert a BCD string to numeric string.
BOOL AT_Util_BCD2Numeric(CONST UINT8 *pBcd, UINT8 *pNumeric, UINT8 *pLen)
{
    BOOL bRet;
    UINT8 iInLen;
    UINT8 iOutLen;
    UINT32 i;

    // if pont of paramert is NULL,return false.
    if ((NULL == pNumeric) || (NULL == pBcd) || (NULL == pLen))
    {
        return FALSE;
    }

    AT_MemSet(pNumeric, 0x00, (UINT32)(*pLen));
    iInLen  = (UINT8)AT_StrLen(pBcd);
    iOutLen = 0;

    for (i = 0; i < iInLen; i++)
    {
        if ((pBcd[i] & 0xf0) != 0xf0)       // check the validate for high bit
        {
            // get the low bit,
            pNumeric[2 * i] = (UINT8)((((pBcd[i]) >> 4) & 0x0f) | '0');
            iOutLen++;
        }

        if ((pBcd[i] & 0x0f) != 0x0f)       // check the validate for low bit
        {
            if (iOutLen == 2 * i)           // Get low bit
            {
                pNumeric[2 * i] = (UINT8)(((pBcd[i]) & 0x0f) | '0');
            }
            else                        // Get high bit
            {
                pNumeric[2 * i + 1] = (UINT8)(((pBcd[i]) & 0x0f) | '0');
            }

            iOutLen++;
        }
    }

    // if the buffer is less than the result of conversion,return false.
    if (*pLen < iOutLen)
    {
        bRet = FALSE;
    }
    else
    {
        *pLen = iOutLen;
        bRet  = TRUE;
    }
    return bRet;
}

// This function calls srx_malloc to allocate storage space for a copy of strSource and then copies strSource to the allocated space.
// REMARKS: must calling the fuction SFW_MemFree() to free the memory.
UINT8 *AT_Util_StrDup(CONST UINT8 *strSource)
{
    INT32 iLen;
    UINT8 *pBuff;

    if (NULL == strSource)
    {
        return NULL;
    }

    iLen = AT_StrLen(strSource);
    pBuff = (UINT8 *)AT_MALLOC(iLen + 1);
    if (NULL == pBuff)
    {
        return NULL;
    }
    AT_MemSet(pBuff, 0x00, iLen + 1);
    AT_StrCpy(pBuff, strSource);
    return pBuff;
}

// This function is compare string .
INT32 AT_Util_StrCmp(CONST UINT8 *szStr1, CONST UINT8 *szStr2)
{
    UINT32 i   = 0;
    BOOL bLoop = TRUE;

    if (NULL == szStr1)
    {
        if (NULL == szStr2)
            return 0;
        else
            return - 1;
    }
    else
    {
        if (NULL == szStr1)
            return 1;
    }

    while (bLoop)
    {
        if (szStr1[i] > szStr2[i])
            return 1;
        else if (szStr1[i] < szStr2[i])
            return - 1;
        else
        {
            if ('\0' == szStr1[i])
                return 0;
        }

        i++;
    }
    return 0;
}

BOOL AT_Util_CheckValidUn(CONST UINT8 *pParam, UINT8 nType)
{
    BOOL bResult;
    UINT8 szMax[11];
    UINT32 i, iMaxLen;
    INT32 iLen;
    BOOL bRet;
    UINT8 pBuff[AT_UTLI_BUF_LEN];

    AT_MemSet(pBuff, 0x00, AT_UTLI_BUF_LEN);
    if (NULL == pParam)
    {
        return FALSE;
    }

    iLen = AT_StrLen(pParam);
    if (iLen > AT_UTLI_BUF_LEN)
    {
        return FALSE;
    }

    AT_StrCpy(pBuff, pParam);       // Set the max value.
    AT_Util_Trim(pBuff);            // Trim the left and right whitespace.
    iLen = AT_StrLen(pBuff);            // Trim the left '0' from numerical string.

    for (i = 0; i < iLen; i++)
    {
        if ('0' != pBuff[i])
        {
            break;
        }
    }

    if (i == iLen)  // 数字串中全是0
    {
        AT_MemSet(pBuff, 0x00, AT_UTLI_BUF_LEN);
        pBuff[0] = '0';
    }
    else  // 数字串中有非0字符
    {
        AT_Util_TrimLeftChar(pBuff, '0');
    }

    // Check if is a numeric string.
    bResult = AT_Util_IsStringNumeric(pBuff);
    if (FALSE == bResult)
    {
        return FALSE;
    }

    AT_MemSet(szMax, 0x00, 11);
    switch (nType)
    {
    case AT_UTLI_U8:
        AT_StrCpy(szMax, (UINT8 *)"255");
        break;

    case AT_UTLI_U16:
        AT_StrCpy(szMax, (UINT8 *)"65535");
        break;

    case AT_UTLI_U32:
        AT_StrCpy(szMax, (UINT8 *)"4294967295");
        break;

    default:
        AT_StrCpy(szMax, (UINT8 *)"0");
        break;
    }

    iLen = (UINT8)AT_StrLen(pBuff);
    iMaxLen = (UINT8)AT_StrLen((UINT8 *)szMax);
    if (iLen == iMaxLen)
    {
        bRet = FALSE;
        for (i = 0; i < iMaxLen; i++)
        {
            if (pBuff[i] < szMax[i])
            {
                bRet = TRUE;
                break;
            }
            else if (pBuff[i] == szMax[i])
            {
                bRet = TRUE;
                continue;
            }
            else
            {
                bRet = FALSE;
                break;
            }
        }
    }
    else if (iLen > iMaxLen)
    {
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }
    return bRet;
}

// This function is check the paramert if is a UINT8 numeric string .
// The range is 0-255
BOOL AT_Util_CheckValidateU8(CONST UINT8 *pParam)
{
    return AT_Util_CheckValidUn(pParam, AT_UTLI_U8);
}

// This function is check the paramert if is a UINT16 numeric string .
BOOL AT_Util_CheckValidateU16(CONST UINT8 *pParam)
{
    return AT_Util_CheckValidUn(pParam, AT_UTLI_U16);
}

// This function is check the paramert if is a UINT32 numeric string .
BOOL AT_Util_CheckValidateU32(CONST UINT8 *pParam)
{
    return AT_Util_CheckValidUn(pParam, AT_UTLI_U32);
}

// This function is check the paramert if is a keys visible string with quotation mark .
BOOL AT_Util_CheckValidateString(CONST UINT8 *pParam)
{
    UINT32 iLen;
    UINT8 pBuff[AT_UTLI_BUF_LEN] = { 0x00, };

    if (NULL == pParam)
    {
        return FALSE;
    }

    AT_StrCpy(pBuff, pParam);
    AT_Util_Trim(pBuff);
    iLen = (UINT8)AT_StrLen(pBuff);

    return TRUE; // Modify for supporting without '"' at string by lixp at 20130726
    if ('\"' != pBuff[0] || '\"' != pBuff[iLen - 1])
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

INT32 AT_Util_GetParaCount(UINT8 *pPara, UINT8 *pCount)
{
    INT32 lErrCode;
    PARA result;

    AT_MemSet(&result, 0x00, sizeof(PARA));
    lErrCode = AT_Util_ParaParser(&result, pPara);
    if (ERR_SUCCESS != lErrCode)
    {
        *pCount = 0;
        return lErrCode;
    }

    *pCount = (UINT8)(result.uParaNum);
    return ERR_SUCCESS;
}

char** getargv(char* param ,int* count) {
	int j=1;

	char** argv=COS_MALLOC(1000);
	argv[0]="abc";
	AT_TC( g_sw_GPRS,"enterring getargv");
	
	   char* token = strtok( param, " ");
	    AT_TC( g_sw_GPRS,"strtok");
	   while( token != NULL )
	   {
		   AT_TC( g_sw_GPRS,"TOKEN= %s ", token );
		   argv[j]=token;
		   j++;    
		   token = strtok( NULL, " ");
		   AT_TC( g_sw_GPRS,"j= %d ", j );
	   }
	   *count = j;
       AT_TC(g_sw_GPRS, "getargv");
	   return argv;

}

INT32 AT_Util_GetPara(UINT8 *pRes, CONST UINT8 iIndex, UINT8 *pParam, UINT16 *pLen)
{
    PARA result;

    AT_MemSet(&result, 0x00, sizeof(PARA));
    if (ERR_SUCCESS != AT_Util_ParaParser(&result, pRes))
    {
        *pLen = 0;
        return ERR_AT_UTIL_PARA_ERROR;
    }

    if (result.uParaNum == 0)
    {
        *pLen = 0;
        return ERR_AT_UTIL_PARA_ERROR;
    }

    if (*pLen <= result.sParaPos[iIndex].len)
    {
        return ERR_AT_UTIL_PARA_ERROR;
    }

    if (iIndex <= result.uParaNum)
    {
        AT_MemSet(pParam, 0x00, *pLen);
        AT_MemCpy(pParam, pRes + result.sParaPos[iIndex].begin, result.sParaPos[iIndex].len);
        *pLen = result.sParaPos[iIndex].len;
    }
    else
    {
        return ERR_AT_UTIL_PARA_ERROR;
    }
    return ERR_SUCCESS;
}

INT32 AT_Util_GetTrimedPara(UINT8 *pRes, CONST UINT8 iIndex, UINT8 *pPara, UINT16 *pLen)
{
    INT32 lErrCode = AT_Util_GetPara(pRes, iIndex, pPara, pLen);

    if (ERR_SUCCESS != lErrCode)
    {
        return lErrCode;
    }
    else
    {
        AT_Util_Trim(pPara);
        *pLen = AT_StrLen(pPara);
        return ERR_SUCCESS;
    }
}

// This function is get the paramert with specify index and rule.
// The index start from zero.
// Mark:
INT32 AT_Util_GetParaWithRule(UINT8 *pRes, CONST UINT8 iIndex, CONST AT_UTIL_PARA_TYPE eType, PVOID pPara, UINT16 *pLen)
{
    UINT32 iNumValue32; // the value of integer32.
    UINT16 iNmbValue16; // the value of integer16.
    UINT8 iNmbValue8; // the value of integer8.
    UINT8 *pNextParam;  // the next paramert.
    UINT16 iParamLen;  // paramert length.
    INT32 lErrCode;

    if ((NULL == pRes) || (NULL == pPara) || NULL == pLen || 0 == * pLen)
    {
        return ERR_AT_UTIL_PARA_ERROR;
    }

    if (AT_UTIL_PARA_TYPE_STRING == eType) // String type
    {
        // Get the paramert with index.
        pNextParam = (UINT8 *)pPara;
        iParamLen  = *pLen;
        lErrCode   = AT_Util_GetTrimedPara(pRes, iIndex, pNextParam, &iParamLen);
        if (ERR_SUCCESS != lErrCode)
        {
            return lErrCode;
        }

        iParamLen = AT_StrLen(pNextParam);
        if (0 == iParamLen)         // if the parameter length is 0,return ISNULL.
        {
            AT_MemSet(pPara, 0x00, *pLen);
            *pLen = 0;
            return ERR_AT_UTIL_CMD_PARA_NULL;
        }
        if (AT_Util_CheckValidateString(pNextParam))        // Check the string if validate.
        {
            if('"' == pNextParam[iParamLen - 1])
                pNextParam[iParamLen - 1] = '\0'; // Trim the tail '"'
            if('"' == pNextParam[0])
                pNextParam++; // Trim the head '"'

            if ('\0' == *pNextParam)
            {
                AT_MemSet(pPara, 0x00, *pLen);
                *pLen = 0;
            }
            else
            {
                AT_StrCpy((UINT8 *)pPara, pNextParam);
                *pLen = AT_StrLen((UINT8 *)pPara);
            }
            return ERR_SUCCESS;
        }
        else  // Invalid string
        {
            AT_MemSet(pPara, 0x00, *pLen);
            *pLen = 0;
            return ERR_AT_UTIL_FAILED;
        }
    }
    else
    {
        UINT16 iNumLen = AT_UTLI_BUF_LEN;
        UINT8 szNum[AT_UTLI_BUF_LEN];

        AT_MemSet(szNum, 0x00, (UINT32)iNumLen);
        // get the paramert by index.
        lErrCode = AT_Util_GetTrimedPara(pRes, iIndex, szNum, &iNumLen);
        if (ERR_SUCCESS != lErrCode)
        {
            return lErrCode;
        }

        if (0 == AT_StrLen(szNum))
        {
            return ERR_AT_UTIL_CMD_PARA_NULL;
        }

        if (AT_UTIL_PARA_TYPE_UINT8 == eType)
        {
            if (sizeof(UINT8) != *pLen)
            {
                return ERR_AT_UTIL_PARA_ERROR;
            }
            if (AT_Util_CheckValidateU8(szNum))
            {
                iNmbValue8 = (UINT8)AT_StrAToI(szNum);
                *(UINT8 *)pPara = (UINT8)iNmbValue8;
                return ERR_SUCCESS;
            }
        }
        else if (AT_UTIL_PARA_TYPE_UINT16 == eType)
        {
            if (sizeof(UINT16) != * pLen)
            {
                return ERR_AT_UTIL_PARA_ERROR;
            }

            if (AT_Util_CheckValidateU16(szNum))
            {
                iNmbValue16 = (UINT16)AT_StrAToI(szNum);
                *(UINT16 *)pPara = (UINT16)iNmbValue16;
                return ERR_SUCCESS;
            }
        }
        else if(AT_UTIL_PARA_TYPE_UINT32 == eType)
        {
            if (sizeof(UINT32) != * pLen)
            {
                return ERR_AT_UTIL_PARA_ERROR;
            }

            if (AT_Util_CheckValidateU32(szNum))
            {
                iNumValue32 = AT_StrAToI(szNum);
                *(UINT32 *)pPara = (UINT32)iNumValue32;
                return ERR_SUCCESS;
            }
        }
        else
        {
            AT_TC(g_sw_DFMS, "exe     AT_Util_CheckValidateString  failed\n");
            return ERR_AT_UTIL_PARA_ERROR;
        }
        return ERR_AT_UTIL_FAILED;
    }
}

/***************************************************************************************************
                                CSW SUL LIB PATCH SECTION
****************************************************************************************************/

UINT16 AT_UtilGbkStrCharLen(const UINT8 *gbk_string)
{
    UINT16 idx = 0;
    UINT16 len = 0;

    /*
       @RUBY STYLE PSEUDOCODE@
       check parameter
       * */
    AT_ASSERT(gbk_string != NULL);
    /*
       check for NULL character only at the odd no. of bytes assuming forst byte start from zero
       * */
    while (gbk_string[idx] != 0)
    {
        len++;
        /*
           @RUBY STYLE PSEUDOCODE@
           if is an ASCII char then
           idx++
           else
           idx += 2
           end
           * */

        if (gbk_string[idx] <= 127)
            idx++;
        else
            idx += 2;
    }
    return len; /* One is added to count 0th byte */
}

#define AT_UtilGbkStrByteLen(s) (strlen((s)))
UINT16 AT_UtilUcs2StrCharLen(const UINT8 *ucs2_string)
{
    UINT16 idx = 0;
    UINT16 len = 0;
    /*
       @RUBY STYLE PSEUDOCODE@
       check parameter
       * */
    AT_ASSERT(ucs2_string != NULL);
    /*
       check for NULL character only at the odd no. of bytes assuming forst byte start from zero
       * */
    while (ucs2_string[idx] != 0 || ucs2_string[idx + 1] != 0)
    {
        len++;
        idx += 2;
    }
    return len; /* One is added to count 0th byte */
}

UINT16 AT_UtilGbk2Ucs2(UINT8 *gbk_string, UINT16 gbk_str_bytelen, UINT8 *ucs2_string)
{
    /*
       @RUBY STYLE PSEUDOCODE@
       check parameter
       * */
    AT_ASSERT(gbk_string != NULL && gbk_str_bytelen == 0 && ucs2_string != NULL);
    /*
       @RUBY STYLE PSEUDOCODE@
       if convert is okay then
       return the length of the converted (UCS2) string, in bytes
       else
       return -1
       end
       * */
    return 0;
}

UINT16 AT_UtilUcs2ToGbk(UINT8 *ucs2_string, UINT16 ucs2_str_bytelen, UINT8 *gbk_string)
{
    /*
       @RUBY STYLE PSEUDOCODE@
       check parameter
       * */
    AT_ASSERT(ucs2_string != NULL && ucs2_str_bytelen == 0 && gbk_string != NULL);
    /*
       @RUBY STYLE PSEUDOCODE@
       if convert is okay then
       return the length of the converted (GBK) string, in bytes
       else
       return -1
       end
       * */
    return 0;
}


BOOL AT_UtilStringToOctet(UINT8 *src_text_string, UINT16 input_len, UINT8 *dst_real_octet)
{
    UINT16 length;
    UINT16 i = 0, j = 0;
    UINT8   stream[3000];
    AT_Util_Trim(src_text_string);
    length = strlen(src_text_string);
   // if((length % 2 !=0) || (length != 2*input_len))
    //{
        //return FALSE;
    //}
    for(i=0; i < length; i++)
    {
        if(src_text_string[i] != ' ')
        {
            if((src_text_string[i] >= '0')&&(src_text_string[i] <= '9'))
            {
                stream[j] = src_text_string[i] - '0';
            }
            else if((src_text_string[i] >= 'a')&&(src_text_string[i] <= 'f'))
            {
                stream[j] = 10 + src_text_string[i] - 'a';
            }
            else if((src_text_string[i] >= 'A')&&(src_text_string[i] <= 'F'))
            {
                stream[j] = 10 + src_text_string[i] - 'A';
            }
            else
            {
                return FALSE;
            }
            j++;
          //  if(j%2 == 0)
            //{
                
            //}
        }
    }
    if(j%2 == 0)
    {
        if(j != 2*input_len)
        {
            return FALSE;
        }
    }
    else 
    {
        return FALSE;
    }
    for( i = 0, j = 0; i < input_len; i++)
    {
        dst_real_octet[i] = (stream[j] << 4) | stream[j+1];
	 j+=2;
    }
    return TRUE;
}

BOOL AT_UtilOctetToString(UINT8 *src_real_octet,UINT8 src_length, UINT8 *dst_text_string)
{
    UINT16 i = 0, j = 0;
    UINT8 high_4bit = 0;
    UINT8 low_4bit = 0;
    while(i < src_length)
    {
        high_4bit = (src_real_octet[i] >> 4)&0x0f;
	 low_4bit = src_real_octet[i] & 0x0f;
	 if((high_4bit >= 0)&&(high_4bit <= 9))
	 {
	     dst_text_string[j] = high_4bit + '0';
	 }
	 else if((high_4bit >= 0x0a)&&(high_4bit <= 0x0f))
	 {
	     dst_text_string[j] = high_4bit -10 + 'a';
	 }
        else
	 {
	     return FALSE;
        }
	 if((low_4bit >= 0)&&(low_4bit <= 9))
	 {
	     dst_text_string[j+1] = low_4bit + '0';
	 }
	 else if((low_4bit >= 0x0a)&&(low_4bit <= 0x0f))
	 {
	     dst_text_string[j+1] = low_4bit -10 + 'a';
	 }
        else
	 {
	     return FALSE;
        }
        i++;
        j+=2;
    }
    return TRUE;
}


BOOL AT_Util_BitStrToByte(UINT8 *byteDest, CONST UINT8 *strSource)
{
    INT32 iLen;
    UINT8 i;
    UINT8 outputByte = 0;

    if (NULL == strSource)
    {
        return FALSE;
    }

    iLen = AT_StrLen(strSource);
    if (iLen > 8)
    {
        AT_TC(g_sw_UTIL, "AT_Util_BitStrToByte: Strlen is larger than 8, [%s] length is %d", 
            strSource, iLen);
        return FALSE;
    }

    for (i = 0; i < iLen; i++)
    {
        if (strSource[i] == '0')
        {
            outputByte = (outputByte << 1);
        }
        else if (strSource[i] == '1')
        {
            outputByte = ((outputByte << 1) | 0x01);
        }
        else
        {
            AT_TC(g_sw_UTIL, "AT_Util_BitStrToByte: the %dth[%c] is illegal char of [%s]",
                i, strSource[i], strSource);
            return FALSE;
        }
    }

    *byteDest = outputByte;        
    return TRUE;;
}



BOOL AT_Util_ByteToBitStr(UINT8 *bitStrDest, CONST UINT8 byteSource, UINT8 bitLen)
{
    UINT8 i = 0;
    UINT8 bit;

    if (NULL == bitStrDest)
    {
        AT_TC(g_sw_UTIL, "AT_Util_ByteToBitStr: bitStrDest NULL");
        return FALSE;
    }

    if (bitLen > 8 || bitLen == 0)
    {
        AT_TC(g_sw_UTIL, "AT_Util_ByteToBitStr: bitLen=%d is illegal", bitLen);
        return FALSE;
    }

    for (i = 0; i < bitLen; i++)
    {
        bit = (byteSource>>(bitLen - i - 1))&0x01;
        if (bit == 1)
        {
            bitStrDest[i] = '1';
        }
        else if (bit == 0)
        {
            bitStrDest[i] = '0';
        }
    }
    bitStrDest[i] = '\0';
    
    return TRUE;
}



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



#ifndef _AT_UTILITY_H_
#define _AT_UTILITY_H_

#define TEL_DIAL_NUM_MAX  41  // the length of phone number.

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// string compare
//
// Paramert      :
// szStr1:[in]The first string to compare.
// szStr2:[in]The second string to compare.
//
// Return Value  :
// if szStr1 > szStr2  return 1,
// if szStr1 < szStr2  return -1,
// if szStr1 == szStr2 return 0.
// ///////////////////////////////////////////////////////////////////////////////////////////
INT32 AT_Util_StrCmp(const UINT8 *szStr1, const UINT8 *szStr2);

UINT8 *AT_Util_StrDup(const UINT8 *strSource);

// Utility function return code
#define ERR_AT_UTIL_TOO_MANY_CMD_PARA   1
#define ERR_AT_UTIL_PARA_ERROR          2
#define ERR_AT_UTIL_FAILED              3
#define ERR_AT_UTIL_CMD_PARA_NULL       4

// The type of paramert .
typedef enum
{
    AT_UTIL_PARA_TYPE_UINT8,  // UINT8
    AT_UTIL_PARA_TYPE_UINT16, // UINT16
    AT_UTIL_PARA_TYPE_UINT32, // UINT32
    AT_UTIL_PARA_TYPE_STRING, // UINT8*
} AT_UTIL_PARA_TYPE;

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Trim the left whitespace(' ', '\t',<CR>,<LF>) character(s) in the string.
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be trimmed.
//
// Return Value:
// None.
// ///////////////////////////////////////////////////////////////////////////////////////////
VOID AT_Util_TrimLeft(UINT8 *pString);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Trim the right whitespace(' ', '\t',<CR>,<LF>) character(s) in the string.
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be trimmed.
//
// Return Value:
// None.
// ///////////////////////////////////////////////////////////////////////////////////////////
VOID AT_Util_TrimRight(UINT8 *pString);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Trim the left and right whitespace(' ', '\t',<CR>,<LF>) character(s) in the string.
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be trimmed.
//
// Return Value:
// None.
// ///////////////////////////////////////////////////////////////////////////////////////////
VOID AT_Util_Trim(UINT8 *pString);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Trim all of the whitespace(' ', '\t',<CR>,<LF>) character(s) in the string.
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be trimmed.
//
// Return Value:
// None.
// ///////////////////////////////////////////////////////////////////////////////////////////
#define AT_UtilTrimAll AT_Util_TrimAll
VOID AT_Util_TrimAll(UINT8 *pString);

#define AT_UtilTrimAllParam AT_Util_TrimAll_Param
extern VOID AT_Util_TrimAll_Param(UINT8 *szParam);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Trim the left specified character(s) in the string.
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be trimmed.
//
// Return Value:
// None.
// ///////////////////////////////////////////////////////////////////////////////////////////
VOID AT_Util_TrimLeftChar(UINT8 *pString, const UINT8 cTrimChar);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Trim the backspace character(s) and its leader character in the string.
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be trimmed.
//
// Return Value:
// None.
// ///////////////////////////////////////////////////////////////////////////////////////////
#define AT_UtilTrimBackspace AT_Util_TrimBackspace
VOID AT_Util_TrimBackspace(UINT8 *pString, const UINT8 iBSChar);

/*
Trim all the suffix CR and LF character(s).
**/
extern VOID AT_Util_TrimRspStringSuffixCrLf(UINT8 *buff, UINT16 *plen);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Judge if the string is the a numeric string
//
// Parameter:
// pString
// [in][out] A null-terminated  string containing the target characters to be test.
//
// Return Value:
// TRUE if the string is an numeric one, FALSE if not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_IsStringNumeric(const UINT8 *pString);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Convert a numeric string to BCD string.
//
// Parameter:
// pNumeric
// [in] A Null-terminated numeric string.
// pBcd
// [out] A null-terminated BCD string.
// iLen
// [in][out] Using as an input parameter, it is the length of 'pBcd' buffer;
// as an output parameter, it is the length of 'pBcd' value.
//
// Return Value:
// TRUE means the conversion is successfuu and FALUSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_Numeric2BCD(const UINT8 *pNumeric, UINT8 *pBcd, UINT8 *iLen);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Convert a numeric string to BCD string.
//
// Parameter:
// pBcd
// [in] A null-terminated BCD string.
// pNumeric
// [out] A null-terminated numeric string.
// iLen
// [in][out] Using as an input parameter, it is the length of 'pNumeric' buffer;
// as an output parameter, it is the length of 'pNumeric' value.
//
// Return Value:
// TRUE means the conversion is successfuu and FALUSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_BCD2Numeric(const UINT8 *pBcd, UINT8 *pNumeric, UINT8 *pLen);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Test if the parameter is a UINT8 numeric string.
//
// Parameter:
// pParam
// [in] A null-terminated parameter string.
//
// Return Value:
// TRUE if 'pParam' is a UINT8 numeric string and FALUSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_CheckValidateU8(const UINT8 *pNum);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Test if the parameter is a UINT16 numeric string.
//
// Parameter:
// pParam
// [in] A null-terminated parameter string.
//
// Return Value:
// TRUE if 'pParam' is a UINT16 numeric string and FALUSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_CheckValidateU16(const UINT8 *pNum);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Test if the parameter is a UINT32 numeric string.
//
// Parameter:
// pParam
// [in] A null-terminated parameter string.
//
// Return Value:
// TRUE if 'pParam' is a UINT32 numeric string and FALUSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_CheckValidateU32(const UINT8 *pNum);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Test if the parameter is a valid AT string parameter.
//
// Parameter:
// pParam
// [in] A null-terminated parameter string.
//
// Return Value:
// TRUE if 'pParam' is a valid AT string parameter and FALUSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_CheckValidateString(const UINT8 *pStrPara);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Get the count of AT command parameter
//
// Parameter:
// pPara
// [in]A null-terminated string, point to the original AT command parameter.
// pCount
// [out] The count of the 'pPara' retrieved.
//
// Return Value:
// ERR_AT_UTIL_TOO_MANY_CMD_PARA:     'pPara' contains more parameters
// than the AT parameter count limitation.
// ERR_AT_UTIL_PARA_ERROR:            The format of 'pPara' is not up
// to the AT command mustard
// ERR_SUCCESS:                       Successfully.
// ///////////////////////////////////////////////////////////////////////////////////////////
INT32 AT_Util_GetParaCount(UINT8 *pPara, UINT8 *pCount);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Get the pointed sub parameter of AT command by index.
//
// Parameter:
// pOrgPara
// [in] A null-terminated string, point to the original AT command parameter.
// iIndex
// [in] the sub parameter index in the original AT command parameter.
// pPara
// [out] the buffer to store the sub parameter retrieved by 'iIndex'.
// pLen
// [in,out] As an input parameter, it specifies the 'pPara' buffer length.
// as an output paramter, it stores the length of 'pPara'
//
// Return Value:
// ERR_AT_UTIL_TOO_MANY_CMD_PARA:     'pPara' contains more parameters
// than the AT parameter count limitation.
// ERR_AT_UTIL_PARA_ERROR:            The format of 'pPara' is not up to
// the AT command mustard
// ERR_SUCCESS:                       Successfully.
// ///////////////////////////////////////////////////////////////////////////////////////////
INT32 AT_Util_GetPara(UINT8 *pOrgPara, const UINT8 iIndex, UINT8 *pPara, UINT16 *pLen);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Get the pointed sub parameter of AT command by index,
// and the retrieved sub parameter is trimed
//
// Parameter:
// pOrgPara
// [in] A null-terminated string, point to the original AT command parameter.
// iIndex
// [in] the sub parameter index in the original AT command parameter.
// pPara
// [out] the buffer to store the sub parameter retrieved by 'iIndex'.
// pLen
// [in,out] As an input parameter, it specifies the 'pPara' buffer length.
// as an output paramter, it stores the length of 'pPara'
//
// Return Value:
// ERR_AT_UTIL_TOO_MANY_CMD_PARA:     'pPara' contains more parameters
// than the AT parameter count limitation.
// ERR_AT_UTIL_PARA_ERROR:            The format of 'pPara' is not up to the AT command mustard
// ERR_SUCCESS:                       Successfully.
// ///////////////////////////////////////////////////////////////////////////////////////////
INT32 AT_Util_GetTrimedPara(UINT8 *pOrgPara, const UINT8 iIndex, UINT8 *pPara, UINT16 *pLen);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// Get the pointed sub parameter of AT command by index,
// and the retrieved sub parameter is trimed
//
// Parameter:
// pOrgPara
// [in] A null-terminated string, point to the original AT command parameter.
// iIndex
// [in] the sub parameter index in the original AT command parameter.
// pPara
// [out] the buffer to store the sub parameter retrieved by 'iIndex'.
// pLen
// [in,out] As an input parameter, it specifies the 'pPara' buffer length.
// as an output paramter, it stores the length of 'pPara'
//
// Return Value:
// ERR_AT_UTIL_TOO_MANY_CMD_PARA: 'pPara' contains more parameters
// than the AT parameter count limitation.
// ERR_AT_UTIL_PARA_ERROR:        The format of 'pPara' is not up to the AT command mustard
// ERR_AT_UTIL_CMD_PARA_NULL:     If the sub parameter is retrieved and the corresponding
// original AT command parameter is NULL, the value is returned.
// ERR_AT_UTIL_FAILED:            The format of the sub parameter is invalid.
// ERR_SUCCESS:                   Successfully.
// ///////////////////////////////////////////////////////////////////////////////////////////
INT32 AT_Util_GetParaWithRule(UINT8 *pOrgPara, const UINT8 iIndex,
                              const AT_UTIL_PARA_TYPE eType, PVOID pPara, UINT16 *pLen);

/***************************************************************************************************
                                CSW SUL LIB PATCH SECTION
****************************************************************************************************/

/*
因为目前的SUL库存在一些问题，因此专门在这里进行了patch和问题说明。请大家在用SUL之前先浏览一下这里的问题列表和patch。

欢迎大家在这里打补丁和列举问题 !!!
**/

/*
PATCH: 没有GBK string的strlen()函数。
**/

/*
FUNCTION:
Gives the length of GBK encoded string.

PARAMETERS:
[gbk_string][I]Array containing GBK encoded characters.

RETURN:
The character length of the <gbk_string> string. The length is in chracter (1 or 2 bytes), NOT in bytes!
**/
extern UINT16 AT_UtilGbkStrCharLen(const UINT8 *gbk_string);

#define AT_UtilGbkStrByteLen(s) (strlen((s)))

/*
PATCH: 没有USC2 string的strlen()函数。
**/

/*
FUNCTION:
Gives the length of UCS2 encoded string.

PARAMETERS:
[ucs2_string][I]Array containing UCS2 encoded characters.

RETURN:
The character length of the <ucs2_string> string. The length is in chracter (2 bytes), NOT in bytes!
**/
extern UINT16 AT_UtilUcs2StrCharLen(const UINT8 *ucs2_string);

#define AT_UtilUcs2StrByteLen(s) (AT_UtilUcs2StrCharLen((s)) * 2)

/*
PATCH: SUL_LocallanguageToUCS2()返回值根本就没用! 应该返回处理后的USC2字符串的个数才对.
另外目前的SUL库根本就不支持其它的local language，因此干脆就定死支持GBK本地语言了。因此不要再用SUL提供的这个函数了!
**/

/*
FUNCTION:
Converts a GBK encoding string to an UCS2 string.

PARAMETER:
[gbk_string][I] The input GBK encoding string.
[gbk_str_len][I] The length of <gbk_string>, in bytes.
[ucs2_string][o] The converted string in USC2 encoding, little endian format.

RETURN:
Returns the length of converted (output) UCS2 encoding string, in bytes.
**/
extern UINT16 AT_UtilGbk2Ucs2(UINT8 *gbk_string, UINT16 gbk_str_bytelen, UINT8 *ucs2_string);

extern UINT16 AT_UtilUcs2ToGbk(UINT8 *ucs2_string, UINT16 ucs2_str_bytelen, UINT8 *gbk_string);

extern char** getargv(char* param ,int* count) ;


// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// convert a 8bit string to a byte value, e.g. string:01100111 to byte 0x67
//
// Parameter:
// byteDest
// [out] byte value
// strSource
// [in] A null-terminated 8bit string.
// Return Value:
// TRUE means the conversion is successful and FALSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_BitStrToByte(UINT8 *byteDest, CONST UINT8 *strSource);

// ///////////////////////////////////////////////////////////////////////////////////////////
// Function:
// convert a byte to N bit string, e.g. byte 6 to string:0110 with 4bitlen
//
// Parameter:
// bitStrDest
// [out] A null-terminated N bit string
// byteSource
// [in] byte value
// bitLen
// [in] bit length of output bit string
// Return Value:
// TRUE means the conversion is successful and FALSE not.
// ///////////////////////////////////////////////////////////////////////////////////////////
BOOL AT_Util_ByteToBitStr(UINT8 *bitStrDest, CONST UINT8 byteSource, UINT8 bitLen);

BOOL AT_UtilStringToOctet(UINT8 *text_string, UINT16 input_len,UINT8 *real_octet);
BOOL AT_UtilOctetToString(UINT8 *src_real_octet,UINT8 src_length, UINT8 *dst_text_string);


#endif

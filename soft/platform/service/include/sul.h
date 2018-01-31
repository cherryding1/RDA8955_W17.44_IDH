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






















#if !defined(__SUL_H__)
#define __SUL_H__

#include <cswtype.h>
#include <errorcode.h>
#include "stdarg.h"

/******************************************************************************/
/*                           [begin] Ctype Manipulation                       */
/******************************************************************************/
#define _U  0x01    // upper 
#define _L  0x02    // lower 
#define _D  0x04    // digit 
#define _C  0x08    // cntrl 
#define _P  0x10    // punct 
#define _S  0x20    // white space (space/lf/tab) 
#define _X  0x40    // hex digit 
#define _SP 0x80    // hard space (0x20) 

extern const unsigned char _SUL_ctype[];

#define _SUL_ismask(x) (_SUL_ctype[(int)(unsigned char)(x)])
#define SUL_isalnum(c)  ((_SUL_ismask(c)&(_U|_L|_D)) != 0)
#define SUL_isalpha(c)  ((_SUL_ismask(c)&(_U|_L)) != 0)
#define SUL_iscntrl(c)  ((_SUL_ismask(c)&(_C)) != 0)
#define SUL_isdigit(c)  ((_SUL_ismask(c)&(_D)) != 0)
#define SUL_isgraph(c)  ((_SUL_ismask(c)&(_P|_U|_L|_D)) != 0)
#define SUL_islower(c)  ((_SUL_ismask(c)&(_L)) != 0)
#define SUL_isprint(c)  ((_SUL_ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define SUL_ispunct(c)  ((_SUL_ismask(c)&(_P)) != 0)
#define SUL_isspace(c)  ((_SUL_ismask(c)&(_S)) != 0)
#define SUL_isupper(c)  ((_SUL_ismask(c)&(_U)) != 0)
#define SUL_isxdigit(c) ((_SUL_ismask(c)&(_D|_X)) != 0)
#define SUL_isascii(c) (((unsigned char)(c))<=0x7f)
#define SUL_toascii(c) (((unsigned char)(c))&0x7f)
#if 0
unsigned char SRVAPI _SUL_tolower(unsigned char c);
unsigned char SRVAPI _SUL_toupper(unsigned char c);
#define SUL_tolower(c) _SUL_tolower(c)
#define SUL_toupper(c) _SUL_toupper(c)
#else
static inline unsigned char _SUL_tolower(unsigned char c)
{
    if (SUL_isupper(c))
        c -= 'A' - 'a';
    return c;
}

static inline unsigned char _SUL_toupper(unsigned char c)
{
    if (SUL_islower(c))
        c -= 'a' - 'A';
    return c;
}


#define SUL_tolower(c) _SUL_tolower(c)
#define SUL_toupper(c) _SUL_toupper(c)

#define SUL_mkupper(c)  ( (c)-'a'+'A' )
#define SUL_mklower(c)  ( (c)-'A'+'a' )
#endif
#define SUL_mkupper(c)  ( (c)-'a'+'A' )
#define SUL_mklower(c)  ( (c)-'A'+'a' )

/******************************************************************************/
/*                           [end] Ctype Manipulation                         */
/******************************************************************************/



/******************************************************************************/
/*                           [begin] String Manipulation                      */
/******************************************************************************/

PTCHAR  SRVAPI SUL_StrCat(
    TCHAR *strDestination, //string which append one string to.
    CONST TCHAR *strSource //string which append the string from.
);
PTCHAR  SRVAPI SUL_StrNCat(
    TCHAR *dest,      //Null-terminated destination string
    CONST TCHAR *src, //Null-terminated source string
    UINT32 count      //Number of characters to append
);
PTCHAR  SRVAPI SUL_StrChr(
    CONST TCHAR *string,    // The string to be searched
    TCHAR  c                // The character to search for
);
INT32  SRVAPI SUL_StrCompare(
    CONST TCHAR *stringa,
    CONST TCHAR *stringb
);
INT32  SRVAPI SUL_StrCompareAscii(
    CONST TCHAR *stringa,
    CONST TCHAR *stringb
);
INT32  SRVAPI SUL_StrCaselessCompare(
    CONST TCHAR *stringa,
    CONST TCHAR *stringb
);
PTCHAR  SRVAPI  SUL_StrCopy(
    TCHAR *strDestination,  //the string which copy one string to.
    CONST TCHAR *strSource    //the string which copy the string from.
);
PTCHAR  SRVAPI  SUL_StrNCopy(
    TCHAR *dest,
    CONST TCHAR *src,
    INT32 n
);
INT32   SRVAPI  SUL_StrIsAscii(
    CONST TCHAR *stringa    //The string is null terminated to be tested
);
UINT32  SRVAPI  SUL_Strlen(
    CONST TCHAR *string     //The string to be sized
);
INT16  SRVAPI   SUL_StrNCompare (
    CONST TCHAR *stringa, //One string
    CONST TCHAR *stringb, //Another string
    INT32 count //Number of characters to compare
);
INT16  SRVAPI   SUL_StrNCaselessCompare (
    CONST TCHAR *stringa, //One string
    CONST TCHAR *stringb, //Another string
    UINT32  count //Number of characters to compare
);
PTCHAR  SRVAPI  SUL_Strtok (
    TCHAR *string,         // String containing token
    CONST TCHAR *control,  // Another string
    TCHAR **newStr        // The new string
);
BOOL SRVAPI SUL_StrTrim (
    TCHAR *pszSource,           // Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars  // Pointer to a null-terminated string containing the characters
);
PTCHAR SRVAPI SUL_StrTrimEx(
    TCHAR *pszSource,          // Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars  // Pointer to a null-terminated string containing the characters
);
PTCHAR SRVAPI SUL_StrTrimLeft (
    TCHAR *pString  //Pointer to the null-terminated string to be trimmed.
);
BOOL SRVAPI SUL_StrTrimLeftChar(
    TCHAR *pszSource,          //Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars //Pointer to a null-terminated string containing the characters
);
BOOL SRVAPI SUL_StrTrimRight(
    TCHAR *pString  //Pointer to the null-terminated string to be trimmed.
);
void  SRVAPI     SUL_StrTrimRightChar(
    TCHAR *pszSource,
    const TCHAR *pszTrimChars
);
BOOL  SRVAPI     SUL_StrTrimSpace(
    TCHAR *pString  //Pointer to the null-terminated string to be trimmed.
);

PTCHAR SRVAPI    SUL_Strrchr (
    const TCHAR *string,
    INT32 ch
);
UINT32 SRVAPI  SUL_Strnlen(
    const TCHAR *s,
    UINT32 count
);
PSTR  SRVAPI     SUL_CharLower(
    PSTR string
);
PVOID SRVAPI SUL_MemChr(
    CONST VOID *pSource,    //Pointer to the start of the area.
    INT32   c,      //The byte to fill the area with
    UINT32  count   //The size of the area.
);
INT16 SRVAPI SUL_MemCompare(
    CONST VOID *buf1, // One area of memory
    CONST VOID *buf2, // Another area of memory
    UINT16 count      // The size of the area.
);
BOOL SRVAPI SUL_MemCopy32(
    VOID *dest,    //Where to copy to
    CONST VOID *src, //Where to copy from
    UINT32 count       //The size of the area.
);
PVOID SRVAPI SUL_MemCopy8 (
    VOID  *dest,        //Where to copy to
    CONST VOID *src,  //Where to copy from
    UINT32 count        //The size of the area.
);
BOOL SRVAPI SUL_MemCopyEx (
    VOID *dest,    //Where to copy to
    CONST VOID *src, //Where to copy from
    UINT16 count,      //The size of the area.
    UINT16 nOption   //The size of the area.
);
BOOL SRVAPI SUL_MemDMACopy (
    VOID *dest,    //Where to copy to
    CONST VOID *src, //Where to copy from
    UINT16 count,      //The size of the area.
    UINT16 nOption
);
PVOID SRVAPI SUL_MemMove(
    void *dest,         //Where to copy to
    CONST void  *src,   //Where to copy from
    unsigned int    count   //The size of the area.
);
BOOL SRVAPI SUL_MemSet32 (
    VOID *pSource, //Pointer to the start of the area.
    INT32 c,       //The bytes to fill the area with
    UINT16 count   //The size of the area.
);
BOOL SRVAPI SUL_MemSet8(
    void *pSource,  //Pointer to the start of the area.
    INT32 c,        //The byte to fill the area with
    UINT16 count    //The size of the area.
);
BOOL SRVAPI SUL_ZeroMemory32(
    VOID  *pBuf,
    UINT32 count
);
BOOL SRVAPI SUL_ZeroMemory8(
    VOID  *pBuf,
    UINT32 count
);

#define STRING_IS_BLANKS( c )   ((c) == ' ')
#define STRING_IS_SPACE( c )    (((c) == ' ') || ((c) == '\t') || ((c) == '\n'))

/******************************************************************************/
/*                           [END] String Manipulation                        */
/******************************************************************************/


/******************************************************************************/
/*                           [begin] vsprintf Utility                         */
/******************************************************************************/

INT32  SRVAPI    SUL_StrAToI(
    const TCHAR *nptr
);
INT32  SRVAPI   SUL_StrPrint(
    TCHAR   *buffer,        // Storage location for output
    CONST TCHAR *format,        // Format-control string
    ...                         // Optional arguments
);
INT32 SRVAPI SUL_StrVPrint(
    TCHAR   *str,
    CONST TCHAR *fmtstr,
    va_list arg
);

/******************************************************************************/
/*                           [end] vsprintf Utility                           */
/******************************************************************************/


/******************************************************************************/
/*                           [begin] File System Utiltiy                      */
/******************************************************************************/

INT32  SRVAPI SUL_GetFileName(
    const TCHAR *pathname,
    TCHAR *out_file_name
);
INT32  SRVAPI SUL_Splitpath(
    TCHAR *fullpath,
    TCHAR *dir,
    TCHAR *filename
);
INT32  SRVAPI SUL_Makepath(
    TCHAR *dir,
    TCHAR *filename,
    TCHAR *fullpath
);

#define getfilename SUL_GetFileName // retrieves the name of the filename, For example, getfilename will return TEXT.DAT for the file \My Documents\Text.dat. 
#define splitpath   SUL_Splitpath       // Break a path name into components
#define makepath    SUL_Makepath        // Create a path name from components
/******************************************************************************/
/*                           [END] File System Utiltiy                        */
/******************************************************************************/


/******************************************************************************/
/*                           [begin] Phone Utility                            */
/******************************************************************************/
#define SUL_SIZEOFFORLOWSTRING          50  //define the string's size for getURLString function，这个也可以看作是URL的长度要求
#define SUL_SIZEOFPATTERN               0X07
#define SUL_MAXEMAILLENGTHBEFORESYMBOL  25
#define SUL_MAXEMAILLENGTHAFTERSYMBOL   25
#define SUL_PHONE_MAXWEBADDLENGTH      50

BOOL  SRVAPI SUL_IsValidPhoneNumber(
    TCHAR *PhoneNumber,
    UINT8 nSize
);
UINT32  SRVAPI SUL_CompressPhoneString(
    PCSTR pSrc,
    UINT8 nSrcSize,
    PSTR pDest,
    UINT8 nDestSize
);
UINT32  SRVAPI SUL_UnCompressPhoneString(
    PCSTR pSrc,
    UINT8 nSrcSize,
    PSTR pDest,
    UINT8 nDestSize
);
UINT32  SRVAPI SUL_GetNumberString (
    PCSTR pString,
    UINT8 *pArray,
    UINT8 nSize,
    UINT8 nCount,
    UINT8 nMode
);
UINT32  SRVAPI SUL_GetURLString (
    PCSTR pString,
    UINT8 *pArray,
    UINT8 nSize,
    UINT8 nCount,
    UINT8 nMode
);
UINT32  SRVAPI SUL_GetEmaiString (
    PCSTR pString,
    UINT8 *pArray,
    UINT8 nSize,
    UINT8 nCount,
    UINT8 nMode
);
BOOL SRVAPI  SUL_IsValidWebAddr(
    PCSTR pString,
    UINT8 nSize);
BOOL SRVAPI  SUL_IsValidEmailAddr(
    PCSTR pString,
    UINT8 nSize
);
/*
#define IsValidWebAddr          SUL_IsValidWebAddr
#define IsValidWebAddr          SUL_IsValidWebAddr
#define IsValidPhoneNumber      SUL_IsValidPhoneNumber
#define CompressPhoneString     SUL_CompressPhoneString
#define UnCompressPhoneString   SUL_UnCompressPhoneString
#define GetNumberString         SUL_GetNumberString
#define GetURLString            SUL_GetURLString
#define GetEmaiString           SUL_GetEmaiString
*/
/******************************************************************************/
/*                           [END] PHONE Utiltiy                              */
/******************************************************************************/

/******************************************************************************/
/*                           [begin] Image Decode                             */
/******************************************************************************/
typedef struct _Gif_DecodeData
{
    UINT32 nTime;     //延时
    UINT16 nX;        //起始点横坐标
    UINT16 nY;        //起始点纵坐标
    UINT16 nWidth;    //图片宽度
    UINT16 nHigh;     //图片高度
    UINT16 nFrame;    //当前帧数
    UINT16 *pOutData; //解码后的图片buffer，使用以后由MMI释放
} Gif_DecodeData;

typedef struct _Jpeg_DecodeData
{
    UINT16 nWidth;
    UINT16 nHigh;
    UINT16 *pOutData; //MMI free -->length = nWidth*nHigh
    //16 bit array
} Jpeg_DecodeData;

typedef struct _Bmp_DecodeData
{
    UINT16 nWidth;
    UINT16 nHigh;
    UINT16 *pOutData; //MMI free -->length = nWidth*nHigh
    //16 bit array
} Bmp_DecodeData;


UINT32  SRVAPI    SUL_GifDecode(
    UINT8 *GifData,
    UINT32 nLength,
    Gif_DecodeData *pDecodeData
);

UINT32  SRVAPI   SUL_JpegDecode(
    UINT8 *JpegData,
    UINT32 nLength,
    Jpeg_DecodeData *pDecodeData
);


UINT32 SRVAPI   SUL_BmpDecode(
    UINT8 *BmpData,
    UINT32 nLength,
    Bmp_DecodeData *pDecodeData
);


/******************************************************************************/
/*                           [end] Image Decode                               */
/******************************************************************************/

INT32 SRVAPI SUL_CharacterLen(
    PCSTR   pString,        // The given SMS string.
    UINT16  nSize           //The size of the SMS string
);

INT32 SRVAPI SUL_Encode7Bit(
    PCSTR   pSrc,          // The given SMS string.
    PSTR    pDest,         // Output encode SMS string.
    UINT16  nSrcSize      // the given SMS sting size in bytes.
);

INT32 SRVAPI SUL_Decode7Bit(
    PCSTR   pSrc,               // The given SMS string.
    PSTR    pDest,            // Output Decode SMS string.
    UINT16  nSrcSize          // the given SMS sting size in bytes.
);

INT32 SRVAPI SUL_ASCII2GSM(
    PCSTR  pSrc,            // The given ASCII string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest        // Output translation string.
);

INT32 SRVAPI SUL_GSM2ASCII(
    PCSTR  pSrc,        // The given GSM string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest        // Output translation string.
);

INT32 SRVAPI SUL_LocallanguageToUCS2(
    PCSTR  pSrc,            // The given local language string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest        // Output translation string.s
);

INT32 SRVAPI SUL_UCS2ToLocallanguage(
    PCSTR  pSrc,            // The given local language string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest        // Output translation string.
);

PSTR SRVAPI SUL_CharLower(PSTR string);

PSTR SRVAPI CharLowerBuff(
    PSTR string,
    UINT32 Count
);

PSTR SRVAPI SUL_CharUpper(PSTR string);

PSTR SRVAPI SUL_CharUpperBuff(
    PSTR string,
    UINT32 Count
);

BOOL SRVAPI IsCharAlpha(
    TCHAR ch
);

BOOL SRVAPI IsCharAlphaNumeric(
    TCHAR ch
);
BOOL SRVAPI IsCharLower(
    TCHAR ch
);

BOOL SRVAPI IsCharUpper(
    TCHAR ch
);

PTCHAR SRVAPI SUL_CharPrev(
    const TCHAR *lpszStart,
    const TCHAR *lpszCurrent
);

PTCHAR SRVAPI SUL_CharNext(
    const TCHAR *lpsz
);

INT32 SRVAPI SUL_CountChar(
    const TCHAR *pStrStart
);

BOOL SRVAPI SUL_SeekCodeTable(HRES hdata,
                              INT32 src,
                              UINT8 srcByte,
                              UINT16 *pNum
                             );

BOOL SRVAPI SUL_EncodePDU(
    UINT8 *pSmsContent,
    INT8   *phoneNum,
    UINT8 *pPackedSubmitMsg
);
INT32 SRVAPI SUL_GSM2ISO8859_x(
    PCSTR  pSrc,        // The given GSM string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest ,      // Output translation string.
    UINT8 ISO_type
);
INT32 SRVAPI SUL_ISO8859_x2GSM(
    PCSTR  pSrc,            // The given ISO string.
    UINT16 nSrcSize,    // the given string's length.
    PSTR   pDest  ,     // Output translation string.
    UINT8 ISO_type
);

INT32 SRVAPI SUL_GET_ISOStrlen_FROM_GSM(
    PCSTR  pGSM,        // The given GSM string.
    UINT16 nSrcSize,    // the given string's length.
    UINT8 ISO_type
);
INT32 SRVAPI SUL_GET_GSMStrlen_FROM_ISO(
    PCSTR  pISO,        // The given ISO string.
    UINT16 nSrcSize,    // the given string's length.
    UINT8 ISO_type
);

#define GSM_7BIT        0
#define GSM_8BIT        4
#define GSM_UCS2        8
#define MAX_SMS_CONTENT 140

typedef struct _PSMS_PDU_INFO
{
    UINT8 pSCA[21];
    UINT8 pTPA[21];
    UINT8 TP_PID;
    UINT8 TP_DCS;
    UINT8 pTP_SCTS[16];
    UINT8 TP_UD[161];
    UINT8 index;
} SMS_PDU_INFO, *PSMS_PDU_INFO;

UINT16 SRVAPI SUL_DecodePdu(
    CONST INT8 *pSrc,
    PSMS_PDU_INFO pDst
);

UINT16 SRVAPI SUL_GSM2UCS2(
    UINT8 *pSrc,
    UINT8 *pDes,
    UINT16 nLength);

UINT16 SRVAPI SUL_String2Bytes(
    CONST UINT8 *pSrc,
    UINT8 *pDst,
    UINT16 nSrcLength
);

UINT16 SRVAPI SUL_SerializeNumbers(
    INT8 *pSrc,
    INT8 *pDst,
    UINT16 nSrcLength
);

VOID SRVAPI SUL_SetLastError(
    UINT32 nErrCode   // error code
);

UINT32 SRVAPI SUL_GetLastError(VOID);

typedef struct _SCA_PARAM
{
    UINT8 nBCD[22];
    UINT8 nLength;
    UINT8 nType;
} SCA_PARAM;

//
//该函数会去掉最后的F
//0x08, 0x91, 0x68, 0x31 0x18 0x11 0x98 0x38 0xF6 --> '13811189836'
//
BOOL SRVAPI SUL_GetSCAStruct(
    UINT8 nSCABcd[12],
    SCA_PARAM *pParam
);

//
// GSMBCD码,如果转化奇数个数字的话，将会在对应高位补F
// 支持'*''#'''+'p'
// +13811189836 --> 0x0D, 0x31 0x18 0x11 0x98 0x38 0xF6
// 13811189836 --> 0x31 0x18 0x11 0x98 0x38 0xF6
//
UINT16 SRVAPI SUL_AsciiToGsmBcd(
    INT8 *pNumber,  // input
    UINT8 nNumberLen,
    UINT8 *pBCD           // output (The size of this buffer should >= nNumberLen/2+1)
);

//
// 该函数会去掉最后的F，同时将A转化为‘*’将B转化为‘#’将C转化为‘p’
// 将D转化为"+"
// 0x31 0x18 0x11 0x98 0x38 0xF6 --> 13811189836
//
UINT16 SRVAPI SUL_GsmBcdToAscii (
    UINT8 *pBcd,     // input
    UINT8 nBcdLen,
    UINT8 *pNumber  // output (The size of this buffer should >= nBcdLen*2)
);

#if !defined(ISODDNUM)
#define ISODDNUM(a)            ((a & 0x01) ? (TRUE) : (FALSE))
#endif
#if !defined(ISEVENNUM)
#define ISEVENNUM(a)           ((a & 0x01) ? (FALSE) : (TRUE))
#endif

//
//Change the user dial number(UDN) to stack dial number(SDN).
//The space's of SDN is  alloted by the user,and the length of SDN is 2 bytes
//biger than the UDN length at least.
//Also the pUDN and pSDN can be pointed to the same pointer.
//
#if 0
UINT32 SUL_UserDialNum2StackDialNum(
    CFW_DIALNUMBER *pNumber,
    UINT8 *pSDN,
    UINT8 *pSDNLen
);
#endif

// "+,001,138111" ---> "91,001,138111". // internal type
// "+,138111" ---> "91,138111".   // national
// "138111" ---> "81,138111".     // unknown type
// "86138111" ---> "81,86138111". // unknown type

UINT8   *SUL_Itoa   (INT32 value, UINT8 *string, INT32 radix);
UINT8   *SUL_Ltoa   (INT32 value, UINT8 *string, INT32 radix);

//For Wide string utility functions
UINT16  *SUL_Wcschr (const UINT16 *str, UINT16 ch);
UINT16  *SUL_Wcsstr (const UINT16 *s, const UINT16 *b);
UINT16  *SUL_Wcsrchr    (const UINT16 *str, UINT16 ch);
INT32   SUL_Wcsnicmp(const UINT16 *cs, const UINT16 *ct, UINT32 count);
UINT32  SUL_Wcsnlen (const UINT16 *s, UINT32 count);
UINT16  *SUL_Wcscat (UINT16 *dest, const UINT16 *src);
INT32   SUL_Wcscmp  (const UINT16 *cs, const UINT16 *ct);
UINT16  *SUL_Wcscpy (UINT16 *str1, const UINT16 *str2);
INT32   SUL_Wcsicmp(const UINT16 *cs, const UINT16 *ct);
UINT16  *SUL_Wcsupr (UINT16 *x);
UINT16  *SUL_Wcslwr (UINT16 *x);
UINT16  *SUL_Wcsncat    (UINT16 *dest, const UINT16 *src, UINT32 count);
INT32   SUL_Wcsncmp(const UINT16 *cs, const UINT16 *ct, UINT32 count);
UINT16  *SUL_Wcstok (UINT16 *s, const UINT16 *ct);
UINT32  SUL_Wcstoul (const UINT16 *cp, UINT16 **endp, UINT32 base);
INT32   SUL_Iswspace(UINT16 wc);
UINT32  SUL_FloorEX (double z);
UINT32  SUL_CeilEX  (double z);

#endif // _H_


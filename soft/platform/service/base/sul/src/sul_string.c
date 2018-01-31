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















#include <sul.h>
#include <rfm.h>
#include<stdio.h>

#include <csw_mem_prv.h>

const unsigned char _SUL_ctype[] =
{
    _C, _C, _C, _C, _C, _C, _C, _C,         /* 0-7 */
    _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C, _C,        /* 8-15 */
    _C, _C, _C, _C, _C, _C, _C, _C,         /* 16-23 */
    _C, _C, _C, _C, _C, _C, _C, _C,         /* 24-31 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P,           /* 32-39 */
    _P, _P, _P, _P, _P, _P, _P, _P,         /* 40-47 */
    _D, _D, _D, _D, _D, _D, _D, _D,         /* 48-55 */
    _D, _D, _P, _P, _P, _P, _P, _P,         /* 56-63 */
    _P, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,   /* 64-71 */
    _U, _U, _U, _U, _U, _U, _U, _U,         /* 72-79 */
    _U, _U, _U, _U, _U, _U, _U, _U,         /* 80-87 */
    _U, _U, _U, _P, _P, _P, _P, _P,         /* 88-95 */
    _P, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,   /* 96-103 */
    _L, _L, _L, _L, _L, _L, _L, _L,         /* 104-111 */
    _L, _L, _L, _L, _L, _L, _L, _L,         /* 112-119 */
    _L, _L, _L, _P, _P, _P, _P, _C,         /* 120-127 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     /* 128-143 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     /* 144-159 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 160-175 */
    _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 176-191 */
    _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, /* 192-207 */
    _U, _U, _U, _U, _U, _U, _U, _P, _U, _U, _U, _U, _U, _U, _U, _L, /* 208-223 */
    _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, /* 224-239 */
    _L, _L, _L, _L, _L, _L, _L, _P, _L, _L, _L, _L, _L, _L, _L, _L
};      /* 240-255 */


INLINE TCHAR checkmatch(TCHAR *p, const TCHAR *match)
{
    while(*match != '\0')
        if(*p == *match++)
            return 1;

    return 0;
}
/******************************************************************************/
/* Function name:    SUL_StrCat                                               */
/* Description:                                                               */
/*                   Concatenates one null-terminated string to another.      */
/* Parameters:                                                                */
/*                  strDestination      The string to be appended to          */
/*                  strSource           The string to append to it            */
/* Return Values:                                                             */
/*                  The return values is the pointer of the dest string       */
/*                                                                            */
/******************************************************************************/
TCHAR *SUL_StrCat(
    TCHAR *strDestination, //string which append one string to.
    CONST TCHAR *strSource //string which append the string from.
)
{
    TCHAR *tmp = strDestination;

    while (*strDestination)
    {
        strDestination++;
    }
    while ((*strDestination++ = *strSource++) != '\0')
        ;

    return(tmp);
}
/******************************************************************************/
/* Function name:    SUL_StrNCat                                              */
/* Description:                                                               */
/*                   The SUL_StrNCat function append characters of string     */
/* Parameters:                                                                */
/*                  dest        Null-terminated destination string            */
/*                  src         Null-terminated source string                 */
/*                  count       Number of characters to append                */
/* Return Values:                                                             */
/*                  The return values is the pointer of the dest string       */
/*                                                                            */
/******************************************************************************/
TCHAR *SUL_StrNCat(
    TCHAR *dest,      //Null-terminated destination string
    CONST TCHAR *src, //Null-terminated source string
    UINT32 count      //Number of characters to append
)
{
    TCHAR *tmp = dest;
    count = count - SUL_Strlen(dest) - 1;
    if (count)
    {
        while (*dest)
            dest++;
        while ((*dest++ = *src++) != '\0')
        {
            if (--count == 0)
            {
                *dest = '\0';
                break;
            }
        }
    }

    return tmp;
}
/******************************************************************************/
/* Function name:    SUL_StrChr                                               */
/* Description:                                                               */
/*                   把字符串s中的所有字符都设置成字符c。                     */
/* Parameters:                                                                */
/*                  string  The string to be searched                         */
/*                  c       The character to search for                       */
/* Return Values:                                                             */
/*                  returns a pointer to the first occurrence of c in string, */
/*                  or NULL if c is not found。                               */
/******************************************************************************/
TCHAR *SUL_StrChr(
    CONST TCHAR *string,    // The string to be searched
    TCHAR  c                // The character to search for
)
{
    const TCHAR *ptmp = string;
    TCHAR itmp = (TCHAR)c;
    while(*ptmp && (*ptmp != itmp))
    {
        ptmp++;
    }

    if (*ptmp != itmp)
    {
        ptmp = NULL;
    }

    return (TCHAR *) ptmp;
}

/******************************************************************************/
/* Function name:    SUL_StrCompare                                           */
/* Description:                                                               */
/*                   Case-sensitive comparison of two strings, and return the */
/*                   lexicographic relation of string1 to string2.            */
/* Parameters:                                                                */
/*                  stringa     One string                                    */
/*                  stringb     Another string                                */
/* Return Values:                                                             */
/*                  The return values is the the lexicographic relation of    */
/*                          string1 to string2.                               */
/*      Value       Relationship of string1 to string2                        */
/*      <0              string1 less than string2                             */
/*      =0              string1 identical to string2                          */
/*      >0              string1 greater than                                  */
/*                                                                            */
/******************************************************************************/

INT32 SUL_StrCompare(
    CONST TCHAR *stringa,
    CONST TCHAR *stringb
)
{
    register INT8 __res = 0;

    for(;;)
    {
        if ((__res = (TCHAR)(*stringa - *stringb++)) != 0 || !*stringa++)
            break;
    }

    return __res;
}

/******************************************************************************/
/* Function name:    SUL_StrCompareAscii                                      */
/* Description:                                                               */
/*                   Compares two ASCII strings out to n bytes. This function */
/*                   calls through to the standard strncmp() function         */
/* Parameters:                                                                */
/*                  stringa     One string                                    */
/*                  stringb     Another string                                */
/* Return Values:                                                             */
/*                  The return values is the the lexicographic relation of    */
/*                          string1 to string2.                               */
/*      Value       Relationship of string1 to string2                        */
/*      <0              string1 less than string2                             */
/*      =0              string1 identical to string2                          */
/*      >0              string1 greater than                                  */
/*                                                                            */
/******************************************************************************/

INT32 SUL_StrCompareAscii(
    CONST TCHAR *stringa,
    CONST TCHAR *stringb
)
{
    register INT8 __res = 0;

    for(;;)
    {
        if ((__res = (TCHAR)(*stringa - *stringb++)) != 0 || !*stringa++)
            break;
    }

    return __res;
}
/******************************************************************************/
/* Function name:    SUL_StrCaselessCompare                                   */
/* Description:                                                               */
/*                   Case-sensitive comparison of two strings, and return the */
/*                   lexicographic relation of string1 to string2.            */
/* Parameters:                                                                */
/*                  stringa     One string                                    */
/*                  stringb     Another string                                */
/* Return Values:                                                             */
/*                  The return values is the the lexicographic relation of    */
/*                          string1 to string2.                               */
/*      Value       Relationship of string1 to string2                        */
/*      <0              string1 less than string2                             */
/*      =0              string1 identical to string2                          */
/*      >0              string1 greater than                                  */
/*                                                                            */
/******************************************************************************/

INT32 SUL_StrCaselessCompare(
    CONST TCHAR *stringa,
    CONST TCHAR *stringb
)
{
    register INT8 __res = 0;

    for(;;)
    {
        if(((*stringa - *stringb) != 32) && ((*stringa - *stringb) != -32) && ((*stringa - *stringb) != 0))
        {
            __res = (TCHAR)(*stringa - *stringb);
            break;
        }
        if(!*stringa++ || !*stringb++)
        {
            break;
        }
    }

    return __res;
}
/******************************************************************************/
/* Function name:    SUL_StrCopy                                              */
/* Description:                                                               */
/*                   Copy a NULL terminated string                            */
/* Parameters:                                                                */
/*                  strDestination   the string which copy one string to.     */
/*                  strSource        the string which copy the string from.   */
/* Return Values:                                                             */
/*                  returns the destination string (strDestination). No return*/
/*                      value is reserved to indicate an error.               */
/******************************************************************************/
TCHAR *SUL_StrCopy(
    TCHAR *strDestination,  //the string which copy one string to.
    CONST TCHAR *strSource    //the string which copy the string from.
)
{
    TCHAR *tmp = strDestination;

    while ((*strDestination++ = *strSource++) != '\0')
        /* nothing */;
    return(tmp);
}
/******************************************************************************/
/* Function name:    SUL_StrNCopy                                             */
/* Description:                                                               */
/*                   Copy a length-limited, NULL-terminated string            */
/* Parameters:                                                                */
/*                  dest    Where to copy the string to                       */
/*                  src     Where to copy the string from                     */
/*                  count   The maximum number of bytes to copy               */
/* Return Values:                                                             */
/*                  The return values is the point of the dest string         */
/*                                                                            */
/* Remark:                                                                    */
/*              Note that unlike userspace strncpy, this does not %NUL-pad    */
/*              the buffer.However, the result is not %NUL-terminated if the  */
/*              source exceeds count bytes.                                   */
/******************************************************************************/
TCHAR *SUL_StrNCopy(
    TCHAR *dest,
    CONST TCHAR *src,
    INT32 n
)
{
    TCHAR *tmp = dest;

    while (n-- && (*dest++ = *src++) != '\0')
        /* nothing */;

    return tmp;
}
/******************************************************************************/
/* Function name:    SUL_StrIsAscii                                           */
/* Description:                                                               */
/*                   Determine if given string in valid ascii range           */
/* Parameters:                                                                */
/*                  stringa  a String is null terminated to be tested         */
/* Return Values:                                                             */
/*                  TRUE if all TCHARacters are in ASCII range.                */
/******************************************************************************/
INT32 SUL_StrIsAscii(
    CONST TCHAR *stringa    //The string is null terminated to be tested
)
{
    while(*stringa)
    {
        if( 0x80 & *stringa)
            return 0;
        stringa++;
    }
    return 1;
}
/******************************************************************************/
/* Function name:    SUL_Strlen                                               */
/* Description:                                                               */
/*                   Find the length of a string                              */
/* Parameters:                                                                */
/*                  string   The string to be sized                           */
/* Return Values:                                                             */
/*                  The return values is the length of the special string     */
/*                                                                            */
/******************************************************************************/
UINT32 SUL_Strlen(
    CONST TCHAR *string     //The string to be sized
)
{
    const TCHAR *sc;

    for (sc = string; *sc != '\0'; ++sc)
        /* nothing */;
    return(sc - string);
}

/******************************************************************************/
/* Function name:    SUL_Strnlen                                              */
/* Description:                                                               */
/*                   Find the length of a length-limited string               */
/* Parameters:                                                                */
/*                  s       The string to be sized                            */
/*                  count   The maximum number of bytes to search             */
/* Return Values:                                                             */
/*                  The return values is the length of a length-limited string*/
/*                                                                            */
/******************************************************************************/
UINT32 SUL_Strnlen(const TCHAR *s, UINT32 count)
{
    const TCHAR *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc)
        /* nothing */;
    return(sc - s);
}

/******************************************************************************/
/* Function name:    SUL_StrNCompare                                          */
/* Description:                                                               */
/*                   Compare two length-limited strings                       */
/* Parameters:                                                                */
/*                  stringa     One string                                    */
/*                  stringb     Another string                                */
/*                  count   The maximum number of bytes to compare            */
/* Return Values:                                                             */
/*                  The return values is the the lexicographic relation of    */
/*                          string1 to string2.                               */
/*      Value       Relationship of string1 to string2                        */
/*      <0              string1 less than string2                             */
/*      =0              string1 identical to string2                          */
/*      >0              string1 greater than                                  */
/*                                                                            */
/******************************************************************************/
INT16 SUL_StrNCompare (
    CONST TCHAR *stringa, //One string
    CONST TCHAR *stringb, //Another string
    INT32 count //Number of characters to compare
)
{
    register INT8 __res = 0;

    while (count)
    {
        if ((__res = (TCHAR)(*stringa - *stringb++)) != (TCHAR)0 || !*stringa++)
            break;
        count--;
    }

    return __res;
}
/******************************************************************************/
/* Function name:    SUL_StrNCaselessCompare                                  */
/* Description:                                                               */
/*                   Compare two length-limited strings without regard to case*/
/* Parameters:                                                                */
/*                  stringa     One string                                    */
/*                  stringb     Another string                                */
/*                  count   The maximum number of bytes to compare            */
/* Return Values:                                                             */
/*                  The return values is the the lexicographic relation of    */
/*                          string1 to string2.                               */
/*      Value       Relationship of string1 to string2                        */
/*      <0              string1 less than string2                             */
/*      =0              string1 identical to string2                          */
/*      >0              string1 greater than                                  */
/*                                                                            */
/******************************************************************************/
INT16 SUL_StrNCaselessCompare (
    CONST TCHAR *stringa, //One string
    CONST TCHAR *stringb, //Another string
    UINT32  count //Number of characters to compare
)
{
    // DBG_ASSERT(stringa,"stringa NULL");
    // DBG_ASSERT(stringb,"stringb NULL");
    if((!stringa) && stringb) return -1;
    else if((!stringb) && stringa) return 1;
    else if((!stringb) && (!stringa))return 0;

    // MODIFY at 2005-11-14
    //    while (count) {
    //        if ((__res = (TCHAR)(*stringa - *stringb++)) != ((TCHAR)0 || !*stringa++ || -32 || 32))
    //      {
    //              break;
    //      }
    //      if(__res == 32 || __res == -32)
    //      {
    //          __res = 0;
    //          break;
    //      }
    //
    //        count--;
    //    }
    int Tmpa, Tmpb;

    if ( count )
    {
        do
        {
            if ( ((Tmpa = (unsigned char)(*(stringa++))) >= 'A') &&
                    (Tmpa <= 'Z') )
                Tmpa -= 'A' - 'a';

            if ( ((Tmpb = (unsigned char)(*(stringb++))) >= 'A') &&
                    (Tmpb <= 'Z') )
                Tmpb -= 'A' - 'a';

        }
        while ( --count && Tmpa && (Tmpa == Tmpb) );
        return((UINT16)(Tmpa - Tmpb));
    }

    return(0);
}
/******************************************************************************/
/* Function name:    SUL_Strtok                                           */
/* Description:                                                               */
/*                   Find the next token in a string.                         */
/* Parameters:                                                                */
/*                  string      String containing token                       */
/*                  controls2   Another string                                */
/*                  newStr      The new string                                */
/* Return Values:                                                             */
/*                  If there is a next token, the return value is the pointer */
/*                  of the next token, If there isnot a next token,           */
/*                  the return value is NULL.                                 */
/******************************************************************************/
TCHAR *SUL_Strtok (
    TCHAR *string,         // String containing token
    CONST TCHAR *control,  // Another string
    TCHAR **newStr        // The new string
)
{
    //unsigned CHAR *str;
    //const unsigned CHAR *ctrl = control;
    //unsigned CHAR map[32];
    TCHAR *str;
    const TCHAR *ctrl = control;
    TCHAR map[32];
    TCHAR *nextoken = *newStr;
    INT32 count;

    /* Clear control map */
    for (count = 0; count < 32; count++)
        map[count] = 0;

    /* Set bits in delimiter table */
    do
    {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    }
    while (*ctrl++);

    /* Initialize str. If string is NULL, set str to the saved
    * pointer (i.e., continue breaking tokens out of the string
    * from the last strtok call) */
    if (string)
        str = string;
    else
        str = nextoken;

    /* Find beginning of token (skip over leading delimiters). Note that
    * there is no token iff this loop sets str to point to the terminal
    * null (*str == '\0') */
    while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
        str++;

    string = str;

    /* Find the end of the token. If it is not the end of the string,
    * put a null there. */
    for ( ; *str ; str++ )
        if ( map[*str >> 3] & (1 << (*str & 7)) )
        {
            *str++ = '\0';
            break;
        }

    /* Update nextoken (or the corresponding field in the per-thread data
    * structure */
    *newStr = str;

    /* Determine if a token has been found. */
    if ( string == str )
        return NULL;
    else
        return string;
}

/******************************************************************************/
/* Function name:    SUL_StrTrim                                              */
/* Description:                                                               */
/*                   Removes (trims) specified leading and trailing CHARacters*/
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  pszSource  Pointer to null-terminated string to be trimmed*/
/*                  pszTrimChars string containing the CHARacters             */
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/
BOOL SUL_StrTrim (
    TCHAR *pszSource,           // Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars  // Pointer to a null-terminated string containing the characters
)
{
    int len;
    TCHAR *p;
    TCHAR *pWrite;

    len = SUL_Strlen(pszSource);

    // Remove right
    for (p = pszSource + len - 1; len && checkmatch(p, pszTrimChars); len--, p--)
    {
        *p = '\0';
    }

    // Remove left
    for (p = pszSource; *p && checkmatch(p, pszTrimChars); p++)
    {
        ;
    }

    // write back
    for (pWrite = pszSource; *p; )
        *(pWrite++) = (TCHAR)(*p ? * (p++) : *p);
    return 1;
}
/******************************************************************************/
/* Function name:    SUL_StrTrimEx                                            */
/* Description:                                                               */
/*                   Removes (trims) specified leading and trailing CHARacters*/
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  pszSource  Pointer to null-terminated string to be trimmed*/
/*                  pszTrimChars string containing the CHARacters             */
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/
TCHAR *SUL_StrTrimEx(
    TCHAR *pszSource,          // Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars  // Pointer to a null-terminated string containing the characters
)
{
    int len;
    TCHAR *p;
    //TCHAR *pWrite;

    len = SUL_Strlen(pszSource);

    // Remove right
    for (p = pszSource + len - 1; len && checkmatch(p, pszTrimChars); len--, p--)
    {
        *p = '\0';
    }

    // Remove left
    for (p = pszSource; *p && checkmatch(p, pszTrimChars); p++)
    {
        ;
    }

    return p;
}
/******************************************************************************/
/* Function name:    SUL_StrTrimLeft                                          */
/* Description:                                                               */
/*                   Removes (trims) specified all(' ','\t', and , '\n')      */
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  str  Pointer to null-terminated string to be trimmed      */
/* Return Values:                                                             */
/*                  Pointer to the string                                     */
/******************************************************************************/
TCHAR *SUL_StrTrimLeft (
    TCHAR *pString  //Pointer to the null-terminated string to be trimmed.
)
{
    TCHAR *pRead;

    pRead = pString;

    for (; *pRead && STRING_IS_SPACE(*pRead); pRead++)
    {
        ;
    }

    return pRead;
}
/******************************************************************************/
/* Function name:    SUL_StrTrimLeftChar                                      */
/* Description:                                                               */
/*                   Removes (trims) specified leading CHARacters             */
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  pszSource  Pointer to null-terminated string to be trimmed*/
/*                  pszTrimChars  string containing the CHARacters            */
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/
BOOL SUL_StrTrimLeftChar(
    TCHAR *pszSource,          //Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars //Pointer to a null-terminated string containing the characters
)
{
    int len;
    TCHAR *p;
    TCHAR *pWrite;

    len = SUL_Strlen(pszSource);

    // Remove left
    for (p = pszSource; *p && checkmatch(p, pszTrimChars); p++)
    {
        ;
    }

    // write back
    for (pWrite = pszSource; *p; )
        *(pWrite++) = (TCHAR)(*p ? * (p++) : *p);

    *pWrite = '\0';   //modify at 2005-11-28
    return 1;
}
/******************************************************************************/
/* Function name:    SUL_StrTrimRight                                         */
/* Description:                                                               */
/*                   Removes (trims) specified all(' ')CHARacters             */
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  s  Pointer to null-terminated string to be trimmed        */
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/
BOOL SUL_StrTrimRight(
    TCHAR *pString  //Pointer to the null-terminated string to be trimmed.
)
{
    int len;
    TCHAR *p;

    len = SUL_Strlen(pString);

    for (p = pString + len - 1; len && STRING_IS_SPACE(*p); len--, p--)
    {
        *p = '\0';
    }
    return 1;
}
/******************************************************************************/
/* Function name:    SUL_StrTrimRightChar                                 */
/* Description:                                                               */
/*                   Removes (trims) specified trailing CHARacters            */
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  pszSource  Pointer to null-terminated string to be trimmed*/
/*                  pszTrimChars  string containing the CHARacters            */
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/

void SUL_StrTrimRightChar(TCHAR *pszSource,  const TCHAR *pszTrimChars)
{
    int len;
    TCHAR *p;
    //TCHAR *pWrite;

    len = SUL_Strlen(pszSource);

    // Remove right
    for (p = pszSource + len - 1; len && checkmatch(p, pszTrimChars); len--, p--)
    {
        *p = '\0';
    }

    // write back
    //for (pWrite = pszSource; *p; )
    //    *(pWrite++) = (TCHAR)(*p ? *(p++) : *p);
    //modify at 2005-11-28
    *(++p) = '\0';
}
/******************************************************************************/
/* Function name:    SUL_StrTrimSpace                                         */
/* Description:                                                               */
/*                   Removes (trims) specifiedall (' ' , '\t', '\n')CHARacters*/
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  pszSource  Pointer to null-terminated string to be trimmed*/
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/
BOOL SUL_StrTrimSpace(
    TCHAR *pString  //Pointer to the null-terminated string to be trimmed.
)
{
    TCHAR *pRead, *pWrite;

    for (pWrite = pRead = pString; *pRead; )
    {
        for (; *pRead && STRING_IS_SPACE(*pRead); pRead++)
        {
            ;
        }
        *(pWrite++) = (TCHAR)(*pRead ? * (pRead++) : *pRead);
    }
    return 1;
}

/******************************************************************************/
/* Function name:    SUL_StrTrimAll                                       */
/* Description:                                                               */
/*                   Removes (trims) specifiedall CHARacters*/
/*                   from a string.                                           */
/* Parameters:                                                                */
/*                  pszSource  Pointer to null-terminated string to be trimmed*/
/* Return Values:                                                             */
/*                  NONE                                                      */
/******************************************************************************/
BOOL SUL_StrTrimAll(
    TCHAR *pszSource,        // Pointer to the null-terminated string to be trimmed.
    CONST TCHAR *pszTrimChars  // Pointer to a null-terminated string containing the characters
)
{
    TCHAR *pRead, *pWrite;

    for (pWrite = pRead = pszSource; *pRead; )
    {
        for (; *pRead && checkmatch(pRead, pszTrimChars); pRead++)
        {
            ;
        }
        *(pWrite++) = (TCHAR)(*pRead ? * (pRead++) : *pRead);
    }
    return 1;
}

/* we use this so that we can do without the ctype library */
#define is_digit(c)     ((c) >= '0' && (c) <= '9')


static int skip_atoi(const TCHAR **s)
{
    int i = 0;

    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}


INT32 SUL_StrAToI( const TCHAR *nptr)
{
    return ( skip_atoi( &nptr ) );
}


#define ZEROPAD 1               /* pad with zero */
#define SIGN    2               /* unsigned/signed long */
#define PLUS    4               /* show plus */
#define SPACE   8               /* space if plus */
#define LEFT    16              /* left justified */
#define SPECIAL 32              /* 0x */
#define LARGE   64              /* use 'ABCDEF' instead of 'abcdef' */

#if 0
static TCHAR *number(TCHAR *str, long num, int base, int size, int precision, int type)
{
    int res;
    TCHAR c, sign, tmp[66];
    const TCHAR *digits = (TCHAR *)TEXT("0123456789abcdefghijklmnopqrstuvwxyz");
    int i;

    if (type & LARGE)
        digits = (TCHAR *)TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    c = (TCHAR)((type & ZEROPAD) ? '0' : ' ');
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }

    i = 0;
    if (num == 0)
        tmp[i++] = '0';

    else while (num != 0)
        {
            res = ((unsigned long)num ) % (unsigned) base;
            num = ((unsigned long) num) / (unsigned) base;
            tmp[i++] = digits[res];
        }

    if (i > precision)
        precision = i;
    size -= precision;

    if (!(type & (ZEROPAD + LEFT)))
    {
        while(size-- > 0)
            *str++ = ' ';
    }

    if (sign)  *str++ = sign;
    if (type & SPECIAL)
    {
        if (base == 8)
            *str++ = '0';
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))
        while (size-- > 0)
            *str++ = c;
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    return str;
}
#endif


/*
   replace vsprintf  with new_vsprintf  because vsprintf has a bug in rom
   when call SUL_StrPrint(buf, "%%%02X", 0x20),it will output "%%20" ,
   but it should output "%20"
*/
#if 1
#define _put(c,buf,ebuf,count)  __put(c,&buf,ebuf,&count)
static void __put(char c,char **_buf,char *ebuf,int *pcount)
{
    char *buf = *_buf;

    if (ebuf>buf)
    {
        *buf++ = (c);
        *_buf = buf;
        *pcount += 1;
    }
    else
        *buf = '\0';
}


/*
 * Macros for converting digits to letters and vice versa
 */
#define to_digit(c) ((c) - '0')
// #define is_digit(c) ((unsigned)to_digit(c) <= 9)
#define to_char(n)  ((char)((n) + '0'))


/* flags definition */
/* 4 bits reserved for base :) */
#define FLAG_BASE_8     8
#define FLAG_BASE_10    10
#define FLAG_BASE_16    16
#define MASK_FLAG_BASE  0x1a

#define FLAG_HALF_WORD  0x01
#define FLAG_LONG_WORD  0x04
#define FLAG_ZERO_PAD   0x20
#define FLAG_RIGHT_PAD  0x40
#define FLAG_WITH_PREC  0x80
#define FLAG_SPACE      0x100
#define FLAG_PLUS       0x200
#define FLAG_SIGNED     0x400

static int
new_printn(unsigned long ul, unsigned int width, unsigned int flags,char *tbuf, char *ebuf, unsigned int capcase)
{
    /* hold a long in base 8 */
    char *p, buf[(sizeof(long) * 8 / 3) + 1];
    unsigned int len, pad;
    char sign = 0;
    unsigned int base = flags & MASK_FLAG_BASE;
    int count=0;
    const char *nllow = "0123456789abcdef";
    const char *nlcap = "0123456789ABCDEF";
    const char *nl;

    if (capcase)
    {
        nl = nlcap;
    }
    else
    {
        nl = nllow;
    }

    if (flags & FLAG_SIGNED)
    {
        if ( (flags & FLAG_HALF_WORD) && ((short) ul < 0))
        {
            sign = '-';
            ul = -(short)ul;
        }
        else if ((long)ul < 0)
        {
            sign = '-';
            ul = -(long)ul;
        }
        /* Plus overrides space ... */
        else if (flags & FLAG_PLUS)
        {
            sign = '+';
        }
        else if (flags & FLAG_SPACE)
        {
            sign = ' ';
        }
    }

    else if (flags & FLAG_HALF_WORD)
        ul = (unsigned short) ul;

    p = buf;
    do
    {
        *p++ = nl[ul % base];
    }
    while (ul /= base);

    len = (unsigned int) (p - buf);
    pad =  width - len;

    if (sign)
    {
        if (flags&FLAG_ZERO_PAD) _put(sign,tbuf,ebuf,count);
        pad--;
    }

    if ( !(flags&FLAG_RIGHT_PAD) && (width > len))
    {
        while (pad--)
        {
            _put((flags & FLAG_ZERO_PAD) ? '0' : ' ',tbuf,ebuf,count);
        }
    }

    if (sign && !(flags & FLAG_ZERO_PAD))
        _put(sign,tbuf,ebuf,count);

    do
    {
        _put(*--p,tbuf,ebuf,count);
    }
    while (p > buf);

    if ( (flags&FLAG_RIGHT_PAD) && (width > len))
    {
        while (pad--)
        {
            _put(' ',tbuf,ebuf,count);
        }
    }

    return count;
}
static int
new_doprnt(const char *fmt,char *buf,char *ebuf, va_list ap)
{
    char *p;
    const char *fmt0;
    int ch;
    unsigned long ul;
    unsigned int flags = 0;
    unsigned int width = 0;
    unsigned int prec = 0;
    int count=0,tmp_count=0;

    // Handle the null string.
    if (fmt == NULL)
    {
        return 0;
    }

    for (;;)
    {
        while ((ch = *fmt++) != '%')
        {
            if (ch == '\0')
            {
                _put(ch,buf,ebuf,count);
                return count-1;
            }
            _put(ch,buf,ebuf,count);
        }
        flags = width = prec = 0;
        fmt0 = fmt; // useful to test if we are at the first char
reswitch:   switch (ch = *fmt++)
        {
            case '0':
                // 0 can be either zero padding if first after %
                //   or part of a precision or width if after... /
                if (fmt == fmt0 + 1)
                {
                    flags |= FLAG_ZERO_PAD;
                    goto reswitch;
                }
            // else fall through
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if (flags & FLAG_WITH_PREC)
                {
                    prec = 10 * prec + to_digit(ch);
                }
                else
                {
                    width = 10 * width + to_digit(ch);
                }
                goto reswitch;
            case '-':
                flags |= FLAG_RIGHT_PAD;
                goto reswitch;
            case ' ':
                flags |= FLAG_SPACE;
                goto reswitch;
            case '+':
                flags |= FLAG_PLUS;
                goto reswitch;
            case '.':
                flags |= FLAG_WITH_PREC;
                goto reswitch;
            case 'h':
                flags |= FLAG_HALF_WORD;
                goto reswitch;
            case 'l':
                flags |= FLAG_LONG_WORD;
                goto reswitch;
            case 'c':
                ch = va_arg(ap, int);
                _put(ch & 0x7f,buf,ebuf,count);
                break;
            case 's':
                p = va_arg(ap, char *);
                if(p != NULL)
                {
                    if ((flags & FLAG_WITH_PREC) && prec)
                    {
                        while ((prec--) && (ch = *p++))
                            _put(ch,buf,ebuf,count);
                    }
                    else
                    {
                        while ((ch = *p++))
                            _put(ch,buf,ebuf,count);
                    }
                }
                break;
            case 'i':
            case 'd':
                ul = va_arg(ap, long);
                flags |= (FLAG_BASE_10|FLAG_SIGNED);
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf+= tmp_count;
                count += tmp_count;
                break;
            case 'o':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_8;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf += tmp_count;
                count += tmp_count;
                break;
            case 'u':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_10;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf += tmp_count;
                count += tmp_count;
                break;
            case 'p':
                _put('0',buf,ebuf,count);
                _put('x',buf,ebuf,count);
            // fall through
            case 'x':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_16;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf += tmp_count;
                count += tmp_count;
                break;
            case 'X':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_16;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 1);
                buf += tmp_count;
                count += tmp_count;
                break;
            case '%':
                _put(ch,buf,ebuf,count);
                break;
            default:
                _put('%',buf,ebuf,count);
                if (flags & FLAG_HALF_WORD)
                    _put('h',buf,ebuf,count);
                if (flags & FLAG_LONG_WORD)
                    _put('l',buf,ebuf,count);
                // dingmx fix the bug
                if (ch == '\0')
                {
                    _put(ch,buf,ebuf,count);
                    return count-1;
                }
                _put(ch,buf,ebuf,count);
        } /* end switch */
    } /* end for */
}
static int
new_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    int count;
    char *ebuf = buf+size-1;
    count = new_doprnt(fmt, buf, ebuf, ap);
    return (count);
}

static int
new_vsprintf(char *buf, const char *fmt, va_list ap)
{
    return (new_vsnprintf(buf, -(size_t)buf, fmt, ap));
}
INT32 SUL_Vsprintf(TCHAR *buf, const TCHAR *fmt, va_list args)
{
    return new_vsprintf(buf,  fmt, args);
}
#else
INT32 SUL_Vsprintf(TCHAR *buf, const TCHAR *fmt, va_list args)
{
    return vsprintf(buf,  fmt, args);
}
#endif
INT32 SUL_StrPrint(
    TCHAR   *buffer,        // Storage location for output
    CONST TCHAR *format,        // Format-control string
    ...                         // Optional arguments
)
{
    va_list args;
    INT32 Tmp;

    va_start(args, format);
    Tmp = SUL_StrVPrint(buffer, format, args);
    va_end(args);
    return Tmp;
}
INT32 SUL_StrVPrint(
    TCHAR   *str,
    CONST TCHAR *fmtstr,
    va_list arg
)
{

    return(SUL_Vsprintf(str, fmtstr, arg));
}

/******************************************************************************/
/* Function name:    SUL_Memchr                                               */
/* Description:                                                               */
/*                   Find a CHARacter in an area of memory.                   */
/* Parameters:                                                                */
/*                  s  The memory area                                        */
/*                  c  The byte to search for                                 */
/*                  n  The size of the area.                                  */
/* Return Values:                                                             */
/*                  returns the address of the first occurrence of c, or NULL */
/*                  if c is not found                                        */
/******************************************************************************/
VOID *SUL_MemChr(
    CONST VOID *pSource,    //Pointer to the start of the area.
    INT32   c,      //The byte to fill the area with
    UINT32  count   //The size of the area.
)
{
    const TCHAR *p = pSource;
    while (count-- != 0)
    {
        if ((TCHAR)c == *p++)
        {
            return (void *)(p - 1);
        }
    }
    return NULL;
}

/******************************************************************************/
/* Function name:    SUL_MemCompare                                           */
/* Description:                                                               */
/*                   Compare two areas of memory                              */
/* Parameters:                                                                */
/*                  cs  One area of memory                                    */
/*                  ct  Another area of memory                                */
/*                  count  The size of the area.                              */
/* Return Values:                                                             */
/*                  The return values is the the lexicographic relation of    */
/*                          cs to ct.                                         */
/*      Value       Relationship of string1 to ct                             */
/*      <0              cs less than ct                                       */
/*      =0              cs identical to ct                                    */
/*      >0              cs greater than ct                                    */
/*                                                                            */
/******************************************************************************/
INT16 SUL_MemCompare(
    CONST VOID *buf1, // One area of memory
    CONST VOID *buf2, // Another area of memory
    UINT16 count      // The size of the area.
)
{
    const TCHAR *su1, *su2;
    int res = 0;

    for( su1 = buf1, su2 = buf2; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return (INT16)res;
}
/******************************************************************************/
/* Function name:    SUL_MemCopy32                                            */
/* Description:                                                               */
/*                   Copy one area of memory to another                       */
/* Parameters:                                                                */
/*                  dest    Where to copy the string to                       */
/*                  src     Where to copy the string from                     */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  NONE                                                      */
/*                                                                            */
/******************************************************************************/
BOOL SUL_MemCopy32(
    VOID *dest,    //Where to copy to
    CONST VOID *src, //Where to copy from
    UINT32 count       //The size of the area.
)
{
    UINT32 i;
    INT32 *dstTmp, *srcTmp;

    count = count >> 2;

    dstTmp = (INT32 *)dest;
    srcTmp = (INT32 *)src;

    for (i = 0; i < count; i++)
    {
        *dstTmp++ = *srcTmp++;
    }
    return 1;
}

/******************************************************************************/
/* Function name:    SUL_MemCopy8                                             */
/* Description:                                                               */
/*                   Copy one area of memory to another                       */
/* Parameters:                                                                */
/*                  dest    Where to copy the string to                       */
/*                  src     Where to copy the string from                     */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  The return values is the point of the dest string         */
/*                                                                            */
/* Remark:                                                                    */
/*              You should not use this function to access IO space,          */
/*              use memcpy_toio() or memcpy_fromio() instead.                 */
/******************************************************************************/
VOID *SUL_MemCopy8 (
    VOID  *dest,        //Where to copy to
    CONST VOID *src,  //Where to copy from
    UINT32 count        //The size of the area.
)
{
    TCHAR *tmp = (TCHAR *) dest, *s = (TCHAR *) src;

    while (count--)
        *tmp++ = *s++;

    return dest;
}


/******************************************************************************/
/* Function name:    SUL_MemCopyEx                                            */
/* Description:                                                               */
/*                   Copy one area of memory to another                       */
/* Parameters:                                                                */
/*                  dest    Where to copy the string to                       */
/*                  src     Where to copy the string from                     */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  The return values is the point of the dest string         */
/*                                                                            */
/* Remark:                                                                    */
/*              You should not use this function to access IO space,          */
/*              use memcpy_toio() or memcpy_fromio() instead.                 */
/******************************************************************************/
BOOL SUL_MemCopyEx (
    VOID *dest,    //Where to copy to
    CONST VOID *src, //Where to copy from
    UINT16 count,      //The size of the area.
    UINT16 nOption   //The size of the area.
)
{
    dest = dest;
    src = src;
    count = count;
    nOption = nOption;
    return 1;
}

/******************************************************************************/
/* Function name:    SUL_MemDMACopy                                           */
/* Description:                                                               */
/*                   Copy one area of memory to another                       */
/* Parameters:                                                                */
/*                  dest    Where to copy the string to                       */
/*                  src     Where to copy the string from                     */
/*                  count   The size of the area.                             */
/*                  nOption     The size of the area.                         */
/* Return Values:                                                             */
/*                  The return values is the point of the dest string         */
/*                                                                            */
/* Remark:                                                                    */
/*              You should not use this function to access IO space,          */
/*              use memcpy_toio() or memcpy_fromio() instead.                 */
/******************************************************************************/
BOOL SUL_MemDMACopy (
    VOID *dest,    //Where to copy to
    CONST VOID *src, //Where to copy from
    UINT16 count,      //The size of the area.
    UINT16 nOption
)
{
    dest = dest;
    src = src;
    count = count ;
    nOption = nOption;
    return 1;
}


/******************************************************************************/
/* Function name:    SUL_Memmove                                              */
/* Description:                                                               */
/*                   Moves one buffer to another.                             */
/* Parameters:                                                                */
/*                  dest    Where to move the buffer to                       */
/*                  src     Where to copy the buffer from                     */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  The dest buffer pointer                                   */
/*                                                                            */
/******************************************************************************/
VOID *SUL_MemMove(
    void *dest,         //Where to copy to
    CONST void  *src,   //Where to copy from
    unsigned int    count   //The size of the area.
)
{
    TCHAR *tmp, *s;

    if (dest <= src)
    {
        tmp = (TCHAR *) dest;
        s = (TCHAR *) src;
        while (count--)
            *tmp++ = *s++;
    }
    else
    {
        tmp = (TCHAR *) dest + count;
        s = (TCHAR *) src + count;
        while (count--)
            *--tmp = *--s;
    }

    return dest;
}


/******************************************************************************/
/* Function name:    SUL_Memset32                                             */
/* Description:                                                               */
/*                   把字符串s中的所有字符都设置成字符c。                     */
/* Parameters:                                                                */
/*                  s  The string to be reset                                 */
/*                  c  The special CHAR                                       */
/*                  n  The length to be reset                                 */
/* Return Values:                                                             */
/*                  返回指向s的指针。                                         */
/******************************************************************************/
BOOL SUL_MemSet32 (
    VOID *pSource, //Pointer to the start of the area.
    INT32 c,       //The bytes to fill the area with
    UINT16 count   //The size of the area.
)
{
    long *xs = (long *)pSource;

    count = (UINT16)(count >> 2);
    while (count--)
        *xs++ = c;

    return(1);
}
/******************************************************************************/
/* Function name:    SUL_MemSet8                                              */
/* Description:                                                               */
/*                   Fill a region of memory with the given value             */
/* Parameters:                                                                */
/*                  s   Pointer to the start of the area.                     */
/*                  c   The byte to fill the area with                        */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  Returns a pointer fill with the given value. No return    */
/*                      value is reserved to indicate an error.               */
/*                                                                            */
/* Remark:                                                                    */
/*              You should not use this function to access IO space,          */
/*              use memset_toio() or memset_fromio() instead.                 */
/******************************************************************************/
BOOL SUL_MemSet8(
    void *pSource,  //Pointer to the start of the area.
    INT32 c,        //The byte to fill the area with
    UINT16 count    //The size of the area.
)
{
    TCHAR *xs = (TCHAR *) pSource;

    while (count--)
        *xs++ = (TCHAR)c;

    return(1);
}

/******************************************************************************/
/* Function name:    SUL_ZeroMemory32                                         */
/* Description:                                                               */
/*                   Fills a block of memory with zeros.                      */
/* Parameters:                                                                */
/*                  s       One area of memory                                */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  NONE                                                      */
/*                                                                            */
/******************************************************************************/
BOOL SUL_ZeroMemory32(
    VOID  *pBuf,
    UINT32 count
)
{
    long *xs = (long *) pBuf;

    count >>= 2;

    while (count--)
        *xs++ = 0x0;
    return 1;
}

/******************************************************************************/
/* Function name:    SUL_ZeroMemory8                                          */
/* Description:                                                               */
/*                   Fills a block of memory with zeros.                      */
/* Parameters:                                                                */
/*                  s       a string                                          */
/*                  count   The size of the area.                             */
/* Return Values:                                                             */
/*                  NONE                                                      */
/*                                                                            */
/******************************************************************************/
BOOL SUL_ZeroMemory8(
    VOID  *pBuf,
    UINT32 count
)
{
    TCHAR *xs = pBuf;

    while (count--)
        *xs++ = 0x0;
    return 1;
}
/******************************************************************************/
/* Function name:    SUL_Strrchr                                              */
/* Description:                                                               */
/*                   Scan a string for the last occurrence of a CHARacter.    */
/* Parameters:                                                                */
/*                  string      Null-terminated string to search              */
/*                  ch          Character to be located                       */
/* Return Values:                                                             */
/*                  Each of these functions returns a pointer to the last     */
/*                  occurrence of ch in string, or NULL if ch is not found.   */
/*                                                                            */
/******************************************************************************/
TCHAR *SUL_Strrchr (
    const TCHAR *string,
    INT32 ch
)
{
    TCHAR *start = (TCHAR *)string;

    while (*string++)                       /* find end of string */
        ;
    /* search towards front */
    while (--string != start && *string != (TCHAR)ch)
        ;

    if (*string == (TCHAR)ch)                /* TCHAR found ? */
        return( (TCHAR *)string );

    return (NULL);
}

/******************************************************************************/
//Location
/******************************************************************************/

/******************************************************************************/
/* Function name:    SUL_CharLower                                            */
/* Description:                                                               */
/*                   把字符串s中的所有字符都设置成小写。                      */
/* Parameters:                                                                */
/*                  string  The string to be set to lower                     */
/* Return Values:                                                             */
/*                  返回指向源字符串的指针。                                  */
/******************************************************************************/
PSTR SUL_CharLower(PSTR string)
{
    PSTR pTmp = string;
    if(*pTmp != '\0')
    {
        while(*pTmp != '\0')
        {
            if(SUL_isascii(*pTmp))
            {
                if(65 <= *pTmp && *pTmp <= 90)
                {
                    //*pTmp = (TCHAR)(*pTmp + (('a') - ('A')));
                    *pTmp = (TCHAR)(*pTmp + 32);
                }
                pTmp ++;
            }
            else
            {
                pTmp ++;
            }
        }
    }
    return string;
}

/******************************************************************************/
/* Function name:    SUL_CharLowerBuff                                        */
/* Description:                                                               */
/*                   把字符串s中的所有字符都设置成小写。                      */
/* Parameters:                                                                */
/*                  string  The string to be set to lower                     */
/* Return Values:                                                             */
/*                  返回指向源字符串的指针。                                  */
/******************************************************************************/
PSTR SUL_CharLowerBuff(
    PSTR string,
    UINT32 Count
)
{
    PSTR pTmp = string;
    if(*pTmp != '\0')
    {
        while(*pTmp != '\0' && Count--)
        {
            if(SUL_isascii(*pTmp))
            {
                if(65 <= *pTmp && *pTmp <= 90)
                {
                    //*pTmp = (TCHAR)(*pTmp + (('a') - ('A')));
                    *pTmp = (TCHAR)(*pTmp + 32);
                }
                pTmp ++;
            }
            else
            {
                pTmp ++;
            }
        }
    }
    return string;
}

/******************************************************************************/
/* Function name:    SUL_CharUpper                                            */
/* Description:                                                               */
/*                   把字符串s中的所有字符都设置成大写。                      */
/* Parameters:                                                                */
/*                  string  The string to be set to upper                     */
/* Return Values:                                                             */
/*                  返回指向源字符串的指针。                                  */
/******************************************************************************/
PSTR SUL_CharUpper(PSTR string)
{
    PSTR pTmp = string;
    if(*pTmp != '\0')
    {
        while(*pTmp != '\0')
        {
            if(SUL_isascii(*pTmp))
            {
                if(97 <= *pTmp && *pTmp <= 122)
                {
                    //*pTmp = (TCHAR)(*pTmp + (('a') - ('A')));
                    *pTmp = (TCHAR)(*pTmp - 32);
                }
                pTmp ++;
            }
            else
            {
                pTmp ++;
            }
        }
    }
    return string;
}

/******************************************************************************/
/* Function name:    CharUpperBuff                                            */
/* Description:                                                               */
/*                   把字符串s中的所有字符都设置成大写。                      */
/* Parameters:                                                                */
/*                  string  The string to be set to upper                     */
/* Return Values:                                                             */
/*                  返回指向源字符串的指针。                                  */
/******************************************************************************/
PSTR SUL_CharUpperBuff(
    PSTR string,
    UINT32 Count
)
{
    PSTR pTmp = string;
    if(*pTmp != '\0')
    {
        while(*pTmp != '\0' && Count--)
        {
            if(SUL_isascii(*pTmp))
            {
                if(97 <= *pTmp && *pTmp <= 122)
                {
                    //*pTmp = (TCHAR)(*pTmp + (('a') - ('A')));
                    *pTmp = (TCHAR)(*pTmp - 32);
                }
                pTmp ++;
            }
            else
            {
                pTmp ++;
            }
        }
    }
    return string;
}
/******************************************************************************/
/* Function name:    SUL_CharPrev                                             */
/* Description:                                                               */
/*                    returns a pointer to the preceding character in a string*/
/* Parameters:                                                                */
/*           lpszStart  Pointer to the beginning of the null-terminated string*/
/*           lpszCurrent  Pointer to a character in a null-terminated string. */
/* Return Values:                                                             */
/*                  A pointer to the preceding character in the string, or    */
/*                  to the first character in the string if the               */
/*                  lpszCurrent parameter equals the lpszStart parameter,     */
/*                  indicates success.                                        */
/******************************************************************************/
TCHAR *SUL_CharPrev(
    const TCHAR *lpszStart,
    const TCHAR *lpszCurrent
)
{
    //TCHAR * StartTmp = (TCHAR *)lpszStart;
    TCHAR *CurrentTmp = (TCHAR *)lpszCurrent;
    if(lpszStart == lpszCurrent)
    {
        return (TCHAR *)lpszStart;
    }
    if(*(--CurrentTmp) >= 0x80)
    {
        return(--CurrentTmp);
    }
    else
    {
        return(CurrentTmp);
    }

}

/******************************************************************************/
/* Function name:    SUL_CharNext                                             */
/* Description:                                                               */
/*                    returns a pointer to the next  character in a string    */
/* Parameters:                                                                */
/*           lpsz  Pointer to a character in a null-terminated string.        */
/* Return Values:                                                             */
/*                  A pointer to the next  character in the string, or        */
/*                  to the terminating null character  in the string if the   */
/*                  lpsz points to the terminating null character,            */
/*                  the return value is equal to lpsz.                        */
/******************************************************************************/

TCHAR *SUL_CharNext(
    const TCHAR *lpsz
)
{
    TCHAR *Tmp = (TCHAR *)lpsz;
    if(*Tmp == '\0')
    {
        return Tmp;
    }
    else
    {
        if(*lpsz < 0x80)
        {
            Tmp = Tmp + 1;
            return (Tmp);
        }
        else
        {
            return(Tmp + 2);
        }
    }
}
/******************************************************************************/
/* Function name:    SUL_IsCharAlpha                                          */
/* Description:                                                               */
/*                    Determines whether a character is an alphabetic character*/
/******************************************************************************/
BOOL IsCharAlpha(
    TCHAR ch
)
{
    if((97 <= ch && ch <= 122) || (65 <= ch && ch <= 90))
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* Function name:    SUL_IsCharAlphaNumeric                                   */
/* Description:                                                               */
/*                    Determines whether a character is either an alphabetic  */
/*                      or a numeric character.                               */
/******************************************************************************/
BOOL IsCharAlphaNumeric(
    TCHAR ch
)
{
    if((97 <= ch && ch <= 122) || (65 <= ch && ch <= 90) || (48 <= ch && ch <= 57))
    {
        return TRUE;
    }
    return FALSE;
}
/******************************************************************************/
/* Function name:    SUL_IsCharLower                                          */
/* Description:                                                               */
/*                    This function determines whether a character is lowercase*/
/******************************************************************************/
BOOL IsCharLower(
    TCHAR ch
)
{
    if(97 <= ch && ch <= 122)
    {
        return TRUE;
    }
    return FALSE;
}
/******************************************************************************/
/* Function name:    SUL_IsCharUpper                                          */
/* Description:                                                               */
/*                    This function determines whether a character is uppercase*/
/******************************************************************************/
BOOL IsCharUpper(
    TCHAR ch
)
{
    if(65 <= ch && ch <= 90)
    {
        return TRUE;
    }
    return FALSE;
}
/******************************************************************************/
/* Function name:    SUL_CountChar                                            */
/* Description:                                                               */
/*                    returns the number of the total  character in a string  */
/******************************************************************************/

INT32 SUL_CountChar(
    const TCHAR *pStrStart
)
{
    TCHAR *pTmp = (TCHAR *)pStrStart;
    INT32 TotalNum = 0;
    while(*pTmp != 0x00)
    {
        if(*pTmp == 0x00)
        {
            return TotalNum;
        }
        if(*pTmp < 0x80)
        {
            TotalNum++;
            pTmp++;
        }
        else
        {
            if(*(++pTmp) == '\0')
            {
                return( -1 );
            }
            pTmp++;
            TotalNum++;
        }
    }
    return TotalNum;

}


/******************************************************************************/
/* Function name:    SUL_SeekCodeTable                                        */
/* Description:                                                               */
/*                   Seek the special value in code table                     */
/******************************************************************************/
BOOL SUL_SeekCodeTable(HRES hdata, INT32 src, UINT8 srcByte, UINT16 *pNum)
{
    RFM_FILE_INFO fileHrd;
    INT32 start = 0, end , mid;
    UINT8 *pTmp;

    SUL_ZeroMemory32(&fileHrd, SIZEOF(RFM_FILE_INFO));

#ifndef MF32_BASE_REG_DISABLE
    RFM_GetFileInfo(hdata, 0, &fileHrd);
#endif

    pTmp = (UINT8 *)fileHrd.pFileData;

    end = (fileHrd.size) / srcByte;
    //end = 7445;
    mid = (start  + end ) / 2;
    if(end < 0)
    {
        *pNum = (UINT16) - 1;
        return(-1);
    }
    if(NULL != (void *)hdata)
    {
        while(start < end)
        {
            mid = (start  + end ) / 2;
            if(srcByte == 1)
            {
                if((*(pTmp + mid )) < src)
                {
                    start = mid + 1;
                    //mid = (start + end) / 2;
                }
                else if((*(pTmp + mid )) > src)
                {
                    end = mid - 1;
                    //mid = (start + end) / 2;
                }
                else
                {
                    //*pDes = (unsigned short)(*(pTmp + 2 * mid +1) + 0x8080);
                    break;
                }
            }
            if(srcByte == 2)
            {
                if((*((UINT16 *)pTmp + mid )) < (UINT16)src)
                {
                    start = mid + 1;
                    //mid = (start + end) / 2;
                }
                else if((*((INT16 *)pTmp + mid )) > (UINT16)src)
                {
                    end = mid;
                    //mid = (start + end) / 2;
                }
                else
                {
                    //*pDes = (unsigned short)(*(pTmp + 2 * mid +1) + 0x8080);
                    break;
                }
            }
            if(srcByte == 4)
            {
                if((*((UINT32 *)pTmp + mid )) < (UINT32)src)
                {
                    start = mid + 1;
                    //mid = (start + end) / 2;
                }
                else if((*((UINT32 *)pTmp + mid )) > (UINT32)src)
                {
                    end = mid;
                    //mid = (start + end) / 2;
                }
                else
                {
                    //*pDes = (unsigned short)(*(pTmp + 2 * mid +1) + 0x8080);
                    break;
                }
            }


        }
        if(start >  end)
        {
            (*pNum) = (UINT16) - 1;
            return(-1);
        }
        else if(start == end)
        {
            if(srcByte == 1)
            {
                if((*(pTmp + mid )) == src)
                {
                    (*pNum) = (UINT16)mid;
                    return(1);
                }
                else
                {
                    (*pNum) = (UINT16) - 1;
                    return (-1);
                }
            }
            else if(srcByte == 2)
            {
                if((*((UINT16 *)pTmp + mid )) == (UINT16)src)
                {
                    (*pNum) = (UINT16)mid;
                    return(1);
                }
                else
                {
                    (*pNum) = (UINT16) - 1;
                    return (-1);
                }
            }
            else if(srcByte == 4)
            {
                if((*((UINT32 *)pTmp + mid )) == (UINT32)src)
                {
                    (*pNum) = (UINT16)mid;
                    return(1);
                }
                else
                {
                    (*pNum) = (UINT16) - 1;
                    return (-1);
                }
            }
            else
            {
                //todo..;
                //(*pNum) = (UINT16)-1;
                return (-1);
            }
        }
        else
        {
            (*pNum) = (UINT16)mid;
            return(1);
        }
    }
    else
    {
        (*pNum) = (UINT16) - 1;
        return (-1);
    }

}


/******************************************************************************/
/* Function name:    SUL_IsStringAscii                                        */
/* Description:                                                               */
/*                    Determine weather the string is ascii string            */
/******************************************************************************/

BOOL SUL_IsStringAscii(
    const TCHAR *pStrStart
)
{
    if(SUL_Strlen(pStrStart) == (UINT32)SUL_CharacterLen(pStrStart, \
            (UINT16)SUL_Strlen(pStrStart)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



UINT8 *
SUL_Itoa(INT32 value, UINT8 *string, INT32 radix)
{
    UINT8 tmp[33];
    UINT8 *tp = tmp;
    INT32 i;
    UINT32  v;
    INT32 sign;
    UINT8 *sp;

    if (radix > 36 || radix <= 1)
    {
        return 0;
    }

    sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (UINT32)value;
    while (v || tp == tmp)
    {
        i = v % radix;
        v = v / radix;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    if (string == 0)
        string = (UINT8 *)CSW_SUL_MALLOC((tp - tmp) + sign + 1);
    sp = string;

    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;
    return string;
}


UINT8 *
SUL_Ltoa(INT32 value, UINT8 *string, INT32 radix)
{
    UINT8 tmp[33];
    UINT8 *tp = tmp;
    INT32 i;
    UINT32 v;
    INT32 sign;
    UINT8 *sp;

    if (radix > 36 || radix <= 1)
    {
        return 0;
    }

    sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned long)value;
    while (v || tp == tmp)
    {
        i = v % radix;
        v = v / radix;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    if (string == 0)
        string = (UINT8 *)CSW_SUL_MALLOC((tp - tmp) + sign + 1);
    sp = string;

    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;
    return string;
}

VOID SUL_UtoBinString(UINT8 *string, UINT8 value)
{
    UINT8 i = 0;
    while(i < 8)
    {
        string[7-i] = ((value>>i)&0x01) + '0';
	 i++;
    }
}



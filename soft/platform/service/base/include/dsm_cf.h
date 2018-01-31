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

#ifndef _CII_DSM_CF_H_
#define _CII_DSM_CF_H_

#ifdef _FS_SIMULATOR_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include<unistd.h>
#include <errorcode.h>
#include <assert.h>
#endif
#include "cs_types.h"
#include "ts.h"
#include "drv_mmc.h"
#include "drv_flash.h"
#include "dsm_dev_driver.h"
#include "dsm_config.h"
#include "dsm_dbg.h"
#else
#include <cswtype.h>
#include <sul.h>
#include <ts.h>
#include <ds.h>/*for DS_ASSERT*/
#include <base_prv.h>
#include <tm.h>
#include <csw.h>
#include <csw_ver.h>
#include <drv_flash.h>
#include "dsm_dbg.h"
#include "drv_mmc.h"
#include "dsm_dev_driver.h"
#include "dsm_config.h"
#include "tgt_dsm_cfg.h"
#include "dbg.h"
#include "errorcode.h"
#endif



#if defined(DSM_SHELL_COMMAND) || defined(VDS_SHELL_COMMAND) || defined(FAT_SHELL_COMMAND)
#include "vds_shell.h"
#include "dsm_shell.h"
#endif

#ifdef _DSM_SIMUFLASH_FILE
#include "flash_simu_file.h"
#endif

#ifdef _USERGEN_SHELL
//#include "usrgen.h"
#endif

#include "vds_api.h"

#ifdef _FS_SIMULATOR_
//#define MMI_TS_ID              6
//#define BASE_FS_TS_ID  (MMI_TS_ID + 1)
//#define BASE_FFS_TS_ID (MMI_TS_ID + 1)
//#define BASE_VDS_TS_ID (MMI_TS_ID + 2)
//#define BASE_DSM_TS_ID (MMI_TS_ID + 3)
//#define BASE_FAT_TS_ID (MMI_TS_ID + 4)
//#define CFW_ML_TS_ID   (MMI_TS_ID + 5)
//#define C_DETAIL (1<<8)

#define LEN_FOR_NULL_CHAR 1


//#define DSM_ASSERT(ex) assert(ex)
VOID DSM_Assert(BOOL ex, CONST CHAR *format, ...);
//INT32 CSW_TRACE(UINT32 id, CONST INT8 *fmt, ...);
char *strupr2(char *s);
char *strlwr2(char *s);
//char *DSM_StrUpr(char* str) ;
//char *DSM_StrLwr(char* str) ;

#define DSM_ASSERT DSM_Assert
#define DSM_Sleep(obj)

#define DSM_MAlloc                          malloc
#define DSM_Free                            free


#define DSM_MemSet                          memset
#define DSM_MemCpy                          memcpy
#define DSM_MemCmp                          memcmp
#define DSM_MemZero(str,len)                memset((char*)(str),0,(len));
#define DSM_MemChr                             memchr

#define DSM_StrLen(str)                     strlen((char*)str)
#define DSM_StrCpy(dst,src)                 strcpy((char*)dst,(char*)src)
#define DSM_StrNCpy(dst,src,size)           strncpy((char*)dst,(char*)src,(size_t)size)
#define DSM_StrCmp(str1,str2)               strcmp((char*)str1,(char*)str2)

#ifdef WIN32
#define DSM_StrNCaselessCmp(str1,str2,size) _strnicmp((char*)str1,(char*)str2,(size_t)size)
#define DSM_StrCaselessCmp(str1,str2)       _stricmp((char*)str1,(char*)str2)
#define DSM_StrNCmp(str1,str2,size)         strncmp((char*)str1,(char*)str2,(size_t)size)
#define DSM_StrVPrint                                  _vsnprintf
#else
#define DSM_StrNCaselessCmp(str1,str2,size) strncasecmp((char*)str1,(char*)str2,(size_t)size)
#define DSM_StrCaselessCmp(str1,str2)       strcasecmp((char*)str1,(char*)str2)
#define DSM_StrNCmp(str1,str2,size)         strncmp((char*)str1,(char*)str2,(size_t)size)
#define DSM_StrVPrint                                  vsnprintf
#endif

#define DSM_StrChr(str,ch)                  strchr((char*)str,(int)ch)
#define DSM_StrRChr(str,ch)                 strrchr((char*)str,(int)ch)
#define DSM_StrUpr(str)                       strupr2(str)
#define DSM_StrLwr(str)                       strlwr2(str)
#define DSM_StrCat(dst, src)                strcat((char*)dst,(char*)src)
#define DSM_StrNCat(dst,src,count)          strncat((char*)dst,(char*)src,(size_t)count)
#define DSM_StrAToI(str)                         atoi((const char*)str)
#define DSM_StrIToA                         itoa
#define DSM_U64ToA                         ui64toa
#define DSM_StrPrint                         sprintf
#define DSM_GetRandNum                      rand
#define DSM_Time(ptr)                      time(ptr)
#define DSM_StrUpper(pstr)                strupr2(pstr) // _strupr((char*)pstr)
#define DSM_StrStr                          strstr

#define  FS_TIME(ptr)                       time(ptr)
#define sxr_Sleep(x)     // exit()
#define sxr_NewSemaphore(obj)       1
#define sxr_TakeSemaphore(a)
#define sxr_ReleaseSemaphore(a)
#define sxr_FreeSemaphore(a)
extern UINT32 ML_Unicode2LocalLanguage(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12]);
extern UINT32 ML_LocalLanguage2UnicodeBigEnding(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset1[12]);
extern VOID CSW_TC_MEMBLOCK(UINT16 uMask, UINT8 *buffer, UINT16 len, UINT8 radix);
#define DM_Reset() //exits(1)
#define hal_HstSendEvent(x) printf("event:0x%x\n",x)

#define hal_EnterCriticalSection() (0)
#define hal_SysExitCriticalSection(a)
#define hal_ComregsClrMask(a,b)

#define HAL_COMREGS_XCPU_IRQ_SLOT_0 0
#define HAL_COMREGS_XCPU_IRQ_SLOT_1 1
#define hal_ComregsGetMask(x) (x)
#define hal_FintIrqSetMask(x) //exits(1)
#else

#ifdef FS_PRIVATE_MEM_MANAGE
#define DSM_MAlloc private_malloc
#define DSM_Free private_free
void get_mega_kilo_byte( int n, int *mega, int *kilo, int *mybyte );
void *private_malloc( UINT32 size );
void private_free( void *ptr );
#else
#define DSM_MAlloc(obj)                     CSW_MALLOC(BASE_DSM_TS_ID, (obj))
#define DSM_Free(obj)                       CSW_FREE(BASE_DSM_TS_ID, (PSTR)(obj))
#endif
#define DSM_MemSet                          SUL_MemSet8
#define DSM_MemCpy                          SUL_MemCopy8
#define DSM_MemCmp                          SUL_MemCompare
#define DSM_MemZero                         SUL_ZeroMemory8
#define DSM_MemChr                             SUL_MemChr

#define DSM_StrLen(str)                     SUL_Strlen((PCSTR)(str))
#define DSM_StrCpy(dst,src)                 SUL_StrCopy((PSTR)(dst),(PCSTR)(src))
#define DSM_StrNCpy(dst,src,size)           SUL_StrNCopy((PSTR)(dst),(PCSTR)(src), (size))
#define DSM_StrCmp(str1,str2)               SUL_StrCompare((PCSTR)(str1),(PCSTR)(str2))
#define DSM_StrNCmp(str1,str2,size)         SUL_StrNCompare((PCSTR)(str1),(PCSTR)(str2), (size))
#define DSM_StrCaselessCmp(str1,str2)       SUL_StrCaselessCompare((PCSTR)str1,(PCSTR)str2)
#define DSM_StrNCaselessCmp(str1,str2,size) SUL_StrNCaselessCompare((PCSTR)str1,(PCSTR)str2,(UINT32)size)
#define DSM_StrChr(str,ch)                  SUL_StrChr((PSTR)str,(INT32)ch)
#define DSM_StrRChr(str,ch)                 strrchr2((PSTR)str,(INT32)ch)
#define DSM_StrUpr(str)                     SUL_CharUpper((PSTR)str)
#define DSM_StrCat(dst, src)                SUL_StrCat((PSTR)(dst),(PCSTR)(src))
#define DSM_StrNCat(dst,src,count)          SUL_StrNCat((PSTR)dst,(PCSTR)src,(UINT32)count)
#define DSM_StrIToA                         itoa
#define DSM_U64ToA                         ui64toa
#define DSM_StrAToI                         SUL_StrAToI
#define DSM_StrPrint                        SUL_StrPrint
#define DSM_StrVPrint                       SUL_StrVPrint
#define  DSM_Time(ptr)                      TM_GetTime()
#define  FS_TIME(ptr)                       TM_GetTime()
UINT32 DSM_GetRandNum(VOID);
extern void sxr_Sleep( UINT32 );
extern BOOL hal_HstSendEvent(UINT32 ch);
extern VOID hal_DbgAssert(CONST CHAR *format, ...);
#define DSM_Sleep(obj) sxr_Sleep(obj)
UINT8 *itoa( INT32 value, UINT8 *string, UINT32 radix );
#define DSM_Assert(bl, format, ...)         \
{if (!(bl)) {                                                            \
    hal_DbgAssert(format, ##__VA_ARGS__);       \
}}
#define DSM_ASSERT DSM_Assert
#endif //_FS_SIMULATOR_

#if defined(_FS_DEBUG)
#if defined(_USERGEN_SHELL)
#define DSM_HstSendEvent(x)  // printf("SendEvent:0x%x",x)
#else
#define DSM_HstSendEvent(x)  hal_HstSendEvent(x)
#endif
#else
#define DSM_HstSendEvent(x)  
#endif

#ifndef _T_DSM
#define  _T_DSM(x)         (x)
#endif

#ifndef _L_DSM
#define  _L_DSM(x)         L##x
#endif

#ifndef PCWSTR
#define PCWSTR UINT16*
#endif

#ifndef PWSTR
#define PWSTR UINT16*
#endif

#ifndef WCHAR
#define WCHAR UINT16
#endif

UINT32 tcslen(PCWSTR str);
int tcscmp(PCWSTR s1, PCWSTR s2);
int tcsncmp(PCWSTR s1, PCWSTR s2, UINT32 n);
WCHAR *tcscpy(PWSTR to, PCWSTR from);
WCHAR *tcsncpy(PWSTR dst, PCWSTR src, UINT32 n);
WCHAR *tcscat(PWSTR s, PCWSTR append);
WCHAR *tcsncat(PWSTR dst, PCWSTR src, UINT32 len);
WCHAR *tcschr(PCWSTR p, WCHAR ch);
WCHAR *tcsrchr(PCWSTR p, WCHAR ch);
WCHAR *tcsupr(PWSTR p);


#define DSM_TcStrCpy                    tcscpy
#define DSM_TcStrNCpy                   tcsncpy
#define DSM_TcStrChr                    tcschr
#define DSM_TcStrCmp                    tcscmp
#define DSM_TcStrNCmp                   tcsncmp
#define DSM_TcStrLen                    tcslen
#define DSM_TcStrCat                    tcscat
#define DSM_TcStrUpr                    tcsupr
#define DSM_Unicode2OEM ML_Unicode2LocalLanguage
#define DSM_OEM2Uincode ML_LocalLanguage2UnicodeBigEnding
#ifdef WIN32
#define ERR_INVALID_HANDLE 0x03
#define ERR_BUSY_DEVICE    0x0c
#define ERR_SUCCESS 0
#endif
UINT16 DSM_GetTime(VOID);
UINT16 DSM_GetData(VOID);
UINT8   DSM_GetTimeTeenth(VOID);
UINT32 DSM_UniteFileTime(UINT16 iDate,UINT16 iTime,UINT8 iTimeTeenth);
UINT32 DSM_GetFileTime(VOID);
UINT16 DSM_GetFileTime2Date(UINT32 iFileTime);
UINT16 DSM_GetFileTime2Time(UINT32 iFileTime);
UINT16 DSM_UnicodeLen(UINT8 *pszUnicode);
UINT8 *ui64toa(UINT64  value, UINT8 *string, UINT32 radix);
VOID DSM_Dump(UINT8 *pBuff, UINT32 iTotalSize, INT32 iLineSize);
void DSM_Hex2Str(UINT8 *hex, INT32 len, UINT8 *bufout);
// Error Code Base for VC
#endif // _CII_VDS_CF_H_


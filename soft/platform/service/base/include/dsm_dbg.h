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

#ifndef _DSM_DBG_H_
#define _DSM_DBG_H_


#define _DSM_COPYRIGHT_INFO_ "Copyright(c) 2003-2005 by Coolsand Technologies, Inc"


#define DSM_MOD_NR             5
#define NR_SUBMODULE           DSM_MOD_NR

// DSM Module id.
typedef enum
{
    DSM_MOD_DL = 0x0,
    DSM_MOD_VDS = 0x1,
    DSM_MOD_FAT = 0x2,
    DSM_MOD_VFS = 0x4,
    DSM_MOD_REG = 0x8,
    DSM_MOD_SMS = 0x10,
    DSM_MOD_NVRAM = 0x20,
} DSM_MOD;

#define DSM_ERROR                0x8
#define DSM_WARNING           0x4
#define DSM_BRIEF                 0x2
#define DSM_DETAIL               0x1

#define DL_ERROR                  ((DSM_MOD_DL <<16) |DSM_ERROR)
#define DL_WARNING             ((DSM_MOD_DL <<16) |DSM_WARNING)
#define DL_BRIEF                   ((DSM_MOD_DL <<16) |DSM_BRIEF)
#define DL_DETAIL                 ((DSM_MOD_DL <<16) |DSM_DETAIL)

#define DL_VDSERROR            ((DSM_MOD_VDS <<16) |DSM_ERROR)
#define DL_VDSWARNING       ((DSM_MOD_VDS <<16) |DSM_WARNING)
#define DL_VDSBRIEF             ((DSM_MOD_VDS <<16) |DSM_BRIEF)
#define DL_VDSDETAIL           ((DSM_MOD_VDS <<16) |DSM_DETAIL)

#define DL_FATERROR            ((DSM_MOD_FAT <<16) |DSM_ERROR)
#define DL_FATWARNING       ((DSM_MOD_FAT <<16) |DSM_WARNING)
#define DL_FATBRIEF             ((DSM_MOD_FAT <<16) |DSM_BRIEF)
#define DL_FATDETAIL           ((DSM_MOD_FAT <<16) |DSM_DETAIL)

#define DL_VFSERROR            ((DSM_MOD_VFS <<16) |DSM_ERROR)
#define DL_VFSWARNING       ((DSM_MOD_VFS <<16) |DSM_WARNING)
#define DL_VFSBRIEF             ((DSM_MOD_VFS <<16) |DSM_BRIEF)
#define DL_VFSDETAIL           ((DSM_MOD_VFS <<16) |DSM_DETAIL)


#define DL_REGERROR            ((DSM_MOD_REG <<16) |DSM_ERROR)
#define DL_REGWARNING       ((DSM_MOD_REG <<16) |DSM_WARNING)
#define DL_REGBRIEF             ((DSM_MOD_REG <<16) |DSM_BRIEF)
#define DL_REGDETAIL           ((DSM_MOD_REG <<16) |DSM_DETAIL)

#define DL_SMSDMERROR        ((DSM_MOD_SMS <<16) |DSM_ERROR)
#define DL_SMSDMWARNING   ((DSM_MOD_SMS <<16) |DSM_WARNING)
#define DL_SMSDMBRIEF         ((DSM_MOD_SMS <<16) |DSM_BRIEF)
#define DL_SMSDMDETAIL       ((DSM_MOD_SMS <<16) |DSM_DETAIL)

#define DL_NVRAMERROR         ((DSM_MOD_NVRAM <<16) |DSM_ERROR)
#define DL_NVRAMWARNING    ((DSM_MOD_NVRAM <<16) |DSM_WARNING)
#define DL_NVRAMBRIEF          ((DSM_MOD_NVRAM <<16) |DSM_BRIEF)
#define DL_NVRAMDETAIL        ((DSM_MOD_NVRAM <<16) |DSM_DETAIL)

#define VDS_DEBUG_BUF_SIZE                 512

typedef struct _dsm_mode_gdb
{
    UINT8 szModName[16];
    DSM_MOD iModId;
    UINT16 iLevel;
} DSM_MODE_GDB;

#ifdef _FS_SIMULATOR_

#define __FUNCTION__ __FILE__
#define CSW_PROFILE_FUNCTION_EXIT(fp)
#define CSW_PROFILE_FUNCTION_ENTER(fp)
#define _ASSERT_(x)     assert(x)
/*
#define SUL_SetLastError( errno )  \
do { \
    wfcheat = FALSE;     \
    g_dsm_errno = errno; \
}while ( wfcheat )
*/
int dsm_Printf( int index, const char *format, ... );
#else
#define _ASSERT_(x)  DS_ASSERT(x)
#define dsm_Printf CSW_TRACE

#endif

#ifndef CSW_NO_TRACE
#ifdef WIN32
VOID dsm_Debugout( UINT32 ulLevel, CONST UINT8 *szFormat, ... );
#define D( x )         dsm_Debugout x
#else
VOID dsm_Debugout( UINT32 ulLevel, CONST UINT8 *szFormat, ... );
#define DSM_DEBUGOUT(ID, format, ...)  // dsm_Debugout(ID,(UINT8*)format, ##__VA_ARGS__)
#define D( x )    DSM_DEBUGOUT x
#endif

#else
// VOID dsm_Debugout( UINT32 ulLevel, CONST UINT8* szFormat, ... );
// #define DSM_DEBUGOUT(ID, format, ...)  dsm_Debugout(ID,(UINT8*)format, ##__VA_ARGS__)
#define D( x )    //DSM_DEBUGOUT x

#endif




VOID DSM_GetVersion( INT32 iModId, CHAR *pVersion );


#endif


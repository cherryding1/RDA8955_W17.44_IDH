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

#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "fs.h"
#include "sms_dm.h"
#ifdef _FS_SIMULATOR_
INT32 g_dsm_errno;
BOOL  wfcheat = FALSE;
#endif

UINT32 g_dsm_DbgSwtich[ NR_SUBMODULE ] = {1, 1, 1, 1, 1};
UINT32 g_dsm_DbgLevel[ NR_SUBMODULE ] = {DL_ERROR, DL_ERROR, DL_ERROR, DL_ERROR, DL_ERROR};


DSM_MODE_GDB g_dsmGbdLevel[] =
{
    {"DL", DSM_MOD_DL, DL_ERROR},
    {"VDS", DSM_MOD_VDS, DL_ERROR},
    {"FAT", DSM_MOD_FAT, DL_ERROR},
    {"VFS", DSM_MOD_VFS, DL_ERROR},
    {"NVRAM", DSM_MOD_NVRAM, DL_ERROR}
};


#ifdef _FS_SIMULATOR_
int dsm_Printf( int index, const char *format, ... )
{
    /*
    这里假设展开后的字符串的长度不超过1024.
    例如: dsm_Printf( index, "%s %x", "gaokejun", 0xDEADBEEF );
    则展开后的字符串是"gaokejun DEADBEEF".
    */
    UINT8 cTemp[ 1024 + LEN_FOR_NULL_CHAR ];
    va_list ap;

    index = 0;
    va_start( ap, format );
    ( void ) vsprintf( (char *)cTemp, format , ap );
    va_end( ap );

    printf( "%s", cTemp );
    return 0;
}
#endif


UINT8 g_dsm_DbgBuf[ VDS_DEBUG_BUF_SIZE ];

VOID dsm_Debugout( UINT32 ulLevel, CONST UINT8 *szFormat, ... )
{
    va_list va_format;
    UINT32 lLen = 0;
    UINT32 iIndex = 0;
    UINT32 iCount = 0;
    UINT32 iModeId = 0;
    UINT32 iLevel = 0;

    iModeId = (ulLevel >> 16);
    iLevel = (ulLevel & 0xffff);
    iCount = sizeof(g_dsmGbdLevel) / sizeof(DSM_MODE_GDB);
    for(iIndex = 0; iIndex < iCount; iIndex++)
    {
        if((UINT32)g_dsmGbdLevel[iIndex].iModId == iModeId )
        {
            break;
        }
    }
    if(iIndex == iCount || iLevel < g_dsmGbdLevel[iIndex].iLevel)
    {
        return;
    }

    DSM_MemSet( g_dsm_DbgBuf, 0, VDS_DEBUG_BUF_SIZE );

    va_start( va_format, szFormat );
#ifdef _FS_SIMULATOR_

    lLen = ( long ) DSM_StrVPrint( (char *)g_dsm_DbgBuf, ( VDS_DEBUG_BUF_SIZE - 1 ), (char *)szFormat, va_format );
#else

    lLen = ( long ) DSM_StrVPrint( g_dsm_DbgBuf, szFormat, va_format );
#endif

    if ( lLen < 0 )
    {
        va_end( va_format );
        return ;
    }
    va_end( va_format );


    switch (iLevel)
    {
        case DSM_ERROR:
            dsm_Printf( BASE_DSM_TS_ID, "%s: [ERROR] %s\r\n",  g_dsmGbdLevel[iIndex].szModName, g_dsm_DbgBuf );
            break;
        case DSM_WARNING:
            dsm_Printf( BASE_DSM_TS_ID, "%s: [WARNING] %s\r\n", g_dsmGbdLevel[iIndex].szModName, g_dsm_DbgBuf );
            break;
        case DSM_BRIEF:
            dsm_Printf( BASE_DSM_TS_ID, "%s: [brief] %s\r\n", g_dsmGbdLevel[iIndex].szModName, g_dsm_DbgBuf );
            break;
        case DSM_DETAIL:
            dsm_Printf( BASE_DSM_TS_ID, "%s: [detail] %s\r\n", g_dsmGbdLevel[iIndex].szModName, g_dsm_DbgBuf );
            break;
        default:
            break;
    }
    return ;
}
#ifdef _T_UPGRADE_PROGRAMMER
VOID DSM_GetVersion( INT32 iModId, CHAR *pVersion )
{
    DSM_StrCpy(pVersion, "0x0");
}

#else


/* It is the caller's reponsibility to allocate memory for pVersion */
VOID DSM_GetVersion( INT32 iModId, CHAR *pVersion )
{

    switch ( iModId )
    {
        case DSM_MOD_VDS:
            DSM_StrPrint(pVersion, "0x%x", (unsigned int)VDS_GetVersion());
            break;
        case DSM_MOD_FAT:
            DSM_StrPrint(pVersion, "0x%x", (unsigned int)FS_GetVersion() & 0xffff0000);
            break;

        case DSM_MOD_VFS:
            DSM_StrPrint(pVersion, "0x%x", (unsigned int)FS_GetVersion() & 0xffff);
        default:
            break;
    }

    return;
}

#endif

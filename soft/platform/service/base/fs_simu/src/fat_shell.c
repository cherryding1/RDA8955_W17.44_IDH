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

#ifdef DSM_SHELL_COMMAND
#include "dsm_cf.h"
#include "fat_base.h"
#include "exfat_base.h"
#include "fat_local.h"
#include "exfat_local.h"
#include "fs.h"
#include "fs_base.h"
#include "tupgrade.h"
#include "nvram.h"
#define FS_SHELL_BUFFER_LEN_MAX 1024
#define FFS_SLEEP_VALUE   10
#define LEN_FOR_OEM_UNI_BUFFER      (FS_FILE_NAME_UNICODE_LEN + 2)

typedef struct _demo_file_info
{
    char filename[LEN_FOR_OEM_UNI_BUFFER];
    unsigned int Attributes;
    struct _demo_file_info *next;
} DEMO_FILE_INFO;


char g_lwrite_buf[ VDS_SHELL_WRITE_BUFFER_LEN_MAX ];



extern char g_shell_buffer[ ];
extern char g_lwrite_buf[];

INT32 g_fs_has_initiated = FALSE;

#ifdef _FS_SIMULATOR_
extern INT32 g_dsm_errno;
#endif

DEMO_FILE_INFO *g_fssim_sort_file_list = NULL;
UINT32 g_ffsim_sort_file_count = 0;

INT8 UniStrCmp_IgnoreHAndL(const UINT16 *str1, const UINT16 *str2)

{
    UINT16 ch1, ch2;
    UINT16 len, len1, len2;

    if(str1 == 0 || str2 == 0)
    {
        return 0;
    }
    while(str1)
    {
        if(*str1 <= 0x20)
        {
            str1 ++;
        }
        else
        {
            break;
        }
    }

    while(str2)
    {
        if(*str2 <= 0x20)
        {
            str2 ++;
        }
        else
        {
            break;
        }
    }

    len1 = DSM_UnicodeLen((UINT8 *)str1);
    len2 = DSM_UnicodeLen((UINT8 *)str2);
    len = (UINT16)(len1 > len2 ? len2 : len1);

    while(len > 0)
    {
        ch1 = *str1;
        ch2 = *str2;
        if (ch1 != ch2)
        {
            // change lower  to upper.
            if((ch1 & 0xff) == ch1 && (ch1 >= 'a') && (ch1  <= 'z'))
            {
                ch1 = (UINT16)(ch1  - ('a' - 'A'));
            }
            if((ch2 & 0xff) == ch2 && (ch2 >= 'a') && (ch2 <= 'z'))
            {
                ch2 = (UINT16)(ch2  - ('a' - 'A'));
            }
            return (INT8)(((ch1 - ch2) > 0 ? 1 : -1));
        }
        str1++;
        str2++;
        len --;
    }

    if(len1 > len2)
    {
        return -1;
    }
    else if(len1 < len2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void AppendFile(DEMO_FILE_INFO *p)
{

    if(!p)
    {
        return;
    }
    if(!g_fssim_sort_file_list)
    {
        g_ffsim_sort_file_count = 0;
    }
    p->next = g_fssim_sort_file_list;
    g_fssim_sort_file_list = p;
    g_ffsim_sort_file_count ++;
}

void ReleaseAllChain(void)
{
    DEMO_FILE_INFO *p = NULL;
    DEMO_FILE_INFO *pTmp = NULL;

    p = g_fssim_sort_file_list;
    while(p)
    {
        pTmp = p;
        p = pTmp->next;
        DSM_Free(pTmp);
    }
    g_fssim_sort_file_list = NULL;
    g_ffsim_sort_file_count = 0;
}

void PrintAllChain(void)
{
    DEMO_FILE_INFO *p = NULL;
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;

    p = g_fssim_sort_file_list;
    while(p)
    {
        if(pOemName)
        {
            DSM_Free(pOemName);
            pOemName = NULL;
        }
        DSM_Unicode2OEM((UINT8 *)p->filename, DSM_UnicodeLen((UINT8 *)p->filename), &pOemName, &uOemLen, NULL);
        dsm_Printf(BASE_VDS_TS_ID, "#debug sort>attrib = 0x%x,filename = %s.\n", p->Attributes, pOemName);
        if(pOemName)
        {
            DSM_Free(pOemName);

        }
        pOemName = NULL;
        p = p->next;

    }
}



#define FSSIM_FILE_INFO_NODE DEMO_FILE_INFO
#define COS_MALLOC malloc
#define COS_FREE free
void SortByName_1(UINT32 len)
{
#if 0
    INT32 exchange = 0;
    FSSIM_FILE_INFO_NODE **node = NULL;
    FSSIM_FILE_INFO_NODE **dir_node = NULL;
    UINT32 dir_count = 0;
    FSSIM_FILE_INFO_NODE **file_node = NULL;
    UINT32 file_count = 0;
    FSSIM_FILE_INFO_NODE *p = NULL;
    UINT32 i = 0, j = 0;

    UINT8 *file_name = NULL;
    UINT32 f_len = 0;


    node = (FSSIM_FILE_INFO_NODE **)COS_MALLOC(SIZEOF(UINT32) * len);

    if(NULL == node)
    {
        return;
    }
    //   ML_SetCodePage("CP936");
    // get pointer list.
    p = g_fssim_sort_file_list;
    i = 0;
    while(p)
    {
        if(i >= len || p == NULL)
        {
            dsm_Printf(BASE_VDS_TS_ID, "sort name node number[%d] > len[%d],p = 0x%x.\n", i, len, p);
            break;
        }

        node[i] = (FSSIM_FILE_INFO_NODE *)p;
        iResult = ML_Unicode2LocalLanguage(p->filename, (UINT32)DSM_UnicodeLen(p->filename), &file_name, &f_len, NULL);
        if(iResult == 0)
        {
            dsm_Printf(BASE_VDS_TS_ID, "#debug SortByName[%d] file = %s.\n", i, file_name);
            if(file_name)
            {
                COS_FREE(file_name);
                file_name = NULL;
            }
        }
        else
        {
            dsm_Printf(BASE_VDS_TS_ID, "#debug SortByName[%d] file = unknown.n", i);
        }

        sxr_Sleep(5);
        p = p->next;
        i++;
    }

    // find dir list and file list.

    for(i = 0; i < len; i++)
    {
        exchange = 0;
        for(j = i; j < len - i - 1; j++)
        {
            if((node[j]->Attributes  & FS_ATTR_DIR) < (node[j + 1]->Attributes  & FS_ATTR_DIR))
            {
                p = node[j];
                node[j] = node[j + 1];
                node[j + 1] = p;
                exchange = 1;
            }
        }
        if(exchange == 0)
        {
            break;
        }
    }
    dir_node = node;
    p = node[0];

    // get dir list and file list.
    for(i = 0; i < len; i++)
    {
        if((node[i]->Attributes & FS_ATTR_DIR) == 0)
        {
            file_node = &node[i];
            break;

        }
        dir_count ++;
    }
    file_count = len - dir_count;

    // dir sort by name
    for(i = 0 ; i < dir_count; i++)
    {
        exchange = 0;
        for(j = 0; j < dir_count - i - 1; j++)
        {

            if (UniStrCmp_IgnoreHAndL((UINT16 *)dir_node[j]->filename, (UINT16 *)dir_node[j + 1]->filename) > 0)
            {
                p = dir_node[j];
                dir_node[j] = dir_node[j + 1];
                dir_node[j + 1] = p;
                exchange = 1;
            }
            else
            {
                // do nothing.
            }
        }
        if(exchange == 0)
        {
            break;
        }
    }

    // file sort by name
    for(i = 0 ; i < file_count; i++)
    {
        exchange = 0;
        for(j = 0; j < file_count - i - 1; j++)
        {

            if (UniStrCmp_IgnoreHAndL((UINT16 *)file_node[j]->filename, (UINT16 *)file_node[j + 1]->filename) > 0)
            {
                p = file_node[j];
                file_node[j] = file_node[j + 1];
                file_node[j + 1] = p;
                exchange = 1;
            }
            else
            {
                // do nothing.
            }
        }
        if(exchange == 0)
        {
            break;
        }
    }

    // Create chain.

    // add dir list to chain.
    p = *dir_node;
    if(p)
    {
        for(i = 1; i < dir_count; i++)
        {
            p->next = dir_node[i];
            p = dir_node[i]->next;
        }

        if(p)
        {
            // add file list to chain.
            for(i = 0; i < file_count; i++)
            {
                p->next = file_node[i];
                p = file_node[i]->next;
            }
            // set the last node
            if(p)
            {
                p->next = NULL;
            }
        }
    }
    g_fssim_sort_file_list = *dir_node;
    if(node)
    {
        COS_FREE(node);
    }
    return;
#else
    len = len;
#endif
}


void SortByName(INT32 len)
{

    DEMO_FILE_INFO *node1 = NULL, *node2 = NULL, *dir_list = NULL, *prenode = NULL;
    DEMO_FILE_INFO *file_list = NULL;

    int i, j;
    int dir_count = 0;
    int file_count = 0;
    int exchange = 0;

    // classify de.
    // de_chain = dir_chain + file_chain
    prenode = g_fssim_sort_file_list;
    for(i = 0; i < len; i++)
    {
        exchange = 0;
        node1 = g_fssim_sort_file_list;
        for(j = 0; j < len - i; j++)
        {

            if(node1->next == 0)
            {
                break;
            }
            node2 = node1->next;
            if((node1->Attributes & FS_ATTR_DIR) < (node2->Attributes & FS_ATTR_DIR))
            {
                if(node1 == g_fssim_sort_file_list)
                {
                    prenode = node2;
                    node1->next = node2->next;
                    node2->next = node1;
                    g_fssim_sort_file_list = node2;
                    node1 = prenode->next;

                }
                else
                {
                    prenode->next = node2;
                    node1->next = node2->next;
                    node2->next = node1;
                    prenode = node2;
                    node1 = prenode->next;
                }
                exchange = 1;
            }
            else
            {
                prenode = node1;
                node1 = node1->next;
            }
        }
        if(exchange == 0)
        {
            break;
        }

    }

    // find the file_list header.
    // accumulative total of directory number.
    dir_list = g_fssim_sort_file_list;
    while(dir_list)
    {
        if((dir_list->Attributes & FS_ATTR_DIR))
        {
            dir_count ++;
        }
        if(dir_list->next == 0)
        {
            break;
        }
        if((dir_list->next->Attributes & FS_ATTR_DIR) == 0)
        {
            file_list = dir_list->next;
            break;
        }

        dir_list = dir_list->next;
    }

    file_count = len - dir_count;

    // sort directory list by name.
    prenode = g_fssim_sort_file_list;
    for(i = 0; i < dir_count; i++)
    {
        exchange = 0;
        node1 = g_fssim_sort_file_list;
        for(j = 0; j < dir_count - i; j++)
        {

            if(node1->next == 0)
            {
                break;
            }
            node2 = node1->next;
            if (UniStrCmp_IgnoreHAndL((UINT16 *)node1->filename, (UINT16 *)node2->filename) > 0)
            {
                if(node1 == g_fssim_sort_file_list)
                {

                    prenode = node2;
                    node1->next = node2->next;
                    node2->next = node1;
                    g_fssim_sort_file_list = node2;
                    node1 = prenode->next;

                }
                else
                {
                    prenode->next = node2;
                    node1->next = node2->next;
                    node2->next = node1;
                    prenode = node2;
                    node1 = prenode->next;
                }
                exchange = 1;
            }
            else
            {
                prenode = node1;
                node1 = node1->next;
            }
        }
        if(exchange == 0)
        {
            break;
        }
    }

    // sort file list by name.
    prenode = dir_list;
    for(i = 0; i < file_count; i++)
    {
        exchange = 0;
        node1 = file_list;
        if(!node1)
        {
            break;
        }
        for(j = 0; j < file_count - i; j++)
        {

            if(node1->next == 0)
            {
                prenode->next = file_list;
                break;
            }
            node2 = node1->next;
            if(UniStrCmp_IgnoreHAndL((UINT16 *)node1->filename, (UINT16 *)node2->filename) > 0)
            {
                if(node1 == file_list)
                {
                    prenode = node2;
                    node1->next = node2->next;
                    node2->next = node1;
                    file_list = node2;
                    node1 = prenode->next;

                }
                else
                {
                    prenode->next = node2;
                    node1->next = node2->next;
                    node2->next = node1;
                    prenode = node2;
                    node1 = prenode->next;
                }
                exchange = 1;

            }
            else
            {
                prenode = node1;
                node1 = node1->next;
            }
        }
        if(exchange == 0)
        {
            break;
        }
    }
    return ;
}


//init [YES| NO] [root_dev_name] {[normal | quick]}
//YES means force format option is selected;
//NO  means force format option is not selected.
static BOOL shell_fs_init( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    INT32 iFormatType = FS_FORMAT_QUICK;

    if ( g_fs_has_initiated )
    {
        dsm_Printf( BASE_VDS_TS_ID, "Init command is not allowed to call from shell\n" );
        return FALSE;
    }

    if ( argc < 3 )
    {
        goto show_init_cmd;
    }

    if ( 4 == argc && ( 'n' == argv[ 3 ][ 0 ] || 'N' == argv[ 3 ][ 0 ] ) )
    {
        iFormatType = FS_FORMAT_NORMAL;
    }

    if ( 0 == DSM_StrCaselessCmp( "yes", argv[ 1 ] ) )
    {
        iRet = FS_Format( (UINT8 *)argv[ 2 ], FS_TYPE_FAT, (UINT8)iFormatType );
        if(ERR_SUCCESS != iRet)
        {
            dsm_Printf( BASE_DSM_TS_ID, "format [%s] failed: %d\n", argv[ 2 ], iRet );
            return FALSE;
        }
        else
        {
            dsm_Printf( BASE_DSM_TS_ID, "Format %s OK\n", argv[ 2 ] );
        }
    }
    else
    {
        iRet = FS_HasFormatted( (UINT8 *)argv[ 2 ], FS_TYPE_FAT );
        if ( ERR_FS_HAS_FORMATED != iRet )
        {
            iRet = FS_Format( (UINT8 *)argv[ 2 ], FS_TYPE_FAT, (UINT8)iFormatType );
            if(ERR_SUCCESS != iRet)
            {
                dsm_Printf( BASE_DSM_TS_ID, "format [%s] failed: %d\n", argv[ 2 ], iRet );
                return FALSE;
            }
            else
            {
                dsm_Printf( BASE_DSM_TS_ID, "Format %s OK\n", argv[ 2 ] );
            }
        }
        else
        {
            dsm_Printf( BASE_DSM_TS_ID, "%s has formated before\n", argv[ 2 ] );
        }
    }

    iRet = FS_MountRoot( (UINT8 *)argv[ 2 ] );
    if(ERR_SUCCESS != iRet)
    {
        dsm_Printf( BASE_DSM_TS_ID, "Mount Root FS on %s failed: %d", argv[ 2 ], iRet );
        return FALSE;
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "Mount Root FS on %s OK\n", argv[ 2 ] );
    }

    g_fs_has_initiated = TRUE;
    return TRUE;

show_init_cmd:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}

static BOOL shell_FS_Format( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT8 iFsType;
    UINT8 iFormatType;

    if ( 4 == argc )
    {
        if(DSM_StrCmp(argv[2], "FAT") == 0)
        {
            iFsType = 0x01;
        }
        else if(DSM_StrCmp(argv[2], "EXFAT") == 0)
        {
            iFsType = 0x03;
        }
        else
        {
            iFsType = 0x02;
        }

        iFormatType = (UINT8)DSM_StrAToI(argv[3]);
        iRet = FS_Format( (UINT8 *)argv[ 1 ], iFsType, iFormatType );
        if ( ERR_SUCCESS == iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "format succeed!\n");
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "format failed! ErrorCode: [%d]\n", iRet);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
static BOOL shell_VDS_Flush( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT32 uUniLen = 0;
    UINT8 uFsType = 0;
    UINT8 *pUniName = NULL;
    if (1 <= argc )
    {
        VDS_CacheFlushAllowed();
        do
        {
            iRet = VDS_CacheFlush();

            if ( ERR_SUCCESS != iRet )
            {
                //dsm_Printf( BASE_FAT_TS_ID, "vds flush failed: %d!\n", iRet );
                break;
            }

        }
        while(ERR_SUCCESS == iRet);

    }
    else
    {
        PRINT_CMD_USAGE( ent );
    }

    return TRUE;
}

static BOOL shell_VDS_Flush_All( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT32 uUniLen = 0;
    UINT8 uFsType = 0;
    UINT8 *pUniName = NULL;
    if (1 <= argc )
    {
        VDS_CacheFlushAllowed();
        iRet = VDS_CacheFlushAll();
        if ( ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "vds flush failed: %d!\n", iRet );
        }
        //VDS_CacheFlushAllowed();
    }
    else
    {
        PRINT_CMD_USAGE( ent );
    }

    return TRUE;
}
#endif

/*mount [FAT|FFS|MinixFS] <dev_name> <mount_point>*/
static BOOL shell_mount( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT32 uUniLen = 0;
    UINT8 uFsType = 0;
    UINT8 *pUniName = NULL;
    if ( 4 <= argc )
    {
        uFsType = (UINT8)(DSM_StrCmp(argv[1], "FAT") == 0 ? FS_TYPE_FAT : FS_TYPE_FFS);
        DSM_OEM2Uincode((UINT8 *)argv[ 3 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 3 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_Mount( (UINT8 *)argv[ 2 ], pUniName, 0, uFsType );
        if ( ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "mount failed: %d!\n", iRet );
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
    }
    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return TRUE;
}


/*mountr <dev_name>*/
static BOOL shell_mount_root( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if ( 2 == argc )
    {
        iRet = FS_MountRoot( (UINT8 *)argv[ 1 ] );
        if ( ERR_SUCCESS != iRet )
            dsm_Printf( BASE_FAT_TS_ID, "mount root fs on %s failed: %d!\n", argv[ 1 ], iRet );
    }
    else
    {
        PRINT_CMD_USAGE( ent );
    }

    return TRUE;
}


/*unmount <mount_point>*/
static BOOL shell_unmount( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    BOOL bForce = FALSE;
    UINT32 uUniLen = 0;
    UINT8 *pUniName = NULL;
    BOOL bRet = FALSE;

    if ( argc < 2 )
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    if ( argc >= 3 )
    {
        bForce = (BOOL)(DSM_StrAToI(argv[ 2 ] ) == 0 ? FALSE : TRUE);
    }

    DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
    iRet = FS_Unmount( pUniName, bForce );
    if ( ERR_SUCCESS != iRet )
    {
        dsm_Printf( BASE_FAT_TS_ID, "Unmount failed: [%d]!\n", iRet );
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_Find( struct cmd_entry *ent, int argc, char **argv )
{
#define FS_SHELL_DIR_FILENAME_LEN 256
    FS_FIND_DATA FindFileData;
    INT32 fd = -1;
    long errCode;
    char filename[ FS_SHELL_DIR_FILENAME_LEN ] = { 0, };
    long filename_len;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;
    BOOL bRet = FALSE;
    DEMO_FILE_INFO *p = NULL;

    if( 0 == DSM_StrCaselessCmp( "find", argv[0] ) && argc < 2)
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    if ( 0 == DSM_StrCaselessCmp( "ls", argv[0] ) || 0 == DSM_StrCaselessCmp( "dir", argv[0] ) )
    {
        if ( 1 == argc )
        {
            DSM_StrCpy( filename,  "./*" );
        }
        else
        {
            if('-' == argv[1][0])
            {
                DSM_StrCpy( filename,  "./*" );
            }
            else
            {
                filename_len = DSM_StrLen( argv[ 1 ] );
                if ( filename_len > FS_SHELL_DIR_FILENAME_LEN - 2 ) // 2 is for the length of "/*"
                {
                    dsm_Printf( BASE_FAT_TS_ID, "Invalid pathname: too long\n" );
                    return FALSE;
                }

                DSM_StrCpy( filename, argv[1] );
                DSM_StrCat( filename, "/*" );
            }
        }
        DSM_OEM2Uincode((UINT8 *)filename, (UINT16)DSM_StrLen(filename), &pUniName, &uUniLen, NULL);
    }
    else if (0 == DSM_StrCaselessCmp( "find", argv[0] ))
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1], (UINT16)DSM_StrLen((UINT8 *)argv[ 1]), &pUniName, &uUniLen, NULL);
    }
    else
    {
        dsm_Printf( BASE_FAT_TS_ID, "Invalid command.\n" );
        bRet = FALSE;
        goto func_end;
    }
    DSM_MemSet( FindFileData.st_name, NULL_CHAR, sizeof( FindFileData.st_name ) );

    // Find the first file.
    fd = FS_FindFirstFile( pUniName , &FindFileData );
    if ( fd < 0 )
    {
        if ( ERR_FS_NO_MORE_FILES == fd )
        {
            dsm_Printf( BASE_FAT_TS_ID, "No Matching entry is found\n" );
            bRet = TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "Find failed: [%d]\n", fd );
            bRet = FALSE;
        }

        goto func_end;
    }

    if(pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }

    if ( !( FS_IS_DIR( FindFileData.st_mode ) &&
            ( 0 == DSM_StrCmp( ".", FindFileData.st_name ) || 0 == DSM_StrCmp( "..", FindFileData.st_name ))))
    {
        DSM_Unicode2OEM(FindFileData.st_name, DSM_UnicodeLen(FindFileData.st_name), &pOemName, &uOemLen, NULL);
        if(!FS_IS_DIR( FindFileData.st_mode ))
        {

            if( 2 == argc && DSM_StrCaselessCmp(argv[1], "-l") == 0 ||
                    3 == argc && DSM_StrCaselessCmp(argv[2], "-l") == 0)
            {
                dsm_Printf( BASE_FAT_TS_ID, "\t%s", pOemName);
                dsm_Printf( BASE_FAT_TS_ID, "\t0x%x%s\n",
                            FindFileData.st_size > 1024 ? FindFileData.st_size / 1024 : FindFileData.st_size,
                            FindFileData.st_size > 1024 ? "kb" : "byte");
            }
            else
            {
                dsm_Printf( BASE_FAT_TS_ID, "\t%s%s\n", pOemName, FS_IS_DIR( FindFileData.st_mode ) ? "/" : "" );
            }
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "\t%s%s\n", pOemName, FS_IS_DIR( FindFileData.st_mode ) ? "/" : "" );
        }
    }

    p = DSM_MAlloc(sizeof(DEMO_FILE_INFO));
    if(NULL == p)
    {
        DSM_ASSERT(0, "shell_FS_Find: 1.malloc(0x%x) failed.", sizeof(DEMO_FILE_INFO));
    }
    tcscpy((PCWSTR)p->filename, (PCWSTR)FindFileData.st_name);
    p->Attributes = FindFileData.st_mode;
    p->next = NULL;
    AppendFile(p);

    while ( ERR_SUCCESS == ( errCode = FS_FindNextFile( fd, &FindFileData ) ) )
    {
        if(pOemName)
        {
            DSM_Free(pOemName);
            pOemName = NULL;
        }
        if ( !( FS_IS_DIR( FindFileData.st_mode ) &&
                ( 0 == DSM_StrCmp( ".", FindFileData.st_name ) || 0 == DSM_StrCmp( "..", FindFileData.st_name ))))
        {
            DSM_Unicode2OEM(FindFileData.st_name, DSM_UnicodeLen(FindFileData.st_name), &pOemName, &uOemLen, NULL);
            if(!FS_IS_DIR( FindFileData.st_mode ))
            {

                if( 2 == argc && DSM_StrCaselessCmp(argv[1], "-l") == 0 ||
                        3 == argc && DSM_StrCaselessCmp(argv[2], "-l") == 0)
                {
                    dsm_Printf( BASE_FAT_TS_ID, "\t%s", pOemName);
                    dsm_Printf( BASE_FAT_TS_ID, "\t%08d%s\n",
                                FindFileData.st_size > 1024 ? FindFileData.st_size / 1024 : FindFileData.st_size,
                                FindFileData.st_size > 1024 ? "kb" : "byte");
                }
                else
                {
                    dsm_Printf( BASE_FAT_TS_ID, "\t%s%s\n", pOemName, FS_IS_DIR( FindFileData.st_mode ) ? "/" : "" );
                }
            }
            else
            {
                dsm_Printf( BASE_FAT_TS_ID, "\t%s%s\n", pOemName, FS_IS_DIR( FindFileData.st_mode ) ? "/" : "" );
            }
        }
        /*
        p = DSM_MAlloc(sizeof(DEMO_FILE_INFO));
        if(p)
        {
                tcscpy((PCWSTR)p->filename,(PCWSTR)FindFileData.st_name);
                p->Attributes = FindFileData.st_mode;
                p->next = NULL;
                AppendFile(p);
            }
        */

    }

    //    SortByName_1(g_ffsim_sort_file_count);
    //    PrintAllChain();
    //    ReleaseAllChain();
    if ( ERR_FS_NO_MORE_FILES != errCode )
    {
        dsm_Printf( BASE_FAT_TS_ID, "ERROR[%d] in FindNextFile\n", errCode );
        FS_FindClose( fd );
        bRet = FALSE;
        goto func_end;
    }

    dsm_Printf( BASE_FAT_TS_ID, "\n" );
    FS_FindClose( fd );
    goto func_end;

func_end:
    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    if(pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return bRet;
}


static BOOL shell_FS_TUpdate( struct cmd_entry *ent, int argc, char **argv )
{
    long errCode;
    BOOL bRet = FALSE;
    UINT8 filename[FS_SHELL_DIR_FILENAME_LEN];
    UINT32 filename_len;

    if(argc > 2)
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    if ( 0 == DSM_StrCaselessCmp( "tupdate", argv[0] ))
    {
        if ( 1 == argc )
        {
            DSM_StrCpy( filename,  "/t/updata.lod" );
        }
        else
        {
            filename_len = DSM_StrLen( argv[ 1 ] );
            if ( filename_len > FS_SHELL_DIR_FILENAME_LEN - 2 ) // 2 is for the length of "/*"
            {
                dsm_Printf( BASE_FAT_TS_ID, "Invalid pathname: too long\n" );
                return FALSE;
            }

            DSM_StrCpy( filename, argv[1] );
        }
    }
    else
    {
        dsm_Printf( BASE_FAT_TS_ID, "Invalid command.\n" );
        bRet = FALSE;
        goto func_end;
    }

    // Find the first file.
    errCode = T_UP_Main(NULL);
    if ( errCode != 0 )
    {
        dsm_Printf( BASE_FAT_TS_ID, "Tupdate failed: [%d]\n", errCode );
        bRet = FALSE;
        goto func_end;
    }
    goto func_end;

func_end:
    return bRet;
}


static BOOL shell_FS_SearchFirst( struct cmd_entry *ent, int argc, char **argv )
{
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;
    FS_FIND_DATA FindFileData;
    INT32 fd = -1;

    if( argc < 2 )
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    DSM_MemSet( FindFileData.st_name, NULL_CHAR, sizeof( FindFileData.st_name ) );
    DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
    fd = FS_FindFirstFile( pUniName , &FindFileData );

    if ( fd >= 0)
    {
        dsm_Printf( BASE_FAT_TS_ID, "search descriptor[%d]\n", fd );

        DSM_Unicode2OEM(FindFileData.st_name, DSM_UnicodeLen(FindFileData.st_name), &pOemName, &uOemLen, NULL);
        dsm_Printf( BASE_FAT_TS_ID, "\t%s%s\n", pOemName, FS_IS_DIR( FindFileData.st_mode ) ? "/" : "" );
    }
    else
        dsm_Printf( BASE_FAT_TS_ID, "search file failed[%d]\n", fd );

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    if(pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return TRUE;
}


static BOOL shell_FS_SearchNext( struct cmd_entry *ent, int argc, char **argv )
{
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;
    FS_FIND_DATA FindFileData;
    INT32 fd = -1;
    long errCode;

    if( argc < 2)
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    fd = DSM_StrAToI( argv[1] );

    while ( ERR_SUCCESS == ( errCode = FS_FindNextFile( fd, &FindFileData ) ) )
    {
        DSM_Unicode2OEM(FindFileData.st_name, DSM_UnicodeLen(FindFileData.st_name), &pOemName, &uOemLen, NULL);
        dsm_Printf( BASE_FAT_TS_ID, "\t%s%s\n", pOemName, FS_IS_DIR( FindFileData.st_mode ) ? "/" : "" );
    }

    if ( ERR_FS_NO_MORE_FILES != errCode )
    {
        dsm_Printf( BASE_FAT_TS_ID, "ERROR[%d] in FindNextFile\n", errCode );
        if(pOemName)
        {
            DSM_Free(pOemName);
            pOemName = NULL;
        }
        return FALSE;
    }

    dsm_Printf( BASE_FAT_TS_ID, "No more file to search\n" );

    if(pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return TRUE;
}


static BOOL shell_FS_SearchClose( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if( argc < 2)
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    iRet = FS_FindClose( DSM_StrAToI( argv[1] ) );
    if ( ERR_SUCCESS != iRet )
    {
        dsm_Printf( BASE_FAT_TS_ID, "Search close failed[%d]\n", iRet );
    }
    return TRUE;
}

extern struct task_struct g_fs_current;
char g_dsm_Prompt[ 255 + 1 ] = { '/', '>', };
INT32 FS_ChangeDir( PCSTR pszDirName );

static BOOL shell_FS_ChangeDir( struct cmd_entry *ent, int argc, char **argv )
{
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    INT32 iRet;
    BOOL bRet = FALSE;

    if ( argc == 2 )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_ChangeDir( pUniName );
        if ( ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Change directory failed!\n" );
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }


    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_PWD( struct cmd_entry *ent, int argc, char **argv )
{

    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;
    BOOL bRet = FALSE;

    argv = argv;

    if ( argc == 1 )
    {
        FS_GetCurDir(SIZEOF(g_dsm_Prompt), (UINT8 *)g_dsm_Prompt);
        DSM_Unicode2OEM((UINT8 *)g_dsm_Prompt, (UINT16)DSM_UnicodeLen((UINT8 *)g_dsm_Prompt), &pOemName, &uOemLen, NULL);
        dsm_Printf( BASE_FAT_TS_ID, "%s\n", pOemName) ;
        bRet = TRUE;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    if(NULL != pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return bRet;
}


static BOOL shell_FS_Open( struct cmd_entry *ent, int argc, char **argv )
{
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;
    INT32 fd;
    UINT32 flag = 0;
    UINT32 mode = 0;
    char *p;

    if ( 2 <= argc )
    {
        // flag
        if ( 3 <= argc )
        {
            for ( p = argv[ 2 ]; *p; p++ )
            {
                switch ( *p )
                {
                    case 'A':
                        flag  |= FS_O_APPEND;
                        break;
                    case 'C':
                        flag  |= FS_O_CREAT;
                        break;
                    case 'E':
                        flag  |= FS_O_EXCL;
                        break;
                    case 'T':
                        flag  |= FS_O_TRUNC;
                        break;
                    case 'R':
                        flag  |= ( FS_O_WRONLY == ( flag & FS_O_ACCMODE ) ) ?
                                 FS_O_RDWR : FS_O_RDONLY;
                        break;
                    case 'W':
                        flag  |= ( FS_O_RDONLY == ( flag & FS_O_ACCMODE ) ) ?
                                 FS_O_RDWR : FS_O_WRONLY;
                        break;
                    case '+':
                        flag  |= FS_O_RDWR;
                        break;
                    default:
                        break;
                }
            }
            // mode
            if(4 <= argc)
            {
                for ( p = argv[ 3 ]; *p; p++ )
                {
                    switch ( *p )
                    {
                        case 'A':
                            mode  |= FS_ATTR_ARCHIVE;
                            break;
                        case 'R':
                            mode  |= FS_ATTR_RO;
                            break;
                        case 'S':
                            mode  |= FS_ATTR_SYSTEM;
                            break;
                        case 'H':
                            mode  |= FS_ATTR_HIDDEN;
                            break;
                        case '|':
                            break;
                        default:
                            break;
                    }
                }
            }
            else
            {
                mode = 0;
            }
        }
        else
        {
            flag = FS_O_RDWR;
        }

        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);

        fd = FS_Open( pUniName, flag, mode );
        if ( fd >= 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "open file with fd[%d] returned\n", fd );
            bRet = TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "open file failed! ErrorCode = %d\n", fd );
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}



static BOOL shell_FS_EndOfFile( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if ( 2 == argc )
    {
        iRet = FS_IsEndOfFile( DSM_StrAToI( argv[ 1 ] ) );
        if ( 1 == iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "End of file\n" );
            return TRUE;
        }
        else if ( 0 == iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Not end of file\n" );
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "error: [%d]\n", iRet );
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


static BOOL shell_FS_Create( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    UINT32 mode = 0;
    UINT8 *p;
    BOOL bRet = FALSE;


    if ( 2 <= argc )
    {
        // mode
        if(3 <= argc)
        {
            for ( p = (UINT8 *)argv[2]; *p; p++ )
            {
                switch ( *p )
                {
                    case 'A':
                        mode  |= FS_ATTR_ARCHIVE;
                        break;
                    case 'R':
                        mode  |= FS_ATTR_RO;
                        break;
                    case 'S':
                        mode  |= FS_ATTR_SYSTEM;
                        break;
                    case 'H':
                        mode  |= FS_ATTR_HIDDEN;
                        break;
                    case '|':
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            mode = 0;
        }
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        fd = FS_Create( pUniName, mode );
        if ( fd >= 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "created file with fd[%d] returned. The file has been truncated to zero.\n", fd );
            bRet = TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "create file failed!\n" );
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_CreClo( struct cmd_entry *ent, int argc, char **argv )
{

    INT32 fd;
    INT32 iRet;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;

    if ( 2 == argc )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        fd = FS_Create( pUniName, 0x0 );
        if ( fd >= 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "created file successfully.\n");
            iRet = FS_Close( fd );
            if ( ERR_SUCCESS != iRet )
            {
                dsm_Printf( BASE_FAT_TS_ID, "Close file failed!\n" );
                return FALSE;
            }

            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "create file failed!\n" );
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_DeleteFile( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;

    if ( 2 == argc )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_Delete( pUniName);
        if ( ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Delete file failed! error[%d]\n", iRet );
        }
        if(ERR_SUCCESS == iRet)
        {
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}


static BOOL shell_FS_Write( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    int len;

    if ( 3 <= argc )
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        len = FS_Write( fd, (UINT8 *)argv[ 2 ], strlen( argv[ 2 ] ) );
        if ( len > 0 )
        {
            //dsm_Printf( BASE_FAT_TS_ID, "Write [%d] bytes to file OK\n", len );
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "Write file failed\n" );
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_Read( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    int len;
    int i;

    if ( 3 == argc )
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        len = DSM_StrAToI( argv[ 2 ] );
        if ( len > FS_SHELL_BUFFER_LEN_MAX )
            len = FS_SHELL_BUFFER_LEN_MAX;

#ifndef _FS_SIMULATOR_
        if( len > TS_OUTPUT_LEN_MAX )
        {
            dsm_Printf( BASE_FAT_TS_ID, "length has tuncated as %d\n", TS_OUTPUT_LEN_MAX );
            len = TS_OUTPUT_LEN_MAX;
        }
#endif
        DSM_MemSet(g_shell_buffer, 0x00, VDS_SHELL_BUFFER_LEN_MAX + LEN_FOR_NULL_CHAR);
        len = FS_Read( fd, (UINT8 *)g_shell_buffer, len );
        if ( len > 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Read file[%d bytes] OK:\n", len );
            g_shell_buffer[ len ] = NULL_CHAR;

            for ( i = 0; i < len; i++ )
            {
                if ( NULL_CHAR == g_shell_buffer[ i ] )
                    g_shell_buffer[ i ] = ' ';
            }
            dsm_Printf( BASE_FAT_TS_ID, "%s\n", g_shell_buffer );
            return TRUE;
        }
        else if ( 0 == len )
        {
            dsm_Printf( BASE_FAT_TS_ID, "end-of-file is encountered\n" );
            return FALSE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "Read file failed\n" );
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}
static BOOL shell_FS_Seek( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    INT64 offset = 0;
    UINT8 origin = FS_SEEK_SET;
    INT64 lRet;

    if ( argc >= 3 )
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        offset = DSM_StrAToI( argv[ 2 ] );
        if ( 4 == argc )
        {
            if ( !DSM_StrCmp( argv[ 3 ], "beg" ) )
                origin = FS_SEEK_SET;
            else if ( !DSM_StrCmp( argv[ 3 ], "cur" ) )
                origin = FS_SEEK_CUR;
            else if ( !DSM_StrCmp( argv[ 3 ], "end" ) )
                origin = FS_SEEK_END;
            else
                goto lseek_cmd;
        }
        lRet = FS_Seek( fd, offset, origin );
        if ( lRet < 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Seek failed!\n" );
            return FALSE;
        }
        return TRUE;
    }
    else
    {
lseek_cmd:
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_Close( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if ( 2 == argc )
    {
        iRet = FS_Close( DSM_StrAToI( argv[ 1 ] ) );
        if ( ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Close fd(%d) failed[%d]!\n", DSM_StrAToI( argv[ 1 ] ), iRet );
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_Lwrite( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    static char c = 'a';
    char  repeated_char = NULL_CHAR;
    int nwritten = 0, tot_len;
    int original_len, len, BuffLen;

    if ( 3 <= argc )
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        tot_len = DSM_StrAToI( argv[ 2 ] );
        if ( argc > 3 )
        {
            repeated_char = argv[ 3 ][ 0 ];
        }
        BuffLen = SIZEOF(g_lwrite_buf);
        while ( tot_len > 0 )
        {
            len = MIN( tot_len, BuffLen );
            if ( repeated_char )
            {
                memset( g_lwrite_buf, repeated_char, BuffLen );
            }
            else
            {
                memset( g_lwrite_buf, 'a' + ( ( c++ ) - 'a' ) % 26, BuffLen );
            }
            /*
                #ifdef FFS_MAIL_BUG_CHECK
                checkEvent();
                #endif
            */
#ifndef _FS_SIMULATOR_
            sxr_Sleep( FFS_SLEEP_VALUE );
#endif

            original_len = len;
            if ( original_len != ( len = FS_Write( fd, (UINT8 *)g_lwrite_buf, len ) ) )
            {
                if ( len > 0 )
                {
                    nwritten += len;
                    dsm_Printf( BASE_FAT_TS_ID, "To write %d bytes ,only %d bytes written, Write file failed\n", len, original_len );
                    dsm_Printf( BASE_FAT_TS_ID, "Only write %d bytes, Write file failed\n", nwritten );
                    return FALSE;
                }

                dsm_Printf( BASE_FAT_TS_ID, "Only write %d bytes, Write file failed(%d)\n", atoi( argv[ 2 ] ) - tot_len, len );
                return FALSE;
            }

            nwritten += len;
            tot_len -= len;
        }

        dsm_Printf( BASE_FAT_TS_ID, "Write [%d] bytes to file OK\n", nwritten );
        return TRUE;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

/*
#ifndef FS_PRIVATE_MEM_MANAGE
void get_mega_kilo_byte( int n, int *mega, int *kilo, int *byte )
{
    *mega = *kilo = *byte = 0;

    if ( n > 1024 * 1024 )
        *mega = n / (1024 * 1024);
    n -= (*mega) * ( 1024 * 1024 );
    if ( n > 1024 )
        *kilo = n / 1024;
    n -= (*kilo) * 1024;
    *byte = n;
}
#endif
*/
static BOOL shell_FS_bigfile( struct cmd_entry *ent, int argc, char **argv )
{
    unsigned int c = 'A';
    INT32 fd;
    int nwritten = 0;
    int chunk_size = 199;
#ifdef _FS_SIMULATOR_
    int is_rand = 0;
#endif
    int mega = 0, kilo = 0, byte = 0;
    long lRet;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;


    if ( 3 <= argc )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        fd = FS_Create( pUniName, 0x0 );
        if( fd < 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "open/create file failed" );
            bRet = FALSE;
            goto func_end;
        }

        if ( 0 == DSM_StrCmp( argv[ 2 ], "random" ) )
        {
#ifdef _FS_SIMULATOR_
            srand( (unsigned)FS_TIME( NULL ) );
            is_rand = 1;
#endif
        }
        else if ( 0 == DSM_StrCmp( argv[ 2 ], "fixed" ) )
        {
            chunk_size = MIN( ( 4 == argc ) ? atoi( argv[ 3 ] ) : chunk_size, sizeof( g_lwrite_buf ) );
        }
        else
        {
            PRINT_CMD_USAGE( ent );
            bRet = FALSE;
            goto func_end;
        }

        memset( g_lwrite_buf, c, sizeof( g_lwrite_buf ) );

#ifdef _FS_SIMULATOR_
        if ( is_rand ) chunk_size = rand() % sizeof( g_lwrite_buf );
#endif
        while( 0 <= ( lRet = FS_Write( fd, (UINT8 *)g_lwrite_buf, chunk_size ) ) )
        {
#ifdef FFS_MAIL_BUG_CHECK
            checkEvent();
#endif

#ifndef _FS_SIMULATOR_
            sxr_Sleep( FFS_SLEEP_VALUE );
#endif

            nwritten += lRet;
            DSM_MemSet( g_lwrite_buf, 'A' + ( ( ( ++c ) - 'A' ) % 26 ), sizeof( g_lwrite_buf ) );

#ifdef _FS_SIMULATOR_
            if ( is_rand ) chunk_size = rand() % sizeof( g_lwrite_buf );
#endif
        }

        FS_Close( fd );
#ifdef FS_PRIVATE_MEM_MANAGE
        get_mega_kilo_byte( nwritten, &mega, &kilo, &byte );
#endif
        dsm_Printf( BASE_FAT_TS_ID, "write %d[0x%x] ( %dM %dK %dB)bytes to the file %s\n",
                    nwritten, nwritten, mega, kilo, byte, argv[ 1 ] );
        bRet = TRUE;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
func_end:
    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;

}


static BOOL shell_FS_MakeDir( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    //UINT8 name[] = {0x28,0x06, 0x0, 0x0};
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;
    UINT32 mode = 0;
    UINT8 *p;

    if ( 2 <= argc )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        // mode
        if(3 <= argc)
        {
            for ( p = (UINT8 *)argv[2]; *p; p++ )
            {
                switch ( *p )
                {
                    case 'A':
                        mode  |= FS_ATTR_ARCHIVE;
                        break;
                    case 'R':
                        mode  |= FS_ATTR_RO;
                        break;
                    case 'S':
                        mode  |= FS_ATTR_SYSTEM;
                        break;
                    case 'H':
                        mode  |= FS_ATTR_HIDDEN;
                        break;
                    case '|':
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            mode = 0;
        }
        iRet = FS_MakeDir(pUniName, mode);
        if ( ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_FAT_TS_ID, "Make direcotry failed!,iResult[%d]\n", iRet );
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_RemoveDir( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;

    if ( 2 == argc )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_RemoveDir( pUniName );
        dsm_Printf( BASE_FAT_TS_ID, "%s\n", ( ERR_SUCCESS == iRet ) ? "Remove direcotry OK" : "Remove direcotry failed!" );
        if(ERR_SUCCESS != iRet)
        {
            dsm_Printf( BASE_FAT_TS_ID, "errorcode =  %d\n", iRet);
            bRet = FALSE;
        }
        else
        {
            bRet =  TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet =  FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_Rename( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT8 *pUniName1 = NULL;
    UINT32 uUniLen1 = 0;
    UINT8 *pUniName2 = NULL;
    UINT32 uUniLen2 = 0;
    BOOL bRet = FALSE;


    if ( 3 == argc )
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName1, &uUniLen1, NULL);
        DSM_OEM2Uincode((UINT8 *)argv[ 2 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 2 ]), &pUniName2, &uUniLen2, NULL);
        iRet = FS_Rename(pUniName1, pUniName2 );
        if ( ERR_SUCCESS != iRet)
        {
            dsm_Printf( BASE_FAT_TS_ID, "Rename failed[%d]!\n", iRet );
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName1)
    {
        DSM_Free(pUniName1);
        pUniName1 = NULL;
    }

    if(NULL != pUniName2)
    {
        DSM_Free(pUniName2);
        pUniName2 = NULL;
    }
    return bRet;
}


/*BPB: BIOS Parameter Block*/
VOID fat_dump_bpb( PCSTR pszDevName )
{
    FAT_BOOT_RECORD boot_rec;
    //UINT8* secbuf;
    UINT32 uDevNo;
    INT32 iRet;

    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        dsm_Printf( BASE_FAT_TS_ID, "Invalid device name" );
        return;
    }

    iRet = DRV_BLOCK_READ( uDevNo, 0, (UINT8 *)g_shell_buffer );
    if(_ERR_FAT_SUCCESS == iRet)
    {
        Buf2FBR((UINT8 *)g_shell_buffer, &boot_rec);

        dsm_Printf(BASE_FAT_TS_ID, "BPB_BytesPerSec = %d\n", boot_rec.BPB_BytesPerSec);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_TotSec32    = %d\n", boot_rec.BPB_TotSec32);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_TotSec16    = %d\n", boot_rec.BPB_TotSec16);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_RsvdSecCnt  = %d\n", boot_rec.BPB_RsvdSecCnt);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_RootEntCnt  = %d\n", boot_rec.BPB_RootEntCnt);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_FATSz16     = %d\n", boot_rec.BPB_FATSz16);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_FATSz32     = %d\n", boot_rec.u.sub_rec32.BPB_FATSz32);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_SecPerTrk   = %d\n", boot_rec.BPB_SecPerTrk);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_NumHeads    = %d\n", boot_rec.BPB_NumHeads);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_SecPerClus  = %d\n", boot_rec.BPB_SecPerClus);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_NumFATs     = %d\n", boot_rec.BPB_NumFATs);
        dsm_Printf(BASE_FAT_TS_ID, "BPB_Media       = %d\n", boot_rec.BPB_Media);
        return;
    }
    else
    {
        dsm_Printf( BASE_FAT_TS_ID, "Read the first sector of the volume failed" );
    }
}

VOID fat_dump_file( PCSTR pszResName, PCSTR pszDestName)
{

    UINT8 *pUniName = NULL;
    UINT32 iUniLen = 0;
    INT32 iRet;
    INT32 fd_res = -1;
    FILE *fp_dest = NULL;
    //UINT8* dest_file = "";
    UINT8 szBuff[512];
    UINT32 r_size = 0;
    UINT32 w_size = 0;

    iRet = DSM_OEM2Uincode(pszResName, DSM_StrLen(pszResName), &pUniName, &iUniLen, NULL);

    if(iRet != 0)
    {
        dsm_Printf( BASE_FAT_TS_ID, "to unicode failed.err_code = %d.", iRet );
        goto dump_file_end;
    }

    fd_res = FS_Open(pUniName, FS_O_RDONLY, 0);
    if(fd_res < 0)
    {
        dsm_Printf( BASE_FAT_TS_ID, "open res file(%s) failed.err_code = %d.", pszResName, fd_res );
        goto dump_file_end;
    }

    fp_dest = fopen((const char *)pszDestName, (const char *)"w+");
    if(fp_dest == NULL)
    {
        dsm_Printf( BASE_FAT_TS_ID, "open res file(%s) failed.", pszDestName);
        goto dump_file_end;
    }
    do
    {
        r_size = FS_Read(fd_res, szBuff, 512);
        if(r_size > 0)
        {
            w_size = fwrite(szBuff, 1, r_size, fp_dest);
            if(w_size != r_size)
            {
                goto dump_file_end;
            }
        }
    }
    while(r_size > 0);



dump_file_end:
    if(pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    if(fd_res >= 0)
    {
        FS_Close(fd_res);
        fd_res = -1;
    }

    if(fp_dest >= 0)
    {
        fclose(fp_dest);
        fp_dest = NULL;
    }

    return;

}


/*BPB: BIOS Parameter Block*/
VOID fat_dump_dir( PCSTR pszResName, PCSTR pszDestName)
{

    UINT8 *pUniName = NULL;
    UINT32 iUniLen = 0;
    INT32 iRet;
    INT32 fd_res = -1;
    FILE *fp_dest = NULL;
    //UINT8* dest_file = "";
    UINT8 szBuff[512];
    UINT32 r_size = 0;
    UINT32 w_size = 0;

    iRet = DSM_OEM2Uincode(pszResName, DSM_StrLen(pszResName), &pUniName, &iUniLen, NULL);

    if(iRet != 0)
    {
        dsm_Printf( BASE_FAT_TS_ID, "to unicode failed.err_code = %d.", iRet );
        goto dump_file_end;
    }

    fd_res = FS_Open(pUniName, FS_O_RDONLY, 0);
    if(fd_res < 0)
    {
        dsm_Printf( BASE_FAT_TS_ID, "open res file(%s) failed.err_code = %d.", pszResName, fd_res );
        goto dump_file_end;
    }

    fp_dest = fopen((const char *)pszDestName, (const char *)"w+");
    if(fp_dest == NULL)
    {
        dsm_Printf( BASE_FAT_TS_ID, "open res file(%s) failed.", pszDestName);
        goto dump_file_end;
    }
    do
    {
        r_size = FS_Read(fd_res, szBuff, 512);
        if(r_size > 0)
        {
            w_size = fwrite(szBuff, 1, r_size, fp_dest);
            if(w_size != r_size)
            {
                goto dump_file_end;
            }
        }
    }
    while(r_size > 0);



dump_file_end:
    if(pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    if(fd_res >= 0)
    {
        FS_Close(fd_res);
        fd_res = -1;
    }

    if(fp_dest >= 0)
    {
        fclose(fp_dest);
        fp_dest = NULL;
    }

    return;

}


VOID fat_dump_nvram(UINT32 iId, UINT8 *pszDestName);
static BOOL shell_FS_FAT_Dump( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 counter;
    INT32 iRet;
    UINT8 *pUniName = NULL;
    UINT8 szNvram[128];
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;

    if ( argc >= 3 )
    {
        if ( 0 == DSM_StrCaselessCmp("fatlist", argv[1] ) )
        {
            DSM_OEM2Uincode((UINT8 *)argv[ 2 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 2 ]), &pUniName, &uUniLen, NULL);
            iRet = fat_dump_fat_list( (UINT8 *)pUniName );
            if(ERR_SUCCESS == iRet)
            {
                bRet = TRUE;
            }
            else
            {
                bRet = FALSE;
            }
            if(NULL != pUniName)
            {
                DSM_Free(pUniName);
                pUniName = NULL;
            }
        }
        else if ( 0 == DSM_StrCaselessCmp("fatent", argv[1] ) )
        {
            if ( 3 == argc )
            {
                counter = 8;
            }
            else
            {
                counter = DSM_StrAToI(argv[ 3 ] );
            }
            iRet = fat_dump_fat( DSM_StrAToI(argv[ 2 ] ), counter );
            if(ERR_SUCCESS == iRet)
            {
                bRet = TRUE;
            }
            else
            {
                bRet = FALSE;
            }
        }
        else if ( 0 == DSM_StrCaselessCmp("bpb", argv[1] ) )
        {
            fat_dump_bpb((PCSTR)argv[2]);
            bRet = TRUE;
        }
        else if ( 0 == DSM_StrCaselessCmp("file", argv[1] ) )
        {
            if(argc >= 4)
            {
                fat_dump_file((PCSTR)argv[2], (PCSTR)argv[3]);
                bRet = TRUE;
            }
            else if(argc >= 3)
            {
                fat_dump_file((PCSTR)argv[2], (PCSTR)argv[2]);
                bRet = TRUE;
            }

        }
        else if(0 == DSM_StrCaselessCmp("nvram", argv[1]))
        {
            if(argc >= 4)
            {
                fat_dump_nvram(DSM_StrAToI(argv[ 2 ] ), (UINT8 *)argv[3]);
                bRet = TRUE;
            }
            else if(argc >= 3)
            {
                DSM_StrPrint((char *)szNvram, (const char *)"NVRAM_%d.dat", (char *)argv[ 2 ]);
                fat_dump_nvram(DSM_StrAToI(argv[ 2 ]), (UINT8 *)szNvram);
                bRet = TRUE;
            }

        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}
static BOOL shell_FS_GetFSInfo(struct cmd_entry *ent, int argc, char **argv)
{
    INT32 iRet;
    FS_INFO sFSInfo;
    UINT8 szUsed[32];
    UINT8 szTotal[32];

    iRet = ERR_SUCCESS;
    if(argc == 2)
    {
        iRet = FS_GetFSInfo((UINT8 *)argv[1], &sFSInfo);
        if(ERR_SUCCESS != iRet)
        {
            dsm_Printf( BASE_FAT_TS_ID, "GetFsInfo failed!\n" );
            return FALSE;
        }
        else
        {
            ui64toa(sFSInfo.iTotalSize, szTotal, 10);
            ui64toa(sFSInfo.iUsedSize, szUsed, 10);
            dsm_Printf( BASE_FAT_TS_ID, "total_size = %s,used size = %s.",
                        szTotal,
                        szUsed);
            return TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_GetFileAttr(struct cmd_entry *ent, int argc, char **argv)
{
    INT32 iRet = ERR_SUCCESS;
    FS_FILE_ATTR sFileAttr;
    struct tm *psSysTime;
    char *pctime;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;


    if(argc == 2)
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_GetFileAttr( pUniName, &sFileAttr );
        if( iRet == ERR_SUCCESS )
        {
            dsm_Printf( BASE_FAT_TS_ID, "ino = %I64u,size=%I64u,mode = 0x%x,count=0x%x\n",
                        sFileAttr.i_no, sFileAttr.i_size, sFileAttr.i_mode, sFileAttr.i_count );

            psSysTime = localtime ((INT32 *)(&sFileAttr.i_atime));
            pctime = ctime((INT32 *)&sFileAttr.i_atime);
            dsm_Printf(BASE_FAT_TS_ID, "atime: 0x%x, 0x%x, 0x%x %s",
                       psSysTime->tm_yday, psSysTime->tm_mon, psSysTime->tm_year, pctime );

            psSysTime = localtime ((INT32 *)&sFileAttr.i_ctime);
            pctime = ctime((INT32 *)&sFileAttr.i_ctime);
            dsm_Printf(BASE_FAT_TS_ID, "ctime: 0x%x, 0x%x, 0x%x %s",
                       psSysTime->tm_yday, psSysTime->tm_mon, psSysTime->tm_year, pctime );

            psSysTime = localtime ((INT32 *)&sFileAttr.i_mtime);
            pctime = ctime((INT32 *)&sFileAttr.i_mtime);
            dsm_Printf(BASE_FAT_TS_ID, "mtime: 0x%x, 0x%x, 0x%x %s",
                       psSysTime->tm_yday, psSysTime->tm_mon, psSysTime->tm_year, pctime );

            bRet = TRUE;
        }
        else
        {
            dsm_Printf(BASE_FAT_TS_ID, "FS_GetFileAttr Failed\n");
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    return bRet;
}


static BOOL shell_FS_GetDirSize(struct cmd_entry *ent, int argc, char **argv)
{
    INT64 iRet = ERR_SUCCESS;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;
    UINT64 iSize = 0;

    if(argc >= 2)
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_GetDirSize( pUniName, &iSize);
        if( iRet == ERR_SUCCESS )
        {
            dsm_Printf(BASE_FAT_TS_ID, "dir size:0x%x(%dK)(%dM)\n", iSize, iSize / 1024, iSize / 1024 / 1024);
            bRet = TRUE;
        }
        else
        {
            dsm_Printf(BASE_FAT_TS_ID, "FS_GetDirSize Failed\n");
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    return bRet;
}

static BOOL shell_FS_SetFileAttr(struct cmd_entry *ent, int argc, char **argv)
{
    INT32 iRet = ERR_SUCCESS;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    BOOL bRet = FALSE;
    UINT32 mode = 0;
    UINT8 *p;


    if(argc >= 2)
    {
        // mode
        if(3 <= argc)
        {
            for ( p = (UINT8 *)argv[2]; *p; p++ )
            {
                switch ( *p )
                {
                    case 'A':
                        mode  |= FS_ATTR_ARCHIVE;
                        break;
                    case 'R':
                        mode  |= FS_ATTR_RO;
                        break;
                    case 'S':
                        mode  |= FS_ATTR_SYSTEM;
                        break;
                    case 'H':
                        mode  |= FS_ATTR_HIDDEN;
                        break;
                    case '|':
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            mode = 0;
        }
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName, &uUniLen, NULL);
        iRet = FS_SetFileAttr( pUniName, mode);
        if( iRet == ERR_SUCCESS )
        {
            bRet = TRUE;
        }
        else
        {
            dsm_Printf(BASE_FAT_TS_ID, "FS_SetFileAttr Failed\n");
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    return bRet;
}

static BOOL shell_FS_GetVolLab(struct cmd_entry *ent, int argc, char **argv)
{
	INT32 iRet;
    UINT8 szBuff[32];
	if(argc == 2)
	{
		iRet = FS_GetVolLab((PCSTR)argv[1],szBuff);
		return TRUE;
	}
	else
	{
		PRINT_CMD_USAGE( ent );
		return FALSE;
	}
}

static BOOL shell_FS_SetVolLab(struct cmd_entry *ent, int argc, char **argv)
{
	INT32 iRet;
	UINT8* iVolLab = NULL;
	if(argc == 3)
	{
		iRet = FS_SetVolLab((PCSTR)argv[1], (PCSTR)argv[2]);
		return TRUE;
	}
	else
	{
		PRINT_CMD_USAGE( ent );
		return FALSE;
	}
}

static BOOL shell_FS_HasFormated(struct cmd_entry *ent, int argc, char **argv)
{
    INT32 iRet = ERR_SUCCESS;
    if(argc == 3)
    {
        iRet = FS_HasFormatted((PCSTR)argv[1], (UINT8)DSM_StrAToI(argv[2]));
        if(ERR_FS_HAS_FORMATED == iRet)
        {
            dsm_Printf(BASE_FAT_TS_ID, "fs has formatted!\n");
            return TRUE;
        }
        else
        {
            dsm_Printf(BASE_FAT_TS_ID, "fs has not formatted! errorcode = %d\n", iRet);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}
static BOOL Shell_FS_ScanDisk(struct cmd_entry *ent, int argc, char **argv)
{
    INT32 iRet = ERR_SUCCESS;
    UINT8 iFsType;
    if(argc == 3)
    {
        iFsType =  (UINT8)(DSM_StrCaselessCmp((UINT8 *)"FAT", (UINT8 *)argv[2]) == 0 ? FS_TYPE_FAT : FS_TYPE_FFS);
        iRet = FS_ScanDisk((UINT8 *)argv[1], iFsType);
        if(ERR_SUCCESS != iRet)
        {
            dsm_Printf(BASE_FAT_TS_ID, "Scan Disk Error,errorcode =  %d\n", iRet);
            return FALSE;
        }
        else
        {
            dsm_Printf(BASE_FAT_TS_ID, "SCAN DISK SUCCEED!\n");
            return TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


static BOOL shell_FS_GetFileName( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    INT32 iRet;
    UINT8 fileName[LEN_FOR_OEM_UNI_BUFFER] = {0,};
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;
    BOOL bRet = FALSE;

    if ( 2 <= argc )
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        iRet = FS_GetFileName( fd, LEN_FOR_OEM_UNI_BUFFER, fileName);
        if ( ERR_SUCCESS == iRet )
        {
            DSM_Unicode2OEM((UINT8 *)fileName, (UINT16)DSM_UnicodeLen((UINT8 *)fileName), &pOemName, &uOemLen, NULL);
            dsm_Printf( BASE_FAT_TS_ID, "%s\n", pOemName);
            bRet = TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "FS_GetFileName failed!errirCode = %d. \n", iRet);
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return bRet;
}

static BOOL shell_FS_GetSize( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;

    INT64 size = 0;

    if (argc >= 2)
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        size = FS_GetFileSize(fd);
        if (size >= 0 )
        {
            dsm_Printf( BASE_FAT_TS_ID, "get [%d] size %llx\n", fd, size);
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "get [%d] size failed.ret=%d\n", fd, (INT32)size);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_ChangeSize( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    UINT64 nFileSize;
    INT64 iRet;

    if (argc >= 3)
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        nFileSize = DSM_StrAToI( argv[ 2 ] );
        iRet = FS_ChangeSize( fd, nFileSize);
        if ( iRet >= 0 )
        {
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "FS_ChangeSize failed!errirCode = %d. \n", iRet);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


static BOOL shell_FS_Flush( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 fd;
    INT32 iRet;

    if (argc >= 2)
    {
        fd = DSM_StrAToI( argv[ 1 ] );
        iRet = FS_Flush(fd);
        if ( ERR_SUCCESS == iRet )
        {
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "FS_Flush failed!errirCode = %d. \n", iRet);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


#define FS_ERROR_RESERVED -1
#define NR_MAX_DEPTH           100
#define INVALID_FD                 -1
#define LEN_PATH_BUF              512

UINT8 pcPath[LEN_PATH_BUF] = {0,};

INT32 FS_DropDirTree(const UINT8 *pcDirName)
{
    INT32 i, iDepth = 0;
    INT32 iFindFd[NR_MAX_DEPTH];
    UINT8 *pOriginal, *p = pcPath;
    UINT8 str[] = "/*";
    INT32 iRet;
    UINT8 c;
    UINT32 iNameLen;
    FS_FIND_DATA sFindData;
    BOOL bCheat = TRUE;

    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;


    DSM_Unicode2OEM((UINT8 *)g_dsm_Prompt, (UINT16)DSM_UnicodeLen((UINT8 *)g_dsm_Prompt), &pOemName, &uOemLen, NULL);
    DSM_StrCpy(pcPath, pOemName);

    if ('\\' == *pcDirName || '/' == *pcDirName)
    {
        if (!DSM_StrCmp(pcDirName, "\\") || !DSM_StrCmp(pcDirName, "/"))
        {
            //return -1;
        }
        else
        {
            *p = 0;
        }
    }
    else
    {
        while (*p)
        {
            p++;
        }
        while (('/' == (c = *(p - 1)) || '\\' == c ) && p > pcPath)
        {
            p--;
            *p = 0;
        }
    }
    DSM_StrNCpy(p, "/", 1);
    p++;
    iNameLen = DSM_StrLen(pcDirName);
    DSM_StrNCpy(p, pcDirName, iNameLen);
    p += iNameLen;
    pOriginal = p;


    for (i = 0; i < NR_MAX_DEPTH; ++i)
    {
        iFindFd[i] = INVALID_FD;
    }

    while (TRUE == bCheat)
    {
        if (INVALID_FD == iFindFd[iDepth])
        {
            if ((p + DSM_StrLen(str)) >= (pcPath + LEN_PATH_BUF))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            DSM_StrCpy(p, str);
            p++;

            DSM_OEM2Uincode((UINT8 *)pcPath, (UINT16)DSM_StrLen((UINT8 *)pcPath), &pUniName, &uUniLen, NULL);
            iFindFd[iDepth] = FS_FindFirstFile(pUniName, &sFindData);
            if (ERR_FS_NO_MORE_FILES == iFindFd[iDepth])
            {
                ;
            }
            else if (iFindFd[iDepth] < 0)
            {
                CSW_TRACE(BASE_DSM_TS_ID,"[#error!!]: FS_FindFirstFile return = %d.\n", iFindFd[iDepth]);
                iRet = iFindFd[iDepth];
                iDepth--;
                goto failed;
            }
            if(NULL != pOemName)
            {
                DSM_Free(pOemName);
                pOemName = NULL;
            }
            DSM_Unicode2OEM((UINT8 *)sFindData.st_name, (UINT16)DSM_UnicodeLen((UINT8 *)sFindData.st_name), &pOemName, &uOemLen, NULL);
            if ((p + DSM_StrLen(pOemName)) >= (pcPath + LEN_PATH_BUF))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            DSM_StrCpy(p, pOemName);

            if(NULL != pUniName)
            {
                DSM_Free(pUniName);
                pUniName = NULL;
            }
            DSM_OEM2Uincode((UINT8 *)pcPath, (UINT16)DSM_StrLen((UINT8 *)pcPath), &pUniName, &uUniLen, NULL);

            if (!FS_IS_DIR( sFindData.st_mode ))
            {
                if ( ERR_SUCCESS != (iRet = FS_Delete(pUniName)) )
                {
                    CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: FS_Delete 1 return = %d.\n",iRet);
                    goto failed;
                }
            }
            else if (FS_IS_DIR( sFindData.st_mode ) && '.' == sFindData.st_name[0]
                     && 0 == sFindData.st_name[1])
            {
                ;
            }
            else
            {
                iRet = FS_RemoveDir(pUniName);
                if (ERR_SUCCESS == iRet)
                {
                    ;
                }
                else if (iRet != ERR_FS_DIR_NOT_EMPTY)
                {
                    CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: FS_RemoveDir 1 return = %d.\n", iRet);
                    goto failed;
                }
                else
                {
                    goto dir_not_empty;
                }
            }
        }

        while ( ERR_SUCCESS == ( iRet = FS_FindNextFile( iFindFd[iDepth], &sFindData ) ) )
        {
            if(NULL != pOemName)
            {
                DSM_Free(pOemName);
                pOemName = NULL;
            }
            DSM_Unicode2OEM((UINT8 *)sFindData.st_name, (UINT16)DSM_UnicodeLen((UINT8 *)sFindData.st_name), &pOemName, &uOemLen, NULL);
            if ((p + DSM_StrLen(pOemName)) >= (pcPath + LEN_PATH_BUF))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            DSM_StrCpy(p, pOemName);

            if(NULL != pUniName)
            {
                DSM_Free(pUniName);
                pUniName = NULL;
            }
            DSM_OEM2Uincode((UINT8 *)pcPath, (UINT16)DSM_StrLen((UINT8 *)pcPath), &pUniName, &uUniLen, NULL);

            // Delete file.
            if (!FS_IS_DIR( sFindData.st_mode ))
            {
                if ( ERR_SUCCESS != (iRet = FS_Delete(pUniName)))
                {
                    CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: FS_Delete 2 return = %d.\n", iRet);
                    goto failed;
                }
            }
            else if ( '.' == sFindData.st_name[0] &&
                      0 == sFindData.st_name[1])
            {
                // Do nothing.
            }
            else
            {
                iRet = FS_RemoveDir(pUniName);
                if (ERR_SUCCESS == iRet)
                {
                    // Do nothing.
                }
                else if (iRet != ERR_FS_DIR_NOT_EMPTY)
                {
                    CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: FS_RemoveDir 2 return = %d.\n", iRet);
                    goto failed;
                }
                else
                {
                    goto dir_not_empty;
                }
            }
        }

        if (iRet != ERR_FS_NO_MORE_FILES)
        {
            goto failed;
        }

        FS_FindClose(iFindFd[iDepth]);
        iFindFd[iDepth] = INVALID_FD;
        iDepth--;
        p--;
        *p = 0;

        if(NULL != pUniName)
        {
            DSM_Free(pUniName);
            pUniName = NULL;
        }
        DSM_OEM2Uincode((UINT8 *)pcPath, (UINT16)DSM_StrLen((UINT8 *)pcPath), &pUniName, &uUniLen, NULL);

        if ((iRet = FS_RemoveDir(pUniName)) != ERR_SUCCESS)
        {
            CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: FS_RemoveDir333 return = %d.\n", iRet);
            goto failed;
        }
        if (p == pOriginal)
        {
            if (iDepth != -1)
            {
                CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: p == pOriginal && iDepth != 0.\n");
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            break;
        }

        while ((c = *(p - 1)) != '/' && c != '\\')
        {
            p--;
        }
        *p = 0;
        continue;

dir_not_empty:
        if(NULL != pOemName)
        {
            DSM_Free(pOemName);
            pOemName = NULL;
        }
        DSM_Unicode2OEM((UINT8 *)sFindData.st_name, (UINT16)DSM_UnicodeLen((UINT8 *)sFindData.st_name), &pOemName, &uOemLen, NULL);
        if ((NR_MAX_DEPTH - 1) == iDepth)
        {
            iRet = FS_ERROR_RESERVED;
            goto failed;
        }
        iNameLen = DSM_StrLen(pOemName);
        p += iNameLen;
        iDepth++;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    if(NULL != pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return ERR_SUCCESS;

failed:
    CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: failed+++++ iFindFd[%d] = %d \n", iDepth, iFindFd[iDepth]);
    for (i = iDepth; i >= 0; --i)
    {
        FS_FindClose(iFindFd[i]);
    }
    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    if(NULL != pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return iRet;
}


#define FS_FILE_TYPE                0x001
#define FS_DIR_TYPE                 0x010
#define FS_RECURSIVE_TYPE     0x100

INT32 FS_Count(const UINT8 *pcDirName, INT32 Flag)
{
    INT32 i, iDepth = 0;
    INT32 iFindFd[NR_MAX_DEPTH];
    UINT8 *pOriginal, *p = pcPath;
    UINT8 str[] = "/*";
    INT32 iRet;
    UINT8 c;
    UINT32 iNameLen;
    FS_FIND_DATA sFindData;
    INT32 iFileNum = 0;
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;
    UINT8 *pOemName = NULL;
    UINT32 uOemLen = 0;
    BOOL bCheat = TRUE;

    DSM_Unicode2OEM((UINT8 *)g_dsm_Prompt, (UINT16)DSM_UnicodeLen((UINT8 *)g_dsm_Prompt), &pOemName, &uOemLen, NULL);
    DSM_StrCpy(pcPath, pOemName);

    if ('\\' == *pcDirName || '/' == *pcDirName)
    {
        *p = 0;
    }
    else
    {
        while (*p)
        {
            p++;
        }
        while (('/' == (c = *(p - 1)) || '\\' == c) && p > pcPath)
        {
            p--;
            *p = 0;
        }
    }
    DSM_StrNCpy(p, "/", 1);
    p++;
    iNameLen = DSM_StrLen(pcDirName);
    DSM_StrNCpy(p, pcDirName, iNameLen);
    p += iNameLen;
    pOriginal = p;

    for (i = 0; i < NR_MAX_DEPTH; ++i)
    {
        iFindFd[i] = INVALID_FD;
    }

    while (TRUE == bCheat)
    {
        if (INVALID_FD == iFindFd[iDepth])
        {
            if ((p + DSM_StrLen(str)) >= (pcPath + LEN_PATH_BUF))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            DSM_StrCpy(p, str);
            p++;
            if(NULL != pUniName)
            {
                DSM_Free(pUniName);
                pUniName = NULL;
            }
            DSM_OEM2Uincode((UINT8 *)pcPath, (UINT16)DSM_StrLen((UINT8 *)pcPath), &pUniName, &uUniLen, NULL);
            iFindFd[iDepth] = FS_FindFirstFile(pUniName, &sFindData);
            if (ERR_FS_NO_MORE_FILES == iFindFd[iDepth])
            {
                goto current_dir_empty;
            }
            else if (iFindFd[iDepth] < 0)
            {
                CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: FS_FindFirstFile return = %d.\n", iFindFd[iDepth]);
                iDepth--;
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }

            if (!FS_IS_DIR( sFindData.st_mode ))
            {
                if ( Flag & FS_FILE_TYPE )
                {
                    iFileNum++;
                }
            }
            else
            {
                if ('.' == sFindData.st_name[0] && 0 == sFindData.st_name[1])
                {
                    ;
                }
                else
                {
                    if (Flag & FS_DIR_TYPE)
                    {
                        iFileNum++;
                    }
                    if (Flag & FS_RECURSIVE_TYPE)
                    {
                        if ((p + DSM_StrLen(sFindData.st_name)) >= (pcPath + LEN_PATH_BUF))
                        {
                            iRet = FS_ERROR_RESERVED;
                            goto failed;
                        }
                        DSM_StrCpy(p, sFindData.st_name);
                        goto recursive_dir;
                    }
                }
            }
        }

        while ( ERR_SUCCESS == ( iRet = FS_FindNextFile( iFindFd[iDepth], &sFindData ) ) )
        {
            if (!FS_IS_DIR( sFindData.st_mode ))
            {
                if ( Flag & FS_FILE_TYPE )
                {
                    iFileNum++;
                }
            }
            else
            {
                if ('.' == sFindData.st_name[0] && 0 == sFindData.st_name[1])
                {
                    ;
                }
                else
                {
                    if (Flag & FS_DIR_TYPE)
                    {
                        iFileNum++;
                    }
                    if (Flag & FS_RECURSIVE_TYPE)
                    {
                        if(NULL != pOemName)
                        {
                            DSM_Free(pOemName);
                            pOemName = NULL;
                        }
                        DSM_Unicode2OEM((UINT8 *)sFindData.st_name, (UINT16)DSM_UnicodeLen((UINT8 *)sFindData.st_name), &pOemName, &uOemLen, NULL);
                        if ((p + DSM_StrLen(pOemName)) >= (pcPath + LEN_PATH_BUF))
                        {
                            iRet = FS_ERROR_RESERVED;
                            goto failed;
                        }
                        DSM_StrCpy(p, pOemName);
                        goto recursive_dir;
                    }
                }
            }
        }

        if (iRet != ERR_FS_NO_MORE_FILES)
        {
            iRet = FS_ERROR_RESERVED;
            goto failed;
        }
        iRet = FS_FindClose(iFindFd[iDepth]);

current_dir_empty:
        iFindFd[iDepth] = INVALID_FD;
        iDepth--;
        p--;
        *p = 0;

        if (p == pOriginal)
        {
            if (iDepth != -1)
            {
                CSW_TRACE(BASE_DSM_TS_ID, "[#error!!]: p == pOriginal && iDepth != 0.\n");
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            break;
        }

        while ((c = *(p - 1)) != '/' && c != '\\')
        {
            p--;
        }
        *p = 0;
        continue;

recursive_dir:

        if(NULL != pOemName)
        {
            DSM_Free(pOemName);
            pOemName = NULL;
        }
        DSM_Unicode2OEM((UINT8 *)sFindData.st_name, (UINT16)DSM_UnicodeLen((UINT8 *)sFindData.st_name), &pOemName, &uOemLen, NULL);

        if ((NR_MAX_DEPTH - 1) == iDepth)
        {
            iRet = FS_ERROR_RESERVED;
            goto failed;
        }
        iNameLen = DSM_StrLen(pOemName);
        p += iNameLen;
        iDepth++;
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }
    if(NULL != pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return iFileNum;

failed:

    CSW_TRACE(BASE_DSM_TS_ID, "+++++failed !!!+++++.\n", iRet);
    for (i = iDepth; i >= 0; --i)
    {
        CSW_TRACE(BASE_DSM_TS_ID, "+++++FS_FindClose: iFindFd[%d] = %d +++++.\n", i, iFindFd[i]);
        FS_FindClose(iFindFd[i]);
    }

    if(NULL != pUniName)
    {
        DSM_Free(pUniName);
        pUniName = NULL;
    }

    if(NULL != pOemName)
    {
        DSM_Free(pOemName);
        pOemName = NULL;
    }
    return iRet;

}


static BOOL shell_DropDirTree( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if ( 2 <= argc )
    {
        iRet = FS_DropDirTree((const UINT8 *)argv[ 1 ]);
        if ( ERR_SUCCESS == iRet )
        {
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "MMI_FS_RemoveDir failed!errirCode = %d. \n", iRet);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


static BOOL shell_Count_file( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    INT32 flag = 1;

    if ( argc >= 2)
    {
        if (argc > 2)
        {
            flag = DSM_StrAToI( argv[2] );
        }
        if (1 == flag)
        {
            flag = FS_FILE_TYPE | FS_DIR_TYPE | FS_RECURSIVE_TYPE;
        }
        else if (2 == flag)
        {
            flag = FS_FILE_TYPE | FS_RECURSIVE_TYPE;
        }
        else if (3 == flag)
        {
            flag = FS_DIR_TYPE | FS_RECURSIVE_TYPE;
        }
        else if (4 == flag)
        {
            flag = FS_FILE_TYPE | FS_DIR_TYPE;
        }
        else if (5  == flag)
        {
            flag = FS_FILE_TYPE;
        }
        else if (6  == flag)
        {
            flag = FS_DIR_TYPE;
        }
        else
        {
            PRINT_CMD_USAGE( ent );
            return FALSE;
        }

        iRet = FS_Count((const UINT8 *)argv[ 1 ], flag);
        if (iRet >= 0)
        {
            dsm_Printf( BASE_FAT_TS_ID, "\t%d. \n", iRet);
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "FS_Count failed!errirCode = %d. \n", iRet);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


INT32 UniStrCmpi(const UINT16 *str1, const UINT16 *str2)
{
#define IS_LETTER(c)        ((c >= 'A' && c <= 'z') || (c >= 'a' && c <= 'z'))
#define IS_MATCH_LETTER(c1, c2)     (c1 - c2 == 32 || c2 - c1 == 32 || c1 -c2 == 0)

    const UINT16 *p1 = str1;
    const UINT16 *p2 = str2;
    UINT8 c1, c2;

    while ( *p1 || *p2 )
    {
        if (*p1 != *p2)
        {
            c1 = ((UINT8 *)p1)[1];
            c2 = ((UINT8 *)p2)[1];
            if (c1 != 0 || c2 != 0)
            {
                return (*p1 > *p2 ? 1 : -1);
            }
            c1 = ((UINT8 *)p1)[0];
            c2 = ((UINT8 *)p2)[0];
            if (c1 >= 'A' && c1 <= 'Z')
            {
                c1 += 32;
            }
            if (c2 >= 'A' && c2 <= 'Z')
            {
                c2 += 32;
            }
            if (!IS_LETTER(c1) || !IS_LETTER(c2) || !IS_MATCH_LETTER(c1, c2))
            {
                return (c1 > c2 ? 1 : -1);
            }
        }
        p1++;
        p2++;
    }
    return 0;
}


static BOOL shell_UniCmp( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    UINT8 *pUniName1 = NULL;
    UINT32 uUniLen1 = 0;
    UINT8 *pUniName2 = NULL;
    UINT32 uUniLen2 = 0;
    BOOL bRet = FALSE;

    if ( argc >= 3)
    {
        DSM_OEM2Uincode((UINT8 *)argv[ 1 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 1 ]), &pUniName1, &uUniLen1, NULL);

        DSM_OEM2Uincode((UINT8 *)argv[ 2 ], (UINT16)DSM_StrLen((UINT8 *)argv[ 2 ]), &pUniName2, &uUniLen2, NULL);

        iRet = UniStrCmpi( (UINT16 *)pUniName1, (UINT16 *)pUniName2);
        dsm_Printf( BASE_FAT_TS_ID, "\t%d. \n", iRet);
        bRet = TRUE;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }

    if(NULL != pUniName1)
    {
        DSM_Free(pUniName1);
        pUniName1 = NULL;
    }

    if(NULL != pUniName2)
    {
        DSM_Free(pUniName2);
        pUniName2 = NULL;
    }

    return bRet;
}

static BOOL shell_FS_ReleaseDev(struct cmd_entry *ent, int argc, char **argv)
{
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;

    if (argc == 2)
    {
        if (FS_ReleaseDev(argv[1]) == ERR_SUCCESS)
        {
            dsm_Printf( BASE_FAT_TS_ID, "release %s success\n", argv[1]);
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "release %s failed\n", argv[1]);
            return TRUE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_IsValidFileName(struct cmd_entry *ent, int argc, char **argv)
{
    UINT8 *pUniName = NULL;
    UINT32 uUniLen = 0;

    if (argc == 2)
    {
        DSM_OEM2Uincode((UINT8 *)argv[1], (UINT16)DSM_StrLen((UINT8 *)argv[1]), &pUniName, &uUniLen, NULL);
        if (FS_IsValidFileName((PCSTR)pUniName, TRUE))
        {
            dsm_Printf( BASE_FAT_TS_ID, "%s is valid name\n", argv[1]);
            return TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "%s is invalid name\n", argv[1]);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}

static BOOL shell_FS_GetDevInfo( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 i;
    INT32 iRet;
    UINT32 uDeviceCount = 0;
    FS_DEV_INFO *pFSDevInfo = NULL;
    BOOL bRet;

    argv = argv;
    if(argc > 1)
    {
        dsm_Printf( BASE_FAT_TS_ID, "The devinfo command none parameter.\n");
        return FALSE;
    }
    iRet = FS_GetDeviceInfo(&uDeviceCount, &pFSDevInfo);
    if(ERR_SUCCESS == iRet)
    {
        for(i = 0; i < uDeviceCount; i++)
        {
            dsm_Printf( BASE_FAT_TS_ID, "name = %s,type = %d,is_root = %d.\n",
                        pFSDevInfo[i].dev_name,
                        pFSDevInfo[i].dev_type,
                        pFSDevInfo[i].is_root
                      );
        }
        bRet = TRUE;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}


INT32 fat_dump_fat_list( PCSTR pathname )
{
    struct inode *inode;
    FAT_SB_INFO *sb;
    EXFAT_SB_INFO *exsb;
    UINT32 clus_nr;
    UINT32 next_clus_nr;
    INT32 counter = 0;
    BOOL is_valid_cluser;

    inode = namei((PCWSTR) pathname );
    if( !inode )
    {
        return ERR_FS_NO_DIR_ENTRY;
    }

    switch (inode->i_sb->s_fstype)
    {
        case FS_TYPE_FAT:
            sb = &(inode->i_sb->u.fat_sb);
            clus_nr = inode->u.fat_i.entry.DIR_FstClusLO;
            is_valid_cluser = !fat_is_last_cluster(sb, clus_nr);
            break;
#ifdef FS_SUPPORT_EXFAT
        case FS_TYPE_EXFAT:
            exsb = &(inode->i_sb->u.exfat_sb);
            clus_nr = inode->u.exfat_i.ext_entry.FirstCluster;
            is_valid_cluser = EXFAT_IS_VALID_CLUSTER(exsb, clus_nr);
            break;
#endif
        default:
            return ERR_FS_NO_DIR_ENTRY;
    }

    while(is_valid_cluser)
    {
        switch (inode->i_sb->s_fstype)
        {
            case FS_TYPE_FAT:
                fat_get_next_cluster(sb, clus_nr, &next_clus_nr);
                is_valid_cluser = !fat_is_last_cluster(sb, next_clus_nr);
                break;
#ifdef FS_SUPPORT_EXFAT
            case FS_TYPE_EXFAT:
                exfat_get_next_cluster(inode, clus_nr, &next_clus_nr);
                is_valid_cluser = EXFAT_IS_VALID_CLUSTER(exsb, next_clus_nr);
                break;
#endif
            default:
                break;
        }

        dsm_Printf( BASE_DSM_TS_ID, "%3d%s", clus_nr, is_valid_cluser ? "-->" : ""  );
        clus_nr = next_clus_nr;
        counter++;

        if ( 0 == counter % 8 )
        {
            dsm_Printf( BASE_DSM_TS_ID, "\n" );
        }
    }
    dsm_Printf( BASE_DSM_TS_ID, "\n" );

    iput( inode );
    return ERR_SUCCESS;
}


INT32 fat_dump_fat(UINT32 begin_clus_nr, UINT32 counter )
{
    struct inode *root_i;
    FAT_SB_INFO *sb_info;
    UINT32 entry_ofs_in_fat;
    UINT32 sec_nr;
    UINT32 entry_ofs_in_sec = 0, old_sec_nr = 0;
    UINT32 clus_nr = 0;
    UINT8  *secbuf = NULL;
    UINT32 i;
    INT32 iResult = _ERR_FAT_SUCCESS;
    INT32 iRet = _ERR_FAT_SUCCESS;

    root_i = namei( (PCWSTR)"/" );
    if (!root_i)
    {
        return ERR_FS_INVALID_DIR_ENTRY;
    }

    sb_info = &(root_i->i_sb->u.fat_sb);
    iput(root_i);

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR, (const unsigned char *)"in fat_dump_fat,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_dump_fat,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }
    i = begin_clus_nr;

    dsm_Printf( BASE_DSM_TS_ID, "\n%2x: ", 0 );
    if(sb_info->iFATn == FAT12)
    {
        do
        {
            entry_ofs_in_fat = i + (i / 2);
            sec_nr = sb_info->iFATStartSec  + (entry_ofs_in_fat / sb_info->iBytesPerSec);
            entry_ofs_in_sec = entry_ofs_in_fat % sb_info->iBytesPerSec;

            if(old_sec_nr < sec_nr)
            {
                DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
                DSM_MemSet(secbuf + DEFAULT_SECSIZE, 0x00, DEFAULT_SECSIZE);

                iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, sec_nr, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    D( ( DL_FATERROR, (const unsigned char *)"In fat_dump_fat DRV_BLOCK_READ_1 failed iResult = %d\n", iResult));
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    goto end;
                }

                iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, sec_nr + 1, secbuf + DEFAULT_SECSIZE );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    D( ( DL_FATERROR, (const unsigned char *)"In fat_dump_fat DRV_BLOCK_READ_2 failed iResult = %d\n", iResult));
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    goto end;
                }

                old_sec_nr = sec_nr;
            }

            clus_nr = MAKEFATUINT16(secbuf[entry_ofs_in_sec], secbuf[entry_ofs_in_sec + 1]);
            if(i & 0x0001)
            {
                clus_nr = ((clus_nr >> 4) & 0x0fff);
            }
            else
            {
                clus_nr = clus_nr & 0x0fff;
            }

            if ( i != begin_clus_nr && 0 == ( (i - begin_clus_nr) % 8 ) )
            {
                dsm_Printf( BASE_DSM_TS_ID, "\n%2x: ", (i - begin_clus_nr) / 8);
            }
            dsm_Printf( BASE_DSM_TS_ID, "%04x ", clus_nr );

            counter--;
            if ( 0 == counter )
                break;

            i++;
        }
        while(sec_nr < sb_info->iFATStartSec  + sb_info->iFATSize);

        if(sec_nr >= sb_info->iFATStartSec  + sb_info->iFATSize)
        {
            iRet = _ERR_FAT_DISK_FULL;
        }
    }
    else if(sb_info->iFATn == FAT16)
    {
        do
        {
            entry_ofs_in_fat = i * 2;
            sec_nr = sb_info->iFATStartSec  + (entry_ofs_in_fat / sb_info->iBytesPerSec);
            entry_ofs_in_sec = entry_ofs_in_fat % sb_info->iBytesPerSec;

            if(old_sec_nr < sec_nr)
            {
                DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
                iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, sec_nr, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    D( ( DL_FATERROR, (const unsigned char *)"In fat_dump_fat DRV_BLOCK_READ_2 failed iResult = %d\n", iResult));
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    goto end;
                }

                old_sec_nr = sec_nr;
            }

            clus_nr = MAKEFATUINT16(secbuf[entry_ofs_in_sec], secbuf[entry_ofs_in_sec + 1]);
            if ( i != begin_clus_nr && 0 == ( (i - begin_clus_nr) % 8 ) )
            {
                dsm_Printf( BASE_DSM_TS_ID, "\n%2x: ", (i - begin_clus_nr) / 8);
            }
            dsm_Printf( BASE_DSM_TS_ID, "%04x ", clus_nr );

            counter--;
            if ( 0 == counter )
                break;

            i++;
        }
        while(sec_nr < sb_info->iFATStartSec + sb_info->iFATSize);

        if(sec_nr >= sb_info->iFATStartSec + sb_info->iFATSize)
        {
            iRet = _ERR_FAT_DISK_FULL;
        }
    }
    else // FAT32
    {
        entry_ofs_in_fat = begin_clus_nr * 4;
        iRet = _ERR_FAT_NOT_SUPPORT;
    }

    iRet = _ERR_FAT_SUCCESS;

end:
    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }

    return iRet;
}

INT32 g_nvram_hd = 0;
//BOOL NVRAM_Check(CONST UINT32 iId,BOOL bCheckout);
static BOOL shell_NVRAM_Check( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iId;
    BOOL bResult;
    BOOL bCheckout = 0;
    BOOL bRet = TRUE;

    if(argc == 3)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
        bCheckout = (BOOL)(DSM_StrAToI(argv[2]) == 0 ? FALSE : TRUE);
    }
    else if(argc == 2)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    bResult = NVRAM_Check(iId, bCheckout);
    if(bResult)
    {
        bRet = TRUE;
        dsm_Printf( BASE_DSM_TS_ID, "valid");
    }
    else
    {
        bRet = FALSE;
        dsm_Printf( BASE_DSM_TS_ID, "invalid");
    }

    return bRet;
}


static BOOL shell_NVRAM_Foramt( struct cmd_entry *ent, int argc, char **argv )
{
    // UINT32 iId;
    INT32 iResult;
    BOOL bRet = TRUE;

    argv = argv;
    if(argc == 1)
    {

    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    iResult = NVRAM_Format();
    if(ERR_SUCCESS == iResult)
    {
        bRet = TRUE;
        dsm_Printf( BASE_DSM_TS_ID, "ok");
    }
    else
    {
        bRet = FALSE;
        dsm_Printf( BASE_DSM_TS_ID, "fail");
    }

    return bRet;
}


static BOOL shell_NVRAM_Open( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iId;
    BOOL bCheckout = FALSE;
    //INT32 iRet;
    INT32 hd;
    BOOL bRet = TRUE;

    if (argc == 3)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
        bCheckout = (BOOL)(DSM_StrAToI(argv[2]) == 0 ? FALSE : TRUE);
    }
    else if(argc == 2)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    hd = NVRAM_Open(iId, bCheckout);
    if(hd >= 0)
    {
        g_nvram_hd = hd;
        dsm_Printf( BASE_DSM_TS_ID, "hd = 0x%x", hd);
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
        dsm_Printf( BASE_DSM_TS_ID, "NVRAM_Open failed!err_code = %d", hd);
    }

    return bRet;
}


//  { "nvram_remove",shell_NVRAM_Remove, "Remove a nvram entry","nvram_remove <id>"},
static BOOL shell_NVRAM_Remove( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iId;
    INT32 iRet;
    BOOL bRet = TRUE;

    if(argc == 2)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    iRet = NVRAM_Remove(iId);
    if(ERR_SUCCESS == iRet)
    {
        dsm_Printf( BASE_DSM_TS_ID, "ok");
        bRet = TRUE;
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "fail");
        bRet = FALSE;
    }

    return bRet;
}

//  { "nvram_close",shell_NVRAM_Close, "Close a nvram entry","nvram_close <id>"},
static BOOL shell_NVRAM_Close( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;
    INT32 hd;
    BOOL bRet = TRUE;

    if(argc == 2)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    iRet = NVRAM_Close(hd);
    if(ERR_SUCCESS == iRet)
    {
        bRet = TRUE;
        dsm_Printf( BASE_DSM_TS_ID, "ok");
    }
    else
    {
        bRet = FALSE;
        dsm_Printf( BASE_DSM_TS_ID, "fail");
    }

    return bRet;
}


//  { "nvram_isexist",shell_NVRAM_IsExit, "Check a nvram entry if exist","nvram_isexist <id>"},
static BOOL shell_NVRAM_IsExit( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iId;
    BOOL bResult;
    BOOL bRet = TRUE;

    if(argc == 2)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    bResult = NVRAM_IsExist(iId);
    if(bResult)
    {
        bRet = TRUE;
        dsm_Printf( BASE_DSM_TS_ID, "TRUE");
    }
    else
    {
        bRet = FALSE;
        dsm_Printf( BASE_DSM_TS_ID, "FALSE");
    }

    return bRet;
}


//  { "nvram_setvalue",shell_NVRAM_SetValue, "Set value of nvram entry","nvram_setvalue <id>,[offset],[size],[value]"},
static BOOL shell_NVRAM_SetValue( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 hd;
    UINT32 iOffs = 0;
    UINT32 iSize = 1024;
    UINT8   iValue = 0xaa;
    UINT32 iRet;
    UINT8 *pBuff = NULL;
    BOOL bRet = TRUE;

    if (argc == 5)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
        iOffs = DSM_StrAToI(argv[ 2 ]);
        iSize = DSM_StrAToI(argv[ 3 ]);
        iValue = (UINT8)DSM_StrAToI(argv[4]);
    }
    else if(argc == 4)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
        iOffs = DSM_StrAToI(argv[ 2 ]);
        iSize = DSM_StrAToI(argv[ 3 ]);
    }
    else if(argc == 3)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
        iOffs = DSM_StrAToI(argv[ 2 ]);
    }
    else if(argc == 2)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
    }
    else if(argc == 1)
    {
        hd = g_nvram_hd;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    pBuff = DSM_MAlloc(iSize);
    if(pBuff == NULL)
    {
        return FALSE;
    }
    DSM_MemSet(pBuff, iValue, iSize);
    iRet = NVRAM_SetValue(hd, iOffs, iSize, pBuff);
    if(iSize == iRet)
    {
        bRet = TRUE;
        dsm_Printf( BASE_DSM_TS_ID, "ok");
    }
    else
    {
        bRet = FALSE;
        dsm_Printf( BASE_DSM_TS_ID, "fail");
    }
    if(NULL != pBuff)
    {
        DSM_Free(pBuff);
    }
    return bRet;
}


// { "nvram_getvalue",shell_NVRAM_GetValue, "Get value of nvram entry","nvram_getvalue <id>,[offset],[size],[value]"},
static BOOL shell_NVRAM_GetValue( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 hd;
    UINT32 iOffs = 0;
    UINT32 iSize = 1024;
    UINT8   iValue = 0xaa;
    UINT32 iRet;
    UINT8 *pBuff = NULL;
    BOOL bRet = TRUE;

    if (argc == 5)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
        iOffs = DSM_StrAToI(argv[ 2 ]);
        iSize = DSM_StrAToI(argv[ 3 ]);
        iValue = (UINT8)DSM_StrAToI(argv[4]);
    }
    else if(argc == 4)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
        iOffs = DSM_StrAToI(argv[ 2 ]);
        iSize = DSM_StrAToI(argv[ 3 ]);
    }
    else if(argc == 3)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
        iOffs = DSM_StrAToI(argv[ 2 ]);
    }
    else if(argc == 2)
    {
        hd = DSM_StrAToI(argv[ 1 ]);
    }
    else if(argc == 1)
    {
        hd = g_nvram_hd;
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    pBuff = DSM_MAlloc(iSize);
    if(pBuff == NULL)
    {
        return FALSE;
    }
    DSM_MemSet(pBuff, 0, iSize);
    iRet = NVRAM_GetValue(hd, iOffs, iSize, pBuff);
    if(iSize == iRet)
    {
        DSM_Dump(pBuff, iSize, 32);
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    if(NULL != pBuff)
    {
        DSM_Free(pBuff);
    }
    return bRet;
}


// { "nvram_create",shell_NVRAM_Create, "Create nvram entry","nvram_create <id>,<size>"},
static BOOL shell_NVRAM_Create( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iId;
    UINT32 iSize;
    //INT32 iRet;
    INT32 hd;
    BOOL bRet = TRUE;

    if (argc == 3)
    {
        iId = DSM_StrAToI(argv[ 1 ]);
        iSize = DSM_StrAToI(argv[ 2 ]);
        hd = NVRAM_Create(iId, iSize);
        if(hd >= 0)
        {
            g_nvram_hd = hd;
            bRet = TRUE;
            dsm_Printf( BASE_DSM_TS_ID, "hd = 0x%x", hd);
        }
        else
        {
            bRet = FALSE;
            dsm_Printf( BASE_DSM_TS_ID, "NVRAM_Create failed!errcode = 0x%x", hd);
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}
//INT32 NVRAM_GetInfo(UINT32* piTotalSize,UINT32* piUsedSize);
static BOOL shell_NVRAM_GetInfo( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iTotalSize = 0;
    UINT32 iUsedSize = 0;
    INT32 iRet;
    BOOL bRet = TRUE;

    argv = argv;
    if (argc == 1)
    {
        iRet = NVRAM_GetInfo(&iTotalSize, &iUsedSize);
        if(iRet >= 0)
        {
            bRet = TRUE;
            dsm_Printf( BASE_DSM_TS_ID, "total_size = 0x%x,used_size = 0x%x.", iTotalSize, iUsedSize);


        }
        else
        {
            bRet = FALSE;
            dsm_Printf( BASE_DSM_TS_ID, "NVRAM_GetEntries failed!ret = 0x%x", iRet);
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}

//INT32 NVRAM_GetEntries(NVRAM_ENTRY* psEntries,UINT32* piCount);
static BOOL shell_NVRAM_GetEntries( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 i;
    UINT32 iCount = NVRAM_ENTRIES_COUNT;
    NVRAM_ENTRY psEntirs[NVRAM_ENTRIES_COUNT];
    INT32 iRet;
    UINT8 szNName[32];
    UINT8 szID[16];
    UINT8 szBuff[1024];
    FILE *fp;
    UINT32 hd;
    UINT32 iSize;
    UINT32 iOffs = 0;
    BOOL bRet = TRUE;
    INT32 w_size, r_size;

    argv = argv;
    if (argc == 1)
    {
        iRet = NVRAM_GetEntries(psEntirs, &iCount);
        if(iRet >= 0)
        {
            bRet = TRUE;
            for(i = 0; i < iCount; i++)
            {
                dsm_Printf( BASE_DSM_TS_ID, "i =%d,id = %d,size = %d.\n", i, psEntirs[i].iId, psEntirs[i].iSize);
                DSM_MemSet(szID, 0, 16);
                DSM_MemSet(szNName, 0, 32);
                DSM_StrIToA(psEntirs[i].iId, (char *)szID, 10);
                DSM_StrCpy(szNName, "NVRAM_");
                DSM_StrCat(szNName, szID);
                DSM_StrCat(szNName, ".dat");
                fp = fopen((char *)szNName, (char *)"w+");
                if(fp == NULL)
                {
                    dsm_Printf( BASE_FAT_TS_ID, "open file(%s) failed.", szNName);
                    continue;
                }
                hd = NVRAM_Open(psEntirs[i].iId, FALSE);
                if(hd >= 0)
                {
                    if(psEntirs[i].iId == 20101)
                    {
                        iOffs = iOffs;
                    }
                    iOffs = 0;
                    do
                    {
                        iSize =  psEntirs[i].iSize - iOffs > 1024 ? 1024 : psEntirs[i].iSize - iOffs;
                        DSM_MemSet(szBuff, 0, 1024);
                        r_size = NVRAM_GetValue(hd, iOffs, iSize, szBuff);
                        if(r_size <= 0)
                        {
                            break;
                        }
                        iOffs += r_size;
                        w_size = fwrite(szBuff, 1, r_size, fp);
                        if(w_size != r_size)
                        {
                            break;
                        }
                    }
                    while(w_size > 0 && iOffs < psEntirs[i].iSize);
                    NVRAM_Close(hd);
                    fclose(fp);
                    dsm_Printf( BASE_DSM_TS_ID, "i =%d,id = %d,file_size = %d.\n", i, psEntirs[i].iId, iOffs);
                }
            }
        }
        else
        {
            bRet = FALSE;
            dsm_Printf( BASE_DSM_TS_ID, "NVRAM_GetEntries failed!ret = 0x%x", iRet);
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}

//INT32 NVRAM_GetEntriesCount(UINT32* piCount);
static BOOL shell_NVRAM_GetEntriesCount( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iCount = 0;
    INT32 iRet;
    BOOL bRet = TRUE;

    argv = argv;
    if (argc == 1)
    {
        iRet = NVRAM_GetEntriesCount(&iCount);
        if(iRet >= 0)
        {
            bRet = TRUE;
            dsm_Printf( BASE_DSM_TS_ID, "NVRAM Entries Count: %d ", iCount);
        }
        else
        {
            bRet = FALSE;
            dsm_Printf( BASE_DSM_TS_ID, "NVRAM_GetEntries failed!ret = 0x%x", iRet);
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}


VOID fat_dump_nvram(UINT32 iId, UINT8 *pszDestName)
{
    //    INT32 iRet;
    INT32 fd_res = -1;
    FILE *fp_dest = NULL;
    UINT8 szBuff[512];
    UINT32 r_size = 0;
    UINT32 w_size = 0;
    UINT32 iOffs = 0;


    fd_res = NVRAM_Open(iId, FALSE);
    if(fd_res < 0)
    {
        dsm_Printf( BASE_FAT_TS_ID, "open res nvram(%d) failed.", iId);
        goto dump_file_end;
    }

    fp_dest = fopen((char *)pszDestName, (char *)"w+");
    if(fp_dest == NULL)
    {
        dsm_Printf( BASE_FAT_TS_ID, "open res file(%s) failed.", pszDestName);
        goto dump_file_end;
    }
    do
    {
        r_size = NVRAM_GetValue(fd_res, iOffs, 512, szBuff);
        if(r_size > 0)
        {
            w_size = fwrite(szBuff, 1, r_size, fp_dest);
            if(w_size != r_size)
            {
                goto dump_file_end;
            }
        }
        else
        {
            break;
        }
        iOffs += r_size;
    }
    while(r_size > 0);



dump_file_end:
    if(fd_res >= 0)
    {
        NVRAM_Close(fd_res);
        fd_res = -1;
    }

    if(fp_dest >= 0)
    {
        fclose(fp_dest);
        fp_dest = NULL;
    }

    return;

}



extern INT32  REG_OpenKey(HANDLE hKey, PCSTR pcSubKey, HANDLE *phKey);
extern INT32 REG_GetValue(HANDLE hKey, PCSTR pcValueName, UINT8 *pcType, PVOID pvData, UINT8 *pDataSize);

static BOOL shell_REG_OpenKey( struct cmd_entry *ent, int argc, char **argv )
{

    INT32 iRet;
    BOOL bRet = TRUE;
    HANDLE hKey;
    UINT32 root_key;

    if (argc >= 2)
    {
        root_key = DSM_StrAToI(argv[ 1 ]);
        iRet = REG_OpenKey(root_key, argv[ 2 ], &hKey);
        if(iRet != ERR_SUCCESS)
        {
            dsm_Printf( BASE_FAT_TS_ID, "shell_REG_OpenKey: REG_OpenKey failed!errirCode = %d.", iRet);
            return FALSE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "shell_REG_OpenKey: REG_OpenKey ok.hKey = %d.", hKey);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}

static BOOL shell_REG_GetValue( struct cmd_entry *ent, int argc, char **argv )
{

    INT32 iRet;
    UINT8 iType = 0;
    UINT8 iSize = 127;
    UINT8 szBuff[128];
    HANDLE hKey = 0;
    BOOL bRet = TRUE;

    if (argc >= 2)
    {
        hKey = DSM_StrAToI(argv[ 1 ]);
        iRet = REG_GetValue(hKey, argv[ 2 ], &iType, szBuff, &iSize);
        if(iRet != ERR_SUCCESS)
        {
            dsm_Printf( BASE_FAT_TS_ID, "shell_REG_GetValue: REG_GetValue failed!iType = %d,buff = %s.", iRet, szBuff);
            return FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    return bRet;
}

//INT32   nvram_GetEntries(NVRAM_ND_INFO * psEntries,UINT32* piCount);

#define U8 UINT8
#define MAX_FILE_PATH_UNICODE_LEN FS_PATH_UNICODE_LEN
#define OslMalloc DSM_MAlloc
#define OslMfree DSM_Free
#define ConvertRelative2Full DSM_TcStrCpy
#define ConvertDosName2Unix DSM_TcStrCpy
#define FS_PARAM_ERROR (-1)
#define TRACE_FMGR_FUNC(a) CSW_TRACE(DL_FATERROR,a)
#define pfnUnicodeStrlen DSM_UnicodeLen
#define pfnUnicodeStrcpy DSM_TcStrCpy
#define TRACE_EF CSW_TRACE
#define g_sw_FMGR DL_FATERROR
#define FS_NO_ERROR 0
INT64 MMI_FS_GetFileSizeByName(const UINT8 *pcFileName)
{
    INT32 fd;
    INT64 size = 0;
    if(!pcFileName)
        return 0;
    fd = FS_Open(pcFileName, FS_O_RDONLY, 0);
    if(fd >= 0)
    {
        size = FS_GetFileSize(fd);
    }
    else
    {
        size = 0;
        CSW_TRACE(BASE_DSM_TS_ID, "MMI_FS_GetFileSizeByName: open file failed!ret = 0x%08x.", fd);
    }
    return size;
}

INT32 MMI_FS_RemoveDir(const UINT8 *pcDirName)
{
    INT32 fd;
    INT32 i, iDepth = 0;
    INT32 iFindFd[NR_MAX_DEPTH];
    UINT16 *pOriginal, *p = NULL;
    INT32 iRet = FS_ERROR_RESERVED;
    UINT8 c, c2;
    UINT16 UniChar;
    UINT32 iNameLen;
    FS_FIND_DATA sFindData;
    UINT8 strUni[] = {'/', 0, '*', 0, 0, 0};
    UINT8 *pcPath = NULL;
    UINT8 *pcDosPath = NULL;
    INT64 removedSize = 0;
    INT64 tmpfileSize = 0;
    BOOL bLoop = TRUE;
    DSM_ASSERT((BOOL)(pcDirName != NULL), "MMI_FS_RemoveDir");

    pcPath = (UINT8 *)OslMalloc(MAX_FILE_PATH_UNICODE_LEN +
                                LEN_FOR_UNICODE_NULL_CHAR);
    if( NULL == pcPath )
    {
        return FS_ERROR_RESERVED;
    }
    pcDosPath = (UINT8 *)OslMalloc(MAX_FILE_PATH_UNICODE_LEN +
                                   LEN_FOR_UNICODE_NULL_CHAR);
    if( NULL == pcDosPath )
    {
        OslMfree(pcPath);
        pcPath  = NULL;
        return FS_ERROR_RESERVED;
    }

    ConvertRelative2Full((UINT16 *)pcDosPath, (UINT16 *)pcDirName);
    ConvertDosName2Unix((UINT16 *)pcPath, (UINT16 *)pcDosPath);

    // can't remove root-dir
    if (('/' == pcPath[0] || '\\' == pcPath[0])
            && (0 == pcPath[1] && 0 == pcPath[2] && 0 == pcPath[3]))
    {
        iRet = FS_PARAM_ERROR;
        goto failed;
    }
    p = (UINT16 *)pcPath;

    TRACE_FMGR_FUNC("[MMI_FS_RemoveDir:] pcPath");

    while (*p)
    {
        p++;
    }
    while (TRUE == bLoop)
    {
        // 去掉字符串后面多余的'/'。
        UniChar = *(p - 1);
        c = *((UINT8 *)(&UniChar));
        c2 = *((UINT8 *)(&UniChar) + 1);
        if (('/' == c || '\\' == c) && c2 == 0)
        {
            p--;
        }
        else
        {
            break;
        }
    }
    *p = (UINT16)0;
    pOriginal = p;

    for (i = 0; i < NR_MAX_DEPTH; ++i)
    {
        iFindFd[i] = INVALID_FD;
    }
    bLoop = TRUE;
    while (TRUE == bLoop)
    {
        if (INVALID_FD == iFindFd[iDepth])
        {
            if ((UINT32)(p + pfnUnicodeStrlen(strUni)) > (UINT32)(pcPath +
                    MAX_FILE_PATH_UNICODE_LEN))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            pfnUnicodeStrcpy((UINT16 *)p, (UINT16 *)strUni);
            p++;

            fd = FS_FindFirstFile(pcPath, &sFindData);

            if (ERR_FS_NO_MORE_FILES == fd || fd < 0)
            {
                iFindFd[iDepth] = -1;
                goto DIR_NULL;
            }
            iFindFd[iDepth] = fd;
            if ((UINT32)(p + pfnUnicodeStrlen(sFindData.st_name)) > (UINT32)(
                        pcPath + MAX_FILE_PATH_UNICODE_LEN))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            pfnUnicodeStrcpy((UINT16 *)p, (UINT16 *)sFindData.st_name);

            if (! FS_IS_DIR( sFindData.st_mode ))
            {
                tmpfileSize = MMI_FS_GetFileSizeByName(pcPath);
                if ( ERR_SUCCESS != FS_Delete(pcPath) )
                {
                    TRACE_EF(g_sw_FMGR, "[#error!!]: FS_Delete111 return = %d.\n", iRet);
                    iRet = FS_ERROR_RESERVED;
                    goto failed;
                }
                removedSize += tmpfileSize;
                //COS_Sleep(1);
            }
            else if (FS_IS_DIR( sFindData.st_mode ) && '.' == sFindData.st_name[0] && 0 == sFindData.st_name[1])
            {
                ;
            }
            else
            {
                iRet = FS_RemoveDir(pcPath);
                if (ERR_SUCCESS == iRet)
                {
                    ;
                }
                else if (iRet != ERR_FS_DIR_NOT_EMPTY)
                {
                    TRACE_EF(g_sw_FMGR,  "[#error!!]: FS_RemoveDir111 return = %d.\n", iRet);
                    iRet = FS_ERROR_RESERVED;
                    goto failed;
                }
                else
                {
                    goto dir_not_empty;
                }
            }
        }

        while ( ERR_SUCCESS == ( iRet = FS_FindNextFile( iFindFd[iDepth], &sFindData ) ) )
        {
            if ((UINT32)(p + pfnUnicodeStrlen(sFindData.st_name)) > (UINT32)(pcPath + MAX_FILE_PATH_UNICODE_LEN))
            {
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            pfnUnicodeStrcpy((UINT16 *)p, (UINT16 *)sFindData.st_name);

            if (!FS_IS_DIR( sFindData.st_mode ))
            {
                tmpfileSize = MMI_FS_GetFileSizeByName(pcPath);
                if ( ERR_SUCCESS != FS_Delete(pcPath) )
                {
                    TRACE_EF(g_sw_FMGR, "[#error!!]: FS_Delete222 return = %d.\n", iRet);
                    iRet = FS_ERROR_RESERVED;
                    goto failed;
                }
                removedSize += tmpfileSize;
                sxr_Sleep(1);
            }
            else if (FS_IS_DIR( sFindData.st_mode )
                     && '.' == sFindData.st_name[0]
                     && 0 == sFindData.st_name[1])
            {
                ;
            }
            else
            {
                iRet = FS_RemoveDir(pcPath);
                if (ERR_SUCCESS == iRet)
                {
                    ;
                }
                else if (iRet != ERR_FS_DIR_NOT_EMPTY)
                {
                    TRACE_EF(g_sw_FMGR,  "[#error!!]: FS_RemoveDir222 return = %d.\n", iRet);
                    iRet = FS_ERROR_RESERVED;
                    goto failed;
                }
                else
                {
                    goto dir_not_empty;
                }
            }
        }

        if (iRet != ERR_FS_NO_MORE_FILES)
        {
            iRet = FS_ERROR_RESERVED;
            goto failed;
        }

        iRet = FS_FindClose(iFindFd[iDepth]);
        iFindFd[iDepth] = INVALID_FD;
        iDepth--;
        p--;
        *p = (UINT16)0;
DIR_NULL:
        if ((iRet = FS_RemoveDir(pcPath)) != ERR_SUCCESS)
        {
            TRACE_EF(g_sw_FMGR,  "[#error!!]: FS_RemoveDir333 return = %d.\n", iRet);
            //COS_Sleep(100);
            iRet = FS_ERROR_RESERVED;
            goto failed;
        }
        if (p == pOriginal)
        {
            if (iDepth != -1)
            {
                TRACE_EF(g_sw_FMGR,  "[#error!!]: p == pOriginal && iDepth !=0.\n");
                iRet = FS_ERROR_RESERVED;
                goto failed;
            }
            break;
        }

        bLoop = TRUE;
        while (TRUE == bLoop)
        {
            UniChar = *(p - 1);
            c = *((UINT8 *)(&UniChar));
            c2 = *((UINT8 *)(&UniChar) + 1);
            if (!(('/' == c || '\\' == c) && c2 == 0))
            {
                p--;
            }
            else
            {
                break;
            }
        }

        *p = (UINT16)0;
        continue;

dir_not_empty:
        if ((NR_MAX_DEPTH - 1) == iDepth)
        {
            iRet = FS_ERROR_RESERVED;
            goto failed;
        }
        iNameLen = pfnUnicodeStrlen(sFindData.st_name);
        p += iNameLen;
        iDepth++;
    }

    OslMfree(pcPath);
    OslMfree(pcDosPath);
    return FS_NO_ERROR;

failed:

    OslMfree(pcPath);
    OslMfree(pcDosPath);

    TRACE_EF(g_sw_FMGR, "+++++failed !!!+++++.\n", iRet);
    for (i = iDepth; i >= 0; --i)
    {
        TRACE_EF(g_sw_FMGR, "+++++FS_FindClose: iFindFd[%d] = %d +++++.\n", i, iFindFd[i]);
        if(iFindFd[i] >= 0)
        {
            FS_FindClose(iFindFd[i]);
        }
    }
    if(iRet < 0)
    {
        return iRet;
    }
    else
    {
        return (INT32)removedSize;
    }

}


static BOOL shell_FS_RemoveDirRec( struct cmd_entry *ent, int argc, char **argv )
{
    UINT8 *pUName = NULL;
    UINT32 iULen = 0;
    //UINT32 iFileSize;
    INT32 iRet;
    BOOL bRet = TRUE;

    if (argc >= 2)
    {
        iRet = DSM_OEM2Uincode((UINT8 *)argv[ 1 ], DSM_StrLen((UINT8 *)argv[1]), &pUName, &iULen, NULL);
        if(iRet != ERR_SUCCESS)
        {
            dsm_Printf( BASE_FAT_TS_ID, "shell_FS_RemoveDirRec: DSM_OEM2Uincode failed!errirCode = %d.", iRet);
            return FALSE;
        }
        iRet = MMI_FS_RemoveDir(pUName);
        if ( iRet == 0 )
        {
            bRet = TRUE;
        }
        else
        {
            dsm_Printf( BASE_FAT_TS_ID, "shell_FS_RemoveDirRec:MMI_FS_RemoveDir failed!errirCode = %d.", iRet);
            bRet = FALSE;
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        bRet = FALSE;
    }
    if(pUName)
    {
        DSM_Free(pUName);
    }
    return bRet;
}

extern INT32  make_user_data(DSM_CONFIG_T *dsm_config, UINT32 sec_cnt, UINT32 sec_sz, UINT8 **ppuser_data_bin);
extern INT32 vds_TstGetFlashInfo (UINT32 *iSecCnt, UINT32 *iSecSz);
static BOOL shell_make_user_data( struct cmd_entry *ent, int argc, char **argv )
{
#define  USER_DATA_BIN_NAME "user_data.bin"
    UINT8 szFileName[256];
    DSM_CONFIG_T *dsm_config = NULL;
    UINT32 sec_cnt, sec_sz;
    UINT8 *puser_data_bin = NULL;
    UINT8 *p = NULL;
    INT32 iRet;
    BOOL bRet = TRUE;
    UINT32 w_size = 0;
    FILE *fp;
    UINT32 i;

    ent = ent;
    if (argc >= 2)
    {
        strcpy((INT8 *)szFileName, (INT8 *)argv[1]);
    }
    else
    {
        strcpy((INT8 *)szFileName, (INT8 *)USER_DATA_BIN_NAME);
    }
    iRet = vds_TstGetFlashInfo(&sec_cnt, &sec_sz);
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_ERROR, (UINT8 *)"shell_make_user_data: get flash information failed err code = %d.", iRet ));
        return FALSE;
    }
    dsm_config = (DSM_CONFIG_T *)tgt_GetDsmCfg();
    iRet = make_user_data(dsm_config, sec_cnt, sec_sz, &puser_data_bin);
    if ( iRet == 0 )
    {
        bRet = TRUE;
        fp = fopen((char *)szFileName, (char *)"w+");
        if(fp == NULL)
        {
            dsm_Printf( BASE_FAT_TS_ID, "open res file(%s) failed.", szFileName);
        }
        else
        {
            p = puser_data_bin;
            for(i = 0; i < sec_cnt; i++)
            {
                w_size = fwrite(p, 1, sec_sz, fp);
                if(w_size != sec_sz)
                {
                    bRet = FALSE;
                    break;
                }
                p += sec_sz;
            }
            fclose(fp);
        }
    }
    else
    {
        dsm_Printf( BASE_FAT_TS_ID, "shell_make_user_data:MMI_FS_RemoveDir failed!errirCode = %d.", iRet);
        bRet = FALSE;
    }
    if(puser_data_bin)
    {
        DSM_Free(puser_data_bin);
    }
    return bRet;
}

struct cmd_entry g_FatCmdEnts[ ] =
{
    { "ms", shell_module_switch, "module switch for running specified module's command", "ms [FAT|VDS]" },
    { "rep", shell_repeat, "do something repetitively, %LOOP% is the name for loop variable", "rep ( base, step, counter ) command line" },
    { "debug", shell_debug, "debug switch", "debug [VDS|FAT] [on | off] [error | brief | detail]" },
    { "brep", shell_block_repeat, "do something for block repetitively", "brep {[-F]} <counter> {command line}" },
    { "help", shell_help, "print help message", NULL },
    { "version", shell_version, "show the version of the module", "version [vds | fat |vfs | reg | sms]" },
    { "?", shell_help, "print help message", NULL },


    { "init", shell_fs_init, "initiate the file system, yes or no means formating device forcibly or not", "init [YES| NO] <root_dev_name> [quick | normal]"},
    { "format", shell_FS_Format, "Format device as a type of filesystem", "format <dev_name> <filesystem_type[FAT|FFS|MinixFS]> <format_type>"},

    { "mountr", shell_mount_root, "mount root filesystem", "mountr <dev_name>"},
    { "mount", shell_mount, "mount filesystem", "mount [FAT|FFS|MinixFS] <dev_name> <mount_point>"},
    { "unmount", shell_unmount, "unmount filesystem(1/0 means force to unmount or not)", "unmount  <mount_point> [1 | 0]"},
    { "um", shell_unmount, "unmount filesystem(1/0 means force to unmount or not)", "unmount  <mount_point> [1 | 0]"},

    { "ls", shell_FS_Find, "print a directory listing",  "ls {<pathname> [-l]}" },
    { "dir", shell_FS_Find, "print a directory listing(same as ls)",  "dir {<pathname>}" },
    { "find", shell_FS_Find, "find files/subdirectoies", "\n\tfind <pattern>" },
    { "cd", shell_FS_ChangeDir, "change directory", "cd <pathname>" },
    { "pwd", shell_FS_PWD, "get current work directory",  "pwd" },
    { "sfirst", shell_FS_SearchFirst, "search files/subdirectoies first",  "sfirst <pathname>" },
    { "snext", shell_FS_SearchNext, "search files/subdirectoies next",  "snext <fd>" },
    { "sclose", shell_FS_SearchClose, "close search descriptor",  "sclose <fd>" },

    { "eof", shell_FS_EndOfFile, "check if end of file", "eof <fd>" },
    { "create", shell_FS_Create, "create a file by specifying the name", "create <filename> [mode] \n mode can be: A:FS_ATTR_ARCHIVE R: FS_ATTR_RO S: FS_ATTR_SYSTEM H: FS_ATTR_HIDDEN" },
    { "creclo", shell_FS_CreClo, "create a file by specifying the name, and close the handle",  "create <filename> [mode] \n mode can be: A:FS_ATTR_ARCHIVE R: FS_ATTR_RO S: FS_ATTR_SYSTEM H: FS_ATTR_HIDDEN" },
    { "cc",     shell_FS_CreClo, "create a file by specifying the name, and close the handle",  "create <filename>" },
    { "open", shell_FS_Open, "open an existing file to read/write with the flag", "open [PathName] [Flag] [mode ], \n Flag can be: A:FS_O_APPEND C:FS_O_CREAT E:FS_O_EXCL T:FS_O_TRUNC R:FS_O_RDONLY W:FS_O_WRONLY +:FS_O_RDWR \n mode can be: A:FS_ATTR_ARCHIVE R: FS_ATTR_RO S: FS_ATTR_SYSTEM H: FS_ATTR_HIDDEN" },
    { "rm", shell_FS_DeleteFile, "delete the file", "rm <filename>" },
    { "rf", shell_FS_DeleteFile, "delete the file", "rf <filename>" },
    { "read", shell_FS_Read, "read from the file associated with fd", "read <fd> <length>" },
    { "write", shell_FS_Write, "write string to the file associated with fd",  "write <fd> <string>" },
    { "seek", shell_FS_Seek, "seek the file pointer", "seek <fd> <offset> [beg | cur | end ]" },
    { "close", shell_FS_Close, "close file associated with fd", "close <fd>" },
    //  { "bigfile", shell_FS_bigfile, "create a file and write data to it until the disk filled up",  "\n\tbigfile <filename> fixed <chunk-size>\n\tbigfile <filename> random" },
    { "lwrite", shell_FS_Lwrite, "write string to the file associated with fd",  "lwrite <fd> <length>" },

    { "md", shell_FS_MakeDir, "create a directory", "md <pathname> [mode] \n mode can be: A:FS_ATTR_ARCHIVE R: FS_ATTR_RO S: FS_ATTR_SYSTEM H: FS_ATTR_HIDDEN" },
    { "mkdir", shell_FS_MakeDir, "create a directory", "mkdir <pathname> [ mode] \n mode can be: A:FS_ATTR_ARCHIVE R: FS_ATTR_RO S: FS_ATTR_SYSTEM H: FS_ATTR_HIDDEN" },
    { "rd", shell_FS_RemoveDir, "remove a directory", "rd <pathname>" },
    { "rmdir", shell_FS_RemoveDir, "remove a directory", "rmdir <pathname>" },
    { "bigfile", shell_FS_bigfile, "create a file and write data to it until the disk filled up",  "\n\tbigfile <filename> fixed <chunk-size>\n\tbigfile <filename> random" },
    { "bf", shell_FS_bigfile, "create a file and write data to it until the disk filled up",  "\n\tbigfile <filename> fixed <chunk-size>\n\tbigfile <filename> random" },
    { "rn", shell_FS_Rename, "rename a file",  "rename <oldname> <newname>" },

    {
        "dump", shell_FS_FAT_Dump,
        "dump data structure on the disk",
        "\ndump [fatlist] <filename>"
        "\ndump [fatent] <begin_cluster_number>"
        "\ndump [bpb] <dev_name>"
        "\ndump [file] <res_file_name> [res_file_name]"
        "\ndump [nvram] <id> [file_name]"
    },
    { "info", shell_FS_GetFSInfo, "get the dev info", "info <devname>"},
    { "gfatt", shell_FS_GetFileAttr, "get file attribute", "getattr <pathname>"},
    { "sfatt", shell_FS_SetFileAttr, "get file attribute", "getattr <pathname> [mode] \n mode can be: A:FS_ATTR_ARCHIVE R: FS_ATTR_RO S: FS_ATTR_SYSTEM H: FS_ATTR_HIDDEN"},
    { "gvol", shell_FS_GetVolLab, "get the Vol label", "gvol <devname>"},
	{ "svol", shell_FS_SetVolLab, "set the Vol label", "svol <devname> <labelname>"},
	{ "hasformat", shell_FS_HasFormated, "check if the file system was formated", "hasformat<devname>[1 | 2]"},
    { "scandisk", Shell_FS_ScanDisk, "scan disk", "scandisk<devname> <filesystem_type>" },
    { "getname", shell_FS_GetFileName, "get file name with fd", "getname <fd>" },
    { "dtree", shell_DropDirTree, "drop dir tree", "dtree <dir_path>" },
    { "count", shell_Count_file, "count file and dir", "count <dir_path> <flag: FS_FILE_TYPE | FS_DIR_TYPE | FS_RECURSIVE_TYPE>" },
    { "ucmp", shell_UniCmp, "compare unicode string", "ucmp <ustr1> <ustr2>" },
    { "chsize", shell_FS_ChangeSize, "change file size", "chsize <fd> <size>" },
    { "truncate", shell_FS_ChangeSize, "truncate file size", "truncate <fd> <size>" },
    { "getsize", shell_FS_GetSize, "get file size", "getsize <fd>" },
    { "flush", shell_FS_Flush, "update file cache", "flush <fd>" },
    { "reldev", shell_FS_ReleaseDev, "release dev", "release <devname>" },
    { "validname", shell_FS_IsValidFileName, "check valid file name", "validname <filename>" },
    { "devinfo", shell_FS_GetDevInfo, "Get the deviece information", "devinfo" },
    { "tupdate", shell_FS_TUpdate, "Update lod from T-Flash",  "tupdate {<pathname>}" },
    { "dirsize", shell_FS_GetDirSize, "Get directory size",  "dirsize {<pathname>}" },
    { "removedir", shell_FS_RemoveDirRec, "remove dir recursion", "removedir pathname"},
    {"mk_ud", shell_make_user_data, "make user data", "mk_ud pathname"},
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    { "vds_flush", shell_VDS_Flush, "Flush vds cache node",  "vds_flush<nodenumber>" },
    { "vds_flushall", shell_VDS_Flush_All, "Flush all vds cache node", "vds_flushall"},
#endif
    { "nvram_create", shell_NVRAM_Create, "Create nvram entry", "nvram_create <id>,<size>"},
    { "nvram_open", shell_NVRAM_Open, "Open a nvram entry", "nvram_open <id>"},
    { "nvram_remove", shell_NVRAM_Remove, "Remove a nvram entry", "nvram_remove <id>"},
    { "nvram_close", shell_NVRAM_Close, "Close a nvram entry", "nvram_close <id>"},
    { "nvram_isexist", shell_NVRAM_IsExit, "Check a nvram entry if exist", "nvram_isexist <id>"},
    { "nvram_setvalue", shell_NVRAM_SetValue, "Set value of nvram entry", "nvram_setvalue <id>,[offset],[size],[value]"},
    { "nvram_getvalue", shell_NVRAM_GetValue, "Get value of nvram entry", "nvram_getvalue <id>,[offset],[size],[value]"},
    { "nvram_getinfo", shell_NVRAM_GetInfo, "Get nvram information", "nvram_getinfo"},
    { "nvram_getentries", shell_NVRAM_GetEntries, "Get nvram entries", "nvram_getentries"},
    { "nvram_check", shell_NVRAM_Check, "Check nvram entries", "nvram_check <id>,[checkout 0|1] "},
    { "nvram_format", shell_NVRAM_Foramt, "Format nvram", "nvram_format [quick/normal 0|1] "},
    { "nvram_getcount", shell_NVRAM_GetEntriesCount, "Get nvram entries count", "nvram_getcount"},
    { "reg_get", shell_REG_GetValue, "get reg value", "reg_get <key> <valuename> "},
    { "reg_open", shell_REG_OpenKey, "open reg key", "reg_open <1|2|3|4> <sub_key>"},
    { NULL, NULL, NULL, NULL},
};


#if 0
// Print mode defining.

// Print to file.
#define TST_PRINT_MODE_F            0X01

// Print to trace.
#define TST_PRINT_MODE_T            0X02

// Print to file and trace.
#define TST_PRINT_MODE_FT          0X03

// T-Flash device mount point defining.
// Mount the t - flash to root dir.
#define TST_T_MOUNT_ROOT       "/"

// Mount the t - flash to "/t"
#define TST_T_MOUNT_T             "/t"

// Test result file.


#define TEST_PRINT_LEN 512

// Test result base file name .
CONST UINT8 *g_pcTestBase =  "csw_test_result";

// Test Print buffer.
UINT8 g_szTestPrintBuf[TEST_PRINT_LEN + 1];

// Test result file name with path.
UINT8 g_szTestResultFile[FS_PATH_UNICODE_LEN] = {0,};

// The fd of opened file for print.
INT32 g_fp = -1;


//-------------------------------------------------------------------------------------------------------------
// tst_Print
// Print the trace to file or trace tool.
// Parameter
// uMode: Output mode. This parameter has flowing option:
// TST_PRINT_MODE_F: Print to file.
// TST_PRINT_MODE_T: Print to trace.
// TST_PRINT_MODE_FT:Print to file and trace.
// szFormat: print format.
// return value:
//   VOID
//------------------------------------------------------------------------------------------------------------
VOID tst_Print(UINT32 uiMode, CONST INT8 *szFormat, ... )
{
    INT32 iResult;
    va_list va_format;
    UINT32 uiLen = 0;

    // Make the output string.
    va_start( va_format, szFormat );
    DSM_MemSet( g_szTestPrintBuf, 0, TEST_PRINT_LEN );
    uiLen = SUL_StrVPrint( g_szTestPrintBuf, szFormat, va_format );
    if ( uiLen < 0 )
    {
        va_end( va_format );
        return;
    }

    // Add a enter character.
    DSM_StrCat(g_szTestPrintBuf, "\n");

    // write to file.
    if(uiMode & TST_PRINT_MODE_F)
    {
        if(g_fp >= 0)
        {
            FS_Write(g_fp, g_szTestPrintBuf, DSM_StrLen(g_szTestPrintBuf));
            FS_Flush(g_fp);
        }
    }
    va_end( va_format );

    // write to trace.
    if(uiMode & TST_PRINT_MODE_T || g_fp < 0)
    {
        CSW_TRACE(_CSW|TLEVEL(BASE_DSM_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("%s",0x080000a1), g_szTestPrintBuf);
    }
    return ;
}


//-------------------------------------------------------------------------------------------------------------
// tst_DsmInit
// FS initalize function. realized flowing processing flash driver init, mult-language init, vds init, reg init,sms-dm init,
// fs - init,mount t-flash device,create csw test print file etc.
// return value:
//         TRUE:  Success.
//         FALSE: Fail.
//------------------------------------------------------------------------------------------------------------
BOOL tst_DsmInit(VOID)
{
    UINT8 i = 0;
    INT32 iResult = 0x00;
    UINT32 uiDevCount = 0;
    FS_DEV_INFO *pDevInfo = NULL;
    FS_DEV_INFO *pRootDevInfo = NULL;
    UINT32 uiRootDevCount = 0;
    UINT8 *pUName = NULL;
    UINT8 szTFPoint[100];
    UINT32 uiULen = 0;
    BOOL bIsFormated = FALSE;
    BOOL bIsMountRoot = FALSE;
    BOOL bIsMountT = FALSE;
    BOOL bRet = TRUE;

    // Initialize flash
    DRV_FlashPowerUp();
    DRV_FlashInit();

    // Initialize Mult-Language.
    ML_Init();
    ML_SetCodePage("cp936");

    // Initialize VDS.
    iResult = VDS_Init();
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("VDS_Init() OK.\n",0x080000a2));
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("VDS_Init() ERROR, Error code: %d.\n",0x080000a3), iResult);
        bRet = FALSE;
    }

    // Initialize device.
    iResult = DSM_DevInit();
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("DSM_DevInit OK.\n",0x080000a4));
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("DSM_DevInit ERROR, Error code: %d. \n",0x080000a5), iResult);
        bRet = FALSE;
    }

    // Initialize REG.
    iResult = REG_Init();
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTXT(TSTR("REG_Init() OK.\n",0x080000a6)));
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTXT(TSTR("REG_Init() Fail!Error code:%d.\n",0x080000a7)), iResult);
        bRet = FALSE;
    }

    // Initialize SMS_DM.
    iResult = SMS_DM_Init();
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTXT(TSTR("SMS_DM_Init OK.\n",0x080000a8)));
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTXT(TSTR("SMS_DM_Init ERROR, Error code: %d.\n",0x080000a9)), iResult);
        bRet = FALSE;
    }

    // FS power on.
    iResult = FS_PowerOn();
    if(ERR_SUCCESS == iResult)
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("FS Power On Check OK.\n",0x080000aa));
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("FS Power On Check ERROR, Error code: %d. \n",0x080000ab), iResult);
        bRet = FALSE;
    }

    // Get FS device table.
    iResult = FS_GetDeviceInfo(&uiDevCount, &pDevInfo);
    if(ERR_SUCCESS != iResult)
    {
        CSW_TRACE(BASE_DSM_TS_ID, TSTR("Device not register.\n",0x080000ac));
        uiDevCount = 0;
        bRet = FALSE;
    }

    // Format the flash device.if the deivce has not format,shell foramt it ,else do noting.
    // mount the root divice.
    for(i = 0; i < uiDevCount; i++)
    {
        // Format the flash device if it not format.
        iResult = FS_HasFormatted(pDevInfo[i].dev_name, FS_TYPE_FAT);
        if(ERR_FS_HAS_FORMATED == iResult)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("The flash device %s has formated.\n",0x080000ad), pDevInfo[i].dev_name);
            bIsFormated = TRUE;
        }
        else if(ERR_FS_NOT_FORMAT == iResult)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("The flash device %s not format.\n",0x080000ae), pDevInfo[i].dev_name);
            if(FS_DEV_TYPE_FLASH == pDevInfo[i].dev_type)
            {
                iResult = FS_Format(pDevInfo[i].dev_name, FS_TYPE_FAT, FS_FORMAT_QUICK);
                if(iResult == ERR_SUCCESS)
                {
                    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("The flash device %s format ok.\n",0x080000af), pDevInfo[i].dev_name);
                    bIsFormated = TRUE;
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("The flash device %s format error.Error code:%d.\n",0x080000b0), pDevInfo[i].dev_name, iResult);
                    bRet = FALSE;
                }
            }
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("The flash device %s has formated error.Error code:%d.\n",0x080000b1), pDevInfo[i].dev_name, iResult);
            bRet = FALSE;
        }

        // Mount root device.
        if(TRUE == pDevInfo[i].is_root)
        {
            if(TRUE == bIsFormated)
            {
                if(uiRootDevCount > 0)
                {
                    CSW_TRACE(BASE_DSM_TS_ID, TSTR("Warning:The FS root device too more:%d.\n",0x080000b2), uiRootDevCount);
                }

                if(FALSE == bIsMountRoot)
                {
                    iResult = FS_MountRoot(pDevInfo[i].dev_name);
                    if(ERR_SUCCESS == iResult)
                    {
                        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("FS MountRoot(%s) OK.\n",0x080000b3), pDevInfo[i].dev_name);
                        pRootDevInfo = pDevInfo + i;
                        bIsMountRoot = TRUE;
                        if(pRootDevInfo->dev_type == FS_DEV_TYPE_FLASH)
                        {
                            bIsMountT = TRUE;
                            SUL_StrCopy(szTFPoint, TST_T_MOUNT_ROOT);
                        }
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("FS MountRoot(%s) ERROR, Error code: %d. \n",0x080000b4), pDevInfo[i].dev_name, iResult);
                        bRet = FALSE;
                    }
                }

                uiRootDevCount ++;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("Warning:The device %s not format,so can't mount root device\n",0x080000b5), pDevInfo[i].dev_name);
                bRet = FALSE;
            }
        }
    }

    // Make a dircetory for T-Flash device  mount point.
    // Mount the t-flash device.
    if(pRootDevInfo)
    {
        if(FS_DEV_TYPE_FLASH == pRootDevInfo->dev_type)
        {
            SUL_StrCopy(szTFPoint, TST_T_MOUNT_T);
            iResult = ML_LocalLanguage2UnicodeBigEnding((const UINT8 *)szTFPoint, SUL_Strlen(szTFPoint), &pUName, &uiULen, NULL);
            if(ERR_SUCCESS == iResult)
            {
                iResult = FS_MakeDir(pUName, pUName);
                if(ERR_SUCCESS == iResult || ERR_FS_FILE_EXIST == iResult)
                {
                    iResult = FS_Mount(pRootDevInfo->dev_name, pUName, 0, FS_TYPE_FAT);
                    if(ERR_SUCCESS == iResult)
                    {
                        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1)|TSMAP(1), TSTR("Mount the t-flash device[%s] success.",0x080000b6), pDevInfo[i].dev_name);
                    }
                    else
                    {
                        CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Mount the t-flash device[%s] fail.err code = %d.",0x080000b7), pDevInfo[i].dev_name, iResult);
                        bRet = FALSE;
                    }
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("Mount the t-flash device[%s] fail.err code = %d.",0x080000b8), pDevInfo[i].dev_name, iResult);
                    bRet = FALSE;
                }
            }
            else
            {
                CSW_TRACE(BASE_DSM_TS_ID, TSTR("Local2Unicode fail.err code = %d.",0x080000b9), iResult);
                bRet = FALSE;
            }
        }
    }

    // Create the print file.
    if(TRUE == bIsMountT)
    {
        if(NULL != pUName)
        {
            CSW_FFS_FREE(pUName);
            pUName = NULL;
        }

        // Make the print file name with path.
        TM_SYSTEMTIME sSystemTime;
        TM_GetSystemTime(&sSystemTime);
        SUL_StrPrint(g_szTestResultFile, "%s/%s%d-%d-%d-%d-%d.txt",
                     szTFPoint,
                     g_pcTestBase,
                     sSystemTime.uYear,
                     sSystemTime.uMonth,
                     sSystemTime.uDay,
                     sSystemTime.uHour,
                     sSystemTime.uMinute);
        iResult = ML_LocalLanguage2UnicodeBigEnding((const UINT8 *)g_szTestResultFile, SUL_Strlen(g_szTestResultFile), &pUName, &uiULen, NULL);
        if(ERR_SUCCESS == iResult)

        {
            // Create the print file.
            g_fp = FS_Create(pUName, 0);
            if(g_fp < 0)
            {
                CSW_TRACE(BASE_DSM_TS_ID, TSTR("Open the print file fail.err code = %d.",0x080000ba), iResult);
            }
            else
            {
                CSW_TRACE(BASE_DSM_TS_ID, TSTR("Open the print file success.g_fp = %d.",0x080000bb), g_fp);
                bRet = FALSE;
            }
        }
    }

    if(NULL != pUName)
    {
        CSW_FFS_FREE(pUName);
        pUName = NULL;
    }
    return TRUE;
}
#endif


#endif // VDS_SHELL_COMMAND



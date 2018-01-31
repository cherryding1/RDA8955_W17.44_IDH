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

#include "vds_cd.h"
#include "dsm_dbg.h"
#include "dsm_shell.h"
//#include "flash_simu.h"
#include "drv_flash.h"
#include "vds_cd.h"
#include "vds_local.h"
#include "vds_dbg.h"
#include "vds_tools.h"
#include "vds_api.h"
#include "vds_shell.h"
#include "dsm_dev_driver.h"
#include "fat_base.h"
#include "exfat_base.h"
#include "fs.h"
#include "nvram.h"
#include "fs_base.h"

#define DSM_NR_MOD  5

#define TST_VDS_BUFF        512


extern char g_dsm_Prompt[ 32 ];
extern char *g_vds_CommandLines[];
extern UINT32 g_vds_part_id;
extern VDS_FLASH_INFO g_vds_flash_info;

extern UINT32 ML_Init();
extern UINT32 ML_SetCodePage(UINT8 nCharset[12]);
extern struct cmd_entry g_VdsCmdEnts[ ];
extern struct cmd_entry g_FatCmdEnts[ ];

extern int g_dsm_DbgSwtich[ NR_SUBMODULE ];
extern int g_dsm_DbgLevel[ NR_SUBMODULE ];
extern DSM_MODE_GDB g_dsmGbdLevel[];
UINT8 *g_pchar;

struct cmd_entry *g_DsmEnts[ DSM_NR_MOD ] =
{
    g_VdsCmdEnts,
    g_FatCmdEnts,
};

//extern char *g_dsm_ModName[ DSM_NR_MOD ];

INT32 g_DsmCurMod = 1;
extern int g_sector_size;

#ifndef _FS_SIMULATOR_
extern void sxr_Sleep( UINT32 );
#define VDS_SLEEP_VALUE 2
#endif
extern INT32 vds_SetPartInfo();
extern INT32 vds_InitEntry();
BOOL shell_REG_TestAll();
BOOL tst_VDS();
BOOL tst_set_default_registry2();
BOOL tst_set_default_registry1();
BOOL tst_get_default_registry();
int g_vds_shell_multilines_index = 0;
int g_input_mode = INPUT_MODE_FROM_CONSOLE;
int g_offset_in_string = 0;

char g_shell_line[ VDS_SHELL_LINE_LEN_MAX + LEN_FOR_NULL_CHAR ] = { 0 };
UINT8 g_shell_buffer[ VDS_SHELL_BUFFER_LEN_MAX + LEN_FOR_NULL_CHAR ] = { 0 };

char **build_argv( char *cmd_line, int *pArgc );

static char *shell_search_argument( int argc, char **argv, char *argument )
{
    int i;
    for ( i = 0; i < argc; i++ )
    {
        if ( 0 == DSM_StrCaselessCmp( argv[ i ], argument ) )
        {
            return argv[ i ];
        }
    }

    return NULL;
}


/*
example:

For argument list: reg query  ROOT/phonebook -v "item number"
shell_get_argument_by_flag( argc, argv, "-v" ) returns the pointer
to the argument "item number".

For argument list: reg query  ROOT/phonebook -v "item number" -t binary
shell_get_argument_by_flag( argc, argv, "-t" ) returns the pointer
to the argument "binary".
*/
#ifdef DSM_SHELL_NEED
static char *shell_get_argument_by_flag( int argc, char **argv, char *flag )
{
    int i;
    for ( i = 0; i < argc; i++ )
    {
        if ( 0 == DSM_StrCmp( argv[ i ], flag ) )
        {
            if ( i + 1 == argc )
            {
                return NULL;
            }
            return argv[ i + 1 ];
        }
    }

    return NULL;
}
#endif

/*
rep ( base step counter ) command line
*/
BOOL shell_repeat( struct cmd_entry *ent, int argc, char **argv )
{
#define LOOP_VAR_NAME "%LOOP%"
    int base, step, counter;
    int i, j, var_counter;
    BOOL bOperSuccessful;
    BOOL ( *func ) ( struct cmd_entry * ent, int argc, char **argv ) = NULL;
    char *pVars[ 4 ] = {NULL, };
    char *p;

    if ( argc >= 7 )
    {
        if ( 0 != DSM_StrCmp( "(", argv[ 1 ] ) || 0 != DSM_StrCmp( ")", argv[ 5 ] ) )
            goto show_repeat_cmd;
        base = DSM_StrAToI( argv[ 2 ] );
        step = DSM_StrAToI( argv[ 3 ] );
        counter = DSM_StrAToI( argv[ 4 ] );
        if ( 0 == counter )
            counter = 0xFFFFFFFF;

        argc -= 6;
        argv += 6;

        var_counter = 0;
        for ( i = 0; i < argc; i++ )
        {
            //if ( 0 == DSM_StrNCmp( argv[ i ], LOOP_VAR_NAME, DSM_StrLen( LOOP_VAR_NAME ) ) )
            p = strstr( argv[ i ], LOOP_VAR_NAME );
            if ( p )
            {
                if ( var_counter > 4 )
                    goto show_repeat_cmd;

                pVars[ var_counter ] = p;
                var_counter++;
            }
        }

        for ( ent = g_DsmEnts[ g_DsmCurMod ]; ent->name != NULL; ent++ )
        {
            if ( 0 == DSM_StrCmp( ent->name, argv[ 0 ] ) )
            {
                func = ent->func;
                break;
            }
        }
        if ( !func )
        {
            dsm_Printf( BASE_DSM_TS_ID, "command[%s] to repeat not found\n", argv[ 0 ] );
            return FALSE;
        }

        for ( i = 0; i < counter; i++ )
        {
            for ( j = 0; j < var_counter; j++ )
            {
                if ( pVars[ j ] )
                {
                    DSM_StrPrint( pVars[ j ], "%06d", base + i * step );
                }
            }
#ifdef VDS_MAIL_BUG_CHECK
            checkEvent();
#endif

#ifndef _FS_SIMULATOR_

            sxr_Sleep( VDS_SLEEP_VALUE );
#endif

            //dsm_Printf( BASE_DSM_TS_ID, "repeated [%d]\n", i + 1 );
            bOperSuccessful = func( ent, argc, argv );
            if ( !bOperSuccessful )
            {
                break;
            }
        }

        for ( j = 0; j < var_counter; j++ )
        {
            if ( pVars[ j ] )
            {
                DSM_StrPrint( pVars[ j ], "%s", LOOP_VAR_NAME );
            }
        }

        return TRUE;
    }
    else
    {
show_repeat_cmd:
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


struct dsm_shell_block_repeat g_shell_cmd_stack[ DSM_SHELL_CMD_STACK_HEIGTH ];


static BOOL do_block_repeat( struct dsm_shell_block_repeat *stack, BOOL bForceCountine, int begin )
{
    struct cmd_entry *ent;
    int i;
    int stack_height;
    BOOL bOperSuccessful;

    for ( i = 0; i < stack[ begin ].counter; i++ )
    {
#ifndef _FS_SIMULATOR_
        sxr_Sleep( VDS_SLEEP_VALUE );
#endif

        for ( stack_height = begin + 1; stack_height < stack[ begin ].end_index; stack_height++ )
        {
            if ( SHELL_BLK_REP_STATEMENT_TYPE_END == g_shell_cmd_stack[ stack_height ].type )
            {
                continue;
            }

            for ( ent = g_DsmEnts[ g_DsmCurMod ]; ent->name != NULL; ent++ )
            {
                if ( g_shell_cmd_stack[ stack_height ].argv &&
                        0 == DSM_StrCmp( ent->name, g_shell_cmd_stack[ stack_height ].argv[ 0 ] ) )
                {
                    if ( 0 == DSM_StrCmp( "brep", g_shell_cmd_stack[ stack_height ].argv[ 0 ] ) )
                    {
                        do_block_repeat( g_shell_cmd_stack, bForceCountine, stack_height );
                        stack_height = g_shell_cmd_stack[ stack_height ].end_index;
                    }
                    else
                    {
                        //dsm_Printf( BASE_DSM_TS_ID, "%s\n", g_shell_cmd_stack[ stack_height ].argv[ 0 ] );
                        bOperSuccessful = ent->func( ent, g_shell_cmd_stack[ stack_height ].argc, g_shell_cmd_stack[ stack_height ].argv );
                        if ( !bOperSuccessful && !bForceCountine )
                        {
                            dsm_Printf( BASE_DSM_TS_ID, "%s failed, return from brep\n", g_shell_cmd_stack[ stack_height ].argv[ 0 ] );
                            return FALSE;
                        }
                    }
                }
            }
        }

        dsm_Printf( BASE_DSM_TS_ID, "\n\n+++++++++++++REPEAT ONCE(%d)+++++++++++++\n\n", i );
    }

    dsm_Printf( BASE_DSM_TS_ID, "\n" );
    return TRUE;
}

/*
在执行block中的诸command时，缺省只要一个command的返回结果为FALSE，
就不再继续执行后面的语句，而是直接返回。
但如果给出-F标志，则不论如何一个command的返回结果，而一直执行直到指定的次数。
brep {[-F]} <counter> {
command-line
command-line
}
*/
BOOL shell_block_repeat( struct cmd_entry *ent, int argc, char **argv )
{
    int i, stack_height = 0;
    struct dsm_shell_block_repeat *blk;
    int match_stack[ 32 ] = { 0 };
    int match_stack_ptr = 0;
    BOOL bForceCountine = FALSE;

    if ( argc < 3 || 0 != DSM_StrCmp( argv[ 2 ], "{" ) )
    {
        goto show_list_cmd;
    }

    if ( 4 == argc )
    {
        bForceCountine = TRUE;
    }

    for ( stack_height = 0; stack_height < DSM_SHELL_CMD_STACK_HEIGTH; stack_height++ )
    {
        blk = g_shell_cmd_stack + stack_height;

        blk->type = SHELL_BLK_REP_STATEMENT_TYPE_UNDEFINED;
        blk->counter = 0;
        blk->argc = 0;
        blk->argv = NULL;
        DSM_MemSet( blk->cmd, 0, sizeof( blk->cmd ) );
    }

    //read line continuely until the last "}" is read and save line in stack
    blk = g_shell_cmd_stack + 0;
    blk->type = SHELL_BLK_REP_STATEMENT_TYPE_BEGIN;
    blk->counter = DSM_StrAToI( argv[ 1 ] );
    if ( 0 == blk->counter )
        blk->counter = 0xFFFFFFFF;
    match_stack[ match_stack_ptr++ ] = 0;

    i = 1;
    while ( i < DSM_SHELL_CMD_STACK_HEIGTH )
    {
#ifdef _FS_SIMULATOR_
        if ( NULL == fgets( g_shell_line, sizeof( g_shell_line ), stdin ) )
        {
            return FALSE;
        }
#else
        if ( ! vds_GetLineFromStr( g_shell_line, sizeof( g_shell_line ),
                                   g_vds_CommandLines[ g_vds_shell_multilines_index ], &g_offset_in_string ) )
        {
            return FALSE;
        }
#endif

        if ( 0 == DSM_StrCmp( g_shell_line, "\n" ) )
            continue;

        blk = g_shell_cmd_stack + i;

        DSM_StrNCpy( blk->cmd, g_shell_line, VDS_SHELL_LINE_LEN_MAX );
        if ( 0 == DSM_StrNCmp( "brep ", g_shell_line, 5 ) )
        {
            blk->type = SHELL_BLK_REP_STATEMENT_TYPE_BEGIN;
            match_stack[ match_stack_ptr++ ] = i;
        }
        else if ( 0 == DSM_StrNCmp( "}", g_shell_line, 1 ) )
        {
            blk->type = SHELL_BLK_REP_STATEMENT_TYPE_END;
            match_stack_ptr--;
            g_shell_cmd_stack[ match_stack[ match_stack_ptr ] ].end_index = i;
        }
        else
        {
            blk->type = SHELL_BLK_REP_STATEMENT_TYPE_COMMON;
        }

        i++;

        if ( 0 == match_stack_ptr )
            break;
    }

    blk = g_shell_cmd_stack + 0;
    DSM_StrPrint( blk->cmd, "%s %d {\n", "brep", blk->counter );

    //parse command line
    for ( stack_height = 0; stack_height < DSM_SHELL_CMD_STACK_HEIGTH; stack_height++ )
    {
        blk = g_shell_cmd_stack + stack_height;

        if ( SHELL_BLK_REP_STATEMENT_TYPE_UNDEFINED == blk->type )
        {
            break;
        }
        else if ( SHELL_BLK_REP_STATEMENT_TYPE_END != blk->type )
        {
            /*对语句"}"无需保存*/
            g_shell_cmd_stack[ stack_height ].argv = build_argv( blk->cmd, &g_shell_cmd_stack[ stack_height ].argc );
        }

        if ( SHELL_BLK_REP_STATEMENT_TYPE_BEGIN == blk->type )
        {
            blk->counter = DSM_StrAToI( g_shell_cmd_stack[ stack_height ].argv[ 1 ] );
            if ( 0 == blk->counter )
                blk->counter = 0xFFFFFFFF;
        }
    }

    do_block_repeat( g_shell_cmd_stack, bForceCountine, 0 );

    for ( stack_height = 0; stack_height < DSM_SHELL_CMD_STACK_HEIGTH; stack_height++ )
    {
        if ( g_shell_cmd_stack[ stack_height ].argv )
        {
            vds_free( g_shell_cmd_stack[ stack_height ].argv );
            g_shell_cmd_stack[ stack_height ].argv = NULL;
        }
    }

    return TRUE;
show_list_cmd:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}


BOOL shell_help( struct cmd_entry *ent, int argc, char **argv )
{
    argv = argv;
    argc = argc;
    dsm_Printf( BASE_DSM_TS_ID, "Command supported:\n" );
    for ( ent = g_DsmEnts[ g_DsmCurMod ]; ent->name != NULL; ent++ )
    {
        dsm_Printf( BASE_DSM_TS_ID, "%8s - %s\n", ent->name, ent->help );
    }
    return TRUE;
}


BOOL shell_debug( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iModId;

    if ( argc >= 3 )
    {
        if(DSM_StrCaselessCmp(argv[1], "FAT") == 0)
        {
            iModId = DSM_MOD_FAT;
        }
        else if(DSM_StrCaselessCmp(argv[1], "VDS") == 0)
        {
            iModId = DSM_MOD_VDS;
        }
        else
        {
            goto show_dbg_cmd;
        }

        if ( !DSM_StrCmp( argv[ 2 ], "on" ) )
        {
            g_dsm_DbgSwtich[ iModId ] = 1;
            g_dsm_DbgLevel[ iModId ] = DL_WARNING;

            if ( 4 == argc )
            {
                if ( !DSM_StrNCmp( argv[ 3 ], "error", 2 ) )
                    g_dsm_DbgLevel[ iModId ] = DL_ERROR;
                else if ( !DSM_StrNCmp( argv[ 3 ], "warning", 2 ) )
                    g_dsm_DbgLevel[ iModId ] = DL_WARNING;
                else if ( !DSM_StrNCmp( argv[ 3 ], "brief", 2 ) )
                    g_dsm_DbgLevel[ iModId ] = DL_BRIEF;
                else if ( !DSM_StrNCmp( argv[ 3 ], "detail", 2 ) )
                    g_dsm_DbgLevel[ iModId ] = DL_DETAIL;
                else
                    goto show_dbg_cmd;

                return TRUE;
            }
            else
                goto show_dbg_cmd;
        }
        else if ( !DSM_StrCmp( argv[ 2 ], "off" ) )
        {
            g_dsm_DbgSwtich[ iModId ] = 1;
            g_dsm_DbgLevel[ iModId ] = DL_WARNING;

            return TRUE;
        }
        else
        {
            goto show_dbg_cmd;
        }
    }
    else
    {
show_dbg_cmd:
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }
}


BOOL shell_module_switch( struct cmd_entry *ent, int argc, char **argv )
{
    if ( argc < 2 )
        goto show_cmd_help;

    if(DSM_StrCaselessCmp(argv[1], "FAT") == 0)
    {
        dsm_Printf( BASE_DSM_TS_ID, "switch modulule: %s ==> %s\n",
                    g_dsmGbdLevel[ g_DsmCurMod + 1].szModName, g_dsmGbdLevel[ 2  ].szModName);
        g_DsmCurMod = 1;

    }
    else if(DSM_StrCaselessCmp(argv[1], "VDS") == 0)
    {
        dsm_Printf( BASE_DSM_TS_ID, "switch modulule: %s ==> %s\n",
                    g_dsmGbdLevel[ g_DsmCurMod + 1 ].szModName,  g_dsmGbdLevel[1].szModName);
        g_DsmCurMod = 0;
    }
    else
    {
        goto show_cmd_help;
    }

    return TRUE;

show_cmd_help:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}


BOOL shell_version( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iModId;
    UINT8 szModStr[32] = {0,};
    UINT8 szVersion[ 32 ] = {0,};

    if ( argc < 2 )
        goto show_cmd_help;

    DSM_StrCpy(szModStr, argv[1]);
    DSM_StrUpper(szModStr);
    if ( !DSM_StrNCmp( szModStr, "VDS", 3 ) )
    {
        iModId = DSM_MOD_VDS;
    }
    else if ( !DSM_StrNCmp( szModStr, "FAT", 3 ) )
    {
        iModId = DSM_MOD_FAT;
    }
    else if ( !DSM_StrNCmp( szModStr, "REG", 3 ) )
    {
        iModId = DSM_MOD_REG;
    }
    else if ( !DSM_StrNCmp( szModStr, "VFS", 3 ) )
    {
        iModId = DSM_MOD_VFS;
    }
    else if ( !DSM_StrNCmp( szModStr, "SMS", 3 ) )
    {
        iModId = DSM_MOD_SMS;
    }
    else
    {
        goto show_cmd_help;
    }

    DSM_GetVersion( iModId, (INT8 *)szVersion );

    dsm_Printf( BASE_VDS_TS_ID, "  %s: %s", szModStr, szVersion );

    return TRUE;

show_cmd_help:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}


/*
parse the command line pointed to by @cmd_line

If successfully, the array which contains the pointers to the parameter parsed  is returned.
The number of parameters parsed is returned by *pArgc.

由一对DOUBLE_QUOTE("")括起来的串被理解为一个参数
由一对SINGLE_QUOTE('')括起来的串被理解为一个参数
在一对DOUBLE_QUOTE("")之间的BACK_SLASH(\)用于对特殊字符进行转义
在一对SINGLE_QUOTE('')之间的BACK_SLASH(\)用于对特殊字符进行转义
不在一对SINGLE_QUOTE(或DOUBLE_QUOTE之间的BACK_SLASH(\)用于对特殊字符进行转义

例如:
命令行
md abc\'\'
将创建目录abc''

最多支持DEFAULT_NR_PARAS个参数
caller负责释放在本函数中为the array which contains the pointers 所分配的空间。
但是不需要释放the pointers which are included in the array.
*/
char **build_argv( char *cmd_line, int *pArgc )
{
#define DEFAULT_NR_PARAS 16

    char **argv;
    long i;

    *pArgc = 0;

    argv = vds_malloc( DEFAULT_NR_PARAS * sizeof( char * ) );
    if ( !argv )
    {
        dsm_Printf( BASE_DSM_TS_ID, "[ERROR]: malloc failed\n" );
        return NULL;
    }

    for ( i = 0; i < DEFAULT_NR_PARAS; i++ )
        argv[ i ] = NULL;

    while ( *cmd_line )
    {
        /* skip intervening white space */
        while ( NULL_CHAR != *cmd_line && IsSeparator( *cmd_line ) )
            cmd_line++;

        if ( NULL_CHAR == *cmd_line )
            break;

        if ( DOUBLE_QUOTE == *cmd_line )
        {
            argv[ *pArgc ] = ++cmd_line;
            while ( *cmd_line && DOUBLE_QUOTE != *cmd_line )
            {
                if ( BACK_SLASH == *cmd_line )
                    DSM_StrCpy( cmd_line, cmd_line + 1 );  /* copy everything down */
                cmd_line++;
            }
        }
        else if ( SINGLE_QUOTE == *cmd_line )
        {
            argv[ *pArgc ] = ++cmd_line;
            while ( *cmd_line && SINGLE_QUOTE != *cmd_line )
            {
                if ( BACK_SLASH == *cmd_line )
                    DSM_StrCpy( cmd_line, cmd_line + 1 );  /* copy everything down */
                cmd_line++;
            }
        }
        else
        {
            argv[ *pArgc ] = cmd_line;
            while ( *cmd_line && !IsSeparator( *cmd_line ) )
            {
                if ( BACK_SLASH == *cmd_line )
                    DSM_StrCpy( cmd_line, cmd_line + 1 );  /* copy everything down */
                cmd_line++;
            }
        }

        if ( NULL_CHAR != *cmd_line )
            * cmd_line++ = NULL_CHAR;   /* chop the string to insure argv[*pArgc] terminated properly*/

        *pArgc = *pArgc + 1;
        if ( *pArgc >= DEFAULT_NR_PARAS - 1 )
        {
            vds_free( argv );
            return NULL;
        }
    }

    return argv;
}

static void shell_main( long input_mode, long index )
{
    int argc;
    char **argv = NULL;
    struct cmd_entry *ent;
    char section_end_str[] = "[END_OF_SECTION]";
    BOOL bOperSuccessful = FALSE;
    BOOL bHasHint = FALSE;
    BOOL cheat = TRUE;

    index = index;

    while ( cheat )
    {
#ifdef _FS_SIMULATOR_
        if ( INPUT_MODE_FROM_CONSOLE == input_mode )
        {
            dsm_Printf( BASE_DSM_TS_ID, "\n%s", g_dsm_Prompt );
            fflush( stdout );
        }
        if ( NULL == fgets( g_shell_line, sizeof( g_shell_line ), stdin ) )
        {
            break;
        }
#else
        if ( ! vds_GetLineFromStr( g_shell_line, sizeof( g_shell_line ), g_vds_CommandLines[ index ], &g_offset_in_string ) )
        {
            break;
        }
#endif

        if ( '\n' == g_shell_line[ 0 ] || ( g_shell_line[ 0 ] == '/' && g_shell_line[ 1 ] == '/' ) )
            continue;

        /*在回归测试时，输出此提示信息，以验证
        bug是否重新被引入*/
        if ( 0 == DSM_StrNCmp( g_shell_line, "#bug", 4 ) )
        {
            if ( !bOperSuccessful )
            {
                if ( DSM_StrLen( g_shell_line ) <= TS_OUTPUT_LEN_MAX )
                {
                    dsm_Printf( BASE_DSM_TS_ID, "%s\n", g_shell_line );
                }
                else
                {
                    dsm_Printf( BASE_DSM_TS_ID, "bug description is too long to display" );
                }
            }
            else if ( !bHasHint )
            {
                dsm_Printf( BASE_DSM_TS_ID, "#BUG NOT FOUND .\n"  );
                bHasHint = TRUE;
            }

            continue;
        }

        /*
        当读一个文件时，shell_read()返回TRUE并不一定表明read正确。
        为此，在回归测试时，我们可能需要特别指明read出来的数据是什么时
        才算正确。#note关键字就提供了这样一个手段。
        */
        if ( 0 == DSM_StrNCmp( g_shell_line, "#note", 4 ) || 0 == DSM_StrNCmp( g_shell_line, "#NOTE", 4 ) )
        {
            if ( DSM_StrLen( g_shell_line ) <= TS_OUTPUT_LEN_MAX )
            {
                dsm_Printf( BASE_DSM_TS_ID, "%s\n", g_shell_line );
            }
            else
            {
                dsm_Printf( BASE_DSM_TS_ID, "note description is too long[%d] to display\n", DSM_StrLen( g_shell_line ) );
            }
            continue;
        }

        if ( 0 == DSM_StrNCmp( g_shell_line, section_end_str, sizeof( section_end_str ) - 1 ) )
        {
            return ;
        }

        /*将命令行文件中的命令输入到stdout中，提高stdout
        的可读性*/
        if ( INPUT_MODE_FROM_CONSOLE != input_mode )
        {
            if ( DSM_StrLen( g_dsm_Prompt ) + DSM_StrLen( g_shell_line ) >= TS_OUTPUT_LEN_MAX )
            {
                /*dsm_Printf()最多只能输出128字节，否则系统reset*/
                dsm_Printf( BASE_DSM_TS_ID, "\n%s%s\n", g_dsm_Prompt, "command line is too long to display" );
            }
            else
            {
                dsm_Printf( BASE_DSM_TS_ID, "\n%s%s", g_dsm_Prompt, g_shell_line );
            }
        }

        argc = 0;
        argv = build_argv( g_shell_line, &argc );
        if ( NULL == argv || 0 == argc )
        {
            if ( argv )
            {
                vds_free( argv );
            }
            continue;
        }

        for ( ent = g_DsmEnts[ g_DsmCurMod ]; ent->name != NULL; ent++ )
        {
            if ( 0 == DSM_StrCmp( ent->name, argv[ 0 ] ) )
            {
                UINT32 beginning = 0, end = 0;
                BOOL bTimeTest;
                bTimeTest = ( BOOL ) ( shell_search_argument( argc, argv, "-time" ) ? TRUE : FALSE );

                if ( bTimeTest )
                {
                    beginning = DSM_Time( NULL );
                    argc--;
                }
                bOperSuccessful = ent->func( ent, argc, argv );
                if ( bTimeTest )
                {
                    end = DSM_Time( NULL );
                    dsm_Printf( BASE_DSM_TS_ID, "%d seconds %d ticks elapsed!\n",
                                ( end - beginning ) / 256,
                                ( end - beginning ) % 256 );
                }

                break;
            }
        }

        if ( !DSM_StrNCmp( argv[ 0 ], "quit", 4 ) ||
                !DSM_StrNCmp( argv[ 0 ], "exit", 4 ) ||
                !DSM_StrNCmp( argv[ 0 ], "bye", 3 ) )
            break;

        if ( ent->name == NULL && argv[ 0 ][ 0 ] != '\0' )
            dsm_Printf( BASE_DSM_TS_ID, "command '%s' not understood\n", &argv[ 0 ][ 0 ] );

        vds_free( argv );
    }
}

#ifdef _FS_SIMULATOR_
/*
在文件中检查指定的section是否存在(其index和section_id一致)
,如果section存在,则file_pointer将指向该section的header的下一个命令行.
否则,file_pointer将指向文件尾.
*/
BOOL get_section( FILE *fp, int section_id )
{
    char line[ 1024 ];
    char section_begin_str[ 16 ] = { 0 };

    DSM_StrPrint( section_begin_str, "[SECTION %d]", section_id );
    while ( NULL != fgets( line, sizeof( line ), fp ) )
    {
        if ( DSM_StrNCmp( line, section_begin_str, DSM_StrLen( section_begin_str ) ) )
        {
            continue;
        }
        return TRUE;
    }
    return FALSE;
}
#else

/*
在multilines中检查指定的section是否存在(其index和section_id一致)
,如果section存在,则g_offset_in_string将指向该section的header的下一个命令行.
否则,g_offset_in_string将指向文件尾.
*/
BOOL get_section_from_multilnes( char *str, int section_id )
{
    char line[ 64 ];
    char section_begin_str[ 16 ] = { 0 };

    DSM_StrPrint( section_begin_str, "[SECTION %d]", section_id );

    while ( vds_GetLineFromStr( line, sizeof( line ), str, &g_offset_in_string ) )
    {
        if ( DSM_StrNCmp( line, section_begin_str, DSM_StrLen( section_begin_str ) ) )
        {
            continue;
        }
        return TRUE;
    }
    return FALSE;
}
#endif

#ifdef _FS_SIMULATOR_
extern INT32 VDS_GetFlashInfo( VDS_FLASH_INFO *psInfo );
INT32 shell_create_image_file()
{
    VDS_FLASH_INFO sFlashInfo;
    UINT32 iSecAddr;
    UINT32 iRSize = 0;
    UINT8 szBuff[512];
    INT32 iResult;
    UINT32 i;

    iResult = VDS_GetFlashInfo( &sFlashInfo );
    if ( ERR_SUCCESS != iResult )
    {
        dsm_Printf( BASE_DSM_TS_ID,  "shell_create_image_file: get flash information failed err code = %d.", iResult);
        return -1;
    }

    for(i = 0; i < sFlashInfo.iNrSector; i++)
    {
        iSecAddr = i * sFlashInfo.iFlashSectorSize;
        iResult = DRV_ReadFlash(iSecAddr + sFlashInfo.iFlashSectorSize - 512, szBuff, 512, &iRSize);
        if(ERR_SUCCESS !=  iResult || 512 != iRSize)
        {
            dsm_Printf( BASE_DSM_TS_ID,  "shell_create_image_file: erase sector failed err code = %d, addr = 0x%x,RSize = %d.", iResult, iSecAddr, iRSize);
            break;
        }
    }

    if(i == sFlashInfo.iNrSector)
    {
        return 0;
    }

    for(i = 0; i < sFlashInfo.iNrSector; i++)
    {
        iSecAddr = i * sFlashInfo.iFlashSectorSize;
        iResult = vds_EraseSec(iSecAddr);
        if(ERR_SUCCESS !=  iResult)
        {
            dsm_Printf( BASE_DSM_TS_ID,  "shell_create_image_file: erase sector failed err code = %d, addr = 0x%x.", iResult, iSecAddr);
            return -2;
        }
    }
    return 0;
}

#endif

#ifdef _FS_SIMULATOR_
/*
在PC环境下执行从console输入的command序列

prompt是否打开debug开关
根据answer设置debug次数
prompt是否format flash
根据answer决定是否调用FS_Format()
调用FS_MountRoot
调用shell_main()从stdin获取command并处理之直到遇到exit command
*/
static int shell_main_from_console( void )
{
    //unsigned int c;
    UINT32 i;
    INT32 iResult;
    DSM_DEV_INFO *pDevInfo = NULL;
    UINT32 iDevCount = 0;
    char szFsRootDev[16] = {0,};

    g_input_mode = INPUT_MODE_FROM_CONSOLE;

    //  g_dsm_DbgSwtich[ DSM_MOD_VDS ] = 1;
    //  g_dsm_DbgLevel[ DSM_MOD_VDS ] =  DL_WARNING;

    /********************** VDS init BEGIN **********************/
    /*    dsm_Printf( BASE_DSM_TS_ID, ">Do you want to debug for VDS moudule? [N]\n" );
        c = getc( stdin );

        g_dsm_DbgSwtich[ DSM_MOD_VDS ] = 1;
        g_dsm_DbgLevel[ DSM_MOD_VDS ] = ( c == 'Y' || c == 'y' ) ? DL_DETAIL : DL_WARNING;

        fflush( stdin );
    */
#if 0
    dsm_Printf( BASE_DSM_TS_ID, ">Do you want to format VDS device? [N]\n" );
    c = getc( stdin );
    if ( c == 'Y' || c == 'y' )
    {

        if ( ERR_SUCCESS != VDS_Init())
        {
            dsm_Printf( BASE_DSM_TS_ID, "Initiate VDS failed!\n" );
            return -1;
        }
        else
        {
            dsm_Printf( BASE_DSM_TS_ID, "Initiate VDS OK!\n" );
        }

        vds_TstUpdateflashMap();
    }
#endif
    DRV_FlashInit();
    if ( ERR_SUCCESS != VDS_Init())
    {
        shell_create_image_file();
        if ( ERR_SUCCESS != VDS_Init())
        {
            dsm_Printf( BASE_DSM_TS_ID, "Initiate VDS failed!\n" );
            return -1;
        }
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "Initiate VDS OK!\n" );
    }

    //vds_TstOpenFlash();  // 打开文件，用于保存vds-flash信息

    /********************** VDS init END **********************/

    /* Open all devices registered*/


    ML_Init();
    ML_SetCodePage("CP936");
    DSM_DevInit();
    REG_Init();

    iResult = NVRAM_Init();
    if ( ERR_SUCCESS == iResult )
    {
        dsm_Printf( BASE_DSM_TS_ID, "NVRAM init OK!\n");
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "NVRAM init failed! errcode = %d\n", iResult);
        //return -1;
    }
    FS_PowerOn();
#if 1
    dsm_Printf( BASE_DSM_TS_ID, ">Select the device which contains root filesystem [1]:\n" );
    iResult = DSM_GetDeviceInfo(&iDevCount, &pDevInfo);
    if(iResult != 0)
    {
        dsm_Printf( BASE_DSM_TS_ID, "\nGet device table is failed.return value = %d.\n", iResult );
    }

    for ( i = 0; i < iDevCount; i++ )
    {
        if ( pDevInfo[i].dev_name )
        {

            if(pDevInfo[i].module_id == DSM_MODULE_FS_ROOT)
            {
                DSM_StrCpy(szFsRootDev, pDevInfo[i].dev_name );
                dsm_Printf( BASE_DSM_TS_ID, "\t%d ==> %s(FS Root)\n", i, pDevInfo[i].dev_name );
            }
            else if (pDevInfo[i].module_id == DSM_MODULE_FS)
            {
                dsm_Printf( BASE_DSM_TS_ID, "\t%d ==> %s\n", i, pDevInfo[i].dev_name );
            }
            if(pDevInfo[i].module_id == DSM_MODULE_FS_ROOT ||
                    pDevInfo[i].module_id == DSM_MODULE_FS)
            {
                iResult = FS_HasFormatted( (PCSTR)pDevInfo[i].dev_name, FS_TYPE_FAT );
                if ( ERR_FS_HAS_FORMATED != iResult )
                {
                    dsm_Printf( BASE_DSM_TS_ID, "WARNING: File system partition %s device not formatted! iResult = %d\n", pDevInfo[i].dev_name, iResult );
                    iResult = FS_Format( (PCSTR)pDevInfo[i].dev_name, FS_TYPE_FAT, 0 );
                    if ( ERR_SUCCESS == iResult )
                    {
                        dsm_Printf( BASE_DSM_TS_ID, "Format FS on %s device OK!\n", pDevInfo[i].dev_name );
                    }
                    else
                    {
                        dsm_Printf( BASE_DSM_TS_ID, "Format FS on %s device failed. error code = %d!\n", pDevInfo[i].dev_name, iResult );
                        return -1;
                    }
                }
            }
        }

    }
    /*
    c = getc( stdin );
    if ( c > '0' && c < '0' + iDevCount && pDevInfo[c - '0'].dev_name )
    {
        DSM_StrCpy(szFsRootDev,pDevInfo[c - '0'].dev_name );
    }
    else
    {
       // do noting
    }
      */
    dsm_Printf( BASE_DSM_TS_ID, "\nRoot filesystem in %s device!!!\n", szFsRootDev );
    fflush( stdin );


    /********************** fat init  BEGIN **********************/
    iResult = FS_HasFormatted( (PCSTR)szFsRootDev, FS_TYPE_FAT );
    if ( ERR_FS_HAS_FORMATED != iResult )
    {
        dsm_Printf( BASE_DSM_TS_ID, "WARNING: File system on root device not formatted! iResult = %d\n", iResult );
        iResult = FS_Format( (PCSTR)szFsRootDev, FS_TYPE_FAT, 0 );
        if ( ERR_SUCCESS == iResult )
        {
            dsm_Printf( BASE_DSM_TS_ID, "Format FS on root device OK!\n" );
        }
        else
        {
            dsm_Printf( BASE_DSM_TS_ID, "Format FS on root device failed. error code = %d!\n", iResult );
            return -1;
        }
    }

    iResult = FS_MountRoot( (PCSTR)szFsRootDev );
    // iResult = FS_MountRoot( (PCSTR)"VDS1" );
    if ( ERR_SUCCESS == iResult )
    {
        dsm_Printf( BASE_DSM_TS_ID, "Mount File System Successfully!\n" );
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "Mount File System Failed with errocode %d!\n", iResult );
        return -1;
    }
#ifdef USER_DATA_CACHE_SUPPORT
    VDS_CacheInit();
    VDS_CacheFlushAlloc();
#endif
#else
    //iResult = FS_MountRoot( (PCSTR)"MMC0" );
    iResult = FS_MountRoot( (PCSTR)"VDS0" );
    if ( ERR_SUCCESS == iResult )
    {
        dsm_Printf( BASE_DSM_TS_ID, "Mount File System Successfully!\n" );
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "Mount File System Failed with errocode %d!\n", iResult );
        return -1;
    }

    // T_UP_Main();
    //    uplod_main();
    //    return;
#endif
    /********************** fat init END **********************/
    shell_main( g_input_mode, -1 );

    dsm_Printf( BASE_DSM_TS_ID, "closing emulated flash.\n\n" );

    return 0;
}
#endif


#ifdef _FS_SIMULATOR_
/*
在PC环境下执行一个file中的command序列
换言之,stdin此时关联的不是console而是该file

在文件中检查指定的section是否存在
读取该section中的第一个command(跳过空行和注释行)
第一个command应该是"init",调用build_argv()解析command
执行command对应的动作
调用FS_MountRoot
调用shell_main()依次处理section中的command直到遇到exit command
*/
static int shell_main_from_file( int section_id )
{
    BOOL bRet;

    g_input_mode = INPUT_MODE_FROM_FILE;

    bRet = get_section( stdin, section_id );
    if ( !bRet )
    {
        dsm_Printf( BASE_DSM_TS_ID, "init: get section_id[%d] failed!\n", section_id );

        return -1;
    }

    shell_main( g_input_mode, -1 );

    dsm_Printf( BASE_DSM_TS_ID, "closing flash.\n\n" );

    return 0;
}
#endif


#ifndef _FS_SIMULATOR_
/*
在开发板上从multilines中读取command序列

在multilines中检查指定的section是否存在
读取该section中的第一个command(跳过空行和注释行)
第一个command应该是"init",调用build_argv()解析command
执行command对应的动作
调用shell_main()依次处理section中的command直到遇到exit command
*/
static int shell_main_from_multilines( int index, int section_id )
{
    char *p;
    int argc;
    char **argv = NULL;
    BOOL bRet;

    g_input_mode = INPUT_MODE_FROM_MULTILINES;
    g_vds_shell_multilines_index = index;
    /*
    在搜索失败时,file_pointer将指向文件尾,因此,应该reset
    g_offset_in_string,为下次搜索做好准备.
    在多次调用本函数时也应该reset g_offset_in_string.
    */
    g_offset_in_string = 0;

    bRet = get_section_from_multilnes( g_vds_CommandLines[ index ], section_id );
    if ( !bRet )
    {
        dsm_Printf( BASE_DSM_TS_ID, "init: get section_id[%d] failed!\n", section_id );
        return -1;
    }

    while ( TRUE )
    {
        bRet = vds_GetLineFromStr( g_shell_line, sizeof( g_shell_line ), g_vds_CommandLines[ index ], &g_offset_in_string );
        p = bRet ? g_shell_line : NULL;
        if ( !p )
        {
            dsm_Printf( BASE_DSM_TS_ID, "init command not found!\n" );
            return -1;
        }

        if ( '\n' == g_shell_line[ 0 ] || ( g_shell_line[ 0 ] == '/' && g_shell_line[ 1 ] == '/' ) )
            continue;

        if ( 0 == DSM_StrNCmp( "init", g_shell_line, 4 ) )
        {
            dsm_Printf( BASE_DSM_TS_ID, "\n%s%s", g_dsm_Prompt, g_shell_line );
        }

        argc = 0;
        argv = build_argv( g_shell_line, &argc );
        if ( argv == NULL || argc == 0 )
        {
            continue;
        }

        if ( 0 == DSM_StrCaselessCmp( "init", argv[ 0 ] ) )
        {
            _ASSERT_( argc == 3 || argc == 5 );

            /*
            g_vds_DbgSwtich = 1;
            g_vds_DbgLevel = ( 0 == DSM_StrCaselessCmp( "on", argv[ 1 ] ) ) ? DL_DETAIL : DL_WARNING;
            */
            if ( 0 == DSM_StrCaselessCmp( "YES", argv[ 2 ] ) )
            {
                /* we not format flash */;
            }

            break; /* init后的command在shell_main()中处理 */
        }
    }

    shell_main( g_input_mode, index );

    dsm_Printf( BASE_DSM_TS_ID, "unmount the flash.\n\n" );

    return 0;
}
#endif


/*
在VC的console下以如下格式调用本函数:
    main(1, "FlashFS.exe"); 从console读取命令行
    main(2, "FlashFS.exe", "1"); 从脚本读取section[1]中的命令行

在模拟器或板子上，以如下格式调用本函数:
    main(3, "FlashFS.exe", "1", "2" );从数组的下标为1的字符串中读取section[2]中的命令行
 */
#ifdef _FS_SIMULATOR_
int main( int argc, char *argv[] )
#else
int dsm_main( int argc, char *argv[] )
#endif
{

    shell_main_from_console( );

}

#endif // VDS_SHELL_COMMAND



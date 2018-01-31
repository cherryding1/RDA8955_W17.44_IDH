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

#ifndef _DSM_SHELL_H_
#define _DSM_SHELL_H_

#define SHELL_BLK_REP_STATEMENT_TYPE_UNDEFINED    0
#define SHELL_BLK_REP_STATEMENT_TYPE_COMMON       1
#define SHELL_BLK_REP_STATEMENT_TYPE_BEGIN        2
#define SHELL_BLK_REP_STATEMENT_TYPE_END          3

#define VDS_SHELL_LINE_LEN_MAX    1024
#define VDS_SHELL_BUFFER_LEN_MAX  1024

#define VDS_SHELL_WRITE_BUFFER_LEN_MAX 1024

#define INPUT_MODE_FROM_CONSOLE            0
#define INPUT_MODE_FROM_FILE                   1
#define INPUT_MODE_FROM_MULTILINES       2


#define DOUBLE_QUOTE  '"'
#define SINGLE_QUOTE  '\''
#define BACK_SLASH    '\\'
#define IsSeparator( ch )    ( (ch) == ' ' || (ch) == '\t' || (ch) == '\n' )


#define DSM_SHELL_CMD_STACK_HEIGTH 128


#define PRINT_CMD_USAGE( ent ) dsm_Printf( BASE_DSM_TS_ID, "%s\nUsage: %s\n", ent->help, ent->usage )

struct cmd_entry
{
    char *name;
    BOOL ( *func ) ( struct cmd_entry *ent, int argc, char **argv );
    char *help;
    char *usage;
};


struct dsm_shell_block_repeat
{
    int type;
    int counter;
    int end_index;
    char cmd[ VDS_SHELL_LINE_LEN_MAX + LEN_FOR_NULL_CHAR ]; /*contains the arguments which are terminated by NULL_CHAR*/
    int argc;
    char **argv;
};


BOOL shell_repeat( struct cmd_entry *ent, int argc, char **argv );
BOOL shell_block_repeat( struct cmd_entry *ent, int argc, char **argv );
BOOL shell_help( struct cmd_entry *ent, int argc, char **argv );
BOOL shell_module_switch( struct cmd_entry *ent, int argc, char **argv );
BOOL shell_debug( struct cmd_entry *ent, int argc, char **argv );
BOOL shell_version( struct cmd_entry *ent, int argc, char **argv );

#endif

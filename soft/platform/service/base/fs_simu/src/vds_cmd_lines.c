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

#ifndef _VDS_CMD_LINES_H_
#define _VDS_CMD_LINES_H_

char *g_vds_CommandLines[] =
{
    "[SECTION 0]\n" \
    "init OFF NO\n" \
    "bigfile bf fixed 1024\n" \
    "bigfile bf fixed 1024\n" \
    "open bf\n" \
    "seek 1 0 end\n" \
    "st file detail 1 4\n" \
    "mcheck\n" \
    "exit\n" \
    "[END_OF_SECTION]\n",
};

int g_nr_command_lines = sizeof( g_vds_CommandLines ) / sizeof( *g_vds_CommandLines );
#endif



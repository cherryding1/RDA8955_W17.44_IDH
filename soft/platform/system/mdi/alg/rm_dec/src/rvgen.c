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













#ifdef SHEEN_VC_DEBUG
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "memory.h"
#endif
#include "rvtypes.h"
#include "rvdebug.h"
#include "rm_memory_default.h"

U32          g_number_of_loads = 0;
/* This counter allows Load() and Unload() to perform */
/* initialization only once, and cleanup only after all */
/* loaded instances have terminated. */

#if defined(DEBUG)

FILE_HDL g_debug_fp = NULL;

RV_Trace_Level g_debug_trace_level = RV_TL_ALWAYS;

char* g_debug_module_name = NULL;



void
RV_Debug_Message(U32 level, char *fmt, ...)
{
    va_list va;

    /*  g_debug_fp = stdout; */
    /*  g_debug_module_name = "rnconsole"; */

    if (level > g_debug_trace_level
            || !g_debug_fp
            || !fmt)
        return;

    va_start(va, fmt);


    fprintf(g_debug_fp, "%s : ",
            g_debug_module_name
            ? g_debug_module_name : " ");

    vfprintf(g_debug_fp, fmt, va);

    (void) fprintf(g_debug_fp, "\n");
    (void) fflush(g_debug_fp);

    va_end(va);
}

/* RV_Assert_Failed */
/* */
/* Print out a message indicating that the assertion failed, and give the */
/* user the option of aborting. */

void
RV_Assert_Failed
(
    char   *file_name,
    I32     line,
    char*   expr
)
{
    if (!expr)
        expr = "0";

    if (!file_name)
        file_name = "- unknown -";

    RVDebug((RV_TL_ALWAYS, "Assertion failed:  %s, file %s, line %ld.",
             expr, file_name, line));

    (void) fprintf(stderr,"Assertion failed:  %s, file %s, line %ld\n",
                   expr, file_name, line);
    (void) fflush(stderr);
#if defined (ARM) || defined(_ARM_) || defined(_WINCE)
    exit(3);
#else
    abort();
#endif
}


#endif  /* DEBUG */

void *
RV_Allocate(U32 size, Bool32 zero_init)
{
    void *p;

    if (size == 0)
    {
        RVDebug((RV_TL_ALWAYS,
                 "WARNING: RV_Allocate() called with zero size"));
        size++;  /* Allocate at least one byte */
    }

    //p = malloc(size);
    p = rm_memory_default_malloc(0, size);

    //printf("RV_Allocate %d\n", size);

    if (p && zero_init)
        (void) memset(p, 0, size);

    RVDebug((RV_TL_SUBPICT,
             "RV_Allocate(%6ld)  returning  0x%0.8lx",size,p));

    return p;
}

void
RV_Free(void *p)
{
    RVDebug((RV_TL_SUBPICT, "RV_Free(0x%0.8lx)",p));

    if (p)
    {
        //free(p);
        rm_memory_default_free(0, p);
        //printf("RV_Free \n");
    }
    else
        RVDebug((RV_TL_ALWAYS, "ERROR  RV_Free(NULL)"));
}





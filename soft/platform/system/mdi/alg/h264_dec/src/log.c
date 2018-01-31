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





#include "avutil.h"
#ifdef SHEEN_VC_DEBUG
#include <stdio.h>
#endif

int av_log_level = AV_LOG_DEBUG;

void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)
{
#ifdef SHEEN_VC_DEBUG
    static int print_prefix=1;
    AVClass* avc= ptr ? *(AVClass**)ptr : NULL;
    if(level>av_log_level)
        return;

//#undef fprintf
    if(print_prefix && avc)
    {
        fprintf(stderr, "[%s @ %p]", avc->item_name(ptr), avc);
    }
//#define fprintf please_use_av_log

    print_prefix= strstr(fmt, "\n") != NULL;

    vfprintf(stderr, fmt, vl);
#endif
}

static void (*av_log_callback)(void*, int, const char*, va_list) = av_log_default_callback;

void av_log(void* avcl, int level, const char *fmt, ...)
{
#ifdef SHEEN_VC_DEBUG
    va_list vl;
    va_start(vl, fmt);
    av_vlog(avcl, level, fmt, vl);
    va_end(vl);
#endif
}

void av_vlog(void* avcl, int level, const char *fmt, va_list vl)
{
    av_log_callback(avcl, level, fmt, vl);
}

int av_log_get_level(void)
{
    return av_log_level;
}

void av_log_set_level(int level)
{
    av_log_level = level;
}

void av_log_set_callback(void (*callback)(void*, int, const char*, va_list))
{
    av_log_callback = callback;
}

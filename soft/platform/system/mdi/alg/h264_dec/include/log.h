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



#ifndef FFMPEG_LOG_H
#define FFMPEG_LOG_H

#include <stdarg.h>

/**
 * Describes the class of an AVClass context structure, that is an
 * arbitrary struct of which the first field is a pointer to an
 * AVClass struct (e.g. AVCodecContext, AVFormatContext etc.).
 */
typedef struct AVCLASS AVClass;
struct AVCLASS
{
    /**
     * The name of the class; usually it is the same name as the
     * context structure type to which the AVClass is associated.
     */
    const char* class_name;

    /**
     * a pointer to a function which returns the name of a context
     * instance \p ctx associated with the class
     */
    const char* (*item_name)(void* ctx);

    /**
     * a pointer to the first option specified in the class if any or NULL
     *
     * @see av_set_default_options()
     */
    const struct AVOption *option;
};

/* av_log API */

#if LIBAVUTIL_VERSION_INT < (50<<16)
#define AV_LOG_QUIET -1
#define AV_LOG_FATAL 0
#define AV_LOG_ERROR 0
#define AV_LOG_WARNING 1
#define AV_LOG_INFO 1
#define AV_LOG_VERBOSE 1
#define AV_LOG_DEBUG 2
#else
#define AV_LOG_QUIET    -8

/**
 * something went really wrong and we will crash now
 */
#define AV_LOG_PANIC     0

/**
 * something went wrong and recovery is not possible
 * like no header in a format which depends on it or a combination
 * of parameters which are not allowed
 */
#define AV_LOG_FATAL     8

/**
 * something went wrong and cannot losslessly be recovered
 * but not all future data is affected
 */
#define AV_LOG_ERROR    16

/**
 * something somehow does not look correct / something which may or may not
 * lead to some problems like use of -vstrict -2
 */
#define AV_LOG_WARNING  24

#define AV_LOG_INFO     32
#define AV_LOG_VERBOSE  40

/**
 * stuff which is only useful for libav* developers
 */
#define AV_LOG_DEBUG    48
#endif

#if LIBAVUTIL_VERSION_INT < (50<<16)
extern int av_log_level;
#endif

/**
 * Send the specified message to the log if the level is less than or equal to
 * the current av_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different av_vlog callback
 * function.
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 * pointer to an AVClass struct.
 * @param level The importance level of the message, lower values signifying
 * higher importance.
 * @param fmt The format string (printf-compatible) that specifies how
 * subsequent arguments are converted to output.
 * @see av_vlog
 */
#ifdef __GNUC__
void av_log(void*, int level, const char *fmt, ...) __attribute__ ((__format__ (__printf__, 3, 4)));
#else
void av_log(void*, int level, const char *fmt, ...);
#endif

void av_vlog(void*, int level, const char *fmt, va_list);
int av_log_get_level(void);
void av_log_set_level(int);
void av_log_set_callback(void (*)(void*, int, const char*, va_list));
void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl);

#endif /* FFMPEG_LOG_H */

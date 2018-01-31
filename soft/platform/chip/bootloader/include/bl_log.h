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

#ifndef _BL_LOG_H_
#define _BL_LOG_H_

#ifndef LOCAL_LOG_LEVEL
#define LOCAL_LOG_LEVEL LOG_LEVEL_INFO
#endif

enum
{
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
};

#define LOGE_EN (LOCAL_LOG_LEVEL >= LOG_LEVEL_ERROR)
#define LOGW_EN (LOCAL_LOG_LEVEL >= LOG_LEVEL_WARN)
#define LOGI_EN (LOCAL_LOG_LEVEL >= LOG_LEVEL_INFO)
#define LOGD_EN (LOCAL_LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define LOGV_EN (LOCAL_LOG_LEVEL >= LOG_LEVEL_VERBOSE)

void bl_log(const char *fmt, ...);
#define _BL_LOG(level, fmt, ...)      \
    do                                \
    {                                 \
        if (LOCAL_LOG_LEVEL >= level) \
            bl_log(fmt, ##__VA_ARGS__); \
    } while (0)

#define BL_LOGE(fmt, ...) _BL_LOG(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define BL_LOGW(fmt, ...) _BL_LOG(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define BL_LOGI(fmt, ...) _BL_LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define BL_LOGD(fmt, ...) _BL_LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define BL_LOGV(fmt, ...) _BL_LOG(LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)

#endif

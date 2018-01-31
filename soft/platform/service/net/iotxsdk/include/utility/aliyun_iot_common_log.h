/*********************************************************************************
 * 文件名称: aliyun_iot_common_log.h
 * 作       者:
 * 版       本:
 * 日       期: 2016-05-30
 * 描       述:
 * 其       它:
 * 历       史:
 **********************************************************************************/
#ifndef ALIYUN_IOT_COMMON_LOG_H
#define ALIYUN_IOT_COMMON_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include "platform/aliyun_iot_platform_datatype.h"


typedef enum IOT_LOG_LEVEL
{
    ALIOT_LOG_LEVEL_DEBUG = 0,
    ALIOT_LOG_LEVEL_INFO,
    ALIOT_LOG_LEVEL_WARN,
    ALIOT_LOG_LEVEL_ERROR,
    ALIOT_LOG_LEVEL_FATAL,
    ALIOT_LOG_LEVEL_NONE,
}aliot_log_level_t;

extern aliot_log_level_t g_iotLogLevel;

void aliyun_iot_common_log_set_level(aliot_log_level_t iotLogLevel);


#define ALIOT_LOG_DEBUG(format, ...) \
{\
    if(g_iotLogLevel <= ALIOT_LOG_LEVEL_DEBUG)\
    {\
        sys_arch_printf("[debug] %s()| "format"\n", __FUNCTION__, ##__VA_ARGS__);\
    }\
}

#define ALIOT_LOG_INFO(format, ...) \
{\
    if(g_iotLogLevel <= ALIOT_LOG_LEVEL_INFO)\
    {\
        sys_arch_printf("[info] %s()| "format"\n", __FUNCTION__, ##__VA_ARGS__);\
    }\
}

#define ALIOT_LOG_WARN(format, ...) \
{\
    if(g_iotLogLevel <= ALIOT_LOG_LEVEL_WARN)\
    {\
        sys_arch_printf("[warn]%s()| "format"\n", __FUNCTION__, ##__VA_ARGS__);\
    }\
}

#define ALIOT_LOG_ERROR(format,...) \
{\
    if(g_iotLogLevel <= ALIOT_LOG_LEVEL_ERROR)\
    {\
        sys_arch_printf("[error]%s()| "format"\n", __FUNCTION__, ##__VA_ARGS__);\
    }\
}

#define ALIOT_LOG_FATAL(format, ...) \
{\
    if(g_iotLogLevel <= ALIOT_LOG_LEVEL_FATAL)\
    {\
        sys_arch_printf("[notice]%s()| "format"\n", __FUNCTION__, ##__VA_ARGS__);\
    }\
}

#endif

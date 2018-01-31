#ifndef _CG_HTTP_TYPES_H_
#define _CG_HTTP_TYPES_H_

#include "cs_types.h"

#define CgInt64 INT64

#define time_t  long

#define UINT_MAX UINTMAX_MAX

typedef void (*packet_handler_t)(const unsigned char *data, size_t len);
typedef void (*memory_handler_t)(void *http_info);
typedef void (*status_handler_t)(long data_finish,long data_remain);
typedef void (*data_handler_t)(char *content,long content_length);
#endif

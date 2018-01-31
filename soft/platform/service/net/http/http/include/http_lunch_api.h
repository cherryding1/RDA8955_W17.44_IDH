// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2012, Coolsand Technologies, Inc.
// All Rights Reserved
//
// FILENAME: http_client_test.h
//
// REVISION HISTORY:
// NAME              DATE                REMAKS
// suminchen      2017-05-27       Created initial version 1.0
//
// //////////////////////////////////////////////////////////////////////////////
#ifndef __HTTP_LUNCH_API_H__
#define __HTTP_LUNCH_API_H__

#include "chttp.h"
#include "http_api.h"
//#include "http_types.h"

extern int finish_flag;
extern HANDLE http_handle;


BOOL lunch_http_Authorization(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_put(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_get(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_downLoad(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_post(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_head(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_options(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_trace(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
BOOL lunch_http_delete(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m);
void start_http_task(void);



typedef enum http_test_type{
        EV_HTTP_CONNECT=101,
        EV_HTTP_AUTHOR,
        EV_HTTP_GET,
        EV_HTTP_POST,
        EV_HTTP_PUT,
        EV_HTTP_HEAD,
        EV_HTTP_TRACE,
        EV_HTTP_DELETE,
        EV_HTTP_OPTIONS,
        EV_HTTP_DOWNLOAD,
        EV_HTTP_GETN,
        EV_HTTP_HEADN,
        EV_HTTP_DELETEN,
        EV_HTTP_PARA,
        EV_HTTP_POSTN,
        EV_HTTP_READ,
        EV_HTTP_DATA,
}HTTP_TEST_TYPE;




#endif

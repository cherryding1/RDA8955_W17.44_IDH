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
#ifndef __HTTP_TEST_API_H__
#define __HTTP_TEST_API_H__


#include "chttp.h"

void TEST_HTTP_Authorization(void);
void TEST_HTTP(void);


void TEST_HTTP_Get(void);
void TEST_HTTP_DownLoad(void);
void TEST_HTTP_Head(void);
void TEST_HTTP_Post(void);
void TEST_HTTP_Options(void);
void TEST_HTTP_Put(void);
void TEST_HTTP_Trace(void);
void TEST_HTTP_Delete(void);
void http_response_print(CgHttpPacket *httpPkt);


#endif

// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2012, Coolsand Technologies, Inc.
// All Rights Reserved
//
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmission of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: at_cmd_gprs.h
//
// DESCRIPTION:
// TODO: ...
//
// REVISION HISTORY:
// NAME              DATE                REMAKS
// Lixp      2012-2-20       Created initial version 1.0
//
// //////////////////////////////////////////////////////////////////////////////
#ifdef CFW_GPRS_SUPPORT
#ifndef __AT_CMD_HTTP_H__
#define __AT_CMD_HTTP_H__
#include "at_common.h"
//#include "http_lunch_api.h"

#ifdef AT_HTTP_SUPPORT
VOID AT_CmdFunc_HTTPAUTHOR(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPGET(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPHEAD(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPPOST(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPOPTIONS(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPPUT(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPTRACE(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPDELETE(AT_CMD_PARA * pParam);
VOID AT_CmdFunc_HTTPDOWNLOAD(AT_CMD_PARA * pParam);
void process_HTTP_html(const unsigned char *data,size_t data_len);
void process_HTTPDLD_data(const unsigned char *data, size_t len);
VOID AT_CmdFunc_HTTPPARA(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPACTION(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPSTATUS(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPREAD(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPDATA(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPINIT(AT_CMD_PARA *pParam);
VOID AT_CmdFunc_HTTPTERM(AT_CMD_PARA *pParam);





//void memory_Free_Http(Http_info *http_info);
VOID AT_CmdFunc_HTTPSSETCRT(AT_CMD_PARA *pParam);

#endif

#endif
#endif

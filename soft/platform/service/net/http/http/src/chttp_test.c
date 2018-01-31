/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_test.cpp
*
*	Revision:
*
*	17/02/15
*		- first revision
*
******************************************************************/
#include "cinterface.h"
#include "curl.h"
#include "clog.h"
#include "chttp.h"
#include "cstring.h"

//CgHttpRequest *g_httpReq = NULL;
//CgSocket *g_sock = NULL;
BOOL cg_http_request_post_test(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);

BOOL api_http_get(UINT8* url_char)
{

	CgNetURI *url;
	CgHttpRequest *g_httpReq = NULL;
    
	url = cg_net_uri_new();
	cg_net_uri_set(url, url_char);
	cg_log_info("url_char: %s \n",url_char);
	char *host;
	int port;
	char *request;
	BOOL ret = FALSE;
	CgHttpResponse *httpRes;
	char *content;
	CgInt64 contentLen;
	BOOL is_http = cg_net_url_ishttpprotocol(url);
	BOOL is_https = cg_net_url_ishttpsprotocol(url);
	if ((is_http == FALSE) && (is_https == FALSE)) {
		cg_log_error("not http/https protocol : %s\n",url_char);
		return FALSE;
	}
		
	host = cg_net_url_gethost(url);
	port = cg_net_url_getport(url);
	cg_log_info("cg_net_url_gethost %s\n",host);
	cg_log_info("cg_net_url_getport %d\n",port);
	if (port <= 0)
	{
		if(is_http)
			port = CG_HTTP_DEFAULT_PORT;
		else if(is_https)
			port = CG_HTTPS_DEFAULT_PORT;
	}
	cg_log_info("cg_net_url_getport %d\n",port);
	request = cg_net_url_getrequest(url);
	cg_log_info("cg_net_url_getrequest %s\n",request);
	g_httpReq = cg_http_request_new();
	cg_http_request_setmethod(g_httpReq, CG_HTTP_GET);
	cg_http_request_seturi(g_httpReq, request);
	cg_http_request_setcontentlength(g_httpReq, 0);
	ret = cg_http_request_post_test(g_httpReq, host, port, is_https);
	cg_log_info("cg_http_request_post done\n");

	return ret;
}

BOOL cg_http_request_post_test(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
{
	
	char *method, *uri, *version;
	CgString *firstLine;
	CgSocket *g_sock = NULL;
	cg_log_debug_l4("Entering...\n");

	cg_http_response_clear(httpReq->httpRes);

	cg_log_info("(HTTP) Posting:\n");
	cg_http_request_print(httpReq);

#if defined(CG_USE_OPENSSL)
	if (isSecure == FALSE)
		g_sock = cg_socket_stream_new();
	else
		g_sock = cg_socket_ssl_new();
#else
	g_sock = cg_socket_stream_new();
#endif

	if (cg_socket_connect(g_sock, ipaddr, port) == FALSE) {
		cg_socket_delete(g_sock);
		return FALSE;
	}
	
	cg_socket_settimeout(g_sock, cg_http_request_gettimeout(httpReq));
	cg_http_request_sethost(httpReq, ipaddr, port);
	cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(httpReq));

	method = cg_http_request_getmethod(httpReq);
	uri = cg_http_request_geturi(httpReq);
	version = cg_http_request_getversion(httpReq);

	if (method == NULL || uri == NULL || version == NULL) {
		//cg_socket_close(sock);
		cg_socket_delete(g_sock);
		g_sock = NULL;
		return FALSE;
	}	
	
	/**** send first line ****/
	firstLine = cg_string_new();
	cg_string_addvalue(firstLine, method);
	cg_string_addvalue(firstLine, CG_HTTP_SP);
	cg_string_addvalue(firstLine, uri);
	cg_string_addvalue(firstLine, CG_HTTP_SP);
	cg_string_addvalue(firstLine, version);
	cg_string_addvalue(firstLine, CG_HTTP_CRLF);
	cg_socket_write(g_sock, cg_string_getvalue(firstLine), cg_string_length(firstLine));
	cg_string_delete(firstLine);
	
	/**** send header and content ****/
	cg_http_packet_post((CgHttpPacket *)httpReq, g_sock);
	
	cg_http_response_read(httpReq->httpRes, g_sock, cg_http_request_isheadrequest(httpReq));
	cg_http_response_print(httpReq->httpRes);
	cg_socket_close(g_sock);
	cg_socket_delete(g_sock);
	g_sock = NULL;
	cg_http_request_delete(httpReq);

	cg_log_debug_l4("Leaving...\n");
	cg_log_info("cg_http_request_post_test: Done\n");

	return TRUE;
}

/*
void start_test_http()
{
	cg_log_info("Entering...start_test_http\n");

	api_http_get("http://123.57.221.42/");
}

void start_test_https()
{
	cg_log_info("Entering...start_test_https\n");
	api_http_get("https://123.57.221.42/");
}*/

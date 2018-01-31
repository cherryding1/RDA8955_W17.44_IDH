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
#include "http_api.h"
#include "http.h"
#include "http_download.h"

memory_handler_t memory_free_handler_d = NULL;

void set_memory_free_handler_d(memory_handler_t handler){

    memory_free_handler_d = handler;

}

BOOL Http_downLoad(Http_info *http_info1){
    
    CgHttpApi *cg_http_api;
    char *url_Char;

    url_Char = http_info1->url;
    cg_http_api = http_info1->cg_http_api;
    
       if(Http_init(cg_http_api,url_Char)!=FALSE)
            {

                        BOOL down_flag = FALSE;

                        cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

                        cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

                        cg_http_request_setcontentlength(cg_http_api->g_httpReq, 0);   

                        down_flag = cg_http_api_downLoad(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

                        memory_free_handler_d(http_info1);

                        cg_http_handler_delete(TRUE);

                        return down_flag;
    
            }else{
                cg_log_info("cg_http_init error ...\n");

                memory_free_handler_d(http_info1);

                cg_http_handler_delete(TRUE);
    
                return FALSE;
            }
 

}
BOOL cg_http_api_downLoad(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
{

        long contentLen;
        char* content;
        char contentput[500]= "HELLO DATE";
        char real_len[16];
        int send_to_pc;

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
            if(isSecure == TRUE){
                Http_WriteUart("failure, pelase check your network or certificate!\n",51);
            }else{
                Http_WriteUart("failure, pelase check your network!\n",36);
            }
            return FALSE;
        }
    
        cg_socket_settimeout(g_sock, cg_http_request_gettimeout(httpReq));
        cg_http_request_sethost(httpReq, ipaddr, port);
    /* add headers here */
        cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(httpReq));
    /* prepare to send firstline */
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

        cg_socket_close(g_sock);
        cg_socket_delete(g_sock);
        g_sock = NULL;
        cg_http_request_delete(httpReq);
        cg_log_debug_l4("Leaving...\n");
        cg_log_info("cg_http_request_post_test: Done\n");
    return TRUE;
}






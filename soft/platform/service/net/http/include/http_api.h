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
*	File: chttp.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
*	10/31/05
*		- Changed CHttpHeader* pointers to CHttpServer* pointers
*		  in CHttpServer struct.
*	
*	16-Jan-06 Aapo Makela
*		- Fixed header comparisons to be case-insensitive
*		- Fixed iskeepaliveconnection to check "Connection:close"
*		- Added M-POST specific stuff
*	04/03/06 Theo Beisch
*		- added (CgHttpHeaderList*) cast (see below)
*		- added LONGLONG macro for platform independence
*		- some re-sorting of get/set macros (see below)
******************************************************************/

#ifndef _HTTP_API_H_
#define _HTTP_API_H_

#include "http_types.h"
#include "http_if.h"
#include "cstring.h"
#include "clist.h"
#include "cthread.h"
#include "ctime.h"
#include "cmutex.h"
#include "csocket.h"
#include "curl.h"
#include "chttp.h"
#include "lwip/sockets.h"

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

/****************************************
Define nHttp_info
****************************************/

#define CID_1 1
#define USER_AGENT_1 2
#define PROIP_1 3
#define PROPORT_1 4
#define REDIR_1 5
#define BREAK_1 6
#define BREAKEND_1 7
#define TIMEOUT_1 8
#define CONTENT_TYPE_1 9
#define URL_1 10
#define USERDATA_1 11

/****************************************
Define nHttp_info
****************************************/

#define HTTP_SEND_MAX 65534

#define CG_HTTP_READLINE_BUFSIZE 512
#define CG_HTTP_SEVERNAME_MAXLEN 64
#define CG_HTTP_DATE_MAXLEN 128

#define CG_HTTP_REQUESTLINE_DELIM "\r\n "
#define CG_HTTP_STATUSLINE_DELIM "\r\n "
#define CG_HTTP_HEADERLINE_DELIM "\r\n :"

#define CG_HTTP_CR "\r"
#define CG_HTTP_LF "\n"
#define CG_HTTP_CRLF "\r\n"
#define CG_HTTP_SP " "
#define CG_HTTP_COLON ":"

#define CG_HTTP_VER10 "HTTP/1.0"
#define CG_HTTP_VER11 "HTTP/1.1"

#if !defined(CG_HTTP_CURL)
#define CG_HTTP_USERAGENT_DEFAULT "CyberGarage-HTTP/1.0"
#else
#define CG_HTTP_USERAGENT_DEFAULT "libcurl-agent/1.0 (Cyberlink for C)"
#endif

#define CG_HTTP_DEFAULT_PORT 80
#define CG_HTTPS_DEFAULT_PORT 443

#define CG_HTTP_STATUS_CONTINUE 100
#define CG_HTTP_STATUS_OK 200
#define CG_HTTP_STATUS_PARTIAL_CONTENT 206
#define CG_HTTP_STATUS_BAD_REQUEST 400
#define CG_HTTP_STATUS_NOT_FOUND 404
#define CG_HTTP_STATUS_METHOD_NOT_ALLOWED 405
#define CG_HTTP_STATUS_PRECONDITION_FAILED 412
#define CG_HTTP_STATUS_INVALID_RANGE 416
#define CG_HTTP_STATUS_INTERNAL_SERVER_ERROR 500

#define CG_HTTP_POST "POST"
#define CG_HTTP_GET "GET"
#define CG_HTTP_HEAD "HEAD"
#define CG_HTTP_PUT "PUT"
#define CG_HTTP_OPTIONS "OPTIONS"
#define CG_HTTP_TRACE "TRACE"
#define CG_HTTP_DELETE "DELETE"

#define CG_HTTP_HOST "HOST"
#define CG_HTTP_DATE "Date"
#define CG_HTTP_CACHE_CONTROL "Cache-Control"
	#define CG_HTTP_NO_CACHE "no-cache"
	#define CG_HTTP_MAX_AGE "max-age"
#define CG_HTTP_CONNECTION "Connection"
	#define CG_HTTP_CLOSE "close"
	#define CG_HTTP_KEEP_ALIVE "Keep-Alive"
#define CG_HTTP_CONTENT_TYPE "Content-Type"
#define CG_HTTP_CONTENT_LENGTH "Content-Length"
#define CG_HTTP_CONTENT_RANGE "Content-Range"
	#define CG_HTTP_CONTENT_RANGE_BYTES "bytes" 
#define CG_HTTP_LOCATION "Location"
#define CG_HTTP_SERVER "Server"
#define CG_HTTP_RANGE "Range"
#define CG_HTTP_TRANSFER_ENCODING "Transfer-Encoding"
	#define CG_HTTP_CHUNKED "Chunked"
#define CG_HTTP_USERAGENT "User-Agent"
#define CG_HTTP_AUTHOR "Authorization"
/**** SOAP Extention ****/
#define CG_HTTP_SOAP_ACTION "SOAPACTION"
#define CG_HTTP_SOAP_ACTION_WITH_NS "01-SOAPACTION"
#define CG_HTTP_SOAP_MAN_VALUE "\"http://schemas.xmlsoap.org/soap/envelope/\"; ns=01"

/**** UPnP Extention ****/
#define CG_HTTP_MPOST "M-POST"
#define CG_HTTP_MSEARCH "M-SEARCH"
#define CG_HTTP_NOTIFY "NOTIFY"
#define CG_HTTP_SUBSCRIBE "SUBSCRIBE"
#define CG_HTTP_UNSUBSCRIBE "UNSUBSCRIBE"	

#define CG_HTTP_ST "ST"
#define CG_HTTP_MX "MX"
#define CG_HTTP_MAN "MAN"
#define CG_HTTP_NT "NT"
#define CG_HTTP_NTS "NTS"
#define CG_HTTP_USN "USN"
#define CG_HTTP_EXT "EXT"
#define CG_HTTP_SID "SID"
#define CG_HTTP_SEQ "SEQ"
#define CG_HTTP_CALLBACK "CALLBACK"
#define CG_HTTP_TIMEOUT "TIMEOUT"

/**** CURL connection timeout: 2 seconds ****/
#define CG_HTTP_CURL_CONNECTTIMEOUT 2

/**** CURL total timeout: 30 seconds (UPnP DA: whole transfer: 30sec ****/
#define CG_HTTP_CONN_TIMEOUT 30

/* HTTP server - client thread blocking timeout */
#define CG_HTTP_SERVER_READ_TIMEOUT 120

/**** HTTP Status code reason phrases ****/
#define CG_HTTP_REASON_100 "Continue"
#define CG_HTTP_REASON_101 "Switching Protocols"
#define CG_HTTP_REASON_200 "OK"
#define CG_HTTP_REASON_201 "Created"
#define CG_HTTP_REASON_202 "Accepted"
#define CG_HTTP_REASON_203 "Non-Authoritative Information"
#define CG_HTTP_REASON_204 "No Content"
#define CG_HTTP_REASON_205 "Reset Content"
#define CG_HTTP_REASON_206 "Partial Content"
#define CG_HTTP_REASON_300 "Multiple Choices"
#define CG_HTTP_REASON_301 "Moved Permanently"
#define CG_HTTP_REASON_302 "Found"
#define CG_HTTP_REASON_303 "See Other"
#define CG_HTTP_REASON_304 "Not Modified"
#define CG_HTTP_REASON_305 "Use Proxy"
#define CG_HTTP_REASON_307 "Temporary Redirect"
#define CG_HTTP_REASON_400 "Bad Request"
#define CG_HTTP_REASON_401 "Unauthorized"
#define CG_HTTP_REASON_402 "Payment Required"
#define CG_HTTP_REASON_403 "Forbidden"
#define CG_HTTP_REASON_404 "Not Found"
#define CG_HTTP_REASON_405 "Method Not Allowed"
#define CG_HTTP_REASON_406 "Not Acceptable"
#define CG_HTTP_REASON_407 "Proxy Authentication Required"
#define CG_HTTP_REASON_408 "Request Time-out"
#define CG_HTTP_REASON_409 "Conflict"
#define CG_HTTP_REASON_410 "Gone"
#define CG_HTTP_REASON_411 "Length Required"
#define CG_HTTP_REASON_412 "Precondition Failed"
#define CG_HTTP_REASON_413 "Request Entity Too Large"
#define CG_HTTP_REASON_414 "Request-URI Too Large"
#define CG_HTTP_REASON_415 "Unsupported Media Type"
#define CG_HTTP_REASON_416 "Requested range not satisfiable"
#define CG_HTTP_REASON_417 "Expectation Failed"
#define CG_HTTP_REASON_500 "Internal Server Error"
#define CG_HTTP_REASON_501 "Not Implemented"
#define CG_HTTP_REASON_502 "Bad Gateway"
#define CG_HTTP_REASON_503 "Service Unavailable"
#define CG_HTTP_REASON_504 "Gateway Time-out"
#define CG_HTTP_REASON_505 "HTTP Version not supported"

/****************************************
* Data Type
****************************************/
typedef struct _CgHttpApi {
	CgNetURI *url;
	CgHttpRequest *g_httpReq;
	char *host;
	int port;
	char *uri_path;
	char *content_type ;
	char *content_body;
	CgHttpResponse *response;
	int contentLen;
	BOOL is_http;
	BOOL is_https;
	BOOL ret;
} CgHttpApi;

typedef struct _http_inf{
        char *username;
        char *password;
        char *content_type;
        char *body_content;
        char *content_name;
        UINT8 *url;
        CgHttpApi *cg_http_api;
 }Http_info;

typedef struct _nhttp_inf{
        int CID;
        char *url;
        char *USER_AGENT;
        char *pro_host;
        int pro_port;
        int REDIR;
        int BREAK;
        int BREAKEND;
        int TIMEOUT;
        char *CONTENT_TYPE;
        void *user_data;
        long data_length;
        CgHttpApi *cg_http_api;
        struct ifreq *device;
        int method_code;
        int function_status;
        int status_code;
        long content_length;
        long data_finish;
        long data_remain; 
        BOOL user_data_used;
}nHttp_info;

/****************************************
* Function (Data Post)
****************************************/

BOOL cg_http_api_post(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);

/****************************************
* Function (Http api)
****************************************/
BOOL Http_init(CgHttpApi *cg_http_api,UINT8* url_char);

CgHttpResponse *Http_get(Http_info *http_info1);

CgHttpResponse *Http_head(Http_info *http_info1);

CgHttpResponse *Http_post(Http_info *http_info1);
CgHttpResponse *Http_put(Http_info *http_info1);
CgHttpResponse *Http_options(Http_info *http_info1);

CgHttpResponse *Http_trace(Http_info *http_info1);

CgHttpResponse *Http_delete(Http_info *http_info1);
BOOL cg_http_api_post(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);
char* Http_post_reg(UINT8* url_char, char *content_type,char* body_content);

char* Http_iccid_reg(UINT8* url_char, char *content_type,char *body_content,char *regver, char *meid, char *modelsms,
    char *swver, char *SIM1Iccid, char *SIM1LteImsi,BOOL issecret);

char* Json_Base64(char *regver, char *meid, char *modelsms,
    char *swver, char *SIM1Iccid, char *SIM1LteImsi);
char* cg_http_api_post_reg(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);
CgHttpResponse *Http_authorization(Http_info *http_info1);

char* Normal_Base64(char *input_buffer);
long cg_http_api_post_contentLen(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);
CgHttpResponse *cg_http_api_response(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);
#if defined(CG_USE_OPENSSL)
BOOL Https_saveCrttoFile(char *pemData,UINT32 pemLen,UINT32 pemtype);
BOOL Https_setCrt(UINT32 pemtype,UINT8** crtPem);
#endif

nHttp_info *Init_Http(void);
void Term_Http(nHttp_info *nHttp_inf);
int Http_strcmp(char *tag);
BOOL Http_para(nHttp_info *nHttp_inf,char *tag,char *value);
BOOL Http_getn(nHttp_info *http_info1);
BOOL Http_headn(nHttp_info *http_info1);
BOOL Http_postn(nHttp_info *http_info1);
BOOL Http_deleten(nHttp_info *http_info1);
CgHttpResponse * cg_nhttp_api_response(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure,nHttp_info *http_info);

VOID http_setUserdata(nHttp_info *at_nHttp_info,UINT8*userData, long dataLen);
void Http_read(nHttp_info *nHttp_info,long offset,long length);





#ifdef  __cplusplus
}
#endif

#endif

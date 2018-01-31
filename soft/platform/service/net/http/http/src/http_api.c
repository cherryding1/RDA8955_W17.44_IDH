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

//CgHttpRequest *g_httpReq = NULL;
//CgSocket *g_sock = NULL;
memory_handler_t memory_free_handler = NULL;


void set_memory_free_handler (memory_handler_t handler){

    memory_free_handler = handler;

}

BOOL cg_http_api_post(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure);

BOOL Http_init(CgHttpApi *cg_http_api,UINT8* url_char)
{
    //cg_http_api->url = cg_net_uri_new();
    cg_net_uri_set(cg_http_api->url, url_char);
    cg_log_info("url_char: %s \n",url_char);

    cg_http_api->is_http = cg_net_url_ishttpprotocol(cg_http_api->url);
    cg_http_api->is_https = cg_net_url_ishttpsprotocol(cg_http_api->url);
    if ((cg_http_api->is_http == FALSE) && (cg_http_api->is_https == FALSE)) {
        cg_log_error("not http/https protocol : %s\n",url_char);
        //cg_net_uri_clear(cg_http_api->url);
        //cg_net_uri_delete(cg_http_api->url);
        return FALSE;
    }

    cg_http_api->host = cg_net_url_gethost(cg_http_api->url);
    cg_http_api->port = cg_net_url_getport(cg_http_api->url);
    cg_log_info("cg_net_url_gethost %s\n",cg_http_api->host);
    cg_log_info("cg_net_url_getport %d\n",cg_http_api->port);
    if (cg_http_api->port <= 0)
    {
        if(cg_http_api->is_http)
        cg_http_api->port = CG_HTTP_DEFAULT_PORT;
        else if(cg_http_api->is_https)
        cg_http_api->port = CG_HTTPS_DEFAULT_PORT;
    }
    cg_log_info("cg_net_url_getport %d\n",cg_http_api->port);
    cg_http_api->uri_path= cg_net_url_getrequest(cg_http_api->url);
    cg_log_info("cg_net_url_getrequest %s\n",cg_http_api->uri_path);
    //cg_http_api->g_httpReq = cg_http_request_new();

    return TRUE;
}
nHttp_info *Init_Http(void)
{
    nHttp_info *InitHttp;
    
    CgHttpApi *cg_http_api;

    UINT8 *USERDATA;

    InitHttp = (nHttp_info *)malloc(sizeof(nHttp_info));
    
    cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
    
    cg_http_api->url = cg_net_uri_new();
                       
    cg_http_api->g_httpReq = cg_http_request_new();
    
    InitHttp->url = (char *)malloc(sizeof(char)*256);            
    InitHttp->CID = 0;
    InitHttp->USER_AGENT = "RDA_8955";
    InitHttp->pro_host = NULL;
    InitHttp->pro_port = 0;
    InitHttp->REDIR =0;
    InitHttp->BREAK = 0;
    InitHttp->BREAKEND = 0;
    InitHttp->CONTENT_TYPE = NULL;
    InitHttp->TIMEOUT = 0;
    InitHttp->cg_http_api = cg_http_api;
    InitHttp->user_data_used = FALSE;
    InitHttp->user_data = (UINT8 *)malloc(319488);

    return InitHttp;
}
void Term_Http(nHttp_info *nHttp_inf){

    nHttp_inf->user_data_used = FALSE;

    cg_log_info("nHttp_inf->user_data_used: Done\n");
    
    cg_net_uri_delete((nHttp_inf->cg_http_api)->url);

    cg_log_info("uri_delete: Done\n");
        
    cg_http_request_delete((nHttp_inf->cg_http_api)->g_httpReq);

    cg_log_info("request_delete: Done\n");

    free(nHttp_inf->url);
    
    cg_log_info("free(nHttp_inf->url): Done\n");

    free(nHttp_inf->cg_http_api);

    cg_log_info("free(nHttp_inf->cg_http_api): Done\n");

    free((nHttp_inf->user_data));

    cg_log_info("free(nHttp_inf->user_data): Done\n");
    
    free(nHttp_inf);

    cg_log_info("free(nHttp_inf): Done\n");

}
int Http_strcmp(char *tag){
    if (strcmp(tag,"CID")==0){
        return CID_1;
    }else if(strcmp(tag,"UA")==0){
        return USER_AGENT_1;
    }else if(strcmp(tag,"PROIP")==0){
        return PROIP_1;
    }else if(strcmp(tag,"PROPORT")==0){
        return PROPORT_1;
    }else if(strcmp(tag,"REDIR")==0){
        return REDIR_1;
    }else if(strcmp(tag,"BREAK")==0){
        return BREAK_1;
    }else if(strcmp(tag,"BREAKEND")==0){
        return BREAKEND_1;
    }else if(strcmp(tag,"TIMEOUT")==0){
        return TIMEOUT_1;
    }else if(strcmp(tag,"CONTENT")==0){
        return CONTENT_TYPE_1;
    }else if(strcmp(tag,"USERDATA")==0){
        return USERDATA_1;
    }else if(strcmp(tag,"URL")==0){
        return URL_1;
    }else {
        return 14;
    }

}
BOOL Http_para(nHttp_info *nHttp_inf,char *tag,char *value){
    BOOL para = FALSE;
    nHttp_inf->user_data_used = TRUE;
    switch(Http_strcmp(tag)){
            case CID_1:
                nHttp_inf->CID = atoi(value);
                para = TRUE;
                break;
            case URL_1:
                strcpy(nHttp_inf->url,value);
                para = TRUE;
                break;
            case USER_AGENT_1: 
                nHttp_inf->USER_AGENT= value;
                para = TRUE;
                break;
            case PROIP_1:
                nHttp_inf->pro_host= value;
                para = TRUE;
                break;
            case PROPORT_1: 
                nHttp_inf->pro_port= atoi(value);
                para = TRUE;
                break;
            case REDIR_1:
                nHttp_inf->REDIR= atoi(value);
                para = TRUE;
                break;
            case BREAK_1:
                nHttp_inf->BREAK= atoi(value);
                para = TRUE;
                break;
            case BREAKEND_1:
                nHttp_inf->BREAKEND= atoi(value);
                para = TRUE;
                break;
            case TIMEOUT_1:
                nHttp_inf->TIMEOUT= atoi(value);
                para = TRUE;
                break;
            case CONTENT_TYPE_1:
                nHttp_inf->CONTENT_TYPE= value;
                para = TRUE;
                break;
            case USERDATA_1:
                nHttp_inf->user_data= value;
                para = TRUE;
                break;
            default:
                para = FALSE;
                break;
    }

    return para;

}

void Http_read(nHttp_info *nHttp_info,long offset,long length){

       UINT8 isEnough = 0;
       UINT8 OutStr[20];
       long send = 512;

       isEnough = (nHttp_info->content_length) - offset - length;

       cg_log_info("isEnough: %d\n",isEnough);

       cg_log_info("offset: %d\n",offset);

       cg_log_info("length: %d\n",length);

        if(isEnough >= 0){

            AT_Sprintf(OutStr, "+HTTPREAD: %d\r\n",length);
            Http_WriteUart(OutStr, AT_StrLen(OutStr));

            while(length > 0){
                               
                if(length > send){

                    length -= send;
                    Http_WriteUart(((nHttp_info->user_data) + offset), send); 
                    offset += send;
                    
                }else{
                    Http_WriteUart(((nHttp_info->user_data) + offset), length);                                               
                    length = 0;
                    cg_log_info("length: %d\n",length);
                    
                }   
                
            }


        }else{

            length = (nHttp_info->content_length) - offset;

            AT_Sprintf(OutStr, "+HTTPREAD: %d\r\n",length);
            Http_WriteUart(OutStr, AT_StrLen(OutStr));

            while(length > 0){

                cg_log_info("nHttp_info->user_data: %s\n",(nHttp_info->user_data)+offset);
                               
                if(length > send){

                    length -= send;
                    Http_WriteUart(((nHttp_info->user_data) + offset), send);
                    offset+=send;
                }else{

                    Http_WriteUart(((nHttp_info->user_data) + offset), length);
                    length = 0;
                    cg_log_info("length: %d\n",length);
                    
                }   
                
            }
            


        }



}

BOOL Http_getn(nHttp_info *http_info1)
{


        CgHttpApi *cg_http_api;

        char *url_char;

        char contentRange[32];

        char begin_downLoad[16];

        char end_downLoad[16];

        UINT8 tmpString[30] = {0x00,};
        
        cg_http_api = http_info1->cg_http_api;

        url_char = http_info1->url;
        
         if(Http_init(cg_http_api,url_char)!=FALSE)
        {
            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

            if(http_info1->pro_host!=NULL){

                    cg_http_api->host = http_info1->pro_host;
                    cg_http_api->port = http_info1->pro_port;
            }

            if((http_info1->BREAK) !=0){
            
                itoa(http_info1->BREAK,begin_downLoad,10);

                itoa(http_info1->BREAKEND,end_downLoad,10);

                strcpy(contentRange, "bytes=");
                
                strcat(contentRange,begin_downLoad);

                strcat(contentRange,"-");

                if((http_info1->BREAKEND)!= 0){

                strcat(contentRange,end_downLoad);

                }

                cg_http_packet_setheadervalue((CgHttpPacket*)cg_http_api->g_httpReq, CG_HTTP_RANGE, contentRange);


            }

            cg_http_packet_setheadervalue((CgHttpPacket*)cg_http_api->g_httpReq, CG_HTTP_USERAGENT, http_info1->USER_AGENT);

            cg_http_packet_setheadervalue((CgHttpPacket*)cg_http_api->g_httpReq, CG_HTTP_CONTENT_TYPE, http_info1->CONTENT_TYPE);

            (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;

            cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https,http_info1);

            cg_log_info("cg_http_request_post done\n");

            AT_Sprintf(tmpString, "%d %d %d",0,http_info1->status_code,http_info1->content_length);

            Http_WriteUart(tmpString, AT_StrLen(tmpString));

            cg_http_handler_delete(TRUE);

            return TRUE;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            return FALSE;
        }
 
}
BOOL Http_headn(nHttp_info *http_info1)
{
            cg_log_info("Enter http_headn\n");

            CgHttpApi *cg_http_api;
            
            char *url_char;
            
            cg_http_api = http_info1->cg_http_api;
            
            url_char = http_info1->url;
            
            UINT8 tmpString[30] = {0x00,};


            if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, 0);
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            cg_log_info("cg_http_request_post done\n");

            AT_Sprintf(tmpString, "%d %d %d",2,http_info1->status_code,0);

            Http_WriteUart(tmpString, AT_StrLen(tmpString));

            cg_http_handler_delete(TRUE);

            return TRUE;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            return FALSE;
        }

}


BOOL Http_postn(nHttp_info *http_info1)
{

        cg_log_info("Enter http_postn\n");

        CgHttpApi *cg_http_api;
        char *url_char;
        char *body_content;
        char *content_type;

        UINT8 tmpString[30] = {0x00,};

        url_char = http_info1->url;
        body_content = http_info1->user_data;
        content_type = http_info1->CONTENT_TYPE;
        cg_http_api = http_info1->cg_http_api;
    

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            cg_http_api->contentLen = cg_strlen(body_content);

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_POST);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            cg_http_request_setcontenttype(cg_http_api->g_httpReq,content_type);

            cg_http_request_setcontent(cg_http_api->g_httpReq,body_content);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

            if(http_info1->pro_host!=NULL){

                    cg_http_api->host = http_info1->pro_host;
                    cg_http_api->port = http_info1->pro_port;
            }

            cg_http_packet_setheadervalue(cg_http_api->g_httpReq, CG_HTTP_USERAGENT, http_info1->USER_AGENT);

            (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;
            
            cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https,http_info1);
            
            cg_log_info("cg_http_request_post done\n");

            AT_Sprintf(tmpString, "%d %d %d",1,http_info1->status_code,0);

            Http_WriteUart(tmpString, AT_StrLen(tmpString));

            cg_http_handler_delete(TRUE);

            return TRUE;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            return FALSE;
        }
}
BOOL Http_deleten(nHttp_info *http_info1)
{

            cg_log_info("Enter http_deleten\n");

            CgHttpApi *cg_http_api;
            char *url_char; 
            UINT8 tmpString[30] = {0x00,};

            url_char = http_info1->url;
            cg_http_api = http_info1->cg_http_api;

            if(Http_init(cg_http_api,url_char)!=FALSE)
            {

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_DELETE);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            if(http_info1->pro_host!=NULL){

                        cg_http_api->host = http_info1->pro_host;
                        cg_http_api->port = http_info1->pro_port;
            }

            cg_http_packet_setheadervalue(cg_http_api->g_httpReq, CG_HTTP_USERAGENT, http_info1->USER_AGENT);

            (cg_http_api->g_httpReq)->timeout = http_info1->TIMEOUT;
                
            cg_nhttp_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https,http_info1);
                
            cg_log_info("cg_http_request_post done\n");

            AT_Sprintf(tmpString, "%d %d %d",3,http_info1->status_code,0);

            Http_WriteUart(tmpString, AT_StrLen(tmpString));

            cg_http_handler_delete(TRUE);

            return TRUE;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            return FALSE;
        }

}



CgHttpResponse * cg_nhttp_api_response(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure,nHttp_info *http_info)
{

    char *method, *uri, *version;
    CgString *firstLine;
    CgSocket *g_sock = NULL;
    CgHttpResponse *response = NULL;
    int statuscode = 0;
    int iResult;
    
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

    iResult = CFW_TcpipSocketSetsockopt(g_sock->id, SOL_SOCKET, SO_BINDTODEVICE, http_info->device, sizeof(struct ifreq));

    if (cg_socket_connect(g_sock, ipaddr, port) == FALSE) {
    cg_socket_delete(g_sock);
    return NULL;
    }

    cg_socket_settimeout(g_sock, cg_http_request_gettimeout(httpReq));
    cg_http_request_sethost(httpReq, ipaddr, port);
/* add headers here */
    
/* prepare to send firstline */
    method = cg_http_request_getmethod(httpReq);
    uri = cg_http_request_geturi(httpReq);
    version = cg_http_request_getversion(httpReq);

    if (method == NULL || uri == NULL || version == NULL) {
//cg_socket_close(sock);
    cg_socket_delete(g_sock);
    g_sock = NULL;
    return NULL;
    }
Redir:
    
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

    cg_log_info("RESPONSE READ BEGIN++:\n");

    cg_http_response_read(httpReq->httpRes, g_sock, cg_http_request_isheadrequest(httpReq));

    cg_log_info("RESPONSE READ END++:\n");

    statuscode = cg_http_response_getstatuscode(httpReq->httpRes);

    http_info->status_code = statuscode;

    http_info->content_length = cg_http_response_getcontentlength(httpReq->httpRes);

    cg_log_info("http_info->content_length: %d\n",http_info->content_length);

    
    if(http_info->REDIR == 1){

        if(statuscode>=300 && statuscode <400){

            uri = cg_http_headerlist_getvalue((httpReq->httpRes)->headerList,CG_HTTP_LOCATION);
            memset(http_info->user_data,0,http_info->content_length);
            goto Redir;

        }
    }
    //cg_http_response_print(httpReq->httpRes);
    //response = httpReq->httpRes;
    cg_socket_close(g_sock);
    cg_socket_delete(g_sock);
    g_sock = NULL;
    //cg_http_request_delete(httpReq);

    cg_log_debug_l4("Leaving...\n");
    cg_log_info("cg_http_request_post_test: Done\n");

    return response;
}

//set user data for post option
VOID http_setUserdata(nHttp_info *at_nHttp_info,UINT8*userData, long dataLen)
{
    cg_log_info("enter http_setUserdata.dataLen:%d\n",dataLen);
    cg_log_info("enter http_setUserdata.userData:%s\n",userData);
    if(dataLen<0 || dataLen>319488){
        cg_log_info("dataLen error!! \n");
        return;
    }
    at_nHttp_info->user_data = (UINT8 *)malloc(dataLen);
    at_nHttp_info->data_length = dataLen;
    if(NULL == at_nHttp_info->user_data)
    {
       cg_log_info("malloc error!! \n");
       return ;
    }
    strncpy(at_nHttp_info->user_data,userData,at_nHttp_info->data_length); 
    cg_log_info("enter http_setUserdata.at_nHttp_info->user_data:%s\n",at_nHttp_info->user_data);
}

CgHttpResponse *Http_authorization(Http_info *http_info1)
{



    char user_data[64] = {0};

    CgHttpApi *cg_http_api;

    char *username;
        
    char *password;

    char *url_char;
    
    cg_http_api = http_info1->cg_http_api;

    username = http_info1->username;

    password = http_info1->password;

    url_char = http_info1->url;

    strcat(user_data,username);

    strcat(user_data,":");

    strcat(user_data,password);

    char user_pwd [100] ="Basic ";

    strcat(user_pwd,Normal_Base64(user_data));

    cg_log_info("b64+++ %s",user_pwd);

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {
            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

            cg_http_packet_setheadervalue((CgHttpPacket*)cg_http_api->g_httpReq,CG_HTTP_AUTHOR,user_pwd);

            CgHttpResponse *ret;
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

           /* CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;
            }*/
            cg_log_info("cg_http_request_post done\n");

            //cg_net_uri_delete(cg_http_api->url);

            //cg_http_request_delete(cg_http_api->g_httpReq);

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            memory_free_handler(http_info1);;

            return NULL;
        }
    
}

CgHttpResponse *Http_head(Http_info *http_info1)
{
    CgHttpApi *cg_http_api;
    char *url_char;
    cg_http_api = http_info1->cg_http_api;
    url_char = http_info1->url;


            if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);

            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, 0);

            CgHttpResponse *ret;
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            /*CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            memory_free_handler(http_info1);

            return NULL;
        }

}


CgHttpResponse *Http_post(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;
    char *body_content;
    char *content_type;

    url_char = http_info1->url;
    body_content = http_info1->body_content;
    content_type = http_info1->content_type;
    cg_http_api = http_info1->cg_http_api;
    

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            cg_http_api->contentLen = cg_strlen(body_content);

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_POST);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            cg_http_request_setcontenttype(cg_http_api->g_httpReq,content_type);

            cg_http_request_setcontent(cg_http_api->g_httpReq,body_content);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

            CgHttpResponse *ret;
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            /*CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            cg_http_handler_delete(TRUE);

            memory_free_handler(http_info1);

            return NULL;
        }
}
CgHttpResponse *Http_put(Http_info *http_info1)
{
        CgHttpApi *cg_http_api;
        char *content_name;
        char *body_content;
        char *content_type;
        char *url_char;
        int statuscode = 0;

        cg_http_api = http_info1->cg_http_api;
        content_name = http_info1->content_name;
        content_type = http_info1->content_type;
        body_content = http_info1->body_content;
        url_char = http_info1->url;

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            CgHttpResponse *ret;

            cg_http_handler_clean(TRUE);

            cg_http_api->uri_path = strcat(cg_http_api->uri_path,content_name);

            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);
            
            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);
            
            ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            if(ret == NULL){

                goto PUTEND;

            }

            statuscode = ret->statusCode;
            
            cg_log_info("statuscode++++ %d\n",statuscode);
            
             if(statuscode == 200 ){
            
                   //cg_http_handler_clean(FALSE);
            
                   Http_WriteUart("failure,file already exist,please delete first\n",47);
            
                   goto PUTEND;
                   
             }else if(statuscode == 404 ){
            
                   //cg_http_handler_clean(FALSE);
            
             }

            cg_http_api->contentLen = cg_strlen(body_content);

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_PUT);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            cg_http_request_setcontenttype(cg_http_api->g_httpReq,content_type);

            cg_http_request_setcontent(cg_http_api->g_httpReq,body_content);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);
            
            ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            /*CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/

            statuscode = ret->statusCode;
            
            cg_log_info("statuscode++++ %d\n",statuscode);
            
             if(statuscode == 201 ){
            
                   Http_WriteUart("success,file created\n",21);
            
                   
             }else {
            
                 Http_WriteUart("failure,file created fail\n",26); 
            
             }
            

            PUTEND:
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            return NULL;
        }


}
CgHttpResponse *Http_options(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;

    cg_http_api = http_info1->cg_http_api;
    url_char = http_info1->url;


        if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_OPTIONS);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            CgHttpResponse *ret;
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            /*CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            return NULL;
        }

}

CgHttpResponse *Http_trace(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;

    cg_http_api = http_info1->cg_http_api;
    url_char = http_info1->url;

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_TRACE);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            CgHttpResponse *ret;
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            /*CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            return NULL;
        }
    
}
CgHttpResponse *Http_delete(Http_info *http_info1)
{

    CgHttpApi *cg_http_api;
    char *url_char;
    char *content_name;
    int statuscode = 0;

    url_char = http_info1->url;
    content_name = http_info1->content_name;
    cg_http_api = http_info1->cg_http_api;

    

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {


            CgHttpResponse *ret;

            cg_http_handler_clean(TRUE);

            cg_http_api->uri_path = strcat(cg_http_api->uri_path,content_name);

            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);
            
            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);
            
            ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            if(ret == NULL){

                goto DELETEEND;

            }
            statuscode = ret->statusCode;
            
            cg_log_info("statuscode++++ %d\n",statuscode);
            
             if(statuscode == 404 ){
            
                   Http_WriteUart("failure,file not exist,please create first\n",43);
            
                   goto DELETEEND;
                   
             }

            cg_log_info("cg_net_url_getrequest === %s\n",cg_http_api->uri_path);

            cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_DELETE);

            cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_HEAD);
                        
            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);
                        
            ret = cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            statuscode = ret->statusCode;
            
            cg_log_info("statuscode++++ %d\n",statuscode);
            
             if(statuscode == 404 ){
            
                   Http_WriteUart("success,file deleted\n",21);
            
                   
             }else {
            
                   Http_WriteUart("failure,file deleted fail\n",26);
                 
            
             }

       /* CgHttpResponse *res_copy = cg_http_response_new();

            if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/

            DELETEEND:
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            return NULL;
        }

}
BOOL cg_http_api_post(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
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
    


    cg_http_response_print(httpReq->httpRes);
    cg_socket_close(g_sock);
    cg_socket_delete(g_sock);
    g_sock = NULL;
    cg_http_request_delete(httpReq);

    cg_log_debug_l4("Leaving...\n");
    cg_log_info("cg_http_request_post_test: Done\n");

    return TRUE;
}

char* Normal_Base64(char *input_buffer){

    unsigned char *buffer;

    int buffer_len;

    int y;

    size_t len;

    size_t input_len;

    input_len= strlen(input_buffer);

    y = input_len % 3;

    if (y!=0)

    {
         buffer_len = ((input_len)+(3-y)) * 4;
    }else{
         buffer_len = (input_len) * 4;
    }

    buffer_len = buffer_len / 3;

    buffer = (unsigned int *)malloc(buffer_len+1);  

    if(mbedtls_base64_encode( buffer, (buffer_len+1), &len, input_buffer, input_len)!=0)
    {

    cg_log_info("base64 error...");
    return NULL;
    
    }else{
        
    return buffer;
        
    } 
}

char* Json_Base64(char *regver, char *meid, char *modelsms,
    char *swver, char *SIM1Iccid, char *SIM1LteImsi){

    unsigned char *buffer;

    int buffer_len;

    int y;

    size_t len;

    size_t out_len;

    char *out = httpgenerateJsonString(regver,meid,modelsms,swver,SIM1Iccid,SIM1LteImsi);

    if(out==NULL)
    {
        cg_log_info("JsonString error...");
        return NULL;
    }

    out_len= strlen(out);

    y = out_len % 3;

    if (y!=0)

    {
         buffer_len = ((out_len)+(3-y)) * 4;
    }else{
         buffer_len = (out_len) * 4;
    }

    buffer_len = buffer_len / 3;

    buffer = (unsigned int *)malloc(buffer_len+1);  

    if(mbedtls_base64_encode( buffer, (buffer_len+1), &len, out, out_len)!=0)
    {

    cg_log_info("base64 error...");
    return NULL;
    
    }else{
        
    return buffer;
        
    } 
}

char* Http_iccid_reg(UINT8* url_char, char *content_type,char *body_content,char *regver, char *meid, char *modelsms,
    char *swver, char *SIM1Iccid, char *SIM1LteImsi,BOOL issecret){

char *post_content;

char *reg_result;

if(issecret == FALSE){

    post_content = Json_Base64(regver,meid,modelsms,swver,SIM1Iccid,SIM1LteImsi);

    if(post_content==NULL){
        
    cg_log_info("post_content error...");
    
    return FALSE;
    }

    if((reg_result=Http_post_reg(url_char,content_type,post_content))!=NULL){

    cg_log_info("reg_result+++ %s",reg_result);
    
    return reg_result;
        
    }else{

    return NULL;

    }
    
    }else{
    post_content=body_content;
    if((reg_result=Http_post_reg(url_char,content_type,post_content))!=NULL){

    cg_log_info("reg_result %s",reg_result);
    
    return reg_result;
        
    }else{

    return NULL;

    }   
    }
}
char* Http_post_reg(UINT8* url_char, char *content_type,char* body_content)
{

    CgHttpApi *cg_http_api;

    BOOL ret;

    char *content;

    cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));

    if (cg_http_api== NULL)

    {
    cg_log_info("MALLOC ERROR \n"); 
    }
    else
    {

        if(Http_init(cg_http_api,url_char)!=FALSE)
        {

        cg_http_api->contentLen = cg_strlen(body_content);

        cg_http_request_setmethod(cg_http_api->g_httpReq,CG_HTTP_POST);

        cg_http_request_seturi(cg_http_api->g_httpReq,cg_http_api->uri_path);

        cg_http_request_setcontenttype(cg_http_api->g_httpReq,content_type);

        cg_http_request_setcontent(cg_http_api->g_httpReq,body_content);

        cg_http_request_setcontentlength(cg_http_api->g_httpReq, cg_http_api->contentLen);

        content = cg_http_api_post_reg(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);
        
        cg_log_info("cg_http_request_post_test done\n");

        free(cg_http_api);
        
        cg_log_info("ram is freed, prepare to return... \n");

        return content;

        }else{
        cg_log_info("cg_http_get error ...\n");

        return NULL;
        }
    }
}
char* cg_http_api_post_reg(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
{

        long contentLen;
        char* content;
        char contentput[500]= "HELLO DATE";

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

        cg_http_response_print(httpReq->httpRes);

        content = cg_http_packet_getcontent((CgHttpPacket *) httpReq->httpRes);
        
        cg_socket_close(g_sock);
        cg_socket_delete(g_sock);
        g_sock = NULL;
        cg_http_request_delete(httpReq);
  
        
        cg_log_debug_l4("Leaving...\n");
        cg_log_info("cg_http_request_post_test: Done\n");
    return content;
}
long cg_http_api_post_contentLen(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
{

        long contentLen;
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
    /* add headers here */
        cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(httpReq));
    /* prepare to send firstline */
        method = cg_http_request_getmethod(httpReq);
        uri = cg_http_request_geturi(httpReq);
        version = cg_http_request_getversion(httpReq);
    
        if (method == NULL || uri == NULL || version == NULL) {
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
        
        contentLen = cg_http_packet_getcontentlength((CgHttpPacket *) httpReq->httpRes);
       

       // cg_http_response_print(httpReq->httpRes);
        cg_socket_close(g_sock);
        cg_socket_delete(g_sock);
        g_sock = NULL;
  //    cg_http_request_delete(httpReq);
    
        cg_log_debug_l4("Leaving...\n");
        cg_log_info("cg_http_request_post_test: Done\n");
    return contentLen;
}


CgHttpResponse *Http_get(Http_info *http_info1)
{


    CgHttpApi *cg_http_api;

    char *url_char;
    
    cg_http_api = http_info1->cg_http_api;

    url_char = http_info1->url;
    
         if(Http_init(cg_http_api,url_char)!=FALSE)
        {
            cg_http_request_setmethod(cg_http_api->g_httpReq, CG_HTTP_GET);

            cg_http_request_seturi(cg_http_api->g_httpReq, cg_http_api->uri_path);

            cg_http_request_setcontentlength(cg_http_api->g_httpReq, 0);
            
            CgHttpResponse *ret;
            
            cg_http_api_response(cg_http_api->g_httpReq, cg_http_api->host, cg_http_api->port, cg_http_api->is_https);

            //CgHttpResponse *res_copy = cg_http_response_new();

            /*if(ret!=NULL){
                
            cg_http_response_copy(res_copy, ret);

            }else{

                cg_http_response_clear(res_copy);

                cg_http_response_delete(res_copy);

                Http_WriteUart("failure, pelase check your network!\n",36);              

                return NULL;

            }*/
            cg_log_info("cg_http_request_post done\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            cg_log_info("ram is freed, prepare to return... \n");

            return ret;

        }else{
            cg_log_info("cg_http_get error ...\n");

            memory_free_handler(http_info1);

            cg_http_handler_delete(TRUE);

            return NULL;
        }
 
}
CgHttpResponse * cg_http_api_response(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
{

    char *method_temp;
    char *method, *uri, *version;
    CgString *firstLine;
    CgSocket *g_sock = NULL;
    CgHttpResponse *response = NULL;
    int statuscode = 0;
    
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
    return NULL;
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
    return NULL;
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

    if(strcmp(method,CG_HTTP_DELETE)!=0){

            cg_log_info("read response begin++++\n");
         
            cg_http_response_read(httpReq->httpRes, g_sock, cg_http_request_isheadrequest(httpReq));
        
            cg_log_info("read response end++++\n");


    }

    response = httpReq->httpRes;
    cg_socket_close(g_sock);
    cg_socket_delete(g_sock);
    g_sock = NULL;
    //cg_http_request_delete(httpReq);

    cg_log_debug_l4("Leaving...\n");
    cg_log_info("cg_http_request_post_test: Done\n");

    return response;
}

#if defined(CG_USE_OPENSSL)
#define AT_HTTPS_FILE_NAME_LEN  32
static UINT8 caCrt_filename[] = "caCrt.pem";
static UINT8 clientCrt_filename[] = "clientCrt.pem";;
static UINT8 clientPrivateKey_filename[] = "privateKey.pem";
UINT8* file_name;

// File name is httpsCrt.pem.
BOOL cg_getFileName(UINT8 set_id,UINT8* file_name)
{
    UINT8 buff[AT_HTTPS_FILE_NAME_LEN];
    UINT8* p;
    UINT32 i = 0;
    UINT8 number_str[4];

    if (set_id >= 2)
    {
        return FALSE;
    }
    SUL_MemSet8(buff,0,AT_HTTPS_FILE_NAME_LEN); 
    SUL_MemSet8(number_str,0,4);
    SUL_StrCopy(buff,"httpsCrt");
    SUL_StrCat(buff,"_");
    SUL_Itoa(set_id,number_str,10);
    SUL_StrCat(buff,number_str);
    SUL_StrCat(buff,".pem");
    p = (UINT8*)buff;
    while(*p)
    {
        file_name[i] = *p;
        file_name[i+1] = 0;
        i += 2;
        p ++;
    }
    file_name[i] = 0;
    file_name[i+1] = 0;
    return TRUE;
}


// Write certificate file.
BOOL cg_writeFile(UINT8* filename,UINT8* cfg_buff,UINT32 cfg_size)
{

    INT32 result;
    INT32 fd;
    UINT32 check_size = 0;

    fd = FS_Open(filename,FS_O_RDWR,0);
    sys_arch_printf("cg_writeFile ,fd:%d",fd);
    if(fd < 0)
    {
        return FALSE;
    }
    result = FS_Write(fd,cfg_buff,cfg_size);
    sys_arch_printf("cg_writeFile,result:%d",result);
    if(result != cfg_size)
    {
        FS_Close(fd);
        return FALSE;
    }
    else
    {
        FS_Close(fd);
        return TRUE;
    }
}

//save pemData to file.set_id : 0 or 1
BOOL cg_https_savetoFile(UINT8* file_name,char* pemData,UINT32 pemLen,UINT8 set_id)
{
    cg_log_info("enter  cg_https_savetoFile. pemLen:%d \n",pemLen);
    BOOL bResult;
    INT32 fd,ret;
    if(set_id >= 2)
    {
        return FALSE;
    }
    /*
    SUL_MemSet8(file_name,0,(AT_HTTPS_FILE_NAME_LEN*2));
    bResult = cg_getFileName(set_id,(UINT8*)file_name);
    if(!bResult)
    {
        return FALSE;
    }
    */
    
    ret = FS_Delete(file_name);
    if((ret != ERR_SUCCESS)&&(ret!=ERR_FS_NO_DIR_ENTRY))
    {   
        cg_log_info("FS_Delete error.ret : %d",ret);
        return FALSE;
    }
    fd = FS_Create(file_name,0);
    if(fd < 0)
    {
        cg_log_info("FS_Create error.");
        return FALSE;
    }
    bResult = cg_writeFile(file_name,pemData, pemLen);
    if(!bResult)
    {   
        cg_log_info("cg_writeFile error.");
        return FALSE;
    }
    else
    {   
        cg_log_info("success: save crt to %s file.",file_name);
        return TRUE;
    }
}

BOOL Https_saveCrttoFile(char* pemData,UINT32 pemLen,UINT32 pemtype)
{
    cg_log_info("enter  Https_saveCrttoFile. pemLen:%d,pemtype:%d \n",pemLen,pemtype);
    BOOL result = FALSE;
    switch(pemtype){
        case CG_HTTPS_CA_CRT ://set ca certificate filename
            return cg_https_savetoFile(caCrt_filename,pemData,pemLen,0);
        case CG_HTTPS_CLIENT_CRT://set client certificate filename
            return cg_https_savetoFile(clientCrt_filename,pemData,pemLen,0);
        case CG_HTTPS_CLIENT_PRIVATE_KEY://set client private key filename
            return cg_https_savetoFile(clientPrivateKey_filename,pemData,pemLen,0);
        default:
            cg_log_info("pemtype ERROR \n");
            return FALSE;
    }
}

BOOL cg_https_setCrt(UINT8* filename,UINT8** crtPem)
{
    cg_log_info("filename:%s. \n",filename);
    INT32 fd;
    UINT32 fileSize = 0;
    INT32 result;
    fd = FS_Open(filename,FS_O_RDWR,0);
    if(fd < 0)
    {
        if(ERR_FS_NO_DIR_ENTRY == fd){
            //Http_WriteUart("ERROR! not set certificate.\n",AT_StrLen("ERROR! not set certificate.\n"));
            //Http_WriteUart("\n",AT_StrLen("\n"));
        }
        cg_log_info("FS_Open error,fd:%d\n",fd);
        return FALSE;
    }
    fileSize = FS_GetFileSize(fd);
    cg_log_info("fd:%d,FS_GetFileSize:%d\n",fd,fileSize);
    if(fileSize < 0){
        cg_log_info("fileSize error\n");
        return FALSE;
    } 
    if(*crtPem != NULL)
    {
        cg_log_info("free crtPem!! \n");
        free(*crtPem);
        *crtPem = NULL;
    }
    *crtPem = (UINT8 *)malloc(fileSize);
    if(NULL == *crtPem)
    {
       cg_log_info("malloc error!! \n");
       return FALSE;
    }
     // Read  crt data from file to buffer
    result = FS_Read(fd,*crtPem,fileSize);
    if(result != fileSize)
    {
        cg_log_info("not read all data!! \n");
        FS_Close(fd);
        return FALSE;
    }
    FS_Close(fd);
    return TRUE; 
}
BOOL Https_setCrt(UINT32 pemtype,UINT8** crtPem)
{
    cg_log_info("+++++++enter  Https_setCrt. pemtype:%d \n",pemtype);
    UINT32 ret;
    switch(pemtype){
        case CG_HTTPS_CA_CRT ://set ca certificate
            return cg_https_setCrt(caCrt_filename,crtPem);
        case CG_HTTPS_CLIENT_CRT://set client certificate
            return cg_https_setCrt(clientCrt_filename,crtPem);
        case CG_HTTPS_CLIENT_PRIVATE_KEY://set client private key
            return cg_https_setCrt(clientPrivateKey_filename,crtPem);
        default:
           cg_log_info("pemtype ERROR \n");
           return FALSE;
    }
}

#endif

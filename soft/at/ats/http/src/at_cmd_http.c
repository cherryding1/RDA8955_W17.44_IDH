/*********************************************************
 *
 * File Name
 *        at_http_api.c
 * Author
 *         shaoboan
 * Date
 *         2017/03/18
 * Descriptions:
 *        Commands specific to MTs supporting the Packet Domain.
 *
 *********************************************************/

#ifdef AT_HTTP_SUPPORT

#include "stdio.h"
#include "cfw.h"
#include "at.h"
#include "at_sa.h"  // caoxh[+]2008-04-19
#include "at_module.h"
#include "at_cmd_http.h"
#include "at_cfg.h"
#include "itf_api.h"
#include "sockets.h"
#include "http_api.h"
#include "http_download.h"
#include "http_test_api.h"
#include "app_http.h"
#include "at_define.h"

#define AT_MAX_PPP_ID                   8
#define AT_TCPIPACTPHASE_GPRS           0


#define HTTP_URL_SIZE 255
UINT8 outString[1500] = {0x00,};
nHttp_info *at_nHttp_info;
long real_len = 0;
BOOL flag = FALSE;

AT_CMD_ENGINE_T *gHttpEngine;

void Http_WriteUart(const uint8_t *data, unsigned length)
{
    AT_AsyncWriteUart(gHttpEngine,data,length);
    AT_AsyncWriteUart(gHttpEngine,"\r\n", AT_StrLen("\r\n"));
}

void process_HTTP_html(const unsigned char *data,size_t data_len){
    if(strcmp(data,"EOF")!=0){
        Http_WriteUart(data, data_len);
    }
}
void memory_Free_Http(Http_info *http_info){

    cg_net_uri_clear((http_info->cg_http_api)->url);
    cg_net_uri_delete((http_info->cg_http_api)->url);
    cg_http_request_clear((http_info->cg_http_api)->g_httpReq);
    cg_http_request_delete((http_info->cg_http_api)->g_httpReq);

    free(http_info->cg_http_api);
    
    free(http_info);
    
}

void process_HTTP_status(long data_remain, long data_finish){

    at_nHttp_info->data_finish = data_finish;
    at_nHttp_info->data_remain = data_remain;

}

void process_read_data(UINT8* content,long content_length){

    AT_TC(g_sw_GPRS, "process_read_data content_length %d:",content_length);

    if(content_length>0){

       AT_TC(g_sw_GPRS, "copy begin %d:",content_length);
        
       //strncpy(((at_nHttp_info->user_data)+real_len),content,content_length);
       strcat((at_nHttp_info->user_data),content);
    }
    real_len += content_length;

}

VOID AT_CmdFunc_HTTPAUTHOR(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       const uint8_t username[HTTP_URL_SIZE] = {0x00,};
       const uint8_t password[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;

       if (NULL == pParam)
       {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if(pParam->paramCount != 3)
                       {
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult));
                       if (!iResult)
                       {
                            AT_TC(g_sw_GPRS, "HTTPGET AUTHOR URL error");
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(username,at_ParamStr(pParam->params[1], &iResult));
                       if (!iResult)
                       {
                            AT_TC(g_sw_GPRS, "HTTPGET AUTHOR username error");
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(password,at_ParamStr(pParam->params[2], &iResult));
                       if (!iResult)
                       {
                            AT_TC(g_sw_GPRS, "HTTPGET AUTHOR password error");
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }

                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{

                               http_info1->url = url;
                               http_info1->username = username;
                               http_info1->password = password;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPAuthorization(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

VOID AT_CmdFunc_HTTPGET(AT_CMD_PARA *pParam)
{
	
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;
       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount!= 1)
                       {
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult));
                       if (!iResult)
                       {
                           AT_TC(g_sw_GPRS, "HTTPGET get URL error");
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPGET get URL : %s",url);

                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{

                               http_info1->url = url;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPGet(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       break;
               case AT_CMD_TEST:
                    AT_StrCpy(tmpString, "+HTTPGET:<url>");
                    at_CmdRespInfoText(pParam->engine, tmpString);
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
               default:
                    AT_TC(g_sw_GPRS, "HTTPGET ERROR");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                    break;
       }
}

VOID AT_CmdFunc_HTTPPOST(AT_CMD_PARA *pParam)
{
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       bool  iResult = true;
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       const uint8_t content_type[HTTP_URL_SIZE] = {0x00,};
       const uint8_t body_content[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       UINT8 content_length = 0;
       CgHttpApi *cg_http_api;
       Http_info *http_info1;
       if (NULL == pParam)
       {
           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if(pParam->paramCount!= 3)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult));
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET POST URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(content_type,at_ParamStr(pParam->params[1], &iResult));
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET POST body_content error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(body_content,at_ParamStr(pParam->params[2], &iResult));
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET POST body_content error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPGET post URL : %s",url);

                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{

                               http_info1->url = url;
                               http_info1->content_type= content_type;
                               http_info1->body_content= body_content;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPPost(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                content_length = AT_StrLen(body_content);
                                AT_Sprintf(outString, "Content_Type: %s\r\n", content_type);                                
                                at_CmdRespInfoText(pParam->engine,outString);
                                
                                                           
                                AT_Sprintf(outString, "Content_Length: %d\r\n", content_length);                                
                                at_CmdRespInfoText(pParam->engine,outString);
                                
                                at_CmdRespInfoNText(pParam->engine,body_content,content_length);

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                                

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       }                   
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPPOST:<url>,<content_type>,<body_content>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
               default:
                       AT_TC(g_sw_GPRS, "HTTPPOST ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

VOID AT_CmdFunc_HTTPPUT(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       const uint8_t content_type[HTTP_URL_SIZE] = {0x00,};
       const uint8_t body_content[HTTP_URL_SIZE] = {0x00,};
       const uint8_t content_name[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;
       if (NULL == pParam)
       {
              AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 4)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult));
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET PUT URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(content_type,at_ParamStr(pParam->params[1], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET PUT content_type error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(content_name,at_ParamStr(pParam->params[2], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET PUT content_name error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(body_content,at_ParamStr(pParam->params[3], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET PUT body_content error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPPUT PUT body_content : %s",body_content);

                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{
                               http_info1->url = url;
                               http_info1->content_type= content_type;
                               http_info1->content_name = content_name;
                               http_info1->body_content = body_content;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPPut(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPPUT:<url>,<content_type>,<content_name>,<body_content>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPPUT ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

VOID AT_CmdFunc_HTTPHEAD(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;

       if (NULL == pParam)
       {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if(pParam->paramCount != 1)
                       {
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPHEAD HEAD URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPHEAD HEAD URL : %s",url);
                       
                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                             AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{

                               http_info1->url = url;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPHead(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPHEAD:<url>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPHEAD ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

VOID AT_CmdFunc_HTTPOPTIONS(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if(pParam->paramCount != 1)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPOPTIONS OPTIONS URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPOPTIONS OPTIONS URL : %s",url);
                       
                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{

                               http_info1->url = url;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPOptions(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPOPTIONS:<url>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPOPTIONS ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

VOID AT_CmdFunc_HTTPTRACE(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 1)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPTRACE TRACE URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPTRACE TRACE URL : %s",url);
                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{

                               http_info1->url = url;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPTrace(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPTRACE:<url>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPTRACE ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

VOID AT_CmdFunc_HTTPDELETE(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t url[HTTP_URL_SIZE] = {0x00,};
       const uint8_t content_name[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;
       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 2)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPDELETE delete URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPDELETE delete URL : %s",url);
                       strcpy(content_name,at_ParamStr(pParam->params[1], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPDELETE delete content_name error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPDELETE delete content_name : %s",content_name);
                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{
                          
                               http_info1->url = url;
                               http_info1->content_name = content_name;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPDelete(http_info1,process_HTTP_html,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                            }

                       } 
                       break;
               case AT_CMD_TEST:
                      AT_StrCpy(tmpString, "+HTTPDELETE:<url>,<content_name>");
                      at_CmdRespInfoText(pParam->engine, tmpString);
                      AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                      break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPDELETE ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

void process_HTTPDLD_data(const unsigned char *data, size_t len){
    UINT8 OutStr[20];
    AT_Sprintf(OutStr, "+HTTPDLD: %d\r\n",len);
    Http_WriteUart(OutStr, AT_StrLen(OutStr));
    Http_WriteUart(data, len);

}


VOID AT_CmdFunc_HTTPDOWNLOAD(AT_CMD_PARA *pParam)
{

       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       uint8_t url[HTTP_URL_SIZE] = {0x00,};
       UINT8 tmpString[60] = {0x00,};
       CgHttpApi *cg_http_api;
       Http_info *http_info1;
       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 1)
                       {
                              AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(url,at_ParamStr(pParam->params[0], &iResult));
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPDOWNLOAD download URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPDOWNLOAD download URL : %s",url);
                       http_info1 = (Http_info *)malloc(sizeof(Http_info));
                       
                       cg_http_api = (CgHttpApi *)malloc(sizeof(CgHttpApi));
                       
                       cg_http_api->url = cg_net_uri_new();
                       
                       cg_http_api->g_httpReq = cg_http_request_new();

                       if (cg_http_api== NULL||http_info1 == NULL)

                       {    
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }else{
                               http_info1->url = url;
                               http_info1->cg_http_api = cg_http_api;

                            if((app_WPHTTPDownLoad(http_info1,process_HTTPDLD_data,memory_Free_Http)==TRUE)){

                                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                            }else{
                                memory_Free_Http(http_info1);
                                at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                            }

                       } 
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPDOWNLOAD:<url>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;

               default:
                       AT_TC(g_sw_GPRS, "HTTPDOWNLOAD ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}

/**
 * @brief This function sets the ca.crt,client.crt and client_private_key to be used for https for secure requests.
 * @This func have one parameter: 0: ca.crt  1: client.crt  2: clientprivate.key
 * @return      No Return value
 */

static UINT32 crtFlag;

VOID AT_CmdFunc_HTTPSSETCRT(AT_CMD_PARA *pParam)
{
    bool iResult = true;
    UINT8 PromptMsg[8] = { 0 };
    UINT8 tmpString[60] = {0x00,};
    BOOL result;
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+SETCACRT: pParam is NULL");
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
    gHttpEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_SET:

            if (!pParam->iExDataLen)
            {
                if(pParam->pPara == NULL){
                    AT_TC(g_sw_GPRS, "AT+SETCACRT: pParam is NULL");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                if (pParam->paramCount == 0)
                {
                    AT_TC(g_sw_GPRS, "AT+SETCACRT: pPara number is error");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                // get crtFlag
                crtFlag = at_ParamUintInRange(pParam->params[0],0,2,&iResult);
                AT_TC(g_sw_GPRS, "crtFlag = %d ",crtFlag);
                if (!iResult)
                {
                    AT_TC(g_sw_GPRS, "get crtFlag error\n");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                PromptMsg[0] = AT_GC_CfgGetS3Value();
                PromptMsg[1] = AT_GC_CfgGetS4Value();
                AT_StrCat(PromptMsg, "> ");
                AT_TC(g_sw_GPRS, " auto sending timer:0 seconds");
                at_CmdRespInfoText(pParam->engine, (UINT8 *)PromptMsg);    
                AT_CMD_ENGINE_T *th = at_CmdGetByChannel(pParam->nDLCI);
                at_CmdSetPromptMode(th);
                return;
            }
            else
            {
                if (pParam->pExData[pParam->iExDataLen - 1] == 27)
                {
                    AT_TC(g_sw_GPRS, "end with esc, cancel send");
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                }
                else if (pParam->pExData[pParam->iExDataLen - 1] != 26)
                {
                    AT_TC(g_sw_GPRS, "not end with ctl+z, err happen");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_INVALID_LEN));
                }
                AT_TC(g_sw_GPRS, "pParam->iExDataLen: %d",pParam->iExDataLen);
                result = Https_saveCrttoFile(pParam->pExData,pParam->iExDataLen - 1,crtFlag);
                if (!result) {
                    AT_TC(g_sw_GPRS, "Https_saveCrttoFile error");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CMS_MEM_FAIL));
                }
               at_CmdRespInfoText(pParam->engine, tmpString);
               AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
               return;
            }
            break;
        case AT_CMD_TEST:
            AT_StrCpy(tmpString, "+SETCACRT=<crtFlag>,crtFlag:0,1,2");
            at_CmdRespInfoText(pParam->engine, tmpString);
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
            break;
        default:
            AT_TC(g_sw_GPRS, "AT_CmdFunc_HTTPSSETCRT ERROR");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
}




VOID AT_CmdFunc_HTTPPARA(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t tag[HTTP_URL_SIZE] = {0x00,};
       int tag_in;
       const uint8_t value[HTTP_URL_SIZE] = {0x00,}; 
       UINT8 tmpString[60] = {0x00,};

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 2)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(tag,at_ParamStr(pParam->params[0], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPPARA tag error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(value,at_ParamStr(pParam->params[1], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPPARA value error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       AT_TC(g_sw_GPRS, "HTTPPARA  value:%s",value);
                       if((lunch_http_para(at_nHttp_info,tag,value))==TRUE){
                        
                           AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                       }else{

                           at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}
VOID AT_CmdFunc_HTTPACTION(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       UINT8 METHOD;
       UINT8 tmpString[60] = {0x00,};
       struct netif *netif = NULL;

       struct ifreq device;

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 1)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       METHOD = at_ParamUintInRange(pParam->params[0],0,3,&iResult);
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPGET AUTHOR URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }

                       AT_TC(g_sw_GPRS, "HTTPACTION begin get netif :");

                       AT_TC(g_sw_GPRS, "HTTPACTION cid %d :",at_nHttp_info->CID);

                       AT_TC(g_sw_GPRS, "HTTPACTION SIM %d :",nSim);

                       netif = getGprsNetIf(nSim,at_nHttp_info->CID);

                       if(netif == NULL){
                           AT_TC(g_sw_GPRS, "HTTPACTION GET NETIF error");
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                                              
                       AT_Sprintf(tmpString, "%c%c%d:\r\n", netif->name[0],netif->name[1], netif->num);

                       AT_StrCpy(device.ifr_name, tmpString);

                       at_nHttp_info->device = &device;

                       AT_TC(g_sw_GPRS, "HTTPACTION begin get device :");

                       AT_TC(g_sw_GPRS, "HTTPACTION METHOD %d :",METHOD);

                       cg_http_set_status_handler(process_HTTP_status);

                       switch (METHOD){
                            case 0:
                                at_nHttp_info->method_code = 0;
                                cg_http_set_data_handler(process_read_data);
                                
                                AT_TC(g_sw_GPRS, "HTTPGET  URL %s",at_nHttp_info->url);

                                if((lunch_http_getn(at_nHttp_info))==TRUE){
                                                        
                                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
         
                                }else{
         
                                    at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                                }
 
                                break;
                                
                            case 1:
                                at_nHttp_info->method_code = 1;
                                if((lunch_http_postn(at_nHttp_info))==TRUE){
                                                        
                                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
         
                                }else{
         
                                    at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                                }
                                break;
                                
                            case 2:
                                at_nHttp_info->method_code = 2;
                                cg_http_set_html_handler(process_HTTP_html);
                                if((lunch_http_headn(at_nHttp_info))==TRUE){
                                                        
                                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
         
                                }else{
         
                                    at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                                }
                                break;
                                
                            case 3:
                                at_nHttp_info->method_code = 3;
                                if((lunch_http_deleten(at_nHttp_info))==TRUE){
                                                        
                                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
         
                                }else{
         
                                    at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                                }
                                break;
                                
                            default:
                                AT_TC(g_sw_GPRS, "HTTPACTION METHOD error, ONLY SUPPORT 0~3");
                                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                                
                                
                       }
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}
VOID AT_CmdFunc_HTTPSTATUS(AT_CMD_PARA *pParam)
{
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       char *method_name = NULL;  
       int status;
       UINT8 tmpString[60] = {0x00,};

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_EXE:
                       
                       if(at_nHttp_info->method_code == 0){

                            method_name = "GET";
                            status = 1;
                            AT_Sprintf(tmpString, "%s %d %d %d",method_name,status,at_nHttp_info->data_finish,at_nHttp_info->data_remain);
                            at_CmdRespInfoText(pParam->engine, tmpString);
                            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                            
                            
                       }else if(at_nHttp_info->method_code == 1){

                            method_name = "POST";
                            status = 2;
                            AT_Sprintf(tmpString, "%s %d %d %d",method_name,status,at_nHttp_info->data_finish,at_nHttp_info->data_remain);
                            at_CmdRespInfoText(pParam->engine, tmpString);
                            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                       }else if(at_nHttp_info->method_code == 2){

                            method_name = "HEAD";
                            status = 1;
                            AT_Sprintf(tmpString, "%s %d %d %d",method_name,status,at_nHttp_info->data_finish,at_nHttp_info->data_remain);
                            at_CmdRespInfoText(pParam->engine, tmpString);
                            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                       }else if(at_nHttp_info->method_code == 3){

                            method_name = "DELETE";
                            status = 1;
                            AT_Sprintf(tmpString, "%s %d %d %d",method_name,status,at_nHttp_info->data_finish,at_nHttp_info->data_remain);
                            at_CmdRespInfoText(pParam->engine, tmpString);
                            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                       }
                   
 
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}
VOID AT_CmdFunc_HTTPREAD(AT_CMD_PARA *pParam)
{
       bool iResult = true;
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       const uint8_t offset_c[HTTP_URL_SIZE] = {0x00,};
       const uint8_t length_c[HTTP_URL_SIZE] = {0x00,}; 
       long offset;
       long length;
       UINT8 tmpString[60] = {0x00,};
        

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_SET:
                       if (pParam->paramCount != 2)
                       {
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(offset_c,at_ParamStr(pParam->params[0], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPREAD OFFSET error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }
                       strcpy(length_c,at_ParamStr(pParam->params[1], &iResult)); 
                       if (!iResult)
                       {
                               AT_TC(g_sw_GPRS, "HTTPREAD LENGTH URL error");
                               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }

                       offset = atoi(offset_c);
                       length = atoi(length_c);

                       if(offset > at_nHttp_info->content_length){

                            AT_TC(g_sw_GPRS, "HTTPREAD  error, ADDR is too large!");
                            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

                       }
                       if((lunch_http_read(at_nHttp_info,offset,length))==TRUE){
                       
                           
                           AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                       }else{

                           at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }                       
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}
VOID AT_CmdFunc_HTTPDATA(AT_CMD_PARA *pParam)
{
    UINT8 PromptMsg[8] = { 0 };
    UINT8 tmpString[60] = {0x00,};
    if (NULL == pParam)
    {
        AT_TC(g_sw_GPRS, "AT+SETCACRT: pParam is NULL");
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
    }
    gHttpEngine = pParam->engine;
    switch (pParam->iType)
    {
        case AT_CMD_EXE:

            if (!pParam->iExDataLen)
            {
                if (pParam->pPara == NULL)
                {
                    AT_TC(g_sw_GPRS, "AT+HTTPDATA: pParam is NULL");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                if (pParam->paramCount != 0)
                {
                    AT_TC(g_sw_GPRS, "AT+HTTPDATA: pPara number is error");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                PromptMsg[0] = AT_GC_CfgGetS3Value();
                PromptMsg[1] = AT_GC_CfgGetS4Value();
                AT_StrCat(PromptMsg, "> ");
                AT_TC(g_sw_GPRS, " auto sending timer:0 seconds");
                at_CmdRespInfoText(pParam->engine, PromptMsg);

                AT_CMD_ENGINE_T *th = at_CmdGetByChannel(pParam->nDLCI);
                at_CmdSetPromptMode(th);

                return;
            }
            else
            {
                if (pParam->pExData[pParam->iExDataLen - 1] == 27)
                {
                    AT_TC(g_sw_GPRS, "end with esc, cancel send");
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                    return;
                }
                else if (pParam->pExData[pParam->iExDataLen - 1] != 26)
                {
                    AT_TC(g_sw_GPRS, "not end with ctl+z, err happen");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                AT_TC(g_sw_GPRS, "pParam->iExDataLen: %d",pParam->iExDataLen);
                


                if((lunch_http_data(at_nHttp_info,pParam->pExData, pParam->iExDataLen - 1))==TRUE){
                   
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));

                }else{

                    at_CmdRespInfoText(pParam->engine,"Task is already running, please wait\n");
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                } 
                return;
            }
            break;
        case AT_CMD_TEST:
            AT_StrCpy(tmpString, "+HTTPDATA");
            at_CmdRespInfoText(pParam->engine, tmpString);
            AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
            break;
        default:
            AT_TC(g_sw_GPRS, "AT_CmdFunc_HTTPDATA ERROR");
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

    }
}
VOID AT_CmdFunc_HTTPINIT(AT_CMD_PARA *pParam)
{
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       UINT8 tmpString[60] = {0x00,};
        

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_EXE:     
                       if(!flag){
                        
                           at_nHttp_info = Init_Http();
                           at_CmdRespInfoText(pParam->engine, tmpString);
                           AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                           flag = TRUE;
                       }else{

                           at_CmdRespInfoText(pParam->engine,"HTTP is already INIT, please TERM first\n");
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                        } 

                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}
VOID AT_CmdFunc_HTTPTERM(AT_CMD_PARA *pParam)
{
       UINT8 nSim = AT_SIM_ID(pParam->nDLCI);
       UINT8 tmpString[60] = {0x00,};
        

       if (NULL == pParam)
       {
               AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
       gHttpEngine = pParam->engine;
       switch (pParam->iType)
       {
               case AT_CMD_EXE:  
                       if(flag){
                           real_len = 0;
                           Term_Http(at_nHttp_info);
                           at_CmdRespInfoText(pParam->engine, tmpString);
                           AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                           flag = FALSE;
                       }else{

                           at_CmdRespInfoText(pParam->engine,"HTTP is already TERM, please INIT first\n");
                           AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                       }    
                       break;
               case AT_CMD_TEST:
                       AT_StrCpy(tmpString, "+HTTPAUTHOR:<url>,<username>,<password>");
                       at_CmdRespInfoText(pParam->engine, tmpString);
                       AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                       break;
               default:
                       AT_TC(g_sw_GPRS, "HTTPAUTHOR ERROR");
                       AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
       }
}







#endif //AT_HTTP_SUPPORT

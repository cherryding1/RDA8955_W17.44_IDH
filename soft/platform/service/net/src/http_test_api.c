
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "http_test_api.h"
#include "at_define.h"
#include "chttp.h"
#include "http_lunch_api.h"
#if 0
void TEST_HTTP_Authorization(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_p;  
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_AUTHOR){
            response_p=Http_authorization(event.nParam1,event.nParam2,event.nParam3);
            if(response_p!=NULL){
                sys_arch_printf("TEST_HTTP_Authorization success!");

                http_response_print((CgHttpPacket *)response_p);

                Http_WriteUart("success!\n",8);

                free(response_p);
            }else{
                sys_arch_printf("TEST_HTTP_Authorization failure!");

                free(response_p);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}

void TEST_HTTP_Get(void)
{
    COS_EVENT event = { 0 };
    
    CgHttpResponse *response_get;
    
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_get=Http_get(event.nParam1);
            if(response_get!=NULL){
                sys_arch_printf("TEST_HTTP_Get success!");
                http_response_print((CgHttpPacket *)response_get);

                cg_http_response_clear(response_get);
                cg_http_response_delete(response_get);
            }else{
                sys_arch_printf("TEST_HTTP_Get failure!");
                Http_WriteUart("failure, pelase check your network!\n",36);
            }         
           COS_StopTask(http_handle);
           COS_DeleteTask(http_handle); 

           
        }
    }
}
void TEST_HTTP_DownLoad(void)
{
    COS_EVENT event = { 0 };
    
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            if(Http_downLoad(event.nParam1)==TRUE){
                sys_arch_printf("TEST_HTTP_Get success!");
                Http_WriteUart("OK\n",3);
            }else{
                sys_arch_printf("TEST_HTTP_Get failure!");
                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}


void TEST_HTTP_Post(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_post;
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_post=Http_post(event.nParam1,event.nParam2,event.nParam3);
            if(response_post!=NULL){
                sys_arch_printf("TEST_HTTP_Post success!");

                http_response_print((CgHttpPacket *)response_post);

                free(response_post);

            }else{
                sys_arch_printf("TEST_HTTP_Post failure!");

                free(response_post);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}

void TEST_HTTP_Put(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_put;
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_put=Http_put(event.nParam1,"text/plain",event.nParam2,event.nParam3);
            if(response_put!=NULL){
                sys_arch_printf("TEST_HTTP_Put success!");

                http_response_print((CgHttpPacket *)response_put);

                Http_WriteUart("success!\n",8);

                free(response_put);
            }else{
                sys_arch_printf("TEST_HTTP_Put failure!");


                free(response_put);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}

VOID TEST_HTTP_Head(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_head;
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_head=Http_head(event.nParam1);
            if(response_head!=NULL){
                sys_arch_printf("TEST_HTTP_Head success!");

                http_response_print((CgHttpPacket *)response_head);
                
                Http_WriteUart("success!\n",8);
                
                free(response_head);

                
            }else{
                sys_arch_printf("TEST_HTTP_Head failure!");

                free(response_head);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}


void TEST_HTTP_Options(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_options;
    
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_options=Http_options(event.nParam1);
            if(response_options!=NULL){
                sys_arch_printf("TEST_HTTP_Options success!");

                http_response_print((CgHttpPacket *)response_options);
                
                Http_WriteUart("success!\n",8);
                
                free(response_options);
            }else{
                sys_arch_printf("TEST_HTTP_Options failure!");

                free(response_options);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }

}

void TEST_HTTP_Trace(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_trace;
    
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_trace=Http_trace(event.nParam1);
            if(response_trace!=NULL){
                sys_arch_printf("TEST_HTTP_Trace success!");

                http_response_print((CgHttpPacket *)response_trace);
                
                Http_WriteUart("success!\n",8);
                
                free(response_trace);
            }else{
                sys_arch_printf("TEST_HTTP_Trace failure!");

                free(response_trace);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}

void TEST_HTTP_Delete(void)
{
    COS_EVENT event = { 0 };
    CgHttpResponse *response_delete;
    for (;;) {
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);
        if (event.nEventId == EV_HTTP_CONNECT){
            response_delete=Http_delete(event.nParam1,event.nParam2);
            if(response_delete!=NULL){
                sys_arch_printf("TEST_HTTP_Delete success!");

                http_response_print((CgHttpPacket *)response_delete);
                
                Http_WriteUart("success!\n",8);
                
                free(response_delete);
            }else{
                sys_arch_printf("TEST_HTTP_Delete failure!");

                free(response_delete);

                Http_WriteUart("failure, pelase check your network!\n",36);
            }
        }
    }
}
#endif

void TEST_HTTP(void)
{

    
    COS_EVENT event = { 0 };
    
    CgHttpResponse *response;
    
    for (;;) {

        memset(&event,0,sizeof(event));
        
        COS_WaitEvent(http_handle, &event, COS_WAIT_FOREVER);

        switch (event.nEventId) {
                case EV_HTTP_AUTHOR:

                    Http_authorization((Http_info*)event.nParam1);

                    break;
                case EV_HTTP_GET:

                    Http_get((Http_info*)event.nParam1);
                    
                    break;
                case EV_HTTP_HEAD:

                    Http_head((Http_info*)event.nParam1);

                    break;
                case EV_HTTP_OPTIONS:

                    Http_options((Http_info*)event.nParam1);

                    break;
                case EV_HTTP_PUT:

                    Http_put((Http_info*)event.nParam1);

                    break;
                case EV_HTTP_DELETE:

                    Http_delete((Http_info*)event.nParam1);

                    break;

                case EV_HTTP_DOWNLOAD:
                    if(Http_downLoad((Http_info*)event.nParam1)==TRUE){

                        Http_WriteUart("Download succeed",16);
                        
                        sys_arch_printf("TEST_HTTP_Get success!");
                    }else{
                        Http_WriteUart("Download fail",13);
                        sys_arch_printf("TEST_HTTP_Get failure!");
                    }

                    break;
                case EV_HTTP_POST:
                    
                    Http_post((Http_info*)event.nParam1);

                    break;
                case EV_HTTP_TRACE:
                    
                    Http_trace((Http_info*)event.nParam1);

                    break;

                case EV_HTTP_PARA:

                    Http_para((nHttp_info *)event.nParam1,(char *)event.nParam2,(UINT8 *)event.nParam3);

                break;
                case EV_HTTP_GETN:

                    Http_getn((nHttp_info *)event.nParam1);

                break;
                case EV_HTTP_HEADN:

                    Http_headn((nHttp_info *)event.nParam1);

                break;
                case EV_HTTP_POSTN:

                    Http_postn((nHttp_info *)event.nParam1);

                break;
                case EV_HTTP_DELETEN:

                    Http_deleten((nHttp_info *)event.nParam1);

                break;
                case EV_HTTP_DATA:

                    http_setUserdata((nHttp_info *)event.nParam1,(UINT8 *)event.nParam2, event.nParam3);

                break;
                case EV_HTTP_READ:

                    Http_read((nHttp_info *)event.nParam1,event.nParam2,event.nParam3);

                break;
                
            }
            sys_arch_printf("FINISH++++!");
            finish_flag = 0;
            COS_StopTask(http_handle);
           
                         
    }
}


void http_response_print(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	char *content;
	long contentLen;
    char *headerName;
    char *headerValue;
    
	/**** print headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
        headerName = cg_http_header_getname(header);
        headerValue = cg_http_header_getvalue(header);
        
        Http_WriteUart((UINT8*)headerName,AT_StrLen(headerName));
        Http_WriteUart((UINT8*)headerValue,AT_StrLen(headerValue));
	}
	
	/**** print content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	
	if (content != NULL && 0 < contentLen){
		
        Http_WriteUart((UINT8*)content,contentLen);
    }
	
}



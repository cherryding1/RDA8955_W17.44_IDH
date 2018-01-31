

#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "http_test_api.h"
#include "at_define.h"
#include "chttp.h"
#include "http_lunch_api.h"


int task_flag = 0;
int finish_flag = 0;
HANDLE http_handle = NULL;

#define HTTP_THREAD_STACKSIZE			1024*4
#define HTTP_THREAD_PRIO               (COS_MMI_TASKS_PRIORITY_BASE+11)
#define HTTP_MAX_SIZE 255


BOOL lunch_http_Authorization(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_AUTHOR;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_put(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_PUT;
        ev.nParam1 = http_info1;
        start_http_task();
        //cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{

        return FALSE;
    
    }
}
BOOL lunch_http_get(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_GET;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}
BOOL lunch_http_post(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_POST;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}
BOOL lunch_http_head(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_HEAD;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}
BOOL lunch_http_options(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_OPTIONS;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}
BOOL lunch_http_trace(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_TRACE;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{

        return FALSE;

    }
}
BOOL lunch_http_delete(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_DELETE;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_html_handler(handler);
        set_memory_free_handler(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}
BOOL lunch_http_downLoad(Http_info *http_info1,packet_handler_t handler,memory_handler_t handler_m) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_DOWNLOAD;
        ev.nParam1 = http_info1;
        start_http_task();
        cg_http_set_packet_handler(handler);
        set_memory_free_handler_d(handler_m);
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_para(nHttp_info *http_info1,char *tag,char *value) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_PARA;
        ev.nParam1 = http_info1;
        ev.nParam2 = tag;
        ev.nParam3 = value;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_getn(nHttp_info *http_info1) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_GETN;
        ev.nParam1 = http_info1;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_postn(nHttp_info *http_info1) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_POSTN;
        ev.nParam1 = http_info1;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_headn(nHttp_info *http_info1) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_HEADN;
        ev.nParam1 = http_info1;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_deleten(nHttp_info *http_info1) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_DELETEN;
        ev.nParam1 = http_info1;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_data(nHttp_info *http_info1,UINT8 *data,long data_len) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_DATA;
        ev.nParam1 = http_info1;
        ev.nParam2 = data;
        ev.nParam3 = data_len;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

BOOL lunch_http_read(nHttp_info *http_info1,long offset,long length) {
    if(finish_flag == 0){
        finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = EV_HTTP_READ;
        ev.nParam1 = http_info1;
        ev.nParam2 = offset;
        ev.nParam3 = length;
        start_http_task();
        COS_SendEvent(http_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }else{
        return FALSE;
    }
}

void start_http_task(void){

    if(task_flag ==1){
        COS_DeleteTask(http_handle);
        task_flag = 0;
    }
    if(task_flag == 0){    
        http_handle = sys_thread_new("http_task", TEST_HTTP, NULL, HTTP_THREAD_STACKSIZE, HTTP_THREAD_PRIO);
        task_flag = 1;
    }
   
}


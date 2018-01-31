#include "stddef.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"
#include "sockets.h"
#include "csw.h"
#include "cfw.h"

#include "lwip_app_task.h"

HANDLE lwip_task_handle;

int lwip_app_task_flag = 0;
int lwip_app_finish_flag = 0;

BOOL lunch_coap_client(int argc, char **argv) {
    if (lwip_app_finish_flag == 0)
    {
        lwip_app_finish_flag = 1;
        COS_EVENT ev={0};
        ev.nEventId = LWIP_APP_COAP;
        ev.nParam1 = argc;
        ev.nParam2 = argv;
        start_lwip_app_task();
        COS_SendEvent(lwip_task_handle,&ev,COS_WAIT_FOREVER,COS_EVENT_PRI_NORMAL);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL lunch_lwm2m_client(int argc, char **argv) {
    if (lwip_app_finish_flag == 0)
    {
        lwip_app_finish_flag = 1;

        COS_EVENT ev= { 0 };
        ev.nEventId = LWIP_APP_LWM2M;
        ev.nParam1 = argc;
        ev.nParam2 = argv;
        start_lwip_app_task();
        COS_SendEvent(lwip_task_handle, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#ifdef AT_MIPL_SUPPORT
BOOL nbiot_lwm2m_register(UINT8 ref)
{
    if (lwip_app_finish_flag == 0)
    {
        lwip_app_finish_flag = 1;

        COS_EVENT ev= { 0 };
        ev.nEventId = LWIP_APP_MIPL_LWM2M;
        ev.nParam1 = ref;
        start_lwip_app_task();
        COS_SendEvent(lwip_task_handle, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

#ifdef AT_COAP_SUPPORT
u8_t coap_isWaitingData() {
    return coap_client_remain_data()>0;
}

void coap_sendData(UINT8 *data, UINT32 size)
{
    u32_t recived_size=0;
    recived_size = coap_client_set_payload(data,size,1);
    sys_arch_printf("Coap# coap_sendData size=%d ,total recived_size\n", size,recived_size);
}
#endif
void lwm2m_sendData(UINT8 *data, UINT32 size)
{
    //handleCommand(data);
}


void lwipAppEntry(COS_EVENT *pEvent) {
    COS_EVENT event = { 0 };
    int nResult = -1;
    for (;;) {
        memset(&event,0,sizeof(event));
        COS_WaitEvent(lwip_task_handle, &event, COS_WAIT_FOREVER);
        sys_arch_printf("lwipAppEntry waited a event :%d \n",event.nEventId);
        switch (event.nEventId) {
#ifdef AT_COAP_SUPPORT
            case LWIP_APP_COAP:
                while (coap_isWaitingData()) {
                    COS_Sleep(1000);
                }
                nResult = coap_client_main(event.nParam1,event.nParam2);
                CFW_CoapPostEvent(EV_CFW_COAP_CLIENT_RSP,event.nParam1,event.nParam2,nResult);
            break;
#endif
#ifdef AT_LWM2M_SUPPORT
            case LWIP_APP_LWM2M:
                nResult = lwm2mclient_main(event.nParam1, event.nParam2);
                CFW_LWM2MPostEvent(EV_CFW_LWM2M_DISCONN_RSP,event.nParam1,event.nParam2,nResult);
                break;
#endif
#ifdef AT_MIPL_SUPPORT
            case LWIP_APP_MIPL_LWM2M:
                mipl_register(event.nParam1);
                break;

            break;
#endif
        }
        lwip_app_finish_flag = 0;
        COS_StopTask(lwip_task_handle);
    }
}


#define LWIP_APP_STACKSIZE          2048
#define LWIP_APP_PRIO               (COS_MMI_TASKS_PRIORITY_BASE + 11)

void start_lwip_app_task()
{
    if (lwip_app_task_flag ==1)
    {
        COS_DeleteTask(lwip_task_handle);
        lwip_app_task_flag = 0;
    }

    if (lwip_app_task_flag == 0)
    {
        lwip_task_handle = sys_thread_new("lwip_task", lwipAppEntry, NULL, LWIP_APP_STACKSIZE, LWIP_APP_PRIO);
        lwip_app_task_flag = 1;
    }
}

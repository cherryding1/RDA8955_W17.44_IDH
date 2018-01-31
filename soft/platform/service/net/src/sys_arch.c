/* 
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "arch/sys_arch.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxs_io.h"
#include "api_msg.h"

int errno;

u32_t ms2ticks(u32_t mills)
{
    return (mills << 14) /1000;
}

void sys_init(void)
{
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	if(NULL == sem)
		return ERR_ARG;
	sem->sem_id = sxr_NewSemaphore(count);
	sem->is_vail = TRUE;
	sem->wait_count = 0;
	sem->is_timeout = FALSE;
    //sxr_TraceSbx(sem->sem_id,1);
	sys_arch_printf("sys_sem_new sem_id=%x\n",sem->sem_id);
	return ERR_OK;
}

static void sys_sem_timeout_cb(void* Handle)
{
	sys_sem_t *pSem = (sys_sem_t*)Handle;
	sys_arch_printf("sys_sem_timeout_cb sem_id=%x,wait_count=%d\n",pSem->sem_id,pSem->wait_count);
	if(pSem != NULL)
	{
		pSem->is_timeout = TRUE;
		pSem->wait_count--;
		sxr_ReleaseSemaphore(pSem->sem_id);	
	}
}


void sys_sem_free(sys_sem_t *sem)
{
	if(NULL == sem)
		return;
	sys_arch_printf("sys_sem_free sem_id=%x\n",sem->sem_id);

    sxr_StopFunctionTimer2(sys_sem_timeout_cb,sem);
	if(sem->wait_count > 0)
	{
		sem->wait_count--;
		sxr_ReleaseSemaphore(sem->sem_id);
	}
	sxr_FreeSemaphore(sem->sem_id);
	sem->is_vail = FALSE;
}

void sys_sem_signal(sys_sem_t *sem)
{
	if(NULL == sem)
		return;
	sys_arch_printf("sys_sem_signal sem_id=%x\n",sem->sem_id);
    sxr_StopFunctionTimer2(sys_sem_timeout_cb,sem);
	sem->wait_count--;
	sxr_ReleaseSemaphore(sem->sem_id);	
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	u32_t wait_time = 0; //Millionsecond
	sxr_StopFunctionTimer2(sys_sem_timeout_cb,sem);
	//Unsupport timeout now
	//LWIP_ASSERT("Unsupport timeout now", timeout == 0);
	sys_arch_printf("sys_arch_sem_wait sem_id=%x,timeout=%d\n",sem->sem_id,timeout);
	if(NULL == sem)
		return wait_time;
	wait_time = sys_now();
	sem->is_timeout = FALSE;
	if(timeout>0 && timeout<0xffffffffUL)
	{
		sxr_StartFunctionTimer(ms2ticks(timeout),sys_sem_timeout_cb,sem,COS_TIMER_MODE_SINGLE);
	}

	sem->wait_count++;
	sxr_TakeSemaphore(sem->sem_id);
	sxr_StopFunctionTimer2(sys_sem_timeout_cb,sem);
	if(sem->is_timeout)
		return SYS_ARCH_TIMEOUT;
	return sys_now()-wait_time;
}


int sys_sem_valid(sys_sem_t *sem)
{
	if(NULL == sem)
		return 0;
    return sem->is_vail;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    sem->is_vail = 0;
}

#if !LWIP_COMPAT_MUTEX

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{  
    mutex->id = sxr_NewMutex();
	return ERR_OK;
}
/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
    mutex->user_id = sxr_TakeMutex(mutex->id);
}
/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    sxr_ReleaseMutex(mutex->id,mutex->user_id);
}
/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
    sxr_FreeMutex(mutex->id);
}

#ifndef sys_mutex_valid
/** Check if a mutex is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mutex_valid(sys_mutex_t *mutex)
{
}
#endif
#ifndef sys_mutex_set_invalid
/** Set a mutex invalid so that sys_mutex_valid returns 0 */
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
}
#endif
#endif
/******************************************************************************
 * sys_mbox_new: 
 * DESCRIPTION: - 
 * 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * 
 ******************************************************************************/
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{	
	LWIP_ASSERT("sys_mbox_new size != 0",size==0);
	mbox->id = sxr_NewMailBox();
    mbox->is_vail = 1;
	//sxr_TraceSbx(mbox->id,1);
	return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{	
	if(NULL == mbox)
		return;
	sxr_FreeMailBox(mbox->id);
    mbox->is_vail = 0;
}



err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	sxr_Send (msg, mbox->id, SXR_SEND_MSG | SXR_QUEUE_LAST);
	return ERR_OK;

}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	sys_mbox_trypost(mbox, msg);
}



u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    if (sxr_SbxHot (mbox->id))
    {
        *msg = sxr_Wait (NIL, mbox->id);
        return 0;
    }
	return SYS_MBOX_EMPTY;
}

static void sys_mbox_fetch_timeout_cb(void* Handle)
{
	sys_mbox_t *mbox = (sys_mbox_t*)Handle;
	sys_arch_printf("sys_mbox_fetch_timeout_cb mbox_id=%d\n",mbox->id);
	if(mbox != NULL)
	{
		sxr_Send (NIL, mbox->id, SXR_SEND_MSG | SXR_QUEUE_LAST);
	}
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	u32_t wait_time = 0; //Millionsecond
	sxr_StopFunctionTimer2(sys_mbox_fetch_timeout_cb,mbox);
	//Unsupport timeout now
	//LWIP_ASSERT("Unsupport timeout now", timeout == 0);
	if(NULL == mbox)
		return wait_time;
	sys_arch_printf("sys_arch_mbox_fetch mbox_id=%d,timeout=%d,msg=0x%x\n",mbox->id,timeout,msg);
	wait_time = sys_now();

	if(timeout>0 && timeout<0xffffffffUL)
	{
		sxr_StartFunctionTimer(ms2ticks(timeout),sys_mbox_fetch_timeout_cb,mbox,COS_TIMER_MODE_SINGLE);
	}
	*msg = sxr_Wait (NIL, mbox->id);
	sxr_StopFunctionTimer2(sys_mbox_fetch_timeout_cb,mbox);
	sys_arch_printf("sys_arch_mbox_fetch wait mbox_id=%d,msg=%x\n",mbox->id,*msg);
	if(*msg == NULL)
		return SYS_ARCH_TIMEOUT;
	else
		return sys_now()-wait_time;
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
	if(NULL == mbox)
		return 0;
	return mbox->is_vail;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    mbox->is_vail = 0;
}

#if LWIP_DEBUG
void sys_arch_assert(u8_t *msg)
{
#if defined(WIN32)
	hal_DbgAssert(1,msg);
#else
	hal_DbgAssert(msg);
#endif /* WIN32 */
}

static void sys_arch_printf_ex(CONST CHAR* format, ...)
{
    va_list args;
    CHAR msg[256];
    va_start(args, format);
    vsnprintf(msg, 255,format, args);
    va_end(args);
    SXS_TRACE(_AT | TNB_ARG(0) | TSTDOUT, msg);
}

void sys_arch_printf(CONST CHAR* format, ...)
{
    CHAR * taskName = sxr_GetTaskName();
    va_list args;
    CHAR msg[256];
    va_start(args, format);
    vsnprintf(msg, 255,format, args);
    va_end(args);
    sys_arch_printf_ex("TaskName: %s -- %s\n",taskName,msg);
}
#endif

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    return COS_CreateTask(thread,arg,NULL,stacksize,prio,0,0,name);
}
/******************************************************************************
 * sys_now: 
 * DESCRIPTION: - 
 *   This optional function returns the current time in milliseconds (don't care  for wraparound, 
 *this is only used for time diffs).  Not implementing this function means you cannot use some 
 *modules (e.g. TCP  timestamps, internal timeouts for NO_SYS==1).
 * Input: 
 * Output: 
 * Returns: 
 * 
 * 
 ******************************************************************************/
u32_t sys_now(void)
{
	volatile u32_t ticks = hal_TimGetUpTime();
    return (ticks*1000)>>14;
}

u32_t sys_jiffies(void)
{
  return (u32_t)hal_TimGetUpTime(); /* todo */
}

#if !LWIP_TIMERS
typedef void (* sys_timeout_handler)(void *arg);

typedef struct timer_data {
    u32_t handler;
    u32_t args;
    struct timer_data *next;
} timer_data_t;

static void timer_sender(void *arg){
    /*
    COS_EVENT evt;
    evt.nEventId = HVY_TIMER_IN + PRV_CFW_IP_TIMER_ID;
    evt.nParam1 = 0;
    evt.nParam2 = (UINT32)arg;
    BAL_SendTaskEvent(&evt, CSW_TASK_ID_APS);
    */
    CFW_BalSendMessage(CFW_MBX, PRV_CFW_IP_TIMER_ID, &arg, 4, CFW_SIM_END);
}

timer_data_t *g_timer_list = NULL;

timer_data_t *findOrCreateTimer(sys_timeout_handler handler, void *arg)
{
    u32 Status     = sxr_EnterSc ();
    timer_data_t *tmp = g_timer_list;
    timer_data_t *pre = NULL;
    while (tmp != NULL) {
        if (tmp->handler == (u32_t)handler && tmp->args == (u32_t)arg)
            break;
        pre = tmp;
        tmp = tmp->next;
    }
    if (tmp != NULL) {
        sxr_ExitSc (Status);
        return tmp;
    }
    timer_data_t *timer_data = (timer_data_t *)COS_MALLOC(sizeof(timer_data_t));
    timer_data->handler = handler;
    timer_data->args = arg;
    timer_data->next = NULL;
    if (pre == NULL) {
        g_timer_list = timer_data;
    } else {
        pre->next = timer_data;
    }
    sxr_ExitSc (Status);
    return timer_data;
}

timer_data_t *findAndDelTimer(sys_timeout_handler handler, void *arg)
{
    u32 Status     = sxr_EnterSc ();
    timer_data_t *tmp = g_timer_list;
    timer_data_t *pre = NULL;
    while (tmp != NULL) {
        if (tmp->handler == (u32_t)handler && tmp->args == (u32_t)arg)
            break;
        pre = tmp;
        tmp = tmp->next;
    }
    if (pre == NULL)
        g_timer_list = g_timer_list->next;
    else if (tmp != NULL)
        pre->next = tmp->next;
    sxr_ExitSc (Status);
    return tmp;
}

void sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg)
{
    sys_arch_printf("sys_timeout msecs=%d,handler=0x%x,arg=0x%x",msecs,handler,arg);
    timer_data_t *timer_data = findOrCreateTimer(handler,arg);
    sxr_StartFunctionTimer(ms2ticks(msecs),timer_sender,timer_data,0);
}

void
sys_untimeout(sys_timeout_handler handler, void *arg)
{
    sys_arch_printf("sys_untimeout handler=0x%x,arg=0x%x",handler,arg);
    timer_data_t *timer_data = findAndDelTimer(handler,arg);
    if (timer_data != NULL) {
        int ret = sxr_StopFunctionTimer2(timer_sender,timer_data);
        sys_arch_printf("sys_untimeout ret=%d",ret);
        COS_FREE(timer_data);
    } else {
        sys_arch_printf("sys_untimeout NOT FOUND");
    }
        
}

void sys_dotimeout(void *arg) 
{
    timer_data_t *timer_data = (timer_data_t *)arg;
    sys_timeout_handler handler = (sys_timeout_handler)timer_data->handler;
    void *args = (void *)timer_data->args;
    sys_arch_printf("sys_dotimeout handler=0x%x,args=0x%x",handler,args);
    handler(args);
}

#endif


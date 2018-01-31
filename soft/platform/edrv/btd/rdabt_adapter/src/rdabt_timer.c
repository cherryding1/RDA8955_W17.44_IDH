/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _RDABT_TIMER_C
#define _RDABT_TIMER_C

/*****************************************************************************
* Include
*****************************************************************************/
/* basic system service headers */
#include "kal_release.h"        /* basic data type */
#include "stack_common.h"          /* message and module ids */
#include "stack_msgs.h"            /* enum for message ids */
#include "app_ltlcom.h"         /* task message communiction */
//#include "syscomp_config.h"         /* type of system module components */
//#include "task_config.h"          /* task creation */
#include "stacklib.h"              /* basic type for dll, evshed, stacktimer */
#include "event_shed.h"         /* event scheduler */
#include "stack_timer.h"        /* stack timer */
//#include "stdio.h"                  /* basic c i/o functions */

/* RDABT internal headers */
#include "rdabt.h"                   /* global header file of rdabt */
//#include "rdabt_timer.h"             /* header file of timer utilities */
#include "ts.h"
#include "papi.h"
/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/

/*****************************************************************************
* Local Variable
*****************************************************************************/

/*****************************************************************************
* Local Function
*****************************************************************************/

/*****************************************************************************
* Global Variable
*****************************************************************************/
//extern rdabt_context_struct* rdabt_p;
/*****************************************************************************
* Global Function
*****************************************************************************/
void *rdabt_pmalloc(kal_uint32 size);
void pFree(void *ptr);

/*****************************************************************************
* FUNCTION
*   rdabt_timed_deinit
* DESCRIPTION
*   Remove all times
* PARAMETERS
*   None.
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_timed_deinit(void)
{
#if 0
    rdabt_timer_struct* curr_timer = rdabt_p->timer_list.t_head;
    while(curr_timer)
    {
        rdabt_p->timer_list.t_head = rdabt_p->timer_list.t_head->next;

        /* remove event from event scheduler */
        //evshed_cancel_event (rdabt_p->evsh_sched, &curr_timer->evsh_id);
        sxr_StopFunctionTimer(curr_timer->callback);

        pFree(curr_timer);
        curr_timer = rdabt_p->timer_list.t_head;
    }

    rdabt_p->timer_list.t_head = rdabt_p->timer_list.t_tail = NULL;
#endif
}


/*****************************************************************************
* FUNCTION
*   rdabt_timer_expiry_hdlr
* DESCRIPTION
*   General handler of timer TIMER_EXPIRY message
* PARAMETERS
*   param IN local parameter of message
* RETURNS
*   None.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
void rdabt_timer_expiry_hdlr(void *local_para)
{
#if 0
    /* check if the timer is already stopped */
    if (stack_is_time_out_valid(&rdabt_p->base_timer))
        evshed_timer_handler(rdabt_p->evsh_sched);

    stack_process_time_out(&rdabt_p->base_timer);
#endif
}


/*****************************************************************************
* FUNCTION
*   rdabt_time_msec_2_tick
* DESCRIPTION
*   This function converts millisecond to system ticks
* PARAMETERS
*   msec IN millisecond to be converted
* RETURNS
*   number of ticks
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
kal_uint32 rdabt_time_msec_2_tick(kal_uint32 msec)
{
    kal_uint32 num_ticks;

    num_ticks = msec/4615;

    while(num_ticks*4615 < msec)
        num_ticks++;


    return num_ticks;
}


/*****************************************************************************
* FUNCTION
*   rdabt_time_ticks_2_msec
* DESCRIPTION
*   This function converts millisecond to system ticks
* PARAMETERS
*   msec IN micro second to be converted
* RETURNS
*   number of ticks
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
kal_uint32 rdabt_time_ticks_2_msec(kal_uint32 ticks)
{
    return ticks*4615;
}


/*****************************************************************************
* FUNCTION
*   rdabt_get_time
* DESCRIPTION
*   Get current system time
* PARAMETERS
*   None.
* RETURNS
*   Current system tick
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
kal_uint32 rdabt_get_time(void)
{
    kal_uint32 sys_time;
    kal_get_time(&sys_time);
    return rdabt_time_ticks_2_msec(sys_time);
}


/*****************************************************************************
* FUNCTION
*   rdabt_timed_event_in
* DESCRIPTION
*   Start a timer for a duration
* PARAMETERS
*   duration   IN    duration of timer in microsecond
*   fn        IN    callback function when timer expires
*   mi        IN    first parameter of timer callback function
*   mv       IN    second parameter of timer callback function
* RETURNS
*   sequence number of timer (for cancel case).
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
scheduler_identifier rdabt_timed_event_in(kal_uint32 duration, rdabt_timer_func fn, /*uint16_t mi,*/ void *mv)
{
    if(duration <1000)
        asm("break 1");
    return pTimerCreateMs(duration/1000,fn,mv,0);

}




/*****************************************************************************
* FUNCTION
*   rdabt_cancel_timed_event
* DESCRIPTION
*   Cancel a timer
* PARAMETERS
*   eventid     IN    timer id to be cancelled
*   pmi        OUT    first parameter of timer callback function
*   pmv       OUT    second parameter of timer callback function
* RETURNS
*   TRUE if cancel successfully; otherwise, FALSE.
* GLOBALS AFFECTED
*   None.
*****************************************************************************/
kal_bool rdabt_cancel_timed_event(scheduler_identifier event_id/*, kal_uint16 *pmi, void **pmv*/)
{
    pTimerCancel(event_id);
#if 0
    rdabt_timer_struct *prev_cursor = NULL;
    rdabt_timer_struct *curr_cursor;
    curr_cursor = rdabt_p->timer_list.t_head;

    /* traverse each link list to find out the timer to execute */
    while(curr_cursor!= NULL)
    {
        if(curr_cursor->timer_id == eventid)
            break;

        prev_cursor = curr_cursor;
        curr_cursor = curr_cursor->next;
    }

    if(curr_cursor == NULL)
        return KAL_FALSE;

    /* remove timer from list */
    if(curr_cursor == rdabt_p->timer_list.t_head) /* first timer */
    {
        if(rdabt_p->timer_list.t_head == rdabt_p->timer_list.t_tail)
            rdabt_p->timer_list.t_head = rdabt_p->timer_list.t_tail = NULL;
        else
            rdabt_p->timer_list.t_head = curr_cursor->next;
    }
    else if(curr_cursor == rdabt_p->timer_list.t_tail)
    {
        rdabt_p->timer_list.t_tail = prev_cursor;
        rdabt_p->timer_list.t_tail->next = NULL;
    }
    else
        prev_cursor->next = curr_cursor->next; /* remove curr_cursor from list */
#if 0
    /* remove event from event scheduler */
    evshed_cancel_event (rdabt_p->evsh_sched, &curr_cursor->evsh_id);
#else
    sxr_StopFunctionTimer(curr_cursor->callback);
#endif
    /* restore mi and mv parameters */
    /*
       if(pmi)
          *pmi = curr_cursor->mi;

       if(pmv)
          *pmv = curr_cursor->mv;
    */
    pFree(curr_cursor);
#endif
    return KAL_TRUE;
}
#endif /* #ifndef _RDABT_TIMER_C */

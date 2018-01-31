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

#ifndef __RDABT_H__
#define __RDABT_H__

#define BT_DRIVER_VERSION   0x0345
#include "kal_release.h"          /* basic data type */
//#include "stack_common.h"       /* message and module ids */
//#include "stack_msgs.h"         /* enum for message ids */
#include "app_ltlcom.h"           /* task message communiction */
//#include "syscomp_config.h"     /* type of system module components */
//#include "task_config.h"          /* task creation */
#include "stacklib.h"            /* basic type for dll, evshed, stacktimer */
#include "event_shed.h"           /* event scheduler */
#include "stack_timer.h"          /* stack timer */
//#include "cs_types.h"
//#include "bt_user_config.h"
#include "bluetooth_bm_struct.h"


#define BLUETOOTH_NO_TCS

#define RDABT_MAX_SEQ_NUM  0xFFFFFFFF  /* maximum sequence number */

#define RDABT_RX_BUFF_SIZE  4096      /* maximum size of Rx buffer */
#define RDABT_TEMP_MEM_SIZE (100*1024+140*1024)

#define RDABT_NUM_TASK   1

/* global state of rdabt*/
#define RDABT_STATE_POWER_OFF  0
#define RDABT_STATE_POWER_ON   1
#define RDABT_STATE_PANIC      2
#define RDABT_PANIC_NONE      0

/*****************************************************************************
* Typedef
*****************************************************************************/
#define SIZE_OF_BDADDR 6

#ifndef BD_ADDR_CMP /* An optimised version may be defined in platform_operations.h */
#define BD_ADDR_CMP(x,y) !pMemcmp((x).bytes, (y).bytes, SIZE_OF_BDADDR)
#endif

typedef struct
{
    kal_uint16  NAP;
    kal_uint8   UAP;
    kal_uint32 LAP;
} rdabt_bd_addr_struct;
/*
typedef struct {
    kal_uint8 bytes[6];
} t_bdaddr;
*/
#define BDADDR_Get_LAP(p_bd_addr)                                       \
            ( ((kal_uint32) (p_bd_addr)->bytes[2] << 16) +                   \
            (  (kal_uint32) (p_bd_addr)->bytes[1] << 8) + (p_bd_addr)->bytes[0] )

#define BDADDR_Get_UAP(p_bd_addr)                                       \
            ( (kal_uint8) (p_bd_addr)->bytes[3] )

#define BDADDR_Get_NAP(p_bd_addr)                                       \
            ( (kal_uint16) (((kal_uint16) (p_bd_addr)->bytes[5] << 8) +           \
              (p_bd_addr)->bytes[4]))

#define BDADDR_Get_UAP_LAP(p_bd_addr)                                   \
             ( (kal_uint32) BDADDR_Get_UAP(p_bd_addr)<<24 |              \
               (kal_uint32) BDADDR_Get_LAP(p_bd_addr) )

#define BDADDR_Set_LAP(p_bd_addr, LAP)                   \
            {  kal_uint32 lap = (LAP);                                       \
               (p_bd_addr)->bytes[0] = (u_int8)(lap & 0xFF);            \
               (p_bd_addr)->bytes[1] = (u_int8)((lap>>8) & 0xFF);       \
               (p_bd_addr)->bytes[2] = (u_int8) (lap>>16); }

#define BDADDR_Set_UAP(p_bd_addr, UAP)    \
            ((p_bd_addr)->bytes[3] = (UAP))

#define BDADDR_Set_NAP(p_bd_addr, NAP)        \
            {  kal_uint16 nap = (NAP);                                       \
               (p_bd_addr)->bytes[4] = (u_int8) (nap & 0xFF);           \
               (p_bd_addr)->bytes[5] = (u_int8) (nap>>8); }

#define BDADDR_Set_LAP_UAP_NAP(p_bd_addr, lap, uap, nap)   \
            {  BDADDR_Set_LAP(p_bd_addr, lap);                          \
               BDADDR_Set_UAP(p_bd_addr, uap);                          \
               BDADDR_Set_NAP(p_bd_addr, nap); }

typedef kal_uint32                scheduler_identifier;
//typedef scheduler_identifier    tid;
//typedef scheduler_identifier    msgid;
typedef kal_uint16                 qid;
typedef kal_uint16                taskid;

//typedef void (* rdabt_timer_func) (uint16_t, void*);
typedef void (* rdabt_timer_func) (void*);
typedef void (* rdabt_task_func) (void**);
typedef void (* rdabt_external_msg_func)(ilm_struct*, void*);

/* basic timer element */
typedef struct rdabt_timer_struct_def
{
    struct rdabt_timer_struct_def *next;    /* Pointer to the next timer in the list */
    kal_uint32 timer_id;              /* Sequence number of  timer within BCHS task. Used for
                                internal tasks to cancel a timer */
    rdabt_timer_func callback;   /* Callback function when timer expired */
    void *mv;                 /* Second parameter of callback function. Change the order
                                of two parameters because of alignment. */
    //uint16_t mi;               /* First parameter of callback function */
} rdabt_timer_struct;

/* structure for a queue element */
typedef struct rdabt_queue_entry_struct_def
{
    struct rdabt_queue_entry_struct_def *next;
    kal_uint16                        mi;
    void                            *mv;
    scheduler_identifier                            id;
    kal_uint16                        sender;
} rdabt_queue_entry_struct;

/* structure for a task instance */
typedef struct
{
    rdabt_task_func                 init_func;
    rdabt_task_func                 handler_func;
    rdabt_task_func                 deinit_func;
    void                                 *task_inst;
    rdabt_queue_entry_struct           *q_head;
    rdabt_queue_entry_struct           *q_tail;
} rdabt_int_task_info_struct;
#if 0
/* timer list */
typedef struct
{
    rdabt_timer_struct *t_head;  /* head of timer list */
    rdabt_timer_struct *t_tail;    /* tail of timer list */
} rdabt_timer_list_struct;
/* global context */
typedef struct
{
    kal_uint32           seq_num;      /* global sequence number */
    event_scheduler       *evsh_sched;  /* Event scheduler of BCHS task */
    stack_timer_struct      base_timer;   /* Base timer of event scheduler */
    rdabt_timer_list_struct   timer_list;    /* Information for list of timers in BCHS */
    kal_uint32            num_int_msg; /* Aggregated number of messages in
                                         all of the internal queues. Auxiliary
                                         variable for scheduling. */
    kal_msgqid           queue_id;       /* external queue id */
    task_indx_type      task_id;
    kal_mutexid          mem_mutex;     /* mutex for ADM memory */
    kal_uint32           rx_cursor;     /* starting address of rx_buffer */
    kal_uint32           rx_length;     /* length of data in rx_buffer */
    qid                   curr_queue;  /* queue ID of current task */
    KAL_ADM_ID            mem_pool_id; /* Index of ADM memory pool */
    rdabt_int_task_info_struct task_list[RDABT_NUM_TASK]; /* internal task list */
    //scDbDeviceRecord_t     sc_cache[BCHS_MAX_LINK_KEY+1]; /* link key */
    kal_uint8              rx_buff[RDABT_RX_BUFF_SIZE]; /* Rx buffer in local side */
    //kal_uint8               mem_pool[RDABT_TEMP_MEM_SIZE]; /* memory pool */
    kal_bool               host_wake_up;
    kal_uint8               panic_choke; /* choke flag to prevent processing message when panic happens */
    kal_bool               sent_rx_ind;
    kal_bool               sent_tx_ind;
    kal_uint8               state;
//   kal_uint8              l1_handle;
    kal_bool               chip_knocked;
    kal_bool               original_boot_state;
} rdabt_context_struct;
#endif


#endif



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

#ifndef __RDABT_MAIN_H__
#define __RDABT_MAIN_H__

#include "bt.h"
#include "papi.h"
#include "event.h"
#define TRANSPORT_UART_ID       (1)

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

#define RDA_UPTOMMI_PRIM_BASE      ((u_int16) (0x8000))


#define LINK_Q   0x0101
#define A2DP_Q      0x0200
#define AVRCP_Q    0x0300
#define HFP_Q        0x0400
#define HSP_Q        0x0800


/* first the common core stack parts */
//extern  void    init_link(void **gash);
//extern  void    link_handler(void **gash);
#define LINK_INIT           NULL
#define LINK_TASK           NULL
#define LINK_DEINIT                NULL


#define LINK_QUEUE           (0)

#define EXTERNAL_TASK_MASK      0x4000
extern void BT_Send_Msg_Up(u_int32 msg_id, u_int32 param);
//extern BOOL  hal_HstSendEvent(u_int32 event);

typedef u_int8  t_freq;        /* frequency type */
typedef u_int8  t_uap;         /* upper address part type */
typedef u_int32 t_lap;         /* lower address part type */
typedef u_int16 t_nap;         /* non-significant address part type */
typedef u_int32 t_clock;       /* clock type */
typedef u_int16 t_status;      /* status type */
typedef u_int8  t_am_addr;     /* AM address type */
typedef u_int8  t_lt_addr;     /* LT address type */

/*****************************************************************************
* Typedef
*****************************************************************************/
#define SIZE_OF_BDADDR 6

#ifndef BD_ADDR_CMP /* An optimised version may be defined in platform_operations.h */
#define BD_ADDR_CMP(x,y) !memcmp(((x).bytes), ((y).bytes), SIZE_OF_BDADDR)
#endif



/*
typedef struct
{
   u_int16  NAP;
   u_int8   UAP;
   u_int32 LAP;
}rdabt_bd_addr_struct;

#define BDADDR_Get_LAP(p_bd_addr)                                       \
            ( ((u_int32) (p_bd_addr)->bytes[2] << 16) +                   \
            (  (u_int32) (p_bd_addr)->bytes[1] << 8) + (p_bd_addr)->bytes[0] )

#define BDADDR_Get_UAP(p_bd_addr)                                       \
            ( (u_int8) (p_bd_addr)->bytes[3] )

#define BDADDR_Get_NAP(p_bd_addr)                                       \
            ( (u_int16) (((u_int16) (p_bd_addr)->bytes[5] << 8) +           \
              (p_bd_addr)->bytes[4]))

#define BDADDR_Get_UAP_LAP(p_bd_addr)                                   \
             ( (u_int32) BDADDR_Get_UAP(p_bd_addr)<<24 |              \
               (u_int32) BDADDR_Get_LAP(p_bd_addr) )

#define BDADDR_Set_LAP(p_bd_addr, LAP)                   \
            {  u_int32 lap = (LAP);                                       \
               (p_bd_addr)->bytes[0] = (u_int8)(lap & 0xFF);            \
               (p_bd_addr)->bytes[1] = (u_int8)((lap>>8) & 0xFF);       \
               (p_bd_addr)->bytes[2] = (u_int8) (lap>>16); }

#define BDADDR_Set_UAP(p_bd_addr, UAP)    \
            ((p_bd_addr)->bytes[3] = (UAP))

#define BDADDR_Set_NAP(p_bd_addr, NAP)        \
            {  u_int16 nap = (NAP);                                       \
               (p_bd_addr)->bytes[4] = (u_int8) (nap & 0xFF);           \
               (p_bd_addr)->bytes[5] = (u_int8) (nap>>8); }
*/

/*#define BDADDR_Set_LAP_UAP_NAP(p_bd_addr, lap, uap, nap)   \
            {  BDADDR_Set_LAP(p_bd_addr, lap);                          \
               BDADDR_Set_UAP(p_bd_addr, uap);                          \
               BDADDR_Set_NAP(p_bd_addr, nap); }
*/
#define RDA_SEND_COSMSG(MOD_DEST,P_COS_EVENT) \
    sxr_Send(P_COS_EVENT,COS_TaskMailBoxId(GetMmiTaskHandle(MOD_DEST)), SXR_SEND_EVT)


#define rda_msg_send_ext_queue(ilm_ptr) \
do \
{ \
    COS_EVENT ev; \
    ev.nEventId = ilm_ptr->msg_id; \
    ev.nParam1 = (UINT32)ilm_ptr; \
    RDA_SEND_COSMSG(ilm_ptr->dest_mod_id, &ev); \
} while (0)

/* global context */
typedef struct
{
//   stack_timer_struct      base_timer;   /* Base timer of event scheduler */
//   kal_msgqid           queue_id;       /* external queue id */
    u_int8               timer_id;
    u_int8               host_wake_up;
    u_int8               state;
    u_int8               tx_busy;
    u_int8              l1_handle;
    u_int8               chip_knocked;
    u_int8               original_boot_state;
} rdabt_context_struct;

#endif


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



#if 0
#include "stdlib.h"
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
#include "gpio_sw.h"               /* for gpio */

#include "bluetooth_bm_struct.h"

#include "bt_a2dp_struct.h"
#include "rda_bt_hsg.h"
#include "rdabt_avrcp.h"
#include "bt_dm_struct.h"
#include "bt_avrcp_struct.h"


#include "rdabt.h"
#include "bt.h"


#include "rdabt_task.h"
#include "rdabt_profile.h"
#include "rdabt_link_prim.h"
#include "rdabt_hsp.h"

#include "bt_hsg_struct.h"
//#include "rda_bt_hfg.h"
#include "mmi_trace.h"
#include "papi.h"
extern void HangupAllCalls(void);

typedef enum
{
    HSG_ACTIVATE_CNF=0,
    HSG_DEACTIVATE_CNF,
    HSG_ACL_CONNECT_CNF,
    HSG_ACL_DISCONNECT_CNF,
    HSG_REJECT_INCOMING_CALL_CNF,
    HSG_DISCONNECT_FROM_HS_IND,
    HSG_BLDN_FROM_HS_IND,
    HSG_RING_CNF,
    HSG_ACL_CONNECT_IND,
    HSG_ACL_DISCONNECT_IND,
    HSG_SPEAKER_GAIN_IND,
    HSG_MIC_GAIN_IND
} HSP_Q_T;

const struct
{
    kal_int16   type;
    void        (*handler)(void  *);
} hsp_msg_hdlr_table[] =
{
    {HSG_ACTIVATE_CNF,  rdabt_hsg_activate_cnf_cb},
    {HSG_DEACTIVATE_CNF,  rdabt_hsg_deactivate_cnf_cb},
    {HSG_ACL_CONNECT_CNF,  rdabt_hsg_connect_cnf_cb},
    {HSG_ACL_DISCONNECT_CNF,  rdabt_hsg_disconnect_cnf_cb},
    {HSG_REJECT_INCOMING_CALL_CNF,  rdabt_hsg_reject_incoming_call_cnf_cb},
    {HSG_DISCONNECT_FROM_HS_IND,  rdabt_hsg_disconnect_from_hs_ind_cb},
    {HSG_BLDN_FROM_HS_IND,  rdabt_hsg_bldn_from_hs_ind_cb},
    {HSG_RING_CNF,  rdabt_hsg_ring_cnf_cb},
    {HSG_ACL_CONNECT_IND, rdabt_hsg_connect_ind_cb},
    {HSG_ACL_DISCONNECT_IND, rdabt_hsg_disconnect_ind_cb},
    {HSG_SPEAKER_GAIN_IND, rdabt_hsg_speaker_gain_ind_cb},
    {HSG_MIC_GAIN_IND, rdabt_hsg_mic_gain_ind_cb}
};

void rdabt_hsg_reject_incoming_call_cnf_cb(void * pArgs)
{
    kal_prompt_trace(1, "rdabt_hsg_reject_incoming_call_cnf_cb");
//There are not reject function in headset profile!!
}

void rdabt_hsg_ring_cnf_cb(void * pArgs)
{


    //  kal_uint8 result =*(kal_uint8*)pArgs;
    bt_hsg_incoming_accept_req_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_incoming_accept_req_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_incoming_accept_req_struct), TD_CTRL);
//    cnf_param_ptr ->result = result;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_INCOMING_ACCEPT_REQ, (void *)cnf_param_ptr, NULL);

}

void rdabt_hsp_int_msg_hdlr( kal_uint16 mi,void *mv )
{
    int I;
    int n = sizeof(hsp_msg_hdlr_table)/sizeof(hsp_msg_hdlr_table[0]);
    kal_uint16 type = (mi & 0x00ff);
    for(I=0; I<n; I++)
    {
        if( type==hsp_msg_hdlr_table[I].type )
        {
            hsp_msg_hdlr_table[I].handler( mv );
            break;
        }
    }
}

void rdabt_hsg_activate_cnf_cb(void * pArgs)
{
    kal_uint8 result =*(kal_uint8*)pArgs;
    bt_hsg_activate_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_activate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_activate_cnf_struct), TD_CTRL);
    kal_prompt_trace(1, "rdabt_hsg_activate_cnf_cb result=%d", result);
    cnf_param_ptr ->result = result;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_ACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_deactivate_cnf_cb(void * pArgs)
{
    kal_uint8 result =*(kal_uint8*)pArgs;
    bt_hsg_deactivate_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_deactivate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_deactivate_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = result;

    kal_prompt_trace(1, "rdabt_hsg_deactivate_cnf_cb");
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_DEACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
}
extern scheduler_identifier pin_code_ind_timer;
void rdabt_hsg_connect_cnf_cb(void * pArgs)
{
    st_t_hsp_connect_cnf *msg_p =(st_t_hsp_connect_cnf *)pArgs;
    bt_hsg_connect_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_connect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_connect_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = msg_p->result;
    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->bt_addr.lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);
    if(pin_code_ind_timer)//zhou siyou add 20080729
    {
        pTimerCancel(pin_code_ind_timer);
        pin_code_ind_timer=0;
    }
    kal_prompt_trace(g_sw_BT, "rdabt_hsg_connect_cnf_cb");
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_CONNECT_CNF, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_connect_ind_cb(void * pArgs)
{
    st_t_hsg_connect_ind *msg_p =(st_t_hsg_connect_ind *)pArgs;
    bt_hsg_connect_ind_struct *cnf_param_ptr;

    cnf_param_ptr = (bt_hsg_connect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_connect_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->bt_addr.lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);

    kal_prompt_trace(1, "rdabt_hsg_connect_ind_cb");
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_CONNECT_IND, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_disconnect_cnf_cb(void * pArgs)
{
    st_t_hsp_connect_cnf *msg_p =(st_t_hsp_connect_cnf *)pArgs;
    bt_hsg_disconnect_cnf_struct *cnf_param_ptr;

    cnf_param_ptr = (bt_hsg_disconnect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_disconnect_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = msg_p->result;
    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->bt_addr.lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);

    kal_prompt_trace(1, "rdabt_hsg_disconnect_cnf_cb msg_p->connection_id:%x",msg_p->connection_id);
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_DISCONNECT_CNF, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_disconnect_ind_cb(void * pArgs)
{
    st_t_hsg_disconnect_ind *msg_p =(st_t_hsg_disconnect_ind *)pArgs;
    bt_hsg_disconnect_ind_struct *cnf_param_ptr;

    kal_prompt_trace(1, "rdabt_hsg_disconnect_ind_cb");

    cnf_param_ptr = (bt_hsg_disconnect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_disconnect_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->bt_addr.lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_DISCONNECT_IND, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_set_hs_volume_cnf_cb(void * pArgs)
{
    st_t_hsp_set_volume_cnf *msg_p = (st_t_hsp_set_volume_cnf *)pArgs;
    bt_hsg_speaker_gain_cnf_struct  *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_speaker_gain_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_speaker_gain_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = msg_p->result;
    cnf_param_ptr->connection_id = msg_p->connection_id;

    rdabt_send_msg_up(MOD_MED, MSG_ID_BT_HSG_SPEAKER_GAIN_CNF, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_disconnect_from_hs_ind_cb(void * pArgs)
{
    kal_uint8 connection_id = *(kal_uint8*)pArgs;

    kal_prompt_trace(1, "rdabt_hsg_disconnect_from_hs_ind_cb connection_id=%d", connection_id);
    bt_hsg_hangup_req_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_hangup_req_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_hangup_req_struct), TD_CTRL);
//   cnf_param_ptr ->result = result;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_HANGUP_REQ, (void *)cnf_param_ptr, NULL);




}

void rdabt_hsg_speaker_gain_ind_cb(void * pArgs)
{
    st_t_hsg_gain_ind *msg_p =(st_t_hsg_gain_ind *)pArgs;
    bt_hsg_speaker_gain_ind_struct *cnf_param_ptr;

    kal_prompt_trace(1, "rdabt_hsg_speaker_gain_ind_cb");

    cnf_param_ptr = (bt_hsg_speaker_gain_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_speaker_gain_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->gain = msg_p->gain;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_SPEAKER_GAIN_IND, (void *)cnf_param_ptr, NULL);
}

void rdabt_hsg_mic_gain_ind_cb(void * pArgs)
{
    st_t_hsg_gain_ind *msg_p =(st_t_hsg_gain_ind *)pArgs;
    bt_hsg_mic_gain_ind_struct *cnf_param_ptr;

    kal_prompt_trace(1, "rdabt_hsg_mic_gain_ind_cb");

    cnf_param_ptr = (bt_hsg_mic_gain_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_mic_gain_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->gain = msg_p->gain;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HSG_MIC_GAIN_IND, (void *)cnf_param_ptr, NULL);
}



extern t_SDP_service_Parse_Results uuid_finded;

void rdabt_hsg_msg_hdler(ilm_struct *message)
{
    switch(message->msg_id)
    {
        case MSG_ID_BT_HSG_ACTIVATE_REQ:
        {
            rda_bt_hsg_send_activate_req();
            break;
        }

        case MSG_ID_BT_HSG_DEACTIVATE_REQ:
        {
            rda_bt_hsg_send_deactivate_req();
            break;
        }

        case MSG_ID_BT_HSG_CONNECT_REQ:
        {
            t_bdaddr bd_addr;
            kal_uint8 chn_num=0;
//                kal_uint8 i=0;
            bt_hsg_connect_req_struct  *msg_req= (bt_hsg_connect_req_struct*)message->local_para_ptr;

            /*for(i=0;i<uuid_finded.total_searched;i++)
            {
                if(uuid_finded.service[i].PDL_uuid==SDP_SCLASS_UUID_HEADSET)
                    chn_num = uuid_finded.service[i].chn_num;

                kal_prompt_trace(1, "mmi_bth_hfg_connect_req_hdler PDL_uuid=%x, chn_num=%d", uuid_finded.service[i].PDL_uuid,uuid_finded.service[i].chn_num);
            }*/

            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            rda_bt_hsg_send_connect_req(chn_num, bd_addr);
            break;
        }

        case MSG_ID_BT_HSG_DISCONNECT_REQ:
        {
            t_bdaddr bd_addr;
            bt_hsg_disconnect_req_struct  *msg_req= (bt_hsg_disconnect_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_BT_HSG_DISCONNECT_REQ");
            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            rda_bt_hsg_send_disconnect_req(msg_req->connection_id, bd_addr);
            break;
        }

        case MSG_ID_BT_HSG_SPEAKER_GAIN_REQ:
        {
            bt_hsg_speaker_gain_req_struct  *msg_req= (bt_hsg_speaker_gain_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_BT_HSG_SPEAKER_GAIN_REQ");
            rda_bt_hsg_send_set_volume_req(msg_req->connection_id,msg_req->gain);
            break;
        }

        default:
            ASSERT(0);
            break;
            //end of hfp msg
    }

}
void rdabt_hsg_bldn_from_hs_ind_cb(void * pArgs)
{

}
#endif

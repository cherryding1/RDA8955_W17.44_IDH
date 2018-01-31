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
//#include "rda_bt_hfg.h"
//#include "rda_bt_avrcp.h"
#include "bt_dm_struct.h"
#include "bt_hfg_struct.h"
#include "bt_avrcp_struct.h"
//#include "rdabtmmia2dp.h"

#include "rdabt.h"
#include "bt.h"
//#include "manage.h"
#include "avrcp.h"

#include "rdabt_task.h"
#include "rdabt_profile.h"
#include "rdabt_link_prim.h"
#include "rdabt_avrcp.h"
#include "mmi_trace.h"

#if 1
static u_int8 avrcp_chnl_num;
static u_int8 avrcp_local_role;

static u_int8 avrcp_current_staust=0; //0 ,idle, 1 receive cmd res not res still ;


extern u_int8 A2DP_local_role;
extern  t_bdaddr a2dp_snk_remote_addr;
bt_avrcp_dev_addr_struct a2dp_avrcp_remote_addr;// connet to headset,local init con avrcp
u_int16 avrcp_connect_id=0;

void rdabt_avrcp_connect_ind_cb(void * pArgs)
{
    t_avrcp_connect_inf *msg_p = (t_avrcp_connect_inf*)pArgs;
    bt_avrcp_connect_ind_struct *cnf_param_ptr;
#ifdef __SUPPORT_BT_A2DP_SNK__

    if(A2DP_local_role==BT_A2DP_SINK)
        memcpy(msg_p->device_addr.bytes,a2dp_snk_remote_addr.bytes,6);
#endif
    cnf_param_ptr = (bt_avrcp_connect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_connect_ind_struct), TD_CTRL);
    cnf_param_ptr->chnl_num = avrcp_chnl_num;
    cnf_param_ptr->connect_id = msg_p->connect_id;
    cnf_param_ptr->device_addr.lap = BDADDR_Get_LAP(&msg_p->device_addr);
    cnf_param_ptr->device_addr.nap = BDADDR_Get_NAP(&msg_p->device_addr);
    cnf_param_ptr->device_addr.uap = BDADDR_Get_UAP(&msg_p->device_addr);
    avrcp_connect_id=msg_p->connect_id;
    a2dp_avrcp_remote_addr.uap= cnf_param_ptr->device_addr.uap;
    a2dp_avrcp_remote_addr.nap=  cnf_param_ptr->device_addr.nap;
    a2dp_avrcp_remote_addr.lap= cnf_param_ptr->device_addr.lap;

    if(( cnf_param_ptr->device_addr.uap==0)&&( cnf_param_ptr->device_addr.nap==0)&&( cnf_param_ptr->device_addr.lap==0))
    {
        cnf_param_ptr->device_addr.uap =a2dp_avrcp_remote_addr.uap;
        cnf_param_ptr->device_addr.nap =a2dp_avrcp_remote_addr.nap;
        cnf_param_ptr->device_addr.lap =a2dp_avrcp_remote_addr.lap;
    }        
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_CONNECT_IND, (void *)cnf_param_ptr, NULL);	
	avrcp_current_staust=0; 

}

void rdabt_avrcp_connect_cnf_cb(void * pArgs)
{
    t_avrcp_connect_inf *msg_p = (t_avrcp_connect_inf *)pArgs;
    bt_avrcp_connect_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_avrcp_connect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_connect_cnf_struct), TD_CTRL);
    cnf_param_ptr->chnl_num = avrcp_chnl_num;
    cnf_param_ptr->connect_id = msg_p->connect_id;
    kal_prompt_trace(1, "rdabt_avrcp_connect_cnf_cb msg_p->result=%d",msg_p->result);
    if(msg_p->result == RDABT_NOERROR)
        cnf_param_ptr->result = BT_AVRCP_RESULT_SUCCESS;
    else
        cnf_param_ptr->result = BT_AVRCP_RESULT_ERROR;
    avrcp_connect_id=msg_p->connect_id;

    cnf_param_ptr->device_addr.lap = BDADDR_Get_LAP(&msg_p->device_addr);
    cnf_param_ptr->device_addr.nap = BDADDR_Get_NAP(&msg_p->device_addr);
    cnf_param_ptr->device_addr.uap = BDADDR_Get_UAP(&msg_p->device_addr);
    
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_CONNECT_CNF, (void *)cnf_param_ptr, NULL);	
	avrcp_current_staust=0; 
}

void rdabt_avrcp_disconnect_ind_cb(void * pArgs)
    {
        t_avrcp_connect_inf *msg_p = (t_avrcp_connect_inf*)pArgs;
        bt_avrcp_disconnect_ind_struct *cnf_param_ptr;
        cnf_param_ptr = (bt_avrcp_disconnect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_disconnect_ind_struct), TD_CTRL);
        cnf_param_ptr->chnl_num = avrcp_chnl_num;
        cnf_param_ptr->connect_id = msg_p->connect_id;
    
        kal_prompt_trace(1, "rdabt_avrcp_disconnect_ind_cb chnl_num:%d, connect_id:%x",avrcp_chnl_num, msg_p->connect_id);
    
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_DISCONNECT_IND, (void *)cnf_param_ptr, NULL);   
		avrcp_current_staust=0; 
    }


void rdabt_avrcp_disconnect_cnf_cb(void * pArgs)
    {
        t_avrcp_connect_inf *msg = (t_avrcp_connect_inf *)pArgs;
        bt_avrcp_disconnect_cnf_struct *cnf_param_ptr;
    rdabt_avrcp_disconnect_ind_cb(pArgs);
    return;
	
	cnf_param_ptr = (bt_avrcp_disconnect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_disconnect_cnf_struct), TD_CTRL);
        cnf_param_ptr->chnl_num = avrcp_chnl_num;
        cnf_param_ptr->connect_id = msg->connect_id;
        if(msg->result == RDABT_NOERROR)
            cnf_param_ptr->result = BT_AVRCP_RESULT_SUCCESS;
        else
            cnf_param_ptr->result = BT_AVRCP_RESULT_ERROR;
    
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_DISCONNECT_CNF, (void *)cnf_param_ptr, NULL);    
		avrcp_current_staust=0; 
    }


void rdabt_avrcp_cmd_frame_cnf_cb(void * pArgs)
{
    bt_avrcp_cmd_frame_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_avrcp_cmd_frame_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_cmd_frame_cnf_struct), TD_CTRL);

    //TO DO LATER.....
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_CMD_FRAME_CNF, (void *)cnf_param_ptr, NULL);
}


void rdabt_avrcp_cmd_frame_ind_cb(void * pArgs)
{
        t_avrcp_cmd_frame_ind *msg_p = (t_avrcp_cmd_frame_ind*)pArgs;
        bt_avrcp_cmd_frame_ind_struct *cnf_param_ptr;
        kal_prompt_trace(1, "rdabt_avrcp_cmd_frame_ind_cb avrcp_current_staust=%d, data_len=%d, subunit_type=%x, subunit_id=%d, frame_data[0]=%x",avrcp_current_staust, msg_p->data_len, msg_p->subunit_type, msg_p->subunit_id, msg_p->frame_data[0]);
        if(avrcp_current_staust)
			return;//res not send ,ignore new cmd
	    cnf_param_ptr = (bt_avrcp_cmd_frame_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_cmd_frame_ind_struct), TD_CTRL);
        cnf_param_ptr->chnl_num = avrcp_chnl_num;
        cnf_param_ptr->c_type = msg_p->c_type;
        cnf_param_ptr->data_len  = msg_p->data_len;
        cnf_param_ptr->seq_id  = msg_p->seq_id;
        cnf_param_ptr->subunit_type  = msg_p->subunit_type;
        cnf_param_ptr->subunit_id  = msg_p->subunit_id;
        cnf_param_ptr->profile_id  = msg_p->profile_id;
        memcpy(cnf_param_ptr->frame_data, msg_p->frame_data, msg_p->data_len);
    
        //kal_prompt_trace(1, "rdabt_avrcp_cmd_frame_ind_cb chnl_num=%d,chnl_num=%d, profile_id=%x",msg_p->chnl_num, cnf_param_ptr->chnl_num, cnf_param_ptr->profile_id);
        
		avrcp_current_staust=1; 
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_CMD_FRAME_IND, (void *)cnf_param_ptr, NULL); 
    }

void rdabt_avrcp_send_activate_req(module_type src_mod_id,u_int8 chnl_num,u_int8 local_role )
{
    bt_avrcp_activate_cnf_struct *cnf_param_ptr;

    mmi_trace(1, "bt_avrcp_send_activate_req  local_role=%d, chnl_num=%d\n", local_role, chnl_num);
    avrcp_current_staust=0; 
   // Avrcp_Init();

    avrcp_chnl_num = chnl_num;
    avrcp_local_role = local_role;

    cnf_param_ptr = (bt_avrcp_activate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_activate_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = BT_AVRCP_RESULT_SUCCESS;
    cnf_param_ptr->chnl_num = chnl_num;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_ACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
}

// deactivate
void rdabt_avrcp_send_deactivate_req(module_type src_mod_id,u_int8 chnl_num )
{
    bt_avrcp_deactivate_cnf_struct *cnf_param_ptr;

    mmi_trace(1, "bt_avrcp_send_deactivate_req");

    //Avrcp_Set_State(AVRCP_INITIALISING);

    cnf_param_ptr = (bt_avrcp_deactivate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_avrcp_deactivate_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = BT_AVRCP_RESULT_SUCCESS;
    cnf_param_ptr->chnl_num = chnl_num ;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_AVRCP_DEACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
}
const struct
{
    kal_int16   type;
    void        (*handler)(void  *);
} avrcp_msg_hdlr_table[] =
{
//    {AVRCP_ACTIVATE_CNF,  rdabt_avrcp_activate_cb},
//    {AVRCP_DEACTIVATE_CNF,  rdabt_avrcp_deactivate_cb},
    {AVRCP_CONNECT_CNF,  rdabt_avrcp_connect_cnf_cb},
    {AVRCP_CONNECT_IND,  rdabt_avrcp_connect_ind_cb},
    {AVRCP_DISCONNECT_CNF,  rdabt_avrcp_disconnect_cnf_cb},
    {AVRCP_DISCONNECT_IND,  rdabt_avrcp_disconnect_ind_cb},
    {AVRCP_CMD_FRAME_CNF,  rdabt_avrcp_cmd_frame_cnf_cb},
    {AVRCP_CMD_FRAME_IND,  rdabt_avrcp_cmd_frame_ind_cb}
};

void rdabt_avrcp_int_msg_hdlr( kal_uint16 mi,void *mv )
{
    int I;
    int n = sizeof(avrcp_msg_hdlr_table)/sizeof(avrcp_msg_hdlr_table[0]);
    // kal_uint16 type = (mi & 0x00ff);
    for(I=0; I<n; I++)
    {
        if( mi==avrcp_msg_hdlr_table[I].type )
        {
            avrcp_msg_hdlr_table[I].handler( mv );
            break;
        }
    }

    //ASSERT( I<N ); // not supported by the avadp
}

// connect
void rdabt_avrcp_send_connect_req(module_type src_mod_id, u_int8 chnl_num, bt_avrcp_dev_addr_struct *device_addr, u_int8 local_role )
{
    t_bdaddr bdaddr;
kal_prompt_trace(1,"rdabt_avrcp_send_connect_req");
    //Avrcp_Set_Connect_Cfm_Callback(chnl_num, local_role);
    //avrcp_chnl_num=chnl_num;
    avrcp_local_role = local_role;
    BDADDR_Set_LAP_UAP_NAP(&bdaddr, device_addr->lap, device_addr->uap, device_addr->nap);
    Avrcp_Connect_Req(bdaddr);
}

// disconnect
void rdabt_avrcp_send_disconnect_req(module_type src_mod_id,u_int8 chnl_num)
{
    t_bdaddr bd_addr;
    BDADDR_Set_LAP_UAP_NAP(&bd_addr, a2dp_avrcp_remote_addr.lap, a2dp_avrcp_remote_addr.uap, a2dp_avrcp_remote_addr.nap);
    Avrcp_Disconnect_Req(bd_addr);
}
u_int8 audio_stop_timer=0;
extern u_int8 a2dp_snk_play;
#ifdef __SUPPORT_BT_A2DP_SNK__
extern u_int8 a2dp_snk_avrp_play_req;
#endif
void rdabt_audio_stop_timeout(void)
{

	kal_prompt_trace(1,"rdabt_audio_stop_timeout,a2dp_snk_play=%d",a2dp_snk_play);
audio_stop_timer=0;
	if(a2dp_snk_play)
	{
#ifdef __SUPPORT_BT_A2DP_SNK__			
		a2dp_snk_MCI_AudioStop();
#else
        MCI_AudioStop();
#endif
        a2dp_snk_play=0;
    }

}
void rdabt_avrcp_msg_hdlr(ilm_struct *message)
{

        switch(message->msg_id)
        {
            case MSG_ID_BT_AVRCP_ACTIVATE_REQ:
                {
                    bt_avrcp_activate_req_struct *msg_p = (bt_avrcp_activate_req_struct*)message->local_para_ptr;
                    rdabt_avrcp_send_activate_req(MOD_MMI, msg_p->local_role, msg_p->local_role);
                }
                break;
            case MSG_ID_BT_AVRCP_DEACTIVATE_REQ:
                {
                    bt_avrcp_deactivate_req_struct *msg_p = (bt_avrcp_deactivate_req_struct*)message->local_para_ptr;
                    rdabt_avrcp_send_deactivate_req(MOD_MMI, msg_p->chnl_num);
                }
                break;
                        //AVRCP msg
            case MSG_ID_BT_AVRCP_CONNECT_REQ:
            {
                bt_avrcp_connect_req_struct * msg_p = (bt_avrcp_connect_req_struct*)message->local_para_ptr;
                rdabt_avrcp_send_connect_req(MOD_MMI, msg_p->chnl_num, &(msg_p->device_addr), msg_p->local_role);
   		 a2dp_avrcp_remote_addr.lap=msg_p->device_addr.lap;
   		 a2dp_avrcp_remote_addr.uap=msg_p->device_addr.uap;
   		 a2dp_avrcp_remote_addr.nap=msg_p->device_addr.nap;
                break;
            }
            case MSG_ID_BT_AVRCP_DISCONNECT_REQ:
            {
                bt_avrcp_disconnect_req_struct * msg_p = (bt_avrcp_disconnect_req_struct*)message->local_para_ptr;
        		kal_prompt_trace(1, "MSG_ID_BT_AVRCP_DISCONNECT_REQ msg_p->chnl_num=%d",msg_p->chnl_num);
				rdabt_avrcp_send_disconnect_req(MOD_MMI,  msg_p->chnl_num);
                break;                
            }
#ifdef __SUPPORT_BT_A2DP_SNK__			
		case MSG_ID_BT_AVRCP_CMD_FRAME_REQ:
		{
			bt_a2dp_snk_avrcp_cmd_req_struct  *cmd=(bt_a2dp_snk_avrcp_cmd_req_struct*)message->local_para_ptr;
			kal_prompt_trace(1," process MSG_ID_BT_AVRCP_CMD_FRAME_REQ key_id=%x,status=%x,a2dp_snk_play=%x,audio_stop_timer=%x,a2dp_snk_avrp_play_req=%d",cmd->key_id,cmd->key_status,a2dp_snk_play,audio_stop_timer,a2dp_snk_avrp_play_req); 
			a2dp_snk_avrp_play_req=0;
			if((cmd->key_id==OPID_REWIND)||(cmd->key_id==OPID_FAST_FORWARD))
			{
				if(cmd->key_status==0)
				Avrcp_Send_Press(cmd->key_id);
				else
				Avrcp_Send_Release(cmd->key_id);
			}
			else
				{				      	
					if(cmd->key_id==OPID_PAUSE)
					{
						if(a2dp_snk_play&&(!audio_stop_timer))
						{
							Avrcp_Send_Key(cmd->key_id);
							audio_stop_timer=rdabt_timer_create(40,  rdabt_audio_stop_timeout,NULL,1);
						}
					}
					else
						{
							Avrcp_Send_Key(cmd->key_id);
							if(cmd->key_id==OPID_PLAY)
								a2dp_snk_avrp_play_req=1;
						}


            }
            break;

        }
#endif
        default:
            ;//  ASSERT(0);
            break;
            //end of AVRCP msg
    }


}
#endif


void rda_bt_avrcp_send_cmd_frame_ind_res(kal_uint8 chnl_num, kal_uint8 seq_id, kal_uint16 profile_id,kal_uint8 result, kal_uint8 subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8* frame_data )
{
    // u_int8 data[48];
  //  u_int16 packet_size =5+data_len;
    kal_prompt_trace(1, "rda_bt_avrcp_send_cmd_frame_ind_res ,avrcp_current_staust=%dframe_data[0~2]=%x %x %x",avrcp_current_staust,frame_data[0],frame_data[1],frame_data[2]);

   /* data[0] = (seq_id<<4) |(AVCTP0_PACKET_TYPE_SINGLE<<2) |(AVRCP_TYPE_RESPONSE<<1) | AVRCP_MESSAGE_VALID;
    data[1] = (u_int8)((profile_id >>8) &0xFF);
    data[2] = (u_int8)(profile_id  & 0xFF);
    data[3] = result;
    data[4] = (subunit_type<<3) |subunit_id;
    pMemcpy(&data[5], frame_data, data_len);
    
    Avrcp_Send_Response(data, packet_size, result);
    */
    
   avrcp_current_staust=0; 
	Avrcp_Send_Cmd_Response(    avrcp_connect_id,seq_id,profile_id, result, subunit_type, subunit_id,frame_data,data_len,0x7c);
}
/*APIDECL1 t_api APIDECL2 Avrcp_Send_Cmd_Response(u_int8 seq_id, u_int16 profile_id, u_int8 result, u_int8 subunit_type, u_int8 subunit_id, u_int8 frame_data)
{
    u_int8 data[48];
    u_int16 packet_size =5+3;

    data[0] = (seq_id<<4) |(AVCTP0_PACKET_TYPE_SINGLE<<2) |(AVRCP_TYPE_RESPONSE<<1) | AVRCP_MESSAGE_VALID;
    data[1] = (u_int8)((profile_id >>8) &0xFF);
    data[2] = (u_int8)(profile_id  & 0xFF);
    data[3] = result;
    data[4] = (subunit_type<<3) |subunit_id;

    data[5] = AVRCP2_PASSTHROUGH;
    data[6] = frame_data;
    data[7] = 0;

    Avrcp_Send_Response(data, packet_size, result);

    return RDABT_NOERROR;
}*/
u_int16 rda_cmd_avrcp=OPID_FORWARD;
void avrcp_send_cmd(void)
{
   kal_prompt_trace(1, "avrcp_send_cmd rda_cmd_avrcp=%x",rda_cmd_avrcp);
   Avrcp_Send_Key(rda_cmd_avrcp);


}
#ifdef __SUPPORT_BT_A2DP_SNK__
#include "sxr_sbx.h"
#include "rdabt_main.h"
void MCI_Audio_send_btstream_status(unsigned int msg_id, void *local_param_ptr, void *peer_buf_ptr)
{
    ilm_struct *msg = allocate_ilm(MOD_MED);
    msg->src_mod_id = MOD_MED;
    msg->dest_mod_id = (module_type)MOD_MMI;
    msg->sap_id = BT_APP_SAP;
    msg->msg_id = msg_id;
    msg->local_para_ptr = local_param_ptr;
    msg->peer_buff_ptr = peer_buf_ptr;
    rda_msg_send_ext_queue(msg);
}

void avrcp_send_btstream_status(UINT8 status)
{
    MCI_Audio_send_btstream_status(MSG_ID_BT_AVRCP_STATUS_IND, (void *)status, NULL);
}
#endif


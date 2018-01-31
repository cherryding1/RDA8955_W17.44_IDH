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



#if 1
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


#include "rdabt.h"
#include "bt.h"
#include "sxr_tim.h"


#include "rdabt_task.h"
#include "rdabt_profile.h"
#include "rdabt_link_prim.h"
#include "rdabt_hfp.h"
#include "mmi_trace.h"
#include "bt_hfg_struct.h"
#include "bt_hsg_struct.h"
#include "hfp.h"
#include "papi.h"
#include "hci.h"
#include "rdabt_drv.h"
#include "bluetooth_struct.h"
extern void HangupAllCalls(void);


static t_bdaddr current_device;
u_int16 current_cid;
static bt_dm_addr_struct current_addr;

static u_int8 spp_port;
static u_int8 current_hf_NREC=0;
static u_int8 connect_flag=0;// 1is connect by mmi,0 is headset initiall connect
static u_int8 dis_connect_flag=0;// 1is disconnect by mmi,0 is headset initiall disconnect
static u_int8  connect_ind_result=0;
static bt_dm_addr_struct current_addr_connect;
static void rdabt_hfg_connect_cnf_cb(u_int8 result);
static void rdabt_hfg_disconnect_cnf_cb(u_int8 result);

static void rdabt_hfg_connect_ind_cb(void * pArgs);
static void rdabt_hfg_connect_req_cb(void * pArgs);
static void rdabt_hfg_disconnect_ind_cb(void * pArgs);
static void rdabt_hfg_channel_connect_ind_cb(void * pArgs);
static void rdabt_hfg_response_ind_cb(void * pArgs);
static void rdabt_hfg_call_req_ind_cb(void * pArgs);
static void rdabt_hfg_call_wait_ind_cb(void * pArgs);
static void rdabt_hfg_voice_reconition_ind_cb(void * pArgs);
static void rdabt_hfg_speaker_gain_ind_cb(void * pArgs);
static void rdabt_hfg_mic_gain_ind_cb(void * pArgs);
static void rdabt_hsg_ckpd_from_hs_ind_cb(void * pArgs);
static void rdabt_hf_ring_ind(void);

static void rdabt_hf_call_status_ind(void * pArgs);

static void rdabt_hf_call_clcc_cnf(void * pArgs);
static void rdabt_hf_no_carrier_ind_cb(void * pArgs);

static  void rdabt_hfg_NREC_ind_cb(void * pArgs);
static  void rdabt_hfg_vts_dtmf_ind_cb(void * pArgs);
extern void MGR_SetVoiceSetting_Config(u_int16 config);// define in the bt lib,to set sco link use PCM or UART
void rdabt_hfp_clcc_ind(void);
u_int8 rdabt_currentType=0;
#ifdef __SUPPORT_BT_PHONE__
u_int16 BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;
static u_int8 rdabt_clcc_received = 0;
extern u_int8 current_voice_change_staus;;
extern u_int8 phonebook_step;
extern u_int8 history_step;
u_int8 btphone_auto_con_flag=0;
u_int8 btphone_auto_con_timer=0;
//void rdabt_auto_con_test2(void);
u_int8 btphone_current_profile=0;
#ifdef BTPHONE_RECOGNITION_CALL
u_int16 btphone_recognition_call=0;
void rdabt_hfp_end_Voice_Recognition(void);
void rdabt_hfp_Voice_Recognition(void);
#endif

#endif

extern u_int8 A2DP_local_role;

const struct
{
    kal_int16   type;
    void        (*handler)(void  *);
} hfp_msg_hdlr_table[] =

{
    {HFG_ACL_CONNECT_IND, rdabt_hfg_connect_ind_cb},
    {HFG_ACL_CONNECT_REQ,rdabt_hfg_connect_req_cb},
    {HFG_ACL_DISCONNECT_IND, rdabt_hfg_disconnect_ind_cb},
    {HFG_CKPD_FROM_HS_IND, rdabt_hsg_ckpd_from_hs_ind_cb},//not use
    {HFG_CALL_RESPONSE_IND, rdabt_hfg_response_ind_cb},
    {HFG_CALL_REQ_IND, rdabt_hfg_call_req_ind_cb},
    {HFG_CALL_WAIT_3WAY_IND, rdabt_hfg_call_wait_ind_cb},
    {HFG_VOICE_RECOGNITION_IND, rdabt_hfg_voice_reconition_ind_cb},
    {HFG_SPEAKER_GAIN_IND, rdabt_hfg_speaker_gain_ind_cb},
    {HFG_MIC_GAIN_IND, rdabt_hfg_mic_gain_ind_cb},
    {HFG_VTS_DTMF_IND, rdabt_hfg_vts_dtmf_ind_cb},
    {HFG_NREC_VALUE_IND, rdabt_hfg_NREC_ind_cb},
    {HFG_CLCC_IND,rdabt_hfp_clcc_ind},
#ifdef __SUPPORT_BT_PHONE__
    ,
    {HFG_RING_IND,rdabt_hf_ring_ind},
    {HFG_CALL_STATUS_IND,rdabt_hf_call_status_ind},
    {HFG_CALL_CLCC_CNF,rdabt_hf_call_clcc_cnf},
    {HFG_NO_CARRIER_IND,rdabt_hf_no_carrier_ind_cb}

#endif
};

#ifdef __SUPPORT_BT_PHONE__

void rdabt_hfp_getClcc_delay(void)
{
    kal_prompt_trace(1, "rdabt_hfp_getClcc_delay rdabt_clcc_received %d ,state? %d",rdabt_clcc_received,BtPhone_CurrentCallStatus==BT_CALL_STATUS_NONE);
    if(rdabt_clcc_received||BtPhone_CurrentCallStatus==BT_CALL_STATUS_NONE) return;
    HF_Get_Clcc();
}
#endif

#ifdef __SUPPORT_BT_A2DP_SNK__
void rdabt_delay_con_a2dp_timeout(void)
{
    kal_prompt_trace(1, "rdabt_delay_con_a2dp_timeout");
#ifdef __SUPPORT_BT_A2DP_SNK__
    extern uint8 mmi_btphone_a2dp_con_flag(void);
    if(mmi_btphone_a2dp_con_flag())
        return;
#endif
    AVDTP_Setup_Connect(current_device);
    A2DP_local_role=BT_A2DP_SINK;
}
#endif

void rdabt_hfg_connect_ind_cb(void * pArgs)
{
    struct st_t_dataBuf *cmd = NULL;
    t_hfp_connect_ind *msg_p =(t_hfp_connect_ind *)pArgs;
    bt_hfg_connect_ind_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_hfg_connect_ind_cb msg_p->result=%d,connect_flag=%d,msg_p->profile=%x",
                     msg_p->result,connect_flag,msg_p->profile);
    if(msg_p->result!=RDABT_NOERROR)
    {
        rdabt_hfg_connect_cnf_cb(msg_p->result);
#ifdef __SUPPORT_BT_PHONE__
        if(btphone_auto_con_flag)
            btphone_auto_con_flag=0;
#endif
        return;

    }
    /*{
                struct st_t_dataBuf *cmd = NULL;
              //  APIDECL1 t_api APIDECL2 HCI_SwitchRole(struct st_t_dataBuf **cmd, t_bdaddr bdaddr, u_int8 role);

        HCI_SwitchRole(&cmd, msg_p->bt_addr, 0x01);

            HCI_SendRawPDU(HCI_PRIMARY_TRANSPORT, HCI_pduCOMMAND, cmd);
        }*/
    I2C_Open();
    cnf_param_ptr = (bt_hfg_connect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_connect_ind_struct), TD_CTRL);
    current_device = msg_p->bt_addr;
    current_cid=msg_p->connection_id;
    connect_ind_result=msg_p->result;
    current_addr_connect .lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    current_addr_connect .uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    current_addr_connect .nap = BDADDR_Get_NAP(&msg_p->bt_addr);
    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->bt_addr.lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);
#ifdef __SUPPORT_BT_PHONE__
    cnf_param_ptr->profile=msg_p->profile;
#endif
    if(connect_flag)
    {
        rdabt_send_msg_up(MOD_MMI, msg_p->profile==ACTIVE_PROFILE_HANDSFREE_AG?MSG_ID_BT_HFG_CONNECT_IND:MSG_ID_BT_HSG_CONNECT_IND, (void *)cnf_param_ptr, NULL);
    }
    else
    {
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_CONNECT_IND, (void *)cnf_param_ptr, NULL);
    }
    HF_Set_MemorySize(100,10);
    rdabt_currentType=msg_p->profile;

#ifdef __SUPPORT_BT_PHONE__
    if(ACTIVE_PROFILE_HANDSFREE==msg_p->profile)//BT PHONE ,sco via UART
    {
#ifdef USE_VOC_CVSD
        rdabt_audio_setting(1,1);
#else
        rdabt_audio_setting(1,0);
#endif
        if( RDABT_NOERROR == HCI_WriteVoiceSetting(&cmd, 0xc060))
            HCI_SendRawPDU(HCI_PRIMARY_TRANSPORT, HCI_pduCOMMAND, cmd);
        {
            u_int16 call_status=0;
            current_voice_change_staus=0;
            HF_Get_CallStatus(&call_status);
            kal_prompt_trace(1, "HF_Get_CallStatus return %x",call_status);
            if(call_status==1)
            {
                bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
                BtPhone_CurrentCallStatus=BT_CALL_STATUS_ACTIVE;
                current_voice_change_staus=0;
                rdabt_timer_create(5,  rdabt_hfp_getClcc_delay,NULL,pTIMER_ONESHOT);
                HF_Get_Clcc();
                call_info->call_status=2;
                call_info->call_number_length=0;
                call_info->call_director=2;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
            }

#ifdef USE_VOC_CVSD
            MGR_SetVoiceSetting_Config(0xc040);
#else
            MGR_SetVoiceSetting_Config(0x0060);
#endif





        }
        if(btphone_auto_con_flag)
        {
            btphone_auto_con_flag=0;
#ifdef __SUPPORT_BT_A2DP_SNK__
            rdabt_timer_create(60,  rdabt_delay_con_a2dp_timeout,NULL,pTIMER_ONESHOT);
#endif

        }
    }
    else//PHONE ,sco via pcm
#endif
    {
        rdabt_audio_setting(0,0);

        if( RDABT_NOERROR == HCI_WriteVoiceSetting(&cmd, 0x0060))
            HCI_SendRawPDU(HCI_PRIMARY_TRANSPORT, HCI_pduCOMMAND, cmd);
        MGR_SetVoiceSetting_Config(0x0060);
    }
    I2C_Close();

#ifdef __BT_PROFILE_BQB__
    {
        extern U8 rdabt_get_CPOS_info(char * content);
        u_int8 cpos_info[21]= {0};
        rdabt_get_CPOS_info(cpos_info);
        HF_Set_NetworkOperator(cpos_info);
    }
#endif
}


typedef int (*p_rda_GetCurrentCallStatus)();                   //get current call status
p_rda_GetCurrentCallStatus rda_GetCurrentCallStatus = NULL;
void rda_setCurrentCallStatusCB(void * func)
{
    rda_GetCurrentCallStatus = (p_rda_GetCurrentCallStatus)func;
}

typedef int (*p_rda_GetCallHoldCount)();                       //get hold call count num
p_rda_GetCallHoldCount rda_GetCallHoldCount = NULL;
void rda_setCallHoldCountCB(void * func)
{
    rda_GetCallHoldCount = (p_rda_GetCallHoldCount)func;
}

typedef int (*p_rda_GetSignalStrength)();                     //get signalStrength
p_rda_GetSignalStrength rda_GetSignalStrength = NULL;
void rda_SetSignalStrengthCB(void * func)
{
    rda_GetSignalStrength = (p_rda_GetSignalStrength)func;
}
#include "manager.h"
typedef int (*p_rda_GetRoamStatus)();                       //get roaming status
p_rda_GetRoamStatus rda_GetRoamStatus = NULL;
void rda_SetRoamStatusCB(void * func)
{
    rda_GetRoamStatus = (p_rda_GetRoamStatus)func;
}
void rdabt_hfg_connect_req_cb(void * pArgs)
{
    extern MMI_BOOL mmi_bt_is_hs_connected(void);
    t_hfp_connect_ind *msg_p =(t_hfp_connect_ind *)pArgs;
    current_hf_NREC=0;
    if(mmi_bt_is_hs_connected())
        HF_Reject_SLC(msg_p->bt_addr);
    else
    {
        u_int8 status_array[7]= {1,0,0,0,5,5,0};
        //\"service\",(0,1)),(\"call\",(0,1)),(\"callsetup\",(0,3)),(\"callheld\",(0,2)),(\"battchg\",(0,5)),(\"signal\",(0,5)),(\"roam\",(0,1))\r\n";
        current_device = msg_p->bt_addr;

        if( rdabt_get_device_authriose_info(msg_p->bt_addr))
        {
            if(rda_GetCurrentCallStatus())
                status_array[1]=1;
            status_array[3]=rda_GetCallHoldCount();
            status_array[5]=rda_GetSignalStrength();
            status_array[6]=rda_GetRoamStatus();
            kal_prompt_trace(1, "hfp_set_indicator_status call %d,hold %d,sig %d,rost %d", status_array[1], status_array[3], status_array[5], status_array[6]);
            hfp_set_indicator_status(status_array);
            HF_Accept_SLC( status_array[1],msg_p->bt_addr);
        }
        else
        {
            goep_authorize_ind_struct *param_ptr ;
            t_MGR_DeviceEntry *device;
            param_ptr = (goep_authorize_ind_struct*) construct_local_para(sizeof(goep_authorize_ind_struct), TD_CTRL);
            param_ptr->uuid[0] = 0;
            param_ptr->uuid_len = 0;
            param_ptr->goep_conn_id =0;

            param_ptr->bd_addr.lap= BDADDR_Get_LAP(&msg_p->bt_addr);
            param_ptr->bd_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
            param_ptr->bd_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);
            memset((char*)param_ptr->dev_name , 0 , 80);
            MGR_FindDeviceRecord((msg_p->bt_addr), &device);
            if( 0 != device )
                memcpy((char*)param_ptr->dev_name , (const char*)(device->name) , 80);
            rdabt_send_msg_up(MOD_MMI, MSG_ID_HFP_CON_AUTHORIZE_IND, (void *)param_ptr, NULL);
        }
    }
}
void rdabt_hfg_disconnect_cnf_cb(u_int8 result)
{
    bt_hsg_disconnect_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_hsg_disconnect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_disconnect_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = result;
    cnf_param_ptr ->connection_id = current_cid;
    cnf_param_ptr ->bt_addr = current_addr;
    kal_prompt_trace(1, "rdabt_hfg_disconnect_cnf_cb msg_p->connection_id:%x",current_cid);
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_DISCONNECT_CNF, (void *)cnf_param_ptr, NULL);
    current_hf_NREC=0;

}

void rdabt_hfg_disconnect_ind_cb(void * pArgs)
{
    t_hfp_connect_ind *msg_p =(t_hfp_connect_ind *)pArgs;
    bt_hfg_disconnect_ind_struct *cnf_param_ptr;
    current_hf_NREC=0;

    kal_prompt_trace(1, "rdabt_hfg_disconnect_ind_cb connect_flag=%d,result=%x,profile=%x",connect_flag,msg_p->result,msg_p->profile);
    connect_flag=FALSE;
    cnf_param_ptr = (bt_hfg_disconnect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_disconnect_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->bt_addr.lap = BDADDR_Get_LAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.uap = BDADDR_Get_UAP(&msg_p->bt_addr);
    cnf_param_ptr ->bt_addr.nap = BDADDR_Get_NAP(&msg_p->bt_addr);
#ifdef __SUPPORT_BT_PHONE__
    cnf_param_ptr->profile=msg_p->profile;
    cnf_param_ptr->result= msg_p->result;
#endif
    rdabt_currentType=0;
    rdabt_send_msg_up(MOD_MMI,MSG_ID_BT_HFG_DISCONNECT_IND, (void *)cnf_param_ptr, NULL);
#ifdef __SUPPORT_BT_PHONE__
    {
        void rdabt_btphone_reset();
        rdabt_btphone_reset();
    }
#endif

}









void rdabt_hsg_ckpd_from_hs_ind_cb(void * pArgs)
{
    //mmi_ucm_headset_key();
    // mmi_ucm_app_headset_key();
    kal_prompt_trace(1, "rdabt_hsg_ckpd_from_hs_ind_cb ");
}
static void rdabt_hfg_response_ind_cb(void * pArgs)
{
    t_hfp_call_ind *msg_p = (t_hfp_call_ind*)pArgs;
    kal_prompt_trace(1, "rdabt_hfg_call_req_ind_cb receive HF_CALL_REJECT msg_p->type =0x%x",msg_p->type );

    if(msg_p->type == HF_CALL_ACCEPT)
    {
        bt_hfg_incoming_accept_req_struct *cnf_param_ptr;
        cnf_param_ptr = (bt_hfg_incoming_accept_req_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_incoming_accept_req_struct), TD_CTRL);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_INCOMING_ACCEPT_REQ, (void *)cnf_param_ptr, NULL);

    }
    else if(msg_p->type == HF_CALL_REJECT)
    {
        bt_hfg_incoming_reject_req_struct *cnf_param_ptr;

        cnf_param_ptr = (bt_hfg_incoming_reject_req_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_incoming_reject_req_struct), TD_CTRL);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_INCOMING_REJECT_REQ, (void *)cnf_param_ptr, NULL);

        // will send ciev
    }
}
extern u_int8 current_sco_connected;

static void rdabt_hfg_call_req_ind_cb(void * pArgs)
{
    t_hfp_call_ind *msg_p = (t_hfp_call_ind*)pArgs;

    if(msg_p->type == HF_CALL_RELEASE)
    {
        bt_hfg_hangup_req_struct *cnf_param_ptr;

        kal_prompt_trace(1, "rdabt_hfg_call_req_ind_cb receive HF_CALL_RELEASE,current_sco_connected=%d",current_sco_connected);
        cnf_param_ptr = (bt_hfg_hangup_req_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_hangup_req_struct), TD_CTRL);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_HANGUP_REQ, (void *)cnf_param_ptr, NULL);
        // if(!current_sco_connected)
        //           HF_Call_Request(HF_CALL_RELEASE,1);


    }

    else if(msg_p->type == HF_CALL_REDIAL)
    {
        bt_hfg_redial_lastnum_req_struct *cnf_param_ptr;
        //rdabt_read_log();
        kal_prompt_trace(1, "rdabt_hfg_call_req_ind_cb receive HF_CALL_REDIAL");
        cnf_param_ptr = (bt_hfg_redial_lastnum_req_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_redial_lastnum_req_struct), TD_CTRL);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_REDIAL_LASTNUMB_REQ, (void *)cnf_param_ptr, NULL);
    }
    else if(msg_p->type == HF_CALL_NUM)
    {
        u_int8 number[30];
        u_int8 length = 0;
        bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
        HF_Get_Number(number, &length);
        kal_prompt_trace(1, "rdabt_hfg_call_req_ind_cb,will send MSG_ID_HFP_NUMBER_CALL_IND,phone_num=%s",number);
        call_info->call_number_length=length;
        memcpy(call_info->call_number,number,length+1);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_HFP_NUMBER_CALL_IND, (void *)call_info, NULL);
    }
    else if(msg_p->type == HF_CALL_MEM)
    {
        u_int8 number[30];
        u_int8 length = 0;
        HF_Get_Number(number, &length);
        kal_prompt_trace(1, "rdabt_hfg_call_req_ind_cb,will send MSG_ID_HFP_NUMBER_CALL_IND,length=%d,phone_num=%s",length,number);
        bt_hfg_call_ind_struct *cnf_param_ptr;
        cnf_param_ptr = (bt_hfg_call_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_call_ind_struct), TD_CTRL);
        cnf_param_ptr ->type= 2;
        cnf_param_ptr ->nm_length= length;
        memcpy(cnf_param_ptr ->num_Phone,number, length);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_ATD_CALL_IND, (void *)cnf_param_ptr, NULL);
    }

}
static void rdabt_hfg_call_wait_ind_cb(void * pArgs)
{
    t_hfp_call_ind *msg_p = (t_hfp_call_ind*)pArgs;

    char *cnf_param_ptr;

    cnf_param_ptr = (char *)construct_local_para( (kal_uint16)sizeof(char), TD_CTRL);

    *cnf_param_ptr = msg_p->type+'0';
    kal_prompt_trace(1, "rdabt_hfg_call_wait_ind_cb  msg_p->type %x %x", msg_p->type,*cnf_param_ptr );

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_ACTIVE_CALL_CHLD_IND, (void *)cnf_param_ptr, NULL);
}

void rdabt_hfg_voice_reconition_ind_cb(void * pArgs)
{

}

void rdabt_hfg_speaker_gain_ind_cb(void * pArgs)
{
    t_hfp_gain_ind *msg_p =(t_hfp_gain_ind *)pArgs;
    bt_hfg_speaker_gain_ind_struct *cnf_param_ptr;

    kal_prompt_trace(1, "rdabt_hfg_speaker_gain_ind_cb" ,msg_p->profile, msg_p->connection_id, msg_p->gain);

    cnf_param_ptr = (bt_hfg_speaker_gain_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_speaker_gain_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->gain = msg_p->gain;

    rdabt_send_msg_up(MOD_MMI, /*msg_p->profile==ACTIVE_PROFILE_HANDSFREE_AG?*/MSG_ID_BT_HFG_SPEAKER_GAIN_IND/*:MSG_ID_BT_HSG_SPEAKER_GAIN_IND*/, (void *)cnf_param_ptr, NULL);
}

void rdabt_hfg_mic_gain_ind_cb(void * pArgs)
{
    t_hfp_gain_ind *msg_p =(t_hfp_gain_ind *)pArgs;
    bt_hfg_mic_gain_ind_struct *cnf_param_ptr;

    kal_prompt_trace(1, "rdabt_hfg_mic_gain_ind_cb,%x %x %x", msg_p->profile, msg_p->connection_id, msg_p->gain);

    cnf_param_ptr = (bt_hfg_mic_gain_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_mic_gain_ind_struct), TD_CTRL);

    cnf_param_ptr ->connection_id = msg_p->connection_id;
    cnf_param_ptr ->gain = msg_p->gain;

    rdabt_send_msg_up(MOD_MMI, /*msg_p->profile==ACTIVE_PROFILE_HANDSFREE_AG?*/MSG_ID_BT_HFG_MIC_GAIN_IND/*:MSG_ID_BT_HSG_MIC_GAIN_IND*/, (void *)cnf_param_ptr, NULL);
}

void rdabt_hfg_vts_dtmf_ind_cb(void * pArgs)
{
    U16 *msg_p;

    msg_p = (u_int16 *)construct_local_para( (u_int16)sizeof(u_int16), TD_CTRL);
    *msg_p = pArgs;
    mmi_trace(1,"rdabt_hfg_vts_dtmf_ind_cb");
    kal_prompt_trace(1, "rdabt_hfg_vts_dtmf_ind_cb *msg_p =%d",*msg_p );
    if( *msg_p== '#')
        *msg_p = 20;
    else if( *msg_p== '*')
        *msg_p = 19;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_VTS_DTMF_IND, (void *)msg_p, NULL);
}

void rdabt_hfg_NREC_ind_cb(void * pArgs)
{
    kal_prompt_trace(1,"rdabt_hfg_NREC_ind_cb msg_p->gain =%x",pArgs);
    current_hf_NREC=pArgs;

}
u_int8 rdabt_get_NREC_val(void)
{
    //kal_prompt_trace(1,"rdabt_get_NREC_val current_hf_NREC=%d",current_hf_NREC);
    return current_hf_NREC;

}

#ifdef __SUPPORT_BT_PHONE__
u_int8 BtPhone_ring_num=0;
u_int8 BtPhone_CallDirector=0;// 1 incoming,2 outgoing
kal_uint8 btphone_ATcommand_timer=0;
void rdabt_btphone_atcommand_timeout(void)
{
    kal_prompt_trace(1,"rdabt_btphone_atcommand_timeout,BtPhone_CallDirector=%d",BtPhone_CallDirector);
#if 1
    bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
    HF_Get_Number(call_info->call_number,&call_info->call_number_length);
    call_info->call_status=BT_CALL_STATUS_NONE;
    call_info->call_director=BtPhone_CallDirector;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
    BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;
#else
    bt_phone_call_info_struct call_info;

    HF_Get_Number(call_info.call_number,&call_info.call_number_length);
    call_info.call_status=BT_CALL_STATUS_NONE;
    call_info.call_director=BtPhone_CallDirector;
    mmi_bt_phone_call_info_ind_hdler((void*)(&call_info));
    BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;

#endif
}

void rdabt_hf_ring_ind(void)
{
    u_int8 number[30];
    u_int8 length = 0;
    BtPhone_CallDirector=1;
    return;
    kal_prompt_trace(1, "rrrrrrdabt_hf_ring_ind,,BtPhone_ring_num=%d",BtPhone_ring_num);
    rdabt_a2dp_pause_by_hfp();
    if(!BtPhone_ring_num)// first ring ,maybe +clip not arrive still
    {
        BtPhone_ring_num=1;
        return;

    }
    else
    {
        //BtPhone_ring_num=0;
        bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
        HF_Get_Number(number, &length);
        kal_prompt_trace(1, "rdabt_hf_ring_ind,,phone_num=%s",number);
        call_info->call_director=1;
        call_info->call_status=1;
        call_info->call_number_length=length;
        memcpy(call_info->call_number,number,length+1);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
    }
}
void rdabt_hf_call_status_ind(void * pArgs)
{
    u_int32 ind_values=pArgs;
    bt_phone_call_info_struct  call_info_tmp;

    kal_prompt_trace(1, "rdabt_hf_call_status_ind,ind_values=%x,BtPhone_CurrentCallStatus=%x,BtPhone_ring_num=%d",ind_values,BtPhone_CurrentCallStatus,BtPhone_ring_num);

    rdabt_a2dp_pause_by_hfp();
    sxr_StopFunctionTimer(rdabt_btphone_atcommand_timeout);
    if((ind_values==BT_CALL_STATUS_OUTGOING))//||(ind_values==BT_CALL_STATUS_ALERT))//out going call setup// add BT_CALL_STATUS_ALERT for iphone unknow number
    {
        if((    BtPhone_CurrentCallStatus==BT_CALL_STATUS_OUTGOING)||(  BtPhone_CurrentCallStatus==BT_CALL_STATUS_ACTIVE))//, call is canceled
        {
            bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
            HF_Get_Number(call_info->call_number,&call_info->call_number_length);
            call_info->call_status=BT_CALL_STATUS_NONE;
            call_info->call_director=BtPhone_CallDirector;
            BtPhone_ring_num=0;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
        }
        else
        {
            HF_Get_Number(call_info_tmp.call_number,&call_info_tmp.call_number_length);

            kal_prompt_trace(1, "rdabt_hf_call_status_ind ?phone_num=%s,call_info->call_number_length=%d",call_info_tmp.call_number,call_info_tmp.call_number_length);
            BtPhone_CallDirector = 2;//set outgoing state
            if(call_info_tmp.call_number_length == 0)
            {
                HF_Get_Clcc();
            }
            //  else
            {
                bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
                HF_Get_Number(call_info->call_number,&call_info->call_number_length);
                call_info->call_status=BT_CALL_STATUS_ACTIVE;
                current_voice_change_staus=0;
                call_info->call_director=BtPhone_CallDirector;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
            }
            BtPhone_CurrentCallStatus=BT_CALL_STATUS_OUTGOING;
        }
    }
    else if((ind_values==BT_CALL_STATUS_NONE)||((ind_values&0x0001)==BT_CALL_STATUS_ACTIVE))//call is active or no call active
    {
        if(((ind_values&0x0001)==BT_CALL_STATUS_ACTIVE)&&(BtPhone_CurrentCallStatus!=BT_CALL_STATUS_ACTIVE))//call is active
        {
            bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
            call_info->call_director=BtPhone_CallDirector;
            call_info->call_status=2;
            BtPhone_CurrentCallStatus=BT_CALL_STATUS_ACTIVE;
            current_voice_change_staus=0;
            HF_Get_Number(call_info->call_number,&call_info->call_number_length);
            kal_prompt_trace(1, "rdabt_hf_call_status_ind ACTIVE OR NOT,phone_num=%s,call_info->call_number_length=%d,BtPhone_ring_num=%d",call_info->call_number,call_info->call_number_length,BtPhone_ring_num);
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
        }
        else if(ind_values==BT_CALL_STATUS_NONE)
        {
            bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
            call_info->call_director=BtPhone_CallDirector;
            call_info->call_status=0;
            BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;
            BtPhone_ring_num=0;
            HF_Get_Number(call_info->call_number,&call_info->call_number_length);
            kal_prompt_trace(1, "rdabt_hf_call_status_ind ACTIVE OR NOT,phone_num=%s,call_info->call_number_length=%d,BtPhone_ring_num=%d",call_info->call_number,call_info->call_number_length,BtPhone_ring_num);
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
        }
        else if(ind_values==BT_CALL_STATUS_ALERT)
        {
            HF_Get_Number(call_info_tmp.call_number,&call_info_tmp.call_number_length);

            kal_prompt_trace(1, "rdabt_hf_call_status_ind BT_CALL_STATUS_ALERT phone_num=%s,call_info->call_number_length=%d",call_info_tmp.call_number,call_info_tmp.call_number_length);
            BtPhone_CallDirector = 2;//set outgoing state
            if(call_info_tmp.call_number_length == 0)
            {
                HF_Get_Clcc();
            }
            {
                bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
                HF_Get_Number(call_info->call_number,&call_info->call_number_length);
                call_info->call_status=BT_CALL_STATUS_ACTIVE;
                call_info->call_director=BtPhone_CallDirector;
                current_voice_change_staus=0;
                rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
            }
            BtPhone_CurrentCallStatus=BT_CALL_STATUS_OUTGOING;

        }
        else
            return;

    }

    if(ind_values==BT_CALL_STATUS_INCOMING)//incoming call setup
    {
        // BtPhone_ring_num=1;
        //       rdabt_hf_ring_ind();
        BtPhone_CurrentCallStatus=BT_CALL_STATUS_INCOMING;

    }
    //BtPhone_CurrentCallStatus=BT_CALL_STATUS_INCOMING;
}
void rdabt_hf_call_clcc_cnf(void * pArgs)//clip or clcc
{
    u_int32 result=pArgs;
    rdabt_clcc_received = 1;
    kal_prompt_trace(1, "rdabt_hf_call_clcc_cnf,result=%x,BtPhone_CurrentCallStatus=%d",result,BtPhone_CurrentCallStatus);
    if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_RELEASING)
    {
        HF_Terminate_Call();
        return;
    }

    if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_NONE)
        return;
    else
    {
        bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);

        if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_ACTIVE)
        {
            call_info->call_status=2;
            call_info->call_director=2;
            BtPhone_CallDirector=2;
        }
        else if(BtPhone_CurrentCallStatus==BT_CALL_STATUS_INCOMING)
        {

            call_info->call_status=1;
            call_info->call_director=1;
            BtPhone_CallDirector=1;
        }
        else
        {

            call_info->call_status=1;
            call_info->call_director=2;
            BtPhone_CallDirector=2;
        }

        HF_Get_Number(call_info->call_number,&call_info->call_number_length);
        kal_prompt_trace(1, ",length=%d,phone_num=%s",call_info->call_number_length,call_info->call_number);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
    }
}

void rdabt_btphone_reset()
{
    bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
    call_info->call_status=0;
    call_info->call_number_length=0;
    BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;
    BtPhone_ring_num=0;
    rdabt_clcc_received = 0;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);

}
void rdabt_hf_no_carrier_ind_cb(void * pArgs)
{
    kal_prompt_trace(1, "rdabt_hf_no_carrier_ind_cb");
    bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
    call_info->call_status=0;
    BtPhone_CurrentCallStatus=BT_CALL_STATUS_NONE;
    BtPhone_ring_num=0;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);

}
void rdabt_check_send_call_active(void)
{
    u_int8 number[30];
    u_int8 length = 0;

    if((BtPhone_CurrentCallStatus!=BT_CALL_STATUS_OUTGOING))
        return;
    HF_Get_Number(number, &length);
    //kal_prompt_trace(1, "rdabt_check number=%s,l=%d,BtPhone_CurrentCallStatus=%d",number,length,BtPhone_CurrentCallStatus);
    if((number[0]=='1')&&(number[1]=='1')&&(number[2]=='2'))
    {
        bt_phone_call_info_struct *call_info=(bt_phone_call_info_struct *)construct_local_para( (kal_uint16)sizeof(bt_phone_call_info_struct), TD_CTRL);
        HF_Get_Number(call_info->call_number,&call_info->call_number_length);
        call_info->call_status=2;
        call_info->call_director=BtPhone_CallDirector;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_PHONE_CALL_INFO_IND, (void *)call_info, NULL);
        BtPhone_CurrentCallStatus=BT_CALL_STATUS_ACTIVE;
    }
}

#endif
void rdabt_hfp_int_msg_hdlr( kal_uint16 mi,void *mv )
{
    int I;
    int n = sizeof(hfp_msg_hdlr_table)/sizeof(hfp_msg_hdlr_table[0]);
    for(I=0; I<n; I++)
    {

        //kal_prompt_trace(1, "rdabt_hfp_int_msg_hdlr type=0x%x,mgr_msg_hdlr_table[I].type =0x%x",mi,hfp_msg_hdlr_table[I].type );
        if( mi==hfp_msg_hdlr_table[I].type )
        {
            hfp_msg_hdlr_table[I].handler( mv );
            break;
        }
    }
}



extern scheduler_identifier pin_code_ind_timer;
void rdabt_hfg_connect_cnf_cb(u_int8 result)
{
    bt_hfg_connect_cnf_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_hfg_connect_cnf_cb,result=%x",result);

    cnf_param_ptr = (bt_hfg_connect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hfg_connect_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = result;
    cnf_param_ptr ->connection_id = current_cid;
    cnf_param_ptr ->bt_addr= current_addr;
#ifdef __SUPPORT_BT_PHONE__
    if(btphone_auto_con_flag)
        btphone_auto_con_flag=0;
    cnf_param_ptr ->profile=btphone_current_profile;
#endif
    kal_prompt_trace(1, "rdabt_hfg_connect_cnf_cb,result=%d",result);
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_HFG_CONNECT_CNF, (void *)cnf_param_ptr, NULL);

}

void rda_bt_hfg_start_call_setup_1_handler(void)
{
    kal_prompt_trace(1, "rda_bt_hfg_start_call_setup_1_handler");

//outgoing call send 3233
    HF_Call_Request(HF_CALL_OUTGOING, 1);
    HF_Call_Request(HF_CALL_ALERT, 1);

}

void rda_bt_hfg_start_call_notify(u_int32 IsIncoming)
{
    kal_prompt_trace(1, "rda_bt_hfg_start_call_notify");

//outgoing call send 3233
    if(IsIncoming == 0)
    {
        //HF_Call_Request(HF_CALL_OUTGOING, 1);
        //HF_Call_Request(HF_CALL_ALERT, 1);
    }
    else //incoming call
    {

    }

    HF_Call_Request(HF_CALL_START,1);

    /*   t_HFP_AG_Peer_Device* peer_device=     &HFP_AG_Device[0];
    kal_prompt_trace(1,"rda_bt_hfg_start_call_notify  Bldn_Present_Flag %d,g_call_answered_by_hf %d",Bldn_Present_Flag,g_call_answered_by_hf);
       if (peer_device->ring_timer)//syzhou added
       {

           pTimerCancel(peer_device->ring_timer);
    peer_device->ring_timer=0;
       }
           if(Bldn_Present_Flag)
           {
               // send +CIEV:3,3
               // send +CIEV:4,3
               HF_Send_Indicators(HFP_CALLSETUP_INDICATOR,3,&peer_device->address,peer_device->dlci);
               Bldn_Present_Flag=0;
           }//syzhou added ,only process the AT+BLDN command need send abrove
           if(!g_call_answered_by_hf)
           {
               // Send +CIEV:(call=1)
               HF_Send_Indicators(HFP_CALL_INDICATOR, 1, &peer_device->address, peer_device->dlci);

               // Send +CIEV:(callsetup=0)
               HF_Send_Indicators(HFP_CALLSETUP_INDICATOR,0,&peer_device->address,peer_device->dlci);
           }
           else
               g_call_answered_by_hf = FALSE;
               */
}
#include "hci.h"
void rda_bt_hfg_end_call_notify(void)
{
    //struct st_t_dataBuf *cmd = NULL;


    kal_prompt_trace(1, "rda_bt_hfg_end_call_notify");
    HF_Call_Request(HF_CALL_RELEASE,1);
    //if( RDABT_NOERROR == HCI_WriteLinkPolicySettings(&cmd, current_cid, 0x0f))
    //  HCI_SendRawPDU(HCI_PRIMARY_TRANSPORT, HCI_pduCOMMAND, cmd);
    /*  kal_prompt_trace(1,"rda_bt_hfg_end_call_notify  ");
          t_HFP_AG_Peer_Device* peer_device=   &HFP_AG_Device[0];

              HF_Send_Indicators(HFP_CALL_INDICATOR,0,&peer_device->address,peer_device->dlci);//syzhou added
                                 HCI_WriteLinkPolicySettings((HFP_AG_Device[0]).acl_handle, 0x0f);//zhou siyou 20120828
                                 */
}






//extern t_SDP_service_Parse_Results uuid_finded;
#ifdef __SUPPORT_SPP_SYNC__
#include "sdp.h"
#endif
u_int8 CS_timer=0;
void rdabt_delay_SendCallSetup_process()
{
    BT_TraceOutputText(1,"rdabt_delay_SendCallSetup_process process");

    if(CS_timer)
        rdabt_cancel_timer(CS_timer);
    CS_timer=0;
    HF_Call_Status(HF_CALLSETUP_INDICATOR,0);
}
void rdabt_hfg_msg_hdler(ilm_struct *message)
{
    u_int8 profile = ACTIVE_PROFILE_HANDSFREE_AG;
    t_api result = RDABT_NOERROR;

    switch(message->msg_id)
    {
        case MSG_ID_BT_HSG_ACTIVATE_REQ:
            profile = ACTIVE_PROFILE_HEADSET_AG;
        case MSG_ID_BT_HFG_ACTIVATE_REQ:
        {
            kal_uint8 result;
#ifdef __SUPPORT_BT_PHONE__
            if(profile == ACTIVE_PROFILE_HANDSFREE_AG)
                result=  HF_Activate(profile|ACTIVE_PROFILE_HANDSFREE);
            else
                result=  HF_Activate(profile|ACTIVE_PROFILE_HEADSET);
#else
            result=  HF_Activate(profile);
#endif
            bt_hsg_activate_cnf_struct *cnf_param_ptr;
            cnf_param_ptr = (bt_hsg_activate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_activate_cnf_struct), TD_CTRL);
            kal_prompt_trace(1, "rdabt_hsg_activate_cnf_cb result=%d", result);
            cnf_param_ptr ->result = result;
#ifdef __SUPPORT_SPP_SYNC__
            SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);
            SPP_Active_Req(SDP_SCLASS_UUID_SERIAL, NULL, 0x0100, NULL);
#endif
            rdabt_send_msg_up(MOD_MMI, profile==ACTIVE_PROFILE_HANDSFREE_AG?MSG_ID_BT_HFG_ACTIVATE_CNF:MSG_ID_BT_HSG_ACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
            break;
        }
        case MSG_ID_BT_HSG_DEACTIVATE_REQ:
            profile = ACTIVE_PROFILE_HEADSET_AG;
        case MSG_ID_BT_HFG_DEACTIVATE_REQ:
        {
            kal_uint8 result = HF_Deactivate(profile);
            bt_hsg_deactivate_cnf_struct *cnf_param_ptr;
            // connect_flag=FALSE;
            cnf_param_ptr = (bt_hsg_deactivate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_deactivate_cnf_struct), TD_CTRL);
            cnf_param_ptr ->result = result;

            kal_prompt_trace(1, "rdabt_hsg_deactivate_cnf_cb");
            rdabt_send_msg_up(MOD_MMI, profile==ACTIVE_PROFILE_HANDSFREE_AG?MSG_ID_BT_HFG_DEACTIVATE_CNF:MSG_ID_BT_HSG_DEACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
            break;
        }

        //HFP message
        case MSG_ID_MEDIA_BT_AUDIO_OPEN_REQ:
        {
            bt_hfg_sco_connect_req_struct  *msg_req= (bt_hfg_sco_connect_req_struct*)message->local_para_ptr;

            kal_prompt_trace(1, "MSG_ID_MEDIA_BT_AUDIO_OPEN_REQ process");
            //  HF_Call_Request(HF_CALL_OUTGOING, 1);
            rdabt_mgr_connect_sco (message->src_mod_id, current_device,  msg_req->connection_id);
            break;
        }

        case MSG_ID_MEDIA_BT_AUDIO_CLOSE_REQ:
        {
            bt_hfg_sco_disconnect_req_struct  *msg_req= (bt_hfg_sco_disconnect_req_struct*)message->local_para_ptr;
            rdabt_mgr_disconnect_sco (message->src_mod_id,  current_device,  msg_req->connection_id,msg_req->speech_on);
            break;
        }
        case MSG_ID_BT_HSG_CONNECT_REQ:
            profile = ACTIVE_PROFILE_HEADSET_AG;
        case MSG_ID_BT_HFG_CONNECT_REQ:
        {
            t_bdaddr bd_addr;
            bt_hsg_connect_req_struct  *msg_req= (bt_hsg_connect_req_struct*)message->local_para_ptr;
            connect_flag=TRUE;
            dis_connect_flag=0;
            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            current_addr = msg_req->bt_addr;
            current_device = bd_addr;
#ifdef __SUPPORT_BT_PHONE__

            btphone_auto_con_flag=0;
            btphone_current_profile=profile;
#endif
            current_hf_NREC=0;
            {
                u_int8 status_array[7]= {1,0,0,0,5,5,0};
                //\"service\",(0,1)),(\"call\",(0,1)),(\"callsetup\",(0,3)),(\"callheld\",(0,2)),(\"battchg\",(0,5)),(\"signal\",(0,5)),(\"roam\",(0,1))\r\n";
                if(rda_GetCurrentCallStatus())
                    status_array[1]=1;
                status_array[3]=rda_GetCallHoldCount();
                status_array[5]=rda_GetSignalStrength();
                status_array[6]=rda_GetRoamStatus();
                kal_prompt_trace(1, "hfp_set_indicator_status call %d,hold %d,sig %d,rost %d", status_array[1], status_array[3], status_array[5], status_array[6]);
                hfp_set_indicator_status(status_array);

                result = HF_Connect_SLC(status_array[1],bd_addr, profile);
            }
            if((result != RDABT_NOERROR)&&((result != RDABT_PENDING)))
                rdabt_hfg_connect_cnf_cb(result);
            break;
        }


        case MSG_ID_BT_HFG_DISCONNECT_REQ:
        {
            t_bdaddr bd_addr;
            bt_hsg_disconnect_cnf_struct *cnf_param_ptr;
            bt_hsg_disconnect_req_struct  *msg_req= (bt_hsg_disconnect_req_struct*)message->local_para_ptr;
            connect_flag=FALSE;
            dis_connect_flag=1;
            kal_prompt_trace(1,"MSG_ID_BT_HSG_DISCONNECT_REQ,connect_flag=%d",connect_flag);
            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            //a2dptimer_stoped=1;
            //stack_stop_timer(&rdabt_p->base_timer);
            //stack_start_timer(&rdabt_p->base_timer, 0, 21);
            result = HF_Release_SLC(bd_addr);
            if((result != RDABT_NOERROR)&&((result != RDABT_PENDING)))
            {
                cnf_param_ptr = (bt_hsg_disconnect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_disconnect_cnf_struct), TD_CTRL);
                cnf_param_ptr ->result = result;
                cnf_param_ptr ->connection_id = msg_req->connection_id;
                cnf_param_ptr ->bt_addr = msg_req->bt_addr;
                rdabt_send_msg_up(MOD_MMI, profile==ACTIVE_PROFILE_HANDSFREE_AG?MSG_ID_BT_HFG_DISCONNECT_CNF:MSG_ID_BT_HSG_DISCONNECT_CNF, (void *)cnf_param_ptr, NULL);
            }
            break;
        }
        case MSG_ID_BT_HFG_SPEAKER_GAIN_REQ:
        {
            bt_hsg_speaker_gain_req_struct  *msg_req= (bt_hsg_speaker_gain_req_struct*)message->local_para_ptr;
            bt_hsg_speaker_gain_cnf_struct *cnf_param_ptr;

            kal_prompt_trace(1,"MSG_ID_BT_HSG_SPEAKER_GAIN_REQ process msg_req->gain=%d",msg_req->gain);
            result = HF_Send_Speaker_Gain(msg_req->gain);

            cnf_param_ptr = (bt_hsg_speaker_gain_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_hsg_speaker_gain_cnf_struct), TD_CTRL);

            cnf_param_ptr ->connection_id = msg_req->connection_id;
            cnf_param_ptr ->result = result;

            rdabt_send_msg_up(MOD_MMI, profile==ACTIVE_PROFILE_HANDSFREE_AG?MSG_ID_BT_HFG_SPEAKER_GAIN_CNF:MSG_ID_BT_HSG_SPEAKER_GAIN_CNF, (void *)cnf_param_ptr, NULL);
            break;
        }
        case MSG_ID_BT_HFG_INCOMING_DISCONNECT_REQ:
        {

            kal_prompt_trace(1,"MSG_ID_BT_HFG_INCOMING_DISCONNECT_REQ is called!");
            HF_Call_Request(HF_CALL_RELEASE,1);

            break;
        }
        case MSG_ID_BT_HFG_SEND_RING_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_HFG_SEND_RING_REQ process");
            HF_Call_Request(HF_CALL_INCOMMING,1);
            break;

        }
        case MSG_ID_BT_HFG_SECOND_CALL_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_HFG_SECOND_CALL_REQ process");
            HF_Call_Request(HF_CALL_INCOMMING,1);
            break;

        }
        case MSG_ID_BT_HFG_SECOND_CALL_ACTIVE:
        {
            kal_prompt_trace(1,"MSG_ID_BT_HFG_SECOND_CALL_ACTIVE process");


            HF_Call_Request(HF_CALL_START,1);

            break;

        }
        case MSG_ID_BT_HFG_CALL_SWAP:
        {

            kal_prompt_trace(1,"MSG_ID_BT_HFG_CALL_SWAP process");

            HF_Call_Request(HF_CALL_SWAP,1);
            break;
        }
        case   MSG_ID_BT_HFG_CALL_HOLD :
        {

            kal_prompt_trace(1,"MSG_ID_BT_HFG_CALL_HOLD process");

            HF_Call_Request(HF_CALL_HOLD,1);
            break;
        }
        case MGS_ID_BT_HFP_SET_CALL_NUM_REQ:
        {
            bt_phone_dial_req_struct  *call_info=(bt_phone_dial_req_struct*)message->local_para_ptr;
            kal_uint8 call_number[30]= {0};
            memcpy(call_number,call_info->call_number,call_info->call_number_length+1);
            kal_prompt_trace(1,"MGS_ID_BT_HFP_SET_CALL_NUM_REQ process %s ",call_info->call_number);

            HF_Set_Number(call_number);
            break;
        }
        case MSG_ID_BT_HFG_SEND_STOP_RING_REQ:
        {
            BT_TraceOutputText(1,"MSG_ID_BT_HFG_STOP_RING_REQ process");
            HF_Stop_Ring();
            if(CS_timer)
                rdabt_cancel_timer(CS_timer);
            CS_timer=rdabt_timer_create(20,  rdabt_delay_SendCallSetup_process,NULL,pTIMER_ONESHOT);

            //   HF_Call_Status(HF_CALLSETUP_INDICATOR,0); // if call is accept ,then not send 3.0 need, else delay send  send
            break;

        }
        case MSG_ID_BT_HFG_BLDN_FAIL_RES:
        {
            kal_prompt_trace(1,"MSG_ID_BT_HFG_BLDN_FAIL_RES process");
            HF_Call_Status(HF_CALLSETUP_INDICATOR,0);
            break;
        }
        case MSG_ID_BT_HFG_SEND_CALL_SETUP_REQ:
        {
            //u_int32  IsIncoming= *((u_int32*)(message->local_para_ptr));
            if(CS_timer)
                rdabt_cancel_timer(CS_timer);
            CS_timer=0;
            rda_bt_hfg_start_call_notify(1);

            break;

        }
        case MSG_ID_BT_HFG_SEND_CALL_SETUP_1_RES:
        {
            extern void rda_bt_hfg_start_call_setup_1_handler(void);
            rda_bt_hfg_start_call_setup_1_handler();
            break;
        }
#ifndef __SUPPORT_BT_PHONE__
        case MSG_ID_BT_HFG_SEND_CALL_END_REQ:
        {
            void rda_bt_hfg_end_call_notify(void);
            rda_bt_hfg_end_call_notify();

            break;

        }
#endif
#ifdef __SUPPORT_BT_PHONE__
        case MSG_ID_BT_HFG_SEND_CALL_END_REQ:
        {
            void rda_bt_hfg_end_call_notify(void);
            rda_bt_hfg_end_call_notify();
            if(rdabt_currentType==ACTIVE_PROFILE_HANDSFREE)
            {
                sxr_StopFunctionTimer(rdabt_btphone_atcommand_timeout);
                sxr_StartFunctionTimer(2*16384, rdabt_btphone_atcommand_timeout, 0,0);
            }
            break;
        }
        case MSG_ID_BT_PHONE_SEND_CHUP_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_PHONE_SEND_CHUP_REQ process  0x%x %x",btphone_ATcommand_timer,BtPhone_CurrentCallStatus);
#ifdef BTPHONE_RECOGNITION_CALL
            if((btphone_recognition_call)&&(BtPhone_CurrentCallStatus==BT_CALL_STATUS_NONE))
            {
                rdabt_hfp_end_Voice_Recognition();
                return;
            }
#endif
            BtPhone_CurrentCallStatus=BT_CALL_STATUS_RELEASING;
            HF_Terminate_Call();
            sxr_StopFunctionTimer(rdabt_btphone_atcommand_timeout);
            sxr_StartFunctionTimer(2*16384, rdabt_btphone_atcommand_timeout, 0,0);

            kal_prompt_trace(1,"MSG_ID_BT_PHONE_SEND_CHUP_REQ22 btphone_ATcommand_timer=%x",btphone_ATcommand_timer);
            break;

        }
        case MSG_ID_BT_PHONE_SEND_DIAL_NUMBER_REQ:
        {
            kal_uint8 call_number[MAX_PHONE_NUMBER]= {0};
            bt_phone_dial_req_struct *dial_info=(bt_phone_dial_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_BT_PHONE_SEND_DIAL_NUMBER_REQ process btphone_ATcommand_timer=%x",btphone_ATcommand_timer);
            memcpy(call_number,dial_info->call_number,dial_info->call_number_length);
            HF_Set_Number(call_number);
            HF_Call_Request(HF_CALL_NUM,1);
            //HF_Terminate_Call();

            sxr_StopFunctionTimer(rdabt_btphone_atcommand_timeout);
            sxr_StartFunctionTimer(2*16384, rdabt_btphone_atcommand_timeout, 0,0);


            //btphone_ATcommand_timer= rdabt_timer_create(20, rdabt_btphone_atcommand_timeout, NULL,pTIMER_ONESHOT);
            BtPhone_CallDirector = 2;
            break;

        }
        case MSG_ID_BT_PHONE_SEND_CALL_ANSWER_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_PHONE_SEND_CALL_ANSWER_REQ process");

            HF_Call_Answer(HF_CALL_ACCEPT);
            break;

        }
        case MSG_ID_BT_PHONE_SEND_DTMF_REQ:
        {
            char dtmf_send[2]= {'0','\0'}; //123456789*#
            bt_phone_dtmf_send_req_struct  *msg_p=(bt_phone_dtmf_send_req_struct*)message->local_para_ptr;
            if(  ((msg_p->num>='0')&&(msg_p->num<='9'))  ||   (msg_p->num=='*') ||(msg_p->num=='#'))
            {
                dtmf_send[0]=msg_p->num;
                HF_Send_DTMF(dtmf_send );
                kal_prompt_trace(1,"MSG_ID_BT_PHONE_SEND_DTMF_REQ process OK");

            }
            else
                kal_prompt_trace(1,"MSG_ID_BT_PHONE_SEND_DTMF_REQ process error num");

            break;

        }
        case MSG_ID_BT_PHONE_AUTO_CON_REQ:
        {
            bt_hsg_connect_req_struct  *msg_req= (bt_hsg_connect_req_struct*)message->local_para_ptr;

            kal_prompt_trace(1,"MSG_ID_BT_PHONE_AUTO_CON_REQ addr=%x %x %x " ,msg_req->bt_addr.lap,msg_req->bt_addr.nap,msg_req->bt_addr.uap);
            rdabt_btphone_auto_con_req(msg_req->bt_addr);
            break;
        }

#ifdef __SUPPORT_BT_PHONE__
        case MSG_ID_BT_PHONE_SCO_DATA_SEND_REQ:
        {
            AudioSCO_ASYN_PDU  *sco_info=(AudioSCO_ASYN_PDU*)message->local_para_ptr;
            if( (rdabt_sco_player_states()==0)||(BtPhone_CurrentCallStatus==BT_CALL_STATUS_NONE))
            {
                //do nothing
            }
            else
            {
                rdabt_uart_tx_sco_data(sco_info->pdu, sco_info->handle, sco_info->length);
            }
            COS_FREE(sco_info->pdu);
            //  kal_prompt_trace(1,"rdabt_main-msg_hdler process MSG_ID_BT_PHONE_SCO_DATA_SEND_REQ");
            break;
        }
#endif
        case MSG_ID_BT_PHONE_VOIE_CHANGE_TO_PHONE:
        {
            extern u_int16 sco_index;
            MGR_ReleaseSynchronous(sco_index);

            break;
        }
        case MSG_ID_BT_PHONE_VOIE_CHANGE_TO_LOCAL:
        {
            current_voice_change_staus=1;
            MGR_SetupSynchronous(current_device);
            break;
        }
#endif
        case MSG_ID_BT_HFG_SEND_NO_CARRIER_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_HFG_SEND_NO_CARRIER_REQ process ");
            HF_Send_NoCarrier();
            break;
        }
        case MSG_ID_HFP_CON_AUTHORIZE_RES:
        {
            goep_authorize_res_struct  *accept=(goep_authorize_res_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_HFP_CON_AUTHORIZE_RES process accept->rsp_code=%d ",accept->rsp_code);
            if(accept->rsp_code)
            {
                u_int8 status_array[7]= {1,0,0,0,5,5,0};

                if(rda_GetCurrentCallStatus())
                    status_array[1]=1;
                status_array[3]=rda_GetCallHoldCount();
                status_array[5]=rda_GetSignalStrength();
                status_array[6]=rda_GetRoamStatus();
                kal_prompt_trace(1, "hfp_set_indicator_status call %d,hold %d,sig %d,rost %d", status_array[1], status_array[3], status_array[5], status_array[6]);
                hfp_set_indicator_status(status_array);
                HF_Accept_SLC( status_array[1],current_device);
            }
            else
            {
                HF_Reject_SLC(current_device);

            }
            break;
        }
#ifdef __SUPPORT_BT_SYNC_PHB__
        case  MSG_ID_BT_PBAP_CON_REQ:
        {
            t_bdaddr bd_addr;
            bt_pbap_connect_req_struct  *msg_req= (bt_pbap_connect_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_BT_PBAP_CON_REQ lap=%x,uap=%x,nap=%x", msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            pbap_phone_book_con_req(bd_addr,msg_req->con_type);
            break;
        }
        case MSG_ID_BT_PBAP_DISCON_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_PBAP_DISCON_REQ");
            pbap_discon_req();
            break;
        }
        case MSG_ID_BT_PBAP_GET_NEXT_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_PBAP_GET_NEXT_REQ");
            rdabt_pbap_get_next_req();
            break;
        }
#endif
#ifdef __SUPPORT_BT_SYNC_MAP__

        case  MSG_ID_BT_MAP_CON_REQ:
        {
            t_bdaddr bd_addr;
            bt_pbap_connect_req_struct  *msg_req= (bt_pbap_connect_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_BT_MAP_CON_REQ lap=%x,uap=%x,nap=%x", msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            BDADDR_Set_LAP_UAP_NAP(&bd_addr, msg_req->bt_addr.lap, msg_req->bt_addr.uap, msg_req->bt_addr.nap);
            bt_map_con_req(bd_addr);
            break;
        }
        case MSG_ID_BT_MAP_DISCON_REQ:
        {
            kal_prompt_trace(1,"MSG_ID_BT_MAP_DISCON_REQ");
            map_discon_req();
            break;
        }
#endif
        case MSG_ID_SEND_CALL_CIEV_REQ:
        {
            bt_call_state_struct *msg_req=(bt_call_state_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"MSG_ID_SEND_CALL_CIEV_REQ %d %d %d",msg_req->call_flag,msg_req->call_setup_flag,msg_req->call_hold_flag);
            if(msg_req->call_flag)
                HF_Send_Indicators_direct(HF_CALL_INDICATOR,msg_req->call);
            if(msg_req->call_setup_flag)
                HF_Send_Indicators_direct(HF_CALLSETUP_INDICATOR,msg_req->call_setup);
            if(msg_req->call_hold_flag)
                HF_Send_Indicators_direct(HF_CALLHOLD_INDICATOR,msg_req->call_hold);
            break;
        }
        case MSG_ID_BT_HFG_BLDN_RES_STATUS:
        {
            extern   void rda_bt_hfg_send_bldn_res(u_int8 flag);
            kal_uint8  *msg_req= (kal_uint8*)message->local_para_ptr;
            kal_prompt_trace(1,"process MSG_ID_BT_HFG_BLDN_RES_STATUS %d",*msg_req);
            rda_bt_hfg_send_bldn_res(*msg_req);

            break;
        }
#ifdef      __BT_PROFILE_BQB__

        case MSG_ID_BT_HFG_SEND_NETWORK_STATUS_REQ:
        {

            kal_uint8  *msg_req= (kal_uint8*)message->local_para_ptr;
            kal_prompt_trace(1,"process MSG_ID_BT_HFG_SEND_NETWORK_STATUS_REQ %d",*msg_req);
            HF_Send_Indicators_direct(HF_SERVICE_INDICATOR,*msg_req);
            if(*msg_req)
                HF_Send_Indicators_direct(HF_SIGNAL_INDICATOR,5);
            else
                HF_Send_Indicators_direct(HF_SIGNAL_INDICATOR,0);


            break;
        }

        case MSG_ID_BT_HFG_SEND_SIGNAL_LEVEL_REQ:
        {

            kal_uint8  *msg_req= (kal_uint8*)message->local_para_ptr;
            kal_prompt_trace(1,"process MSG_ID_BT_HFG_SEND_SIGNAL_LEVEL_REQ %d",*msg_req);

            HF_Send_Indicators_direct(HF_SIGNAL_INDICATOR,*msg_req);

            break;
        }
        case MSG_ID_BT_HFG_BATTERY_LEVEL_REQ:
        {

            kal_uint8  *msg_req= (kal_uint8*)message->local_para_ptr;
            kal_prompt_trace(1,"process MSG_ID_BT_HFG_BATTERY_LEVEL_REQ %d",*msg_req);

            HF_Send_Indicators_direct(HF_BATTCHG_INDICATOR,*msg_req);

            break;
        }







#endif
        default:
            ;//ASSERT(0);
            break;
            //end of hfp msg
    }

}
#endif
//void rda_bt_hfg_send_CallSetup_1_ind(void)
//{}


void rda_bt_hfg_send_CallSetup_1_ind(void)
{
}
void rda_bt_hfg_send_bldn_res(u_int8 flag);
extern    u_int8 rdabt_get_call_info(char * content,u_int8 *call_dir,u_int8 *call_status,u_int8 *number_len);//no call return 0, other return total call

void rdabt_hfp_clcc_ind(void)
{

    int offset = 0;
    char * CLCC;
    u_int8 num_len=0;
    u_int8 call_dir, call_status;
    u_int8 total_call=0;
    total_call=rdabt_get_call_info(NULL,NULL,NULL,&num_len);
    kal_prompt_trace(1,"Handle_Rx_CLCC total_call=%d",total_call);
    if(!total_call)
    {
        rda_bt_hfg_send_bldn_res(1);//send OK
        return;
    }

    CLCC= (char*)rdabt_malloc(100);
    memset(CLCC, 0, 100);
    memcpy(CLCC, "\r\n+CLCC: 1,1,1,0,0,\"", 20);
    offset += 20;
    num_len=1;// get first call info
    rdabt_get_call_info(CLCC+offset,&call_dir,&call_status,&num_len);
    if(call_dir==1)
        *(CLCC+11)='1';//incoming call
    else if(call_dir==0)
        *(CLCC+11)='0';//outcoming call


    if(call_status==3/*CM_ACTIVE_STATE*/)
        *(CLCC+13)='0';
    else if(call_status==4/*CM_HOLD_STATE*/)
        *(CLCC+13)='1';

    offset+=num_len;
    memcpy(CLCC+offset,"\",129\r\n",7);
    offset += 7;
    kal_prompt_trace(1," CLIP=%s", CLCC);

    HF_Send_Raw_Data((u_int8*)CLCC, offset);
    if(total_call>1)
    {
        memset(CLCC, 0, 100);
        offset=0;
        memcpy(CLCC, "\r\n+CLCC: 2,1,1,0,0,\"", 20);
        offset += 20;
        num_len=2;// get second call info
        rdabt_get_call_info(CLCC+offset,&call_dir,&call_status,&num_len);
        kal_prompt_trace(1,"Handle_Rx_CLCC call_dir  =%d,call_status=%d,num_len=%d",call_dir,call_status,num_len);

        if(call_dir==1)
            *(CLCC+11)=0x31;//incoming call
        else if(call_dir==0)
            *(CLCC+11)=0x30;//outcoming call


        if(call_status==3/*CM_ACTIVE_STATE*/)
            *(CLCC+13)=0x30;
        else if(call_status==4/*CM_HOLD_STATE*/)
            *(CLCC+13)=0x31;

        offset+=num_len;
        memcpy(CLCC+offset,"\",129\r\n",7);
        offset += 7;
        kal_prompt_trace(1," +CLCC=%s", CLCC);

        HF_Send_Raw_Data((u_int8*)CLCC, offset);
    }

    rda_bt_hfg_send_bldn_res(1);//send OK
    rdabt_free(CLCC);


}


#ifdef __SUPPORT_BT_PHONE__
void rdabt_a2dp_send_signal_connect_req( module_type src_mod_id, bt_device_addr_struct *device_addr, u_int8 local_role);
void rdabt_btphone_auto_con_req(bt_dm_addr_struct addr)
{
    t_bdaddr bd_addr;
    kal_prompt_trace(1,"rdabt_btphone_auto_con_req %x %x %x", addr.lap, addr.uap, addr.nap);
    BDADDR_Set_LAP_UAP_NAP(&bd_addr, addr.lap, addr.uap, addr.nap);
    btphone_auto_con_flag=1;
    btphone_current_profile=ACTIVE_PROFILE_HANDSFREE;
    dis_connect_flag=0;
    current_addr.lap= addr.lap;
    current_addr.nap= addr.nap;
    current_addr.uap= addr.uap;
    memcpy(current_device.bytes,bd_addr.bytes,6);
    {
        u_int8 status_array[7]= {1,0,0,0,5,5,0};
        //\"service\",(0,1)),(\"call\",(0,1)),(\"callsetup\",(0,3)),(\"callheld\",(0,2)),(\"battchg\",(0,5)),(\"signal\",(0,5)),(\"roam\",(0,1))\r\n";
        if(rda_GetCurrentCallStatus())
            status_array[1]=1;
        status_array[3]=rda_GetCallHoldCount();
        status_array[5]=rda_GetSignalStrength();
        status_array[6]=rda_GetRoamStatus();
        kal_prompt_trace(1, "hfp_set_indicator_status call %d,hold %d,sig %d,rost %d", status_array[1], status_array[3], status_array[5], status_array[6]);
        hfp_set_indicator_status(status_array);

        HF_Connect_SLC(status_array[1],bd_addr, ACTIVE_PROFILE_HANDSFREE);

    }
}

#ifdef  BTPHONE_RECOGNITION_CALL
int HF_DeActivate_Voice_Recognition(void);
int HF_Activate_Voice_Recognition(void);

void rdabt_hfp_end_Voice_Recognition(void)
{
    kal_prompt_trace(1,"rdabt_hfp_end_Voice_Recognition btphone_recognition_call=%d",btphone_recognition_call);
    btphone_recognition_call=0;
    HF_DeActivate_Voice_Recognition();

}

void rdabt_hfp_Voice_Recognition(void)
{
    kal_prompt_trace(1,"rdabt_hfp_Voice_Recognition,btphone_recognition_call=%d",btphone_recognition_call);
    if(btphone_recognition_call)
        return;
    BtPhone_CurrentCallStatus=BT_CALL_STATUS_ACTIVE;
    HF_Activate_Voice_Recognition();
    btphone_recognition_call=1;
}
#endif
#endif

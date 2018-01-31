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




#include "mmi_features.h"

#ifdef  __BT_A2DP_PROFILE__

#include "stdlib.h"
#include "kal_release.h"        /* basic data type */
//#include "stack_common.h"          /* message and module ids */
//#include "stack_msgs.h"            /* enum for message ids */
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

#include "hci.h"
#include "rdabt_task.h"
#include "rdabt_profile.h"
#include "rdabt_link_prim.h"
#include "a2dp.h"
#include "papi.h"
#include "mmi_trace.h"
#include "rdabt_a2dp.h"
#include "avrcp.h"
#include "manager.h"
#ifdef __SUPPORT_BT_A2DP_SNK__
#include "sxr_sbx.h"
#include "rdabt_main.h"
#endif


#define AVDTP_BING_BUFF_SIZE          1024
#define AVDTP_TIME_INTERVAL           1152

bt_a2dp_sbc_codec_cap_struct last_config_sbc_cap =
{
    2, //min bit pool
    32, //max bit pool
    0x1, //block len:16
    0x1, //subband num: 8,
    0x1, //alloc method: SNR.
    0x02, //sample rate:44.1
    0x01  //channel mode:joint stereo
};

bt_a2dp_mp3_codec_cap_struct last_config_mp3_cap =
{
    0x01, //layer 3
    0,    //CRC:not support
    0x1,  //channel mode:joint stereo
    0,    //MPF: 0
    0x1,  //sample rate: 48
    0x1,  //VBR:support
    0x09  //bit rate:
};
static u_int8 NeedSendDisconInd=0;


static kal_uint8 stream_handle;
//extern rdabt_context_struct* rdabt_p;

static u_int8           buf_ping[AVDTP_BING_BUFF_SIZE];
static u_int32          gbuf_len;
u_int8  a2dptimer_stoped=0;
u_int8 A2DP_local_role=BT_A2DP_SOURCE;
#define STEREO_HEADSET_COD 0X240404
u_int16 a2dp_signal_cid=0;
u_int16 a2dp_media_cid=0;
extern bt_avrcp_dev_addr_struct a2dp_avrcp_remote_addr;

#ifdef __SUPPORT_BT_A2DP_SNK__
#define SBC_SAMPLING_FREQ_16000        128   /* Octet 0 */
#define SBC_SAMPLING_FREQ_32000         64
#define SBC_SAMPLING_FREQ_44100         32
#define SBC_SAMPLING_FREQ_48000         16
#define SBC_CHANNEL_MODE_MONO            8
#define SBC_CHANNEL_MODE_DUAL_CHAN       4
#define SBC_CHANNEL_MODE_STEREO          2
#define SBC_CHANNEL_MODE_JOINT_STEREO    1
#define SBC_BLOCK_LENGTH_4             128   /* Octet 1 */
#define SBC_BLOCK_LENGTH_8              64
#define SBC_BLOCK_LENGTH_12             32
#define SBC_BLOCK_LENGTH_16             16
#define SBC_SUBBANDS_4                   8
#define SBC_SUBBANDS_8                   4
#define SBC_ALLOCATION_SNR               2
#define SBC_ALLOCATION_LOUDNESS          1
#define SBC_BITPOOL_MIN                  2   /* Octet 2 (min bitpool)  /  Octet 3 (max bitpool) */
#define SBC_BITPOOL_MAX                250
#define SBC_BITPOOL_MEDIUM_QUALITY      32
#define SBC_BITPOOL_HIGH_QUALITY        100
#define AVDTP_MEDIA_CODEC_SBC               (0)
extern BOOL mmi_btphone_audio_busy_flag(void);
t_a2dp_stream_config_ind g_bt_a2dp_sink_config;
u_int8 a2dp_snk_play=0;
u_int16 a2dp_snk_connect_id;
u_int8 a2dp_snk_sep=0;
u_int8 a2dp_snk_avrp_play_req=0;

t_bdaddr a2dp_snk_remote_addr;

void a2dp_snk_play_req_hdlr(ilm_struct *ilm_ptr)
{
    if(!audCodec_is_Playing())
    {
        BOOL send_mmc_flag = (BOOL)ilm_ptr->local_para_ptr;


        avrcp_send_btstream_status(1);
        sxr_Sleep(200);
        MCI_PlayBTStream(&g_bt_a2dp_sink_config.audio_cap);
        a2dp_snk_play=1;
        if(send_mmc_flag)
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_A2DP);
    }
}

static  u_int8 sbc_caps_sink[] =
{
    AVDTP_MEDIA_CODEC_SBC,

    SBC_SAMPLING_FREQ_16000     + SBC_SAMPLING_FREQ_32000    + SBC_SAMPLING_FREQ_44100    + SBC_SAMPLING_FREQ_48000    +
    SBC_CHANNEL_MODE_DUAL_CHAN + SBC_CHANNEL_MODE_STEREO    + SBC_CHANNEL_MODE_JOINT_STEREO,

    SBC_BLOCK_LENGTH_4          + SBC_BLOCK_LENGTH_8         + SBC_BLOCK_LENGTH_12        + SBC_BLOCK_LENGTH_16        +
    SBC_SUBBANDS_8             + SBC_ALLOCATION_SNR         + SBC_ALLOCATION_LOUDNESS,

    SBC_BITPOOL_MIN,
    SBC_BITPOOL_MEDIUM_QUALITY,
};

static bt_a2dp_audio_codec_cap_struct a2dp_snk_codec_cap =
{
    {
        2, //min bit pool
        32, //max bit pool
        0x1, //block len:16
        0x1, //subband num: 8,
        0x1, //alloc method: SNR.
        0x01, //sample rate: 1  48K 2 44.1kz
        0x01  //channel mode:joint stereo
    }
};
void rdabt_a2dp_snk_init(void)
{
    Avrcp_RegisterService(AVRCP_CONTROLLER);
    Avdtp_Register_Service(SEP_SINK);
    Avdtp_Register_Sep(A2DP_SBC, SEP_SINK, sbc_caps_sink, sizeof(sbc_caps_sink));
    //Avdtp_Register_Sep(A2DP_MPEG_AUDIO, SEP_SINK, NULL, 0);
}
kal_uint8 btphone_audio_busy=0;
kal_uint8 data_num=0;
kal_uint8 a2dp_need_pause_timer=0;
void rdabt_a2dp_send_pause_timerout(void)
{
    kal_prompt_trace(1,"rdabt_a2dp_send_pause_timerout");
    Avrcp_Send_Key(OPID_PAUSE);
    a2dp_need_pause_timer=0;

}

void rdabt_a2dp_sink_media_data_ind_callback(void)
{

    /// kal_prompt_trace(1, "rdabt_a2dp_sink_media_data_ind_callback");
    kal_prompt_trace(1,"A2DP D IND ,%d,%x,%x ,%d",btphone_audio_busy,a2dp_need_pause_timer,a2dp_snk_play,a2dp_snk_avrp_play_req);

    if(btphone_audio_busy)
    {
        btphone_audio_busy=mmi_btphone_audio_busy_flag();
        kal_prompt_trace(1,"A2DP D IND22 ,%d",btphone_audio_busy);
        if(btphone_audio_busy)
        {
            if(!a2dp_need_pause_timer)
                a2dp_need_pause_timer= rdabt_timer_create(50, rdabt_a2dp_send_pause_timerout, NULL,pTIMER_ONESHOT);
            Avdtp_Clean_Data();
        }
        else
        {
            if(a2dp_need_pause_timer)
                rdabt_cancel_timer(a2dp_need_pause_timer);
            a2dp_need_pause_timer=0;
            //avrcp_send_btstream_status(1);
            //sxr_Sleep(200);
            snd_a2dp_snk_play_req(1);
            //a2dp_snk_play=1;
            //mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_A2DP);
        }
    }
    else
    {
        if(a2dp_snk_play)
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_A2DP);
        else
        {
            if(a2dp_snk_avrp_play_req)
            {

                if(a2dp_need_pause_timer)
                    rdabt_cancel_timer(a2dp_need_pause_timer);
                a2dp_need_pause_timer=0;

                //avrcp_send_btstream_status(1);
                //sxr_Sleep(200);

                snd_a2dp_snk_play_req(1);
                //a2dp_snk_play=1;
                //mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_A2DP);
                a2dp_snk_avrp_play_req=0;
                return;
            }
            if(!a2dp_need_pause_timer)
                a2dp_need_pause_timer= rdabt_timer_create(50, rdabt_a2dp_send_pause_timerout, NULL,pTIMER_ONESHOT);
            Avdtp_Clean_Data();
        }

    }


}

void snd_a2dp_snk_play_req(void *local_param_ptr)
{
    ilm_struct *msg = allocate_ilm(MOD_MED);
    msg->src_mod_id = MOD_BT;
    msg->dest_mod_id = (module_type)MOD_MED;
    msg->sap_id = BT_APP_SAP;
    msg->msg_id = MSG_ID_BT_PHONE_A2DP_SNK_PLAY_REQ;
    msg->local_para_ptr = local_param_ptr;
    msg->peer_buff_ptr = NULL;
    rda_msg_send_ext_queue(msg);
}
#endif


#ifdef AP525_TEST_ON
u_int8 headset_ap525_reset=0;
t_bdaddr headset_ap525_addr;
static u_int16 headset_ap525_sample_rate = 0;
t_pTimer ap525_connect_timer;

static u_int32 a2dp_check_headset( t_bdaddr addr, u_int16 *name)
{
    u_int8 *ap525_name="APX";
    t_MGR_DeviceEntry *entry=NULL;

    MGR_FindDeviceRecord(addr, &entry);

    if(entry==NULL)
    {
        mmi_trace(g_sw_BT,"a2dp_check_headset  not find the device!!!!");
        return 0;
    }

    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"a2dp_check_headset  entry->name[0-2]=0x%x,0x%x, 0x%x, ap525_name[0-2]=0x%x, 0x%x, 0x%x",
                    entry->name[0],entry->name[1],entry->name[2], ap525_name[0], ap525_name[1], ap525_name[2]));

    if(strncmp(entry->name, ap525_name, 3)==0)
    {
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"a2dp_check_headset is ap525"));
        return 1;
    }
    else
    {
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"a2dp_check_headset not ap525"));
        return 0;
    }
}

u_int32 a2dp_check_ap525(void)
{
    u_int16 *ap525_name=L"APX";
    t_bdaddr addr;

    BDADDR_Set_LAP_UAP_NAP(&headset_ap525_addr, a2dp_avrcp_remote_addr.lap, a2dp_avrcp_remote_addr.uap, a2dp_avrcp_remote_addr.nap);

    return a2dp_check_headset(headset_ap525_addr, ap525_name);
}

static void a2dp_connect_ap525(void)
{
#if 0
    bt_a2dp_signal_connect_req_struct* msg_p = (bt_a2dp_signal_connect_req_struct*)construct_local_para(sizeof(bt_a2dp_signal_connect_req_struct), TD_CTRL);

    msg_p->device_addr.lap=BDADDR_Get_LAP(headset_ap525_addr);
    msg_p->device_addr.nap=BDADDR_Get_NAP(headset_ap525_addr);
    msg_p->device_addr.uap=BDADDR_Get_UAP(headset_ap525_addr);
    msg_p->local_role = 0;

    send_msg_to_bt( MOD_MMI, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_CONNECT_REQ, msg_p );
#endif

}

#endif



void bt_a2dp_send_media_channel_connect_req(void);


void rdabt_a2dp_send_activate_req( module_type src_mod_id, u_int8 local_role)
{
    bt_a2dp_activate_cnf_struct *cnf_param_ptr;



    NeedSendDisconInd=0;
#if pDEBUG
    kal_prompt_trace(1, "bt_a2dp_send_activate_req local_role=%d\n",local_role);
#endif

    if(local_role == BT_A2DP_SOURCE)
    {
        //Avdtp_Register_Sep(A2DP_MPEG_AUDIO, 0, 0);  //register the SEP of MP3
        Avdtp_Register_Sep(A2DP_SBC,BT_A2DP_SOURCE,NULL, 0);         //register the SEP of SBC.
        Avdtp_Register_Service(SEP_SOURCE);
        Avrcp_RegisterService(AVRCP_TARGET);

        cnf_param_ptr = (bt_a2dp_activate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_activate_cnf_struct), TD_CTRL);
        cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_ACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
    }
#ifdef __SUPPORT_BT_A2DP_SNK__
    void rdabt_a2dp_snk_init(void);
    rdabt_a2dp_snk_init();


#endif
}

void rdabt_a2dp_send_deactivate_req( module_type src_mod_id )
{
    bt_a2dp_deactivate_cnf_struct *cnf_param_ptr;
#if pDEBUG
    kal_prompt_trace(1, "rda_bt_a2dp_send_deactivate_req \n");
#endif
    //Avrcp_Set_State(AVRCP_INITIALISING);

    cnf_param_ptr = (bt_a2dp_deactivate_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_deactivate_cnf_struct), TD_CTRL);
    cnf_param_ptr ->result = BT_A2DP_RESULT_OK;

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_DEACTIVATE_CNF, (void *)cnf_param_ptr, NULL);
}

const struct
{
    kal_int16   type;
    void        (*handler)(void  *);
} a2dp_msg_hdlr_table[] =
{
    {A2DP_SIGNAL_CONNECT_CNF,  rdabt_a2dp_signal_connect_cnf_cb},
    {A2DP_MEDIA_CONNECT_CNF,  rdabt_a2dp_media_connect_cnf_cb},
    {A2DP_SIGNAL_DISCONN_CNF,rdabt_a2dp_signal_disconnect_cnf_cb},
    {A2DP_SEND_SEP_DISCOVER_CNF,rdabt_a2dp_discover_cnf_callback},
    {A2DP_SEND_SEP_GET_CAPABILITIES_CNF,rdabt_a2dp_get_capabilities_cnf_callback},
    {A2DP_SEND_SEP_SET_CONFIG_CNF,rdabt_a2dp_set_config_cnf_callback},
    {A2DP_SEND_SEP_RECONFIG_CNF,rdabt_a2dp_reconfig_cnf_callback},
    {A2DP_SEND_SEP_OPEN_CNF,rdabt_a2dp_open_cnf_callback},
    {A2DP_SEND_SEP_START_CNF,rdabt_a2dp_start_cnf_callback},
    {A2DP_SEND_SEP_PAUSE_CNF,rdabt_a2dp_pause_cnf_callback},
    {A2DP_SEND_SEP_CLOSE_CNF,rdabt_a2dp_close_cnf_callback},
    {A2DP_SEND_SEP_ABORT_CNF,rdabt_a2dp_abort_cnf_callback},
//  {A2DP_ACTIVATE_CNF,rdabt_a2dp_activate_cb},
//  {A2DP_DEACTIVATE_CNF,rdabt_a2dp_deactivate_cb},
    {A2DP_SIGNAL_CONNECT_IND,rdabt_a2dp_signal_connect_ind_cb},
    {A2DP_SIGNAL_DISCONNECT_IND,rdabt_a2dp_signal_disconnect_ind_cb},
    {A2DP_MEDIA_DISCONNECT_IND,rdabt_a2dp_media_disconnect_ind_cb},
    {A2DP_SEND_SEP_DISCOVER_IND,rdabt_a2dp_discover_ind_callback},
    {A2DP_SEND_SEP_GET_CAPABILITIES_IND,rdabt_a2dp_get_capabilities_ind_callback},
    {A2DP_SEND_SEP_SET_CONFIG_IND,rdabt_a2dp_set_config_ind_callback},
    {A2DP_SEND_SEP_OPEN_IND,rdabt_a2dp_open_ind_callback},
    {A2DP_SEND_SEP_START_IND,rdabt_a2dp_start_ind_callback},
    {A2DP_SEND_SEP_PAUSE_IND,rdabt_a2dp_pause_ind_callback},
    {A2DP_SEND_SEP_CLOSE_IND,rdabt_a2dp_close_ind_callback},
    {A2DP_SEND_SEP_ABORT_IND,rdabt_a2dp_abort_ind_callback},
    {A2DP_SEND_SIGNAL_NEED_CON_IND,av_rda_bt_a2dp_send_need_con_ind}
#ifdef __SUPPORT_BT_A2DP_SNK__
    {A2DP_MEDIA_DATA_IND,rdabt_a2dp_sink_media_data_ind_callback},
    {A2DP_SEND_SEP_RECONFIG_IND,rdabt_a2dp_set_reconfig_ind_callback}
#endif
};

void rdabt_a2dp_int_msg_hdlr( kal_uint16 mi,void *mv )
{
    int I;
    int n = sizeof(a2dp_msg_hdlr_table)/sizeof(a2dp_msg_hdlr_table[0]);
    // kal_prompt_trace(1, "rdabt_a2dp_int_msg_hdlr type=0x%x",mi );
    for(I=0; I<n; I++)
    {
        if( mi==a2dp_msg_hdlr_table[I].type )
        {
            a2dp_msg_hdlr_table[I].handler( mv );

            break;
        }
    }

    //ASSERT( I<N ); // not supported by the avadp
}

void rdabt_a2dp_signal_connect_ind_cb(void * pArgs)
{
    a2dp_connect_ind_msg_t *msg =(a2dp_connect_ind_msg_t*)pArgs;
    bt_a2dp_signal_connect_ind_struct *cnf_param_ptr;

    t_MGR_DeviceEntry *devEntry;
    MGR_FindDeviceRecord(msg->bdaddr, &devEntry);
    rdabt_mgr_setState(RDABT_A2DP_CONN, 0);
    if(devEntry)
    {
        if((devEntry->classOfDevice==STEREO_HEADSET_COD)
#ifdef __BT_PROFILE_BQB__    //sy
                ||(devEntry->classOfDevice==0x240414)
#endif
          )
            A2DP_local_role=BT_A2DP_SOURCE;
        else
            A2DP_local_role=BT_A2DP_SINK;
    }
#ifdef __SUPPORT_BT_A2DP_SNK__

    memcpy(a2dp_snk_remote_addr.bytes,devEntry->bdAddress.bytes,6);
#endif
    //if(A2DP_local_role==BT_A2DP_SINK)
    //  Avrcp_Connect_Req(a2dp_snk_remote_addr);
    a2dp_signal_cid=msg->cid;

    cnf_param_ptr = (bt_a2dp_signal_connect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_connect_ind_struct), TD_CTRL);
    cnf_param_ptr->connect_id = msg->cid;
    cnf_param_ptr->device_addr.lap = BDADDR_Get_LAP(&msg->bdaddr);
    cnf_param_ptr->device_addr.nap = BDADDR_Get_NAP(&msg->bdaddr);
    cnf_param_ptr->device_addr.uap = BDADDR_Get_UAP(&msg->bdaddr);
    cnf_param_ptr->local_role=A2DP_local_role;
    a2dp_avrcp_remote_addr.lap= BDADDR_Get_LAP(&msg->bdaddr);
    a2dp_avrcp_remote_addr.nap=BDADDR_Get_NAP(&msg->bdaddr);
    a2dp_avrcp_remote_addr.uap=BDADDR_Get_UAP(&msg->bdaddr);
    a2dptimer_stoped=0;
    NeedSendDisconInd=0;
    kal_prompt_trace(1, "rdabt_a2dp_signal_connect_ind_cb msg->connect_id=%x,cnf_param_ptr->local_role=%d",msg->cid,cnf_param_ptr->local_role);
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SIGNAL_CONNECT_IND, (void *)cnf_param_ptr, NULL);
}
void rdabt_change_3MPTK(u_int8 open3mpkt)
{
    struct st_t_dataBuf *cmd = NULL;
    int status=0;
    t_bdaddr addr_t;

    t_MGR_ConnectionEntry *entry=NULL;
    BDADDR_Set_LAP_UAP_NAP(&addr_t, a2dp_avrcp_remote_addr.lap, a2dp_avrcp_remote_addr.uap, a2dp_avrcp_remote_addr.nap);

    MGR_GetConnectionRecord(addr_t, &entry);
    if(!entry)
        return;
    if(open3mpkt)
        HCI_ChangeConnectionPacketType(&cmd,entry->aclHandle,0xEE1C);
    else
        HCI_ChangeConnectionPacketType(&cmd,entry->aclHandle,0xFC18);

    if(status == RDABT_NOERROR)
    {
        status = HCI_SendRawPDU(0, 1, cmd);
    }
    if(status == RDABT_NOERROR)
        pDebugPrintfEX((pLOGNOTICE,pLOGHFP, "HCI_ChangeConnectionPacketType OK"));
    else
        pDebugPrintfEX((pLOGNOTICE,pLOGHFP, "HCI_ChangeConnectionPacketType FAIL"));

}

void rdabt_a2dp_signal_disconnect_ind_cb(void *data)
{
    a2dp_disconnect_ind_msg_t *msg=(a2dp_disconnect_ind_msg_t *)data;
    bt_a2dp_signal_disconnect_ind_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_a2dp_signal_disconnect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_disconnect_ind_struct), TD_CTRL);
    cnf_param_ptr->connect_id = msg->cid;
    a2dptimer_stoped=1;
    NeedSendDisconInd=0;
    pTimerCancel_base(2);
    rdabt_mgr_setState(0, RDABT_A2DP_CONN);
    //stack_stop_timer(&rdabt_p->base_timer);
    //stack_start_timer(&rdabt_p->base_timer, 0, 21);

#ifdef __SUPPORT_BT_A2DP_SNK__
    kal_prompt_trace(1, "rdabt_a2dp_signal_disconnect_ind_cb  msg->cid=%x,%d,%d,msg->result=%x", msg->cid,A2DP_local_role,a2dp_snk_play,msg->result);
#endif
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_IND, (void *)cnf_param_ptr, NULL);
    if(A2DP_local_role==BT_A2DP_SINK)
    {
#ifdef __SUPPORT_BT_A2DP_SNK__
        if(a2dp_snk_play)
            a2dp_snk_MCI_AudioStop();
        a2dp_snk_play=0;
#endif
    }
    A2DP_local_role=BT_A2DP_SOURCE;

    //rdabt_change_3MPTK(0);
}

void rdabt_a2dp_media_disconnect_ind_cb(void *data)
{
    a2dp_disconnect_ind_msg_t *msg_p = (a2dp_disconnect_ind_msg_t *)data;
    kal_prompt_trace(1, "rdabt_a2dp_media_disconnect_ind_cb",msg_p->cid);
    AVDTP_Disconnect(a2dp_signal_cid);
    pTimerCancel_base(2);
    NeedSendDisconInd=1;
    if(A2DP_local_role==BT_A2DP_SOURCE)
        rdabt_avrcp_send_disconnect_req(0,0);

    rdabt_change_3MPTK(0);
}

void rdabt_a2dp_signal_connect_cnf_cb(void *data)
{
    a2dp_connect_ind_msg_t *msg_p = (a2dp_connect_ind_msg_t*)data;

    if(A2DP_local_role==BT_A2DP_SOURCE)
    {
        bt_a2dp_signal_connect_cnf_struct *cnf_param_ptr;
        cnf_param_ptr = (bt_a2dp_signal_connect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_connect_cnf_struct), TD_CTRL);
        kal_prompt_trace(1, "rdabt_a2dp_signal_connect_cnf_cb msg_p->result=%x", msg_p->result);
        if(msg_p->result== RDABT_NOERROR)
        {
            rdabt_mgr_setState(RDABT_A2DP_CONN, 0);
            cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
        }
        else
            cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
        a2dptimer_stoped=0;
        a2dp_signal_cid=msg_p->cid;
        cnf_param_ptr->device_addr.lap = BDADDR_Get_LAP(&msg_p->bdaddr);
        cnf_param_ptr->device_addr.uap = BDADDR_Get_UAP(&msg_p->bdaddr);
        cnf_param_ptr->device_addr.nap = BDADDR_Get_NAP(&msg_p->bdaddr);
        cnf_param_ptr->connect_id = msg_p->cid;
        kal_prompt_trace(1, "rdabt_a2dp_signal_connect_cnf_cb cid=%x,lap:%x,uap:%x,nap:%x", msg_p->cid,cnf_param_ptr->device_addr.lap,cnf_param_ptr->device_addr.uap,cnf_param_ptr->device_addr.nap);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SIGNAL_CONNECT_CNF, (void *)cnf_param_ptr, NULL);

    }
#ifdef __SUPPORT_BT_A2DP_SNK__
    else
    {
        memcpy(a2dp_snk_remote_addr.bytes,msg_p->bdaddr.bytes,6);
        a2dp_snk_connect_id=msg_p->cid;
        a2dptimer_stoped=0;
        NeedSendDisconInd=0;
        Avrcp_Connect_Req(a2dp_snk_remote_addr);
        kal_prompt_trace(1, "rdabt_a2dp_signal_connect_cnf  to ind  msg->connect_id=%x,cnf_param_ptr->local_role=%d",a2dp_snk_connect_id,A2DP_local_role);
        Avdtp_Send_Discover(msg_p->cid);

    }
#endif


}

void rdabt_a2dp_media_connect_cnf_cb(void *data)
{
    a2dp_connect_ind_msg_t *msg_p = (a2dp_connect_ind_msg_t *)data;

    kal_prompt_trace(1, "rdabt_a2dp_media_connect_cnf_cb mtu=%d,A2DP_local_role=%x",msg_p->mtu,A2DP_local_role);
    gbuf_len = msg_p->mtu - 48;
    if(gbuf_len > AVDTP_BING_BUFF_SIZE)
        gbuf_len = AVDTP_BING_BUFF_SIZE;
    if(A2DP_local_role==BT_A2DP_SOURCE)
    {
        bt_a2dp_stream_open_cnf_struct *cnf_param_ptr = (bt_a2dp_stream_open_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_open_cnf_struct), TD_CTRL);
        cnf_param_ptr->stream_handle = stream_handle;
        if(msg_p->result== RDABT_NOERROR)//  if(msg_p->result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
            cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
        else
            cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
        a2dp_media_cid=msg_p->cid;

        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_OPEN_CNF, (void *)cnf_param_ptr, NULL);
    }
#ifdef __SUPPORT_BT_A2DP_SNK__
    else
    {
        bt_a2dp_signal_connect_ind_struct *cnf_param_ptr;
        cnf_param_ptr = (bt_a2dp_signal_connect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_connect_ind_struct), TD_CTRL);
        cnf_param_ptr->connect_id = a2dp_snk_connect_id;
        cnf_param_ptr->device_addr.lap = BDADDR_Get_LAP(&a2dp_snk_remote_addr);
        cnf_param_ptr->device_addr.nap = BDADDR_Get_NAP(&a2dp_snk_remote_addr);
        cnf_param_ptr->device_addr.uap = BDADDR_Get_UAP(&a2dp_snk_remote_addr);
        cnf_param_ptr->local_role = A2DP_local_role;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SIGNAL_CONNECT_IND, (void *)cnf_param_ptr, NULL);
    }
#endif

}

void rdabt_a2dp_signal_disconnect_cnf_cb(void *data)
{
    bt_a2dp_signal_disconnect_cnf_struct *cnf_param_ptr;
    a2dp_disconnect_ind_msg_t *msg_p = (a2dp_disconnect_ind_msg_t *)data;
    kal_prompt_trace(1, "rdabt_a2dp_signal_disconnect_cnf_cb NeedSendDisconInd=%d",NeedSendDisconInd);
    if(NeedSendDisconInd)
    {
        bt_a2dp_signal_disconnect_ind_struct *cnf_param_ptr;
        cnf_param_ptr = (bt_a2dp_signal_disconnect_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_disconnect_ind_struct), TD_CTRL);
        cnf_param_ptr->connect_id = msg_p->cid;
        NeedSendDisconInd=0;
        pTimerCancel_base(2);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_IND, (void *)cnf_param_ptr, NULL);
    }
    cnf_param_ptr = (bt_a2dp_signal_disconnect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_disconnect_cnf_struct), TD_CTRL);
    if(msg_p->result== RDABT_NOERROR)
    {
        rdabt_mgr_setState(0, RDABT_A2DP_CONN);
        cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
    }
    else
        cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
    cnf_param_ptr->connect_id = msg_p->cid;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_CNF, (void *)cnf_param_ptr, NULL);
    if(A2DP_local_role==BT_A2DP_SOURCE)
        rdabt_avrcp_send_disconnect_req(0,0);

    rdabt_change_3MPTK(0);
}


void rdabt_a2dp_discover_cnf_callback(void *data)
{
    t_a2dp_command_cnf *msg_p =(t_a2dp_command_cnf *)data;
    bt_a2dp_sep_discover_cnf_struct *cnf_param_ptr;
    kal_uint8 data_length = msg_p->length;

    kal_uint8 result = msg_p->data[0]&3;
    kal_uint8 *ptr= &msg_p->data[2];


    int i,j;
    cnf_param_ptr = (bt_a2dp_sep_discover_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_sep_discover_cnf_struct), TD_CTRL);
    for (i=j = 0;  i<(data_length-2); i+=2)
    {
        cnf_param_ptr->sep_list[j].seid= (ptr[i])>>2;
        cnf_param_ptr->sep_list[j].in_use =  (ptr[i]>>1) & 1;
        cnf_param_ptr->sep_list[j].media_type = ptr[i+1] >> 4;
        cnf_param_ptr->sep_list[j].sep_type = (ptr[i+1] >> 3) & 1;
        if(cnf_param_ptr->sep_list[j].sep_type==1)
            j++;
    }

    cnf_param_ptr ->sep_num = j;

    if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
        cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
    else
        cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;

    cnf_param_ptr ->connect_id = 0;
    kal_prompt_trace(1, "discover_cnf_callback sep_num=%d, seid[0]=%x, seid[0].in_use:%d ",cnf_param_ptr->sep_num, cnf_param_ptr->sep_list[0].seid, cnf_param_ptr->sep_list[0].in_use);
    if(A2DP_local_role==BT_A2DP_SOURCE)
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SEP_DISCOVER_CNF, (void *)cnf_param_ptr, NULL);
#ifdef __SUPPORT_BT_A2DP_SNK__
    else
    {
        a2dp_snk_sep=cnf_param_ptr->sep_list[0].seid;
        Avdtp_Send_GetCapabilities(cnf_param_ptr->sep_list[0].seid, a2dp_snk_connect_id);
    }
#endif
}

void rdabt_a2dp_get_capabilities_cnf_callback(void *data)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)data;

    u_int8 *msg_p = msg->data;
    bt_a2dp_capabilities_get_cnf_struct *cnf_param_ptr;
    kal_uint8 result = (msg_p[0] & 3);
    kal_uint8 code_type;
    kal_uint8 media_codec[6];
    kal_uint8 losc = 0;
    kal_uint8 num =0;

    msg_p+=2; //skip the packet type, message type, and singnaling ID
    while(*msg_p !=AVDTP_SERVICE_MEDIA_CODEC &&num<7)
    {
        losc = *(msg_p +1);
        if(losc !=0)
        {
            msg_p +=(losc+2);
        }
        else
            msg_p+=2;
        num++;
    }

    msg_p ++;
    losc = *msg_p;
    msg_p++;

    memcpy(media_codec, msg_p, losc);
    code_type = media_codec[1];
    kal_prompt_trace(1, "rdabt_a2dp_get_capabilities_cnf_callback code_type=%d, result=%d",code_type,result);
    if(A2DP_local_role==BT_A2DP_SOURCE)
    {
        cnf_param_ptr = (bt_a2dp_capabilities_get_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_capabilities_get_cnf_struct), TD_CTRL);
        kal_prompt_trace(1, "rdabt_a2dp_get_capabilities_cnf_callback cnf_param_ptr=0x%x,end=%x",cnf_param_ptr,sizeof(bt_a2dp_capabilities_get_cnf_struct));

        if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
            cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
        else
            cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;

        cnf_param_ptr ->connect_id = 0;
        cnf_param_ptr->audio_cap_num=1;   //only 1 cap.
        if(code_type == 0)
        {
            cnf_param_ptr->audio_cap_list[0].codec_type =  BT_A2DP_SBC;
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.sample_rate = (media_codec[2]&0xF0)>>4;
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.channel_mode = (media_codec[2]&0x0F);
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.block_len = (media_codec[3]&0xF0)>>4;
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.subband_num = (media_codec[3]&0x0C)>>2;
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.alloc_method = (media_codec[3]&0x03);
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.min_bit_pool = media_codec[4];
            cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.max_bit_pool = media_codec[5];
            last_config_sbc_cap = cnf_param_ptr->audio_cap_list[0].codec_cap.sbc;
            kal_prompt_trace(1, "SBC cap sample_rate:%d, ch_mode:%x, block_len:%d, subband_num:%d, method:%d, min_pool:%d,max_pool:%d",
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.sample_rate ,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.channel_mode ,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.block_len ,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.subband_num,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.alloc_method,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.min_bit_pool,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.sbc.max_bit_pool);
        }
        else if(code_type == 1)
        {
            cnf_param_ptr->audio_cap_list[0].codec_type =  BT_A2DP_MP3;
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.layer = (media_codec[2]&0x20)>>5;
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.CRC = (media_codec[2]&0x10)>>4;
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.channel_mode = (media_codec[2]&0x0F);
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.MPF = (media_codec[3]&0x40)>>6;
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.sample_rate = (media_codec[3]&0x3F);
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.VBR = (media_codec[4]&0x80)>>7;
            cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.bit_rate = (kal_uint16)((media_codec[5])|(media_codec[5]&0x7f)<<8);
            kal_prompt_trace(1, "MP3 cap layer:%d, CRC:%x, channel_mode:%d, MPF:%d, sample_rate:%d, VBR:%d,bit_rate:%d",
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.layer ,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.CRC ,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.channel_mode,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.MPF,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.sample_rate,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.VBR,
                             cnf_param_ptr->audio_cap_list[0].codec_cap.mp3.bit_rate);
        }
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_CAPABILITIES_GET_CNF, (void *)cnf_param_ptr, NULL);
    }
#ifdef __SUPPORT_BT_A2DP_SNK__
    else
    {
        if(( (media_codec[2]&0xF0)>>4)&0x02)
            a2dp_snk_codec_cap.sbc.sample_rate=0x02;
        else
            a2dp_snk_codec_cap.sbc.sample_rate=0x01;
        Avdtp_Send_SetConfiguration(a2dp_snk_connect_id, a2dp_snk_sep, a2dp_snk_sep, A2DP_SBC, &a2dp_snk_codec_cap);
    }
#endif
}

void rdabt_a2dp_set_config_cnf_callback(void *pArgs)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)pArgs;
    bt_a2dp_stream_config_cnf_struct *cnf_param_ptr;
    kal_uint8 *msg_p =msg->data;
    kal_uint8 result = (msg_p[0] & 3);

    kal_prompt_trace(1, "rdabt_a2dp_set_config_cnf_callback result=%d",result);
    if(A2DP_local_role==BT_A2DP_SOURCE)
    {
        cnf_param_ptr = (bt_a2dp_stream_config_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_config_cnf_struct), TD_CTRL);

        cnf_param_ptr->connect_id = 0;

        if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
            cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
        else if(result == BT_A2DP_MESSAGE_TYPE_REJECT)
        {
            kal_uint8 error_code = msg_p[3];
            switch(error_code)
            {
                case 0x12:
                    cnf_param_ptr ->result = BT_A2DP_RESULT_BAD_ACP_SEID;
                    break;

                case 0x13:
                    cnf_param_ptr ->result = BT_A2DP_RESULT_SEP_IND_USE;
                    break;

                case  0x11:
                case 0x17:
                case 0x18:
                case 0x19:
                case 0x22:
                case 0x23:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x29:
                    cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
                    break;

                case 0x31:
                    cnf_param_ptr ->result = BT_A2DP_RESULT_BAD_STATE;
                    break;
                default:
                    break;

            }
        }

        cnf_param_ptr->stream_handle = stream_handle;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_CONFIG_CNF, (void *)cnf_param_ptr, NULL);
    }
#ifdef __SUPPORT_BT_A2DP_SNK__
    else
        Avdtp_Send_Open(a2dp_snk_sep);
#endif
}


void rdabt_a2dp_reconfig_cnf_callback(void *pArgs)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)pArgs;
    bt_a2dp_stream_reconfig_cnf_struct *cnf_param_ptr;
    kal_uint8 *msg_p =msg->data;
    kal_uint8 result = (msg_p[0] & 3);
    kal_prompt_trace(1,"reconfig_cnf_callback msg_p[0-3]=%d,%d,%d,%d,resutlt=0x%x",msg_p[0],msg_p[1],msg_p[2],msg_p[3],result);
    cnf_param_ptr = (bt_a2dp_stream_reconfig_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_reconfig_cnf_struct), TD_CTRL);

    if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
        cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
    else
        cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
    cnf_param_ptr->stream_handle = stream_handle;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_RECONFIG_CNF, (void *)cnf_param_ptr, NULL);

}
void av_rda_bt_a2dp_send_need_con_ind(void *pArgs)
{
    a2dp_connect_ind_msg_t *msg_p = (a2dp_connect_ind_msg_t *)pArgs;
    bt_a2dp_signal_connect_cnf_struct *cnf_param_ptr;
    cnf_param_ptr = (bt_a2dp_signal_connect_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_signal_connect_cnf_struct), TD_CTRL);
    mmi_trace(1, "av_rda_bt_a2dp_send_need_con_ind ");

    cnf_param_ptr->device_addr.lap = BDADDR_Get_LAP(&msg_p->bdaddr);
    cnf_param_ptr->device_addr.uap = BDADDR_Get_UAP(&msg_p->bdaddr);
    cnf_param_ptr->device_addr.nap = BDADDR_Get_NAP(&msg_p->bdaddr);



    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_NEED_CONNECT_IND, (void *)cnf_param_ptr, NULL);

}
void rdabt_a2dp_open_cnf_callback(void *pArgs)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)pArgs;
    bt_a2dp_stream_open_cnf_struct *cnf_param_ptr;
    kal_uint8 *msg_p =msg->data;
    kal_uint8 result = (msg_p[0] & 3);
    kal_prompt_trace(1,"rdabt_a2dp_open_cnf_callback result=%x",result);
    if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
        //cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
        bt_a2dp_send_media_channel_connect_req();
    else
    {
        if(A2DP_local_role==BT_A2DP_SOURCE)
        {
            cnf_param_ptr = (bt_a2dp_stream_open_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_open_cnf_struct), TD_CTRL);
            cnf_param_ptr->stream_handle = stream_handle;
            cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
            rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_OPEN_CNF, (void *)cnf_param_ptr, NULL);
        }
    }
}


void rdabt_a2dp_start_cnf_callback(void *pArgs)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)pArgs;
    bt_a2dp_stream_start_cnf_struct *cnf_param_ptr;
    kal_uint8 *msg_p =msg->data;
    kal_uint8 result = (msg_p[0] & 3);
    cnf_param_ptr = (bt_a2dp_stream_start_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_start_cnf_struct), TD_CTRL);
    cnf_param_ptr->stream_handle =stream_handle;
    if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
        cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
    else
        cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_START_CNF, (void *)cnf_param_ptr, NULL);
}


void rdabt_a2dp_pause_cnf_callback(void *pArgs)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)pArgs;

    bt_a2dp_stream_pause_cnf_struct *cnf_param_ptr;
    kal_uint8 *msg_p =msg->data;
    kal_uint8 result = (msg_p[0] & 3);
    cnf_param_ptr = (bt_a2dp_stream_pause_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_pause_cnf_struct), TD_CTRL);

    cnf_param_ptr->stream_handle = stream_handle;
    if(result == BT_A2DP_MESSAGE_TYPE_ACCEPT)
        cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
    else
        cnf_param_ptr ->result = BT_A2DP_RESULT_FATAL_ERROR;
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_PAUSE_CNF, (void *)cnf_param_ptr, NULL);
}

void rdabt_a2dp_send_closeCnf(void *arg)
{
    kal_prompt_trace(1, "rdabt_a2dp_send_closeCnf ");
    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_CLOSE_CNF, (void *)arg, NULL);

}
#ifdef __BT_PROFILE_BQB__//ss
u_int8 g_need2disconnectSignal;
#endif
void rdabt_a2dp_close_cnf_callback(void *pArgs)
{
    t_a2dp_command_cnf *msg =(t_a2dp_command_cnf *)pArgs;
    kal_uint8 *msg_p =msg->data;
    bt_a2dp_stream_close_cnf_struct *cnf_param_ptr;
    kal_uint8 result = (msg_p[0] & 3);
    cnf_param_ptr = (bt_a2dp_stream_close_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_close_cnf_struct), TD_CTRL);
    cnf_param_ptr->stream_handle = stream_handle;
    cnf_param_ptr ->result = BT_A2DP_RESULT_OK;
#ifdef __BT_PROFILE_BQB__//ss
    if(g_need2disconnectSignal)
    {
        g_need2disconnectSignal=0;
        AVDTP_Disconnect(a2dp_signal_cid);

    }
#endif
    Avdtp_Disconnect_Media_Channel(a2dp_media_cid);
    kal_prompt_trace(1, "rdabt_a2dp_close_cnf_callback result= %d",result);
    rdabt_timer_create(3, rdabt_a2dp_send_closeCnf,(void *)cnf_param_ptr, pTIMER_ONESHOT);
}

void rdabt_a2dp_abort_cnf_callback(void *pArgs)
{
    bt_a2dp_stream_abort_cnf_struct *cnf_param_ptr;

    cnf_param_ptr = (bt_a2dp_stream_abort_cnf_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_abort_cnf_struct), TD_CTRL);
    cnf_param_ptr->stream_handle = stream_handle;

    kal_prompt_trace(1, "rdabt_a2dp_abort_cnf_callback");

    rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_ABORT_CNF, (void *)cnf_param_ptr, NULL);
}

void rdabt_a2dp_discover_ind_callback(void * pArgs)
{
    t_a2dp_command_ind *msg = (t_a2dp_command_ind*)pArgs;
    bt_a2dp_sep_discover_ind_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_a2dp_discover_ind_callback cid=%x,A2DP_local_role=%d",msg->connect_id,A2DP_local_role);
    if(A2DP_local_role==BT_A2DP_SINK)
        Avdtp_Response_Discover(msg->connect_id);
    else
    {
        cnf_param_ptr = (bt_a2dp_sep_discover_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_sep_discover_ind_struct), TD_CTRL);
        cnf_param_ptr->connect_id = msg->connect_id;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_SEP_DISCOVER_IND, (void *)cnf_param_ptr, NULL);
    }

}

void rdabt_a2dp_get_capabilities_ind_callback(void * pArgs)
{
    t_a2dp_capabilities_get_ind *res = (t_a2dp_capabilities_get_ind*)pArgs;
    bt_a2dp_capabilities_get_ind_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_a2dp_get_capabilities_ind_callback");
    if(A2DP_local_role==BT_A2DP_SINK)
        Avdtp_Response_Get_Capabilities(res->connect_id,  res->acp_seid);
    else
    {
        cnf_param_ptr = (bt_a2dp_capabilities_get_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_capabilities_get_ind_struct), TD_CTRL);
        cnf_param_ptr->acp_seid = res->acp_seid;
        cnf_param_ptr->connect_id = res->connect_id;
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_CAPABILITIES_GET_IND, (void *)cnf_param_ptr, NULL);
    }
}
void rdabt_a2dp_set_config_ind_callback(void * pArgs)
{
    t_a2dp_stream_config_ind *res = (t_a2dp_stream_config_ind *)pArgs;
    bt_a2dp_stream_config_ind_struct *cnf_param_ptr;
    kal_uint8 code_type = res->audio_cap.codec_type+1;
    kal_prompt_trace(1, "rdabt_a2dp_set_config_ind_callback  code_type=%d, res->audio_cap.codec_type=%d",code_type, res->audio_cap.codec_type);
    kal_prompt_trace(1,"conf ind %x %x %x %x %x %x %x %x %x %x %x sizeof %x %x",
                     res->connect_id,
                     res->acp_seid ,
                     res->stream_handle
                     ,res->audio_cap.codec_type

                     , res->audio_cap.codec_cap.sbc.alloc_method
                     ,res->audio_cap.codec_cap.sbc.block_len
                     ,res->audio_cap.codec_cap.sbc.channel_mode
                     ,res->audio_cap.codec_cap.sbc.max_bit_pool
                     ,res->audio_cap.codec_cap.sbc.min_bit_pool
                     ,res->audio_cap.codec_cap.sbc.sample_rate
                     ,res->audio_cap.codec_cap.sbc.subband_num
                     ,sizeof(t_a2dp_stream_config_ind)
                     ,sizeof(res) );
    // conf ind 40 1 1 0   2 0 5 0    35 1 0 sizeof 20 4

#ifdef __SUPPORT_BT_A2DP_SNK__












    if(A2DP_local_role==BT_A2DP_SINK)
    {
        memcpy(&g_bt_a2dp_sink_config, res, sizeof(t_a2dp_stream_config_ind));
        Avdtp_Send_Res(AVDTP_SET_CONFIGURATION, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);
    }
    else
#endif
    {
        cnf_param_ptr = (bt_a2dp_stream_config_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_config_ind_struct), TD_CTRL);
        cnf_param_ptr->connect_id = res->connect_id;
        cnf_param_ptr->acp_seid= res->acp_seid;
        cnf_param_ptr->int_seid= res->int_seid;
        cnf_param_ptr->stream_handle= res->stream_handle;
        cnf_param_ptr->audio_cap.codec_type = code_type;
        if(code_type == BT_A2DP_SBC)
        {
            cnf_param_ptr->audio_cap.codec_cap.sbc.alloc_method = res->audio_cap.codec_cap.sbc.alloc_method;
            cnf_param_ptr->audio_cap.codec_cap.sbc.block_len  = res->audio_cap.codec_cap.sbc.block_len;
            cnf_param_ptr->audio_cap.codec_cap.sbc.channel_mode = res->audio_cap.codec_cap.sbc.channel_mode;
            cnf_param_ptr->audio_cap.codec_cap.sbc.max_bit_pool = res->audio_cap.codec_cap.sbc.max_bit_pool;
            cnf_param_ptr->audio_cap.codec_cap.sbc.min_bit_pool = res->audio_cap.codec_cap.sbc.min_bit_pool;
            cnf_param_ptr->audio_cap.codec_cap.sbc.sample_rate = res->audio_cap.codec_cap.sbc.sample_rate;
            cnf_param_ptr->audio_cap.codec_cap.sbc.subband_num = res->audio_cap.codec_cap.sbc.subband_num;
            kal_prompt_trace(1,"conf ind %x %x %x %x %x %x %x",cnf_param_ptr->audio_cap.codec_cap.sbc.alloc_method
                             ,cnf_param_ptr->audio_cap.codec_cap.sbc.block_len
                             ,cnf_param_ptr->audio_cap.codec_cap.sbc.channel_mode
                             ,cnf_param_ptr->audio_cap.codec_cap.sbc.max_bit_pool
                             ,cnf_param_ptr->audio_cap.codec_cap.sbc.min_bit_pool
                             ,cnf_param_ptr->audio_cap.codec_cap.sbc.sample_rate
                             ,cnf_param_ptr->audio_cap.codec_cap.sbc.subband_num);
            //cnf_param_ptr->audio_cap.codec_cap.sbc.sample_rate =0x08;
        }
        else if(code_type == BT_A2DP_MP3)
        {
            cnf_param_ptr->audio_cap.codec_cap.mp3.bit_rate = res->audio_cap.codec_cap.mp3.bit_rate;
            cnf_param_ptr->audio_cap.codec_cap.mp3.channel_mode = res->audio_cap.codec_cap.mp3.channel_mode;
            cnf_param_ptr->audio_cap.codec_cap.mp3.CRC = res->audio_cap.codec_cap.mp3.CRC;
            cnf_param_ptr->audio_cap.codec_cap.mp3.layer = res->audio_cap.codec_cap.mp3.layer;
            cnf_param_ptr->audio_cap.codec_cap.mp3.MPF = res->audio_cap.codec_cap.mp3.MPF;
            cnf_param_ptr->audio_cap.codec_cap.mp3.sample_rate = res->audio_cap.codec_cap.mp3.sample_rate;
            cnf_param_ptr->audio_cap.codec_cap.mp3.VBR = res->audio_cap.codec_cap.mp3.VBR;
        }
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_CONFIG_IND, (void *)cnf_param_ptr, NULL);
    }
}

void rdabt_a2dp_open_ind_callback(void * pArgs)
{
    kal_uint8 handle = (kal_uint8)pArgs;
    bt_a2dp_stream_open_ind_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_a2dp_open_ind_callback");

    if(A2DP_local_role==BT_A2DP_SINK)
        Avdtp_Send_Res(AVDTP_OPEN, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);
    else
    {
        cnf_param_ptr = (bt_a2dp_stream_open_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_open_ind_struct), TD_CTRL);
        cnf_param_ptr->stream_handle = handle; //store the acp endpoint.
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_OPEN_IND, (void *)cnf_param_ptr, NULL);
    }
}
#ifdef __SUPPORT_BT_A2DP_SNK__
void rdabt_a2dp_set_reconfig_ind_callback(void * pArgs)
{
    t_a2dp_stream_config_ind *res = (t_a2dp_stream_config_ind *)pArgs;
    bt_a2dp_stream_config_ind_struct *cnf_param_ptr;
    kal_uint8 code_type = res->audio_cap.codec_type;
    kal_prompt_trace(1, "rdabt_a2dp_set_reconfig_ind_callback  code_type=%d, res->audio_cap.codec_type=%d",code_type, res->audio_cap.codec_type);
    kal_prompt_trace(1,"conf ind %x %x %x %x %x %x %x %x %x %x %x sizeof %x %x",
                     res->connect_id,
                     res->acp_seid ,
                     res->stream_handle
                     ,res->audio_cap.codec_type
                     , res->audio_cap.codec_cap.sbc.alloc_method
                     ,res->audio_cap.codec_cap.sbc.block_len
                     ,res->audio_cap.codec_cap.sbc.channel_mode
                     ,res->audio_cap.codec_cap.sbc.max_bit_pool
                     ,res->audio_cap.codec_cap.sbc.min_bit_pool
                     ,res->audio_cap.codec_cap.sbc.sample_rate
                     ,res->audio_cap.codec_cap.sbc.subband_num
                     ,sizeof(t_a2dp_stream_config_ind)
                     ,sizeof(res) );

    if(A2DP_local_role==BT_A2DP_SINK)
    {
        memcpy(&g_bt_a2dp_sink_config, res, sizeof(t_a2dp_stream_config_ind));
        Avdtp_Send_Res(AVDTP_RECONFIGURE, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);
    }
}
#endif
void rdabt_a2dp_start_ind_callback(void * pArgs)
{
    kal_uint8 handle = (kal_uint8)pArgs;
    bt_a2dp_stream_start_ind_struct *cnf_param_ptr;

#ifdef __SUPPORT_BT_A2DP_SNK__
    a2dp_snk_avrp_play_req=0;
    kal_prompt_trace(1, "rdabt_a2dp_start_ind_callback handle=%d,a2dp_need_pause_timer=%x",handle,a2dp_need_pause_timer);
    if(A2DP_local_role==BT_A2DP_SINK)
    {
        Avdtp_Send_Res(AVDTP_START, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);
        Avrcp_Connect_Req(a2dp_snk_remote_addr);
        btphone_audio_busy=mmi_btphone_audio_busy_flag();
        data_num=0;
        if(!btphone_audio_busy)
        {
            //avrcp_send_btstream_status(1);
            //sxr_Sleep(200);

            snd_a2dp_snk_play_req(0);
            //a2dp_snk_play=1;

        }
        else
        {

            if(a2dp_need_pause_timer)
                rdabt_cancel_timer(a2dp_need_pause_timer);
            a2dp_need_pause_timer=0;
        }

    }
    else
#endif
    {
        cnf_param_ptr = (bt_a2dp_stream_start_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_start_ind_struct), TD_CTRL);
        cnf_param_ptr->stream_handle = handle; //store the acp endpoint.
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_START_IND, (void *)cnf_param_ptr, NULL);
    }
}
extern u_int8 audio_stop_timer;

void rdabt_a2dp_pause_by_hfp(void)
{
#ifdef __SUPPORT_BT_A2DP_SNK__

    if(a2dp_snk_play)
        a2dp_snk_MCI_AudioStop();
    a2dp_snk_play=0;
    if(a2dp_need_pause_timer)
        rdabt_cancel_timer(a2dp_need_pause_timer);
    a2dp_need_pause_timer=0;
#endif


}

void rdabt_a2dp_pause_ind_callback(void * pArgs)
{
    kal_uint8 handle = (kal_uint8)pArgs;
    bt_a2dp_stream_pause_ind_struct *cnf_param_ptr;
#ifdef __SUPPORT_BT_A2DP_SNK__
    kal_prompt_trace(1, "rdabt_a2dp_pause_ind_callback,a2dp_need_pause_timer=%d,audio_stop_timer=%x",a2dp_need_pause_timer,audio_stop_timer);
    if(A2DP_local_role==BT_A2DP_SINK)
    {
        if(audio_stop_timer)
        {
            rdabt_cancel_timer(audio_stop_timer);
            audio_stop_timer=0;
        }
        if(a2dp_snk_play)
            a2dp_snk_MCI_AudioStop();
        a2dp_snk_play=0;
        Avdtp_Send_Res(AVDTP_SUSPEND, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);
        if(a2dp_need_pause_timer)
            rdabt_cancel_timer(a2dp_need_pause_timer);
        a2dp_need_pause_timer=0;
    }
    else
#endif
    {
        cnf_param_ptr = (bt_a2dp_stream_pause_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_pause_ind_struct), TD_CTRL);
        cnf_param_ptr->stream_handle = handle; //store the acp endpoint.
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_PAUSE_IND, (void *)cnf_param_ptr, NULL);
    }
}

void rdabt_a2dp_close_ind_callback(void * pArgs)
{
    kal_uint8 handle = (kal_uint8)pArgs;
    bt_a2dp_stream_close_ind_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_a2dp_close_ind_callback");
    if(A2DP_local_role==BT_A2DP_SINK)
    {
#ifdef __SUPPORT_BT_A2DP_SNK__
        if(a2dp_snk_play)
            a2dp_snk_MCI_AudioStop();
        a2dp_snk_play=0;
#endif
        Avdtp_Send_Res(AVDTP_CLOSE, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);
    }
    else
    {
        cnf_param_ptr = (bt_a2dp_stream_close_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_close_ind_struct), TD_CTRL);
        cnf_param_ptr->stream_handle = handle; //store the acp endpoint.
        pTimerCancel_base(2);
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_CLOSE_IND, (void *)cnf_param_ptr, NULL);
    }
}

void rdabt_a2dp_abort_ind_callback(void * pArgs)
{
    kal_uint8 handle = (kal_uint8)pArgs;
    bt_a2dp_stream_abort_ind_struct *cnf_param_ptr;
    kal_prompt_trace(1, "rdabt_a2dp_abort_ind_callback");
#ifdef __SUPPORT_BT_A2DP_SNK__
    if(A2DP_local_role==BT_A2DP_SINK)
    {
        if(a2dp_snk_play)
            a2dp_snk_MCI_AudioStop();
        a2dp_snk_play=0;
        Avdtp_Send_Res(AVDTP_ABORT, AVDTP_MESSAGE_TYPE_ACCEPT,avdtp_bad_header_format,a2dp_signal_cid);

        if(a2dp_need_pause_timer)
            rdabt_cancel_timer(a2dp_need_pause_timer);
        a2dp_need_pause_timer=0;
    }
    else
#endif
    {
        cnf_param_ptr = (bt_a2dp_stream_abort_ind_struct *)construct_local_para( (kal_uint16)sizeof(bt_a2dp_stream_abort_ind_struct), TD_CTRL);
        cnf_param_ptr->stream_handle = handle; //store the acp endpoint.
        rdabt_send_msg_up(MOD_MMI, MSG_ID_BT_A2DP_STREAM_ABORT_IND, (void *)cnf_param_ptr, NULL);
    }
}


void rdabt_a2dp_send_signal_connect_req( module_type src_mod_id, bt_device_addr_struct *device_addr, u_int8 local_role)
{
    t_bdaddr bd_addr;
    u_int8 status=RDABT_NOERROR;
    BDADDR_Set_LAP_UAP_NAP(&bd_addr, device_addr->lap, device_addr->uap, device_addr->nap);

    status=AVDTP_Setup_Connect(bd_addr);
    A2DP_local_role=BT_A2DP_SOURCE;
    a2dp_avrcp_remote_addr.lap=device_addr->lap;
    a2dp_avrcp_remote_addr.nap=device_addr->nap;
    a2dp_avrcp_remote_addr.uap=device_addr->uap;
    if(status!=RDABT_NOERROR && status!=RDABT_PENDING)
    {
        a2dp_connect_ind_msg_t msg;
        msg.result = status;
        msg.cid    = 0;
        memcpy(msg.bdaddr.bytes, bd_addr.bytes, sizeof(t_bdaddr));
        rdabt_a2dp_signal_connect_cnf_cb(&msg);
    }

}

void rdabt_a2dp_send_signal_disconnect_req( module_type src_mod_id, u_int16 connect_id  )
{
    u_int8 status=RDABT_NOERROR;

#ifdef __SUPPORT_BT_A2DP_SNK__
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_signal_disconnect_req a2dp_snk_play=%d\n",a2dp_snk_play));
    if(a2dp_snk_play)
        a2dp_snk_MCI_AudioStop();
    a2dp_snk_play=0;
#endif
#ifdef __BT_PROFILE_BQB__//ss
    {
        t_bdaddr bd_addr;
        g_need2disconnectSignal=1;

        BDADDR_Set_LAP_UAP_NAP(&bd_addr, a2dp_avrcp_remote_addr.lap, a2dp_avrcp_remote_addr.uap, a2dp_avrcp_remote_addr.nap);
        status=Avdtp_Send_Close(bd_addr);
    }
#else
    status = AVDTP_Disconnect(a2dp_signal_cid);// L2_Disconnect(connect_id);
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_signal_disconnect_req connect_id=%d, status=%d\n", connect_id,status));
#endif
    a2dptimer_stoped=1;
    NeedSendDisconInd=0;
    pTimerCancel_base(2);
    if(status!=RDABT_NOERROR)
    {
        a2dp_connect_ind_msg_t msg;
        msg.result = status;
        msg.cid    = 0;
        rdabt_a2dp_signal_disconnect_cnf_cb(&msg);
    }
#endif
}

void rdabt_a2dp_send_sep_discover_req( module_type src_mod_id, u_int16 connect_id  )
{
    u_int8 status=RDABT_NOERROR;
    status=Avdtp_Send_Discover(connect_id);
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_sep_discover_req connect_id=0x%x, status=%d\n", connect_id,status));
#endif

    if(status!=RDABT_NOERROR)
    {
        u_int8 resp[4]= {0};
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_pause_req ERROR!!! status=%d\n",status ));
        resp[1] = BT_A2DP_MESSAGE_TYPE_REJECT;
        resp[3] = status;
        rdabt_a2dp_discover_cnf_callback(resp);
    }

}

void rdabt_a2dp_send_capabilities_get_req( module_type src_mod_id, u_int16 connect_id, u_int8 acp_seid)
{
    u_int8 status=RDABT_NOERROR;

    status =Avdtp_Send_GetCapabilities(acp_seid, connect_id);
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_capabilities_get_req connect_id=%d acp_seid=%d,status=%d\n", connect_id,acp_seid,status));
#endif

    if(status!=RDABT_NOERROR)
    {
        u_int8 resp[4]= {0};
        resp[0]=BT_A2DP_MESSAGE_TYPE_REJECT;
        resp[3] = status;
        rdabt_a2dp_get_capabilities_cnf_callback(resp);
    }
}


void rdabt_a2dp_send_stream_config_req( u_int16 connect_id, u_int8 acp_seid, u_int8 int_seid, bt_a2dp_audio_cap_struct* audio_cap )
{
    u_int8 status=RDABT_NOERROR;
    u_int8 codec_type =  audio_cap->codec_type;

    stream_handle = acp_seid;
    if(codec_type == A2DP_SBC)
    {
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rdabt_a2dp_send_stream_config_req,audio_cap->codec_cap.sbc.min_bit_pool=0x%x, audio_cap->codec_cap.sbc.channel_mode=%d",
                        audio_cap->codec_cap.sbc.min_bit_pool, audio_cap->codec_cap.sbc.channel_mode));
        last_config_sbc_cap.sample_rate = audio_cap->codec_cap.sbc.sample_rate;
        if(audio_cap->codec_cap.sbc.max_bit_pool>last_config_sbc_cap.max_bit_pool)
            audio_cap->codec_cap.sbc.max_bit_pool=last_config_sbc_cap.max_bit_pool;

#ifdef AP525_TEST_ON
        if(a2dp_check_ap525() )
        {
            pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rdabt_a2dp_send_stream_config_req is AP525, set the channel mode to dual mode!!"));
            //audio_cap->codec_cap.sbc.channel_mode = 4;  //use the dual replace the mono for the EQ/THD Test.
        }
#endif

    }
    else if(codec_type == A2DP_MPEG_AUDIO)
    {
        last_config_mp3_cap.sample_rate = audio_cap->codec_cap.mp3.sample_rate;
    }
    status=Avdtp_Send_SetConfiguration(connect_id, acp_seid, int_seid,codec_type,0,0 ,&(audio_cap->codec_cap));
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_config_req connect_id=%d acp_seid=%d int_seid=%d,status=%d\n", connect_id, acp_seid, int_seid,status));
#endif
    kal_prompt_trace(1,"rda_bt_a2dp_send_stream_config_req connect_id=%d acp_seid=%d int_seid=%d,status=%d\n", connect_id, acp_seid, int_seid,status);
    if(status!=RDABT_NOERROR)
    {
        u_int8 resp[4]= {0};
        resp[0] = BT_A2DP_MESSAGE_TYPE_REJECT;
        resp[3] = status;
        rdabt_a2dp_set_config_cnf_callback(resp);
    }

}

void rdabt_a2dp_send_stream_open_req( module_type src_mod_id, u_int8 acp_seid)
{
    u_int8 status=RDABT_NOERROR;

    status=Avdtp_Send_Open(acp_seid,a2dp_signal_cid);
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_open_req acp_seid=%d,status=%d \n", acp_seid,status ));
#endif
    if(status!=RDABT_NOERROR)
    {
        u_int8 resp[4]= {0};
        resp[0]=BT_A2DP_MESSAGE_TYPE_REJECT;
        resp[3] = status;
        rdabt_a2dp_open_cnf_callback(resp);
    }
}



void rdabt_exit_sniff(void)
{
    struct st_t_dataBuf *cmd = NULL;
    int status=0;
    t_bdaddr addr_t;

    t_MGR_ConnectionEntry *entry=NULL;
    BDADDR_Set_LAP_UAP_NAP(&addr_t, a2dp_avrcp_remote_addr.lap, a2dp_avrcp_remote_addr.uap, a2dp_avrcp_remote_addr.nap);

    MGR_GetConnectionRecord(addr_t, &entry);

    if(entry)
    {
        status = HCI_ExitSniffMode(&cmd,entry->aclHandle);
    }
    if(status == RDABT_NOERROR)
    {
        status = HCI_SendRawPDU(0, 1, cmd);
    }
    if(status == RDABT_NOERROR)
        pDebugPrintfEX((pLOGNOTICE,pLOGHFP, "rdabt_exit_sniff OK"));
    else
        pDebugPrintfEX((pLOGNOTICE,pLOGHFP, "rdabt_exit_sniff FAIL"));

}

void rdabt_a2dp_send_stream_start_req( module_type src_mod_id, u_int8 acp_seid )
{
    u_int8 status = RDABT_NOERROR;

    kal_prompt_trace(1, "rdabt_a2dp_send_stream_start_req");
    status=Avdtp_Send_Start(a2dp_signal_cid);
    rdabt_exit_sniff();

    rdabt_change_3MPTK(1);
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_start_req acp_seid=%d,status=%d\n", acp_seid,status));
#endif

    if(status!= RDABT_NOERROR)
    {
        u_int8 resp[4];
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_pause_req ERROR!!! status=%d\n",status ));
        resp[0] = BT_A2DP_MESSAGE_TYPE_REJECT;
        rdabt_a2dp_start_cnf_callback(resp);
    }

}

void bt_a2dp_send_media_channel_connect_req(void)
{
    u_int8 status=RDABT_NOERROR;

    status=AVDTP_Start_Media_Channel_Connection(a2dp_signal_cid);

#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_media_channel_connect_req status=%d",status));
#endif
}

void bt_a2dp_send_media_channel_disconnect_req(void)
{
    Avdtp_Disconnect_Media_Channel(a2dp_media_cid);
}


eventid data_send_timer;

void rdabt_a2dp_send_stream_pause_req( module_type src_mod_id, u_int8 stream_handle)
{
    u_int8 status=RDABT_NOERROR;
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_pause_req stream_handle=%d\n",stream_handle ));
#endif
    //stack_stop_timer(&rdabt_p->base_timer);
    //stack_start_timer(&rdabt_p->base_timer, 0, 21);
    pTimerCancel_base(2);


    if(data_send_timer)
    {
        ;//evshed_cancel_event(rdabt_p->evsh_sched, &data_send_timer);
    }

    status=Avdtp_Send_Suspend(a2dp_signal_cid);
    if(status != RDABT_NOERROR)
    {
        u_int8 resp[4];
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_pause_req ERROR!!! status=%d\n",status ));
        resp[0] = BT_A2DP_MESSAGE_TYPE_REJECT;
        rdabt_a2dp_pause_cnf_callback(resp);
    }
}

void rdabt_a2dp_send_stream_close_req( module_type src_mod_id, u_int8 stream_handle)
{
    u_int8 status=RDABT_NOERROR;
    t_bdaddr bd_addr;

#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_close_req stream_handle=%d,status=%d\n", stream_handle,status ));
#endif
    BDADDR_Set_LAP_UAP_NAP(&bd_addr, a2dp_avrcp_remote_addr.lap, a2dp_avrcp_remote_addr.uap, a2dp_avrcp_remote_addr.nap);
    status=Avdtp_Send_Close(bd_addr);


    if(status != RDABT_NOERROR)
    {
        u_int8 resp[4];
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_pause_req ERROR!!! status=%d\n",status ));
        resp[0] = BT_A2DP_MESSAGE_TYPE_REJECT;
        rdabt_a2dp_close_cnf_callback(resp);
    }
    pTimerCancel_base(2);
    //stack_stop_timer(&rdabt_p->base_timer);
    //stack_start_timer(&rdabt_p->base_timer, 0, 21);
    pTimerCancel_base(2);

    if(data_send_timer)
    {
        ;//evshed_cancel_event(rdabt_p->evsh_sched, &data_send_timer);
    }
}

void rdabt_a2dp_send_stream_abort_req( module_type src_mod_id, u_int8 stream_handle )
{
    u_int8 status=RDABT_NOERROR;

    if(data_send_timer)
    {
        ; //evshed_cancel_event(rdabt_p->evsh_sched, &data_send_timer);
    }
    //stack_stop_timer(&rdabt_p->base_timer);
    //stack_start_timer(&rdabt_p->base_timer, 0, 21);
    pTimerCancel_base(2);

    status=Avdtp_Send_Abort(a2dp_signal_cid);
#if pDEBUG
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_abort_req status=%d \n" ,status));
#endif
    if(status != RDABT_NOERROR)
    {
        u_int8 resp[4];
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_pause_req ERROR!!! status=%d\n",status ));
        resp[0] = BT_A2DP_MESSAGE_TYPE_REJECT;
        rdabt_a2dp_abort_cnf_callback(resp);
    }
}
A2DP_codec_struct *a2dp_codec;

void A2dpDataSendTimerCallback(void)
{
    bt_a2dp_send_stream_data(a2dp_codec);
}

#ifdef AP525_TEST_ON
u_int32 a2dp_check_ap525(void);
u_int8 a2dp_start_data_timer=0;
static void rdabt_a2dp_send_data_start(void *arg)
{
    A2DP_codec_struct* codec = (A2DP_codec_struct*)arg;
    if(a2dp_start_data_timer)
    {
        rdabt_cancel_timer(a2dp_start_data_timer);
        a2dp_start_data_timer=0;
    }
    pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rdabt_a2dp_send_data_start"));

    bt_a2dp_send_stream_data(codec);

}
#endif


void rdabt_a2dp_send_stream_data_send_req( module_type src_mod_id, u_int8 stream_handle, A2DP_codec_struct* codec )
{
#if 1
//   u_int8 codec_type=Avdtp_Get_Codec_Type();

    if(codec==NULL)
    {
        return;
    }

    if(codec->state &2)
    {
#if pDEBUG
        pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"rda_bt_a2dp_send_stream_data_send_req codec->state=2\n" ));
#endif
        pMemset(buf_ping, 0,sizeof(buf_ping));
        //evshed_cancel_event(rdabt_p->evsh_sched, &data_send_timer);
        return;
    }

    if(codec->QueryPayloadSize ==NULL || codec->GetPayload ==NULL)
        return;

#ifdef AP525_TEST_ON
    if(a2dp_check_ap525())
        a2dp_start_data_timer= rdabt_timer_create(10, rdabt_a2dp_send_data_start, (void*)codec, pTIMER_PERIODIC);
    else
#endif
        bt_a2dp_send_stream_data(codec);
#endif
}

extern void checkMsg(void *message);

void rdabt_a2dp_msg_hdlr(ilm_struct *message)
{
    // kal_prompt_trace(1,"received message->msg_id %d MSG_ID_BT_A2DP_SIGNAL_CONNECT_REQ %d",message->msg_id,MSG_ID_BT_A2DP_SIGNAL_CONNECT_REQ);

    switch(message->msg_id)
    {
        //A2DP
        case MSG_ID_BT_A2DP_SIGNAL_CONNECT_REQ:
        {
            bt_a2dp_signal_connect_req_struct * msg_p = (bt_a2dp_signal_connect_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"process MSG_ID_BT_A2DP_SIGNAL_CONNECT_REQ ");
            rdabt_a2dp_send_signal_connect_req(MOD_MMI, &(msg_p->device_addr), msg_p->local_role);
            break;
        }

        case MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_REQ:
        {
            bt_a2dp_signal_disconnect_req_struct * msg_p = (bt_a2dp_signal_disconnect_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_signal_disconnect_req(MOD_MMI, msg_p->connect_id);
            break;
        }

        case MSG_ID_BT_A2DP_SEP_DISCOVER_REQ:
        {
            bt_a2dp_sep_discover_req_struct * msg_p = (bt_a2dp_sep_discover_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_sep_discover_req(MOD_MMI, msg_p->connect_id);
            break;
        }

        case MSG_ID_BT_A2DP_CAPABILITIES_GET_REQ:
        {
            bt_a2dp_capabilities_get_req_struct * msg_p = (bt_a2dp_capabilities_get_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_capabilities_get_req(MOD_MMI, msg_p->connect_id, msg_p->acp_seid);
            break;
        }


        case MSG_ID_BT_A2DP_STREAM_CONFIG_REQ:
        {
            bt_a2dp_stream_config_req_struct * msg_p = (bt_a2dp_stream_config_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_stream_config_req(
                msg_p->connect_id,
                msg_p->acp_seid,
                msg_p->int_seid,
                //msg_p->audio_cap.codec_type,
                (bt_a2dp_audio_cap_struct*)&msg_p->audio_cap
            );
            break;
        }

        case MSG_ID_BT_A2DP_STREAM_OPEN_REQ:
        {
            bt_a2dp_stream_open_req_struct * msg_p = (bt_a2dp_stream_open_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_stream_open_req(MOD_MMI, msg_p->stream_handle);
            break;
        }
        case MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ:
        {
            u_int8 status=RDABT_NOERROR;
            bt_a2dp_stream_reconfig_req_struct * msg_p = (bt_a2dp_stream_reconfig_req_struct*)message->local_para_ptr;
            stream_handle = msg_p->acp_seid;
            pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"a2dp reconfig_req msg_p->audio_cap.codec_type=%d, channel_mode=%d",\
                            msg_p->audio_cap.codec_type, msg_p->audio_cap.codec_cap.sbc.channel_mode));

#ifdef AP525_TEST_ON
            if(a2dp_check_ap525() )
            {
                pDebugPrintfEX((pLOGNOTICE,pLOGA2DP,"MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ is AP525, set the channel mode to dual mode!!"));
                //msg_p->audio_cap.codec_cap.sbc.channel_mode = 4;  //use the dual replace the mono for the EQ/THD Test.
            }
#endif

            status=Avdtp_Send_Reconfigure(msg_p->connect_id,
                                          msg_p->acp_seid,
                                          0,
                                          msg_p->audio_cap.codec_type,
                                          &msg_p->audio_cap.codec_cap
                                         );

            if(status!=RDABT_NOERROR)
            {
                u_int8 resp[4]= {0};
                resp[0]=BT_A2DP_MESSAGE_TYPE_REJECT;
                resp[3] = status;
                RDABT_Send_Message(A2DP_SEND_SEP_RECONFIG_CNF, RDABT_A2DP, RDABT_ADP, 4, resp);
            }
            break;
        }

        case MSG_ID_BT_A2DP_STREAM_START_REQ:
        {
            bt_a2dp_stream_start_req_struct * msg_p = (bt_a2dp_stream_start_req_struct*)message->local_para_ptr;
            kal_prompt_trace(1,"received MSG_ID_BT_A2DP_STREAM_START_REQ");
            rdabt_a2dp_send_stream_start_req(MOD_MMI, msg_p->stream_handle);
            break;
        }


        case MSG_ID_BT_A2DP_STREAM_PAUSE_REQ:
        {
            kal_prompt_trace(1,"received MSG_ID_BT_A2DP_STREAM_PAUSE_REQ");
            bt_a2dp_stream_pause_req_struct * msg_p = (bt_a2dp_stream_pause_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_stream_pause_req(MOD_MMI,msg_p->stream_handle);
            break;
        }

        case MSG_ID_BT_A2DP_STREAM_CLOSE_REQ:
        {
            kal_prompt_trace(1,"received MSG_ID_BT_A2DP_STREAM_PAUSE_REQ");
            bt_a2dp_stream_close_req_struct * msg_p = (bt_a2dp_stream_close_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_stream_close_req(MOD_MMI, msg_p->stream_handle);
            break;
        }

        case MSG_ID_BT_A2DP_STREAM_ABORT_REQ:
        {
            kal_prompt_trace(1,"received MSG_ID_BT_A2DP_STREAM_ABORT_REQ");
            bt_a2dp_stream_abort_req_struct * msg_p = (bt_a2dp_stream_abort_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_stream_abort_req(MOD_MMI, msg_p->stream_handle);
            break;
        }

        case MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ:
        {
            kal_prompt_trace(1,"received MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ");
            bt_a2dp_stream_data_send_req_struct  *msg_p = (bt_a2dp_stream_data_send_req_struct*)message->local_para_ptr;
#ifdef __BT_PROFILE_BQB__//ss
            if(!g_need2disconnectSignal)
#endif
                rdabt_a2dp_send_stream_data_send_req(MOD_MMI, msg_p->stream_handle, msg_p->codec);
            break;
        }
        case MSG_ID_BT_A2DP_ACTIVATE_REQ:
        {
            bt_a2dp_activate_req_struct *msg_p = (bt_a2dp_activate_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_activate_req(MOD_MMI, msg_p->local_role);
        }
        break;
        case MSG_ID_BT_A2DP_DEACTIVATE_REQ:
        {
            bt_a2dp_deactivate_req_struct *msg_p = (bt_a2dp_deactivate_req_struct*)message->local_para_ptr;
            rdabt_a2dp_send_deactivate_req(MOD_MMI);
        }
        break;
        //end of A2DP msg
        default:
            ;// ASSERT(0);
            break;

    }


}

void bt_a2dp_send_stream_data(A2DP_codec_struct *codec)

{
#if 1
    u_int32 min, total, sample_cnt=0;
    int status= RDABT_NOERROR;
    static u_int32 size_ping=0;
    static u_int32 seq_num =0;
    static u_int32 get_len=0;
    static u_int32  g_TimeStamp;
    static u_int8 ReTrySendFlag=0;
    a2dp_codec = codec;
    kal_prompt_trace(1,"Rs=%d,%d",ReTrySendFlag,gbuf_len);
    if(ReTrySendFlag == 0)
    {
        codec->QueryPayloadSize(&min, &total);
        get_len = total;
        // kal_prompt_trace(1,"tt %d",total);
        if(min<=total && size_ping<=total  &&  gbuf_len<=get_len)
        {
            memset(buf_ping, 0,sizeof(buf_ping));
            size_ping=codec->GetPayload(buf_ping, gbuf_len, &sample_cnt);
            // size_ping=   sample_cnt;
        }
        // pDebugPrintfEX((pLOGDEBUG,pLOGA2DP,"bt_a2dp_send_stream_data min:%d, total:%d, size_ping=%d, gbuf_len=%d,get_len=%d \n", min, total, size_ping, gbuf_len, get_len));

        if(size_ping > 0)
        {
            status=Avdtp_Send_Data(size_ping-1, buf_ping+1, g_TimeStamp, seq_num,0, 0, buf_ping[0]);
            // pDebugPrintfEX((pLOGDEBUG,pLOGA2DP," sample_cnt:%d, buf_ping[0]:%x,%x,%x,%x, size_ping=%d,  \n", sample_cnt, buf_ping[0],buf_ping[1],buf_ping[2],buf_ping[3] ,size_ping));

            if(status==RDABT_NORESOURCES)
            {
                //if(ReTrySendFlag == FALSE)
                ReTrySendFlag = TRUE;
                //else
                //ReTrySendFlag = FALSE;
            }
            else
            {
                ReTrySendFlag = FALSE;
                get_len-=size_ping;

                if(seq_num==0xFFFFFFFF)
                    seq_num=0;

                seq_num++;
                g_TimeStamp += AVDTP_TIME_INTERVAL;
            }
        }
        /*if((get_len <gbuf_len*2)&&(get_len >gbuf_len*1))
            {
             if(codec->GetPayloadDone)
                       codec->GetPayloadDone();

        }*/
        if((get_len >gbuf_len)&&(!a2dptimer_stoped))
        {
            pTimerCreate_base(1,1);
            //stack_start_timer(&rdabt_p->base_timer, 1, 3);
            //evshed_set_event(rdabt_p->evsh_sched,
            //            (kal_timer_func_ptr)A2dpDataSendTimerCallback, codec, 3);
        }
        else
        {
            if(codec->GetPayloadDone)
                codec->GetPayloadDone();
            get_len=0;
        }
    }
    else if(!a2dptimer_stoped)
    {
        status=Avdtp_Send_Data(size_ping-1, buf_ping+1, g_TimeStamp, seq_num,0, 0, buf_ping[0]);
        pDebugPrintfEX((pLOGDEBUG,pLOGA2DP," sample_cnt:%d, buf_ping[0]:%d, size_ping=%d,  \n", sample_cnt, buf_ping[0], size_ping));

        if(status==RDABT_NORESOURCES)
        {
            //if(ReTrySendFlag == FALSE)
            ReTrySendFlag = TRUE;
            //else
            // ReTrySendFlag = FALSE;
        }
        else
        {
            ReTrySendFlag = FALSE;
            get_len-=size_ping;

            if(seq_num==0xFFFFFFFF)
                seq_num=0;

            seq_num++;
            g_TimeStamp += AVDTP_TIME_INTERVAL;
        }
        //stack_start_timer(&rdabt_p->base_timer, 1, 3);
        pTimerCreate_base(18,1);

    }
#endif
}



void rda_bt_a2dp_send_sep_discover_res( module_type src_mod_id, u_int16 connect_id, u_int16 result, u_int8 sep_num, bt_sep_info_struct* sep_list)
{
    if(result == BT_A2DP_RESULT_OK )
        result = BT_A2DP_MESSAGE_TYPE_ACCEPT;
    else
        result = BT_A2DP_MESSAGE_TYPE_REJECT;
    Avdtp_Send_Sep_Discover_Res(connect_id, sep_list, result, result);
}

void rda_bt_a2dp_send_capabilities_get_res( module_type src_mod_id, u_int16 connect_id, u_int16 result, u_int8 codec_type, bt_a2dp_audio_codec_cap_struct* audio_cap )
{
    if(result == BT_A2DP_RESULT_OK )
        result = BT_A2DP_MESSAGE_TYPE_ACCEPT;
    else
        result = BT_A2DP_MESSAGE_TYPE_REJECT;
    Avdtp_Send_Sep_Capabilities_Get_Res(connect_id, codec_type, audio_cap);
}

void rda_bt_a2dp_send_stream_config_res( module_type src_mod_id, u_int16 result, u_int8 stream_handle)
{

    Avdtp_Send_Sep_Stream_Config_Res(result, stream_handle,a2dp_signal_cid);
}

void rdabt_a2dp_send_stream_reconfig_res( module_type src_mod_id, u_int16 result, u_int8 stream_handle)
{
    if(result == BT_A2DP_RESULT_OK )
        result = BT_A2DP_MESSAGE_TYPE_ACCEPT;
    else
        result = BT_A2DP_MESSAGE_TYPE_REJECT;

}

void rda_bt_a2dp_send_stream_open_res( module_type src_mod_id, u_int16 result, u_int8 stream_handle )
{

    Avdtp_Send_Sep_Stream_Open_Res(result, stream_handle,a2dp_signal_cid);
}

void rda_bt_a2dp_send_stream_start_res( module_type src_mod_id, u_int16 result, u_int8 stream_handle )
{

    Avdtp_Send_Sep_Stream_Start_Res(result, stream_handle,a2dp_signal_cid);
}
void rda_bt_a2dp_send_stream_close_res( u_int8 src_mod_id, u_int16 result, u_int8 stream_handle)
{
    Avdtp_Send_Sep_Stream_Close_Res(result,stream_handle,a2dp_signal_cid);
}

void rda_bt_a2dp_send_stream_pause_res( u_int8 src_mod_id, u_int16 result, u_int8 stream_handle )
{
    Avdtp_Send_Sep_Stream_Suspend_Res(result,stream_handle,a2dp_signal_cid);
}

void rdabt_a2dp_send_stream_pause_res( module_type src_mod_id, u_int16 result, u_int8 stream_handle )
{
    if(result == BT_A2DP_RESULT_OK )
        result = BT_A2DP_MESSAGE_TYPE_ACCEPT;
    else
        result = BT_A2DP_MESSAGE_TYPE_REJECT;
    kal_prompt_trace(1,"rdabt_a2dp_send_stream_pause_res result=%x",result);
    Avdtp_Send_Sep_Stream_Suspend_Res(result,stream_handle,a2dp_signal_cid);
}

void rdabt_a2dp_send_stream_close_res( module_type src_mod_id, u_int16 result, u_int8 stream_handle)
{
    if(result == BT_A2DP_RESULT_OK )
        result = BT_A2DP_MESSAGE_TYPE_ACCEPT;
    else
        result = BT_A2DP_MESSAGE_TYPE_REJECT;

    Avdtp_Send_Sep_Stream_Close_Res(result,stream_handle,a2dp_signal_cid);
}

void rdabt_a2dp_send_stream_abort_res( module_type src_mod_id, u_int8 stream_handle )
{
    Avdtp_Send_Sep_Stream_Abort_Res(stream_handle,a2dp_signal_cid);
}



#if 0
kal_uint8 Avdtp_Check_SBC_Codecs_Compatible(bt_a2dp_sbc_codec_cap_struct local_cap, bt_a2dp_sbc_codec_cap_struct req_cfg)
{
    kal_prompt_trace(1,"SBC Local_Cap max_pool:%d, min_pool:%d, block_len:%d, subband_num:%d,alloc_method:%d, sample_rate:%d, channel_mode:%d",
                     local_cap.max_bit_pool, local_cap.min_bit_pool,local_cap.block_len,local_cap.subband_num, local_cap.alloc_method,local_cap.sample_rate, local_cap.channel_mode);

    kal_prompt_trace(1,"SBC Req_Cfg max_pool:%d, min_pool:%d, block_len:%d, subband_num:%d,alloc_method:%d, sample_rate:%d, channel_mode:%d",
                     req_cfg.max_bit_pool, req_cfg.min_bit_pool,req_cfg.block_len,req_cfg.subband_num, req_cfg.alloc_method,req_cfg.sample_rate, req_cfg.channel_mode);

    if(local_cap.min_bit_pool> req_cfg.min_bit_pool)
        return KAL_FALSE;

    //BT_TraceOutputText(1,"Avdtp_Check_SBC_Codecs_Compatible local_cap.max_pool:%d, req_cfg.max_pool:%d",local_cap.max_bit_pool, req_cfg.max_bit_pool);
    //if(local_cap.max_bit_pool < req_cfg.max_bit_pool)
    //    req_cfg.max_bit_pool = local_cap.max_bit_pool;
    //return KAL_FALSE;

    if(!(local_cap.block_len & req_cfg.block_len))
        return KAL_FALSE;

    if(!(local_cap.subband_num & req_cfg.subband_num))
        return KAL_FALSE;

    if(!(local_cap.alloc_method & req_cfg.alloc_method))
        return KAL_FALSE;

    if(!(local_cap.sample_rate & req_cfg.sample_rate))
        return KAL_FALSE;

    if(!(local_cap.channel_mode & req_cfg.channel_mode))
        return KAL_FALSE;

    return KAL_TRUE;
}

kal_uint8 Avdtp_Check_MP3_Codecs_Compatible(bt_a2dp_mp3_codec_cap_struct local_cap, bt_a2dp_mp3_codec_cap_struct req_cfg)
{
    kal_prompt_trace(1,"MP3 Local_Cap layer:%d, channel_mode:%d, sample_rate:%d, bit_rate:%d",local_cap.layer, local_cap.channel_mode,local_cap.sample_rate,local_cap.bit_rate);

    kal_prompt_trace(1,"MP3 Req_Cfg layer:%d, channel_mode:%d, sample_rate:%d, bit_rate:%d",req_cfg.layer, req_cfg.channel_mode,req_cfg.sample_rate,req_cfg.bit_rate);


    if(!(local_cap.layer & req_cfg.layer))
        return KAL_FALSE;

    if(!(local_cap.channel_mode & req_cfg.channel_mode))
        return KAL_FALSE;

    if(!(local_cap.sample_rate & req_cfg.sample_rate))
        return KAL_FALSE;

    if(!(local_cap.bit_rate & req_cfg.bit_rate))
        return KAL_FALSE;

    return KAL_TRUE;
}
#endif
kal_uint8 rda_bt_a2dp_match_audio_capabilities( bt_a2dp_audio_cap_struct *audio_cap, bt_a2dp_audio_cap_struct *audio_cfg, kal_uint16 *result )
{
    kal_uint8 status = KAL_TRUE;

    mmi_trace(g_sw_BT, "rda_bt_a2dp_match_audio_capabilities audio_cap->codec_type=%x,audio_cfg->codec_type=%x,",audio_cap->codec_type,audio_cfg->codec_type);
    if(audio_cap->codec_type != audio_cfg->codec_type)
    {
        *result = KAL_FALSE;
        return KAL_FALSE;
    }

    if(audio_cap->codec_type == BT_A2DP_SBC)
    {
        status = Avdtp_Check_SBC_Codecs_Compatible(audio_cap->codec_cap.sbc, audio_cfg->codec_cap.sbc);
    }
    else if(audio_cap->codec_type == BT_A2DP_MP3)
    {
        status = Avdtp_Check_MP3_Codecs_Compatible(audio_cap->codec_cap.mp3, audio_cfg->codec_cap.mp3);
    }
    *result = status;

    return status;
}
extern kal_uint8 mmi_bt_is_a2dp_connect(void);
U8 mmi_bt_is_avrcp_connect(void);



unsigned char is_a2dp_connect(void)
{
    if( (mmi_bt_is_a2dp_connect()) || (mmi_bt_is_avrcp_connect()) )
        return 1;
    return 0;
}




#endif // !__BT_A2DP_PROFILE__


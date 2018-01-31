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






#ifndef EXCLUDE_AV_MODULE

/* General header file */
#include "kal_release.h"        /* basic data type */
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h"         /* task message communiction */

//#include "kal_trace.h"
#include "bt_a2dp_struct.h"
#include "mmi_trace.h"
// aud_ilm.c
void send_msg( module_type src_id, module_type dst_id, kal_uint32 sap_id, kal_uint32 msg_id, void *local_param_ptr )
{
    ilm_struct *ilm_ptr = allocate_ilm(src_id);
    ilm_ptr->src_mod_id = src_id;
    ilm_ptr->dest_mod_id = dst_id;
    ilm_ptr->sap_id = sap_id;
    ilm_ptr->msg_id = (msg_type_t)msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*)local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;

    msg_send_ext_queue(ilm_ptr);
}

//#define MOD_BT MOD_BCHS

#define aud_send_msg_to_med(src_id, msg_id, local_param_ptr) send_msg( (src_id), MOD_MED, MED_SAP, (msg_id), (local_param_ptr) )
#define aud_send_msg_from_med(dst_id, msg_id, local_param_ptr) send_msg( MOD_MED, (dst_id), MED_SAP, (msg_id), (local_param_ptr) )
#define send_msg_to_bt(src_id, msg_id, local_param_ptr) send_msg( (src_id), MOD_BT, BT_A2DP_SAP, (msg_id), (local_param_ptr) )
#define send_msg_from_bt(dst_id, msg_id, local_param_ptr) send_msg( MOD_BT, (dst_id), BT_A2DP_SAP, (msg_id), (local_param_ptr) )

// BT_A2DP SAP
// activate
void bt_a2dp_send_activate_req( module_type src_mod_id, kal_uint8 local_role )
{
    bt_a2dp_activate_req_struct* msg_p = (bt_a2dp_activate_req_struct*)
                                         construct_local_para(sizeof(bt_a2dp_activate_req_struct), TD_CTRL);

    msg_p->local_role = local_role;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_ACTIVATE_REQ, msg_p );
}

/*
void bt_a2dp_send_activate_cnf( module_type dst_mod_id, kal_uint32 result )
{
    bt_a2dp_activate_cnf_struct* msg_p = (bt_a2dp_activate_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_activate_cnf_struct), TD_CTRL);

    msg_p->result = result;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_ACTIVATE_CNF, msg_p );
}
*/
// deactivate
void bt_a2dp_send_deactivate_req( module_type src_mod_id )
{
    bt_a2dp_deactivate_req_struct* msg_p = (bt_a2dp_deactivate_req_struct*)
                                           construct_local_para(sizeof(bt_a2dp_deactivate_req_struct), TD_CTRL);

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_DEACTIVATE_REQ, msg_p );
}
/*
void bt_a2dp_send_deactivate_cnf( module_type dst_mod_id, kal_uint32 result )
{
    bt_a2dp_deactivate_cnf_struct* msg_p = (bt_a2dp_deactivate_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_deactivate_cnf_struct), TD_CTRL);

    msg_p->result = result;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_DEACTIVATE_CNF, msg_p );
}
*/
// connect
void bt_a2dp_send_signal_connect_req( module_type src_mod_id, bt_device_addr_struct* device_addr, kal_uint8 local_role )
{
    bt_a2dp_signal_connect_req_struct* msg_p = (bt_a2dp_signal_connect_req_struct*)
            construct_local_para(sizeof(bt_a2dp_signal_connect_req_struct), TD_CTRL);

    memcpy( &msg_p->device_addr, device_addr, sizeof(bt_device_addr_struct) );
    msg_p->local_role = local_role;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_CONNECT_REQ, msg_p );
}
/*
void bt_a2dp_send_signal_connect_cnf( module_type dst_mod_id, kal_uint32 connect_id, kal_uint32 result, bt_device_addr_struct* device_addr )
{
    bt_a2dp_signal_connect_cnf_struct* msg_p = (bt_a2dp_signal_connect_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_signal_connect_cnf_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;
    memcpy( &msg_p->device_addr, device_addr, sizeof(bt_device_addr_struct) );

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_CONNECT_CNF, msg_p );
}
*/
/*
void bt_a2dp_send_signal_connect_ind( module_type dst_mod_id, kal_uint32 connect_id, bt_device_addr_struct* device_addr )
{
    bt_a2dp_signal_connect_ind_struct* msg_p = (bt_a2dp_signal_connect_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_signal_connect_ind_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    memcpy( &msg_p->device_addr, device_addr, sizeof(bt_device_addr_struct) );

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_CONNECT_IND, msg_p );
}
*/
// disconnect
void bt_a2dp_send_signal_disconnect_req( module_type src_mod_id, kal_uint32 connect_id )
{
    bt_a2dp_signal_disconnect_req_struct* msg_p = (bt_a2dp_signal_disconnect_req_struct*)
            construct_local_para(sizeof(bt_a2dp_signal_disconnect_req_struct), TD_CTRL);

    msg_p->connect_id = connect_id;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_REQ, msg_p );
}

/*
void bt_a2dp_send_signal_disconnect_cnf( module_type dst_mod_id, kal_uint32 connect_id, kal_uint32 result )
{
    bt_a2dp_signal_disconnect_cnf_struct* msg_p = (bt_a2dp_signal_disconnect_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_signal_disconnect_cnf_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_CNF, msg_p );
}

void bt_a2dp_send_signal_disconnect_ind( module_type dst_mod_id, kal_uint32 connect_id )
{
    bt_a2dp_signal_disconnect_ind_struct* msg_p = (bt_a2dp_signal_disconnect_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_signal_disconnect_ind_struct), TD_CTRL);

    msg_p->connect_id = connect_id;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SIGNAL_DISCONNECT_IND, msg_p );
}
*/
// discover
void bt_a2dp_send_sep_discover_req( module_type src_mod_id, kal_uint32 connect_id )
{
    bt_a2dp_sep_discover_req_struct* msg_p = (bt_a2dp_sep_discover_req_struct*)
            construct_local_para(sizeof(bt_a2dp_sep_discover_req_struct), TD_CTRL);

    msg_p->connect_id = connect_id;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SEP_DISCOVER_REQ, msg_p );
}
/*
void bt_a2dp_send_sep_discover_cnf( module_type dst_mod_id, kal_uint32 connect_id, kal_uint32 result, kal_uint8 sep_num, bt_sep_info_struct* sep_list )
{
    bt_a2dp_sep_discover_cnf_struct* msg_p = (bt_a2dp_sep_discover_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_sep_discover_cnf_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;
    if( (msg_p->sep_num = sep_num) != 0 )
    {
        if( msg_p->sep_num > MAX_NUM_REMOTE_SEIDS )
            msg_p->sep_num = MAX_NUM_REMOTE_SEIDS;
        memcpy( &msg_p->sep_list, sep_list, sizeof(bt_sep_info_struct)*msg_p->sep_num );
    }

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SEP_DISCOVER_CNF, msg_p );
}
*/
/*
void bt_a2dp_send_sep_discover_ind( module_type dst_mod_id, kal_uint32 connect_id )
{
    bt_a2dp_sep_discover_ind_struct* msg_p = (bt_a2dp_sep_discover_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_sep_discover_ind_struct), TD_CTRL);

    msg_p->connect_id = connect_id;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SEP_DISCOVER_IND, msg_p );
}
*/
void bt_a2dp_send_sep_discover_res( module_type src_mod_id, kal_uint32 connect_id, kal_uint32 result, kal_uint8 sep_num, bt_sep_info_struct* sep_list )
{
    bt_a2dp_sep_discover_res_struct* msg_p = (bt_a2dp_sep_discover_res_struct*)
            construct_local_para(sizeof(bt_a2dp_sep_discover_res_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;
    if( (msg_p->sep_num = sep_num) != 0 )
    {
        if( msg_p->sep_num > MAX_NUM_REMOTE_SEIDS )
            msg_p->sep_num = MAX_NUM_REMOTE_SEIDS;
        memcpy( &msg_p->sep_list, sep_list, sizeof(bt_sep_info_struct)*msg_p->sep_num );
    }

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_SEP_DISCOVER_RES, msg_p );
}

// get capabilities
void bt_a2dp_send_capabilities_get_req( module_type src_mod_id, kal_uint32 connect_id, kal_uint8 acp_seid )
{
    bt_a2dp_capabilities_get_req_struct* msg_p = (bt_a2dp_capabilities_get_req_struct*)
            construct_local_para(sizeof(bt_a2dp_capabilities_get_req_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->acp_seid = acp_seid;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_CAPABILITIES_GET_REQ, msg_p );
}
/*
void bt_a2dp_send_capabilities_get_cnf( module_type dst_mod_id, kal_uint32 connect_id, kal_uint32 result, kal_uint8 audio_cap_num, bt_a2dp_audio_cap_struct* audio_cap_list )
{
    bt_a2dp_capabilities_get_cnf_struct* msg_p = (bt_a2dp_capabilities_get_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_capabilities_get_cnf_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;
    if( (msg_p->audio_cap_num = audio_cap_num) != 0 )
    {
        if( msg_p->audio_cap_num > MAX_NUM_REMOTE_CAPABILITIES )
            msg_p->audio_cap_num = MAX_NUM_REMOTE_CAPABILITIES;
        memcpy( &msg_p->audio_cap_list, audio_cap_list, sizeof(bt_a2dp_audio_cap_struct)*msg_p->audio_cap_num );
    }

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_CAPABILITIES_GET_CNF, msg_p );
}
*/
/*
void bt_a2dp_send_capabilities_get_ind( module_type dst_mod_id, kal_uint32 connect_id, kal_uint8 acp_seid )
{
    bt_a2dp_capabilities_get_ind_struct* msg_p = (bt_a2dp_capabilities_get_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_capabilities_get_ind_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->acp_seid = acp_seid;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_CAPABILITIES_GET_IND, msg_p );
}
*/
void bt_a2dp_send_capabilities_get_res( module_type src_mod_id, kal_uint32 connect_id, kal_uint32 result, kal_uint8 audio_cap_num, bt_a2dp_audio_cap_struct* audio_cap_list )
{
    bt_a2dp_capabilities_get_res_struct* msg_p = (bt_a2dp_capabilities_get_res_struct*)
            construct_local_para(sizeof(bt_a2dp_capabilities_get_res_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;
    if( (msg_p->audio_cap_num = audio_cap_num) != 0 )
    {
        if( msg_p->audio_cap_num > MAX_NUM_REMOTE_CAPABILITIES )
            msg_p->audio_cap_num = MAX_NUM_REMOTE_CAPABILITIES;
        memcpy( &msg_p->audio_cap_list, audio_cap_list, sizeof(bt_a2dp_audio_cap_struct)*msg_p->audio_cap_num );
    }

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_CAPABILITIES_GET_RES, msg_p );
}

// set configuration
void bt_a2dp_send_stream_config_req( module_type src_mod_id, kal_uint32 connect_id, kal_uint8 acp_seid, kal_uint8 int_seid, bt_a2dp_audio_cap_struct* audio_cap )
{
    bt_a2dp_stream_config_req_struct* msg_p = (bt_a2dp_stream_config_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_config_req_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->acp_seid = acp_seid;
    msg_p->int_seid = int_seid;
    memcpy( &msg_p->audio_cap, audio_cap, sizeof(bt_a2dp_audio_cap_struct) );

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CONFIG_REQ, msg_p );
}

/*
void bt_a2dp_send_stream_config_cnf( module_type dst_mod_id, kal_uint32 connect_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_config_cnf_struct* msg_p = (bt_a2dp_stream_config_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_config_cnf_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CONFIG_CNF, msg_p );
}
*/
/*
void bt_a2dp_send_stream_config_ind( module_type dst_mod_id, kal_uint32 connect_id, kal_uint8 acp_seid, kal_uint8 int_seid, kal_uint8 stream_handle, bt_a2dp_audio_cap_struct* audio_cap )
{
    bt_a2dp_stream_config_ind_struct* msg_p = (bt_a2dp_stream_config_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_config_ind_struct), TD_CTRL);

    msg_p->connect_id = connect_id;
    msg_p->acp_seid = acp_seid;
    msg_p->int_seid = int_seid;
    msg_p->stream_handle = stream_handle;
    memcpy( &msg_p->audio_cap, audio_cap, sizeof(bt_a2dp_audio_cap_struct) );

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CONFIG_IND, msg_p );
}
*/
void bt_a2dp_send_stream_config_res( module_type src_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_config_res_struct* msg_p = (bt_a2dp_stream_config_res_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_config_res_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CONFIG_RES, msg_p );
}

// reconfigure
void bt_a2dp_send_stream_reconfig_req( module_type src_mod_id,kal_uint16 connect_id, kal_uint8 acp_seid, bt_a2dp_audio_cap_struct* audio_cap )
{
    bt_a2dp_stream_reconfig_req_struct* msg_p = (bt_a2dp_stream_reconfig_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_reconfig_req_struct), TD_CTRL);


    msg_p->connect_id = connect_id;
    msg_p->acp_seid = acp_seid;
    memcpy( &msg_p->audio_cap, audio_cap, sizeof(bt_a2dp_audio_cap_struct) );
    mmi_trace(g_sw_BT,"bt_a2dp_send_stream_reconfig_req msg_p->audio_cap.codec_type=%d",msg_p->audio_cap.codec_type);
    /*mmi_trace(g_sw_BT,"bt_a2dp_send_stream_reconfig_req reconfig_req MP3 av_bt_ctx.cfg,bit_rate=%d,channel_mode=%d, CRC=%d, layer=%d, MPF=%d,sample_rate=%d, VBR=%d,",\
    msg_p->audio_cap.codec_cap.mp3.bit_rate,
    msg_p->audio_cap.codec_cap.mp3.channel_mode,
    msg_p->audio_cap.codec_cap.mp3.CRC,
    msg_p->audio_cap.codec_cap.mp3.layer,
    msg_p->audio_cap.codec_cap.mp3.MPF,
    msg_p->audio_cap.codec_cap.mp3.sample_rate,
    msg_p->audio_cap.codec_cap.mp3.VBR);*/
    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ, msg_p );
}

void bt_a2dp_send_stream_reconfig_cnf( module_type dst_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_reconfig_cnf_struct* msg_p = (bt_a2dp_stream_reconfig_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_reconfig_cnf_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_RECONFIG_CNF, msg_p );
}

void bt_a2dp_send_stream_reconfig_ind( module_type dst_mod_id, kal_uint8 stream_handle, bt_a2dp_audio_cap_struct* audio_cap )
{
    bt_a2dp_stream_reconfig_ind_struct* msg_p = (bt_a2dp_stream_reconfig_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_reconfig_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;
    memcpy( &msg_p->audio_cap, audio_cap, sizeof(bt_a2dp_audio_cap_struct) );

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_RECONFIG_IND, msg_p );
}

void bt_a2dp_send_stream_reconfig_res( module_type src_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_reconfig_res_struct* msg_p = (bt_a2dp_stream_reconfig_res_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_reconfig_res_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_RECONFIG_RES, msg_p );
}

// open
void bt_a2dp_send_stream_open_req( module_type src_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_open_req_struct* msg_p = (bt_a2dp_stream_open_req_struct*)
                                            construct_local_para(sizeof(bt_a2dp_stream_open_req_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_OPEN_REQ, msg_p );
}
/*
void bt_a2dp_send_stream_open_cnf( module_type dst_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_open_cnf_struct* msg_p = (bt_a2dp_stream_open_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_open_cnf_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_OPEN_CNF, msg_p );
}

void bt_a2dp_send_stream_open_ind( module_type dst_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_open_ind_struct* msg_p = (bt_a2dp_stream_open_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_open_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_OPEN_IND, msg_p );
}
*/
void bt_a2dp_send_stream_open_res( module_type src_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_open_res_struct* msg_p = (bt_a2dp_stream_open_res_struct*)
                                            construct_local_para(sizeof(bt_a2dp_stream_open_res_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_OPEN_RES, msg_p );
}

// start
void bt_a2dp_send_stream_start_req( module_type src_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_start_req_struct* msg_p = (bt_a2dp_stream_start_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_start_req_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_START_REQ, msg_p );
}
/*
void bt_a2dp_send_stream_start_cnf( module_type dst_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_start_cnf_struct* msg_p = (bt_a2dp_stream_start_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_start_cnf_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_START_CNF, msg_p );
}

void bt_a2dp_send_stream_start_ind( module_type dst_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_start_ind_struct* msg_p = (bt_a2dp_stream_start_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_start_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_START_IND, msg_p );
}
*/
void bt_a2dp_send_stream_start_res( module_type src_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_start_res_struct* msg_p = (bt_a2dp_stream_start_res_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_start_res_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_START_RES, msg_p );
}

// pause
void bt_a2dp_send_stream_pause_req( module_type src_mod_id, kal_uint8 stream_handle, A2DP_codec_struct* codec )
{
    bt_a2dp_stream_pause_req_struct* msg_p = (bt_a2dp_stream_pause_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_pause_req_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;
    msg_p->codec = codec;
    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_PAUSE_REQ, msg_p );
}
/*
void bt_a2dp_send_stream_pause_cnf( module_type dst_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_pause_cnf_struct* msg_p = (bt_a2dp_stream_pause_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_pause_cnf_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_PAUSE_CNF, msg_p );
}

void bt_a2dp_send_stream_pause_ind( module_type dst_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_pause_ind_struct* msg_p = (bt_a2dp_stream_pause_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_pause_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_PAUSE_IND, msg_p );
}
*/
void bt_a2dp_send_stream_pause_res( module_type src_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_pause_res_struct* msg_p = (bt_a2dp_stream_pause_res_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_pause_res_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_PAUSE_RES, msg_p );
}

// qos
void bt_a2dp_send_stream_qos_ind( module_type dst_mod_id, kal_uint8 stream_handle, kal_uint8 qos )
{
    bt_a2dp_stream_qos_ind_struct* msg_p = (bt_a2dp_stream_qos_ind_struct*)
                                           construct_local_para(sizeof(bt_a2dp_stream_qos_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;
    msg_p->qos = qos;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_QOS_IND, msg_p );
}

// send data
void bt_a2dp_send_stream_data_send_req( module_type src_mod_id, kal_uint8 stream_handle, A2DP_codec_struct* codec )
{
    bt_a2dp_stream_data_send_req_struct* msg_p = (bt_a2dp_stream_data_send_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_data_send_req_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    //memcpy( &msg_p->codec, codec, sizeof(A2DP_codec_struct) ); YS fixed
    msg_p->codec = codec;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ, msg_p );
}

// close
void bt_a2dp_send_stream_close_req( module_type src_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_close_req_struct* msg_p = (bt_a2dp_stream_close_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_close_req_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CLOSE_REQ, msg_p );
}
/*
void bt_a2dp_send_stream_close_cnf( module_type dst_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_close_cnf_struct* msg_p = (bt_a2dp_stream_close_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_close_cnf_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CLOSE_CNF, msg_p );
}

void bt_a2dp_send_stream_close_ind( module_type dst_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_close_ind_struct* msg_p = (bt_a2dp_stream_close_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_close_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CLOSE_IND, msg_p );
}
*/
void bt_a2dp_send_stream_close_res( module_type src_mod_id, kal_uint32 result, kal_uint8 stream_handle )
{
    bt_a2dp_stream_close_res_struct* msg_p = (bt_a2dp_stream_close_res_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_close_res_struct), TD_CTRL);

    msg_p->result = result;
    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_CLOSE_RES, msg_p );
}

// abort
void bt_a2dp_send_stream_abort_req( module_type src_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_abort_req_struct* msg_p = (bt_a2dp_stream_abort_req_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_abort_req_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_ABORT_REQ, msg_p );
}
/*
void bt_a2dp_send_stream_abort_cnf( module_type dst_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_abort_cnf_struct* msg_p = (bt_a2dp_stream_abort_cnf_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_abort_cnf_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_ABORT_CNF, msg_p );
}

void bt_a2dp_send_stream_abort_ind( module_type dst_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_abort_ind_struct* msg_p = (bt_a2dp_stream_abort_ind_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_abort_ind_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_ABORT_IND, msg_p );
}
*/
void bt_a2dp_send_stream_abort_res( module_type src_mod_id, kal_uint8 stream_handle )
{
    bt_a2dp_stream_abort_res_struct* msg_p = (bt_a2dp_stream_abort_res_struct*)
            construct_local_para(sizeof(bt_a2dp_stream_abort_res_struct), TD_CTRL);

    msg_p->stream_handle = stream_handle;

    send_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_A2DP_STREAM_ABORT_RES, msg_p );
}

#endif


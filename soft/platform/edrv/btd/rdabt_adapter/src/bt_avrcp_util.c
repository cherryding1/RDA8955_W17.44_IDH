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

#ifndef EXCLUDE_AVRCP_MODULE

/* General header file */
#include "kal_release.h"        /* basic data type */
//#include "stack_common.h"
//#include "stack_msgs.h"
#include "app_ltlcom.h"         /* task message communiction */

//#include "kal_trace.h"
#include "mmi_trace.h"
#include "bt_avrcp_struct.h"


void send_avrcp_msg( module_type src_id, module_type dst_id, kal_uint32 sap_id, kal_uint32 msg_id, void *local_param_ptr, void *peer_buff_ptr )
{
    ilm_struct *ilm_ptr = allocate_ilm(src_id);
    ilm_ptr->src_mod_id = src_id;
    ilm_ptr->dest_mod_id = dst_id;
    ilm_ptr->sap_id = sap_id;
    ilm_ptr->msg_id = (msg_type_t)msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*)local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) peer_buff_ptr;

    msg_send_ext_queue(ilm_ptr);
}

//#define MOD_BT MOD_BCHS

#define send_avrcp_msg_to_bt(src_id, msg_id, local_param_ptr, peer_buff_ptr) send_avrcp_msg( (src_id), MOD_BT, BT_AVRCP_SAP, (msg_id), (local_param_ptr), (peer_buff_ptr) )
#define send_avrcp_msg_from_bt(dst_id, msg_id, local_param_ptr, peer_buff_ptr) send_avrcp_msg( MOD_BT, (dst_id), BT_AVRCP_SAP, (msg_id), (local_param_ptr), (peer_buff_ptr) )

// activate
void bt_avrcp_send_activate_req( module_type src_mod_id, kal_uint8 local_role )
{
    bt_avrcp_activate_req_struct* msg_p = (bt_avrcp_activate_req_struct*)
                                          construct_local_para(sizeof(bt_avrcp_activate_req_struct), TD_CTRL);

    msg_p->local_role = local_role;

    send_avrcp_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_ACTIVATE_REQ, msg_p, NULL );
}

void bt_avrcp_send_activate_cnf( module_type dst_mod_id, kal_uint16 result )
{
    bt_avrcp_activate_cnf_struct* msg_p = (bt_avrcp_activate_cnf_struct*)
                                          construct_local_para(sizeof(bt_avrcp_activate_cnf_struct), TD_CTRL);

    msg_p->result = result;

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_ACTIVATE_CNF, msg_p, NULL );
}

// deactivate
void bt_avrcp_send_deactivate_req( module_type src_mod_id ,U8 avrcp_chnl)
{
    bt_avrcp_deactivate_req_struct* msg_p = (bt_avrcp_deactivate_req_struct*)
                                            construct_local_para(sizeof(bt_avrcp_deactivate_req_struct), TD_CTRL);
    msg_p->chnl_num = avrcp_chnl;
    send_avrcp_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_DEACTIVATE_REQ, msg_p, NULL );
}

void bt_avrcp_send_deactivate_cnf( module_type dst_mod_id, kal_uint16 result )
{
    bt_avrcp_deactivate_cnf_struct* msg_p = (bt_avrcp_deactivate_cnf_struct*)
                                            construct_local_para(sizeof(bt_avrcp_deactivate_cnf_struct), TD_CTRL);

    msg_p->result = result;

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_DEACTIVATE_CNF, msg_p, NULL );
}

// connect
void bt_avrcp_send_connect_req( module_type src_mod_id, kal_uint8 chnl_num, bt_avrcp_dev_addr_struct* device_addr, kal_uint8 local_role )
{
    bt_avrcp_connect_req_struct* msg_p = (bt_avrcp_connect_req_struct*)
                                         construct_local_para(sizeof(bt_avrcp_connect_req_struct), TD_CTRL);

    msg_p->local_role = local_role;
    memcpy( &msg_p->device_addr, device_addr, sizeof(bt_avrcp_dev_addr_struct) );

    send_avrcp_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CONNECT_REQ, msg_p, NULL );
}

void bt_avrcp_send_connect_cnf( module_type dst_mod_id, kal_uint16 result, bt_avrcp_dev_addr_struct* device_addr )
{
    bt_avrcp_connect_cnf_struct* msg_p = (bt_avrcp_connect_cnf_struct*)
                                         construct_local_para(sizeof(bt_avrcp_connect_cnf_struct), TD_CTRL);

    msg_p->result = result;
    memcpy( &msg_p->device_addr, device_addr, sizeof(bt_avrcp_dev_addr_struct) );

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CONNECT_CNF, msg_p, NULL );
}

void bt_avrcp_send_connect_ind( module_type dst_mod_id, bt_avrcp_dev_addr_struct* device_addr )
{
    bt_avrcp_connect_ind_struct* msg_p = (bt_avrcp_connect_ind_struct*)
                                         construct_local_para(sizeof(bt_avrcp_connect_ind_struct), TD_CTRL);

    memcpy( &msg_p->device_addr, device_addr, sizeof(bt_avrcp_dev_addr_struct) );

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CONNECT_IND, msg_p, NULL );
}

// disconnect
void bt_avrcp_send_disconnect_req( module_type src_mod_id)
{
    bt_avrcp_disconnect_req_struct* msg_p = (bt_avrcp_disconnect_req_struct*)
                                            construct_local_para(sizeof(bt_avrcp_disconnect_req_struct), TD_CTRL);

    send_avrcp_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_DISCONNECT_REQ, msg_p, NULL );
}

void bt_avrcp_send_disconnect_cnf( module_type dst_mod_id, kal_uint16 result )
{
    bt_avrcp_disconnect_cnf_struct* msg_p = (bt_avrcp_disconnect_cnf_struct*)
                                            construct_local_para(sizeof(bt_avrcp_disconnect_cnf_struct), TD_CTRL);

    msg_p->result = result;

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_DISCONNECT_CNF, msg_p, NULL );
}

void bt_avrcp_send_disconnect_ind( module_type dst_mod_id)
{
    bt_avrcp_disconnect_ind_struct* msg_p = (bt_avrcp_disconnect_ind_struct*)
                                            construct_local_para(sizeof(bt_avrcp_disconnect_ind_struct), TD_CTRL);

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_DISCONNECT_IND, msg_p, NULL );
}

//command frame
void bt_avrcp_send_cmd_frame_req( module_type src_mod_id, kal_uint8 seq_id, kal_uint8 c_type, kal_uint8 subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8* frame_data)
{
    bt_avrcp_cmd_frame_req_struct* msg_p = (bt_avrcp_cmd_frame_req_struct*)
                                           construct_local_para(sizeof(bt_avrcp_cmd_frame_req_struct), TD_CTRL);

    msg_p->seq_id = seq_id;
    msg_p->c_type = c_type;
    msg_p->subunit_type = subunit_type;
    msg_p->subunit_id = subunit_id;
    msg_p->data_len = data_len;

    if((data_len > 0) && (frame_data != NULL))
    {
        kal_mem_cpy(msg_p->frame_data, frame_data, data_len);
    }

    send_avrcp_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CMD_FRAME_REQ, msg_p, NULL );
}

void bt_avrcp_send_cmd_frame_cnf( module_type dst_mod_id, kal_uint16 type, kal_uint8 seq_id, kal_bool invalid_profile,  kal_uint8   c_type, kal_uint8   subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8 *frame_data)
{
    bt_avrcp_cmd_frame_cnf_struct* msg_p = (bt_avrcp_cmd_frame_cnf_struct*)
                                           construct_local_para(sizeof(bt_avrcp_cmd_frame_cnf_struct), TD_CTRL);

    //msg_p->type = type;
    msg_p->seq_id = seq_id;
    msg_p->c_type = c_type;
    msg_p->subunit_type = subunit_type;
    msg_p->subunit_id = subunit_id;
    msg_p->data_len = data_len;
//  msg_p->invalid_profile = invalid_profile;

    if((data_len > 0) && (frame_data != NULL))
    {
        kal_mem_cpy(msg_p->frame_data, frame_data, data_len);
    }

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CMD_FRAME_CNF, msg_p, NULL );
}

void bt_avrcp_send_cmd_frame_ind( module_type dst_mod_id, kal_uint8 seq_id, kal_uint16 profile_id,  kal_uint8   c_type, kal_uint8   subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8 *frame_data)
{
    bt_avrcp_cmd_frame_ind_struct* msg_p = (bt_avrcp_cmd_frame_ind_struct*)
                                           construct_local_para(sizeof(bt_avrcp_cmd_frame_ind_struct), TD_CTRL);

    msg_p->seq_id = seq_id;
    msg_p->profile_id = profile_id;
    msg_p->c_type = c_type;
    msg_p->subunit_type = subunit_type;
    msg_p->subunit_id = subunit_id;
    msg_p->data_len = data_len;

    if((data_len > 0) && (frame_data != NULL))
    {
        kal_mem_cpy(msg_p->frame_data, frame_data, data_len);
    }

    send_avrcp_msg_from_bt( dst_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CMD_FRAME_IND, msg_p, NULL );
}

void bt_avrcp_send_cmd_frame_ind_res( module_type src_mod_id, kal_uint8 seq_id, kal_uint16 profile_id,kal_uint8 c_type, kal_uint8 subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8* frame_data)
{
    bt_avrcp_cmd_frame_ind_res_struct* msg_p = (bt_avrcp_cmd_frame_ind_res_struct*)
            construct_local_para(sizeof(bt_avrcp_cmd_frame_ind_res_struct), TD_CTRL);

    msg_p->seq_id = seq_id;
    msg_p->profile_id  = profile_id;
    msg_p->c_type = c_type;
    msg_p->subunit_type = subunit_type;
    msg_p->subunit_id = subunit_id;
    msg_p->data_len = data_len;

    if((data_len > 0) && (frame_data != NULL))
    {
        kal_mem_cpy(msg_p->frame_data, frame_data, data_len);
    }

    send_avrcp_msg_to_bt( src_mod_id, (kal_uint32)MSG_ID_BT_AVRCP_CMD_FRAME_IND_RES, msg_p, NULL );
}

#endif


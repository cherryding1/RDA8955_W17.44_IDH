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

#ifndef _RDA_BT_A2DP_H_
#define _RDA_BT_A2DP_H_

//#include "..\rdabt\user\bt.h"
//#include "..\..\platform\edrv\btd\rda_bt\include\bt.h"
#include "bt.h"
// activate
void rda_bt_a2dp_send_activate_req( kal_uint8 src_mod_id, kal_uint8 local_role );

// deactivate
void rda_bt_a2dp_send_deactivate_req( kal_uint8 src_mod_id );

// connect
u_int8 rda_bt_a2dp_send_signal_connect_req( kal_uint8 src_mod_id, t_bdaddr device_addr, kal_uint8 local_role);

// disconnect
void rda_bt_a2dp_send_signal_disconnect_req( kal_uint8 src_mod_id, kal_uint16 connect_id );

// discover
void rda_bt_a2dp_send_sep_discover_req( kal_uint8 src_mod_id, kal_uint16 connect_id  );
void rda_bt_a2dp_send_sep_discover_res( kal_uint8 src_mod_id, kal_uint16 connect_id, kal_uint16 result, kal_uint8 sep_num, bt_sep_info_struct* sep_list);

// get capabilities
void rda_bt_a2dp_send_capabilities_get_req( kal_uint8 src_mod_id, kal_uint16 connect_id, kal_uint8 acp_seid );
void rda_bt_a2dp_send_capabilities_get_res( kal_uint8 src_mod_id, kal_uint16 connect_id, kal_uint16 result, kal_uint8 codec_type, bt_a2dp_audio_codec_cap_struct* audio_cap );

// set configuration
void rda_bt_a2dp_send_stream_config_req(kal_uint16 connect_id, kal_uint8 acp_seid, kal_uint8 int_seid, kal_uint8 codec_type, bt_a2dp_audio_codec_cap_struct* audio_cap );
void rda_bt_a2dp_send_stream_config_res( kal_uint8 src_mod_id, kal_uint16 result, kal_uint8 stream_handle);

// reconfigure
void rda_bt_a2dp_send_stream_reconfig_req(u_int16 connect_id, u_int8 acp_seid, u_int8 codec_type, bt_a2dp_audio_codec_cap_struct* audio_cap );

void rda_bt_a2dp_send_stream_reconfig_res( kal_uint8 src_mod_id, kal_uint16 result, kal_uint8 stream_handle);

// open
void rda_bt_a2dp_send_stream_open_req( kal_uint8 src_mod_id, kal_uint8 stream_handle );
void rda_bt_a2dp_send_stream_open_res( kal_uint8 src_mod_id, kal_uint16 result, kal_uint8 stream_handle );

// start
void rda_bt_a2dp_send_stream_start_req( kal_uint8 src_mod_id, kal_uint8 stream_handle );
void rda_bt_a2dp_send_stream_start_res( kal_uint8 src_mod_id, kal_uint16 result, kal_uint8 stream_handle );

// pause
void rda_bt_a2dp_send_stream_pause_req( kal_uint8 src_mod_id, kal_uint8 stream_handle, A2DP_codec_struct* codec  );
void rda_bt_a2dp_send_stream_pause_res( kal_uint8 src_mod_id, kal_uint16 result, kal_uint8 stream_handle );

// qos
//void rda_bt_a2dp_send_stream_qos_ind( kal_uint8 dst_mod_id, kal_uint8 stream_handle, kal_uint8 qos );

// send data
void rda_bt_a2dp_send_stream_data_send_req( kal_uint8 src_mod_id, kal_uint8 stream_handle, A2DP_codec_struct* codec );

// close
void rda_bt_a2dp_send_stream_close_req( kal_uint8 src_mod_id, kal_uint8 stream_handle);
void rda_bt_a2dp_send_stream_close_res( kal_uint8 src_mod_id, kal_uint16 result, kal_uint8 stream_handle);

// abort
void rda_bt_a2dp_send_stream_abort_req( kal_uint8 src_mod_id, kal_uint8 stream_handle );
void rda_bt_a2dp_send_stream_abort_res( kal_uint8 src_mod_id, kal_uint8 stream_handle );


#endif


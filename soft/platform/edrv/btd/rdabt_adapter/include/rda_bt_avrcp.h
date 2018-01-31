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

#ifndef _RDA_BT_AVRCP_H_
#define _RDA_BT_AVRCP_H_

//#include "..\rdabt\user\bt.h"
#include "bt.h"
typedef struct
{
    kal_uint8   chnl_num;
    kal_uint8   seq_id;
    kal_uint8   c_type;
    kal_uint8   subunit_type;
    kal_uint8   subunit_id;
    kal_uint16  data_len;
    kal_uint16  profile_id;
    kal_uint8   frame_data[20];
} t_avrcp_cmd_frame_ind;

typedef struct
{
    kal_uint8 chnl_num;
    kal_uint8 connect_id;
    t_bdaddr device_addr;
} t_avrcp_connect_ind;

typedef struct
{
    kal_uint8 chnl_num;
    kal_uint8 connect_id;
    kal_uint16 result;
    t_bdaddr device_addr;
} t_avrcp_connect_cnf;

typedef struct
{
    kal_uint8 chnl_num;
    kal_uint8 connect_id;
} t_avrcp_disconnect_ind;

typedef struct
{
    kal_uint8 chnl_num;
    kal_uint8 connect_id;
    kal_uint16 result;
} t_avrcp_disconnect_cnf;

// activate
void rda_bt_avrcp_send_activate_req(kal_uint8 chnl_num, kal_uint8 local_role);

// deactivate
void rda_bt_avrcp_send_deactivate_req( kal_uint8 chnl_num);

// connect
void rda_bt_avrcp_send_connect_req( kal_uint8 chnl_num, t_bdaddr device_addr, kal_uint8 local_role );

// disconnect
void rda_bt_avrcp_send_disconnect_req( kal_uint8 chnl_num );

//command frame
void rda_bt_avrcp_send_cmd_frame_req( kal_uint8 chnl_num, kal_uint8 seq_id, kal_uint8 c_type, kal_uint8 subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8* frame_data);
void rda_bt_avrcp_send_cmd_frame_ind_res(kal_uint8 chnl_num, kal_uint8 seq_id, kal_uint16 profile_id,kal_uint8 result, kal_uint8 subunit_type, kal_uint8 subunit_id, kal_uint16 data_len, kal_uint8* frame_data );


#endif /* #ifndef _RDA_BT_AVRCP_H_ */


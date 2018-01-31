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


#ifndef _RDABT_AVRCP_H_
#define  _RDABT_AVRCP_H_

void rdabt_avrcp_activate_cb(void * pArgs);
void rdabt_avrcp_deactivate_cb(void * pArgs);
void rdabt_avrcp_connect_ind_cb(void * pArgs);
void rdabt_avrcp_connect_cnf_cb(void * pArgs);
void rdabt_avrcp_disconnect_ind_cb(void * pArgs);
void rdabt_avrcp_disconnect_cnf_cb(void * pArgs);
void rdabt_avrcp_cmd_frame_cnf_cb(void * pArgs);
void rdabt_avrcp_cmd_frame_ind_cb(void * pArgs);
void rdabt_avrcp_msg_hdlr(ilm_struct *message);
void rdabt_avrcp_int_msg_hdlr( kal_uint16 mi,void *mv );


#endif

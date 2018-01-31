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




#ifndef _RDABT_MGR_H_
#define _RDABT_MGR_H_


void rdabt_mgr_msg_hdler(ilm_struct *ext_message);

void rdabt_mgr_int_msg_hdlr( u_int16 mi,void *mv );

#ifdef __SUPPORT_BT_PHONE__
void rdabt_mgr_connect_sco(module_type module, t_bdaddr address, u_int16 cid);
void rdabt_mgr_disconnect_sco(module_type module, t_bdaddr address, u_int16 cid,u_int8 speed_on);
#endif
void load_paired_device_info(void);

#ifdef __MMI_BT_SIMPLE_PAIR__
void rdabt_ssp_peerkey_ind(void *data);
void rdabt_ssp_dhkey_process(void);
#endif
int rdabt_get_device_authriose_info(t_bdaddr address);
#endif // _RDABT_ADP_MGR_H_


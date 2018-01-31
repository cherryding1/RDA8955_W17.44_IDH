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

#ifndef _RDA_BT_HFG_H_
#define _RDA_BT_HFG_H_

//#include "..\rdabt\user\bt.h"

//#include "..\..\platform\edrv\btd\rda_bt\include\bt.h"
//#include "bt.h"
// activate
void rda_bt_hfg_send_activate_req(void);

// deactivate
void rda_bt_hfg_send_deactivate_req(void);

// connect
void rda_bt_hfg_send_connect_req(kal_uint8 chn_num, t_bdaddr device_addr );

// disconnect
void rda_bt_hfg_send_disconnect_req(kal_uint32 cid, t_bdaddr device_addr);

void rda_bt_hfg_send_sco_connect_req( kal_uint16 connect_id);

void rda_bt_hfg_send_sco_disconnect_req( kal_uint16 connect_id);

// set  HF volume
void rda_bt_hfg_send_set_volume_req( kal_uint16 connect_id,kal_uint8 gain /*,void (*callbackFunc)(void *pArgs)*/ );

void rda_bt_register_BLDN_req(void /*(*callbackFunc)(void *pArgs)*/ );

void rda_bt_hfg_send_ring();

void rda_bt_hfg_send_reject_cfm();//syzhou added,use to cancel ringtimer,send CIEV
//void rda_bt_hfg_send_CallSetup_1_ind(void);
void rda_bt_hfg_send_BldnFail_ind();
#endif


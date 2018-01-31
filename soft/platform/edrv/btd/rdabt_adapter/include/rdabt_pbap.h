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

#ifndef RDABT_PBAP_H
#define RDABT_PBAP_H
#ifdef __SUPPORT_BT_SYNC_PHB__
#include "bt_dm_struct.h"
#include "bt_hfg_struct.h"

typedef struct
{
    char name[PB_LENGTH];
    char time[PB_LENGTH];
    char tel[PB_LENGTH];
    char status;
} History_info;
enum
{
    PBAP_DIALED_CALL,
    PBAP_RECEIVED_CALL,
    PBAP_MISSED_CALL
};


void rdabt_pbap_data_ind_callback(obex_cli_data_ind_msg_t *pbap_data_message);
void pbap_phone_book_con_req(t_bdaddr addr,u_int8 con_type);
void pbap_discon_req(void);
#endif
#endif


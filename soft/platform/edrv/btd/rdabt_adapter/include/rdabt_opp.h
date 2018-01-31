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




#ifndef _RDA_OPP_H
#define _RDA_OPP_H
#include "uart_sw.h"
#include "bluetooth_struct.h"

/*****************************************************************************
* Define
*****************************************************************************/

#define MGR_HCI_CONNREJECT_REASON 0x0D /* Reason code used for automatic rejection - 0x0d=limited resources*/
#define RDA_FTPS_Server_Channel 0x03 /*  */
#define RDA_OPPS_Server_Channel 0x04 /*  */
#define Timer_Re_Out_CREDITS 0x20 /*  */

/*****************************************************************************
* Typedef
*****************************************************************************/

typedef enum
{
    GOEP_Client_Idle,            /* GOEP Initial State */
    GOEP_Client_Connectting,
    GOEP_Client_Connect,        /* Connection established state */
    GOEP_Client_Disconnect,   /* GOEP is disconnecting the connection == GOEP_Client_active_rfcomm,*/
    GOEP_Client_pending_rfcomm,
    GOEP_Client_pending_sdp,
    GOEP_Client_wait,
    GOEP_Client_pending_Disconnect,
} mmi_GOEP_state_enum;



////////////////opp client//////////////////////
typedef struct
{
    kal_uint32 goep_tidOPPC;  /* goep_tid of bt */
    kal_uint32 goep_tidOPPC_mmi;    /* goep_tid of mmi */
    kal_uint32 req_idOPPC;
    t_bdaddr OPPCaddress;
    kal_uint8 oppCstatus;

    kal_uint8 current_FirstPkt;
    kal_uint8 during_put;
} rdabt_oppc_struct;

////////////////ftp client//////////////////////
typedef struct
{
    kal_uint32 goep_tidFTPC;  /* goep_tid of bt */
    kal_uint32 goep_tidFTPC_mmi;    /* goep_tid of mmi */
    kal_uint32 req_idFTPC;
    t_bdaddr FTPCaddress;
    kal_uint8 ftpCstatus;
    u_int8 first;
    u_int32 total_len;
} rdabt_ftpc_struct;

////////////////opp server//////////////////////
typedef struct
{
    kal_uint32 goep_tid_OPPS;  /* goep_tid of bt */
    kal_uint32 req_idOPPS;    /* goep_tid of mmi */
    kal_uint32 OPPStid;
    kal_uint8* buf_ptrOPPS;
    kal_uint16 buf_sizeOPPS;
    t_bdaddr OPPSaddress;
    kal_uint8 opps_obj_name[80];
    kal_uint16 OPPSmax_len;
    kal_uint16 OPPSoffsetVar;
    kal_uint8 OPPSstatus;
    kal_uint8 OPPSdlci;
    kal_uint8 NAME_obex;
    kal_uint8 OPPSfirst;
} rdabt_opps_struct;

////////////////ftp server//////////////////////
typedef struct
{
    kal_uint32 goep_tid_FTPS;    /* goep_tid of mmi */
    kal_uint32 req_idFTPS;
    kal_uint32 FTPStid;
    kal_uint8* buf_ptrFTPS;
    kal_uint16 buf_sizeFTPS;
    t_bdaddr FTPSaddress;
    kal_uint8 ftps_objs_mime_type[80];
    kal_uint8 ftps_obj_name[512];
    kal_uint16 FTPSmax_len;
    kal_uint16 FTPSoffsetVar;
    kal_uint16 FTPS_offset_len;
    kal_uint8 FTPSstatus;
    kal_uint8 FTPSfirst;
    kal_uint8 FTPSdlci;
} rdabt_ftps_struct;


////////////////opp/ftp client//////////////////////
typedef struct
{
    kal_uint32 goep_tidClient;  /* goep_tid of bt */
    kal_uint32 goep_tidClient_mmi;    /* goep_tid of mmi */
    kal_uint32 goep_cidClient;
    kal_uint32 req_idClient;
    t_bdaddr ClientAddress;
    kal_uint8 ClientStatus;
    kal_uint8 isFtpClient;
    kal_uint8* buf_ptrFTC;
    kal_uint16 buf_sizeFTC;
    kal_uint8 FTCfirst;
} rdabt_client_struct;



/*****************************************************************************
* Extern Global Variable
*****************************************************************************/

/*****************************************************************************
* Extern Global Function
*****************************************************************************/
extern void rdabt_goep_connect_req(t_bdaddr address,goep_connect_req_struct *msg_p);
extern void rdabt_goep_disconnect_req(goep_disconnect_req_struct *msg_p);
extern void rdabt_goep_push_req(goep_push_req_struct *msg_p);
extern void rdabt_goep_abort_req(goep_abort_req_struct *msg_p);
extern void rdabt_goep_register_server_req(goep_register_server_req_struct *msg_p);
extern void rdabt_bt_sdpdb_register_req(bt_sdpdb_register_req_struct *msg_p);
extern void rdabt_bt_link_connect_accept_rsp(bt_bm_link_connect_accept_rsp_struct *msg_p);
extern void rdabt_goep_authorize_res(goep_authorize_res_struct *msg_p);
extern void rdabt_goep_connect_res(goep_connect_res_struct *msg_p);
extern void rdabt_goep_push_res(goep_push_res_struct *msg_p);
//extern void rdabt_goep_abort_res(goep_abort_res_struct *msg_p);
extern void rdabt_goep_pull_res(goep_pull_res_struct *msg_p);
extern void rdabt_goep_set_floder_res(goep_set_folder_res_struct *msg_p);
extern void rdabt_notify_evm_ind(void);
extern void rdabt_sdpdb_deregister_req(bt_sdpdb_deregister_req_struct *msg_p);
extern void rdabt_goep_deregister_req(goep_deregister_server_req_struct *msg_p);
extern void rdabt_goep_link_disallow(void);
extern void rdabt_opp_msg_hdler(ilm_struct *ext_message);


#endif /* _RDA_OPP_H */

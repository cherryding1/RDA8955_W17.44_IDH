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



#ifndef BLUETOOTH_HOST_CTRL_DECLARED
#define BLUETOOTH_HOST_CTRL_DECLARED

#ifdef __cplusplus
extern "C" {
#endif

#define RDABT_CORE_TYPE_BR_EDR                              0x01
#define RDABT_CORE_TYPE_LE                                  0x02
#define RDABT_CORE_TYPE_AMP                                 0x04


/* message dispatch function prototypes */
#ifndef _RDA_BT_MSG_H_
typedef struct st_rdabt_msg_t rdabt_msg_t;
#endif

/*
    These flags will be passed to the per-layer startup/shutdown functions.
*/
#define RDABT_HOST_STARTUP            0x01
#define RDABT_HOST_SHUTDOWN           0x02
#define RDABT_HOST_RESET              0x04


t_api rdabt_adp_msg_dispatch(rdabt_msg_t *message);

/* layer control function prototypes */
t_api rdabt_hci_layer_ctrl(u_int8 layerFlags);
t_api rdabt_hci_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_manager_layer_ctrl(u_int8 layerFlags);
t_api rdabt_manager_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_l2cap_layer_ctrl(u_int8 layerFlags);
t_api rdabt_l2cap_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_sdp_cli_layer_ctrl(u_int8 layerFlags);
t_api rdabt_sdp_cli_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_sdp_srv_layer_ctrl(u_int8 layerFlags);
t_api rdabt_sdp_srv_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_rfcomm_layer_ctrl(u_int8 layerFlags);
t_api rdabt_rfcomm_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_OBEX_SRV_layer_ctrl(u_int8 flags);
t_api rdabt_OBEX_SRV_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_OBEX_CLI_layer_ctrl(u_int8 flags);
t_api rdabt_OBEX_CLI_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_hfp_layer_ctrl(u_int8 layerFlags);
t_api rdabt_hfp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_spp_layer_ctrl(u_int8 layerFlags);
t_api rdabt_spp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_avdtp_layer_ctrl(u_int8 flags);
t_api rdabt_avdtp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_avctp_layer_ctrl(u_int8 flags);
t_api rdabt_avctp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_hid_layer_ctrl(u_int8 flags);
t_api rdabt_hid_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_amp_layer_ctrl(u_int8 flags);
t_api rdabt_amp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_pal_layer_ctrl(u_int8 flags);
t_api rdabt_pal_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_hcrp_layer_ctrl(u_int8 flags);
t_api rdabt_hcrp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_pan_layer_ctrl(u_int8 flags);
t_api rdabt_pan_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_att_layer_ctrl(u_int8 flags);
t_api rdabt_att_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_smp_layer_ctrl(u_int8 flags);
t_api rdabt_smp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_gatt_tester_layer_ctrl(u_int8 flags);
t_api rdabt_gatt_tester_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_gatt_hids_layer_ctrl(u_int8 flags);
t_api rdabt_gatt_hids_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_gatt_hidp_layer_ctrl(u_int8 flags);
t_api rdabt_gatt_hidp_msg_dispatch(rdabt_msg_t *message);

t_api rdabt_gatt_fmp_layer_ctrl(u_int8 flags);
t_api rdabt_gatt_fmp_msg_dispatch(rdabt_msg_t *message);

typedef t_api (*layer_ctrl_fun_t)(u_int8);
typedef t_api (*msg_dispatch_t)(rdabt_msg_t*);


APIDECL1 t_api APIDECL2 RDABT_Initialise(const bt_config_t *pBTConfig);
APIDECL1 t_api APIDECL2 RDABT_AddModule(u_int8 module_id, layer_ctrl_fun_t ctrl_fun, msg_dispatch_t msg_dispath);
APIDECL1 t_api APIDECL2 RDABT_Add_Transport(u_int8 transport_id, u_int8 core_type);
APIDECL1 t_api APIDECL2 RDABT_Start_Up(u_int8 transport_id);
APIDECL1 t_api APIDECL2 RDABT_Execute(u_int32 maxIterations);
APIDECL1 t_api APIDECL2 RDABT_HandleEvent(u_int16 event_id, u_int8 *msg) ;
APIDECL1 t_api APIDECL2 RDABT_Shut_Down(void);
APIDECL1 t_api APIDECL2 RDABT_Remove_Transport(u_int8 transport_id);
APIDECL1 t_api APIDECL2 RDABT_Terminate(u_int32 reservedFlags);

APIDECL1 t_api APIDECL2 RDABT_Reset(u_int32 reservedFlags);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BLUETOOTH_HOST_CTRL_DECLARED */

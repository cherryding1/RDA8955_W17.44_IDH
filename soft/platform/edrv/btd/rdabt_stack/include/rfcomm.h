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




#ifndef BLUETOOTH_RFCOMM_API_DECLARED
#define BLUETOOTH_RFCOMM_API_DECLARED


/*
   NOTE:    These functions are provided by the Port Entity and not by
   RFCOMM itself. These functions are concerned with Port Entity
   setup only. The actual RFCOMM transport interface is provided
   by the Port Entity in an OS-specific manner.
*/


/* Connection states for port entity callback function newState argument*/
#define RFCOMM_PE_CONNECTED         0
#define RFCOMM_PE_DISCONNECTED      1
#define RFCOMM_PE_CONNECTFAILED     2


#ifdef __cplusplus
extern "C" {
#endif


/* Server side functions */
APIDECL1 int APIDECL2 PE_SrvRegisterPort(char *portName, u_int8 *serverChan, void (*callbackFunc)(u_int8 serverChan, t_bdaddr remoteDevice, u_int8 newState));
APIDECL1 int APIDECL2 PE_SrvDeregisterPort(u_int8 serverChan);
APIDECL1 int APIDECL2 PE_SrvDisconnectPort(u_int8 serverChan);


/* Client side functions */
APIDECL1 int APIDECL2 PE_CliConnectPort(t_bdaddr remoteDevice, u_int8 serverChan, char *portName, void (*callbackFunc)(u_int8 serverChan, t_bdaddr remoteDevice, u_int8 newState));
APIDECL1 int APIDECL2 PE_CliDisconnectPort(t_bdaddr remoteDevice, u_int8 serverChan);
APIDECL1 int APIDECL2 PE_GetCIDFromConnection(t_bdaddr bdaddr, u_int8 dlci, u_int16 *cid);


/* RDA Stack Entity code.. can be used to implement data-only services
   on top of RFCOMM */

/* defines for flow indication */
#define RSE_FLOW_STOP 0
#define RSE_FLOW_GO 1

/* defines for connection confirmation */
#define RSE_CONNECT_REFUSE 0
#define RSE_CONNECT_ACCEPT 1

typedef struct st_t_RSE_Callbacks
{
    int (*connectionConfirmation)(u_int8 schan, t_bdaddr address, u_int8 status, u_int8 dlci, u_int16 max_frame_size);
    int (*connectionIndication)(u_int8 schan, t_bdaddr address, u_int8 dlci, u_int16 max_frame_size);
    int (*dataIndication)(t_bdaddr address, u_int8 dlci, t_DataBuf *p_buf);
    int (*flowIndication)(t_bdaddr address, u_int8 dlci,  u_int8 flow_state);
    int (*disconnectionIndication)(t_bdaddr address, u_int8 dlci);
    int (*framesizeIndication)(t_bdaddr address, u_int8 dlci, u_int16 max_frame_size);
} t_RSE_Callbacks;

APIDECL1 int APIDECL2 RSE_Init(void);
APIDECL1 int APIDECL2 RSE_SrvRegisterPort(u_int8 *serv_chan, t_RSE_Callbacks *cbs);
APIDECL1 int APIDECL2 RSE_SrvDeregisterPort(u_int8 schan);
APIDECL1 int APIDECL2 RSE_SrvDisconnectPort(u_int8 schan);
APIDECL1 int APIDECL2 RSE_CliConnectPort(t_bdaddr remote_bd_addr, u_int8 rem_schan, u_int16 max_frame_size, t_RSE_Callbacks *cbs);
APIDECL1 int APIDECL2 RSE_CliDisconnectPort(t_bdaddr remote_bd_addr, u_int8 schan);
APIDECL1 int APIDECL2 RSE_Write(t_bdaddr remote_bd_addr, u_int8 dlci, t_DataBuf *p_buf);
APIDECL1 int APIDECL2 RSE_SrvConnectResponse(t_bdaddr remote_bd_addr, u_int8 dlci, u_int8 accept);
APIDECL1 int APIDECL2 RSE_GetWriteBuffer(t_bdaddr bdaddr, int dlci, int length, t_DataBuf **p_buf);
APIDECL1 int APIDECL2 RSE_FreeWriteBuffer(t_DataBuf *frame);
APIDECL1 int APIDECL2 RSE_GetCIDFromConnection(t_bdaddr bdaddr, u_int8 dlci, u_int16 *cid);

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* BLUETOOTH_RFCOMM_API_DECLARED */

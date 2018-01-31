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

#ifndef COMMON_L2CAP_H
#define COMMON_L2CAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#define RDABT_L2CAP_EXTENDED_FEATURE_FLOWCONTROL_MODE                       0x00000001
#define RDABT_L2CAP_EXTENDED_FEATURE_RETRANSMISSION_MODE                    0x00000002
#define RDABT_L2CAP_EXTENDED_FEATURE_BIDIRECTIONAL_QOS                      0x00000004
#define RDABT_L2CAP_EXTENDED_FEATURE_ENHANCED_RETRANSMISSION_MODE           0x00000008
#define RDABT_L2CAP_EXTENDED_FEATURE_STREAMING_MODE                         0x00000010
#define RDABT_L2CAP_EXTENDED_FEATURE_FCS_OPTION                             0x00000020
#define RDABT_L2CAP_EXTENDED_FEATURE_EXTENDED_FLOW_SPECIFICATION            0x00000040
#define RDABT_L2CAP_EXTENDED_FEATURE_FIXED_CHANNELS                         0x00000080
#define RDABT_L2CAP_EXTENDED_FEATURE_EXTENDED_WINDOW_SIZE                   0x00000100
#define RDABT_L2CAP_EXTENDED_FEATURE_UNICAST_CONNECTIONLESS_DATA_RECEPTION  0x00000200

#define RDABT_L2CAP_EXTENDED_FEATURE_RESERVED                               0x80000000


typedef struct
{
    u_int32 result;
    u_int16 mtu;
    u_int16 handle;
} l2cap_conn_cnf_t;

typedef struct
{
    u_int16 length;
    u_int16 handle;
    u_int8* data;
} l2cap_data_ind_t;


APIDECL1 t_api APIDECL2 L2_GetInfo(t_bdaddr BDAddress, u_int16 infoType, u_int8 srcModule);
APIDECL1 t_api APIDECL2 L2_Ping(t_bdaddr BDAddress,u_int8 *echoData, u_int16 length, void (*callbackFunc)(t_bdaddr BDAddress, u_int16 result, u_int8 *echoData, u_int16 length));

// for l2cap bqb test
int L2CAP_ConnectToDevice(t_bdaddr address);
void L2CAP_Disconnect();
void L2CAP_SendData(u_int8 *buff, u_int32 length);
void L2CAP_LocalBusyDetected();
void L2CAP_LocalBusyClear();

#ifdef __cplusplus
}
#endif

#endif

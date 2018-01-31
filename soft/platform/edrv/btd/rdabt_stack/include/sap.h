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



#ifndef __SAP_H__
#define __SAP_H__

// SAP message ID defines
#define SAP_CONNECT_REQ                     0x00
#define SAP_CONNECT_RESP                    0x01
#define SAP_DISCONNECT_REQ                  0x02
#define SAP_DISCONNECT_RESP                 0x03
#define SAP_DISCONNECT_IND                  0x04
#define SAP_TRANSFER_APDU_REQ               0x05
#define SAP_TRANSFER_APDU_RESP              0x06
#define SAP_TRANSFER_ATR_REQ                0x07
#define SAP_TRANSFER_ATR_RESP               0x08
#define SAP_POWER_SIM_OFF_REQ               0x09
#define SAP_POWER_SIM_OFF_RESP              0x0A
#define SAP_POWER_SIM_ON_REQ                0x0B
#define SAP_POWER_SIM_ON_RESP               0x0C
#define SAP_RESET_SIM_REQ                   0x0D
#define SAP_RESET_SIM_RESP                  0x0E
#define SAP_TRANSFER_CARD_READER_STATUS_REQ     0x0F
#define SAP_TRANSFER_CARD_READER_STATUS_RESP    0x10
#define SAP_STATUS_IND                          0x11
#define SAP_ERROR_RESP                          0x12
#define SAP_SET_TRANSPORT_PROTOCOL_REQ          0x13
#define SAP_SET_TRANSPORT_PROTOCOL_RESP         0x14

// sap parameter ID defines
#define SAP_PARAM_MAXMSGSIZE                0x00
#define SAP_PARAM_CONNECTIONSTATUS          0x01
#define SAP_PARAM_RESULTCODE                0x02
#define SAP_PARAM_DISCONNECTIONTYPE         0x03
#define SAP_PARAM_COMMANDAPDU               0x04
#define SAP_PARAM_COMMANDAPDU7816           0x10
#define SAP_PARAM_RESPONSEAPDU              0x05
#define SAP_PARAM_ATR                       0x06
#define SAP_PARAM_CARDREADERSTATUS          0x07
#define SAP_PARAM_STATUSCHANGE              0x08
#define SAP_PARAM_TRANSPORT_PROTOCOL        0x09

// SAP_PARAM_CONNECTIONSTATUS values
#define SAP_CONNECT_OK                      0x00
#define SAP_CONNECT_FAIL                    0x01
#define SAP_CONNECT_UNSUPPORT_SIZE          0x02
#define SAP_CONNECT_SIZE_TOO_SMALL          0x03
#define SAP_CONNECT_CALLING                 0x04

// SAP_PARAM_DISCONNECTIONTYPE values
#define SAP_DISCONNECT_GRACEFUL             0x00
#define SAP_DISCONNECT_IMMEDIATE            0x01

// SAP_PARAM_RESULTCODE values
#define SAP_RESULT_OK                       0x00
#define SAP_RESULT_ERROR                    0x01
#define SAP_RESULT_NOT_ACCESSIBLE           0x02
#define SAP_RESULT_POWER_OFF                0x03
#define SAP_RESULT_CARD_REMOVED             0x04
#define SAP_RESULT_POWER_ON                 0x05
#define SAP_RESULT_NOT_AVAILABLE            0x06
#define SAP_RESULT_NOT_SUPPORT              0x07

// SAP_PARAM_STATUSCHANGE values
#define SAP_UNKNOWN_ERROR                   0x00
#define SAP_CARD_RESET                      0x01
#define SAP_CARD_NOT_ACCESSIBLE             0x02
#define SAP_CARD_REMOVED                    0x03
#define SAP_CARD_INSERTED                   0x04
#define SAP_CARD_RECOVERED                  0x05

typedef struct sap_response_param_t
{
    u_int8 msg_id; // message id
    u_int8 status; // status, result or type
    u_int16 max_size; // SAP_CONNECT_RESP, status in TRANSFER_CARD_READER_STATUS_RESP
    u_int8 *pdu; // SAP_TRANSFER_APDU_RESP and SAP_TRANSFER_ATR_RESP
} sap_response_param;

#ifdef __cplusplus
extern "C" {
#endif

// after connect spp success, call SAP_Connect_Req to connect sap connect
APIDECL1 t_api APIDECL2 SAP_Connect_Req(u_int8 port, u_int16 max_size);
// send request
APIDECL1 t_api APIDECL2 SAP_Send_Request(u_int8 reqest_id);
// set protocol
APIDECL1 t_api APIDECL2 SAP_Set_Protocol(u_int8 procotol);
// send apdu data
APIDECL1 t_api APIDECL2 SAP_Send_APDU(u_int8 param_id, u_int8 *data, u_int8 length);
// parse response data
APIDECL1 t_api APIDECL2 SAP_Parse_Response(sap_response_param *parm, u_int8 *data);

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif


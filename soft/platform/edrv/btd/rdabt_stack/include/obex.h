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



#ifndef _OBEX_COMMON_H_
#define _OBEX_COMMON_H_

#include "bt_types.h"
/*
  response codes
 */
#define OBEX_CONTINUE 0x90

#define OBEX_OK 0xa0
#define OBEX_CREATED 0xa1
#define OBEX_ACCEPTED 0xa2
#define OBEX_NON_AUTH_INFO 0xa3
#define OBEX_NO_CONTENT 0xa4
#define OBEX_RESET_CONTENT 0xa5
#define OBEX_PARTIAL_RESET 0xa6

#define OBEX_MULTIPLE_CHOICE 0xb0
#define OBEX_MOVED_PERM 0xb1
#define OBEX_MOVED_TEMP 0xb2
#define OBEX_SEE_OTHER 0xb3
#define OBEX_NOT_MODIFIED 0xb4
#define OBEX_USE_PROXY 0xb5

#define OBEX_BAD_REQUEST 0xc0
#define OBEX_UNAUTHORIZED 0xc1
#define OBEX_PAYMENT_REQUIRED 0xc2
#define OBEX_FORBIDDEN 0xc3
#define OBEX_NOT_FOUND 0xc4
#define OBEX_METHOD_NOT_ALLOWED 0xc5
#define OBEX_NOT_ACCEPTABLE 0xc6
#define OBEX_PROXY_AUTH_REQUIRED 0xc7
#define OBEX_REQUEST_TIME_OUT 0xc8
#define OBEX_CONFLICT 0xc9
#define OBEX_GONE 0xca
#define OBEX_LENGTH_REQUIRED 0xcb
#define OBEX_PRECONDITION_FAIL 0xcc
#define OBEX_REQUEST_ENTITY_TOO_LARGE 0xcd
#define OBEX_REQUEST_URL_TOO_LARGE 0xce
#define OBEX_UNSUPPORTED_MEDIA_TYPE 0xcf

#define OBEX_SERVER_ERROR 0xd0
#define OBEX_NOT_IMPLEMENTED 0xd1
#define OBEX_BAD_GATEWAY 0xd2
#define OBEX_SERVICE_UNAVAILABLE 0xd3
#define OBEX_GATEWAY_TIMEOUT 0xd4
#define OBEX_HTTP_VER_NOT_SUPPORTED 0xd5

#define OBEX_DATABASE_FULL 0xe0
#define OBEX_DATABASE_LOCKED 0xe1

/*
  Operation Opcodes
 */

#define OBEX_CONNECT 0x80
#define OBEX_DISCONNECT 0x81
#define OBEX_PUT_FINAL 0x82
#define OBEX_PUT 0X02
#define OBEX_GET 0x03
#define OBEX_GET_FINAL 0x83
#define OBEX_SETPATH 0x85
#define OBEX_RESERVED 0x04
#define OBEX_RESERVED_FINAL 0x84
#define OBEX_SETPATH 0x85
#define OBEX_ABORT 0xff
#define OBEX_OP_RESERVED_LOWER 0x06
#define OBEX_OP_RESERVER_UPPER 0x0f
#define OBEX_OP_USER_LOWER 0x10
#define OBEX_OP_USER_UPPER 0x1f

/* header codes */

#define OBEX_COUNT 0xc0
#define OBEX_NAME 0x01
#define OBEX_TYPE 0x42
#define OBEX_LENGTH 0xc3
#define OBEX_TIME_ISO 0x44
#define OBEX_TIME_32 0xc4
#define OBEX_DESCRIPTION 0x05
#define OBEX_TARGET 0x46
#define OBEX_HTTP 0x47
#define OBEX_BODY 0x48
#define OBEX_END_BODY 0x49
#define OBEX_WHO 0x4a
#define OBEX_CID 0xcb
#define OBEX_APP_PARAM 0x4c
#define OBEX_AUTH_CHALLANGE 0x4d
#define OBEX_AUTH_RESPONSE 0x4e
#define OBEX_CLASS 0x4f
#define OBEX_HD_RESERVED_LOWER 0x10
#define OBEX_HD_RESERVER_UPPER 0x2f
#define OBEX_HD_USER_LOWER 0x30
#define OBEX_HD_USER_UPPER 0x3f

#define OBEX_HEADER 3
#define OBEX_SETPATH_HEADER 5
#define OBEX_CONNECT_HEADER 7

/* supported types */

#define OBEX_PUSH_VCARD_21 1
#define OBEX_PUSH_VCARD_30 2
#define OBEX_PUSH_VCAL_10 4
#define OBEX_PUSH_ICAL_20 8
#define OBEX_PUSH_VNOTE : 16
#define OBEX_PUSH_VMESSAGE : 32
#define OBEX_PUSH_ANY : 64

/* supported datastores */

#define OBEX_SYNC_PB 1
#define OBEX_SYNC_CAL 2
#define OBEX_SYNC_NT 4
#define OBEX_SYNC_MSG 8

/* service type */

#define OBEX_INBOX_SERVICE
#define OBEX_CAPABILITY_SERVICE
#define OBEX_IRMC_SERVCE

/* object types */

#define OBEX_VCARD 1
#define OBEX_VCAL  2
#define OBEX_VNOTE 3
#define OBEX_VMESS 4
#define OBEX_VOTHER 5



// con type
#define SCLASS_UUID_OBEX_PUSH 0x1105
#define SCLASS_UUID_OBEX_FTP 0x1106
#define SCLASS_UUID_PHONEBOOK_ACCESS_PSE 0x112F
#define SCLASS_UUID_MESSAGE_ACCESS_MSE 0x1132
#define SCLASS_UUID_MESSAGE_NOTIFICATION_SERVER 0x1133
#define SCLASS_UUID_MESSAGE_ACCESS_PROFILE  0x1134
#define SCLASS_UUID_OBEX_DIRECT_PRINTING 0x1118
#define SCLASS_UUID_OBEX_PRINTING_STATUS 0x1123

#define BPP_BUF_MAX_LENGTH 4096


static const u_int8 OBEX_FTP_UUID   [16] = {0xf9,0xec,0x7b,0xc4,0x95,0x3c,0x11,0xd2,0x98,0x4e,0x52,0x54,0x00,0xdc,0x9e,0x09};

//static u_int8 BPP_DP_UUID[16] = {0x00,0x00,0x11,0x18,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5f,0x9b,0x34,0xfb};//direct printing

//static u_int8 BPP_PS_UUID[16] = {0x00,0x00,0x11,0x23,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5f,0x9b,0x34,0xfb}; //printing status

//typedef void (*t_obex_command_handler)(u_int8 status, u_int8* data, u_int16 length);


/*struct rdabt_obex_command_callback {
  u_int32 tid;
  t_obex_command_handler callback;
  struct rdabt_obex_command_callback* p_next;
};  */


typedef struct st_t_obex_header_offsets
{
    u_int16 count,
            name,
            type,
            len,
            time_iso,
            time_32,
            des,
            tar,
            http,
            body,
            eob,
            who,
            cid,
            app,
            au_cal,
            au_rsp,
            cla;
} t_obex_header_offsets;

typedef struct st_t_obex_headers
{

    u_int16 nameOffset;
    u_int16 typeOffset;
    u_int16 bodyOffset;

} t_obex_headers;

//obex
enum obex_cli_process_pending
{
    OBEX_PROCESS_PENDING_NULL,
    OBEX_CONNECT_CNF_ID,
    OBEX_DISCONNCET_CNF_ID,
    OBEX_PUT_ID,
    OBEX_GET_ID,
    OBEX_SET_PATH_ID,
    OBEX_ABORT_ID
};

//bpp
enum obex_bpp_process_operation
{
    BPP_OPERATION_NULL,
    BPP_CREATE_JOB,
    BPP_SEND_DOCUMENT,
    BPP_GET_JOB_ATTRIBUTES,
    BPP_GET_PRINTERATTRIBUTES,
    BPP_CANCEL_JOB,
    BPP_GET_REFERENCED_OBJECTS,
    BPP_GET_EVENT
};


typedef struct st_obex_pushserver_cnf_msg
{
    t_bdaddr address;
    u_int8 channel;
    u_int8* formatc;

} obex_pushserver_cnf_msg_t;

typedef struct st_obex_cli_data_ind_msg
{

    u_int8   typeorstatus;
    u_int8   process_pending;
    u_int16 current_uuid;
    u_int16 length;
    u_int32 tid;
    u_int8* rx_buffer;
    u_int8 dlci;
} obex_cli_data_ind_msg_t;

typedef struct st_obex_server_data_ind_msg
{
    u_int32 server_tid;
    u_int32 tid;
    u_int8 messageType;
    t_obex_headers headers;
    u_int8 *data;
    u_int16 len;
    void* cbref;
} obex_server_data_ind_msg_t;

/*#ifndef __BLUETOOTH_STRUCT_H_
typedef enum
{
    GOEP_FIRST_PKT = 0,
    GOEP_NORMAL_PKT,
    GOEP_FINAL_PKT,
    GOEP_SINGLE_PKT
} goep_pkt_type_enum;
#endif*/

typedef struct
{
    u_int8 goep_conn_id;
    //goep_pkt_type_enum pkt_type;
    u_int8 pkt_type;

    u_int8* obj_mime_type;//[80];
    u_int32 total_obj_len;
    u_int16*obj_name;
    //u_int16 obj_name[OBEX_MAX_PATH];

    u_int16 NameLength;
    u_int32 TypeLength;
    u_int8* frag_ptr;
    u_int16 frag_len;
} obex_push_req_handle_struct;


#ifdef __cplusplus
extern "C" {
#endif


/* Header encoding functions*/
APIDECL1 int APIDECL2 OBEX_CreateStringHeader(u_int8 id, u_int16 length, u_int16* pData, t_DataBuf *txBuffer, u_int16* pOffset);
APIDECL1 int APIDECL2 OBEX_CreateSequenceHeader(u_int8 id, u_int16 length, u_int8* pData, t_DataBuf *txBuffer, u_int16* pOffset);
APIDECL1 int APIDECL2 OBEX_CreateByteHeader(u_int8 id, u_int8 data, t_DataBuf* txBuffer, u_int16* pOffset);
APIDECL1 int APIDECL2 OBEX_Create4ByteHeader(u_int8 id, u_int32 data, t_DataBuf *txBuffer, u_int16* pOffset);
APIDECL1 int APIDECL2 OBEX_ExtractHeaders(t_obex_header_offsets* headers,u_int16* offset,u_int8* finger, u_int16 len);


/* Write Buffer functions*/
APIDECL1 int APIDECL2 OBEX_GetWriteBuffer(u_int16 len, t_DataBuf **writeBuffer);
APIDECL1 int APIDECL2 OBEX_FreeWriteBuffer(t_DataBuf *writeBuffer);


typedef void (*t_obex_service_handler)(u_int32 tid, u_int8 messageType, t_obex_headers headers, u_int8 *data,u_int16 len,void* cbref);


struct rdabt_obex_session_state
{
    u_int32 tid;
    u_int32 server_tid;
    u_int8 state;
    t_obex_service_handler handler;
    struct rdabt_obex_session_state* p_next;
};

struct rdabt_obex_inbox_registration
{
    u_int32 tid;
    t_obex_service_handler handler;
    struct rdabt_obex_inbox_registration * p_next;
};

struct rdabt_obex_service_registration
{
    u_int32 tid;
    t_obex_service_handler handler;
    u_int8* target;
    u_int8 target_length;
    u_int32 cid;
    u_int8 cid_length;
    struct rdabt_obex_service_registration* p_next;
};
typedef struct st_obex_server_authorize_ind_msg
{
    t_bdaddr addr;
    u_int8 dlci;
    u_int8 schan;
    u_int16 uuid;
    u_int32 tid;
    u_int16 psm;
} obex_server_authorize_ind_msg_t;

typedef struct
{
    t_DataBuf *tx_buffer;
    u_int32 cid;
    u_int32 tid;
    u_int8 final;
    u_int16 header_length;
    u_int16 *name;
    u_int16 name_length;

} ftpc_info_req;

typedef struct
{
    u_int32 tid;
    u_int8 response_code;
    u_int16 header_length;
    t_DataBuf* txbuffer;

} ftps_info_req;

// uuid: SDP_SCLASS_UUID_OBEX_FTP, SDP_SCLASS_UUID_OBEX_PUSH, SDP_SCLASS_UUID_PHONEBOOK_ACCESS_PSE
APIDECL1 int APIDECL2 OBEX_RegisterService(u_int16 psm, u_int8* rx_buffer, u_int16 buff_length, u_int16 uuid);


APIDECL1 int APIDECL2 OBEX_Connect(u_int32 tid, u_int8 version, u_int8 flags, u_int16 max_length, u_int16 target_length, u_int8* target, u_int16 header_length, t_DataBuf *txbuffer);//t_obex_command_handler handler);
APIDECL1 int APIDECL2 OBEX_Disconnect(u_int32 tid);//, u_int16 header_length, t_DataBuf *tx_buffer);//, t_obex_command_handler handler);
APIDECL1 int APIDECL2 OBEX_Put(u_int32 tid, u_int8 final, u_int16 header_length, t_DataBuf *txBuffer);//, t_obex_command_handler handler);
APIDECL1 int APIDECL2 OBEX_Get(u_int32 tid, u_int8 final, u_int16 header_length, t_DataBuf *txBuffer);
APIDECL1 int APIDECL2 OBEX_SetPath(u_int32 tid, u_int8 flags, u_int8 constants, u_int16 header_length, t_DataBuf *txBuffer);
APIDECL1 int APIDECL2 OBEX_Abort(u_int32 tid);

APIDECL1 int APIDECL2 OBEX_GetMTU(u_int32 tid,u_int16* mtu);
/* the connection routines */
APIDECL1 int APIDECL2 OBEX_TransportConnect_RF(t_bdaddr address,u_int8 server_channel, u_int16 psm, u_int32* tid, u_int16 mtu);//,void (*callbackFunc)(u_int32 tid));
//APIDECL1 int APIDECL2 OBEX_TransportConnect_RF(t_bdaddr address,u_int8 server_channel, u_int32* tid, u_int16 mtu,void (*disconnect)(u_int32 tid));
/*int OBEX_TransportConnect_Serial(port);
int OBEX_TransportConnect_Socket(address,port);*/


APIDECL1 int APIDECL2 OBEX_TransportDisconnect(u_int32 tid);


APIDECL1 int APIDECL2 OBEX_RegisterInbox(u_int32 tid, void *cref);
APIDECL1 int APIDECL2 OBEX_Response(u_int32 tid, u_int8 response_code,u_int16 header_length, t_DataBuf* txbuffer);
APIDECL1 int APIDECL2 OBEX_ConnectResponse(u_int32 tid, u_int8 response_code, u_int8 version, u_int8 flags,u_int16 maxLen, u_int16 header_length, t_DataBuf* txbuffer);
APIDECL1 int APIDECL2 OBEX_GetCID(u_int32 tid,u_int8 target_length,u_int8* target,u_int32* cid);
APIDECL1 int APIDECL2 OBEX_ServerTransportDisconnect(u_int32 tid);

/* the listen routines must return back a transport identifier that can
be used in the service registration */

APIDECL1 int APIDECL2 OBEX_GetConnectionInfo_RF(u_int32 tid,t_bdaddr *address,u_int8 *server_channel);

typedef u_int8 t_obex_nonce[18];
typedef u_int8 t_obex_digest[18];

#define RDABT_MAX_PINCODE_SIZE 20

enum OBEX_SecurityLevel { NOSECURITY, AUTHENTICATION};

void CallSecurityCallback(void);

/*APIDECL1 int APIDECL2 OBEX_RegisterSecurityLevel(u_int8 secLevel, char *userID, char *localUserPin, void (*SecCallback)(u_int8 cid, u_int16 serverChanNum, u_int8 status));*/
/*APIDECL1 int APIDECL2 OBEX_EnterAuthInfo(char *userID, char *remoteUserPin, u_int8 cid, u_int16 serverChanNum);*/
APIDECL1 int APIDECL2 OBEX_EnableAuthentication(u_int8 pinLen, u_int8 *pinCode);
APIDECL1 int APIDECL2 OBEX_GenerateNonce(u_int8 pinLen, u_int8 *pinCode, u_int8 *nonce);
APIDECL1 int APIDECL2 OBEX_GenerateDigest(u_int8 pinLen, u_int8 *pinCode, u_int8 *nonce, u_int8 *digest);
APIDECL1 int APIDECL2 OBEX_AuthRequest(u_int8 *localPinCode, u_int8 localPinCodeLen, t_obex_nonce nonce, u_int16 *offset_len, t_DataBuf *txBuffer);
APIDECL1 int APIDECL2 OBEX_AuthResponse(u_int8 *remotePinCode, u_int8 remotePinCodeLen, u_int8 *remoteNonceHeader, u_int16 *offset_len, t_DataBuf *txBuffer);
APIDECL1 int APIDECL2 OBEX_AuthCheck(u_int8 *localPinCode, u_int8 localPinCodeLen, t_obex_nonce localNonce, u_int8 *digestHeader);


APIDECL1 t_api APIDECL2 OBEX_ObjectPush(u_int32 tid, u_int8 type, u_int16 *name, u_int16 nameLen, u_int8 *data, u_int16 dataLen, u_int32 totalLen,u_int8 final);
APIDECL1 int APIDECL2 OBEX_ObjectPull(u_int32 tid);

APIDECL1 int APIDECL2 OBEX_ObjectExchange(u_int32 tid, u_int16 *name, u_int16 nameLen, u_int8 *data, u_int16 len, u_int32 totalLen);//,t_obex_command_handler handler);

APIDECL1 int APIDECL2 OBEX_FindObjectPushServer(t_bdaddr* address,u_int8* channel,u_int8* formats,u_int16 connect_type);


void OBEX_ConnectRequest(t_bdaddr* address,u_int16 connect_type,u_int8 * client_rx_buffer);
void printf_oppc_tid(u_int32 opp_send_tid,u_int32 msg_tid,u_int8 pending_pro);





/*APIDECL1 int APIDECL2 OBEX_PushConnectResponse(u_int32 tid, u_int16 maxLength, u_int8 responseCode);*/
APIDECL1 int APIDECL2 OBEX_PullResponse(u_int32 tid, u_int8 responseCode, u_int16 *name, u_int8 nameLen, u_int8* data, u_int16 dataLen, u_int16 totalLen);
APIDECL1 int APIDECL2 OBEX_RegisterObjectPushServer(u_int8 channel, u_int8 supported_types);


APIDECL1 int APIDECL2 OBEX_FindFTPServer(t_bdaddr* address,u_int8* channel,u_int8* formats, void (*callbackFunc)(t_bdaddr address,u_int8 channel,u_int8* formats));

APIDECL1 int APIDECL2 OBEX_FTPConnect(u_int32 tid, u_int16 mtu, t_DataBuf *txBuffer,u_int16 headerLength);
APIDECL1 int APIDECL2 OBEX_Delete(u_int32 tid, u_int32 cid, u_int16* name, u_int8 length);
APIDECL1 int APIDECL2 OBEX_CreateFolder(u_int32 tid,u_int32 cid, u_int16* name,u_int8 length);
APIDECL1 int APIDECL2 OBEX_ChooseCurrentFolder(u_int32 tid, u_int32 cid, u_int16* name, u_int8 length);
APIDECL1 int APIDECL2 OBEX_GetFolderListingObject(u_int32 tid, u_int32 cid);

APIDECL1 int APIDECL2 OBEX_RegisterFTPServer(u_int8 channel);
APIDECL1 int APIDECL2 OBEX_FTPConnectResponse(u_int32 tid, u_int16 maxLength,u_int8 responseCode);
APIDECL1 int APIDECL2 OBEX_ChooseCurrentFolderResponse(u_int32 tid, u_int8 responseCode, u_int8 *folderListingOb, u_int8 floLen, u_int16 totalLen);
APIDECL1 int APIDECL2 OBEX_FTPPullResponse(u_int32 tid, u_int8 responseCode, u_int16 *name, u_int8 nameLen, u_int16 totalLen, u_int8* data, u_int16 dataLen);

APIDECL1 int APIDECL2 OBEX_RegisterSyncCommandServer(u_int8 channel);
APIDECL1 int APIDECL2 OBEX_FindSyncServer(t_bdaddr* address,u_int8* channel,u_int8* formats);

APIDECL1 int APIDECL2 OBEX_SuggestSync(u_int32 tid, u_int8 type, char* pAny, u_int8 length);//,t_obex_command_handler handler);
APIDECL1 int APIDECL2 OBEX_RegisterSyncServer(u_int8 channel, u_int8 supported_types);
APIDECL1 int APIDECL2 OBEX_FindSyncCommandServer(t_bdaddr* address,u_int8* channel);

APIDECL1 int APIDECL2 OBEX_SrvConnectResponse(t_bdaddr remote_bd_addr, u_int8 dlci, u_int8 accept);

APIDECL1 int APIDECL2 RDABT_OPP_ConnectProcess(t_bdaddr* btt_address,u_int16 connect_type);

APIDECL1 int APIDECL2 RDABT_OPPC_PutHandle(obex_push_req_handle_struct *  push_req);
APIDECL1 int APIDECL2 RDABT_OPPC_PushRequest(obex_push_req_handle_struct *  push_req);

APIDECL1 int APIDECL2 RDABT_OPPC_Disconnect(u_int32 tid);
APIDECL1 int APIDECL2 RDABT_OPPC_AbortRequest(u_int32 tid);

APIDECL1 int APIDECL2 RDABT_FTPC_GetFolderListingObject(u_int32 tid, u_int32 cid);
APIDECL1 int APIDECL2 RDABT_FTPC_GetRequest(u_int32 tid, u_int8 final, u_int16 header_length, t_DataBuf *txBuffer);

APIDECL1 int APIDECL2 RDABT_FTPC_RemoveFile(u_int32 tid,u_int16 *name,u_int8 length);
APIDECL1 int APIDECL2 RDABT_FTPC_CreateFolder(u_int32 tid,u_int16 *name,u_int8 length);

APIDECL1 int APIDECL2 RDABT_OBEX_SrvConnectResponse(t_bdaddr bd_addr, u_int8 dlci, u_int8 accept);
APIDECL1 int APIDECL2 RDABT_FTPS_Response(u_int32 tid, u_int8 response_code, u_int16 header_length, t_DataBuf* txbuffer);


/* phone book access profile */
// place of the phone book
enum PhoneBook_Place
{
    PHONE_BOOK_PHONE,
    PHONE_BOOK_SIM1,
    PHONE_BOOK_SIM2,
    PHONE_BOOK_SIM3,
    PHONE_BOOK_SIM4,
};

// phone book type
enum PhoneBook_Type
{
    PHONE_BOOK_PHONE_BOOK,
    PHONE_BOOK_INCOMING_HISTORY,
    PHONE_BOOK_OUTCOMING_HISTORY,
    PHONE_BOOK_MISSED_CALL,
    PHONE_BOOK_COMBINED_HISTORY,
};

enum Msg_Type
{
    MSG_INBOX,
    MSG_OUTBOX,
    MSG_SENT,
    MSG_DELETED,
    MSG_DRAFT
};
enum Map_Msg_Type
{
    EMAIL,// for emails on RFC2822 or MIME type basis or
    SMS_GSM,//  for GSM short messages [12] or
    SMS_CDMA,//  for CDMA short messages [14] or
    MMS//  for 3GPP MMS messages [13].
};

enum Map_Msg_STATUS_Type
{
    NEWMESSAGE,// for emails on RFC2822 or MIME type basis or
    DELIVERYSUCCESS,//  for GSM short messages [12] or
    SENDINGSUCCESS,//  for CDMA short messages [14] or
    DELIVERYFAILURE,
    SENDINGFAILURE,
    MEMORYFULL,
    MEMORYAVAILLABLE,
    MESSAGEDELETED,
    MESSAGESHIFT
};

enum PhoneBook_Format
{
    PHONE_BOOK_VCARD2_1,
    PHONE_BOOK_VCARD3_0,
};

// low 32 fields
// bit0 ~ bit7
#define PHONE_BOOK_FIELD_VERSION                    0x00000001
#define PHONE_BOOK_FIELD_FN                         0x00000002
#define PHONE_BOOK_FIELD_N                          0x00000004
#define PHONE_BOOK_FIELD_PHOTO                      0x00000008
#define PHONE_BOOK_FIELD_BDAY                       0x00000010
#define PHONE_BOOK_FIELD_ADR                        0x00000020
#define PHONE_BOOK_FIELD_LABEL                      0x00000040
#define PHONE_BOOK_FIELD_TEL                        0x00000080
// bit8 ~ bit15
#define PHONE_BOOK_FIELD_EMAIL                      0x00000100
#define PHONE_BOOK_FIELD_MAILER                     0x00000200
#define PHONE_BOOK_FIELD_TZ                         0x00000400
#define PHONE_BOOK_FIELD_GEO                        0x00000800
#define PHONE_BOOK_FIELD_TITLE                      0x00001000
#define PHONE_BOOK_FIELD_ROLE                       0x00002000
#define PHONE_BOOK_FIELD_LOGO                       0x00004000
#define PHONE_BOOK_FIELD_AGENT                      0x00008000
// bit16 ~ bit23
#define PHONE_BOOK_FIELD_ORG                        0x00010000
#define PHONE_BOOK_FIELD_NOTE                       0x00020000
#define PHONE_BOOK_FIELD_REV                        0x00040000
#define PHONE_BOOK_FIELD_SOUND                      0x00080000
#define PHONE_BOOK_FIELD_URL                        0x00100000
#define PHONE_BOOK_FIELD_UID                        0x00200000
#define PHONE_BOOK_FIELD_KEY                        0x00400000
#define PHONE_BOOK_FIELD_NICKNAME                   0x00800000
// bit24 ~ bit28
#define PHONE_BOOK_FIELD_CATEGORIES                 0x01000000
#define PHONE_BOOK_FIELD_PROID                      0x02000000
#define PHONE_BOOK_FIELD_CLASS                      0x04000000
#define PHONE_BOOK_FIELD_SORT_STRING                0x08000000
#define PHONE_BOOK_FIELD_DATETIME                   0x10000000

// high 32 fields
#define PHONE_BOOK_FIELD_PROPRIETARY_FILTER         0x00000080

#define RDABT_PBAP_SORT_ALPHA    0X01
#define RDABT_PBAP_SORT_INDEX    0X00
#define RDABT_PBAP_SORT_PHONE    0X02

APIDECL1 t_api APIDECL2 OBEX_SetPhoneBook(u_int32 tid, u_int32 cid, enum PhoneBook_Place which, enum PhoneBook_Type type, u_int8 use_list);
APIDECL1 t_api APIDECL2 OBEX_SetPhoneBookFormat(u_int32 tid, u_int32 low_fields, u_int32 high_fields, u_int8 format, u_int8 sort);

APIDECL1 t_api APIDECL2 OBEX_PullPhoneBook(u_int32 tid, u_int32 cid,enum PhoneBook_Type type, u_int32 start, u_int32 number);
APIDECL1 t_api APIDECL2 OBEX_PullvCardList(u_int32 tid, u_int32 cid,enum PhoneBook_Type type, u_int32 start, u_int32 number);
APIDECL1 t_api APIDECL2 OBEX_PullvCardEntry(u_int32 tid, u_int32 cid,enum PhoneBook_Type type, u_int32 index);
APIDECL1 t_api APIDECL2 OBEX_GetMsgList(u_int32 tid, u_int32 cid,enum Msg_Type type, u_int32 start, u_int32 number);
APIDECL1 t_api APIDECL2 OBEX_SetMap(u_int32 tid, u_int32 cid, enum Msg_Type type, u_int8 use_list);
APIDECL1 t_api APIDECL2 OBEX_GetMsg(u_int32 tid, u_int32 cid,enum Msg_Type type, u_int16 *hanlde,u_int8 handle_length);

APIDECL1 t_api APIDECL2 OBEX_PhoneBookDisconnect(void);
APIDECL1 t_api APIDECL2 OBEX_MapSetNotificationRegistration(u_int32 tid, u_int32 cid, u_int8 flag);

#ifdef __cplusplus
}
#endif

#endif // _RDABT_OBEX_H_


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




#ifndef BLUETOOTH_SDP_API_DECLARED
#define BLUETOOTH_SDP_API_DECLARED

#include "platform_config.h"
#include "bt_types.h"
/* These constants are also required to interpet search results */

#define SDP_UINT_1      0x08
#define SDP_UINT_2      0x09
#define SDP_UINT_4      0x0a
#define SDP_UINT_8      0x0b
#define SDP_UINT_16     0x0c

#define SDP_SINT_1      0x10
#define SDP_SINT_2      0x11
#define SDP_SINT_4      0x12
#define SDP_SINT_8      0x13
#define SDP_SINT_16     0x14

#define SDP_UUID_2      0x19
#define SDP_UUID_4      0x1a
#define SDP_UUID_16     0x1c

#define SDP_TEXT_SMALL  0x25
#define SDP_TEXT_MEDIUM 0x26
#define SDP_TEXT_LARGE  0x27

#define SDP_BOOL        0x28

#define SDP_URL_SMALL   0x45
#define SDP_URL_MEDIUM  0x46
#define SDP_URL_LARGE   0x47

/* These SEQUENCE constants are also used for SDP_CreateList() */
#define SDP_SEQUENCE_SMALL      0x35
#define SDP_SEQUENCE_MEDIUM     0x36
#define SDP_SEQUENCE_LARGE      0x37

#define SDP_UNION_SMALL         0x3d
#define SDP_UNION_MEDIUM        0x3e
#define SDP_UNION_LARGE         0x3f


/* protocol errors */

#define PDU_ID_SDP_ERROR_RESPONSE 0x01
#define PDU_ID_SDP_SERVICE_SEARCH_REQUEST 0x02
#define PDU_ID_SDP_SERVICE_SEARCH_RESPONSE 0x03
#define PDU_ID_SDP_SERIVCE_ATTRIBUTE_REQUEST 0x04
#define PDU_ID_SDP_SERVICE_ATTRIBUTE_RESPONSE 0x05
#define PDU_ID_SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST 0x06
#define PDU_ID_SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE 0x07



/* protocol uuids */

#define SDP_PROTOCOL_UUID_SDP 0X0001
#define SDP_PROTOCOL_UUID_RFCOMM 0X0003
#define SDP_PROTOCOL_UUID_TCS_BIN 0X0005
#define SDP_PROTOCOL_UUID_L2CAP 0X0100
#define SDP_PROTOCOL_UUID_IP 0X0009
#define SDP_PROTOCOL_UUID_UDP 0X0002
#define SDP_PROTOCOL_UUID_TCP 0X0004
#define SDP_PROTOCOL_UUID_TCS_AT 0X0006
#define SDP_PROTOCOL_UUID_OBEX 0X0008
#define SDP_PROTOCOL_UUID_FTP 0X000A
#define SDP_PROTOCOL_UUID_HTTP 0X000C
#define SDP_PROTOCOL_UUID_WSP 0X000E
#define SDP_PROTOCOL_UUID_AVCTP 0x0017
#define SDP_PROTOCOL_UUID_AVDTP 0x0019

/* service class uuids */

#define SDP_SCLASS_UUID_SDS 0x1000
#define SDP_SCLASS_UUID_BROWSE 0x1001
#define SDP_SCLASS_UUID_PUBLIC 0x1002
#define SDP_SCLASS_UUID_SERIAL 0x1101
#define SDP_SCLASS_UUID_LAN_PPP 0x1102
#define SDP_SCLASS_UUID_DIALUP 0x1103
#define SDP_SCLASS_UUID_IRMC_SYNC 0x1104
#define SDP_SCLASS_UUID_OBEX_PUSH 0x1105
#define SDP_SCLASS_UUID_OBEX_FTP 0x1106
#define SDP_SCLASS_UUID_IRMC_COMMAND 0x1107
#define SDP_SCLASS_UUID_HEADSET 0x1108
#define SDP_SCLASS_UUID_CORDLESS 0x1109
#define SDP_SCLASS_UUID_AUDIOSOURCE 0x110A
#define SDP_SCLASS_UUID_AUDIOSINK 0x110B
#define SDP_SCLASS_UUID_AVREMOTECONTROL_TARGET 0x110C
#define SDP_SCLASS_UUID_A2DP 0x110D
#define SDP_SCLASS_UUID_AVREMOTECONTROL 0x110E
#define SDP_SCLASS_UUID_INTERCOM 0x1110
#define SDP_SCLASS_UUID_FAX 0x1111
#define SDP_SCLASS_UUID_HEADSET_GATEWAY 0x1112
#define SDP_SCLASS_UUID_WAP 0x1113
#define SDP_SCLASS_UUID_WAP_CLIENT 0x1114
#define SDP_SCLASS_UUID_PAN_PANU 0x1115
#define SDP_SCLASS_UUID_PAN_NAP 0x1116
#define SDP_SCLASS_UUID_PAN_GN 0x1117
#define SDP_SCLASS_UUID_DIRECT_PRINTING 0x1118
#define SDP_SCLASS_UUID_IMAGE_RESPONDER 0x111B
#define SDP_SCLASS_UUID_BASIC_IMAGE 0x111D
#define SDP_SCLASS_UUID_HANDSFREE 0x111E
#define SDP_SCLASS_UUID_HANDSFREE_AUDIOGATEWAY 0x111F
#define SDP_SCLASS_UUID_PRINTING_STATUS 0x1123
#define SDP_SCLASS_UUID_HID_DEVICE 0x1124
#define SDP_SCLASS_UUID_HARDCOPY_CABLE_REPLACEMENT  0x1125
#define SDP_SCLASS_UUID_HARDCOPY_CABLE_PRINT  0x1126
#define SDP_SCLASS_UUID_SIM_ACCESS 0x112D
#define SDP_SCLASS_UUID_PHONEBOOK_ACCESS_PCE 0x112E
#define SDP_SCLASS_UUID_PHONEBOOK_ACCESS_PSE 0x112F
#define SDP_SCLASS_UUID_PHONEBOOK_ACCESS 0x1130
#define SDP_SCLASS_UUID_HEADSET_HS 0x1131
#define SDP_SCLASS_UUID_MESSAGE_ACCESS_SERVER 0x1132
#define SDP_SCLASS_UUID_MESSAGE_NOTIFICATION_SERVER 0x1133
#define SDP_SCLASS_UUID_MESSAGE 0x1134
#define SDP_SCLASS_UUID_PNP 0x1200
#define SDP_SCLASS_UUID_GEN_NETWORK 0x1201
#define SDP_SCLASS_UUID_GEN_FTP 0x1202
#define SDP_SCLASS_UUID_GEN_AUDIO 0x1203
#define SDP_SCLASS_UUID_GEN_TELE 0x1204


/* attribute identifers */

#define SDP_AID_RECORD_HANDLE 0x0000
#define SDP_AID_SERVICE_CLASS 0x0001
#define SDP_AID_RECORD_STATE 0x0002
#define SDP_AID_SERVICE_ID 0x0003
#define SDP_AID_PROTOCOL 0x0004
#define SDP_AID_BROWSE_LIST 0x0005
#define SDP_AID_LANGUAGE_BASE 0x0006
#define SDP_AID_TIME_TO_LIVE 0x0007
#define SDP_AID_AVAILABILITY 0x0008
#define SDP_AID_PROFILE_LIST 0x0009
#define SDP_AID_DOC_URL 0x000a
#define SDP_AID_EXE_URL 0x000b
#define SDP_AID_ICON_URL 0x000c
#define SDP_AID_ADDITIONAL_PROTOCOL_LIST 0x000d

#define SDP_AID_DEFAULT_LANGUAGE 0x0100
#define SDP_AID_SERVICE_NAME_OFFSET 0x0000
#define SDP_AID_DESCRIPTION_OFFSET 0x0001
#define SDP_AID_PROVIDER_NAME_OFFSET 0x0002

#define SDP_AID_VERSION_LIST 0x0200
#define SDP_AID_DATABASE_STATE 0x0201
#define SDP_AID_GROUP_ID 0x0200

#define SDP_AID_AUDIO_VOLUME 0x0302
#define SDP_AID_NETWORK 0x0301

#define SDP_AID_VERSION 0x0300
#define SDP_AID_DATA_STORES 0x0301

#define SDP_AID_FORMATS_LIST 0x0303
#define SDP_AID_FAX_CLASS_1 0x0302
#define SDP_AID_FAX_CLASS_2 0x0303
#define SDP_AID_AUDIO_FEEDBACK 0x0305
#define SDP_AID_NETWORK_ADDRESS 0x0306
#define SDP_AID_WAP_GATEWAY 0x0307
#define SDP_AID_HOMEPAGE_URL 0x0308
#define SDP_AID_WAP_STACK_TYPE 0x0309
#define SDP_AID_SUPPORTED_CAPABALITIES 0x0310
#define SDP_AID_SUPPORTED_FEATURES 0x0311
#define SDP_AID_SUPPORTED_FUNCTIONS 0x0312


typedef struct
{
    u_int16 maxItems;
    u_int16 maxDuration;
    u_int16 maxBytes;
} t_SDP_StopRule;

typedef struct
{
    u_int16 numItems;
    u_int32* patternUUIDs;
} t_SDP_SearchPattern;

typedef struct
{
    u_int16 numItems;
    u_int8 *aid_range;
    u_int32* attributeIDs;
} t_SDP_AttributeIDs;


typedef struct st_t_SDP_Element
{
    t_bdaddr deviceAddress;
    char* deviceName;
    u_int16 numMatchingRecords;
    u_int16 dataLen;
    u_int8* pData;
    struct st_t_SDP_Element* pNext;
} t_SDP_Element;

typedef struct
{
    u_int8 status;
    u_int8 resultType;
    u_int16 numElements;
    t_SDP_Element element;
} t_SDP_Results;

/* SDP protocol constants for status member */
#define SDP_NOERROR                 0x00
#define SDP_UNSUPPORTED             0x01
#define SDP_INVALIDHANDLE           0x02
#define SDP_INVALIDSYNTAX           0x03
#define SDP_INVALIDSIZE             0x04
#define SDP_INVALIDCONTINUATION     0x05
#define SDP_NORESOURCES             0x06

/* Constants for resultType member */
#define SDP_SEARCH_RESULT       1
#define SDP_BROWSE_RESULT       2
#define SDP_GENBROWSE_RESULT    3

typedef struct
{
    t_bdaddr bdAddress;
    u_int32 patten_uuid;  //Service Class ID
    u_int32 sc_only;//only found SDP_AID_SERVICE_CLASS
} t_SDP_Service_Request;

typedef struct
{
    u_int32 chn_num;
    u_int32 SC_uuid;  //Service Class ID List uuid
    u_int32 PDL_uuid;//profile descriptor list uuid
} t_SDP_service_Results;

#define MAX_SDP_UUID_NUM 30
typedef struct
{
    u_int32 total_searched;
    t_SDP_service_Results service[MAX_SDP_UUID_NUM];
} t_SDP_service_Parse_Results;

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct st_t_BT_DeviceEntry t_MGR_DeviceEntry;

APIDECL1 t_api APIDECL2 SDP_DeviceServiceSearch(t_bdaddr bdAddress, t_SDP_SearchPattern *pPattern,
        t_SDP_AttributeIDs *pAttributes, t_SDP_StopRule *pRule);

APIDECL1 int APIDECL2  SDP_ServiceResultParse(t_SDP_Results *pResults, t_SDP_service_Parse_Results *service_finded);

APIDECL1 int APIDECL2 SDP_FreeResults(t_SDP_Results* pResults);

#define RDABT_SDP_ServiceResultParse   SDP_ServiceResultParse

APIDECL1 t_api APIDECL2 RDABT_SDP_DeviceServiceSearch(t_bdaddr bdAddress, u_int32 patten_uuid, u_int32 sc_only);

APIDECL1 int APIDECL2 RDABT_SDP_FreeResults(t_SDP_Results* pResults);

//add a DID service record, do not call this function more than once.
APIDECL1 int APIDECL2 rdabt_sdp_add_did_service(u_int16 vendorId, u_int16 productId, u_int16 version);

//APIDECL1 int APIDECL2 rdabt_sdp_replace_service(u_int32 handle,/*const t_SDP_serviceRecord*/void  *new_service);
//void rdabt_sdp_replace_service(u_int32 handle, const t_SDP_serviceRecord *new_service);

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* BLUETOOTH_SDP_API_DECLARED */

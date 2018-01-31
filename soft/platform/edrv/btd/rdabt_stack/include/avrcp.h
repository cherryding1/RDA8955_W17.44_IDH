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


#ifndef _RDABT_AVRCP_H_
#define _RDABT_AVRCP_H_

#define AVRCP0_CTYPE_CONTROL            0x00
#define AVRCP0_CTYPE_STATUS            0x01
#define AVRCP0_CTYPE_NOTIFY                   0x03

#define AVRCP_REGISTER_NOTIFICATION     0x31
#define AVRCP_SET_ABSOLUET_VOLUME      0x50
#define AVRCP_SET_ADDRESSED_PLAYER      0x60
#define AVRCP_SET_BROWSED_PLAYER      0x70
#define AVRCP_GET_FOLDER_ITEMS             0x71
#define AVRCP_CHANGE_PATH                     0x72
#define AVRCP_GET_ITEM_ATTRIBUTES      0x73
#define AVRCP_PLAY_ITEM                           0x74
#define AVRCP_SEARCH                                 0x80
#define AVRCP_ADD_TO_NOW_PLAYING       0x90
#define AVRCP_GENERAL_REJECT       0xa0


#define AVRCP_MAX_FOLDER_ITEM_NUMBER 255
#define AVRCP_MAX_FOLDER_NAME_LENGTH 255

#define AVRCP_MAX_MEDIA_ITEM_NUMBER  8
#define AVRCP_MAX_MEDIA_ELEMENT_NUMBER 8

/*!
    @brief Operation ID, used to identify operation. See table 9.21 AV/C Panel
    Subunit spec. 1.1 #
*/
typedef enum
{
    OPID_SELECT             = (0x0),
    OPID_UP,
    OPID_DOWN,
    OPID_LEFT,
    OPID_RIGHT,
    OPID_RIGHT_UP,
    IPID_RIGHT_DOWN,
    OPID_LEFT_UP,
    OPID_LEFT_DOWN,
    OPID_ROOT_MENU,
    OPID_SETUP_MENU,
    OPID_CONTENTS_MENU,
    OPID_FAVOURITE_MENU,
    OPID_EXIT,
    /* 0x0e to 0x1f Reserved */
    OPID_0                  = (0x20),
    OPID_1,
    OPID_2,
    OPID_3,
    OPID_4,
    OPID_5,
    OPID_6,
    OPID_7,
    OPID_8,
    OPID_9,
    OPID_DOT,
    OPID_ENTER,
    OPID_CLEAR,
    /* 0x2d - 0x2f Reserved */
    OPID_CHANNEL_UP         = (0x30),
    OPID_CHANNEL_DOWN,
    OPID_SOUND_SELECT,
    OPID_INPUT_SELECT,
    OPID_DISPLAY_INFORMATION,
    OPID_HELP,
    OPID_PAGE_UP,
    OPID_PAGE_DOWN,
    /* 0x39 - 0x3f Reserved */
    OPID_POWER              = (0x40),
    OPID_VOLUME_UP,
    OPID_VOLUME_DOWN,
    OPID_MUTE,
    OPID_PLAY,
    OPID_STOP,
    OPID_PAUSE,
    OPID_RECORD,
    OPID_REWIND,
    OPID_FAST_FORWARD,
    OPID_EJECT,
    OPID_FORWARD,
    OPID_BACKWARD,
    /* 0x4d - 0x4f Reserved */
    OPID_ANGLE              = (0x50),
    OPID_SUBPICTURE,
    /* 0x52 - 0x70 Reserved */
    OPID_F1                 = (0x71),
    OPID_F2,
    OPID_F3,
    OPID_F4,
    OPID_F5,
    OPID_VENDOR_UNIQUE      = (0x7e)
                              /* Ox7f Reserved */
} t_AVC_OPERATION_ID;

typedef enum
{
    AVRCP_PDU_GETCAPABILITIES                                     = 0X10,
    AVRCP_PDU_LISTPLAYERAPPLICATIONSETTINGATTRIBUTES              = 0X11,
    AVRCP_PDU_LISTPLAYERAPPLICATIONSETTINGVALUES                  = 0X12,
    AVRCP_PDU_GETCURRENTPLAYERAPPLICATIONSETTINGVALUE             = 0X13,
    AVRCP_PDU_SETPLAYERAPPLICATIONSETTINGVALUE                    = 0X14,
    AVRCP_PDU_GETPLAYERAPPLICATIONSETTINGATTRIBUTETEXT            = 0X15,
    AVRCP_PDU_GETPLAYERAPPLICATIONSETTINGVALUETEXT                = 0X16,
    AVRCP_PDU_INFORMDISPLAYABLECHARACTERSET                       = 0X17,
    AVRCP_PDU_INFORMBATTERYSTATUSOFCT                             = 0X18,
    AVRCP_PDU_GETELEMENTATTRIBUTES                                = 0X20,
    AVRCP_PDU_GETPLAYSTATUS                                       = 0X30,
    AVRCP_PDU_REGISTERNOTIFICATION                                = 0X31,
    AVRCP_PDU_REQUESTCONTINUINGRESPONSE                           = 0X40,
    AVRCP_PDU_ABORTCONTINUINGRESPONSE                             = 0X41,
    AVRCP_PDU_SETABSOLUTEVOLUME                             = 0X50,
    AVRCP_PDU_SETBROWSEDPLAYER                                    = 0X70,
    AVRCP_PDU_GETFOLDERITEMS                                      = 0X71,
    AVRCP_PDU_CHANGEPATH                                          = 0X72,
    AVRCP_PDU_GETITEMATTRIBUTES                                   = 0X73,
    AVRCP_PDU_PLAYITEM                                            = 0X74,
    AVRCP_PDU_SEARCH                                              = 0X80,
} t_AVRCP_PDU_ID;

typedef enum
{
    AVRCP_STATUS_STOPPED,
    AVRCP_STATUS_PLAYING,
    AVRCP_STATUS_PAUSED,
    AVRCP_STATUS_FWD_SEEK,
    AVRCP_STATUS_REV_SEEK,
    AVRCP_STATUS_ERROR = 0xff
} t_AVRCP_PLAY_STATUS;

typedef enum
{
    AVRCP_EVENT_PLAYBACK_STATUS_CHANGED=(0x01),
    AVRCP_EVENT_TRACK_CHANGED=(0x02) ,
    AVRCP_EVENT_TRACK_REACHED_END=(0x03),
    AVRCP_EVENT_TRACK_REACHED_START=(0x04),
    AVRCP_EVENT_PLAYBACK_POS_CHANGED=(0x05),
    AVRCP_EVENT_BATT_STATUS_CHANGED=(0x06),
    AVRCP_EVENT_SYSTEM_STATUS_CHANGED=(0x07),
    AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED=(0x08),
    AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED=(0x09),
    AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED=(0x0A),
    AVRCP_EVENT_ADDRESSED_PLAYERS_CHANGED=(0x0B),
    AVRCP_EVENT_UIDS_CHANGED=(0x0C),
    AVRCP_EVENT_VOLUME_CHANGED=(0x0D),
} t_AVRCP_EVENT;

typedef enum
{
    AVRCP_DEVICE_NONE,
    AVRCP_TARGET,
    AVRCP_CONTROLLER,
    AVRCP_TARGET_AND_CONTROLLER
} t_AVRCP_DEVICE_TYPE;


typedef enum
{
    AVRCP_INITIALISING,
    AVRCP_READY,
    AVRCP_CONNECTING,
    AVRCP_CONNECTED
} t_AVRCP_STATE;

typedef enum
{
    AVRCP_ATTR_TITLE = 1,
    AVRCP_ATTR_ARTIST,
    AVRCP_ATTR_ALBUM,
    AVRCP_ATTR_TRACK,
    AVRCP_ATTR_TOTAL,
    AVRCP_ATTR_GENRE,
    AVRCP_ATTR_POSITION,
} t_AVRCP_MEDIA_ATTRIBUTE;

typedef enum
{
    AVRCP_VENDOR_UNIQUE_NEXT_GROUP=0,
    AVRCP_VENDOR_UNIQUE_PREVIOUS_GROUP
} t_AVRCP_VENDOR_UNIQUE_OPERATION_ID;

typedef enum
{
    AVRCP_SCOPE_MEDIA_PLAYER_LIST=0,
    AVRCP_SCOPE_FILE_SYSTEM,
    AVRCP_SCOPE_SEARCH,
    AVRCP_SCOPE_NOW_PLAYING
} t_AVRCP_BROWSING_SCOPE_TYPE;


typedef enum
{
    AVRCP_INVALID_COMMAND = (0),
    AVRCP_INVALID_PARAMETER,
    AVRCP_PARAMETER_CONTENT_ERROR,
    AVRCP_INTERNAL_ERROR,
    AVRCP_OPERATION_COMPLETED_WITHOUT_ERROR,
    AVRCP_UID_CHANGED,
    AVRCP_RESERVED,
    AVRCP_INVALID_DIRECTION,
    AVRCP_NOT_A_DIRECTORY,
    AVRCP_DOES_NOT_EXIST,
    AVRCP_INVALID_SCOPE=0xa,
    AVRCP_RANGE_OUT_OF_BOUNDS,
    AVRCP_FOLDER_IS_NOT_PLAYABLE,
    AVRCP_MEDIA_IN_USE,
    AVRCP_NOW_PLAYING_LIST_FULL,
    AVRCP_SEARCH_NOT_SUPPORTED,
    AVRCP_SEARCH_IN_PROGRESS=0x10,
    AVRCP_INVALID_PLAYER_ID,
    AVRCP_PLAYER_NOT_BROWSABLE,
    AVRCP_PLAYER_NOT_ADDRESSED,
    AVRCP_NO_VALID_SEARCH_RESULTS,
    AVRCP_NO_AVAILABLE_PLAYERS,
    AVRCP_ADDRESSED_PLAYER_CHANGED
} t_AVRCP_ERROR_STATUS_CODE;

typedef enum
{
    AVRCP_ITEM_MEDIA_PLAYER_LIST=1,
    AVRCP_ITEM_FILE_SYSTEM,
    AVRCP_ITEM_SEARCH,
    AVRCP_ITEM_NOW_PLAYING
} t_AVRCP_ITEM_TYPE;

typedef enum
{
    AVRCP_FOLDER_DIRECTION_UP=0,
    AVRCP_FOLDER_DIRECTION_DOWN
} t_AVRCP_FOLDER_DIRECTION_TYPE;


typedef enum
{
    AVRCP_MEDIA_PLAYER_ITEM=0x01,
    AVRCP_FOLDER_ITEM,
    AVRCP_MEIDA_ELEMENT_ITEM
} t_AVRCP_BROWSABLE_ITEM_TYPE;

typedef enum
{
    AVRCP_MEDIA_PLAYER_AUDIO=0x01,
    AVRCP_MEDIA_PLAYER_VIDEO=0x02,
    AVRCP_MEDIA_PLAYER_BROADCASTING_AUDIO=0x04,
    AVRCP_MEDIA_PLAYER_BROADCASTING_VIDEO=0x08
} t_AVRCP_MEDIA_PLAYER_TYPE;  //see Bluetooth Assigned number--host operatiing

typedef enum
{
    AVRCP_ATTRIBUTES_ILLEGAL=0,
    AVRCP_ATTRIBUTES_EQUALIZER_ON_OFF_STATUS,
    AVRCP_ATTRIBUTES_REPEAT_MODE_STATUS,
    AVRCP_ATTRIBUTES_SHUFFLE_ON_OFF_STATUS,
    AVRCP_ATTRIBUTES_SCAN_ON_OFF_STATUS,
} t_AVRCP_PLAYER_APPLICATION_SETTING_ATTRIBUTES_TYPE;

typedef enum
{
    AVRCP_PACKET_NON_FRAGMENTED=0,
    AVRCP_PACKET_START,
    AVRCP_PACKET_CONTINUE,
    AVRCP_PACKET_END
} t_AVRCP_PACKET_TYPE;

typedef struct
{
    u_int8   seq_id;
    u_int8   c_type;
    u_int8   subunit_type;
    u_int8   subunit_id;
    u_int16  data_len;
    u_int16  profile_id;
    u_int16 cid;
    u_int8   frame_data[48];
} t_avrcp_cmd_frame_ind;

typedef struct
{
    u_int8   label;
    u_int8   packet_type;
    u_int8   pdu_id;
    u_int16  profile_id;
    u_int16  data_len;
    u_int16 cid;
    u_int8   frame_data[48];
} t_avrcp_browsing_cmd_frame_ind;

typedef struct
{
    u_int16 connect_id;
    u_int16 result;
    u_int16 psm;
    t_bdaddr device_addr;
} t_avrcp_connect_inf;


typedef struct
{
    u_int8 connect_id;
    u_int8 status;
    u_int32 totalTime;
    u_int32 currentTime;
} t_avrcp_status_ind;

typedef struct
{
    u_int8 connect_id;
    u_int8 type;//1:changed    0:interim
    u_int8 len;
    u_int8 status;
    u_int8 event_id;
    u_int8 para_buf[10];
} t_avrcp_notify_ind;

typedef struct
{
    u_int8 connect_id;
    u_int8 pdu_id;
    u_int16 length;
    u_int8 *param;
} t_avrcp_data_ind;

typedef struct
{
    u_int8 folder_uid[8];
    u_int8 folder_type;
    u_int8 isplayable;
    u_int16 charset;
    u_int16 name_length;
    u_int8 display_name[AVRCP_MAX_FOLDER_NAME_LENGTH];
} t_avrcp_folder_items_info;

typedef struct
{
    u_int32 attribute_id;
    u_int16 charset;
    u_int16 attribute_length;
    u_int8 attribute_value[AVRCP_MAX_FOLDER_NAME_LENGTH];
} t_avrcp_media_attribute_info;

typedef struct
{
    u_int8 media_element_uid[8];
    u_int8 media_type;
    u_int16 charset;
    u_int16 name_length;
    u_int8 display_name[AVRCP_MAX_FOLDER_NAME_LENGTH];
    u_int8 number_of_attribues;
    t_avrcp_media_attribute_info attribute_info[AVRCP_MAX_MEDIA_ELEMENT_NUMBER];
} t_avrcp_media_element_item_info;

typedef struct
{
    u_int16  NumberOfItem;
    t_avrcp_media_element_item_info item_list[AVRCP_MAX_MEDIA_ITEM_NUMBER];
} t_avrcp_media_element_info;

typedef struct
{
    u_int16 item_length;
    u_int16 play_id;
    u_int16 character_set_id;
    u_int16 name_len;
    u_int32 sub_type;
    u_int8  major_type;
    u_int8  play_statu;
    u_int8 feature_bit_mask[16];
    char name[20];
} t_avrcp_media_play_info;

typedef struct
{
    u_int16  NumberOfPlay;
    t_avrcp_media_play_info play_list[8];
} t_avrcp_media_playlist_info;

#pragma pack (push,1)

typedef struct
{
    u_int8                scope;
    u_int8                uid[8];
    u_int16               uidCounter;
    u_int8                numOfAttr;
    u_int32               attr[0];
} t_AVRCP_GetItemAttrbutes_req;

typedef struct
{
    u_int16               playerId;
} t_AVRCP_SetBrowsedPlayer_req;

typedef struct
{
    u_int8                scope;
    u_int32               startItem;
    u_int32               endItem;
    u_int8                attrCnt;
    u_int32               attr[0];
} t_AVRCP_GetFolderItems_req;

typedef struct
{
    u_int16               charStId;
    u_int16               length;
    u_int32               attr[0];
} t_AVRCP_Search_req;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

APIDECL1 t_api APIDECL2 Avrcp_RegisterService(u_int8 role); // t_AVRCP_DEVICE_TYPE
APIDECL1 t_api APIDECL2 Avrcp_Set_State(t_AVRCP_STATE state, u_int8 index);
APIDECL1 t_api APIDECL2 Avrcp_Connect_Req(t_bdaddr bdaddr);
APIDECL1 t_api APIDECL2 Avrcp_Disconnect_Req(t_bdaddr bdaddr);

APIDECL1 t_api APIDECL2 Avrcp_Get_UnitInfo(u_int8 unit_type, u_int16 cid);
APIDECL1 t_api APIDECL2 Avrcp_Send_Key(u_int8 key);
APIDECL1 t_api APIDECL2 Avrcp_Send_Press(u_int8 key);
APIDECL1 t_api APIDECL2 Avrcp_Send_Release(u_int8 key);
APIDECL1 t_api APIDECL2 Avrcp_Update_Player_Status(void);
APIDECL1 t_api APIDECL2 Avrcp_Get_Element_Attribute(void);

t_AVRCP_STATE Get_Avrcp_Browsing_State(u_int8 index);
APIDECL1 t_api APIDECL2 Avrcp_Set_Browsing_State(t_AVRCP_STATE state, u_int8 index);
APIDECL1 t_api APIDECL2 Avrcp_Browsing_Connect_Req(t_bdaddr bdaddr);
APIDECL1 t_api APIDECL2 Avrcp_Browsing_Disconnect_Req(u_int8 index);
APIDECL1 t_api APIDECL2 Avrcp_Metadata_Transfer(u_int8 ctype, u_int8 pdu_id, u_int8 *param, u_int16 param_length);

APIDECL1 t_api APIDECL2 Avrcp_Send_Cmd_Response(u_int16 cid, u_int8 seq_id, u_int16 profile_id, u_int8 result, u_int8 subunit_type, u_int8 subunit_id, u_int8 *frame_data, u_int8 data_len, u_int8 operation_code);
APIDECL1 t_api APIDECL2 Avrcp_Send_Browsing_Cmd_Response(u_int16 cid, u_int8 label, u_int8 packet_type, u_int16 profile_id, u_int8 data_len,  u_int8 *frame_data);


APIDECL1 t_api APIDECL2 Avrcp_Send_Get_Folder_Items(u_int8 pdu_id, u_int16 cid);
APIDECL1 t_api APIDECL2 Avrcp_Set_Absolute_Volume(u_int16 cid);
APIDECL1 t_api APIDECL2 Avrcp_Get_Capabilities(u_int8 capability_id);
APIDECL1 t_api APIDECL2 Avrcp_Set_Browsing_command(u_int8 pdu_id, u_int16 data_len, u_int8* data , u_int16 cid);
APIDECL1 t_api APIDECL2 Avrcp_Get_Player_Status(void);
void Avrcp_Set_Notification(u_int8 event_id);
APIDECL1 t_api APIDECL2 Avrcp_List_Player_Application_Setting_Attributes(void);
APIDECL1 t_api APIDECL2 Avrcp_List_Player_Application_Setting_Values(u_int8 attribute_id);
APIDECL1 t_api APIDECL2 Avrcp_Get_Current_Player_Application_Setting_Values(u_int8 number, u_int8 *attribute_id);
APIDECL1 t_api APIDECL2 Avrcp_Set_Player_Application_Setting_Values(u_int8 number, u_int8 *attribute);
APIDECL1 t_api APIDECL2 Avrcp_Get_Player_Application_Setting_Attributes_Text(u_int8 number, u_int8 *attribute_id);
APIDECL1 t_api APIDECL2 Avrcp_Get_Player_Application_Setting_Values_Text(u_int8 number, u_int8 attribute_id, u_int8 *value_id );
APIDECL1 t_api APIDECL2 Avrcp_Inform_Displayable_Character_Set(u_int8 number, u_int16 *charset_id );
APIDECL1 t_api APIDECL2 Avrcp_Inform_Battery_Status_of_CT(u_int8 status);

APIDECL1 t_api APIDECL2 Avrcp_Send_Group_Press(u_int16 group_key);
APIDECL1 t_api APIDECL2 Avrcp_Send_Group_Release(u_int16 group_key);
APIDECL1 t_api APIDECL2 Avrcp_Send_Unit_Info_Command(u_int8 ctype, u_int8 subunit_type, u_int8 subunit_id, u_int8 op_code, u_int8 param_length, u_int8 *param, u_int16 cid);

APIDECL1 t_api APIDECL2 Avrcp_Browsing_GetItemAttributes_Req(t_AVRCP_GetItemAttrbutes_req * cmdParBuf, u_int16 cid);

APIDECL1 t_api APIDECL2 Avrcp_Browsing_SetBrowsedPlayer_Req(t_AVRCP_SetBrowsedPlayer_req* cmdParBuf, u_int16 cid);

APIDECL1 t_api APIDECL2 Avrcp_Browsing_GetFolderItems_Req(t_AVRCP_GetFolderItems_req* cmdParBuf, u_int16 cid);

APIDECL1 t_api APIDECL2 Avrcp_Browsing_Search_Req(t_AVRCP_Search_req* cmdParBuf, u_int16 cid);






#define RDABT_Avrcp_Set_State( state)       Avrcp_Set_State( state)
#define RDABT_Avrcp_Connect_Req( bdaddr)    Avrcp_Connect_Req( bdaddr)
#define RDABT_Avrcp_Disconnect_Req          Avrcp_Disconnect_Req
#define RDABT_Avrcp_Send_Key(key)           Avrcp_Send_Key(key)
#define RDABT_Avrcp_Send_Press(key)         Avrcp_Send_Press(key)
#define RDABT_Avrcp_Send_Release(key)       Avrcp_Send_Release(key)

#ifdef __cplusplus
}
#endif

#endif //_RDABT_AVRCP_H_


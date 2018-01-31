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

#ifndef _USERGEN_H_
#define _USERGEN_H_
#define PREFAB_ROOT "./PREFAB"
#define PREFAB_DIAG "/"

#ifndef MMI_ON_WIN32
#include "kal_non_specific_general_types.h"
#include "mmi_features.h"
#include "device.h"
#include "mcd_l4_common.h"
#include "l4c_aux_struct.h"
#include "l4c2uem_struct.h"
#include "l4c_rspfmttr.h"
#include "nvram_user_defs.h"
#include "phonebookdef.h"
#include "custom_mmi_default_value.h"
#include "custom_nvram_editor_data_item.h"

//#if defined (_USERGEN_SHELL) && defined(_FS_SIMULATOR_)

#define _CSW 0
#define USERGEN_FLASH_SECTORCOUNT 252
#define USERGEN_FLASH_BASEADDR (0)
#define USERGEN_FLASH_SECTORSIZE (4 * 1024)
#define USERGEN_FLASH_TOTALSIZE (USERGEN_FLASH_SECTORSIZE * USERGEN_FLASH_SECTORCOUNT)
#define USERGEN_FLASH_BLOCKSIZE (512)

#define USERGEN_TS_ID 0

#define CALLLOG_MAX_RECORD MAX_PHB_LN_ENTRY
#define MAX_SIM_SETTING_PASSWD_LEN 8

#else
#define U8 UINT8
#define U16 UINT16
#define USERGEN_TS_ID 0

#define PHONE_PHB_ENTRY_COUNT 200
#define CALLLOG_MAX_RECORD MAX_PHB_LN_ENTRY

#define ENCODING_LENGTH 2
#define MAX_GRP_NAME_LENGTH 20
#define MAX_PB_CALLER_GROUPS 5
#define MIN_MIDI_ID 151
#define PHONE_BOOK_BASE 1000
#define NVRAM_SETTING_SIZE 60
#define NVRAM_CACHE_SIZE 256
#define PHONE_SETUP_BASE 13000
#define SCREENSAVER_BASE (PHONE_SETUP_BASE + 200)

#if defined(__MMI_SUPPORT_AZAAN__)
#if defined(__MMI_FM_RADIO_SCHEDULE_REC__)
#define NVRAM_ALM_QUEUE_SIZE 280
#else
#define NVRAM_ALM_QUEUE_SIZE 240
#endif
#else
#if defined(__MMI_FM_RADIO_SCHEDULE_REC__)
#define NVRAM_ALM_QUEUE_SIZE 230
#else
#define NVRAM_ALM_QUEUE_SIZE 190
#endif
#endif

#define NVRAM_SETTING_SIZE 60
#define NVRAM_EF_BT_INFO_SIZE 2200
#define NVRAM_EF_CHIS_NUM_TOTAL 1
#define NVRAM_EF_CHIS_NUM_SIZE 1
#define ADP_SIM_NUMBER 2
#define MAX_CC_ADDR_LEN 41

#define LEVEL4 3
#define FACTORY_RESTORE_DEFAULT_FMRDO_BACKGROUND_PLAY 1
#define FACTORY_RESTORE_DEFAULT_FMRDO_SKIN 0
#if defined(VOLUME_WITH_15_LEVEL)
#define FACTORY_RESTORE_DEFAULT_FMRDO_VOLUME 12
#else
#define FACTORY_RESTORE_DEFAULT_FMRDO_VOLUME 4
#endif
#define FACTORY_RESTORE_DEFAULT_AUDPLY_LIST_AUTO_GEN 1
#if defined(VOLUME_WITH_15_LEVEL)
#define FACTORY_RESTORE_DEFAULT_AUDPLY_VOLUME 12
#else
#define FACTORY_RESTORE_DEFAULT_AUDPLY_VOLUME 4
#endif
#define FACTORY_RESTORE_DEFAULT_AUDPLY_PREFER_LIST 0
#define FACTORY_RESTORE_DEFAULT_AUDPLY_SKIN 0
#define FACTORY_RESTORE_DEFAULT_AUDPLY_REPEAT 0
#define FACTORY_RESTORE_DEFAULT_AUDPLY_SHUFFLE 0
#define FACTORY_RESTORE_DEFAULT_AUDPLY_BACKGROUND_PLAY 1
#define FACTORY_RESTORE_DEFAULT_AUDPLY_LYRICS_DISPLAY 0
#define FACTORY_RESTORE_DEFAULT_SNDREC_STORAGE 0
#define FACTORY_RESTORE_DEFAULT_SNDREC_FORMAT 0
#define NVRAM_PHB_IDS_SIZE 510 /*For 85 entries in one NVRAM record.*/
#define FACTORY_RESTORE_DEFAULT_FMRDO_BACKGROUND_PLAY 1
#define FACTORY_RESTORE_DEFAULT_FMRDO_SKIN 0
#if defined(VOLUME_WITH_15_LEVEL)
#define FACTORY_RESTORE_DEFAULT_FMRDO_VOLUME 12
#else
#define FACTORY_RESTORE_DEFAULT_FMRDO_VOLUME 4
#endif
#define FACTORY_RESTORE_DEFAULT_SPEED_DIAL 0
#define FACTORY_RESTORE_DEFAULT_CONTRAST_SUBLCD_LEVEL_VALUE 7
#define FACTORY_RESTORE_DEFAULT_CONTRAST_MAINLCD_LEVEL_VALUE 7
#define FACTORY_RESTORE_DEFAULT_WELCOME_TEXT_STATUS 0
#define FACTORY_RESTORE_DEFAULT_APHORISM_TEXT_STATUS 1
#define FACTORY_RESTORE_DEFAULT_OWNER_NO_STATUS 0
#define FACTORY_RESTORE_DEFAULT_STATUS_SHOW_DT_TIME_STATUS 1
#define FACTORY_RESTORE_DEFAULT_AUTOUPDATE_TIME_VALUE 0
#define FACTORY_RESTORE_DEFAULT_SCREENSAVER_STATUS 0
#define FACTORY_RESTORE_DEFAULT_SCREENSAVER_WAITING_TIME 30
#define FACTORY_RESTORE_DEFAULT_CURRENT_SCREENSVER_ID (SCREENSAVER_BASE + 1)
#define FACTORY_RESTORE_DEFAULT_PROFILES_ACTIVATED_ID 0

typedef unsigned char kal_uint8;
typedef unsigned short int kal_uint16;
typedef unsigned int kal_uint32;

typedef enum {
    MMI_ALERT_NONE,
    MMI_RING,
    MMI_VIBRATION_ONLY,
    MMI_VIBRATION_AND_RING,
    MMI_VIBRATION_THEN_RING,
    MMI_SILENT
} MMI_ALERT_TYPE;

typedef enum {
    IMG_SCR_PBOOK_CAPTION = PHONE_BOOK_BASE + 1,
    IMG_MITEM1010,
    IMG_MITEM1011,
    IMG_MITEM1012,
    IMG_MITEM1013,
    IMG_MITEM1014,
    IMG_MITEM1015,
    IMG_MITEM1010_SEARCH,
    IMG_MITEM1011_INPUT_METHOD,
    IMG_MITEM1051,
    IMG_MITEM1052,
    IMG_MITEM1101,
    IMG_MITEM1102,
    IMG_MITEM1103,
    IMG_ENTRY_MOVED_MSG,
    IMG_ENTRY_DUPLICATED_MSG,
    IMG_PROCESSING_PHONEBOOK,
    IMG_STORAGE_SIM,
    IMG_STORAGE_HANDSET,
    IMG_NAME,
    IMG_MOBILE_NUMBER,
    IMG_HOME_NUMBER,
    IMG_COMPANY_NAME,
    IMG_EMAIL_ADDRESS,
    IMG_OFFICE_NUMBER,
    IMG_FAX_NUMBER,
    IMG_PICTURE,
    IMG_RING_TONE,
    IMG_CALLER_GROUP,
    IMG_ID_PHB_CALLERGRP_MEMBER,
    IMG_ID_PHB_ASSOCIATE_VIDEO,
    IMG_ID_PHB_BIRTHDAY,
    IMG_PHB_VIEW_ENTRY, //CSD 2003-11-11 Wilson, Chicago Only
    IMG_PHB_SEARCH_ENTRY,
    IMG_PHB_ADD_ENTRY,
    IMG_PHB_DELETE_ALL,
    IMG_PHB_DELETE_ALL_ONE,
    IMG_PHB_COPY_ALL,
    IMG_PHB_OWNER_NUMBER,
    IMG_PHB_SDN,
    IMG_PHB_SOS_NUMBER,
    IMG_PHB_SPEED_DIAL,
    IMG_PHB_CALLER_GROUP,
    IMG_PHB_SETTINGS,
    IMG_EDIT_LSK,
    IMG_DONE_RSK,
    IMG_VIEW_LSK,
    IMG_CG_NAME,
    IMG_CG_ALERT_TYPE,
    IMG_CG_RING_TONE,
    IMG_CG_PICTURE,
    IMG_CG_INDICATOR_LITE,
    IMG_SDL_CALL_LSK,
    IMG_COPIED,
    IMG_COPY_ALL_ABORT,
    IMG_COPY_ALL_ABORTING,
    IMG_ID_PHB_NO_IMAGE,
    IMG_ID_PHB_QUICK_SEARCH,      /*CSD 2004-04-27 Wilson, Quick Search*/
    IMG_ID_PHB_QUICK_SEARCH_FIND, /*CSD 2004-04-27 Wilson, Quick Search*/
    IMG_ID_PHB_LIST_ICON1,
    IMG_ID_PHB_CALLER_PIC,
    IMG_ID_PHB_CALLER_PIC_1,
    IMG_ID_PHB_CALLER_RING,
    IMG_ID_PHB_CALLER_RING_1,
    IMG_ID_PHB_CALLER_VIDEO,
    IMG_ID_PHB_EXPORT,
    IMG_ID_PHB_INPORT,
    IMG_PHB_DEFAULT = PHONE_BOOK_BASE + 800,
    IMG_PHB_PICTURE1,
    IMG_PHB_PICTURE2,
    IMG_PHB_PICTURE3,
    IMG_PHB_PICTURE4,
    VDO_ID_PHB_MTCALL_1 = PHONE_BOOK_BASE + 900,
    VDO_ID_PHB_MTCALL_2,
    VDO_ID_PHB_MTCALL_3,
    VDO_ID_PHB_MTCALL_4,
    VDO_ID_PHB_MTCALL_5,
    VDO_ID_PHB_MTCALL_6,
    VDO_ID_PHB_MTCALL_7,
    VDO_ID_PHB_MTCALL_8,
    VDO_ID_PHB_MTCALL_9,
    VDO_ID_PHB_MTCALL_10,
    IMG_ID_PHB_CALLER_PICTURE,        // 15510
    IMG_ID_PHB_CALLER_VIDEO_1,        // 15511
    IMG_ID_PHB_CALLERGROUP_BLACKLIST, // 15512
    IMG_ID_PHB_CALLERGROUP_REDLIST,   // 15513
    /*Guojian Add Start For phb 底部的中间图标   Ver: Jessamine  on 2008-6-26 10:14 */
    SIMULATE_CENTER_KEY_ICON1,
    SIMULATE_CENTER_KEY_ICON2
/*Guojian Add End  For phb 底部的中间图标   Ver: Jessamine  on 2008-6-26 10:14 */

#ifdef __MMI_MULTI_SIM__
    ,
    IMG_STORAGE_SIM_1,
    IMG_STORAGE_SIM_2,
    IMG_STORAGE_SIM_3,
    IMG_STORAGE_SIM_4
#endif
} IMAGEID_LIST_PHONE_BOOK;

typedef struct
{
    kal_uint8 groupName[(MAX_GRP_NAME_LENGTH + 1) * ENCODING_LENGTH];
    kal_uint16 ringToneID;
    kal_uint16 pictureTagID;
    kal_uint16 LEDPatternId;
    kal_uint16 VideoID; /*Originally "entryCount" is obselete, so change to store VideoID*/
    kal_uint16 black_list_flag_Id;
    kal_uint16 red_list_flag_Id;
    MMI_ALERT_TYPE alertType;
    kal_uint8 is_modified;
} PHB_CALLER_GROUP_STRUCT;

typedef struct l4c_result_struct
{
    kal_uint8 flag;
    kal_uint16 cause;
} l4c_result_struct;

typedef struct
{
    kal_uint8 type;
    kal_uint8 length;
    kal_uint8 number[MAX_CC_ADDR_LEN];
} l4c_number_struct;

typedef struct
{
    kal_uint8 rtc_sec;
    kal_uint8 rtc_min;
    kal_uint8 rtc_hour;
    kal_uint8 rtc_day;
    kal_uint8 rtc_mon;
    kal_uint8 rtc_wday;
    kal_uint8 rtc_year;
} rtc_format_struct;

typedef struct l4_name_struct
{
    kal_uint8 name_length;
    kal_uint8 name_dcs;
    kal_uint8 name[32];
} l4_name_struct;

typedef struct
{
    l4c_number_struct tel;
    l4_name_struct alpha_id;
    rtc_format_struct time;
    kal_uint8 count;
} l4c_phb_ln_entry_struct;

#ifdef RWAPMMS_SUPPORT
#define NVRAM_WAP_PROFILE_HOMEPAGE_URL_LEN 256
#define NVRAM_WAP_PROFILE_USERNAME_LEN 31
#define NVRAM_WAP_PROFILE_PASSWORD_LEN 31
typedef struct
{
    U8 editable;
    U8 empty;
    U16 proxy_port;
    U8 homepage_url[NVRAM_WAP_PROFILE_HOMEPAGE_URL_LEN];
    U8 data_account;
    U8 conn_type;
    U8 proxy_ip[4];
    U8 username[NVRAM_WAP_PROFILE_USERNAME_LEN];
    U8 password[NVRAM_WAP_PROFILE_PASSWORD_LEN];
} nvram_wap_profile_content_struct;
typedef struct
{
    U8 conn_type;
    U8 dcs;
    U8 profile_name[NVRAM_WAP_PROFILE_NAME_LEN];
} nvram_wap_profile_name_struct;

typedef struct
{
    U8 conn_type;
    U8 dcs;
    U8 profile_name[NVRAM_MMS_PROFILE_NAME_LEN];
} nvram_mms_profile_name_struct;

#endif

#endif

#ifndef CHIP_IS_MODEM

#define LOCAL_PARA_HDR \
    U8 ref_count;      \
    U16 msg_len;

typedef struct
{
    LOCAL_PARA_HDR
    l4c_result_struct result;
    kal_uint8 no_list;
    kal_uint8 type;
    kal_uint8 more_data;
    l4c_phb_ln_entry_struct list[CALLLOG_MAX_RECORD * ADP_SIM_NUMBER];
} mmi_adp_calllog_data_struct;

#define MMI_LEDLIGHT_STATUS_ENUM U8

#define NO_COLOR 0
#define COLOR_PATTERN1 1
#define COLOR_PATTERN2 2
#define COLOR_PATTERN3 3
#define COLOR_PATTERN4 4
#define COLOR_PATTERN5 5
#define COLOR_PATTERN6 6

typedef struct
{
    U8 setsec;
} SET_TIMER;

typedef struct
{
    MMI_LEDLIGHT_STATUS_ENUM status;
    pBOOL lcdBacklight;
    SET_TIMER timer;
} LIGHT;

typedef struct
{
    U16 ringTone;
    U16 powerOffTone;
    U16 powerOnTone;
    U16 coverOpenTone;
    U16 coverCloseTone;
    U16 messageTone;
    U8 keypadTone;
    U16 alarmTone;
#ifdef __MMI_DUAL_PROFILE_SUPPORT__ //Add by Renyh for 221 project   20090213
    U16 card2_ringTone;
    U16 card2_messageTone;
#endif //Add by Renyh for 221 project   20090213
} TONE_SETUP;

typedef struct
{
    BOOL *coverAnswer;
    BOOL *anyKey;
    BOOL *automatic;
} MMI_ANSWERING_MODE;

#define MMI_FONT_SIZE_ENUM U8

#define THEME_DEFAULT 0
#define SMALL 1
#define MEDIUM 2
#define LARGE 3

typedef struct
{
    BOOL *errorTone;
    BOOL *connectTone;
    BOOL *campOnTone;
    BOOL *warningTone;
} EXTRA_TONE;
#define MMI_VOLUME_LEVEL_ENUM U8
typedef struct
{
    MMI_VOLUME_LEVEL_ENUM ringVolumeLevel;
    MMI_VOLUME_LEVEL_ENUM keypadVolumeLevel;
    MMI_VOLUME_LEVEL_ENUM loudSpeakerVolumeLevel;
    MMI_ALERT_TYPE mtCallAlertTypeEnum;
    LIGHT light;
#ifdef __MMI_KEYPAD_LIGHT_SETTIMER_SUPPORT__
    LIGHT keypadLight;
#endif
    U8 ringTypeEnum;
    TONE_SETUP toneSetup;
    MMI_ANSWERING_MODE answeringMode;
    U8 fontSizeEnum;
    MMI_INTELLIGENT_CALL_ALERT_TYPE intelligentCallAlert;
    EXTRA_TONE extraTone;
} PROFILE;

#endif
typedef enum {
    USRGEN_STATUS_INIT,
    USRGEN_STATUS_NEW,
    USRGEN_STATUS_FIRST,
    USRGEN_STATUS_NEXT,
    USRGEN_STATUS_LAST,
    USRGEN_STATUS_EXIT,
    USRGEN_STATUS_ERROR,
} USRGEN_REFAB_STATUS;

#ifdef USER_DATA_SUPPORT_PREFAB
typedef struct _usrgen_node
{
    char dir_name[FS_FILE_NAME_LEN + 1];
    DIR *dp;
} USRGEN_NODE;
#endif
typedef struct _usrgen_mount_point
{
    char dev_name[16];
    char point[FS_PATH_LEN + 1];
} USRGEN_MOUNT_POINT;

INT32 usrgen_fs_init(void);
INT32 usrgen_wap_mms(void);
INT32 usrgen_nvram(void);
INT32 usrgen_prefab_file(void);

#endif

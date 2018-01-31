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

#ifndef _GATT_H_
#define _GATT_H_


//GAP AD type from Generic Access Profile of Assigned Numbers
#define GAP_ADTYPE_FLAGS 0x01                                                      //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES
#define GAP_ADTYPE_16BIT_MORE 0x02                                            //!< Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_COMPLETE 0x03                                    //!< Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_32BIT_MORE 0x04                                            //!< Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_COMPLETE 0x05                                    //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_128BIT_MORE 0x06                                          //!< Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_COMPLETE 0x07                                  //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_LOCAL_NAME_SHORT 0x08                              //!< Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE 0x09                        //!< Complete local name
#define GAP_ADTYPE_POWER_LEVEL 0x0A                                         //!< TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE 0x0D                         //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC 0x0E               //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR 0x0F               //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_SM_TK 0x10                                                     //!< Security Manager TK Value
#define GAP_ADTYPE_SM_OOB_FLAG 0x11                                        //!< Secutiry Manager OOB Flags
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE 0x12            //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADTYPE_SIGNED_DATA 0x13                                        //!< Signed Data field
#define GAP_ADTYPE_SERVICES_LIST_16BIT 0x14                           //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT 0x15                         //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICE_DATA 0x16                                       //!< Service Data
#define GAP_ADTYPE_APPEARANCE 0x19                                          //!< Appearance
#define GAP_ADTYPE_ADVERTISING_INTERVAL  0x1A                         //!<Advertising Interval?
#define GAP_ADTYPE_LE_BLUETOOTH_DEVICE_ADDRESS 0x1B  //!<LE Bluetooth Device Address?
#define GAP_ADTYPE_LE_ROLE 0x1C                                            //!<LE Role?
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC 0xFF                    //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data

//GAP Appearance Characteristic from Assigned Numbers of Bluetooth.org
/*!!!NOTE:The values are composed of a category (10-bits) and sub-categories (6-bits)!!!.*/
#define GAP_APPEARANCE_CHAR_UNKNOW                                                      0
#define GAP_APPEARANCE_CHAR_GENERIC_PHONE                                         1//64
#define GAP_APPEARANCE_CHAR_GENERIC_COMPUTER                                   2 //128

#define GAP_APPEARANCE_CHAR_GENERIC_WATCH                                        3//192
#define GAP_APPEARANCE_CHAR_SUB_SPORTS_WATCH                                             1//193

#define GAP_APPEARANCE_CHAR_GENERIC_CLOCK                                          4 //256
#define GAP_APPEARANCE_CHAR_GENERIC_DISPLAY                                      5//320
#define GAP_APPEARANCE_CHAR_GENERIC_REMOTE_CONTROL                       6//384                
#define GAP_APPEARANCE_CHAR_GENERIC_EYE_GLASSES                              7//448
#define GAP_APPEARANCE_CHAR_GENERIC_TAG                                              8//512
#define GAP_APPEARANCE_CHAR_GENERIC_KEYRING                                       9//576
#define GAP_APPEARANCE_CHAR_GENERIC_MEDIA_PLAYER                            10//640
#define GAP_APPEARANCE_CHAR_GENERIC_BARCODE_SCANNER                     11//704

#define GAP_APPEARANCE_CHAR_GENERIC_THERMOMETER                             12//768
#define GAP_APPEARANCE_CHAR_SUB_THERMOMETER_EAR                                        1//769

#define GAP_APPEARANCE_CHAR_GENERIC_HEART_RATE_SENSOR                  13//832
#define GAP_APPEARANCE_CHAR_SUB_HEART_RATE_SENSOR_BELT                           1//833

#define GAP_APPEARANCE_CHAR_GENERIC_BLOOD_PRESSURE                        14//896
#define GAP_APPEARANCE_CHAR_SUB_BLOOD_PRESSURE_ARM                                  1//897
#define GAP_APPEARANCE_CHAR_SUB_BLOOD_PRESSURE_WRIST                               2//898

#define GAP_APPEARANCE_CHAR_GENERIC_HID                                               15//960
#define GAP_APPEARANCE_CHAR_SUB_HID_KEYBOARD                                              1//961
#define GAP_APPEARANCE_CHAR_SUB_HID_MOUSE                                                    2//962
#define GAP_APPEARANCE_CHAR_SUB_HID_JOYSTICK                                                3//963
#define GAP_APPEARANCE_CHAR_SUB_HID_GAMEPAD                                                4//964
#define GAP_APPEARANCE_CHAR_SUB_HID_DIGITIZER_TABLET                                5//965
#define GAP_APPEARANCE_CHAR_SUB_HID_CARD_READER                                        6//966
#define GAP_APPEARANCE_CHAR_SUB_HID_DIGITAL_PEN                                         7//967
#define GAP_APPEARANCE_CHAR_SUB_HID_BARCODE_SCANNER                                8//968

#define GAP_APPEARANCE_CHAR_GENERIC_GLUCOSE_METER                          16//1024

#define GAP_APPEARANCE_CHAR_GENERIC_RUNNING_WALKING_SENSOR       17//1088
#define GAP_APPEARANCE_CHAR_SUB_RUNNING_WALKING_SENSOR_IN_SHOE           1//1089
#define GAP_APPEARANCE_CHAR_SUB_RUNNING_WALKING_SENSOR_ON_SHOE           2//1090
#define GAP_APPEARANCE_CHAR_SUB_RUNNING_WALKING_SENSOR_ON_HIP             3//1091

#define GAP_APPEARANCE_CHAR_GENERIC_CYCLING                                        18//1152
#define GAP_APPEARANCE_CHAR_SUB_CYCLING_COMPUTER                                           1//1153
#define GAP_APPEARANCE_CHAR_SUB_CYCLING_SPEED_SENSOR                                   2//1154
#define GAP_APPEARANCE_CHAR_SUB_CYCLING_CADENCE_SENSOR                              3//1155
#define GAP_APPEARANCE_CHAR_SUB_CYCLING_POWER_SENSOR                                  4//1156
#define GAP_APPEARANCE_CHAR_SUB_CYCLING_SPEED_CADENCE_SENSOR                  5//1157

#define GAP_APPEARANCE_CHAR_GENERIC_PULSE_OXIMETER                         49//3136
#define GAP_APPEARANCE_CHAR_SUB_OXIMETER_FINGERTIP                                        1//3137
#define GAP_APPEARANCE_CHAR_SUB_OXIMETER_WRIST_WORMN                                 2//3138

#define GAP_APPEARANCE_CHAR_GENERIC_WEIGHT_SCALE                             50//3200

#define GAP_APPEARANCE_CHAR_GENERIC_OUTDOOR_SPORTS_ACTIVITY       81//5184
#define GAP_APPEARANCE_CHAR_SUB_LOCATION_DISPLAY_DEVICE                               1//5185
#define GAP_APPEARANCE_CHAR_SUB_LOCATION_NAVIGATION_DISPLAY_DEVICE         2//5186
#define GAP_APPEARANCE_CHAR_SUB_LOCATION_POD                                                     3//5187
#define GAP_APPEARANCE_CHAR_SUB_LOCATION_NAVIGATION_POD                              4//5188


//PRIVACY Flag characteristic
#define GAP_PERIPHERAL_PRIVACY_FLAG_DISABLE      0
#define GAP_PERIPHERAL_PRIVACY_FLAG_ENABLE      1


//GATT Attribute Types
#define ATT_UUID_PRIMARY            0x2800
#define ATT_UUID_SECONDARY      0x2801
#define ATT_UUID_INCLUDE            0x2802
#define ATT_UUID_CHAR               0x2803

//GATT Characteristic Descriptors
#define ATT_UUID_CHAR_EXT           0x2900
#define ATT_UUID_CHAR_USER      0x2901
#define ATT_UUID_CLIENT         0x2902
#define ATT_UUID_SERVER         0x2903
#define ATT_UUID_CHAR_FORMAT        0x2904
#define ATT_UUID_CHAR_AGGREGATE 0x2905
#define ATT_UUID_EXTERNAL_REF       0x2907
#define ATT_UUID_REPORT_REF     0x2908

//GATT primary service UUID
#define ATT_UUID_GAP                0x1800   //Generic Access
#define ATT_UUID_GATT               0x1801   //Generic Attribute
#define ATT_UUID_IAS                0x1802   //Immediate Alert Service
#define ATT_UUID_LLS                0x1803   //Link Loss Service
#define ATT_UUID_TPS                0x1804   //Tx Power Service
#define ATT_UUID_CTS                0x1805   //Current Tim??e Service
#define ATT_UUID_RTUS               0x1806   //Reference Time Update Service
#define ATT_UUID_NDCS               0x1807   //Next DST Change Service
#define ATT_UUID_GLS                   0x1808   //Glucose Service
#define ATT_UUID_HTS                0x1809   //Health Thermometer Service
#define ATT_UUID_DIS                      0x180A   //Device Information Service
#define ATT_UUID_HRS                0x180D   //Heart Rate Service
#define ATT_UUID_PASS               0x180E   //Phone Alert Status Service
#define ATT_UUID_BAS                0x180F   //Battery Service
#define ATT_UUID_BLS                0x1810   //Blood Pressure Service
#define ATT_UUID_ANS                0x1811   //Alert Notification Service
#define ATT_UUID_HIDS               0x1812   //HID Service
#define ATT_UUID_SCPS               0x1813   //Scan Parameters Service
#define ATT_UUID_RSCS               0x1814   //Running Speed and Cadence Service
#define ATT_UUID_AIOS               0x1815   //Automation IO Service
#define ATT_UUID_CSCS               0x1816   //Cycling Speed and Cadence Service
#define ATT_UUID_CPS                0x1818   //Cycling Power Service
#define ATT_UUID_LNS                0x1819   //Location and Navigation Service
#define ATT_UUID_ESS                0x181A   //Environmental Sensing Service
#define ATT_UUID_BCS                0x181B   //Body Composition Service
#define ATT_UUID_UDS                0x181C   //User Data Service
#define ATT_UUID_WSS                0x181D   //Weight Scale Service
#define ATT_UUID_BMS                0x181E   //Bond Management Service
#define ATT_UUID_CGMS               0x181F   //Continuous Glucose Monitoring Service
#define ATT_UUID_IPSS               0x1820   //Internet Protocol Support Service
#define ATT_UUID_IPS                0x1821   //Indoor Positioning Service
#define ATT_UUID_PLXS               0x1822   //Pulse Oximeter Service


//GATT Characteristic Types
#define ATT_UUID_DEVICENAME         0x2A00
#define ATT_UUID_APPEARANCE         0x2A01
#define ATT_UUID_PRIVACY_FLAG              0x2A02
#define ATT_UUID_RECONN_ADDR               0x2A03
#define ATT_UUID_CONN_PARAMETER          0x2A04
#define ATT_UUID_SERVICE_CHANGE     0x2A05
#define ATT_UUID_ALERT_LEVEL                  0x2A06

#define ATT_UUID_BATTERY_LEVEL      0x2A19

//device information service characteristics uuid
#define ATT_UUID_SYSTEM_ID                 0x2A23
#define ATT_UUID_MODEL_NUMBER       0x2A24
#define ATT_UUID_SERIAL_NUMBER      0x2A25
#define ATT_UUID_FIRMWARE_REVISION  0x2A26
#define ATT_UUID_HARDWARE_REVISION  0x2A27
#define ATT_UUID_SOFTWARE_REVISION  0x2A28
#define ATT_UUID_MANUFACTURER_NAME  0x2A29
#define ATT_UUID_REGISTRATION_CERTIFICATE   0x2A2A
#define ATT_UUID_PNP_ID             0x2A50
#define ATT_UUID_INDOOR_POSITION_CONFIG             0x2AAD


#define ATT_FMT_SHORT_UUID      0x0001
#define ATT_FMT_FIXED_LENGTH        0x0002
#define ATT_FMT_WRITE_NOTIFY        0x0004
#define ATT_FMT_GROUPED         0x0008

#define ATT_PM_KEY_SIZE(x)          ((x<<8)&0xff00)

#define ATT_PM_READABLE               0x0001
#define ATT_PM_WRITEABLE                     0x0002
#define ATT_PM_AUTHENT_REQUIRED       0x0004
#define ATT_PM_AUTHORIZE_REQUIRED     0x0008
#define ATT_PM_ENCRYPTION_REQUIRED    0x0010
#define ATT_PM_AUTHENT_MITM_REQUERED 0x0020

//characteristic property definitions
#define ATT_CHARA_PROP_BROADCAST           0x01
#define ATT_CHARA_PROP_READ         0x02
#define ATT_CHARA_PROP_WWP          0x04    // WWP short for "write without response"
#define ATT_CHARA_PROP_WRITE               0x08
#define ATT_CHARA_PROP_NOTIFY              0x10
#define ATT_CHARA_PROP_INDICATE     0x20
#define ATT_CHARA_PROP_ASW          0x40    // ASW short for "Authenticated signed write"
#define ATT_CHARA_PROP_EX_PROP      0x80

//gatt client request opcode
#define GATT_PRIMARY_SERVICE_REQ            (0x0001<<8)
#define GATT_RELATIONSHIP_REQ               (0x0002<<8)
#define GATT_CHAR_REQ                       (0x0003<<8)
#define GATT_DISCOVER_DESCRIPTOR_REQ        (0x0004<<8)
#define GATT_CHAR_VALUE_READ_REQ            (0x0005<<8)
#define GATT_CHAR_VALUE_WRITE_REQ       (0x0006<<8)
#define GATT_CHAR_DESCRIPTOR_REQ            (0x0007<<8)

//subcode of each gatt client request
#define GATT_PS_ALL         0x01
#define GATT_PS_BY_UUID 0x02

#define GATT_CHAR_ALL       0x01
#define GATT_CHAR_BY_UUID   0x02

#define GATT_CHAR_VR            0x01
#define GATT_CHAR_VR_BY_UUID    0x02
#define GATT_CHAR_VR_LONG       0x03
#define GATT_CHAR_VR_MULTIPLE   0x04

#define GATT_CHAR_WWP           0x01
#define GATT_CHAR_SWWP      0x02
#define GATT_CHAR_W         0x03
#define GATT_CHAR_W_LONG        0x04
#define GATT_CHAR_RELIABLE_W    0x05

#define GATT_CHAR_DES_READ_S    0x01
#define GATT_CHAR_DES_READ_L    0x02
#define GATT_CHAR_DES_WRITE_S   0x03
#define GATT_CHAR_DES_WRITE_L   0x04

//message id to upper layer apps
#define GATT_CONNECT_CNF            0x11
#define GATT_DISCONNECT_IND     0x12
#define GATT_CLIENT_REQ_RSP     0x13
#define GATT_NOTIFICATION           0x14
#define GATT_INDICATION         0x15
#define GATT_IND_TIMEOUT            0x16
#define GATT_WRITE_NOTIFY           0x17

#define GATT_CLIENT_SUPPORT 0
typedef void (*gattValueChangeCb_t)(void *param);
typedef int (*gattModuleStateCb_t)(u_int16 msgId, void *param);

typedef struct
{
    t_bdaddr address;
    u_int16 aclHandle;
    u_int16 mtu;
    u_int8 result;
} gatt_connect_cnf_t;

typedef struct
{
    u_int16 aclHandle;
    u_int16 result;
} gatt_disconn_ind_t;


typedef struct
{
    u_int16 aclHandle;
} gatt_srv_ind_timeout_t;

typedef struct
{
    u_int16 valueHandle;
} gatt_srv_write_notify_t;

typedef struct
{
    u_int16 uuid;
    u_int16 handle;
    u_int32 length;
    u_int8 *value;
} gatt_chara_desc_t;

typedef struct
{
    u_int16 handle;
    u_int16 uuid;
    u_int16 valueHandle;
    u_int8 properties;
    int descNum;
    u_int8* value;
    gatt_chara_desc_t *pDescList;
} gatt_chara_t;

typedef struct
{
    u_int16 startHandle;
    u_int16 endHandle;
    u_int16 uuid;
    u_int16 charNum;
    gatt_chara_t *pCharaList;
} gatt_prime_service_t;

typedef struct
{
    u_int16 reqCode;
    u_int16 module;
    u_int16 startHandle;
    u_int16 endHandle;
    u_int16 uuid;
    u_int8  uuid_l[16];
    u_int16 offset;
    u_int16 attValueLen;
    u_int8  uuidFmt;
    u_int8* attValue;
} gatt_client_req_t;

typedef struct
{
    u_int16 reqCode;
    u_int16 payLoadLen;
    u_int8  result;
    u_int8  contiueFlag;
    u_int8* rspPayLoad;
} gatt_client_req_rsp_t;

typedef struct
{
    u_int16 length;
    u_int8* value;
} gatt_server_noti_t;

typedef struct
{
    u_int16 aclHandle;
    u_int16 reqCode;
    u_int16 length;
    u_int16 attHandle;
    u_int8* data;
} gatt_server_req_t;


typedef struct
{
    //u_int8    properties;
    //u_int16 handle;
    //u_int8 uuid[2];
    u_int8 value[5];
} gatt_chara_def_short_t;

typedef struct
{
    //u_int8    properties;
    //u_int16 handle;
    //u_int8 uuid[2];
    u_int8 value[19];
} gatt_chara_def_long_t;

typedef struct
{
    u_int8 value[6];
} gatt_include_value_short_t;

typedef struct
{
    u_int8 value[20];
} gatt_include_value_long_t;

typedef struct
{
    u_int16 configurationBits;
    t_bdaddr address;
} gatt_chara_ccb_t;

typedef struct
{
    u_int8 value[7];
} gatt_chara_format_t;

typedef struct
{
    u_int16 min_conn_interval;
    u_int16 max_conn_interval;
    u_int16 slave_latency;
    u_int16 conn_timeout;
} gatt_chara_pcp_t;

typedef struct
{
    //u_int16 startHandle;
    //u_int16 endHandle;
    u_int8 handle[4];
} gatt_chara_sc_t;

typedef union
{
    u_int16 uuid_s;
    u_int8  *uuid_l;
} gatt_uuid_t;

typedef struct
{
    u_int16 length;
    u_int16 permisssion;
    gatt_uuid_t uuid;
    u_int8  fmt;            //bitmap
    void *attValue;
    gattValueChangeCb_t cb;
} gatt_element_t;

/* le data message structure */
typedef struct st_gatt_data_msg
{
    unsigned char discov_mode;
    unsigned char adv_type;
    unsigned char noBrEdr;
    unsigned short  advMin;
    unsigned short  advMax;
    unsigned char name[48];
    unsigned char service_size;
    gatt_element_t *service;
} adp_gatt_data_msg_t;

//gatt server api
int gatt_add_service(gatt_element_t* pElementArray, u_int16 numElement);
int gatt_server_req(gatt_server_req_t *req);
void gatt_init_server(u_int16 module, gattModuleStateCb_t cb);
u_int32 gatt_add_sdp_record();

void gatt_add_basic_service();
int gatt_get_reconn_address(t_bdaddr *address);

//gatt client api
int gatt_register_module(t_bdaddr address, u_int8 module);
int gatt_unregister_module(u_int16 module);

int gatt_client_req(gatt_client_req_t *pClientReq);
int gatt_connect(u_int8 module, u_int16 psm);
int gatt_exchange_mtu(u_int8 module, u_int16 mtu);



#endif

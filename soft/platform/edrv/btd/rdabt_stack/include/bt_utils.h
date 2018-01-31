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



#ifndef __HOST_UTILS_H
#define __HOST_UTILS_H

#include "bt_ctrl.h"

void *rdabt_malloc(u_int size);
void rdabt_free(void *ptr);

#define RDABT_TIMER_ONESHOT 1
#define RDABT_TIMER_PERIODIC 2

#define A2DP_MAX_LINK                        2

u_int8 rdabt_timer_create(u_int32 timeoutTenths, void(*callbackFunc)(void *), void *callbackArg, u_int8 flags);
t_api rdabt_change_timer(u_int8 timerHandle, u_int32 timeoutTenths);
t_api rdabt_cancel_timer(u_int8 timerHandle);

/*  These are #defined to the smallest way to write/read u_intXX to/from a buffer on the platform
    The UINT8 version is included for completeness, and probably won't/shouldn't be used....   */
#ifndef BT_WRITE_LE_UINT8
#define BT_WRITE_LE_UINT8(ptr,val)  (((*((u_int8 *)(ptr)))=(val)))
#define BT_WRITE_LE_UINT16(ptr,val) (((*((u_int8 *)(ptr)))=((u_int8)(val))),((*((u_int8 *)((ptr)+1)))=((u_int8)((val)>>8))))
#define BT_WRITE_LE_UINT24(ptr,val) (((*((u_int8 *)(ptr)))=((u_int8)(val))),((*((u_int8 *)((ptr)+1)))=((u_int8)((val)>>8))),((*((u_int8 *)((ptr)+2)))=((u_int8)((val)>>16))))
#define BT_WRITE_LE_UINT32(ptr,val) (((*((u_int8 *)(ptr)))=((u_int8)(val))),((*((u_int8 *)((ptr)+1)))=((u_int8)((val)>>8))),((*((u_int8 *)((ptr)+2)))=((u_int8)((val)>>16))),((*((u_int8 *)((ptr)+3)))=((u_int8)((val)>>24))))

#define BT_READ_LE_UINT8(ptr)       ((u_int8)(((*((u_int8 *)(ptr))))))
#define BT_READ_LE_UINT16(ptr)      ((u_int16)(((u_int16)(*((u_int8 *)(ptr))))|((*((u_int8 *)((ptr)+1)))<<8)))
#define BT_READ_LE_UINT24(ptr)      ((u_int32)(((u_int32)(*((u_int8 *)(ptr))))|((*((u_int8 *)((ptr)+1)))<<8)|((*((u_int8 *)((ptr)+2)))<<16)))
#define BT_READ_LE_UINT32(ptr)      ((u_int32)(((u_int32)(*((u_int8 *)(ptr))))|((*((u_int8 *)((ptr)+1)))<<8)|((*((u_int8 *)((ptr)+2)))<<16)|((*((u_int8 *)((ptr)+3)))<<24)))
#endif

#define TOGGLE_ENDIANESS_UINT16(x)  ((((x)>>8)&0xFF)|(((x)<<8)&0xFF00))
#define TOGGLE_ENDIANESS_UINT32(x) ((((x)>>24)&0xFF)|(((x)>>8)&0xFF00)|(((x)<<8)&0xFF0000)|(((x)<<24)&0xFF000000))


/* Used for determining Local Supported Features of the Host Controller */
/* byte 0 */
#define SLOT_3_PACKETS_MASK                               0x01
#define SLOT_5_PACKETS_MASK                               0x02
#define ENCRYPTION_MASK                                   0x04
#define SLOT_OFFSET_MASK                                  0x08
#define TIMING_ACCURACY_MASK                              0x10
#define SWITCH_ROLE_MASK                                  0x20
#define HOLD_MODE_MASK                                    0x40
#define SNIFF_MODE_MASK                                   0x80
/* byte 1 */
#define PARK_MODE_MASK                                    0x01
#define RSSI_MASK                                         0x02
#define RATE_CHANGE_MASK                                  0x04
#define SCO_LINK_MASK                                     0x08
#define HV2_PACKETS_MASK                                  0x10
#define HV3_PACKETS_MASK                                  0x20
#define U_LAW_MASK                                        0x40
#define A_LAW_MASK                                        0x80
/* byte 2 */
#define CVSD_MASK                                         0x01
#define PAGING_SCHEME_MASK                                0x02
#define POWER_CONTROL_MASK                                0x04
#define TRANSPARENT_SYN_DATA_MASK                         0x08
/* skip flow control lag */
#define BROADCAST_ENCRYPTION_MASK                         0x80
/* byte 3 */
#define SCATTER_MODE_MASK                                 0x01
#define EDR_ACL_2MBPS_MODE_MASK                           0x02
#define EDR_ACL_3MBPS_MODE_MASK                           0x04
#define ENHANCED_INQ_SCAN_MASK                            0x08
#define INTERLACED_INQ_SCAN_MASK                          0x10
#define INTERLACED_PAGE_SCAN_MASK                         0x20
#define RSSI_WITH_INQ_RESULTS_MASK                        0x40
#define ESCO_LINK_MASK                                    0x80
/* byte 4 */
#define EV4_PACKETS_MASK                                  0x01
#define EV5_PACKETS_MASK                                  0x02
#define ABSENCE_MASK                                      0x04
#define AFH_CAPABLE_SLAVE_MASK                            0x08
#define AFH_CLASSIFICATION_SLAVE_MASK                     0x10
#define ALIAS_AUTHENTICATION_MASK                         0x20
#define LE_SUPPORTED_MASK                                 0x40
#define _3_SLOT_EDR_ACL_PACKETS_MASK                      0x80
/* byte 5 */
#define _5_SLOT_EDR_ACL_PACKETS_MASK                      0x01
#define SNIFF_SUBRATING_MASK                              0x02
#define PAUSE_ENCRYPTION_FEATURE_MASK                     0x04
#define AFH_CAPABLE_MASTER_MASK                           0x08
#define AFH_CLASSIFICATION_MASTER_MASK                    0x10
#define EDR_ESCO_2MBPS_MODE_MASK                          0x20
#define EDR_ESCO_3MBPS_MODE_MASK                          0x40
#define _3_SLOT_EDR_ESCO_PACKETS_MASK                     0x80
/* byte 6 */
#define EXTENDED_INQUIRY_RESPONSE_MASK                    0x01
#define SECURE_SIMPLE_PAIRING_MASK                        0x08
#define ENCAPSULATED_PDU_MASK                             0x10
#define ERRONEOUS_DATA_REPORTING_MASK                     0x20
#define NONFLUSHABLE_PACKET_BOUNDARY_FLAG_MASK            0x40
/* byte 7 */
#define LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT_MASK       0x01
#define INQUIRY_RESPONSE_TX_POWER_LEVEL_MASK              0x02
#define ENHANCED_POWER_CONTROL_MASK                       0x04
#define EXTENDED_FEATURE                                  0x80

/* host features */
/* byte 0 */
#define HOST_SECURE_SIMPLE_PAIRING_MASK                   0x01
#define HOST_LE_SUPPORTED_MASK                            0x02
#define HOST_SIMULTANEOUS_LE_AND_BREDR_MASK               0X03

/* byte 0 */
#define RDABT_Check_3_Slot(features)                    ( features[0] & SLOT_3_PACKETS_MASK )
#define RDABT_Check_5_Slot(features)                    ( features[0] & SLOT_5_PACKETS_MASK )
#define RDABT_Check_Encryption(features)                ( features[0] & ENCRYPTION_MASK     )
#define RDABT_Check_Slot_Offset(features)               ( features[0] & SLOT_OFFSET_MASK    )
#define RDABT_Check_Timing_Accuracy(features)           ( features[0] & TIMING_ACCURACY_MASK)
#define RDABT_Check_Role_Switch(features)               ( features[0] & SWITCH_ROLE_MASK    )
#define RDABT_Check_Hold_Mode(features)                 ( features[0] & HOLD_MODE_MASK      )
#define RDABT_Check_Sniff_Mode(features)                ( features[0] & SNIFF_MODE_MASK     )
/* byte 1 */
#define RDABT_Check_Park_State(features)                ( features[1] & PARK_MODE_MASK      )
#define RDABT_Check_RSSI(features)                      ( features[1] & RSSI_MASK           )
#define RDABT_Check_CQDDR(features)                     ( features[1] & RATE_CHANGE_MASK    )
#define RDABT_Check_Sco_Link(features)                  ( features[1] & SCO_LINK_MASK       )
#define RDABT_Check_HV2_Packets(features)               ( features[1] & HV2_PACKETS_MASK    )
#define RDABT_Check_HV3_Packets(features)               ( features[1] & HV3_PACKETS_MASK    )
#define RDABT_Check_U_Law(features)                     ( features[1] & U_LAW_MASK          )
#define RDABT_Check_A_Law(features)                     ( features[1] & A_LAW_MASK          )
/* byte 2 */
#define RDABT_Check_CVSD(features)                      ( features[2] & CVSD_MASK           )
#define RDABT_Check_Paging_System(features)             ( features[2] & PAGING_SCHEME_MASK  )
#define RDABT_Check_Power_Control(features)             ( features[2] & POWER_CONTROL_MASK  )
#define RDABT_Check_Transparent_Sync_Data(features)     ( features[2] & TRANSPARENT_SYN_DATA_MASK )
#define RDABT_Check_Broadcast_Encryption(features)      ( features[2] & BROADCAST_ENCRYPTION_MASK )
/* byte 3 */
#define RDABT_Check_Scatter_Mode(features)              ( features[3] & SCATTER_MODE_MASK   )
#define RDABT_Check_EDR_ACL_2Mbps_Mode(features)        ( features[3] & EDR_ACL_2MBPS_MODE_MASK   )
#define RDABT_Check_EDR_ACL_3Mbps_Mode(features)        ( features[3] & EDR_ACL_3MBPS_MODE_MASK   )
#define RDABT_Check_Enhanced_Inquiry_Scan(features)     ( features[3] & ENHANCED_INQ_SCAN_MASK )
#define RDABT_Check_Interlaced_Inquiry_Scan(features)   ( features[3] & INTERLACED_INQ_SCAN_MASK )
#define RDABT_Check_Interlaced_Page_Scan(features)      ( features[3] & INTERLACED_PAGE_SCAN_MASK )
#define RDABT_Check_RSSI_With_Inquiry_Results(features) ( features[3] & RSSI_WITH_INQ_RESULTS_MASK )
#define RDABT_Check_eSco_Link(features)                 ( features[3] & ESCO_LINK_MASK      )
/* byte 4 */
#define RDABT_Check_EV4_Packets(features)               ( features[4] & EV4_PACKETS_MASK    )
#define RDABT_Check_EV5_Packets(features)               ( features[4] & EV5_PACKETS_MASK    )
#define RDABT_Check_Absence_Masks(features)             ( features[4] & ABSENCE_MASK  )
#define RDABT_Check_AFH_Capable_Slave(features)         ( features[4] & AFH_CAPABLE_SLAVE_MASK )
#define RDABT_Check_AFH_Classification_Slave(features)  ( features[4] & AFH_CLASSIFICATION_SLAVE_MASK )
#define RDABT_Check_Alias_Authentication(features)      ( features[4] & ALIAS_AUTHENTICATION_MASK )
#define RDABT_Check_LE_SUPPORTED(features)            ( features[4] & LE_SUPPORTED_MASK )
#define RDABT_Check_3_SLOT_EDR_ACL_PACKETS(features)   ( features[4] & _3_SLOT_EDR_ACL_PACKETS_MASK   )
/* byte 5 */
#define RDABT_Check_5_SLOT_EDR_ACL_PACKETS(features)   ( features[5] & _5_SLOT_EDR_ACL_PACKETS_MASK   )
#define RDABT_Check_Sniff_Subrating(features)           ( features[5] & SNIFF_SUBRATING_MASK)
#define RDABT_Check_Pause_Encryption(features)          ( features[5] & PAUSE_ENCRYPTION_FEATURE_MASK)
#define RDABT_Check_AFH_Capable_Master(features)        ( features[5] & AFH_CAPABLE_MASTER_MASK )
#define RDABT_Check_AFH_Classification_Master(features) ( features[5] & AFH_CLASSIFICATION_MASTER_MASK )
#define RDABT_Check_EDR_ESCO_2MBPS_MODE(features)       ( features[5] & EDR_ESCO_2MBPS_MODE_MASK   )
#define RDABT_Check_EDR_ESCO_3MBPS_MODE(features)       ( features[5] & EDR_ESCO_3MBPS_MODE_MASK   )
#define RDABT_Check_3_SLOT_EDR_ESCO_PACKETS(features)  ( features[5] & _3_SLOT_EDR_ESCO_PACKETS_MASK   )
/* byte 6 */
#define RDABT_Check_Extend_Inquiry_Response(features)   ( features[6] & EXTENDED_INQUIRY_RESPONSE_MASK )
#define RDABT_Check_Secure_Simple_Pairing(features)     ( features[6] & SECURE_SIMPLE_PAIRING_MASK )
#define RDABT_Check_Encapsulated(features)              ( features[6] & ENCAPSULATED_PDU_MASK)
#define RDABT_Check_Erroneous_Data_Reporting(features)  ( features[6] & ERRONEOUS_DATA_REPORTING_MASK)
#define RDABT_Check_Nonflushable_Packet_Boundary_Flag(features)           ( features[6] & NONFLUSHABLE_PACKET_BOUNDARY_FLAG_MASK)

/* byte 7 */
#define RDABT_Check_Link_Supervision_Timeout_Changed_Event(features)      ( features[7] & LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT_MASK )
#define RDABT_Check_Inquiry_Response_TX_Power_Level(features)             ( features[7] & INQUIRY_RESPONSE_TX_POWER_LEVEL_MASK )
#define RDABT_Check_Enhanced_Power_Control(features)                      ( features[7] & ENHANCED_POWER_CONTROL_MASK )
#define RDABT_Check_Extended_Features(features)                           ( features[7] & EXTENDED_FEATURE )

/* host features */
/* byte 0 */
#define RDABT_Check_HOST_Secure_Simple_Pairing(features)                    ( features[0] & HOST_SECURE_SIMPLE_PAIRING_MASK )
#define RDABT_Check_HOST_LE_Supported(features)                             ( features[0] & HOST_LE_SUPPORTED_MASK)
#define RDABT_Check_HOST_Simultaneous_LE_BREDR(features)                    ( features[0] & HOST_SIMULTANEOUS_LE_AND_BREDR_MASK)

/* Class of Device */

/* Major Service Classes: bits 23-13 */
#define RDABT_LIMITEDDISCOVERYMODE_BIT              0x002000
#define RDABT_CODSERVICEBIT_NETWORKING              0x020000
#define RDABT_CODSERVICEBIT_RENDERING                   0x040000
#define RDABT_CODSERVICEBIT_CAPTURING                   0x080000
#define RDABT_CODSERVICEBIT_OBJECTTRANSFER          0x100000
#define RDABT_CODSERVICEBIT_AUDIO                       0x200000
#define RDABT_CODSERVICEBIT_TELEPHONY                   0x400000
#define RDABT_CODSERVICEBIT_INFORMATION             0x800000
/* Major Device Classes: bits 12-8 */
#define RDABT_CODMAJORDEVICE_MISCELLANEOUS          0x000000
#define RDABT_CODMAJORDEVICE_COMPUTER                   0x000100
#define RDABT_CODMAJORDEVICE_PHONE                  0x000200
#define RDABT_CODMAJORDEVICE_LANACCESPOINT          0x000300
#define RDABT_CODMAJORDEVICE_AUDIO                  0x000400
#define RDABT_CODMAJORDEVICE_PERIPHERAL             0x000500
#define RDABT_CODMAJORDEVICE_UNCLASSIFIED               0x001F00
/* Minor Device Classes: bits 7-2 */
/* Minor Device Class Field - Computer Major Class */
#define RDABT_CODMINORDEVICE_COMPUTER_UNCLASSIFIED  0x000000
#define RDABT_CODMINORDEVICE_COMPUTER_DESKTOP           0x000004
#define RDABT_CODMINORDEVICE_COMPUTER_SERVER            0x000008
#define RDABT_CODMINORDEVICE_COMPUTER_LAPTOP            0x00000C
#define RDABT_CODMINORDEVICE_COMPUTER_HANDHELDPCPDA 0x000010
#define RDABT_CODMINORDEVICE_COMPUTER_PALMSIZEDPCPDA    0x000014
/* Minor Device Class Field - Phone Major Class */
#define RDABT_CODMINORDEVICE_PHONE_UNCLASSIFIED     0x000000
#define RDABT_CODMINORDEVICE_PHONE_CELLULAR         0x000004
#define RDABT_CODMINORDEVICE_PHONE_CORDLESS         0x000008
#define RDABT_CODMINORDEVICE_PHONE_SMARTPHONE           0x00000C
#define RDABT_CODMINORDEVICE_PHONE_WIREDMODEMORGW       0x000010
/* Minor Device Class Field - LAN AP Major Class: bits 7 - 5*/
#define RDABT_CODMINORDEVICE_LANAP_FULLYAVAILABLE       0x000000
#define RDABT_CODMINORDEVICE_LANAP_1_17_UTILISED        0x000004
#define RDABT_CODMINORDEVICE_LANAP_17_33_UTILISED       0x000008
#define RDABT_CODMINORDEVICE_LANAP_33_50_UTILISED       0x00000C
#define RDABT_CODMINORDEVICE_LANAP_50_67_UTILISED       0x000010
#define RDABT_CODMINORDEVICE_LANAP_67_83_UTILISED       0x000014
#define RDABT_CODMINORDEVICE_LANAP_83_99_UTILISED       0x000018
#define RDABT_CODMINORDEVICE_LANAP_NOSERVICEAVAILABLE   0x00001C
/* Minor Device Class Field - LAN AP Major Class: bits 4 - 2*/
#define RDABT_CODMINORDEVICE_LANAP_UNCLASSIFIED     0x000000
/* Minor Device Class Field - Audio Major Class */
#define RDABT_CODMINORDEVICE_AUDIO_UNCLASSIFIED     0x000000
#define RDABT_CODMINORDEVICE_AUDIO_HEADSETPROFILE       0x000004
#define RDABT_CODMINORDEVICE_AUDIO_HANDSFREE            0x000008
#define RDABT_CODMINORDEVICE_AUDIO_MICROPHONE           0x000010
#define RDABT_CODMINORDEVICE_AUDIO_LOUDSPEAKER          0x000014
#define RDABT_CODMINORDEVICE_AUDIO_HEADPHONES           0x000018
#define RDABT_CODMINORDEVICE_AUDIO_PORTABLE_AUDIO       0x00001c
#define RDABT_CODMINORDEVICE_AUDIO_CAR_AUDIO            0x000020
#define RDABT_CODMINORDEVICE_AUDIO_SETTOP_BOX           0x000024
#define RDABT_CODMINORDEVICE_AUDIO_HIFI_AUDIO           0x000028


/* Service Class Macros */
#define RDABT_LIMITEDDISCOVERYMODE_SET(classOfDevice)  ((classOfDevice) &  RDABT_LIMITEDDISCOVERYMODE_BIT)
#define RDABT_IS_NETWORKINGSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_NETWORKING)
#define RDABT_IS_RENDERINGSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_RENDERING)
#define RDABT_IS_CAPTURINGSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_CAPTURING)
#define RDABT_IS_OBJECTTRANSFERSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_OBJECTTRANSFER)
#define RDABT_IS_AUDIOSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_AUDIO)
#define RDABT_IS_TELEPHONYSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_TELEPHONY)
#define RDABT_IS_INFORMATIONSERVICE(classOfDevice) ((classOfDevice) & RDABT_CODSERVICEBIT_INFORMATION)

/* Major Device Class Macros */
#define MAJORDEVICECLASS_MASK       0x00001F00 /* bits 12 - 8 */
#define RDABT_IS_PHONEMAJORDEVICE(classOfDevice)  ( ((classOfDevice) & MAJORDEVICECLASS_MASK) == RDABT_CODMAJORDEVICE_PHONE)

/* Minor Device Class Macros */
#define MINORDEVICECLASS_MASK       0x000000FC  /* bits 7 - 2 */
#define RDABT_IS_CORDLESSMINORDEVICE(classOfDevice) ( ((classOfDevice) & MINORDEVICECLASS_MASK) == RDABT_CODMINORDEVICE_PHONE_CORDLESS)
#define RDABT_IS_WIREDMODEDORGWMINORDEVICE(classOfDevice) ( ((classOfDevice) & MINORDEVICECLASS_MASK) == RDABT_CODMINORDEVICE_PHONE_WIREDMODEMORGW)


/* SCO types*/
#define RDABT_MGR_HV1PACKETTYPE 0x0020
#define RDABT_MGR_HV2PACKETTYPE 0x0040
#define RDABT_MGR_HV3PACKETTYPE 0x0080

/* Bitmasks for MGR_SetLinkFlags */
#define MGR_KEYREQ_HANDLE           0x0001
#define MGR_KEYREQ_IGNORE           0x0002
#define MGR_KEYREQ_REJECT           0x0004
#define MGR_PINREQ_HANDLE           0x0008
#define MGR_PINREQ_IGNORE           0x0010
#define MGR_PINREQ_REJECT           0x0020
#define MGR_CONNREQ_SCO_ACCEPT      0x0040
#define MGR_CONNREQ_SCO_IGNORE      0x0080
#define MGR_CONNREQ_SCO_REJECT      0x0100
#define MGR_CONNREQ_ACL_ACCEPT      0x0200
#define MGR_CONNREQ_ACL_IGNORE      0x0400
#define MGR_CONNREQ_ACL_REJECT      0x0800
#define MGR_CONNREQ_ACL_SWITCH      0x1200
#define MGR_CONNSETUP_ALLOW_SWITCH  0x2000
#define MGR_CONNSETUP_REFUSE_SWITCH 0x4000
#define MGR_CONNREQ_IGNORE_ALL      (MGR_CONNREQ_SCO_IGNORE|MGR_CONNREQ_ACL_IGNORE)
#define MGR_CONNREQ_ACCEPT_ALL      (MGR_CONNREQ_SCO_ACCEPT|MGR_CONNREQ_ACL_ACCEPT)
#define MGR_SECREQ_IGNORE_ALL       (MGR_KEYREQ_IGNORE|MGR_PINREQ_IGNORE)
#define MGR_SECREQ_HANDLE_ALL       (MGR_KEYREQ_HANDLE|MGR_PINREQ_HANDLE)
#define MGR_LINKFLAGS_IGNORE        (MGR_CONNREQ_IGNORE_ALL|MGR_SECREQ_IGNORE_ALL|MGR_CONNSETUP_ALLOW_SWITCH)


#define MGR_Slots_To_mSec(slots) ((slots)*5/8)
#define MGR_mSec_To_Slots(msec)  ((u_int32)((msec)*8/5))




/* API FUNCTIONS */


/*
    Utility functions
*/
#define MGR_ADDRSTRLEN 18 /* The length of the string returned by MGR_PrintBDAddress() */


APIDECL1 t_api APIDECL2 MGR_PrintBDAddress(t_bdaddr address, char *outBuffer);
APIDECL1 t_api APIDECL2 MGR_ParseHexBDAddress(const char *inString, t_bdaddr *outAddress);

APIDECL1 t_api APIDECL2 MGR_PrintLinkKey(u_int8 *linkey, char *outBuffer);

typedef struct
{
    u_int8 module_id;
    layer_ctrl_fun_t ctrl_fun;
    msg_dispatch_t msg_dispatch;
} BtStackModuleList;

#endif // __HOST_UTILS_H




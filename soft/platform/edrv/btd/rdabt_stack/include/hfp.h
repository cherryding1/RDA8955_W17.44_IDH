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


#ifndef __HFP_H__
#define __HFP_H__

#include "obex.h"
/*********************************************
 * HFP Class of Device
 *********************************************/
#define HANDS_FREE_UNIT_COD     0x200408
#define HANDS_FREE_AG_COD       0x200408
#define HEADSET_UNIT_COD        0x200404
#define HEADSET_AG_COD          0x200404


#define ACTIVE_PROFILE_HEADSET            0x01
#define ACTIVE_PROFILE_HEADSET_AG         0x02
#define ACTIVE_PROFILE_HANDSFREE          0x04
#define ACTIVE_PROFILE_HANDSFREE_AG       0x08

/*****************************************************************************/
/*                        Handsfree Feature Definitions                      */
/*****************************************************************************/
//AG supported features bit masks .
#define AG_FEATURE_THREE_WAY_CALLING                (1)
#define AG_FEATURE_NREC_FUNCTION                    (1<<1)
#define AG_FEATURE_VOICE_RECOGNITION                (1<<2)
#define AG_FEATURE_IN_BAND_RING                     (1<<3)
#define AG_FEATURE_VOICE_TAG                        (1<<4)
#define AG_FEATURE_REJECT_CALL                      (1<<5)
#define AG_FEATURE_ENHANCED_CALL_STATUS             (1<<6)
#define AG_FEATURE_ENHANCED_CALL_CONTROL            (1<<7)
#define AG_FEATURE_EXTENDED_ERROR_CODES             (1<<8)

/*Bits for HF_Modify_Remote_Supplementary_Feature(). */
//HF supported features bit masks .
#define HF_FEATURE_NREC_FUNCTION                    (1)
#define HF_FEATURE_CALL_WAIT                        (1<<1)
#define HF_FEATURE_CLIP                             (1<<2)
#define HF_FEATURE_VOICE_RECOGNITION                (1<<3)
#define HF_FEATURE_REMOTE_VOL                       (1<<4)
#define HF_FEATURE_ENHANCED_CALL_STATUS             (1<<5)
#define HF_FEATURE_ENHANCED_CALL_CONTROL            (1<<6)



/* second argument of HF_Call_Request */
enum call_type
{
    /* use by HF_Call_Request */
    HF_CALL_INCOMMING,
    HF_CALL_OUTGOING,
    HF_CALL_START,
    HF_CALL_ALERT,
    HF_CALL_RELEASE,
    HF_CALL_NUM,
    HF_CALL_MEM,
    HF_CALL_REDIAL,
    HF_CALL_SWAP,
    HF_CALL_HOLD,

    /* use by HF_Call_Answer */
    HF_CALL_ACCEPT,
    HF_CALL_REJECT,

    /* use by HF_Call_Hold */
    HF_CALL_HELD,
    HF_CALL_WAIT,
    HF_CALL_ACTIVE,
    /* use by HF_Call_Status */
    HF_SERVICE_INDICATOR,
    HF_CALL_INDICATOR,
    HF_CALLSETUP_INDICATOR,
    HF_CALLHOLD_INDICATOR,
    HF_BATTCHG_INDICATOR,
    HF_SIGNAL_INDICATOR,
    HF_ROAM_INDICATOR,

    /* use by HF_Voice_Recognition */
    HF_VR_START,
    HF_VR_STOP
};

/*CODEC NEGOTIATION  CODEC ID'S*/
enum codec_id
{
    HF_VOC_CVSD = 1,
    HF_VOC_MSBC,
    HF_VOC_UNKNOW
};



/*Bits for HF_Multiparty_Call_Handling() */
typedef enum
{
    PARTY_CURR_ACTIVE_OTHER_DISMISS = 0,
    PARTY_CURR_RELEASE_OTHER_ACCEPT,
    PARTY_CURR_HOLD_OTHER_ACCEPT,
    PARTY_JOIN_3WAY,
    PARTY_LEAVE_3WAY
} HF_PART_CALL_TYPE;

typedef struct
{
    t_bdaddr       bt_addr;
    u_int16        connection_id;
    u_int8         profile;
    u_int8         result;
} t_hfp_connect_ind;

typedef struct
{
    u_int8         gain;
    u_int8         profile;
    u_int16        connection_id;
} t_hfp_gain_ind;

typedef struct
{
    u_int8          type;
    u_int8          value;
    u_int16         connection_id;
    u_int8          call_end;
} t_hfp_call_ind;
typedef struct
{
    u_int8          num_call;
    u_int8          call_dir;
    u_int16         call_status;
    u_int8    num_Phone[48];
    u_int8  num_length;
} t_hfp_clcc_cnf_info;
#ifdef __cplusplus
extern "C" {
#endif

// must call in the bt task
APIDECL1 t_api APIDECL2 HF_Activate(u_int8 active_profile);
APIDECL1 t_api APIDECL2 HF_Deactivate(u_int8 active_profile);
APIDECL1 t_api APIDECL2 HF_Connect_SLC(u_int8 call_active, t_bdaddr device_addr, u_int8 active_profile);
APIDECL1 t_api APIDECL2 HF_Accept_SLC(u_int8 call_active, t_bdaddr ag_bdaddr);
APIDECL1 t_api APIDECL2 HF_Reject_SLC(t_bdaddr ag_bdaddr);
APIDECL1 t_api APIDECL2 HF_Release_SLC(t_bdaddr ag_bdaddr);

APIDECL1 t_api APIDECL2 HF_ServiceDiscover(t_bdaddr address, u_int8 active_profile);

APIDECL1 u_int APIDECL2 HF_Get_Current_Profile(void);
APIDECL1 u_int APIDECL2 HF_Get_Device_State(void);
APIDECL1 u_int APIDECL2 HF_Get_Current_Call_Status(void);
APIDECL1 t_api APIDECL2 HF_Get_Current_BdAddr(t_bdaddr *addr);
APIDECL1 t_api APIDECL2 HF_Stop_Ring(void);

APIDECL1 t_api APIDECL2 HF_Send_Raw_Data(u_int8 *data, int dataLen);

APIDECL1 t_api APIDECL2 HF_Send_Mic_Gain(u_int8 gain) ;
APIDECL1 t_api APIDECL2 HF_Send_Speaker_Gain(u_int8 gain);

APIDECL1 t_api APIDECL2 HF_Send_Button_Press(void);
APIDECL1 t_api APIDECL2 HF_Enable_Band_Ring(u_int8 enabled);

APIDECL1 t_api APIDECL2 HF_Set_Number(u_int8 *number);
APIDECL1 t_api APIDECL2 HF_Get_Number(u_int8 *number, u_int8 *length);
APIDECL1 t_api APIDECL2 HF_Set_MemorySize(u_int16 mem_size, u_int8 history_size);
APIDECL1 t_api APIDECL2 HF_Set_NetworkOperator(u_int8 *netop);
APIDECL1 t_api APIDECL2 HF_Get_NetworkOperator(void);
APIDECL1 t_api APIDECL2 HF_Get_CallStatus(u_int16 *status);

APIDECL1 t_api APIDECL2 HF_Call_Request(u_int8 type, u_int8 mode);
APIDECL1 t_api APIDECL2 HF_Call_Answer(u_int8 type);
APIDECL1 t_api APIDECL2 HF_Call_Release(void);
APIDECL1 t_api APIDECL2 HF_Call_Hold(u_int8 type);
APIDECL1 t_api APIDECL2 HF_Call_Status(u_int8 type, u_int8 value);
APIDECL1 t_api APIDECL2 HF_Terminate_Call(void);
APIDECL1 t_api APIDECL2 HF_Voice_Recognition(u_int8 type);
APIDECL1 t_api APIDECL2 HF_Get_Clcc(void);
APIDECL1 t_api APIDECL2 HF_Send_Cnum(void);
APIDECL1 t_api APIDECL2 HF_Send_BTRH(void);
APIDECL1 t_api APIDECL2 HF_Send_NREC(u_int8 type);
APIDECL1 t_api APIDECL2 HF_Send_BINP_2(u_int8 type);
APIDECL1 t_api APIDECL2 HF_Send_DTMF( char* dtmf_sequence );
APIDECL1 t_api APIDECL2 HF_Send_NoCarrier(void);
APIDECL1 t_api APIDECL2 HF_Set_PhoneBook_PATH(enum PhoneBook_Place which, enum PhoneBook_Type type);
APIDECL1 t_api APIDECL2 HF_Set_PhoneBook_UTF8(void);
APIDECL1 t_api APIDECL2 HF_Get_PhoneBook(u_int32 start, u_int32 num);
APIDECL1 t_api APIDECL2 HF_Activate_CLIP(void);
APIDECL1 t_api APIDECL2 hfp_set_indicator_status(u_int8 *array);
int HF_Send_Indicators_direct(u_int8 indicator_type, u_int8 indicator_value);
#define RDABT_HF_Activate(  active_profile)                   HF_Activate(  active_profile)
#define RDABT_HF_Deactivate(  active_profile)                 HF_Deactivate(  active_profile)
#define RDABT_HF_Reject_SLC(  ag_bdaddr)                      HF_Reject_SLC(  ag_bdaddr)
#define RDABT_HF_ServiceDiscover(  address,  active_profile)  HF_ServiceDiscover(  address,  active_profile)
#define RDABT_HF_Get_Current_Profile                          HF_Get_Current_Profile
#define RDABT_HF_Get_Device_State                             HF_Get_Device_State
#define RDABT_HF_Get_Current_Call_Status                      HF_Get_Current_Call_Status
#define RDABT_HF_Get_Current_BdAddr (  addr)                  HF_Get_Current_BdAddr (  addr)
#define RDABT_HF_Send_Raw_Data(   data,  dataLen)             HF_Send_Raw_Data(   data,  dataLen)
#define RDABT_HF_Send_Mic_Gain(  gain)                        HF_Send_Mic_Gain(  gain)
#define RDABT_HF_Send_Speaker_Gain(  gain)                    HF_Send_Speaker_Gain(  gain)
#define RDABT_HF_Send_Button_Press                            HF_Send_Button_Press
#define RDABT_HF_Set_Number(   number)                        HF_Set_Number(   number)
#define RDABT_HF_Call_Answer(  type)                          HF_Call_Answer(  type)
#define RDABT_HF_Call_Hold(  type)                            HF_Call_Hold(  type)
#define RDABT_HF_Voice_Recognition(  type)                    HF_Voice_Recognition(  type)

APIDECL1 t_api APIDECL2 RDABT_HF_Connect_SLC(t_bdaddr device_addr);
APIDECL1 t_api APIDECL2 RDABT_HF_Accept_SLC(t_bdaddr ag_bdaddr);
APIDECL1 u_int APIDECL2 RDABT_HF_Release_SLC(t_bdaddr ag_bdaddr);
//APIDECL1 t_api APIDECL2 RDABT_HF_Call_Request(u_int8 type);

APIDECL1 t_api RDABT_HF_Call_Request(t_hfp_call_ind * call_info);

APIDECL1 t_api APIDECL2 RDABT_HF_Call_Status(u_int8 type, u_int8 value);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif


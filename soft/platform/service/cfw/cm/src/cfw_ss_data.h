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





#if !defined(CFW_SS_DATA_H)
#define CFW_SS_DATA_H

#define SS_INVALID_TI  0xFF
#define NO_SUCH_CODE   0xFF


typedef struct _SS_SM_INFO
{
    UINT8 nTI;
    UINT8 nIndex;
    UINT16 nUTI_Mmi; // Qiaoguangjun[+] for check the nUTI
    VOID *pMsg;
    UINT8 nClass;//mmi_class
    UINT8 nUSSDFlag; //0: ss_USSDNetworkInvoke  1:ss_USSDNetworkReturnResult 0x02:ss_USSDNetworkNoComponentPresent
    UINT16 nApi_SS_Code;
    UINT16 nApi_SS_Operation;
} SS_SM_INFO ;
typedef enum
{
    USSD_AO_State_idle,
    USSD_AO_State_requst,
    USSD_AO_State_reject,
    USSD_AO_State_error,
    USSD_AO_State_release



} T_USSD_AO_State;

typedef enum
{
    COMM_SS_ALL_TELESERVICES = 10,
    // COMM_SS_ALL_SPEECH_TELESERVICES = 0x10,
    COMM_SS_TELEPHONY = 11,
    //  COMM_SS_EMERGENCY_CALLS = 0x12,
    COMM_SS_ALL_SMS = 16,
    //  COMM_SS_SM_MT_PP = 0x21,
    //  COMM_SS_SM_MO_PP = 0x22,
    COMM_SS_ALL_FAX_SERVICES = 13,
    //  COMM_SS_FAX_GROUP3_ALTER_SPEECH = 0x61,
    //  COMM_SS_AUTO_FAX_GROUP3 = 0x62,
    //  COMM_SS_FAX_GROUP4 = 0x63,
    COMM_SS_ALL_DATA_TELESERVICES = 12,
    COMM_SS_ALL_TELESERVICES_XCPT_SMS = 19,
    //  COMM_SS_ALL_VOICE_GRP_CALL_SERVICES = 0x90,
    COMM_SS_VOICE_GROUP_CALL = 17,
    COMM_SS_VOICE_BROADCAST_CALL = 18,
    COMM_SS_allPLMN_specificTS = 50,
    COMM_SS_plmn_specificTS_1 = 51,
    COMM_SS_plmn_specificTS_2 = 52,
    COMM_SS_plmn_specificTS_3 = 53,
    COMM_SS_plmn_specificTS_4 = 54,
    COMM_SS_plmn_specificTS_5 = 55,
    COMM_SS_plmn_specificTS_6 = 56,
    COMM_SS_plmn_specificTS_7 = 57,
    COMM_SS_plmn_specificTS_8 = 58,
    COMM_SS_plmn_specificTS_9 = 59,
    COMM_SS_plmn_specificTS_A = 60,
    COMM_SS_plmn_specificTS_B = 61,
    COMM_SS_plmn_specificTS_C = 62,
    COMM_SS_plmn_specificTS_D = 63,
    COMM_SS_plmn_specificTS_E = 64,
    COMM_SS_plmn_specificTS_F = 65,
    COMM_SS_INVALID_TELESERVICE = 0xff
} T_COMM_eSsTeleServiceCode;
typedef enum
{
    COMM_SS_AllBearerServices = 20,
    // COMM_SS_AllDataCDAServices = 0x10,
    // COMM_SS_DataCDA_300bps = 0x11,
    // COMM_SS_DataCDA_1200bps = 0x12,
    // COMM_SS_DataCDA_1200_75bps = 0x13,
    // COMM_SS_DataCDA_2400bps = 0x14,
    // COMM_SS_DataCDA_4800bps = 0x15,
    // COMM_SS_DataCDA_9600bps = 0x16,
    // COMM_SS_general_dataCDA = 0x17,
    // COMM_SS_AllDataCDSServices = 0x18,
    // COMM_SS_DataCDS_1200bps = 0x1A,
    // COMM_SS_DataCDS_2400bps = 0x1C,
    // COMM_SS_DataCDS_4800bps = 0x1D,
    // COMM_SS_DataCDS_9600bps = 0x1E,
    // COMM_SS_general_dataCDS = 0x1F,
    // COMM_SS_AllPadAccessCAServices = 0x20,
    // COMM_SS_PadAccessCA_300bps = 0x21,
    // COMM_SS_PadAccessCA_1200bps = 0x22,
    // COMM_SS_PadAccessCA_1200_75bps = 0x23,
    // COMM_SS_PadAccessCA_2400bps = 0x24,
    // COMM_SS_PadAccessCA_4800bps = 0x25,
    // COMM_SS_PadAccessCA_9600bps = 0x26,
    // COMM_SS_general_padAccessCA = 0x27,
    COMM_SS_allGPRS_BearerServices = 99,
    //COMM_SS_ALLDataPDSServices = 0x28,
    // COMM_SS_DataPDS_2400bps = 0x2C,
    // COMM_SS_DataPDS_4800bps = 0x2D,
    // COMM_SS_DataPDS_9600bps = 0x2E,
    // COMM_SS_general_dataPDS = 0x2F,
    // COMM_SS_allAlternateSpeech_DataCDA = 0x30,
    // COMM_SS_allAlternateSpeech_DataCDS = 0x38,
    // COMM_SS_allSpeechFollowedByDataCDA = 0x40,
    // COMM_SS_allSpeechFollowedByDataCDS = 0x48,
    COMM_SS_allDataCircuitAsynchronous = 25,
    COMM_SS_allAsynchronousServices = 21,
    COMM_SS_allDataCircuitSynchronous = 24,
    COMM_SS_allSynchronousServices = 22,
    COMM_SS_allPLMN_specificBS = 70,
    COMM_SS_plmn_specificBS_1 = 71,
    COMM_SS_plmn_specificBS_2 = 72,
    COMM_SS_plmn_specificBS_3 = 73,
    COMM_SS_plmn_specificBS_4 = 74,
    COMM_SS_plmn_specificBS_5 = 75,
    COMM_SS_plmn_specificBS_6 = 76,
    COMM_SS_plmn_specificBS_7 = 77,
    COMM_SS_plmn_specificBS_8 = 78,
    COMM_SS_plmn_specificBS_9 = 79,
    COMM_SS_plmn_specificBS_A = 80,
    COMM_SS_plmn_specificBS_B = 81,
    COMM_SS_plmn_specificBS_C = 82,
    COMM_SS_plmn_specificBS_D = 83,
    COMM_SS_plmn_specificBS_E = 84,
    COMM_SS_plmn_specificBS_F = 85,
    COMM_SS_INVALID_BearerServices = 0xff
} T_COMM_eSsBearerServiceCode;

typedef enum
{
    LOCAL_SS_ALL_TELESERVICES = 0,
    LOCAL_SS_ALL_SPEECH_TELESERVICES = 0x10,
    LOCAL_SS_TELEPHONY = 0x11,
    LOCAL_SS_EMERGENCY_CALLS = 0x12,
    LOCAL_SS_ALL_SMS = 0x20,
    LOCAL_SS_SM_MT_PP = 0x21,
    LOCAL_SS_SM_MO_PP = 0x22,
    LOCAL_SS_ALL_FAX_SERVICES = 0x60,
    LOCAL_SS_FAX_GROUP3_ALTER_SPEECH = 0x61,
    LOCAL_SS_AUTO_FAX_GROUP3 = 0x62,
    LOCAL_SS_FAX_GROUP4 = 0x63,
    LOCAL_SS_ALL_DATA_TELESERVICES = 0x70,
    LOCAL_SS_ALL_TELESERVICES_XCPT_SMS = 0x80,
    LOCAL_SS_ALL_VOICE_GRP_CALL_SERVICES = 0x90,
    LOCAL_SS_VOICE_GROUP_CALL = 0x91,
    LOCAL_SS_VOICE_BROADCAST_CALL = 0x92,
    LOCAL_SS_allPLMN_specificTS = 0xD0,
    LOCAL_SS_plmn_specificTS_1 = 0xD1,
    LOCAL_SS_plmn_specificTS_2 = 0xD2,
    LOCAL_SS_plmn_specificTS_3 = 0xD3,
    LOCAL_SS_plmn_specificTS_4 = 0xD4,
    LOCAL_SS_plmn_specificTS_5 = 0xD5,
    LOCAL_SS_plmn_specificTS_6 = 0xD6,
    LOCAL_SS_plmn_specificTS_7 = 0xD7,
    LOCAL_SS_plmn_specificTS_8 = 0xD8,
    LOCAL_SS_plmn_specificTS_9 = 0xD9,
    LOCAL_SS_plmn_specificTS_A = 0xDA,
    LOCAL_SS_plmn_specificTS_B = 0xDB,
    LOCAL_SS_plmn_specificTS_C = 0xDC,
    LOCAL_SS_plmn_specificTS_D = 0xDD,
    LOCAL_SS_plmn_specificTS_E = 0xDE,
    LOCAL_SS_plmn_specificTS_F = 0xDF,
    LOCAL_SS_INVALID_TELESERVICE = 0xff
} T_LOCAL_eSsTeleServiceCode;
typedef enum
{
    LOCAL_SS_AllBearerServices = 0,
    LOCAL_SS_AllDataCDAServices = 0x10,
    LOCAL_SS_DataCDA_300bps = 0x11,
    LOCAL_SS_DataCDA_1200bps = 0x12,
    LOCAL_SS_DataCDA_1200_75bps = 0x13,
    LOCAL_SS_DataCDA_2400bps = 0x14,
    LOCAL_SS_DataCDA_4800bps = 0x15,
    LOCAL_SS_DataCDA_9600bps = 0x16,
    LOCAL_SS_general_dataCDA = 0x17,
    LOCAL_SS_AllDataCDSServices = 0x18,
    LOCAL_SS_DataCDS_1200bps = 0x1A,
    LOCAL_SS_DataCDS_2400bps = 0x1C,
    LOCAL_SS_DataCDS_4800bps = 0x1D,
    LOCAL_SS_DataCDS_9600bps = 0x1E,
    LOCAL_SS_general_dataCDS = 0x1F,
    LOCAL_SS_AllPadAccessCAServices = 0x20,
    LOCAL_SS_PadAccessCA_300bps = 0x21,
    LOCAL_SS_PadAccessCA_1200bps = 0x22,
    LOCAL_SS_PadAccessCA_1200_75bps = 0x23,
    LOCAL_SS_PadAccessCA_2400bps = 0x24,
    LOCAL_SS_PadAccessCA_4800bps = 0x25,
    LOCAL_SS_PadAccessCA_9600bps = 0x26,
    LOCAL_SS_general_padAccessCA = 0x27,
    LOCAL_SS_ALLDataPDSServices = 0x28,
    LOCAL_SS_DataPDS_2400bps = 0x2C,
    LOCAL_SS_DataPDS_4800bps = 0x2D,
    LOCAL_SS_DataPDS_9600bps = 0x2E,
    LOCAL_SS_general_dataPDS = 0x2F,
    LOCAL_SS_allAlternateSpeech_DataCDA = 0x30,
    LOCAL_SS_allAlternateSpeech_DataCDS = 0x38,
    LOCAL_SS_allSpeechFollowedByDataCDA = 0x40,
    LOCAL_SS_allSpeechFollowedByDataCDS = 0x48,
    LOCAL_SS_allDataCircuitAsynchronous = 0x50,
    LOCAL_SS_allAsynchronousServices = 0x60,
    LOCAL_SS_allDataCircuitSynchronous = 0x58,
    LOCAL_SS_allSynchronousServices = 0x68,
    LOCAL_SS_allPLMN_specificBS = 0xD0,
    LOCAL_SS_plmn_specificBS_1 = 0xD1,
    LOCAL_SS_plmn_specificBS_2 = 0xD2,
    LOCAL_SS_plmn_specificBS_3 = 0xD3,
    LOCAL_SS_plmn_specificBS_4 = 0xD4,
    LOCAL_SS_plmn_specificBS_5 = 0xD5,
    LOCAL_SS_plmn_specificBS_6 = 0xD6,
    LOCAL_SS_plmn_specificBS_7 = 0xD7,
    LOCAL_SS_plmn_specificBS_8 = 0xD8,
    LOCAL_SS_plmn_specificBS_9 = 0xD9,
    LOCAL_SS_plmn_specificBS_A = 0xDA,
    LOCAL_SS_plmn_specificBS_B = 0xDB,
    LOCAL_SS_plmn_specificBS_C = 0xDC,
    LOCAL_SS_plmn_specificBS_D = 0xDD,
    LOCAL_SS_plmn_specificBS_E = 0xDE,
    LOCAL_SS_plmn_specificBS_F = 0xDF,
    LOCAL_SS_INVALID_BearerServices = 0xff
} T_LOCAL_eSsBearerServiceCode;

// Supplementary Service
typedef enum
{
    COMM_SS_CLIP = 30,                      // CLIP
    COMM_SS_CLIR = 31,                      // CLIR
    COMM_SS_COLP = 76,                      // COLP
    COMM_SS_COLR = 77,                      // COLR
    COMM_SS_CNAP = 300,                     // CNAP


    COMM_SS_CFA = 4,                        // All forwarding
    COMM_SS_CFU = 0,                       // CF unconditional
    COMM_SS_CD = 66,                        // call deflection
    COMM_SS_CFC = 5,                        // CF conditional
    COMM_SS_CFB = 1,                       // CF on MS busy
    COMM_SS_CFNRY = 2,                     // CF on MS no reply
    COMM_SS_CFNRC = 3,                     // CF on MS not reachable

    // Operations Only
    COMM_SS_ECT = 96,                       // explicit call transfer

    COMM_SS_CW = 43,                        // call waiting
    COMM_SS_HOLD = 102,                     // call hold  (defined by CIITEC)


    COMM_SS_MPTY = 103,                     // multiparty (defined by CIITEC)

    COMM_SS_AOCI = 104,                     // advice of charge information (defined by CIITEC)
    COMM_SS_AOCC = 105,                     // advice of charge charging    (defined by CIITEC)


    COMM_SS_UUS1 = 361,                     // uus1
    COMM_SS_UUS2 = 362,                     // uus2
    COMM_SS_UUS3 = 363,                     // uus3

    COMM_SS_AllBarringSS = 16706,             // all call barring
    COMM_SS_BarringOfOutgoingCalls = 16711,   // barring of MO call
    COMM_SS_BAOC = 16719,                      // barring of all MO call
    COMM_SS_BOIC = 20297,                     // barring of international MO call
    COMM_SS_BOIC_ExHC = 20312,                // barring of international MO call
    // except those going to the home plmn
    COMM_SS_BarringOfIncomingCalls = 16707,   // barring of MT call
    COMM_SS_BAIC = 16713,                      // barring of all MT call
    COMM_SS_BIC_ROAM = 18770,                  // barring of MT call when roaming
    // outside of home plmn
    COMM_SS_USSD_V2 = 100,                  // USSD version 2 services, supports
    // all USSD Operations except
    // ss_ProcessUnstructuredSSData
    COMM_SS_USSD_V1 = 101,                  // USSD version 1 services, supports
    // ss_ProcessUnstructuredSSData

    COMM_SS_NO_CODE
} T_COMM_eSsCode;

typedef enum
{
    COMM_SS_TeleserviceCode = 0x83,
    COMM_SS_BearerServiceCode = 0x82,
    COMM_SS_ServicecodeNotApplicable = 0    // In many cases, this means that
                                       // the transaction applies to
                                       // ALL BasicServiceCodes
} T_COMM_eSsBasicServiceCodeCategory;

typedef struct
{
    T_COMM_eSsTeleServiceCode MMIBSCode;       //Basic service code input from MMI
    T_LOCAL_eSsTeleServiceCode ProtocolCode;    // Basic service code used in protocol
} T_COMM_sSsTeleBSCodeList;

typedef struct
{
    T_COMM_eSsBearerServiceCode MMIBSCode;       //Basic service code input from MMI
    T_LOCAL_eSsBearerServiceCode ProtocolCode;    // Basic service code used in protocol
} T_COMM_sSsBearBSCodeList;

typedef struct
{
    T_COMM_eSsCode MMISSCode;       // service code input from MMI
    api_SsCode_t ProSSCode;        //  service code used in protocol
} T_COMM_sSsServCodeList;

#endif


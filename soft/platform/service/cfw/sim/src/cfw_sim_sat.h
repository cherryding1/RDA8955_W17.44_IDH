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

#ifndef CFW_SIM_SAT_H
#define CFW_SIM_SAT_H

#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT                   4
#endif
#ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif



//Add for SIM sat command set by lixp at 20100806

#define SIM_SAT_FRESH_COM               0x01
#define SIM_SAT_REFRESH_COM             0x01
#define SIM_SAT_MORE_TIME_COM           0x02
#define SIM_SAT_POLL_INTERVAL_COM       0x03
#define SIM_SAT_POLLING_OFF_COM         0x04
#define SIM_SAT_SETUP_EVENT_LIST_COM    0x05
#define SIM_SAT_CALL_SETUP_COM          0x10
#define SIM_SAT_SEND_SS_COM             0x11
#define SIM_SAT_SEND_USSD_COM           0x12
#define SIM_SAT_SEND_SMS_COM            0x13
#define SIM_SAT_SEND_DTMF_COM           0x14
#define SIM_SAT_LAUNCH_BROWSER_COM      0x15
#define SIM_SAT_PLAY_TONE_COM           0x20
#define SIM_SAT_DISPLAY_TEXT_COM        0x21
#define SIM_SAT_GET_INKEY_COM           0x22
#define SIM_SAT_GET_INPUT_COM          0x23
#define SIM_SAT_SELECT_ITEM_COM         0x24
#define SIM_SAT_SETUP_MENU_COM          0x25
#define SIM_SAT_PROVIDE_LOCAL_INFO_COM  0x26
#define SIM_SAT_TIMER_MANAGEMENT_COM    0x27
#define SIM_SAT_SETUP_IDLE_DISPLAY_COM  0x28
#define SIM_SAT_CARD_APDU_COM           0x30
#define SIM_SAT_POWER_ON_CARD_COM       0x31
#define SIM_SAT_POWER_OFF_CARD_COM      0x32
#define SIM_SAT_GET_READER_STATUS_COM   0x33
#define SIM_SAT_RUN_AT_COMMAND_COM      0x34
#define SIM_SAT_LANG_NOTIFY_COM         0x35
#define SIM_SAT_OPEN_CHANNEL_COM        0x40
#define SIM_SAT_CLOSE_CHANNEL_COM       0x41
#define SIM_SAT_RECEIVE_DATA_COM        0x42
#define SIM_SAT_SEND_DATA_COM           0x43
#define SIM_SAT_GET_CHANNEL_STATUS_COM  0x44

#define SIM_SAT_PROACTIVE_SIM_COMMAND   0xD0
#define SIM_SAT_SMSPP_DOWNLOAD          0xD1
#define SIM_SAT_CB_DOWNLOAD             0xD2
#define SIM_SAT_MENU_SELECTION          0xD3
#define SIM_SAT_CALL_CONTROL            0xD4
#define SIM_SAT_MO_SMS_CONTROL          0xD5
#define SIM_SAT_EVENT_DOWNLOAD          0xD6
#define SIM_SAT_TIMER_EXPIRATION        0xD7


//BER-TLV tags in ME to SIM direction
#define SMS_PP_DOWNLOAD_TAG                      0xD1
#define CELL_BROADCAST_DOWNLOAD_TAG              0xD2
#define MENU_SELECTION_TAG                       0xD3
#define CALL_CONTROL_TAG                         0xD4
#define MO_SHORT_MSG_CONTROL_TAG                 0xD5
#define EVENT_DOWNLOAD_TAG                       0xD6
#define TIMER_EXPIRATION_TAG                     0xD7
#define RESERVED_TIA_EIA136_TAG                  0xDF
//SIMPLE_TLV tag
#define COMMAND_DETAILS_TAG                      0x01
#define DEVICE_IDENTITY_TAG                      0x02
#define RESULT_TAG                               0x03
#define DURATION_TAG                             0x04
#define ALPHA_IDENTIFIER_TAG                     0x05
#define ADDRESS_TAG                              0x06
#define CAPABILITY_CONFIG_PARAM_TAG       0x07
#define SUBADDRESS_TAG                           0x08
#define SS_STRING_TAG                            0x09
#define USSD_STRING_TAG                          0x0A
#define SMS_TPDU_TAG                             0x0B
#define CELL_BROADCAST_PAGE_TAG                  0x0C
#define TEXT_STRING_TAG                          0x0D
#define TONE_TAG                                 0x0E
#define ITEM_TAG                                 0x0F
#define ITEM_IDENTIFIER_TAG                      0x10
#define RESPONSE_LENGTH_TAG                      0x11
#define FILE_LIST_TAG                            0x12
#define LOCATION_INFO_TAG                        0x13
#define IMEI_TAG                                 0x14
#define HELP_REQUEST_TAG                         0x15
#define NETWORK_MEASURE_RESULTS_TAG              0x16
#define DEFAULT_TEXT_TAG                         0x17
#define ITEMS_NEXT_ACTION_INDICATOR_TAG          0x18
#define EVENT_LIST_TAG                           0x19
#define CAUSE_TAG                                0x1A
#define LOCATION_STATUS_TAG                      0x1B
#define TRANSACTION_IDENTIFIER_TAG               0x1C
#define BCCH_CHANNEL_LIST_TAG                    0x1D
#define ICON_IDENTIFIER_TAG                      0x1E
#define ITEM_ICON_IDENTIFIER_LIST_TAG            0x1F
#define CARD_READER_STATUS_A_TAG           0x20
#define CARD_ATR_A_TAG                     0x21
#define C_APDU_A_TAG                       0x22
#define R_APDU_A_TAG                       0x23
#define TIMER_IDENTIFIER_TAG                     0x24
#define TIMER_VALUE_TAG                          0x25
#define DATE_TIME_ZONE_TAG                  0x26
#define CALL_CONTROL_REQURESTED_ACTION_TAG       0x27
#define AT_COMMAND_TAG                           0x28
#define AT_RESPONSE_B_TAG                  0x29
#define BC_REPEAT_INDICATOR_TAG                  0x2A
#define IMMEDIATE_RESPONSE_TAG                   0x2B
#define DTMF_STRING_TAG                          0x2C
#define LANGUAGE_TAG                             0x2D
#define TIMING_ADVANCE_TAG                       0x2E
#define RESERVED_FOR_3GPP31_111_TAG              0x2F
#define BROWSER_IDENTITY_C_TAG                   0x30
#define URL_C_E_TAG                              0x31
#define BEARER_C_TAG                             0x32
#define PROVISIONING_REFER_FILE_C_TAG            0x33
#define BROWSER_TERMINATION_CAUSE_C_TAG          0x34
#define BEARER_DESCRIPTION_E_TAG                0x35
#define CHANNEL_DATA_E_TAG                       0x36
#define CHANNEL_DATA_LENGTH_E_TAG                0x37
#define CHANNEL_STATUS_E_TAG                     0x38
#define BUFFER_SIZE_E_TAG                        0x39
#define CARD_READER_IDENTIFIER_A_TAG             0x3A
//#define NOT_USED_TAG                0x3B
#define SIM_ME_INTERFACE_TRANSPORT_LEVEL_E_TAG  0x3C
#define NOT_USED_TAG                             0x3D
#define OTHER_ADDRESS_E_TAG                      0x3E
#define NETWORK_ACCESS_NAME_TAG                  0x47
#define RESERVED_60FOR_TIA_EIA136_TAG            0x60
#define RESERVED_61FOR_TIE_EIA136_TAG            0x61

#define REFRESH_SIM_INIT_FULL_FILE  0
#define REFRESH_FILE_CHANGE         1
#define REFRESH_SIM_INIT_FILE_CHANGE    2
#define REFRESH_SIM_INIT        3
#define REFRESH_SIM_RESET   4


#define ADDRESS2BYTE          2
#define ADDRESS1BYTE          1
#define MAX_IN_LINE           520    // > 256*2


#ifdef __GCC__

struct _T1LS
{
    UINT8 Tag;
    UINT8 Len;
    UINT8 String[0];
} __attribute__ ((packed, aligned(1)));

struct _T1LBS
{
    UINT8 Tag;
    UINT8 Len;
    UINT8 OneByte;
    UINT8 String[0];
} __attribute__ ((packed, aligned(1)));

struct _T2LS
{
    UINT8 Tag;
    UINT8 Prefix;
    UINT8 Len;
    UINT8 String[0];
} __attribute__ ((packed, aligned(1)));

struct _T2LBS
{
    UINT8 Tag;
    UINT8 Prefix;
    UINT8 Len;
    UINT8 OneByte;
    UINT8 String[0];
} __attribute__ ((packed, aligned(1)));

struct CommandDetail
{
    UINT8 DetailsTag;
    UINT8 Length;
    UINT8 Number;
    UINT8 Type;
    UINT8 Qualifier;
} __attribute__ ((packed, aligned(1)));

struct DeviceIden
{
    UINT8 IdentifiesTag;
    UINT8 Length;
    UINT8 Source;
    UINT8 Destination;
} __attribute__ ((packed, aligned(1)));

struct CellBroadcastPage
{
    UINT8 CellBroadcastPageTag;
    UINT8 Length;
    UINT8 CellBroadcastPage[88];
} __attribute__ ((packed, aligned(1)));

struct Duration
{
    UINT8 DurationTag;
    UINT8 Length;
    UINT8 TimeUnit;
    UINT8 TimeInterval;
} __attribute__ ((packed, aligned(1)));

struct ItemIdentifier
{
    UINT8 ItemIdenTag;
    UINT8 Length;
    UINT8 Identifier;
} __attribute__ ((packed, aligned(1)));

struct ResponseLength
{
    UINT8 ResponseLenTag;
    UINT8 Length;
    UINT8 MiniLen;
    UINT8 MaxLen;
} __attribute__ ((packed, aligned(1)));


struct Tone
{
    UINT8 ToneTag;
    UINT8 Length;
    UINT8 Tone;
} __attribute__ ((packed, aligned(1)));

struct LocationInfo
{
    UINT8 LocationInfoTag;
    UINT8 Length;
    UINT8 MCCandMNC[3];
    UINT8 LACInfo[2];
    UINT8 CellIDInfo[2];
} __attribute__ ((packed, aligned(1)));

struct IMEIInfo
{
    UINT8 IMEITag;
    UINT8 Length;
    UINT8 IMEIInfo[8];
} __attribute__ ((packed, aligned(1)));

struct HelpRequest
{
    UINT8 HelpRequestTag;
    UINT8 Length;
} __attribute__ ((packed, aligned(1)));

struct NetWorkMeasResult
{
    UINT8 NetWorkMeasResultTag;
    UINT8 Length;
    UINT8 NetWorkMeasResult[16];
} __attribute__ ((packed, aligned(1)));

struct ItemsNAI //Items Next Action Indicator
{
    UINT8 ItemsNAITAG;
    UINT8 Length;
    UINT8 ItemsNAIList[0];
} __attribute__ ((packed, aligned(1)));

struct CauseInfo
{
    UINT8 CauseTag;
    UINT8 Length;
    UINT8 CauseInfo[0];
} __attribute__ ((packed, aligned(1)));

struct LocationStatus
{
    UINT8 LocationStatusTag;
    UINT8 Length;
    UINT8 LocationStatus;
} __attribute__ ((packed, aligned(1)));

struct TransactionIden
{
    UINT8 TransactionIdenTag;
    UINT8 Length;
    UINT8 TransactionIdenList[0];
} __attribute__ ((packed, aligned(1)));

struct BCCHChannelList
{
    UINT8 BCCHChannelListTag;
    UINT8 Length;
    UINT8 BCCHChannelList[0];
} __attribute__ ((packed, aligned(1)));

struct IconIden
{
    UINT8 IconIdentifierTag;
    UINT8 Length;
    UINT8 IconQualifier;
    UINT8 IconIdentifier;
} __attribute__ ((packed, aligned(1)));

struct ItemIconIdenList
{
    UINT8 ItemIconIdenListTag;
    UINT8 Length;
    UINT8 IconListQualifier;
    UINT8 IconIdenList[0];
} __attribute__ ((packed, aligned(1)));


struct CardReaderStatus
{
    UINT8 CardReaderStatusTag;
    UINT8 Length;
    UINT8 CardReaderStatus;
} __attribute__ ((packed, aligned(1)));

struct CardATR
{
    UINT8 CardATRTag;
    UINT8 Length;
    UINT8 ATR[0];
} __attribute__ ((packed, aligned(1)));

struct TimerIden
{
    UINT8 TimerIdenTag;
    UINT8 Length;
    UINT8 Identifier;
} __attribute__ ((packed, aligned(1)));

struct TimerValue
{
    UINT8 TimerValueTag;
    UINT8 Length;
    UINT8 TimerValue[3];
} __attribute__ ((packed, aligned(1)));

struct DateTimeZone
{
    UINT8 DateTimeZoneTag;
    UINT8 Length;
    UINT8 DateTimeZone[7];
} __attribute__ ((packed, aligned(1)));

struct BCRepeatInd
{
    UINT8 BCRepeatIndTag;
    UINT8 Length;
    UINT8 BCRepeatIndValue;
} __attribute__ ((packed, aligned(1)));

struct ImmediateRSP
{
    UINT8 ImmediateRSPTag;
    UINT8 Length;
} __attribute__ ((packed, aligned(1)));

struct LanguageInfo
{
    UINT8 LanguageTag;
    UINT8 Length;
    UINT8 LanguageInfo[2];
} __attribute__ ((packed, aligned(1)));

struct TimingAdvanceInfo
{
    UINT8 TimingAdvanceTag;
    UINT8 Length;
    UINT8 MEStatusInfo;
    UINT8 TimingAdvanceInfo;
} __attribute__ ((packed, aligned(1)));

struct BrowserIden
{
    UINT8 BrowserIdenTag;
    UINT8 Length;
    UINT8 BrowserIdentifier;
} __attribute__ ((packed, aligned(1)));

struct BrowserTerminationCause
{
    UINT8 BrowserTerminationCauseTag;
    UINT8 Length;
    UINT8 BrowserTerminationCause;
} __attribute__ ((packed, aligned(1)));

struct BearerDes
{
    UINT8 BearerDesTag;
    UINT8 Length;
    UINT8 Type;
    UINT8 BearerParameters[0];
} __attribute__ ((packed, aligned(1)));

struct ChannelDataLength
{
    UINT8 ChannelDataLengthTag;
    UINT8 Length;
    UINT8 ChannelDataLength;
} __attribute__ ((packed, aligned(1)));

struct BufferSize
{
    UINT8 BufferSizeTag;
    UINT8 Length;
    UINT8 BufferSize[2];
} __attribute__ ((packed, aligned(1)));

struct ChannelStatusInfo
{
    UINT8 ChannelStatusTag;
    UINT8 Length;
    UINT8 ChannelStatusInfo[2];
} __attribute__ ((packed, aligned(1)));

struct CardReaderIden
{
    UINT8 CardReaderIdenTag;
    UINT8 Length;
    UINT8 IdenCardReader[0];
} __attribute__ ((packed, aligned(1)));

struct OtherAddress
{
    UINT8 OtherAddressTag;
    UINT8 Length;
    UINT8 Type;
    UINT8 OtherAddress[0];
} __attribute__ ((packed, aligned(1)));

struct TransportLevel
{
    UINT8 TransportLevelTag;
    UINT8 Length;
    UINT8 Type;
    UINT8 PortNumber[2];
} __attribute__ ((packed, aligned(1)));

struct NetworkAccessName    //Network Access Name
{
    UINT8 NetworkAccessNameTag;
    UINT8 Length;
    UINT8 NetworkAccessName[0];
} __attribute__ ((packed, aligned(1)));

#else

struct CommandDetail
{
    UINT8 DetailsTag;
    UINT8 Length;
    UINT8 Number;
    UINT8 Type;
    UINT8 Qualifier;
};

struct DeviceIden
{
    UINT8 IdentifiesTag;
    UINT8 Length;
    UINT8 Source;
    UINT8 Destination;
};

struct CellBroadcastPage
{
    UINT8 CellBroadcastPageTag;
    UINT8 Length;               // length == 88
    UINT8 *CellBroadcastPage;
};

struct Duration
{
    UINT8 DurationTag;
    UINT8 Length;
    UINT8 TimeUnit;
    UINT8 TimeInterval;
};

struct ItemIdentifier
{
    UINT8 ItemIdenTag;
    UINT8 Length;
    UINT8 Identifier;
};

struct ResponseLength
{
    UINT8 ResponseLenTag;
    UINT8 Length;
    UINT8 MiniLen;
    UINT8 MaxLen;
};

struct Tone
{
    UINT8 ToneTag;
    UINT8 Length;
    UINT8 Tone;
};

struct LocationInfo
{
    UINT8 LocationInfoTag;
    UINT8 Length;
    UINT8 *MCCandMNC;       // length = 3
    UINT8 *LACInfo;         // length = 2
    UINT8 *CellIDInfo;      //length = 2
};

struct IMEIInfo
{
    UINT8 IMEITag;
    UINT8 Length;
    UINT8 *IMEIInfo;            //length = 8
};

struct HelpRequest
{
    UINT8 HelpRequestTag;
    UINT8 Length;
};

struct NetWorkMeasResult
{
    UINT8 NetWorkMeasResultTag;
    UINT8 Length;               //length ==16
    UINT8 *NetWorkMeasResult;
};

struct ItemsNAI //Items Next Action Indicator
{
    UINT8 ItemsNAITAG;
    UINT8 Length;
    UINT8 *ItemsNAIList;
};

struct CauseInfo
{
    UINT8 CauseTag;
    UINT8 Length;
    UINT8 *CauseInfo;
};

struct LocationStatus
{
    UINT8 LocationStatusTag;
    UINT8 Length;
    UINT8 LocationStatus;
};

struct TransactionIden
{
    UINT8 TransactionIdenTag;
    UINT8 Length;
    UINT8 *TransactionIdenList;
};

struct BCCHChannelList
{
    UINT8 BCCHChannelListTag;
    UINT8 Length;
    UINT8 *BCCHChannelList;
};

struct IconIden
{
    UINT8 IconIdentifierTag;
    UINT8 Length;
    UINT8 IconQualifier;
    UINT8 IconIdentifier;
};

struct ItemIconIdenList
{
    UINT8 ItemIconIdenListTag;
    UINT8 Length;
    UINT8 IconListQualifier;
    UINT8 *IconIdenList;
};

struct CardReaderStatus
{
    UINT8 CardReaderStatusTag;
    UINT8 Length;
    UINT8 CardReaderStatus;
};

struct CardATR
{
    UINT8 CardATRTag;
    UINT8 Length;
    UINT8 *ATR;
};

struct TimerIden
{
    UINT8 TimerIdenTag;
    UINT8 Length;           //length == 1
    UINT8 Identifier;
};

struct TimerValue
{
    UINT8 TimerValueTag;
    UINT8 Length;           //length == 3
    UINT8 *TimerValue;
};

struct DateTimeZone
{
    UINT8 DateTimeZoneTag;
    UINT8 Length;           //length == 7
    UINT8 *DateTimeZone;
};

struct BCRepeatInd
{
    UINT8 BCRepeatIndTag;
    UINT8 Length;
    UINT8 BCRepeatIndValue;
};

struct ImmediateRSP
{
    UINT8 ImmediateRSPTag;
    UINT8 Length;
};

struct LanguageInfo
{
    UINT8 LanguageTag;
    UINT8 Length;       //length == 2
    UINT8 *LanguageInfo;
};

struct TimingAdvanceInfo
{
    UINT8 TimingAdvanceTag;
    UINT8 Length;
    UINT8 MEStatusInfo;
    UINT8 TimingAdvanceInfo;
};

struct BrowserIden
{
    UINT8 BrowserIdenTag;
    UINT8 Length;
    UINT8 BrowserIdentifier;
};

struct BrowserTerminationCause
{
    UINT8 BrowserTerminationCauseTag;
    UINT8 Length;
    UINT8 BrowserTerminationCause;
};

struct BearerDes
{
    UINT8 BearerDesTag;
    UINT8 Length;
    UINT8 Type;
    UINT8 *BearerParameters;
};

struct ChannelDataLength
{
    UINT8 ChannelDataLengthTag;
    UINT8 Length;
    UINT8 ChannelDataLength;
};

struct BufferSize
{
    UINT8 BufferSizeTag;
    UINT8 Length;           //length == 2
    UINT8 *BufferSize;
};

struct ChannelStatusInfo
{
    UINT8 ChannelStatusTag;
    UINT8 Length;           //length == 2
    UINT8 *ChannelStatusInfo;
};

struct CardReaderIden
{
    UINT8 CardReaderIdenTag;
    UINT8 Length;
    UINT8 *IdenCardReader;
};

struct OtherAddress
{
    UINT8 OtherAddressTag;
    UINT8 Length;
    UINT8 Type;
    UINT8 *OtherAddress;
};

struct TransportLevel
{
    UINT8 TransportLevelTag;
    UINT8 Length;       // length == 2
    UINT8 Type;
    UINT8 *PortNumber;
};

struct NetworkAccessName    //Network Access Name
{
    UINT8 NetworkAccessNameTag;
    UINT8 Length;
    UINT8 *NetworkAccessName;
};

#endif

struct Address
{
    UINT8 AddressTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 TONandNPI;
    UINT8 *DiallingNumber;
};

struct AlphaIden
{
    UINT8 AlphaTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *AlphaIdentifier;
};

struct SubAddress
{
    UINT8 SubAddressTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *SubAddress;
};

struct CapabilityCP
{
    UINT8 CapabilityCPTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *CapabilityCP;
};

struct Item
{
    UINT8 ItemTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 Identifier;
    UINT8 *TextString;
};

struct Result
{
    UINT8 ResultTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 GeneralResult;
    UINT8 *AdditionalInfo;
};

struct SMSTPDU
{
    UINT8 SMSTPDUTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *SMSTPDU;
};

struct SSString
{
    UINT8 SSStringTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 TONandNPI;
    UINT8 *SSorUSSDString;
};

struct TextString
{
    UINT8 TextStringTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 DataCodingScheme;
    UINT8 *TextString;
};

struct USSDString
{
    UINT8 USSDStringTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 DataCodingScheme;
    UINT8 *USSDString;
};

struct FileList
{
    UINT8 FileListTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 Numberoffiles;
    UINT8 *Files;
};

struct DefaultText
{
    UINT8 DefaultTextTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 DataCodingScheme;
    UINT8 *TextString;
};

struct EventList
{
    UINT8 EventListTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *EventList;
};

struct CallControlReqAct
{
    UINT8 CallControlReqActTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *CallControlReqAct;
};

struct CAPDU
{
    UINT8 CAPDUTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 CLA;
    UINT8 INS;
    UINT8 P1;
    UINT8 P2;
    UINT8 Lc;
    UINT8 *data;
    UINT8 Le;
};

struct RAPDU
{
    UINT8 RAPDUTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *RAPDUdata;
    UINT8 SW1;
    UINT8 SW2;
};

struct ATCommand
{
    UINT8 ATCommandTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *ATCommandString;
};

struct DTMFString
{
    UINT8 DTMFStringTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *DTMFString;
};

struct URL
{
    UINT8 URLTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *URL;
};

struct Bearer
{
    UINT8 BearerTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *ListBearers;
};

struct ProvisoinFileRef //Provisioning File Reference
{
    UINT8 ProvisionFileRefTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *Path;
};

struct ChannelData
{
    UINT8 ChannelDataTag;
    UINT8 AddType;
    UINT8 Length;
    UINT8 *ChannelDataString;
};

#define RESPONSEBASE            0x0C
#define RESPONSEITEM            0x03
#define RESPONSETEXT            0x03
#define RESPONSEDURA            0x04
#define RESPONSELOCAL           0x09
#define RESPONSEUSSD            0x03

#define RESPONSECHANNEL         0x04
#define RESPONSEIMEI            0x0A
#define RESPONSEMEASURE         0x12
#define RESPONSEDATETIME        0X09
#define RESPONSELANGUAGE        0x04
#define RESPONSETADVANCE        0x04

#define RESPONSECARDID          0x03
#define RESPONSECARDSTATUS      0x03


#define ComType                 0x03
#define ComQual                 0x04
#define ComResult               0x0B

#define TextLength              0x01
#define TextScheme              0x02

#define ItemSelection           0x02

#define ItemNumber              0x08        //if Length occupy two byte,this value should add one.

#define MCC_MNC                 0x02
#define LAC                     0x05
#define CellID                  0x07

#define ChannelStatus           0x02
#define IMEI                    0x02
#define Measurement             0x02
#define DateTime                0x02
#define Language                0x02
#define MEStatus                0x02
#define TimingAdvance           0x03

#define CardID_Length           0x01
#define CardID                  0x02

#define CardStatus              0x02

//typedef struct SAT_CardStatus
//{
//    UINT8 Status;   //bits 1-3    = identity of card reader x.
//                    //bit 4        0 = Card reader is not removable
//                    //            1 = Card reader is removable
//                    //bit 5        0 = Card reader is not present
//                    //            1 = Card reader is present
//                    //bit 6        0 = Card reader present is not ID-1 size
//                    //            1 = Card reader present is ID-1 size
//                    //bit 7        0 = No card present
//                    //            1 = Card is present in reader
//                    //bit 8        0 = No card powered
//                    //            1 = Card in reader is powered
//}SAT_CardStatus_t;

//Proactive command definition
#define SAT_PRO_COM_SCALL           0x10
#define SAT_PRO_COM_SSMS            0X13
#define SAT_PRO_COM_DTEXT           0x21
#define SAT_PRO_COM_GINPUT          0x23
#define SAT_PRO_COM_SITEM           0x24
#define SAT_PRO_COM_SMENU           0x25
#define SAT_PRO_COM_PLINFO          0x26
#define SAT_PRO_COM_GRSTATUS        0x33
#define SAT_PRO_COM_GCSTATUS        0x44

//Status of a proactive command
#define SAT_STATUS_SUCCESS_NOMAL    0x00
#define SAT_STATUS_SUCCESS_PARTIAL  0x01
#define SAT_STATUS_SUCCESS_MISSINFO 0x02
#define SAT_STATUS_SUCCESS_LIMITSER 0x06
#define SAT_STATUS_TERMINAL_USER    0x10
#define SAT_STATUS_BACKWARDMOVE     0x11
#define SAT_STATUS_NORESPONSE       0x12
#define SAT_STATUS_HELP_INFO        0x13

#define SAT_STATUS_ME_NOPROCESS     0x20
#define SAT_STATUS_NETWORK_NOPRO    0x21
#define SAT_STATUS_USER_NOT_ACCEPT  0x22

#define SAT_STATUS_COM_BEYONG_ME    0x30
#define SAT_STATUS_COMTYPE_UNKOWN   0x31
#define SAT_STATUS_COMDATA_UNKOWN   0x32
#define SAT_STATUS_COMNUM_UNKOWN    0x33

#define SM_ACT_INIT                 0xFF
#define SM_ACT_IDLE                 0x01
#define SM_ACT_TERMINALPROFILE      0x02
#define SM_ACT_FETCH                0x03
#define SM_ACT_TERMINALRESPONSE     0x04

#define SM_RESP_INIT                0xFF
#define SM_RESP_IDLE                0x01
#define SM_RESP_TERMINALRESPONSE    0x02
#define SM_RESP_FETCH               0x03
#define SM_RESP_TERMINALENVELOPE    0x04

extern CFW_PROFILE nMEProfile;

extern UINT8 nCommondNum[68][CFW_SIM_COUNT];
extern BOOL gSatFreshComm[CFW_SIM_COUNT];

extern const UINT8 TerminalResponse[];
extern const UINT8 TerminalResponse1[];
extern const UINT8 ResponseDuration[];
extern const UINT8 ResponseText[];
extern const UINT8 ResponseTextEX[];
extern const UINT8 ResponseUSSD[];
extern const UINT8 ResponseUSSDEX[];
extern const UINT8 ResponseItem[];
extern const UINT8 MenuSelection[];
extern const UINT8 EventDownload[];
extern UINT8 CallControlDownload[];
extern const UINT8 SMPPDownSelection[];
extern const UINT8 SMPPDownSelectionEx[];
extern const UINT8 TimerExpirationDownload[];
extern const UINT8 ResponseLocalInfo[];

extern const UINT8 ResponseIMEI[];

extern UINT32 gSATTimerValue[8][CFW_SIM_COUNT];

UINT32 sendSetupMenuRsp(CFW_SAT_RESPONSE *pSimSatResponseData,
                        SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendResponsePDU(CFW_SAT_RESPONSE *pSimSatResponseData, UINT8 nSimID);
UINT32 sendSelectItemRsp(CFW_SAT_RESPONSE *pSimSatResponseData,
                         SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);

UINT32 sendGetInKeyRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                       SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendSMSPPDownload(CFW_SAT_RESPONSE *pSimSatResponseData,
                         SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendGetInputRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                       SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendUSSDRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                   SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendDisplayTextRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                          SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendPlayToneRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                       SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
//UINT32 sendCommonRsp(CFW_SAT_RESPONSE *pSimSatResponse,
//                     SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendCommonRsp(CFW_SAT_RESPONSE *pSimSatResponse, UINT8 nSimID);
UINT32 sendGetChannelStatusRsp(CFW_SAT_RESPONSE *pSimSatResponse,
                               SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);
UINT32 sendEventDownload(CFW_SAT_RESPONSE *pSimSatResponseData, UINT8 nSimID);

UINT32 sendTimerExpiration(CFW_SAT_RESPONSE *pSimSatResponseData, UINT8 nSimID);

UINT32 sendPollIntervalRsp(CFW_SAT_RESPONSE *pSimSatResponseData,
                           SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);

UINT32 sendCallControl(CFW_SAT_RESPONSE *pSimSatResponseData, UINT8 nSimID);

UINT32 sendMenuSelection(CFW_SAT_RESPONSE *pSimSatResponseData, UINT8 nSimID);

UINT32 sendEnvelopePDU(CFW_SAT_RESPONSE *pSimSatResponseData, UINT8 nSimID);

UINT32 sendProvideLocalInfoRsp(CFW_SAT_RESPONSE *pSimSatResponseData,
                               SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);

UINT32 sendGetReadStatusRsp(CFW_SAT_RESPONSE *pSimSatResponseData,
                            SIM_SAT_PARAM *pG_SimSat, UINT8 nSimID);

UINT32 processFetchInd(CFW_EV *pEvent, UINT32 nUTI, CFW_SIM_ID nSimID);
#endif


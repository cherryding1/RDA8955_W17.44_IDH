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


#ifndef CFW_SIM_H
#define CFW_SIM_H

extern UINT8 nECC_Code[5][3];
//extern BOOL g_SAT_INIT_COMPLETE;

extern bool sxs_UsimSupported(VOID);

#define ERR_CFW_UTI_EXIST                       ERR_CFW_UTI_IS_BUSY //The UTI is exist.

UINT32 CFW_SimSetMeProfile(UINT8* pProfile, UINT8 Length);
UINT32 CFW_SimTest(UINT8 Index);

VOID cfw_PLMNtoBCD(UINT8 *pIn, UINT8 *pOut, UINT8 *nInLength);
VOID cfw_BCDtoPLMN(UINT8 *pIn, UINT8 *pOut, UINT8 nInLength);
VOID cfw_IMSItoASC(UINT8 *InPut, UINT8 *OutPut, UINT8 *OutLen);


#define  CHV10                 0x00     //used to get usim pin1 retry times
#define  CHV1                   0x01
#define  CHV2                   0x02
#define  CHV20                 0x03     //used to get usim pin2 retry times

//#define SIM_RESET_REQ                   0x00
#define SIM_READBINARY              0x02
#define SIM_UPDATEBINARY            0x03

#define SIM_READRECORD              0x04
#define SIM_UPDATERECORD            0x05

//#define SIM_ELEMFILESTATUS          0x06
//#define SIM_DEDICFILESTATUS         0x07
#define SIM_SEEK                    0x08

#define SIM_VERIFYCHV               0x09
#define SIM_CHANGECHV               0x0A
#define SIM_DISABLECHV              0x0B
#define SIM_ENABLECHV               0x0C
#define SIM_UNBLOCKCHV              0x0D

#define SIM_INVALIDATE              0x0E
#define SIM_REHABILITATE            0x0F

//Add for SIM sat command set by lixp at 20100806

#define SIM_SAT_FRESH_IND               0x01
#define SIM_SAT_REFRESH_IND             0x01
#define SIM_SAT_MORE_TIME_IND           0x02
#define SIM_SAT_POLL_INTERVAL_IND       0x03
#define SIM_SAT_POLLING_OFF_IND         0x04
#define SIM_SAT_SETUP_EVENT_LIST_IND    0x05
#define SIM_SAT_CALL_SETUP_IND          0x10
#define SIM_SAT_SEND_SS_IND             0x11
#define SIM_SAT_SEND_USSD_IND           0x12
#define SIM_SAT_SEND_SMS_IND            0x13
#define SIM_SAT_SEND_DTMF_IND           0x14
#define SIM_SAT_LAUNCH_BROWSER_IND      0x15
#define SIM_SAT_PLAY_TONE_IND           0x20
#define SIM_SAT_DISPLAY_TEXT_IND        0x21
#define SIM_SAT_GET_INKEY_IND           0x22
#define SIM_SAT_GET_INPUT_IND           0x23
#define SIM_SAT_SELECT_ITEM_IND         0x24
#define SIM_SAT_SETUP_MENU_IND          0x25
#define SIM_SAT_PROVIDE_LOCAL_INFO_IND  0x26
#define SIM_SAT_TIMER_MANAGEMENT_IND    0x27
#define SIM_SAT_SETUP_IDLE_DISPLAY_IND  0x28
#define SIM_SAT_CARD_APDU_IND           0x30
#define SIM_SAT_POWER_ON_CARD_IND       0x31
#define SIM_SAT_POWER_OFF_CARD_IND      0x32
#define SIM_SAT_GET_READER_STATUS_IND   0x33
#define SIM_SAT_RUN_AT_COMMAND_IND      0x34
#define SIM_SAT_LANG_NOTIFY_IND         0x35
#define SIM_SAT_OPEN_CHANNEL_IND        0x40
#define SIM_SAT_CLOSE_CHANNEL_IND       0x41
#define SIM_SAT_RECEIVE_DATA_IND        0x42
#define SIM_SAT_SEND_DATA_IND           0x43
#define SIM_SAT_GET_CHANNEL_STATUS_IND  0x44

#define SIM_SAT_PROACTIVE_SIM_COMMAND   0xD0
#define SIM_SAT_SMSPP_DOWNLOAD          0xD1
#define SIM_SAT_CB_DOWNLOAD             0xD2
#define SIM_SAT_MENU_SELECTION          0xD3
#define SIM_SAT_CALL_CONTROL            0xD4
#define SIM_SAT_MO_SMS_CONTROL          0xD5
#define SIM_SAT_EVENT_DOWNLOAD          0xD6
#define SIM_SAT_TIMER_EXPIRATION        0xD7



/** Event : Profile Download  = 1                                       */
#define EVENT_PROFILE_DOWNLOAD     1
/** Event : Envelope SMS-PP Data Download (03.48 formatted)  = 2        */
#define EVENT_FORMATTED_SMS_PP_ENV     2
/** Event : Update Record EF sms APDU     (03.48 formatted)  = 3        */
#define EVENT_FORMATTED_SMS_PP_UPD     3
/** Event : Envelope SMS-PP Data Download unformatted sms  = 4          */
#define EVENT_UNFORMATTED_SMS_PP_ENV     4
/** Event : Update Record EFsms APDU      unformatted sms   = 5         */
#define EVENT_UNFORMATTED_SMS_PP_UPD     5
/** Event : Cell Broadcast Data Download  = 6                           */
#define EVENT_UNFORMATTED_SMS_CB     6
/** Event : Menu Selection  = 7                                         */
#define EVENT_MENU_SELECTION     7
/** Event : Menu Selection Help Request = 8                             */
#define EVENT_MENU_SELECTION_HELP_REQUEST     8
/** Event : Call Control by SIM = 9                                     */
#define EVENT_CALL_CONTROL_BY_SIM     9
/** Event : MO Short Message Control by SIM = 10                        */
#define EVENT_MO_SHORT_MESSAGE_CONTROL_BY_SIM     10
/** Event : Timer Expiration = 11                                       */
#define EVENT_TIMER_EXPIRATION     11
/** Event : Event Download - MT call type = 12                          */
#define EVENT_EVENT_DOWNLOAD_MT_CALL     12
/** Event : Event Download - Call connected type = 13                   */
#define EVENT_EVENT_DOWNLOAD_CALL_CONNECTED     13
/** Event : Event Download - Call disconnected type = 14                */
#define EVENT_EVENT_DOWNLOAD_CALL_DISCONNECTED     14
/** Event : Event Download - Location status type = 15                  */
#define EVENT_EVENT_DOWNLOAD_LOCATION_STATUS     15
/** Event : Event Download - User activity type = 16                    */
#define EVENT_EVENT_DOWNLOAD_USER_ACTIVITY     16
/** Event : Event Download - Idle screen available type = 17            */
#define EVENT_EVENT_DOWNLOAD_IDLE_SCREEN_AVAILABLE     17
/** Event : Event Download - Card Reader Status = 18                    */
#define EVENT_EVENT_DOWNLOAD_CARD_READER_STATUS =  (UINT8)18
/** Event : Status APDU command = 19                                    */
#define EVENT_STATUS_COMMAND     19
/** Event : Event Download - Language Selection = 20                    */
#define EVENT_EVENT_DOWNLOAD_LANGUAGE_SELECTION =   (UINT8)20
/** Event : Event Download - Browser Termination = 21                   */
#define EVENT_EVENT_DOWNLOAD_BROWSER_TERMINATION =  (UINT8)21
/** Event : Cell Broadcast Data Download Formatted = 24                 */
#define EVENT_FORMATTED_SMS_CB     24
/** Event : Proprietary events reserved range = [-128, -1]              */
/** Event : Unrecognized Envelope = -1                                  */
#define EVENT_UNRECOGNIZED_ENVELOPE     -1
// -------------------------- BER-TLV Constants -------------------------
/** BER-TLV : Proactive SIM command tag                          = 0xD0 */
#define BTAG_PROACTIVE_SIM_COMMAND     0xD0
/**     * @deprecated Replaced by {@link #BTAG_SMS_PP_DOWNLOAD} in     * version 7.4.0      */
#define BTAG_SMS_PP_DOWNWLOAD     0xD1
/** BER-TLV : SMS-PP download tag                                = 0xD1 */
#define BTAG_SMS_PP_DOWNLOAD     0xD1
/** BER-TLV : Cell Broadcast download tag                        = 0xD2 */
#define BTAG_CELL_BROADCAST_DOWNLOAD     0xD2
/** BER-TLV : Menu Selection tag                                 = 0xD3 */
#define BTAG_MENU_SELECTION     0xD3
/** BER-TLV : Call Control tag                                   = 0xD4 */
#define BTAG_CALL_CONTROL     0xD4
/** BER-TLV : MO short message control tag                       = 0xD5 */
#define BTAG_MO_SHORT_MESSAGE_CONTROL     0xD5
/** BER-TLV : Event download tag                                 = 0xD6 */
#define BTAG_EVENT_DOWNLOAD     0xD6
/** BER-TLV : Timer Expiration tag                               = 0xD7 */
#define BTAG_TIMER_EXPIRATION     0xD7
// ------------------------ Simple-TLV Constants -------------------------
/** Simple-TLV : Command Details tag                             = 0x01 */
#define TAG_COMMAND_DETAILS     0x01
/** Simple-TLV : Device Identities tag                           = 0x02 */
#define TAG_DEVICE_IDENTITIES     0x02
/** Simple-TLV : Result tag                                      = 0x03 */
#define TAG_RESULT     0x03
/** Simple-TLV : Duration tag                                    = 0x04 */
#define TAG_DURATION     0x04
/** Simple-TLV : Alpha Identifier tag                            = 0x05 */
#define TAG_ALPHA_IDENTIFIER     0x05
/** Simple-TLV : Address tag                                     = 0x06 */
#define TAG_ADDRESS     0x06
/** Simple-TLV : Capability Configuration Parameters tag         = 0x07 */
#define TAG_CAPABILITY_CONFIGURATION_PARAMETERS     0x07
/** Simple-TLV : Called Party Subaddress tag                     = 0x08 */
#define TAG_CALLED_PARTY_SUBADDRESS     0x08
/** Simple-TLV : SS String tag                                   = 0x09 */
#define TAG_SS_STRING     0x09
/** Simple-TLV : USSD String tag                                 = 0x0A */
#define TAG_USSD_STRING     0x0A
/** Simple-TLV : SMS TPDU tag                                    = 0x0B */
#define TAG_SMS_TPDU     0x0B
/** Simple-TLV : Cell Broadcast Page tag                         = 0x0C */
#define TAG_CELL_BROADCAST_PAGE     0x0C
/** Simple-TLV : Text String tag                                 = 0x0D */
#define TAG_TEXT_STRING     0x0D
/** Simple-TLV : Tone tag                                        = 0x0E */
#define TAG_TONE     0x0E
/** Simple-TLV : Item tag                                        = 0x0F */
#define TAG_ITEM     0x0F
/** Simple-TLV : Item Identifier tag                             = 0x10 */
#define TAG_ITEM_IDENTIFIER     0x10
/** Simple-TLV : Response Length tag                             = 0x11 */
#define TAG_RESPONSE_LENGTH     0x11
/** Simple-TLV : File List tag                                   = 0x12 */
#define TAG_FILE_LIST     0x12
/** Simple-TLV : Location Information tag                        = 0x13 */
#define TAG_LOCATION_INFORMATION     0x13
/** Simple-TLV : IMEI tag                                        = 0x14 */
#define TAG_IMEI     0x14
/** Simple-TLV : Help Request tag                                = 0x15 */
#define TAG_HELP_REQUEST     0x15
/** Simple-TLV : Network Measurement Results tag                 = 0x16 */
#define TAG_NETWORK_MEASUREMENT_RESULTS     0x16
/** Simple-TLV : Default Text tag                                = 0x17 */

#define TAG_DEFAULT_TEXT     0x17
/** Simple-TLV : Items Next Action Indicator tag                 = 0x18 */
#define TAG_ITEMS_NEXT_ACTION_INDICATOR     0x18
/** Simple-TLV : Event List tag                                  = 0x19 */
#define TAG_EVENT_LIST     0x19
/** Simple-TLV : Cause tag                                       = 0x1A */
#define TAG_CAUSE     0x1A
/** Simple-TLV : Location Status tag                             = 0x1B */
#define TAG_LOCATION_STATUS     0x1B
/** Simple-TLV : Transaction Identifier tag                      = 0x1C */
#define TAG_TRANSACTION_IDENTIFIER     0x1C
/** Simple-TLV : BCCH Channel List tag                           = 0x1D */
#define TAG_BCCH_CHANNEL_LIST     0x1D
/** Simple-TLV : Icon Identifier tag                             = 0x1E */
#define TAG_ICON_IDENTIFIER     0x1E
/** Simple-TLV : Item Icon Identifier list tag                   = 0x1F */
#define TAG_ITEM_ICON_IDENTIFIER_LIST     0x1F
/** Simple-TLV : Card Reader status tag                          = 0x20 */
#define TAG_CARD_READER_STATUS     0x20
/** Simple-TLV : Card ATR tag                                    = 0x21 */
#define TAG_CARD_ATR     0x21
/** Simple-TLV : C-APDU tag                                      = 0x22 */
#define TAG_C_APDU     0x22
/** Simple-TLV : R-APDU tag                                      = 0x23 */
#define TAG_R_APDU     0x23
/** Simple-TLV : Timer Identifier tag                            = 0x24 */
#define TAG_TIMER_IDENTIFIER     0x24
/** Simple-TLV : Timer Value tag                                 = 0x25 */
#define TAG_TIMER_VALUE     0x25
/** Simple-TLV : Date-Time and Time Zone tag                     = 0x26 */
#define TAG_DATE_TIME_AND_TIME_ZONE     0x26
/** Simple-TLV : Call Control requested action tag               = 0x27 */
#define TAG_CALL_CONTROL_REQUESTED_ACTION     0x27
/** Simple-TLV : AT Command tag                                  = 0x28 */
#define TAG_AT_COMMAND     0x28
/** Simple-TLV : AT Response tag                                 = 0x29 */
#define TAG_AT_RESPONSE     0x29
/** Simple-TLV : BC Repeat Indicator tag                         = 0x2A */
#define TAG_BC_REPEAT_INDICATOR     0x2A
/** Simple-TLV : Immediate response tag                          = 0x2B */
#define TAG_IMMEDIATE_RESPONSE     0x2B
/** Simple-TLV : DTMF string tag                                 = 0x2C */
#define TAG_DTMF_STRING     0x2C
/** Simple-TLV : Language tag                                    = 0x2D */
#define TAG_LANGUAGE     0x2D
/** Simple-TLV : Timing Advance tag                              = 0x2E */
#define TAG_TIMING_ADVANCE     0x2E
/** Simple-TLV : Browser Identity tag                            = 0x30 */
#define TAG_BROWSER_IDENTITY     0x30
/** Simple-TLV : URL tag                                         = 0x31 */
#define TAG_URL     0x31
/** Simple-TLV : Bearer tag                                      = 0x32 */
#define TAG_BEARER     0x32
/** Simple-TLV : Provisioning Reference File tag                 = 0x33 */
#define TAG_PROVISIONING_REFERENCE_FILE     0x33
/** Simple-TLV : Browser Termination Cause tag                   = 0x34 */
#define TAG_BROWSER_TERMINATION_CAUSE     0x34
/** Simple-TLV : Card reader identifier tag                      = 0x3A */
#define TAG_CARD_READER_IDENTIFIER     0x3A
/** Simple-TLV : mask to set the CR flag of any Simple-TLV tag   = 0x80 */
#define TAG_SET_CR     0x80
/** Simple-TLV : mask to reset the CR flag of any Simple-TLV tag = 0x7F */
#define TAG_SET_NO_CR     0x7F
// ---------------------- Constants for TLV management --------------------
/**      * Value of a TLV Length field's first UINT8 in TLV with a Value     * field is greater than 127 bytes = 0x81      */
#define TLV_LENGTH_CODED_2BYTES     0x81
// ---------------------- Constants for findTLV method --------------------
/** findTLV method result : if TLV is not found in the handler   = 0x00 */
#define TLV_NOT_FOUND     0
/** findTLV method result : if TLV is found with CR set          = 0x01 */
#define TLV_FOUND_CR_SET     1
/** findTLV method result : if TLV is found with CR not set      = 0x02 */
#define TLV_FOUND_CR_NOT_SET     2
// ------------------ Type of proactive command constants -----------------
/** Type of proactive command : REFRESH                          = 0x01 */
#define PRO_CMD_REFRESH     0x01
/** Type of proactive command : MORE TIME                        = 0x02 */
#define PRO_CMD_MORE_TIME     0x02
/** Type of proactive command : SET UP CALL                      = 0x10 */
#define PRO_CMD_SET_UP_CALL     0x10
/** Type of proactive command : SEND SS                          = 0x11 */
#define PRO_CMD_SEND_SS     0x11
/** Type of proactive command : SEND USSD                        = 0x12 */
#define PRO_CMD_SEND_USSD     0x12
/** Type of proactive command : SEND SHORT MESSAGE               = 0x13 */
#define PRO_CMD_SEND_SHORT_MESSAGE     0x13
/** Type of proactive command : SEND DTMF                        = 0x14 */
#define PRO_CMD_SEND_DTMF     0x14
/** Type of proactive command : LAUNCH BROWSER                   = 0x15 */
#define PRO_CMD_LAUNCH_BROWSER     0x15
/** Type of proactive command : PLAY TONE                        = 0x20 */
#define PRO_CMD_PLAY_TONE     0x20
/** Type of proactive command : DISPLAY TEXT                     = 0x21 */
#define PRO_CMD_DISPLAY_TEXT     0x21
/** Type of proactive command : GET INKEY                        = 0x22 */
#define PRO_CMD_GET_INKEY     0x22
/** Type of proactive command : GET INPUT                        = 0x23 */
#define PRO_CMD_GET_INPUT     0x23
/** Type of proactive command : SELECT ITEM                      = 0x24 */
#define PRO_CMD_SELECT_ITEM     0x24
/** Type of proactive command : PROVIDE LOCAL INFORMATION        = 0x26 */
#define PRO_CMD_PROVIDE_LOCAL_INFORMATION     0x26
/** Type of proactive command : TIMER MANAGEMENT                 = 0x27 */
#define PRO_CMD_TIMER_MANAGEMENT     0x27
/** Type of proactive command : SET UP IDLE MODE TEXT            = 0x28 */
#define PRO_CMD_SET_UP_IDLE_MODE_TEXT     0x28
/** Type of proactive command : PERFORM CARD APDU                = 0x30 */
#define PRO_CMD_PERFORM_CARD_APDU     0x30
/** Type of proactive command : POWER ON CARD                    = 0x31 */
#define PRO_CMD_POWER_ON_CARD     0x31
/** Type of proactive command : POWER OFF CARD                   = 0x32 */
#define PRO_CMD_POWER_OFF_CARD     0x32
/** Type of proactive command : GET READER STATUS                = 0x33 */
#define PRO_CMD_GET_READER_STATUS     0x33
/** Type of proactive command : RUN AT COMMAND                   = 0x34 */
#define PRO_CMD_RUN_AT_COMMAND     0x34
/** Type of proactive command : LANGUAGE NOTIFICATION            = 0x35 */
#define PRO_CMD_LANGUAGE_NOTIFICATION     0x35


UINT32 SimParseDedicatedStatus(UINT8 *pDedicatedData, CFW_SIM_ID nSimID);
UINT32 Sim_ParseSW1SW2(UINT8 SW1, UINT8 SW2, CFW_SIM_ID nSimID);
UINT32 SimResetReq(CFW_SIM_ID nSimID);
UINT32 SimReadBinaryReq(UINT8 nFile, UINT16 nOffset, UINT8 nLen, CFW_SIM_ID nSimID);
UINT32 SimUpdateBinaryReq(UINT8 nFile, UINT16 nOffset, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID);
UINT32 SimElemFileStatusReq(UINT8 nFile, CFW_SIM_ID nSimID);
UINT32 SimDedicFileStatusReq(UINT8 nFile, CFW_SIM_ID nSimID);
UINT32 SimReadRecordReq(UINT8 nFile, UINT8 nRecordNb, UINT8 nMode, UINT8 nLen, CFW_SIM_ID nSimID);
UINT32 SimUpdateRecordReq(UINT8 nFile, UINT8 nRecordNb, UINT8 nMode, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID);
UINT32 SimSeekReq(UINT8 nFile, UINT8 nTypeMode, UINT8 nLen, UINT8 *pData, CFW_SIM_ID nSimID);
UINT32 SimVerifyCHVReq(UINT8 *pCHVValue, UINT8 nCHVNumber, CFW_SIM_ID nSimID);
UINT32 SimChangeCHVReq(UINT8 *pOldCHVValue, UINT8 *pNewCHVValue, UINT8 nCHVNumber, CFW_SIM_ID nSimID);
UINT32 SimDisableCHVReq(UINT8 *pCHVValue, CFW_SIM_ID nSimID);
UINT32 SimEnableCHVReq(UINT8 *pCHVValue, CFW_SIM_ID nSimID);
UINT32 SimUnblockCHVReq(UINT8 *pUnblockCHVValue, UINT8 *pNewCHVValue, UINT8 nCHVNumber, CFW_SIM_ID nSimID);
UINT32 SimInvalidateReq(UINT8 nFile, CFW_SIM_ID nSimID);
UINT32 SimRehabilitateReq(UINT8 nFile, CFW_SIM_ID nSimID);
//UINT32 CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID);

UINT32 SimSelectApplicationReq(UINT8 *AID, UINT8 AIDLength, CFW_SIM_ID nSimID);
UINT32 SimSetUsimPbk(UINT8 *Data, UINT8 DataLength, UINT8 RecordNum, UINT8 flag, CFW_SIM_ID nSimID);


UINT32 CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID);
VOID CFW_ValidatePDU(UINT8 *pTPDU);
UINT32 cfw_SatProCmdProccess(HAO hAo, UINT8 nCmdType, UINT8 *pData, UINT8 nLen);
UINT32 cfw_SatProCmdSetupMenu(HAO hAo, UINT8 *pData, UINT8 nLength);
UINT32 cfw_SatProCmdLocal(HAO hAo, UINT8 *pData);
#endif //CFW_SIM_H



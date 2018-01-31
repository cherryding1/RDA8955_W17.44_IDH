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

#include "at_cfg.h"

// /////////////////////////////////////////////////////////////////////////////////////
// At default setttings (Manufacture specific)
// /////////////////////////////////////////////////////////////////////////////////////
#define AT_ASCII_CR 13
#define AT_ASCII_LF 10
#define AT_ASCII_BS 8
#define RECEIVE_GAIN 6
#define TRANSMIT_GAIN 1
// 10
#define IND_CTRL_MASK 0
#define TE_CHEST cs_gbk
#define MT_CHSET cs_gbk
#define DISABLE_CMEE_ERROR 1
#define DURATION_OF_TONE 1
#define OPER_FORMAT_NUMERIC 2
#define AUTOMATIC_REGISTER 0
#define MONP_DISPLAY_PERIOD 10
#define MONI_DISPLAY_PERIOD 10
#define DISABLE_UPLINK_VOICE 0
#define OPERATOR_NUMBER_ORDER 100
// Random value
#define NO_INDICATOR_EVENT_REPORT 0
#define DISABLE_AUTOMATIC_ANSWERING 0
#define PHONEBOOK_MEMORY_STORAGE CFW_PBK_SIM
#define CLIR_PRESENTION_INDICATOR 0
#define DISABLE_CSSU_RESULT_CODE 0
#define DISABLE_CSSI_RESULT_CODE 0
#define DISABLE_RESULT_CODE_PRESENTION_STATUS 0

const at_setting_t gDefaultSettings = {
    // default setttings
    // gc related
    .resultCodePresent = GC_RESULTCODE_PRESENTATION_ENABLE,
    .resultCodeFormat = GC_RESULTCODE_FORMAT_VERBOSE,
    .commandEchoMode = GC_COMMANDECHO_MODE_ENABLE,
    .nS3 = AT_ASCII_CR,
    .nS4 = AT_ASCII_LF,
    .nS5 = AT_ASCII_BS,
    .cmee = DISABLE_CMEE_ERROR,
    .cgreg = 0, // disable +CGREG URC
    .rxfc = 0,
    .txfc = 0,
    .icfFormat = 3, // 8N1
    .icfParity = 3, // PARITY SPACE
    .dtrData = AT_DTR_DATA_ESCAPE,
    .secUartEnable = 0,
    .secUartBaud = 115200,
    .secUartIcfFormat = 3, // 8N1
    .secUartIcfParity = 3, // PARITY SPACE
    .crc = 0,
    .cmerInd = {[0 ...(NUMBER_OF_SIM - 1)] = NO_INDICATOR_EVENT_REPORT},
    .cfgInfo = {
        .ind_ctrl_mask = IND_CTRL_MASK,
        .sind_ctrl_mask = 0,
        .te_chset = TE_CHEST,
        .mt_chset = MT_CHSET,
    },

    // cc related
    .cc_VTD = DURATION_OF_TONE,
    .cc_s0 = DISABLE_AUTOMATIC_ANSWERING,

    // nw related
    .operFormat = {[0 ...(NUMBER_OF_SIM - 1)] = OPER_FORMAT_NUMERIC},
    .copsModeTemp = AUTOMATIC_REGISTER,
    .preferredOperFormat = NW_PREFERRED_OPERATOR_FORMAT_NUMERIC,

    .preferredOperator = {0x00},
    .preferredOperatorIndex = OPERATOR_NUMBER_ORDER,
    .preferredOperatorCaller = NW_GETPREFERREDOPERATORS_CALLER_READ,

    // pbk related
    .pbkListEntryCaller = PBK_LISTENTRY_CALLER_CPBR,
    .pbkCharacterSet = PBK_CHARACTER_SET_TRANSPARENT,
    .tempStorage = PHONEBOOK_MEMORY_STORAGE,

    // sat related
    .alphabet = UCS2_SET,

    // TODO..sms related
    .smsSetting = {
        .csmpFo = 17,  // [+]2007.11.20 bug 7000 TODO = 17???
        .csdhShow = 0, // show header
        .smsInfo = {
            {.nStorage1 = CFW_SMS_STORAGE_SM, // Add by XP 20130130 for SMS default storage setting.
             .nStorage2 = CFW_SMS_STORAGE_SM,
             .nStorage3 = CFW_SMS_STORAGE_ME,
             .sCnmi.nBfr = 0,
             .sCnmi.nBm = 0,
             .sCnmi.nDs = 0,
             .sCnmi.nMode = 0,
             .sCnmi.nMt = 0,
             .nServType = 0,
             .nOverflowCtrl = 1,
             .nOverflowMode = 0},
            {.nStorage1 = CFW_SMS_STORAGE_SM, // Add by XP 20130130 for SMS default storage setting.
             .nStorage2 = CFW_SMS_STORAGE_SM,
             .nStorage3 = CFW_SMS_STORAGE_ME,
             .sCnmi.nBfr = 0,
             .sCnmi.nBm = 0,
             .sCnmi.nDs = 0,
             .sCnmi.nMode = 0,
             .sCnmi.nMt = 0,
             .nServType = 0,
             .nOverflowCtrl = 1,
             .nOverflowMode = 0},
        },              // CPMS命令相关的存储器信息
        .smsFormat = 0, // SMS Foramt: 0: PDU 1:text
    },

    // ss related
    .callWaitingPresentResultCode = DISABLE_RESULT_CODE_PRESENTION_STATUS, // CCWA 命令用到的cfg
    .ussd = DISABLE_RESULT_CODE_PRESENTION_STATUS,                         // URC control signal
    .ucClip = DISABLE_RESULT_CODE_PRESENTION_STATUS,
    .ucClir = CLIR_PRESENTION_INDICATOR,
    .ucColp = DISABLE_RESULT_CODE_PRESENTION_STATUS,
    .ucCSSU = DISABLE_CSSU_RESULT_CODE,
    .ucCSSI = DISABLE_CSSI_RESULT_CODE,

    // gprs
    .gprsAuto = 3,      // yy [mod] 2008-4-23 for Bug ID:8185
    .baudRate = 115200, // added by yangtt 04-16
    // special related
    .moniPara = MONI_DISPLAY_PERIOD,
    .monpPara = MONP_DISPLAY_PERIOD,
    .pmIndMark = PM_BATTERY_IND_DISABLE,
    .vgrVolume = RECEIVE_GAIN,
    .vgtVolume = TRANSMIT_GAIN,
    .cmutMute = DISABLE_UPLINK_VOICE,
    .audioTestMode = AT_AUDIO_TEST_EARPIECE,

    .tcpip = {
        .cipSCONT_unSaved = 1,
        .cipMux_multiIp = 0,
        .cipHEAD_addIpHead = 0,
        .cipSHOWTP_dispTP = 0,
        .cipSRIP_showIPPort = 0,
        .cipATS_setTimer = 0,
        .cipATS_time = 1,
        .cipSPRT_sendPrompt = 1,
        .cipQSEND_quickSend = 0,
        .cipMODE_transParent = 0,
        .cipCCFG_NmRetry = 5,
        .cipCCFG_WaitTm = 2,
        .cipCCFG_SendSz = 1024,
        .cipCCFG_esc = 1,
        .cipCCFG_Rxmode = 0,
        .cipCCFG_RxSize = 1460,
        .cipCCFG_Rxtimer = 50,
        .cipDPDP_detectPDP = 0,
        .cipDPDP_interval = 0,
        .cipDPDP_timer = 3,
        .cipCSGP_setGprs = 0,
        .cipRDTIMER_rdsigtimer = 2000,
        .cipRDTIMER_rdmuxtimer = 3500,
        .cipRXGET_manualy = 0,
        .cipApn = {0},
        .cipUserName = {0},
        .cipPassWord = {0},
    },
};

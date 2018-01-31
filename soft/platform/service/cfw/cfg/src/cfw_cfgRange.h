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











#define AT_PARAM_RANGE_VTS               "0-9,#,*,A,D" //Dial tone.

// for AT^SNFO=?
#define AT_PARAM_RANGE_SNFO_OUTBBCGAIN     "0-4"       // OutBbcGain
#define AT_PARAM_RANGE_SNFO_OUTCALIBRATE_0 "0-32767"   // OutCalibrate 0
#define AT_PARAM_RANGE_SNFO_OUTCALIBRATE_1 "0-32767"   // OutCalibrate 1
#define AT_PARAM_RANGE_SNFO_OUTCALIBRATE_2 "0-32767"   // OutCalibrate 2
#define AT_PARAM_RANGE_SNFO_OUTCALIBRATE_3 "0-32767"   // OutCalibrate 3
#define AT_PARAM_RANGE_SNFO_OUTCALIBRATE_4 "0-32767"   // OutCalibrate 4
#define AT_PARAM_RANGE_SNFO_OUTSTEP        "0-4"       // OutStep
#define AT_PARAM_RANGE_SNFO_SIDETONE       "0-32767"   // SideTone

// for AT^SNFS=?
#define AT_PARAM_RANGE_SNFS_AUDMODE        "0-6"       // AudMode

// for AT+CHLD=?
#define AT_PARAM_RANGE_SNFS_CHLD           "0,1,1X,2,2X,3"       // AudMode 

// add by cxh end.


/************************************************************************/
/* Wei Na                                                               */
/************************************************************************/
#define AT_CISS_NUMTYPE_RANGE   "129,145"
#define AT_CISS_CLASS_RANGE     "1,2,4,8,16,32,64,128"

#define AT_CCFC_REASON_RANGE    "0-5"
#define AT_CCFC_MODE_RANGE      "0-4"
#define AT_CCFC_TIME_RANGE      "1-30"
#define AT_CCFC_STATUS_RANGE    "0-1"

#define AT_CCWA_STATUS_RANGE    "0-1"
#define AT_CCWA_MODE_RANGE      "0-2"

#define AT_CLIP_N_RANGE         "0-1"
#define AT_CLIP_M_RANGE         "0-2"

#define AT_COPS_MODE_RANGE      "0-4"
#define AT_COPS_FORMAT_RANGE    "0,2"
#define AT_COPS_STATE_RANGE     "0-3"

#define AT_CREG_N_RANGE         "0-2"
#define AT_CREG_STAT_RANGE      "0-5"

#define AT_CSQ_RSSI_RANGE       "0-31,99"
#define AT_CSQ_BER_RANGE        "0-7,99"

#define AT_CSSN_NM_RANGE         "0-1"

/************************************************************************/
/* Nie Yong Feng                                                        */
/************************************************************************/
// AT+CMER
#define AT_PARAM_CMER_MODE "0-3"
#define AT_PARAM_CMER_KEYP "0-1"
#define AT_PARAM_CMER_DISP "0-2"
#define AT_PARAM_CMER_IND  "0-2"
#define AT_PARAM_CMER_BFR  "0-1"


// AT+CLVL
#define AT_PARAM_CLVL "0-4"

// AT+CFUN
#define AT_PARAM_CFUN_LEVEL_RANGE  "0-4,5-127"
#define AT_PARAM_CFUN_RESULT_RANGE  "0-1"

// AT+CALA
#define AT_PARAM_CALA_INDEX "1-15"





/************************************************************************/
/* Qi Yan                                                               */
/************************************************************************/
#define CNMI_DEFAULT_VALUE    1
#define CMER_DEFAULT_VALUE    0
#define NEW_SMS_DEFAULT_VALUE 1
/*
NAME    FUNCTION            PARAMETER   RANGE    DEFAULT
ATE     CFW_CfgSetEchoMode  nMode       0-1      1
*/
#define AT_E_RANGE            "0-1"
#define AT_E_DEDAULT          1

/*
NAME    FUNCTION                        PARAMETER   RANGE    DEFAULT
ATQ     CFW_CfgSetResultCodePresentMode nMode       0-1      0
*/
#define AT_Q_RANGE              "0-1"
#define AT_Q_DEDAULT            0


/*
NAME    FUNCTION                        PARAMETER   RANGE    DEFAULT
ATV     CFW_CfgSetResultCodeFormatMode  nMode       0-1      1
*/
#define AT_V_RANGE              "0-1"
#define AT_V_DEDAULT            1


/*
NAME    FUNCTION                            PARAMETER   RANGE    DEFAULT
ATX     CFW_CfgSetConnectResultFormatCode   nValue      0-4      4
*/
#define AT_X_RANGE            "0-4"
#define AT_X_DEDAULT          4

/*
AT+CBST     CFW_CfgSetBearServType  nSpeed      0,4,6,7,14,68,70,71,75  7
*/
#define AT_BST_RANGE            "0,4,6,7,14,68,70,71,75"
#define AT_BST_DEDAULT          7


/*
NAME    FUNCTION                PARAMETER   RANGE  DEFAULT
AT+CMEE CFW_CfgSetErrorReport   nLevel      0-2    0
*/
#define AT_CMEE_RANGE           "0-2"
#define AT_CMEE_DEDAULT          0


/*
NAME        FUNCTION            PARAMETER   RANGE   DEFAULT
AT+CRLP     CFW_CfgSetRLPType   iws         0-61    61
                                mws         0-61    61
                                T1          48-255  78
                                N2          1-255   6
*/
#define AT_RLP_IWS_RANGE            "0-61"
#define AT_RLP_IWS_DEDAULT          61

#define AT_RLP_MWS_RANGE            "0-61"
#define AT_RLP_MWS_DEDAULT          61

#define AT_RLP_T1_RANGE            "48-255"
#define AT_RLP_T1_DEDAULT          78

#define AT_RLP_N2_RANGE            "1-255"
#define AT_RLP_M2_DEDAULT          6

/*
NAME    FUNCTION                          PARAMETER   RANGE  DEFAULT
AT+CRC  CFW_CfgSetIncomingCallResultMode  nMode       0-1    0
*/
#define AT_CRC_RANGE            "0-1"
#define AT_CRC_DEDAULT          0

/*
NAME    FUNCTION                            PARAMETER   RANGE  DEFAULT
AT+CR   CFW_CfgSetServiceReportingMode      nMode       0-1    0
*/
#define AT_CR_RANGE            "0-1"
#define AT_CR_DEDAULT          0

/*
NAME    FUNCTION                PARAMETER   RANGE   DEFAULT
AT+CSMS CFW_CfgSelectSmsService nService    0-1     0
*/
#define AT_SMS_SERV_SELECT_DEDAULT          0


/*
NAME        FUNCTION                PARAMETER   RANGE   DEFAULT
AT+CREG     CFW_CfgSetNwStatus      N           0-2     0
*/
#define AT_REG_DEDAULT          1

/*
NAME        FUNCTION                PARAMETER   RANGE   DEFAULT
AT+CLIP     CFW_CfgSetClip          N           0-1     1
*/
#define AT_CLIP_DEDAULT          1
//[[hameina [+] 2006.8.23
#define NW_WORKMODE_RANGE       "0,1,4"
#define NW_DETECTMBS_RANGE       "0,1"
#define NW_REG_RANGE            "0-2"
#define NW_DispFormat_RANGE     "0,2"

//Lixp add 20080801
#define NW_FLIGHTMODE_RANGE     "0-2"

#define GPRS_CIND_RANGE         "1-7"

#ifdef CFW_CUSTOM_IMEI_COUNT
#define SMS_IMEI_RANGE           "0,1"

#endif
#define SMS_SERV_SELECT_RANGE "0-1"
#define SMS_OVERFLOW_RANGE      "0,1"
#define SMS_FULLFORNEW_RANGE    "0-255"
#define SMS_STORAGE_RANGE       "1,2,4,8,16,32,64"
#define SMS_OPTION_RANGE        "0-255"
#define SMS_OPT_STORAGE_RABGE    "0-255"

#define CM_CCWA_RANGE           "0,1"
#define CM_CLIR_RANGE           "0,1,2,4,5"
#define CM_CLIP_RANGE           "0-1"
#define CM_COLP_RANGE           "0-1"

#define DE_AUDIOMODE_RANGE      "0-2"
#define PBK_STORAGE_RANGE       "1,2,32,64"

#define GE_EV_INFO_BC_RANGE         "0-5"
#define GE_EV_INFO_NWSRV_RANGE      "0-1"
#define GE_EV_INFO_NWBITER_RANGE    "0,1,2,3,4,5,6,7,99"

#define SIMCARD_SWITCH_RABGE    "0-255"
#define IS_FIRST_POWER_ON_RANGE       "0,1"

//]]hameina[+]
#define SOFT_RESETNUM_RANGE     "0-4"

/*
NAME             FUNCTION                          PARAMETER   RANGE  DEFAULT
AT+CSCS CFW_CfgSetCharacterSet                        0-2                  0
*/
#define AT_CSCS_RANGE           "GSM,UCS2"





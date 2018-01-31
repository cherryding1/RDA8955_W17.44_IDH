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




#include "cs_types.h"
#include "sppp_gsm_private.h"
#include "spal_vitac.h"
#include "spal_cap.h"



// Parameters for CRC encoding/decoding
//-------------------------------------
PROTECTED CONST SPAL_CAP_CRC_PARAM_T g_sppCsCrcParam[CS_PARAM_QTY] =
{
    //                                  CAP Config,       Dec bits,  Enc bits
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_RACH),               0,        8}, // CS_RACH
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_PRACH),              0,       11}, // CS_PRACH
    {CAP_CTRL_CONFIG(4,15,7,7,7,7,7,7,0,0,CS_SCH),               35,        0}, // CS_SCH
    {CAP_CTRL_CONFIG(6,15,7,7,7,7,7,7,0,0,CS_CS1),              224,      184}, // CS_CS1
    {CAP_CTRL_CONFIG(5,15,7,7,7,7,7,7,0,0,CS_CS2),              287,      271}, // CS_CS2
    {CAP_CTRL_CONFIG(5,15,7,7,7,7,7,7,0,0,CS_CS3),              331,      315}, // CS_CS3
    {CAP_CTRL_CONFIG(5,15,7,7,7,7,7,7,0,0,CS_CS4),              447,      431}, // CS_CS4
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_HEAD_1),           135,       46}, // CS_HEAD_1
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_HEAD_2),            99,       37}, // CS_HEAD_2
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_HEAD_3),           108,       31}, // CS_HEAD_3
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS1),             196,      178}, // CS_MCS1
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS2),             244,      226}, // CS_MCS2
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS3),             316,      298}, // CS_MCS3
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS4),             372,      354}, // CS_MCS4
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS5),             468,      450}, // CS_MCS5
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS6),             612,      594}, // CS_MCS6
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS7),             468,      450}, // CS_MCS7
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS8),             564,      546}, // CS_MCS8
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_MCS9),             612,      594}, // CS_MCS9
    {CAP_CTRL_CONFIG(1,15,7,7,7,7,7,7,0,0,CS_HR),                25,       22}, // CS_HR
    {CAP_CTRL_CONFIG(1,15,7,7,7,7,7,7,0,0,CS_FR),                53,       50}, // CS_FR
    {CAP_CTRL_CONFIG(0,15,7,7,7,7,7,7,0,0,CS_EFR),               73,       65}, // CS_EFR
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_H24),                0,        0}, // CS_H24
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_H48),                0,        0}, // CS_H48
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_F24),                0,        0}, // CS_F24
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_F48),                0,        0}, // CS_F48
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_F96),                0,        0}, // CS_F96
    {CAP_CTRL_CONFIG(0, 0,0,0,0,0,0,0,0,0,CS_F144),               0,        0}, // CS_F144
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_122),    87,       81}, // CS_AFS_SPEECH_122
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_102),    71,       65}, // CS_AFS_SPEECH_102
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_795),    81,       75}, // CS_AFS_SPEECH_795
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_74),     67,       61}, // CS_AFS_SPEECH_74
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_67),     61,       55}, // CS_AFS_SPEECH_67
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_59),     61,       55}, // CS_AFS_SPEECH_59
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_515),    55,       49}, // CS_AFS_SPEECH_515
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AFS_SPEECH_475),    45,       39}, // CS_AFS_SPEECH_475
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AHS_SPEECH_795),    73,       67}, // CS_AHS_SPEECH_795
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AHS_SPEECH_74),     67,       61}, // CS_AHS_SPEECH_74
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AHS_SPEECH_67),     61,       55}, // CS_AHS_SPEECH_67
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AHS_SPEECH_59),     61,       55}, // CS_AHS_SPEECH_59
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AHS_SPEECH_515),    55,       49}, // CS_AHS_SPEECH_515
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,7,0,0,CS_AHS_SPEECH_475),    45,       39}, // CS_AHS_SPEECH_475
    {CAP_CTRL_CONFIG(2,15,7,7,7,7,7,7,0,0,CS_AFS_SID_UPDATE),    49,       35}, // CS_AFS_SID_UPDATE
    {CAP_CTRL_CONFIG(2,15,7,7,7,7,7,7,0,0,CS_AFS_RATSCCH),       49,       35}, // CS_AFS_RATSCCH
    {CAP_CTRL_CONFIG(2,15,7,7,7,7,7,7,0,0,CS_AHS_SID_UPDATE),    49,       35}, // CS_AHS_SID_UPDATE
    {CAP_CTRL_CONFIG(2,15,7,7,7,7,7,7,0,0,CS_AHS_RATSCCH_DATA),  49,       35}, // CS_AHS_RATSCCH_DATA
};

// Parameters for Conv encoding
//-----------------------------
PROTECTED CONST SPAL_CAP_CONV_PARAM_T g_sppCsConvParam[CS_PARAM_QTY] =
{
    //                                  CAP Config,         In bits, Out Words
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,0,0,CS_RACH),              18,       2}, // CS_RACH
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,1,0,CS_PRACH),             21,       2}, // CS_PRACH
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,0,0,CS_SCH),               39,       3}, // CS_SCH
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,0,0,CS_CS1),              228,      15}, // CS_CS1
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,1,0,CS_CS2),              294,      15}, // CS_CS2
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,1,0,CS_CS3),              338,      15}, // CS_CS3
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_CS4),                0,       0}, // CS_CS4
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_HEAD_1),            60,       0}, // CS_HEAD_1
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_HEAD_2),            51,       0}, // CS_HEAD_2
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_HEAD_3),            45,       0}, // CS_HEAD_3
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS1),             196,       0}, // CS_MCS1
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS2),             244,       0}, // CS_MCS2
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS3),             316,       0}, // CS_MCS3
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS4),             372,       0}, // CS_MCS4
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS5),             468,       0}, // CS_MCS5
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS6),             612,       0}, // CS_MCS6
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS7),             468,       0}, // CS_MCS7
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS8),             564,       0}, // CS_MCS8
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_MCS9),             612,       0}, // CS_MCS9
    {CAP_CTRL_CONFIG( 4,5,6,7,7,7,7,3,1,0,CS_HR),               104,       7}, // CS_HR
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,0,0,CS_FR),               189,      12}, // CS_FR
    {CAP_CTRL_CONFIG( 0,0,0,0,0,0,0,0,0,0,CS_EFR),                0,       0}, // CS_EFR
    {CAP_CTRL_CONFIG( 1,2,3,7,7,7,7,3,0,0,CS_H24),              152,      15}, // CS_H24
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,1,0,CS_H48),              244,      15}, // CS_H48
    {CAP_CTRL_CONFIG( 1,2,3,1,2,3,7,6,0,0,CS_F24),               76,      15}, // CS_F24
    {CAP_CTRL_CONFIG( 1,2,3,7,7,7,7,3,0,0,CS_F48),              152,      15}, // CS_F48
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,1,0,CS_F96),              244,      15}, // CS_F96
    {CAP_CTRL_CONFIG( 0,1,7,7,7,7,7,2,1,0,CS_F144),             294,      15}, // CS_F144
    {CAP_CTRL_CONFIG(15,1,7,7,7,7,0,2,1,0,CS_AFS_SPEECH_122),   254,      14}, // CS_AFS_SPEECH_122
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,3,1,0,CS_AFS_SPEECH_102),   214,      14}, // CS_AFS_SPEECH_102
    {CAP_CTRL_CONFIG(15,5,6,7,7,7,4,3,1,0,CS_AFS_SPEECH_795),   171,      14}, // CS_AFS_SPEECH_795
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,3,1,0,CS_AFS_SPEECH_74),    158,      14}, // CS_AFS_SPEECH_74
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,4,1,0,CS_AFS_SPEECH_67),    144,      14}, // CS_AFS_SPEECH_67
    {CAP_CTRL_CONFIG( 4,5,7,7,7,7,6,4,1,0,CS_AFS_SPEECH_59),    130,      14}, // CS_AFS_SPEECH_59
    {CAP_CTRL_CONFIG( 1,1,2,7,7,7,3,5,1,0,CS_AFS_SPEECH_515),   113,      14}, // CS_AFS_SPEECH_515
    {CAP_CTRL_CONFIG( 4,4,5,7,7,7,6,5,1,0,CS_AFS_SPEECH_475),   107,      14}, // CS_AFS_SPEECH_475
    {CAP_CTRL_CONFIG(15,1,7,7,7,7,0,2,1,0,CS_AHS_SPEECH_795),   133,       6}, // CS_AHS_SPEECH_795
    {CAP_CTRL_CONFIG(15,1,7,7,7,7,0,2,1,0,CS_AHS_SPEECH_74),    130,       7}, // CS_AHS_SPEECH_74
    {CAP_CTRL_CONFIG(15,1,7,7,7,7,0,2,1,0,CS_AHS_SPEECH_67),    120,       7}, // CS_AHS_SPEECH_67
    {CAP_CTRL_CONFIG(15,1,7,7,7,7,0,2,1,0,CS_AHS_SPEECH_59),    112,       7}, // CS_AHS_SPEECH_59
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,3,1,0,CS_AHS_SPEECH_515),   101,       7}, // CS_AHS_SPEECH_515
    {CAP_CTRL_CONFIG(15,5,6,7,7,7,4,3,1,0,CS_AHS_SPEECH_475),    95,       7}, // CS_AHS_SPEECH_475
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,4,0,0,CS_AFS_SID_UPDATE),    53,       7}, // CS_AFS_SID_UPDATE
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,4,0,0,CS_AFS_RATSCCH),       53,       7}, // CS_AFS_RATSCCH
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,4,0,0,CS_AHS_SID_UPDATE),    53,       7}, // CS_AHS_SID_UPDATE
    {CAP_CTRL_CONFIG( 1,2,7,7,7,7,3,4,0,0,CS_AHS_RATSCCH_DATA),  53,       7}, // CS_AHS_RATSCCH_DATA
};

// Parameters for Post SRC encoding
//---------------------------------
PROTECTED CONST SPAL_CAP_CONV_PARAM_T g_sppCsSrcParam[CS_SRC_PARAM_QTY] =
{
    //                                          CAP Config, In bits, Out Words
    {CAP_CTRL_CONFIG(0,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_122),   250,       8}, // CS_AFS_SPEECH_122
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_102),   210,       7}, // CS_AFS_SPEECH_102
    {CAP_CTRL_CONFIG(4,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_795),   165,       6}, // CS_AFS_SPEECH_795
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_74),    154,       5}, // CS_AFS_SPEECH_74
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_67),    140,       5}, // CS_AFS_SPEECH_67
    {CAP_CTRL_CONFIG(6,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_59),    124,       5}, // CS_AFS_SPEECH_59
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_515),   109,       4}, // CS_AFS_SPEECH_515
    {CAP_CTRL_CONFIG(6,15,7,7,7,7,7,1,0,0,CS_AFS_SPEECH_475),   101,       4}, // CS_AFS_SPEECH_475

    {CAP_CTRL_CONFIG(0,15,7,7,7,7,7,1,0,0,CS_AHS_SPEECH_795),   129,       5}, // CS_AHS_SPEECH_795
    {CAP_CTRL_CONFIG(0,15,7,7,7,7,7,1,0,0,CS_AHS_SPEECH_74),    126,       5}, // CS_AHS_SPEECH_74
    {CAP_CTRL_CONFIG(0,15,7,7,7,7,7,1,0,0,CS_AHS_SPEECH_67),    116,       4}, // CS_AHS_SPEECH_67
    {CAP_CTRL_CONFIG(0,15,7,7,7,7,7,1,0,0,CS_AHS_SPEECH_59),    108,       4}, // CS_AHS_SPEECH_59
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AHS_SPEECH_515),   97,       4}, // CS_AHS_SPEECH_515
    {CAP_CTRL_CONFIG(4,15,7,7,7,7,7,1,0,0,CS_AHS_SPEECH_475),   89,       3}, // CS_AHS_SPEECH_475

    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AFS_SID_UPDATE),   212,       2}, // CS_AFS_SID_UPDATE
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AFS_RATSCCH),      212,       2}, // CS_AFS_RATSCCH
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AHS_SID_UPDATE),   212,       2}, // CS_AHS_SID_UPDATE
    {CAP_CTRL_CONFIG(3,15,7,7,7,7,7,1,0,0,CS_AHS_RATSCCH_DATA), 212,       2}, // CS_AHS_RATSCCH_DATA
};


// Parameters for VITERBI decoding
//--------------------------------
PROTECTED CONST SPAL_VITAC_DEC_PARAM_T g_sppVitacDecParam[CS_PARAM_QTY] =
{
    // trellis states,      code rate, punct, rP0, rP1, rP2,  header  nbSymbol
    {              0,              0,     0,   0,   0,   0,  FALSE,     0}, // RACH
    {              0,              0,     0,   0,   0,   0,  FALSE,     0}, // PRACH
    {VITAC_16_STATES, VITAC_RATE_1_2, FALSE,   0,   1,   0,  FALSE,    39}, // SCH
    {VITAC_16_STATES, VITAC_RATE_1_2, FALSE,   0,   1,   0,  FALSE,   228}, // CS1
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   294}, // CS2
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   338}, // CS3
    {              0,              0,     0,   0,   0,   0,  FALSE,     0}, // CS4
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,   TRUE,   135}, // HEAD_1
    {VITAC_64_STATES, VITAC_RATE_1_3, FALSE,   4,   7,   5,   TRUE,    99}, // HEAD_2
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,   TRUE,   108}, // HEAD_3
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   196}, // MCS1
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   244}, // MCS2
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   316}, // MCS3
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   372}, // MCS4
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   468}, // MCS5
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   612}, // MCS6
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   468}, // MCS7
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   564}, // MCS8
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   7,   5,  FALSE,   612}, // MCS9
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   5,   6,  FALSE,   104}, // HR
    {VITAC_16_STATES, VITAC_RATE_1_2, FALSE,   0,   1,   0,  FALSE,   189}, // FR
    {VITAC_16_STATES, VITAC_RATE_1_2, FALSE,   0,   1,   0,  FALSE,   189}, // EFR
    {VITAC_16_STATES, VITAC_RATE_1_3, FALSE,   1,   2,   3,  FALSE,   152}, // H24
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   244}, // H48
    {VITAC_16_STATES, VITAC_RATE_1_6, FALSE,   1,   2,   3,  FALSE,    76}, // F24
    {VITAC_16_STATES, VITAC_RATE_1_3, FALSE,   1,   2,   3,  FALSE,   152}, // F48
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   244}, // F96
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   294}, // F144
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   254}, // CS_AFS_SPEECH_122
    {VITAC_16_STATES, VITAC_RATE_1_3,  TRUE,   1,   2,   3,  FALSE,   214}, // CS_AFS_SPEECH_102
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   5,   6,  FALSE,   171}, // CS_AFS_SPEECH_795
    {VITAC_16_STATES, VITAC_RATE_1_3,  TRUE,   1,   2,   3,  FALSE,   158}, // CS_AFS_SPEECH_74
    {VITAC_16_STATES, VITAC_RATE_1_4,  TRUE,   1,   2,   3,  FALSE,   144}, // CS_AFS_SPEECH_67
    {VITAC_64_STATES, VITAC_RATE_1_4,  TRUE,   4,   5,   6,  FALSE,   130}, // CS_AFS_SPEECH_59
    {VITAC_16_STATES, VITAC_RATE_1_5,  TRUE,   1,   2,   3,  FALSE,   113}, // CS_AFS_SPEECH_515
    {VITAC_64_STATES, VITAC_RATE_1_5,  TRUE,   4,   5,   6,  FALSE,   107}, // CS_AFS_SPEECH_475
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   133}, // CS_AHS_SPEECH_795
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   130}, // CS_AHS_SPEECH_74
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   120}, // CS_AHS_SPEECH_67
    {VITAC_16_STATES, VITAC_RATE_1_2,  TRUE,   0,   1,   0,  FALSE,   112}, // CS_AHS_SPEECH_59
    {VITAC_16_STATES, VITAC_RATE_1_3,  TRUE,   1,   2,   3,  FALSE,   101}, // CS_AHS_SPEECH_515
    {VITAC_64_STATES, VITAC_RATE_1_3,  TRUE,   4,   5,   6,  FALSE,    95}, // CS_AHS_SPEECH_475
    {VITAC_16_STATES, VITAC_RATE_1_4, FALSE,   1,   2,   3,  FALSE,    53}, // CS_AFS_SID_UPDATE
    {VITAC_16_STATES, VITAC_RATE_1_4, FALSE,   1,   2,   3,  FALSE,    53}, // CS_AFS_RATSCCH
    {VITAC_16_STATES, VITAC_RATE_1_4, FALSE,   1,   2,   3,  FALSE,    53}, // CS_AHS_SID_UPDATE
    {VITAC_16_STATES, VITAC_RATE_1_4, FALSE,   1,   2,   3,  FALSE,    53}, // CS_AHS_RATSCCH_DATA
};


// Polynomes for the convolution
//------------------------------
PROTECTED CONST UINT32 g_sppVitacPolyres[8] =
{
    0x000000F0,
    0x0000005A,
    0x000000CC,
    0x00000096,
    0xC3C33C3C,
    0x55AA55AA,
    0x69966996,
    0x96969696
};


// Reordering Tables
//------------------

// Table 7: Sorting of the speech encoded bits for TCH/AFS12.2
PRIVATE CONST UINT8 REORDER_AMR122[244] =
{
    0  ,  1  ,  2  ,  3  ,  4  ,  5  ,  6  ,  7  ,  8  ,  9  ,
    10 ,  11 ,  12 ,  13 ,  14 ,  23 ,  15 ,  16 ,  17 ,  18 ,
    19 ,  20 ,  21 ,  22 ,  24 ,  25 ,  26 ,  27 ,  28 ,  38 ,
    141,  39 ,  142,  40 ,  143,  41 ,  144,  42 ,  145,  43 ,
    146,  44 ,  147,  45 ,  148,  46 ,  149,  47 ,  97 ,  150,
    200,  48 ,  98 ,  151,  201,  49 ,  99 ,  152,  202,  86 ,
    136,  189,  239,  87 ,  137,  190,  240,  88 ,  138,  191,
    241,  91 ,  194,  92 ,  195,  93 ,  196,  94 ,  197,  95 ,
    198,  29 ,  30 ,  31 ,  32 ,  33 ,  34 ,  35 ,  50 ,  100,
    153,  203,  89 ,  139,  192,  242,  51 ,  101,  154,  204,
    55 ,  105,  158,  208,  90 ,  140,  193,  243,  59 ,  109,
    162,  212,  63 ,  113,  166,  216,  67 ,  117,  170,  220,
    36 ,  37 ,  54 ,  53 ,  52 ,  58 ,  57 ,  56 ,  62 ,  61 ,
    60 ,  66 ,  65 ,  64 ,  70 ,  69 ,  68 ,  104,  103,  102,
    108,  107,  106,  112,  111,  110,  116,  115,  114,  120,
    119,  118,  157,  156,  155,  161,  160,  159,  165,  164,
    163,  169,  168,  167,  173,  172,  171,  207,  206,  205,
    211,  210,  209,  215,  214,  213,  219,  218,  217,  223,
    222,  221,  73 ,  72 ,  71 ,  76 ,  75 ,  74 ,  79 ,  78 ,
    77 ,  82 ,  81 ,  80 ,  85 ,  84 ,  83 ,  123,  122,  121,
    126,  125,  124,  129,  128,  127,  132,  131,  130,  135,
    134,  133,  176,  175,  174,  179,  178,  177,  182,  181,
    180,  185,  184,  183,  188,  187,  186,  226,  225,  224,
    229,  228,  227,  232,  231,  230,  235,  234,  233,  238,
    237,  236,  96 ,  199
};


// Table 8: Sorting of the speech encoded bits for TCH/AFS10.2
PRIVATE CONST UINT8 REORDER_AMR102[204] =
{
    7  ,  6  ,  5  ,  4  ,  3  ,  2  ,  1  ,  0  ,  16 ,  15 ,
    14 ,  13 ,  12 ,  11 ,  10 ,  9  ,  8  ,  26 ,  27 ,  28 ,
    29 ,  30 ,  31 ,  115,  116,  117,  118,  119,  120,  72 ,
    73 ,  161,  162,  65 ,  68 ,  69 ,  108,  111,  112,  154,
    157,  158,  197,  200,  201,  32 ,  33 ,  121,  122,  74 ,
    75 ,  163,  164,  66 ,  109,  155,  198,  19 ,  23 ,  21 ,
    22 ,  18 ,  17 ,  20 ,  24 ,  25 ,  37 ,  36 ,  35 ,  34 ,
    80 ,  79 ,  78 ,  77 ,  126,  125,  124,  123,  169,  168,
    167,  166,  70 ,  67 ,  71 ,  113,  110,  114,  159,  156,
    160,  202,  199,  203,  76 ,  165,  81 ,  82 ,  92 ,  91 ,
    93 ,  83 ,  95 ,  85 ,  84 ,  94 ,  101,  102,  96 ,  104,
    86 ,  103,  87 ,  97 ,  127,  128,  138,  137,  139,  129,
    141,  131,  130,  140,  147,  148,  142,  150,  132,  149,
    133,  143,  170,  171,  181,  180,  182,  172,  184,  174,
    173,  183,  190,  191,  185,  193,  175,  192,  176,  186,
    38 ,  39 ,  49 ,  48 ,  50 ,  40 ,  52 ,  42 ,  41 ,  51 ,
    58 ,  59 ,  53 ,  61 ,  43 ,  60 ,  44 ,  54 ,  194,  179,
    189,  196,  177,  195,  178,  187,  188,  151,  136,  146,
    153,  134,  152,  135,  144,  145,  105,  90 ,  100,  107,
    88 ,  106,  89 ,  98 ,  99 ,  62 ,  47 ,  57 ,  64 ,  45 ,
    63 ,  46 ,  55 ,  56
};


// Table 9: Sorting of the speech encoded bits for TCH/AFS7.95 and TCH/AHS7.95
PRIVATE CONST UINT8 REORDER_AMR795[159] =
{
    8  ,  7  ,  6  ,  5  ,  4  ,  3  ,  2  ,  14 ,  16 ,  9  ,
    10 ,  12 ,  13 ,  15 ,  11 ,  17 ,  20 ,  22 ,  24 ,  23 ,
    19 ,  18 ,  21 ,  56 ,  88 ,  122,  154,  57 ,  89 ,  123,
    155,  58 ,  90 ,  124,  156,  52 ,  84 ,  118,  150,  53 ,
    85 ,  119,  151,  27 ,  93 ,  28 ,  94 ,  29 ,  95 ,  30 ,
    96 ,  31 ,  97 ,  61 ,  127,  62 ,  128,  63 ,  129,  59 ,
    91 ,  125,  157,  32 ,  98 ,  64 ,  130,  1  ,  0  ,  25 ,
    26 ,  33 ,  99 ,  34 ,  100,  65 ,  131,  66 ,  132,  54 ,
    86 ,  120,  152,  60 ,  92 ,  126,  158,  55 ,  87 ,  121,
    153,  117,  116,  115,  46 ,  78 ,  112,  144,  43 ,  75 ,
    109,  141,  40 ,  72 ,  106,  138,  36 ,  68 ,  102,  134,
    114,  149,  148,  147,  146,  83 ,  82 ,  81 ,  80 ,  51 ,
    50 ,  49 ,  48 ,  47 ,  45 ,  44 ,  42 ,  39 ,  35 ,  79 ,
    77 ,  76 ,  74 ,  71 ,  67 ,  113,  111,  110,  108,  105,
    101,  145,  143,  142,  140,  137,  133,  41 ,  73 ,  107,
    139,  37 ,  69 ,  103,  135,  38 ,  70 ,  104,  136
};


// Table 10: Sorting of the speech encoded bits for TCH/AFS7.4 and TCH/AHS7.4
PRIVATE CONST UINT8 REORDER_AMR74[148] =
{
    0  ,  1  ,  2  ,  3  ,  4  ,  5  ,  6  ,  7  ,  8  ,  9  ,
    10 ,  11 ,  12 ,  13 ,  14 ,  15 ,  16 ,  26 ,  87 ,  27 ,
    88 ,  28 ,  89 ,  29 ,  90 ,  30 ,  91 ,  51 ,  80 ,  112,
    141,  52 ,  81 ,  113,  142,  54 ,  83 ,  115,  144,  55 ,
    84 ,  116,  145,  58 ,  119,  59 ,  120,  21 ,  22 ,  23 ,
    17 ,  18 ,  19 ,  31 ,  60 ,  92 ,  121,  56 ,  85 ,  117,
    146,  20 ,  24 ,  25 ,  50 ,  79 ,  111,  140,  57 ,  86 ,
    118,  147,  49 ,  78 ,  110,  139,  48 ,  77 ,  53 ,  82 ,
    114,  143,  109,  138,  47 ,  76 ,  108,  137,  32 ,  33 ,
    61 ,  62 ,  93 ,  94 ,  122,  123,  41 ,  42 ,  43 ,  44 ,
    45 ,  46 ,  70 ,  71 ,  72 ,  73 ,  74 ,  75 ,  102,  103,
    104,  105,  106,  107,  131,  132,  133,  134,  135,  136,
    34 ,  63 ,  95 ,  124,  35 ,  64 ,  96 ,  125,  36 ,  65 ,
    97 ,  126,  37 ,  66 ,  98 ,  127,  38 ,  67 ,  99 ,  128,
    39 ,  68 ,  100,  129,  40 ,  69 ,  101,  130
};


// Table 11: Sorting of the speech encoded bits for TCH/AFS6.7 and TCH/AHS6.7
PRIVATE CONST UINT8 REORDER_AMR67[134] =
{
    0  ,  1  ,  4  ,  3  ,  5  ,  6  ,  13 ,  7  ,  2  ,  8  ,
    9  ,  11 ,  15 ,  12 ,  14 ,  10 ,  28 ,  82 ,  29 ,  83 ,
    27 ,  81 ,  26 ,  80 ,  30 ,  84 ,  16 ,  55 ,  109,  56 ,
    110,  31 ,  85 ,  57 ,  111,  48 ,  73 ,  102,  127,  32 ,
    86 ,  51 ,  76 ,  105,  130,  52 ,  77 ,  106,  131,  58 ,
    112,  33 ,  87 ,  19 ,  23 ,  53 ,  78 ,  107,  132,  21 ,
    22 ,  18 ,  17 ,  20 ,  24 ,  25 ,  50 ,  75 ,  104,  129,
    47 ,  72 ,  101,  126,  54 ,  79 ,  108,  133,  46 ,  71 ,
    100,  125,  128,  103,  74 ,  49 ,  45 ,  70 ,  99 ,  124,
    42 ,  67 ,  96 ,  121,  39 ,  64 ,  93 ,  118,  38 ,  63 ,
    92 ,  117,  35 ,  60 ,  89 ,  114,  34 ,  59 ,  88 ,  113,
    44 ,  69 ,  98 ,  123,  43 ,  68 ,  97 ,  122,  41 ,  66 ,
    95 ,  120,  40 ,  65 ,  94 ,  119,  37 ,  62 ,  91 ,  116,
    36 ,  61 ,  90 ,  115
};


// Table 12: Sorting of the speech encoded bits for TCH/AFS5.9 and TCH/AHS5.9
PRIVATE CONST UINT8 REORDER_AMR59[118] =
{
    0  ,  1  ,  4  ,  5  ,  3  ,  6  ,  7  ,  2  ,  13 ,  15 ,
    8  ,  9  ,  11 ,  12 ,  14 ,  10 ,  16 ,  28 ,  74 ,  29 ,
    75 ,  27 ,  73 ,  26 ,  72 ,  30 ,  76 ,  51 ,  97 ,  50 ,
    71 ,  96 ,  117,  31 ,  77 ,  52 ,  98 ,  49 ,  70 ,  95 ,
    116,  53 ,  99 ,  32 ,  78 ,  33 ,  79 ,  48 ,  69 ,  94 ,
    115,  47 ,  68 ,  93 ,  114,  46 ,  67 ,  92 ,  113,  19 ,
    21 ,  23 ,  22 ,  18 ,  17 ,  20 ,  24 ,  111,  43 ,  89 ,
    110,  64 ,  65 ,  44 ,  90 ,  25 ,  45 ,  66 ,  91 ,  112,
    54 ,  100,  40 ,  61 ,  86 ,  107,  39 ,  60 ,  85 ,  106,
    36 ,  57 ,  82 ,  103,  35 ,  56 ,  81 ,  102,  34 ,  55 ,
    80 ,  101,  42 ,  63 ,  88 ,  109,  41 ,  62 ,  87 ,  108,
    38 ,  59 ,  84 ,  105,  37 ,  58 ,  83 ,  104
};


// Table 13: Sorting of the speech encoded bits for TCH/AFS5.15 and TCH/AHS5.15
PRIVATE CONST UINT8 REORDER_AMR515[103] =
{
    7 ,   6 ,   5 ,   4  ,  3  ,  2  ,  1 ,   0 ,   15,   14,
    13,   12,   11,   10 ,  9  ,  8  ,  23,   24,   25,   26,
    27,   46,   65,   84 ,  45 ,  44 ,  43,   64,   63,   62,
    83,   82,   81,   102,  101,  100,  42,   61,   80,   99,
    28,   47,   66,   85 ,  18 ,  41 ,  60,   79,   98,   29,
    48,   67,   17,   20 ,  22 ,  40 ,  59,   78,   97,   21,
    30,   49,   68,   86 ,  19 ,  16 ,  87,   39,   38,   58,
    57,   77,   35,   54 ,  73 ,  92 ,  76,   96,   95,   36,
    55,   74,   93,   32 ,  51 ,  33 ,  52,   70,   71,   89,
    90,   31,   50,   69 ,  88 ,  37 ,  56,   75,   94,   34,
    53,   72,   91
};


// Table 14: Sorting of the speech encoded bits for TCH/AFS4.75 and TCH/AHS4.75
PRIVATE CONST UINT8 REORDER_AMR475[ 95] =
{
    0 ,   1 ,   2 ,   3 ,   4 ,   5 ,   6 ,   7 ,   8 ,   9 ,
    10,   11,   12,   13,   14,   15,   23,   24,   25,   26,
    27,   28,   48,   49,   61,   62,   82,   83,   47,   46,
    45,   44,   81,   80,   79,   78,   17,   18,   20,   22,
    77,   76,   75,   74,   29,   30,   43,   42,   41,   40,
    38,   39,   16,   19,   21,   50,   51,   59,   60,   63,
    64,   72,   73,   84,   85,   93,   94,   32,   33,   35,
    36,   53,   54,   56,   57,   66,   67,   69,   70,   87,
    88,   90,   91,   34,   55,   68,   89,   37,   58,   71,
    92,   31,   52,   65,   86
};

// AFS Speech channel coding parameters
//-------------------------------------
//// Input: 244, 204, 159, 148, 134, 118, 103, 95 bits
//PROTECTED UINT8 g_sppInputLengthBitAfs[8] = {244, 204, 159, 148, 134, 118, 103, 95};
// Input: 95, 103, 118, 134, 148, 159, 204, 244 bits
PROTECTED CONST UINT8 g_sppInputLengthBitAfs[8] = {95, 103, 118, 134, 148, 159, 204, 244};

// Reordering table
PROTECTED CONST UINT8* CONST g_sppAfsReorderTables[8] =
{
    REORDER_AMR475,
    REORDER_AMR515,
    REORDER_AMR59,
    REORDER_AMR67,
    REORDER_AMR74,
    REORDER_AMR795,
    REORDER_AMR102,
    REORDER_AMR122
};

// AHS Speech channel coding parameters
//-------------------------------------
// Input: 159, 148, 134, 118, 103, 95 bits
PROTECTED CONST UINT8 g_sppInputLengthBitAhs[6] = {95, 103, 118, 134, 148, 159};
// Reordering table
PROTECTED CONST UINT8* CONST g_sppAhsReorderTables[6] =
{
    REORDER_AMR475,
    REORDER_AMR515,
    REORDER_AMR59,
    REORDER_AMR67,
    REORDER_AMR74,
    REORDER_AMR795
};



// Constant tables used for non-AMR channels
//------------------------------------------
// reordering of HR (table 3a & 3b)
PROTECTED CONST UINT8 g_sppMatrixHr[2][112] =
{
    {
        1,  32,  52,  71,  90, 109,   5,  21,  20,  19,
        18,  17,  31,  30,  29,  28,  27,  26,  55,  68,
        67,  66,  65,  64,  63,  62,  80,  79,  78,  77,
        76,  75,  74,  87,  86,  85,  84,  25,   0,  33,
        56,  57,  58,  59,  60,  61,  43,  44,  45,  46,
        47,  48,  49,  36,  37,  38,  39,  40,  41,  42,
        107,  88,  69,  50,  16, 108,  89,  70,  51,   6,
        7,   8,   9,  10, 110,  91,  72,  53,  22,  23,
        24, 111,  92,  73,  54,  11,  12,  13,  14,   2,
        15,   3,   4,  34,  35, 100, 101, 102, 103, 104,
        105, 106,  93,  94,  95,  96,  97,  98,  99,  81,
        82,  83
    },
    {
        7,   6,  22,  21,  20,  57,  75,  93, 111,  56,
        74,  92, 110,  55,  73,  91, 109,  52,  51,  50,
        49,  48,  47,  46,  45,  44,  70,  69,  68,  67,
        64,  63,  62,  88,  66,  65,  54,  72,  90, 108,
        53,  71,  89, 107,  33,  16,  25,  94,  26,  17,
        95,  76,  58,  36,  96,  77,  59,  37,  27,  28,
        29,  18,  30,  31,  19,   0,  32,   5,  97,  78,
        60,  38,   1,  79,  61,  39,  40,  23,  24,   8,
        9,  10,  11,   2,  41,  42,  43,  12,  13,  14,
        15,   3,   4,  34,  35,  98,  99, 100, 101, 102,
        103, 104, 105, 106,  80,  81,  82,  83,  84,  85,
        86, 87
    }
};
// Inverse matrix: reordering of FR (channel encoding)
PROTECTED CONST UINT16 g_sppMatrixFrInv[260] =
{
    191,  64,  43,   8,   5,   0, 258, 192,  65,  20,
    9,   6, 198, 193,  66,  10,   7, 200, 199,  68,
    44,  11, 201,  69,  50,  21, 259, 194,  51,  22,
    195,  70,  45, 197, 196,  67,  56,  46,  35,  31,
    27,  23,  12,  71,  52, 131,  60, 202, 135,  75,
    39,  16,   1, 206, 139,  79, 207, 140,  80, 208,
    141,  81, 209, 142,  82, 210, 143,  83, 211, 144,
    84, 212, 145,  85, 213, 146,  86, 214, 147,  87,
    215, 148,  88, 216, 149,  89, 217, 150,  90, 218,
    151,  91,  57,  47,  36,  32,  28,  24,  13,  72,

    53, 132,  61, 203, 136,  76,  40,  17,   2, 219,
    152,  92, 220, 153,  93, 221, 154,  94, 222, 155,
    95, 223, 156,  96, 224, 157,  97, 225, 158,  98,
    226, 159,  99, 227, 160, 100, 228, 161, 101, 229,
    162, 102, 230, 163, 103, 231, 164, 104,  58,  48,
    37,  33,  29,  25,  14,  73,  54, 133,  62, 204,
    137,  77,  41,  18,   3, 232, 165, 105, 233, 166,
    106, 234, 167, 107, 235, 168, 108, 236, 169, 109,
    237, 170, 110, 238, 171, 111, 239, 172, 112, 240,
    173, 113, 241, 174, 114, 242, 175, 115, 243, 176,

    116, 244, 177, 117,  59,  49,  38,  34,  30,  26,
    15,  74,  55, 134,  63, 205, 138,  78,  42,  19,
    4, 245, 178, 118, 246, 179, 119, 247, 180, 120,
    248, 181, 121, 249, 182, 122, 250, 183, 123, 251,
    184, 124, 252, 185, 125, 253, 186, 126, 254, 187,
    127, 255, 188, 128, 256, 189, 129, 257, 190, 130,
};


// Matrix for preparing data for CRC decoding
// (see 3GPP TS 45.003:3.1.1.1)
PROTECTED CONST UINT8 g_sppMatrixEfr1[]=
{
    38,  39,  40,  41,  42,  43,  47,  86,  44,   1,
    2,   7,   9,  17,  18,  23,  45,  46, 141, 142,
    143, 144, 145, 146,  91,  92, 194, 195,  97, 136,
    147,  93, 196, 148, 149,  94, 197,   3,   4,  10,
    11,  15,   8,   5,   6,  12,  16,  19,  95, 198,
    0,  13,  14,  20,  24,  25,  27, 150, 200, 189,
    239,  87, 137, 190, 240,
};

// Matrix for preparing data for CRC encoding
// INVERSE matrix of the table in 3.1.1.1
// (see 3GPP TS 45.003:3.1.1.1)
// ATTENTION: This table takes bits after the reordering,
// ie. dividend[0]=s[38]
PROTECTED CONST UINT8 g_sppMatrixEfr1Inv[65]=
{
    0,   1,   2,   3,   4,   5,  16,  17,  20,  24,
    25,  26,  27,  28,  29,  30,  31,  32,   6,   7,
    8,   9,  10,  11,  12,  13,  14,  15,  18,  19,
    21,  22,  23,  33,  34,  35,  36,  37,  38,  39,
    40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
    50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,
};


// Matrix for reordering of EFR (channel encoding)
// INVERSE matrix of table 6 (45.003 annex)
// Repetition bits not included
// The indices of the table are the indices of s[k]
// The elements are the indices of w[k]
// Example: w[g_sppMatrixEfr2Inv[0]] = s[0],
//          g_sppMatrixEfr2Inv[0] = 50,
//        =>w[50] = s[0]
// (see 3GPP TS 45.003:3.1.1.3)
PROTECTED CONST UINT16 g_sppMatrixEfr2Inv[] =
{
    50,  24,  25,  37,  38,  43,  44,  26,  42,  27,
    39,  40,  45,  51,  52,  41,  46,  28,  29,  47,
    53,  77,  78,  30,  54,  55,  79,  56,  80, 109,
    110, 111, 112, 113, 114, 115, 180, 181,   0,   1,
    2,   3,   4,   5,  20,  31,  32,  16,  73, 105,
    157,  81, 118, 138, 165,  82, 119, 139, 166,  83,
    120, 140, 167,  84, 121, 141, 168,  85, 122, 184,
    196, 200, 220, 240, 201, 221, 241, 202, 222, 242,
    203, 223, 243, 204, 224, 244,  17,  61, 101, 153,
    161,  12,  13,  22,  35,  48, 116,  18,  74, 106,

    158,  86, 123, 142, 169,  87, 124, 143, 170,  88,
    125, 144, 171,  89, 126, 145, 172,  90, 127, 187,
    197, 205, 225, 245, 206, 226, 246, 207, 227, 247,
    208, 228, 248, 209, 229, 249,  19,  62, 102, 154,
    162,   6,   7,   8,   9,  10,  11,  21,  33,  34,
    57,  75, 107, 159,  91, 128, 146, 173,  92, 129,
    147, 174,  93, 130, 148, 175,  94, 131, 149, 176,
    95, 132, 190, 198, 210, 230, 250, 211, 231, 251,
    212, 232, 252, 213, 233, 253, 214, 234, 254,  59,
    63, 103, 155, 163,  14,  15,  23,  36,  49, 117,

    58,  76, 108, 160,  96, 133, 150, 177,  97, 134,
    194, 195,  98, 135, 151, 178,  99, 136, 152, 179,
    100, 137, 193, 199, 215, 235, 255, 216, 236, 256,
    217, 237, 257, 218, 238, 258, 219, 239, 259,  60,
    64, 104, 156, 164,  65,  66,  67,  68,  69,  70,
    71, 72
};

#if (CHIP_VAMOSI_SUPPORTED)

PROTECTED CONST UINT8 g_sppNbFwequStartSt[32] = {0x7,0x7,0xE,0xE,0xB,0xA,0xF,0xC,
                                                 0x7,0x1,0xC,0x0,0xE,0x3,0x5,0x9,
                                                 0x2,0x8,0x6,0xC,0x6,0x0,0x0,0x4,
                                                 0x0,0x0,0x0,0x8,0x0,0x6,0x0,0xE
                                                };




PROTECTED CONST UINT8 g_sppNbBwequStartSt[32] = {0x4,0x4,0x2,0x2,0x8,0x2,0x5,0x7,
                                                 0x6,0xA,0x2,0x4,0xE,0x2,0x8,0x2,
                                                 0x3,0x4,0x3,0xC,0x8,0x3,0xD,0x7,
                                                 0x3,0x6,0x7,0x6,0xB,0xB,0x4,0xA
                                                };
#else
PROTECTED CONST UINT8 g_sppNbFwequStartSt[8] = {0x7,0x7,0xE,0xE,0xB,0xA,0xF,0xC};
PROTECTED CONST UINT8 g_sppNbBwequStartSt[8] = {0x4,0x4,0x2,0x2,0x8,0x2,0x5,0x7};

#endif

// Training sequences
//-------------------
// constant tables for optimized code
// 1101 1000 0110 1110 1010 0010 1011 0100
//    D    8    6    E    A    2    B    4
// 1111 0000 0010 0000 0100 0110 1001 1101
//    F    0    2    0    4    6    9    D
PROTECTED CONST UINT32 g_sppGsmTscSch[2]= {GSM_TSC_SCH0,GSM_TSC_SCH1};
PROTECTED CONST UINT32 g_sppGsmTscAburst[2] = {0x5599FED2, 0x0000003C};
#ifdef CHIP_DIE_8955
PROTECTED CONST UINT32 g_sppGsmTscAburst_1[2] = {0xF4611F2A, 0x00000164};
PROTECTED CONST UINT32 g_sppGsmTscAburst_2[2] = {0xB06AE4F7, 0x000001DD};
#endif
#if (CHIP_VAMOSI_SUPPORTED)
PROTECTED CONST UINT32 g_sppGsmTscNburst[32]=
{
    0x03a443a4, 0x03b47bb4, 0x01C25DC2, 0x01E22DE2,
    0x03582758, 0x01720D72, 0x03E51BE5, 0x00F748F7,
    0x03af2446, 0x021dd97a, 0x00ddc682, 0x002f3bb4,
    0x01f72f2e, 0x033cac82, 0x02bb0b08, 0x0253f3a2,
    0x0115e243, 0x004291f4, 0x01b5df13, 0x00d0650c,
    0x01885d78, 0x0009eaf3, 0x0023f59d, 0x009077a7,
    0x002c4173, 0x003a2846, 0x001ca827, 0x00615f36,
    0x0034421b, 0x01ac5fcb, 0x00354fe4, 0x01c6503a
};
#else
PROTECTED CONST UINT32 g_sppGsmTscNburst[8]=
{
    0x03a443a4, 0x03b47bb4, 0x01C25DC2, 0x01E22DE2,
    0x03582758, 0x01720D72, 0x03E51BE5, 0x00F748F7
};

#endif
#ifdef CHIP_DIE_8955
PROTECTED CONST INT8 g_sppSaicTable[8][9] =
{
    {23, -3, -1, -3, 23, -3, -1, -3, 23},
    {23, -3, -1, -3, 23, -3, -1, -3, 23},
    {23, 3, 1, 3, 23, 1, 1, 1, 23},
    {23, 3, 1, 3, 23, 1, 1, 1, 23},
    {23, -3, 1, -3, 23, -1, 1, -1, 23},
    {23, -1, -1, -1, 23, -1, -1, -1, 23},
    {23, 1, 1, 1, 23, -1, 1, -1, 23},
    {23, 1, 1, 1, 23, 3, 1, 3, 23}
};

PROTECTED CONST INT8 g_sppSaicPMatrixTable[8][3] =
{
    { 1, -1, -3 },
    { 1, -1, -3 },
    { -3, 1, 3 },
    { -3, 1, 3 },
    { 3, 1, 1 },
    { 1, 1, 1 },
    { -3, -1, -1},
    { -1, 1, 1 }
};
#endif
PROTECTED CONST UINT8 g_sppUsfCs2Cs3[8] = {0x00, 0x29, 0x1A, 0x33, 0x34, 0x1D, 0x2E, 0x07};
PROTECTED CONST UINT16 g_spp_UsfCs4[8] = {0x000, 0xD0B, 0x6EC, 0xBE7, 0xBB0, 0x6BB, 0xD5C, 0x057};



// In-band data encoding
//----------------------
PROTECTED CONST UINT16 g_sppIcSidRatscch[4] = {0x530F, 0x3EB8, 0x8863, 0xE5D4};

PROTECTED CONST UINT16 g_sppIcAfsSpeech[4] = {0x00, 0xBA, 0x5D, 0xE7};
PROTECTED CONST UINT16 g_sppIcAhsSpeech[4] = {0x00, 0x09, 0x07, 0x0E};


// Identity marker
//----------------
// 24 times repetition of sequence "0 1111 0010"
PROTECTED CONST UINT32 g_sppImAfsSid[7] =
{
    0x93c9e4f2, 0x3c9e4f27, 0xc9e4f279, 0x9e4f2793,
    0xe4f2793c, 0x4f2793c9, 0x00093c9e,
};
// IM of AHS SID_UPDATE & SID_FIRST_INH
// (Note: the data have been shift by 16 bits)
// 24 times repetition of sequence "1 0000 1101"
PROTECTED CONST UINT32 g_sppImAhsSid[8] =
{
    0x1b0d0000, 0xb0d86c36, 0x0d86c361, 0xd86c361b,
    0x86c361b0, 0x6c361b0d, 0xc361b0d8, 0x00000006,
};
// IM of RATSCCH (Note: the data have been shift by 16 bits)
// 20 times repetition of sequence "100 0110 1001"
PROTECTED CONST UINT32 g_sppImRatscch[8] =
{
    0x4c690000, 0x98d31a63, 0x31a634c6, 0x634c698d,
    0xc698d31a, 0x8d31a634, 0x1a634c69, 0x00000003
};


// Silence Frame markers
//----------------------
PROTECTED CONST UINT32 g_sppAfsSidMarker[7] =
{
    0x93c9e4f2, 0x3c9e4f27, 0xc9e4f279, 0x9e4f2793,
    0xe4f2793c, 0x4f2793c9, 0x00093c9e
};
PROTECTED CONST UINT32 g_sppAhsSidMarker[7] =
{
    0x6c361b0d, 0xc361b0d8, 0x361b0d86, 0x61b0d86c,
    0x1b0d86c3, 0xb0d86c36, 0x0006c361
};
PROTECTED CONST UINT32 g_sppRatscchMarker[7] =
{
    0x1a634c69, 0x34c698d3, 0x698d31a6, 0xd31a634c,
    0xa634c698, 0x4c698d31, 0x00031a63
};



// Convolutional encoder parameters
//---------------------------------
// Numer of words of Convolutional code output (AHS)
PROTECTED CONST UINT8 g_sppConvOutLengthBitAhs[6] = {212, 212, 208, 200, 196, 188};


// AHS BitError calculation parameters
//------------------------------------
PROTECTED CONST UINT32 g_sppConvOutMaskAhs[6] =
{
    0x000FFFFF, 0x000FFFFF, 0x0000FFFF,
    0x000000FF, 0x0000000F, 0x0FFFFFFF
};

#ifdef CHIP_DIE_8955
PROTECTED CONST INT16 g_spp_8TapChestTab[8][8][19]=
{
    {
        {-1878,-1976,-1278,1745,1275,1681,1103,-1595,2347,2470,1597,-2175,1745,1745,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2054,1665,1536,1665,-1567,1728,1882,2084,-2355,1740,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2335,2012,1901,2012,1582,-1444,1613,2372,2077,-2018,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1760,-2012,2193,2083,2513,1444,-1613,1723,2018,2018,-1871,2012,1265,-1155,1290},
        {1398,1300,-1278,1745,-2000,-1595,-2172,1681,2347,2470,1597,-2175,1745,1745,1853,-1681,1398,1300,-1278},
        {-1253,1382,1505,-1740,2041,-1665,-1536,-1665,1567,2367,2213,2011,-1740,2355,1912,1665,-1253,1382,1505},
        {1265,-1155,1290,2018,-1760,2083,-1901,-2012,-1582,1444,2482,1723,2018,-2077,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,1776,-2015,1646,-2015,-2335,-2243,1198,2464,1308,1308,-1886,2015,1868,1794,-958}
    },
    {
        {-1878,-1976,-1278,1745,-1423,-1103,-2003,-1275,2347,2470,1597,-1745,1853,-1917,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2183,-1536,-1493,-2054,-1567,1728,1882,2355,-1912,1837,-1912,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2224,2193,-1865,-1760,-2513,-1444,1613,2018,1871,-1935,1871,-2012,-1265,1155,-1290},
        {-1398,-1300,1278,-1745,-1853,-2172,2003,-2000,-2347,-2470,-1597,1745,1423,1917,-1853,1681,-1398,-1300,1278},
        {1253,-1382,-1505,1740,-1912,-1536,-1493,2041,-1567,-2367,-2213,-1740,2183,1837,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,1871,-1901,-1865,-1760,1582,-1444,-2482,-2077,-2224,2160,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1871,1901,-2230,-2335,-1582,1444,-1613,-2018,-1871,-2160,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,-1886,-1646,2593,-1776,-2335,-2243,1198,-1308,-1886,-1438,-1886,2015,1868,1794,-958}
    },
    {
        {-958,-1794,1868,-2015,-1886,-1308,1308,-2464,1198,2243,-2335,2015,1646,2015,1776,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2077,-2018,1723,-2482,1444,1582,-2012,1901,2083,1760,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,1912,-2355,-1740,-2011,2213,-2367,1567,1665,-1536,1665,2041,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,1745,-1745,-2175,-1597,2470,-2347,1681,2172,-1595,2000,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-2018,2018,-1723,-1613,-1444,2513,-2083,2193,2012,-1760,2077,1290,1155,1265},
        {1290,1155,1265,2083,-1871,-2018,-2077,2372,-1613,-1444,-1582,2012,-1901,2012,2335,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,2183,-1740,-2355,-2084,1882,-1728,-1567,-1665,1536,-1665,2054,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,1745,-1745,-2175,-1597,2470,-2347,-1595,-1103,1681,-1275,1745,1278,-1976,1878}
    },
    {
        {-958,-1794,1868,-2015,-1886,1438,-1886,1308,1198,2243,-2335,1776,2593,1646,-1886,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2160,1871,-2018,1613,1444,1582,-2335,2230,1901,1871,-2077,-1290,-1155,-1265},
        {-1290,-1155,-1265,-2083,1871,-2160,-2224,2077,-2482,1444,1582,1760,-1865,1901,1871,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,-2183,1837,-2183,-1740,2213,-2367,1567,2041,1493,-1536,1912,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,-1917,1423,-1745,-1597,2470,-2347,2000,2003,2172,-1853,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-1935,-1871,2018,-1613,-1444,2513,-1760,1865,2193,2224,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,-1912,-1837,-1912,-2355,1882,-1728,-1567,2054,-1493,1536,2183,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,-1917,-1853,-1745,-1597,2470,-2347,-1275,2003,-1103,1423,1745,1278,-1976,1878}
    },
    {
        {1249,-1384,-1480,-2076,1874,1706,-1874,2076,2331,1528,1336,2221,-1706,-2221,1706,-1874,1249,-1384,-1480},
        {-1269,1153,-1269,-2365,-1903,2047,1903,-1730,1413,2480,1730,1903,2047,-1903,-2047,1903,-1269,1153,-1269},
        {1269,-1153,1269,-1730,-2192,-2047,2192,1730,-1413,1615,2365,2192,2047,1903,-2047,-1903,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,-2192,2365,1413,-1615,1730,1903,2047,2192,2047,-2192,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,-2019,1648,1528,-1394,2221,1706,1874,2389,2221,-1480,-1384,1249},
        {1249,-1384,-1480,2019,-2221,1706,-1874,-2019,-1764,1528,1336,-1874,2389,1874,1706,2221,1249,-1384,-1480},
        {1480,1384,-1249,-2019,2221,-2389,1874,-2076,-1648,-1528,1394,1874,-1706,2221,1706,1874,1480,1384,-1249},
        {1499,1615,1499,-1672,-1846,2047,-2249,1672,-1297,-1442,-1672,1846,2047,-1846,2047,1846,1499,1615,1499}
    },
    {
        {1269,-1153,1269,-1730,-2192,2047,2192,1730,2365,1615,-1413,1903,2047,-1903,-2047,-2192,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,1903,2365,1730,2480,1413,-1903,2047,1903,-2047,-1903,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,2076,1336,1528,2331,1874,-1706,2221,1706,-2221,-1480,-1384,1249},
        {-1499,-1615,-1499,1672,-2249,2047,-1846,-1672,1672,1442,1297,1846,2047,-1846,2047,1846,-1499,-1615,-1499},
        {1249,-1384,-1480,-2076,1874,-2389,2221,-2019,-1394,1528,1648,1874,1706,2221,-1706,1874,1249,-1384,-1480},
        {1480,1384,-1249,-2019,-1874,1706,-2221,2019,-1336,-1528,1764,2221,1706,1874,2389,-1874,1480,1384,-1249},
        {-1249,1384,1480,-2019,-1874,-1706,1874,-2076,1394,-1528,-1648,2221,2389,1874,1706,2221,-1249,1384,1480},
        {1269,-1153,1269,2365,-2192,-2047,-1903,1730,-1730,1615,-1413,-2192,2047,2192,2047,1903,1269,-1153,1269}
    },
    {
        {-1269,-1269,-1153,2192,-2047,-1903,2047,1903,1413,-2365,1615,-1730,2192,1730,-2192,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,2047,-1903,-2047,1903,1413,1730,-2480,2365,-1903,1730,1903,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,-2047,2192,-2047,-2192,1413,1730,1615,-1730,2192,-2365,1903,2047,-1269,-1269,-1153},
        {1480,-1249,-1384,-1874,-1706,-1874,1706,-2221,-1648,1394,1528,2019,-1874,2076,-2221,2389,1480,-1249,-1384},
        {1499,1499,-1615,-1846,-2047,-1846,-2047,1846,-1297,-1672,1442,1672,2249,-1672,1846,-2047,1499,1499,-1615},
        {-1480,1249,1384,-2221,-2389,-2221,-1706,-1874,1648,-1394,-1528,2076,1874,2019,-1874,1706,-1480,1249,1384},
        {1249,-1480,1384,1874,-1706,-2221,-2389,-1874,-1764,1336,-1528,-2019,1874,2019,2221,-1706,1249,-1480,1384},
        {-1249,1480,-1384,2221,1706,-1874,-1706,-2221,-2331,-1336,1528,-2076,-1874,2076,1874,1706,-1249,1480,-1384}
    },
    {
        {-1480,1384,1249,1706,-1874,2389,1874,-1874,1648,-1528,-1394,-2221,2076,-2221,-2019,-1706,-1480,1384,1249},
        {-1249,-1384,1480,1706,1874,-1706,2221,1874,-2331,1528,-1336,-1874,-2076,2221,-2076,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,1903,2047,-1903,1903,1413,-2480,1730,-1903,-1730,-1903,2365,-2047,-1269,-1153,-1269},
        {-1269,-1153,-1269,-2047,1903,2047,2192,-2192,1413,1615,-2365,2192,-1730,-1903,-1730,2047,-1269,-1153,-1269},
        {1480,-1384,-1249,-1706,-2221,1706,2221,1874,-1648,1528,1394,-1874,2019,-1874,-2076,-2389,1480,-1384,-1249},
        {-1499,1615,-1499,-2047,-1846,-2047,1846,2249,1297,-1442,1672,1846,-1672,1846,-1672,-2047,-1499,1615,-1499},
        {-1249,-1384,1480,-2389,-2221,-1706,-1874,1874,1764,1528,-1336,2221,2019,-1874,2019,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,-2192,-2047,-1903,-2192,1413,1615,1730,-1903,2365,2192,-1730,2047,-1269,-1153,-1269}
    }
};
#endif


#if (CHIP_CHEST_LEN == 5)

#if (CHIP_VAMOSI_SUPPORTED)

//  channel estimation matrix
PROTECTED CONST INT16 g_sppChestTab[33][5][22] =
{
    //SET1:0-7
    {
        {1513,-1304,1252,-1513,-1252,-1721,1826,1513,1460,1200,-1565,1513,1513,1721,-1826,1513,1513,-1304,1252,-1513,-1252,-1721},
        {1103,1434,-1151,1155,-1673,-1382,-2008,1668,1952,1956,1439,-1720,1103,1382,2008,-1720,1103,1434,-1151,1155,-1673,-1382},
        {-1155,1434,1673,-1103,1151,-1382,-2008,-1720,1387,1956,2004,1668,-1155,1382,2008,1668,-1155,1434,1673,-1103,1151,-1382},
        {1281,-1017,1586,1542,-1021,1278,-1852,-2107,-1538,1591,2112,1846,1281,-1278,1852,1846,1281,-1017,1586,1542,-1021,1278},
        {1542,1017,-1021,1281,1586,-1278,1852,-1846,-1851,-1591,1277,2107,1542,1278,-1852,2107,1542,1017,-1021,1281,1586,-1278}
    },
    {
        {-1513,-1252,1304,-1252,-1513,-1721,1826,-1513,-1513,-1721,-1200,1565,1513,1460,-1826,1513,-1513,-1252,1304,-1252,-1513,-1721},
        {1155,-1673,-1434,1151,-1103,-1382,-2008,1720,-1103,-1382,-1956,-1439,1668,1952,2008,-1720,1155,-1673,-1434,1151,-1103,-1382},
        {-1103,1151,-1434,-1673,1155,-1382,-2008,-1668,1155,-1382,-1956,-2004,-1720,1387,2008,1668,-1103,1151,-1434,-1673,1155,-1382},
        {1542,-1021,1017,-1586,-1281,1278,-1852,-1846,-1281,1278,-1591,-2112,-2107,-1538,1852,1846,1542,-1021,1017,-1586,-1281,1278},
        {1281,1586,-1017,1021,-1542,-1278,1852,-2107,-1542,-1278,1591,-1277,-1846,-1851,-1852,2107,1281,1586,-1017,1021,-1542,-1278}
    },
    {
        {1278,1586,-1281,-1021,-1017,1542,-2107,-1852,-1278,1542,-2107,1277,1591,-1851,1846,1852,1278,1586,-1281,-1021,-1017,1542},
        {1278,1021,1542,-1586,-1017,-1281,1846,-1852,-1278,-1281,1846,-2112,1591,1538,-2107,1852,1278,1021,1542,-1586,-1017,-1281},
        {1382,1151,1103,1673,-1434,-1155,-1668,2008,-1382,-1155,-1668,2004,-1956,1387,1720,-2008,1382,1151,1103,1673,-1434,-1155},
        {-1382,1673,1155,1151,1434,-1103,-1720,-2008,1382,-1103,-1720,-1439,1956,-1952,1668,2008,-1382,1673,1155,1151,1434,-1103},
        {1721,-1252,1513,1252,1304,1513,-1513,-1826,-1721,1513,-1513,-1565,-1200,1460,-1513,1826,1721,-1252,1513,1252,1304,1513}
    },
    {
        {1278,-1542,-1021,-1017,-1586,1281,-2107,-1852,1851,-1846,1277,1591,1278,-1542,2107,1852,1278,-1542,-1021,-1017,-1586,1281},
        {1278,1281,-1586,-1017,-1021,-1542,1846,-1852,-1538,2107,-2112,1591,1278,1281,-1846,1852,1278,1281,-1586,-1017,-1021,-1542},
        {1382,1155,1673,-1434,-1151,-1103,-1668,2008,-1387,-1720,2004,-1956,1382,1155,1668,-2008,1382,1155,1673,-1434,-1151,-1103},
        {-1382,1103,1151,1434,-1673,-1155,-1720,-2008,1952,-1668,-1439,1956,-1382,1103,1720,2008,-1382,1103,1151,1434,-1673,-1155},
        {1721,-1513,1252,1304,1252,-1513,-1513,-1826,-1460,1513,-1565,-1200,1721,-1513,1513,1826,1721,-1513,1252,1304,1252,-1513}
    },
    {
        {-1490,1486,-1239,1234,-1538,-1496,-1852,1481,1490,-1486,1239,1800,1852,1553,1858,-1176,-1490,1486,-1239,1234,-1538,-1496},
        {-1256,-1517,1256,-1308,994,-1308,-2146,-1570,1256,1517,-1256,1622,2146,1884,2198,1884,-1256,-1517,1256,-1308,994,-1308},
        {1408,-1643,-1408,1125,-1182,1125,-1758,-1926,-1408,1643,1408,-1067,1758,1983,2041,1983,1408,-1643,-1408,1125,-1182,1125},
        {1617,1339,-1617,-1387,1381,-1387,1329,-1664,-1617,-1339,1617,1392,-1329,1669,1674,1669,1617,1339,-1617,-1387,1381,-1387},
        {1239,1486,1490,-1496,-1538,1234,-1852,1481,-1239,-1486,-1490,1800,1852,-1176,1858,1553,1239,1486,1490,-1496,-1538,1234}
    },
    {
        {-1516,-1516,-1516,1213,-1516,1213,-1213,-1820,1516,1516,1516,1820,1820,-1213,1516,1820,-1516,-1516,-1516,1213,-1516,1213},
        {1213,-1516,-1516,-1516,1213,-1516,1516,-1820,-1213,1516,1516,1820,1820,1516,-1213,1820,1213,-1516,-1516,-1516,1213,-1516},
        {1638,1365,-1365,-1365,-1638,1365,-1638,1638,-1638,-1365,1365,1365,1638,1638,1638,-1365,1638,1365,-1365,-1365,-1638,1365},
        {-1516,1213,1213,-1516,-1516,-1516,1516,-1820,1516,-1213,-1213,1820,1820,1516,1516,1820,-1516,1213,1213,-1516,-1516,-1516},
        {1638,-1365,1365,1365,-1638,-1365,-1638,1638,-1638,1365,-1365,-1365,1638,1638,1638,1365,1638,-1365,1365,1365,-1638,-1365}
    },
    {
        {991,-1565,-1513,-1304,-1043,-1304,1826,-1878,-1252,1565,1513,1304,-2087,1878,-1878,2139,991,-1565,-1513,-1304,-1043,-1304},
        {1456,1652,-1360,-1126,-1173,-1126,-1700,1982,-1408,-1652,1360,1126,1747,-1982,1982,-2030,1456,1652,-1360,-1126,-1173,-1126},
        {-1621,1339,1613,-1387,-1382,-1387,-1334,-1669,1617,-1339,-1613,1387,1330,1669,-1669,1674,-1621,1339,1613,-1387,-1382,-1387},
        {1282,-1234,1491,1747,-1278,-1529,-1790,-1481,-1533,1234,-1491,-1747,1539,1481,1794,-1543,1282,-1234,1491,1747,-1278,-1529},
        {-1282,1234,-1491,1529,1278,-1747,-1486,-1794,-1742,-1234,1491,-1529,-1539,1794,1481,1543,-1282,1234,-1491,1529,1278,-1747}
    },
    {
        {-1638,-1365,-1365,-1365,1638,1365,1365,-1638,1638,1365,-1638,1638,-1638,-1365,-1365,1638,-1638,-1365,-1365,-1365,1638,1365},
        {1638,-1365,-1365,-1365,-1638,1365,1365,1638,-1638,1365,1638,-1638,1638,-1365,-1365,-1638,1638,-1365,-1365,-1365,-1638,1365},
        {-1260,1764,-1512,-1512,-1260,-1260,1764,1512,1260,-1764,1512,1512,-1764,1512,-1512,-1512,-1260,1764,-1512,-1512,-1260,-1260},
        {-1260,-1512,1764,-1512,-1260,-1260,-1512,1512,1260,1512,-1764,1512,1512,-1764,1764,-1512,-1260,-1512,1764,-1512,-1260,-1260},
        {-1260,-1512,-1512,1764,-1260,-1260,-1512,-1764,1260,1512,1512,-1764,1512,1512,-1512,1764,-1260,-1512,-1512,1764,-1260,-1260}
    },
    //SET1:8-15
    {
        {1329,1850,-1286,1180,1276,1276,-1286,1180,1276,-1807,1754,1276,-1807,-1329,-1850,-1797,1861,-1180,1807,-1233,-1850,-1276},
        {1148,1664,1903,-1340,1430,1430,1903,-1340,1430,1387,-1106,1430,1387,-1148,-1664,-1946,-1669,1340,-1387,1621,-1664,-1430},
        {-1435,1333,1717,1675,-1105,1626,1717,1675,-1105,1679,1382,-1105,1679,1435,-1333,-1664,-2009,-1675,1052,-1344,1397,-1626},
        {-1435,-1397,1717,1675,1626,-1105,1717,1675,1626,-1052,1382,1626,-1052,1435,1397,-1664,-2009,-1675,-1679,1387,-1333,1105},
        {882,-1983,-1116,1701,1175,1175,-1116,1701,1175,1749,-1457,1175,1749,-882,1983,1690,-2041,-1701,-1749,-1409,1983,-1175}
    },
    {
        {-1214,-1517,-1517,1517,-1820,1820,1214,-1517,-1214,1517,-1820,-1214,-1517,1517,-1820,-1214,-1517,1517,1214,1517,1517,-1517},
        {-1517,-1214,-1214,-1517,1820,-1820,1517,1517,-1517,-1517,1820,-1517,-1214,-1517,1820,-1517,-1214,-1517,1517,1214,1214,1517},
        {1517,-1517,-1517,-1214,-1820,1820,-1517,1214,1517,-1214,-1820,1517,-1517,-1214,-1820,1517,-1517,-1214,-1517,1517,1517,1214},
        {-1365,1365,-1365,-1638,-1638,-1638,1365,-1365,1365,1638,-1365,-1638,1365,-1365,-1638,-1638,1365,-1365,-1638,-1638,1365,1638},
        {1365,-1365,1365,-1638,-1638,-1638,-1365,1365,-1365,1638,1365,-1638,-1365,1365,-1638,-1638,-1365,1365,-1638,-1638,-1365,1638}
    },
    {
        {1175,1983,1409,-1749,1701,-2041,-1690,1983,882,1749,-1175,-1457,-1749,1175,-1701,-1116,-1175,1175,-1701,-1116,1983,882},
        {1105,1333,1387,1679,-1675,2009,-1664,-1397,1435,1052,1626,-1382,-1052,-1626,1675,-1717,-1105,-1626,1675,-1717,-1397,1435},
        {1626,1397,1344,1052,1675,-2009,1664,-1333,-1435,1679,1105,1382,-1679,-1105,-1675,1717,-1626,-1105,-1675,1717,-1333,-1435},
        {-1430,1664,1621,1387,1340,1669,-1946,1664,-1148,-1387,1430,1106,1387,-1430,-1340,-1903,1430,-1430,-1340,-1903,1664,-1148},
        {1276,-1850,1233,1807,1180,1861,1797,-1850,1329,-1807,-1276,1754,1807,1276,-1180,-1286,-1276,1276,-1180,-1286,-1850,1329}
    },
    {
        {-1638,-1365,1638,-1638,-1365,-1365,1638,-1638,-1365,-1365,1638,1365,-1638,-1365,-1365,-1365,1638,-1638,1638,1365,1365,1365},
        {1638,-1365,-1638,1638,-1365,-1365,-1638,1638,-1365,-1365,-1638,1365,1638,-1365,-1365,-1365,-1638,1638,-1638,1365,1365,1365},
        {-1214,1517,-1517,-1517,1214,-1517,-1517,-1820,1214,-1517,-1517,-1820,1214,1517,-1214,-1517,-1820,-1820,1214,-1517,1517,1517},
        {1517,-1214,1214,-1517,-1517,1214,-1517,-1820,-1517,1214,-1517,-1820,-1517,1517,1517,-1517,-1820,-1820,-1517,1214,-1214,1517},
        {1517,1517,-1517,1214,-1517,-1517,1214,-1820,-1517,-1517,1214,-1820,-1517,-1214,1517,1214,-1820,-1820,-1517,-1517,1517,-1214}
    },
    {
        {1638,-1638,1365,1365,-1365,-1638,-1365,-1365,1365,-1638,1365,1365,-1365,-1638,-1365,1638,-1638,-1638,-1638,-1365,-1638,1365},
        {-1260,1764,-1512,1512,1512,-1764,-1512,-1260,-1260,1764,-1512,1512,1512,-1764,-1512,-1260,1764,-1512,-1512,-1260,-1260,-1260},
        {-1260,-1512,1764,-1764,1512,1512,-1512,-1260,-1260,-1512,1764,-1764,1512,1512,-1512,-1260,-1512,1764,-1512,-1260,-1260,-1260},
        {-1260,-1512,-1512,1512,-1764,1512,1764,-1260,-1260,-1512,-1512,1512,-1764,1512,1764,-1260,-1512,-1512,1764,-1260,-1260,-1260},
        {1638,-1638,-1365,-1365,1365,-1638,1365,1365,-1365,-1638,-1365,-1365,1365,-1638,1365,1638,-1638,-1638,-1638,1365,-1638,-1365}
    },
    {
        {1638,1638,1638,-1365,1638,1365,-1365,-1365,1365,-1638,1638,-1638,1638,1365,-1365,-1365,-1638,-1638,1365,1365,-1365,-1365},
        {1260,1764,1512,1512,-1512,1512,1260,-1260,-1512,1260,-1512,1764,-1764,1764,1260,-1260,-1512,-1764,-1512,1512,1512,-1260},
        {1638,1638,1638,1365,1638,-1365,1365,1365,-1365,-1638,1638,-1638,1638,-1365,1365,1365,-1638,-1638,-1365,-1365,1365,1365},
        {-1260,1512,1764,1764,1512,1764,-1260,1260,1512,-1260,-1764,1512,-1512,1512,-1260,1260,1512,-1512,-1764,-1512,-1512,1260},
        {1260,-1512,1512,1512,1764,1512,1260,-1260,1764,1260,-1512,-1512,1512,-1512,1260,-1260,1764,1512,-1512,-1764,-1764,-1260}
    },
    {
        {1260,1764,1512,1512,-1512,-1764,1512,-1260,1260,1764,1512,1512,-1512,-1764,1512,-1260,-1764,-1512,1512,-1260,1260,-1260},
        {-1260,1512,1764,1764,1512,-1512,-1512,1260,-1260,1512,1764,1764,1512,-1512,-1512,1260,-1512,-1764,-1512,1260,-1260,1260},
        {1260,-1512,1512,1512,1764,1512,-1764,-1260,1260,-1512,1512,1512,1764,1512,-1764,-1260,1512,-1512,-1764,-1260,1260,-1260},
        {1365,1365,-1638,1638,1365,1365,1365,-1638,-1365,1638,-1638,1638,1365,1365,1365,-1638,-1365,1638,-1638,-1365,-1365,1638},
        {1365,1365,1638,-1638,1365,1365,1365,1638,-1365,-1638,1638,-1638,1365,1365,1365,1638,-1365,-1638,1638,-1365,-1365,-1638}
    },
    {
        {1365,-1365,1638,-1365,-1638,-1365,1365,1365,-1638,-1365,-1365,-1638,-1638,-1638,1365,1365,-1638,1638,-1365,1365,1638,-1638},
        {1517,1214,-1517,1820,-1517,-1517,-1214,1517,1214,-1820,-1214,-1214,-1517,-1517,-1517,1517,1214,-1820,1820,-1517,1517,1517},
        {1214,1517,1517,-1820,1517,-1214,-1517,-1517,1517,1820,-1517,-1517,-1214,-1214,-1214,-1517,1517,1820,-1820,1517,-1517,1214},
        {-1365,1365,1638,1365,-1638,1365,-1365,-1365,-1638,1365,1365,-1638,-1638,-1638,-1365,-1365,-1638,1638,1365,-1365,1638,-1638},
        {1517,-1517,1214,1820,1214,-1517,1517,-1214,-1517,-1820,1517,1517,-1517,-1517,-1517,-1214,-1517,-1820,1820,1214,-1214,1517}
    },
    //SET2:16-23
    {
        {1236,1495,-1535,1319,2178,-1794,1215,2178,1339,-1638,-1815,-1236,-1495,1535,-1319,956,-1059,1059,2074,1339,-1638,1319},
        {1495,1236,1535,-1319,1918,1794,-1898,1918,2074,956,-1599,-1495,-1236,-1535,1319,-1638,1059,-1059,1339,2074,956,-1319},
        {1387,1344,1621,1146,-2070,1664,1447,-2070,1085,1923,1725,-1387,-1344,-1621,-1146,1491,-1189,1189,-1768,1085,1923,1146},
        {-1513,1513,1210,1729,1513,-1816,1772,1513,-1556,1254,1470,1513,-1513,-1210,-1729,-1254,1297,-1297,1556,-1556,1254,1729},
        {-1344,-1387,1621,1146,2026,1664,-1966,2026,1768,-1491,1042,1344,1387,-1621,-1146,-1923,-1189,1189,-1085,1768,-1491,1146}
    },
    {
        {-1201,-1529,-1311,-874,-1311,1857,1529,874,-2294,1638,1966,-1857,1201,-1201,1638,1529,1311,874,-1857,1638,1966,1311},
        {1254,-1254,-1559,-1322,-994,-1322,1819,1887,1650,-2147,927,1887,-1819,1254,-1582,1254,1559,1322,1322,-2147,927,1559},
        {-1446,1446,-859,-1797,-1141,-1797,-1729,1514,2452,1073,-2102,1514,1729,-1446,791,-1446,859,1797,1797,1073,-2102,859},
        {1130,-1130,1695,-1412,-1412,-1412,-1977,-1695,1412,1977,1130,-1695,1977,1130,-1130,1130,-1695,1412,1412,1977,1130,-1695},
        {1529,1201,-1311,1857,-1311,-874,-1201,-1857,-2294,1638,1966,874,-1529,1529,1638,-1201,1311,-1857,874,1638,1966,1311}
    },
    {
        {-995,1256,1309,1832,-1570,-2198,-2146,-1570,-1256,1884,-942,-1309,-1832,1570,-942,1832,-1518,-1623,1309,-1256,-1309,1309},
        {1539,-1240,1496,1549,1481,-1858,-1853,-1795,-1491,-1177,1544,-1496,-1549,-1481,1544,-1182,1487,-1801,-1234,1240,-1496,-1234},
        {1382,1617,-1387,1335,1665,1675,-1330,-1612,-1617,-1670,-1623,1387,-1335,-1665,-1623,1335,-1340,1392,-1387,-1617,1387,-1387},
        {-1183,1408,1125,-1701,1926,2041,1759,-1351,-1408,-1984,-1466,-1125,1701,-1926,-1466,-1701,1644,-1068,1125,-1408,-1125,1125},
        {-1539,-1491,1234,1182,-1481,1858,1853,1795,-1240,-1554,-1544,-1234,-1182,1481,-1544,-1549,-1487,1801,-1496,1491,-1234,-1496}
    },
    {
        {1517,1517,1517,1517,-1214,1820,-1517,1820,1214,-1214,-1517,1517,1517,1517,1517,1820,-1214,1820,-1517,-1214,1214,1517},
        {-1365,1365,1365,1638,1638,-1365,1638,-1638,1638,1365,-1365,-1365,1365,1365,1638,1638,1638,-1365,1638,-1638,-1365,1365},
        {-1214,-1214,1517,1517,1517,1820,-1517,1820,-1517,1517,1214,-1214,-1214,1517,1517,1820,1517,1820,-1517,1517,-1517,-1214},
        {1365,-1365,-1365,1638,1638,1365,1638,-1638,1638,-1365,1365,1365,-1365,-1365,1638,1638,1638,1365,1638,-1638,1365,-1365},
        {1517,1517,-1214,-1214,1517,1820,1214,1820,-1517,1517,-1517,1517,1517,-1214,-1214,1820,1517,1820,1214,1517,-1517,1517}
    },
    {
        {-1388,-1109,-1680,1445,-1720,1165,-1148,-1960,-1092,1428,-1720,1165,1960,1092,1680,-1445,1720,1943,1092,-1428,-1388,2000},
        {-1412,-1243,-847,-1864,1469,-2034,1073,-1017,-2203,-904,1469,-2034,1017,2203,847,1864,-1469,1977,2203,904,-1412,-1299},
        {1485,-1631,-1651,-1316,-1206,1800,-1185,1465,-1016,-1931,-1206,1800,-1465,1016,1651,1316,1206,-2080,1016,1931,1485,-1910},
        {1300,1280,-1875,-905,-1872,-884,1283,-1855,1678,-1303,-1872,-884,1855,-1678,1875,905,1872,1456,-1678,1303,1300,1852},
        {716,1331,1729,-1676,-699,-2291,-1098,1114,-2058,1712,-699,-2291,-1114,2058,-1729,1676,699,2274,2058,-1712,716,1314}
    },
    {
        {-2074,-1780,-1179,-1224,2271,-1232,1428,-1186,1186,-1186,-2067,-937,-1224,2271,2022,-2074,1474,1825,937,1224,982,982},
        {1700,-2033,-1451,-877,-1459,1950,-1708,1375,-1375,1375,-1126,-1783,-877,-1459,1700,1700,-2282,1459,1783,877,1209,1209},
        {2022,2316,-1179,-1224,-1825,-1232,1428,-1186,1186,-1186,2029,-937,-1224,-1825,-2074,2022,1474,-2271,937,1224,982,982},
        {-1799,1859,1753,-1330,-1224,-1844,-1179,1239,-1239,1239,-1285,1814,-1330,-1224,-1799,-1799,1904,1224,-1814,1330,1270,1270},
        {-1330,-1723,1572,1632,-1663,-1088,-1904,-1149,1149,-1149,1391,-1481,1632,-1663,-1330,-1330,-1965,1663,1481,-1632,1421,1421}
    },
    {
        {-734,1559,1595,-1875,-947,1875,-2309,1993,-1346,-1910,-734,-1697,-1049,-1049,2207,1595,1381,-1559,1661,1263,734,1697},
        {-1247,-1447,1372,1454,-1776,-1454,2388,-2381,1977,-1365,-1247,-835,-1239,-1239,-1851,1372,842,1447,-1984,1769,1247,835},
        {-1336,-1256,-1776,1559,1786,-1559,-1572,1876,-1866,2080,-1336,-1043,-1033,-1033,-1247,-1776,1346,1256,1563,-2089,1336,1043},
        {1544,-1234,-1183,-1802,1241,1802,-1556,-1479,1537,-1852,1544,-1549,-1491,-1491,-1176,-1183,-1486,1234,1498,1794,-1544,1549},
        {-1469,1074,-1655,-1016,-1479,1016,2126,-2068,-1065,1713,-1469,1722,-1411,-1411,-2059,-1655,-1664,-1074,1007,1421,1469,-1722}
    },
    {
        {1502,-1607,1229,-1890,-841,-2000,-901,1830,-1667,-1279,-2000,2268,1502,1562,901,1339,-1830,1667,1279,-1169,1229,1279},
        {1465,1782,-1465,1585,-1585,-1126,-1574,-1574,1793,-1257,-1126,-1902,1465,1454,1574,1246,1574,-1793,1257,1454,-1465,1257},
        {-1361,1585,1361,-1656,1656,-1705,-1350,-1350,-1421,1596,-1705,-1290,-1361,1645,1350,1410,1350,1421,-1596,1645,1361,-1596},
        {-1415,-1049,1415,1831,-1831,2372,-1536,-1536,-754,-1170,2372,-2197,-1415,-1711,1536,874,1536,754,1170,-1711,1415,1170},
        {-1229,-1607,-1502,841,1890,-2000,1830,-901,-1667,-1279,-2000,2268,-1229,-1169,-1830,1339,901,1667,1279,1562,-1502,1279}
    },
    //SET3:24-31
    {
        {-1648,-1365,-1412,1648,-1365,1695,1695,1083,1412,1083,-1977,1365,1365,1083,-1648,-1695,1648,-1695,1695,1695,1083,1412},
        {1412,-1638,-1130,-1412,1638,-1921,1356,1412,1130,1412,1695,-1638,1638,1412,1412,-1356,-1412,1921,-1921,1356,1412,1130},
        {1412,1638,-1130,-1412,-1638,1356,-1921,1412,1130,1412,1695,1638,-1638,1412,1412,1921,-1412,-1356,1356,-1921,1412,1130},
        {-1489,1489,1489,-1489,-1489,-1489,1489,-1489,1489,1489,1489,1489,1489,-1489,1489,1489,1489,-1489,-1489,1489,-1489,1489},
        {-1083,-1365,1412,1083,-1365,-1695,-1695,1648,-1412,1648,1977,1365,1365,1648,-1083,1695,1083,1695,-1695,-1695,1648,-1412}
    },
    {
        {1124,861,-1620,1944,2071,500,1809,-1981,1944,-1357,997,2432,500,-1620,1944,-1357,-2432,-500,1620,1485,861,1809},
        {1993,1169,1312,-1575,1640,2083,816,2227,-1575,2137,-1222,726,2083,1312,-1575,2137,-726,-2083,-1312,1079,1169,816},
        {-1694,1333,1784,1136,-1866,1398,1161,1850,1136,-1243,1308,-1932,1398,1784,1136,-1243,1932,-1398,-1784,-1759,1333,1161},
        {-1199,-1487,846,2262,2081,-1371,1793,962,2262,1134,-1018,1965,-1371,846,2262,1134,-1965,1371,-846,-1314,-1487,1793},
        {1822,-1384,-907,1089,1938,1888,-1268,2364,1089,2299,973,-1334,1888,-907,1089,2299,1334,-1888,907,-1450,-1384,-1268}
    },
    {
        {1543,-2167,1602,1852,967,1287,1282,-1858,1597,-1288,1282,-1282,1282,1857,-2172,-1538,-973,1853,1543,973,1287,1282},
        {1282,1602,-2167,1538,1857,973,1543,1858,-1597,1853,-1282,1282,-1282,967,2172,-1852,-1287,-1288,1282,1287,973,1543},
        {-1670,2009,1381,-2004,1157,1386,1105,1675,1099,-1433,1152,-1152,1152,-1722,1727,1951,-1439,-1957,-1670,1439,1386,1105},
        {-1466,-1832,1832,1518,-1570,1518,1466,1152,1780,1204,-1256,1256,-1256,1570,-1884,1518,1518,-1204,-1466,-1518,1518,1466},
        {-1105,-1381,-2009,1951,1722,-1439,1670,1675,1099,1957,1152,-1152,1152,-1157,1727,-2004,1386,1433,-1105,-1386,-1439,1670}
    },
    {
        {-1764,-1512,1764,1260,-1764,-1260,-1260,-1512,-1512,1512,-1764,1512,-1512,1512,1512,1260,1512,-1512,-1260,1764,1260,1260},
        {1512,-1512,-1512,1260,1512,-1260,-1260,-1512,-1512,-1764,1512,-1764,1764,-1764,1512,1260,1512,1764,-1260,-1512,1260,1260},
        {-1365,1365,-1638,-1365,1365,1365,-1365,-1365,-1638,-1638,-1638,1365,-1638,1638,-1638,1638,1365,1638,1638,-1365,-1365,1365},
        {-1512,-1764,1512,-1260,-1512,1260,1260,-1764,-1764,-1512,-1512,-1512,1512,-1512,1764,-1260,1764,1512,1260,1512,-1260,-1260},
        {1365,-1365,-1638,1365,-1365,-1365,1365,1365,-1638,-1638,-1638,-1365,-1638,1638,-1638,1638,-1365,1638,1638,1365,1365,-1365}
    },
    {
        {-1125,1644,1408,1408,1125,-1926,1759,1125,1466,1125,-1926,1759,1125,1466,-1984,1759,-1984,-1068,1644,1408,1408,1125},
        {-1309,-1518,1256,1256,1309,1570,-2146,1309,942,1309,1570,-2146,1309,942,1884,-2146,1884,-1623,-1518,1256,1256,1309},
        {1496,-1487,-1491,1240,1234,1481,1853,-1496,1544,1234,1481,1853,-1496,1544,1177,1853,-1554,1801,-1487,-1491,1240,1234},
        {-1387,1340,-1617,-1617,1387,1665,1330,1387,-1623,1387,1665,1330,1387,-1623,1670,1330,1670,-1392,1340,-1617,-1617,1387},
        {-1234,-1487,1240,-1491,-1496,1481,1853,1234,1544,-1496,1481,1853,1234,1544,-1554,1853,1177,1801,-1487,1240,-1491,-1496}
    },
    {
        {1201,1311,-1311,-2294,-1201,-874,-1529,-1529,-1529,1638,-1529,1201,1311,1857,-2294,-1638,1966,-874,1201,-1857,-1311,1311},
        {-1130,1412,1695,-1695,-1977,-1412,-1130,-1130,-1130,-1130,1977,-1130,1412,1695,1412,-1977,-1977,1695,-1130,1412,-1412,-1695},
        {1529,-1311,1311,2294,-1529,-1857,-1201,-1201,-1201,-1638,-1201,1529,-1311,874,2294,1638,-1966,-1857,1529,-874,1311,-1311},
        {-1254,994,-1559,2215,1819,-1322,-1254,-1254,-1254,-1582,-1819,-1254,994,-1887,1650,2147,1492,-1887,-1254,1322,-994,1559},
        {-1446,-1141,859,-2169,1729,1797,-1446,-1446,-1446,-791,-1729,-1446,-1141,1514,-2452,1073,2384,1514,-1446,-1797,1141,-859}
    },
    {
        {1725,-1957,-1721,-1099,-1104,-1156,-1156,-1156,1952,1669,-2009,1387,-1383,1435,-1435,1435,-1435,-1673,1957,1721,1099,1104},
        {1099,1957,-1669,-1725,-1156,-1104,-1104,-1104,-1387,1721,2009,-1952,1383,-1435,1435,-1435,1435,-1152,-1957,1669,1725,1156},
        {-1848,1591,2108,-1283,-1543,-1282,-1282,-1282,-1851,-1847,1852,1538,-1278,1017,-1017,1017,-1017,1587,-1591,-2108,1283,1543},
        {1283,-1591,1847,1848,-1282,-1543,-1543,-1543,-1538,-2108,-1852,1851,1278,-1017,1017,-1017,1017,-1022,1591,-1847,-1848,1282},
        {1774,1200,-1513,1774,1513,-1513,-1513,-1513,-1461,-1513,-1826,-1461,1722,1304,-1304,1304,-1304,1252,-1200,1513,-1774,-1513}
    },
    {
        {-1692,-960,1336,800,1161,2053,1852,1852,-1537,2588,-1898,1697,1692,-2428,-599,2227,599,1161,-1336,-800,-1161,1336},
        {-1180,-1582,-1226,1318,628,490,1444,1444,2180,-2055,2871,-1917,1180,2318,-2272,-1364,2272,628,1226,-1318,-628,-1226},
        {1742,-1412,-1051,-1554,1601,1412,1224,1224,863,1915,-2291,2103,-1742,1051,1742,-1240,-1742,1601,1051,1554,-1601,-1051},
        {-2109,1356,-1029,-1130,-1318,1921,1883,1883,992,2021,1180,-1218,2109,-2247,1168,2210,-1168,-1318,1029,1130,1318,-1029},
        {1168,-1921,1155,-1130,-1318,-1356,1883,1883,2084,929,2272,967,-1168,2122,-2109,1117,2109,-1318,-1155,1130,1318,1155}
    },
    //SCH
    {
        {2520,1680,540,1680,1680,-1800,-1380,-540,-540,1800,1380,-2940,960,-960,-1380,1800,-960,960,-2100,2100,1380,1680},
        {825,1492,1347,927,927,1347,-1810,-1912,-1347,-1347,1810,2332,-1810,1245,-1245,-1912,1810,-1245,1665,-1665,1245,1492},
        {-2303,724,1846,1006,1006,1846,1745,-1564,-1846,-1846,-1745,2404,1745,-1463,1463,-1564,-1745,1463,-622,622,-1463,724},
        {1129,-1694,1412,1412,1412,1412,1977,1694,-1412,-1412,-1977,-1694,1977,1129,-1129,1694,-1977,-1129,1129,-1129,1129,-1694},
        {2520,1680,-1800,1680,1680,540,960,1800,1800,-540,-960,-2940,-1380,1380,960,-540,1380,-1380,-2100,2100,-960,1680}
    }

};

#else
//  channel estimation matrix
PROTECTED CONST INT16 g_sppChestTab[9][5][22] =
{
    {
        {1513,-1304,1252,-1513,-1252,-1721,1826,1513,1460,1200,-1565,1513,1513,1721,-1826,1513,1513,-1304,1252,-1513,-1252,-1721},
        {1103,1434,-1151,1155,-1673,-1382,-2008,1668,1952,1956,1439,-1720,1103,1382,2008,-1720,1103,1434,-1151,1155,-1673,-1382},
        {-1155,1434,1673,-1103,1151,-1382,-2008,-1720,1387,1956,2004,1668,-1155,1382,2008,1668,-1155,1434,1673,-1103,1151,-1382},
        {1281,-1017,1586,1542,-1021,1278,-1852,-2107,-1538,1591,2112,1846,1281,-1278,1852,1846,1281,-1017,1586,1542,-1021,1278},
        {1542,1017,-1021,1281,1586,-1278,1852,-1846,-1851,-1591,1277,2107,1542,1278,-1852,2107,1542,1017,-1021,1281,1586,-1278}
    },
    {
        {-1513,-1252,1304,-1252,-1513,-1721,1826,-1513,-1513,-1721,-1200,1565,1513,1460,-1826,1513,-1513,-1252,1304,-1252,-1513,-1721},
        {1155,-1673,-1434,1151,-1103,-1382,-2008,1720,-1103,-1382,-1956,-1439,1668,1952,2008,-1720,1155,-1673,-1434,1151,-1103,-1382},
        {-1103,1151,-1434,-1673,1155,-1382,-2008,-1668,1155,-1382,-1956,-2004,-1720,1387,2008,1668,-1103,1151,-1434,-1673,1155,-1382},
        {1542,-1021,1017,-1586,-1281,1278,-1852,-1846,-1281,1278,-1591,-2112,-2107,-1538,1852,1846,1542,-1021,1017,-1586,-1281,1278},
        {1281,1586,-1017,1021,-1542,-1278,1852,-2107,-1542,-1278,1591,-1277,-1846,-1851,-1852,2107,1281,1586,-1017,1021,-1542,-1278}
    },
    {
        {1278,1586,-1281,-1021,-1017,1542,-2107,-1852,-1278,1542,-2107,1277,1591,-1851,1846,1852,1278,1586,-1281,-1021,-1017,1542},
        {1278,1021,1542,-1586,-1017,-1281,1846,-1852,-1278,-1281,1846,-2112,1591,1538,-2107,1852,1278,1021,1542,-1586,-1017,-1281},
        {1382,1151,1103,1673,-1434,-1155,-1668,2008,-1382,-1155,-1668,2004,-1956,1387,1720,-2008,1382,1151,1103,1673,-1434,-1155},
        {-1382,1673,1155,1151,1434,-1103,-1720,-2008,1382,-1103,-1720,-1439,1956,-1952,1668,2008,-1382,1673,1155,1151,1434,-1103},
        {1721,-1252,1513,1252,1304,1513,-1513,-1826,-1721,1513,-1513,-1565,-1200,1460,-1513,1826,1721,-1252,1513,1252,1304,1513}
    },
    {
        {1278,-1542,-1021,-1017,-1586,1281,-2107,-1852,1851,-1846,1277,1591,1278,-1542,2107,1852,1278,-1542,-1021,-1017,-1586,1281},
        {1278,1281,-1586,-1017,-1021,-1542,1846,-1852,-1538,2107,-2112,1591,1278,1281,-1846,1852,1278,1281,-1586,-1017,-1021,-1542},
        {1382,1155,1673,-1434,-1151,-1103,-1668,2008,-1387,-1720,2004,-1956,1382,1155,1668,-2008,1382,1155,1673,-1434,-1151,-1103},
        {-1382,1103,1151,1434,-1673,-1155,-1720,-2008,1952,-1668,-1439,1956,-1382,1103,1720,2008,-1382,1103,1151,1434,-1673,-1155},
        {1721,-1513,1252,1304,1252,-1513,-1513,-1826,-1460,1513,-1565,-1200,1721,-1513,1513,1826,1721,-1513,1252,1304,1252,-1513}
    },
    {
        {-1490,1486,-1239,1234,-1538,-1496,-1852,1481,1490,-1486,1239,1800,1852,1553,1858,-1176,-1490,1486,-1239,1234,-1538,-1496},
        {-1256,-1517,1256,-1308,994,-1308,-2146,-1570,1256,1517,-1256,1622,2146,1884,2198,1884,-1256,-1517,1256,-1308,994,-1308},
        {1408,-1643,-1408,1125,-1182,1125,-1758,-1926,-1408,1643,1408,-1067,1758,1983,2041,1983,1408,-1643,-1408,1125,-1182,1125},
        {1617,1339,-1617,-1387,1381,-1387,1329,-1664,-1617,-1339,1617,1392,-1329,1669,1674,1669,1617,1339,-1617,-1387,1381,-1387},
        {1239,1486,1490,-1496,-1538,1234,-1852,1481,-1239,-1486,-1490,1800,1852,-1176,1858,1553,1239,1486,1490,-1496,-1538,1234}
    },
    {
        {-1516,-1516,-1516,1213,-1516,1213,-1213,-1820,1516,1516,1516,1820,1820,-1213,1516,1820,-1516,-1516,-1516,1213,-1516,1213},
        {1213,-1516,-1516,-1516,1213,-1516,1516,-1820,-1213,1516,1516,1820,1820,1516,-1213,1820,1213,-1516,-1516,-1516,1213,-1516},
        {1638,1365,-1365,-1365,-1638,1365,-1638,1638,-1638,-1365,1365,1365,1638,1638,1638,-1365,1638,1365,-1365,-1365,-1638,1365},
        {-1516,1213,1213,-1516,-1516,-1516,1516,-1820,1516,-1213,-1213,1820,1820,1516,1516,1820,-1516,1213,1213,-1516,-1516,-1516},
        {1638,-1365,1365,1365,-1638,-1365,-1638,1638,-1638,1365,-1365,-1365,1638,1638,1638,1365,1638,-1365,1365,1365,-1638,-1365}
    },
    {
        {991,-1565,-1513,-1304,-1043,-1304,1826,-1878,-1252,1565,1513,1304,-2087,1878,-1878,2139,991,-1565,-1513,-1304,-1043,-1304},
        {1456,1652,-1360,-1126,-1173,-1126,-1700,1982,-1408,-1652,1360,1126,1747,-1982,1982,-2030,1456,1652,-1360,-1126,-1173,-1126},
        {-1621,1339,1613,-1387,-1382,-1387,-1334,-1669,1617,-1339,-1613,1387,1330,1669,-1669,1674,-1621,1339,1613,-1387,-1382,-1387},
        {1282,-1234,1491,1747,-1278,-1529,-1790,-1481,-1533,1234,-1491,-1747,1539,1481,1794,-1543,1282,-1234,1491,1747,-1278,-1529},
        {-1282,1234,-1491,1529,1278,-1747,-1486,-1794,-1742,-1234,1491,-1529,-1539,1794,1481,1543,-1282,1234,-1491,1529,1278,-1747}
    },
    {
        {-1638,-1365,-1365,-1365,1638,1365,1365,-1638,1638,1365,-1638,1638,-1638,-1365,-1365,1638,-1638,-1365,-1365,-1365,1638,1365},
        {1638,-1365,-1365,-1365,-1638,1365,1365,1638,-1638,1365,1638,-1638,1638,-1365,-1365,-1638,1638,-1365,-1365,-1365,-1638,1365},
        {-1260,1764,-1512,-1512,-1260,-1260,1764,1512,1260,-1764,1512,1512,-1764,1512,-1512,-1512,-1260,1764,-1512,-1512,-1260,-1260},
        {-1260,-1512,1764,-1512,-1260,-1260,-1512,1512,1260,1512,-1764,1512,1512,-1764,1764,-1512,-1260,-1512,1764,-1512,-1260,-1260},
        {-1260,-1512,-1512,1764,-1260,-1260,-1512,-1764,1260,1512,1512,-1764,1512,1512,-1512,1764,-1260,-1512,-1512,1764,-1260,-1260}
    },
    {
        {2520,1680,540,1680,1680,-1800,-1380,-540,-540,1800,1380,-2940,960,-960,-1380,1800,-960,960,-2100,2100,1380,1680},
        {825,1492,1347,927,927,1347,-1810,-1912,-1347,-1347,1810,2332,-1810,1245,-1245,-1912,1810,-1245,1665,-1665,1245,1492},
        {-2303,724,1846,1006,1006,1846,1745,-1564,-1846,-1846,-1745,2404,1745,-1463,1463,-1564,-1745,1463,-622,622,-1463,724},
        {1129,-1694,1412,1412,1412,1412,1977,1694,-1412,-1412,-1977,-1694,1977,1129,-1129,1694,-1977,-1129,1129,-1129,1129,-1694},
        {2520,1680,-1800,1680,1680,540,960,1800,1800,-540,-960,-2940,-1380,1380,960,-540,1380,-1380,-2100,2100,-960,1680}
    }
};

#endif



#elif (CHIP_CHEST_LEN == 8)
#if (CHIP_VAMOSI_SUPPORTED)
PROTECTED CONST INT16 g_sppChestTab[33][8][19] =
{
    //SET0:0-7
    {
        {-1878,-1976,-1278,1745,1275,1681,1103,-1595,2347,2470,1597,-2175,1745,1745,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2054,1665,1536,1665,-1567,1728,1882,2084,-2355,1740,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2335,2012,1901,2012,1582,-1444,1613,2372,2077,-2018,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1760,-2012,2193,2083,2513,1444,-1613,1723,2018,2018,-1871,2012,1265,-1155,1290},
        {1398,1300,-1278,1745,-2000,-1595,-2172,1681,2347,2470,1597,-2175,1745,1745,1853,-1681,1398,1300,-1278},
        {-1253,1382,1505,-1740,2041,-1665,-1536,-1665,1567,2367,2213,2011,-1740,2355,1912,1665,-1253,1382,1505},
        {1265,-1155,1290,2018,-1760,2083,-1901,-2012,-1582,1444,2482,1723,2018,-2077,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,1776,-2015,1646,-2015,-2335,-2243,1198,2464,1308,1308,-1886,2015,1868,1794,-958}
    },
    {
        {-1878,-1976,-1278,1745,-1423,-1103,-2003,-1275,2347,2470,1597,-1745,1853,-1917,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2183,-1536,-1493,-2054,-1567,1728,1882,2355,-1912,1837,-1912,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2224,2193,-1865,-1760,-2513,-1444,1613,2018,1871,-1935,1871,-2012,-1265,1155,-1290},
        {-1398,-1300,1278,-1745,-1853,-2172,2003,-2000,-2347,-2470,-1597,1745,1423,1917,-1853,1681,-1398,-1300,1278},
        {1253,-1382,-1505,1740,-1912,-1536,-1493,2041,-1567,-2367,-2213,-1740,2183,1837,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,1871,-1901,-1865,-1760,1582,-1444,-2482,-2077,-2224,2160,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1871,1901,-2230,-2335,-1582,1444,-1613,-2018,-1871,-2160,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,-1886,-1646,2593,-1776,-2335,-2243,1198,-1308,-1886,-1438,-1886,2015,1868,1794,-958}
    },
    {
        {-958,-1794,1868,-2015,-1886,-1308,1308,-2464,1198,2243,-2335,2015,1646,2015,1776,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2077,-2018,1723,-2482,1444,1582,-2012,1901,2083,1760,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,1912,-2355,-1740,-2011,2213,-2367,1567,1665,-1536,1665,2041,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,1745,-1745,-2175,-1597,2470,-2347,1681,2172,-1595,2000,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-2018,2018,-1723,-1613,-1444,2513,-2083,2193,2012,-1760,2077,1290,1155,1265},
        {1290,1155,1265,2083,-1871,-2018,-2077,2372,-1613,-1444,-1582,2012,-1901,2012,2335,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,2183,-1740,-2355,-2084,1882,-1728,-1567,-1665,1536,-1665,2054,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,1745,-1745,-2175,-1597,2470,-2347,-1595,-1103,1681,-1275,1745,1278,-1976,1878}
    },
    {
        {-958,-1794,1868,-2015,-1886,1438,-1886,1308,1198,2243,-2335,1776,2593,1646,-1886,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2160,1871,-2018,1613,1444,1582,-2335,2230,1901,1871,-2077,-1290,-1155,-1265},
        {-1290,-1155,-1265,-2083,1871,-2160,-2224,2077,-2482,1444,1582,1760,-1865,1901,1871,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,-2183,1837,-2183,-1740,2213,-2367,1567,2041,1493,-1536,1912,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,-1917,1423,-1745,-1597,2470,-2347,2000,2003,2172,-1853,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-1935,-1871,2018,-1613,-1444,2513,-1760,1865,2193,2224,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,-1912,-1837,-1912,-2355,1882,-1728,-1567,2054,-1493,1536,2183,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,-1917,-1853,-1745,-1597,2470,-2347,-1275,2003,-1103,1423,1745,1278,-1976,1878}
    },
    {
        {1249,-1384,-1480,-2076,1874,1706,-1874,2076,2331,1528,1336,2221,-1706,-2221,1706,-1874,1249,-1384,-1480},
        {-1269,1153,-1269,-2365,-1903,2047,1903,-1730,1413,2480,1730,1903,2047,-1903,-2047,1903,-1269,1153,-1269},
        {1269,-1153,1269,-1730,-2192,-2047,2192,1730,-1413,1615,2365,2192,2047,1903,-2047,-1903,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,-2192,2365,1413,-1615,1730,1903,2047,2192,2047,-2192,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,-2019,1648,1528,-1394,2221,1706,1874,2389,2221,-1480,-1384,1249},
        {1249,-1384,-1480,2019,-2221,1706,-1874,-2019,-1764,1528,1336,-1874,2389,1874,1706,2221,1249,-1384,-1480},
        {1480,1384,-1249,-2019,2221,-2389,1874,-2076,-1648,-1528,1394,1874,-1706,2221,1706,1874,1480,1384,-1249},
        {1499,1615,1499,-1672,-1846,2047,-2249,1672,-1297,-1442,-1672,1846,2047,-1846,2047,1846,1499,1615,1499}
    },
    {
        {1269,-1153,1269,-1730,-2192,2047,2192,1730,2365,1615,-1413,1903,2047,-1903,-2047,-2192,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,1903,2365,1730,2480,1413,-1903,2047,1903,-2047,-1903,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,2076,1336,1528,2331,1874,-1706,2221,1706,-2221,-1480,-1384,1249},
        {-1499,-1615,-1499,1672,-2249,2047,-1846,-1672,1672,1442,1297,1846,2047,-1846,2047,1846,-1499,-1615,-1499},
        {1249,-1384,-1480,-2076,1874,-2389,2221,-2019,-1394,1528,1648,1874,1706,2221,-1706,1874,1249,-1384,-1480},
        {1480,1384,-1249,-2019,-1874,1706,-2221,2019,-1336,-1528,1764,2221,1706,1874,2389,-1874,1480,1384,-1249},
        {-1249,1384,1480,-2019,-1874,-1706,1874,-2076,1394,-1528,-1648,2221,2389,1874,1706,2221,-1249,1384,1480},
        {1269,-1153,1269,2365,-2192,-2047,-1903,1730,-1730,1615,-1413,-2192,2047,2192,2047,1903,1269,-1153,1269}
    },
    {
        {-1269,-1269,-1153,2192,-2047,-1903,2047,1903,1413,-2365,1615,-1730,2192,1730,-2192,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,2047,-1903,-2047,1903,1413,1730,-2480,2365,-1903,1730,1903,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,-2047,2192,-2047,-2192,1413,1730,1615,-1730,2192,-2365,1903,2047,-1269,-1269,-1153},
        {1480,-1249,-1384,-1874,-1706,-1874,1706,-2221,-1648,1394,1528,2019,-1874,2076,-2221,2389,1480,-1249,-1384},
        {1499,1499,-1615,-1846,-2047,-1846,-2047,1846,-1297,-1672,1442,1672,2249,-1672,1846,-2047,1499,1499,-1615},
        {-1480,1249,1384,-2221,-2389,-2221,-1706,-1874,1648,-1394,-1528,2076,1874,2019,-1874,1706,-1480,1249,1384},
        {1249,-1480,1384,1874,-1706,-2221,-2389,-1874,-1764,1336,-1528,-2019,1874,2019,2221,-1706,1249,-1480,1384},
        {-1249,1480,-1384,2221,1706,-1874,-1706,-2221,-2331,-1336,1528,-2076,-1874,2076,1874,1706,-1249,1480,-1384}
    },
    {
        {-1480,1384,1249,1706,-1874,2389,1874,-1874,1648,-1528,-1394,-2221,2076,-2221,-2019,-1706,-1480,1384,1249},
        {-1249,-1384,1480,1706,1874,-1706,2221,1874,-2331,1528,-1336,-1874,-2076,2221,-2076,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,1903,2047,-1903,1903,1413,-2480,1730,-1903,-1730,-1903,2365,-2047,-1269,-1153,-1269},
        {-1269,-1153,-1269,-2047,1903,2047,2192,-2192,1413,1615,-2365,2192,-1730,-1903,-1730,2047,-1269,-1153,-1269},
        {1480,-1384,-1249,-1706,-2221,1706,2221,1874,-1648,1528,1394,-1874,2019,-1874,-2076,-2389,1480,-1384,-1249},
        {-1499,1615,-1499,-2047,-1846,-2047,1846,2249,1297,-1442,1672,1846,-1672,1846,-1672,-2047,-1499,1615,-1499},
        {-1249,-1384,1480,-2389,-2221,-1706,-1874,1874,1764,1528,-1336,2221,2019,-1874,2019,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,-2192,-2047,-1903,-2192,1413,1615,1730,-1903,2365,2192,-1730,2047,-1269,-1153,-1269}
    },
    //SET1:8-15
    {
        {999,1774,1943,-2087,765,1412,-1785,1412,1156,-1913,-1954,-2802,-1981,2500,-1384,2188,-1352,-1803,-1558},
        {-1133,1245,1347,2203,-1576,1318,1118,-1412,1497,1634,-1715,-2576,-2981,-2482,1829,-572,2424,-1661,-2043},
        {2040,-1137,1300,2279,1338,-1384,1461,1347,-1744,1917,1756,-2353,-2707,-2972,-1945,1678,-282,1735,-1395},
        {1824,1878,-1405,2096,1604,1306,-841,1306,1116,-1193,2442,1454,-2132,-2613,-2366,-2223,2202,-818,1948},
        {1259,1550,1690,-1933,2019,959,2314,-1771,1584,964,-556,2995,2102,-1098,-1985,-2934,-1885,2206,-964},
        {-1150,1425,967,1762,-1296,2408,944,2408,-1576,2072,1402,-977,3036,1796,-1706,-1257,-2571,-2127,1889},
        {-1914,-1530,1737,721,1835,-1800,1971,931,2095,-2049,1435,2772,-903,2123,1789,-2453,-1910,-1190,-1612},
        {711,-1965,-1911,2450,441,2014,-1549,2014,762,2701,-1603,1050,2874,-1147,2286,1873,-1995,-2335,-1086},
    },
    {
        {1372,-2075,1101,1628,-2212,-1119,1894,-2459,-999,-2337,536,-1829,-1724,-2817,1054,1994,2202,1564,-1852},
        {-1640,2078,-1568,1335,850,-3081,-2013,1927,-2235,-720,-1655,1027,-2002,-1538,-2803,1130,2020,2322,822},
        {-988,-2665,2499,-1964,1627,1644,-2090,-1449,1398,-2288,-1040,-2015,1191,-1845,-1451,-1561,1483,2141,1431},
        {-1427,-1932,-2278,2347,-2086,2473,2011,-1975,-1446,1072,-2242,-1075,-2101,1275,-1502,-715,-1769,1411,1630},
        {-1729,-1774,-1751,-2138,1797,-1447,1711,1642,-1193,-1863,1602,-1690,-1370,-1956,1779,-1801,-1729,-2160,1636},
        {1536,-2575,-1987,-1344,-2340,2101,-784,755,1595,-2116,-2016,1006,-2135,-2265,-1588,1732,-1643,-1565,-1685},
        {-1398,1951,-1990,-1630,-2256,-3222,1503,-967,839,1628,-1454,-1758,690,-2230,-2152,-1278,1988,-1375,-2459},
        {935,-429,1775,-2385,-893,-3033,-3012,2424,-1595,1840,1187,-1556,-1166,1142,-2970,-2074,-536,2183,-1633},
    },
    {
        {-1086,2335,-1995,-1873,2286,1147,2874,-1050,-1603,-2701,762,-2014,-1549,-2014,441,-2450,-1911,1965,711},
        {1612,-1190,1910,-2453,-1789,2123,903,2772,-1435,-2049,-2095,931,-1971,-1800,-1835,721,-1737,-1530,1914},
        {1889,2127,-2571,1257,-1706,-1796,3036,977,1402,-2072,-1576,-2408,944,-2408,-1296,-1762,967,-1425,-1150},
        {964,2206,1885,-2934,1985,-1098,-2102,2995,556,964,-1584,-1771,-2314,959,-2019,-1933,-1690,1550,-1259},
        {1948,818,2202,2223,-2366,2613,-2132,-1454,2442,1193,1116,-1306,-841,-1306,1604,-2096,-1405,-1878,1824},
        {1395,1735,282,1678,1945,-2972,2707,-2353,-1756,1917,1744,1347,-1461,-1384,-1338,2279,-1300,-1137,-2040},
        {-2043,1661,2424,572,1829,2482,-2981,2576,-1715,-1634,1497,1412,1118,-1318,-1576,-2203,1347,-1245,-1133},
        {1558,-1803,1352,2188,1384,2500,1981,-2802,1954,-1913,-1156,1412,1785,1412,-765,-2087,-1943,1774,-999},
    },
    {
        {-1633,-2183,-536,2074,-2970,-1142,-1166,1556,1187,-1840,-1595,-2424,-3012,3033,-893,2385,1775,429,935},
        {2459,-1375,-1988,-1278,2152,-2230,-690,-1758,1454,1628,-839,-967,-1503,-3222,2256,-1630,1990,1951,1398},
        {-1685,1565,-1643,-1732,-1588,2265,-2135,-1006,-2016,2116,1595,-755,-784,-2101,-2340,1344,-1987,2575,1536},
        {-1636,-2160,1729,-1801,-1779,-1956,1370,-1690,-1602,-1863,1193,1642,-1711,-1447,-1797,-2138,1751,-1774,1729},
        {1630,-1411,-1769,715,-1502,-1275,-2101,1075,-2242,-1072,-1446,1975,2011,-2473,-2086,-2347,-2278,1932,-1427},
        {-1431,2141,-1483,-1561,1451,-1845,-1191,-2015,1040,-2288,-1398,-1449,2090,1644,-1627,-1964,-2499,-2665,988},
        {822,-2322,2020,-1130,-2803,1538,-2002,-1027,-1655,720,-2235,-1927,-2013,3081,850,-1335,-1568,-2078,-1640},
        {1852,1564,-2202,1994,-1054,-2817,1724,-1829,-536,-2337,999,-2459,-1894,-1119,2212,1628,-1101,-2075,-1372},
    },
    {
        {829,-962,-1448,-1040,-2046,2253,-3083,2124,1872,-962,-1448,-1040,1935,-2426,-2035,-2456,-617,-1552,2639},
        {1787,1632,-1390,-1855,-817,-1533,2345,-2588,1767,1632,-1390,-1855,-1513,2967,-2150,-1318,-1478,-578,-2174},
        {-2306,1379,1678,-1266,-2217,-940,-1915,2282,-2052,1379,1678,-1266,-1866,-1983,2489,-1965,-1640,-1849,-617},
        {1502,-2062,1610,1983,-1231,-1766,-1565,-2091,1201,-2062,1610,1983,-1478,-1638,-1630,1868,-1759,-1733,-1993},
        {-1609,1022,-1956,1775,1637,-1315,-2182,-1667,-1701,1022,-1956,1775,2186,-2124,-2037,-1900,1395,-2044,-1465},
        {-1418,-1736,1332,-1664,1883,1204,-1096,-2609,-1722,-1736,1332,-1664,1496,2232,-2060,-1672,-2054,1428,-2428},
        {-2236,-1787,-1331,1730,-1674,1323,1329,-1635,-2486,-1787,-1331,1730,-1851,1242,2145,-1802,-2087,-2103,1160},
        {1336,-1870,-1377,-1085,2455,-2216,2328,761,-2450,-1870,-1377,-1085,1412,-1153,2054,2359,-1635,-1423,-2521},
    },
    {
        {-200,1776,2126,-1566,-1901,1869,-1963,1043,-2092,3422,1823,-2107,-1060,-3472,-2615,-194,1788,-538,-1601},
        {2070,-459,1938,1418,-1946,-2206,622,-1138,1124,-2081,2726,2091,-1953,-2042,-3380,-1927,1117,1249,-1280},
        {1976,2397,-444,1916,1363,-964,-2476,1779,-1603,2267,-1396,1791,996,-2596,-3266,-2246,-792,1628,871},
        {2268,1781,2133,-1610,1164,1712,-1805,-2078,1632,-1274,2031,-1495,1397,1066,-2463,-2505,-1488,-1353,1512},
        {1692,2096,1390,1160,-2241,1578,1288,-1710,-2202,1875,-1348,1905,-1922,1906,1268,-1901,-2207,-1905,-1177},
        {1423,1404,1531,1894,1723,-2153,1929,1850,-1408,-2419,1791,-1364,1489,-1474,1787,2120,-1565,-1674,-1769},
        {-1256,1305,1373,2814,2894,1133,-1391,1857,2300,-2106,-2573,2152,-944,1546,-1398,1490,1691,-938,-1606},
        {810,-1115,1780,1117,2590,2879,294,-701,1665,2724,-2216,-2703,2162,-2161,1110,-1095,2479,1510,-1658},
    },
    {
        {2603,-1190,-1190,1735,-818,1735,2479,2776,1809,-2231,-1190,1735,-818,-2231,-2107,2404,-2156,322,-1239},
        {2352,1452,-1824,-1162,2023,-1162,1616,2072,2228,1495,-1824,-1162,2023,-1782,-2684,-1775,1609,-1690,830},
        {1353,2040,1385,-1829,-1464,1448,-1957,1793,1402,2351,1385,-1829,-1464,1696,-1875,-1912,-1627,1987,-1971},
        {1924,1236,1892,1829,-1812,-1448,1957,-1793,1874,926,1892,1829,-1812,-1696,1875,-1365,-1650,-1987,1971},
        {-1403,1802,1147,1795,1649,-1482,-1461,2348,-1512,1905,1147,1795,1649,-2027,-2297,1846,-2058,-1225,-2218},
        {2394,-1750,1527,1596,1869,1596,-996,-1378,2320,-2052,1527,1596,1869,1224,-1938,-1720,1948,-2325,-1140},
        {1353,2040,-1892,1448,1812,1448,1320,-1484,-1874,2351,-1892,1448,1812,1696,1401,-1912,-1627,1987,-1971},
        {2444,998,1654,-1101,1301,2176,2453,2039,-1041,-2797,1654,-1101,1301,1135,1454,2392,-2082,-1922,1723},
    },
    {
        {-2278,-1281,-1960,2582,469,-1655,-2267,-1288,-2100,-1964,-1831,1122,1277,-2453,1822,-1470,1672,1947,-1331},
        {2027,-1529,-1562,-1511,1893,1665,-1965,-1814,-1450,-1529,-1496,-1765,1562,1578,-2330,1696,-1798,1583,2014},
        {-1162,1558,-1111,-2204,-916,975,2309,-2381,-1740,-1856,-1917,-1473,-2302,1398,1668,-2548,2002,-1920,1330},
        {767,-1516,1777,-1273,-2445,-1324,1472,1649,-2311,-1516,-1532,-1904,-1777,-2037,2155,1434,-1887,2245,-1748},
        {1162,1173,-2302,2886,-1814,-1657,-2309,1698,1057,-2240,-1496,-1258,-1111,-2081,-1668,1865,1412,-1494,2084},
        {2045,1593,1737,-3022,2624,-1426,-548,-2465,1752,1593,-1829,-1416,-1737,-544,-1488,-2316,2005,946,-1680},
        {-2417,1160,1075,2466,-2755,1706,-2468,-824,-2419,1160,1244,-1960,-1075,-2296,-872,-736,-2045,2758,1332},
        {2278,-1449,1277,832,2261,-1758,2267,-2125,-1314,-2132,1149,1609,-1960,-960,-1822,-1943,-989,-2629,2014},
    },
    //SET2:16-23
    {
        {983,2116,-1748,2150,2653,1839,-1767,-2066,-1713,-2358,2396,-983,1662,-1559,686,2291,1348,-1348,1102},
        {-1303,1761,2079,-1757,2252,2814,1137,-1546,-1811,-2364,-2083,1303,-1290,1162,-1100,1488,2348,1748,-1422},
        {1303,-1761,2017,1757,-2252,1282,2959,1546,-2285,-1732,-2013,-1303,1290,-1162,1100,-1488,1748,2348,1422},
        {1428,1426,-1755,2013,2674,-1645,1063,2566,1811,-2421,-1726,-1428,-1201,1472,-1671,1115,-1614,1614,2125},
        {1363,1719,2035,-2581,2377,2387,-2361,1669,2949,1352,-2240,-1363,-2085,-1133,1141,-1473,961,-961,620},
        {1747,1646,2010,1620,-2231,1894,1693,-2014,1239,2300,2057,-1747,-1573,-2227,-1258,1432,-1214,1214,-1651},
        {-1516,1587,1252,2539,1105,-2189,2378,1442,-2265,1546,2160,1516,-1397,-1706,-1820,-1327,1548,-1548,1927},
        {-1278,-879,1324,1571,3237,2228,-2519,1934,2284,-2501,1251,1278,1489,-1588,-1654,-1268,-1601,1601,-1282},
    },
    {
        {-732,-2433,3028,2114,1705,-2174,2306,1470,-2480,1456,-1058,1198,1708,2318,637,-1466,1218,1943,1324},
        {-1439,-528,-2633,2821,1651,1582,-2701,1688,1493,-2245,2045,-1198,1055,1828,2324,874,-1613,1018,2032},
        {-1597,-1513,-1099,-2351,1656,2053,1036,-3006,2172,2443,-1991,989,-1444,1417,1395,2016,882,-2243,1466},
        {-753,-1614,-1552,-1221,-2424,1521,1679,584,-3160,2614,2797,-1978,1210,-1255,1849,1593,1988,933,-2043},
        {1783,-1194,-1628,-796,-1305,-1817,2390,2951,1036,-3194,1468,2177,-2143,1181,-2072,959,1900,1792,981},
        {-1679,2488,-1752,-2269,-1501,-1295,-2693,1430,3261,756,-2263,1343,1631,-2420,959,-1495,555,1431,1547},
        {1066,-1855,2572,-2053,-1416,-1539,-1555,-2988,1033,3811,745,-1778,2252,1631,-940,1382,-1428,1093,1632},
        {1498,941,-2430,2450,-1874,-683,-1952,-1584,-2662,1866,3478,354,-1169,2084,2624,-846,1450,-1458,1365},
    },
    {
        {1958,-2448,-2236,-1388,-2295,-1527,2017,-957,-1863,-2017,1820,-1447,1250,-1250,-2002,1805,-2295,-1096,1096},
        {1504,1533,-1840,-2360,-1874,-1846,-1192,2077,-1533,-1539,-1394,1701,-1221,1903,-1701,-1915,2222,-2187,-1226},
        {1226,1880,1840,-1736,-2222,-1568,-1539,-1394,2216,-1192,-2019,-1701,1903,-1221,1701,-1498,-2222,2187,-1504},
        {-1586,1982,1933,1736,-1443,-2187,-2306,-2037,-1120,2306,-1412,-1641,-1562,1562,-946,1840,-1443,-1863,1863},
        {1226,-2216,1840,2360,1874,-1568,-1539,-1394,-1880,-1192,2077,-1701,-2193,-1221,1701,-1498,1874,-1909,-1504},
        {1504,1533,-1840,1736,2222,2250,-1192,-2019,-1533,-1539,-1394,1701,-1221,-2193,-1701,2181,-1874,1909,-1226},
        {-1863,2329,1516,-1736,2306,2187,1443,-1412,-1467,-1443,-2037,-946,1562,-1562,-1641,-1840,2306,-1586,1586},
        {-1096,-2726,1374,1388,-2017,1527,2295,1820,-1586,-2295,-957,-2002,-1250,1250,-1447,-1805,-2017,1958,-1958},
    },
    {
        {1044,-1297,2606,-1978,1789,1303,-2039,-1819,1191,3088,1723,1044,2556,-892,1591,-2054,-2064,948,1744},
        {1265,794,-949,2210,-1711,1922,981,-1946,-2477,2185,3590,1265,1149,2741,-1140,1260,-2066,-2122,996},
        {1993,1156,1466,-1321,1887,-1399,1936,1246,-1810,-1941,2796,1993,1227,1735,2022,-1680,1816,-1739,-1605},
        {1613,1870,1436,1789,-1638,1539,-1445,1734,2191,-2025,-2472,1613,1596,1332,1669,2029,-1365,1917,-1494},
        {-1455,1611,1958,1393,1996,-1477,1424,-1906,2429,1869,-2128,-1455,2107,1154,1987,1618,1499,-1170,2131},
        {-1202,-1374,1454,1511,1711,2422,-1466,1705,-3001,2878,2591,-1202,-1044,2121,1064,1860,1381,1425,-1356},
        {1645,-1253,-1871,2339,1243,699,2457,-1481,1905,-3447,1618,1645,-2268,-1047,2071,1743,2258,890,886},
        {791,1687,-986,-2096,2074,1499,851,2761,-1572,2080,-2996,791,1611,-1859,-1582,1800,2150,2448,1135},
    },
    {
        {1463,-1179,1842,-650,-2739,-1939,1712,-2475,2278,3241,1582,2259,-1067,1140,2089,1040,-1315,-1496,1263},
        {-1854,1693,-1293,1204,-1334,-2732,-1321,1906,-2011,1915,2808,1390,1456,-1371,1682,2548,924,-1975,-1351},
        {-1268,-1738,1964,-1561,1209,-967,-2719,-1160,2054,-1779,1066,2690,1263,1644,-2087,1541,2714,1189,-2157},
        {-1203,-1369,-582,1724,-2420,1004,-870,-3396,-193,2963,-2098,1403,1945,1722,1468,-2289,1612,2863,1643},
        {-2017,-737,-2260,-1200,1946,-1791,1421,150,-3740,-1218,1927,-2783,1209,1409,1571,1870,-2229,1211,2381},
        {1563,-2053,-2199,-2215,-94,2299,-1562,2274,-1067,-4241,-1176,1790,-1559,926,1300,2031,1566,-2502,352},
        {2146,1348,-1616,-1334,-2491,-933,1664,-2401,2467,-1267,-2492,-938,1607,-1677,2012,937,2090,1676,-1673},
        {577,2065,1939,-1155,-1251,-2603,-1525,1267,-2292,2708,-970,-2416,-906,1837,-1979,1476,1196,2674,1931},
    },
    {
        {-1547,2380,-855,1636,-1494,1352,-2008,-2733,-1245,-962,2894,2716,-2698,1068,2963,659,448,1848,1262},
        {-545,-1332,2392,-1516,1353,-1191,1778,-1448,-2496,-1405,-1048,2366,2784,-2990,1349,3356,1121,719,1578},
        {-1163,-1335,-1604,1185,-1063,941,-143,2991,-1176,-1804,-2691,-2298,2913,2004,-4155,1817,3160,-158,483},
        {-1798,-1285,-2240,-1485,1414,-1343,718,-729,2337,-472,-1923,-2215,-3128,2800,1567,-3664,1110,1934,607},
        {1968,-1887,-1083,-2055,-1549,1056,-1657,533,-1007,1854,-842,-1606,-2182,-2742,2685,1121,-2899,1966,2080},
        {1734,1811,-1445,-1947,-2271,-1703,1572,-705,795,-1625,976,-1607,-1008,-2290,-2894,2565,2460,-1842,1517},
        {-2025,1951,1044,-1853,-2090,-2158,-1607,1453,-1089,1174,-1649,1032,-2360,-1305,-2241,-2110,2426,1803,-1396},
        {-1386,-1541,1885,1462,-2016,-1526,-2368,-2028,1595,-952,2187,-1341,725,-2356,-608,-2030,-2777,2209,1775},
    },
    {
        {-1983,-1548,2041,-2760,1729,-859,-1902,-1998,-1347,-1342,-880,3165,2213,1518,-2500,1347,1032,854,1750},
        {1514,-1466,-1695,2419,-3421,2426,-656,-2327,-1471,-1197,-1922,-1015,2150,1650,1699,-2195,1651,966,927},
        {1633,2234,-1807,-892,1937,-2716,2586,-744,-1538,-848,-1669,-2042,-2216,1586,2059,1995,-1808,1568,891},
        {-1163,1483,2056,-2097,-1086,2072,-2513,2258,-841,-2202,-582,-2146,-1782,-2004,1847,1835,1527,-1705,1568},
        {-1196,-1356,1166,1718,-2738,-931,2677,-2766,1997,-1853,-1778,-1696,-1900,-1769,-1561,1297,992,1487,-1891},
        {-1405,-772,-2040,2042,2485,-2530,-1226,3086,-1938,2800,-1314,-1702,-1836,-1337,-905,-1541,1271,1271,1269},
        {1197,-1685,-272,-2616,1477,2316,-2307,-1450,2495,-2434,2390,-1361,-1650,-2297,-1974,-920,-1486,1038,1403},
        {-1879,812,-1594,-1099,-2563,1652,2190,-3248,-1670,1404,-2325,2404,-754,-1079,-2346,-1919,-1279,-1137,1414},
    },
    {
        {-1848,-969,-1785,-1227,1879,-1598,-1725,-1583,3135,1879,2098,842,1848,-2177,2281,1089,-2191,1275,1339},
        {1524,-1437,-1342,-1963,-909,2171,-1328,-1781,-2575,2653,1260,1500,613,2026,-2825,1643,2416,-1938,865},
        {-1770,1594,-1608,-1258,-2246,-1124,2098,-1359,-1458,-2246,2415,1628,1770,1203,1797,-2374,1438,1655,-1728},
        {1768,-1833,2346,-1420,-1479,-1619,-847,2695,-2117,-2192,-2306,1893,1081,1479,1146,1146,-2122,1958,1320},
        {1306,2021,-2196,2370,-1010,-2018,-1278,-1797,2763,-1722,-1782,-1993,1543,935,1716,1418,1426,-1820,1655},
        {-977,1583,1633,-2174,2282,-1225,-1924,-1518,-1830,2994,-2120,-1570,-1873,2187,500,1989,1768,802,-1820},
        {-1647,-1396,2079,1699,-2634,2574,-1626,-1146,-1086,-1921,3018,-2091,-1202,-1909,2362,574,1098,1473,1233},
        {-613,-1437,-2054,1599,1940,-2103,2234,-1068,-1862,-1621,-3015,2213,-1524,-823,-2825,1643,1704,912,1577},
    },
    //SET2:24-31
    {
        {2027,-1011,1879,1601,1196,1386,1737,-2700,1993,2176,1684,-1915,-2044,1697,-1172,1991,2225,949,1386},
        {-1665,1993,-1507,1573,1906,982,1462,1595,-2285,1748,2140,1513,-1725,-1969,2197,-1659,1696,2169,982},
        {-1737,-1438,1495,-2129,1495,1737,1032,2028,1399,-2451,1304,1928,1705,-1969,-1645,1686,-2436,1419,1737},
        {-1402,-1765,-1815,1181,-1815,1402,2180,970,2436,1338,-2520,1091,2142,1611,-1592,-2126,1476,-2503,1402},
        {1331,-1637,-1870,-1468,1543,-2014,1898,1789,1479,2016,1636,-1545,1980,1630,1879,-2084,-1286,1668,-2014},
        {1735,1367,-1160,-1496,-1843,1678,-2473,1988,1297,1589,2092,1883,-1797,2060,1152,2458,-1815,-1208,1678},
        {-1330,1708,1536,-1460,-1195,-1401,2820,-2528,2378,2123,1522,1011,1389,-1720,1890,1217,2260,-1879,-1401},
        {-1052,-1352,1230,1566,-1501,-1678,-2873,2498,-2501,1642,2254,1688,915,2064,-1227,1866,881,2303,-1678},
    },
    {
        {1461,3076,1437,1699,-3005,1849,-914,1071,2560,208,-2386,2350,-1803,-2125,-1002,1545,1688,1327,1264},
        {-1564,1593,2122,660,2739,-2122,1975,-1583,670,2896,1230,-1873,2284,-1322,-2774,-1015,1397,1636,1312},
        {966,-1959,1750,1842,1260,1892,-1654,1509,-2251,572,3259,661,-1349,2621,-1380,-3511,-1522,1339,1471},
        {1811,2041,-1455,2002,1275,1415,2138,-1908,2175,-2145,254,3819,130,-1648,1982,-1340,-2994,-763,1475},
        {1550,2306,1629,-1984,2414,1403,2126,1242,-1468,2433,-1248,435,3241,863,-2235,1904,-713,-2197,-1378},
        {-1344,1296,2512,2041,-2397,2559,802,2848,1115,-1614,1760,-1229,686,2682,1285,-1982,1617,-1244,-1755},
        {-1702,-1678,735,2447,2048,-1809,2166,1160,3004,1173,-1491,1858,-1394,781,3050,1823,-1810,1300,-1338},
        {1096,-2697,-1531,764,2484,874,-1524,2291,1015,2728,1273,-2610,2182,-905,1641,2764,1769,-1966,654},
    },
    {
        {2198,320,1654,1764,-2662,2312,-1313,1663,-1176,1725,2456,-3251,-1730,-640,2282,1805,1068,1535,1214},
        {1187,2500,555,1329,2416,-2752,2554,-1246,1818,-1521,1040,3003,-2363,-2078,-1074,1902,1786,611,1032},
        {-2728,1705,2065,516,2257,1652,-1898,1177,-1331,649,-2085,1930,2980,-1819,-2520,-1883,1061,1313,1198},
        {1640,-2525,2193,2072,458,3270,1034,-922,1149,-739,1383,-2576,1765,2598,-1543,-1833,-1817,1588,1662},
        {1967,1967,-2078,1891,2584,1014,3048,1723,-779,1271,-812,1548,-1979,1265,2394,-1395,-1821,-1833,1399},
        {-1670,2754,2216,-1559,2116,2394,1686,2061,1233,-1651,1073,-798,1388,-1784,1433,1871,-1829,-2111,-1141},
        {-1387,-2116,2178,2133,-1727,2092,2095,1650,2198,1551,-1029,1012,-1607,1857,-1714,1809,1637,-1357,-1617},
        {-988,-1769,-2396,2408,1552,-2115,2161,1767,1566,2283,1594,-1280,643,-1330,2063,-1754,2002,1657,-1441},
    },
    {
        {1360,-2329,-1150,-1104,-1780,-2055,1634,-2349,2459,-784,2139,3035,994,2165,-2020,-1634,2669,554,554},
        {1689,1799,-1555,-1493,-1094,-1468,-2440,1622,-2373,2202,-1756,1518,2244,717,2818,-1872,-1376,1799,937},
        {-1589,1783,1369,-1307,-1421,-1748,-1670,-1440,1489,-2341,1660,-2441,1258,1797,1294,2533,-2181,-1525,1925},
        {-1251,-2579,1540,1478,-2603,-2234,-905,-1983,-1299,1762,-1583,2489,-1473,2229,1280,905,2267,-1454,-1454},
        {1861,-804,-2081,1279,2029,-1748,-2533,-1440,-1961,-1478,1660,-1578,2121,-1653,2157,1670,1269,1925,-1525},
        {-1754,2113,-1770,-1699,2247,1823,-2045,-1764,-1486,-1919,-1266,1395,-1605,1481,-1684,2045,1544,1565,1565},
        {-1795,-1648,2000,-1357,-1892,1865,1718,-1043,-2283,-1201,-2439,-2002,1406,-1914,2087,-1718,1199,1601,1601},
        {827,-1651,-693,1957,-1957,-1468,1872,1622,-1510,-2110,-1756,-2794,-2068,1579,-1494,2440,-2239,937,1799},
    },
    {
        {1365,1339,-1116,1777,962,952,617,-2202,2218,962,952,-3597,2295,-1725,-3420,1455,2669,1806,1339},
        {2133,1335,2299,-1486,1915,961,627,1373,-2047,1915,961,910,-2827,2563,-2112,-2878,1519,1572,1335},
        {1561,1529,1639,2069,-1953,1314,932,1738,1484,-1953,1314,1486,1595,-2082,2865,-2366,-2384,976,1529},
        {-1414,1444,1751,2005,1632,-2205,1530,1254,1879,1632,-2205,1633,1169,2106,-1957,1691,-2275,-1541,1444},
        {-1017,-1213,1876,2127,1899,1697,-2096,1572,1518,1899,1697,-2328,1488,1362,1758,-2651,1731,-1626,-1213},
        {1132,-1946,-1831,1218,1623,1164,1650,-1291,983,1623,1164,2813,-2307,2011,2708,1809,-2654,898,-1946},
        {-2289,776,-2641,-2163,913,1413,1219,1461,-2260,913,1413,2305,1302,-2199,3522,2047,774,-2385,776},
        {-1940,-1929,891,-2115,-2187,1128,1610,794,1659,-2187,1128,1169,1796,1242,-2526,2756,1947,1833,-1929},
    },
    {
        {-2828,-891,-1992,-1106,-955,-1950,1950,-2115,1399,1548,1345,-3057,-1442,1671,-1160,2115,-1358,-2328,1560},
        {-1892,-2599,-966,-1600,-1335,-1035,-1815,2088,-1913,1365,1542,1782,-2698,-1689,1855,-1375,1845,-1352,-2022},
        {2204,-1845,-1829,-1277,-904,-1035,-1815,-1362,1859,-1976,1111,2105,1829,-2443,-2025,2074,-1065,1990,-2022},
        {1794,2024,-1488,-2152,-932,-678,-1459,-1778,-1521,1804,-2209,1301,1962,2094,-2840,-1784,2330,-935,1682},
        {-1794,2250,2200,-1409,-1917,-1459,-678,-1072,-1328,-1091,2209,-2013,1600,2180,2128,-2490,-2330,1648,-970},
        {1638,-2109,2085,2210,-1612,-1425,-1425,-922,-1569,-1753,-1380,1925,-2253,1254,2399,1635,-1940,-1667,1567},
        {-1950,1566,-2851,2091,1714,-2205,-645,-1941,-514,-1198,-1273,-2251,1697,-2109,1332,2653,1160,-2533,-1085},
        {-1268,-2451,1129,-2667,1386,1950,-1950,-1335,-1722,-793,-1776,-716,-2223,1671,-2720,1335,2543,1573,-1560},
    },
    {
        {-685,-1321,-1281,-1024,-1744,2076,2099,-3693,578,-1790,935,-1193,1914,-1506,-2722,2699,3094,1092,1321},
        {-1451,-835,-2365,-859,-1030,-2198,1970,1743,-3504,1355,-2207,701,-531,1722,-1745,-2423,2650,2643,835},
        {-1428,-1362,-1326,-864,-577,-2540,-2744,3054,1854,-1742,908,-1369,1082,-683,2247,-2044,-3755,1826,1362},
        {2311,-1684,-1820,-1665,-1281,-864,-1966,-1871,3171,1945,-1108,954,-1339,988,-1053,2155,-2249,-2662,1684},
        {1386,2266,-2379,-2638,-1705,-1221,-711,-1708,-1842,3039,2027,-1768,835,-1046,772,-1282,2278,-1597,-2266},
        {-2304,1534,3120,-1847,-2435,-1240,-1554,-1767,-1807,-2161,2667,2300,-1712,666,-815,1130,-918,1258,-1534},
        {842,-2143,2512,2160,-1798,-1314,-1143,-2151,-1986,-3182,-1946,2298,1660,-1424,703,-874,1882,-606,2143},
        {1465,534,-3226,2027,1790,-2009,-818,-827,-1962,-1787,-2867,-2386,2622,1585,-1992,801,-792,2742,-534},
    },
    {
        {1169,1166,2750,2183,1499,-2474,2833,-1747,1884,2158,-2064,-1327,2897,1276,482,-1223,-534,-1840,1263},
        {1517,853,886,1977,1995,2407,-2251,2845,-2207,1264,2302,-2382,-1568,2477,871,1298,-1440,-722,-1506},
        {-1379,1428,892,1098,2110,1822,2113,-2338,2763,-2370,1234,2527,-2608,-1399,2440,604,1494,-1202,-945},
        {-1283,-1151,2114,1535,1500,1203,2102,1759,-1910,3381,-2868,1173,2883,-1956,-1186,1817,536,1246,-1164},
        {-988,-1309,-1018,2360,1696,1284,1092,1964,1765,-1966,3281,-2749,1116,3049,-1972,-1430,1952,649,1126},
        {1361,-1329,-1627,-1805,2160,2622,1434,1124,1469,1102,-1793,3005,-3058,697,2636,-1165,-1623,1815,942},
        {-1929,1413,-1395,-1854,-2077,1498,1901,1994,1249,2338,1223,-1450,2310,-2234,1190,2180,-1531,-1568,1435},
        {1640,-1662,1805,-1253,-1738,-2257,2266,1686,2360,1700,2040,777,-740,2300,-2148,1286,1922,-2055,-1131},
    },
    // TODO : replace this vector with the vector for SCH
    {
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
    }

};
#else
PROTECTED CONST INT16 g_sppChestTab[9][8][19] =
{
    {
        {-1878,-1976,-1278,1745,1275,1681,1103,-1595,2347,2470,1597,-2175,1745,1745,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2054,1665,1536,1665,-1567,1728,1882,2084,-2355,1740,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2335,2012,1901,2012,1582,-1444,1613,2372,2077,-2018,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1760,-2012,2193,2083,2513,1444,-1613,1723,2018,2018,-1871,2012,1265,-1155,1290},
        {1398,1300,-1278,1745,-2000,-1595,-2172,1681,2347,2470,1597,-2175,1745,1745,1853,-1681,1398,1300,-1278},
        {-1253,1382,1505,-1740,2041,-1665,-1536,-1665,1567,2367,2213,2011,-1740,2355,1912,1665,-1253,1382,1505},
        {1265,-1155,1290,2018,-1760,2083,-1901,-2012,-1582,1444,2482,1723,2018,-2077,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,1776,-2015,1646,-2015,-2335,-2243,1198,2464,1308,1308,-1886,2015,1868,1794,-958}
    },
    {
        {-1878,-1976,-1278,1745,-1423,-1103,-2003,-1275,2347,2470,1597,-1745,1853,-1917,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2183,-1536,-1493,-2054,-1567,1728,1882,2355,-1912,1837,-1912,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2224,2193,-1865,-1760,-2513,-1444,1613,2018,1871,-1935,1871,-2012,-1265,1155,-1290},
        {-1398,-1300,1278,-1745,-1853,-2172,2003,-2000,-2347,-2470,-1597,1745,1423,1917,-1853,1681,-1398,-1300,1278},
        {1253,-1382,-1505,1740,-1912,-1536,-1493,2041,-1567,-2367,-2213,-1740,2183,1837,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,1871,-1901,-1865,-1760,1582,-1444,-2482,-2077,-2224,2160,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1871,1901,-2230,-2335,-1582,1444,-1613,-2018,-1871,-2160,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,-1886,-1646,2593,-1776,-2335,-2243,1198,-1308,-1886,-1438,-1886,2015,1868,1794,-958}
    },
    {
        {-958,-1794,1868,-2015,-1886,-1308,1308,-2464,1198,2243,-2335,2015,1646,2015,1776,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2077,-2018,1723,-2482,1444,1582,-2012,1901,2083,1760,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,1912,-2355,-1740,-2011,2213,-2367,1567,1665,-1536,1665,2041,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,1745,-1745,-2175,-1597,2470,-2347,1681,2172,-1595,2000,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-2018,2018,-1723,-1613,-1444,2513,-2083,2193,2012,-1760,2077,1290,1155,1265},
        {1290,1155,1265,2083,-1871,-2018,-2077,2372,-1613,-1444,-1582,2012,-1901,2012,2335,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,2183,-1740,-2355,-2084,1882,-1728,-1567,-1665,1536,-1665,2054,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,1745,-1745,-2175,-1597,2470,-2347,-1595,-1103,1681,-1275,1745,1278,-1976,1878}
    },
    {
        {-958,-1794,1868,-2015,-1886,1438,-1886,1308,1198,2243,-2335,1776,2593,1646,-1886,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2160,1871,-2018,1613,1444,1582,-2335,2230,1901,1871,-2077,-1290,-1155,-1265},
        {-1290,-1155,-1265,-2083,1871,-2160,-2224,2077,-2482,1444,1582,1760,-1865,1901,1871,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,-2183,1837,-2183,-1740,2213,-2367,1567,2041,1493,-1536,1912,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,-1917,1423,-1745,-1597,2470,-2347,2000,2003,2172,-1853,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-1935,-1871,2018,-1613,-1444,2513,-1760,1865,2193,2224,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,-1912,-1837,-1912,-2355,1882,-1728,-1567,2054,-1493,1536,2183,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,-1917,-1853,-1745,-1597,2470,-2347,-1275,2003,-1103,1423,1745,1278,-1976,1878}
    },
    {
        {1249,-1384,-1480,-2076,1874,1706,-1874,2076,2331,1528,1336,2221,-1706,-2221,1706,-1874,1249,-1384,-1480},
        {-1269,1153,-1269,-2365,-1903,2047,1903,-1730,1413,2480,1730,1903,2047,-1903,-2047,1903,-1269,1153,-1269},
        {1269,-1153,1269,-1730,-2192,-2047,2192,1730,-1413,1615,2365,2192,2047,1903,-2047,-1903,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,-2192,2365,1413,-1615,1730,1903,2047,2192,2047,-2192,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,-2019,1648,1528,-1394,2221,1706,1874,2389,2221,-1480,-1384,1249},
        {1249,-1384,-1480,2019,-2221,1706,-1874,-2019,-1764,1528,1336,-1874,2389,1874,1706,2221,1249,-1384,-1480},
        {1480,1384,-1249,-2019,2221,-2389,1874,-2076,-1648,-1528,1394,1874,-1706,2221,1706,1874,1480,1384,-1249},
        {1499,1615,1499,-1672,-1846,2047,-2249,1672,-1297,-1442,-1672,1846,2047,-1846,2047,1846,1499,1615,1499}
    },
    {
        {1269,-1153,1269,-1730,-2192,2047,2192,1730,2365,1615,-1413,1903,2047,-1903,-2047,-2192,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,1903,2365,1730,2480,1413,-1903,2047,1903,-2047,-1903,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,2076,1336,1528,2331,1874,-1706,2221,1706,-2221,-1480,-1384,1249},
        {-1499,-1615,-1499,1672,-2249,2047,-1846,-1672,1672,1442,1297,1846,2047,-1846,2047,1846,-1499,-1615,-1499},
        {1249,-1384,-1480,-2076,1874,-2389,2221,-2019,-1394,1528,1648,1874,1706,2221,-1706,1874,1249,-1384,-1480},
        {1480,1384,-1249,-2019,-1874,1706,-2221,2019,-1336,-1528,1764,2221,1706,1874,2389,-1874,1480,1384,-1249},
        {-1249,1384,1480,-2019,-1874,-1706,1874,-2076,1394,-1528,-1648,2221,2389,1874,1706,2221,-1249,1384,1480},
        {1269,-1153,1269,2365,-2192,-2047,-1903,1730,-1730,1615,-1413,-2192,2047,2192,2047,1903,1269,-1153,1269}
    },
    {
        {-1269,-1269,-1153,2192,-2047,-1903,2047,1903,1413,-2365,1615,-1730,2192,1730,-2192,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,2047,-1903,-2047,1903,1413,1730,-2480,2365,-1903,1730,1903,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,-2047,2192,-2047,-2192,1413,1730,1615,-1730,2192,-2365,1903,2047,-1269,-1269,-1153},
        {1480,-1249,-1384,-1874,-1706,-1874,1706,-2221,-1648,1394,1528,2019,-1874,2076,-2221,2389,1480,-1249,-1384},
        {1499,1499,-1615,-1846,-2047,-1846,-2047,1846,-1297,-1672,1442,1672,2249,-1672,1846,-2047,1499,1499,-1615},
        {-1480,1249,1384,-2221,-2389,-2221,-1706,-1874,1648,-1394,-1528,2076,1874,2019,-1874,1706,-1480,1249,1384},
        {1249,-1480,1384,1874,-1706,-2221,-2389,-1874,-1764,1336,-1528,-2019,1874,2019,2221,-1706,1249,-1480,1384},
        {-1249,1480,-1384,2221,1706,-1874,-1706,-2221,-2331,-1336,1528,-2076,-1874,2076,1874,1706,-1249,1480,-1384}
    },
    {
        {-1480,1384,1249,1706,-1874,2389,1874,-1874,1648,-1528,-1394,-2221,2076,-2221,-2019,-1706,-1480,1384,1249},
        {-1249,-1384,1480,1706,1874,-1706,2221,1874,-2331,1528,-1336,-1874,-2076,2221,-2076,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,1903,2047,-1903,1903,1413,-2480,1730,-1903,-1730,-1903,2365,-2047,-1269,-1153,-1269},
        {-1269,-1153,-1269,-2047,1903,2047,2192,-2192,1413,1615,-2365,2192,-1730,-1903,-1730,2047,-1269,-1153,-1269},
        {1480,-1384,-1249,-1706,-2221,1706,2221,1874,-1648,1528,1394,-1874,2019,-1874,-2076,-2389,1480,-1384,-1249},
        {-1499,1615,-1499,-2047,-1846,-2047,1846,2249,1297,-1442,1672,1846,-1672,1846,-1672,-2047,-1499,1615,-1499},
        {-1249,-1384,1480,-2389,-2221,-1706,-1874,1874,1764,1528,-1336,2221,2019,-1874,2019,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,-2192,-2047,-1903,-2192,1413,1615,1730,-1903,2365,2192,-1730,2047,-1269,-1153,-1269}
    },
    // TODO : replace this vector with the vector for SCH
    {
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
        {-1010,1010,1010,1010,-1010,1010,1010,-1010,1010,-1010,-1010,-1010,1010,-1010,-1010,-1010,-1010,1010,1010},
    }
};

#endif
#else

#error "Channel Estimation Length Not Supported, modify value in your_target_chip.def\n"

#endif




// Puncturing tables
//------------------

//------- Puncture matrix ----------
//------------ TCH -------------
PRIVATE CONST UINT32 PUNCTURE_TCHHS[22]=
{
    // 312=32*9+24
    /* 1*/  0x6DB6DB6D,
    /* 2*/  0xDB6DB6DB,
    /* 3*/  0xB6DB6DB6,
    /* 4*/  0x6DB6DB6D,
    /* 5*/  0xDB6DB6DB,
    /* 6*/  0xB6DB6DB6,
    /* 7*/  0x6DB6DB6D,
    /* 8*/  0xDB6DB6DB,
    /* 9*/  0xF6DB6DB6,
    /*10*/  0xFFB6DB7F,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};
// ------ TCH data ---------------
// TCHF96, TCHH48: 11+15*j (j=0~31)
PRIVATE CONST UINT32 PUNCTURE_TCHF96H48[22]=
{
    //488=32*15+8
    /* 1*/  0xFBFFF7FF,
    /* 2*/  0xFEFFFDFF,
    /* 3*/  0xFFBFFF7F,
    /* 4*/  0xFFEFFFDF,
    /* 5*/  0xFFFBFFF7,
    /* 6*/  0x7FFEFFFD,
    /* 7*/  0xDFFFBFFF,
    /* 8*/  0xF7FFEFFF,
    /* 9*/  0xFDFFFBFF,
    /*10*/  0xFF7FFEFF,
    /*11*/  0xFFDFFFBF,
    /*12*/  0xFFF7FFEF,
    /*13*/  0xFFFDFFFB,
    /*14*/  0xBFFF7FFE,
    /*15*/  0xEFFFDFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};
// TCHF48, TCHH24 = CCH (15)
// TCHF24 = CCH (15)
// TCHF144: {18*j+1, 18*j+6, 18*j+11, 18*j+15}(j=0~31), 577, 582, 584, 587
PRIVATE CONST UINT32 PUNCTURE_TCHF144[22]=
{
    //588=32*18+12
    /* 1*/  0xDEF777BD,
    /* 2*/  0xEF777BDD,
    /* 3*/  0xF777BDDD,
    /* 4*/  0x777BDDDE,
    /* 5*/  0x77BDDDEF,
    /* 6*/  0x7BDDDEF7,
    /* 7*/  0xBDDDEF77,
    /* 8*/  0xDDDEF777,
    /* 9*/  0xDDEF777B,
    /*10*/  0xDEF777BD,
    /*11*/  0xEF777BDD,
    /*12*/  0xF777BDDD,
    /*13*/  0x777BDDDE,
    /*14*/  0x77BDDDEF,
    /*15*/  0x7BDDDEF7,
    /*16*/  0xBDDDEF77,
    /*17*/  0xDDDEF777,
    /*18*/  0xDDEF777B,
    /*19*/  0xFFFFF6BD,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

//---------- TCH/AFS ------------
PRIVATE CONST UINT32 PUNCTURE_AFS122[22]=
{
    // 508=32*16+28
    /* 1*/  0xFFFFFFFF,
    /* 2*/  0xFFFFFFFF,
    /* 3*/  0xFFFFFFFF,
    /* 4*/  0xFFFFFFFF,
    /* 5*/  0xFFFFFFFF,
    /* 6*/  0xFFFFFFFF,
    /* 7*/  0xFFFFFFFF,
    /* 8*/  0xFFFFFFFF,
    /* 9*/  0xFFFFFFFF,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xDDDDDDDD,
    /*12*/  0xD5DDD5DD,
    /*13*/  0xD5DDD5DD,
    /*14*/  0xD5DDD5DD,
    /*15*/  0xD5DDD5DD,
    /*16*/  0xF55555DD,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};
PRIVATE CONST UINT32 PUNCTURE_AFS102[22]=
{
    // 642=32*20+2
    /* 1*/  0x6FB6FB6D,
    /* 2*/  0xFB6FB6FB,
    /* 3*/  0xB6FB6FB6,
    /* 4*/  0x6FB6FB6F,
    /* 5*/  0xFB6FB6FB,
    /* 6*/  0xB6FB6FB6,
    /* 7*/  0x6FB6FB6F,
    /* 8*/  0xFB6FB6FB,
    /* 9*/  0xB6FB6FB6,
    /*10*/  0x6FB6FB6F,
    /*11*/  0xDB6DB6DB,
    /*12*/  0xB6DB6DB6,
    /*13*/  0x6DB6DB6D,
    /*14*/  0xDB6DB6DB,
    /*15*/  0xB6DB6DB6,
    /*16*/  0x6DB6DB6D,
    /*17*/  0xDB6DB6DB,
    /*18*/  0xB6DB6DB6,
    /*19*/  0x6DB6DB6D,
    /*20*/  0x496596D9,
    /*21*/  0xFFFFFFFE,
    /*22*/  0xFFFFFFFF
};
PRIVATE CONST UINT32 PUNCTURE_AFS795[22]=
{
    // 513=32*16+1
    /* 1*/  0xFFBFFEC9,
    /* 2*/  0xFFFFFFFF,
    /* 3*/  0xFFFFFFBF,
    /* 4*/  0xFFBFFFFF,
    /* 5*/  0xFFFFFFFF,
    /* 6*/  0xFFFFFFBF,
    /* 7*/  0xFFBFFFFF,
    /* 8*/  0xFFFFFFFF,
    /* 9*/  0xFFFFFFBF,
    /*10*/  0x5FBFFFFF,
    /*11*/  0xDF5FAFDF,
    /*12*/  0xAFDF5FAF,
    /*13*/  0x5DAFDF5D,
    /*14*/  0xDF5DAFDF,
    /*15*/  0xAFDF5DAF,
    /*16*/  0x492FDB5D,
    /*17*/  0xFFFFFFFE,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AFS74[22]=
{
    // 474=32*14+26
    /* 1*/  0xFFFFFFFE,
    /* 2*/  0xFFFFFFFF,
    /* 3*/  0xFFFFFFFF,
    /* 4*/  0xFFFFFFFF,
    /* 5*/  0xFFFFFFFF,
    /* 6*/  0xFFFFFFFF,
    /* 7*/  0xFFFFFFFF,
    /* 8*/  0xFFFFFFFF,
    /* 9*/  0xFFFFFFFF,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xF7DF7DF7,
    /*13*/  0x7DF7DF7D,
    /*14*/  0xDF7DF7DF,
    /*15*/  0xFE4B6DF7,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AFS67[22]=
{
    // 576=32*18+0
    /* 1*/  0xF7FF7775,
    /* 2*/  0xFF7FFF7F,
    /* 3*/  0x7FFF7FF7,
    /* 4*/  0xFF7FF7FF,
    /* 5*/  0x7FF7FF7F,
    /* 6*/  0xF7FF7FFF,
    /* 7*/  0xFF7FFF7F,
    /* 8*/  0x7FFF7FF7,
    /* 9*/  0x7F7FF7FF,
    /*10*/  0x77777777,
    /*11*/  0x77777777,
    /*12*/  0x75757777,
    /*13*/  0x75757575,
    /*14*/  0x75757575,
    /*15*/  0x75757575,
    /*16*/  0x75757575,
    /*17*/  0x75757575,
    /*18*/  0x55555555,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AFS59[22]=
{
    // 520=32*16+8
    /* 1*/  0x7FFF7754,
    /* 2*/  0x7FFF7FFF,
    /* 3*/  0x7FFF7FFF,
    /* 4*/  0x7FFF7FFF,
    /* 5*/  0x7FFF7FFF,
    /* 6*/  0x7FFF7FFF,
    /* 7*/  0x7FFF7FFF,
    /* 8*/  0x7FFF7FFF,
    /* 9*/  0x7FFF7FFF,
    /*10*/  0x7FFF7FFF,
    /*11*/  0x777F777F,
    /*12*/  0x777F777F,
    /*13*/  0x777F777F,
    /*14*/  0x777F777F,
    /*15*/  0x7777777F,
    /*16*/  0x57777777,
    /*17*/  0xFFFFFF44,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AFS515[22]=
{
    //565=32*17+21
    /* 1*/  0xBDEF39CE,
    /* 2*/  0xEFFBFEF7,
    /* 3*/  0xFBFEFFBF,
    /* 4*/  0xFEFFBFEF,
    /* 5*/  0xFFBFEFFB,
    /* 6*/  0xBFEFFBFE,
    /* 7*/  0xEFFBFEFF,
    /* 8*/  0xFBFEFFBF,
    /* 9*/  0xFEFFBFEF,
    /*10*/  0xF7BFEFFB,
    /*11*/  0xBCEF3BDE,
    /*12*/  0xEF3BCEF3,
    /*13*/  0x3BCEF3BC,
    /*14*/  0xCEF3BCEF,
    /*15*/  0xF3BCEF3B,
    /*16*/  0xBCEF3BCE,
    /*17*/  0xE739CEF3,
    /*18*/  0xFFEE739C,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AFS475[22]=
{
    // 535=32*16+23
    /* 1*/  0xFDFF7D48,
    /* 2*/  0xFF7FDFF7,
    /* 3*/  0x7FDFF7FD,
    /* 4*/  0xDFF7FDFF,
    /* 5*/  0xF7FDFF7F,
    /* 6*/  0xFDFF7FDF,
    /* 7*/  0xFF7FDFF7,
    /* 8*/  0x7FDFF7FD,
    /* 9*/  0xDFF7FDFF,
    /*10*/  0xF7FDFF7F,
    /*11*/  0xFDFF7FDF,
    /*12*/  0xFF7FDFF7,
    /*13*/  0x7BDEF7FD,
    /*14*/  0xDEF7BDEF,
    /*15*/  0x77BDE77B,
    /*16*/  0x9DE77BDE,
    /*17*/  0xFFA10A57,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};


// AFS_RATSCCH = AFS_SID_UPDATE (7)

// TCH/AHS
// AHS_SID_UPDATE = AFS_SID_UPDATE (7)
PRIVATE CONST UINT32 PUNCTURE_AHS795[22]=
{
    // 266=32*8+10
    /* 1*/  0x77777755,
    /* 2*/  0x777777F7,
    /* 3*/  0x77777F77,
    /* 4*/  0x7777F777,
    /* 5*/  0x777F7777,
    /* 6*/  0x75757777,
    /* 7*/  0x57575755,
    /* 8*/  0x75757557,
    /* 9*/  0xFFFFFD55,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AHS74[22]=
{
    // 260=32*8+4
    /* 1*/  0xF777F775,
    /* 2*/  0xF777F777,
    /* 3*/  0xF777F777,
    /* 4*/  0xF777F777,
    /* 5*/  0x77777777,
    /* 6*/  0x77777777,
    /* 7*/  0x57777777,
    /* 8*/  0x57575757,
    /* 9*/  0xFFFFFFF5,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AHS67[22]=
{
    // 240=32*7+16
    /* 1*/  0xDFF7FDF5,
    /* 2*/  0xF7FDFF7F,
    /* 3*/  0xFDFF7FDF,
    /* 4*/  0xFF7FDFF7,
    /* 5*/  0x7FDFF7FD,
    /* 6*/  0xD7F5FD7F,
    /* 7*/  0x75DD775F,
    /* 8*/  0xFFFF5557,
    /* 9*/  0xFFFFFFFF,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AHS59[22]=
{
    // 224=32*7
    /* 1*/  0xFFFF7FFD,
    /* 2*/  0xFFFFFFFF,
    /* 3*/  0xFFFFFF7F,
    /* 4*/  0x7FFFFFFF,
    /* 5*/  0xFF7FF7FF,
    /* 6*/  0xF7FF7FF7,
    /* 7*/  0x5777F7F7,
    /* 8*/  0xFFFFFFFF,
    /* 9*/  0xFFFFFFFF,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AHS515[22]=
{
    // 303=32*9+15
    /* 1*/  0xF7DB6DA4,
    /* 2*/  0x7DB7DF7D,
    /* 3*/  0xDB7DF7DF,
    /* 4*/  0xB7DF7DF7,
    /* 5*/  0x7DF7DF7D,
    /* 6*/  0xDB6DB6DB,
    /* 7*/  0xB6DB6DB6,
    /* 8*/  0x6DA6DB6D,
    /* 9*/  0xDA6DA6DA,
    /*10*/  0xFFFFA934,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

PRIVATE CONST UINT32 PUNCTURE_AHS475[22]=
{
    // 285=32*8+29
    /* 1*/  0xEFBEDA49,
    /* 2*/  0xFBEFBEFB,
    /* 3*/  0xBEFBEFBE,
    /* 4*/  0xEFBEFBEF,
    /* 5*/  0xFB6FBEFB,
    /* 6*/  0xB6FB6FB6,
    /* 7*/  0x6FB6FB6F,
    /* 8*/  0xDB6DB6FB,
    /* 9*/  0xE4936DB6,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};

// AHS_RATSCCH_DATA = AFS_SID_UPDATE (7)

// PDTCHCOCONSTNST
// PTCCH = CCH (15)
PRIVATE CONST UINT32 PUNCTURE_PRACH[22]=
{
    // 42=32*1+10
    /* 1*/  0xFFFFFFDA,
    /* 2*/  0xFFFFFD5F,
    /* 3*/  0xFFFFFFFF,
    /* 4*/  0xFFFFFFFF,
    /* 5*/  0xFFFFFFFF,
    /* 6*/  0xFFFFFFFF,
    /* 7*/  0xFFFFFFFF,
    /* 8*/  0xFFFFFFFF,
    /* 9*/  0xFFFFFFFF,
    /*10*/  0xFFFFFFFF,
    /*11*/  0xFFFFFFFF,
    /*12*/  0xFFFFFFFF,
    /*13*/  0xFFFFFFFF,
    /*14*/  0xFFFFFFFF,
    /*15*/  0xFFFFFFFF,
    /*16*/  0xFFFFFFFF,
    /*17*/  0xFFFFFFFF,
    /*18*/  0xFFFFFFFF,
    /*19*/  0xFFFFFFFF,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};
// CS1 = CCH (15)
PRIVATE CONST UINT32 PUNCTURE_CS2[22]=
{
    // 588=32*18+12
    /* 1*/  0x77777FFF,
    /* 2*/  0x777777F7,
    /* 3*/  0x77F77777,
    /* 4*/  0x77777777,
    /* 5*/  0x777777F7,
    /* 6*/  0x77F77777,
    /* 7*/  0x77777777,
    /* 8*/  0x777777F7,
    /* 9*/  0x77F77777,
    /*10*/  0x77777777,
    /*11*/  0x777777F7,
    /*12*/  0x77F77777,
    /*13*/  0x77777777,
    /*14*/  0x777777F7,
    /*15*/  0x77F77777,
    /*16*/  0x77777777,
    /*17*/  0x777777F7,
    /*18*/  0x77F77777,
    /*19*/  0xFFFFF777,
    /*20*/  0xFFFFFFFF,
    /*21*/  0xFFFFFFFF,
    /*22*/  0xFFFFFFFF,
};
PRIVATE CONST UINT32 PUNCTURE_CS3[22]=
{
    // 676=32*21+4
    /* 1*/  0xD75D7FFF,
    /* 2*/  0x75D75D75,
    /* 3*/  0x5D75D75D,
    /* 4*/  0xD75D75D7,
    /* 5*/  0x75D75D75,
    /* 6*/  0x5D75D75D,
    /* 7*/  0xD75D75D7,
    /* 8*/  0x75D75D75,
    /* 9*/  0x5D75D75D,
    /*10*/  0xD75D75D7,
    /*11*/  0x75D75D75,
    /*12*/  0x5D75D75D,
    /*13*/  0xD75D75D7,
    /*14*/  0x75D75D75,
    /*15*/  0x5D75D75D,
    /*16*/  0xD75D75D7,
    /*17*/  0x75D75D75,
    /*18*/  0x5D75D75D,
    /*19*/  0xD75D75D7,
    /*20*/  0x75D75D75,
    /*21*/  0x5D75D75D,
    /*22*/  0xFFFFFFFF,
};


#define PUNCTURE_HEAD_1 NULL
#define PUNCTURE_HEAD_3 NULL
#define PUNCTURE_MCS1   NULL
#define PUNCTURE_MCS2   NULL
#define PUNCTURE_MCS3   NULL
#define PUNCTURE_MCS4   NULL
#define PUNCTURE_MCS5   NULL
#define PUNCTURE_MCS6   NULL
#define PUNCTURE_MCS7   NULL
#define PUNCTURE_MCS8   NULL
#define PUNCTURE_MCS9   NULL






PROTECTED CONST UINT32* CONST g_sppCsPuncturingTable[CS_PARAM_QTY] =
{
    NULL,               // RACH
    PUNCTURE_PRACH,     // PRACH
    NULL,               // SCH
    NULL,               // CS1
    PUNCTURE_CS2,       // CS2
    PUNCTURE_CS3,       // CS3
    NULL,               // CS4
    PUNCTURE_HEAD_1,    // HEAD_1
    NULL,               // HEAD_2
    PUNCTURE_HEAD_3,    // HEAD_3
    PUNCTURE_MCS1,      // MCS1
    PUNCTURE_MCS2,      // MCS2
    PUNCTURE_MCS3,      // MCS3
    PUNCTURE_MCS4,      // MCS4
    PUNCTURE_MCS5,      // MCS5
    PUNCTURE_MCS6,      // MCS6
    PUNCTURE_MCS7,      // MCS7
    PUNCTURE_MCS8,      // MCS8
    PUNCTURE_MCS9,      // MCS9
    PUNCTURE_TCHHS,     // HR
    NULL,               // FR
    NULL,               // EFR
    NULL,               // H24
    PUNCTURE_TCHF96H48, // H48
    NULL,               // F24
    NULL,               // F48
    PUNCTURE_TCHF96H48, // F96
    PUNCTURE_TCHF144,   // F144
    PUNCTURE_AFS122,    // CS_AFS_SPEECH_122
    PUNCTURE_AFS102,    // CS_AFS_SPEECH_102
    PUNCTURE_AFS795,    // CS_AFS_SPEECH_795
    PUNCTURE_AFS74,     // CS_AFS_SPEECH_74
    PUNCTURE_AFS67,     // CS_AFS_SPEECH_67
    PUNCTURE_AFS59,     // CS_AFS_SPEECH_59
    PUNCTURE_AFS515,    // CS_AFS_SPEECH_515
    PUNCTURE_AFS475,    // CS_AFS_SPEECH_475
    PUNCTURE_AHS795,    // CS_AHS_SPEECH_795
    PUNCTURE_AHS74,     // CS_AHS_SPEECH_74
    PUNCTURE_AHS67,     // CS_AHS_SPEECH_67
    PUNCTURE_AHS59,     // CS_AHS_SPEECH_59
    PUNCTURE_AHS515,    // CS_AHS_SPEECH_515
    PUNCTURE_AHS475,    // CS_AHS_SPEECH_475
};


































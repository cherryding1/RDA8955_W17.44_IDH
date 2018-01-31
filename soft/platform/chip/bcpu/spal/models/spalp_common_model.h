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



#ifndef __ESPALP_COMMON_MODEL_H
#define __ESPALP_COMMON_MODEL_H


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define HEAD_COD        0
#define DATA_COD        1


#define MOVEBITESPAL(a, i, b, j)  b[(j)/8] |= ((!!(a[(i)/8]&1<<((i)%8)))<<((j)%8))
#define CLRBITESPAL(b, j)         b[(j)/8] &= (~(1<<((j)%8)))
#define SETBITESPAL(b, j)         b[(j)/8] |= ( (1<<((j)%8)))
#define ISBITESPAL(b, j)      (!!(b[(j)/8] &  ( (1<<((j)%8)))))
#define MOVEBIT(a, i, b, j)      (b)[(j)/8]|=((!!((a)[(i)/8]&1<<((i)%8)))<<((j)%8))
#define CLEARBIT(b, j)            b[(j)/8]&=(~(1<<((j)%8)))
#define MOVEBIT2(a, i, b, k, j)   b[(k)+((j)/8)]|=((!!(a[(i)/8]&1<<((i)%8)))<<((j)%8))
#define CLEARBIT2(b, i, j)        b[(i)+((j)/8)]&=(~(1<<((j)%8)))


typedef enum
{
    ITLV_TYPE_1A                                = 0x00000000,
    ITLV_TYPE_1B                                = 0x00000001,
    ITLV_TYPE_1C                                = 0x00000002,
    ITLV_TYPE_2A                                = 0x00000003,
    ITLV_TYPE_2B                                = 0x00000004,
    ITLV_TYPE_3                                 = 0x00000005,
    ITLV_TYPE_H1                                = 0x00000006,
    ITLV_TYPE_H2                                = 0x00000007,
    ITLV_TYPE_H3                                = 0x00000008,
    ITLV_TYPE_H4                                = 0x00000009,
    ITLV_TYPE_D1                                = 0x0000000A,
    ITLV_TYPE_D2                                = 0x0000000B,
    ITLV_TYPE_D3                                = 0x0000000C
} SPAL_ITLV_TYPE_T;


typedef enum
{
    CS_RACH                                     = 0x00000000,
    CS_PRACH                                    = 0x00000001,
    CS_SCH                                      = 0x00000002,
    CS_CS1                                      = 0x00000003,
    CS_CS2                                      = 0x00000004,
    CS_CS3                                      = 0x00000005,
    CS_CS4                                      = 0x00000006,
    CS_HEAD_1                                   = 0x00000007,
    CS_HEAD_2                                   = 0x00000008,
    CS_HEAD_3                                   = 0x00000009,
    CS_MCS1                                     = 0x0000000A,
    CS_MCS2                                     = 0x0000000B,
    CS_MCS3                                     = 0x0000000C,
    CS_MCS4                                     = 0x0000000D,
    CS_MCS5                                     = 0x0000000E,
    CS_MCS6                                     = 0x0000000F,
    CS_MCS7                                     = 0x00000010,
    CS_MCS8                                     = 0x00000011,
    CS_MCS9                                     = 0x00000012,
    CS_HR                                       = 0x00000013,
    CS_FR                                       = 0x00000014,
    CS_EFR                                      = 0x00000015,
    CS_H24                                      = 0x00000016,
    CS_H48                                      = 0x00000017,
    CS_F24                                      = 0x00000018,
    CS_F48                                      = 0x00000019,
    CS_F96                                      = 0x0000001A,
    CS_F144                                     = 0x0000001B,
    CS_AFS_SPEECH_122                           = 0x0000001C,
    CS_AFS_SPEECH_102                           = 0x0000001D,
    CS_AFS_SPEECH_795                           = 0x0000001E,
    CS_AFS_SPEECH_74                            = 0x0000001F,
    CS_AFS_SPEECH_67                            = 0x00000020,
    CS_AFS_SPEECH_59                            = 0x00000021,
    CS_AFS_SPEECH_515                           = 0x00000022,
    CS_AFS_SPEECH_475                           = 0x00000023,
    CS_AHS_SPEECH_795                           = 0x00000024,
    CS_AHS_SPEECH_74                            = 0x00000025,
    CS_AHS_SPEECH_67                            = 0x00000026,
    CS_AHS_SPEECH_59                            = 0x00000027,
    CS_AHS_SPEECH_515                           = 0x00000028,
    CS_AHS_SPEECH_475                           = 0x00000029,
    CS_AFS_SID_UPDATE                           = 0x0000002A,
    CS_AFS_RATSCCH                              = 0x0000002B,
    CS_AHS_SID_UPDATE                           = 0x0000002C,
    CS_AHS_RATSCCH_DATA                         = 0x0000002D,
    CS_AFS_SID_FIRST                            = 0x0000002E,
    CS_AFS_ONSET                                = 0x0000002F,
    CS_AHS_SID_UPDATE_INH                       = 0x00000030,
    CS_AHS_SID_FIRST_P1                         = 0x00000031,
    CS_AHS_SID_FIRST_P2                         = 0x00000032,
    CS_AHS_SID_FIRST_INH                        = 0x00000033,
    CS_AHS_ONSET                                = 0x00000034,
    CS_AHS_RATSCCH_MARKER                       = 0x00000035,
    CS_QTY                                      = 0x00000036
} SPAL_CODING_SCHEME_T;

/// Offset for the SRC tables
#define CS_NEED_SRC                              (CS_AFS_SPEECH_122)
/// Offset for Post SRC tables
#define CS_END_NEED_SRC                          (CS_AFS_SID_FIRST)
/// size of the CRC/CONV table
#define CS_PARAM_QTY                             (CS_END_NEED_SRC)
/// size of the SRC table
#define CS_SRC_PARAM_QTY                         (CS_END_NEED_SRC - CS_NEED_SRC)


PROTECTED BOOL spal_CfgIsUL (VOID);

// TODO : to be removed
EXPORT PROTECTED CONST UINT32* CONST g_sppCsPuncturingTable[CS_END_NEED_SRC];


#endif  // __ESPALP_COMMON_MODEL_H

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


#ifndef __CALIB_STUB_NST_H__
#define __CALIB_STUB_NST_H__

#include "cs_types.h"
#include "hal_speech.h"
// =============================================================================
//  MACROS
// =============================================================================
#define L1S_SIZ_RSSI 548

#define SL1_NO_MEAS  ((u8)0)
#define SL1_GET_MEAS ((u8)(1 << 0))
#define SL1_MEAS_SUB ((u8)(1 << 1))

#define SL1_NB_MEAS_SERVING                       (16)
#define SL1_NB_MEAS_SERVING_MSK  (SL1_NB_MEAS_SERVING - 1)
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,
#define CURPOS CurrOfs
#define PUSH_INT(val, bits) do {EDIntToBits (Buffer, CURPOS, val, bits); CURPOS+=bits;} while (0)
// =============================================================================
//  TYPES
// =============================================================================

/*
   ================================================================================
Primitive  : nst_MonitorInd_t
--------------------------------------------------------------------------------

Scope      : nst gives measurement result.
This structure is used in NST mesurement:
- in Idle/Transfer for Cell reselection
- in Connected mode for measurement report
- in Connected mode for extended measurement report

================================================================================
*/
typedef struct
{
    u8         MonNb              ;   // Number of elem in list
    bool       Tag                ;   // Discard MonitorInd if diff Tag
    u8         RlaL [6]  ;   // List of Rla
    u8         RxLevFull          ;   // RxLev  Full of Main
    u8         RxLevSub           ;   // RxLev  Sub  of Main
    u8         RlaVal             ;   // Rla in absolute dBm for RxLevVal computation in enhanced measurement
    u8         RxQualFull         ;   // RxQual Full of Main
    u8         RxQualSub          ;   // RxQual Sub  of Main
    u8         NbrRcvdBlocks      ;   // For enhanced measurement, see 05.08 8.4.8.2
    u8         MeanBEP            ;   // Mean Bit Error Probability [MEAN_BEP_0..MEAN_BEP_31]
    u8         CvBEP              ;   // Coefficient of variation of the Bit Error Probability [CV_BEP0..CV_BEP7].
    bool       DtxUsed            ;   // Dtx has been used or not
    bool       ExtendedMeas       ;   // Extended measurement report when TRUE.
    u8         TA                 ;   // Time Advice value of Main, 0~63, 0xFF is unvalid
} nst_MonitorInd_t ;



typedef enum
{
    CALIB_NST_STATUS_SUCCESS = 0,
    CALIB_NST_STATUS_FAILED
} CALIB_NST_STATUS_T;


/**********************************************
 * NST STATE
 *
 **********************************************/
typedef enum
{
    NST_NO_STATE,
    NST_IDLE_STATE,
    NST_POWER_STATE,
    NST_SYNC_STATE,
    NST_OFFSET_STATE,
    NST_RX_STATE,
    NST_ACQUIRE_STATE,
    NST_BER_CACULATION_STATE,
    NST_STOP_RX_SYNC_STATE,
    NST_SIMU_LOG_STATE,
    NST_DISCONN_STATE,
} CALIB_NST_STAT_T;

/**********************************************
 * NST SYNCRONIZATION PASE
 *
 **********************************************/
typedef enum
{
    NST_SYNC_SCH_FOUND,
    NST_SYNC_DECODE_DONE
} CALIB_NST_SYNC_PHASE_T;


/**********************************************
 * initial acquisition step
 *
 **********************************************/

#define FOREACH_FSM_STATE(STATE) \
    STATE(ACQUIRE_INIT)   \
STATE(ACQUIRE_VERIFY)  \
STATE(ACQUIRE_DONE)   \
STATE(ACQUIRE_ABORT)  \

typedef enum
{
    FOREACH_FSM_STATE(GENERATE_ENUM)
} ACQUIRE_FSM_STAT_T;

/**********************************************
 * PRBS context
 *
 **********************************************/
typedef struct
{
    UINT16 poly;
    UINT16 RegLen;
    UINT8 ber_mode;
    UINT8 invert;
    UINT8 acq_timeout;
    UINT16 acq_threshold;
    UINT16 acq_lost_threshold;
    UINT16 *array;
    UINT16 *array2pos;
    UINT32 PosRtAligned;
    UINT32 RegRtAligned;
} prbs_ctx_t;

/**********************************************
 * Acquire general output fsm structure
 *
 **********************************************/
typedef struct
{
    UINT32 state; //current state in acquire_fsm
    UINT32 offset; //offset recorded in step ACQUIRE_INIT
    UINT32 seed; //seed recorded in step ACQUIRE_INIT
} acquire_fsm_t;

typedef struct
{
    u16         Arfcn ;   // [0..1024]
    u16         Rssi  ;   // [0..120] dBm after L1_LOW_RLA  it's noise only

} NstArfRssi_t ;

typedef struct
{
    UINT16         Nb                ;  // Number of Elem in Table
    UINT16         Index;
    NstArfRssi_t   List[L1S_SIZ_RSSI] ;  // List of (Arfcn, Measure)

} NstPowerReq_t;

typedef struct
{
    u8  NbMeas;           // Nb Measures held in current MeasFull accumulator
    u8  NbMeasSub;        // Nb Measures held in current MeasSub accumulator
    u16 MeasCumul;        // Measure full accumulator.
    u16 MeasCumulSub;     // Measure Sub accumulator, Connected state only.

    u16 TotalBit;         // Number of received bits accumulator for RxQualFull calculation.
    u16 TotalBitSub;      // Number of received bits accumulator for RxQualSub calculation.
    u16 WrongBitCumul;    // Number of wrong bits received accumultor for RxQualFull calculation.
    u16 WrongBitCumulSub; // Number of wrong bits received accumultor for RxQualSub calculation.

    u8         RxLevFull          ;   // RxLev  Full of Main
    u8         RxLevSub           ;   // RxLev  Sub  of Main
    u8         RlaVal             ;   // Rla in absolute dBm for RxLevVal computation in enhanced measurement
    u8         RxQualFull         ;   // RxQual Full of Main
    u8         RxQualSub          ;   // RxQual Sub  of Main

} Nst_MeasCfg_t;


typedef struct
{
    VOLATILE UINT32 NstState;
    VOLATILE UINT32 NstReqState;
    VOLATILE UINT32 NstReqCurrState;
    VOLATILE UINT8 sync_numerrors;
    VOLATILE UINT8 loopback;
    VOLATILE UINT8 syncPhase;
    VOLATILE UINT8 codec;
    VOLATILE UINT8 order;
    VOLATILE UINT8 logEnable;
    VOLATILE UINT8 num_samples;
    VOLATILE u32 measuretimes;
    VOLATILE u32 num_bits;
    VOLATILE u32 num_errors;
    VOLATILE u8 pn_type;
    VOLATILE u8 ber_mode;
    VOLATILE u8 invert;
    VOLATILE u8 pad;
    VOLATILE u16 acq_threshold;
    VOLATILE u16 acq_lost_threshold;
    VOLATILE u8 rspcId;
    VOLATILE u8 status;
    VOLATILE u16 bcchArfcn;
    VOLATILE u16 tchArfcn;
    VOLATILE u8 band;
    VOLATILE u8 monPower;
    VOLATILE u8 nbPower;
    VOLATILE u8 txPcl;
    VOLATILE u8 tsc;
    VOLATILE u8 tn;
    prbs_ctx_t prbs;
    NstPowerReq_t  powerReq;
    Nst_MeasCfg_t ServMeasCfg;
    UINT8 ReorderOut[260];
    UINT8 PrbsOut[260];
    HAL_SPEECH_DEC_IN_T speech_dec;
    HAL_SPEECH_ENC_OUT_T speech_enc;
} CALIB_STUB_NST_CTX_T;

// =============================================================================
//  FUNCTIONS
// =============================================================================

/*
   ==============================================================================
Function   :calib_NstParse
----------------------------------------------------------------------------
Scope      :nst stub message parser
Parameters :
Return     : None.
==============================================================================
*/
VOID calib_StubNstParse();

/*
   ==============================================================================
Function   :calib_SetNststateIdle
----------------------------------------------------------------------------
Scope      :
Parameters :
Return     : None.
==============================================================================
*/
void calib_SetNststateIdle();
/*
   ==============================================================================
Function   :calib_GetReqCurrState
----------------------------------------------------------------------------
Scope      :
Parameters :
Return     : None.
==============================================================================
*/
UINT32 calib_GetReqCurrState();
/*
   ==============================================================================
Function   :calib_SetReqCurrState
----------------------------------------------------------------------------
Scope      :
Parameters :
Return     : None.
==============================================================================
*/
VOID calib_SetReqCurrState(UINT32 state);
/*
   ==============================================================================
Function   :calib_GetReqState
----------------------------------------------------------------------------
Scope      :
Parameters :
Return     : None.
==============================================================================
*/
UINT32 calib_GetReqState();
/*
   ==============================================================================
Function   :calib_StubNstOpen
----------------------------------------------------------------------------
Scope      :nst stub open
Parameters :
Return     : None.
==============================================================================
*/

VOID calib_StubNstOpen();

/*
   ==============================================================================
Function   :calib_StubNstProcess
----------------------------------------------------------------------------
Scope      :general nst processing stub
Parameters :
Return     : None.
==============================================================================
*/
VOID calib_StubNstProcess();
#endif


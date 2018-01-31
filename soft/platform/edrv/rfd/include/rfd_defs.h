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

















#ifndef _RFD_DEFS_H
#define _RFD_DEFS_H

#include "gsm.h"
#include "hal_tcu.h"

//--------------------------------------------------------------------//
//                                                                    //
//               Memory Location definitions for                      //
//                time-critical text and data                         //
//                                                                    //
//--------------------------------------------------------------------//
#define RFD_FUNC_INTERNAL            __attribute__((section(".sramtext")))
#define RFD_DATA_INTERNAL            __attribute__((section(".sramdata")))
#define RFD_UNCACHED_DATA_INTERNAL   __attribute__((section(".sramucdata")))

//--------------------------------------------------------------------//
//                                                                    //
//                       ARFCN limits                                 //
//                                                                    //
//--------------------------------------------------------------------//
// GSM
#define RFD_ARFCN_GSM850_MIN         128
#define RFD_ARFCN_GSM850_MAX         251
// EGSM RX
#define RFD_ARFCN_EGSM_RX_MIN1       0
#define RFD_ARFCN_EGSM_RX_MAX1       124
#define RFD_ARFCN_EGSM_RX_MIN2       975
#define RFD_ARFCN_EGSM_RX_MAX2       1023
// EGSM TX
#define RFD_ARFCN_EGSM_TX_MIN1_1     0
#define RFD_ARFCN_EGSM_TX_MAX1_1     24
#define RFD_ARFCN_EGSM_TX_MIN1_2     50
#define RFD_ARFCN_EGSM_TX_MAX1_2     124
#define RFD_ARFCN_EGSM_TX_MIN2       25
#define RFD_ARFCN_EGSM_TX_MAX2       49
#define RFD_ARFCN_EGSM_TX_MIN3       975
#define RFD_ARFCN_EGSM_TX_MAX3       1023
// DCS
#define RFD_ARFCN_DCS_MIN            512
#define RFD_ARFCN_DCS_MAX            885
// PCS
#define RFD_ARFCN_PCS_MIN            512
#define RFD_ARFCN_PCS_MAX            810

//--------------------------------------------------------------------//
//                                                                    //
//                       AGC Related Values                           //
//                                                                    //
//--------------------------------------------------------------------//
#define RFD_AGC                 0xff
// To Do Decide
//-12 -13 -14dbm is Reference Input power level in RF interface
#define RF_Reference_Power -10

//--------------------------------------------------------------------//
//                                                                    //
//                      TYPES and ENUMs                               //
//                                                                    //
//--------------------------------------------------------------------//

/// @brief Flags for Rx/Tx Window selection
typedef enum
{
    RFD_DIR_RX, ///< Rx Window
    RFD_DIR_TX, ///< Tx Window
    RFD_DIR_RX_DOWN, ///< End of Rx Window
    RFD_DIR_TX_DOWN, ///< End of Tx Window

    RFD_DIR_QTY
} RFD_RFDIR_T;

/// @brief Flags for Tx ramp direction
typedef enum
{
    RFD_RAMP_UP, ///< Rising Ramp
    RFD_RAMP_DOWN, ///< Falling Ramp

    RFD_RAMP_QTY
} RFD_PA_RAMPDIR_T;

/// @brief Flags for AFC Calibration control
typedef enum
{
    RFD_AFC_BOUND_CENTER = 0, ///< Ask for AFC midrange value
    RFD_AFC_BOUND_NEG = 0x1, ///< Ask for AFC negative value
    RFD_AFC_BOUND_POS = 0x2, ///< Ask for AFC positive value

    RFD_AFC_BOUND_QTY
} RFD_AFC_BOUND_T;

typedef enum
{
    RFD_NO_DCOC = 0,
    RFD_HW_DCOC_ENABLED = 0x1,
    RFD_SW_DCOC_ENABLED = 0x2
} RFD_DCOC_SUPPORT_T;

/// Low Power modes
typedef enum
{
    /// Low power mode/setting. \n
    /// Module in normal power on state
    RFD_POWER_ON,
    /// Module's power supplies will not be shut down in this mode
    RFD_LOW_POWER,
    /// Ultra low power. \n Module's power supplies will be shut down in this mode
    /// (module Vdd switched off but not Vi/o)
    RFD_POWER_OFF,
    RFD_LP_MODE_QTY
} RFD_LP_MODE_T;

typedef enum
{
    RFD_DIGRF_NORM_CLK_POL = 0,
    RFD_DIGRF_INV_CLK_POL = 1
} RFD_DIGRF_CLKPOL_T;

typedef enum
{
    RFD_DIGRF_1_SAMPLE_PER_SYMB = 0,
    RFD_DIGRF_2_SAMPLE_PER_SYMB = 1
} RFD_DIGRF_OVERSAMP_T;

typedef enum
{
    RFD_DIGRF_TX_STREAM = 0,
    RFD_DIGRF_TX_BLOCK = 1
} RFD_DIGRF_TX_MODE_T;

typedef enum
{
    RFD_DIGRF_SAMPLE_ALIGN_MSB= 0,
    RFD_DIGRF_SAMPLE_ALIGN_LSB= 1
} RFD_DIGRF_RX_SAMPLE_ALIGNMENT_T;

typedef enum
{
    /// Normal behaviour: V_RF active high and low during low power.
    RFD_LDO_BEHAVIOR_NORMAL = 0,
    /// Always high behaviour: V_RF Active high and high during low power.
    RFD_LDO_BEHAVIOR_ALWAYS_HIGH,
    /// The V_RF from pmd is not used by the transceiver (no LDO).
    RFD_LDO_BEHAVIOR_DONT_USE,

    RFD_LDO_BEHAVIOR_QTY
} RFD_LDO_BEHAVIOR_T;

typedef enum
{
    RFD_TX_GMSK_MOD= 0,
    RFD_TX_8PSK_MOD= 1
} RFD_TX_MODULATION_T;

//======================================================================
/// RFD RF Window structure
/// This structure contains the window descriptor filled by PAL.
/// All fields are input fields except @b swapIQ that must be filled by
/// the Xcver's driver for each opened window.
//======================================================================
typedef struct
{
    ///  Window Type
    GSM_WIN_TYPE_T type;
    ///  Window start time in Qb
    INT32 date;
    ///  RF Channel number
    UINT16 arfcn;
#ifdef XCV_PLL_SDM_MODE
    INT16 fof;
#endif
    ///  RF Channel Band
    GSM_RFBAND_T band;

    /// @brief Window's slots Gain.
    ///  - for Rx windows : Expected power at antenna in -dBm
    ///  - for Tx windows : PCL power level */
    UINT8 gain[4];
    ///  TimeSlot bitmap allocation
    UINT8 bitmap;
    ///  TimeSlot bitmap modulation description
    UINT8 txModBitmap;
    ///  Index of window's first TCU event
    UINT8 firstTcu;
    ///  Index of window's last TCU event
    UINT8 lastTcu;
    ///  Index of the window
    UINT8 winIdx;
    ///  if TRUE, I and Q channels will be swapped for this window
    BOOL swapIQ;
    // Modulation Mode, 0 8psk, 1 gmsk
    GSM_MODULATION_T ModulMode;
#ifdef MULTI_BANDWIDE
    UINT8 bw;
#endif
} RFD_WIN_T;

//======================================================================
/// RFD RF Windows boundary times
/// This structure contains the windows setup and hold times that must
/// be respected for proper function.
/// PAL derives overall setup and holds from values returned by the
/// Xcver, the PA and the RF switch.
//======================================================================
typedef struct
{
    /// Time offset (from on-air bit[0]) of the earliest event
    /// constituting the Rx Window
    INT16   rxSetup;

    /// Time offset (from on-air bit[last]) of the latestest event
    /// constituting the Rx Window
    INT16   rxHold;

    /// Time offset (from on-air bit[0]) of the earliest event
    /// constituting the Tx Window
    INT16   txSetup;

    /// Time offset (from on-air bit[last]) of the latestest event
    /// constituting the Tx Window
    INT16   txHold;
} RFD_WIN_BOUNDS_T;

#define RF_CTRL0            TCO(0)
#define RF_CTRL1            TCO(1)
#define RF_CTRL2            TCO(2)
#define RF_CTRL3            TCO(3)
#define RF_CTRL4            TCO(4)
#define RF_CTRL5            TCO(5)
#define RF_CTRL6            TCO(6)
#define RF_CTRL7            TCO(7)
#define RF_CTRL8            TCO(8)
#define RF_CTRL9            TCO(9)

#ifdef CHIP_DIE_8909
//NBIOT
#ifdef __FPGA_V1__
// RF operation time(200us) + Max TA (167us) + Tolerance(45 TCU)
#define RFD_NBIOT_TCU_WRAPER_COUNTER_OFFSET      (750)
#else
// RF operation time(300us) + Max TA (267us)
#define RFD_NBIOT_TCU_WRAPER_COUNTER_OFFSET      (1089)
#endif

#include "nb_rf_data"

#define RFD_NBIOT_NEW_INTERFACE

// General macros
#define RFD_NBIOT_MAX_SUBFRAME_ENTRY_NUMBER      (10)
#define RFD_NBIOT_MAX_EVENT_NUM_PER_SUBFRAME     (16)
//#define RFD_NBIOT_TCU_WRAPER_COUNTER_OFFSET      1089 //ceil((300+267)*(30720/16)/1000)
#define RFD_NBIOT_TCU_COUNTER_LENGTH             (1920)
#define RFD_NBIOT_TCU_WRAP_COUNTER               (1919)
// (1920*0.067)
#define RFD_NBIOT_SUBFRAME_INT_RXCNT_POS         (128)
//(567 + 67) us * 1.92 = 1217 TCU counter
#define RFD_NBIOT_SUBFRAME_INT_TCU_POS           (RFD_NBIOT_TCU_WRAPER_COUNTER_OFFSET + RFD_NBIOT_SUBFRAME_INT_RXCNT_POS)

// (1920*0.067)
//#define RFD_NBIOT_TCU_RF_OFF_COUNTER_OFFSET      RFD_NBIOT_SUBFRAME_INT_RXCNT_POS
// 480 for temp
#define RFD_NBIOT_TCU_RF_OFF_COUNTER_OFFSET      (480)
// when TCU is disabled, this MACRO is only related with RF TTI process time cost
// when TCU is enabled, this MACRO is only a workaround to extend RF stable time
// -(1920*(0.5 + 0.067))
#define RFD_NBIOT_TCU_RF_ON_COUNTER_OFFSET       (-(960 + RFD_NBIOT_SUBFRAME_INT_RXCNT_POS))

#define ADVANCE                    (80)
#define DELAY                      (10)
#define PROTECTION                 (40)

#define RXON_ACP405_ADVANCE                                  (288 + ADVANCE)
// TCU event date for TUNE_FREQ
#define RFD_NBIOT_FREQ_SPI_CMD_EVENT_DATE                    (-RXON_ACP405_ADVANCE)
#define RFD_NBIOT_FREQTUNE_DURATION                          (RXON_ACP405_ADVANCE)

// TCU event date for RXON
#ifdef RF_FRONTEND_OPEN_BEFORE_ACP405_RXON
#define RFD_NBIOT_RXON_ANT_SWITCHON_EVENT_DATE               (-RXON_ACP405_ADVANCE - 20)
#define RFD_NBIOT_RXON_SPI_CMD1_EVENT_DATE                   (-RXON_ACP405_ADVANCE)
#define RFD_NBIOT_RXON_SPI_CMD2_EVENT_DATE                   (-RXON_ACP405_ADVANCE + 58)
#define RFD_NBIOT_RXON_300KHZ_FILTER_EVENT_DATE              (-RXON_ACP405_ADVANCE + 230)
#else
#define RFD_NBIOT_RXON_SPI_CMD1_EVENT_DATE                   (-RXON_ACP405_ADVANCE)
#define RFD_NBIOT_RXON_SPI_CMD2_EVENT_DATE                   (-RXON_ACP405_ADVANCE + 58)
#define RFD_NBIOT_RXON_300KHZ_FILTER_EVENT_DATE              (-RXON_ACP405_ADVANCE + 230)
#define RFD_NBIOT_RXON_ANT_SWITCHON_EVENT_DATE               (-RXON_ACP405_ADVANCE + 268)
#endif
#define RFD_NBIOT_RXON_DURATION                              (RXON_ACP405_ADVANCE + PROTECTION)

// TCU event date for TXON
#ifdef RF_FRONTEND_OPEN_BEFORE_ACP405_TXON
// RF ACP405 operation offset
#define TXON_ACP405_ADVANCE                                  (270 + ADVANCE)
#define RFD_NBIOT_TXON_SPI_CMD1_EVENT_DATE                   (-TXON_ACP405_ADVANCE)
#define RFD_NBIOT_TXON_SPI_CMD2_EVENT_DATE                   (-TXON_ACP405_ADVANCE + 27)
#define RFD_NBIOT_TXON_SPI_CMD3_EVENT_DATE                   (-TXON_ACP405_ADVANCE + 200)

// RF frontend operation offset
// DCDC ON Offset = 52us + 18us + 10us
#define RFD_NBIOT_TXON_DCDCON_SPI_CMD1_EVENT_DATE            (-TXON_ACP405_ADVANCE - 210)
#define RFD_NBIOT_TXON_DCDCON_SPI_CMD2_EVENT_DATE            (-TXON_ACP405_ADVANCE - 110)
#define RFD_NBIOT_TXON_PAON_EVENT_DATE                       (-TXON_ACP405_ADVANCE - 70)
#define RFD_NBIOT_TXON_ANT_SWITCHON_EVENT_DATE               (-TXON_ACP405_ADVANCE - 50)
#else
// RF ACP405 operation offset
#define TXON_ACP405_ADVANCE                                  (270 + ADVANCE)
#define RFD_NBIOT_TXON_SPI_CMD1_EVENT_DATE                   (-TXON_ACP405_ADVANCE)
#define RFD_NBIOT_TXON_SPI_CMD2_EVENT_DATE                   (-TXON_ACP405_ADVANCE + 27)
#define RFD_NBIOT_TXON_SPI_CMD3_EVENT_DATE                   (-TXON_ACP405_ADVANCE + 200)

// RF frontend operation offset
#define RFD_NBIOT_TXON_DCDCON_SPI_CMD1_EVENT_DATE            (-TXON_ACP405_ADVANCE + 50)
#define RFD_NBIOT_TXON_DCDCON_SPI_CMD2_EVENT_DATE            (-TXON_ACP405_ADVANCE + 150)
#define RFD_NBIOT_TXON_PAON_EVENT_DATE                       (-TXON_ACP405_ADVANCE + 190)
#define RFD_NBIOT_TXON_ANT_SWITCHON_EVENT_DATE               (-TXON_ACP405_ADVANCE + 230)
#endif
#define RFD_NBIOT_TXON_DURATION                              (TXON_ACP405_ADVANCE + PROTECTION)

// TCU event date for RXOFF
#define RXOFF_ACP405_DELAY                                   (10 + DELAY)
#define RFD_NBIOT_RXOFF_SPI_CMD_EVENT_DATE                   (RXOFF_ACP405_DELAY - 8)
#define RFD_NBIOT_RXOFF_ANT_SWITCHOFF_EVENT_DATE             (RXOFF_ACP405_DELAY)
#define RFD_NBIOT_RXOFF_DURATION                             (RXOFF_ACP405_DELAY + PROTECTION)

// TCU event date for TXOFF
#define TXOFF_ACP405_DELAY                                   (76 + DELAY)
#define RFD_NBIOT_TXOFF_SPI_CMD_EVENT_DATE                   (TXOFF_ACP405_DELAY - 64)
#define RFD_NBIOT_TXOFF_PAOFF_EVENT_DATE                     (TXOFF_ACP405_DELAY - 56)
#define RFD_NBIOT_TXOFF_DCDCOFF_SPI_CMD1_EVENT_DATE          (TXOFF_ACP405_DELAY - 48)
#define RFD_NBIOT_TXOFF_DCDCOFF_SPI_CMD2_EVENT_DATE          (TXOFF_ACP405_DELAY - 8)
#define RFD_NBIOT_TXOFF_ANT_SWITCHOFF_EVENT_DATE             (TXOFF_ACP405_DELAY)
#define RFD_NBIOT_TXOFF_DURATION                             (TXOFF_ACP405_DELAY + PROTECTION)

// TCU event date for RX Adjust
#define RFD_NBIOT_RXADJ_SPI_CMD_EVENT_DATE                    0
#define RFD_NBIOT_RXADJ_DURATION                             (10)

// TCU event date for TX Adjust
#define RFD_NBIOT_TXADJ_SPI_CMD_EVENT_DATE                    0
#define RFD_NBIOT_TXADJ_DURATION                             (PROTECTION)

#define RFD_MAX_EVENT_DATE                                   (RFD_NBIOT_TCU_COUNTER_LENGTH - RFD_NBIOT_TCU_WRAPER_COUNTER_OFFSET-1)
#define RFD_MIN_EVENT_DATE                                   (-RFD_NBIOT_TCU_WRAPER_COUNTER_OFFSET)

#define RFD_RX_COUNTER_TOLERANCE                             (PROTECTION)

#define RFD_NBIOT_INVALID_FREQUENCY       0xFFFFFFFF
#define RFD_NBIOT_INVALID_BAND            0xFFFF
#define RFD_NBIOT_INVALID_AGCGAIN         0x7FFF
#define RFD_NBIOT_INVALID_AFCOFFSET       0x7FFF
#define RFD_NBIOT_INVALID_APCPOWER        0xFFFF
#define RFD_NBIOT_INVALID_TIMEADV         0xFFFF

#define RFD_RFFE_IMM_OFF_DISABLE  (0)
#define RFD_RFFE_IMM_OFF_ENABLE   (1)

//RSRP Calibration
#if defined(__RF_NEW_BOARD_VERSION__) || defined(__RF_KU095_BOARD_VERSION__) 
#define RFD_RSRP_CALIBRATION_OFFSET_B1  (42)
#define RFD_RSRP_CALIBRATION_OFFSET_B2  (32)
#define RFD_RSRP_CALIBRATION_OFFSET_B3  (32)
#define RFD_RSRP_CALIBRATION_OFFSET_B5  (42)
#define RFD_RSRP_CALIBRATION_OFFSET_B8  (42)
#define RFD_RSRP_CALIBRATION_OFFSET_B20 (42)
#else
#define RFD_RSRP_CALIBRATION_OFFSET_B1  (16)
#define RFD_RSRP_CALIBRATION_OFFSET_B2  (16)
#define RFD_RSRP_CALIBRATION_OFFSET_B3  (16)
#define RFD_RSRP_CALIBRATION_OFFSET_B5  (16)
#define RFD_RSRP_CALIBRATION_OFFSET_B8  (16)
#define RFD_RSRP_CALIBRATION_OFFSET_B20 (16)
#endif

// RF State modes
typedef enum
{
    RFD_STATE_OFF     = 0,
    RFD_STATE_RX_ON   = 1,
    RFD_STATE_TX_ON   = 2,
    RFD_STATE_RXTX_ON = 3
} RFD_STATE_MODE_T;

// Direction RX or TX
typedef enum 
{
    RFD_EVENT_RX_OPERATION                     = 0, // RX
    RFD_EVENT_TX_OPERATION                     = 1, // TX
}RFD_Event_Direction_T;

// RFFE State modes
typedef enum
{
    RFFE_STATE_OFF     = 0,
    RFFE_STATE_ON      = 1
} RFFE_STATE_MODE_T;

typedef struct
{
#ifdef RFD_NBIOT_NEW_INTERFACE
    UINT32                         frequency; //in unit of 0.5 KHz
    INT16                          band;
#else
    UINT32                         eArfcn;
    INT8                           Moffset;
    INT8                           ulFreqAbsent;
#endif
    INT16                          agcGain;
    INT16                          afcOffset;
    INT16                          apcPower;
    INT16                          timingAdv;
    RFD_Event_Direction_T          RxOrTx;
} RFD_Action_Parameter_T;

typedef struct
{
    INT16                          subframeNo;
    INT16                          counterOffset; // RX counter offset
    UINT8                          rffeImmOff;
} RFD_Action_Time_T;

typedef struct
{
    UINT32                         eArfcn_DL;
    INT8                           Moffset_DL;
    UINT32                         eArfcn_UL;
    INT8                           Moffset_UL;	
    INT8                           ulFreqAbsent;
    INT16                          agcGain;
    INT16                          afcOffset;
    INT16                          apcPower;
    INT16                          timingAdv;
    UINT32                         DLFrequency; //in unit of KHz
    UINT32                         ULFrequency; //in unit of KHz
    INT16                          DLBand;
    INT16                          ULBand;
    INT16                          agcIndex;
    INT16                          apcIndex;
} RFD_Action_Record_Para_T;

typedef struct
{
    INT16                          eventId;
    INT16                          eventDate;
    INT16                          actionType;
    INT16                          eventType;
} RFD_Event_Record_Para_T;
#endif //!CHIP_DIE_8909
#endif // _RF_DEFS_H

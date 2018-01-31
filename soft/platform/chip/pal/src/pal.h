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

















#ifndef PAL_H
#define PAL_H

#include "cs_types.h"
#include "chip_id.h"
#include "gsm.h"
#include "baseband_defs.h"

#include "spc_mailbox.h"

#include "pal_gsm.h"

#include "pal_profile_codes.h"
#include "palp_cfg.h"

#include "rfd_defs.h"
#include "rfd_xcv.h"
#include "rfd_pa.h"
#include "rfd_sw.h"

//--------------------------------------------------------------------//
//                                                                    //
//              PAL Internal Conditional Compile Flags                //
//                                                                    //
//--------------------------------------------------------------------//

// Enable Skip Frame mechanism. The switch to the slow clock
// is allowed depending on the chip version.
#define __LPS_ENABLED__
#define __LPS_VERBOSE__

// Symbol cropping for digRf Xcvers
//#define __SATURATE_SAMPLES_TO_MODEM_WIDTH__

// Switch to 78MHz whenever the USF interrupt is enabled on a buffer.
// Don't use it for FPGA...
#define __SWITCH_TO_78_IN_GPRS__






#ifndef PAL_NO_DBGTRC
#define palDbgTrc(x) hal_DbgPalFillTrace x
#else
#define palDbgTrc(x)
#endif //PAL_DBGTRC

// Conditional Profiling Usage
#ifdef PAL_FUNC_PROFILING
#define PAL_CP_ENTRY(n) CP_ENTRY(n)
#define PAL_CP_EXIT(n) CP_EXIT(n)
#else
#define PAL_CP_ENTRY(n)
#define PAL_CP_EXIT(n)
#endif
#ifdef PAL_WIN_PROFILING
#define PAL_CP_WINENTRY(n) CP_WINENTRY(n)
#define PAL_CP_WINEXIT(n) CP_WINEXIT(n)
#else
#define PAL_CP_WINENTRY(n)
#define PAL_CP_WINEXIT(n)
#endif

//--------------------------------------------------------------------//
//                                                                    //
//                  PAL Internal Macros & Defines                     //
//                                                                    //
//--------------------------------------------------------------------//

// Chip specific
#define UNCACHED(addr)              ((addr)|0xa0000000)
#define CACHED(addr)                ((addr)|0x80000000)

// General
#define PAL_UNDEF_VALUE         -1
#define PAL_GSM_MAGIC_COOKIE    (int)0xC00170B1
#define PAL_SAME_POWER_WORD     0xFF
#define PAL_SCH_PENDING         0x5A
#define PAL_FCCH_BITMAP         0x1F // Trick to avoid RF switch off
#define PAL_LEV(n)              _PAL | TLEVEL (n)

// Absolute value
#define PAL_ABS(val)        ((val<0)?(-val):val)

// General bitfield access macros
#define PAL_BF_MASK(field)          (PAL_BF_ ## field ## _MASK)
#define PAL_BF_OFFSET(field)        (PAL_BF_ ## field ## _OFFSET)
#define PAL_BF_FIELD(dword, field)      \
    (((dword) >> PAL_BF_OFFSET(field)) & PAL_BF_MASK(field))
#define PAL_BF_SET_FIELD(dword, field, value)       \
    dword = (((dword) & ~(PAL_BF_MASK(field) << PAL_BF_OFFSET(field)))  \
    | (((value) & PAL_BF_MASK(field)) << PAL_BF_OFFSET(field)))

// TCH bit field access and values
// TCH field definitions: valid filed names are:
//   TCH_CHAN_TYPE
//   TCH_SUB_CHAN
//   TCH_CHAN_MODE
//   TCH_SPEECH_ALGO
//   TCH_DATA_RATE
#define PAL_BF_TCH_CHAN_TYPE_OFFSET 0
#define PAL_BF_TCH_CHAN_TYPE_MASK   0x3
#define PAL_BF_TCH_SUB_CHAN_OFFSET  1
#define PAL_BF_TCH_SUB_CHAN_MASK    0x1
#define PAL_BF_TCH_CHAN_MODE_OFFSET 2
#define PAL_BF_TCH_CHAN_MODE_MASK   0x3
#define PAL_BF_TCH_SPEECH_ALGO_OFFSET   4
#define PAL_BF_TCH_SPEECH_ALGO_MASK 0x3
#define PAL_BF_TCH_DATA_RATE_OFFSET 6
#define PAL_BF_TCH_DATA_RATE_MASK   0x3

// Use general bitfield access macro
#define TCH_MODE(ChMode) (PAL_BF_FIELD(ChMode,TCH_CHAN_MODE))
#define TCH_ALGO(ChMode) (PAL_BF_FIELD(ChMode,TCH_SPEECH_ALGO))
#define TCH_BAUDRATE(ChMode) (PAL_BF_FIELD(ChMode,TCH_DATA_RATE))

// TCH enums
enum palTchChanType
{
    PAL_TCH_FR_CHAN = 0,
    PAL_TCH_HR_CHAN = 1
};
enum palTchChanMode
{
    PAL_TCH_SIGNAL_CHAN = 0,
    PAL_TCH_SPEECH_CHAN = 1,
    PAL_TCH_DATA_CHAN = 2
};
enum palTchSpeechAlgo
{
    PAL_TCH_FR_HR = 0,
    PAL_TCH_EFR_UNDEF =1,
    PAL_TCH_FR_HR_AMR = 2
};
enum palTchDataRate
{
    PAL_TCH_2400 = 0,
    PAL_TCH_4800 = 1,
    PAL_TCH_9600 = 2,
    PAL_TCH_14400 = 3
};


//--------------------------------------------------------------------//
//                                                                    //
//                      PAL Time related constants                    //
//                                                                    //
//--------------------------------------------------------------------//

// Deadlock counter for BCPU Handshake (number of ticks)
#define PAL_DEADLOCK_COUNTER            0x200U

// HardWare setup time
#define PAL_HW_SETUP                    450 // Use to be 625 (1 full TS)

// ---- Window Timings ----
// PAL window timings (in Quarter Bits)
#define PAL_RX_SCH_MARGIN               (4*BB_RX_SCH_MARGIN)
#define PAL_RX_NBURST_MARGIN            (4*BB_RX_NBURST_MARGIN)
#define PAL_TS_DURATION                 625
#define PAL_BURST_ACTIVE_DURATION       (4*BB_BURST_ACTIVE_SIZE)
#define PAL_MON_WIN_DURATION            (4*MBOX_MON_WIN_SIZE)
#define PAL_INTRF_DURATION              PAL_MON_WIN_DURATION
#define PAL_SCH_WIN_DURATION            (PAL_BURST_ACTIVE_DURATION + 2*PAL_RX_SCH_MARGIN)
#define PAL_NBURST_RX_WIN_DURATION      (PAL_BURST_ACTIVE_DURATION + 2*PAL_RX_NBURST_MARGIN)
#define PAL_NBURST_TX_WIN_DURATION      (4*BB_BURST_TOTAL_SIZE)
#define PAL_ABURST_TX_WIN_DURATION      (88*4)
#define PAL_NBURST_TX_ACTIVE_DURATION   (4*148)
#define PAL_ABURST_TX_ACTIVE_DURATION   (4*88)

// Following is used to be sure that a "pal_TimeTuning" will not move the
// FINT in a window as the FINT is always away of the last TCU event of
// this offset, which is a max for time tuning (there is an assert).
#define PAL_TIME_SYNC_MARGIN            20

// In DigRf the sampling is actually trigged by the Xcver itself, but we have
// to make sure RFIN_RECORD is active during the sampling window (for internal
// clocking reason) hence the added margin.
#define PAL_TIME_DIGRF_RFIN_MARGIN      (4*1)

// Equalization Interrupt frame mask
#define PAL_RX_FRAME_EXPECTED           0x4

// Counters wrap values
#define PAL_T1_MODULO                   2048
#define PAL_T2_MODULO                   26
#define PAL_T3_MODULO                   51
#define PAL_TCU_DEFAULT_WRAP_COUNT      4999

//--------------------------------------------------------------------//
//                                                                    //
//                      PAL Size related constants                    //
//                                                                    //
//--------------------------------------------------------------------//

// Coding Scheme Data Sizes
#define PAL_CS_1_SIZE   23 // 184 bits
#define PAL_CS_2_SIZE   34 // 271 bits
#define PAL_CS_3_SIZE   40 // 315 bits
#define PAL_CS_4_SIZE   54 // 431 bits
#define PAL_CS_QTY      4

#define PAL_CSD_2400_SIZE   9 // 72 bits
#define PAL_CSD_4800_SIZE   15 // 120 bits
#define PAL_CSD_9600_SIZE   30 // 240 bits
#define PAL_CSD_14400_SIZE  37 // 290 bits
#define PAL_CSD_QTY     4

// Number of TimeSlots in a window bitmap
#define PAL_TS_IN_BM_QTY        4

// Number of VCO ressources
#define PAL_VCO_QTY     HAL_RESOURCE_QTY

// Rx ADC Gain : This is for chips using ACCO analog I/Q only
// The ADC is 2Vppd max for 12 bits (1Vp=2048 Adc Units)
// as 1Vp = sqrt(2)*Vrms and ADCGain = 20 Log (1 Vrms in Adc Units)
// then RxADCgain = 20 Log ( 2048 / sqrt(2) ) = 63 dB
#ifdef FPGA
#define PAL_IQ_ADC_GAIN    (66) // ADC used on RFDC has higher gain
#else
#define PAL_IQ_ADC_GAIN    (63)
#endif

// dBm Conversion Gain of the RxADC:
// 2 Vppd (50 Ohm) = 10 dBm
#define PAL_IQ_ADC2DBM_GAIN (PAL_IQ_ADC_GAIN - 10)

// Used for samples saturation in DigRf
#if (CHIP_HAS_14_BIT_RF_PATH)
#define PAL_MAX_MODEM_WIDTH 14
#else
#define PAL_MAX_MODEM_WIDTH 13
#endif

// AFC DAC default
#define PAL_AFC_DEFAULT_VALUE   AFC_DEFAULT_VALUE

// Window Quantity
#define PAL_SCH_DATA_QTY            4
#define PAL_TX_WIN_ROUTE_MAP_SIZE   4
#define PAL_TX_WIN_POWER_SIZE       4
#define PAL_WINDOWS_QTY             MBOX_WINDOWS_QTY
#ifdef CHIP_HAS_EXTRA_WINDOWS
#define PAL_EXT_WINDOWS_QTY    7-PAL_WINDOWS_QTY
#else
#define PAL_EXT_WINDOWS_QTY    0
#endif
#define PAL_RX_WIN_MONIT_QTY        PAL_WINDOWS_QTY
#define PAL_RX_WIN_MON_EXT_QTY      PAL_EXT_WINDOWS_QTY
#define PAL_TX_WINDOWS_QTY          1

// Maximum TCU events
#define PAL_MAX_TCU_EVENTS  60

// Requested system frequencies for the different PAL operating modes
#if 0//(CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#define PAL_STACK_SYS_FREQ_DEFAULT      HAL_SYS_FREQ_26M
#else
#define PAL_STACK_SYS_FREQ_DEFAULT      HAL_SYS_FREQ_39M
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#define PAL_STACK_SYS_FREQ_GPRS         HAL_SYS_FREQ_78M
#else // 8808 or later
#define PAL_STACK_SYS_FREQ_GPRS         HAL_SYS_FREQ_104M
#endif
#define PAL_STACK_SYS_FREQ_TCH        HAL_SYS_FREQ_78M

#if 0//(CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#define PAL_BCPU_SYS_FREQ_DEFAULT       HAL_SYS_FREQ_26M
#define PAL_BCPU_SYS_FREQ_HIGH_ACTIVITY HAL_SYS_FREQ_26M
#define PAL_BCPU_SYS_FREQ_DUMMY_PCH     HAL_SYS_FREQ_26M
#else
#if  (defined(CHIP_DIE_8955)&&defined(CHIP_SAIC_SUPPORTED))
#define PAL_BCPU_SYS_FREQ_DEFAULT       HAL_SYS_FREQ_78M
#define PAL_BCPU_SYS_FREQ_HIGH_ACTIVITY HAL_SYS_FREQ_78M
#define PAL_BCPU_SYS_FREQ_DUMMY_PCH     HAL_SYS_FREQ_78M
#else
#define PAL_BCPU_SYS_FREQ_DEFAULT       HAL_SYS_FREQ_52M
#define PAL_BCPU_SYS_FREQ_HIGH_ACTIVITY HAL_SYS_FREQ_52M
#define PAL_BCPU_SYS_FREQ_DUMMY_PCH     HAL_SYS_FREQ_52M
#endif
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
#define PAL_BCPU_SYS_FREQ_GPRS          HAL_SYS_FREQ_78M
#else // 8808 or later
#if  (defined(CHIP_DIE_8955)&&defined(CHIP_SAIC_SUPPORTED))
#define PAL_BCPU_SYS_FREQ_GPRS          HAL_SYS_FREQ_208M
#else
#define PAL_BCPU_SYS_FREQ_GPRS          HAL_SYS_FREQ_104M
#endif
#endif

// date thresholds
#define PAL_TIME_LATE_WINDOW_THRESHOLD              (3*PAL_FRAME_DURATION/4)
#define PAL_TIME_DUMMY_PCH_LATE_WINDOW_THRESHOLD    (1*PAL_FRAME_DURATION/4)

#ifdef USE_BB_DC_CANCEL
#define PAL_DC_BAND_NUM                 (2)
#define PAL_DC_ANALEV_NUM               (8)
#define PAL_DC_MAXDIGLEV                (24)
#define PAL_DC_MINDIGLEV                (0)
#define DC_THRESHOLD                     (511)
#endif
typedef enum
{
       PAL_TU_SUSPEND_IDLE = 0,
       PAL_TU_SUSPEND_LOCK,
       PAL_TU_SUSPEND_WAITFINT
}PAL_TU_SUSPEND_T;

//--------------------------------------------------------------------//
//                                                                    //
//                       PAL Internal Structures                      //
//                                                                    //
//--------------------------------------------------------------------//


// Buffer to keep Tx context to support early encodings
typedef struct
{
    UINT8   type;
    UINT8   encDone;
    UINT8   cs;
} PAL_TX_CONTEXT_T;

typedef struct
{
    INT32  Freq; // Asked Freq correction
    UINT16  Arfcn;
    BOOL Update; // Update needed
    BOOL UpdtPending; // Keep track of asked correction
} PAL_AFC_CONTEXT_T;

// Stores PAL Contex (Misc,TCU,LPS,AFC)
typedef struct
{
    UINT8   FrameValid; // Stores the valid snap
    UINT8   FrameResult; // Stores the result snap
    SPC_MBOX_GSM_COUNTERS_T NextCounter; // GSM Counters

    BOOL FchOpened; // Notify an open Fcch window

    UINT8   IntrfSlotQty; // Number of interference measurements

    UINT8   Adc2dBmGain; // dBm conversion gain

    UINT8   NBGain[MBOX_CTX_SNAP_QTY]; // Normal burst Gain buffer
    UINT8   MonGain[MBOX_CTX_SNAP_QTY][MBOX_MONIT_WIN_QTY+PAL_EXT_WINDOWS_QTY]; // Monitoring Gain buffer

    UINT8   SchGain; // Sch burst gain

    PAL_TX_CONTEXT_T TxCtx[MBOX_MAX_BUF_IDX_QTY]; // buffer type

    UINT8   EquIrqMap; // Used to Enable/Disable Equalization IT
#ifdef __USE_SCH_DECODE_IRQ__
    UINT8   SchDecIrqMap; // Used to Enable/Disable SCH Decode IT
#endif

    INT16  internalTimeShift; // Keep track of FINT shift
    INT16  SetUpTime; // Time between physical and logical FINT

    BOOL SynchedOnce; // Flag if we found a valid SCCH yet
    UINT16 activeLastStop; // Date of last event programmed in active frame

    PAL_AFC_CONTEXT_T Afc; // Automatic frequency control related
    // TODO: this will not work with a Digital AFC
    UINT32  XtalAgeValue; // Keeps track of Xtal Aging

    const RFD_XCV_PARAMS_T* xcv;
    const RFD_PA_PARAMS_T* pa;
    const RFD_SW_PARAMS_T* sw;

    RFD_WIN_BOUNDS_T winBounds;

    BOOL PcsH_DcsL; // TRUE: High band is PCS, FALSE: High band is DCS
    BOOL palModeIsGprs;

    // variables added for dummy PCH detection
    BOOL palDummyPchDetection;
    UINT8 palDummyPchThresh_a;
    UINT8 palDummyPchThresh_b;

    // BCPU clock management
    HAL_SYS_FREQ_T palBcpuSysFreqNext;
    HAL_SYS_FREQ_T palBcpuSysFreqCurr;

    PAL_TU_SUSPEND_T palTuSuspend;
    INT8  palTuVal;
    UINT8 palTuFrame;
    BOOL palCchDecDone;
#ifdef USE_BB_DC_CANCEL
    INT16 DCOff_I[PAL_DC_BAND_NUM][PAL_DC_ANALEV_NUM];
    INT16 DCOff_Q[PAL_DC_BAND_NUM][PAL_DC_ANALEV_NUM];
    UINT8 NBGainAnaLev[MBOX_CTX_SNAP_QTY];
    UINT8 NBGainDigLev[MBOX_CTX_SNAP_QTY];
#endif
} PAL_CONTEXT_T;

// Stores Frame Informations
typedef struct
{
    RFD_WIN_T Win[PAL_NB_MAX_WIN_PER_FRAME+PAL_NB_EXTR_WIN_PER_FRAME]; // Set of Windows
    UINT8   WinOrder[PAL_NB_MAX_WIN_PER_FRAME+PAL_NB_EXTR_WIN_PER_FRAME]; // Time ordered windows index
    UINT8   WinNb; // Number of active windows
    UINT16  lastStop; // Date of frame's last event
    UINT16  WrapValue; // Current frame TCU Wrap value
} PAL_FRAME_T;

// Stores all Debug linked data
typedef struct
{
    UINT32* ext_samples;   /// pointer to external samples (4*156 read buffer needed)
    UINT32* samples;       /// pointer to received samples (4*156 write buffer needed)
    UINT32* ref;           /// pointer to reconstructed symbols (4*156 write buffer needed)
    UINT32* taps;          /// pointer to channel taps (4*5 write buffer needed)
    UINT8   Req;           /// stores request
    UINT8   ReqStack;      /// Request stack
    UINT8   ReqOngoing;    /// Request Flag

    BOOL FrameStarted;     // For frame reentrance
    BOOL FirstInit;        // One time init Flag
    BOOL RFEnabled;        // RF Enabled flag (flight mode)
    BOOL InitPending;      // Flag that an Init has been requested during the frame
    BOOL RfspiFlushFifo;   // Flag to postpone the Rfspi Fifo Flush
    BOOL calibProcessEnable; // Flag to process the calibration commands in pal fint irq
} PAL_DEBUG_T;

#ifdef USE_BB_DC_CANCEL
typedef struct
{
    INT32 DCOff_I_temp[PAL_DC_BAND_NUM][PAL_DC_ANALEV_NUM];
    INT32 DCOff_Q_temp[PAL_DC_BAND_NUM][PAL_DC_ANALEV_NUM];
} PAL_DC_T;
#endif

//--------------------------------------------------------------------//
//                                                                    //
//                       PAL Non-API functions                        //
//                                                                    //
//--------------------------------------------------------------------//

void palDspIrqHandler (UINT8 interrupt_id);

#endif /* PAL_H */

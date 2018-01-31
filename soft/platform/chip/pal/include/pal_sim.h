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




#ifndef PAL_SIM_H
#define PAL_SIM_H

#if 0 //def PAL_SIMD_IMPLEMENTATION

// ============================================================================
//  pal_EnableSimCard
// ----------------------------------------------------------------------------
// This is used to select  sim-card , it enables switching between SIM 0 and
//SIM 1 as well as setting parameters.
/// @paramsim_Index: the sim card index. 0 corresponding to sim card 0,
///and 1 corresponding to sim card 1.
// ============================================================================
PUBLIC BOOL pal_EnableSimCard(UINT8 sim_Index);

// To initialize the pal sim (there is no sim reset)
PUBLIC VOID pal_SimInit(VOID);

// To know the number of bytes received by the pal
//extern UINT16 pal_GetNumRxBytes(void);
//#define pal_GetNumRxBytes simd_GetNumRxBytes
PUBLIC UINT16 pal_SimGetNumRxBytes(void); // hal_SimGetNumRxBytes

// Used to start the reset sequence (timing of each signal clock, data, vcc
// are directly controled by pal)
//   At the end of the reset => Interrupt
//extern void pal_SimReset(UINT8 * PtRxByte);
//#define pal_SimReset simd_Reset
PUBLIC VOID pal_SimReset(UINT8 * PtRxByte); // simd_Reset

// Used to power down the pal
// Timing of each signal is controled by pal
// => pal switch off clock and Vcc
//extern void pal_SimPowerDown(void);
//#define pal_SimPowerDown simd_PowerDown
PUBLIC VOID pal_SimPowerDown(VOID);


// ============================================================================
// PAL_SIM_CLOCK_STOP_MODE_T
// ----------------------------------------------------------------------------
/// This type is used to set the clock stop mode.
// ============================================================================
typedef enum
{
    /// Continuous clock mode, the autostop is disabled
    PAL_SIM_CLOCK_STOP_NO = 0,
    /// Automatic clock stop mode, stopped at low-level
    PAL_SIM_CLOCK_STOP_L = 1,
    /// Automatic clock stop mode, stopped at high-level
    PAL_SIM_CLOCK_STOP_H =2
} PAL_SIM_CLOCK_STOP_MODE_T;


// Used to indicate how pal could switch off the sim clock (according to the sim ATR)
//extern void pal_SimSetClockStopMode(UINT8 Mode);
//#define pal_SimSetClockStopMode simd_SetClockStopMode
PUBLIC VOID pal_SimSetClockStopMode(PAL_SIM_CLOCK_STOP_MODE_T mode);

// Used to set the work waiting time, pal is in charge of checking the time between
// each received sim character
// Default value (10) is not initialised by pal
//extern void pal_SetWorkWaitingTime(UINT8 WI);
//#define pal_SetWorkWaitingTime simd_SetWorkWaitingTime
PUBLIC VOID pal_SimSetWorkWaitingTime(UINT8 wi);

// ============================================================================
// PAL_SIM_ETU_DURATION_T
// ----------------------------------------------------------------------------
/// This type describes the possible ETU duration.
// ============================================================================
typedef enum
{
    /// F and D to default value F=372, D=1
    PAL_SIM_ETU_F_372_AND_D_1 = 0,
    /// F=512 and D=8 (
    PAL_SIM_ETU_F_512_AND_D_8 = 1
} PAL_SIM_ETU_DURATION_T;


// Used to select the clock rate
//extern void pal_SetEtuDuration(BOOL EtuDuration);
//#define pal_SimSetEtuDuration simd_SetEtuDuration
PUBLIC VOID pal_SimSetEtuDuration(PAL_SIM_ETU_DURATION_T fAndD);

//#define pal_SimDelayAfterEpps simd_DelayAfterEpps
PUBLIC VOID pal_SimDelayAfterEpps();

// Used to set the sim  voltage
PUBLIC BOOL pal_SimSetSimVoltage(UINT8 Level);
#define PAL_SIM_VOLTAGE_CLASS_A       0   // 5   V
#define PAL_SIM_VOLTAGE_CLASS_B       1   // 3   V
#define PAL_SIM_VOLTAGE_CLASS_C       2   // 1.8 V
#define PAL_SIM_VOLTAGE_NULL          3   // 0   V

// Used to start a sim transfer.
// A sim transfer consists in sending NbTxByte and receiving an answer of NbRxByte
// At the end of the DMA transfer => Interrupt
//extern void pal_SimTxByteRxByte(UINT16 NbTxByte, UINT8* PtTxByte, UINT16 NbRxByte, UINT8* PtRxByte);
//#define pal_SimTxByteRxByte simd_TxByteRxByte
PUBLIC VOID pal_SimTxByteRxByte(
    UINT16 nTxByte, CONST UINT8* pTxByte,
    UINT16 nRxByte, UINT8* pRxByte,
    BOOL stopClock);

// Interrupt cause
#define SIM_RESET_TIMEOUT     0
#define SIM_PARITY_ERROR      1
#define SIM_BAD_TS            2
#define SIM_ATR_SUCCESS       3
#define SIM_DMA_SUCCESS       4
#define SIM_ATR_START        5
#define SIM_WWT_TIMEOUT        6
#define SIM_EXTRA_RX        7
#define SIM_INT_CHK        8

// The following has been merged from old "palcsim.h"
UINT8 pal_GetIFCChan();

void pal_SimIntHandler(UINT8 interrupt_id);

//extern void pal_SetChGuard(UINT8 Ch_Guard);
//#define pal_SetChGuard simd_SetChGuard
// Ch_Guard is the number of ETUs _after_ the parity bit is sent
// Default (and minimum) value is 2

//extern void pal_SetTurnaroundGuard(UINT8 Turnaround_Guard);
//#define pal_SetTurnaroundGuard simd_SetTurnaroundGuard
// Turnaround_Guard is the number of ETUs between characters
// sent in opposite directions.  Default = 6

//extern void pal_SetDMAFlags(BOOL RxDMA, UINT8* PtRxByte);
//#define pal_SetDMAFlags hal_SimSetDMAFlags
// Debug function for simulation test


PUBLIC BOOL pal_SetTS(UINT8 TS);
// this is needed again for Dual SIM mode to manually configure the format
//#define pal_SetTS(a)

//#define pal_SimClockStart simd_ClockStart



#else // !PAL_SIMD_IMPLEMENTATION

#include "hal_sim.h"

#define    PAL_MAX_SIM       NUMBER_OF_SIM

#define PAL_SIM_VOLTAGE_5V      0    /// Set the voltage class A (5 V)
#define PAL_SIM_VOLTAGE_3V   1    /// Set the voltage class B (3 V)
#define PAL_SIM_VOLTAGE_1V8  2    /// Set the voltage class C (1,8 V)
#define PAL_SIM_VOLTAGE_0V   3    /// Set the Sim voltage to  0 V


#define    PAL_SIM_0         0            // sim index
#define    PAL_SIM_1         1
#define    PAL_SIM_2         2
#define    PAL_SIM_3         3


// To initialize the pal sim (there is no sim reset)
PUBLIC BOOL pal_SimInit(VOID);

PUBLIC BOOL pal_SimClose(VOID);

// To know the number of bytes received by the pal
//extern UINT16 pal_GetNumRxBytes(void);
#define pal_SimGetNumRxBytes hal_SimGetNumRxBytes

// Used to start the reset sequence (timing of each signal clock, data, vcc
// are directly controled by pal)
//   At the end of the reset => Interrupt
//extern void pal_SimReset(UINT8 * PtRxByte);
#define pal_SimReset hal_SimReset

// Used to power down the pal
// Timing of each signal is controled by pal
// => pal switch off clock and Vcc
//extern void pal_SimPowerDown(void);
#define pal_SimPowerDown hal_SimPowerDown

#define pal_SimDeactive  hal_SimDeactive

// Used to indicate how pal could switch off the sim clock (according to the sim ATR)
//extern void pal_SimSetClockStopMode(UINT8 Mode);
PUBLIC VOID pal_SimSetClockStopMode(HAL_SIM_CLOCK_STOP_MODE_T mode);
#define PAL_SIM_CLOCK_STOP_NO        HAL_SIM_CLOCK_NO_STOP
#define PAL_SIM_CLOCK_STOP_L        HAL_SIM_CLOCK_STOP_L
#define PAL_SIM_CLOCK_STOP_H        HAL_SIM_CLOCK_STOP_H

// Used to set the work waiting time, pal is in charge of checking the time between
// each received sim character
// Default value (10) is not initialised by pal
//extern void pal_SetWorkWaitingTime(UINT8 WI);
#define pal_SimSetWorkWaitingTime hal_SimSetWorkWaitingTime

// Used to select the clock rate
PUBLIC VOID pal_SimSetEtuDuration (UINT16 F, UINT8 D);

#define pal_SimDelayAfterEpps hal_SimDelayAfterEpps

#define pal_SimDMAReleaseIFC hal_SimDMAReleaseIFC

PUBLIC BOOL pal_SimSetSimVoltage(UINT8 level);

#define PAL_SIM_VOLTAGE_CLASS_A       0   // 5   V
#define PAL_SIM_VOLTAGE_CLASS_B       1   // 3   V
#define PAL_SIM_VOLTAGE_CLASS_C       2   // 1.8 V
#define PAL_SIM_VOLTAGE_NULL          3   // 0   V

// Used to start a sim transfer.
// A sim transfer consists in sending NbTxByte and receiving an answer of NbRxByte
// At the end of the DMA transfer => Interrupt
//extern void pal_SimTxByteRxByte(UINT16 NbTxByte, UINT8* PtTxByte, UINT16 NbRxByte, UINT8* PtRxByte);
#define pal_SimTxByteRxByte hal_SimTxByteRxByte

// Interrupt cause
#define SIM_RESET_TIMEOUT     0
#define SIM_PARITY_ERROR      1
#define SIM_BAD_TS                   2
#define SIM_ATR_SUCCESS       3
#define SIM_DMA_SUCCESS       4
#define SIM_ATR_START        5
#define SIM_WWT_TIMEOUT     6
#define SIM_EXTRA_RX        7
#define SIM_INT_CHK               8


#define pal_SimClockStart hal_SimClockStart


PUBLIC VOID pal_SetSimIndex(UINT8 sim_Index);
PUBLIC VOID pal_GetSimIndex(UINT8 *sim_Index);
PUBLIC BOOL pal_EnableSimCard(UINT8 sim_Index);

PUBLIC BOOL pal_SetTS(UINT8 TS);
PUBLIC UINT8 pal_getTS();
PUBLIC BOOL  pal_SelectSwitch();
PUBLIC VOID pal_SimClkInv();

#endif // PAL_SIMD_IMPLEMENTATION

#endif  /* PAL_SIM_H */

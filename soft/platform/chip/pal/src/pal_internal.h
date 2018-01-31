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


#ifndef PAL_INTERNAL_H
#define PAL_INTERNAL_H



#include "cs_types.h"
#include "string.h"
#include "gsm.h"

#include "hal_comregs.h"
#include "calib_m.h"
#include "hal_sim.h"

#include "pal.h"
#include "pal_gsm.h"
#include "pal_calib.h"
#include "hal_debugpaltrc.h"

// Type to store stack handlers
// Global variables to store external handlers
typedef struct
{
    void(*fintHandler)(UINT32);
    void(*simHandler)(UINT32);
    void(*dspHandler)(UINT32);
} PAL_HANDLERS_T;

// Structure where handlers are stored
EXPORT  PAL_HANDLERS_T g_palHandlers ;

// Structures declared in pal_gsm.c
extern PAL_CONTEXT_T g_palCtx;
extern PAL_FRAME_T g_palFrame;
extern PAL_DEBUG_T g_palDebug;
extern UINT32 _mbbsram;

// Structure declared in pal_gsms.c
extern CALIB_CALIBRATION_T* g_palCalibration;

//--------------------------------------------------------------------//
//                                                                    //
//                      PAL Internal Functions                        //
//                                                                    //
//--------------------------------------------------------------------//
void palCommonInit (void);
void palHwInit (void);
void palEnableRF (void);
void palWinInit (int WinIdx);
void palFrameInit(void);
void palSendRfFirstGain(UINT16 date);
void palSendRfNextGain(UINT16 date);
void palReorderWin (int WinIdx);
void palProgramRxWin (int WinIdx);
void palProgramTxWin (int WinIdx);
void palSetRamps (int WinIdx);
void palSetRamps_calib (UINT16 dacValueIdx, GSM_RFBAND_T band);
void pal_DspIrqHandler(HAL_COMREGS_IRQ_STATUS_T status);
void palStartBcpu(void);
void palTimersSkippedUpdate (UINT16 elapsedFrames);
void palSetUpperBound (int stop);
//void hal_LpsSkipFrame (UINT16 frameQty);
UINT8 palCalcAgcPower(UINT8 WinIdx, UINT16 Arfcn);
GSM_RFBAND_T palGetBand(UINT16 arfcn);
UINT8 palGetTcuPtr (void);
void palSetTcuPtr (UINT8 TcuPtr);
void palRxBstOn(INT16 start);
void palRxBstOff(INT16 stop);
void palSetAGC(INT16 date);
void palXcvNeedsAfcDac(UINT16 AfcDefault);
void palTurnRfIfOff(void);
void palTurnRfIfOn(void);
void pal_FintIrqHandler (void);
void palTcuIrq0Vector (UINT32 status);
void palTcuIrq1Vector (UINT32 status);
PROTECTED VOID pal_SimIrqHandler(HAL_SIM_IRQ_STATUS_T cause);



#endif // PAL_INTERNAL_H

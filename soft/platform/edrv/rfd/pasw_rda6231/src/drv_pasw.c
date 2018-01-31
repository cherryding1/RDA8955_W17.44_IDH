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
#include "gsm.h"
#include "hal_tcu.h"
#include "calib_m.h"
#include "sxr_mem.h"

#include "drv_pasw.h"
#include "drv_pasw_calib.h"

#include "rfd_cfg.h"
#include "rfd_pa.h"
#include "rfd_sw.h"
#include "rfd_defs.h"

//--------------------------------------------------------------------//
//                                                                    //
//                              MACROS                                //
//                                                                    //
//--------------------------------------------------------------------//

#define SW_UNDEF_VALUE  -1

enum
{
    PASW_DIR_RX, // match PAL definition
    PASW_DIR_TX,
    PASW_DIR_QTY
};

#define PA_MIN_OUTPUT (INT16)(1<<15) // Min PA output level
//--------------------------------------------------------------------//
//                                                                    //
//        Structures used for ramps and switches handling             //
//                                                                    //
//--------------------------------------------------------------------//

// Tx Rx switches
typedef struct
{
    UINT8 sw1;
    UINT8 sw2;
    UINT8 sw3;
} SW_TCU_VALUE_T;

// RDA6231 is for 850/900/DCS/PCS.
// The controlling are same. So we use the same driver.
//======================================|
//         Switch Truth Table           |
//======================================|
// TX_EN |  SW2 |  SW1 |  SW0  |TX Module Mode |Gallite Eng
// -------------------------------------|
//   0   |   0  |   0  |   0   |Stand By Mode  |
//   0   |   1  |   0  |   0   |RX1  (GSM850)  | PCS
//   0   |   0  |   1  |   0   |RX2  (GSM900)  | DCS
//   0   |   0  |   1  |   1   |RX3   (DCS)    | 900
//   0   |   0  |   0  |   1   |RX4   (PCS)    | 850
//   1   |   0  |   1  |   0   |TX Low Band    |
//   1   |   0  |   1  |   1   |TX High Band   |
//======================================|

SW_TCU_VALUE_T RFD_DATA_INTERNAL g_TxRxSwTcuVal[GSM_BAND_QTY][PASW_DIR_QTY];
SW_TCU_VALUE_T RFD_DATA_INTERNAL g_SwTcuCfg =
{SW_UNDEF_VALUE,SW_UNDEF_VALUE,SW_UNDEF_VALUE};

//CONST PA_CONFIG_T* RFD_DATA_INTERNAL PA;
CONST SW_CONFIG_T* RFD_DATA_INTERNAL SW;

// Some types to interpolate the PA Profile.
typedef struct
{
    INT32      x;
    INT32      y;
    INT32      IsCtrl;
} PA_POINT_T;



typedef struct
{
    INT32      x;
    INT32      y;
} POINT_T;

typedef struct
{
    INT32      nVal;
    INT32      IsCtrl;
} PA_INSERT_T;

/* Function to interpolate the PA Profile. */

//static VOID interpolate (   POINT_T * CtrlArr, INT32 CtrlNum,
//                            PA_POINT_T * RetArr, INT32 RetNum );
// FIXME TODO WARNING do that properly and check which one is
// the first to initialise ...
// Pointer attributes in paOpen just in case ...
CALIB_CALIBRATION_T* RFD_DATA_INTERNAL g_paCalibPtr;

#if 0
// =============================================================================
// g_paStartAddresses
// -----------------------------------------------------------------------------
/// This array is used to store the start value of each ramp, as it is
/// determined by the last value of the previous ramp.
// =============================================================================
UINT16 RFD_DATA_INTERNAL g_paStartValues[HAL_ANA_PA_MAX_RAMP_QTY]  ;
#endif


//--------------------------------------------------------------------//
//                                                                    //
//                       Internal  Functions                          //
//                                                                    //
//--------------------------------------------------------------------//

// Converts Pcl levels in dBm values (actually 100th of dBm)
// Handles output power variations upon arfcn.

//--------------------------------------------------------------------//
//                                                                    //
//                          API Functions                             //
//                                                                    //
//--------------------------------------------------------------------//

RFD_PA_PARAMS_T RFD_DATA_INTERNAL g_paParams =
{
    .dtxTcoSettings = 0,
    .rxOnTcoSettings = 0,
    // Window bounds
    // NOTE : these timings will be wrong if changed by a calibration
    // file, they should be updated at execution time
    .winBounds.rxSetup = 0, // irrelevant
    .winBounds.rxHold  = 0, // irrelevant
    // txSetup should be LDO_UP but LDO is not switched on/off with windows
    .winBounds.txSetup = DEFAULT_CALIB_TX_EN_UP,
    // txHold should be LDO_DN but LDO is not switched on/off with windows
    .winBounds.txHold  = DEFAULT_CALIB_TX_EN_DN,
};

const RFD_PA_PARAMS_T* rfd_PaGetParameters(VOID)
{
    // Get the calibration pointer
    //g_paCalibPtr = calib_GetPointers();

    //g_paParams.dtxTcoSettings    = MASK_TCO(PA->TXEN);
    //g_paParams.winBounds.txSetup = PA_TIME(PA_TIME_TX_EN_UP);
    //g_paParams.winBounds.txHold  = PA_TIME(PA_TIME_TX_EN_DN);
    return &g_paParams;
}

#ifndef INTERNAL_XCV_CONTROL_PASW

VOID RFD_FUNC_INTERNAL rfd_PaTxOn(INT16 start, GSM_RFBAND_T band)
{
    // Enable TX on the PA
    //hal_TcuSetEvent (SET_TCO(PA->TXEN), start + PA_TIME(PA_TIME_TX_EN_UP));
    //rfd_fprintf(RFD_INFO_TRC,"paTxOn");
    return;
}

VOID RFD_FUNC_INTERNAL rfd_PaTxOff(INT16 stop, GSM_RFBAND_T band)
{
    // Disable TX on the PA
    //hal_TcuSetEvent (CLR_TCO(PA->TXEN), stop + PA_TIME(PA_TIME_TX_EN_DN));
    //rfd_fprintf(RFD_INFO_TRC,"paTxOff");
    return;
}

VOID RFD_FUNC_INTERNAL rfd_PaSetRamps(RFD_WIN_T *Win, INT16 start)
{
#if 0
    UINT8 txBitmap = (0xF & Win->bitmap) << 1;
    // Levels are in 1/100 of dBm
    INT16 padBmMax;
    INT16 padBmMin;
    INT16 paFulldBmSpan;
    UINT16 rampSwap;
    UINT16 *win = g_paCalibPtr->pa->palcust.rampLow;

    // NOTE :   win[0:15] = rising ramp
    //          win[16:31] = falling ramp

    UINT32 idxTab[16];
    INT16 dBmStart,dBmStop,dBmDelta;
    UINT32 i, dBmStartOffset;
    UINT16 *pa ;

    UINT16 dacMax  = 1024;

#ifdef LOW_VOLTAGE_CORRECTION
    UINT16 voltage = hal_GpadcGetBatVolt();
#endif

    // Select PA profile
    if (Win->band <= GSM_BAND_GSM900) // GSM
    {
        pa = (UINT16 *)g_paCalibPtr->pa->palcust.profileG;
        padBmMax = g_paCalibPtr->pa->palcust.profileDbmMaxG;
        padBmMin = g_paCalibPtr->pa->palcust.profileDbmMinG;
        rampSwap  = g_paCalibPtr->pa->palcust.rampSwapG;
    }
    else // DCS/PCS
    {
        pa = (UINT16 *)g_paCalibPtr->pa->palcust.profileDp;
        padBmMax = g_paCalibPtr->pa->palcust.profileDbmMaxDp;
        padBmMin = g_paCalibPtr->pa->palcust.profileDbmMinDp;
        rampSwap  = g_paCalibPtr->pa->palcust.rampSwapDp;
    }
    paFulldBmSpan = (INT16)(padBmMax - padBmMin);

#ifdef LOW_VOLTAGE_CORRECTION
    // This is to maintain PA control loop bandwidth under
    // low battery voltage conditions. (avoid Vramp saturation)
    for (i=0; i<CALIB_LOW_VOLT_QTY; i++)
    {
        // if we've reached a voltage threshold...
        if (voltage < g_paCalibPtr->pa->palcust.lowVoltLimit[i])
        {
            //...we set a saturation index
            dacMax = g_paCalibPtr->pa->palcust.lowDacLimit[i];
        }
        else
        {
            break;
        }
    }
#endif

    // Always start from lower level
    g_paStartValues[0] = (PA_MIN_OUTPUT);

    for (i=0; i < 4; i++)
    {
        UINT32 j;
        UINT8 prevSlot = txBitmap & 0x1;
        UINT8 currSlot = txBitmap & 0x2;
        UINT8 nextSlot = txBitmap & 0x4;
        UINT16 txTsDuration = (Win->type == GSM_WIN_TYPE_TXNBURST) ?
                              PA_NBURST_DURATION:PA_ABURST_DURATION;

        // Slot i is ON and...
        if (currSlot)
        {
            INT32 windate = start + (i * txTsDuration);

            // ... previous slot is OFF
            if (!prevSlot)
            {
                g_ramp[g_rampIdx].id = g_rampIdx;
                g_paStartValues[g_rampIdx+1] =
                    dBmStop = Pcl2dBm(Win->band, Win->arfcn, Win->gain[i]);
                dBmStart = g_paStartValues[g_rampIdx];

                // Boundaries check. Handle out-of-range values that
                // might come from calibration procedure
                if (dBmStart < padBmMin)        dBmStart = padBmMin;
                else if (dBmStart > padBmMax)   dBmStart = padBmMax;
                if (dBmStop < padBmMin)         dBmStop = padBmMin;
                else if (dBmStop > padBmMax)    dBmStop = padBmMax;

                dBmDelta = dBmStop - dBmStart;
                if (dBmDelta > rampSwap)
                {
                    win = (UINT16 *)g_paCalibPtr->pa->palcust.rampHigh;
                }
                dBmStartOffset = ((dBmStart-padBmMin)<<10) / paFulldBmSpan;
                for (j=0; j<16; j++)
                {
                    idxTab[j] = (dBmDelta * win[j]) / paFulldBmSpan;
                    idxTab[j] += dBmStartOffset;
                    //RFD_TRACE(TSTDOUT,0,"dacIdx[%d]: %d",j,idxTab[j]);
                }

                for (j=0 ; j<16; j++)
                {
                    // Set all ramps one by one
                    g_ramp[g_rampIdx].rampDacVal[j] =
                        (pa[idxTab[j]] > dacMax) ? dacMax : pa[idxTab[j]];
                    //RFD_TRACE(TSTDOUT,0,"%d dac[%d]: %d",g_rampIdx,j,g_ramp[g_rampIdx].rampDacVal[j]);
                }

                //RFD_TRACE(TSTDOUT,0,"PA rise %d -> %d @%d",dBmStart,dBmStop,windate + PA_TIME(PA_TIME_RAMP_UP));
                hal_TcuSetEvent (g_paEvent[g_rampIdx++],
                                 windate + PA_TIME(PA_TIME_RAMP_UP));

            }

            // ... next slot is OFF
            if (!nextSlot)
            {
                g_ramp[g_rampIdx].id = g_rampIdx;
                g_paStartValues[g_rampIdx+1] =
                    dBmStop = PA_MIN_OUTPUT;
                dBmStart = g_paStartValues[g_rampIdx];

                // Boundaries check. Handle out-of-range values that
                // might come from calibration procedure
                if (dBmStart < padBmMin)        dBmStart = padBmMin;
                else if (dBmStart > padBmMax)   dBmStart = padBmMax;
                if (dBmStop < padBmMin)         dBmStop = padBmMin;
                else if (dBmStop > padBmMax)    dBmStop = padBmMax;


                dBmDelta = dBmStart - dBmStop ;
                if (dBmDelta > rampSwap)
                {
                    win = (UINT16 *)g_paCalibPtr->pa->palcust.rampHigh;
                }
                dBmStartOffset = ((dBmStop-padBmMin)<<10) / paFulldBmSpan;
                for (j=0; j<16; j++)
                {
                    idxTab[j] = (dBmDelta * win[j+16]) / paFulldBmSpan;
                    idxTab[j] += dBmStartOffset;
                    //RFD_TRACE(TSTDOUT,0,"dacIdx[%d]: %d",j,idxTab[j]);
                }

                for (j=0 ; j<16; j++)
                {
                    // Set all ramps one by one
                    g_ramp[g_rampIdx].rampDacVal[j] =
                        (pa[idxTab[j]] > dacMax) ? dacMax : pa[idxTab[j]];
                    //RFD_TRACE(TSTDOUT,0,"%d dac[%d]: %d",g_rampIdx,j,g_ramp[g_rampIdx].rampDacVal[j]);
                }

                hal_TcuSetEvent (g_paEvent[g_rampIdx++],
                                 windate +
                                 txTsDuration +
                                 PA_TIME(PA_TIME_RAMP_DN));
                //RFD_TRACE(TSTDOUT,0,"PA fall %d -> %d TS:%d @%d",dBmStart,dBmStop,txTsDuration,windate + txTsDuration + PA_TIME(PA_TIME_RAMP_DN));

            }

            // ... next slot is ON -> consecutive TX
            else
            {
                // Dont need ramp between equal PCL
                if (Win->gain[i] != Win->gain[i+1])
                {
                    g_ramp[g_rampIdx].id = g_rampIdx;
                    g_paStartValues[g_rampIdx+1] =
                        dBmStop = Pcl2dBm(Win->band, Win->arfcn, Win->gain[i+1]);
                    dBmStart = g_paStartValues[g_rampIdx];

                    // Boundaries check. Handle out-of-range values that
                    // might come from calibration procedure
                    if (dBmStart < padBmMin)        dBmStart = padBmMin;
                    else if (dBmStart > padBmMax)   dBmStart = padBmMax;
                    if (dBmStop < padBmMin)         dBmStop = padBmMin;
                    else if (dBmStop > padBmMax)    dBmStop = padBmMax;

                    if (dBmStart<dBmStop) // Rising Ramp
                    {
                        dBmDelta = dBmStop - dBmStart;
                        if (dBmDelta > rampSwap)
                        {
                            win = (UINT16 *)g_paCalibPtr->pa->palcust.rampHigh;
                        }
                        dBmStartOffset = ((dBmStart-padBmMin)<<10) / paFulldBmSpan;
                        for (j=0; j<16; j++)
                        {
                            idxTab[j] = (dBmDelta * win[j]) / paFulldBmSpan;
                            idxTab[j] += dBmStartOffset;
                        }
                        //RFD_TRACE(TSTDOUT,0,"PA rise %d -> %d @%d",dBmStart,dBmStop,windate + PA_TIME(PA_TIME_RAMP_UP));
                    }
                    else // Falling Ramp
                    {
                        dBmDelta = dBmStart - dBmStop ;
                        if (dBmDelta > rampSwap)
                        {
                            win = (UINT16 *)g_paCalibPtr->pa->palcust.rampHigh;
                        }
                        dBmStartOffset = ((dBmStop-padBmMin)<<10) / paFulldBmSpan;
                        for (j=0; j<16; j++)
                        {
                            idxTab[j] = (dBmDelta * win[j+16]) / paFulldBmSpan;
                            idxTab[j] += dBmStartOffset;
                        }

                        //RFD_TRACE(TSTDOUT,0,"PA fall %d -> %d TS:%d @%d",dBmStart,dBmStop,txTsDuration,windate + txTsDuration + PA_TIME(PA_TIME_RAMP_DN));
                    }
                    for (j=0 ; j<16; j++)
                    {
                        // Set all ramps one by one
                        g_ramp[g_rampIdx].rampDacVal[j] =
                            (pa[idxTab[j]] > dacMax) ? dacMax : pa[idxTab[j]];
                    }

                    hal_TcuSetEvent (g_paEvent[g_rampIdx++],
                                     windate +
                                     txTsDuration +
                                     PA_TIME(PA_TIME_RAMP_DN));
                }
            }
        } // Active Slot
        txBitmap >>= 1;
    } // TS loop
#endif
}

VOID RFD_FUNC_INTERNAL rfd_PaFrameStart(VOID)
{
    //INT32 i;

    //for (i=0; i < g_rampIdx; i++)
    // {
    // Calculates and fill the ramp
    //    hal_AnaPaSetRamp(g_ramp[i]);
    //}
    //g_rampIdx = 0;
    return;

}

VOID RFD_FUNC_INTERNAL rfd_PaWakeUp(VOID)
{
    // nothing here
}

VOID RFD_FUNC_INTERNAL rfd_PaSleep(RFD_LP_MODE_T lp_mode)
{
    // nothing here everything's done in rfd_SwSleep
}

#endif // !INTERNAL_XCV_CONTROL_PASW

VOID rfd_PaOpen(CONST PA_CONFIG_T* paConfig)
{
    //PA = paConfig;
    // Get the calibration pointer
    g_paCalibPtr = calib_GetPointers();
}

VOID rfd_PaClose(VOID)
{
    // nothing here
}

// RF SWITCH API
RFD_SW_PARAMS_T RFD_DATA_INTERNAL g_swParams=
{
    .dtxTcoSettings = 0,
    // Window bounds
    // NOTE : these timings will be wrong if changed by a calibration
    // file, they should be updated at execution time
    .winBounds.rxSetup = DEFAULT_CALIB_MOD_EN_UP,
    .winBounds.rxHold  = DEFAULT_CALIB_MOD_EN_DN,
    .winBounds.txSetup = DEFAULT_CALIB_MOD_EN_UP,
    .winBounds.txHold  = DEFAULT_CALIB_MOD_EN_DN,
};

const RFD_SW_PARAMS_T* rfd_SwGetParameters(VOID)
{
#ifndef INTERNAL_XCV_CONTROL_PASW
    // Get the calibration pointer
    g_paCalibPtr = calib_GetPointers();

    g_swParams.winBounds.rxSetup = SW_TIME(SW_TIME_MOD_EN_UP);
    g_swParams.winBounds.rxHold = SW_TIME(SW_TIME_MOD_EN_DN);
    g_swParams.winBounds.txSetup = SW_TIME(SW_TIME_MOD_EN_UP);
    g_swParams.winBounds.txHold = SW_TIME(SW_TIME_MOD_EN_DN);
#endif // !INTERNAL_XCV_CONTROL_PASW

    return &g_swParams;
}

VOID rfd_SwOpen(CONST SW_CONFIG_T* swConfig)
{
#ifndef INTERNAL_XCV_CONTROL_PASW

    SW = swConfig;

#if 0

// Recommended layout
    // GSM850 RX
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_RX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_RX].sw2 = CLR_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_RX].sw3 = SET_TCO(SW->SW3);
    // GSM850 TX
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_TX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

    // GSM900 RX
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_RX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_RX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_RX].sw3 = CLR_TCO(SW->SW3);
    // GSM900 TX
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_TX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

    // DCS RX
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_RX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_RX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_RX].sw3 = CLR_TCO(SW->SW3);
    // DCS TX
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_TX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

    // PCS RX
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_RX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_RX].sw2 = CLR_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_RX].sw3 = CLR_TCO(SW->SW3);
    // PCS TX
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_TX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

#else // NOT Recommended layout

// GALLITE ENG layout
    // GSM850 RX
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_RX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_RX].sw2 = CLR_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_RX].sw3 = SET_TCO(SW->SW3);
    // GSM850 TX
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_TX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM850][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

    // GSM900 RX
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_RX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_RX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_RX].sw3 = CLR_TCO(SW->SW3);
    // GSM900 TX
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_TX].sw1 = CLR_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_GSM900][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

    // DCS RX
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_RX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_RX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_RX].sw3 = CLR_TCO(SW->SW3);
    // DCS TX
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_TX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_DCS1800][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

    // PCS RX
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_RX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_RX].sw2 = CLR_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_RX].sw3 = CLR_TCO(SW->SW3);
    // PCS TX
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_TX].sw1 = SET_TCO(SW->SW1);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_TX].sw2 = SET_TCO(SW->SW2);
    g_TxRxSwTcuVal[GSM_BAND_PCS1900][PASW_DIR_TX].sw3 = CLR_TCO(SW->SW3);

#endif

#endif // !INTERNAL_XCV_CONTROL_PASW
}

VOID rfd_SwClose(VOID)
{
    // nothing here
}

#ifndef INTERNAL_XCV_CONTROL_PASW

VOID RFD_FUNC_INTERNAL rfd_SwWakeUp(VOID)
{
    // nothing here
}

VOID RFD_FUNC_INTERNAL rfd_SwSetPosition (RFD_RFDIR_T direction, GSM_RFBAND_T band, INT16 date)
{
    if (direction == RFD_DIR_TX)
    {
//        hal_TcuSetEvent (SET_TCO(PA->ENA), date + PA_TIME(PA_TIME_TX_EN_UP));
        date += SW_TIME(SW_TIME_TX_SWITCH_UP);
        rfd_fprintf(RFD_INFO_TRC,"RfSwitch[TX] band:%d",band);
    }
    else if(direction == RFD_DIR_TX_DOWN)
    {

        direction = RFD_DIR_TX;
//        hal_TcuSetEvent (CLR_TCO(PA->ENA), date + PA_TIME(PA_TIME_TX_EN_DN));
        date += SW_TIME(SW_TIME_RX_SWITCH_DN);
        //rfd_fprintf(RFD_INFO_TRC,"RfSwitch[off]");

    }
    else if(direction == RFD_DIR_RX_DOWN)
    {
        return;
    }
    else  //RFD_DIR_RX
    {
        date += SW_TIME(SW_TIME_RX_SWITCH_UP);
        //rfd_fprintf(RFD_INFO_TRC,"RfSwitch[RX] band:%d",band);

    }

    if (SW->SW1 != TCO_UNUSED && g_SwTcuCfg.sw1 != g_TxRxSwTcuVal[band][direction].sw1)
    {
        g_SwTcuCfg.sw1 = g_TxRxSwTcuVal[band][direction].sw1;
        hal_TcuSetEvent (g_SwTcuCfg.sw1, date);
    }
    if (SW->SW2 != TCO_UNUSED && g_SwTcuCfg.sw2 != g_TxRxSwTcuVal[band][direction].sw2)
    {
        g_SwTcuCfg.sw2 = g_TxRxSwTcuVal[band][direction].sw2;
        hal_TcuSetEvent (g_SwTcuCfg.sw2, date);
    }
    if (SW->SW3 != TCO_UNUSED && g_SwTcuCfg.sw3 != g_TxRxSwTcuVal[band][direction].sw3)
    {
        g_SwTcuCfg.sw3 = g_TxRxSwTcuVal[band][direction].sw3;
        hal_TcuSetEvent (g_SwTcuCfg.sw3, date);
    }
}

VOID RFD_FUNC_INTERNAL rfd_SwSleep(RFD_LP_MODE_T lp_mode)
{
    // move the switches to OFF

    //hal_TcuClrTco (PA->TXEN);
    g_SwTcuCfg.sw1 = CLR_TCO(SW->SW1);
    hal_TcuClrTco (SW->SW1);
    g_SwTcuCfg.sw2 = CLR_TCO(SW->SW2);
    hal_TcuClrTco (SW->SW2);
    g_SwTcuCfg.sw3 = CLR_TCO(SW->SW3);
    hal_TcuClrTco (SW->SW3);
}

#endif // !INTERNAL_XCV_CONTROL_PASW


//----------------------//
// Calibration Related  //
//----------------------//

#define PA_DEADLOCK 32

//#define ABS(val)        ((val<0)?(-val):val)

enum {INIT_PADAC,NEXT_PADAC,INTERP_PADAC};
UINT8 g_padac_state = INIT_PADAC;

const CALIB_PA_T pa_default_calib =
{
    DEFAULT_CALIB_PA_TIMES,
    DEFAULT_CALIB_PA_PARAM,
    DEFAULT_CALIB_PA_PALCUST
};

const CALIB_PA_T * rfd_PaCalibGetDefault(VOID)
{
    return &pa_default_calib;
}

VOID rfd_PaCalibUpdateValues(VOID)
{
    return;
}

VOID rfd_PaCalibResetValues(VOID)
{
    g_padac_state = INIT_PADAC;
    return;
}

const CALIB_SW_T sw_default_calib =
{
    DEFAULT_CALIB_SW_TIMES,
    DEFAULT_CALIB_SW_PARAM,
    DEFAULT_CALIB_SW_PALCUST
};

const CALIB_SW_T * rfd_SwCalibGetDefault(VOID)
{
    return &sw_default_calib;
}

VOID rfd_SwCalibUpdateValues(VOID)
{
    return;
}

VOID rfd_SwCalibResetValues(VOID)
{
    return;
}

PRIVATE PA_POINT_T* g_paProfileTmpArray = NULL;
PRIVATE PA_INSERT_T* g_paProfileInsertArray = NULL;
PRIVATE PA_POINT_T* g_paProfileCurveArray = NULL;

VOID rfd_PaCalibInit(VOID)
{
#ifndef DIGRF_APC_TABLE
    g_paProfileTmpArray = sxr_HMalloc(sizeof(PA_POINT_T)*CALIB_PADAC_PROF_QTY);
    g_paProfileInsertArray = sxr_HMalloc(sizeof(PA_INSERT_T)*CALIB_PADAC_PROF_QTY);
    g_paProfileCurveArray = sxr_HMalloc(sizeof(PA_POINT_T)*CALIB_PADAC_PROF_QTY);

    RFD_ASSERT(
        g_paProfileTmpArray != NULL &&
        g_paProfileInsertArray != NULL &&
        g_paProfileCurveArray != NULL,
        "Pa Calib: Memory allocation fails");
#endif // ! DIGRF_APC_TABLE
}

/* -------------------------------------------------- */
/* ----- Functions to interpolate the PA Profile. */

#ifndef DIGRF_APC_TABLE
static INT32 segment (INT32 tx1, INT32 ty1, INT32 tx2, INT32 ty2, INT32 x)
{
    INT32 dx,dy;

    dx = ABS(tx2-tx1);
    dy = ABS(ty2-ty1);

    if(dx == 0) return (INT32)((ty1+ty2)/2);
    if(dy == 0) return (INT32)(ty1);

    return (INT32)( ((ty2-ty1)*x+ty1*tx2-ty2*tx1)/(tx2-tx1) );
}



static VOID interpolate (   POINT_T * CtrlArr, INT32 CtrlNum,
                            PA_POINT_T * RetArr, INT32 RetNum )
{
    /* The shift parameter to retrieve the fixed point. */
    /* The format is 25.7 bits. */
#define FXD_SFT         (1 << 7)
    typedef UINT32             my_fxd_t;

    INT32           i,j,k,TmpNum;
    INT32           x0,y0,x1,y1;
    my_fxd_t      x,y,t,t1,t2,t3,a,b,c,d;
    POINT_T       pt[17], tail[8];

    for( i=0; i<CALIB_PADAC_PROF_QTY; i++ )
    {
        g_paProfileInsertArray[i].nVal   = -1;
        g_paProfileInsertArray[i].IsCtrl = 0;
        g_paProfileTmpArray[i].IsCtrl    = 0;
    }

    for( i=0; i < 17; i++ )
    {
        if(i==0)
        {
            pt[i].x = CtrlArr[0].x;
            pt[i].y = CtrlArr[0].y;
        }
        else if(i==16)
        {
            pt[i].x = CtrlArr[i-1].x;
            pt[i].y = CtrlArr[i-1].y;
        }
        else
        {
            pt[i].x = CtrlArr[i-1].x;
            pt[i].y = CtrlArr[i-1].y;
        }
    }

    TmpNum = 0;
    g_paProfileTmpArray[TmpNum].x      = pt[0].x ;
    g_paProfileTmpArray[TmpNum].y      = pt[0].y ;
    g_paProfileTmpArray[TmpNum].IsCtrl = 1;
    TmpNum ++;

    for( i=0; i<14; i++ )
    {
        k             = CtrlArr[i+1].x - CtrlArr[i].x;
        if(k==0)    k = 1;
        t             = (FXD_SFT*FXD_SFT*FXD_SFT) / (2*k);

        for( j=1; j<k; j++ )
        {
            t1 = j * t;
            t2 = (t1 / (FXD_SFT)) * (t1 / (FXD_SFT*FXD_SFT));
            t3 = (t2 / (FXD_SFT)) * (t1 / (FXD_SFT*FXD_SFT));

            a  = 4*t2 - t1 - 4*t3;
            b  = 1*FXD_SFT*FXD_SFT*FXD_SFT - 10*t2 + 12*t3;
            c  = t1 + 8*t2 - 12*t3;
            d  = 4*t3 - 2*t2;

            x =  a*pt[i].x+b*pt[i+1].x+c*pt[i+2].x+d*pt[i+3].x ;
            y =  a*pt[i].y+b*pt[i+1].y+c*pt[i+2].y+d*pt[i+3].y ;

            g_paProfileTmpArray[TmpNum].x      = (INT32)(x / (FXD_SFT*FXD_SFT*FXD_SFT));
            g_paProfileTmpArray[TmpNum].y      = (INT32)(y / (FXD_SFT*FXD_SFT*FXD_SFT));
            TmpNum ++;
        }
        g_paProfileTmpArray[TmpNum].x      = pt[i+2].x ;
        g_paProfileTmpArray[TmpNum].y      = pt[i+2].y ;
        g_paProfileTmpArray[TmpNum].IsCtrl = 1;
        TmpNum ++;
    }

    k          = CtrlArr[15].x-CtrlArr[14].x  ;
    if(k==0) k = 1;
    t          = (FXD_SFT*FXD_SFT*FXD_SFT) / (2*k);

    tail[0].x = CtrlArr[13].x;
    tail[0].y = CtrlArr[13].y;

    tail[1].x = CtrlArr[14].x;
    tail[1].y = CtrlArr[14].y;

    tail[2].x = CtrlArr[15].x;
    tail[2].y = CtrlArr[15].y;

    tail[3].x = CtrlArr[15].x;
    tail[3].y = CtrlArr[15].y;

    for( j=1; j<k; j++ )
    {
        t1 = j*t;
        t2 = (t1 / (FXD_SFT)) * (t1 / (FXD_SFT*FXD_SFT));
        t3 = (t2 / (FXD_SFT)) * (t1 / (FXD_SFT*FXD_SFT));

        a  = 4*t2 - t1 - 4*t3;
        b  = 1*FXD_SFT*FXD_SFT*FXD_SFT - 10*t2 + 12*t3;
        c  = t1 + 8*t2 - 12*t3;
        d  = 4*t3 - 2*t2;

        x =  a*tail[0].x+b*tail[1].x+c*tail[2].x+d*tail[3].x ;
        y =  a*tail[0].y+b*tail[1].y+c*tail[2].y+d*tail[3].y ;

        g_paProfileTmpArray[TmpNum].x      = (INT32)( x / (FXD_SFT*FXD_SFT*FXD_SFT) );
        g_paProfileTmpArray[TmpNum].y      = (INT32)( y / (FXD_SFT*FXD_SFT*FXD_SFT) );
        TmpNum ++;
    }
    g_paProfileTmpArray[TmpNum].x      = tail[3].x ;
    g_paProfileTmpArray[TmpNum].y      = tail[3].y ;
    g_paProfileTmpArray[TmpNum].IsCtrl = 1;

    for(i=0; i<CALIB_PADAC_PROF_QTY; i++)
    {
        if(g_paProfileTmpArray[i].x>=0 && g_paProfileTmpArray[i].x<=(CALIB_PADAC_PROF_QTY -1))
        {
            g_paProfileInsertArray[g_paProfileTmpArray[i].x].nVal   = g_paProfileTmpArray[i].y;
            if(g_paProfileTmpArray[i].IsCtrl==1)
            {
                g_paProfileInsertArray[g_paProfileTmpArray[i].x].IsCtrl = 1;
            }
        }
    }

    x0 = RetArr[0].x = 0;
    y0 = RetArr[0].y = g_paProfileInsertArray[0].nVal;
    RetArr[0].IsCtrl = g_paProfileInsertArray[0].IsCtrl;

    for( i=1; i<CALIB_PADAC_PROF_QTY; i++ )
    {
        if( g_paProfileInsertArray[i].nVal!=-1 )
        {
            x1 = RetArr[i].x = i;
            y1 = RetArr[i].y = g_paProfileInsertArray[i].nVal;
            RetArr[i].IsCtrl = g_paProfileInsertArray[i].IsCtrl;

            for( j=x0+1; j<x1; j++ )
            {
                RetArr[j].x      = j;
                RetArr[j].y      = segment(x0,y0,x1,y1,j);
            }
            x0 = x1;
            y0 = y1;
        }
    }
}

#endif // ! DIGRF_APC_TABLE

/* ----- End of function to interpolate the PA Profile. */
/* -------------------------------------------------- */

UINT8 rfd_PaCalibPaProfile(GSM_RFBAND_T band, INT32 PowerMeas, UINT16 *NextDAC)
{
#ifndef DIGRF_APC_TABLE
    static INT32 paTabIdx = 0;
    static UINT16 *interpTab = NULL;
    static UINT16 *profileTab = NULL;
    static POINT_T      dacArray[16];
    INT32 min;
    INT32 max;

    switch (g_padac_state)
    {
        case INIT_PADAC:

#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207)) || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
            if (GSM_BAND_PCS1900 == band)
            {
                interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpP;
                profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileP;
            }
            else if (GSM_BAND_DCS1800 == band)
            {
                interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpD;
                profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileD;
            }
            else if (GSM_BAND_GSM900 == band)
            {
                interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpG;
                profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileG;
            }
            else
            {
                interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpG850;
                profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileG850;
            }
#else
            if (GSM_BAND_DCS1800 == band || GSM_BAND_PCS1900 == band)
            {
                interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpDp;
                profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileDp;
            }
            else
            {
                interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpG;
                profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileG;
            }
#endif
            paTabIdx = 0;
            *NextDAC = interpTab[paTabIdx];
            dacArray[paTabIdx].y = interpTab[paTabIdx];
            g_padac_state = NEXT_PADAC;
            return CALIB_PROCESS_CONTINUE;
            break;

        case NEXT_PADAC:
            // Store Measured Power
            dacArray[paTabIdx].x = PowerMeas;
            // Set next DAC value
            *NextDAC = interpTab[++paTabIdx];
            if (paTabIdx >= 16) // Finished -> interpolate Profile
            {
                *NextDAC = 0;
                g_padac_state = INTERP_PADAC;
                return CALIB_PROCESS_NEED_CALM;
            }
            dacArray[paTabIdx].y = interpTab[paTabIdx];
            return CALIB_PROCESS_CONTINUE;
            break;

        case INTERP_PADAC:
            // NOTE: processing time of the following code is larger
            // than a FINT duration. -> calib stub must ensure that no
            // programation are done in this frame.
            min = dacArray[0].x;
            max = dacArray[15].x;
            *NextDAC = 0;

            if (0 == max - min)
            {
                rfd_fprintf(RFD_INFO_TRC,
                            "Error PA profile interpolation, "
                            "bad power values...");

                g_padac_state = INIT_PADAC;
                return CALIB_PROCESS_ERR_BAD_POW;
            }

            /* Check the data to interpolate. */
            for (paTabIdx = 1; paTabIdx < 16; paTabIdx++)
            {
                /* Check the monotony for the PA profile. */
                if (dacArray[paTabIdx-1].x > dacArray[paTabIdx].x)
                {
                    rfd_fprintf(RFD_INFO_TRC,
                                "Error PA profile interpolation, "
                                "power values not monotonous...");

                    g_padac_state = INIT_PADAC;
                    return CALIB_PROCESS_ERR_NO_MONO_POW;
                }
            }

            /* Save the power range. */
#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207)) || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
            if (GSM_BAND_PCS1900 == band)
            {
                g_paCalibPtr->hstPa->palcust.profileDbmMinP = min;
                g_paCalibPtr->hstPa->palcust.profileDbmMaxP = max;
            }
            else if (GSM_BAND_DCS1800 == band)
            {
                g_paCalibPtr->hstPa->palcust.profileDbmMinD = min;
                g_paCalibPtr->hstPa->palcust.profileDbmMaxD = max;
            }
            else if (GSM_BAND_GSM900 == band)
            {
                /* Save the power range. */
                g_paCalibPtr->hstPa->palcust.profileDbmMinG = min;
                g_paCalibPtr->hstPa->palcust.profileDbmMaxG = max;
            }
            else
            {
                /* Save the power range. */
                g_paCalibPtr->hstPa->palcust.profileDbmMinG850 = min;
                g_paCalibPtr->hstPa->palcust.profileDbmMaxG850 = max;
            }
#else
            if (GSM_BAND_DCS1800 == band || GSM_BAND_PCS1900 == band)
            {
                g_paCalibPtr->hstPa->palcust.profileDbmMinDp = min;
                g_paCalibPtr->hstPa->palcust.profileDbmMaxDp = max;
            }
            else
            {
                /* Save the power range. */
                g_paCalibPtr->hstPa->palcust.profileDbmMinG = min;
                g_paCalibPtr->hstPa->palcust.profileDbmMaxG = max;
            }
#endif
            /* Resize the power from dB to a range from 0 to 1023 before
             * doing the interpolation. */
            for (   paTabIdx = 0;
                    paTabIdx < CALIB_PADAC_PROF_INTERP_QTY;
                    paTabIdx++)
            {
                dacArray[paTabIdx].x = CALIB_PADAC_PROF_QTY * (dacArray[paTabIdx].x - min)
                                       / (max - min);
            }

            // Interpolate 1024 points from the 16 ctrl points.
            interpolate(dacArray, CALIB_PADAC_PROF_INTERP_QTY,
                        g_paProfileCurveArray, CALIB_PADAC_PROF_QTY);

            // Put the interpolated points in the calib structure in RAM.
            for (paTabIdx = 0; paTabIdx < CALIB_PADAC_PROF_QTY; paTabIdx++)
            {
                if(g_paProfileCurveArray[paTabIdx].y == 0)
                {
                    rfd_fprintf(RFD_INFO_TRC,
                                "Error PA profile interpolation, "
                                "a zero DAC value found...");

                    g_padac_state = INIT_PADAC;
                    return CALIB_PROCESS_ERR_ZERO_DAC;
                }

                profileTab[paTabIdx] = g_paProfileCurveArray[paTabIdx].y;
            }

            g_padac_state = INIT_PADAC;
            return CALIB_PROCESS_STOP;
            break;
    }
#endif // ! DIGRF_APC_TABLE

    return CALIB_PROCESS_CONTINUE;
}

UINT8 rfd_PaCalibfastPaProfile(GSM_RFBAND_T band)
{
#ifndef DIGRF_APC_TABLE
    static INT32 paTabIdx = 0;
    static UINT16 *interpTab = NULL;
    static UINT16 *profileTab = NULL;
    static POINT_T      dacArray[16];
    INT32 min;
    INT32 max;
#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207)) || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
    if (GSM_BAND_PCS1900 == band)
    {
        interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpP;
        profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileP;
    }
    else if (GSM_BAND_DCS1800 == band)
    {
        interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpD;
        profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileD;
    }
    else if (GSM_BAND_GSM900 == band)
    {
        interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpG;
        profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileG;
    }
    else
    {
        interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpG850;
        profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileG850;
    }
#else
    if (GSM_BAND_DCS1800 == band || GSM_BAND_PCS1900 == band)
    {
        interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpDp;
        profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileDp;
    }
    else
    {
        interpTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileInterpG;
        profileTab = (UINT16*)g_paCalibPtr->hstPa->palcust.profileG;
    }
#endif
    // Store Measured Power
    for(paTabIdx = 0; paTabIdx < 15; paTabIdx++)
    {
        dacArray[paTabIdx].y = interpTab[paTabIdx];
        dacArray[paTabIdx].x = g_paCalibPtr->hstPa->param[paTabIdx];
    }
    dacArray[paTabIdx].y = interpTab[paTabIdx];
    dacArray[paTabIdx].x = g_paCalibPtr->hstSw->param[13];
    // Set next DAC value
    // NOTE: processing time of the following code is larger
    // than a FINT duration. -> calib stub must ensure that no
    // programation are done in this frame.
    min = dacArray[0].x;
    max = dacArray[15].x;
    /* Save the power range. */
#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207)) || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
    if (GSM_BAND_PCS1900 == band)
    {
        g_paCalibPtr->hstPa->palcust.profileDbmMinP = min;
        g_paCalibPtr->hstPa->palcust.profileDbmMaxP = max;
    }
    else if (GSM_BAND_DCS1800 == band)
    {
        g_paCalibPtr->hstPa->palcust.profileDbmMinD = min;
        g_paCalibPtr->hstPa->palcust.profileDbmMaxD = max;
    }
    else if (GSM_BAND_GSM900 == band)
    {
        /* Save the power range. */
        g_paCalibPtr->hstPa->palcust.profileDbmMinG = min;
        g_paCalibPtr->hstPa->palcust.profileDbmMaxG = max;
    }
    else
    {
        /* Save the power range. */
        g_paCalibPtr->hstPa->palcust.profileDbmMinG850 = min;
        g_paCalibPtr->hstPa->palcust.profileDbmMaxG850 = max;
    }
#else
    if (GSM_BAND_DCS1800 == band || GSM_BAND_PCS1900 == band)
    {
        g_paCalibPtr->hstPa->palcust.profileDbmMinDp = min;
        g_paCalibPtr->hstPa->palcust.profileDbmMaxDp = max;
    }
    else
    {
        /* Save the power range. */
        g_paCalibPtr->hstPa->palcust.profileDbmMinG = min;
        g_paCalibPtr->hstPa->palcust.profileDbmMaxG = max;
    }
#endif
    /* Resize the power from dB to a range from 0 to 1023 before
     * doing the interpolation. */
    for (   paTabIdx = 0;
            paTabIdx < CALIB_PADAC_PROF_INTERP_QTY;
            paTabIdx++)
    {
        dacArray[paTabIdx].x = CALIB_PADAC_PROF_QTY * (dacArray[paTabIdx].x - min)
                               / (max - min);
    }

    // Interpolate 1024 points from the 16 ctrl points.
    interpolate(dacArray, CALIB_PADAC_PROF_INTERP_QTY,
                g_paProfileCurveArray, CALIB_PADAC_PROF_QTY);
    // Put the interpolated points in the calib structure in RAM.
    for (paTabIdx = 0; paTabIdx < CALIB_PADAC_PROF_QTY; paTabIdx++)
    {
        if(g_paProfileCurveArray[paTabIdx].y == 0)
        {
            rfd_fprintf(RFD_INFO_TRC,
                        "Error PA profile interpolation, "
                        "a zero DAC value found...");

            g_padac_state = INIT_PADAC;
            return CALIB_PROCESS_ERR_ZERO_DAC;
        }

        profileTab[paTabIdx] = g_paProfileCurveArray[paTabIdx].y;
    }
    return CALIB_PROCESS_STOP;
#endif // ! DIGRF_APC_TABLE


}

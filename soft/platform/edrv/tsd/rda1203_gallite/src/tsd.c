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

#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxr_tls.h"

#include "pmd_m.h"

#include "hal_host.h"

#include "tsdp_debug.h"
#include "tsd_config.h"
#include "tsdp_calib.h"
#include "tsd_m.h"

#include "tgt_tsd_cfg.h"


// ============================================================================
// LOCAL DEFINITIONS
// ============================================================================

// Number of samples we ask the touch screen controller for each coordinate.
// We need more than one because of the touch screen settling time.
// Indeed, the voltage across the touch panel "ring" and then settle down due
// to mechanical bouncing and switch driver parasiting.
// From 1 to 4 : 4 is obviously the more precise.
// well 4 is too long, settling time of switch is not relevant with opal, as opal
// switch between each measure, we keep 3 here to have some filtering, but 1 would
// probably be nice enough for Opal tsd, as it seams fairly stable !
// Security would choose 3
// According to the test, we choose 3 now. You may easily find unstable pen tracking during
// the pen keeping pressing on the same position, especially while handwriting.
#define NUMBER_SAMPLES_PER_POINT 3


// With 12-bit Conversion, keeping the tenth most significant bits.
#define MAX_ADC_VAL    0x3ff

// Flag to indicate if the tsd_GetRawPoints succeeded or not.
#define SUCCESSFUL   1
#define NOT_SUCCESSFUL   0
#define SAMPLE_VALID 3


#define READ_X_AXIS  1
#define READ_Y_AXIS  2


// Parameter of sxr_StartFunctionTimer
#define TSD_DEBOUNCE_TIME (HAL_TICK1S/1000)
#define TSD_REPITITION_TIME (3*(TSD_DEBOUNCE_TIME))


// TSD key stuff
#ifdef _USED_TSC_KEY2_
#define TSD_KEY_FILTER_COUNT 6
#else
#define TSD_KEY_FILTER_COUNT 3
#endif

PRIVATE VOID tsd_Debounce(VOID);


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// TSD config
PRIVATE CONST TSD_CONFIG_T*  g_tsdConfig;

// HAL GPIO CFG
PRIVATE HAL_GPIO_CFG_T       g_gpioCfg;

// Tab of NUMBER_SAMPLES_PER_POINT TSC_POINT samples.
PRIVATE TSD_POINT_T          g_tabSamples[NUMBER_SAMPLES_PER_POINT];

// This variable is a counter used in #debounce() function to count how many
// period of g_tsdConfig->debounceTime msec the pen state hold the same value.
PRIVATE UINT8                g_count;

// This variable stores the previous pen state as it has been detected by the
// #debounce() function and is used to detect a change in the pen state.
// When 0, the pen is considered not touching the screen.
// When 1, the pen is considered touching the screen.
PRIVATE UINT32 g_previousPenIrqPinState = 0;

PRIVATE TSD_CALLBACK_T          g_userCallback = NULL;

PRIVATE TSD_REPORTING_MODE_T    g_userReportingMode = { FALSE, FALSE, FALSE, 0};

PRIVATE TSD_PEN_STATE_T         g_penState = TSD_PEN_UP;

PRIVATE TSD_PEN_STATE_T         g_lastSendPenState = TSD_PEN_UP;


#ifdef _USED_TSC_KEY_

PRIVATE TSD_KEY_CALLBACK_T g_userKeyCallback = NULL;
PRIVATE UINT8 g_tsdKeyFiltIndex = 0;
PRIVATE UINT8 g_tsdKeyFiltKeys[TSD_KEY_FILTER_COUNT];
PRIVATE UINT8 g_TscKeyDownValueSave = 0;

PRIVATE CONST UINT16 *gc_tsdKeyAdcValue;
PRIVATE UINT8 g_tsdKeyCnt;

#ifdef _USED_TSC_KEY2_

PRIVATE CONST UINT16 *gc_tsdKey2AdcValue;
PRIVATE UINT8 g_tsdKey2Cnt;
PRIVATE UINT8 g_tsdKeyCurLine = 0;

#endif // _USED_TSC_KEY2_

#else // !_USED_TSC_KEY_

PRIVATE TSD_POINT_T          g_tabCalibratedExpectedPoint [3];

PRIVATE TSD_POINT_T          g_tabCalibratedMeasuredPoint [3];

PRIVATE TSD_MATRIX_T         g_calibrationMatrix;

PRIVATE BOOL                 g_isCalibSuccess = TRUE;

PRIVATE UINT32               g_isCalibrationDone = 0;

#endif // !_USED_TSC_KEY_


// =============================================================================
//  FUNCTIONS
// =============================================================================

#ifdef _USED_TSC_KEY_

PRIVATE UINT32 tsd_GetKeyIndex(UINT32 value)
{
    UINT16 index;

#ifdef _USED_TSC_KEY2_
    if (g_tsdKeyCurLine == 1)
#endif
    {
        for(index=0; index<g_tsdKeyCnt; index++)
        {
            if (value <= gc_tsdKeyAdcValue[index])
            {
                break;
            }
        }

        if (index == 0)
        {
            return 0;
        }
        if (index == g_tsdKeyCnt)
        {
            return g_tsdKeyCnt-1;
        }
        if (value <= (gc_tsdKeyAdcValue[index-1]+gc_tsdKeyAdcValue[index])/2)
        {
            return index-1;
        }
        return index;
    }
#ifdef _USED_TSC_KEY2_
    else
    {
        for(index=0; index<g_tsdKey2Cnt; index++)
        {
            if (value <= gc_tsdKey2AdcValue[index])
            {
                break;
            }
        }

        if (index == 0)
        {
            return 0+g_tsdKeyCnt;
        }
        if (index == g_tsdKey2Cnt)
        {
            return g_tsdKey2Cnt-1+g_tsdKeyCnt;
        }
        if (value <= (gc_tsdKey2AdcValue[index-1]+gc_tsdKey2AdcValue[index])/2)
        {
            return index-1+g_tsdKeyCnt;
        }
        return index+g_tsdKeyCnt;
    }
#endif
}

PRIVATE UINT32 tsd_FilterKey(UINT32 key)
{
    UINT32 ret = NOT_SUCCESSFUL;
    UINT32 i;

    if (g_tsdKeyFiltIndex < TSD_KEY_FILTER_COUNT)
    {
        if (g_tsdKeyFiltIndex == TSD_KEY_FILTER_COUNT-1)
        {
            g_tsdKeyFiltIndex = 0;
            for (i=0; i<TSD_KEY_FILTER_COUNT-1; i++)
            {
                if (g_tsdKeyFiltKeys[i] != (UINT8)key)
                {
                    break;
                }
            }
            if (i == TSD_KEY_FILTER_COUNT-1)
            {
                ret = SUCCESSFUL;
            }
            else
            {
                TSD_TRACE(TSTDOUT, 0, "tsd_FilterKey: Key index filter failed");
            }
        }
        else
        {
            g_tsdKeyFiltKeys[g_tsdKeyFiltIndex++] = (UINT8)key;
        }
    }

    return ret;
}

PRIVATE VOID tsd_KeyCallback(TSD_POINT_T *pointTouched,TSD_PEN_STATE_T status)
{
    if (g_userKeyCallback == NULL)
    {
        return;
    }

    if(status == TSD_PEN_DOWN)
    {
        g_TscKeyDownValueSave = pointTouched->x;
    }
    else if (status == TSD_PEN_UP)
    {
        pointTouched->x = g_TscKeyDownValueSave;
    }

    (*g_userKeyCallback)(pointTouched->x, status);
}

#endif // _USED_TSC_KEY_


// ============================================================================
//  tsd_DisableHostPenIrq
// ----------------------------------------------------------------------------
/// This function disables host pen irq.
// ============================================================================
PRIVATE VOID tsd_DisableHostPenIrq(VOID)
{
    TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_DisableHostPenIrq");
    // actually stop irq
    g_gpioCfg.irqMask.rising= FALSE;
    hal_GpioIrqSetMask(g_tsdConfig->penGpio, &g_gpioCfg.irqMask);
}



// ============================================================================
//  tsd_ClearTouchIrq
// ----------------------------------------------------------------------------
/// This function enables host pen irq.
// ============================================================================
VOID tsd_ClearTouchIrq(VOID)
{
    TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_ClearTouchIrq");
    pmd_TsdClearTouchIrq();
}

// ============================================================================
//  tsd_EnableHostPenIrq
// ----------------------------------------------------------------------------
/// This function enables host pen irq.
// ============================================================================
VOID tsd_EnableHostPenIrq(VOID)
{
    TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_EnableHostPenIrq");

    if (FALSE == pmd_ResetTouch())
    {
        sxr_StartFunctionTimer(g_tsdConfig->debounceTime,tsd_EnableHostPenIrq,(VOID*)NULL,0x03);
        return;

    }

    // filter out previous in queue Irq
    hal_GpioResetIrq(g_tsdConfig->penGpio);

    g_gpioCfg.irqMask.rising = TRUE;
    hal_GpioIrqSetMask(g_tsdConfig->penGpio, &g_gpioCfg.irqMask);
}


// ============================================================================
//  tsd_CoordFromMode
// ----------------------------------------------------------------------------
/// This function returns the &coordinate corresponding to the mode_xy
// ============================================================================
PRIVATE inline UINT32* tsd_CoordFromMode(TSD_POINT_T* sample,UINT8 mode_xy)
{
    switch(mode_xy)
    {
        case READ_X_AXIS :
            return &sample->x;
            break;
        case READ_Y_AXIS:
            return &sample->y;
            break;
        default:
            break;
    }
    return((UINT32*)1);
}




// ============================================================================
//  tsd_GetSamples
// ----------------------------------------------------------------------------
// ============================================================================
PRIVATE TSD_ERR_T tsd_GetSamples(TSD_POINT_T* TabSamples)
{
    //TSD_TRACE(EDRV_TSD_TRC, 0, "tsd:  tsd_GetSamples ");

    UINT16 i,k;
    UINT16 xy[2];
    BOOL result;
    TSD_PROFILE_FUNCTION_ENTRY(tsd_GetSamples);

    for(i = 0; i < NUMBER_SAMPLES_PER_POINT; i++)
    {
        k=0;
        do
        {
            result = pmd_TsdReadCoordinatesInternal(&xy[0],&xy[1]);
            if(result)
            {
                break;
            }
            hal_SysWaitMicrosecond(30);
        }
        while (k++ < 10);

        if(result == FALSE)
        {
            break;
        }
        TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_GetSamples:[%d] x=0x%04x,y=0x%04X",i,xy[0],xy[1]);
        pmd_TsdClearEomIrqInternal();
        *(tsd_CoordFromMode(&TabSamples[i], READ_X_AXIS)) = xy[0];
        *(tsd_CoordFromMode(&TabSamples[i], READ_Y_AXIS)) = xy[1];
    }

    hal_TimDelay(1);
    pmd_TsdClearTouchIrq();

    TSD_PROFILE_FUNCTION_EXIT(tsd_GetSamples);

    if(result == FALSE)
        return TSD_ERR_RESOURCE_BUSY;

    return TSD_ERR_NO;
}

// ============================================================================
//  tsd_EvaluateSamples
// ----------------------------------------------------------------------------
/// Each time,
/// on a pen down, NUMBER_SAMPLES_PER_POINT pairs XY from the ADC are collected.
/// NUMBER_SAMPLES_PER_POINT=4 :
/// The first sample is discarding (less time of acquisition) and the 3 others
/// will be used here. This function implements the algorithm to the best sample
/// from 3 pairs of samples by discarding one that is too way out and take a mean
/// of the rest two.
/// NUMBER_SAMPLES_PER_POINT=3 :
/// The same thing but we take in consideration the first sample.
/// NUMBER_SAMPLES_PER_POINT=2 :
/// Average of the two samples.
/// NUMBER_SAMPLES_PER_POINT=1 :
/// No evaluation here, we take the keep the only sample we have.
///
/// @param TabSamples the sampled data
/// @param sample where to fill an evaluation of the sample(s).
/// @param mode_xy selects the coordinate on wich we apply the evaluation
/// @return \c TRUE if the evaluation is valid

/// Evaluate both samples, 4 X's and 4 Y's.
/// Use global var g_samplesX, g_samplesY instead mode_xy
// ============================================================================
PRIVATE BOOL tsd_EvaluateSamples (TSD_POINT_T* TabSamples,UINT32 *sample,UINT8 mode_xy)
{
    UINT16 val0, val1, val2, diff0, diff1, diff2;
    BOOL retval = FALSE;

    TSD_PROFILE_FUNCTION_ENTRY(tsd_EvaluateSamples);

    switch(NUMBER_SAMPLES_PER_POINT)
    {
        case 4:

            // We discard the first sample : The one with the less time of acquisition.
            val0=*tsd_CoordFromMode(&TabSamples[1],mode_xy);
            val1=*tsd_CoordFromMode(&TabSamples[2],mode_xy);
            val2=*tsd_CoordFromMode(&TabSamples[3],mode_xy);

            // We check if there is no flagrant mistake during the transfert by the SPI.
            if ((val0 <= MAX_ADC_VAL) && (val1 <= MAX_ADC_VAL) && (val2 <= MAX_ADC_VAL)
                    && (val0 > 0) && (val1 > 0) && (val2 > 0))
            {
                // Calculate the absolute value of the differences of the samples.
                diff0 = (val0> val1)  ? (val0 -val1) : (val1 - val0);
                diff1 = (val1> val2)  ? (val1 -val2) : (val2 - val1);
                diff2 = (val2> val0)  ? (val2 -val0) : (val0 - val2);

                // We estimate the average valid if the difference between two samples are less than g_tsdConfig->maxError.
                if ((diff0 < g_tsdConfig->maxError) && (diff1 < g_tsdConfig->maxError) && (diff2 < g_tsdConfig->maxError))
                {
                    retval = TRUE;
                    // Eliminate the one away from other two and add the two others.
                    if (diff0 < diff1)
                    {
                        *sample=(UINT16)(val0 + ((diff2 < diff0) ? val2 : val1));
                    }
                    else
                    {
                        *sample=(UINT16)(val2 + ((diff2 < diff1) ? val0 : val1));
                    }
                    // Get the average of the two good samples.
                    *sample>>=1;
                }
                else
                {
                    TSD_TRACE(TSTDOUT, 0, "Samples not valid (>= maxError: 0x%X)", g_tsdConfig->maxError);
                }

            }
            break;

        case 3:

            // We take in consideration all samples.
            val0=*tsd_CoordFromMode(&TabSamples[0],mode_xy);
            val1=*tsd_CoordFromMode(&TabSamples[1],mode_xy);
            val2=*tsd_CoordFromMode(&TabSamples[2],mode_xy);

            // We check if there is no flagrant mistake during the transfert by the SPI.
            if ((val0 <= MAX_ADC_VAL) && (val1 <= MAX_ADC_VAL) && (val2 <= MAX_ADC_VAL)
                    && (val0 > 0) && (val1 > 0) && (val2 > 0))
            {
                // Calculate the absolute value of the differences of the samples.
                diff0 = (val0> val1)  ? (val0 -val1) : (val1 - val0);
                diff1 = (val1> val2)  ? (val1 -val2) : (val2 - val1);
                diff2 = (val2> val0)  ? (val2 -val0) : (val0 - val2);

                // We estimate the average valid if the difference between two samples are less than g_tsdConfig->maxError.
                if ((diff0 < g_tsdConfig->maxError) && (diff1 < g_tsdConfig->maxError) && (diff2 < g_tsdConfig->maxError))
                {
                    retval = TRUE;
                    // Eliminate the one away from other two and add the two others.
                    if (diff0 < diff1)
                    {
                        *sample=(UINT16)(val0 + ((diff2 < diff0) ? val2 : val1));
                    }
                    else
                    {
                        *sample=(UINT16)(val2 + ((diff2 < diff1) ? val0 : val1));
                    }
                    // Get the average of the two good samples.
                    *sample>>=1;
                }
                else
                {
                    TSD_TRACE(TSTDOUT, 0, "Samples not valid with g_tsdConfig->maxError");
                }

            }
            break;

        case 2:

            // We take in consideration all samples.
            val0=*tsd_CoordFromMode(&TabSamples[0],mode_xy);
            val1=*tsd_CoordFromMode(&TabSamples[1],mode_xy);

            // We check if there is no flagrant mistake during the transfert by the SPI.
            if ((val0 <= MAX_ADC_VAL) && (val1 <= MAX_ADC_VAL)
                    && (val0 > 0) && (val1 > 0))
            {
                // Calculate the absolute value of the differences of the samples.
                diff0 = (val0> val1)  ? (val0 -val1) : (val1 - val0);

                // We estimate the average valid if the difference between two samples are less than g_tsdConfig->maxError.
                if (diff0 < g_tsdConfig->maxError)
                {
                    retval = TRUE;
                    *sample=(UINT16)(val0 + val1);
                    // Get the average of the two good samples.
                    *sample>>=1;
                }
                else
                {
                    TSD_TRACE(TSTDOUT, 0, "Samples not valid with g_tsdConfig->maxError");
                }

            }
            break;

        case 1:

            val0=*tsd_CoordFromMode(&TabSamples[0],mode_xy);
            // We check if there is no flagrant mistake during the transfert by the SPI.
            if ((val0 <= MAX_ADC_VAL) && (val0 > 0))
            {
                retval = TRUE;
                *sample=(UINT16)(val0);
            }

            break;

        default:
            break;
    }
    TSD_PROFILE_FUNCTION_EXIT(tsd_EvaluateSamples);
    return(retval);
}


// ============================================================================
//  tsd_GetScreenPoints
// ----------------------------------------------------------------------------
/// Get X and Y samples from the Touch Screen, evaluate them, then store
/// the average of valid raw samples: the raw point corresponding to the point
/// touched on the touch screen.
/// This raw point presents distortion errors, so has to be calibrated.
///
/// @screenPoint will contain raw coordinates if calibration parameters aren't as
/// accurate as we need (calibration process will be re-launched), else it will
/// contain a calibrated coordinates ready for display.
///
/// @return 1 if the function succeeded, 0 otherwise.
// ============================================================================
PRIVATE UINT32 tsd_GetScreenPoints(TSD_POINT_T* screenPoint)
{
    UINT32 retval = NOT_SUCCESSFUL;
    // lcdScreenTouchPoint : Estimated lcd coordinates ( average of samples )
    // but without calibration fixing.
    // lcdScreenTouchPoint.x = 0;
    // lcdScreenTouchPoint.y = 0;
    TSD_POINT_T lcdScreenTouchPoint = { 0, 0, };
    TSD_PROFILE_FUNCTION_ENTRY(tsd_GetScreenPoints);
    // Get 4 X samples and 4 Y samples and store them to g_tabSamples
    if (tsd_GetSamples(g_tabSamples) == TSD_ERR_RESOURCE_BUSY)
    {
        return NOT_SUCCESSFUL;
    }

    BOOL checkX = TRUE;
    BOOL checkY = TRUE;

#ifdef _USED_TSC_KEY_
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifdef TSC_KEY_USE_X_COORDINATE
    checkY = FALSE;
#else
    checkX = FALSE;
#endif
#else // 8809 or later
    checkY = FALSE;
#endif
#endif // _USED_TSC_KEY_

    // Then, If the samples are valid,evaluate them and use matrix calibration to fix them.
    if ( (!checkX ||
            tsd_EvaluateSamples(g_tabSamples,&(lcdScreenTouchPoint.x),READ_X_AXIS)) &&
            (!checkY ||
             tsd_EvaluateSamples(g_tabSamples,&(lcdScreenTouchPoint.y),READ_Y_AXIS))
       )
    {
#ifdef _USED_TSC_KEY_
        UINT32 value;
        if (checkX)
        {
            value = lcdScreenTouchPoint.x;
        }
        if (checkY)
        {
            value = lcdScreenTouchPoint.y;
        }

        screenPoint->x = tsd_GetKeyIndex(value);

        TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_GetScreenPoints:mean x=0x%04x,y=0x%04x,KeyIndex=%d",
                  lcdScreenTouchPoint.x, lcdScreenTouchPoint.y, screenPoint->x);
        // Filter key index
        retval = tsd_FilterKey(screenPoint->x);
#else
        TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_GetScreenPoints:mean x=0x%04x,y=0x%04x",
                  lcdScreenTouchPoint.x, lcdScreenTouchPoint.y);

        retval = SAMPLE_VALID;

        if( g_isCalibrationDone == 1)
        {
            retval = SUCCESSFUL;
            // Provide calibrated coordinates ready for display
            GetDisplayPoint(screenPoint,&lcdScreenTouchPoint,&g_calibrationMatrix);

            //TSD_TRACE(TSTDOUT,0,"calib: 2 x=%d y=%d",lcdScreenTouchPoint.x,lcdScreenTouchPoint.y);
            //TSD_TRACE(TSTDOUT,0,"calib: 3 x=%d y=%d %d ",screenPoint->x,screenPoint->y,retval2);
            //TSD_TRACE(TSTDOUT,0,"calib: 1=%d 2=%d",g_calibrationMatrix.An,g_calibrationMatrix.Bn);
            //TSD_TRACE(TSTDOUT,0,"calib: 3=%d 4=%d ",g_calibrationMatrix.Cn,g_calibrationMatrix.Dn);
            //TSD_TRACE(TSTDOUT,0,"calib: 5=%d 6=%d 7=%d",g_calibrationMatrix.En,g_calibrationMatrix.Fn,g_calibrationMatrix.Divider);

            if(!g_isCalibSuccess)
            {
                // If calibration matrix not accurate, raw point will be
                // delivered for a new calibration process
                screenPoint->x = lcdScreenTouchPoint.x;
                screenPoint->y = lcdScreenTouchPoint.y;
            }
        }
        else
        {
            // If the calibration has not been done yet, we return raw coordinate with scaling,rotating errors.
            screenPoint->x = lcdScreenTouchPoint.x;
            screenPoint->y = lcdScreenTouchPoint.y;
        }
#endif
    }

    TSD_PROFILE_FUNCTION_EXIT(tsd_GetScreenPoints);
    return(retval);
}


// ============================================================================
//  tsd_SetCalibStatus
// ----------------------------------------------------------------------------
/// Set the calibration status
/// @param isCalibrated \c TRUE when calibrated, \c FALSE to restart calibration
/// Once the status is set to \c TRUE, the callback #TSD_CALLBACK_T will be
/// called with corrected values.
/// note that tsd_SetCalibPoints() must be called before setting the calibration
/// to \c TRUE
// ============================================================================
PUBLIC VOID tsd_SetCalibStatus(BOOL isCalibStatus)
{
#ifndef _USED_TSC_KEY_
    g_isCalibSuccess = isCalibStatus;
#endif
}

// ============================================================================
//  tsd_Debounce
// ----------------------------------------------------------------------------
/// This function debounces signal coming from pen irq gpio pin.
/// It return a raw point coordinates to the g_userCallback function which will
/// do calibration process.
// ============================================================================
PRIVATE VOID tsd_Debounce(VOID)
{
    TSD_POINT_T          screenPoint;
    TSD_PROFILE_FUNCTION_ENTRY(tsd_Debounce);
    UINT32 resultOfsample = 0;
    UINT32 tsdClearState = 0;
    UINT32 currentPenIrqPinState = hal_GpioGet(g_tsdConfig->penGpio);

#if defined(_USED_TSC_KEY_) && defined(_USED_TSC_KEY2_)
    PMD_TSD_IRQ_T tsdIrq = pmd_TsdGetActiveIrq();
    if (tsdIrq == PMD_TSD_IRQ_ERROR)
    {
        TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_Debounce: ISPI busy");
        sxr_StartFunctionTimer(g_tsdConfig->debounceTime,tsd_Debounce,(VOID*)NULL,0x03);
        return;
    }

    g_tsdKeyCurLine = (tsdIrq & PMD_TSD_IRQ_KEY) ? 1 : 2;
#endif

    TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_Debounce: curState=%d, prevState=%d",
              currentPenIrqPinState, g_previousPenIrqPinState);

    if(currentPenIrqPinState != g_previousPenIrqPinState)
    {
        // The pen state has just changed : We reset the counter.
        g_count = 0;
#ifdef _USED_TSC_KEY_
        g_tsdKeyFiltIndex = 0;
#endif
        sxr_StartFunctionTimer((TSD_DEBOUNCE_TIME), tsd_Debounce,(VOID*)NULL,0x03);
    }
    else
    {
        // The pen state does not change.
        g_count++;
        switch (g_penState)
        {
            case TSD_PEN_DOWN :
            {
                TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_Debounce: TSD_PEN_DOWN");

                // If we reach upPeriod and if PenIrqPinState is low
                if( (g_count >= g_tsdConfig->upPeriod) && (currentPenIrqPinState == 0) )
                {
                    // We consider the pen down.
                    g_penState = TSD_PEN_UP;
                    // We reset the counter.
                    g_count = 0;

                    // We calculate the coordinates of the touch screen point.

                    // If the user wants a call to g_userCallback function.
                    if (g_userReportingMode.onUp)
                    {
                        if(g_lastSendPenState == TSD_PEN_DOWN || g_lastSendPenState == TSD_PEN_PRESSED)
                        {
                            if (g_userCallback)
                            {
                                (*g_userCallback)(&screenPoint, g_penState);
                            }
                            g_lastSendPenState = TSD_PEN_UP;
                        }
                    }
                    // We enable the host pen irq and do not reload the timer.
                    tsdClearState = 1;
                }
                else
                {
                    // If we reach repetitionPeriod and if PenIrqPinState is high.
                    if( (g_count >= g_userReportingMode.repetitionPeriod) && (currentPenIrqPinState == 1) )
                    {
                        // We consider the pen down.
                        g_penState = TSD_PEN_PRESSED;
                        // We reset the counter.
                        g_count = 0;

                        // We calculate the coordinates of the touch screen point.

                        // If the user wants a call to g_userCallback function.
#ifdef _USED_TSC_KEY_
                        resultOfsample = SUCCESSFUL;
#else
                        resultOfsample = tsd_GetScreenPoints(&screenPoint);
#endif

                        if (g_userReportingMode.onPressed)
                        {
                            if(resultOfsample != NOT_SUCCESSFUL)
                            {
                                if (g_userCallback)
                                {
                                    (*g_userCallback)(&screenPoint, g_penState);
                                }
                                g_lastSendPenState = TSD_PEN_PRESSED;
                            }
                        }
                    }

                    // We reload the timer.
                    sxr_StartFunctionTimer((TSD_REPITITION_TIME),tsd_Debounce,(VOID*)NULL,0x03);
                }

                break;
            }

            case TSD_PEN_UP :
            {
                TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_Debounce: TSD_PEN_UP");

                if(currentPenIrqPinState == 1)
                {
                    if (g_count >= g_tsdConfig->downPeriod)
                    {
                        // We consider the pen down.
                        g_penState = TSD_PEN_DOWN;
                        // We reset the counter.
                        g_count = 0;

                        // We calculate the coordinates of the touch screen point.

                        // If the user wants a call to g_userCallback function.
                        resultOfsample = tsd_GetScreenPoints(&screenPoint);

                        if (g_userReportingMode.onDown)
                        {
                            if(resultOfsample != NOT_SUCCESSFUL)
                            {
                                if (g_userCallback)
                                {
                                    (*g_userCallback)(&screenPoint, g_penState);
                                }
                                g_lastSendPenState = TSD_PEN_DOWN;
                            }
                            else
                            {
                                g_penState = TSD_PEN_UP;
                            }
                        }
                    }

                    sxr_StartFunctionTimer((TSD_DEBOUNCE_TIME),tsd_Debounce,(VOID*)NULL,0x03);
                }
                else
                {
                    if (g_count <= g_tsdConfig->upPeriod)
                    {
                        sxr_StartFunctionTimer((TSD_DEBOUNCE_TIME),tsd_Debounce,(VOID*)NULL,0x03);
                    }
                    else
                    {
                        g_count = 0;
                        tsdClearState = 1;
                    }
                }
                break;
            }

            case TSD_PEN_PRESSED :
            {
                TSD_TRACE(EDRV_TSD_TRC, 0, "tsd_Debounce: TSD_PEN_PRESSED");

                // If we reach upPeriod and if PenIrqPinState is low
                if( (g_count >= g_tsdConfig->upPeriod) && (currentPenIrqPinState == 0) )
                {
                    // We consider the pen up.
                    g_penState = TSD_PEN_UP;
                    // We reset the counter.
                    g_count = 0;

                    // We calculate the coordinates of the touch screen point.

                    // If the user wants a call to g_userCallback function.
                    if (g_userReportingMode.onUp)
                    {
                        if(g_lastSendPenState == TSD_PEN_DOWN || g_lastSendPenState == TSD_PEN_PRESSED)
                        {
                            if (g_userCallback)
                            {
                                (*g_userCallback)(&screenPoint, g_penState);
                            }
                            g_lastSendPenState = TSD_PEN_UP;
                        }
                    }
                    // We enable the host pen irq and do not reload the timer.
                    tsdClearState = 1;
                }
                else
                {
                    if( (g_count >= g_userReportingMode.repetitionPeriod) && (currentPenIrqPinState==1) )
                    {
                        // We consider the pen still pressed and we reset the counter.
                        g_count=0;

                        // We calculate the coordinates of the touch screen point.

                        // If the user wants a call to g_userCallback function.
#ifdef _USED_TSC_KEY_
                        resultOfsample = SUCCESSFUL;
#else
                        resultOfsample = tsd_GetScreenPoints(&screenPoint);
#endif

                        if (g_userReportingMode.onPressed)
                        {
                            if(resultOfsample != NOT_SUCCESSFUL)
                            {
                                if (g_userCallback)
                                {
                                    (*g_userCallback)(&screenPoint, g_penState);
                                }
                                g_lastSendPenState = TSD_PEN_PRESSED;
                            }
                        }
                    }

                    // We reload the timer.
                    sxr_StartFunctionTimer((TSD_REPITITION_TIME),tsd_Debounce,(VOID*)NULL,0x03);
                }

                break;

            }

            default:
            {
            }
        }
    }

    g_previousPenIrqPinState = currentPenIrqPinState;

    tsd_ClearTouchIrq();

    if(tsdClearState == 1)
        tsd_EnableHostPenIrq();

    TSD_PROFILE_FUNCTION_EXIT(tsd_Debounce);
}


// ============================================================================
//  tsd_GpioIrqHandler
// ----------------------------------------------------------------------------
// This function handles GPIO hard interruptions.
// ============================================================================
PRIVATE VOID tsd_GpioIrqHandler(VOID)
{
    TSD_PROFILE_FUNCTION_ENTRY(tsd_GpioIrqHandler);
    // We check the value of the penIrqPin to avoid false triggering when we reset
    // the touch screen controler.

    if (hal_GpioGet(g_tsdConfig->penGpio) == 1)
    {
        // We disable host Pen IRQ while the screen is pressed.
        tsd_DisableHostPenIrq();
        sxr_StartFunctionTimer(g_tsdConfig->debounceTime,tsd_Debounce,(VOID*)NULL,0x03);
    }
    TSD_PROFILE_FUNCTION_EXIT(tsd_GpioIrqHandler);
}


#ifdef _USED_TSC_KEY_
// ============================================================================
//  tsd_KeyOpen
// ----------------------------------------------------------------------------
/// This function initializes the touch screen key driver. The configuration of the
/// touch screen pins used is board dependent and stored in a TSD_CONFIG_T struct.
// ============================================================================
PUBLIC VOID tsd_KeyOpen(TSD_KEY_CALLBACK_T callback)
{
    g_userKeyCallback = callback;

    g_userReportingMode.onDown  = TRUE;
    g_userReportingMode.onUp = TRUE;

    tsd_Open(tsd_KeyCallback);
}
#endif


// ============================================================================
//  tsd_Open
// ----------------------------------------------------------------------------
/// This function initializes the touch screen driver. The configuration of the
/// touch screen pins used is board dependent and stored in a TSD_CONFIG_T struct.
// ============================================================================
PUBLIC VOID tsd_Open(TSD_CALLBACK_T callback)
{
#ifdef _USED_TSC_KEY_
    static BOOL opened = FALSE;

    if (opened)
    {
        return;
    }
    opened = TRUE;

#ifndef _USED_TSC_KEY_
    g_userCallback = callback;
#endif

    tgt_GetTsdKeyAdcValue(&gc_tsdKeyAdcValue, &g_tsdKeyCnt);
#ifdef _USED_TSC_KEY2_
    tgt_GetTsdKey2AdcValue(&gc_tsdKey2AdcValue, &g_tsdKey2Cnt);
#endif
#endif

    g_tsdConfig = tgt_GetTsdConfig();
    g_gpioCfg.direction = HAL_GPIO_DIRECTION_INPUT;

    while(!pmd_TsdEnableIrq())
    {
        sxr_Sleep(10);
    }

    // GPIO irq mask cfg
    g_gpioCfg.irqMask.falling = FALSE;
    g_gpioCfg.irqMask.debounce = TRUE;
    g_gpioCfg.irqHandler = tsd_GpioIrqHandler;
    g_gpioCfg.irqMask.level = TRUE;
    // Rising level interrupt will be actually enabled in tsd_EnableHostPenIrq
    g_gpioCfg.irqMask.rising = FALSE;
    hal_GpioOpen(g_tsdConfig->penGpio, &g_gpioCfg);

    tsd_EnableHostPenIrq();
}


// ============================================================================
//  tsd_Close
// ----------------------------------------------------------------------------
/// This function closes the touch screen driver. Deactivate and close the SPI
// ============================================================================
PUBLIC VOID tsd_Close(VOID)
{
    while(!pmd_TsdDisableIrq())
    {
        sxr_Sleep(10);
    }

    hal_GpioClose(g_tsdConfig->penGpio);
}

// ============================================================================
//  tsd_SetPowerMode
// ----------------------------------------------------------------------------
/// This function switch the power mode of TSD.
///
/// @param powerMode The power mode to switch. See #TSD_POWER_MODE_T.
// ============================================================================
PUBLIC VOID tsd_SetPowerMode(TSD_POWER_MODE_T powerMode)
{
}

// ============================================================================
//  tsd_SetReportingMode
// ----------------------------------------------------------------------------
/// This function controls under which conditions the touch screen interrupt
/// will be generated. It can be when the pen is pressed, when it is hold down
/// and when it is released. The function passed via #tsd_Open() will be
/// called in case of a touch screen interruption.
///
/// @param mode Defines which event will generate a call to the user
/// callback function. See the documentation of the type for details.
// ============================================================================
PUBLIC VOID tsd_SetReportingMode(TSD_REPORTING_MODE_T* mode)
{
#ifdef _USED_TSC_KEY_

    return;

#else // !_USED_TSC_KEY_

    g_userReportingMode.onDown    = mode->onDown;
    g_userReportingMode.onPressed = mode->onPressed;
    g_userReportingMode.onUp      = mode->onUp;
    g_userReportingMode.repetitionPeriod = mode->repetitionPeriod;

    TSD_ASSERT( (g_userReportingMode.repetitionPeriod != 0)
                || (!g_userReportingMode.onPressed),
                "Please set the repetitionPeriod when using onPressed!");

    // don't actually enable IRQ before Open.
    if (g_tsdConfig)
    {
        // depending on wanted events we can disable the IRQ.
        if (g_userReportingMode.onDown
                || g_userReportingMode.onPressed
                || g_userReportingMode.onUp)
        {
            while(!pmd_TsdEnableIrq())
            {
                sxr_Sleep(10);
            }
            tsd_EnableHostPenIrq();
        }
        else
        {
            // no events enabled, stop pen Irq
            tsd_DisableHostPenIrq();
            while(!pmd_TsdDisableIrq())
            {
                sxr_Sleep(10);
            }
        }
    }

#endif // !_USED_TSC_KEY_
}


// ============================================================================
//  tsd_GetReportingMode
// ----------------------------------------------------------------------------
/// This function returns the touch screen reporting mode. Refer to the type
/// documentation for its interpretation.
///
/// @param mode A pointer to receive the current reporting mode.
// ============================================================================
PUBLIC VOID tsd_GetReportingMode(TSD_REPORTING_MODE_T* mode)
{
    TSD_ASSERT( mode != NULL, "Pointer mode is NULL!");
    mode->onDown    = g_userReportingMode.onDown;
    mode->onUp      = g_userReportingMode.onUp;
    mode->onPressed = g_userReportingMode.onPressed;
    mode->repetitionPeriod = g_userReportingMode.repetitionPeriod;
}


// ============================================================================
//  tsd_SetCalibPoints
// ----------------------------------------------------------------------------
/// This function fixes the calibration parameters of the touch screen driver.
/// From an array of 3 samples per point, it sets an array of
/// 3 Display points and another array of 3 Screen points needed for calibration
///
/// @param calibPoints array of 3 calibration points
//  @return TSD_ERR_INVALID_CALIBRATION if the calibration matrix is invalid.
///
/// This function include a fatal assert that will be triggered if the coordinates
/// of the expected points do not allow a valid calibration (divide by 0)
/// this occurs when the point are aligned, or some have the same value for X or Y
// ============================================================================
PUBLIC TSD_ERR_T tsd_SetCalibPoints(TSD_CALIBRATION_POINT_T calibPoints[3])
{
#ifndef _USED_TSC_KEY_
    UINT8  i;

    // Fix tab of 3 calibration points from tab calibPoints[3] of 3

    for(i = 0; i < 3; i++)
    {
        // fill tab of 3 samples for each point N°1 N°2 and N°3
        g_tabCalibratedExpectedPoint[i].x = calibPoints[i].expected.x;
        g_tabCalibratedExpectedPoint[i].y = calibPoints[i].expected.y;
        g_tabCalibratedMeasuredPoint[i].x  = calibPoints[i].measured.x;
        g_tabCalibratedMeasuredPoint[i].y  = calibPoints[i].measured.y;
    }

    g_isCalibrationDone = SetCalibrationMatrix(g_tabCalibratedExpectedPoint, g_tabCalibratedMeasuredPoint
                          , &g_calibrationMatrix);

    if(g_isCalibrationDone)
    {
        return TSD_ERR_NO;
    }
#endif
    return TSD_ERR_INVALID_CALIBRATION;
}

// ============================================================================
//  tsd_GetHwVersion
// ----------------------------------------------------------------------------
/// This function returns the version of tp hardware.
// ============================================================================
PUBLIC CHAR* tsd_GetHwVersion(VOID)
{
    return "Unknown";
}

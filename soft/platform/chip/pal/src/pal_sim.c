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


#if 0 //defined(PAL_SIMD_IMPLEMENTATION)

#include "hal_timers.h"
#include "pal_sim.h"
#include "pmd_m.h"
#include "tgt_simd_cfg.h"
#include "simd_m.h"
#include "sxr_tls.h"

#define         NULLVOLT_DLY     (20)           // delay in 1/1000 sec
#define         SETVOLT_DLY      (50)           // delay in 1/1000 sec

//UINT8 g_SimIndex     = 0;

// ============================================================================
//  pal_EnableSimCard
// ----------------------------------------------------------------------------
// This is used to select  sim-card , it enables switching between SIM 0 and
//SIM 1 as well as setting parameters.
/// @paramsim_Index: the sim card index. 0 corresponding to sim card 0,
///and 1 corresponding to sim card 1.
/// @return value _should_ be always true in single SIM platforms
// ============================================================================
PUBLIC BOOL pal_EnableSimCard(UINT8 sim_Index)
{
    //g_SimIndex = sim_Index;
    return (simd_SetSimEnable(sim_Index));
}

// ============================================================================
// pal_SimInit
// ----------------------------------------------------------------------------
// ============================================================================
PUBLIC VOID pal_SimInit(VOID)
{
    // SCI init
    // Configure PMIC for level shift
    //pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
    simd_Open(tgt_GetSimdConfig());

    // TODO: This should not be done here.  Should be done by stack which
    // calls pal_EnableSimCard.
    pal_EnableSimCard(0);

    simd_SetSimVoltage(SIMD_VOLTAGE_NULL);
}

PUBLIC VOID pal_SimDelayAfterEpps()
{
    simd_DelayAfterEpps();
}

PUBLIC VOID pal_SimTxByteRxByte(
    UINT16 nTxByte, CONST UINT8* pTxByte,
    UINT16 nRxByte, UINT8* pRxByte,
    BOOL stopClock)
{
    simd_TxByteRxByte(nTxByte,pTxByte,nRxByte,pRxByte,stopClock);
}

PUBLIC UINT16 pal_SimGetNumRxBytes(void)    // hal_SimGetNumRxBytes
{
    return simd_GetNumRxBytes();
}

PUBLIC VOID pal_SimSetClockStopMode(PAL_SIM_CLOCK_STOP_MODE_T mode)
{
    simd_SetClockStopMode(mode);
}

PUBLIC VOID pal_SimPowerDown(VOID)
{
    simd_PowerDown();
}

PUBLIC VOID pal_SimSetEtuDuration(PAL_SIM_ETU_DURATION_T fAndD)
{
    simd_SetEtuDuration(fAndD);
}

PUBLIC VOID pal_SimSetWorkWaitingTime(UINT8 wi)
{
    simd_SetWorkWaitingTime(wi);
}

PUBLIC VOID pal_SimReset(UINT8* pRxBytes)
{
    simd_Reset(pRxBytes);
}

// ============================================================================
// pal_SimClose
// ----------------------------------------------------------------------------
// ============================================================================
PUBLIC VOID pal_SimClose(VOID)
{
    simd_Close();

    // SCI init
    // Configure PMIC for level shift
    //pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
    simd_SetSimVoltage(SIMD_VOLTAGE_NULL);
}

// ============================================================================
// pal_SimSetVoltage
// ----------------------------------------------------------------------------
/// WARNING
/// This function is called under IRQ. Fancy things like
/// call to sxr_Sleep or mailbox creating are FORBIDDEN
/// This function sets the SIM voltage.
/// @param level Voltage to set.
// ============================================================================
PUBLIC BOOL pal_SimSetSimVoltage(UINT8 level)
{
    UINT32 delay;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)

    delay = SETVOLT_DLY;

    switch (level)
    {
        case PAL_SIM_VOLTAGE_CLASS_A:
            //pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_5V0);
            simd_SetSimVoltage(SIMD_VOLTAGE_CLASS_A);
            break;
        case PAL_SIM_VOLTAGE_CLASS_B:
            //pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_3V0);
            simd_SetSimVoltage(SIMD_VOLTAGE_CLASS_B);
            break;
        case PAL_SIM_VOLTAGE_CLASS_C:
            //pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_1V8);
            simd_SetSimVoltage(SIMD_VOLTAGE_CLASS_C);
            break;
        case PAL_SIM_VOLTAGE_NULL:
            //pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
            simd_SetSimVoltage(SIMD_VOLTAGE_NULL);
            // Add extra delay for voltage 0
            delay += NULLVOLT_DLY;
            break;
    }

    hal_TimDelay(delay MS_WAITING);
    return TRUE;
}


// ============================================================================
// pal_SetTS
// ----------------------------------------------------------------------------
/// This function used to set ATR TS value.
/// @param TS is TS value from the ATR response
// ============================================================================
PUBLIC BOOL pal_SetTS(UINT8 TS)
{
    if(TS == 0x23 || TS == 0x3F)
    {
        // This only needs to be called in dual SIM mode
        simd_Switch2Manual(TS);
    }
    return FALSE;
}


#else // !PAL_SIMD_IMPLEMENTATION

// Common includes
#include "cs_types.h"


//PAL includes
#include "pal_sim.h"
#include "palp_cfg.h"
#include "pal.h"

// HAL includes
#include "hal_timers.h"
#include "hal_sim.h"
#include "sxs_io.h"
#include "pmd_m.h"

// Dual sim includes
#include "drv_dualsim.h"

#define         NULLVOLT_DLY     (20)           // delay in 1/1000 sec
#define         SETVOLT_DLY      (50)           // delay in 1/1000 sec

UINT8 g_SimIndex      = 0;

UINT8 g_Sim0VoltLevel = PAL_SIM_VOLTAGE_0V;        //initial voltage
UINT8 g_Sim1VoltLevel = PAL_SIM_VOLTAGE_0V;
UINT8 g_Sim2VoltLevel = PAL_SIM_VOLTAGE_0V;
UINT8 g_Sim3VoltLevel = PAL_SIM_VOLTAGE_0V;

UINT16 g_palEtuParaF[4] = {0};
UINT8  g_palEtuParaD[4] = {0};
UINT8  g_palEtuClkInvFlag[4] = {1,1,1,1};

UINT8 g_TS[4] = {0x3B, 0x3B, 0x3B, 0x3B};
HAL_SIM_CLOCK_STOP_MODE_T g_palSimClkStopMode[4] = {0};

#ifdef USER_SIM_SWAP

typedef  enum
{
    PAL_SIM_ORDER_SIM_1,
    PAL_SIM_ORDER_SIM_2,
    PAL_SIM_ORDER_SIM_3,
    PAL_SIM_ORDER_SIM_4,
    PAL_SIM_ORDER_SIM_NUM
} PAL_SIM_LIST_T;

PAL_SIM_LIST_T g_palSimList[] = { PAL_SIM_LIST };

#endif

#ifdef FOURSIMD_8851
#define         SWITCH_NUM 2
#elif defined(THREESIMD_8852C)
#define         SWITCH_NUM 0
#else
#define         SWITCH_NUM 1
#endif

// ============================================================================
// pal_SimInit
// ----------------------------------------------------------------------------
// ============================================================================
PUBLIC BOOL pal_SimInit(VOID)
{
    UINT8 i, j;
    for (i=0; i<PAL_MAX_SIM; i++)
    {

#ifdef USER_SIM_SWAP
        j = g_palSimList[i];
#endif

        g_palEtuClkInvFlag[j] = 1;
        g_TS[j] = 0x3B;
        g_palSimClkStopMode[j] = HAL_SIM_CLOCK_NO_STOP;
    }

    hal_SimOpen();

#if !defined(GALLITE_IS_8805)
    drv_simInitVolt(g_SimIndex, SWITCH_NUM);

#else // GALLITE_IS_8805

#ifdef SIM_SWITCH_USED

#ifdef TGT_THREE_SIM

    threesimsw_Open();

    if(threesimsw_SelectSwitch(FOURSIM_SWITCH_0))    // select switch
        return FALSE;
    if(threesimsw_SetSimEnable(DUALSIM_SIM_SELECT_NULL))
        return FALSE;

    pal_SetSimIndex(PAL_SIM_0);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);    //initial sim 0,1 voltage

    if(threesimsw_SelectSwitch(FOURSIM_SWITCH_1))    // select switch
        return FALSE;

    pal_SetSimIndex(PAL_SIM_2);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);    //initial sim 0,1 voltage

#else // !TGT_THREE_SIM

    foursimsw_Open();

    if(foursimsw_SelectSwitch(FOURSIM_SWITCH_0))    // select switch
        return FALSE;
    if(foursimsw_SetSimEnable(DUALSIM_SIM_SELECT_NULL, FOURSIM_SWITCH_0))
        return FALSE;

    pal_SetSimIndex(PAL_SIM_0);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);    //initial sim 0,1 voltage


    if(foursimsw_SelectSwitch(FOURSIM_SWITCH_1))
        return FALSE;
    if(foursimsw_SetSimEnable(DUALSIM_SIM_SELECT_NULL, FOURSIM_SWITCH_1))
        return FALSE;

    pal_SetSimIndex(PAL_SIM_2);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);    //initial sim 2,3 voltage

#endif // !TGT_THREE_SIM

#else // !SIM_SWITCH_USED

#if (NUMBER_OF_SIM == 2)
    // set sim 0 as default sim
    dualsim_Open();
    // Select null as default. Cannot call pal_EnableSimCard here, for
    // 1. DUALSIM_SIM_SELECT_NULL is not a valid SIM index;
    // 2. hal_SimSetDataConvFormat will set SCI_RESET, which will trigger a SIM reset
    dualsim_SetSimEnable(DUALSIM_SIM_SELECT_NULL);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);
#else // NUMBER_OF_SIM != 2
    // SCI init
    // Configure PMIC for level shift
    pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
#endif // NUMBER_OF_SIM != 2

#endif // !SIM_SWITCH_USED

#endif // GALLITE_IS_8805

    return TRUE;
}



// ============================================================================
// pal_SimClose
// ----------------------------------------------------------------------------
// ============================================================================
PUBLIC BOOL pal_SimClose(VOID)
{
    hal_SimClose();

#if !defined(GALLITE_IS_8805)

    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);

#else // GALLITE_IS_8805

#ifdef SIM_SWITCH_USED

#ifdef TGT_THREE_SIM

    if(threesimsw_SelectSwitch(FOURSIM_SWITCH_0))        // select switch 0
        return FALSE;
    pal_SetSimIndex(PAL_SIM_0);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);

    if(threesimsw_SelectSwitch(FOURSIM_SWITCH_1))        // select switch 0
        return FALSE;
    pal_SetSimIndex(PAL_SIM_2);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);

    threesimsw_Close();

#else // !TGT_THREE_SIM

    if(foursimsw_SelectSwitch(FOURSIM_SWITCH_0))        // select switch 0
        return FALSE;
    pal_SetSimIndex(PAL_SIM_0);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);

    if(foursimsw_SelectSwitch(FOURSIM_SWITCH_1))        // select switch 0
        return FALSE;
    pal_SetSimIndex(PAL_SIM_2);
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);

    foursimsw_Close();

#endif // !TGT_THREE_SIM

#else // !SIM_SWITCH_USED

#if (NUMBER_OF_SIM == 2)
    pal_SimSetSimVoltage(PAL_SIM_VOLTAGE_NULL);
    dualsim_Close();
#else // NUMBER_OF_SIM != 2
    // SCI init
    // Configure PMIC for level shift
    pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
#endif // NUMBER_OF_SIM != 2

#endif // !SIM_SWITCH_USED

#endif // GALLITE_IS_8805

    return TRUE;
}


// ============================================================================
//  pal_EnableSimCard
// ----------------------------------------------------------------------------
// This is used to select  sim-card , it enables switching between SIM 0 and
//SIM 1 as well as setting parameters.
/// @paramsim_Index: the sim card index. 0 corresponding to sim card 0,
///and 1 corresponding to sim card 1.
// ============================================================================
PUBLIC BOOL pal_EnableSimCard(UINT8 sim_Index)
{
    INT32 flag = 0;
    UINT32 startTime;

#if defined(GALLITE_IS_8805)
#ifdef SIM_SWITCH_USED
    UINT8 SwitchNum ;
    UINT8 SimNum;
#endif
#endif

#ifdef USER_SIM_SWAP
    sim_Index = g_palSimList[sim_Index];
#endif

#if defined(GALLITE_IS_8805)
#ifdef SIM_SWITCH_USED
    SwitchNum = sim_Index & 0x02;
    SimNum    = sim_Index & 0x01;

    if(SwitchNum == 2)
        SwitchNum = 1;

    if(sim_Index > FOURSIM_SIM_SELECT_CARD_3)
        return FALSE;
#endif
#endif

    PAL_TRACE(PAL_LEV(16), 0, "pal_EnableSimCard simID = %d", sim_Index);

    pal_SetSimIndex(sim_Index);

    // For modem chips,
    // the SIM parameters should be changed after SIM index is updated in HAL,
    // and the HAL update is triggerd in dualsim_SetSimEnable/pmd_SelectSimCard.
    // Further more, there is no need to change the parameters, as
    // one SIM card controller is dedicated to one SIM card.
#ifndef CHIP_HAS_AP
    hal_SimSetEtuDuration(g_palEtuParaF[g_SimIndex], g_palEtuParaD[g_SimIndex], g_palEtuClkInvFlag[g_SimIndex]);
    hal_SimSetClockStopMode(g_palSimClkStopMode[g_SimIndex]);
    hal_SimSetDataConvFormat(g_TS[g_SimIndex]);
#endif

    startTime = hal_TimGetUpTime();

    // Wait for clock stop
    while(hal_SimGetClkStatus() && hal_SimGetClkStopStatus())
    {
        // If time > 1 ms; break
        if (hal_TimGetUpTime() - startTime > 16)
        {
            break;
        }
    }

#if !defined(GALLITE_IS_8805)

    flag = drv_SetSimEnable(sim_Index, SWITCH_NUM);

#else // GALLITE_IS_8805

#ifdef SIM_SWITCH_USED

#ifdef TGT_THREE_SIM
    if(SwitchNum == 0)
    {
        if(threesimsw_SelectSwitch(SwitchNum))    // select switch
            return FALSE;
        flag = threesimsw_SetSimEnable(SimNum);
    }
    else
        flag = threesimsw_SelectSwitch(SwitchNum);
#else // !TGT_THREE_SIM
    if(foursimsw_SelectSwitch(SwitchNum))    // select switch
        return FALSE;
    flag = foursimsw_SetSimEnable(SimNum, SwitchNum);
#endif // !TGT_THREE_SIM

#else // !SIM_SWITCH_USED

    flag = dualsim_SetSimEnable(sim_Index);

#endif // !SIM_SWITCH_USED

#endif // GALLITE_IS_8805

    return (flag == 0);
}


PUBLIC BOOL  pal_SelectSwitch()
{
#ifdef SIM_SWITCH_USED

    UINT8 number, i;
    pal_GetSimIndex(&i);

    if(i & 0x02)
        number = 1;
    else
        number = 0;

#ifdef TGT_THREE_SIM
    threesimsw_SelectSwitch(number);
#else
    foursimsw_SelectSwitch(number);
#endif

    return TRUE;

#else // !SIM_SWITCH_USED

    return FALSE;

#endif // !SIM_SWITCH_USED
}


// ============================================================================
// pal_SetSimIndex
// ----------------------------------------------------------------------------
/// This function used to set SIM index.
/// @param sim_Index is SIM index
// ============================================================================
VOID pal_SetSimIndex(UINT8 sim_Index)
{
    g_SimIndex = sim_Index;
}


// ============================================================================
// pal_GetSimIndex
// ----------------------------------------------------------------------------
/// This function used to set SIM index.
/// @param sim_Index is SIM index
// ============================================================================
VOID pal_GetSimIndex(UINT8 *sim_Index)
{
    *sim_Index = g_SimIndex;
}


PUBLIC VOID pal_SimSetClockStopMode(HAL_SIM_CLOCK_STOP_MODE_T mode)
{
    g_palSimClkStopMode[g_SimIndex] = mode;
    hal_SimSetClockStopMode(mode);
}


// ============================================================================
// pal_SimSetSimVoltage
// ----------------------------------------------------------------------------
/// WARNING
/// This function is called under IRQ. Fancy things like
/// call to sxr_Sleep or mailbox creating are FORBIDDEN
/// This function sets the SIM voltage.
/// four card case: need two step, step 1 set gallite 1203 voltage, step to set 6302 switch voltage
/// @param level Voltage to set.
// ============================================================================
PUBLIC BOOL pal_SimSetSimVoltage(UINT8 level)
{
    INT32 flag = 0;

#if !defined(GALLITE_IS_8805)

    UINT8 sim0Level = 0;
    UINT8 sim1Level = 0;
    UINT8 sim2Level = 0;
    UINT8 sim3Level = 0;

    PAL_TRACE(PAL_LEV(16), 0, "pal_SimSetSimVoltage power level = %d, g_SimIndex = %d", level, g_SimIndex);

    if (g_SimIndex == 0)
    {
        sim0Level = level;
        sim1Level = g_Sim1VoltLevel;
        sim2Level = g_Sim2VoltLevel;
        sim3Level = g_Sim3VoltLevel;

        g_Sim0VoltLevel = level;
    }
    else if (g_SimIndex == 1)
    {
        sim0Level = g_Sim0VoltLevel;
        sim1Level = level;
        sim2Level = g_Sim2VoltLevel;
        sim3Level = g_Sim3VoltLevel;

        g_Sim1VoltLevel = level;
    }
    else if (g_SimIndex == 2)
    {
        sim0Level = g_Sim0VoltLevel;
        sim1Level = g_Sim1VoltLevel;
        sim2Level = level;
        sim3Level = g_Sim3VoltLevel;

        g_Sim2VoltLevel = level;
    }
    else if (g_SimIndex == 3)
    {
        sim0Level = g_Sim0VoltLevel;
        sim1Level = g_Sim1VoltLevel;
        sim2Level = g_Sim2VoltLevel;
        sim3Level = level;

        g_Sim3VoltLevel = level;
    }

    flag = foursim_SetSimVoltage(sim0Level, sim1Level, sim2Level, sim3Level, g_SimIndex);


#else // GALLITE_IS_8805

#ifdef SIM_SWITCH_USED

    UINT8 sim0Level = 0;
    UINT8 sim1Level = 0;
    UINT8 CsIdx = 0;

    PAL_TRACE(PAL_LEV(16), 0, "(pal_SimSetSimVoltage) FOUR CARD power level = %d, g_SimIndex = %d", level, g_SimIndex);

#ifdef TGT_THREE_SIM
    if(g_SimIndex < 2)
        flag = threesimsw_SetGalliteVoltage(DUALSIM_SIM_VOLTAGE_CLASS_B, g_Sim2VoltLevel);
    else
        flag = threesimsw_SetGalliteVoltage(DUALSIM_SIM_VOLTAGE_CLASS_B, level);
#else
    flag = foursimsw_SetGalliteVoltage(DUALSIM_SIM_VOLTAGE_CLASS_B, DUALSIM_SIM_VOLTAGE_CLASS_B);
#endif

    if(flag)
        return (!flag);

    if (g_SimIndex == 0)
    {
        sim0Level = level;
        sim1Level = g_Sim1VoltLevel;

        g_Sim0VoltLevel = level;
    }
    else if (g_SimIndex == 1)
    {
        sim0Level = g_Sim0VoltLevel;
        sim1Level = level;

        g_Sim1VoltLevel = level;
    }
    else if (g_SimIndex == 2)
    {
        sim0Level = level;
        sim1Level = g_Sim3VoltLevel;

        g_Sim2VoltLevel = level;
        CsIdx = 1;
    }
    else if (g_SimIndex == 3)
    {
        sim0Level = g_Sim2VoltLevel;
        sim1Level = level;

        g_Sim3VoltLevel = level;
        CsIdx = 1;
    }

#ifdef TGT_THREE_SIM
    if(g_SimIndex < 2)
        flag = threesimsw_SetSimVoltage(sim0Level, sim1Level);
#else
    flag = foursimsw_SetSimVoltage(sim0Level, sim1Level, CsIdx);
#endif

#else // !SIM_SWITCH_USED

#if (NUMBER_OF_SIM == 2)

    UINT8 sim0Level = 0;
    UINT8 sim1Level = 0;

    PAL_TRACE(PAL_LEV(16), 0, "(pal_SimSetSimVoltage) power level = %d, g_SimIndex = %d", level, g_SimIndex);

    if (g_SimIndex == 0)
    {
        sim0Level = level;
        sim1Level = g_Sim1VoltLevel;

        g_Sim0VoltLevel = level;
    }
    else if (g_SimIndex == 1)
    {
        sim0Level = g_Sim0VoltLevel;
        sim1Level = level;

        g_Sim1VoltLevel = level;
    }

    flag = dualsim_SetSimVoltage(sim0Level, sim1Level);

#else // NUMBER_OF_SIM != 2

    UINT32 delay;

    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)

    delay = SETVOLT_DLY;

    switch (level)
    {
        case PAL_SIM_VOLTAGE_CLASS_A:
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_5V0);
            break;
        case PAL_SIM_VOLTAGE_CLASS_B:
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_3V0);
            break;
        case PAL_SIM_VOLTAGE_CLASS_C:
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_1V8);
            break;
        case PAL_SIM_VOLTAGE_NULL:
            pmd_SetLevel(PMD_LEVEL_SIM, PMD_SIM_VOLTAGE_0V0);
            // Add extra delay for voltage 0
            delay += NULLVOLT_DLY;
            break;
    }

    hal_TimDelay(delay MS_WAITING);

#endif // NUMBER_OF_SIM != 2

#endif // !SIM_SWITCH_USED

#endif // GALLITE_IS_8805

    return (flag == 0);
}


// ============================================================================
// pal_SimSetEtuDuration
// ----------------------------------------------------------------------------
/// Select the clock rate.
/// @param etuDuration F param to set.
/// @param etuDuration D param to set.
// ============================================================================
PUBLIC VOID pal_SimSetEtuDuration (UINT16 F, UINT8 D)
{
    hal_SimSetEtuDuration(F, D, g_palEtuClkInvFlag[g_SimIndex]);

    g_palEtuParaF[g_SimIndex] = F;
    g_palEtuParaD[g_SimIndex] = D;
}


// ============================================================================
// pal_SetTS
// ----------------------------------------------------------------------------
/// This function used to set ATR TS value.
/// @param TS is TS value from the ATR response
// ============================================================================
PUBLIC BOOL  pal_SetTS(UINT8 TS)
{
    extern UINT32 hal_SimGetDataConvFormat();
    UINT32 simDataConv = hal_SimGetDataConvFormat();
    BOOL needReset = FALSE;

    if(simDataConv == 1)
    {
        if(TS == 0x23)
        {
            TS = 0x3B;
            hal_SimSetDataConvFormat(TS);
            needReset = TRUE;
        }
    }
    else if(simDataConv == 2)
    {
        if(TS == 0x03)
        {
            TS = 0x3F;
            hal_SimSetDataConvFormat(TS);
            needReset = TRUE;
        }
    }
    else
    {
        PAL_ASSERT(FALSE, "Unsupported SIM data convention");
    }

    if(TS == 0x3B || TS == 0x3F)
    {
        g_TS[g_SimIndex] = TS;
    }
    else
    {
        needReset = TRUE;
        SXS_TRACE(_SIM, " Bad TS \n");
    }

    return needReset;
}


// ============================================================================
// pal_getTS
// ----------------------------------------------------------------------------
/// This function used to get ATR TS value.
/// @param g_TS is TS value
// ============================================================================
PUBLIC UINT8 pal_getTS()
{
    return g_TS[g_SimIndex];
}


PUBLIC VOID pal_SimClkInv()
{
    g_palEtuClkInvFlag[g_SimIndex] = 1 - g_palEtuClkInvFlag[g_SimIndex];
    hal_SimSetEtuDuration(g_palEtuParaF[g_SimIndex], g_palEtuParaD[g_SimIndex],  g_palEtuClkInvFlag[g_SimIndex]);
}


#endif // PAL_SIMD_IMPLEMENTATION


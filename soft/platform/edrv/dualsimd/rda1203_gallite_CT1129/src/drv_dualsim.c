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
#include "cs_types.h"
#include "stdlib.h"
#include "sxs_io.h"

#include "hal_timers.h"
#include "drv_dualsim.h"

#include "sxr_tls.h"
#include "hal_host.h"

#define        DS_SETVOLT_DLY           (0) // delay in 1/1000 sec
//#define        DS_SETVOLT_DLY           (50) // delay in 1/1000 sec
#define        DS_SETSIMEN_DLY          (0)
//#define        DS_SETSIMEN_DLY          (1)


extern DUALSIM_ERR_T pmd_FourSimSetVoltageClass(DUALSIM_SIM_VOLTAGE_T voltClass0,DUALSIM_SIM_VOLTAGE_T voltClass1,
        DUALSIM_SIM_VOLTAGE_T voltClass2,DUALSIM_SIM_VOLTAGE_T voltClass3);

extern BOOL pmd_SelectSimCard(UINT8 sim_card);


// ======================================================================
// dualsim_Open
// ----------------------------------------------------------------------
/// This function Open Spi for dualsim
// ======================================================================
PUBLIC VOID dualsim_Open(VOID)
{
}

// ======================================================================
// dualsim_Close
// ----------------------------------------------------------------------
/// This function Close dualsim Spi
// ======================================================================

PUBLIC VOID  dualsim_Close(VOID)
{
}

// ==========================================================================
// foursim_SetSimVoltage
// --------------------------------------------------------------------------
/// This function set sim0-3 Voltage
// ==========================================================================

PUBLIC DUALSIM_ERR_T foursim_SetSimVoltage(
    DUALSIM_SIM_VOLTAGE_T sim0Level,
    DUALSIM_SIM_VOLTAGE_T sim1Level,
    DUALSIM_SIM_VOLTAGE_T sim2Level,
    DUALSIM_SIM_VOLTAGE_T sim3Level,
    UINT8                   SimID)
{
    UINT32 delay;
    DUALSIM_ERR_T  errStatus;
    // Level 0 = Off
    // Level 1 = Class C (1.8V)
    // Level 2 = Class B (2.7 - 3.3V)
    // Level 3 = Class A (5V)  //2.8V

    errStatus = pmd_FourSimSetVoltageClass(sim0Level, sim1Level, sim2Level, sim3Level);

    if (errStatus == DUALSIM_ERR_NO)
    {
        delay = DS_SETVOLT_DLY;
        hal_TimDelay(delay MS_WAITING);
    }

    return errStatus;
}

// ======================================================================
// foursim_SetSimEnable
// ----------------------------------------------------------------------
/// This function set one of the sim card enabled
// ======================================================================
PUBLIC DUALSIM_ERR_T drv_SetSimEnable(DUALSIM_SIM_CARD_T number, UINT8 SwitchNum)
{
    UINT32 delay;

    if (number < FOURSIM_SIM_SELECT_CARD_0 &&
            number > FOURSIM_SIM_SELECT_CARD_3)
    {
        return DUALSIM_ERR_NO;
    }

    delay = DS_SETSIMEN_DLY;
    hal_TimDelay(delay MS_WAITING);

    if (pmd_SelectSimCard(number))
    {
        delay = DS_SETSIMEN_DLY;
        hal_TimDelay(delay MS_WAITING);

        return DUALSIM_ERR_NO;
    }

    return DUALSIM_ERR_RESOURCE_BUSY;
}

PUBLIC DUALSIM_ERR_T drv_simInitVolt(UINT8 SimID, UINT8 SwitchNum)
{
    DUALSIM_ERR_T  errStatus;
    errStatus = foursim_SetSimVoltage(DUALSIM_SIM_VOLTAGE_NULL, DUALSIM_SIM_VOLTAGE_NULL,
                                      DUALSIM_SIM_VOLTAGE_NULL, DUALSIM_SIM_VOLTAGE_NULL, SimID);

    return errStatus;
}


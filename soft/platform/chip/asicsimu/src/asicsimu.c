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

#include "chip_id.h"
#include "asicsimu_m.h"

#include "global_macros.h"
#include "sys_ctrl.h"
#include "debug_port.h"
#include "tester_chip.h"
#include "cfg_regs.h"

#include "mem_bridge.h"
#include "hal_ebc.h"


// =============================================================================
//  MACROS
// =============================================================================


// CS3 configuration refer to asicenv chip_test/drivers/src/configuration.h
// however tester chip run on sys_clock out were ebc is on mem_clock
// so the timing must keep a margin
//
#define CS3_TIME_SETTING { \
    .rdLatHalfMode= 0, \
    .rdLat=         4, \
    .oedlyHalfMode= 0, \
    .oedly=         1, \
    .pageLat=       0, \
    .wrLatHalfMode= 0, \
    .wrLat=         5, \
    .wedlyHalfMode= 0, \
    .wedly=         1, \
    .relax=         1, \
    .forceRelax=    0}
#define CS3_MODE_SETTING { \
    .csEn=          1, \
    .polarity=      0, \
    .admuxMode=     0, \
    .writeAllow=    1, \
    .wbeMode=       0, \
    .bedlyMode=     0, \
    .waitMode=      0, \
    .pageSize=      0, \
    .pageMode=      0, \
    .writePageMode= 0, \
    .burstMode=     0, \
    .writeBurstMode=0, \
    .advAsync=      0, \
    .advwePulse=    0, \
    .admuxDly=      0, \
    .writeSingle=   0, \
    .writeHold=     0}

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

HWP_TESTER_CHIP_T* g_asicsimu_hwpTesterChip=0;

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// asicsimu_Open
// -----------------------------------------------------------------------------
/// This function configure the access to the tester chip.
/// It must be called BEFORE starting the BCPU to ensure the init is correct.
// =============================================================================
PUBLIC VOID asicsimu_Open(VOID)
{
    HAL_EBC_CS_CFG_T testerChipCfg = {CS3_TIME_SETTING,CS3_MODE_SETTING};
    //TODO: check register state before and warn if someting is changed
    // clock out for tester chip
    hwp_debugPort->Debug_Port_Mode = DEBUG_PORT_MODE_CONFIG_MODE_EXL;

    // Enable CS3
    hwp_configRegs->GPIO_Mode &= ~CFG_REGS_MODE_PIN_M_CS_3;

    g_asicsimu_hwpTesterChip = (HWP_TESTER_CHIP_T*) hal_EbcCsOpen(HAL_EBC_CS3, &testerChipCfg);

}


// =============================================================================
// asicsimu_Print
// -----------------------------------------------------------------------------
/// This function Prints a string on the simulator output terminal.
/// @param string the message to print (NO FORMATING)
/// NOTE: the print in multi cpu safe (it can safely be called by XCPU or BCPU)
/// however it is not Interrupt (Task&job) safe: once a print is started,
/// starting a new print with the same CPU will result in an endless loop !
/// either use critical section arround it or make sure the interrupt code is
/// not using it.
// =============================================================================
PUBLIC VOID asicsimu_Print(CHAR * string)
{
    while (g_asicsimu_hwpTesterChip->STR_Print != 0); // wait that nobody use the print...
    while (*string)
    {
        g_asicsimu_hwpTesterChip->STR_Print = (*string++);
    }
    // send the string
    g_asicsimu_hwpTesterChip->STR_Print = 0;

    {u32 flush __attribute__ ((unused)) = hwp_memFifo->Flush;}

}

// =============================================================================
// asicsimu_Stop
// -----------------------------------------------------------------------------
/// This function stop the simulation
/// @param success whether or not the simulation is succesfull
// =============================================================================
PUBLIC VOID asicsimu_Stop(BOOL success)
{
    g_asicsimu_hwpTesterChip->Stopper = (success)?TESTER_CHIP_STOP_CAUSE_SUCCESS:TESTER_CHIP_STOP_CAUSE_ERROR;
    {u32 flush __attribute__ ((unused)) = hwp_memFifo->Flush;}
}

// =============================================================================
// asicsimu_Fatal
// -----------------------------------------------------------------------------
/// This function Prints a string on the simulator output terminal.
/// @param string the message to print (NO FORMATING)
/// Note: - This function is not CPU safe (as #asicsimu_Print is)
///       - This function will force #asicsimu_Open if not done
///       - This fucntion will steal the print resource to ensure the message
///         is printed
///       - This function will ternimate on error as asicsimu_Stop(FALSE)
///       .
// =============================================================================
PUBLIC VOID asicsimu_Fatal(CHAR * string)
{
    if(g_asicsimu_hwpTesterChip == 0)
    {
        asicsimu_Open();
    }
    if (g_asicsimu_hwpTesterChip->STR_Print != 0)
    {
        // terminate the started print (if the other CPU is using it it will look strange be we want to quit...
        g_asicsimu_hwpTesterChip->STR_Print = 0;
    }
    while (*string)
    {
        g_asicsimu_hwpTesterChip->STR_Print = (*string++);
    }
    // send the string
    g_asicsimu_hwpTesterChip->STR_Print = 0;

    // Stop with error flag
    g_asicsimu_hwpTesterChip->Stopper = TESTER_CHIP_STOP_CAUSE_ERROR;

    {u32 flush __attribute__ ((unused)) = hwp_memFifo->Flush;}
}

// =============================================================================
// asicsimu_SetWatch
// -----------------------------------------------------------------------------
/// This function set the Watch register value
/// @param value the value to write
// =============================================================================
PUBLIC VOID asicsimu_SetWatch(UINT16 value)
{
    *((VOLATILE UINT16*)&g_asicsimu_hwpTesterChip->Watch) = value;
}

// =============================================================================
// asicsimu_GetWatch
// -----------------------------------------------------------------------------
/// This function Get the Watch register value
/// @return the read value
// =============================================================================
PUBLIC UINT16 asicsimu_GetWatch(VOID)
{
    return *((VOLATILE UINT16*)&g_asicsimu_hwpTesterChip->Watch);
}

// =============================================================================
// asicsimu_SetW0
// -----------------------------------------------------------------------------
/// This function set the W0 register value
/// @param value the value to write
// =============================================================================
PUBLIC VOID asicsimu_SetW0(UINT32 value)
{
    g_asicsimu_hwpTesterChip->W0 = value;
}

// =============================================================================
// asicsimu_GetW0
// -----------------------------------------------------------------------------
/// This function Get the W0 register value
/// @return the read value
// =============================================================================
PUBLIC UINT32 asicsimu_GetW0(VOID)
{
    return g_asicsimu_hwpTesterChip->W0;
}


// =============================================================================
// asicsimu_SetW1
// -----------------------------------------------------------------------------
/// This function set the W1 register value
/// @param value the value to write
// =============================================================================
PUBLIC VOID asicsimu_SetW1(UINT32 value)
{
    g_asicsimu_hwpTesterChip->W1 = value;
}

// =============================================================================
// asicsimu_GetW1
// -----------------------------------------------------------------------------
/// This function Get the W1 register value
/// @return the read value
// =============================================================================
PUBLIC UINT32 asicsimu_GetW1(VOID)
{
    return g_asicsimu_hwpTesterChip->W1;
}


// =============================================================================
// asicsimu_SetW2
// -----------------------------------------------------------------------------
/// This function set the bit to 1 in W2 register
/// @param value the bits to set to 1
// =============================================================================
PUBLIC VOID asicsimu_SetW2(UINT32 value)
{
    g_asicsimu_hwpTesterChip->W2_set = value;
}

// =============================================================================
// asicsimu_ClrW2
// -----------------------------------------------------------------------------
/// This function clear the bit to 0 in W2 register
/// @param value the bits to clear to 0
// =============================================================================
PUBLIC VOID asicsimu_ClrW2(UINT32 value)
{
    g_asicsimu_hwpTesterChip->W2_clr = value;
}

// =============================================================================
// asicsimu_GetW2
// -----------------------------------------------------------------------------
/// This function Get the W2 register value
/// @return the read value
// =============================================================================
PUBLIC UINT32 asicsimu_GetW2(VOID)
{
    return g_asicsimu_hwpTesterChip->W2_set;
}


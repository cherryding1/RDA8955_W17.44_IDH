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



#include "global_macros.h"
#include "boot_ispi.h"
#include "hal_ispi.h"
#include "hal_map_engine.h"
#include "bootp_key.h"
#include "bootp_sector.h"
#include "bootp_host.h"
#include "keypad.h"
#include "key_defs.h"
#undef KP_CTRL
#include "timer.h"

#include "tgt_m.h"

// ============================================================================
// TYPES
// ============================================================================

// =============================================================================
//  MACROS
// =============================================================================

/// The high 16 bits of force-monitor key status events
#define BOOT_FORCE_MONITOR_KEY_EVENT_PREFIX (0xFE900000)

/// The address and mask for AC_ON checking.
#define RDA_ADDR_CHARGER_STATUS     0x14
#define RDA_PMU_CHR_AC_ON           (1<<7)

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// ============================================================================
// FUNCTIONS
// ============================================================================

// =============================================================================
// boot_MonitorKeyPressed
// -----------------------------------------------------------------------------
/// Check whether all keys in the list are pressed.
// =============================================================================
PROTECTED BOOL boot_MonitorKeyPressed(CONST UINT8 * monKeys, UINT32 listLen)
{
    if (monKeys == NULL || listLen == 0)
        return FALSE;

    // Assume all the key-in and key-out lines are in use
    hwp_keypad->KP_IRQ_MASK = 0;
    hwp_keypad->KP_IRQ_CLR = KEYPAD_KP_IRQ_CLR;
    hwp_keypad->KP_CTRL  = KEYPAD_KP_DBN_TIME(0)
                           | KEYPAD_KP_ITV_TIME(0)
                           | KEYPAD_KP_EN | KEYPAD_KP_IN_MASK(0xff)
                           | KEYPAD_KP_OUT_MASK(0xff);

    // 0.3125 * keyOutLines * ( debouceTime + 1 ) =
    // 0.3125 * 8 * (0 + 1) ~= 3 ms
#define DEBOUNCE_TIME ((3 * 2 + 1) * 16384 / 1000)

    UINT32 startTime = hwp_timer->HWTimer_CurVal;
    while (hwp_timer->HWTimer_CurVal - startTime < DEBOUNCE_TIME);

    UINT32 index;
    UINT32 key;
    BOOL down;
    BOOL forceMon = TRUE;
    for (UINT32 i=0; i<listLen; i++)
    {
        index = tgt_GetKeyMapIndex(monKeys[i]);
        if (index < LOW_KEY_NB)
        {
            key = GET_BITFIELD(hwp_keypad->KP_DATA_L, KEYPAD_KP_DATA_L);
            down = ((key & (1<<index)) != 0);
        }
        else
        {
            key = GET_BITFIELD(hwp_keypad->KP_DATA_H, KEYPAD_KP_DATA_H);
            down = ((key & (1<<(index-LOW_KEY_NB))) != 0);
        }
        mon_Event(BOOT_FORCE_MONITOR_KEY_EVENT_PREFIX|(index<<8)|down);
        if (!down)
        {
            forceMon = FALSE;
        }
    }
    return forceMon;
}

// =============================================================================
// boot_IspiRead
// -----------------------------------------------------------------------------
/// Read ISPI register
// =============================================================================
PROTECTED UINT16 boot_IspiRead(BOOT_ISPI_CS_T csId, UINT16 regIdx)
{
    UINT32 wrData, rdData;
    UINT32 count;
    UINT16 value;

    wrData = (1<<25) | ((regIdx & 0x1ff) << 16) | 0;

    while(boot_IspiTxFifoAvail() < 1 ||
            boot_IspiSendData(csId, wrData, TRUE) == 0);

    //wait until any previous transfers have ended
    while(!boot_IspiTxFinished());
    count = boot_IspiGetData(&rdData);

    value = (UINT16)(rdData & 0xffff);
    return value;
}

// =============================================================================
// boot_ForceMonitorKeyCheck
// -----------------------------------------------------------------------------
/// Enter boot monitor if all of the force-monitor keys are pressed.
// =============================================================================
PROTECTED VOID boot_ForceMonitorKeyCheck(VOID)
{
#ifndef RAMRUN
    // If power-on key is pressed ...
    if (hwp_keypad->KP_STATUS & KEYPAD_KP_ON)
    {
        CONST UINT8 *monKeys = NULL;
        UINT32 listLen = 0;

        tgt_GetBootDownloadKeyList(&monKeys, &listLen);

        if (boot_MonitorKeyPressed(monKeys, listLen))
        {
            mon_Event(BOOT_SECTOR_EVENT_ENTER_MONITOR);
            boot_SectorEnterBootMonitor();
        }
    }
#endif // !RAMRUN
}

// =============================================================================
// boot_ForceUsbMonitorKeyCheck
// -----------------------------------------------------------------------------
/// Enter boot monitor if all of the usb-force-monitor keys are pressed with AC_ON.
// =============================================================================
PROTECTED VOID boot_ForceUsbMonitorKeyCheck()
{
#ifdef FPGA
return;
#endif
#ifndef RAMRUN
    UINT16 status = boot_IspiRead(BOOT_ISPI_PIN_CS_PMU, RDA_ADDR_CHARGER_STATUS);
    if ((status & RDA_PMU_CHR_AC_ON) == 0)
        return;

    CONST UINT8 *monKeys = NULL;
    UINT32 listLen = 0;

    tgt_GetBootUsbDownloadKeyList(&monKeys, &listLen);

    if (boot_MonitorKeyPressed(monKeys, listLen))
    {
        hal_MapEngineOpen();
        tgt_RegisterMonitorModules();

        mon_Event(BOOT_SECTOR_EVENT_ENTER_MONITOR);
        boot_SectorEnterBootMonitor();
    }
#endif // !RAMRUN
}

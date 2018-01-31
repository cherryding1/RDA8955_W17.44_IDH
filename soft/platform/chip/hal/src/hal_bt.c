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

#include "global_macros.h"
#include "sys_ctrl.h"

#include "chip_id.h"

#include "halp_bt.h"
#include "halp_sys.h"

#include "hal_bt.h"
#include "reg_alias.h"



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// btcpu_main_entry
// -----------------------------------------------------------------------------
/// Pointer to the first function lauched by the btcpu.
// =============================================================================
EXPORT UINT32* btcpu_main_entry;

// =============================================================================
// btcpu_int_entry
// -----------------------------------------------------------------------------
/// Pointer to the function to handle IRQ.
// =============================================================================
EXPORT UINT32* btcpu_int_entry;

// =============================================================================
// btcpu_sp_context
// -----------------------------------------------------------------------------
/// Variable holding the pointer where to save the btcpu context
// =============================================================================
EXPORT UINT32* btcpu_sp_context;

// =============================================================================
// btcpu_error_code
// -----------------------------------------------------------------------------
/// Global variable holding the btcpu error code.
// =============================================================================
EXPORT volatile UINT32 btcpu_error_code;

// =============================================================================
// btcpu_error_status
// -----------------------------------------------------------------------------
/// Global variable holding the btcpu error status.
// =============================================================================
EXPORT volatile UINT32 btcpu_error_status;

// =============================================================================
// btcpu_stack_base
// -----------------------------------------------------------------------------
/// Variable holding the stack pointer for the btcpu. This variable must
/// be set before the btcpu is started.
// =============================================================================
EXPORT UINT32 btcpu_stack_base;

// =============================================================================
// g_halBtHostIrqHandler
// -----------------------------------------------------------------------------
/// Variable holding the user irq handler to process data sent from BT.
// =============================================================================
HAL_BT_HOST_IRQ_HANDLER_T g_halBtHostIrqHandler = NULL;

// =============================================================================
// g_halBtStarted
// -----------------------------------------------------------------------------
/// Variable to tell whether BT is started.
// =============================================================================
BOOL g_halBtStarted = FALSE;


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_BtSetIrqHandler
// -----------------------------------------------------------------------------
/// Set an function called when an irq indicating that BT has sent some data
/// to host.
// =============================================================================
PUBLIC VOID hal_BtSetHostIrqHandler(HAL_BT_HOST_IRQ_HANDLER_T handler)
{
    g_halBtHostIrqHandler = handler;

    UINT32 status = hal_SysEnterCriticalSection();
    if (g_halBtStarted)
    {
        if (handler)
        {
            hwp_sysCtrl->BT_Cfg |= SYS_CTRL_BT_IRQ_MASK_DATAIN;
        }
        else
        {
            hwp_sysCtrl->BT_Cfg &= ~SYS_CTRL_BT_IRQ_MASK_DATAIN;
        }
    }
    hal_SysExitCriticalSection(status);
}

// =============================================================================
// hal_BtIrqHandler
// -----------------------------------------------------------------------------
/// Handles BT IRQ.
// =============================================================================
PROTECTED VOID hal_BtIrqHandler(UINT8 interruptId)
{
    UINT32 status;
    if (hwp_sysCtrl->BT_Cfg & SYS_CTRL_BT_WAKE_STATUS)
    {
        hal_SwRequestClk(FOURCC_BT, HAL_SYS_FREQ_104M);
        status = hal_SysEnterCriticalSection();
        hwp_sysCtrl->BT_Cfg &= ~(SYS_CTRL_BT_IRQ_MASK_WAKE|SYS_CTRL_BT_WAKEBT);
        hal_SysExitCriticalSection(status);
    }
    if (hwp_sysCtrl->BT_Cfg & SYS_CTRL_BT_DATAIN_STATUS)
    {
        if (g_halBtHostIrqHandler)
        {
            // Tell BT to clear the IRQ cause, and
            // disallow BT to send more data
            status = hal_SysEnterCriticalSection();
            hwp_sysCtrl->BT_Cfg &= ~SYS_CTRL_BT_DATAIN_READY;
            hal_SysExitCriticalSection(status);
            // Process the data
            (*g_halBtHostIrqHandler)();
            // Allow BT to send next data
            status = hal_SysEnterCriticalSection();
            hwp_sysCtrl->BT_Cfg |= SYS_CTRL_BT_DATAIN_READY;
            hal_SysExitCriticalSection(status);
        }
        else
        {
            // No user handler yet. Just disable the irq
            status = hal_SysEnterCriticalSection();
            hwp_sysCtrl->BT_Cfg &= ~SYS_CTRL_BT_IRQ_MASK_DATAIN;
            hal_SysExitCriticalSection(status);
        }
    }
}


// =============================================================================
// hal_BtSleep
// -----------------------------------------------------------------------------
/// Try to release frequence for BT if it is in sleep state.
// =============================================================================
PROTECTED VOID hal_BtSleep(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    if ((hwp_sysCtrl->BT_Cfg & SYS_CTRL_BT_WAKE_STATUS) == 0)
    {
        hal_SwReleaseClk(FOURCC_BT);
        hwp_sysCtrl->BT_Cfg |= SYS_CTRL_BT_IRQ_MASK_WAKE;
    }
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_BtStart
// -----------------------------------------------------------------------------
/// Start BT CPU.
// =============================================================================
PUBLIC VOID hal_BtStart(VOID *main, VOID *irqHdlr, VOID *stackStartAddr)
{
    btcpu_stack_base = (UINT32)stackStartAddr;
    btcpu_int_entry = (UINT32 *)irqHdlr;
    btcpu_main_entry = (UINT32 *)main;

    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Sys_Enable2 = SYS_CTRL_ENABLE_SYS_BT;
    hwp_sysCtrl->Clk_Other_Enable = SYS_CTRL_ENABLE_OC_BT26M|SYS_CTRL_ENABLE_OC_BT32K;
    hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_CLR_RST_BT;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    hwp_sysCtrl->BT_Cfg |= SYS_CTRL_BT_IRQ_MASK_WAKE|
                           SYS_CTRL_BT_IRQ_MASK_DATAIN|
                           SYS_CTRL_BT_WAKEBT;
    g_halBtStarted = TRUE;
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_BtStop
// -----------------------------------------------------------------------------
/// Stop BT CPU.
// =============================================================================
PUBLIC VOID hal_BtStop(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SET_RST_BT;
    hwp_sysCtrl->Clk_Other_Disable = SYS_CTRL_DISABLE_OC_BT26M|SYS_CTRL_DISABLE_OC_BT32K;
    hwp_sysCtrl->Clk_Sys_Disable2 = SYS_CTRL_DISABLE_SYS_BT;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;

    hwp_sysCtrl->BT_Cfg &= ~(SYS_CTRL_BT_IRQ_MASK_MASK|SYS_CTRL_BT_WAKEBT);
    g_halBtStarted = FALSE;
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_BtWakeUp
// -----------------------------------------------------------------------------
/// Wake up BT CPU.
// =============================================================================
PUBLIC VOID hal_BtWakeUp(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_sysCtrl->BT_Cfg |= SYS_CTRL_BT_IRQ_MASK_WAKE|SYS_CTRL_BT_WAKEBT;
    hal_SysExitCriticalSection(status);
}


// =============================================================================
// hal_BtNotifyDataSent
// -----------------------------------------------------------------------------
/// Tell BT that host just sends some data.
// =============================================================================
PUBLIC VOID hal_BtNotifyDataSent(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();
    // Toggle SYS_CTRL_BT_DATA2BT bit
    if (hwp_sysCtrl->BT_Cfg & SYS_CTRL_BT_DATA2BT)
    {
        hwp_sysCtrl->BT_Cfg &= ~SYS_CTRL_BT_DATA2BT;
    }
    else
    {
        hwp_sysCtrl->BT_Cfg |= SYS_CTRL_BT_DATA2BT;
    }
    hal_SysExitCriticalSection(status);
}


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

#include "global_macros.h"
#include "mem_bridge.h"
#include "page_spy.h"

#include "hal_ebc.h"
#include "hal_mem_map.h"
#include "hal_sys.h"
#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#endif

#include "halp_sys.h"
#include "halp_ebc.h"
#include "halp_debug.h"

#include "boot_mem_init.h"
#include "boot_sector_driver.h"


// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is false,
/// raise an assert.
// =============================================================================
#define EBC_ASSERT(condition, format, ...) \
    if (UNLIKELY(!(condition))) { asm volatile("break 2"); }

/// Wait for the EBC config to be coherent
/// WARNING: When the BB side is running, on a multi-fifo
/// system, this must be implemented using the EBC_Status
/// CS_Update bit.
/// (Reading the Fifo flush just ensure the fifo is empty).
INLINE VOID hal_EbcWaitReady(VOID)
{
#ifdef USE_OPIDDR_PSRAM
#else
    UINT32 flush __attribute__((unused)) = hwp_memFifo->Flush;
#endif
}

/// TODO Implement the remap driver.

PUBLIC VOID hal_EbcConfigRemap(HAL_EBC_FLSH_PHYSADD_T physAddr)
{
#ifdef USE_OPIDDR_PSRAM
#else
    hwp_memFifo->EBC_CS4_Block_Remap = physAddr;
#endif
}


#ifdef CHIP_HAS_AP
PRIVATE CONST char gc_errInvalidMemScId[] = "Invalid MemSC ID: %d";

// ============================================================================
// hal_MemScTake
// ----------------------------------------------------------------------------
/// Try to take a hardware general purpose semaphore.
///
/// @param id The semaphore ID
/// @return 0 if failure; otherwise success
// ============================================================================
PUBLIC UINT32 DDR_SLEEP_FUNC hal_MemScTake(HAL_MEM_SC_T id)
{
    EBC_ASSERT(id < HAL_MEM_SC_QTY, gc_errInvalidMemScId, id);
#if 0
    return hwp_memBridge->MemSC[id];
#else
    return 1;
#endif
}


// ============================================================================
// hal_MemScRelease
// ----------------------------------------------------------------------------
/// Release a hardware general purpose semaphore.
///
/// @param id The semaphore ID
// ============================================================================
PUBLIC VOID DDR_SLEEP_FUNC hal_MemScRelease(HAL_MEM_SC_T id)
{
    EBC_ASSERT(id < HAL_MEM_SC_QTY, gc_errInvalidMemScId, id);
#if 0
    hwp_memBridge->MemSC[id] = 1;
#endif
}


// ============================================================================
// hal_MemScGetStatus
// ----------------------------------------------------------------------------
/// Get the status of all hardware general purpose semaphores.
///
/// @return status
// ============================================================================
PUBLIC UINT32 hal_MemScGetStatus(VOID)
{
#if 0
    return hwp_memBridge->MemSC_Status;
#else
    return 0xFFFFFFFF;
#endif
}


// ============================================================================
// hal_MemScGetStatusById
// ----------------------------------------------------------------------------
/// Get the status of a hardware general purpose semaphore.
///
/// @param id The semaphore ID
/// @return status
// ============================================================================
PUBLIC UINT32 hal_MemScGetStatusById(HAL_MEM_SC_T id)
{
    EBC_ASSERT(id < HAL_MEM_SC_QTY, gc_errInvalidMemScId, id);
#if 0
    return (hwp_memBridge->MemSC_Status & (1 << id)) ? 1 : 0;
#else
    return 1;
#endif
}


#else // !CHIP_HAS_AP
// ============================================================================
// hal_EbcFlashOpen
// ----------------------------------------------------------------------------
/// Enable the CS for FLASH. That chip select is named like this for its
/// common use, but can in fact be plugged to a RAM.
/// This function must be called before any other EBC driver function,
/// as the Flash Chip is the device imposing the frequency on the memory
/// bridge. This optimal frequency is set as a parameter and the corresponding
/// EBC settings are also specified.
/// The Chip Select used is not a parameter of the function as it is always
/// #HAL_EBC_FLASH (ie. CS0)
///
/// @param optimalFreq Optimal Frequency to use the chip plugged on the
/// #HAL_EBC_FLASH chip select.
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in flash
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* HAL_BOOT_FUNC hal_EbcFlashOpen(HAL_SYS_FREQ_T optimalFreq, CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc)
{
#ifdef CHIP_HAS_EBC_CS2_BUG
    // union representing the status bitfield in a 32 bits value
    // loadable in the register
    union
    {
        UINT32 reg;
        HAL_EBC_TIME_CFG_T bitfield;
    } timeCfgUnion;

    // union representing the status bitfield in a 32 bits value
    // loadable in the register
    union
    {
        UINT32 reg;
        HAL_EBC_MODE_CFG_T bitfield;
    } modeCfgUnion;
#endif

    VOID* retval = NULL;

    //ENTER CRITICAL
    UINT32 status = hal_SysEnterCriticalSection();

    // Wait if the lock is locked
    hal_EbcWaitReady();

    // Set EBC frequency
    // FIXME The membridge clock must be set-up before the system clock, and done
    // only once. The flash open should (must) be done before hal_Open for
    // the Admux. As long as it's not, hal_EbcCsSetup is called by hal_Open.
    // hal_EbcCsSetup(optimalFreq);

    // Write the pointer into the table
    //g_halEbcCsConfigArray[0]              = csConfig;

    boot_EbcCsSetupWithCallback(HAL_EBC_FLASH, csConfig, cbFunc);

    retval = (VOID*)hwp_cs0;

#ifdef CHIP_HAS_EBC_CS2_BUG
    timeCfgUnion.bitfield = csConfig->time;
    modeCfgUnion.bitfield = csConfig->mode;

    g_halCs0TimeReg  = timeCfgUnion.reg;
    g_halCs0ModeReg  = modeCfgUnion.reg;
#endif

    //EXIT CRITICAL
    hal_SysExitCriticalSection(status);

    return retval;
}
#endif // !CHIP_HAS_AP


// ============================================================================
// hal_EbcSramOpen
// ----------------------------------------------------------------------------
/// Enable a CS for SRAM. The chip selects for
/// FLASH have been set before by calling #hal_EbcFlashOpen.
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given
/// chip select, and must support the external maximal clock as it has been
/// set by the configuring of the CS0 (Flash) chipo select.
///
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in sram
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* HAL_BOOT_FUNC hal_EbcSramOpen(CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc)
{
    return boot_EbcSramOpen(csConfig, cbFunc);
}


// ============================================================================
// hal_EbcCsOpen
// ----------------------------------------------------------------------------
/// Enable a CS other than the one for FLASH. The chip selects for
/// FLASH have been set before by calling #hal_EbcFlashOpen.
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given
/// chip select, and must support the external maximal clock as it has been
/// set by the configuring of the CS0 (Flash) chip select.
///
/// @param cs Chip Select to Enable. (HAL_EBC_SRAM and CS2 or above)
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* HAL_BOOT_FUNC hal_EbcCsOpen(HAL_EBC_CS_T cs, CONST HAL_EBC_CS_CFG_T* csConfig)
{
    return boot_EbcCsOpen(cs, csConfig);
}


// ============================================================================
// hal_EbcCsClose
// ----------------------------------------------------------------------------
/// Disables a CS (except FLASH and SRAM).
/// The address space previously allocated for this chip select becomes
/// unavailable after a call to this function
///
/// @param cs The Chip Select to Disable
// ============================================================================
PUBLIC VOID hal_EbcCsClose(HAL_EBC_CS_T cs)
{
#ifdef USE_OPIDDR_PSRAM
#else
    UINT32 status = hal_SysEnterCriticalSection();
    hal_EbcWaitReady();
    hwp_memBridge->CS_Time_Write = 0;
    switch(cs)
    {
        case HAL_EBC_CS2:
            hwp_memBridge->CS_Config[2].CS_Mode = 0;
            //g_halEbcCsConfigArray[2]= NULL;
            break;
        case HAL_EBC_CS3:
            hwp_memBridge->CS_Config[3].CS_Mode = 0;
            //g_halEbcCsConfigArray[3]= NULL;
            break;
        case HAL_EBC_CS4:
            hwp_memBridge->CS_Config[4].CS_Mode = 0;
            //g_halEbcCsConfigArray[4]= NULL;
            break;
        default:
            break;
    }
    hal_SysExitCriticalSection(status);
#endif
}


// ============================================================================
// hal_EbcGetFrequency
// ----------------------------------------------------------------------------
/// Gets the EBC frequency
///
/// @return The EBC frequency
// ============================================================================
PUBLIC HAL_SYS_MEM_FREQ_T hal_EbcGetFrequency(VOID)
{
    return boot_BootSectorGetEbcFreq();
}


// ============================================================================
// hal_EbcGetCsAddress
// ----------------------------------------------------------------------------
/// Return the address of a given Chip Select.
///
/// @param cs Chip Select
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* hal_EbcGetCsAddress(HAL_EBC_CS_T cs)
{
    VOID* retval = NULL;

    switch(cs)
    {
#ifdef CHIP_HAS_AP
        case HAL_EBC_SRAM :
            retval = (VOID*)hwp_cs0;
            break;

#else // !CHIP_HAS_AP
        case HAL_EBC_FLASH :
            retval = (VOID*)hwp_cs0;
            break;

        case HAL_EBC_SRAM :
            retval = (VOID*)hwp_cs1;
            break;
#if !defined(CHIP_DIE_8955) && !defined(CHIP_DIE_8909)
        case HAL_EBC_CS2 :
            retval = (VOID*)hwp_cs2;
            break;

        case HAL_EBC_CS3 :
            retval = (VOID*)hwp_cs3;
            break;

        case HAL_EBC_CS4:
            retval = (VOID*)hwp_cs4;
            break;
#endif

#endif // !CHIP_HAS_AP
        default:
            break;
    }

    return retval;
}


// ============================================================================
//                            PROTECTED
// ============================================================================

// ============================================================================
// hal_EbcCsSetup
// ----------------------------------------------------------------------------
/// Configure the CS settings according to the system clock frequency
/// This function configures the CS time and mode settings.
///
/// @param sysFreq System frequency to which adjust
/// the CS settings
// ============================================================================
PROTECTED VOID hal_EbcCsSetup(HAL_SYS_MEM_FREQ_T ebcFreq)
{
    boot_EbcCsSetup(ebcFreq);
}



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
#include "string.h"
#include "sxr_tls.h"

#include "hal_overlay.h"
#include "hal_sys.h"
#include "hal_dma.h"

#include "halp_debug.h"
#include "halp_gdb_stub.h"
#include "halp_proxy_memory.h"

// =============================================================================
// MACROS
// =============================================================================
/// Maximum number of overlay instance.
#define HAL_OVERLAY_INSTANCE_MAX_PER_LOCATION_QTY        6


#define HAL_OVERLAY_ID_NONE                 0xFFFFFFFF

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// HAL_OVERLAY_MEMORY_LOCATION_ID_T
// -----------------------------------------------------------------------------
/// This private type defines Ids to refer to the memory location in the owner
/// array. Don't confuse it with #HAL_OVERLAY_MEMORY_LOCATION_T.
// =============================================================================
typedef enum
{
    HAL_OVERLAY_MEMORY_LOCATION_INT_SRAM_ID = 0,
    HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM_ID,

    HAL_OVERLAY_MEMORY_LOCATION_ID_QTY
} HAL_OVERLAY_MEMORY_LOCATION_ID_T;


// =============================================================================
// HAL_OVERLAY_DESC_T
// -----------------------------------------------------------------------------
/// Describes an overlay with the pointers in flash from where to get the data,
/// the pointers in memory where to copy the code.
// =============================================================================
typedef struct
{
    UINT32* flashStart;
    UINT32* flashEnd;
    /// ?
    UINT32* ramStart;
    /// ?
    UINT32* ramEnd;
    /// Some free room may be available at the end of the overlay,
    /// as the memory reserved is big enough to be able to fit
    /// the bigger overlay.
    UINT32* freeContentStart;
    UINT32* overlayEnd;
} HAL_OVERLAY_DESC_T;



// =============================================================================
// EXTERNAL VARIABLES
// -----------------------------------------------------------------------------
/// They come from the linker script.
// =============================================================================
// Internal Sram Overlay
extern UINT32 _sys_sram_overlay_start;
extern UINT32 _sys_sram_overlay_end;
extern UINT32 __load_start_sys_sram_overlay_0;
extern UINT32 __load_start_sys_sram_overlay_1;
extern UINT32 __load_start_sys_sram_overlay_2;
extern UINT32 __load_start_sys_sram_overlay_3;
extern UINT32 __load_start_sys_sram_overlay_4;
extern UINT32 __load_start_sys_sram_overlay_5;
extern UINT32 __load_stop_sys_sram_overlay_0;
extern UINT32 __load_stop_sys_sram_overlay_1;
extern UINT32 __load_stop_sys_sram_overlay_2;
extern UINT32 __load_stop_sys_sram_overlay_3;
extern UINT32 __load_stop_sys_sram_overlay_4;
extern UINT32 __load_stop_sys_sram_overlay_5;


#if (CHIP_HAS_PROXY_MEMORY == 1)
// Fast Cpu Memory overlay
extern UINT32 _fast_cpu_memory_overlay_start;
extern UINT32 _fast_cpu_memory_overlay_end;
extern UINT32 __load_start_fast_cpu_memory_overlay_0;
extern UINT32 __load_start_fast_cpu_memory_overlay_1;
extern UINT32 __load_start_fast_cpu_memory_overlay_2;
extern UINT32 __load_start_fast_cpu_memory_overlay_3;
extern UINT32 __load_start_fast_cpu_memory_overlay_4;
extern UINT32 __load_start_fast_cpu_memory_overlay_5;
extern UINT32 __load_stop_fast_cpu_memory_overlay_0;
extern UINT32 __load_stop_fast_cpu_memory_overlay_1;
extern UINT32 __load_stop_fast_cpu_memory_overlay_2;
extern UINT32 __load_stop_fast_cpu_memory_overlay_3;
extern UINT32 __load_stop_fast_cpu_memory_overlay_4;
extern UINT32 __load_stop_fast_cpu_memory_overlay_5;
#endif



// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

/// This array registers the descriptors of all the overlays.
PROTECTED CONST HAL_OVERLAY_DESC_T g_halOverlayDesc
[HAL_OVERLAY_MEMORY_LOCATION_ID_QTY][HAL_OVERLAY_INSTANCE_MAX_PER_LOCATION_QTY] =
{
    {
        // INT_SRAM
        {
            .flashStart         = &__load_start_sys_sram_overlay_0,
            .flashEnd           = &__load_stop_sys_sram_overlay_0,
            .ramStart           = &_sys_sram_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_sys_sram_overlay_end
        },
        {
            .flashStart         = &__load_start_sys_sram_overlay_1,
            .flashEnd           = &__load_stop_sys_sram_overlay_1,
            .ramStart           = &_sys_sram_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_sys_sram_overlay_end
        },
        {
            .flashStart         = &__load_start_sys_sram_overlay_2,
            .flashEnd           = &__load_stop_sys_sram_overlay_2,
            .ramStart           = &_sys_sram_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_sys_sram_overlay_end
        },
        {
            .flashStart         = &__load_start_sys_sram_overlay_3,
            .flashEnd           = &__load_stop_sys_sram_overlay_3,
            .ramStart           = &_sys_sram_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_sys_sram_overlay_end
        },
        {
            .flashStart         = &__load_start_sys_sram_overlay_4,
            .flashEnd           = &__load_stop_sys_sram_overlay_4,
            .ramStart           = &_sys_sram_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_sys_sram_overlay_end
        },
        {
            .flashStart         = &__load_start_sys_sram_overlay_5,
            .flashEnd           = &__load_stop_sys_sram_overlay_5,
            .ramStart           = &_sys_sram_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_sys_sram_overlay_end
        }
    },
#if (CHIP_HAS_PROXY_MEMORY == 1)
    {
        // FAST CPU memory
        {
            .flashStart         = &__load_start_fast_cpu_memory_overlay_0,
            .flashEnd           = &__load_stop_fast_cpu_memory_overlay_0,
            .ramStart           = &_fast_cpu_memory_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_fast_cpu_memory_overlay_end
        },
        {
            .flashStart         = &__load_start_fast_cpu_memory_overlay_1,
            .flashEnd           = &__load_stop_fast_cpu_memory_overlay_1,
            .ramStart           = &_fast_cpu_memory_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_fast_cpu_memory_overlay_end
        },
        {
            .flashStart         = &__load_start_fast_cpu_memory_overlay_2,
            .flashEnd           = &__load_stop_fast_cpu_memory_overlay_2,
            .ramStart           = &_fast_cpu_memory_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_fast_cpu_memory_overlay_end
        },
        {
            .flashStart         = &__load_start_fast_cpu_memory_overlay_3,
            .flashEnd           = &__load_stop_fast_cpu_memory_overlay_3,
            .ramStart           = &_fast_cpu_memory_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_fast_cpu_memory_overlay_end
        },
        {
            .flashStart         = &__load_start_fast_cpu_memory_overlay_4,
            .flashEnd           = &__load_stop_fast_cpu_memory_overlay_4,
            .ramStart           = &_fast_cpu_memory_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_fast_cpu_memory_overlay_end
        },
        {
            .flashStart         = &__load_start_fast_cpu_memory_overlay_5,
            .flashEnd           = &__load_stop_fast_cpu_memory_overlay_5,
            .ramStart           = &_fast_cpu_memory_overlay_start,
            .ramEnd             = NULL,
            .freeContentStart   = NULL,
            .overlayEnd         = &_fast_cpu_memory_overlay_end
        }
    },
#endif
};

/// Each memory location has only one owner.
PROTECTED HAL_OVERLAY_ID_T g_halOverlayOwners[HAL_OVERLAY_MEMORY_LOCATION_ID_QTY] =
{
    HAL_OVERLAY_ID_NONE, HAL_OVERLAY_ID_NONE
};

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_OverlayGetMemoryLocationId
// -----------------------------------------------------------------------------
/// Get the Id of the memory location used by an overlay id.
///
/// @param id Overlay Id
/// @return The memory location Id of this overlay id.
// =============================================================================
PRIVATE HAL_OVERLAY_MEMORY_LOCATION_ID_T hal_OverlayGetMemoryLocationId(HAL_OVERLAY_ID_T id)
{
    HAL_OVERLAY_MEMORY_LOCATION_ID_T memLocId = 0;
    if (HAL_OVERLAY_INT_SRAM_ID_0 <= id
            && HAL_OVERLAY_INT_SRAM_ID_QTY > id)
    {
        memLocId = HAL_OVERLAY_MEMORY_LOCATION_INT_SRAM_ID;
    }
    else if (HAL_OVERLAY_FAST_CPU_RAM_ID_0 <= id
             && HAL_OVERLAY_FAST_CPU_RAM_ID_QTY > id)
    {
#if (CHIP_HAS_PROXY_MEMORY == 1)
        memLocId = HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM_ID;
#else
        HAL_ASSERT(FALSE, "HAL Overlay Unknown overlay id %d", id);
#endif
    }
    else
    {
        HAL_ASSERT(FALSE, "HAL Overlay Unknown overlay id %d", id);
    }
    return memLocId;
}


// =============================================================================
// hal_OverlayGetDescriptorId
// -----------------------------------------------------------------------------
/// Get the Id of the memory location used by an overlay id.
///
/// @param id Overlay Id
/// @return The id to use to access information about that overlay in the
/// descriptor array.
// =============================================================================
PRIVATE HAL_OVERLAY_MEMORY_LOCATION_ID_T hal_OverlayGetDescriptorId(HAL_OVERLAY_ID_T id)
{
    UINT32 descId = 0;
    if (HAL_OVERLAY_INT_SRAM_ID_0 <= id
            && HAL_OVERLAY_INT_SRAM_ID_QTY > id)
    {
        descId = id;
    }
    else if (HAL_OVERLAY_FAST_CPU_RAM_ID_0 <= id
             && HAL_OVERLAY_FAST_CPU_RAM_ID_QTY > id)
    {
        // Remove the tag and offset by the number of overlays
        // per previously ordered memory locations.
#if (CHIP_HAS_PROXY_MEMORY == 1)
        descId = (id - HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM);
#else
        HAL_ASSERT(FALSE, "HAL Overlay Unknown overlay id %d", id);
#endif
    }
    else
    {
        descId = 0;
        HAL_ASSERT(FALSE, "HAL Overlay Unknown overlay id %d", id);
    }
    return descId;
}


// =============================================================================
// hal_OverlayLoad
// -----------------------------------------------------------------------------
/// Load the \c id overlay in memory. It is loaded at the memory specified by
/// the id value (Internal SRAM, etc ...)
///
/// @param id Id of the overlay of \c location memory to load.
/// @return #HAL_ERR_NO when everything is fine.
///         #HAL_ERR_RESOURCE_BUSY when this memory location is already
/// being used by another overlay.
// =============================================================================
PUBLIC HAL_ERR_T hal_OverlayLoad(HAL_OVERLAY_ID_T id)
{
    HAL_ERR_T result = HAL_ERR_NO;
    HAL_OVERLAY_MEMORY_LOCATION_ID_T memId = hal_OverlayGetMemoryLocationId(id);
    UINT32  descId = hal_OverlayGetDescriptorId(id);
    HAL_DMA_CFG_T transferCfg;

    // Protect access to global variables.
    UINT32  status = hal_SysEnterCriticalSection();
    if (g_halOverlayOwners[memId] != HAL_OVERLAY_ID_NONE)
    {
        // FIXME What happens if own code
        // is already there ?
        result = HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Space free take it.
        g_halOverlayOwners[memId] = id;
        result = HAL_ERR_NO;
    }

    // Unprotect.
    hal_SysExitCriticalSection(status);

    if (result == HAL_ERR_NO)
    {
        // We got the place, loading ...
        HAL_TRACE(HAL_DBG_TRC, 0, "ram start %08x", g_halOverlayDesc[memId][descId].ramStart);
        HAL_TRACE(HAL_DBG_TRC, 0, "flash start  %08x", g_halOverlayDesc[memId][descId].flashStart);
        HAL_TRACE(HAL_DBG_TRC, 0, "flash end %08x", g_halOverlayDesc[memId][descId].flashEnd);
        HAL_TRACE(HAL_DBG_TRC, 0, "size %08x", (UINT32)g_halOverlayDesc[memId][descId].flashEnd -
                  (UINT32)g_halOverlayDesc[memId][descId].flashStart);

        HAL_ASSERT((UINT32)g_halOverlayDesc[memId][descId].flashEnd >
                   (UINT32)g_halOverlayDesc[memId][descId].flashStart,
                   "HAL Overlay: Attempt to load an empty overlay, id:%d", id);
        HAL_ASSERT((UINT32)g_halOverlayDesc[memId][descId].flashEnd -
                   (UINT32)g_halOverlayDesc[memId][descId].flashStart,
                   "HAL Overlay: Overlay too big to be loaded by DMA: %d",
                   (UINT32)g_halOverlayDesc[memId][descId].flashEnd -
                   (UINT32)g_halOverlayDesc[memId][descId].flashStart);

        // Use a DMA transfer as it will be faster than the CPU.
        transferCfg.srcAddr         = (UINT8*)g_halOverlayDesc[memId][descId].flashStart;
        transferCfg.dstAddr         = (UINT8*)g_halOverlayDesc[memId][descId].ramStart;
        transferCfg.alterDstAddr    = NULL;
        transferCfg.pattern         = 0;
        transferCfg.transferSize    = (UINT32)g_halOverlayDesc[memId][descId].flashEnd -
                                      (UINT32)g_halOverlayDesc[memId][descId].flashStart;
        transferCfg.mode            = HAL_DMA_MODE_NORMAL;
        transferCfg.userHandler     = NULL;

#if (CHIP_HAS_PROXY_MEMORY == 1)
        if (memId == HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM_ID)
        {
            // Allow loading access to the proxy memory.
            hal_PMemEnableExternalAccess(TRUE);
        }
#endif
        // Start the DMA
        while (HAL_ERR_NO != hal_DmaStart(&transferCfg))
        {
            sxr_Sleep(10);
        }

        // Wait for the DMA to finish
        // (hal_OverlayLoad is a blocking function).
        while (!hal_DmaDone())
        {
            sxr_Sleep(10);
        }

        // TODO Add a fallback in case of too busy DMA
        //memcpy(g_halOverlayDesc[memId][descId].ramStart,
        //        g_halOverlayDesc[memId][descId].flashStart,
        //        (UINT32)g_halOverlayDesc[memId][descId].flashEnd - (UINT32)g_halOverlayDesc[memId][descId].flashStart);

#if (CHIP_HAS_PROXY_MEMORY == 1)
        if (memId == HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM_ID)
        {
            // Disable access to the proxy memory.
            hal_PMemEnableExternalAccess(FALSE);
        }
#endif

        // Fluch Caches
        hal_GdbFlushCache();
    }

    return result;
}


// =============================================================================
// hal_OverlayUnload
// -----------------------------------------------------------------------------
/// Unload the \c id overlay from memory.
///
/// @param id Id of the overlay of \c location memory to load.
/// @return #HAL_ERR_NO when everything is fine.
///         #HAL_ERR_RESOURCE_BUSY when this memory location is already
/// being used by another overlay.
// =============================================================================
PUBLIC HAL_ERR_T hal_OverlayUnload(HAL_OVERLAY_ID_T id)
{
    HAL_ERR_T result = HAL_ERR_NO;
    HAL_OVERLAY_MEMORY_LOCATION_ID_T memId = hal_OverlayGetMemoryLocationId(id);
    UINT32  descId = hal_OverlayGetDescriptorId(id);
    HAL_DMA_CFG_T transferCfg;

    // Protect access to global variables.
    UINT32  status = hal_SysEnterCriticalSection();
    if (g_halOverlayOwners[memId] != id)
    {
        // FIXME What happens if owns code
        // is already there ?
        result = HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // We own the place.
        // We will clean it before releasing
        // ownership.
        result = HAL_ERR_NO;
    }

    // Unprotect.
    hal_SysExitCriticalSection(status);

    if (result == HAL_ERR_NO)
    {
        // We got the place, loading ...

        HAL_ASSERT((UINT32)g_halOverlayDesc[memId][descId].flashEnd > (UINT32)g_halOverlayDesc[memId][descId].flashStart,
                   "HAL Overlay: Attempt to load an empty overlay, id:%d", id);
        HAL_ASSERT((UINT32)g_halOverlayDesc[memId][descId].flashEnd - (UINT32)g_halOverlayDesc[memId][descId].flashStart,
                   "HAL Overlay: Overlay too big to be loaded by DMA: %d",
                   (UINT32)g_halOverlayDesc[memId][descId].flashEnd - (UINT32)g_halOverlayDesc[memId][descId].flashStart);

        // Use a DMA transfer as it will be faster than the CPU.
        transferCfg.srcAddr         = NULL;
        transferCfg.dstAddr         = (UINT8*)g_halOverlayDesc[memId][descId].ramStart;
        transferCfg.alterDstAddr    = NULL;
        transferCfg.pattern         = 0;
        transferCfg.transferSize    = (UINT32)g_halOverlayDesc[memId][descId].flashEnd - (UINT32)g_halOverlayDesc[memId][descId].flashStart;
        transferCfg.mode            = HAL_DMA_MODE_PATTERN;
        transferCfg.userHandler     = NULL;

#if (CHIP_HAS_PROXY_MEMORY == 1)
        if (memId == HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM_ID)
        {
            // Allow loading access to the proxy memory.
            hal_PMemEnableExternalAccess(TRUE);
        }
#endif
        // Start the DMA
        while (HAL_ERR_NO != hal_DmaStart(&transferCfg))
        {
            sxr_Sleep(10);
        }

        // Wait for the DMA to finish
        // (hal_OverlayLoad is a blocking function).
        while (!hal_DmaDone())
        {
            sxr_Sleep(10);
        }

        // TODO Add a fallback in case of too busy DMA
        //memset(g_halOverlayDesc[memId][descId].ramStart,
        //        0,
        //        g_halOverlayDesc[memId][descId].flashEnd - g_halOverlayDesc[memId][descId].flashStart);

        // Fluch Caches
        hal_GdbFlushCache();

#if (CHIP_HAS_PROXY_MEMORY == 1)
        if (memId == HAL_OVERLAY_MEMORY_LOCATION_FAST_CPU_RAM_ID)
        {
            // Disable access to the proxy memory.
            hal_PMemEnableExternalAccess(FALSE);
        }
#endif

        // Release memory location.
        g_halOverlayOwners[memId] = HAL_OVERLAY_ID_NONE;
    }

    return result;
}



// ========================================================================
// hal_OverlayIsLoaded
// -----------------------------------------------------------------------------
/// Check if the \c id overlay is loaded into memory.
///
/// @param id Id of the overlay we check if it is loaded.
/// @return \c TRUE if \c Id is loaded,
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_OverlayIsLoaded(HAL_OVERLAY_ID_T id)
{
    HAL_OVERLAY_MEMORY_LOCATION_ID_T memId = hal_OverlayGetMemoryLocationId(id);
    return (g_halOverlayOwners[memId] == id);
}




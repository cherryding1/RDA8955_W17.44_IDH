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
#include "dma.h"

#include "halp_sys.h"
#include "halp_debug.h"
#include "hal_dma.h"
#include "hal_sys.h"
#include "hal_mem_map.h"
#include "hal_clk.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
// HAL_DMA_PREEMPT_THSLD
// -----------------------------------------------------------------------------
/// Threshold above which a low priority dma transfert can be preempted by
/// a High Priority One.
// =============================================================================
#define HAL_DMA_PREEMPT_THSLD           1024
#ifdef CHIP_DIE_8909
#define hwp_dma hwp_sysDma
#endif

// =============================================================================
//  TYPES
// =============================================================================
// =============================================================================
//
// -----------------------------------------------------------------------------
// Private type to identify the type of transfer in progress
// =============================================================================
typedef enum
{
    HAL_DMA_NONE,
    HAL_DMA_STD,
    HAL_DMA_GEA,
    HAL_DMA_FCS,

    HAL_DMA_QTY,
} HAL_DMA_USERS_T;



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// g_halDmaUserId
// -----------------------------------------------------------------------------
/// Identifier for a tranfer. Might be useful somehow
// =============================================================================
HAL_DMA_USERS_T g_halDmaUserId
    = HAL_DMA_NONE;

// =============================================================================
// g_halDmaMode
// -----------------------------------------------------------------------------
/// Mode of the standard transfer
// =============================================================================
HAL_DMA_MODE_T  g_halDmaMode
    = HAL_DMA_MODE_NORMAL;

// =============================================================================
// g_halDmaRegistry
// -----------------------------------------------------------------------------
/// User handler called at the end of a transfer if any interrupt arise.
// =============================================================================
HAL_DMA_IRQ_HANDLER_T g_halDmaRegistry
    = NULL;

static HAL_CLK_T *gDmaClk = NULL;


// =============================================================================
//  FUNCTIONS
// =============================================================================



// =============================================================================
// hal_DmaDone
// -----------------------------------------------------------------------------
/// Test if a DMA transfer is still running.
/// @return \c FALSE if the transfer is still in progress, \c TRUE if it has
/// terminated.
// =============================================================================
PUBLIC BOOL HAL_FUNC_RAM hal_DmaDone(VOID)
{
    UINT32 status;
    // Check the status of DMA
    status = hwp_dma->status & DMA_INT_DONE_STATUS;

    // The interrupt handler may have cleared the done
    // bit beforehand.
    if (status || (g_halDmaUserId == HAL_DMA_NONE))
    {
        // Finished
        g_halDmaUserId = HAL_DMA_NONE;
        return TRUE;
    }

    // Not finished
    return FALSE;
}

// =============================================================================
// hal_DmaStopStd
// -----------------------------------------------------------------------------
/// Stop a standard transfer in progress. If the remaining size to transfer is
/// less than the defined threshold, that function waits for the end of the
/// transfert. This behaviour is expected by both the FCS and GEA transfert,
/// as those transfers are blocking functions.
///
/// It has previously been tested that the transfer is currently a standard one.
/// @param xferState Pointer where the state of the transfer will be stored
/// for a later resume.
// =============================================================================
PRIVATE VOID hal_DmaStopStd(HAL_DMA_CFG_T* xferState)
{
    UINT32 status;
    UINT32 remainingSize = 0;

    status = hal_SysEnterCriticalSection();
    remainingSize = hwp_dma->xfer_size;

    if (remainingSize > HAL_DMA_PREEMPT_THSLD)
    {
        // Preempt - stop
        hwp_dma->control |= DMA_STOP_TRANSFER;

        // Wait for the end
        while (!hal_DmaDone());

        // Really needed ?
        if (hwp_dma->xfer_size == 0)
        {
            // the transfer has finished
            // If an IT was programmed, it will be triggered
            // once we go out of critical section.
            hal_SysExitCriticalSection(status);
            // Remaining size is used by the calling function to
            // know if there is a transfer to resume.
            xferState->transferSize = 0;
        }
        else
        {
            // Stopping the transfer has triggered the IT
            // We must clear IT as this IRQ is meant to
            // happen once the stopped transfer is fully
            // completed (ie NOT now)
            // --> Done automatically by clearing the command
            // register
            xferState->srcAddr =(CONST UINT8*) hwp_dma->src_addr;
            xferState->dstAddr = (UINT8*) hwp_dma->dst_addr;
            xferState->alterDstAddr = (UINT8*) hwp_dma->sd_dst_addr;
            xferState->pattern = hwp_dma->pattern;
            xferState->transferSize =(UINT16) hwp_dma->xfer_size;
            xferState->mode = g_halDmaMode;

            xferState->userHandler = g_halDmaRegistry;

            g_halDmaRegistry = NULL;

            hwp_dma->control = 0; //clear it and prog of the stopped transfer

            hal_SysExitCriticalSection(status);
        }
    }
    else
    {
        // Too few bytes to transfer, we wait
        hal_SysExitCriticalSection(status);
        // Remaining size is used by the calling function to
        // know if there is a transfer to resume.
        xferState->transferSize = 0;
        // Fill other fields for coherency ?
        xferState->srcAddr = (CONST UINT8*)0;
        xferState->dstAddr = (UINT8*)0;
        xferState->alterDstAddr = (UINT8*)0;
        xferState->pattern = 0;
        xferState->mode = 0;
        xferState->userHandler = NULL;

        while (!hal_DmaDone());
    }

    g_halDmaUserId = HAL_DMA_NONE;
}

// =============================================================================
// hal_DmaGeaStart
// -----------------------------------------------------------------------------
/// Configure the registers and launch the transfer
/// with GEA. This is a blocking function, that returns only after the
/// transfer is finished.
///
/// @param pCfg The transfer configuration. Refer to the type descriptions for
/// more details.
/// @return DMA transfer error status: #HAL_ERR_NO.
// =============================================================================
PUBLIC HAL_ERR_T hal_DmaGeaStart(CONST HAL_DMA_GEA_CFG_T* pCfg)
{
    HAL_DMA_CFG_T preemptedXfer =
    {
        .srcAddr = NULL,
        .dstAddr = NULL,
        .alterDstAddr = NULL,
        .pattern = 0,
        .transferSize = 0,
        .mode = 0,
        .userHandler = NULL
    };

    switch (g_halDmaUserId)
    {
        case HAL_DMA_GEA:
        case HAL_DMA_FCS:
            return HAL_ERR_RESOURCE_BUSY;
            break; // ^^
        case HAL_DMA_STD:
            // Stop or wait for the end of a normal transfer
            hal_DmaStopStd(&preemptedXfer);
            break;
        case HAL_DMA_NONE:
        default:
            // carry on with a normal execution
            break;
    }
    // Check DMA channel availability and get if necessary !
    if (!((hwp_dma->get_channel) & DMA_GET_CHANNEL))
    {
        // That case shouldn't happen, as we have checked everything
        // in the previous switch/case
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Take resource to do that fast (And to avoid to go into deep
        // sleep while a transfer is in progress ...)

        if (!hal_ClkIsEnabled(gDmaClk)) {
            hal_ClkEnable(gDmaClk);
            hal_ClkSetRate(gDmaClk, HAL_CLK_RATE_104M);
        }

        g_halDmaUserId = HAL_DMA_GEA;
        // Set the user handler
        g_halDmaRegistry = pCfg->userHandler;
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
	if(((UINT32)pCfg->srcAddr & 0x20000000) == 0)
	{
		hal_DcacheFlushAddrRange((UINT32)pCfg->srcAddr, (UINT32)pCfg->srcAddr + pCfg->transferSize, FALSE);
	}
#endif
        // configure the registers
        hwp_dma->src_addr = (UINT32)pCfg->srcAddr;
        hwp_dma->dst_addr = (UINT32)pCfg->dstAddr;
        hwp_dma->xfer_size = DMA_TRANSFER_SIZE(pCfg->transferSize);
        hwp_dma->gea_kc_low = pCfg->kcLow;
        hwp_dma->gea_kc_high = pCfg->kcHigh;
        hwp_dma->gea_messkey = pCfg->messKey;
        // configure the CTRL register and launch the transfer
        hwp_dma->control =
            DMA_ENABLE
            | (pCfg->userHandler==NULL?0:DMA_INT_DONE_MASK)
            | DMA_INT_DONE_CLEAR
            | DMA_GEA_ENABLE
            | ((pCfg->geaAlgo ==0)?DMA_GEA_ALGORITHM_GEA1:DMA_GEA_ALGORITHM_GEA2)
            | ((pCfg->direction & 0x1)<<DMA_GEA_DIRECTION_SHIFT);


        // Wait for the end of the transfer
        while (!hal_DmaDone());

        // if any, reprogram the preempted transfer
        if (preemptedXfer.transferSize != 0)
        {
            hal_DmaStart(&preemptedXfer);
            // Resource will be released by the DMA interrupt.
        }
        else
        {
            // Release manually the resource.

            hal_ClkDisable(gDmaClk);
        }

        return HAL_ERR_NO;
    }
}


// =============================================================================
// hal_DmaStart
// -----------------------------------------------------------------------------
/// Configure the registers and launch the DMA transfer.
///
/// @param pCfg The transfer configuration. Refer to the type descriptions for
/// more details.
/// @return DMA transfer error status: #HAL_ERR_NO, #HAL_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC HAL_ERR_T hal_DmaStart(CONST HAL_DMA_CFG_T *pCfg)
{
    UINT32 controlReg;

    // On Greenstone and Gallite (and Jade by the way), we should not poll
    // get_channel when the DMA is running.
    if ((hwp_dma->status) & DMA_ENABLE)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    if (!((hwp_dma->get_channel) & DMA_GET_CHANNEL))
    {
        return HAL_ERR_RESOURCE_BUSY;
    }

    // Take resource to do that fast (And to avoid to go into deep
    // sleep while a transfer is in progress ...)

    if (!hal_ClkIsEnabled(gDmaClk)) {
        hal_ClkEnable(gDmaClk);
        hal_ClkSetRate(gDmaClk, HAL_CLK_RATE_104M);
    }
    // Resource is released in the interrupt.

    g_halDmaUserId = HAL_DMA_STD;
    g_halDmaRegistry = pCfg->userHandler;
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
	if(((UINT32)pCfg->srcAddr & 0x20000000) == 0)
	{
		hal_DcacheFlushAddrRange((UINT32)pCfg->srcAddr, (UINT32)pCfg->srcAddr + pCfg->transferSize, FALSE);
	}
#endif
    hwp_dma->src_addr = (UINT32)pCfg->srcAddr;
    hwp_dma->dst_addr = (UINT32)pCfg->dstAddr;
    hwp_dma->sd_dst_addr = (UINT32)pCfg->alterDstAddr;
    hwp_dma->pattern  = pCfg->pattern;
    hwp_dma->xfer_size = DMA_TRANSFER_SIZE(pCfg->transferSize);
    // Configure the CTRL register and launch the transfer.
    switch (pCfg->mode)
    {
        case HAL_DMA_MODE_PATTERN:
            controlReg = DMA_USE_PATTERN|DMA_DST_ADDR_MGT_NORMAL_DMA;
            break;

        case HAL_DMA_MODE_PATTERN_CONST_ADDR:
            controlReg = DMA_USE_PATTERN|DMA_DST_ADDR_MGT_CONST_ADDR;
            break;

        default:
            // No pattern
            controlReg = DMA_DST_ADDR_MGT(pCfg->mode);
            break;
    }

    // Complete with the other fields.
    hwp_dma->control = controlReg
                       | DMA_ENABLE
                       | DMA_INT_DONE_MASK
                       | DMA_INT_DONE_CLEAR;

    {
        // Flush the CPU write buffer.
        UINT32 unused __attribute__((unused)) = hwp_dma->control;
    }

    return HAL_ERR_NO;

}




// =============================================================================
// hal_DmaFcsStart
// -----------------------------------------------------------------------------
/// Perform a FCS checksum on the data specified by the transfer configuration
/// structure. This is a blocking function, that returns only after the
/// transfer is finished.
///
/// @param pCfg The transfer configuration.
/// @return DMA transfer error status: #HAL_ERR_NO.
// =============================================================================
PUBLIC HAL_ERR_T hal_DmaFcsStart(CONST HAL_DMA_FCS_CFG_T * pCfg)
{
    HAL_DMA_CFG_T preemptedXfer  =
    {
        .srcAddr = NULL,
        .dstAddr = NULL,
        .alterDstAddr = NULL,
        .pattern = 0,
        .transferSize = 0,
        .mode = 0,
        .userHandler = NULL
    };

    switch (g_halDmaUserId)
    {
        case HAL_DMA_GEA:
        case HAL_DMA_FCS:
            return HAL_ERR_RESOURCE_BUSY;
            break; // ^^
        case HAL_DMA_STD:
            // Stop or wait for the end of a normal transfer
            hal_DmaStopStd(&preemptedXfer);
            break;
        case HAL_DMA_NONE:
        default:
            // carry on with a normal execution
            break;
    }
    // Check DMA channel availability and get if necessary !
    if (!((hwp_dma->get_channel) & DMA_GET_CHANNEL))
    {
        // That case shouldn't happen, as we have checked everything
        // in the previous switch/case
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Take resource to do that fast (And to avoid to go into deep
        // sleep while a transfer is in progress ...)

        if (!hal_ClkIsEnabled(gDmaClk)) {
            hal_ClkEnable(gDmaClk);
            hal_ClkSetRate(gDmaClk, HAL_CLK_RATE_104M);
        }

        g_halDmaUserId = HAL_DMA_FCS;
        // Set the user handler
        g_halDmaRegistry = pCfg->userHandler;
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
	if(((UINT32)pCfg->srcAddr & 0x20000000) == 0)
	{
		hal_DcacheFlushAddrRange((UINT32)pCfg->srcAddr, (UINT32)pCfg->srcAddr + pCfg->transferSize, FALSE);
	}
#endif
        // configure the registers
        hwp_dma->src_addr = (UINT32)pCfg->srcAddr;
        hwp_dma->dst_addr = 0xFFFFFFFF;
        hwp_dma->xfer_size = DMA_TRANSFER_SIZE(pCfg->transferSize);
        // configure the CTRL register and launch the transfer
        hwp_dma->control =
            DMA_ENABLE
            | (pCfg->userHandler==NULL?0:DMA_INT_DONE_MASK)
            | DMA_INT_DONE_CLEAR
            | DMA_FCS_ENABLE;


        // Wait for the end of the transfer
        while (!hal_DmaDone());

        // if any, reprogram the preempted transfer
        if (preemptedXfer.transferSize != 0)
        {
            hal_DmaStart(&preemptedXfer);
            // Resource will be released by the DMA interrupt.
        }
        else
        {
            // Release manually the resource.

            hal_ClkDisable(gDmaClk);
        }


        return HAL_ERR_NO;
    }
}






// =============================================================================
// hal_DmaReadFcs
// -----------------------------------------------------------------------------
/// After a FCS has been started and finished (checked by #hal_DmaDone),
/// this function recovers the FCSdata.
///
/// @param fcs Pointer where the FCS data will be stored
// =============================================================================
PUBLIC VOID hal_DmaReadFcs(UINT8 *fcs)
{
    UINT32 data;
    data = hwp_dma->fcs;
    fcs[0] = (data) & 0xFF;
    fcs[1] = (data>>8) & 0xFF;
    fcs[2] = (data>>16) & 0xFF;
}

// =============================================================================
// hal_DmaCheckFcs
// -----------------------------------------------------------------------------
/// After a FCS has been started and finished (checked by #hal_DmaDone),
/// checks the validity of the FCS
/// @return \c TRUE if the FCS is correct, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_DmaCheckFcs(VOID)
{
    if (hwp_dma->fcs & DMA_FCS_CORRECT)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



// =============================================================================
// hal_DmaIrqHandler
// -----------------------------------------------------------------------------
/// DMA Module interruption handler.
// =============================================================================
PROTECTED VOID hal_DmaIrqHandler(UINT8 dmaIrqId)
{
    UINT16 status;

    // Read the status of DMA module
    status = hwp_dma->status & DMA_INT_DONE_STATUS;

    // Clear the transfer done interruption
    hwp_dma->control = DMA_INT_DONE_CLEAR;

    // The IRQ happened because the transfer is finished
    g_halDmaUserId = HAL_DMA_NONE;

    if (g_halDmaRegistry)
    {
        g_halDmaRegistry();
    }

    // Release the resource.

    hal_ClkDisable(gDmaClk);
}

HAL_MODULE_CLK_INIT(DMA)
{
    gDmaClk = hal_ClkGet(FOURCC_DMA);
    HAL_ASSERT((gDmaClk != NULL), "Clk DMA not found!");
}


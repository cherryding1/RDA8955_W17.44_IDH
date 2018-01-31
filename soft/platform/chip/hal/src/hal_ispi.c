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

#if (CHIP_ISPI_CS_QTY != 0)

#include "global_macros.h"
#include "spi.h"
#include "sys_ifc.h"
#include "sys_irq.h"
#include "cfg_regs.h"

#include "boot_ispi.h"
#include "hal_ispi.h"
#include "halp_ispi.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "halp_sys.h"
#include "hal_mem_map.h"

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================

#ifdef CHIP_HAS_AP
#define hwp_ispi hwp_spi2
#else
#define hwp_ispi hwp_spi3
#endif


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// BOOT_ISPI_PROP_T
// -----------------------------------------------------------------------------
/// This type is used to remember of all the properties of each ISPI present in
/// the system, and its driver status.
// =============================================================================
typedef struct
{
    HAL_ISPI_CS_T              activatedCs;
    UINT32                     spiFreq;
} HAL_ISPI_PROP_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Global variable holding the state of each ISPI present in the system,
/// and holding the configuration of every one of its (opened) CS.
PRIVATE HAL_ISPI_PROP_T HAL_DATA_INTERNAL g_halIspiProperties =
{
    .activatedCs = HAL_ISPI_CS_ALL,
    .spiFreq = 13000000
};



// =============================================================================
//  FUNCTIONS
// ============================================================================

// =============================================================================
// hal_IspiOpen
// -----------------------------------------------------------------------------
/// Open the ISPI driver.
///
/// @param spiConfigPtr Pointer to the ISPI configuration to use. If \c NULL,
/// a default, romed configuration is used.
// =============================================================================
PUBLIC VOID hal_IspiOpen(CONST HAL_ISPI_CFG_T* spiConfigPtr)
{
    UINT32 status;

    if (NULL == spiConfigPtr)
    {
        spiConfigPtr = (CONST HAL_ISPI_CFG_T*) boot_IspiGetRomedCfg();
    }

    boot_IspiOpen((CONST BOOT_ISPI_CFG_T*) spiConfigPtr);
    g_halIspiProperties.activatedCs = HAL_ISPI_CS_ALL;
    g_halIspiProperties.spiFreq = spiConfigPtr->spiFreq;

    // Record divider update handler.
    // We use the (standard) SPI id to update the divider, so
    // we need to ask for the same frequency actually requested
    // by the SPI if the handler has been registered by the SPI
    // driver before the ISPI is opened by the call to #hal_IspiOpen().
    // (And we don't want the SPI to loose it's clock-required
    // system frequency, nor changing it while we open.)
    status = hal_SysEnterCriticalSection();
    // Set proper divider.
    hal_IspiUpdateDivider(hal_SysGetFreq());
    hal_SysExitCriticalSection(status);
}




// =============================================================================
// hal_IspiClose
// -----------------------------------------------------------------------------
/// Close the ISPI.
// =============================================================================
PUBLIC VOID hal_IspiClose(VOID)
{
    boot_IspiClose();
    g_halIspiProperties.activatedCs = HAL_ISPI_CS_ALL;
    g_halIspiProperties.spiFreq = 13000000;

}


// =============================================================================
// hal_IspiFlushFifos
// -----------------------------------------------------------------------------
/// Flush both ISPI Fifos.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. It must be the same as the activated Chip Select, unless the ISPI
/// is activated for all the Chip Select (#HAL_ISPI_CS_ALL)
/// @return #HAL_ERR_NO if the operation could be properly conducted, or
/// #HAL_ERR_RESOURCE_BUSY is another Chip Select is activated.
// =============================================================================
PUBLIC HAL_ERR_T hal_IspiFlushFifos(HAL_ISPI_CS_T csId)
{
    if ((g_halIspiProperties.activatedCs == HAL_ISPI_CS_ALL)
            || (g_halIspiProperties.activatedCs == csId))
    {
        boot_IspiFlushFifos();
        return HAL_ERR_NO;
    }
    else
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
}


// =============================================================================
// hal_IspiCsActivate
// -----------------------------------------------------------------------------
/// Activate a Chip Select. Activating a Chip Select means than all operation
/// that could interfere with the operation on that activated Chip Select are
/// forbidden. Namely, all read operation on other Chip Selects are forbidden,
/// as well as other activation.
///
/// @param csId Identifier of the Chip Select to activate.
/// @return \c TRUE if the Chip Select could be activated, \c FALSE if another
/// Chip Select is already activated.
// =============================================================================
PUBLIC BOOL hal_IspiCsActivate(HAL_ISPI_CS_T csId)
{
    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;

    // The same CS ID can be activated for once only,
    // to ensure mutually exclusive access even on the same CS.
#if 0
    if (g_halIspiProperties.activatedCs != HAL_ISPI_CS_ALL
            && g_halIspiProperties.activatedCs != csId)
#else
    if (g_halIspiProperties.activatedCs != HAL_ISPI_CS_ALL)
#endif
    {
        // Exit critical section.
        hwp_sysIrq->SC = status;
        return FALSE;
    }
    else
    {
        g_halIspiProperties.activatedCs = csId;
        // Exit critical section.
        hwp_sysIrq->SC = status;
        return TRUE;
    }

}


// =============================================================================
// hal_IspiCsDeactivate
// -----------------------------------------------------------------------------
/// Deactivate a Chip Select previously reserved by #hal_IspiCsActivate().
///
/// @param csId Identifier of the Chip Select to activate.
/// @return \c TRUE if the Chip Select could be deactivated, \c FALSE if another
/// Chip Select is already activated.
/// =============================================================================
PUBLIC BOOL hal_IspiCsDeactivate(HAL_ISPI_CS_T csId)
{
    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;

    if (g_halIspiProperties.activatedCs != HAL_ISPI_CS_ALL
            && g_halIspiProperties.activatedCs != csId)
    {
        // Exit critical section.
        hwp_sysIrq->SC = status;
        return FALSE;
    }
    else
    {
        g_halIspiProperties.activatedCs = HAL_ISPI_CS_ALL;
        // Exit critical section.
        hwp_sysIrq->SC = status;
        return TRUE;
    }

}




// =============================================================================
// hal_IspiForcePin
// -----------------------------------------------------------------------------
/// Force an ISPI pin to a certain state or release the force mode
/// and put the pin back to normal ISPI mode.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. It must be the same as the activated Chip Select, unless the ISPI
/// is activated for all the Chip Select (#HAL_ISPI_CS_ALL)
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
/// @return #HAL_ERR_NO if the operation could be properly conducted, or
/// #HAL_ERR_RESOURCE_BUSY is another Chip Select is activated.
// =============================================================================
PUBLIC HAL_ERR_T hal_IspiForcePin(HAL_ISPI_CS_T csId, HAL_ISPI_PIN_T pin,
                                  HAL_ISPI_PIN_STATE_T state)
{
    if ((g_halIspiProperties.activatedCs == HAL_ISPI_CS_ALL)
            || (g_halIspiProperties.activatedCs == csId))
    {
        boot_IspiForcePin(pin, state);
        return HAL_ERR_NO;
    }
    else
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
}


// =============================================================================
// hal_IspiSendData
// -----------------------------------------------------------------------------
/// Send one data.
/// This functions sends one data frame.
/// The number returned is the number of data frames actually sent.
///
/// @param csId The CS to use to send the data. This cs must be activated before
/// sending data, unless the data to send are not requiring a read operation
/// (TX only), in which case the sending is operated if room is available in
/// the Fifo.
/// @param data Frame of data to send.
/// @param read \c TRUE if the response of the device to this sent data is
/// expected to be read later and thus will be put in the read Fifo.
/// @return 1 if the data was sent, 0 otherwise.
// =============================================================================
PUBLIC UINT32 HAL_BOOT_FUNC_INTERNAL hal_IspiSendData(HAL_ISPI_CS_T csId, UINT32 data, BOOL read)
{
    UINT32 retVal = 0;

    // Check the possibility to write on the CS.
    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;

    // Write-only operation can be filled in the Tx Fifo freely,
    // as they won't pollute the Rx Fifo.
    if (g_halIspiProperties.activatedCs == HAL_ISPI_CS_ALL
            || g_halIspiProperties.activatedCs == csId
            || !(read))
    {
        retVal = boot_IspiSendData(csId, data, read);
    }
    else
    {
        retVal = 0;
    }

    // Exit critical section.
    hwp_sysIrq->SC = status;

    return retVal;


}


// =============================================================================
// hal_IspiSendDataBuffer
// -----------------------------------------------------------------------------
/// Send a bunch of data.
/// This functions sends \c length data (Number of 32 bits words) starting from
/// the address \c start_address. These data have previously been prepared to
/// be sent by using the #BOOT_ISPI_CS() and #BOOT_ISPI_REQ_READ macros.
/// The number returned is the number of data actually sent. (Number of 32 bits
/// items.)
/// In DMA mode, this function returns 0 when no DMA channel is available, it
/// returns length otherwise.
///
/// FIXME: Doc that needs the CS to be activated before use.
/// ATTENTION: Not safe in a concurrent environment
/// Idea to get it working are:
/// - use a global variable whose value is changed on polling or by a IRQ handler
/// -
/// ...
/// FIXME Implement DMA mode.
/// FIXME DON'T USE THAT FUNCTION !
/// @param csId Identifier of the ISPI Chip Select on which to send data.
/// @param startAddress Pointer on the buffer to send
/// @param length number of bytes to send (Up to 4 kB).
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of data (32 bits words) sent.
// =============================================================================
PUBLIC UINT32 hal_IspiSendDataBuffer(HAL_ISPI_CS_T csId, CONST UINT32* startAddress, UINT32 length)
{
    if (g_halIspiProperties.activatedCs == HAL_ISPI_CS_ALL
            || g_halIspiProperties.activatedCs == csId)
    {
        return boot_IspiSendDataBuffer(startAddress, length, BOOT_ISPI_DIRECT_POLLING);
    }
    else
    {
        return 0;
    }
}



// =============================================================================
// hal_IspiTxFifoAvail
// -----------------------------------------------------------------------------
/// Get available data spaces in the Spi Tx FIFO.
/// This function returns the available space in the Tx FIFO, as a number
/// of 32 bits data that can be filled into it.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return The size of the available space in the Tx FIFO. (In Fifo elements.)
// =============================================================================
PUBLIC UINT8 HAL_BOOT_FUNC_INTERNAL hal_IspiTxFifoAvail(HAL_ISPI_CS_T csId)
{
#ifdef FPGA
    return 1;
#endif // FPGA
    return boot_IspiTxFifoAvail();
}


// =============================================================================
// hal_IspiTxDmaDone
// -----------------------------------------------------------------------------
/// Check if the transmission is finished.
///
/// This function returns \c TRUE when the last DMA transfer is finished.
/// Before sending new data in DMA mode, the previous transfer must be finished,
/// hence the use of this function for polling.\n
/// Note that the DMA transfer can be finished but the Tx FIFO of the ISPI is
/// not empty. Before shutting down the ISPI, one must check that the ISPI FIFO
/// is empty and that the last byte has been completely sent by using
/// #hal_spiTxFinished.\n
/// Even if the Tx FIFO is not empty, if a previous DMA transfer is over, one
/// can start a new DMA transfert
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return \c TRUE is the previous DMA transfert is finshed.\n
///          \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_IspiTxDmaDone(HAL_ISPI_CS_T csId)
{
#ifdef FPGA
    return TRUE;
#endif // FPGA
    return boot_IspiTxDmaDone();
}


// =============================================================================
// hal_IspiTxFinished
// -----------------------------------------------------------------------------
/// Check if the last transfer is done.
/// This function returns \c TRUE when the transmit FIFO is empty and when the
/// last byte is completely sent. It should be called before closing the ISPI if
/// the last bytes of the transfer are important.\n
/// This function should not be called between transfers, in direct or DMA mode.
/// The @link #hal_IspiTxFifoLevel FIFO level @endlink for direct mode and the
/// @link #hal_IspiTxDmaDone DMA done indication @endlink for DMA allow
/// for a more optimized transmission.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return \c TRUE if the last tranfer is done and the Tx FIFO empty.\n
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL HAL_BOOT_FUNC_INTERNAL hal_IspiTxFinished(HAL_ISPI_CS_T csId)
{
#ifdef FPGA
    return TRUE;
#endif // FPGA
    return boot_IspiTxFinished();
}



// =============================================================================
// hal_IspiGetData
// -----------------------------------------------------------------------------
/// Get one datum.
///
/// This functions gets one element of 32 bits from the ISPI Rx Fifo.
/// If the Fifo was empty at the time #hal_IspiGetData() is called, 0
/// is returned and \c recData is untainted. Otherwise one datum is get
/// from the Fifo and 1 is returned
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param  recData Pointer to store the received datum.
/// @return Returns the number of received data (1 or 0, if the Fifo is empty).
// =============================================================================
PUBLIC UINT32 hal_IspiGetData(HAL_ISPI_CS_T csId, UINT32* recData)
{
#ifdef FPGA
    *recData = 0;
    return 1;
#endif // FPGA
    UINT32 nbAvailable;
    // Check the possibility to write on the CS.
    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;

    if (g_halIspiProperties.activatedCs == HAL_ISPI_CS_ALL
            || g_halIspiProperties.activatedCs == csId)
    {
        nbAvailable = boot_IspiGetData(recData);
    }
    else
    {
        nbAvailable = 0;
    }

    // Exit critical section.
    hwp_sysIrq->SC = status;
    return nbAvailable;

}


// =============================================================================
// hal_IspiGetDataBuffer
// -----------------------------------------------------------------------------
/// Get a bunch of data.
///
/// This functions gets \c length 32 bits data from the ISPI and stores them
/// starting from the address \c destAddress. The number returned is the number
/// of 32-bits data item actually received. In DMA mode, this function returns
/// 0 when no DMA channel is available. It returns length otherwise.
///
/// FIXME Implement DMA mode.
/// FIXME: Doc that needs the CS to be activated before use.
/// ATTENTION: Not safe in a concurrent environment
/// Idea to get it working are:
/// - use a global variable whose value is changed on polling or by a IRQ handler
/// -
/// ...
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param destAddress Pointer on the buffer to store received data
/// @param length Number of byte to receive.
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of received 32 bits data.
// =============================================================================
PUBLIC UINT32 hal_IspiGetDataBuffer(HAL_ISPI_CS_T csId, UINT32* destAddress, UINT32 length)
{

    if (g_halIspiProperties.activatedCs == HAL_ISPI_CS_ALL
            || g_halIspiProperties.activatedCs == csId)
    {
        return boot_IspiGetDataBuffer(destAddress, length, BOOT_ISPI_DIRECT_POLLING);
    }
    else
    {
        return 0;
    }
}

// =============================================================================
// hal_IspiRxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Rx FIFO.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return The number of 32 bits data items in the Rx FIFO.
// =============================================================================
PUBLIC UINT8 hal_IspiRxFifoLevel(HAL_ISPI_CS_T csId)
{
    return boot_IspiRxFifoLevel();
}



// =============================================================================
// hal_IspiRxDmaDone
// -----------------------------------------------------------------------------
/// Check if the reception is finished.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return This function returns \c TRUE when the last DMA transfer is finished.
/// Before receiving new data in DMA mode, the previous transfer must be
/// finished, hence the use of this function for polling.
// =============================================================================
PUBLIC BOOL hal_IspiRxDmaDone(HAL_ISPI_CS_T csId)
{
    return boot_IspiRxDmaDone();
}




// =============================================================================
// hal_IspiGetRxIfcTc
// -----------------------------------------------------------------------------
/// Get the RxIfcTc.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
/// @return The number of transfers yet to be performed on the current Ifc
/// reception of ISPI data.
// =============================================================================
PUBLIC UINT32 hal_IspiGetRxIfcTc(HAL_ISPI_CS_T csId)
{
    return boot_IspiGetRxIfcTc();
}


// =============================================================================
// hal_IspiClearRxDmaDone
// -----------------------------------------------------------------------------
/// Clear the RX DMA Done status.
/// Useful when doing DMA polling kind of transfers.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
// =============================================================================
PUBLIC VOID hal_IspiClearRxDmaDone(HAL_ISPI_CS_T csId)
{
    boot_IspiClearRxDmaDone();
}


// =============================================================================
// hal_IspiClearTxDmaDone
// -----------------------------------------------------------------------------
/// Clear the TX DMA Done status
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
// =============================================================================
PUBLIC VOID hal_IspiClearTxDmaDone(HAL_ISPI_CS_T csId)
{
    boot_IspiClearTxDmaDone();
}


// =============================================================================
// hal_IspiRxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the RxIfc channel owned by the request HAL_ISPI_IFC_REQ_RX.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
// =============================================================================
PUBLIC VOID hal_IspiRxIfcChannelRelease(HAL_ISPI_CS_T csId)
{
    boot_IspiRxIfcChannelRelease();
}


// =============================================================================
// hal_IspiTxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the TxIfc channel owned by the request HAL_ISPI_IFC_REQ_TX.
///
/// @param csId Identifier of the Chip Select on which behalf the operation is
/// done. This parameter is ignored.
// =============================================================================
PUBLIC VOID hal_IspiTxIfcChannelRelease(HAL_ISPI_CS_T csId)
{
    boot_IspiTxIfcChannelRelease();
}


// =============================================================================
// hal_IspiUpdateDivider
// -----------------------------------------------------------------------------
/// Update the ISPI divider so that the maximum frequency defined is not
/// overpassed.
///
/// @param sysFreq Newly applied system frequency
// =============================================================================
UINT32 hal_IspiUpdateDivider(HAL_SYS_FREQ_T sysFreq)
{
    UINT32 clkDivider;
    UINT32 cfg;
    // Round up
    clkDivider = (sysFreq + (2 * g_halIspiProperties.spiFreq - 1)) / ( 2 * g_halIspiProperties.spiFreq);
    clkDivider = (clkDivider) ? clkDivider-1 : 0;

    // Apply change
    cfg = hwp_ispi->cfg &  ~SPI_CLOCK_DIVIDER_MASK;
    // Not to use clock limiter (which is enabled by default
    // on 8809 and previous chips)
    cfg &= ~SPI_CLOCK_LIMITER;
    hwp_ispi->cfg = cfg | SPI_CLOCK_DIVIDER(clkDivider);
    return g_halIspiProperties.spiFreq;
}

// =============================================================================
// hal_IspiRegRead
// -----------------------------------------------------------------------------
/// Read a register through ISPI. It is executed with interrupt disabled.
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param  address ISPI Chip register address.
/// @return Returns register value.
// =============================================================================
PUBLIC UINT16 HAL_BOOT_FUNC hal_IspiRegRead(HAL_ISPI_CS_T csId, UINT16 address)
{
    UINT32 sc = hwp_sysIrq->SC;
    UINT32 cmd = SPI_READ_ENA | SPI_CS(csId) | (1<<25) | ((address & 0x1ff) << 16);

    // wait at least one tx space
    while (GET_BITFIELD(hwp_ispi->status, SPI_TX_SPACE) == 0)
        ;

    hwp_ispi->rxtx_buffer = cmd;

    // wait tx finish and rx ready
    for (;;)
    {
        UINT32 status = hwp_ispi->status;
        if ((status & SPI_ACTIVE_STATUS) == 0 &&
                GET_BITFIELD(status, SPI_TX_SPACE) == SPI_TX_FIFO_SIZE &&
                GET_BITFIELD(status, SPI_RX_LEVEL) > 0)
            break;
    }

    UINT16 data = hwp_ispi->rxtx_buffer & 0xffff;
    hwp_sysIrq->SC = sc;
    return data;
}

// =============================================================================
// hal_IspiRegWrite
// -----------------------------------------------------------------------------
/// Write a register through ISPI. It is executed with interrupt disabled.
///
/// @param csId Identifier of the ISPI Chip Select on which to get data.
/// @param address ISPI Chip register address.
/// @param data Register value.
// =============================================================================
PUBLIC VOID HAL_BOOT_FUNC_INTERNAL hal_IspiRegWrite(HAL_ISPI_CS_T csId, UINT16 address, UINT16 data)
{
    UINT32 sc = hwp_sysIrq->SC;
    UINT32 cmd = SPI_CS(csId) | (0<<25) | ((address & 0x1ff) << 16) | data;

    // wait at least one tx space
    while (GET_BITFIELD(hwp_ispi->status, SPI_TX_SPACE) == 0)
        ;

    hwp_ispi->rxtx_buffer = cmd;

    // wait tx finish
    for (;;)
    {
        UINT32 status = hwp_ispi->status;
        if ((status & SPI_ACTIVE_STATUS) == 0 &&
                GET_BITFIELD(status, SPI_TX_SPACE) == SPI_TX_FIFO_SIZE)
            break;
    }

    hwp_sysIrq->SC = sc;
}

#endif // CHIP_ISPI_CS_QTY


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
#include "spi.h"
#include "sys_ifc.h"
#include "sys_irq.h"
#include "cfg_regs.h"

#include "boot_ispi.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================
#define BOOT_ISPI_ROMED_SYSTEM_FREQUENCY    (26000000)

/// Unknown IFC channel.
#define BOOT_ISPI_IFC_UNKNOWN_CHANNEL       0xFF

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
    UINT8                       rxIfcCh;
    UINT8                       txIfcCh;
} BOOT_ISPI_PROP_T;


// =============================================================================
// BOOT_ISPI_IFC_REQ_ID_T
// -----------------------------------------------------------------------------
/// Type use to describe the IFC request for the ISPI.
// =============================================================================
typedef enum
{
    /// Write ISPI request.
    BOOT_ISPI_IFC_REQ_TX = 6,

    /// Read ISPI request.
    BOOT_ISPI_IFC_REQ_RX = 7
} BOOT_ISPI_IFC_REQ_ID_T;

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Global variable holding the state of each ISPI present in the system,
/// and holding the configuration of every one of its (opened) CS.
PRIVATE BOOT_ISPI_PROP_T g_bootIspiProperties
=
{
    .rxIfcCh    = BOOT_ISPI_IFC_UNKNOWN_CHANNEL,
    .txIfcCh    = BOOT_ISPI_IFC_UNKNOWN_CHANNEL,
};


/// Global (common to all Chip Select) configuration for the ISPI
/// TODO Double check with Adv-Asic guys.
/// TODO Triple check with the FM chip.
PROTECTED CONST BOOT_ISPI_CFG_T g_bootIspiRomedCfg
=
{
    .csFmActiveLow  = TRUE,
    .csAbbActiveLow = TRUE,
    .csPmuActiveLow = TRUE,
    .clkFallEdge    = TRUE,
    .clkDelay       = BOOT_ISPI_HALF_CLK_PERIOD_0,
    .doDelay        = BOOT_ISPI_HALF_CLK_PERIOD_0,
    .diDelay        = BOOT_ISPI_HALF_CLK_PERIOD_2,
    .csDelay        = BOOT_ISPI_HALF_CLK_PERIOD_1,
    .csPulse        = BOOT_ISPI_HALF_CLK_PERIOD_3,
    .frameSize      = 26,
    .oeRatio        = 0,
    .spiFreq        = 13000000,
};


// =============================================================================
//  FUNCTIONS
// ============================================================================

// =============================================================================
// boot_IspiGetRomedCfg
// -----------------------------------------------------------------------------
/// Returned a pointer to the romed ISPI configuration, that is used by default
/// in the boot rom code or when #boot_IspiOpen() is called with a \c NULL
/// parameter.
/// @return Pointer to the romed ISPI default configuration.
// =============================================================================
CONST BOOT_ISPI_CFG_T* boot_IspiGetRomedCfg(VOID)
{
    return &g_bootIspiRomedCfg;
}

// =============================================================================
// boot_IspiIfcTransferStart
// -----------------------------------------------------------------------------
/// Start an IFC transfer
///
/// This is a non blocking function that starts the transfer
/// and returns the channel number.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved,
/// using the register syntax naming.
/// @param memStartAddr. Start address of the buffer where data
/// to be sent are located or where to put the data read, according
/// to the request defined by the previous parameter. Must be aligned
/// on a 32 bits boundary.
/// @param xferSize Number of bytes to transfer. The maximum size
/// is 2^20 - 1 bytes.
// =============================================================================
PRIVATE UINT8 boot_IspiIfcTransferStart(BOOT_ISPI_IFC_REQ_ID_T requestId, UINT32* memStartAddr, UINT32 xferSize)
{
    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;
    UINT8 channel;

    channel = SYS_IFC_CH_TO_USE(hwp_sysIfc->get_ch) ;

    if (channel >= SYS_IFC_STD_CHAN_NB)
    {
        hwp_sysIrq->SC = status;
        return BOOT_ISPI_IFC_UNKNOWN_CHANNEL;
    }

    hwp_sysIfc->std_ch[channel].start_addr  =  (UINT32) memStartAddr;
    hwp_sysIfc->std_ch[channel].tc          =  xferSize;
    hwp_sysIfc->std_ch[channel].control     = (SYS_IFC_REQ_SRC(requestId)
            | SYS_IFC_CH_RD_HW_EXCH
            | SYS_IFC_SIZE
            | SYS_IFC_ENABLE
            | SYS_IFC_AUTODISABLE);
    // Exit critical section.
    hwp_sysIrq->SC = status;
    return channel;
}


// =============================================================================
// boot_IspiIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of a channel owned by a request.
///
/// The channel is only released if the specified request
/// owns the channel.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel to release
// =============================================================================
PRIVATE VOID boot_IspiIfcChannelRelease(UINT32 requestId, UINT8 channel)
{
    UINT32 status;

    // Here, we consider the transfer as previously finished.
    if (channel == BOOT_ISPI_IFC_UNKNOWN_CHANNEL) return;

    status = hwp_sysIrq->SC;
    if (GET_BITFIELD(hwp_sysIfc->std_ch[channel].control, SYS_IFC_REQ_SRC) == requestId)
    {
        // Disable this channel.
        hwp_sysIfc->std_ch[channel].control = (SYS_IFC_REQ_SRC(requestId)
                                               | SYS_IFC_CH_RD_HW_EXCH
                                               | SYS_IFC_DISABLE);
        hwp_sysIfc->std_ch[channel].tc =  0;
    }
    hwp_sysIrq->SC = status;
}


// =============================================================================
// boot_IspiOpen
// -----------------------------------------------------------------------------
/// Open the ISPI driver.
///
/// @param spiConfigPtr Pointer to the ISPI configuration to use. If \c NULL,
/// a default, romed configuration is used.
// =============================================================================
PUBLIC VOID boot_IspiOpen(CONST BOOT_ISPI_CFG_T* spiConfigPtr)
{
    UINT32  clkDivider  = 0;
    UINT32  cfgReg      = 0;
    UINT32  ctrlReg     = 0;
    UINT32  ispiFreq    = 0;

    // When no configuration is passed, use the default
    // romed one.
    if (spiConfigPtr == NULL)
    {
        spiConfigPtr = &g_bootIspiRomedCfg;
    }

    //  Configuration setter.
    cfgReg =
        ((spiConfigPtr->csPmuActiveLow ? SPI_CS_POLARITY_0_ACTIVE_LOW:
          SPI_CS_POLARITY_0_ACTIVE_HIGH)
         | (spiConfigPtr->csAbbActiveLow ? SPI_CS_POLARITY_1_ACTIVE_LOW:
            SPI_CS_POLARITY_1_ACTIVE_HIGH)
         | (spiConfigPtr->csFmActiveLow ? SPI_CS_POLARITY_2_ACTIVE_LOW:
            SPI_CS_POLARITY_2_ACTIVE_HIGH));
    // Calculate divider.
    ispiFreq = spiConfigPtr->spiFreq;

    // In the romed boot code, the only supported
    // system frequency is 26 MHz. In case of system frequency change, a not romed
    // function will be implemented and update the divider accordingly. (Round up)
    clkDivider = (BOOT_ISPI_ROMED_SYSTEM_FREQUENCY + (2 * ispiFreq - 1)) / ( 2 * ispiFreq);


    // There is a plus one in the register.
    clkDivider = (clkDivider) ? clkDivider-1 : 0;

    // Update divider in the registry image.
    cfgReg &=  ~SPI_CLOCK_DIVIDER_MASK;
    // NOTE:
    // (Not to use clock limiter any more, but 8809 ROM has been built ...)
    cfgReg |= SPI_CLOCK_DIVIDER(clkDivider);


    ctrlReg =
        (
            SPI_ENABLE
            // CS: We use the configuration from the data (tx Fifo) register.
            | SPI_CS_SEL(0)
            // Input enable: We use the configuration from the data (tx Fifo) register.
            | 0
            | (spiConfigPtr->clkFallEdge?SPI_CLOCK_POLARITY:0)
            | SPI_CLOCK_DELAY(spiConfigPtr->clkDelay )
            | SPI_DO_DELAY(spiConfigPtr->doDelay )
            | SPI_DI_DELAY(spiConfigPtr->diDelay )
            | SPI_CS_DELAY(spiConfigPtr->csDelay )
            | SPI_CS_PULSE(spiConfigPtr->csPulse )
            | SPI_FRAME_SIZE(spiConfigPtr->frameSize - 1 )
            // Normally No Output Enable Delay on ISPI, but needed for FPGA
            | SPI_OE_DELAY(spiConfigPtr->oeRatio)
            // Control comes from the data.
            | SPI_CTRL_DATA_MUX_SEL_DATA_REG_SEL
            // Input selection: We use the data register.
            | SPI_INPUT_SEL(0)
        );

    //  ------------------------------
    //  Transfert mode selection.
    //  ------------------------------
    g_bootIspiProperties.rxIfcCh = BOOT_ISPI_IFC_UNKNOWN_CHANNEL;
    g_bootIspiProperties.txIfcCh = BOOT_ISPI_IFC_UNKNOWN_CHANNEL;


    // Activate the ISPI.
    hwp_spi3->cfg  = cfgReg;
    hwp_spi3->ctrl = ctrlReg;

    // No IRQ.
    hwp_spi3->irq = 0;

}


// =============================================================================
// boot_IspiFlushFifos
// -----------------------------------------------------------------------------
/// Flush both ISPI Fifos.
// =============================================================================
PUBLIC VOID boot_IspiFlushFifos(VOID)
{
    hwp_spi3->status = SPI_FIFO_FLUSH;
}


// =============================================================================
// boot_IspiClearRxDmaDone
// -----------------------------------------------------------------------------
/// clear the RX DMA Done status
// =============================================================================
PUBLIC VOID boot_IspiClearRxDmaDone(VOID)
{
    hwp_spi3->status = SPI_RX_DMA_DONE;
}


// =============================================================================
// boot_IspiClearTxDmaDone
// -----------------------------------------------------------------------------
/// clear the TX DMA Done status
// =============================================================================
PUBLIC VOID boot_IspiClearTxDmaDone(VOID)
{
    hwp_spi3->status = SPI_TX_DMA_DONE;
}


// =============================================================================
// boot_IspiClose
// -----------------------------------------------------------------------------
/// Close the ISPI.
// =============================================================================
PUBLIC VOID boot_IspiClose(VOID)
{
    /// Turn off the ISPI. and mask its irqs
    hwp_spi3->ctrl = 0;
    hwp_spi3->irq = 0;

    /// Disable the Rx IFC Channel.
    if (g_bootIspiProperties.rxIfcCh != BOOT_ISPI_IFC_UNKNOWN_CHANNEL)
    {
        boot_IspiIfcChannelRelease(BOOT_ISPI_IFC_REQ_RX, g_bootIspiProperties.rxIfcCh);
    }

    /// Disable the Tx IFC Channel.
    if (g_bootIspiProperties.txIfcCh != BOOT_ISPI_IFC_UNKNOWN_CHANNEL)
    {
        boot_IspiIfcChannelRelease(BOOT_ISPI_IFC_REQ_TX, g_bootIspiProperties.txIfcCh);
    }

    // Clear all status
    hwp_spi3->status = 0xffffffff;

    // Reset the configuration
    g_bootIspiProperties.rxIfcCh    = BOOT_ISPI_IFC_UNKNOWN_CHANNEL;
    g_bootIspiProperties.txIfcCh    = BOOT_ISPI_IFC_UNKNOWN_CHANNEL;

}



// =============================================================================
// boot_IspiRxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the RxIfc channel owned by
/// the request BOOT_ISPI_IFC_REQ_RX.
// =============================================================================
PUBLIC VOID boot_IspiRxIfcChannelRelease(VOID)
{
    boot_IspiIfcChannelRelease(BOOT_ISPI_IFC_REQ_RX, g_bootIspiProperties.rxIfcCh);
}


// =============================================================================
// boot_IspiTxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the TxIfc channel owned by
/// the request BOOT_ISPI_IFC_REQ_TX.
// =============================================================================
PUBLIC VOID boot_IspiTxIfcChannelRelease(VOID)
{
    boot_IspiIfcChannelRelease(BOOT_ISPI_IFC_REQ_TX, g_bootIspiProperties.rxIfcCh);
}




// =============================================================================
// boot_IspiSendData
// -----------------------------------------------------------------------------
/// Send one data.
/// This functions sends one data frame.
/// The number returned is the number of data frames actually sent.
///
/// @param csId The CS to use to send the data. This cs must be activated before
/// sending data.
/// @param data Frame of data to send.
/// @param read \c TRUE if the response of the device to this sent data is
/// expected to be read later and thus will be put in the read Fifo.
/// @return 1 if the data was sent, 0 otherwise.
// =============================================================================
PUBLIC UINT32 boot_IspiSendData(BOOT_ISPI_CS_T csId, UINT32 data, BOOL read)
{
    UINT32 freeRoom;

    // Clear data upper bit to only keep the data frame.
    UINT32 reg = data & ~(SPI_CS_MASK | SPI_READ_ENA_MASK);

    // Add CS and read mode bit
    reg |= SPI_CS(csId) | (read?SPI_READ_ENA:0);

    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;

    // Check FIFO availability.
    freeRoom = GET_BITFIELD(hwp_spi3->status, SPI_TX_SPACE);

    if (freeRoom > 0)
    {
        // Write data.
        hwp_spi3->rxtx_buffer = reg;

        // Exit critical section.
        hwp_sysIrq->SC = status;
        return 1;
    }
    else
    {
        // Exit critical section.
        hwp_sysIrq->SC = status;
        return 0;
    }

}


// =============================================================================
// boot_IspiSendDataBuffer
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
/// @param startAddress Pointer on the buffer to send
/// @param length number of bytes to send (Up to 4 kB).
/// @param mode Mode of the transfer (Direct or DMA).
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of data (32 bits words) sent.
// =============================================================================
PUBLIC UINT32 boot_IspiSendDataBuffer(CONST UINT32* startAddress, UINT32 length, BOOT_ISPI_TRANSFERT_MODE_T mode)
{
    // FIXME Add a kind of semaphore to ensure there is no concurrent access.
    UINT32 i;
    UINT32 freeRoom;

    if (mode == BOOT_ISPI_DIRECT_POLLING)
    {
        // -----------------------------
        // DIRECT TRANSFER
        // -----------------------------
        // Enter critical section.
        UINT32 status = hwp_sysIrq->SC;

        freeRoom = GET_BITFIELD(hwp_spi3->status, SPI_TX_SPACE);

        if (freeRoom > length)
        {
            freeRoom = length;
        }

        //  Send data byte by byte.
        for (i = 0; i < freeRoom; i++)
        {
            hwp_spi3->rxtx_buffer = *(startAddress + i);
        }

        // Exit critical section.
        hwp_sysIrq->SC = status;
        return freeRoom;
    }
    else
    {
        // -----------------------------
        // DMA TRANSFER
        // -----------------------------
        // We transfer 32 bits items (4 bytes)
        g_bootIspiProperties.txIfcCh = boot_IspiIfcTransferStart(BOOT_ISPI_IFC_REQ_TX,
                                       (UINT32*)startAddress, length*4);

        // get IFC channel and start if any available
        if (g_bootIspiProperties.txIfcCh == BOOT_ISPI_IFC_UNKNOWN_CHANNEL)
        {
            // No channel available.
            // No data received.
            return 0;
        }
        else
        {
            // all data will be fetched.
            return length;
        }
    }
}



// =============================================================================
// boot_IspiTxFifoAvail
// -----------------------------------------------------------------------------
/// Get available data spaces in the Spi Tx FIFO.
/// This function returns the available space in the Tx FIFO, as a number
/// of 32 bits data that can be filled into it.
///
/// @return The size of the available space in the Tx FIFO. (In Fifo elements.)
// =============================================================================
PUBLIC UINT8 boot_IspiTxFifoAvail(VOID)
{
    UINT8 freeRoom;

    // Get avail level.
    freeRoom = GET_BITFIELD(hwp_spi3->status, SPI_TX_SPACE);

    return freeRoom;
}


// =============================================================================
// boot_IspiTxDmaDone
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
/// @return \c TRUE is the previous DMA transfert is finshed.\n
///          \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL boot_IspiTxDmaDone(VOID)
{
    if (g_bootIspiProperties.txIfcCh == BOOT_ISPI_IFC_UNKNOWN_CHANNEL)
    {
        return TRUE;
    }

    if (hwp_spi3->status & SPI_TX_DMA_DONE)
    {
        // clear done
        g_bootIspiProperties.txIfcCh = BOOT_ISPI_IFC_UNKNOWN_CHANNEL;
        hwp_spi3->status = SPI_TX_DMA_DONE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// =============================================================================
// boot_IspiTxFinished
// -----------------------------------------------------------------------------
/// Check if the last transfer is done.
/// This function returns \c TRUE when the transmit FIFO is empty and when the
/// last byte is completely sent. It should be called before closing the ISPI if
/// the last bytes of the transfer are important.\n
/// This function should not be called between transfers, in direct or DMA mode.
/// The @link #boot_IspiTxFifoLevel FIFO level @endlink for direct mode and the
/// @link #boot_IspiTxDmaDone DMA done indication @endlink for DMA allow
/// for a more optimized transmission.
///
/// @return \c TRUE if the last tranfer is done and the Tx FIFO empty.\n
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL boot_IspiTxFinished(VOID)
{
    UINT32 spiStatus;

    spiStatus = hwp_spi3->status;

    // If ISPI FSM is active and the TX Fifo is empty
    // (ie available space == Fifo size), the tf is not done
    if ((!(hwp_spi3->status & SPI_ACTIVE_STATUS))
            && (SPI_TX_FIFO_SIZE == GET_BITFIELD(spiStatus, SPI_TX_SPACE)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



// =============================================================================
// boot_IspiGetData
// -----------------------------------------------------------------------------
/// Get one datum.
///
/// This functions gets one element of 32 bits from the ISPI Rx Fifo.
/// If the Fifo was empty at the time #boot_IspiGetData() is called, 0
/// is returned and \c recData is untainted. Otherwise one datum is get
/// from the Fifo and 1 is returned
///
/// @param  recData Pointer to store the received datum.
/// @return Returns the number of received data (1 or 0, if the Fifo is empty).
// =============================================================================
PUBLIC UINT32 boot_IspiGetData(UINT32* recData)
{
    UINT32 nbAvailable;

    // Enter critical section.
    UINT32 status = hwp_sysIrq->SC;

    nbAvailable = GET_BITFIELD(hwp_spi3->status, SPI_RX_LEVEL);

    if (nbAvailable > 0)
    {
        *recData = hwp_spi3->rxtx_buffer;

        // Exit critical section.
        hwp_sysIrq->SC = status;
        return 1;
    }
    else
    {
        // Exit critical section.
        hwp_sysIrq->SC = status;
        return 0;
    }

}


// =============================================================================
// boot_IspiGetDataBuffer
// -----------------------------------------------------------------------------
/// Get a bunch of data.
///
/// This functions gets \c length 32 bits data from the ISPI and stores them
/// starting from the address \c destAddress. The number returned is the number
/// of 32-bits data item actually received. In DMA mode, this function returns
/// 0 when no DMA channel is available. It returns length otherwise.
///
/// @param destAddress Pointer on the buffer to store received data
/// @param length Number of byte to receive.
/// @param mode Mode of the transfer (Direct or DMA).
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of received 32 bits data.
// =============================================================================
PUBLIC UINT32 boot_IspiGetDataBuffer(UINT32* destAddress, UINT32 length, BOOT_ISPI_TRANSFERT_MODE_T mode)
{
    UINT32 i;
    UINT32 nbAvailable;

    if (mode == BOOT_ISPI_DIRECT_POLLING)
    {
        // -----------------------------
        // DIRECT TRANSFER
        // -----------------------------
        // Enter critical section.
        UINT32 status = hwp_sysIrq->SC;

        nbAvailable = GET_BITFIELD(hwp_spi3->status, SPI_RX_LEVEL);

        if (nbAvailable > length)
        {
            nbAvailable = length;
        }

        //  Get data byte by byte.
        for (i = 0; i < nbAvailable; i++)
        {
            *(destAddress + i) = hwp_spi3->rxtx_buffer;
        }

        // Exit critical section.
        hwp_sysIrq->SC = status;
        return nbAvailable;
    }
    else
    {
        // -----------------------------
        // DMA TRANSFER
        // -----------------------------
        // We transfer 32 bits items (4 bytes)
        g_bootIspiProperties.rxIfcCh = boot_IspiIfcTransferStart(BOOT_ISPI_IFC_REQ_RX,
                                       destAddress, length*4);

        // Get IFC channel and start if any available
        if (g_bootIspiProperties.rxIfcCh == BOOT_ISPI_IFC_UNKNOWN_CHANNEL)
        {
            // No channel available.
            // No data received.
            return 0;
        }
        else
        {
            // All data will be fetched.
            return length;
        }
    }
}

// =============================================================================
// boot_IspiRxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Rx FIFO.
///
/// @return The number of 32 bits data items in the Rx FIFO.
// =============================================================================
PUBLIC UINT8 boot_IspiRxFifoLevel(VOID)
{
    UINT8 rxLevel;
    // Get level
    rxLevel = GET_BITFIELD(hwp_spi3->status, SPI_RX_LEVEL);
    return rxLevel;
}



// =============================================================================
// boot_IspiRxDmaDone
// -----------------------------------------------------------------------------
/// Check if the reception is finished.
///
/// @return This function returns \c TRUE when the last DMA transfer is finished.
/// Before receiving new data in DMA mode, the previous transfer must be
/// finished, hence the use of this function for polling.
// =============================================================================
PUBLIC BOOL boot_IspiRxDmaDone(VOID)
{
    if (g_bootIspiProperties.rxIfcCh == BOOT_ISPI_IFC_UNKNOWN_CHANNEL)
    {
        return TRUE;
    }

    if (hwp_spi3->status & SPI_RX_DMA_DONE)
    {
        // clear done
        g_bootIspiProperties.rxIfcCh = BOOT_ISPI_IFC_UNKNOWN_CHANNEL;
        hwp_spi3->status = SPI_RX_DMA_DONE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// =============================================================================
// boot_IspiForcePin
// -----------------------------------------------------------------------------
/// Force an ISPI pin to a certain state or release the force mode
/// and put the pin back to normal ISPI mode.
///
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
// =============================================================================
PUBLIC VOID boot_IspiForcePin(BOOT_ISPI_PIN_T pin,  BOOT_ISPI_PIN_STATE_T state)
{
    // Parameter check.
    switch (pin)
    {
        case BOOT_ISPI_PIN_CS_FM  :
            hwp_spi3->pin_control &= ~SPI_CS0_CTRL_MASK;
            hwp_spi3->pin_control |= SPI_CS0_CTRL(state);
            break;

        case BOOT_ISPI_PIN_CS_ABB  :
            hwp_spi3->pin_control &= ~SPI_CS1_CTRL_MASK;
            hwp_spi3->pin_control |= SPI_CS1_CTRL(state);
            break;

        case BOOT_ISPI_PIN_CS_PMU  :
            hwp_spi3->pin_control &= ~SPI_CS2_CTRL_MASK;
            hwp_spi3->pin_control |= SPI_CS2_CTRL(state);
            break;

        case BOOT_ISPI_PIN_CLK  :
            hwp_spi3->pin_control &= ~SPI_CLK_CTRL_MASK;
            hwp_spi3->pin_control |= SPI_CLK_CTRL(state);
            break;

        case BOOT_ISPI_PIN_DO   :
            hwp_spi3->pin_control &= ~SPI_DO_CTRL_MASK;
            hwp_spi3->pin_control |= SPI_DO_CTRL(state);
            break;

        case BOOT_ISPI_PIN_DI   :
            // Nothing doable
            break;

        default:
            // No assert in ROMed code: Do nothing.
            break;
    }
}


// =============================================================================
// boot_IspiGetRxIfcTc
// -----------------------------------------------------------------------------
/// Get the RxIfcTc.
/// @return ifcTc
// =============================================================================
PUBLIC UINT32 boot_IspiGetRxIfcTc(VOID)
{
    UINT32 ifcTc;
    ifcTc = hwp_sysIfc->std_ch[g_bootIspiProperties.rxIfcCh].tc;
    return ifcTc;
}




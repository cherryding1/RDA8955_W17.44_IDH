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




#ifndef _BOOT_ISPI_H_
#define _BOOT_ISPI_H_

#include "cs_types.h"

/// @defgroup spi HAL SPI Driver
/// This document describes the characteristics of the SPI module and how
/// to use them via its Hardware Abstraction Layer API.
///
/// One SPI with 2 chip selects is available. All of them support the
/// following functions:
/// - Supports Write only or Read Write operation
/// - Programmable receive and transmit FIFO (16 bytes deep each)
/// - DMA capabilities to allow fully automated data transfer
/// - Wide selection of programmable interrupts to allow interrupt driven data
///   transfer management.
/// - Up to 13 Mbits/s transfer rate.
/// - stream mode, associated with a pattern mode to program the reception of
/// a given number of bytes after a pattern has been read on the CS.
///
/// \b Note: SPI can only read while sending, and it reads the same number of
/// bits, if the chips interfaced does not handle this full duplex operation,
/// you'll need to write zeros when you actually need to read. And you'll also
/// need to read from FIFO the garbage read during actual write operation
/// (unless you change the CS setting to disable the read before writing).
///
/// @par I/O Signals
///
/// <TABLE BORDER >
/// <TR><TH>Signal Name</TH><TH>Direction</TH></TR>
///
/// <TR><TD> SPI_SCLK  </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_SEN_0 </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_SEN_1 </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_DO    </TD><TD> Output </TD></TR>
/// <TR><TD> SPI_DI    </TD><TD> Input  </TD></TR>
/// </TABLE>
///
///
/// @par SPI Operation
/// A configuration structure of type BOOT_ISPI_CFG_T allows to define the SPI behavior.
/// The available configurations are as follow :
/// - @b Chip @b Select @b Choice \n
///          The SPI has three chip select, the available values are defined
///          as follow:
///          - #BOOT_ISPI_CS0 &mdash; The chip select 0
///          - #BOOT_ISPI_CS1 &mdash; The chip select 1
///          - #BOOT_ISPI_CS2 &mdash; The chip select 2
///          - #BOOT_ISPI_CS3 &mdash; The chip select 3
///          .
///          \n
/// - @b Timing @b and @b Data @b Format \n
///          The SPI module supports several configuration parameters to allow
///          connection with a wide range of devices. \n
///          \n
///          The polarity of the serial clock is configurable. The polarity of
///          both the Chip Selects are needed for operating on any one of those
///          Chip Select. Therefore, this information is recorded in the
///          target configuration structure (tgt_BoardConfig), which records
///          the polarity of both Chip Selects.
///          If the first edge after CS activation is a falling edge, set
///          the proper field to  \c TRUE.\n
///          \n
///          The configuration delays are all defined in half of SCLK period.\n
///          The delay between the CS activation and the first clock edge can
///          be 0 to 2 half clocks.\n
///          The delay between the CS activation and the output of the data can
///          be 0 to 2 half clocks.\n
///          The delay between the CS activation and the sampling of the input
///          data can be 0 to 3 half clocks.\n
///          The delay between the end of transfer and the CS deactivation can
///          be 0 to 3 half clocks.\n
///          The time when the CS must remain deactivated before a new transfer
///          can be 0 to 3 half clocks.\n
///          \n
///          The transfer size is defined by the frame size. It is the number
///          of bits per transfer, it can be 4 to 32 bits.\n
///          \n
///          The SCLK period is defined by setting the divider between the
///          system clock and the half of the SCLK (the SCLK is actually
///          divided by \c clkDivider x 2).\n
///          \n
///          Transfer sample <CODE> (clkFallEdge = true, clkDelay = 0,
///          doDelay = 0, diDelay = 1, csDelay = 1, csPulse = 2) </CODE>
///          \n
///          @image latex spi_timing.png "SPI Timing"
///          @image html spi_timing.png "SPI Timing"
///          \n
/// - @b Transfer @b Modes \n
///          To allow for an easy use of the SPI modules, a non blocking
///          Hardware Abstraction Layer interface is provided. Each transfer
///          direction (send/receive) can be configured as:
///          - @b Direct @b polling: \n
///          The application sends/receives the data directly to/from the
///          hardware module. The number of bytes actually sent/received is
///          returned. No Irq is generated. \n
///          \n
///          - @b DMA @b polling: \n
///          The application sends/receives the data through a DMA to the
///          hardware module. The function returns 0 when no DMA channel is
///          available. No bytes are sent. The function returns the number of
///          bytes to send when a DMA resource is available. They will all be
///          sent. A function allows to check if the previous DMA transfer is
///          finished. No new DMA transfer in the same direction will be
///          allowed  before the end of the previous transfer. \n
///          \n
///          .
///          \n
/// @par Configuration Structure
/// A configuration structure allows to open or change the SPI with the desired
/// parameters : #BOOT_ISPI_CFG_T.
///
/// @{

// =============================================================================
//  MACROS
// =============================================================================
/// Defines the SPI minimum frame size
#define BOOT_ISPI_MIN_FRAME_SIZE        4

/// Defines the SPI maximum frame size
#define BOOT_ISPI_MAX_FRAME_SIZE        29


/// Macro to build a data to send on the ISPI through the boot_IspiSendDataBuffer.
/// This add the ISPI chip select on which the data are going to be sent.
/// (#boot_IspiSendData() building internally the data written on the TX buffer,
/// of the ISPI, #boot_IspiSendData() doesn't need to be fed with data prepared
/// previously by BOOT_ISPI_CS)
/// This macro MUST correspond to the private hardware description macro
/// SPI_CS(n) in spi.h
#define BOOT_ISPI_CS(cs)        (((n)&3)<<29)

/// Macro to build a data to send on the ISPI through the boot_IspiSendDataBuffer.
/// This tells the ISPI module that input data are expected for this sending
/// operation.
/// (#boot_IspiSendData() building internally the data written on the TX buffer,
/// of the ISPI, #boot_IspiSendData() doesn't need to be fed with data prepared
/// previously by BOOT_ISPI_REQ_READ)
/// This macro MUST correspond to the private hardware description macro
/// SPI_READ_ENA in spi.h
#define BOOT_ISPI_REQ_READ      (1<<31)




// =============================================================================
//  TYPES
// =============================================================================



// =============================================================================
// BOOT_ISPI_DELAY_T
// -----------------------------------------------------------------------------
/// Delays
/// Used to define the configuration delays
// =============================================================================
typedef enum
{
    /// Delay of 0 half-period
    BOOT_ISPI_HALF_CLK_PERIOD_0,
    /// Delay of 1 half-period
    BOOT_ISPI_HALF_CLK_PERIOD_1,
    /// Delay of 2 half-period
    BOOT_ISPI_HALF_CLK_PERIOD_2,
    /// Delay of 3 half-period
    BOOT_ISPI_HALF_CLK_PERIOD_3,

    BOOT_ISPI_HALF_CLK_PERIOD_QTY
} BOOT_ISPI_DELAY_T;



// =============================================================================
// BOOT_ISPI_CS_T
// -----------------------------------------------------------------------------
/// Chip Select
/// Used to select a Chip Select
// =============================================================================
typedef enum
{
    /// Chip Select for the PMU analog module.
    BOOT_ISPI_CS_PMU = 0,
    /// Chip Select for the ABB analog module.
    BOOT_ISPI_CS_ABB,
    /// Chip Select for the FM analog module.
    BOOT_ISPI_CS_FM,

    BOOT_ISPI_CS_QTY
} BOOT_ISPI_CS_T;

// =============================================================================
// BOOT_ISPI_TRANSFERT_MODE_T.
// -----------------------------------------------------------------------------
/// Data transfert mode: via DMA or direct. .
/// To allow for an easy use of the SPI modules, a non blocking Hardware
/// Abstraction Layer interface is provided. Each transfer direction
/// (send/receive) can be configured as:
// =============================================================================
typedef enum
{
    /// Direct polling: The application sends/receives the data directly to/from
    /// the hardware module. The number of bytes actually sent/received is
    /// returned. No Irq is generated.
    BOOT_ISPI_DIRECT_POLLING = 0,

    /// DMA polling: The application sends/receives the data through a DMA to
    /// the hardware module. The function returns 0 when no DMA channel is
    /// available. No bytes are sent. The function returns the number of bytes
    /// to send when a DMA resource is available. They will all be sent. A
    /// function allows to check if the previous DMA transfer is finished. No
    /// new DMA transfer in the same direction will be allowed before the end
    /// of the previous transfer.
    BOOT_ISPI_DMA_POLLING,

    /// The SPI is off.
    BOOT_ISPI_OFF,

    BOOT_ISPI_TM_QTY
} BOOT_ISPI_TRANSFERT_MODE_T;


// =============================================================================
// BOOT_ISPI_CFG_T
// -----------------------------------------------------------------------------
/// Structure for configuration.
/// A configuration structure allows to open or change the SPI with the desired
/// parameters.
// =============================================================================
typedef struct
{
    /// Polarity of the FM CS.
    BOOL csFmActiveLow;

    /// Polarity of the ABB CS.
    BOOL csAbbActiveLow;

    /// Polarity of the PMU CS.
    BOOL csPmuActiveLow;

    /// If the first edge after the CS activation is a falling edge, set to
    /// \c TRUE.\n Otherwise, set to \c FALSE.
    BOOL clkFallEdge;

    /// The delay between the CS activation and the first clock edge,
    /// can be 0 to 2 half clocks.
    BOOT_ISPI_DELAY_T clkDelay;

    /// The delay between the CS activation and the output of the data,
    /// can be 0 to 2 half clocks.
    BOOT_ISPI_DELAY_T doDelay;

    /// The delay between the CS activation and the sampling of the input data,
    /// can be 0 to 3 half clocks.
    BOOT_ISPI_DELAY_T diDelay;

    /// The delay between the end of transfer and the CS deactivation, can be
    /// 0 to 3 half clocks.
    BOOT_ISPI_DELAY_T csDelay;

    /// The time when the CS must remain deactivated before a new transfer,
    /// can be 0 to 3 half clocks.
    BOOT_ISPI_DELAY_T csPulse;

    /// Frame size in bits
    UINT32 frameSize;

    /// OE ratio - Value from 0 to 31 is the number of data out to transfert
    /// before the SPI_DO pin switches to input.
    /// Not needed in the chip, but needed for the FPGA
    UINT8 oeRatio;

    /// SPI maximum clock frequency: the SPI clock will be the highest
    /// possible value inferior to this parameter.
    UINT32 spiFreq;

} BOOT_ISPI_CFG_T;


// =============================================================================
// BOOT_ISPI_PIN_T
// -----------------------------------------------------------------------------
/// SPI Pin
///
/// Used to specify an SPI pin.
// =============================================================================
typedef enum
{
    /// Chip select pin for the PMU analog module.
    BOOT_ISPI_PIN_CS_PMU = 0,
    /// Chip select pin for the ABB analog module.
    BOOT_ISPI_PIN_CS_ABB,
    /// Chip select pin for the FM analog module.
    BOOT_ISPI_PIN_CS_FM,
    /// Clock pin
    BOOT_ISPI_PIN_CLK,
    /// Data out pin
    BOOT_ISPI_PIN_DO,
    /// Data in pin
    BOOT_ISPI_PIN_DI,
    BOOT_ISPI_PIN_LIMIT
} BOOT_ISPI_PIN_T;



// =============================================================================
// BOOT_ISPI_PIN_STATE_T
// -----------------------------------------------------------------------------
/// SPI Pin State
///
/// Used to specify the state of an SPI pin. It can be
/// zero, one or Z (high impedence, tri-state).
// =============================================================================
typedef enum
{
    BOOT_ISPI_PIN_STATE_SPI   =   0,
    BOOT_ISPI_PIN_STATE_Z     =   1,
    BOOT_ISPI_PIN_STATE_0     =   2,
    BOOT_ISPI_PIN_STATE_1     =   3
} BOOT_ISPI_PIN_STATE_T;


// =============================================================================
//  GLOBAL VARIABLES.
// =============================================================================
// =============================================================================
// g_bootIspiCfg
// -----------------------------------------------------------------------------
/// ISPI configuration.
// =============================================================================
EXPORT PROTECTED CONST BOOT_ISPI_CFG_T g_bootIspiCfg;

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// boot_IspiFlushFifos
// -----------------------------------------------------------------------------
/// Flush both ISPI Fifos.
// =============================================================================
PUBLIC VOID boot_IspiFlushFifos(VOID);



// =============================================================================
// boot_IspiClearRxDmaDone
// -----------------------------------------------------------------------------
/// clear the RX DMA Done status
// =============================================================================
PUBLIC VOID boot_IspiClearRxDmaDone(VOID);



// =============================================================================
// boot_IspiClearTxDmaDone
// -----------------------------------------------------------------------------
/// clear the TX DMA Done status
// =============================================================================
PUBLIC VOID boot_IspiClearTxDmaDone(VOID);



// =============================================================================
// boot_IspiOpen
// -----------------------------------------------------------------------------
/// Open the ISPI driver.
///
/// @param spiConfigPtr Pointer to the ISPI configuration to use. If \c NULL,
/// a default, romed configuration is used.
// =============================================================================
PUBLIC VOID boot_IspiOpen(CONST BOOT_ISPI_CFG_T* spiConfigPtr);


// =============================================================================
// boot_IspiClose
// -----------------------------------------------------------------------------
/// Close the ISPI.
// =============================================================================
PUBLIC VOID boot_IspiClose(VOID);


// =============================================================================
// boot_IspiRxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the RxIfc channel owned by
/// the request BOOT_ISPI_IFC_REQ_RX.
// =============================================================================
PUBLIC VOID boot_IspiRxIfcChannelRelease(VOID);



// =============================================================================
// boot_IspiTxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the TxIfc channel owned by
/// the request BOOT_ISPI_IFC_REQ_TX.
// =============================================================================
PUBLIC VOID boot_IspiTxIfcChannelRelease(VOID);



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
PUBLIC UINT32 boot_IspiSendData(BOOT_ISPI_CS_T csId, UINT32 data, BOOL read);



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
/// @param startAddress Pointer on the buffer to send
/// @param length number of bytes to send (Up to 4 kB).
/// @param mode Mode of the transfer (Direct or DMA).
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of data (32 bits words) sent.
// =============================================================================
PUBLIC UINT32 boot_IspiSendDataBuffer(CONST UINT32* startAddress, UINT32 length, BOOT_ISPI_TRANSFERT_MODE_T mode);




// =============================================================================
// boot_IspiTxFifoAvail
// -----------------------------------------------------------------------------
/// Get available data spaces in the Spi Tx FIFO.
/// This function returns the available space in the Tx FIFO, as a number
/// of 32 bits data that can be filled into it.
///
/// @return The size of the available space in the Tx FIFO. (In Fifo elements.)
// =============================================================================
PUBLIC UINT8 boot_IspiTxFifoAvail(VOID);



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
PUBLIC BOOL boot_IspiTxDmaDone(VOID);



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
PUBLIC BOOL boot_IspiTxFinished(VOID);




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
PUBLIC UINT32 boot_IspiGetData(UINT32* recData);


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
PUBLIC UINT32 boot_IspiGetDataBuffer(UINT32* destAddress, UINT32 length, BOOT_ISPI_TRANSFERT_MODE_T mode);


// =============================================================================
// boot_IspiRxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Rx FIFO.
///
/// @return The number of 32 bits data items in the Rx FIFO.
// =============================================================================
PUBLIC UINT8 boot_IspiRxFifoLevel(VOID);




// =============================================================================
// boot_IspiRxDmaDone
// -----------------------------------------------------------------------------
/// Check if the reception is finished.
///
/// @return This function returns \c TRUE when the last DMA transfer is finished.
/// Before receiving new data in DMA mode, the previous transfer must be
/// finished, hence the use of this function for polling.
// =============================================================================
PUBLIC BOOL boot_IspiRxDmaDone(VOID);



// =============================================================================
// boot_IspiForcePin
// -----------------------------------------------------------------------------
/// Force an ISPI pin to a certain state or release the force mode
/// and put the pin back to normal ISPI mode.
///
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
// =============================================================================
PUBLIC VOID boot_IspiForcePin(BOOT_ISPI_PIN_T pin,  BOOT_ISPI_PIN_STATE_T state);



// =============================================================================
// boot_IspiGetRxIfcTc
// -----------------------------------------------------------------------------
/// Get the RxIfcTc.
/// @return ifcTc
// =============================================================================
PUBLIC UINT32 boot_IspiGetRxIfcTc(VOID);


// =============================================================================
// boot_IspiGetRomedCfg
// -----------------------------------------------------------------------------
/// Returned a pointer to the romed ISPI configuration, that is used by default
/// in the boot rom code or when #boot_IspiOpen() is called with a \c NULL
/// parameter.
/// @return Pointer to the romed ISPI default configuration.
// =============================================================================
CONST BOOT_ISPI_CFG_T* boot_IspiGetRomedCfg(VOID);



///  @} <- End of SPI group


#endif //_BOOT_ISPI_H_







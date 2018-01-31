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


#ifndef _EDRV_GPIO_SPI_H_
#define _EDRV_GPIO_SPI_H_


typedef enum
{
    GPIO_SPI_BPS_80K,
    GPIO_SPI_BPS_160K,
    GPIO_SPI_BPS_STEP_1,
    GPIO_SPI_BPS_STEP_2,
    GPIO_SPI_BPS_STEP_3,
    GPIO_SPI_BPS_STEP_4,
    GPIO_SPI_BPS_STEP_5



} GPIO_SPI_BPS_T;




#include "cs_types.h"


// =============================================================================
//  MACROS
// =============================================================================
/// Defines the SPI minimum frame size
#define GPIO_SPI_MIN_FRAME_SIZE          4

/// Defines the SPI maximum frame size
#define GPIO_SPI_MAX_FRAME_SIZE          32


/// Defines the maximum number of Chip Select
/// possible on a same given SPI.
#define GPIO_SPI_CS_MAX_QTY 4


#define GPIO_UNKNOWN_CHANNEL      0xff

#define GPIO_SPI_CS_SEL_CS0              (0<<1)
#define GPIO_SPI_CS_SEL_CS1              (1<<1)
#define GPIO_SPI_CS_SEL_CS2              (2<<1)
#define GPIO_SPI_CS_SEL_CS3              (3<<1)


// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
// GPIO_SPI_ID_T
// -----------------------------------------------------------------------------
/// Defines which SPI is used by a function of the SPI. Depending on the
/// hardware you are running on, some of those SPIs might not exist
// =============================================================================
typedef enum
{
    GPIO_SPI,
    GPIO_SPI_2,
    GPIO_SPI_QTY = 1
} GPIO_SPI_ID_T;


typedef enum
{
    GPIO_SYS_FREQ_32K     = 32768,
    GPIO_SYS_FREQ_13M     = 13000000,
    GPIO_SYS_FREQ_26M     = 26000000,
    GPIO_SYS_FREQ_39M     = 39000000,
    GPIO_SYS_FREQ_52M     = 52000000,
    GPIO_SYS_FREQ_78M     = 78000000,
    GPIO_SYS_FREQ_104M    = 104000000,
    GPIO_SYS_FREQ_156M    = 156000000,
    GPIO_SYS_FREQ_208M    = 208000000,
    GPIO_SYS_FREQ_UNKNOWN = 0
} GPIO_SYS_FREQ_T;

// =============================================================================
// GPIO_SPI_DELAY_T
// -----------------------------------------------------------------------------
/// Delays
/// Used to define the configuration delays
// =============================================================================
typedef enum
{
    /// Delay of 0 half-period
    GPIO_SPI_HALF_CLK_PERIOD_0,
    /// Delay of 1 half-period
    GPIO_SPI_HALF_CLK_PERIOD_1,
    /// Delay of 2 half-period
    GPIO_SPI_HALF_CLK_PERIOD_2,
    /// Delay of 3 half-period
    GPIO_SPI_HALF_CLK_PERIOD_3,

    GPIO_SPI_HALF_CLK_PERIOD_QTY
} GPIO_SPI_DELAY_T;


// =============================================================================
// GPIO_SPI_CS_T
// -----------------------------------------------------------------------------
/// Chip Select
/// Used to select a Chip Select
// =============================================================================
typedef enum
{
    /// Chip Select 0
    GPIO_SPI_CS0 = 0,
    /// Chip Select 1
    GPIO_SPI_CS1,
    /// Chip Select 2

    /// Chip Select 3 - It doesn't necessarily exists for a given SPI.


    GPIO_SPI_CS_QTY
} GPIO_SPI_CS_T;






// =============================================================================
// GPIO_SPI_CFG_T
// -----------------------------------------------------------------------------
/// Structure for configuration.
/// A configuration structure allows to open or change the SPI with the desired
/// parameters.
// =============================================================================
typedef struct
{
    /// Select the Chip Select
    GPIO_SPI_CS_T enabledCS;

    /// Polarity of this CS
    BOOL csActiveLow;

    /// If the first edge after the CS activation is a falling edge, set to
    /// \c TRUE.\n Otherwise, set to \c FALSE.
    BOOL clkFallEdge;

    /// The delay between the CS activation and the first clock edge,
    /// can be 0 to 2 half clocks.
    GPIO_SPI_DELAY_T clkDelay;

    /// The delay between the CS activation and the output of the data,
    /// can be 0 to 2 half clocks.
    GPIO_SPI_DELAY_T doDelay;

    /// The delay between the CS activation and the sampling of the input data,
    /// can be 0 to 3 half clocks.
    GPIO_SPI_DELAY_T diDelay;

    /// The delay between the end of transfer and the CS deactivation, can be
    /// 0 to 3 half clocks.
    GPIO_SPI_DELAY_T csDelay;

    /// The time when the CS must remain deactivated before a new transfer,
    /// can be 0 to 3 half clocks.
    GPIO_SPI_DELAY_T csPulse;


    UINT32 spiFreq;


} GPIO_SPI_CFG_T;



// =============================================================================
// GPIO_SPI_PATTERN_MODE_T
// -----------------------------------------------------------------------------
/// The pattern mode can be used in two: record data after the pattern has been
/// received on the SPI, or data are not recorded while the pattern is seens on
/// the bus. GPIO_SPI_PATTERN_WHILE feature is not available for every chip.
// =============================================================================
typedef enum
{
    /// Standard mode, we don't care about the pattern
    GPIO_SPI_PATTERN_NO,
    /// No data written until the pattern is read on the bus
    GPIO_SPI_PATTERN_UNTIL,
    /// No data are written while the pattern is read on the bus
    GPIO_SPI_PATTERN_WHILE
} GPIO_SPI_PATTERN_MODE_T;

// =============================================================================
// GPIO_SPI_INFINITE_TX_T
// -----------------------------------------------------------------------------
/// That type describes the characteristics and configuration of an infinite
/// transfer on a SPI CS. That is typically used for devices like MMC which use
/// the pattern mode read feature. (The infinite transfer provides the clock
/// coping with the trashing reads before the pattern is finally seen).
/// When the pattern mode is enabled, data received on the SPI bus will only
/// be put in the Rx FIFO after a given pattern has been received.
/// The SPI will continue transfering data (zeros or ones, depending on the
/// configuration of sendOne) after the Tx fifo is empty (i.e. the IFC DMA
/// is finished) and until the Rx transfer is completed (i.e. the pattern
/// has been received and the Rx transfer size is reached).
/// After you start the inifinite write mode, you have to use the usual
/// send and get data functions.
/// The infinite transfert can be stopped by two ways:
/// - manually, by a call to #hal_SpiStopInfiniteWrite;
/// - automatically (prefered behaviour): the infinite transfer is stopped
/// by the RxDmaDone IRQ generated by the end of the reception done parallelly,
/// if the Rx mode is configured as #HAL_SPI_DMA_POLLING or #HAL_SPI_DMA_IRQ.
// =============================================================================
typedef struct
{
    /// \c TRUE if the infinite transfer is done by sending ones,
    /// \c FALSE if it is done by sending zeroes.
    BOOL sendOne;

    /// \c TRUE enable the auto-stopping feature of the infinite transfer.
    BOOL autoStop;

    /// Described the pattern mode. That is the read buffer
    /// will only be filled with data received after a given pattern,
    /// specified by the following field, is seen on the line,
    /// or after the pattern is no more read on it (while the pattern
    /// is seen, no data is recorded).
    GPIO_SPI_PATTERN_MODE_T patternMode;

    /// Value of the pattern waited before useful data are received.
    UINT32 pattern;
} GPIO_SPI_INFINITE_TX_T;

// =============================================================================
// GPIO_SPI_PIN_T
// -----------------------------------------------------------------------------
/// SPI Pin
///
/// Used to specify an SPI pin.
// =============================================================================
typedef enum
{
    /// Chip select 0 pin
    GPIO_SPI_PIN_CS0 = 0,
    /// Chip select 1 pin
    GPIO_SPI_PIN_CS1,
    /// Chip select 2 pin, if available
    GPIO_SPI_PIN_CS2,
    /// Chip select 3 pin, if available
    GPIO_SPI_PIN_CS3,
    /// Clock pin
    GPIO_SPI_PIN_CLK,
    /// Data out pin
    GPIO_SPI_PIN_DO,
    /// Data in pin
    GPIO_SPI_PIN_DI,
    GPIO_SPI_PIN_LIMIT
} GPIO_SPI_PIN_T;

// =============================================================================
// GPIO_SPI_PIN_STATE_T
// -----------------------------------------------------------------------------
/// SPI Pin State
///
/// Used to specify the state of an SPI pin. It can be
/// zero, one or Z (high impedence, tri-state).
// =============================================================================
typedef enum
{
    GPIO_SPI_PIN_STATE_SPI   =   0,
    GPIO_SPI_PIN_STATE_Z     =   1,
    GPIO_SPI_PIN_STATE_0     =   2,
    GPIO_SPI_PIN_STATE_1     =   3
} GPIO_SPI_PIN_STATE_T;



// =============================================================================
// gpio_SpiOpen
// -----------------------------------------------------------------------------
/// Open a SPI CS driver.
/// This function enables the SPI in the mode selected by \c spiCfg.
/// <B> The polarity of both the Chip Select must be set in the configuration
/// structure of HAL in the tgt_BoardConfig structure. The knowledge about those
/// two polarities
/// is needed by the driver, and it cannot operate without them. </B>
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum Chip Select for which this configuration applies.
/// @param spiCfg The configuration for SPI
// =============================================================================
PUBLIC VOID gpio_SpiOpen(GPIO_SPI_CS_T csNum, CONST GPIO_SPI_CFG_T* spiCfg);



// =============================================================================
// gpio_SpiActivateCs
// -----------------------------------------------------------------------------
/// Activate a Chip Select previously opened by a call to #hal_SpiOpen. Once
/// the CS is no more used, it must be deactivated by a call to
/// #hal_SpiDeActivateCs.
///
/// This is a non blocking function, the return value *MUST* be checked
///
/// If an attempt is made to activate a previously opened CS, two case can
/// happen:
/// - the SPI bus is free: the CS will be activated immediatly and
/// the function returns TRUE.
/// - another CS is activated: the new activation is not done and
/// the function returns FALSE, the calling task must retry later.
///
/// The hal_SpiActivateCs function will always exit immediately.
///
/// A CS must be activated before being used, and only one CS at a time can be
/// activated.
///
/// This function requests a resource corresponding to the needed frequency.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS to activate.
/// @return \c TRUE if the cs has been successfully activated
/// \c FALSE when another cs is already active
// =============================================================================
PUBLIC BOOL gpio_SpiActivateCs(GPIO_SPI_CS_T csNum);




// =============================================================================
// hal_SpiDeActivateCs
// -----------------------------------------------------------------------------
/// Deactivate a Chip Select. This is the opposite operation to #hal_SpiActivateCs.
/// It must be called after #hal_SpiActivateCs to allow the activation of another
/// CS. The deactivation of a non-activated CS will trig an ASSERT.
///
/// This function release the resource to #GPIO_SYS_FREQ_32K.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS to deactivate.
// =============================================================================
PUBLIC VOID gpio_SpiDeActivateCs( GPIO_SPI_CS_T csNum);

PUBLIC VOID gpio_SpiClose( GPIO_SPI_CS_T csNum);

// -----------------------------------------------------------------------------
/// Send a bunch of data.
/// This functions sends \c length bytes starting from the address \c
/// start_address. The number returned is the number of bytes actually sent.
/// In DMA mode, this function returns 0 when no DMA channel is available, it
/// returns length otherwise. This function is not to be used for the infinite
/// mode. Use instead the dedicated driver function #hal_SpiStartInfiniteWrite.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum The CS to use to send the data. This cs must be activated before
/// sending data.
/// @param startAddress Pointer on the buffer to send
/// @param length number of bytes to send (Up to 4 kB) .
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of sent bytes.
// =============================================================================
PUBLIC UINT32 gpio_SpiSendData(  GPIO_SPI_CS_T csNum,  CONST UINT8* startAddress, UINT32 length);
// =============================================================================
// gpio_SpiGetData
// -----------------------------------------------------------------------------
/// Get a bunch of data.
///
/// This functions gets \c length bytes from the SPI and stores them starting
/// from the address \c dest_address. The number returned is the number of bytes
/// actually received. In DMA mode, this function returns 0 when no DMA channel
/// is available. It returns length otherwise.
/// A pattern mode is available.
/// When enabled, the actual reception and count of received bytes starts
/// only after a precised pattern has been read on the SPI. This pattern is not
/// copied into the reception buffer.
/// This feature is only enabled by using #hal_SpiStartInfiniteWrite.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  csNum   Chip select
/// @param destAddress Pointer on the buffer to store received data
/// @param length Number of byte to receive.
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of received bytes.
// =============================================================================
PUBLIC UINT32 gpio_SpiGetData( GPIO_SPI_CS_T csNum,  UINT8*      destAddress,     UINT32      length   );



BOOL gpio_spi_open(VOID);
BOOL gpio_spi_close(VOID);



#endif




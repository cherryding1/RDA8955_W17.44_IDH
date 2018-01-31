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
#include "spigpio_config.h"
#include "tgt_gpiospi_cfg.h"
#include "spigpio_debug.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "sxr_tim.h"
#include "sxr_tls.h"
//#include "cmn_defs.h"
#include "gpio_spi.h"




// =============================================================================
// HAL_SPI_ACTIVE_T
// -----------------------------------------------------------------------------
/// Type used to remember the activation state of the SPI.
// =============================================================================
typedef enum
{
    /// Active CS0 to CS3 are the first one to
    /// keep compatibility with the HAL_SPI_CS_T type.
    GPIO_SPI_ACTIVE_CS_0,
    GPIO_SPI_ACTIVE_CS_1,
    GPIO_SPI_ACTIVE_CS_2,
    GPIO_SPI_ACTIVE_CS_3,
    GPIO_SPI_ACTIVE_NONE,
    /// To avoid race and several things done simultaneously.
    GPIO_SPI_OPERATION_IN_PROGRESS,

    GPIO_SPI_ACTIVE_QTY
} GPIO_SPI_ACTIVE_T;

//CONST TGT_GPIOSPI_CONFIG_T*  g_gspigpioConfig;
PRIVATE UINT16 g_gpioSpiCsActive[GPIO_SPI_CS_QTY];
PRIVATE UINT16 g_gpioSpiCsOpen[GPIO_SPI_CS_QTY];


struct GPIO_SPI_CONFIG_STRUCT_T* g_gspigpioConfig;
PRIVATE GPIO_SPI_CFG_T* g_spiConfigCs[2];


// gpio_SpiOpen
// -----------------------------------------------------------------------------
/// Open a SPI CS driver.
/// This function enables the SPI in the mode selected by \c spiCfg.
/// <B> The polarity of both the Chip Select must be set in the configuration
/// structure tgt_BoardConfig.halCfg. The knowledge about those two polarities
/// is needed by the driver, and it cannot operate without them. </B>
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum Chip Select for which this configuration applies.
/// @param spiCfg The configuration for SPI scl_spis
// =============================================================================
PRIVATE BOOL g_output = FALSE;
PRIVATE UINT32 g_halClkCycle;

PUBLIC VOID gpio_SpiOpen(GPIO_SPI_CS_T csNum, CONST GPIO_SPI_CFG_T* spiCfg)
{
    GSPI_ASSERT(csNum < 2, "Improper SPI cs! \n");
    //  Checkers
    GSPI_ASSERT((UINT32)spiCfg->enabledCS < GPIO_SPI_CS_QTY,"Bad SPI CS number %d", spiCfg->enabledCS);
    GSPI_ASSERT((UINT32)spiCfg->clkDelay < GPIO_SPI_HALF_CLK_PERIOD_3,"Improper SPI clkDelay %d", spiCfg->clkDelay);
    GSPI_ASSERT((UINT32)spiCfg->doDelay < GPIO_SPI_HALF_CLK_PERIOD_3,"Improper SPI doDelay %d", spiCfg->doDelay);
    GSPI_ASSERT((UINT32)spiCfg->diDelay < GPIO_SPI_HALF_CLK_PERIOD_QTY,"Improper SPI diDelay %d", spiCfg->diDelay);
    GSPI_ASSERT((UINT32)spiCfg->csDelay < GPIO_SPI_HALF_CLK_PERIOD_QTY, "Improper SPI csDelay %d", spiCfg->csDelay);
    GSPI_ASSERT((UINT32)spiCfg->csPulse < GPIO_SPI_HALF_CLK_PERIOD_QTY,"Improper SPI csPulse %d", spiCfg->csPulse);
    GSPI_ASSERT(spiCfg->spiFreq != 0,"Improper SPI Tx mode  " );

    // Check unsupported mode
    GSPI_ASSERT(!((spiCfg->diDelay == 3) && (spiCfg->csDelay == 0)&& (spiCfg->doDelay == 0)), "Unsupported SPI timing configuration:DI=3, CS=0 and, DO=0 ");

    // Open without any direction enabled

    // This CS has already be opened
    GSPI_ASSERT(g_gpioSpiCsOpen[csNum] == 0, "CS already opened");

    GSPI_ASSERT (csNum == spiCfg->enabledCS, "Incoherency between CS and configuration for SPI");
    g_spiConfigCs[csNum] = spiCfg;
    g_gpioSpiCsOpen[csNum] = 1;
    g_halClkCycle = g_spiConfigCs[csNum]->spiFreq;
    if(g_output == FALSE)
    {
        if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_IO)
        {
            hal_GpioSetOut(g_gspigpioConfig->scl_spi.gpioId);
        }
        if(spiCfg->csActiveLow == TRUE)
        {
            hal_GpioSet(g_gspigpioConfig->scl_spi);
        }
        else
        {
            hal_GpioClr(g_gspigpioConfig->scl_spi);
        }
        g_output = TRUE;
    }
}




// =============================================================================
// hal_SpiActivateCs
// -----------------------------------------------------------------------------

PUBLIC BOOL gpio_SpiActivateCs(GPIO_SPI_CS_T csNum)
{

    UINT32 scStatus;
    GSPI_ASSERT(csNum < 2, "Improper SPI id   ! \n");
    // This CS has not been opened
    GSPI_ASSERT(g_gpioSpiCsOpen[csNum] !=0, "CS %d not opened", csNum);
    if( csNum == GPIO_SPI_CS0)
    {
        if ( g_gpioSpiCsActive[GPIO_SPI_CS1] == 1)  return FALSE;// the other CS is active
    }
    else if( csNum == GPIO_SPI_CS1)
    {
        if ( g_gpioSpiCsActive[GPIO_SPI_CS0] == 1)  return FALSE;// the other CS is active
    }

    if ( g_gpioSpiCsActive[csNum] == 1)  return TRUE; // this CS is already active

    scStatus = hal_SysEnterCriticalSection();
    g_gpioSpiCsActive[csNum] = 1;
    hal_SysExitCriticalSection(scStatus);
    return TRUE;

}

PUBLIC VOID gpio_SpiDeActivateCs(GPIO_SPI_CS_T csNum)
{
    UINT32 scStatus;

    GSPI_ASSERT(csNum < 2, "Improper SPI id %d ! \n" );

    scStatus = hal_SysEnterCriticalSection();
    // test and set
    g_gpioSpiCsActive[csNum] = 0;
    hal_SysExitCriticalSection(scStatus);
}

PUBLIC VOID gpio_SpiClose(GPIO_SPI_CS_T csNum)
{
    g_gpioSpiCsOpen[csNum] = 0;
}



// =============================================================================
//
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
/// @param length number of bytes to send (Up to 4 kB).
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of sent bytes.
// =============================================================================


#define spi_delay(delay) \
{ \
  register UINT32 count_delay = 0; \
  while(count_delay<delay){\
  asm volatile("nop"); \
  asm volatile("nop"); \
  asm volatile("nop"); \
  count_delay++; \
  };\
}

static inline VOID gpio_set_level(HAL_APO_ID_T apo,UINT8 level)
{
    if(level == 1)
        hal_GpioSet(apo);
    else
        hal_GpioClr(apo);
}
static inline VOID disable_cs_gpio(GPIO_SPI_CS_T csNum)
{
    if (g_spiConfigCs[csNum]->csActiveLow == TRUE)
    {
        hal_GpioClr(g_gspigpioConfig->scl_spi);
    }
    else
    {
        hal_GpioSet(g_gspigpioConfig->scl_spi);
    }

}
static inline VOID enable_cs_gpio(GPIO_SPI_CS_T csNum)
{
    if (g_spiConfigCs[csNum]->csActiveLow == TRUE)
    {
        hal_GpioSet(g_gspigpioConfig->scl_spi);
    }
    else
    {
        hal_GpioClr(g_gspigpioConfig->scl_spi);
    }

}

#if 1
PUBLIC UINT32 gpio_SpiSendData(GPIO_SPI_CS_T csNum,CONST UINT8* startAddress,UINT32 length)
{

    UINT8 i ,k,data;
    UINT8 mask[]= {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
    BOOL clk_leve = FALSE;
    GSPI_ASSERT(csNum < 2, "Improper SPI id %d ! \n");
    //  Check the address pointer.
    GSPI_ASSERT((startAddress != NULL), "SPI Tx with a NULL start address");
    GSPI_ASSERT((g_gpioSpiCsActive[csNum] == 1), "not actived");

    if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_IO)
    {
        hal_GpioSetOut(g_gspigpioConfig->sdo_spi.gpioId);
    }


    ///  the first edge after the CS activation is a falling edge.
    if (g_spiConfigCs[csNum]->clkFallEdge == FALSE)
    {
        clk_leve = TRUE;
    }

    data = startAddress[0];
    enable_cs_gpio(csNum);
    if(g_spiConfigCs[csNum]->clkDelay == g_spiConfigCs[csNum]->doDelay)
    {
        spi_delay(g_halClkCycle*(UINT16)(g_spiConfigCs[csNum]->clkDelay));
        for(i=7; i>=0; i--)
        {
            gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
            gpio_set_level(g_gspigpioConfig->sdo_spi,(data&mask[i])>>i);
            spi_delay(g_halClkCycle);
            gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
            spi_delay(g_halClkCycle);
        }
        for(k = 1; k<length; k++)
        {
            data = startAddress[k];
            for(i=7; i>=0; i--)
            {
                gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
                gpio_set_level(g_gspigpioConfig->sdo_spi,(data&mask[i])>>i);
                spi_delay(g_halClkCycle);
                gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
                spi_delay(g_halClkCycle);
            }
        }


    }
    else if(g_spiConfigCs[csNum]->clkDelay > g_spiConfigCs[csNum]->doDelay)
    {
        spi_delay(g_halClkCycle*(UINT16)(g_spiConfigCs[csNum]->doDelay));
        gpio_set_level(g_gspigpioConfig->sdo_spi,(data&mask[7])>>7);
        spi_delay(g_halClkCycle*((UINT16)(g_spiConfigCs[csNum]->clkDelay)-(UINT16)(g_spiConfigCs[csNum]->doDelay)));
        gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
        spi_delay(g_halClkCycle);
        gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
        for(i=6; i>=0; i--)
        {
            spi_delay(g_halClkCycle);
            gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
            gpio_set_level(g_gspigpioConfig->sdo_spi,(data&mask[i])>>i);
            spi_delay(g_halClkCycle);
            gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);

        }
        for(k = 1; k<length; k++)
        {
            data = startAddress[k];
            for(i=7; i>=0; i--)
            {
                spi_delay(g_halClkCycle);
                gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
                gpio_set_level(g_gspigpioConfig->sdo_spi,(data&mask[i])>>i);
                spi_delay(g_halClkCycle);
                gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);

            }
        }


    }
    else
    {
        GSPI_ASSERT(FALSE, "no support time");
    }

    spi_delay(g_halClkCycle*(UINT16)(g_spiConfigCs[csNum]->csDelay));


    disable_cs_gpio(csNum);
    return length;
}
#endif
// =============================================================================
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
/// @param  cs   Chip select
/// @param dest_address Pointer on the buffer to store received data
/// @param length Number of byte to receive.

// =============================================================================
PUBLIC UINT32 gpio_SpiGetData(GPIO_SPI_CS_T csNum,UINT8* destAddress,UINT32 length)
{
    UINT32 i,data,k;
    BOOL clk_leve = FALSE;

    GSPI_ASSERT(csNum < 2, "Improper SPI cs ! \n");
    // Check the address pointer.
    GSPI_ASSERT((destAddress != NULL) ,"NULL destination for SPI Rx");

    // Check if this CS has been activated
    GSPI_ASSERT((g_gpioSpiCsActive[csNum] == 1), "not actived");

    if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_IO)
    {
        hal_GpioSetIn(g_gspigpioConfig->sdi_spi.gpioId);
    }

    ///  the first edge after the CS activation is a falling edge.
    if (g_spiConfigCs[csNum]->clkFallEdge == FALSE)
    {
        clk_leve = TRUE;
    }
    {

        i = 7; data =0;
        enable_cs_gpio(csNum);
        if(g_spiConfigCs[csNum]->clkDelay == g_spiConfigCs[csNum]->doDelay)
        {
            spi_delay(g_halClkCycle*(UINT16)(g_spiConfigCs[csNum]->clkDelay));
            for(i=7; i>=0; i--)
            {
                gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
                spi_delay(g_halClkCycle);
                gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
                data = data << 1;
                if(hal_GpioGet(g_gspigpioConfig->sdi_spi.gpioId) == 1)  data |= 1;
                spi_delay(g_halClkCycle);
            }
            destAddress[0] = data;
            for(k = 1; k<length; k++)
            {
                for(i=7; i>=0; i--)
                {
                    gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
                    spi_delay(g_halClkCycle);
                    gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
                    data = data << 1;
                    if(hal_GpioGet(g_gspigpioConfig->sdi_spi.gpioId) == 1)  data |= 1;
                    spi_delay(g_halClkCycle);
                }
                destAddress[k] = data;
            }


        }
        else if(g_spiConfigCs[csNum]->clkDelay > g_spiConfigCs[csNum]->doDelay)
        {
            spi_delay(g_halClkCycle*(UINT16)(g_spiConfigCs[csNum]->doDelay));
            spi_delay(g_halClkCycle*((UINT16)(g_spiConfigCs[csNum]->clkDelay)-(UINT16)(g_spiConfigCs[csNum]->doDelay)));
            gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
            spi_delay(g_halClkCycle);
            gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
            data = data << 1;
            if(hal_GpioGet(g_gspigpioConfig->sdi_spi.gpioId) == 1)  data |= 1;
            for(i=6; i>=0; i--)
            {
                spi_delay(g_halClkCycle);
                gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
                spi_delay(g_halClkCycle);
                gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
                data = data << 1;
                if(hal_GpioGet(g_gspigpioConfig->sdi_spi.gpioId) == 1)  data |= 1;


            }
            destAddress[0] = data;
            for(k = 1; k<length; k++)
            {

                for(i=7; i>=0; i--)
                {
                    spi_delay(g_halClkCycle);
                    gpio_set_level(g_gspigpioConfig->scl_spi,clk_leve);
                    spi_delay(g_halClkCycle);
                    gpio_set_level(g_gspigpioConfig->scl_spi,!clk_leve);
                    data = data << 1;
                    if(hal_GpioGet(g_gspigpioConfig->sdi_spi.gpioId) == 1)  data |= 1;

                }
                destAddress[k] = data;
            }


        }
        else
        {
            GSPI_ASSERT(FALSE, "no support time");
        }

        spi_delay(g_halClkCycle*(UINT16)(g_spiConfigCs[csNum]->csDelay));

    }
    disable_cs_gpio(csNum);
    return length;




}




// =============================================================================
// gpio_SpiForcePin
// -----------------------------------------------------------------------------
/// Force an SPI pin to a certain state or release the force mode
/// and put the pin back to normal SPI mode.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
// =============================================================================
// 将gpio设置为input，此时为高阻。
// set the gpio to input mode,in will high Z mode.
PUBLIC VOID gpio_SpiForcePin(GPIO_SPI_ID_T id, GPIO_SPI_PIN_T pin,  GPIO_SPI_PIN_STATE_T state)
{
    // Parameter check.
    GSPI_ASSERT(id < GPIO_SPI_QTY, "Improper SPI id %d ! \n"
                " Your system only has %d SPI", id, GPIO_SPI_QTY);

    GSPI_ASSERT(pin >= GPIO_SPI_PIN_CS0,"Invalid SPI pin %d (negative).", pin);
    GSPI_ASSERT(pin < GPIO_SPI_PIN_LIMIT,  "Invalid SPI pin %d (too big).", pin);

    // Apply the state to the pin
    switch (pin)
    {
        case GPIO_SPI_PIN_CS0  :
            switch (state)
            {
                case GPIO_SPI_PIN_STATE_SPI:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scs_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_Z:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetIn(g_gspigpioConfig->scs_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_0:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scs_spi.gpioId);
                        hal_GpioClr(g_gspigpioConfig->scs_spi);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioClr(g_gspigpioConfig->scs_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_1:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scs_spi.gpioId);
                        hal_GpioSet(g_gspigpioConfig->scs_spi);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioSet(g_gspigpioConfig->scs_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;

            }
            break;

        case GPIO_SPI_PIN_CS1  :
            switch (state)
            {
                case GPIO_SPI_PIN_STATE_SPI:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scs_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_Z:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetIn(g_gspigpioConfig->scs_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");
                    }

                    break;
                case GPIO_SPI_PIN_STATE_0:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scs_spi.gpioId);
                        hal_GpioClr(g_gspigpioConfig->scs_spi);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioClr(g_gspigpioConfig->scs_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_1:
                    if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scs_spi.gpioId);
                        hal_GpioSet(g_gspigpioConfig->scs_spi);
                    }
                    else if(g_gspigpioConfig->scs_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioSet(g_gspigpioConfig->scs_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;

            }
            break;

        case GPIO_SPI_PIN_CLK  :
            switch (state)
            {
                case GPIO_SPI_PIN_STATE_SPI:
                    if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scl_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_Z:
                    if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetIn(g_gspigpioConfig->scs_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_0:
                    if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scl_spi.gpioId);
                        hal_GpioClr(g_gspigpioConfig->scl_spi);
                    }
                    else if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioClr(g_gspigpioConfig->scl_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_1:
                    if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->scl_spi.gpioId);
                        hal_GpioSet(g_gspigpioConfig->scl_spi);
                    }
                    else if(g_gspigpioConfig->scl_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioSet(g_gspigpioConfig->scl_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;

            }
            break;

        case GPIO_SPI_PIN_DO   :
            switch (state)
            {
                case GPIO_SPI_PIN_STATE_SPI:
                    if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->sdo_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_Z:
                    if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetIn(g_gspigpioConfig->sdo_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_0:
                    if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->sdo_spi.gpioId);
                        hal_GpioClr(g_gspigpioConfig->sdo_spi);
                    }
                    else if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioClr(g_gspigpioConfig->sdo_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_1:
                    if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->sdo_spi.gpioId);
                        hal_GpioSet(g_gspigpioConfig->sdo_spi);
                    }
                    else if(g_gspigpioConfig->sdo_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioSet(g_gspigpioConfig->sdo_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;

            }

            break;

        case GPIO_SPI_PIN_DI   :
            switch (state)
            {
                case GPIO_SPI_PIN_STATE_SPI:
                    if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->sdi_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_Z:
                    if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetIn(g_gspigpioConfig->sdi_spi.gpioId);
                    }
                    else if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_O)
                    {

                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_0:
                    if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->sdi_spi.gpioId);
                        hal_GpioClr(g_gspigpioConfig->sdi_spi);
                    }
                    else if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioClr(g_gspigpioConfig->sdi_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;
                case GPIO_SPI_PIN_STATE_1:
                    if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_IO)
                    {
                        hal_GpioSetOut(g_gspigpioConfig->sdi_spi.gpioId);
                        hal_GpioSet(g_gspigpioConfig->sdi_spi);
                    }
                    else if(g_gspigpioConfig->sdi_spi.type == HAL_GPIO_TYPE_O)
                    {
                        hal_GpioSet(g_gspigpioConfig->sdi_spi);
                    }
                    else
                    {
                        GSPI_ASSERT(FALSE, "error cs0!");

                    }

                    break;

            }
            break;

        default:
            // There already a check, we shouldn't arrive there
            GSPI_ASSERT(FALSE, "What the heck is this SPI pin %d your trying to force to %d", pin,   state);
            break;
    }
}







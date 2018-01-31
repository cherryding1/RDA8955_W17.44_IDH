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
#include "cfg_regs.h"

#include "gpio.h"


#include "hal_spi.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_config.h"
#include "hal_gpio.h"

#include "halp_config.h"
#include "halp_sys_ifc.h"
#include "halp_debug.h"
#include "halp_spi.h"
#include "halp_ispi.h"
#include "halp_sys.h"


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_SPI_ACTIVE_T
// -----------------------------------------------------------------------------
/// Type used to remember the activation state of the SPI.
// =============================================================================
typedef enum
{
    /// Active CS0 to CS3 are the first one to
    /// keep compatibility with the HAL_SPI_CS_T type.
    HAL_SPI_ACTIVE_CS_0,
    HAL_SPI_ACTIVE_CS_1,
    HAL_SPI_ACTIVE_CS_2,
    HAL_SPI_ACTIVE_CS_3,
    HAL_SPI_ACTIVE_NONE,
    /// To avoid race and several things done simultaneously.
    HAL_SPI_OPERATION_IN_PROGRESS,

    HAL_SPI_ACTIVE_QTY
} HAL_SPI_ACTIVE_T;

// =============================================================================
// HAL_SPI_SETTINGS_T
// -----------------------------------------------------------------------------
/// Type used to remember the SPI configuration for each Chip Select.
// =============================================================================
typedef VOLATILE struct
{
    UINT32       ctrl;
    UINT32       cfg;
    HAL_SPI_TRANSFERT_MODE_T rxMode; ///< Reception transfer mode
    HAL_SPI_TRANSFERT_MODE_T txMode; ///< Emission transfer mode
    /// Desired freq
    UINT32      freq;
    /// Number of bytes per frame. Used to check that we only
    /// send complete frames.
    UINT32      bytesPerFrame;
    HAL_SPI_IRQ_HANDLER_T irqHandler;
    /// Image of the irq mask register
    UINT32      irqMask;
} HAL_SPI_SETTINGS_T;


// =============================================================================
// HAL_SPI_PROP_T
// -----------------------------------------------------------------------------
/// This type is used to remember of all the properties of each SPI present in
/// the system, and its driver status.
// =============================================================================
typedef struct
{
    /// Configuration per CS
    HAL_SPI_SETTINGS_T          cfg[HAL_SPI_CS_MAX_QTY];
    /// Control Register picture
    UINT32                      ctrl;
    VOLATILE UINT8              rxIfcCh;
    VOLATILE UINT8              txIfcCh;
    HAL_SPI_TRANSFERT_MODE_T    rxMode;
    HAL_SPI_TRANSFERT_MODE_T    txMode;
    // The SPI availability is represented by the activeStatus variable.
    // A CS Activation must use a critical section to check and set
    // (if possible) the activeStatus and thus become activated.
    // The Deactivation mechanism relies on the same mechanism.
    HAL_SPI_ACTIVE_T            activeStatus;
    // Used to store the user irq handler
    HAL_SPI_IRQ_HANDLER_T       irqHandler;
    // Used to store required frequency
    HAL_SYS_FREQ_T              requiredSysFreq;
} HAL_SPI_PROP_T;



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Global variable holding the state of each SPI present in the system,
/// and holding the configuration of every one of its (opened) CS.
PRIVATE HAL_SPI_PROP_T g_halSpiPropArray[HAL_SPI_QTY]
    =
#if (CHIP_SPI2_CS_QTY == 0)
        // There is only one SPI
{
    {
        .cfg =
        {
            {
                .ctrl           = 0,
                .cfg            = 0,
                .rxMode         = 0,
                .txMode         = 0,
                .freq           = 0,
                .bytesPerFrame  = 0,
                .irqHandler     = NULL,
                .irqMask        = 0
            },
            {
                .ctrl           = 0,
                .cfg            = 0,
                .rxMode         = 0,
                .txMode         = 0,
                .freq           = 0,
                .bytesPerFrame  = 0,
                .irqHandler     = NULL,
                .irqMask        = 0
            },
            {
                .ctrl           = 0,
                .cfg            = 0,
                .rxMode         = 0,
                .txMode         = 0,
                .freq           = 0,
                .bytesPerFrame  = 0,
                .irqHandler     = NULL,
                .irqMask        = 0

            },
            {
                .ctrl           = 0,
                .cfg            = 0,
                .rxMode         = 0,
                .txMode         = 0,
                .freq           = 0,
                .bytesPerFrame  = 0,
                .irqHandler     = NULL,
                .irqMask        = 0

            }
        },
        .ctrl               = 0,
        .rxIfcCh            = HAL_UNKNOWN_CHANNEL,
        .txIfcCh            = HAL_UNKNOWN_CHANNEL,
        .rxMode             = HAL_SPI_OFF,
        .txMode             = HAL_SPI_OFF,
        .activeStatus       = HAL_SPI_ACTIVE_NONE,
        .irqHandler         = NULL
    }
};
#else
        // There are two SPIs (or more, but that is
        // another story)
        {
            {
                .cfg =
{
    {
        .ctrl           = 0,
        .cfg            = 0,
        .rxMode         = 0,
        .txMode         = 0,
        .freq           = 0,
        .bytesPerFrame  = 0,
        .irqHandler     = NULL,
        .irqMask        = 0

    },
    {
        .ctrl           = 0,
        .cfg            = 0,
        .rxMode         = 0,
        .txMode         = 0,
        .freq           = 0,
        .bytesPerFrame  = 0,
        .irqHandler     = NULL,
        .irqMask        = 0

    },
    {
        .ctrl           = 0,
        .cfg            = 0,
        .rxMode         = 0,
        .txMode         = 0,
        .freq           = 0,
        .bytesPerFrame  = 0,
        .irqHandler     = NULL,
        .irqMask        = 0

    },
    {
        .ctrl           = 0,
        .cfg            = 0,
        .rxMode         = 0,
        .txMode         = 0,
        .freq           = 0,
        .bytesPerFrame  = 0,
        .irqHandler     = NULL,
        .irqMask        = 0

    }
},
.ctrl               = 0,
 .rxIfcCh            = HAL_UNKNOWN_CHANNEL,
 .txIfcCh            = HAL_UNKNOWN_CHANNEL,
 .rxMode             = HAL_SPI_OFF,
 .txMode             = HAL_SPI_OFF,
 .activeStatus       = HAL_SPI_ACTIVE_NONE,
 .irqHandler         = NULL
            },
{
    .cfg =
    {
        {
            .ctrl           = 0,
            .cfg            = 0,
            .rxMode         = 0,
            .txMode         = 0,
            .freq           = 0,
            .bytesPerFrame  = 0,
            .irqHandler     = NULL,
            .irqMask        = 0

        },
        {
            .ctrl           = 0,
            .cfg            = 0,
            .rxMode         = 0,
            .txMode         = 0,
            .freq           = 0,
            .bytesPerFrame  = 0,
            .irqHandler     = NULL,
            .irqMask        = 0

        },
        {
            .ctrl           = 0,
            .cfg            = 0,
            .rxMode         = 0,
            .txMode         = 0,
            .freq           = 0,
            .bytesPerFrame  = 0,
            .irqHandler     = NULL,
            .irqMask        = 0

        },
        {
            .ctrl           = 0,
            .cfg            = 0,
            .rxMode         = 0,
            .txMode         = 0,
            .freq           = 0,
            .bytesPerFrame  = 0,
            .irqHandler     = NULL,
            .irqMask        = 0

        }
    },
    .ctrl               = 0,
    .rxIfcCh            = HAL_UNKNOWN_CHANNEL,
    .txIfcCh            = HAL_UNKNOWN_CHANNEL,
    .rxMode             = HAL_SPI_OFF,
    .txMode             = HAL_SPI_OFF,
    .activeStatus       = HAL_SPI_ACTIVE_NONE,
    .irqHandler         = NULL
}
        };
#endif

/// Array of pointers towards the SPI HW modules
PRIVATE HWP_SPI_T* CONST gc_halSpiHwpArray[HAL_SPI_QTY] =
#if (CHIP_SPI2_CS_QTY == 0)
// Just one SPI
{hwp_spi1};
#else
// Two SPIs
#ifdef CHIP_DIE_8809
    {hwp_spi1, hwp_spi2,hwp_spi3};
#else
    {hwp_spi1, hwp_spi2};
#endif
#endif

PRIVATE CONST HAL_IFC_REQUEST_ID_T gc_halSpiIfTxId[HAL_SPI_QTY] =
{
#if (CHIP_SPI2_CS_QTY == 0)
    HAL_IFC_SPI_TX,
#else
    HAL_IFC_SPI_TX,
    HAL_IFC_SPI2_TX,
#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
    HAL_IFC_SPI3_TX,
#endif
#endif
};

PRIVATE CONST HAL_IFC_REQUEST_ID_T gc_halSpiIfRxId[HAL_SPI_QTY] =
{
#if (CHIP_SPI2_CS_QTY == 0)
    HAL_IFC_SPI_RX,
#else
    HAL_IFC_SPI_RX,
    HAL_IFC_SPI2_RX,
#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
    HAL_IFC_SPI3_RX,
#endif

#endif
};

static HAL_CLK_T *gSpi1Clk = NULL;
static HAL_CLK_T *gSpi2Clk = NULL;

// =============================================================================
//  FUNCTIONS
// ============================================================================

// =============================================================================
// hal_SpiGetCsBitField
// -----------------------------------------------------------------------------
/// Return the bitfield with the code representing the CS to use and corresponding
/// to the chip the code is running on. The check of the existence of the CS is
/// done at a upper level.
// =============================================================================
PRIVATE UINT32 hal_SpiGetCsBitField(HAL_SPI_CS_T csNum)
{
    UINT32 result=0;
    // Same bad thing, but the macros having different names
    switch (csNum)
    {
        case HAL_SPI_CS0:
            result = SPI_CS_SEL_CS0;
            break;
        case HAL_SPI_CS1:
            result = SPI_CS_SEL_CS1;
            break;
        case HAL_SPI_CS2:
            result = SPI_CS_SEL_CS2;
            break;
        case HAL_SPI_CS3:
            result = SPI_CS_SEL_CS3;
            break;
        default:
            HAL_ASSERT(FALSE, "This SPI Chip Select doesn't exist, u know ?");
    }
    return result;
}

// =============================================================================
// hal_SpiDoConfigCs
// -----------------------------------------------------------------------------
///  ** Local functions  **
///  Change SPI chips select setting
/// @param id Identifier of the SPI for which the function is called.
/// @param spiConfigPtr Pointer to the SPI CS configuration.
// =============================================================================
PRIVATE VOID hal_SpiDoConfigCs(HAL_SPI_ID_T id, CONST HAL_SPI_CFG_T* spiConfigPtr)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    CONST HAL_CFG_CONFIG_T* halCfg = g_halCfg;
    //  Checkers
    HAL_ASSERT((UINT32)spiConfigPtr->enabledCS < HAL_SPI_CS_QTY,
               "Bad SPI CS number %d", spiConfigPtr->enabledCS);
    HAL_ASSERT((UINT32)spiConfigPtr->clkDelay < HAL_SPI_HALF_CLK_PERIOD_3,
               "Improper SPI clkDelay %d", spiConfigPtr->clkDelay);
    HAL_ASSERT((UINT32)spiConfigPtr->doDelay < HAL_SPI_HALF_CLK_PERIOD_3,
               "Improper SPI doDelay %d", spiConfigPtr->doDelay);
    HAL_ASSERT((UINT32)spiConfigPtr->diDelay < HAL_SPI_HALF_CLK_PERIOD_QTY,
               "Improper SPI diDelay %d", spiConfigPtr->diDelay);
    HAL_ASSERT((UINT32)spiConfigPtr->csDelay < HAL_SPI_HALF_CLK_PERIOD_QTY,
               "Improper SPI csDelay %d", spiConfigPtr->csDelay);
    HAL_ASSERT((UINT32)spiConfigPtr->csPulse < HAL_SPI_HALF_CLK_PERIOD_QTY,
               "Improper SPI csPulse %d", spiConfigPtr->csPulse);
    HAL_ASSERT((spiConfigPtr->frameSize>=HAL_SPI_MIN_FRAME_SIZE)
               && (spiConfigPtr->frameSize<=HAL_SPI_MAX_FRAME_SIZE),
               "Improper SPI framesize", spiConfigPtr->frameSize);
    HAL_ASSERT((UINT32)spiConfigPtr->rxTrigger < HAL_SPI_RX_TRIGGER_QTY,
               "Improper SPI Rx trigger level %d", spiConfigPtr->rxTrigger);
    HAL_ASSERT((UINT32)spiConfigPtr->txTrigger < HAL_SPI_TX_TRIGGER_QTY,
               "Improper SPI Tx trigger level %d", spiConfigPtr->txTrigger);
    HAL_ASSERT((UINT32)spiConfigPtr->rxMode < HAL_SPI_TM_QTY,
               "Improper SPI Rx mode %d", spiConfigPtr->rxMode);
    HAL_ASSERT((UINT32)spiConfigPtr->txMode < HAL_SPI_TM_QTY,
               "Improper SPI Tx mode %d", spiConfigPtr->txMode);
    // HAL_ASSERT((UINT32)spiConfigPtr->oeRatio <= 31,
    //      "Improper OE Ratio :%d",spiConfigPtr->oeRatio );

    // Check unsupported mode
    HAL_ASSERT(!((spiConfigPtr->diDelay == 3)
                 && (spiConfigPtr->csDelay == 0)
                 && (spiConfigPtr->doDelay == 0)),
               "Unsupported SPI timing configuration:DI=3, CS=0 and, DO=0 ");

    // Open without any direction enabled
    HAL_ASSERT(!((spiConfigPtr->rxMode==HAL_SPI_OFF) && (spiConfigPtr->txMode==HAL_SPI_OFF)),
               "SPI opened without any direction enabled %d",0);

    // This CS has already be opened
    HAL_ASSERT(g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].cfg == 0,
               "CS already opened");

    BOOL activity = 0;
    switch (spiConfigPtr->enabledCS)
    {
        case HAL_SPI_CS0:
            activity = halCfg->spiCfg[id].cs0ActiveLow;
            break;

        case HAL_SPI_CS1:
            activity = halCfg->spiCfg[id].cs1ActiveLow;
            break;

        case HAL_SPI_CS2:
            activity = halCfg->spiCfg[id].cs2ActiveLow;
            break;

#if (CHIP_SPI_CS_QTY>3 || CHIP_SPI2_CS_QTY>3)
        case HAL_SPI_CS3:
            activity = halCfg->spiCfg[id].cs3ActiveLow;
            break;
#endif
        default:
            // Check of the value of enabledCs
            // already done in the hal_SpiOpen
            // function.
            break;
    }
#if 0
    // Check polarity against the one in the target configuration.
    HAL_ASSERT(spiConfigPtr->csActiveLow == activity,
               "Conflict between requested polarity and the one declared in board configuration"
               " for CS %d", spiConfigPtr->enabledCS);
    // Set Resource as active
#endif
    // Will be done in the activate function

    //  Setter
    g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].cfg =

        // Okay, that's non canonical about the way chip.def flags
        // should be used but, hey, the macro's name are different !

        //HAL_TRACE(_PAL | TSTDOUT, 0, "SPI_CS0 polarity: %d",halCfg->spiCfg[id].cs0ActiveLow);
        //HAL_TRACE(_PAL | TSTDOUT, 0, "SPI_CS0 polarity after forcing:
        //%d",halCfg->spiCfg[id].cs0ActiveLow);

#if !defined(CHIP_DIE_8955) && !defined(CHIP_DIE_8909)

        ((halCfg->spiCfg[id].cs0ActiveLow ? SPI_CS_POLARITY_0_ACTIVE_LOW:
          SPI_CS_POLARITY_0_ACTIVE_HIGH)
         | (halCfg->spiCfg[id].cs1ActiveLow ? SPI_CS_POLARITY_1_ACTIVE_LOW:
            SPI_CS_POLARITY_1_ACTIVE_HIGH)
         | (halCfg->spiCfg[id].cs2ActiveLow ? SPI_CS_POLARITY_2_ACTIVE_LOW:
            SPI_CS_POLARITY_2_ACTIVE_HIGH)
         | (halCfg->spiCfg[id].cs3ActiveLow? SPI_CS_POLARITY_3_ACTIVE_LOW:
            SPI_CS_POLARITY_3_ACTIVE_HIGH));
#else
        ((halCfg->spiCfg[id].cs0ActiveLow ? SPI_CS_POLARITY_0_ACTIVE_LOW:
          SPI_CS_POLARITY_0_ACTIVE_HIGH)
         | (halCfg->spiCfg[id].cs1ActiveLow ? SPI_CS_POLARITY_1_ACTIVE_LOW:
            SPI_CS_POLARITY_1_ACTIVE_HIGH)
         | (halCfg->spiCfg[id].cs2ActiveLow ? SPI_CS_POLARITY_2_ACTIVE_LOW:
            SPI_CS_POLARITY_2_ACTIVE_HIGH));
#endif


    // the divider is more tweaky... as it depend on the actual system
    // frequency... It is thus done at the "real" activation.

    // set spi mask
    union
    {
        UINT32 reg;
        HAL_SPI_IRQ_STATUS_T bitfield;
    } u;
    if (id == HAL_SPI_2 || spiConfigPtr->oeRatio == 0xff)
    {
        u.bitfield = spiConfigPtr->mask;
        // for granite SPI_INPUT_SEL must be '0' to fix
        if(spiConfigPtr->inputSel == TRUE)
        {

            g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].ctrl =
                (
                    SPI_ENABLE
                    | hal_SpiGetCsBitField(spiConfigPtr->enabledCS)
                    | (spiConfigPtr->inputEn?SPI_INPUT_MODE:0)
                    | (spiConfigPtr->clkFallEdge?SPI_CLOCK_POLARITY:0)
                    | SPI_CLOCK_DELAY(spiConfigPtr->clkDelay )
                    | SPI_DO_DELAY(spiConfigPtr->doDelay )
                    | SPI_DI_DELAY(spiConfigPtr->diDelay )
                    | SPI_CS_DELAY(spiConfigPtr->csDelay )
                    | SPI_CS_PULSE(spiConfigPtr->csPulse )
                    | SPI_FRAME_SIZE(spiConfigPtr->frameSize - 1 )
                    // TODO:quite dirty, see if we need to change API
                    | ((spiConfigPtr->inputSel == 1)?SPI_INPUT_SEL(1):0)
                    | SPI_OE_DELAY((spiConfigPtr->oeRatio==0xff) ?
                                   0 : spiConfigPtr->oeRatio)
                );
        }
        else
        {
            g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].ctrl =
                (
                    SPI_ENABLE
                    | hal_SpiGetCsBitField(spiConfigPtr->enabledCS)
                    | (spiConfigPtr->inputEn?SPI_INPUT_MODE:0)
                    | (spiConfigPtr->clkFallEdge?SPI_CLOCK_POLARITY:0)
                    | SPI_CLOCK_DELAY(spiConfigPtr->clkDelay )
                    | SPI_DO_DELAY(spiConfigPtr->doDelay )
                    | SPI_DI_DELAY(spiConfigPtr->diDelay )
                    | SPI_CS_DELAY(spiConfigPtr->csDelay )
                    | SPI_CS_PULSE(spiConfigPtr->csPulse )
                    | SPI_FRAME_SIZE(spiConfigPtr->frameSize - 1 )
                    // TODO:quite dirty, see if we need to change API
                    | ((spiConfigPtr->oeRatio == 1)?SPI_INPUT_SEL(1):0)
                    | SPI_OE_DELAY((spiConfigPtr->oeRatio==0xff) ?
                                   0 : spiConfigPtr->oeRatio)
                );
        }
        // This is done in another register
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].irqMask = ( 0
                | SPI_RX_THRESHOLD(spiConfigPtr->rxTrigger )
                | SPI_TX_THRESHOLD(spiConfigPtr->txTrigger )
                | SPI_IRQ_MASK(u.reg));

        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].bytesPerFrame = (spiConfigPtr->frameSize + 7)/8;

        //  ------------------------------
        //  Transfert mode selection.
        //  ------------------------------
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].rxMode = spiConfigPtr->rxMode;
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].txMode = spiConfigPtr->txMode;
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].irqHandler = spiConfigPtr->handler;

        g_halSpiPropArray[id].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        g_halSpiPropArray[id].txIfcCh = HAL_UNKNOWN_CHANNEL;

        // Register asked frequency
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].freq = spiConfigPtr->spiFreq;
    }
    else
    {
        u.bitfield = spiConfigPtr->mask;
        // for granite SPI_INPUT_SEL must be '0' to fix
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].ctrl =
            (
                SPI_ENABLE
                | hal_SpiGetCsBitField(spiConfigPtr->enabledCS)
                | (spiConfigPtr->inputEn?SPI_INPUT_MODE:0)
                | (spiConfigPtr->clkFallEdge?SPI_CLOCK_POLARITY:0)
                | SPI_CLOCK_DELAY(spiConfigPtr->clkDelay )
                | SPI_DO_DELAY(spiConfigPtr->doDelay )
                | SPI_DI_DELAY(spiConfigPtr->diDelay )
                | SPI_CS_DELAY(spiConfigPtr->csDelay )
                | SPI_CS_PULSE(spiConfigPtr->csPulse )
                | SPI_FRAME_SIZE(spiConfigPtr->frameSize - 1 )
                // TODO:quite dirty, see if we need to change API
                | ((spiConfigPtr->oeRatio == 0)?SPI_INPUT_SEL(1):0)
                | SPI_OE_DELAY(spiConfigPtr->oeRatio)
            );

        // This is done in another register
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].irqMask = ( 0
                | SPI_RX_THRESHOLD(spiConfigPtr->rxTrigger )
                | SPI_TX_THRESHOLD(spiConfigPtr->txTrigger )
                | SPI_IRQ_MASK(u.reg));

        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].bytesPerFrame = (spiConfigPtr->frameSize + 7)/8;

        //  ------------------------------
        //  Transfert mode selection.
        //  ------------------------------
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].rxMode = spiConfigPtr->rxMode;
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].txMode = spiConfigPtr->txMode;
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].irqHandler = spiConfigPtr->handler;

        g_halSpiPropArray[id].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        g_halSpiPropArray[id].txIfcCh = HAL_UNKNOWN_CHANNEL;

        // Register asked frequency
        g_halSpiPropArray[id].cfg[spiConfigPtr->enabledCS].freq = spiConfigPtr->spiFreq;
    }
}


// =============================================================================
// hal_SpiFlushFifos
// -----------------------------------------------------------------------------
/// Flush both SPI Fifos.
/// @param id Identifier of the SPI for which the function is called.
// =============================================================================
PUBLIC VOID hal_SpiFlushFifos(HAL_SPI_ID_T id)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    gc_halSpiHwpArray[id]->status = SPI_FIFO_FLUSH;
}


// =============================================================================
// hal_SpiClearRxDmaDone
// -----------------------------------------------------------------------------
/// clear the RX DMA Done status
// =============================================================================
PUBLIC VOID hal_SpiClearRxDmaDone(HAL_SPI_ID_T id)
{
    gc_halSpiHwpArray[id]->status = SPI_RX_DMA_DONE;
}


// =============================================================================
// hal_SpiClearTxDmaDone
// -----------------------------------------------------------------------------
/// clear the TX DMA Done status
// =============================================================================
PUBLIC VOID hal_SpiClearTxDmaDone(HAL_SPI_ID_T id)
{
    gc_halSpiHwpArray[id]->status = SPI_TX_DMA_DONE;
}


// =============================================================================
// hal_SpiOpen
// -----------------------------------------------------------------------------
/// Open a SPI CS driver.
/// This function enables the SPI in the mode selected by \c spiCfg.
/// <B> The polarity of both the Chip Select must be set in the configuration
/// structure tgt_BoardConfig.halCfg. The knowledge about those two polarities
/// is needed by the driver, and it cannot operate without them. </B>
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum Chip Select for which this configuration applies.
/// @param spiCfg The configuration for SPI
// =============================================================================
PUBLIC VOID hal_SpiOpen(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, CONST HAL_SPI_CFG_T* spiCfg)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    // Check the availability of the CS on the SPI used
    if (id == HAL_SPI)
    {
        HAL_ASSERT(csNum < CHIP_SPI_CS_QTY, "CS %d unavailable on SPI %d", csNum, id);
    }

    if (id == HAL_SPI_2)
    {
        HAL_ASSERT(csNum < CHIP_SPI2_CS_QTY, "CS %d unavailable on SPI %d", csNum, id);
    }

    // Driver initialization to do only once.
    // It could deserve its own function, but that would break
    // the interface from Jade, and, well, that's not mandatory
    // to just initialize one global variable.
    //HAL_TRACE(TSTDOUT, 0, "In hal_SpiOpen() ");

    HAL_ASSERT (csNum == spiCfg->enabledCS, "Incoherency between CS and configuration for SPI");
    hal_SpiDoConfigCs(id, spiCfg);
//hal_DbgXcpuBkptForce();
//g_halSpiActivating[csNum] = FALSE;
}


// =============================================================================
// hal_SpiGetActiveId
// -----------------------------------------------------------------------------
/// Private tool function to get the ActiveId of a given CS. Useful to test that
/// the currently active CS is the good one.
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum HAL_SPI_CS0, HAL_SPI_CS1 or HAL_SPI_CS2
/// @return  HAL_SPI_ACTIVE_CS_0, HAL_SPI_ACTIVE_CS_1 or HAL_SPI_ACTIVE_CS_2
/// FIXME this function is useless as HAL_SPI_CS_T == HAL_SPI_ACTIVE_T for
/// the active status ...
// =============================================================================
INLINE HAL_SPI_ACTIVE_T hal_SpiGetActiveId(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    switch (csNum)
    {
        case HAL_SPI_CS0:
            return HAL_SPI_ACTIVE_CS_0;
            break;

        case HAL_SPI_CS1:
            return HAL_SPI_ACTIVE_CS_1;
            break;

        case HAL_SPI_CS2:
            return HAL_SPI_ACTIVE_CS_2;
            break;

#if (CHIP_SPI_CS_QTY>3 || CHIP_SPI2_CS_QTY>3)
        case HAL_SPI_CS3:
            return HAL_SPI_ACTIVE_CS_3;
            break;
#endif

        default:
            HAL_ASSERT(FALSE, "Invalid SPI CS (hal_SpiGetActiveId)");
            return HAL_SPI_ACTIVE_NONE;
    }
}




// =============================================================================
// hal_SpiUpdateDivider
// -----------------------------------------------------------------------------
/// Update the SPI divider so that the maximum frequency defined for the currently
/// active CS is not overpassed.
/// NOTA: the check of the existence of this CS for this SPI is done once
/// at the activation.
/// @param id Identifier of the SPI for which the function is called.
/// @param sysFreq Newly applied system frequency
// =============================================================================
PROTECTED VOID hal_SpiUpdateDivider(HAL_SPI_ID_T id, HAL_SYS_FREQ_T sysFreq)
{
    UINT32 clkDivider;
    HAL_SPI_CS_T cs;
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    if (g_halSpiPropArray[id].activeStatus == HAL_SPI_ACTIVE_CS_0)
    {
        cs = HAL_SPI_CS0;
    }
    else if (g_halSpiPropArray[id].activeStatus == HAL_SPI_ACTIVE_CS_1)
    {
        cs = HAL_SPI_CS1;
    }
    else if (g_halSpiPropArray[id].activeStatus == HAL_SPI_ACTIVE_CS_2)
    {
        cs = HAL_SPI_CS2;
    }
#if (CHIP_SPI_CS_QTY>3 || CHIP_SPI2_CS_QTY>3)
    else if (g_halSpiPropArray[id].activeStatus == HAL_SPI_ACTIVE_CS_3)
    {
        cs = HAL_SPI_CS3;
    }
#endif
    else
    {
        // No active CS
        return;
    }

    // bad freq
    if (g_halSpiPropArray[id].cfg[cs].freq == 0)
        return;

    clkDivider =  sysFreq/ ( 2 * g_halSpiPropArray[id].cfg[cs].freq);
    // The required freq being a maximum, we should round up
    if (sysFreq%( 2 * g_halSpiPropArray[id].cfg[cs].freq) != 0)
    {
        clkDivider += 1;
    }

    clkDivider = (clkDivider) ? clkDivider-1 : 0;
    g_halSpiPropArray[id].cfg[cs].cfg &=  ~SPI_CLOCK_DIVIDER_MASK;
    g_halSpiPropArray[id].cfg[cs].cfg |= SPI_CLOCK_DIVIDER(clkDivider);

    // Apply change
    gc_halSpiHwpArray[id]->cfg = g_halSpiPropArray[id].cfg[cs].cfg;
}

// =============================================================================
// hal_SpiUpdateAllDividers
// -----------------------------------------------------------------------------
/// Update the SPI dividers so that the maximum frequency defined for the currently
/// active CS is not overpassed.
/// NOTA: the check of the existence of this CS for this SPI is done once
/// at the activation.
/// @param sysFreq Newly applied system frequency
// =============================================================================
UINT32 hal_SpiUpdateAllDividers(HAL_SYS_FREQ_T sysFreq)
{
    HAL_SPI_ID_T i;
    for(i=0; i<HAL_SPI_QTY; i++)
    {
        hal_SpiUpdateDivider(i, sysFreq);
    }

#if (CHIP_ISPI_CS_QTY != 0)
    // To avoid using another Id for the ISPI, we record its clock divider
    // update here.
    // hal_IspiUpdateDivider(sysFreq);
#endif
    return sysFreq;
}

// =============================================================================
// hal_SpiActivateCs
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
PUBLIC BOOL hal_SpiActivateCs(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_SYS_FREQ_T suitedSysFreq;
    HAL_SPI_ID_T i;
    UINT32 scStatus;
    BOOL doActivate = FALSE;
#ifdef FPGA
    UINT32 clkDivider;
#endif

    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    // This CS has not been opened
    HAL_ASSERT(g_halSpiPropArray[id].cfg[csNum].ctrl !=0, "CS %d not opened", csNum);

    HAL_TRACE(HAL_SPI_TRC, 0, "hal_SpiActivateCs: SPI%d CS%d",id,csNum);

    // test the Driver Configuration usage under critical section for data
    // coherency (OS Semaphore is not wanted to avoid descheduling possibility
    // so hal_SpiActivateCs can be called outside of a task)
    scStatus = hal_SysEnterCriticalSection();
    // test and set
    if (g_halSpiPropArray[id].activeStatus == HAL_SPI_ACTIVE_NONE)
    {
        doActivate = TRUE;
        g_halSpiPropArray[id].activeStatus = HAL_SPI_OPERATION_IN_PROGRESS;
    }
    hal_SysExitCriticalSection(scStatus);

    // do the activation job
    if (doActivate)
    {
        // Set Resource as active and set the clock
        // TODO implement a way to try and set the most suited
        // system frequency.
        if (HAL_SYS_FREQ_26M >= 2*g_halSpiPropArray[id].cfg[csNum].freq)
        {
            suitedSysFreq = HAL_SYS_FREQ_26M;
        }
        else  if (HAL_SYS_FREQ_39M >= 2*g_halSpiPropArray[id].cfg[csNum].freq)
        {
            suitedSysFreq = HAL_SYS_FREQ_39M;
        }
        else  if (HAL_SYS_FREQ_52M >= 2*g_halSpiPropArray[id].cfg[csNum].freq)
        {
            suitedSysFreq = HAL_SYS_FREQ_52M;
        }
        else  if (HAL_SYS_FREQ_78M >= 2*g_halSpiPropArray[id].cfg[csNum].freq)
        {
            suitedSysFreq = HAL_SYS_FREQ_78M;
        }
        else
        {
            suitedSysFreq = HAL_SYS_FREQ_104M;
        }
        // store frequency for this SPI
        g_halSpiPropArray[id].requiredSysFreq = suitedSysFreq;

        // compute the needed frequency for ALL SPIs
        for(i=0; i<HAL_SPI_QTY; i++)
        {
            if(g_halSpiPropArray[id].requiredSysFreq > suitedSysFreq)
            {
                suitedSysFreq = g_halSpiPropArray[id].requiredSysFreq;
            }
        }

        /// The call to #hal_SysRequestFreq will mechanically
        /// not necessarily call the UpdateDivider function, which will set
        /// maybe the best one for the choosen frequency.
        if (id == HAL_SPI) {
            if (!hal_ClkIsEnabled(gSpi1Clk))
                hal_ClkEnable(gSpi1Clk);
            hal_ClkSetRate(gSpi1Clk, suitedSysFreq);
        } else if (id == HAL_SPI_2) {
            if (!hal_ClkIsEnabled(gSpi2Clk))
                hal_ClkEnable(gSpi2Clk);
            hal_ClkSetRate(gSpi2Clk, suitedSysFreq);
        }
        /// Therefore, we update the divider manually at the end of the function

        /// But for FPGA, sysFreq is 39 MHZ and the system frequency change
        /// function is not called.
#ifdef FPGA
        clkDivider = HAL_SYS_FREQ_39M / ( 2 * g_halSpiPropArray[id].cfg[csNum].freq);
        // The required freq being a maximum, we should round up
        if (HAL_SYS_FREQ_39M%( 2 * g_halSpiPropArray[id].cfg[csNum].freq) != 0)
        {
            clkDivider += 1;
        }
        clkDivider = (clkDivider) ? clkDivider-1 : 0;
        g_halSpiPropArray[id].cfg[csNum].cfg &=  ~SPI_CLOCK_DIVIDER_MASK;
        g_halSpiPropArray[id].cfg[csNum].cfg |= SPI_CLOCK_DIVIDER(clkDivider);
#endif

        //hal_SpiIrqSetHandler(id, csNum, g_halSpiPropArray[id].cfg[csNum].irqHandler);
        g_halSpiPropArray[id].irqHandler = g_halSpiPropArray[id].cfg[csNum].irqHandler;

        // no CS is activated - Activate this onea
        gc_halSpiHwpArray[id]->cfg  = g_halSpiPropArray[id].cfg[csNum].cfg;
        gc_halSpiHwpArray[id]->ctrl = g_halSpiPropArray[id].cfg[csNum].ctrl;
#if (CHIP_SPI_HAS_DIO == 1)
        gc_halSpiHwpArray[id]->irq = g_halSpiPropArray[id].cfg[csNum].irqMask;
#endif
        g_halSpiPropArray[id].rxMode  = g_halSpiPropArray[id].cfg[csNum].rxMode;
        g_halSpiPropArray[id].txMode  = g_halSpiPropArray[id].cfg[csNum].txMode;
        g_halSpiPropArray[id].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        g_halSpiPropArray[id].txIfcCh = HAL_UNKNOWN_CHANNEL;

        // active the CS
        HAL_PROFILE_SPI_ACTIVATION(id, csNum);
        g_halSpiPropArray[id].activeStatus = csNum;

        scStatus = hal_SysEnterCriticalSection();
        // Update the divider
        // hal_SpiUpdateDivider need the activeStatus to be valid so we to it last
        hal_SpiUpdateDivider(id, hal_SysGetFreq());
        hal_SysExitCriticalSection(scStatus);
    }

    if (g_halSpiPropArray[id].activeStatus == hal_SpiGetActiveId(id, csNum))
    {
        // this CS is already active
        return TRUE;
    }
    else
    {
        // the other CS is active
        return FALSE;
    }
}




// =============================================================================
// hal_SpiRxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the RxIfc channel owned by
/// the request HAL_IFC_SPI_RX.
// =============================================================================
PUBLIC VOID hal_SpiRxIfcChannelRelease(HAL_SPI_ID_T id)
{
    hal_IfcChannelRelease(gc_halSpiIfRxId[id], g_halSpiPropArray[id].rxIfcCh);
}


// =============================================================================
// hal_SpiTxIfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of the TxIfc channel owned by
/// the request HAL_IFC_SPI_TX.
// =============================================================================
PUBLIC VOID hal_SpiTxIfcChannelRelease(HAL_SPI_ID_T id)
{
    hal_IfcChannelRelease(gc_halSpiIfTxId[id], g_halSpiPropArray[id].txIfcCh);
}


// =============================================================================
// hal_SpiDeActivateCs
// -----------------------------------------------------------------------------
/// Deactivate a Chip Select. This is the opposite operation to #hal_SpiActivateCs.
/// It must be called after #hal_SpiActivateCs to allow the activation of another
/// CS. The deactivation of a non-activated CS will trig an ASSERT.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS to deactivate.
// =============================================================================
PUBLIC VOID hal_SpiDeActivateCs(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    UINT32 scStatus;
    HAL_SYS_FREQ_T suitedSysFreq=HAL_SYS_FREQ_32K;
    HAL_SPI_ID_T i;
    BOOL doDeactivate = FALSE;

    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    HAL_TRACE(HAL_SPI_TRC, 0, "hal_SpiDeActivateCs: SPI%d CS%d",id,csNum);

    // test the Driver Configuration usage under critical section for data
    // coherency (OS Semaphore is not wanted to avoid descheduling possibility
    // so hal_SpiActivateCs can be called outside of a task)
    scStatus = hal_SysEnterCriticalSection();
    // test and set
    if (g_halSpiPropArray[id].activeStatus == hal_SpiGetActiveId(id, csNum))
    {
        doDeactivate = TRUE;
        g_halSpiPropArray[id].activeStatus = HAL_SPI_OPERATION_IN_PROGRESS;
    }
    hal_SysExitCriticalSection(scStatus);

    // do the deactivation job
    if (doDeactivate)
    {
        //HAL_TRACE(TSTDOUT, 0, "Deactivation with status=%d", g_halSpiPropArray[id].activeStatus);

        /// Turn off the SPI. and mask its irqs
        gc_halSpiHwpArray[id]->ctrl = 0;

        /// Disable the Rx IFC Channel.
        if ((IS_DMA(g_halSpiPropArray[id].rxMode)!=0) && (g_halSpiPropArray[id].rxIfcCh != HAL_UNKNOWN_CHANNEL))
        {
            hal_SpiRxIfcChannelRelease(id);
        }

        /// Disable the Tx IFC Channel.
        if ((IS_DMA(g_halSpiPropArray[id].txMode)!= 0) && (g_halSpiPropArray[id].txIfcCh != HAL_UNKNOWN_CHANNEL))
        {
            hal_SpiTxIfcChannelRelease(id);
        }

        // Clear all status
        gc_halSpiHwpArray[id]->status = 0xffffffff;

        g_halSpiPropArray[id].rxMode = HAL_SPI_OFF;
        g_halSpiPropArray[id].txMode = HAL_SPI_OFF;

        g_halSpiPropArray[id].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        g_halSpiPropArray[id].txIfcCh = HAL_UNKNOWN_CHANNEL;

        //  Release resource for low power
        g_halSpiPropArray[id].requiredSysFreq = HAL_SYS_FREQ_32K;

        if (id == HAL_SPI)
            hal_ClkDisable(gSpi1Clk);
        else if (id == HAL_SPI_2)
            hal_ClkDisable(gSpi2Clk);

        // Deactivate.
        g_halSpiPropArray[id].activeStatus = HAL_SPI_ACTIVE_NONE;
        HAL_PROFILE_SPI_DEACTIVATION(id, csNum);
    }
    else
    {
        // Attempt to deactivate a SPI that is not activated
        // RAISE
        //HAL_ASSERT(FALSE, "SPI: Improper deactivation of CS:%d", csNum);
    }

}



// =============================================================================
// hal_SpiClose
// -----------------------------------------------------------------------------
/// Close the SPI CS \c csNum. To use it again,
/// it must be opened and activated again.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum SPI CSto close.
// =============================================================================
PUBLIC VOID hal_SpiClose(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    /*HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
                                " Your system only has %d SPI", id, HAL_SPI_QTY);*/
    // few asserts
    //  SPI not opened
//    HAL_ASSERT((g_halSpiPropArray[id].cfg[csNum].ctrl!=0), "CS %d closed but not opened", csNum);

    // CS being deactivated before the SPI is closed,
    // there is no need to do that here ...
    // hal_SpiDeActivateCs(id, csNum);

    // Reset the configuration
    g_halSpiPropArray[id].cfg[csNum].cfg = 0;
    g_halSpiPropArray[id].cfg[csNum].ctrl = 0;
    g_halSpiPropArray[id].cfg[csNum].freq = 0;
    g_halSpiPropArray[id].cfg[csNum].rxMode = HAL_SPI_OFF;
    g_halSpiPropArray[id].cfg[csNum].txMode = HAL_SPI_OFF;
    g_halSpiPropArray[id].cfg[csNum].irqHandler = NULL;
}




// =============================================================================
// hal_SpiSendData
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
PUBLIC UINT32 hal_SpiSendData(
    HAL_SPI_ID_T id,
    HAL_SPI_CS_T csNum,
    CONST UINT8* startAddress,
    UINT32 length)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    UINT32 i;
    UINT32 freeRoom;

    //  Check the address pointer.
    HAL_ASSERT((startAddress != NULL),
               "SPI Tx with a NULL start address");

    // Check that we send a complete frames
    HAL_ASSERT(length%(g_halSpiPropArray[id].cfg[csNum].bytesPerFrame) == 0,
               "Sent size: %d does not correspond to an integer number of frames",
               length);

    //  Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Attempt to send data on the unactivated %d spi CS of SPI %d", csNum, id);

    if (!IS_DMA(g_halSpiPropArray[id].txMode))
    {
        // -----------------------------
        // DIRECT TRANSFER
        // -----------------------------
        freeRoom = GET_BITFIELD(gc_halSpiHwpArray[id]->status, SPI_TX_SPACE);

        if (freeRoom > length)
        {
            freeRoom = length;
        }

        //  Send data byte by byte.
        for (i = 0; i < freeRoom; i++)
        {
            gc_halSpiHwpArray[id]->rxtx_buffer = *(startAddress + i);
        }

        return freeRoom;
    }
    else
    {
        // -----------------------------
        // DMA TRANSFER
        // -----------------------------
        //  Transfer size too big
        HAL_ASSERT(length< (EXP2(SYS_IFC_TC_LEN)),
                   "SPI DMA transfer too large: %d bytes", length);

        g_halSpiPropArray[id].txIfcCh=hal_IfcTransferStart(gc_halSpiIfTxId[id],
                                      (UINT8*)startAddress,
                                      length, HAL_IFC_SIZE_8_MODE_AUTO);

        // get IFC channel and start if any available
        if (g_halSpiPropArray[id].txIfcCh == HAL_UNKNOWN_CHANNEL)
        {
            // No channel available
            // No data received
            return 0;
        }
        else
        {
            // all data will be fetched
            return length;
        }
    }
}



// =============================================================================
// hal_SpiTxFifoAvail
// -----------------------------------------------------------------------------
/// Get available data spaces in the Spi Tx FIFO.
/// This function returns the size of the available space in the Tx FIFO.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  cs   Chip select
/// @return Tthe size of the available space in the Tx FIFO.
// =============================================================================
PUBLIC UINT8 hal_SpiTxFifoAvail(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    UINT8 freeRoom;

    // Check if this CS has been activated.
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);

    // Get avail level.
    freeRoom = GET_BITFIELD(gc_halSpiHwpArray[id]->status, SPI_TX_SPACE);

    return freeRoom;
}



// =============================================================================
// hal_SpiTxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Tx FIFO.
/// This functions returns the number of bytes in the Tx FIFO.
//
/// @param id Identifier of the SPI for which the function is called.
/// @param  cs   Chip select
/// @return The number of bytes in the Tx FIFO.
// =============================================================================
#ifndef HAL_NO_ASSERT
PRIVATE UINT8 hal_SpiTxFifoLevel(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    UINT8 txLevel;

    // Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);

    // Get level
    txLevel = SPI_TX_FIFO_SIZE
              - GET_BITFIELD(gc_halSpiHwpArray[id]->status, SPI_TX_SPACE);

    return txLevel;
}
#endif

// =============================================================================
// hal_SpiTxDmaDone
// -----------------------------------------------------------------------------
/// Check if the transmission is finished.
///
/// This function returns \c TRUE when the last DMA transfer is finished.
/// Before sending new data in DMA mode, the previous transfer must be finished,
/// hence the use of this function for polling.\n
/// Note that the DMA transfer can be finished but the Tx FIFO of the SPI is
/// not empty. Before shutting down the SPI, one must check that the SPI FIFO
/// is empty and that the last byte has been completely sent by using
/// #hal_spiTxFinished.\n
/// Even if the Tx FIFO is not empty, if a previous DMA transfer is over, one
/// can start a new DMA transfert
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  cs   Chip select
/// @return \c TRUE is the previous DMA transfert is finshed.\n
///          \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SpiTxDmaDone(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
//  Check that the DMA mode is active.
    HAL_ASSERT(IS_DMA(g_halSpiPropArray[id].txMode),
               "SPI CS %d not in DMA mode ", csNum);

//  Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);

    if (g_halSpiPropArray[id].txIfcCh == HAL_UNKNOWN_CHANNEL)
    {
        return TRUE;
    }

    if (gc_halSpiHwpArray[id]->status & SPI_TX_DMA_DONE)
    {
        // clear done
        g_halSpiPropArray[id].txIfcCh = HAL_UNKNOWN_CHANNEL;
        gc_halSpiHwpArray[id]->status = SPI_TX_DMA_DONE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// =============================================================================
// hal_SpiTxFinished
// -----------------------------------------------------------------------------
/// Check if the last transfer is done
/// This function returns \c TRUE when the transmit FIFO is empty and when the
/// last byte is completely sent. It should be called before closing the SPI if
/// the last bytes of the transfer are important.\n
/// This function should not be called between transfers, in direct or DMA mode.
/// The @link #hal_SpiTxFifoLevel FIFO level @endlink for direct mode and the
/// @link #hal_SpiTxDmaDone DMA done indication @endlink for DMA allow
/// for a more optimized transmission.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  cs   Chip select
/// @return \c TRUE if the last tranfer is done and the Tx FIFO empty.\n
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SpiTxFinished(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    UINT32 spiStatus;
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    // Check if this CS has been activated.
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);
    spiStatus = gc_halSpiHwpArray[id]->status;

    // If SPI FSM is active and the TX Fifo is empty
    // (ie available space == Fifo size), the tf is not done
    if ((!(gc_halSpiHwpArray[id]->status & SPI_ACTIVE_STATUS))
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
// hal_SpiGetData
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
/// @param  cs   Chip select
/// @param dest_address Pointer on the buffer to store received data
/// @param length Number of byte to receive.
/// @return When in DMA mode, returns 0 if no DMA channel is available. \n
///         In direct mode or DMA mode with an available channel, returns the
///         number of received bytes.
// =============================================================================
PUBLIC UINT32 hal_SpiGetData(
    HAL_SPI_ID_T id,
    HAL_SPI_CS_T csNum,
    UINT8*      destAddress,
    UINT32      length
)
{
    UINT32 i;
    UINT32 nbAvailable;

    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    // Check the address pointer.
    HAL_ASSERT((destAddress != NULL) ,"NULL destination for SPI Rx");

    // Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);

    // Global var is change too as a system clock
    // change would applied that saved config to the SPI.
    gc_halSpiHwpArray[id]->ctrl = g_halSpiPropArray[id].cfg[csNum].ctrl;
    //gc_halSpiHwpArray[id]->cfg = g_halSpiCsCfg[csNum].cfg;

    // tmp while (1) to check the ctrl values
    // while(1);

    if (!IS_DMA(g_halSpiPropArray[id].rxMode))
    {
        // -----------------------------
        // DIRECT TRANSFER
        // -----------------------------
        nbAvailable = GET_BITFIELD(gc_halSpiHwpArray[id]->status, SPI_RX_LEVEL);

        if (nbAvailable > length)
        {
            nbAvailable = length;
        }

        //  Get data byte by byte.
        for (i = 0; i < nbAvailable; i++)
        {
            *(destAddress + i) = gc_halSpiHwpArray[id]->rxtx_buffer;
        }

        return nbAvailable;
    }
    else
    {
        // -----------------------------
        // DMA TRANSFER
        // -----------------------------
        //  Transfer size too big
        HAL_ASSERT(length< (EXP2(SYS_IFC_TC_LEN)),
                   "SPI Transfer size too big %d", length);

        g_halSpiPropArray[id].rxIfcCh = hal_IfcTransferStart( gc_halSpiIfRxId[id],
                                        destAddress,
                                        length, HAL_IFC_SIZE_8_MODE_AUTO);

        //g_halSpiRxIfcCh = 0;
        // get IFC channel and start if any available
        if (g_halSpiPropArray[id].rxIfcCh == HAL_UNKNOWN_CHANNEL)
        {
            // No channel available
            // No data received
            return 0;
        }
        else
        {
            // all data will be fecthed
            return length;
        }
    }
}



// =============================================================================
// hal_SpiRxFifoLevel
// -----------------------------------------------------------------------------
/// Get data quantity in the Spi Rx FIFO.
///
/// Returns the number of bytes in the Rx FIFO.
/// @param id Identifier of the SPI for which the function is called.
/// @param  cs   Chip select
/// @return The number of bytes in the Rx FIFO
// =============================================================================
PUBLIC UINT8 hal_SpiRxFifoLevel(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    UINT8 rxLevel;

    // Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);

    // Get level
    rxLevel = GET_BITFIELD(gc_halSpiHwpArray[id]->status, SPI_RX_LEVEL);

    return rxLevel;
}



// =============================================================================
// hal_SpiRxDmaDone
// -----------------------------------------------------------------------------
/// Check if the reception is finished.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param  cs   Chip select
/// @return This function returns \c TRUE when the last DMA transfer is finished.
/// Before receiving new data in DMA mode, the previous transfer must be
/// finished, hence the use of this function for polling.
// =============================================================================
PUBLIC BOOL hal_SpiRxDmaDone(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);
    // Check that the DMA mode is active.
    HAL_ASSERT(IS_DMA(g_halSpiPropArray[id].rxMode)," %d",0);

    // Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Unactivated spi CS %d", csNum);

    if (g_halSpiPropArray[id].rxIfcCh == HAL_UNKNOWN_CHANNEL)
    {
        return TRUE;
    }

    if (gc_halSpiHwpArray[id]->status & SPI_RX_DMA_DONE)
    {
        // clear done
        g_halSpiPropArray[id].rxIfcCh = HAL_UNKNOWN_CHANNEL;
        gc_halSpiHwpArray[id]->status = SPI_RX_DMA_DONE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



// =============================================================================
// hal_SpiStartInfiniteWriteMode
// -----------------------------------------------------------------------------
/// Do a stream write on the given CS.
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
/// @param infTx Pointer to the structure describing the streaming write.
// =============================================================================
PUBLIC VOID hal_SpiStartInfiniteWriteMode(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, CONST HAL_SPI_INFINITE_TX_T* infTx)
{
    UINT32 patternReg = 0;
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    //  Check if this CS has been activated

    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Attempt to send data on the unactivated %d spi CS of SPI %d", csNum, id);

    // Check that the line is free for use
    HAL_ASSERT(hal_SpiTxFifoLevel(id, csNum) == 0,
               "Data in the SPI Tx Fifo before an infinite write");
    HAL_ASSERT(hal_SpiRxFifoLevel(id, csNum) == 0,
               "Data in the SPI Rx Fifo before an infinite write");

    hal_SpiFlushFifos(id);
#if (CHIP_SPI_HAS_PATTERN_WHILE == 1)
    if (infTx->patternMode == HAL_SPI_PATTERN_UNTIL)
    {
        patternReg = SPI_PATTERN_MODE_ENABLED | SPI_PATTERN_SELECTOR_UNTIL | SPI_PATTERN(infTx->pattern);
    }

    if (infTx->patternMode == HAL_SPI_PATTERN_WHILE)
    {
        patternReg = SPI_PATTERN_MODE_ENABLED | SPI_PATTERN_SELECTOR_WHILE | SPI_PATTERN(infTx->pattern);
    }
#else
    patternReg = SPI_PATTERN(infTx->pattern)
                 | (infTx->patternMode?SPI_PATTERN_MODE_ENABLED:SPI_PATTERN_MODE_DISABLED);
#endif
    gc_halSpiHwpArray[id]->pattern = patternReg;

    gc_halSpiHwpArray[id]->stream = SPI_TX_STREAM_MODE_ENABLED
                                    | (infTx->sendOne?SPI_TX_STREAM_BIT_ONE:SPI_TX_STREAM_BIT_ZERO)
                                    | (infTx->autoStop?SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_ENABLED:
                                       SPI_TX_STREAM_STOP_WITH_RX_DMA_DONE_DISABLED);

}


// =============================================================================
// hal_SpiStopInfiniteWriteMode
// -----------------------------------------------------------------------------
/// Stop the stream write on the given CS.
/// This function is useless when the auto-stop mode is enabled.
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
// =============================================================================
PUBLIC VOID hal_SpiStopInfiniteWriteMode(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    //  Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Attempt to send data on the unactivated %d spi CS of SPI %d", csNum, id);

    gc_halSpiHwpArray[id]->stream = SPI_TX_STREAM_MODE_DISABLED;
}




// =============================================================================
// hal_SpiForcePin
// -----------------------------------------------------------------------------
/// Force an SPI pin to a certain state or release the force mode
/// and put the pin back to normal SPI mode.
///
/// @param id Identifier of the SPI for which the function is called.
/// @param pin Identification of the pins to be forced.
/// @param state State to enforce.
// =============================================================================
PUBLIC VOID hal_SpiForcePin(HAL_SPI_ID_T id, HAL_SPI_PIN_T pin,  HAL_SPI_PIN_STATE_T state)
{
    // Parameter check.
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    HAL_ASSERT(pin >= HAL_SPI_PIN_CS0,"Invalid SPI pin %d (negative).", pin);
    HAL_ASSERT(pin < HAL_SPI_PIN_LIMIT,  "Invalid SPI pin %d (too big).", pin);

    // Check against the chip capabilities
    if (id == HAL_SPI)
    {
        if (pin < HAL_SPI_PIN_CLK)
        {
            // That is a chip select, check ...
            HAL_ASSERT(pin < CHIP_SPI_CS_QTY, "This CS Pin %d doesn't exist for SPI %d", pin,id);
        }
    }

    if (id == HAL_SPI_2)
    {
        if (pin < HAL_SPI_PIN_CLK)
        {
            // That is a chip select, check ...
            HAL_ASSERT(pin < CHIP_SPI2_CS_QTY, "This CS Pin %d doesn't exist for SPI %d", pin,id);
        }
    }

    // Add a check about the muxing and the board hardware:
    // if a pin is not used as a SPI pin, say somenthing
    // awful there. (<=> Check against the chip configuration)
    // TODO

#if (CHIP_SPI_HAS_INTERNAL_FORCE==1)
    switch (pin)
    {
        case HAL_SPI_PIN_CS0  :
            gc_halSpiHwpArray[id]->pin_control &= ~SPI_CS0_CTRL_MASK;
            gc_halSpiHwpArray[id]->pin_control |= SPI_CS0_CTRL(state);
            break;

        case HAL_SPI_PIN_CS1  :
            gc_halSpiHwpArray[id]->pin_control &= ~SPI_CS1_CTRL_MASK;
            gc_halSpiHwpArray[id]->pin_control |= SPI_CS1_CTRL(state);
            break;

        case HAL_SPI_PIN_CS2  :
            gc_halSpiHwpArray[id]->pin_control &= ~SPI_CS2_CTRL_MASK;
            gc_halSpiHwpArray[id]->pin_control |= SPI_CS2_CTRL(state);
            break;
#if !defined(CHIP_DIE_8955) && !defined(CHIP_DIE_8909)
        case HAL_SPI_PIN_CS3  :
            gc_halSpiHwpArray[id]->pin_control &= ~SPI_CS3_CTRL_MASK;
            gc_halSpiHwpArray[id]->pin_control |= SPI_CS3_CTRL(state);
            break;
#endif
        case HAL_SPI_PIN_CLK  :
            gc_halSpiHwpArray[id]->pin_control &= ~SPI_CLK_CTRL_MASK;
            gc_halSpiHwpArray[id]->pin_control |= SPI_CLK_CTRL(state);
            break;

        case HAL_SPI_PIN_DO   :
            gc_halSpiHwpArray[id]->pin_control &= ~SPI_DO_CTRL_MASK;
            gc_halSpiHwpArray[id]->pin_control |= SPI_DO_CTRL(state);
            break;

        case HAL_SPI_PIN_DI   :
            // Nothing doable
            break;

        default:
            // There already a check, we shouldn't arrive there
            HAL_ASSERT(FALSE, "What the heck is this SPI pin %d your trying to force to %d",
                       pin,
                       state);
            break;
    }
#else
    BOOL gpioMode ;
    BOOL output ;
    BOOL value ;

    // Configure the state to apply to the concerned the pin
    switch (state)
    {
        case HAL_SPI_PIN_STATE_0:
            gpioMode = TRUE;
            output = TRUE;
            value = FALSE;
            break;

        case HAL_SPI_PIN_STATE_1:
            gpioMode = TRUE;
            output = TRUE;
            value = TRUE;
            break;

        case HAL_SPI_PIN_STATE_Z:
            gpioMode = TRUE;
            output = FALSE;
            value = FALSE;
            break;

        default: // HAL_SPI_PIN_STATE_SPI
            gpioMode = FALSE;
            output = FALSE;
            value = FALSE;
            break;
    }

    // Apply the state to the pin
    switch (pin)
    {
        case HAL_SPI_PIN_CS0  :
            hal_MuxGpio(IOMUX_MODE_PIN_SPI_CS_0, gpioMode, output, value);
            break;

        case HAL_SPI_PIN_CS1  :
            hal_MuxGpio(IOMUX_MODE_PIN_SPI_CS_1, gpioMode, output, value);
            break;

        case HAL_SPI_PIN_CS2  :
            hal_MuxGpio(IOMUX_MODE_PIN_SPI_CS_2, gpioMode, output, value);
            break;

        // That pin does not exist for chips up to now, which
        // don't have a force pin register, so will it probably
        // never appear. Kept here for, hum, esthaetics considerations.
        case HAL_SPI_PIN_CS3  :
            // hal_MuxGpio(0, gpioMode, output, value);
            break;

        case HAL_SPI_PIN_CLK  :
            hal_MuxGpio(IOMUX_MODE_PIN_SPI_CLK, gpioMode, output, value);
            break;

        case HAL_SPI_PIN_DO   :
            hal_MuxGpio(IOMUX_MODE_PIN_SPI_DO, gpioMode, output, value);
            break;

        case HAL_SPI_PIN_DI   :
            hal_MuxGpio(IOMUX_MODE_PIN_SPI_DI_1, gpioMode, output, value);
            break;

        default:
            // There already a check, we shouldn't arrive there
            HAL_ASSERT(FALSE, "What the heck is this SPI pin %d your trying to force to %d",
                       pin,
                       state);
            break;
    }

#endif


}



// -------------------------------------------------------------
// ------------------ IRQ functions --------------------
// -------------------------------------------------------------


// =============================================================================
// hal_SpiIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user irq handler function
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
/// @param handler User function called in case of SPI-related IRQ.
// =============================================================================
PUBLIC VOID hal_SpiIrqSetHandler(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, HAL_SPI_IRQ_HANDLER_T handler)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    //  Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Attempt to send data on the unactivated %d spi CS of SPI %d", csNum, id);
    g_halSpiPropArray[id].cfg[csNum].irqHandler = handler;
    g_halSpiPropArray[id].irqHandler = handler;
}



// =============================================================================
// hal_SpiIrqSetMask
// -----------------------------------------------------------------------------
/// Set the irq mask
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
/// @param mask Mask to set.
// =============================================================================
PUBLIC VOID hal_SpiIrqSetMask(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum, HAL_SPI_IRQ_STATUS_T mask)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    //  Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Attempt to send data on the unactivated %d spi CS of SPI %d", csNum, id);

    union
    {
        UINT32 reg;
        HAL_SPI_IRQ_STATUS_T bitfield;
    } u;

    // wazza ? no need for a mask if the spi is not enabled ><?
    if (g_halSpiPropArray[id].ctrl==0)
    {
        return;
    }
    u.bitfield = mask;

    UINT32 status = hal_SysEnterCriticalSection();
    // record mask
#if (CHIP_SPI_HAS_DIO == 1)
    g_halSpiPropArray[id].cfg[csNum].irqMask &= ~SPI_IRQ_MASK_MASK;
    g_halSpiPropArray[id].cfg[csNum].irqMask |= SPI_IRQ_MASK(u.reg) ;
    // set spi mask
    gc_halSpiHwpArray[id]->irq = g_halSpiPropArray[id].cfg[csNum].irqMask;
#else
    g_halSpiPropArray[id].cfg[csNum].ctrl &= ~SPI_IRQ_MASK_MASK;
    g_halSpiPropArray[id].cfg[csNum].ctrl |= SPI_IRQ_MASK(u.reg) ;
    // set spi mask
    gc_halSpiHwpArray[id]->ctrl = g_halSpiPropArray[id].cfg[csNum].ctrl;
#endif
    hal_SysExitCriticalSection(status);


}


// =============================================================================
// hal_SpiIrqGetMask
// -----------------------------------------------------------------------------
/// Get the irq mask.
/// @param id Identifier of the SPI for which the function is called.
/// @param csNum CS on which send the stream.
/// @return The IRQ mask.
// =============================================================================
PUBLIC HAL_SPI_IRQ_STATUS_T hal_SpiIrqGetMask(HAL_SPI_ID_T id, HAL_SPI_CS_T csNum)
{
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    //  Check if this CS has been activated
    HAL_ASSERT((g_halSpiPropArray[id].activeStatus == csNum),
               "Attempt to send data on the unactivated %d spi CS of SPI %d", csNum, id);

    union
    {
        UINT32 reg;
        HAL_SPI_IRQ_STATUS_T bitfield;
    } u;

#if (CHIP_SPI_HAS_DIO == 1)
    u.reg = GET_BITFIELD(gc_halSpiHwpArray[id]->irq, SPI_IRQ_MASK);
    HAL_ASSERT (u.reg == GET_BITFIELD(g_halSpiPropArray[id].cfg[csNum].irqMask, SPI_IRQ_MASK),
                "SPI %d CS %d, IRQ Mask Inconsitency", id, csNum);
#else
    u.reg = GET_BITFIELD(gc_halSpiHwpArray[id]->status, SPI_IRQ_MASK);
    HAL_ASSERT (u.reg == GET_BITFIELD(g_halSpiPropArray[id].cfg[csNum].ctrl, SPI_IRQ_MASK),
                "SPI %d CS %d, IRQ Mask Inconsitency", id, csNum);
#endif

    return u.bitfield;
}






// =============================================================================
// hal_SpiIrqHandler
// -----------------------------------------------------------------------------
/// Handler called in case of SPI interrupt
///
/// @param interruptId  Will be spi Hw module interrupt
/// id when used by the IRQ driver.
// =============================================================================
PROTECTED VOID hal_SpiIrqHandler(UINT8 interruptId)
{

    // Under the hypothesis that several SPI
    // would have their IRQ lines in a consecutive
    // order ... (First id is 0, potential SPI2 is 1)
    HAL_SPI_ID_T id = interruptId - SYS_IRQ_SPI1;
    HAL_ASSERT(id < HAL_SPI_QTY, "Improper SPI id %d ! \n"
               " Your system only has %d SPI", id, HAL_SPI_QTY);

    union
    {
        UINT32 reg;
        HAL_SPI_IRQ_STATUS_T bitfield;
    } cause;

    // cause.reg contains a copy of the status register
    cause.reg = gc_halSpiHwpArray[id]->status;
    // clear all cause
    gc_halSpiHwpArray[id]->status = cause.reg;

    // cause.reg is masked and shift to contain only
    // the IRQ cause bitfield
    cause.reg = GET_BITFIELD(cause.reg, SPI_IRQ_CAUSE);

    if (cause.bitfield.rxDmaDone)
    {
        g_halSpiPropArray[id].rxIfcCh = HAL_UNKNOWN_CHANNEL;
    }

    if (cause.bitfield.txDmaDone)
    {
        g_halSpiPropArray[id].txIfcCh = HAL_UNKNOWN_CHANNEL;
    }

    // Call the user IRQ handler function.
    if (g_halSpiPropArray[id].irqHandler)
    {
        g_halSpiPropArray[id].irqHandler(cause.bitfield);
    }
}


// =============================================================================
// hal_SpiGetRxIfcTc
// -----------------------------------------------------------------------------
/// Get the RxIfcTc.
/// @return ifcTc
// =============================================================================

PUBLIC UINT16 hal_SpiGetRxIfcTc(HAL_SPI_ID_T id)
{
    UINT16 ifcTc;
    ifcTc = hwp_sysIfc->std_ch[g_halSpiPropArray[id].rxIfcCh].tc;
    return ifcTc;
}

HAL_MODULE_CLK_INIT(SPI1)
{
    gSpi1Clk = hal_ClkGet(FOURCC_SPI1);
    HAL_ASSERT((gSpi1Clk != NULL), "Clk SPI1 not found!");
}

HAL_MODULE_CLK_INIT(SPI2)
{
    gSpi2Clk = hal_ClkGet(FOURCC_SPI2);
    HAL_ASSERT((gSpi2Clk != NULL), "Clk SPI2 not found!");
}


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

#include "i2c_master.h"
#include "cfg_regs.h"

#include "halp_debug.h"
#include "halp_sys.h"
#include "halp_config.h"

#include "hal_i2c.h"
#include "hal_clk.h"
#include "hal_host.h"

#include "pmd_m.h"

PRIVATE CONST CHAR* CONST g_halI2cErrStrIdOutOfRange = "I2C bus ID out of range: %d";
PRIVATE CONST CHAR* CONST g_halI2cErrStrIdOutOf8955Range = "I2C bus ID 8955: %d";
PRIVATE CONST CHAR* CONST g_halI2cErrStrIdNotUsed = "I2C bus is not configured as used: %d";
PRIVATE CONST CHAR* CONST g_halI2cErrStrIdNotActive = "I2C resource not active: %d";

static HAL_CLK_T *gI2cClk = NULL;
// =============================================================================
//  MACROS
// =============================================================================

/// Time in 16k clock we wait before sending a byte ...
#define HAL_I2C_SEND_BYTE_DELAY 20
/// Max i2c OPERATE TIME 10ms
#define HAL_I2C_OPERATE_TIME 163

// When (attempt) taking the bus.
#define HAL_I2C_TAKE_BUS(id)                                        \
    BOOL needFreqRes = TRUE;                                        \
    UINT32 criticalSectionValue = hal_SysEnterCriticalSection();    \
    if (g_halI2cInUse[id])                                          \
    {                                                               \
        /* I2C is busy serving another request. */                  \
        hal_SysExitCriticalSection(criticalSectionValue);           \
        return HAL_ERR_RESOURCE_BUSY;                               \
    }                                                               \
    else                                                            \
    {                                                               \
        for (int i=0; i<HAL_I2C_BUS_ID_QTY; i++)                    \
        {                                                           \
            if (g_halI2cInUse[i])                                   \
            {                                                       \
                needFreqRes = FALSE;                                \
                break;                                              \
            }                                                       \
        }                                                           \
        /* Use the bus. */                                          \
        g_halI2cInUse[id] = TRUE;                                   \
        if (needFreqRes)                                            \
        {                                                           \
            /*  Set the resource as active */                       \
            hal_ClkEnable(gI2cClk); \
        }                                                           \
        hal_SysExitCriticalSection(criticalSectionValue);           \
        /* And carry on the execution... */                         \
    }                                                               \

// When releasing the bus. (Hyp: Write is atomic)
#define HAL_I2C_RELEASE_BUS(id)                                     \
    needFreqRes = FALSE;                                            \
    criticalSectionValue = hal_SysEnterCriticalSection();           \
    g_halI2cInUse[id] = FALSE;                                      \
    for (int i=0; i<HAL_I2C_BUS_ID_QTY; i++)                        \
    {                                                               \
        if (g_halI2cInUse[i])                                       \
        {                                                           \
            needFreqRes = TRUE ;                                    \
            break;                                                  \
        }                                                           \
    }                                                               \
    hal_SysExitCriticalSection(criticalSectionValue);               \
    if (!needFreqRes)                                               \
    {                                                               \
        /* release the resource */                                  \
        hal_ClkDisable(gI2cClk);    \
    }                                                               \



// =============================================================================
//  TYPES
// =============================================================================



// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE HWP_I2C_MASTER_T* CONST g_halI2cMasterAddr[HAL_I2C_BUS_ID_QTY] =
{
#ifdef CHIP_DIE_8909
    hwp_i2c1,       hwp_i2c2,       hwp_i2c3
#else
    hwp_i2cMaster,  hwp_i2cMaster2, hwp_i2cMaster3
#endif
};

PRIVATE HAL_I2C_BPS_T   g_halI2cBps[HAL_I2C_BUS_ID_QTY]      =
{
    HAL_I2C_BPS_100K,
    HAL_I2C_BPS_100K,
    HAL_I2C_BPS_100K,
};
// Allow the first open.
PRIVATE BOOL            g_halI2cInUse[HAL_I2C_BUS_ID_QTY]    =
{
    FALSE,
    FALSE,
    FALSE,
};

PRIVATE BOOL            g_halI2cOpened[HAL_I2C_BUS_ID_QTY]   =
{
    FALSE,
    FALSE,
    FALSE,
};

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// hal_I2cUpdateDivider
// -----------------------------------------------------------------------------
/// Update the divider of the I2C module to cope with system frequency scaling.
/// @param sysFreq New System frequency to adapt the divider for in order to
/// keep a constant baudrate.
// =============================================================================
UINT32 hal_I2cUpdateDivider(HAL_SYS_FREQ_T sysFreq)
{
    UINT32 newClkScale;
    UINT32 ctrlReg;
    UINT32 i2cRate = 0;

    for(int i=0; i<HAL_I2C_BUS_ID_QTY; i++)
    {
        if (!g_halI2cInUse[i])
        {
            // Skip if bus is NOT taken
            continue;
        }

        HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[i];
        // Save control register
        ctrlReg = i2cMaster->CTRL & ~(I2C_MASTER_CLOCK_PRESCALE_MASK);

        switch(g_halI2cBps[i])
        {
            case HAL_I2C_BPS_100K:
                newClkScale = sysFreq/( 5 * 100000)-1-1;// for sprd  require.
                i2cRate = 100*1000;
                break;

            case HAL_I2C_BPS_400K:
                newClkScale = sysFreq/(5 * 400000)-1;
                i2cRate = 400*1000;
                break;

            default:
                // die
                HAL_ASSERT(FALSE, "Improper baudrate for I2C[%d]: %d", i, g_halI2cBps[i]);
                newClkScale = 100;
                break;
        }
        ctrlReg |= I2C_MASTER_CLOCK_PRESCALE(newClkScale);

        // Set the new clock scal.
        i2cMaster->CTRL = ctrlReg;
    }
    return i2cRate;
}


// ============================================================================
// hal_I2cOpen
// ----------------------------------------------------------------------------
/// This function opens the I2C modules and enable the use of its API.
///
/// @param id ID of the I2C bus
/// @param freq Frequency of the I2C bus
/// i2c1 muxed with mmc;i2c2 for camera; i2c3 for fm on 8955.
// ============================================================================
PUBLIC HAL_ERR_T hal_I2cOpen(HAL_I2C_BUS_ID_T id)
{
    UINT32 second_time,first_time;
    HAL_ASSERT(id < HAL_I2C_BUS_ID_QTY, g_halI2cErrStrIdOutOfRange, id);
#ifdef CHIP_DIE_8955
    HAL_ASSERT(id != HAL_I2C_BUS_ID_1, g_halI2cErrStrIdOutOf8955Range, id);
#endif

    if (id == HAL_I2C_BUS_ID_1)
    {
        HAL_ASSERT(g_halCfg->i2cCfg.i2cUsed == TRUE,
                   g_halI2cErrStrIdNotUsed, id);
    }
    else if (id == HAL_I2C_BUS_ID_2)
    {
        HAL_ASSERT(g_halCfg->i2cCfg.i2c2Used == TRUE,
                   g_halI2cErrStrIdNotUsed, id);
    }
    else // if (id == HAL_I2C_BUS_ID_3)
    {
        HAL_ASSERT(g_halCfg->i2cCfg.i2c3Used == TRUE,
                   g_halI2cErrStrIdNotUsed, id);
    }
    first_time = hal_TimGetUpTime();

    // I2C Pin's are not in GPIO mode
    // TODO Implement once the register is readable
    HAL_I2C_TAKE_BUS(id);

    // I2C needs 2 sets of power supply, one for I/O, and the other for pull-up resistor
    pmd_EnablePower(PMD_POWER_I2C, TRUE);

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    // I2C is initialized at the first open
    if (!g_halI2cOpened[id])
    {
        // Register Bps as a global variable.
        if (id == HAL_I2C_BUS_ID_1)
        {
            g_halI2cBps[id] = g_halCfg->i2cCfg.i2cBps;
        }
        else if (id == HAL_I2C_BUS_ID_2)
        {
            g_halI2cBps[id] = g_halCfg->i2cCfg.i2c2Bps;
        }
        else // if (id == HAL_I2C_BUS_ID_3)
        {
            g_halI2cBps[id] = g_halCfg->i2cCfg.i2c3Bps;
        }

        UINT32 scStatus = hal_SysEnterCriticalSection();
        // Set initial clock divider
        hal_I2cUpdateDivider(hal_SysGetFreq());
        hal_SysExitCriticalSection(scStatus);

        // Enable the I2c
        i2cMaster->CTRL |= I2C_MASTER_EN;
        i2cMaster->CMD  = I2C_MASTER_STO;

        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };
        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

        g_halI2cOpened[id] = TRUE;
    }

    // When I2C is powered off, some I2C slave devices might see a fake
    // start bit in the I2C bus. Then after I2C is powered on again,
    // the device might interpret the signal in a wrong manner.
    // Send a series of '1' to this I2C bus so as to avoid the possible
    // wrong interpretation.
    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->TXRX_BUFFER = 0xFE;
    i2cMaster->CMD = I2C_MASTER_WR|I2C_MASTER_STA;
    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->TXRX_BUFFER = 0xFF;
    i2cMaster->CMD = I2C_MASTER_WR;
    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->TXRX_BUFFER = 0xFF;
    i2cMaster->CMD = I2C_MASTER_WR;
    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->TXRX_BUFFER = 0xFF;
    i2cMaster->CMD = I2C_MASTER_WR|I2C_MASTER_STO;


    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };


    // Allow
    HAL_I2C_RELEASE_BUS(id);
    return HAL_ERR_NO;
}



// ============================================================================
// hal_I2cClose
// ----------------------------------------------------------------------------
/// This function closes the I2C module.
///
/// @param id ID of the I2C bus
// ============================================================================
PUBLIC VOID hal_I2cClose(HAL_I2C_BUS_ID_T id)
{
    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    // I2C needs 2 sets of power supply, one for I/O, and the other for pull-up resistor
    pmd_EnablePower(PMD_POWER_I2C, FALSE);

    // I2C is never really closed once initialized
}


// ============================================================================
// hal_I2cSleep
// ----------------------------------------------------------------------------
/// Check if I2C driver can be put to sleep.
///
/// @param id ID of the I2C bus
/// @return \c TRUE If going to lowpower is possible.
// ============================================================================
PROTECTED BOOL hal_I2cSleep(HAL_I2C_BUS_ID_T id)
{
    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    // Check if possible to release the resource
    if (g_halI2cInUse[id] == TRUE
            || (i2cMaster->STATUS & (I2C_MASTER_TIP|I2C_MASTER_BUSY)))
    {
        // Busy
        return FALSE;
    }
    else
    {
        // Configure pins for low power
        // TODO: ???
        return TRUE;
    }

}


// ============================================================================
// hal_I2cWakeup
// ----------------------------------------------------------------------------
/// Wake-Up I2C driver.
///
/// @param id ID of the I2C bus
// ============================================================================
PROTECTED VOID hal_I2cWakeup(HAL_I2C_BUS_ID_T id)
{
    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    // Allow usage.
    //g_halI2cInUse[id] = FALSE;
}

// ============================================================================
// hal_I2cSendByte
// ----------------------------------------------------------------------------
/// This function write one byte at a given address of a given slave.
///
/// @param id ID of the I2C bus
/// @param slaveAddr Address of the slave to which we write a byte.
/// @param memAddr Address on the slave's memmory map where to write the
/// data byte.
/// @param data Byte to write
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY, if the I2C FIFO is full
/// at that time, or HAL_ERR_COMMUNICATION_FAILED.
// ============================================================================
PUBLIC HAL_ERR_T hal_I2cSendByte(HAL_I2C_BUS_ID_T id, UINT32 slaveAddr, UINT32 memAddr, UINT8 data)
{
    UINT32 second_time,first_time;

    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    HAL_I2C_TAKE_BUS(id);

    hal_TimDelay(HAL_I2C_SEND_BYTE_DELAY);

    first_time = hal_TimGetUpTime();

    // test bus availability
    if (i2cMaster->STATUS & (I2C_MASTER_TIP|I2C_MASTER_BUSY))
    {
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_RESOURCE_BUSY;
    }

    // Clear status bit in case previous transfer (Raw) hasn't
    // cleared it.
    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Write slave address
    i2cMaster -> TXRX_BUFFER = (slaveAddr << 1);
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STA;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Write memory address
    i2cMaster -> TXRX_BUFFER = memAddr;
    i2cMaster -> CMD = I2C_MASTER_WR;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }


    // Write Value
    i2cMaster -> TXRX_BUFFER = data;
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STO ;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;


    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Stop condition sent via the previous
        // command
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    HAL_I2C_RELEASE_BUS(id);
    return HAL_ERR_NO;
}



// ============================================================================
// hal_I2cGetByte
// ----------------------------------------------------------------------------
/// This function reads one byte from a given address of a given slave's memory
/// map.
///
/// @param id ID of the I2C bus
/// @param slaveAddr Address of the slave from which we read a byte.
/// @param memAddr Address in the memory map where to get the byte.
/// @param pData Pointer to the place where the read data will be written.
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY, if the I2C FIFO is full
/// at that time, or HAL_ERR_COMMUNICATION_FAILED.
// ============================================================================
PUBLIC HAL_ERR_T hal_I2cGetByte(HAL_I2C_BUS_ID_T id, UINT32 slaveAddr, UINT32 memAddr, UINT8* pData)
{
    UINT32 second_time,first_time;

    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    first_time = hal_TimGetUpTime();

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    HAL_I2C_TAKE_BUS(id);

    // Clear status bit in case previous transfer (Raw) hasn't
    // cleared it.
    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Write slave address (Write mode, to write memory address)
    i2cMaster -> TXRX_BUFFER = (slaveAddr << 1);
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STA;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Write memory address
    i2cMaster -> TXRX_BUFFER = memAddr;
    i2cMaster -> CMD = I2C_MASTER_WR;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Write slave address + R/W = '1' (Read mode)
    i2cMaster -> TXRX_BUFFER = (slaveAddr << 1 | 0x1);
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STA;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }


    // Read value - send no acknowledge - send stop condition/bit
    i2cMaster -> CMD = I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done
// No Acknowledge is send by the  master.
#if 0
    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        return HAL_ERR_COMMUNICATION_FAILED;
    }
#endif
    *pData = i2cMaster -> TXRX_BUFFER;

    HAL_I2C_RELEASE_BUS(id);
    return HAL_ERR_NO;
}

// ============================================================================
// hal_I2cSendData
// ----------------------------------------------------------------------------
/// This function write a buffer of data to a given slave on the I2C bus.
/// A typical use of this function is to set several consecutive registers
/// in just one access for device with auto-incremented register addresses.
/// The memAddr value is the first written to the slave and is the address of
/// the first register to set. The values to write are the length bytes of the
/// buffer.
///
/// @param id ID of the I2C bus
/// @param slaveAddr Address of the slave to which we write a byte.
/// @param memAddr Address on the slave's memory map from which to write the
/// data bytes, or (equivalent to) first value written to the slave.
/// @param pData Pointer to the buffer to write.
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY, if the I2C FIFO is full
/// at that time, or HAL_ERR_COMMUNICATION_FAILED.
// ============================================================================
PUBLIC HAL_ERR_T hal_I2cSendData(HAL_I2C_BUS_ID_T id, UINT32 slaveAddr,  UINT32 memAddr,
                                 CONST UINT8* pData,      UINT32 length)
{
    UINT32 second_time,first_time;

    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    first_time = hal_TimGetUpTime();

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    HAL_I2C_TAKE_BUS(id);

    UINT32 currentByte = 0;
    // test bus availability
    if (i2cMaster->STATUS & (I2C_MASTER_TIP|I2C_MASTER_BUSY))
    {
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_RESOURCE_BUSY;
    }

    // Clear status bit in case previous transfer (Raw) hasn't
    // cleared it.
    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Write slave address
    i2cMaster -> TXRX_BUFFER = (slaveAddr << 1);
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STA;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Write memory address
    i2cMaster -> TXRX_BUFFER = memAddr;
    i2cMaster -> CMD = I2C_MASTER_WR;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }


    // Write all data but the last one.
    for (currentByte = 0 ; currentByte < length-1 ; currentByte++)
    {
        i2cMaster -> TXRX_BUFFER = pData[currentByte];
        i2cMaster -> CMD = I2C_MASTER_WR;

        // Polling on the TIP flag
        /*         while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                i2cMaster->CMD = I2C_MASTER_STO;
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

        // Check RxACK
        if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
        {
            // Stop condition sent via the previous
            // command
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_COMMUNICATION_FAILED;
        }
    }

    // Send last byte with stop condition
    i2cMaster -> TXRX_BUFFER = pData[length-1];
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STO ;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if(second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;


    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Stop condition sent via the previous
        // command
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    HAL_I2C_RELEASE_BUS(id);
    return HAL_ERR_NO;
}


// ============================================================================
// hal_I2cGetData
// ----------------------------------------------------------------------------
/// This function reads a number of bytes from a given address of a given slave's memory
/// map.
///
/// @param id ID of the I2C bus
/// @param slaveAddr Address of the slave from which we read a byte.
/// @param memAddr Address in the memory map where to get the byte.
/// @param pData Pointer to the place where the read data will be written.
/// @param length Number of bytes to read
/// @return #HAL_ERR_NO or #HAL_ERR_RESOURCE_BUSY, if the I2C FIFO is full
/// at that time, or HAL_ERR_COMMUNICATION_FAILED.
// ============================================================================
PUBLIC HAL_ERR_T hal_I2cGetData(HAL_I2C_BUS_ID_T id, UINT32 slaveAddr,   UINT32 memAddr,
                                UINT8* pData,       UINT32 length)
{
    UINT32 second_time,first_time;

    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    first_time = hal_TimGetUpTime();

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    HAL_I2C_TAKE_BUS(id);
    UINT32 currentByte = 0;

    // Clear status bit in case previous transfer (Raw) hasn't
    // cleared it.
    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Write slave address (Write mode, to write memory address)
    i2cMaster -> TXRX_BUFFER = (slaveAddr << 1);
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STA;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Write memory address
    i2cMaster -> TXRX_BUFFER = memAddr;
    i2cMaster -> CMD = I2C_MASTER_WR;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Write slave address + R/W = '1' (Read mode)
    i2cMaster -> TXRX_BUFFER = (slaveAddr << 1 | 0x1);
    i2cMaster -> CMD = I2C_MASTER_WR | I2C_MASTER_STA;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            i2cMaster->CMD = I2C_MASTER_STO;
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Transfert done

    // Check RxACK
    if (i2cMaster -> STATUS & I2C_MASTER_RXACK)
    {
        // Abort the transfert
        i2cMaster -> CMD = I2C_MASTER_STO ;
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;
        HAL_I2C_RELEASE_BUS(id);
        return HAL_ERR_COMMUNICATION_FAILED;
    }

    // Read all values but the last one
    for (currentByte=0; currentByte<length-1 ; currentByte++)
    {
        // Read value
        i2cMaster -> CMD = I2C_MASTER_RD;

        // Polling on the TIP flag
        /*         while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
        while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
        {
            second_time = hal_TimGetUpTime();
            if (second_time - first_time > HAL_I2C_OPERATE_TIME)
            {
                i2cMaster->CMD = I2C_MASTER_STO;
                HAL_I2C_RELEASE_BUS(id);
                return HAL_ERR_RESOURCE_TIMEOUT;
            }
        };

        i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

        // Store read value
        pData[currentByte] = i2cMaster -> TXRX_BUFFER;
    }

    // Read last value - send no acknowledge - send stop condition/bit
    i2cMaster -> CMD = I2C_MASTER_RD | I2C_MASTER_ACK | I2C_MASTER_STO;

    // Polling on the TIP flag
    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return HAL_ERR_RESOURCE_TIMEOUT;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    pData[length-1] = i2cMaster -> TXRX_BUFFER;

    HAL_I2C_RELEASE_BUS(id);
    return HAL_ERR_NO;
}





// ============================================================================
// hal_I2cSendRawByte
// ----------------------------------------------------------------------------
/// This function sends a single byte on the I2C interface
/// It is not designed for use in the real I2C protocol, but allows access
/// for non-standard usages such as SCCB for the Omnivision Camera control
///
/// @param id ID of the I2C bus
/// @param SendByte raw data byte to be sent.
/// @param CmdMask this is the command associated with this byte.  It must
/// be sent at the same time so the two actions (write to the data fifo and
/// issue the command) are done together.
/// @return HAL_ERR_NO if everything was OK, otherwise Resource Busy if the
/// bus is in use
// ============================================================================
PUBLIC HAL_ERR_T hal_I2cSendRawByte(HAL_I2C_BUS_ID_T id, UINT8 SendByte, UINT32 CmdMask)
{
    UINT32 second_time,first_time;

    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    first_time = hal_TimGetUpTime();

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    HAL_I2C_TAKE_BUS(id);

// TODO: Don't check bus right now, but need to do some check later
// test bus availability
//    if (i2cMaster->STATUS & (I2C_MASTER_TIP|I2C_MASTER_BUSY))
//    {
//        HAL_I2C_RELEASE_BUS(id);
//        return HAL_ERR_RESOURCE_BUSY;
//    }


    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            //camera drv used HAL_ERR_RESOURCE_BUSY
            return HAL_ERR_RESOURCE_BUSY;
        }
    };


    // Write slave address
    i2cMaster->TXRX_BUFFER = (REG32)SendByte;
    i2cMaster->CMD = (REG32)CmdMask;

    HAL_I2C_RELEASE_BUS(id);
    return HAL_ERR_NO;
}

// ============================================================================
// hal_I2cReadRawByte
// ----------------------------------------------------------------------------
/// This function completes the 2 phase read cycle which was previously
/// initiated by the hal_I2cSendRawByte() function with 2PHASE_RD_PH1 as the
/// command.  This function requires the command mask to be passed as a
/// parameter, but for the SCCB, it should be (Stop | Read | NACK)
///
/// @param id ID of the I2C bus
/// @param CmdMask this is the command mask required for the final phase of the
/// SCCB read cycle.
/// @return Data byte read by the I2C interface
/// FIXME Shouldn't we lock the bus since the beginning of hal_I2cSendRawByte
/// and only release it here ?
// ============================================================================
PUBLIC UINT8 hal_I2cReadRawByte(HAL_I2C_BUS_ID_T id, UINT32 CmdMask)
{
    UINT32 second_time,first_time;

    HAL_ASSERT(id<HAL_I2C_BUS_ID_QTY && g_halI2cOpened[id],
               g_halI2cErrStrIdNotActive, id);

    first_time = hal_TimGetUpTime();

    HWP_I2C_MASTER_T* CONST i2cMaster = g_halI2cMasterAddr[id];

    // FIXME How do we know that what is returned there is an error ?
    // and not the received byte ?
    HAL_I2C_TAKE_BUS(id);

    while(i2cMaster -> STATUS & I2C_MASTER_TIP)
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return 0xff;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    // Read value - send no acknowledge - send stop condition/bit
    i2cMaster->CMD = CmdMask;

    // Polling on the TIP flag
    /*     while(i2cMaster -> STATUS & I2C_MASTER_TIP); */
    while(!(i2cMaster -> STATUS & I2C_MASTER_IRQ_STATUS))
    {
        second_time = hal_TimGetUpTime();
        if (second_time - first_time > HAL_I2C_OPERATE_TIME)
        {
            HAL_I2C_RELEASE_BUS(id);
            return 0xff;
        }
    };

    i2cMaster->IRQ_CLR = I2C_MASTER_IRQ_CLR;

    HAL_I2C_RELEASE_BUS(id);
    return((UINT8)i2cMaster->TXRX_BUFFER);
}

HAL_MODULE_CLK_INIT(I2C)
{
    gI2cClk = hal_ClkGet(FOURCC_I2C);
    HAL_ASSERT((gI2cClk != NULL), "Clk I2C not found!");
}

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

#include "global_macros.h"
#include "sys_ctrl.h"
#include "uart.h"
#include "debug_uart.h"
#include "timer.h"
#include "debug_host.h"
#include "boot_host.h"

#include "boot_uart.h"
#include "boot.h"
#include "hal_uart.h"
#include "hal_sys.h"

#include "sxs_rmc.h"
#include "sxs_io.h"
#include "cfg_regs.h"

// =============================================================================
//  MACROS
// =============================================================================
#define UART_MONITOR_FRAME_SIZE     4

// Break duration in 16384Hz ticks
#define UART_MONITOR_BREAK_DURATION 4 // 3 is at least (worst case) 2/16384 = 122 us
// and break min is 10/115200 = 86.8 us  /

// Framse start pattern
#define SXS_RMT_START_FRM   0xAD

// Tx time-out in 16kHz ticks (0.004s)
#define BOOT_UART_MONITOR_TX_TIME_OUT    (16384 * 10)

// Rx time-out in 16kHz ticks (0.004) * 10
#define BOOT_UART_MONITOR_RX_TIME_OUT    (16384 * 120)

// Time during which we attempt to read a config string at a given
// baudrate (xxx s)
#define BOOT_UART_MONITOR_ABR_DURATION    (16384 * 2)


// =============================================================================
//  TYPES
// =============================================================================
typedef enum
{
    BOOT_UM_ERR_NO,
    BOOT_UM_ERR_RX_FAILED,
    BOOT_UM_ERR_TX_FAILED,
    BOOT_UM_ERR_BAD_CRC,

    BOOT_UM_ERR_QTY
}
BOOT_UM_ERR_T;


// =============================================================================
// BOOT_UART_MONITOR_CMD_TYPE_T
// -----------------------------------------------------------------------------
/// That type describes the kind of command we will receive.
// =============================================================================
typedef enum
{
    BOOT_UART_MONITOR_AB,
    BOOT_UART_MONITOR_AT,
    BOOT_UART_MONITOR_BAD
} BOOT_UART_MONITOR_CMD_TYPE_T;


// =============================================================================
// BOOT_UART_MONITOR_FRAME_HEADER_T
// -----------------------------------------------------------------------------
/// Type describing the header of a serial Frame
// =============================================================================
typedef struct
{
    UINT8 startPattern;
    UINT8 frameLengthMSB;
    UINT8 frameLengthLSB;
    UINT8 flowId;
} BOOT_UART_MONITOR_FRAME_HEADER_T;

// =============================================================================
// BOOT_UART_MONITOR_RW_HEADER_T
// -----------------------------------------------------------------------------
/// Type used to parse the header of a read/write command sent through the uart.
// =============================================================================
typedef struct
{
    // Number of bytes to write
    UINT16 size;
    // Type of access to the memory: 1 = byte, 2 = DWORD, 4 = WORD
    UINT8  access;
    UINT8  pad;
    // Start address where to write
    UINT8* address;
} BOOT_UART_MONITOR_RW_HEADER_T;


// =============================================================================
// BOOT_UART_MONITOR_DUMP_HEADER_T
// -----------------------------------------------------------------------------
/// Type used to construct the header of a read(dump) command sent through the uart.
// =============================================================================
typedef struct
{
    // Trace id, unused ?
    UINT16 id;
    // Number of bytes to write
    UINT16 size;
    // unused
    UINT8    format[4];
    // Start address where to write
    UINT8* address;
} BOOT_UART_MONITOR_DUMP_HEADER_T;




// =============================================================================
// BOOT_UART_MONITOR_RPC_HEADER_T
// -----------------------------------------------------------------------------
/// Type used to parse the header of a read/write command sent through the uart.
// =============================================================================
typedef struct
{
    // Function to call
    UINT32 (*function)(UINT32, ...);

    /// Id describing the function
    /// - returns VOID
    /// - returns a value
    /// - returns a pointer
    UINT32 id;

    /// Size of each one of the four parameters
    /// SXS_RPC_NO_PARAM is no parameter (0xFF)
    /// SXS_RPC_VALUE_PARAM means a UINT32 parameter (0)
    /// Other values are not supported
    UINT8 len[SXS_NB_RPC_PARAM];

} BOOT_UART_MONITOR_RPC_HEADER_T;






// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PROTECTED HAL_UART_BAUD_RATE_T g_bootUartMonitorBaudRate;

PROTECTED BOOL g_bootUartMonitorBrDetected = FALSE;



// =============================================================================
//  FUNCTIONS
// =============================================================================

PROTECTED BOOT_UM_ERR_T boot_UartMonitorProcess(VOID);

PROTECTED BOOT_UM_ERR_T boot_UartMonitorWriteProcess(UINT32 length);

PROTECTED BOOT_UM_ERR_T boot_UartMonitorRpcProcess(UINT32 length);

PROTECTED BOOT_UM_ERR_T boot_UartMonitorReadProcess(UINT32 length);

PROTECTED BOOT_UM_ERR_T boot_UartMonitorExitProcess(UINT32 length);

// =============================================================================
// boot_UartMonitorGetBaudRate
// -----------------------------------------------------------------------------
/// Used to recover the value of the detected monitor uart
/// @param baudRate pointer where the detected value, if any, will be
/// written.
/// @return \c TRUE If a connected Uart was detected and the baudrate
/// discover, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL boot_UartMonitorGetBaudRate(HAL_UART_BAUD_RATE_T* rate)
{
    *rate = g_bootUartMonitorBaudRate;
    return g_bootUartMonitorBrDetected;
}



// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================
INLINE VOID boot_UartSetDivider(HAL_UART_BAUD_RATE_T rate)
{
    // Formula from the uart driver
    // Divider mode is 4
    // There is no PLL during monitoring
    // Rq: the plus 4/2 * rate is a subtle way to get the upper integer
    UINT32 divider = ((HAL_SYS_FREQ_26M + 2 * rate) / ( 4 * rate)) - 2;
    hwp_sysCtrl->Cfg_Clk_Uart[1] = SYS_CTRL_UART_DIVIDER(divider)
                                   | SYS_CTRL_UART_SEL_PLL_SLOW;

}


// =============================================================================
// boot_UartMonitorSend
// -----------------------------------------------------------------------------
/// Basic sending function.  Done on polling, check
/// TX fifo availability and wait for room if needed.
///
/// @param data Data to send
/// @param length Number of byte to send.
/// @param checksum Pointer to the UINT8 holding the CRC of the frame
/// this transmitting is as part of.
/// @return #BOOT_UM_ERR_NO or #BOOT_UM_ERR_RESOURCE_TIMEOUT;
// =============================================================================
PROTECTED BOOT_UM_ERR_T boot_UartMonitorSend(UINT8* data, UINT32 length, UINT8* checksum)
{
    UINT32 i;
    UINT32 startTime;
    BOOL    onTime = TRUE;
    for (i=0 ; i<length ; i++)
    {
        startTime = hwp_timer->HWTimer_CurVal;

        // Place in the TX Fifo ?
        while ((GET_BITFIELD(hwp_uart->status, UART_TX_FIFO_SPACE) == 0)
                && (onTime = ((hwp_timer->HWTimer_CurVal - startTime) < BOOT_UART_MONITOR_TX_TIME_OUT)));

        if (!onTime)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }

        hwp_uart->rxtx_buffer = data[i];
        *checksum ^= data[i];
    }
    return BOOT_UM_ERR_NO;
}


// =============================================================================
// boot_UartMonitorGet
// -----------------------------------------------------------------------------
/// Read some bytes and actualizes the checksum.
/// @param data Array where read bytes will be stored
/// @param size Number of bytes to receive.
/// @param checksum Pointer to the value to XOR the read byte to calculate
/// the checksum.
// =============================================================================
PROTECTED BOOT_UM_ERR_T boot_UartMonitorGet(UINT8* data, UINT32 size, UINT8* checksum)
{
    UINT32 i;
    UINT32 startTime;
    BOOL    onTime = TRUE;

    for (i=0; i<size; i++)
    {
        startTime = hwp_timer->HWTimer_CurVal;

        while ((GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL) == 0)
                && (onTime = ((hwp_timer->HWTimer_CurVal - startTime) < BOOT_UART_MONITOR_RX_TIME_OUT)));
        if (!onTime)
        {
            return BOOT_UM_ERR_RX_FAILED;
        }

        data[i] = hwp_uart->rxtx_buffer;
        *checksum ^= data[i];
    }
    return BOOT_UM_ERR_NO;
}


PROTECTED VOID boot_UartMonitorOpen(VOID)
{
    hwp_configRegs->GPIO_Mode        &=
        ~(CFG_REGS_MODE_PIN_UART1_RTS | CFG_REGS_MODE_PIN_UART1_CTS);
    //      Enable the UART, @115200.
    hwp_sysCtrl->Cfg_Clk_Uart[1]  = SYS_CTRL_UART_DIVIDER(55)
                                    | SYS_CTRL_UART_SEL_PLL_SLOW;
    // Configure the UART.
    hwp_uart->triggers            = UART_AFC_LEVEL(1); //7 ?
    hwp_uart->ctrl                = UART_ENABLE | UART_DATA_BITS_8_BITS |
                                    UART_TX_STOP_BITS_1_BIT | UART_PARITY_ENABLE_NO |
                                    UART_RTS | UART_AUTO_FLOW_CONTROL;
    // Allow reception
    hwp_uart->CMD_Set             = UART_RTS;
}

PROTECTED VOID boot_UartMonitorClose(VOID)
{
    hwp_uart->ctrl    = 0;
    hwp_uart->CMD_Clr = UART_RTS;
}

// =============================================================================
// boot_UartMonitorAck
// -----------------------------------------------------------------------------
/// This functions sends small ack strings. Done on polling, check
/// TX fifo availability and wait for room if needed. A dump payload
/// is sent in a SXS_DATA_RMC flow Id.
///
/// @param string Data to send
/// @param length Number of byte to send.
/// @return #BOOT_UM_ERR_NO or #BOOT_UM_ERR_RESOURCE_TIMEOUT;
// =============================================================================
PROTECTED BOOT_UM_ERR_T boot_UartMonitorAck(UINT8* string, UINT32 length)
{
    BOOT_UART_MONITOR_FRAME_HEADER_T frameHeader;
    BOOT_UART_MONITOR_DUMP_HEADER_T  dumpHeader;
    UINT16 frameLength;
    UINT8 sendCrc;
    UINT8 tmp;

    dumpHeader.id = _HAL;
    dumpHeader.size = length;
    dumpHeader.format[0] = 0;
    dumpHeader.format[1] = 0;
    dumpHeader.format[2] = 0;
    dumpHeader.format[3] = 0;
    dumpHeader.address = (UINT8*)0x0;

    frameLength = sizeof(BOOT_UART_MONITOR_DUMP_HEADER_T) + length + 1;

    frameHeader.startPattern = SXS_RMT_START_FRM;
    frameHeader.frameLengthMSB = (frameLength & (0xFF00))>>8;
    frameHeader.frameLengthLSB = (frameLength & (0x00FF));
    frameHeader.flowId = SXS_DATA_RMC;

    sendCrc = frameHeader.flowId;

    // Send frame header and payload header
    // CRC doesn't take frame header into account
    if (BOOT_UM_ERR_NO !=boot_UartMonitorSend((UINT8*) &frameHeader,
            sizeof(BOOT_UART_MONITOR_FRAME_HEADER_T), &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    if (BOOT_UM_ERR_NO != boot_UartMonitorSend((UINT8*) &dumpHeader,
            sizeof(BOOT_UART_MONITOR_DUMP_HEADER_T), &sendCrc))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    // Send data
    if (BOOT_UM_ERR_NO != boot_UartMonitorSend(string, length, &sendCrc))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    // Send CRC
    if (BOOT_UM_ERR_NO !=boot_UartMonitorSend((UINT8*) &sendCrc, 1, &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }
    return BOOT_UM_ERR_NO;
}


/// @return TRUE if Times out
PROTECTED BOOL boot_UartMonitorTimeOut(BOOL start, UINT32* chrono)
{
    UINT32 delay = *chrono;

    if (start)
    {
        *chrono = hwp_timer->HWTimer_CurVal;
        *chrono += delay;
    }
    else
    {
        // sign extension on the difference allow to transparently
        // handle the wrap
        return ((INT32)(hwp_timer->HWTimer_CurVal - *chrono) > 0);
    }
    return FALSE;
}



PROTECTED BOOT_MONITOR_OP_STATUS_T boot_UartMonitor(VOID)
{
    // Configured initially baudrate @ 115200
    HAL_UART_BAUD_RATE_T         baudRate   = HAL_UART_BAUD_RATE_115200;
    VOLATILE UINT8               cmdType[2] = {0,0};
    BOOT_UART_MONITOR_CMD_TYPE_T cmdId;
    BOOL                         tryFaster  = TRUE;
    UINT32                       timeOut;

    BOOT_UM_ERR_T                errCode;
    BOOT_MONITOR_OP_STATUS_T     returnStatus = BOOT_MONITOR_OP_STATUS_NONE;

    g_bootUartMonitorBrDetected = FALSE;

    if (GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL) == 0)
    {
        return BOOT_MONITOR_OP_STATUS_NONE;
    }

    // We received a command: we are not waiting anymore but actually acting
    // as a monitor.
    hwp_debugHost->p2h_status   = BOOT_HST_STATUS_NONE;

    while (tryFaster)
    {
        timeOut = BOOT_UART_MONITOR_ABR_DURATION;

        boot_UartMonitorTimeOut(TRUE, &timeOut);

        // Need 1 byte
        while ((GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL) == 0 )
                && (!boot_UartMonitorTimeOut(FALSE, &timeOut)));

        if (GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL)!= 0)
        {
            // First byte correct ?
            cmdType[0] = hwp_uart->rxtx_buffer;
            if (cmdType[0] == 'A' || cmdType[0] == 'A')
            {
                // Valid char, wait for a second char
                timeOut = BOOT_UART_MONITOR_RX_TIME_OUT;
                boot_UartMonitorTimeOut(TRUE, &timeOut);

                while ((GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL) == 0)
                        && !(boot_UartMonitorTimeOut(FALSE, &timeOut)));

                if (GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL)!= 0)
                {
                    // Second byte correct?
                    cmdType[1] = hwp_uart->rxtx_buffer;
                    if (cmdType[0] == 'A' && cmdType[1] == 'B')
                    {
                        // "AB" string received
                        cmdId = BOOT_UART_MONITOR_AB;
                    }
                    else if (cmdType[0] == 'A' && cmdType[1] == 'T')
                    {
                        // "AT" string received
                        cmdId = BOOT_UART_MONITOR_AT;
                    }
                    else
                    {
                        // Unknown string received
                        cmdId = BOOT_UART_MONITOR_BAD;
                    }

                }
                else
                {
                    // Time out on 2nd char
                    cmdId = BOOT_UART_MONITOR_BAD;
                }
            }
            else
            {
                // Invalid first char, bad baudrate
                cmdId = BOOT_UART_MONITOR_BAD;
            }
        }
        else
        {
            // Time out on first char
            cmdId = BOOT_UART_MONITOR_BAD;
        }



        switch (cmdId)
        {
            case BOOT_UART_MONITOR_AB: //"AB"
                // Ack
                errCode = boot_UartMonitorAck("OK", 2);

                // Store baudrate somewhere ?
                g_bootUartMonitorBaudRate   = baudRate;
                g_bootUartMonitorBrDetected = TRUE;

                // If an error occured in Tx, exit the monitor now, ie
                // don't execute the monitor process
                if (errCode == BOOT_UM_ERR_NO)
                {
                    errCode = boot_UartMonitorProcess();
                }
                if (errCode != BOOT_UM_ERR_NO)
                {
                    // Error occured. Exit
                    returnStatus = BOOT_MONITOR_OP_STATUS_EXIT_ON_ERROR;
                }

                // Normal exit
                /// The uart monitor, contrary to the USB and HOST monitor, is a blocking
                /// process. Exiting it means we're to exit the monitoring loop.
                tryFaster = FALSE;
                returnStatus = BOOT_MONITOR_OP_STATUS_EXIT;
                break;

            case BOOT_UART_MONITOR_AT: //"AT"
                // Debug ack --------------
                hwp_uart->rxtx_buffer = 'O';
                hwp_uart->rxtx_buffer = 'K';
                hwp_uart->rxtx_buffer = '\r';
                // ----------

                // Store baudrate somewhere ?
                g_bootUartMonitorBaudRate = baudRate;
                g_bootUartMonitorBrDetected = TRUE;

                // Normal exit
                tryFaster = FALSE;
                returnStatus = BOOT_MONITOR_OP_STATUS_CONTINUE;
                break;

            default:

                // Break on UART
                hwp_uart->CMD_Set = UART_TX_BREAK_CONTROL;
                timeOut = UART_MONITOR_BREAK_DURATION;
                boot_UartMonitorTimeOut(TRUE, &timeOut);
                while (!boot_UartMonitorTimeOut(FALSE, &timeOut));
                hwp_uart->CMD_Clr = UART_TX_BREAK_CONTROL;

                // Try a faster baudrate
                baudRate *= 2;
                if (baudRate > HAL_UART_BAUD_RATE_921600) // Max rate, too fast
                {
                    // Exit
                    tryFaster = FALSE;
                    // TODO Check that status
                    returnStatus = BOOT_MONITOR_OP_STATUS_NONE;
                }
                else
                {

                    // Close
                    hwp_uart->ctrl &= ~UART_ENABLE;

                    // Reopen at new rate
                    boot_UartSetDivider(baudRate);
                    hwp_uart->ctrl |= UART_ENABLE_ENABLE;
                    hwp_uart->CMD_Set = UART_RX_FIFO_RESET;
                }
        }
    }

    // Finish sending before doing anything silly
    while ((hwp_uart->status & (UART_TX_ACTIVE | UART_TX_FIFO_SPACE_MASK))
            != UART_TX_FIFO_SPACE(UART_TX_FIFO_SIZE));

    // NO Break

    return returnStatus;
}










// =============================================================================
// boot_UartMonitorProcess
// -----------------------------------------------------------------------------
/// The uart monitor, contrary to the USB and HOST monitor, is a blocking
/// process. Exiting it means we're to exit the monitoring loop.
// =============================================================================
PROTECTED BOOT_UM_ERR_T boot_UartMonitorProcess(VOID)
{
    BOOL monitoring = TRUE;
    UINT8 frameLengthMSB = 0;
    UINT8 frameLengthLSB = 0;
    // Frame length is the number of bytes of payload (ie from byte 4)
    // Full frame length is frame length + 4(pattern, frame length MSB and LSB
    // + frameId + checksum.
    UINT16 frameLength = 0;
    /*BOOT_UART_MONITOR_FLOW_ID_T*/ UINT8 flowId;

    UINT32 timeOut;

    while (monitoring)
    {
        timeOut = 4 * BOOT_UART_MONITOR_RX_TIME_OUT;
        boot_UartMonitorTimeOut(TRUE, &timeOut);
        // Wait for a valid command
        while ( (GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL) < 4 )
                && !boot_UartMonitorTimeOut(FALSE, &timeOut));
        // Start pattern ?
        // If the start pattern is not right, we ignore the byte.
        if (hwp_uart->rxtx_buffer != SXS_RMT_START_FRM)
        {
            // Bad start pattern.
            if (boot_UartMonitorAck("TRASHED", 7) != BOOT_UM_ERR_NO)
            {
                return BOOT_UM_ERR_TX_FAILED;
            }
            continue;
        }

        // Byte 1 and 2 are frame length
        frameLengthMSB = hwp_uart->rxtx_buffer;
        frameLengthLSB = hwp_uart->rxtx_buffer;
        frameLength = (frameLengthMSB << 8) | frameLengthLSB;

        // Byte 3 is flow Id
        flowId = hwp_uart->rxtx_buffer;

        switch (flowId)
        {
            case SXS_WRITE_RMC:
                if (boot_UartMonitorWriteProcess(frameLength)
                        == BOOT_UM_ERR_TX_FAILED)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                break;
            // Remote procedure call
            case SXS_RPC_RMC:
                if (boot_UartMonitorRpcProcess(frameLength)
                        == BOOT_UM_ERR_TX_FAILED)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                break;
            case SXS_READ_RMC:
                if (boot_UartMonitorReadProcess(frameLength)
                        == BOOT_UM_ERR_TX_FAILED)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                break;
            case SXS_EXIT_RMC:
                if (boot_UartMonitorExitProcess(frameLength)
                        == BOOT_UM_ERR_TX_FAILED)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                // Exit the command loop
                monitoring = FALSE;
                break;
            default:
                // Unsupported command
                if (boot_UartMonitorAck("BAD_CMD", 7) != BOOT_UM_ERR_NO)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
        }
    }

    // Normal Exit
    return BOOT_UM_ERR_NO;
}






PROTECTED BOOT_UM_ERR_T boot_UartMonitorWriteProcess(UINT32 length)
{
    UINT8 checksum = SXS_WRITE_RMC;
    BOOT_UART_MONITOR_RW_HEADER_T writeHeader;
    UINT32 size;
    UINT32 remainingBytes = length;
    UINT8 tmp;
    UINT32 i;

    // Get and parse header
    if (BOOT_UM_ERR_NO != boot_UartMonitorGet((UINT8*)&writeHeader, sizeof(BOOT_UART_MONITOR_RW_HEADER_T), &checksum))
    {
        if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        else
        {
            return BOOT_UM_ERR_RX_FAILED;
        }
    }

    remainingBytes -= sizeof(BOOT_UART_MONITOR_RW_HEADER_T);
    remainingBytes -= writeHeader.size;

    if ((writeHeader.access != 1) && (writeHeader.access != 2)
            && (writeHeader.access != 4))
    {
        writeHeader.access = 1;
    }

    size = writeHeader.size / writeHeader.access;

    switch (writeHeader.access)
    {
        case 1:
        {
            // Byte access
            if (BOOT_UM_ERR_NO != boot_UartMonitorGet(writeHeader.address, size, &checksum))
            {
                if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                else
                {
                    return BOOT_UM_ERR_RX_FAILED;
                }
            }
        }
        break;

        case 2:
        {
            // double access
            UINT16* address = (UINT16*) ((UINT32)writeHeader.address & ~0x1);
            UINT8   value[2];

            for (i = 0 ; i < size; i++)
            {
                if (BOOT_UM_ERR_NO != boot_UartMonitorGet(value, 2, &checksum))
                {
                    if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
                    {
                        return BOOT_UM_ERR_TX_FAILED;
                    }
                    else
                    {
                        return BOOT_UM_ERR_RX_FAILED;
                    }
                }
                // Check the size, if not a multiple of 2, do not write.
                if (writeHeader.size % 2 == 0)
                {
                    *address++ = *((UINT16*)value);
                }
            }

            // Check the size of the packet, if not a multiple of 2, error.
            if (writeHeader.size  % 2 != 0)
            {
                if (boot_UartMonitorAck("SIZ_ERR", 7) != BOOT_UM_ERR_NO)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                else
                {
                    return BOOT_UM_ERR_RX_FAILED;
                }
            }
        }
        break;

        case 4:
        {
            // word access
            UINT32* address = (UINT32*) ((UINT32)writeHeader.address & ~0x3);
            UINT8   value[4];

            for (i=0; i<size; i++)
            {
                if (BOOT_UM_ERR_NO != boot_UartMonitorGet(value, 4, &checksum))
                {
                    if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
                    {
                        return BOOT_UM_ERR_TX_FAILED;
                    }
                    else
                    {
                        return BOOT_UM_ERR_RX_FAILED;
                    }

                }

                // Check the size, if not a multiple of 4, do not write.
                if (writeHeader.size % 4 == 0)
                {
                    *address++ = *((UINT32*)value);
                }
            }

            // Check the size of the packet, if not a multiple of 4, error.
            if (writeHeader.size % 4 != 0)
            {
                if (boot_UartMonitorAck("SIZ_ERR", 7) != BOOT_UM_ERR_NO)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                else
                {
                    return BOOT_UM_ERR_RX_FAILED;
                }
            }
        }
        break;
    }

    while (remainingBytes >0)
    {
        // Byte access
        if (BOOT_UM_ERR_NO != boot_UartMonitorGet(&tmp, 1, &checksum))
        {
            if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
            {
                return BOOT_UM_ERR_TX_FAILED;
            }
            else
            {
                return BOOT_UM_ERR_RX_FAILED;
            }
        }
        remainingBytes --;
    }

    // Check the CRC
    if (checksum != 0)
    {
        if (boot_UartMonitorAck("CRC_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        return BOOT_UM_ERR_BAD_CRC;
    }
    else
    {
        if (boot_UartMonitorAck("OK", 2) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
    }

    return BOOT_UM_ERR_NO;
}




PROTECTED BOOT_UM_ERR_T boot_UartMonitorRpcProcess(UINT32 length)
{

    BOOT_UART_MONITOR_FRAME_HEADER_T frameHeader;
    BOOT_UART_MONITOR_RPC_HEADER_T   rpcHeader;
    UINT32 parameters[SXS_NB_RPC_PARAM];
    UINT32 returnedValue;
    UINT32 remainingBytes = length;
    UINT8 checksum = SXS_RPC_RMC;

    UINT16 frameLength;
    // CRC for the sent message
    UINT8 sendCrc;
    UINT8 tmp;
    UINT32 i;

    // Get and parse header
    if (BOOT_UM_ERR_NO != boot_UartMonitorGet((UINT8*)&rpcHeader, sizeof(BOOT_UART_MONITOR_RPC_HEADER_T), &checksum))
    {
        if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        else
        {
            return BOOT_UM_ERR_RX_FAILED;
        }
    }
    remainingBytes -= sizeof(BOOT_UART_MONITOR_RPC_HEADER_T);

    // Get RPC data (ie parameters)
    for (i=0 ; i<SXS_NB_RPC_PARAM ; i++)
    {
        if (rpcHeader.len[i] != SXS_RPC_NO_PARAM)
        {
            if (BOOT_UM_ERR_NO != boot_UartMonitorGet((UINT8*)&parameters[i], 4, &checksum))
            {
                if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
                else
                {
                    return BOOT_UM_ERR_TX_FAILED;
                }
            }
            remainingBytes -= 4;
        }
        else
        {
            // no more parameters
            break;
        }
    }

    // check cmd crc
    while (remainingBytes >0)
    {
        // Byte access
        if (BOOT_UM_ERR_NO != boot_UartMonitorGet(&tmp, 1, &checksum))
        {
            if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
            {
                return BOOT_UM_ERR_TX_FAILED;
            }
            else
            {
                return BOOT_UM_ERR_RX_FAILED;
            }
        }
        remainingBytes--;
    }

    // Check the CRC
    if (checksum != 0)
    {
        if (boot_UartMonitorAck("CRC_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        return BOOT_UM_ERR_BAD_CRC;
    }

    // Send an acknowledge to tell the remote side we will start the execution.
    if (boot_UartMonitorAck("OK", 2) != BOOT_UM_ERR_NO)
    {
        return BOOT_UM_ERR_TX_FAILED;
    }

    // Execute procedure
    returnedValue = rpcHeader.function(parameters[0], parameters[1], parameters[2], parameters[3]);

    // When the functions sends no return value, the value 0xc001c001 is sent
    // to be used as an acknowledge.
    if (rpcHeader.id == SXS_RPC_RETURN_VOID)
    {
        returnedValue = 0xc001c001;
        rpcHeader.id = SXS_RPC_RETURN_VALUE;
    }

    // Frame length is the payload length + CRC. Payload is rsp header (4 bytes of id)
    // + number of returned bytes (4 for a UINT32 or content of memory pointed by a return
    // buffer. This size is defined by the
    frameLength = 4 + ((SXS_GET_RPC_RET_SZE(rpcHeader.id) == 0)?4:SXS_GET_RPC_RET_SZE(rpcHeader.id))
                  + 1;
    frameHeader.startPattern = SXS_RMT_START_FRM;
    frameHeader.frameLengthMSB = (frameLength & (0xFF00))>>8;
    frameHeader.frameLengthLSB = (frameLength & (0x00FF));
    frameHeader.flowId = SXS_RPC_RSP_RMC;

    sendCrc = frameHeader.flowId;

    // Send frame header
    // CRC is calculated with data only
    if (BOOT_UM_ERR_NO !=boot_UartMonitorSend((UINT8*) &frameHeader,
            sizeof(BOOT_UART_MONITOR_FRAME_HEADER_T), &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    // First data is RSP id ( == RPC id)
    if (BOOT_UM_ERR_NO != boot_UartMonitorSend((UINT8*) &rpcHeader.id, 4, &sendCrc))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    // Actual return
    if (rpcHeader.id & SXS_RPC_RETURN_VALUE)
    {
        if (BOOT_UM_ERR_NO != boot_UartMonitorSend((UINT8*) &returnedValue, 4, &sendCrc))
        {
            return BOOT_UM_ERR_TX_FAILED; // Send ERROR
        }
    }
    else
    {
        if (BOOT_UM_ERR_NO != boot_UartMonitorSend((UINT8*) returnedValue, SXS_GET_RPC_RET_SZE(rpcHeader.id), &sendCrc))
        {
            return BOOT_UM_ERR_TX_FAILED; // Send ERROR
        }
    }

    // Last byte is the CRC
    if (BOOT_UM_ERR_NO != boot_UartMonitorSend(&sendCrc, 1, &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    return BOOT_UM_ERR_NO;
}



PROTECTED BOOT_UM_ERR_T boot_UartMonitorReadProcess(UINT32 length)
{
    BOOT_UART_MONITOR_FRAME_HEADER_T frameHeader;
    BOOT_UART_MONITOR_DUMP_HEADER_T  dumpHeader;
    BOOT_UART_MONITOR_RW_HEADER_T    readHeader;
    UINT16 frameLength;
    // CRC for the sent message
    UINT8 sendCrc;
    UINT32 remainingBytes = length ;
    UINT8 tmp;
    UINT8 checksum = SXS_READ_RMC;
    UINT32 i;
    UINT32 size;

    // Get and parse header
    if (BOOT_UM_ERR_NO != boot_UartMonitorGet((UINT8*)&readHeader, sizeof(BOOT_UART_MONITOR_RW_HEADER_T), &checksum))
    {
        if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        else
        {
            return BOOT_UM_ERR_RX_FAILED;
        }
    }
    remainingBytes -= sizeof(BOOT_UART_MONITOR_RW_HEADER_T);

    // check cmd crc
    while (remainingBytes >0)
    {
        // Byte access
        if (BOOT_UM_ERR_NO != boot_UartMonitorGet(&tmp, 1, &checksum))
        {
            if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
            {
                return BOOT_UM_ERR_TX_FAILED;
            }
            else
            {
                return BOOT_UM_ERR_RX_FAILED;
            }
        }
        remainingBytes --;
    }

    // Check the CRC
    if (checksum != 0)
    {
        if (boot_UartMonitorAck("CRC_ERR", 10) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        return BOOT_UM_ERR_BAD_CRC;
    }


    // Frame length is the payload length + CRC. Payload is dump header + number of
    // read bytes. Read bytes is given by the command.
    frameLength = sizeof(BOOT_UART_MONITOR_DUMP_HEADER_T) + readHeader.size + 1;


    frameHeader.startPattern = SXS_RMT_START_FRM;
    frameHeader.frameLengthMSB = (frameLength & (0xFF00))>>8;
    frameHeader.frameLengthLSB = (frameLength & (0x00FF));
    frameHeader.flowId = SXS_DUMP_RMC;

    sendCrc = frameHeader.flowId;

    dumpHeader.id = _HAL;
    dumpHeader.size = readHeader.size;
    dumpHeader.format[0] = ' ';
    dumpHeader.format[1] = ' ';
    dumpHeader.format[2] = ' ';
    dumpHeader.format[3] = 0;
    dumpHeader.address = readHeader.address;

    if ((readHeader.access != 1) && (readHeader.access != 2)
            && (readHeader.access != 4))
    {
        readHeader.access = 1;
    }

    size = readHeader.size / readHeader.access;

    // Configure format field.
    if (readHeader.access == 1)
    {
        dumpHeader.format[0] = '%';
        dumpHeader.format[1] = '2';
        dumpHeader.format[2] = 'x';
    }
    else if (readHeader.access == 2)
    {
        dumpHeader.format[0] = '%';
        dumpHeader.format[1] = '4';
        dumpHeader.format[2] = 'x';
    }
    else if (readHeader.access == 4)
    {
        dumpHeader.format[0] = '%';
        dumpHeader.format[1] = '8';
        dumpHeader.format[2] = 'x';
    }

    // Check the size of the packet, if not a multiple of "access mode", error.
    if (readHeader.size % readHeader.access != 0)
    {
        if (boot_UartMonitorAck("SIZ_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        else
        {
            return BOOT_UM_ERR_RX_FAILED;
        }
    }

    // Send frame header and payload header
    // CRC is calculated only with data
    if (BOOT_UM_ERR_NO != boot_UartMonitorSend((UINT8*) &frameHeader,
            sizeof(BOOT_UART_MONITOR_FRAME_HEADER_T), &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    if (BOOT_UM_ERR_NO != boot_UartMonitorSend((UINT8*) &dumpHeader,
            sizeof(BOOT_UART_MONITOR_DUMP_HEADER_T), &sendCrc))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    switch (readHeader.access)
    {
        case 1:
        {
            // Byte access
            if (BOOT_UM_ERR_NO != boot_UartMonitorSend(readHeader.address, size, &sendCrc))
            {
                return BOOT_UM_ERR_TX_FAILED; // Send ERROR
            }
        }
        break;

        case 2:
        {
            // Double access
            UINT16* address = (UINT16*) ((UINT32)readHeader.address & ~0x1);
            UINT8   value[2];

            for (i = 0; i<size; i++)
            {
                *((UINT16*)value) = *address++ ;
                if (BOOT_UM_ERR_NO != boot_UartMonitorSend(value, 2, &sendCrc))
                {
                    return BOOT_UM_ERR_TX_FAILED; // Send ERROR
                }
            }
        }
        break;

        case 4:
        {
            // Word access
            UINT32* address = (UINT32*) ((UINT32)readHeader.address & ~0x3);
            UINT8   value[4];

            for (i=0; i<size; i++)
            {
                *((UINT32*)value) = *address++ ;
                if (BOOT_UM_ERR_NO != boot_UartMonitorSend(value, 4, &sendCrc))
                {
                    return BOOT_UM_ERR_TX_FAILED; // Send ERROR
                }
            }
        }
        break;
    }

    // Last byte is the CRC
    if (BOOT_UM_ERR_NO != boot_UartMonitorSend(&sendCrc, 1, &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    if (boot_UartMonitorAck("OK", 2) != BOOT_UM_ERR_NO)
    {
        return BOOT_UM_ERR_TX_FAILED;
    }

    return BOOT_UM_ERR_NO;
}


PROTECTED BOOT_UM_ERR_T boot_UartMonitorExitProcess(UINT32 length)
{
    UINT8 crc;
    UINT8 tmp;

    // To keep the compiler warning
    length = length;

    if (BOOT_UM_ERR_NO != boot_UartMonitorGet(&crc, 1, &tmp))
    {
        if (boot_UartMonitorAck("COM_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        else
        {
            return BOOT_UM_ERR_RX_FAILED;
        }
    }

    if (crc != SXS_EXIT_RMC)
    {
        if (boot_UartMonitorAck("CRC_ERR", 7) != BOOT_UM_ERR_NO)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }
        else
        {
            return BOOT_UM_ERR_BAD_CRC;
        }
    }

    if (boot_UartMonitorAck("OK", 2) != BOOT_UM_ERR_NO)
    {
        return BOOT_UM_ERR_TX_FAILED;
    }
    else
    {
        return BOOT_UM_ERR_NO;
    }
}


#if 0

// =============================================================================
// boot_HstUartSend
// -----------------------------------------------------------------------------
/// Basic sending function.  Done on polling, check
/// TX fifo availability and wait for room if needed.
///
/// @param data Data to send
/// @param length Number of byte to send.
/// @param checksum Pointer to the UINT8 holding the CRC of the frame
/// this transmitting is as part of.
/// @return #BOOT_UM_ERR_NO or #BOOT_UM_ERR_RESOURCE_TIMEOUT;
// =============================================================================
PROTECTED BOOT_UM_ERR_T boot_HstUartSend(UINT8* data, UINT32 length, UINT8* checksum)
{
    UINT32 i;
    UINT32 startTime;
    BOOL    onTime = TRUE;
    for (i=0 ; i<length ; i++)
    {
        startTime = hwp_timer->HWTimer_CurVal;

        // Place in the TX Fifo ?
        while ((GET_BITFIELD(hwp_debugUart->status, DEBUG_UART_TX_FIFO_LEVEL) == DEBUG_UART_TX_FIFO_SIZE)
                && (onTime = ((hwp_timer->HWTimer_CurVal - startTime) < BOOT_UART_MONITOR_TX_TIME_OUT)));

        if (!onTime)
        {
            return BOOT_UM_ERR_TX_FAILED;
        }

        hwp_debugUart->rxtx_buffer = data[i];
        *checksum ^= data[i];
    }
    return BOOT_UM_ERR_NO;
}


// =============================================================================
// boot_HstUartDump
// -----------------------------------------------------------------------------
/// Send a buffer of data in a dump packet through the host.
/// @param data Pointer to the data to send.
/// @param length Lenght of the buffer to send.
/// @param access Access width in bytes (1,2 or 4).
/// @return #BOOT_UM_ERR_NO or #BOOT_UM_ERR_TX_FAILED.
// =============================================================================
PUBLIC UINT32 boot_HstUartDump(UINT8* data, UINT32 length, UINT32 access)
{
    BOOT_UART_MONITOR_FRAME_HEADER_T frameHeader;
    BOOT_UART_MONITOR_DUMP_HEADER_T  dumpHeader;
    UINT16 frameLength;
    // CRC for the sent message
    UINT8 sendCrc;
    UINT8 tmp;
    UINT32 i;
    UINT32 size;

    // Frame length is the payload length + CRC. Payload is dump header + number of
    // read bytes. Read bytes is given by the command.
    frameLength = sizeof(BOOT_UART_MONITOR_DUMP_HEADER_T) + length + 1;


    frameHeader.startPattern = SXS_RMT_START_FRM;
    frameHeader.frameLengthMSB = (frameLength & (0xFF00))>>8;
    frameHeader.frameLengthLSB = (frameLength & (0x00FF));
    frameHeader.flowId = SXS_DUMP_RMC;

    sendCrc = frameHeader.flowId;

    dumpHeader.id = _HAL;
    dumpHeader.size = length;
    dumpHeader.format[0] = ' ';
    dumpHeader.format[1] = ' ';
    dumpHeader.format[2] = ' ';
    dumpHeader.format[3] = 0;
    dumpHeader.address = data;

    if ((access != 1) && (access != 2)
            && (access != 4))
    {
        access = 1;
    }

    size = length / access;

    // Configure format field.
    if (access == 1)
    {
        dumpHeader.format[0] = '%';
        dumpHeader.format[1] = '2';
        dumpHeader.format[2] = 'x';
    }
    else if (access == 2)
    {
        dumpHeader.format[0] = '%';
        dumpHeader.format[1] = '4';
        dumpHeader.format[2] = 'x';
    }
    else if (access == 4)
    {
        dumpHeader.format[0] = '%';
        dumpHeader.format[1] = '8';
        dumpHeader.format[2] = 'x';
    }

    // Check the size of the packet, if not a multiple of "access mode", error.
    if (length % access != 0)
    {
        // FIXME Use a real error message.
        return BOOT_UM_ERR_QTY;
    }

    // Send frame header and payload header
    // CRC is calculated only with data
    if (BOOT_UM_ERR_NO != boot_HstUartSend((UINT8*) &frameHeader,
                                           sizeof(BOOT_UART_MONITOR_FRAME_HEADER_T), &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    if (BOOT_UM_ERR_NO != boot_HstUartSend((UINT8*) &dumpHeader,
                                           sizeof(BOOT_UART_MONITOR_DUMP_HEADER_T), &sendCrc))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }

    switch (access)
    {
        case 1:
        {
            // Byte access
            if (BOOT_UM_ERR_NO != boot_HstUartSend(data, size, &sendCrc))
            {
                return BOOT_UM_ERR_TX_FAILED; // Send ERROR
            }
        }
        break;

        case 2:
        {
            // Double access
            UINT16* address = (UINT16*) ((UINT32)data & ~0x1);
            UINT8   value[2];

            for (i = 0; i<size; i++)
            {
                *((UINT16*)value) = *address++ ;
                if (BOOT_UM_ERR_NO != boot_HstUartSend(value, 2, &sendCrc))
                {
                    return BOOT_UM_ERR_TX_FAILED; // Send ERROR
                }
            }
        }
        break;

        case 4:
        {
            // Word access
            UINT32* address = (UINT32*) ((UINT32)data & ~0x3);
            UINT8   value[4];

            for (i=0; i<size; i++)
            {
                *((UINT32*)value) = *address++ ;
                if (BOOT_UM_ERR_NO != boot_HstUartSend(value, 4, &sendCrc))
                {
                    return BOOT_UM_ERR_TX_FAILED; // Send ERROR
                }
            }
        }
        break;
    }

    // Last byte is the CRC
    if (BOOT_UM_ERR_NO != boot_HstUartSend(&sendCrc, 1, &tmp))
    {
        return BOOT_UM_ERR_TX_FAILED; // Send ERROR
    }
    return BOOT_UM_ERR_NO;
}

#endif


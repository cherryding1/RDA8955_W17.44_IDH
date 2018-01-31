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



#ifndef _BOOT_HOST_H_
#define _BOOT_HOST_H_

/// @defgroup host BOOT Host Port Driver/Client
/// That group describes briefly the exchange media available
/// betweem BOOT and a Host (Personal Computer, Mac, etc)
/// connected through the Host Port.
///
/// @{
///

#ifndef CT_ASM

#include "cs_types.h"


// =============================================================================
// BOOT_HST_HOST_RATE
// -----------------------------------------------------------------------------
/// Rate of the Host communication through the Host Uart.
// =============================================================================
#define BOOT_HST_HOST_RATE  (921600)

// =============================================================================
// BOOT_HST_SYSTEM_CLOCK
// -----------------------------------------------------------------------------
/// System clock at boot while doing host monitoring.
// =============================================================================
#define BOOT_HST_SYSTEM_CLOCK  (26000000)






// =============================================================================
// BOOT_HST_CMD_T
// -----------------------------------------------------------------------------
/// Possible commands received from the Host
/// given to a callback function
// =============================================================================
typedef enum
{
    /// No Command
    BOOT_HST_NO_COMMAND        = 0x00,

    /// Release the Host Clock resource
    BOOT_HST_CLK_REL           = 0xc0,
    /// Set the Host Clock resource as active
    BOOT_HST_CLK_REQ           = 0xc1,

    /// Enter calib daemon
    BOOT_HST_CALIB_ENTER       = 0xed,
    /// Enter in monitor.
    BOOT_HST_MONITOR_ENTER     = 0xee,
    /// Execute command immediatly - under IRQ.
    BOOT_HST_MONITOR_X_IRQ_CMD = 0xef,

    /// Execute extended command.
    BOOT_HST_MONITOR_X_EXT_CMD = 0xfc,
    /// Start the monitor command - monitor only command.
    BOOT_HST_MONITOR_START_CMD = 0xfd,
    /// Exit/end/esta verde command - monitor only command.
    BOOT_HST_MONITOR_END_CMD   = 0xfe,
    /// Execute command - also used in idle.
    BOOT_HST_MONITOR_X_CMD     = 0xff

} BOOT_HST_CMD_T;


// =============================================================================
// BOOT_HST_CMD_T
// -----------------------------------------------------------------------------
/// Possible status returned by the target to the host, through the P2H_STATUS
/// register (Or an emulation of it.)
// =============================================================================
typedef enum
{
    /// No status
    BOOT_HST_STATUS_NONE                = 0x00,

    /// In the monitor loop, waiting for a command on any monitor.
    BOOT_HST_STATUS_WAITING_FOR_COMMAND = 0xaa

} BOOT_HST_STATUS_T;


// =============================================================================
// BOOT_HST_ACK_MASK
// -----------------------------------------------------------------------------
/// Mask to build events for HOST
// =============================================================================
#define BOOT_HST_ACK_MASK 0xff000000


// =============================================================================
// BOOT_HST_EVT_T
// -----------------------------------------------------------------------------
/// Those are predefined events to do (asynchronous ?) communication
/// with the host. Other ack should start with the #BOOT_HST_ACK_MASK bit
/// set
// =============================================================================
typedef enum
{
    BOOT_HST_MONITOR_START       = 0xff000001,
    /// Invalid, unknown command received
    BOOT_HST_UNSUPPORTED_CMD     = 0xff000002,
    /// End of monitor
    BOOT_HST_MONITOR_END         = 0xff000003,
    /// Start of command handling
    BOOT_HST_START_CMD_TREATMENT = 0xff000004,
    /// End of command handling
    BOOT_HST_END_CMD_TREATMENT   = 0xff000005, // 0xfe, with payload in the rest of the event ?

} BOOT_HST_EVT_T;


// =============================================================================
// BOOT_HST_MONITOR_X_CTX_T
// -----------------------------------------------------------------------------
/// This structure is  used by the HOST execution command
/// It can store PC, SP, param ptr and returned value ptr
/// The command type field could reveal itself as really
/// relevant in the future
// =============================================================================
typedef volatile struct
{
    UINT32 cmdType;         // 0x00000000
    UINT32 pc;              // 0x00000004
    UINT32 sp;              // 0x00000008
    VOID*  param;           // 0x0000000c
    VOID*  returnedValue;   // 0x00000010
} BOOT_HST_MONITOR_X_CTX_T;


// =============================================================================
// BOOT_HST_MONITOR_EXTENDED_HANDLER_T
// -----------------------------------------------------------------------------
/// Extension handler (HstCmdExtendedHandler)
// =============================================================================
typedef volatile struct
{
    VOID* ExtendedCmdHandler; // 0x00000000
} BOOT_HST_MONITOR_EXTENDED_HANDLER_T;


// =============================================================================
// BOOT_HST_EXECUTOR_T
// -----------------------------------------------------------------------------
// Execution context used by the target executor.
// =============================================================================
typedef struct
{
    BOOT_HST_MONITOR_X_CTX_T hstMonitorXCtx;
    BOOT_HST_MONITOR_EXTENDED_HANDLER_T hstMonitorExtendedHandler;
} BOOT_HST_EXECUTOR_T;


// =============================================================================
// boot_TimeDelay
// -----------------------------------------------------------------------------
/// This function is to be used when we want to wait for a delay.
/// Attention must be paid to the fact that this is a blocking function
/// and that it could be avoided as possible, and OS function should be
/// used instead. But sometimes, we cannot use the OS functions ...
///
/// @param delay The delay to wait, in number of 16 kHz periods.
// =============================================================================
PROTECTED VOID boot_TimeDelay(UINT32 ms);

// =============================================================================
// boot_TimGetUpTime
// -----------------------------------------------------------------------------
/// Return the up time of the system in 16384Hz ticks.
/// The uptime is given as a 32 bits numbers, therefore the user of the
/// function must take care of the count wrap, that will occurs every
/// three days.
///
/// @return Up time, in 16384Hz ticks.
// =============================================================================
PROTECTED UINT32  boot_TimGetUpTime(VOID);
// boot_HstCmdBasicHandler
// -----------------------------------------------------------------------------
/// That function handles execution requests from the host
/// It uses a BOOT_HST_MONITOR_X_CTX_T structure previously set by the
/// host to execute commands.
// =============================================================================
PUBLIC VOID boot_HstCmdBasicHandler(VOID);



EXPORT PROTECTED volatile BOOT_HST_MONITOR_X_CTX_T boot_HstMonitorXCtx;

EXPORT PROTECTED volatile BOOT_HST_MONITOR_EXTENDED_HANDLER_T boot_HstMonitorExtendedHandler;

PROTECTED VOID boot_HstMonitorX(VOID);


#else

#define BOOT_HST_MONITOR_X_CTX_CMD_TYPE  0x00000000
#define BOOT_HST_MONITOR_X_CTX_PC        0x00000004
#define BOOT_HST_MONITOR_X_CTX_SP        0x00000008
#define BOOT_HST_MONITOR_X_CTX_PARAM     0x0000000c
#define BOOT_HST_MONITOR_X_CTX_RETURNED_VALUE     0x00000010

#define BOOT_HST_MONITOR_EXTENDED_HANDLER_EXTENDED_COMMAND_HANDLER 0x00000014

#endif



// @} // <-- End of the host group.

#endif





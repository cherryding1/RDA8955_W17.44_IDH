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


#ifndef _BOOT_USB_MONITOR_H_
#define _BOOT_USB_MONITOR_H_


#include "cs_types.h"
#include "boot_usb.h"
#include "chip_id.h"
#include "boot.h"

// =============================================================================
//  MACROS
// =============================================================================
#define BOOT_HOST_USB_FIFO_READ_SIZE             (32)
#define BOOT_HOST_USB_BUFFER_OUT_SIZE            (2048)
#define BOOT_HOST_USB_BUFFER_IN_SIZE             (1032)

// ============================================================================
// BOOT_HOST_USB_STATE_T
// -----------------------------------------------------------------------------
/// Out Ep state
// =============================================================================
typedef enum
{
    BOOT_HOST_USB_STATE_IDLE                    = 0x00000000,
    BOOT_HOST_USB_STATE_HEADER                  = 0x00000001,
    BOOT_HOST_USB_STATE_DATA                    = 0x00000002,
    BOOT_HOST_USB_STATE_SXS_CMD                 = 0x00000003,
    BOOT_HOST_USB_STATE_SXS_ADDR                = 0x00000004,
    BOOT_HOST_USB_STATE_SXS_DATA                = 0x00000005,
    BOOT_HOST_USB_STATE_SXS_RID                 = 0x00000006,
    BOOT_HOST_USB_STATE_SXS_READ_SIZE           = 0x00000007
} BOOT_HOST_USB_STATE_T;


// ============================================================================
// BOOT_HOST_USB_CMD_ACCESS_T
// -----------------------------------------------------------------------------
/// Enum access value
// =============================================================================
typedef enum
{
    BOOT_HOST_USB_CMD_ACCESS_BYTE               = 0x00000000,
    BOOT_HOST_USB_CMD_ACCESS_HALF_WORD          = 0x00000001,
    BOOT_HOST_USB_CMD_ACCESS_WORD               = 0x00000002,
    BOOT_HOST_USB_CMD_ACCESS_BLOCK              = 0x00000003
} BOOT_HOST_USB_CMD_ACCESS_T;


// ============================================================================
// BOOT_HOST_USB_RECV_STATE_T
// -----------------------------------------------------------------------------
/// Recv Task state
// =============================================================================
typedef enum
{
    BOOT_HOST_USB_RECV_STATE_RUN                = 0x00000000,
    BOOT_HOST_USB_RECV_STATE_WAIT               = 0x00000001
} BOOT_HOST_USB_RECV_STATE_T;


// ============================================================================
// BOOT_HOST_USB_MODE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    BOOT_HOST_USB_MODE_BOOT                     = 0x00000001,
    BOOT_HOST_USB_MODE_SX                       = 0x00000002
} BOOT_HOST_USB_MODE_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// BOOT_HOST_USB_CMD_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT8                          access:2;
    UINT8                          internal:1;
    UINT8                          reserved:4;
    UINT8                          rw:1;
} BOOT_HOST_USB_CMD_T;
//noname
#define BOOT_HOST_USB_CMD_T_ACCESS(n) (((n)&3)<<0)
#define BOOT_HOST_USB_CMD_T_INTERNAL (1<<2)
#define BOOT_HOST_USB_CMD_T_RESERVED(n) (((n)&15)<<3)
#define BOOT_HOST_USB_CMD_T_RW      (1<<7)



// ============================================================================
// BOOT_HOST_USB_MONITOR_FIFO_ELEM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT32                         Addr;                         //0x00000000
    UINT8                          Rid;                          //0x00000004
    UINT16                         Size;                         //0x00000006
} BOOT_HOST_USB_MONITOR_FIFO_ELEM_T; //Size : 0x8



// ============================================================================
// BOOT_HOST_USB_MONITOR_BUFFER_IN_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    UINT8                          Buffer[BOOT_HOST_USB_BUFFER_IN_SIZE]; //0x00000000
} BOOT_HOST_USB_MONITOR_BUFFER_IN_T; //Size : 0x408



// ============================================================================
// BOOT_HOST_USB_MONITOR_CTX_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    BOOT_HOST_USB_MODE_T           MonitorMode;                  //0x00000000
    UINT8                          BufferOut[BOOT_HOST_USB_BUFFER_OUT_SIZE]; //0x00000004
    BOOT_HOST_USB_MONITOR_BUFFER_IN_T BufferIn[2];               //0x00000804
    BOOT_HOST_USB_MONITOR_FIFO_ELEM_T Fifo[BOOT_HOST_USB_FIFO_READ_SIZE]; //0x00001014
} BOOT_HOST_USB_MONITOR_CTX_T; //Size : 0x1114



// ============================================================================
// BOOT_HOST_USB_MONITOR_VAR_T
// -----------------------------------------------------------------------------
/// Global variable of usb monitor
// =============================================================================
typedef struct
{
    BOOT_HOST_USB_RECV_STATE_T     RecvState;                    //0x00000000
    BOOT_HOST_USB_STATE_T          State;                        //0x00000004
    UINT16                         DataSize;                     //0x00000008
    /// Remaining from a being received USB packet.
    UINT16                         Residue;                      //0x0000000A
    UINT32                         Addr;                         //0x0000000C
    BOOT_HOST_USB_CMD_T            Cmd;                          //0x00000010
    UINT8                          ReadPt;                       //0x00000011
    UINT8                          WritePt;                      //0x00000012
    /// Index in the swap buffer for data to send to the PC.
    UINT8                          CurrentBufferIn;              //0x00000013
    /// Position in on the buffer of data to send to the PC.
    UINT16                         PositionBufferIn;             //0x00000014
    BOOT_HOST_USB_MONITOR_CTX_T*   Context;                      //0x00000018
} BOOT_HOST_USB_MONITOR_VAR_T; //Size : 0x1C

//Cmd
#define BOOT_HOST_USB_MONITOR_VAR_T_ACCESS(n) (((n)&3)<<0)
#define BOOT_HOST_USB_MONITOR_VAR_T_INTERNAL (1<<2)
#define BOOT_HOST_USB_MONITOR_VAR_T_RESERVED(n) (((n)&15)<<3)
#define BOOT_HOST_USB_MONITOR_VAR_T_RW (1<<7)



// ============================================================================
// BOOT_USB_MONITOR_GLOBALS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
} BOOT_USB_MONITOR_GLOBALS_T; //Size : 0x0






// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

EXPORT PUBLIC CONST HAL_USB_INTERFACE_DESCRIPTOR_T boot_HostUsbInterface[2];

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// BOOT_HOST_USB_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback for decoding trame
// =============================================================================
typedef VOID (*BOOT_HOST_USB_CALLBACK_T)(VOID *buffer, UINT16 size);

// =============================================================================
// BOOT_HOST_USB_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call when a command is write in internal H2PSTATUS register
// =============================================================================
typedef VOID (*BOOT_HOST_USB_CALLBACK_H2PSTATUS_T)(UINT32 value);

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// boot_HostUsbOpen
// -----------------------------------------------------------------------------
/// Configure USB host
/// @param context Define the context use. The context containt read request
/// fifo and buffer for read and write on USB
/// @param callback This callback is use to change the parsing process of host
/// protocol
// =============================================================================
PUBLIC VOID boot_HostUsbOpen (BOOT_HOST_USB_MONITOR_CTX_T*       context,
                              BOOT_HOST_USB_CALLBACK_T           callback,
                              BOOT_HOST_USB_CALLBACK_H2PSTATUS_T callbackH2pstatus);

// =============================================================================
// boot_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb (disable USB interrupt)
// =============================================================================
PUBLIC VOID  boot_HostUsbClose(VOID);

// =============================================================================
// boot_HostUsbSend
// -----------------------------------------------------------------------------
/// Generate packet must be send and send it
/// Execute the read commands.
// =============================================================================
PUBLIC UINT8 boot_HostUsbSend (VOID);

// =============================================================================
// boot_HostUsbRecv
// -----------------------------------------------------------------------------
/// Receive USB packet and parse it
///  - write command executed here.
///  - execute command are executed here.
///  - read command are put in a fifo for boot_HostUsbSend usage.
// =============================================================================
PUBLIC BOOT_MONITOR_OP_STATUS_T boot_HostUsbRecv (VOID);

// =============================================================================
// boot_HostUsbClockEnable
// -----------------------------------------------------------------------------
/// Enable usb clock
// =============================================================================
PUBLIC VOID  boot_HostUsbClockEnable(VOID);

// =============================================================================
// boot_HostUsbInit
// -----------------------------------------------------------------------------
/// Init global variables of host usb
// =============================================================================
PUBLIC VOID boot_HostUsbInit(VOID);

// =============================================================================
// boot_HostUsbDeviceCallback
// -----------------------------------------------------------------------------
/// Callback use to define the host usb descriptor. It's call by USB driver
// =============================================================================
PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T* boot_HostUsbDeviceCallback(BOOL speedEnum);

// =============================================================================
// boot_HostUsbDeviceCallback
// -----------------------------------------------------------------------------
/// Callback use to define the host usb uart descriptor. It's call by USB driver
// =============================================================================
PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T* boot_HostUsbUartDeviceCallback(BOOL speedEnum);

// =============================================================================
// boot_HostUsbEvent
// -----------------------------------------------------------------------------
/// Send USB host event with a timeout
// =============================================================================
PUBLIC VOID boot_HostUsbEvent(UINT32 event);


#endif


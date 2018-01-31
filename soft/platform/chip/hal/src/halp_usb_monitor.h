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


#ifndef _HALP_USB_MONITOR_H_
#define _HALP_USB_MONITOR_H_


#include "cs_types.h"
#include "hal_usb.h"
#include "hal_usb_monitor.h"

// =============================================================================
//  MACROS
// =============================================================================

typedef enum
{
    HAL_HOST_USB_STATE_IDLE                     = 0x00000000,
    HAL_HOST_USB_STATE_HEADER                   = 0x00000001,
    HAL_HOST_USB_STATE_DATA                     = 0x00000002,
    HAL_HOST_USB_STATE_SXS_CMD                  = 0x00000003,
    HAL_HOST_USB_STATE_SXS_ADDR                 = 0x00000004,
    HAL_HOST_USB_STATE_SXS_WINT                 = 0x00000005,
    HAL_HOST_USB_STATE_SXS_W8                   = 0x00000006,
    HAL_HOST_USB_STATE_SXS_W16                  = 0x00000007,
    HAL_HOST_USB_STATE_SXS_W32                  = 0x00000008,
    HAL_HOST_USB_STATE_SXS_WDATA                = 0x00000009,
    HAL_HOST_USB_STATE_SXS_RID                  = 0x0000000A,
    HAL_HOST_USB_STATE_SXS_READ_SIZE            = 0x0000000B
} HAL_HOST_USB_STATE_T;


typedef enum
{
    HAL_HOST_USB_CMD_ACCESS_BYTE                = 0x00000000,
    HAL_HOST_USB_CMD_ACCESS_HALF_WORD           = 0x00000001,
    HAL_HOST_USB_CMD_ACCESS_WORD                = 0x00000002,
    HAL_HOST_USB_CMD_ACCESS_BLOCK               = 0x00000003
} HAL_HOST_USB_CMD_ACCESS_T;


typedef enum
{
    HAL_HOST_USB_RECV_STATE_RUN                 = 0x00000000,
    HAL_HOST_USB_RECV_STATE_WAIT                = 0x00000001
} HAL_HOST_USB_RECV_STATE_T;



// =============================================================================
//  TYPES
// =============================================================================
// ============================================================================
// HAL_HOST_USB_CMD_T
// -----------------------------------------------------------------------------
///
// =============================================================================


typedef struct
{
    UINT8                          access:2;
    UINT8                          internal:1;
    UINT8                          reserved:4;
    UINT8                          rw:1;
} HAL_HOST_USB_CMD_T;
//noname
#define HAL_HOST_USB_CMD_T_ACCESS(n) (((n)&3)<<0)
#define HAL_HOST_USB_CMD_T_INTERNAL (1<<2)
#define HAL_HOST_USB_CMD_T_RESERVED(n) (((n)&15)<<3)
#define HAL_HOST_USB_CMD_T_RW       (1<<7)


// ============================================================================
// HAL_HOST_USB_MONITOR_VAR_T
// -----------------------------------------------------------------------------
/// Global variable of usb monitor
// =============================================================================


typedef struct
{
    HAL_HOST_USB_RECV_STATE_T      RecvState;                    //0x00000000
    HAL_HOST_USB_STATE_T           State;                        //0x00000004
    UINT16                         DataSize;                     //0x00000008
    UINT16                         Residue;                      //0x0000000A
    UINT32                         Addr;                         //0x0000000C
    HAL_HOST_USB_CMD_T             Cmd;                          //0x00000010
    UINT8                          ReadPt;                       //0x00000011
    UINT8                          WritePt;                      //0x00000012
    UINT8                          CurrentBufferIn;              //0x00000013
    UINT16                         PositionBufferIn;             //0x00000014
    HAL_HOST_USB_MONITOR_CTX_T*    Context;                      //0x00000018
} HAL_HOST_USB_MONITOR_VAR_T; //Size : 0x1C

//Cmd
#define HAL_HOST_USB_MONITOR_VAR_T_ACCESS(n) (((n)&3)<<0)
#define HAL_HOST_USB_MONITOR_VAR_T_INTERNAL (1<<2)
#define HAL_HOST_USB_MONITOR_VAR_T_RESERVED(n) (((n)&15)<<3)
#define HAL_HOST_USB_MONITOR_VAR_T_RW (1<<7)



// ============================================================================
// HAL_USB_MONITOR_GLOBALS_T
// -----------------------------------------------------------------------------
///
// =============================================================================


typedef struct
{
} HAL_USB_MONITOR_GLOBALS_T; //Size : 0x0






// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// boot_HostUsbOpen
// -----------------------------------------------------------------------------
/// Configure USB host
// =============================================================================
PUBLIC HAL_USB_INTERFACE_DESCRIPTOR_T* hal_HostUsbOpen(VOID);

// =============================================================================
// boot_HostUsbClose
// -----------------------------------------------------------------------------
/// Close host usb
// =============================================================================
PUBLIC VOID  hal_HostUsbClose(VOID);

// ============================================================================
// hal_HostUsbSend
// -----------------------------------------------------------------------------
/// Poll the output data in host
///
/// @return 1 if new data is send
// =============================================================================
PUBLIC UINT8 hal_HostUsbSend (VOID);

// =============================================================================
// boot_HostUsbRecv
// -----------------------------------------------------------------------------
/// Receive USB packet and parse it
///
/// @return 1 if new data is receive
// =============================================================================
PUBLIC UINT8 hal_HostUsbRecv (VOID);

// ============================================================================
// hal_HostUsbEvent
// -----------------------------------------------------------------------------
/// Send event on usb host
///
/// @param event Event number, you want send
// =============================================================================
PUBLIC VOID hal_HostUsbEvent(UINT32 event);

// =============================================================================
// hal_HostUsbDeviceCallback
// -----------------------------------------------------------------------------
/// Callback use to define the host usb descriptor. It's call by USB driver
// =============================================================================
PUBLIC HAL_USB_DEVICE_DESCRIPTOR_T* hal_HostUsbDeviceCallback(BOOL speedEnum);


#endif

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

#ifndef _USBMSCP_
#define _USBMSCP_

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************
#include "usb.h"
#include "usbmsc.h"
#include "usb_hcd.h"

//*****************************************************************************
// D E F I N E S
//*****************************************************************************

#define USBMSC_SIG 'UMSC'

#define MAX_USB_BULK_LENGTH 0x2000L // 8k

// default timeout values   ,  in ms
#define RESET_TIMEOUT           4000
#define COMMAND_BLOCK_TIMEOUT   4000
#define COMMAND_STATUS_TIMEOUT  4000


#define ONE_FRAME_PERIOD        (16384)

// TBD: may want this in the registry
#define MAX_INT_RETRIES 3


// Is the context pointer valid
//&& USBMSC_SIG == p->Sig
#define MSC_VALID_CONTEXT( p ) \
   ( (p != NULL) )

// Can the device accept any I/O requests
#define MSC_ACCEPT_IO( p ) \
   ( MSC_VALID_CONTEXT( p ) && \
     p->Flags.AcceptIo && \
    !p->Flags.DeviceRemoved )

//*****************************************************************************
// T Y P E D E F S
//*****************************************************************************
extern UINT32 g_NumDevices;

//
// Device Flags
//
typedef struct _MSCDEVICE_FLAGS
{
    //
    // The device is initialized and ready to accept I/O
    //
    UINT8   AcceptIo : 1;

    //
    // The device has been removed
    //
    UINT8   DeviceRemoved : 1;

    //
    // The device claims CBIT protocol,
    // but does not use the Interrupt pipe
    //
    UINT8   IgnoreInterrupt : 1;

} MSCDEVICE_FLAGS, *PMSCDEVICE_FLAGS;


// Our notion of a Pipe
typedef struct _PIPE
{
    // USB Pipe handle received from the stack
    USB_PIPE hPipe;

    // Endpoint's Address
    UINT8    bIndex;

    // Endpoint's wMaxPacketSize
    UINT16 wMaxPacketSize;
} PIPE, *PPIPE;


// Reg configurable Timeouts
typedef struct _MSCTIMEOUTS
{

    UINT32 Reset;

    UINT32 CommandBlock;

    UINT32 CommandStatus;

} MSCTIMEOUTS, *PMSCTIMEOUTS;


typedef struct _USBMSC_DEVICE
{
    // device context Signature
    UINT32 Sig;

    HANDLE  Lock;

    // USB handle to our device
    HANDLE   hUsbDevice;
    UINT32    Index;

    // Fields from USB_INTERFACE that we need
    USBINTERFACEDESC * pUsbInterface;

    // USB Configutation Index
    UINT16 ConfigIndex;

    // Bulk OUT Pipe
    PIPE BulkOut;

    // Bulk IN Pipe
    PIPE BulkIn;

    // Interrupt Pipe
    PIPE  Interrupt;
    UINT8 InterruptErrors;

    // completion signal used for endpoint 0
    HANDLE hEP0Event;

    // Multi-Lun Support.
    UINT32 dwMaxLun;
    void* DiskContext[MAX_LUN];

    // FLAGS
    MSCDEVICE_FLAGS Flags;

    MSCTIMEOUTS Timeouts;

    UINT32 dwCurTag;

} USBMSC_DEVICE, *PUSBMSC_DEVICE;



#endif // _USBMSCP_

// EOF

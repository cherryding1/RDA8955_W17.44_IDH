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



#ifndef _USBHOST_USB_H_
#define _USBHOST_USB_H_

#include "cs_types.h"

/// @file USBHOST_usb.h

/// This document describes the characteristics of the  USB Disk
/// Driver and how to use it via its API.

#ifdef __cplusplus
extern "C"
{
#endif

#define USBHOSTUSB_SUCCESS  0x00000000
#define USBHOSTUSB_FAILED   0x00000001

typedef enum
{
    USBHOST_ERR_NO = 0,
    USBHOST_ERR_CARD_TIMEOUT = 1,
    USBHOST_ERR_DMA_BUSY = 3,
    USBHOST_ERR_CSD = 4,
    USBHOST_ERR_SPI_BUSY = 5,
    USBHOST_ERR_BLOCK_LEN = 6,
    USBHOST_ERR_CARD_NO_RESPONSE,
    USBHOST_ERR_CARD_RESPONSE_BAD_CRC,
    USBHOST_ERR_CMD,
    USBHOST_ERR_UNUSABLE_CARD,
    USBHOST_ERR_NO_CARD,
    USBHOST_ERR_NO_HOTPLUG,

    /// A general error value
    USBHOST_ERR,
} USBHOST_ERR_T;


typedef enum
{
    // Card present and mcd is open
    USBHOST_STATUS_OPEN,
    // Card present and mcd is not open
    USBHOST_STATUS_NOTOPEN_PRESENT,
    // Card not present
    USBHOST_STATUS_NOTPRESENT,
    // Card removed, still open (please close !)
    USBHOST_STATUS_OPEN_NOTPRESENT
} USBHOST_STATUS_T ;
// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// USBHOST_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================

typedef VOID (*USBHOST_CARD_DETECT_HANDLER_T)(BOOL cardPlugged);


typedef struct tagUSBClientDriver
{
    int InterfaceClass;
    int InterfaceSubClass;
    void (*UsbDeviceAttach)(void *configuration, uint8 index);
    void (*UsbDeviceDetach)(void*);
} USBCLIENTDRIVER;

int USBHOST_RegisterClientDriver(USBCLIENTDRIVER *pdriverInst);


#ifdef __cplusplus
};
#endif



/// @} <- End of the mmc group

#endif // _USBHOST_USB_H_


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


#ifndef _BOOT_USB_H_
#define _BOOT_USB_H_

#include "cs_types.h"
#include "chip_id.h"

#include "hal_usb.h"

#if CHIP_HAS_USB == 1

// =============================================================================
// MACROS
// =============================================================================



#define USB_DMA_SRC_ADDRESS(n)         (((n)&0xFFFFFFF)<<0)
// =============================================================================
// BOOT_USB_PRODUCT_ID
// -----------------------------------------------------------------------------
/// Define the USB product Id. This is a 16 bits numbers constructed this way:
/// - 15-8: Chip CT_ASIC_ID identifier.
/// - 5-7 : Usb mode. Needed so that windows detect properly different devices
/// for the different functionalities implemented through USB (Trace, mass
/// storage etc ...)
/// - 3-4 : Reserved
/// - 0-2 : Chip's BOND_ID.
// =============================================================================
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#define BOOT_USB_PRODUCT_ID(usbMode)  ((CHIP_ASIC_ID_GALLITE << 8) | (((usbMode)&0x07) << 5) | GET_BITFIELD(hwp_configRegs->CHIP_ID, CFG_REGS_BOND_ID))
#else
#define BOOT_USB_PRODUCT_ID(usbMode)  ((CHIP_ASIC_ID << 8) | (((usbMode)&0x07) << 5) | GET_BITFIELD(hwp_configRegs->CHIP_ID, CFG_REGS_BOND_ID))
#endif

#ifndef BOOT_VERSION_REVISION
//#define BOOT_VERSION_REVISION HAL_VERSION_REVISION
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef USB_WIFI_SUPPORT
#define BOOT_VERSION_REVISION 0x1e35
#else
#define BOOT_VERSION_REVISION 0x1210
#endif
#elif (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
#define BOOT_VERSION_REVISION 0x3410
#else
#define BOOT_VERSION_REVISION 0x847b
#endif
#endif


// =============================================================================
// BOOT_USB_REVISION_ID
// -----------------------------------------------------------------------------
/// Revision Id. It is defined as the software database version number at the
/// time of the compilation of the USB driver. In case the USB driver is not,
/// the romed one, but compiled as a part of HAL. (Flag
/// #HAL_USB_DRIVER_DONT_USE_ROMED_CODE defined.) the revision number used
/// is the HAL one, which is coherent as both software modules (boot and HAL)
/// are versioned in the same database.
// =============================================================================
#define BOOT_USB_REVISION_ID (BOOT_VERSION_REVISION)


// =============================================================================
// BOOT_USB_VENDOR_ID
// -----------------------------------------------------------------------------
/// Coolsand USB vendor Id.
// =============================================================================
#ifndef USB_WIFI_SUPPORT
#define BOOT_USB_VENDOR_ID      0x1E04
#else
#define BOOT_USB_VENDOR_ID      0x045E
#endif

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// boot_UsbOpen
// -----------------------------------------------------------------------------
/// This function enable USB
///
/// @param callback See #HAL_USB_GETDESCRIPTOR_CALLBACK_T
// =============================================================================
PUBLIC VOID boot_UsbOpen(HAL_USB_GETDESCRIPTOR_CALLBACK_T callback);

// =============================================================================
// hal_UsbClose
// -----------------------------------------------------------------------------
/// This function disable USB
// =============================================================================
PUBLIC VOID boot_UsbClose(VOID);

// =============================================================================
// hal_UsbReset
// -----------------------------------------------------------------------------
/// This function is use to reset USB and/or change configuration of usb
// =============================================================================
PUBLIC VOID boot_UsbReset(VOID);


// =============================================================================
// hal_UsbRecv
// -----------------------------------------------------------------------------
/// Recv an usb packet
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param buffer Pointer on the data buffer to be send.
/// @param size Number of bytes to be send.
/// @param flag No flag you must set this parameter to zero
/// @return Number of sent bytes or -1 if error
// =============================================================================
PUBLIC INT32 boot_UsbRecv(UINT8 ep, UINT8* buffer, UINT16 size, UINT32 flag);

// =============================================================================
// hal_UsbSend
// -----------------------------------------------------------------------------
/// Send an usb packet
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param buffer Pointer on the date buffer to be send.
/// @param size Number of bytes to be send.
/// @param flag For partial send set the flag to 1
/// @return Number of sent bytes or -1 if error
// =============================================================================
PUBLIC INT32 boot_UsbSend(UINT8 ep, UINT8* buffer, UINT16 size, UINT32 flag);

// =============================================================================
// hal_UsbEpStall
// -----------------------------------------------------------------------------
/// Stall an End point
/// @param ep Define the endpoint index for the direction
/// use #HAL_USB_EP_DIRECTION_IN and use #HAL_USB_EP_DIRECTION_OUT
/// @param stall \c TRUE Set EndPoint Stall
///              \c FALSE Clear EndPoint Stall
// =============================================================================
PUBLIC VOID boot_UsbEpStall(UINT8 ep, BOOL stall);

// =============================================================================
// hal_UsbCompletedCommand
// -----------------------------------------------------------------------------
/// This function must be call for completed an delayed EP0 command (when you
/// return #HAL_USB_CALLBACK_RETURN_RUNNING on the interface or EP callback)
// =============================================================================
PUBLIC VOID boot_UsbCompletedCommand(VOID);

// =============================================================================
// hal_UsbEnableInterrupt
// -----------------------------------------------------------------------------
/// Enable or disable interrupt for an specific EndPoint. The default value
/// after initialisation is true
/// @param ep Number of Endpoint
/// @param enable \c TRUE Enable the interrupt
///               \c FALSE Disable the interrupt
// =============================================================================
PUBLIC VOID boot_UsbEpEnableInterrupt(UINT8 ep, BOOL enable);

// =============================================================================
// hal_UsbEpRxTransferedSize
// -----------------------------------------------------------------------------
/// This function return number of bytes send on the last transfert
/// @param ep Number of Endpoint
/// @return Size transfered
// =============================================================================
PUBLIC UINT16 boot_UsbEpRxTransferedSize(UINT8 ep);

// =============================================================================
// hal_UsbEpTransfertDone
// -----------------------------------------------------------------------------
/// This function indicate if an transfert is finish.
/// @param ep Number of Endpoint
/// @return return the end transfert status
// =============================================================================
PUBLIC BOOL boot_UsbEpTransfertDone(UINT8 ep);


PUBLIC VOID boot_UsbIrqHandler(UINT8 interruptId);

// =============================================================================
// hal_UsbInitVar
// -----------------------------------------------------------------------------
/// Initialize global variable of driver
// =============================================================================
PUBLIC VOID boot_UsbInitVar(VOID);

#endif /* CHIP_HAS_USB == 1 */

#endif // _BOOT_USB_H_

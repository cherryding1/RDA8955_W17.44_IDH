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


// The purpose of the following is to avoid the inclusion of big symbols in
// the final flash programmer lodfile.


// This bcpu_main replaces the regular bcpu_main present in the platform library
PROTECTED VOID bcpu_main(VOID) { }


//================================================
// EDRV stuff
//================================================

PUBLIC VOID aud_InitEarpieceSetting(VOID) {}
PUBLIC VOID aud_EnableEarpieceSetting(VOID) {}


//================================================
// Calib stuff
//================================================

PUBLIC VOID calib_StubMain(VOID) {}
PUBLIC VOID calib_DaemonInit(VOID) {}
PUBLIC VOID* calib_GetPointers(VOID) { return NULL; }


//================================================
// SX stuff
//================================================

UINT32 sxs_Boot;
PUBLIC VOID sxr_Tampax (VOID) {}
PUBLIC VOID sxs_BackTrace (VOID) {}
PUBLIC VOID sxs_SaveRmtBuffer (VOID) {}
PUBLIC VOID _sxr_StartFunctionTimer(VOID) {}
PUBLIC VOID _sxr_StopFunctionTimer(VOID) {}
PUBLIC VOID sxs_Raise(VOID) {}
PUBLIC VOID sxr_Sleep(VOID) {}
PUBLIC VOID sxs_SetTraceEnabled(VOID) {}

//================================================
// HAL stuff
//================================================

#if (CHIP_HAS_USB == 1)
// USB need a lot of stuff, so stub what can be stubbed
PUBLIC VOID* hal_UsbDescriptorNewCmdEpDescriptor(VOID) { return 0; }
PUBLIC BOOL hal_HostUsbSend(VOID) { return boot_HostUsbRecv(); }
PUBLIC UINT8 hal_HostUsbRecv(VOID) { return boot_HostUsbSend(); }
PUBLIC VOID* hal_UsbDescriptorNewBulkEpDescriptor(VOID) { return NULL; }
PUBLIC VOID* hal_UsbDescriptorNewInterruptEpDescriptor(VOID) { return NULL; }
PUBLIC VOID* hal_UsbDescriptorNewIsoEpDescriptor(VOID) { return NULL; }
PUBLIC VOID* hal_UsbDescriptorNewInterfaceDescriptor(VOID) { return NULL; }
PUBLIC VOID hal_UsbDescriptorCleanInterfaceListDescriptor(VOID) { }
PUBLIC VOID hal_UsbDescriptorCleanDeviceDescriptor(VOID) { }
PUBLIC VOID* hal_UsbDescriptorCopyInterfaceDescriptor(VOID) { return NULL; }
//PUBLIC VOID hal_UsbReset(BOOL reset) { }
//PUBLIC VOID hal_UsbOpen(VOID) { }
PUBLIC VOID* hal_HostUsbDeviceCallback(VOID) { return NULL; }
//PROTECTED VOID hal_UsbHandleIrqInGdb(VOID) { }
PUBLIC VOID* hal_HostUsbOpen(VOID) { return NULL; }
#endif // (CHIP_HAS_USB == 1)

VOID rfd_XcvReadChipId(VOID) {}
UINT32 rfd_XcvGetChipId(VOID) { return 0; }


//================================================
// MISC stuff
//================================================

PUBLIC VOID patch_Open(VOID) {}
PUBLIC VOID pal_Open(VOID) {}


PUBLIC VOID calib_RegisterYourself(VOID) {}
PUBLIC VOID sx_RegisterYourself(VOID) {}
PUBLIC VOID std_RegisterYourself(VOID) {}
PUBLIC VOID rfd_RegisterYourself(VOID) {}
PUBLIC VOID bcpu_RegisterYourself(VOID) {}

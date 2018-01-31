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

#ifndef COOL_PROFILE_BOOT_PROFILE_CODES_H
#define COOL_PROFILE_BOOT_PROFILE_CODES_H


#define CP_boot_UsbIrqHandler 0x0130
#define CP_boot_UsbIrqHandlerI 0x0131
#define CP_boot_UsbIrqHandlerO 0x0132
#define CP_boot_UsbIrqHandlerIcp 0x0133
#define CP_boot_UsbIrqHandlerIto 0x0134
#define CP_boot_UsbIrqHandlerOsp 0x0135
#define CP_boot_UsbIrqHandlerOcp 0x0136
#define CP_boot_UsbIrqHandlerReset 0x0137
#define CP_boot_UsbIrqHandlerEnum 0x0138
#define CP_boot_HostUsbDecode 0x0139
#define CP_boot_HostUsbGenerateReadPacket 0x013A
#define CP_boot_HostUsbSendReadPacket 0x013B
#define CP_boot_HostUsbWriteInternal 0x013C
#define CP_boot_HostUsbInit 0x013D
#define CP_XCPU_BOOT 0x2F00
#define CP_XCPU_MAIN 0x2F01
#define CP_BOOT_START_or_POWER_STATE_DONE 0x2F04
#define CP_BOOT_PU_ST_START_or_ENTER_MONITOR_LOOP 0x2F05
#define CP_BOOT_PU_ST_END_or_USB_TIMEOUT_IN 0x2F06
#define CP_BOOT_CACHE_FLUSHED_or_USB_MONITOR_EVENT_TIMEOUT 0x2F07
#define CP_BOOT_ADMUX 0x2F08
#define CP_BOOT_WRITE_FLASH_MODE 0x2F09
#define CP_BOOT_PU_END_START 0x2F0A
#define CP_BOOT_PU_END_END 0x2F0B
#define CP_BOOT_DIRECT_IN_FLASH 0x2F0C
#define CP_BOOT_EXIT_BOOT 0x2F0D
#define CP_BOOT_POWER_STATE_DONE 0x2F0E
#define CP_BOOT_ENTER_MONITOR_LOOP 0x2F0F
#define CP_BOOT_USB_TIMEOUT_IN 0x2F10
#define CP_BOOT_USB_MONITOR_EVENT_TIMEOUT 0x2F11
#define CP_BOOT_ROMED_XCPU_ERROR 0x2F12
#define CP_BOOT_START 0x2F13
#define CP_BOOT_PU_ST_START 0x2F14
#define CP_BOOT_PU_ST_END 0x2F15
#define CP_BOOT_CACHE_FLUSHED 0x2F16
#define CP_BOOT_USB_REQUEST_DEVICE_SETADDR 0x2F17
#define CP_BOOT_USB_STATUS_OUT 0x2F18
#define CP_BOOT_USB_STATUS_IN 0x2F19



#endif

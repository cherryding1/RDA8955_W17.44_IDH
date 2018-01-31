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


#ifndef _HALP_BOOT_MODE_H_
#define _HALP_BOOT_MODE_H_


#define BOOT_MODE_STD                   0

#define BOOT_MODE_NO_AUTO_PU          (1<<0)
#define BOOT_MODE_FORCE_MONITOR       (1<<1)
#define BOOT_MODE_UART_MONITOR_ENABLE (1<<2)
#define BOOT_MODE_USB_MONITOR_DISABLE (1<<3)
#define BOOT_MODE_NAND_FLASH          (1<<4)
#define BOOT_MODE_ADMUX_OR_NAND_PAGE_2K (1<<5)


#endif // _HALP_BOOT_MODE_H_



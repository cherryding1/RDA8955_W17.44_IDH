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


#ifndef _8809_H_
#define _8809_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'gallite'."
#endif


#include "globals.h"
#include "global_macros.h"
#include "8809_generic_config.h"
#include "bb_ahb_monitor.h"
#include "bb_cp2.h"
#include "bb_ifc.h"
#include "bb_irq.h"
#include "bb_sram.h"
#include "comregs.h"
#include "bcpu_cache.h"
#include "bcpu_tag.h"
#include "bist.h"
#include "cipher.h"
#include "debug_host.h"
#include "debug_host_internals.h"
#include "debug_uart.h"
#include "dma.h"
#include "gouda.h"
#include "mem_bridge.h"
#include "debug_port.h"
#include "gpio.h"
#include "i2c_master.h"
#include "itlv.h"
#include "page_spy.h"
#include "rf_if.h"
#include "rf_spi.h"
#include "sci.h"
#include "spi.h"
#include "sys_ahb_monitor.h"
#include "sys_ctrl.h"
#include "sys_ifc.h"
#include "sys_ifc2.h"
#include "sys_irq.h"
#include "tcu.h"
#include "timer.h"
#include "uart.h"
#include "vitac.h"
#include "xcor.h"
#include "cp0.h"
#include "regdef.h"
#include "xcpu_cache.h"
#include "xcpu_tag.h"
#include "xcpu.h"
#include "keypad.h"
#include "pwm.h"
#include "calendar.h"
#include "aif.h"
#include "usbc.h"
#include "sdmmc.h"
#include "camera.h"
#include "cfg_regs.h"
#include "voc_ram.h"
#include "voc_ahb.h"
#include "voc_cfg.h"
#include "tester_chip.h"
#include "test_uart.h"
#include "memory_burst_adapter.h"

// =============================================================================
//  MACROS
// =============================================================================
/// System APB base
#define REG_SYS_APB_BASE                         (0X01A00000)
/// Baseband APB base
#define REG_BB_APB_BASE                          (0X01900000)
/// VOC AHB base
#define REG_VOC_BASE                             (0X01940000)

// =============================================================================
//  TYPES
// =============================================================================



#endif


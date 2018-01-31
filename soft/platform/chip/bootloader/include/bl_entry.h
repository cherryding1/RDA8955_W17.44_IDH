/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BL_ENTRY_H_
#define _BL_ENTRY_H_

#include <stdint.h>

extern void bl_main(uint32_t param);

void bl_rt_init(void);
void bl_init(void);
void bl_run_with_stack(uint32_t param, void (*fun)(uint32_t param), void *sp);
void bl_run_with_sram_top(uint32_t param, void (*fun)(uint32_t param));

#endif

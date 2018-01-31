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

#ifndef _BL_MALLOC_H_
#define _BL_MALLOC_H_

#include <stdint.h>

void bl_heap_init(uint32_t *sram_start, unsigned sram_wcount,
                  uint32_t *ddr_start, unsigned ddr_wcount);

void *bl_sram_malloc(unsigned size);
void *bl_ddr_malloc(unsigned size);
void bl_free(void *ptr);

#endif

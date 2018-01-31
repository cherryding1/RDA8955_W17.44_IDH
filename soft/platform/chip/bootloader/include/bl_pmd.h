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

#ifndef _BL_PMD_H_
#define _BL_PMD_H_

#include <stdint.h>

void bl_pmu_init(void);
void bl_pmd_close(void);

void bl_pmd_set_lcd_level(uint8_t level);
void bl_pmd_set_keypad_level(uint8_t level);
void bl_pmd_enable_camera_flash(bool enabled);
void bl_pmd_set_charge_current(uint32_t ma);
void bl_pmd_set_charge_max_vfb(void);
bool bl_pmd_ac_on(void);

void bl_pmd_force_set_core_voltage(bool ldo, uint8_t vcore);

#endif

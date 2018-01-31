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

#ifndef _BL_MAP_H_
#define _BL_MAP_H_

#include <stdint.h>

enum
{
    BL_MAP_HAL = 0x00000000,
    BL_MAP_PAL = 0x00000001,
    BL_MAP_SPAL = 0x00000002,
    BL_MAP_SPP = 0x00000003,
    BL_MAP_SPC = 0x00000004,
    BL_MAP_RFD = 0x00000005,
    BL_MAP_FREE1 = 0x00000006,
    BL_MAP_AT = 0x00000007,
    BL_MAP_MEMD = 0x00000008,
    BL_MAP_PMD = 0x00000009,
    BL_MAP_STACK = 0x0000000A,
    BL_MAP_VPP = 0x0000000B,
    BL_MAP_SX = 0x0000000C,
    BL_MAP_STD = 0x0000000D,
    BL_MAP_LCDD = 0x0000000E,
    BL_MAP_ARS = 0x0000000F,
    BL_MAP_APS = 0x00000010,
    BL_MAP_VOIS = 0x00000011,
    BL_MAP_CALIB = 0x00000012,
    BL_MAP_GTES = 0x00000013,
    BL_MAP_FLASH_PROG = 0x00000014,
    BL_MAP_CSW = 0x00000015,
    BL_MAP_MMI = 0x00000016,
    BL_MAP_AUD = 0x00000017,
    BL_MAP_MDI = 0x00000018,
    BL_MAP_BTD = 0x00000019,
    BL_MAP_COUNT = 0x0000001A
};

typedef struct
{
    uint32_t revision;
    uint32_t number;
    uint32_t date;
    uint8_t *str;
} BL_MAP_VERSION_T;

typedef struct
{
    BL_MAP_VERSION_T *version;
    void *access;
} BL_MAP_MODULE_T;

typedef struct
{
    BL_MAP_MODULE_T modules[BL_MAP_COUNT];
} BL_MAP_T;

void bl_map_init(void);
void bl_map_register(int id, void *version, void *access);

#endif

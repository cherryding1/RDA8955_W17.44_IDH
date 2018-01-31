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

#ifndef TGT_MEMD_CONFIG
#define TGT_MEMD_CONFIG

#ifdef RAM_WITH_DDR_MODE


#ifdef RDA_PSRAM_ADAPTIVE
#ifndef GALLITE_IS_8805
#include "memd_tgt_params_8809_aptic.h"
#include "memd_tgt_params_8809_etron.h"
#include "memd_tgt_params_8809_winbond.h"
#include "memd_tgt_params_8809_emc.h"

//#include "memd_tgt_params_8808s_aptic.h"
//#include "memd_tgt_params_8808s_emc.h"
//#include "memd_tgt_params_8808s_winbond.h"
//#include "memd_tgt_params_8808s_etron.h"
#endif
#endif


// THIS COMBO IS ADMUX

// -------------------------------
// RAM SETTINGS for 52 MHz
// -------------------------------
#ifdef MEMD_RAM_IS_BURST
#ifdef RAM_CLK_IS_52M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 4,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 4,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_78M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 6,                                                \
    .oedlyHalfMode  = 1,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 7,                                                \
    .wedlyHalfMode  = 1,                                                \
    .wedly          = 1,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_104M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 6,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 7,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_125M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 9,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 1,                                                \
    .wrLat          = 9,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_139M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 10,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 1,                                                \
    .wrLat          = 10,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 6,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 0,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 1,                                                \
    .wrLat          = 7,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#endif //RAM_CLK_IS_139M
#endif //RAM_CLK_IS_125M
#endif //RAM_CLK_IS_104M
#endif //RAM_CLK_IS_78M
#endif //RAM_CLK_IS_52M
#else //!MEMD_RAM_IS_BURST
#ifndef FPGA
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 4,                                                \
    .oedlyHalfMode  = 1,                                                \
    .oedly          = 0,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 4,                                                \
    .wedlyHalfMode  = 1,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 0,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 0,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 1}
#else // FPGA
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 4,                                                \
    .oedlyHalfMode  = 1,                                                \
    .oedly          = 0,                                                \
    .pageLat        = 1,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 4,                                                \
    .wedlyHalfMode  = 1,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 0,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 0,                                                \
    .pageMode       = 1,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 0,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 1}
#endif // FPGA
#endif //!MEMD_RAM_IS_BURST


#define TGT_MEMD_RAM_CONFIG                                             \
    {                                                                   \
        {SRAM_TIME_SETTING, SRAM_MODE_SETTING}                          \
    }

// -------------------------------
// FLASH SETTINGS
// -------------------------------
#define FLASH_TIME_SETTING {                                            \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 3,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 1,                                                \
    .pageLat        = 1,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 5,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define FLASH_MODE_SETTING {                                            \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 0,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 0,                                                \
    .pageMode       = 1,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 0,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}

#define TGT_MEMD_FLASH_CONFIG                                           \
    {                                                                   \
        {FLASH_TIME_SETTING, FLASH_MODE_SETTING}                        \
    }


#else  //no  RAM_WITH_DDR_MODE



#ifdef RDA_PSRAM_ADAPTIVE
#ifndef GALLITE_IS_8805
#include "memd_tgt_params_8809_aptic.h"
#include "memd_tgt_params_8809_etron.h"
#include "memd_tgt_params_8809_winbond.h"
#include "memd_tgt_params_8809_emc.h"

//#include "memd_tgt_params_8808s_aptic.h"
//#include "memd_tgt_params_8808s_emc.h"
//#include "memd_tgt_params_8808s_winbond.h"
//#include "memd_tgt_params_8808s_etron.h"
#endif
#endif


// THIS COMBO IS ADMUX

// -------------------------------
// RAM SETTINGS for 52 MHz
// -------------------------------
#ifdef MEMD_RAM_IS_BURST
#ifdef RAM_CLK_IS_52M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 3,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 4,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_78M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 6,                                                \
    .oedlyHalfMode  = 1,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 7,                                                \
    .wedlyHalfMode  = 1,                                                \
    .wedly          = 1,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_104M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 6,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 7,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_125M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 9,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 1,                                                \
    .wrLat          = 9,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#ifdef RAM_CLK_IS_139M
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 10,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 2,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 1,                                                \
    .wrLat          = 10,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#else
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 1,                                                \
    .rdLat          = 6,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 0,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 1,                                                \
    .wrLat          = 7,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 2,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 1,                                                \
    .writeBurstMode = 1,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}
#endif //RAM_CLK_IS_139M
#endif //RAM_CLK_IS_125M
#endif //RAM_CLK_IS_104M
#endif //RAM_CLK_IS_78M
#endif //RAM_CLK_IS_52M
#else //!MEMD_RAM_IS_BURST
#ifndef FPGA
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 4,                                                \
    .oedlyHalfMode  = 1,                                                \
    .oedly          = 0,                                                \
    .pageLat        = 0,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 4,                                                \
    .wedlyHalfMode  = 1,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 1,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 0,                                                \
    .pageMode       = 0,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 0,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 1}
#else // FPGA
#define SRAM_TIME_SETTING {                                             \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 4,                                                \
    .oedlyHalfMode  = 1,                                                \
    .oedly          = 0,                                                \
    .pageLat        = 1,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 4,                                                \
    .wedlyHalfMode  = 1,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define SRAM_MODE_SETTING {                                             \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 0,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 0,                                                \
    .pageMode       = 1,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 0,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 1}
#endif // FPGA
#endif //!MEMD_RAM_IS_BURST


#define TGT_MEMD_RAM_CONFIG                                             \
    {                                                                   \
        {SRAM_TIME_SETTING, SRAM_MODE_SETTING}                          \
    }

// -------------------------------
// FLASH SETTINGS
// -------------------------------
#define FLASH_TIME_SETTING {                                            \
    .rdLatHalfMode  = 0,                                                \
    .rdLat          = 3,                                                \
    .oedlyHalfMode  = 0,                                                \
    .oedly          = 1,                                                \
    .pageLat        = 1,                                                \
    .wrLatHalfMode  = 0,                                                \
    .wrLat          = 5,                                                \
    .wedlyHalfMode  = 0,                                                \
    .wedly          = 0,                                                \
    .relax          = 1,                                                \
    .forceRelax     = 0}
#define FLASH_MODE_SETTING {                                            \
    .csEn           = 1,                                                \
    .polarity       = 0,                                                \
    .admuxMode      = 0,                                                \
    .writeAllow     = 1,                                                \
    .wbeMode        = 0,                                                \
    .bedlyMode      = 0,                                                \
    .waitMode       = 0,                                                \
    .pageSize       = 0,                                                \
    .pageMode       = 1,                                                \
    .writePageMode  = 0,                                                \
    .burstMode      = 0,                                                \
    .writeBurstMode = 0,                                                \
    .advAsync       = 0,                                                \
    .advwePulse     = 0,                                                \
    .admuxDly       = 0,                                                \
    .writeSingle    = 0,                                                \
    .writeHold      = 0}

#define TGT_MEMD_FLASH_CONFIG                                           \
    {                                                                   \
        {FLASH_TIME_SETTING, FLASH_MODE_SETTING}                        \
    }






#endif
#endif // TGT_MEMD_CONFIG


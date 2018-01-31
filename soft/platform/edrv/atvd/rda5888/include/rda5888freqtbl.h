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


#ifndef __RDA5888FREQTBL_H__
#define __RDA5888FREQTBL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rda5888app.h"

// ---------------------------------------------------------------------------
// Default Channel Maps for USA, Canada, Korea, Taiwan, Mexico,
// Chile, Venezuela, the Philippines
// Default Channel map for NTSC
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_ntsc_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for China
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_pald_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for ShenZhen
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_pald_sz_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Vietnam
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_vietnam_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Western Europe,
// Turkey, UAE, Afghanistan, Singapore, Thailand,
// Cambodia, Indonesia, Malaysia, India,Pakistan,
// Portugal,Spain, and Laos
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_w_europe_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Burma
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_burma_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Japan
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_ntsc_japan_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Maps for UK, Hong Kong,
// and South Africa
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_a_hongkong_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Brazil
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_brazil_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Argentina
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_argentina_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Egypt, Germany, Iraq, Iran
//  Libya, Lebanon, SandiArabia, Tunisia.
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_secam_b_g_chn_map[];

// ---------------------------------------------------------------------------
// Default Channel Map for Russia, Bulgaria, Congo,
//  Hungry, Poland, Romania.
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_secam_d_k_chn_map[];

// ---------------------------------------------------------------------------
// Default channel map for factory test.
// ---------------------------------------------------------------------------
extern const rdamtv_channel_t rdamtv_factory_chn_map[];

extern const rdamtv_channel_t rdamtv_test_chn_map[];
extern const rdamtv_channel_t rdamtv_test1_chn_map[];
extern const rdamtv_channel_t rdamtv_test2_chn_map[];
extern const rdamtv_channel_t rdamtv_test_Hanzhou_chn_map[];
extern const rdamtv_channel_t rdamtv_search_chn_map[];

#ifdef __cplusplus
}
#endif

#endif

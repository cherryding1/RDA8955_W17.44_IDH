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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#include "isomedia_dev.h"

uint32 gf_m4a_get_profile(GF_M4ADecSpecInfo *cfg)
{
    switch (cfg->base_object_type)
    {
        case 2: /*AAC LC*/
            if (cfg->nb_chan<=2) return (cfg->base_sr<=24000) ? 0x28 : 0x29; /*LC@L1 or LC@L2*/
            return (cfg->base_sr<=48000) ? 0x2A : 0x2B; /*LC@L4 or LC@L5*/
        case 5: /*HE-AAC*/
            if (cfg->nb_chan<=2) return (cfg->base_sr<=24000) ? 0x2C : 0x2D; /*HE@L2 or HE@L3*/
            return (cfg->base_sr<=48000) ? 0x2E : 0x2F; /*HE@L4 or HE@L5*/
        /*default to HQ*/
        default:
            if (cfg->nb_chan<=2) return (cfg->base_sr<24000) ? 0x0E : 0x0F; /*HQ@L1 or HQ@L2*/
            return 0x10; /*HQ@L3*/
    }
}

GF_Err gf_m4a_get_config(int8 *dsi, uint32 dsi_size, GF_M4ADecSpecInfo *cfg)
{
    GF_BitStream *bs;
    memset(cfg, 0, sizeof(GF_M4ADecSpecInfo));
    if (!dsi || !dsi_size || (dsi_size<2) ) return GF_NON_COMPLIANT_BITSTREAM;
    bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);

    cfg->base_object_type = gf_bs_read_int(bs, 5);
    cfg->base_sr_index = gf_bs_read_int(bs, 4);
    if (cfg->base_sr_index == 0x0F)
    {
        cfg->base_sr = gf_bs_read_int(bs, 24);
    }
    else
    {
        cfg->base_sr = GF_M4ASampleRates[cfg->base_sr_index];
    }
    cfg->nb_chan = gf_bs_read_int(bs, 4);
    if (cfg->base_object_type==5)
    {
        cfg->has_sbr = 1;
        cfg->sbr_sr_index = gf_bs_read_int(bs, 4);
        if (cfg->sbr_sr_index == 0x0F)
        {
            cfg->sbr_sr = gf_bs_read_int(bs, 24);
        }
        else
        {
            cfg->sbr_sr = GF_M4ASampleRates[cfg->sbr_sr_index];
        }
        cfg->sbr_object_type = gf_bs_read_int(bs, 5);
    }

    gf_bs_align(bs);
    if (gf_bs_available(bs)>=2)
    {
        uint32 sync = gf_bs_read_int(bs, 11);
        if (sync==0x2b7)
        {
            cfg->sbr_object_type = gf_bs_read_int(bs, 5);
            cfg->has_sbr = gf_bs_read_int(bs, 1);
            if (cfg->has_sbr)
            {
                cfg->sbr_sr_index = gf_bs_read_int(bs, 4);
                if (cfg->sbr_sr_index == 0x0F)
                {
                    cfg->sbr_sr = gf_bs_read_int(bs, 24);
                }
                else
                {
                    cfg->sbr_sr = GF_M4ASampleRates[cfg->sbr_sr_index];
                }
            }
        }
    }
    cfg->audioPL = gf_m4a_get_profile(cfg);
    gf_bs_del(bs);
    return GF_OK;
}

#endif

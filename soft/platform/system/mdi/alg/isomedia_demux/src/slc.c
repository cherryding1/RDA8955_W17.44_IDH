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

//
//  Set the SL to the ISO predefined value
//
GF_Err gf_odf_slc_set_pref(GF_SLConfig *sl)
{
    if (! sl) return GF_BAD_PARAM;

    switch (sl->predefined)
    {

        case SLPredef_MP4:
            sl->useAccessUnitStartFlag = 0;
            sl->useAccessUnitEndFlag = 0;
            //each packet is an AU, and we need RAP signaling
            sl->useRandomAccessPointFlag = 1;
            sl->hasRandomAccessUnitsOnlyFlag = 0;
            sl->usePaddingFlag = 0;
            //in MP4 file, we use TimeStamps
            sl->useTimestampsFlag = 1;
            sl->useIdleFlag = 0;
            sl->durationFlag = 0;
            sl->timestampLength = 0;
            sl->OCRLength = 0;
            sl->AULength = 0;
            sl->instantBitrateLength = 0;
            sl->degradationPriorityLength = 0;
            sl->AUSeqNumLength = 0;
            sl->packetSeqNumLength = 0;
            break;

        case SLPredef_Null:
            sl->useAccessUnitStartFlag = 0;
            sl->useAccessUnitEndFlag = 0;
            sl->useRandomAccessPointFlag = 0;
            sl->hasRandomAccessUnitsOnlyFlag = 0;
            sl->usePaddingFlag = 0;
            sl->useTimestampsFlag = 0;
            sl->useIdleFlag = 0;
            sl->AULength = 0;
            sl->degradationPriorityLength = 0;
            sl->AUSeqNumLength = 0;
            sl->packetSeqNumLength = 0;

            //for MPEG4 IP
            sl->timestampResolution = 1000;
            sl->timestampLength = 32;
            break;
        case SLPredef_SkipSL:
            sl->predefined = SLPredef_SkipSL;
            break;
        /*handle all unknown predefined as predef-null*/
        default:
            sl->useAccessUnitStartFlag = 0;
            sl->useAccessUnitEndFlag = 0;
            sl->useRandomAccessPointFlag = 0;
            sl->hasRandomAccessUnitsOnlyFlag = 0;
            sl->usePaddingFlag = 0;
            sl->useTimestampsFlag = 1;
            sl->useIdleFlag = 0;
            sl->AULength = 0;
            sl->degradationPriorityLength = 0;
            sl->AUSeqNumLength = 0;
            sl->packetSeqNumLength = 0;

            sl->timestampResolution = 1000;
            sl->timestampLength = 32;
            break;
    }

    return GF_OK;
}

GF_Err gf_odf_read_slc(GF_BitStream *bs, GF_SLConfig *sl, uint32 DescSize)
{
    GF_Err e;
    uint32 reserved, nbBytes = 0;

    if (!sl) return GF_BAD_PARAM;

    //APPLE fix
    if (!DescSize)
    {
        sl->predefined = SLPredef_MP4;
        return gf_odf_slc_set_pref(sl);
    }

    sl->predefined = gf_bs_read_int(bs, 8);
    nbBytes += 1;

    /*MPEG4 IP fix*/
    if (!sl->predefined && nbBytes==DescSize)
    {
        sl->predefined = SLPredef_Null;
        gf_odf_slc_set_pref(sl);
        return GF_OK;
    }

    if (sl->predefined)
    {
        //predefined configuration
        e = gf_odf_slc_set_pref(sl);
        if (e) return e;
    }
    else
    {
        sl->useAccessUnitStartFlag = gf_bs_read_int(bs, 1);
        sl->useAccessUnitEndFlag = gf_bs_read_int(bs, 1);
        sl->useRandomAccessPointFlag = gf_bs_read_int(bs, 1);
        sl->hasRandomAccessUnitsOnlyFlag = gf_bs_read_int(bs, 1);
        sl->usePaddingFlag = gf_bs_read_int(bs, 1);
        sl->useTimestampsFlag = gf_bs_read_int(bs, 1);
        sl->useIdleFlag = gf_bs_read_int(bs, 1);
        sl->durationFlag = gf_bs_read_int(bs, 1);
        sl->timestampResolution = gf_bs_read_int(bs, 32);
        sl->OCRResolution = gf_bs_read_int(bs, 32);

        sl->timestampLength = gf_bs_read_int(bs, 8);
        if (sl->timestampLength > 64) return GF_ODF_INVALID_DESCRIPTOR;

        sl->OCRLength = gf_bs_read_int(bs, 8);
        if (sl->OCRLength > 64) return GF_ODF_INVALID_DESCRIPTOR;

        sl->AULength = gf_bs_read_int(bs, 8);
        if (sl->AULength > 32) return GF_ODF_INVALID_DESCRIPTOR;

        sl->instantBitrateLength = gf_bs_read_int(bs, 8);
        sl->degradationPriorityLength = gf_bs_read_int(bs, 4);
        sl->AUSeqNumLength = gf_bs_read_int(bs, 5);
        if (sl->AUSeqNumLength > 16) return GF_ODF_INVALID_DESCRIPTOR;
        sl->packetSeqNumLength = gf_bs_read_int(bs, 5);
        if (sl->packetSeqNumLength > 16) return GF_ODF_INVALID_DESCRIPTOR;

        reserved = gf_bs_read_int(bs, 2);
        nbBytes += 15;
    }

    if (sl->durationFlag)
    {
        sl->timeScale = gf_bs_read_int(bs, 32);
        sl->AUDuration = gf_bs_read_int(bs, 16);
        sl->CUDuration = gf_bs_read_int(bs, 16);
        nbBytes += 8;
    }
    if (DescSize != nbBytes) return GF_ODF_INVALID_DESCRIPTOR;
    return GF_OK;
}

GF_Descriptor *gf_odf_new_slc(uint8 predef)
{
    //diag_printf("mmc_MemMalloc .... gf_odf_new_slc \n");
    GF_SLConfig *newDesc = (GF_SLConfig *) mmc_MemMalloc(sizeof(GF_SLConfig));
    if (!newDesc) return NULL;
    memset(newDesc, 0, sizeof(GF_SLConfig));
    newDesc->tag = GF_ODF_SLC_TAG;
    newDesc->predefined = predef;
    if (predef) gf_odf_slc_set_pref(newDesc);
    newDesc->useTimestampsFlag = 1;

    return (GF_Descriptor *)newDesc;
}
#endif


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


s32 gf_odf_size_field_size(uint32 size_desc)
{
    if (size_desc < 0x00000080)
    {
        return 1 + 1;
    }
    else if (size_desc < 0x00004000)
    {
        return 2 + 1;
    }
    else if (size_desc < 0x00200000)
    {
        return 3 + 1;
    }
    else if (size_desc < 0x10000000)
    {
        return 4 + 1;
    }
    else
    {
        return -1;
    }

}

GF_Descriptor *gf_odf_create_descriptor(uint8 tag)
{
    GF_Descriptor *desc;

    switch (tag)
    {
        case GF_ODF_ESD_TAG:
            return gf_odf_new_esd();
        case GF_ODF_DCD_TAG:
            return gf_odf_new_dcd();
        case GF_ODF_SLC_TAG:
            //default : we create it without any predefinition...
            return gf_odf_new_slc(0);
        //default. The DecSpecInfo is handled as default
        //the appropriate decoder (audio, video, bifs...) has to decode the DecSpecInfo alone !
        case GF_ODF_DSI_TAG:
            desc = gf_odf_new_default();
            if (!desc) return desc;
            desc->tag = GF_ODF_DSI_TAG;
            return desc;

        case 0:
        case 0xFF:
            return NULL;
        default:
            //ISO Reserved
            if ( (tag >= GF_ODF_ISO_RES_BEGIN_TAG) &&
                    (tag <= GF_ODF_ISO_RES_END_TAG) )
            {
                return NULL;
            }
            desc = gf_odf_new_default();
            if (!desc) return desc;
            desc->tag = tag;
            return desc;
    }
}


//
//      READERS
//
GF_Err gf_odf_read_descriptor(GF_BitStream *bs, GF_Descriptor *desc, uint32 DescSize)
{
    switch (desc->tag)
    {
        case GF_ODF_ESD_TAG :
            return gf_odf_read_esd(bs, (GF_ESD *)desc, DescSize);
        case GF_ODF_DCD_TAG :
            return gf_odf_read_dcd(bs, (GF_DecoderConfig *)desc, DescSize);
        case GF_ODF_SLC_TAG :
            return gf_odf_read_slc(bs, (GF_SLConfig *)desc, DescSize);
        //default:
        case GF_ODF_DSI_TAG:
        default:
            return gf_odf_read_default(bs, (GF_DefaultDescriptor *)desc, DescSize);

    }
    return GF_OK;
}


GF_Err gf_odf_parse_descriptor(GF_BitStream *bs, GF_Descriptor **desc, uint32 *desc_size)
{
    uint32 val, size, sizeHeader;
    uint8 tag;
    GF_Err err;
    GF_Descriptor *newDesc;
    if (!bs) return GF_BAD_PARAM;

    *desc_size = 0;

    //tag
    tag = gf_bs_read_int(bs, 8);
    sizeHeader = 1;

    //size
    size = 0;
    do
    {
        val = gf_bs_read_int(bs, 8);
        sizeHeader++;
        size <<= 7;
        size |= val & 0x7F;
    }
    while ( val & 0x80 );
    *desc_size = size;

    newDesc = gf_odf_create_descriptor(tag);
    if (! newDesc)
    {
        *desc = NULL;
        *desc_size = sizeHeader;
        if ( (tag >= GF_ODF_ISO_RES_BEGIN_TAG) &&
                (tag <= GF_ODF_ISO_RES_END_TAG) )
        {
            return GF_ODF_FORBIDDEN_DESCRIPTOR;
        }
        else if (!tag || (tag == 0xFF))
        {
            return GF_ODF_INVALID_DESCRIPTOR;
        }
        return GF_OUT_OF_MEM;
    }

    newDesc->tag = tag;

    err = gf_odf_read_descriptor(bs, newDesc, *desc_size);

    /*FFMPEG fix*/
    if ((tag==GF_ODF_SLC_TAG) && (((GF_SLConfig*)newDesc)->predefined==2))
    {
        if (*desc_size==3)
        {
            *desc_size = 1;
            err = GF_OK;
        }
    }

    //little trick to handle lazy bitstreams that encode
    //SizeOfInstance on a fix number of bytes
    //This nb of bytes is added in Read methods
    *desc_size += sizeHeader - gf_odf_size_field_size(*desc_size);
    *desc = newDesc;
    if (err)
    {
        ;//gf_odf_delete_descriptor(newDesc);
        *desc = NULL;
    }
    return err;
}

GF_Descriptor *gf_odf_new_dcd()
{
    //diag_printf("mmc_MemMalloc .... gf_odf_new_dcd \n");
    GF_DecoderConfig *newDesc = (GF_DecoderConfig *) mmc_MemMalloc(sizeof(GF_DecoderConfig));
    if (!newDesc) return NULL;
    newDesc->avgBitrate = 0;
    newDesc->bufferSizeDB = 0;
    newDesc->maxBitrate = 0;
    newDesc->objectTypeIndication = 0;
    newDesc->streamType = 0;
    newDesc->upstream = 0;
    newDesc->decoderSpecificInfo = NULL;
//  newDesc->profileLevelIndicationIndexDescriptor = gf_list_new();
    newDesc->tag = GF_ODF_DCD_TAG;
    return (GF_Descriptor *) newDesc;
}

#endif


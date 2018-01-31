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


GF_Box *gf_isom_box_new(uint32 boxType)
{
    GF_Box *a;
    switch (boxType)
    {
        case GF_ISOM_BOX_TYPE_MDAT: return mdat_New();
        case GF_ISOM_BOX_TYPE_MOOV: return moov_New();
        case GF_ISOM_BOX_TYPE_MVHD: return mvhd_New();
        case GF_ISOM_BOX_TYPE_MDHD: return mdhd_New();
        case GF_ISOM_BOX_TYPE_VMHD: return vmhd_New();
        case GF_ISOM_BOX_TYPE_SMHD: return smhd_New();
        case GF_ISOM_BOX_TYPE_STBL: return stbl_New();
        //case GF_ISOM_BOX_TYPE_DINF: return dinf_New(); reduce memory sheen.
        //case GF_ISOM_BOX_TYPE_URL: return url_New(); reduce memory sheen.
        //case GF_ISOM_BOX_TYPE_URN: return urn_New(); reduce memory sheen.
//  case GF_ISOM_BOX_TYPE_CPRT: return cprt_New();
//  case GF_ISOM_BOX_TYPE_CHPL: return chpl_New();
        case GF_ISOM_BOX_TYPE_HDLR: return hdlr_New();
//  case GF_ISOM_BOX_TYPE_IODS: return iods_New();
        case GF_ISOM_BOX_TYPE_TRAK: return trak_New();
//  case GF_ISOM_BOX_TYPE_MP4S: return mp4s_New();
        case GF_ISOM_BOX_TYPE_MP4V: return mp4v_New();
        case GF_ISOM_BOX_TYPE_MP4A: return mp4a_New();
        case GF_ISOM_BOX_TYPE_MP3: return mp3_New();
        //case GF_ISOM_BOX_TYPE_UDTA: return udta_New();shenh
        case GF_ISOM_BOX_TYPE_DREF: return dref_New();
        case GF_ISOM_BOX_TYPE_STSD: return stsd_New();
        case GF_ISOM_BOX_TYPE_STTS: return stts_New();
//  case GF_ISOM_BOX_TYPE_CTTS: return ctts_New();
        case GF_ISOM_BOX_TYPE_STSC: return stsc_New();
        //case GF_ISOM_BOX_TYPE_GNRA: return gnra_New();shenh
        case GF_ISOM_BOX_TYPE_STZ2:
        case GF_ISOM_BOX_TYPE_STSZ:
            a = stsz_New();
            if (a) a->type = boxType;
            return a;
        case GF_ISOM_BOX_TYPE_STCO: return stco_New();
        case GF_ISOM_BOX_TYPE_STSS: return stss_New();
//  case GF_ISOM_BOX_TYPE_CO64: return co64_New();
        case GF_ISOM_BOX_TYPE_ESDS: return esds_New();
        case GF_ISOM_BOX_TYPE_MINF: return minf_New();
        case GF_ISOM_BOX_TYPE_TKHD: return tkhd_New();
        case GF_ISOM_BOX_TYPE_MDIA: return mdia_New();

        case GF_ISOM_BOX_TYPE_FTYP: return ftyp_New();
        //add shenh
        case GF_ISOM_BOX_TYPE_S263: return s263_New();
        case GF_ISOM_BOX_TYPE_SAMR: return samr_New();
        case GF_ISOM_BOX_TYPE_D263: return d263_New();
        case GF_ISOM_BOX_TYPE_DAMR: return damr_New();
        case GF_ISOM_BOX_TYPE_AVC1: return avc1_New();
        case GF_ISOM_BOX_TYPE_AVCC: return avcC_New();
        case GF_ISOM_BOX_TYPE_JPEG: return jpeg_New();
        //

        default:
#if 0//reduce memory sheen
            a = defa_New();
            if (a)
            {
                a->type = boxType;
                /*to identify a default box regardless of its type we format the UUID (not used)*/
                memset(a->uuid, 1, 16);
            }
#else
            a=0;
#endif

            return a;
    }
}

GF_Err gf_isom_box_read(GF_Box *a, GF_BitStream *bs)
{
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_MDAT: return mdat_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MOOV: return moov_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MVHD: return mvhd_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MDHD: return mdhd_Read(a, bs);
        case GF_ISOM_BOX_TYPE_VMHD: return vmhd_Read(a, bs);
        case GF_ISOM_BOX_TYPE_SMHD: return smhd_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STBL: return stbl_Read(a, bs);
        case GF_ISOM_BOX_TYPE_DINF: return dinf_Read(a, bs);
        case GF_ISOM_BOX_TYPE_URL: return url_Read(a, bs);
        case GF_ISOM_BOX_TYPE_URN: return urn_Read(a, bs);
        case GF_ISOM_BOX_TYPE_HDLR: return hdlr_Read(a, bs);
        case GF_ISOM_BOX_TYPE_TRAK: return trak_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_MP4S: return mp4s_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MP4V: return mp4v_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MP4A: return mp4a_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MP3: return mp3_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_EDTS: return edts_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_UDTA: return udta_Read(a, bs);
        case GF_ISOM_BOX_TYPE_DREF: return dref_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STSD: return stsd_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STTS: return stts_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_CTTS: return ctts_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STSC: return stsc_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STZ2:
        case GF_ISOM_BOX_TYPE_STSZ:
            return stsz_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STCO: return stco_Read(a, bs);
        case GF_ISOM_BOX_TYPE_STSS: return stss_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_STDP: return stdp_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_CO64: return co64_Read(a, bs);
        case GF_ISOM_BOX_TYPE_ESDS: return esds_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MINF: return minf_Read(a, bs);
        case GF_ISOM_BOX_TYPE_TKHD: return tkhd_Read(a, bs);
        case GF_ISOM_BOX_TYPE_MDIA: return mdia_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_CHPL: return chpl_Read(a, bs);
        case GF_ISOM_BOX_TYPE_FTYP: return ftyp_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_FADB: return padb_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_VOID: return void_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_STSF: return stsf_Read(a, bs);
//  case GF_ISOM_BOX_TYPE_PDIN: return pdin_Read(a, bs);
        //add shenh
        case GF_ISOM_BOX_TYPE_S263: return s263_Read(a, bs);
        case GF_ISOM_BOX_TYPE_SAMR: return samr_Read(a, bs);
        case GF_ISOM_BOX_TYPE_D263: return d263_Read(a, bs);
        case GF_ISOM_BOX_TYPE_DAMR: return damr_Read(a, bs);
        case GF_ISOM_BOX_TYPE_AVC1: return avc1_Read(a, bs);
        case GF_ISOM_BOX_TYPE_AVCC: return avcC_Read(a, bs);
        case GF_ISOM_BOX_TYPE_JPEG: return jpeg_Read(a, bs);
        //

        default:
            return defa_Read(a, bs);
    }
}

GF_Err gf_isom_parse_box(GF_Box **outBox, GF_BitStream *bs)
{
    uint32 type, hdr_size;
    u64 size, start, end;
    uint8 uuid[16];
    GF_Err e;
    GF_Box *newBox;
    e = GF_OK;
    if ((bs == NULL) || (outBox == NULL) ) return GF_BAD_PARAM;
    *outBox = NULL;


    start = gf_bs_get_position(bs);
    size = (u64) gf_bs_read_u32(bs);//读码流－当前包的大小
    hdr_size = 4;
    type = gf_bs_read_u32(bs);
    hdr_size += 4;


    /*handle uuid*/
    memset(uuid, 0, 16);
    if (type == GF_ISOM_BOX_TYPE_UUID )
    {
        gf_bs_read_data(bs, (uint8 *) uuid, 16);
        hdr_size += 16;
    }

    //handle large box
    if (size == 1)
    {
        size = gf_bs_read_u64(bs);
        hdr_size += 8;
    }

    if ( size < hdr_size )
    {
        *outBox = (void*)((uint32)hdr_size);
        //diag_printf("gf_isom_parse_box  size.");
        return GF_INVALID_BOX_SIZE;

    }
    //OK, create the box based on the type

    newBox = gf_isom_box_new(type);

    if (!newBox)
    {
        //skip invalid box.sheen
        *outBox = (void*)((uint32)size);//put size in outbox.sheen
        gf_bs_skip_bytes(bs, size - hdr_size);
        return GF_INVALID_BOX;
        //return GF_OUT_OF_MEM; sheen.
    }

    //OK, init and read this box
    memcpy(newBox->uuid, uuid, 16);
    if (!newBox->type) newBox->type = type;

    end = gf_bs_available(bs);
    if (size - hdr_size > end )
    {
        newBox->size = size - hdr_size - end;
        *outBox = newBox;
        return GF_ISOM_INCOMPLETE_FILE;
    }
    //we need a special reading for these boxes...
    if (newBox->type == GF_ISOM_BOX_TYPE_STDP)
    {
        newBox->size = size;
        *outBox = newBox;
        return GF_OK;
    }

    //check track vide soun .sheen
    if(newBox->type == GF_ISOM_BOX_TYPE_TRAK)
    {
        u64 bs_pos;
        u64 tmp_size;
        uint32 trk_offset=hdr_size;
        uint8 get;

        bs_pos=gf_bs_get_position(bs);

        get=0;//get mdia
        do
        {
            tmp_size=gf_bs_read_u32(bs);
            trk_offset += 4;
            type = gf_bs_read_u32(bs);
            trk_offset += 4;
            if(type!=GF_ISOM_BOX_TYPE_MDIA)
            {
                gf_bs_skip_bytes(bs, tmp_size - 8);
                trk_offset+=tmp_size - 8;
            }
            else
            {
                get=1;
                break;
            }
        }
        while(trk_offset < size);

        if(!get)return GF_ISOM_INVALID_MEDIA;

        get=0;//get hdlr
        do
        {
            tmp_size=gf_bs_read_u32(bs);
            trk_offset += 4;
            type = gf_bs_read_u32(bs);
            trk_offset += 4;
            if(type!=GF_ISOM_BOX_TYPE_HDLR)
            {
                gf_bs_skip_bytes(bs, tmp_size - 8);
                trk_offset+=tmp_size - 8;
            }
            else
            {
                get=1;
                break;
            }
        }
        while(trk_offset < size);

        if(!get)return GF_ISOM_INVALID_MEDIA;

        gf_bs_skip_bytes(bs, 8);
        trk_offset += 8;
        type = gf_bs_read_u32(bs);
        trk_offset += 4;

        if(type==GF_ISOM_MEDIA_VISUAL || type==GF_ISOM_MEDIA_AUDIO)
        {
            gf_bs_seek(bs, bs_pos);//back to pos and continue to do.
        }
        else
        {
            //not read unuse track.
            newBox->size = size;
            *outBox = newBox;
            gf_bs_skip_bytes(bs, size - trk_offset);
            return GF_OK;
        }
    }

    newBox->size = size - hdr_size;

    e = gf_isom_box_read(newBox, bs);
    newBox->size = size;
    end = gf_bs_get_position(bs);
    *outBox = newBox;

    return e;
}

void gf_isom_full_box_init(GF_Box *a)
{
    GF_FullBox *ptr = (GF_FullBox *)a;
    if (! ptr) return;
    ptr->flags = 0;
    ptr->version = 0;
}

GF_Err gf_isom_full_box_read(GF_Box *ptr, GF_BitStream *bs)
{
    GF_FullBox *self = (GF_FullBox *) ptr;
    if (ptr->size<4) return GF_ISOM_INVALID_FILE;
    self->version = gf_bs_read_u8(bs);
    self->flags = gf_bs_read_u24(bs);
    ptr->size -= 4;
    return GF_OK;
}

#if 0
GF_Err gf_isom_full_box_get_size(GF_Box *ptr)
{
    GF_Err e;
    e = gf_isom_box_get_size(ptr);
    if (e) return e;
    ptr->size += 4;
    return GF_OK;
}
#endif

GF_Err gf_isom_read_box_list(GF_Box *parent, GF_BitStream *bs, GF_Err (*add_box)(GF_Box *par, GF_Box *b))
{
    GF_Err e;
    GF_Box *a;
    while (parent->size)
    {
        e = gf_isom_parse_box(&a, bs);
        if(e==GF_INVALID_BOX)//sheen
        {
            if (parent->size<(uint32)a) return GF_ISOM_INVALID_FILE;
            parent->size -= (uint32)a;//recode the size.sheen
            continue;//skip this box
        }

        if (e==GF_INVALID_BOX_SIZE)
        {
            parent->size -= (uint32)a;//recode the size.sheen
        }

        if (e) return e;
        if (parent->size < a->size) return GF_ISOM_INVALID_FILE;
        parent->size -= a->size;
        e = add_box(parent, a);
        if (e)
        {
            //gf_isom_box_del(a);
            return e;
        }
    }
    return GF_OK;
}

GF_Err gf_isom_box_get_size(GF_Box *ptr)
{
    ptr->size = 8;

    if (ptr->type == GF_ISOM_BOX_TYPE_UUID)
    {
        ptr->size += 16;
    }
    //the large size is handled during write, cause at this stage we don't know the size
    return GF_OK;
}

void gf_isom_audio_sample_entry_init(GF_AudioSampleEntryBox *ptr)
{
    ptr->channel_count = 2;
    ptr->bitspersample = 16;
}

void gf_isom_audio_sample_entry_size(GF_AudioSampleEntryBox *ptr)
{
    ptr->size += 28;
}

void gf_isom_video_sample_entry_size(GF_VisualSampleEntryBox *ent)
{
    ent->size += 78;
}

void gf_isom_video_sample_entry_init(GF_VisualSampleEntryBox *ent)
{
    ent->horiz_res = ent->vert_res = 0x00480000;
    ent->frames_per_sample = 1;
    ent->bit_depth = 0x18;
    ent->color_table_index = -1;
}

GF_Err gf_isom_video_sample_entry_read(GF_VisualSampleEntryBox *ptr, GF_BitStream *bs)
{
    if (ptr->size < 78) return GF_ISOM_INVALID_FILE;
    ptr->size -= 78;
    gf_bs_read_data(bs, (uint8*)ptr->reserved, 6);
    ptr->dataReferenceIndex = gf_bs_read_u16(bs);
    ptr->version = gf_bs_read_u16(bs);
    ptr->revision = gf_bs_read_u16(bs);
    ptr->vendor = gf_bs_read_u32(bs);
    ptr->temporal_quality  = gf_bs_read_u32(bs);
    ptr->spacial_quality = gf_bs_read_u32(bs);
    ptr->Width = gf_bs_read_u16(bs);
    ptr->Height = gf_bs_read_u16(bs);
    ptr->horiz_res = gf_bs_read_u32(bs);
    ptr->vert_res = gf_bs_read_u32(bs);
    ptr->entry_data_size = gf_bs_read_u32(bs);
    ptr->frames_per_sample = gf_bs_read_u16(bs);
    gf_bs_read_data(bs, ptr->compressor_name, 32);
    // FIXME  Use a macro.
    ptr->compressor_name[32-1] = 0;
    ptr->bit_depth = gf_bs_read_u16(bs);
    ptr->color_table_index = gf_bs_read_u16(bs);
    return GF_OK;
}

GF_Err gf_isom_audio_sample_entry_read(GF_AudioSampleEntryBox *ptr, GF_BitStream *bs)
{
    if (ptr->size<28) return GF_ISOM_INVALID_FILE;
    ptr->size -= 28;
    gf_bs_read_data(bs, (uint8*)ptr->reserved, 6);
    ptr->dataReferenceIndex = gf_bs_read_u16(bs);
    ptr->version = gf_bs_read_u16(bs);
    ptr->revision = gf_bs_read_u16(bs);
    ptr->vendor = gf_bs_read_u32(bs);
    ptr->channel_count = gf_bs_read_u16(bs);
    ptr->bitspersample = gf_bs_read_u16(bs);
    ptr->compression_id = gf_bs_read_u16(bs);
    ptr->packet_size = gf_bs_read_u16(bs);
    ptr->samplerate_hi = gf_bs_read_u16(bs);
    ptr->samplerate_lo = gf_bs_read_u16(bs);
    return GF_OK;
}

GF_DataMap *gf_isom_fdm_new(HANDLE file_handle, uint8 file_mode, uint32 data_Len)
{

    GF_FileDataMap *tmp = (GF_FileDataMap *) mmc_MemMalloc(sizeof(GF_FileDataMap));

    if (!tmp) return NULL;
    memset(tmp, 0, sizeof(GF_FileDataMap));
    tmp->type = GF_ISOM_DATA_FILE;
    //tmp->stream =fopen(fileName,"rb"); change shenh
    //tmp->stream=FS_Open((PCSTR)fileName, FS_O_RDONLY, 0);
    tmp->stream=(HANDLE*)file_handle;
    if(tmp->stream<0)return NULL;

    tmp->bs = gf_bs_from_file(tmp->stream, file_mode,  data_Len, GF_BITSTREAM_READ);
    if (!tmp->bs)
    {
        //fclose(tmp->stream); change shenh
        //FS_Close((INT32)tmp->stream);
        //free(tmp);change shenh
        return NULL;
    }
    return (GF_DataMap *)tmp;
}


GF_Err gf_isom_datamap_new(HANDLE file_handle, uint8 file_mode, uint32 data_Len, GF_DataMap **outDataMap)
{
    *outDataMap = NULL;
    *outDataMap = gf_isom_fdm_new( file_handle, file_mode, data_Len);
    if (! (*outDataMap)) return GF_URL_ERROR;
    return GF_OK;
}

void gf_isom_sample_del(GF_ISOSample **samp)
{
    if (! *samp) return;
    /*change shenh
    if ((*samp)->data && (*samp)->dataLength) free((*samp)->data);
    free(*samp);

    *samp = NULL;
    */
}

#endif


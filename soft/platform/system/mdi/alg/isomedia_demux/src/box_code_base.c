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
#include "mcip_debug.h"
#endif

#ifdef MP4_3GP_SUPPORT

#include "isomedia_dev.h"

uint8 *pBufGlob;//temp buf. 512bytes .add shenh

#if 0 //change shenh
void co64_del(GF_Box *s)
{
    GF_ChunkLargeOffsetBox *ptr;
    ptr = (GF_ChunkLargeOffsetBox *) s;
    if (ptr == NULL) return;
    if (ptr->offsets) free(ptr->offsets);
    free(ptr);
}

GF_Err co64_Read(GF_Box *s,GF_BitStream *bs)
{
    GF_Err e;
    uint32 entries;
    GF_ChunkLargeOffsetBox *ptr = (GF_ChunkLargeOffsetBox *) s;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    ptr->entryCount = gf_bs_read_u32(bs);
    ptr->offsets = (u64 *) malloc(ptr->entryCount * sizeof(u64) );
    if (ptr->offsets == NULL) return GF_OUT_OF_MEM;
    for (entries = 0; entries < ptr->entryCount; entries++)
    {
        ptr->offsets[entries] = gf_bs_read_u64(bs);
    }
    return GF_OK;
}

GF_Box *co64_New()
{
    GF_ChunkLargeOffsetBox *tmp;

    tmp = (GF_ChunkLargeOffsetBox *) malloc(sizeof(GF_ChunkLargeOffsetBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_ChunkLargeOffsetBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_CO64;
    return (GF_Box *)tmp;
}


GF_Err co64_Size(GF_Box *s)
{
    GF_Err e;
    GF_ChunkLargeOffsetBox *ptr = (GF_ChunkLargeOffsetBox *) s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 4 + (8 * ptr->entryCount);
    return GF_OK;
}
#endif

#if 0
void url_del(GF_Box *s)
{
    GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;
    if (ptr == NULL) return;
    /* change shenh
    if (ptr->location) free(ptr->location);
    free(ptr);
    */
    return;
}
#endif

GF_Err url_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    if (ptr->size)
    {
        //diag_printf("mmc_MemMalloc .... url_Read \n");
        ptr->location = (int8*)mmc_MemMalloc((uint32) ptr->size);
        if (! ptr->location) return GF_OUT_OF_MEM;
        memset( ptr->location, 0, ptr->size);
        gf_bs_read_data(bs, (uint8*)ptr->location, (uint32)ptr->size);
    }
    return GF_OK;
}

GF_Box *url_New()
{
    //diag_printf("mmc_MemMalloc .... url_New \n");
    GF_DataEntryURLBox *tmp = (GF_DataEntryURLBox *) mmc_MemMalloc(sizeof(GF_DataEntryURLBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_DataEntryURLBox));

    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_URL;
    return (GF_Box *)tmp;
}

#if 0
GF_Err url_Size(GF_Box *s)
{
    GF_Err e;
    GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;

    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    if ( !(ptr->flags & 1))
    {
        if (ptr->location) ptr->size += 1 + strlen(ptr->location);
    }
    return GF_OK;
}
#endif

#if 0
void urn_del(GF_Box *s)
{
    GF_DataEntryURNBox *ptr = (GF_DataEntryURNBox *)s;
    if (ptr == NULL) return;
    /* change shenh
    if (ptr->location) free(ptr->location);
    if (ptr->nameURN) free(ptr->nameURN);
    free(ptr);
    */
}
#endif

GF_Err urn_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 i, to_read;
    int8 *tmpName;
    GF_DataEntryURNBox *ptr = (GF_DataEntryURNBox *)s;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    if (! ptr->size ) return GF_OK;

    //here we have to handle that in a clever way
    to_read = (uint32) ptr->size;
    //diag_printf("mmc_MemMalloc .... urn_Read 1 \n");
    tmpName = (int8*)mmc_MemMalloc(sizeof(int8) * to_read);
    if (!tmpName) return GF_OUT_OF_MEM;
    memset(tmpName, 0, sizeof(int8) * to_read);
    //get the data
    gf_bs_read_data(bs, (uint8*)tmpName, to_read);

    //then get the break
    i = 0;
    while ( (tmpName[i] != 0) && (i < to_read) )
    {
        i++;
    }
    //check the data is consistent
    if (i == to_read)
    {
        //free(tmpName);change shenh
        return GF_ISOM_INVALID_FILE;
    }
    //no NULL char, URL is not specified
    if (i == to_read - 1)
    {
        ptr->nameURN = tmpName;
        ptr->location = NULL;
        return GF_OK;
    }
    //OK, this has both URN and URL
    //diag_printf("mmc_MemMalloc .... urn_Read 2 \n");
    ptr->nameURN = (int8*)mmc_MemMalloc(sizeof(int8) * (i+1));
    if (!ptr->nameURN)
    {
        //free(tmpName);change shenh
        return GF_OUT_OF_MEM;
    }
    memset(ptr->nameURN, 0, sizeof(int8) * (i+1));
    //diag_printf("mmc_MemMalloc .... urn_Read 3 \n");
    ptr->location = (int8*)mmc_MemMalloc(sizeof(int8) * (to_read - i - 1));
    if (!ptr->location)
    {
        /*change shenh
        free(tmpName);
        free(ptr->nameURN);
        ptr->nameURN = NULL;
        */
        return GF_OUT_OF_MEM;
    }
    memset(ptr->location , 0, sizeof(int8) * (to_read - i - 1));

    memcpy(ptr->nameURN, tmpName, i + 1);
    memcpy(ptr->location, tmpName + i + 1, (to_read - i - 1));
    //free(tmpName);change shenh
    return GF_OK;
}

GF_Box *urn_New()
{
    //diag_printf("mmc_MemMalloc .... urn_New \n");
    GF_DataEntryURNBox *tmp = (GF_DataEntryURNBox *) mmc_MemMalloc(sizeof(GF_DataEntryURNBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_DataEntryURNBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_URN;
    return (GF_Box *)tmp;
}

#if 0
GF_Err urn_Size(GF_Box *s)
{
    GF_Err e;
    GF_DataEntryURNBox *ptr = (GF_DataEntryURNBox *)s;

    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    if ( !(ptr->flags & 1))
    {
        if (ptr->nameURN) ptr->size += 1 + strlen(ptr->nameURN);
        if (ptr->location) ptr->size += 1 + strlen(ptr->location);
    }
    return GF_OK;
}
#endif

GF_Err dinf_AddBox(GF_Box *s, GF_Box *a)
{
    GF_DataInformationBox *ptr = (GF_DataInformationBox *)s;
    switch(a->type)
    {
        case GF_ISOM_BOX_TYPE_DREF:
            if (ptr->dref) return GF_ISOM_INVALID_FILE;
            ptr->dref = (GF_DataReferenceBox *)a;
            return GF_OK;
    }
    //gf_isom_box_del(a);
    return GF_OK;
}

GF_Err dinf_Read(GF_Box *s, GF_BitStream *bs)
{
    return gf_isom_read_box_list(s, bs, dinf_AddBox);
}

GF_Box *dinf_New()
{
    //diag_printf("mmc_MemMalloc .... dinf_New \n");
    GF_DataInformationBox *tmp = (GF_DataInformationBox *) mmc_MemMalloc(sizeof(GF_DataInformationBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_DataInformationBox));
    tmp->type = GF_ISOM_BOX_TYPE_DINF;
    return (GF_Box *)tmp;
}

#if 0
GF_Err dref_AddDataEntry(GF_DataReferenceBox *ptr, GF_Box *entry)//no use ,shenh
{
    return gf_list_add(ptr->boxList, entry);
}
#endif

GF_Err dref_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 count, i;
    GF_Box *a;
    GF_DataReferenceBox *ptr = (GF_DataReferenceBox *)s;

    if (ptr == NULL) return GF_BAD_PARAM;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    count = gf_bs_read_u32(bs);

    if(!ptr->boxList)return GF_BAD_PARAM;
    ptr->boxList->slots=(void**)mmc_MemMalloc(count*sizeof(void*));//add shenh
    if (!ptr->boxList->slots)
    {
        ptr->boxList->entryCount = 0;
        return GF_OUT_OF_MEM;
    }
    memset( ptr->boxList->slots , 0,  count*sizeof(void*));

    for ( i = 0; i < count; i++ )
    {
        e = gf_isom_parse_box(&a, bs);
        if(e==GF_INVALID_BOX)//sheen
        {
            if (ptr->size<(uint32)a) return GF_ISOM_INVALID_FILE;
            ptr->size -= (uint32)a;//recode the size.sheen
            continue;//skip this box
        }
        if (e) return e;
        if (ptr->size<a->size) return GF_ISOM_INVALID_FILE;
        //e = gf_list_add(ptr->boxList, a);//change shenh
        //if (e) return e;
        ptr->boxList->entryCount ++;
        ptr->boxList->slots[ptr->boxList->entryCount-1] = a;

        ptr->size -= a->size;
    }
    return GF_OK;
}

GF_Box *dref_New()
{
    //diag_printf("mmc_MemMalloc .... dref_New \n");
    GF_DataReferenceBox *tmp = (GF_DataReferenceBox *) mmc_MemMalloc(sizeof(GF_DataReferenceBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_DataReferenceBox));

    gf_isom_full_box_init((GF_Box *) tmp);
    tmp->boxList = gf_list_new();
    if (!tmp->boxList)
    {
        //free(tmp);change shenh
        return NULL;
    }
    tmp->type = GF_ISOM_BOX_TYPE_DREF;
    return (GF_Box *)tmp;
}

GF_Err esds_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 descSize;
    int8 *enc_desc;
    uint32 SLIsPredefined(GF_SLConfig *sl);
    GF_ESDBox *ptr = (GF_ESDBox *)s;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    descSize = (uint32) (ptr->size);

    if (descSize)
    {
        //diag_printf("mmc_MemMalloc .... esds_Read \n");
        enc_desc = (int8*)mmc_MemMalloc(sizeof(int8) * descSize);
        if (!enc_desc) return GF_OUT_OF_MEM;
        memset(enc_desc, 0, sizeof(int8) * descSize);
        //get the payload
        gf_bs_read_data(bs, (uint8*)enc_desc, descSize);
        //send it to the OD Codec
        e = gf_odf_desc_read(enc_desc, descSize, (GF_Descriptor **) &ptr->desc);
        //OK, free our desc
        //free(enc_desc);change shenh
        //we do not abbort on error, but skip the descritpor
    }
    return GF_OK;
}

GF_Box *esds_New()
{
    //diag_printf("mmc_MemMalloc .... esds_New \n");
    GF_ESDBox *tmp = (GF_ESDBox *) mmc_MemMalloc(sizeof(GF_ESDBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_ESDBox));

    gf_isom_full_box_init((GF_Box *) tmp);
    tmp->type = GF_ISOM_BOX_TYPE_ESDS;

    return (GF_Box *)tmp;
}

void ftyp_del(GF_Box *s)
{
    //GF_FileTypeBox *ptr = (GF_FileTypeBox *) s;
    /*change shenh
    if (ptr->altBrand) free(ptr->altBrand);
    free(ptr);
    */
}

GF_Box *ftyp_New()
{
    GF_FileTypeBox *tmp;
    //diag_printf("mmc_MemMalloc .... ftyp_New \n");
    tmp = (GF_FileTypeBox *) mmc_MemMalloc(sizeof(GF_FileTypeBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_FileTypeBox));

    tmp->type = GF_ISOM_BOX_TYPE_FTYP;
    return (GF_Box *)tmp;
}

GF_Err ftyp_Read(GF_Box *s,GF_BitStream *bs)
{
    uint32 i;
    GF_FileTypeBox *ptr = (GF_FileTypeBox *)s;

    ptr->majorBrand = gf_bs_read_u32(bs);
    ptr->minorVersion = gf_bs_read_u32(bs);
    ptr->size -= 8;

    ptr->altCount = ( (uint32) (ptr->size)) / 4;
    if (!ptr->altCount) return GF_OK;
    if (ptr->altCount * 4 != (uint32) (ptr->size)) return GF_ISOM_INVALID_FILE;
    //diag_printf("mmc_MemMalloc .... ftyp_Read \n");
    ptr->altBrand = (uint32*)mmc_MemMalloc(sizeof(uint32)*ptr->altCount);
    if(!ptr->altBrand)return GF_BAD_PARAM;
    memset(ptr->altBrand, 0, sizeof(uint32)*ptr->altCount);

    for (i = 0; i<ptr->altCount; i++)
    {
        ptr->altBrand[i] = gf_bs_read_u32(bs);
    }
    return GF_OK;
}

#if 0
GF_Err ftyp_Size(GF_Box *s)
{
    GF_Err e;
    GF_FileTypeBox *ptr = (GF_FileTypeBox *)s;

    e = gf_isom_box_get_size(s);
    if (e) return e;
    ptr->size += 8 + ptr->altCount * 4;
    return GF_OK;
}
#endif

#if 0
void hdlr_del(GF_Box *s)
{
    GF_HandlerBox *ptr = (GF_HandlerBox *)s;
    if (ptr == NULL) return;
    /*change shenh
    if (ptr->nameUTF8) free(ptr->nameUTF8);
    free(ptr);
    */
}
#endif

GF_Err hdlr_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_HandlerBox *ptr = (GF_HandlerBox *)s;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    ptr->reserved1 = gf_bs_read_u32(bs);
    ptr->handlerType = gf_bs_read_u32(bs);

    gf_bs_read_data(bs, ptr->reserved2, 12);
    ptr->size -= 20;
    ptr->nameLength = (uint32) (ptr->size);
    //diag_printf("mmc_MemMalloc .... hdlr_Read \n");
    ptr->nameUTF8 = (int8*)mmc_MemMalloc(ptr->nameLength+1);
    if (ptr->nameUTF8 == NULL) return GF_OUT_OF_MEM;
    memset(ptr->nameUTF8, 0, ptr->nameLength+1);
    gf_bs_read_data(bs, (uint8*)ptr->nameUTF8, ptr->nameLength);
    ptr->nameUTF8[ptr->nameLength]=0;

    return GF_OK;
}

GF_Box *hdlr_New()
{
    //diag_printf("mmc_MemMalloc .... hdlr_New \n");
    GF_HandlerBox *tmp = (GF_HandlerBox *) mmc_MemMalloc(sizeof(GF_HandlerBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_HandlerBox));

    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_HDLR;
    return (GF_Box *)tmp;
}

#if 0
GF_Err hdlr_Size(GF_Box *s)
{
    GF_Err e;
    GF_HandlerBox *ptr = (GF_HandlerBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 20 + ptr->nameLength;
    return GF_OK;
}
#endif

#if 0
void mdat_del(GF_Box *s)
{
    GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
    if (!s) return;
    /*change shenh
    if (ptr->data) free(ptr->data);
    free(ptr);
    */
}
#endif

GF_Err mdat_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
    if (ptr == NULL) return GF_BAD_PARAM;

    ptr->dataSize = s->size;
    //then skip these bytes
    gf_bs_skip_bytes(bs, ptr->dataSize);
    return GF_OK;
}

GF_Box *mdat_New()
{
    //diag_printf("mmc_MemMalloc .... mdat_New \n");
    GF_MediaDataBox *tmp = (GF_MediaDataBox *) mmc_MemMalloc(sizeof(GF_MediaDataBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MediaDataBox));
    tmp->type = GF_ISOM_BOX_TYPE_MDAT;
    return (GF_Box *)tmp;
}

#if 0
GF_Err mdat_Size(GF_Box *s)
{
    GF_Err e;
    GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
    e = gf_isom_box_get_size(s);
    if (e) return e;

    ptr->size += ptr->dataSize;
    return GF_OK;
}
#endif

#if 0
void mdhd_del(GF_Box *s)
{
    GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;
    if (ptr == NULL) return;
    //free(ptr);change shenh
}
#endif

GF_Err mdhd_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    if (ptr->version == 1)
    {
        ptr->creationTime = gf_bs_read_u64(bs);
        ptr->modificationTime = gf_bs_read_u64(bs);
        ptr->timeScale = gf_bs_read_u32(bs);
        ptr->duration = gf_bs_read_u64(bs);
    }
    else
    {
        ptr->creationTime = gf_bs_read_u32(bs);
        ptr->modificationTime = gf_bs_read_u32(bs);
        ptr->timeScale = gf_bs_read_u32(bs);
        ptr->duration = gf_bs_read_u32(bs);
    }
    //our padding bit
    gf_bs_read_int(bs, 1);
//  //the spec is unclear here, just says "the value 0 is interpreted as undetermined"
    ptr->packedLanguage[0] = gf_bs_read_int(bs, 5);
    ptr->packedLanguage[1] = gf_bs_read_int(bs, 5);
    ptr->packedLanguage[2] = gf_bs_read_int(bs, 5);
    //but before or after compaction ?? We assume before
    if (ptr->packedLanguage[0] || ptr->packedLanguage[1] || ptr->packedLanguage[2])
    {
        ptr->packedLanguage[0] += 0x60;
        ptr->packedLanguage[1] += 0x60;
        ptr->packedLanguage[2] += 0x60;
    }
    else
    {
        ptr->packedLanguage[0] = 'u';
        ptr->packedLanguage[1] = 'n';
        ptr->packedLanguage[2] = 'd';
    }
    ptr->reserved = gf_bs_read_u16(bs);
    return GF_OK;
}

GF_Box *mdhd_New()
{
    //diag_printf("mmc_MemMalloc .... mdhd_New \n");
    GF_MediaHeaderBox *tmp = (GF_MediaHeaderBox *) mmc_MemMalloc(sizeof(GF_MediaHeaderBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MediaHeaderBox));

    gf_isom_full_box_init((GF_Box *) tmp);
    tmp->type = GF_ISOM_BOX_TYPE_MDHD;

    tmp->packedLanguage[0] = 'u';
    tmp->packedLanguage[1] = 'n';
    tmp->packedLanguage[2] = 'd';
    return (GF_Box *)tmp;
}

#if 0
GF_Err mdhd_Size(GF_Box *s)
{
    GF_Err e;
    GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 4;
    ptr->size += (ptr->version == 1) ? 28 : 16;
    return GF_OK;
}
#endif

GF_Err mdia_AddBox(GF_Box *s, GF_Box *a)
{
    GF_MediaBox *ptr = (GF_MediaBox *)s;
    switch(a->type)
    {
        case GF_ISOM_BOX_TYPE_MDHD:
            if (ptr->mediaHeader) return GF_ISOM_INVALID_FILE;
            ptr->mediaHeader = (GF_MediaHeaderBox *)a;
            return GF_OK;

        case GF_ISOM_BOX_TYPE_HDLR:
            if (ptr->handler) return GF_ISOM_INVALID_FILE;
            ptr->handler = (GF_HandlerBox *)a;
            return GF_OK;

        case GF_ISOM_BOX_TYPE_MINF:
            if (ptr->information) return GF_ISOM_INVALID_FILE;
            ptr->information = (GF_MediaInformationBox *)a;
            return GF_OK;
    }
    //gf_isom_box_del(a);
    return GF_OK;
}


GF_Err mdia_Read(GF_Box *s, GF_BitStream *bs)
{
    return gf_isom_read_box_list(s, bs, mdia_AddBox);
}

GF_Box *mdia_New()
{
    //diag_printf("mmc_MemMalloc .... mdia_New \n");
    GF_MediaBox *tmp = (GF_MediaBox *) mmc_MemMalloc(sizeof(GF_MediaBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MediaBox));
    tmp->type = GF_ISOM_BOX_TYPE_MDIA;
    return (GF_Box *)tmp;
}

GF_Err minf_AddBox(GF_Box *s, GF_Box *a)
{
    GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_VMHD:
        case GF_ISOM_BOX_TYPE_SMHD:
        case GF_ISOM_BOX_TYPE_HMHD:
            if (ptr->InfoHeader) return GF_ISOM_INVALID_FILE;
            ptr->InfoHeader = a;
            return GF_OK;

        case GF_ISOM_BOX_TYPE_DINF:
            if (ptr->dataInformation ) return GF_ISOM_INVALID_FILE;
            ptr->dataInformation = (GF_DataInformationBox *)a;
            return GF_OK;

        case GF_ISOM_BOX_TYPE_STBL:
            if (ptr->sampleTable ) return GF_ISOM_INVALID_FILE;
            ptr->sampleTable = (GF_SampleTableBox *)a;
            return GF_OK;
    }
    //gf_isom_box_del(a);
    return GF_OK;
}


GF_Err minf_Read(GF_Box *s, GF_BitStream *bs)
{

    return gf_isom_read_box_list(s, bs, minf_AddBox);
}

GF_Box *minf_New()
{
    //diag_printf("mmc_MemMalloc .... minf_New \n");
    GF_MediaInformationBox *tmp = (GF_MediaInformationBox *) mmc_MemMalloc(sizeof(GF_MediaInformationBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MediaInformationBox));
    tmp->type = GF_ISOM_BOX_TYPE_MINF;
    return (GF_Box *)tmp;
}

GF_Err moov_AddBox(GF_Box *s, GF_Box *a)
{
    GF_MovieBox *ptr = (GF_MovieBox *)s;
    switch (a->type )
    {
        case GF_ISOM_BOX_TYPE_MVHD:
            if (ptr->mvhd) return GF_ISOM_INVALID_FILE;
            ptr->mvhd = (GF_MovieHeaderBox *)a;
            return GF_OK;

        case GF_ISOM_BOX_TYPE_TRAK:
            //set our pointer to this obj
            ((GF_TrackBox *)a)->moov = ptr;
            return gf_list_add3(ptr->trackList, a);
        default:
            //gf_isom_box_del(a);
            return GF_OK;
    }
}

GF_Err moov_Read(GF_Box *s, GF_BitStream *bs)
{
    return gf_isom_read_box_list(s, bs, moov_AddBox);
}

GF_Box *moov_New()
{
    //  diag_printf("mmc_MemMalloc .... moov_New 1\n");
    GF_MovieBox *tmp = (GF_MovieBox *) mmc_MemMalloc(sizeof(GF_MovieBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MovieBox));
    tmp->trackList = gf_list_new();

    if (!tmp->trackList)
    {
        //free(tmp);change shenh
        return NULL;
    }
    tmp->type = GF_ISOM_BOX_TYPE_MOOV;
    //add shenh
    //diag_printf("mmc_MemMalloc .... moov_New 2\n");
    tmp->trackList->slots = (void **) mmc_MemMalloc(MAX_TRACK_NUM*sizeof(void *));
    if (! tmp->trackList->slots) return NULL;
    memset(tmp->trackList->slots, 0, MAX_TRACK_NUM*sizeof(void *));
    //
    return (GF_Box *)tmp;
}

#if 0
void mp4a_del(GF_Box *s)
{
    GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;
    if (ptr == NULL) return;
//  if (ptr->esd) gf_isom_box_del((GF_Box *)ptr->esd);
//  if (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
//  if ((ptr->type == GF_ISOM_BOX_TYPE_ENCA) && (ptr->protection_info != NULL) )
//      gf_isom_box_del((GF_Box *)ptr->protection_info);
    //free(ptr);change shenh
}
#endif

GF_Err mp4a_AddBox(GF_Box *s, GF_Box *a)
{
    GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_ESDS:
            //if (ptr->esd) return GF_ISOM_INVALID_FILE;
            ptr->esd = (GF_ESDBox *)a;
            break;
//  case GF_ISOM_BOX_TYPE_SINF:
//      if (ptr->protection_info) return GF_ISOM_INVALID_FILE;
//      ptr->protection_info = (GF_ProtectionInfoBox*)a;
//      break;
        default:
            ;//gf_isom_box_del(a);
            break;
    }
    return GF_OK;
}

GF_Err mp4a_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    e = gf_isom_audio_sample_entry_read((GF_AudioSampleEntryBox*)s, bs);
    if (e) return e;
    return gf_isom_read_box_list(s, bs, mp4a_AddBox);
}

GF_Box *mp4a_New()
{
    //diag_printf("mmc_MemMalloc .... mp4a_New \n");
    GF_MPEGAudioSampleEntryBox *tmp = (GF_MPEGAudioSampleEntryBox *)mmc_MemMalloc(sizeof(GF_MPEGAudioSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MPEGAudioSampleEntryBox));
    tmp->type = GF_ISOM_BOX_TYPE_MP4A;
    gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*)tmp);
    return (GF_Box *)tmp;
}

//add shenh
GF_Err mp3_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    e = gf_isom_audio_sample_entry_read((GF_AudioSampleEntryBox*)s, bs);

    return e;
}

//add shenh
GF_Box *mp3_New()
{
    //diag_printf("mmc_MemMalloc .... mp3_New \n");
    GF_MPEGAudioSampleEntryBox *tmp = (GF_MPEGAudioSampleEntryBox *)mmc_MemMalloc(sizeof(GF_MPEGAudioSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MPEGAudioSampleEntryBox));
    tmp->type = GF_ISOM_BOX_TYPE_MP3;
    gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*)tmp);
    return (GF_Box *)tmp;
}

#if 0
void mp4v_del(GF_Box *s)
{
    GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;
    if (ptr == NULL) return;
    // (ptr->esd) gf_isom_box_del((GF_Box *)ptr->esd);
    // (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
    //free(ptr);change shenh
}
#endif

GF_Err mp4v_AddBox(GF_Box *s, GF_Box *a)
{
    GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_ESDS:
            //if (ptr->esd) return GF_ISOM_INVALID_FILE;
            ptr->esd = (GF_ESDBox *)a;
            break;
//  case GF_ISOM_BOX_TYPE_SINF:
//      if (ptr->protection_info) return GF_ISOM_INVALID_FILE;
//      ptr->protection_info = (GF_ProtectionInfoBox*)a;
//      break;
        default:
            ;//gf_isom_box_del(a);
            break;
    }
    return GF_OK;
}

GF_Err mp4v_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    e = gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *)s, bs);
    if (e) return e;
    return gf_isom_read_box_list(s, bs, mp4v_AddBox);
}

GF_Box *mp4v_New()
{
//diag_printf("mmc_MemMalloc .... mp4v_New \n");
    GF_MPEGVisualSampleEntryBox *tmp = (GF_MPEGVisualSampleEntryBox *) mmc_MemMalloc(sizeof(GF_MPEGVisualSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MPEGVisualSampleEntryBox));
    gf_isom_video_sample_entry_init((GF_VisualSampleEntryBox *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_MP4V;
    return (GF_Box *)tmp;
}

//add shenh

GF_Err avcC_Read(GF_Box *s, GF_BitStream *bs)
{
    //GF_Err e;
    GF_AVCConfig *ptr = (GF_AVCConfig *)s;
    int32 i;

    ptr->configurationVersion=gf_bs_read_u8(bs);
    ptr->AVCProfileIndication=gf_bs_read_u8(bs);
    ptr->profile_compatibility=gf_bs_read_u8(bs);
    ptr->AVCLevelIndication=gf_bs_read_u8(bs);
    ptr->lengthSizeMinusOne=(gf_bs_read_u8(bs))&0x03; //high bit(6) reserved=111111, use low bit(2);
    ptr->numOfSequenceParameterSets=(gf_bs_read_u8(bs))&0x1F;//high bit(3) reserved=111, use low bit(5)
    if(ptr->numOfSequenceParameterSets>0)
    {
        //N sequence Parameter Set,
        ptr->sps=(GF_DefaultDescriptor*)mmc_MemMalloc(sizeof(GF_DefaultDescriptor) * ptr->numOfSequenceParameterSets);
        if (!ptr->sps) return GF_OUT_OF_MEM;
        for(i=0; i<ptr->numOfSequenceParameterSets; i++)
        {
            ptr->sps[i].dataLength=gf_bs_read_u16(bs);
            if(ptr->sps[i].dataLength>0)
            {
                ptr->sps[i].data = (int8*)mmc_MemMalloc(sizeof(int8) * ptr->sps[i].dataLength);
                if (!ptr->sps[i].data) return GF_OUT_OF_MEM;
                gf_bs_read_data(bs, ptr->sps[i].data, ptr->sps[i].dataLength);
            }
        }
    }

    ptr->numOfPictureParameterSets=gf_bs_read_u8(bs);
    if(ptr->numOfPictureParameterSets>0)
    {
        //N picture Parameter Set,
        ptr->pps=(GF_DefaultDescriptor*)mmc_MemMalloc(sizeof(GF_DefaultDescriptor) * ptr->numOfPictureParameterSets);
        if (!ptr->pps) return GF_OUT_OF_MEM;
        for(i=0; i<ptr->numOfPictureParameterSets; i++)
        {
            ptr->pps[i].dataLength=gf_bs_read_u16(bs);
            if(ptr->pps[i].dataLength>0)
            {
                ptr->pps[i].data = (int8*)mmc_MemMalloc(sizeof(int8) * ptr->pps[i].dataLength);
                if (!ptr->pps[i].data) return GF_OUT_OF_MEM;
                gf_bs_read_data(bs, ptr->pps[i].data, ptr->pps[i].dataLength);
            }
        }
    }

    return GF_OK;
}

GF_Box *avcC_New()
{
    GF_AVCConfig *tmp = (GF_AVCConfig *) mmc_MemMalloc(sizeof(GF_AVCConfig));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_AVCConfig));

    //gf_isom_full_box_init((GF_Box *) tmp);
    //tmp->type = GF_ISOM_BOX_TYPE_AVCC;

    return (GF_Box *)tmp;
}

GF_Err avc1_AddBox(GF_Box *s, GF_Box *a)
{
    GF_AVCVisualSampleEntryBox *ptr = (GF_AVCVisualSampleEntryBox *)s;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_AVCC:
            ptr->avcC = (GF_AVCConfig *)a;
            break;
        default:
            ;//gf_isom_box_del(a);
            break;
    }
    return GF_OK;
}

GF_Err avc1_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    e = gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *)s, bs);
    if (e) return e;
    return gf_isom_read_box_list(s, bs, avc1_AddBox);
}

GF_Box *avc1_New()
{
    GF_AVCVisualSampleEntryBox *tmp = (GF_AVCVisualSampleEntryBox *) mmc_MemMalloc(sizeof(GF_AVCVisualSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_AVCVisualSampleEntryBox));
    gf_isom_video_sample_entry_init((GF_VisualSampleEntryBox *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_AVC1;
    return (GF_Box *)tmp;
}


#if 0
void samr_del(GF_Box *s)
{
    GF_3gpAmrSampleEntryBox *ptr = (GF_3gpAmrSampleEntryBox *)s;
    if (ptr == NULL) return;
    //free(ptr);change shenh
}
#endif

GF_Err samr_AddBox(GF_Box *s, GF_Box *a)
{
    GF_3gpAmrSampleEntryBox *ptr = (GF_3gpAmrSampleEntryBox *)s;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_DAMR:
            ptr->amrSp= (amrDecSpecStrc *)a;
            break;
        default:
            ;
            break;
    }
    return GF_OK;
}

GF_Err samr_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    //diag_printf("samr_Read in");
    //e = gf_isom_audio_sample_entry_read((GF_3gpAmrSampleEntryBox*)s, bs);
    e = gf_isom_audio_sample_entry_read((GF_AudioSampleEntryBox*)s, bs);
    if (e) return e;
    e= gf_isom_read_box_list(s, bs, samr_AddBox);
    if(e ==GF_INVALID_BOX_SIZE)
    {
        gf_bs_skip_bytes(bs, s->size);
        e = GF_OK;
    }
    //diag_printf("samr_Read out");
    return e;
}

GF_Box *samr_New()
{
    //diag_printf("samr_New in");
    GF_3gpAmrSampleEntryBox *tmp = (GF_3gpAmrSampleEntryBox*)mmc_MemMalloc(sizeof(GF_3gpAmrSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_3gpAmrSampleEntryBox));
    tmp->type = GF_ISOM_BOX_TYPE_SAMR;
    //gf_isom_audio_sample_entry_init((GF_3gpAmrSampleEntryBox*)tmp);
    gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*)tmp);
    //diag_printf("samr_New out");
    return (GF_Box *)tmp;
}

GF_Err damr_Read(GF_Box *s, GF_BitStream *bs)
{
    amrDecSpecStrc *ptr = (amrDecSpecStrc *)s;

    ptr->vendor= gf_bs_read_u32(bs);
    ptr->decoder_version= gf_bs_read_u8(bs);
    ptr->mode_set= gf_bs_read_u16(bs);
    ptr->mode_change_period= gf_bs_read_u8(bs);
    ptr->frames_per_sample= gf_bs_read_u8(bs);
    return GF_OK;
}

GF_Box *damr_New()
{
    amrDecSpecStrc *tmp = (amrDecSpecStrc *)mmc_MemMalloc(sizeof(amrDecSpecStrc));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(amrDecSpecStrc));
    tmp->type = GF_ISOM_BOX_TYPE_DAMR;
    return (GF_Box *)tmp;
}

#if 0
void s263_del(GF_Box *s)
{
    GF_3gpH263SampleEntryBox *ptr = (GF_3gpH263SampleEntryBox *)s;
    if (ptr == NULL) return;
    //free(ptr);change shenh
}
#endif

GF_Err s263_AddBox(GF_Box *s, GF_Box *a)
{
    GF_3gpH263SampleEntryBox *ptr = (GF_3gpH263SampleEntryBox *)s;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_D263:
            ptr->h263Sp= (h263DecSpecStrc *)a;
            break;
        default:
            ;
            break;
    }
    return GF_OK;
}

GF_Err s263_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    //diag_printf("s263_Read in");
    //e = gf_isom_video_sample_entry_read((GF_3gpH263SampleEntryBox *)s, bs);
    e = gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *)s, bs);
    if (e) return e;
    e= gf_isom_read_box_list(s, bs, s263_AddBox);
    //diag_printf("s263_Read out");
    return e;
}

GF_Box *s263_New()
{
    //diag_printf("s263_New in");
    GF_3gpH263SampleEntryBox *tmp = (GF_3gpH263SampleEntryBox *) mmc_MemMalloc(sizeof(GF_3gpH263SampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_3gpH263SampleEntryBox));
    //gf_isom_video_sample_entry_init((GF_3gpH263SampleEntryBox *)tmp);
    gf_isom_video_sample_entry_init((GF_VisualSampleEntryBox *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_S263;
    //diag_printf("s263_New out");
    return (GF_Box *)tmp;
}

GF_Err d263_Read(GF_Box *s, GF_BitStream *bs)
{
    h263DecSpecStrc *ptr = (h263DecSpecStrc *)s;

    ptr->vendor= gf_bs_read_u32(bs);
    ptr->decoder_version= gf_bs_read_u8(bs);
    ptr->level= gf_bs_read_u8(bs);
    ptr->profile= gf_bs_read_u8(bs);
    if(s->size>7)
        gf_bs_skip_bytes(bs,s->size-7);
    return GF_OK;
}

GF_Box *d263_New()
{
    h263DecSpecStrc *tmp = (h263DecSpecStrc*)mmc_MemMalloc(sizeof(h263DecSpecStrc));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(h263DecSpecStrc));
    tmp->type = GF_ISOM_BOX_TYPE_D263;
    return (GF_Box *)tmp;
}

GF_Err jpeg_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    e = gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *)s, bs);
    return e;
}

GF_Box *jpeg_New()
{

    GF_3gpJpegSampleEntryBox *tmp = (GF_3gpJpegSampleEntryBox *) mmc_MemMalloc(sizeof(GF_3gpJpegSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_3gpJpegSampleEntryBox));
    gf_isom_video_sample_entry_init((GF_VisualSampleEntryBox *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_JPEG;

    return (GF_Box *)tmp;
}

//
#if 0
void mvhd_del(GF_Box *s)
{
    GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
    if (ptr == NULL) return;
    //free(ptr);change shenh
}
#endif

GF_Err mvhd_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
    if (ptr == NULL) return GF_BAD_PARAM;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    if (ptr->version == 1)
    {
        ptr->creationTime = gf_bs_read_u64(bs);
        ptr->modificationTime = gf_bs_read_u64(bs);
        ptr->timeScale = gf_bs_read_u32(bs);
        ptr->duration = gf_bs_read_u64(bs);
    }
    else
    {
        ptr->creationTime = gf_bs_read_u32(bs);
        ptr->modificationTime = gf_bs_read_u32(bs);
        ptr->timeScale = gf_bs_read_u32(bs);
        ptr->duration = gf_bs_read_u32(bs);
    }
    ptr->preferredRate = gf_bs_read_u32(bs);
    ptr->preferredVolume = gf_bs_read_u16(bs);
    gf_bs_read_data(bs, (uint8*)ptr->reserved, 10);
    ptr->matrixA = gf_bs_read_u32(bs);
    ptr->matrixB = gf_bs_read_u32(bs);
    ptr->matrixU = gf_bs_read_u32(bs);
    ptr->matrixC = gf_bs_read_u32(bs);
    ptr->matrixD = gf_bs_read_u32(bs);
    ptr->matrixV = gf_bs_read_u32(bs);
    ptr->matrixX = gf_bs_read_u32(bs);
    ptr->matrixY = gf_bs_read_u32(bs);
    ptr->matrixW = gf_bs_read_u32(bs);
    ptr->previewTime = gf_bs_read_u32(bs);
    ptr->previewDuration = gf_bs_read_u32(bs);
    ptr->posterTime = gf_bs_read_u32(bs);
    ptr->selectionTime = gf_bs_read_u32(bs);
    ptr->selectionDuration = gf_bs_read_u32(bs);
    ptr->currentTime = gf_bs_read_u32(bs);
    ptr->nextTrackID = gf_bs_read_u32(bs);
    return GF_OK;
}

GF_Box *mvhd_New()
{
//diag_printf("mmc_MemMalloc .... mvhd_New \n");
    GF_MovieHeaderBox *tmp = (GF_MovieHeaderBox *) mmc_MemMalloc(sizeof(GF_MovieHeaderBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_MovieHeaderBox));

    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_MVHD;
    tmp->preferredRate = (1<<16);
    tmp->preferredVolume = (1<<8);

    tmp->matrixA = (1<<16);
    tmp->matrixD = (1<<16);
    tmp->matrixW = (1<<30);

    tmp->nextTrackID = 1;

    return (GF_Box *)tmp;
}

#if 0
GF_Err mvhd_Size(GF_Box *s)
{
    GF_Err e;
    GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += (ptr->version == 1) ? 28 : 16;
    ptr->size += 80;
    return GF_OK;
}
#endif

#if 0
void smhd_del(GF_Box *s)
{
    GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;
    if (ptr == NULL ) return;
    //free(ptr);change shenh
}
#endif

GF_Err smhd_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    ptr->reserved = gf_bs_read_u32(bs);
    return GF_OK;
}

GF_Box *smhd_New()
{
//diag_printf("mmc_MemMalloc .... smhd_New \n");
    GF_SoundMediaHeaderBox *tmp = (GF_SoundMediaHeaderBox *) mmc_MemMalloc(sizeof(GF_SoundMediaHeaderBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_SoundMediaHeaderBox));
    gf_isom_full_box_init((GF_Box *) tmp);
    tmp->type = GF_ISOM_BOX_TYPE_SMHD;
    return (GF_Box *)tmp;
}

#if 0
GF_Err smhd_Size(GF_Box *s)
{
    GF_Err e;
    GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->reserved = 0;
    ptr->size += 4;
    return GF_OK;
}
#endif

GF_Err stbl_AddBox(GF_SampleTableBox *ptr, GF_Box *a)
{
    if (!a) return GF_OK;
    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_STTS:
            if (ptr->TimeToSample) return GF_ISOM_INVALID_FILE;
            ptr->TimeToSample = (GF_TimeToSampleBox *)a;
            break;
        case GF_ISOM_BOX_TYPE_STSS:
            if (ptr->SyncSample) return GF_ISOM_INVALID_FILE;
            ptr->SyncSample = (GF_SyncSampleBox *)a;
            break;
        case GF_ISOM_BOX_TYPE_STSD:
            if (ptr->SampleDescription) return GF_ISOM_INVALID_FILE;
            ptr->SampleDescription  =(GF_SampleDescriptionBox *)a;
            break;
        case GF_ISOM_BOX_TYPE_STZ2:
        case GF_ISOM_BOX_TYPE_STSZ:
            if (ptr->SampleSize) return GF_ISOM_INVALID_FILE;
            ptr->SampleSize = (GF_SampleSizeBox *)a;
            break;
        case GF_ISOM_BOX_TYPE_STSC:
            if (ptr->SampleToChunk) return GF_ISOM_INVALID_FILE;
            ptr->SampleToChunk = (GF_SampleToChunkBox *)a;
            break;
        case GF_ISOM_BOX_TYPE_CO64:
        case GF_ISOM_BOX_TYPE_STCO:
            if (ptr->ChunkOffset) return GF_ISOM_INVALID_FILE;
            ptr->ChunkOffset = (GF_ChunkOffset *)a;
            return GF_OK;
        //what's this box ??? delete it
        default:
            ;//isom_box_del(a);
    }

    return GF_OK;
}

GF_Err stbl_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_Box *a;
    //we need to parse DegPrior in a special way
    //GF_Err stdp_Read(GF_Box *s, GF_BitStream *bs);
    GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;

    while (ptr->size)
    {
        e = gf_isom_parse_box(&a, bs);
        if(e==GF_INVALID_BOX)//sheen
        {
            if (ptr->size<(uint32)a) return GF_ISOM_INVALID_FILE;
            ptr->size -= (uint32)a;//recode the size.sheen
            continue;//skip this box
        }
        if (e) return e;
        e = stbl_AddBox(ptr, a);
        if (e) return e;
        if (ptr->size<a->size) return GF_ISOM_INVALID_FILE;
        ptr->size -= a->size;
    }
    return GF_OK;
}

GF_Box *stbl_New()
{

    GF_SampleTableBox *tmp = (GF_SampleTableBox *) mmc_MemMalloc(sizeof(GF_SampleTableBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_SampleTableBox));

    tmp->type = GF_ISOM_BOX_TYPE_STBL;
    //maxSamplePer chunk is 10 by default
//  tmp->MaxSamplePerChunk = 7901;
//  tmp->groupID = 1;
    return (GF_Box *)tmp;
}

#if 0
void stco_del(GF_Box *s)
{
    GF_ChunkOffsetBox *ptr = (GF_ChunkOffsetBox *)s;
    if (ptr == NULL) return;
    /*change shenh
    if (ptr->offsets) free(ptr->offsets);
    free(ptr);
    */
}
#endif

GF_Err stco_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 entries;
    GF_ChunkOffsetBox *ptr = (GF_ChunkOffsetBox *)s;
    uint32 res;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    ptr->entryCount = gf_bs_read_u32(bs);

    if (ptr->entryCount)
    {
        uint8 *p;
        uint8 tmp;
        //diag_printf("mmc_MemMalloc .... stco_Read \n");
        //ptr->offsets = (uint32 *) mmc_MemMalloc(ptr->entryCount * sizeof(uint32) );//shenh
        ptr->offsets = (uint32 *) mmc_MemMalloc(INDEX_BUF_SIZE* sizeof(uint32) );
        if (ptr->offsets == NULL) return GF_OUT_OF_MEM;
        memset(ptr->offsets, 0,INDEX_BUF_SIZE* sizeof(uint32) );
        // change shenh
        /*
        for (entries = 0; entries < ptr->entryCount; entries++) {
            ptr->offsets[entries] = gf_bs_read_u32(bs);
        }*/

        /*shenh
        if(gf_bs_read_data(bs, (unsigned char *)(ptr->offsets), ptr->entryCount * sizeof(uint32))!=ptr->entryCount * sizeof(uint32))
        {
            diag_printf(" stco_Read err \n");
            return GF_IO_ERR;
        }
        */

        ptr->offsetInFile=bs->position;//add shenh
        ptr->firstRealInBuf=0;//add shenh

        if(ptr->entryCount<=INDEX_BUF_SIZE)
        {
            if((res=gf_bs_read_data(bs, (uint8 *)(ptr->offsets), ptr->entryCount * sizeof(uint32)))!=ptr->entryCount * sizeof(uint32))
            {
                diag_printf(" stco_Read err \n");
                return GF_IO_ERR;
            }
            ptr->countInBuf=ptr->entryCount;//add shenh
        }
        else
        {
            if((res=gf_bs_read_data(bs, (uint8 *)(ptr->offsets), INDEX_BUF_SIZE * sizeof(uint32)))!=INDEX_BUF_SIZE* sizeof(uint32))
            {
                diag_printf(" stco_Read err \n");
                return GF_IO_ERR;
            }
            ptr->countInBuf=INDEX_BUF_SIZE;//add shenh

            //FS_Seek((INT32)bs->stream, (ptr->entryCount-INDEX_BUF_SIZE)*sizeof(uint32), FS_SEEK_CUR);//move the file offset
            mp4_fs_seek(bs, (ptr->entryCount-INDEX_BUF_SIZE)*sizeof(uint32), FS_SEEK_CUR);//move the file offset
            bs->position+=(ptr->entryCount-INDEX_BUF_SIZE)*sizeof(uint32);
        }

        /*shenh
        for (entries = 0; entries < ptr->entryCount; entries++) {
            p=(unsigned char *)&ptr->offsets[entries];
            tmp=*p;
            *p=*(p+3);
            *(p+3)=tmp;
            tmp=*(p+1);
            *(p+1)=*(p+2);
            *(p+2)=tmp;
        }
        */
        for (entries = 0; entries < ptr->countInBuf; entries++)
        {
            p=(uint8 *)(ptr->offsets+entries);
            tmp=*p;
            *p=*(p+3);
            *(p+3)=tmp;
            tmp=*(p+1);
            *(p+1)=*(p+2);
            *(p+2)=tmp;
        }


    }
    return GF_OK;
}

GF_Box *stco_New()
{

    GF_ChunkOffsetBox *tmp = (GF_ChunkOffsetBox *) mmc_MemMalloc(sizeof(GF_ChunkOffsetBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_ChunkOffsetBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_STCO;
    return (GF_Box *)tmp;
}

#if 0
GF_Err stco_Size(GF_Box *s)
{
    GF_Err e;
    GF_ChunkOffsetBox *ptr = (GF_ChunkOffsetBox *)s;

    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 4 + (4 * ptr->entryCount);
    return GF_OK;
}
#endif

#if 0
void stsc_del(GF_Box *s)
{
    uint32 entryCount;
    uint32 i;
    GF_StscEntry *ent;
    GF_SampleToChunkBox *ptr = (GF_SampleToChunkBox *)s;
    if (ptr == NULL) return;

    entryCount = gf_list_count(ptr->entryList);
    if (entryCount)
    {
        for (i = 0; i < entryCount; i++)
        {
            ent = (GF_StscEntry*)gf_list_get(ptr->entryList, i);
            //if (ent) free(ent);change shenh
        }
    }
    gf_list_del(ptr->entryList);
    //free(ptr);change shenh
}
#endif

GF_Err stsc_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 i;
    uint32 entryCount;
    GF_StscEntry *ent, *firstEnt;
    GF_SampleToChunkBox *ptr = (GF_SampleToChunkBox *)s;

    GF_StscEntry *pStsc=NULL;//shenh add
    uint32 countHaveRead;//shenh add
    uint32 countRemainInBuf;//shenh add
    uint32 countMaxRead=BUFGLOB_SIZE/12;//shenh add
    uint8 *pBufTemp=NULL;//shenh add

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    entryCount = gf_bs_read_u32(bs);

    //shenh add
    pStsc=(GF_StscEntry *)mmc_MemMalloc(entryCount*sizeof(GF_StscEntry));
    if (!pStsc) return GF_OUT_OF_MEM;
    memset(pStsc, 0,entryCount*sizeof(GF_StscEntry));

    if(!ptr)return GF_BAD_PARAM;
    if(!ptr->entryList)return GF_BAD_PARAM;
    ptr->entryList->slots=(void**)mmc_MemMalloc(entryCount*sizeof(void*));
    if (!ptr->entryList->slots) return GF_OUT_OF_MEM;
    memset(ptr->entryList->slots, 0, entryCount*sizeof(void*));
    //

    firstEnt = NULL;
    countHaveRead=0;
    countRemainInBuf=0;
    pBufTemp=pBufGlob;
    if(!pBufTemp)return GF_BAD_PARAM;
    for (i = 0; i < entryCount; i++)
    {
        //diag_printf("mmc_MemMalloc .... stsc_Read \n");
        //ent = (GF_StscEntry *)mmc_MemMalloc(sizeof(GF_StscEntry));//shenh change
        //if (!ent) return GF_OUT_OF_MEM;

        //add shenh
        if(countRemainInBuf==0 && countHaveRead<entryCount)
        {
            uint32 res;
            if(entryCount-countHaveRead>=countMaxRead)
                res=countMaxRead;
            else
                res=entryCount-countHaveRead;

            if(gf_bs_read_data(bs, pBufGlob, res*12)!=res*12)
                return GF_BAD_PARAM;
            countHaveRead+=res;
            countRemainInBuf=res;
            pBufTemp=pBufGlob;
        }
        //
        ent=pStsc+i;//shenh

        if(countRemainInBuf && (pBufTemp-pBufGlob)<(countMaxRead*12))
        {
            ent->firstChunk =gf_buf_read_u32(pBufTemp);//shenh change gf_bs_read_u32(bs);
            pBufTemp+=4;//add shenh
            ent->samplesPerChunk = gf_buf_read_u32(pBufTemp);//shenh changegf_bs_read_u32(bs);
            pBufTemp+=4;//add shenh
            ent->sampleDescriptionIndex = gf_buf_read_u32(pBufTemp);//shenh changegf_bs_read_u32(bs);
            pBufTemp+=4;//add shenh
            countRemainInBuf--;//add shenh
        }
        else
            return GF_BAD_PARAM;
        //  ent->isEdited = 0;
        ent->nextChunk = 0;

        //create our cache at first load
        if (! ptr->currentEntry)
        {
            firstEnt = ent;
        }
        else
        {
            //update the next chunk
            ptr->currentEntry->nextChunk = ent->firstChunk;
        }
        ptr->currentEntry = ent;
        //e = gf_list_add(ptr->entryList, ent);
        //if (e) return e;
        //shenh change
        ptr->entryList->entryCount ++;
        ptr->entryList->slots[ptr->entryList->entryCount-1] = ent;
        //

    }
    //create our cache
    if (firstEnt)
    {
        ptr->currentEntry = firstEnt;
        ptr->currentIndex = 0;
        ptr->firstSampleInCurrentChunk = 0;
        ptr->currentChunk = 0;
        ptr->ghostNumber = 0;
    }
    return GF_OK;
}

GF_Box *stsc_New()
{
    //diag_printf("mmc_MemMalloc .... stsc_New \n");
    GF_SampleToChunkBox *tmp = (GF_SampleToChunkBox *) mmc_MemMalloc(sizeof(GF_SampleToChunkBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_SampleToChunkBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->entryList = gf_list_new();
    if (tmp->entryList == NULL)
    {
        //free(tmp);change shenh
        return NULL;
    }
    tmp->type = GF_ISOM_BOX_TYPE_STSC;
    return (GF_Box *)tmp;
}

#if 0
GF_Err stsc_Size(GF_Box *s)
{
    GF_Err e;
    GF_SampleToChunkBox *ptr = (GF_SampleToChunkBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 4 + (12 * gf_list_count(ptr->entryList));
    return GF_OK;
}
#endif

GF_Err stsd_AddBox(GF_SampleDescriptionBox *ptr, GF_Box *a)
{
    GF_UnknownBox *def;
    if (!a) return GF_OK;
    if(!ptr)return GF_BAD_PARAM;
    if(!ptr->boxList)return GF_BAD_PARAM;
    if(!ptr->boxList->slots)return GF_BAD_PARAM;

    switch (a->type)
    {
        case GF_ISOM_BOX_TYPE_MP4S:

        case GF_ISOM_BOX_TYPE_MP4A:

        case GF_ISOM_BOX_TYPE_MP4V:

        case GF_ISOM_BOX_TYPE_MP3:
            //return gf_list_add(ptr->boxList, a);//change shenh
            ptr->boxList->entryCount ++;
            ptr->boxList->slots[ptr->boxList->entryCount-1] = a;
            return GF_OK;
        //add shenh
        case GF_ISOM_BOX_TYPE_S263:
        case GF_ISOM_BOX_TYPE_SAMR:
        case GF_ISOM_BOX_TYPE_AVC1:
        case GF_ISOM_BOX_TYPE_JPEG:
            //return gf_list_add(ptr->boxList, a);change shenh
            ptr->boxList->entryCount ++;
            ptr->boxList->slots[ptr->boxList->entryCount-1] = a;
            return GF_OK;
        //

        //unknown sample description: we need a specific box to handle the data ref index
        //rather than a default box ...
        default:
            def = (GF_UnknownBox *)a;
            /*we need at least 8 bytes for unknown sample entries*/
            if (def->dataSize < 8)
            {
                //gf_isom_box_del(a);
                return GF_OK;
            }
            //return gf_list_add(ptr->boxList, a);change shenh
            ptr->boxList->entryCount ++;
            ptr->boxList->slots[ptr->boxList->entryCount-1] = a;
            return GF_OK;
    }
}


GF_Err stsd_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 entryCount;
    uint32 i;
    GF_Box *a;
    GF_SampleDescriptionBox *ptr = (GF_SampleDescriptionBox *)s;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    entryCount = gf_bs_read_u32(bs);

    if(!ptr)return GF_BAD_PARAM;
    if(!ptr->boxList)return GF_BAD_PARAM;
    ptr->boxList->slots = (void**)mmc_MemMalloc(entryCount*sizeof(void*));//add shenh
    if (!ptr->boxList->slots)
    {
        ptr->boxList->entryCount = 0;
        return GF_OUT_OF_MEM;
    }
    memset(ptr->boxList->slots, 0, entryCount*sizeof(void*));

    for (i = 0; i < entryCount; i++)
    {
        e = gf_isom_parse_box(&a, bs);
        if(e==GF_INVALID_BOX)//sheen
        {
            continue;//skip this box
        }
        if (e) return e;
        e = stsd_AddBox(ptr, a);
        if (e) return e;
    }
    return GF_OK;
}

GF_Box *stsd_New()
{
    //diag_printf("mmc_MemMalloc .... stsd_New \n");
    GF_SampleDescriptionBox *tmp = (GF_SampleDescriptionBox *) mmc_MemMalloc(sizeof(GF_SampleDescriptionBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_SampleDescriptionBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->boxList = gf_list_new();
    if (! tmp->boxList)
    {
        //free(tmp);change shenh
        return NULL;
    }
    tmp->type = GF_ISOM_BOX_TYPE_STSD;
    return (GF_Box *)tmp;
}

#if 0
void stss_del(GF_Box *s)
{
    GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;
    if (ptr == NULL) return;
    /*change shenh
    if (ptr->sampleNumbers) free(ptr->sampleNumbers);
    free(ptr);
    */
}
#endif

GF_Err stss_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 i;
    GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;
    uint8 *p;
    uint8 tmp;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    ptr->entryCount = gf_bs_read_u32(bs);
    if(ptr->entryCount>0)
    {
        //diag_printf("mmc_MemMalloc .... stss_Read \n");
        //ptr->sampleNumbers = (uint32 *) mmc_MemMalloc( (ptr->entryCount + 1) * sizeof(uint32)); change shenh
        ptr->sampleNumbers = (uint32 *) mmc_MemMalloc( ptr->entryCount * sizeof(uint32));
        if (ptr->sampleNumbers == NULL) return GF_OUT_OF_MEM;
        memset(ptr->sampleNumbers , 0, ptr->entryCount * sizeof(uint32));
        // change shenh
        /*
        for (i = 0; i < ptr->entryCount; i++) {
            ptr->sampleNumbers[i] = gf_bs_read_u32(bs);
        }
        */
        if(gf_bs_read_data(bs, (uint8 *)(ptr->sampleNumbers), ptr->entryCount  * sizeof(uint32))!=ptr->entryCount * sizeof(uint32))
        {
            diag_printf(" stss_Read err \n");
            return GF_IO_ERR;
        }

        for (i = 0; i < ptr->entryCount; i++)
        {
            p=(uint8 *)&ptr->sampleNumbers[i];
            tmp=*p;
            *p=*(p+3);
            *(p+3)=tmp;
            tmp=*(p+1);
            *(p+1)=*(p+2);
            *(p+2)=tmp;
        }
    }
    else
    {
        //every frame is key
        ptr->entryCount=0;
        ptr->sampleNumbers=0;
    }
    return GF_OK;
}

GF_Box *stss_New()
{
    //diag_printf("mmc_MemMalloc .... stss_New \n");
    GF_SyncSampleBox *tmp = (GF_SyncSampleBox *) mmc_MemMalloc(sizeof(GF_SyncSampleBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_SyncSampleBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_STSS;
    return (GF_Box*)tmp;
}

#if 0
GF_Err stss_Size(GF_Box *s)
{
    GF_Err e;
    GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 4 + (4 * ptr->entryCount);
    return GF_OK;
}
#endif


#if 0
void stsz_del(GF_Box *s)
{
    GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;
    if (ptr == NULL) return;
    /*change shenh
    if (ptr->sizes) free(ptr->sizes);
    free(ptr);
    */
}
#endif

GF_Err stsz_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 i, estSize;
    GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;
    uint32 res;

    if (ptr == NULL) return GF_BAD_PARAM;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    //support for CompactSizes
    if (s->type == GF_ISOM_BOX_TYPE_STSZ)
    {
        ptr->sampleSize = gf_bs_read_u32(bs);
        ptr->sampleCount = gf_bs_read_u32(bs);
        ptr->size -= 8;
    }
    else
    {
        //24-reserved
        gf_bs_read_int(bs, 24);
        i = gf_bs_read_u8(bs);
        ptr->sampleCount = gf_bs_read_u32(bs);
        ptr->size -= 8;
        switch (i)
        {
            case 4:
            case 8:
            case 16:
                ptr->sampleSize = i;
                break;
            default:
                //try to fix the file
                //no samples, no parsing pb
                if (!ptr->sampleCount)
                {
                    ptr->sampleSize = 16;
                    return GF_OK;
                }
                estSize = (uint32) (ptr->size) / ptr->sampleCount;
                if (!estSize && ((ptr->sampleCount+1)/2 == (ptr->size)) )
                {
                    ptr->sampleSize = 4;
                    break;
                }
                else if (estSize == 1 || estSize == 2)
                {
                    ptr->sampleSize = 8 * estSize;
                }
                else
                {
                    return GF_ISOM_INVALID_FILE;
                }
        }
    }
    if (s->type == GF_ISOM_BOX_TYPE_STSZ)
    {
        if (! ptr->sampleSize && ptr->sampleCount)
        {
            uint8 *p;
            uint8 tmp;
            //diag_printf("mmc_MemMalloc .... stsz_Read 1\n");
            //ptr->sizes = (uint32 *) mmc_MemMalloc(ptr->sampleCount * sizeof(uint32));shenh
            ptr->sizes = (uint32 *) mmc_MemMalloc(INDEX_BUF_SIZE* sizeof(uint32));
            if (! ptr->sizes) return GF_OUT_OF_MEM;
            memset(ptr->sizes  , 0, INDEX_BUF_SIZE* sizeof(uint32));
            // change shenh
            /*
            for (i = 0; i < ptr->sampleCount; i++) {
                ptr->sizes[i] = gf_bs_read_u32(bs);
            }
            */
            /*shenh
            if(gf_bs_read_data(bs, (unsigned char *)ptr->sizes, ptr->sampleCount * sizeof(uint32))!=ptr->sampleCount * sizeof(uint32))
            {
                diag_printf(" stsz_Read err\n");
                return GF_IO_ERR;
            }
            */

            ptr->offsetInFile=bs->position;//shenh
            ptr->firstRealInBuf=0;//shenh

            if(ptr->sampleCount<=INDEX_BUF_SIZE)
            {
                if((res=gf_bs_read_data(bs, (uint8 *)ptr->sizes, ptr->sampleCount * sizeof(uint32)))!=ptr->sampleCount * sizeof(uint32))
                {
                    diag_printf(" stsz_Read err\n");
                    return GF_IO_ERR;
                }
                ptr->countInBuf=ptr->sampleCount;//add shenh
            }
            else
            {
                if((res=gf_bs_read_data(bs, (uint8 *)ptr->sizes,INDEX_BUF_SIZE * sizeof(uint32)))!=INDEX_BUF_SIZE * sizeof(uint32))
                {
                    diag_printf(" stsz_Read err\n");
                    return GF_IO_ERR;
                }
                ptr->countInBuf=INDEX_BUF_SIZE;//add shenh

                //FS_Seek((INT32)bs->stream, (ptr->sampleCount-INDEX_BUF_SIZE)*sizeof(uint32), FS_SEEK_CUR);//move the file offset
                mp4_fs_seek(bs, (ptr->sampleCount-INDEX_BUF_SIZE)*sizeof(uint32), FS_SEEK_CUR);//move the file offset
                bs->position+=(ptr->sampleCount-INDEX_BUF_SIZE)*sizeof(uint32);
            }
            /*shenh
            for (i = 0; i < ptr->sampleCount; i++) {
                p=(unsigned char *)&ptr->sizes[i];
                tmp=*p;
                *p=*(p+3);
                *(p+3)=tmp;
                tmp=*(p+1);
                *(p+1)=*(p+2);
                *(p+2)=tmp;
            }
            */
            for (i = 0; i < ptr->countInBuf; i++)
            {
                p=(uint8 *)(ptr->sizes+i);
                tmp=*p;
                *p=*(p+3);
                *(p+3)=tmp;
                tmp=*(p+1);
                *(p+1)=*(p+2);
                *(p+2)=tmp;
            }
        }
    }
    else
    {

        //note we could optimize the mem usage by keeping the table compact
        //in memory. But that would complicate both caching and editing
        //we therefore keep all sizes as uint32 and uncompress the table
        //diag_printf("mmc_MemMalloc .... stsz_Read 2\n");

        /*shenh
        ptr->sizes = (uint32 *) mmc_MemMalloc(ptr->sampleCount * sizeof(uint32));
        if (! ptr->sizes) return GF_OUT_OF_MEM;

        for (i = 0; i < ptr->sampleCount; ) {
            switch (ptr->sampleSize) {
            case 4:
                ptr->sizes[i] = gf_bs_read_int(bs, 4);
                if (i+1 < ptr->sampleCount) {
                    ptr->sizes[i+1] = gf_bs_read_int(bs, 4);
                } else {
                    //0 padding in odd sample count
                    gf_bs_read_int(bs, 4);
                }
                i += 2;
                break;
            default:
                ptr->sizes[i] = gf_bs_read_int(bs, ptr->sampleSize);
                i += 1;
                break;
            }
        }
        */
        diag_printf("[WARNING!!! ] stsz_Read GF_NOT_SUPPORTED \n");
        return GF_NOT_SUPPORTED;//shenh add
    }

    return GF_OK;
}

GF_Box *stsz_New()
{

    GF_SampleSizeBox *tmp = (GF_SampleSizeBox *) mmc_MemMalloc(sizeof(GF_SampleSizeBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_SampleSizeBox));

    gf_isom_full_box_init((GF_Box *)tmp);
    //type is unknown here, can be regular or compact table
    return (GF_Box *)tmp;
}

#if 0
GF_Err stsz_Size(GF_Box *s)
{
    GF_Err e;
    uint32 i, fieldSize, size;
    GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;

    ptr->size += 8;
    if (!ptr->sampleCount) return GF_OK;

    //regular table
    if (ptr->type == GF_ISOM_BOX_TYPE_STSZ)
    {
        if (ptr->sampleSize) return GF_OK;
        ptr->size += (4 * ptr->sampleCount);
        return GF_OK;
    }

    fieldSize = 4;
    size = ptr->sizes[0];

    for (i=0; i < ptr->sampleCount; i++)
    {
        if (ptr->sizes[i] <= 0xF) continue;
        //switch to 8-bit table
        else if (ptr->sizes[i] <= 0xFF)
        {
            fieldSize = 8;
        }
        //switch to 16-bit table
        else if (ptr->sizes[i] <= 0xFFFF)
        {
            fieldSize = 16;
        }
        //switch to 32-bit table
        else
        {
            fieldSize = 32;
        }

        //check the size
        if (size != ptr->sizes[i]) size = 0;
    }
    //if all samples are of the same size, switch to regular (more compact)
    if (size)
    {
        ptr->type = GF_ISOM_BOX_TYPE_STSZ;
        ptr->sampleSize = size;
        /*change shenh
        free(ptr->sizes);
        ptr->sizes = NULL;
        */
    }

    if (fieldSize == 32)
    {
        //oops, doesn't fit in a compact table
        ptr->type = GF_ISOM_BOX_TYPE_STSZ;
        ptr->size += (4 * ptr->sampleCount);
        return GF_OK;
    }

    //make sure we are a compact table (no need to change the mem representation)
    ptr->type = GF_ISOM_BOX_TYPE_STZ2;
    ptr->sampleSize = fieldSize;
    if (fieldSize == 4)
    {
        //do not forget the 0 padding field for odd count
        ptr->size += (ptr->sampleCount + 1) / 2;
    }
    else
    {
        ptr->size += (ptr->sampleCount) * (fieldSize/8);
    }
    return GF_OK;
}
#endif

#if 0
void stts_del(GF_Box *s)
{
    uint32 i;
    GF_SttsEntry *ent;
    GF_TimeToSampleBox *ptr = (GF_TimeToSampleBox *)s;
    if (ptr == NULL) return;
    if (ptr->entryList)
    {
        for (i = 0; i < gf_list_count(ptr->entryList); i++)
        {
            ent = (GF_SttsEntry*)gf_list_get(ptr->entryList, i);
            //if (ent) free(ent);change shenh
        }
        gf_list_del(ptr->entryList);
    }
    //free(ptr);change shenh
}
#endif

GF_Err stts_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    uint32 i;
    uint32 entryCount;
    GF_SttsEntry *ent;
    GF_TimeToSampleBox *ptr = (GF_TimeToSampleBox *)s;

    GF_SttsEntry *pEnt=NULL;//add shenh

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;

    ent = NULL;

    //ptr->w_LastDTS = 0;
    entryCount = gf_bs_read_u32(bs);

    pEnt= (GF_SttsEntry *) mmc_MemMalloc(entryCount*sizeof(GF_SttsEntry));//add shenh
    if (!pEnt) return GF_OUT_OF_MEM;
    memset(pEnt, 0, entryCount*sizeof(GF_SttsEntry));

    if(!ptr->entryList)return GF_BAD_PARAM;
    ptr->entryList->slots = (void**)mmc_MemMalloc(entryCount*sizeof(void*));//add shenh
    if (!ptr->entryList->slots)
    {
        ptr->entryList->entryCount = 0;
        return GF_OUT_OF_MEM;
    }
    memset(ptr->entryList->slots , 0, entryCount*sizeof(void*));

    if(gf_bs_read_data(bs, (uint8*)pEnt, entryCount*8)!=entryCount*8)//shenh add
        return GF_BAD_PARAM;

    for (i = 0; i < entryCount; i++)
    {
        //diag_printf("mmc_MemMalloc .... stts_Read \n");
        //ent = (GF_SttsEntry *) mmc_MemMalloc(sizeof(GF_SttsEntry));change shenh
        //if (! ent) return GF_OUT_OF_MEM;
        ent=pEnt+i;

        //ent->sampleCount = gf_bs_read_u32(bs);//shenh change
        //ent->sampleDelta = gf_bs_read_u32(bs);
        ent->sampleCount=gf_buf_read_u32((uint8*)(&(ent->sampleCount)));
        ent->sampleDelta=gf_buf_read_u32((uint8*)(&(ent->sampleDelta)));


        //e = gf_list_add(ptr->entryList, ent);change shenh
        //if (e) return e;
        ptr->entryList->entryCount ++;
        ptr->entryList->slots[ptr->entryList->entryCount-1] = ent;

        //ptr->w_currentEntry = ent;
        //ptr->w_currentSampleNum += ent->sampleCount;

    }
    //remove the last sample delta.
//  if (ent) ptr->w_LastDTS -= ent->sampleDelta;

    return GF_OK;
}

GF_Box *stts_New()
{
    //diag_printf("mmc_MemMalloc .... stts_New \n");
    GF_TimeToSampleBox *tmp = (GF_TimeToSampleBox *) mmc_MemMalloc(sizeof(GF_TimeToSampleBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_TimeToSampleBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->entryList = gf_list_new();
    if (!tmp->entryList)
    {
        //free(tmp);change shenh
        return NULL;
    }
    tmp->type = GF_ISOM_BOX_TYPE_STTS;
    return (GF_Box *)tmp;
}

#if 0
void tkhd_del(GF_Box *s)
{
    GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;
    if (ptr == NULL) return;
    //free(ptr);change shenh
    return;
}
#endif

GF_Err tkhd_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    if (ptr->version == 1)
    {
        ptr->creationTime = gf_bs_read_u64(bs);
        ptr->modificationTime = gf_bs_read_u64(bs);
        ptr->trackID = gf_bs_read_u32(bs);
        ptr->reserved1 = gf_bs_read_u32(bs);
        ptr->duration = gf_bs_read_u64(bs);
    }
    else
    {
        ptr->creationTime = gf_bs_read_u32(bs);
        ptr->modificationTime = gf_bs_read_u32(bs);
        ptr->trackID = gf_bs_read_u32(bs);
        ptr->reserved1 = gf_bs_read_u32(bs);
        ptr->duration = gf_bs_read_u32(bs);
    }
    ptr->reserved2[0] = gf_bs_read_u32(bs);
    ptr->reserved2[1] = gf_bs_read_u32(bs);
    ptr->layer = gf_bs_read_u16(bs);
    ptr->alternate_group = gf_bs_read_u16(bs);
    ptr->volume = gf_bs_read_u16(bs);
    ptr->reserved3 = gf_bs_read_u16(bs);
    ptr->matrix[0] = gf_bs_read_u32(bs);
    ptr->matrix[1] = gf_bs_read_u32(bs);
    ptr->matrix[2] = gf_bs_read_u32(bs);
    ptr->matrix[3] = gf_bs_read_u32(bs);
    ptr->matrix[4] = gf_bs_read_u32(bs);
    ptr->matrix[5] = gf_bs_read_u32(bs);
    ptr->matrix[6] = gf_bs_read_u32(bs);
    ptr->matrix[7] = gf_bs_read_u32(bs);
    ptr->matrix[8] = gf_bs_read_u32(bs);
    ptr->width = gf_bs_read_u32(bs);
    ptr->height = gf_bs_read_u32(bs);
    return GF_OK;
}

GF_Box *tkhd_New()
{
    //diag_printf("mmc_MemMalloc .... tkhd_New \n");
    GF_TrackHeaderBox *tmp = (GF_TrackHeaderBox *) mmc_MemMalloc(sizeof(GF_TrackHeaderBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_TrackHeaderBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->type = GF_ISOM_BOX_TYPE_TKHD;
    tmp->matrix[0] = 0x00010000;
    tmp->matrix[4] = 0x00010000;
    tmp->matrix[8] = 0x40000000;
    return (GF_Box *)tmp;
}

#if 0
GF_Err tkhd_Size(GF_Box *s)
{
    GF_Err e;
    GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;

    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += (ptr->version == 1) ? 32 : 20;
    ptr->size += 60;
    return GF_OK;
}
#endif

GF_Err trak_AddBox(GF_Box *s, GF_Box *a)
{
    GF_TrackBox *ptr = (GF_TrackBox *)s;
    if (!a) return GF_OK;
    switch(a->type)
    {
        case GF_ISOM_BOX_TYPE_TKHD:
            if (ptr->Header) return GF_ISOM_INVALID_FILE;
            ptr->Header = (GF_TrackHeaderBox *)a;
            return GF_OK;
        case GF_ISOM_BOX_TYPE_MDIA:
            if (ptr->Media) return GF_ISOM_INVALID_FILE;
            ptr->Media = (GF_MediaBox *)a;
            ((GF_MediaBox *)a)->mediaTrack = ptr;
            return GF_OK;
    }
    ;//gf_isom_box_del(a);
    return GF_OK;
}

static void gf_isom_check_sample_desc(GF_TrackBox *trak)
{
    //GF_BitStream *bs;
    //GF_GenericVisualSampleEntryBox *genv;
    //GF_GenericAudioSampleEntryBox *gena;
    //GF_GenericSampleEntryBox *genm;
    uint32 i;
    //u64 read;

    for (i=0; i<gf_list_count(trak->Media->information->sampleTable->SampleDescription->boxList); i++)
    {
        GF_UnknownBox *a = gf_list_get(trak->Media->information->sampleTable->SampleDescription->boxList, i);
        switch (a->type)
        {
            case GF_ISOM_BOX_TYPE_MP4S:
            //case GF_ISOM_BOX_TYPE_ENCS:
            case GF_ISOM_BOX_TYPE_MP4A:
            case GF_ISOM_BOX_TYPE_MP3:
            //case GF_ISOM_BOX_TYPE_ENCA:
            case GF_ISOM_BOX_TYPE_MP4V:
            //case GF_ISOM_BOX_TYPE_ENCV:
            case GF_ISOM_SUBTYPE_3GP_AMR:
            case GF_ISOM_SUBTYPE_3GP_AMR_WB:
            case GF_ISOM_SUBTYPE_3GP_EVRC:
            case GF_ISOM_SUBTYPE_3GP_QCELP:
            case GF_ISOM_SUBTYPE_3GP_SMV:
            case GF_ISOM_SUBTYPE_3GP_H263:
            case GF_ISOM_BOX_TYPE_AVC1:
                continue;
            default:
                break;
        }
        /*only process visual or audio*/
#if 0 //change shenh
        switch (trak->Media->handler->handlerType)
        {
            case GF_ISOM_MEDIA_VISUAL:
                /*remove entry*/
                gf_list_rem(trak->Media->information->sampleTable->SampleDescription->boxList, i);
                genv = (GF_GenericVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRV);
                bs = gf_bs_new(a->data, a->size, GF_BITSTREAM_READ);
                genv->size = a->size;
                gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *) genv, bs);
                genv->data_size = (uint32) gf_bs_available(bs);
                if (genv->data_size)
                {
                    genv->data = malloc(sizeof(char) * genv->data_size);
                    gf_bs_read_data(bs, genv->data, genv->data_size);
                }
                gf_bs_del(bs);
                genv->size = a->size;
                genv->EntryType = a->type;
                //gf_isom_box_del((GF_Box *)a);
                gf_list_insert(trak->Media->information->sampleTable->SampleDescription->boxList, genv, i);
                break;
            case GF_ISOM_MEDIA_AUDIO:
                /*remove entry*/
                gf_list_rem(trak->Media->information->sampleTable->SampleDescription->boxList, i);
                gena = (GF_GenericAudioSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRA);
                gena->size = a->size;
                bs = gf_bs_new(a->data, a->size, GF_BITSTREAM_READ);
                gf_isom_audio_sample_entry_read((GF_AudioSampleEntryBox *) gena, bs);
                gena->data_size = (uint32) gf_bs_available(bs);
                if (gena->data_size)
                {
                    gena->data = malloc(sizeof(char) * gena->data_size);
                    gf_bs_read_data(bs, gena->data, gena->data_size);
                }
                gf_bs_del(bs);
                gena->size = a->size;
                gena->EntryType = a->type;
                //gf_isom_box_del((GF_Box *)a);
                gf_list_insert(trak->Media->information->sampleTable->SampleDescription->boxList, gena, i);
                break;

            default:
                /*remove entry*/
                gf_list_rem(trak->Media->information->sampleTable->SampleDescription->boxList, i);
                genm = (GF_GenericSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRM);
                genm->size = a->size;
                bs = gf_bs_new(a->data, a->size, GF_BITSTREAM_READ);
                read = 0;
                gf_bs_read_data(bs, genm->reserved, 6);
                genm->dataReferenceIndex = gf_bs_read_u16(bs);
                genm->data_size = (uint32) gf_bs_available(bs);
                if (genm->data_size)
                {
                    genm->data = malloc(sizeof(char) * genm->data_size);
                    gf_bs_read_data(bs, genm->data, genm->data_size);
                }
                gf_bs_del(bs);
                genm->size = a->size;
                genm->EntryType = a->type;
                //gf_isom_box_del((GF_Box *)a);
                gf_list_insert(trak->Media->information->sampleTable->SampleDescription->boxList, genm, i);
                break;
        }
#endif

    }
}


GF_Err trak_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_TrackBox *ptr = (GF_TrackBox *)s;
    e = gf_isom_read_box_list(s, bs, trak_AddBox);
    if (e) return e;
    gf_isom_check_sample_desc(ptr);
    return GF_OK;
}

GF_Box *trak_New()
{
    //diag_printf("mmc_MemMalloc .... trak_New \n");
    GF_TrackBox *tmp = (GF_TrackBox *) mmc_MemMalloc(sizeof(GF_TrackBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_TrackBox));
    tmp->type = GF_ISOM_BOX_TYPE_TRAK;
    return (GF_Box *)tmp;
}

#if 0
GF_Box *gnra_New()
{
    //diag_printf("mmc_MemMalloc .... gnra_New \n");
    GF_GenericAudioSampleEntryBox *tmp = (GF_GenericAudioSampleEntryBox *) mmc_MemMalloc(sizeof(GF_GenericAudioSampleEntryBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_GenericAudioSampleEntryBox));
    tmp->type = GF_ISOM_BOX_TYPE_GNRA;
    gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*) tmp);
    return (GF_Box *)tmp;
}
#endif

#if 0
void vmhd_del(GF_Box *s)
{
    GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;
    if (ptr == NULL) return;
    //free(ptr);change shenh
}
#endif

GF_Err vmhd_Read(GF_Box *s, GF_BitStream *bs)
{
    GF_Err e;
    GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;
    e = gf_isom_full_box_read(s, bs);
    if (e) return e;
    ptr->reserved = gf_bs_read_u64(bs);
    return GF_OK;
}

GF_Box *vmhd_New()
{
//diag_printf("mmc_MemMalloc .... vmhd_New \n");
    GF_VideoMediaHeaderBox *tmp = (GF_VideoMediaHeaderBox *) mmc_MemMalloc(sizeof(GF_VideoMediaHeaderBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_VideoMediaHeaderBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->flags = 1;
    tmp->type = GF_ISOM_BOX_TYPE_VMHD;
    return (GF_Box *)tmp;
}

#if 0
GF_Err vmhd_Size(GF_Box *s)
{
    GF_Err e;
    GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 8;
    return GF_OK;
}
#endif

#if 0
GF_Box *pdin_New()
{
//diag_printf("mmc_MemMalloc .... pdin_New \n");
    GF_ProgressiveDownloadBox *tmp = (GF_ProgressiveDownloadBox*) mmc_MemMalloc(sizeof(GF_ProgressiveDownloadBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_ProgressiveDownloadBox));
    gf_isom_full_box_init((GF_Box *)tmp);
    tmp->flags = 1;
    tmp->type = GF_ISOM_BOX_TYPE_PDIN;
    return (GF_Box *)tmp;
}


void pdin_del(GF_Box *s)
{
    GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox*)s;
    if (ptr == NULL) return;
    /*change shenh
    if (ptr->rates) free(ptr->rates);
    if (ptr->times) free(ptr->times);
    free(ptr);
    */
}


GF_Err pdin_Read(GF_Box *s, GF_BitStream *bs)
{
    uint32 i;
    GF_Err e;
    GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox*)s;

    e = gf_isom_full_box_read(s, bs);
    if (e) return e;

    ptr->count = (uint32) (ptr->size) / 8;
    //diag_printf("mmc_MemMalloc .... pdin_Read \n");
    ptr->rates = mmc_MemMalloc(sizeof(uint32)*ptr->count);
    ptr->times = mmc_MemMalloc(sizeof(uint32)*ptr->count);
    if(!ptr->rates || !ptr->times)return GF_OUT_OF_MEM;
    memset(ptr->rates, 0, sizeof(uint32)*ptr->count);
    memset(ptr->times, 0, sizeof(uint32)*ptr->count);

    for (i=0; i<ptr->count; i++)
    {
        ptr->rates[i] = gf_bs_read_u32(bs);
        ptr->times[i] = gf_bs_read_u32(bs);
    }
    return GF_OK;
}


GF_Err pdin_Size(GF_Box *s)
{
    GF_Err e;
    GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox *)s;
    e = gf_isom_full_box_get_size(s);
    if (e) return e;
    ptr->size += 8*ptr->count;
    return GF_OK;
}
#endif

#if 0
GF_UserDataMap *udta_getEntry(GF_UserDataBox *ptr, uint32 boxType, bin128 UUID)
{
    uint32 i;
    GF_UserDataMap *map;
    for (i = 0; i < gf_list_count(ptr->recordList); i++)
    {
        map = (GF_UserDataMap *) gf_list_get(ptr->recordList, i);
        if (map->boxType == boxType)
        {
            if (boxType != GF_ISOM_BOX_TYPE_UUID) return map;
            if (!memcmp(map->uuid, UUID, 16)) return map;
        }
    }
    return NULL;
}

GF_Err udta_AddBox(GF_UserDataBox *ptr, GF_Box *a)
{
    GF_Err e;
    GF_UserDataMap *map;
    if (!ptr) return GF_BAD_PARAM;
    if (!a) return GF_OK;

    map = udta_getEntry(ptr, a->type, a->uuid);
    if (map == NULL)
    {
        //diag_printf("mmc_MemMalloc .... udta_AddBox \n");
        map = (GF_UserDataMap *) mmc_MemMalloc(sizeof(GF_UserDataMap));
        if (map == NULL) return GF_OUT_OF_MEM;
        memset(map, 0, sizeof(GF_UserDataMap));

        map->boxType = a->type;
        if (a->type == GF_ISOM_BOX_TYPE_UUID) memcpy(map->uuid, a->uuid, 16);
        map->boxList = gf_list_new();
        if (!map->boxList)
        {
            //free(map);change shenh
            return GF_OUT_OF_MEM;
        }
        e = gf_list_add(ptr->recordList, map);
        if (e) return e;
    }
    return gf_list_add(map->boxList, a);
}

GF_Err udta_Read(GF_Box *s, GF_BitStream *bs)//no use now,shenh
{
    GF_Err e;
    uint32 sub_type;
    GF_Box *a;
    GF_UserDataBox *ptr = (GF_UserDataBox *)s;
    while (ptr->size)
    {
        /*if no udta type coded, break*/
        sub_type = gf_bs_peek_bits(bs, 32, 0);
        if (sub_type)
        {
            e = gf_isom_parse_box(&a, bs);
            if(e==GF_INVALID_BOX)//sheen
            {
                if (ptr->size<(uint32)a) return GF_ISOM_INVALID_FILE;
                ptr->size -= (uint32)a;//recode the size.sheen
                continue;//skip this box
            }
            if (e) return e;
            e = udta_AddBox(ptr, a);
            if (e) return e;
            if (ptr->size<a->size) return GF_ISOM_INVALID_FILE;
            ptr->size -= a->size;
        }
        else
        {
            gf_bs_read_u32(bs);
            ptr->size -= 4;
        }
    }
    return GF_OK;
}

GF_Box *udta_New()
{
//diag_printf("mmc_MemMalloc .... udta_New \n");
    GF_UserDataBox *tmp = (GF_UserDataBox *) mmc_MemMalloc(sizeof(GF_UserDataBox));
    if (tmp == NULL) return NULL;
    memset(tmp, 0, sizeof(GF_UserDataBox));
    tmp->recordList = gf_list_new();
    if (!tmp->recordList)
    {
        //free(tmp);change shenh
        return NULL;
    }
    tmp->type = GF_ISOM_BOX_TYPE_UDTA;
    return (GF_Box *)tmp;
}
#endif

GF_Err defa_Read(GF_Box *s, GF_BitStream *bs)
{
    uint32 bytesToRead;
    uint32 res;
    GF_UnknownBox *ptr = (GF_UnknownBox *)s;
    if (ptr->size > 0xFFFFFFFF) return GF_ISOM_INVALID_FILE;
    bytesToRead = (uint32) (ptr->size);

    if (bytesToRead)
    {
        //diag_printf("mmc_MemMalloc .... defa_Read \n");
        ptr->data = (int8*)mmc_MemMalloc(bytesToRead);
        if (ptr->data == NULL ) return GF_OUT_OF_MEM;
        memset(ptr->data , 0, bytesToRead);
        ptr->dataSize = bytesToRead;
        if((res=gf_bs_read_data(bs, (uint8*)ptr->data, ptr->dataSize))!=ptr->dataSize)
            return GF_ISOM_INVALID_FILE;
    }
    return GF_OK;
}

//warning: we don't have any boxType, trick has to be done while creating..
GF_Box *defa_New()
{
//diag_printf("mmc_MemMalloc .... defa_New \n");
    GF_UnknownBox *tmp = (GF_UnknownBox *) mmc_MemMalloc(sizeof(GF_UnknownBox));
    if(!tmp)return 0;
    memset(tmp, 0, sizeof(GF_UnknownBox));
    return (GF_Box *) tmp;
}

#endif


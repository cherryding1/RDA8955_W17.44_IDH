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

#include "list.h"
#include "isomedia_dev.h"


/*after some tuning, this seems to be the fastest mode on WINCE*/
#ifdef _WIN32_WCE
#define GF_ARRAY_LINKED
#else
#define GF_ARRAY_ARRAY
#endif

#define GF_ARRAY_STEP_ALLOC 10

#if defined(GF_ARRAY_ARRAY)
/*
struct _tag_array
{
    void **slots;
    uint32 entryCount;
};
*/

GF_List * gf_list_new()
{
    //diag_printf("mmc_MemMalloc .... gf_list_new \n");
    GF_List *nlist = (GF_List *) mmc_MemMalloc(sizeof(GF_List));
    if (! nlist) return NULL;
    nlist->slots = NULL;
    nlist->entryCount = 0;
    return nlist;
}

void gf_list_del(GF_List *ptr)
{
    if (!ptr) return;
    /*change shenh
    free(ptr->slots);
    free(ptr);
    */
}

GF_Err gf_list_add(GF_List *ptr, void *item)
{
    if (! ptr) return GF_BAD_PARAM;

    ptr->entryCount ++;
    //ptr->slots = realloc(ptr->slots, ptr->entryCount*sizeof(void*));
    //diag_printf("mmc_MemMalloc .... gf_list_add \n");
    ptr->slots = (void**)mmc_MemMalloc(ptr->entryCount*sizeof(void*));//change shenh
    if (!ptr->slots)
    {
        ptr->entryCount = 0;
        return GF_OUT_OF_MEM;
    }
    memset(ptr->slots , 0, ptr->entryCount*sizeof(void*));
    ptr->slots[ptr->entryCount-1] = item;
    return GF_OK;
}

//add shenh
GF_Err gf_list_add2(GF_List *ptr, void *item)
{
    if (! ptr) return GF_BAD_PARAM;

    ptr->entryCount ++;
    //ptr->slots = realloc(ptr->slots, ptr->entryCount*sizeof(void*));

    if (ptr->entryCount>MAX_TOPBOX_SLOT_NUM)
    {

        return GF_NOT_SUPPORTED;
    }
    ptr->slots[ptr->entryCount-1] = item;
    return GF_OK;
}

GF_Err gf_list_add3(GF_List *ptr, void *item)
{
    if (! ptr) return GF_BAD_PARAM;

    ptr->entryCount ++;
    //ptr->slots = realloc(ptr->slots, ptr->entryCount*sizeof(void*));
    if (ptr->entryCount>MAX_TRACK_NUM)
    {

        return GF_NOT_SUPPORTED;
    }
    ptr->slots[ptr->entryCount-1] = item;
    return GF_OK;
}
//


void ChainExpand(GF_List *ptr, uint32 nbItems)
{
}

uint32 gf_list_count(GF_List *ptr)
{
    return ptr ? ptr->entryCount : 0;
}

void *gf_list_get(GF_List *ptr, uint32 itemNumber)
{
    if(!ptr || (itemNumber >= ptr->entryCount)) return NULL;
    return ptr->slots[itemNumber];
}

#if 0 //change shenh
/*WARNING: itemNumber is from 0 to entryCount - 1*/
GF_Err gf_list_rem(GF_List *ptr, uint32 itemNumber)
{
    uint32 i;
    if ( !ptr || !ptr->slots || !ptr->entryCount) return GF_BAD_PARAM;
    i = ptr->entryCount - itemNumber - 1;
    if (i) memmove(&ptr->slots[itemNumber], & ptr->slots[itemNumber +1], sizeof(void *)*i);
    ptr->slots[ptr->entryCount-1] = NULL;
    ptr->entryCount -= 1;
    ptr->slots = realloc(ptr->slots, sizeof(void*)*ptr->entryCount);
    return GF_OK;
}


/*WARNING: position is from 0 to entryCount - 1*/
GF_Err gf_list_insert(GF_List *ptr, void *item, uint32 position)
{
    uint32 i;
    if (!ptr || !item) return GF_BAD_PARAM;
    /*if last entry or first of an empty array...*/
    if (position >= ptr->entryCount) return gf_list_add(ptr, item);
    ptr->slots = realloc(ptr->slots, (ptr->entryCount+1)*sizeof(void*));
    i = ptr->entryCount - position;
    memmove(&ptr->slots[position + 1], &ptr->slots[position], sizeof(void *)*i);
    ptr->entryCount++;
    ptr->slots[position] = item;
    return GF_OK;
}
#endif

void gf_list_reset(GF_List *ptr)
{
    if (ptr)
    {
        ptr->entryCount = 0;
        /*change shenh
        free(ptr->slots);
        ptr->slots = NULL;
        */
    }
}

s32 gf_list_find(GF_List *ptr, void *item)
{
    uint32 i;
    for (i=0; i<gf_list_count(ptr); i++)
    {
        if (gf_list_get(ptr, i) == item) return (s32) i;
    }
    return -1;
}

#if 0 //change shenh
s32 gf_list_del_item(GF_List *ptr, void *item)
{
    s32 i = gf_list_find(ptr, item);
    if (i>=0) gf_list_rem(ptr, (uint32) i);
    return i;
}
#endif

#endif

#endif


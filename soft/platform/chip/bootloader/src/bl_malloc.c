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

#include "bl_malloc.h"
#include "bl_platform.h"

// A simple memory management for bootloader
//
// Each region to be managed is called as a heap. At malloc, create a
// block with one word at head, and one word at tail for management.
// The format of the word is the same:
//   [31]: 1/allocated, 0/free
//   [30:0]: word count
//
// At malloc, always malloc from tail. At free, adjacent free blocks
// will be merged.

struct BL_HEAP_T
{
    uint32_t *start;
    uint32_t *end;
    uint32_t *tail;
};

static struct BL_HEAP_T gSramHeap;
static struct BL_HEAP_T gDdrHeap;

static inline bool bl_is_allocated(uint32_t info)
{
    return (info & (1 << 31)) ? true : false;
}

static inline unsigned bl_wcount(uint32_t info)
{
    return info & 0x7fffffff;
}

static inline void bl_set_gead_tail(uint32_t *head, unsigned wcount)
{
    head[0] = head[wcount + 1] = (1 << 31) | wcount;
}

static inline void bl_unset_head_tail(uint32_t *head, unsigned wcount)
{
    head[0] = head[wcount + 1] = wcount;
}

static void bl_heap_init2(struct BL_HEAP_T *heap, uint32_t *start, unsigned wcount)
{
    heap->start = start;
    heap->end = start + wcount;
    heap->tail = start;
}

static void *bl_malloc2(struct BL_HEAP_T *heap, unsigned size)
{
    unsigned wcount = (size + 3) / 4;
    if (heap->tail + wcount + 2 > heap->end)
        return NULL;

    uint32_t *ptr = heap->tail;
    bl_set_gead_tail(ptr, wcount);
    heap->tail += wcount + 2;
    return (void *)&ptr[1];
}

static void bl_free2(struct BL_HEAP_T *heap, void *ptr)
{
    uint32_t *blk = (uint32_t *)ptr - 1;
    unsigned wcount = bl_wcount(blk[0]);

    for (;;)
    {
        uint32_t *nblk = blk + (wcount + 2);
        if (nblk == heap->tail || bl_is_allocated(nblk[0]))
            break;
        unsigned nwcount = bl_wcount(nblk[0]);
        wcount += nwcount + 2;
        bl_set_gead_tail(blk, wcount);
    }

    for (;;)
    {
        if (blk == heap->start || bl_is_allocated(blk[-1]))
            break;
        unsigned pwcount = bl_wcount(blk[-1]);
        blk -= pwcount + 2;
        wcount += pwcount + 2;
        bl_set_gead_tail(blk, wcount);
    }

    bl_unset_head_tail(blk, wcount);
    if (heap->tail == blk + wcount + 2)
        heap->tail = blk;
}

void bl_heap_init(uint32_t *sram_start, unsigned sram_wcount,
                  uint32_t *ddr_start, unsigned ddr_wcount)
{
    bl_heap_init2(&gSramHeap, sram_start, sram_wcount);
    bl_heap_init2(&gDdrHeap, ddr_start, ddr_wcount);
}

void *bl_sram_malloc(unsigned size)
{
    return bl_malloc2(&gSramHeap, size);
}

void *bl_ddr_malloc(unsigned size)
{
    return bl_malloc2(&gDdrHeap, size);
}

void bl_free(void *ptr)
{
    uint32_t *p = (uint32_t *)ptr;
    if (p >= gSramHeap.start && p < gSramHeap.end)
        bl_free2(&gSramHeap, p);
    else if (p >= gDdrHeap.start && p < gDdrHeap.end)
        bl_free2(&gDdrHeap, p);
    else
        bl_panic();
}

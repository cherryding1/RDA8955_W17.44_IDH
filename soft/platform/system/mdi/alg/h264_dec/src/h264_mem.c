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





#include "common.h"
#include "define.h"
#ifdef SHEEN_VC_DEBUG
#include <stdio.h>
#else
#include "cos.h"
#endif

/* here we can use OS dependent allocation functions */
/* for error link in RAM code, sheen
#undef malloc
#undef free
#undef realloc
*/
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

//sheen.add for self memory management.
//#define H264_MEM_RESERVE

#define H264_MEM_SIZE (1500*1024)
#define H264_MEM_SIZE2 (1300*1024) //480x320 more than 1.2M
#define H264_MEM_BLK_NUM 200
static int h264_mem_blk_idx;
static int h264_wit_fre_num;
char *h264_mem_start;
static char *h264_mem_local;
static char *h264_mem_end;
static char *h264_mem_pos;
short h264_mem_overflow;

typedef struct
{
    char *pb; //malloc address
    //mark record
    //ifdef H264_MEM_RESERVE: block state, 0=no need free, 1=need free
    //ifndef H264_MEM_RESERVE: block size.
    unsigned int mark;
} h264memblk;

static h264memblk *pH264MemBlk;

#ifndef SHEEN_VC_DEBUG
extern char * mmc_MemMalloc(int32 nsize);
#endif

int h264_memory_init()
{
    int memsize=0;
    h264_mem_blk_idx=0;
    h264_wit_fre_num=0;
    //h264_mem_start=0;
    h264_mem_local=0;
    h264_mem_end=0;
    h264_mem_pos=0;
    pH264MemBlk=0;
    h264_mem_overflow=0;

    //malloc a whole block for rm.

    //h264_mem_start=(char*)mmc_MemMalloc(H264_MEM_SIZE);
    //h264_mem_start=(char*)COS_Malloc(H264_MEM_SIZE,COS_MMI_HEAP);
    if(!h264_mem_start)
    {
#ifdef H264_MEM_RESERVE
#ifdef SHEEN_VC_DEBUG
        h264_mem_local=(char*)malloc(H264_MEM_SIZE2);
#else
        h264_mem_local=(char*)COS_Malloc(H264_MEM_SIZE2,COS_MMI_HEAP);
#endif
        memsize= H264_MEM_SIZE2;
        if(!h264_mem_local)
        {
            printf("h264_memory_init h264_mem_local fail!");
            return -1;
        }
#endif
    }
    else
    {
        h264_mem_local=h264_mem_start;
        memsize= H264_MEM_SIZE;

        if(!h264_mem_local)
        {
            printf("h264_memory_init h264_mem_local fail!");
            return -1;
        }
    }

    //h264_mem_end=h264_mem_start+H264_MEM_SIZE;
    //h264_mem_pos=h264_mem_start;
    h264_mem_end=h264_mem_local+ memsize;
    h264_mem_pos=h264_mem_local;

    //for record every block.
#ifdef SHEEN_VC_DEBUG
    pH264MemBlk=(h264memblk*)malloc(H264_MEM_BLK_NUM*sizeof(h264memblk));
#else
    //pH264MemBlk=(h264memblk*)mmc_MemMalloc(H264_MEM_BLK_NUM*sizeof(h264memblk));
    pH264MemBlk=(h264memblk*)COS_Malloc(H264_MEM_BLK_NUM*sizeof(h264memblk),COS_MMI_HEAP);
#endif

    if(!pH264MemBlk)
    {
        printf("h264_memory_init pH264MemBlk fail!");
        return -1;
    }

    memset(pH264MemBlk, 0, H264_MEM_BLK_NUM*sizeof(h264memblk));
    printf("h264_memory_init h264_mem_start=%x h264_mem_local= %x\n", h264_mem_start, h264_mem_local);
    return 0;
}

short h264_check_mem(void)
{
    return h264_mem_overflow;
}

void h264_memory_free_all()
{
    int i;
    //real free memory.
#ifdef SHEEN_VC_DEBUG
#ifdef H264_MEM_RESERVE
    free(h264_mem_local);
#else
    if(pH264MemBlk)
        for(i=0; i<H264_MEM_BLK_NUM; i++)
        {
            if(pH264MemBlk[i].pb)
            {
                free(pH264MemBlk[i].pb);
                pH264MemBlk[i].pb= 0;
                pH264MemBlk[i].mark= 0;
            }
        }
#endif
    free(pH264MemBlk);
#else
    if(h264_mem_start==0)
    {
#ifdef H264_MEM_RESERVE
        COS_Free(h264_mem_local);
#else
        if(pH264MemBlk)
            for(i=0; i<H264_MEM_BLK_NUM; i++)
            {
                if(pH264MemBlk[i].pb)
                {
                    COS_Free(pH264MemBlk[i].pb);
                    pH264MemBlk[i].pb= 0;
                    pH264MemBlk[i].mark= 0;
                }
            }
#endif
    }
    COS_Free(pH264MemBlk);
#endif

    h264_mem_blk_idx=0;
    h264_wit_fre_num=0;
    h264_mem_local=0;
    if(h264_mem_overflow==0)
        h264_mem_start=0;
    h264_mem_end=0;
    h264_mem_pos=0;
    pH264MemBlk=0;
    h264_mem_overflow=0;
    printf("h264_memory_free_all \n");
}
//sheen.end



/* you can redefine av_malloc and av_free in your project to use your
   memory allocator. You do not need to suppress this file because the
   linker will do it automatically */

void *av_malloc(unsigned int size)
{
    void *ptr;
#ifdef CONFIG_MEMALIGN_HACK
    long diff;
#endif

    /* let's disallow possible ambiguous cases */
    // if(size > ((int)INT_MAX-16) )
    //      return NULL;

#ifdef CONFIG_MEMALIGN_HACK
    ptr = malloc(size+16);
    if(!ptr)
        return ptr;
    diff= ((-(long)ptr - 1)&15) + 1;
    ptr = (char*)ptr + diff;
    ((char*)ptr)[-1]= diff;
#elif defined (HAVE_MEMALIGN)
    ptr = memalign(16,size);
    /* Why 64?
       Indeed, we should align it:
         on 4 for 386
         on 16 for 486
         on 32 for 586, PPro - k6-III
         on 64 for K7 (maybe for P3 too).
       Because L1 and L2 caches are aligned on those values.
       But I don't want to code such logic here!
     */
    /* Why 16?
       Because some CPUs need alignment, for example SSE2 on P4, & most RISC CPUs
       it will just trigger an exception and the unaligned load will be done in the
       exception handler or it will just segfault (SSE2 on P4)
       Why not larger? Because I did not see a difference in benchmarks ...
    */
    /* benchmarks with p3
       memalign(64)+1          3071,3051,3032
       memalign(64)+2          3051,3032,3041
       memalign(64)+4          2911,2896,2915
       memalign(64)+8          2545,2554,2550
       memalign(64)+16         2543,2572,2563
       memalign(64)+32         2546,2545,2571
       memalign(64)+64         2570,2533,2558

       btw, malloc seems to do 8 byte alignment by default here
    */
#else
    //ptr = malloc(size);
#ifndef H264_MEM_RESERVE
    if(h264_mem_start)
#endif
    {
        size=((size+3)>>2)<<2;//align for 4 byte. or may be assert
        if(h264_mem_pos +size >= h264_mem_end)
        {
            printf("av_malloc need more memory %d !\n", h264_mem_pos +size-h264_mem_end);
            h264_mem_overflow=1;
            return 0;
        }
        else if(h264_mem_blk_idx >= H264_MEM_BLK_NUM)
        {
            printf("av_malloc need more block !\n");
            h264_mem_overflow=1;
            return 0;
        }

        pH264MemBlk[h264_mem_blk_idx].pb=h264_mem_pos;
        pH264MemBlk[h264_mem_blk_idx].mark=0;
        h264_mem_blk_idx+=1;
        h264_mem_pos+=size;

        ptr= pH264MemBlk[h264_mem_blk_idx-1].pb;
        printf("av_mlc %x sz=%d tt=%d blk=%d\n",ptr, size, h264_mem_pos-h264_mem_local, h264_mem_blk_idx);
    }
#ifndef H264_MEM_RESERVE
    else
    {
        int i;
        for(i=0; i<H264_MEM_BLK_NUM; i++)
        {
            if(pH264MemBlk[i].pb==0)
                break;
        }
        if(i==H264_MEM_BLK_NUM)
        {
            printf("av_malloc need more block record !\n");
            h264_mem_overflow=1;
            return 0;
        }
        size=((size+3)>>2)<<2;//align for 4 byte. or may be assert
#ifdef SHEEN_VC_DEBUG
        ptr= pH264MemBlk[i].pb = malloc(size);
#else
        ptr= pH264MemBlk[i].pb = COS_Malloc(size,COS_MMI_HEAP);
#endif
        if(!ptr)
        {
            printf("av_malloc malloc fail! size %d tt %d blk %d\n",size,h264_mem_pos,h264_mem_blk_idx);
            h264_mem_overflow=1;
            return 0;
        }
        pH264MemBlk[i].mark= size;
        h264_mem_pos += size;//reuse for record the total size malloc.
        h264_mem_blk_idx++;//reuse for record the total block count.
        printf("av_mlc p=%x sz=%d tt=%d blk=%d\n",ptr, size, (int)h264_mem_pos, h264_mem_blk_idx);

    }
#endif

#endif
    return ptr;
}

void *av_realloc(void *ptr, unsigned int size)
{
#ifdef CONFIG_MEMALIGN_HACK
    int diff;
#endif
    void *p;

    /* let's disallow possible ambiguous cases */
    // if(size > ((int)INT_MAX-16) )
    //     return NULL;

#ifdef CONFIG_MEMALIGN_HACK
    //FIXME this isn't aligned correctly, though it probably isn't needed
    if(!ptr) return av_malloc(size);
    diff= ((char*)ptr)[-1];
    return (char*)realloc((char*)ptr - diff, size + diff) + diff;
#else

    //replace realloc by malloc .sheen
    //p=realloc(ptr, size);
#ifndef H264_MEM_RESERVE
    if(h264_mem_start)
#endif
    {
        //p=malloc(size);
        p=av_malloc(size);
        //copy data from ptr, also include some invalid data(for ptr buffer smaller than size normally).sheen
        if(p && ptr)
        {
            memcpy(p, ptr, size);
            //free(ptr);
            av_free(ptr);
            //printf("sheen: realloc ptr=%x\n",ptr);
        }
    }
#ifndef H264_MEM_RESERVE
    else
    {
        int i;
        for(i=0; i<H264_MEM_BLK_NUM; i++)
        {
            //ptr=0 is ok.
            if(pH264MemBlk[i].pb==ptr)
                break;
        }

        if(i==H264_MEM_BLK_NUM)
        {
            printf("av_realc fail.ptr %x not record or block record %d full!\n",ptr,h264_mem_blk_idx);
            return 0;
        }

#ifdef SHEEN_VC_DEBUG
        p=realloc(ptr,size);
#else
        p=COS_Realloc(ptr,size);
#endif
        if(p)
        {
            h264_mem_pos -= pH264MemBlk[i].mark;
            pH264MemBlk[i].pb= p;
            pH264MemBlk[i].mark= size;
            h264_mem_pos += size;
        }
    }
#endif
    //return realloc(ptr, size);
    return p;
#endif
}

void av_free(void *ptr)
{
    int i;
    /* XXX: this test should not be needed on most libcs */
    if (ptr)
#ifdef CONFIG_MEMALIGN_HACK
        free((char*)ptr - ((char*)ptr)[-1]);
#else
        //free(ptr);
#ifndef H264_MEM_RESERVE
        if(h264_mem_start)
#endif
        {
            //find the position in block sequence.
            for(i=h264_mem_blk_idx-1; i>=0; i--)
            {
                if(pH264MemBlk[i].pb==ptr)
                {
                    pH264MemBlk[i].mark=1;
                    h264_wit_fre_num+=1;
                    break;
                }
            }

            //free last one by one
            while(h264_wit_fre_num >0 )
            {
                i=h264_mem_blk_idx-1;
                if(pH264MemBlk[i].mark==1)
                {
                    h264_mem_pos=pH264MemBlk[i].pb;
                    pH264MemBlk[i].pb=0;
                    pH264MemBlk[i].mark=0;
                    h264_mem_blk_idx-=1;
                    h264_wit_fre_num-=1;
                }
                else
                    break;
            }
            printf("av_free ptr=%x wait=%d\n", ptr, h264_wit_fre_num);
        }
#ifndef H264_MEM_RESERVE
        else
        {
            for(i=0; i<H264_MEM_BLK_NUM; i++)
            {
                //ptr=0 is ok.
                if(pH264MemBlk[i].pb==ptr)
                    break;
            }

            if(i==H264_MEM_BLK_NUM)
            {
                printf("av_free ptr %x not record!\n",ptr);
                return;
            }

#ifdef SHEEN_VC_DEBUG
            free(ptr);
#else
            COS_Free(ptr);
#endif
            h264_mem_pos -= pH264MemBlk[i].mark;
            h264_mem_blk_idx--;
            printf("av_free ptr=%x sz %d blk %d tt %d\n", ptr, pH264MemBlk[i].mark,h264_mem_blk_idx,h264_mem_pos);
            pH264MemBlk[i].pb= 0;
            pH264MemBlk[i].mark= 0;
        }
#endif

#endif
}

void av_freep(void *arg)
{
    void **ptr= (void**)arg;
    av_free(*ptr);
    *ptr = NULL;
}

void *av_mallocz(unsigned int size)
{
    void *ptr = av_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

char *av_strdup(const char *s)
{
    char *ptr= NULL;
    if(s)
    {
        int len = strlen(s) + 1;
        ptr = av_malloc(len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}


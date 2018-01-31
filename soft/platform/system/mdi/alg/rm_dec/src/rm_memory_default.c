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


#include "helix_types.h"
#ifdef SHEEN_VC_DEBUG
#if !defined(_SYMBIAN)
#include <malloc.h>
#else
#include <stdlib.h>
#endif
#endif
#include "rm_memory.h"
#include "rm_memory_default.h"

//sheen
#define RM_MEM_SIZE (750*1024)
#define RM_MEM_BLK_NUM 300
static INT32 rm_mem_blk_idx;
static INT32 rm_wit_fre_num;
static INT8 *rm_mem_start;
static INT8 *rm_mem_end;
static INT8 *rm_mem_pos;

typedef struct
{
    INT8 *pb; //malloc address
    INT8 freeMrk; //block state, 0=no need free, 1=need free
} memblk;

static memblk *pMemBlk;

#ifndef SHEEN_VC_DEBUG
extern char * mmc_MemMalloc(int32 nsize);
#endif

INT32 rm_memory_default_init()
{
    rm_mem_blk_idx=0;
    rm_wit_fre_num=0;
    rm_mem_start=0;
    rm_mem_end=0;
    rm_mem_pos=0;
    pMemBlk=0;

    //malloc a whole block for rm.
#ifdef SHEEN_VC_DEBUG
    rm_mem_start=(INT8*)malloc(RM_MEM_SIZE);
#else
    rm_mem_start=(INT8*)mmc_MemMalloc(RM_MEM_SIZE);
#endif
    if(!rm_mem_start)
    {
        rm_printf("rm_memory_default_init rm_mem_start fail!");
        return -1;
    }

    rm_mem_end=rm_mem_start+RM_MEM_SIZE;
    rm_mem_pos=rm_mem_start;

    //for record every block.
#ifdef SHEEN_VC_DEBUG
    pMemBlk=(memblk*)malloc(RM_MEM_BLK_NUM*sizeof(memblk));
#else
    pMemBlk=(memblk*)mmc_MemMalloc(RM_MEM_BLK_NUM*sizeof(memblk));
#endif

    if(!pMemBlk)
    {
        rm_printf("rm_memory_default_init pMemBlk fail!");
        return -1;
    }

    memset(pMemBlk, 0, RM_MEM_BLK_NUM*sizeof(memblk));
    rm_printf("rm_memory_default_init rm_mem_start=%x size=%d", rm_mem_start,RM_MEM_SIZE);
    return 0;
}

void* rm_memory_default_malloc(void* pUserMem, UINT32 ulSize)
{
    //printf("rm_memory_default_malloc %d\n", ulSize);
    //return malloc(ulSize);
    ulSize=((ulSize+3)>>2)<<2;//align for 4 byte. or may be assert
    if(rm_mem_pos +ulSize >= rm_mem_end)
    {
        rm_printf("rm_memory_default_malloc need more memory %d !\n", rm_mem_pos +ulSize-rm_mem_end);
        return 0;
    }
    else if(rm_mem_blk_idx >= RM_MEM_BLK_NUM)
    {
        rm_printf("rm_memory_default_malloc need more block !\n");
        return 0;
    }

    pMemBlk[rm_mem_blk_idx].pb=rm_mem_pos;
    pMemBlk[rm_mem_blk_idx].freeMrk=0;
    rm_mem_blk_idx+=1;
    rm_mem_pos+=ulSize;

    //rm_printf("rm_memory_default_malloc ulSize=%d total=%d block=%d\n", ulSize, rm_mem_pos-rm_mem_start, rm_mem_blk_idx);

    return pMemBlk[rm_mem_blk_idx-1].pb;
}

void rm_memory_default_free(void* pUserMem, void* ptr)
{
    INT32 i;
    //free(ptr);
    //printf("rm_memory_default_free\n");

    //find the position in block sequence.
    for(i=rm_mem_blk_idx-1; i>=0; i--)
    {
        if(pMemBlk[i].pb==ptr)
        {
            pMemBlk[i].freeMrk=1;
            rm_wit_fre_num+=1;
            break;
        }
    }

    //free last one by one
    while(rm_wit_fre_num >0 )
    {
        i=rm_mem_blk_idx-1;
        if(pMemBlk[i].freeMrk==1)
        {
            rm_mem_pos=pMemBlk[i].pb;
            pMemBlk[i].pb=0;
            pMemBlk[i].freeMrk=0;
            rm_mem_blk_idx-=1;
            rm_wit_fre_num-=1;
        }
        else
            break;
    }
    //rm_printf("rm_memory_default_free wait=%d\n", rm_wit_fre_num);
}

void rm_memory_default_free_all()
{
    //real free memory.
#ifdef SHEEN_VC_DEBUG
    free(pMemBlk);
    free(rm_mem_start);
#else
    //do nothing
#endif

    rm_mem_blk_idx=0;
    rm_wit_fre_num=0;
    rm_mem_start=0;
    rm_mem_end=0;
    rm_mem_pos=0;
    pMemBlk=0;
    rm_printf("rm_memory_default_free_all \n");
}

//sheen. end


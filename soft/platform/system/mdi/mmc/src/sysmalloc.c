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

#include "cs_types.h"
#include "mcip_debug.h"
#include "global.h"
#include "cswtype.h"

#include "cos.h"
//char gMmcMem[CONST_MMC_MEM_SIZE];
//uint32 gPosOffset;
//uint32 gPosOffsetBack;

#define Malloc_Max 500

unsigned char *g_Malloced_p[Malloc_Max];
int32 g_MallocedCount=0;


typedef struct _Mmc_MemHead_
{
    int32 msize;
} Mmc_MemHead;
BOOL mmc_MemInit()
{
    MCI_TRACE (MCI_MALLOC_TRC,0,"**********************modify mmc malloc************************");
    g_MallocedCount=0;
    // gPosOffset = 0;
    //  gPosOffsetBack = CONST_MMC_MEM_SIZE;
    return TRUE;
}




char * mmc_MemMalloc(int32 nsize)
{
    UINT32 nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    ass(g_MallocedCount>=Malloc_Max);

    char * pmalloc=g_Malloced_p[g_MallocedCount]=COS_SHMEMMALLOC(nsize);

    //ass(pmalloc == NULL);

    if(pmalloc == NULL)
    {
        MCI_TRACE (MCI_MALLOC_TRC|TSTDOUT,0,"[MCI]************no memory to malloc!!!  ************\n");
    }
    else
    {
        COS_UpdateMemCallAdd(pmalloc,nCallerAdd);
        g_MallocedCount++;
    }

    MCI_TRACE (MCI_MALLOC_TRC,0,"mmc mlc ct:%d sz:%d addr:%x\n",g_MallocedCount,nsize,pmalloc);





    /*
      char * pmalloc;
      int32 nfactsize;
      Mmc_MemHead * p_memhead;
      uint32 * ptail;
      nfactsize = (nsize + 3)/4 * 4;
      nfactsize += sizeof(Mmc_MemHead) + 4; //head + tail
      ass(gPosOffsetBack - (uint32)gPosOffset <= nfactsize );
      p_memhead = (Mmc_MemHead *)&gMmcMem[gPosOffset];
      pmalloc = (char *)(p_memhead + 1);
      p_memhead->msize = nfactsize - 4 - sizeof(Mmc_MemHead);
      ptail = (uint32 *)(pmalloc + p_memhead->msize);
      ptail[0] = 0xcccccccc;
      gPosOffset += nfactsize;*/
    return pmalloc;
}

VOID mmc_MemCheck(BOOL bprintinfo)
{

    /*
      Mmc_MemHead * p_memhead;
      char * ptemp;
      int32 i;
      ass(gPosOffsetBack <= gPosOffset);
      if (bprintinfo)
        diag_printf("MMC CHECK MEM INFO, TOTAL SIZE: %d bytes, MALLOCED SIZE:%d bytes, MALLOCED FORM BACK SIZE:%d bytes\n",
                    CONST_MMC_MEM_SIZE, gPosOffset, CONST_MMC_MEM_SIZE - gPosOffsetBack);
      for(i = 0,ptemp = gMmcMem; ptemp < &gMmcMem[gPosOffset]; ptemp += p_memhead->msize + sizeof(Mmc_MemHead) + 4,i++)
      {
        p_memhead = (Mmc_MemHead *)ptemp;
        ass(*(uint32 *)&ptemp[p_memhead->msize + sizeof(Mmc_MemHead)]!=0xcccccccc);
        if (bprintinfo)
          diag_printf("index %d, size:%d\n",i, p_memhead->msize);
      }
      for(i = 0,ptemp = gMmcMem + gPosOffsetBack; ptemp < &gMmcMem[CONST_MMC_MEM_SIZE]; ptemp += p_memhead->msize + sizeof(Mmc_MemHead) + 4,i++)
      {
        p_memhead = (Mmc_MemHead *)ptemp;
        ass(*(uint32 *)&ptemp[p_memhead->msize + sizeof(Mmc_MemHead)]!=0xcccccccc);
        if (bprintinfo)
          diag_printf("index(back) %d, size:%d\n",i, p_memhead->msize );
      }*/

}
void mmc_MemFreeLast(char * pbuf)
{
    g_MallocedCount--;
    ass((unsigned char *)pbuf != g_Malloced_p[g_MallocedCount]);
    COS_SHMEMFREE(g_Malloced_p[g_MallocedCount]);
    MCI_TRACE (MCI_MALLOC_TRC,0,"mmc_MemFreeLast g_MallocedCount is :%d\n",g_MallocedCount);

    /*
     int * phead, *ptail;
     int noffset;
     static uint32 i;
     diag_printf("mmc_MemFreeLast called count is :%d\n",i++);
      mmc_MemCheck(0);
      phead = (int *)(pbuf - sizeof(Mmc_MemHead));
      ptail = phead + ((*phead + sizeof(Mmc_MemHead)) >> 2);
      assinfo(*ptail != 0xcccccccc, "free address is fail, or mem is bad\n");
      noffset = pbuf - gMmcMem;
      if (noffset < gPosOffset)
      {
        assinfo(ptail + 1 != (int *)(gMmcMem + gPosOffset), "free address not is last\n");
        gPosOffset -= *phead + sizeof(Mmc_MemHead) + 4;
      }
      else if (noffset > gPosOffsetBack)
      {
        assinfo(phead !=(int *)( gMmcMem + gPosOffsetBack), "free address not is last by back\n");
        gPosOffsetBack += *phead + sizeof(Mmc_MemHead) + 4;
      }
      else
        ass(1);*/
    return ;
}

void mmc_MemFreeAll()
{
    // mmc_MemCheck(TRUE);
    //gPosOffset = 0;
    MCI_TRACE (MCI_MALLOC_TRC,0,"mmc_MemFreeAll g_MallocedCount is :%d\n",g_MallocedCount);

    for(g_MallocedCount--; g_MallocedCount>=0; g_MallocedCount--)
    {
        int revalue;
        revalue=COS_SHMEMFREE(g_Malloced_p[g_MallocedCount]);
	if(revalue!=TRUE)
		MCI_TRACE (MCI_MALLOC_TRC,0,"mmc_MemFreeAll count=%d addr=%x fail!!\n",g_MallocedCount,g_Malloced_p[g_MallocedCount]);
    }

    g_MallocedCount=0;
    return;
}
int mmc_MemGetFree()
{
    return  800*1024;//gPosOffsetBack - gPosOffset;
}
char * mmc_MemMallocBack(int32 nsize)
{
    ass(g_MallocedCount>=Malloc_Max);
    MCI_TRACE (MCI_MALLOC_TRC,0,"mmc_MemMallocBack g_MallocedCount is :%d\n",g_MallocedCount);
    char * pmalloc=g_Malloced_p[g_MallocedCount]=COS_SHMEMMALLOC(nsize);
    ass(pmalloc == NULL);
    g_MallocedCount++;

    /*
      char * pmalloc;
      int32 nfactsize;
      Mmc_MemHead * p_memhead;
      uint32 * ptail;
      nfactsize = (nsize + 3)/4 * 4;
      nfactsize += sizeof(Mmc_MemHead) + 4; //head + tail
    #if 1
      if (gPosOffsetBack - gPosOffset <= nfactsize)
        return NULL;
    #endif
      //ass(gPosOffsetBack - gPosOffset <= nfactsize );
      p_memhead = (Mmc_MemHead *)&gMmcMem[gPosOffsetBack - nfactsize];
      pmalloc = (char *)(p_memhead + 1);
      p_memhead->msize = nfactsize - 4 - sizeof(Mmc_MemHead);
      ptail = (uint32 *)(pmalloc + p_memhead->msize);
      ptail[0] = 0xcccccccc;
      gPosOffsetBack -= nfactsize;*/
    return pmalloc;
}

void mmc_MemFreeAllBack()
{
    //mmc_MemCheck(TRUE);
    //gPosOffsetBack = CONST_MMC_MEM_SIZE;

}

#ifdef SMALL_BSS_RAM_SIZE

char * mmc_MemMalloc_BssRam(int32 nsize)
{
    INT8* temp_alloc = NULL;
    temp_alloc = COS_SHMEMMALLOC(nsize);
    if(temp_alloc == NULL)
    {
        asm("break 7");
        mmc_MemFreeAll();
        temp_alloc = COS_SHMEMMALLOC(nsize);
    }
    return(temp_alloc);
}
void mmc_MemFreeAll_BssRam(INT8* p)
{
    COS_SHMEMFREE(p);
}
#endif


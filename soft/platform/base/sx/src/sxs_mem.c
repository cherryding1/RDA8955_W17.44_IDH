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





#include "sxs_type.h"
#include "sxs_rmc.h"
#include "sxs_rmt.h"

#define __SXR_MEM_VAR__
#include "sxr_mem.h"
#include "sxr_mem.hp"
#undef  __SXR_MEM_VAR__
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "sxr_csp.h"

void sxr_ChkHp (u8 Idx);
void sxr_ChkUdtHp (u8 Idx);


// =============================================================================
// sxr_NewCluster
// -----------------------------------------------------------------------------
/// This function is used to create a new cluster. The cluster will be composed
/// of NbUnit of Size bytes.
/// @param  Size of units
/// @param  NbCluster number of units
// =============================================================================
void sxr_NewCluster (u16 Size, u8 NbCluster)
{
#ifdef __SXR_CLUSTER__
    u32 i;
    sxr_MemHead_t *Header;
    u16 AlignSize = ( (Size + 3 + 1 + sizeof (sxr_MemHead_t)) & ~3 );
#else
    u16 AlignSize = ( (Size + 3) & ~3 );
#endif

    u32 Status = sxr_EnterSc ();

    if (sxr_NbCluster >= SXR_NB_MAX_POOL)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Too many clusters.\n",0x06bc0001)));
    }

    u8 clusterIdx = sxr_NbCluster;
    sxr_NbCluster++;

    sxr_ExitSc (Status);

// FIXME The malloc getting memory for the cluster
// cast the size on 16 bits (We don't know why).
// Thus a cluster cannot be bigger than 64kB, taking
// headers into account.
    if ((AlignSize * NbCluster) >= (1 << 16))
    {
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(2)|TDB,TSTR("Total cluster > 64 k Size %i Nb %i.\n",0x06bc0002), Size, NbCluster));
    }

    sxr_Cluster [clusterIdx].Size         = Size;
    sxr_Cluster [clusterIdx].AlignSize    = AlignSize;
    sxr_Cluster [clusterIdx].NbCluster    = NbCluster;
    sxr_Cluster [clusterIdx].Add          = sxr_HMalloc ((u16) ( AlignSize * NbCluster));
    sxr_Cluster [clusterIdx].FirstCluster = 0;

    if  ( sxr_Cluster [clusterIdx].Add == NIL )
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Out of memory for cluster creation.\n",0x06bc0003)));
    }

    memset ( (u8 *)sxr_Cluster [clusterIdx].Add, 0, ( AlignSize * NbCluster));

#ifdef __SXR_CLUSTER__
    for ( i=0; i<NbCluster; i++ )
    {
        Header = (sxr_MemHead_t *)(&((u8 *)sxr_Cluster [clusterIdx].Add) [i*AlignSize]);

        Header -> CallerAdd = SXR_POOL_FREE;
        Header -> Size     = 0;
        Header -> Link     = 0;
        Header -> Index    = i;
        Header -> Next     = (i+1);
        Header -> PIdx     = clusterIdx;
        sxr_SetCheckSum (&Header -> CheckSum);

        ((u8 *)sxr_Cluster [clusterIdx].Add) [(i+1)*AlignSize - 1] = SXR_MEM_PATTERN;

#ifdef __SXR_DEBUG__
        memset (((u8 *)Header) + sizeof (sxr_MemHead_t), SXR_MEM_PATTERN, Size);
#endif
    }

    Header -> Next = NIL_IDX;
    sxr_SetCheckSum (&Header -> CheckSum);
#else
    _sxr_NewHeap (sxr_Cluster [clusterIdx].Add, AlignSize * NbCluster, (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER) + clusterIdx);
#endif
}


// =============================================================================
// _sxr_NewHeap
// -----------------------------------------------------------------------------
/// Create a heap memory area.
/// @param  HeapStart   first address of the heap.
/// @param  Size        in bytes.
/// @param  Idx         Heap instance index.
// =============================================================================
void _sxr_NewHeap (void *HeapStart, u32 Size, u8 Idx)
{
    if ((u32)HeapStart & 3)
    {
        HeapStart = (void *)(((u32)HeapStart + 3) & ~3);
        Size -= 4;
    }
    Size = Size & ~3;

    sxr_Heap [Idx].Top   =
        sxr_Heap [Idx].Start = (u32 *) HeapStart;

    sxr_Heap [Idx].End   = (u32 *) ((u8 *)HeapStart + Size);

    sxr_Heap [Idx].CurSize =
        sxr_Heap [Idx].LowestSize = Size;

#ifdef __SXR_DEBUG__
    memset ((u8*)HeapStart, SXR_MEM_PATTERN, Size);
#endif
}



// =============================================================================
// sxr_GetAbsoluteRemainingSize
// -----------------------------------------------------------------------------
/// Return the number of bytes remaining in the identified heap.
/// If the Heap is identified thanks to its index, the parameter Size is useless.
/// When Idx == SXR_CLUSTER_LIKE_MEM the function will return the size remaining
/// in the "cluster heap" targeting the providing size.
/// @param Size allows to identify the target "cluster heap"
/// @param Idx  heap index or SXR_CLUSTER_LIKE_MEM.
/// @return     number of bytes available in the identified Heap.
// =============================================================================
u32 sxr_GetAbsoluteRemainingSize (u32 Size, u8 Idx)
{
    if (Idx == SXR_CLUSTER_LIKE_MEM)
    {
        u32 ClusterIdx;

        if (sxr_NbCluster == 0)
        {
            SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No cluster exist\n",0x06bc0004)));
        }

        if (sxr_Cluster [sxr_NbCluster-1].Size <= Size)
        {
            SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("Required size too big.%i\n",0x06bc0005), Size));
        }

        for (ClusterIdx=0; ClusterIdx<sxr_NbCluster; ClusterIdx++)
        {
            if ( sxr_Cluster [ClusterIdx].Size >= Size )
            {
                break;
            }
        }

#ifdef __SXR_CLUSTER__
        return (sxr_Cluster [ClusterIdx].NbCluster - sxr_Cluster [ClusterIdx].NbAllocation) * sxr_Cluster [ClusterIdx].Size;
#else
        Idx = PAL_NB_RAM_AREA  + SXR_NB_HEAP_USER + ClusterIdx;
#endif
    }
    return sxr_Heap [Idx].CurSize;
}


// =============================================================================
// sxr_GetRelativeRemainingSize
// -----------------------------------------------------------------------------
/// Return the ratio of bytes remaining in the identified memory the total
/// number of bytes dedicated to the heap.
/// If the Heap is identified thanks to its index, the parameter Size is useless.
/// When Idx == SXR_CLUSTER_LIKE_MEM the function will return the size remaining
/// in the "cluster heap" targeting the providing size.
/// @param Size allows to identify the target "cluster heap"
/// @param Idx  heap index or SXR_CLUSTER_LIKE_MEM.
/// @return     percentage of bytes available in the identified Heap.
// =============================================================================
u32 sxr_GetRelativeRemainingSize (u32 Size, u8 Idx)
{
    if (Idx == SXR_CLUSTER_LIKE_MEM)
    {
        u32 ClusterIdx;

        if (sxr_NbCluster == 0)
        {
            SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No cluster exist\n",0x06bc0006)));
        }

        if (sxr_Cluster [sxr_NbCluster-1].Size <= Size)
        {
            SXS_RAISE ((_SXR|TABORT|TNB_ARG(1)|TDB,TSTR("Required size too big.%i\n",0x06bc0007), Size));
        }

        for (ClusterIdx=0; ClusterIdx<sxr_NbCluster; ClusterIdx++)
        {
            if ( sxr_Cluster [ClusterIdx].Size >= Size )
            {
                break;
            }
        }

#ifdef __SXR_CLUSTER__
        return (sxr_Cluster [ClusterIdx].NbAllocation * 100) / sxr_Cluster [ClusterIdx].NbCluster;
#else
        Idx = PAL_NB_RAM_AREA + SXR_NB_HEAP_USER + ClusterIdx;
#endif
    }

    return (sxr_Heap [Idx].CurSize * 100) / ((u32)sxr_Heap [Idx].End - (u32)sxr_Heap [Idx].Start);
}



// =============================================================================
// sxr_GetHeapEnd
// -----------------------------------------------------------------------------
/// Return Heap end address.
/// @param  Idx Heap index.
/// @return Address on heap end.
// =============================================================================
void *sxr_GetHeapEnd (u8 Idx)
{

    return sxr_Heap [Idx].End;
}


// =============================================================================
// sxr_CheckCluster
// -----------------------------------------------------------------------------
/// Check clusters load and consistency.
// =============================================================================
void sxr_CheckCluster (void)
{
    u32 i,j;
#ifdef __SXR_CLUSTER__
    sxr_MemHead_t *Header;
#endif

    SXS_FPRINTF((_SXR|TSTDOUT|TDB|TNB_ARG(1),TSTR("NbCluster %d\n",0x06bc0008), sxr_NbCluster));

    for (i=0; i<sxr_NbCluster; i++)
    {
        SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TNB_ARG(4)|TDB,TSTR("Cluster %i: %i * %i bytes. %i allocated blocks.\n",0x06bc0009),
                      i,
                      sxr_Cluster [i].NbCluster,
                      sxr_Cluster [i].AlignSize,
                      sxr_Cluster [i].NbAllocation));
//                sxr_GetAbsoluteHeapRemainingSize (0, PAL_NB_RAM_AREA + SXR_NB_HEAP_USER + i)));

#ifdef __SXR_CLUSTER__
        for (j=0; j<sxr_Cluster [i].NbCluster; j++)
        {
            Header = (sxr_MemHead_t *)(&((u8 *)sxr_Cluster [i].Add) [j * sxr_Cluster [i].AlignSize]);

            if (!sxr_CheckSum (&Header -> CheckSum))
            {
                SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("Cluster Header consistency failure %lx\n",0x06bc000a), Header + 1));
            }

            if ((((u8 *)sxr_Cluster [Header -> PIdx].Add) [((Header -> Index + 1) * sxr_Cluster [Header -> PIdx].AlignSize) -1]) != SXR_MEM_PATTERN)
            {
                SXS_RAISE ((_SXR|TDB|TNB_ARG(2),TSTR("Cluster Footer erased %lx, %lxy\n",0x06bc000b), Header + 1, Header -> CallerAdd));
            }

            if (Header -> CallerAdd != SXR_POOL_FREE)
            {
                SXS_FPRINTF ((_SXR|TSTDOUT|TIDU|TDB|TNB_ARG(3),TSTR("(0x%08lx , %5i) -> %lxy\n",0x06bc000c), Header + 1, Header -> Size, Header -> CallerAdd));
            }
        }
#else
        sxr_ChkHp (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER + i);
#endif

        for (j=0; j<SXR_LOAD_HIST_GRANULARITY; j++)
        {
            if (sxr_Cluster [i].LoadHist [j] != 0)
            {
                SXS_TRACE (_SXR|TSTDOUT|TNB_ARG(2)|TDB,TSTR("Load < %i/100: %i\n",0x06bc000d), ((j+1)*100)/SXR_LOAD_HIST_GRANULARITY, sxr_Cluster [i].LoadHist [j]);
            }
        }

        SXS_TRACE (_SXR|TSTDOUT|TDB,TSTR("\n",0x06bc000e));
    }
}



// =============================================================================
// sxr_CheckClusterRmt
// -----------------------------------------------------------------------------
/// Check clusters load and consistency and send Info to Remote.
// =============================================================================
void sxr_CheckClusterRmt (void)
{
    u8 i, j, count;
    sxr_ClusterCheck_t *Data;
    sxr_MemHead_t *Header;

    for (i=0; i<sxr_NbCluster; i++)
    {
        for (j=0,count=0; j<sxr_Cluster [i].NbCluster; j++)
        {
            Header = (sxr_MemHead_t *)(&((u8 *)sxr_Cluster [i].Add) [j * sxr_Cluster [i].AlignSize]);

            if (Header -> CallerAdd != SXR_POOL_FREE)
            {
                count++;
            }
        }

        if ((Data = (sxr_ClusterCheck_t *)sxs_SendToRemote (SXS_MEM_RMC, NIL, sizeof(sxr_ClusterCheck_t))) != NIL )
        {
            Data->ClusterNb = i;
            Data->PoolNb = sxr_NbCluster;
            Data->MaxBlockNb = sxr_Cluster [i].NbCluster;
            Data->ClusterSize = sxr_Cluster [i].AlignSize;
            Data->NbBlockAllocated = count;
        }
    }
}


// =============================================================================
// sxr_CheckUpdateCluster
// -----------------------------------------------------------------------------
/// Check clusters load and consistency in debug mode.
/// @param  Ptr
/// @param  ClusterIdx
// =============================================================================
#if defined (__SXR_DEBUG__) || defined (__SXR_MEM_CHECK__)
void sxr_CheckUpdateCluster (u8 *Ptr, u8 ClusterIdx)
{
    u32 i;
    sxr_MemHead_t *Header = (sxr_MemHead_t *)(Ptr - sizeof (sxr_MemHead_t));
    sxr_HMemHead_t *HBlock = ((sxr_HMemHead_t *)Ptr) - 1;
    u32 Size = (HBlock -> Size << 2) - HBlock -> H.DSize;

#ifdef __SXR_DEBUG__
    if (Ptr != NIL)
    {
        for (i=0; i<(u32)(sxr_Cluster [ClusterIdx].Size - Size); i++)
        {
            if (((u8 *)Ptr) [Size + i] != SXR_MEM_PATTERN)
            {
                SXS_RAISE ((_SXR|TDB|TNB_ARG(3),TSTR("Cluster memory deleted %lx offset %i (%lx)\n",0x06bc000f), (Ptr, Size + i, (u8 *)Ptr)[Size + i]));
                break;
            }
        }

        if (Header -> Link == 0)
            memset ((u8 *)Ptr, SXR_MEM_PATTERN, sxr_Cluster [ClusterIdx].Size);
    }
#endif

    for (i=0; i<sxr_NbCluster; i++)
    {
#ifdef __SXR_CLUSTER__
        u32 j;

        for (j=0; j<sxr_Cluster [i].NbCluster; j++)
        {
            Header = (sxr_MemHead_t *)(&((u8 *)sxr_Cluster [i].Add) [j * sxr_Cluster [i].AlignSize]);
            Ptr    = ((u8 *)Header) + sizeof (sxr_MemHead_t);

            if (!sxr_CheckSum (&Header -> CheckSum))
            {
                SXS_RAISE ((_SXR|TSMAP(6)|TDB|TNB_ARG(3),TSTR("Cluster Header consistency failure %lx %s %s\n",0x06bc0010), Header + 1, sxr_GetTaskName (), sxr_GetJobName ()));
            }

            if ((((u8 *)sxr_Cluster [Header -> PIdx].Add) [((Header -> Index + 1) * sxr_Cluster [Header -> PIdx].AlignSize) -1]) != SXR_MEM_PATTERN)
            {
                SXS_RAISE ((_SXR | TSMAP(0x0C)|TDB|TNB_ARG(4),TSTR("Cluster Footer erased %lx, %lxy %s %s\n",0x06bc0011), Header + 1, Header -> CallerAdd, sxr_GetTaskName (), sxr_GetJobName ()));
            }

#ifdef __SXR_DEBUG__
            if (Header -> CallerAdd != SXR_POOL_FREE)
            {
                u32 k;

                for (k=0; k<(u32)(sxr_Cluster [i].Size - Header -> Size); k++)
                {
                    if (((u8 *)Ptr) [Header -> Size + k] != SXR_MEM_PATTERN)
                    {
                        SXS_RAISE ((_SXR|TDB|TNB_ARG(3),TSTR("Cluster memory deletion %lx offset 0x%x  (%lx)\n",0x06bc0012), Ptr, Header -> Size + k, ((u8 *)Ptr)[Header -> Size + k]));
                        break;
                    }
                }
            }
            else
            {
                u32 k;

                for (k=0; k<(sxr_Cluster [i].Size); k++)
                {
                    if (((u8 *)Ptr) [k] != (SXR_MEM_PATTERN))
                    {
                        SXS_RAISE ((_SXR|TDB|TNB_ARG(3),TSTR("Cluster memory deletion %lx offset 0x%x (%lx)\n",0x06bc0013), Ptr,  k, ((u8 *)Ptr)[k]));
                        break;
                    }
                }
            }
#endif
        }
#else
        sxr_ChkUdtHp (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER + i);
#endif
    }
}
#endif


// =============================================================================
// sxr_ChkHp
// -----------------------------------------------------------------------------
/// Check Heaps load and consistency.
/// @param  Idx Heap index.
// =============================================================================
void sxr_ChkHp (u8 Idx)
{
    sxr_HMemHead_t *HBlock = (sxr_HMemHead_t *)sxr_Heap [Idx].Start;
    u8 POSSIBLY_UNUSED Cnt = 0;

    SXS_FPRINTF ((_SXR|TSTDOUT|TDB|TNB_ARG(3),TSTR("Heap size Total %i Cur %i Min %i\n",0x06bc0014), (u32)sxr_Heap [Idx].End - (u32)sxr_Heap [Idx].Start, sxr_Heap [Idx].CurSize, sxr_Heap [Idx].LowestSize));

    while (HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].Top)
    {
        if (HBlock != ((sxr_HMemHead_t *) ((u32 *)HBlock) [HBlock -> Size - 1]))
        {
            SXS_RAISE ((_SXR|TDB|TNB_ARG(3),TSTR("Heap block consistency failure %lx Size %i Heap End %lx\n",0x06bc0015), HBlock, HBlock -> Size, sxr_Heap [Idx].End));
        }

        if (HBlock -> H.Index == SXR_HEAP_ALLOCATED)
        {
            SXS_FPRINTF ((_SXR|TSTDOUT|TDB|TNB_ARG(4),TSTR("(%08lx, %5i) -> %lxy (%i)\n",0x06bc0016), HBlock+1, (HBlock -> Size << 2) - HBlock -> H.DSize, HBlock -> H.CallerAdd, Cnt++));

            if (HBlock == (sxr_HMemHead_t *)sxr_Heap [Idx].Top)
            {
                break;
            }

            if (!sxr_CheckSum (&HBlock -> H.CheckSum))
            {
                SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("Heap Header consistency failure %lx\n",0x06bc0017), HBlock + 1));
            }

            if (((u8 *)HBlock)[sizeof (sxr_HMemHead_t) + (HBlock -> Size << 2) - HBlock -> H.DSize] != SXR_MEM_PATTERN)
            {
                SXS_RAISE ((_SXR|TDB|TNB_ARG(2),TSTR("Heap Footer erased %lx, %lxy\n",0x06bc0018), HBlock, HBlock -> H.CallerAdd));
            }
        }

        if (HBlock -> Size  > 0xFFFFF)
            SXS_RAISE ((_SXR|TDB|TNB_ARG(2),TSTR("Heap destroyed. %lx %lx\n",0x06bc0019), HBlock, (HBlock -> Size << 2) - HBlock -> H.DSize));

        {
            u32 *Zob = (u32 *)HBlock;
            Zob += HBlock -> Size;
            HBlock = (sxr_HMemHead_t *)Zob;
        }
    }

    if (HBlock != (sxr_HMemHead_t *)sxr_Heap [Idx].Top)
    {
        SXS_RAISE ((_SXR|TDB|TNB_ARG(2),TSTR("Heap unconsistency %lx / %lx\n",0x06bc001a), HBlock, sxr_Heap [Idx].End));
    }
}




// =============================================================================
// sxr_CheckHeap
// -----------------------------------------------------------------------------
/// Check Heaps load and consistency.
// =============================================================================
void sxr_CheckHeap (void)
{
    u8 Idx;

    for (Idx = 0; Idx <PAL_NB_RAM_AREA + SXR_NB_HEAP_USER; Idx++)
    {
        sxr_ChkHp (Idx);
    }
}



// =============================================================================
// sxr_CheckUpdateHeap
// -----------------------------------------------------------------------------
/// Check heaps load and consistency in debug mode.
/// @param  Idx Heap index.
// =============================================================================
#if defined (__SXR_DEBUG__) || defined (__SXR_MEM_CHECK__)
void sxr_ChkUdtHp (u8 Idx)
{
    sxr_HMemHead_t *HBlock = (sxr_HMemHead_t *)sxr_Heap [Idx].Start;
    u32 **PHBlock = (u32 **)&HBlock;
    u8  Cnt = 0;

    while (HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].Top)
    {
        if (HBlock != ((sxr_HMemHead_t *) ((u32 *)HBlock) [HBlock -> Size - 1]))
        {
            SXS_RAISE ((_SXR|TSMAP(0x18)|TDB|TNB_ARG(5),TSTR("Heap block consistency failure %lx Size %i Heap End %lx %s %s\n",0x06bc001b), HBlock, HBlock -> Size, sxr_Heap [Idx].End, sxr_GetTaskName (), sxr_GetJobName ()));
        }

        if (HBlock -> H.Index == SXR_HEAP_ALLOCATED)
        {
            if (!sxr_CheckSum (&HBlock -> H.CheckSum))
            {
                SXS_RAISE ((_SXR|TSMAP(6)|TDB|TNB_ARG(3),TSTR("Heap Header consistency failure %lx %s %s\n",0x06bc001c), HBlock + 1, sxr_GetTaskName (), sxr_GetJobName ()));
            }

            if (((u8 *)HBlock)[sizeof (sxr_HMemHead_t) + (HBlock -> Size << 2) - HBlock -> H.DSize] != SXR_MEM_PATTERN)
            {
                SXS_RAISE ((_SXR|TSMAP(0x0C)|TDB|TNB_ARG(4),TSTR("Heap Footer erased %lx, %lxy %s %s\n",0x06bc001d), HBlock, HBlock -> H.CallerAdd, sxr_GetTaskName (), sxr_GetJobName ()));
            }
        }
#ifdef __SXR_DEBUG__
        else
        {
            u32 i;
            for (i= sizeof (sxr_HMemHead_t) / 4; i< HBlock -> Size - 1; i++)
            {
                if (((u32 *)HBlock) [i] != SXR_DW_MEM_PATTERN)
                {
                    SXS_RAISE ((_SXR|TDB|TNB_ARG(2),TSTR("Heap memory deletion %lx (%lx)\n",0x06bc001e), &HBlock[i+1], HBlock[i+1]));
                }
            }
        }
#endif

        if (HBlock -> Size  > 0xFFFFF)
        {
            SXS_RAISE ((_SXR|TSMAP(0x0A)|TDB|TNB_ARG(4),TSTR("Heap destroyed. %lx %lx %s %s\n",0x06bc001f), HBlock, HBlock -> Size, sxr_GetTaskName (), sxr_GetJobName ()));
        }

        *PHBlock += HBlock -> Size;
    }

    if (HBlock != (sxr_HMemHead_t *)sxr_Heap [Idx].Top)
    {
        SXS_RAISE ((_SXR|TSMAP(0x0A)|TDB|TNB_ARG(4),TSTR("Heap unconsistency %lx / %lx %s %s\n",0x06bc0020), HBlock, sxr_Heap [Idx].End, sxr_GetTaskName (), sxr_GetJobName ()));
    }

    HBlock++;

#ifdef __SXR_DEBUG__
    while (HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].End)
    {
        if (*((u32 *)HBlock) != SXR_DW_MEM_PATTERN)
        {
            SXS_RAISE ((_SXR|TDB|TNB_ARG(3),TSTR("Heap memory deletion %lx (%lx), top %lx\n",0x06bc0021), HBlock, HBlock [0], sxr_Heap [Idx].Top));
        }

        (*PHBlock)++;
    }
#endif
}
#endif



// =============================================================================
// sxr_CheckUpdateHeap
// -----------------------------------------------------------------------------
/// Check heaps load and consistency in debug mode.
/// @param  Block
/// @param  Idx Heap index.
// =============================================================================
#if defined (__SXR_DEBUG__) || defined (__SXR_MEM_CHECK__)
void sxr_CheckUpdateHeap (void *Block, u8 Idx)
{
#ifdef __SXR_DEBUG__
    if (Block != NIL)
    {
        if (Block < (void *)sxr_Heap [Idx].Top)
        {
            memset (((u8 *)Block) + sizeof (sxr_HMemHead_t), SXR_MEM_PATTERN, (4 * ((sxr_HMemHead_t *)Block) -> Size) - 4 - sizeof (sxr_HMemHead_t));
        }
        else
        {
            memset (((u8 *)Block), SXR_MEM_PATTERN, 4 * ((sxr_HMemHead_t *)Block) -> Size);
        }
    }
#endif

    for (Idx=0; Idx<PAL_NB_RAM_AREA + SXR_NB_HEAP_USER; Idx++)
    {
        sxr_ChkUdtHp (Idx);
    }
}
#endif


#ifdef __SXR_MEM_TRACKER__

// =============================================================================
// sxr_MemTrackAlloc
// -----------------------------------------------------------------------------
/// Get track of the memory allocation.
/// @param  Header  Pointer on sxr_HMemHead_t
// =============================================================================
void sxr_MemTrackAlloc (sxr_HMemHead_t *Header)
{
    sxr_MemTracker.MemHeld [sxr_MemTracker.HeldIdx & (SXR_MEM_TRACK_DEEP - 1)].Add = (u32 *)(Header + 1);
    sxr_MemTracker.MemHeld [sxr_MemTracker.HeldIdx & (SXR_MEM_TRACK_DEEP - 1)].CallerAdd = Header -> H.CallerAdd;
    sxr_MemTracker.MemHeld [sxr_MemTracker.HeldIdx & (SXR_MEM_TRACK_DEEP - 1)].Stamp = sxr_MemTracker.Stamp++;
    sxr_MemTracker.HeldIdx++;

}


// =============================================================================
// sxr_MemTrackFree
// -----------------------------------------------------------------------------
/// Get track of the memory Free.
/// @param  Header  Pointer on sxr_HMemHead_t
// =============================================================================
void sxr_MemTrackFree (sxr_HMemHead_t *Header)
{
    sxr_MemTracker.MemFree [sxr_MemTracker.FreeIdx & (SXR_MEM_TRACK_DEEP - 1)].Add = (u32 *)(Header + 1);
    sxr_MemTracker.MemFree [sxr_MemTracker.FreeIdx & (SXR_MEM_TRACK_DEEP - 1)].CallerAdd = Header -> H.CallerAdd;
    sxr_MemTracker.MemFree [sxr_MemTracker.FreeIdx & (SXR_MEM_TRACK_DEEP - 1)].Stamp = sxr_MemTracker.Stamp++;
    sxr_MemTracker.FreeIdx++;
}



// =============================================================================
// sxr_CheckMemTracker
// -----------------------------------------------------------------------------
/// Print the last memory allocation activity.
// =============================================================================
void sxr_CheckMemTracker (void)
{
#ifdef __SXR_MEM_TRACKER__
    u32 i, j;

    for (i=1; i<=SXR_MEM_TRACK_DEEP; i++)
    {
        j = (sxr_MemTracker.HeldIdx - i) & (SXR_MEM_TRACK_DEEP - 1);

        if (sxr_MemTracker.MemHeld [j].Add != 0)
        {
            SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(3),TSTR("H %08i Add 0x%lx (%lxy)\n",0x06bc0022), sxr_MemTracker.MemHeld [j].Stamp, sxr_MemTracker.MemHeld [j].Add, sxr_MemTracker.MemHeld [j].CallerAdd);
        }
    }

    for (i=1; i<=SXR_MEM_TRACK_DEEP; i++)
    {
        j = (sxr_MemTracker.FreeIdx - i) & (SXR_MEM_TRACK_DEEP - 1);

        if (sxr_MemTracker.MemFree [j].Add != 0)
        {
            SXS_TRACE (_SXR|TSTDOUT|TDB|TNB_ARG(3),TSTR("F %08i Add 0x%lx (%lxy)\n",0x06bc0023), sxr_MemTracker.MemFree [j].Stamp, sxr_MemTracker.MemFree [j].Add, sxr_MemTracker.MemFree [j].CallerAdd);
        }
    }
#endif
}
#endif



// =============================================================================
// sxr_GetClusterInfo
// -----------------------------------------------------------------------------
/// Provide information about the specified cluster.
/// @param  ClusterIdx  Cluster index
/// @param  ClusterInfo Pointer to sxr_ClusterInfo_t
// =============================================================================
void sxr_GetClusterInfo (u8 ClusterIdx, sxr_ClusterInfo_t *ClusterInfo)
{
    ClusterInfo -> Size         = sxr_Cluster [ClusterIdx].Size;
    ClusterInfo -> NbCluster    = sxr_Cluster [ClusterIdx].NbCluster;
    ClusterInfo -> NbAllocation = sxr_Cluster [ClusterIdx].NbAllocation;
}



// =============================================================================
// sxr_MemoryAlertDispatchRegistration
// -----------------------------------------------------------------------------
/// Register a call back for the memory level notification.
/// @param  CallBack
// =============================================================================
void sxr_MemoryAlertDispatchRegistration (void (*CallBack)(u32))
{
    sxr_MemoryAlert.CallBack = CallBack;
}


// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================


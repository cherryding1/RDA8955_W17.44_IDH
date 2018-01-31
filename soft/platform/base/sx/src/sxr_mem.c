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





#include "sxr_ops.h"
#include "sxr_mem.hp"
#include "sxs_type.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "sxs_rmt.h"
#include "sxr_cnf.h"
#include "sxp_debug.h"


// =============================================================================
// _sxr_UpMemCallerAdd
// -----------------------------------------------------------------------------
/// Update the memory \p CallerAdd field, to be used by custom allocation
/// function to actually record the user function and not the custom allocation
/// function.
/// @param  Ptr     pointer to allocated memory
/// @param  callAdd new value of \p CallerAdd field.
// =============================================================================
void  _sxr_UpMemCallerAdd(void *Ptr,  u32 callAdd)
{
    u32  Status;
    sxr_HMemHead_t *HBlock;

    if(Ptr == NULL)
    {
        return;
    }

    HBlock = ((sxr_HMemHead_t *)Ptr) - 1;
    Status = sxr_EnterSc();
    HBlock->H.CallerAdd = callAdd;
    sxr_ExitSc (Status);
}


// =============================================================================
// sxr_HMalloc
// -----------------------------------------------------------------------------
/// Heap memory allocation.
/// @param  Size    Requered memory size in bytes.
/// @param  Idx     Heap instance index.
/// @return Pointer onto free memory of required size.
// =============================================================================
void *_sxr_HMalloc (u32 Size, u8 Idx)
{
    u32  Status;
    u32  CallerAdd;
    u32  AlignSize = Size;
    sxr_HMemHead_t *HBlock = NIL;
    u8   ClusterFlag = FALSE;

    SXS_GET_RA(CallerAdd);

    SX_PROFILE_FUNCTION_ENTER(_sxr_HMalloc);

#ifndef __TARGET__
    sxr_NbUsedBlk++;
#endif

#ifndef __SXR_CLUSTER__
    if (Idx == SXR_CLUSTER_LIKE_MEM)
    {
        u32 ClusterIdx;

        if (sxr_NbCluster == 0)
        {
            SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No cluster exist\n",0x06a70001)));
        }

        if (sxr_Cluster [sxr_NbCluster-1].Size <= Size)
        {
            Idx = 0;
        }
        else
        {
            for (ClusterIdx=0; ClusterIdx<sxr_NbCluster; ClusterIdx++)
            {
                if (sxr_Cluster [ClusterIdx].Size >= Size)
                {
                    break;
                }
            }

            {
                u16 PrevSize = ClusterIdx == 0 ? 0 : sxr_Cluster [ClusterIdx - 1].Size;
                sxr_Cluster [ClusterIdx].LoadHist [((Size - PrevSize) * (SXR_LOAD_HIST_GRANULARITY-1)) / (sxr_Cluster [ClusterIdx].Size - PrevSize)]++;
            }

            Idx = PAL_NB_RAM_AREA + SXR_NB_HEAP_USER + ClusterIdx;

            sxr_Cluster [ClusterIdx].NbAllocation++;

            ClusterFlag = TRUE;
        }
    }
#endif

    // Heap block layout
    // -----------------
    // HeapMemHead (sizeof sxr_HMemHead_t) +
    // ReqSize (Size) +
    // MemPattern (1, '0xFD') +
    // Alignment (1 to 3) +
    // TailPtr (4, pointing to block header)
    // -----------------
    //AlignSize = (AlignSize + 1 + (4 - (AlignSize & 3)) + sizeof (sxr_HMemHead_t) + 4) >> 2;
    // add 3 instead of (4 - (AlignSize & 3) does the same as >> 2 will discard lower bits anyway
    AlignSize = (AlignSize + 1 + 3 + sizeof (sxr_HMemHead_t) + 4) >> 2;

    Status = sxr_EnterSc();

    while (TRUE)
    {
        if ((sxr_Heap [Idx].Top + AlignSize) > sxr_Heap [Idx].End)
        {
            HBlock = (sxr_HMemHead_t *)sxr_Heap [Idx].Start;

            while ((((HBlock -> H.Index != SXR_HEAP_ALLOCATED) && (HBlock -> Size < AlignSize))
                    ||     (HBlock -> H.Index == SXR_HEAP_ALLOCATED))
                    &&     (HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].Top))
            {
                HBlock = (sxr_HMemHead_t *)(((u32 *)HBlock) + HBlock -> Size);
            }

            if (HBlock >= (sxr_HMemHead_t *)sxr_Heap [Idx].Top)
            {

#ifndef __SXR_CLUSTER__
                if (ClusterFlag == TRUE)
                {
                    if ((sxr_MemoryAlert.ExtraAllocationCnt == 0)
                            &&  (sxr_MemoryAlert.CallBack != NIL))
                    {
                        sxr_MemoryAlert.ExtraAllocationCnt = Idx;
                        sxr_MemoryAlert.CallBack (SXR_MEMORY_LEVEL_LOW);
                    }

                    if (Idx >= (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER))
                    {
                        // No room found in a cluster of that size.
                        // We try the heap.
#ifdef __IRAT__
                        //IRAT, DUALSIM is in debuging progress, so crash here , to check our memory related issues.
                        SXS_FPRINTF((_SXR|TSTDOUT|TABORT|TDB|TNB_ARG(1),
                                     TSTR("Out of memory in cluster %i, trying to allocate on the heap\n",0x06a70020),
                                     Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)));

#else
                        SXS_FPRINTF((_SXR|TSTDOUT|TDB|TNB_ARG(1),
                                     TSTR("Out of memory in cluster %i, trying to allocate on the heap\n",0x06a70020),
                                     Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)));
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810) //no space to do opt for 8810
                        Idx = 0;
#else
                        // Pass through the while(TRUE) loop again
                        // with Idx pointing to the heap.
                        sxr_Cluster [Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)].NbAllocation--;
                        Idx++;

                        if(Idx >= PAL_NB_RAM_AREA + SXR_NB_HEAP_USER + sxr_NbCluster)
                        {
                            Idx = 0;
                        }
                        else
                        {
                            sxr_Cluster [Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)].NbAllocation++;
                        }
#endif
                    }
                    else
                    {
                        sxr_CheckCluster ();
                        if (Idx == 0)
                        {
                            // No room was found on the heap.
                            SXS_RAISE ((_SXR|TSTDOUT|TABORT|TDB,
                                        TSTR("Out of memory on the heap\n",0x06a70021),
                                        Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)));
                        }
                        else
                        {
                            // No room found in a cluster of that heap
                            SXS_RAISE ((_SXR|TSTDOUT|TABORT|TDB|TNB_ARG(1),
                                        TSTR("Out of memory in cluster %i\n",0x06a70002),
                                        Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)));
                        }
                    }
                }
                else
#endif
                {
                    sxr_ExitSc (Status);
                    SX_PROFILE_FUNCTION_EXIT(_sxr_HMalloc);
                    return NIL;
                }
            }
            else
            {
                // make sure we don't cut block into too small pieces.
                if ((HBlock -> Size - AlignSize) <= (sizeof (sxr_HMemHead_t) + 4))
                {
                    AlignSize = HBlock -> Size;
                }
                else
                {
                    //sxr_HMemHead_t *NextBlock = ((sxr_HMemHead_t *)((u32 *)HBlock) + HBlock -> Size);
                    sxr_HMemHead_t *NextBlock = (sxr_HMemHead_t *)(((u32 *)HBlock) + AlignSize);
                    NextBlock -> H.Index = SXR_HEAP_NOT_ALLOCATED;

                    NextBlock -> Size = HBlock -> Size - AlignSize;
                    ((u32 *)NextBlock) [NextBlock -> Size - 1] = (u32)NextBlock;
                }

                break;
            }
        }
        else
        {
            HBlock = (sxr_HMemHead_t *)sxr_Heap [Idx].Top;
            sxr_Heap [Idx].Top += AlignSize;
            break;
        }
    }


    sxr_Heap [Idx].CurSize -= (AlignSize << 2);

    if (sxr_Heap [Idx].CurSize < sxr_Heap [Idx].LowestSize)
    {
        sxr_Heap [Idx].LowestSize = sxr_Heap [Idx].CurSize;
    }

    if (HBlock != NIL)
    {
        HBlock -> H.CallerAdd = CallerAdd;
        HBlock -> H.Link      = 1;
        HBlock -> H.PIdx      = ((ClusterFlag == TRUE) && (Idx == 0)) ?
                                Idx | SXR_HEAP_MEMORY | SXR_POOL_OVERFLOW:
                                Idx | SXR_HEAP_MEMORY;
        //HBlock -> H.Size      = Size;
        HBlock -> H.DSize     = (AlignSize << 2) - Size;
        HBlock -> Size        = AlignSize;
        HBlock -> H.Index     = SXR_HEAP_ALLOCATED;

        ((u32 *)HBlock) [HBlock -> Size - 1] = (u32)HBlock;

        sxr_ExitSc(Status);

        sxr_SetCheckSum (&HBlock -> H.CheckSum);


        ((u8 *)HBlock)[sizeof (sxr_HMemHead_t) + Size] = SXR_MEM_PATTERN;

#ifndef __SXR_CLUSTER__
#ifdef __SXR_DEBUG__
        if (Idx >= (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER))
        {
            SXS_TRACE(_SXR|TDB|TNB_ARG(3),TSTR("sxr_Malloc(%d bytes)= 0x%x in cluster %d\n",0x06a70003), Size, HBlock + 1, Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER));
        }
#endif
#endif

#ifdef __SXR_MEM_TRACKER__
        if (Idx >= (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER))
        {
            sxr_MemTrackAlloc (HBlock);
        }
#endif

        SX_PROFILE_FUNCTION_EXIT(_sxr_HMalloc);

        return (void *)(HBlock + 1);
    }

    sxr_ExitSc(Status);
    SX_ASSERT(FALSE, "Out of memory\n");

    SX_PROFILE_FUNCTION_EXIT(_sxr_HMalloc);

    return NIL;
}


// =============================================================================
// sxr_HFree
// -----------------------------------------------------------------------------
/// Free memory allocated in heap.
/// @param  Ptr on memory.
// =============================================================================
void _sxr_HFree (
#ifdef __SXR_RELEASED_PTR_TO_NIL__
    void **Ptr
#else
    void *Ptr
#endif
)
{
    u32 Status;
    u8  Idx;
#ifdef __SXR_RELEASED_PTR_TO_NIL__
    void *_Ptr = *Ptr;
#else
    void *_Ptr = Ptr;
#endif
    sxr_HMemHead_t *HBlock = ((sxr_HMemHead_t *)_Ptr) - 1;

#ifdef __SXR_MEM_TRACKER__
    u32 CallerAdd;
    SXS_GET_RA(CallerAdd);
#endif
    SX_PROFILE_FUNCTION_ENTER(_sxr_HFree);

    if (_Ptr == NIL)
    {
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Free request on NIL Ptr !!!\n",0x06a70004)));
    }

    Status = sxr_EnterSc();

#ifndef __TARGET__
    if ( sxr_NbUsedBlk == 0 )
    {
        SXS_RAISE ((_SXR|TDB,TSTR("NbAllocatedBlocks = 0 !!!\n",0x06a70005)));
    }

    sxr_NbUsedBlk--;
#endif

    if (HBlock -> H.Index != SXR_HEAP_ALLOCATED)
    {
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(2)|TDB,TSTR("Not an allocated block 0x%x 0x%lxy\n",0x06a70006), _Ptr, HBlock -> H.CallerAdd));
    }

    if (!sxr_CheckSum (&HBlock -> H.CheckSum))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Heap consistency failure %lx.\n",0x06a70007), _Ptr));
    }

    if (((u8 *)_Ptr) [(HBlock -> Size << 2) - HBlock -> H.DSize] != SXR_MEM_PATTERN)
    {
        //SXS_RAISE ((_SXR|TDB|TNB_ARG(2),TSTR("Memory deletion %lx offset %i\n",0x06a70008), _Ptr, HBlock -> Size));
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(3)|TDB,TSTR("Memory deletion (%lxy) %lx offset %i\n",0x06a70009), HBlock -> H.CallerAdd, _Ptr, HBlock -> Size));
    }

    //if (HBlock -> H.PIdx & SXR_POOL_OVERFLOW)
    {
        if ((sxr_MemoryAlert.ExtraAllocationCnt != 0)
                &&  (sxr_MemoryAlert.CallBack != NIL))
        {
            if (sxr_GetRelativeRemainingSize (0, sxr_MemoryAlert.ExtraAllocationCnt) > 10)
            {
                sxr_MemoryAlert.ExtraAllocationCnt = 0;
                sxr_MemoryAlert.CallBack (SXR_MEMORY_LEVEL_OK);
            }
        }
    }

    Idx = HBlock -> H.PIdx & ~(SXR_HEAP_MEMORY | SXR_POOL_OVERFLOW);


    if ((HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].Start)
            || (_Ptr > (void *)sxr_Heap [Idx].End))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(4),TSTR("Not a heap block %lx Heap %i Start %lx End %lx\n",0x06a7000a), HBlock, Idx, sxr_Heap [Idx].Start, sxr_Heap [Idx].End));
    }

#ifndef __SXR_CLUSTER__
#ifdef __SXR_DEBUG__
    if (Idx >= (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER))
    {
        SXS_TRACE(_SXR|TDB|TNB_ARG(2),TSTR("sxr_Free(0x%x) in cluster %d\n",0x06a7000b), _Ptr, Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)) ;
    }
#endif
#endif

    if (HBlock -> H.Link > 1)
    {
        HBlock -> H.Link--;
        sxr_SetCheckSum (&HBlock -> H.CheckSum);
        sxr_ExitSc (Status);
        SX_PROFILE_FUNCTION_EXIT(_sxr_HFree);
        return;
    }

    if (Idx >= (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER))
    {
        sxr_Cluster [Idx - (PAL_NB_RAM_AREA + SXR_NB_HEAP_USER)].NbAllocation--;

#ifdef __SXR_MEM_TRACKER__
        HBlock -> H.CallerAdd = CallerAdd;
        sxr_MemTrackFree (HBlock);
#endif
    }

    if (((((u32 *)HBlock) + HBlock -> Size) > sxr_Heap [Idx].End)
            || (HBlock != ((sxr_HMemHead_t *) ((u32 *)HBlock) [HBlock -> Size - 1])))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(3),TSTR("Heap block consistency failure (1) %lx Size %i HeapEnd %lx\n",0x06a7000c), HBlock, HBlock -> Size, sxr_Heap [Idx].End));
    }

    HBlock -> H.Index = SXR_HEAP_NOT_ALLOCATED;

    sxr_Heap [Idx].CurSize += HBlock -> Size << 2;

#if 0
    if ((HBlock != (sxr_HMemHead_t *)sxr_Heap [Idx].Start)
            &&  (((sxr_HMemHead_t *)((u32 *)(HBlock - 1))) -> H.Index == SXR_HEAP_NOT_ALLOCATED))
    {
        ((sxr_HMemHead_t *)((u32 *)(HBlock - 1))) -> Size += HBlock -> Size;
        HBlock = (sxr_HMemHead_t *)((u32 *)(HBlock - 1));

        ((u32 *)HBlock) [HBlock -> Size - 1] = (u32)HBlock;

        if (HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].Start)
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Heap block consistency failure (2) 0x%lx < 0x%lx\n",0x06a7000d), HBlock, sxr_Heap [Idx].Start));
        }
    }
#else
    if ((HBlock != (sxr_HMemHead_t *)sxr_Heap [Idx].Start)
            &&  (((sxr_HMemHead_t *)*(((u32 *)HBlock) - 1)) -> H.Index == SXR_HEAP_NOT_ALLOCATED))
    {
        ((sxr_HMemHead_t *)*(((u32 *)HBlock) - 1)) -> Size += HBlock -> Size;
        HBlock = (sxr_HMemHead_t *)*(((u32 *)HBlock) - 1);

        ((u32 *)HBlock) [HBlock -> Size - 1] = (u32)HBlock;

        if (HBlock < (sxr_HMemHead_t *)sxr_Heap [Idx].Start)
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(2),TSTR("Heap block consistency failure (2) 0x%lx < 0x%lx\n",0x06a7000e), HBlock, sxr_Heap [Idx].Start));
        }
    }
#endif

    if ((((u32 *)HBlock) + HBlock -> Size) < sxr_Heap [Idx].Top)
    {
        if ((((sxr_HMemHead_t *)(((u32 *)HBlock) + HBlock -> Size)) -> H.Index == SXR_HEAP_NOT_ALLOCATED) )
        {
            HBlock -> Size += ((sxr_HMemHead_t *)(((u32 *)HBlock) + HBlock -> Size)) -> Size;

            ((u32 *)HBlock) [HBlock -> Size - 1] = (u32)HBlock;
        }
    }
    else
    {
        sxr_Heap [Idx].Top = (u32 *)HBlock;
    }

#ifdef __SXR_RELEASED_PTR_TO_NIL__
    *Ptr = NIL;
#endif

    sxr_ExitSc (Status);

    sxr_CheckUpdateHeap (HBlock, Idx);

    SX_PROFILE_FUNCTION_EXIT(_sxr_HFree);
    return;
}


// =============================================================================
// sxr_GetLink
// -----------------------------------------------------------------------------
/// Get Link counter
/// @param  Ptr on allocated memory.
/// @return Number of link.
// =============================================================================
u8 sxr_GetLink (void *Ptr)
{
    return (((sxr_HMemHead_t *)Ptr) - 1) -> H.Link;
}


#ifdef __SXR_CLUSTER__
// =============================================================================
// sxr_Malloc
// -----------------------------------------------------------------------------
/// Memory allocation from cluster.
/// @param  Memory Size
/// @return Pointer onto memory block.
// =============================================================================
void *sxr_Malloc (u16 Size)
{
    u8 ClusterIdx;
    u32 CallerAdd;

#ifndef __TARGET__
    sxr_NbUsedBlk++;
#endif

    SXS_GET_RA(CallerAdd);

    SX_PROFILE_FUNCTION_ENTER(sxr_Malloc);

    if (sxr_NbCluster == 0)
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No cluster exist\n",0x06a7000f)));

    if (sxr_Cluster [sxr_NbCluster-1].Size <= Size)
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Required size too big.%i\n",0x06a70010), Size));

    for (ClusterIdx=0; ClusterIdx<sxr_NbCluster; ClusterIdx++)
    {
        if ( sxr_Cluster [ClusterIdx].Size > Size )
        {
            break;
        }
    }

    if (sxr_Cluster [ClusterIdx].FirstCluster == NIL_IDX)
    {
        sxr_CheckCluster ();
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Out of memory in cluster %i\n",0x06a70011), ClusterIdx));
    }

    {
        u32 Status = sxr_EnterSc ();

        sxr_MemHead_t *Ptr = (sxr_MemHead_t *)&((u8 *)sxr_Cluster [ClusterIdx].Add) [sxr_Cluster [ClusterIdx].FirstCluster * sxr_Cluster [ClusterIdx].AlignSize];

        if (!sxr_CheckSum (&Ptr -> CheckSum))
        {
            SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("Cluster consistency failure %lx.\n",0x06a70012), Ptr));
        }

        sxr_Cluster [ClusterIdx].FirstCluster = Ptr -> Next;
        Ptr -> CallerAdd = CallerAdd;
        Ptr -> Size      = Size;
        Ptr -> Link      = 1;

        sxr_SetCheckSum (&Ptr -> CheckSum);

        Ptr++;

#ifdef __SXR_DEBUG__
        memset ((u8 *)&((u8 *)Ptr)[Size], SXR_MEM_PATTERN, sxr_Cluster [ClusterIdx].Size - Size);
        SXS_TRACE(_SXR|TDB|TNB_ARG(3),TSTR("sxr_Malloc(%d bytes)= 0x%x in cluster %d\n",0x06a70013), Size, Ptr, ClusterIdx );
#else
        ((u8 *)Ptr)[Size] = SXR_MEM_PATTERN;
#endif

        sxr_ExitSc (Status);

#ifdef __SXR_MEM_TRACKER__
        //sxr_MemTrackAlloc (HBlock);
#endif

        SX_PROFILE_FUNCTION_EXIT(sxr_Malloc);
        return (void *)Ptr;
    }
}
#endif



// =============================================================================
// sxr_Link
// -----------------------------------------------------------------------------
/// Link a piece of memory already allocated.
/// @param  Ptr Pointer onto cluster
// =============================================================================
void sxr_Link (void *Ptr)
{
    sxr_MemHead_t *Header = (sxr_MemHead_t *)( ((u8 *)Ptr) - sizeof (sxr_MemHead_t));

    if (!sxr_CheckSum (&Header -> CheckSum))
    {
        Header = (sxr_MemHead_t *)( ((u8 *)Ptr) - sizeof (sxr_HMemHead_t));

        if (!sxr_CheckSum (&Header -> CheckSum))
        {
            SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Memory consistency failure %lx.\n",0x06a70014), Ptr));
        }
    }
    Header -> Link++;

#ifndef __TARGET__
    sxr_NbUsedBlk++;
#endif

#ifdef __SXR_DEBUG__
    SXS_TRACE(_SXR|TDB|TNB_ARG(1),TSTR("sxr_Link(0x%x)\n",0x06a70015), Ptr ) ;
#endif
    sxr_SetCheckSum (&Header -> CheckSum);
}


#ifdef __SXR_CLUSTER__
// =============================================================================
// sxr_Free
// -----------------------------------------------------------------------------
/// Free memory.
/// @param  Ptr Pointer onto memory block to be released.
// =============================================================================
void _sxr_Free (
#ifdef __SXR_RELEASED_PTR_TO_NIL__
    void **Ptr
#else
    void *Ptr
#endif
)
{
#ifdef __SXR_RELEASED_PTR_TO_NIL__
    void *_Ptr = *Ptr;
#else
    void *_Ptr = Ptr;
#endif
    sxr_MemHead_t *Header = (sxr_MemHead_t *)((u8 *)_Ptr - sizeof (sxr_MemHead_t));
    u8  ClusterIdx;

#ifdef __SXR_MEM_TRACKER__
    u32 CallerAdd;
    SXS_GET_RA(CallerAdd);
#endif
    SX_PROFILE_FUNCTION_ENTER(sxr_Free);

    if ((_Ptr == NIL)
            ||  (!sxr_CheckSum (&Header -> CheckSum)))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Bad Release %lx\n",0x06a70016), _Ptr));
    }

    if (((u8 *)_Ptr) [Header -> Size] != SXR_MEM_PATTERN)
    {
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(3)|TDB,TSTR("Memory deletion (%lxy) %lx offset %i\n",0x06a70017), Header -> CallerAdd, _Ptr, Header -> Size));
    }

    ClusterIdx = Header -> PIdx;

    if (ClusterIdx & SXR_HEAP_MEMORY)
    {
        sxr_HFree (_Ptr);
    }
    else
    {
        u32 Status ;

#ifndef __TARGET__
        if ( sxr_NbUsedBlk == 0 )
        {
            SXS_RAISE ((_SXR|TDB,TSTR("NbAllocatedBlocks = 0 !!!\n",0x06a70018)));
        }

        sxr_NbUsedBlk--;
#endif

        Status = sxr_EnterSc ();

        if (--Header -> Link == 0)
        {
            Header -> CallerAdd = SXR_POOL_FREE;
            Header -> Next      = sxr_Cluster [ClusterIdx].FirstCluster;

            sxr_Cluster [ClusterIdx].FirstCluster = Header -> Index;
            sxr_Cluster [ClusterIdx].NbAllocation--;

#ifdef __SXR_MEM_TRACKER__
            //HBlock -> H.CallerAdd = CallerAdd;
            //sxr_MemTrackFree (HBlock);
#endif
        }

        sxr_SetCheckSum (&Header -> CheckSum);

        sxr_CheckUpdateCluster ((u8 *)_Ptr, ClusterIdx);
#ifdef __SXR_DEBUG__
        SXS_TRACE(_SXR|TDB|TNB_ARG(2),TSTR("sxr_Free(0x%x) in cluster %d\n",0x06a70019), _Ptr, ClusterIdx ) ;
#endif

#ifdef __SXR_RELEASED_PTR_TO_NIL__
        if (Header -> Link == 0)
        {
            *Ptr = NIL;
        }
#endif
        sxr_ExitSc (Status);
    }
    SX_PROFILE_FUNCTION_EXIT(sxr_Free);
}
#endif



// =============================================================================
// sxr_GetMemSize
// -----------------------------------------------------------------------------
/// Get the allocated size of the memory which start at the specified address.
/// @param  Address Start address of the allocated memory.
/// @return Allocated size.
// =============================================================================
u16 sxr_GetMemSize (void *Address)
{
#ifdef __SXR_CLUSTER__
    return ((sxr_MemHead_t *)((u8 *)Address - sizeof (sxr_MemHead_t))) -> Size;
#else
    {
        sxr_HMemHead_t *HBlock = (sxr_HMemHead_t *)((u8 *)Address - sizeof (sxr_HMemHead_t));
        return (HBlock -> Size << 2) - HBlock -> H.DSize;
    }
#endif
}


// =============================================================================
// sxs_FuncName
// -----------------------------------------------------------------------------
/// Scope
/// @param
/// @return
// =============================================================================


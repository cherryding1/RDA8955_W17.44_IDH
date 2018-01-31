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
#include "sxr_usrmem.h"

/*
===============================================================================
   Function   : sxr_UsrMalloc
 -----------------------------------------------------------------------------
   Scope      : Memory allocation from user cluster.
   Parameters : Memory Size
   Return     : Pointer onto memory block.
===============================================================================
*/
void *sxr_UsrMalloc (u32 Size)
{
    u8 ClusterIdx;
    u32 CallerAdd;
    u32 Status;
    SXS_GET_RA(CallerAdd);

    if (sxr_NbUsrCluster == 0)
    {
        return NIL;
//  SXS_RAISE ((_SXR|TABORT|TDB,TSTR("No cluster exist\n",0x06a7000f)));
    }

    if (sxr_UsrCluster [sxr_NbUsrCluster-1].Size < Size)
    {
        return NIL;
        //SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Required size too big.%i\n",0x06a70010), Size));
    }

    Status = sxr_EnterSc ();

    for (ClusterIdx=0; ClusterIdx<sxr_NbUsrCluster; ClusterIdx++)
    {
        if ( (sxr_UsrCluster [ClusterIdx].Size >= Size) && (sxr_UsrCluster [ClusterIdx].FirstCluster != NIL_IDX) )
        {
            break;
        }
    }

    if (ClusterIdx== sxr_NbUsrCluster)
    {
        sxr_ExitSc (Status);
        //SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Out of memory in cluster %i\n",0x06a70011), ClusterIdx));
        return NIL;
    }
    else
    {

        sxr_HMemHead_t *Ptr = (sxr_HMemHead_t *)&((u8 *)sxr_UsrCluster [ClusterIdx].Add) [sxr_UsrCluster [ClusterIdx].FirstCluster * sxr_UsrCluster [ClusterIdx].AlignSize];

        if (!sxr_CheckSum (&Ptr -> H. CheckSum))
            SXS_RAISE ((_SXR|TDB|TNB_ARG(1),TSTR("Cluster consistency failure %lx.\n",0x06a70012), Ptr));

        sxr_UsrCluster [ClusterIdx].FirstCluster = Ptr -> H.Next;
        sxr_UsrCluster [ClusterIdx].NbAllocation++;
        Ptr -> H. CallerAdd = CallerAdd;
        Ptr -> H. DSize      =  sxr_UsrCluster [ClusterIdx].AlignSize - Size;
        Ptr -> H. Link      = 1;

        sxr_SetCheckSum (&Ptr -> H. CheckSum);

        Ptr -> Size = sxr_UsrCluster [ClusterIdx].AlignSize >> 2;

        Ptr++;

#ifdef __SXR_DEBUG__
        memset ((u8 *)&((u8 *)Ptr)[Size], SXR_MEM_PATTERN, sxr_UsrCluster [ClusterIdx].Size - Size);
        SXS_TRACE(_SXR|TDB|TNB_ARG(3),TSTR("sxr_UsrMalloc(%d bytes)= 0x%x in cluster %d\n",0x06a70022), Size, Ptr, ClusterIdx );
#else
        ((u8 *)Ptr)[Size] = SXR_MEM_PATTERN;
#endif

        sxr_ExitSc (Status);

#ifdef __SXR_MEM_TRACKER__
// sxr_MemTrackAlloc (HBlock);
#endif

        return (void *)Ptr;
    }
}


/*
===============================================================================
   Function   : _sxr_UsrFree
 -----------------------------------------------------------------------------
   Scope      : Free memory.
   Parameters : Pointer onto memory block to be released.
   Return     : none
===============================================================================
*/
#ifdef __SXR_RELEASED_PTR_TO_NIL__
void _sxr_UsrFree (void **Ptr)
#else
void sxr_UsrFree (void *Ptr)
#endif
{
#ifdef __SXR_RELEASED_PTR_TO_NIL__
    void *_Ptr = *Ptr;
#else
    void *_Ptr = Ptr;
#endif
    sxr_HMemHead_t *HBlock = (sxr_HMemHead_t *)((u8 *)_Ptr - sizeof (sxr_HMemHead_t));

    u8  ClusterIdx;

#ifdef __SXR_MEM_TRACKER__
    u32 CallerAdd;
    SXS_GET_RA(CallerAdd);
#endif

    if ((_Ptr == NIL)
            ||  (!sxr_CheckSum (&HBlock ->H. CheckSum)))
    {
        SXS_RAISE ((_SXR|TABORT|TDB|TNB_ARG(1),TSTR("Bad Release %lx\n",0x06a70016), _Ptr));
    }

    if (((u8 *)_Ptr) [(HBlock -> Size << 2) - HBlock -> H.DSize] != SXR_MEM_PATTERN)
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(3)|TDB,TSTR("Memory deletion (%lxy) %lx offset %i\n",0x06a70017), HBlock -> H. CallerAdd, _Ptr, HBlock -> H. DSize));

    ClusterIdx = HBlock ->H. PIdx;

    if (ClusterIdx & SXR_HEAP_MEMORY)
        sxr_HFree (_Ptr);
    else
    {
        u32 Status ;

        Status = sxr_EnterSc ();

        if (--HBlock ->H. Link == 0)
        {
            HBlock ->H. CallerAdd = SXR_POOL_FREE;
            HBlock ->H. Next      = sxr_UsrCluster [ClusterIdx].FirstCluster;

            sxr_UsrCluster [ClusterIdx].FirstCluster = HBlock ->H. Index;
            sxr_UsrCluster [ClusterIdx].NbAllocation--;

#ifdef __SXR_MEM_TRACKER__
//   HBlock -> H.CallerAdd = CallerAdd;
//   sxr_MemTrackFree (HBlock);
#endif
        }

        sxr_SetCheckSum (&HBlock ->H. CheckSum);

        sxr_CheckUpdateCluster ((u8 *)_Ptr, ClusterIdx);
#ifdef __SXR_DEBUG__
        SXS_TRACE(_SXR|TDB|TNB_ARG(2),TSTR("sxr_UsrFree(0x%x) in cluster %d\n",0x06a70023), _Ptr, ClusterIdx ) ;
#endif

#ifdef __SXR_RELEASED_PTR_TO_NIL__
        if (HBlock ->H. Link == 0)
            *Ptr = NIL;
#endif
        sxr_ExitSc (Status);
    }

}


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
#define __SXR_USRMEM_VAR__
#include "sxr_usrmem.h"
#undef __SXR_USRMEM_VAR__

/*
===============================================================================
   Function   : sxr_checkInUsrCluster
  ----------------------------------------------------------------------------
   Scope      : Check user cluster scope
   Parameters : paddress, malloc address
   Return     : TRUE if address found in cluster
===============================================================================
*/
bool sxr_checkInUsrCluster (void *paddress)
{
    int32 vi=0;
    for(vi=0; vi<sxr_NbUsrCluster; vi++)
    {
        if(paddress>=sxr_UsrCluster [vi].Add     &&
                paddress<(sxr_UsrCluster [vi].Add+(sxr_UsrCluster [vi].AlignSize*sxr_UsrCluster [vi].NbCluster)/sizeof(void)  ) )
        {
            return TRUE;
        }
    }

    return FALSE;
}



/*
===============================================================================
   Function   : sxr_NewUsrCluster
  ----------------------------------------------------------------------------
   Scope      : Cluster creation from a dedicated heap.
   Parameters : Size, Cluster size
                       NbCluster, number of cluster
                       Idx, heap index
   Return     : none
===============================================================================
*/
void sxr_NewUsrCluster (u16 Size, u16 NbCluster, u8 Idx)
{

    u32 i;
    sxr_HMemHead_t *HBlock = NIL;
    u16 AlignSize = ( (Size + 3 + 1 + 4 + sizeof (sxr_HMemHead_t)) & ~3 );

    u32 Status = sxr_EnterSc ();

    if (sxr_NbUsrCluster >= SXR_NB_USER_CLUSTER)
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Too many clusters.\n",0x06bc0001)));

    u8 usrClusterIdx = sxr_NbUsrCluster;
    sxr_NbUsrCluster++;

    sxr_ExitSc (Status);

// FIXME The malloc getting memory for the cluster
// cast the size on 16 bits (We don't know why).
// Thus a cluster cannot be bigger than 64kB, taking
// headers into account.
    if ((AlignSize * NbCluster) >= (1 << 16))
        SXS_RAISE ((_SXR|TABORT|TNB_ARG(2)|TDB,TSTR("Total cluster > 64 k Size %i Nb %i.\n",0x06bc0002), Size, NbCluster));

    sxr_UsrCluster [usrClusterIdx].Size         = Size;
    sxr_UsrCluster [usrClusterIdx].AlignSize    = AlignSize;
    sxr_UsrCluster [usrClusterIdx].NbCluster    = NbCluster;
    sxr_UsrCluster [usrClusterIdx].Add          = _sxr_HMalloc((u16) ( AlignSize * NbCluster), Idx); //sxr_HMalloc ((u16) ( AlignSize * NbCluster));
    sxr_UsrCluster [usrClusterIdx].FirstCluster = 0;
    sxr_UsrCluster [usrClusterIdx].NbAllocation = 0;

    if  ( sxr_UsrCluster [usrClusterIdx].Add == NIL )
        SXS_RAISE ((_SXR|TABORT|TDB,TSTR("Out of memory for cluster creation.\n",0x06bc0003)));

    memset ( (u8 *)sxr_UsrCluster [usrClusterIdx].Add, 0, ( AlignSize * NbCluster));

    for ( i=0; i<NbCluster; i++ )
    {
        HBlock = (sxr_HMemHead_t *)(&((u8 *)sxr_UsrCluster [usrClusterIdx].Add) [i*AlignSize]);

        HBlock ->H. CallerAdd = SXR_POOL_FREE;
        HBlock ->H. DSize     = 0;
        HBlock ->H. Link     = 0;
        HBlock ->H. Index    = i;
        HBlock ->H. Next     = (i+1);
        HBlock ->H. PIdx     = usrClusterIdx;
        sxr_SetCheckSum (&HBlock ->H. CheckSum);

        HBlock -> Size = 0;

        ((u8 *)sxr_UsrCluster [usrClusterIdx].Add) [(i+1)*AlignSize - 1] = SXR_MEM_PATTERN;

#ifdef __SXR_DEBUG__
        memset (((u8 *)HBlock) + sizeof (sxr_HMemHead_t), SXR_MEM_PATTERN, Size);
#endif
    }

    HBlock ->H. Next = NIL_IDX;
    sxr_SetCheckSum (&HBlock ->H. CheckSum);
}

#ifdef __USER_CLUSTER_TEST_FUNC__
U16 user_cluster_size[SXR_NB_USER_CLUSTER] = {0,};
U16 user_cluster_firstcluster_max[SXR_NB_USER_CLUSTER] = {0,};
U8 user_cluster_used_total =0;
void sxr_userCluster_check(void)
{
    U8 i = 0;
    user_cluster_used_total = sxr_NbUsrCluster;

    for(; i<sxr_NbUsrCluster; i++)
    {
        //need update
        user_cluster_size[i] = sxr_UsrCluster [i].AlignSize;
        if(user_cluster_firstcluster_max[i] < sxr_UsrCluster [i].FirstCluster)
        {
            user_cluster_firstcluster_max[i]= sxr_UsrCluster [i].FirstCluster ;
        }
    }
}
#endif

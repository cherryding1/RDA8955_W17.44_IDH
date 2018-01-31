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

#ifndef _DSM_CONFIG_H_
#define _DSM_CONFIG_H_


// =============================================================================
// DSM_CONFIG_T
// -----------------------------------------------------------------------------
// This structue discripte the DSM partition config information,include partition number and partition table.
// dsmPartitionNumber: Partition nmuber.
// dsmPartitionInfo: Array of partition information.
// =============================================================================
typedef struct _dsm_config_t
{
    // Partition number.
    UINT32                  dsmPartitionNumber;
    // Partiton table.
#ifdef WIN32
    DSM_PARTITION_CONFIG    dsmPartitionInfo[16];
#else
    DSM_PARTITION_CONFIG    dsmPartitionInfo[];
#endif
} DSM_CONFIG_T;
CONST DSM_CONFIG_T *tgt_GetDsmCfg(VOID);
#endif


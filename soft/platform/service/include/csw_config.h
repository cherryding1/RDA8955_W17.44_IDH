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

#ifndef COS_CONFIG_H
#define COS_CONFIG_H

// =============================================================================
// CSW_CONFIG_T
// -----------------------------------------------------------------------------
/// This structure describes the user heap size
/// user heap is defined by SXR_NB_HEAP_USER which is currently set to 3
/// Any other heap would require to change the define in sxr_cnf.h

/// cswHeapSize: Size of the heap available for csw
/// cosHeapSize: Size of the heap available for mmi

// =============================================================================
// Only the memory part is represented but more config could be added
typedef struct
{
    // Memory configuration
    UINT32 cswHeapSize;
    UINT32 cosHeapSize;
} CSW_CONFIG_T;

#endif //end of COS_CONFIG_H


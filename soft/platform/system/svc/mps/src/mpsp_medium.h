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


#ifndef _MPSP_MEDIUM_H_
#define _MPSP_MEDIUM_H_

#include "cs_types.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MPS_MEDIUM_TYPE_T
// -----------------------------------------------------------------------------
/// List of input medium
// =============================================================================
typedef enum
{
    MPS_MEDIUM_TYPE_INVALID,
    MPS_MEDIUM_TYPE_FS,
    MPS_MEDIUM_TYPE_MEM
} MPS_MEDIUM_TYPE_T;

// =============================================================================
// MPS_MEDIUM_DATA_FS_T
// -----------------------------------------------------------------------------
/// File descriptor for filesystem medium
// =============================================================================
typedef struct
{
    INT32 fd;
} MPS_MEDIUM_DATA_FS_T ;

// =============================================================================
// MPS_MEDIUM_DATA_MEM_T
// -----------------------------------------------------------------------------
/// Buffer description and position for memory medium
// =============================================================================
typedef struct
{
    UINT8* buffer;
    UINT32 size;

    UINT32 position;
} MPS_MEDIUM_DATA_MEM_T ;

// =============================================================================
// MPS_MEDIUM_DATA_T
// -----------------------------------------------------------------------------
/// Internal medium data
// =============================================================================
typedef union
{
    MPS_MEDIUM_DATA_FS_T  fs;
    MPS_MEDIUM_DATA_MEM_T mem;
} MPS_MEDIUM_DATA_T;

// =============================================================================
// MPS_MEDIUM_T
// -----------------------------------------------------------------------------
/// Medium description
// =============================================================================
typedef struct
{
    MPS_MEDIUM_TYPE_T type;
    MPS_MEDIUM_DATA_T data;
} MPS_MEDIUM_T ;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

PROTECTED INT32 mps_MediumOpen(MPS_MEDIUM_T* medium, MPS_MEDIUM_TYPE_T mediumType,
                               UINT8* buffer, UINT32 size);

PROTECTED INT32 mps_MediumClose(MPS_MEDIUM_T* medium);

PROTECTED INT32 mps_MediumRead(MPS_MEDIUM_T* medium, VOID* buffer, UINT32 size);

PROTECTED INT32 mps_MediumSeek(MPS_MEDIUM_T* medium, INT32 offset, INT32 whence);

PROTECTED UINT32 mps_MediumTell(MPS_MEDIUM_T* medium);

PROTECTED BOOL mps_MediumIsEof(MPS_MEDIUM_T* medium);

PROTECTED UINT32 mps_MediumSize(MPS_MEDIUM_T* medium);

#endif // _MPSP_MEDIUM_H_

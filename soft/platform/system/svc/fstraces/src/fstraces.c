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

#include "fstraces.h"
#include "sxs_io.h"
#include "sxs_srl.h"
#include "sxs_rmt.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// FUNCTIONS
// =============================================================================
// =============================================================================
// fstraces_Open
// -----------------------------------------------------------------------------
/// enable recording trace to T card
// =============================================================================
PUBLIC VOID fstraces_Open()
{
    hal_HstTraceSetTxEnabled(FALSE);
}

// =============================================================================
// fstraces_Close
// -----------------------------------------------------------------------------
/// Disable recording trace to T card
// =============================================================================
PUBLIC VOID fstraces_Close(VOID)
{

}

// =============================================================================
// fstraces_Read
// -----------------------------------------------------------------------------
/// Read a bunch of data from the debug remote buffer.
/// @param data   points a buffer where data must be copied.
/// @param data_len      Size in bytes of buffer pointed by data.
// =============================================================================
PUBLIC UINT16  fstraces_Read(UINT8 *data,UINT16 data_len)
{
    return hal_HstTraceFetch(data, data_len);
}

// =============================================================================
// fstraces_setLevel
// -----------------------------------------------------------------------------
/// Set trace level
/// @param Id   trace Id
/// @param LevelBitmap, bitmap of trace Id
// =============================================================================
PUBLIC VOID fstraces_setLevel(UINT8 Id, UINT16 LevelBitmap )
{
    sxs_SetTraceLevel(Id, LevelBitmap);
}

// =============================================================================
// fstraces_getStatus
// -----------------------------------------------------------------------------
/// Get status of external trace flag
// =============================================================================
PUBLIC UINT32 fstraces_getStatus()
{
    return (1);

}



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


#ifndef _MALLOC_H_
#define _MALLOC_H_

#include "cs_types.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// h263_myMalloc
// -----------------------------------------------------------------------------
/// This function alloc memory and register the memory allocated in internal
/// list
///
/// @param size Size of memory allocated
/// @return memory allocated
// =============================================================================
VOID* h263_myMalloc(UINT32 size);

// =============================================================================
// h263_myFree
// -----------------------------------------------------------------------------
/// This function free memory allocated by h263_malloc and unregister the memory
/// allocated in internal list
///
/// @param buffer Memory buffer, you want free
// =============================================================================
VOID h263_myFree(VOID* buffer);

// =============================================================================
// h263_freeAll
// -----------------------------------------------------------------------------
/// This function free all memory allocated by h263_malloc and already register
/// in internal list
// =============================================================================
VOID h263_freeAll(VOID);

#endif // _MALLOC_H_




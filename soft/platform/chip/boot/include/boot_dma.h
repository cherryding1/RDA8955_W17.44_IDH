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



#ifndef _BOOT_DMA_H_
#define _BOOT_DMA_H_

/// @defgroup BOOT DMA Driver
///
/// @{
///

#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// boot_DmaCopy
// -----------------------------------------------------------------------------
/// Copy data via DMA
/// @param dst  The destination address
/// @param src  The source address
/// @param len  The data length in bytes
/// @return None
// =============================================================================

PUBLIC VOID boot_DmaCopy(VOID *dst, CONST VOID *src, UINT32 len);


// =============================================================================
// boot_DmaSet
// -----------------------------------------------------------------------------
/// Set the memory to a pattern value via DMA
/// @param dst      The destination address (should be word-aligned)
/// @param pattern  The pattern to be written
/// @param len      The data length in bytes (should be mulitple of 4)
/// @return None
// =============================================================================

PUBLIC VOID boot_DmaSet(UINT32 *dst, UINT32 pattern, UINT32 len);


/// @} <-- End of the dma group

#endif // _BOOT_DMA_H_



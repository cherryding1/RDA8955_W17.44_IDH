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


#include "cs_types.h"

// =============================================================================
// hal_PMemEnableExternalAccess
// -----------------------------------------------------------------------------
/// Enable or disable the external access to the proxy memory.
/// The proxy memory is normally only accessible by its suzerain CPU. In order
/// to program its content (eg Through a DMA loading an overlay), it is necessary
/// to enable the access to this memory from the external (From a CPU
/// stand point) APB bus. 'Internal' access must be restored before using
/// this memory from the CPU by a call to #hal_PMemEnableExternalAccess(FALSE).
///
/// @param enable When \c TRUE, enable access to the proxy memory for
/// programmation. When \c FALSE, disable that access. (Only the CPU can access
/// the memory).
// =============================================================================
PROTECTED VOID hal_PMemEnableExternalAccess(BOOL enable);








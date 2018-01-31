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


















#ifndef VPPP_HELLO_H
#define VPPP_HELLO_H

#include "cs_types.h"

///@defgroup vppp_hello_struct
///@{

// ============================================================================
// Types
// ============================================================================


// ============================================================================
// VPP_HELLO_CODE_CFG_T
// ----------------------------------------------------------------------------
/// Internal VPP Hello configuration structure
// ============================================================================

typedef struct
{
    /// pointer to the hi code
    INT32* hiPtr;
    /// pointer to the data constants
    INT32* constPtr;

} VPP_HELLO_CODE_CFG_T;


///  @} <- End of the vppp_hello_struct group


#endif  // VPPP_HELLO_H

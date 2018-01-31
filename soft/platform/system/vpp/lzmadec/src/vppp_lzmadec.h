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


















#ifndef VPPP_LZMADEC_H
#define VPPP_LZMADEC_H

#include "cs_types.h"

///@defgroup vppp_hello_struct
///@{

// ============================================================================
// Types
// ============================================================================


// ============================================================================
// VPP_LZMADEC_CODE_CFG_T
// ----------------------------------------------------------------------------
/// Internal VPP LzmaDec configuration structure
// ============================================================================

typedef struct
{
    /// pointer to the hi code
    INT32* hiPtr;
    /// pointer to the bye code
    INT32* byePtr;
    /// pointer to the data constants
    INT32* constPtr;


    INT32* inbuf;

    INT32* outbuf;



} VPP_LZMADEC_CODE_CFG_T;


///  @} <- End of the vppp_hello_struct group


#endif  // VPPP_LZMADEC_H

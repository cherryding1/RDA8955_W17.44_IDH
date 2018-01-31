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


#ifndef _SPC_MAP_H_
#define _SPC_MAP_H_


#include "spc_mailbox.h"
#include "spc_ctx.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPC_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// Type used to define the accessible structures of SPC.
// =============================================================================
typedef struct
{
    SPC_MAILBOX_T*                 mailbox;                      //0x00000000
    SPC_CONTEXT_T*                 context;                      //0x00000004
} SPC_MAP_ACCESS_T; //Size : 0x8





#endif


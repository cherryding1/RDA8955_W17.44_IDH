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























#ifndef _CSW_SHELL_H_
#define _CSW_SHELL_H_

#include <cfw.h>

//
// Shell
//

UINT32 SRVAPI SHL_EraseFlash(
    UINT8 nMode  // 0: FS flash area, 1: all flash area exclude code.
);

#ifdef CFW_PBK_SYNC_VER
UINT32 SRVAPI SHL_SimAddPbkEntry (
    UINT8 nStorage,
    CFW_SIM_PBK_ENTRY_INFO *pEntryInfo,
    CFW_PBK_OUT_PARAM *pOutParam
);

UINT32 SRVAPI SHL_SimDeletePbkEntry (
    UINT8 nStorage,
    UINT8 nIndex,
    CFW_PBK_OUT_PARAM *pOutParam
);
#endif

#endif // _H


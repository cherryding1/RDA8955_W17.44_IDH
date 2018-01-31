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

#ifndef _AT_CMD_SPECIAL_UPDATE_H_
#define _AT_CMD_SPECIAL_UPDATE_H_

#ifdef AT_FOTA_SUPPORT

#include "at_common.h"

VOID AT_FOTA_CmdFunc_UPDATE(AT_CMD_PARA *pParam);
VOID AT_FOTA_CmdFunc_UPGRADE(AT_CMD_PARA *pParam);

#endif // AT_FOTA_SUPPORT
#endif

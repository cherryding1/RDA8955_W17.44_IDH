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

#if !defined(__DM_VOIS_H__)
#define __DM_VOIS_H__

#include "vois_m.h"

BOOL DM_VoisRecordStart(INT32 *startAddress, INT32 length, VOIS_USER_HANDLER_T handler);
BOOL DM_VoisRecordStop(VOID);

#endif // __DM_VOIS_H__


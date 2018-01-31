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



#ifndef RM_IO_DEFAULT_H
#define RM_IO_DEFAULT_H

#include "helix_types.h"
#include "helix_result.h"

UINT32 rm_io_default_read(void*  pUserRead, BYTE* pBuf, UINT32 ulBytesToRead);
void rm_io_default_seek(void* pUserRead, UINT32 ulOffset, UINT32 ulOrigin);

#endif /* #ifndef RM_IO_DEFAULT_H */

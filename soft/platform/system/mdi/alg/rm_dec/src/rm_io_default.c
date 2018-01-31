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



#include <stdio.h>
#include "helix_types.h"
#include "rm_io_default.h"


UINT32 rm_io_default_read(void* pUserRead, BYTE* pBuf, UINT32 ulBytesToRead)
{
    UINT32 ulRet = 0;
#ifdef SHEEN_VC_DEBUG
    if (pUserRead && pBuf && ulBytesToRead)
#else
    if (pBuf && ulBytesToRead)
#endif
    {
        /* For default, the void* is a FILE* */
        FILE_HDL fp = (FILE_HDL) pUserRead;
        /* Read the number of bytes requested */
#ifdef SHEEN_VC_DEBUG
        ulRet = (UINT32) fread(pBuf, 1, ulBytesToRead, fp);
#else
        ulRet = (UINT32) FS_Read(fp, pBuf, ulBytesToRead);
#endif
    }

    return ulRet;
}

void rm_io_default_seek(void* pUserRead, UINT32 ulOffset, UINT32 ulOrigin)
{
#ifdef SHEEN_VC_DEBUG
    if (pUserRead)
#endif
    {
        /* For default, the void* is a FILE* */
        FILE_HDL fp = (FILE_HDL) pUserRead;
        /* Do the seek */
#ifdef SHEEN_VC_DEBUG
        fseek(fp, ulOffset, ulOrigin);
#else
        FS_Seek(fp, ulOffset , ulOrigin);
#endif
    }
}


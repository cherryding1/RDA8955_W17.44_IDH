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




#include "mpsp_medium.h"
#include "mpsp_debug.h"

#include "fs.h"

#include <string.h>


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

#ifndef MPS_MEDIUM_PRINTF
#undef MPS_TRACE
#define MPS_TRACE(fmt, ...)
#endif

PROTECTED INT32 mps_MediumOpen(MPS_MEDIUM_T* medium, MPS_MEDIUM_TYPE_T mediumType,
                               UINT8* buffer, UINT32 size)
{
    MPS_ASSERT(medium, "mps_MediumOpenBuffer: medium == NULL");

    MPS_TRACE("mps_MediumOpen type=%i buffer=%#x size=%i", mediumType, buffer, size);

    switch(mediumType)
    {
#ifndef WITHOUT_FS
        case MPS_MEDIUM_TYPE_FS:
            medium->type              = mediumType;
            medium->data.fs.fd        = FS_Open(buffer, FS_O_RDONLY, 0);
            return medium->data.fs.fd;
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_MEM:
            medium->type              = mediumType;
            medium->data.mem.buffer   = buffer;
            medium->data.mem.size     = size;
            medium->data.mem.position = 0;
            return 0;
        default:
            break;
    }

    return -1;
}

PROTECTED INT32 mps_MediumClose(MPS_MEDIUM_T* medium)
{
    MPS_ASSERT(medium, "mps_MediumClose: medium == NULL");
    MPS_ASSERT(medium->type != MPS_MEDIUM_TYPE_INVALID,
               "mps_MediumClose: medium->type == MPS_MEDIUM_TYPE_INVALID");

    MPS_TRACE("mps_MediumClose");

    switch(medium->type)
    {
        case MPS_MEDIUM_TYPE_MEM:
            medium->data.mem.buffer   = 0;
            medium->data.mem.size     = 0;
            medium->data.mem.position = 0;
            medium->type              = MPS_MEDIUM_TYPE_INVALID;
            return 0;
        case MPS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            FS_Close(medium->data.fs.fd);
            medium->data.fs.fd        = -1;
            medium->type              = MPS_MEDIUM_TYPE_INVALID;
            return 0;
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_INVALID:
            break;
    }

    return -1;
}

PROTECTED INT32 mps_MediumRead(MPS_MEDIUM_T* medium, VOID* buffer, UINT32 size)
{
#ifndef WITHOUT_FS
    INT32 readBytes;
#endif /* WITHOUT_FS */
    MPS_ASSERT(medium, "mps_MediumRead: medium == NULL");
    MPS_ASSERT(buffer, "mps_MediumRead: buffer == NULL");
    MPS_ASSERT(medium->type != MPS_MEDIUM_TYPE_INVALID,
               "mps_MediumRead: medium->type == MPS_MEDIUM_TYPE_INVALID");

    MPS_TRACE("mps_MediumRead: size=%i", size);

    switch(medium->type)
    {
        case MPS_MEDIUM_TYPE_MEM:
            MPS_TRACE("mps_MediumRead: mem=%#x File position=%i",
                      &medium->data.mem.buffer[medium->data.mem.position],
                      medium->data.mem.position);
            if(size+medium->data.mem.position > medium->data.mem.size)
            {
                size = medium->data.mem.size-medium->data.mem.position;
            }
            if(size != 0)
            {
                memcpy(buffer, &medium->data.mem.buffer[medium->data.mem.position], size);
            }
            medium->data.mem.position += size;
            return size;
        case MPS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            readBytes = FS_Read(medium->data.fs.fd, buffer, size);
            return readBytes;
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_INVALID:
            break;
    }
    return -1;
}

PROTECTED INT32 mps_MediumSeek(MPS_MEDIUM_T* medium, INT32 offset, INT32 whence)
{
#ifndef WITHOUT_FS
    INT32 newPosition;
#endif /* WITHOUT_FS */

    MPS_ASSERT(medium, "mps_MediumSeek: medium == NULL");
    MPS_ASSERT(medium->type != MPS_MEDIUM_TYPE_INVALID,
               "mps_MediumSeek: medium->type == MPS_MEDIUM_TYPE_INVALID");

    MPS_TRACE("mps_MediumSeek: offset=%i whence %i", offset, whence);

    switch(medium->type)
    {
        case MPS_MEDIUM_TYPE_MEM:
            switch(whence)
            {
                case FS_SEEK_SET:
                    medium->data.mem.position = offset;
                    if(offset > medium->data.mem.size)
                    {
                        medium->data.mem.position = medium->data.mem.size;
                    }
                    break;
                case FS_SEEK_CUR:
                    if(offset+medium->data.mem.position > medium->data.mem.size)
                    {
                        medium->data.mem.position = medium->data.mem.size;
                    }
                    else
                    {
                        if(offset+medium->data.mem.position < 0)
                        {
                            medium->data.mem.position = 0;
                        }
                        else
                        {
                            medium->data.mem.position += offset;
                        }
                    }
                    break;
                case FS_SEEK_END:
                    medium->data.mem.position = medium->data.mem.size - offset;
                    if(offset > medium->data.mem.size)
                    {
                        medium->data.mem.position = 0;
                    }
                    break;
            }
            return medium->data.mem.position;
        case MPS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            newPosition = (INT32)FS_Seek(medium->data.fs.fd, (INT64)offset, whence);
            return newPosition;
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_INVALID:
            break;
    }
    return -1;
}

PROTECTED BOOL mps_MediumIsEof(MPS_MEDIUM_T* medium)
{
    MPS_ASSERT(medium, "mps_MediumIsEof: medium == NULL");
    MPS_ASSERT(medium->type != MPS_MEDIUM_TYPE_INVALID,
               "mps_MediumIsEof: medium->type == MPS_MEDIUM_TYPE_INVALID");

    switch(medium->type)
    {
        case MPS_MEDIUM_TYPE_MEM:
            if(medium->data.mem.position == medium->data.mem.size)
            {
                return TRUE;
            }
            break;
        case MPS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            return FS_IsEndOfFile(medium->data.fs.fd);
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_INVALID:
            break;
    }
    return FALSE;
}

PROTECTED UINT32 mps_MediumSize(MPS_MEDIUM_T* medium)
{
#ifndef WITHOUT_FS
    UINT32 oldPos;
    UINT32 pos;
#endif /* WITHOUT_FS */

    MPS_ASSERT(medium, "mps_MediumSize: medium == NULL");
    MPS_ASSERT(medium->type != MPS_MEDIUM_TYPE_INVALID,
               "mps_MediumSize: medium->type == MPS_MEDIUM_TYPE_INVALID");

    switch(medium->type)
    {
        case MPS_MEDIUM_TYPE_MEM:
            return medium->data.mem.size;
        case MPS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            oldPos = (UINT32)FS_Seek(medium->data.fs.fd, 0, FS_SEEK_CUR);
            pos    = (UINT32)FS_Seek(medium->data.fs.fd, 0, FS_SEEK_END);
            FS_Seek(medium->data.fs.fd, (INT64)oldPos, FS_SEEK_SET);
            return pos;
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_INVALID:
            break;
    }
    return 0;
}

PROTECTED UINT32 mps_MediumTell(MPS_MEDIUM_T* medium)
{
    MPS_ASSERT(medium, "mps_MediumTell: medium == NULL");
    MPS_ASSERT(medium->type != MPS_MEDIUM_TYPE_INVALID,
               "mps_MediumTell: medium->type == MPS_MEDIUM_TYPE_INVALID");

    switch(medium->type)
    {
        case MPS_MEDIUM_TYPE_MEM:
            return medium->data.mem.position;
        case MPS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            return (UINT32)FS_Seek(medium->data.fs.fd, 0, FS_SEEK_CUR);
#endif /* WITHOUT_FS */
        case MPS_MEDIUM_TYPE_INVALID:
            break;
    }

    return 0;
}

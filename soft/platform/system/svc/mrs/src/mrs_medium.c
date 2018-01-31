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

#include "mrsp_medium.h"
#include "mrsp_debug.h"

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

// =============================================================================
// mrs_MediumOpenFs
// -----------------------------------------------------------------------------
/// Open a file, and store its configuration in the medium structure pointed.
///
/// @param medium Structure where the opened file configuration will be stored
/// after the function has been called.
/// @param path Path to the file to open (string).
/// @param mode Define the behaviour to adopt on the opened file:
///   - we are creating or replacing the file: use #MRS_FILE_MODE_CREATE.
///   - we are appending data to an already existing file: use #MRS_FILE_MODE_APPEND.
///   - we are reading the file: use another value. (@todo Add a READ mode).
/// @return The file descriptor.
// =============================================================================
PROTECTED INT32 mrs_MediumOpenFs(MRS_MEDIUM_T* medium, UINT8* path, MRS_FILE_MODE_T mode)
{
    MRS_ASSERT(medium, "mrs_MediumOpenFs: medium == NULL");

    medium->type       = MRS_MEDIUM_TYPE_FS;
    switch (mode)
    {
        case MRS_FILE_MODE_CREATE:
            medium->data.fs.fd = FS_Open(path, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
            break;

        case MRS_FILE_MODE_APPEND:
            medium->data.fs.fd = FS_Open(path, FS_O_RDWR, 0);
            // Go to the file's end.
            if (medium->data.fs.fd >= 0)
            {
                FS_Seek(medium->data.fs.fd, 0, FS_SEEK_END);
            }
            break;

        default:
            // Read only
            medium->data.fs.fd = FS_Open(path, FS_O_RDONLY, 0);
            break;
    }


    return medium->data.fs.fd;
}


// =============================================================================
// mrs_MediumOpenBuffer
// -----------------------------------------------------------------------------
/// DON'T USE.
// =============================================================================
PROTECTED INT32 mrs_MediumOpenBuffer(MRS_MEDIUM_T* medium, UINT8* buffer, UINT32 size)
{
    MRS_ASSERT(medium, "mrs_MediumOpenBuffer: medium == NULL");

    medium->type              = MRS_MEDIUM_TYPE_MEM;
    medium->data.mem.buffer   = buffer;
    medium->data.mem.size     = size;
    medium->data.mem.position = 0;

    return 0;
}


// =============================================================================
// mrs_MediumClose
// -----------------------------------------------------------------------------
/// Close a medium.
///
/// @param medium The medium to close.
/// @return 0 in case of success. -1 when the medium is improper, and couldn't
/// be closed.
// =============================================================================
PROTECTED INT32 mrs_MediumClose(MRS_MEDIUM_T* medium)
{
    MRS_ASSERT(medium, "mrs_MediumClose: medium == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumClose: medium->type == MRS_MEDIUM_TYPE_INVALID");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_MediumClose");

    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
            medium->data.mem.buffer   = 0;
            medium->data.mem.size     = 0;
            medium->data.mem.position = 0;
            medium->type              = MRS_MEDIUM_TYPE_INVALID;
            return 0;
        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            FS_Close(medium->data.fs.fd);
            medium->data.fs.fd = -1;
            medium->type              = MRS_MEDIUM_TYPE_INVALID;
            return 0;
#endif // WITHOUT_FS 
        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }
    return -1;
}


// =============================================================================
// mrs_MediumRead
// -----------------------------------------------------------------------------
/// Read data from a medium.
///
/// @param medium Medium to read.
/// @param buffer Buffer in memory where to store the read data.
/// @param size Number of bytes of data to read.
/// @return The number of read bytes when the read is successful, or a negative
/// number when the read failed. The returned value is the file system error
/// code when the medium is a file.
// =============================================================================
PROTECTED INT32 mrs_MediumRead(MRS_MEDIUM_T* medium, VOID* buffer, UINT32 size)
{
#ifndef WITHOUT_FS
    INT32 readBytes;
#endif // WITHOUT_FS
    MRS_ASSERT(medium, "mrs_MediumRead: medium == NULL");
    MRS_ASSERT(buffer, "mrs_MediumRead: buffer == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumRead: medium->type == MRS_MEDIUM_TYPE_INVALID");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_MediumRead: size=%i", size);

    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_MediumRead: mem=%#x File position=%i",
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

        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            readBytes = FS_Read(medium->data.fs.fd, buffer, size);
            return readBytes;

#endif // WITHOUT_FS 
        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }
    return -1;
}


// =============================================================================
// mrs_MediumWrite
// -----------------------------------------------------------------------------
/// Write data from a medium.
///
/// @param medium Medium to write.
/// @param buffer Buffer in memory where to fetch the data to write.
/// @param size Number of bytes of data to write.
/// @return The number of written bytes when the write is successful, or a negative
/// number when the write failed. The returned value is the file system error
/// code when the medium is a file.
// =============================================================================
PROTECTED INT32 mrs_MediumWrite(MRS_MEDIUM_T* medium, VOID* buffer, UINT32 size)
{
    MRS_ASSERT(medium, "mrs_MediumWrite: medium == NULL");
    MRS_ASSERT(buffer, "mrs_MediumWrite: buffer == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumWrite: medium->type == MRS_MEDIUM_TYPE_INVALID");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_MediumWrite: size=%i", size);

#ifndef WITHOUT_FS
    INT32 writtenBytes;
#endif // WITHOUT_FS

    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
            MRS_TRACE(MRS_INFO_TRC, 0, "mrs_MediumWrite: mem=%#x File position=%i",
                      &medium->data.mem.buffer[medium->data.mem.position],
                      medium->data.mem.position);
            if(size+medium->data.mem.position > medium->data.mem.size)
            {
                size = medium->data.mem.size-medium->data.mem.position;
            }
            if(size != 0)
            {
                memcpy(&medium->data.mem.buffer[medium->data.mem.position], buffer, size);
            }
            medium->data.mem.position += size;
            return size;

        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            writtenBytes = FS_Write(medium->data.fs.fd, buffer, size);
            return writtenBytes;

#endif // WITHOUT_FS 

        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }
    return -1;
}


// =============================================================================
// mrs_MediumSeek
// -----------------------------------------------------------------------------
/// Seek in a medium.
///
/// @param medium to seek.
/// @param offset Seek offset.
/// @param Seek whence (FS_SEEK_SET, FS_SEEK_CUR, or FS_SEEK_END)
/// @return The position in the media (positive value), or an error message from
/// the filesystem, or -1.
// =============================================================================
PROTECTED INT32 mrs_MediumSeek(MRS_MEDIUM_T* medium, INT32 offset, INT32 whence)
{
#ifndef WITHOUT_FS
    INT32 newPosition;
#endif // WITHOUT_FS 

    MRS_ASSERT(medium, "mrs_MediumSeek: medium == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumSeek: medium->type == MRS_MEDIUM_TYPE_INVALID");

    MRS_TRACE(MRS_INFO_TRC, 0, "mrs_MediumSeek: offset=%i whence %i", offset, whence);

    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
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

        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            newPosition = (INT32)FS_Seek(medium->data.fs.fd,(INT64)offset, whence);
            return newPosition;

#endif // WITHOUT_FS 
        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }
    return -1;
}


// =============================================================================
// mrs_MediumIsEof
// -----------------------------------------------------------------------------
/// Check the end of a medium (end of file or of buffer)
///
/// @param medium Medium to check
/// @return \c TRUE if the medium reached its end, \c FALSE otherwise.
// =============================================================================
PROTECTED BOOL mrs_MediumIsEof(MRS_MEDIUM_T* medium)
{
    MRS_ASSERT(medium, "mrs_MediumIsEof: medium == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumIsEof: medium->type == MRS_MEDIUM_TYPE_INVALID");
    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
            if(medium->data.mem.position == medium->data.mem.size)
            {
                return TRUE;
            }
        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            return FS_IsEndOfFile(medium->data.fs.fd);
#endif // WITHOUT_FS 
        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }
    return FALSE;
}


// =============================================================================
// mrs_MediumSize
// -----------------------------------------------------------------------------
/// Return the size of a medium. That is either the size of a file or the
/// size of the allocated buffer, respectively for file media or buffer media.
///
/// @param medium Medium whose size is requested.
/// @return Size of the medium.
// =============================================================================
PROTECTED UINT32 mrs_MediumSize(MRS_MEDIUM_T* medium)
{
#ifndef WITHOUT_FS
    UINT32 oldPos;
    UINT32 pos;
#endif // WITHOUT_FS 

    MRS_ASSERT(medium, "mrs_MediumSize: medium == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumSize: medium->type == MRS_MEDIUM_TYPE_INVALID");

    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
            return medium->data.mem.size;
        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            oldPos = (UINT32)FS_Seek(medium->data.fs.fd, 0, FS_SEEK_CUR);
            pos    = (UINT32)FS_Seek(medium->data.fs.fd, 0, FS_SEEK_END);
            FS_Seek(medium->data.fs.fd, (INT64)oldPos, FS_SEEK_SET);
            return pos;
#endif // WITHOUT_FS 
        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }
    return 0;
}

PROTECTED UINT32 mrs_MediumTell(MRS_MEDIUM_T* medium)
{
    MRS_ASSERT(medium, "mrs_MediumTell: medium == NULL");
    MRS_ASSERT(medium->type != MRS_MEDIUM_TYPE_INVALID,
               "mrs_MediumTell: medium->type == MRS_MEDIUM_TYPE_INVALID");

    switch(medium->type)
    {
        case MRS_MEDIUM_TYPE_MEM:
            return medium->data.mem.position;
        case MRS_MEDIUM_TYPE_FS:
#ifndef WITHOUT_FS
            return FS_Seek(medium->data.fs.fd, 0, FS_SEEK_CUR);
#endif // WITHOUT_FS 
        case MRS_MEDIUM_TYPE_INVALID:
            break;
    }

    return 0;
}











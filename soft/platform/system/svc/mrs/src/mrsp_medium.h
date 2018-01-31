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


#ifndef _MRSP_MEDIUM_H_
#define _MRSP_MEDIUM_H_

#include "cs_types.h"
#include "mrs_m.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// MRS_MEDIUM_TYPE_T
// -----------------------------------------------------------------------------
/// List of input medium
// =============================================================================
typedef enum
{
    MRS_MEDIUM_TYPE_INVALID,
    MRS_MEDIUM_TYPE_FS,
    MRS_MEDIUM_TYPE_MEM
} MRS_MEDIUM_TYPE_T;

// =============================================================================
// MRS_MEDIUM_DATA_FS_T
// -----------------------------------------------------------------------------
/// File descriptor for filesystem medium
// =============================================================================
typedef struct
{
    INT32 fd;
} MRS_MEDIUM_DATA_FS_T ;

// =============================================================================
// MRS_MEDIUM_DATA_MEM_T
// -----------------------------------------------------------------------------
/// Buffer description and position for memory medium
// =============================================================================
typedef struct
{
    UINT8* buffer;
    UINT32 size;

    UINT32 position;
} MRS_MEDIUM_DATA_MEM_T ;

// =============================================================================
// MRS_MEDIUM_DATA_T
// -----------------------------------------------------------------------------
/// Internal medium data
// =============================================================================
typedef union
{
    MRS_MEDIUM_DATA_FS_T  fs;
    MRS_MEDIUM_DATA_MEM_T mem;
} MRS_MEDIUM_DATA_T;

// =============================================================================
// MRS_MEDIUM_T
// -----------------------------------------------------------------------------
/// Medium description
// =============================================================================
typedef struct
{
    MRS_MEDIUM_TYPE_T type;
    MRS_MEDIUM_DATA_T data;
} MRS_MEDIUM_T ;

// =============================================================================
// GLOBAL VARIABLES
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
PROTECTED INT32 mrs_MediumOpenFs(MRS_MEDIUM_T* medium, UINT8* path, MRS_FILE_MODE_T mode);


// =============================================================================
// mrs_MediumOpenBuffer
// -----------------------------------------------------------------------------
/// DON'T USE.
// =============================================================================
PROTECTED INT32 mrs_MediumOpenBuffer(MRS_MEDIUM_T* medium, UINT8* buffer, UINT32 size);


// =============================================================================
// mrs_MediumClose
// -----------------------------------------------------------------------------
/// Close a medium.
///
/// @param medium The medium to close.
/// @return 0 in case of success. -1 when the medium is improper, and couldn't
/// be closed.
// =============================================================================
PROTECTED INT32 mrs_MediumClose(MRS_MEDIUM_T* medium);

PROTECTED UINT32 mps_MediumTell(MRS_MEDIUM_T* medium);

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
PROTECTED INT32 mrs_MediumRead(MRS_MEDIUM_T* medium, VOID* buffer, UINT32 size);

PROTECTED UINT32 mps_MediumSize(MRS_MEDIUM_T* medium);

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
PROTECTED INT32 mrs_MediumWrite(MRS_MEDIUM_T* medium, VOID* buffer, UINT32 size);

// =============================================================================
// mrs_MediumSeek
// -----------------------------------------------------------------------------
///  Seek in a medium.
///
/// @param medium to seek.
/// @param offset Seek offset.
/// @param Seek whence (FS_SEEK_SET, FS_SEEK_CUR, or FS_SEEK_END)
/// @return The position in the media (positive value), or an error message from
/// the filesystem, or -1.
// =============================================================================
PROTECTED INT32 mrs_MediumSeek(MRS_MEDIUM_T* medium, INT32 offset, INT32 whence);


// =============================================================================
// mrs_MediumIsEof
// -----------------------------------------------------------------------------
/// Check the end of a medium (end of file or of buffer)
///
/// @param medium Medium to check
/// @return \c TRUE if the medium reached its end, \c FALSE otherwise.
// =============================================================================
PROTECTED BOOL mrs_MediumIsEof(MRS_MEDIUM_T* medium);

// =============================================================================
// mrs_MediumSize
// -----------------------------------------------------------------------------
/// Return the size of a medium. That is either the size of a file or the
/// size of the allocated buffer, respectively for file media or buffer media.
///
/// @param medium Medium whose size is requested.
/// @return Size of the medium.
// =============================================================================
PROTECTED UINT32 mrs_MediumSize(MRS_MEDIUM_T* medium);


#endif // _MRSP_MEDIUM_H_




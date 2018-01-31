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

#ifndef _SF_API_H_
#define _SF_API_H_

#include <stdbool.h>
#include <stdint.h>

#define SF_APPEND (1 << 0)
#define SF_TRUNC (1 << 1)
#define SF_CREAT (1 << 2)
#define SF_RDONLY (1 << 3)
#define SF_WRONLY (1 << 4)
#define SF_RDWR (SF_RDONLY | SF_WRONLY)
#define SF_DIRECT (1 << 5)
#define SF_EXCL (1 << 6)

#define SF_SEEK_SET (0)
#define SF_SEEK_CUR (1)
#define SF_SEEK_END (2)

#define SF_OK 0
#define SF_ERR_NOT_MOUNTED -10000
#define SF_ERR_FULL -10001
#define SF_ERR_NOT_FOUND -10002
#define SF_ERR_END_OF_OBJECT -10003
#define SF_ERR_DELETED -10004
#define SF_ERR_NOT_FINALIZED -10005
#define SF_ERR_NOT_INDEX -10006
#define SF_ERR_OUT_OF_FILE_DESCS -10007
#define SF_ERR_FILE_CLOSED -10008
#define SF_ERR_FILE_DELETED -10009
#define SF_ERR_BAD_DESCRIPTOR -10010
#define SF_ERR_IS_INDEX -10011
#define SF_ERR_IS_FREE -10012
#define SF_ERR_INDEX_SPAN_MISMATCH -10013
#define SF_ERR_DATA_SPAN_MISMATCH -10014
#define SF_ERR_INDEX_REF_FREE -10015
#define SF_ERR_INDEX_REF_LU -10016
#define SF_ERR_INDEX_REF_INVALID -10017
#define SF_ERR_INDEX_FREE -10018
#define SF_ERR_INDEX_LU -10019
#define SF_ERR_INDEX_INVALID -10020
#define SF_ERR_NOT_WRITABLE -10021
#define SF_ERR_NOT_READABLE -10022
#define SF_ERR_CONFLICTING_NAME -10023
#define SF_ERR_NOT_CONFIGURED -10024
#define SF_ERR_NOT_A_FS -10025
#define SF_ERR_MOUNTED -10026
#define SF_ERR_ERASE_FAIL -10027
#define SF_ERR_MAGIC_NOT_POSSIBLE -10028
#define SF_ERR_NO_DELETED_BLOCKS -10029
#define SF_ERR_FILE_EXISTS -10030
#define SF_ERR_NOT_A_FILE -10031
#define SF_ERR_RO_NOT_IMPL -10032
#define SF_ERR_RO_ABORTED_OPERATION -10033
#define SF_ERR_PROBE_TOO_FEW_BLOCKS -10034
#define SF_ERR_PROBE_NOT_A_FS -10035
#define SF_ERR_NAME_TOO_LONG -10036
#define SF_ERR_IX_MAP_UNMAPPED -10037
#define SF_ERR_IX_MAP_MAPPED -10038
#define SF_ERR_IX_MAP_BAD_RANGE -10039
#define SF_ERR_SEEK_BOUNDS -10040
#define SF_ERR_WRONG_PARAMETER -10041
#define SF_ERR_INTERNAL -10050
#define SF_ERR_TEST -10100

#define SF_OBJ_NAME_LEN 32

typedef int16_t sf_file;
typedef uint16_t sf_flags;

typedef struct sf_stat
{
    uint32_t size;
    uint8_t name[SF_OBJ_NAME_LEN];
} sf_stat_t;

typedef struct sf_config
{
    uint32_t phys_start;
    uint32_t phys_size;
    bool format_enable;
    bool check_enable;
} sf_config_t;

// =============================================================================
// Generic File System APIs
// -----------------------------------------------------------------------------
/// Generic file system APIs. Currently, they are based on the singleton spiffs.
/// Maybe they will support multiple file systems later.
///
/// * File system is multi-task safe.
/// * File handle is not multi-task safe. Especially, the offset of fle handle
///   is not. So, application should avoid to access the same file in multiple
///   tasks unless you know what you are doing.
// =============================================================================
extern int sf_init(sf_config_t *cfg);
extern void sf_destroy();
extern sf_file sf_open(const char *path, sf_flags flags);
extern int sf_close(sf_file fh);
extern int sf_create(const char *path, int mode);
extern int sf_remove(const char *path);
extern int sf_fremove(sf_file fh);
extern int sf_read(sf_file fh, void *buffer, uint32_t len);
extern int sf_write(sf_file fh, const void *data, uint32_t len);
extern int sf_rename(const char *old, const char *newph);
extern int sf_access(const char *pth, int mode);
extern int sf_stat(const char *path, sf_stat_t *s);
extern int sf_fstat(sf_file fh, sf_stat_t *s);
extern int sf_lseek(sf_file fh, int32_t offset, int whence);

// =============================================================================
// sf_rename_atomic
// -----------------------------------------------------------------------------
/// Rename a file atomicly, power-off safe.
/// * As remove, newpath will be removed rather than return error.
/// * Newpath should be prefix of oldpath (shorter, and the same with length)
// =============================================================================
extern int sf_rename_atomic(const char *old, const char *newph);

// =============================================================================
// safefile APIs
// -----------------------------------------------------------------------------
/// safefile is abstract based on sf_file. Typical use case is for configuration.
/// At power off, it is required that either old version or new version will
/// exists. And it is not permitted that both of them disappears, or the content
/// is incomplete.
///
/// Requirements:
/// * On read/write, only support read/write the whole data.
/// * On read, the buffer size shouldn't be smaller than data size.
/// * "init" must be called before any other access.
/// * Maximum file name length is one byte shorter than normal file.
///
/// safefile can be accessed in multiple task.
// =============================================================================
extern bool sf_safefile_init(const char *path);
extern bool sf_safefile_data_valid(const char *path);
extern int sf_safefile_data_size(const char *path);
extern int sf_safefile_read(const char *path, void *buffer, uint32_t len);
extern bool sf_safefile_write(const char *path, const void *data, uint32_t len);

// =============================================================================
// async
// -----------------------------------------------------------------------------
/// sf provide a low priority task, which can only receive task callback.
/// Typical use case is to send safefile update into the low priority task,
/// and can use task flush to wait all scheduled writing are done.
// =============================================================================
extern HANDLE sf_async_task_handle(void);

#endif /* _SF_API_H_ */

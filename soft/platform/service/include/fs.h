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

#ifndef __FS_H__
#define __FS_H__

#include "cs_types.h"
EXTERN_C_START

#ifndef SRVAPI
#define SRVAPI
#endif


//--------------------------------------------------------------------------------------------------
// The FS API define begin.
//-------------------------------------------------------------------------------------------------
// Open flag define

// Read only.
#define FS_O_RDONLY                0

// Write only.
#define FS_O_WRONLY         1

// Read and Write.
#define FS_O_RDWR                2

// Access.
#define FS_O_ACCMODE         3

// If the file exists, this flag has no effect except as noted under FS_O_EXCL below. Otherwise, the file shall be created.
#define FS_O_CREAT              00100

// If FS_O_CREAT and FS_O_EXCL are set, the function shall fail if the file exists.
#define FS_O_EXCL              00200

// If the file exists, and is a regular file, and the file is successfully opened FS_O_WRONLY or FS_O_RDWR, its length shall be truncated to 0.
#define FS_O_TRUNC              01000

// If set, the file offset shall be set to the end of the file prior to each write.
#define FS_O_APPEND          02000


//--------------------------------------------------------------------------------------------------
// Seek flag define.
//--------------------------------------------------------------------------------------------------
// Seek from beginning of file.
#define FS_SEEK_SET                 0

// Seek from current position.
#define FS_SEEK_CUR                 1

// Set file pointer to EOF plus "offset"
#define FS_SEEK_END                 2

//--------------------------------------------------------------------------------------------------
// File attribute define.
//--------------------------------------------------------------------------------------------------
// mark.
#define FS_ATTR_MARK       0x0

// read-only
#define FS_ATTR_RO            0x00000001

// hidden
#define FS_ATTR_HIDDEN    0x00000002

// system
#define FS_ATTR_SYSTEM    0x00000004

// volume label
#define FS_ATTR_VOLUME    0x00000008

// directory
#define FS_ATTR_DIR           0x00000010

// archived
#define FS_ATTR_ARCHIVE   0x00000020

// Check the dir entry mode if is a directory. Diretory:TRUE; File:FALSE.
#define FS_IS_DIR(m)       (((m) & FS_ATTR_DIR) == FS_ATTR_DIR)

// Check the dir entry mode if is hidden. Hidden:TRUE; Not Hidden:FALSE.
#define FS_IS_HIDDEN(m)       (((m) & FS_ATTR_HIDDEN) == FS_ATTR_HIDDEN)

//--------------------------------------------------------------------------------------------------
// Length or number define.
//--------------------------------------------------------------------------------------------------

// Folder or File name size in byts, to support fat long file name.
#define FS_FILE_NAME_LEN                     255

// Max path length,to support fat long file name.
#define FS_PATH_LEN                               260

// Max number of  open file
#define FS_NR_OPEN_FILES_MAX             32

// Max number of directory layer.
#define FS_NR_DIR_LAYERS_MAX             16

// Max folder or File name size in byts for uincode.
#define FS_FILE_NAME_UNICODE_LEN    (2*FS_FILE_NAME_LEN)

// Max path size for unicode.
#define FS_PATH_UNICODE_LEN              (2*FS_PATH_LEN)

// Length of terminated character('\0' for OEM).
#define LEN_FOR_NULL_CHAR                   1

// Size of terminated character('\0' for Unicode).
#define LEN_FOR_UNICODE_NULL_CHAR   2


//--------------------------------------------------------------------------------------------------
// File type define.
//--------------------------------------------------------------------------------------------------

// File system type -- FAT.
#define FS_TYPE_FAT                  0x01
#ifdef FS_SUPPORT_EXFAT
#define FS_TYPE_EXFAT               0x03
#endif
#define FS_TYPE_UNKNOW           0x0
//--------------------------------------------------------------------------------------------------
// Format flag.
//--------------------------------------------------------------------------------------------------

// format type -- normal(full).
#define FS_FORMAT_NORMAL        0X01

// format type -- quick.
#define FS_FORMAT_QUICK           0X00


// Data type define.

//--------------------------------------------------------------------------------------------------
// The file attribute struct.
//--------------------------------------------------------------------------------------------------
typedef struct _FS_FILE_ATTR
{
    UINT64  i_no;          // inode number
    UINT64  i_size;       // For regular files, the file size in bytes
    UINT32  i_mode;    // Mode of file
    UINT32  i_count;    // has opened count.
    UINT32  i_atime;    // Time of last access to the file
    UINT32  i_mtime;   // Time of last data modification
    UINT32  i_ctime;    // Time of last status(or inode) change
} FS_FILE_ATTR;

//--------------------------------------------------------------------------------------------------
// Storage information.
//--------------------------------------------------------------------------------------------------
typedef struct _FS_INFO
{
    UINT64 iTotalSize;    // Total size
    UINT64 iUsedSize;     // Has used  size
} FS_INFO;

//--------------------------------------------------------------------------------------------------
// Find file information.
//--------------------------------------------------------------------------------------------------
typedef struct _FS_FIND_DATA
{
    UINT32  st_mode;     // Mode of file
    UINT64  st_size;       // For regular files, the file size in bytes
    UINT32  atime;    // Time of last access to the file
    UINT32  mtime;   // Time of last data modification
    UINT32  ctime;    // Time of last status(or inode) change
    UINT8   st_name[ FS_FILE_NAME_UNICODE_LEN + LEN_FOR_UNICODE_NULL_CHAR ]; // The name of file.
} FS_FIND_DATA, *PFS_FIND_DATA;

//--------------------------------------------------------------------------------------------------
// Device type enum.
//--------------------------------------------------------------------------------------------------
typedef enum
{
    FS_DEV_TYPE_FLASH = 0,     // Flash device.
    FS_DEV_TYPE_TFLASH,         // T-Flash device.
} FS_DEV_TYPE;

//--------------------------------------------------------------------------------------------------
// Device information.
//--------------------------------------------------------------------------------------------------
typedef struct _fs_dev_info
{
    UINT8  dev_name[16];           // Device name
    FS_DEV_TYPE dev_type;        // Device type.
    BOOL is_root;                        // If is root device.
    UINT8 spare[3];                     // Spare field.
} FS_DEV_INFO;


//Function define.

INT32 SRVAPI FS_HasFormatted(
    PCSTR pszDevName,
    UINT8 iFsType
);

INT32 SRVAPI FS_Format(
    PCSTR pszDevName,
    UINT8 iFsType,
    UINT8 iFlag
);

INT32 SRVAPI FS_MountRoot(
    PCSTR pszRootDevName
);

INT32 SRVAPI FS_Mount
(
    PCSTR pszDevName,
    PCSTR pszMountPoint,
    INT32 iFlag,
    UINT8 iType
);

INT32 SRVAPI FS_Unmount(
    PCSTR pszMountPoint,
    BOOL  bForce
);

INT32 SRVAPI FS_Open(
    PCSTR pszFileName,
    UINT32 iFlag,
    UINT32 iMode
);

INT32 SRVAPI FS_Create(
    PCSTR pszFileName,
    UINT32 iMode
);

INT32 SRVAPI FS_Close(
    INT32 iFd
);

INT32 SRVAPI FS_Read(
    INT32 iFd,
    UINT8 *pBuf,
    UINT32 iLen
);

INT32 SRVAPI FS_Write(
    INT32 iFd,
    UINT8 *pBuf,
    UINT32 iLen
);

INT32 SRVAPI FS_Delete(
    PCSTR pszFileName
);

INT64 SRVAPI FS_Seek(
    INT32 iFd,
    INT64 iOffset,
    UINT8 iOrigin
);

INT32 SRVAPI FS_MakeDir(
    PCSTR pszDirName,
    UINT32 iMode
);

INT32 SRVAPI FS_RemoveDir(
    PCSTR pszDirName
);

INT32 SRVAPI FS_FindFirstFile(
    PCSTR pszFileName,
    PFS_FIND_DATA  pFindData
);

INT32 SRVAPI FS_FindNextFile(
    INT32 iFd,
    PFS_FIND_DATA pFindData
);

INT32 SRVAPI FS_FindClose(
    INT32 iFd
);

INT32 SRVAPI FS_GetFileAttr(
    PCSTR     pszFileName,
    FS_FILE_ATTR *pFileAttr
);

INT32 SRVAPI FS_SetFileAttr(
    PCSTR     pszFileName,
    UINT32    iMode
);

INT32 SRVAPI FS_GetFSInfo(
    PCSTR pszDevName,
    FS_INFO *psFSInfo
);

INT32 SRVAPI FS_IsEndOfFile(
    INT32 iFd
);

INT32 SRVAPI FS_Rename(
    PCSTR pszOldname,
    PCSTR pszNewname
);

INT32 SRVAPI FS_GetFileName(
    INT32 iFd,
    INT32 iNameBufferLen,
    UINT8 *pcFileName
);

INT32 SRVAPI FS_PowerOn(
    VOID
);

INT32 SRVAPI FS_PowerOff(
    VOID
);

INT64 SRVAPI FS_ChangeSize(
    INT32 fd,
    UINT64 nFileSize
);

INT64 SRVAPI FS_GetFileSize(
    INT32 fd
);

INT64 SRVAPI FS_GetDirSize(
       PCSTR pszFileName,
    UINT64 *pSize
);

INT32 SRVAPI FS_Flush(
    INT32 fd
);

INT32 SRVAPI FS_GetCurDir(
    UINT32 uSize,
    PSTR pCurDir
);

INT32 SRVAPI FS_ChangeDir(
    PCSTR pszDirName
);

INT32 SRVAPI FS_ReleaseDev(
    PCSTR pszDevName
);

INT64 SRVAPI FS_Truncate(
    INT32 fd,
    UINT64 nFileSize
);

INT32 SRVAPI FS_GetDeviceInfo(
    UINT32 *pDeviceCount,
    FS_DEV_INFO **ppFSDevInfo
);

INT32 SRVAPI FS_GetDeviceType(
    INT32 fd,
    FS_DEV_TYPE* pDevType
);

UINT32 SRVAPI FS_GetVersion(
    VOID
);

INT32 SRVAPI FS_Dump(
    PCSTR pszDevName,
    UINT32 iBegSec,
    UINT32 iSectCount,
    PCSTR pszFileName
);

BOOL SRVAPI FS_IsValidFileName(
    PCSTR pszFileName,
    BOOL bIgnoreBaseName
);

INT32 SRVAPI FS_ScanDisk(
    PCSTR pszDevName,
    UINT8 iFsType
);

INT32 FS_GetVolLab(
    PCSTR pszDevName,
    UINT8* szVolLab
);

INT32 FS_SetVolLab(
    PCSTR pszDevName,
    PCSTR szVolLab
);

// Error code define.
/******************************************************************************/
//The named file is a directory. The functions that use it include: FS_open();
//FS_link();FS_unlink()
#define ERR_FS_IS_DIRECTORY                              -4200001
/******************************************************************************/

/******************************************************************************/
//The named file is not a directory. The functions that use it include:
//FS_RemoveDir()
#define ERR_FS_NOT_DIRECTORY                             -4200002
/******************************************************************************/

/******************************************************************************/
//FS_mkdir(),FS_rmdir(),FS_chdir(), FS_link(), FS_unlink(),FS_readdir():
//A component of the path prefix does not name an existing directory or path is
// an empty string. FS_open():O_CREAT is not set and the named file does not ex
//ist; or O_CREAT is set and either the path prefix does not exist or the path a
//rgument points to an empty string.
#define ERR_FS_NO_DIR_ENTRY                              -4200003
/******************************************************************************/

/******************************************************************************/
// Operation is not granted.This error code Generally return when the count field of
// inode not equal the prescriptive value or the  flag field of file is FS_O_FIND.
// The function that use it inlude:FS_Rename();FS_Mount();FS_Unmount();FS_Delete();
// FS_Seek();FS_Seek();FS_IsEndOfFile();FS_Read();FS_Write();FS_RemoveDir();
// FS_ChangeSize();FS_Flush();FS_GetFileSize();FS_GetFileName().
//
#define ERR_FS_OPERATION_NOT_GRANTED                     -4200005
/******************************************************************************/

/******************************************************************************/
// Directory is not empty.
// The function that use it inlude:FS_Rename();FS_RemoveDir().
#define ERR_FS_DIR_NOT_EMPTY                             -4200006
/******************************************************************************/

/******************************************************************************/

// No file descriptor is available.
// The function that use it inlude:FS_FindFirstFile();FS_Open().
#define ERR_FS_FDS_MAX                                   -4200007
/******************************************************************************/

/******************************************************************************/
//open files too many for a process
#define ERR_FS_PROCESS_FILE_MAX                          -4200008
/******************************************************************************/

/******************************************************************************/
// File has existed.
// The function that use it inlude:FS_Rename();FS_Open();FS_MakeDir().
#define ERR_FS_FILE_EXIST                                -4200009
/******************************************************************************/

/******************************************************************************/
// The base file name(or directory name) length is zero in the path string.
// The function that use it inlude:FS_FindFirstFile();FS_Rename();FS_Open();
// FS_Delete();FS_MakeDir();FS_RemoveDir().
#define ERR_FS_NO_BASENAME                               -4200011
/******************************************************************************/

/******************************************************************************/
// Bad file descriptor or find descriptor.
// The function that use it inlude:FS_FindNextFile();FS_IsEndOfFile();
// FS_ChangSize();FS_Flush();FS_Close();FS_FindClose();FS_Seek();FS_Write();
// FS_Read();FS_GetFileName().
#define ERR_FS_BAD_FD                                    -4200012
/******************************************************************************/

/******************************************************************************/
// No more files in the specifiyed directory.
// The function that use it inlude:FS_FindFirstFile();FS_FindNextFile();
#define ERR_FS_NO_MORE_FILES                            -4200013
/******************************************************************************/

/******************************************************************************/
// The device has mounted.
// The function that use it inlude:FS_Mount().
#define ERR_FS_HAS_MOUNTED                               -4200014
/******************************************************************************/

/******************************************************************************/
// The count of mounted device more than the max mount count.
// The function that use it inlude:FS_Mount().
#define ERR_FS_MOUNTED_FS_MAX                           -4200015
/******************************************************************************/

/******************************************************************************/
// The FS type is not support.when the file type not FAT type,return this error
// code.
// The function that use it inlude:FS_HasFormated();FS_GetFsInfo();FS_Mount();
// FS_Open();FS_Delete();FS_Read();FS_Write();FS_MakeDir();FS_RemoveDir();
// FS_ScanDisk();FS_ChangeSize();FS_Flush();FS_GetFileName().
#define ERR_FS_UNKNOWN_FILESYSTEM                       -4200016
/******************************************************************************/

/******************************************************************************/
// The dirctory is invalid.when the mount point not found,this error code will
// return.
// The function that use it inlude:FS_Mount();FS_Unmount().
#define ERR_FS_INVALID_DIR_ENTRY                       -4200018
/******************************************************************************/

/******************************************************************************/
// The input parameter is invalid.
// The function that use it inlude:FS_Format();FS_FindFirstFile();FS_GetFileAttr();
// FS_GetCurDir();FS_Rename();FS_Rename();FS_MountRoot();FS_Mount();FS_Unmount();
// FS_ChangeDir();FS_Open();FS_Delete();FS_Seek();FS_Read();FS_Write();FS_MakeDir();
// FS_RemoveDir().
#define ERR_FS_INVALID_PARAMETER                       -4200019
/******************************************************************************/

/******************************************************************************/
// The FS type not support.
// The function that use it inlude:FS_Flush();FS_HasFormated().
#define ERR_FS_NOT_SUPPORT                               -4200020
/******************************************************************************/

/******************************************************************************/
#define ERR_FS_UNMOUNT_FAILED                               -4200021
/******************************************************************************/

/******************************************************************************/
// Malloc failed.
// The function that use it inlude all function.
#define ERR_FS_NO_MORE_MEMORY                           -4200025
/******************************************************************************/

/******************************************************************************/
// The device is unregister.
// The function that use it inlude:FS_HasFormated();FS_Format();FS_GetFSInfo();
// FS_MountRoot();FS_Mount();FS_ScanDisk().
#define ERR_FS_DEVICE_NOT_REGISTER                        -4200027

/******************************************************************************/
// The disk is full,no more space to use.
// The function that use it include: FS_Write();FS_Create();FS_Open();
// FS_MakeDir();FS_ChangeSize().
#define ERR_FS_DISK_FULL                               -4200030
/******************************************************************************/

/******************************************************************************/
// The device is not format.
// The function that use it include: FS_HasFormated().
#define ERR_FS_NOT_FORMAT                               -4200032
/******************************************************************************/

/******************************************************************************/
// The device has formated.
// The function that use it include: FS_HasFormated().
#define ERR_FS_HAS_FORMATED                               -4200033
/******************************************************************************/

/******************************************************************************/
// Can't find the supper block.
// The function that use it include: FS_Unmount().
#define ERR_FS_NOT_FIND_SB                        -4200035
/******************************************************************************/

/******************************************************************************/
// The device is busy,can't unmount.
// The function that use it include: FS_Unmount().
#define ERR_FS_DEVICE_BUSY                                           -4200037
/******************************************************************************/

/******************************************************************************/
// The device is wrong,can't open.
// The function that use it include: FS_HasFormated();FS_Format();FS_Unmount();
// FS_Mount();FS_ScanDisk().
#define ERR_FS_OPEN_DEV_FAILED                                   -4200038
/******************************************************************************/

/******************************************************************************/
// The count more than the max count of root entry count,that dir entry in the
// root dirctory.
// The function that use it include: FS_Open();FS_Create();FS_MakeDir();FS_Rename().
#define ERR_FS_ROOT_FULL                                  -4200039
/******************************************************************************/

/******************************************************************************/
// Access the reg failed.
// The function that use it include: FS_PowerOn();FS_PowerOff().
#define ERR_FS_ACCESS_REG_FAILED                          -4200040
/******************************************************************************/

/******************************************************************************/
// Parse the path name error.
// The function that use it include: FS_FindFirstFile();FS_GetFileAttr();
// FS_GetCurDir();FS_Rename();FS_ChangeDir();FS_Open();FS_Delete();FS_MakeDir();
// FS_RemoveDir().
#define ERR_FS_PATHNAME_PARSE_FAILED                    -4200041
/******************************************************************************/

/******************************************************************************/
// Get the dir entry failed in the specifiy dirctory.
// The function that use it include: FS_FindFirstFile().
#define ERR_FS_READ_DIR_FAILED                          -4200048
/******************************************************************************/

/******************************************************************************/
// Get root inode failed.
// The function that use it include: FS_MountRoot();FS_Mount().
#define ERR_FS_MOUNT_READ_ROOT_INODE_FAILED                -4200050
/******************************************************************************/

/******************************************************************************/
// The device number is invalid.
// The function that use it include: FS_MountRoot().
#define ERR_FS_INVALID_DEV_NUMBER               -4200051
/******************************************************************************/

/******************************************************************************/
// Move directory is not permitted
// The function that use it include: FS_Rename().
#define ERR_FS_RENAME_DIFF_PATH               -4200052
/******************************************************************************/

/******************************************************************************/
// The device is mounted,when format option is begin.
// The function that use it include: FS_Format().
#define ERR_FS_FORMAT_MOUNTING_DEVICE                   -4200053
/******************************************************************************/


/******************************************************************************/
// The boot information is destroy on the specifiy device.
// The function that use it include: FS_Format() FS_HasFormated();FS_Mount();
// FS_MountRoot().
#define ERR_FS_DATA_DESTROY                                -4200056
/******************************************************************************/

/******************************************************************************/
// Read the FAT entry failed.
// The function that use it include: FS_Open(); FS_Create();FS_MakeDir();
// FS_Write(); FS_Read();FS_ChangeSize();FS_RemoveDir();FS_Delete();FS_Rename();
// FS_ScanDisk();FS_HasFormate();FS_MountRoot().
#define ERR_FS_READ_SECTOR_FAILED                          -4200057
/******************************************************************************/


/******************************************************************************/
// Write the FAT entry failed.
// The function that use it include: FS_Open(); FS_Create();FS_MakeDir();
// FS_Write(); FS_ChangeSize();FS_RemoveDir();FS_Delete();FS_Rename();
// FS_ScanDisk();.
#define ERR_FS_WRITE_SECTOR_FAILED                         -4200058
/******************************************************************************/

/******************************************************************************/
// Read the file exceed the end of file.
// The function that use it include: FS_Read();
#define ERR_FS_READ_FILE_EXCEED                            -4200059
/******************************************************************************/

/******************************************************************************/
// The pos exceed the end of file when write a file.
// The function that use it include: FS_Write();
#define ERR_FS_WRITE_FILE_EXCEED                           -4200060
/******************************************************************************/

/******************************************************************************/
// The sample long name too more in a dirctory,so can't make a short file as
// "fo~xxx ext".the max count is 999 in a dirctory.
// The function that use it include: FS_MakeDir();FS_Create();FS_Open().
#define ERR_FS_FILE_TOO_MORE                               -4200061
/******************************************************************************/

/******************************************************************************/
// The file not exist.
// The function that use it include: FS_Open();FS_Delete();FS_GetFileAttr();
// FS_Rename().
#define ERR_FS_FILE_NOT_EXIST                              -4200062
/******************************************************************************/

/******************************************************************************/
// Try to rename a file to another device.
// The function that use it include: FS_Rename().
#define ERR_FS_DEVICE_DIFF                                 -4200063
/******************************************************************************/

/******************************************************************************/
// Get the device information  failed.
// The function that use it include: FS_Format().
#define ERR_FS_GET_DEV_INFO_FAILED                         -4200064
/******************************************************************************/

/******************************************************************************/
// No more supper block item.
// The function that use it include: FS_MountRoot();FS_Mount().
#define ERR_FS_NO_MORE_SB_ITEM                             -4200065
/******************************************************************************/


/******************************************************************************/
// The device not mount.
// The function that use it include:FS_GetFSInfo().
#define ERR_FS_NOT_MOUNT                                   -4200066
/******************************************************************************/

/******************************************************************************/
// The buffer too short to save a file name.
// The function that use it include:FS_GetCurDir();FS_GetFileName().
#define ERR_FS_NAME_BUFFER_TOO_SHORT                       -4200068
/******************************************************************************/


/******************************************************************************/
// The inode not reguler.
// The function that use it include:FS_Read();FS_Write();FS_GetFileSize().
#define ERR_FS_NOT_REGULAR                                  -42000100
/******************************************************************************/

/******************************************************************************/
// The inode not reguler.
// The function that use it include:FS_Read();FS_Write();FS_GetFileSize().
#define ERR_FS_VOLLAB_IS_NULL                             -42000101
/******************************************************************************/


// The fs-api define end.

EXTERN_C_END

#endif // _H_



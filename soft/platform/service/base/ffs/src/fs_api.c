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

#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "fat_base.h"
#include "exfat_base.h"
#include "fat_local.h"
#include "exfat_local.h"
#include "fs.h"
#include "fs_base.h"

#if defined(_FS_SIMULATOR_)
#define FS_SemInit()
#define FS_Down()
#define FS_Up()
#else
#define FS_Down()   sxr_TakeSemaphore(g_fs_sem)
#define FS_Up()     sxr_ReleaseSemaphore( g_fs_sem )
#endif

#define FS_INVALID_SEM_HANDLE    0xff
// FS API semarphore.
HANDLE g_fs_sem = FS_INVALID_SEM_HANDLE;

extern struct super_block *g_fs_super_block_table;
extern struct task_struct g_fs_current;
extern struct file *g_fs_file_list;
extern FS_DEV_INFO *g_pFsDevInfo;

/********************************************************************************/
// Function:
//     Detect the device if is formated.
// Parameter:
//     pszDevName[in]:  the device name.
//     iFsType[in]: the file system type.
// Return value:
//     if is formated return ERR_FS_HAS_FORMATED,else return ERR_FS_NOT_FORMAT;
//*******************************************************************************
INT32 FS_HasFormatted(PCSTR pszDevName, UINT8 iFsType)
{
    UINT32 uDevNo = 0;
    // BOOL bIsMMC = FALSE;
    INT32 iResult;
    INT32 iRet;

    if ( !pszDevName  || FS_TYPE_FAT != iFsType)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    CSW_TRACE(BASE_DSM_TS_ID,"FS_HasFormatted pszDevName =%s",pszDevName);
    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in fs_hasformatted DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in fs_hasformatted DevNo is invalid,uDevNo = 0x%x.\n", uDevNo ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        // bIsMMC = TRUE;
        iRet = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iRet )
        {
            D( ( DL_FATERROR, "check formatted or not[%s]: Open t-flash memory device failed.\n", pszDevName ) );
            return ERR_FS_OPEN_DEV_FAILED;
        }
    }
    FS_Down();
    iResult = fs_has_formated(uDevNo);
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    iRet = fs_set_error_code(iResult);
    return iRet;
}

//********************************************************************************
// Function:
//   This function is format the specified partitition.
// Parameter:
//   pszDevType[in]: the name of device.
//   iFsType[in]: the type of file system.
//   iFlag[in]: The option of format, 0:Quickly;1:Normal.
// Return value:
//      ERR_SUCCESS indicates a successful operation.
//         And unsuccessful operation is denoted by the err code as following.
//      ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************

INT32 FS_Format(PCSTR pszDevName, UINT8 iFsType, UINT8 iFlag)
{
    UINT32 uDevNo = 0;
    INT32 iRet;

    if (NULL == pszDevName
            || ((FS_TYPE_FAT != iFsType)
#ifdef FS_SUPPORT_EXFAT
                && (FS_TYPE_EXFAT != iFsType)
#endif
               ))
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    iRet = ERR_SUCCESS;
    D( ( DL_FATDETAIL, "FS_Format devname = %s, FsType = %d, Flag = %d.\n",
         pszDevName, iFsType,iFlag ));

    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_Format DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in FS_Format DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
        iRet = ERR_FS_DEVICE_NOT_REGISTER;
        return iRet;
    }

    //  Allow only format the partiton for FS module .
    if ( NULL != pszDevName &&
            DSM_MODULE_FS_ROOT != DSM_GetDevModuleId( uDevNo) &&
            DSM_MODULE_FS != DSM_GetDevModuleId( uDevNo)
       )
    {
        D( ( DL_FATERROR, "Device[%s] is not permitted to access for FS",pszDevName) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        return iRet;
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        // bIsMMC = TRUE;
        iRet = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iRet )
        {
            D( ( DL_FATERROR, "Format device[%s]: Open t-flash device fail.\n", pszDevName ) );
            iRet = ERR_FS_OPEN_DEV_FAILED;
            return iRet;
        }
    }

    FS_Down();
    iRet = fs_format(uDevNo,iFsType,iFlag);
    if( ERR_SUCCESS != iRet )
    {
        D(( DL_FATERROR, "Format device[%s]: fs_format fail,iRet = %d.\n", pszDevName,iRet));
        iRet = ERR_FS_OPEN_DEV_FAILED;
        FS_Up();
        return iRet;
    }
    FS_Up();
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {

        iRet = DRV_DEV_FLUSH(uDevNo);
        if(ERR_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, " FS_Format flush cache error.  return error = 0x%x.",iRet) );
        }

        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    return iRet;
}

/*
We custom the structure file{} for FS_Find*** APIs:
1. Use f_pos field to save the next dir-entry to be found next time
2. Use f_mode and  f_flags fields to save the pointer to the pattern for FindNextFile().
    The memory will be freed at FindClose().
*/
INT32 FS_FindFirstFile( PCSTR pszFileName, PFS_FIND_DATA  pFindData )
{
    WCHAR *path = NULL;
    WCHAR *pattern = NULL;
    INT32 patternlen;
    struct inode *dir_i;
    struct file *f = NULL;
    struct dentry *psDe = NULL;
    INT32 fd;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;

    if(NULL == pFindData) // invalid pathname or pFindData is NULL
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, parameter error" ) );
        return  ERR_FS_INVALID_PARAMETER;
    }

    iULen = DSM_UnicodeLen((UINT8 *)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    // path name character validate, Ignore base name.
    if( !is_file_name_valid((PCWSTR) pszFileName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    for ( fd = 0; fd < (INT32)g_fs_cfg.uNrOpenFileMax; fd++ )
    {
        if ( NULL == g_fs_current.filp[ fd ] )                 /*unused fd found*/
        {
            break;
        }
    }
    if((INT32)g_fs_cfg.uNrOpenFileMax == fd) // No fd is available
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, fd overflow, fd = %d", fd ) );
        iRet = ERR_FS_FDS_MAX;
        goto step0_failed;
    }

    f = fs_alloc_fd();
    if (NULL == f)
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, fs_alloc_fd() return NULL") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }
    f = fs_fd_list_add(&g_fs_file_list, f);

    path = format_dir_name((PCWSTR)pszFileName );
    if(NULL == path) // malloc failed
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, path is NULL" ) );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }

    dir_i = dir_namei( path, &patternlen, (CONST WCHAR **)&pattern );
    // path is invalid
    if(NULL == dir_i )
    {
        D((DL_FATERROR, "++++ Call dir_namei() return NULL! ++++\n"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    // patternlen is 0
    if(0 == patternlen)
    {
        D((DL_FATERROR, "++++ Call dir_namei() return. ERR_FS_NO_BASENAME! ++++\n"));
        iRet = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "In FS_FindFirstFile malloc dentry failed.\n") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }

    // Copying  string of filename overflow impossibility,because has checked the file path befor.
    DSM_MemSet( psDe, '\0', sizeof(struct dentry) );
    DSM_TcStrCpy( psDe->name, pattern);
    psDe->append_data2 = FS_FLAG_FIND_FILE;
    iRet = fs_find_entry(&dir_i,psDe,TRUE);
    if (0 != iRet )
    {
        D((DL_FATWARNING, "+++ find_entry() return failed\n"));
        iRet = ERR_FS_NO_MORE_FILES;
        goto step1_failed;
    }

    // Modify by suwg at 2007/7/6
    f->f_pattern = DSM_MAlloc((DSM_UnicodeLen((UINT8 *)pattern) + LEN_FOR_NULL_CHAR * SIZEOF(WCHAR)));
    if(NULL == f->f_pattern)
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, f_pattern is NULL" ) );
        DSM_ASSERT(0, "FS_FindFirstFile: 1.malloc(0x%x) failed.", (DSM_UnicodeLen((UINT8 *)pattern) + LEN_FOR_NULL_CHAR * SIZEOF(WCHAR)));
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step2_failed;
    }

    g_fs_current.filp[ fd ] = f;
    f->f_count = 1;
    f->f_inode = dir_i;
    f->f_flags = FS_O_FIND;
    f->f_pos = psDe->append_data;

    DSM_TcStrCpy( f->f_pattern, pattern );

    pFindData->st_mode = psDe->inode->i_mode;
    pFindData->st_size = psDe->inode->i_size;
    pFindData->atime = psDe->inode->i_atime;
    pFindData->mtime = psDe->inode->i_mtime;
    pFindData->ctime = psDe->inode->i_ctime;

    iULen = DSM_UnicodeLen((UINT8 *)psDe->name);
    if(iULen > FS_PATH_UNICODE_LEN)
    {
        iRet =  ERR_FS_READ_DIR_FAILED;
        goto step2_failed;
    }

    DSM_TcStrCpy((PCWSTR)pFindData->st_name, psDe->name);
    iRet = ERR_SUCCESS;
    CSW_TRACE(BASE_DSM_TS_ID,
              "#fs debug find first  success. fd= %d,DevNo = %d.",
              fd, f->f_inode->i_dev);
step2_failed:
    iput( psDe->inode );

step1_failed:

    //if failuer, free the inode,else free it in FS_FindClose.

    if ( ERR_SUCCESS != iRet )
    {
        iput( dir_i );
    }

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

step0_failed:

    if (iRet != ERR_SUCCESS && f != NULL)
    {
        if(f->f_pattern != NULL)
        {
            DSM_Free(f->f_pattern);
        }
        fs_fd_list_del(&g_fs_file_list, f);
    }

    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }

    FS_Up();

    return ( ERR_SUCCESS == iRet ) ? fd : iRet;
}


INT32 FS_FindNextFile( INT32 fd, PFS_FIND_DATA pFindData)
{
    struct file *f;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet;
    UINT16 iULen = 0;

    if(NULL == pFindData)
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, parameter error, pFindData is NULL" ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    if(!FD_IS_VALID( fd ))
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, parameter error, fd is invalid,fd = %d", fd ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    f = g_fs_current.filp[fd];
    if(!f || !fs_is_file_opened(&g_fs_file_list, f))
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, f is NULL" ) );
        return ERR_FS_BAD_FD;
    }

    // handle is not for finding
    if(0 == (f->f_flags & FS_O_FIND))
    {
        D( ( DL_FATERROR, "in FS_FindNextFile, handle is not for finding, f->flag = %d", f->f_flags) );
        return ERR_FS_BAD_FD;
    }
    if ( 0 == f->f_pos )
    {
        return ERR_FS_NO_MORE_FILES;
    }

    FS_Down();

    dir_i = f->f_inode;
    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        FS_Up();
        return ERR_FS_NO_MORE_MEMORY;
    }
    DSM_MemSet( psDe, '\0', sizeof(struct dentry) );
    DSM_TcStrCpy( psDe->name, f->f_pattern);

    psDe->append_data2 = FS_FLAG_FIND_FILE;
    psDe->append_data = f->f_pos;
    iRet = fs_find_entry(&dir_i,psDe,FALSE);
    if(0 == iRet)
    {
        f->f_pos = psDe->append_data;
        // directory or regular file
        pFindData->st_mode = psDe->inode->i_mode;
        pFindData->st_size = psDe->inode->i_size;
        pFindData->atime = psDe->inode->i_atime;
        pFindData->mtime = psDe->inode->i_mtime;
        pFindData->ctime = psDe->inode->i_ctime;
        // NameFat2User(psDe.inode->u.fat_i.entry.DIR_Name, pFindData->st_name,0x0);
        // DSM_StrCpy(pFindData->st_name, psDe.name);
        iULen = DSM_UnicodeLen((UINT8 *)psDe->name);
        if(iULen > FS_FILE_NAME_UNICODE_LEN)
        {
            DENTRY_FREE(psDe);
            FS_Up();
            return ERR_FS_READ_DIR_FAILED;
        }
        DSM_TcStrCpy((PCWSTR)pFindData->st_name, psDe->name);
        iput( psDe->inode );
        iRet = ERR_SUCCESS;
    }
    else
    {
        iRet = ERR_FS_NO_MORE_FILES;
    }

    DENTRY_FREE(psDe);
    FS_Up();
    return iRet;
}


INT32 FS_FindClose(INT32 fd)
{
    INT32 iRet;
    struct file *filp;
    UINT32 uDevNo = 0;

    if( !FD_IS_VALID( fd ) ) // fd is invalid
    {
        //  DSM_ASSERT(0,"find_close error1. fd = 0x%x",fd);
        return ERR_FS_BAD_FD;
    }
    filp = g_fs_current.filp[ fd ];
    if(!filp)
    {
        //   DSM_ASSERT(0,"find_close error2. fd = 0x%x",fd);
        return ERR_FS_BAD_FD;
    }
    uDevNo = filp->f_inode->i_dev;

    FS_Down();

    iRet = fs_findclose(fd);
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }

    }
    if(iRet == 0)
    {
        CSW_TRACE(BASE_DSM_TS_ID,
                  "#fs debug find close success. fd= %d,uDevNo = %d.",
                  fd, uDevNo);
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID,
                  "#fs debug find close failed. fd= %d,uDevNo = %d,ret_value = %d.",
                  fd, uDevNo, iRet);
        // DSM_ASSERT(0,"find_close error3. fd = 0x%x",fd);
    }
    FS_Up();

    return iRet;
}

//---------------------------------------------------------------------------------------------------------------------
// FS_GetFileAttr
// This funciton to get the file or directory attribute .
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName[in]
// File name,the string format is unicode.
// @pFileAttr[out]
// Output the attributes of the specified file or director.
// Return
// Suceeds return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_FILE_NOT_EXIST:The file is not exist.
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_GetFileAttr( PCSTR pszFileName, FS_FILE_ATTR *pFileAttr)
{
    BOOL bResult;
    INT32 iRet = ERR_SUCCESS;
    struct inode *inode = NULL;
    UINT16 iULen = 0;
    UINT32 uDevNo =  INVALID_DEVICE_NUMBER;

    if( !pszFileName || !pFileAttr ) // hFile is invalid
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    iULen = DSM_UnicodeLen((UINT8 *)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    bResult = is_file_name_valid( (PCWSTR)pszFileName, TRUE );
    if(!bResult)
    {
        D((DL_FATWARNING, "Invalid file name"));
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();
    inode = namei((PCWSTR) pszFileName );
    if(!inode)
    {
        D( ( DL_FATERROR, "directory entry does not exist" ) );
        iRet = ERR_FS_FILE_NOT_EXIST;
    }
    else
    {
        pFileAttr->i_no = inode->i_ino;
        pFileAttr->i_mode = inode->i_mode;
        pFileAttr->i_size = inode->i_size;
        pFileAttr->i_count = (inode->i_count - 1);
        pFileAttr->i_atime = inode->i_atime;
        pFileAttr->i_mtime = inode->i_mtime;
        pFileAttr->i_ctime = inode->i_ctime;
        uDevNo =  inode->i_dev;
        iput( inode );
    }
    if(uDevNo != INVALID_DEVICE_NUMBER)
    {
        if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
        {
            if(!fs_is_opened(uDevNo))
            {
                DRV_DEV_DEACTIVE(uDevNo);
            }
        }
    }
    FS_Up();
    return iRet;
}



//---------------------------------------------------------------------------------------------------------------------
// FS_SetFileAttr
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName
// File name,the string format is unicode.
// @iMode
// File or directory attibute. The following values can be combined by using the bitwise OR operator.
// FS_ATTR_RO:read-only
// FS_ATTR_HIDDEN:hidden
// FS_ATTR_SYSTEM:system
// FS_ATTR_ARCHIVE:archived
// The macro FS_IS_DIR(m) be used to check the dir entry mode if is a directory.Diretory:TRUE;File:FALSE.
// Return
// Sucesse return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_NO_MORE_MEMORY:No more memory to alloc.
// ERR_FS_PATHNAME_PARSE_FAILED:Parse the path failed.
// ERR_FS_NO_BASENAME:No base file name.
// ERR_FS_FILE_NOT_EXIST:The file is not exist.
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_SetFileAttr(PCSTR pszFileName, UINT32 iMode)
{
    WCHAR *path = NULL;
    WCHAR *pattern = NULL;
    INT32 patternlen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;

    iULen = DSM_UnicodeLen((UINT8 *)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    // path name character validate, Ignore base name.
    if( !is_file_name_valid((PCWSTR) pszFileName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    path = format_dir_name((PCWSTR)pszFileName );
    if(NULL == path) // malloc failed
    {
        D( ( DL_FATERROR, "in FS_FindFirstFile, path is NULL" ) );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }

    dir_i = dir_namei( path, &patternlen, (CONST WCHAR **)&pattern );
    if(NULL == dir_i ) // path is invalid
    {
        D((DL_FATERROR, "++++ Call dir_namei() return NULL! ++++\n"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    if(0 == patternlen) // patternlen is 0
    {
        D((DL_FATERROR, "++++ Call dir_namei() return. ERR_FS_NO_BASENAME! ++++\n"));
        iRet = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    uDevNo = dir_i->i_dev;
    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "In FS_FindFirstFile malloc dentry failed.\n") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }

    DSM_MemSet( psDe, '\0', sizeof(struct dentry) );
    DSM_TcStrCpy( psDe->name, pattern);
    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    iRet = fs_find_entry(&dir_i, psDe,FALSE);
    if (0 != iRet )
    {
        D((DL_FATWARNING, "+++ do_find_entry() return failed\n"));
        iRet = ERR_FS_FILE_NOT_EXIST;
        goto step1_failed;
    }

    psDe->inode->i_mode |= ((iMode & FS_ATTR_RO)  == FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
    psDe->inode->i_mode |= ((iMode & FS_ATTR_HIDDEN)  == FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
    psDe->inode->i_mode |= ((iMode & FS_ATTR_SYSTEM)  == FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
    psDe->inode->i_mode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;

    if(psDe->inode->u.fat_i.entry.DIR_Attr != psDe->inode->i_mode)
    {
        psDe->inode->u.fat_i.entry.DIR_Attr = (UINT8)psDe->inode->i_mode;
        psDe->inode->u.fat_i.Dirty = TRUE;
    }

    iput( psDe->inode );
    iRet = ERR_SUCCESS;

step1_failed:

    iput( dir_i );
    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;
}


INT32 FS_GetFSInfo(PCSTR pszDevName, FS_INFO *psFSInfo)
{
    UINT32 uDevNo = 0;
    UINT32 iFatEntry;
    UINT64 iSizePerClus = 0;
    UINT64 iFreeSize = 0;
    struct super_block* pSB = NULL;
    struct fat_sb_info*  fat_sb_info;
#ifdef FS_SUPPORT_EXFAT
    struct exfat_sb_info*  exfat_sb_info;
#endif
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    FS_Down();
    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        FS_Up();
        return ERR_FS_DEVICE_NOT_REGISTER;
    }
    iResult = fs_find_sb(uDevNo,&pSB);
    if(_ERR_FS_SUCCESS == iResult)
    {
        switch(pSB->s_fstype)
        {
            case FS_TYPE_FAT:
                fat_sb_info = &pSB->u.fat_sb;
                iFreeSize = 0;
                iFatEntry = 0;
                // iOldFatEntry = 0;
                iSizePerClus = (UINT64)(fat_sb_info->iBytesPerSec) * (UINT64)(fat_sb_info->iSecPerClus);
                psFSInfo->iTotalSize = ((UINT64)((fat_sb_info->iDataSec)/(fat_sb_info->iSecPerClus)))*iSizePerClus;




                iResult = fat_stat_free_cluster(fat_sb_info, &iFatEntry);
                if(ERR_SUCCESS != iResult)
                {
                    iRet = fs_set_error_code(iResult);
                }
                iFreeSize = (UINT64)iFatEntry*iSizePerClus;
                psFSInfo->iUsedSize = (psFSInfo->iTotalSize - iFreeSize);
                break;
#ifdef FS_SUPPORT_EXFAT
            case FS_TYPE_EXFAT:
                exfat_sb_info = &pSB->u.exfat_sb;
                iFreeSize = 0;
                iFatEntry = 0;
                iSizePerClus = (UINT64)(exfat_sb_info->BytesPerSector) * (UINT64)(exfat_sb_info->SectorsPerCluster);
                psFSInfo->iTotalSize = (UINT64)((UINT64)iSizePerClus*(UINT64)exfat_sb_info->ClusterCount);
                iResult = exfat_stat_free_cluster(exfat_sb_info, &iFatEntry);
                if(ERR_SUCCESS != iResult)
                {
                    iRet = fs_set_error_code(iResult);
                }
                iFreeSize = (UINT64)iFatEntry * (UINT64)iSizePerClus;
                psFSInfo->iUsedSize = (psFSInfo->iTotalSize - iFreeSize);
                break;
#endif
            default:
                iRet = ERR_FS_UNKNOWN_FILESYSTEM;
                break;
        }
    }
    else
    {
        iRet = fs_set_error_code(iResult);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;
}


INT32 FS_GetCurDir(UINT32 uSize, PSTR pCurDir)
{
    INT32 iRet = ERR_SUCCESS;
    INT16 uLen;

    FS_Down();
    if (NULL == pCurDir)
    {
        D((DL_FATERROR, "FS_GetCurDir: pCurDir is null"));
        iRet = ERR_FS_INVALID_PARAMETER;
        goto _get_cur_dir_end;
    }

    if (NULL == g_fs_current.wdPath)
    {
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto _get_cur_dir_end;
    }

    uLen = DSM_UnicodeLen((UINT8*)g_fs_current.wdPath);
    if ((UINT32)uLen + LEN_FOR_NULL_CHAR > uSize / SIZEOF(WCHAR))
    {
        D((DL_FATERROR, "FS_GetCurDir: uLen(%d), uSize(%d), ERR_DSS_FS_NAME_BUFFER_TOO_SHORT", uLen, uSize));
        iRet = ERR_FS_NAME_BUFFER_TOO_SHORT;
        goto _get_cur_dir_end;
    }

    DSM_TcStrCpy((PCWSTR)pCurDir, g_fs_current.wdPath);
    iRet = ERR_SUCCESS;

_get_cur_dir_end:

    FS_Up();
    return iRet;
}

INT32 FS_Rename( PCSTR pszOldname, PCSTR pszNewname )
{
    BOOL bRet;
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    WCHAR *basename;
    INT32 namelen;
    struct inode *dir_old, *dir_new;
    struct dentry *psOld_de = NULL, *psDe = NULL;
    INT16 iULen = 0;
    UINT32 uDevNo = 0;

    if (NULL == pszOldname || NULL ==pszNewname)
    {
        DSM_ASSERT(0,"rename error1. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }

    iULen = DSM_UnicodeLen((UINT8*)pszOldname);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        DSM_ASSERT(0,"rename error2. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }

    bRet = is_file_name_valid((PCWSTR) pszOldname, FALSE );
    if(!bRet)
    {
        D((DL_FATWARNING, "invalid old pathname"));
        // DSM_ASSERT(0,"rename error3. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }

    iULen = DSM_UnicodeLen((UINT8*)pszNewname);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    bRet = is_file_name_valid( (PCWSTR)pszNewname, FALSE );
    if(!bRet)
    {
        D((DL_FATWARNING, "invalid new pathname"));
        //  DSM_ASSERT(0,"rename error4. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    dir_old = dir_namei((PCWSTR) pszOldname, &namelen, (CONST WCHAR**)&basename );
    if(!dir_old)
    {
        D((DL_FATWARNING, "old path is invalid"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        //DSM_ASSERT(0,"rename error5. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step0_failed;
    }
    if(!namelen)
    {
        D((DL_FATWARNING, "file name not specified"));
        iRet = ERR_FS_NO_BASENAME;
        // DSM_ASSERT(0,"rename error6. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step1_failed;
    }

    uDevNo = dir_old->i_dev;

    psOld_de = DENTRY_ALLOC();
    if (NULL == psOld_de)
    {
        D((DL_FATERROR, "FS_Rename: DENTRY_ALLOC psOld_de is NULL.\n"));
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"rename error7. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step1_failed;
    }

    DSM_MemSet( psOld_de, 0, sizeof( *psOld_de ) );
    DSM_TcStrCpy(psOld_de->name, basename);

    psOld_de->append_data2 = FS_FLAG_NOT_FIND_FILE;
    iResult = fs_find_entry( &dir_old, psOld_de, FALSE);
    if(-1 == iResult)
    {
        D((DL_FATWARNING, "file not found"));
        iRet = ERR_FS_NO_DIR_ENTRY;
        // DSM_ASSERT(0,"rename error8. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step1_failed;
    }

    // Some process is dealing with the file. It will not be removed.
    if( psOld_de->inode->i_count > 1)
    {
        D((DL_FATWARNING, "Some process is dealing with the file. It will not be renamed."));
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        // DSM_ASSERT(0,"rename error9. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step2_failed;
    }

    dir_new = dir_namei((PCWSTR) pszNewname, &namelen, (CONST WCHAR**)&basename );
    if(!dir_new)
    {
        D((DL_FATWARNING, "old path is invalid"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        // DSM_ASSERT(0,"rename error10. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step2_failed;
    }
    if(!namelen)
    {
        D((DL_FATWARNING, "file name not specified"));
        iRet = ERR_FS_NO_BASENAME;
        //DSM_ASSERT(0,"rename error11. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    if ( FS_IS_DIR( psOld_de->inode->i_mode ) && dir_new != dir_old )
    {
        D((DL_FATWARNING, "Move directory is not permitted"));
        iRet = ERR_FS_RENAME_DIFF_PATH;
        DSM_ASSERT(0,"rename error12. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D((DL_FATERROR, "FS_Rename: DENTRY_ALLOC psDe is NULL.\n"));
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"rename error13. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename);
    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    iResult = fs_find_entry( &dir_new, psDe, FALSE);
    if(!iResult)
    {
        D((DL_FATWARNING, "file has existed"));
        iput( psDe->inode );
        iRet = ERR_FS_FILE_EXIST;
        //DSM_ASSERT(0,"rename error14. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    iResult = fs_do_link( psOld_de, dir_new, psDe,psOld_de->inode->i_mode  );
    if(iResult)
    {
        D((DL_FATWARNING, "do link failed"));
        iRet = fs_set_error_code(iResult);
        //DSM_ASSERT(0,"rename error15. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    iResult = fs_do_unlink(dir_old, psOld_de, TRUE );
    if(iResult)
    {
        D((DL_FATWARNING, "do unlink failed"));
        iRet = fs_set_error_code(iResult);
        //DSM_ASSERT(0,"rename error16. pszOldname= 0x%x,pszNewname = 0x%x.",pszOldname,pszNewname);
        goto step3_failed;
    }

    iRet = ERR_SUCCESS;

step3_failed:
    iput( dir_new );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

step2_failed:
    iput( psOld_de->inode );
step1_failed:
    iput( dir_old );

    if (psOld_de != NULL)
    {
        DENTRY_FREE(psOld_de);
    }

step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;
}




/* NO semaphore protection */
INT32 FS_MountRoot( PCSTR pszRootDevName )
{
    INT32 iResult = ERR_SUCCESS;
    INT32 iRet = ERR_SUCCESS;
    UINT32 uDevNo = 0;
    struct super_block * sb = NULL;
    struct inode *root_i;
    UINT8 szDevName[16] = {0,};

    if ( g_fs_current.root )
    {
        D( ( DL_FATWARNING, "Root file system has mounted" ) );
        return ERR_FS_HAS_MOUNTED;
    }

    g_fs_current.pwd = NULL;
    g_fs_current.root = NULL;
    DSM_MemSet( g_fs_current.filp, 0x00, (sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax);

    if(NULL == pszRootDevName)
    {
        iResult = DSM_GetFsRootDevName(szDevName);
        if(iResult != ERR_SUCCESS)
        {
            DSM_ASSERT(0,"FS_MountRoot1:iRet = %d",iResult);
            return ERR_FS_DEVICE_NOT_REGISTER;
        }
        uDevNo = DSM_DevName2DevNo( szDevName );
    }
    else
    {
        uDevNo = DSM_DevName2DevNo(pszRootDevName );
    }

    //uDevNo = DRV_DevName2DevNr( pszRootDevName ? pszRootDevName : szDevName );
    if (INVALID_DEVICE_NUMBER == uDevNo)
    {
        DSM_ASSERT(0,"FS_MountRoot2:Can't find the divie.szDevName = %s",szDevName);
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    if (DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH  )
    {
        iResult = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iResult )
        {
            D( ( DL_FATERROR, "Mount root filesystem: Open t-flash memory device failed" ) );
            DSM_ASSERT(0,"FS_MountRoot3:iRet = %d",iResult);
            return ERR_FS_OPEN_DEV_FAILED;
        }
    }
    iResult = fs_find_sb(uDevNo, &sb);
    if(iResult == ERR_SUCCESS)
    {
        D((DL_FATERROR, "Mount root filesystem: the device is mounted. uDevNo = %d\n",uDevNo));
        return ERR_FS_HAS_MOUNTED;
    }

    iResult = fs_get_free_sb(&sb);
    if( ERR_SUCCESS != iResult )
    {
        D( ( DL_FATERROR, "Mount root filesystem: fs_get_free_sb failed." ) );
        iRet = ERR_FS_NO_MORE_SB_ITEM;
        return iRet;
    }

    /* We ASSUME that root filesystem is FAT */
    iResult = fs_read_super(sb, uDevNo);
    if( ERR_SUCCESS != iResult )
    {
        D( ( DL_FATERROR, "Mount root filesystem: failed to read superblock" ) );
        // DSM_ASSERT(0,"FS_MountRoot4:iRet = %d",iRet);
        iRet = iResult;
        goto step0_failed;
    }

    root_i = iget(sb, FS_ROOT_INO,FALSE);
    if( NULL == root_i)
    {
        D( ( DL_FATERROR, "Mount root filesystem: failed to read root inode" ) );
        iRet = ERR_FS_MOUNT_READ_ROOT_INODE_FAILED;
        DSM_ASSERT(0,"FS_MountRoot5:iRet = %d",iResult);
        goto step0_failed;
    }

    root_i->i_is_mountpoint = FALSE;
    sb->s_mountpoint = sb->s_root = root_i;

    g_fs_current.pwd = root_i;
    g_fs_current.root = root_i;

    root_i->i_count += 3;

    DSM_TcStrCpy(g_fs_current.wdPath, (PCWSTR)_L_DSM("/"));

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        DRV_SET_WCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE);
        DRV_SET_RCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE*2);
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    return ERR_SUCCESS;

step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    if(sb)
    {
        fs_delete_sb(uDevNo);
    }

    return fs_set_error_code(iRet);
}


INT32 FS_Mount( PCSTR pszDevName, PCSTR pszMountPoint, INT32 iFlag, UINT8 iType )
{
    struct inode *mount_point_i = NULL;
    struct inode *root_i = NULL;
    UINT32 uDevNo = 0;
    struct super_block * sb = NULL;
    INT32 iRet = ERR_SUCCESS;
    INT32 iResult = ERR_SUCCESS;
    // UINT32 iCacheSize = 0;
    UINT16 iULen = 0;

    //cheat compiler
    iFlag = iFlag;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    iULen = DSM_UnicodeLen((UINT8*)pszMountPoint);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    uDevNo = DSM_DevName2DevNo( pszDevName );
    if (INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "Unknown device name[%s]", pszDevName ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    // file name character validate.
    if( !is_file_name_valid( (PCWSTR)pszMountPoint, FALSE ))
    {
        D( ( DL_FATERROR, "in FS_Mount , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    mount_point_i = namei( (PCWSTR)pszMountPoint );

    if(!mount_point_i) // mount point not found
    {
        D( ( DL_FATERROR, "Mountpoint does not exist!\n" ) );
        iRet = ERR_FS_INVALID_DIR_ENTRY;
        goto step0_failed;
    }

    if(!FS_IS_DIR( mount_point_i->i_mode ))  // mount point is not directory
    {
        D( ( DL_FATERROR, "Mountpoint MUST be a directory" ) );
        iRet = ERR_FS_NOT_DIRECTORY;
        goto step1_failed;
    }

    if( mount_point_i->i_count != 1 ) // mount point is busy
    {
        D( ( DL_FATERROR, "[%s] is under reference, i_count = %d.\n", pszMountPoint, mount_point_i->i_count) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step1_failed;
    }

    if( mount_point_i->i_ino == FS_ROOT_INO )
    {
        D( ( DL_FATERROR, "some fs has mounted on the mountpoint[%s]", pszMountPoint ) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step1_failed;
    }

    iResult = fs_find_sb(uDevNo, &sb);
    if(iResult == ERR_SUCCESS)  // file system has been mounted
    {
        D( ( DL_FATERROR, "file system on the device[%s] HAS mounted", pszDevName ) );
        iRet = ERR_FS_HAS_MOUNTED;
        goto step1_failed;
    }

    iResult = fs_get_free_sb(&sb);
    if( ERR_SUCCESS != iResult )
    {
        D( ( DL_FATERROR, "Too many file systems has mounted" ) );
        iRet = ERR_FS_MOUNTED_FS_MAX;
        goto step1_failed;
    }

    switch( iType )
    {
        case FS_TYPE_FAT:
#ifdef FS_SUPPORT_EXFAT
        case FS_TYPE_EXFAT:
#endif
            if ( DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH )
            {
                iRet = DRV_DEV_ACTIVE(uDevNo);
                if( ERR_SUCCESS != iRet )
                {
                    D( ( DL_FATERROR, "Mount filesystem: Open t-flash memory device failed" ) );
                    iRet =  ERR_FS_OPEN_DEV_FAILED;
                    goto step1_failed;
                }
            }
            iResult = fs_read_super(sb, uDevNo );
            if(iResult != ERR_SUCCESS) // read fat super block from device failed
            {
                D( ( DL_FATERROR, "Read the super block for the file system to mount failed" ) );
                iRet = fs_set_error_code(iResult);
                goto step1_failed;
            }

            break;
        default:  // unknown file system iType
            iRet = ERR_FS_UNKNOWN_FILESYSTEM;
            D((DL_FATERROR, "unknown file system type"));
            goto step1_failed;

            break;
    }

    root_i = iget( sb, (UINT64)FS_ROOT_INO,FALSE);
    if(!root_i)  // Unable to read root i-node
    {
        D( ( DL_FATERROR, "Read the root inode for the file system to mount failed" ) );
        iRet = ERR_FS_MOUNT_READ_ROOT_INODE_FAILED;
        goto step2_failed;
    }
    mount_point_i->i_is_mountpoint = TRUE;

    /*DO NOT increase the reference for the inode, for mount_point_i is a emporary pointer*/
    sb->s_mountpoint = mount_point_i;

    /*DO NOT increase the reference for the inode, for root_i is a emporary pointer*/
    sb->s_root = root_i;

    // NOTE! we don't iput( mount_point_i ), we do that in umount

    // Setup cache size for mmc drier.
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(FS_TYPE_FAT == sb->s_fstype)
        {
            DRV_SET_WCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE);
            DRV_SET_RCACHE_SIZE(uDevNo,sb->u.fat_sb.iSecPerClus*DEFAULT_SECSIZE*4);
        }
#ifdef FS_SUPPORT_EXFAT
        else if(FS_TYPE_EXFAT == sb->s_fstype)
        {
            DRV_SET_WCACHE_SIZE(uDevNo,sb->u.exfat_sb.SectorsPerCluster*DEFAULT_SECSIZE);
            DRV_SET_RCACHE_SIZE(uDevNo,sb->u.exfat_sb.SectorsPerCluster*DEFAULT_SECSIZE*4);
        }
#endif
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();

    // If succeful,not free it until device unmount.
    // else free it befor return.
    return ERR_SUCCESS;

step2_failed:
    if(sb)
    {
        fs_delete_sb(uDevNo);
    }
step1_failed:
    if(mount_point_i)
    {
        iput( mount_point_i );
    }
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;
}


INT32 FS_Unmount( PCSTR pszMountPoint, BOOL bForce )
{
    struct inode *mount_point_i, *inode;
    struct super_block * sb;
    INT32 iRet;
    INT32 i;
    UINT8 DevName[ 8 ] = { 0, };
    UINT32 fd;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;
    struct file *f;

    iULen = DSM_UnicodeLen((UINT8*)pszMountPoint);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    // file name character validate.
    if( !is_file_name_valid( (PCWSTR)pszMountPoint, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_Unmount , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();
    mount_point_i = namei( (PCWSTR)pszMountPoint );

    if( !mount_point_i )
    {
        D( ( DL_FATERROR, "Mountpoint does not exist" ) );
        iRet = ERR_FS_INVALID_DIR_ENTRY;
        goto step0_failed;
    }

    if(!FS_IS_DIR( mount_point_i->i_mode ))  // mount point is not directory
    {
        D( ( DL_FATERROR, "Mountpoint MUST be a directory file" ) );
        iRet = ERR_FS_NOT_DIRECTORY;
        goto step1_failed;
    }

    for ( i = 0, sb = g_fs_super_block_table; i < (INT32)g_fs_cfg.uNrSuperBlock; i++, sb++ )
    {
        if ( INVALID_DEVICE_NUMBER != sb->s_dev && sb->s_root == mount_point_i )
        {
            break;
        }
    }
    if( (INT32)g_fs_cfg.uNrSuperBlock == i ) // super block not found
    {
        D( ( DL_FATERROR, "NO file system mounts on [%s]", pszMountPoint ) );
        iRet = ERR_FS_NOT_FIND_SB;
        goto step1_failed;
    }
    uDevNo = sb->s_dev;
    iRet = DSM_DevNr2Name( sb->s_dev, DevName );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_FATERROR, "Device not found for mountpoint [%s]", pszMountPoint ) );
        iRet = ERR_FS_INVALID_DEV_NUMBER;
        goto step1_failed;
    }

    if ( !bForce )
    {
        for ( inode = sb->inode_chain; inode != NULL; inode = inode->next )
        {
            /*why we skip mount_point_i? because it is surely in inode table:-)*/
            if ( inode->i_count > 0 && inode->i_dev == sb->s_dev && inode != mount_point_i )
            {
                D( ( DL_FATERROR, "File system to unmount is being used" ) );
                iRet = ERR_FS_DEVICE_BUSY;
                goto step1_failed;
            }
        }
    }
    else
    {
        for ( fd = 0; fd < g_fs_cfg.uNrOpenFileMax; fd++ )
        {
            f = g_fs_current.filp[ fd ];
            if (!f)
            {
                continue;
            }

            inode = f->f_inode;
            if ( inode->i_dev == sb->s_dev )
            {
                if(f->f_flags & FS_O_FIND) // fd is for finding
                {
                    iRet = fs_findclose( fd );
                    if( ERR_SUCCESS != iRet )
                    {
                        D( ( DL_FATERROR, "FindClose(%d) failed[%d]", fd, iRet ) );
                        iRet = fs_set_error_code(iRet);
                        goto step1_failed;
                    }
                }
                else
                {
                    iRet = fs_close(fd);
                    if( ERR_SUCCESS != iRet )
                    {
                        D( ( DL_FATERROR, "Close(%d) failed[%d]", fd, iRet ) );
                        iRet = fs_set_error_code(iRet);
                        goto step1_failed;
                    }
                }
            }
        }
        if( g_fs_current.pwd == mount_point_i)
        {
            iput( g_fs_current.pwd );
            g_fs_current.pwd = g_fs_current.root;
            g_fs_current.root->i_count++;
        }
        fs_flush_cache(uDevNo);

    }

    /*Is root dir device*/
    if(g_fs_current.root == mount_point_i)
    {
        if(mount_point_i->i_count > 4 ) // mount point  is busy
        {
            D( ( DL_FATERROR, "[%s] is under reference", pszMountPoint ) );
            iRet = ERR_FS_OPERATION_NOT_GRANTED;
            goto step1_failed;
        }
        else
        {
            /* Unmount root-FS from "/." is permitted */
            sb->s_mountpoint->i_is_mountpoint = FALSE;

            iput( sb->s_root );
            sb->s_root  = NULL;

            if(g_fs_current.pwd == mount_point_i)
            {
                iput(g_fs_current.pwd);
                g_fs_current.pwd = NULL;
            }
            iput(g_fs_current.root);
            g_fs_current.root = NULL;
            DSM_MemSet( g_fs_current.filp, 0x00, (sizeof( struct file * ) ) * g_fs_cfg.uNrOpenFileMax);
        }
    }
    else
    {
        if(sb->s_mountpoint->i_count > 1 ) // mount point  is busy
        {
            D( ( DL_FATERROR, "[%s] is under reference", pszMountPoint ) );
            iRet = ERR_FS_OPERATION_NOT_GRANTED;
            goto step1_failed;
        }
        else
        {
            iput( mount_point_i);
            iput(sb->s_root);
            iput(sb->s_mountpoint);
            if(g_fs_current.pwd->i_sb == sb)
            {
                if(g_fs_current.pwd->i_count > 1)
                {
                    D( ( DL_FATERROR, "pwd is busy,pwd->i_count = %d.",
                         g_fs_current.pwd->i_count) );
                    iRet = ERR_FS_OPERATION_NOT_GRANTED;
                    goto step1_failed;
                }
                iput(g_fs_current.pwd);
                g_fs_current.pwd = g_fs_current.root;
            }
        }
    }

    DSM_MemSet( sb, 0, sizeof(*sb) );
    iRet = DRV_DEV_FLUSH(uDevNo);
    if(ERR_SUCCESS != iRet)
    {
        D( ( DL_FATERROR, " unmount:flush cache error.  return error = 0x%x.", iRet) );
    }
    CSW_TRACE(BASE_DSM_TS_ID, "#fs debug unmount(%d) success.",uDevNo);
    FS_Up();
    return ERR_SUCCESS;
step1_failed:
    iput( mount_point_i );
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    CSW_TRACE(BASE_DSM_TS_ID,"#fs debug unmount(%d) failed.err_code = %d.",
              uDevNo, iRet);
    FS_Up();
    return iRet;
}


/*
Upon successful completion, TRUE is returned, else FALSE is returned.
*/
INT32 FS_ChangeDir( PCSTR pszDirName )
{
    WCHAR *path;
    struct inode *dir_i = NULL;
    struct inode *inode = NULL;
    struct dentry *psDe = NULL;
    WCHAR *basename;
    INT32 namelen;
    INT32 ret;
    UINT16 len = 0;
    UINT32 dev_no = 0;
#ifdef FS_SUPPORT_EXFAT
    WCHAR *p;
    WCHAR uni_char;
    UINT8 temp1, temp2;
    WCHAR *cur_dir = NULL;
#endif

    len = DSM_UnicodeLen((UINT8*)pszDirName);
    if(0 == len || len > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    if( !is_file_name_valid( (PCWSTR)pszDirName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, pszDirName is invalid" ) );
        return ERR_FS_INVALID_PARAMETER;
    }

    path = format_dir_name((PCWSTR) pszDirName );
    if(!path)
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, path is invalid" ) );
        return ERR_FS_NO_MORE_MEMORY;
    }

    FS_Down();
    /*namei()--begin*/
    if(DSM_TcStrCmp(path,(PCWSTR)L".." ) == 0 )
    {
#ifdef FS_SUPPORT_EXFAT
        if(g_fs_current.pwd->i_fstype == FS_TYPE_EXFAT)
        {
            //FS_GetCurDir(SIZEOF(current_dir), (UINT8 *)current_dir);
            cur_dir = NAME_BUF_ALLOC();
            if(!cur_dir)
            {
                FS_Up();
                DSM_ASSERT(0,"NAME_BUF_ALLOC faile.");
                return ERR_FS_NO_MORE_MEMORY;
            }
            DSM_TcStrCpy((PCWSTR)cur_dir, g_fs_current.wdPath);
            p = (WCHAR *)cur_dir;
            while(*p)
            {
                p++;
            }

            while (TRUE)
            {
                //delete the '/' in the end
                uni_char = *(p - 1);
                temp1 = *((UINT8*)(&uni_char));
                temp2 = *((UINT8*)(&uni_char) + 1);
                if (!(('/' == temp1 || '\\' == temp1) && temp2 == 0))
                {
                    p--;
                    *p = (UINT16)0;
                }
                else
                {
                    break;
                }
            }
            if(path != NULL)
            {
                NAME_BUF_FREE(path);
            }
            path = format_dir_name((PCWSTR)cur_dir);
            if(!path)
            {
                D( ( DL_FATERROR, "in FS_ChangeDir, path is invalid" ) );
                ret = ERR_FS_PATHNAME_PARSE_FAILED;
                goto step0_failed;
            }
        }
#endif
        dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    }
    else
    {
        dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    }

    if ( !dir_i )
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, dir_i is invalid" ) );
        ret = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }

    if ( !namelen ) // path is invalid
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, namelen = %d",namelen) );
        ret = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    dev_no = dir_i->i_dev;

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, malloc psDe failed!.\n") );
        ret = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }
    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename);
    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    ret = fs_find_entry( &dir_i, psDe, FALSE);
    if(ret != 0)
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, find_entry error iRet = %d",ret) );
        ret = ERR_FS_NO_DIR_ENTRY;
        goto step1_failed;
    }

    inode = psDe->inode;
    /*namei()--end*/

    if ( !FS_IS_DIR( inode->i_mode ) )
    {
        D( ( DL_FATERROR, "in FS_ChangeDir, inode is not dir") );
        iput( inode );
        ret = ERR_FS_NOT_DIRECTORY;
        goto step1_failed;
    }

    ret = fs_set_work_dir(g_fs_current.wdPath, (PCWSTR)pszDirName);
    if (ret != ERR_SUCCESS)
    {
        D((DL_FATERROR, "fs_set_work_dir: fs_set_work_dir return %d", ret));
        iput(inode);
        goto step1_failed;
    }

    iput( g_fs_current.pwd );
    g_fs_current.pwd = inode;   /*we have add the refcnt for inode in find_entry()*/

    ret = ERR_SUCCESS;
step1_failed:
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }
#ifdef FS_SUPPORT_EXFAT
    if(cur_dir)
    {
        NAME_BUF_FREE(cur_dir);
    }
#endif
    if(DSM_GetDevType(dev_no) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(dev_no))
        {
            DRV_DEV_DEACTIVE(dev_no);
        }
    }
    FS_Up();
    return fs_set_error_code(ret);
}

//---------------------------------------------------------------------------------------------------------------------
// FS_Open
//---------------------------------------------------------------------------------------------------------------------
// @pszFileName
// File name,the string format is unicode.
// @iFlag
// Open flag of access permitted.
// Specifies the action to take.The following values can be combined by using the bitwise OR operator.
// FS_O_RDONLY:      Read only.
// FS_O_WRONLY:      Write only.
// FS_O_RDWR:        Read and Write.
// FS_O_ACCMODE:     Access.
// FS_O_CREAT:       If the file exists, this flag has no effect except as noted under FS_O_EXCL below. Otherwise, the file shall be created.
// FS_O_EXCL:        If FS_O_CREAT and FS_O_EXCL are set, the function shall fail if the file exists.
// FS_O_TRUNC:       If the file exists, and is a regular file, and the file is successfully opened FS_O_WRONLY or FS_O_RDWR, its length shall be truncated to 0.
// FS_O_APPEND         If set, the file offset shall be set to the end of the file prior to each write.
// @iMode
// File or directory attibute. The following values can be combined by using the bitwise OR operator.If file has existed,the oprate of open  will effect when flag
// is been set FS_O_CREAT and FS_O_TRUNC.
// FS_ATTR_RO          read-only
// FS_ATTR_HIDDEN   hidden
// FS_ATTR_SYSTEM   system
// FS_ATTR_ARCHIVE archived
// Return
// Sucesse return ERR_SUCCESS,else return flowing error code:
// ERR_FS_INVALID_PARAMETER: Invalid parameter.
// ERR_FS_NO_MORE_MEMORY:No more memory to alloc.
// Todo
//---------------------------------------------------------------------------------------------------------------------
INT32 FS_Open(PCSTR pszFileName, UINT32 iFlag, UINT32 iMode)
{
    INT32 fd;
    struct inode *dir_i = NULL;
    struct file *f = NULL;
    WCHAR *basename = NULL;
    INT32 namelen;
    struct dentry *psDe = NULL;
    INT32 iRet = ERR_SUCCESS;
    INT16 iULen = 0;
    UINT8 iLocalMode;

    // change general mode(32bit) to fat mode(8bit).
    iLocalMode= FS_ATTR_MARK;
    iLocalMode |= ((iMode & FS_ATTR_RO)  == FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
    iLocalMode |= ((iMode & FS_ATTR_HIDDEN)  == FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
    iLocalMode |= ((iMode & FS_ATTR_SYSTEM)  == FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
    iLocalMode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;

    iULen = (UINT16)DSM_UnicodeLen((UINT8 *)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        D( ( DL_FATERROR, "in FS_Open ,file name length  too long.iULen = %d.", iULen) );
        DSM_ASSERT(0, "open1. len= 0x%x,pszDirName = 0x%x", iULen, pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }
    {
        uint8* oem_name = NULL;
        UINT32 oem_len = 0;
        INT32 ret;
        ret = DSM_Unicode2OEM(pszFileName,(UINT32)iULen,&oem_name,&oem_len,NULL);
        if(ret == 0)
        {
            CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:open file(%s) begin,flag = 0x%x,mode = 0x%x,name_len = %d.",
                      oem_name,iFlag,iMode,iULen);
            DSM_Free(oem_name);
        }
    }
    //DSM_Dump((UINT8*)pszFileName,iULen + 2,16);

    // file name character validate.
    if( !is_file_name_valid((PCWSTR) pszFileName, FALSE ))
    {
        D( ( DL_FATERROR, "in FS_Open ,file name is invalid.") );
        //DSM_ASSERT(0,"open2. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        return  ERR_FS_INVALID_PARAMETER;
    }

    // To truncate the file, we should have the write access to the file
    if ( ( iFlag & FS_O_TRUNC ) && ( ( iFlag & FS_O_ACCMODE ) == FS_O_RDONLY ) )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_INVALID_PARAMETER1,iFlag = 0x%x.\n", iFlag));
        DSM_ASSERT(0, "open3. len= 0x%x,pszDirName = 0x%x", iULen, pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    // To append the file, we should have the write access to the file
    if ( ( iFlag & FS_O_APPEND ) && ( ( iFlag & FS_O_ACCMODE ) == FS_O_RDONLY ) )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_INVALID_PARAMETER2,iFlag = 0x%x.\n", iFlag));
        DSM_ASSERT(0, "open4. len= 0x%x,pszDirName = 0x%x", iULen, pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    // The low byte greate less 3.
    if((iFlag & 0xf) > FS_O_ACCMODE )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_INVALID_PARAMETER3.iFlag = 0x%x.\n", iFlag));
        DSM_ASSERT(0, "open5. len= 0x%x,pszDirName = 0x%x", iULen, pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    // Get a valid file descriptor
    for ( fd = 0; fd < (INT32)g_fs_cfg.uNrOpenFileMax; fd++ )
    {
        if ( NULL == g_fs_current.filp[ fd ] )                 /*unused fhd found*/
            break;
    }

    if(fd == (INT32)g_fs_cfg.uNrOpenFileMax)
    {
        D((DL_FATWARNING, "FS_Open: FS_NR_OPEN_FILES_MAX.\n"));
        iRet = ERR_FS_FDS_MAX;
        DSM_ASSERT(0,"open6. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        goto step0_failed;
    }

    f = fs_alloc_fd();
    if (NULL == f)
    {
        D( ( DL_FATERROR, "in FS_Open, fs_alloc_fd() return NULL") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"open7. len= 0x%x,pszDirName = 0x%x",iULen,pszFileName);
        goto step0_failed;
    }

    f = fs_fd_list_add(&g_fs_file_list, f);
    dir_i = dir_namei((PCWSTR) pszFileName, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i )
    {
        D((DL_FATERROR, "FS_Open: ERR_FS_PATHNAME_PARSE_FAILED.\n"));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    if(0 == namelen)
    {
        iRet = ERR_FS_NO_BASENAME;
        goto step1_failed;
    }

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_Open, malloc psDe failed!.\n") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step1_failed;
    }

    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename );
    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    if ( 0 == fs_find_entry( &dir_i, psDe, TRUE ) )
    {
        D( ( DL_FATDETAIL, "in FS_Open, --->psDe.inode.i_ino = %d\n",psDe->inode->i_ino) );

        if(( iFlag & FS_O_CREAT ) && ( iFlag &FS_O_EXCL ))
        {
            D( ( DL_FATERROR, "in FS_Open the file already existed, and flag is FS_O_CREAT") );
            iRet = ERR_FS_FILE_EXIST;
            goto step2_failed;
        }

        if ( FS_IS_DIR( psDe->inode->i_mode ) ) // Attempt to open a directroy file
        {
            D( ( DL_FATERROR, "in FS_Open, the inode is not a file,but a dir\n") );
            iRet = ERR_FS_IS_DIRECTORY;
            goto step2_failed;
        }

        if ( iFlag & FS_O_TRUNC )
        {
            if(!(iFlag & FS_O_CREAT))
            {
                iLocalMode = (UINT8)psDe->inode->i_mode;
            }
            iRet = fs_trunc_file( dir_i, psDe, iLocalMode );
            if(ERR_SUCCESS != iRet) // create file failed
            {
                D( ( DL_FATERROR, "in FS_Open, trunc file error, ErrorCode = %d\n",iRet) );
                //lErrCode = ERR_FS_TRUNC_FILE_FAILED;
                goto step2_failed;
            }
        }
    }
    else
    {
        // entry not found
        if ( !( iFlag & FS_O_CREAT ) )
        {
            // FS_O_CREAT flag not setting.
            D( ( DL_FATERROR, "in FS_Open the file not existed!") );
            iRet = ERR_FS_NO_DIR_ENTRY;
            goto step1_failed;
        }
        iRet = fs_create_file( dir_i, psDe, iLocalMode);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D(( DL_FATERROR,"in FS_Open() fs_create_file() failed,lErrCode = %d.\n",iRet));
            goto step1_failed;
        }
    }


    g_fs_current.filp[ fd ] = f;
    f->f_mode = psDe->inode->i_mode;
    f->f_flags = ( UINT16 ) iFlag;
    f->f_count = 1;
    f->f_inode = psDe->inode;
    /* Whether FS_O_APPEND flag is specified or not, we always set file pointer to 0,
         because we may read the file before the first write. */
    f->f_pos = 0;

    iput( dir_i );
    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

    //  CSW_TRACE(BASE_DSM_TS_ID,
    //             "#fs debug open success = %d",
    //              fd);
    FS_Up();
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:open hd = 0x%x",fd);
    return fd;

step2_failed:
    iput( psDe->inode );

step1_failed:
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

step0_failed:
    if (f != NULL)
    {
        fs_fd_list_del(&g_fs_file_list, f);
    }
    FS_Up();
    if(iRet < 0)
    {
        CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:open failed,iRet = %d",iRet);
    }
    return fs_set_error_code(iRet);
}


INT32 FS_Create(PCSTR pszFileName, UINT32 iMode)
{
    return FS_Open( pszFileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, iMode );
}

INT32 FS_Close(INT32 fd)
{
    INT32 iRet;
    struct file *filp;
    UINT32 uDevNo = 0;

    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:close begin,hd = 0x%x",fd);
    if( !FD_IS_VALID( fd ) ) // fd is invalid
    {
        return ERR_FS_BAD_FD;
    }
    filp = g_fs_current.filp[ fd ];
    if(!filp)
    {
        return ERR_FS_BAD_FD;
    }
    uDevNo = filp->f_inode->i_dev;

    FS_Down();
    iRet = fs_close(fd);
    if(ERR_SUCCESS == iRet && DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        iRet = DRV_DEV_FLUSH(uDevNo);
        if(ERR_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, " cache:FS_Close flush cache error.  return error = 0x%x.", iRet) );
        }
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:close hd = 0x%x",fd);
    return iRet;
}

INT32 FS_Delete(PCSTR pszFileName)
{
    WCHAR* basename;
    INT32 namelen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;

    iULen = (UINT16)DSM_UnicodeLen((UINT8*)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        DSM_ASSERT(0,"delete file1,pszFilename = 0x%x,",pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    CSW_TRACE(BASE_DSM_TS_ID, "delete file:");
    //DSM_Dump((UINT8*)pszFileName,iULen + 2,16);

    if(!is_file_name_valid( (PCWSTR)pszFileName, FALSE )) // invalid pathname
    {
        D( ( DL_FATERROR, "in FS_Delete, FileName is invalid\n") );
        //     DSM_ASSERT(0,"delete file2,pszFilename = 0x%x,",pszFileName);
        return ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    dir_i = dir_namei( (PCWSTR)pszFileName, &namelen, (CONST WCHAR**)&basename );
    if( !dir_i ) // path is invalid
    {
        D( ( DL_FATERROR, "in FS_Delete, dir is invalid\n") );
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        // DSM_ASSERT(0,"delete file3,pszFilename = 0x%x,",pszFileName);
        goto step0_failed;
    }

    if(!namelen) // file name not specified
    {
        D( ( DL_FATERROR, "in FS_Delete, namelen = %d\n", namelen) );
        iRet = ERR_FS_NO_BASENAME;
        // DSM_ASSERT(0,"delete file4,pszFilename = 0x%x,",pszFileName);
        goto step1_failed;
    }

    uDevNo = dir_i->i_dev;

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_Delete, malloc psDe failed!.\n") );
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"delete file5,pszFilename = 0x%x,",pszFileName);
        goto step1_failed;
    }

    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename );

    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    iRet = fs_find_entry( &dir_i, psDe, FALSE);
    if(-1 == iRet)
    {
        D( ( DL_FATERROR, "in FS_Delete, find_entry return -1, find no entry\n") );
        iRet = ERR_FS_NO_DIR_ENTRY;
        // DSM_ASSERT(0,"delete file6,pszFilename = 0x%x,",pszFileName);
        goto step1_failed;
    }

    if(FS_IS_DIR( psDe->inode->i_mode )) // entry is directory
    {
        D( ( DL_FATERROR, "in FS_Delete, entry is directory.\n") );
        iRet = ERR_FS_IS_DIRECTORY;
        // DSM_ASSERT(0,"delete file7,pszFilename = 0x%x,",pszFileName);
        goto step2_failed;
    }

    if(psDe->inode->i_dev != dir_i->i_dev) // Not permition
    {
        D( ( DL_FATERROR, "in FS_Delete, psDe.inode->i_dev = %d, dir_i->i_dev = %d\n",psDe->inode->i_dev,dir_i->i_dev) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"delete file8,pszFilename = 0x%x,",pszFileName);
        goto step2_failed;
    }

    // Some process is dealing with the file. It will not be removed.
    if(psDe->inode->i_count > 1)
    {
        D( ( DL_FATERROR, "in FS_Delete, psDe.inode->i_count = %d\n",psDe->inode->i_count) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto step2_failed;
    }

    iRet = fs_do_unlink( dir_i, psDe, FALSE );
    if(ERR_SUCCESS != iRet) // unlink failed.
    {
        iRet = fs_set_error_code(iRet);
        goto step2_failed;
    }


    dir_i->i_mtime = DSM_GetFileTime();

    iRet = ERR_SUCCESS;

step2_failed:
    iput( psDe->inode );
step1_failed:
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }
step0_failed:
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;
}


INT64 FS_Seek(INT32 fd, INT64 iOffset, UINT8 iOrigin)
{
    struct file * file;
    INT64 tmp = 0;
    INT64 iRet = ERR_SUCCESS;
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Seek() begin,fd = 0x%x,iOffset = 0x%llx,iOrigin = 0x%x.",
              fd,iOffset,iOrigin);
    // DSM_ASSERT(fd >= 0);
    if ( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_Seek, invalide fd = %d\n",fd) );
        // DSM_ASSERT(0,"seek error1. fd = 0x%x",fd);
        return ERR_FS_BAD_FD;
    }

//   FS_Down();

    file = g_fs_current.filp[ fd ];

    if ( !( file ) || !fs_is_file_opened(&g_fs_file_list, file) || !( file->f_inode ) )
    {
        D( ( DL_FATERROR, "in FS_Seek, file is invalid\n") );
        // DSM_ASSERT(0,"seek error2. fd = 0x%x",fd);
        return ERR_FS_INVALID_PARAMETER;
    }

    if ( file->f_flags & FS_O_FIND ) // handle is for finding
    {
        D( ( DL_FATERROR, "in FS_Seek, file->f_flags is for finding, = %d\n",file->f_flags) );
        // DSM_ASSERT(0,"seek error3. fd = 0x%x",fd);
        return ERR_FS_OPERATION_NOT_GRANTED;
    }

    switch ( iOrigin )
    {
        case FS_SEEK_SET:
            tmp = 0 + iOffset;
            file->f_pos = tmp < 0 ? 0 : (tmp > (INT32)file->f_inode->i_size ? (INT32)file->f_inode->i_size: tmp);
            iRet = file->f_pos;
            break;

        case FS_SEEK_CUR:
            tmp = file->f_pos + iOffset;
            file->f_pos = tmp < 0 ? 0 : (tmp > (INT32)file->f_inode->i_size ? (INT32)file->f_inode->i_size: tmp);
            iRet = file->f_pos;
            break;

        case FS_SEEK_END:
            tmp = file->f_inode->i_size + iOffset;
            file->f_pos = tmp < 0 ? 0 : (tmp > (INT32)file->f_inode->i_size ? (INT32)file->f_inode->i_size: tmp);
            iRet = file->f_pos;
            break;

        default:
            iRet = ERR_FS_INVALID_PARAMETER;
    }
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Seek() end,fd = 0x%x,iRet = 0x%llx.",
              fd,iRet);
//   FS_Up();
    return iRet;
}


INT32 FS_IsEndOfFile(INT32 fd)
{
    struct file *filp;
    INT32 iRet;

    if(!FD_IS_VALID( fd ))
    {
        D( ( DL_FATERROR, "in FS_IsEndOfFile, fd = %d, is invalid\n", fd) );
        // DSM_ASSERT(0,"is_end_of_file error1. fd = 0x%x",fd);
        return ERR_FS_BAD_FD;
    }

    FS_Down();

    filp = g_fs_current.filp[ fd ];
    if(!filp || !fs_is_file_opened(&g_fs_file_list, filp)) // fd has closed
    {
        iRet = ERR_FS_BAD_FD;
        // DSM_ASSERT(0,"is_end_of_file error2. fd = 0x%x",fd);
        goto label_exit;
    }
    if ( filp->f_flags & FS_O_FIND ) // handle is for finding
    {
        D( ( DL_FATERROR, "in FS_IsEndOfFile, filp->f_flags = %d\n", filp->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto label_exit;
    }

    iRet = (( filp->f_inode->i_size == (UINT32)filp->f_pos ) ? 1 : 0);

label_exit:

    FS_Up();
    return iRet;
}


INT32 FS_Read(INT32 fd, UINT8 *pBuf, UINT32 iLen)
{
    struct file *file = NULL;
    struct inode *inode;
    INT32 iLocalLen;
    INT32 iResult = 0;
    INT32 iRet;
    // UINT32 tst_begin_time;
    // UINT32 tst_read_time;

    //tst_begin_time = hal_TimGetUpTime();

    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Read() begin,fd = 0x%x,iLen = %d.",
              fd,iLen);

    if( NULL == pBuf || iLen < 0)
    {
        D( ( DL_FATERROR, "in FS_Read, parameter error, iLen = %d\n", iLen) );
        DSM_ASSERT(0, "read error1. pBuf = 0x%x,iLen = 0x%x.", pBuf, iLen);
        return ERR_FS_INVALID_PARAMETER;
    }

    if ( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_Read, parameter error, fd = %d\n", fd) );
        // DSM_ASSERT(0,"read error2. fd = 0x%x.",fd);
        return ERR_FS_BAD_FD;
    }

    FS_Down();

    file = g_fs_current.filp[ fd ];
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Read() ,fd = 0x%x,file_pos = 0x%llx.",
              fd,file->f_pos);
    if ( iLen < 0 || !file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_Read, iLen = 0x%x\n",iLen) );
        iRet = ERR_FS_BAD_FD;
        // DSM_ASSERT(0,"read error3. fd = 0x%x.",fd);
        goto label_exit;
    }

    iLocalLen = iLen;

    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        D( ( DL_FATERROR, "in FS_Read, 0_file->f_flags = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"read error4. fd = 0x%x.",fd);
        goto label_exit;
    }
    if (FS_O_WRONLY == ( file->f_flags & FS_O_ACCMODE ) )
    {
        D( ( DL_FATERROR, "in FS_Read, 1_file->f_flags = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"read error5. fd = 0x%x.",fd);
        goto label_exit;
    }

    if ( 0 == iLocalLen )
    {
        iRet = 0;
        goto label_exit;
    }

    inode = file->f_inode;

    if (FS_IS_DIR( inode->i_mode ) )
    {
        D( ( DL_FATERROR, "in FS_Read, not regular file\n") );
        iRet = ERR_FS_NOT_REGULAR;
        DSM_ASSERT(0,"read error6. fd = 0x%x.",fd);
        goto label_exit;
    }

    if ( iLocalLen + file->f_pos > inode->i_size )
    {
        iLocalLen = (INT32)(inode->i_size - file->f_pos);
    }

    if ( iLocalLen <= 0 )/*after seek opteration, len may be negative*/
    {
        iRet = 0;
        goto label_exit;
    }

    iResult = fs_file_read(file, (INT8*)pBuf, iLocalLen);
    if(0 <= iResult)
    {
        iRet = iResult;
    }
    else
    {
        D( ( DL_FATERROR, "Call fs_file_read() failed. Local error code:%d.\n", iResult) );
        iRet = fs_set_error_code((INT32)iResult);
        //DSM_ASSERT(0,"read error7. fd = 0x%x,iResult = %d.",fd,iResult);
        goto label_exit;
    }
label_exit:
    FS_Up();
    //tst_read_time = hal_TimGetUpTime() - tst_begin_time;
    //CSW_TRACE(BASE_DSM_TS_ID, "read hd = 0x%x,size = 0x%x,time = 0x%x.",fd,iLen,tst_read_time);
    //DSM_Dump(pBuf, iLen, 16);
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Read() end,fd = 0x%x,iLen = %d, iRet = %d.",
              fd,iLen,iRet);
    return iRet;
}


INT32 FS_Write(INT32 fd, UINT8 *pBuf, UINT32 iLen)
{
    struct file * file;
    struct inode * inode;
    INT32 iResult;
    INT32 iRet;
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Write() begin,fd = 0x%x,iLen = %d.",
              fd,iLen);
    if( NULL == pBuf || iLen < 0)
    {
        D( ( DL_FATERROR, "in FS_Write, invalid parameter\n") );
        DSM_ASSERT(0,"write error1. pBuf = 0x%x,iLen = 0x%x.",pBuf,iLen);
        return ERR_FS_INVALID_PARAMETER;
    }
    //CSW_TRACE(BASE_DSM_TS_ID, "write hd = 0x%x,size = 0x%x.",fd,iLen);
    //DSM_Dump(pBuf, iLen, 16);
    if( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_Write, invalid fd = %d\n",fd) );
        // DSM_ASSERT(0,"write error2. fd = 0x%x.",fd);
        return ERR_FS_BAD_FD;
    }

    FS_Down();

    file = g_fs_current.filp[ fd ];
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Write() ,fd = 0x%x,file_pos = 0x%llx.",
              fd,file->f_pos);
    if (NULL == file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_Write, file is NULL.\n") );
        iRet = ERR_FS_BAD_FD;
        // DSM_ASSERT(0,"write error3. fd = 0x%x.",fd);
        goto label_exit;
    }

    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        D( ( DL_FATERROR, "0_in FS_Write,file->f_flags  = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"write error4. fd = 0x%x.",fd);
        goto label_exit;
    }

    if (FS_O_RDONLY == ( file->f_flags & FS_O_ACCMODE ) )
    {
        D( ( DL_FATERROR, "1_in FS_Write,file->f_flags  = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"write error5. fd = 0x%x.",fd);
        goto label_exit;
    }

    if ( 0 == iLen )
    {
        iRet = 0;
        goto label_exit;
    }

    inode = file->f_inode;

    if (FS_IS_DIR( inode->i_mode ) )
    {
        D( ( DL_FATERROR, "1_in FS_Write,file is not regular\n") );
        iRet = ERR_FS_NOT_REGULAR;
        DSM_ASSERT(0,"write error6. fd = 0x%x.",fd);
        goto label_exit;
    }

    iResult = fs_file_write(file, (INT8*)pBuf, iLen );
    if(_ERR_FAT_SUCCESS == iResult)
    {
        iRet = iLen;
    }
    else
    {
        D( ( DL_FATERROR, "1_in FS_Write,fs_file_write error\n") );
        iRet = fs_set_error_code(iResult);
        // DSM_ASSERT(0,"write error7. fd = 0x%x,iResult = %d.",fd,iResult);
    }

label_exit:
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:FS_Write() end,fd = 0x%x,iLen = %d,iRet = %d.",
              fd,iLen,iRet);
    FS_Up();
    return iRet;
}


INT32 FS_MakeDir(PCSTR pszDirName, UINT32 iMode)
{
    WCHAR *path = NULL;
    WCHAR *basename;
    INT32 namelen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;
    INT32  uDevNo = 0;
    UINT8 iFileMode;

    iFileMode = FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_RO)  == FS_ATTR_RO) ? FS_ATTR_RO : FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_HIDDEN)  == FS_ATTR_HIDDEN) ? FS_ATTR_HIDDEN : FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_SYSTEM)  == FS_ATTR_SYSTEM) ? FS_ATTR_SYSTEM : FS_ATTR_MARK;
    iFileMode |= ((iMode & FS_ATTR_ARCHIVE)  == FS_ATTR_ARCHIVE) ? FS_ATTR_ARCHIVE : FS_ATTR_MARK;
    iFileMode |= FS_ATTR_DIR;

    iULen = (UINT16)DSM_UnicodeLen((UINT8 *)pszDirName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        DSM_ASSERT(0, "len error. len= 0x%x,pszDirName = 0x%x", iULen, pszDirName);
        return ERR_FS_INVALID_PARAMETER;
    }

    CSW_TRACE(BASE_DSM_TS_ID, "make dir,mode = 0x%x filename(0x%x):", iMode, iULen);

    // file name character validate.
    if(!is_file_name_valid( (PCWSTR)pszDirName, FALSE ))
    {
        D( ( DL_FATERROR, "in FS_MakeDir,DirName is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        //  DSM_ASSERT(0,"file name is invalid.pszDirName = 0x%x",pszDirName);
        return ERR_FS_INVALID_PARAMETER;
    }

    path = format_dir_name( (PCWSTR)pszDirName );
    if(!path)
    {
        D( ( DL_FATERROR, "in FS_MakeDir, format_dir_name is NULL\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        DSM_ASSERT(0, "path is null.pszDirName = 0x%x,iULen = %d.", pszDirName, iULen);
        return ERR_FS_NO_MORE_MEMORY;
    }

    FS_Down();

    dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i ) // path is invalid
    {
        D( ( DL_FATERROR, "in FS_MakeDir,dir_i is NULL\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        //DSM_ASSERT(0,"dir_i is null.pszDirName = 0x%x",pszDirName);
        goto step0_failed;
    }
    if(0 == namelen) // namelen is 0
    {
        D( ( DL_FATERROR, "in FS_MakeDir,namelen = %d\n",namelen) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_NO_BASENAME;
        //DSM_ASSERT(0,"namelen = 0.pszDirName = 0x%x",pszDirName);
        goto step1_failed;
    }

    uDevNo = dir_i->i_dev;

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "in FS_MakeDir(), malloc psDe failed!.\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"psDe is null,malloc failed.pszDirName = 0x%x",pszDirName);
        goto step0_failed;
    }

    DSM_MemSet(psDe, 0x00, sizeof(struct dentry));
    DSM_TcStrCpy( psDe->name, basename);
    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    iRet = fs_find_entry( &dir_i, psDe, FALSE );
    if(!iRet)
    {
        // D( ( DL_FATERROR, "in FS_MakeDir,dentry is exist. iRet = %d.\n",iRet) );
        // DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_FILE_EXIST;
        goto step1_failed;
    }

    iRet = fs_create_file( dir_i, psDe, iFileMode);
    if(iRet)
    {
        D( ( DL_FATERROR, "in FS_MakeDir,fat_create_file error, iRet = %d\n",iRet) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = fs_set_error_code(iRet);
        goto step1_failed;
    }

    iRet = ERR_SUCCESS;

step1_failed:

    iput( dir_i );

    if (psDe != NULL)
    {
        if ( psDe->inode )
        {
            iput( psDe->inode );
        }
        DENTRY_FREE(psDe);
    }

step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }

    FS_Up();
    return iRet;
}


INT32 FS_RemoveDir(PCSTR pszDirName)
{
    WCHAR *path;
    WCHAR * basename;
    INT32 namelen;
    struct inode *dir_i;
    struct dentry *psDe = NULL;
    INT32 iRet = ERR_SUCCESS;
    INT32 result;
    UINT16 iULen = 0;
    UINT32 uDevNo = 0;

    CSW_TRACE(BASE_DSM_TS_ID, "remove dir:");
    iULen = (UINT16)DSM_UnicodeLen((UINT8*)pszDirName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        D( ( DL_FATERROR, "remove dir: len error.len = 0x%x,\n",iULen) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        DSM_ASSERT(0,"file name is invalid.pszDirName = 0x%x",pszDirName);
        return ERR_FS_INVALID_PARAMETER;
    }

    //DSM_Dump((UINT8*)pszDirName,iULen,16);
    if(!is_file_name_valid( (PCWSTR)pszDirName, FALSE ))
    {
        D( ( DL_FATERROR, "remove dir: DirName is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        DSM_ASSERT(0,"remove dir:file name is invalid.pszDirName = 0x%x",pszDirName);
        return ERR_FS_INVALID_PARAMETER;
    }

    path = format_dir_name( (PCWSTR)pszDirName );
    if(!path)
    {
        D( ( DL_FATERROR, "remove dir: path is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        DSM_ASSERT(0,"remove dir:path is null.pszDirName = 0x%x",pszDirName);
        return ERR_FS_NO_MORE_MEMORY;
    }

    FS_Down();

    dir_i = dir_namei( path, &namelen, (CONST WCHAR**)&basename );
    if ( !dir_i ) // path is invalid
    {
        D( ( DL_FATERROR, "remove dir: dir_i is invalid\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        DSM_ASSERT(0,"remove dir:dir is null.pszDirName = 0x%x",pszDirName);
        goto step0_failed;
    }
    if(0 == namelen) // namelen is 0
    {
        D( ( DL_FATERROR, "remove dir: namelen = %d\n",namelen) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_NO_BASENAME;
        DSM_ASSERT(0,"remove dir:path is null.pszDirName = 0x%x",pszDirName);
        goto step1_failed;
    }

    uDevNo = dir_i->i_dev;

    psDe = DENTRY_ALLOC();
    if (NULL == psDe)
    {
        D( ( DL_FATERROR, "remove dir: malloc psDe failed!.\n") );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_NO_MORE_MEMORY;
        DSM_ASSERT(0,"remove dir:psDe is null.pszDirName = 0x%x",pszDirName);
        goto step1_failed;
    }

    DSM_MemSet( psDe, 0, sizeof( *psDe ) );
    DSM_TcStrCpy( psDe->name, basename);
    psDe->append_data2 = FS_FLAG_NOT_FIND_FILE;
    iRet = fs_find_entry( &dir_i, psDe, FALSE ); // directory not found
    if(-1 == iRet)
    {
        D( ( DL_FATERROR, "remove dir: find_entry error, iRet = %d\n",iRet) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_NO_DIR_ENTRY;
        goto step1_failed;
    }

    if(!FS_IS_DIR( psDe->inode->i_mode )) // Not directory
    {
        D( ( DL_FATERROR, "remove dir: psDe->inode->i_mode = %d\n",psDe->inode->i_mode) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_NOT_DIRECTORY;
        DSM_ASSERT(0,"remove dir:FS_IS_DIR = false.pszDirName = 0x%x",pszDirName);
        goto step2_failed;
    }

    if(psDe->inode->i_dev != dir_i->i_dev)
    {
        D( ( DL_FATERROR, "remove dir: psDe->inode->i_dev = %d, dir_i->i_dev = %d\n",psDe->inode->i_dev,dir_i->i_dev) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        DSM_ASSERT(0,"remove dir:dev error.pszDirName = 0x%x,dev1 = %d,dev2 = %d.",pszDirName,psDe->inode->i_dev,dir_i->i_dev);
        goto step2_failed;
    }
    /*
    After calling find_entry(), its refcnt is increased by one. Its refcnt will be greater than 1 when
    SOME process is accessing the directory. For example, try to delete itself under the directory.
    NOTE: if we run the command "rd ..", the refcnt of the .. inode may be 1, but it will not be
    empty!!!
    */
    if(psDe->inode->i_count > 1) // Not permition
    {
        D( ( DL_FATERROR, "remove dir:psDe->inode->i_count = %d\n",psDe->inode->i_count) );
        //DSM_Dump((UINT8*)pszDirName,iULen,16);
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        //DSM_ASSERT(0,"remove dir:i_count  > 0.pszDirName = 0x%x,i_count = 0x%x.",pszDirName,psDe->inode->i_count);
        goto step2_failed;
    }

    result = fs_do_unlink( dir_i, psDe, FALSE );
    if(result)
    {
        D( ( DL_FATERROR, "in FS_RemoveDir, unlink failed iRet = %d\n",iRet) );
        iRet = fs_set_error_code(result);
        goto step2_failed;
    }

    dir_i->i_mtime = DSM_GetFileTime();
    iRet = ERR_SUCCESS;

step2_failed:
    iput( psDe->inode );

step1_failed:
    iput( dir_i );

    if (psDe != NULL)
    {
        DENTRY_FREE(psDe);
    }

step0_failed:
    if(path != NULL)
    {
        NAME_BUF_FREE(path);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }

    FS_Up();
    return iRet;
}


// This  function changes the size of a file.
// Move the file point to end after change the file size.
INT64 FS_ChangeSize(INT32 fd, UINT64 nFileSize)
{
    struct file *file = NULL;
    struct inode *psInode;
    INT64 iRet = ERR_SUCCESS;
    UINT32 uDevNo = 0;

    if( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_ChangeSize,fd = %d, ERR_FS_INVALID_PARAMETER",fd) );
        return ERR_FS_BAD_FD;
    }

    FS_Down();
    file = g_fs_current.filp[fd];
    if(NULL == file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        D( ( DL_FATERROR, "in FS_ChangeSize, file is NULL\n") );
        iRet = ERR_FS_BAD_FD;
        goto end;
    }

    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto end;
    }

    if (FS_O_RDONLY == ( file->f_flags & FS_O_ACCMODE ) )
    {
        D( ( DL_FATERROR, "FS_ChangeSize, file->f_flags  = %d\n",file->f_flags) );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto end;
    }

    psInode = file->f_inode;
    if (nFileSize == psInode->i_size)
    {
        D( ( DL_WARNING, "in FS_ChangeSize, SizeToChange = %d, fileSize = %d", nFileSize, psInode->i_size) );
        iRet = ERR_SUCCESS;
        goto end;
    }

    if (nFileSize > psInode->i_size)
    {
        iRet = fs_file_extend(file, nFileSize - psInode->i_size);
        if(iRet != _ERR_FAT_SUCCESS)
        {
            D(( DL_FATERROR, "FS_ChangeSize: fs_file_extend() failed, iRet = %d.\n", iRet));
            goto end;
        }
    }
    else
    {
        iRet = fs_file_truncate( file, nFileSize );
        if(iRet != _ERR_FAT_SUCCESS)
        {
            D(( DL_FATERROR, "FS_ChangeSize: fs_file_truncate() failed, iRet = %d.\n", iRet));
            goto end;
        }
        if (file->f_pos > psInode->i_size)
        {
            file->f_pos = psInode->i_size;
        }
    }
    uDevNo = psInode->i_dev;
    iRet = ERR_SUCCESS;
end:
    if(iRet < 0)
    {
        iRet = fs_set_error_code((INT32)iRet);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }
    FS_Up();
    return iRet;

}


INT32 FS_Flush(INT32 fd)
{
    struct file * filp;
    INT32 iRet;

    if( !FD_IS_VALID( fd ) ) // fd is invalid
    {
        return ERR_FS_BAD_FD;
    }

    FS_Down();
    filp = g_fs_current.filp[ fd ];

    if(!filp || !fs_is_file_opened(&g_fs_file_list, filp)) // fd has closed
    {
        iRet = ERR_FS_BAD_FD;
        goto label_exit;
    }

    if(filp->f_flags & FS_O_FIND) // handle is for finding
    {
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto label_exit;
    }

    if(!filp->f_count) // f_count should not be ZERO
    {
        iRet = ERR_FS_BAD_FD;
        goto label_exit;
    }

    iRet = fs_write_inode( filp->f_inode);
    if(iRet != _ERR_FAT_SUCCESS)
    {
        D(( DL_FATERROR, "FS_Flush: fat_write_inode() failed, iRet = %d.\n", iRet));
        goto label_exit;
    }
    else
    {
        iRet = ERR_SUCCESS;
    }

label_exit:
    FS_Up();
    return iRet;
}


INT64 FS_GetFileSize(INT32 fd)
{
    struct file *file;
    struct inode *inode;

    if( !FD_IS_VALID( fd ) )
    {
        D( ( DL_FATERROR, "in FS_GetFileSize,fd is invalid ,= % d\n",fd) );
        return ERR_FS_BAD_FD;
    }
    FS_Down();
    file = g_fs_current.filp[fd];
    if(NULL == file || !fs_is_file_opened(&g_fs_file_list, file))
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileSize,file is NULL\n") );
        return ERR_FS_BAD_FD;
    }

    if(file->f_flags & FS_O_FIND) // handle is for finding
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileSize, handle is for finding\n") );
        return ERR_FS_OPERATION_NOT_GRANTED;
    }
    inode = file->f_inode;
    if ( FS_IS_DIR( inode->i_mode ) )
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileSize,file is not regular \n") );
        return ERR_FS_NOT_REGULAR;

    }
    else
    {
        D(( DL_FATDETAIL, "iSize = %d\n",inode->i_size));
        FS_Up();
        return inode->i_size;
    }
}

#if 0
INT64 FS_GetDirSize1( PCSTR pszFileName,UINT64 *pSize)
{
    WCHAR *path = NULL;
    struct inode *dir_i;
    UINT64 iSize = 0;
    INT32 iRet = ERR_SUCCESS;
    UINT16 iULen = 0;

    iULen = DSM_UnicodeLen((UINT8*)pszFileName);
    if(0 == iULen || iULen > FS_PATH_UNICODE_LEN)
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    // path name character validate, Ignore base name.
    if( !is_file_name_valid((PCWSTR) pszFileName, TRUE ))
    {
        D( ( DL_FATERROR, "in FS_GetDirSize , pathname is invalid") );
        return  ERR_FS_INVALID_PARAMETER;
    }

    FS_Down();

    path = format_dir_name((PCWSTR)pszFileName );
    if(NULL == path) // malloc failed
    {
        D( ( DL_FATERROR, "in FS_GetDirSize, path is NULL" ) );
        iRet = ERR_FS_NO_MORE_MEMORY;
        goto step0_failed;
    }
    dir_i = namei((PCWSTR)pszFileName);
    if(NULL == dir_i)
    {
        D((DL_FATERROR, "FS_GetDirSize, Call namei() return NULL."));
        iRet = ERR_FS_PATHNAME_PARSE_FAILED;
        goto step0_failed;
    }
    iRet = fs_get_dir_size(dir_i,&iSize,TRUE);
    if (0 != iRet )
    {
        D((DL_FATWARNING, "FS_GetDirSize: fat_get_dir_size() return failed.err_code = %d.",iRet));
        iRet = ERR_FS_NO_MORE_FILES;
        goto step1_failed;
    }
    *pSize = iSize;
    iRet = ERR_SUCCESS;
    CSW_TRACE(BASE_DSM_TS_ID,
              "FS_GetDirSize  success. iSize= 0x%x,DevNo = %d.",
              iSize,dir_i->i_dev);
    iput(dir_i);

step1_failed:

    if ( ERR_SUCCESS != iRet )
    {
        iput( dir_i );
    }

step0_failed:

    if(path != NULL)
    {
        DSM_Free(path);
    }
    FS_Up();

    return fs_set_error_code(iRet);
}
#endif

INT32 FS_GetFileName(INT32 hFile, INT32 iNameBufferSize, PSTR pFileName)
{
    struct file * psFile;
    struct inode * psInode;
    WCHAR *pUniName = NULL;
    UINT16 uUniLen;
    INT32 iRet;

    if( !FD_IS_VALID( hFile ) )
    {
        D( ( DL_FATERROR, "in FS_GetFileName, invalid fd = %d\n",hFile) );
        iRet = ERR_FS_BAD_FD;
        return iRet;
    }
    pUniName = NAME_BUF_ALLOC();
    if (!pUniName)
    {
        D( ( DL_FATERROR, "In FS_GetFileName 1. malloc pUniName failed.") );
        DSM_ASSERT(0, "In FS_GetFileName 1. malloc pUniName failed.");
        iRet = ERR_FS_NO_MORE_MEMORY;
        return iRet;
    }

    FS_Down();
    psFile = g_fs_current.filp[ hFile ];
    if (NULL == psFile || !fs_is_file_opened(&g_fs_file_list, psFile))
    {
        D( ( DL_FATERROR, "in FS_GetFileName, psFile is NULL.\n") );
        iRet = ERR_FS_BAD_FD;
        goto failed;
    }
    if(psFile->f_flags & FS_O_FIND) // handle is for finding
    {
        FS_Up();
        D( ( DL_FATERROR, "in FS_GetFileName, handle is for finding\n") );
        iRet = ERR_FS_OPERATION_NOT_GRANTED;
        goto failed;
    }
    psInode = psFile->f_inode;

    iRet = (UINT32)fs_get_file_name( psInode,  pUniName);
    if(_ERR_FAT_SUCCESS == iRet)
    {
        //DSM_MemZero(pUniName, FAT_MAX_LONG_PATH_LEN + 2);
        uUniLen = DSM_UnicodeLen((UINT8*)pUniName);
        if ((UINT32)(uUniLen + LEN_FOR_NULL_CHAR) > iNameBufferSize / SIZEOF(WCHAR))
        {
            iRet = ERR_FS_NAME_BUFFER_TOO_SHORT;
            goto failed;
        }
        DSM_TcStrCpy((PCWSTR)pFileName, pUniName);
        iRet = ERR_SUCCESS;
    }
    else
    {
        D( ( DL_FATERROR, "fat_get_file_name error, errorCode = %d.\n", iRet) );
        iRet = fs_set_error_code(iRet);
    }

failed:
    if (pUniName)
    {
        NAME_BUF_FREE(pUniName);
    }
    FS_Up();
    return iRet;
}

#ifdef _T_UPGRADE_PROGRAMMER

INT32 FS_PowerOn(VOID)
{
    INT32 iResult;

    iResult = fs_global_var_init();
    if (iResult != ERR_SUCCESS)
    {
        D((DL_FATERROR, "FS_PowerOn: fs_global_var_init() failed, errorCode = %d.", iResult));
        return iResult;
    }

    if (FS_INVALID_SEM_HANDLE == g_fs_sem)
    {
        g_fs_sem = sxr_NewSemaphore(1);
    }

    return ERR_SUCCESS;
}

#else

INT32 FS_PowerOn(VOID)
{
    INT32 iResult;
    INT32 iRet = ERR_SUCCESS;
    UINT32 i;

    iResult = fs_global_var_init();
    if (iResult != ERR_SUCCESS)
    {
        D((DL_FATERROR, "FS_PowerOn: fs_global_var_init() failed, errorCode = %d.", iResult));
        return iResult;
    }

    if (FS_INVALID_SEM_HANDLE == g_fs_sem)
    {
        g_fs_sem = sxr_NewSemaphore(1);
    }

    for(i = 0; i < g_fs_cfg.uNrSuperBlock; i++)
    {
        if(g_pFsDevInfo[i].dev_type != FS_DEV_TYPE_FLASH)
        {
            continue;
        }

        iResult = fs_scan_disk((UINT8*)g_pFsDevInfo[i].dev_name);
        if (ERR_SUCCESS == iResult)
        {
            D( ( DL_FATDETAIL, "scan device[%s] success.\n",g_pFsDevInfo[i].dev_name) );
        }
        else
        {
            D( ( DL_FATERROR, "scan device[%s] failing. error code = %d.\n",g_pFsDevInfo[i].dev_name,iResult) );
            // iRet = iResult;
        }
    }
    return iRet;
}

#endif

INT32 FS_ScanDisk(PCSTR pszDevName,UINT8 iFsType)
{
    INT32 iRet = ERR_SUCCESS;
    INT32 iResult;

    FS_Down();
    iResult = fs_scan_disk((UINT8*)pszDevName);
    if(_ERR_FAT_SUCCESS == iResult)
    {
        iRet = ERR_SUCCESS;
    }
    else
    {
        iRet = fs_set_error_code(iResult);
    }
    FS_Up();
    return iRet;
}

INT32 FS_PowerOff(VOID)
{
    if (FS_INVALID_SEM_HANDLE != g_fs_sem)
    {
        sxr_FreeSemaphore(g_fs_sem);
        g_fs_sem = FS_INVALID_SEM_HANDLE;
    }
    return fs_global_var_free();
}


INT32 FS_ReleaseDev(PCSTR pszDevName)
{
    UINT32 uDevNo;
    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_ReleaseDev DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in FS_ReleaseDev DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        FS_Down();
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
        FS_Up();
    }
    return ERR_SUCCESS;
}

INT64 FS_Truncate(INT32 fd, UINT64 nFileSize)
{
    return  FS_ChangeSize(fd,nFileSize);
}

INT32  FS_GetDeviceInfo(UINT32* pDeviceCount,FS_DEV_INFO** ppFSDevInfo)
{
    if(NULL == pDeviceCount ||
            NULL == ppFSDevInfo)
    {
        return ERR_FS_INVALID_PARAMETER;
    }
    *ppFSDevInfo = g_pFsDevInfo;
    *pDeviceCount = g_fs_cfg.uNrSuperBlock;
    return ERR_SUCCESS;
}

INT32 FS_GetDeviceType(INT32 fd, FS_DEV_TYPE* pDevType)
{
    UINT32 uDevNo;
    struct file *file;
    file = g_fs_current.filp[fd];
    uDevNo = file->f_inode->i_dev;
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_GetDevType DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    *pDevType = DSM_GetDevType(uDevNo);
    return ERR_SUCCESS;
}

UINT32  FS_GetVersion(VOID)
{
    return ((FS_VFS_VERSION << 16) | FAT_VER);
}

UINT8* g_fsDumpBuff = NULL;
INT32 FS_Dump(PCSTR pszDevName, UINT32 iBegSec,UINT32 iSectCount,PCSTR pszFileName)
{
    UINT32 uDevNo = 0;
    UINT32 iBlockSize;
    UINT32 iBlockCount;
    INT32 iResult;
    INT32 iRet;
    UINT32 i;
    UINT8 szBuff[512];
    INT32 fd = -1;
    UINT32 iDumpNumber = 0;
    UINT8* pDumpBuff = NULL;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    iRet = ERR_SUCCESS;
#if 1
    sxr_Sleep(30);
    CSW_TRACE(BASE_DSM_TS_ID,
              "FS_Dump devname = %s, iBegSec = 0x%x, iSectCount = 0x%x.\n",
              pszDevName,
              iBegSec,
              iSectCount
             );
    sxr_Sleep(30);
#endif
    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_Dump DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in FS_Dump DevNo is invalid,uDevNo = %x0x\n", uDevNo ) );
        iRet = ERR_FS_DEVICE_NOT_REGISTER;
        goto step0_failed;
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        iRet = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iRet )
        {
            D( ( DL_FATERROR, "in FS_Dump device[%s]: Open t-flash memory failed.\n", pszDevName ) );
            iRet = ERR_FS_OPEN_DEV_FAILED;
            goto step0_failed;
        }
    }

    CSW_TRACE(BASE_DSM_TS_ID, "in FS_Dump Call DRV_GET_DEV_INFO  uDevNo = %d.\n", uDevNo);
    iResult = DRV_GET_DEV_INFO(uDevNo,&iBlockCount,&iBlockSize);
    if(ERR_SUCCESS != iResult)
    {
        D( ( DL_FATERROR,
             "in FS_Dump DRV_GET_DEV_INFO  failed!,iResult = %d.\n",
             iResult));
        iRet = ERR_FS_GET_DEV_INFO_FAILED;
        goto step1_failed;
    }
    else
    {
        D( ( DL_FATDETAIL,
             "in FS_Dump DRV_GET_DEV_INFO  success!,iBlockCount = %d,iBlockSize = %d.\n",
             iBlockCount,
             iBlockSize));
        if(iBegSec + iSectCount > iBlockCount)
        {
            D( ( DL_FATERROR,
                 "in FS_Dump   parameter error! iBeginSec = 0x%x, iEndSec = 0x%x.\n",
                 iResult));
        }
        if(pszFileName)
        {
            fd = FS_Open( pszFileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
            if(fd < 0)
            {
                D( ( DL_FATERROR,
                     "in FS_Dump   Open file failed!fd = %d.\n",
                     fd));
                iRet = iResult;
                goto step0_failed;
            }
            for(i = iBegSec; i < iBegSec + iSectCount; i++)
            {
                iResult = DRV_BLOCK_READ(uDevNo,i,szBuff);
                if(iResult != ERR_SUCCESS)
                {
                    D( ( DL_FATERROR,
                         "in FS_Dump   read block failed! uDevNo = 0x%x, block no = 0x%x,err code = %d.\n",
                         uDevNo,
                         i,
                         iResult));
                    iRet = ERR_FS_READ_SECTOR_FAILED;
                    break;
                }
                iResult = (INT32)FS_Write(fd,szBuff,DEFAULT_SECSIZE);
                if(iResult != DEFAULT_SECSIZE)
                {
                    D( ( DL_FATERROR,
                         "in FS_Dump   write file failed! uDevNo = 0x%x, sector no = 0x%x,err code = %d.\n",
                         uDevNo,
                         i,
                         iResult)
                     );
                    iRet = ERR_FS_WRITE_SECTOR_FAILED;
                    break;
                }
#if 1
                // sxr_Sleep(5);
                CSW_TRACE(BASE_DSM_TS_ID, "dump sector no = %d.\n",i);
                DSM_HstSendEvent(i);
                //sxr_Sleep(5);
#endif
                iDumpNumber ++;
            }
        }
        else
        {
            pDumpBuff = DSM_MAlloc(iSectCount*512);
            if(NULL == pDumpBuff)
            {
                DSM_ASSERT(0,"FS_Dump: 1.malloc(0x%x) failed.",(iSectCount*512));
            }
            for(i = iBegSec; i < iBegSec + iSectCount; i++)
            {
                iResult = DRV_BLOCK_READ(uDevNo,i,szBuff);
                if(iResult != ERR_SUCCESS)
                {
                    D( ( DL_FATERROR,
                         "in FS_Dump   read block failed! uDevNo = 0x%x, block no = 0x%x,err code = %d.\n",
                         uDevNo,
                         i,
                         iResult));
                    iRet = ERR_FS_READ_SECTOR_FAILED;
                    break;
                }
                DSM_MemCpy(pDumpBuff,szBuff,512);
#if 1
                // sxr_Sleep(30);
                CSW_TRACE(BASE_DSM_TS_ID, "dump sector no = %d.\n",i);
                //sxr_Sleep(30);
#endif
                iDumpNumber ++;
            }
            g_fsDumpBuff = pDumpBuff;

        }
        iRet = ERR_SUCCESS;
    }

step1_failed:
step0_failed:
    if(fd >= 0)
    {
        FS_Close(fd);
    }
    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        if(!fs_is_opened(uDevNo))
        {
            DRV_DEV_DEACTIVE(uDevNo);
        }
    }

#if 1
    sxr_Sleep(30);
    CSW_TRACE(BASE_DSM_TS_ID, "FS_Dump Finish,dump sector count = %d.\n",
              iDumpNumber);

    CSW_TRACE(BASE_DSM_TS_ID, "g_fsDumpBuff = 0x%x,iSectCount = 0x%x.",
              g_fsDumpBuff,iSectCount);
    DSM_HstSendEvent(0xffffeeee);
    DSM_HstSendEvent(iDumpNumber);
    sxr_Sleep(30);
#endif
    return iRet;
}

// Function:FS_IsValidFileName
// Chech the file name if valid.It is allowable that with full path.
// This function will check the per directory name or file name.it is't allowable that include wildcard character in the directory,
// the last name include wildcard character is allowed when input parameter bIgnoreBaseName is TRUE.
// Parameter:
// @pszFileName: The path name in unicode format.
// @bIgnoreBaseName: The flag of ignore base name. if it is TRUE, allow filename is NULL or include wildcard characters,else
//            not allow.
// Return
//   The path name is valid,return TRUE.
//   The path name is invalid, return FALSE.
BOOL FS_IsValidFileName( PCSTR pszFileName, BOOL bIgnoreBaseName )
{
    return is_file_name_valid((PCWSTR)pszFileName,bIgnoreBaseName);
}


INT32 FS_GetVolLab(PCSTR pszDevName, UINT8* szVolLab)
{
    UINT32 uDevNo = 0;
    INT32 iResult;
    INT32 iRet;
    struct super_block *sb;
    UINT8 iFsType;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_GetVolLab DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in FS_GetVolLab DevNo is invalid, uDevNo = 0x%x.\n", uDevNo ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        iRet = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iRet )
        {
            D( ( DL_FATERROR, "check FS_GetVolLab or not[%s]: Open t-flash memory device failed.\n", pszDevName ) );
            return ERR_FS_OPEN_DEV_FAILED;
        }
    }

    iResult = fs_find_sb(uDevNo, &sb);
    if(ERR_SUCCESS != iResult)  // file system has been mounted
    {
        CSW_TRACE(BASE_DSM_TS_ID, "should mount first, uDevNo = 0x%x.\n", uDevNo);
        return _ERR_FAT_NOT_SUPPORT;
    }

    iFsType = sb->s_fstype;
    switch(iFsType)
    {
        case FS_TYPE_FAT:
            FS_Down();
            iResult = fat_get_vollab(uDevNo, (UINT8*)szVolLab, sb);
            if(iResult != ERR_SUCCESS)
            {
                D( ( DL_FATERROR, "fat_get_vollab fail.") );
            }
            else
            {
                CSW_TRACE(BASE_DSM_TS_ID, "fat_get_vollab success! szVolLab = %s",szVolLab);
            }
            FS_Up();
            if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
            {
                if(!fs_is_opened(uDevNo))
                {
                    DRV_DEV_DEACTIVE(uDevNo);
                }
            }
            iRet = fs_set_error_code(iResult);
            break;
#ifdef FS_SUPPORT_EXFAT
        case FS_TYPE_EXFAT:
            FS_Down();
            iRet = exfat_get_vollab(uDevNo, szVolLab, sb);
            if(iRet != ERR_SUCCESS)
            {
                D( ( DL_FATERROR, "exfat_get_vollab fail.") );
            }
            else
            {
                CSW_TRACE(BASE_DSM_TS_ID, "exfat_get_vollab success! szVolLab = %s",szVolLab);
            }
            FS_Up();
            if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
            {
                if(!fs_is_opened(uDevNo))
                {
                    DRV_DEV_DEACTIVE(uDevNo);
                }
            }
            iRet = fs_set_error_code(iResult);
            break;
#endif
        default:
            iRet = ERR_FS_UNKNOWN_FILESYSTEM;
            break;
    }
    return iRet;
}


INT32 FS_SetVolLab(PCSTR pszDevName, PCSTR szVolLab)
{
    UINT32 uDevNo = 0;
    INT32 iResult;
    INT32 iRet;
    struct super_block *sb;
    UINT8 iFsType;

    if ( !pszDevName )
    {
        return ERR_FS_INVALID_PARAMETER;
    }

    uDevNo = DSM_DevName2DevNo(pszDevName);
    if(INVALID_DEVICE_NUMBER == uDevNo)
    {
        D( ( DL_FATERROR, "in FS_SetVolLab DevNo is invalid, pszDevName = %s\n", pszDevName) );
        D( ( DL_FATERROR, "in FS_SetVolLab DevNo is invalid, uDevNo = 0x%x.\n", uDevNo ) );
        return ERR_FS_DEVICE_NOT_REGISTER;
    }

    if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
    {
        iRet = DRV_DEV_ACTIVE(uDevNo);
        if( ERR_SUCCESS != iRet )
        {
            D( ( DL_FATERROR, "check FS_SetVolLab or not[%s]: Open t-flash memory device failed.\n", pszDevName ) );
            return ERR_FS_OPEN_DEV_FAILED;
        }
    }
    if(DSM_StrLen(szVolLab) > 11)
    {
        D( ( DL_FATERROR, "The volume label must be less than 11 characters!") );
        return ERR_FS_INVALID_PARAMETER;
    }

    iResult = fs_find_sb(uDevNo, &sb);
    if(ERR_SUCCESS != iResult)  // file system has been mounted
    {
        CSW_TRACE(BASE_DSM_TS_ID, "should mount first, uDevNo = 0x%x.\n", uDevNo);
        return ERR_FS_NOT_SUPPORT;
    }

    iFsType = sb->s_fstype;
    switch(iFsType)
    {
        case FS_TYPE_FAT:
            FS_Down();
            iResult = fat_set_vollab(uDevNo, (PCSTR)szVolLab, sb);
            if(iResult != ERR_SUCCESS)
            {
                D( ( DL_FATERROR, "fat_set_vollab fail.") );
            }
            else
            {
                CSW_TRACE(BASE_DSM_TS_ID, "fat_set_vollab success.");
            }
            FS_Up();
            if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
            {
                if(!fs_is_opened(uDevNo))
                {
                    DRV_DEV_DEACTIVE(uDevNo);
                }
            }
            iRet = fs_set_error_code(iResult);
            break;
#ifdef FS_SUPPORT_EXFAT
        case FS_TYPE_EXFAT:
            FS_Down();
            iResult = exfat_set_vollab(uDevNo, (PCSTR)szVolLab, sb);
            if(iResult != ERR_SUCCESS)
            {
                D( ( DL_FATERROR, "exfat_set_vollab fail.") );
            }
            else
            {
                CSW_TRACE(BASE_DSM_TS_ID, "exfat_set_vollab success.");
            }
            FS_Up();
            if(DSM_GetDevType(uDevNo) == DSM_MEM_DEV_TFLASH)
            {
                if(!fs_is_opened(uDevNo))
                {
                    DRV_DEV_DEACTIVE(uDevNo);
                }
            }
            iRet = fs_set_error_code(iResult);
            break;
#endif
        default:
            iRet = ERR_FS_UNKNOWN_FILESYSTEM;
            break;
    }
    return iRet;
}


INT64 FS_GetDirSize(PCSTR pszFileName,UINT64 *pSize)
{
    typedef enum _get_dir_status
    {
        STATUS_INIT,
        STATUS_ROOT_DIR,
        STATUS_PARENTS_DIR,
        STATUS_SUB_DIR,
        STATUS_FILE,
        STATUS_END,
        STATUS_ERROR,
    } GET_DIR_STATUS;
    typedef struct _dir_info
    {
        // UINT32 clus;                           // Cluster number.
        // UINT16 sec;                              // sector number.
        // UINT16 eoff;                           // dir enrty offset.
        INT32 fd;
        FS_FIND_DATA* find_data;
    } DIR_INFO;
    // struct inode *l_inode = NULL;
    UINT8 *name_filter = NULL;
    UINT8 *name_slash = NULL;
    UINT8 *name_point = NULL;
    UINT32 uni_len = 0;
    WCHAR *tempname = NULL;
    FS_FIND_DATA * pfind_data = NULL;
    DIR_INFO dir_info[FS_NR_DIR_LAYERS_MAX];
    GET_DIR_STATUS scan_state = STATUS_INIT;
    INT32 i;
    INT32 d = 0;
    UINT64 size = 0;
    INT32 result;
    INT32 ret = ERR_FS_NO_DIR_ENTRY;

    DSM_MemSet((UINT8*)dir_info,0,sizeof(DIR_INFO)*FS_NR_DIR_LAYERS_MAX);
    for(i = 0; i < FS_NR_DIR_LAYERS_MAX; i ++)
    {
        dir_info[i].fd = -1;
        dir_info[i].find_data = NULL;
    }
    tempname = NAME_BUF_ALLOC();
    if(tempname == NULL)
    {
        DSM_ASSERT(0,"finame malloc fail.");
    }
    result = DSM_OEM2Uincode((const UINT8*)"/*",2,&name_filter,&uni_len,NULL);
    if(result != ERR_SUCCESS)
    {
        DSM_ASSERT(0,"DSM_OEM2Uincode(//*) error.");
    }
    result = DSM_OEM2Uincode((const UINT8*)"/",1,&name_slash,&uni_len,NULL);
    if(result != ERR_SUCCESS)
    {
        DSM_ASSERT(0,"DSM_OEM2Uincode(//*) error.");
    }
    result = DSM_OEM2Uincode((const UINT8*)".",2,&name_point,&uni_len,NULL);
    if(result != ERR_SUCCESS)
    {
        DSM_ASSERT(0,"DSM_OEM2Uincode(.) error.");
    }
    while(d >= 0)
    {
        switch(scan_state)
        {
            case STATUS_INIT:
                d = 0;
                scan_state = STATUS_ROOT_DIR;
                break;
            case STATUS_ROOT_DIR:
                if(d != 0)
                {
                    DSM_ASSERT(0, "d = %d in scan_state STATUS_ROOT_DIR.",d);
                }
                if(dir_info[d].fd == -1)
                {
                    pfind_data = DSM_MAlloc(sizeof(FS_FIND_DATA));
                    if(pfind_data == NULL)
                    {
                        DSM_Assert(0, "find_data malloc fail.");
                    }
                    result = FS_FindFirstFile(pszFileName, pfind_data);
                    if(result >= 0)
                    {
                        dir_info[d].fd = result;
                        dir_info[d].find_data = pfind_data;
                        if(FS_IS_DIR(dir_info[d].find_data->st_mode))
                        {
                            if(DSM_MemCmp(name_point,dir_info[d].find_data->st_name,DSM_UnicodeLen(name_point)) == 0)
                            {
                                scan_state = STATUS_SUB_DIR;
                            }
                            else
                            {
                                scan_state = STATUS_SUB_DIR;
                            }
                        }
                        else
                        {
                            DSM_ASSERT(0,"pszFileName(0x%x) not a directory!",(UINT32)pszFileName);
                        }
                        // size += dir_info[d].find_data->st_size;
                        d ++;
                    }
                    else if(result == ERR_FS_NO_MORE_FILES
                            ||result == ERR_FS_NO_DIR_ENTRY)
                    {
                        DSM_Free(pfind_data);
                        DSM_ASSERT(0,"pszFileName(0x%x) not exsit!",(UINT32)pszFileName);
                        scan_state = STATUS_ERROR;
                    }
                    else
                    {
                        DSM_Free(pfind_data);
                        scan_state = STATUS_ERROR;
                    }
                }
                else
                {
                    DSM_Free(dir_info[d].find_data);
                    dir_info[d].find_data = NULL;
                    result = FS_FindClose(dir_info[d].fd);
                    if(result == ERR_SUCCESS)
                    {
                        dir_info[d].fd = -1;
                        scan_state = STATUS_END;
                    }
                    else
                    {
                        scan_state = STATUS_ERROR;
                    }
                }
                break;
            case STATUS_PARENTS_DIR:
                if(d == 0)
                {
                    scan_state = STATUS_ROOT_DIR;
                }
                else if(dir_info[d].fd == -1)
                {
                    DSM_ASSERT(0,"scan_state:STATUS_PARENTS_DIR!");
                }
                else
                {
                    result = FS_FindNextFile(dir_info[d].fd, dir_info[d].find_data);
                    if(result  == ERR_SUCCESS)
                    {
                        if(FS_IS_DIR(dir_info[d].find_data->st_mode))
                        {
                            if(DSM_MemCmp(name_point,dir_info[d].find_data->st_name,
                                          DSM_UnicodeLen(name_point)) == 0)
                            {
                                scan_state = STATUS_FILE;
                            }
                            else
                            {
                                scan_state = STATUS_SUB_DIR;
                            }
                        }
                        else
                        {
                            size += dir_info[d].find_data->st_size;
                            scan_state = STATUS_FILE;
                        }
                        d ++;
                    }
                    else if(result == ERR_FS_NO_MORE_FILES)
                    {
                        DSM_Free(dir_info[d].find_data);
                        dir_info[d].find_data = NULL;
                        result = FS_FindClose(dir_info[d].fd);
                        if(result == ERR_SUCCESS)
                        {
                            scan_state = STATUS_PARENTS_DIR;
                        }
                        else
                        {
                            scan_state = STATUS_ERROR;
                        }
                        dir_info[d].fd = -1;
                        d --;
                    }
                    else
                    {
                        DSM_Free(dir_info[d].find_data);
                        dir_info[d].find_data = NULL;
                        FS_FindClose(dir_info[d].fd);
                        dir_info[d].fd = -1;
                        scan_state = STATUS_ERROR;
                    }
                }
                break;
            case STATUS_SUB_DIR:
                if(dir_info[d].fd == -1)
                {
                    DSM_TcStrCpy(tempname,(PWSTR)pszFileName);
                    for(i = 1; i < d; i++)
                    {
                        DSM_TcStrCat(tempname,(PCWSTR)name_slash);
                        DSM_TcStrCat(tempname,(PCWSTR)dir_info[i].find_data->st_name);
                    }
                    DSM_TcStrCat(tempname,(PCWSTR)name_filter);
                    pfind_data = (FS_FIND_DATA*)DSM_MAlloc(sizeof(FS_FIND_DATA));
                    if(pfind_data == NULL)
                    {
                        DSM_ASSERT(0, "find_data malloc fail.");
                    }
                    result = FS_FindFirstFile((PCSTR)tempname, pfind_data);
                    if(result >= 0)
                    {
                        dir_info[d].fd = result;
                        dir_info[d].find_data = pfind_data;
                        if(FS_IS_DIR(dir_info[d].find_data->st_mode))
                        {
                            if(DSM_MemCmp(name_point,dir_info[d].find_data->st_name,
                                          DSM_UnicodeLen(name_point)) == 0)
                            {
                                scan_state = STATUS_FILE;
                            }
                            else
                            {
                                scan_state = STATUS_SUB_DIR;
                            }
                        }
                        else
                        {
                            scan_state = STATUS_FILE;
                            size += dir_info[d].find_data->st_size;
                        }
                        d ++;
                    }
                    else if(result == ERR_FS_NO_MORE_FILES)
                    {
                        DSM_Free(pfind_data);
                        d --;
                        scan_state = STATUS_PARENTS_DIR;
                    }
                    else
                    {
                        DSM_Free(pfind_data);
                        scan_state = STATUS_ERROR;
                    }
                }
                else
                {
                    DSM_ASSERT(0,"scan_state:STATUS_SUB_DIR!");
                }
                break;
            case STATUS_FILE:
                d--;
                scan_state = STATUS_PARENTS_DIR;
                break;
            case STATUS_END:
                *pSize = size;
                ret = ERR_SUCCESS;
                d = -1;
                break;
            case STATUS_ERROR:
                for(i = 0; i < FS_NR_DIR_LAYERS_MAX; i ++)
                {
                    if(dir_info[i].fd >= 0)
                    {
                        FS_FindClose(dir_info[i].fd);
                        dir_info[i].fd = -1;
                    }
                    if(dir_info[i].find_data)
                    {
                        DSM_Free(dir_info[i].find_data);
                        dir_info[i].find_data = NULL;
                    }
                }
                ret = fs_set_error_code(result);
                d = -1;
                break;
            default:
                DSM_ASSERT(0,"scan_state:default(%d)!",scan_state);
                break;
        }
    }
    if(tempname)
    {
        NAME_BUF_FREE(tempname);
    }
    if(name_filter)
    {
        DSM_Free(name_filter);
    }
    if(name_slash)
    {
        DSM_Free(name_slash);
    }
    if(name_point)
    {
        DSM_Free(name_point);
    }
    return ret;
}


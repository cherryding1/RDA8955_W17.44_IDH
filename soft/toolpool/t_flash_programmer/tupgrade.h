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

#ifndef _TUPGRADE_H_
#define _TUPGRADE_H_
// tupgrade.h
// error code.
#define ERR_T_UP_TO_UNICODE_FAILED         -1001
#define ERR_T_UP_TO_LOCAL_FAILED             -1002
#define ERR_T_UP_OPEN_FILE_FAILED            -1003
#define ERR_T_UP_READ_FILE_FAILED            -1004
#define ERR_T_UP_WRITE_FILE_FAILED          -1005
#define ERR_T_UP_FILE_ERROR                        -1006
#define ERR_T_UP_READ_FLASH_FAILED         -1007
#define ERR_T_UP_WRITE_FLASH_FAILED       -1008
#define ERR_T_UP_ERASE_FLASH_FAILED       -1009
#define ERR_T_UP_MALLOC_FAILED                 -1010
#define ERR_T_UP_NOT_EXIST_LOD                 -1011
#define ERR_T_UP_NOT_EXIST_CFG                 -1012
#define ERR_T_UP_CALIB_ADDR_ERROR           -1013


// Structure.
typedef struct _t_up_file_list
{
    UINT8 szFile[FS_FILE_NAME_UNICODE_LEN + LEN_FOR_UNICODE_NULL_CHAR];
} T_UP_FILE_LIST;

typedef void (*PF_T_UP_SHOWPROGRASS)(UINT32 progress);

// function.
INT32 T_UP_FsInit();
INT32 T_UP_Main(PF_T_UP_SHOWPROGRASS pFProgress);
INT32 T_UP_GetFileList(UINT8* pFind, T_UP_FILE_LIST* pFileList,UINT32 uCount);

INT32 T_UP_Verify(void);
#endif

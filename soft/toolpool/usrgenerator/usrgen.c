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

#ifdef _USERGEN

#include "dsm_cf.h"

#include "ml.h"
#include "fs.h"
#include "usrgen.h"
#include "sms_dm.h"
#ifdef __MMI_NEW_NVRAM_INTERFACE__
#include "nvram.h"
#endif
#ifdef __MMI_R_WATCH__
#include "rwatchSetting.h"
#endif

#include "nvram_enum_defs.h"
// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

extern UINT32 vds_CRC32(CONST VOID *pvData, INT32 iLen);
extern UINT32 ML_Init();
extern UINT32 ML_SetCodePage(UINT8 nCharset[12]);

#ifndef CHIP_IS_MODEM
extern const kal_uint8 NVRAM_SETTING_DEFAULT[NVRAM_SETTING_SIZE];

kal_uint8 NVRAM_EF_ZERO_DEFAULT_TOOLS[78 /*sizeof(CFW_PBK_ENTRY_INFO)*/ * (500 + 1)];
kal_uint8 NVRAM_EF_FF_DEFAULT_TOOLS[NVRAM_EF_EXT_MELODY_INFO_SIZE];

extern PHB_CALLER_GROUP_STRUCT const NVRAM_PHB_CALLER_GROUPS_DEFAULT[MAX_PB_CALLER_GROUPS];
extern const kal_uint8 NVRAM_CACHE_BYTE_DEFAULT[NVRAM_CACHE_SIZE];

#ifdef RWAPMMS_SUPPORT
extern nvram_wap_profile_content_struct const NVRAM_EF_WAP_PROFILE_CONTENT_DEFAULT[NVRAM_WAP_PROFILE_MAX];
extern nvram_mms_profile_content_struct const NVRAM_EF_MMS_PROFILE_CONTENT_DEFAULT[NVRAM_MMS_PROFILE_MAX];
extern nvram_wap_profile_name_struct const NVRAM_EF_WAP_PROFILE_NAMES_DEFAULT[NVRAM_WAP_PROFILE_MAX];
extern nvram_mms_profile_name_struct const NVRAM_EF_MMS_PROFILE_NAMES_DEFAULT[NVRAM_MMS_PROFILE_MAX];
#endif

#ifdef __GPRS_MODE__
extern nvram_ef_abm_gprs_profile_struct const NVRAM_EF_ABM_GPRS_PROFILE_DEFAULT[NVRAM_EF_ABM_GPRS_PROFILE_TOTAL];
#endif

extern PROFILE const NVRAM_PROFILES_DEFAULT[MAX_ELEMENTS + USER_DEF_PROFILE_NUM];
extern kal_uint8 const NVRAM_EF_AUTOTEST_DEFAULT[NVRAM_EF_AUTOTEST_SIZE];
extern kal_uint8 const NVRAM_EF_BARCODE_NUM_DEFAULT[];
extern kal_uint8 const NVRAM_PHB_SETTINGS_DEFAULT[NVRAM_PHB_SETTINGS_SIZE];

#ifdef __IP_NUMBER__
extern kal_uint8 const NVRAM_IP_NUMBER_DEFAULT[NVRAM_IP_NUMBER_SIZE];
#endif
#ifdef __MMI_MESSAGES_CB__
extern kal_uint8 const NVRAM_EF_ADP_SMS_CB_IMSI_DEFAULT[];
extern kal_uint16 const NVRAM_EF_ADP_SMS_CB_CHANNEL_DEFAULT[];
extern kal_uint8 const NVRAM_EF_ADP_SMS_CB_CHANNEL_MASK_DEFAULT[];
extern kal_uint16 const NVRAM_EF_ADP_SMS_CB_DCS_DEFAULT[];
extern kal_uint8 const NVRAM_EF_ADP_SMS_CB_DCS_MASK_DEFAULT[];
extern kal_uint8 const NVRAM_EF_MMI_SMS_CB_MSG_INFO_DEFAULT[];
extern kal_uint8 const NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_DEFAULT[];
extern kal_uint8 const NVRAM_EF_MMI_SMS_CB_CHANNEL_DEFAULT[];
#endif

USRGEN_MOUNT_POINT g_usrgen_mount_point[] =
    {
        {"vds1", "/home"},
        {"wap_mms", "/wap_mms"},
};
#else
USRGEN_MOUNT_POINT g_usrgen_mount_point[] =
    {
        {"vds1", "/home"},
};
#endif
// =============================================================================
//  FUNCTIONS
// =============================================================================

INT32 usrgen_fs_init(void)
{
    UINT8 i = 0;
    INT32 err_code = 0x0;
    UINT32 fs_dev_count = 0;
    FS_DEV_INFO *fs_dev_info = NULL;
    UINT32 fs_root_dev_count = 0;
    UINT32 dev_nr;
    UINT8 *uni_file = NULL;
    UINT32 uni_len = 0;

    ML_Init();
    ML_SetCodePage((UINT8 *)"CP936");
    DRV_FlashPowerUp();
    DRV_FlashInit();
    err_code = VDS_Init();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(USERGEN_TS_ID, "VDS_Init() OK.\n");
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, "VDS_Init() ERROR, Error code: %d.\n", err_code);
        return -1;
    }
    err_code = DSM_DevInit();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(USERGEN_TS_ID, "DSM_DevInit OK.\n");
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, "DSM_DevInit ERROR, Error code: %d. \n", err_code);
        return -2;
    }
#if 0
    err_code = REG_Init();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(USERGEN_TS_ID, TSTXT("REG_Init() OK.\n"));
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, TSTXT("REG_Init() Fail!Error code:%d.\n"), err_code);
        return -3;
    }

    err_code = SMS_DM_Init();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(USERGEN_TS_ID, TSTXT("SMS_DM_Init OK.\n"));
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, TSTXT("SMS_DM_Init ERROR, Error code: %d.\n"), err_code);
        return -4;
    }
#endif
    err_code = FS_PowerOn();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(USERGEN_TS_ID, "FS Power On Check OK.\n");
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, "FS Power On Check ERROR, Error code: %d. \n", err_code);
        return -5;
    }

    // Get FS device table.
    err_code = FS_GetDeviceInfo(&fs_dev_count, &fs_dev_info);
    if (err_code != ERR_SUCCESS)
    {
        CSW_TRACE(USERGEN_TS_ID, "Device not register.\n");
        fs_dev_count = 0;
        return -6;
    }

    // Check the state of root device ,if not format, format it.
    // Mount root device.
    for (i = 0; i < fs_dev_count; i++)
    {
        // format the flash device.
        if (fs_dev_info[i].dev_type == FS_DEV_TYPE_FLASH)
        {
            err_code = FS_HasFormatted(fs_dev_info[i].dev_name, FS_TYPE_FAT);

            if (ERR_FS_HAS_FORMATED == err_code)
            {
                CSW_TRACE(USERGEN_TS_ID, "The flash device %s has formated.\n", fs_dev_info[i].dev_name);
            }
            else if (ERR_FS_NOT_FORMAT == err_code)
            {

                CSW_TRACE(USERGEN_TS_ID, "The flash device %s not format.\n", fs_dev_info[i].dev_name);
                err_code = FS_Format(fs_dev_info[i].dev_name, FS_TYPE_FAT, 0);
                if (ERR_SUCCESS == err_code)
                {
                    CSW_TRACE(USERGEN_TS_ID, "The flash device %s format ok.\n", fs_dev_info[i].dev_name);
                }
                else if (ERR_FS_NOT_FORMAT == err_code)
                {
                    CSW_TRACE(USERGEN_TS_ID, "The flash device %s format error.Error code:%d.\n", fs_dev_info[i].dev_name, err_code);
                    return -8;
                }
            }
            else
            {
                CSW_TRACE(USERGEN_TS_ID, "The flash device %s has formated error.Error code:%d.\n", fs_dev_info[i].dev_name, err_code);
                return -9;
            }

            // Mount root device.
            if (TRUE == fs_dev_info[i].is_root)
            {
                if (fs_root_dev_count > 0)
                {
                    CSW_TRACE(USERGEN_TS_ID, "The FS root device too more:%d.\n", fs_root_dev_count);
                    return -10;
                }
                else
                {
                    err_code = FS_MountRoot(NULL);
                    if (ERR_SUCCESS == err_code)
                    {
                        CSW_TRACE(USERGEN_TS_ID, "FS MountRoot(%s) OK.\n", fs_dev_info[i].dev_name);
                        fs_root_dev_count++;
                    }
                    else
                    {
                        CSW_TRACE(USERGEN_TS_ID, "FS MountRoot(%s) ERROR, Error code: %d. \n", fs_dev_info[i].dev_name, err_code);
                        DSM_ASSERT(0, "mount root failed.err_code = %d", err_code);
                        return -11;
                    }
                }
            }
            else
            {
                for (dev_nr = 0; dev_nr < sizeof(g_usrgen_mount_point) / sizeof(USRGEN_MOUNT_POINT); dev_nr++)
                {
                    if (strcasecmp((const char *)g_usrgen_mount_point[dev_nr].dev_name, (const char *)fs_dev_info[i].dev_name) == 0)
                    {
                        err_code = DSM_OEM2Uincode((UINT8 *)g_usrgen_mount_point[dev_nr].point, strlen((char *)g_usrgen_mount_point[dev_nr].point), &uni_file, &uni_len, NULL);
                        if (ERR_SUCCESS == err_code)
                        {
                            CSW_TRACE(USERGEN_TS_ID, TSTXT("DSM_OEM2Uincode(%s) ok.\n"), g_usrgen_mount_point[dev_nr].point);
                        }
                        else
                        {
                            CSW_TRACE(USERGEN_TS_ID, TSTXT("DSM_OEM2Uincode(%s) failed. err_code = %d.\n"), g_usrgen_mount_point[dev_nr].point, err_code);
                            continue;
                        }

                        err_code = FS_MakeDir(uni_file, 0);
                        if (ERR_SUCCESS == err_code)
                        {
                            CSW_TRACE(USERGEN_TS_ID, TSTXT("FS_MakeDir(%s) ok.\n"), g_usrgen_mount_point[dev_nr].point);
                        }
                        else
                        {
                            CSW_TRACE(USERGEN_TS_ID, TSTXT("FS_MakeDir(%s) failed,err_code = %d.\n"), g_usrgen_mount_point[dev_nr].point, err_code);
                        }

                        err_code = FS_Mount(fs_dev_info[i].dev_name, uni_file, 0, FS_TYPE_FAT);
                        if (ERR_SUCCESS == err_code)
                        {
                            CSW_TRACE(USERGEN_TS_ID, TSTXT("FS_Mount(%s) ok.\n"), fs_dev_info[i].dev_name);
                        }
                        else
                        {
                            CSW_TRACE(USERGEN_TS_ID, TSTXT("FS_Mount(%s) failed. err_code = %d.\n"), fs_dev_info[i].dev_name, err_code);
                        }
                        DSM_Free(uni_file);
                        uni_file = NULL;
                        uni_len = 0;
                        break;
                    }
                }
                // todo mount.
            }
        }
    }
#ifdef __MMI_NEW_NVRAM_INTERFACE__
    err_code = NVRAM_Init();
    if (ERR_SUCCESS == err_code)
    {
        CSW_TRACE(USERGEN_TS_ID, TSTXT("NVRAM_Init OK.\n"));
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, TSTXT("NVRAM_Init ERROR, Error code: %d.\n"), err_code);
        return -5;
    }
#else
    {
        UINT8 pDirName[] = {'/', 0, 'N', 0, 'V', 0, 'R', 0, 'A', 0, 'M', 0, 0, 0};

        err_code = FS_MakeDir(pDirName, 0);
        if (ERR_SUCCESS == err_code)
        {
            CSW_TRACE(USERGEN_TS_ID, TSTXT("Make directory nvram OK.\n"));
        }
        else
        {
            CSW_TRACE(USERGEN_TS_ID, TSTXT("Make directory nvram error, Error code: %d.\n"), err_code);
            return -5;
        }
    }
#endif
    return 0;
}

INT32 usrgen_wap_mms(void)
{
    //todo: Create and write files for wap mms.
    return 0;
}
#ifdef USER_DATA_SUPPORT_PREFAB
USRGEN_NODE g_prefab_node[FS_NR_DIR_LAYERS_MAX];

INT32 usrgen_make_dest_file_path(USRGEN_NODE *prefab_node, UINT32 cur_deep, char *cur_name, char *path)
{
    UINT32 i;
    char file_name[FS_FILE_NAME_LEN + 1];
    UINT8 *uni_name = NULL;
    UINT32 uni_len = 0;
    INT32 result;

    strcpy(file_name, PREFAB_DIAG);
    for (i = 0; i < cur_deep; i++)
    {
        strcat(file_name, PREFAB_DIAG);
        strcat(file_name, g_prefab_node[i].dir_name);
    }

    strcat(file_name, PREFAB_DIAG);
    strcat(file_name, cur_name);
    //CSW_TRACE(USERGEN_TS_ID,"in usrgen_make_dest_file_path, path = %s cur_deep = %d.", file_name,cur_deep);
    result = DSM_OEM2Uincode((UINT8 *)file_name, strlen(file_name), &uni_name, &uni_len, NULL);
    if (0 == result)
    {
        DSM_TcStrCpy((PWSTR)path, (PWSTR)uni_name);
        DSM_Free(uni_name);
        return 0;
    }
    else
    {
        CSW_TRACE(USERGEN_TS_ID, "in usrgen_make_dest_file_name, DSM_OEM2Uincode failed,result = %d.", result);
        return 1;
    }
}

INT32 usrgen_make_res_file_path(USRGEN_NODE *prefab_node, UINT32 cur_deep, char *cur_name, char *path)
{
    UINT32 i;

    strcpy(path, PREFAB_ROOT);
    for (i = 0; i < cur_deep; i++)
    {
        strcat(path, PREFAB_DIAG);
        strcat(path, g_prefab_node[i].dir_name);
    }
    if (cur_name)
    {
        strcat(path, PREFAB_DIAG);
        strcat(path, cur_name);
    }
    return 0;
}
#endif

INT32 usrgen_prefab_file(void)
{
#ifdef USER_DATA_SUPPORT_PREFAB
    INT32 cur_deep = 0;
    USRGEN_REFAB_STATUS pefab_status;
    char res_path[FS_FILE_NAME_LEN + 1];
    char res_dir_path[FS_FILE_NAME_LEN + 1];
    char dest_path[FS_FILE_NAME_UNICODE_LEN + 2];

    UINT8 buff[512];
    INT32 read_size;
    INT32 writen_size;
    UINT8 is_node;
    INT32 result;
    INT32 ret;
    FILE *res_fd;
    INT32 dest_fd;
    UINT32 size;

    struct dirent *entry;
    struct stat statbuf;
    UINT8 *oem_name = NULL;
    UINT32 oem_len = 0;

    ret = 0;
    pefab_status = USRGEN_STATUS_INIT;
    cur_deep = 0;
    is_node = 0;
    do
    {
        entry = NULL;
        is_node = 0;
        switch (pefab_status)
        {
        case USRGEN_STATUS_INIT:
        case USRGEN_STATUS_NEW:
            memset(res_dir_path, 0, (FS_FILE_NAME_LEN + 1));
            usrgen_make_res_file_path(g_prefab_node, cur_deep, NULL, res_dir_path);
            pefab_status = USRGEN_STATUS_FIRST;
            break;
        case USRGEN_STATUS_FIRST:
            g_prefab_node[cur_deep].dp = opendir(res_dir_path);
            if (g_prefab_node[cur_deep].dp != NULL)
            {
                //chdir(res_path);
                pefab_status = USRGEN_STATUS_NEXT;
            }
            else
            {
                pefab_status = USRGEN_STATUS_LAST;
            }
            break;
        case USRGEN_STATUS_NEXT:
            entry = readdir(g_prefab_node[cur_deep].dp);
            if (entry != NULL)
            {
                lstat(entry->d_name, &statbuf);
                if (strncmp(".", entry->d_name, 1) == 0)
                {
                }
                else
                {
                    is_node = 1;
                }
            }
            else
            {
                is_node = 0;
                pefab_status = USRGEN_STATUS_LAST;
            }

            break;
        case USRGEN_STATUS_LAST:
            if (g_prefab_node[cur_deep].dp)
            {
                closedir(g_prefab_node[cur_deep].dp);
                g_prefab_node[cur_deep].dp = NULL;
            }
            else
            {
                CSW_TRACE(USERGEN_TS_ID, "error.in usrgen_prefab_file, dp  = NULL,cur_deep = %d.", cur_deep);
            }
            //CSW_TRACE(USERGEN_TS_ID,"in usrgen_prefab_file, at last entry.cur_deep = %d.",cur_deep);
            cur_deep--;
            pefab_status = USRGEN_STATUS_NEXT;
            break;
        case USRGEN_STATUS_EXIT:
        //CSW_TRACE(USERGEN_TS_ID,"in usrgen_prefab_file, at USRGEN_STATUS_EXIT.cur_deep = %d.",cur_deep);
        case USRGEN_STATUS_ERROR:
            //CSW_TRACE(USERGEN_TS_ID,"in usrgen_prefab_file, at USRGEN_STATUS_ERROR.cur_deep = %d.",cur_deep);
            cur_deep = -1;
            break;

        default:
            break;
        }

        if (1 == is_node)
        {
            result = usrgen_make_res_file_path(g_prefab_node, cur_deep, entry->d_name, res_path);
            if (result != 0)
            {
                CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, usrgen_make_res_file_path failed.result  = %d.", result);
                ret = -1;
                pefab_status = USRGEN_STATUS_ERROR;
                continue;
            }
            else
            {
                CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file,res_path = %s,cur_deep = %d.", res_path, cur_deep);
            }

            result = usrgen_make_dest_file_path(g_prefab_node, cur_deep, entry->d_name, dest_path);
            if (0 != result)
            {
                CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, usrgen_make_dest_file_path failed.result  = %d.", result);
                ret = -2;
                pefab_status = USRGEN_STATUS_ERROR;
                continue;
            }
            else
            {
                result = DSM_Unicode2OEM((UINT8 *)dest_path, (UINT32)DSM_UnicodeLen((UINT8 *)dest_path), &oem_name, &oem_len, NULL);
                if (0 == result)
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file,dest_path = %s.", oem_name);
                    DSM_Free(oem_name);
                }
                else
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, DSM_Unicode2OEM failed,result = %d.", result);
                }
            }

            lstat(res_path, &statbuf);
            if (S_ISDIR(statbuf.st_mode))
            {
                result = FS_MakeDir((PCSTR)dest_path, 0);
                if (result == ERR_SUCCESS ||
                    result == ERR_FS_FILE_EXIST)
                {
                    strcpy(g_prefab_node[cur_deep].dir_name, entry->d_name);
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file,FS_MakeDir ok.dir_name = %s,cur_deep = %d.", entry->d_name, cur_deep);
                    cur_deep++;
                    //chdir(entry->d_name);
                    pefab_status = USRGEN_STATUS_NEW;
                }
                else
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file,FS_MakeDir failed.dir_name = %s,cur_deep = %d,result = %d.", entry->d_name, cur_deep, result);
                    ret = -3;
                    pefab_status = USRGEN_STATUS_ERROR;
                    continue;
                }
            }
            else
            {
                dest_fd = FS_Create((PCSTR)dest_path, 0);
                if (dest_fd >= 0)
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file,FS_Create ok.dir_name = %s,cur_deep = %d.", entry->d_name, cur_deep);
                }
                else
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, FS_Create failed.filename = %s,dest_fd  = %d.", entry->d_name, dest_fd);
                    ret = -4;
                    pefab_status = USRGEN_STATUS_ERROR;
                    continue;
                }

                res_fd = fopen(res_path, "rb");
                if (NULL == res_fd)
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, fopen failed.res_fd  = %d.", res_fd);
                    ret = -5;
                    pefab_status = USRGEN_STATUS_ERROR;
                    continue;
                }
                else
                {
                    CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, fopen(%s) ok.res_fd  = 0x%x.", res_path, res_fd);
                }
                size = 0;
                do
                {
                    read_size = fread(buff, 1, 512, res_fd);
                    if (read_size > 0)
                    {
                        writen_size = FS_Write(dest_fd, buff, read_size);
                        if (writen_size != read_size)
                        {
                            ret = -6;
                            CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, FS_Write failed.dest_fd  = %d,read_size = %d,writen_size = %d.", dest_fd, read_size, writen_size);
                            break;
                        }
                        size += writen_size;
                    }
                } while (read_size > 0);

                CSW_TRACE(USERGEN_TS_ID, "in usrgen_prefab_file, write size = 0x%x.", size);
                fclose(res_fd);
                FS_Close(dest_fd);
            }
        }

    } while (cur_deep >= 0);

    return ret;
#else
    return 0;
#endif
}
#ifndef CHIP_IS_MODEM
#ifdef __MMI_NEW_NVRAM_INTERFACE__
INT32 usrgen_nvram(void)
{

    INT32 handle = -1;
    UINT32 nvram_id = 0;
    UINT16 nRecordId = 0;
    UINT16 nSimNumber = NUMBER_OF_SIM;
    char pStr[MAX_SIM_SETTING_PASSWD_LEN + 1] = "1234\0";

    memset(NVRAM_EF_FF_DEFAULT_TOOLS, 0xff, sizeof(NVRAM_EF_FF_DEFAULT_TOOLS));

#ifdef __MMI_MESSAGES_CB__
    for (nRecordId = 1; nRecordId <= NVRAM_EF_TWO_RECORD; nRecordId++)
    {
        nvram_id = NVRAM_EF_ADP_SMS_CB_IMSI_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_ADP_SMS_CB_IMSI_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_ADP_SMS_CB_IMSI_SIZE, (UINT8 *)NVRAM_EF_ADP_SMS_CB_IMSI_DEFAULT);
        NVRAM_Close(handle);
    }

    for (nRecordId = 1; nRecordId <= NVRAM_EF_TWO_RECORD; nRecordId++)
    {
        nvram_id = NVRAM_EF_ADP_SMS_CB_DCS_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_ADP_SMS_CB_DCS_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_ADP_SMS_CB_DCS_SIZE, (UINT8 *)NVRAM_EF_ADP_SMS_CB_DCS_DEFAULT);
        NVRAM_Close(handle);
    }

    for (nRecordId = 1; nRecordId <= NVRAM_EF_TWO_RECORD; nRecordId++)
    {
        nvram_id = NVRAM_EF_ADP_SMS_CB_DCS_MASK_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_ADP_SMS_CB_DCS_MASK_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_ADP_SMS_CB_DCS_MASK_SIZE, (UINT8 *)NVRAM_EF_ADP_SMS_CB_DCS_MASK_DEFAULT);
        NVRAM_Close(handle);
    }

    for (nRecordId = 1; nRecordId <= NVRAM_EF_TWO_RECORD; nRecordId++)
    {
        nvram_id = NVRAM_EF_ADP_SMS_CB_CHANNEL_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_ADP_SMS_CB_CHANNEL_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_ADP_SMS_CB_CHANNEL_SIZE, (UINT8 *)NVRAM_EF_ADP_SMS_CB_CHANNEL_DEFAULT);
        NVRAM_Close(handle);
    }

    for (nRecordId = 1; nRecordId <= NVRAM_EF_TWO_RECORD; nRecordId++)
    {
        nvram_id = NVRAM_EF_ADP_SMS_CB_CHANNEL_MASK_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_ADP_SMS_CB_CHANNEL_MASK_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_ADP_SMS_CB_CHANNEL_MASK_SIZE, (UINT8 *)NVRAM_EF_ADP_SMS_CB_CHANNEL_MASK_DEFAULT);
        NVRAM_Close(handle);
    }

    for (nRecordId = 1; nRecordId <= NVRAM_EF_MMI_SMS_CB_CHANNEL_RECORD_TOTAL; nRecordId++)
    {
        nvram_id = NVRAM_EF_MMI_SMS_CB_CHANNEL_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_MMI_SMS_CB_CHANNEL_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_MMI_SMS_CB_CHANNEL_SIZE, (UINT8 *)NVRAM_EF_MMI_SMS_CB_CHANNEL_DEFAULT);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_CHIS_NUM_LID
    for (nRecordId = 1; nRecordId <= NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_NUM_TOTAL; nRecordId++)
    {
        nvram_id = NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE, (UINT8 *)NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_DEFAULT);
        NVRAM_Close(handle);
    }

#ifdef __MMI_MULTI_SIM__
    for (nRecordId = 1; nRecordId <= NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_NUM_TOTAL; nRecordId++)
    {
        nvram_id = NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_SIM1_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE, (UINT8 *)NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_DEFAULT);
        NVRAM_Close(handle);
    }
#if (MMI_SIM_NUMBER > 2)
    for (nRecordId = 1; nRecordId <= NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_NUM_TOTAL; nRecordId++)
    {
        nvram_id = NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_SIM2_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE, (UINT8 *)NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_DEFAULT);
        NVRAM_Close(handle);
    }
    for (nRecordId = 1; nRecordId <= NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_NUM_TOTAL; nRecordId++)
    {
        nvram_id = NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_SIM3_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_SIZE, (UINT8 *)NVRAM_EF_MMI_SMS_CB_MSG_CONTENT_PAGE_DEFAULT);
        NVRAM_Close(handle);
    }
#endif
#endif
    for (nRecordId = 1; nRecordId <= NVRAM_SMS_CB_SMS_INFO_TOTAL; nRecordId++)
    {
        nvram_id = NVRAM_EF_MMI_SMS_CB_MSG_INFO_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_MMI_SMS_CB_MSG_INFO_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_MMI_SMS_CB_MSG_INFO_SIZE, (UINT8 *)NVRAM_EF_MMI_SMS_CB_MSG_INFO_DEFAULT);
        NVRAM_Close(handle);
    }
#endif

    //NVRAM_EF_PHB_SETTINGS_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_PHB_SETTINGS_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, (NVRAM_PHB_SETTINGS_SIZE * NVRAM_PHB_SETTINGS_TOTAL));
        NVRAM_SetValue(handle, 0, (NVRAM_PHB_SETTINGS_SIZE * NVRAM_PHB_SETTINGS_TOTAL), (UINT8 *)NVRAM_PHB_SETTINGS_DEFAULT);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_PHB_IDS_LID
    for (nRecordId = 1; nRecordId < ((PHONE_PHB_ENTRY_COUNT + NUMBER_OF_SIM * (MAX_PHB_SIM_ENTRY)) / (85)) + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_PHB_IDS_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_PHB_IDS_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_PHB_IDS_SIZE, (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_PHB_CALLER_GROUPS_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_PHB_CALLER_GROUPS_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, (MAX_PB_CALLER_GROUPS * sizeof(PHB_CALLER_GROUP_STRUCT)));
        NVRAM_SetValue(handle, 0, (MAX_PB_CALLER_GROUPS * sizeof(PHB_CALLER_GROUP_STRUCT)), (UINT8 *)NVRAM_PHB_CALLER_GROUPS_DEFAULT);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_PROFILES_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_PROFILES_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, (NVRAM_PROFILES_RECORD_TOTAL * NVRAM_PROFILES_RECORD_SIZE));
        NVRAM_SetValue(handle, 0, (NVRAM_PROFILES_RECORD_TOTAL * NVRAM_PROFILES_RECORD_SIZE), (UINT8 *)NVRAM_PROFILES_DEFAULT);
        NVRAM_Close(handle);
    }

#ifdef __MMI_SUPPORT_BLUETOOTH__
    //NVRAM_EF_BT_INFO_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_BT_INFO_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_BT_INFO_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_BT_INFO_SIZE, (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }
#endif

    //NVRAM_EF_ALM_QUEUE_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_ALM_QUEUE_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_ALM_QUEUE_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_ALM_QUEUE_SIZE, (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_AUTOTEST_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_AUTOTEST_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, (NVRAM_EF_AUTOTEST_SIZE * NVRAM_EF_AUTOTEST_TOTAL));
        NVRAM_SetValue(handle, 0, (NVRAM_EF_AUTOTEST_SIZE * NVRAM_EF_AUTOTEST_TOTAL), (UINT8 *)NVRAM_EF_AUTOTEST_DEFAULT);
        NVRAM_Close(handle);
    }

#ifdef __IP_NUMBER__
    //NVRAM_EF_IP_NUMBER_LID,
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_IP_NUMBER_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, (NVRAM_IP_NUMBER_SIZE * NVRAM_IP_NUMBER_TOTAL));
        NVRAM_SetValue(handle, 0, (NVRAM_IP_NUMBER_SIZE * NVRAM_IP_NUMBER_TOTAL), (UINT8 *)NVRAM_IP_NUMBER_DEFAULT);
        NVRAM_Close(handle);
    }
#endif

//NVRAM_EF_EXT_MELODY_INFO_LID
#ifdef __MMI_PROFILE_EXTMELODY_SUPPORT__
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_EXT_MELODY_INFO_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_EXT_MELODY_INFO_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_EXT_MELODY_INFO_SIZE, (UINT8 *)NVRAM_EF_FF_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }
#endif

//NVRAM_EF_PHB_VCARD_LID
#ifdef __MMI_VCARD__
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_PHB_VCARD_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_PHB_VCARD_LID_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_PHB_VCARD_LID_SIZE, (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }
#endif

//NVRAM_EF_AUDIO_EQUALIZER_LID
#ifdef __MMI_SOUND_EFFECT__
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_AUDIO_EQUALIZER_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_AUDIO_EQUALIZER_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_EF_AUDIO_EQUALIZER_SIZE, (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }
#endif

//NVRAM_EF_WAP_PROFILE_CONTENT_LID
#ifdef RWAPMMS_SUPPORT_WAP
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_WAP_PROFILE_CONTENT_TOTAL; nRecordId++)
        {
            nvram_id = (NVRAM_EF_WAP_PROFILE_CONTENT_LID + nSimNumber) * 100 + nRecordId;
            handle = NVRAM_Create(nvram_id, NVRAM_EF_WAP_PROFILE_CONTENT_SIZE);
            NVRAM_SetValue(handle, 0, NVRAM_EF_WAP_PROFILE_CONTENT_SIZE, (UINT8 *)&NVRAM_EF_WAP_PROFILE_CONTENT_DEFAULT[nRecordId - 1]);
            NVRAM_Close(handle);
        }

    //NVRAM_EF_WAP_PROFILE_NAMES_LID

    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_WAP_PROFILE_NAMES_TOTAL; nRecordId++)
        {
            nvram_id = (NVRAM_EF_WAP_PROFILE_NAMES_LID + nSimNumber) * 100 + nRecordId;
            handle = NVRAM_Create(nvram_id, NVRAM_EF_WAP_PROFILE_NAMES_SIZE);
            NVRAM_SetValue(handle, 0, NVRAM_EF_WAP_PROFILE_NAMES_SIZE, (UINT8 *)&NVRAM_EF_WAP_PROFILE_NAMES_DEFAULT[nRecordId - 1]);
            NVRAM_Close(handle);
        }
#endif

//NVRAM_EF_MMS_PROFILE_CONTENT_LID
#ifdef RWAPMMS_SUPPORT_MMS
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_MMS_PROFILE_CONTENT_TOTAL; nRecordId++)
        {
            nvram_id = (NVRAM_EF_MMS_PROFILE_CONTENT_LID + nSimNumber) * 100 + nRecordId;
            handle = NVRAM_Create(nvram_id, NVRAM_EF_MMS_PROFILE_CONTENT_SIZE);
            NVRAM_SetValue(handle, 0, NVRAM_EF_MMS_PROFILE_CONTENT_SIZE, (UINT8 *)&NVRAM_EF_MMS_PROFILE_CONTENT_DEFAULT[nRecordId - 1]);
            NVRAM_Close(handle);
        }

    //NVRAM_EF_MMS_PROFILE_NAMES_LID

    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_MMS_PROFILE_NAMES_TOTAL; nRecordId++)
        {
            nvram_id = (NVRAM_EF_MMS_PROFILE_NAMES_LID + nSimNumber) * 100 + nRecordId;
            handle = NVRAM_Create(nvram_id, NVRAM_EF_MMS_PROFILE_NAMES_SIZE);
            NVRAM_SetValue(handle, 0, NVRAM_EF_MMS_PROFILE_NAMES_SIZE, (UINT8 *)&NVRAM_EF_MMS_PROFILE_NAMES_DEFAULT[nRecordId - 1]);
            NVRAM_Close(handle);
        }
#endif

//NVRAM_EF_ABM_GPRS_PROFILE_LID
#ifdef __GPRS_MODE__
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
    //for(nRecordId = 1; nRecordId < 1 + NVRAM_EF_ABM_GPRS_PROFILE_TOTAL;nRecordId++)
    {
        nRecordId = 1;
        nvram_id = (NVRAM_EF_ABM_GPRS_PROFILE_LID + nSimNumber) * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_ABM_GPRS_PROFILE_SIZE * NVRAM_EF_ABM_GPRS_PROFILE_TOTAL);
        NVRAM_SetValue(handle, 0, NVRAM_EF_ABM_GPRS_PROFILE_SIZE * NVRAM_EF_ABM_GPRS_PROFILE_TOTAL, (UINT8 *)&NVRAM_EF_ABM_GPRS_PROFILE_DEFAULT[nRecordId - 1]);
        NVRAM_Close(handle);
    }

#endif

//NVRAM_EF_SHORTCUTS_LID
#ifdef SHORTCUTS_APP
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_SHORTCUTS_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_SHORTCUTS_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_SHORTCUTS_SIZE, (UINT8 *)NVRAM_EF_FF_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }
#endif

    //NVRAM_EF_SETTING_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_SETTING_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_SETTING_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_SETTING_SIZE, (UINT8 *)NVRAM_SETTING_DEFAULT);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_CACHE_BYTE_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_CACHE_BYTE_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, NVRAM_CACHE_SIZE);
        NVRAM_SetValue(handle, 0, NVRAM_CACHE_SIZE, (UINT8 *)NVRAM_CACHE_BYTE_DEFAULT);
        NVRAM_Close(handle);
    }

    //NVRAM_PHB_NVRAM_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_PHB_NVRAM_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, 78 /*sizeof(CFW_PBK_ENTRY_INFO)*/ * (PHONE_PHB_ENTRY_COUNT + 1));
        NVRAM_SetValue(handle, 0, 78 /*sizeof(CFW_PBK_ENTRY_INFO)*/ * (PHONE_PHB_ENTRY_COUNT + 1), (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_EF_CHIS_NUM_LID
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
    {
        nvram_id = NVRAM_EF_CHIS_NUM_LID * 100 + nSimNumber;
        handle = NVRAM_Create(nvram_id, NVRAM_EF_CHIS_NUM_TOTAL);
        NVRAM_SetValue(handle, 0, NVRAM_EF_CHIS_NUM_SIZE, (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_DIALEDCALL_NVRAM_LID
    for (nRecordId = 1; nRecordId < NUMBER_OF_SIM + 1; nRecordId++)
    {
        nvram_id = NVRAM_DIALEDCALL_NVRAM_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, sizeof(mmi_adp_calllog_data_struct));
        NVRAM_SetValue(handle, 0, sizeof(mmi_adp_calllog_data_struct), (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_RECVDCALL_NVRAM_LID
    for (nRecordId = 1; nRecordId < NUMBER_OF_SIM + 1; nRecordId++)
    {
        nvram_id = NVRAM_RECVDCALL_NVRAM_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, sizeof(mmi_adp_calllog_data_struct));
        NVRAM_SetValue(handle, 0, sizeof(mmi_adp_calllog_data_struct), (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_MISSEDCALL_NVRAM_LID
    for (nRecordId = 1; nRecordId < NUMBER_OF_SIM + 1; nRecordId++)
    {
        nvram_id = NVRAM_MISSEDCALL_NVRAM_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, sizeof(mmi_adp_calllog_data_struct));
        NVRAM_SetValue(handle, 0, sizeof(mmi_adp_calllog_data_struct), (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_REJECTEDCALL_NVRAM_LID
    for (nRecordId = 1; nRecordId < NUMBER_OF_SIM + 1; nRecordId++)
    {
        nvram_id = NVRAM_REJECTEDCALL_NVRAM_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, sizeof(mmi_adp_calllog_data_struct));
        NVRAM_SetValue(handle, 0, sizeof(mmi_adp_calllog_data_struct), (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS);
        NVRAM_Close(handle);
    }

    //NVRAM_PHONELOCK_NVRAM_LID
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_PHONELOCK_NVRAM_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, (MAX_SIM_SETTING_PASSWD_LEN + 1));
        NVRAM_SetValue(handle, 0, (MAX_SIM_SETTING_PASSWD_LEN + 1), (UINT8 *)pStr);
        NVRAM_Close(handle);
    }
#ifdef __MMI_R_WATCH_SERVICE__
    extern rwatch_setup_info NVRAM_EF_RWATCH_SETTING_DEFAULT;
    for (nRecordId = 1; nRecordId < 1 + 1; nRecordId++)
    {
        nvram_id = NVRAM_EF_RWATCH_SETTING_LID * 100 + nRecordId;
        handle = NVRAM_Create(nvram_id, sizeof(rwatch_setup_info));
        NVRAM_SetValue(handle, 0, sizeof(rwatch_setup_info), (UINT8 *)&NVRAM_EF_RWATCH_SETTING_DEFAULT);
        NVRAM_Close(handle);
    }
#endif

    return 0;
}

#else //__MMI_NEW_NVRAM_INTERFACE__

#define NVRAM_CRC
#ifdef NVRAM_CRC
#define NVRAM_CR_STR "NVRM"
#define NVRAM_CRC_STR_LEN 4
#define NVRAM_CRC_CODE_LEN 4
#define NVRAM_CRC_FILE_CODE_LEN 4
#define NVRAM_CRC_INFO_LEN (NVRAM_CRC_STR_LEN + NVRAM_CRC_CODE_LEN)

extern void WriteNvramCRCInfo(INT32 filehandle);
#endif
#define NVRAM_DEFAULT_FOLDER "NVRAM"

U8 UnicodeToUCS2Encoding(U16 unicode, U8 *charLength, U8 *arrOut)
{

    U8 status = ST_SUCCESS;
    U8 index = 0;

    if (arrOut != 0)
    {

        if (unicode < 256)
        {
            arrOut[index++] = *((U8 *)(&unicode));
            arrOut[index] = 0;
        }
        else
        {
            arrOut[index++] = *((U8 *)(&unicode));
            arrOut[index] = *(((U8 *)(&unicode)) + 1);
        }
        *charLength = 2;
    }
    else
    {

        status = ST_FAILURE;
    }

#ifdef __FOR_TESTING //MMI_ON_HARDWARE_P
    if (arrOut != 0)
    {

        if (unicode < 256)
        {
            arrOut[index++] = 0; /* *((U8*)(&unicode));*/
            arrOut[index] = *((U8 *)(&unicode));
        }
        else
        {
            arrOut[index++] = *(((U8 *)(&unicode)) + 1); /* *((U8*)(&unicode));*/
            arrOut[index] = *((U8 *)(&unicode));         /* *(((U8*)(&unicode)) + 1);*/
        }
        *charLength = 2;
    }
    else
    {

        status = ST_FAILURE;
    }
#endif

    return status;
}

U16 AnsiiToUnicodeString(S8 *pOutBuffer, S8 *pInBuffer)
{

    S16 count = -1;
    U8 charLen = 0;
    U8 arrOut[2];

    while (*pInBuffer != '\0')
    {

        UnicodeToUCS2Encoding(((U16)*pInBuffer) & 0x00ff, &charLen, arrOut);

        //#ifdef MMI_ON_WIN32
        pOutBuffer[++count] = arrOut[0];
        pOutBuffer[++count] = arrOut[1];
        pInBuffer++;
//#endif

#ifdef __FOR_TESTING //MMI_ON_HARDWARE_P
        pOutBuffer[++count] = arrOut[1]; /*arrOut[0];*/
        pOutBuffer[++count] = arrOut[0]; /* arrOut[1];*/
        pInBuffer++;
#endif
    }

    pOutBuffer[++count] = '\0';
    pOutBuffer[++count] = '\0';
    return count + 1;
}

void WriteNvramCRCInfo(INT32 filehandle)
{
    INT32 ret = 0;
    INT32 fs_crc_code = 0;
    UINT32 file_len = 0;
    UINT32 crc_len = NVRAM_CRC_FILE_CODE_LEN;
    S8 *filecontent = NULL;

    if (filehandle < 0)
        return;

    file_len = (INT32)FS_GetFileSize(filehandle);
    CSW_TRACE(USERGEN_TS_ID, " func : %s file_len = 0x%x.", __FUNCTION__, file_len);

    if (file_len < NVRAM_CRC_INFO_LEN)
    {
        return;
    }

    crc_len = file_len - NVRAM_CRC_INFO_LEN;

    filecontent = (S8 *)malloc(crc_len + NVRAM_CRC_INFO_LEN);
    if (filecontent == NULL)
        return;

    ret = FS_Seek(filehandle, 0, FS_SEEK_SET);
    if (ret >= 0)
    {
        FS_Read(filehandle, (UINT8 *)filecontent, (crc_len + NVRAM_CRC_INFO_LEN));
    }

    ret = FS_Seek(filehandle, 0, FS_SEEK_SET);
    CSW_TRACE(USERGEN_TS_ID, " func : %s seek ret  = 0x%x", __FUNCTION__, ret);
    if (ret >= 0)
    {
        ret = FS_Write(filehandle, (UINT8 *)NVRAM_CR_STR, NVRAM_CRC_STR_LEN);

        fs_crc_code = vds_CRC32((CONST VOID *)(filecontent + NVRAM_CRC_INFO_LEN), crc_len);
        ret = FS_Write(filehandle, (UINT8 *)&fs_crc_code, NVRAM_CRC_CODE_LEN);
        if (ret != NVRAM_CRC_CODE_LEN)
        {
            CSW_TRACE(USERGEN_TS_ID, " func : %s fs_write failed.ret  = %d.", __FUNCTION__, ret);
        }
    }

    if (filecontent != NULL)
    {
        free(filecontent);
    }

    CSW_TRACE(USERGEN_TS_ID, " func : %s ret  = %d, fs_crc_code = 0x%x.", __FUNCTION__, ret, fs_crc_code);
}

BOOL write_nvram_default_value(U16 nFileId, U16 nRecordId, U16 nRecordId_cnt, UINT8 *pBuffer, U16 nBufferSize)
{
    INT32 handle = -1;
    char fileName[100];
    S8 name_uncode[100];
#ifdef NVRAM_CRC
    S8 crc_zero[NVRAM_CRC_INFO_LEN] = {0};
#endif
    U16 i;

    for (i = nRecordId; i < nRecordId_cnt + nRecordId; i++)
    {
        memset(fileName, 0x00, sizeof(fileName));
        memset(name_uncode, 0x00, sizeof(name_uncode));

        sprintf(fileName, "/%s/NVRAM_%d_%d_FILE.dat", NVRAM_DEFAULT_FOLDER, nFileId, i);

        AnsiiToUnicodeString(name_uncode, fileName);

        handle = FS_Create((PCSTR)name_uncode, 0);
#ifdef NVRAM_CRC
        FS_Write(handle, (UINT8 *)crc_zero, NVRAM_CRC_INFO_LEN);
#endif
        FS_Write(handle, (UINT8 *)pBuffer, nBufferSize);

#ifdef NVRAM_CRC
        WriteNvramCRCInfo(handle);
#endif

        FS_Close(handle);
    }
    return TRUE;
}

INT32 usrgen_nvram(void)
{
#ifdef RWAPMMS_SUPPORT_WAP
    UINT16 nRecordId = 1;
#endif
    UINT16 nSimNumber = NUMBER_OF_SIM;
    char pStr[MAX_SIM_SETTING_PASSWD_LEN + 1] = "1234\0";

    memset(NVRAM_EF_FF_DEFAULT_TOOLS, 0xff, sizeof(NVRAM_EF_FF_DEFAULT_TOOLS));

    //NVRAM_EF_PHB_IDS_LID
    write_nvram_default_value(
        NVRAM_EF_PHB_IDS_LID,
        1,
        ((PHONE_PHB_ENTRY_COUNT + NUMBER_OF_SIM * (MAX_PHB_SIM_ENTRY)) / (85)),
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        NVRAM_PHB_IDS_SIZE);

    //NVRAM_EF_PHB_CALLER_GROUPS_LID
    write_nvram_default_value(
        NVRAM_EF_PHB_CALLER_GROUPS_LID,
        1,
        1,
        (UINT8 *)NVRAM_PHB_CALLER_GROUPS_DEFAULT,
        (MAX_PB_CALLER_GROUPS * sizeof(PHB_CALLER_GROUP_STRUCT)));
#ifdef __MMI_SUPPORT_BLUETOOTH__
    //NVRAM_EF_BT_INFO_LID
    write_nvram_default_value(
        NVRAM_EF_BT_INFO_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        NVRAM_EF_BT_INFO_SIZE);
#endif
    //NVRAM_EF_ALM_QUEUE_LID
    write_nvram_default_value(
        NVRAM_EF_ALM_QUEUE_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        NVRAM_ALM_QUEUE_SIZE);

//NVRAM_EF_EXT_MELODY_INFO_LID
#ifdef __MMI_PROFILE_EXTMELODY_SUPPORT__
    write_nvram_default_value(
        NVRAM_EF_EXT_MELODY_INFO_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_FF_DEFAULT_TOOLS,
        NVRAM_EF_EXT_MELODY_INFO_SIZE);
#endif

//NVRAM_EF_PHB_VCARD_LID
#ifdef __MMI_VCARD__
    write_nvram_default_value(
        NVRAM_EF_PHB_VCARD_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        NVRAM_PHB_VCARD_LID_SIZE);
#endif

//NVRAM_EF_AUDIO_EQUALIZER_LID
#ifdef __MMI_SOUND_EFFECT__
    write_nvram_default_value(
        NVRAM_EF_AUDIO_EQUALIZER_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        NVRAM_EF_AUDIO_EQUALIZER_SIZE);
#endif

//NVRAM_EF_WAP_PROFILE_CONTENT_LID
#ifdef RWAPMMS_SUPPORT_WAP
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_WAP_PROFILE_CONTENT_TOTAL; nRecordId++)
        {
            write_nvram_default_value(
                NVRAM_EF_WAP_PROFILE_CONTENT_LID + nSimNumber,
                nRecordId,
                1,
                (UINT8 *)&NVRAM_EF_WAP_PROFILE_CONTENT_DEFAULT[nRecordId - 1],
                NVRAM_EF_WAP_PROFILE_CONTENT_SIZE);
        }

#endif

//NVRAM_EF_MMS_PROFILE_CONTENT_LID
#ifdef RWAPMMS_SUPPORT_MMS
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_MMS_PROFILE_CONTENT_TOTAL; nRecordId++)
        {
            write_nvram_default_value(
                NVRAM_EF_MMS_PROFILE_CONTENT_LID + nSimNumber,
                nRecordId,
                1,
                (UINT8 *)&NVRAM_EF_MMS_PROFILE_CONTENT_DEFAULT[nRecordId - 1],
                NVRAM_EF_MMS_PROFILE_CONTENT_SIZE);
        }
#endif

//NVRAM_EF_WAP_PROFILE_NAMES_LID
#ifdef RWAPMMS_SUPPORT_WAP
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_WAP_PROFILE_NAMES_TOTAL; nRecordId++)
        {
            write_nvram_default_value(
                NVRAM_EF_WAP_PROFILE_NAMES_LID + nSimNumber,
                nRecordId,
                1,
                (UINT8 *)&NVRAM_EF_WAP_PROFILE_NAMES_DEFAULT[nRecordId - 1],
                NVRAM_EF_WAP_PROFILE_NAMES_SIZE);
        }
#endif

//NVRAM_EF_MMS_PROFILE_NAMES_LID
#ifdef RWAPMMS_SUPPORT_MMS
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
        for (nRecordId = 1; nRecordId < 1 + NVRAM_EF_MMS_PROFILE_NAMES_TOTAL; nRecordId++)
        {
            write_nvram_default_value(
                NVRAM_EF_MMS_PROFILE_NAMES_LID + nSimNumber,
                nRecordId,
                1,
                (UINT8 *)&NVRAM_EF_MMS_PROFILE_NAMES_DEFAULT[nRecordId - 1],
                NVRAM_EF_WAP_PROFILE_NAMES_SIZE);
        }
#endif

//NVRAM_EF_ABM_GPRS_PROFILE_LID

#if defined(__GPRS_MODE__) && defined(__MMI_GPRS_FEATURES__)
    for (nSimNumber = 0; nSimNumber < NUMBER_OF_SIM; nSimNumber++)
    //for(nRecordId = 1; nRecordId < 1 + NVRAM_EF_ABM_GPRS_PROFILE_TOTAL;nRecordId++)
    {
        nRecordId = 1;
        write_nvram_default_value(
            NVRAM_EF_ABM_GPRS_PROFILE_LID + nSimNumber,
            nRecordId,
            1,
            (UINT8 *)&NVRAM_EF_ABM_GPRS_PROFILE_DEFAULT[nRecordId - 1],
            NVRAM_EF_ABM_GPRS_PROFILE_SIZE * NVRAM_EF_ABM_GPRS_PROFILE_TOTAL);
    }

#endif

//NVRAM_EF_SHORTCUTS_LID
#ifdef SHORTCUTS_APP
    write_nvram_default_value(
        NVRAM_EF_SHORTCUTS_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_FF_DEFAULT_TOOLS,
        NVRAM_SHORTCUTS_SIZE);
#endif

    //NVRAM_EF_SETTING_LID
    write_nvram_default_value(
        NVRAM_EF_SETTING_LID,
        1,
        1,
        (UINT8 *)NVRAM_SETTING_DEFAULT,
        NVRAM_SETTING_SIZE);

    //NVRAM_EF_CACHE_BYTE_LID
    write_nvram_default_value(
        NVRAM_EF_CACHE_BYTE_LID,
        1,
        1,
        (UINT8 *)NVRAM_CACHE_BYTE_DEFAULT,
        NVRAM_CACHE_SIZE);

    //NVRAM_PHB_NVRAM_LID
    write_nvram_default_value(
        NVRAM_PHB_NVRAM_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        78 /*sizeof(CFW_PBK_ENTRY_INFO)*/ * (PHONE_PHB_ENTRY_COUNT + 1));

    //NVRAM_EF_CHIS_NUM_LID
    write_nvram_default_value(
        NVRAM_EF_CHIS_NUM_LID,
        1,
        1,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        NVRAM_EF_CHIS_NUM_SIZE);

    //NVRAM_DIALEDCALL_NVRAM_LID
    write_nvram_default_value(
        NVRAM_DIALEDCALL_NVRAM_LID,
        1,
        nSimNumber,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        sizeof(mmi_adp_calllog_data_struct));

    //NVRAM_RECVDCALL_NVRAM_LID
    write_nvram_default_value(
        NVRAM_RECVDCALL_NVRAM_LID,
        1,
        nSimNumber,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        sizeof(mmi_adp_calllog_data_struct));

    //NVRAM_MISSEDCALL_NVRAM_LID
    write_nvram_default_value(
        NVRAM_MISSEDCALL_NVRAM_LID,
        1,
        nSimNumber,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        sizeof(mmi_adp_calllog_data_struct));

    //NVRAM_REJECTEDCALL_NVRAM_LID
    write_nvram_default_value(
        NVRAM_REJECTEDCALL_NVRAM_LID,
        1,
        nSimNumber,
        (UINT8 *)NVRAM_EF_ZERO_DEFAULT_TOOLS,
        sizeof(mmi_adp_calllog_data_struct));

    //NVRAM_PHONELOCK_NVRAM_LID
    write_nvram_default_value(
        NVRAM_PHONELOCK_NVRAM_LID,
        1,
        1,
        (UINT8 *)pStr,
        sizeof(pStr));

    return 0;
}
#endif // __MMI_NEW_NVRAM_INTERFACE__
#endif //CHIP_IS_MODEM
#endif // _USERGEN

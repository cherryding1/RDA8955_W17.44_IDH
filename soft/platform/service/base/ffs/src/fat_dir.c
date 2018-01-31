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
#include "fat_base.h"
#include "exfat_base.h"
#include "fat_local.h"
#include "exfat_local.h"
#include "fs.h"
#include "fs_base.h"

#include "dsm_dbg.h"

extern INT32 g_TstFsErrCode;

INT32 fat_delete_fatentries(FAT_SB_INFO *sb_info, UINT32 lFstCluLO)
{
    UINT32 curclu = 0, nextClu = 0;
    BOOL bSynchFAT = FALSE;
    INT32 lErrCode = _ERR_FAT_SUCCESS;
    UINT32 i;

    if (lFstCluLO > ((sb_info->iDataSec / sb_info->iSecPerClus) + 2) || lFstCluLO < 2)
    {
        D((DL_FATERROR, "fat_delete_fatentries: lFstCluLO = 0x%x, _ERR_FAT_PARAM_ERROR!\n", lFstCluLO));
        g_TstFsErrCode = 1015;
        return _ERR_FAT_ERROR;
    }
    curclu = lFstCluLO;
    do
    {
        lErrCode = fat_get_next_cluster(sb_info, curclu, &nextClu);
        if (lErrCode != _ERR_FAT_SUCCESS)
        {
            D((DL_FATERROR, "fat_delete_fatentries: fat_get_next_cluster() return %d", lErrCode));
            return lErrCode;
        }
        if(nextClu == 0)
        {
            D((DL_FATERROR, "fat_delete_fatentries: nextClu is zero."));
            bSynchFAT = TRUE;
        }
        if (fat_is_last_cluster(sb_info, nextClu))
        {
            bSynchFAT = TRUE;
        }

        lErrCode = fat_set_fat_entry_value(sb_info, curclu, CLUSTER_FREE, bSynchFAT, NULL);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "in fat_delete_fatentries,fat_set_fat_entry_value failed, ErrorCode = %d \n", lErrCode));
            return lErrCode;
        }

        if(DSM_GetDevType(sb_info->sb->s_dev) == DSM_MEM_DEV_FLASH)
        {
            for(i = 0;  i < sb_info->iSecPerClus; i++)
            {
                lErrCode = fat_revert_cluster( sb_info, curclu, i);
                if(lErrCode != ERR_SUCCESS)
                {
                    D( ( DL_FATERROR, "in fat_delete_fatentries,fat_revert_cluster failed, err_code = %d \n", lErrCode));
                    return lErrCode;
                }
            }
        }
        curclu = nextClu;
    }
    while (!(fat_is_last_cluster(sb_info, curclu)) && nextClu >= 2);
    return _ERR_FAT_SUCCESS;
}


/********************************************************************************/
// Function:
//   Remove the dir entry on the pointed position.
// Parameter:
//      sb_info[in]: super block, just for getting essential information.
//      de [in], the direcotry entry to be remove.
//      bsec[in], the sector on which the 'de' stays.
//      eoff[in], the offsec in the 'bsec' where the 'de' stays.
// Return value:
//      _ERR_FAT_SUCCESS indicates a successful operation.
//
//      And unsuccessful operation is denoted by the err code as following:
//
//      _ERR_FAT_WRITE_SEC_FAILED:  Write the sector failed.
//      _ERR_FAT_READ_SEC_FAILED:   Read the sector failed.
//      _FAT_DIR_NOT_NULL:          The operated dir is not null.
//*******************************************************************************
//
// Operation steps:
//
// 1) Test the type of the entry, regular file or directory
// 2) Remove the content which the 'de' point to according the test result
//      a) regular file, remove it anyway;
//      b) directory, remove it if the directory is NULL(has only '.' and '..'
//          directory), or do not remove it.
// 3) Remove the 'de' itselft
INT32 fat_remove_entry(FAT_SB_INFO *sb_info, struct fat_dir_entry *de,
                       UINT32 bclu, UINT16 bsec, UINT16 eoff, BOOL bRename)
{
    INT32 lErrCode = _ERR_FAT_SUCCESS;
    UINT8 *secbuf = NULL;
    INT32 i;

    FAT_DIR_ENTRY dent;
    UINT32 cluster;
    UINT8  cursec;
    UINT8  ecount;

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D( ( DL_FATERROR, "in fat_remove_fatentry, NULL == secbuf."));
        DSM_ASSERT(0, "in fat_remove_fatentry, NULL == secbuf.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    if(bRename)
    {
        goto delete_entry;
    }

    // The root directory cannot be deleted, all deleted directory items are
	// in the data area, which means that the value of the directory entry
	// must be greater than 2.
	// Note: the deleted directory entry may be in the root directory region.

    cluster = (UINT32)(de->DIR_FstClusLO | (UINT32)(de->DIR_FstClusHI << 16));

    //
    // Step 1) Test the type of the entry, regular file or directory
    //
    if(de->DIR_Attr & ATTR_DIR)
    {
        // If the entry pointed to a not null directory, we do not delete it.
        do
        {
            for(cursec = 0; cursec < sb_info->iSecPerClus; cursec++)
            {
                DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
                lErrCode = fat_read_cluster(sb_info, cluster, cursec, secbuf);
                if(_ERR_FAT_SUCCESS == lErrCode)
                {
                    for(i = 0; i < DEFAULT_SECSIZE; i += DEFAULT_DIRENTRYSIZE)
                    {
                        Buf2FDE((UINT8 *)(secbuf + i), &dent);
                        if(0x00 == dent.DIR_Name[0])
                        {
                            goto delete_entry;
                        }
                        else if(0xE5 == dent.DIR_Name[0]) // ignore deleted dir
                        {
                            continue;
                        }
                        else if('.' == dent.DIR_Name[0]) // igore '.' and '..'
                        {
                            continue;
                        }
                        else if(!FAT_IS_SHORT_ENTRY(secbuf + i))
                        {
                            continue;
                        }
                        else if(FAT_IS_SHORT_ENTRY(secbuf + i))
                        {
                            if(FAT_IS_JAP_DELETE_ENTRY(secbuf + i))
                            {
                                continue;
                            }
                            else
                            {
                                D( ( DL_FATWARNING, "In  fat_remove_entry, _ERR_FAT_DIR_NOT_NULL.\n"));
                                lErrCode = _ERR_FAT_DIR_NOT_NULL;
                                goto end;
                            }
                        }
                        else  // found one, that is, the directory is not null.
                        {
                            continue;
                        }
                    }
                }
                else
                {
                    lErrCode = _ERR_FAT_READ_SEC_FAILED;
                    goto end;
                }
            }

            lErrCode = fat_get_next_cluster(sb_info, cluster, &cluster);
            if (lErrCode != _ERR_FAT_SUCCESS)
            {
                D((DL_FATERROR, "fat_remove_entry: fat_get_next_cluster() return %d", lErrCode));
                goto end;
            }
        }
        while(!fat_is_last_cluster(sb_info, cluster) && cluster >= 2);
    }

    //delete_entry:
    //
    // Coming here, the entry pointed to eithe a not-null directory or a regular file
    // and we can delete it safely.
    //

    // Step 2) Remove the 'de' itselft
    //
    // Delete the de by setting the DIR_Name[0] to 0xE5, that is, just mark it.
    //

delete_entry:

    DSM_MemSet(&dent, 0x00, sizeof(FAT_DIR_ENTRY));

    if(bclu >= 2)  // Operation not on the root directory
    {
        lErrCode = fat_read_entries(sb_info, bclu, bsec, eoff, secbuf, &ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "In  fat_remove_entry,fat_read_entries error, ErrorCode = %d \n", lErrCode));
            goto end;
        }

        for(i = 0; i < ecount; i++)
        {
            *(secbuf + i * DEFAULT_DIRENTRYSIZE) = 0xe5;
        }

        Buf2FDE((secbuf + (ecount - 1)*DEFAULT_DIRENTRYSIZE), &dent);

        lErrCode = fat_write_entries(sb_info, bclu, bsec, eoff, secbuf, ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "In  fat_remove_entry,fat_write_entries error, ErrorCode = %d \n", lErrCode));
            goto end;
        }
    }
    else  if(0 == bclu)// Operation on the root directory
    {
        lErrCode = fat_read_root_entries(sb_info, bsec, eoff, secbuf, &ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "In  fat_remove_entry,fat_read_root_entries error, ErrorCode = %d \n", lErrCode));
            goto end;
        }

        for(i = 0; i < ecount; i++)
        {
            *(secbuf + i * DEFAULT_DIRENTRYSIZE) = 0xe5;
        }

        Buf2FDE((secbuf + (ecount - 1)*DEFAULT_DIRENTRYSIZE), &dent);

        lErrCode = fat_write_root_entries(sb_info, bsec, eoff, secbuf, ecount);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "In  fat_remove_entry,fat_write_root_entries error, ErrorCode = %d \n", lErrCode));
            goto end;
        }
    }
    else
    {
        D( ( DL_FATERROR, "In  fat_remove_entry,custer number is invalid: 0x%x.", bclu));
        DSM_ASSERT(0, "In  fat_remove_entry,custer number is invalid: 0x%x.", bclu);
    }

    // Step 3) Remove the content which the 'de' point to according the test result
    // if the directory is not null, we can not get here.
    if(!bRename && (de->DIR_FstClusLO != CLUSTER_FREE || de->DIR_FstClusHI != CLUSTER_FREE))
    {
        UINT32 FstCluster;

        FstCluster = (UINT32)((UINT32)(de->DIR_FstClusHI << 16) | de->DIR_FstClusLO);
        lErrCode = fat_delete_fatentries(sb_info, FstCluster);
        if(_ERR_FAT_SUCCESS != lErrCode)
        {
            D( ( DL_FATERROR, "In  fat_remove_entry,fat_delete_fatentries error, ErrorCode = %d \n", lErrCode));
        }
    }
end:
    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }
    return lErrCode;
}


//*******************************************************************************
// Function:
//   create a directory or file entry in the pointed directory and alloc cluster
//   for the newly created entry.
// Parameter:
//   diri[in], the pointed directory on which to create entry.
//   de [in][out], point to the entry to add, output the DIR_FstClusLO filed.
// Return value:
//    _ERR_FAT_SUCCESS indicates a successful operation.
//         And unsuccessful operation is denoted by the err code as following.
//      _ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      _ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      _ERR_FAT_DISK_FULL:          The logical disk is full.
//      _ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************
INT32 fat_add_entry(struct inode *diri, FAT_DIR_ENTRY *de,
                    UINT32 *bclu, UINT16 *bsec, UINT16 *eoff, WCHAR *pszFileName, BOOL bRename)
{
    FAT_DIR_ENTRY *d_entry = NULL;
    FAT_SB_INFO *sb_info = NULL;
    UINT8 *secbuf = NULL;
    UINT32 curclu = 0, preclu = 0;
    UINT32 curclu2 = 0, preclu2 = 0;
    UINT16 secoff = 0;
    UINT8 cursec;
    FAT_DIR_ENTRY dent;

    INT32 iRet = _ERR_FAT_SUCCESS;
    INT32 iResult = _ERR_FAT_SUCCESS;

    UINT32 ecount = 0;
    FAT_DIR_ENTRY snent;  // short name entry
    UINT8 *ebuf;
    UINT16 freenr = 0;
    UINT16 freebegoff = 0;
    UINT16 freebegsec = 0;
    UINT32 freebegclu = 0;
    UINT16 leftspace = 0;
    BOOL   hasset = FALSE;
    UINT32 i;

    d_entry = &(diri->u.fat_i.entry);
    sb_info = &(diri->i_sb->u.fat_sb);

    ebuf = FAT_SECT_BUF_ALLOC();
    if(NULL == ebuf)
    {
        D( ( DL_FATERROR, "in fat_add_entry,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_add_entry,1.FAT_SECT_BUF_ALLOC failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        iRet = _ERR_FAT_MALLOC_FAILED;
        D( ( DL_FATERROR, "in fat_add_entry,2.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_add_entry,2.FAT_SECT_BUF_ALLOC failed.");
        goto fat_add_entry_end;
    }

    // Get the first cluster for curren directory.
    curclu = (UINT32)((UINT32)(d_entry->DIR_FstClusHI << 16) | d_entry->DIR_FstClusLO);
    iRet = fat_get_long_entry(sb_info, curclu, pszFileName, (FAT_LONG_DIR_ENTRY *)ebuf, &ecount);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D((DL_FATERROR, "fat_add_entry: fat_get_long_entry() return %d\n", iRet));
        goto fat_add_entry_end;
    }

    Buf2FDE((ebuf + (ecount - 1) * sizeof(FAT_DIR_ENTRY)), &snent);

    // Set the member value of FAT_DIR_ENTRY
    snent.DIR_Attr = de->DIR_Attr;
    snent.DIR_CrtDate = de->DIR_CrtDate;
    snent.DIR_CrtTime = de->DIR_CrtTime;
    snent.DIR_CrtTimeTenth = de->DIR_CrtTimeTenth;
    snent.DIR_FileSize = de->DIR_FileSize;
    // If the entry is dirctory, shell be reset  after get free cluster.
    snent.DIR_FstClusHI = de->DIR_FstClusHI;
    snent.DIR_FstClusLO = de->DIR_FstClusLO;
    snent.DIR_LstAccDate = de->DIR_LstAccDate;
    snent.DIR_NTRes = de->DIR_NTRes;
    snent.DIR_WrtDate = de->DIR_WrtDate;
    snent.DIR_WrtTime = de->DIR_WrtTime;

    //
    // 1) find free DIR entry in the existed cluster
    // 2) if all of the cluster is full, alloc a new cluster
    // 3) add the entry gotten from 1) or 2) into parent DIR entry
    //

    // Find the free entry in already allocate cluster.
    do
    {
        //Find the free entry in current cluser.
        for(cursec = 0; cursec < sb_info->iSecPerClus; cursec++)
        {
            // Find the free entry in current sector.
            DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
            iResult = fat_read_cluster(sb_info, curclu, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                iRet = _ERR_FAT_READ_SEC_FAILED;
                D( ( DL_FATERROR, "In  fat_add_entry,fat_read_cluster error, ErrorCode = %d \n", iRet));
                goto fat_add_entry_end;
            }

            // find a group of consecutive free or deleting entriy.
            for(secoff = 0; secoff < DEFAULT_SECSIZE; secoff += DEFAULT_DIRENTRYSIZE)
            {
                Buf2FDE((UINT8 *)(secbuf + secoff), &dent);

                // Deleted entry or free entry.
                if((0x00 == dent.DIR_Name[0]) || (0xE5 == dent.DIR_Name[0]))
                {
                    if(!hasset)
                    {
                        freebegsec = cursec;
                        freebegoff = secoff;
                        freebegclu = curclu;
                        hasset = TRUE;
                    }

                    freenr++;

                    if(freenr == ecount)
                    {
                        goto free_entry_found;
                    }
                }
                else
                {
                    freebegsec = 0;
                    freebegoff = 0;
                    freebegclu = 0;
                    freenr = 0;
                    hasset = FALSE;
                }
            }
        }

        preclu = curclu;
        iRet = fat_get_next_cluster(sb_info, preclu, &curclu);
        if (iRet != _ERR_FAT_SUCCESS)
        {
            D((DL_FATERROR, "fat_add_entry: fat_get_next_cluster() return %d", iRet));
            goto fat_add_entry_end;
        }
    }
    while((!fat_is_last_cluster(sb_info, curclu)));

    //
    // If NOT found the free entry in allocted cluster,
    // allocte one or two new clusters for it.
    //
    preclu2 = preclu;
    iRet = fat_get_free_cluster(sb_info, preclu2, &curclu2);
    if(iRet < 0 || curclu2 < 2)
    {
        D( ( DL_FATERROR, "In  fat_add_entry,fat_get_free_cluster()1 failed! ErrorCode = %d\n", iRet));
        goto fat_add_entry_end;
    }
    DSM_ASSERT((BOOL)(curclu2 >= 2), "In  fat_remove_entry,custer number is invalid: 0x%x.", bclu);

    // Clean all the sector of the new cluster.
    DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
    for(i = 0; i < sb_info->iSecPerClus; i++)
    {
        iRet = fat_write_cluster(sb_info, curclu2, i, secbuf);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_write_cluster failed, ErrorCode = %d\n", iRet));
            break;
        }
    }

    if(0 == freebegsec && 0 == freebegoff && 0 == freebegclu)
    {
        freebegclu = curclu2;
    }

    // Set the FAT table to 0xFFFF for the new-cluster.
    iRet = fat_set_fat_entry_value(sb_info, curclu2, CLUSTER_NULL, TRUE, NULL);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D( ( DL_FATERROR, "In  fat_add_entry,fat_set_fat_entry_value_0, ErrorCode = %d\n", iRet));
        goto fat_add_entry_end;
    }

    // Set the FAT table to new-cluster for the pre-cluster.
    iRet = fat_set_fat_entry_value(sb_info, preclu2, curclu2, TRUE, NULL);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D( ( DL_FATERROR, "In  fat_add_entry,fat_set_fat_entry_value_1, ErrorCode = %d\n", iRet));
        goto fat_add_entry_end;
    }
    if(TRUE == hasset)
    {
        leftspace = (UINT16)(sb_info->iSecPerClus * sb_info->iBytesPerSec - (freebegoff + freebegsec * sb_info->iBytesPerSec));
    }
    else
    {
        leftspace = 0;
    }
    // Allocate the second cluster
    if((UINT16)(ecount * sizeof(FAT_DIR_ENTRY)) > sb_info->iSecPerClus * sb_info->iBytesPerSec + leftspace)
    {
        preclu2 = curclu2;
        iRet = fat_get_free_cluster(sb_info, preclu2, &curclu2);
        if(iRet < 0 || curclu2 < 2)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_get_free_cluster()2 failed! ErrorCode = %d\n", iRet));
            goto fat_add_entry_end;
        }
        DSM_ASSERT((BOOL)(curclu2 >= 2), "curclu2(%d) < 2,preclu2 = %d.", curclu2, preclu2);

        // Clean all the sector of the new cluster.
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
        for(i = 0; i < sb_info->iSecPerClus; i++)
        {
            iRet = fat_write_cluster(sb_info, curclu2, i, secbuf);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D( ( DL_FATERROR, "In  fat_add_entry,fat_write_cluster failed, ErrorCode = %d\n", iRet));
                break;
            }
        }
        // Set the FAT table to 0xFFFF for the new-cluster.
        iRet = fat_set_fat_entry_value(sb_info, curclu2, CLUSTER_NULL, TRUE, NULL);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_set_fat_entry_value_2, ErrorCode = %d\n", iRet));
            goto fat_add_entry_end;
        }

        // Set the FAT table to new-cluster for the pre-cluster.
        iRet = fat_set_fat_entry_value(sb_info, preclu2, curclu2, TRUE, NULL);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_set_fat_entry_value_3, ErrorCode = %d\n", iRet));
            goto fat_add_entry_end;
        }

    }


free_entry_found:
    if((de->DIR_Attr & ATTR_DIR) && !bRename)
    {
        // Alloc a cluster for the creating dir-entry.
        // Set the output parameter.
        iRet = fat_get_free_cluster(sb_info, 2, &curclu2);
        if(iRet < 0 || curclu2 < 2)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_get_free_cluster error, ErrorCode = %d", iRet));
            goto fat_add_entry_end;
        }
        DSM_ASSERT((BOOL)(curclu2 >= 2), "In  fat_add_entry,fat_get_free_cluster error, ErrorCode = %d.", iRet);

        // Clean all the sector of the new cluster.
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
        for(i = 0; i < sb_info->iSecPerClus; i++)
        {
            iRet = fat_write_cluster(sb_info, curclu2, i, secbuf);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D( ( DL_FATERROR, "In  fat_add_entry,fat_write_cluster failed, ErrorCode = %d\n", iRet));
                break;
            }
        }
        snent.DIR_FstClusLO = (UINT16)(curclu2 & 0x0000FFFF);
        snent.DIR_FstClusHI = (UINT16)((curclu2 >> 16) & 0x0000FFFF);

        // set the FAT of new cluster .
        iRet = fat_set_fat_entry_value(sb_info, curclu2, CLUSTER_NULL, TRUE, NULL);
        if(_ERR_FAT_SUCCESS !=  iRet)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_set_fat_entry_value_4 error, ErrorCode = %d\n", iRet));
            goto fat_add_entry_end;
        }

    }

    // Write the creating dir-entry into current directory

    // Set the output parameters
    *bclu = freebegclu;
    *bsec = freebegsec;
    *eoff = freebegoff;

    FDE2Buf(&snent, (ebuf + (ecount - 1) * sizeof(FAT_DIR_ENTRY)));
    iRet = fat_write_entries(sb_info, *bclu, *bsec, *eoff, ebuf, (UINT8)ecount);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D( ( DL_FATERROR, "In  fat_add_entry,fat_write_entries error, ErrorCode = %d\n", iRet));
        goto fat_add_entry_end;
    }

    //
    // If we are creating a directory, we make '.' and '..' diretory
    // into the newly creating diretory
    //
    if((de->DIR_Attr & ATTR_DIR) && !bRename)
    {
        DSM_MemSet(&dent, 0x00, sizeof(dent));
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);

        DSM_MemCpy(dent.DIR_Name, ".          ", 11);
        dent.DIR_FstClusLO = snent.DIR_FstClusLO;
        dent.DIR_FstClusHI = snent.DIR_FstClusHI;
        dent.DIR_Attr |= ATTR_DIR;
        FDE2Buf(&dent, secbuf);

        DSM_MemCpy(dent.DIR_Name, "..         ", 11);
        dent.DIR_Attr |= ATTR_DIR;
        if (FS_ROOT_INO == diri->i_ino)
        {
            // The root directory cluster number is identified with 0.
            dent.DIR_FstClusLO = 0;
            dent.DIR_FstClusHI = 0;
        }
        else
        {
            dent.DIR_FstClusLO = d_entry->DIR_FstClusLO;
            dent.DIR_FstClusHI = d_entry->DIR_FstClusHI;
        }
        FDE2Buf(&dent, (UINT8 *)(secbuf + DEFAULT_DIRENTRYSIZE));

        iRet = fat_write_cluster(sb_info, curclu2, 0, secbuf);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, "In  fat_add_entry,fat_write_cluster error, ErrorCode = %d\n", iRet));
            goto fat_add_entry_end;
        }
    }

fat_add_entry_end:

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }
    if(NULL != ebuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)ebuf);
    }

    return iRet;
}

/********************************************************************************/
// Function:
//   create a directory or file entry in the root directory and alloc cluster
//   for the newly created entry.
//
// Parameter:
//   rdiri[in], Point to the inode of root node.
//   de [in][out], point to the entry to add, output the DIR_FstClusLO filed.
// Return value:
//    _ERR_FAT_SUCCESS indicates a successful operation.
//         And unsuccessful operation is denoted by the err code as following.
//      _ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      _ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      _ERR_FAT_ROOT_FULL:          The root dir is full.
//      _ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************
INT32 fat_add_root_entry(struct inode *rdiri, FAT_DIR_ENTRY *de,
                         UINT16 *bsec, UINT16 *eoff, WCHAR *pszFileName, BOOL bRename)
{
    FAT_SB_INFO *sb_info = NULL;
    UINT8 *secbuf = NULL;
    UINT32  secoff = 0;
    FAT_DIR_ENTRY dent;
    UINT8 cheat = 1;
    UINT32 cursec = 0;
    INT32 iRet = _ERR_FAT_SUCCESS;

    UINT32 ecount = 0;
    FAT_DIR_ENTRY snent;  // short name entry
    UINT8 *ebuf = NULL;
    UINT16 freenr = 0;
    UINT16 freebegoff = 0;
    UINT16 freebegsec = 0;
    BOOL   hasset = FALSE;
    UINT32 i;

    sb_info = &(rdiri->i_sb->u.fat_sb);

    ebuf = FAT_SECT_BUF_ALLOC();
    if(NULL == ebuf)
    {

        D(( DL_FATERROR, "in fat_add_root_entry,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_add_root_entry,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR, "in fat_add_root_entry,2.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_add_root_entry,2.FAT_SECT_BUF_ALLOC() failed.");
        iRet = _ERR_FAT_MALLOC_FAILED;
        goto fat_add_root_entry_end;
    }

    iRet = fat_get_long_entry(sb_info, 0, pszFileName, (FAT_LONG_DIR_ENTRY *)ebuf, &ecount);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D((DL_FATWARNING, "call fat_get_long_entry , iRet = %d\n", iRet));
        goto fat_add_root_entry_end;
    }

    Buf2FDE((ebuf + (ecount - 1) * sizeof(FAT_DIR_ENTRY)), &snent);

    // Set the member value of FAT_DIR_ENTRY
    snent.DIR_Attr = de->DIR_Attr;
    snent.DIR_CrtDate = de->DIR_CrtDate;
    snent.DIR_CrtTime = de->DIR_CrtTime;
    snent.DIR_CrtTimeTenth = de->DIR_CrtTimeTenth;
    snent.DIR_FileSize = de->DIR_FileSize;
    snent.DIR_FstClusHI = de->DIR_FstClusHI;
    snent.DIR_FstClusLO = de->DIR_FstClusLO;
    snent.DIR_LstAccDate = de->DIR_LstAccDate;
    // de->DIR_Name, has set before.
    snent.DIR_NTRes = de->DIR_NTRes;
    snent.DIR_WrtDate = de->DIR_WrtDate;
    snent.DIR_WrtTime = de->DIR_WrtTime;

    cursec = sb_info->iRootDirStartSec;

    do
    {
        iRet = DRV_BLOCK_READ( sb_info->sb->s_dev, cursec, secbuf );
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D((DL_FATERROR, "DRV_BLOCK_READ error, ErrorCode = %d\n", iRet));
            goto fat_add_root_entry_end;

        }

        for(secoff = 0; secoff < DEFAULT_SECSIZE; secoff += DEFAULT_DIRENTRYSIZE)
        {
            Buf2FDE((UINT8 *)(secbuf + secoff), &dent);

            // Deleted entry or free entry.
            if(0x00 == dent.DIR_Name[0] || 0xE5 == dent.DIR_Name[0])
            {
                if(!hasset)
                {
                    freebegsec = (UINT16)cursec;
                    freebegoff = (UINT16)secoff;
                    hasset = TRUE;
                }

                freenr++;

                if(freenr == ecount)
                {
                    goto free_entry_found;
                }
            }
            else
            {
                freebegsec = 0;
                freebegoff = 0;
                freenr = 0;
                hasset = FALSE;
            }
        }

        cursec++;
        if(cursec - sb_info->iRootDirStartSec >= sb_info->iRootDirSecCnt)
        {
            iRet = _ERR_FAT_ROOT_FULL;
            D((DL_DETAIL, " in fat_add_root_entry,fat_root is full\n"));
            goto fat_add_root_entry_end;
        }
    }
    while(1 == cheat);   // dead loop

free_entry_found:
    if((de->DIR_Attr & ATTR_DIR) && !bRename)
    {
        UINT32 iFreeClu = 0;

        //
        // Alloc a cluster for the newly created directory.
        //
        iRet = fat_get_free_cluster(sb_info, 2, &iFreeClu);
        if(iRet < 0 || iFreeClu < 2)
        {
            D((DL_FATWARNING, " in fat_add_root_entry,fat_get_free_cluster error ErrorCode = %d.", iRet));
            goto fat_add_root_entry_end;
        }
        DSM_ASSERT((BOOL)(iFreeClu >= 2), " in fat_add_root_entry,fat_get_free_cluster error ErrorCode = %d.", iRet);

        // Clean all the sector of the new cluster.
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
        for(i = 0; i < sb_info->iSecPerClus; i++)
        {
            iRet = fat_write_cluster(sb_info, iFreeClu, i, secbuf);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D( ( DL_FATERROR, "In  fat_add_root_entry,fat_write_cluster failed, ErrorCode = %d\n", iRet));
                break;
            }
        }
        snent.DIR_FstClusLO = (UINT16)(iFreeClu & 0xffff);
        snent.DIR_FstClusHI = (UINT16)((iFreeClu >> 16) & 0xffff);

        iRet = fat_set_fat_entry_value(sb_info, iFreeClu, CLUSTER_NULL, TRUE, NULL);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D((DL_FATWARNING, " in fat_add_root_entry,fat_set_fat_entry_value_0 error ErrorCode = %d\n", iRet));
            goto fat_add_root_entry_end;
        }
    }

    // Set the out parameters.
    *bsec = (UINT16)(freebegsec - sb_info->iRootDirStartSec);
    *eoff = (UINT16)freebegoff;

    FDE2Buf(&snent, (ebuf + (ecount - 1) * sizeof(FAT_DIR_ENTRY)));
    iRet = fat_write_root_entries(sb_info, *bsec, *eoff, ebuf, (UINT8)ecount);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D( ( DL_FATDETAIL, "in fat_add_root_entry(), fat_write_root_entries iRet = %d.\n", iRet));
        goto fat_add_root_entry_end;
    }

    // If we are creating a directory, we make '.' and '..' diretory
    // into the newly creating diretory
    if((de->DIR_Attr & ATTR_DIR) && !bRename)
    {
        DSM_MemSet(&dent, 0x00, sizeof(dent));
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);

        DSM_MemCpy(dent.DIR_Name, ".          ", 11);
        dent.DIR_Attr |= ATTR_DIR;
        dent.DIR_FstClusLO = snent.DIR_FstClusLO;
        dent.DIR_FstClusHI = snent.DIR_FstClusHI;
        FDE2Buf(&dent, secbuf);

        DSM_MemCpy(dent.DIR_Name, "..         ", 11);
        dent.DIR_Attr |= ATTR_DIR;
        dent.DIR_FstClusLO = 0;
        dent.DIR_FstClusHI = 0;
        FDE2Buf(&dent, (UINT8 *)(secbuf + DEFAULT_DIRENTRYSIZE));

        iRet = fat_write_cluster(sb_info, (UINT32)((UINT16)snent.DIR_FstClusLO | (UINT32)(snent.DIR_FstClusHI << 16)), 0, secbuf);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D( ( DL_FATERROR, "In  fat_add_root_entry,fat_write_cluster error, ErrorCode = %d\n", iRet));
            goto fat_add_root_entry_end;
        }
    }

fat_add_root_entry_end:
    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }
    if(NULL != ebuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)ebuf);
    }

    return iRet;
}

INT32 fat_write_entries(FAT_SB_INFO *sb,
                        UINT32 bclu, UINT16 bsec, UINT16 eoff, UINT8 *ebuf, UINT8 ecount)
{
    UINT32 len = 0;
    UINT8 *bufp = NULL;
    UINT8  secnum = 0;
    UINT32 clunum = 0;
    UINT32 secoff = 0;
    UINT8 *secbuf = NULL;

    UINT8  cheat = 1;

    INT32 iResult;
    INT32 iRet = _ERR_FAT_SUCCESS;

    bufp = ebuf;
    secnum = (UINT8)bsec;
    clunum = bclu;
    secoff = eoff;

    len = sizeof(FAT_DIR_ENTRY) * ecount;

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR, "in fat_write_entries,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_write_entries,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    while(cheat)
    {
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);

        if(secoff == 0)
        {
            if(len > DEFAULT_SECSIZE)
            {
                iResult = fat_write_cluster(sb, clunum, secnum, bufp);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_write_cluster_0 error, ErrorCode = %d\n", iRet));
                    break;
                }

                bufp += DEFAULT_SECSIZE;
                secnum++;
                secoff = 0;
                len -= DEFAULT_SECSIZE;
            }
            else if(len == DEFAULT_SECSIZE)
            {
                iResult = fat_write_cluster(sb, clunum, secnum, bufp);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_write_cluster_1 error, ErrorCode = %d\n", iRet));
                    break;
                }

                break;
            }
            else // secoff + len < DEFAULT_SECSIZE (1)
            {

                iResult = fat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_read_cluster_0 error, ErrorCode = %d\n", iRet));
                    break;
                }

                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, len);
                iResult = fat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_write_cluster_2 error, ErrorCode = %d\n", iRet));
                    break;
                }

                break;
            }
        }
        else if(secoff > 0)
        {
            if(secoff + len > DEFAULT_SECSIZE)
            {
                iResult = fat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_read_cluster_1 error, ErrorCode = %d\n", iRet));
                    break;
                }

                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, DEFAULT_SECSIZE - secoff);

                iResult = fat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_write_cluster_3 error, ErrorCode = %d\n", iRet));
                    break;
                }

                bufp += (DEFAULT_SECSIZE - secoff);

                len -= (DEFAULT_SECSIZE - secoff);
                secnum++;
                secoff = 0;
            }
            else //secoff + len <= DEFAULT_SECSIZE (1)
            {
                iResult = fat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_read_cluster_2 error, ErrorCode = %d\n", iRet));
                    break;
                }

                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, len);

                iResult = fat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_entries,fat_write_cluster_4 error, ErrorCode = %d\n", iRet));
                    break;
                }

                break;
            }
        }

        if(secnum == sb->iSecPerClus) // Get to the end of the cluster
        {
            secnum = 0;
            iRet = fat_get_next_cluster(sb, clunum, &clunum);
            if (iRet != _ERR_FAT_SUCCESS)
            {
                D((DL_FATERROR, "fat_write_entries: fat_get_next_cluster() return %d", iRet));
                break;
            }
        }
    }// while(cheat)

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }

    return iRet;
}


INT32 fat_write_root_entries(FAT_SB_INFO *sb,
                             UINT16 bsec, UINT16 eoff, UINT8 *ebuf, UINT8 ecount)
{
    UINT32 len;
    UINT8* bufp = NULL;
    UINT32  secnum = 0;
    UINT32 secoff = 0;
   UINT8* secbuf;
    UINT8  cheat = 1;
    INT32 iResult;
    INT32 iRet = _ERR_FAT_SUCCESS;

    secbuf = DSM_MAlloc(DEFAULT_SECSIZE*2);
    if(!secbuf)
    {
        D( ( DL_FATERROR,"in fat_write_root_entries,malloc(%d) failed.",(DEFAULT_SECSIZE*2)));
        return _ERR_FAT_READ_SEC_FAILED;
    }
    bufp = ebuf;
    secnum = bsec + sb->iRootDirStartSec;
    secoff = eoff;

    len = sizeof(FAT_DIR_ENTRY) * ecount;

    while(cheat)
    {
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);

        if(secoff == 0)
        {
            if(len > DEFAULT_SECSIZE)
            {
                D( ( DL_FATDETAIL,
                     "in fat_write_root_entries() call DRV_BLOCK_WRITE(),with secnum = %d.\n",
                     secnum));
                iResult = DRV_BLOCK_WRITE( sb->sb->s_dev, secnum, bufp );
                D( ( DL_FATDETAIL,
                     "in fat_write_root_entries() after DRV_BLOCK_WRITE(),iResul = %d.\n",
                     iResult));
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_WRITE_0 error, ErrorCode = %d\n", iRet));
                    break;
                }

                bufp += DEFAULT_SECSIZE;

                secnum++;
                secoff = 0;
                len -= DEFAULT_SECSIZE;
            }
            else if(len == DEFAULT_SECSIZE)
            {
                D( ( DL_FATDETAIL, "secoff == 0----------------in write =,secnum = %d, sb->iRootDirStartSec=%d\n", secnum, sb->iRootDirStartSec));
                iResult = DRV_BLOCK_WRITE( sb->sb->s_dev, secnum, bufp );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_WRITE_1 error, ErrorCode = %d\n", iRet));
                    break;
                }

                break;
            }
            else // secoff + len < DEFAULT_SECSIZE (1)
            {
                iResult = DRV_BLOCK_READ( sb->sb->s_dev, secnum, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_READ_0 error, ErrorCode = %d\n", iRet));
                    break;
                }

                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, len);
                D( ( DL_FATDETAIL, "secoff == 0----------------in write <,secnum = %d, sb->iRootDirStartSec=%d\n", secnum, sb->iRootDirStartSec));
                iResult = DRV_BLOCK_WRITE( sb->sb->s_dev, secnum, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_WRITE_2 error, ErrorCode = %d\n", iRet));
                    break;
                }

                break;
            }
        }
        else if(secoff > 0)
        {
            if(secoff + len > DEFAULT_SECSIZE)
            {
                iResult = DRV_BLOCK_READ( sb->sb->s_dev, secnum, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_READ_1 error, ErrorCode = %d\n", iRet));
                    break;
                }

                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, DEFAULT_SECSIZE - secoff);
                D( ( DL_FATDETAIL, "secoff>0----------------in write >,secnum = %d,sb->iRootDirStartSec=%d\n", secnum, sb->iRootDirStartSec));
                iResult = DRV_BLOCK_WRITE( sb->sb->s_dev, secnum, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_WRITE_3 error, ErrorCode = %d\n", iRet));
                    break;
                }

                bufp += (DEFAULT_SECSIZE - secoff);

                len -= (DEFAULT_SECSIZE - secoff);
                secnum++;
                secoff = 0;
            }
            else //secoff + len <= DEFAULT_SECSIZE (1)
            {
                iResult = DRV_BLOCK_READ( sb->sb->s_dev, secnum, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_READ_2 error, ErrorCode = %d\n", iRet));
                    break;
                }

                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, len);
                D( ( DL_FATDETAIL, "secoff>0----------------in write <=,secnum = %d,sb->iRootDirStartSec=%d\n", secnum, sb->iRootDirStartSec));
                iResult = DRV_BLOCK_WRITE( sb->sb->s_dev, secnum, secbuf );
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR, "in fat_write_root_entries,DRV_BLOCK_WRITE_4 error, ErrorCode = %d\n", iRet));
                    break;
                }

                break;
            }
        }
    }// while(cheat)
    if(secbuf)
    {
        DSM_Free(secbuf);
    }
    return iRet;
}


INT32 fat_read_root_entries(FAT_SB_INFO *sb,
                            UINT16 bsec, UINT16 eoff, UINT8 *ebuf, UINT8 *ecount)
{
    UINT16 cursec = 0;
    UINT32 secoff = 0;
    UINT8 *secbuf = NULL;
    UINT8 *bufp = NULL;
    INT32 iResult = _ERR_FAT_SUCCESS;
    UINT8 cheat = 1;
    UINT8 status = 0;
    UINT8 count = 0;

    bufp = ebuf;
    cursec = (UINT16)(bsec + sb->iRootDirStartSec);

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR, "in fat_read_root_entries,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_read_root_entries,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }
    D( ( DL_FATDETAIL, "in fat_read_root_entries, bsec = %d, eoff = %d.\n", bsec, eoff));
    while(cheat)
    {
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
        iResult = DRV_BLOCK_READ(sb->sb->s_dev, cursec, secbuf);
        if(_ERR_FAT_SUCCESS != iResult)
        {
            iResult = _ERR_FAT_READ_SEC_FAILED;
            D( ( DL_FATERROR, "in fat_read_root_entries,DRV_BLOCK_READ error, ErrorCode = %d\n", iResult));
            goto end;
        }

        for(secoff = eoff; secoff < DEFAULT_SECSIZE; secoff += DEFAULT_DIRENTRYSIZE)
        {
            UINT8 *np = (UINT8 *)(secbuf + secoff);
            D( ( DL_FATDETAIL, "------------in read, cursec = %d, secoff = %d\n", cursec, secoff));
            switch(status)
            {
                case 0: // start status
                    if(FAT_IS_SHORT_ENTRY(np))   // short
                    {
                        D( ( DL_FATDETAIL, "------------0 in short entry, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        goto end;
                    }
                    else if(FAT_IS_LAST_LONG_ENTRY(np) && FAT_IS_FIRST_LONG_ENTRY(np)) // last & first
                    {
                        D( ( DL_FATDETAIL, "------------0 in long entry_1, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        status = 1;
                    }
                    else if(FAT_IS_LAST_LONG_ENTRY(np) && !FAT_IS_FIRST_LONG_ENTRY(np)) // last & not first
                    {
                        D( ( DL_FATDETAIL, "------------0 in long entry_2, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        status = 2;
                    }
                    else
                    {
                        D( ( DL_FATDETAIL, "------------0 in else, eoff = %d\n", eoff));
                        D( ( DL_FATERROR, "in fat_read_root_entries,_ERR_FAT_ERROR\n"));
                        g_TstFsErrCode = 1016;
                        iResult = _ERR_FAT_ERROR;
                        goto end;
                        /*
                        bufp = ebuf;
                        status = 0;
                        count = 0;*/
                    }
                    break;

                case 1:  // last & first status
                    if(FAT_IS_SHORT_ENTRY(np)) // short
                    {
                        D( ( DL_FATDETAIL, "------------1 in short, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        goto end;
                    }
                    else
                    {
                        D( ( DL_FATDETAIL, "------------1 in else, eoff = %d\n", eoff));
                        bufp = ebuf;
                        status = 0;
                        count = 0;
                    }
                    break;

                case 2: // middle long name status
                    if(FAT_IS_FIRST_LONG_ENTRY(np))
                    {
                        D( ( DL_FATDETAIL, "------------2 in first long, eoff = %d\n", eoff));
                        D( ( DL_FATDETAIL, "------------long entry\n"));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        status = 1;
                    }
                    else if(FAT_IS_MIDDLE_LONG_ENTRY(np))
                    {
                        D( ( DL_FATDETAIL, "------------2 in middle long, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        status = 3;
                    }
                    else
                    {
                        D( ( DL_FATDETAIL, "------------2 in else long, eoff = %d\n", eoff));
                        bufp = ebuf;
                        status = 0;
                        count = 0;
                    }
                    break;

                case 3: // last long name status
                    if(FAT_IS_MIDDLE_LONG_ENTRY(np))
                    {
                        D( ( DL_FATDETAIL, "------------3 in middle long, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        status = 3;
                    }
                    else if(FAT_IS_FIRST_LONG_ENTRY(np))
                    {
                        D( ( DL_FATDETAIL, "------------3 in first long, eoff = %d\n", eoff));
                        DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                        bufp += DEFAULT_DIRENTRYSIZE;
                        count++;
                        status = 1;
                    }
                    else
                    {
                        D( ( DL_FATDETAIL, "------------3 in else, eoff = %d\n", eoff));
                        bufp = ebuf;
                        status = 0;
                        count = 0;
                    }
                    break;
            }
        }

        cursec++;
        eoff = 0;

        if(cursec - sb->iRootDirStartSec >= sb->iRootDirSecCnt)
        {
            g_TstFsErrCode = 1017;
            iResult = _ERR_FAT_ERROR;
            D( ( DL_FATERROR, "in fat_read_root_entries,FAT_ROOT_FULL:\n"));
            D( ( DL_FATERROR, "cursec = %d, sb->iRootDirStartSec = %d, sb->iRootDirSecCnt = %d.\n",
                 cursec, sb->iRootDirStartSec, sb->iRootDirSecCnt));
            goto end;
        }
    }

end:
    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }

    *ecount = count;

    return iResult;
}


INT32 fat_read_entries(FAT_SB_INFO *sb, UINT32 bclu,
                       UINT16 bsec, UINT16 eoff, UINT8 *ebuf, UINT8 *ecount)
{
    UINT8  cursec = 0, secnum = 0;
    UINT32 clunum = 0, secoff = 0;

    UINT8 *bufp = NULL;
    INT32 iResult = _ERR_FAT_SUCCESS;
    UINT8 status = 0;
    UINT8 count = 0;
    UINT8* secbuf = NULL;

    secbuf = DSM_MAlloc(DEFAULT_SECSIZE*2);
    if(!secbuf)
    {
        D( ( DL_FATERROR,"in fat_read_entries,malloc(%d) failed.",(DEFAULT_SECSIZE*2)));
        return _ERR_FAT_READ_SEC_FAILED;
    }
    bufp = ebuf;
    clunum = bclu;
    secnum = (UINT8)bsec;

    while(!fat_is_last_cluster(sb, clunum))
    {
        for(cursec = secnum; cursec < sb->iSecPerClus; cursec++)
        {
            DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);

            iResult = fat_read_cluster(sb, clunum, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                iResult = _ERR_FAT_READ_SEC_FAILED;
                D( ( DL_FATERROR, "in fat_read_entries,fat_read_cluster error, ErrorCode = %d\n", iResult));
                goto end;

            }
            for(secoff = (UINT32)eoff; secoff < DEFAULT_SECSIZE; secoff += DEFAULT_DIRENTRYSIZE)
            {
                UINT8 *np = (UINT8 *)(secbuf + secoff);
                switch(status)
                {
                    case 0: // start status
                        if(FAT_IS_SHORT_ENTRY(np))   // short
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            goto end;
                        }
                        else if(FAT_IS_LAST_LONG_ENTRY(np) && FAT_IS_FIRST_LONG_ENTRY(np)) // last & first
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            status = 1;
                        }
                        else if(FAT_IS_LAST_LONG_ENTRY(np) && !FAT_IS_FIRST_LONG_ENTRY(np)) // last & not first
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            status = 2;
                        }
                        else
                        {
                            D( ( DL_FATERROR, "in fat_read_entries, _ERR_FAT_ERROR\n"));
                            g_TstFsErrCode = 1018;
                            //   DSM_ASSERT(0,"fat_read_entries fail.");
                            //     g_secoff = secoff;
                            iResult = _ERR_FAT_ERROR;
                            goto end;

                            //  bufp = ebuf;
                            //  status = 0;
                            //  count = 0;

                        }
                        break;

                    case 1:  // last & first status
                        if(FAT_IS_SHORT_ENTRY(np)) // short
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            goto end;
                        }
                        else
                        {
                            bufp = ebuf;
                            status = 0;
                            count = 0;
                        }
                        break;

                    case 2: // middle long name status
                        if(FAT_IS_FIRST_LONG_ENTRY(np))
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            status = 1;
                        }
                        else if(FAT_IS_MIDDLE_LONG_ENTRY(np))
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            status = 3;
                        }
                        else
                        {
                            bufp = ebuf;
                            status = 0;
                            count = 0;
                        }
                        break;

                    case 3: // last long name status
                        if(FAT_IS_MIDDLE_LONG_ENTRY(np))
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            status = 3;
                        }
                        else if(FAT_IS_FIRST_LONG_ENTRY(np))
                        {
                            DSM_MemCpy(bufp, np, DEFAULT_DIRENTRYSIZE);
                            bufp += DEFAULT_DIRENTRYSIZE;
                            count++;
                            status = 1;
                        }
                        else
                        {
                            bufp = ebuf;
                            status = 0;
                            count = 0;
                        }
                        break;
                }
            }
            eoff = 0;
        }

        iResult = fat_get_next_cluster(sb, clunum, &clunum);
        if (iResult != _ERR_FAT_SUCCESS)
        {
            D((DL_FATERROR, "fat_read_entries: fat_get_next_cluster() return %d", iResult));
            goto end;
        }
        secnum = 0;
    }

end:
    *ecount = count;
    if(secbuf)
    {
        DSM_Free(secbuf);
    }
    return iResult;

}


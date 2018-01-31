#ifdef FS_SUPPORT_EXFAT
#include "dsm_cf.h"
#include "fat_base.h"
#include "exfat_base.h"
#include "fat_local.h"
#include "exfat_local.h"
#include "fs.h"
#include "fs_base.h"

#include "dsm_dbg.h"

INT32 exfat_trunc_fatentry(struct inode* pinode, UINT32 lst_clus,UINT32 trunc_clus)
{
    UINT32 cur_clus = 0, next_clus = 0;
    INT32 result = _ERR_FAT_SUCCESS;
    EXFAT_SB_INFO *sb_info;
    EXFAT_EXT_DIR_ENTRY* ext_entry = NULL;

    sb_info = &pinode->i_sb->u.exfat_sb;
    ext_entry =  &(pinode->u.exfat_i.ext_entry);
    cur_clus = trunc_clus;
    while(EXFAT_IS_VALID_CLUSTER(sb_info, cur_clus))
    {
        result = exfat_get_next_cluster(pinode, cur_clus, &next_clus);
        if (result != _ERR_FAT_SUCCESS)
        {
             D((DL_FATERROR, "exfat_trunc_fatentry: fat_get_next_cluster() return %d", result));
             return result;
        }
        if(next_clus != EXFAT_CLUSTER_FINAL)
        {
            result = exfat_set_bat_entry_value(sb_info,next_clus,CLUSTER_FREE,FALSE,NULL);
            if(result != ERR_SUCCESS)
        	{
                D( ( DL_FATERROR, "exfat_trunc_fatentry:fexfat_set_bat_entry_value failed, result = %d.\n",result) );
                return result;
            }
            cur_clus = next_clus;
        }
        else
        {
            break;
        }
    }
    result = exfat_set_bat_entry_value(sb_info,trunc_clus,CLUSTER_FREE,TRUE,NULL);
    if(result != ERR_SUCCESS)
    {
        D( ( DL_FATERROR, "exfat_trunc_fatentry:fexfat_set_bat_entry_value failed, result = %d.\n",result) );
        return result;
    }
    if(lst_clus != trunc_clus
        && !EXFAT_GENSECON_FLAGS_NOFATCHAIN(ext_entry->GenSeconFlags)
        && EXFAT_IS_VALID_CLUSTER(sb_info,lst_clus))
    {
        result = exfat_set_fat_entry_value(sb_info,lst_clus,CLUSTER_NULL,TRUE,NULL);
        if(result != ERR_SUCCESS)
        {
            D( ( DL_FATERROR, "exfat_trunc_fatentry:exfat_set_fat_entry_value failed, result = %d.\n",result) );
            return result;
        }
    }
    return _ERR_FAT_SUCCESS;
}


INT32 exfat_remove_entry(struct inode *pinode,
                UINT32 bclu, UINT16 bsec, UINT16 eoff, BOOL bRename)
{
    INT32 result;
    INT32 ret = _ERR_FAT_SUCCESS;
    INT32 i;
    EXFAT_DIR_ENTRY* dent = NULL;
    UINT32 sub_clus;
    UINT16  sub_sec;
    UINT16 sub_eoff;
    UINT16  ecount;
    UINT64 ino = 0;
    EXFAT_SB_INFO *sb_info;
    EXFAT_EXT_DIR_ENTRY* ext_entry;


    sb_info = &pinode->i_sb->u.exfat_sb;
    ext_entry =  &pinode->u.exfat_i.ext_entry;

    // Step 1) Check the type of the entry, regular file or directory.
    // If the entry pointed to a not null directory, we do not delete it.
    sub_clus = ext_entry->FirstCluster;
    sub_sec = 0;
    sub_eoff = 0;

    ino = EXFAT_CSO2INO(sub_clus,sub_sec,sub_eoff);
    if(EXFAT_IS_DIR(pinode->i_mode) &&
        FALSE == bRename)
    {
         result = exfat_lookup_file(pinode,
                                                    (WCHAR*)L"*",
                                                    &ino,
                                                    NULL,
                                                    (WCHAR*)NULL,
                                                    &ecount);
         if(result == ERR_SUCCESS)
         {
             return _ERR_FAT_DIR_NOT_NULL;
         }
    }

    // Step 2) Remove the 'de' itselft
    //
    // Coming here, the entry pointed to eithe a not-null directory or a regular file
    // and we can delete it safely.
    //
    // Delete the de by setting the DIR_Name[0] to 0x7f, that is, just mark it.
    //
    dent = DSM_MAlloc(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY));
    if(!dent)
    {
        D( ( DL_FATERROR,"In  exfat_remove_entry,malloc(%d) failed.",
                (EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY))));
        return _ERR_FAT_MALLOC_FAILED;
    }
    DSM_MemSet(dent,0,(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY)));
    if(EXFAT_IS_VALID_CLUSTER(sb_info,bclu))
    {
        result = exfat_read_entries(pinode,
                        bclu,
                        bsec,
                        eoff,
                        dent,
                        &ecount);
        if(_ERR_FAT_SUCCESS == result)
        {
            for(i=0; i< ecount; i++)
            {
                dent[i].EntryType = dent[i].EntryType & 0x7f;
            }

            result = exfat_write_entries(pinode,bclu, bsec, eoff, dent, ecount);
            if(_ERR_FAT_SUCCESS == result)
            {
                // Step 3) Remove the content which the 'de' point to according the test result
                // if the directory is not null, we can not get here.
                if(!bRename && EXFAT_IS_VALID_CLUSTER(sb_info,sub_clus))
                {
                    result = exfat_trunc_fatentry(pinode, sub_clus,sub_clus);
                    if(_ERR_FAT_SUCCESS != result)
                    {
                        D( ( DL_FATERROR,"In  exfat_remove_entry,exfat_trunc_fatentry error, ErrorCode = %d \n",result));
                        ret = result;
                    }
                    else
                    {
                        ret = ERR_SUCCESS;
                    }
                }
            }
            else
            {
                D( ( DL_FATERROR,"In  exfat_remove_entry,fat_write_entries error, ErrorCode = %d \n",result));
                ret = result;
            }
        }
        else
        {
            D( ( DL_FATERROR,"In  exfat_remove_entry,fat_read_entries error, ErrorCode = %d \n",result));
            ret = result;
        }
    }
    else
    {
        D( ( DL_FATERROR,"In  exfat_remove_entry,custer number is invalid: 0x%x.",bclu));
        DSM_ASSERT(0,"In  exfat_remove_entry,custer number is invalid: 0x%x.",bclu);
        ret =  _ERR_FAT_INVALID_FAT_NUM;
    }
    if(dent)
    {
        DSM_Free(dent);
    }
    return ret;
}



INT32 exfat_get_null_entry_ino(struct inode *diri,UINT32 ecount,UINT64 *ino,UINT32 *nullcount)
{
    UINT32 cur_clus;
    UINT32 next_clus;
    UINT16 cursec;
    UINT16 secoff;
    UINT32 freebegclu = 0;
    UINT16 freebegsec = 0;
    UINT16 freebegoff = 0;
    UINT32 freenr = 0;
    EXFAT_SB_INFO* sb_info;
    EXFAT_EXT_DIR_ENTRY* ext_entry;
    UINT8* secbuf = NULL;
    EXFAT_DIR_ENTRY* pentry;
    BOOL hasset = FALSE;
    INT32 result;
    INT32 ret = _ERR_FAT_NO_MORE_NULL_ENTRY;

    secbuf = FAT_SECT_BUF_ALLOC();
    sb_info = &diri->i_sb->u.exfat_sb;
    ext_entry = &diri->u.exfat_i.ext_entry;
    if(*ino == 0)
    {
        cur_clus = ext_entry->FirstCluster;
        cursec = 0;
        secoff = 0;
    }
    else
    {
        cur_clus = EXFAT_INO2CLUS(*ino);
        cursec = EXFAT_INO2SEC(*ino);
        secoff = EXFAT_INO2EOFF(*ino);
        freenr = *nullcount;
    }

    while(EXFAT_IS_VALID_CLUSTER(sb_info, cur_clus))
    {
        //Find the free entry in current cluser.
        for(cursec = cursec; cursec < sb_info->SectorsPerCluster; cursec++)
        {
            // Find the free entry in current sector.
            DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
            result = exfat_read_cluster(sb_info, cur_clus, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != result)
            {
                ret = _ERR_FAT_READ_SEC_FAILED;
                D( ( DL_FATERROR,"In  exfat_get_null_entry_ino,fat_read_cluster error, result = %d \n",result));
                goto _func_end;
            }

            // find a group of consecutive free or deleting entriy.
            for(secoff = secoff; secoff < DEFAULT_SECSIZE; secoff += DEFAULT_DIRENTRYSIZE)
            {
                pentry = (EXFAT_DIR_ENTRY*)(secbuf+secoff);
                // Deleted entry or free entry.
                if(EXFAT_IS_DELETE_DENTRY(pentry->EntryType) ||
                    EXFAT_IS_FREE_DENTRY(pentry->EntryType))
                {
                    if(!hasset)
                    {
                        freebegsec = cursec;
                        freebegoff = secoff;
                        freebegclu = cur_clus;
                        hasset = TRUE;
                    }

                    freenr++;

                    if(freenr == ecount)
                    {
                        ret = ERR_SUCCESS;
                        goto _func_end;
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
            secoff = 0;
        }

        result = exfat_get_next_cluster(diri,cur_clus, &next_clus);
        if(ERR_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_get_null_entry_ino,exfat_get_next_cluster() return = %d.\n",result) );
            ret = result;
            goto _func_end;
        }
        cursec = 0;
        cur_clus = next_clus;
    }
_func_end:
    if(ERR_SUCCESS == ret)
    {
        *ino = EXFAT_CSO2INO(freebegclu,freebegsec,freebegoff);
        *nullcount = freenr;
        ret = ERR_SUCCESS;
    }
    else if(_ERR_FAT_NO_MORE_NULL_ENTRY == ret)
    {
        *ino = EXFAT_CSO2INO(freebegclu,freebegsec,freebegoff);
        *nullcount = freenr;
    }
    else
    {
        *ino = 0;
        *nullcount = 0;
    }
    return ret;
}


VOID exfat_make_entries(EXFAT_SB_INFO* sb_info,WCHAR* pszFileName,UINT16 attrib,UINT64 data_len, EXFAT_DIR_ENTRY* pdentries)
{
    EXFAT_FILE_DIR_ENTRY* pfile_entry;
    EXFAT_EXT_DIR_ENTRY* pext_entry;
    EXFAT_NAME_DIR_ENTRY* pname_entry;
    UINT32 name_len;
    UINT32 count = 0;
    UINT32 tmp_len = 0;
    UINT32 spare_len;
    WCHAR* pname;

    // Make exfat file name entries.
    spare_len = name_len = DSM_TcStrLen(pszFileName);
    count = 2;
    pname = pszFileName;
    while(spare_len != 0)
    {
        pname_entry = (EXFAT_NAME_DIR_ENTRY*)(&pdentries[count]);
        DSM_MemSet((UINT8*)pname_entry,0,EXFAT_DIR_ENTRY_SIZE);
        pname_entry->EntryType = EXFAT_ETYPE_FNAME;
        pname_entry->GenSeconFlags = 0;
        tmp_len = DSM_TcStrLen(pname) >= 15 ? 15:DSM_TcStrLen(pname);
        DSM_TcStrNCpy((WCHAR*)pname_entry->FileName,pname,tmp_len);
        spare_len -= tmp_len;
        pname += tmp_len;
        count ++;
    }
    // Make exfat ext entry.
    pext_entry = (EXFAT_EXT_DIR_ENTRY*)(&pdentries[1]);
    pext_entry->EntryType = EXFAT_ETYPE_EXT;
    pext_entry->GenSeconFlags = 0x3;
    pext_entry->Reserved1 = 0;
    pext_entry->NameLength = (UINT8)name_len;
    pext_entry->NameHash = exfat_file_name_hash(pszFileName);
    pext_entry->Reserved2 = 0;
    pext_entry->ValidDataLength = data_len;
    pext_entry->Reserved3 = 0;
    pext_entry->FirstCluster = 0;
    pext_entry->DataLength = data_len;

    // Make exfat file entry.
    pfile_entry = (EXFAT_FILE_DIR_ENTRY*)(&pdentries[0]);
    pfile_entry->EntryType = EXFAT_ETYPE_FILE;
    pfile_entry->SeconCount = (UINT8)(count - 1);
    pfile_entry->FileAttributes = attrib;
    pfile_entry->Reserved1 = 0;
    pfile_entry->CreatTime = DSM_GetFileTime();
    pfile_entry->LMTime = DSM_GetFileTime();
    pfile_entry->LATime = DSM_GetFileTime();
    pfile_entry->LM10msInc = DSM_GetTimeTeenth();
    pfile_entry->C10msInc = DSM_GetTimeTeenth();
    pfile_entry->LMTimezOffs = 0;
    pfile_entry->LATimezOffs = 0;
    DSM_MemSet(pfile_entry->Reserved2,0,7);
    pfile_entry->SetChecksum = exfat_GetEntryCheckSum((UINT8*)pdentries);
}


INT32 exfat_add_entry(struct inode *diri, EXFAT_DIR_ENTRY* pdentries,
            UINT32 fst_clus,UINT32* bclu, UINT16 *bsec, UINT16 *eoff)
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 freeclus = 0;
    UINT16 count = 0;
    UINT32 nullcount = 0;
    UINT64 ino = 0;
    EXFAT_FILE_DIR_ENTRY* file_entry;
    EXFAT_EXT_DIR_ENTRY* ext_entry;
    UINT32 curclu;
    UINT16 cursec;
    UINT16 secoff;
    EXFAT_SB_INFO* sb_info;
    UINT8* secbuf = NULL;

    sb_info = &diri->i_sb->u.exfat_sb;
    file_entry = (EXFAT_FILE_DIR_ENTRY*)(&pdentries[0]);
    ext_entry = (EXFAT_EXT_DIR_ENTRY*)(&pdentries[1]);
    count = file_entry->SeconCount + 1;

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        ret = _ERR_FAT_MALLOC_FAILED;
        D( ( DL_FATERROR,"in exfat_add_entry.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0,"in exfat_add_entry.FAT_SECT_BUF_ALLOC failed.");
        goto exfat_add_entry_end;
    }

    // Get null dentry region.
    while(nullcount < count)
    {
        result =exfat_get_null_entry_ino(diri,count,&ino,&nullcount);
        if(ERR_SUCCESS == result)
        {
            break;
        }
        else
        {
            // The root dir cluster number = 1.
            if(result == _ERR_FAT_NO_MORE_NULL_ENTRY
                && diri->u.exfat_i.CurClus == sb_info->RootDirectoryCluster)
            {
                ret = result;
                goto exfat_add_entry_end;
            }
            // Get a free cluster and add it to deentry.
            result = exfat_get_free_cluster(sb_info,sb_info->CurCluster, &freeclus);
            if(result == ERR_SUCCESS)
            {
                curclu = EXFAT_INO2CLUS(ino);
                result = exfat_set_bat_fat_entry(sb_info,curclu,freeclus,EXFAT_CLUSTER_FINAL, TRUE,diri);
                if(result != ERR_SUCCESS)
                {
                    ret = result;
                    goto exfat_add_entry_end;
                }
                result = exfat_set_cluser_zero(sb_info,freeclus);
                if(ERR_SUCCESS != result)
                {
                    ret = result;
                    goto exfat_add_entry_end;
                }
            }
            else
            {
                ret = result;
                goto exfat_add_entry_end;
            }
        }
    }

    if(EXFAT_IS_VALID_CLUSTER(sb_info,fst_clus))
    {
        ext_entry->FirstCluster = fst_clus;
    }
    else
    {
        result = exfat_get_free_cluster(sb_info,sb_info->CurCluster, &freeclus);
        if(result == ERR_SUCCESS)
        {
            result = exfat_set_bat_fat_entry(sb_info,fst_clus,freeclus,EXFAT_CLUSTER_FINAL, TRUE,NULL);
            if(result == ERR_SUCCESS)
            {
                ext_entry->FirstCluster = freeclus;
            }
            else
            {
                ret = result;
                goto exfat_add_entry_end;
            }
        }
        else
        {
            ret = result;
            goto exfat_add_entry_end;
        }
    }
    if(EXFAT_IS_DIR(file_entry->FileAttributes))
    {
        result = exfat_set_cluser_zero(sb_info,ext_entry->FirstCluster);
        if(ERR_SUCCESS != result)
        {
            ret = result;
            goto exfat_add_entry_end;
        }
    }
    // Write entries.
    curclu = EXFAT_INO2CLUS(ino);
    cursec = EXFAT_INO2SEC(ino);
    secoff = EXFAT_INO2EOFF(ino);
    // Update entry checksum.
    file_entry->SetChecksum = exfat_GetEntryCheckSum((UINT8*)pdentries);
    result = exfat_write_entries(diri,curclu,cursec,secoff,pdentries,count);
    if(ERR_SUCCESS == result)
    {
        *bclu = curclu;
        *bsec = cursec;
        *eoff = secoff;
    }
    else
    {
        ret = result;
    }

exfat_add_entry_end:
   if(secbuf)
   {
       FAT_SECT_BUF_FREE(secbuf);
   }

   return ret;
}


INT32 exfat_read_entries(struct inode *pinode,  UINT32 bclu,
                UINT16 bsec, UINT16 eoff, EXFAT_DIR_ENTRY* dir_entry, UINT16* ecount)
{
    UINT32 cur_clus = 0,next_clus = 0;
    UINT16  cursec = 0;
    UINT32 cureoff = 0;
    UINT8*  secbuf = NULL;
    INT32 result = _ERR_FAT_SUCCESS;
    UINT8 count = 0;
    EXFAT_DIR_ENTRY* pdentry;
    EXFAT_FILE_DIR_ENTRY* file_entry;
    EXFAT_SB_INFO* sb_info;

    sb_info = &pinode->i_sb->u.exfat_sb;
    file_entry = (EXFAT_FILE_DIR_ENTRY*)(&dir_entry[0]);
    cur_clus = bclu;
    cursec = (UINT8)bsec;
    cureoff = eoff;
    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D((DL_FATERROR, "exfat_read_entries: malloc failed."));
        DSM_ASSERT(0,"exfat_read_entries: malloc failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }
    while(EXFAT_IS_VALID_CLUSTER(sb_info,cur_clus))
    {
        for(cursec = cursec; cursec < sb_info->SectorsPerCluster; cursec ++)
        {
            result = exfat_read_cluster(sb_info, cur_clus, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != result)
            {
                result = _ERR_FAT_READ_SEC_FAILED;
                D( ( DL_FATERROR," exfat_read_entries,fat_read_cluster error, ErrorCode = %d\n",result));
                goto _func_end;
            }
            for(cureoff = cureoff; cureoff < sb_info->BytesPerSector;cureoff += EXFAT_DIR_ENTRY_SIZE)
            {
                pdentry = (EXFAT_DIR_ENTRY*)(secbuf + cureoff);
                dir_entry[count] = *pdentry;
                count++;

                if(count == file_entry->SeconCount + 1)
                {
                    result = _ERR_FAT_SUCCESS;
                   *ecount = count;
                    goto _func_end;
                }
            }
            cureoff = 0;
        }
        result = exfat_get_next_cluster(pinode,cur_clus, &next_clus);
        if(ERR_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_write_entries,exfat_get_next_cluster() return = %d.\n",result) );
            result = _ERR_FAT_ERROR;
            goto _func_end;
        }
        cur_clus = next_clus;
        cursec = 0;
    }

_func_end:
    if(secbuf)
    {
        FAT_SECT_BUF_FREE(secbuf);
    }
    return result;

}



INT32 exfat_write_entries(struct inode *pinode,
            UINT32 bclu, UINT16 bsec, UINT16 eoff, EXFAT_DIR_ENTRY* dentry, UINT16 ecount)
{
    UINT32 len = 0;
    UINT8* bufp = NULL;
    UINT8  secnum = 0;
    UINT32 cur_clus = 0,next_clus = 0;
    UINT32 clus_cnt = 0;
    UINT32 secoff = 0;
    UINT8* secbuf = NULL;
    UINT8  cheat = 1;
    INT32 result;
    INT32 iRet = _ERR_FAT_SUCCESS;
    EXFAT_SB_INFO* sb_info;

    sb_info = &pinode->i_sb->u.exfat_sb;
    bufp = (UINT8*)dentry;
    secnum = (UINT8)bsec;
    cur_clus = bclu;
    secoff = eoff;

    len = sizeof(FAT_DIR_ENTRY)*ecount;

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR,"in exfat_write_entries,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0,"in exfat_write_entries,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    while(cheat)
    {
        DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);

        if(secoff == 0)
        {
            if(len > DEFAULT_SECSIZE)
            {
                result = exfat_write_cluster(sb_info, cur_clus, secnum, bufp);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_write_cluster_0 error, ErrorCode = %d\n",iRet));
                    break;
                }

                bufp += DEFAULT_SECSIZE;
                secnum++;
                secoff = 0;
                len -= DEFAULT_SECSIZE;
            }
            else if(len == DEFAULT_SECSIZE)
            {
                result = exfat_write_cluster(sb_info, cur_clus, secnum, bufp);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_write_cluster_1 error, ErrorCode = %d\n",iRet));
                    break;
                }

                break;
            }
            else // secoff + len < DEFAULT_SECSIZE (1)
            {

                result = exfat_read_cluster(sb_info, cur_clus, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_read_cluster_0 error, ErrorCode = %d\n",iRet));
                    break;
                }

                DSM_MemCpy((UINT8*)(secbuf+secoff), bufp, len);
                result = exfat_write_cluster(sb_info, cur_clus, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_write_cluster_2 error, ErrorCode = %d\n",iRet));
                    break;
                }
                break;
            }
        }
        else if(secoff > 0)
        {
            if(secoff + len > DEFAULT_SECSIZE)
            {
                result = exfat_read_cluster(sb_info, cur_clus, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_read_cluster_1 error, ErrorCode = %d\n",iRet));
                    break;
                }
                DSM_MemCpy((UINT8*)(secbuf+secoff), bufp, DEFAULT_SECSIZE-secoff);

                result = exfat_write_cluster(sb_info, cur_clus, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_write_cluster_3 error, ErrorCode = %d\n",iRet));
                    break;
                }

                bufp += (DEFAULT_SECSIZE-secoff);

                len -= (DEFAULT_SECSIZE-secoff);
                secnum++;
                secoff = 0;
            }
            else //secoff + len <= DEFAULT_SECSIZE (1)
            {
                result = exfat_read_cluster(sb_info, cur_clus, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_read_cluster_2 error, ErrorCode = %d\n",iRet));
                    break;
                }

                DSM_MemCpy((UINT8*)(secbuf+secoff), bufp, len);

                result = exfat_write_cluster(sb_info, cur_clus, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != result)
                {
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    D( ( DL_FATERROR,"in exfat_write_entries,fat_write_cluster_4 error, ErrorCode = %d\n",iRet));
                    break;
                }
                break;
            }
        }
        if(secnum == sb_info->SectorsPerCluster) // Get to the end of the cluster
        {
            result = exfat_get_next_cluster(pinode,cur_clus, &next_clus);
            if(ERR_SUCCESS != result)
            {
                D( ( DL_FATERROR, "in exfat_write_entries,exfat_get_next_cluster() return = %d.\n",result) );
                result = _ERR_FAT_ERROR;
                break;
            }
            cur_clus = next_clus;
            clus_cnt ++;
            secnum = 0;
        }
    }// while(cheat)

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    }

    return iRet;
}


INT32 exfat_lookup_file(struct inode *inode,
                                           WCHAR* filefilter,
                                           UINT64 *cur_ino,
                                           UINT64 *nex_ino,
                                           WCHAR* filename,
                                           UINT16* ecount)
{
    INT32 result;
    INT32 ret = _ERR_FAT_SUCCESS;
    UINT32 curclus = 0;
    UINT32 nextclus = 0;
    UINT16  cursec = 0;
    UINT16 cureoff = 0;
    UINT32 clus = 0;
    UINT16 sec= 0;
    UINT16 eoff = 0;
    UINT8*  secbuf = NULL;
    WCHAR* tmp_filename = NULL;
    UINT8 count = 0;
    EXFAT_DENTRY_PARSE_STATE parse_state;
    EXFAT_DIR_ENTRY* dent = NULL;
    EXFAT_DIR_ENTRY* pdentry;
    EXFAT_FILE_DIR_ENTRY*      pFileEntry = NULL;
    EXFAT_EXT_DIR_ENTRY*      pExtEntry = NULL;
    UINT16 name_hash;
    EXFAT_SB_INFO* sb_info;

    sb_info = &inode->i_sb->u.exfat_sb;
    if(*cur_ino)
    {
        curclus = EXFAT_INO2CLUS(*cur_ino);
        cursec = EXFAT_INO2SEC(*cur_ino);
        cureoff = EXFAT_INO2EOFF(*cur_ino);
    }
    parse_state = EXFAT_DENTRY_PARSE_INIT;

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D((DL_FATERROR, "exfat_lookup_file: malloc secbuf failed."));
        DSM_ASSERT(0,"exfat_lookup_file: malloc secbuf failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    tmp_filename = NAME_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D((DL_FATERROR, "exfat_lookup_file: malloc filename failed."));
        DSM_ASSERT(0,"exfat_lookup_file: malloc filename failed.");
        ret =  _ERR_FAT_MALLOC_FAILED;
        goto end;
    }

    dent = DSM_MAlloc(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY));
    if(!dent)
    {
        D( ( DL_FATERROR,"exfat_lookup_file:malloc(%d) failed.",
                (EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY))));
        ret =  _ERR_FAT_MALLOC_FAILED;
        goto end;
    }
    DSM_MemSet(dent,0,(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY)));
    while(EXFAT_IS_VALID_CLUSTER(sb_info,curclus))
    {
        for(cursec = cursec; cursec < sb_info->SectorsPerCluster; cursec ++)
        {
            result = exfat_read_cluster(sb_info, curclus, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != result)
            {
                D( ( DL_FATERROR," exfat_lookup_file,fat_read_cluster error, ErrorCode = %d\n",result));
                ret = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }

            for(cureoff = cureoff; cureoff < sb_info->BytesPerSector;cureoff += DEFAULT_DIRENTRYSIZE)
            {
                pdentry = (EXFAT_DIR_ENTRY*)(secbuf + cureoff);
                switch(parse_state)
                {
                    case EXFAT_DENTRY_PARSE_INIT:
                        count = 0;
                        if(pdentry->EntryType == EXFAT_ETYPE_NULL)
                        {
                             parse_state = EXFAT_DENTRY_PARSE_NOT_FOUND;
                             goto end;
                        }
                        else if(pdentry->EntryType == EXFAT_ETYPE_FILE)
                        {
                             dent[count] = *pdentry;
                             pFileEntry =(EXFAT_FILE_DIR_ENTRY*)(&dent[count]);

                             if(pFileEntry->SeconCount >= 2
                                && pFileEntry->SeconCount < EXFAT_DIR_ENTRY_COUNT)
                             {
                                clus = curclus;
                                sec = cursec;
                                eoff = cureoff;
                                count++;
                                parse_state = EXFAT_DENTRY_PARSE_FILE;
                             }
                             else
                             {
                                 parse_state = EXFAT_DENTRY_PARSE_INIT;
                             }
                        }
                        else
                        {
                            parse_state = EXFAT_DENTRY_PARSE_INIT;
                         }
                        break;
                    case EXFAT_DENTRY_PARSE_FILE:
                        if(pdentry->EntryType == EXFAT_ETYPE_NULL)
                        {
                             parse_state = EXFAT_DENTRY_PARSE_NOT_FOUND;
                             goto end;
                        }
                        else if(pdentry->EntryType == EXFAT_ETYPE_EXT)
                        {
                             parse_state = EXFAT_DENTRY_PARSE_EXT;
                             dent[count] = *pdentry;
                             pExtEntry = (EXFAT_EXT_DIR_ENTRY*)(&dent[count]);
                             count++;
                        }
                        else
                        {
                             parse_state = EXFAT_DENTRY_PARSE_INIT;
                        }
                        break;
                    case EXFAT_DENTRY_PARSE_EXT:
                        if(pdentry->EntryType == EXFAT_ETYPE_NULL)
                        {
                             parse_state = EXFAT_DENTRY_PARSE_NOT_FOUND;
                             goto end;
                        }
                        if(pdentry->EntryType != EXFAT_ETYPE_FNAME)
                        {
                            parse_state = EXFAT_DENTRY_PARSE_INIT;
                            break;
                        }
                         dent[count] = *pdentry;
                         if(count == pFileEntry->SeconCount)
                         {
                            if(pFileEntry->SetChecksum != exfat_GetEntryCheckSum((UINT8*)dent))
                            {
                                parse_state = EXFAT_DENTRY_PARSE_INIT;
                                break;
                            }
                            DSM_MemSet(tmp_filename,0,((FS_PATH_LEN + 2*LEN_FOR_NULL_CHAR) * SIZEOF(WCHAR)));
                            exfat_build_file_name((EXFAT_NAME_DIR_ENTRY*)dent,(count + 1),tmp_filename);
                            name_hash = exfat_file_name_hash(tmp_filename);
                            if(pExtEntry->NameHash ==  name_hash )
                            {
                                if(pattern_match(tmp_filename,filefilter) == PATTERN_CMP_MATCH)
                                {
                                    count ++;
                                    parse_state = EXFAT_DENTRY_PARSE_FOUND;
                                    goto end;
                                }
                                else
                                {
                                    D( ( DL_FATERROR," exfat_lookup_file,pattern_match fail"));
                                    parse_state = EXFAT_DENTRY_PARSE_INIT;
                                }
                            }
                            else
                            {
                                D( ( DL_FATERROR," exfat_lookup_file,exfat_file_name_hash fail,pExtEntry->NameHash = 0x%x, name_hash = 0x%x",pExtEntry->NameHash,name_hash));
                                parse_state = EXFAT_DENTRY_PARSE_INIT;
                            }
                        }
                        else
                        {
                            count ++;
                        }
                        break;
                    default:
                        break;
                }
            }
            cureoff = 0;
        }
        result = exfat_get_next_cluster(inode,curclus, &nextclus);
        if(ERR_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_lookup_file,exfat_get_next_cluster() return = %d.\n",result) );
            result = _ERR_FAT_ERROR;
            break;
        }
        cursec = 0;
        curclus = nextclus;
    }
    parse_state = EXFAT_DENTRY_PARSE_NOT_FOUND;

end:
    if(EXFAT_DENTRY_PARSE_NOT_FOUND ==  parse_state)
    {
         ret = _ERR_FAT_FILE_NOT_EXISTED;
    }
    else if(EXFAT_DENTRY_PARSE_FOUND == parse_state)
    {
        if(filename != NULL)
        {
            DSM_TcStrCpy(filename,tmp_filename);
        }
        *cur_ino = EXFAT_CSO2INO(clus,sec,eoff);
        if(nex_ino)
        {
            *nex_ino =EXFAT_CSO2INO(curclus,cursec,(cureoff + DEFAULT_DIRENTRYSIZE));
        }
        *ecount = count;
        ret = _ERR_FAT_SUCCESS;
    }
    if(secbuf)
    {
        FAT_SECT_BUF_FREE(secbuf);
    }
    if(tmp_filename)
    {
        NAME_BUF_FREE(tmp_filename);
    }
    if(dent)
    {
        DSM_Free(dent);
    }
    return ret;
}


INT32 exfat_get_name_for_entrys(struct inode *pinode, WCHAR* filename, UINT32 bclu, UINT16 bsec, UINT16 eoff)
{
    INT32 result;                    // call function result.
    INT32 iRet = _ERR_FAT_FILE_NOT_EXISTED;    // Retrun value.
    UINT32 iMaxSecCount;              // max sector count to read.
    UINT8* pSecBuf = NULL;                       // Sector buffer to read.
    EXFAT_FILE_DIR_ENTRY* file_entry = NULL;
    EXFAT_EXT_DIR_ENTRY* ext_entry = NULL;
    EXFAT_DIR_ENTRY* dent = NULL;
    UINT32 cur_clus = 0;              // Current cluster.
    UINT32 next_clus = 0;              // Next cluster.
    UINT32 clus_cnt = 0;
    UINT16 iCurSec = 0;               // Current sector.
    UINT16 iCurEOff = 0;              // Current entry offset.
    UINT32 count = 0;
    UINT32 i = 0;
    EXFAT_SB_INFO* sb_info;


    sb_info = &pinode->i_sb->u.exfat_sb;

    // Check input paramert.
    if(!EXFAT_IS_VALID_CLUSTER(sb_info, bclu))
    {
        D( ( DL_FATERROR, "In exfat_get_name_for_entrys paramert error.bclu  == %d\n", bclu) );
        return _ERR_FAT_PARAM_ERROR;
    }

    cur_clus = bclu;
    iCurSec = bsec;
    iCurEOff = eoff;

    // malloc sector buffer.
    pSecBuf = FAT_SECT_BUF_ALLOC();
    if(NULL == pSecBuf)
    {
        D( ( DL_FATERROR, "In exfat_get_name_for_entrys malloc error \n") );
        DSM_ASSERT(0,"In exfat_get_name_for_entrys malloc error \n");
        return _ERR_FAT_MALLOC_FAILED;
    }
    dent = DSM_MAlloc(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY));
    if(!dent)
    {
        D( ( DL_FATERROR,"exfat_get_name_for_entrys:malloc(%d) failed.",
                (EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY))));
        return _ERR_FAT_MALLOC_FAILED;
    }
    DSM_MemSet(dent,0,(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY)));
    iMaxSecCount = sb_info->SectorsPerCluster;
    do
    {
        // loop read sector,if root dir, loop count = 32 else loop count = secPerClus.
        for(; iCurSec < iMaxSecCount; iCurSec++)
        {
            DSM_MemSet(pSecBuf, 0x00, DEFAULT_SECSIZE);
            result = exfat_read_cluster(sb_info, cur_clus, iCurSec, pSecBuf);
            if(_ERR_FAT_SUCCESS != result)
            {
                D( ( DL_FATERROR, "In exfat_get_name_for_entrys DRV_BLOCK_READ error ErrorCode = %d \n",result) );
                iRet = _ERR_FAT_READ_SEC_FAILED;
                goto fat_get_name_for_entrys;
            }

            // Get current position and correlative long entries.
            for(i = iCurEOff; i < DEFAULT_SECSIZE; i += DEFAULT_DIRENTRYSIZE)
            {
                 DSM_MemCpy(&dent[count],(pSecBuf + i),EXFAT_DIR_ENTRY_SIZE);
                 if(0 == count)
                 {
                     if(EXFAT_ETYPE_FILE != dent[count].EntryType)
                     {
                        iRet = _ERR_FAT_FILE_NOT_EXISTED;
                        D((DL_FATERROR, "exfat_get_name_for_entrys: count = %d,entry_type = 0x%x.",count,dent[count].EntryType));
                        goto fat_get_name_for_entrys;
                     }
                     file_entry = (EXFAT_FILE_DIR_ENTRY*)(&dent[count]);
                 }
                 else if(1 == count)
                 {
                     if(EXFAT_ETYPE_EXT != dent[count].EntryType)
                     {
                        iRet = _ERR_FAT_FILE_NOT_EXISTED;
                        D((DL_FATERROR, "exfat_get_name_for_entrys: count = %d,entry_type = 0x%x.",dent[count].EntryType));
                        goto fat_get_name_for_entrys;
                     }
                     ext_entry = (EXFAT_EXT_DIR_ENTRY*)(&file_entry[count]);
                 }
                 else
                 {
                     if(EXFAT_ETYPE_FNAME != dent[count].EntryType)
                     {
                        iRet = _ERR_FAT_FILE_NOT_EXISTED;
                        D((DL_FATERROR, "exfat_get_name_for_entrys: count = %d,entry_type = 0x%x.",count,dent[count].EntryType));
                        goto fat_get_name_for_entrys;
                     }
                     if(count == file_entry->SeconCount)
                     {
                         if(file_entry->SetChecksum != exfat_GetEntryCheckSum((UINT8*)dent))
                         {
                            iRet = _ERR_FAT_FILE_NOT_EXISTED;
                            D((DL_FATERROR, "exfat_get_name_for_entrys: SetChecksum: 0x%x != 0x%x.",file_entry->SetChecksum,exfat_GetEntryCheckSum((UINT8*)dent)));
                            goto fat_get_name_for_entrys;
                         }
                         exfat_build_file_name((EXFAT_NAME_DIR_ENTRY*)dent,count + 1,filename);
                         if(ext_entry->NameHash != exfat_file_name_hash(filename))
                         {
                            iRet = _ERR_FAT_FILE_NOT_EXISTED;
                            D((DL_FATERROR, "exfat_get_name_for_entrys: NameHash: 0x%x != 0x%x.",ext_entry->NameHash,exfat_file_name_hash(filename)));
                            goto fat_get_name_for_entrys;
                         }
                         iRet = ERR_SUCCESS;
                         goto fat_get_name_for_entrys;
                     }
                 }
                 count ++;
            }
            iCurEOff = 0;
        }
        result = exfat_get_next_cluster(pinode,cur_clus, &next_clus);
        if(ERR_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_get_name_for_entrys,exfat_get_next_cluster() return = %d.\n",result) );
            iRet = _ERR_FAT_ERROR;
            break;
        }
        cur_clus = next_clus;
        clus_cnt ++;
        iCurSec = 0;
        iCurEOff = 0;

    }while(EXFAT_IS_VALID_CLUSTER(sb_info,cur_clus));

fat_get_name_for_entrys:
    if(NULL != pSecBuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)pSecBuf);
    }
    if(dent)
    {
        DSM_Free(dent);
    }
    return iRet;
}
#endif

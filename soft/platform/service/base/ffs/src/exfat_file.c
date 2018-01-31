#ifdef FS_SUPPORT_EXFAT
#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "fat_base.h"
#include "exfat_base.h"
#include "fat_local.h"
#include "exfat_local.h"
#include "fs.h"
#include "fs_base.h"
#include "fs.h"

INT32 exfat_file_read(struct file * filp, INT8 * buf, INT32 len)
{
    struct inode *pinode;
    EXFAT_EXT_DIR_ENTRY* ext_entry;
    EXFAT_SB_INFO* sb = NULL;
    UINT64 fpos = 0;
    UINT64 fsize = 0;
    UINT32 secperclu = 0;

    INT32 rcount = 0;
    UINT32 rsize = 0;
    UINT32  cursec, secnum;
    UINT32 clunum, secoff;
    UINT8* secbuf;
    INT8* bufp = NULL;

    INT32 result = _ERR_FAT_SUCCESS;

     pinode = filp->f_inode;
     ext_entry = &pinode->u.exfat_i.ext_entry;
     sb = &(pinode->i_sb->u.exfat_sb);
     fpos = filp->f_pos;
     fsize = (UINT32)ext_entry->DataLength;
     secperclu = (UINT32)sb->SectorsPerCluster;
     bufp = buf;


    // set the rsize.
    if(fpos > fsize)
    {
        D( ( DL_FATERROR, "in exfat_file_read. fpos(%d) >  fsize(%d).\n",fpos,fsize) );
        return 0;
    }
    else if(fpos+len > fsize)
    {
        rsize = (UINT32)(fsize - fpos);
    }
    else
    {
        rsize = len;
    }

    // malloc the secbuf for read sector.
    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR,"in exfat_file_read,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0,"in exfat_file_read,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    // Get next cluster, secnum, secoff
    result = exfat_fpos2CSO(filp->f_inode, fpos, &clunum, &secnum, &secoff);
    if (result != _ERR_FAT_SUCCESS)
    {
        // Out of file size,error.
        D( ( DL_FATERROR, "exfat_file_read: fat_fpos2CSO error,fpos = 0x%llx,result = %d.\n",fpos,result) );
        result = _ERR_FAT_ERROR;
        goto end;
    }

    cursec = secnum;

    while(EXFAT_IS_VALID_CLUSTER(sb, clunum))
    {
        for(cursec = secnum; cursec < secperclu; cursec++)
        {
            // DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
            result = exfat_read_cluster(sb, clunum, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != result)
            {
                D( ( DL_FATERROR, "exfat_file_read:fat_read_cluster() failed. Local error code:%d. clunum = 0x%x, secnum = 0x%x.\n",result, clunum, cursec) );
                result = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }

            if(0 == secoff)
            {
                if(rsize > DEFAULT_SECSIZE)
                {
                    DSM_MemCpy(bufp, secbuf, DEFAULT_SECSIZE);

                    filp->f_pos += DEFAULT_SECSIZE;
                    bufp += DEFAULT_SECSIZE;
                    rsize -= DEFAULT_SECSIZE;
                    rcount += DEFAULT_SECSIZE;
                    secoff = 0;
                }
                else if (rsize == DEFAULT_SECSIZE)
                {
                    DSM_MemCpy(bufp, secbuf, DEFAULT_SECSIZE);

                    filp->f_pos += DEFAULT_SECSIZE;
                    bufp += DEFAULT_SECSIZE;
                    rsize -= DEFAULT_SECSIZE;
                    rcount += DEFAULT_SECSIZE;
                    secoff = 0;
                    goto end;
                }
                else //secoff + rsize <= DEFAULT_SECSIZE (1)
                {
                    DSM_MemCpy(bufp, secbuf, (UINT32)rsize);

                    filp->f_pos += rsize;
                    bufp += rsize;
                    secoff = secoff + (UINT32)rsize;
                    rcount += rsize;
                    rsize = 0;
                    goto end;
                }
            }
            else // secoff > 0
            {
                if(secoff + rsize > DEFAULT_SECSIZE)
                {
                    DSM_MemCpy(bufp, secbuf+secoff, DEFAULT_SECSIZE-secoff);

                    filp->f_pos += (DEFAULT_SECSIZE-secoff);
                    bufp += (DEFAULT_SECSIZE-secoff);
                    rsize -= (DEFAULT_SECSIZE-secoff);
                    rcount += (DEFAULT_SECSIZE-secoff);
                    secoff = 0;
                }
                else //secoff + rsize <= DEFAULT_SECSIZE (1)
                {
                    DSM_MemCpy(bufp, secbuf+secoff, rsize);

                    filp->f_pos += rsize;
                    bufp += rsize;
                    secoff = secoff + rsize;
                    rcount += rsize;
                    rsize = 0;
                    goto end;
                }
            }
        }

        // Get next cluster, secnum, secoff
        result = exfat_fpos2CSO(filp->f_inode, filp->f_pos, &clunum, &secnum, &secoff);
        if (result != _ERR_FAT_SUCCESS)
        {
           // Out of file size,error.
            D( ( DL_FATERROR, "exfat_file_read: fat_fpos2CSO error2 !!!\n") );
            goto end;
        }
        DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);
    }

end:
    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    }
    rcount = _ERR_FAT_SUCCESS == result ? rcount: result;
    return rcount;
}


// Check if inode relating with FAT cache.
BOOL exfat_is_inode_relating_with_fatcache(CONST struct inode *psInode, EXFAT_CACHE **ppsFATCache)
{
    EXFAT_CACHE* psFatCache = NULL;
    PFILE_INODE_LIST psTmpNode = NULL;

    if (NULL == psInode)
    {
        return FALSE;
    }
    psFatCache = &psInode->i_sb->u.exfat_sb.BATCache;
    //psFatCache = exfat_get_fat_cache(psInode->i_dev);
    psTmpNode = psFatCache->psFileInodeList;

    while (psTmpNode)
    {
        if (psTmpNode->psInode == psInode)
        {
            *ppsFATCache = psFatCache;
            return TRUE;
        }
        psTmpNode = psTmpNode->next;
    }

    return FALSE;

}


INT32 exfat_relate_inode_with_fatcache(EXFAT_CACHE *psCache, struct inode *psInode)
{
    PFILE_INODE_LIST psTmpNode = NULL;

    if (NULL == psCache || NULL == psInode)
    {
        D( ( DL_FATERROR, "in exfat_relate_inode_with_fatcache, input parameter is NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    psTmpNode = psCache->psFileInodeList;

    while (psTmpNode)
    {
        // Check the node if is related.
        if (psTmpNode->psInode == psInode)
        {
            return _ERR_FAT_SUCCESS;
        }
        psTmpNode = psTmpNode->next;
    }

    if (NULL == (psTmpNode = DSM_MAlloc(sizeof(FILE_INODE_LIST))))
    {
        DSM_ASSERT(0,"exfat_relate_inode_with_fatcache: 1.malloc(0x%x) failed.",(sizeof(FAT_CACHE)));
        return _ERR_FAT_MALLOC_FAILED;
    }

    psTmpNode->psInode = psInode;
    psTmpNode->next = psCache->psFileInodeList;
    psCache->psFileInodeList = psTmpNode;
    return _ERR_FAT_SUCCESS;

}


INT32 exfat_synch_relating_file(EXFAT_SB_INFO *sb_info, EXFAT_CACHE *psCache)
{
    INT32 iRet = _ERR_FAT_SUCCESS;
    PFILE_INODE_LIST psRelatingInode = NULL;
    PFILE_INODE_LIST psTmpNode = NULL;

    if (NULL == sb_info || NULL == psCache)
    {
        D( ( DL_FATERROR, "in fat_synch_relating_file, input parameter is NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    psRelatingInode = psCache->psFileInodeList;

    while (psRelatingInode)
    {
        if(psRelatingInode->psInode)
        {
            iRet = exfat_update_dentry(psRelatingInode->psInode);
            if (_ERR_FAT_SUCCESS != iRet)
            {
                // DSM_ASSERT(0,"fat_synch_relating_file:fat_update_dentry fail.err_code = %d.",iRet);
                // FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
                // return iRet;
                psTmpNode = psRelatingInode;
                psRelatingInode = psRelatingInode->next;
                DSM_Free(psTmpNode);
                psCache->psFileInodeList = psRelatingInode;
            }
            else
            {
                psTmpNode = psRelatingInode;
                psRelatingInode = psRelatingInode->next;
                DSM_Free(psTmpNode);
                psCache->psFileInodeList = psRelatingInode;
            }
         }
         else
         {
            psTmpNode = psRelatingInode;
            psRelatingInode = psRelatingInode->next;
            DSM_Free(psTmpNode);
         }
    }
    psCache->psFileInodeList = NULL;
    return _ERR_FAT_SUCCESS;

}

// Flush fat cache.
INT32 exfat_synch_cache(EXFAT_SB_INFO *sb_info, EXFAT_CACHE *psCache)
{
    INT32 ret = _ERR_FAT_SUCCESS;

    if (NULL == sb_info || NULL == psCache)
    {
        D( ( DL_FATERROR, "in exfat_synch_cache, _ERR_FAT_PARAM_ERROR.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    if (!psCache->iDirty)
    {
        return _ERR_FAT_SUCCESS;
    }

    ret = DRV_BLOCK_WRITE( sb_info->sb->s_dev, psCache->iSecNum, psCache->szSecBuf );
    if(_ERR_FAT_SUCCESS != ret)
    {
        //psFATCache->dev = 0;
        //psFATCache->FATSecNum = 0;
        D( ( DL_FATERROR, "in exfat_synch_cache, DRV_BLOCK_WRITE_0 ErrorCode = %d.\n", ret) );
        return _ERR_FAT_WRITE_SEC_FAILED;
    }

    psCache->iDirty = 0;
    ret = exfat_synch_relating_file(sb_info, psCache);
    D( ( DL_FATDETAIL, "********* End of exfat_synch_cache.\n") );
    return ret;
}


INT32 exfat_set_bat_entry_value(EXFAT_SB_INFO *sb_info, UINT32 fatent, UINT32 value,
                                                                BOOL bSynchFlag, struct inode *psInode)
{
    EXFAT_CACHE *psBATCache = NULL;
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 SecNum;
    UINT32 ByteOffs;
    UINT32 BitOffs;

    psBATCache = &sb_info->BATCache;
    SecNum = EXFAT_BAT_SECTOR(sb_info,fatent);
    ByteOffs = EXFAT_BAT_BYTEOFFS(sb_info,fatent);
    BitOffs = EXFAT_BAT_BITOFFS(sb_info,fatent);

    // Read block to BAT cache buffer.
    if(psBATCache->iSecNum != SecNum)
    {
        // Exchange fat fat cache block.
        result = exfat_synch_cache(sb_info, psBATCache);
        if(_ERR_FAT_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_set_bat_entry_value,fat_synch_fat_cache()2 return = %d.\n",result) );
            ret = _ERR_FAT_READ_SEC_FAILED;
            goto _func_end;
        }

        result = DRV_BLOCK_READ( sb_info->sb->s_dev, SecNum, psBATCache->szSecBuf);
        if(_ERR_FAT_SUCCESS != result)
        {
            D((DL_FATERROR, "DRV_BLOCK_READ failed3, dev = %d, FATSecNum = %d, result = %d, goto end.\n",
               sb_info->sb->s_dev, SecNum, result));
            psBATCache->iSecNum = 0;
            ret = _ERR_FAT_READ_SEC_FAILED;
            goto _func_end;
        }
        psBATCache->iSecNum = SecNum;
        psBATCache->iDirty = 0;
    }
    if(0 == value)
    {
        psBATCache->szSecBuf[ByteOffs] &= (UINT8)(~(0x1 << BitOffs) & 0xff);
        psBATCache->iDirty = 1;
    }
    else
    {
        psBATCache->szSecBuf[ByteOffs] |= (UINT8)(0x1 << BitOffs);
        psBATCache->iDirty = 1;
    }
    if (bSynchFlag)
    {
        result = exfat_synch_cache(sb_info,psBATCache);
        if(_ERR_FAT_SUCCESS != result)
        {
            D((DL_FATERROR, "exfat_synch_cache failed result = %d.\n",result));
            ret = result;
            goto _func_end;
        }
    }
    else
    {
        if (psInode != NULL)
        {
            result = exfat_relate_inode_with_fatcache(psBATCache, psInode);
            if(_ERR_FAT_SUCCESS != result)
            {
                D((DL_FATERROR, "exfat_relate_inode_with_fatcache failed result = %d.\n",result));
                ret = result;
                goto _func_end;
            }
        }
    }
_func_end:
    return ret;
}


INT32 exfat_set_fat_entry_value(EXFAT_SB_INFO *sb_info, UINT32 fatent, UINT32 value,
                                BOOL bSynchFlag, struct inode *psInode)
{
    UINT32 FATOffset;
    UINT32 ThisFATSecNum;
    UINT32 ThisFATEntOffset;
    EXFAT_CACHE *psFatCache = NULL;
    UINT32 bufvalue32;
    UINT32 localvalue;
    INT32 result;
    INT32 ret = ERR_SUCCESS;

    // Check fatent validity.
    if (!EXFAT_IS_VALID_CLUSTER(sb_info,fatent))
    {
        D((DL_FATERROR, "exfat_set_fat_entry_value: fatent = 0x%x, _ERR_FAT_PARAM_ERROR!\n", fatent));
        return _ERR_FAT_ERROR;
    }
    psFatCache = &sb_info->FATCache;
    FATOffset = fatent * 4;
    ThisFATSecNum = EXFAT_GET_FAT_SEC_NUM(sb_info,FATOffset);
    ThisFATEntOffset = EXFAT_GET_FAT_ENT_OFFS(sb_info,FATOffset);

    if(psFatCache->iSecNum != ThisFATSecNum)
    {
        if(psFatCache->iSecNum != 0)
        {
            result = DRV_BLOCK_WRITE( sb_info->sb->s_dev, psFatCache->iSecNum, psFatCache->szSecBuf );
            if(_ERR_FAT_SUCCESS != result)
            {
                psFatCache->iSecNum = 0;
                D( ( DL_FATERROR, "in exfat_set_fat_entry_value,DRV_BLOCK_WRITE ErrorCode = %d.\n",result) );
                ret = _ERR_FAT_READ_SEC_FAILED;
                goto _func_end;
            }
        }
        result = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum , psFatCache->szSecBuf );
        if(_ERR_FAT_SUCCESS != result)
        {
            psFatCache->iSecNum = 0;
            D( ( DL_FATERROR, "in exfat_set_fat_entry_value,DRV_BLOCK_READ_0 ErrorCode = %d.\n",result) );
            ret = _ERR_FAT_READ_SEC_FAILED;
            goto _func_end;
        }
        psFatCache->iSecNum = ThisFATSecNum;
        psFatCache->iDirty = 0;
    }

    localvalue = value;

    bufvalue32 = (UINT32)(localvalue & 0xFFFFFFFF);
    *(UINT32*)(psFatCache->szSecBuf + ThisFATEntOffset) = \
        *(UINT32*)(psFatCache->szSecBuf + ThisFATEntOffset) & 0x00000000;
    *(UINT32*)(psFatCache->szSecBuf + ThisFATEntOffset) = \
        (*(UINT32*)(psFatCache->szSecBuf + ThisFATEntOffset)) | bufvalue32;
    psFatCache->iDirty = 1;

    if (bSynchFlag)
    {
        if(psFatCache->iSecNum != 0)
        {
            result = exfat_synch_cache(sb_info, psFatCache);
            if(_ERR_FAT_SUCCESS != result)
            {
                D( ( DL_FATERROR, "in exfat_set_fat_entry_value,fat_synch_fat_cache() return = %d.\n",result) );
                ret = _ERR_FAT_READ_SEC_FAILED;
                goto _func_end;
            }
        }
    }
    else
    {
        if (psInode != NULL)
        {
            ret = exfat_relate_inode_with_fatcache(psFatCache, psInode);
        }
    }

_func_end:
    return ret;
}


INT32 exfat_set_bat_fat_entry(EXFAT_SB_INFO *sb_info, UINT32 pre_fatent,UINT32 fatent, UINT32 value,
                                                                BOOL bSynchFlag, struct inode *psInode)
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    struct exfat_inode_info*  exfat_inode;
    EXFAT_FILE_DIR_ENTRY *file_entry;
    EXFAT_EXT_DIR_ENTRY *ext_entry;
    UINT32 ClusCnt;
    UINT32 CurClus;
    UINT32 NextClus;

    if(psInode)
    {
        exfat_inode = &psInode->u.exfat_i;
        file_entry = &exfat_inode->file_entry;
        ext_entry = &exfat_inode->ext_entry;
        if(EXFAT_GENSECON_FLAGS_NOFATCHAIN(ext_entry->GenSeconFlags))
        {
            ClusCnt = (UINT32)(ext_entry->ValidDataLength/(sb_info->BytesPerSector*sb_info->SectorsPerCluster));
            if(ext_entry->FirstCluster + ClusCnt != fatent)
            {
                ext_entry->GenSeconFlags &= ~0x2;
                for(CurClus = ext_entry->FirstCluster; CurClus < (ext_entry->FirstCluster + ClusCnt); CurClus ++)
                {
                    NextClus = (CurClus + 1);
                    result = exfat_set_fat_entry_value(sb_info,CurClus,NextClus,FALSE,psInode);
                    if(result != ERR_SUCCESS)
                    {
                        D( ( DL_FATERROR, "exfat_set_fat_entry_value failed, CurClus = 0x%x,NextClus = 0x%x,result = %d.\n",
                            CurClus,
                            NextClus,
                            result) );
                        ret = result;
                        goto _func_end;
                    }
                }
                exfat_inode->Dirty = 1;
            }
        }
        if(EXFAT_IS_DIR(file_entry->FileAttributes))
        {
            ext_entry->ValidDataLength += (sb_info->BytesPerSector*sb_info->SectorsPerCluster);
            ext_entry->DataLength = ext_entry->ValidDataLength;
            exfat_inode->Dirty = 1;
        }
        if(!EXFAT_GENSECON_FLAGS_NOFATCHAIN(ext_entry->GenSeconFlags))
        {
            result = exfat_set_fat_entry_value(sb_info,fatent,value,FALSE,psInode);
            if(result != ERR_SUCCESS)
            {
                D( ( DL_FATERROR, "exfat_set_fat_entry_value failed, fatent = 0x%x,value = 0x%x result = %d.\n",
                    fatent,
                    value,
                    result) );
                ret = result;
                goto _func_end;
            }
            if(EXFAT_IS_VALID_CLUSTER(sb_info,pre_fatent))
            {
                result = exfat_set_fat_entry_value(sb_info,pre_fatent,fatent,bSynchFlag,psInode);
                if(result != ERR_SUCCESS)
                {
                    D( ( DL_FATERROR, "exfat_set_fat_entry_value failed,pre_fatent = 0x%x, fatent = 0x%x,value = 0x%x result = %d.\n",
                        pre_fatent,
                        fatent,
                        value,
                        result) );
                    ret = result;
                    goto _func_end;
                }
            }
        }
    }
    result = exfat_set_bat_entry_value(sb_info,fatent,value,bSynchFlag,psInode);
    if(result != ERR_SUCCESS)
	{
        D( ( DL_FATERROR, "exfat_set_bat_entry_value failed, result = %d.\n",result) );
        ret = result;
        goto _func_end;
    }
_func_end:
    return ret;
}

INT32 exfat_update_dentry(struct inode *inode)
{
    EXFAT_DIR_ENTRY  *dir_entries = NULL;
    EXFAT_FILE_DIR_ENTRY *file_entry = NULL;
    EXFAT_EXT_DIR_ENTRY *ext_entry = NULL;
    EXFAT_INODE_INFO *exfat_inode = NULL;
    UINT32 bclu;
    UINT16 bsec, eoff;
    UINT16 ecount = 0;
    UINT32 result = _ERR_FAT_SUCCESS;
    INT32 ret = _ERR_FAT_SUCCESS;

    exfat_inode = &(inode->u.exfat_i);
    file_entry = &(inode->u.exfat_i.file_entry);
    ext_entry = &(inode->u.exfat_i.ext_entry);
    if(DATA_DIRTY == exfat_inode->Dirty)
    {
        bclu = EXFAT_INO2CLUS(inode->i_ino);
        bsec = EXFAT_INO2SEC(inode->i_ino);
        eoff = EXFAT_INO2EOFF(inode->i_ino);
        dir_entries = DSM_MAlloc(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY));
        if(!dir_entries)
        {
            D( ( DL_FATERROR,"In  exfat_update_dentry,malloc(%d) failed.",
                    (EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY))));
            return _ERR_FAT_MALLOC_FAILED;
        }
        DSM_MemSet(dir_entries,0,(EXFAT_DIR_ENTRY_COUNT*sizeof(EXFAT_DIR_ENTRY)));
        result = exfat_read_entries(inode, bclu, bsec, eoff, dir_entries, &ecount);
        if(_ERR_FAT_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_update_dentry,exfat_read_entries ErrorCode = %d.\n",result) );
            ret = result;
            goto end;
        }
        file_entry->LATime = DSM_GetFileTime();
        file_entry->LMTime = DSM_GetFileTime();
        file_entry->LM10msInc = DSM_GetTimeTeenth();
        file_entry->C10msInc = DSM_GetTimeTeenth();
        // file_entry->FileAttributes = inode->i_mode;
        // ext_entry->ValidDataLength = inode->i_size;
        DSM_MemCpy(&dir_entries[0],file_entry,EXFAT_DIR_ENTRY_SIZE);
        DSM_MemCpy(&dir_entries[1],ext_entry,EXFAT_DIR_ENTRY_SIZE);
        // Update entry checksum.
        file_entry = (EXFAT_FILE_DIR_ENTRY*)&dir_entries[0];
        file_entry->SetChecksum = exfat_GetEntryCheckSum((UINT8*)dir_entries);
        result = exfat_write_entries(inode, bclu, bsec, eoff, dir_entries, ecount);
        if(_ERR_FAT_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_update_dentry,fat_write_entries_0 ErrorCode = %d.\n",result) );
            ret = result;
            goto end;
        }
        exfat_inode->Dirty = DATA_CLEAN;
    }
end:
    if(dir_entries)
    {
        DSM_Free(dir_entries);
    }
    return ret;
}


/********************************************************************************/
// Function:
//   Write the file.
// Parameter:
//   inode[in], Point to the parent inode.
//   filep[in],Point to the file struct to indicate mode,flags,cout,pos etc infromation.
//   buf[out],Storage location for data.
//   len[in],Indicate the buf data length.
// Return value:
//     if successful,return value is >= 0,else return value < 0, as following:
//      _ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      _ERR_FAT_MALLOC_FAILED:      Allocte memory failed.
//      _ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      _ERR_FAT_DISK_FULL:          The logical disk is full
//*******************************************************************************
INT32 exfat_file_write(struct file * filp, INT8 * buf, UINT32 lenToWrite)
{
    struct inode *pinode;
    EXFAT_EXT_DIR_ENTRY* ext_entry = NULL;
    EXFAT_SB_INFO* sb = NULL;
    EXFAT_INODE_INFO* fat_inode = NULL;

    UINT64 fsize = 0;
    UINT8* bufp = NULL;
    UINT16 secperclu = 0;

    UINT32 clunum = 0, secnum = 0, secoff = 0;
    UINT8* secbuf;

    UINT32 preCluster = 0;
    UINT32 sizeWriten = 0;
    UINT32 thisLenToWrite = 0;

    INT32 ret = _ERR_FAT_SUCCESS;

    pinode = filp->f_inode;
    ext_entry = (EXFAT_EXT_DIR_ENTRY*)&(pinode->u.exfat_i.ext_entry);
    sb = &(pinode->i_sb->u.exfat_sb);
    fat_inode = &(pinode->u.exfat_i);

    fsize = ext_entry->DataLength;
    bufp = (UINT8*)buf;
    secperclu = sb->SectorsPerCluster;

    if ( filp->f_flags & FS_O_APPEND )
    {
        filp->f_pos = pinode->i_size;
    }
    else
    {
        // Do nothing.
    }

    if(filp->f_pos > fsize)
    {
        D((DL_FATERROR, "exfat_file_write: _ERR_FAT_WRITE_EXCEED.\n"));
        return _ERR_FAT_WRITE_EXCEED;
    }

    // malloc the secbuf for read sector.

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
       DSM_HstSendEvent(0xba000001);
        D(( DL_FATERROR,"in exfat_file_write,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0,"in exfat_file_write,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }
    if(EXFAT_IS_VALID_CLUSTER(sb, fat_inode->CurClus))
    {
        clunum = fat_inode->CurClus;
    }
    else
    {
        clunum = ext_entry->FirstCluster;
    }

    while (sizeWriten < lenToWrite)
    {
        preCluster = clunum;
        ret = exfat_fpos2CSO(pinode, filp->f_pos, &clunum, &secnum, &secoff);
        if(_ERR_FAT_NOT_MATCH_CLUSTER == ret)
        {
            secnum = 0;
            secoff = 0;
            ret = exfat_get_free_cluster(sb, preCluster,&clunum);
            if(ret < 0 || clunum < 2)
            {
                D( ( DL_FATERROR,"In  exfat_file_write,fat_get_free_cluster() failed! ErrorCode = %d\n",ret));
                goto failed;
            }
            DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);
            // Write the fat cache to disk,if not to do, the laster cluster value zero is  make for in system broken.
            ret = exfat_set_bat_fat_entry(sb, preCluster,clunum,CLUSTER_NULL, TRUE, pinode);
            // ret = fat_set_fat_entry_value(sb, clunum, CLUSTER_NULL, TRUE, inode);
            if(_ERR_FAT_SUCCESS != ret)
            {
                D((DL_FATERROR, "exfat_file_write: fat_set_fat_entry_value()1 return %d.\n", ret));
                ret = _ERR_FAT_WRITE_SEC_FAILED;
                goto failed;
            }
            exfat_SetPos(pinode,(UINT32)filp->f_pos,clunum);
        }
        DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);
        // write to destination cluster.
        for (; secnum < secperclu && sizeWriten < lenToWrite; ++secnum, secoff = 0)
        {
            // compute current writing size.
            thisLenToWrite = ((secoff + (lenToWrite - sizeWriten)) > DEFAULT_SECSIZE) \
                                             ? (DEFAULT_SECSIZE - secoff) : (lenToWrite - sizeWriten);

            if (0 == secoff && ((lenToWrite - sizeWriten) >= DEFAULT_SECSIZE))
            {
                // if the writing size equit to sector size, write to the sector directly use a section of input buffer.

                ret = exfat_write_cluster(sb, clunum, secnum, bufp);
                if(_ERR_FAT_SUCCESS != ret)
                {
                    D((DL_FATERROR, "exfat_file_write: fat_write_cluster() return %d.\n", ret));
                    ret = _ERR_FAT_WRITE_SEC_FAILED;
                    DSM_HstSendEvent(0xba000006);
                    DSM_HstSendEvent((0-ret));
                    goto failed;
                }
            }
            else
            {
                // Read destination sector content .
                ret = exfat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != ret)
                {
                    D((DL_FATERROR, "exfat_file_write: fat_read_cluster() return %d.\n", ret));
                    ret = _ERR_FAT_READ_SEC_FAILED;
                    DSM_HstSendEvent(0xba000007);
                    DSM_HstSendEvent((0-ret));
                    goto failed;
                }

                // Update the content of needing to write.
                DSM_MemCpy((UINT8*)(secbuf+secoff), bufp, thisLenToWrite);
                ret = exfat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != ret)
                {
                    D((DL_FATERROR, "exfat_file_write: fat_write_cluster()2 return %d.\n", ret));
                    ret = _ERR_FAT_WRITE_SEC_FAILED;
                    DSM_HstSendEvent(0xba000008);
                    DSM_HstSendEvent((0-ret));
                    goto failed;
                }
            }

            sizeWriten +=thisLenToWrite;
            filp->f_pos += (UINT64)thisLenToWrite;
            bufp += thisLenToWrite;
        }
    }

failed:

    fat_inode->Dirty = DATA_DIRTY;
    if(filp->f_pos >= fsize)
    {
        ext_entry->DataLength = filp->f_pos;
        ext_entry->ValidDataLength =  filp->f_pos;
        filp->f_inode->i_size = filp->f_pos;
    }

    // Write the entry of this inode into disk
    // if the file size is change.
    // It changes other entry member as well.

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    }
    return ret;
}


//
// Extend file.
//
INT32 exfat_file_extend(struct file * filp, UINT64 SizeExtend)
{
    struct inode *pinode;
    EXFAT_EXT_DIR_ENTRY* ext_entry = NULL;
    EXFAT_SB_INFO* sb = NULL;
    EXFAT_INODE_INFO* fat_inode = NULL;
    UINT16 secperclu = 0;
    UINT32 clunum = 0, secnum = 0, secoff = 0;
    UINT8* secbuf = NULL;

    UINT32 LastCluster = 0;
    UINT64 sizeExtended = 0;
    UINT64 thisLenToWrite;
    INT32 ret = _ERR_FAT_SUCCESS;

    pinode = filp->f_inode;
    ext_entry = &(pinode->u.exfat_i.ext_entry);
    sb = &(pinode->i_sb->u.exfat_sb);
    fat_inode = &(pinode->u.exfat_i);
    secperclu = sb->SectorsPerCluster;

    if(0 == SizeExtend)
    {
        return _ERR_FAT_SUCCESS;
    }

    // malloc the secbuf for read sector.
    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR,"in exfat_file_extend,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0,"in exfat_file_extend,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    ret = exfat_fpos2CSO(pinode, pinode->i_size, &clunum, &secnum, &secoff);
    if (_ERR_FAT_SUCCESS == ret)
    {
        DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);
        LastCluster = clunum;
        for (; secnum < secperclu && sizeExtended < SizeExtend; ++secnum, secoff = 0)
        {
            thisLenToWrite = ((secoff + (SizeExtend - sizeExtended)) > DEFAULT_SECSIZE) \
                                             ? (DEFAULT_SECSIZE - secoff) : (SizeExtend - sizeExtended);
            if (0 == secoff)
            {
                DSM_MemZero(secbuf, DEFAULT_SECSIZE);
                ret = exfat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != ret)
                {
                    D((DL_FATERROR, "exfat_file_extend: fat_write_cluster() return %d.\n", ret));
                    ret = _ERR_FAT_WRITE_SEC_FAILED;
                    goto failed;
                }
            }
            else
            {
                ret = exfat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != ret)
                {
                    D((DL_FATERROR, "exfat_file_extend: fat_read_cluster() return %d.\n", ret));
                    ret = _ERR_FAT_READ_SEC_FAILED;
                    goto failed;
                }
                DSM_MemZero((UINT8*)(secbuf+secoff), (UINT32)thisLenToWrite);
                ret = exfat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != ret)
                {
                    D((DL_FATERROR, "exfat_file_extend: fat_write_cluster()2 return %d.\n", ret));
                    ret = _ERR_FAT_WRITE_SEC_FAILED;
                    goto failed;
                }
            }
            sizeExtended += thisLenToWrite;
        }

    }

    while (sizeExtended < SizeExtend)
    {
        ret = exfat_get_free_cluster(sb, LastCluster, &clunum);
        if(ret != 0)
        {
            D( ( DL_FATERROR,"In  exfat_file_extend,fat_get_free_cluster() failed! ErrorCode = %d\n",ret));
            goto failed;
        }
        DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);
        ret = exfat_set_fat_entry_value(sb, clunum, CLUSTER_NULL, FALSE, pinode);
        if(_ERR_FAT_SUCCESS != ret)
        {
            D((DL_FATERROR, "exfat_file_extend: fat_set_fat_entry_value()1 return %d.\n", ret));
            ret = _ERR_FAT_WRITE_SEC_FAILED;
            goto failed;
        }

        exfat_SetPos(pinode,(UINT32)filp->f_pos,clunum);

        if (0 == LastCluster)
        {
            ext_entry->DataLength = ext_entry->ValidDataLength = clunum;
            fat_inode->Dirty = DATA_DIRTY;
        }
        CSW_TRACE(BASE_DSM_TS_ID, "exfat_file_extend:curclu = 0x%x.\n", clunum);
        sizeExtended += (( SizeExtend - sizeExtended >= (UINT32)(DEFAULT_SECSIZE * secperclu)) ? \
                                    DEFAULT_SECSIZE * secperclu : SizeExtend - sizeExtended);
        LastCluster = clunum;
    }

failed:

    fat_inode->Dirty = DATA_DIRTY;
	ext_entry->ValidDataLength += sizeExtended;
	ext_entry->DataLength = ext_entry->ValidDataLength;
	filp->f_inode->i_size = ext_entry->DataLength;

    // Write the entry of this inode into disk.
    // if the file size is change.
    // It changes other entry member as well.
    ret = exfat_update_dentry(pinode);
    filp->f_inode->i_mtime = DSM_GetFileTime();
    filp->f_inode->i_atime = DSM_GetFileTime();
    FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    return ret;
}


//
// Truncat file.
//
INT32 exfat_file_truncate(struct inode *psInode, UINT64 nFileSize)
{
    INT32 ret = _ERR_FAT_SUCCESS;
    INT32 result;
    EXFAT_SB_INFO* sb_info = NULL ;
    UINT32 cur_clus = 0;
    UINT32 nex_clus = 0;
    UINT32 nSec = 0, nOff = 0;
    UINT32 lst_clus = 0, trunc_clus = 0;
    EXFAT_FILE_DIR_ENTRY* file_entry = NULL;
    EXFAT_EXT_DIR_ENTRY* ext_entry = NULL;

    D((DL_FATDETAIL, "Enter in exfat_file_truncate, nFileSize = 0x%llx\n", nFileSize));
    sb_info = &(psInode->i_sb->u.exfat_sb);
    file_entry =  &(psInode->u.exfat_i.file_entry);
    ext_entry =  &(psInode->u.exfat_i.ext_entry);

    if (nFileSize > ext_entry->ValidDataLength)
    {
        D( ( DL_FATERROR, "in exfat_file_truncate, truncate size greater than file size.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }
    if (nFileSize == ext_entry->ValidDataLength)
    {
        return _ERR_FAT_SUCCESS;
    }

    psInode->i_atime = \
    psInode->i_mtime = \
    file_entry->LATime = \
    file_entry->LMTime =  DSM_GetFileTime();

    psInode->i_size = ext_entry->DataLength = ext_entry->ValidDataLength = nFileSize;
    psInode->u.exfat_i.Dirty = DATA_DIRTY;
    ret = exfat_update_dentry(psInode);
    if (ret != _ERR_FAT_SUCCESS)
    {
        goto end;
    }

    if(nFileSize == 0)
    {
        lst_clus = ext_entry->FirstCluster;
        trunc_clus = ext_entry->FirstCluster;
    }
    else
    {
        ret = exfat_fpos2CSO(psInode, nFileSize, &cur_clus, &nSec, &nOff);
        if (ret != _ERR_FAT_SUCCESS)
        {
            ret = _ERR_FAT_ERROR;
            goto end;
        }
        if(EXFAT_IS_VALID_CLUSTER(sb_info,cur_clus))
        {
            lst_clus = cur_clus;
            result = exfat_get_next_cluster(psInode,cur_clus, &nex_clus);
            if(ERR_SUCCESS != result)
            {
                D( ( DL_FATERROR, "in exfat_file_truncate,exfat_get_next_cluster() return = %d.\n",result) );
                ret = _ERR_FAT_ERROR;
                goto end;
            }
            if(EXFAT_IS_VALID_CLUSTER(sb_info,nex_clus))
            {
                trunc_clus = nex_clus;
            }
        }
    }
    if(EXFAT_IS_VALID_CLUSTER(sb_info,trunc_clus))
    {
        ret = exfat_trunc_fatentry(psInode, lst_clus,trunc_clus);
        if(_ERR_FAT_SUCCESS != ret)
        {
            D((DL_FATERROR, "in exfat_file_truncate,exfat_trunc_fatentry error, ErrorCode = %d\n",ret));
            goto end;
        }
     }
end:
    return ret;
}

#endif


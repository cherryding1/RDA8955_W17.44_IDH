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
#include "fs.h"

extern FAT_CACHE* g_FatCache;
extern INT32 g_TstFsErrCode;

INT32 fat_file_read(struct file * filp, INT8 * buf, INT32 len)
{
    struct inode *pinode;
    FAT_DIR_ENTRY* d_entry = NULL;
    FAT_SB_INFO* sb = NULL;
    UINT32 fpos = 0;
    UINT32 fsize = 0;
    UINT32 secperclu = 0;

    INT32 rcount = 0;
    UINT32 rsize = 0;
    UINT32  cursec = 0, secnum = 0;
    UINT32 clunum = 0, secoff = 0;
    UINT32 pre_clunum = 0;
    UINT8* secbuf;
    INT8* bufp = NULL;

    INT32 iResult = _ERR_FAT_SUCCESS;

    pinode = filp->f_inode;

     d_entry = &pinode->u.fat_i.entry;
     sb = &(pinode->i_sb->u.fat_sb);
     fpos = (UINT32)filp->f_pos;
     fsize = d_entry->DIR_FileSize;
     secperclu = (UINT32)sb->iSecPerClus;
     bufp = buf;


    // set the rsize.
    if(fpos > fsize)
    {
        D( ( DL_FATERROR, "in fat_file_read. fpos(0x%x) >  fsize(0x%x).\n", fpos, fsize) );
        return _ERR_FAT_READ_EXCEED;
    }
    else if(fpos + len > fsize)
    {
        rsize = fsize - fpos;
    }
    else
    {
        rsize = len;
    }

    // malloc the secbuf for read sector.
    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR, "in fat_file_read,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_file_read,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    // Get next cluster, secnum, secoff
    iResult = fat_fpos2CSO(filp->f_inode, fpos,  &pre_clunum, &clunum, &secnum, &secoff);
    if (iResult != _ERR_FAT_SUCCESS)
    {
        // Out of file size,error.
        D( ( DL_FATERROR, "fat_file_read: fat_fpos2CSO error!!!\n") );
        g_TstFsErrCode = 1019;
        iResult = _ERR_FAT_ERROR;
        goto end;
    }

    cursec = secnum;

    while(!fat_is_last_cluster(sb, clunum))
    {
        for(cursec = secnum; cursec < secperclu; cursec++)
        {
            // DSM_MemSet(secbuf, 0x00, DEFAULT_SECSIZE);
            iResult = fat_read_cluster(sb, clunum, cursec, secbuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D( ( DL_FATERROR, "Call fat_read_cluster() failed. Local error code:%d. clunum = 0x%x, secnum = 0x%x.\n", iResult, clunum, cursec) );
                iResult = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }

            if(0 == secoff)
            {
                if(rsize > DEFAULT_SECSIZE)
                {
                    DSM_MemCpy(bufp, secbuf, DEFAULT_SECSIZE);

                    filp->f_pos += (UINT64)DEFAULT_SECSIZE;
                    bufp += DEFAULT_SECSIZE;
                    rsize -= DEFAULT_SECSIZE;
                    rcount += DEFAULT_SECSIZE;
                    secoff = 0;
                }
                else if (rsize == DEFAULT_SECSIZE)
                {
                    DSM_MemCpy(bufp, secbuf, DEFAULT_SECSIZE);

                    filp->f_pos += (UINT64)DEFAULT_SECSIZE;
                    bufp += DEFAULT_SECSIZE;
                    rsize -= DEFAULT_SECSIZE;
                    rcount += DEFAULT_SECSIZE;
                    secoff = 0;
                    goto end;
                }
                else //secoff + rsize <= DEFAULT_SECSIZE (1)
                {
                    DSM_MemCpy(bufp, secbuf, rsize);

                    filp->f_pos += (UINT64)rsize;
                    bufp += rsize;
                    secoff = secoff + rsize;
                    rcount += rsize;
                    rsize = 0;
                    goto end;
                }
            }
            else // secoff > 0
            {
                if(secoff + rsize > DEFAULT_SECSIZE)
                {
                    DSM_MemCpy(bufp, secbuf + secoff, DEFAULT_SECSIZE - secoff);

                    filp->f_pos += (UINT64)(DEFAULT_SECSIZE - secoff);
                    bufp += (DEFAULT_SECSIZE - secoff);
                    rsize -= (DEFAULT_SECSIZE - secoff);
                    rcount += (DEFAULT_SECSIZE - secoff);
                    secoff = 0;
                }
                else //secoff + rsize <= DEFAULT_SECSIZE (1)
                {
                    DSM_MemCpy(bufp, secbuf + secoff, rsize);

                    filp->f_pos += (UINT64)rsize;
                    bufp += rsize;
                    secoff = secoff + rsize;
                    rcount += rsize;
                    rsize = 0;
                    goto end;
                }
            }
        }

        // Get next cluster, secnum, secoff
        iResult = fat_fpos2CSO(filp->f_inode,(UINT32)filp->f_pos,&pre_clunum, &clunum, &secnum, &secoff);
        if (iResult != _ERR_FAT_SUCCESS)
        {
            // Out of file size,error.
            D( ( DL_FATERROR, "fat_file_read: fat_fpos2CSO error2 !!!\n") );
            iResult = _ERR_FAT_ERROR;
            g_TstFsErrCode = 1020;
            goto end;
        }
        DSM_ASSERT((BOOL)(clunum >= 2), "clunum = %d.", clunum);

    }

end:

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }

    rcount = _ERR_FAT_SUCCESS == iResult ? rcount : iResult;
    return rcount;
}


// Check if inode relating with FAT cache.
BOOL fat_is_inode_relating_with_fatcache(CONST struct inode *psInode, FAT_CACHE **ppsFATCache)
{
    FAT_CACHE *psFatCache = NULL;
    PFILE_INODE_LIST psTmpNode = NULL;

    if (NULL == psInode)
    {
        return FALSE;
    }

    psFatCache = fat_get_fat_cache(psInode->i_dev);
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


// inode relating with FAT cache.
INT32 fat_relate_inode_with_fatcache(FAT_CACHE *psFATCache, struct inode *psInode)
{
    PFILE_INODE_LIST psTmpNode = NULL;

    if (NULL == psFATCache || NULL == psInode)
    {
        D( ( DL_FATERROR, "in fat_relate_inode_with_fatcache, input parameter is NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    psTmpNode = psFATCache->psFileInodeList;

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
        DSM_ASSERT(0, "fat_relate_inode_with_fatcache: 1.malloc(0x%x) failed.", (sizeof(FAT_CACHE)));
        return _ERR_FAT_MALLOC_FAILED;
    }

    psTmpNode->psInode = psInode;
    psTmpNode->next = psFATCache->psFileInodeList;
    psFATCache->psFileInodeList = psTmpNode;
    return _ERR_FAT_SUCCESS;

}


// Synch the inode relating the FAT chache.
INT32 fat_synch_relating_file(FAT_SB_INFO *sb_info, FAT_CACHE *psFATCache)
{
    INT32 iRet = _ERR_FAT_SUCCESS;
    PFILE_INODE_LIST psRelatingInode = NULL;
    PFILE_INODE_LIST psTmpNode = NULL;

    if (NULL == sb_info || NULL == psFATCache)
    {
        D( ( DL_FATERROR, "in fat_synch_relating_file, input parameter is NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }


    psRelatingInode = psFATCache->psFileInodeList;

    while (psRelatingInode)
    {
        if(psRelatingInode->psInode)
        {
            iRet = fat_update_dentry(psRelatingInode->psInode);
            if (_ERR_FAT_SUCCESS != iRet)
            {
                // DSM_ASSERT(0,"fat_synch_relating_file:fat_update_dentry fail.err_code = %d.",iRet);
                // FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
                // return iRet;
                psTmpNode = psRelatingInode;
                psRelatingInode = psRelatingInode->next;
                DSM_Free(psTmpNode);
                psFATCache->psFileInodeList = psRelatingInode;
            }
            else
            {
                psTmpNode = psRelatingInode;
                psRelatingInode = psRelatingInode->next;
                DSM_Free(psTmpNode);
                psFATCache->psFileInodeList = psRelatingInode;
            }
        }
        else
        {
            psTmpNode = psRelatingInode;
            psRelatingInode = psRelatingInode->next;
            DSM_Free(psTmpNode);
        }
    }
    psFATCache->psFileInodeList = NULL;
    return _ERR_FAT_SUCCESS;

}


// Flush fat cache.
INT32 fat_synch_fat_cache(FAT_SB_INFO *sb_info, FAT_CACHE *psFATCache)
{
    INT32 iRet = _ERR_FAT_SUCCESS;

    D( ( DL_FATDETAIL, "********* Enter in fat_synch_fat_cache, dev = %d, FATSecNum = %d, bDirty = %d.\n",
         psFATCache->iDevNo, psFATCache->iSecNum, psFATCache->iDirty) );
    if (NULL == sb_info || NULL == psFATCache)
    {
        D( ( DL_FATERROR, "in fat_set_fat_entry_value, _ERR_FAT_PARAM_ERROR.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    if (!psFATCache->iDirty)
    {
        return _ERR_FAT_SUCCESS;
    }

    iRet = DRV_BLOCK_WRITE( psFATCache->iDevNo, psFATCache->iSecNum, psFATCache->szSecBuf );
    if(_ERR_FAT_SUCCESS != iRet)
    {
        //psFATCache->dev = 0;
        //psFATCache->FATSecNum = 0;
        D( ( DL_FATERROR, "in fat_synch_fat_cache, DRV_BLOCK_WRITE_0 ErrorCode = %d.\n", iRet) );
        return _ERR_FAT_WRITE_SEC_FAILED;
    }

    if(FAT12 == sb_info->iFATn)
    {
        iRet = DRV_BLOCK_WRITE( psFATCache->iDevNo, psFATCache->iSecNum + 1,
                                psFATCache->szSecBuf + DEFAULT_SECSIZE );
        if(_ERR_FAT_SUCCESS != iRet)
        {
            //psFATCache->dev = 0;
            //psFATCache->FATSecNum = 0;
            D( ( DL_FATERROR, "in fat_synch_fat_cache, DRV_BLOCK_WRITE_1 ErrorCode = %d.\n", iRet) );
            return _ERR_FAT_WRITE_SEC_FAILED;
        }
    }

    // Flush FSInfo cache.
    if(FAT32 == sb_info->iFATn)
    {
        fat_synch_fsinfo(sb_info);
    }

    psFATCache->iDirty = 0;
    iRet = fat_synch_relating_file(sb_info, psFATCache);

    D( ( DL_FATDETAIL, "********* End of fat_synch_fat_cache.\n") );
    return iRet;
}




/********************************************************************************/
// Function:
//   This function is used to get FAT entry value.
// Parameter:
//   sb_info[in], Point to the struct of FAT_SB_INFO.
//   fatent[in,the postion in FAT, based on 0
//   value[out],the value prepared to set to the postion.
// Return value:
//      _ERR_FAT_SUCCESS indicates a successful operation.
//         And unsuccessful operation is denoted by the err code as following.
//      _ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      _ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      _ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************
INT32 fat_get_fat_entry_value(FAT_SB_INFO *sb_info, UINT32 fatent, UINT32 *value)
{
    UINT32 FATOffset;
    UINT32 ThisFATSecNum;
    UINT32 ThisFATEntOffset;
    FAT_CACHE *psFatCache = NULL;
    INT32 iResult;
    INT32 iRet;

    if (fatent > ((sb_info->iDataSec / sb_info->iSecPerClus) + 1) || fatent < 2)
    {
        D((DL_FATERROR, "fat_set_fat_entry_value: fatent = 0x%x, _ERR_FAT_PARAM_ERROR!\n", fatent));
        g_TstFsErrCode = 1021;
        return _ERR_FAT_ERROR;
    }

    psFatCache = fat_get_fat_cache(sb_info->sb->s_dev);
    if(NULL == psFatCache)
    {
        D((DL_FATERROR, "fat_get_fat_entry_value: fat cache is null,s_dev = 0x%x.\n", sb_info->sb->s_dev));
        return _ERR_FAT_CACHE_IS_NULL;
    }

    iRet = _ERR_FAT_SUCCESS;

    if(sb_info->iFATn == FAT12)
    {
        FATOffset = fatent + (fatent / 2);
    }
    else if(sb_info->iFATn == FAT16)
    {
        FATOffset = fatent * 2;
    }
    else // FAT32
    {
        FATOffset = fatent * 4;
    }

    ThisFATSecNum = GET_FAT_SEC_NUM(sb_info, FATOffset);
    ThisFATEntOffset = GET_FAT_ENT_OFFS(sb_info, FATOffset);

    if (psFatCache->iSecNum != ThisFATSecNum)
    {
        if (psFatCache->iDevNo != 0 && psFatCache->iSecNum != 0)
        {
            iResult = fat_synch_fat_cache(sb_info, psFatCache);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D( ( DL_FATERROR, "in fat_set_fat_entry_value,fat_synch_fat_cache() return = %d.\n", iResult) );
                iRet = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }
        }

        iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum , psFatCache->szSecBuf );
        if(_ERR_FAT_SUCCESS != iResult)
        {
            psFatCache->iSecNum = 0;
            D( ( DL_FATERROR, "in fat_set_fat_entry_value,DRV_BLOCK_READ_0 ErrorCode = %d.\n", iResult) );
            iRet = _ERR_FAT_READ_SEC_FAILED;
            goto end;
        }
        if(sb_info->iFATn == FAT12)
        {
            iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum + 1,
                                      psFatCache->szSecBuf + DEFAULT_SECSIZE );
            if(_ERR_FAT_SUCCESS != iResult)
            {
                psFatCache->iSecNum = 0;
                D( ( DL_FATERROR, "in fat_set_fat_entry_value,DRV_BLOCK_READ_1 ErrorCode = %d.\n", iResult) );
                iRet = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }
        }

        psFatCache->iSecNum = ThisFATSecNum;
        psFatCache->iDirty = 0;
    }

    if(sb_info->iFATn == FAT12)
    {
        *value = MAKEFATUINT16(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset + 1]);
        if(fatent & 0x0001)  // cluster number is ODD
        {
            *value = (UINT16)((*value >> 4) & 0x0fff);
        }
        else // cluster number is EVEN
        {
            *value = (UINT16)(*value & 0x0fff);
        }
    }
    else if(sb_info->iFATn == FAT16)
    {
        *value = MAKEFATUINT16(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset + 1]);
    }
    else
    {
        *value = MAKEFATUINT32(psFatCache->szSecBuf[ThisFATEntOffset],
                               psFatCache->szSecBuf[ThisFATEntOffset + 1],
                               psFatCache->szSecBuf[ThisFATEntOffset + 2],
                               psFatCache->szSecBuf[ThisFATEntOffset + 3]);
        *value &= 0x0FFFFFFF;
    }


end:
    return iRet;
}



/********************************************************************************/
// Function:
//   This function is used to set FAT.
// Parameter:
//   sb_info[in], Point to the struct of FAT_SB_INFO.
//   fatent[in,the postion in FAT, based on 0
//   value[in],the value prepared to set to the postion.
// Return value:
//      _ERR_FAT_SUCCESS indicates a successful operation.
//         And unsuccessful operation is denoted by the err code as following.
//      _ERR_FAT_READ_SEC_FAILED:    Read the sector failed.
//      _ERR_FAT_WRITE_SEC_FAILED:   Write the sector failed.
//      _ERR_FAT_MALLOC_FAILED       allocte memory failed.
//*******************************************************************************
INT32 fat_set_fat_entry_value(FAT_SB_INFO *sb_info, UINT32 fatent, UINT32 value,
                              BOOL bSynchFlag, struct inode *psInode)
{
    UINT32 FATOffset;
    UINT32 ThisFATSecNum;
    UINT32 ThisFATEntOffset;
    FAT_CACHE *psFatCache = NULL;
    UINT16 bufvalue;
    UINT32 bufvalue32;
    UINT32 localvalue;
    INT32 iResult;
    INT32 iRet;
    UINT32 FreeClusCnt = 0;


    // Check fatent validity.
    if (!IS_VALID_CLUSTER(sb_info, fatent))
    {
        D((DL_FATERROR, "fat_set_fat_entry_value: fatent = 0x%x, _ERR_FAT_PARAM_ERROR!\n", fatent));
        g_TstFsErrCode = 1022;
        return _ERR_FAT_ERROR;
    }

    psFatCache = fat_get_fat_cache(sb_info->sb->s_dev);
    if(NULL == psFatCache)
    {
        D((DL_FATERROR, "fat_set_fat_entry_value: fat cache is null,s_dev = 0x%x.\n", sb_info->sb->s_dev));
        return _ERR_FAT_CACHE_IS_NULL;
    }

    iRet = _ERR_FAT_SUCCESS;

    if(sb_info->iFATn == FAT12)
    {
        FATOffset = fatent + (fatent / 2);
    }
    else if(sb_info->iFATn == FAT16)
    {
        FATOffset = fatent * 2;
    }
    else // FAT32
    {
        FATOffset = fatent * 4;
    }


    ThisFATSecNum = GET_FAT_SEC_NUM(sb_info, FATOffset);
    ThisFATEntOffset = GET_FAT_ENT_OFFS(sb_info, FATOffset);

    if (psFatCache->iSecNum != ThisFATSecNum)
    {
        if (psFatCache->iDevNo != 0 && psFatCache->iSecNum != 0)
        {
            iResult = fat_synch_fat_cache(sb_info, psFatCache);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D( ( DL_FATERROR, "in fat_set_fat_entry_value,fat_synch_fat_cache() return = %d.\n", iResult) );
                iRet = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }
        }

        iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum , psFatCache->szSecBuf );
        if(_ERR_FAT_SUCCESS != iResult)
        {
            psFatCache->iSecNum = 0;
            D( ( DL_FATERROR, "in fat_set_fat_entry_value,DRV_BLOCK_READ_0 ErrorCode = %d.\n", iResult) );
            iRet = _ERR_FAT_READ_SEC_FAILED;
            goto end;
        }
        if(sb_info->iFATn == FAT12)
        {
            iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum + 1,
                                      psFatCache->szSecBuf + DEFAULT_SECSIZE );
            if(_ERR_FAT_SUCCESS != iResult)
            {
                psFatCache->iSecNum = 0;
                D( ( DL_FATERROR, "in fat_set_fat_entry_value,DRV_BLOCK_READ_1 ErrorCode = %d.\n", iResult) );
                iRet = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }
        }

        psFatCache->iSecNum = ThisFATSecNum;
        psFatCache->iDirty = 0;
    }

    localvalue = value;
    if(sb_info->iFATn == FAT12)
    {
        if(fatent & 0x0001) // cluster number is ODD
        {
            localvalue = ((localvalue << 4) & 0xfff0);
            bufvalue = MAKEFATUINT16(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset + 1]);
            bufvalue &= 0x000f;
        }
        else // cluster number is EVEN
        {
            localvalue = localvalue & 0x0fff;
            bufvalue = MAKEFATUINT16(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset + 1]);
            bufvalue &= 0xf000;
        }

        bufvalue |= (UINT16)localvalue;
        DSM_MemCpy(psFatCache->szSecBuf + ThisFATEntOffset, &bufvalue, sizeof(UINT16));
    }
    else if(sb_info->iFATn == FAT16)
    {
        bufvalue = (UINT16)localvalue;
        DSM_MemCpy(psFatCache->szSecBuf + ThisFATEntOffset, &bufvalue, sizeof(UINT16));
    }
    else
    {
        bufvalue32 = (UINT32)(localvalue & 0x0FFFFFFF);
        *(UINT32 *)(psFatCache->szSecBuf + ThisFATEntOffset) = \
                *(UINT32 *)(psFatCache->szSecBuf + ThisFATEntOffset) & 0xF0000000;
        *(UINT32 *)(psFatCache->szSecBuf + ThisFATEntOffset) = \
                (*(UINT32 *)(psFatCache->szSecBuf + ThisFATEntOffset)) | bufvalue32;
    }

    if(1 == sb_info->iFreeCnterInitFlag)
    {
        if(CLUSTER_FREE == value)
        {
            if(sb_info->iFreeCluCnt  < (sb_info->iDataSec / sb_info->iSecPerClus))
            {
                sb_info->iFreeCluCnt ++;
            }
            else
            {
                sb_info->iFreeCnterInitFlag = 0;
            }
        }

        if(CLUSTER_NULL == value)
        {
            if(sb_info->iFreeCluCnt > 0)
            {
                sb_info->iFreeCluCnt --;
            }
            else
            {
                // DSM_ASSERT(0,"FreeCluCnt = 0.");
                sb_info->iFreeCnterInitFlag = 0;
                iResult = fat_stat_free_cluster(sb_info, &FreeClusCnt);
                if(iResult == ERR_SUCCESS)
                {
                    if(FreeClusCnt == 0)
                    {
                        iRet = _ERR_FAT_DISK_FULL;
                        goto end;
                    }
                }
                else
                {
                    iRet = _ERR_FAT_DISK_FULL;
                    goto end;
                }
            }
        }
    }


    if (bSynchFlag)
    {
        psFatCache->iDirty = 1;
        if(psFatCache->iSecNum != 0)
        {
            iResult = fat_synch_fat_cache(sb_info, psFatCache);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D( ( DL_FATERROR, "in fat_set_fat_entry_value,fat_synch_fat_cache() return = %d.\n", iResult) );
                iRet = _ERR_FAT_READ_SEC_FAILED;
                goto end;
            }
        }
    }
    else
    {
        if (psInode != NULL)
        {
            iRet = fat_relate_inode_with_fatcache(psFatCache, psInode);
        }
        psFatCache->iDirty = 1;
    }

end:
    return iRet;
}


INT32 fat_update_dentry(struct inode *inode)
{
    FAT_DIR_ENTRY* d_entry = NULL;
    FAT_SB_INFO* sb = NULL;
    FAT_INODE_INFO* fat_inode = NULL;
    UINT32 fileTime;
    UINT32 lErrCode = _ERR_FAT_SUCCESS;
    UINT8  entbuf[DEFAULT_DIRENTRYSIZE];
    UINT32 bclu;
    UINT16 bsec, eoff;
    UINT8 ecount;
    UINT8*  secbuf = NULL;

    d_entry = &(inode->u.fat_i.entry);
    sb = &(inode->i_sb->u.fat_sb);
    fat_inode = &(inode->u.fat_i);

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR,"in fat_update_dentry,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0,"in fat_update_dentry,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    if(fat_inode->Dirty)
    {
        fileTime = DSM_GetFileTime();
        d_entry->DIR_WrtDate = DSM_GetFileTime2Date(fileTime);
        d_entry->DIR_WrtTime = DSM_GetFileTime2Time(fileTime);
        d_entry->DIR_LstAccDate = d_entry->DIR_WrtDate;

        bclu = FAT_INO2CLUS(inode->i_ino);
        bsec = FAT_INO2SEC(inode->i_ino);
        eoff = FAT_INO2OFF(inode->i_ino);

        if(bclu > 0)
        {
            lErrCode = fat_read_entries(sb, bclu, bsec, eoff, secbuf, &ecount);
            if(_ERR_FAT_SUCCESS != lErrCode)
            {
                D( ( DL_FATERROR, "in fat_update_dentry,fat_read_entries_0 ErrorCode = %d.\n", lErrCode) );
                goto end;
            }

            FDE2Buf(d_entry, entbuf);
            DSM_MemCpy((UINT8 *)(secbuf + (ecount - 1)*DEFAULT_DIRENTRYSIZE), entbuf, DEFAULT_DIRENTRYSIZE);

            lErrCode = fat_write_entries(sb, bclu, bsec, eoff, secbuf, ecount);
            if(_ERR_FAT_SUCCESS != lErrCode)
            {
                D( ( DL_FATERROR, "in fat_update_dentry,fat_write_entries_0 ErrorCode = %d.\n", lErrCode) );
                goto end;
            }
        }
        else
        {
            lErrCode = fat_read_root_entries(sb, bsec, eoff, secbuf, &ecount);
            if(_ERR_FAT_SUCCESS != lErrCode)
            {
                D( ( DL_FATERROR, "in fat_update_dentry,fat_read_entries_1 ErrorCode = %d.\n", lErrCode) );
                goto end;
            }

            FDE2Buf(d_entry, entbuf);
            DSM_MemCpy((UINT8 *)(secbuf + (ecount - 1)*DEFAULT_DIRENTRYSIZE), entbuf, DEFAULT_DIRENTRYSIZE);

            lErrCode = fat_write_root_entries(sb, bsec, eoff, secbuf, ecount);
            if(_ERR_FAT_SUCCESS != lErrCode)
            {
                D( ( DL_FATERROR, "in fat_update_dentry,fat_write_entries_1 ErrorCode = %d.\n", lErrCode) );
                goto end;
            }
        }
        fat_inode->Dirty = DATA_CLEAN;
    }

end:
    if(secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    }
    return lErrCode;
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
INT32 fat_file_write(struct file * filp, INT8 * buf, UINT32 lenToWrite)
{
    struct inode *pinode;
    FAT_DIR_ENTRY* d_entry = NULL;
    FAT_SB_INFO* sb = NULL;
    FAT_INODE_INFO* fat_inode = NULL;
    UINT32 fsize = 0;
    UINT8* bufp = NULL;
    UINT8 secperclu = 0;
    UINT32 clunum = 0, secnum = 0, secoff = 0;
    UINT8* secbuf;
    UINT32 lastCluster = 0;
    UINT32 sizeWriten = 0;
    UINT32 thisLenToWrite = 0;
    INT32 iRet = _ERR_FAT_SUCCESS;
    INT32 iRet2 = _ERR_FAT_SUCCESS;

    pinode = filp->f_inode;
    d_entry = &(pinode->u.fat_i.entry);
    sb = &(pinode->i_sb->u.fat_sb);
    fat_inode = &(pinode->u.fat_i);

    fsize = d_entry->DIR_FileSize;
    bufp = (UINT8*)buf;
    secperclu = sb->iSecPerClus;

    if ( filp->f_flags & FS_O_APPEND )
    {
        filp->f_pos = pinode->i_size;
    }
    else
    {
        // Do nothing.
    }


    if(filp->f_pos > (UINT64)fsize)
    {
        D((DL_FATERROR, "fat_file_write: _ERR_FAT_WRITE_EXCEED.\n"));
        return _ERR_FAT_WRITE_EXCEED;
    }

    // malloc the secbuf for read sector.

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        DSM_HstSendEvent(0xba000001);
        D(( DL_FATERROR, "in fat_file_write,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_file_write,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }


    while (sizeWriten < lenToWrite)
    {
        iRet = fat_fpos2CSO(pinode, (UINT32)filp->f_pos, &lastCluster, &clunum, &secnum, &secoff);
        if(_ERR_FAT_NOT_MATCH_CLUSTER == iRet)
        {
            secnum = 0;
            secoff = 0;
            iRet = fat_get_free_cluster(sb, lastCluster, &clunum);
            if(iRet < 0 || clunum < 2)
            {
                D( ( DL_FATERROR, "In  fat_file_write,fat_get_free_cluster() failed! ErrorCode = %d\n", iRet));
                DSM_HstSendEvent(0xba000002);
                DSM_HstSendEvent((0 - iRet));
                DSM_HstSendEvent(clunum);
                goto failed;
            }
            DSM_ASSERT((BOOL)(clunum >= 2), "clunum = %d.", clunum);
            // Write the fat cache to disk,if not to do, the laster cluster value zero is  make for in system broken.
            iRet = fat_set_fat_entry_value(sb, clunum, CLUSTER_NULL, FALSE, pinode);
            // iRet = fat_set_fat_entry_value(sb, clunum, CLUSTER_NULL, TRUE, inode);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D((DL_FATERROR, "fat_file_write: fat_set_fat_entry_value()1 return %d.\n", iRet));
                iRet = _ERR_FAT_WRITE_SEC_FAILED;
                DSM_HstSendEvent(0xba000003);
                goto failed;
            }

            // fat_SetPos(pinode,(UINT32)filp->f_pos,clunum);
            if (0 == lastCluster)
            {
                d_entry->DIR_FstClusLO = (UINT16)(clunum & 0x0000ffff);
                d_entry->DIR_FstClusHI = (UINT16)((clunum >> 16) & 0x0000ffff);
                fat_inode->Dirty = DATA_DIRTY;
            }
            else
            {
                DSM_ASSERT((BOOL)(clunum >= 2), "clunum = %d.", clunum);
                iRet = fat_set_fat_entry_value(sb, lastCluster, clunum, FALSE, pinode);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_set_fat_entry_value()2 return 0x%x.\n", iRet));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    DSM_HstSendEvent(0xba000004);
                    goto failed;
                }
            }
            iRet = fat_append_fatentries_chain(&(filp->f_inode->u.fat_i.CluChain), clunum);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D((DL_FATERROR, "fat_file_write: fat_append_fatentries_chain() return %d.\n", iRet));
                DSM_HstSendEvent(0xba000005);
                goto failed;
            }
        }
        lastCluster = clunum;
        DSM_ASSERT((BOOL)(clunum >= 2), "clunum = %d.", clunum);
        // write to destination cluster.
        for (; secnum < secperclu && sizeWriten < lenToWrite; ++secnum, secoff = 0)
        {
            // compute current writing size.
            thisLenToWrite = ((secoff + (lenToWrite - sizeWriten)) > DEFAULT_SECSIZE) \
                             ? (DEFAULT_SECSIZE - secoff) : (lenToWrite - sizeWriten);

            if (0 == secoff && ((lenToWrite - sizeWriten) >= DEFAULT_SECSIZE))
            {
                // if the writing size equit to sector size, write to the sector directly use a section of input buffer.

                iRet = fat_write_cluster(sb, clunum, secnum, bufp);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_write_cluster() return %d.\n", iRet));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    DSM_HstSendEvent(0xba000006);
                    DSM_HstSendEvent((0 - iRet));
                    goto failed;
                }
            }
            else
            {

                // Read destination sector content .
                iRet = fat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_read_cluster() return %d.\n", iRet));
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    DSM_HstSendEvent(0xba000007);
                    DSM_HstSendEvent((0 - iRet));
                    goto failed;
                }


                // Update the content of needing to write.
                DSM_MemCpy((UINT8 *)(secbuf + secoff), bufp, thisLenToWrite);
                iRet = fat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_write_cluster()2 return %d.\n", iRet));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    DSM_HstSendEvent(0xba000008);
                    DSM_HstSendEvent((0 - iRet));
                    goto failed;
                }
            }

            sizeWriten += thisLenToWrite;
            filp->f_pos += (UINT64)thisLenToWrite;
            bufp += thisLenToWrite;
        }
    }

failed:

    fat_inode->Dirty = DATA_DIRTY;
    if(filp->f_pos > (UINT64)fsize)
    {
        d_entry->DIR_FileSize = (UINT32)filp->f_pos;
        filp->f_inode->i_size = (UINT64)d_entry->DIR_FileSize;
    }

    // Write the entry of this inode into disk
    // if the file size is change.
    // It changes other entry member as well.

    //iRet2 = fat_update_dentry(inode, secbuf);
    //filp->f_inode->i_mtime = DSM_UniteFileTime(inode->u.fat_i.entry.DIR_WrtDate, inode->u.fat_i.entry.DIR_WrtTime,0);
    //filp->f_inode->i_atime = DSM_UniteFileTime(inode->u.fat_i.entry.DIR_WrtDate, 0,0);

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE((SECT_BUF *)secbuf);
    }

    if (iRet != _ERR_FAT_SUCCESS)
    {
        return iRet;
    }
    else if (iRet2 != _ERR_FAT_SUCCESS)
    {
        return iRet2;
    }
    else
    {
        return _ERR_FAT_SUCCESS;
    }

}


//
// Extend file.
//
INT32 fat_file_extend(struct file * filp, UINT64 SizeExtend)
{
    struct inode *pinode;
    FAT_DIR_ENTRY* d_entry = NULL;
    FAT_SB_INFO* sb = NULL;
    FAT_INODE_INFO* fat_inode = NULL;

    UINT8 secperclu = 0;

    UINT32 clunum = 0, secnum = 0, secoff = 0;
    UINT8* secbuf = NULL;

    UINT32 lastCluster = 0;
    UINT32 sizeExtended = 0;
    UINT32 thisLenToWrite;

    INT32 iRet = _ERR_FAT_SUCCESS;
    INT32 iRet2 = _ERR_FAT_SUCCESS;

    pinode = filp->f_inode;
    d_entry = &(pinode->u.fat_i.entry);
    sb = &(pinode->i_sb->u.fat_sb);
    fat_inode = &(pinode->u.fat_i);
    secperclu = sb->iSecPerClus;
    lastCluster = ((d_entry->DIR_FstClusHI << 16) | d_entry->DIR_FstClusLO);
    if(0 == SizeExtend)
    {
        return _ERR_FAT_SUCCESS;
    }

    // malloc the secbuf for read sector.

    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D(( DL_FATERROR, "in fat_file_extend,1.FAT_SECT_BUF_ALLOC() failed."));
        DSM_ASSERT(0, "in fat_file_extend,1.FAT_SECT_BUF_ALLOC() failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    iRet = fat_fpos2CSO(pinode, (UINT32)pinode->i_size, &lastCluster, &clunum, &secnum, &secoff);
    if (_ERR_FAT_SUCCESS == iRet)
    {
        DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);

        for (; secnum < secperclu && sizeExtended < SizeExtend; ++secnum, secoff = 0)
        {
            thisLenToWrite = (UINT32)(((secoff + (SizeExtend - sizeExtended)) > DEFAULT_SECSIZE) \
                                             ? (DEFAULT_SECSIZE - secoff) : (SizeExtend - sizeExtended));
            if (0 == secoff)
            {
                DSM_MemZero(secbuf, DEFAULT_SECSIZE);
                iRet = fat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_write_cluster() return %d.\n", iRet));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    goto failed;
                }
            }
            else
            {
                iRet = fat_read_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_read_cluster() return %d.\n", iRet));
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    goto failed;
                }

                DSM_MemZero((UINT8 *)(secbuf + secoff), thisLenToWrite);
                iRet = fat_write_cluster(sb, clunum, secnum, secbuf);
                if(_ERR_FAT_SUCCESS != iRet)
                {
                    D((DL_FATERROR, "fat_file_write: fat_write_cluster()2 return %d.\n", iRet));
                    iRet = _ERR_FAT_WRITE_SEC_FAILED;
                    goto failed;
                }
            }

            sizeExtended += thisLenToWrite;
        }

    }

    lastCluster = clunum;
    while (sizeExtended < SizeExtend)
    {
        iRet = fat_get_free_cluster(sb, lastCluster, &clunum);
        if(iRet < 0 || clunum < 2)
        {
            D( ( DL_FATERROR,"In  fat_file_write,fat_get_free_cluster() failed! ErrorCode = %d\n",iRet));
            goto failed;
        }
        DSM_ASSERT((BOOL)(clunum >= 2),"clunum = %d.",clunum);
        iRet = fat_set_fat_entry_value(sb, clunum, CLUSTER_NULL, FALSE, pinode);
        if(_ERR_FAT_SUCCESS != iRet)
        {
            D((DL_FATERROR, "fat_file_write: fat_set_fat_entry_value()1 return %d.\n", iRet));
            iRet = _ERR_FAT_WRITE_SEC_FAILED;
            goto failed;
        }
        // fat_SetPos(pinode,(UINT32)filp->f_pos,clunum);
        if (0 == lastCluster)
        {
            d_entry->DIR_FstClusLO = (UINT16)(clunum & 0x0000ffff);
            d_entry->DIR_FstClusHI = (UINT16)((clunum >> 16) & 0x0000ffff);
            fat_inode->Dirty = DATA_DIRTY;
        }
        else
        {
            DSM_ASSERT((BOOL)(clunum >= 2), "clunum = %d.", clunum);
            iRet = fat_set_fat_entry_value(sb, lastCluster, clunum, FALSE, pinode);
            if(_ERR_FAT_SUCCESS != iRet)
            {
                D((DL_FATERROR, "fat_file_write: fat_set_fat_entry_value()2 return %d.\n", iRet));
                iRet = _ERR_FAT_WRITE_SEC_FAILED;
                goto failed;
            }
        }

        CSW_TRACE(BASE_DSM_TS_ID, "fat_file_extend:curclu = 0x%x.\n", clunum);

        sizeExtended += (UINT32)(( SizeExtend - sizeExtended >= (UINT32)(DEFAULT_SECSIZE * secperclu)) ? \
                                    DEFAULT_SECSIZE * secperclu : SizeExtend - sizeExtended);
        lastCluster = clunum;
    }

failed:

    fat_inode->Dirty = DATA_DIRTY;
    d_entry->DIR_FileSize += sizeExtended;
    filp->f_inode->i_size = d_entry->DIR_FileSize;

    // Write the entry of this inode into disk
    // if the file size is change.
    // It changes other entry member as well.

    iRet2 = fat_update_dentry(pinode);
    filp->f_inode->i_mtime = DSM_UniteFileTime(pinode->u.fat_i.entry.DIR_WrtDate, pinode->u.fat_i.entry.DIR_WrtTime,0);
    filp->f_inode->i_atime = DSM_UniteFileTime(pinode->u.fat_i.entry.DIR_WrtDate, 0,0);
    if(secbuf)
    {
         FAT_SECT_BUF_FREE((SECT_BUF*)secbuf);
    }

    if (iRet != _ERR_FAT_SUCCESS)
    {
        return iRet;
    }
    else if (iRet2 != _ERR_FAT_SUCCESS)
    {
        return iRet2;
    }
    else
    {
        return _ERR_FAT_SUCCESS;
    }

}

//
// Truncat file.
//
INT32 fat_file_truncate(struct inode *psInode, UINT64 nFileSize)
{
    INT32 iRet = _ERR_FAT_SUCCESS;
    FAT_SB_INFO* sb_info = NULL ;
    UINT32 nClu = 0, nSec = 0, nOff = 0;
    UINT32 lastCluster = 0;
    FAT_DIR_ENTRY* fde = NULL;
    UINT32 fileTime;

    D((DL_FATDETAIL, "Enter in fat_file_truncate, nFileSize = %d\n", nFileSize));
    sb_info = &(psInode->i_sb->u.fat_sb);
    fde =  &(psInode->u.fat_i.entry);
    lastCluster = ((fde->DIR_FstClusHI << 16) | fde->DIR_FstClusLO);
    if (nFileSize > fde->DIR_FileSize)
    {
        D( ( DL_FATERROR, "in fat_file_truncate, truncate size greater than file size.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }
    if (nFileSize == fde->DIR_FileSize)
    {
        return _ERR_FAT_SUCCESS;
    }

    fileTime = DSM_GetFileTime();
    psInode->i_ctime = psInode->i_mtime = fileTime;
    fde->DIR_WrtDate = DSM_GetFileTime2Date(fileTime);
    fde->DIR_WrtTime = DSM_GetFileTime2Time(fileTime);
    psInode->i_atime = DSM_UniteFileTime(fde->DIR_WrtDate, 0, 0);
    fde->DIR_LstAccDate = fde->DIR_WrtDate;
    fde->DIR_FileSize = (UINT32)nFileSize;
    psInode->i_size = fde->DIR_FileSize;
    psInode->u.fat_i.Dirty = DATA_DIRTY;
    iRet = fat_update_dentry(psInode);
    if (iRet != _ERR_FAT_SUCCESS)
    {
        goto end;
    }

    iRet = fat_fpos2CSO(psInode, (UINT32)nFileSize, &lastCluster, &nClu, &nSec, &nOff);
    if (iRet != _ERR_FAT_SUCCESS)
    {
        g_TstFsErrCode = 1023;
        iRet = _ERR_FAT_ERROR;
        goto end;
    }
    DSM_ASSERT((BOOL)(nClu >= 2),"nClu= %d.",nClu);

    // free fat_chain
    iRet = fat_trunc_fragentries(sb_info, &(psInode->u.fat_i.CluChain), nClu);
    if(_ERR_FAT_SUCCESS != iRet)
    {
        D((DL_FATERROR, "in fat_trunc_file,fat_trunc_fragentries error, ErrorCode = %d\n",iRet));
    }
end:
    return iRet;
}




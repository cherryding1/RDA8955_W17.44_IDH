#ifndef _CII_EXFAT_LOCAL_H
#define _CII_EXFAT_LOCAL_H

INT32 exfat_read_inode ( struct inode *inode);

INT32 exfat_do_find_entry( struct inode * dir_i, struct dentry *de, UINT64 *ino);

INT32 exfat_write_inode( struct inode * inode);

INT32 exfat_create_file( struct inode *dir_i, struct dentry *de, UINT8 mode );

INT32 exfat_trunc_file( struct inode *dir_i, struct dentry *de, UINT8 mode );

INT32 exfat_do_unlink( struct inode *dir_i, struct dentry *de, BOOL bRename );

INT32 exfat_do_link( struct dentry *old_de, struct inode *dir_i, struct dentry *de, UINT32 type );

INT32 exfat_is_formated(struct super_block *sb);

INT32 exfat_format(UINT32 dev_no, UINT32 total_sec, UINT32 partitionoffset, UINT8* vollab,UINT8* fsname,UINT8 opt);

INT32 exfat_get_file_name(struct inode * pinode, WCHAR *fileName);

// INT32 exfat_get_dir_size(struct inode *inode,UINT64* pSize,BOOL IgnoreDir);
INT32 exfat_get_vollab(UINT32 uDevNo, UINT8* szVolLab, struct super_block *sb);
INT32 exfat_set_vollab(UINT32 uDevNo, PCSTR szVolLab, struct super_block *sb);
INT32 exfat_scan_disk(struct super_block* sb);
INT32 exfat_lookup_file(struct inode *inode,
                                           WCHAR* filefilter,
                                           UINT64 *cur_ino,
                                           UINT64 *nex_ino,
                                           WCHAR* filename,
                                           UINT16* ecount);

INT32 exfat_trunc_fatentry(struct inode* pinode, UINT32 lst_clus,UINT32 trunc_clus);

INT32 exfat_remove_entry(struct inode *pinode,
                UINT32 bclu, UINT16 bsec, UINT16 eoff, BOOL bRename);

INT32 exfat_get_null_entry_ino(struct inode *diri,UINT32 ecount,UINT64* ino,UINT32* nullcount);

VOID exfat_make_entries(EXFAT_SB_INFO* sb_info,WCHAR* pszFileName,
	     UINT16 attrib,UINT64 data_len, EXFAT_DIR_ENTRY* pdentries);

INT32 exfat_add_entry(struct inode *diri, EXFAT_DIR_ENTRY* pdentries,
            UINT32 fst_clus,UINT32* bclu, UINT16 *bsec, UINT16 *eoff);

INT32 exfat_read_entries(struct inode *pinode,  UINT32 bclu,
                UINT16 bsec, UINT16 eoff, EXFAT_DIR_ENTRY* dir_entry, UINT16* ecount);

INT32 exfat_write_entries(struct inode *pinode,
            UINT32 bclu, UINT16 bsec, UINT16 eoff, EXFAT_DIR_ENTRY* dentry, UINT16 ecount);


INT32 exfat_get_name_for_entrys(struct inode *pinode, WCHAR* filename, UINT32 bclu, UINT16 bsec, UINT16 eoff);

INT32 exfat_file_read(struct file * filp, INT8 * buf, INT32 len);

BOOL exfat_is_inode_relating_with_fatcache(CONST struct inode *psInode, EXFAT_CACHE **ppsFATCache);

INT32 exfat_relate_inode_with_fatcache(EXFAT_CACHE *psCache, struct inode *psInode);

INT32 exfat_synch_relating_file(EXFAT_SB_INFO *sb_info, EXFAT_CACHE *psCache);

INT32 exfat_synch_cache(EXFAT_SB_INFO *sb_info, EXFAT_CACHE *psCache);

INT32 exfat_set_bat_entry_value(EXFAT_SB_INFO *sb_info, UINT32 fatent, UINT32 value,
                                                                BOOL bSynchFlag, struct inode *psInode);

INT32 exfat_set_fat_entry_value(EXFAT_SB_INFO *sb_info, UINT32 fatent, UINT32 value,
                                BOOL bSynchFlag, struct inode *psInode);
INT32 exfat_set_bat_fat_entry(EXFAT_SB_INFO *sb_info, UINT32 pre_fatent, UINT32 fatent, UINT32 value,
                                                                BOOL bSynchFlag, struct inode *psInode);
INT32 exfat_update_dentry(struct inode *inode);

INT32 exfat_file_write(struct file * filp, INT8 * buf, UINT32 lenToWrite);

INT32 exfat_file_extend(struct file * filp, UINT64 SizeExtend);

INT32 exfat_file_truncate(struct inode *psInode, UINT64 nFileSize);

#endif //_CII_EXFAT_LOCAL_H


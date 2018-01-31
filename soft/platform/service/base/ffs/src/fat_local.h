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

#ifndef _CII_FAT_LOCAL_H
#define _CII_FAT_LOCAL_H

#define DATA_DIRTY  1
#define DATA_CLEAN  0

struct file;
struct inode;
struct super_block;
struct dentry;

INT32 fat_format(UINT32 uDevNo, UINT32 iDiskTotSec, UINT32 iPartitionOffset, UINT8 *vollab, UINT8 *oemname, UINT8 opt);
INT32 fat_is_formated(struct super_block *sb);
INT32 fat_file_read(struct file * filp, INT8 * buf, INT32 len);
INT32 fat_file_write(struct file * filp, INT8 * buf, UINT32 lenToWrite);
INT32 fat_file_extend(struct file * filp, UINT64 SizeExtend);
INT32 fat_read_inode ( struct inode *inode, BOOL is_get_fatentry);
INT32 fat_do_find_entry( struct inode * dir_i, struct dentry *de, UINT64 *ino);
INT32 fat_write_inode( struct inode * inode,BOOL bFreeFlag);
INT32 fat_create_file( struct inode *dir_i, struct dentry *de, UINT8 mode );
INT32 fat_trunc_file( struct inode *dir_i, struct dentry *de, int mode );
INT32 fat_do_unlink( struct inode *dir_i, struct dentry *de, BOOL bRename );
INT32 fat_do_link( struct dentry *old_de, struct inode *dir_i, struct dentry *de, UINT32 type );
INT32 fat_dump_fat_list( PCSTR pathname );
INT32 fat_dump_fat(UINT32 begin_clus_nr, UINT32 counter );
INT32 fat_scan_disk(struct super_block* sb);
INT32 fat_get_file_name(struct inode *inode, WCHAR *fileName);
INT32 fat_file_truncate(struct inode *psInode, UINT64 nFileSize);

// A file is denoted by a inode in VFS .
// MinxFS throug inode-list to denode a file on disk.
// FAT throug a group to denode a file,the group include cluster number,sector number in cluster and dentry offset.
// Used a UINT32 variable to describe the group of 3 number,as flowing:
// 63-->32 bits: cluster number
// 31  -->  16 bits: sector number
// 15  -->  0 bits: entry offset
//#define FAT_INO2CLUS(ino) ( (UINT32) ( ( ((ino)>>10) & 0x3fffff ) == 0x3fffff ? 0x0fffffff : ( ((ino)>>10) & 0x3fffff ) ) )
//#define FAT_INO2SEC(ino) ( (UINT16) ( (((ino)&0x3ff)>>4) & 0x3f ) )
//#define FAT_INO2OFF(ino) ( (UINT16) (( (ino)&0xf )<<5))

//#define FAT_CSO2INO(clus, bsec, eoff)  (UINT32) ( (((clus)&0x3fffff)<<10) | (((bsec)&0x3f)<<4) | (((eoff)>>5)&0xf) )

#define MAST_HEX32B 0xffffffff
#define MAST_HEX16B 0xffff
#define FAT32_FAT_EOF 0x0fffffff
#define FAT_INO2CLUS(ino) ( (UINT32) (( ((ino)>>32) & MAST_HEX32B) == MAST_HEX32B ? FAT32_FAT_EOF : ( ((ino)>>32) & MAST_HEX32B ) ))
#define FAT_INO2SEC(ino) ( (UINT16) ((((ino) & MAST_HEX32B)>>16) & MAST_HEX16B))
#define FAT_INO2OFF(ino) ( (UINT16) ((ino) & MAST_HEX16B ))
#define FAT_CSO2INO(clus, bsec, eoff)  (UINT64)((((((UINT64)clus)&MAST_HEX32B)<<32)) |(((((UINT16)bsec)&MAST_HEX16B)<<16)) |((((UINT16)eoff) & MAST_HEX16B)))

#endif //_CII_FAT_LOCAL_H


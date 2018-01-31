#ifndef _CII_EXFAT_BASE_H
#define _CII_EXFAT_BASE_H

//#include "dsm_cf.h"

// Support fat file system type.
#define EXFAT 64

#define EXFAT_JUMPBOOT                                    0xEB7690            // EXFAT JumpBoot.
#define EXFAT_FS_NAME                                      "EXFAT"              // EXFAT FileSystemName.
#define EXFAT_FSI_TRAILSIG                               0xAA55             // EXFAT BootSignature.
#define EXFAT_MBR_SECTOR_SART                       0                      // EXFAT Main Boot Region start sector.
#define EXFAT_EBR_SECTOR_COUNT                     8                      // EXFAT Extend Boot Region sector count.
#define EXFAT_MBR_SECTOR_COUNT                    12                     // EXFAT Main Boot Region sector count.
#define EXFAT_BBR_SECTOR_SART                       12                       // EXFAT Backup Boot Region start sector.
#define EXFAT_BBR_SECTOR_COUNT                     12                     // EXFAT Backup Boot Region sector count.
#define EXFAT_RESERVED_SECTOR_COUNT          24                     // EXFAT Reserved sector count.
#define EXFAT_FAT_ALIGNMENT_LENGTH_2048SEC            1024*1024                    // EXFAT FatAlignmentLength.
#define EXFAT_FAT_ALIGNMENT_LENGTH_128SEC             64*1024                    // EXFAT FatAlignmentLength.
#define EXFAT_FAT_COUNT                                   1                     // EXFAT Version 01.00
#define EXFAT_BITMAP_CHAR_BIT                    8                      //EXFAT BitMap Bit Count
#define EXFAT_ROOT_DIR_VL_OFFSET                 0                      // EXFAT Volume Label Entry offset.
#define EXFAT_ROOT_DIR_AB_OFFSET                32                     // EXFAT Allocation Bitmap Entry offset.
#define EXFAT_ROOT_DIR_UCT_OFFSET              64                     // EXFAT Up-case Table Entry offset.
#define EXFAT_DIR_ENTRY_SIZE     sizeof(EXFAT_DIR_ENTRY)      // EXFAT dir entry size.
#define EXFAT_DIR_ENTRY_COUNT                      19                    // EXFAT dir entry count.
#define EXFAT_UCT_LENGTH                                5836                 // EXFAT UCT(Up-case Table) Data Length.
#define EXFAT_DEFAULT_SECSIZE                       512                   // EXFAT default sector size.
#define EXFAT_NAME_LEN_MAX                           255                   // EXFAT name length max.

// Exfat-EntryType defined.
#define EXFAT_ETYPE_VLR                             0x03                          // Volume Label reseved
#define EXFAT_ETYPE_AB                               0x81	                   // Allocation Bitmap
#define EXFAT_ETYPE_UCT                             0x82	                   // Up-case Table
#define EXFAT_ETYPE_VL                               0x83	                   // Volume Label
#define EXFAT_ETYPE_FILE                            0x85	                   // File
#define EXFAT_ETYPE_VGUID                          0xA0	                   // Volume GUID
#define EXFAT_ETYPE_PAD                             0xA1	                   // TexFAT Padding
#define EXFAT_ETYPE_WINCE                         0xA2	                   // Windows CE Access Control Table
#define EXFAT_ETYPE_EXT                              0xC0	                   // Stream Extension
#define EXFAT_ETYPE_FNAME                         0xC1	                   // File Name
#define EXFAT_ETYPE_NULL                            0x0	                   // NULL Entry.

// Exfat-Bitmap Identifier define.
#define EXFAT_BITMAP_FLAGS_1ST                   0	                   // 0 - 1st bitmap.
#define EXFAT_BITMAP_FLAGS_2ND                   1	                   // 0 - 2nd bitmap.

#define EXFAT_CLUSTER_FREE                            0
#define EXFAT_CLUSTER_FINAL                          (~0)

// Exfat-Preconcerted Cluster.
#define EXFAT_CLUSTER_MEDIA                        0	                          // The  cluster descriptor.
#define EXFAT_CLUSTER_RESERVED                  1	                          // The  cluster reserved.
#define EXFAT_CLUSTER_DATA_START             2	                          // The  cluster data start number.
#define EXFAT_LAST_DATA_CLUSTER              0xFFFFFFF6

#define EXFAT_FST_CLUSTER_AB                      2	                          // The first cluster of AB(Allocation Bitmap).
#define EXFAT_FST_CLUSTER_UCT                    3	                          // The first cluster of UCT(Up-Case Table ).
#define EXFAT_FST_CLUSTER_RD                      4	                          // The first cluster of RD(Root Directory ).

// Valid values of FAT entries.
#define EXFAT_FAT_VALUE_MEDIA                   0xFFFFFFF8           // media descriptor.
#define EXFAT_FAT_VALUE_BAD                       0xFFFFFFF7           // bad cluster.
#define EXFAT_FAT_VALUE_EOF                       0xFFFFFFFF           // end of file (EOF mark).
#define EXFAT_FAT_VALUE_RESERVED             0xFFFFFFFF           // end of file (EOF mark).
#define EXFAT_FAT_VALUE_UNDEFINED           0x00000000           // does not mean the cluster is free, it is an undefined value.

// Exfat- common define.
#define EXFAT_FILE_SYSTEM_REVISION            0x0001                 // File System Version.
#define EXFAT_VOLUEM_FLAGS                          0x0                       // Volume Flags.
#define EXFAT_BYTESPERSECTORSHIFT             9                           // BytesPerSectorShift.
#define EXFAT_DRIVESELECT                             0x80                      // DriveSelect.

// EXFAT-FBR(Fat Boot Recode)
#define EXFAT_FBR_JMPBOOT_POS                                0
#define EXFAT_FBR_JMPBOOT_SIZE                              (sizeof(UINT8)*3)
#define EXFAT_FBR_FILESYSTEMNAME_POS                 (EXFAT_FBR_JMPBOOT_POS + EXFAT_FBR_JMPBOOT_SIZE)
#define EXFAT_FBR_FILESYSTEMNAME_SIZE                (sizeof(UINT8)*8)

#define EXFAT_FBR_MUSTBEZERO_POS                         (EXFAT_FBR_FILESYSTEMNAME_POS + EXFAT_FBR_FILESYSTEMNAME_SIZE)
#define EXFAT_FBR_MUSTBEZERO_SIZE                        (sizeof(UINT8)*53)

#define EXFAT_FBR_PARTITIONOFFSET_POS                (EXFAT_FBR_MUSTBEZERO_POS + EXFAT_FBR_MUSTBEZERO_SIZE)
#define EXFAT_FBR_PARTITIONOFFSET_SIZE               sizeof(UINT32)

#define EXFAT_FBR_VALUEMLENGTH_POS                     (EXFAT_FBR_PARTITIONOFFSET_POS + EXFAT_FBR_PARTITIONOFFSET_SIZE)
#define EXFAT_FBR_VALUEMLENGTH_SIZE                    sizeof(UINT32)

#define EXFAT_FBR_FATOFFSET_POS                            (EXFAT_FBR_VALUEMLENGTH_POS + EXFAT_FBR_VALUEMLENGTH_SIZE)
#define EXFAT_FBR_FATOFFSET_SIZE                           sizeof(UINT32)

#define EXFAT_FBR_FATLENGTH_POS                            (EXFAT_FBR_FATOFFSET_POS + EXFAT_FBR_FATOFFSET_SIZE)
#define EXFAT_FBR_FATLENGTH_SIZE                           sizeof(UINT32)

#define EXFAT_FBR_CLUSTERHEAPOFFSET_POS            (EXFAT_FBR_FATLENGTH_POS + EXFAT_FBR_FATLENGTH_SIZE)
#define EXFAT_FBR_CLUSTERHEAPOFFSET_SIZE           sizeof(UINT32)

#define EXFAT_FBR_CLUSTERCOUNT_POS                      (EXFAT_FBR_CLUSTERHEAPOFFSET_POS + EXFAT_FBR_CLUSTERHEAPOFFSET_SIZE)
#define EXFAT_FBR_CLUSTERCOUNT_SIZE                     sizeof(UINT32)

#define EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS      (EXFAT_FBR_CLUSTERCOUNT_POS + EXFAT_FBR_CLUSTERCOUNT_SIZE)
#define EXFAT_FBR_ROOTDIRECTORYCLUSTER_SIZE     sizeof(UINT32)

#define EXFAT_FBR_VOLUEMSERIALNUMBER_POS          (EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS + EXFAT_FBR_ROOTDIRECTORYCLUSTER_SIZE)
#define EXFAT_FBR_VOLUEMSERIALNUMBER_SIZE         sizeof(UINT32)

#define EXFAT_FBR_FILESYSTEMREVISION_POS            (EXFAT_FBR_VOLUEMSERIALNUMBER_POS + EXFAT_FBR_VOLUEMSERIALNUMBER_SIZE)
#define EXFAT_FBR_FILESYSTEMREVISION_SIZE           sizeof(UINT16)

#define EXFAT_FBR_VOLUEMFLAGS_POS                         (EXFAT_FBR_FILESYSTEMREVISION_POS + EXFAT_FBR_FILESYSTEMREVISION_SIZE)
#define EXFAT_FBR_VOLUEMFLAGS_SIZE                        sizeof(UINT16)

#define EXFAT_FBR_BYTESPERSECTORSHIFT_POS          (EXFAT_FBR_VOLUEMFLAGS_POS + EXFAT_FBR_VOLUEMFLAGS_SIZE)
#define EXFAT_FBR_BYTESPERSECTORSHIFT_SIZE         sizeof(UINT8)

#define EXFAT_FBR_SECTORSPERCLUSTER_POS              (EXFAT_FBR_BYTESPERSECTORSHIFT_POS + EXFAT_FBR_BYTESPERSECTORSHIFT_SIZE)
#define EXFAT_FBR_SECTORSPERCLUSTER_SIZE             sizeof(UINT8)

#define EXFAT_FBR_NUMBEROFFATS_POS                        (EXFAT_FBR_SECTORSPERCLUSTER_POS + EXFAT_FBR_SECTORSPERCLUSTER_SIZE)
#define EXFAT_FBR_NUMBEROFFATS_SIZE                       sizeof(UINT8)

#define EXFAT_FBR_DRIVESELECT_POS                            (EXFAT_FBR_NUMBEROFFATS_POS + EXFAT_FBR_NUMBEROFFATS_SIZE)
#define EXFAT_FBR_DRIVESELECT_SIZE                           sizeof(UINT8)

#define EXFAT_FBR_PERCENTINUSE_POS                         (EXFAT_FBR_DRIVESELECT_POS + EXFAT_FBR_DRIVESELECT_SIZE)
#define EXFAT_FBR_PERCENTINUSE_SIZE                        sizeof(UINT8)

#define EXFAT_FBR_RESERVED_POS                                 (EXFAT_FBR_PERCENTINUSE_POS + EXFAT_FBR_PERCENTINUSE_SIZE)
#define EXFAT_FBR_RESERVED_SIZE                                (sizeof(UINT8)*7)

#define EXFAT_FBR_BOOTCODE_POS                                 (EXFAT_FBR_RESERVED_POS + EXFAT_FBR_RESERVED_SIZE)
#define EXFAT_FBR_BOOTCODE_SIZE                                (sizeof(UINT8)*390)

#define EXFAT_FBR_BOOTSIGNATURE_POS                       (EXFAT_FBR_BOOTCODE_POS + EXFAT_FBR_BOOTCODE_SIZE)
#define EXFAT_FBR_BOOTSIGNATURE_SIZE                      (sizeof(UINT8)*7)


// Exfat Allocation Bitmap Directory entry position and size defined.
#define EXFAT_ABD_ENTRYTYPE_POS           0
#define EXFAT_ABD_ENTRYTYPE_SIZE          1
#define EXFAT_ABD_BITMAPFLAGS_POS       (EXFAT_ABD_ENTRYTYPE_POS + EXFAT_ABD_ENTRYTYPE_SIZE)
#define EXFAT_ABD_BITMAPFLAGS_SIZE      1
#define EXFAT_ABD_RESERVED_POS              (EXFAT_ABD_BITMAPFLAGS_POS + EXFAT_ABD_BITMAPFLAGS_SIZE)
#define EXFAT_ABD_RESERVED_SIZE            18
#define EXFAT_ABD_FIRSTCLUSTER_POS      (EXFAT_ABD_RESERVED_POS + EXFAT_ABD_RESERVED_SIZE)
#define EXFAT_ABD_FIRSTCLUSTER_SIZE     4
#define EXFAT_ABD_DATALENGTH_POS        (EXFAT_ABD_FIRSTCLUSTER_POS + EXFAT_ABD_FIRSTCLUSTER_SIZE)
#define EXFAT_ABD_DATALENGTH_SIZE       8

// Exfat Up-Case Table Directory entry position and size defined.
#define EXFAT_UCT_ENTRYTYPE_POS              0
#define EXFAT_UCT_ENTRYTYPE_SIZE             1
#define EXFAT_UCT_RESERVED1_POS              (EXFAT_ABD_ENTRYTYPE_POS + EXFAT_ABD_ENTRYTYPE_SIZE)
#define EXFAT_UCT_RESERVED1_SIZE             3
#define EXFAT_UCT_TABLECHECKSUM_POS      (EXFAT_UCT_RESERVED1_POS + EXFAT_UCT_RESERVED1_SIZE)
#define EXFAT_UCT_TABLECHECKSUM_SIZE     4
#define EXFAT_UCT_RESERVED2_POS               (EXFAT_UCT_TABLECHECKSUM_POS + EXFAT_UCT_TABLECHECKSUM_SIZE)
#define EXFAT_UCT_RESERVED2_SIZE              12
#define EXFAT_UCT_FIRSTCLUSTER_POS          (EXFAT_UCT_RESERVED2_POS + EXFAT_UCT_RESERVED2_SIZE)
#define EXFAT_UCT_FIRSTCLUSTER_SIZE         4
#define EXFAT_UCT_DATALENGTH_POS            (EXFAT_UCT_FIRSTCLUSTER_POS + EXFAT_UCT_FIRSTCLUSTER_SIZE)
#define EXFAT_UCT_DATALENGTH_SIZE           8

// Exfat Valumelable Directory entry position and size defined.
#define EXFAT_VL_ENTRYTYPE_POS              0
#define EXFAT_VL_ENTRYTYPE_SIZE             1
#define EXFAT_VL_CHARACTERCOUNT_POS   (EXFAT_VL_ENTRYTYPE_POS + EXFAT_VL_ENTRYTYPE_SIZE)
#define EXFAT_VL_CHARACTERCOUNT_SIZE  1
#define EXFAT_VL_VOLUMELABEL_POS          (EXFAT_VL_CHARACTERCOUNT_POS + EXFAT_VL_CHARACTERCOUNT_SIZE)
#define EXFAT_VL_VOLUMELABEL_SIZE         22
#define EXFAT_VL_RESERVED_POS                (EXFAT_VL_VOLUMELABEL_POS + EXFAT_VL_VOLUMELABEL_SIZE)
#define EXFAT_VL_RESERVED_SIZE               8

// Offset and size defined end.

// exfat dir entry parse state enum.
/*
typedef enum
{
     EXFAT_DENTRY_PARSE_INIT,
     EXFAT_DENTRY_PARSE_FILE,
     EXFAT_DENTRY_PARSE_EXT,
     EXFAT_DENTRY_PARSE_FNAME,
     EXFAT_DENTRY_PARSE_AB,
     EXFAT_DENTRY_PARSE_UCT,
     EXFAT_DENTRY_PARSE_VL,
     EXFAT_DENTRY_PARSE_VLR,
     EXFAT_DENTRY_PARSE_VGUID,
     EXFAT_DENTRY_PARSE_PAD,
     EXFAT_DENTRY_PARSE_WINCE,
     EXFAT_DENTRY_PARSE_ERROR,
     EXFAT_DENTRY_PARSE_END,
     EXFAT_DENTRY_PARSE_NULL,
}EXFAT_DENTRY_PARSE_STATE;
*/
typedef enum
{
     EXFAT_DENTRY_PARSE_INIT,
     EXFAT_DENTRY_PARSE_FILE,
     EXFAT_DENTRY_PARSE_EXT,
     EXFAT_DENTRY_PARSE_FNAME,
     EXFAT_DENTRY_PARSE_AB,
     EXFAT_DENTRY_PARSE_UCT,
     EXFAT_DENTRY_PARSE_VL,
     EXFAT_DENTRY_PARSE_VLR,
     EXFAT_DENTRY_PARSE_VGUID,
     EXFAT_DENTRY_PARSE_PAD,
     EXFAT_DENTRY_PARSE_WINCE,
     EXFAT_DENTRY_PARSE_FILE_END,
     EXFAT_DENTRY_PARSE_END,
     EXFAT_DENTRY_PARSE_FOUND,
     EXFAT_DENTRY_PARSE_NOT_FOUND,
     EXFAT_DENTRY_PARSE_ERROR,
}EXFAT_DENTRY_PARSE_STATE;


//EXFAT-FBR(ExFat Boot Recode)
typedef struct _EXFAT_BOOT_RECORD
{
      UINT8 jumpBoot[EXFAT_FBR_JMPBOOT_SIZE];
      UINT8 FileSystemName[EXFAT_FBR_FILESYSTEMNAME_SIZE];
      UINT8 MustBeZero[EXFAT_FBR_MUSTBEZERO_SIZE];
      UINT32 PartitionOffset;
      UINT32 ValuemLength;
      UINT32 FatOffset;
      UINT32 FatLength;
      UINT32 ClusterHeapOffset;
      UINT32 ClusterCount;
      UINT32 RootDirectoryCluster;
      UINT32 VoluemSerialNumber;
      UINT16 FileSystemRevision;
      UINT16 VoluemFlags;
      UINT8   BytesPerSectorShift;
      UINT8   SectorsPerCluster;
      UINT8   NumberOfFats;
      UINT8   DriveSelect;
      UINT8   PercentInUse;
      UINT8   Reserved[EXFAT_FBR_RESERVED_SIZE];
      UINT8   BootCode[EXFAT_FBR_BOOTCODE_SIZE];
      UINT16 BootSignature;
}EXFAT_BOOT_RECORD;

//EXFAT-EBS(ExFat Extend Boot Recode)
typedef struct _EXFAT_EXTEND_BOOT_RECORD
{
      UINT8   ExtBootCode[EXFAT_DEFAULT_SECSIZE - 2];
      UINT16 ExtBootSignature;
}EXFAT_EXTEND_BOOT_RECORD;

//EXFAT-OPR(ExFat Oem Parameter Record)
typedef struct _EXFAT_OEM_RECODE
{
      UINT8 ParameterType[16];
      UINT32 EraseBlockSize;
      UINT32 PageSize;
      UINT32 NumberOfSpareBlocks;
      UINT32 tRandomAccess;
      UINT32 tProgram;
      UINT32 tReadCycle;
      UINT32 tWriteCycle;
      UINT8 Reserved[4];

}EXFAT_OEM_RECORD;

//EXFAT-OP(ExFat Oem Parameter)
typedef struct _EXFAT_OEM_PARA
{
     EXFAT_OEM_RECORD Para[10];
     UINT8 Reserved[32];
}EXFAT_OEM_PARA;

// ExFAT Entry type.
// ...

// ExFAT Directory entry.
// 1.Generic Primary Directory Entry.
// EXFAT_PRI_DIR_ENTRY_TEMP.
// Each directory entry derives from Generica Directory Entry template.Size of directory entry is 32 bytes.
typedef struct _exfat_pri_dir_entry_temp
{
    // Entry Type.See the difine of Entry type.
    UINT8   Type;
    // Number of secondary entries which immediately fllow this peimary entry
    // and together comprise a directory entry set.Valid is 0..255.
    UINT8   SecondaryCount;
    // Checksum of all directory entries in the given set excluding this field.
    // See the function exfat_entrySetChecksum().
    UINT16 SetCheksum;
    // General Primary Flags.
    // Offset        Bitssize                         Comments
    // 0               1                                 0-not possible (FirstCluster and DataLength undefined), 1-possible
    // 1               1                                 0-FAT cluster chain is valid 1-FAT cluster chain is not used (contiguous data)
    // 2	          14             	                  CustomDefined
    UINT16 GeneralPriFlags;
    // Custom defined.
    UINT8   CustomDefined[14];
    // First Cluster number.
    UINT32 FirstCluster;
    // Data length.
    UINT64 DataLength;
}EXFAT_PRI_DIR_ENTRY_TEMP;

// 2.Generic Secondary Directory Entry Template.
// EXFAT_SECON_DIR_ENTRY_TEMP.
// Each directory entry derives from Generica Directory Entry template.Size of directory entry is 32 bytes.
typedef struct _exfat_second_dir_entry_temp
{
    // Entry Type.See the difine of Entry type.
    UINT8   Type;
    // General Secondary Flags
    // Bits 	Size 	Description
    // 0	       1	      Allocation Possible
    // 1	       1	      No Fat Chain
    // 2	       6	      CustomDefined
    UINT8   GeneralSecondaryFlags;
    // Custom Defined.
    UINT8 CustomDefined[18];
    // First Cluster number.
    UINT32 FirstCluster;
    // Data length.
    UINT64 DataLength;
}EXFAT_SECOND_DIR_ENTRY_TEMP;


// 3.Allocation Bitmap Directory Entry.
// EXFAT_AB_DIR_ENTRY
// 0 (0x00)	1	EntryType	0x81   Entry Type.See the EntryType	defined.
// 0x01	1	BitmapFlags
//              Bits	Size	Description	                                                              Comments
//              0	       1	BitmapIdentifier	0 ¨C 1st bitmap, 1 -2nd bitmap         Clusters 10-17
//              1	       7	Reserved
// 2 (0x02)	18	Reserved
// 20 (0x14)	4	First Cluster
// 24 (0x18)	8	Data Length
// The number of bitmaps and therefore a number of Bitmap Allocation entries is equal to the number of FATs.
// In case of TexFAT two FATs are used and bit 0 of Flags indicates which bitmap and FAT are referred.
// The First Allocation Bitmap shall be used in conjunction with the First FAT and the Second Allocation Bitmap
// shall be used with the Second FAT. ActiveFat field in Boot Sector defines which FAT and Allocation Bitmap are active.
// Bitmap size in bytes must be a number of clusters in the volume divided by 8 and rounded up.
// This is a Generic Secondary Directory Entry.
typedef struct _exfat_alloc_bmp_dir_entry
{
    UINT8   EntryType;
    UINT8   BitmapFlags;
    UINT8   Reserved[EXFAT_ABD_RESERVED_SIZE];
    UINT32 FirstCluster;
    UINT64 DataLength;
}EXFAT_AB_DIR_ENTRY;

// 4.Up-Case Table Directory Entry.
// EXFAT_UCT_DIR_ENTRY
// Offset	         Size	Description	                  Comments
// 0 (0x00)	  1	      EntryType	                         0x82
// 1 (0x01)	  3	      Reserved1
// 4 (0x04)	  4	      TableChecksum	          Up-case Table checksum
// 8 (0x08)	  12	      Reserved2
// 20 (0x14)	  4	      FirstCluster
// 24 (0x18)	  8	      DataLength
// The checksum is calculated against DataLength bytes of Up-case Table according to the function UpCaseTableChecksum().
typedef struct _exfat_uct_dir_entry
{
    UINT8     EntryType;
    UINT8     Reserved1[EXFAT_UCT_RESERVED1_SIZE];
    UINT32   TableChecksum;
    UINT8     Reserved2[EXFAT_UCT_RESERVED2_SIZE];
    UINT32   FirstCluster;
    UINT64 DataLength;
}EXFAT_UCT_DIR_ENTRY;

// 5.Volume Label Directory Entry.
// EXFAT_VL_DIR_ENTRY
// Offset	      Size	         Description	               Comments
// 0 (0x00)	1	         EntryType	               0x83
// 1 (0x01)	1	         CharacterCount	        Length in Unicode characters (max 11)
// 2 (0x02)	22	         VolumeLabel	               Unicode string
// 24 (0x18)	8               Reserved
// If volume is formatted without a label, the Volume Label Entry will be present but Entry Type will be set to 0x03 (not in use).
typedef struct _exfat_vl_dir_entry
{
    UINT8     EntryType;
    UINT8     CharacterCount;
    UINT8     VolumeLabel[EXFAT_VL_VOLUMELABEL_SIZE];
    UINT8     Reserved[EXFAT_VL_RESERVED_SIZE];
}EXFAT_VL_DIR_ENTRY;


// 6.File Directory Entry
// File directory entry describes files and directories. It is a primary critical directory entry and must be immediately followed by 1 Stream Extension directory entry and from 1 to 17 File Name directory entries . Those 3-19 directory entries comprise a directory entry set describing a single file or a directory.
// Offset 	Size	    Description  	                  Comments
// 0 (0x00)	1	    EntryType	                         0x85
// 1 (0x01)	1	    SecondaryCount                  Must be from 2 to 18
// 2 (0x02)	2	    SetChecksum
// 4 (0x04)	2	    FileAttributes
// FileAttributes--.
// Bits    	Size	Attribute
// 0	      1	    ReadOnly
// 1	      1	    Hidden
// 2	      1	    System
// 3	      1	    Reserved1
// 4	      1	    Directory
// 5	      1	    Archive
// 6	      10	  Reserved2
//FileAttributes--.
// 6        	2	    Reserved1
// 8        	4	    CreateTimestamp
// 12             4	    LastModifiedTimestamp
// 16             4	    LastAccessedTimestamp
// 20             1	    Create10msIncrement	        0..199
// 21             1	    LastModified10msIncrement	 0..199
// 22             1	    CreateTimezoneOffset	        Offset from UTC in 15 min increments
// 23             1	    LastModifiedTimezoneOffset	 Offset from UTC in 15 min increments
// 24             1	    LastAccessedTimezoneOffset	 Offset from UTC in 15 min increments
// 25             7	    Reserved2// Timestamp Format
// Bits	Size	Description	Comments
// 0-4	5	Seconds (as number of 2-second intervals)	0..29 29 represents 58 seconds
// 5-10	6	Minutes	0..59
// 11-15	5	Hour	0..23
// 16-20	5	Day	1..31
// 21-24	4	Month	1..12
// 25-31	7	Year (as offset from 1980)	0 represents 1980
// Timestamp format records seconds as 2 seconds intervals, so 10ms increments are used to increase precision from 2 seconds to 10 milliseconds.
// The valid values are from 0 to 199 in 10ms intervals which are added to correspondent timestamp. Timestamp is recorded in local time. Time zone offset is expressed in 15 minutes increments.
//
// Time Zone Offset Table
// TimezoneOffset field	TZ Offset	Time Zone	Comments
// 128 (0x80)	UTC	Greenwich Standard Time
// 132 (0x84)	UTC+01:00	Central Europe Time
// 136 (0x88)	UTC+02:00	Eastern Europe Standard Time
// 140 (0x8C)	UTC+03:00	Moscow Standard Time
// 144 (0x90)	UTC+04:00	Arabian Standard Time
// 148 (0x94)	UTC+05:00	West Asia Standard Time
// 152 (0x98)	UTC+06:00	Central Asia Standard Time
// 156 (0x9C)	UTC+07:00	North Asia Standard Time
// 160 (0xA0)	UTC+08:00	North Asia East Standard Time
// 164 (0xA4)	UTC+09:00	Tokyo Standard Time
// 168 (0xA8)	UTC+10:00	West Pacific Standard Time
// 172 (0xAC)	UTC+11:00	Central Pacific Standard Time
// 176 (0xB0)	UTC+12:00	New Zealand Standard Time
// 180 (0xB4)	UTC+13:00	Tonga Standard Time
// 208 (0xD0)	UTC-12:00	Dateline Standard Time
// 212 (0xD4)	UTC-11:00	Samoa Standard Time
// 216 (0xD8)	UTC-10:00	Hawaii Standard Time
// 220 (0xDC)	UTC-09:00	Alaska Standard Time
// 224 (0xE0)	UTC-08:00	Pacific Standard Time
// 228 (0xE4)	UTC-07:00	Mountain Standard Time
// 232 (0xE8)	UTC-06:00	Central Standard Time
// 236 (0xEC)	UTC-05:00	Eastern Standard Time
// 240 (0xF0)	UTC-04:00	Atlantic Standard time
// 242 (0xF2)	UTC-03:30	Newfoundland Standard Time
// 244 (0xF4)	UTC-03:00	Greenland Standard Time
// 248 (0xF8)	UTC-02:00	Mid-Atlantic Standard Time
// 252 (0xFC)	UTC-01:00	Azores Standard Time
typedef struct _exfat_file_dir_entry
{
    UINT8     EntryType;                // EntryType
    UINT8     SeconCount;             // SecondaryCount
    UINT16   SetChecksum;           // SetChecksum
    UINT16   FileAttributes;          //  FileAttributes
    UINT16   Reserved1;               // Reserved1
    UINT32   CreatTime;               // CreateTimestamp
    UINT32   LMTime;                    // LastModifiedTimestamp
    UINT32   LATime;                    // LastAccessedTimestamp
    UINT8     C10msInc;               // Create10msIncrement
    UINT8     LM10msInc;             // LastModified10msIncrement
    UINT8     CTimezOffs;             // CreateTimezoneOffset
    UINT8     LMTimezOffs;           // LastModifiedTimezoneOffset
    UINT8     LATimezOffs;           // LastAccessedTimezoneOffset
    UINT8     Reserved2[7];         // Reserved2
}EXFAT_FILE_DIR_ENTRY;

// 6. Volume GUID Directory Entry
// Offset	      Size	    Description          	 Comments
// 0 (0x00)   1          EntryType	               0xA0
// 1 (0x01)   1          SecondaryCount         Must be 0x00
// 2 (0x02)   2          SetChecksum
// 4 (0x04)   2          GeneralPrimaryFlags
// GeneralPrimaryFlags--
// Bits	Size	     Description       	Comments
// 0	       1          AllocationPossible     Must be 0
// 1	       1          NoFatChain            	Must be 0
// 2	       14	     CustomDefined
// GeneralPrimaryFlags--
// 6 (0x06)   	16 	   VolumeGuid	               All values are valid except null GUID {00000000-0000-0000-0000-000000000000}(hex)
// 2 (0x16)   	10	   Reserved
// This is a benign primary directory entry and may not present in a file system.
typedef struct _exfat_vguid_dir_entry
{
    UINT8     EntryType;                // EntryType
    UINT8     SeconCount;             // SecondaryCount
    UINT16   SetChecksum;           // SetChecksum
    UINT16   GenPriFlags;             // GeneralPrimaryFlags
    UINT8     VolumeGuid[16];       // VolumeGuid
    UINT8     Reserved[10];           // Reserved
}EXFAT_VGUID_DIR_ENTRY;

// 7.TexFAT Padding Directory Entry
// Offset   	    Size    Description	     Comments
// 0 (0x00)        1  	EntryType            0xA1
// 1 (0x01)	    31	Reserved
// exFAT 1.00 does not define TexFAT Padding directory entry. TexFAT Padding directory entries are only valid in the first cluster of directory and occupy every directory entry of the cluster. The implementations should not move TexFAT Padding directory entries.
typedef struct _exfat_pad_dir_entry
{
    UINT8     EntryType;                // EntryType
    UINT8     Reserved[31];           // Reserved
}EXFAT_PAD_DIR_ENTRY;

//8.Windows CE Access Control Table Directory Entry
// Offset     Size    Description     Comments
// 0 (0x00)   1       EntryType       0xA2
// 1 (0x01)   31      Reserved
// exFAT 1.00 does not define Windows CE Access Control Table Directory Entry.
typedef struct _exfat_wince_dir_entry
{
    UINT8     EntryType;                // EntryType
    UINT8     Reserved[31];           // Reserved
}EXFAT_WINCE_DIR_ENTRY;

// 9.Stream Extension Directory Entry
// Offset        Size    Description          Comments
// 0 (0x00)      1       EntryType            0xC0
// 1 (0x01)      1       GeneralSecondaryFlags
// GeneralSecondaryFlags--
// Bits	Size	Description	Comments
// 0	1	AllocationPossible	Must be 1
// 1	1	NoFatChain
// 2	14	CustomDefined
// GeneralSecondaryFlags--
// 2 (0x02)      1       Reserved1
// 3 (0x03)      1       NameLength           Length of Unicode name contained in subsequent File Name directory entries
// 4 (0x04)      2       NameHash             Hash of up-cased file name
// 6 (0x06)      2       Reserved2
// 8 (0x08)      8       ValidDataLength      Must be between 0 and DataLength
// 16 (0x10)     4       Reserved3
// 20 (0x14)     4       FirstCluster
// 24 (0x18)	8	DataLength	For directories maximum 256 MB
// Stream Extension directory entry must immediately follow the File directory entry in the set. It could be only one Stream Extension entry in the set. If NoFatChain flag is set, all allocated clusters are contiguous.
// The NameHash field facilitates the purpose of fast file name comparison and is performed on up-cased file name. NameHash verify against a mismatch, however matching hashes cannot guarantee the equality offile names. If name hashes match, a subsequent full name comparison must be performed.
// ValidDataLength determines how much actual data written to the file. Implementation shall update this field as data has been written. The data beyond the valid data length is undefined and implementation shall return zeros.
typedef struct _exfat_ext_dir_entry
{
    UINT8     EntryType;                                 // EntryType
    UINT8     GenSeconFlags;                         // GeneralSecondaryFlags
    UINT8     Reserved1;                                // Reserved1
    UINT8     NameLength;                             // NameLength
    UINT16   NameHash;                                // NameHash
    UINT16   Reserved2;                                // Reserved2
    UINT64   ValidDataLength;                      // ValidDataLength
    UINT32   Reserved3;                               // Reserved3
    UINT32   FirstCluster;                             // FirstCluster
    UINT64   DataLength;                             // DataLength
}EXFAT_EXT_DIR_ENTRY;

// File Name Directory Entry
// Offset	       Size	    Description              Comments
// 0 (0x00)      1        EntryType                0xC1
// 1 (0x01)      1        GeneralSecondaryFlags
// GeneralSecondaryFlags--
// Bits	Size	Attribute           Comments
// 0	  1	    AllocationPossible  Must be 0
// 1    1     NoFatChain          Must be 0
// 2    14    CustomDefined
// GeneralSecondaryFlags--
// 2 (0x02)	     30	      FileName
// File Name directory entries must immediately follow the Steam Extension directory entry in the number of NameLength/15 rounded up.
// The maximum number of File Name entries is 17, each can hold up to 15 Unicode characters and the maximum file name length is 255. Unused portion of FileName field must be set to 0x0000.
typedef struct _exfat_name_dir_entry
{
    UINT8     EntryType;                                 // EntryType
    UINT8     GenSeconFlags;                         // GeneralSecondaryFlags
    UINT8     FileName[30];                            // FileName
}EXFAT_NAME_DIR_ENTRY;

/*
union
{
    EXFAT_AB_DIR_ENTRY        ABEntry;
    EXFAT_UCT_DIR_ENTRY      UCTEntry;
    EXFAT_VL_DIR_ENTRY        VLEntry;
    EXFAT_FILE_DIR_ENTRY      FileEntry;
    EXFAT_VGUID_DIR_ENTRY  VGuidEntry;
    EXFAT_PAD_DIR_ENTRY      PADEntry;
    EXFAT_WINCE_DIR_ENTRY  WinceEntry;
    EXFAT_EXT_DIR_ENTRY       ExtEntry;
    EXFAT_NAME_DIR_ENTRY     NameEntry;
}EXFAT_DIR_ENTRY;
*/
typedef struct _exfat_dir_entry
{
    UINT8 EntryType;
    UINT8 EntryData[31];
}EXFAT_DIR_ENTRY;



// Macro of function.

// Macro of function end.

// Fat cache.
//typedef struct _FILE_INODE_LIST* PFILE_INODE_LIST;
typedef struct _exfat_cache{
    UINT32 iSecNum;
    UINT8   szSecBuf[EXFAT_DEFAULT_SECSIZE];
    PFILE_INODE_LIST psFileInodeList;
    UINT32 iDirty;
}EXFAT_CACHE;


// EXFAT Super Block information structure.
typedef struct exfat_sb_info
{
    UINT32 PartitionOffset;
    UINT32 ValuemLength;
    UINT32 FatOffset;
    UINT32 FatLength;
    UINT32 ClusterHeapOffset;
    UINT32 ClusterCount;
    UINT32 RootDirectoryCluster;
    UINT32 VoluemSerialNumber;
    UINT16 FileSystemRevision;
    UINT16 VoluemFlags;
    UINT16 BytesPerSector;
    UINT16 SectorsPerCluster;
    UINT32 BATCluster;
    UINT32 UCTCluster;
    UINT8   NumberOfFats;
    UINT8   DriveSelect;
    UINT8   PercentInUse;
    UINT32 CurCluster;
    EXFAT_CACHE BATCache;
    EXFAT_CACHE FATCache;
    struct super_block *sb;
}EXFAT_SB_INFO;

// exfat node.
typedef struct exfat_inode_info
{
    UINT32 CurClus;                // Current cluster for reading and writing.
    UINT32 ClusIndx;              // Custer index in FAT chain.
    UINT32 Dirty;
    EXFAT_FILE_DIR_ENTRY file_entry;
    EXFAT_EXT_DIR_ENTRY ext_entry;
}EXFAT_INODE_INFO;

#define DIV_ROUND_UP(x, d) (((x) + (d) - 1) / (d))
#define ROUND_UP(x, d) (DIV_ROUND_UP(x, d) * (d))

#define MAKEFATUINT64(a0, a1,a2, a3,a4,a5,a6,a7) MAKEUINT64(MAKEUINT32(MAKEUINT16((a0), (a1)), MAKEUINT16((a2), (a3))),\
                                                                                                          MAKEUINT32(MAKEUINT16((a4), (a5)), MAKEUINT16((a6), (a7))))

#define MAKEFATINT32(a, b, c, d) MAKEINT32(MAKEINT16((a), (b)), MAKEINT16((c), (d)))
#define MAKEFATINT16(a, b) MAKEINT16((a), (b))
#define MAKEFATUINT32(a, b, c, d) MAKEUINT32(MAKEUINT16((a), (b)), MAKEUINT16((c), (d)))
#define MAKEFATUINT16(a, b) MAKEUINT16((a), (b))

#define EXFAT_INO2CLUS(ino) ( (UINT32) (  (((UINT64)ino)>>32) & (UINT64)0xffffffff ) )
#define EXFAT_INO2SEC(ino) ( (UINT16) ( ((((UINT64)ino)&0xffff0000)>>16) & (UINT64)0xffff ) )
#define EXFAT_INO2EOFF(ino) ( (UINT16) (( ((UINT64)ino)&0xffff)))

#define EXFAT_CSO2INO(clus, bsec, eoff)  (UINT64) ( (((UINT64)clus<<32)&(UINT64)0xffffffff00000000) | (((UINT64)bsec<<16)&(UINT64)0x00000000ffff0000) | (((UINT64)eoff)&(UINT64)0xffff) )

#define EXFAT_IS_VALID_CLUSTER(sb,clus) (clus > 1 && clus < sb->ClusterCount)

#define EXFAT_GET_FAT_SEC_NUM(sb,offs) ((UINT32)sb->PartitionOffset + sb->FatOffset + (offs / sb->BytesPerSector))

#define EXFAT_GET_FAT_ENT_OFFS(sb,offs) (offs % sb->BytesPerSector)

#define  EXFAT_CHAR_UPPERW(ch)  (g_UctTable[ch])

#define EXFAT_IS_DIR(attr)  (attr & 0x10)

#define EXFAT_GENSECON_FLAGS_POSSIBLE(flags) (flags & 0x1)
#define EXFAT_GENSECON_FLAGS_NOFATCHAIN(flags) (flags & 0x2)

#define EXFAT_IS_FREE_DENTRY(type)  (0 == type)
#define EXFAT_IS_DELETE_DENTRY(type)  (0x05 == type || 0x40 == type || 0x41 == type)

#define EXFAT_BAT_OFFS(clus_num)  ((clus_num - 2)/8)
#define EXFAT_BAT_SECTOR(sb,clus_num) (EXFAT_BAT_OFFS(clus_num) /sb->BytesPerSector  + \
                    (UINT32)sb->PartitionOffset + sb->ClusterHeapOffset + sb->SectorsPerCluster*(sb->BATCluster - 2))
#define EXFAT_BAT_BYTEOFFS(sb,clus_num) (EXFAT_BAT_OFFS(clus_num) % sb->BytesPerSector)
#define EXFAT_BAT_BITOFFS(sb,clus_num) ((clus_num-2)%8)
#define EXFAT_IS_LAST_FAT_ENTRY(fst_clus,cur_clus,clus_sz,vilid_length) \
                         ((cur_clus - fst_clus + 1) == DIV_ROUND_UP(vilid_length,clus_sz))

// EXFAT FBR to buffer.
VOID EXFBR2Buf(EXFAT_BOOT_RECORD *fbr, UINT8 *buf);

// EXFat Volume Label Directory Entry to buffer.
VOID EXVLDE2Buf(EXFAT_VL_DIR_ENTRY *de,UINT8 *buf);

// EXFat Allocation Bitmap Directory Entry to buffer.
VOID EXABDE2Buf(EXFAT_AB_DIR_ENTRY *de,UINT8 *buf);

// EXFat Up-Case Table Directory Entry to buffer.
VOID EXUCTDE2Buf(EXFAT_UCT_DIR_ENTRY*de,UINT8 *buf);

// Buffer to EXFAT FBR.
VOID Buf2EXFBR(UINT8 *buf, EXFAT_BOOT_RECORD *fbr);

// Buffer to EXFat Volume Label Directory Entry.
VOID Buf2EXVLDE(UINT8 *buf,EXFAT_VL_DIR_ENTRY *de);

// Buffer to EXFat Allocation Bitmap Directory Entry.
VOID Buf2EXABDE(UINT8 *buf,EXFAT_AB_DIR_ENTRY *de);

// Buffer to EXFat Up-Case Table Directory Entry.
VOID Buf2EXUCTDE(UINT8 *buf,EXFAT_UCT_DIR_ENTRY*de);
void exfat_GetUpCaseTable(UINT8* pBuff);
UINT32 exfat_BootChecksum(const UINT8* data, UINT32 bytes);
UINT32 exfat_UCTChecksum(const UINT8* data, UINT32 data_len);
UINT32 exfat_VGuidChecksum(const UINT8* data, UINT32 data_len);
UINT8 exfat_GetSecNumPerClus(UINT32 iDiskTotSec);
VOID exfat_GetBR(
            UINT8* fsname,
            UINT32 volserial,
            UINT32 iTotSec,
            UINT32 iPartitionOffset,
            UINT32 iFatOffset,
            UINT32 iFATSz,
			UINT32 iClusterHeapOffset,
            UINT32 iCountOfClus,
            UINT32 iRootDirCluster,
            EXFAT_BOOT_RECORD *pBR
            );

VOID exfat_GetEBR(EXFAT_EXTEND_BOOT_RECORD *pEBR);

VOID exfat_GetOEM(EXFAT_OEM_RECORD* pOR);

VOID exfat_SetBootChecksum(UINT32 iChecksum,UINT8* pBuff);

UINT32 exfat_GetFATSz(UINT32 iTotalSec,
	               UINT32 iFatOffset,
                   UINT32 iSecPerClus);

UINT32 exfat_GetBitMapSz(UINT32 iCountOfClus);

UINT32 exfat_GetUCTSz();

UINT32 exfat_GetRootDirSz(UINT32 iSecPerClus);

VOID exfat_SetRootDir(UINT8* pValueName, UINT32 iBitmapLength,
					  UINT32 iBitmapClusCnt,UINT32 iUCTLenth,UINT32 iTableChecksum,UINT8* pBuff);

VOID exfat_SetBitMap(UINT8* pBuff,UINT32 iBitmapClusCnt);

VOID exfat_SetFat(UINT8* pBuff);

VOID exfat_BFR2SB(EXFAT_BOOT_RECORD* boot_rec,EXFAT_SB_INFO* sb_info);

INT32 exfat_read_root_dir(UINT32 DevNo,UINT32 SecNum,UINT32* BATClus,UINT32* UCTClus,UINT32* UCTTable);

INT32 exfat_read_sb_info(UINT32 DevNo,UINT32 SecNum,EXFAT_SB_INFO* sb_info);

INT32 exfat_read_cluster(EXFAT_SB_INFO *sb_info, UINT32 clunum,
    UINT32 offset, UINT8 *secbuf);

INT32 exfat_write_cluster(EXFAT_SB_INFO *sb_info, UINT32 clunum,
    UINT32 offset, UINT8 *secbuf);

VOID exfat_build_file_name(EXFAT_NAME_DIR_ENTRY* pdentry,UINT32 count,WCHAR* file_name);

UINT16 exfat_file_name_hash(PCWSTR filename);

INT32 exfat_get_next_cluster(struct inode *inode, UINT32 cur_clus, UINT32 *next_clus);

INT32 exfat_get_free_cluster(EXFAT_SB_INFO *sb_info, UINT32 iCurClu, UINT32 *pFreeClu);

INT32 exfat_stat_free_cluster(EXFAT_SB_INFO *sb_info, UINT32 *piFreeCluNum);

UINT32 exfat_fpos2cluster(struct inode *pinode,UINT64 iPos);

INT32 exfat_fpos2CSO(struct inode *inode, UINT64 iPos, UINT32 *piClu, UINT32 *piSec, UINT32 *piOff);

VOID exfat_SetPos(struct inode *inode, UINT32 iPos, UINT32 iClu);
UINT16 exfat_GetEntryCheckSum(UINT8* entry);
EXFAT_CACHE* exfat_get_fat_cache(UINT32 iDevNo);

INT32 exfat_set_fat_entry_value(EXFAT_SB_INFO *sb_info, UINT32 fatent, UINT32 value,
                                BOOL bSynchFlag, struct inode *psInode);

INT32 exfat_set_cluser_zero(EXFAT_SB_INFO *sb_info, UINT32 clunum);
// FAT API Begin
//UINT32 exfat_BootChecksum(const UINT8* data, int bytes);

// EXFAT API End.

#endif //_CII_EXFAT_BASE_H




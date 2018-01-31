#ifdef FS_SUPPORT_EXFAT
#include "dsm_cf.h"
#include "fs.h"
#include "dsm_dbg.h"
#include "fat_local.h"
#include "fat_base.h"
#include "exfat_base.h"
#include "fs_base.h"
#include "exfat_file.h"

extern UINT8 g_UctTable[];
extern UINT16 g_UctTableLen;
#if 0
UINT8 g_CaseUp[] =
{
    0x00,0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08,0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10,0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18,0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20,0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28,0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30,0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38,0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40,0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48,0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50,0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58,0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60,0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48,0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50,0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58,0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
}
#endif

VOID EXFBR2Buf(EXFAT_BOOT_RECORD *fbr, UINT8 *buf)
{
    DSM_MemCpy(&buf[EXFAT_FBR_JMPBOOT_POS], &(fbr->jumpBoot), EXFAT_FBR_JMPBOOT_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_FILESYSTEMNAME_POS], &(fbr->FileSystemName), EXFAT_FBR_FILESYSTEMNAME_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_MUSTBEZERO_POS], &(fbr->MustBeZero), EXFAT_FBR_MUSTBEZERO_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_PARTITIONOFFSET_POS], &(fbr->PartitionOffset), EXFAT_FBR_PARTITIONOFFSET_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_VALUEMLENGTH_POS], &(fbr->ValuemLength), EXFAT_FBR_VALUEMLENGTH_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_FATOFFSET_POS], &(fbr->FatOffset), EXFAT_FBR_FATOFFSET_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_FATLENGTH_POS], &(fbr->FatLength), EXFAT_FBR_FATLENGTH_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_CLUSTERHEAPOFFSET_POS], &(fbr->ClusterHeapOffset), EXFAT_FBR_CLUSTERHEAPOFFSET_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_CLUSTERCOUNT_POS], &(fbr->ClusterCount), EXFAT_FBR_CLUSTERCOUNT_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS], &(fbr->RootDirectoryCluster), EXFAT_FBR_ROOTDIRECTORYCLUSTER_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_VOLUEMSERIALNUMBER_POS], &(fbr->VoluemSerialNumber), EXFAT_FBR_VOLUEMSERIALNUMBER_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_FILESYSTEMREVISION_POS], &(fbr->FileSystemRevision), EXFAT_FBR_FILESYSTEMREVISION_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_VOLUEMFLAGS_POS], &(fbr->VoluemFlags), EXFAT_FBR_VOLUEMFLAGS_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_BYTESPERSECTORSHIFT_POS], &(fbr->BytesPerSectorShift), EXFAT_FBR_BYTESPERSECTORSHIFT_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_SECTORSPERCLUSTER_POS], &(fbr->SectorsPerCluster), EXFAT_FBR_SECTORSPERCLUSTER_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_NUMBEROFFATS_POS], &(fbr->NumberOfFats), EXFAT_FBR_NUMBEROFFATS_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_DRIVESELECT_POS], &(fbr->DriveSelect), EXFAT_FBR_DRIVESELECT_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_PERCENTINUSE_POS], &(fbr->PercentInUse), EXFAT_FBR_PERCENTINUSE_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_RESERVED_POS], &(fbr->Reserved), EXFAT_FBR_RESERVED_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_BOOTCODE_POS], &(fbr->BootCode), EXFAT_FBR_BOOTCODE_SIZE);
    DSM_MemCpy(&buf[EXFAT_FBR_BOOTSIGNATURE_POS], &(fbr->BootSignature), EXFAT_FBR_BOOTSIGNATURE_SIZE);
}

// Volume Label Directory Entry to buffer.
VOID EXVLDE2Buf(EXFAT_VL_DIR_ENTRY *de,UINT8 *buf)
{
    DSM_MemCpy(&buf[EXFAT_VL_ENTRYTYPE_POS], &(de->EntryType), EXFAT_VL_ENTRYTYPE_SIZE);
    DSM_MemCpy(&buf[EXFAT_VL_CHARACTERCOUNT_POS], &(de->CharacterCount), EXFAT_VL_CHARACTERCOUNT_SIZE);
    DSM_MemCpy(&buf[EXFAT_VL_VOLUMELABEL_POS], &(de->VolumeLabel), EXFAT_VL_VOLUMELABEL_SIZE);
    DSM_MemCpy(&buf[EXFAT_VL_RESERVED_POS], &(de->Reserved), EXFAT_VL_RESERVED_SIZE);
}


// Allocation Bitmap Directory Entry to buffer.
VOID EXABDE2Buf(EXFAT_AB_DIR_ENTRY *de,UINT8 *buf)
{
    DSM_MemCpy(&buf[EXFAT_ABD_ENTRYTYPE_POS], &(de->EntryType), EXFAT_ABD_ENTRYTYPE_SIZE);
    DSM_MemCpy(&buf[EXFAT_ABD_BITMAPFLAGS_POS], &(de->BitmapFlags), EXFAT_ABD_BITMAPFLAGS_SIZE);
    DSM_MemCpy(&buf[EXFAT_ABD_RESERVED_POS], &(de->Reserved), EXFAT_ABD_RESERVED_SIZE);
    DSM_MemCpy(&buf[EXFAT_ABD_FIRSTCLUSTER_POS], &(de->FirstCluster), EXFAT_ABD_FIRSTCLUSTER_SIZE);
    DSM_MemCpy(&buf[EXFAT_ABD_DATALENGTH_POS], &(de->DataLength), EXFAT_ABD_DATALENGTH_SIZE);
}


// Up-Case Table Directory Entry to buffer.
VOID EXUCTDE2Buf(EXFAT_UCT_DIR_ENTRY*de,UINT8 *buf)
{
    DSM_MemCpy(&buf[EXFAT_UCT_ENTRYTYPE_POS], &(de->EntryType), EXFAT_UCT_ENTRYTYPE_SIZE);
    DSM_MemCpy(&buf[EXFAT_UCT_RESERVED1_POS], &(de->Reserved1), EXFAT_UCT_RESERVED1_SIZE);
    DSM_MemCpy(&buf[EXFAT_UCT_TABLECHECKSUM_POS], &(de->TableChecksum), EXFAT_UCT_TABLECHECKSUM_SIZE);
    DSM_MemCpy(&buf[EXFAT_UCT_RESERVED2_POS], &(de->Reserved2), EXFAT_UCT_RESERVED2_SIZE);
    DSM_MemCpy(&buf[EXFAT_UCT_FIRSTCLUSTER_POS], &(de->FirstCluster), EXFAT_UCT_FIRSTCLUSTER_SIZE);
    DSM_MemCpy(&buf[EXFAT_UCT_DATALENGTH_POS], &(de->DataLength), EXFAT_UCT_DATALENGTH_SIZE);
}


VOID Buf2EXFBR(UINT8 *buf, EXFAT_BOOT_RECORD *fbr)
{
    DSM_MemSet(fbr,0,SIZEOF(EXFAT_BOOT_RECORD));

    DSM_MemCpy(fbr->jumpBoot, &buf[EXFAT_FBR_JMPBOOT_POS], EXFAT_FBR_JMPBOOT_SIZE);
    DSM_MemCpy(fbr->FileSystemName, &buf[EXFAT_FBR_FILESYSTEMNAME_POS], EXFAT_FBR_FILESYSTEMNAME_SIZE);
    DSM_MemCpy(fbr->MustBeZero, &buf[EXFAT_FBR_MUSTBEZERO_POS], EXFAT_FBR_MUSTBEZERO_SIZE);
    fbr->PartitionOffset = MAKEFATUINT64((buf)[EXFAT_FBR_PARTITIONOFFSET_POS],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+1],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+2],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+3],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+4],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+5],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+6],
                                                                 (buf)[EXFAT_FBR_PARTITIONOFFSET_POS+7]
                                                                 );
    fbr->ValuemLength = MAKEFATUINT64((buf)[EXFAT_FBR_VALUEMLENGTH_POS],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+1],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+2],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+3],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+4],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+5],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+6],
                                                                 (buf)[EXFAT_FBR_VALUEMLENGTH_POS+7]
                                                                 );
    fbr->FatOffset =        MAKEFATUINT32((buf)[EXFAT_FBR_FATOFFSET_POS],
                                                                 (buf)[EXFAT_FBR_FATOFFSET_POS+1],
                                                                 (buf)[EXFAT_FBR_FATOFFSET_POS+2],
                                                                 (buf)[EXFAT_FBR_FATOFFSET_POS+3]
                                                                 );
    fbr->FatLength =       MAKEFATUINT32((buf)[EXFAT_FBR_FATLENGTH_POS],
                                                                 (buf)[EXFAT_FBR_FATLENGTH_POS+1],
                                                                 (buf)[EXFAT_FBR_FATLENGTH_POS+2],
                                                                 (buf)[EXFAT_FBR_FATLENGTH_POS+3]
                                                                 );
    fbr->ClusterHeapOffset = MAKEFATUINT32((buf)[EXFAT_FBR_CLUSTERHEAPOFFSET_POS],
                                                                 (buf)[EXFAT_FBR_CLUSTERHEAPOFFSET_POS+1],
                                                                 (buf)[EXFAT_FBR_CLUSTERHEAPOFFSET_POS+2],
                                                                 (buf)[EXFAT_FBR_CLUSTERHEAPOFFSET_POS+3]
                                                                 );
    fbr->ClusterCount = MAKEFATUINT32((buf)[EXFAT_FBR_CLUSTERCOUNT_POS],
                                                                 (buf)[EXFAT_FBR_CLUSTERCOUNT_POS+1],
                                                                 (buf)[EXFAT_FBR_CLUSTERCOUNT_POS+2],
                                                                 (buf)[EXFAT_FBR_CLUSTERCOUNT_POS+3]
                                                                 );
    fbr->RootDirectoryCluster = MAKEFATUINT32((buf)[EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS],
                                                                 (buf)[EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS+1],
                                                                 (buf)[EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS+2],
                                                                 (buf)[EXFAT_FBR_ROOTDIRECTORYCLUSTER_POS+3]
                                                                 );
    fbr->VoluemSerialNumber = MAKEFATUINT32((buf)[EXFAT_FBR_VOLUEMSERIALNUMBER_POS],
                                                                 (buf)[EXFAT_FBR_VOLUEMSERIALNUMBER_POS+1],
                                                                 (buf)[EXFAT_FBR_VOLUEMSERIALNUMBER_POS+2],
                                                                 (buf)[EXFAT_FBR_VOLUEMSERIALNUMBER_POS+3]
                                                                 );
    fbr->FileSystemRevision = MAKEFATUINT16((buf)[EXFAT_FBR_FILESYSTEMREVISION_POS],
                                                                 (buf)[EXFAT_FBR_FILESYSTEMREVISION_POS+1]
                                                                 );
    fbr->VoluemFlags = MAKEFATUINT16((buf)[EXFAT_FBR_VOLUEMFLAGS_POS],
                                                                 (buf)[EXFAT_FBR_VOLUEMFLAGS_POS+1]
                                                                 );
    fbr->BytesPerSectorShift = (UINT8)(buf)[EXFAT_FBR_BYTESPERSECTORSHIFT_POS];
    fbr->SectorsPerCluster = (UINT8)(buf)[EXFAT_FBR_SECTORSPERCLUSTER_POS];
    fbr->NumberOfFats = (UINT8)(buf)[EXFAT_FBR_NUMBEROFFATS_POS];
    fbr->DriveSelect = (UINT8)(buf)[EXFAT_FBR_DRIVESELECT_POS];
    fbr->PercentInUse = (UINT8)(buf)[EXFAT_FBR_PERCENTINUSE_POS];
    DSM_MemCpy(fbr->Reserved, &buf[EXFAT_FBR_RESERVED_POS], EXFAT_FBR_RESERVED_SIZE);
    DSM_MemCpy(fbr->BootCode, &buf[EXFAT_FBR_BOOTCODE_POS], EXFAT_FBR_BOOTCODE_SIZE);
    fbr->BootSignature = MAKEFATUINT16((buf)[EXFAT_FBR_BOOTSIGNATURE_POS],
                                                                 (buf)[EXFAT_FBR_BOOTSIGNATURE_POS+1]
                                                                 );
}


// Buffer to Volume Label Directory Entry.
VOID Buf2EXVLDE(UINT8 *buf,EXFAT_VL_DIR_ENTRY *de)
{
    de->EntryType = buf[EXFAT_VL_ENTRYTYPE_POS];
    de->CharacterCount = buf[EXFAT_VL_CHARACTERCOUNT_POS];
    DSM_MemCpy(de->VolumeLabel,&buf[EXFAT_VL_VOLUMELABEL_POS],EXFAT_VL_VOLUMELABEL_SIZE);
    DSM_MemCpy(de->Reserved,&buf[EXFAT_VL_RESERVED_POS],EXFAT_VL_RESERVED_SIZE);
}


// Buffer to Allocation Bitmap Directory Entry.
VOID Buf2EXABDE(UINT8 *buf,EXFAT_AB_DIR_ENTRY *de)
{
    de->EntryType = buf[EXFAT_ABD_ENTRYTYPE_POS];
    de->BitmapFlags = buf[EXFAT_ABD_BITMAPFLAGS_POS];
    DSM_MemCpy(de->Reserved,&buf[EXFAT_ABD_RESERVED_POS],EXFAT_ABD_RESERVED_SIZE);
    de->FirstCluster = MAKEFATUINT32(buf[EXFAT_ABD_FIRSTCLUSTER_POS],
                                                             buf[EXFAT_ABD_FIRSTCLUSTER_POS + 1],
                                                             buf[EXFAT_ABD_FIRSTCLUSTER_POS + 2],
                                                             buf[EXFAT_ABD_FIRSTCLUSTER_POS + 3]
                                                            );
    de->DataLength = MAKEFATUINT64(buf[EXFAT_ABD_DATALENGTH_POS],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 1],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 2],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 3],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 4],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 5],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 6],
                                                             buf[EXFAT_ABD_DATALENGTH_POS + 7]);
}


// Buffer to Up-Case Table Directory Entry.
VOID Buf2EXUCTDE(UINT8 *buf,EXFAT_UCT_DIR_ENTRY*de)
{
    de->EntryType = buf[EXFAT_UCT_ENTRYTYPE_POS];
    DSM_MemCpy(de->Reserved1,&buf[EXFAT_UCT_RESERVED1_POS],EXFAT_UCT_RESERVED1_SIZE);
    de->TableChecksum = MAKEFATUINT32(buf[EXFAT_UCT_TABLECHECKSUM_POS],
                                                                   buf[EXFAT_UCT_TABLECHECKSUM_POS + 1],
                                                                   buf[EXFAT_UCT_TABLECHECKSUM_POS + 2],
                                                                   buf[EXFAT_UCT_TABLECHECKSUM_POS + 3]
                                                                   );
    DSM_MemCpy(de->Reserved2,&buf[EXFAT_UCT_RESERVED2_POS],EXFAT_UCT_RESERVED2_SIZE);
    de->FirstCluster = MAKEFATUINT32(buf[EXFAT_UCT_FIRSTCLUSTER_POS],
                                                             buf[EXFAT_UCT_FIRSTCLUSTER_POS + 1],
                                                             buf[EXFAT_UCT_FIRSTCLUSTER_POS + 2],
                                                             buf[EXFAT_UCT_FIRSTCLUSTER_POS + 3]
                                                            );
    de->DataLength = MAKEFATUINT64(buf[EXFAT_UCT_DATALENGTH_POS],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 1],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 2],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 3],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 4],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 5],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 6],
                                                             buf[EXFAT_UCT_DATALENGTH_POS + 7]
                                                            );
}


void exfat_GetUpCaseTable(UINT8* pBuff)
{
    UINT32 i;

    for(i = 0; i < 0x80; i++)
    {
        if(i >= 0x61  && i <= 0x6F)
        {
            pBuff[i] = ((UINT8)i - 0x61) + 0x41;
        }
        else
        {
            pBuff[i] = (UINT8)i;
        }
    }
}

UINT32 exfat_BootChecksum(const UINT8* data, UINT32 bytes)
{
    UINT32 check_sum   =   0;
    UINT32 i;

    for (i = 0; i < bytes; i++)
    {
        if (i == 106 || i == 107 || i == 112)
        {
            continue;
        }
        check_sum = ((check_sum<<31) | (check_sum>> 1)) + data[i];
    }
    return check_sum;
}


UINT32 exfat_UCTChecksum(const UINT8* data, UINT32 data_len)
{
    UINT32 check_sum   =   0;
    UINT32 i;

    for (i = 0; i < data_len; i++)
    {
        check_sum = ((check_sum<<31) | (check_sum>> 1)) + (UINT32)data[i];
    }

    return check_sum;
}


UINT32 exfat_VGuidChecksum(const UINT8* data, UINT32 data_len)
{
    UINT32 check_sum   =   0;
    UINT32 i;

    for (i = 0; i < data_len; i++)
    {
        check_sum = ((check_sum<<31) | (check_sum>> 1)) + (UINT32)data[i];
    }
    return check_sum;
}


UINT8 exfat_GetSecNumPerClus(UINT32 iDiskTotSec)
{
    UINT64 iDiskTotSec_64;
    UINT64 iDiskToSize;
	UINT32 i;
	UINT8 Powerof2;
    iDiskTotSec_64 = (UINT64)iDiskTotSec;
	iDiskToSize = iDiskTotSec_64 * 512;
    // disks up to 256M, 3 power of 2     4KB
    if( iDiskTotSec < 524228)
    {
        Powerof2 = (UINT8)MAX(0 , 12 - EXFAT_BYTESPERSECTORSHIFT);
		return Powerof2;
    }
    // disks up to 32 GB, .6 power of 2    32KB
    if(iDiskTotSec < 67108864)
    {
        Powerof2 = (UINT8)MAX(0 , 15 - EXFAT_BYTESPERSECTORSHIFT);
		return Powerof2;
    }
    //disks larger than 32 GB, 128GB or more
	for(i = 17; ; i++)
	{
        if(DIV_ROUND_UP(iDiskToSize , 1 << i) <= EXFAT_LAST_DATA_CLUSTER)
        {
            Powerof2 = (UINT8)MAX(0 , i - EXFAT_BYTESPERSECTORSHIFT);
			return Powerof2;
		}
	}
    /*
    else if(iCountOfSec >= 0x4000000000 && iCountOfSec <= 0x400000000000)
    {
        iPowerOf2 = 16;
    }
    else if(iCountOfSec >= 0x400000000000 && iCountOfSec <= 0x4000000000000)
    {
        iPowerOf2 = 18;
    }
    else if(iCountOfSec >= 0x4000000000000 && iCountOfSec <= 0x40000000000000)
    {
        iPowerOf2 = 20;
    }
    else if(iCountOfSec >= 0x40000000000000 && iCountOfSec <= 0x400000000000000)
    {
        iPowerOf2 = 22;
    }
    else if(iCountOfSec >= 0x400000000000000 && iCountOfSec <= 0x4000000000000000)
    {
        iPowerOf2 = 24;
    }
    else if(iCountOfSec >= 0x4000000000000000 && iCountOfSec <= 0x40000000000000000)
    {
        iPowerOf2 = 25;
    }
    */
}


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
            )
{
    // Set the fbr stuct.
    DSM_MemSet(pBR,0,SIZEOF(EXFAT_BOOT_RECORD));

    // Index  FieldName Offset Size(byte)
    // 1 jumpBoot              0   3
    pBR->jumpBoot[0] = 0xEB;
    pBR->jumpBoot[1] = 0x76;
    pBR->jumpBoot[2] = 0x90;

    // 2 FileSystemName     3   8
    DSM_StrNCpy(pBR->FileSystemName,fsname,EXFAT_FBR_FILESYSTEMNAME_SIZE);

    // 3 MustBeZero           11  53
    DSM_MemSet(pBR->MustBeZero,0,EXFAT_FBR_MUSTBEZERO_SIZE);

    // 4 PartitionOffset        64  8
    pBR->PartitionOffset = iPartitionOffset;

    // 5 ValuemLength        72   8
    pBR->ValuemLength = iTotSec;

    // 6 FatOffset                80   4
    pBR->FatOffset = iFatOffset;

    // 7 FatLength               84   4
    pBR->FatLength = iFATSz / EXFAT_DEFAULT_SECSIZE;

    // 8 ClusterHeapOffset   88   4
    pBR->ClusterHeapOffset = iClusterHeapOffset;

    // 9 ClusterCount         92  4
    pBR->ClusterCount = iCountOfClus;

    // 10 RootDirectoryCluster  96  4
    pBR->RootDirectoryCluster = iRootDirCluster;

    // 11 VoluemSerialNumber  100  4
    pBR->VoluemSerialNumber = volserial;

    // 12 FileSystemRevision     104  2
    pBR->FileSystemRevision = EXFAT_FILE_SYSTEM_REVISION;

    // 13 VoluemFlags               106   2
    pBR->VoluemFlags = EXFAT_VOLUEM_FLAGS;

    // 14 VoluemFlags               108   1
    pBR->BytesPerSectorShift = EXFAT_BYTESPERSECTORSHIFT;

    // 15 SectorsPerCluster       109   1
    pBR->SectorsPerCluster = exfat_GetSecNumPerClus(iTotSec);

    // 16 NumberOfFats            110   1
    pBR->NumberOfFats = EXFAT_FAT_COUNT;

    // 17 DriveSelect               111    1
    pBR->DriveSelect = EXFAT_DRIVESELECT;

    // 18 PercentInUse             112    1
    pBR->PercentInUse = 0;

    // 19 Reserved                  113   7
    DSM_MemSet(pBR->Reserved,0,EXFAT_FBR_RESERVED_SIZE);

    // 20 BootCode                  120   390
    DSM_MemSet(pBR->BootCode,0,EXFAT_FBR_BOOTCODE_SIZE);

    // 21 BootSignature            510   2
    pBR->BootSignature = EXFAT_FSI_TRAILSIG;
}

VOID exfat_GetEBR(EXFAT_EXTEND_BOOT_RECORD *pEBR)
{
    DSM_MemSet(pEBR,0,sizeof(EXFAT_EXTEND_BOOT_RECORD));
    pEBR->ExtBootSignature = EXFAT_FSI_TRAILSIG;
}


VOID exfat_GetOEM(EXFAT_OEM_RECORD* pOR)
{
    DSM_MemSet(pOR,0,sizeof(EXFAT_OEM_RECORD));
}


VOID exfat_SetBootChecksum(UINT32 iChecksum,UINT8* pBuff)
{
    UINT32* p;
    UINT32 i;

    p = (UINT32*)pBuff;
    for(i = 0; i < DEFAULT_SECSIZE/4; i++)
    {
            *p = iChecksum;
            p++;
    }
}

UINT32 exfat_GetFATSz(UINT32 iTotalSec,
	               UINT32 iFatOffset,
                   UINT32 iSecPerClus)
{
    UINT32 iFATSz;
    UINT32 iResvdSec;
	UINT32 iClusMax;
//    UINT32 iDataSec;
    UINT32 iFAT_SECTORS;

    iResvdSec = iFatOffset;
	iClusMax = (UINT32)(iTotalSec / iSecPerClus);
	iFAT_SECTORS = (UINT32)(DIV_ROUND_UP(iClusMax * UINT32_SIZE , EXFAT_DEFAULT_SECSIZE));
    iFATSz = (UINT32)(ROUND_UP((iResvdSec + iFAT_SECTORS) , iSecPerClus) - iResvdSec);
    return iFATSz;
}

UINT32 exfat_GetBitMapSz(UINT32 iCountOfClus)
{
    return (UINT32)DIV_ROUND_UP(iCountOfClus , EXFAT_BITMAP_CHAR_BIT);
}

UINT32 exfat_GetUCTSz()
{
    return EXFAT_UCT_LENGTH;
}

UINT32 exfat_GetRootDirSz(UINT32 iSecPerClus)
{
    return (UINT32)iSecPerClus * EXFAT_DEFAULT_SECSIZE;
}

VOID exfat_SetRootDir(UINT8* pValueName, UINT32 iBitmapLength, UINT32 iBitmapClusCnt,UINT32 iUCTLenth,UINT32 iTableChecksum,UINT8* pBuff)
{
     EXFAT_VL_DIR_ENTRY  vl_dentry;
     EXFAT_UCT_DIR_ENTRY uct_dentry;
     EXFAT_AB_DIR_ENTRY ab_dentry;
     UINT8 *p = NULL;
	 UINT8 *pUniVolLab = NULL;
	 UINT32 iULen = 0;

     p = pBuff;
     // Volume Label.
     vl_dentry.EntryType = EXFAT_ETYPE_VL;
     vl_dentry.CharacterCount = (UINT8)DSM_StrLen(pValueName);
	 DSM_OEM2Uincode(pValueName, (UINT16)DSM_StrLen(pValueName), &pUniVolLab, &iULen, NULL);
	 DSM_MemSet((WCHAR*)vl_dentry.VolumeLabel, 0, EXFAT_VL_VOLUMELABEL_SIZE);
     DSM_MemCpy((WCHAR*)vl_dentry.VolumeLabel,pUniVolLab, iULen);
	 if(pUniVolLab != NULL)
	 {
		DSM_Free(pUniVolLab);
	 }

     DSM_MemSet(vl_dentry.Reserved,0,EXFAT_VL_RESERVED_SIZE);

     DSM_MemCpy(p,&vl_dentry,sizeof(EXFAT_VL_DIR_ENTRY));
     p += EXFAT_DIR_ENTRY_SIZE;

     // Allocation Bitmap
     ab_dentry.EntryType = EXFAT_ETYPE_AB;
     ab_dentry.BitmapFlags = EXFAT_BITMAP_FLAGS_1ST;
     DSM_MemSet(ab_dentry.Reserved,0,EXFAT_ABD_RESERVED_SIZE);
     ab_dentry.FirstCluster = EXFAT_FST_CLUSTER_AB;
     ab_dentry.DataLength = iBitmapLength;

     DSM_MemCpy(p,&ab_dentry,sizeof(EXFAT_AB_DIR_ENTRY));
     p += EXFAT_DIR_ENTRY_SIZE;

     // Up-case Table
     uct_dentry.EntryType = EXFAT_ETYPE_UCT;
     DSM_MemSet(uct_dentry.Reserved1,0,EXFAT_UCT_RESERVED1_SIZE);
     uct_dentry.TableChecksum = iTableChecksum;
     DSM_MemSet(uct_dentry.Reserved2,0,EXFAT_UCT_RESERVED2_SIZE);
     uct_dentry.FirstCluster = EXFAT_FST_CLUSTER_AB + iBitmapClusCnt;
     uct_dentry.DataLength = iUCTLenth;

     DSM_MemCpy(p,&uct_dentry,sizeof(EXFAT_UCT_DIR_ENTRY));

}

VOID exfat_SetBitMap(UINT8* pBuff,UINT32 iBitmapClusCnt)
{
	UINT32 Cluster = 0;
	UINT8 p = 0x00;
	for(Cluster = 0; Cluster < (2 + iBitmapClusCnt); Cluster++)
	{
        p |= 1<<(Cluster % 8);
	}

	*pBuff = p;
}

VOID exfat_SetFat(UINT8* pBuff)
{
      UINT32* pFatEntry;

      pFatEntry = (UINT32*)pBuff;

      pFatEntry[0] = EXFAT_FAT_VALUE_MEDIA;

      pFatEntry[1] = EXFAT_FAT_VALUE_RESERVED;

      pFatEntry[2] = EXFAT_FAT_VALUE_EOF;

      pFatEntry[3] = EXFAT_FAT_VALUE_EOF;

      pFatEntry[4] = EXFAT_FAT_VALUE_EOF;

}

VOID exfat_BFR2SB(EXFAT_BOOT_RECORD* boot_rec,EXFAT_SB_INFO* sb_info)
{
    sb_info->PartitionOffset = boot_rec->PartitionOffset;
    sb_info->ValuemLength = boot_rec->ValuemLength;
    sb_info->FatOffset = boot_rec->FatOffset;
    sb_info->FatLength = boot_rec->FatLength;
    sb_info->ClusterHeapOffset = boot_rec->ClusterHeapOffset;
    sb_info->ClusterCount = boot_rec->ClusterCount;
    sb_info->RootDirectoryCluster = boot_rec->RootDirectoryCluster;
    sb_info->VoluemSerialNumber = boot_rec->VoluemSerialNumber;
    sb_info->FileSystemRevision = boot_rec->FileSystemRevision;
    sb_info->VoluemFlags = boot_rec->VoluemFlags;
    sb_info->BytesPerSector = (1 << boot_rec->BytesPerSectorShift);
    sb_info->SectorsPerCluster = (1<<boot_rec->SectorsPerCluster);
    sb_info->BATCluster = EXFAT_FST_CLUSTER_AB;
    sb_info->UCTCluster = EXFAT_FST_CLUSTER_UCT;
    sb_info->NumberOfFats = boot_rec->NumberOfFats;
    sb_info->DriveSelect = boot_rec->DriveSelect;
    sb_info->PercentInUse = boot_rec->PercentInUse;
    sb_info->CurCluster = boot_rec->RootDirectoryCluster;
    sb_info->BATCache.iDirty = 0;
    sb_info->BATCache.iSecNum = 0;
    sb_info->BATCache.psFileInodeList = NULL;
    DSM_MemSet(sb_info->BATCache.szSecBuf,0,EXFAT_DEFAULT_SECSIZE);
    sb_info->FATCache.iDirty = 0;
    sb_info->FATCache.iSecNum = 0;
    sb_info->FATCache.psFileInodeList = NULL;
    DSM_MemSet(sb_info->FATCache.szSecBuf,0,EXFAT_DEFAULT_SECSIZE);
    return;
}


INT32 exfat_read_root_dir(UINT32 DevNo,UINT32 SecNum,UINT32* BATClus,UINT32* UCTClus, UINT32* UCTTable)
{
    INT32 result;
    INT32 ret = _ERR_FAT_DENRTY_PARSE_ERROR;
    UINT8* secbuf;
    UINT32 secoffs;
    EXFAT_DIR_ENTRY* dir_entry;
    EXFAT_AB_DIR_ENTRY* ab_entry;
    EXFAT_UCT_DIR_ENTRY* uct_entry;

    *BATClus = 0;
    *UCTClus = 0;
    secbuf = FAT_SECT_BUF_ALLOC();
    if(NULL == secbuf)
    {
        D((DL_FATERROR, "exfat_read_root_dir: FAT_SECT_BUF_ALLOC failed."));
              DSM_ASSERT(0,"exfat_read_root_dir: FAT_SECT_BUF_ALLOC failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }
    result = DRV_BLOCK_READ( DevNo, SecNum, secbuf);
    if(_ERR_FAT_SUCCESS != result)
    {
        D((DL_FATERROR, "exfat_read_root_dir(),DRV_BLOCK_READ error ErrorCode = %d",result));
        ret  = _ERR_FAT_READ_SEC_FAILED;
        goto _func_end;
    }

    for(secoffs = 0; secoffs < EXFAT_DEFAULT_SECSIZE; secoffs += DEFAULT_DIRENTRYSIZE)
    {
        if(0 != *BATClus && 0 != *UCTClus)
        {
            ret = ERR_SUCCESS;
            break;
        }
        dir_entry = (EXFAT_DIR_ENTRY*)(secbuf + secoffs);
        if(EXFAT_ETYPE_AB == dir_entry->EntryType)
        {
             ab_entry = (EXFAT_AB_DIR_ENTRY*)dir_entry;
             *BATClus = ab_entry->FirstCluster;
        }
        if(EXFAT_ETYPE_UCT == dir_entry->EntryType)
        {
             uct_entry = (EXFAT_UCT_DIR_ENTRY*)dir_entry;
			 *UCTTable = uct_entry->TableChecksum;
             *UCTClus = uct_entry->FirstCluster;
        }
    }

 _func_end:
    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE(secbuf);
    }

    return ret;
}


INT32 exfat_read_sb_info(UINT32 DevNo,UINT32 SecNum,EXFAT_SB_INFO* sb_info)
{
    EXFAT_BOOT_RECORD boot_rec;
    INT32 iResult;
    INT32 iRet;
    UINT8* secbuf = NULL;
    UINT32 value_32 = 1;
    UINT32 RootSecNum = 0;
    UINT32 BATClus = 0;
	UINT8* BitMap_buf = NULL;
	UINT32 iBitMapSz = 0;
	UINT32 iBitMapClusCnt = 0;
	UINT32 iBitMapAlign = 0;
    UINT32 UCTClus = 0;
	UINT32 UCTSec;
	UINT8* iUctTable = NULL;
	UINT32 iUCTChecksum_Cpt = 0;
	UINT32 iUCTChecksum_Read = 0;
    UINT32 iStartSec;
    UINT32 iOffs;
    UINT32 iTmpSize;

    CSW_TRACE(BASE_DSM_TS_ID, "debug EXFAT: begin.");
    iRet = _ERR_FAT_SUCCESS;
    secbuf = FAT_SECT_BUF_ALLOC();
	BitMap_buf = DSM_MAlloc(EXFAT_DEFAULT_SECSIZE);
	iUctTable = DSM_MAlloc(EXFAT_UCT_LENGTH);
    if(NULL == secbuf)
    {
        D((DL_FATERROR, "exfat_read_sb_info: FAT_SECT_BUF_ALLOC failed."));
              DSM_ASSERT(0,"exfat_read_sb_info: FAT_SECT_BUF_ALLOC failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

	if(NULL == BitMap_buf)
    {
        D((DL_FATERROR, "exfat_read_sb_info: FAT_BITMAP_BUF_ALLOC failed."));
              DSM_ASSERT(0,"exfat_read_sb_info: FAT_BITMAP_BUF_ALLOC failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

    if(NULL == iUctTable)
    {
        D((DL_FATERROR, "exfat_read_sb_info: UCT_TABLE failed."));
              DSM_ASSERT(0,"exfat_read_sb_info: UCT_TABLE failed.");
        return _ERR_FAT_MALLOC_FAILED;
    }

     // Check the sector 0 to find the MBR.
    iResult = DRV_BLOCK_READ( DevNo, SecNum, secbuf);
    if(_ERR_FAT_SUCCESS != iResult)
    {
        D((DL_FATERROR, "exfat_read_sb_info(),read block error,DevNo = %d,SecNum = 0x%x ErrorCode = %d",
                DevNo,SecNum,iResult));
        iRet = _ERR_FAT_READ_SEC_FAILED;
        goto end;
    }
    Buf2EXFBR(secbuf,&boot_rec);
    if(EXFAT_FSI_TRAILSIG != boot_rec.BootSignature)
    {
        D((DL_FATERROR, " BootSignature = 0x%x",boot_rec.BootSignature));
        CSW_TRACE(BASE_DSM_TS_ID, "BootSignature = 0x%x",boot_rec.BootSignature);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    CSW_TRACE(BASE_DSM_TS_ID, "debug EXFAT:boot_rec:\n"
            "PartitionOffset = 0x%x,\n",
            boot_rec.PartitionOffset
            );
    if(SecNum != boot_rec.PartitionOffset)
    {
        D((DL_FATERROR, " iSecNum = 0x%x,PartitionOffset = 0x%x",SecNum,boot_rec.PartitionOffset));
        CSW_TRACE(BASE_DSM_TS_ID, " iSecNum = 0x%x,PartitionOffset = 0x%x",SecNum,boot_rec.PartitionOffset);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }

    if(boot_rec.ValuemLength < (UINT32)(value_32<<20) || boot_rec.ValuemLength > (UINT32)((value_32<<63) -1))
    {
        D((DL_FATERROR, " ValuemLength = 0x%x",boot_rec.ValuemLength));
        CSW_TRACE(BASE_DSM_TS_ID, " ValuemLength = 0x%x",boot_rec.ValuemLength);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }

    if(boot_rec.FatOffset < 24)
    {
        D((DL_FATERROR, " FatOffset = 0x%x",boot_rec.FatOffset));
        CSW_TRACE(BASE_DSM_TS_ID," FatOffset = 0x%x",boot_rec.FatOffset);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.FatLength == 0)
    {
        D((DL_FATERROR, " FatLength = 0x%x",boot_rec.FatLength));
        CSW_TRACE(BASE_DSM_TS_ID," FatLength = 0x%x",boot_rec.FatLength);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.ClusterHeapOffset < (boot_rec.FatOffset + boot_rec.FatLength)*boot_rec.NumberOfFats)
    {
        D((DL_FATERROR, " ClusterHeapOffset = 0x%x",boot_rec.ClusterHeapOffset));
        CSW_TRACE(BASE_DSM_TS_ID," ClusterHeapOffset = 0x%x",boot_rec.ClusterHeapOffset);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.ClusterCount == 0)
    {
        D((DL_FATERROR, " ClusterCount = 0x%x",boot_rec.ClusterCount));
        CSW_TRACE(BASE_DSM_TS_ID," ClusterCount = 0x%x",boot_rec.ClusterCount);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.RootDirectoryCluster < EXFAT_CLUSTER_DATA_START)
    {
        D((DL_FATERROR, " RootDirectoryCluster = 0x%x",boot_rec.RootDirectoryCluster));
        CSW_TRACE(BASE_DSM_TS_ID," RootDirectoryCluster = 0x%x",boot_rec.RootDirectoryCluster);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.BytesPerSectorShift != EXFAT_BYTESPERSECTORSHIFT)
    {
        D((DL_FATERROR, " BytesPerSectorShift = 0x%x",boot_rec.BytesPerSectorShift));
        CSW_TRACE(BASE_DSM_TS_ID," BytesPerSectorShift = 0x%x",boot_rec.BytesPerSectorShift);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.SectorsPerCluster > 25)
    {
        D((DL_FATERROR, " SectorsPerCluster = 0x%x",boot_rec.SectorsPerCluster));
        CSW_TRACE(BASE_DSM_TS_ID," SectorsPerCluster = 0x%x",boot_rec.SectorsPerCluster);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.NumberOfFats < 1 || boot_rec.NumberOfFats > 2)
    {
        D((DL_FATERROR, " NumberOfFats = 0x%x",boot_rec.NumberOfFats));
        CSW_TRACE(BASE_DSM_TS_ID," NumberOfFats = 0x%x",boot_rec.NumberOfFats);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    if(boot_rec.PercentInUse > 100)
    {
        D((DL_FATERROR, " PercentInUse = 0x%x",boot_rec.PercentInUse));
        CSW_TRACE(BASE_DSM_TS_ID," PercentInUse = 0x%x",boot_rec.PercentInUse);
        iRet = _ERR_FAT_BPB_ERROR;
        goto end;
    }
    exfat_BFR2SB(&boot_rec,sb_info);

	//Read RootDir
    RootSecNum = sb_info->PartitionOffset + sb_info->ClusterHeapOffset +\
        (sb_info->RootDirectoryCluster - EXFAT_CLUSTER_DATA_START)*sb_info->SectorsPerCluster;
    CSW_TRACE(BASE_DSM_TS_ID," PartitionOffset = 0x%x",sb_info->PartitionOffset);
    CSW_TRACE(BASE_DSM_TS_ID," ClusterHeapOffset = 0x%x",sb_info->ClusterHeapOffset);
    CSW_TRACE(BASE_DSM_TS_ID," RootDirectoryCluster = 0x%x",sb_info->RootDirectoryCluster);
    CSW_TRACE(BASE_DSM_TS_ID," SectorsPerCluster = 0x%x",sb_info->SectorsPerCluster);
    CSW_TRACE(BASE_DSM_TS_ID," RootSecNum = 0x%x",RootSecNum);
    iResult = exfat_read_root_dir(DevNo,RootSecNum,&BATClus,&UCTClus,&iUCTChecksum_Read);
    if(ERR_SUCCESS == iResult)
    {
        sb_info->BATCluster = BATClus;
        sb_info->UCTCluster = UCTClus;
    }
    else
    {
        CSW_TRACE(BASE_DSM_TS_ID," exfat_read_root_dir faile iResult = %d",iResult);
        iRet = _ERR_FAT_ROOTDIR_ERROR;
        goto end;
    }
    // Read and check UCT
    // UCT(UP-Case Table).

    iStartSec= (UINT32)sb_info->PartitionOffset + sb_info->ClusterHeapOffset  + \
                     (sb_info->UCTCluster - EXFAT_CLUSTER_DATA_START) *\
                     (UINT32)sb_info->SectorsPerCluster;
	UCTSec = (EXFAT_UCT_LENGTH + DEFAULT_SECSIZE - 1)/DEFAULT_SECSIZE;
    for(iOffs = 0; iOffs < UCTSec; iOffs ++)
    {

        SecNum = iStartSec + iOffs;
		DSM_MemSet(secbuf,0,DEFAULT_SECSIZE);
        iResult = DRV_BLOCK_READ(DevNo, SecNum,secbuf);
        if(_ERR_FAT_SUCCESS != iResult)
        {
            D((DL_FATERROR, "exfat_read_sb_info(),read block error,DevNo = %d,SecNum = 0x%x ErrorCode = %d",
                DevNo,SecNum,iResult));
            iRet = _ERR_FAT_READ_SEC_FAILED;
            goto end;
        }

        if(iOffs < UCTSec -1)
        {
             DSM_MemCpy((UINT8*)iUctTable + iOffs*DEFAULT_SECSIZE,secbuf,DEFAULT_SECSIZE);
        }
        else
        {
             iTmpSize = (EXFAT_UCT_LENGTH - iOffs*DEFAULT_SECSIZE);
             DSM_MemCpy((UINT8*)iUctTable + iOffs*DEFAULT_SECSIZE,secbuf,iTmpSize);
        }
    }

	iUCTChecksum_Cpt = exfat_UCTChecksum(iUctTable,EXFAT_UCT_LENGTH);
	if(iUCTChecksum_Cpt != iUCTChecksum_Read)
	{
        CSW_TRACE(BASE_DSM_TS_ID," exfat_uct_check fail!");
	    iRet = _ERR_FAT_UCT_CHECK_ERROR;
		goto end;
	}

	//Read BitMap
	iStartSec= (UINT32)sb_info->PartitionOffset + sb_info->ClusterHeapOffset  + \
                     (sb_info->BATCluster - EXFAT_CLUSTER_DATA_START) *\
                     (UINT32)sb_info->SectorsPerCluster;
	SecNum = iStartSec;
	DSM_MemSet(secbuf,0,DEFAULT_SECSIZE);
	iResult = DRV_BLOCK_READ(DevNo, SecNum,secbuf);
	if(_ERR_FAT_SUCCESS != iResult)
    {
        D((DL_FATERROR, "exfat_read_sb_info(),read block error,DevNo = %d,SecNum = 0x%x ErrorCode = %d",
                DevNo,SecNum,iResult));
        iRet = _ERR_FAT_READ_SEC_FAILED;
        goto end;
    }

    iBitMapSz = exfat_GetBitMapSz(sb_info->ClusterCount);
    iBitMapAlign = sb_info->SectorsPerCluster * DEFAULT_SECSIZE;
    iBitMapClusCnt = DIV_ROUND_UP(iBitMapSz, iBitMapAlign);
    exfat_SetBitMap(BitMap_buf,iBitMapClusCnt);
	if(*BitMap_buf != (*secbuf & *BitMap_buf))
	{
        CSW_TRACE(BASE_DSM_TS_ID," exfat_bitmap_check fail!");
	    iRet = _ERR_FAT_BITMAP_CHECK_ERROR;
		goto end;
	}

    iRet = ERR_SUCCESS;
 end:

    if(NULL != secbuf)
    {
        FAT_SECT_BUF_FREE(secbuf);
    }

	if(NULL != BitMap_buf)
	{
        DSM_Free(BitMap_buf);
	}

	if(NULL != iUctTable)
	{
        DSM_Free(iUctTable);
	}
    return iRet;
}

// this function is error!!!
INT32 exfat_read_cluster(EXFAT_SB_INFO *sb_info, UINT32 clunum,
    UINT32 offset, UINT8 *secbuf)
{
    UINT32 secnum;
    INT32 iResult;
    INT32 iRet;
    UINT32 iTime = 0;

    if(NULL == secbuf)
    {
        //D( ( DL_FATERROR, "In exfat_read_cluster paramert error.secbuf == NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    if (clunum > sb_info->ClusterCount || clunum < 2 || offset >= sb_info->SectorsPerCluster)
    {
        D((DL_FATERROR, "exfat_read_cluster: parameter error.clunum = 0x%x, SecOffset = %d.!\n", clunum, offset));
        DSM_HstSendEvent(0xbabe0001);
        return _ERR_FAT_ERROR;
    }

    iRet = _ERR_FAT_SUCCESS;

    secnum = sb_info->PartitionOffset + sb_info->ClusterHeapOffset
                        + ((clunum-2) * sb_info->SectorsPerCluster)+ offset;

    CSW_TRACE(BASE_FFS_TS_ID, "exfat_debug:exfat_read_cluster,clunum = 0x%x,offset = %d,secnum = 0x%x",
            clunum,offset,secnum);
    do
    {

        iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, secnum, secbuf );
        if(_ERR_FAT_SUCCESS == iResult)
        {
            iRet = _ERR_FAT_SUCCESS;
            break;
        }
        else
        {
            iRet = iResult;
            D( ( DL_FATERROR, "DRV_BLOCK_READ() failed!!! Local error code:%d,\n",iResult) );
            D( ( DL_FATERROR, "sb_info->sb->s_dev = %d, rsecnum = %d, secbuf = 0x%x,dev handle = 0x%x.\n",
                sb_info->sb->s_dev,
                secnum,
                secbuf,
                g_pDevDescrip[ sb_info->sb->s_dev ].dev_handle));
            D( ( DL_FATERROR,
                "sb_info->SectorsPerCluster = %d,clunum = %d,sb_info->ClusterHeapOffset = %d,offset = %d.\n",
                sb_info->SectorsPerCluster,
                clunum,
                sb_info->ClusterHeapOffset,
                offset));
        }
        iTime ++;
    }while(iTime <= 3);
    return iRet;
}


INT32 exfat_write_cluster(EXFAT_SB_INFO *sb_info, UINT32 clunum,
    UINT32 offset, UINT8 *secbuf)
{
    INT32 iResult;
    INT32 iRet;
    UINT32 secnum;
    UINT32 iTime = 0;

    if(NULL == secbuf)
    {
        D( ( DL_FATERROR, "In fat_write_cluster paramert error.secbuf == NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }

    if (clunum > sb_info->ClusterCount || clunum < 2 || offset >= sb_info->SectorsPerCluster)
    {
        D((DL_FATERROR, "exfat_write_cluster: parameter error.clunum = 0x%x, offset = %d.!\n", clunum, offset));
        return _ERR_FAT_ERROR;
    }

    iRet = _ERR_FAT_SUCCESS;

    secnum = sb_info->PartitionOffset + sb_info->ClusterHeapOffset
                        + ((clunum-2) * sb_info->SectorsPerCluster)+ offset;
    CSW_TRACE(BASE_FFS_TS_ID, "exfat_debug:exfat_write_cluster,clunum = 0x%x,offset = %d,secnum = 0x%x",
            clunum,offset,secnum);
   do
    {
        iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev, secnum, secbuf );
        if(_ERR_FAT_SUCCESS == iResult)
        {
            iRet = _ERR_FAT_SUCCESS;
            break;
        }
        else
        {
            D( ( DL_FATERROR, "In exfat_write_cluster: DRV_BLOCK_WRITE() return failed. ErrorCode = %d\n",iResult) );
            iRet = _ERR_FAT_WRITE_SEC_FAILED;
        }
        iTime ++;
    }while(iTime <= 3);
    return iRet;
}


INT32 exfat_set_cluser_zero(EXFAT_SB_INFO *sb_info, UINT32 clunum)
{
    INT32 iResult;
    INT32 iRet = _ERR_FAT_SUCCESS;
    UINT32 fst_clus_sec,sec_num;
    UINT32 i;
    UINT8 *secbuf = NULL;

    if (clunum > sb_info->ClusterCount || clunum < 2 )
    {
        D((DL_FATERROR, "exfat_set_cluser_zero: parameter error.clunum = 0x%x.!\n", clunum));
        return _ERR_FAT_ERROR;
    }

    iRet = _ERR_FAT_SUCCESS;

    fst_clus_sec = sb_info->PartitionOffset + sb_info->ClusterHeapOffset
                        + ((clunum-2) * sb_info->SectorsPerCluster);
    CSW_TRACE(BASE_FFS_TS_ID, "exfat_debug:exfat_set_cluser_zero,clunum = 0x%x,fst_clus_sec = 0x%x",
            clunum,fst_clus_sec);

    secbuf = DSM_MAlloc(DEFAULT_SECSIZE);
    if(NULL == secbuf)
    {
        D( ( DL_FATERROR, "In exfat_set_cluser_zero paramert error.secbuf == NULL.\n") );
        return _ERR_FAT_PARAM_ERROR;
    }
    DSM_MemSet(secbuf,0,DEFAULT_SECSIZE);

    for(i = 0; i < sb_info->SectorsPerCluster; i++)
    {
        sec_num = fst_clus_sec+ i;
        iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev, sec_num, secbuf );
        if(_ERR_FAT_SUCCESS != iResult)
        {
            D( ( DL_FATERROR, "In exfat_set_cluser_zero: DRV_BLOCK_WRITE() return failed. ErrorCode = %d\n",iResult) );
            iRet = _ERR_FAT_WRITE_SEC_FAILED;
            break;
        }
    }
    if(secbuf)
    {
        DSM_Free(secbuf);
    }
    return iRet;
}


VOID exfat_build_file_name(EXFAT_NAME_DIR_ENTRY* pdentry,UINT32 count,WCHAR* file_name)
{
    UINT32 i;

    for(i = 2; i < count; i++ )
    {
        if(i == 2)
        {
            DSM_TcStrNCpy(file_name,(WCHAR*)pdentry[i].FileName,sizeof(pdentry[i].FileName)/2);
        }
        else
        {
            DSM_TcStrNCpy(file_name + DSM_TcStrLen(file_name),(WCHAR*)pdentry[i].FileName,sizeof(pdentry[i].FileName)/2);
        }
    }
    return;
}


UINT16 exfat_file_name_hash(PCWSTR filename)
{
    UINT16 checksum = 0;
    UINT16* pUCT;
    UINT32 len,i;
    UINT16 c;
    UINT16 id;

    len = DSM_TcStrLen(filename);
    pUCT = (UINT16*)g_UctTable;
    for(i = 0; i < len; i++)
    {
        id = filename[i];
        if(id < g_UctTableLen)
        {
            c = pUCT[id];
        }
        else
        {
            c = id;
        }
        checksum = (UINT16) (((checksum << 15) | (checksum >> 1)) + (c & 0xff));
        checksum = (UINT16) (((checksum << 15) | (checksum >> 1)) + ((c & 0xff00)>> 8));
    }
    return checksum;
}


INT32 exfat_get_next_cluster(struct inode *inode, UINT32 cur_clus, UINT32 *next_clus)
{
    UINT32 FATOffset;
    UINT32 ThisFATSecNum;
    UINT32 ThisFATEntOffset;
    UINT32 FATCluEntry = 0;
    EXFAT_CACHE *psFatCache = NULL;
    INT32 iResult;
    INT32 iRet;
    EXFAT_SB_INFO *sb_info;
    struct exfat_inode_info* exfat_i;
    EXFAT_EXT_DIR_ENTRY* ext_entry;

    iRet = _ERR_FAT_SUCCESS;
    // if clusters is sequential,the next cluster is cur_clus + 1, else get it from FAT.
    sb_info = &inode->i_sb->u.exfat_sb;
    exfat_i = &inode->u.exfat_i;
    ext_entry = &exfat_i->ext_entry;
    if(EXFAT_GENSECON_FLAGS_NOFATCHAIN(ext_entry->GenSeconFlags))
    {
        if(EXFAT_IS_LAST_FAT_ENTRY(ext_entry->FirstCluster,
                           cur_clus,
                           (sb_info->SectorsPerCluster * sb_info->BytesPerSector),
                           ext_entry->ValidDataLength)
           || cur_clus <= sb_info->RootDirectoryCluster)
        {
            *next_clus = EXFAT_CLUSTER_FINAL;
        }
        else
        {
            *next_clus = cur_clus + 1;
        }
        return ERR_SUCCESS;
    }

    psFatCache = &sb_info->FATCache;
    FATOffset = cur_clus*4;
    ThisFATSecNum = EXFAT_GET_FAT_SEC_NUM(sb_info,FATOffset);
    ThisFATEntOffset = EXFAT_GET_FAT_ENT_OFFS(sb_info,FATOffset);
    if (psFatCache->iSecNum != ThisFATSecNum)
    {
        if (psFatCache->iSecNum != 0)
        {
            iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev, psFatCache->iSecNum , psFatCache->szSecBuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                //  psFatCache->iSecNum = 0;
                iRet = _ERR_FAT_READ_SEC_FAILED;
                D((DL_FATERROR, "exfat_get_next_cluster(), write block failed!iSecNum = 0x%x,iResult = %d\n",
                    psFatCache->iSecNum,iResult));
                goto end;
            }
        }

        iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum, psFatCache->szSecBuf);
        if(_ERR_FAT_SUCCESS != iResult)
        {
            psFatCache->iSecNum = 0;
            iRet = _ERR_FAT_READ_SEC_FAILED;
            D((DL_FATERROR, "exfat_get_next_cluster(), read block failed!ThisFATSecNum = 0x%x, iResult = %d\n",
                ThisFATSecNum, iResult));
            goto end;
        }

        psFatCache->iSecNum = ThisFATSecNum;
        psFatCache->iDirty = 0;
    }
    FATCluEntry = MAKEFATUINT32(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset+1],
                                                     psFatCache->szSecBuf[ThisFATEntOffset+2], psFatCache->szSecBuf[ThisFATEntOffset+3]);
    if (FATCluEntry == cur_clus)
    {
        D((DL_FATERROR, "exfat_get_next_cluster: next-cluster(0x%x) == current-cluster(0x%x).", FATCluEntry, cur_clus));
        FATCluEntry = 0xFFFFFFFF;
    }
    *next_clus = FATCluEntry;

    iRet = _ERR_FAT_SUCCESS;

end:

    return iRet;
}

INT32 exfat_get_next_cluster1(EXFAT_SB_INFO *sb_info, UINT32 cur_clus, UINT32 *next_clus)
{
    UINT32 FATOffset;
    UINT32 ThisFATSecNum;
    UINT32 ThisFATEntOffset;
    UINT32 FATCluEntry = 0;
    EXFAT_CACHE *psFatCache = NULL;
    INT32 iResult;
    INT32 iRet;
    // EXFAT_INODE_INFO* inode_info;

    iRet = _ERR_FAT_SUCCESS;

    psFatCache = &sb_info->FATCache;
    FATOffset = cur_clus*4;
    ThisFATSecNum = EXFAT_GET_FAT_SEC_NUM(sb_info,FATOffset);
    ThisFATEntOffset = EXFAT_GET_FAT_ENT_OFFS(sb_info,FATOffset);
    if (psFatCache->iSecNum != ThisFATSecNum)
    {
        if (psFatCache->iSecNum != 0)
        {
            iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev, psFatCache->iSecNum , psFatCache->szSecBuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                //  psFatCache->iSecNum = 0;
                iRet = _ERR_FAT_READ_SEC_FAILED;
                D((DL_FATERROR, "exfat_get_next_cluster(), write block failed!iSecNum = 0x%x,iResult = %d\n",psFatCache->iSecNum,iResult));
                goto end;
            }
        }

        iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum, psFatCache->szSecBuf);
        if(_ERR_FAT_SUCCESS != iResult)
        {
            psFatCache->iSecNum = 0;
            iRet = _ERR_FAT_READ_SEC_FAILED;
            D((DL_FATERROR, "exfat_get_next_cluster(), read block failed!ThisFATSecNum = 0x%x, iResult = %d\n",ThisFATSecNum, iResult));
            goto end;
        }

        psFatCache->iSecNum = ThisFATSecNum;
        psFatCache->iDirty = 0;
    }
    FATCluEntry = MAKEFATUINT32(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset+1],
                                                     psFatCache->szSecBuf[ThisFATEntOffset+2], psFatCache->szSecBuf[ThisFATEntOffset+3]);
    if (FATCluEntry == cur_clus)
    {
        D((DL_FATERROR, "exfat_get_next_cluster: next-cluster(0x%x) == current-cluster(0x%x).", FATCluEntry, cur_clus));
        FATCluEntry = 0xFFFFFFFF;
    }
    *next_clus = FATCluEntry;

    iRet = _ERR_FAT_SUCCESS;

end:

    return iRet;
}

#if 0
INT32 exfat_get_next_cluster(struct inode *pinode, UINT32 cur_clus, UINT32 *next_clus)
{
    UINT32 FATOffset;
    UINT32 ThisFATSecNum;
    UINT32 ThisFATEntOffset;
    UINT32 FATCluEntry = 0;
    FAT_CACHE *psFatCache = NULL;
    INT32 iResult;
    INT32 iRet;
    EXFAT_SB_INFO *sb_info;
    EXFAT_INODE_INFO* inode_info;

    sb_info = &pinode->i_sb->u.exfat_sb;
    inode_info = &pinode->u.exfat_i;
    if(!EXFAT_IS_VALID_CLUSTER(sb_info,cur_clus) ||
        !EXFAT_GENSECON_FLAGS_POSSIBLE(inode_info->ext_entry.GenSeconFlags) )
    {
        D((DL_FATERROR, "exfat_get_next_cluster: iPreClu = 0x%x, _ERR_FAT_PARAM_ERROR!\n", cur_clus));
        return _ERR_FAT_ERROR;
    }

    iRet = _ERR_FAT_SUCCESS;
    if(EXFAT_GENSECON_FLAGS_NOFATCHAIN(inode_info->ext_entry.GenSeconFlags))
    {
        if(((cur_clus - inode_info->ext_entry.FirstCluster)*
             (sb_info->SectorsPerCluster*sb_info->BytesPerSector))
             >  inode_info->ext_entry.ValidDataLength)
        {
             *next_clus = EXFAT_FAT_VALUE_EOF;
        }
        else
        {
            *next_clus = cur_clus + 1;
        }
    }
    else
    {
        psFatCache = fat_get_fat_cache(sb_info->sb->s_dev);
        FATOffset = cur_clus*4;
        ThisFATSecNum = EXFAT_GET_FAT_SEC_NUM(sb_info,FATOffset);
        ThisFATEntOffset = EXFAT_GET_FAT_ENT_OFFS(sb_info,FATOffset);
        if (psFatCache->iSecNum != ThisFATSecNum)
        {
            if (psFatCache->iSecNum != 0)
            {
                iResult = DRV_BLOCK_WRITE( sb_info->sb->s_dev, psFatCache->iSecNum , psFatCache->szSecBuf);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    psFatCache->iSecNum = 0;
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    D((DL_FATERROR, "exfat_get_next_cluster(), write block failed!iSecNum = 0x%x,iResult = %d\n",psFatCache->iSecNum,iResult));
                    goto end;
                }
            }

            iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, ThisFATSecNum, psFatCache->szSecBuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                psFatCache->iSecNum = 0;
                iRet = _ERR_FAT_READ_SEC_FAILED;
                D((DL_FATERROR, "exfat_get_next_cluster(), read block failed!ThisFATSecNum = 0x%x, iResult = %d\n",ThisFATSecNum, iResult));
                goto end;
            }

            psFatCache->iSecNum = ThisFATSecNum;
            psFatCache->iDirty = 0;
        }
        FATCluEntry = MAKEFATUINT32(psFatCache->szSecBuf[ThisFATEntOffset], psFatCache->szSecBuf[ThisFATEntOffset+1],
                                                         psFatCache->szSecBuf[ThisFATEntOffset+2], psFatCache->szSecBuf[ThisFATEntOffset+3]);
        if (FATCluEntry == cur_clus)
        {
            D((DL_FATERROR, "exfat_get_next_cluster: next-cluster(0x%x) == current-cluster(0x%x).", FATCluEntry, cur_clus));
            FATCluEntry = 0xFFFFFFFF;
        }
        *next_clus = FATCluEntry;
    }


    iRet = _ERR_FAT_SUCCESS;

end:

    return iRet;
}
#endif

INT32 exfat_get_free_cluster(EXFAT_SB_INFO *sb_info, UINT32 iCurClu, UINT32 *pFreeClu)
{
    UINT32 SecNum = 0;
    UINT32 ByteOffs = 0;
    UINT32 BitOffs = 0;
    UINT32 BegClus;
    UINT8 IsFirst = 1;
    INT32 iResult = _ERR_FAT_SUCCESS;
    INT32 iRet = _ERR_FAT_SUCCESS;
    UINT8 cheat = 1;
    EXFAT_CACHE *psBATCache = NULL;

    // If Free cluster count is 0, return.
    *pFreeClu = 0;
    if(100 == sb_info->PercentInUse)
    {
        D((DL_FATERROR, "in exfat_get_free_cluster, disk is full 1,s_dev = %d.", sb_info->sb->s_dev));
        return  _ERR_FAT_DISK_FULL;
    }

    // Get fat cache.
    psBATCache = &sb_info->BATCache;

    BegClus = iCurClu;

    // Find free cluster and next free cluster.
    do
    {
        SecNum = EXFAT_BAT_SECTOR(sb_info,iCurClu);
        ByteOffs = EXFAT_BAT_BYTEOFFS(sb_info,iCurClu);
        BitOffs = EXFAT_BAT_BITOFFS(sb_info,iCurClu);

        // Read block to BAT cache buffer.
        if(psBATCache->iSecNum != SecNum)
        {
            // Exchange fat fat cache block.
            if(IsFirst == 1)
            {
                iResult = exfat_synch_cache(sb_info, psBATCache);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    D( ( DL_FATERROR, "in exfat_get_free_cluster,fat_synch_fat_cache()2 return = %d.\n",iResult) );
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    break;
                }
                IsFirst = 0;
            }
            iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, SecNum, psBATCache->szSecBuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D((DL_FATERROR, "DRV_BLOCK_READ failed3, dev = %d, FATSecNum = %d, iResult = %d, goto end.\n",
                   sb_info->sb->s_dev, SecNum, iResult));
                psBATCache->iSecNum = 0;
                iRet = _ERR_FAT_READ_SEC_FAILED;
                break;
            }
            psBATCache->iSecNum = SecNum;
            psBATCache->iDirty = 0;
        }
        if((psBATCache->szSecBuf[ByteOffs] & (UINT8)(0x1 << BitOffs)) == 0)
        {
            sb_info->CurCluster = iCurClu;
            *pFreeClu = iCurClu;
            iRet = ERR_SUCCESS;
            break;
        }

        iCurClu = (iCurClu + 1) < (sb_info->ClusterCount  + 2) ? (iCurClu+1):2;

        if(iCurClu == BegClus)
        {
            D((DL_FATDETAIL,"iCurClus(%d) = iPreClu, disk if full.break.\n",iCurClu));
            iRet =  _ERR_FAT_DISK_FULL;
            break;
        }
    }while(cheat);
    return iRet;
}


INT32 exfat_stat_free_cluster(EXFAT_SB_INFO *sb_info, UINT32 *piFreeCluNum)
{
    UINT32 BATBaseSector = 0;
    UINT32 SecNum = 0;
    UINT32 ByteOffs = 0;
    UINT32 BitOffs = 0;
    UINT32 iCurClu = 2;
    UINT32 iFreeClusterNum = 0;
    INT32 iResult = _ERR_FAT_SUCCESS;
    INT32 iRet = _ERR_FAT_SUCCESS;
    INT32 cheat = 1;
    EXFAT_CACHE *psBATCache = NULL;


    // If Free cluster count is 0, return.
    *piFreeCluNum = 0;
    if(100 == sb_info->PercentInUse)
    {
        D((DL_FATERROR, "in exfat_get_free_cluster, disk is full 1,s_dev = %d.", sb_info->sb->s_dev));
        return  _ERR_FAT_DISK_FULL;
    }

    // Get fat cache.
    psBATCache = &sb_info->BATCache;

    // Find free cluster and next free cluster.
    BATBaseSector = sb_info->PartitionOffset  + sb_info->ClusterHeapOffset + (sb_info->BATCluster - 2)*sb_info->SectorsPerCluster;
    // BATSectorCount = (sb_info->ClusterCount + (8 *512 -1))  / (8 *512)
    SecNum = BATBaseSector;
    do
    {
        // Read block to BAT cache buffer.
        if(psBATCache->iSecNum != SecNum)
        {
            // Exchange fat fat cache block.
            if(psBATCache->iDirty == 1)
            {
                iResult = exfat_synch_cache(sb_info, psBATCache);
                if(_ERR_FAT_SUCCESS != iResult)
                {
                    D( ( DL_FATERROR, "in exfat_get_free_cluster,fat_synch_fat_cache()2 return = %d.\n",iResult) );
                    iRet = _ERR_FAT_READ_SEC_FAILED;
                    break;
                }
            }
            iResult = DRV_BLOCK_READ( sb_info->sb->s_dev, SecNum, psBATCache->szSecBuf);
            if(_ERR_FAT_SUCCESS != iResult)
            {
                D((DL_FATERROR, "DRV_BLOCK_READ failed3, dev = %d, FATSecNum = %d, iResult = %d, goto end.\n",
                   sb_info->sb->s_dev, SecNum, iResult));
                psBATCache->iSecNum = 0;
                iRet = _ERR_FAT_READ_SEC_FAILED;
                break;
            }
            psBATCache->iSecNum = SecNum;
            psBATCache->iDirty = 0;
        }
        for(ByteOffs = 0; ByteOffs < EXFAT_DEFAULT_SECSIZE;ByteOffs ++)
        {
            for(BitOffs = 0; BitOffs < 8; BitOffs ++)
            {
                if(iCurClu >= sb_info->ClusterCount  + 2)
                {
                     *piFreeCluNum = iFreeClusterNum;
                     iRet = ERR_SUCCESS;
                     goto _func_end;
                }
                iFreeClusterNum += (psBATCache->szSecBuf[ByteOffs] & (0x1 << BitOffs)) == 0 ? 1:0;
                iCurClu  ++;
            }
        }
        SecNum ++;
    }while(cheat);

 _func_end:
    return iRet;
}


UINT32 exfat_fpos2cluster(struct inode *pinode,UINT64 iPos)
{
    EXFAT_SB_INFO* sb_info = NULL;
    UINT32 cur_clus = 0;
    UINT32 next_clus = 0;
    //EXFAT_EXT_DIR_ENTRY* ext_entry = NULL;
    UINT32 clus_offs =0;
    UINT32 clus_ind = 0;
    INT32 result;
    struct exfat_inode_info* exfat_inode_info;

    sb_info = &(pinode->i_sb->u.exfat_sb);
    exfat_inode_info = &pinode->u.exfat_i;
    clus_offs =  (UINT32)iPos/(sb_info->SectorsPerCluster*sb_info->BytesPerSector);
    if(exfat_inode_info->ClusIndx <= clus_offs)
    {
        clus_ind = exfat_inode_info->ClusIndx;
        cur_clus = exfat_inode_info->CurClus;
    }
    else
    {
        clus_ind = 0;
        cur_clus = exfat_inode_info->ext_entry.FirstCluster;
    }
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:clus_ind = 0x%x,clus_offs = 0x%x,cur_clus = 0x%x.",clus_ind,clus_offs,cur_clus);
    // if cluster chain exist fragment.
    while(EXFAT_IS_VALID_CLUSTER(sb_info, cur_clus))
    {
        CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:clus_ind = 0x%x,cur_clus = 0x%x",clus_ind,cur_clus);
        if(!EXFAT_GENSECON_FLAGS_POSSIBLE(exfat_inode_info->ext_entry.GenSeconFlags))
        {
            D( ( DL_FATERROR, "in exfat_fpos2cluster GenSeconFlags = 0x%x.\n",exfat_inode_info->ext_entry.GenSeconFlags) );
            break;
        }

        if(clus_offs == clus_ind)
        {
           exfat_inode_info->ClusIndx = clus_ind;
           exfat_inode_info->CurClus = cur_clus;
           CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:OK,cur_clus = 0x%x",cur_clus);
           return cur_clus;
        }
        result = exfat_get_next_cluster(pinode, cur_clus,&next_clus);
        if(ERR_SUCCESS != result)
        {
            D( ( DL_FATERROR, "in exfat_fpos2cluster,exfat_get_next_cluster() return = %d.\n",result) );
            break;
        }
        cur_clus = next_clus;
        clus_ind ++;
    }
    CSW_TRACE(BASE_DSM_TS_ID, "EXFAT_DEBUG:ERROR,cur_clus = 0x%x,clus_ind = 0x%x.",cur_clus,clus_ind);
    return 0;
}


INT32 exfat_fpos2CSO(struct inode *inode, UINT64 iPos, UINT32 *piClu, UINT32 *piSec, UINT32 *piOff)
{
    EXFAT_SB_INFO* sb = NULL;
    UINT16 secperclu = 0;

    sb = &(inode->i_sb->u.exfat_sb);
    secperclu = sb->SectorsPerCluster;
    *piClu = exfat_fpos2cluster(inode, iPos); //(iPos / (secperclu * DEFAULT_SECSIZE));
    if (0 == *piClu)
    {
        D( ( DL_FATERROR, "exfat_fpos2CSO: exfat_fpos2cluster = 0.\n") );
        return _ERR_FAT_NOT_MATCH_CLUSTER;
    }
    *piSec = (UINT32)((iPos / DEFAULT_SECSIZE) % secperclu);
    *piOff = (UINT32)(iPos % DEFAULT_SECSIZE);
    return _ERR_FAT_SUCCESS;
}

VOID exfat_SetPos(struct inode *inode, UINT32 iPos, UINT32 iClu)
{
    EXFAT_SB_INFO* sb = NULL;
    UINT32 clus_index;

    sb = &(inode->i_sb->u.exfat_sb);
    clus_index = iPos / (sb->SectorsPerCluster * DEFAULT_SECSIZE);
    inode->u.exfat_i.CurClus = iClu;
    inode->u.exfat_i.ClusIndx = clus_index;
  }

UINT16 exfat_GetEntryCheckSum(UINT8* entry)
{
    UINT16 checksum = 0;
    UINT32 len,i;

    len = 32*(entry[1] + 1);
    for(i = 0; i < len; i++)
    {
        if(i == 2 ||i == 3 )
        {
            continue;
        }
        checksum = (UINT16)(((checksum << 15) | (checksum >> 1)) + entry[i]);
    }
    return checksum;
}

extern struct super_block* g_fs_super_block_table;
extern struct fs_cfg_info g_fs_cfg;
EXFAT_CACHE* exfat_get_fat_cache(UINT32 iDevNo)
{
    UINT32 i;
    EXFAT_SB_INFO* sb;

    for(i = 0 ; i < g_fs_cfg.uNrSuperBlock; i++)
    {
        if(g_fs_super_block_table->s_dev == iDevNo)
        {
		sb = &g_fs_super_block_table->u.exfat_sb;
		return &sb->BATCache;
        }
    }
    return NULL;
}

#endif


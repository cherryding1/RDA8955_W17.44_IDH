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

#ifndef _VDS_CD_H_
#define _VDS_CD_H_


#ifdef _FS_SIMULATOR_
#define vds_malloc( size )        malloc(size)
#define vds_free( p )             free(p)
#else
#define vds_malloc( size )        CSW_MALLOC(BASE_VDS_TS_ID, size)
#define vds_free( p )             CSW_FREE( BASE_VDS_TS_ID, p )
#endif


// Sector magic number, babe face
#define VDS_MAGIC 0xBABEFACE

// Sector remain magic.  set the sector to remain sector before erase it.
#define VDS_ERASING_MAGIC 0xB0B0F0C0

// PB Status
#define VDS_STATUS_PB_FREE    0xFFFF
#define VDS_STATUS_PB_VALID   0xFF00
#define VDS_STATUS_PB_INVALID 0x0000

// Sector Status, cafe beef
#define VDS_STATUS_SECT_FREE          0xCAFEBEEF
#define VDS_STATUS_SECT_REMAIN        0xCAFEBEE0
#define VDS_STATUS_SECT_TEMP          0xCAFEBE00
#define VDS_STATUS_SECT_CURRENT       0xCAFEB000
#define VDS_STATUS_SECT_DIRTY         0xCAFE0000
#define VDS_STATUS_SECT_GCING         0xCA000000
#define VDS_STATUS_SECT_ERASING       0x00000000


// VDS_PBD struct
// field position and size.
#define VDS_POS_PBD_STATUS   0x00
#define VDS_POS_PBD_VBN      0x02
#define VDS_POS_PBD_PBDCRC   0x04
#define VDS_POS_PBD_PBCRC    0x08

#define VDS_SZ_PBD_STATUS    (VDS_POS_PBD_VBN    - VDS_POS_PBD_STATUS)
#define VDS_SZ_PBD_VBN       (VDS_POS_PBD_PBDCRC - VDS_POS_PBD_VBN)
#define VDS_SZ_PBD_PBDCRC    (VDS_POS_PBD_PBCRC  - VDS_POS_PBD_PBDCRC)
#define VDS_SZ_PBD_PBCRC     0x04

// VDS_SH struct
// field position and size.
#define VDS_POS_SH_STATUS       0x04
#define VDS_SZ_SH_STATUS        0x04

// SH size.
#define VDS_SZ_SH sizeof(VDS_SH)

// PBD size.
#define VDS_SZ_PBD sizeof(VDS_PBD)

// PB size.
#define VDS_SZ_PB(part_pointer)  (part_pointer->sPartInfo.iVBSize)

// The count of per group.
#define VDS_NR_PBD_PER_GROUP        8

// The max number of partition.
#ifdef MEM_ULC_3216_SAVE_SRAM
#define VDS_NR_PART_MAX             6
#else
#define VDS_NR_PART_MAX             10
#endif

// Sector number on the partiton.
#define VDS_NR_SECT(part_pointer) \
    (part_pointer->sPartInfo.iFlashSectorCount)

// PB number on per sector.
// arithmetic discription:
// SecSize = HeaderSize + PBDTSize + PBLSize + SpareSize;        (1)
// PBDTSize = PBCount*PBDSize;                                   (2)
// PBLSzie = PBCount*PBSize;                                     (3)
// (1),(2),(3)==>
// SecSize = HeaderSize + PBCount*(PBDSize + PBSize) + SpareSize (4)

//  0 <= SpareSize <= BlkSize - PBDSize
// as BlkSize equal to PBSize
// so 0 <= SpareSize <= PBSize - PBDSize
// Set SpareSize = PBSize - PBDSize                              (5)
// (4),(5) ==>
// PBCount = (SecSize - HeaderSize - (PBSize - PBDSize))/(PBDSize + PBSize);  (6)
// Sometime,PBDT Rogion has a block is otiose.
/*
#define VDS_NR_PB_PER_SECT(part_pointer) \
    ((part_pointer->sPartInfo.iFlashSectorSize - VDS_SZ_SH -(VDS_SZ_PB(part_pointer) -VDS_SZ_PBD ))/(VDS_SZ_PBD + VDS_SZ_PB(part_pointer)))

*/
#define VDS_NR_PB_PER_SECT(part_pointer) \
((part_pointer->sPartInfo.iFlashSectorSize - VDS_SZ_SH) / (VDS_SZ_PBD + VDS_SZ_PB(part_pointer)))

// The size of PBDT.
#define VDS_SZ_PBDT(part_pointer) \
    (VDS_NR_PB_PER_SECT(part_pointer) * VDS_SZ_PBD)

// The number of MT valid entry.
#define VDS_NR_MT_VALID_ENTRY(part_pointer) \
    (( VDS_NR_PB_PER_SECT(part_pointer) * (VDS_NR_SECT(part_pointer))) \
      - part_pointer->sPartInfo.iRsvBlkCount)

// The size of MT.
#define VDS_SZ_MT(part_pointer) \
    (VDS_NR_MT_VALID_ENTRY(part_pointer) * sizeof(VDS_PB_REF) )

// Sector address.
#define VDS_ADDR_SECT(sect_no) \
    ( g_sFlashInfo.iFlashStartAddr\
     + g_sFlashInfo.iFlashSectorSize *(sect_no))

// PBDT address.
#define VDS_ADDR_PBDT(part_pointer, sect_no) \
    (VDS_ADDR_SECT(sect_no) + VDS_SZ_SH)

// The first PB address.
#define VDS_ADDR_PB(part_pointer, sect_no) \
    (((VDS_ADDR_PBDT(part_pointer, sect_no) + VDS_SZ_PBDT(part_pointer) + (VDS_SZ_PB(part_pointer) -VDS_SZ_PBD ))/VDS_SZ_PB(part_pointer))*VDS_SZ_PB(part_pointer))

// Get sector address depend on sector index.
#define VDS_SECT_INDEX_TO_ADDR( index ,flash_info) \
    (flash_info.iFlashStartAddr + index * flash_info.iFlashSectorSize )

// Original sector index.
#define VDS_SECT_NONE            0xFFFF

// Original block index.
#define VDS_BLOCK_NONE           0xFFFF

// Invalid pointer.
#define VDS_INVALID_POINTER      0xFFFFFFFF

// Original PB data.
#define VDS_PB_BASE_DATA         0xff


// Check flag.
// To check.
#define VDS_FLAG_CHECK            1

// Not to check.
#define VDS_FLAG_NOT_CHECK        0

// Original MT value.
#define VDS_VALUE_MT_INIT         0xff

// MT SN value is free.
#define VDS_MT_SN_FREE            0xffff

// MT PBN value is free.
#define VDS_MT_PBN_FREE           0xffff

// Default block size.
#define VDS_BLOCK_SIZE             512


// Local error code.

// Parameter error.
#define _ERR_VDS_PARAM_ERROR                    -101

// VDS partition config error.
#define _ERR_VDS_CONFIG_ERROR                   -102

// Get flash information error.
#define _ERR_VDS_FLASH_INFO_ERROR               -103

// Remain sector is null.
#define _ERR_VDS_REM_SECT_NULL                  -104

// Dirty list is null.
#define _ERR_VDS_DIRTY_LIST_NULL                -105

// Free list is null.
#define _ERR_VDS_FREE_LIST_NULL                 -106

// Remain sector has existed.
#define _ERR_VDS_REM_SEC_EXIST                  -107

// Partition space is full.
#define _ERR_VDS_PARTITION_FULL                 -108

//  No more valid PBD on sector.
#define _ERR_VDS_NO_MORE_PBD                     -109

// Write check error.
#define _ERR_VDS_WRITE_CHECK_ERROR               -110

// Setup sector status failed.
#define _ERR_VDS_CHANGE_STATUS_FAILED            -111

// Read SH failed.
#define _ERR_VDS_READ_SH_FAILED                  -112

// Write SH failed.
#define _ERR_VDS_WRITE_SH_FAILED                 -113

// Update MT failed.
#define _ERR_VDS_UPDATE_MT_FAILED                -114

// GC failed.
#define _ERR_VDS_GC_FAILED                       -115

// Scan sector failed.
#define _ERR_VDS_SCAN_SECTOR_FAILED              -116

// Current position error.
#define _ERR_VDS_CURRENT_POS_ERROR               -117

// Erase flash failed.
#define _ERR_VDS_ERASE_FLASH_FAILED              -118

// Read flash failed.
#define _ERR_VDS_READ_FLASH_FAILED               -119

// Wrtie flash failed.
#define _ERR_VDS_WRITE_FLASH_FAILED              -120

// No more memory.
#define _ERR_VDS_NO_MORE_MEM                          -121

// No more memory.
#define _ERR_VDS_NO_MORE_SECT                          -122

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#ifndef __PRJ_WITH_SPIFLSH__

#if (NUMBER_OF_SIM > 3)

#define VDS_CACHE_ERASE_TIME_MAX                    24

#elif (NUMBER_OF_SIM > 2)

#define VDS_CACHE_ERASE_TIME_MAX                    64


#else

#define VDS_CACHE_ERASE_TIME_MAX                    150

#endif

#else
extern UINT32 memd_FlashErase2SuspendTime(VOID);
#define VDS_CACHE_ERASE_TIME_MAX                    memd_FlashErase2SuspendTime()

#endif
#if (NUMBER_OF_SIM > 3)
#define VDS_CACHE_FLUSH_NODE_NUMBER             2
#elif (NUMBER_OF_SIM > 2)
#define VDS_CACHE_FLUSH_NODE_NUMBER             4
#elif(NUMBER_OF_SIM > 1)
#define VDS_CACHE_FLUSH_NODE_NUMBER             8
#else
#define VDS_CACHE_FLUSH_NODE_NUMBER             16
#endif

// 1 frams  need 4.6 ms. -->

#define VDS_CACHE_FLUSH_REMAIN_FRAMES_NUMBER (VDS_CACHE_ERASE_TIME_MAX/4 + 1)

#define VDS_CACHE_NODE_NUMBER_MAX                  400

// Gc error code.
#define _ERR_VDS_GC_INIT                                      -200
#define _ERR_VDS_GC_GET_REMAIN                        -201
#define _ERR_VDS_GC_COPY                                     -202
#define _ERR_VDS_GC_COPY_COMPLETE                   -203
#define _ERR_VDS_GC_SETMT                                   -204
#define _ERR_VDS_GC_ERASE                                   -205
#define _ERR_VDS_GC_EREASE_SUSPEND                 -206                    // Erase flash suspend.
#define _ERR_VDS_GC_EREASE_FINISH                    -207                    // Erase flash resume.
#define _ERR_VDS_GC_FINISH                                  -208                    // Erase flash finish. 
#define _ERR_VDS_GC_DEACTIVE                              -210
#define _ERR_VDS_GC_EREASE_BUSY                       -211                    // Erase flash busy.

#endif

#endif


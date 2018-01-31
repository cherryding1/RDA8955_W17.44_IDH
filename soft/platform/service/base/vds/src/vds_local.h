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

#ifndef _VDS_LOCAL_H_
#define _VDS_LOCAL_H_

// VDS layout version.
#define VDS_LAYOUT_VERSION   0x20171107


// VDS partition information structure.
// all of sectors on the partition is uniform.
typedef struct vds_partition_info
{
    UINT8  acPartName[ 16];               // partition name.
    UINT32 iFlashSectorSize;              // sector size.
    UINT32 iFlashSectorCount;           // sector count on the partition.
    UINT32 iVBSize;                             // VB size.
    DSM_DEV_TYPE eDevType;            // Device type
    DSM_MODULE_ID eModuleId;         // Module id
    DSM_CHECK_LEVEL eCheckLevel;  // Checkout level
    UINT32 iRsvBlkCount;                   // Reserved block count.
} VDS_PARTITION_INFO;


// VDS flash information structure.
typedef  struct vds_flash_info
{
    INT32  bHasInit;                    // the flag of has initialized.
    UINT32 iFlashStartAddr;       // flash start address.
    UINT32 iFlashSectorSize;     // sector size.
    UINT32 iNrSector;                // sector number.
}
VDS_FLASH_INFO;


// VDS SH(Sector Header) structure.
typedef struct _vds_sh
{
    UINT32 iMagic;              // magic number.
    UINT32 iSectorStatus;              // status.
    UINT32 iEraseCounter;           // counter for the erase operation
    UINT32 iLayoutVer;                 // VDS Layout version.
    UINT32 iFlashSecCount;           // sector count pertain to partition.
    UINT32 iFlashSectorSize;         // sector size.
    UINT32 iVBSize;                        // VB size.
    UINT32 iReservedBlock;           // reserved block count.
    UINT8  acPartName[16];           // partition name.
    UINT32 iCRC;                          // CRC for sector header
    UINT8  reserved[ 16 ];          // reseved field.
} VDS_SH;

// PBD(Physical Block Descriptor) structure.
typedef struct _vds_pbd
{
    UINT16 iStatus;              // PBD(Physical Block Dscription) status.
    UINT16 iVBN;                        // VB(Virtual Block) number.
    UINT32 iVersion;                    // write version.
    UINT32 iPBDCRC;              // CRC for PBD.
    UINT32 iPBCRC;                      // CRC for PB.
} VDS_PBD;

//PB reference structure, also MT(Map Table) entry structure.
typedef struct _vds_pb_ref
{
    UINT16 iSN;                    // sector number
    UINT16 iPBN;                          // physical block number.
} VDS_PB_REF;

// Sector reference structure.
typedef struct vds_sect_ref
{
    UINT16 iSN;                         // sector number.
    UINT16 iCurrentPBN;           // current PB number,only used writing.
    struct vds_sect_ref *next;   // the next reference.
} VDS_SECT_REF;


// VDS partition structure.
typedef struct vds_partition
{
    UINT32 iRefCount;                          // sec ref count.
    VDS_PARTITION_INFO sPartInfo;    // partition information
    VDS_PB_REF *psMT;                       // Map table.
    VDS_SECT_REF *pDirtyList;            // Dirty list
    VDS_SECT_REF *pFreeList;            // Current sector list.
    VDS_SECT_REF *pCurrentSect;      // Current sector.
    VDS_SECT_REF *pGcingSect;         // GC Sector.
    VDS_SECT_REF *pTempSect;         // temporary Sector,copy the valid block to it in period of gcing .
} VDS_PARTITION;


// Function declare.
INT32 vds_Gc(
    VDS_PARTITION *psPart
);
INT32 vds_SetSectStatus(
    UINT16 iSN,
    UINT32 iStatus
);
INT32 vds_CountBlock(
    VDS_PARTITION *psPart,
    VDS_SECT_REF *psRef,
    UINT16 iBegIndex,
    UINT16 iStatus
);
INT32 vds_GetFirstBlockPos(
    VDS_PARTITION *psPart,
    VDS_SECT_REF *psRef,
    UINT16 iBlockStatus,
    INT16 *pPBN
);
INT32 vds_SetPBStatus(
    VDS_PARTITION *psPart,
    UINT16 iSN,
    UINT16 iPBN,
    UINT16 iStatus
);
INT32 vds_SetMT(
    VDS_PARTITION *psCP,
    VDS_PBD *pPBD,
    UINT16 iSectNo,
    UINT16 iPBN
);
INT32 vds_SetMTByScanSec(
    VDS_PARTITION *psCP,
    UINT16 iSectNo,
    UINT16 *iNrBlkScaaned
);

INT32 vds_ChangeSectStatus(
    VDS_PARTITION *psPart,
    VDS_SECT_REF *psSect,
    UINT32 iOldStatus,
    UINT32 iNewStatus
);

INT32 vds_MoveCurrentPos(
    VDS_PARTITION *psVDSPart
);
INT32 vds_GetCurrentPos(
    VDS_PARTITION *psCP,
    UINT16 *pSN,
    UINT16 *pPBN
);
INT32 vds_Pop(
    VDS_PARTITION *psVDSPart,
    VDS_SECT_REF **list
);
INT32 vds_ReadPBD(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    UINT16 iPBN,
    VDS_PBD *pPBD
);
INT32 vds_WritePBD(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    UINT16 iPBN,
    VDS_PBD *pPBD
);
INT32 vds_ReadPB(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    UINT16 iPBN,
    UINT8 *pBuff
);
INT32 vds_WritePB(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    UINT16 iPBN,
    UINT8 *pBuff
);
INT32 vds_ReadPBDT(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    UINT16 iBegIndex,
    UINT16 iNum,
    VOID *pBuf
) ;
INT32 vds_ReadSHWithCrc(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    VDS_SH *psSH
);
INT32 vds_ReadSH(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    VDS_SH *psSH
);
INT32 vds_WriteSH(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    VDS_SH *psSH
);
INT32 vds_WritePBDAndPB(
    VDS_PARTITION *psCP,
    UINT16 iSN,
    UINT16 iPBN,
    VDS_PBD *psPBD,
    UINT8 *pPB
);
INT32 vds_ListScan(
    VDS_PARTITION *psCP,
    UINT32 uStatus
);
UINT32 vds_GetShCrc(
    VDS_SH sh
);
INT32 vds_InitEntry(
    VOID
);
BOOL vds_IsValidSn(
    VDS_PARTITION *psCP,
    UINT16 iSn
);
BOOL vds_IsSecRefListEmpty(
    VDS_SECT_REF *list
);
VDS_SECT_REF *vds_AllocSecRef(
    VOID
);
VDS_SECT_REF  *vds_GetRemainSecRef(
    VOID
);
VDS_SECT_REF *vds_AddSecRef(
    VDS_SECT_REF **list,
    VDS_SECT_REF *psRef
);
VDS_SECT_REF *vds_AppendSecRef(
    VDS_SECT_REF **list,
    VDS_SECT_REF *psRef
);
VDS_SECT_REF *vds_RemoveSecRef(
    VDS_SECT_REF **list,
    VDS_SECT_REF *psRef
);
VDS_SECT_REF *vds_GetFirstSecRef(
    VDS_SECT_REF *list
);
VDS_SECT_REF *vds_GetNextSecRef(
    VDS_SECT_REF *list,
    VDS_SECT_REF *psRef
);
VDS_SECT_REF *vds_PopFistSecRef(
    VDS_SECT_REF **list
);



INT32 vds_EraseSec(
    UINT32 iSecAddr
);
INT32 vds_FormatSec(
    UINT32 iSecAddr,
    VDS_PARTITION_INFO sPartInfo
);
INT32 vds_EraseAndFormatSec(
    UINT32 iSecAddr,
    VDS_PARTITION_INFO sPartInfo
);
INT32 VDS_GetFlashInfo(
    VDS_FLASH_INFO *psInfo
);
INT32 vds_SetAllSectorsEraseFlag(
    VOID
);

VOID VDS_ModuleSemInit(
    VOID
);
VOID VDS_WaitForSemaphore(
    VOID
);
VOID VDS_ReleaseSemaphore(
    VOID
);

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

// VDS Gc status enum for vds cache.
typedef enum
{
    VDS_GC_IDLE,                                  // idle.
    VDS_GC_INIT,                                  // init
    VDS_GC_GET_REMAIN,                    // get remain sector
    VDS_GC_COPY,                                // copying
    VDS_GC_SETMT,                              // set MT
    VDS_GC_ERASE_BEGIN,                  // erase temp sector being.
    VDS_GC_ERASE_SUSPEND,             // erase suspend.
    VDS_GC_ERASE_BUSY,                    // erase busy.
    VDS_GC_ERASE_FINISH,                // erase temp sector finish.
    VDS_GC_FINISH,                             // gc finish.
} VDS_GC_STATUS;

// vds gc scene structure for vds cache.
typedef struct  _vds_gc_scene
{
    BOOL bIsActive;                        // active:0 deactive;1:active
    VDS_GC_STATUS eStatus;         // gc status.
    VDS_SECT_REF *psGcSect;       // point to the gcing-sector reference.
    VDS_SECT_REF *psRemSect;    // point to the remain(or temp)-sector reference.
    VDS_PARTITION *psPart;          // VDS Partition information.
    UINT32 iEraseCounter;             // sector erase counter.
    UINT8 *pGcBuff;                       // point to a buffer, size = GROUP_NR*VB_SIZE.
    UINT32 iIndex;                         // check position(index) in gc-sector.
    UINT32 iSuspendTime;            // The last suspend time.
} VDS_GC_SCENE;


// function define for vds cache.
VOID vds_SetFlushAllowed(
    VOID
);
VOID vds_SetFlushUnallowed(
    VOID
);
BOOL vds_IsFlushAllowed(
    VOID
);
VOID vds_DisableFlush(
    UINT32 keep_time
);
VOID vds_EnableFlush(
    VOID
);
VOID vds_InitCache(
    VOID
);
BOOL vds_IsCacheInited(
    VOID
);
VOID vds_GcActive(
    VDS_PARTITION *psPart
);
BOOL vds_GcIsActive(
    VOID
);
VOID vds_GcDeactive(
    VOID
);
INT32 vds_GcExt(
    UINT8 iIsSuspend
);
#endif

#endif // _VDS_LOCAL_H_






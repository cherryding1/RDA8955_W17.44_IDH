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


#ifndef _NVRAM_LOCAL_H_
#define _NVRAM_LOCAL_H_


#define NVRAM_MAGIC                                                0xaacc1122
#define NVRAM_VERSION                                            0x20111122

#define DSM_NVRAM_BASE_ADDR                               0

#define NVRAM_DATA_SIZE                                        496
#define NVRAM_BLOCK_SIZE                                       512

#define NVRAM_H_BASE_ADDR                                    0
#define NVRAM_H_SIZE                                              sizeof(NVRAM_H_INFO)
#define NVRAM_ND_SIZE                                            sizeof(NVRAM_ND_INFO)
#define NVRAM_R_SIZE                                               sizeof(NVRAM_R_INFO)
#define NVRAM_REF_SIZE                                          sizeof(NVRAM_REF)
#define NVRAM_INFO_SIZE                                        sizeof(NVRAM_INFO)
#define NVRAM_NDT_BASE_ADDR (((NVRAM_H_SIZE + NVRAM_R_SIZE -1)/NVRAM_R_SIZE)*NVRAM_R_SIZE)
#define NVRAM_NDT_SIZE            (((NVRAM_ENTRIES_COUNT*NVRAM_ND_SIZE + NVRAM_R_SIZE - 1)/NVRAM_R_SIZE)*NVRAM_R_SIZE)
#define NVRAM_R_BASE_ADDR     (((NVRAM_NDT_BASE_ADDR + NVRAM_NDT_SIZE +NVRAM_BLOCK_SIZE-1)/NVRAM_BLOCK_SIZE)*NVRAM_BLOCK_SIZE)

#define NVRAM_R_COUNT             ((nvram_GetDevSize() - NVRAM_R_BASE_ADDR)/NVRAM_R_SIZE)

#define NVRAM_NR_COUNT(size)          ((size + NVRAM_DATA_SIZE - 1)/NVRAM_DATA_SIZE)

#define NVRAM_IS_VALID_ID(id) (id < NVRAM_INVALID_ID && id < NVRAM_FREE_ID)
#define NVRAM_IS_VALID_RN(rn) (rn < NVRAM_R_COUNT)
#define NVRAM_IS_VALID_NDN(ndn) (ndn < NVRAM_ENTRIES_COUNT)

#define NVRAM_STATUS_FREE        0x01
#define NVRAM_STATUS_INVALID   0x02

#define NVRAM_STATUS_USED       0x04
#define NVRAM_STATUS_VALID      0x08


#define NVRAM_INVALID_RN           NVRAM_R_COUNT
#define NVRAM_INVALID_NDN        NVRAM_ENTRIES_COUNT
#define NVRAM_INVALID_ID           0xfffffffe
#define NVRAM_FREE_ID                 0xffffffff
#define NVRAM_INVALID_IND         0xfffffffe
#define NVRAM_INVALID_CRC         0x0

#define NVRAM_DEFAULT_VALUE   0xff

#define NVRAM_GROUP_NR            16

#define NVRAM_INVALID_ND         0xffffffff

#define NDN_IS_EXIST                  1
#define NDN_NOT_EXIST               2

#define NVRAM_CHECKOUT_RESULT_OK       1
#define NVRAM_CHECKOUT_RESULT_ERR     0


typedef struct
{
    UINT32  iMagic ;                        // Magic number.
    UINT32  iVer;                             // version number.
    UINT32  iDevSize;                     // device size.
    UINT32  iHOffs;                          // header offset.
    UINT32  iHSize;                         // header size.
    UINT32  iNDOffs;                       //  record dscriptor offset.
    UINT32  iNDSize;                       // record dscriptor size.
    UINT32  iROffs;                         // record offset.
    UINT32  iRSize;                         // record size.
    UINT32  iRCount;                      // record count.
    UINT32  iCrc;                            // header information crc value.
    UINT8   szServed[20];              // served field.
} NVRAM_H_INFO;

typedef struct _nvram_nd_info
{
    UINT32 iId;
    UINT32 iSize;
} NVRAM_ND_INFO;

typedef struct _nvram_ref
{
    UINT32 iId;
    UINT32 iNext;
} NVRAM_REF;

typedef struct _nvram_info
{
    UINT32 iId;
    UINT32 iSize;
    UINT32 iFstRN;
} NVRAM_INFO;

typedef struct _nvram_r_ctr
{
    UINT32 iStatus;
    UINT32 iId;
    UINT32 iInd;
    UINT32 iCrc;
} NVRAM_R_CTR;

// The record information struct.
typedef struct _nvram_r_info
{
    NVRAM_R_CTR sCtr;
    UINT8 pData[NVRAM_DATA_SIZE];
} NVRAM_R_INFO;


typedef struct _nvram_global_var
{
    UINT32 iHasInited;                              // Nvram module initialized flag: 1 has initialized;0 not initialize.
    HANDLE hSem;                                     // Nvram module semaphore handle
    NVRAM_H_INFO  sNH;                           // Nvram header.
    NVRAM_INFO *psNInfo;
    NVRAM_ND_INFO *psNDT;
    NVRAM_REF *psRef;
} NVRAM_GLOBAL_VAL;

typedef enum
{
    NVRAM_OPT_ADD,
    NVRAM_OPT_SET_DEFAULT,
    NVRAM_OPT_RESET,
    NVRAM_OPT_DO_NOTHING,
} NVRAM_OPT;

BOOL nvram_Existed(VOID);

INT32 nvram_InitGlobal(VOID);

VOID nvram_CloseNE(VOID);

INT32 nvram_Format(VOID);

INT32 nvram_ScanNR(UINT32 iNDCount, NVRAM_ND_INFO *psNDT, UINT32 iNRCount, NVRAM_REF *pRef);

UINT32 nvram_GetDevSize(VOID);

INT32   nvram_GetUsedSize(UINT32 *piUsedSize);

INT32   nvram_GetEntries(NVRAM_ND_INFO *psEntries, UINT32 *piCount);

INT32   nvram_GetEntriesCount(UINT32 *piCount);

INT32 nvram_Read(UINT32 iAddrOffset, UINT8 *pBuffer, UINT32 iBytesToRead);

INT32 nvram_Write(UINT32 iAddrOffset, CONST UINT8 *pBuffer, UINT32 iBytesToWrite);

INT32 nvram_WriteNH(NVRAM_H_INFO sNH);

INT32 nvram_ReadNH(NVRAM_H_INFO *psNH);

INT32 nvram_AddND(NVRAM_ND_INFO sNDInfo);

INT32 nvram_SetNDE(NVRAM_ND_INFO sNDInfo, UINT32 iNDN);

INT32 nvram_DelNDE(UINT32 iNDN);

INT32 nvram_WriteR(NVRAM_R_INFO sRInfo, UINT32 iRN);

INT32 nvram_ReadR(NVRAM_R_INFO *psRInfo, UINT32 iRN);

INT32 nvram_DeleteR(UINT32 iRN);

INT32 nvram_WriteNV(NVRAM_INFO *psNInfo, UINT32 iId, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff);

INT32 nvram_ReadNV(NVRAM_INFO *psNInfo, UINT32 iId, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff);

INT32 nvram_GetNDE(UINT32 iId, NVRAM_ND_INFO *psNDInfo, UINT32 *piNDN);

INT32 nvram_GetFstRN(UINT32 iId, UINT32 *piFstRN);

INT32 nvram_GetNextRN(UINT32 iId, UINT32 iCurRN, UINT32 *piNextRN);

INT32 nvram_GetFreeRN(UINT32 iId, UINT32 iCurRN, UINT32 *piFreeRN);

INT32 nvram_GetRNCount(UINT32 iId, UINT32 *piRNCount);

BOOL nvram_CheckId(UINT32 iId, UINT32 iRNCount);

VOID nvram_SemInit(VOID);

VOID nvram_WaitForSem(VOID);

VOID nvram_ReleaseSem(VOID);

extern UINT32 vds_CRC32( CONST VOID *pvData, UINT32 iLen );
#define nvram_CRC32 vds_CRC32

#define _ERR_NVRAM_UNINIT                                                 -1007
#define _ERR_NVRAM_PARAM_ERROR                                      -1008
#define _ERR_NVRAM_MEDIA_READ_FAILED                          -1009
#define _ERR_NVRAM_MEDIA_WRITE_FAILED                        -1010
#define _ERR_NVRAM_GET_DEV_NR_FAILED                          -1011

#define _ERR_NVRAM_NO_MORE_ND                                       -1012
#define _ERR_NVRAM_NO_MORE_NR                                       -1013
#define _ERR_NVRAM_INVALID_ID                                          -1014
#define _ERR_NVRAM_INVALID_ND                                         -1015
#define _ERR_NVRAM_OFFS_OVERFLOW                                  -1016
#define _ERR_NVRAM_STATUS_ERROR                                     -1017
#define _ERR_NVRAM_CHECKOUT_ERROR                                 -1018
#define _ERR_NVRAM_MALLOC_FAILED                                   -1019
#define _ERR_NVRAM_OPEN_NVE_TOO_MORE                         -1020
#define _ERR_NVRAM_INVALID_HD                                         -1021
#define _ERR_NVRAM_GET_DEV_INFO_FAILED                       -1022
#define _ERR_NVRAM_ENTRY_IS_OPEN                                   -1023
#define _ERR_NVRAM_RN_ERROR                                             -1024
#define _ERR_NVRAM_CREATE_FAILED                                   -1025


#endif // end _NVRAM_LOCAL_H_


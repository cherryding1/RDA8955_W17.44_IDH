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











#ifndef _NVRAM_H_
#define _NVRAM_H_


#ifdef _FS_SIMULATOR_

#endif

#define NVRAM_OPEN_MAX                                                                         32
#define NVRAM_ENTRIES_COUNT                                                                512

//
// Data type
//
typedef struct _nvram_entry
{
    UINT32 iId;
    UINT32 iSize;
} NVRAM_ENTRY;

typedef enum
{
    NVRAM_PBK_ME_STORGE_LID,
#ifdef WIFI_SMARTCONFIG
    NVRAM_WIFI_SMARTCONFIG_LID,
#endif
    NVRAM_LAST_LID
} NVRAM_LID;


// Function define.
INT32 NVRAM_Init(VOID);

INT32 NVRAM_Format(void);

INT32 NVRAM_Create(CONST UINT32  iId, UINT32 iSize);

INT32 NVRAM_CreateEx(CONST UINT32  iId, UINT32 iSize, UINT8 iDefault);

INT32 NVRAM_Open(CONST UINT32  iId, BOOL bCheckout);

INT32 NVRAM_Close(INT32 hd);

BOOL NVRAM_IsExist(CONST UINT32 iId);

BOOL NVRAM_Check(CONST UINT32 iId, BOOL bCheckout);

INT32 NVRAM_Remove(CONST UINT32 iId);


UINT32 NVRAM_SetValue(INT32 hd, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff);

UINT32 NVRAM_GetValue(INT32 hd, UINT32 iOffs, UINT32 iSize, UINT8 *pBuff);

INT32 NVRAM_GetInfo(UINT32 *piTotalSize, UINT32 *piUsedSize);

UINT32 NVRAM_GetNESize(CONST UINT32 iId);

INT32 NVRAM_GetEntries(NVRAM_ENTRY *psEntries, UINT32 *piCount);

INT32 NVRAM_GetEntriesCount(UINT32 *piCount);

#define ERR_NVRAM_UNINIT                                                         -10007
#define ERR_NVRAM_PARAM_ERROR                                              -10008
#define ERR_NVRAM_DEV_READ_FAILED                                      -10009
#define ERR_NVRAM_DEV_WRITE_FAILED                                    -10010
#define ERR_NVRAM_DEV_GET_NR_FAILED                                  -10011

#define ERR_NVRAM_NO_MORE_ND                                               -10012
#define ERR_NVRAM_NO_MORE_NR                                               -10013
#define ERR_NVRAM_INVALID_ID                                                  -10014
#define ERR_NVRAM_INVALID_ND                                                 -10015
#define ERR_NVRAM_OFFS_OVERFLOW                                          -10016
#define ERR_NVRAM_STATUS_ERROR                                            -10017
#define ERR_NVRAM_CHECKOUT_ERROR                                        -10018
#define ERR_NVRAM_MALLOC_FAILED                                           -10019
#define ERR_NVRAM_OPEN_NVE_TOO_MORE                                 -10020
#define ERR_NVRAM_INVALID_HD                                                 -10021
#define ERR_NVRAM_GET_DEV_INFO_FAILED                               -10022
#define ERR_NVRAM_ENTRY_IS_OPEN                                           -10023
#define ERR_NVRAM_RN_ERROR                                                     -10024
#define ERR_NVRAM_CREATE_FAILED                                            -10025
#define ERR_NVRAM_NOT_OPEN                                                    -10026

#endif // for _NVRAM_H_


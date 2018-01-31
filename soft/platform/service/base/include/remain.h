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

#ifndef _REMAIN_H
#define _REMAIN_H
#define REMAIN_VERSION                    0x20150402

INT32 Remain_Init(void);
INT32 Remain_Write(UINT8 *buff, UINT16 timestamp, UINT32 size);
INT32 Remain_Read(UINT8 *buff, UINT16 *timestamp, UINT32 size);
INT32 Remain_Gc(BOOL suspend);
BOOL Remain_NeedGc(VOID);
BOOL Remain_GcIsActive(VOID);
BOOL Remain_GcIsSuspend(VOID);
// error code.
#define ERR_REMAIN_GET_SECTOR_ERROR             110001
#define ERR_REMAIN_SECTORS_IS_NULL                110002
#define ERR_REMAIN_SECTORS_FORMAT_FAILED   110003
#define ERR_REMAIN_MALLOC_FAILED                    110004
#define ERR_REMAIN_FREE_RECORD_IS_NULL        110005
#define ERR_REMAIN_REC_NOT_FOUND                  110006
#define ERR_REMAIN_WRITE_FLASH_FAILED         110007
#define ERR_REMAIN_READ_FLASH_FAILED           110008
#define ERR_REMAIN_ERASE_FLASH_FAILED         110009
#define ERR_REMAIN_SECTOR_CHECK_ERROR        110010
#define ERR_REMAIN_REC_STATE_FAILED             110011
#define ERR_REMAIN_REC_CHECKSUM_ERROR        110012
#define ERR_REMAIN_NOT_INIT                             110013
#endif


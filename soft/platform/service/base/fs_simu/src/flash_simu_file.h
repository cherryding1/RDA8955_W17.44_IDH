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

#if (defined _DSM_SIMUFLASH_FILE)

#ifndef _FLASH_SIMU_H_
#define _FLASH_SIMU_H_

// sumu flash driver error code.
#define ERR_DRV_INVALID_PARAMETER          1101
#define ERR_DRV_OPEN_DEV_FAILED            1102
#define ERR_DRV_DEV_NOT_OPEN               1103
#define ERR_DRV_DEV_HAS_OPEN               1104
#define ERR_DRV_DEV_TYPE_UNREGISTER        1105

#define ERR_FAILURE                        1106
#define ERR_DIRTY                          1107
#define ERR_VFLASH_WRITE_FAIL              1108
#define ERR_INVALID_PARAMETER              1109


#define OPEN_FLAG_FOR_FLASH (_O_CREAT | _O_BINARY | _O_RDWR)
#define MODE_FOR_FLASH (_S_IREAD | _S_IWRITE)
#define FLASH_SIMU_FILE "emu.fla"

#endif

#endif






















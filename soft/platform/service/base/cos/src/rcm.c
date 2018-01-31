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













#include <cswtype.h>
#include <errorcode.h>
#include <event.h>
#include <rcm.h>

typedef struct _RCM_FLASH_SECTOR_INFO
{
    UINT32 nSectorSize;
    UINT32 nSectorNum;
} RCM_FLASH_SECTOR_INFO;

typedef struct _RCM_FLASH_INFO
{
    RCM_FLASH_SECTOR_INFO nStartSec;
    RCM_FLASH_SECTOR_INFO nMidSec;
    RCM_FLASH_SECTOR_INFO nEndSec;
} RCM_FLASH_INFO;

/******************************************************************************/
/*              Copyright (C) 2003, Coolsand Technologies, Inc.               */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:  rcm.c                                                           */
/*                                                                            */
/* Description:                                                               */
/*   Prototypes for Resource configuration management functions and defines.  */
/*                                                                            */
/*   @SHK 2006-05-10.                                                         */
/******************************************************************************/

#include <cswtype.h>
#include <errorcode.h>
#include <event.h>
#include <rcm.h>
/*
//#include <csw_cfg.h>
typedef unsigned long       UINT32;
typedef signed long         s32;
typedef unsigned short      u16;
typedef signed short        s16;
typedef unsigned char       u8;
typedef signed char         s8;
#ifndef HAVE_CFG_TYPES
typedef unsigned long       UINT32;
#endif
*/
typedef struct _FLASH_SECTOR_GRP_MAP
{
    long nStartSectorNum;
    long nStartSectorSize;

    long nMidSectorNum;
    long nMidSectorSize;

    long nEndSectorNum;
    long nEndSectorSize;

} FLASH_SECTOR_GRP_MAP;

typedef struct _CFG_CAP_INFO
{
    UINT32  m_FfsSize;
    UINT32  m_FfsStartAddr;

    UINT32  m_SmsMaxNum;
    UINT32  m_SmsCbMaxNum;
    UINT32  m_SmsSrMaxNum;

    UINT32  m_PbkMinNum;
    UINT32  m_PbkMcMaxNumber;
    UINT32  m_PbkRcMaxNum;
    UINT32  m_PbkLdNum;

    UINT32    m_Uart0Cap;

    FLASH_SECTOR_GRP_MAP srecMap;
} CFG_CAP_INFO;


#if 0
typedef struct _FLASH_SECTOR_GRP_MAP
{
    long nStartSectorNum;
    long nStartSectorSize;

    long nMidSectorNum;
    long nMidSectorSize;

    long nEndSectorNum;
    long nEndSectorSize;

} FLASH_SECTOR_GRP_MAP;

typedef struct _CFG_CAP_INFO
{
    UINT32  m_FfsSize;
    UINT32  m_FfsStartAddr;

    UINT32  m_SmsMaxNum;
    UINT32  m_SmsCbMaxNum;
    UINT32  m_SmsSrMaxNum;

    UINT32  m_PbkMinNum;
    UINT32  m_PbkMcMaxNumber;
    UINT32  m_PbkRcMaxNum;
    UINT32  m_PbkLdNum;

    UINT32    m_Uart0Cap;

    FLASH_SECTOR_GRP_MAP srecMap;
} CFG_CAP_INFO;

#endif

const CFG_CAP_INFO g_CswCapInfo;

UINT32 RCM_GetPbkCap(RCM_PBK_CAP *pPbkCap )
{
    pPbkCap->m_PbkMinNum          = 8 * 1024 ; //g_CswCapInfo.m_PbkMinNum; //temp change by bridge for pbk cfg
    pPbkCap->m_PbkMcMaxNumber     = g_CswCapInfo.m_PbkMcMaxNumber;
    pPbkCap->m_PbkRcMaxNum        = g_CswCapInfo.m_PbkRcMaxNum;
    pPbkCap->m_PbkLdNum           = g_CswCapInfo.m_PbkLdNum;

    return ERR_SUCCESS;
}

UINT32 RCM_GetSmsCap(RCM_SMS_CAP *pSmsCap )
{
    pSmsCap->m_SmsMaxNum          = g_CswCapInfo.m_SmsMaxNum;
    pSmsCap->m_SmsCbMaxNum        = g_CswCapInfo.m_SmsCbMaxNum;
    pSmsCap->m_SmsCbMaxNum        = g_CswCapInfo.m_SmsCbMaxNum;
    pSmsCap->m_SmsCbMaxNum        = g_CswCapInfo.m_SmsCbMaxNum;

    return ERR_SUCCESS;
}

#if 0
UINT32 RCM_GetFlashCap(RCM_FLASH_CAP *pFlashCap )
{

    return ERR_SUCCESS;
}
#endif



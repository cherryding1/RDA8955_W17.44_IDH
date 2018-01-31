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

#ifndef _NVRAM_ME_PBK_API_H_
#define _NVRAM_ME_PBK_API_H_

#ifdef _FS_SIMULATOR_

#endif

//add for ME PBK
#define MAX_ME_PHONE_ENTRIES	       100
#define NVRAM_ME_PBK_INFO_BASE		   0
#define NVRAM_ME_DEFAULT_VALUE         0xff

#define ME_PBK_NUMBER_SIZE             10
#define ME_PBK_NAME_SIZE               14

#define NVRAM_ME_STORGE_POS            NVRAM_ME_PBK_INFO_BASE
#define NVRAM_ME_STORGE_SIZE           1
#define NVRAM_ME_TYPE_POS              (NVRAM_ME_STORGE_POS + NVRAM_ME_STORGE_SIZE)
#define NVRAM_ME_TYPE_SIZE             1
#define NVRAM_ME_INDEX_POS             (NVRAM_ME_TYPE_POS + NVRAM_ME_TYPE_SIZE)
#define NVRAM_ME_INDEX_SIZE            4
#define NVRAM_ME_TEL_POS               (NVRAM_ME_INDEX_POS + NVRAM_ME_INDEX_SIZE)
#define NVRAM_ME_TEL_SIZE              ME_PBK_NUMBER_SIZE + 1
#define NVRAM_ME_NAME_POS              (NVRAM_ME_TEL_POS + NVRAM_ME_TEL_SIZE)
#define NVRAM_ME_NAME_SIZE             ME_PBK_NAME_SIZE + 3
#define NVRAM_ME_TEL_LEN_POS           (NVRAM_ME_NAME_POS + NVRAM_ME_NAME_SIZE)
#define NVRAM_ME_TEL_LEN_SIZE          1
#define NVRAM_ME_NAME_LEN_POS          (NVRAM_ME_TEL_LEN_POS + NVRAM_ME_TEL_LEN_SIZE)
#define NVRAM_ME_NAME_LEN_SIZE         1
#define NVRAM_ME_BUFF_LEN			   (NVRAM_ME_NAME_LEN_POS + NVRAM_ME_NAME_LEN_SIZE)

typedef struct
{
    UINT8 storage;        /*0-none, 1-SIM, 2-ME*/
    UINT8 type;
    UINT16 index;
    UINT8 tel[ME_PBK_NUMBER_SIZE + 1];
    UINT8 name[ME_PBK_NAME_SIZE + 3];
	UINT8 tel_len;
	UINT8 name_len;
} NVRAM_ME_PHB_INFO;

VOID PBKI2Buf(NVRAM_ME_PHB_INFO *pbki, UINT8 *buf);
VOID Buf2PBKI(UINT8 *buf, NVRAM_ME_PHB_INFO *pbki);
INT32 NVRAM_WRITE_ME_PBK(INT32 hd, UINT32 iIndex, UINT8 *pBuff);
INT32 NVRAM_GET_STORGE_ME_PBK(INT32 hd, UINT32 iMaxEntries);

#endif // for _NVRAM_ME_PBK_API_H_


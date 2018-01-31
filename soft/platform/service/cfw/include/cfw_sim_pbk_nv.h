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

#ifndef _CFW_SIM_PBK_NV_H_
#define _CFW_SIM_PBK_NV_H_

//add for ME PBK
#define PBK_NV_VERSION                  0x20171024
#define MAX_ME_PHONE_ENTRIES            100
#define NV_ME_PBK_INFO_BASE		        0
#define NV_ME_DEFAULT_VALUE             0xff

#define ME_PBK_NUMBER_SIZE              10
#define ME_PBK_NAME_SIZE                14

#define NV_ME_STORGE_POS                NV_ME_PBK_INFO_BASE
#define NV_ME_STORGE_SIZE               1
#define NV_ME_TYPE_POS                  (NV_ME_STORGE_POS + NV_ME_STORGE_SIZE)
#define NV_ME_TYPE_SIZE                 1
#define NV_ME_INDEX_POS                 (NV_ME_TYPE_POS + NV_ME_TYPE_SIZE)
#define NV_ME_INDEX_SIZE                4
#define NV_ME_TEL_POS                   (NV_ME_INDEX_POS + NV_ME_INDEX_SIZE)
#define NV_ME_TEL_SIZE                  ME_PBK_NUMBER_SIZE + 1
#define NV_ME_NAME_POS                  (NV_ME_TEL_POS + NV_ME_TEL_SIZE)
#define NV_ME_NAME_SIZE                 ME_PBK_NAME_SIZE + 3
#define NV_ME_TEL_LEN_POS               (NV_ME_NAME_POS + NV_ME_NAME_SIZE)
#define NV_ME_TEL_LEN_SIZE              1
#define NV_ME_NAME_LEN_POS              (NV_ME_TEL_LEN_POS + NV_ME_TEL_LEN_SIZE)
#define NV_ME_NAME_LEN_SIZE             1
#define NV_ME_BUFF_LEN			        (NV_ME_NAME_LEN_POS + NV_ME_NAME_LEN_SIZE)

#define NV_ME_WRITE_ADD                 0x01
#define NV_ME_WRITE_DEL                 0x02

typedef struct
{
    UINT8 storage;        /*0-none, 1-SIM, 2-ME*/
    UINT8 type;
    UINT16 index;
    UINT8 tel[ME_PBK_NUMBER_SIZE + 1];
    UINT8 name[ME_PBK_NAME_SIZE + 3];
	UINT8 tel_len;
	UINT8 name_len;
} NV_ME_PHB_INFO;

typedef UINT8 PBK_REC[NV_ME_BUFF_LEN];
typedef struct
{
    UINT32 version;
    UINT8 usenum;
    PBK_REC pbkRec[MAX_ME_PHONE_ENTRIES];
}NV_PBK;

VOID PBKI2Buf(NV_ME_PHB_INFO *pbki, UINT8 *buf);
VOID Buf2PBKI(UINT8 *buf, NV_ME_PHB_INFO *pbki);
INT32 NV_PBK_Init(VOID);
INT32 NV_READ_ME_PBK(UINT32 iIndex, UINT8 *pBuff);
INT32 NV_WRITE_ME_PBK(UINT32 iIndex, UINT8 *pBuff, UINT8 type);
INT32 NV_GET_STORGE_ME_PBK(UINT32 iMaxEntries);

#endif // for _CFW_SIM_PBK_NV_H_



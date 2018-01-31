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

#include "dsm_cf.h"
#include "nvram.h"
#include "nvram_me_pbk_api.h"

VOID PBKI2Buf(NVRAM_ME_PHB_INFO *pbki, UINT8 *buf)
{
    DSM_MemCpy(&buf[NVRAM_ME_STORGE_POS], &(pbki->storage), NVRAM_ME_STORGE_SIZE);
    DSM_MemCpy(&buf[NVRAM_ME_TYPE_POS], &(pbki->type), NVRAM_ME_TYPE_SIZE);
    DSM_MemCpy(&buf[NVRAM_ME_INDEX_POS], &(pbki->index), NVRAM_ME_INDEX_SIZE);
    DSM_MemCpy(&buf[NVRAM_ME_TEL_POS], &(pbki->tel), NVRAM_ME_TEL_SIZE);
    DSM_MemCpy(&buf[NVRAM_ME_NAME_POS], &(pbki->name), NVRAM_ME_NAME_SIZE);
    DSM_MemCpy(&buf[NVRAM_ME_TEL_LEN_POS], &(pbki->tel_len), NVRAM_ME_TEL_LEN_SIZE);
    DSM_MemCpy(&buf[NVRAM_ME_NAME_LEN_POS], &(pbki->name_len), NVRAM_ME_NAME_LEN_SIZE);
}

VOID Buf2PBKI(UINT8 *buf, NVRAM_ME_PHB_INFO *pbki)
{
    pbki->storage = buf[NVRAM_ME_STORGE_POS];
    pbki->type = buf[NVRAM_ME_TYPE_POS];
	pbki->index= MAKEUINT16((buf)[NVRAM_ME_INDEX_POS],
                                     (buf)[NVRAM_ME_INDEX_POS+1]);
    DSM_MemCpy(pbki->tel,(UINT8*)&buf[NVRAM_ME_TEL_POS],NVRAM_ME_TEL_SIZE);
	DSM_MemCpy(pbki->name,(UINT8*)&buf[NVRAM_ME_NAME_POS],NVRAM_ME_NAME_SIZE);
    pbki->tel_len= buf[NVRAM_ME_TEL_LEN_POS];
	pbki->name_len = buf[NVRAM_ME_NAME_LEN_POS];
}

INT32 NVRAM_READ_ME_PBK(INT32 hd, UINT32 iIndex, UINT8 *pBuff)
{
	UINT32 iBytes;
	UINT32 iOffs = (iIndex - 1) * NVRAM_ME_BUFF_LEN;
	UINT32 iSize = NVRAM_ME_BUFF_LEN;
	iBytes = NVRAM_GetValue(hd, iOffs, iSize, pBuff);
	if(iBytes <= 0)
		return ERR_NVRAM_DEV_WRITE_FAILED;

	return iBytes;
}

INT32 NVRAM_WRITE_ME_PBK(INT32 hd, UINT32 iIndex, UINT8 *pBuff)
{
	UINT32 iBytes;
	UINT32 iOffs = (iIndex - 1) * NVRAM_ME_BUFF_LEN;
	UINT32 iSize = NVRAM_ME_BUFF_LEN;
	iBytes = NVRAM_SetValue(hd, iOffs, iSize, pBuff);
	if(iBytes <= 0)
		return ERR_NVRAM_DEV_WRITE_FAILED;

	return iBytes;
}

INT32 NVRAM_GET_STORGE_ME_PBK(INT32 hd, UINT32 iMaxEntries)
{
	UINT8 pBuff[NVRAM_ME_BUFF_LEN];
	INT32 usenum = 0;
	UINT32 iBytes;
	UINT32 i = 0;
	NVRAM_ME_PHB_INFO sPBKI = { 0 };
	for(i = 0; i < iMaxEntries; i++)
	{
		iBytes = NVRAM_GetValue(hd, i * NVRAM_ME_BUFF_LEN, NVRAM_ME_BUFF_LEN, pBuff);
		if(iBytes <= 0)
		{
			return ERR_NVRAM_PARAM_ERROR;
		}

		Buf2PBKI(pBuff, &sPBKI);
		if(sPBKI.index != (NVRAM_ME_DEFAULT_VALUE << 8 | NVRAM_ME_DEFAULT_VALUE))
		{
			usenum++;
		}
	}

	return usenum;
}



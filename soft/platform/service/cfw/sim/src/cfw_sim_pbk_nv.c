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

#include <sul.h>
#include "cs_types.h"
#include "cfw_sim_pbk_nv.h"
#include "fs/sf_api.h"

#define CFW_NV_PBK_FILE "cfw_nv_pbk.bin"

NV_PBK g_pbk_nv = {0, };
BOOL g_pbk_is_init = false;
VOID PBKI2Buf(NV_ME_PHB_INFO *pbki, UINT8 *buf)
{
    SUL_MemCopy8(&buf[NV_ME_STORGE_POS], &(pbki->storage), NV_ME_STORGE_SIZE);
    SUL_MemCopy8(&buf[NV_ME_TYPE_POS], &(pbki->type), NV_ME_TYPE_SIZE);
    SUL_MemCopy8(&buf[NV_ME_INDEX_POS], &(pbki->index), NV_ME_INDEX_SIZE);
    SUL_MemCopy8(&buf[NV_ME_TEL_POS], &(pbki->tel), NV_ME_TEL_SIZE);
    SUL_MemCopy8(&buf[NV_ME_NAME_POS], &(pbki->name), NV_ME_NAME_SIZE);
    SUL_MemCopy8(&buf[NV_ME_TEL_LEN_POS], &(pbki->tel_len), NV_ME_TEL_LEN_SIZE);
    SUL_MemCopy8(&buf[NV_ME_NAME_LEN_POS], &(pbki->name_len), NV_ME_NAME_LEN_SIZE);
}

VOID Buf2PBKI(UINT8 *buf, NV_ME_PHB_INFO *pbki)
{
    pbki->storage = buf[NV_ME_STORGE_POS];
    pbki->type = buf[NV_ME_TYPE_POS];
	pbki->index= MAKEUINT16((buf)[NV_ME_INDEX_POS],
                                     (buf)[NV_ME_INDEX_POS+1]);
    SUL_MemCopy8(pbki->tel,(UINT8*)&buf[NV_ME_TEL_POS],NV_ME_TEL_SIZE);
	SUL_MemCopy8(pbki->name,(UINT8*)&buf[NV_ME_NAME_POS],NV_ME_NAME_SIZE);
    pbki->tel_len= buf[NV_ME_TEL_LEN_POS];
	pbki->name_len = buf[NV_ME_NAME_LEN_POS];
}

static INT32 NV_UpdatePBK(VOID)
{
    BOOL result;
    const UINT8* cfg_path = CFW_NV_PBK_FILE;
    result = sf_safefile_write(cfg_path, (void*)&g_pbk_nv, sizeof(NV_PBK));
    if(!result)
    {
        return -1;
    }

    return 0;
}

INT32 NV_PBK_Init(VOID)
{
    BOOL bresult;
    INT32 result;
    const UINT8* cfg_path = CFW_NV_PBK_FILE;

    // Read NV data from file.
    bresult = sf_safefile_init(cfg_path);
    if(!bresult)
    {
        SUL_MemSet8((UINT8*)&g_pbk_nv, NV_ME_DEFAULT_VALUE, sizeof(NV_PBK));
        g_pbk_nv.version = PBK_NV_VERSION;
        g_pbk_nv.usenum = 0;
        result = NV_UpdatePBK();
        if(result != 0)
        {
            return -1;
        }
    }
    else
    {
        result = sf_safefile_read(cfg_path, (void*)&g_pbk_nv, sizeof(NV_PBK));
        if(result != sizeof(NV_PBK) || PBK_NV_VERSION != g_pbk_nv.version)
        {
            SUL_MemSet8((UINT8*)&g_pbk_nv, NV_ME_DEFAULT_VALUE, sizeof(NV_PBK));
            g_pbk_nv.version = PBK_NV_VERSION;
            g_pbk_nv.usenum = 0;
            result = NV_UpdatePBK();
            if(result != 0)
            {
                return -1;
            }
        }
    }

    g_pbk_is_init = true;
    return ERR_SUCCESS;
}

INT32 NV_READ_ME_PBK(UINT32 iIndex, UINT8 *pBuff)
{
    if(!g_pbk_is_init)
        return -1;

    SUL_MemCopy8(pBuff, g_pbk_nv.pbkRec[iIndex - 1], NV_ME_BUFF_LEN);
	return ERR_SUCCESS;
}

INT32 NV_WRITE_ME_PBK(UINT32 iIndex, UINT8 *pBuff, UINT8 type)
{
	if(!g_pbk_is_init)
        return -1;

    INT32 result;
    UINT8 pBuff_r[NV_ME_BUFF_LEN];
    NV_ME_PHB_INFO sPBKI = { 0 };
    SUL_MemCopy8(pBuff_r, (UINT8*)g_pbk_nv.pbkRec[iIndex - 1], NV_ME_BUFF_LEN);
	Buf2PBKI(pBuff_r, &sPBKI);
	if(sPBKI.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE) && type == NV_ME_WRITE_DEL)
	{
		g_pbk_nv.usenum -= 1;
	}
    else if(sPBKI.index == (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE) && type == NV_ME_WRITE_ADD)
    {
        g_pbk_nv.usenum += 1;
    }

	SUL_MemCopy8((UINT8*)g_pbk_nv.pbkRec[iIndex - 1], pBuff, NV_ME_BUFF_LEN);
    result = NV_UpdatePBK();
    if(result != 0)
    {
        return -1;
    }

	return ERR_SUCCESS;
}

INT32 NV_GET_STORGE_ME_PBK(UINT32 iMaxEntries)
{
    if(!g_pbk_is_init)
        return -1;

	return g_pbk_nv.usenum;
}


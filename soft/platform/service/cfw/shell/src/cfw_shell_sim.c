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

#include <base.h>
#include <cfw.h>
#include <stdkey.h>
#include <sul.h>
#include "api_msg.h"
#include <ts.h>
#include <cfw_prv.h>
#include <event_prv.h>

#if 0
UINT32 SHL_EraseFlash(UINT8 nMode) // 0: FS flash area, 1: all flash area exclude boot code.
{
    UINT32 flash_offset = 0;
    UINT32 sector_size  = 0;
    UINT32 i = 0 ;

#ifdef s71pl064
#define FLASH_ADDRESS       ((UINT32)(0x00700000)) // Base Address for User Flash Area,it can be used for 64K&8K sector
#define FALSH_MAX_SIZE      (0x100000)
#define FALSH_SECTOR_SIZE   (0x10000) //64k
#else
#define FLASH_ADDRESS       ((UINT32)(4*64*1024 + 51*256*1024)) // (4 x 64K, 62 x 256K, 4 x 64K) 
#define FALSH_MAX_SIZE      (0x00300000)
#define FALSH_SECTOR_SIZE   (0x40000) //256k
#endif

    if(nMode == 0)
    {
        flash_offset = FLASH_ADDRESS;
        sector_size  = FALSH_SECTOR_SIZE;

        for ( i = 0; i < FALSH_MAX_SIZE / sector_size; i++ )
        {
            if(DRV_RawEraseFlash( i * sector_size + flash_offset ) == ERR_SUCCESS)
                CSW_TRACE( _CSW|TLEVEL(200)|TDB|TNB_ARG(1), TSTR("Erase sector[0x%x]\n",0x08100a27), i * sector_size + flash_offset);
        }
    }
    else if(nMode == 1)
    {
        flash_offset = 4 * 64 * 1024;
        sector_size  = FALSH_SECTOR_SIZE;

        for ( i = 0; i < FALSH_MAX_SIZE / sector_size; i++ )
        {
            if(DRV_RawEraseFlash( i * sector_size + flash_offset ) == ERR_SUCCESS)
                CSW_TRACE( _CSW|TLEVEL(200)|TDB|TNB_ARG(1), TSTR("Erase sector[0x%x]\n",0x08100a28), i * sector_size + flash_offset);
        }
    }
    else
        return FALSE;

    return TRUE;
}
#endif

#ifdef CFW_PBK_SYNC_VER
UINT32 SRVAPI SHL_SimAddPbkEntry (
    UINT8 nStorage,
    CFW_SIM_PBK_ENTRY_INFO *pEntryInfo,
    CFW_PBK_OUT_PARAM *pOutParam
)
{


}

UINT32 SRVAPI SHL_SimDeletePbkEntry (
    UINT8 nStorage,
    UINT8 nIndex,
    CFW_PBK_OUT_PARAM *pOutParam
)
{

}
#endif


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

#ifndef _VDS_SHELL_H_
#define _VDS_SHELL_H_

#define VDS_TEST_NR_VB              29
#define VDS_TEST_NO_MAX_VBN    (VDS_TEST_NR_VB - 1)
#define VDS_TEST_SZ_BLOCK         512
#define VDS_TEST_NR_OP                 3

// The map file of VDS.
#define FLASH_MAP_FILE "map.fla"

#define ERR_TEST_FAILURE -0x1


#define VDS_OPERTYPE_BASE    0xF0
#define VDS_OPERTYPE_READ    ( VDS_OPERTYPE_BASE + 0 )
#define VDS_OPERTYPE_WRITE   ( VDS_OPERTYPE_BASE + 1 )
#define VDS_OPERTYPE_ERASE   ( VDS_OPERTYPE_BASE + 2 )
#define VDS_OPERTYPE_COUNTER 3
#define VDS_IS_OPERTYPE( value, type )   ( (type) == VDS_OPERTYPE_BASE + (value % VDS_OPERTYPE_COUNTER ) )


typedef struct vds_operation
{
    INT32 iOperType;
    UINT32 iVbn;
    UINT32 iData;
}
VDS_Operation;

#define VDS_STATUS_VB_UNMAPPED 0xAAAA
#define VDS_STATUS_VB_MAPPED   0xBBBB

typedef struct vds_block_shadow
{
    INT32 iVBStatus;
    INT32 iData;
}
VDS_BlockShadow;

#ifdef _FS_SIMULATOR_
INT32 vds_TstOpenFlash( VOID );
INT32 vds_TstUpdateflashMap();
#endif
UINT16 vds_TstInit();
INT32 vds_TstEraseVB( UINT32 iPartId, UINT16 iVBN );
INT32 vds_TstWriteVB( UINT32 iPartId, UINT16 iVBN );
INT32 vds_TstReadVB( UINT32 iPartId, UINT16 iVBN );
UINT16 vds_TstGetRandVBN( VOID );
UINT8 vds_TstGetRandOP( void );
INT32 vds_TstGetFlashInfo(UINT32 *iSecCnt, UINT32 *iSecSz);

#endif // _VDS_SHELL_H_



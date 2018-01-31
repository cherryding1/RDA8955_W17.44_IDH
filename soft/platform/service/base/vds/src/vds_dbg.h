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

#ifndef _VDS_DBG_H_
#define _VDS_DBG_H_


#define DSM_HTON16(x) \
    ((UINT16)( \
        (((UINT16)(x) & (UINT16)0x00ffU) << 8) | \
        (((UINT16)(x) & (UINT16)0xff00U) >> 8) ))


#define DSM_HTON32(x) \
    ((UINT32)( \
        (((UINT32)(x) & (UINT32)0x000000ffUL) << 24) | \
        (((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) | \
        (((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) | \
        (((UINT32)(x) & (UINT32)0xff000000UL) >> 24) ))



typedef struct vds_ValueAndDescription
{
    INT32 iValue;
    INT8 *pDesc;
}
VDS_VALUE_AND_DESC;

#define FIELD_TYPE_INT16        0x0
#define FIELD_TYPE_INT32        0x1
#define FIELD_TYPE_HEX16        0x2
#define FIELD_TYPE_HEX32        0x3
#define FIELD_TYPE_HEX_STR      0x4
#define FIELD_TYPE_STRING       0x5
#define FIELD_TYPE_INT16_DESC   0x6
#define FIELD_TYPE_INT32_DESC   0x7

#define VDS_FIELD_FOUND       0
#define VDS_FIELD_NOT_FOUND   1
#define VDS_FIELD_MORE_FOUND  2


struct field_attr
{
    INT8 *name;
    UINT32 iType;
    UINT32 iPos;
    UINT32 iSize;
};

#ifdef _FS_SIMULATOR_
#define TS_OUTPUT_LEN_MAX       1000
#else
#define TS_OUTPUT_LEN_MAX       100
#endif

#define VDS_DUMP_LINE_SIZE                  16

#define ONE_IS_EQUAL_TO_ZERO  0

#define VDS_FREE_LIST_ID               0
#define VDS_DIRTY_LIST_ID              1
#define VDS_CURRENT_CANDIDATE_LIST_ID  2

#define VDS_CURRENT_SECTOR_ID          0
#define VDS_GCING_SECTOR_ID            1
#define VDS_REMAIN_SECTOR_ID           2

void vds_Dump( char *szPrompt, unsigned char *pBuf, long ulBufLen );
VOID vds_ShowPartition( UINT32 iPartId );
VOID vds_ShowSectRef( UINT32 iPartId, INT32 iSectId );
VOID vds_ShowSectRefList( UINT32 iPartId, INT32 iListId );
VOID vds_ShowMT( UINT32 iPartId, UINT32 index, UINT32 counter );
#ifdef _FS_SIMULATOR_
VOID vds_DumpObject
( FILE *file, INT32 iObjAddr, INT32 iObjSize, UINT8 *buf,
  struct field_attr *psFieldAttr, INT32 iNrField );
#else
VOID vds_DumpObject
( INT32 iObjAddr, INT32 iObjSize, UINT8 *buf,
  struct field_attr *psFieldAttr, INT32 iNrField );
#endif
VOID vds_DumpPBDT( UINT32 iPartId, UINT16 iSN, UINT16 iPBN, INT32 counter );
VOID vds_DumpBlock( UINT32 iPartId, UINT16 iSN, UINT16 iPBN, UINT32 iOffset, UINT32 iLen );

INT32 vds_GetValue( VDS_VALUE_AND_DESC *val_descs, UINT8 *desc );
INT32 vds_FindField( INT8 *substr, struct field_attr *layout, INT32 iNrFields, INT32 *piIndex );

VOID vds_RawWriteObject
( INT32 iObjAddr, struct field_attr *pAttr, UINT8 *pBuf );




#endif



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

#ifdef DSM_SHELL_COMMAND

#include "dsm_cf.h"
#include "dsm_dbg.h"
#include "vds_cd.h"
#include "vds_dbg.h"
#include "vds_local.h"
#include "vds_tools.h"


// extern variable declaring.
// partiton table.
extern VDS_PARTITION g_VDS_partition_table[];

// partition count.
extern UINT32 g_vds_partition_count;

// sector ref chain.
extern VDS_SECT_REF *g_psSectRef;


// remain sector chain.
extern VDS_SECT_REF *g_psRemainRef;

// flash information.
extern VDS_FLASH_INFO g_sFlashInfo;


VDS_VALUE_AND_DESC g_vds_value_and_descs[] =
{
    { VDS_STATUS_SECT_FREE, "FREE   " },
    { VDS_STATUS_SECT_REMAIN, "REMAIN " },
    { VDS_STATUS_SECT_TEMP, "TEMP" },
    { VDS_STATUS_SECT_CURRENT, "CURRENT" },
    { VDS_STATUS_SECT_DIRTY, "DIRTY  " },
    { VDS_STATUS_SECT_GCING, "GCING  " },

    { VDS_STATUS_PB_FREE, "FRESH  " },
    { VDS_STATUS_PB_VALID, "VALID  " },
    { VDS_STATUS_PB_INVALID, "INVALID" },

    //  { VDS_STATUS_VB_MAPPED,     "Mapped " },
    //  { VDS_STATUS_VB_UNMAPPED,   "Unmapped" },

    //  { VDS_OPERTYPE_READ,        "read   " },
    //  { VDS_OPERTYPE_WRITE,       "write  " },
    // { VDS_OPERTYPE_ERASE,       "erase  " },
    // { 0,                        NULL      },
};


/* see MACROS FIELD_TYPE_*** */
char *g_field_types[] =
{
    "%d", "%d", "0x%X", "0x%X", "%s", "%s", "%s", "%s",
};

struct field_attr _vds_sh_layout[] =
{
    { "Magic Number",      FIELD_TYPE_HEX32,       (UINT32) &((struct _vds_sh * )0)->iMagic,          sizeof(((struct _vds_sh * )0)->iMagic),           },
    { "Sector Status",     FIELD_TYPE_INT32_DESC,  (UINT32) &((struct _vds_sh * )0)->iSectorStatus,   sizeof(((struct _vds_sh * )0)->iSectorStatus),    },
    { "Erase Counter",     FIELD_TYPE_INT32,       (UINT32) &((struct _vds_sh * )0)->iEraseCounter,   sizeof(((struct _vds_sh * )0)->iEraseCounter),    },
    //    { "with partition",    FIELD_TYPE_INT32_DESC,  (UINT32)&((struct _vds_sh* )0)->bAssociatedWithPart,   sizeof(((struct _vds_sh* )0)->bAssociatedWithPart),    },
    { "number of sectors in partition",   FIELD_TYPE_INT32,       (UINT32) &((struct _vds_sh * )0)->iFlashSecCount,   sizeof(((struct _vds_sh * )0)->iFlashSecCount),    },
    { "sector size",       FIELD_TYPE_INT32,       (UINT32) &((struct _vds_sh * )0)->iFlashSectorSize,   sizeof(((struct _vds_sh * )0)->iFlashSectorSize),    },
    { "block size",        FIELD_TYPE_INT32,       (UINT32) &((struct _vds_sh * )0)->iVBSize,         sizeof(((struct _vds_sh * )0)->iVBSize),    },
    { "partition name",    FIELD_TYPE_STRING,       (UINT32) &((struct _vds_sh * )0)->acPartName,         sizeof(((struct _vds_sh * )0)->acPartName),    },
    { "CRC",               FIELD_TYPE_HEX32,       (UINT32) &((struct _vds_sh * )0)->iCRC,            sizeof(((struct _vds_sh * )0)->iCRC),             },
    { "reserved",          FIELD_TYPE_INT32,     (UINT32) &((struct _vds_sh * )0)->reserved,        sizeof(((struct _vds_sh * )0)->reserved),         },
};


struct field_attr vds_pbd_layout[] =
{
    { "status",           FIELD_TYPE_INT16_DESC,  (UINT32) &((struct _vds_pbd * )0)->iStatus,        sizeof(((struct _vds_pbd * )0)->iStatus),         },
    { "vbn",              FIELD_TYPE_INT16,       (UINT32) &((struct _vds_pbd * )0)->iVBN,           sizeof(((struct _vds_pbd * )0)->iVBN),            },
    { "version",          FIELD_TYPE_INT32,       (UINT32) &((struct _vds_pbd * )0)->iVersion,       sizeof(((struct _vds_pbd * )0)->iVersion),        },
    { "PBDCRC",           FIELD_TYPE_HEX32,       (UINT32) &((struct _vds_pbd * )0)->iPBDCRC,        sizeof(((struct _vds_pbd * )0)->iPBDCRC),         },
    { "PBCRC",            FIELD_TYPE_HEX32,       (UINT32) &((struct _vds_pbd * )0)->iPBCRC,         sizeof(((struct _vds_pbd * )0)->iPBCRC),          },
};

#ifdef _FS_SIMULATOR_
struct field_attr vds_operation_layout[] =
{
    { "type",             FIELD_TYPE_INT16_DESC,  (UINT32) &((struct vds_operation * )0)->iOperType, sizeof(((struct vds_operation * )0)->iOperType),  },
    { "vbn",              FIELD_TYPE_INT32,       (UINT32) &((struct vds_operation * )0)->iVbn,      sizeof(((struct vds_operation * )0)->iVbn),       },
    { "data",             FIELD_TYPE_HEX32,       (UINT32) &((struct vds_operation * )0)->iData,     sizeof(((struct vds_operation * )0)->iData),      },
};


struct field_attr vds_block_shadow_layout[] =
{
    { "status",           FIELD_TYPE_INT16_DESC,  (UINT32) &((struct vds_block_shadow * )0)->iVBStatus, sizeof(((struct vds_block_shadow * )0)->iVBStatus), },
    { "data",             FIELD_TYPE_HEX32,       (UINT32) &((struct vds_block_shadow * )0)->iData,  sizeof(((struct vds_block_shadow * )0)->iData),   },
};
#endif

/**/
INT32 _vds_FindField( INT8 *substr, INT32 iLen, struct field_attr *layout, INT32 iNrFields, INT32 *piIndex )
{
    INT32 i;
    INT32 iTmp = 0; /* Initialization ONLY for compiler */
    BOOL bFound;

    for ( i = 0, bFound = FALSE; i < iNrFields; i++ )
    {
        if ( 0 == DSM_StrNCaselessCmp( substr, layout[ i ].name, iLen ) )
        {
            if ( !bFound )
            {
                iTmp = i;
                bFound = TRUE;
            }
            else
            {
                return VDS_FIELD_MORE_FOUND;
            }
        }
    }

    if ( bFound )
    {
        *piIndex = iTmp;
        return VDS_FIELD_FOUND;
    }
    else
    {
        return VDS_FIELD_NOT_FOUND;
    }
}


INT32 vds_FindField( INT8 *substr, struct field_attr *layout, INT32 iNrFields, INT32 *piIndex )
{
    INT32 iLen = DSM_StrLen( substr );
    INT32 i;
    INT32 iRet;

    for ( i = 1; i <= iLen; i++ )
    {
        iRet = _vds_FindField( substr, i, layout, iNrFields, piIndex );
        if ( VDS_FIELD_NOT_FOUND == iRet || VDS_FIELD_FOUND == iRet )
        {
            return iRet;
        }
        else
        {
            continue;
        }
    }
    return VDS_FIELD_NOT_FOUND;
}

INT8 *vds_GetDesc( VDS_VALUE_AND_DESC *val_descs, INT32 iValue )
{
    INT32 i;

    for ( i = 0; val_descs[ i ].pDesc; i++ )
    {
        if ( iValue == val_descs[ i ].iValue )
            return val_descs[ i ].pDesc;
    }

    return NULL;
}



INT32 vds_GetValue( VDS_VALUE_AND_DESC *val_descs, UINT8 *desc )
{
    INT32 i;
    INT32 iLen;

    iLen = DSM_StrLen( desc );

    for ( i = 0; val_descs[ i ].pDesc; i++ )
    {
        if ( 0 == DSM_StrNCmp( val_descs[ i ].pDesc, desc, iLen ) )
            return val_descs[ i ].iValue;
    }

    return -1;
}

void vds_Dump( char *szPrompt, unsigned char *pBuf, long ulBufLen )
{
    long i;
    long j;
    char acTmp[ 3 * VDS_DUMP_LINE_SIZE + LEN_FOR_NULL_CHAR ] = { 0 };
    long remainder = ulBufLen % VDS_DUMP_LINE_SIZE;
    unsigned char *p = NULL;
    long arr_size = sizeof( acTmp );

    if ( pBuf == NULL || arr_size > TS_OUTPUT_LEN_MAX )
    {
        return ;
    }

    if ( szPrompt && DSM_StrLen( szPrompt ) <= TS_OUTPUT_LEN_MAX )
    {
        dsm_Printf( BASE_VDS_TS_ID, "%s:\r\n", szPrompt );
    }

    for ( p = pBuf; p < pBuf + ulBufLen - remainder; p += VDS_DUMP_LINE_SIZE )
    {
        for ( i = 0, j = 0; i < VDS_DUMP_LINE_SIZE; i++ )
        {
            j += DSM_StrPrint( acTmp + j, "%2.2X ", p[ i ] );
        }
        dsm_Printf( BASE_VDS_TS_ID, "%s\r\n", acTmp );
    }

    acTmp[ 0 ] = NULL_CHAR;

    for ( i = 0, j = 0; i < remainder; i++ )
    {
        j += DSM_StrPrint( acTmp + j, "%2.2X ", p[ i ] );
    }
    dsm_Printf( BASE_VDS_TS_ID, "%s\r\n", acTmp );

    return ;
}


VOID vds_ShowPartition( UINT32 uPartId )
{
    VDS_PARTITION *psPart;

    if ( uPartId < 0 || uPartId >= VDS_GetPartitionCount() )
    {
        return ;
    }

    psPart = &g_VDS_partition_table[ uPartId ];

    dsm_Printf( BASE_VDS_TS_ID, "--------------------partion ID[%d]--------------------\r\n", psPart - g_VDS_partition_table );
    //  dsm_Printf( BASE_VDS_TS_ID, "Beginning Addr   :  0x%08X\r\n", psPart->sPartInfo.iFirstSecAddr );
    dsm_Printf( BASE_VDS_TS_ID, "Total sector count :  0x%08X\r\n", psPart->sPartInfo.iFlashSectorCount);
    dsm_Printf( BASE_VDS_TS_ID, "Number of sectors:  %d\r\n", VDS_NR_SECT( psPart ) );
    dsm_Printf( BASE_VDS_TS_ID, "Sector size      :  0x%08X\r\n", psPart->sPartInfo.iFlashSectorSize );
    dsm_Printf( BASE_VDS_TS_ID, "Block size       :  0x%X(%4d)\r\n", psPart->sPartInfo.iVBSize, psPart->sPartInfo.iVBSize );
    /*
    nr of dirty sectors
    nr of free sectors
    nr of current-candidate sectors
    */
}


VOID vds_ShowSectRef( UINT32 uPartId, INT32 iSectId )
{
    VDS_PARTITION *psCP;
    VDS_SECT_REF *psRef;
    INT8 *names[] = { "current sector", "gcing sector  ", "remain sector ", };

    if ( uPartId < 0 || uPartId >= VDS_GetPartitionCount() )
    {
        return ;
    }

    psCP = &g_VDS_partition_table[ uPartId ];

    if ( iSectId == VDS_CURRENT_SECTOR_ID )
        psRef = psCP->pCurrentSect;
    else if ( iSectId == VDS_GCING_SECTOR_ID )
        psRef = psCP->pGcingSect;
    else if ( iSectId == VDS_REMAIN_SECTOR_ID )
        psRef = g_psRemainRef;
    else
        return ;

    if ( psRef )
        dsm_Printf( BASE_VDS_TS_ID, "%s: (%4d, %5d)\r\n", names[ iSectId ], psRef->iSN, psRef->iCurrentPBN );
    else
        dsm_Printf( BASE_VDS_TS_ID, "%s: -------------\r\n", names[ iSectId ] );
}

VOID vds_ShowSectRefList( UINT32 uPartId, INT32 iListId )
{
    VDS_PARTITION *psCP;
    VDS_SECT_REF *list;
    VDS_SECT_REF *p;

    if ( uPartId < 0 || uPartId >= VDS_GetPartitionCount() )
    {
        return ;
    }

    psCP = &g_VDS_partition_table[ uPartId ];

    if ( iListId == VDS_FREE_LIST_ID )
        list = psCP->pFreeList;
    else if ( iListId == VDS_DIRTY_LIST_ID )
        list = psCP->pDirtyList;
    else
        return ;

    if ( !list )
    {
        dsm_Printf( BASE_VDS_TS_ID, "\r\n\r\n" );
        return ;
    }

    dsm_Printf( BASE_VDS_TS_ID, "(  SN,   PBN )\r\n" );
    for ( p = vds_GetFirstSecRef( list ); p; p = vds_GetNextSecRef( list, p ) )
    {
        dsm_Printf( BASE_VDS_TS_ID, "(%4d, %5d)\r\n", p->iSN, p->iCurrentPBN );
    }

    dsm_Printf( BASE_VDS_TS_ID, "\r\n" );

    return ;
}


VOID vds_ShowMT( UINT32 uPartId, UINT32 index, UINT32 counter )
{
    VDS_PARTITION *psCP;
    UINT32 i;
    UINT32 nr_entry;

    if ( uPartId < 0 || uPartId >= VDS_GetPartitionCount() )
    {
        return ;
    }

    psCP = &g_VDS_partition_table[ uPartId ];
    nr_entry = VDS_NR_MT_VALID_ENTRY( psCP );

    if ( index > nr_entry )
    {
        return ;
    }

    if ( index + counter > nr_entry )
    {
        counter = nr_entry - index;
    }

    dsm_Printf( BASE_VDS_TS_ID, "( VBN,   SN,   PBN)\r\n" );
    for ( i = index; i < index + counter; i++ )
    {
        dsm_Printf( BASE_VDS_TS_ID, "(%4d, %5d, %5d)\r\n", i, psCP->psMT[ i ].iSN, psCP->psMT[ i ].iPBN );
    }

    return ;
}

#ifdef _FS_SIMULATOR_
VOID vds_DumpObject
( FILE *file, INT32 iObjAddr, INT32 iObjSize, UINT8 *buf,
  struct field_attr *psFieldAttr, INT32 iNrField )
#else
VOID vds_DumpObject
( INT32 iObjAddr, INT32 iObjSize, UINT8 *buf,
  struct field_attr *psFieldAttr, INT32 iNrField )
#endif
{
    UINT32 iSize;
    INT32 i;
    struct field_attr *p;
    INT16 *pInt16;
    INT32 *pInt32;
    INT8 *desc;
    INT8 formats[ 64 ] = { 0, };
    INT32 iRet;

#ifdef _FS_SIMULATOR_

    if ( !file )
    {
        iRet = DRV_ReadFlash( iObjAddr, buf, iObjSize, &iSize );
        if ( ( iRet != ERR_SUCCESS ) || ( ( INT32 ) iSize != iObjSize ) )
        {
            return ;
        }
    }
    else
    {
        iSize = fread( buf, 1, iObjSize, file );
        if ( ( INT32 ) iSize != iObjSize )
        {
            return ;
        }
    }
#else
    iRet = DRV_ReadFlash( iObjAddr, buf, iObjSize, &iSize );
    if ( ( iRet != ERR_SUCCESS ) || ( iSize != iObjSize ) )
    {
        return ;
    }
#endif

    for ( i = 0; i < iNrField; i++ )
    {
        p = psFieldAttr + i;

        DSM_StrPrint( formats, "%%-16s:  %s\n", g_field_types[ p->iType ] );

        switch ( p->iType )
        {
            case FIELD_TYPE_INT16:
            case FIELD_TYPE_HEX16:
                pInt16 = ( INT16 * ) ( buf + p->iPos );
                dsm_Printf( BASE_VDS_TS_ID, formats, p->name, *pInt16 );
                break;
            case FIELD_TYPE_INT32:
            case FIELD_TYPE_HEX32:
                pInt32 = ( INT32 * ) ( buf + p->iPos );
                dsm_Printf( BASE_VDS_TS_ID, formats, p->name, *pInt32 );
                break;
            case FIELD_TYPE_STRING:
                dsm_Printf( BASE_VDS_TS_ID, formats, p->name, buf + p->iPos );
                break;
            case FIELD_TYPE_HEX_STR:
                break;
            case FIELD_TYPE_INT16_DESC:
            case FIELD_TYPE_INT32_DESC:
                desc = vds_GetDesc( g_vds_value_and_descs, *( INT32 * ) ( buf + p->iPos ) );
                if ( !desc )
                {
                    dsm_Printf( BASE_VDS_TS_ID, "%-16s:  0x%X(ILLEGAL status)\n", p->name, *( INT32 * ) ( buf + p->iPos ) );
                }
                else
                {
                    dsm_Printf( BASE_VDS_TS_ID, formats, p->name, desc );
                }
                break;
            default:
                break;
        }

    }
}


VOID vds_DumpPBDT( UINT32 uPartId, UINT16 iSN, UINT16 iPBN, INT32 counter )
{
    VDS_PARTITION *psCP;
    VDS_PBD pbd;
    UINT32 iSize;
    INT32 iPBDTAddr;
    INT32 iPBDAddr;
    UINT16 i;
    UINT16 iNrPBDPerSect;
    INT8 *pStatus;
    INT32 iRet;

    if ( uPartId < 0 || uPartId >= VDS_GetPartitionCount() )
    {
        return ;
    }

    psCP = &g_VDS_partition_table[ uPartId ];

    if(FALSE == vds_IsValidSn(psCP, iSN))
    {
        return ;
    }

    iNrPBDPerSect = ( UINT16 ) VDS_NR_PB_PER_SECT( psCP );

    iPBDTAddr = VDS_ADDR_PBDT( psCP, iSN );

    dsm_Printf( BASE_VDS_TS_ID, "PBDN   Status     VBN   Version      PBDCRC        PBCRC\r\n" );

    counter--;
    i = iPBN;
    do
    {
        iPBDAddr = iPBDTAddr + i * VDS_SZ_PBD;
        iRet = DRV_ReadFlash( iPBDAddr, ( UINT8 * ) & pbd, VDS_SZ_PBD, &iSize );
        if ( ( iRet != ERR_SUCCESS ) || ( iSize != VDS_SZ_PBD ) )
        {
            return ;
        }

        pStatus = vds_GetDesc( g_vds_value_and_descs, pbd.iStatus );

        if ( pStatus )
        {
            dsm_Printf( BASE_VDS_TS_ID, "%4d   %-8s   %-5d %-5d     0x%08X     0x%08X\r\n",
                        i, pStatus, pbd.iVBN, pbd.iVersion, pbd.iPBDCRC, pbd.iPBCRC );
        }
        else
        {
            dsm_Printf( BASE_VDS_TS_ID, "%4d   0x%-8X %-5d %-5d     0x%08X     0x%08X\r\n",
                        i, pbd.iStatus, pbd.iVBN, pbd.iVersion, pbd.iPBDCRC, pbd.iPBCRC );
        }

        counter--;
        i++;

        if ( 0 == i % 16 )
        {
            dsm_Printf( BASE_VDS_TS_ID, "PBDN   Status     VBN   Version      PBDCRC        PBCRC\r\n" );
        }
    }
    while ( i < iNrPBDPerSect && counter >= 0 );

}


VOID vds_DumpBlock( UINT32 uPartId, UINT16 iSN, UINT16 iPBN, UINT32 iOffset, UINT32 iLen )
{
    VDS_PARTITION *psCP;
    UINT32 iSize;
    INT32 iPBAddr;
    INT32 iRet;
    UINT8 szBuff[VDS_BLOCK_SIZE];
    if ( uPartId < 0 || uPartId >= VDS_GetPartitionCount() )
    {
        return ;
    }

    psCP = &g_VDS_partition_table[ uPartId ];

    if(FALSE == vds_IsValidSn(psCP, iSN))
    {
        return ;
    }

    if ( iOffset > VDS_SZ_PB( psCP ) )
    {
        return ;
    }

    if ( 0 == iLen )
    {
        iLen = VDS_SZ_PB( psCP );
    }
    if ( iOffset + iLen > VDS_SZ_PB( psCP ) )
    {
        iLen = VDS_SZ_PB( psCP ) - iOffset;
    }

    iPBAddr = VDS_ADDR_PB( psCP, iSN ) + iPBN * VDS_SZ_PB( psCP );

    iRet = DRV_ReadFlash( iPBAddr, szBuff, VDS_SZ_PB( psCP ), &iSize );
    if ( ( iRet != ERR_SUCCESS ) || ( iSize != VDS_SZ_PB( psCP ) ) )
    {
        return ;
    }

    vds_Dump( NULL, szBuff + iOffset, iLen );
}



VOID vds_RawWriteObject
( INT32 iObjAddr, struct field_attr *pAttr, UINT8 *pBuf )
{
    INT32 iValue = 0;
    INT32 iSize;
    INT32 iWSize;
    UINT8 *pData;
    INT32 iValueForDesc;
    INT32 iRet;

    switch ( pAttr->iType )
    {
        case FIELD_TYPE_INT16:
        case FIELD_TYPE_INT32:
            iValue = DSM_StrAToI( ( INT8 * ) pBuf );
            pData = ( UINT8 * ) & iValue;
            break;
        case FIELD_TYPE_HEX16:
        case FIELD_TYPE_HEX32:
            vds_str_to_hex( ( INT8 * ) pBuf, &iSize, NULL );
            if ( ( UINT32 ) iSize > pAttr->iSize )
                return ;

            vds_str_to_hex( ( INT8 * ) pBuf, &iSize, ( char * ) & iValue );
            if ( FIELD_TYPE_HEX16 == pAttr->iType )
            {
                iValue = DSM_HTON16( ( UINT16 ) iValue );
            }
            else
            {
                iValue = DSM_HTON32( iValue );
            }

            pData = ( UINT8 * ) & iValue;
            break;

        case FIELD_TYPE_INT16_DESC:
        case FIELD_TYPE_INT32_DESC:
            iValueForDesc = vds_GetValue( g_vds_value_and_descs, pBuf );
            if ( -1 == iValueForDesc )
            {
                vds_str_to_hex( ( INT8 * ) pBuf, &iSize, NULL );
                if ( ( UINT32 ) iSize > pAttr->iSize )
                    return ;

                vds_str_to_hex( ( INT8 * ) pBuf, &iSize, ( char * ) & iValueForDesc );
                if ( FIELD_TYPE_INT16_DESC == pAttr->iType )
                {
                    iValueForDesc = DSM_HTON16( ( UINT16 ) iValueForDesc );
                }
                else
                {
                    iValueForDesc = DSM_HTON32( iValueForDesc );
                }
            }
            pData = ( UINT8 * ) & iValueForDesc;
            break;

        default:
            return ;
    }

    iRet = DRV_WriteFlash( iObjAddr + pAttr->iPos, pData, pAttr->iSize, ( UINT32 * ) &iWSize );
    if ( ( ERR_SUCCESS != iRet ) || ( pAttr->iSize != ( UINT32 ) iWSize ) )
    {
        dsm_Printf( BASE_VDS_TS_ID, "vds_RawWriteObject:call DRV_WriteFlash() failed,iRet = %d, write size = %d, written size = %d.\n", iRet, pAttr->iSize, iWSize);
        return ;
    }
}
#endif


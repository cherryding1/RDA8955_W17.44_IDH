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
#include "dsm_shell.h"
#include "vds_cd.h"
#include "vds_local.h"
#include "vds_dbg.h"
#include "vds_tools.h"

#include "vds_api.h"
#include "vds_shell.h"

extern char *g_vds_CommandLines[];

extern VDS_PARTITION g_VDS_partition_table[];
// partition count.
extern UINT32 g_vds_partition_count;

// sector ref chain.
extern VDS_SECT_REF *g_psSectRef;

// free sector chain.
extern VDS_SECT_REF *g_psFreeRef;

// remain sector chain.
extern VDS_SECT_REF *g_psRemainRef;

// flash information.
extern VDS_FLASH_INFO g_sFlashInfo;

VDS_FLASH_INFO g_vds_flash_info;
extern VDS_VALUE_AND_DESC g_vds_value_and_descs[];
extern struct field_attr _vds_sh_layout[];
extern struct field_attr vds_pbd_layout[];
extern struct field_attr vds_operation_layout[];
extern struct field_attr vds_block_shadow_layout[];

extern int g_dsm_DbgSwtich[ NR_SUBMODULE ];
extern int g_dsm_DbgLevel[ NR_SUBMODULE ];

extern BOOL g_IsFlashSimuStrictCheck;

extern int g_sector_size;
#ifndef _FS_SIMULATOR_
extern void sxr_Sleep( UINT32 );
#define VDS_SLEEP_VALUE 2
#endif

extern INT32 VDS_GetFlashInfo( VDS_FLASH_INFO *psInfo );


static UINT8 g_vds_char = 0;
UINT32 g_vds_part_id = 0;
INT32 g_vds_has_initiated = FALSE;

UINT8 g_caVbChar[ VDS_TEST_NR_VB ] = {0};
UINT8 g_caBufRead[ VDS_TEST_SZ_BLOCK ];
UINT8 g_caBufWrite[ VDS_TEST_SZ_BLOCK ];


INT32 vds_TstOpenFlash( VOID )
{
    INT32 fd = _open( FLASH_MAP_FILE, _O_CREAT | _O_BINARY | _O_RDWR,
                      _S_IREAD | _S_IWRITE );
    if ( -1 == fd )
    {
        return FALSE;
    }

    if ( VDS_TEST_NR_VB != _read( fd, g_caVbChar, VDS_TEST_NR_VB ) )
    {
        _close( fd );
        return ERR_TEST_FAILURE;
    }

    _close( fd );
    return ERR_SUCCESS;

}

INT32 vds_TstUpdateflashMap()
{
    INT32 fd = _open( FLASH_MAP_FILE, _O_CREAT | _O_BINARY | _O_RDWR,
                      _S_IREAD | _S_IWRITE );
    if ( -1 == fd )
    {
        return FALSE;
    }

    if ( VDS_TEST_NR_VB != _write( fd, g_caVbChar, VDS_TEST_NR_VB ) )
    {
        _close( fd );
        return ERR_TEST_FAILURE;
    }

    _close( fd );
    return ERR_SUCCESS;

}



UINT8 vds_TstGetRandOP( void )
{
    static UINT16 iIndex3 = 0;
    UINT8 iNum;
    UINT8 caRandNum[ 50 ] =
    {
        6, 0, 2, 0, 6, 7, 5, 5, 8, 6,
        4, 8, 1, 9, 2, 0, 2, 1, 3, 7,
        9, 1, 0, 5, 4, 9, 2, 1, 9, 6,
        1, 1, 5, 3, 4, 0, 9, 5, 1, 2,
        9, 8, 6, 4, 2, 3, 6, 4, 9, 1,
    };

    iNum = caRandNum[ iIndex3 ];
    iIndex3++;

    iNum %= VDS_TEST_NR_OP;

    if ( 50 == iIndex3 )
    {
        iIndex3 = 0;
    }

    return iNum;

}


UINT16 vds_TstGetRandVBN( void )
{
    static UINT16 iIndex2 = 0;
    UINT16 iNum;
    UINT16 caRandNum[ 100 ] =
    {
        346, 130, 230, 66, 392, 461, 187, 271, 420, 406,
        300, 222, 499, 351, 60, 336, 292, 97, 447, 471,
        495, 209, 22, 161, 494, 373, 44, 459, 443, 360,
        177, 195, 75, 41, 150, 510, 395, 491, 201, 220,
        57, 24, 306, 472, 260, 39, 238, 244, 231, 159,
        459, 100, 195, 108, 260, 378, 59, 98, 318, 465,
        469, 271, 450, 189, 78, 198, 253, 243, 318, 489,
        53, 117, 237, 374, 141, 193, 372, 468, 316, 314,
        280, 480, 266, 470, 483, 51, 268, 10, 132, 183,
        160, 499, 105, 105, 171, 453, 283, 368, 18, 377
    };

    iNum = caRandNum[ iIndex2 ];
    iIndex2++;

    iNum %= VDS_TEST_NR_VB;

    if ( 100 == iIndex2 )
    {
        iIndex2 = 0;
    }

    return iNum;

}


INT32 vds_TstEraseVB( UINT32 iPartId, UINT16 iVBN )
{
    INT32 iRet = 0;
    iPartId = iPartId;
    iVBN = iVBN;


    return iRet;
}



INT32 vds_TstWriteVB( UINT32 iPartId, UINT16 iVBN )
{
    INT32 iRet;

    DSM_MemSet( g_caBufWrite, g_vds_char, VDS_TEST_SZ_BLOCK );

    iRet = VDS_WriteBlock( iPartId, iVBN, g_caBufWrite );
    if ( ERR_SUCCESS == iRet )
    {
        g_caVbChar[ iVBN ] = g_vds_char;
        vds_TstUpdateflashMap();
        dsm_Printf( BASE_VDS_TS_ID, "data which repeats [0x%02X] written.\n", g_vds_char );
    }
    else
    {
        dsm_Printf( BASE_VDS_TS_ID, "VDS_WriteBlock return failed! errorCode = %d.\n\n", iRet );
    }
    g_vds_char++;

    return iRet;
}


INT32 vds_TstReadVB( UINT32 iPartId, UINT16 iVBN )
{
    INT32 iRet;

    DSM_MemSet( g_caBufRead, 0, VDS_TEST_SZ_BLOCK );
    DSM_MemSet( g_caBufWrite, g_caVbChar[ iVBN ], VDS_TEST_SZ_BLOCK );


    iRet = VDS_ReadBlock( iPartId, iVBN, g_caBufRead );
    if ( ERR_SUCCESS != iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "VDS_ReadBlock return failed! errorCode = %d.\n", iRet );
        return ERR_TEST_FAILURE;
    }

    if ( 0 == *g_caBufWrite )
    {
        dsm_Printf( BASE_VDS_TS_ID, "This VB is fresh!\n" );
        DSM_MemSet( g_caBufWrite, 0xff, VDS_TEST_SZ_BLOCK );
        if ( 0 == DSM_MemCmp( g_caBufWrite, g_caBufRead, VDS_TEST_SZ_BLOCK ) )
        {
            dsm_Printf( BASE_VDS_TS_ID, "character of bufRead is 0xff.\n\n" );
        }
        else
        {
            dsm_Printf( BASE_VDS_TS_ID, "Compare bufRead to 0xff failed.\n\n" );
            return ERR_TEST_FAILURE;
        }
        return iRet;
    }

    if ( 0 == DSM_MemCmp( g_caBufWrite, g_caBufRead, VDS_TEST_SZ_BLOCK ) )
    {
        dsm_Printf( BASE_VDS_TS_ID, "data which repeats [0x%02X] read\n", *g_caBufRead );
    }
    else
    {
        dsm_Printf( BASE_VDS_TS_ID, "data read is is \'0x%02X\', should be \'0x%02X\'.\n",
                    *g_caBufRead, *g_caBufWrite );
        dsm_Printf( BASE_VDS_TS_ID, "Read failed\n" );
        return ERR_TEST_FAILURE;
    }
    return iRet;

}

static BOOL shell_vds_Read( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iSVbn = 0;
    UINT16 iNum = 0;
    UINT16 i;
    INT32 iRet;

    if ( 3 == argc || 2 == argc )
    {
        iSVbn = (UINT16)DSM_StrAToI( argv[ 1 ] );
        if ( 2 == argc )
        {
            iNum = 1;
        }
        else
        {
            iNum = (UINT16)DSM_StrAToI( argv[ 2 ] );
        }

        for ( i = 0; i < iNum; ++i )
        {
            iRet = vds_TstReadVB( g_vds_part_id, ( UINT16 ) ( iSVbn + i ) );
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;

}


static BOOL shell_vds_Erase( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iSVbn = 0;
    UINT16 iNum = 0;
    UINT16 i;
    INT32 iRet;

    if ( 3 == argc || 2 == argc )
    {
        iSVbn = (UINT16)DSM_StrAToI( argv[ 1 ] );
        if ( 2 == argc )
        {
            iNum = 1;
        }
        else
        {
            iNum = (UINT16)DSM_StrAToI( argv[ 2 ] );
        }

        for ( i = 0; i < iNum; ++i )
        {
            iRet = vds_TstEraseVB( g_vds_part_id, ( UINT16 ) ( iSVbn + i ) );
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;

}



static BOOL shell_vds_Write( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iSVbn = 0;
    UINT16 iNum = 0;
    UINT16 i;
    INT32 iRet;

    if ( 3 == argc || 2 == argc )
    {
        iSVbn = (UINT16)DSM_StrAToI( argv[ 1 ] );
        if ( 2 == argc )
        {
            iNum = 1;
        }
        else
        {
            iNum = (UINT16)DSM_StrAToI( argv[ 2 ] );
        }

        for ( i = 0; i < iNum; ++i )
        {
            iRet = vds_TstWriteVB( g_vds_part_id, ( UINT16 ) ( iSVbn + i ) );
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;

}


static BOOL shell_vds_EraseRandom( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iNumToErase;
    UINT16 iRandVBN;
    UINT16 i;
    INT32 iRet;

    if ( 2 == argc )
    {
        iNumToErase = (UINT16)DSM_StrAToI( argv[ 1 ] );

        for ( i = 0; i < iNumToErase; ++i )
        {
            iRandVBN = vds_TstGetRandVBN();
            iRet = vds_TstEraseVB( g_vds_part_id, iRandVBN );
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;

}


static BOOL shell_vds_WriteRandom( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iNumToWrtie;
    UINT16 iRandVBN;
    UINT16 i;
    INT32 iRet;

    if ( 2 == argc )
    {
        iNumToWrtie = (UINT16)DSM_StrAToI( argv[ 1 ] );

        for ( i = 0; i < iNumToWrtie; ++i )
        {
            iRandVBN = vds_TstGetRandVBN();
            iRet = vds_TstWriteVB( g_vds_part_id, iRandVBN );
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;
}


static BOOL shell_vds_ReadRandom( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iNumToRead;
    UINT16 iRandVBN;
    UINT16 i;
    INT32 iRet;

    if ( 2 == argc )
    {
        iNumToRead = (UINT16)DSM_StrAToI( argv[ 1 ] );

        for ( i = 0; i < iNumToRead; ++i )
        {
            iRandVBN = vds_TstGetRandVBN();
            iRet = vds_TstReadVB( g_vds_part_id, iRandVBN );
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;
}


static BOOL shell_vds_RandomOperate( struct cmd_entry *ent, int argc, char **argv )
{
    UINT16 iCounter;
    UINT16 iRandVBN;
    UINT8 iOP;
    UINT16 i;
    INT32 iRet = -1;

    if ( 2 == argc )
    {
        iCounter = (UINT16)DSM_StrAToI( argv[ 1 ] );

        for ( i = 0; i < iCounter; ++i )
        {
            iRandVBN = vds_TstGetRandVBN();
            iOP = vds_TstGetRandOP();
            switch ( iOP )
            {
                case 0:
                    iRet = vds_TstWriteVB( g_vds_part_id, iRandVBN );
                    break;
                case 1:
                    iRet = vds_TstReadVB( g_vds_part_id, iRandVBN );
                    break;
                case 2:
                    break;
                default:
                    ;
            }
            if ( iRet < 0 )
            {
                return FALSE;
            }
        }
    }
    else
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    return TRUE;
}


//init [ON | OFF] [YES | NO] [sector_size] [nr_sector_in_virtual_disk] [blk_size]

static BOOL shell_vds_init( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if ( g_vds_has_initiated )
    {
        dsm_Printf( BASE_VDS_TS_ID, "Init command is not allowed to call from shell\n" );
        return FALSE;
    }

    if ( 6 != argc )
    {
        goto show_init_cmd;
    }

    //   g_dsm_DbgSwtich[ DSM_MOD_VDS ] = 1;
    // g_dsm_DbgLevel[ DSM_MOD_VDS ] = ( 0 == DSM_StrCaselessCmp( "on", argv[ 1 ] ) ) ? DL_DETAIL : DL_WARNING;

    g_vds_flash_info.bHasInit = TRUE;
    g_vds_flash_info.iFlashSectorSize = g_sector_size
                                        = DSM_StrAToI( argv[ 3 ] ) * 1024;
    g_vds_flash_info.iFlashStartAddr = 0;
    g_vds_flash_info.iNrSector = DSM_StrAToI( argv[ 4 ] );

    dsm_Printf( BASE_DSM_TS_ID, "Flash region belonging to VDS: start_addr: 0x%x, sector_size: 0x%x(%d K), nr_sector: %d\n",
                g_vds_flash_info.iFlashStartAddr, g_vds_flash_info.iFlashSectorSize,
                g_vds_flash_info.iFlashSectorSize / 1024, g_vds_flash_info.iNrSector );
    if ( 0 == DSM_StrCaselessCmp( "yes", argv[ 2 ] ) )
    {
        /*  iRet = VDS_PartitionFormat( (PCSTR)"vds0", TRUE,
                                       DEFAULT_NR_SEC_OF_VDS2,
                                       g_vds_flash_info.iNrSector - DEFAULT_NR_SEC_OF_VDS2,
                                       512 );
          if (ERR_SUCCESS != iRet )
          {
              dsm_Printf( BASE_DSM_TS_ID, "Format partition[vds0] failed. iRet = %d!\n", iRet );
              return FALSE;
          }
          dsm_Printf( BASE_DSM_TS_ID, "Format VDS partition[vds0] OK\n" );
          iRet = VDS_PartitionFormat( (PCSTR)"vds1", TRUE,
                                       0, DEFAULT_NR_SEC_OF_VDS2, 512 );
          if (ERR_SUCCESS != iRet )
          {
              dsm_Printf( BASE_DSM_TS_ID, "Format partition[vds1] failed. iRet = %d!\n", iRet );
              return FALSE;
          }
          dsm_Printf( BASE_DSM_TS_ID, "Format VDS partition[vds1] OK\n" );
          vds_TstUpdateflashMap();
          */
        iRet = VDS_Init();
        if (ERR_SUCCESS != iRet )
        {
            dsm_Printf( BASE_DSM_TS_ID, "Vds init failed. iRet = %d!\n", iRet );
            return FALSE;
        }
        vds_TstUpdateflashMap();
    }

    if ( ERR_SUCCESS != VDS_Init())
    {
        dsm_Printf( BASE_DSM_TS_ID, "Initiate VDS failed!\n" );
        return FALSE;
    }
    else
    {
        dsm_Printf( BASE_DSM_TS_ID, "Initiate VDS OK\n" );
    }

    DSM_DevInit();

    g_vds_has_initiated = TRUE;
    vds_TstOpenFlash();
    return TRUE;

show_init_cmd:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}

/*format <partition_name> <first_sector_index> <number_of_sectors> <block_size> {[1|0]}*/
static BOOL shell_vds_format( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    if ( argc != 2 )
    {
        goto show_format_cmd;
    }

    // Get flash information.
    iRet = VDS_GetFlashInfo( &g_vds_flash_info );
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_ERROR, "vds_Init_Entry: GetFlashInfo failed[%d].", iRet ));
        return FALSE;
    }

    iRet = VDS_FormatPartition( (PCSTR)argv[ 1 ]);
    dsm_Printf( BASE_VDS_TS_ID, "Format partition[%s] %s!\n",
                argv[ 1 ], ( ERR_SUCCESS == iRet ) ? "OK" : "failed" );

    if( ERR_SUCCESS == iRet )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

show_format_cmd:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}


static BOOL shell_get_partition_info( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 iNrBlk;
    UINT32 iBlkSize;
    UINT32 iPartId;
    INT32 iRet;

    if ( 2 != argc )
    {
        goto show_format_cmd;
    }

    iPartId = DSM_StrAToI( argv[ 1 ] );
    iRet = VDS_GetPartitionInfo( iPartId, &iNrBlk, &iBlkSize );
    if ( ERR_SUCCESS == iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "number of block: %d\n", iNrBlk );
        dsm_Printf( BASE_VDS_TS_ID, "block size     : %d Bytes\n", iBlkSize );

        dsm_Printf( BASE_VDS_TS_ID, "sector size    : 0x%x(%d)\n", g_vds_flash_info.iFlashSectorSize, g_vds_flash_info.iFlashSectorSize );

        return TRUE;
    }
    else
    {
        dsm_Printf( BASE_VDS_TS_ID, "Get partition[%d] information failed!\n", iPartId );
        return FALSE;
    }

show_format_cmd:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}



static BOOL shell_get_flash_info( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iRet;

    argc = argc;
    argv = argv;
    ent = ent;

    iRet = VDS_GetFlashInfo( &g_vds_flash_info );
    if ( ERR_SUCCESS != iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "get flash information failed\n" );
        return FALSE;

    }

    dsm_Printf( BASE_VDS_TS_ID, "start addr   : %d\n", g_vds_flash_info.iFlashStartAddr );
    dsm_Printf( BASE_VDS_TS_ID, "sector size  : %d\n", g_vds_flash_info.iFlashSectorSize );
    dsm_Printf( BASE_VDS_TS_ID, "sector number: %d\n", g_vds_flash_info.iNrSector );

    return TRUE;
}

static BOOL shell_set( struct cmd_entry *ent, int argc, char **argv )
{
    if ( argc >= 3 )
    {
        switch ( argv[ 1 ][ 0 ] )
        {
            case 'p':
                g_vds_part_id = DSM_StrAToI( argv[ 2 ] );
                break;

            default:
                break;
        }
        return TRUE;
    }

    PRINT_CMD_USAGE( ent );
    return FALSE;
}

//flashchk [on|off]
static BOOL shell_flash_check_switch( struct cmd_entry *ent, int argc, char **argv )
{
    if ( argc >= 2 )
    {
        if ( 0 == DSM_StrCaselessCmp( argv[ 1 ], "on" ) )
        {
            g_IsFlashSimuStrictCheck = TRUE;
        }
        else if ( 0 == DSM_StrCaselessCmp( argv[ 1 ], "off" ) )
        {
            g_IsFlashSimuStrictCheck = FALSE;
        }
        else
        {
            goto show_flash_switch_cmd;
        }

        return TRUE;
    }

show_flash_switch_cmd:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}

/* export <filename> {[vpn] [counter]}  */
static BOOL shell_export( struct cmd_entry *ent, int argc, char **argv )
{
    VDS_PARTITION *psCP;
    FILE *file;
    UINT16 iVbn;
    UINT32 iCounter;
    UINT32 iNrBlk;
    UINT32 iBlkSize;
    UINT16 i;
    INT32  size_write;
    INT32 iRet;
    UINT8 *pBuff = NULL;

    if ( argc < 2 )
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    psCP = &g_VDS_partition_table[ g_vds_part_id ];

    iRet = VDS_GetPartitionInfo( g_vds_part_id, &iNrBlk, &iBlkSize );
    if ( ERR_SUCCESS != iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "Get partition information failed\n" );
        return FALSE;
    }


    iVbn = (UINT16)(( argc >= 3 ) ? DSM_StrAToI( argv[ 2 ] ) : 0);
    if ( iVbn > iNrBlk )
    {
        dsm_Printf( BASE_VDS_TS_ID, "Invalid vbn\n" );
        return FALSE;
    }

    iCounter = ( argc >= 4 ) ? DSM_StrAToI( argv[ 3 ] ) : 0;
    if ( 0 == iCounter )
    {
        iCounter = iNrBlk;
    }

    if ( iVbn + iCounter > iNrBlk )
    {
        iCounter = iNrBlk - iVbn;
    }
    if ( 0 == iCounter )
    {
        dsm_Printf( BASE_VDS_TS_ID, "No block is exported!\n" );

        return FALSE;
    }

    file = fopen( argv[ 1 ], "wb" );
    if ( NULL == file )
    {
        dsm_Printf( BASE_VDS_TS_ID, "open the file on the host failed\n" );
        return FALSE;
    }
    pBuff = DSM_MAlloc(psCP->sPartInfo.iVBSize);
    for ( i = 0; i < iCounter; i++ )
    {
        iRet = VDS_ReadBlock( g_vds_part_id, iVbn + i, pBuff );
        if ( ERR_SUCCESS != iRet )
        {
            break;
        }

        size_write = fwrite(pBuff, 1, iBlkSize, file );
        if ( size_write != (INT32)iBlkSize )
        {
            iRet = iRet;
            break;
        }
    }

    fclose( file );
    if ( ERR_SUCCESS == iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "export VB[%d --> %d] to the file[%s] on the host\n",
                    iVbn, iVbn + iCounter, argv[ 1 ] );
        return TRUE;
    }
    else
    {
        dsm_Printf( BASE_VDS_TS_ID, "export failed!\n" );
        return FALSE;
    }
    if(pBuff)
    {
        DSM_Free(pBuff);
    }
}


/* fwrite [operation_file] */
static BOOL shell_fwrite( struct cmd_entry *ent, int argc, char **argv )
{
    VDS_PARTITION *psCP;
    FILE *file;
    FILE *file_shadow;
    UINT32 iNrBlk;
    UINT32 iBlkSize;
    UINT8 file_shadow_name[ 32 ];
    VDS_Operation sOper;
    VDS_BlockShadow sShadow;
    INT32 iRet;
    UINT8 *pBuff = NULL;

    /*
    UINT16 x = 0xFFFF;
    INT16  y = 0xFFFF;

    dsm_Printf( BASE_VDS_TS_ID, "0x%x\n", x + 1  );
    dsm_Printf( BASE_VDS_TS_ID, "0x%x\n", y + 1  );
    */


    if ( argc < 2 )
    {
        PRINT_CMD_USAGE( ent );
        return FALSE;
    }

    psCP = &g_VDS_partition_table[ g_vds_part_id ];

    iRet = VDS_GetPartitionInfo( g_vds_part_id, &iNrBlk, &iBlkSize );
    if ( ERR_SUCCESS != iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "Get partition information failed\n" );
        return FALSE;
    }

    file = fopen( argv[ 1 ], "rb+" );
    if ( NULL == file )
    {
        dsm_Printf( BASE_VDS_TS_ID, "open the operation file on the host failed\n" );
        return FALSE;
    }

    DSM_StrCpy( file_shadow_name, argv[ 1 ] );
    DSM_StrCat( file_shadow_name, ".shadow" );

    file_shadow = fopen( (const char *)file_shadow_name, "wb" );
    if ( NULL == file_shadow )
    {
        dsm_Printf( BASE_VDS_TS_ID, "open the shadow file on the host failed\n" );
        iRet = -1;
        goto step0_failed;
    }

    pBuff = DSM_MAlloc(psCP->sPartInfo.iVBSize);
    iRet = fread( ( INT8 * ) &sOper, 1, sizeof( sOper ), file );
    while ( sizeof( sOper ) == iRet )
    {
        fseek( file, -iRet, SEEK_CUR );
        sOper.iOperType %= VDS_OPERTYPE_COUNTER;
        sOper.iOperType += VDS_OPERTYPE_BASE;
        sOper.iVbn %= iNrBlk;
        iRet = fwrite( ( INT8 * ) & sOper, 1, sizeof( sOper ), file );
        if ( iRet <= 0 )
        {
            dsm_Printf( BASE_VDS_TS_ID, "write the operation file on the host failed\n" );
            iRet = -1;
            goto step1_failed;
        }

        if ( VDS_IS_OPERTYPE( sOper.iOperType, VDS_OPERTYPE_WRITE ) )
        {
            /*
            for ( i = 0; i < iBlkSize / sizeof(INT32); i++ )
            {
              UINT32 *pi = (UINT32*)psCP->pPBBuff;
              *pi = sOper.iData + i;
            }
            */
            *( UINT32 * ) pBuff = sOper.iData;

            dsm_Printf( BASE_VDS_TS_ID, "write:  vbn, data: %06d, 0x%X\n", sOper.iVbn, sOper.iData );
            iRet = VDS_WriteBlock( g_vds_part_id, ( UINT16 ) ( sOper.iVbn ), pBuff );
            if ( ERR_SUCCESS != iRet )
            {
                dsm_Printf( BASE_VDS_TS_ID, "write vds block failed\n" );
                iRet = -1;
                goto step1_failed;
            }

            fseek( file_shadow, ( sOper.iVbn % (UINT32)iNrBlk ) * sizeof( VDS_BlockShadow ), SEEK_SET );
            sShadow.iVBStatus = FALSE;
            sShadow.iData = sOper.iData;
            iRet = fwrite( ( INT8 * ) & sShadow, 1, sizeof( sShadow ), file_shadow );
            if ( iRet <= 0 )
            {
                dsm_Printf( BASE_VDS_TS_ID, "write the shadow file on the host failed\n" );
                iRet = -1;
                goto step1_failed;
            }
        }

        iRet = fread( ( INT8 * ) & sOper, 1, sizeof( sOper ), file );
    }

    iRet = 0;

step1_failed:
    fclose( file_shadow );
step0_failed:
    fclose( file );
    if ( 0 == iRet )
    {
        dsm_Printf( BASE_VDS_TS_ID, "write vds according the host file[%s] OK\n", argv[ 1 ] );
        return TRUE;
    }
    else
    {
        dsm_Printf( BASE_VDS_TS_ID, "write vds according the host file[%s] failed\n", argv[ 1 ] );
        return FALSE;
    }
    if(pBuff)
    {
        DSM_Free(pBuff);
    }
}



/* hdump [filename] [operation | shadow] [index] {[counter]} */
static BOOL shell_dump_host_file( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 index;
    INT32 counter;
    INT32 i;
    VDS_Operation sOper;
    VDS_BlockShadow sShadow;
    FILE *file;

    if ( argc < 3 )
        goto dump_cmd_help;

    file = fopen( argv[ 1 ], "rb" );
    if ( NULL == file )
    {
        dsm_Printf( BASE_VDS_TS_ID, "open the operation file on the host failed\n" );
        return FALSE;
    }

    index = ( argc >= 4 ) ? DSM_StrAToI( argv[ 3 ] ) : 0;
    counter = ( argc >= 5 ) ? DSM_StrAToI( argv[ 4 ] ) : 1;


    switch ( argv[ 2 ][ 0 ] )
    {
        case 's':  /* shadow */
            for ( i = index; i < index + counter; i++ )
            {
                dsm_Printf( BASE_VDS_TS_ID, "\n--------------------VBN[#%d]--------------------\r\n", i );
                vds_DumpObject( file, i * sizeof( VDS_BlockShadow ), sizeof( VDS_BlockShadow ),
                                ( UINT8 * ) & sShadow, vds_block_shadow_layout, 2 );
            }

            break;

        case 'o':  /* operation */
            for ( i = index; i < index + counter; i++ )
            {
                dsm_Printf( BASE_VDS_TS_ID, "\n--------------------Index[#%d]--------------------\r\n", i );
                vds_DumpObject( file, i * sizeof( VDS_Operation ), sizeof( VDS_Operation ),
                                ( UINT8 * ) & sOper, vds_operation_layout, 3 );
            }
            break;
        default:
            goto dump_cmd_help;
    }

    fclose( file );
    return TRUE;

dump_cmd_help:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}


static BOOL shell_show_ds( struct cmd_entry *ent, int argc, char **argv )
{
    UINT32 index;
    INT32 counter;
    UINT32 i;
    INT32 iListId;
    INT32 iSectId;

    if ( argc < 2 )
        goto show_cmd_help;

    switch ( argv[ 1 ][ 0 ] )
    {
        case 'p':  /* Partitions */
            if ( argc >= 3 )
            {
                index = DSM_StrAToI( argv[ 2 ] );
                vds_ShowPartition( index );
            }
            else
            {
                for ( i = 0; i < VDS_GetPartitionCount(); i++ )
                    vds_ShowPartition( i );
            }

            break;

        case 'l':  /* sector reference List */
            if ( argc >= 3 )
            {
                if ( 'd' == argv[ 2 ][ 0 ] )
                    iListId = VDS_DIRTY_LIST_ID;
                else if ( 'f' == argv[ 2 ][ 0 ] )
                    iListId = VDS_FREE_LIST_ID;
                else if ( 'c' == argv[ 2 ][ 0 ] )
                    iListId = VDS_CURRENT_CANDIDATE_LIST_ID;
                else
                    goto show_cmd_help;

                vds_ShowSectRefList( g_vds_part_id, iListId );
            }
            else
            {
                dsm_Printf( BASE_VDS_TS_ID, "--------------------dirty list--------------------\r\n" );
                vds_ShowSectRefList( g_vds_part_id, VDS_DIRTY_LIST_ID );
                dsm_Printf( BASE_VDS_TS_ID, "--------------------free list---------------------\r\n" );
                vds_ShowSectRefList( g_vds_part_id, VDS_FREE_LIST_ID );
                dsm_Printf( BASE_VDS_TS_ID, "--------------current candidate list--------------\r\n" );
                vds_ShowSectRefList( g_vds_part_id, VDS_CURRENT_CANDIDATE_LIST_ID );
            }

            break;

        case 's':  /* Sector */
            if ( argc >= 3 )
            {
                if ( 'c' == argv[ 2 ][ 0 ] )
                    iSectId = VDS_CURRENT_SECTOR_ID;
                else if ( 'g' == argv[ 2 ][ 0 ] )
                    iSectId = VDS_GCING_SECTOR_ID;
                else if ( 'r' == argv[ 2 ][ 0 ] )
                    iSectId = VDS_REMAIN_SECTOR_ID;
                else
                    goto show_cmd_help;

                vds_ShowSectRef( g_vds_part_id, iSectId );
            }
            else
            {
                vds_ShowSectRef( g_vds_part_id, VDS_CURRENT_SECTOR_ID );
                vds_ShowSectRef( g_vds_part_id, VDS_GCING_SECTOR_ID );
                vds_ShowSectRef( g_vds_part_id, VDS_REMAIN_SECTOR_ID );


            }
            break;

        case 'm':  /* Map table */
            index = ( argc >= 3 ) ? DSM_StrAToI( argv[ 2 ] ) : 0;
            counter = ( argc >= 4 ) ? DSM_StrAToI( argv[ 3 ] ) : 1;

            vds_ShowMT( g_vds_part_id, index, counter );
            break;

        default:
            goto show_cmd_help;
    }

    return TRUE;

show_cmd_help:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}


static BOOL shell_dump_flash( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 index;
    INT32 counter;
    INT32 i;
    UINT16 sn, pbn, vbn;
    VDS_SH sh;
    VDS_PARTITION *psCP;
    VDS_SECT_REF *psRef = NULL;

    if ( argc < 2 )
        goto dump_cmd_help;

    switch ( argv[ 1 ][ 0 ] )
    {
        case 's':  /* Sector header */
            index = ( argc >= 3 ) ? DSM_StrAToI( argv[ 2 ] ) : 0;
            counter = ( argc >= 4 ) ? DSM_StrAToI( argv[ 3 ] ) : 1;

            psCP = &g_VDS_partition_table[ g_vds_part_id ];

            psRef = psCP->pDirtyList;
            while(psRef)
            {
                dsm_Printf( BASE_VDS_TS_ID, "\n--------------------Sector NO[#%d]--------------------\r\n", psRef->iSN);
                vds_DumpObject( NULL, VDS_ADDR_SECT(psRef->iSN), VDS_SZ_SH, ( UINT8 * ) & sh, _vds_sh_layout, 11 );
                psRef = psRef->next;
            }

            psRef = psCP->pCurrentSect;
            // for ( i = index; i < index + counter; i++ )
            while(psRef)
            {
                dsm_Printf( BASE_VDS_TS_ID, "\n--------------------Sector NO[#%d]--------------------\r\n", psRef->iSN);
                vds_DumpObject( NULL, VDS_ADDR_SECT(psRef->iSN), VDS_SZ_SH, ( UINT8 * ) & sh, _vds_sh_layout, 11 );
                psRef = psRef->next;
            }

            break;

        case 'p':  /* Pbdt */
            if ( argc < 3 )
                goto dump_cmd_help;

            sn = (UINT16)DSM_StrAToI( argv[ 2 ] );
            pbn = (UINT16)(( argc >= 4 ) ? DSM_StrAToI( argv[ 3 ] ) : 0);
            counter = ( argc >= 5 ) ? DSM_StrAToI( argv[ 4 ] ) : 1;

            vds_DumpPBDT( g_vds_part_id, sn, pbn, counter );
            break;

        case 'b':  /* Block */
            if ( argc < 4 )
                goto dump_cmd_help;

            sn = (UINT16)DSM_StrAToI( argv[ 2 ] );
            pbn = (UINT16)DSM_StrAToI( argv[ 3 ] );
            counter = ( argc >= 5 ) ? DSM_StrAToI( argv[ 4 ] ) : 1;

            for ( i = 0; i < counter; i++ )
            {
                vds_DumpBlock( g_vds_part_id, sn, ( UINT16 ) ( pbn + i ), 0, 0 );
            }
            break;

        case 'v':  /* Virtual lock */
            if ( argc < 3 )
                goto dump_cmd_help;

            vbn = (UINT16)DSM_StrAToI( argv[ 2 ] );
            counter = ( argc >= 4 ) ? DSM_StrAToI( argv[ 3 ] ) : 1;

            psCP = &g_VDS_partition_table[ g_vds_part_id ];
            for ( i = 0; i < counter && ( UINT16 ) ( vbn + i ) < VDS_NR_MT_VALID_ENTRY( psCP ); i++ )
            {
                vds_DumpBlock( g_vds_part_id, psCP->psMT[ vbn + i ].iSN, psCP->psMT[ vbn + i ].iPBN, 0, 0 );
            }
            break;
        default:
            goto dump_cmd_help;
    }

    return TRUE;

dump_cmd_help:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}



static BOOL shell_raw_write_flash( struct cmd_entry *ent, int argc, char **argv )
{
    INT32 iObjAddr;
    UINT16 sn, pbn = 0xFFFF;
    UINT32 iLen, iOffset = 0, iData, iPBAddr, iWSize, iRSize;
    INT32 iFieldIndex;
    UINT32 iCRC;
    UINT8 strCRC[ 16 ];
    struct field_attr *pFieldAttr;
    VDS_SH sSH;
    VDS_PARTITION *psCP;
    INT32 iRet;
    UINT8 *pBuff = NULL;

    if ( argc <= 4
            || ( ( 's' == argv[ 1 ][ 0 ] ) && ( 'c' != argv[ 3 ][ 0 ] ) && ( argc < 5 ) )
            || ( ( 'p' == argv[ 1 ][ 0 ] ) && ( 'p' != argv[ 4 ][ 0 ] ) && ( argc < 6 ) )
       )
        goto rw_cmd_help;

    psCP = &g_VDS_partition_table[ g_vds_part_id ];
    sn = (UINT16)DSM_StrAToI( argv[ 2 ] );
    if ( FALSE == vds_IsValidSn(psCP, sn))
    {
        dsm_Printf( BASE_VDS_TS_ID, "sector number overflow!\n" );
        return FALSE;
    }


    if ( 's' != argv[ 1 ][ 0 ] )
    {
        pbn = (UINT16)DSM_StrAToI( argv[ 3 ] );
        if ( pbn >= ( UINT16 ) VDS_NR_PB_PER_SECT( psCP ) )
        {
            dsm_Printf( BASE_VDS_TS_ID, "pbn overflow!\n" );
            return FALSE;
        }
    }
    pBuff = DSM_MAlloc(psCP->sPartInfo.iVBSize);
    switch ( argv[ 1 ][ 0 ] )
    {
        case 's':  /* Sector header */
            iObjAddr = VDS_ADDR_SECT(sn );

            if ( VDS_FIELD_NOT_FOUND == vds_FindField( argv[ 3 ], _vds_sh_layout, 5, &iFieldIndex ) )
                goto rw_cmd_help;

            pFieldAttr = &_vds_sh_layout[ iFieldIndex ];
            if ( 4 == argc )
            {
                if ( 0 == DSM_StrNCaselessCmp( pFieldAttr->name, "crc", DSM_StrLen( "crc" ) ) )
                {
                    iPBAddr = VDS_ADDR_PB( psCP, sn ) + pbn * VDS_SZ_PB( psCP );

                    iRet = DRV_ReadFlash( VDS_ADDR_SECT(sn ), ( UINT8 * ) & sSH, VDS_SZ_SH, &iRSize );
                    if ( ( iRet != ERR_SUCCESS ) || ( iRSize != VDS_SZ_SH ) )
                    {
                        return FALSE;
                    }
                    iCRC = vds_CRC32( ( UINT8 * ) & sSH, VDS_SZ_SH );
                    iCRC = DSM_HTON32( iCRC );
                    vds_hex_to_str( ( INT8 * ) & iCRC, 4 , (char *)strCRC, NULL_CHAR );
                }
                else
                {
                    if(pBuff)
                    {
                        DSM_Free(pBuff);
                        pBuff = NULL;
                    }
                    return FALSE;
                }
            }
            vds_RawWriteObject( iObjAddr, pFieldAttr, ( argc > 4 ) ? ( UINT8 * ) argv[ 4 ] : strCRC );
            break;

        case 'p':  /* Pbdt */
            iObjAddr = VDS_ADDR_PBDT( psCP, sn ) + pbn * VDS_SZ_PBD;
            if ( VDS_FIELD_NOT_FOUND == vds_FindField( argv[ 4 ], vds_pbd_layout, 5, &iFieldIndex ) )
                goto rw_cmd_help;

            pFieldAttr = &vds_pbd_layout[ iFieldIndex ];
            if ( 5 == argc )
            {
                if ( 0 == DSM_StrNCaselessCmp( pFieldAttr->name, "pbcrc", DSM_StrLen( "pbcrc" ) ) )
                {
                    //cal crc for pb
                    //read pb to buff
                    iPBAddr = VDS_ADDR_PB( psCP, sn ) + pbn * VDS_SZ_PB( psCP );

                    iRet = DRV_ReadFlash( iPBAddr, pBuff, VDS_SZ_PB( psCP ), &iRSize );
                    if ( ( iRet != ERR_SUCCESS ) || ( iRSize != VDS_SZ_PB( psCP ) ) )
                    {
                        if(pBuff)
                        {
                            DSM_Free(pBuff);
                            pBuff = NULL;
                        }
                        return FALSE;
                    }
                    iCRC = vds_CRC32( pBuff, VDS_SZ_PB( psCP ) );
                    iCRC = DSM_HTON32( iCRC );
                    vds_hex_to_str( ( INT8 * ) & iCRC, 4 , (char *)strCRC, NULL_CHAR );
                }
                else
                {
                    //cal crc for pbd
                }
            }
            vds_RawWriteObject( iObjAddr, pFieldAttr, ( argc > 5 ) ? ( UINT8 * ) argv[ 5 ] : strCRC );
            break;

        case 'b':  /* Block */
            if ( 6 == argc )
            {
                iOffset = DSM_StrAToI( argv[ 4 ] );
            }

            vds_str_to_hex( 6 == argc ? argv[ 5 ] : argv[ 4 ], (INT32 *)&iLen, NULL );
            if ( iLen > 4 )
                goto rw_cmd_help;

            if ( iOffset + iLen > VDS_SZ_PB( psCP ) )
            {
                iLen = VDS_SZ_PB( psCP ) - iOffset;
            }

            vds_str_to_hex( 6 == argc ? argv[ 5 ] : argv[ 4 ], (INT32 *)&iLen, ( INT8 * ) & iData );

            iPBAddr = VDS_ADDR_PB( psCP, sn ) + pbn * VDS_SZ_PB( psCP );

            iRet = DRV_WriteFlash( iPBAddr + iOffset, ( UINT8 * ) & iData, iLen, &iWSize );
            if ( ( iRet != ERR_SUCCESS ) || ( iLen != iWSize ) )
            {
                dsm_Printf( BASE_VDS_TS_ID, "Write block failed!\n" );
                if(pBuff)
                {
                    DSM_Free(pBuff);
                    pBuff = NULL;
                }
                return FALSE;
            }

            break;

        default:
            goto rw_cmd_help;
    }
    if(pBuff)
    {
        DSM_Free(pBuff);
        pBuff = NULL;
    }
    return TRUE;

rw_cmd_help:
    PRINT_CMD_USAGE( ent );
    return FALSE;
}

INT32 vds_TstGetFlashInfo(UINT32 *iSecCnt, UINT32 *iSecSz)
{
    VDS_FLASH_INFO sFlashInfo;
    INT32 iRet;

    iRet = VDS_GetFlashInfo( &sFlashInfo);
    if ( ERR_SUCCESS != iRet )
    {
        D( ( DL_ERROR, "vds_TstGetFlashInfo: get flash information failed err code = %d.", iRet ));
        return iRet;
    }
    *iSecCnt = sFlashInfo.iNrSector;
    *iSecSz = sFlashInfo.iFlashSectorSize;
    return iRet;
}
#define CMD_SHOW_USAGE  \
  "\n\tshow partitions { [ partid ] }\n" \
  "\tshow list { [ dirty | free | current ] }\n" \
  "\tshow sector { [ gc | current | remain ] }\n" \
  "\tshow  mt { [ index ] { [ counter ] } }\n"

#define CMD_DUMP_USAGE  \
  "\n\tdump sh { [ sn ] { [ counter ] } }\n" \
  "\tdump pbdt [ sn ] { [ pbn ] { [ counter ] } }\n" \
  "\tdump block [ sn ] [ pbn ] { [ counter ] }\n" \
  "\tdump vb [ vbn ] { [ counter ] }\n"
//  "\tdump block [ sn ] [ pbn ] [ -offset offset ] { [ -len len ] }\n"

#define CMD_RAW_WRITE_USAGE \
"\n\trw sh [ sn ] magic [hex_value] \n" \
"\trw sh [ sn ] status [FREE | DIRTY | CURRENT | REMAIN | CURCAND | GCING]\n" \
"\trw sh [ sn ] status [[hex_value]]\n" \
"\trw sh [ sn ] erase_counter  [erase_counter]\n" \
"\trw sh [ sn ] crc [hex_CRC_value]\n" \
"\n" \
"\trw pbd [ sn ] [ pbn ] status [VALID | INVALID | FRESH]\n" \
"\trw pbd [ sn ] [ pbn ] status [hex_value]\n" \
"\trw pbd [ sn ] [ pbn ] vbn [vbn]\n" \
"\trw pbd [ sn ] [ pbn ] version [version]\n" \
"\trw pbd [ sn ] [ pbn ] pbdcrc [hex_CRC_value]\n" \
"\trw pbd [ sn ] [ pbn ] pbcrc [hex_CRC_value]\n" \
"\n" \
"\trw block [ sn ] [ pbn ] {[offset]} [data]\n"

#define CMD_INIT_USAGE \
"\tinit [ON | OFF] [YES | NO] [sector_size] [nr_sector_in_partition] [blk_size]\n"


struct cmd_entry g_VdsCmdEnts[ ] =
{
    { "ms", shell_module_switch, "module switch for running specified module's command", "ms [FAT|VDS|REG]" },
    { "rep", shell_repeat, "do something repetitively, %LOOP% is the name for loop variable", "rep ( base, step, counter ) command line" },
    { "debug", shell_debug, "debug switch", "debug [VDS|FAT] [on | off] [error | brief | detail]" },
    { "brep", shell_block_repeat, "do something for block repetitively", "brep {[-F]} <counter> {command line}" },
    { "help", shell_help, "print help message", NULL },
    { "?", shell_help, "print help message", NULL },

    { "init", shell_vds_init, "initiate the vds system, including on/off debug switch, \n\tformat or not, if format, parameters is given", CMD_INIT_USAGE },
    { "format", shell_vds_format, "foramt partition", "format <partition_name> {[1|0]}" },
    { "set", shell_set, "set parameters in VDS", "set partid [ partid ]" },
    { "version", shell_version, "show the version of the module", "version [vds | fat |vfs | reg | sms]" },

    { "export", shell_export, "export vds data to host",  "export <filename> {[vbn] [counter]} " },
    { "fwrite", shell_fwrite, "write vds according to the host file",  "fwrite [operation_file]" },
    { "hdump", shell_dump_host_file, "dump host file",  "hdump [filename] [operation | shadow] [index] {[counter]}" },

    { "show", shell_show_ds, "show data structure in memory", CMD_SHOW_USAGE },
    { "dump", shell_dump_flash, "dump data from flash", CMD_DUMP_USAGE },
    { "rw", shell_raw_write_flash, "write flash by raw interface rather than vds interface", CMD_RAW_WRITE_USAGE },


    { "erase", shell_vds_Erase, "erase block", "erase [vbn] [counter]"},
    { "read", shell_vds_Read, "read block", "read [vbn] [counter]"},
    { "write", shell_vds_Write, "write block", "write [vbn] [counter]"},
    { "rerase", shell_vds_EraseRandom, "erase VB randomly", "rerase [counter]"},
    { "rread", shell_vds_ReadRandom, "read VB randomly", "rread [counter]"},
    { "rwrite", shell_vds_WriteRandom, "write VB randomly", "rwrite [counter]"},
    { "rr", shell_vds_RandomOperate, "Operate(write/read/erase) randomly", "rr [ loop ]"},
    { "flashchk", shell_flash_check_switch, "flash write check switch", "flashchk [on|off]"},
    { "pinfo", shell_get_partition_info, "get partition information", "pinfo [partid]"},
    { "finfo", shell_get_flash_info, "get flash information allocated for VDS", "finfo"},

    { NULL, NULL, NULL, NULL },
};

#endif // VDS_SHELL_COMMAND


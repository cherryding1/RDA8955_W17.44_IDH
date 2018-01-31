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







BYTE  Map19_Chr_Ram[ 0x2000 ];
BYTE  Map19_Regs[ 2 ];

BYTE  Map19_IRQ_Enable;
DWORD Map19_IRQ_Cnt;




void Map19_Init()
{

    MapperInit = Map19_Init;


    MapperWrite = Map19_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map19_Apu;


    MapperReadApu = Map19_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map19_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    if ( NesHeader.byVRomSize > 0 )
    {
        DWORD dwLastPage = (DWORD)NesHeader.byVRomSize << 3;
        PPUBANK[ 0 ] = VROMPAGE( dwLastPage - 8 );
        PPUBANK[ 1 ] = VROMPAGE( dwLastPage - 7 );
        PPUBANK[ 2 ] = VROMPAGE( dwLastPage - 6 );
        PPUBANK[ 3 ] = VROMPAGE( dwLastPage - 5 );
        PPUBANK[ 4 ] = VROMPAGE( dwLastPage - 4 );
        PPUBANK[ 5 ] = VROMPAGE( dwLastPage - 3 );
        PPUBANK[ 6 ] = VROMPAGE( dwLastPage - 2 );
        PPUBANK[ 7 ] = VROMPAGE( dwLastPage - 1 );
        InfoNES_SetupChr();
    }


    Map19_Regs[ 0 ] = 0x00;
    Map19_Regs[ 1 ] = 0x00;
    Map19_Regs[ 2 ] = 0x00;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map19_Write( WORD wAddr, BYTE byData )
{

    switch ( wAddr & 0xf800 )
    {
        case 0x8000:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 0 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 0 ] = Map19_VROMPAGE( 0 );
            }
            InfoNES_SetupChr();
            break;

        case 0x8800:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 1 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 1 ] = Map19_VROMPAGE( 1 );
            }
            InfoNES_SetupChr();
            break;

        case 0x9000:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 2 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 2 ] = Map19_VROMPAGE( 2 );
            }
            InfoNES_SetupChr();
            break;

        case 0x9800:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 3 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 3 ] = Map19_VROMPAGE( 3 );
            }
            InfoNES_SetupChr();
            break;

        case 0xa000:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 4 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 4 ] = Map19_VROMPAGE( 4 );
            }
            InfoNES_SetupChr();
            break;

        case 0xa800:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 5 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 5 ] = Map19_VROMPAGE( 5 );
            }
            InfoNES_SetupChr();
            break;

        case 0xb000:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 6 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 6 ] = Map19_VROMPAGE( 6 );
            }
            InfoNES_SetupChr();
            break;

        case 0xb800:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ 7 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ 7 ] = Map19_VROMPAGE( 7 );
            }
            InfoNES_SetupChr();
            break;

        case 0xc000:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ NAME_TABLE0 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ NAME_TABLE0 ] = VRAMPAGE( byData & 0x01 );
            }
            break;

        case 0xc800:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ NAME_TABLE1 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ NAME_TABLE1 ] = VRAMPAGE( byData & 0x01 );
            }
            break;

        case 0xd000:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ NAME_TABLE2 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ NAME_TABLE2 ] = VRAMPAGE( byData & 0x01 );
            }
            break;

        case 0xd800:
            if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
            {
                byData %= ( NesHeader.byVRomSize << 3 );
                PPUBANK[ NAME_TABLE3 ] = VROMPAGE( byData );
            }
            else
            {
                PPUBANK[ NAME_TABLE3 ] = VRAMPAGE( byData & 0x01 );
            }
            break;

        case 0xe000:
            byData &= 0x3f;
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            break;

        case 0xe800:
            Map19_Regs[ 0 ] = ( byData & 0x40 ) >> 6;
            Map19_Regs[ 1 ] = ( byData & 0x80 ) >> 7;

            byData &= 0x3f;
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        case 0xf000:
            byData &= 0x3f;
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK2 = ROMPAGE( byData );
            break;

        case 0xf800:
            if ( wAddr == 0xf800 )
            {
            }
            break;
    }
}




void Map19_Apu( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xf800 )
    {
        case 0x4800:
            if ( wAddr == 0x4800 )
            {
            }
            break;

        case 0x5000:
            Map19_IRQ_Cnt = ( Map19_IRQ_Cnt & 0xff00 ) | byData;
            break;

        case 0x5800:
            Map19_IRQ_Cnt = ( Map19_IRQ_Cnt & 0x00ff ) | ( (DWORD)( byData & 0x7f ) << 8 );
            Map19_IRQ_Enable = ( byData & 0x80 ) >> 7;
            break;
    }
}




BYTE Map19_ReadApu( WORD wAddr )
{
    switch ( wAddr & 0xf800 )
    {
        case 0x4800:
            if ( wAddr == 0x4800 )
            {
            }
            return (BYTE)( wAddr >> 8 );

        case 0x5000:
            return (BYTE)(Map19_IRQ_Cnt & 0x00ff );

        case 0x5800:
            return (BYTE)( (Map19_IRQ_Cnt & 0x7f00) >> 8 );

        default:
            return (BYTE)( wAddr >> 8 );
    }
}




void Map19_HSync()
{

    BYTE Map19_IRQ_Timing = 113;

    if ( Map19_IRQ_Enable )
    {
        if ( Map19_IRQ_Cnt >= (DWORD)(0x7fff - Map19_IRQ_Timing) )
        {
            Map19_IRQ_Cnt = 0x7fff;
            IRQ_REQ;
        }
        else
        {
            Map19_IRQ_Cnt += Map19_IRQ_Timing;
        }
    }
}

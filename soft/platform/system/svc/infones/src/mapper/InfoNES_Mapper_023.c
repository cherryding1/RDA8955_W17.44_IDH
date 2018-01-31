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







BYTE Map23_Regs[ 9 ];

BYTE Map23_IRQ_Enable;
BYTE Map23_IRQ_Cnt;
BYTE Map23_IRQ_Latch;




void Map23_Init()
{

    MapperInit = Map23_Init;


    MapperWrite = Map23_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    Map23_Regs[ 0 ] = 0;
    Map23_Regs[ 1 ] = 1;
    Map23_Regs[ 2 ] = 2;
    Map23_Regs[ 3 ] = 3;
    Map23_Regs[ 4 ] = 4;
    Map23_Regs[ 5 ] = 5;
    Map23_Regs[ 6 ] = 6;
    Map23_Regs[ 7 ] = 7;
    Map23_Regs[ 8 ] = 0;

    Map23_IRQ_Enable = 0;
    Map23_IRQ_Cnt = 0;
    Map23_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map23_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr )
    {
        case 0x8000:
        case 0x8004:
        case 0x8008:
        case 0x800c:
            byData %= ( NesHeader.byRomSize << 1 );

            if ( Map23_Regs[ 8 ] )
            {
                ROMBANK2 = ROMPAGE( byData );
            }
            else
            {
                ROMBANK0 = ROMPAGE( byData );
            }
            break;

        case 0x9000:
            switch ( byData & 0x03 )
            {
                case 0x00:
                    InfoNES_Mirroring( 1 );
                    break;
                case 0x01:
                    InfoNES_Mirroring( 0 );
                    break;
                case 0x02:
                    InfoNES_Mirroring( 3 );
                    break;
                case 0x03:
                    InfoNES_Mirroring( 2 );
                    break;
            }
            break;

        case 0x9008:
            Map23_Regs[ 8 ] = byData & 0x02;
            break;

        case 0xa000:
        case 0xa004:
        case 0xa008:
        case 0xa00c:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;

        case 0xb000:
            Map23_Regs[ 0 ] = ( Map23_Regs[ 0 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 0 ] = VROMPAGE( Map23_Regs[ 0 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb001:
        case 0xb004:
            Map23_Regs[ 0 ] = ( Map23_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 0 ] = VROMPAGE( Map23_Regs[ 0 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb002:
        case 0xb008:
            Map23_Regs[ 1 ] = ( Map23_Regs[ 1 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 1 ] = VROMPAGE( Map23_Regs[ 1 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb003:
        case 0xb00c:
            Map23_Regs[ 1 ] = ( Map23_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 1 ] = VROMPAGE( Map23_Regs[ 1 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc000:
            Map23_Regs[ 2 ] = ( Map23_Regs[ 2 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 2 ] = VROMPAGE( Map23_Regs[ 2 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc001:
        case 0xc004:
            Map23_Regs[ 2 ] = ( Map23_Regs[ 2 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 2 ] = VROMPAGE( Map23_Regs[ 2 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc002:
        case 0xc008:
            Map23_Regs[ 3 ] = ( Map23_Regs[ 3 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 3 ] = VROMPAGE( Map23_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc003:
        case 0xc00c:
            Map23_Regs[ 3 ] = ( Map23_Regs[ 3 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 3 ] = VROMPAGE( Map23_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd000:
            Map23_Regs[ 4 ] = ( Map23_Regs[ 4 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 4 ] = VROMPAGE( Map23_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd001:
        case 0xd004:
            Map23_Regs[ 4 ] = ( Map23_Regs[ 4 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 4 ] = VROMPAGE( Map23_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd002:
        case 0xd008:
            Map23_Regs[ 5 ] = ( Map23_Regs[ 5 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 5 ] = VROMPAGE( Map23_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd003:
        case 0xd00c:
            Map23_Regs[ 5 ] = ( Map23_Regs[ 5 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 5 ] = VROMPAGE( Map23_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe000:
            Map23_Regs[ 6 ] = ( Map23_Regs[ 6 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 6 ] = VROMPAGE( Map23_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe001:
        case 0xe004:
            Map23_Regs[ 6 ] = ( Map23_Regs[ 6 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 6 ] = VROMPAGE( Map23_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe002:
        case 0xe008:
            Map23_Regs[ 7 ] = ( Map23_Regs[ 7 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 7 ] = VROMPAGE( Map23_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe003:
        case 0xe00c:
            Map23_Regs[ 7 ] = ( Map23_Regs[ 7 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 7 ] = VROMPAGE( Map23_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xf000:
            Map23_IRQ_Latch = ( Map23_IRQ_Latch & 0xf0 ) | ( byData & 0x0f );
            break;

        case 0xf004:
            Map23_IRQ_Latch = ( Map23_IRQ_Latch & 0xf0 ) | ( ( byData & 0x0f ) << 4 );
            break;

        case 0xf008:
            Map23_IRQ_Enable = byData & 0x03;
            if ( Map23_IRQ_Enable & 0x02 )
            {
                Map23_IRQ_Cnt = Map23_IRQ_Latch;
            }
            break;

        case 0xf00c:
            if ( Map23_IRQ_Enable & 0x01 )
            {
                Map23_IRQ_Enable |= 0x02;
            }
            else
            {
                Map23_IRQ_Enable &= 0x01;
            }
            break;
    }
}




void Map23_HSync()
{

    if ( Map23_IRQ_Enable & 0x02 )
    {
        if ( Map23_IRQ_Cnt == 0xff )
        {
            IRQ_REQ;

            Map23_IRQ_Cnt = Map23_IRQ_Latch;
            if ( Map23_IRQ_Enable & 0x01 )
            {
                Map23_IRQ_Enable |= 0x02;
            }
            else
            {
                Map23_IRQ_Enable &= 0x01;
            }
        }
        else
        {
            Map23_IRQ_Cnt++;
        }
    }
}

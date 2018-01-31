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







BYTE Map21_Regs[ 10 ];
BYTE Map21_IRQ_Enable;
BYTE Map21_IRQ_Cnt;
BYTE Map21_IRQ_Latch;




void Map21_Init()
{

    MapperInit = Map21_Init;


    MapperWrite = Map21_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map21_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    for ( int nPage = 0; nPage < 8; nPage++ )
    {
        Map21_Regs[ nPage ] = nPage;
    }
    Map21_Regs[ 8 ] = 0;

    Map21_IRQ_Enable = 0;
    Map21_IRQ_Cnt = 0;
    Map21_IRQ_Latch = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map21_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xf00f )
    {

        case 0x8000:
            if ( Map21_Regs[ 8 ] & 0x02 )
            {
                byData %= ( NesHeader.byRomSize << 1 );
                ROMBANK2 = ROMPAGE( byData );
            }
            else
            {
                byData %= ( NesHeader.byRomSize << 1 );
                ROMBANK0 = ROMPAGE( byData );
            }
            break;

        case 0xa000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;


        case 0x9000:
            switch ( byData & 0x03 )
            {
                case 0:
                    InfoNES_Mirroring( 1 );
                    break;

                case 1:
                    InfoNES_Mirroring( 0 );
                    break;

                case 2:
                    InfoNES_Mirroring( 3 );
                    break;

                case 3:
                    InfoNES_Mirroring( 2 );
                    break;
            }
            break;

        case 0x9002:
            Map21_Regs[ 8 ] = byData;
            break;

        case 0xb000:
            Map21_Regs[ 0 ] = ( Map21_Regs[ 0 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 0 ] = VROMPAGE( Map21_Regs[ 0 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb002:
            Map21_Regs[ 0 ] = ( Map21_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 0 ] = VROMPAGE( Map21_Regs[ 0 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb001:
        case 0xb004:
            Map21_Regs[ 1 ] = ( Map21_Regs[ 1 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 1 ] = VROMPAGE( Map21_Regs[ 1 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xb003:
        case 0xb006:
            Map21_Regs[ 1 ] = ( Map21_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 1 ] = VROMPAGE( Map21_Regs[ 1 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc000:
            Map21_Regs[ 2 ] = ( Map21_Regs[ 2 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 2 ] = VROMPAGE( Map21_Regs[ 2 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc002:
            Map21_Regs[ 2 ] = ( Map21_Regs[ 2 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 2 ] = VROMPAGE( Map21_Regs[ 2 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc001:
        case 0xc004:
            Map21_Regs[ 3 ] = ( Map21_Regs[ 3 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 3 ] = VROMPAGE( Map21_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xc003:
        case 0xc006:
            Map21_Regs[ 3 ] = ( Map21_Regs[ 3 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 3 ] = VROMPAGE( Map21_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd000:
            Map21_Regs[ 4 ] = ( Map21_Regs[ 4 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 4 ] = VROMPAGE( Map21_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd002:
            Map21_Regs[ 4 ] = ( Map21_Regs[ 4 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 4 ] = VROMPAGE( Map21_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd001:
        case 0xd004:
            Map21_Regs[ 5 ] = ( Map21_Regs[ 5 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 5 ] = VROMPAGE( Map21_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xd003:
        case 0xd006:
            Map21_Regs[ 5 ] = ( Map21_Regs[ 5 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 5 ] = VROMPAGE( Map21_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe000:
            Map21_Regs[ 6 ] = ( Map21_Regs[ 6 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 6 ] = VROMPAGE( Map21_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe002:
            Map21_Regs[ 6 ] = ( Map21_Regs[ 6 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 6 ] = VROMPAGE( Map21_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe001:
        case 0xe004:
            Map21_Regs[ 7 ] = ( Map21_Regs[ 7 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 7 ] = VROMPAGE( Map21_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xe003:
        case 0xe006:
            Map21_Regs[ 7 ] = ( Map21_Regs[ 7 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 7 ] = VROMPAGE( Map21_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xf000:
            Map21_IRQ_Latch = ( Map21_IRQ_Latch & 0xf0 ) | ( byData & 0x0f );
            break;

        case 0xf002:
            Map21_IRQ_Latch = ( Map21_IRQ_Latch & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            break;

        case 0xf003:
            if ( Map21_IRQ_Enable & 0x01 )
            {
                Map21_IRQ_Enable |= 0x02;
            }
            else
            {
                Map21_IRQ_Enable &= 0x01;
            }
            break;

        case 0xf004:
            Map21_IRQ_Enable = byData & 0x03;
            if ( Map21_IRQ_Enable & 0x02 )
            {
                Map21_IRQ_Cnt = Map21_IRQ_Latch;
            }
            break;
    }
}




void Map21_HSync()
{

    if ( Map21_IRQ_Enable & 0x02 )
    {
        if ( Map21_IRQ_Cnt == 0xff )
        {
            Map21_IRQ_Cnt = Map21_IRQ_Latch;

            if ( Map21_IRQ_Enable & 0x01 )
            {
                Map21_IRQ_Enable |= 0x02;
            }
            else
            {
                Map21_IRQ_Enable &= 0x01;
            }
            IRQ_REQ;
        }
        else
        {
            Map21_IRQ_Cnt++;
        }
    }
}

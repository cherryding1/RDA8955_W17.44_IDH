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







BYTE Map18_Regs[11];
BYTE Map18_IRQ_Enable;
WORD Map18_IRQ_Latch;
WORD Map18_IRQ_Cnt;




void Map18_Init()
{

    MapperInit = Map18_Init;


    MapperWrite = Map18_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map18_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );


    for ( int i = 0; i < sizeof( Map18_Regs ); i++ )
    {
        Map18_Regs[ i ] = 0;
    }
    Map18_IRQ_Enable = 0;
    Map18_IRQ_Latch = 0;
    Map18_IRQ_Cnt = 0;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map18_Write( WORD wAddr, BYTE byData )
{
    switch( wAddr )
    {

        case 0x8000:
            Map18_Regs[ 0 ] = ( Map18_Regs[ 0 ] & 0xf0 ) | ( byData & 0x0f );
            ROMBANK0 = ROMPAGE( Map18_Regs[ 0 ] % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x8001:
            Map18_Regs[ 0 ] = ( Map18_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            ROMBANK0 = ROMPAGE( Map18_Regs[ 0 ] % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x8002:
            Map18_Regs[ 1 ] = ( Map18_Regs[ 1 ] & 0xf0 ) | ( byData & 0x0f );
            ROMBANK1 = ROMPAGE( Map18_Regs[ 1 ] % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x8003:
            Map18_Regs[ 1 ] = ( Map18_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            ROMBANK1 = ROMPAGE( Map18_Regs[ 1 ] % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x9000:
            Map18_Regs[ 2 ] = ( Map18_Regs[ 2 ] & 0xf0 ) | ( byData & 0x0f );
            ROMBANK2 = ROMPAGE( Map18_Regs[ 2 ] % ( NesHeader.byRomSize << 1 ) );
            break;

        case 0x9001:
            Map18_Regs[ 2 ] = ( Map18_Regs[ 2 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            ROMBANK2 = ROMPAGE( Map18_Regs[ 2 ] % ( NesHeader.byRomSize << 1 ) );
            break;


        case 0xA000:
            Map18_Regs[ 3 ]  = ( Map18_Regs[ 3 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 0 ] = VROMPAGE( Map18_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xA001:
            Map18_Regs[ 3 ] = ( Map18_Regs[ 3 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 0 ] = VROMPAGE( Map18_Regs[ 3 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xA002:
            Map18_Regs[ 4 ]  = ( Map18_Regs[ 4 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 1 ] = VROMPAGE( Map18_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xA003:
            Map18_Regs[ 4 ] = ( Map18_Regs[ 4 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 1 ] = VROMPAGE( Map18_Regs[ 4 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xB000:
            Map18_Regs[ 5 ]  = ( Map18_Regs[ 5 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 2 ] = VROMPAGE( Map18_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xB001:
            Map18_Regs[ 5 ] = ( Map18_Regs[ 5 ] &0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 2 ] = VROMPAGE( Map18_Regs[ 5 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xB002:
            Map18_Regs[ 6 ]  = ( Map18_Regs[ 6 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 3 ] = VROMPAGE( Map18_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xB003:
            Map18_Regs[ 6 ] = ( Map18_Regs[ 6 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 3 ] = VROMPAGE( Map18_Regs[ 6 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xC000:
            Map18_Regs[ 7 ]  = ( Map18_Regs[ 7 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 4 ] = VROMPAGE( Map18_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xC001:
            Map18_Regs[ 7 ] = ( Map18_Regs[ 7 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 4 ] = VROMPAGE( Map18_Regs[ 7 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xC002:
            Map18_Regs[ 8 ]  = ( Map18_Regs[ 8 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 5 ] = VROMPAGE( Map18_Regs[ 8 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xC003:
            Map18_Regs[ 8 ] = ( Map18_Regs[ 8 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 5 ] = VROMPAGE( Map18_Regs[ 8 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD000:
            Map18_Regs[ 9 ]  = ( Map18_Regs[ 9 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 6 ] = VROMPAGE( Map18_Regs[ 9 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD001:
            Map18_Regs[ 9 ] = ( Map18_Regs[ 9 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 6 ] = VROMPAGE( Map18_Regs[ 9 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD002:
            Map18_Regs[ 10 ]  = ( Map18_Regs[ 10 ] & 0xf0 ) | ( byData & 0x0f );
            PPUBANK[ 7 ] = VROMPAGE( Map18_Regs[ 10 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xD003:
            Map18_Regs[ 10 ] = ( Map18_Regs[ 10 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
            PPUBANK[ 7 ] = VROMPAGE( Map18_Regs[ 10 ] % ( NesHeader.byVRomSize << 3 ) );
            InfoNES_SetupChr();
            break;

        case 0xE000:
            Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xfff0 ) | ( byData & 0x0f );
            break;

        case 0xE001:
            Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xff0f ) | ( ( byData & 0x0f ) << 4 );
            break;

        case 0xE002:
            Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xf0ff ) | ( ( byData & 0x0f ) << 8 );
            break;

        case 0xE003:
            Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0x0fff ) | ( ( byData & 0x0f ) << 12 );
            break;

        case 0xF000:
            if ( ( byData & 0x01 ) == 0 )
            {
                Map18_IRQ_Cnt = 0;
            }
            else
            {
                Map18_IRQ_Cnt = Map18_IRQ_Latch;
            }
            break;

        case 0xF001:
            Map18_IRQ_Enable = byData & 0x01;
            break;


        case 0xF002:
            switch ( byData & 0x03 )
            {
                case 0:
                    InfoNES_Mirroring( 0 );
                    break;
                case 1:
                    InfoNES_Mirroring( 1 );
                    break;
                case 2:
                    InfoNES_Mirroring( 3 );
                    break;
            }
            break;
    }
}




void Map18_HSync()
{

    if ( Map18_IRQ_Enable )
    {
        if ( Map18_IRQ_Cnt <= 113 )
        {
            IRQ_REQ;
            Map18_IRQ_Cnt = 0;
            Map18_IRQ_Enable = 0;
        }
        else
        {
            Map18_IRQ_Cnt -= 113;
        }
    }
}

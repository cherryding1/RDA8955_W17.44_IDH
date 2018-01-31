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







BYTE Map68_Regs[4];




void Map68_Init()
{

    MapperInit = Map68_Init;


    MapperWrite = Map68_Write;


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


    for ( int i = 0; i < 4; i++ )
        Map68_Regs[ i ] = 0x00;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map68_Write( WORD wAddr, BYTE byData )
{
    switch( wAddr )
    {
        case 0x8000:

            byData %= ( NesHeader.byVRomSize << 2 );
            byData <<= 1;
            PPUBANK[ 0 ] = VROMPAGE( byData );
            PPUBANK[ 1 ] = VROMPAGE( byData + 1);
            InfoNES_SetupChr();
            break;

        case 0x9000:

            byData %= ( NesHeader.byVRomSize << 2 );
            byData <<= 1;
            PPUBANK[ 2 ] = VROMPAGE( byData );
            PPUBANK[ 3 ] = VROMPAGE( byData + 1);
            InfoNES_SetupChr();
            break;

        case 0xA000:

            byData %= ( NesHeader.byVRomSize << 2 );
            byData <<= 1;
            PPUBANK[ 4 ] = VROMPAGE( byData );
            PPUBANK[ 5 ] = VROMPAGE( byData + 1);
            InfoNES_SetupChr();
            break;

        case 0xB000:

            byData %= ( NesHeader.byVRomSize << 2 );
            byData <<= 1;
            PPUBANK[ 6 ] = VROMPAGE( byData );
            PPUBANK[ 7 ] = VROMPAGE( byData + 1);
            InfoNES_SetupChr();
            break;

        case 0xC000:
            Map68_Regs[ 2 ] = byData;
            Map68_SyncMirror();
            break;

        case 0xD000:
            Map68_Regs[ 3 ] = byData;
            Map68_SyncMirror();
            break;

        case 0xE000:
            Map68_Regs[ 0 ] = ( byData & 0x10 ) >> 4;
            Map68_Regs[ 1 ] = byData & 0x03;
            Map68_SyncMirror();
            break;

        case 0xF000:

            byData %= NesHeader.byRomSize;
            byData <<= 1;
            ROMBANK0 = ROMPAGE( byData );
            ROMBANK1 = ROMPAGE( byData + 1 );
            break;
    }
}




void Map68_SyncMirror( void )
{
    if ( Map68_Regs[ 0 ] )
    {
        switch( Map68_Regs[ 1 ] )
        {
            case 0x00:
                PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                break;

            case 0x01:
                PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                break;

            case 0x02:
                PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
                break;

            case 0x03:
                PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
                break;
        }
        InfoNES_SetupChr();
    }
    else
    {
        InfoNES_Mirroring( Map68_Regs[ 1 ] );
    }
}

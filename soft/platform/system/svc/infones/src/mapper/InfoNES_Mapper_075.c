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







BYTE Map75_Regs[ 2 ];




void Map75_Init()
{

    MapperInit = Map75_Init;


    MapperWrite = Map75_Write;


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


    Map75_Regs[ 0 ] = 0;
    Map75_Regs[ 1 ] = 1;


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map75_Write( WORD wAddr, BYTE byData )
{
    switch ( wAddr & 0xf000 )
    {

        case 0x8000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK0 = ROMPAGE( byData );
            break;

        case 0x9000:

            if ( byData & 0x01 )
            {
                InfoNES_Mirroring( 0 );
            }
            else
            {
                InfoNES_Mirroring( 1 );
            }


            Map75_Regs[ 0 ] = ( Map75_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x02 ) << 3 );
            PPUBANK[ 0 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 0 );
            PPUBANK[ 1 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 1 );
            PPUBANK[ 2 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 2 );
            PPUBANK[ 3 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 3 );

            Map75_Regs[ 1 ] = ( Map75_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x04 ) << 2 );
            PPUBANK[ 4 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 0 );
            PPUBANK[ 5 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 1 );
            PPUBANK[ 6 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 2 );
            PPUBANK[ 7 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 3 );
            InfoNES_SetupChr();
            break;


        case 0xA000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK1 = ROMPAGE( byData );
            break;


        case 0xC000:
            byData %= ( NesHeader.byRomSize << 1 );
            ROMBANK2 = ROMPAGE( byData );
            break;

        case 0xE000:

            Map75_Regs[ 0 ] = ( Map75_Regs[ 0 ] & 0x10 ) | ( byData & 0x0f );
            PPUBANK[ 0 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 0 );
            PPUBANK[ 1 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 1 );
            PPUBANK[ 2 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 2 );
            PPUBANK[ 3 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 3 );
            InfoNES_SetupChr();
            break;

        case 0xF000:

            Map75_Regs[ 1 ] = ( Map75_Regs[ 1 ] & 0x10 ) | ( byData & 0x0f );
            PPUBANK[ 4 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 0 );
            PPUBANK[ 5 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 1 );
            PPUBANK[ 6 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 2 );
            PPUBANK[ 7 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 3 );
            InfoNES_SetupChr();
            break;
    }
}

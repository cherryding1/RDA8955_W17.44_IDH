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







BYTE Map41_Regs[ 2 ];




void Map41_Init()
{

    MapperInit = Map41_Init;


    MapperWrite = Map41_Write;


    MapperSram = Map41_Sram;


    MapperApu = Map0_Apu;


    MapperReadApu = Map0_ReadApu;


    MapperVSync = Map0_VSync;


    MapperHSync = Map0_HSync;


    MapperPPU = Map0_PPU;


    MapperRenderScreen = Map0_RenderScreen;


    SRAMBANK = SRAM;


    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( int nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map41_Write( WORD wAddr, BYTE byData )
{

    if ( Map41_Regs[ 0 ] )
    {
        BYTE byChrBank;

        byChrBank = Map41_Regs[ 1 ] | ( byData & 0x0003 );
        byChrBank <<= 3;
        byChrBank %= ( NesHeader.byVRomSize << 3 );

        PPUBANK[ 0 ] = VROMPAGE( byChrBank );
        PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
        PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
        PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
        PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
        PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
        PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
        PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );

        InfoNES_SetupChr();
    }
}




void Map41_Sram( WORD wAddr, BYTE byData )
{
    BYTE byBank;

    if ( wAddr < 0x6800 )
    {
        byData = ( BYTE )( wAddr & 0xff );


        byBank = ( byData & 0x07 ) << 2;
        byBank %= ( NesHeader.byRomSize << 1 );

        ROMBANK0 = ROMPAGE( byBank );
        ROMBANK1 = ROMPAGE( byBank + 1 );
        ROMBANK2 = ROMPAGE( byBank + 2 );
        ROMBANK3 = ROMPAGE( byBank + 3 );

        Map41_Regs[ 0 ] = ( byData & 0x04 );
        Map41_Regs[ 1 ] = ( byData & 0x18 ) >> 1;


        if ( byData & 0x20 )
        {
            InfoNES_Mirroring( 0 );
        }
        else
        {
            InfoNES_Mirroring( 1 );
        }
    }
}

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










void Map79_Init()
{

    MapperInit = Map79_Init;


    MapperWrite = Map0_Write;


    MapperSram = Map0_Sram;


    MapperApu = Map79_Apu;


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




void Map79_Apu( WORD wAddr, BYTE byData )
{
    BYTE byPrgBank = ( byData & 0x08 ) >> 3;
    BYTE byChrBank = byData & 0x07;


    byPrgBank <<= 2;
    byPrgBank %= ( NesHeader.byRomSize << 1 );

    ROMBANK0 = ROMPAGE( byPrgBank + 0 );
    ROMBANK1 = ROMPAGE( byPrgBank + 1 );
    ROMBANK2 = ROMPAGE( byPrgBank + 2 );
    ROMBANK3 = ROMPAGE( byPrgBank + 3 );


    byChrBank <<= 3;
    byChrBank %= ( NesHeader.byVRomSize  << 3 );

    PPUBANK[ 0 ] = VROMPAGE( byChrBank + 0 );
    PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
    PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
    PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
    PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
    PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
    PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
    PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );
    InfoNES_SetupChr();
}

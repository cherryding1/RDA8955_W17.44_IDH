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










void Map11_Init()
{
    int nPage;


    MapperInit = Map11_Init;


    MapperWrite = Map11_Write;


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
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );


    if ( NesHeader.byVRomSize > 0 )
    {
        for ( nPage = 0; nPage < 8; ++nPage )
            PPUBANK[ nPage ] = VROMPAGE( nPage );
        InfoNES_SetupChr();
    }


    InfoNES_Mirroring( 1 );


    K6502_Set_Int_Wiring( 1, 1 );
}




void Map11_Write( WORD wAddr, BYTE byData )
{
    BYTE byPrgBank = ( byData & 0x01 ) << 2;
    BYTE byChrBank = ( ( byData & 0x70 ) >> 4 ) << 3;


    ROMBANK0 = ROMPAGE( ( byPrgBank + 0 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( ( byPrgBank + 1 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( ( byPrgBank + 2 ) % ( NesHeader.byRomSize << 1 ) );
    ROMBANK3 = ROMPAGE( ( byPrgBank + 3 ) % ( NesHeader.byRomSize << 1 ) );


    PPUBANK[ 0 ] = VROMPAGE( ( byChrBank + 0 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 1 ] = VROMPAGE( ( byChrBank + 1 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 2 ] = VROMPAGE( ( byChrBank + 2 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 3 ] = VROMPAGE( ( byChrBank + 3 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 4 ] = VROMPAGE( ( byChrBank + 4 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 5 ] = VROMPAGE( ( byChrBank + 5 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 6 ] = VROMPAGE( ( byChrBank + 6 ) % ( NesHeader.byVRomSize << 3 ) );
    PPUBANK[ 7 ] = VROMPAGE( ( byChrBank + 7 ) % ( NesHeader.byVRomSize << 3 ) );
    InfoNES_SetupChr();
}
